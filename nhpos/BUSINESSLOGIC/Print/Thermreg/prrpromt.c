/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print Promotional header                 
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRPromT.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct:
*      PrtPromotion() : print promotional header
*       PrtProm_SP()
*       PrtProm_TH()
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-11-93 : 01.00.12 :  R.Itoh    : initial                                   
* Jul-13-93 : 01.00.12 : K.Nakajima : add PrtProm_TH()
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
*===========================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===========================================================================
*/
/**
;========================================================================
;+                  I N C L U D E     F I L E s                         +
;========================================================================
**/
/**------- M S - C --------**/
#include	<tchar.h>
#include<stdlib.h>
#include<string.h>
/**------- 2170 local------**/
#include<ecr.h>
#include<regstrct.h>
#include<transact.h>
#include<paraequ.h>
#include<para.h>
#include<csstbpar.h>
#include<pmg.h>
#include"prtcom.h"
#include"prtrin.h"
#include"prrcolm_.h"
#include"shr.h"
#include"prtshr.h"
#include"prt.h"
#include"trans.h"

/**
;========================================================================
;+        P R O G R A M    D E C L A R A T I O N s                      +
;========================================================================
**/
/*
*===========================================================================
** Format  : VOID  PrtPromotion(TRANINFORMATION *pTran, ITEMPRINT *pItem);      
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*            ITEMPRINT       *pItem    -item information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This functions prints Promotional header. 
*===========================================================================
*/
VOID PrtPromotion(TRANINFORMATION  *pTran, SHORT fsPrintStatus)
{

    PrtPortion(fsPrintStatus);        /* check no-print, compulsory print */
                                      /* and write status to "fsPrtPrintPort" */

    if( (pTran->TranCurQual.fsCurStatus & CURQUAL_MULTICHECK) ) {
        return;    
    }

    if ( !(pTran->TranCurQual.flPrintStatus & CURQUAL_NOT_PRINT_PROM) && (fsPrtPrintPort & PRT_RECEIPT) ) {
        PrtProm_TH(pTran);
    }

}

/*
*===========================================================================
** Format  : VOID  PrtProm_TH(TRANINFORMATION  *pTran)
*
*   Input  : pTran                          -transaction information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints Promotional header. (thermal) 
*===========================================================================
*/
VOID  PrtProm_TH(TRANINFORMATION  *pTran)
{

	PmgStartReceipt();

	/* print header and promotional lines if necessary */
	if (PrtTHHead(pTran) > 0) {
		PARAPROMOTION  PM = {0};
		TCHAR          aszMnemD[PARA_PROMOTION_LEN * 2 + 1];
		UCHAR          i;

		PM.uchMajorClass = CLASS_PARAPROMOTION;
		for ( i = PRM_LINE1_ADR; i <= PRM_LINE3_ADR; i++) {
			PM.uchAddress = i;
			CliParaRead(&PM);
			PM.aszProSales[PARA_PROMOTION_LEN] = '\0';

			PrtDouble(aszMnemD, PARA_PROMOTION_LEN * 2 + 1, PM.aszProSales);    /* convert to double */

			if (aszMnemD[PARA_PROMOTION_LEN * 2] == PRT_DOUBLE) {
				aszMnemD[PARA_PROMOTION_LEN * 2] = PRT_SPACE;
			}

			PrtPrint((USHORT)PMG_PRT_RECEIPT, aszMnemD, (USHORT) _tcslen(aszMnemD)); /* ### Mod (2171 for Win32) V1.0 */
		}
	}

    PrtFeed(PMG_PRT_RECEIPT, 1);                /* 1 line feed */
}   
/*
*===========================================================================
** Format  : VOID  PrtTransactionBegin(VOID)
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function sends a message to a printer to inform it that
*			 a transaction is beginning, for use with double sided printing.
*===========================================================================
*/
VOID PrtTransactionBegin(SHORT sType, ULONG ulTransNo)
{
	if(CliParaMDCCheckField(MDC_DBLSIDEPRINT_ADR, MDC_PARAM_BIT_A))
	{
		TCHAR	aszFormatString[20] = {0};
		TCHAR	aszTransactionNumber[10] = {0};
		UCHAR	uchTrgPrt;
    
		//if we are in share printer mode, we will have
		//to put in a formatted string that is added to the 
		//shared printing buffer.
		/* -- check shared printer or not -- */
		if ( fbPrtAltrStatus & PRT_NEED_ALTR ) {
			uchTrgPrt = PRT_WITHALTR;
		} else {
			uchTrgPrt = PRT_WITHSHR;
		}

		if ( 0 != PrtCheckShr(uchTrgPrt) ) {
			
			fbPrtShrStatus |= PRT_SHARED_SYSTEM;
			PrtShrInit(&TrnInformation);  /* initialize shared printer buffer */
			aszFormatString[0] = PRT_TRANS_BGN;
			aszFormatString[1] = sType;

			_ultot(ulTransNo, aszTransactionNumber, 10);
			_tcscat(aszFormatString,aszTransactionNumber);

			PrtPrint(PMG_PRT_RECEIPT, aszFormatString, TCHARSIZEOF(aszFormatString));

		}else
		{
			PmgBeginTransaction(sType, ulTransNo);
		}
	}
}
/*
*===========================================================================
** Format  : VOID  PrtTransactionEnd(VOID)
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function sends a message to a printer to inform it that
*			 a transaction is ending, for use with double sided printing.
*===========================================================================
*/
VOID  PrtTransactionEnd(SHORT sType, ULONG ulTransNo)
{
	TCHAR	aszFormatString[20];
	TCHAR	aszTransactionNumber[10];

	if(CliParaMDCCheckField(MDC_DBLSIDEPRINT_ADR, MDC_PARAM_BIT_A))
	{
		if ( fbPrtShrStatus & PRT_SHARED_SYSTEM ) 
		{
			aszFormatString[0] = PRT_TRANS_END;
			aszFormatString[1] = sType;

			_ultot(ulTransNo, aszTransactionNumber, 10);
			_tcscat(aszFormatString,aszTransactionNumber);

			PrtPrint(PMG_PRT_RECEIPT, aszFormatString, TCHARSIZEOF(aszFormatString));
		}else
		{
			PmgEndTransaction(sType, ulTransNo);
		}
	}else
	{
		PrtCut();
	}
}
