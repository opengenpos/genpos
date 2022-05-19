/*
*---------------------------------------------------------------------------
*  Georgia Southern University, Rsearch Services and Sponsored Programs
*    (C) Copyright 2002 - 2020
*
*  NHPOS, donated by NCR Corp to Georgia Southern University, August, 2002.
*  Developemnt with NCR 7448 then ported to Windows XP and generic x86 hardware
*  along with touch screen support.
*
*---------------------------------------------------------------------------
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print  Order Dec key (Eat-In, Dine-In, Drive Thru, etc.)               
* Program Name: PrROrdDecT.C      
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
*
** GenPOS **
* Dec-13-18 : 02.02.02 : R.Chambers : fix for kitchen printing Order Declare
*/

/*
=============================================================================
+                        I N C L U D E   F I L E s
=============================================================================
*/
/**------- MS - C ---------**/
#include	<tchar.h>
#include<stdlib.h>
#include <string.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <rfl.h>
#include <regstrct.h>
#include <transact.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <pmg.h>
#include <shr.h>
#include <dfl.h>
#include "prtrin.h"
#include "prtshr.h"
#include "prtdfl.h"
#include "prrcolm_.h"
#include <Trans.h>
/**
;============================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;============================================================================
**/
/*
*===========================================================================
** Format  : VOID    PrtPLU(TRANINFORMATION  *pTran, ITEMSALES *pItem);      
*               
*    Input : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMSALES        *pItem     -Item Data address
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints Order Declare text on the kitchen printer.
*===========================================================================
*/
VOID PrtOrderDec(TRANINFORMATION  *pTran, ITEMSALES  *pItem)
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    if ((pTran->TranCurQual.auchTotalStatus[4] & CURQUAL_TOTAL_NOCONSOLIDATE_PRINT) ||    /* not consolidation */
        ((pTran->TranCurQual.flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_UNBUFFER) ||        /* unbuffering print */
        (pItem->fsPrintStatus & (PRT_SINGLE_RECPT | PRT_DOUBLE_RECPT))) {  /* tichet */

        PrtPortion(pItem->fsPrintStatus);
    } else {                                                    /* consolidation */
        PrtPortion2(pItem);
    }

    if ( (fsPrtStatus & PRT_REQKITCHEN) && !(fsPrtStatus & PRT_TAKETOKIT) ) 
	{     /* kitchen print */
            /* --- if total mnemonic to kitchen option is ON,
                print it to kitchen --- */
			if(pTran->TranCurQual.fsCurStatus2 & CURQUAL_ORDERDEC_KPRINTED)
			{
				return;
			}else
			{
				pTran->TranCurQual.fsCurStatus2 |= CURQUAL_ORDERDEC_KPRINTED;
			}
			PrtKPOrderDec(pTran, pItem);
	}
    
    if ( pItem->fsPrintStatus & PRT_VALIDATION ) { /* validation print */
        if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) {
            if(PRT_SUCCESS != PrtSPVLInit())
			{
				return;
			}
			PmgBeginSmallValidation( PMG_PRT_SLIP );//change to allow Small Slips
            PrtSPVLHead(pTran);                     /* E-061 corr. '94 4/22 */
            PrtOrderDec_SP(pTran, pItem);
            PrtSPVLTrail(pTran);
			PmgEndSmallValidation( PMG_PRT_SLIP );//change to allow Small Slips
            return;
        }
    }

    if ( fsPrtPrintPort & PRT_SLIP ) {        /* slip print */
        PrtOrderDec_SP(pTran, pItem);
    }

    if ( (fsPrtPrintPort & PRT_RECEIPT) || (fsPrtStatus & PRT_TAKETOKIT) ) {     /* thermal print */
		if(pTran->TranCurQual.fsCurStatus2 & CURQUAL_ORDERDEC_PRINTED)
		{
			return;
		}else
		{
			pTran->TranCurQual.fsCurStatus2 |= CURQUAL_ORDERDEC_PRINTED;
		}
        PrtOrderDec_TH(pTran, pItem);
    }

    if ( fsPrtPrintPort & PRT_JOURNAL ) {     /* electric journal */
        PrtOrderDec_EJ(pItem);
    }

}
/*
*===========================================================================
** Format  : VOID PrtOrderDec_TH(TRANINFORMATION *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*   Input  : ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints Order Declare text  (thermal)
*===========================================================================
*/
VOID  PrtOrderDec_TH(TRANINFORMATION *pTran, ITEMSALES *pItem)
{
	USHORT usTrnAdr = TRN_ORDER_DEC_OFFSET;

	// the order declare number of CLASS_ITEMORDERDEC is stored in the uchAdjective value.
	usTrnAdr += pItem->uchAdjective;

    PrtTHHead(pTran);                         /* print header if necessary */

    PrtFeed(PMG_PRT_RECEIPT, 1);              /* 1 line feed(Receipt) */

    PrtTHOrderDec(usTrnAdr);
	if (pItem->aszNumber[0][0]) {
		// if there is #/Type text with this Order Declare then print it.
		// used with AGR # printing for Amtrak
		TCHAR  aszTranMnem[(PARA_CHAR24_LEN) + 1];
//		TCHAR  aszTranMnemD[(PARA_CHAR24_LEN)* 2 + 1];

		memset (aszTranMnem, 0, sizeof(aszTranMnem));
		_tcsncpy (aszTranMnem, pItem->aszNumber[0], PARA_CHAR24_LEN);
		/* -- convert to double -- */
//		PrtDouble(aszTranMnemD, sizeof(aszTranMnem), aszTranMnem);

 		PrtPrintf(PMG_PRT_RECEIPT, _T("%20s"), aszTranMnem, _T(" "));
	}
}

/*
*===========================================================================
** Format  : VOID PrtTotalKP_TH(TRANINFORMATION *pTran, ITEMTOTAL *pItem )
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints Order Declare text on kitchen printer
*===========================================================================
*/
VOID PrtKPOrderDec(TRANINFORMATION *pTran, ITEMSALES *pItem )
{
	if (pItem->uchMinorClass == CLASS_ITEMORDERDEC && pItem->uchAdjective) {
		TCHAR   aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };  /* NUM_... defined in "regstrct.h" */
		TCHAR   aszItem[PRT_KPCOLUMN + 1] = {0};
		USHORT  usStrLen = 0;
 		USHORT  usTrnAdr = TRN_ORDER_DEC_OFFSET;
		UCHAR	fbPrtStatus = 0;
		USHORT  i;

		// the order declare number of CLASS_ITEMORDERDEC is stored in the uchAdjective value.
		usTrnAdr += pItem->uchAdjective;

		RflGetTranMnem( aszTranMnem, usTrnAdr );    /* seat no. mnemonic */
        RflSPrintf(aszItem, PRT_KPCOLUMN, L"%-18s", aszTranMnem);
        usStrLen = tcharlen(aszItem);
		aszItem[usStrLen] = PRT_RETURN;   /* write return */

		for (i = 0; i < MAX_DEST_SIZE; i++) {
			PrtChkKPStorageEx(aszItem, usStrLen + 1, fbPrtStatus, 0, i);
		}
    }
}

/*
*===========================================================================
** Format  : VOID PrtTotal_EJ(ITEMTOTAL *pItem);      
*
*   Input  : ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints total key  (elctric journal)
*===========================================================================
*/
VOID  PrtOrderDec_EJ(ITEMSALES *pItem)
{
 	USHORT usTrnAdr = TRN_ORDER_DEC_OFFSET;

	// the order declare number of CLASS_ITEMORDERDEC is stored in the uchAdjective value.
	usTrnAdr += pItem->uchAdjective;

    PrtEJOrderDec(usTrnAdr, 0, PRT_DOUBLE);
	if (pItem->aszNumber[0][0]) {
		// if there is #/Type text with this Order Declare then print it.
		// used with AGR # printing for Amtrak
		TCHAR  aszTranMnem[(PARA_CHAR24_LEN) + 1];
//		TCHAR  aszTranMnemD[(PARA_CHAR24_LEN)* 2 + 1];

		memset (aszTranMnem, 0, sizeof(aszTranMnem));
		_tcsncpy (aszTranMnem, pItem->aszNumber[0], PARA_CHAR24_LEN);
		/* -- convert to double -- */
//		PrtDouble(aszTranMnemD, sizeof(aszTranMnem), aszTranMnem);

 		PrtPrintf(PMG_PRT_JOURNAL, _T("%20s"), aszTranMnem, _T(" "));
	}
}

/*
*===========================================================================
** Format  : VOID PrtTotal_SP(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints total key  (slip)
*===========================================================================
*/
VOID PrtOrderDec_SP(TRANINFORMATION *pTran, ITEMSALES *pItem)
{
	TCHAR   aszSPPrintBuff[2][PRT_SPCOLUMN + 1] = {0}; /* print data save area */
    USHORT  usSlipLine = 0;             /* number of lines to be printed */
    USHORT  usSaveLine;                 /* save slip lines to be added */
    USHORT  i;
 	USHORT usTrnAdr = TRN_ORDER_DEC_OFFSET;

	// the order declare number of CLASS_ITEMORDERDEC is stored in the uchAdjective value.
	usTrnAdr += pItem->uchAdjective;

    /* -- check if paper change is necessary or not -- */ 
    usSaveLine = PrtCheckLine(usSlipLine, pTran);

    /* -- print all data in the buffer -- */ 
    for (i = 0; i < usSlipLine; i++) {
        PrtPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);       
    }

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;
}
/*
*===========================================================================
** Format  : VOID PrtTotal_SP(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints total key  (slip)
*===========================================================================
*/
SHORT PrtOrderDecSearch(TRANINFORMATION *pTran, ITEMSALES *pItem)
{
    SHORT   sType;
    TRANCURQUAL     WorkCur;

    TrnGetCurQual(&WorkCur);

    /*--- System is Postcheck or Store/Recall ---*/
    switch (WorkCur.flPrintStatus & CURQUAL_GC_SYS_MASK) {
    case CURQUAL_STORE_RECALL:
        sType = TRN_TYPE_CONSSTORAGE;
        break;
    case CURQUAL_POSTCHECK:
        if ((WorkCur.fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_CASHIER) {
            sType = TRN_TYPE_ITEMSTORAGE;
        } else {
            sType = TRN_TYPE_CONSSTORAGE;
        }
        break;
    case CURQUAL_PRE_BUFFER:
    case CURQUAL_PRE_UNBUFFER:
    default:
        sType = TRN_TYPE_ITEMSTORAGE;
        break;
    }

	return TrnStoOrderDecSearch(pItem,sType);

}
