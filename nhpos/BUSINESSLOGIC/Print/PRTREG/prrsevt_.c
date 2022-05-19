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
* Program Name: PRRSevT_.C                                                      
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               PrtServTotal() : prints total key (service total)
*               PrtServTotal_RJ() : prints total key (receipt & journal)
*               PrtServTotal_VL() : prints total key (validation)
*               PrtServTotal_SP() : prints total key (slip)
*               PrtServTotal_SPVL  : prints service total key (slip validation)
*               PrtServTotalPost() : prints total key (service total for post check)
*               PrtServTotalPost_RJ() : prints total key (receipt & journal)
*               PrtServTotalPost_VL() : prints total key (validation)
*               PrtServTotalPost_SP() : prints total key (slip)
*               PrtServTotalPost_SPVL() : prints total key (slip validation)
*               PrtSoftCheckTotal() : prints total key (soft check)
*               PrtSoftCheckTotal_RJ() : prints total key (receipt & journal)
*               PrtSoftCheckTotal_VL() : prints total key (validation)
*               PrtSoftCheckTotal_SP() : prints total key (slip)
*               PrtSoftCheckTotal_SPVL() : prints total key (slip validation)
*               PrtDflServTotal() : displays total key (service total)
*               PrtDflServTotalPost() : displays total key (service total)
*
*               PrtDflServTotalForm() : displays total key (service total)
*               PrtDflServTotalStubPostForm() : displays total key (service total)
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-04-92 : 00.00.01 : K.Nakajima :                                    
* Jul-13-93 : 01.00.12 : R.Itoh     : add PrtDflServTotal(), PrtDflServTotalPost()                                  
* Jul-28-93 : 01.00.12 : K.You      : add PrtSoftCheckTotal(), PrtSoftCheckTotal_RJ(),
*           :          :            : PrtSoftCheckTotal_VL(), PrtSoftCheckTotal_SP().
* Oct-28-93 : 02.00.02 : K.You      : del. PrtDflServTotal(), PrtDflServTotalStubPost().
* Apr-10-94 : 00.00.04 : K.You      : add validation slip print feature.(mod. PrtServTotal())
* Apr-25-94 : 00.00.05 : K.You      : bug fixed E-33,35,41(mod. PrtSoftCheckTotal_RJ, PrtSoftCheckTotal_SP)
* Apr-29-94 : 02.05.00 : K.You      : bug fixed E-77(mod. PrtServTotal_RJ, PrtServTotal_SP)
* May-16-94 : 02.05.00 : Yoshiko.Jim: bug fixed S-005 (mod PrtServTotal(), add. PrtServTotal_SPVL()
*                                   : mod PrtSoftCheckTotal(), add PrtSoftCheckTotal_SPVL())
* May-19-94 : 02.05.00 : K.You      : bug fixed Q-008 (mod PrtServTotal_SP(), PrtSoftCheckTotal_SP(),
*           :          :            : PrtServTotal_SPVL(), PrtSoftCheckTotal_SPVL())
* May-20-94 : 02.05.00 : K.You      : bug fixed S-026 (mod PrtServTotal_VL(), PrtSoftCheckTotal_VL()).
* Jan-17-95 : 03.00.00 : M.Ozawa    : add PrtDflServTotal(), PrtDflServTotalPost() 
* Jun-14-95 : 03.00.00 : T.Nakahata : add Total Mnemonic to Kitchen Feature
* Jul-21-95 : 03.00.04 : T.Nakahata : display customer name at 1st frame
* Jul-26-95 : 03.00.04 : T.Nakahata : print on journal by service total key
* May-09-96 : 03.05.05 : M.Ozawa    : add PrtServeTotalPost_SPVL() for bug fix
* Dec-14-99 : 00.00.01 : M.Ozawa    : add. PrtDflServTotalForm(), PrtDflServTotalStubPostForm()
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

/**------- 2170 local------**/
#include <ecr.h>
#include <rfl.h>
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


/**
;============================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;============================================================================
**/

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
    SHORT   fsPortSave;

    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( fsPrtStatus & PRT_REQKITCHEN ) {     /* kitchen print */
        if (( ! CliParaMDCCheck( MDC_KPTR_ADR, ODD_MDC_BIT0 )) && ( CliParaMDCCheck( MDC_KPTR_ADR, ODD_MDC_BIT3 ))) {
            /* --- if total mnemonic to kitchen option is ON, print it to kitchen --- */
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
            PrtServTotal_SPVL(pTran, pItem);        /* S-005 corr. '94.5.16 */
            PrtSPVLTrail(pTran);
			PmgEndSmallValidation( PMG_PRT_SLIP );//change to allow Small Slips
        } else {
            PrtServTotal_VL(pTran, pItem);
        }
        return ;
    }

    if ( fsPrtPrintPort & PRT_SLIP ) {        /* slip print */
        PrtServTotal_SP(pTran, pItem);
    }

    if (   (fsPrtPrintPort & PRT_RECEIPT) || (fsPrtPrintPort & PRT_JOURNAL)) {  /* receipt, journal print */
        PrtServTotal_RJ(pTran, pItem);
    }

    if (( ! ( fsPrtPrintPort & PRT_JOURNAL )) && ( fsPrtStatus & PRT_SERV_JOURNAL )) {
        fsPortSave = fsPrtPrintPort;
        fsPrtPrintPort = PRT_JOURNAL;
        PrtServTotal_RJ( pTran, pItem );
        fsPrtPrintPort = fsPortSave;
    }
}

/*
*===========================================================================
** Format  : VOID PrtServTotal_RJ(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
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
VOID  PrtServTotal_RJ(TRANINFORMATION  *pTran, ITEMTOTAL *pItem)
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

    PrtRFeed(1);                                    /* 1 line feed(Receipt) */

    auchApproval[0] = '\0';
    PrtRJOffline( pItem->fbModifier, pItem->auchExpiraDate, auchApproval );

    PrtRJNumber(pItem->aszNumber);                  /* number line */
    PrtJFolioPost(pItem->aszFolioNumber, pItem->aszPostTransNo);   /* for charge posting */

    if (((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT1) || ((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT2)) {
        /* print cashier interrupt mnemonic on journal, R3.3 */
        if (fsPrtPrintPort & PRT_RECEIPT) {
            PrtRJAmtSymP(RflChkTtlAdr(pItem), pItem->lService, sType1, PMG_PRT_RECEIPT);
        }
        if (fsPrtPrintPort & PRT_JOURNAL) {
            PrtRJAmtSymP(TRN_CASINT_ADR, pItem->lService, sType1, PMG_PRT_JOURNAL);
        }
    } else {
        PrtRJAmtSym(RflChkTtlAdr(pItem), pItem->lService, sType1);    /* serv. ttl */
    }

    if ((pTran->TranCurQual.fsCurStatus & CURQUAL_OPEMASK) !=  CURQUAL_NEWCHECK  ) {                /* not newcheck */
        if (pItem->lTax != 0L) {
            PrtRJAmtSym(TRN_TXSUM_ADR, pItem->lTax, PRT_SINGLE); /* tax sum */
        }

        PrtRJAmtSym(TRN_CKSUM_ADR, pItem->lMI, sType2);          /* check sum */
    }
    
    for (i = 0; i < NUM_CPRSPCO; i++) {
        PrtRJCPRspMsgText(pItem->aszCPMsgText[i]);      /* for charge posting */
    }

}
/*
*===========================================================================
** Format  : VOID PrtServTotal_VL(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
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
VOID PrtServTotal_VL(TRANINFORMATION *pTran, ITEMTOTAL *pItem)
{
    
    PmgBeginValidation(PMG_PRT_RCT_JNL);        /* begin validation */

    /* -- in case of 24 char printer -- */
    if ( usPrtVLColumn == PRT_VL_24 ) {
        PrtVLHead(pTran, 0);                    /* send header lines, not print "VOID"  */
        PrtVLNumberCP(pItem->aszNumber, pItem->auchExpiraDate);    /* send number line */
        PrtVLTotal(pItem, pItem->lService, RflChkTtlAdr(pItem));   /* amount line */
        PrtVLTrail(pTran);                                         /* send trailer lines */
    }

    /* -- in case of 21 char printer -- */
    if ( usPrtVLColumn == PRT_VL_21 ) {
        PrtVSSevTotal(pItem, pTran, pItem->lService);
    }

    PrtEndValidation(PMG_PRT_RCT_JNL);          /* end validation */
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
    
    /* -- set exp.date -- */
    auchApproval[0] = '\0';
    usSlipLine += PrtSPOffline(aszSPPrintBuff[usSlipLine], pItem->fbModifier, pItem->auchExpiraDate, auchApproval );

    /* -- set mnemonic and number -- */
    usSlipLine += PrtSPVoidNumber(aszSPPrintBuff[usSlipLine], pItem->fbModifier, pItem->usReasonCode, pItem->aszNumber);

    /* -- in case of new check -- */    
    if ((pTran->TranCurQual.fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_NEWCHECK) {   

        /* -- set service total -- */
        usSlipLine += PrtSPServiceTotal(aszSPPrintBuff[usSlipLine], RflChkTtlAdr(pItem), pItem->lService, sType1, pTran, usSlipLine);

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
            usSlipLine += PrtSPServiceTotal(aszSPPrintBuff[usSlipLine], RflChkTtlAdr(pItem), pItem->lService, (BOOL)sType1, pTran, (USHORT)(usSlipLine + usSaveLine));  /* ### Mod (2171 for Win32) V1.0 */
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
        usSlipLine += PrtSPServiceTotal(aszSPPrintBuff[usSlipLine], TRN_CKSUM_ADR, pItem->lMI, sType2, pTran, usSlipLine);
    
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
            usSlipLine += PrtSPServiceTotal(aszSPPrintBuff[usSlipLine], (UCHAR)TRN_CKSUM_ADR, pItem->lMI, (BOOL)sType2, pTran, (USHORT)(usSlipLine + usSaveLine)); /* ### Mod (2171 for Win32) V1.0 */
            usSlipLine += usCPRspLine;
        }    
    }    
    
    /* -- print all data in the buffer -- */ 
    for (i = 0; i < usSlipLine; i++) {
        PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);       
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
** Synopsis: This function prints total key (service total) for slip validation
*===========================================================================
*/
VOID PrtServTotal_SPVL(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem)
{
    TCHAR   auchApproval[1];
    TCHAR   aszSPPrintBuff[5][PRT_SPCOLUMN + 1];    /* print data save area */
    USHORT  i, usSaveLine, usSlipLine = 0;
    SHORT   sType1, sType2;                         /* double wide or single*/
    
    memset(aszSPPrintBuff[0], '\0', sizeof(aszSPPrintBuff));
    if (pTran->TranCurQual.auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {
        sType1 = PRT_DOUBLE;
    } else {
        sType1 = PRT_SINGLE;
    }
                                                    /* check sum double ?   */
    if (pTran->TranCurQual.flPrintStatus & CURQUAL_SINGLE_CHCK_SUM) {
        sType2 = PRT_SINGLE;
    } else {
        sType2 = PRT_DOUBLE;
    }
    auchApproval[0] = '\0';                         /* set exp.date         */
    usSlipLine += PrtSPOffline(aszSPPrintBuff[usSlipLine], pItem->fbModifier, pItem->auchExpiraDate, auchApproval );

    usSlipLine += PrtSPVoidNumber(aszSPPrintBuff[usSlipLine], pItem->fbModifier, pItem->usReasonCode, pItem->aszNumber);  /* new check operation? */    
    if ((pTran->TranCurQual.fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_NEWCHECK) {   
                                                    /* set service total    */
        usSlipLine += PrtSPMnemNativeAmt(aszSPPrintBuff[usSlipLine], RflChkTtlAdr(pItem), pItem->lService, sType1);
                                                    
        usSaveLine = PrtCheckLine(usSlipLine, pTran);   /* paper change ?   */ 
        if (usSaveLine != 0) {   
            usSlipLine --;                          /* override the data    */
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
                                                     /* check paper change  */ 
        usSaveLine = PrtCheckLine(usSlipLine, pTran);
        if (usSaveLine != 0) {
            usSlipLine --;                          /* override the data    */
            usSlipLine += PrtSPMnemNativeAmt(aszSPPrintBuff[usSlipLine], TRN_CKSUM_ADR, pItem->lMI, sType2);
        }    
    }    
    for (i = 0; i < usSlipLine; i++) {              /* print all data       */ 
        PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);       
    }
                                                    
    usPrtSlipPageLine += usSlipLine + usSaveLine;   /* update current line# */
    usPrtSlipPageLine++;                            /* K  */
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
    SHORT fsPortSave;

    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( fsPrtStatus & PRT_REQKITCHEN ) {     /* kitchen print */
        if (( ! CliParaMDCCheck( MDC_KPTR_ADR, ODD_MDC_BIT0 )) && ( CliParaMDCCheck( MDC_KPTR_ADR, ODD_MDC_BIT3 ))) {
            /* --- if total mnemonic to kitchen option is ON, print it to kitchen --- */
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
        } else {
            PrtServTotalPost_VL(pTran, pItem);
        }
        return ;
    }

    if ( fsPrtPrintPort & PRT_SLIP ) {        /* slip print */
        PrtServTotalPost_SP(pTran, pItem);
    }

    if ((fsPrtPrintPort & PRT_RECEIPT) || (fsPrtPrintPort & PRT_JOURNAL)) {  /* receipt, journal print */
        PrtServTotalPost_RJ(pTran, pItem);
    }

    if (( ! ( fsPrtPrintPort & PRT_JOURNAL )) && ( fsPrtStatus & PRT_SERV_JOURNAL )) {
        fsPortSave = fsPrtPrintPort;
        fsPrtPrintPort = PRT_JOURNAL;

        PrtServTotalPost_RJ( pTran, pItem );

        fsPrtPrintPort = fsPortSave;
    }
}

/*
*===========================================================================
** Format  : VOID PrtServTotalPost_RJ(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
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
VOID  PrtServTotalPost_RJ(TRANINFORMATION  *pTran, ITEMTOTAL *pItem)
{
    SHORT sType;

    if (pTran->TranCurQual.flPrintStatus & CURQUAL_SINGLE_CHCK_SUM) {
        sType = PRT_SINGLE;
    } else {
        sType = PRT_DOUBLE;
    }

    PrtRFeed(1);                                    /* 1 line feed(Receipt) */

    PrtRJNumber(pItem->aszNumber);                  /* number line */

    if (((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT1) || ((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT2)) {

        /* print cashier interrupt mnemonic on journal, R3.3 */
        if (fsPrtPrintPort & PRT_RECEIPT) {
            PrtRJAmtSymP(TRN_CKSUM_ADR, pItem->lService, sType, PMG_PRT_RECEIPT);
        }
        if (fsPrtPrintPort & PRT_JOURNAL) {
            PrtRJAmtSymP(TRN_CASINT_ADR, pItem->lService, sType, PMG_PRT_JOURNAL);
        }
    } else {
        PrtRJAmtSym(TRN_CKSUM_ADR, pItem->lMI, sType);  /* check sum */
    }
}                                                   

/*
*===========================================================================
** Format  : VOID PrtServTotalPost_VL(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
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
VOID PrtServTotalPost_VL(TRANINFORMATION *pTran, ITEMTOTAL *pItem)
{
    SHORT   sType;

    if (pTran->TranCurQual.flPrintStatus & CURQUAL_SINGLE_CHCK_SUM) {
        sType = PRT_SINGLE;
    } else {
        sType = PRT_DOUBLE;
    }

    PmgBeginValidation(PMG_PRT_RCT_JNL);        /* begin validation */

    /* -- in case of 24 char printer -- */
    if ( usPrtVLColumn == PRT_VL_24 ) {
        PrtVLHead(pTran, 0);                    /* send header lines, not print "VOID"  */
        PrtVLNumber(pItem->aszNumber);                  /* send number line */
        PrtVLAmtSym(TRN_CKSUM_ADR, pItem->lMI, sType);  /* amount line */
        PrtVLTrail(pTran);                              /* send trailer lines */
    }

    /* -- in case of 21 char printer -- */
    if ( usPrtVLColumn == PRT_VL_21 ) {
        PrtVSSevTotalPost(pItem, pTran, sType);
    }

    PrtEndValidation(PMG_PRT_RCT_JNL);          /* end validation */
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
    usSlipLine += PrtSPServiceTotal(aszSPPrintBuff[usSlipLine], TRN_CKSUM_ADR, pItem->lMI, sType1, pTran, usSlipLine);
    
    /* -- check if paper change is necessary or not -- */ 
    usSaveLine = PrtCheckLine(usSlipLine, pTran);

    /* -- if paper changed, re-set page, line, consecutive No. -- */    
    if (usSaveLine != 0) {
        usSlipLine --;      /* override the data in the last buffer */
        usSlipLine += PrtSPServiceTotal(aszSPPrintBuff[usSlipLine], (UCHAR)TRN_CKSUM_ADR, pItem->lMI, (BOOL)sType1, pTran, (USHORT)(usSlipLine + usSaveLine)); /* ### Mod (2171 for Win32) V1.0 */
    }    
    
    /* -- print all data in the buffer -- */ 
    for (i = 0; i < usSlipLine; i++) {
        PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);       
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
        PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);       
    }

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;        
    usPrtSlipPageLine++;                    /* K */
}

/*
*===========================================================================
** Format  : VOID   PrtServTotalStubPost(ITEMTOTAL *pItem);      
*
*   Input  : ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints service total(post check)  key stub 
*===========================================================================
*/
VOID   PrtServTotalStubPost(ITEMTOTAL *pItem)
{
    SHORT   sType;

    /* -- total amount printed double wide? -- */
    if (pItem->auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {
        sType = PRT_DOUBLE;
    } else {
        sType = PRT_SINGLE;
    }

    PrtRJAmtSymP(TRN_CKSUM_ADR, pItem->lMI, sType, PMG_PRT_RECEIPT);
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
    SHORT fsPortSave;

    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( fsPrtStatus & PRT_REQKITCHEN ) {     /* kitchen print */
        if (( ! CliParaMDCCheck( MDC_KPTR_ADR, ODD_MDC_BIT0 )) &&
            ( CliParaMDCCheck( MDC_KPTR_ADR, ODD_MDC_BIT3 ))) {
            /* --- if total mnemonic to kitchen option is ON, print it to kitchen --- */
            PrtKPTotal(pItem);
        }
    }

    if ( pItem->fsPrintStatus & PRT_VALIDATION ) {           /* validation print */
        if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) {
            if(PRT_SUCCESS != PrtSPVLInit())
			{
				return;
			}
			PmgBeginSmallValidation( PMG_PRT_SLIP );//change to allow Small Slips
            PrtSPVLHead(pTran);    
            PrtSoftCheckTotal_SPVL(pTran, pItem);
            PrtSPVLTrail(pTran);
			PmgEndSmallValidation( PMG_PRT_SLIP );//change to allow Small Slips
        } else {
            PrtSoftCheckTotal_VL(pTran, pItem);
        }
        return ;
    }

    if ( fsPrtPrintPort & PRT_SLIP ) {        /* slip print */
        PrtSoftCheckTotal_SP(pTran, pItem);
    }

    if ((fsPrtPrintPort & PRT_RECEIPT) || (fsPrtPrintPort & PRT_JOURNAL)) {  /* receipt, journal print */

        PrtSoftCheckTotal_RJ(pTran, pItem);
    }

    if (( ! ( fsPrtPrintPort & PRT_JOURNAL )) && ( fsPrtStatus & PRT_SERV_JOURNAL )) {
        fsPortSave = fsPrtPrintPort;
        fsPrtPrintPort = PRT_JOURNAL;

        PrtSoftCheckTotal_RJ( pTran, pItem );

        fsPrtPrintPort = fsPortSave;
    }
}

/*
*===========================================================================
** Format  : VOID PrtSoftCheckTotal_RJ(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints total key (soft check total)
*===========================================================================
*/
VOID  PrtSoftCheckTotal_RJ(TRANINFORMATION  *pTran, ITEMTOTAL *pItem)
{
    TCHAR   auchApproval[1];
    SHORT   sType;
    USHORT  i;


    /* amount double wide? */
    if (pTran->TranCurQual.auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {
        sType = PRT_DOUBLE;
    } else {
        sType = PRT_SINGLE;
    }

    PrtRFeed(1);                                    /* 1 line feed(Receipt) */

    auchApproval[0] = '\0';
    PrtRJOffline( pItem->fbModifier, pItem->auchExpiraDate, auchApproval );

    PrtRJNumber(pItem->aszNumber);                  /* number line */
    
    pItem->uchMinorClass -= CLASS_SOFTCHK_BASE_TTL;
    if (((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT1) || ((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT2)) {
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
    pItem->uchMinorClass += CLASS_SOFTCHK_BASE_TTL;

    PrtJFolioPost(pItem->aszFolioNumber, pItem->aszPostTransNo);   /* for charge posting */

    for (i = 0; i < NUM_CPRSPCO; i++) {
        PrtRJCPRspMsgText(pItem->aszCPMsgText[i]);      /* for charge posting */
    }
}
/*
*===========================================================================
** Format  : VOID PrtSoftCheckTotal_VL(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints total key (soft check total)
*===========================================================================
*/
VOID PrtSoftCheckTotal_VL(TRANINFORMATION *pTran, ITEMTOTAL *pItem)
{
    SHORT   sType;                      /* double wide or single */

    if (pTran->TranCurQual.auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {
        sType = PRT_DOUBLE;
    } else {
        sType = PRT_SINGLE;
    }

    PmgBeginValidation(PMG_PRT_RCT_JNL);        /* begin validation */

    /* -- in case of 24 char printer -- */
    if ( usPrtVLColumn == PRT_VL_24 ) {
        PrtVLHead(pTran, 0);                    /* send header lines, not print "VOID"  */

        PrtVLNumberCP(pItem->aszNumber, pItem->auchExpiraDate); /* send number line */

        pItem->uchMinorClass -= CLASS_SOFTCHK_BASE_TTL;
        PrtVLAmtSym(RflChkTtlAdr(pItem), pItem->lMI, sType);
        pItem->uchMinorClass += CLASS_SOFTCHK_BASE_TTL;

        PrtVLTrail(pTran);                      /* send trailer lines */
    }

    /* -- in case of 21 char printer -- */
    if ( usPrtVLColumn == PRT_VL_21 ) {
        PrtVSSevTotal(pItem, pTran, pItem->lMI);
    }

    PrtEndValidation(PMG_PRT_RCT_JNL);          /* end validation */
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
** Synopsis: This function prints total key (soft check total)
*===========================================================================
*/
VOID PrtSoftCheckTotal_SP(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem)
{
    TCHAR   auchApproval[1];
    TCHAR   aszSPPrintBuff[3 + NUM_CPRSPCO_EPT][PRT_SPCOLUMN + 1]; /* print data save area */
    SHORT   sType;                      /* double wide or single */
    USHORT  usSlipLine = 0;             /* number of lines to be printed */
    USHORT  usCPRspLine = 0;            /* number of CP response line to be printed */
    USHORT  usCPRspTmpLine = 0;         /* number of CP response temporary line to be printed */
    USHORT  usSaveLine;                 /* save slip lines to be added */
    USHORT  i;
    
    /* -- initialize the buffer -- */    
    memset(aszSPPrintBuff[0], '\0', sizeof(aszSPPrintBuff));

    /* -- service total amount double wide? -- */
    if (pTran->TranCurQual.auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {
        sType = PRT_DOUBLE;
    } else {
        sType = PRT_SINGLE;
    }

    /* -- set exp.date -- */
    auchApproval[0] = '\0';
    usCPRspTmpLine = PrtSPOffline(aszSPPrintBuff[usSlipLine], pItem->fbModifier, pItem->auchExpiraDate, auchApproval );
    usSlipLine += usCPRspTmpLine;
/*    usCPRspLine += usCPRspTmpLine; */

    /* -- set mnemonic and number -- */
    usCPRspTmpLine = PrtSPVoidNumber(aszSPPrintBuff[usSlipLine], pItem->fbModifier, pItem->usReasonCode, pItem->aszNumber);
    usSlipLine += usCPRspTmpLine;
/*    usCPRspLine += usCPRspTmpLine; */
    
    /* -- set service total -- */

    pItem->uchMinorClass -= CLASS_SOFTCHK_BASE_TTL;
    usSlipLine += PrtSPServiceTotal(aszSPPrintBuff[usSlipLine], RflChkTtlAdr(pItem), pItem->lMI, sType, pTran, usSlipLine);
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
        usSlipLine --;        /* override the data in the last buffer */
        usSlipLine -= usCPRspLine;
        usSlipLine += PrtSPServiceTotal(aszSPPrintBuff[usSlipLine], RflChkTtlAdr(pItem), pItem->lMI, sType, pTran, (USHORT)(usSlipLine + usSaveLine));  /* ### Mod (2171 for Win32) V1.0 */
        usSlipLine += usCPRspLine;
    }
    
    /* -- print all data in the buffer -- */ 
    for (i = 0; i < usSlipLine; i++) {
        PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);       
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
** Synopsis: This function prints total key (soft check total) for slip validation.
*===========================================================================
*/
VOID PrtSoftCheckTotal_SPVL(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem)
{
    TCHAR   auchApproval[1];
    TCHAR   aszSPPrintBuff[3][PRT_SPCOLUMN + 1];    /* print data save area */
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
    usCPRspTmpLine                                  
        = PrtSPOffline(aszSPPrintBuff[usSlipLine], pItem->fbModifier, pItem->auchExpiraDate, auchApproval );

    usSlipLine += usCPRspTmpLine;
/*    usCPRspLine += usCPRspTmpLine; */

    usCPRspTmpLine = PrtSPVoidNumber(aszSPPrintBuff[usSlipLine], pItem->fbModifier, pItem->usReasonCode, pItem->aszNumber);    /* set mnemo & number   */

    usSlipLine += usCPRspTmpLine;
/*    usCPRspLine += usCPRspTmpLine; */

    pItem->uchMinorClass -= CLASS_SOFTCHK_BASE_TTL;     /* set service ttl  */
    usSlipLine += PrtSPMnemNativeAmt(aszSPPrintBuff[usSlipLine], RflChkTtlAdr(pItem), pItem->lMI, sType);
    pItem->uchMinorClass += CLASS_SOFTCHK_BASE_TTL;     /* set service ttl  */
    
    usSaveLine = PrtCheckLine(usSlipLine, pTran);       /* paper change ?   */ 
    if (usSaveLine != 0) {   
        usSlipLine --;                              /* override last buffer */
        usSlipLine -= usCPRspLine;
        usSlipLine += PrtSPMnemNativeAmt(aszSPPrintBuff[usSlipLine], RflChkTtlAdr(pItem), pItem->lMI, sType);
        usSlipLine += usCPRspLine;
    }
    
    for (i = 0; i < usSlipLine; i++) {                  /* print all data   */ 
        PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);       
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
    TCHAR  aszDflBuff[24][PRT_DFL_LINE + 1];   /* display data save area */
    USHORT  usLineNo;                       /* number of lines to be displayed */
    USHORT  usOffset = 0;                       
    USHORT  i;                       
    SHORT   sType1, sType2;
    TCHAR   auchApproval[1];

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
        if (((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT1) || ((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT2)) {
            /* send cashier interrupt mnemonic, R3.3 */
            usLineNo += PrtDflAmtSym(aszDflBuff[usLineNo], TRN_CASINT_ADR, pItem->lMI, sType1);
        } else {
            usLineNo += PrtDflAmtSym(aszDflBuff[usLineNo], RflChkTtlAdr(pItem), pItem->lMI, sType1);
        }
    } else {
        if (((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT1) || ((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT2)) {
            /* send cashier interrupt mnemonic, R3.3 */
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
    if (((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT1) || ((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT2)) {
        /* send cashier interrupt mnemonic, R3.3 */
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
    usLineNo += PrtDflOffline(aszDflBuff[usLineNo], pItem->fbModifier, pItem->auchExpiraDate, auchApproval );

    /* -- set item data -- */
    usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber);  
    usLineNo += PrtDflFolioPost(aszDflBuff[usLineNo], pItem->aszFolioNumber, pItem->aszPostTransNo);    /* for charge posting */

    if (((pTran->TranCurQual.flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_POSTCHECK ) &&
        ( CliParaMDCCheck( MDC_GCNO_ADR, EVEN_MDC_BIT3 ) == 0) &&
        ( CliParaMDCCheck( MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2 ) == 0)) {

        /* split check system service total */
        if (((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT1) || ((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT2)) {

            /* print cashier interrupt mnemonic on journal, R3.3 */
            usLineNo += PrtDflAmtSym(aszDflBuff[usLineNo], TRN_CASINT_ADR, pItem->lMI, sType1);
        } else {
            usLineNo += PrtDflAmtSym(aszDflBuff[usLineNo], RflChkTtlAdr(pItem), pItem->lMI, sType1);
        }
    } else {
        if (((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT1) || ((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT2)) {
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
    TCHAR  aszDflBuff[5][PRT_DFL_LINE + 1];   /* display data save area */
    USHORT  usLineNo=0, i;                       /* number of lines to be displayed */
    USHORT  usOffset = 0;                       
    SHORT   sType;

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
    if (((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT1) || ((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == PRT_CASINT2)) {
        /* print cashier interrupt mnemonic on journal, R3.3 */
        usLineNo += PrtDflAmtSym(aszDflBuff[usLineNo], TRN_CASINT_ADR, pItem->lMI, sType); 
    } else {
        usLineNo += PrtDflAmtSym(aszDflBuff[usLineNo], TRN_CKSUM_ADR, pItem->lMI, sType); 
    }
    
    for (i=0; i<usLineNo; i++) {
        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }
    _tcsncpy(puchBuffer, aszDflBuff[0], usLineNo*(PRT_DFL_LINE+1));
    
    return usLineNo;
}

/***** End Of Source *****/

