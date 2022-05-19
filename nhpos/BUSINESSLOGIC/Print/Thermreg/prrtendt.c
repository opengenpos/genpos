/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print  tender key                
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRTendT.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:
*      PrtItemTender() : print items specified class "CLASS_ITEMTENDER"
*      PrtDispTender() : display items specified class "CLASS_ITEMTENDER"
*      PrtDispTenderForm() : display items specified class "CLASS_ITEMTENDER"
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-11-93 : 01.00.12 : R.Itoh     : initial                                   
* Jul-14-93 : 01.00.12 : R.Itoh     : add PrtDispTender()                                   
* Oct-21-93 : 02.00.02 : K.You      : add charge posting feature.                                   
* Oct-29-93 : 02.00.02 : K.You      : del. PrtDispTender()                                   
* Apr-25-94 : 00.00.05 : K.You      : bug fixed E-25 (mod. PrtItemTender)
* Jan-26-93 : 03.00.00 : M.Ozawa    : add PrtDispTender()    
* Dec-06-99 : 01.00.00 : hrkato     : Saratoga
* Dec-14-99 : 00.00.01 : M.Ozawa    : add PrtDispTenderForm()    
* Mar-30-15 : 02.02.01 : R.Chambers : localize variables    
*============================================================================
*============================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*============================================================================
*/

/*
=============================================================================
+                        I N C L U D E   F I L E s
=============================================================================
*/
#include <tchar.h>
#include <string.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <regstrct.h>
#include <transact.h>
#include <paraequ.h>
/* #include <para.h> */
#include "prtrin.h"
#include    "rfl.h"
#include    "pif.h"
#include    "item.h"


/**
;============================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;============================================================================
**/

/*
*===========================================================================
** Format  : VOID  CheckGiftCardPrint();      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTENDER       *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints Gift Card Info
*===========================================================================
*/
VOID	CheckGiftCardPrt(ITEMTENDER  *pItem)
{
	if(pItem->lBalanceDue == 0L){
		TRANGCFQUAL    *pWorkGCF = TrnGetGCFQualPtr();

		if(pWorkGCF->TrnGiftCard[0].ItemTender.lTenderAmount != 0){
			TRANINFORMATION  TranInfo = {0};
			SHORT index;
			for(index = 0; index < 10;index++){
				if(pWorkGCF->TrnGiftCard[index].GiftCard != 0){
					ITEMTENDER ItemTender = pWorkGCF->TrnGiftCard[index].ItemTender;
					RflDecryptByteString((UCHAR *)&(ItemTender.aszNumber[0]), sizeof(ItemTender.aszNumber));
					
					if ( fsPrtPrintPort & PRT_SLIP ) {              /* slip print           */
						PrtTender_SP(&TranInfo, pItem, 0);          /* normal slip          */
					}
					if ( fsPrtPrintPort & PRT_RECEIPT ) {     /* thermal print */
						PrtTender_TH(&TranInfo,&ItemTender);
					}
					if ( fsPrtPrintPort & PRT_JOURNAL ) {     /* electric journal */
						PrtTender_EJ(&ItemTender);
					}
	/*				if(! CliParaMDCCheck(MDC_CPPARA2_ADR, EVEN_MDC_BIT0)){
						memset(&pWorkGCF->TrnGiftCard[index].ItemTender,0,sizeof(ITEMTENDER));
						memset(&ItemTender,0,sizeof(ITEMTENDER));
					}
	*/
					memset (&ItemTender, 0xcc, sizeof(ItemTender));    // overwrite PAN account data per PCI-DSS
				}
			}
		}
	}
}

/*
*===========================================================================
** Format  : VOID  PrtItemTender(TRANINFORMATION *pTran, ITEMTENDER *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTENDER       *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints tender operation
*===========================================================================
*/
VOID   PrtItemTender(TRANINFORMATION  *pTran, ITEMTENDER  *pItem)
{
    if (pItem->uchMajorClass == CLASS_ITEMTENDEREC) {
        PrtErrCorr(pItem);
        return;
    }

    switch (pItem->uchMinorClass) {
    case CLASS_TENDER1:                 /* tender */
    case CLASS_TENDER2:
    case CLASS_TENDER3:
    case CLASS_TENDER4:
    case CLASS_TENDER5:
    case CLASS_TENDER6:
    case CLASS_TENDER7:
    case CLASS_TENDER8:
    case CLASS_TENDER9:
    case CLASS_TENDER10:
    case CLASS_TENDER11:
    case CLASS_TENDER12:
    case CLASS_TENDER13:
    case CLASS_TENDER14:
    case CLASS_TENDER15:
    case CLASS_TENDER16:
    case CLASS_TENDER17:
    case CLASS_TENDER18:
    case CLASS_TENDER19:     /* Saratoga */
    case CLASS_TENDER20:
    case CLASS_TEND_FSCHANGE:           /* Saratoga */
    case CLASS_TEND_TIPS_RETURN:           /* Saratoga */
        PrtTender(pTran, pItem);
		CheckGiftCardPrt(pItem);
        break;

    case CLASS_FOREIGN1:                /* foreign tender */
    case CLASS_FOREIGN2:
    case CLASS_FOREIGN3:
    case CLASS_FOREIGN4:
    case CLASS_FOREIGN5:
    case CLASS_FOREIGN6:
    case CLASS_FOREIGN7:
    case CLASS_FOREIGN8:
        PrtForeignTender(pTran, pItem);
        break;

    case CLASS_EUROTENDER:            /* Euro tender */
        PrtEuroTender(pTran, pItem);
        break;

    default:
        break;
    }
}



/*
*===========================================================================
** Format  : VOID  PrtDispTender(TRANINFORMATION *pTran, ITEMTENDER *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTENDER       *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints tender operation
*===========================================================================
*/
VOID   PrtDispTender(TRANINFORMATION  *pTran, ITEMTENDER  *pItem)
{
    if (pItem->fbStorageStatus & NOT_SEND_KDS) {
        /* not send kds to avoid duplicated send R3.1 */
        return;
    }

    switch (pItem->uchMinorClass) {
    case CLASS_TENDER1:                 /* tender */
    case CLASS_TENDER2:
    case CLASS_TENDER3:
    case CLASS_TENDER4:
    case CLASS_TENDER5:
    case CLASS_TENDER6:
    case CLASS_TENDER7:
    case CLASS_TENDER8:
    case CLASS_TENDER9:
    case CLASS_TENDER10:
    case CLASS_TENDER11:
    case CLASS_TENDER12:
    case CLASS_TENDER13:
    case CLASS_TENDER14:
    case CLASS_TENDER15:
    case CLASS_TENDER16:
    case CLASS_TENDER17:
    case CLASS_TENDER18:
    case CLASS_TENDER19:     /* Saratoga */
    case CLASS_TENDER20:
    case CLASS_TEND_FSCHANGE:           /* Saratoga */
    case CLASS_TEND_TIPS_RETURN:           /* Saratoga */
        PrtDflTender(pTran, pItem);
        break;

    case CLASS_FOREIGN1:                /* foreign tender */
    case CLASS_FOREIGN2:
    case CLASS_FOREIGN3:
    case CLASS_FOREIGN4:
    case CLASS_FOREIGN5:
    case CLASS_FOREIGN6:
    case CLASS_FOREIGN7:
    case CLASS_FOREIGN8:
        PrtDflForeignTender(pTran, pItem);
        break;

    default:
        break;
    }
}

/*
*===========================================================================
** Format  : VOID  PrtDispTender(TRANINFORMATION *pTran, ITEMTENDER *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTENDER       *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints tender operation
*===========================================================================
*/
USHORT   PrtDispTenderForm(TRANINFORMATION  *pTran, ITEMTENDER  *pItem, TCHAR *puchBuffer)
{
    USHORT usLine;
    
    switch (pItem->uchMinorClass) {
    case CLASS_TENDER1:                 /* tender */
    case CLASS_TENDER2:
    case CLASS_TENDER3:
    case CLASS_TENDER4:
    case CLASS_TENDER5:
    case CLASS_TENDER6:
    case CLASS_TENDER7:
    case CLASS_TENDER8:
    case CLASS_TENDER9:
    case CLASS_TENDER10:
    case CLASS_TENDER11:
    case CLASS_TENDER12:
    case CLASS_TENDER13:
    case CLASS_TENDER14:
    case CLASS_TENDER15:
    case CLASS_TENDER16:
    case CLASS_TENDER17:
    case CLASS_TENDER18:
    case CLASS_TENDER19:     /* Saratoga */
    case CLASS_TENDER20:
    case CLASS_TEND_FSCHANGE:           /* Saratoga */
    case CLASS_TEND_TIPS_RETURN:           /* Saratoga */
        usLine = PrtDflTenderForm(pTran, pItem, puchBuffer);
        break;

    case CLASS_FOREIGN1:                /* foreign tender */
    case CLASS_FOREIGN2:
    case CLASS_FOREIGN3:
    case CLASS_FOREIGN4:
    case CLASS_FOREIGN5:
    case CLASS_FOREIGN6:
    case CLASS_FOREIGN7:
    case CLASS_FOREIGN8:
        usLine = PrtDflForeignTenderForm(pTran, pItem, puchBuffer);
        break;

    default:
        usLine = 0;
        break;
    }
    
    return usLine;
}

/***** End Of Source *****/