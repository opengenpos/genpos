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
* Title       : Print  total key (SubTotal, no finalize total, finalize total)               
* Category    : Print, NCR 2170 US Hospitarity Application 
* Program Name: PrRTotlT.C                                                      
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               PrtTotal() : prints total key (Total)
*               PrtTotal_TH() : prints total key  (thermal)
*               PrtTotalKP_TH() : prints total key ( take to kitchen )
*               PrtTotal_EJ() : prints total key  (electric journal)
*               PrtTotal_SP() : prints total key  (slip)
*               PrtTotalStub() : prints total stub receipt
*               PrtTotalOrder(): prints GC/ Order# receipt      
*               PrtTotal_SPVL() : prints total key  (validation slip)
*               PrtTotalAudact(): prints audaction total
*               PrtTotalAudact_SPVL(): prints audaction total
*
*               PrtDflTotal() : displays total key (Total)
*               PrtDflForeignTotal() : displays total key (Foreign Total)
*               PrtDflTotalForm() : displays total key (Total)
*               PrtDflForeignTotalForm() : displays total key (Foreign Total)
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-10-93 : 01.00.12 : R.Itoh     : initial                                   
* Jul-13-93 : 01.00.12 : R.Itoh     : add PrtDflTotal()                                   
* Oct-29-93 : 02.00.02 : K.You      : del. PrtDflTotal()                                   
* Apr-08-94 : 00.00.04 : K.You      : add validation slip print.(mod. PrtTotal())
* Apr-25-94 : 00.00.05 : Yoshiko.Jim: E-064 corrected (add PrtTotalAudact(),
*                                   : PrtTotalAudact_SPVL())
* Jan-26-95 : 03.00.00 : M.Ozawa    : add PrtDflTotal()                                   
* Jun-14-95 : 03.00.00 : T.Nakahata : add Total Mnemonic to Kitchen 
* Jul-24-95 : 03.00.04 : T.Nakahata : display customer name at 1st frame
* Sep-13-95 : 03.00.08 : T.Nakahata : print unique tran# at Chit Rect (FVT)
* Jun-28-96 : 03.01.09 : M.Ozawa    : add PrtDflForeignTotal()
* Dec-06-99 : 01.00.00 : hrkato     : Saratoga
* Oct-29-93 : 02.00.02 : K.You      : del. PrtDflTotal()                                   
* Dec-14-99 : 00.00.01 : M.Ozawa    : add PrtDflTotal(), PrtDflForeignTotal()
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
/**------- MS - C ---------**/
#include	<tchar.h>
#include<stdlib.h>
#include <string.h>

/**------- 2170 local------**/
#include <ecr.h>
/* #include <rfl.h> */
#include <regstrct.h>
#include <transact.h>
#include <paraequ.h>
/* #include <para.h> */
/* #include <csstbpar.h> */
#include <pmg.h>
#include <shr.h>
#include <dfl.h>
#include "prtrin.h"
#include "prtshr.h"
#include "prtdfl.h"
#include "prrcolm_.h"
#include "para.h"
#include "csstbpar.h"
#include <rfl.h>

/**
;========================================================================
;+                    S T A T I C    R A M s
;========================================================================
**/

/**
;============================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;============================================================================
**/
/*
*===========================================================================
** Format  : VOID   PrtTotal(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints total key (Total)
*===========================================================================
*/
VOID  PrtTotal(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem)
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion3(pItem->fsPrintStatus);

    if ( fsPrtStatus & PRT_REQKITCHEN ) {     /* kitchen print */
        if (( ! CliParaMDCCheck( MDC_KPTR_ADR, ODD_MDC_BIT0 )) && ( CliParaMDCCheck( MDC_KPTR_ADR, ODD_MDC_BIT3 ))) {
            /* --- if total mnemonic to kitchen option is ON,
                print it to kitchen --- */
            PrtKPTotal(pItem);
        }
    }

    if ( pItem->fsPrintStatus & PRT_VALIDATION ) { /* validation */
        if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) {
            if(PRT_SUCCESS != PrtSPVLInit())
			{
				return;
			}
			PmgBeginSmallValidation( PMG_PRT_SLIP );//change to allow Small Slips
            PrtSPVLHead(pTran);    
            PrtTotal_SP(pTran, pItem);
            PrtSPVLTrail(pTran);
			PmgEndSmallValidation( PMG_PRT_SLIP );//change to allow Small Slips
            return ;
        } 
    }

    if ( fsPrtPrintPort & PRT_SLIP ) {        /* slip print */
        PrtTotal_SP(pTran, pItem);
    }

    if ( fsPrtPrintPort & PRT_RECEIPT ) {     /* thermal print */
        if (fsPrtStatus & PRT_TAKETOKIT) {
            if (( ! CliParaMDCCheck( MDC_KPTR_ADR, ODD_MDC_BIT0 )) && ( CliParaMDCCheck( MDC_KPTR_ADR, ODD_MDC_BIT3 ))) {
                PrtTotalKP_TH(pTran, pItem);
            }
        } else {
            PrtTotal_TH(pTran, pItem);
        }
    }

    if ( fsPrtPrintPort & PRT_JOURNAL ) {     /* electric journal */
        PrtTotal_EJ(pItem);
    } else if ( fsPrtStatus & PRT_SERV_JOURNAL ) {
		SHORT   fsPortSave = fsPrtPrintPort;

        fsPrtPrintPort = PRT_JOURNAL;
        PrtTotal_EJ(pItem);
        fsPrtPrintPort = fsPortSave;
    }
}

/*
*===========================================================================
** Format  : VOID PrtTotal_TH(TRANINFORMATION *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*   Input  : ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints total key  (thermal)
*===========================================================================
*/
VOID  PrtTotal_TH(TRANINFORMATION *pTran, ITEMTOTAL *pItem)
{
    SHORT   sType;


    /* -- total amount printed double wide? -- */

    if (pItem->auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {
        sType = PRT_DOUBLE;
    } else {
        sType = PRT_SINGLE;
    }

    PrtTHHead(pTran);                         /* print header if necessary */

    PrtFeed(PMG_PRT_RECEIPT, 1);              /* 1 line feed(Receipt) */

    PrtTHNumber(pItem->aszNumber);            /* number line */

    PrtTHAmtSym(RflChkTtlAdr(pItem), pItem->lMI, sType);

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
** Synopsis: This function prints total key  when take to kithen.
*===========================================================================
*/
VOID PrtTotalKP_TH(TRANINFORMATION *pTran, ITEMTOTAL *pItem )
{
    SHORT   sType;

    if ( pItem->uchMinorClass == CLASS_TOTAL1 ) {
        /* --- sub total key is not print on kitchen printer --- */
        return;
    }

    /* -- total amount printed double wide? -- */

    if (pItem->auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {
        sType = PRT_DOUBLE;
    } else {
        sType = PRT_SINGLE;
    }

    PrtTHHead(pTran);                         /* print header if necessary */

    PrtFeed(PMG_PRT_RECEIPT, 1);              /* 1 line feed(Receipt) */

/*    PrtTHNumber(pItem->aszNumber);            / number line */

    PrtTHAmtSym(RflChkTtlAdr(pItem), pItem->lMI, sType);
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
VOID  PrtTotal_EJ(ITEMTOTAL *pItem)
{

    SHORT   sType;

    /* -- total amount printed double wide? -- */

    if (pItem->auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {
        sType = PRT_DOUBLE;
    } else {
        sType = PRT_SINGLE;
    }

    PrtEJNumber(pItem->aszNumber);            /* number line */

    if (((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT1) ||
        ((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT2)) {

        /* print cashier interrupt mnemonic on journal, R3.3 */
        PrtEJAmtSym(TRN_CASINT_ADR, pItem->lMI, sType);

    } else {

        PrtEJAmtSym(RflChkTtlAdr(pItem), pItem->lMI, sType);
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
VOID PrtTotal_SP(TRANINFORMATION *pTran, ITEMTOTAL *pItem)
{
    TCHAR   aszSPPrintBuff[2][PRT_SPCOLUMN + 1]; /* print data save area */
    USHORT  usSlipLine = 0;             /* number of lines to be printed */
    USHORT  usSaveLine;                 /* save slip lines to be added */
    USHORT  i;
    SHORT   sType;                      /* set double wide or single */

    /* initialize the buffer */
    memset(aszSPPrintBuff[0], '\0', sizeof(aszSPPrintBuff));

    /* -- total amount printed double wide? -- */
    if (pItem->auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {
        sType = PRT_DOUBLE;
    } else {
        sType = PRT_SINGLE;
    }

    /* -- set mnemonic and number -- */
    usSlipLine += PrtSPVoidNumber(aszSPPrintBuff[0], pItem->fbModifier, pItem->usReasonCode, pItem->aszNumber);

    /* -- set total line -- */   
    usSlipLine += PrtSPMnemNativeAmt(aszSPPrintBuff[usSlipLine], RflChkTtlAdr(pItem), pItem->lMI, sType);

    /* -- check if paper change is necessary or not -- */ 
    usSaveLine = PrtCheckLine(usSlipLine, pTran);

    /* -- print all data in the buffer -- */ 
    for (i = 0; i < usSlipLine; i++) {
/*  --- fix a glitch (05/15/2001)
        PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN); */
        PrtPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);       
    }

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;
}

/*
*===========================================================================
** Format  : VOID   PrtTotalStub(TRANINFORMATION *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -TRansaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints total key stub 
*===========================================================================
*/
VOID   PrtTotalStub(TRANINFORMATION *pTran, ITEMTOTAL *pItem)
{
    SHORT   sType;

    /* -- total amount printed double wide? -- */
    if (pItem->auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {
        sType = PRT_DOUBLE;
    } else {
        sType = PRT_SINGLE;
    }

    PrtTHHead(pTran);                   /* print header if necessary */

    if ( fbPrtShrStatus & PRT_SHARED_SYSTEM ) { 
        fbPrtAltrStatus |= PRT_TOTAL_STUB;
    }

    PrtTHAmtSym(RflChkTtlAdr(pItem),  pItem->lMI, sType);
}

/*
*===========================================================================
** Format  : VOID   PrtTotalOrder(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*   Input  : ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints total key Order number with transaction mnemonic.
*            Typically called as part of Service Total processing to print a stub or chit.
*
*            See MDC 234 (MDC_TRANNUM_ADR) as well as total key provisioning with P60
*            and function ItemTotalSEGetGC().
*===========================================================================
*/
VOID   PrtTotalOrder(TRANINFORMATION *pTran, ITEMTOTAL *pItem)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };  /* PARA_... defined in "paraequ.h" */

    /* -- order no(gcf#) 0, not print -- */
    if (pItem->usOrderNo == 0) {     
        return ;
    }

    PrtTHHead(pTran);                   /* print header if necessary */

    /* --- if current system uses unique transaction no.,
        print it in double width character. --- */
    if ( pTran->TranCurQual.flPrintStatus & CURQUAL_UNIQUE_TRANS_NO ) {
        PrtTHTblPerson( pTran->TranGCFQual.usTableNo, 0, PRT_DOUBLE);
    }

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, TRN_GCNO_ADR);
    if (aszTranMnem[0] != 0) {
		LONG   ulGuest;
		TCHAR CONST  aszPrtStubDouble[] = _T("%s  %s");     /* Guest Check Number line */
		TCHAR  aszGuest[8 + 1] = {0};
		TCHAR  aszGst_D[8 * 2 + 1] = {0};
		TCHAR  aszMnem_D[PARA_TRANSMNEMO_LEN * 2 + 1] = {0};  // double wide transaction mnemonic

		PrtDouble(aszMnem_D, (PARA_TRANSMNEMO_LEN * 2 + 1), aszTranMnem);

		/* -- cdv = 0, not print cdv -- */
		if (pItem->uchCdv == 0) {
			ulGuest = pItem->usOrderNo;
		} else {
			ulGuest = pItem->usOrderNo;
			ulGuest = 100 * ulGuest;
			ulGuest += pItem->uchCdv;
		}

		_ultot(ulGuest, aszGuest, 10);
		PrtDouble(aszGst_D, TCHARSIZEOF(aszGst_D), aszGuest);

		/* -- print Guest No. with double width -- */
		PrtPrintf(PMG_PRT_RECEIPT, aszPrtStubDouble, aszMnem_D, aszGst_D);

		if (pTran->TranGCFQual.usTableNo && (pTran->TranCurQual.flPrintStatus & CURQUAL_UNIQUE_TRANS_NO) == 0) {
			TCHAR  aszSpecMnem[ PARA_SPEMNEMO_LEN + 1 ] = {0};
			TCHAR  aszSpecMnem_D[ PARA_SPEMNEMO_LEN * 2 + 1 ] = {0};  // double wide special mnemonic

			RflGetSpecMnem( aszSpecMnem, SPC_TBL_ADR);
			PrtDouble(aszSpecMnem_D, TCHARSIZEOF(aszSpecMnem_D), aszSpecMnem);
			ulGuest = pTran->TranGCFQual.usTableNo;
			_ultot(ulGuest, aszGuest, 10);
			PrtDouble(aszGst_D, TCHARSIZEOF(aszGst_D), aszGuest);

			/* -- print Table No. with double width -- */
			PrtPrintf(PMG_PRT_RECEIPT, aszPrtStubDouble, aszSpecMnem_D, aszGst_D);
		}

	}
}

/*
*===========================================================================
** Format  : VOID PrtTotal_SPVL(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints total key  (validation)
*===========================================================================
*/
VOID  PrtTotal_SPVL(TRANINFORMATION *pTran, ITEMTOTAL *pItem)
{

    /* CHAR    achWork[4]; */
    TCHAR   aszSPPrintBuff[2][PRT_SPCOLUMN + 1]; /* print data save area */
    USHORT  usSlipLine = 0;             /* number of lines to be printed */
    USHORT  usSaveLine;                 /* save slip lines to be added */
    USHORT  i;
    SHORT   sType;                      /* set double wide or single */


    if(PRT_SUCCESS != PrtSPVLInit())
	{
		return;
	}
	
	PmgBeginSmallValidation( PMG_PRT_SLIP );//change to allow Small Slips

    /* initialize the buffer */
    memset(aszSPPrintBuff[0], '\0', sizeof(aszSPPrintBuff));

    /* -- total amount printed double wide? -- */
    if (pItem->auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {
        sType = PRT_DOUBLE;
    } else {
        sType = PRT_SINGLE;
    }

    /* -- set mnemonic and number -- */
    usSlipLine += PrtSPVoidNumber(aszSPPrintBuff[0], pItem->fbModifier, pItem->usReasonCode, pItem->aszNumber);
    /* -- set total line -- */   
    usSlipLine += PrtSPMnemNativeAmt(aszSPPrintBuff[usSlipLine], RflChkTtlAdr(pItem), pItem->lMI, sType);

    /* -- check if paper change is necessary or not -- */ 
    usSaveLine = PrtCheckLine(usSlipLine, pTran);

    /* -- print all data in the buffer -- */ 
    for (i = 0; i < usSlipLine; i++) {
/*  --- fix a glitch (05/15/2001)
        PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN); */
        PrtPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);
    }

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;        
    PrtSPVLTrail(pTran);
	
	PmgEndSmallValidation( PMG_PRT_SLIP );//change to allow Small Slips

}

/*
*===========================================================================
** Format  : VOID   PrtTotalAudact(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints total to audaction validation. 
*===========================================================================
*/
VOID   PrtTotalAudact(TRANINFORMATION  *pTran, ITEMTOTAL *pItem)
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( pItem->fsPrintStatus & PRT_VALIDATION ) { /* validation */
        if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) {
            if(PRT_SUCCESS != PrtSPVLInit())
			{
				return;
			}
			PmgBeginSmallValidation( PMG_PRT_SLIP );//change to allow Small Slips
            PrtSPVLHead(pTran);    
            PrtTotalAudact_SPVL(pItem);
            PrtSPVLTrail(pTran);
			PmgEndSmallValidation( PMG_PRT_SLIP );//change to allow Small Slips
        }
        return ;
    }
}
/*
*===========================================================================
** Format  : VOID  PrtTotalAudact_SPVL(ITEMTOTAL *pItem);
*
*               
*    Input : ITEMTOTAL           *pItem       -item information
*
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints total mnemonics, symbol and amount in audaction.
*===========================================================================
*/
VOID  PrtTotalAudact_SPVL(ITEMTOTAL *pItem)
{
	CONST TCHAR FARCONST  aszPrtSPVLTotal[] = _T("%s\t %s");        /* total line */
	TCHAR  aszMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };/* PARA_... defined in "paraequ.h" */
	TCHAR  aszAmountS[PRT_SPCOLUMN] = { 0 };
	TCHAR  aszAmountD[PRT_SPCOLUMN] = { 0 };
    TCHAR  *pszAmount;

	RflGetTranMnem(aszMnem, TRN_AMTTL_ADR);         /* get Amt.total mnemo. */
    PrtAdjustNative(aszAmountS, pItem->lMI);        /* get native & adjust  */

    if (pItem->auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {    /* double wide print*/
        PrtDouble(aszAmountD, (PRT_SPCOLUMN), aszAmountS);
        pszAmount = aszAmountD;
    } else {
        pszAmount = aszAmountS;
    }
/*  --- fix a glitch (05/15/2001)
    PmgPrintf(PMG_PRT_SLIP, aszPrtSPVLTotal, aszMnem, pszAmount); */
    PrtPrintf(PMG_PRT_SLIP, aszPrtSPVLTotal, aszMnem, pszAmount);
    return;
}

/*
*===========================================================================
** Format  : VOID   PrtDflTotal(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays total key (Total)
*===========================================================================
*/
VOID  PrtDflTotal(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem)
{
    TCHAR  aszDflBuff[7][PRT_DFL_LINE + 1];   /* display data save area */
    USHORT  usLineNo;                       /* number of lines to be displayed */
    USHORT  usOffset = 0;                       
    USHORT  i;                       
    SHORT sType;

    /* --- if this frame is 1st frame, display customer name --- */

    PrtDflCustHeader( pTran );

    /* -- total amount printed double wide? -- */

    if (pItem->auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {
        sType = PRT_DOUBLE;
    } else {
        sType = PRT_SINGLE;
    }

    memset(aszDflBuff, '\0', sizeof(aszDflBuff));

    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);

    /* -- set item data -- */
    usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber);  

    if (((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT1) ||
        ((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT2)) {

        /* print cashier interrupt mnemonic on journal, R3.3 */
        usLineNo += PrtDflAmtSym(aszDflBuff[usLineNo], TRN_CASINT_ADR, pItem->lMI, sType);

    } else {

        usLineNo += PrtDflAmtSym(aszDflBuff[usLineNo], RflChkTtlAdr(pItem), pItem->lMI, sType);
    }
    /* -- set destination CRT -- */
    PrtDflSetTtlNo(pTran);

    /* -- set display data in the buffer -- */ 
    PrtDflIType(usLineNo, DFL_TOTAL); 

    for ( i = 0; i < usLineNo; i++ ) {
        PrtDflSetData(aszDflBuff[i], &usOffset);
        if ( aszDflBuff[i][PRT_DFL_LINE] != '\0' ) {
            i++;
        }    
    }

    /* -- send display data -- */
    PrtDflSend();
}

/*
*===========================================================================
** Format  : VOID  PrtDflForeignTotal(TRANINFORMATION *pTran, ITEMTENDER *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTENDER       *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints foreign total operation R3.1
*===========================================================================
*/
VOID PrtDflForeignTotal(TRANINFORMATION  *pTran, ITEMTOTAL  *pItemTotal)
{
    TCHAR  aszDflBuff[11][PRT_DFL_LINE + 1];   /* display data save area */
    USHORT  usLineNo;                       /* number of lines to be displayed */
    USHORT  usOffset = 0;                       
    USHORT  i;                       
    UCHAR  uchAdr1;
	USHORT usTranAdr2;

    /* --- if this frame is 1st frame, display customer name --- */
    PrtDflCustHeader( pTran );

	PrtGetMoneyMnem (pItemTotal->uchMinorClass, &usTranAdr2, &uchAdr1);

    memset(aszDflBuff, '\0', sizeof(aszDflBuff));

    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);

    /* -- set item data -- */
    usLineNo += PrtDflForeign3(aszDflBuff[usLineNo], RflChkTtlAdr(pItemTotal), pItemTotal->lMI, uchAdr1, pItemTotal->auchTotalStatus[0]);

    /* -- set destination CRT -- */
    PrtDflIf.Dfl.DflHead.auchCRTNo[0] = 0x30;
    PrtDflIf.Dfl.DflHead.auchCRTNo[1] = 0x30;

    /* -- set display data in the buffer -- */ 
    PrtDflIType(usLineNo, DFL_TOTAL); 

    for ( i = 0; i < usLineNo; i++ ) {
        PrtDflSetData(aszDflBuff[i], &usOffset);
        if ( aszDflBuff[i][PRT_DFL_LINE] != '\0' ) {
            i++;
        }        
    }

    /* -- send display data -- */
    PrtDflSend();
}

/*
*===========================================================================
** Format  : VOID   PrtDflTotal(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays total key (Total)
*===========================================================================
*/
USHORT  PrtDflTotalForm(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem, TCHAR *puchBuffer)
{
    TCHAR  aszDflBuff[7][PRT_DFL_LINE + 1];   /* display data save area */
    USHORT  usLineNo=0, i;                       /* number of lines to be displayed */
    USHORT  usOffset = 0;                       
    SHORT sType;

    /* -- total amount printed double wide? -- */
    if (pItem->auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {
        sType = PRT_DOUBLE;
    } else {
        sType = PRT_SINGLE;
    }

    memset(aszDflBuff, '\0', sizeof(aszDflBuff));
#if 0
    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);
#endif
    /* -- set item data -- */
    usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber);  

    if (((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT1) ||
        ((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT2)) {
        /* print cashier interrupt mnemonic on journal, R3.3 */
        usLineNo += PrtDflAmtSym(aszDflBuff[usLineNo], TRN_CASINT_ADR, pItem->lMI, sType);
    } else {
        usLineNo += PrtDflAmtSym(aszDflBuff[usLineNo], RflChkTtlAdr(pItem), pItem->lMI, sType);
    }
    
    for (i=0; i<usLineNo; i++) {
        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }
    _tcsncpy(puchBuffer, aszDflBuff[0], usLineNo*(PRT_DFL_LINE+1));
    
    return usLineNo;
}

/*
*===========================================================================
** Format  : VOID  PrtDflForeignTotal(TRANINFORMATION *pTran, ITEMTENDER *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTENDER       *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints foreign total operation R3.1
*===========================================================================
*/
USHORT PrtDflForeignTotalForm(TRANINFORMATION  *pTran, ITEMTOTAL  *pItemTotal, TCHAR *puchBuffer)
{
    TCHAR  aszDflBuff[11][PRT_DFL_LINE + 1];   /* display data save area */
    USHORT  usLineNo=0, i;                       /* number of lines to be displayed */
    USHORT  usOffset = 0;                       
    UCHAR   uchAdr1;
	USHORT  usTranAdr2;

    /* --- if this frame is 1st frame, display customer name --- */
	PrtGetMoneyMnem (pItemTotal->uchMinorClass, &usTranAdr2, &uchAdr1);

    memset(aszDflBuff, '\0', sizeof(aszDflBuff));
#if 0
    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);
#endif
    /* -- set item data -- */
    usLineNo += PrtDflForeign3(aszDflBuff[usLineNo], RflChkTtlAdr(pItemTotal), pItemTotal->lMI, uchAdr1, pItemTotal->auchTotalStatus[0]);

    for (i=0; i<usLineNo; i++) {
        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }
    _tcsncpy(puchBuffer, aszDflBuff[0], usLineNo*(PRT_DFL_LINE+1));
    
    return usLineNo;
}


/***** End Of Source *****/