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
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print  total key (SubTotal, no finalize total, finalize total)               
* Category    : Print, NCR 2170 US Hospitarity Application 
* Program Name: PrRTotl_.C                                                      
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               PrtTotal() : prints total key (Total)
*               PrtTotal_RJ() : prints total key  (receipt & journal)
*               PrtTotalKP_RJ() : prints total key ( take to kitchen )
*               PrtTotal_VL() : prints total key  (validation)
*               PrtTotal_SP() : prints total key  (slip)
*               PrtTotalStub()
*               PrtTotalOrder()
*               PrtTotalAudact()
*               PrtTotalAudact_VL()
*               PrtTotal_SPVL() :  prints total key  (validation slip)
*               PrtDflTotal() : displays total key (Total)
*               PrtDflForeignTotal() : displays total key (Foreign Total)
*               PrtDflTotalForm() : displays total key (Total)
*               PrtDflForeignTotalForm() : displays total key (Foreign Total)
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* May-21-92 : 00.00.01 : K.Nakajima :                                    
* Jul-13-93 : 01.00.12 : R.Itoh     : add PrtDflTotal()                                   
* Oct-28-93 : 02.00.02 : K.You      : del. PrtDflTotal()                                   
* Apr-08-94 : 00.00.04 : K.You      : add validation slip print.(mod. PrtTotal())
* Apr-25-94 : 00.00.05 : Yoshiko.Jim: E-064 corr. (mod. PrtTotal())
* Jan-17-95 : 03.00.00 : M.Ozawa    : add PrtDflTotal() for display on the fly
* Jun-14-95 : 03.00.00 : T.Nakahata : add Total Mnemonic to Kitchen 
* Jul-24-95 : 03.00.04 : T.Nakahata : display customer name at 1st frame
* Sep-13-95 : 03.00.08 : T.Nakahata : print unique tran# at Chit Rect (FVT)
* Jun-28-96 : 03.01.09 : M.Ozawa    : add PrtDflForeignTotal()
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
#include <string.h>
#include <stdlib.h>

/**------- 2170 local------**/
#include <ecr.h>
/* #include <rfl.h> */
#include <regstrct.h>
#include <transact.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <dfl.h>
#include <pmg.h>
#include "prtrin.h"
#include "prtdfl.h"
#include "prrcolm_.h"
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
    SHORT   fsPortSave;

    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion3(pItem->fsPrintStatus);

    if ( fsPrtStatus & PRT_REQKITCHEN ) {     /* kitchen print */
        if (( ! CliParaMDCCheck( MDC_KPTR_ADR, ODD_MDC_BIT0 )) &&
            ( CliParaMDCCheck( MDC_KPTR_ADR, ODD_MDC_BIT3 ))) {
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
        } else {
            PrtTotal_VL(pTran, pItem);
        }
        return ;
    }

    if ( fsPrtPrintPort & PRT_SLIP ) {        /* slip print */
        PrtTotal_SP(pTran, pItem);
    }

    if (   (fsPrtPrintPort & PRT_RECEIPT)     /* receipt, journal print */
        || (fsPrtPrintPort & PRT_JOURNAL)) {

        if (fsPrtStatus & PRT_TAKETOKIT) {

            if (( ! CliParaMDCCheck( MDC_KPTR_ADR, ODD_MDC_BIT0 )) &&
                ( CliParaMDCCheck( MDC_KPTR_ADR, ODD_MDC_BIT3 ))) {
                PrtTotalKP_RJ(pItem);
            }

        } else {

            PrtTotal_RJ(pItem);
        }
    }

    if (( ! ( fsPrtPrintPort & PRT_JOURNAL )) &&    /* for split print R3.1 */
        ( fsPrtStatus & PRT_SERV_JOURNAL )) {
        fsPortSave = fsPrtPrintPort;
        fsPrtPrintPort = PRT_JOURNAL;

        PrtTotal_RJ(pItem);

        fsPrtPrintPort = fsPortSave;
    }
}

/*
*===========================================================================
** Format  : VOID PrtTotal_RJ(ITEMTOTAL *pItem);      
*
*   Input  : ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints total key  (receipt & journal)
*===========================================================================
*/
VOID  PrtTotal_RJ(ITEMTOTAL *pItem)
{

    SHORT   sType;


    /* -- total amount printed double wide? -- */

    if (pItem->auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {
        sType = PRT_DOUBLE;
    } else {
        sType = PRT_SINGLE;
    }

    if (pItem->uchMinorClass == CLASS_PREAUTHTOTAL) {
        sType = PRT_SINGLE;
    }

    PrtRFeed(1);                              /* 1 line feed(Receipt) */

    PrtRJNumber(pItem->aszNumber);            /* number line */

    if (((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT1) ||
        ((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT2)) {

        /* print cashier interrupt mnemonic on journal, R3.3 */
        if (fsPrtPrintPort & PRT_RECEIPT) {
            PrtRJAmtSymP(RflChkTtlAdr(pItem), pItem->lMI, sType, PMG_PRT_RECEIPT);
        }
        if (fsPrtPrintPort & PRT_JOURNAL) {
            PrtRJAmtSymP(TRN_CASINT_ADR, pItem->lMI, sType, PMG_PRT_JOURNAL);
        }
    } else {

        PrtRJAmtSym(RflChkTtlAdr(pItem), pItem->lMI, sType);
    }
}

/*
*===========================================================================
** Format  : VOID PrtTotalKP_RJ( ITEMTOTAL *pItem )
*
*   Input  : ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints total key  when take to kithen.
*===========================================================================
*/
VOID PrtTotalKP_RJ( ITEMTOTAL *pItem )
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

    if (pItem->uchMinorClass == CLASS_PREAUTHTOTAL) {
        sType = PRT_SINGLE;
    }

    PrtRFeed(1);                              /* 1 line feed(Receipt) */

/*    PrtRJNumber(pItem->aszNumber);            / number line */

    PrtRJAmtSym(RflChkTtlAdr(pItem), pItem->lMI, sType);
}

/*
*===========================================================================
** Format  : VOID PrtTotal_VL(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
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
VOID  PrtTotal_VL(TRANINFORMATION *pTran, ITEMTOTAL *pItem)
{

    PmgBeginValidation(PMG_PRT_RCT_JNL);        /* begin validation */

    /* -- in case of 24 char printer -- */

    if ( usPrtVLColumn == PRT_VL_24 ) {

        PrtVLHead(pTran, 0);                    /* send header lines */
                                                /* not print "VOID"  */

        PrtVLNumber(pItem->aszNumber);          /* send number line */

        PrtVLTotal(pItem, pItem->lMI, RflChkTtlAdr(pItem));
                                                /* send amount line */
                                                
        PrtVLTrail(pTran);                      /* send trailer lines */

    }

    /* -- in case of 21 char printer -- */

    if ( usPrtVLColumn == PRT_VL_21 ) {

        PrtVSTotal(pItem, pTran);

    }
    
    PrtEndValidation(PMG_PRT_RCT_JNL);          /* end validation */
    
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

    if (pItem->uchMinorClass == CLASS_PREAUTHTOTAL) {   /* pre-autho. */
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
        PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);       
    }

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;

}

/*
*===========================================================================
** Format  : VOID   PrtTotalStub(ITEMTOTAL *pItem);      
*
*   Input  : ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints total key stub 
*===========================================================================
*/
VOID   PrtTotalStub(ITEMTOTAL *pItem)
{
    SHORT   sType;

    /* -- total amount printed double wide? -- */
    if (pItem->auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {
        sType = PRT_DOUBLE;
    } else {
        sType = PRT_SINGLE;
    }

    PrtRJAmtSymP(RflChkTtlAdr(pItem),  pItem->lMI, sType, PMG_PRT_RECEIPT);
}

/*
*===========================================================================
** Format  : VOID   PrtTotalOrder(TRANINFORMATION  *pTran, ITEMTOTAL *pItem)
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints total key Order 
*===========================================================================
*/
VOID   PrtTotalOrder(TRANINFORMATION  *pTran, ITEMTOTAL *pItem)
{
	CONST TCHAR FARCONST  aszPrtTransNo[] = _T("%-12s %-8s");   /* unique trans# */
	CONST TCHAR FARCONST  aszPrtOrder[] = _T("%-8.8s %lu");           /* total order */ /*8 characters JHHJ*/
	LONG   ulGuest;
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };  /* PARA_... defined in "paraequ.h" */
	TCHAR   aszLeadThru[PARA_LEADTHRU_LEN + 1] = { 0 };
	TCHAR   aszTblNo[PRT_ID_LEN + 1] = { 0 };
	TCHAR   aszTransNo[PRT_ID_LEN * 2 + 1] = { 0 };

    /* -- order no(gcf#) 0, not print -- */
    if (pItem->usOrderNo == 0) {     
        return ;
    }

    /* --- if current system uses unique transaction no.,
        print it in double width character. --- */
    if ( pTran->TranCurQual.flPrintStatus & CURQUAL_UNIQUE_TRANS_NO ) {
        _itot( pTran->TranGCFQual.usTableNo, aszTblNo, 10 );
        PrtDouble( aszTransNo, TCHARSIZEOF(aszTransNo), aszTblNo );

        PrtGetLead( aszLeadThru, LDT_UNIQUE_NO_ADR );

        PmgPrintf( PMG_PRT_RECEIPT, aszPrtTransNo, aszLeadThru, aszTransNo );
    }

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, TRN_GCNO_ADR);

    /* -- cdv = 0, not print cdv -- */
    if (pItem->uchCdv == 0) {
        ulGuest = pItem->usOrderNo;
    } else {
        ulGuest = pItem->usOrderNo;
        ulGuest = 100 * ulGuest;
        ulGuest += pItem->uchCdv;
    }

    PmgPrintf(PMG_PRT_RECEIPT, aszPrtOrder, aszTranMnem, ulGuest);
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

    if ( pItem->fsPrintStatus & PRT_VALIDATION ) {           /* validation       */
        if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) {
                                                        /* E-064 corr. 4/25 */
            if(PRT_SUCCESS != PrtSPVLInit())
			{
				return;
			}
			PmgBeginSmallValidation( PMG_PRT_SLIP );//change to allow Small Slips
            PrtSPVLHead(pTran);    
            PrtTotalAudact_SPVL(pItem);
            PrtSPVLTrail(pTran);
			PmgEndSmallValidation( PMG_PRT_SLIP );//change to allow Small Slips
        } else {
            PrtTotalAudact_VL(pTran, pItem);            /* normal validation*/
        }
    }
    return ;
}

/*
*===========================================================================
** Format  : VOID PrtTotalAudact_VL(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
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
VOID  PrtTotalAudact_VL(TRANINFORMATION *pTran, ITEMTOTAL *pItem)
{
    PmgBeginValidation(PMG_PRT_RCT_JNL);        /* begin validation */

    /* -- in case of 24 char printer -- */
    if ( usPrtVLColumn == PRT_VL_24 ) {
        PrtVLHead(pTran, 0);                    /* send header lines */
                                                /* not print "VOID"  */
        PrtVLNumber(pItem->aszNumber);          /* send number line */
        PrtVLTotal(pItem, pItem->lMI, TRN_AMTTL_ADR);  /* amount line */
        PrtVLTrail(pTran);                      /* send trailer lines */
    }

    /* -- in case of 21 char printer -- */
    if ( usPrtVLColumn == PRT_VL_21 ) {
        PrtVSTotalAudact(pItem, pTran);
    }
    
    PrtEndValidation(PMG_PRT_RCT_JNL);          /* end validation */
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
	TCHAR  aszAmountS[PRT_SPCOLUMN + 1] = { 0 };
	TCHAR  aszAmountD[PRT_SPCOLUMN * 2 + 1] = { 0 };
    TCHAR  *pszAmount;

	RflGetTranMnem(aszMnem, TRN_AMTTL_ADR);         /* get Amt.total mnemo. */
    PrtAdjustNative(aszAmountS, pItem->lMI);        /* get native & adjust  */

    if (pItem->auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {    /* double wide print*/
        PrtDouble(aszAmountD, TCHARSIZEOF(aszAmountD), aszAmountS);
        pszAmount = aszAmountD;
    } else {
        pszAmount = aszAmountS;
    }
    PmgPrintf(PMG_PRT_SLIP, aszPrtSPVLTotal, aszMnem, pszAmount);
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

        /* send cashier interrupt mnemonic, R3.3 */
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
    //memcpy(puchBuffer, aszDflBuff, usLineNo*(PRT_DFL_LINE+1));
    
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
    UCHAR  uchAdr1;
	USHORT usTranAdr2;

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
