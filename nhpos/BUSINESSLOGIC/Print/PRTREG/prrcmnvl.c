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
* Title       : Print common routine, reg mode, validation 55 Char
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRCmnVL.C
*               | ||  ||
*               | ||  |+- S:21Char L:24Char _:not influcenced by print column
*               | ||  +-- R:recept & jounal S:slip V:validation K:kitchen
*               | |+----- comon routine
*               | +------ R:reg mode S:super mode
*               +-------- Pr:print module
* --------------------------------------------------------------------------
* Abstract: The provided function whose names are as follows:
*
*     PrtVLHead()        : validation header
*     PrtVLHead1()       : validation header 1st line
*     PrtVLHead2()       : validation header 2nd line
*     PrtVLHead3()       : validation header 3rd line
*     PrtVLTrail()       : validation trailer
*     PrtVLCasTrail()    : validation trailer(always print cashier)
*     PrtVLNumber()      : number
*     PrtVLNumberCP()    : number and expiration date
*     PrtVLAmtMnem()     : transaction mnemonics and amount
*     PrtVLAmtPerMnem()  : transaction mnemonics, rate and amount
*#    PrtVLCoupon()      : coupon mnemonics, and amount
*#    PrtVLNumQty()      : number and quantity
*     PrtVLNumWeight()   : number and weight
*     PrtVLAmtPluMnem()  : plu, dept, setmenu line
*     PrtVLTender1()     : tender 1st line (normal tender, foreign tender)
*     PrtVLTender2()     : tender 2nd line (normal tender)
*     PrtVLForeignTend() : tender 2nd line (foreign tender)
*     PrtVLTipPaidOut()  : tips paid out
*     PrtVLTotal()       : total
*     PrtVLChgTip()      : chrge Tips
*     PrtVLAccNum()      : account mnem and number
*     PrtVLOffline()     : EPT offline symbol & expiration Date
*
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
* Jun-09-92 : 00.00.01 : K.Nakajima :
* Jun-29-93 : 01.00.12 : K.You      : mod. PrtVLNumQty for US enhance.
* Jul-03-93 : 01.00.12 : R.Itoh     : add PrtVLEtktimeIn(), PrtVLEtkTimeOut()
* Apr-07-94 : 00.00.04 : Yoshiko.J  : add PrtVLAccNum(), PrtVLOffline(),
* Apr-29-94 : 02.05.00 : K.You      : bug fixed E-56 (mod. PrtVLOffline())
* May-20-94 : 02.05.00 : K.You      : bug fixed S-26 (add PrtVLNumberCP)
* Feb-28-95 : 03.00.00 : T.Nakahata : add coupon feature (PrtVLCoupon)
* Apr-25-95 : 03.00.00 : T.Nakahata : add promotional PLU with condiment
* Nov-15-95 : 03.01.00 : M.Ozawa    : Change ETK format at R3.1
* Aug-08-99 : 03.05.00 : M.Teraki   : Remove WAITER_MODEL
* Aug-16-99 : 03.05.00 : M.Teraki   : Merge STORE_RECALL_MODEL
*           :          :            :       /GUEST_CHECK_MODEL
* Dec-06-99 : 01.00.00 : hrkato     : Saratoga
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
#include <csstbpar.h>
#include<pmg.h>
#include<rfl.h>
#include"prtrin.h"
#include"prrcolm_.h"

/**
;========================================================================
;+                    S T A T I C    R A M s
;========================================================================
**/
CONST TCHAR   aszPrtVLHead[] = _T("%-24s       %s"); /* validation header */
CONST TCHAR   aszPrtVLTrail[] = _T("%04d %-4s%3s %-12.12s %04lu-%03lu  %s");  /* trailer, V3.3 */ /* 8-3-2000 TAR# 148297 */
CONST TCHAR   aszPrtVLAmtMnem[] = _T("%s\t %l$"); /* mnemo. and amount */
CONST TCHAR   aszPrtVLAmtPerMnem[] = _T("%-8.8s %-14s           %d%%\t %l$");/* mnem., taxmod, %, amount */ /*8 characters JHHJ*/
CONST TCHAR   aszPrtVLAmtPerMnem2[] = _T("%-8.8s %s\t %l$");              /* mnem., taxmod, amount */ /*8 characters JHHJ*/
CONST TCHAR   aszPrtVLCoupon[] = _T("%-12s %s\t %l$");  /* mnemonic, taxmod. amount */
CONST TCHAR   aszPrtVLNumber[] = _T("%s %s");                           /* number */
CONST TCHAR   aszPrtVLNumberCP[] = _T("%s %s  %s");                     /* number and exp. date */
CONST TCHAR   aszPrtVLNumQuant[] = _T("%s %s\t %4ld X %l$ ");/* number qty unit price */
CONST TCHAR   aszPrtVLNumPPIQuant[] = _T("%s %s\t %4ld X %l$/%ld ");/* number qty unit price */
CONST TCHAR   aszPrtVLNumWeight[] = _T("%s\t %8.*l$%s @ %l$/%s ");/* number weight unit price */
CONST TCHAR   aszPrtVLProduct[] = _T("%-4s %-12s %s %s\t %l$");/* noun mnemo. and product price */
CONST TCHAR   aszPrtVLProduct2[] = _T("%-12s %s %s\t %l$");    /* noun mnemo. and product price */
CONST TCHAR   aszPrtVLTender1[] = _T("%-8.8s %-19s  %s\t %s");/* tender(and foreign) 1st line */ /*8 characters JHHJ*/
CONST TCHAR   aszPrtVLTender2[] = _T("                              %s\t %l$");/* tender 2nd line */
CONST TCHAR   aszPrtVLForeign[] = _T("%.*l$");           /* foreign amount */
CONST TCHAR   aszPrtVLForeignTend[] = _T("%s / %10.*l$     %-8.8s\t %l$");/* foreign tender 2nd line, V3.4 */ /*8 characters JHHJ*/
CONST TCHAR   aszPrtVLTipPO[] = _T("%-4s%03d                       %s\t %l$");/* tips paid out */
CONST TCHAR   aszPrtVLTotal[] = _T("%s\t %s");                        /* total line */
CONST TCHAR   aszPrtRate[] = _T("%3d%%");                             /* rate */
CONST TCHAR   aszPrtWaiter[] = _T("%-4s%8u");                        /* waiter */
CONST TCHAR   aszPrtVLChgTip[] = _T("%-8.8s  %-7s %-4s %s     %l$");  /* charge Tips line */ /*8 characters JHHJ*/
CONST TCHAR   aszPrtVLTray[] = _T("%-40s%s\t %d");                    /* tray total */
CONST TCHAR   aszPrtVLEtkTimeIn[] = _T("%-16s\t%-3s");                /* R3.1 */
CONST TCHAR   aszPrtVLEtkTimeOut[] = _T("%-16s - %-16s");             /* R3.1 */
CONST TCHAR   aszPrtVLOffDate[] = _T("%02d/%02d");                /* expiration date          */
CONST TCHAR   aszPrtVLOffDateTtl[] = _T("%02d/%02d  ");           /* expiration date for service total    */
CONST TCHAR   aszPrtVLOffline[] = _T("%-4s %5s %6s %4s %5s %4s %2s\t%-8.8s %l$");   /*8 characters JHHJ, mnemo. mnem. date & amt  */
CONST TCHAR   aszPrtVLAccNum[] = _T("         %8.8s \t %s");      /*8 characters JHHJ, account mnemo, number    */
CONST TCHAR   aszPrtVLPluBuild[]      = _T("%s\t %13s %04d");     /* Plu no and Dept no. */

extern CONST TCHAR FARCONST  aszCasWaiID[];
extern CONST TCHAR FARCONST  aszEtkCode[];
extern CONST TCHAR FARCONST  aszEtkEmp[];

/* Check Endorsement, 21RFC05402 */
CONST TCHAR   aszPrtVLHznHead[] = _T("\t%-17s");              /* horizontal check endorsement header */
CONST TCHAR   aszPrtVLVrtHead[] = _T("\t%-24s%-13s");         /* vertical check endorsement header */
CONST TCHAR   aszPrtVLHznTrail1[] = _T("\t%04d             ");                      /* check endorsement trailer w/o cashier data */
CONST TCHAR   aszPrtVLHznTrail2[] = _T("\t%04lu-%03lu %s");                         /* check endorsement trailer w/o cashier data */
CONST TCHAR   aszPrtVLHznTrailCas1[] = _T("\t%04d %03d %-8s");                      /* check endorsement trailer w/ cashier data */
CONST TCHAR   aszPrtVLHznTrailClk1[] = _T("\t%04d %04d%-8s");                       /* check endorsement trailer w/ clerk data */
CONST TCHAR   aszPrtVLHznTrailCasClk1[] = _T("\t%04d %03d %04d    ");               /* check endorsement trailer w/ cashier clerk data */
CONST TCHAR   aszPrtVLHznTrailCas2[] = _T("\t%04lu-%03lu %s");                      /* check endorsement trailer w/ cashier data */

CONST TCHAR  aszPrtVLCheckTrail1[]  = _T("%4s   %3s  %-10s   %8s \t %16s");         /* trailer (cons.number,checker no. name store reg number,time and date) */
CONST TCHAR  aszPrtVLCheckTrail2[]  = _T("%4s   %4s %-10s   %8s \t %16s");          /* trailer (cons.number,clerk no. name store reg number,time and date) */
CONST TCHAR  aszPrtVLCheckTrailNoCas[]  = _T("%4s                  %8s \t %16s");   /* trailer (cons.number, store reg number,time and date) */
CONST TCHAR  aszPrtVLCheckTrailClkno[]  = _T("%04d");
CONST TCHAR  aszPrtVLCheckTrailChkno[]  = _T("%03d");
CONST TCHAR  aszPrtVLCheckTrailConsno[]  = _T("%04d");
CONST TCHAR  aszPrtVLCheckTrailMnem[]  = _T("%-10s");
CONST TCHAR  aszPrtVLCheckTrailStrgno[]  = _T("%04lu-%03lu");

CONST TCHAR   aszPrtVLVrtMnemAmt[] = _T("      %8.8s \t %l$");                      /*8 characters JHHJ,  mnem & amt for endorsement */
CONST TCHAR   aszPrtVLHznMnemAmt[] = _T("                                      %s\t %l$");    /* mnem & amt for endorsement */
CONST TCHAR   aszPrtVLHznAmt[] = _T("\t %l$");                                      /* amount for endorsement */
CONST TCHAR   aszPrtVLVrtMnemCSAmt[] = _T("      %8.8s \t %s");                     /*8 characters JHHJ,  mnem & amt for endorsement */
CONST TCHAR   aszPrtVLHznMnemCSAmt[] = _T("                          %8.8s \t%s");  /*8 characters JHHJ,  mnem & amt for endorsement */
CONST TCHAR   aszPrtVLHznCSAmt[] = _T("\t %s");                                     /* amount for endorsement */

/**
;========================================================================
;+             P R O G R A M    D E C L A R A T I O N s
;========================================================================
**/
/*
*===========================================================================
** Format  : VOID  PrtVLHead(TRANINFORMATION *pTran, CHAR fbMod);
*
*    Input : TRANINFORMATION      pTran         -transaction information
*            CHAR                 fbMod         -void status
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints validation header lines.
*===========================================================================
*/
VOID  PrtVLHead(TRANINFORMATION *pTran, USHORT fbMod)
{
    PrtVLHead1(pTran);                          /* validation head 1st line */

    PrtVLHead2(pTran);                          /* validation head 2nd line */

    PrtVLHead3(fbMod, 0);                          /* validation head 3rd line */
}

/*
*===========================================================================
** Format  : VOID  PrtVLHead1(TRANINFORMATION *pTran);
*
*    Input : TRANINFORMATION      pTran         -transaction information
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints validation header line.
*===========================================================================
*/
VOID  PrtVLHead1(TRANINFORMATION *pTran)
{
	TCHAR   aszTraining[PARA_CHAR24_LEN + 1] = { 0 };  /* training mnemo. */
	TCHAR   aszHead1[PARA_CHAR24_LEN + 1] = { 0 };     /* valid. header 1st line */

    /* --- training mode ? -- */
    if ( (pTran->TranCurQual.fsCurStatus) & CURQUAL_TRAINING ) {
        PrtGet24Mnem(aszTraining, CH24_TRNGHED_ADR); /* get training mnemo. */
    }

    PrtGet24Mnem(aszHead1,    CH24_1STVAL_ADR);      /* get validation head */

    PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLHead, aszTraining, aszHead1);
}

/*
*===========================================================================
** Format  : VOID  PrtVLHead2(TRANINFORMATION *pTran);
*
*    Input : TRANINFORMATION      pTran         -transaction information
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints validation header line.
*===========================================================================
*/
VOID  PrtVLHead2(TRANINFORMATION *pTran)
{
	TCHAR   aszPreVoid[PARA_CHAR24_LEN + 1] = { 0 };   /* precelect void */
	TCHAR   aszHead2[PARA_CHAR24_LEN + 1] = { 0 };     /* valid. header 1st line */

    /* --- preselect void ? -- */
    if ( (pTran->TranCurQual.fsCurStatus) & CURQUAL_PVOID ) {
        PrtGet24Mnem(aszPreVoid,  CH24_PVOID_ADR); /* get precelect void */
    }

    /* --- Returns Transaction ? -- */
    if ( (pTran->TranCurQual.fsCurStatus) & (CURQUAL_TRETURN | CURQUAL_PRETURN) ) {
        PrtGet24Mnem(aszPreVoid,  CH24_PRETURN_ADR); /* get returns transaction */
    }

	PrtGet24Mnem(aszHead2, CH24_2NDVAL_ADR);    /* get validation head */

    PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLHead, aszPreVoid, aszHead2);
}

/*
*===========================================================================
** Format  : VOID  PrtVLHead3(CHAR  fbMod);
*
*    Input : CHAR   fbMod               -void status
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints validation header line.
*===========================================================================
*/
VOID  PrtVLHead3(USHORT fbMod, USHORT usReasonCode)
{
	TCHAR   aszVoid[PARA_CHAR24_LEN + 1] = { 0 };      /* void */
	TCHAR   aszHead3[PARA_CHAR24_LEN + 1] = { 0 };     /* valid. header 1st line */

    /* --- void ? -- */
    if ( fbMod & VOID_MODIFIER ) {
		if (0 > PrtGetReturns(fbMod, usReasonCode, aszVoid, TCHARSIZEOF(aszVoid))) {
			/* -- get void mnemonics instead of returns mnemonic -- */
			PrtGetVoid(aszVoid,  TCHARSIZEOF(aszVoid));     /* get void */
		}
    }

    PrtGet24Mnem(aszHead3, CH24_3RDVAL_ADR);    /* get validation head */

    PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLHead, aszVoid, aszHead3);
}

/*
*===========================================================================
** Format  : VOID  PrtVLTrail(TRANINFORMATION *pTran);
*
*    Input : TRANINFORMATION    *pTran       -transaction information
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints validation trailer.
*===========================================================================
*/
VOID  PrtVLTrail(TRANINFORMATION *pTran)
{
    ULONG  ulID;
	TCHAR  aszMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };
	TCHAR  aszDate[PRT_DATETIME_LEN + 1] = { 0 };
    TCHAR  aszID[8 + 1] = {0};
	TCHAR  aszName[PARA_CASHIER_LEN + 1] = { 0 };

    /* -- check cashier or waiter, and get mnemonics --*/
    if ( PRT_CASHIER == PrtChkCasWai(aszMnem, pTran->TranModeQual.ulCashierID, pTran->TranModeQual.ulWaiterID) ) {
        ulID = RflTruncateEmployeeNumber(pTran->TranModeQual.ulCashierID);
        _tcsncpy(aszName, pTran->TranModeQual.aszCashierMnem, PRT_CASNAME_LEN);
    } else {
        ulID = RflTruncateEmployeeNumber(pTran->TranModeQual.ulWaiterID);
        _tcsncpy(aszName, pTran->TranModeQual.aszWaiterMnem, PRT_CASNAME_LEN);
    }

    /* -- convert ID to ascii -- */
    RflSPrintf(aszID, TCHARSIZEOF(aszID), aszCasWaiID, ulID);

    PrtGetDate(aszDate, TCHARSIZEOF(aszDate), pTran);   /* get date time */

    PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLTrail, pTran->TranCurQual.usConsNo, aszMnem, aszID, aszName,
                               pTran->TranCurQual.ulStoreregNo / 1000L, pTran->TranCurQual.ulStoreregNo % 1000L, aszDate);

}

/*
*===========================================================================
** Format  : VOID  PrtVLCasTrail(TRANINFORMATION *pTran, ULONG ulStReg);
*
*    Input : TRANINFORMATION    *pTran       -transaction information
*            ULONG              ulStReg      -store reg number
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints validation trailer.
*===========================================================================
*/
VOID  PrtVLCasTrail(TRANINFORMATION *pTran, ULONG ulStReg)
{
	TCHAR  aszMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };
	TCHAR  aszDate[PRT_DATETIME_LEN + 1] = { 0 };
    TCHAR  aszID[8 + 1] = {0};
	TCHAR  aszName[PARA_CASHIER_LEN + 1] = { 0 };

    /* -- convert ID to ascii -- */
    if (pTran->TranModeQual.ulCashierID) {
        RflSPrintf(aszID, TCHARSIZEOF(aszID), aszCasWaiID, RflTruncateEmployeeNumber(pTran->TranModeQual.ulCashierID));
    }

    /* -- get cashier mnemonics --*/
    PrtGetDate(aszDate, TCHARSIZEOF(aszDate), pTran);   /* get date time */

    _tcsncpy(aszName, pTran->TranModeQual.aszCashierMnem, PRT_CASNAME_LEN);

    PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLTrail, pTran->TranCurQual.usConsNo, aszMnem, aszID, aszName, ulStReg / 1000L, ulStReg % 1000L, aszDate);
}

/*
*===========================================================================
** Format  : VOID  PrtVLNumber(UCHAR  *pszNumber);
*
*    Input : UCHAR   *pszNumber      -number
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints number.
*===========================================================================
*/
VOID  PrtVLNumber(TCHAR  *pszNumber)
{
    if (*pszNumber != 0) {
		TCHAR  aszNumMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };

		RflGetTranMnem(aszNumMnem, TRN_NUM_ADR );
        PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLNumber, aszNumMnem, pszNumber);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtVLNumber(UCHAR  *pszNumber);
*
*    Input : UCHAR   *pszNumber      -number
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints number.
*===========================================================================
*/
VOID  PrtVLMnemNumber(USHORT usAdr, TCHAR  *pszNumber)
{
    if (*pszNumber != 0) {
		TCHAR  aszNumMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };

        if (usAdr == 0) usAdr = TRN_NUM_ADR;

		RflGetTranMnem(aszNumMnem, usAdr );
        PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLNumber, aszNumMnem, pszNumber);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtVLNumber(UCHAR  *pszNumber, UCHAR *auchDate);
*
*    Input : UCHAR   *pszNumber      -number
*            UCHAR       *auchDate          -expiration date
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints number.
*===========================================================================
*/
VOID  PrtVLNumberCP(TCHAR  *pszNumber, TCHAR  *auchDate)
{
	TCHAR  aszDate[NUM_EXPIRA + 1] = { 0 };

    _tcsncpy(aszDate, auchDate, NUM_EXPIRA);
    if (*pszNumber != 0) {
		USHORT usOffDate;
		TCHAR  aszOffDate[7 + 1] = { 0 };
		TCHAR  aszNumMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };

		RflGetTranMnem(aszNumMnem, TRN_NUM_ADR );

        if (aszDate != '\0') {                          /* expiration date  */
            usOffDate = _ttoi(aszDate);
            if (usOffDate != 0) {
                RflSPrintf(aszOffDate, TCHARSIZEOF(aszOffDate), aszPrtVLOffDateTtl, usOffDate / 100, usOffDate % 100);
            }
        }
        PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLNumberCP, aszNumMnem, pszNumber, aszOffDate);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtVLAmtMnem(UCHAR uchAdr, LONG lAmout);
*
*    Input : UCHAR uchAdr          -Transacion mnemonics address
*            LONG  lAmount         -Amount
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints prints transaction mnemonics
*            and Amount line.
*===========================================================================
*/
VOID  PrtVLAmtMnem(USHORT usAdr, DCURRENCY lAmount)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- send mnemoncis and amount -- */
    PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLAmtMnem, aszTranMnem, lAmount);

}

/*
*===========================================================================
** Format  : VOID  PrtVLAmtPerMnem(UCHAR uchAdr, UCHAR uchRate, LONG lAmount);
*
*    Input : UCHAR uchAdr          -Transacion mnemonics address
*            UCHAR uchRate,        -rate
*            LONG  lAmount         -Amount
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints prints transaction mnemonics
*            and Amount line.
*===========================================================================
*/
VOID  PrtVLAmtPerMnem(USHORT usAdr, UCHAR uchRate, SHORT fsTax, USHORT fbMod, DCURRENCY lAmount)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};
    TCHAR  aszTaxMod[PRT_VLCOLUMN + 1] = {0};

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- get tax modifier mnemonics --*/
    PrtGetTaxMod(aszTaxMod, TCHARSIZEOF(aszTaxMod), fsTax, fbMod);

    if (uchRate == 0) {
        PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLAmtPerMnem2, aszTranMnem, aszTaxMod, lAmount);
    } else {
        PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLAmtPerMnem, aszTranMnem, aszTaxMod, uchRate, lAmount);
    }

}

/*
*===========================================================================
** Format  : VOID  PrtVLCoupon( ITEMCOUPON * pItem )
*
*    Input : ITEMCOUPON *pItem  -   address of coupon item to print
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints prints coupon mnemonics, and its amount.
*===========================================================================
*/
VOID  PrtVLCoupon( ITEMCOUPON *pItem, SHORT fsTax )
{
	TCHAR  aszTaxMod[PRT_VLCOLUMN + 1] = { 0 };

    /* -- get tax modifier mnemonics --*/
    PrtGetTaxMod( aszTaxMod, TCHARSIZEOF( aszTaxMod ), fsTax, pItem->fbModifier );

    /* -- send mnemoncis and amount -- */
    PmgPrintf( PMG_PRT_RCT_JNL, aszPrtVLCoupon, pItem->aszMnemonic, aszTaxMod, pItem->lAmount );
}

/*
*===========================================================================
** Format  : VOID  PrtVLNumQty(ITEMSALES *pItem);
*
*    Input : ITEMSALES *pItem        -item information address
*
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints prints number, qty and unitprice
*===========================================================================
*/
VOID  PrtVLNumQty(ITEMSALES *pItem)
{
	TCHAR       aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};
    TCHAR       auchDummy[NUM_PLU_LEN] = {0};
    LONG        lModQty = labs(pItem->lQTY);         /* get absolute value */
    DCURRENCY   lPrice = 0;
    USHORT      i;
    USHORT      usNoOfChild;

    if ( ( *(pItem->aszNumber) == '\0') && (lModQty == PLU_BASE_UNIT) ) {
        return ;
    }

    if ( *(pItem->aszNumber) != '\0') {
        /* -- get transaction mnemonics -- */
        if (pItem->fbModifier2 & SKU_MODIFIER) {
			RflGetTranMnem(aszTranMnem, TRN_SKUNO_ADR);
        } else {
			RflGetTranMnem(aszTranMnem, TRN_NUM_ADR);
        }
    }

    if (lModQty == PLU_BASE_UNIT) {
        PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLNumber, aszTranMnem, pItem->aszNumber);
        return ;
    }

    /* -- get condiment's and noun's price -- */
    usNoOfChild = pItem->uchCondNo + pItem->uchPrintModNo + pItem->uchChildNo;

    lPrice = 0L;
    for ( i = pItem->uchChildNo; i < usNoOfChild; i++) {
        if (_tcsncmp(pItem->Condiment[ i ].auchPLUNo, auchDummy, NUM_PLU_LEN) != 0 ) {
        /* if ( pItem->Condiment[ i ].usPLUNo ) { */
            lPrice += pItem->Condiment[i].lUnitPrice;
        }
    }
    if (pItem->uchPPICode) {
        lPrice = lPrice * (pItem->lQTY / PLU_BASE_UNIT);
        lPrice += pItem->lProduct;      /* print whole at ppi sales */
    } else {
        lPrice += pItem->lUnitPrice;    /* print unit price at normal sales */
    }

    if (pItem->uchPPICode) {    /* print "QTY * Product / QTY" */
        PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLNumPPIQuant, aszTranMnem, pItem->aszNumber, (pItem->lQTY) / PLU_BASE_UNIT, RflLLabs(lPrice), labs(pItem->lQTY / PLU_BASE_UNIT));
    } else
    if (pItem->uchPM) {    /* print "QTY * Product / PM, 2172" */
        PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLNumPPIQuant, aszTranMnem, pItem->aszNumber, (pItem->lQTY) / PLU_BASE_UNIT, RflLLabs(lPrice), pItem->uchPM);
    } else {
        PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLNumQuant, aszTranMnem, pItem->aszNumber, (pItem->lQTY) / PLU_BASE_UNIT, lPrice);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtVLNumWeight(TRANINFORMATION *pTran, ITEMSALES *pItem);
*
*    Input : TRANINFORMATION         pTran      -transaction information
*            ITEMSALES               pItem      -item information
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints prints number, weight and unitprice
*===========================================================================
*/
VOID  PrtVLNumWeight(TRANINFORMATION *pTran, ITEMSALES *pItem)
{

	TCHAR  aszWeightSym[PARA_SPEMNEMO_LEN + 1] = { 0 };  /* PARA_... defined in "paraequ.h" */
	TCHAR  aszWork[PRT_VLCOLUMN + 1] = { 0 };
	TCHAR  aszWork2[PRT_VLCOLUMN + 1] = { 0 };
    USHORT usWorkLen;
    USHORT usWorkLen2;
    SHORT  sDecPoint;                            /* decimal point */
    LONG   lModQty;                              /* modified qty */
	SHORT numCounter;	//US Customs

    /* -- get scalable symbol and adjust qty -- */
    PrtGetScale(aszWeightSym, &sDecPoint, &lModQty, pTran, pItem);

	for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){ //US Customs
		if (pItem->aszNumber[numCounter][0] != '\0') {
			if (pItem->fbModifier2 & SKU_MODIFIER) {
				RflGetTranMnem(aszWork, TRN_SKUNO_ADR);
			} else {
				RflGetTranMnem(aszWork, TRN_NUM_ADR);   /* write number mnem. */
			}

			usWorkLen = _tcslen(aszWork);
			usWorkLen2 = _tcslen(pItem->aszNumber[0]);

			if (usWorkLen + usWorkLen2 + 1  > 25) {

				aszWork[usWorkLen] = _T('\t');              /* write tab */
				aszWork[usWorkLen+1] = _T('\0');            /* make string */

				_tcscat(aszWork, pItem->aszNumber[numCounter]);      /* write number */

				RflStrAdjust(aszWork2, PRT_VLCOLUMN + 1/*sizeof(aszWork2)*/, aszWork, 25,
														 RFL_FEED_OFF);
			} else {

				aszWork[usWorkLen] = PRT_SPACE;         /* write space */

				aszWork[usWorkLen+1] = '\0';            /* make string */

				_tcscpy(aszWork2, aszWork);

				_tcscat(aszWork2, pItem->aszNumber[numCounter]);     /* write number */
			}

		}
		else {//US Customs
			break;
		}
	}//end for(numCounter)

    /* -- send "weight " and unitprice -- */
    PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLNumWeight, aszWork2, sDecPoint,
                               lModQty, aszWeightSym, pItem->lUnitPrice,
                               aszWeightSym);

}

/*
*===========================================================================
** Format  : VOID  PrtVLAmtPluMnem(ITEMSALES *pItem, SHORT fsTax);
*
*    Input : ITEMSALES *pItem       -item information
*            SHORT      fsTax       -tax status (us or canada)
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints adj.,  plu mnem., tax modifier, product price
*===========================================================================
*/
VOID  PrtVLAmtPluMnem(ITEMSALES *pItem, SHORT fsTax)
{
	TCHAR      aszAdj[PARA_ADJMNEMO_LEN + 1] = { 0 };
	TCHAR      aszTaxMod[PRT_TAXMOD_LEN + 1] = { 0 };
    TCHAR      aszManuMnem[PARA_TRANSMNEMO_LEN + 1]; /* PARA_... defined in "paraequ.h" */
    DCURRENCY  lProduct;

    /* -- adjective? -- */
    if ( ((pItem->uchAdjective) != 0 && pItem->uchMinorClass != CLASS_ITEMORDERDEC)
           && (pItem->uchMinorClass != CLASS_DEPT)
           && (pItem->uchMinorClass != CLASS_DEPTITEMDISC)
           && (pItem->uchMinorClass != CLASS_DEPTMODDISC) ) {

        /* -- get adjective -- */
		RflGetAdj(aszAdj, pItem->uchAdjective);
    }

    /* --get taxmodifier --*/
    PrtGetTaxMod(aszTaxMod, TCHARSIZEOF(aszTaxMod), fsTax, pItem->fbModifier);

    /* -- get Manual weight mnemonics -- */
    if (pItem->fsPrintStatus & PRT_MANUAL_WEIGHT) {
		RflGetTranMnem(aszManuMnem, TRN_MSCALE_ADR);
    }

    /* -- get noun and condiment 's  product price -- */
    PrtGetAllProduct(pItem, &lProduct);

    /* -- send adj, plu dept setmenu mnemo., tax mod, product price --*/
    if (pItem->uchAdjective != 0 && pItem->uchMinorClass != CLASS_ITEMORDERDEC) {
        PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLProduct, aszAdj, pItem->aszMnemonic, aszTaxMod, aszManuMnem, lProduct);
    } else {
        PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLProduct2, pItem->aszMnemonic, aszTaxMod, aszManuMnem, lProduct);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtVLTender1(UCHAR *pszNumber, LONG lBalance);
*
*    Input : UCHAR *pszNumber               -number
*            LONG  lBalance                 -balance due
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints tender & foreign tender 1st line.
*===========================================================================
*/
VOID  PrtVLTender1(TCHAR *pszNumber, DCURRENCY lBalance)
{
	TCHAR  aszNumMnem[PARA_TRANSMNEMO_LEN + 1] = {0};
    TCHAR  aszTotalMnem[PARA_TRANSMNEMO_LEN + 1] = {0};
    TCHAR  aszBalance[PRT_VLCOLUMN] = {0};

    if (*pszNumber != '\0') {
        /* -- get transaction mnemonics -- */
		RflGetTranMnem(aszNumMnem, TRN_NUM_ADR);
    }

    /* -- get total mnemonics (not total key mnemonics)--*/
	RflGetTranMnem(aszTotalMnem, TRN_AMTTL_ADR);
    PrtAdjustNative(aszBalance, lBalance);
    PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLTender1, aszNumMnem, pszNumber, aszTotalMnem, aszBalance);
}

/*
*===========================================================================
** Format  : VOID  PrtVLTender2(ITEMTENDER  *pItem);
*
*    Input : ITEMTENDER    *pItem           -item information
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints tender 2nd line.
*===========================================================================
*/
VOID  PrtVLTender2(ITEMTENDER  *pItem)
{
	TCHAR  aszTendMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };/* PARA_... defined in "paraequ.h" */

    /* -- get tender mnemonics -- */
	RflGetTranMnem(aszTendMnem, RflChkTendAdr(pItem));

    PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLTender2, aszTendMnem, pItem->lTenderAmount);

}

/*
*===========================================================================
** Format  : VOID  PrtVLForeignTend(ITEMTENDER  *pItem);
*
*    Input : ITEMTENDER    *pItem           -item information
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints foreign tender 2nd line.
*===========================================================================
*/
VOID  PrtVLForeignTend(ITEMTENDER  *pItem)
{
    UCHAR  uchAdr1;
	USHORT usTranAdr2;
    USHORT usStrLen;
    SHORT  sDecPoint;
	TCHAR  aszFAmt[PRT_VLCOLUMN] = { 0 };
	TCHAR  aszFSym[PARA_SPEMNEMO_LEN + 1] = { 0 };  /* PARA_... defined in "paraequ.h" */
	TCHAR  aszTendMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };/* PARA_... defined in "paraequ.h" */

    /* -- check decimal point -- */
    if (pItem->auchTendStatus[0] & ITEMTENDER_STATUS_0_DECIMAL_3) {
        if (pItem->auchTendStatus[0] & ITEMTENDER_STATUS_0_DECIMAL_0) {
            sDecPoint = PRT_NODECIMAL;
        } else {
            sDecPoint = PRT_3DECIMAL;
        }
    } else {
        sDecPoint = PRT_2DECIMAL;
    }

	PrtGetMoneyMnem (pItem->uchMinorClass, &usTranAdr2, &uchAdr1);

	RflGetSpecMnem(aszFSym, uchAdr1);
	RflGetTranMnem(aszTendMnem, usTranAdr2);

    usStrLen = _tcslen(aszFSym);            /* get foreign symbole length */

    RflSPrintf(&aszFAmt[usStrLen], TCHARSIZEOF(aszFAmt), aszPrtVLForeign, sDecPoint, pItem->lForeignAmount);

    /* -- adjust sign ( + , - ) -- */
    if (pItem->lForeignAmount < 0) {
        aszFAmt[0] = _T('-');
        _tcsncpy(&aszFAmt[1], aszFSym, usStrLen);
    } else {
        _tcsncpy(aszFAmt, aszFSym, usStrLen);
    }

    /* 6 digit decimal point for Euro, V3.4 */
    if (pItem->auchTendStatus[1] & ODD_MDC_BIT0) {
        sDecPoint = PRT_6DECIMAL;
    } else {
        sDecPoint = PRT_5DECIMAL;
    }

    PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLForeignTend, aszFAmt, sDecPoint, pItem->ulFCRate, aszTendMnem, pItem->lTenderAmount);
}

/*
*===========================================================================
** Format  : VOID  PrtVLTipPaidOut(USHORT usID, LONG lAmount);
*
*    Input : USHORT     usID        -waiter ID
*            LONG       lAmount     -amount
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints tips paid out line.
*===========================================================================
*/
VOID  PrtVLTipPaidOut(ULONG ulID, DCURRENCY lAmount)
{
	TCHAR  aszSpeMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };  /* PARA_... defined in "paraequ.h" */
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };/* PARA_... defined in "paraequ.h" */

    /* -- get waiter mnemonics --*/
	RflGetSpecMnem(aszSpeMnem, SPC_CAS_ADR);

    /* -- get tips paid out mnemonics --*/
	RflGetTranMnem(aszTranMnem, TRN_TIPPO_ADR);

    PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLTipPO, aszSpeMnem, ulID, aszTranMnem, lAmount);

}


/*
*===========================================================================
** Format  : VOID  PrtVLAmtSym(UCHAR  uchMnemAdr, LONG lAmount, SHORT sType);
*
*    Input : UCHAR       uchMnemAdr         -Total Mnemonics address
*            LONG        lAmount            -Amount
*            SHORT       sType              -Amount
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints total line.
*===========================================================================
*/
VOID  PrtVLAmtSym(USHORT  usMnemAdr, DCURRENCY lAmount, SHORT sType)
{
	TCHAR  aszMnem[PARA_TRANSMNEMO_LEN + 1] = {0};
    TCHAR  aszAmountS[PRT_VLCOLUMN] = {0};
    TCHAR  aszAmountD[PRT_VLCOLUMN] = {0};
    TCHAR  *pszAmount;

    /* -- get total mnemonics (not total key mnemonics)--*/
	RflGetTranMnem(aszMnem, usMnemAdr);

    PrtAdjustNative(aszAmountS, lAmount);    /* get native mnemo. and adjust */

    /* -- amount printed double wide? -- */
    if (sType == PRT_DOUBLE) {
        PrtDouble(aszAmountD, PRT_VLCOLUMN, aszAmountS);
        pszAmount = aszAmountD;
    } else {
        pszAmount = aszAmountS;
    }

    PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLTotal, aszMnem, pszAmount);
}

/*
*===========================================================================
** Format  : VOID  PrtVLTotal(ITEMTOTAL *pItem, LONG lAmount, USHORT ushMnemAdr);
*
*    Input : ITEMTOTAL   *pItem             -Item information
*            LONG        lAmount            -Amount
*            USHORT      usMnemAdr          -Total Mnemonics address
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints total line.
*===========================================================================
*/
VOID PrtVLTotal(ITEMTOTAL  *pItem, DCURRENCY lAmount, USHORT  usMnemAdr)
{
    SHORT  sType;

    /* -- total amount printed double wide? -- */
    if (pItem->auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {
        sType = PRT_DOUBLE;
    } else {
        sType = PRT_SINGLE;
    }

    PrtVLAmtSym(usMnemAdr, lAmount, sType);
}

/*
*===========================================================================
** Format  : VOID  PrtVLChgTip(USHORT usID, UCHAR uchRate, LONG lamount);
*
*    Input : USHORT usID,            -waiter ID
*            UCHAR uchRate,          -charge tip rate
*            LONG lamount            -amount
*            USHORT fsTax,           -US or Canadian Tax
*            UCHAR  fbMod            -modifier status
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints  charge Tip line.
*===========================================================================
*/
VOID  PrtVLChgTip(ULONG ulID, UCHAR uchRate, DCURRENCY lAmount, USHORT fsTax, USHORT  fbMod, USHORT usAdr)
{
	TCHAR  aszSpeMnem[PARA_SPEMNEMO_LEN + 1] = {0};
	TCHAR  aszChgMnem[PARA_TRANSMNEMO_LEN + 1] = {0};
    TCHAR  aszWaiter[PRT_VLCOLUMN] = {0};
    TCHAR  aszRate[PRT_VLCOLUMN] = {0};
    TCHAR  aszTaxMod[PRT_RJCOLUMN + 1] = {0};

    if (ulID != 0) {
        /* -- get waiter mnemonics --*/
		RflGetSpecMnem(aszSpeMnem, SPC_CAS_ADR);
        RflSPrintf(aszWaiter, TCHARSIZEOF(aszWaiter), aszPrtWaiter, aszSpeMnem, ulID);
    }

    /* get tax modifier mnemonics (if not modifer aszTaxMod[0] = '\0')  */
    PrtGetTaxMod(aszTaxMod, TCHARSIZEOF(aszTaxMod), fsTax, fbMod);

    if (uchRate != 0) {
        RflSPrintf(aszRate, TCHARSIZEOF(aszRate), aszPrtRate, uchRate);
    }

    /* -- get charge Tip mnemonics -- */
	RflGetTranMnem(aszChgMnem, usAdr);

    PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLChgTip, aszChgMnem, aszWaiter, aszTaxMod, aszRate, lAmount);

}

/*
*===========================================================================
** Format  : VOID  PrtVLTray(ITEMTOTAL *pItem);
*
*    Input : ITEMTOTAL *pItem        -item information address
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints  tray total line
*===========================================================================
*/
VOID  PrtVLTray(ITEMTOTAL *pItem)
{
    SHORT sType;
	TCHAR aszTrayAmt[50] = { 0 };
	TCHAR aszCoMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };   /* tray counter mnem. */

    /* -- total amount printed double wide? -- */
    if (pItem->auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) {
        sType = PRT_DOUBLE;
    } else {
        sType = PRT_SINGLE;
    }

    PrtSetMnemSymAmt(aszTrayAmt, sizeof(aszTrayAmt), RflChkTtlAdr(pItem), pItem->lMI, sType);

	RflGetTranMnem(aszCoMnem, TRN_TRAYCO_ADR);

    PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLTray, aszTrayAmt, aszCoMnem, pItem->sTrayCo);
}

/*
*===========================================================================
** Format  : VOID  PrtSetMnemSymAmt(UCHAR  *pszDest, USHORT usDestLen,
*                                   UCHAR uchAdr,
*                                   LONG lAmount, SHORT sType);
*    Input : UCHAR  *pszDest,         -destination buffer
*            USHORT usDestLen         -destination buffer length
*            UCHAR  uchAdr,           -transacation mnemonics address
*            LONG   lAmount           -amount
             SHORT  sType             -amount character size(single or double)
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function set mnemonics and amount.
*===========================================================================
*/
VOID  PrtSetMnemSymAmt(TCHAR  *pszDest, USHORT usDestLen, USHORT usAdr, DCURRENCY lAmount, SHORT sType)
{
    TCHAR  aszTranMnem[50] = {0};
    TCHAR  aszSpecAmtS[PARA_SPEMNEMO_LEN +PRT_AMOUNT_LEN + 1] = {0};
	TCHAR  aszSpecAmtD[(PARA_SPEMNEMO_LEN + PRT_AMOUNT_LEN) * 2  + 1] = {0};
    USHORT usMnemLen, usAmtSLen, usAmtDLen;

    /* -- get transaction mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- adjust native mnemonic and amount sign(+.-) -- */
    PrtAdjustNative(aszSpecAmtS, lAmount);

    /* -- convert to double -- */
    PrtDouble(aszSpecAmtD, ((PARA_SPEMNEMO_LEN + PRT_AMOUNT_LEN) * 2  + 1), aszSpecAmtS);

    usMnemLen = _tcslen(aszTranMnem);
    usAmtSLen = _tcslen(aszSpecAmtS);
    usAmtDLen = _tcslen(aszSpecAmtD);

    aszTranMnem[usMnemLen] = _T('\t');              /* write tab */

    if ((sType == PRT_DOUBLE) && (usMnemLen + usAmtDLen + 1 <= 30) ) {
        _tcscat(aszTranMnem,  aszSpecAmtD);
        RflStrAdjust(pszDest,  usDestLen, aszTranMnem, 30, RFL_FEED_OFF); /* adjust ASCII string :RflStr.c */
    } else {
        _tcscat(aszTranMnem,  aszSpecAmtS);
        RflStrAdjust(pszDest,  usDestLen, aszTranMnem, 30, RFL_FEED_OFF); /* adjust ASCII string :RflStr.c */
    }
}


/*
*===========================================================================
** Format  : VOID  PrtVLEtkTimeIn(TRANINFORMATION *pTran ITEMTOTAL *pItem);
*
*    Input : TRANINFORMATION *pTran  -transaction information address
*            ITEMMISC *pItem         -item information address
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints ETK time-in line
*===========================================================================
*/
VOID  PrtVLEtkTimeIn(TRANINFORMATION *pTran, ITEMMISC *pItem)
{
	TCHAR   aszDate[PRT_DATETIME_LEN + 1] = { 0 };
	TCHAR   aszCode[4] = { 0 };

    /* -- get job time-in code -- */
    PrtGetJobInDate(aszDate, TCHARSIZEOF(aszDate), pTran, pItem);   /* get date time */
    if ( pItem->uchJobCode != 0 ) {
        RflSPrintf(&aszCode[1], 3, aszEtkCode, pItem->uchJobCode);
        aszCode[0] = _T(' ');
        aszCode[3] = _T('\0');
    } else {
        aszCode[0] = _T('\0');
    }

    PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLEtkTimeIn, aszDate, aszCode);

}


/*
*===========================================================================
** Format  : VOID  PrtVLEtkTimeOut(TRANINFORMATION *pTran, ITEMMISC *pItem,
*                                                               SHORT sType)
*
*    Input : TRANINFORMATION *pTran  -transaction information address
*            ITEMMISC *pItem         -item information address
*            SHORT      sType           -print type
*                                           PRT_WTIN: with time-in
*                                           PRT_WOIN: without time-in
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints ETK time-out line
*===========================================================================
*/
VOID  PrtVLEtkTimeOut(TRANINFORMATION *pTran, ITEMMISC *pItem, SHORT sType)
{
	TCHAR   aszDateIn[PRT_DATETIME_LEN + 1] = { 0 };
	TCHAR   aszDateOut[PRT_DATETIME_LEN + 1] = { 0 };

    /* -- get job time-out code -- */
    PrtGetJobOutDate(aszDateOut, TCHARSIZEOF(aszDateOut), pTran, pItem);

    if ( sType == PRT_WOIN) {
        tcharnset(aszDateIn, _T('*'), PRT_DATETIME_LEN);
        aszDateIn[PRT_DATETIME_LEN] = '\0';
    } else {
        *aszDateIn = '\0';
    }

    PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLEtkTimeOut, aszDateIn, aszDateOut);

}

/*
*===========================================================================
** Format  : VOID  PrtVLAccNum(UCHAR *pszNumber);
*
*    Input : UCHAR *pszNumber
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints account mnemonics and number.
*
*  Print format :
*
*       0        1         2         3         4         5
*       1234567890123456789012345678901234567890123456789012345
*      |^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
*      |         MMMMMMMM             9999999999999999999999999|
*      |                                                       |
*       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
*===========================================================================
*/
VOID  PrtVLAccNum(TCHAR *pszNumber)
{
    if ( pszNumber[0] != '\0') {
		TCHAR  aszAccMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };/* PARA_... defined in "paraequ.h" */

		RflGetTranMnem(aszAccMnem, TRN_NUM_ADR);

        PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLAccNum, aszAccMnem, pszNumber);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtVLOffline(UCHAR fbMod, USHORT usDate);
*
*    Input : UCHAR fbMod           -offline modifier
*            USHORT usDate         -expiration date
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints prints EPT offline symbol
*            and expiration date.
*
*  Print format :
*
*       0        1         2         3         4         5
*       1234567890123456789012345678901234567890123456789012345
*      |^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
*      |OFFL       MM/YY  123456     EPT TEND              1.00|
*      |                                                       |
*       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
*  Change Description: SR 131 cwunn 7/22/2003
*		SR 131 requests that EPT and Charge Posting operations allow for
*		masking of account numbers, depending on MDC settings.  In
*		compliance with SR 131 and pending Visa and Mastecard merchant
*		agreements, account numbers can be masked by configuring
*		MDC Bit 377, CP & EPT Account Number Security Settings
*		and MDC Bit 378, CP & EPT Account Number Masking Settings.
*		Options include printing or not printing account # and date for
*		EPT or CP operations, and masking or not masking account # for
*		EPT or CP operations, when performed using certain tender keys.
*		(EPT  can mask account #s on keys 2-6, CP can mask on keys 7-11).
*		The number of digits masked in CP and EPT are indpendently configured.
*===========================================================================
*/
VOID  PrtVLOffline(USHORT usAdr, ITEMTENDER *pItem)
{
	TCHAR  aszApproval[NUM_APPROVAL + 1] = { 0 };
	TCHAR  aszDate[NUM_EXPIRA + 1] = { 0 };
	TCHAR  aszOffMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };
	TCHAR  aszOffDate[5 + 1] = { 0 };
	TCHAR  aszRoomNo[NUM_ROOM + 1] = { 0 };
	TCHAR  aszGuestID[NUM_GUESTID + 1] = { 0 };
	TCHAR  aszRoomNoSMnem[4 + 1] = { 0 };
	TCHAR  aszGuestIDSMnem[4 + 1] = { 0 };
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };
    USHORT usOffDate;
	PARAMDC TenderMDC = { 0 };
	//SR 131 cwunn 7/16/2003 Tender Digit Masking 
	UCHAR	localMDCBitA, localMDCMask;

    _tcsncpy( aszApproval, pItem->auchApproval, NUM_APPROVAL);
    _tcsncpy( aszDate, pItem->auchExpiraDate, NUM_EXPIRA);
    _tcsncpy( aszRoomNo, pItem->aszRoomNo, NUM_ROOM);
    _tcsncpy( aszGuestID, pItem->aszGuestID, NUM_GUESTID);

    if ( pItem->fbModifier & OFFEPTTEND_MODIF ||  pItem->fbModifier & OFFCPTEND_MODIF ) {
        /* -- get special mnemonics -- */
		RflGetSpecMnem( aszOffMnem, SPC_OFFTEND_ADR);
    }

	/*SR 131 cwunn 7/15/2003
		Since odd tender keys use even MDC pages and even tender keys use odd MDC
		pages to indicate their charge posting/EPT settings, the following code
		determines which type of MDC page is used.
		localMDCBitA is always the bottom radio button selection on	an MDC page,
		odd mdc bits use ODD_MDC_BIT3/ODD_MDC_MASK and even use EVEN_MDC_BIT3/EVEN_MDC_MASK
		For the MDC pages in question MDC bit A indicates if the
		current tender key causes EPT (on) or Charge Posting (off).
	*/
	TenderMDC.uchMajorClass = CLASS_PARAMDC;
	TenderMDC.usAddress = MDC_CPTEND1_ADR + (pItem->uchMinorClass - 1);
	ParaRead(&TenderMDC);
	localMDCBitA = (TenderMDC.usAddress % 2) ? ODD_MDC_BIT3 : EVEN_MDC_BIT3;
	localMDCMask = (TenderMDC.usAddress % 2) ? MDC_ODD_MASK : MDC_EVEN_MASK;

    if ( aszRoomNo[0] != '\0' ) {
		RflGetSpecMnem( aszRoomNoSMnem, SPC_ROOMNO_ADR);
    }

    if ( aszGuestID[0] != '\0' ) {
		RflGetSpecMnem( aszGuestIDSMnem, SPC_GUESTID_ADR);
    }

    if ( aszDate[0] != '\0' ){                      /* expiration date  */
        usOffDate = _ttoi(aszDate);
        if (usOffDate != 0) {                       /* E-028 corr. 4/20     */
			//SR 131 cwunn 7/15/2003 Add MDC check to determine if expiration date should be printed
			//if tender status indicates EPT
			if((TenderMDC.uchMDCData & localMDCMask) == localMDCBitA){
				//then check if MDC indicates print expiration date on EPT Tender,
				if(!CliParaMDCCheck(MDC_EPT_MASK1_ADR, ODD_MDC_BIT0)){
					//then print the expiration date
					RflSPrintf( aszOffDate, TCHARSIZEOF(aszOffDate), aszPrtVLOffDate, usOffDate / 100, usOffDate % 100 );
				}
			}
			else {	//if tender status indicates CP
				if((TenderMDC.uchMDCData & localMDCMask) != localMDCBitA){ //only bit 3, ept is not CP
					//then check if MDC indicates print expiration date on CP Tender
					if(!CliParaMDCCheck(MDC_EPT_MASK1_ADR, ODD_MDC_BIT1)){
						RflSPrintf( aszOffDate, TCHARSIZEOF(aszOffDate), aszPrtVLOffDate, usOffDate / 100, usOffDate % 100 );
					}
				}
				else { //tender status indicates non-EPT or CP tender, expiration date
					RflSPrintf( aszOffDate, TCHARSIZEOF(aszOffDate), aszPrtVLOffDate, usOffDate / 100, usOffDate % 100 );
				}
			}
        }
    }

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- send mnemoncis and exp.date -- */
    PmgPrintf( PMG_PRT_RCT_JNL, aszPrtVLOffline, aszOffMnem, aszOffDate, aszApproval,
               aszRoomNoSMnem, aszRoomNo, aszGuestIDSMnem, aszGuestID, aszTranMnem, pItem->lTenderAmount );

}

/*
*===========================================================================
** Format  : VOID  PrtVLPLUBuild (UCHAR *puchPLUNo, UCHAR uchDeptNo);
*
*   Input  : UCHAR   uchAdr
*          : UCHAR  *puchPLUCpn
*
*   Output : none
*   InOut  : none
*
** Return  : none
*
** Synopsis: This function prints PLU no and Dept no.
*
*  Print format :
*
*       0        1         2         3         4         5
*       1234567890123456789012345678901234567890123456789012345
*      |^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
*      |PLU-BUIL                            99999999999999 9999|
*      |                                                       |
*       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
*===========================================================================
*/
VOID    PrtVLPLUBuild(USHORT usAdr, TCHAR *puchPLUNo, USHORT usDeptNo)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 }; /* PARA_... defined in "paraequ.h" */

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    PmgPrintf( PMG_PRT_RCT_JNL, aszPrtVLPluBuild, aszTranMnem, puchPLUNo, usDeptNo);

}

/** Check Endorsement, 21RFC05402 ***/
/*
*===========================================================================
** Format  : VOID  PrtVLEndosHead(UCHAR uchPrintFormat);
*
*    Input : UCHAR uchPrintFormat
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints check endorsement header line.
*
*  Print format :
*
*      Horizontal Insert:
*
*       0        1         2         3         4         5
*       1234567890123456789012345678901234567890123456789012345
*      |^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^:^^^^^^^^^^^^^^^^^^^^^|
*      |                                 :  AAAAAAAAAAAAAAAAA  |
*      |                                 :  BBBBBBBBBBBBBBBBB  |
*      |                                 :                     |
*       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
*                   OR
*      Vertical Insert:
*
*       0        1         2         3         4         5
*       1234567890123456789012345678901234567890123456789012345
*      |^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
*      |                 AAAAAAAAAAAAAAAAAAAAAAAABBBBBBBBBBBBBB|
*      |                                                       |
*       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
*===========================================================================
*/
VOID  PrtVLEndosHead(TCHAR uchPrintFormat)
{
	TCHAR   aszHead1[PARA_CHAR24_LEN + 1] = {0};     /*  header 1st line */
    TCHAR   aszHead2[PARA_CHAR24_LEN + 1] = {0};     /*  header 1st line */

    PrtGet24Mnem(aszHead1, CH24_1STCE_ADR);    /* get check endose head */
    PrtGet24Mnem(aszHead2, CH24_2NDCE_ADR);    /* get check endose head */

    if ( uchPrintFormat & PRT_ENDORSE_HORIZNTL ) {
        aszHead1[17] = '\0';
        aszHead2[17] = '\0';
        PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLHznHead, aszHead1);
        PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLHznHead, aszHead2);
    } else {
        aszHead2[13] = '\0';
        PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLVrtHead, aszHead1, aszHead2);
    }

}

/*
*===========================================================================
** Format  : VOID  PrtVLEndosTrngHead(UCHAR uchPrintFormat);
*
*    Input : UCHAR uchPrintFormat
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints check endorsement header line.
*
*  Print format :
*
*      Horizontal Insert:
*
*       0        1         2         3         4         5
*       1234567890123456789012345678901234567890123456789012345
*      |^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^:^^^^^^^^^^^^^^^^^^^^^|
*      |                                 :  TTTTTTTTTTTTTTTTT  |
*      |                                 :                     |
*       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
*                   OR
*      Vertical Insert:
*
*       0        1         2         3         4         5
*       1234567890123456789012345678901234567890123456789012345
*      |^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
*      |                 TTTTTTTTTTTTTTTTTTTTTTTT              |
*      |                                                       |
*       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
*===========================================================================
*/
VOID  PrtVLEndosTrngHead(TCHAR uchPrintFormat)
{
    TCHAR   asz24Training[PARA_CHAR24_LEN + 1] = {0};     /*  header 1st line */
	TCHAR   aszDummy[PARA_CHAR24_LEN + 1] = {0};          /*  dummy buffer */
    TCHAR   asz8Training[PARA_TRANSMNEMO_LEN + 1] = {0};  /* training mnemo. */

    if ( uchPrintFormat & PRT_ENDORSE_HORIZNTL ) {
        /*-- Horizontal Insert --*/
		RflGetSpecMnem( asz8Training, SPC_TRNGID_ADR );    /* seat no. mnemonic */
        PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLHznHead, asz8Training);
    } else {
		/*-- Vertical Insert --*/
        PrtGet24Mnem(asz24Training, CH24_TRNGHED_ADR);    /* get 24-char training mode */
        PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLVrtHead, asz24Training, aszDummy);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtVLEndosTrail(UCHAR uchPrintFormat,
*                                  TRNINFORMATION *pTran);
*
*   Input  : UCHAR             uchPrintFormat
*            TRNINFORMATION    *pTran       -transaction information
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints validation trailer line.
*
*  Print format :
*
*      Horizontal Insert:
*
*       0        1         2         3         4         5
*       1234567890123456789012345678901234567890123456789012345
*      |^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
*      |                                      9999             |
*      |                                      2170-123 MM/DD/YY|
*      |                                                       |
*       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
*                              OR
*       0        1         2         3         4         5
*       1234567890123456789012345678901234567890123456789012345
*      |^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
*      |                                      9999 999 BARBARA*|
*      |                                      2170-123 MM/DD/YY|
*      |                                                       |
*       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
*      Vertical Insert:
*
*       0        1         2         3         4         5
*       1234567890123456789012345678901234567890123456789012345
*      |^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
*      |                  9999 2170-123                MM/DD/YY|
*      |                                                       |
*       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
*                           OR
*       0        1         2         3         4         5
*       1234567890123456789012345678901234567890123456789012345
*      |^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
*      |                  9999 2170-123 999 BARBARA*** MM/DD/YY|
*      |                                                       |
*       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
*===========================================================================
*/
VOID  PrtVLEndosTrail(TCHAR uchPrintFormat, TRANINFORMATION *pTran)
{
	CONST TCHAR   *aszPrtVLVrtTrail = _T("\t%04d %04lu-%03lu                %s");         /* check endorsement trailer w/o cashier data */
	CONST TCHAR   *aszPrtVLVrtTrailCas = _T("\t%04d %04lu-%03lu %03d %-10s %s");          /* check endorsement trailer w/ cashier data */
	CONST TCHAR   *aszPrtVLVrtTrailClk = _T("\t%04d %04lu-%03lu %04d%-10s %s");           /* check endorsement trailer w/ cashier data */
	CONST TCHAR   *aszPrtVLVrtTrailCasClk = _T("\t%04d %04lu-%03lu %03d %04d       %s");  /* check endorsement trailer w/ cashier clerk data*/

    PARASTOREGNO    StRegNoRcvBuff;
	TCHAR  aszDate[PRT_DATETIME_LEN + 1] = { 0 };
	TCHAR  aszCashierMnem[NUM_OPERATOR] = { 0 };    /* NUM_OPERATOR is 10+1 */
	TCHAR  aszClerkMnem[NUM_OPERATOR] = { 0 };      /* NUM_OPERATOR is 10+1 */
    USHORT    usLen = 0;

    StRegNoRcvBuff.uchMajorClass = CLASS_PARASTOREGNO;
    StRegNoRcvBuff.uchAddress = SREG_NO_ADR;
    CliParaRead( &StRegNoRcvBuff );

    /*--  get date time --*/
    PrtGetDate(aszDate, TCHARSIZEOF(aszDate), pTran);

    _tcsncpy(aszCashierMnem, pTran->TranModeQual.aszCashierMnem, NUM_OPERATOR);
    _tcsncpy(aszClerkMnem,   pTran->TranModeQual.aszWaiterMnem, NUM_OPERATOR);

    /*-- get string length of aszDate --*/
    usLen  = _tcslen(aszDate);

    if ( uchPrintFormat & PRT_ENDORSE_HORIZNTL ) {
		/*---- Horizontal Insert ----*/
        /*--- cashier exists ---*/
        if ( ( pTran->TranModeQual.ulCashierID != 0 ) && ( pTran->TranModeQual.ulWaiterID   == 0 ) ) {
            aszCashierMnem[8] = '\0';
            PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLHznTrailCas1, pTran->TranCurQual.usConsNo, pTran->TranModeQual.ulCashierID, aszCashierMnem);
            PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLHznTrailCas2, (ULONG)StRegNoRcvBuff.usStoreNo, (ULONG)StRegNoRcvBuff.usRegisterNo, &aszDate[usLen-8]);
        /*--- cashier clerk system ---*/
        } else if ( ( pTran->TranModeQual.ulCashierID != 0 ) && ( pTran->TranModeQual.ulWaiterID   != 0 ) ) {
            PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLHznTrailCasClk1, pTran->TranCurQual.usConsNo, pTran->TranModeQual.ulCashierID, pTran->TranModeQual.ulWaiterID);
            PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLHznTrailCas2, (ULONG)StRegNoRcvBuff.usStoreNo, (ULONG)StRegNoRcvBuff.usRegisterNo, &aszDate[usLen-8]);
        /*--- clerk system ---*/
        } else if ( ( pTran->TranModeQual.ulCashierID == 0 ) && ( pTran->TranModeQual.ulWaiterID   != 0 ) ) {
            aszClerkMnem[8] = '\0';
            PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLHznTrailClk1, pTran->TranCurQual.usConsNo, pTran->TranModeQual.ulWaiterID, aszClerkMnem);
            PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLHznTrailCas2, (ULONG)StRegNoRcvBuff.usStoreNo, (ULONG)StRegNoRcvBuff.usRegisterNo, &aszDate[usLen-8]);
        } else {
            PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLHznTrail1, pTran->TranCurQual.usConsNo);
            PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLHznTrail2, (ULONG)StRegNoRcvBuff.usStoreNo, (ULONG)StRegNoRcvBuff.usRegisterNo, &aszDate[usLen-8]);
        }
    } else {
		/*---- Vertical Insert ----*/
        /*--- cashier exists ---*/
        if ( ( pTran->TranModeQual.ulCashierID != 0 ) && ( pTran->TranModeQual.ulWaiterID   == 0 ) ) {
            aszCashierMnem[10] = '\0';
            PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLVrtTrailCas, pTran->TranCurQual.usConsNo, (ULONG)StRegNoRcvBuff.usStoreNo, (ULONG)StRegNoRcvBuff.usRegisterNo, pTran->TranModeQual.ulCashierID, aszCashierMnem, &aszDate[usLen-8]);
        /*--- cashier clerk system ---*/
        } else if((pTran->TranModeQual.ulCashierID != 0) && (pTran->TranModeQual.ulWaiterID != 0)){
            PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLVrtTrailCasClk, pTran->TranCurQual.usConsNo, (ULONG)StRegNoRcvBuff.usStoreNo, (ULONG)StRegNoRcvBuff.usRegisterNo, pTran->TranModeQual.ulCashierID, pTran->TranModeQual.ulWaiterID, &aszDate[usLen-8]);
        /*--- clerk system ---*/
        } else if((pTran->TranModeQual.ulCashierID == 0) && (pTran->TranModeQual.ulWaiterID != 0)){
            aszClerkMnem[10] = '\0';
            PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLVrtTrailClk, pTran->TranCurQual.usConsNo, (ULONG)StRegNoRcvBuff.usStoreNo, (ULONG)StRegNoRcvBuff.usRegisterNo, pTran->TranModeQual.ulWaiterID, aszClerkMnem, &aszDate[usLen-8]);
        } else {
            PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLVrtTrail, pTran->TranCurQual.usConsNo, (ULONG)StRegNoRcvBuff.usStoreNo, (ULONG)StRegNoRcvBuff.usRegisterNo, &aszDate[usLen-8]);
        }
    }
}

/*
*===========================================================================
** Format  : VOID  PrtVLEndosMnemAmt(UCHAR uchPrintFormat,
*                                    UCHAR uchAdr, LONG lAmout);
*
*    Input : UCHAR uchPrintFormat
*            UCHAR uchAdr          -Transacion mnemonics address
*            LONG  lAmount         -Amount
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints prints transaction mnemonics
*            and Amount line for check endorsement.
*
*  Print format :
*
*       Horizontal Insert:
*
*       (sign mnemonics) + (amount) length is in 8 digit.
*
*       0        1         2         3         4         5
*       1234567890123456789012345678901234567890123456789012345
*      |^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^:^^^^^^^^^^^^^^^^^^^|
*      |                                    : MMMMMMMM S9999.99 |
*      |                                    :                   |
*       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
*                               OR
*
*       (sighn mnemonics) + (amount) length is over 8 digit.
*
*       0        1         2         3         4         5
*       1234567890123456789012345678901234567890123456789012345
*      |^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^:^^^^^^^^^^^^^^^^^^^|
*      |                                    :       S9999999.99 |
*      |                                    :                   |
*       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
**      Vertical Insert:
*       0        1         2         3         4         5
*       1234567890123456789012345678901234567890123456789012345
*      |^^^^^^^^^^^^^^^^^^:^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
*      |                  : MMMMMMMM               S99999999.99 |
*      |                  :                                     |
*       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
*===========================================================================
*/
VOID  PrtVLEndosMnemAmt(TCHAR uchPrintFormat, USHORT usAdr, DCURRENCY lAmount)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    if ( uchPrintFormat & PRT_ENDORSE_HORIZNTL ) {
		/*-- horizontal insertion --*/
        /* -- send mnemoncis and amount -- */
        /* bug fixed at 03/21/97 */
        if ((lAmount < 10000000L) && (lAmount > -1000000L)) {
            PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLHznMnemAmt, aszTranMnem, lAmount);
        } else {
            PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLHznAmt, lAmount);
        }
    } else {
		/*-- vertical insertion --*/
        /* -- send mnemoncis and amount -- */
        PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLVrtMnemAmt, aszTranMnem, lAmount);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtVLEndosMnemCSAmt(UCHAR uchPrintFormat,
*                                      UCHAR uchAdr, LONG lAmout);
*
*    Input : UCHAR uchPrintFormat
*            UCHAR uchAdr          -Transacion mnemonics address
*            LONG  lAmount         -Amount
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints prints transaction mnemonics
*            and Amount line for check endorsement.
*
*  Print format :
*
*       Horizontal Insert:
*
*       (sighn mnemonics) + (currency symbol) + (amount) length is
*        in 8 digit.
*
*       0        1         2         3         4         5
*       1234567890123456789012345678901234567890123456789012345
*      |^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^:^^^^^^^^^^^^^^^^^^^|
*      |                                    : MMMMMMMM S$$$9.99 |
*      |                                    :                   |
*       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
*                               OR
*
*       (sighn mnemonics) + (currency symbol) + (amount) length is
*        over 8 digit.
*
*       0        1         2         3         4         5
*       1234567890123456789012345678901234567890123456789012345
*      |^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^:^^^^^^^^^^^^^^^^^^^|
*      |                                    :    S$$$9999999.99 |
*      |                                    :                   |
*       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
**      Vertical Insert:
*       0        1         2         3         4         5
*       1234567890123456789012345678901234567890123456789012345
*      |^^^^^^^^^^^^^^^^^^:^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
*      |                  : MMMMMMMM               S$$$99999.99 |
*      |                  :                                     |
*       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
*===========================================================================
*/
VOID  PrtVLEndosMnemCSAmt(TCHAR uchPrintFormat, USHORT usAdr, DCURRENCY lAmount)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};
    TCHAR  aszSpecAmt[PARA_SPEMNEMO_LEN +PRT_AMOUNT_LEN + 1] = {0};

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- adjust native mnemonic and amount sign(+.-) -- */
    PrtAdjustNative(aszSpecAmt, lAmount);

    if ( uchPrintFormat & PRT_ENDORSE_HORIZNTL ) {
		/*-- horizontal insertion --*/
        /* -- send mnemoncis and amount -- */
        if ((_tcslen(aszSpecAmt)) < 9) {
            PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLHznMnemCSAmt, aszTranMnem, aszSpecAmt);
        } else {
            PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLHznCSAmt, aszSpecAmt);
        }
    } else {
		/*-- vertical insertion --*/
        /* -- send mnemoncis and amount -- */
        PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLVrtMnemCSAmt, aszTranMnem, aszSpecAmt);
    }
}

/* ===== End of File ( PrRCmnVL.C ) ===== */