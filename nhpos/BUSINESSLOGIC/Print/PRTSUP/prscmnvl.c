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
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print common routine, super/prog, validation 55 Char
* Category    : Print, NCR 2170 GP R2.0 Application
* Program Name: PrSCmnVL.C
*               | ||  ||
*               | ||  |+- S:21Char L:24Char _:not influcenced by print column
*               | ||  +-- R:recept & jounal S:slip V:validation K:kitchen
*               | |+----- comon routine
*               | +------ R:reg mode S:super mode
*               +-------- Pr:print module
* --------------------------------------------------------------------------
* Abstract: The provided function whose names are as follows:
*
*     PrtSupVLHead()        : validation header
*     PrtSupVLHead1()       : validation header 1st line
*     PrtSupVLHead2()       : validation header 2nd line
*     PrtSupVLHead3()       : validation header 3rd line
*     PrtSupVLTrail()       : validation trailer
*     PrtSupVLCashNo()      : cashier no print
*     PrtSupVLLoanPickup()  : loan/pickup print
*     PrtSupVLForQty()      : for and quantity
*     PrtSupVLForeignTend() : tender 2nd line (foreign tender)
*
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
* Mar-17-93 : 00.00.01 : M.Ozawa    : Initial for GP
* Oct-15-96 : 02.00.00 :  S.Kubota  : Change for Foreign Currency #3-8
* Dec-03-99 : 01.00.00 : hrkato     : Saratoga
*===========================================================================
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
;+                  I N C L U D E     F I L E s
;========================================================================
**/
/* -- MS  -  C --*/
#include	<tchar.h>
#include<string.h>
#include<stdlib.h>

/* -- 2170 local --*/
#include<ecr.h>
#include<regstrct.h>
#include<transact.h>
#include<paraequ.h>
#include<para.h>
#include<pmg.h>
#include<maint.h>
#include<csstbpar.h>
#include<csttl.h>
#include<csop.h>
#include<report.h>
#include<rfl.h>
#include"prtrin.h"
#include"prtsin.h"
#include"prrcolm_.h"

/**
;========================================================================
;+                    S T A T I C    R A M s
;========================================================================
**/
const TCHAR   aszPrtSupVLHead[] = _T("     %-12s %6s         %-10s %6s");  /* header */
CONST TCHAR   aszPrtSupVLTrail[] = _T("%04u %8.8Mu %-4s%04u %-4s%04u %04u-%03u\t%s");  /* trailer */
CONST TCHAR   aszPrtSupVLQty[] = _T("%9ld X\t %s ");            /* qty */
CONST TCHAR   aszPrtSupVLFor[] = _T("%9ld / ");                 /* for */
CONST TCHAR   aszPrtSupVLForeign[] = _T("%.*l$");               /* foreign amount */
CONST TCHAR   aszPrtSupVLForeignTend[] = _T("%s / %10.*l$     %-8.8s\t %l$");/* foreign tender 2nd line */ /*8 characters JHHJ*/
extern CONST TCHAR   aszPrtVLHead[];
/* CONST UCHAR FARCONST  aszPrtVLHead[] = _T("%s%s"); */
CONST TCHAR   aszPrtVLMnemAmt[] = _T("%s \t %l$");              /* mnemo. and amount */
CONST TCHAR   aszPrtVLAmtTmp[]  = _T("%l$");                    /* amount */
CONST TCHAR   aszPrtVLDoubleMnemAmt[] = _T("%-s\t%s");          /* trans. mnem and amount */

/**
;========================================================================
;+             P R O G R A M    D E C L A R A T I O N s
;========================================================================
**/
/*
*===========================================================================
** Format  : VOID  PrtSupVLHead(UCHAR uchAdr, USHORT usACNumber, USHORT usCashierNo, UCHAR *pszCashMnemo);
*
*    Input : UCHAR   uchAdr       -address of report name
*            USHORT  usACNumber   -action code
*            USHORT  usCashierNO     -Cashier No
*            UCHAR   aszCashMnemo    -Cashier Name
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints validation header lines.
*===========================================================================
*/
VOID  PrtSupVLHead(MAINTSPHEADER *pData)
{
    static const TCHAR FARCONST auchNumber[] = _T("%8.8Mu");
    PARAREPORTNAME ParaReportName;
	TCHAR   aszDoubRepoName[PARA_REPORTNAME_LEN * 2 + 1] = { 0 };
	TCHAR   aszDoubRepoNumb[8 * 2 + 1] = { 0 };
	TCHAR   aszRepoNumb[8 + 1] = { 0 };
	TCHAR  aszDoubCashNumb[8 * 2 + 1] = { 0 };
	TCHAR  aszCashNumb[8 + 1] = { 0 };

    ParaReportName.uchMajorClass = CLASS_PARAREPORTNAME;
    ParaReportName.uchAddress = RPT_ACT_ADR;
    CliParaRead(&ParaReportName);
    memset(aszDoubRepoName, '\0', sizeof(aszDoubRepoName));
    PrtDouble(aszDoubRepoName, TCHARSIZEOF(aszDoubRepoName), ParaReportName.aszMnemonics);

    /* make double wide for report number */

    RflSPrintf(aszRepoNumb, TCHARSIZEOF(aszRepoNumb), auchNumber, pData->usACNumber);
    memset(aszDoubRepoNumb, '\0', sizeof(aszDoubRepoNumb));
    PrtDouble(aszDoubRepoNumb, TCHARSIZEOF(aszDoubRepoNumb), aszRepoNumb);

    /* make double wide for cashier number */
    RflSPrintf(aszCashNumb, TCHARSIZEOF(aszCashNumb), auchNumber, RflTruncateEmployeeNumber(pData->ulCashierNo));
    memset(aszDoubCashNumb, '\0', sizeof(aszDoubCashNumb));
    PrtDouble(aszDoubCashNumb, TCHARSIZEOF(aszDoubCashNumb), aszCashNumb);

    PmgPrintf(PMG_PRT_RCT_JNL, aszPrtSupVLHead, aszDoubRepoName, aszDoubRepoNumb, pData->aszMnemonics, aszDoubCashNumb);
}

/*
*===========================================================================
** Format  : VOID  PrtSupVLTrail(TRNINFORMATION *pTran);
*
*    Input : none
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints validation trailer.
*===========================================================================
*/
VOID  PrtSupVLTrail(TRANINFORMATION *pTran, MAINTSPHEADER *pData)
{
	TCHAR  aszDayRstMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };
	TCHAR  aszPTDRstMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };
	TCHAR  aszDate[PRT_DATETIME_LEN + 1] = { 0 };


    /* set EOD reset counter mnemonics */
	RflGetSpecMnem(aszDayRstMnem, SPC_DAIRST_ADR);

    /* set PTD reset counter mnemonics */
	RflGetSpecMnem(aszPTDRstMnem, SPC_PTDRST_ADR);

    /* -- get date and time -- */
    PrtGetDate(aszDate, TCHARSIZEOF(aszDate), pTran);

    /* print out */
    PmgPrintf(PMG_PRT_RCT_JNL, aszPrtSupVLTrail, pData->usConsCount, RflTruncateEmployeeNumber(pData->ulSuperNumber),
                               aszDayRstMnem, pData->usDayRstCount, aszPTDRstMnem, pData->usPTDRstCount,
                               pData->usStoreNumber, pData->usRegNumber, aszDate);
}

/*
*===========================================================================
** Format  : VOID  PrtSupVLLoanPickup(MAINTLOANPICKUP *pData);
*
*    Input : MAINTLOANPICKUP *pData -loan pickup data address
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints prints transaction mnemonics
*            and Amount line.
*===========================================================================
*/
VOID    PrtSupVLLoanPickup(TRANINFORMATION *pTran, MAINTLOANPICKUP *pData)
{
    PrtSupVLModifier(pTran, pData->usModifier, 0); /* void, e/c print */
    PrtSupVLForQty(pData);                      /* for, qty print */

    switch ( pData->uchMinorClass ) {
    case CLASS_MAINTFOREIGN1:
    case CLASS_MAINTFOREIGN2:
    case CLASS_MAINTFOREIGN3:
    case CLASS_MAINTFOREIGN4:
    case CLASS_MAINTFOREIGN5:
    case CLASS_MAINTFOREIGN6:
    case CLASS_MAINTFOREIGN7:
    case CLASS_MAINTFOREIGN8:
        PrtSupVLForeignTend(pData);             /* foreign loan/pickup line */
        break;

    case CLASS_MAINTCOUNTUP:                    /* loan/pickup total print */
        /* -- send mnemoncis address and amount -- */
        PrtVLMnemAmt(PrtChkLoanPickupAdr( pData->uchMajorClass, pData->uchMinorClass ), pData->lTotal, PRT_SINGLE);
        break;

    default:
        /* -- send mnemoncis address and amount -- */
        PrtVLMnemAmt( PrtChkLoanPickupAdr( pData->uchMajorClass, pData->uchMinorClass ), pData->lAmount, PRT_SINGLE);
    }                                           /* R2.0 End   */
}

/*
*===========================================================================
** Format  : VOID  PrtSupVLForQty(USHORT usModifier, LONG lForQty, LONG lAmount);
*
*    Input : USHORT usModifier   -Modifier Key Status
*          : LONG   lForQty      -For Qty Amount
*          : LONG   lAmount      -Amount
*
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints prints number, qty and unitprice
*===========================================================================
*/
VOID  PrtSupVLForQty(MAINTLOANPICKUP *pData)
{
    if ( pData->usModifier & MAINT_MODIFIER_QTY ) {
		TCHAR  aszSpecAmt[PARA_SPEMNEMO_LEN + PRT_AMOUNT_LEN + 1] = { 0 };
		UCHAR  uchFCAdr;
                                                                /* R2.0 End   */
        switch ( pData->uchMinorClass ) {
        case CLASS_MAINTFOREIGN1:
        case CLASS_MAINTFOREIGN2:
        case CLASS_MAINTFOREIGN3:
        case CLASS_MAINTFOREIGN4:
        case CLASS_MAINTFOREIGN5:
        case CLASS_MAINTFOREIGN6:
        case CLASS_MAINTFOREIGN7:
        case CLASS_MAINTFOREIGN8:

            switch(pData->uchMinorClass){
            case CLASS_MAINTFOREIGN1:
                uchFCAdr  = SPC_CNSYMFC1_ADR;
                break;

            case CLASS_MAINTFOREIGN2:
                uchFCAdr  = SPC_CNSYMFC2_ADR;
                break;

            case CLASS_MAINTFOREIGN3:
                uchFCAdr  = SPC_CNSYMFC3_ADR;
                break;

            case CLASS_MAINTFOREIGN4:
                uchFCAdr  = SPC_CNSYMFC4_ADR;
                break;

            case CLASS_MAINTFOREIGN5:
                uchFCAdr  = SPC_CNSYMFC5_ADR;
                break;

            case CLASS_MAINTFOREIGN6:
                uchFCAdr  = SPC_CNSYMFC6_ADR;
                break;

            case CLASS_MAINTFOREIGN7:
                uchFCAdr  = SPC_CNSYMFC7_ADR;
                break;

            case CLASS_MAINTFOREIGN8:
                uchFCAdr  = SPC_CNSYMFC8_ADR;
                break;
            }

            /* -- adjust foreign mnemonic and amount sign(+.-) -- */
            PrtAdjustForeign(aszSpecAmt, pData->lUnitAmount, uchFCAdr, pData->uchFCMDC);
            break;

        default:
           /* -- adjust native mnemonic and amount sign(+.-) -- */
           PrtAdjustNative(aszSpecAmt, pData->lUnitAmount);
        }                                                       /* R2.0 End   */

        PmgPrintf(PMG_PRT_RCT_JNL, aszPrtSupVLQty, pData->lForQty, aszSpecAmt);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtSupVLForeignTend(MAINTLOANPICKUP *pData);
*
*    Input : MAINTLOANPICKUP  *pData           -loan,pickup data
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints foreign tender 2nd line.
*===========================================================================
*/
VOID  PrtSupVLForeignTend(MAINTLOANPICKUP *pData)
{
    UCHAR  uchSymAdr;
    USHORT usTrnsAdr;
	TCHAR  aszFAmt[PRT_VLCOLUMN] = { 0 };
    USHORT usStrLen;
	TCHAR  aszFSym[PARA_SPEMNEMO_LEN + 1] = { 0 };          /* PARA_... defined in "paraequ.h" */
	TCHAR  aszTendMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };    /* PARA_... defined in "paraequ.h" */
    SHORT  sDecPoint, sDecPoint2;

    switch(pData->uchMinorClass){
    case CLASS_MAINTFOREIGN1:
        uchSymAdr  = SPC_CNSYMFC1_ADR;
        usTrnsAdr = (TRN_FT1_ADR);
        break;

    case CLASS_MAINTFOREIGN2:
        uchSymAdr  = SPC_CNSYMFC2_ADR;
        usTrnsAdr = (TRN_FT2_ADR);
        break;

    case CLASS_MAINTFOREIGN3:
        uchSymAdr  = SPC_CNSYMFC3_ADR;
        usTrnsAdr = (TRN_FT3_ADR);
        break;

    case CLASS_MAINTFOREIGN4:
        uchSymAdr  = SPC_CNSYMFC4_ADR;
        usTrnsAdr = (TRN_FT4_ADR);
        break;

    case CLASS_MAINTFOREIGN5:
        uchSymAdr  = SPC_CNSYMFC5_ADR;
        usTrnsAdr = (TRN_FT5_ADR);
        break;

    case CLASS_MAINTFOREIGN6:
        uchSymAdr  = SPC_CNSYMFC6_ADR;
        usTrnsAdr = (TRN_FT6_ADR);
        break;

    case CLASS_MAINTFOREIGN7:
        uchSymAdr  = SPC_CNSYMFC7_ADR;
        usTrnsAdr = (TRN_FT7_ADR);
        break;

    case CLASS_MAINTFOREIGN8:
        uchSymAdr  = SPC_CNSYMFC8_ADR;
        usTrnsAdr = (TRN_FT8_ADR);
        break;
    }

	// Check the MDC settings that have been retrieved for this foreign currency tender key
	// and set the bits for the display correctly.  Default is 2 decimal places.
    if ( pData->uchFCMDC & PRT_FC_MASK ) {             /* set decimal position */
        if ( (pData->uchFCMDC & PRT_FC_MASK) == PRT_FC_DECIMAL_0 ) {
            sDecPoint = PRT_NODECIMAL;
        } else {
            sDecPoint = PRT_3DECIMAL;
        }
    } else {
        sDecPoint = PRT_2DECIMAL;
    }

	RflGetSpecMnem(aszFSym, uchSymAdr);
	RflGetTranMnem(aszTendMnem, usTrnsAdr);

    usStrLen = _tcslen(aszFSym);                         /* get foreign symbol length */

    RflSPrintf(&aszFAmt[usStrLen], TCHARSIZEOF(aszFAmt), aszPrtSupVLForeign, sDecPoint, pData->lAmount);

    /* -- adjust sign ( + , - ) -- */
    if ( pData->lNativeAmount < 0 ) {
        aszFAmt[0] = _T('-');
        _tcsncpy(&aszFAmt[1], aszFSym, usStrLen);
    } else {
        _tcsncpy(&aszFAmt[0], aszFSym, usStrLen);
    }

    /* 6 digit decimal point for Euro, V2.2 */
    if (pData->uchFCMDC2 & ODD_MDC_BIT0) {
        sDecPoint2 = PRT_6DECIMAL;
    } else {
        sDecPoint2 = PRT_5DECIMAL;
    }

    PmgPrintf(PMG_PRT_RCT_JNL, aszPrtSupVLForeignTend, aszFAmt, sDecPoint2, (DCURRENCY)pData->ulFCRate, aszTendMnem, pData->lNativeAmount);
}

/*
*===========================================================================
** Format  : VOID   PrtSupVLModifier(TRANINFORMATION *pTran, USHORT usModifier)
*               
*    Input :     
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints validation header line.
*===========================================================================
*/
VOID    PrtSupVLModifier(TRANINFORMATION *pTran, USHORT fbMod, USHORT usReasonCode)
{
	TCHAR   aszVoid[PARA_CHAR24_LEN + 1] = { 0 };      /* void */
	TCHAR   aszHead3[PARA_CHAR24_LEN + 1] = { 0 };     /* valid. header 1st line */

    /* --- void ? -- */
    if (fbMod & VOID_MODIFIER) {
		if (0 > PrtGetReturns(fbMod, usReasonCode, aszVoid, TCHARSIZEOF(aszVoid))) {
			/* -- get void mnemonics instead of returns mnemonic -- */
			PrtGetVoid(aszVoid,  TCHARSIZEOF(aszVoid));     /* get void */
		}
    }

    PrtGet24Mnem(aszHead3,    CH24_3RDVAL_ADR);    /* get validation head */
    PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLHead, aszVoid, aszHead3);
}

/*
*===========================================================================
** Format  : VOID  PrtVLMnemAmt(UCHAR uchAdr, LONG lAmout, BOOL fsType);
*
*    Input : UCHAR uchAdr          -Transacion mnemonics address
*            LONG  lAmount         -Amount
*            BOOL        fsType       -character type
*                                     PRT_SINGLE: single character
*                                     PRT_DOUBLE: double character
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints prints transaction mnemonics
*            and Amount line.
*
*  Print format :
*
*       0        1         2         3         4         5
*       1234567890123456789012345678901234567890123456789012345
*      |^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
*      |MMMMMMMM                                     S99999.99 |
*      |                                                       |
*       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
*===========================================================================
*/
VOID    PrtVLMnemAmt(USHORT usAdr, DCURRENCY lAmount, BOOL fsType)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};
    TCHAR  aszTranMnemD[PARA_TRANSMNEMO_LEN*2 + 1] = {0};
    TCHAR  aszAmountS[PRT_AMOUNT_LEN + 1] = {0};
    TCHAR  aszAmountD[PRT_AMOUNT_LEN*2 + 1] = {0};

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- menmonic and amount printed double wide ? -- */
    if (fsType == PRT_DOUBLE ) {
        /* -- convert single char. to double -- */
        PrtDouble(aszTranMnemD, (PARA_TRANSMNEMO_LEN*2 + 1), aszTranMnem);

        /* -- convert long to ASCII -- */
        RflSPrintf(aszAmountS, PRT_AMOUNT_LEN + 1, aszPrtVLAmtTmp, lAmount);

        /* -- convert single char. to double -- */
        PrtDouble(aszAmountD, (PRT_AMOUNT_LEN*2 + 1), aszAmountS);

        if ((_tcslen(aszTranMnemD) + _tcslen(aszAmountD)) < PRT_VLCOLUMN) {
            /* -- set transaction mnemonics and amount -- */
            PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLDoubleMnemAmt, aszTranMnemD, aszAmountD);
            return;
        }
    }

    /* -- send mnemoncis and amount -- */
    PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLMnemAmt, aszTranMnem, lAmount);
}

/***** End of prscmnvl.c *****/