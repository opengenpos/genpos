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
* Title       : Print common routine, reg mode, receipt & journal
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRCmnS_.C
*               | ||  ||
*               | ||  |+- S:21Char L:24Char _:not influcenced by print column
*               | ||  +-- R:recept & jounal S:slip V:validation K:kitchen
*               | |+----- common routine
*               | +------ R:reg mode S:super mode
*               +-------- Pr:print module
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*    PrtSPVoidNumber() : set 'void', 'number' and number
*    PrtSPMnemAmt() : set transaction mnemonics and amount
*    PrtSPDiscount() : set transaction mnemonics, discount rate, and amount
*#   PrtSPCoupon() : set coupon mnemonics and modifier, amount
*    PrtSPTaxMod() : set tax modifier mnemonics
*#   PrtSPMnemTaxQty() : set transaction mnemonics and amount
*    PrtSPMnemTaxAmt() : set trans. mnemonics, tax mod. mnemonics, and amount
*    PrtSPSeatNo() : set seat no. mnemonic and no R3.1
*#   PrtSPQty() : set tax mod. mnemonics, quantity, and unit price
*    PrtSPWeight() : set tax modifier, weight, and "manual" mnemonic
*    PrtSPMnlWeight() : set weight symbol and unit price
*    PrtSPTranMnem() : set transaction mnemonics
*#   PrtSPTblPerson() : set table No., and No. of person
*#   PrtSPCustomerName() : set customer name
*    PrtSPMultiSeatNo() : set seat no. mnemonic and no R3.1
*    PrtSPChild() : set child plu
*    PrtSPTipsPO() : set waiter ID, tips paid out mnemonics and amount
*    PrtSPGCTransfer() : set waiter ID and guest check no.
*    PrtSPGstChkNo():
*    PrtSpForeign() : set foeign tender amount and conversion rate
*    PrtSPMnemNativeAmt() : set trans. and native mnemonic, and amount
*    PrtSPServTaxSum() : set trans and native mnemonic, and amount
*    PrtSPChargeTips() : set waiter mnem. and ID, charge tips mnem., rate, and amount
*    PrtSPCancel() : set transaction cancel data
*    PrtSPTrail1() : set trailer data
*    PrtSPTrail2() : set trailer data
*    PrtSPChkPaid() : set check paid data
*    PrtSPTrayCount(): set tray total mnem. and counter
*    PrtSPServiceTotal() : set the data in service total operation
*    PrtSPGCTranTrail() : set the trailer data in guest check transfer
*#   PrtSPHeader1() : set header 1st line
*    PrtSPHeader2() : set header 2nd line
*#   PrtSPHeader3() : set header 3rd line
*    PrtSPItems() : set adj., noun, print mod., cond., amount/ rate and amount
*    PrtSPEtkTimeIn() :
*    PrtSPEtkTimeOut():
*    PrtCheckLine() : check the rest of the slip lines
*    PrtSPCPRoomCharge() : set room no. and guest id
*    PrtSPCPOffTender() : set off line tender
*    PrtSPCPRspMsgText() : set response message text
*    PrtSPOffline()      : set EPT offline symbol & expiration date
*    PrtTrailEPT_SP()    : EPT trailer logo
*    PrtSPTranNumber ()  : set number and transaction mnem.
*    PrtSPVLTrail()      : print validation trailer to slip
*    PrtSPVLHead()       : print validation header to slip
*    PrtSPVLHead1()      : slip validation header 1st line
*    PrtSPVLHead2()      : slip validation header 2nd line
*    PrtSPVLHead3()      : slip validation header 3rd line
*
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
* Jun-19-92 : 01.00.12 :  R.Itoh    :
* Jun-29-93 : 01.00.12 :  K.You     : mod. PrtSPMnemTaxQty, PrtSPQty for US enhance.
* Jul-03-93 : 01.00.12 :  R.Itoh    : add PrtSPEtkTimeIn(), PrtSPEtkTimeOut().
* Jul-07-93 : 01.00.12 :  K.You     : mod. PrtSPTrail1 for US enhance.
* Apr-07-94 : 00.00.04 : Yoshiko.J  : add PrtSPOffline(), PrtTrailEPT_SP(),
*                                   : PrtSPTranNumber()
* Apr-08-94 : 00.00.04 :  K.You     : add validation slip print feature.(add. PrtSPVLTrail,
*           :          :            : PrtSPVLHead, PrtSPVLHead1, PrtSPVLHead2, PrtSPVLHead3)
* Apr-24-94 : 00.00.05 : K.You      : bug fixed E-32 (mod. PrtSPCPOffTend)
* Apr-26-94 : 00.00.05 : K.You      : bug fixed E-33,35,41 (mod. PrtSPCPRspMsgText)
* Apr-25-95 : 03.00.00 : T.Nakahata : Add Promotional PLU with Condiment
* May-30-95 : 03.00.00 : T.Nakahata : add coupon feature (PrtSPCoupon) and
*                                     unique trans no.(PrtSPTblPerson, PrtSPHeader1)
* Aug-29-95 : 03.00.04 : T.Nakahata : FVT comment (unique tran#, 4 => 12 Char)
*           :          :            : 16 char alpha mnemonics
* Nov-15-95 : 03.01.00 : M.Ozawa    : Change ETK format at R3.1
* Sep-17-98 : 03.03.00 : hrkato     : V3.3(Check Transfer Print)
* Aug-08-99 : 03.05.00 : M.Teraki   : Remove WAITER_MODEL
* Aug-16-99 : 03.05.00 : M.Teraki   : Merge STORE_RECALL_MODEL
*           :          :            :       /GUEST_CHECK_MODEL
* Nov-29-99 : 01.00.00 : hrkato     : Saratoga (Money)
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
;+                  I N C L U D E     F I L E s                         +
;========================================================================
**/
/**------- M S - C -------**/
#include	<tchar.h>
#include <stdlib.h>
#include <string.h>

/**------- 2170 local------**/
#include "ecr.h"
#include "pif.h"
#include "regstrct.h"
#include "transact.h"
#include "paraequ.h"
#include "para.h"
#include "csttl.h"
#include "csop.h"
#include "report.h"
#include "csstbpar.h"
#include "uie.h"
/* #include "fsc.h" */
#include "pmg.h"
#include "rfl.h"
#include "fsc.h"
#include "maint.h"
#include "prtrin.h"
#include "prtsin.h"
#include "prrcolm_.h"


/**
;========================================================================
;+             P R O G R A M    D E C L A R A T I O N s
;========================================================================
**/

/**
;========================================================================
;+                    S T A T I C    R A M s
;========================================================================
**/
CONST TCHAR FARCONST  aszPrtSPMnemAmt[] = _T("%-32s %11l$");
                                    /* trans. mnem and amount */
CONST TCHAR FARCONST  aszPrtSPTaxRateAmt[] = _T("%-16s %7.2l$%%\t %l$");
                                    /* trans. mnem, vat rate, and amount */
CONST TCHAR FARCONST  aszPrtSPMnemAmtD[] = _T("%.8s\t %l$"); /*8 characters JHHJ*/
                                    /* trans. mnem and amount */

CONST TCHAR FARCONST  aszPrtSPTaxMnem[] = _T("          %10s  %10s  %10s");
                                    /* tax mnemnics */
CONST TCHAR FARCONST  aszPrtSPTaxAmt[] = _T("%5.2l$%%  %12l$%12l$%12l$");
                                    /* tax amount */
CONST TCHAR FARCONST  aszPrtSPDiscount[] = _T("%-27s %3d%% %11l$");
                                    /* trans. mnem, rate, and amount */
CONST TCHAR FARCONST  aszPrtSPTaxDisc[] = _T("%-8.8s %4s               %3d%% %11l$"); /*8 characters JHHJ*/
                                    /* trans. mnem, rate, and amount */
CONST TCHAR FARCONST  aszPrtSPTaxMod[] = _T("%s");
                                    /* tax modifier mnemonics */
CONST TCHAR FARCONST  aszPrtSPMnemTax[] = _T("%-8.8s %s");/*8 characters JHHJ*/
                                    /* trans. mnem, and tax mnem */
CONST TCHAR FARCONST  aszPrtSPMnemTaxQty[] = _T("%-8.8s %-14s    %4ld X %9l$");/*8 characters JHHJ*/
                                    /* mod. disc. mnem, tax mod. mnem, qty, and unit price */
CONST TCHAR FARCONST  aszPrtSPMnemTaxAmt[] = _T("%-8.8s %-14s          %11l$"); /*8 characters JHHJ*/
                                    /* trans. mnem, tax mnem, and amount */
CONST TCHAR FARCONST  aszPrtSPSeat[] = _T("%-4s %1d");       /* seat no. */

CONST TCHAR FARCONST  aszPrtSPQty[] = _T("%-14s             %4ld X %9l$");
                                    /* tax mnem, quantity, and unit price */
CONST TCHAR FARCONST  aszPrtSPPPIQty[] = _T("%-14s         %4ld X %9l$/%ld ");
                                    /* tax mnem, quantity, and unit price */
CONST TCHAR FARCONST  aszPrtSPScale[] = _T("@ %l$/%s");
                                    /* scalable item unit price */
CONST TCHAR FARCONST  aszPrtSPWeight1[] = _T("%-14s%8.3l$%-4s %16s");
                                    /* tax mnem, weight, and unit price */
CONST TCHAR FARCONST  aszPrtSPWeight2[] = _T("%-14s%8.2l$%-4s %16s");
                                    /* tax mnem, weight, and unit price */
CONST TCHAR FARCONST  aszPrtSPMnlWeight1[] = _T("%-14s        %8.3l$%-4s %-8.8s"); /*8 characters JHHJ*/
                                    /* tax mnem, weight, and mnemonic */
CONST TCHAR FARCONST  aszPrtSPMnlWeight2[] = _T("%-14s        %8.2l$%-4s %-8.8s"); /*8 characters JHHJ*/
                                    /* tax mnem, weight, and mnemonic */
CONST TCHAR FARCONST  aszPrtSPMnlWeight3[] = _T("                           %16s");
                                    /* tax mnem, weight, and mnemonic */
CONST TCHAR FARCONST  aszPrtSPTranMnem[] = _T("%-8.8s"); /*8 characters JHHJ*/
                                    /* transaction mnemonics */
CONST TCHAR FARCONST  aszPrtSPTblPerson[] = _T("%-4s %-3s     %-8.8s %-3s"); /*8 characters JHHJ*/
                                    /* table No and No. of person */
/****************** Add Unique Transaction Number (REL 3.0) ****************/
CONST TCHAR FARCONST  aszPrtSPTransPerson[] = _T("%-12s %-8s     %-8.8s %-3s");/*8 characters JHHJ*/
                                    /* unique trans. no. and No. of person */
CONST TCHAR FARCONST  aszPrtSPCustomerName[] = _T("%-19s");
/****************** Add Unique Transaction Number (REL 3.0) ****************/

CONST TCHAR FARCONST  aszPrtSPMultiSeat[] = _T("%-s");                /* seat no. */

CONST TCHAR FARCONST  aszPrtSPChildPLU[] = _T(" %-4s %-12s");         /* adj. mnem., and child PLU mnem. */
CONST TCHAR FARCONST  aszPrtSPModLinkPLU1[] = _T("%-4s %-s\t%l$ ");   /* adj. mnem., and child PLU mnem. */
CONST TCHAR FARCONST  aszPrtSPModLinkPLU2[] = _T("%-s\t%l$ ");
CONST TCHAR FARCONST  aszPrtSPLinkPLU1[] = _T("%-4s %-s\t%l$");       /* adj. mnem., and child PLU mnem. */
CONST TCHAR FARCONST  aszPrtSPLinkPLU2[] = _T("%-s\t%l$");            /* adj. mnem., and child PLU mnem. */

CONST TCHAR FARCONST  aszPrtSPTipsPO[] = _T("%-4s %03d                %-8.8s %11l$");    /*8 characters JHHJ, waiter mnem. and ID, and tips p/o mnem. and amount */

CONST TCHAR FARCONST  aszPrtSPGCTran[] = _T("%-.8s  %03d       %-8s %4s");               /* old waiter mnem. and ID, and G/C mnem and number */
CONST TCHAR FARCONST  aszPrtSPGCTranCdv[] = _T("%-.8s  %03d       %-8s %4s%02d");        /* old waiter mnem. and ID, and G/C mnem and number */

CONST TCHAR FARCONST  aszPrtSPChkPaid[] = _T("--------------%-16s--------------");       /* check paid mnemonic (double wide) */

CONST TCHAR FARCONST  aszPrtSPGCNo[] = _T("                       %-8.8s   %4s");        /*8 characters JHHJ, check paid mnemonic and guest check No. */
CONST TCHAR FARCONST  aszPrtSPGCNoCdv[] = _T("                       %-8.8s   %4s%02d"); /*8 characters JHHJ, check paid mnemonic and guest check No. (with Cdv) */
CONST TCHAR FARCONST  aszPrtSPTrayCo[] = _T("%-8.8s                              %6d");  /*8 characters JHHJ, tray total co. mnemonic and counter */
CONST TCHAR FARCONST  aszPrtSPForeign1[] = _T("%.*l$ / %10.*l$");                        /* V3.4 , foreign amount and conversion rate */
CONST TCHAR FARCONST  aszPrtSPForeign2[] = _T("%-44s");                                  /* foreign amount and conversion rate */
CONST TCHAR FARCONST  aszPrtSPMnemNatAmtS[] = _T("%-8.8s                     %15s");     /*8 characters JHHJ, transaction mnem, and native mnem and amount */
CONST TCHAR FARCONST  aszPrtSPMnemNatAmtD[] = _T("%-8.8s      %30s");                    /*8 characters JHHJ, transaction mnem, and native mnem and amount */
CONST TCHAR FARCONST  aszPrtSPSevTaxSumS[] = _T("                    %-.8s");            /*8 characters JHHJ, tax / check sum mnem. */
CONST TCHAR FARCONST  aszPrtSPSevTaxSum[] = _T("                    %-8.8s %15s");       /*8 characters JHHJ, tax / check sum mnem. and amount */
CONST TCHAR FARCONST  aszPrtSPCancel[] = _T("%-16s   %-8.8s      %11l$");                /*8 characters JHHJ, cancel mnem., cancel total mnem, and amount */
CONST TCHAR FARCONST  aszPrtSPTrail2[]  = _T("   %04d %-4s %8.8Mu  %04lu-%03lu %16s");   /* trailer  */
CONST TCHAR FARCONST  aszPrtSPServTotal1[]  = _T("%02d %04d %-4s %03d    %-8.8s");       /*8 characters JHHJ, trailer in service total */
CONST TCHAR FARCONST  aszPrtSPServTotal2[]  = _T("%-28s %15s");                          /* trailer in service total */
CONST TCHAR FARCONST  aszPrtSPGCTrnTril[]  = _T("                       %-8.8s  %11l$"); /*8 characters JHHJ, check transfer line */
CONST TCHAR FARCONST  aszPrtSPHdr1[] = _T("%-4s %8.8Mu %-4s %-3s %-8.8s %4s   %4s");  /*8 characters JHHJ, header 1st line */
CONST TCHAR FARCONST  aszPrtSPHdr1Cdv[] = _T("%-4s %8.8Mu %-4s %-3s %-8.8s %4s%02d %4s"); /*8 characters JHHJ, header 1st line */
CONST TCHAR FARCONST  aszPrtSPHeader2nd[] = _T("%-12.12s %-8.8s %-3s  %16s");            /* V3.3 , 8-2-2000  TAR# 148297 , 8 characters JHHJ, header 2nd line */
/****************** Add Unique Transaction Number (REL 3.0) ****************/
CONST TCHAR FARCONST  aszPrtSPHdrTrans[] = _T("%-12s %-8s   %-16s");                     /* header 1st line (Unique Tran #) */
/****************** Add Unique Transaction Number (REL 3.0) ****************/
CONST TCHAR FARCONST  aszPrtSPItemAmt[] = _T("%44s");                                    /* amount */
CONST TCHAR FARCONST  aszPrtSPEtkTimeIn[] = _T("%-16s\t%-3s");                           /* R3.1 , ETK time-in data */
CONST TCHAR FARCONST  aszPrtSPEtkTimeOut[] = _T("%-16s - %-16s");                        /* R3.1 , ETK time-out data */
CONST TCHAR FARCONST  aszItemAmt[] = _T("%l$");                                          /* amount */
CONST TCHAR FARCONST  aszCasWaiID[] = _T("%8.8Mu");                                      // Cashier number or Waiter number ulCashierNo or ulCashierId

CONST TCHAR FARCONST  aszEtkCode[] = _T("%-2d");

CONST TCHAR FARCONST  aszEtkEmp[] = _T("%4d");

CONST TCHAR FARCONST  aszPrtSPCPRoomCharge[] = _T("%-4s%6s\t %-4s%3s");                  /* room charge */

CONST TCHAR FARCONST  aszPrtSPCPRspMsgText[] = _T("%s");
CONST TCHAR FARCONST  aszPrtSPOffline[] = _T("%-4s   %5s   %6s");                        /* mnemo.  date */
CONST TCHAR FARCONST  aszPrtSPOffDate[] = _T("%02d/%02d");                               /* expiration date  */
CONST TCHAR FARCONST  aszPrtSPMnem[] = _T("%-s");                                        /* mnemonics        */
CONST TCHAR FARCONST  aszPrtSPTranNumber[] = _T("        %-8.8s\t%25s");                 /*8 characters JHHJ, No. mnemo. & No. */

CONST TCHAR FARCONST  aszPrtSPVLHead[] = _T("%-s\t%s");                                  /* slip validation header */

extern CONST TCHAR FARCONST  aszPrtRJEuro1[];                                            /* Euro amount */
extern CONST TCHAR FARCONST  aszPrtRJEuroAmtMnem[];                                      /* trans.mnem and amount */
CONST TCHAR FARCONST  aszPrtSPPLUNo[] = _T("%s");
CONST TCHAR FARCONST  aszPrtSPMnemMnem[] = _T("%s\t %s");                                /* mnem. and mnem. */

CONST TCHAR FARCONST  aszPrtSPPluBuild[]      = _T("%s\t %13s %04d");                    /* Plu no and Dept no. */
CONST TCHAR FARCONST  aszPrtSPMoneyForeign[] = _T("%.*l$");                              /* foreign amount of money, Saratoga */
CONST TCHAR FARCONST  aszPrtSPQtyFor[] = _T("\t %3ld X %s / %2u ");                      /* qty, unit price, for */
CONST TCHAR FARCONST  aszPrtSPDecQty[] = _T("\t %6.*l$ X %s ");                          /* qty, unit price */
CONST TCHAR FARCONST  aszPrtSPDecQtyFor[] = _T("\t %6.*l$ X %s / %2u ");                 /* qty, unit price, for */
CONST TCHAR FARCONST  aszPrtSupSPHead[] = _T("   %-12s %6s  %-10s %6s");                 /* header line */

/*
*===========================================================================
** Format  : USHORT  PrtSPVoidNumber(UCHAR *pszWork, ITEMSALES *pItem);
*
*    Input : ITEMSALES  *pItem
*          : UCHAR      fbMod
*
*   Output : UCHAR  *pszWork         -destination buffer address
*    InOut : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets 'void', 'number' and number to the buffer
*            'pszWork'.
*===========================================================================
*/
USHORT PrtSPVoidNumber(TCHAR *pszWork, USHORT fbMod, USHORT usReasonCode, TCHAR *pszNumber)
{
    USHORT  usWrtLen = 0;

    /* -- get 'void' mnemonics -- */
    if (fbMod & (VOID_MODIFIER | RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) {
		if (0 > PrtGetReturns(fbMod, usReasonCode, pszWork, PARA_SPEMNEMO_LEN * 2 + 1)) {
			/* -- get void mnemonics instead of returns mnemonic -- */
			PrtGetVoid(pszWork, PARA_SPEMNEMO_LEN * 2 + 1);
		}
        usWrtLen = _tcslen(pszWork);
        tcharnset(&(pszWork[usWrtLen]), PRT_SPACE, 2);
        usWrtLen += 2;
    }

    /* -- get 'number' mnemonics -- */
    if (*pszNumber) {
		RflGetTranMnem(&(pszWork[usWrtLen]), TRN_NUM_ADR);
        usWrtLen = _tcslen(pszWork);
        pszWork[usWrtLen] = PRT_SPACE;
        usWrtLen++;
        _tcsncpy(&(pszWork[usWrtLen]), pszNumber, _tcslen(pszNumber));
    }

    if ( !(fbMod & VOID_MODIFIER) && (*pszNumber == '\0')) {
        return(0);
    }

    return(1);
}

/*
*===========================================================================
** Format  : USHORT  PrtSPVoidNumber(UCHAR *pszWork, ITEMSALES *pItem);
*
*    Input : ITEMSALES  *pItem
*          : UCHAR      fbMod
*
*   Output : UCHAR  *pszWork         -destination buffer address
*    InOut : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets 'void', 'number' and number to the buffer
*            'pszWork'.
*===========================================================================
*/
USHORT PrtSPVoidMnemNumber(TCHAR *pszWork, USHORT fbMod, USHORT usReasonCode, USHORT usAdr, TCHAR *pszNumber)
{
    USHORT  usWrtLen = 0;

    /* -- get 'void' mnemonics -- */
    if (fbMod & (VOID_MODIFIER | RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) {
		if (0 > PrtGetReturns(fbMod, usReasonCode, pszWork, PARA_SPEMNEMO_LEN * 2 + 1)) {
			/* -- get void mnemonics instead of returns mnemonic -- */
			PrtGetVoid(pszWork, PARA_SPEMNEMO_LEN * 2 + 1);
		}
        usWrtLen = _tcslen(pszWork);
        tcharnset(&(pszWork[usWrtLen]), PRT_SPACE, 2);
        usWrtLen += 2;
    }

    /* -- get 'number' mnemonics -- */
    if (*pszNumber) {

        if (usAdr == 0) usAdr = TRN_NUM_ADR;

		RflGetTranMnem(&(pszWork[usWrtLen]), usAdr);
        usWrtLen = _tcslen(pszWork);
        pszWork[usWrtLen] = PRT_SPACE;
        usWrtLen++;
        _tcsncpy(&(pszWork[usWrtLen]), pszNumber, _tcslen(pszNumber));
    }

    if ( !(fbMod & VOID_MODIFIER) && (*pszNumber == '\0')) {
        return(0);
    }

    return(1);
}

/*
*===========================================================================
** Format  : USHORT  PrtSPMnemAmt(UCHAR *pszWork, UCHAR uchAdr, LONG lAmount);
*
*    Input : UCHAR  uchAdr          -Transacion mnemonics address
*            LONG   lAmount         -Amount
*
*   Output : UCHAR *pszWork         -destination buffer address
*    InOut : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets transaction mnemonics and amount to the buffer
*            'pszWork'.
*===========================================================================
*/
USHORT  PrtSPMnemAmt(TCHAR *pszWork, USHORT usAdr, DCURRENCY lAmount)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};

    /* -- get transaction mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- set transaction mnemonics and amount -- */
    RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPMnemAmt, aszTranMnem, lAmount);

    return(1);
}

/*
*===========================================================================
** Format  : USHORT  PrtSPDiscount(UCHAR *pszWork, UCHAR uchAdr,
*                                       UCHAR uchRate, LONG lAmount);
*
*    Input : UCHAR  uchAdr          -Transacion mnemonics address
*            UCHAR  uchRate         -Discount rate
*            LONG   lAmount         -Amount
*
*   Output : UCHAR  *pszWork        -destination buffer address
*    InOut : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets transaction mnemonics, discount rate,
*            and amount to the buffer 'pszWork'.
*===========================================================================
*/
USHORT  PrtSPDiscount(TCHAR *pszWork, USHORT usAdr, UCHAR uchRate, DCURRENCY lAmount)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};

    /* -- get transaction mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- set transaction mnemonics, rate, and amount -- */
    if (uchRate == 0) {                  /* amount override ? */
        RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPMnemAmt, aszTranMnem, lAmount);
    } else {
        RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPDiscount, aszTranMnem, uchRate, lAmount);
    }
    return(1);
}

/*
*===========================================================================
** Format  : USHORT  PrtSPCoupon( UCHAR *pszDest,
*                                 UCHAR *pszMnemonic,
*                                 LONG  lAmount )
*
*    Input : UCHAR  *pszDest        -   point to destination buffer
*            UCHAR  *pszMnemonic    -   point to coupon mnemonic
*            LONG   lAmount         -   amount of coupon
*
*   Output : UCHAR  *pszDest        -   point to created string
*    InOut : none
*
** Return  : number of line(s) to be set
*
** Synopsis:    This function sets coupon mnemonics and its amount, and
*               then it stores created string to the buffer 'pszDest'
*===========================================================================
*/
USHORT  PrtSPCoupon( TCHAR *pszDest, TCHAR *pszMnemonic, DCURRENCY lAmount )
{
    /* -- set coupon mnemonics, and its amount -- */
    RflSPrintf( pszDest, ( PRT_SPCOLUMN + 1 ), aszPrtSPMnemAmt, pszMnemonic, lAmount );

    return ( 1 );
}

/*
*===========================================================================
** Format  : USHORT  PrtSPTaxMod(UCHAR *pszWork, USHORT fsTax, UCHAR fbMod);
*
*   Input  : USHORT  fsTax,        -US or Canadian Tax
*            UCHAR   fbMod
*
*   Output : UCHAR   *pszWork      -destination buffer address
*   InOut  : none
*
** Return  : number of lines to be set
*
** Synopsis: This function sets tax modifier mnemonics to the buffer 'pszWork'.
*===========================================================================
*/
USHORT  PrtSPTaxMod(TCHAR *pszWork, USHORT fsTax, USHORT fbMod)
{
	TCHAR  aszTaxMod[(PARA_SPEMNEMO_LEN + 1) * 4] = { 0 };      /* Saratoga */

    /* -- get tax modifier mnemonics -- */
    if ( PRT_WITHDATA == PrtGetTaxMod(aszTaxMod, TCHARSIZEOF(aszTaxMod), fsTax, fbMod) ) {
        RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPTaxMod, aszTaxMod);
    }
    if (*pszWork != '\0') {
        return(1);
    }

    return(0);
}


/*
*===========================================================================
** Format  : USHORT  PrtSPMnemTaxQty(UCHAR *pszWork ,UCHAR uchAdr,
*                                    USHORT fsTax, ITEMSALES pItem);
*
*    Input : UCHAR  uchAdr          -Transacion mnemonics address
*            USHORT fsTax           -US or Canadian Tax
*            ITEMSALES  *pItem      -Item Data buffer address
*
*   Output : UCHAR  *pszWork        -destination buffer address
*    InOut : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets transaction mnemonics and amount
*            to the buffer 'pszWork'.
*===========================================================================
*/
USHORT  PrtSPMnemTaxQty(TCHAR *pszWork, USHORT usTranAdr, USHORT fsTax, ITEMSALES *pItem)
{
	TCHAR       aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};
    TCHAR       aszTaxMod[(PARA_SPEMNEMO_LEN + 1) * 4] = {0};      /* Saratoga */
    TCHAR       auchDummy[NUM_PLU_LEN] = {0};
    DCURRENCY   lPrice = 0;
    USHORT      i;
    USHORT      usNoOfChild;

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usTranAdr);

    /* -- get tax modifier mnemonics -- */
    PrtGetTaxMod(aszTaxMod, TCHARSIZEOF(aszTaxMod), fsTax, pItem->fbModifier);

    if (labs(pItem->lQTY) == PLU_BASE_UNIT) {
        /* -- set mod. disc. mnemonics and tax mod. 1-3 mnem. -- */
        RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPMnemTax, aszTranMnem, aszTaxMod);
        if ((*aszTranMnem) || (*aszTaxMod)) {
            return(1);
        }
    } else {
        /* -- get condiment's and noun's price -- */
        usNoOfChild = pItem->uchCondNo + pItem->uchPrintModNo + pItem->uchChildNo;
        for ( i = pItem->uchChildNo, lPrice = pItem->lUnitPrice; i < usNoOfChild; i++) {
            if (_tcsncmp(pItem->Condiment[ i ].auchPLUNo, auchDummy, NUM_PLU_LEN) != 0 ) {
                lPrice += pItem->Condiment[i].lUnitPrice;
            }
        }

        /* -- set mod. disc. mnemonics, tax mod. 1-3 mnem., qty and unit price -- */
        RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPMnemTaxQty, aszTranMnem, aszTaxMod, pItem->lQTY / PLU_BASE_UNIT, lPrice);
        return(1);
    }

    return(0);
}

/*
*===========================================================================
** Format  : USHORT  PrtSPMnemTaxAmt(UCHAR *pszWork ,UCHAR uchAdr, USHORT fsTax,
*                                  UCHAR fbMod, LONG lAmount);
*
*    Input : UCHAR   uchAdr          -Transacion mnemonics address
*            USHORT  fsTax           -US or Canadian Tax
*            UCHAR   fbMod           -Modifier status
*            LONG    lAmount         -Amount
*
*   Output : UCHAR   *pszWork        -destination buffer address
*    InOut : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets transaction mnemonics, tax modifier mnemonics,
*            and amount to the buffer 'pszWork'.
*===========================================================================
*/
USHORT  PrtSPMnemTaxAmt(TCHAR *pszWork, USHORT usAdr, USHORT fsTax, USHORT fbMod, DCURRENCY lAmount)
{
	TCHAR   aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};
    TCHAR   aszTaxMod[(PARA_SPEMNEMO_LEN + 1) * 4] = {0};      /* Saratoga */
    USHORT  usSetLine = 0;

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- get tax modifier mnemonics -- */
    PrtGetTaxMod(aszTaxMod, TCHARSIZEOF(aszTaxMod), fsTax, fbMod );

    /* -- set transaction mnemonics, rate, and amount -- */
    RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPMnemTaxAmt, aszTranMnem, aszTaxMod, lAmount);

    return(1);

}

/*
*===========================================================================
** Format  : USHORT  PrtSPMAmtShift(UCHAR *pszWork, UCHAR uchAdr, LONG lAmount,
*                                 USHORT usColumn);
*    Input : UCHAR  uchAdr          -Transacion mnemonics address
*            LONG   lAmont          -amount
*            USHORT usColumn        -shift column
*
*   Output : UCHAR *pszWork         -destination buffer address
*    InOut : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets transaction mnemonics and amount to the buffer
*            'pszWork'.
*===========================================================================
*/
USHORT  PrtSPMAmtShift(TCHAR *pszWork, USHORT usAdr, DCURRENCY lAmount, USHORT usColumn)
{
	TCHAR  aszWorkBuff[PRT_SPCOLUMN + 1] = {0};
    TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};
    USHORT usLen;

    /* -- get transaction mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- set transaction mnemonics and amount -- */
    RflSPrintf(aszWorkBuff, TCHARSIZEOF(aszWorkBuff), aszPrtSPMnemAmtD, aszTranMnem, lAmount);

    usLen = RflStrAdjust(pszWork, (PRT_SPCOLUMN + 1), aszWorkBuff, (PRT_SPCOLUMN - usColumn), RFL_FEED_OFF);

    memset( &(pszWork[usLen]), '\0', PRT_SPCOLUMN - usLen);

    return(1);
}

/*
*===========================================================================
** Format  : USHORT  PrtSPTaxRateAmt(UCHAR *pszWork, UCHAR uchAdr,  ULONG ulRate,
*                              LONG lAmount, USHORT usColumn);
*    Input : UCHAR  uchAdr          -Transacion mnemonics address
*            ULONG  ulRate          -tax rate
*            LONG   lAmont          -amount
*            USHORT usColumn        -shift column
*
*   Output : UCHAR *pszWork         -destination buffer address
*    InOut : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets transaction mnemonics, rate and amount to the buffer
*            'pszWork'.
*===========================================================================
*/
USHORT  PrtSPTaxRateAmt(TCHAR *pszWork, USHORT usAdr,  ULONG ulRate,
                                      DCURRENCY lAmount, USHORT usColumn)
{
	TCHAR  aszWorkBuff[PRT_SPCOLUMN + 1] = {0};
    TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};
    USHORT usLen;

    /* -- get transaction mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- set transaction mnemonics and amount -- */
    RflSPrintf(aszWorkBuff, TCHARSIZEOF(aszWorkBuff), aszPrtSPTaxRateAmt, aszTranMnem, ulRate / 100, lAmount);

    usLen = RflStrAdjust(pszWork, (PRT_SPCOLUMN + 1), aszWorkBuff, (PRT_SPCOLUMN - usColumn), RFL_FEED_OFF);

    memset( &(pszWork[usLen]), '\0', PRT_SPCOLUMN - usLen);

    return(1);
}

/*
*===========================================================================
** Format  : USHORT  PrtSPTaxMnem(UCHAR  *pszWork, ITEMAFFECTION  *pItem);
*
*   Input  : ITEMSALES *pITem     -item data address
*
*   Output : UCHAR  *pszWork      -destination buffer address
*   InOut  : none
*
** Return  : USHORT usSetLine     -number of line(s) to be set
*
** Synopsis: This function sets vat appl.mnem, vat mnem., total mnem..
*===========================================================================
*/
USHORT  PrtSPTaxMnem(TCHAR  *pszWork, ITEMAFFECTION  *pItem)
{
	TCHAR   aszVATMnem[3][PARA_TRANSMNEMO_LEN + 1] = { 0 };
    USCANVAT *pUSCanVAT = &(pItem->USCanVAT);

    if ( (   (pUSCanVAT->ItemVAT[0].lVATRate == 0) |
           ( (pUSCanVAT->ItemVAT[0].lVATable == 0) &&
             (pUSCanVAT->ItemVAT[0].lVATAmt == 0)  &&
             (pUSCanVAT->ItemVAT[0].lSum == 0)  )     ) &&

         (   (pUSCanVAT->ItemVAT[1].lVATRate == 0) |
           ( (pUSCanVAT->ItemVAT[1].lVATable == 0) &&
             (pUSCanVAT->ItemVAT[1].lVATAmt == 0)  &&
             (pUSCanVAT->ItemVAT[1].lSum == 0)  )     ) &&

         (   (pUSCanVAT->ItemVAT[2].lVATRate == 0) |
           ( (pUSCanVAT->ItemVAT[2].lVATable == 0) &&
             (pUSCanVAT->ItemVAT[2].lVATAmt == 0)  &&
             (pUSCanVAT->ItemVAT[2].lSum == 0)  )     )  ) {

        return (0);

    }

	RflGetTranMnem(aszVATMnem[0], TRN_TXBL1_ADR);
	RflGetTranMnem(aszVATMnem[1], TRN_TX1_ADR);
	RflGetTranMnem(aszVATMnem[2], TRN_AMTTL_ADR);

    RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPTaxMnem, aszVATMnem[0], aszVATMnem[1], aszVATMnem[2]);
    return(1);
}
/*
*===========================================================================
** Format  : USHORT  PrtSPTaxAmt(UCHAR  *pszWork, ITEMVAT  ItemVAT);
*
*   Input  : ITEMVAT  ItemVAT     -vat data
*
*   Output : UCHAR  *pszWork      -destination buffer address
*   InOut  : none
*
** Return  : USHORT usSetLine     -number of line(s) to be set
*
** Synopsis: This function sets the vat rate, vat net amount, vat amount,
*                                             vat applied total.
*===========================================================================
*/
USHORT  PrtSPTaxAmt(TCHAR  *pszWork, ITEMVAT  ItemVAT)
{
    if (ItemVAT.lVATRate == 0L) {
        return (0);
    }

    if (ItemVAT.lVATable == 0L && ItemVAT.lVATAmt == 0L && ItemVAT.lSum == 0L) {
        return (0);
    }

    /* reason of "100L" is */
    /*  lVATRate has 4digits under */
    /*  decimal point */
    RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPTaxAmt, ItemVAT.lVATRate/100L, ItemVAT.lVATable,  ItemVAT.lVATAmt, ItemVAT.lSum);

    return(1);
}

/*
*===========================================================================
** Format  : USHORT   PrtSPSeatNo(UCHAR *pszWork, ITEMSALES *pItem);
*
*   Input  : ITEMSALES *pItem           -seat no
*   Output : UCHAR *pszWork             -destination buffer address
*   InOut  : none
** Return  : USHORT usWriteLen          -write length
*
** Synopsis: This function formats seat no. line to . R3.1
*===========================================================================
*/
USHORT    PrtSPSeatNo(TCHAR *pszWork, TCHAR uchSeatNo)
{
	TCHAR   aszSpecMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };   /* PARA_... defined in "paraequ.h" */

    if (uchSeatNo == 0) {
        return(0);
    }

	RflGetSpecMnem( aszSpecMnem, SPC_SEAT_ADR );    /* seat no. mnemonic */

    RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPSeat, aszSpecMnem, uchSeatNo);

    return (1);

}


/*
*===========================================================================
** Format  : USHORT  PrtSPQty(UCHAR *pszWork, USHORT fsTax, ITEMSALES *pItem);
*
*   Input  : USHORT     fsTax          -US or Canadian Tax
*          : ITEMSALES  *pItem         -Item Data buffer address
*
*   Output : UCHAR      *pszWork       -destination buffer address
*   InOut  : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets tax modifier, quantity, and unit price
*            to the buffer 'pszWork'.
*===========================================================================
*/
USHORT  PrtSPQty(TCHAR *pszWork, TRANINFORMATION *pTran, ITEMSALES *pItem)
{
	TCHAR       aszTaxMod[(PARA_SPEMNEMO_LEN + 1) * 4] = {0};      /* Saratoga */
    TCHAR       auchDummy[NUM_PLU_LEN] = {0};
    USHORT      usSetLine = 0;
    DCURRENCY   lPrice = 0;
    USHORT      i;
    USHORT      usNoOfChild;

    /* -- get tax modifier mnemonics -- */
    PrtGetTaxMod(aszTaxMod, TCHARSIZEOF(aszTaxMod), pTran->TranModeQual.fsModeStatus, pItem->fbModifier);

    if (labs(pItem->lQTY) == PLU_BASE_UNIT) {
        /* -- set tax modifier mnemonics -- */
        RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPTaxMod, aszTaxMod);

        if (*pszWork != '\0') {
            return(1);
        }
    } else {
        /* -- get condiment's and noun's price -- */
        usNoOfChild = pItem->uchCondNo + pItem->uchPrintModNo + pItem->uchChildNo;
        lPrice = 0L;

        for ( i = pItem->uchChildNo; i < usNoOfChild; i++) {
            if (_tcsncmp(pItem->Condiment[ i ].auchPLUNo, auchDummy, NUM_PLU_LEN) != 0 ) {
                lPrice += pItem->Condiment[i].lUnitPrice;
            }
        }
        if (pItem->uchPPICode) {
            lPrice = lPrice * (pItem->lQTY / PLU_BASE_UNIT);
            lPrice += pItem->lProduct;      /* print whole at ppi sales */
        } else {
            lPrice += pItem->lUnitPrice;    /* print unit price at normal sales */
        }

        /* -- set tax modifier mnemonics, Qty, and amount -- */
        if ((pItem->uchMinorClass == CLASS_DEPTMODDISC)
                || (pItem->uchMinorClass == CLASS_PLUMODDISC)
                || (pItem->uchMinorClass == CLASS_SETMODDISC)
                || (pItem->uchMinorClass == CLASS_OEPMODDISC)) {

            RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPQty, aszTaxMod, pItem->lQTY / PLU_BASE_UNIT, lPrice);

        } else if (PrtChkVoidConsolidation(pTran, pItem) == FALSE) {
            /* not void consolidation */
            if (pItem->uchPPICode) {    /* print "QTY * Product / QTY" */
                RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPPPIQty, aszTaxMod, pItem->lQTY / PLU_BASE_UNIT, RflLLabs(lPrice), labs(pItem->lQTY / PLU_BASE_UNIT));
            } else
            if (pItem->uchPM) {    /* print "QTY * Product / PM", 2172 */
                RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPPPIQty, aszTaxMod, pItem->lQTY / PLU_BASE_UNIT, RflLLabs(lPrice), pItem->uchPM);
            } else {
                RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPQty, aszTaxMod, pItem->lQTY / PLU_BASE_UNIT, lPrice);
            }
        } else {
            /* void consolidation */
            if (pItem->uchPM) {    /* print "QTY * Product / PM", 2172 */
                RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPPPIQty, aszTaxMod, pItem->lQTY / PLU_BASE_UNIT, RflLLabs(lPrice), pItem->uchPM);
            } else
            if (abs(pItem->sCouponQTY) != 1) {
                RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPQty, aszTaxMod, (LONG)pItem->sCouponQTY, lPrice);
            } else {
                return(0);
            }
        }
        return(1);
    }

    return(0);
}

/*
*===========================================================================
** Format  : USHORT  PrtSPQty(UCHAR *pszWork, USHORT fsTax, ITEMSALES *pItem);
*
*   Input  : USHORT     fsTax          -US or Canadian Tax
*          : ITEMSALES  *pItem         -Item Data buffer address
*
*   Output : UCHAR      *pszWork       -destination buffer address
*   InOut  : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets tax modifier, quantity, and unit price
*            to the buffer 'pszWork'.
*===========================================================================
*/
USHORT  PrtSPLinkQty(TCHAR *pszWork, TRANINFORMATION *pTran, ITEMSALES *pItem)
{
	TCHAR       aszTaxMod[(PARA_SPEMNEMO_LEN + 1) * 4] = {0};      /* Saratoga */
    DCURRENCY   lPrice;

    /* -- get tax modifier mnemonics -- */
    PrtGetTaxMod(aszTaxMod, TCHARSIZEOF(aszTaxMod), pTran->TranModeQual.fsModeStatus, pItem->fbModifier);

    if (labs(pItem->lQTY) == PLU_BASE_UNIT) {
        /* -- set tax modifier mnemonics -- */
        RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPTaxMod, aszTaxMod);
        if (*pszWork != '\0') {
            return(1);
        }
    } else {
        lPrice = pItem->Condiment[0].lUnitPrice;

        /* -- set tax modifier mnemonics, Qty, and amount -- */
        if ((pItem->uchMinorClass == CLASS_DEPTMODDISC)
                || (pItem->uchMinorClass == CLASS_PLUMODDISC)
                || (pItem->uchMinorClass == CLASS_SETMODDISC)
                || (pItem->uchMinorClass == CLASS_OEPMODDISC)) {

            RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPQty, aszTaxMod, pItem->lQTY / PLU_BASE_UNIT, lPrice);
        } else if (PrtChkVoidConsolidation(pTran, pItem) == FALSE) {
            RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPQty, aszTaxMod, pItem->lQTY / PLU_BASE_UNIT, lPrice);
        } else {
            if (abs(pItem->sCouponQTY) != 1) {
                RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPQty, aszTaxMod, (LONG)pItem->sCouponQTY, lPrice);
            } else {
                return(0);
            }
        }
        return(1);
    }

    return(0);
}

/*
*===========================================================================
** Format  : USHORT  PrtSPWeight(UCHAR *pszWork, USHORT fsTax, ULONG flScale, ITEMSALES *pItem);
*
*    Input : USHORT     fsTax           -US or Canadian Tax
*            ULONG      flScale         -auto scale print type ".00" or ".000"
*            ITEMSALES  *pItem          -Item Data address
*
*   Output : UCHAR      *pszWork        -destination buffer address
*   InOut  : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets tax modifier, weight, and unit price
*            to the buffer 'pszWork'.
*===========================================================================
*/
USHORT  PrtSPWeight(TCHAR *pszWork, USHORT fsTax, ULONG flScale, ITEMSALES *pItem)
{
	TCHAR  aszWeightSym[PARA_SPEMNEMO_LEN + 1] = { 0 };  /* PARA_... defined in "paraequ.h" */
	TCHAR  aszTaxMod[(PARA_SPEMNEMO_LEN + 1) * 4] = { 0 };   /* Saratoga */
	TCHAR  aszMnlMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };
	TCHAR  aszUPrice[PRT_SPCOLUMN + 1] = { 0 };  /* set scalable item unit price */

    /* -- get tax modifier mnemonics -- */
    PrtGetTaxMod(aszTaxMod, TCHARSIZEOF(aszTaxMod), fsTax, pItem->fbModifier);

    /* -- get scalable symbol -- */
	RflGetSpecMnem(aszWeightSym, SPC_LB_ADR);

    /* -- weight manual entry ? -- */
    if (pItem->fsPrintStatus & PRT_MANUAL_WEIGHT) {
        /* -- get transaction  mnemonics -- */
		RflGetTranMnem(aszMnlMnem, TRN_MSCALE_ADR);
        /* -- print ".000" type ? -- */
        if ( flScale & CURQUAL_SCALE3DIGIT ) {
            RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPMnlWeight1, aszTaxMod, pItem->lQTY, aszWeightSym, aszMnlMnem);
        } else {
			/* -- print ".00" type  -- */
            RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPMnlWeight2, aszTaxMod, pItem->lQTY / 10L, aszWeightSym, aszMnlMnem);
        }
    } else {
		/* -- auto scale -- */
        memset(aszUPrice, '\0', PRT_SPCOLUMN + 1);
        /* -- set unit price -- */
        RflSPrintf(aszUPrice, PRT_SPCOLUMN + 1, aszPrtSPScale, pItem->lUnitPrice, aszWeightSym);

        if ( flScale & CURQUAL_SCALE3DIGIT ) {
			/* -- print ".000" type ? -- */
            RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPWeight1, aszTaxMod, pItem->lQTY, aszWeightSym, aszUPrice);
        } else {
			/* -- print ".00" type  -- */
            RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPWeight2, aszTaxMod, pItem->lQTY / 10L, aszWeightSym, aszUPrice);
        }
    }

    return(1);

}

/*
*===========================================================================
** Format  : USHORT  PrtSPMnlWeight(UCHAR *pszWork, LONG lAmount);
*
*    Input : LONG       lAmount         -scalable item unit price
*
*   Output : UCHAR      *pszWork        -destination buffer address
*   InOut  : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets tax modifier, weight, and unit price
*            to the buffer 'pszWork'.
*===========================================================================
*/
USHORT  PrtSPMnlWeight(TCHAR *pszWork, DCURRENCY lAmount)
{
	TCHAR  aszWeightSym[PARA_SPEMNEMO_LEN + 1] = {0};
    TCHAR  aszUPrice[PRT_SPCOLUMN + 1] = {0};

    /* -- get scalable symbol -- */
	RflGetSpecMnem(aszWeightSym, SPC_LB_ADR);

    /* -- set unit price -- */
    RflSPrintf(aszUPrice, PRT_SPCOLUMN + 1, aszPrtSPScale, lAmount, aszWeightSym);

    /* -- set tax modifier mnemonics, Qty, and amount -- */
    RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPMnlWeight3, aszUPrice);

    return(1);

}

/*
*===========================================================================
** Format  : USHORT  PrtSPTranMnem(UCHAR *pszWork ,UCHAR uchAdr);
*
*    Input : UCHAR  uchAdr          -Transacion mnemonics address
*
*   Output : UCHAR  *pszWork        -destination buffer address
*    InOut : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets transaction mnemonics to the buffer 'pszWork'.
*===========================================================================
*/
USHORT  PrtSPTranMnem(TCHAR *pszWork, USHORT usAdr)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 }; /* PARA_... defined in "paraequ.h" */

    /* -- get transaction mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- set transaction mnemonics and amount -- */
    RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPTranMnem, aszTranMnem);

    return(1);
}

/*
*===========================================================================
** Format  : USHORT  PrtSPTblPerson( UCHAR  *pszWork,
*                                    USHORT usTblNo,
*                                    USHORT usNoPerson,
*                                    SHORT  sWidthType )
*
*    Input : USHORT  usTblNo    -   Table number
*            USHORT  usNoPerson -   No. of Person
*            SHORT   sWidthType -   Type of Print Character Width
*                                       PRT_DOUBLE, PRT_SINGLE
*
*   Output : UCHAR   *pszWork   -   destination buffer address
*    InOut : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets table No., and No. of person to the buffer
*            'pszWork'.
*===========================================================================
*/
USHORT PrtSPTblPerson( TCHAR *pszWork, USHORT usTblNo, USHORT usNoPerson, SHORT sWidthType )
{
	TCHAR   aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 }; /* PARA_... defined in "paraequ.h" */
	TCHAR   aszSpecMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };   /* PARA_... defined in "paraequ.h" */
	TCHAR   aszLeadThru[PARA_LEADTHRU_LEN + 1] = { 0 };
	TCHAR   aszTblNo[PRT_ID_LEN + 1] = { 0 };
	TCHAR   aszNoPer[PRT_ID_LEN + 1] = { 0 };
	TCHAR   aszTransNo[PRT_ID_LEN * 2 + 1] = { 0 };

    if ((usTblNo == 0) && (usNoPerson == 0)) {

        return(0);
    }

    /* --- set up special mnemonic of table no., and table no. --- */
    if ( usTblNo != 0 ) {
        _itot( usTblNo, aszTblNo, 10 );
        if ( sWidthType == PRT_DOUBLE ) {
            /* --- set up unique transaction number string --- */
            PrtDouble( aszTransNo, (PRT_ID_LEN * 2 + 1 ), aszTblNo );
            PrtGetLead( aszLeadThru, LDT_UNIQUE_NO_ADR );
        } else {
			RflGetSpecMnem( aszSpecMnem, SPC_TBL_ADR );
        }
    }

    /* --- set up special mnemonic of no. of person, and no. of person --- */
    if ( (usNoPerson != 0) && (!CliParaMDCCheck(MDC_TRANNUM_ADR, EVEN_MDC_BIT3))) {
        _itot( usNoPerson, aszNoPer, 10 );
		RflGetTranMnem( aszTranMnem, TRN_PSN_ADR );
    }

    if ( sWidthType == PRT_DOUBLE ) {
        if ( usTblNo || usNoPerson ) {
            /* --- print unique transaction no. with double width --- */
            RflSPrintf( pszWork, ( PRT_SPCOLUMN + 1 ), aszPrtSPTransPerson, aszLeadThru, aszTransNo, aszTranMnem, aszNoPer );
        }
    } else {
        if ( usTblNo || usNoPerson ) {
            /* --- print table no. with single width --- */
            RflSPrintf( pszWork, ( PRT_SPCOLUMN + 1 ), aszPrtSPTblPerson, aszSpecMnem, aszTblNo, aszTranMnem, aszNoPer );
        }
    }
    return ( 1 );
}

/*
*===========================================================================
** Format  : USHORT  PrtSPCustomerName( UCHAR *pszDest,
*                                       UCHAR *pszCustomerName )
*
*    Input : UCHAR  *pszCustomerName - address of customer name buffer
*
*   Output : UCHAR  *pszDest    -   address of destination buffer
*
** Return  : number of line(s) to be set
*
** Synopsis:
*       This function sets customer name (max. 16 char without NULL) to
*   destination slip printer buffer.
*===========================================================================
*/
USHORT PrtSPCustomerName( TCHAR *pszDest, TCHAR *pszCustomerName )
{
    if ( *pszCustomerName == '\0' ) {   /* customer name is not specified */
        return ( 0 );
    }

    if ( *( pszCustomerName + NUM_NAME - 2 ) == PRT_DOUBLE ) {
        *( pszCustomerName + NUM_NAME - 2 ) = '\0';
    }

    RflSPrintf( pszDest, ( PRT_SPCOLUMN + 1 ), aszPrtSPCustomerName, pszCustomerName );

    return ( 1 );
}

/*
*===========================================================================
** Format  : VOID   PrtSPMultiSeatNo(TRANINFORMATION *pTran);
*
*   Input  : ITEMSALES *pItem           -seat no
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function formats seat no. line to . R3.1
*===========================================================================
*/
USHORT   PrtSPMultiSeatNo( TCHAR *pszDest, TRANINFORMATION *pTran)
{
	TCHAR   aszPrintBuff[PRT_RJCOLUMN + 1] = {0};
    TCHAR   aszSpecMnem[ PARA_SPEMNEMO_LEN + 1 ] = {0};
    USHORT  i, usPos;

    if (pTran->TranGCFQual.auchFinSeat[0] == 0) {
        return(0);
    }

	RflGetSpecMnem( aszSpecMnem, SPC_SEAT_ADR );    /* seat no. mnemonic */
    usPos = _tcslen(aszSpecMnem);

    _tcsncpy(&aszPrintBuff[0], aszSpecMnem, usPos);

    /* ---- set multiple seat no. ---- */
    for (i = 0; i < NUM_SEAT; i++) {
        if (pTran->TranGCFQual.auchFinSeat[i]) {
            aszPrintBuff[usPos++] = ' ';
            aszPrintBuff[usPos++] = (TCHAR)(pTran->TranGCFQual.auchFinSeat[i] | 0x30);
        } else {
            break;
        }
    }

    aszPrintBuff[usPos++] = '\0';

    RflSPrintf( pszDest, ( PRT_SPCOLUMN + 1 ), aszPrtSPMultiSeat, aszPrintBuff);

    return ( 1 );
}


/*
*===========================================================================
** Format  : USHORT  PrtSPChild(UCHAR uchAdj, UCHAR *pszMnem);
*
*   Input  : UCHAR  uchAdj          -Adjective number
*            UCHAR  *pszMnem        -Adjective mnemonics address
*
*   Output : UCHAR  *pszWork        -destination buffer address
*   InOut  : none
*
** Return  : USHORT  usSetLine      -number of lines to be set
*
** Synopsis: This function sets child plu to the buffer 'pszWork'.
*
*===========================================================================
*/
USHORT  PrtSPChild(TCHAR *pszWork, UCHAR uchAdj, TCHAR *pszMnem)
{
	TCHAR    aszAdjMnem[PARA_ADJMNEMO_LEN + 1] = { 0 };

    if (uchAdj != 0) {
        /* --get adjective mnemocnics -- */
		RflGetAdj(aszAdjMnem, uchAdj);
    }

    /* -- set adjective mnemonics and child plu mnemonics -- */
    RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPChildPLU, aszAdjMnem, pszMnem);

    return(1);
}

/*
*===========================================================================
** Format  : USHORT  PrtSPChild(UCHAR uchAdj, UCHAR *pszMnem);
*
*   Input  : UCHAR  uchAdj          -Adjective number
*            UCHAR  *pszMnem        -Adjective mnemonics address
*
*   Output : UCHAR  *pszWork        -destination buffer address
*   InOut  : none
*
** Return  : USHORT  usSetLine      -number of lines to be set
*
** Synopsis: This function sets child plu to the buffer 'pszWork'.
*
*===========================================================================
*/
USHORT  PrtSPLinkPLU(TCHAR *pszWork, USHORT fsModified, UCHAR uchAdj, TCHAR *pszMnem, DCURRENCY lPrice)
{
    if (uchAdj != 0) {
		TCHAR    aszAdjMnem[PARA_ADJMNEMO_LEN + 1] = {0};

        /* --get adjective mnemocnics -- */
		RflGetAdj(aszAdjMnem, uchAdj);

        /* -- set adjective mnemonics and child plu mnemonics -- */
        if (fsModified) {
            RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPModLinkPLU1, aszAdjMnem, pszMnem, lPrice);
        } else {
            RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPLinkPLU1, aszAdjMnem, pszMnem, lPrice);
        }
    } else {
        /* -- set adjective mnemonics and child plu mnemonics -- */
        if (fsModified) {
            RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPModLinkPLU2, pszMnem, lPrice);
        } else {
            RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPLinkPLU2, pszMnem, lPrice);
        }
    }
    return(1);
}


/*
*===========================================================================
** Format  : USHORT  PrtSPTipsPO(USHORT usWaiID, UCHAR uchAdr, LONG lAmount);
*
     Input : USHORT  usWaiID         -waiter ID
*            UCHAR   uchAdr          -Transacion mnemonics address
*            LONG    lAmount         -Amount
*
*   Output : UCHAR   *pszWork        -destination buffer address
*   InOut  : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets waiter ID, tips paid out mnemonics and amount
*            to the buffer 'pszWork'.
*===========================================================================
*/
USHORT  PrtSPTipsPO(TCHAR *pszWork, ULONG ulWaiID, USHORT usAdr, DCURRENCY lAmount)
{
    TCHAR  aszWaiMnem[PARA_SPEMNEMO_LEN + 1] = {0};
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};

    /* -- get waiter mnemonics -- */
	RflGetSpecMnem(aszWaiMnem, SPC_CAS_ADR);

    /* -- get transaction mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- set transaction mnemonics and amount -- */
    RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPTipsPO, aszWaiMnem, ulWaiID, aszTranMnem, lAmount);

    return(1);
}

/*
*===========================================================================
** Format  : USHORT  PrtSPGCTransfer(UCHAR *pszWork, ITEMMISC *pItem)
*
*    Input : USHORT  usWaiID            -waiter ID
*            TRANINFORMATION  *pTran    -transaction information buffer address
*
*   Output : UCHAR   *pszWork           -destination buffer address
*   InOut  : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets waiter ID and guest check no.
*            to the buffer 'pszWork'.                               V3.3
*===========================================================================
*/
USHORT  PrtSPGCTransfer(TCHAR *pszWork, ITEMMISC *pItem)
{
	TCHAR  aszGCNo[PRT_GC_LEN + 1] = { 0 };
	USHORT usAdr;
	TCHAR  aszWaiMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };   /* PARA_... defined in "paraequ.h" */
	TCHAR  aszGCMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };    /* PARA_... defined in "paraequ.h" */
    USHORT   usSetLine = 0;

    /* -- get old waiter No. mnemonics, V3.3 -- */
    if (pItem->uchMinorClass == CLASS_CHECKTRANS_FROM) {
        usAdr = TRN_CKFRM_ADR;
    } else if (pItem->uchMinorClass == CLASS_CHECKTRANS_TO) {
        usAdr = TRN_CKTO_ADR;
    } else {                                     /* old waiter */
        usAdr = TRN_OLDWT_ADR;
    }
	RflGetTranMnem(aszWaiMnem, usAdr);

    if (pItem->usGuestNo != 0) {
        /* convert int to ascii */
        _itot(pItem->usGuestNo, aszGCNo, 10);
        /* -- get guest check No. mnemonics -- */
		RflGetTranMnem(aszGCMnem, TRN_GCNO_ADR);
    }

    if (pItem->uchCdv == 0) {
        RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPGCTran, aszWaiMnem, pItem->ulID, aszGCMnem, aszGCNo);
    } else {
        RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPGCTranCdv, aszWaiMnem, pItem->ulID, aszGCMnem, aszGCNo, pItem->uchCdv);
    }

    return(1);
}

/*
*===========================================================================
** Format  : USHORT  PrtSPGstChkNo(UCHAR *pszWork, ITEMMULTI *pItem);
*
*   Input  : ITEMMULTI  *pItem          -item data buffer address
*
*   Output : UCHAR  *pszWork            -destination buffer address
*   InOut  : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets guest check no. mnemonic and amount
*            to the buffer 'pszWork'.
*===========================================================================
*/
USHORT  PrtSPGstChkNo(TCHAR *pszWork, ITEMMULTI *pItem)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };    /* PARA_... defined in "paraequ.h" */
	TCHAR  aszGCNo[PRT_GC_LEN + 1] = { 0 };

    if (pItem->usGuestNo != 0) {
        /* convert int to ascii */
        _itot(pItem->usGuestNo, aszGCNo, 10);
        /* -- get check paid mnemonics -- */
		RflGetTranMnem(aszTranMnem, TRN_CKPD_ADR);

        /* -- set guest check No. mnemonic and amount -- */
        if (pItem->uchCdv == 0) {               /* without CDV */
            RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPGCNo, aszTranMnem, aszGCNo);
        } else {                                /* with CDV */
            RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPGCNoCdv, aszTranMnem, aszGCNo, pItem->uchCdv);
        }
        return(1);
    }

    return(0);
}

/*
*===========================================================================
** Format  : USHORT  PrtSPForeign(UCHAR  *pszWork, LONG lForeign, LONG lRate,
*                               UCHAR uchAdr, UCHAR fbStatus);
*
*   Input  : LONG    lForeign      -Foreign tender amount
*            UCHAR   uchAdr        -Foreign symbol address
*            UCHAR   fbStatus      -Foreign currency decimal point status
*            ULONG   ulRate        -rate
*
*   Output : UCHAR   *pszWork      -destination buffer address
*   InOut  : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets foeign tender amount and conversion rate
*            to the buffer 'pszWork'.
*
*===========================================================================
*/
USHORT  PrtSpForeign(TCHAR *pszWork, DCURRENCY lForeign, UCHAR uchAdr, UCHAR fbStatus, ULONG ulRate, UCHAR fbStatus2)
{
	TCHAR  aszFMnem[PARA_SPEMNEMO_LEN + 1] = {0};
    TCHAR  aszFAmt[PRT_SPCOLUMN + 1] = {0};
    USHORT usStrLen;
    USHORT sDecPoint, sDecPoint2;

    /* -- check decimal point -- */
    if (fbStatus & ODD_MDC_BIT1) {
        if (fbStatus & ODD_MDC_BIT2) {
            sDecPoint = PRT_NODECIMAL;
        } else {
            sDecPoint = PRT_3DECIMAL;
        }
    } else {
        sDecPoint = PRT_2DECIMAL;
    }

    /* -- get foreign symbol -- */
	RflGetSpecMnem(aszFMnem, uchAdr);

    /* -- get string length -- */
    usStrLen = _tcslen(aszFMnem);

    /* 6 digit decimal point for Euro, V3.4 */
    if (fbStatus2 & ODD_MDC_BIT0) {

        sDecPoint2 = PRT_6DECIMAL;

    } else {

        sDecPoint2 = PRT_5DECIMAL;
    }
    /* -- format foreign amount -- */
    RflSPrintf(&aszFAmt[usStrLen], TCHARSIZEOF(aszFAmt)-usStrLen, aszPrtSPForeign1, sDecPoint, lForeign, sDecPoint2, ulRate);

    /* -- adjust sign ( + , - ) -- */

    if (lForeign < 0) {
        aszFAmt[0] = _T('-');
        _tcsncpy(&aszFAmt[1], aszFMnem, usStrLen);
    } else {
        _tcsncpy(aszFAmt, aszFMnem, usStrLen);
    }
    /* -- set foreign amount and conversion rate */
    RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPForeign2, aszFAmt);

    return(1);
}

/*
*===========================================================================
** Format  : VOID  PrtSPEuro(uchAdr1, LONG lForeign, UCHAR uchAdr2, ULONG ulRate)
*
*   Input  : UCHAR uchAdr1        -local symbol address
*            LONG  lForeign,      -local tender amount
*            UCHAR uchAdr2        -Euro symbol address
*            ULONG ulRate         -Euro conversion rate
*
*   Output : none
*   InOut  : none
*
** Return  : none
*
** Synopsis: This function prints Euro tender line, V3.4
*
*===========================================================================
*/
USHORT  PrtSPEuro(TCHAR *pszWork, UCHAR uchAdr1, DCURRENCY lForeign, UCHAR uchAdr2, ULONG ulRate, UCHAR fbStatus2)
{
	TCHAR  aszFMnem1[PARA_SPEMNEMO_LEN + 1] = {0};
    TCHAR  aszFMnem2[PARA_SPEMNEMO_LEN + 1] = {0};
    SHORT  sDecPoint;

    /* -- get foreign symbol -- */
	RflGetSpecMnem(aszFMnem1, uchAdr1);
	RflGetSpecMnem(aszFMnem2, uchAdr2);

    if (fbStatus2 & ODD_MDC_BIT0) {
        sDecPoint = PRT_6DECIMAL;
    } else {
        sDecPoint = PRT_5DECIMAL;
    }

    /* -- set foreign amount and conversion rate */
    RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtRJEuro1, aszFMnem1, lForeign, aszFMnem2, sDecPoint, ulRate);

    return(1);
}

/*
*===========================================================================
** Format  : VOID  PrtSPAmtSymEuro(UCHAR uchAdr1, UCHAR uchAdr2, LONG lAmount, UCHAR fbStatus)
*
*   Input  : UCHAR  uchAdr1,            -Transaction mnemonics address
*            UCHAR  uchAdr2             -Euro symbol address
*            LONG   lAmount             -amount
*            UCHAR fbStatus             -Foreign currency decimal point status
*   Output : none
*   InOut  : none
*
** Return  : none
*
** Synopsis: This function prints transaction mnemonic, native mnemonic  line. V3.4
*===========================================================================
*/
USHORT  PrtSPAmtSymEuro(TCHAR *pszWork, USHORT usAdr1, UCHAR uchAdr2, DCURRENCY lAmount, UCHAR fbStatus)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};
    TCHAR  aszFMnem[PARA_SPEMNEMO_LEN +1 + 1] = {0};
    SHORT  sDecPoint;

    /* -- check decimal point -- */
    if (fbStatus & ODD_MDC_BIT1) {
        if (fbStatus & ODD_MDC_BIT2) {
            sDecPoint = PRT_NODECIMAL;
        } else {
            sDecPoint = PRT_3DECIMAL;
        }
    } else {
        sDecPoint = PRT_2DECIMAL;
    }

    /* -- get transaction mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr1);

    /* -- adjust native mnemonic and amount sign(+.-) -- */
    if (lAmount < 0) {
        aszFMnem[0] = '-';
        /* -- get foreign symbol -- */
		RflGetSpecMnem(&aszFMnem[1], uchAdr2);
    } else {
        /* -- get foreign symbol -- */
		RflGetSpecMnem(aszFMnem, uchAdr2);
    }

    RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtRJEuroAmtMnem, aszTranMnem, aszFMnem, sDecPoint, RflLLabs(lAmount));

    return(1);
}

/*
*===========================================================================
** Format  : USHORT  PrtSPMnemNativeAmt(UCHAR *pszWork, UCHAR uchAdr,
*                                     LONG lAmount, BOOL fsType);
*
*    Input : UCHAR  uchAdr        -Transaction mnemonics address
*            LONG   lAmount       -amount
*            BOOL   fsType        -character type
*                                     PRT_SINGLE: single character
*                                     PRT_DOUBLE: double character
*
*   Output : UCHAR  *pszWork      -destination buffer address
*   InOut  : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets transaction mnemonic, native mnemonic
*            and amount.
*===========================================================================
*/
USHORT  PrtSPMnemNativeAmt(TCHAR *pszWork, USHORT usAdr, DCURRENCY lAmount, BOOL fsType)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};
    TCHAR  aszSpecAmtS[PARA_SPEMNEMO_LEN + PRT_AMOUNT_LEN + 1] = {0};
    TCHAR  aszSpecAmtD[(PARA_SPEMNEMO_LEN + PRT_AMOUNT_LEN) * 2  +1] = {0};

    /* -- get transaction mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- adjust native mnemonic and amount sign(+.-) -- */
    PrtAdjustNative(aszSpecAmtS, lAmount);

    /* -- amount printed double wide ? -- */
    if (fsType == PRT_DOUBLE ) {
        /* -- convert single char. to double -- */
        PrtDouble(aszSpecAmtD, ((PARA_SPEMNEMO_LEN + PRT_AMOUNT_LEN) * 2  +1), aszSpecAmtS);
        RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPMnemNatAmtD, aszTranMnem, aszSpecAmtD);
    } else {
        RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPMnemNatAmtS, aszTranMnem, aszSpecAmtS);
    }

    return(1);
}
/*
*===========================================================================
** Format  : USHORT  PrtSPServTaxSum(UCHAR *pszWork, UCHAR uchAdr,
*                                     LONG lAmount, BOOL fsType);
*
*    Input : UCHAR   uchAdr        -Transaction mnemonics address
*            LONG    lAmount       -amount
*            BOOL    fsType        -character type
*                                     PRT_SINGLE: single character
*                                     PRT_DOUBLE: double character
*   Output : UCHAR   *pszWork      -destination buffer address
*   InOut  : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets transaction mnemonic, native mnemonic
*            and amount.
*===========================================================================
*/
USHORT  PrtSPServTaxSum(TCHAR *pszWork, USHORT usAdr, DCURRENCY lAmount, BOOL fsType)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};
    TCHAR  aszSpecAmtS[PARA_SPEMNEMO_LEN +PRT_AMOUNT_LEN + 1] = {0};
    TCHAR  aszSpecAmtD[(PARA_SPEMNEMO_LEN + PRT_AMOUNT_LEN) * 2 + 1] = {0};
    TCHAR  aszStrLen[PRT_SPCOLUMN * 2 + 1] = {0};
    USHORT usStrLen;
    USHORT usDblLen;

    /* -- get transaction mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- adjust native mnemonic and amount sign(+.-) -- */
    PrtAdjustNative(aszSpecAmtS, lAmount);

    RflSPrintf(aszStrLen, TCHARSIZEOF(aszStrLen), aszPrtSPSevTaxSumS, aszTranMnem);
    usStrLen = _tcslen(aszStrLen);
    usDblLen = (_tcslen(aszSpecAmtS) * 2);

    /* -- double wide print possible ? -- */
    if ((fsType == PRT_DOUBLE) && (PRT_SPCOLUMN >= usStrLen + usDblLen + 1)) {
        /* -- convert single char. to double -- */
        PrtDouble(aszSpecAmtD, ((PARA_SPEMNEMO_LEN + PRT_AMOUNT_LEN) * 2  +1), aszSpecAmtS);
        RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPSevTaxSum, aszTranMnem, aszSpecAmtD);

    } else {
        RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPSevTaxSum, aszTranMnem, aszSpecAmtS);
    }

    return(1);
}

/*
*===========================================================================
** Format  : USHORT  PrtSPChargeTips(UCHAR *pszWork, UCHAR uchAdr, USHORT usWaiID,
*                         USHORT fsTax, ITEMDISC *pItem);
*
*    Input : USHORT  usWaiID         -waiter ID
*            UCHAR   uchAdr          -Transacion mnemonics address
*            USHORT  fsTax           -US or Canadian Tax
*            ITEMDISC   *pItem       -item data buffer address
*
*   Output : UCHAR  *pszWork         -destination buffer address
*   InOut  : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets waiter mnem. and ID, charge tips mnem.,
*            rate, and amount to the buffer 'pszWork'.
*===========================================================================
*/
USHORT  PrtSPChargeTips(TCHAR *pszWork, USHORT usAdr, ULONG ulWaiID, USHORT fsTax, ITEMDISC *pItem)
{
	CONST TCHAR   *aszPrtSPChgTps = _T("%-8.8s   %-4s %8.8Mu   %4s       %11l$");       /*8 characters JHHJ, charge tips mnem., waiter mnem and ID, and amount */
	CONST TCHAR   *aszPrtSPChgTpsRate = _T("%-8.8s   %-4s %8.8Mu   %4s  %3d%% %11l$");  /*8 characters JHHJ, charge tips mnem., waiter mnem and ID, rate, and amount */
	TCHAR  aszTaxMod[PARA_SPEMNEMO_LEN + 1] = { 0 };
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };    /* PARA_... defined in "paraequ.h" */

    /* -- get tax modifier mnemonics -- */
    PrtGetTaxMod(aszTaxMod, TCHARSIZEOF(aszTaxMod), fsTax, pItem->fbDiscModifier);

    /* -- get charge tips mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    if (ulWaiID) {
		TCHAR  aszWaiMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };       /* PARA_... defined in "paraequ.h" */

        /* -- get waiter mnemonics -- */
		RflGetSpecMnem(aszWaiMnem, SPC_CAS_ADR);
        if (pItem->uchRate) {
            RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPChgTpsRate, aszTranMnem, aszWaiMnem, RflTruncateEmployeeNumber(ulWaiID), aszTaxMod, pItem->uchRate, pItem->lAmount);
        } else {
            RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPChgTps, aszTranMnem, aszWaiMnem, RflTruncateEmployeeNumber(ulWaiID), aszTaxMod, pItem->lAmount);
        }
    } else {
        if (pItem->uchRate) {
            RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPTaxDisc, aszTranMnem, aszTaxMod, pItem->uchRate, pItem->lAmount);
        } else {
            RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPMnemTaxAmt, aszTranMnem, aszTaxMod, pItem->lAmount);
        }
    }

    return(1);
}

/*
*===========================================================================
** Format  : USHORT  PrtSPCancel(UCHAR *pszWork, UCHAR uchAdr1,
*                                        UCHAR uchAdr2, LONG lAmount);
*
*    Input : USHORT  usWaiID         -waiter ID
*            UCHAR   uchAdr1         -'cancel' mnemonics address
*            UCHAR   uchAdr2         -'cancel total' mnemonics address
*            LONG    lAmount         -Amount
*
*   Output : UCHAR  *pszWork         -destination buffer address
*   InOut  : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets cancel mnem. (double wide), cancel total mnem.,
*            and cancel amount to the buffer 'pszWork'.
*===========================================================================
*/
USHORT  PrtSPCancel(TCHAR *pszWork, USHORT usAdr1, USHORT usAdr2, DCURRENCY lAmount)
{
	TCHAR  aszVoidMnem[PARA_TRANSMNEMO_LEN * 2 + 1] = {0};
    TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};

    /* -- get transaction cancel mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr1);

    /* -- convert character to double chara -- */
    PrtDouble(aszVoidMnem, PARA_TRANSMNEMO_LEN * 2 + 1, aszTranMnem);

    /* -- get transaction cancel total mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr2);

    RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPCancel, aszVoidMnem, aszTranMnem, lAmount);

    return(1);
}

/*
*===========================================================================
** Format  : USHORT  PrtSPTrail1(UCHAR  *pszWork, TRANINFORMATION  *pTran, USHORT usSlipLine);
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*
*   Output : UCHAR  *pszWork           -destination buffer address
*   InOut  : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets trailer data.
*===========================================================================
*/
USHORT PrtSPTrail1(TCHAR *pszWork, TRANINFORMATION  *pTran, USHORT usSlipLine)
{
	CONST TCHAR   *aszPrtSPTrail1  = _T("%02d %04d %-4s %8.8Mu  %04lu-%03lu %16s");      /* trailer ( with line No. ) */
    ULONG   ulID = 0;
	TCHAR   aszDate[PRT_DATETIME_LEN + 1] = { 0 };
	TCHAR   aszMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };  /* PARA_... defined in "paraequ.h" */

    if ( PRT_CASHIER == PrtChkCasWai(aszMnem, pTran->TranModeQual.ulCashierID, pTran->TranModeQual.ulWaiterID) ) {
        ulID = RflTruncateEmployeeNumber(pTran->TranModeQual.ulCashierID);
    } else {
        ulID = RflTruncateEmployeeNumber(pTran->TranModeQual.ulWaiterID);
    }

    /* -- get date and time -- */
    PrtGetDate(aszDate, TCHARSIZEOF(aszDate), pTran);

    RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPTrail1, usPrtSlipPageLine % PRT_SLIP_MAX_LINES + usSlipLine + 1, pTran->TranCurQual.usConsNo,
          aszMnem, ulID, pTran->TranCurQual.ulStoreregNo / 1000L, pTran->TranCurQual.ulStoreregNo % 1000L, aszDate);

    return(1);
}

/*
*===========================================================================
** Format  : USHORT  PrtSPTrail2(UCHAR  *pszWork, TRANINFORMATION  *pTran);
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*
*   Output : UCHAR  *pszWork           -destination buffer address
*   InOut  : none
*
** Return  : USHORT  usSetLine         -number of lines to be set
*
** Synopsis: This function sets trailer data.
*===========================================================================
*/
USHORT PrtSPTrail2(TCHAR *pszWork, TRANINFORMATION  *pTran)
{
    ULONG   ulID = 0;
    USHORT  usSetLine = 0;
	TCHAR   aszDate[PRT_DATETIME_LEN + 1] = { 0 };
	TCHAR   aszMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };  /* PARA_... defined in "paraequ.h" */
/*		//SR 124 cwunn
	TCHAR uchTranNum[8], uchRegNum[8], uchTranNumFinal[8];
	USHORT usTranNumFinal;
	int tranLen, regLen, i;

	//ensure clear buffers
	for(i=0; i<8;i++){
		uchTranNum[i] = 0;
		uchRegNum[i] = 0;
		uchTranNumFinal[i] = 0;
	}*/

    if ( PRT_CASHIER == PrtChkCasWai(aszMnem, pTran->TranModeQual.ulCashierID, pTran->TranModeQual.ulWaiterID) ) {
        ulID = RflTruncateEmployeeNumber(pTran->TranModeQual.ulCashierID);
    } else {
        ulID = RflTruncateEmployeeNumber(pTran->TranModeQual.ulWaiterID);
    }

    /* -- get date and time -- */
    PrtGetDate(aszDate, TCHARSIZEOF(aszDate), pTran);
	/*Begin SR 124 cwunn
	//convert table number and store/regiser number into strings
	_itot(pTran->TranGCFQual.usTableNo, uchTranNum, 10);
	_itot(pTran->TranCurQual.ulStoreregNo, uchRegNum, 10);
	regLen = _tcslen(uchRegNum);
	tranLen = _tcslen(uchTranNum);


	//pull the transaction number from the table number, by removing the store/register numbers
	_tcsncpy(uchTranNumFinal, uchTranNum+regLen, tranLen-regLen);

	usTranNumFinal = _ttoi(uchTranNumFinal);

    RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPTrail2,
        usTranNumFinal, aszMnem, usID, pTran->TranCurQual.ulStoreregNo / 1000,
            pTran->TranCurQual.ulStoreregNo % 1000, aszDate);
	/End SR 124*/

    RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPTrail2, pTran->TranCurQual.usConsNo, aszMnem, ulID, pTran->TranCurQual.ulStoreregNo / 1000,
            pTran->TranCurQual.ulStoreregNo % 1000, aszDate);

    return(1);
}

/*
*===========================================================================
** Format  : USHORT  PrtSPChkPaid(UCHAR  *pszWork, UCHAR uchAdr);
*
*   Input  : UCHAR  uchAdr
*
*   Output : UCHAR  *pszWork           -destination buffer address
*   InOut  : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets check paid data.
*===========================================================================
*/
USHORT PrtSPChkPaid(TCHAR *pszWork, USHORT usAdr)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };    /* PARA_... defined in "paraequ.h" */
	TCHAR  aszMnemDbl[PARA_TRANSMNEMO_LEN * 2 + 1] = { 0 };    /* PARA_... defined in "paraequ.h" */

    /* -- get check paid mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- convert character to double chara -- */
    PrtDouble(aszMnemDbl, PARA_TRANSMNEMO_LEN * 2 + 1, aszTranMnem);

    /* set check paid mnemonic */
    RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPChkPaid, aszMnemDbl);

    return(1);
}

/*
*===========================================================================
** Format  : USHORT  PrtSPTrayCount(UCHAR  *pszWork, UCHAR uchAdr, SHORT sTrayCo);
*
*   Input  : UCHAR  uchAdr             -tray total counter mnemonic
*            SHORT  TrayCo             -tray total counter
*
*   Output : UCHAR  *pszWork           -destination buffer address
*   InOut  : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets tray .
*===========================================================================
*/
USHORT PrtSPTrayCount(TCHAR *pszWork, USHORT usAdr, SHORT sTrayCo)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };  /* PARA_... defined in "paraequ.h" */

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* set check paid mnemonic */
    RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPTrayCo, aszTranMnem, sTrayCo);

    return(1);
}

/*
*===========================================================================
** Format  : USHORT  PrtSPServiceTotal(UCHAR  *pszWork, UCHAR uchAdr, LONG lAmount,
*                                     BOOL fsType, TRANINFORMATION  *pTran, USHORT  usSlipLine );
*
*   Input  : UCHAR  uchAdr              -Transacion mnemonics address
*            LONG   lAmount             -amount
*            BOOL   fsType              -character type
*                                         PRT_SINGLE: single character
*                                         PRT_DOUBLE: double character
*            TRANINFORMATION  *pTran    -transaction information
*            USHORT  usSilpLine         -number of lines to be printed
*
*   Output : UCHAR  *pszWork            -destination buffer address
*   InOut  : none
*
** Return  : USHORT  usSetLine          -number of lines to be set
*
** Synopsis: This function sets the data in service total operation.
*===========================================================================
*/
USHORT PrtSPServiceTotal( TCHAR  *pszWork, USHORT  usAdr, DCURRENCY  lAmount,
                    BOOL  fsType, TRANINFORMATION  *pTran, USHORT  usSlipLine )
{
    ULONG  ulID = 0;
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};
    TCHAR  aszMnem[PARA_SPEMNEMO_LEN + 1] = {0};
    TCHAR  aszSpecAmtS[PARA_SPEMNEMO_LEN +PRT_AMOUNT_LEN + 1] = {0};
    TCHAR  aszSpecAmtD[(PARA_SPEMNEMO_LEN + PRT_AMOUNT_LEN) * 2 + 1] = {0};
    TCHAR  aszStrLen[PRT_SPCOLUMN*2 + 1] = {0};
    USHORT usStrLen;
    USHORT usDblLen;

    if ( PRT_CASHIER == PrtChkCasWai(aszMnem, pTran->TranModeQual.ulCashierID, pTran->TranModeQual.ulWaiterID) ) {
        ulID = RflTruncateEmployeeNumber(pTran->TranModeQual.ulCashierID);
    } else {
        ulID = RflTruncateEmployeeNumber(pTran->TranModeQual.ulWaiterID);
    }

    /* -- get transaction mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- adjust native mnemonic and amount sign(+.-) -- */
    PrtAdjustNative(aszSpecAmtS, lAmount);

    memset(aszStrLen, '\0', sizeof(aszStrLen));
    RflSPrintf(aszStrLen, TCHARSIZEOF(aszStrLen), aszPrtSPServTotal1, usPrtSlipPageLine % PRT_SLIP_MAX_LINES + usSlipLine + 1, pTran->TranCurQual.usConsNo, aszMnem, ulID, aszTranMnem);
    usStrLen = _tcslen(aszStrLen);
    usDblLen = (_tcslen(aszSpecAmtS) * 2);

    /* -- double wide print possible ? -- */
    if ((fsType == PRT_DOUBLE) && (PRT_SPCOLUMN >= (usStrLen + usDblLen + 1))) {
        /* -- convert single char. to double -- */
        PrtDouble(aszSpecAmtD, ((PARA_SPEMNEMO_LEN + PRT_AMOUNT_LEN) * 2 + 1), aszSpecAmtS);
        RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPServTotal2, aszStrLen, aszSpecAmtD);
    } else {
        RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPServTotal2, aszStrLen, aszSpecAmtS);
    }
    return(1);
}

/*
*===========================================================================
** Format  : USHORT  PrtSPGCTranTrail(UCHAR *pszWork, ITEMMISC *pItem)
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*          : USHORT  usSilpLine         -number of lines to be printed
*
*   Output : UCHAR  *pszWork            -destination buffer address
*   InOut  : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets the trailer data in guest check transfer. V3.3
*===========================================================================
*/
USHORT  PrtSPGCTranTrail(TCHAR *pszWork, ITEMMISC *pItem)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 }; /* PARA_... defined in "paraequ.h" */

    /* -- get transferred balance mnemonics -- */
	RflGetTranMnem(aszTranMnem, TRN_TRNSB_ADR);

    RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPGCTrnTril, aszTranMnem, pItem->lAmount);

    return(1);
}

/*
*===========================================================================
** Format   : USHORT PrtSPHeader1( UCHAR            *pszWork,
*                                  TRANINFORMATION  *pTran,
*                                  SHORT            sWidthType )
*
*   Input   : UCHAR           *pszWork   -  Transacion mnemonics address
*             TRANINFORMATION *pTran     -  Transaction information address
*             SHORT           sWidthType -  Print Width Type
*                               PRT_DOUBLE, PRT_SINGLE
*
*   Output  : UCHAR           *pszWork   -  Destination buffer address
*   InOut   : none
*
** Return   : number of lines to be set
*
** Synopsis : This function sets header 1st line to the buffer 'pszWork'.
*===========================================================================
*/
USHORT PrtSPHeader1( TCHAR *pszWork, TRANINFORMATION *pTran, SHORT sWidthType )
{
    ULONG   ulID = 0;
	TCHAR   aszMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };
    TCHAR   aszTranMnem[ PARA_TRANSMNEMO_LEN + 1 ] = { 0 }; /* PARA_... defined in "paraequ.h" */
    TCHAR   aszSpecMnem[ PARA_SPEMNEMO_LEN + 1 ] = { 0 };   /* PARA_... defined in "paraequ.h" */
    TCHAR   aszTblNo[ PRT_ID_LEN + 1 ] = { 0 };
    TCHAR   aszGCNo[ PRT_GC_LEN + 1 ] = { 0 };
    TCHAR   aszPageNo[ PRT_PAGENO_LEN + 1 ] = { 0 };
    TCHAR   aszPageDbl[ PRT_PAGENO_LEN * 2 + 1 ] = { 0 };

    /* --- set up operator special mnemonic, and operator ID --- */

    if ( PRT_CASHIER == PrtChkCasWai( aszMnem, pTran->TranModeQual.ulCashierID, pTran->TranModeQual.ulWaiterID )) {
        ulID = RflTruncateEmployeeNumber(pTran->TranModeQual.ulCashierID);
    } else {
        ulID = RflTruncateEmployeeNumber(pTran->TranModeQual.ulWaiterID);
    }

    /* --- set up table no. special mnemonic, and retrieve table no. or
        unique transaction no. --- */

    if (( pTran->TranGCFQual.usTableNo != 0 ) && ( sWidthType == PRT_SINGLE )) {
        _itot( pTran->TranGCFQual.usTableNo, aszTblNo, 10 );
		RflGetSpecMnem( aszSpecMnem, SPC_TBL_ADR );
    }

    /* --- set up GC no. special mnemonic, and GC no. --- */
    if ( pTran->TranGCFQual.usGuestNo != 0 ) {
        _itot( pTran->TranGCFQual.usGuestNo, aszGCNo, 10 );
		RflGetTranMnem( aszTranMnem, TRN_GCNO_ADR );
    }

    /* -- convert page No. to double chara -- */
    _itot((( usPrtSlipPageLine + PRT_SLIP_MAX_LINES ) / PRT_SLIP_MAX_LINES ), aszPageNo, 10 );
    PrtDouble( aszPageDbl, ( PRT_PAGENO_LEN * 2 + 1 ), aszPageNo );

    /* -- set header 1st line -- */
    if ( pTran->TranGCFQual.uchCdv == 0 ) {
        /* --- print GC# without Check Digit --- */
        RflSPrintf( pszWork, ( PRT_SPCOLUMN + 1 ), aszPrtSPHdr1, aszMnem, ulID, aszSpecMnem, aszTblNo, aszTranMnem, aszGCNo, aszPageDbl );
    } else {
        /* --- print GC# with Check Digit --- */
        /* --- print table no. with single width --- */
        RflSPrintf( pszWork, ( PRT_SPCOLUMN + 1 ), aszPrtSPHdr1Cdv, aszMnem, ulID, aszSpecMnem, aszTblNo, aszTranMnem, aszGCNo, pTran->TranGCFQual.uchCdv, aszPageDbl );
    }
    return ( 1 );
}

/*
*===========================================================================
** Format  : USHORT  PrtSPHeader2(UCHAR  *pszWork, TRANINFORMATION  *pTran)
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*
*   Output : UCHAR  *pszWork           -destination buffer address
*   InOut  : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets header 2nd line to the buffer 'pszWork'.
*===========================================================================
*/
USHORT PrtSPHeader2(TCHAR *pszWork, TRANINFORMATION  *pTran, TCHAR uchSeatNo)
{
	TCHAR  aszMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };
	TCHAR  aszNoPer[PRT_ID_LEN + 1] = { 0 };
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };  /* NUM_... defined in "regstrct.h" */
	TCHAR  aszDate[PRT_DATETIME_LEN + 1] = { 0 };
	TCHAR  aszName[PARA_CASHIER_LEN + 1] = { 0 };

    if ( PRT_CASHIER == PrtChkCasWai(aszMnem, pTran->TranModeQual.ulCashierID, pTran->TranModeQual.ulWaiterID) ) {
        _tcsncpy(aszName, pTran->TranModeQual.aszCashierMnem, PRT_CASNAME_LEN);
    } else {
        _tcsncpy(aszName, pTran->TranModeQual.aszWaiterMnem, PRT_CASNAME_LEN);
    }

    if (uchSeatNo) {
        /* --- set up sepcial mnemonic of seat no, and seat no --- */
        _itot( uchSeatNo, aszNoPer, 10 );
		RflGetSpecMnem( aszTranMnem, SPC_SEAT_ADR );    /* seat no. mnemonic */
    } else if ((pTran->TranGCFQual.usNoPerson != 0) && (!CliParaMDCCheck(MDC_TRANNUM_ADR, EVEN_MDC_BIT3))) {
        /* --- set up sepcial mnemonic of no. of person, and no. of person --- */
        _itot(pTran->TranGCFQual.usNoPerson, aszNoPer, 10);            /* convert int to ascii */
		RflGetTranMnem(aszTranMnem, TRN_PSN_ADR);  /* get no of person mnemo */
    }

    PrtGetDate(aszDate, TCHARSIZEOF(aszDate), pTran);   /* get date time */

    RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPHeader2nd, aszName, aszTranMnem, aszNoPer, aszDate);

    return(1);
}

/*
*===========================================================================
** Format  : USHORT PrtSPHeader3( UCHAR *pszWork, TRANINFORMATION *pTran)
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*
*   Output : UCHAR  *pszWork           -destination buffer address
*   InOut  : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets header 3rd line to the buffer 'pszWork'.
*===========================================================================
*/
USHORT PrtSPHeader3( TCHAR *pszWork, TRANINFORMATION  *pTran )
{
    if (( pTran->TranGCFQual.usTableNo != 0 ) && ( pTran->TranCurQual.flPrintStatus & CURQUAL_UNIQUE_TRANS_NO )) {
		TCHAR   aszLeadThru[PARA_LEADTHRU_LEN + 1] = { 0 };
		TCHAR   aszTransNo[ PRT_ID_LEN * 2 + 1 ] = { 0 };
		TCHAR   aszTblNo[ PRT_ID_LEN + 1 ] = { 0 };

        _itot( pTran->TranGCFQual.usTableNo, aszTblNo, 10 );
        PrtDouble( aszTransNo, (PRT_ID_LEN * 2 + 1 ), aszTblNo );

        PrtGetLead( aszLeadThru, LDT_UNIQUE_NO_ADR );

        RflSPrintf( pszWork, ( PRT_SPCOLUMN + 1 ), aszPrtSPHdrTrans, aszLeadThru, aszTransNo, pTran->TranGCFQual.aszName );

        return ( 1 );
    }

    return ( 0 );
}

/*
*===========================================================================
** Format  : USHORT  PrtSPItems(UCHAR  *pszWork, ITEMSALES  *pItem);
*
*   Input  : ITEMSALES  *pItem    -structure "itemsales" pointer
*
*   Output : UCHAR  *pszWork      -destination buffer address
*   InOut  : none
*
** Return  : USHORT usSetLine     -number of line(s) to be set
*
** Synopsis: This function sets the mnemonics of adjective, noun,
*            print modifier, conidiment, and product amount.
*===========================================================================
*/
USHORT  PrtSPItems(TCHAR  *pszWork, TRANINFORMATION *pTran, ITEMSALES  *pItem)
{
    TCHAR      aszItem[PRT_MAXITEM + PRT_SPCOLUMN] = {0};
	TCHAR      aszAmt[PRT_AMOUNT_LEN + 1 + 1] = {0};      /* 1 + 1 ( space + terminate) */
    DCURRENCY  lProduct;
    TCHAR      *pszStart;
    TCHAR      *pszEnd;
    USHORT     usAmtLen;
    USHORT     usMnemLen;
    USHORT     usSetLine = 0;
    SHORT      sPrintType, i;

    pszStart = aszItem;

    /* --- set condiment print status by mdc, R3.1 --- */
    sPrintType = PrtChkPrintType(pItem);

    /* -- set item mnenemonics to buffer -- */
    PrtSetItem(aszItem, pItem, &lProduct, sPrintType, 0);

    /* -- print out net sales price -- */
    if ((pItem->uchMinorClass == CLASS_DEPTMODDISC)
            || (pItem->uchMinorClass == CLASS_PLUMODDISC)
            || (pItem->uchMinorClass == CLASS_SETMODDISC)
            || (pItem->uchMinorClass == CLASS_OEPMODDISC)
            || (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE)) {
        ;
    } else if (PrtChkVoidConsolidation(pTran, pItem) == FALSE) {
        ;   /* not consolidation */
    } else {    /* void consolidation */
		D13DIGITS d13Work;

        /* recalculate sales price form sCouponQTY */
        d13Work = pItem->lUnitPrice;
        for (i = pItem->uchChildNo; i < STD_MAX_COND_NUM; i++) {
           d13Work += pItem->Condiment[i].lUnitPrice;
        }
        /* --- calcurate product * qty --- */
        lProduct *= (LONG)pItem->sCouponQTY;
    }

    for (;;) {
        /* -- get 1 line -- */
        if ( PRT_LAST == PrtGet1Line(pszStart, &pszEnd, PRT_SPCOLUMN - PRT_AMOUNTLEN) ) {
            break;
        }

        /* set adjective, noun mnemo, print modifier, condiment */
        tcharnset(pszWork, 0x20, PRT_SPCOLUMN);
        _tcsncpy(pszWork, pszStart, pszEnd - pszStart);

        /* -- "1" for space -- */
        pszStart = pszEnd + 1;
        pszWork += PRT_SPCOLUMN + 1;

        usSetLine ++;
    }

    /* get the length of mnemonics */
    usMnemLen = _tcslen(pszStart);
    memset(aszAmt, '\0', sizeof(aszAmt));
    RflSPrintf(aszAmt, TCHARSIZEOF(aszAmt), aszItemAmt, lProduct);
    usAmtLen = _tcslen(aszAmt) + 5;     /* 5 is for space b/w mnemonics and amount */

    /* if data length is over 1 line length, separate into 2 lines */
    if (usMnemLen + usAmtLen > PRT_SPCOLUMN) {
        tcharnset(pszWork, 0x20, PRT_SPCOLUMN);
        _tcsncpy(pszWork, pszStart, usMnemLen);
        usSetLine ++;
        pszWork += PRT_SPCOLUMN + 1;
        RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPItemAmt, aszAmt);
        usSetLine ++;
    } else {
        RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPItemAmt, aszAmt);
        _tcsncpy(pszWork, pszStart, usMnemLen);
        usSetLine ++;
    }

    return(usSetLine);
}


/*
*===========================================================================
** Format  : USHORT  PrtSPEtkTimeIn(UCHAR  *pszWork, TRANINFORMATION *pTran,
*                                       ITEMMISC  *pItem);
*
*   Input  : TRANINFORMATION  *pTran    -structure "transaction information" pointer
*            ITEMMISC  *pItem           -structure "itemmisc" pointer
*
*   Output : UCHAR  *pszWork      -destination buffer address
*   InOut  : none
*
** Return  : USHORT usSetLine     -number of line(s) to be set
*
** Synopsis: This function sets ETK time-in data.
*===========================================================================
*/
USHORT  PrtSPEtkTimeIn(TCHAR  *pszWork, TRANINFORMATION *pTran, ITEMMISC  *pItem)
{
	TCHAR   aszDate[PRT_DATETIME_LEN + 1] = { 0 };
	TCHAR   aszCode[4] = { 0 };

    /* -- get job time-in time -- */
    PrtGetJobInDate(aszDate, TCHARSIZEOF(aszDate), pTran, pItem);
                                                   /* get date time */
    if ( pItem->uchJobCode != 0 ) {
        RflSPrintf(&aszCode[1], 3, aszEtkCode, pItem->uchJobCode);
        aszCode[0] = _T(' ');
        aszCode[3] = _T('\0');
    } else {
        aszCode[0] = _T('\0');
    }

    RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPEtkTimeIn, aszDate, aszCode);

    return(1);
}

/*
*===========================================================================
** Format  : USHORT  PrtSPEtkTimeOut(UCHAR  *pszWork, TRANINFORMATION *pTran,
*                                       ITEMMISC  *pItem, SHORT sType);
*
*   Input  : TRANINFORMATION  *pTran    -structure "transaction information" pointer
*            ITEMMISC  *pItem           -structure "itemmisc" pointer
*            SHORT      sType           -print type
*                                           PRT_WTIN: with time-in
*                                           PRT_WOIN: without time-in
*
*   Output : UCHAR  *pszWork      -destination buffer address
*   InOut  : none
*
** Return  : USHORT usSetLine     -number of line(s) to be set
*
** Synopsis: This function sets ETK time-out data.
*===========================================================================
*/
USHORT  PrtSPEtkTimeOut(TCHAR  *pszWork, TRANINFORMATION *pTran, ITEMMISC  *pItem, SHORT sType)
{
	TCHAR   aszDateIn[PRT_DATETIME_LEN + 1] = { 0 };
	TCHAR   aszDateOut[PRT_DATETIME_LEN + 1] = { 0 };

    /* -- get job time-out time -- */
    PrtGetJobOutDate(aszDateOut, TCHARSIZEOF(aszDateOut), pTran, pItem);   /* get date time */
    if ( sType == PRT_WOIN) {
        tcharnset(aszDateIn, '*', PRT_DATETIME_LEN);
        aszDateIn[PRT_DATETIME_LEN] = _T('\0');
    } else {
        *aszDateIn = _T('\0');
    }

    RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPEtkTimeOut, aszDateIn, aszDateOut);

    return(1);
}

/*
*===========================================================================
** Format  : VOID  PrtSPCheckLine(USHORT  sSPCurrLine, TRANINFORMATION  *pTran);
*
*   Input  : USHORT  sSPCurrLine      - number of lines to be printed
*            TRANINFORMATION  *pTran  -Transaction Information address
*
*   Output : none
*   InOut  : none
*
** Return  : USHORT usSlipLine        -number of lines to be set
*
** Synopsis: This function checks the rest of the slip lines.
*
*               - lack of lines to be printed ?
*                   - print 'continue to next slip' message
*                   - print trailer
*                   - request to change the slip
*                   - print the header lines
*
*===========================================================================
*/
USHORT PrtCheckLine(USHORT  usSPCurrLine, TRANINFORMATION  *pTran)
{
    TCHAR   aszSPPrintBuff[10][PRT_SPCOLUMN + 1];
    USHORT  usSPPrintedLine;         /* number of printed lines */
    USHORT  usMaxLine;               /* number of max lines to be printed */
    USHORT  usCurrPage;              /* current page number */
    USHORT  usSetLine = 0;           /* number of lines to be set */
//    UCHAR   fbStatus;                /* get the status of paper change */
    USHORT  usMsgNo;                 /* set error message address */
    USHORT  i;
    SHORT   sWidthType;
    PARASLIPFEEDCTL  FeedCtl;
    USHORT  usPrevious;

    /* -- get the number of printed lines  -- */
    usSPPrintedLine = usPrtSlipPageLine % PRT_SLIP_MAX_LINES;

    /* -- get max slip line -- */
/*    memset(&FeedCtl, '\0', sizeof(PARASLIPFEEDCTL));   initialize */
    FeedCtl.uchMajorClass = CLASS_PARASLIPFEEDCTL;
    FeedCtl.uchAddress = SLIP_MAXLINE_ADR;
    CliParaRead(&FeedCtl);
    usMaxLine = (USHORT)FeedCtl.uchFeedCtl;

    /* check if paper change is necessary or not */
    if (usSPPrintedLine + usSPCurrLine <= usMaxLine - 2) {
        return(usSetLine);           /* enough lines to be printed ? */
    }                                /* '2' is for trailer */

    /* initialize the buffer */
    memset(aszSPPrintBuff[0], '\0', sizeof(aszSPPrintBuff));

    /* -- set "Continue to Next" Message -- */
    PrtGet24Mnem(aszSPPrintBuff[0], CH24_NXTMSG_ADR);
    usSetLine ++;

    /* -- set trailer line -- */
    PrtSPTrail1(aszSPPrintBuff[1], pTran, usSetLine);

    /* -- print 2 lines -- */
    PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[0], PRT_SPCOLUMN);
    PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[1], PRT_SPCOLUMN);

    /* -- slip release -- */
    PrtSlpRel();

    /* wait until slip printing is finished */
    PrtSlipPmgWait();

    /* allow power switch at error display */
    usPrevious = PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */

    usMsgNo = LDT_PAPERCHG_ADR;
    for (;;) {

        /* allow power switch at error display */
        PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */
        /* -- error message "slip change" -- */
        UieErrorMsg(usMsgNo, UIE_WITHOUT);

        /* -- slip printer on ? -- */
//        if (PmgGetStatus(PMG_PRT_SLIP, &fbStatus) == PMG_SUCCESS) {

            /* -- slip paper changed, and slip printer no error ? -- */
      //      if ((fbStatus & PRT_STAT_FORM_IN) && (fbStatus & PRT_STAT_ERROR)) {

                /* -- get the number of lines for pre-feed -- */
                FeedCtl.uchAddress = SLIP_NO1ST_ADR;
                CliParaRead(&FeedCtl);

                /* -- feed the lines for print before header line -- */
                PrtFeed(PMG_PRT_SLIP, (USHORT)FeedCtl.uchFeedCtl);
                break;

           // } else {
           //     if (fbStatus & PRT_STAT_ERROR) {
            //        usMsgNo = LDT_PAPERCHG_ADR; /* request paper cheange */
            ////        continue;
            //    }
            //}
        //}
        usMsgNo = LDT_PRTTBL_ADR;               /* printer error */
    }

    PifControlPowerSwitch(usPrevious);  /* V1.0.04 */

    /* change page No. and line No. */
    usCurrPage = usPrtSlipPageLine / PRT_SLIP_MAX_LINES;    /* save the current page number */
    usPrtSlipPageLine = usCurrPage * PRT_SLIP_MAX_LINES + PRT_SLIP_MAX_LINES + (USHORT)FeedCtl.uchFeedCtl;
                                /* usPrtSlipPageLine = new page No. & line */
    usSetLine = 0;

    /* -- set header lines -- */
    memset(aszSPPrintBuff[0], '\0', sizeof(aszSPPrintBuff));
                                                /* initialize */
    /* -- training mode ? -- */
    if ( pTran->TranCurQual.fsCurStatus & CURQUAL_TRAINING) { /* training mode */
        PrtGet24Mnem(aszSPPrintBuff[usSetLine], CH24_TRNGHED_ADR);    /* -- set training mode mnemonic -- */
        usSetLine ++;

    }

    /* -- preselect void operation ? -- */
    if ( pTran->TranCurQual.fsCurStatus & CURQUAL_PVOID) {  /* preselect void */
        PrtGet24Mnem(aszSPPrintBuff[usSetLine], CH24_PVOID_ADR);   /* -- set preselect void mnemonic -- */
        usSetLine ++;
    }
    /* --- Returns Transaction ? -- */
    if ( (pTran->TranCurQual.fsCurStatus) & (CURQUAL_TRETURN | CURQUAL_PRETURN) ) {
        PrtGet24Mnem(aszSPPrintBuff[usSetLine], CH24_PRETURN_ADR);   /* -- set returns transaction mnemonic -- */
        usSetLine ++;
    }

    /* --- determine current system uses unique tansaction no. --- */
    if ( pTran->TranCurQual.flPrintStatus & CURQUAL_UNIQUE_TRANS_NO ) {
        sWidthType = PRT_DOUBLE;
    } else {
        sWidthType = PRT_SINGLE;
    }

    if ((auchPrtSeatCtl[0] == 0)||(auchPrtSeatCtl[1] == 0)) {
        /* service ttl 1st print */
        usSetLine += PrtSPHeader3(aszSPPrintBuff[usSetLine], pTran);
        usSetLine += PrtSPHeader1(aszSPPrintBuff[usSetLine], pTran, sWidthType);
        if ((pTran->TranGCFQual.auchFinSeat[1]) && (uchPrtSeatNo)) {
            usSetLine += PrtSPHeader2(aszSPPrintBuff[usSetLine], pTran, 0);
        } else {
            usSetLine += PrtSPHeader2(aszSPPrintBuff[usSetLine], pTran,
                (UCHAR)(auchPrtSeatCtl[0] ? 0 : uchPrtSeatNo));
        }

        usSetLine += PrtSPCustomerName( aszSPPrintBuff[ usSetLine ], pTran->TranGCFQual.aszName );

        /* -- set multiple seat no -- */
        if ((pTran->TranGCFQual.auchFinSeat[1]) && (uchPrtSeatNo)) {
            usSetLine += PrtSPMultiSeatNo( aszSPPrintBuff[ usSetLine ], pTran);
        } else if (auchPrtSeatCtl[0]) {
			/* -- set seat no -- */
			usSetLine += PrtSPSeatNo(aszSPPrintBuff[usSetLine], uchPrtSeatNo);    /* seat no. */
        }
    } else {
        /* service ttl 2nd print */
        usSetLine += PrtSPHeader3(aszSPPrintBuff[usSetLine], pTran);
        usSetLine += PrtSPHeader1(aszSPPrintBuff[usSetLine], pTran, sWidthType);
        usSetLine += PrtSPHeader2(aszSPPrintBuff[usSetLine], pTran, 0);
        usSetLine += PrtSPCustomerName( aszSPPrintBuff[ usSetLine ], pTran->TranGCFQual.aszName );
        usSetLine += PrtSPSeatNo(aszSPPrintBuff[usSetLine], uchPrtSeatNo);    /* seat no. */
    }

    /* -- print all data in the buffer -- */
    for (i = 0; i < usSetLine; i++) {
        PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);
    }

    return(usSetLine);
}

/*
*===========================================================================
** Format  : USHORT PrtSPCPRoomCharge(UCHAR *pszWork, UCHAR *pszWork, UCHAR *pRoomNo, UCHAR *pGuestID);
*
*   Input  : UCHAR  *pRoomNo        -Pointer of Room Number
*            UCHAR  *pGuestID       -Pointer of Guest ID
*
*   Output : UCHAR *pszWork         -destination buffer address
*    InOut : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets room number and guest id.
*===========================================================================
*/

USHORT PrtSPCPRoomCharge(TCHAR *pszWork, TCHAR *pRoomNo, TCHAR *pGuestID)
{
	TCHAR  aszSpecMnem1[PARA_SPEMNEMO_LEN + 1] = { 0 };
	TCHAR  aszSpecMnem2[PARA_SPEMNEMO_LEN + 1] = { 0 };

	RflGetSpecMnem(aszSpecMnem1, SPC_ROOMNO_ADR);   /* get room no. mnemo */
	RflGetSpecMnem(aszSpecMnem2, SPC_GUESTID_ADR);  /* get guest id */

    RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPCPRoomCharge, aszSpecMnem1, pRoomNo, aszSpecMnem2, pGuestID);
    return(1);
}

/*
*===========================================================================
** Format  : USHORT  PrtSPCPOffTend(UCHAR *pszWork, UCHAR fbMode);
*
*    Input : UCHAR  fbMod
*
*   Output : UCHAR  *pszWork         -destination buffer address
*    InOut : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets 'off line tender' to the buffer
*            'pszWork'.
*===========================================================================
*/
USHORT PrtSPCPOffTend(TCHAR *pszWork, USHORT fbMod)
{
    if (fbMod & OFFCPTEND_MODIF) {
        PrtGetOffTend(fbMod, pszWork, PARA_TRANSMNEMO_LEN * 2 + 1);
        return(1);
    }
    return(0);
}

/*
*===========================================================================
** Format  : USHORT PrtSPCPRspMsgText(UCHAR *pszWork, UCHAR *pszRspMsgText);
*
*   Input  : UCHAR  *pszRspMsgText  -Pointer of Response Message Text
*
*   Output : UCHAR *pszWork         -destination buffer address
*    InOut : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets response message text for chrge posting.
*===========================================================================
*/
USHORT PrtSPCPRspMsgText(TCHAR *pszWork, TCHAR *pszRspMsgText)
{
    if (*pszRspMsgText != '\0') {
        RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPCPRspMsgText, pszRspMsgText);
        return(1);
    }
    return(0);
}

/*
*===========================================================================
** Format  : USHORT  PrtSPOffline(UCHAR *pszWork, UCHAR fbMod,
*                           USHORT usDate UCHAR *aszApproval );
*
*    Input : UCHAR fbMod           -offline modifier
*            USHORT usDate         -expiration date
*            UCHAR  *aszApproval   -approval code
*
*   Output : UCHAR *pszWork         -destination buffer address
*    InOut : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function prints prints EPT offline symbol
*            and expiration date.
*  Print format :
*
*       0        1         2         3         4
*       12345678901234567890123456789012345678901234
*      |^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
*      |MMMM       MM/YY  123456                    |
*      |                                            |
*       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
*===========================================================================
*/
USHORT  PrtSPOffline(TCHAR *pszWork, USHORT fbMod, TCHAR *auchDate, TCHAR *auchApproval)
{
	TCHAR  aszApproval[NUM_APPROVAL + 1] = { 0 };
	TCHAR  aszDate[NUM_EXPIRA + 1] = { 0 };
	TCHAR  aszOffMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };
	TCHAR  aszOffDate[5 + 1] = { 0 };
    USHORT usOffDate;
    USHORT usRetLine = 0;

    _tcsncpy(aszApproval, auchApproval, NUM_APPROVAL);
    _tcsncpy(aszDate, auchDate, NUM_EXPIRA);

    if ( fbMod & OFFEPTTEND_MODIF) {                /* offline modifier */
        /* -- get special mnemonics -- */
		RflGetSpecMnem( aszOffMnem, SPC_OFFTEND_ADR);
    }

    if ( *aszDate != '\0' ){                        /* expiration date  */
        usOffDate = _ttoi(aszDate);
        if (usOffDate != 0) {                       /* E-028 corr. 4/20     */
            RflSPrintf( aszOffDate, TCHARSIZEOF(aszOffDate), aszPrtSPOffDate, usOffDate / 100, usOffDate % 100 );
        }
    }

    /* -- send mnemoncis and exp.date -- */
    RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPOffline, aszOffMnem, aszOffDate, aszApproval);
    if (*aszDate == '\0' && *aszApproval == '\0' && *aszOffMnem == '\0') {
        return(0);
    }
    return(1);
}

/*
*===========================================================================
** Format  : VOID   PrtTrailEPT_SP( UCHAR   *pszWork );
*
*    Input :
*
*   Output :
*    InOut : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets and stores EPT logo mnemonics(42-char).
*
*  Print format :
*
*       0        1         2         3         4
*       12345678901234567890123456789012345678901234
*      |^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
*      |** EPT TRAILER LOGO #1**                    |
*      |** EPT TRAILER LOGO #2**                    |
*      |** EPT TRAILER LOGO #3**                    |
*      |** EPT TRAILER LOGO #4**                    |
*      |** EPT TRAILER LOGO #5**                    |
*      |                                            |
*       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
*===========================================================================
*/

VOID    PrtTrailEPT_SP( TCHAR   *pszWork )
{
    UCHAR   i;
    PARASOFTCHK PM;

    PM.uchMajorClass = CLASS_PARASOFTCHK;

    for ( i = 0; i < 5; i++ ) {
        PM.uchAddress = i + (UCHAR)SOFTCHK_EPT1_ADR;
        memset( PM.aszMnemonics, '\0', sizeof( PM.aszMnemonics ) );
        CliParaRead( &PM );

        _tcsncpy(pszWork+(PRT_SPCOLUMN+1)*i, PM.aszMnemonics, PARA_SOFTCHK_LEN);

        if (*(pszWork + (PRT_SPCOLUMN+1) * i) == '\0') {
            tcharnset(pszWork+(PRT_SPCOLUMN+1)*i, _T(' '), PARA_SOFTCHK_LEN);
        }
    }
}

/*
*===========================================================================
** Format  : USHORT  PrtSPTranNumber(UCHAR *pszWork, UCHAR uchAdr,
*                                    UCHAR *pszNumber)
*
*   Input  : UCHAR    *pszNumber
*   Input  : UCHAR    uchAdr
*   Output : none
*   InOut  : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function prints Account # mnemonics and 25-digit number
*            If number is 0, then it is not printed.
*
*  Print format :
*
*       0        1         2         3         4
*       12345678901234567890123456789012345678901234
*      |^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
*      |        MMMMMMMM   9999999999999999999999999|
*      |                                            |
*       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
*===========================================================================
*/
USHORT PrtSPTranNumber(TCHAR *pszWork, USHORT usTranAdr, TCHAR *pszNumber)
{
    if ( pszNumber[0] != 0) {
		TCHAR  aszNumberMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };

		RflGetTranMnem( aszNumberMnem, usTranAdr );
        RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPTranNumber, aszNumberMnem, pszNumber);
        return(1);
    }
    return(0);
}

/*
*===========================================================================
** Format  : VOID  PrtSPVLTrail(TRANINFORMATION *pTran);
*
*    Input : TRANINFORMATION    *pTran       -transaction information
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints validation trailer to slip.
*===========================================================================
*/
VOID  PrtSPVLTrail(TRANINFORMATION *pTran)
{

	TCHAR  achWork[12] = { 0 };
	TCHAR  aszMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };
	TCHAR  aszDate[PRT_DATETIME_LEN + 1] = { 0 };
    ULONG  ulID = 0;

    /* check cashier or waiter, and get mnemonics */
    if ( PRT_CASHIER == PrtChkCasWai(aszMnem, pTran->TranModeQual.ulCashierID, pTran->TranModeQual.ulWaiterID) ) {
        ulID = RflTruncateEmployeeNumber(pTran->TranModeQual.ulCashierID);
    } else {
        ulID = RflTruncateEmployeeNumber(pTran->TranModeQual.ulWaiterID);
    }

    PrtGetDate(aszDate, TCHARSIZEOF(aszDate), pTran);   /* get date time */

    PmgPrintf(PMG_PRT_SLIP, aszPrtSPTrail2, pTran->TranCurQual.usConsNo, aszMnem, ulID,
              pTran->TranCurQual.ulStoreregNo / 1000L, pTran->TranCurQual.ulStoreregNo % 1000L, aszDate);

    achWork[0] = ESC;
    achWork[1] = _T('q');
    achWork[2] = _T('\0');
    achWork[3] = _T('\0');

    PmgPrint(PMG_PRT_SLIP, achWork, (USHORT)_tcslen(achWork));  /* praten open */

}

/*
*===========================================================================
** Format  : VOID  PrtSPVLHead(TRANINFORMATION *pTran);
*
*    Input : TRANINFORMATION      pTran         -transaction information
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints slip validation header lines.
*===========================================================================
*/
VOID  PrtSPVLHead(TRANINFORMATION *pTran)
{
    PrtSPVLHead1(pTran);                        /* validation head 1st line */

    PrtSPVLHead2(pTran);                        /* validation head 2nd line */

    PrtSPVLHead3();                             /* validation head 3rd line */
}

/*
*===========================================================================
** Format  : VOID  PrtSPVLHead1(TRANINFORMATION *pTran);
*
*    Input : TRANINFORMATION      pTran         -transaction information
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints slip validation header line.
*===========================================================================
*/
VOID  PrtSPVLHead1(TRANINFORMATION *pTran)
{
	TCHAR   aszTraining[PARA_CHAR24_LEN + 1] = { 0 };  /* training mnemo. */
	TCHAR   aszHead1[PARA_CHAR24_LEN + 1] = { 0 };     /* valid. header 1st line */

    /* --- training mode ? -- */
    if ( (pTran->TranCurQual.fsCurStatus) & CURQUAL_TRAINING ) {
        PrtGet24Mnem(aszTraining, CH24_TRNGHED_ADR); /* get training mnemo. */
    }

    PrtGet24Mnem(aszHead1,    CH24_1STVAL_ADR);      /* get validation head */

    PmgPrintf(PMG_PRT_SLIP, aszPrtSPVLHead, aszTraining, aszHead1);
}

/*
*===========================================================================
** Format  : VOID  PrtSPVLHead2(TRANINFORMATION *pTran);
*
*    Input : TRANINFORMATION      pTran         -transaction information
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function slip prints validation header line.
*===========================================================================
*/
VOID  PrtSPVLHead2(TRANINFORMATION *pTran)
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

    PrtGet24Mnem(aszHead2,    CH24_2NDVAL_ADR);    /* get validation head */

    PmgPrintf(PMG_PRT_SLIP, aszPrtSPVLHead, aszPreVoid, aszHead2);
}

/*
*===========================================================================
** Format  : VOID  PrtSPVLHead3(VOID);
*
*    Input : none
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints slip validation header line.
*===========================================================================
*/
VOID  PrtSPVLHead3(VOID)
{
	TCHAR   auchVoid[1] = { 0 };                       /* void */
	TCHAR   aszHead3[PARA_CHAR24_LEN + 1] = { 0 };     /* valid. header 1st line */

    PrtGet24Mnem(aszHead3,    CH24_3RDVAL_ADR);    /* get validation head */

    PmgPrintf(PMG_PRT_SLIP, aszPrtSPVLHead, auchVoid, aszHead3);
}

/*
*===========================================================================
** Format  : VOID  PrtSPPLUNo (UCHAR *puchPLUNo);
*
*    Input : UCHAR *puchPLUNo      -PLU or coupon No
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints PLU or vendor coupon number on specified
*            printer.
*
*  Print format :
*
*       0        1         2         3         4
*       1234567890123456789012345678901234567890
*      |^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
*      |99999999999999                          |
*      |                                        |
*       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
*===========================================================================
*/
USHORT  PrtSPPLUNo(TCHAR *pszDest, TCHAR *puchPLUCpn)
{
    if (*puchPLUCpn) {
        RflSPrintf( pszDest, ( PRT_SPCOLUMN + 1 ), aszPrtSPPLUNo, puchPLUCpn );
        return ( 1 );
    }

    return ( 0 );
}

/*
*===========================================================================
** Format  : VOID  PrtSPPLUBuild (UCHAR *puchPLUNo, UCHAR uchDeptNo);
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
*       0        1         2         3         4
*       1234567890123456789012345678901234567890
*      |^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
*      |PLU-BUIL             99999999999999 9999|
*      |                                        |
*       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
*===========================================================================
*/
USHORT    PrtSPPLUBuild (TCHAR *pszDest, USHORT usAdr, TCHAR *puchPLUNo, USHORT usDeptNo)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 }; /* PARA_... defined in "paraequ.h" */

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);
    RflSPrintf( pszDest, ( PRT_SPCOLUMN + 1 ), aszPrtSPPluBuild, aszTranMnem, puchPLUNo, usDeptNo);

    return (1);
}

/*
*===========================================================================
** Format  : USHORT  PrtSPForeignQtyFor(UCHAR *pszWork, LONG lQTY, USHORT usFor,
*                                                       LONG lUnitPrice);
*
*   Input  : LONG   lQTY
*          : USHORT usFor
*          : LONG   lUnitPrice,
*
*   Output : UCHAR      *pszWork       -destination buffer address
*   InOut  : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets  quantity, and unit price
*            to the buffer 'pszWork'.
*  Print format :
*    <Quantity >
*       0        1         2         3         4
*       12345678901234567890123456789012345678901234
*      |^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
*      |                  999999 X            999999|
*      |                                            |
*       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
*    <  FOR  >
*       0        1         2         3         4
*       12345678901234567890123456789012345678901234
*      |^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
*      |                  999999 X         999999/99|
*      |                                            |
*       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
*===========================================================================
*/
USHORT  PrtSPForeignQtyFor(TCHAR *pszWork, LONG lQTY, DCURRENCY lAmount,
                                    UCHAR uchAdr, UCHAR   fbStatus )

{
	TCHAR  aszSpecAmt[PARA_SPEMNEMO_LEN +PRT_AMOUNT_LEN + 1] = {0};
    SHORT  sDecPoint;

    if ( labs( lQTY ) == PLU_BASE_UNIT){
        return(0);

    }
    PrtSetQtyDecimal( &sDecPoint, &lQTY);

    /* -- adjust foreign mnemonic and amount sign(+.-) -- */
    PrtAdjustForeign(aszSpecAmt, lAmount, uchAdr, fbStatus);

    RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPQty, lQTY, aszSpecAmt);

    PrtResetQtyDecimal(&sDecPoint, &lQTY);

    return(1);
}

/*
*===========================================================================
** Format  : USHORT  PrtSPMoneyForeign(UCHAR  *pszWork, LONG lForeign, uchAdr1,
*                               UCHAR uchAdr2, UCHAR fbStatus);
*
*   Input  : LONG    lForeign      -Foreign tender amount
*            UCHAR   uchAdr1       -Foreign tender mnemonic address
*            UCHAR   uchAdr2       -Foreign symbol address
*            UCHAR   fbStatus      -Foreign currency decimal point status
*
*   Output : UCHAR   *pszWork      -destination buffer address
*   InOut  : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets foreign tender amount and tender mnemonic
*            to the buffer 'pszWork'.
*
*  Print Format:
*       0        1         2         3         4
*       12345678901234567890123456789012345678901234
*      |^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
*      |MMMMMMMM                         SMMMM999.99|
*      |                                            |
*       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
*===========================================================================
*/
USHORT PrtSPMoneyForeign(TCHAR* pszWork, DCURRENCY  lForeign, USHORT usTranAdr1,
                         UCHAR  uchAdr2, UCHAR fbStatus)
{
	TCHAR  aszFMnem[PARA_SPEMNEMO_LEN + 1] = {0};
    TCHAR  aszFAmt[PRT_SPCOLUMN + 1] = {0};
    TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};
    USHORT usStrLen;
    USHORT sDecPoint;

    /* -- check decimal point -- */
    if ( fbStatus & ODD_MDC_BIT1 ) {
        if ( fbStatus & ODD_MDC_BIT2 ) {
            sDecPoint = PRT_NODECIMAL;
        } else {
            sDecPoint = PRT_3DECIMAL;
        }
    } else {
        sDecPoint = PRT_2DECIMAL;
    }

    /* -- get foreign symbol -- */
	RflGetSpecMnem(aszFMnem, uchAdr2 );

    /* -- get transaction mnemonics -- */
	RflGetTranMnem( aszTranMnem, usTranAdr1 );

    /* -- get string length -- */
    usStrLen = _tcslen( aszFMnem );

    /* -- format foreign amount -- */
    RflSPrintf(&aszFAmt[usStrLen], TCHARSIZEOF(aszFAmt) - usStrLen, aszPrtSPMoneyForeign, sDecPoint, lForeign );

    /* -- adjust sign ( + , - ) -- */
    if ( lForeign < 0 ) {
        aszFAmt[0] = _T('-');
        _tcsncpy(&aszFAmt[1], aszFMnem, usStrLen );
    } else {
        _tcsncpy( aszFAmt, aszFMnem, usStrLen );
    }
    /* -- set foreign amount and conversion rate */
    RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPMnemMnem, aszTranMnem, aszFAmt );
    return(1);
}

/*
*===========================================================================
** Format  : USHORT  PrtSPQtyFor(UCHAR *pszWork, LONG lQTY, USHORT usFor,
*                                                       LONG lUnitPrice);
*
*   Input  : LONG   lQTY
*          : USHORT usFor
*          : LONG   lUnitPrice,
*
*   Output : UCHAR      *pszWork       -destination buffer address
*   InOut  : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets  quantity, and unit price
*            to the buffer 'pszWork'.
*  Print format :
*    <Quantity >
*       0        1         2         3         4
*       12345678901234567890123456789012345678901234
*      |^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
*      |                  999999 X            999999|
*      |                                            |
*       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
*    <  FOR  >
*       0        1         2         3         4
*       12345678901234567890123456789012345678901234
*      |^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
*      |                  999999 X         999999/99|
*      |                                            |
*       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
*===========================================================================
*/
USHORT  PrtSPQtyFor(TCHAR *pszWork, LONG lQTY, USHORT usFor, DCURRENCY lUnitPrice)
{
	TCHAR  aszSpecAmt[PARA_SPEMNEMO_LEN +PRT_AMOUNT_LEN + 1] = {0};
    SHORT  sDecPoint;

    if ( (labs( lQTY ) == PLU_BASE_UNIT) && (( usFor == 0 ) || ( usFor == 1)) ){
        return(0);
    }
    PrtSetQtyDecimal( &sDecPoint, &lQTY);

    /* -- adjust native mnemonic and amount sign(+.-) -- */
    PrtAdjustNative(aszSpecAmt, lUnitPrice);

    if (sDecPoint == PRT_NODECIMAL) {
        if (( usFor == 0 ) || ( usFor == 1 )) {
            RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPQty, lQTY, aszSpecAmt);
        } else {
            RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPQtyFor, lQTY, aszSpecAmt, usFor );
        }
        PrtResetQtyDecimal(&sDecPoint, &lQTY);
        return(1);
    } else {
        if (( usFor == 0 ) || ( usFor == 1 )) {
            RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPDecQty, sDecPoint, (DWEIGHT)lQTY, aszSpecAmt);
        } else {
            RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSPDecQtyFor, sDecPoint, (DWEIGHT)lQTY, aszSpecAmt, usFor );
        }
        PrtResetQtyDecimal(&sDecPoint, &lQTY);
        return(1);
    }
    PrtResetQtyDecimal(&sDecPoint, &lQTY);
    return(0);
}

/*
*===========================================================================
** Format  : VOID  PrtSupSPHead(TRANINFORMATION *pTran, VOID *pHeader)
*
*    Input : UCHAR   uchAdr      -address of report name
*          : USHORT  usACNumber  -action code
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints validation header line.
*===========================================================================
*/
VOID  PrtSupSPHead(TRANINFORMATION *pTran, VOID *pHeader)
{
	TCHAR           aszSPPrintBuff[3][PRT_SPCOLUMN + 1] = { 0 }; /* print data save area */
    USHORT          usSlipLine=0;
    USHORT          usSaveLine, i;
    MAINTSPHEADER   *pItem = pHeader;

    /* -- set header lines -- */
    memset(aszSPPrintBuff[0], '\0', sizeof(aszSPPrintBuff));
                                                /* initialize */
    /* -- print action code  -- */
    usSlipLine += PrtSupSPHeadLine(aszSPPrintBuff[0], pItem);

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
** Format  : SHORT  PrtSupSPHeadLine(UCHAR *pszWork, VOID *pHeader)
*
*    Input : UCHAR   uchAdr      -address of report name
*          : USHORT  usACNumber  -action code
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints slip header 1'st line.
*===========================================================================
*/
SHORT   PrtSupSPHeadLine(TCHAR *pszWork, VOID *pHeader)
{
    const TCHAR  *auchNumber   = _T("%3d");
    const TCHAR  *auchIdNumber = _T("%8.8Md");
    PARAREPORTNAME  ParaReportName = { 0 };
    TCHAR           aszDoubRepoName[PARA_REPORTNAME_LEN * 2 + 1] = { 0 };
    TCHAR           aszDoubRepoNumb[8 * 2 + 1] = { 0 };
    TCHAR           aszRepoNumb[8 + 1] = { 0 };
    TCHAR           aszDoubCashNumb[8 * 2 + 1] = { 0 };
	TCHAR           aszCashNumb[8 + 1] = { 0 };
    MAINTSPHEADER   *pItem = pHeader;

    ParaReportName.uchMajorClass = CLASS_PARAREPORTNAME;
    ParaReportName.uchAddress = RPT_ACT_ADR;
    CliParaRead(&ParaReportName);
    memset(aszDoubRepoName, '\0', sizeof(aszDoubRepoName));
    PrtDouble(aszDoubRepoName, TCHARSIZEOF(aszDoubRepoName), ParaReportName.aszMnemonics);

    /* make double wide for report number */
    RflSPrintf(aszRepoNumb, TCHARSIZEOF(aszRepoNumb), auchNumber, pItem->usACNumber);
    memset(aszDoubRepoNumb, '\0', sizeof(aszDoubRepoNumb));
    PrtDouble(aszDoubRepoNumb, TCHARSIZEOF(aszDoubRepoNumb), aszRepoNumb);

    /* make double wide for cashier number */
    RflSPrintf(aszCashNumb, TCHARSIZEOF(aszCashNumb), auchIdNumber, RflTruncateEmployeeNumber(pItem->ulCashierNo));
    memset(aszDoubCashNumb, '\0', sizeof(aszDoubCashNumb));
    PrtDouble(aszDoubCashNumb, TCHARSIZEOF(aszDoubCashNumb), aszCashNumb);

    RflSPrintf(pszWork, PRT_SPCOLUMN+1, aszPrtSupSPHead, aszDoubRepoName, aszDoubRepoNumb, pItem->aszMnemonics, aszDoubCashNumb);
    return(1);
}


/*
*===========================================================================
** Format  : VOID  PrtSupSPTrailer(TRANINFORMATION *pTran, MAINTSPHEADER *pTrail);
*
*    Input : UCHAR   uchAdr      -address of report name
*          : USHORT  usACNumber  -action code
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints slip traliler line.
*===========================================================================
*/
VOID    PrtSupSPTrailer(TRANINFORMATION *pTran, MAINTSPHEADER *pTrail)
{
    TCHAR  aszSPPrintBuff[3][PRT_SPCOLUMN + 1]; /* print data save area */
    USHORT usSetLine = 0;

    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pTrail->usPrintControl);
    if (((fsPrtPrintPort & PRT_SLIP) == 0 ) &&  /* no slip */
        (usPrtSlipPageLine == 0)) {             /* for ask key, R2.0GCA */
        return;
    }

    /* initialize the buffer */
    memset(aszSPPrintBuff[0], '\0', sizeof(aszSPPrintBuff));

    /* -- set trailer 1st line -- */
    usSetLine += PrtSupSPTrail1(aszSPPrintBuff[0], pTrail);

    /* -- set trailer 2nd line -- */
    usSetLine += PrtSupSPTrail2(aszSPPrintBuff[1], pTran, pTrail);

    /* -- print 2 lines -- */
    PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[0], PRT_SPCOLUMN);
    PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[1], PRT_SPCOLUMN);

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSetLine;

    /* -- slip release -- */
    PrtSlpRel();

    usPrtSlipPageLine = 0;
}

/*
*===========================================================================
** Format  : USHORT PrtSupSPTrail1(UCHAR *pszWork, VOID *pItem)
*
*   Input  : TRNINFORMATION *pTran    -transaction information
*
*   Output : UCHAR  *pszWork           -destination buffer address
*   InOut  : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets trailer 1'st line.
*===========================================================================
*/
USHORT PrtSupSPTrail1(TCHAR *pszWork, VOID *pItem)
{
	TCHAR  aszDayRstMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };
	TCHAR  aszPTDRstMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };
    MAINTSPHEADER   *pTrail = pItem;

    /* set EOD reset counter mnemonics */
	RflGetSpecMnem(aszDayRstMnem, SPC_DAIRST_ADR);

    /* set PTD reset counter mnemonics */
	RflGetSpecMnem(aszPTDRstMnem, SPC_PTDRST_ADR);

    /* set print format */
	{
		CONST TCHAR  aszPrtSupSPTrail1[]  = _T("%-4s%04u %-4s%04u\t%8.8Mu");    /* trailer ( eod,ptd,sup# ) */
		RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSupSPTrail1, aszDayRstMnem, pTrail->usDayRstCount, aszPTDRstMnem, pTrail->usPTDRstCount, RflTruncateEmployeeNumber(pTrail->ulSuperNumber));
	}

    return(1);
}

/*
*===========================================================================
** Format  : USHORT PrtSupSPTrail2(UCHAR *pszWork, TRANINFORMATION  *pTran, VOID *pItem)
*
*   Input  : TRNINFORMATION *pTran    -transaction information
*
*   Output : UCHAR  *pszWork           -destination buffer address
*   InOut  : none
*
** Return  : USHORT  usSetLine         -number of lines to be set
*
** Synopsis: This function sets trailer 2'nd line.
*===========================================================================
*/
USHORT  PrtSupSPTrail2(TCHAR *pszWork, TRANINFORMATION  *pTran, VOID *pItem)
{
	TCHAR           aszDate[PRT_DATETIME_LEN + 1] = { 0 };
    MAINTSPHEADER   *pData = pItem;

    /* -- get date and time -- */
    PrtGetDate(aszDate, TCHARSIZEOF(aszDate), pTran);

	{
		CONST TCHAR FARCONST  aszPrtSupSPTrail2[]  = _T("%04u     %04u-%03u\t%16s");  /* trailer ( cons,store-reg,date&time )*/
		RflSPrintf(pszWork, PRT_SPCOLUMN + 1, aszPrtSupSPTrail2, pData->usConsCount, pData->usStoreNumber, pData->usRegNumber, aszDate);
	}

    return(1);
}

/*
*===========================================================================
** Format  : SHORT  PrtSupSlipComp(VOID)
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function initialize current slip's lines,
*            checks slip form in or out, decides print portion.
*===========================================================================
*/
SHORT   PrtSupCompSlip(VOID)
{
//    USHORT usRet;                      /* return value */
//    UCHAR  fbPrtStatus;                /* printer status */
    USHORT usPrevious;

    PmgWait();                         /* waits until spool buffer is empty */

    /* allow power switch at error display */
    usPrevious = PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */

    for (;;) {
        /* -- get printer status -- */
//        usRet = PmgGetStatus(PMG_PRT_SLIP, &fbPrtStatus);

       // if (usRet == PMG_SUCCESS ) {    /* slip printer is online */

            /* -- form in? -- */
        //    if ((fbPrtStatus & PRT_STAT_FORM_IN) && (fbPrtStatus & PRT_STAT_ERROR)) {

                PrtSlpCompul();             /* set slip compulsory */
                PifControlPowerSwitch(usPrevious);  /* V1.0.04 */
                return(SUCCESS);

         //   } else {

             //   if (fsPrtCompul & PRT_SLIP) {      /* compulsory slip */
                    /* allow power switch at error display */
               //     PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */
              //      /* -- require form in -- */
               //     UieErrorMsg(LDT_SLPINS_ADR, UIE_WITHOUT);

               // } else {
               //     PifControlPowerSwitch(usPrevious);  /* V1.0.04 */
               //     return(LDT_PRTTBL_ADR);          /* no slip print */
               // }
           //}

      //  } else {        /* slip printer is offline */
      //      if (fsPrtCompul & PRT_SLIP) {      /* compulsory slip */

                /* allow power switch at error display */
              //  PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */
                /* display "printer trouble"  and wait clear */
               // usRet = UieErrorMsg(LDT_PRTTBL_ADR, UIE_ABORT);

                /* if abort key entered, set receipt compul and exit */
             //   if (usRet == FSC_CANCEL || usRet == FSC_P5) {
              //      PifControlPowerSwitch(usPrevious);  /* V1.0.04 */
              //      return(LDT_PRTTBL_ADR);          /* no slip print */
              //  }

            //} else {
            //    PifControlPowerSwitch(usPrevious);  /* V1.0.04 */
            //    return(LDT_PRTTBL_ADR);          /* no slip print */

           // }
       // }
    }
    PifControlPowerSwitch(usPrevious);  /* V1.0.04 */

}

/*
*===========================================================================
** Format  : VOID   PrtSupRemoveSlip(VOID)
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function initialize current slip's lines,
*            checks slip form in or out, decides print portion.
*===========================================================================
*/
VOID    PrtSupRemoveSlip(VOID)
{
//    UCHAR   fbStatus;                /* get the status of paper change */
    USHORT  usPrevious;

    /* wait until slip printing is finished */
    PmgWait();

    /* allow power switch at error display */
    usPrevious = PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */

    for (;;) {
        /* allow power switch at error display */
        PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */
        /* -- error message "slip change" -- */
        UieErrorMsg(LDT_PAPERCHG_ADR, UIE_WITHOUT);

        /* -- slip printer on ? -- */
//        if (PmgGetStatus(PMG_PRT_SLIP, &fbStatus) == PMG_SUCCESS) {

            /* -- slip paper has been existed yet -- */
          //  if ((fbStatus & PRT_STAT_FORM_IN) && (fbStatus & PRT_STAT_ERROR)) {
                PrtSlpCompul();             /* set slip compulsory */
                usPrtSlipPageLine = 0;      /* clear current slip's pages and lines */
                PifControlPowerSwitch(usPrevious);  /* V1.0.04 */
                return;

         //   } else {
          //      continue;                  /* request change paper repeatly */
          //  }
       // } else {
            /* allow power switch at error display */
          //  PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */
          //  UieErrorMsg(LDT_PRTTBL_ADR, UIE_WITHOUT);
          // break;
       // }
    }

    PifControlPowerSwitch(usPrevious);  /* V1.0.04 */

}

/****** End of Source ******/