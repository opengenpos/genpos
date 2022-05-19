/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print  tender key                
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRTend_.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct:
*      PrtItemTender() : print items specified class "CLASS_ITEMTENDER"
*      PrtDispTender() : display items specified class "CLASS_ITEMTENDER"
*      PrtDispTenderForm() : display items specified class "CLASS_ITEMTENDER"
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-04-92 : 00.00.01 : K.Nakajima :                                    
* Jul-14-93 : 01.00.12 : R.Itoh     : add PrtDispTender()                                   
* Oct-27-93 : 02.00.02 : K.You      : add charge posting feature.                                   
* Oct-28-93 : 02.00.02 : K.You      : del. PrtDispTender()                                   
* Apr-25-94 : 00.00.05 : K.You      : bug fixed E-25. (mod. PrtItemTender)                                   
* Jan-17-95 : 03.00.00 : M.Ozawa    : add PrtDispTender() for display on the fly
* Dec-06-99 : 01.00.00 : hrkato     : Saratoga
* Dec-14-99 : 00.00.01 : M.Ozawa    : add PrtDispTenderForm()    
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
/**------- 2170 local------**/
#include	<tchar.h>
#include <string.h>
#include <ecr.h>
#include <pif.h>
#include <regstrct.h>
#include <transact.h>
#include <paraequ.h>
/* #include <para.h> */
/* #include <pmg.h> */
#include <eept.h>
#include "prtrin.h"


/**
;============================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;============================================================================
**/
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
VOID   PrtItemTender(TRANINFORMATION *pTran, ITEMTENDER *pItem)
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
        PrtTender(pTran, pItem);
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
VOID    PrtDispTender(TRANINFORMATION  *pTran, ITEMTENDER  *pItem)
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
/***** End Of Source File *****/