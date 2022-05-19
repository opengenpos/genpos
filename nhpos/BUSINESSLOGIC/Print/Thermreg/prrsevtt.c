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
* Title       : Print  total key (Service Total)               
* Category    : Print, NCR 2170 US Hospitarity Application 
* Program Name: PRRSevTT.C                                                      
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               PrtServTotal() : prints service total key
*               PrtServTotal_TH() : prints service total key (thermal)
*               PrtServTotal_EJ() : prints service total key (electric journal)
*               PrtServTotal_SP() : prints service total key (slip)
*               PrtServTotal_SPVL() : prints service total key (slip validation)
*               PrtServTotalPost() : prints service total key for post check
*               PrtServTotalPost_TH() : prints sevice total key (thermal)
*               PrtServTotalPost_EJ() : prints service total key (electric journal)
*               PrtServTotalPost_SP() : prints service total key (slip)
*               PrtServTotalPost_SPVL() : prints total key (slip validation)
*               PrtServTotalStubPost() : prints total stub for post check
*               PrtSoftCheckTotal() : prints total key (soft check)
*               PrtSoftCheckTotal_TH() : prints total key (thermal)
*               PrtServTotalPost_EJ()  : prints total key (electric journal)
*               PrtSoftCheckTotal_SP() : prints total key (slip)
*               PrtSoftCheckTotal_SPVL() : prints total key (slip validation)
*
*               PrtDflServTotal() : displays total key (service total)
*               PrtDflServTotalStubPost() : displays total key (service total)
*               PrtDflServTotalForm() : displays total key (service total)
*               PrtDflServTotalStubPostForm() : displays total key (service total)
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-10-93 : 00.00.01 : R.Itoh     : initial                                   
* Jul-13-93 : 01.00.12 : R.Itoh     : add PrtDflServTotal()                                  
* Oct-29-93 : 02.00.02 : K.You      : del. PrtDflServTotal(), PrtDflServTotalStubPost()                                  
* Apr-10-94 : 00.00.04 :  K.You     : add validation slip print feature.(mod. PrtServTotal())
* Apr-25-94 : 00.00.05 : K.You      : bug fixed E-33,35,41(mod. PrtSoftCheckTotal_TH, PrtSoftCheckTotal_SP)
* Apr-29-94 : 02.05.00 : K.You      : bug fixed E-77(mod. PrtServTotal_TH, PrtServTotal_EJ, PrtServTotal_SP)
* May-16-94 : 02.05.00 : Yoshiko.Jim: S-005 (mod. PrtServTotal(), add PrtServTotal_SPVL(),
*                                   : mod PrtSoftCheckTotal(), add PrtSoftCheckTotal_SPVL())
* May-19-94 : 02.05.00 : K.You      : bug fixed Q-008 (mod PrtServTotal_SP(), PrtSoftCheckTotal_SP())
* Jan-26-95 : 03.00.00 : M.Ozawa    : add. PrtDflServTotal(), PrtDflServTotalStubPost()
* Jun-14-95 : 03.00.00 : T.Nakahata : add Total Mnemonic to Kitchen Feature
* Jul-21-95 : 03.00.04 : T.Nakahata : display customer name at 1st frame
* Jul-26-95 : 03.00.04 : T.Nakahata : print on journal by service total key
* May-09-96 : 03.05.05 : M.Ozawa    : add PrtServeTotalPost_SPVL() for bug fix
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
* Dec-14-99 : 00.00.01 : M.Ozawa    : add. PrtDflServTotalForm(), PrtDflServTotalStubPostForm()
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

/**------- 2170 local------**/
#include <ecr.h>
/* #include <rfl.h> */
#include <regstrct.h>
#include <transact.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <pmg.h>
#include <shr.h>
#include <rfl.h>
#include <dfl.h>
#include "prtrin.h"
#include "prtshr.h"
#include "prtdfl.h"
#include "prrcolm_.h"


/**
;============================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;============================================================================
**/
UCHAR   CliParaMDCCheck(USHORT address, UCHAR field);
static SHORT sItemCounter = 0;

extern VOID PrtSoftCHK(UCHAR uchMinorClass);

/*
*===========================================================================
** Format  : VOID   PrtServTotal(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints total key (service total)
*===========================================================================
*/
VOID PrtServTotal(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem)
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( fsPrtStatus & PRT_REQKITCHEN ) {     /* kitchen print */
        if (( ! CliParaMDCCheck( MDC_KPTR_ADR, ODD_MDC_BIT0 )) && ( CliParaMDCCheck( MDC_KPTR_ADR, ODD_MDC_BIT3 ))) {
            /* --- if total mnemonic to kitchen option is ON,
                print it to kitchen --- */
            PrtKPTotal(pItem);
        }
    }

    if ( pItem->fsPrintStatus & PRT_VALIDATION ) { /* validation print */
        if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) {
            if(PRT_SUCCESS != PrtSPVLInit())
			{
				return;
			}
			PmgBeginSmallValidation( PMG_PRT_SLIP );//change to allow Small Slips
            PrtSPVLHead(pTran);    
            PrtServTotal_SPVL(pTran, pItem);        /* S-005 corr. '94 5/16 */
            PrtSPVLTrail(pTran);
			PmgEndSmallValidation( PMG_PRT_SLIP );//change to allow Small Slips
            return;
        }
    }

    if ( fsPrtPrintPort & PRT_SLIP ) {        /* slip print */
		if( (pTran->TranItemizers.sItemCounter != 0) && CliParaMDCCheck(MDC_NEWKEYSEQ_ADR, ODD_MDC_BIT3)) //SR 217 JHHJ Item Count Option
		{
			PrtTHItemCount(pTran->TranItemizers.sItemCounter, PMG_PRT_SLIP);
		}

        PrtServTotal_SP(pTran, pItem);
    }

    if ( fsPrtPrintPort & PRT_RECEIPT ) {     /* thermal print */
        PrtServTotal_TH(pTran, pItem);
    }

    if ( fsPrtPrintPort & PRT_JOURNAL ) {     /* electric journal */
        PrtServTotal_EJ(pTran, pItem);
    } else if ( fsPrtStatus & PRT_SERV_JOURNAL ) {
		SHORT   fsPortSave = fsPrtPrintPort;

        fsPrtPrintPort = PRT_JOURNAL;
        PrtServTotal_EJ( pTran, pItem );
        fsPrtPrintPort = fsPortSave;
    }
}

/*
*===========================================================================
** Format  : VOID PrtServTotal_TH(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints service total key (thermal)
*===========================================================================
*/
VOID  PrtServTotal_TH(TRANINFORMATION  *pTran, ITEMTOTAL *pItem)
{
    TCHAR   auchApproval[1];
    SHORT   sType1, sType2;
    USHORT  i;
	TCHAR	tchAcctNo[NUM_NUMBER_EPT+1];

	memset(tchAcctNo, 0x00, sizeof(tchAcctNo));

	RflDecryptByteString ((UCHAR *)&(pItem->aszNumber[0]), sizeof(pItem->aszNumber));
	_tcsncpy(tchAcctNo, pItem->aszNumber, NUM_NUMBER_EPT);
	RflEncryptByteString ((UCHAR *)&(pItem->aszNumber[0]), sizeof(pItem->aszNumber));


    /* amount double wide? */
    if (pTran->TranCurQual.auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {
        sType1 = PRT_DOUBLE;
    } else {
        sType1 = PRT_SINGLE;
    }

    if (pTran->TranCurQual.flPrintStatus & CURQUAL_SINGLE_CHCK_SUM) {
        sType2 = PRT_SINGLE;
    } else {
        sType2 = PRT_DOUBLE;
    }

    PrtTHHead(pTran);                               /* print header if necessary */

    PrtFeed(PMG_PRT_RECEIPT, 1);                    /* 1 line feed(Receipt) */

    /* exp.date */

	memset (tchAcctNo, 0, sizeof(tchAcctNo));
	if (pItem->auchTotalStatus[4] & CURQUAL_TOTAL_PRE_AUTH_TOTAL) {
		if (!CliParaMDCCheck(MDC_PREAUTH_MASK_ADR,EVEN_MDC_BIT0)) {

			PrtServMaskAcctDate(tchAcctNo, pItem);
			auchApproval[0] = '\0';
			PrtTHOffline( pItem->fbModifier, pItem->auchExpiraDate, auchApproval );

			PrtTHNumber(tchAcctNo);                  /* number line */
		}
	} else {
		auchApproval[0] = '\0';
		PrtTHOffline( pItem->fbModifier, pItem->auchExpiraDate, auchApproval );

		PrtTHNumber(tchAcctNo);                  /* number line */
	}

    for (i = 0; i < NUM_CPRSPCO; i++) {
        PrtTHCPRspMsgText(pItem->aszCPMsgText[i]);  /* for charge posting */
    }

	PrtFeed(PMG_PRT_RECEIPT, 1);

    PrtTHAmtSym(RflChkTtlAdr(pItem), pItem->lService, sType1);     /* serv. ttl */
    
    if (  (pTran->TranCurQual.fsCurStatus & CURQUAL_OPEMASK) !=  CURQUAL_NEWCHECK  ) {                /* not newcheck */

        if (pItem->lTax != 0L) {

            PrtTHAmtSym(TRN_TXSUM_ADR, pItem->lTax, PRT_SINGLE); /* tax sum */
        }

        PrtTHAmtSym(TRN_CKSUM_ADR, pItem->lMI, sType2);          /* check sum */
    }
	
	PrtFeed(PMG_PRT_RECEIPT, 1);                    /* 1 line feed(Receipt) */
	
	//SR 746 JHHJ
	if (pItem->auchTotalStatus[4] & CURQUAL_TOTAL_PRE_AUTH_TOTAL) {
		PrtTHTipTotal(PMG_PRT_RECEIPT);
		PrtSoftCHK(SOFTCHK_EPT1_ADR);
	}

	memset (tchAcctNo, 0, sizeof(tchAcctNo));   // clear the memory area per the PABP recommendations
}

/*
*===========================================================================
** Format  : VOID PrtServTotal_EJ(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints service total key (electric journal)
*===========================================================================
*/
VOID  PrtServTotal_EJ(TRANINFORMATION  *pTran, ITEMTOTAL *pItem)
{
    TCHAR   auchApproval[1];
    SHORT   sType1, sType2;
    USHORT  i;

    /* amount double wide? */
    if (pTran->TranCurQual.auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {
        sType1 = PRT_DOUBLE;
    } else {
        sType1 = PRT_SINGLE;
    }

    if (pTran->TranCurQual.flPrintStatus & CURQUAL_SINGLE_CHCK_SUM) {
        sType2 = PRT_SINGLE;
    } else {
        sType2 = PRT_DOUBLE;
    }

    auchApproval[0] = '\0';

	//SR 643 Account# and Date always mask on EJ
	if (pItem->aszNumber[0] != _T('\0')) {
		for (i=0;i<_tcslen(pItem->aszNumber);i++) {
			pItem->aszNumber[i] = _T('X');
		}
	}

	if (pItem->auchExpiraDate[0] != _T('\0')) {
		for (i=0;i<_tcslen(pItem->auchExpiraDate);i++) {
			pItem->auchExpiraDate[i] = _T('X');
		}
	}

    PrtEJOffline( pItem->fbModifier, pItem->auchExpiraDate, auchApproval );

    PrtEJNumber(pItem->aszNumber);                  /* number line */

    PrtEJFolioPost(pItem->aszFolioNumber, pItem->aszPostTransNo);   /* for charge posting */

    if (((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT1) ||
        ((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT2)) {

        /* print cashier interrupt mnemonic on journal, R3.3 */
        PrtEJAmtSym(TRN_CASINT_ADR, pItem->lService, sType1);  /* serv. ttl */
    } else {

        PrtEJAmtSym(RflChkTtlAdr(pItem), pItem->lService, sType1);  /* serv. ttl */
    }
    
    if (  (pTran->TranCurQual.fsCurStatus & CURQUAL_OPEMASK)
           !=  CURQUAL_NEWCHECK  ) {                /* not newcheck */

        if (pItem->lTax != 0L) {

            PrtEJAmtSym(TRN_TXSUM_ADR, pItem->lTax, PRT_SINGLE); /* tax sum */
        }

        PrtEJAmtSym(TRN_CKSUM_ADR, pItem->lMI, sType2);          /* check sum */
    }

    for (i = 0; i < NUM_CPRSPCO; i++) {
        PrtEJCPRspMsgText(pItem->aszCPMsgText[i]);  /* for charge posting */
    }
}

/*
*===========================================================================
** Format  : VOID PrtServTotal_SP(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints total key (service total)
*===========================================================================
*/
VOID PrtServTotal_SP(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem)
{
    TCHAR   auchApproval[1];
    TCHAR   aszSPPrintBuff[5 + NUM_CPRSPCO_EPT][PRT_SPCOLUMN + 1]; /* print data save area */
    USHORT  usSlipLine = 0;             /* number of lines to be printed */
    USHORT  usSaveLine;                 /* save slip lines to be added */
    USHORT  usCPRspLine = 0;            /* number of CP response line to be printed */
    USHORT  usCPRspTmpLine = 0;         /* number of CP response temporary line to be printed */
    SHORT   sType1, sType2;             /* double wide or single */
    USHORT  i;
    
    /* -- initialize the buffer -- */    
    memset(aszSPPrintBuff[0], '\0', sizeof(aszSPPrintBuff));

    /* -- service total amount double wide? -- */
    if (pTran->TranCurQual.auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {
        sType1 = PRT_DOUBLE;
    } else {
        sType1 = PRT_SINGLE;
    }

    /* -- check sum amount double wide? -- */
    if (pTran->TranCurQual.flPrintStatus & CURQUAL_SINGLE_CHCK_SUM) {
        sType2 = PRT_SINGLE;
    } else {
        sType2 = PRT_DOUBLE;
    }
    
    /* -- exp.date -- */
    auchApproval[0] = '\0';
    usSlipLine += PrtSPOffline(aszSPPrintBuff[usSlipLine], pItem->fbModifier, pItem->auchExpiraDate, auchApproval );

    /* -- set mnemonic and number -- */
    usSlipLine += PrtSPVoidNumber(aszSPPrintBuff[usSlipLine], pItem->fbModifier, pItem->usReasonCode, pItem->aszNumber);
    
    /* -- in case of new check -- */    
    if ((pTran->TranCurQual.fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_NEWCHECK) {   

        /* -- set service total -- */
        usSlipLine += PrtSPServiceTotal((TCHAR *)aszSPPrintBuff[usSlipLine], RflChkTtlAdr(pItem), pItem->lService, sType1, pTran, usSlipLine); /* ### Mod (2171 for Win32) V1.0 */
    
        /* -- set response message text -- */
        for (i = 0; i < NUM_CPRSPCO; i++) {
            usCPRspTmpLine = PrtSPCPRspMsgText(aszSPPrintBuff[usSlipLine], pItem->aszCPMsgText[i]);
            usSlipLine += usCPRspTmpLine;
            usCPRspLine += usCPRspTmpLine;
        }

        /* -- check if paper change is necessary or not -- */ 
        usSaveLine = PrtCheckLine(usSlipLine, pTran);  
    
        /* -- if paper changed, re-set page, line, consecutive No. -- */    
        if (usSaveLine != 0) {   
            usSlipLine --;        /* override the data in the last buffer */
            usSlipLine -= usCPRspLine;
            usSlipLine += PrtSPServiceTotal((TCHAR *)aszSPPrintBuff[usSlipLine], RflChkTtlAdr(pItem), pItem->lService, sType1, pTran, (USHORT)(usSlipLine + usSaveLine));
            usSlipLine += usCPRspLine;
        }    
    
    /* -- in case of reorder -- */    
    } else if ((pTran->TranCurQual.fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_REORDER) {
        /* -- set service total -- */    
        usSlipLine += PrtSPServTaxSum(aszSPPrintBuff[usSlipLine], RflChkTtlAdr(pItem), pItem->lService, sType1);
    
        /* -- set tax sum mnemonic and amount -- */    
        if (pItem->lTax != 0L) {
            usSlipLine += PrtSPServTaxSum(aszSPPrintBuff[usSlipLine], TRN_TXSUM_ADR, pItem->lTax, PRT_SINGLE);
        }    
        /* -- set check sum mnemonic and amount -- */    
        usSlipLine += PrtSPServiceTotal((TCHAR *)aszSPPrintBuff[usSlipLine], TRN_CKSUM_ADR, pItem->lMI, sType2, pTran, usSlipLine);
    
        /* -- set response message text -- */
        for (i = 0; i < NUM_CPRSPCO; i++) {
            usCPRspTmpLine = PrtSPCPRspMsgText(aszSPPrintBuff[usSlipLine], pItem->aszCPMsgText[i]);
            usSlipLine += usCPRspTmpLine;
            usCPRspLine += usCPRspTmpLine;
        }

        /* -- check if paper change is necessary or not -- */ 
        usSaveLine = PrtCheckLine(usSlipLine, pTran);
    
        /* -- if paper changed, re-set page, line, consecutive No. -- */    
        if (usSaveLine != 0) {
            usSlipLine --;      /* override the data in the last buffer */
            usSlipLine -= usCPRspLine;
            usSlipLine += PrtSPServiceTotal((TCHAR *)aszSPPrintBuff[usSlipLine],  
                    (UCHAR)TRN_CKSUM_ADR, pItem->lMI, (BOOL)sType2, pTran, (USHORT)(usSlipLine + usSaveLine)); /* ### Mod (2171 for Win32) V1.0 */
            usSlipLine += usCPRspLine;
        }    
    
    }    
    
    /* -- print all data in the buffer -- */ 
    for (i = 0; i < usSlipLine; i++) {
/*  --- fix a glitch (05/15/2001)
        PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN); */
        PrtPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);
    }

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;        

    PrtSlpRel();                            /* slip release */

    usPrtSlipPageLine++;                    /* K */
}

/*
*===========================================================================
** Format  : VOID PrtServTotal_SPVL(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints total key (service total) for slip validation.
*===========================================================================
*/
VOID PrtServTotal_SPVL(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem)
{
    TCHAR   auchApproval[1];
    TCHAR   aszSPPrintBuff[5][PRT_SPCOLUMN + 1]; /* print data save area */
    USHORT  i, usSaveLine, usSlipLine = 0;
    SHORT   sType1, sType2;
    
    memset(aszSPPrintBuff[0], '\0', sizeof(aszSPPrintBuff));
    if (pTran->TranCurQual.auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {
        sType1 = PRT_DOUBLE;
    } else {
        sType1 = PRT_SINGLE;
    }

    if (pTran->TranCurQual.flPrintStatus & CURQUAL_SINGLE_CHCK_SUM) {
        sType2 = PRT_SINGLE;
    } else {
        sType2 = PRT_DOUBLE;
    }
    
    auchApproval[0] = '\0';                             /* set exp.date     */
    usSlipLine += PrtSPOffline(aszSPPrintBuff[usSlipLine], pItem->fbModifier, pItem->auchExpiraDate, auchApproval );

    usSlipLine += PrtSPVoidNumber(aszSPPrintBuff[usSlipLine], pItem->fbModifier, pItem->usReasonCode, pItem->aszNumber);
    
                                                    /* new check operation? */    
    if ((pTran->TranCurQual.fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_NEWCHECK) {   
                                                    /* set service total    */
        usSlipLine += PrtSPMnemNativeAmt(aszSPPrintBuff[usSlipLine], RflChkTtlAdr(pItem), pItem->lService, sType1);

        usSaveLine = PrtCheckLine(usSlipLine, pTran);   /* paper change ?   */ 
        if (usSaveLine != 0) {   
            usSlipLine --;                              /* override data    */
            usSlipLine += PrtSPMnemNativeAmt(aszSPPrintBuff[usSlipLine], RflChkTtlAdr(pItem), pItem->lService, sType1);
        }    
                                                    /* reorder operation    */    
    } else if ((pTran->TranCurQual.fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_REORDER) {
    
                                                    /* set service total    */    
        usSlipLine += PrtSPServTaxSum(aszSPPrintBuff[usSlipLine], RflChkTtlAdr(pItem), pItem->lService, sType1);
                                                /* set tax sum mnemo & amt  */    
        if (pItem->lTax != 0L) {
            usSlipLine += PrtSPServTaxSum(aszSPPrintBuff[usSlipLine], TRN_TXSUM_ADR, pItem->lTax, PRT_SINGLE);
        }    
                                                /* set check sum mnemo & amt*/    
        usSlipLine += PrtSPMnemNativeAmt(aszSPPrintBuff[usSlipLine], TRN_CKSUM_ADR, pItem->lMI, sType2);
                                                    /* check paper change   */ 
        usSaveLine = PrtCheckLine(usSlipLine, pTran);
        if (usSaveLine != 0) {
            usSlipLine --;                          /* override the data    */
            usSlipLine += PrtSPMnemNativeAmt(aszSPPrintBuff[usSlipLine], TRN_CKSUM_ADR, pItem->lMI, sType2);
        }    
    }    
    
    for (i = 0; i < usSlipLine; i++) {              /* print all data       */ 
/*  --- fix a glitch (05/15/2001)
        PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN); */
        PrtPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);
    }

    usPrtSlipPageLine += usSlipLine + usSaveLine;   /* update current line# */
    usPrtSlipPageLine++;                            /* K */
}

/*
*===========================================================================
** Format  : VOID   PrtServTotalPost(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints total key (service total for post check)
*===========================================================================
*/
VOID PrtServTotalPost(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem)
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( fsPrtStatus & PRT_REQKITCHEN ) {     /* kitchen print */
        if (( ! CliParaMDCCheck( MDC_KPTR_ADR, ODD_MDC_BIT0 )) && ( CliParaMDCCheck( MDC_KPTR_ADR, ODD_MDC_BIT3 ))) {
            /* --- if total mnemonic to kitchen option is ON,
                print it to kitchen --- */
            PrtKPTotal(pItem);
        }
    }

    if ( pItem->fsPrintStatus & PRT_VALIDATION ) { /* validation print */
        if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) {
            if(PRT_SUCCESS != PrtSPVLInit())
			{
				return;
			}
			PmgBeginSmallValidation( PMG_PRT_SLIP );//change to allow Small Slips
            PrtSPVLHead(pTran);    
            PrtServTotalPost_SPVL(pTran, pItem);
            PrtSPVLTrail(pTran);
			PmgEndSmallValidation( PMG_PRT_SLIP );//change to allow Small Slips
        }
        return ;
    }

    if ( fsPrtPrintPort & PRT_SLIP ) {        /* slip print */
        PrtServTotalPost_SP(pTran, pItem);
    }

    if ( fsPrtPrintPort & PRT_RECEIPT ) {     /* thermal print */
        PrtServTotalPost_TH(pTran, pItem);
    }

    if ( fsPrtPrintPort & PRT_JOURNAL ) {     /* journal print */
        PrtServTotalPost_EJ(pTran, pItem);
    } else if ( fsPrtStatus & PRT_SERV_JOURNAL ) {
		SHORT fsPortSave = fsPrtPrintPort;

        fsPrtPrintPort = PRT_JOURNAL;
        PrtServTotalPost_EJ( pTran, pItem );
        fsPrtPrintPort = fsPortSave;
    }
}

/*
*===========================================================================
** Format  : VOID PrtServTotalPost_TH(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints service total key for post check (thermal)
*===========================================================================
*/
VOID  PrtServTotalPost_TH(TRANINFORMATION  *pTran, ITEMTOTAL *pItem)
{
    SHORT sType;

    if (pTran->TranCurQual.flPrintStatus & CURQUAL_SINGLE_CHCK_SUM) {
        sType = PRT_SINGLE;
    } else {
        sType = PRT_DOUBLE;
    }
    PrtTHHead(pTran);                               /* print header if necessary */

    PrtFeed(PMG_PRT_RECEIPT, 1);                    /* 1 line feed(Receipt) */

    PrtTHNumber(pItem->aszNumber);                  /* number line */

    PrtTHAmtSym(TRN_CKSUM_ADR, pItem->lMI, sType);  /* check sum */
}                                                   

/*
*===========================================================================
** Format  : VOID PrtServTotalPost_EJ(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints service total key  for post check (electric journal)
*===========================================================================
*/
VOID  PrtServTotalPost_EJ(TRANINFORMATION  *pTran, ITEMTOTAL *pItem)
{
    SHORT sType;

    if (pTran->TranCurQual.flPrintStatus & CURQUAL_SINGLE_CHCK_SUM) {
        sType = PRT_SINGLE;
    } else {
        sType = PRT_DOUBLE;
    }

    PrtEJNumber(pItem->aszNumber);                  /* number line */

    if (((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT1) ||
        ((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT2)) {

        /* print cashier interrupt mnemonic on journal, R3.3 */
        PrtEJAmtSym(TRN_CASINT_ADR, pItem->lMI, sType);  /* check sum */

    } else {

        PrtEJAmtSym(TRN_CKSUM_ADR, pItem->lMI, sType);  /* check sum */
    }
}                                                   

/*
*===========================================================================
** Format  : VOID PrtServTotalPost_SP(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints total key (service total for post check)
*===========================================================================
*/
VOID PrtServTotalPost_SP(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem)
{
    TCHAR   aszSPPrintBuff[2][PRT_SPCOLUMN + 1]; /* print data save area */
    USHORT  usSlipLine = 0;             /* number of lines to be printed */
    USHORT  usSaveLine;                 /* save slip lines to be added */
    SHORT   sType1;                     /* double wide or single */
    USHORT  i;
    
    /* -- initialize the buffer -- */    
    memset(aszSPPrintBuff[0], '\0', sizeof(aszSPPrintBuff));

    /* -- check sum amount double wide? -- */
    if (pTran->TranCurQual.flPrintStatus & CURQUAL_SINGLE_CHCK_SUM) {
        sType1 = PRT_SINGLE;
    } else {
        sType1 = PRT_DOUBLE;
    }
    
    /* -- set mnemonic and number -- */
    usSlipLine += PrtSPVoidNumber(aszSPPrintBuff[0], pItem->fbModifier, pItem->usReasonCode, pItem->aszNumber);
    
    /* -- set check sum mnemonic and amount -- */    
    usSlipLine += PrtSPServiceTotal((TCHAR *)aszSPPrintBuff[usSlipLine],  
                    (UCHAR)TRN_CKSUM_ADR, pItem->lMI, (BOOL)sType1, pTran, usSlipLine); /* ### Mod (2171 for Win32) V1.0 */
    
    /* -- check if paper change is necessary or not -- */ 
    usSaveLine = PrtCheckLine(usSlipLine, pTran);

    /* -- if paper changed, re-set page, line, consecutive No. -- */    
    if (usSaveLine != 0) {
        usSlipLine --;      /* override the data in the last buffer */
        usSlipLine += PrtSPServiceTotal(aszSPPrintBuff[usSlipLine],  
                (UCHAR)TRN_CKSUM_ADR, pItem->lMI, (BOOL)sType1, pTran, (USHORT)(usSlipLine + usSaveLine)); /* ### Mod (2171 for Win32) V1.0 */
    }    
    
    /* -- print all data in the buffer -- */ 
    for (i = 0; i < usSlipLine; i++) {
/*  --- fix a glitch (05/15/2001)
        PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN); */
        PrtPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);
    }

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;        

    PrtSlpRel();                            /* slip release */

    usPrtSlipPageLine++;                    /* K */
}

/*
*===========================================================================
** Format  : VOID PrtServTotalPost_SPVL(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints total key (service total for post check)
*===========================================================================
*/
VOID PrtServTotalPost_SPVL(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem)
{
    TCHAR   aszSPPrintBuff[2][PRT_SPCOLUMN + 1]; /* print data save area */
    USHORT  usSlipLine = 0;             /* number of lines to be printed */
    USHORT  usSaveLine;                 /* save slip lines to be added */
    SHORT   sType1;                     /* double wide or single */
    USHORT  i;
    
    /* -- initialize the buffer -- */    
    memset(aszSPPrintBuff[0], '\0', sizeof(aszSPPrintBuff));

    /* -- check sum amount double wide? -- */
    if (pTran->TranCurQual.flPrintStatus & CURQUAL_SINGLE_CHCK_SUM) {
        sType1 = PRT_SINGLE;
    } else {
        sType1 = PRT_DOUBLE;
    }
    
    /* -- set mnemonic and number -- */
    usSlipLine += PrtSPVoidNumber(aszSPPrintBuff[0], pItem->fbModifier, pItem->usReasonCode, pItem->aszNumber);
    
    /* -- set check sum mnemonic and amount -- */    
    usSlipLine += PrtSPMnemNativeAmt(aszSPPrintBuff[usSlipLine], TRN_CKSUM_ADR, pItem->lMI, sType1);
    
    /* -- check if paper change is necessary or not -- */ 
    usSaveLine = PrtCheckLine(usSlipLine, pTran);

    /* -- if paper changed, re-set page, line, consecutive No. -- */    
    if (usSaveLine != 0) {
        usSlipLine --;      /* override the data in the last buffer */
        usSlipLine += PrtSPMnemNativeAmt(aszSPPrintBuff[usSlipLine], TRN_CKSUM_ADR, pItem->lMI, sType1);
    }    
    
    /* -- print all data in the buffer -- */ 
    for (i = 0; i < usSlipLine; i++) {
/*  --- fix a glitch (05/15/2001)
        PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN); */
        PrtPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);
    }

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;        
    usPrtSlipPageLine++;                    /* K */
}

/*
*===========================================================================
** Format  : VOID   PrtServTotalStubPost(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints service total(post check)  key stub 
*===========================================================================
*/
VOID   PrtServTotalStubPost(TRANINFORMATION  *pTran, ITEMTOTAL *pItem)
{
    SHORT   sType;

    /* -- total amount printed double wide? -- */
    if (pItem->auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {
        sType = PRT_DOUBLE;
    } else {
        sType = PRT_SINGLE;
    }

    PrtTHHead(pTran);                               /* print header if necessary */

    if ( fbPrtShrStatus & PRT_SHARED_SYSTEM ) { 
        fbPrtAltrStatus |= PRT_TOTAL_STUB;
    }

    PrtTHAmtSym(TRN_CKSUM_ADR, pItem->lMI, sType);
}


/*
*===========================================================================
** Format  : VOID   PrtSoftCheckTotal(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints total key (soft check)
*===========================================================================
*/
VOID PrtSoftCheckTotal(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem)
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( fsPrtStatus & PRT_REQKITCHEN ) {     /* kitchen print */
        if (( ! CliParaMDCCheck( MDC_KPTR_ADR, ODD_MDC_BIT0 )) && ( CliParaMDCCheck( MDC_KPTR_ADR, ODD_MDC_BIT3 ))) {
            /* --- if total mnemonic to kitchen option is ON,
                print it to kitchen --- */
            PrtKPTotal(pItem);
        }
    }

    if ( pItem->fsPrintStatus & PRT_VALIDATION ) { /* validation print */
        if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) {
            if(PRT_SUCCESS != PrtSPVLInit())
			{
				return;
			}
			PmgBeginSmallValidation( PMG_PRT_SLIP );//change to allow Small Slips
            PrtSPVLHead(pTran);    
            PrtSoftCheckTotal_SPVL(pTran, pItem);        /* S-005 corr. '94 5/16 */
            PrtSPVLTrail(pTran);
			PmgEndSmallValidation( PMG_PRT_SLIP );//change to allow Small Slips
            return;
        }
    }

    if ( fsPrtPrintPort & PRT_SLIP ) {        /* slip print */
        PrtSoftCheckTotal_SP(pTran, pItem);
    }

    if ( fsPrtPrintPort & PRT_RECEIPT ) {     /* thermal print */
        PrtSoftCheckTotal_TH(pTran, pItem);
    }

    if (( ! ( fsPrtPrintPort & PRT_JOURNAL )) && ( fsPrtStatus & PRT_SERV_JOURNAL )) {
		SHORT fsPortSave = fsPrtPrintPort;

        fsPrtPrintPort = PRT_JOURNAL;
        PrtSoftCheckTotal_EJ( pTran, pItem );
        fsPrtPrintPort = fsPortSave;
    }
}

/*
*===========================================================================
** Format  : VOID PrtSoftCheckTotal_TH(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints service total key for soft check (thermal)
*===========================================================================
*/
VOID  PrtSoftCheckTotal_TH(TRANINFORMATION  *pTran, ITEMTOTAL *pItem)
{
    TCHAR   auchApproval[1];
    SHORT   sType;
    USHORT  i;
	TCHAR	tchAcctNo[NUM_NUMBER_EPT+1];

	memset(tchAcctNo, 0x00, sizeof(tchAcctNo));

	RflDecryptByteString ((UCHAR *)&(pItem->aszNumber[0]), sizeof(pItem->aszNumber));
	_tcsncpy(tchAcctNo, pItem->aszNumber, NUM_NUMBER_EPT);
	RflEncryptByteString ((UCHAR *)&(pItem->aszNumber[0]), sizeof(pItem->aszNumber));

    /* service total amount double wide? */

    if (pTran->TranCurQual.auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {
        sType = PRT_DOUBLE;
    } else {
        sType = PRT_SINGLE;
    }

    PrtTHHead(pTran);                               /* print header if necessary */

    PrtFeed(PMG_PRT_RECEIPT, 1);                    /* 1 line feed(Receipt) */

    /* exp.date */

    auchApproval[0] = '\0';

	if (pItem->auchTotalStatus[4] & CURQUAL_TOTAL_PRE_AUTH_TOTAL) {
		if (!CliParaMDCCheck(MDC_PREAUTH_MASK_ADR,EVEN_MDC_BIT0)) {

			PrtServMaskAcctDate(tchAcctNo, pItem);

			PrtTHOffline( pItem->fbModifier, pItem->auchExpiraDate, auchApproval );

			PrtTHNumber(tchAcctNo);                  /* number line */
		}
	} else {
		PrtTHOffline( pItem->fbModifier, pItem->auchExpiraDate, auchApproval );

		PrtTHNumber(tchAcctNo);                  /* number line */
	}
   
    for (i = 0; i < NUM_CPRSPCO; i++) {
        PrtTHCPRspMsgText(pItem->aszCPMsgText[i]);  /* for charge posting */
    }

	PrtFeed(PMG_PRT_RECEIPT, 1);

	pItem->uchMinorClass -= CLASS_SOFTCHK_BASE_TTL;
    PrtTHAmtSym(RflChkTtlAdr(pItem), pItem->lMI, sType);  /* service total */

    PrtFeed(PMG_PRT_RECEIPT, 1);                    /* 1 line feed(Receipt) */
	
	//SR 746 JHHJ
	if (pItem->auchTotalStatus[4] & CURQUAL_TOTAL_PRE_AUTH_TOTAL) {
		PrtTHTipTotal(PMG_PRT_RECEIPT);
	}

    pItem->uchMinorClass += CLASS_SOFTCHK_BASE_TTL;

	memset (tchAcctNo, 0, sizeof(tchAcctNo));   // clear the memory area per the PABP recommendations
}

/*
*===========================================================================
** Format  : VOID PrtSoftCheckTotal_EJ(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints service total key  for soft check (electric journal)
*===========================================================================
*/
VOID  PrtSoftCheckTotal_EJ(TRANINFORMATION  *pTran, ITEMTOTAL *pItem)
{
    TCHAR   auchApproval[1];
    SHORT   sType;
    USHORT  i;

    if (pTran->TranCurQual.auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {
        sType = PRT_DOUBLE;
    } else {
        sType = PRT_SINGLE;
    }

    auchApproval[0] = '\0';

	//SR 643 Account# and Date always mask on EJ
	if (pItem->aszNumber[0] != _T('\0')) {
		for (i=0;i<_tcslen(pItem->aszNumber);i++) {
			pItem->aszNumber[i] = _T('X');
		}
	}

	if (pItem->auchExpiraDate[0] != _T('\0')) {
		for (i=0;i<_tcslen(pItem->auchExpiraDate);i++) {
			pItem->auchExpiraDate[i] = _T('X');
		}
	}

    PrtEJOffline( pItem->fbModifier, pItem->auchExpiraDate, auchApproval );

    PrtEJNumber(pItem->aszNumber);                  /* number line */

    PrtEJFolioPost(pItem->aszFolioNumber, pItem->aszPostTransNo);   /* for charge posting */

    pItem->uchMinorClass -= CLASS_SOFTCHK_BASE_TTL;
    if (((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT1) ||
        ((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT2)) {

        /* print cashier interrupt mnemonic on journal, R3.3 */
        PrtEJAmtSym(TRN_CASINT_ADR, pItem->lMI, sType);

    } else {

        PrtEJAmtSym(RflChkTtlAdr(pItem), pItem->lMI, sType);
    }
    pItem->uchMinorClass += CLASS_SOFTCHK_BASE_TTL;

    for (i = 0; i < NUM_CPRSPCO; i++) {
        PrtEJCPRspMsgText(pItem->aszCPMsgText[i]);  /* for charge posting */
    }
}                                                   

/*
*===========================================================================
** Format  : VOID PrtSoftCheckTotal_SP(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints total key (soft check)
*===========================================================================
*/
VOID PrtSoftCheckTotal_SP(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem)
{
    TCHAR   auchApproval[1];
    TCHAR   aszSPPrintBuff[3 + NUM_CPRSPCO_EPT][PRT_SPCOLUMN + 1]; /* print data save area */
    SHORT   sType1;                     /* double wide or single */
    USHORT  usSlipLine = 0;             /* number of lines to be printed */
    USHORT  usCPRspLine = 0;            /* number of CP response line to be printed */
    USHORT  usCPRspTmpLine = 0;         /* number of CP response temporary line to be printed */
    USHORT  usSaveLine;                 /* save slip lines to be added */
    USHORT  i;
    
    /* -- initialize the buffer -- */    
    memset(aszSPPrintBuff[0], '\0', sizeof(aszSPPrintBuff));

    /* -- check sum amount double wide? -- */
    if (pTran->TranCurQual.auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {
        sType1 = PRT_DOUBLE;
    } else {
        sType1 = PRT_SINGLE;
    }

    /* -- exp.date -- */

    auchApproval[0] = '\0';
    usCPRspTmpLine = PrtSPOffline(aszSPPrintBuff[usSlipLine], pItem->fbModifier, pItem->auchExpiraDate, auchApproval );
    usSlipLine += usCPRspTmpLine;
/*    usCPRspLine += usCPRspTmpLine; */

    /* -- set mnemonic and number -- */
    usCPRspTmpLine = PrtSPVoidNumber(aszSPPrintBuff[usSlipLine], pItem->fbModifier, pItem->usReasonCode, pItem->aszNumber);
    usSlipLine += usCPRspTmpLine;
/*    usCPRspLine += usCPRspTmpLine; */

    /* -- set check sum mnemonic and amount -- */
    pItem->uchMinorClass -= CLASS_SOFTCHK_BASE_TTL;
    usSlipLine += PrtSPServiceTotal(aszSPPrintBuff[usSlipLine], RflChkTtlAdr(pItem), pItem->lMI, sType1, pTran, usSlipLine);
    pItem->uchMinorClass += CLASS_SOFTCHK_BASE_TTL;
    
    /* -- set response message text -- */
    for (i = 0; i < NUM_CPRSPCO; i++) {
        usCPRspTmpLine = PrtSPCPRspMsgText(aszSPPrintBuff[usSlipLine], pItem->aszCPMsgText[i]);
        usSlipLine += usCPRspTmpLine;
        usCPRspLine += usCPRspTmpLine;
    }

    /* -- check if paper change is necessary or not -- */ 
    usSaveLine = PrtCheckLine(usSlipLine, pTran);

    /* -- if paper changed, re-set page, line, consecutive No. -- */    
    if (usSaveLine != 0) {
        usSlipLine --;      /* override the data in the last buffer */
        usSlipLine -= usCPRspLine;
        pItem->uchMinorClass -= CLASS_SOFTCHK_BASE_TTL;
        usSlipLine += PrtSPServiceTotal(aszSPPrintBuff[usSlipLine],  
			RflChkTtlAdr(pItem), pItem->lMI, (BOOL)sType1, pTran, (USHORT)(usSlipLine + usSaveLine)); /* ### Mod (2171 for Win32) V1.0 */
        pItem->uchMinorClass += CLASS_SOFTCHK_BASE_TTL;
        usSlipLine += usCPRspLine;
    }    
    
    /* -- print all data in the buffer -- */ 
    for (i = 0; i < usSlipLine; i++) {
/*  --- fix a glitch (05/15/2001)
        PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN); */
        PrtPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);       
    }

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;        

    PrtSlpRel();                            /* slip release */
    usPrtSlipPageLine++;                    /* K */
}

/*
*===========================================================================
** Format  : VOID PrtSoftCheckTotal_SPVL(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints total key (soft check) for slip validation.
*===========================================================================
*/
VOID PrtSoftCheckTotal_SPVL(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem)
{
    TCHAR   auchApproval[1];
    TCHAR   aszSPPrintBuff[3][PRT_SPCOLUMN + 1]; /* print data save area */
    SHORT   sType;
    USHORT  usCPRspLine = 0;            /* number of CP response line to be printed */
    USHORT  usCPRspTmpLine = 0;         /* number of CP response temporary line to be printed */
    USHORT  i, usSaveLine, usSlipLine = 0;
    
    memset(aszSPPrintBuff[0], '\0', sizeof(aszSPPrintBuff));
    if (pTran->TranCurQual.auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {  /* double wide? */
        sType = PRT_DOUBLE;
    } else {
        sType = PRT_SINGLE;
    }

    auchApproval[0] = '\0';                             /* set exp.date     */
    usCPRspTmpLine = PrtSPOffline(aszSPPrintBuff[usSlipLine], pItem->fbModifier, pItem->auchExpiraDate, auchApproval );
    usSlipLine += usCPRspTmpLine;
/*    usCPRspLine += usCPRspTmpLine; */

    /* set mnemo & number   */
    usCPRspTmpLine = PrtSPVoidNumber(aszSPPrintBuff[usSlipLine], pItem->fbModifier, pItem->usReasonCode, pItem->aszNumber);
    
    usSlipLine += usCPRspTmpLine;
/*    usCPRspLine += usCPRspTmpLine; */

    pItem->uchMinorClass -= CLASS_SOFTCHK_BASE_TTL;     /* set service ttl  */
    usSlipLine += PrtSPMnemNativeAmt(aszSPPrintBuff[usSlipLine], RflChkTtlAdr(pItem), pItem->lMI, sType);
    pItem->uchMinorClass += CLASS_SOFTCHK_BASE_TTL;

    usSaveLine = PrtCheckLine(usSlipLine, pTran);       /* paper change ?   */ 
    if (usSaveLine != 0) {
        usSlipLine --;                              /* override last buffer */
        usSlipLine -= usCPRspLine;
        usSlipLine += PrtSPMnemNativeAmt(aszSPPrintBuff[usSlipLine], RflChkTtlAdr(pItem), pItem->lMI, sType);
        usSlipLine += usCPRspLine;
    }    
    
    for (i = 0; i < usSlipLine; i++) {                  /* print all data   */ 
/*  --- fix a glitch (05/15/2001)
        PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN); */
        PrtPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);
    }

    usPrtSlipPageLine += usSlipLine + usSaveLine;       /* update line No.  */    
    usPrtSlipPageLine++;
}

/*
*===========================================================================
** Format  : VOID   PrtDflServTotal(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays total key (service total)
*===========================================================================
*/
VOID PrtDflServTotal(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem)
{
    TCHAR   auchApproval[1];
    TCHAR   aszDflBuff[24][PRT_DFL_LINE + 1];   /* display data save area */
    USHORT  usLineNo;                       /* number of lines to be displayed */
    USHORT  usOffset = 0;                       
    USHORT  i;                       
    SHORT   sType1, sType2;

    /* --- if this frame is 1st frame, display customer name --- */
    PrtDflCustHeader( pTran );

    /* amount double wide? */
    if (pTran->TranCurQual.auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {
        sType1 = PRT_DOUBLE;
    } else {
        sType1 = PRT_SINGLE;
    }

    if (pTran->TranCurQual.flPrintStatus & CURQUAL_SINGLE_CHCK_SUM) {
        sType2 = PRT_SINGLE;
    } else {
        sType2 = PRT_DOUBLE;
    }

    memset(aszDflBuff, '\0', sizeof(aszDflBuff));

    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);

    /* EPT offline symbol & exp.date */ 
    auchApproval[0] = '\0';
    usLineNo += PrtDflOffline(aszDflBuff[usLineNo], pItem->fbModifier, pItem->auchExpiraDate, auchApproval );

    /* -- set item data -- */
    usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber);  
    usLineNo += PrtDflFolioPost(aszDflBuff[usLineNo], pItem->aszFolioNumber, pItem->aszPostTransNo);    /* for charge posting */

    if (((pTran->TranCurQual.flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_POSTCHECK ) &&
        ( CliParaMDCCheck( MDC_GCNO_ADR, EVEN_MDC_BIT3 ) == 0) &&
        ( CliParaMDCCheck( MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2 ) == 0)) {

        /* split check system service total */
        if (((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT1) ||
            ((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT2)) {

            /* print cashier interrupt mnemonic on journal, R3.3 */
            usLineNo += PrtDflAmtSym(aszDflBuff[usLineNo], TRN_CASINT_ADR, pItem->lMI, sType1);
        } else {
            usLineNo += PrtDflAmtSym(aszDflBuff[usLineNo], RflChkTtlAdr(pItem), pItem->lMI, sType1);
        }
    } else {
        if (((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT1) ||
            ((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT2)) {

            /* print cashier interrupt mnemonic on journal, R3.3 */
            usLineNo += PrtDflAmtSym(aszDflBuff[usLineNo], TRN_CASINT_ADR, pItem->lService, sType1);
        } else {
            usLineNo += PrtDflAmtSym(aszDflBuff[usLineNo], RflChkTtlAdr(pItem), pItem->lService, sType1);
        }
        if (  (pTran->TranCurQual.fsCurStatus & CURQUAL_OPEMASK) !=  CURQUAL_NEWCHECK  ) {                /* not newcheck */
            if (pItem->lTax != 0L) {
                usLineNo += PrtDflAmtSym(aszDflBuff[usLineNo], TRN_TXSUM_ADR, pItem->lTax, PRT_SINGLE); 
            }

            usLineNo += PrtDflAmtSym(aszDflBuff[usLineNo], TRN_CKSUM_ADR, pItem->lMI, sType2);          
        }
    }

    for (i = 0; i < NUM_CPRSPCO; i++) {
        usLineNo += PrtDflCPRspMsgText(aszDflBuff[usLineNo], pItem->aszCPMsgText[i]);         /* for charge posting */
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
** Format  : VOID   PrtDflServTotalStubPost(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays total key (service total) in post check.
*===========================================================================
*/
VOID PrtDflServTotalStubPost(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem)
{
    TCHAR  aszDflBuff[5][PRT_DFL_LINE + 1];   /* display data save area */
    USHORT  usLineNo;                       /* number of lines to be displayed */
    USHORT  usOffset = 0;                       
    USHORT  i;                       
    SHORT   sType;

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
    if (((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT1) ||
        ((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT2)) {

        /* print cashier interrupt mnemonic on journal, R3.3 */
        usLineNo += PrtDflAmtSym(aszDflBuff[usLineNo], TRN_CASINT_ADR, pItem->lMI, sType); 

    } else {

        usLineNo += PrtDflAmtSym(aszDflBuff[usLineNo], TRN_CKSUM_ADR, pItem->lMI, sType); 
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
** Format  : VOID   PrtDflServTotal(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays total key (service total)
*===========================================================================
*/
USHORT PrtDflServTotalForm(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem, TCHAR *puchBuffer)
{
    TCHAR   auchApproval[1];
    TCHAR   aszDflBuff[24][PRT_DFL_LINE + 1];   /* display data save area */
    USHORT  usLineNo=0, i;                       /* number of lines to be displayed */
    USHORT  usOffset = 0;                       
    SHORT   sType1, sType2;

    /* amount double wide? */
    if (pTran->TranCurQual.auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {
        sType1 = PRT_DOUBLE;
    } else {
        sType1 = PRT_SINGLE;
    }

    if (pTran->TranCurQual.flPrintStatus & CURQUAL_SINGLE_CHCK_SUM) {
        sType2 = PRT_SINGLE;
    } else {
        sType2 = PRT_DOUBLE;
    }

    memset(aszDflBuff, '\0', sizeof(aszDflBuff));
#if 0
    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);
#endif
    /* EPT offline symbol & exp.date */ 
    auchApproval[0] = '\0';
    usLineNo += PrtDflOffline(aszDflBuff[usLineNo], pItem->fbModifier, pItem->auchExpiraDate,
                  auchApproval );

    /* -- set item data -- */
    usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber);  
    usLineNo += PrtDflFolioPost(aszDflBuff[usLineNo], pItem->aszFolioNumber, pItem->aszPostTransNo);    /* for charge posting */

    if (((pTran->TranCurQual.flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_POSTCHECK ) &&
        ( CliParaMDCCheck( MDC_GCNO_ADR, EVEN_MDC_BIT3 ) == 0) &&
        ( CliParaMDCCheck( MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2 ) == 0)) {

        /* split check system service total */
        if (((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT1) ||
            ((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT2)) {

            /* print cashier interrupt mnemonic on journal, R3.3 */
            usLineNo += PrtDflAmtSym(aszDflBuff[usLineNo], TRN_CASINT_ADR, pItem->lMI, sType1);
        } else {

            usLineNo += PrtDflAmtSym(aszDflBuff[usLineNo], RflChkTtlAdr(pItem), pItem->lMI, sType1);
        }
    } else {

        if (((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT1) ||
            ((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT2)) {

            /* print cashier interrupt mnemonic on journal, R3.3 */
            usLineNo += PrtDflAmtSym(aszDflBuff[usLineNo], TRN_CASINT_ADR, pItem->lService, sType1);
        } else {

            usLineNo += PrtDflAmtSym(aszDflBuff[usLineNo], RflChkTtlAdr(pItem), pItem->lService, sType1);
        }
        if (  (pTran->TranCurQual.fsCurStatus & CURQUAL_OPEMASK) !=  CURQUAL_NEWCHECK  ) {     /* not newcheck */

            if (pItem->lTax != 0L) {

                usLineNo += PrtDflAmtSym(aszDflBuff[usLineNo], TRN_TXSUM_ADR, pItem->lTax, PRT_SINGLE); 
            }

            usLineNo += PrtDflAmtSym(aszDflBuff[usLineNo], TRN_CKSUM_ADR, pItem->lMI, sType2);          
        }
    }

    for (i=0; i<usLineNo; i++) {
        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }
    _tcsncpy(puchBuffer, aszDflBuff[0], usLineNo*(PRT_DFL_LINE+1));
    
    return usLineNo;

}

/*
*===========================================================================
** Format  : VOID   PrtDflServTotalStubPost(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays total key (service total) in post check.
*===========================================================================
*/
USHORT PrtDflServTotalStubPostForm(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem, TCHAR *puchBuffer)
{
	TCHAR  aszDflBuff[5][PRT_DFL_LINE + 1] = { 0 };   /* display data save area */
    USHORT  usLineNo=0, i;                       /* number of lines to be displayed */
    USHORT  usOffset = 0;                       
    SHORT   sType;

    /* -- total amount printed double wide? -- */
    if (pItem->auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {
        sType = PRT_DOUBLE;
    } else {
        sType = PRT_SINGLE;
    }

#if 0
    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);
#endif
    /* -- set item data -- */
    if (((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT1) ||
        ((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT2)) {

        /* print cashier interrupt mnemonic on journal, R3.3 */
        usLineNo += PrtDflAmtSym(aszDflBuff[usLineNo], TRN_CASINT_ADR, pItem->lMI, sType); 

    } else {
        usLineNo += PrtDflAmtSym(aszDflBuff[usLineNo], TRN_CKSUM_ADR, pItem->lMI, sType); 
    }
    
	NHPOS_ASSERT_TEXT(usLineNo < 5,"**ERROR: PrtDflServTotalStubPostForm() usLineNo > number lines in aszDflBuff");

    for (i=0; i<usLineNo; i++) {
        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }
    _tcsncpy(puchBuffer, aszDflBuff[0], usLineNo*(PRT_DFL_LINE+1));
    
    return usLineNo;
}

/*==========================================================================
**  Synopsis:   VOID  PrtServMaskAcctDate(TCHAR *pAcctNumber, ITEMTOTAL *pItem)
*
*   Input:
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    If Pre-Authorization system, check and mask acct and date
*                   See also function MldServMaskAcctDate() which does the same
*                   operation for the MLD subsystem.
==========================================================================*/
VOID  PrtServMaskAcctDate(TCHAR *pAcctNumber, ITEMTOTAL *pItem)
{
	//Check if Pre-Authorization
	if (pItem->auchTotalStatus[4] & CURQUAL_TOTAL_PRE_AUTH_TOTAL) {
		//Perform Mask
		if (!CliParaMDCCheck(MDC_PREAUTH_MASK_ADR,EVEN_MDC_BIT1)) {
			PRTPARAMDC		MaskMDC = { 0 };
			int				iShownChars = 0, iMaskedChars = 0, i;

			//How many to mask
			MaskMDC.uchMajorClass = CLASS_PARAMDC;
			MaskMDC.usAddress = MDC_PREAUTH_MASK_ADR;
			CliParaRead(&MaskMDC);

			iShownChars = 0;
			if((MaskMDC.uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == EVEN_MDC_BIT2){
				//show 2 rightmost digits
				iShownChars = 2;
			}
			if((MaskMDC.uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == EVEN_MDC_BIT3){
				//show 4 rightmost digits
				iShownChars = 4;
			}
			if((MaskMDC.uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)){
				//show 6 rightmost digits
				iShownChars = 6;
			}

			iMaskedChars = NUM_NUMBER_EPT - iShownChars;
			iMaskedChars = iMaskedChars ? iMaskedChars : 0;

			for (i = 0; i < iMaskedChars; i++) {
				*pAcctNumber = _T('X');
				pAcctNumber++;
			}

			for (i = 0; i < NUM_EXPIRA; i++) {
				pItem->auchExpiraDate[i] = _T('X');
			}

		}
	}

}

/***** End Of Source *****/
