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
**        *=*=*=*=*=*=*=*                                                 **
**        *  NCR 2170   *             AT&T GIS Corporation, E&M OISO      **
**     @  *=*=*=*=*=*=*=*  0             (C) Copyright, 1995-9            **
**    <|\/~             ~\/|>                                             **
**   _/^\_               _/^\_                                            **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print common routine, reg mode, thermal                 
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRCmnT_.C
*               | ||  ||
*               | ||  |+- S:21Char L:24Char _:not influcenced by print column
*               | ||  +-- T:thermal J:jounal S:slip K:kitchen 
*               | |+----- comon routine
*               | +------ R:reg mode S:super mode
*               +-------- Pr:print module
* --------------------------------------------------------------------------
* Abstract:
*
*   PrtTHVoid()
*   PrtTHOffTend()
*   PrtTHNumber()
*   PrtTHAmtMnem()
*   PrtTHAmtMnemMnem()
*   PrtTHMnem()
*   PrtTHAmtTaxMnem()
*   PrtTHTaxMod()
*   PrtTHPerDisc()
*#  PrtTHCoupon()       add REL 3.0
*   PrtTHAmtSym()
*#  PrtTHTblPerson()    mod. REL 3.0
*#  PrtTHTblPerGC()     mod. REL 3.0
*#  PrtTHCustomerName() add REL 3.0
*   PrtTHMultiSeatNo()  add at R3.1
*   PrtTHTranNum()
*   PrtTHKitchen()
*   PrtTHWaiAmtMnem()
*   PrtTHWaiGC()
*   PrtTHForeign()
*   PrtTHSeatNo()
*#  PrtTHQty()
*   PrtTHWeight()
*   PrtTHChild()
*   PrtTHItems()
*   PrtTHZeroAmtMnem() 
*   PrtTHDiscItems()
*   PrtTHHead()
*   PrtTHDoubleHead()
*   PrtTHGuest()
*   PrtTHMulChk()
*   PrtTHWaiTaxMod()
*   PrtTHMnemCount()
*   PrtTHTrail1()
*   PrtTHTrail2()
*   PrtTHTickTrail1()
*   PrtTHTickTrail2()
*   PrtTHEtkCode()
*   PrtTHEtkOut()
*   PrtTHEtkTimeIn()
*   PrtTHEtkTimeOut()
*   PrtTHCPRoomCharge()
*   PrtTHCPRspMsgText()
*   PrtTHOffline()
*   PrtTHEPTError()

*   PrtTHForeign1()
*   PrtTHForeign2()
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-14-93 : 01.00.12 :  R.Itoh    : initial                                   
* Jun-29-93 : 01.00.12 :  K.You     : mod. PrtTHQty for US enhance.                                   
* Jul-06-93 : 01.00.12 :  R.Itoh    : add PrtTHEtkCode, PrtTHEtkJobTimeIn.                                   
* Oct-21-93 : 02.00.02 :  K.You     : add charge posting feature.                                   
* Apr-08-94 : 00.00.04 : Yoshiko.J  : add PrtTHOffline(), PrtTHEPTError()
* Apr-24-94 : 00.00.05 :  K.You     : bug fixed E-32 (mod. PrtTHOffTend)
* Apr-24-94 : 00.00.05 :  K.You     : bug fixed E-38 (mod. PrtTHCPRspMsgText)
* Mar-01-95 : 03.00.00 : T.Nakahata : Add coupon feature (PrtTHCoupon) and
*                                     Unique Trans No. (mod PrtTHTblPerson,
*                                     PrtTHTblPerGC)
* May-30-95 : 03.00.00 : T.Nakahata : Add Promotional PLU with Condiment
* Aug-25-95 : 03.00.04 : T.Nakahata : insert TAB between Person# and GC#
* Aug-29-95 : 03.00.06 : T.Nakahata : bug fixed (overflow 1 line mod-disc)
* Nov-15-95 : 03.01.00 : M.Ozawa    : Change ETK format for R3.1
* Aug-08-99 : 03.05.00 : M.Teraki   : Remove WAITER_MODEL
* Aug-12-99 : 03.05.00 : K.Iwata    : Marge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
* Nov-29-99 : 01.00.00 : hrkato     : Saratoga (Money)
*
** GenPOS **
* Jan-11-18 : v2.2.2.0 : R.Chambers  : implementing Alt PLU Mnemonic in Kitchen Printing.
* Feb-19-18 : v2.2.2.0 : R.Chambers  : fix coupon printing and printed receipt with food stamp not showing items.
* Feb-19-18 : v2.2.2.0 : R.Chambers  : fix buffer size used with function PrtGetSISymCoupon().
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
/**------- M S - C -------**/
#include <tchar.h>
#include <stdlib.h>
#include <string.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <regstrct.h>
#include <transact.h>
#include <trans.h>
#include <paraequ.h> 
#include <para.h> 
#include <csstbpar.h>
/* #include<uie.h> */
#include <pmg.h>
#include <rfl.h>
#include <shr.h>
#include <prt.h>
#include <maint.h>
#include <item.h>

#include "prtshr.h"
#include "prtcom.h"
#include "prtrin.h"
#include "prrcolm_.h"

#include "PrtRJ.h"

/**
;========================================================================
;+                    S T A T I C    R A M s
;========================================================================
**/

//SR 420 enhancement, added another string in here so that we add the currency
//symbol to total amounts. So that we can be certified with teh weights and scales
//auditors from NCR
CONST TCHAR FARCONST  aszPrtTHAmtMnem[]     = _T("%s\t %s%l$");     /* trans.mnem, currency symbol, and amount */
CONST TCHAR FARCONST  aszPrtTHAmtMnemGift[] = _T("%s");      /* Gift Receipt JHHJ*/

//SR 144 cwunn added to print quantity, mnemonic, and amount on the same line
CONST TCHAR FARCONST  aszPrtTHAmtMnem1Line[]     = _T("%ld %s $%l$ %s\t %s%l$");   //5 X $1.39 Hamburger $6.95
												                                   //"%-14s      %8ld X\t %l$/%ld " (quantity formatting)
                                                                                   /* trans.mnem and amount */
CONST TCHAR FARCONST  aszPrtTHAmtMnem1LineGift[] = _T("%ld %s %s");   //5 X $1.39 Hamburger $6.95
												                        //"%-14s      %8ld X\t %l$/%ld " (quantity formatting) GIFT PRT_RECEIPT
                                                                        /* trans.mnem and amount */
CONST TCHAR FARCONST  aszPrtTHAmtMnemMnem[] = _T("%-16s   %s\t %s%l$");    /* trans.mnem and amount */

CONST TCHAR FARCONST  aszPrtTHEuroAmtMnem[] = _T("%s\t%s%.*l$");           /* trans.mnem and amount */
/* V3.3 */
CONST TCHAR FARCONST  aszPrtTHTaxRateAmt[] = _T("%-16s %7.2l$%%\t $%l$%*s");  //_T("%-16s %7.2l$%%\t %l$%*s") -- SR 1004, adding $ to format - CSMALL
                                                                              /* trans. mnem, vat rate, and amount */    

CONST TCHAR FARCONST  aszPrtTHVatMnem[] = _T("        %10s  %10s  %10s");     /* VAT mnemonics */

CONST TCHAR FARCONST  aszPrtTHVatAmt[] = _T("%5.2l$%%%12l$%12l$%12l$");       /* VAT amount */

CONST TCHAR FARCONST  aszPrtTHAmtDiscMnem[]     = _T("%s\t %d%% %s%11l$");    //SR 706 Fix for Numerical Data JHHJ, disc. rate and amount */
CONST TCHAR FARCONST  aszPrtTHAmtDiscMnemGift[] = _T("%s\t %d%%");            /* disc. rate and amount */

CONST TCHAR FARCONST  aszPrtTHMnemMnem[] = _T("%s\t %s");                     /* mnem. and mnem. */

CONST TCHAR FARCONST  aszPrtTHAmtTaxMnem[] = _T("%-8.8s   %s\t %l$");         /*8 characters JHHJ, mnem. tax mod. and amount */

CONST TCHAR FARCONST  aszPrtTHWaiAmtMnem[] = _T("%-4s %03d      %s\t %l$");   /* waiter ID, mnem. and amount */

CONST TCHAR FARCONST  aszPrtTHTblPerson[] = _T("%-4s %-3s            %s %s"); /* table and person */

CONST TCHAR FARCONST  aszPrtTHTblPerGC[] = _T("%-4s %-3s   %s %s\t %s %s\n");

CONST TCHAR FARCONST  aszPrtTHTblPerGCNoPerson[] = _T("%-4s %-3s  \t %s %s\n");   /* table, person, and G/C */

CONST TCHAR FARCONST  aszPrtTHTblPerGCUnique[] = _T("%-4s %-3s\n   %s %s\t %s %s");

CONST TCHAR FARCONST  aszPrtTHTblPerGCNoPersonUnique[] = _T("%-4s %-3s\n  \t %s %s");

/*** Add Unique Transaction No. (REL 3.0) ***/
CONST TCHAR FARCONST  aszPrtTHTransPerson[] = _T("%-12s %-8.8s\t%8.8s %s");     /*8 characters JHHJ*/
                                                                                /* unique trans no. */
CONST TCHAR FARCONST  aszPrtTHTransNo[]     = _T("%-12s %-8.8s");               /*8 characters JHHJ, unique trans no. */
CONST TCHAR FARCONST  aszPrtTHCustomerName[] = _T("%-19s");          /* customer name */
/*** Add Unique Transaction No. (REL 3.0) ***/

CONST TCHAR FARCONST  aszPrtTHMultiSeat[] = _T("%-s");               /* seat no. */

CONST TCHAR FARCONST  aszPrtTHMnemNum[] = _T("%-8.8s %lu");          /*8 characters JHHJ, mnemo. and number */

CONST TCHAR FARCONST  aszPrtTHPrinNo[]  = _T("PRT%d");               /* printer no. */

CONST TCHAR FARCONST  aszPrtTHWaiTaxMod[] = _T("%s %03d  %s");       /* waiter mnemo. and ID and tax mod. */

CONST TCHAR FARCONST  aszPrtTHEuro1[]  = _T("%s%ld / %s%.*l$");      /* Euro amount */
CONST TCHAR FARCONST  aszPrtTHSeat[]    = _T("%-4s %1d");            //SR206 _T("%-4s %2d");       /* seat no. */

CONST TCHAR FARCONST  aszPrtTHQuant[]     = _T("%-14s       %8ld %s\t %l$ ");    /* qty and unit price */
CONST TCHAR FARCONST  aszPrtTHQuantGift[] = _T("%-14s       %8ld %s");           //Gift Receipt JHHJ, qty and unit price */

CONST TCHAR FARCONST  aszPrtTHPPIQuant[]     = _T("%-14s      %8ld %s\t %l$/%ld ");
CONST TCHAR FARCONST  aszPrtTHPPIQuantGift[] = _T("%-14s      %8ld %s\t %ld "); //Gift Receipt JHHJ, qty, product and qty */

//SR 420 added a unique format string for the manual mnemonic.  We do this
//so that when we are just printing a non manual weighted item, the information
//that is printed out is less confusing than previously before.
CONST TCHAR FARCONST  aszPrtTHWeight1Manual[] = _T("\n%-14s      %8.*l$ %s\t %s ");    /* Weight and Manual Mnem. */

CONST TCHAR FARCONST  aszPrtTHWeight1[] = _T("\n%-14s\t%s  %8.*l$ %s ");               /* Weight and Manual Mnem. */

CONST TCHAR FARCONST  aszPrtTHWeight2[] = _T("\t%s %l$/%d %s ");                       /* scale sym. and unit price */

CONST TCHAR FARCONST  aszPrtTHChild[]   = _T(" %4s %s");                /* child plu */

CONST TCHAR FARCONST  aszPrtTHModLinkPLU1[]   = _T("%4s %s\t%l$ ");
CONST TCHAR FARCONST  aszPrtTHModLinkPLU2[]   = _T("%s\t%l$ ");
CONST TCHAR FARCONST  aszPrtTHLinkPLU1[]   = _T("%4s %s\t%l$");
CONST TCHAR FARCONST  aszPrtTHLinkPLU2[]   = _T("%s\t%l$");             /* link plu */


CONST TCHAR FARCONST  aszPrtTHWaiGC_WTCD[] = _T("%-8.8s  %03d       %s %4d%02d");   /* old waiter No. and G/C No. with C/D */

CONST TCHAR FARCONST  aszPrtTHWaiGC_WOCD[] = _T("%-8s  %03d       %s %4d");         /* old waiter No. and G/C No. with C/D */

CONST TCHAR FARCONST  aszPrtTHGC_WTCD_S[] = _T("                    %s  %4d%02d");  /* G/C No. w/ CD (single wide) */

CONST TCHAR FARCONST  aszPrtTHGC_WOCD_S[] = _T("                    %s  %4d");      /* G/C No. w/o CD (single wide) */

CONST TCHAR FARCONST  aszPrtTHGuest_D[] = _T("%-16s     %s");             /* G/C No. (double wide) */

CONST TCHAR FARCONST  *aszPrtTHTickTrail1  = _T("%-8s %s   %4s %s\t %s"); /* ticket trailer 1st line with CD */

CONST TCHAR FARCONST  aszPrtTHMnemCount[] = _T("%-8s\t%d");               /* mnemo. and counter */

CONST TCHAR FARCONST  aszPrtTHCasOrdr[] = _T("%-4s %03d    %s %2d");      /* cashier No. and order No. */

CONST TCHAR FARCONST  aszPrtTHAmtMnemSpace[] = _T("%s\t %l$ ");           /* trans.mnem and amount */

CONST TCHAR FARCONST  aszPrtTHItemCount[]  = _T("%-4.20s:  %2d");        //SR 217 JHHJ Item Count Option, trailer item count */

CONST TCHAR FARCONST  aszPrtTHItemCountSP[]  = _T("%-4.20s:  %2d");      //SR 217 JHHJ Item Count Option

//SR 805 weights and measures fix for price on 1 qty line
CONST TCHAR FARCONST  aszPrtTHAmtMnemSpaceWeight[]    = _T("%s\t $%l$ ");             /* trans.mnem and amount */
//SR 144 cwunn added to print quantity, mnemonic, and amount on the same line
CONST TCHAR FARCONST  aszPrtTHAmtMnemSpace1Line[]     = _T("%ld %s $%l$ %s\t $%l$ ");  /* trans.mnem and amount */
CONST TCHAR FARCONST  aszPrtTHAmtMnemSpace1LineGift[] = _T("%ld %s $%l$ %s");          /* trans.mnem and amount */

CONST TCHAR FARCONST  aszPrtTHAmtS[]    = _T("%l$");  /* amount */
CONST TCHAR FARCONST  aszPrtTHAmtGift[] = _T("");     //Gift Receipt JHHJ

CONST TCHAR FARCONST  aszPrtTHCPRoomCharge[] = _T("%-4s%6s\t %-4s%3s");            /* room charge */

CONST TCHAR FARCONST  aszPrtTHOffline[] = _T("%-4s   %5s   %6s");      /* mnemo, date, approval code */

CONST TCHAR FARCONST  aszPrtTHOffDate[] = _T("%02d/%02d");             /* expiration date */

CONST TCHAR FARCONST  aszPrtTHEPTError[] = _T("%c %d");                /* mnemo. and error code */
CONST TCHAR FARCONST  aszPrtTHQty[]      = _T("\t%3ld %s %s ");        /* qty, unit price, Saratoga */
CONST TCHAR FARCONST  aszPrtTHMoneyForeign[] = _T("%.*l$");
CONST TCHAR FARCONST  aszPrtTHDecQty[]    = _T("\t%6.*l$ %s %s ");     /* qty, unit price */

CONST TCHAR FARCONST  aszPrtTHAmtDiscMnemSI[]     = _T("%s %s\t %d%% %11l$");    /* disc. rate and amount */
CONST TCHAR FARCONST  aszPrtTHAmtDiscMnemSIGift[] = _T("%s %s\t %d%%");          /* disc. rate and amount */
CONST TCHAR FARCONST  aszPrtTHTent[] = _T("%s %s");

extern CONST TCHAR FARCONST  aszEtkCode[];
extern CONST TCHAR FARCONST aszPrtAmtMnemShift[];/* trans.mnem and amount */

TCHAR *PrtConcatMnemonic (TCHAR *tcsDest, TCHAR *tcsFill, TCHAR *tcsSource, int nMaxCharCount)
{
	if (tcsFill) _tcsncat (tcsDest, tcsFill, nMaxCharCount);
	if (tcsSource) _tcsncat (tcsDest, tcsSource, nMaxCharCount);

	return tcsDest;
}

/**
;========================================================================
;+             P R O G R A M    D E C L A R A T I O N s
;========================================================================
**/
/*
*===========================================================================
** Format  : VOID  PrtTHVoid(CHAR fbMod);
*               
*    Input : CHAR   fbMod                -Void status      
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints "void" line with double wide chara.
*            if void status is off, does not print.
*===========================================================================
*/
VOID PrtTHVoid(USHORT  fbMod, USHORT usReasonCode)
{
	BOOL    bDoPrint = FALSE;

	if (ItemCheckCustomerSettingsFlags (SYSCONFIG_CUSTOMER_ENABLE_AMTRAK)) {
		// Amtrak does not want to print reason mnemonics on the customer receipt for returned items.
		bDoPrint = ((fbMod & (VOID_MODIFIER | RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) == VOID_MODIFIER);
	} else {
		// The default is to print reason mnemonics for returned items on the customer receipt for VCS and others.
		bDoPrint = ((fbMod & (VOID_MODIFIER | RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) != 0);
	}

    if (bDoPrint) {
		TCHAR    aszWork[PRT_THCOLUMN + 1] = {0};             /* print work area */

		if (0 > PrtGetReturns(fbMod, usReasonCode, aszWork, TCHARSIZEOF(aszWork))) {    
			/* -- get void mnemonics instead of returns mnemonic -- */
			PrtGetVoid(aszWork, TCHARSIZEOF(aszWork));
		}

        /* -- print thermal --*/
        PrtPrint(PMG_PRT_RECEIPT, aszWork, usPrtTHColumn);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtTHOffTend(CHAR fbMod);
*               
*    Input : CHAR   fbMod                -Void status      
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints "off line" line with double wide chara.
*            if status is off, does not print.
*===========================================================================
*/
VOID PrtTHOffTend(USHORT  fbMod)
{
    if (fbMod & OFFCPTEND_MODIF) {
		TCHAR    aszWork[PRT_THCOLUMN + 1] = {0};             /* print work area */

        /* -- get off line tender mnemonics -- */
        PrtGetOffTend(fbMod, aszWork, TCHARSIZEOF(aszWork));

        /* -- print thermal --*/
        PrtPrint(PMG_PRT_RECEIPT, aszWork, usPrtTHColumn);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtTHNumber(UCHAR  *pszNumber);
*               
*    Input : UCHAR *pszNumber                -number   
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints "number" line.
*            if "number = 0", does not print.
*===========================================================================
*/
VOID  PrtTHNumber(TCHAR  *pszNumber)
{
    if (*pszNumber != 0) {
		TCHAR   aszNumLine[PRT_THCOLUMN * 2 + 1] = {0};

		PrtConcatMnemonic (RflGetTranMnem (aszNumLine, TRN_NUM_ADR), _T(" "), pszNumber, PRT_THCOLUMN * 2);

        /* -- send number mnemoncis and number -- */
        PrtPrint(PMG_PRT_RECEIPT, aszNumLine, tcharlen(aszNumLine)); /* ### Mod (2171 for Win32) V1.0 */
    }
}

/*
*===========================================================================
** Format  : VOID  PrtTHNumber(UCHAR  *pszNumber);
*               
*    Input : UCHAR *pszNumber                -number   
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints "number" line.
*            if "number = 0", does not print.
*===========================================================================
*/
VOID  PrtTHMnemNumber(USHORT usAdr, TCHAR  *pszNumber)
{
    if (*pszNumber != 0) {
		TCHAR   aszNumLine[PRT_THCOLUMN * 2 + 1];
        
        if (usAdr == 0) usAdr = TRN_NUM_ADR;

		PrtConcatMnemonic (RflGetTranMnem (aszNumLine, usAdr), _T(" "), pszNumber, PRT_THCOLUMN * 2);

        /* -- send number mnemoncis and number -- */
        PrtPrint(PMG_PRT_RECEIPT, aszNumLine, tcharlen(aszNumLine)); /* ### Mod (2171 for Win32) V1.0 */
    }
}

/*
*===========================================================================
** Format  : VOID  PrtTHAmtMnem(UCHAR uchAdr, LONG lAmout);
*               
*    Input : UCHAR uchAdr          -Transacion mnemonics address
*            LONG  lAmount         -Amount 
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints prints transaction mnemonics
*            and Amount line for an ITEMTENDER type of item.
*            It has several different uses and prints the following types of lines:
*               Credit                     $2.00  (usAdr1 only)
*               Transaction Type: Sale     $2.00  (usAdr1 and usAdr2)
*               Credit M/C7891             $2.00  (usAdr1 and pCardLabel)
*
*            See also MldMnemAmt() used for display in receipt window.
*===========================================================================
*/
VOID  PrtTHAmtTwoMnem(USHORT usAdr1, USHORT usAdr2, DCURRENCY lAmount, TCHAR *pCardLabel)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN * 2 + 5] = {0};  /* PARA_... defined in "paraequ.h" */
	TCHAR  aszTranMnem2[PARA_TRANSMNEMO_LEN + 1] = {0};
	TCHAR  aszSpecMnem[STD_SPEMNEMO_LEN + 1] = {0}; /* PARA_... defined in "paraequ.h" */

	if ((usAdr1 & STD_FORMATMASK_INDENT_4) != 0) {
		/* -- get transaction  mnemonics -- */
		aszTranMnem [0] = aszTranMnem [1] = aszTranMnem [2] = aszTranMnem [3] = _T(' ');
		RflGetTranMnem(aszTranMnem + 4, usAdr1);
	} else {
		/* -- get transaction  mnemonics -- */
		RflGetTranMnem(aszTranMnem, usAdr1);
	}

	if (usAdr2) {
		/* -- get transaction  mnemonics -- */
		RflGetTranMnem(aszTranMnem2, usAdr2);
		_tcscat (aszTranMnem, aszTranMnem2);
	} else if (pCardLabel && *pCardLabel) {
		_tcscat (aszTranMnem, _T(" ("));
		_tcsncat (aszTranMnem, pCardLabel, PARA_TRANSMNEMO_LEN - 6);
		_tcscat (aszTranMnem, _T(")"));
	}

	RflGetSpecMnem(aszSpecMnem, SPC_CNSYMNTV_ADR);

    /* -- send mnemoncis and amount -- */
	if((TrnInformation.TranCurQual.fsCurStatus2 & PRT_GIFT_RECEIPT) != 0 || (usAdr1 & STD_FORMATMASK_NO_AMOUNT) != 0)
	{
		//SR 525, Gift Receipt functionality JHHJ 9-01-05
		PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHAmtMnemGift, aszTranMnem);
	} else
	{
		// see also function PrtAdjustNative() with its print format.
		PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHAmtMnem, aszTranMnem, aszSpecMnem, lAmount);
	}
}

VOID  PrtTHAmtMnem(USHORT usAdr, DCURRENCY lAmount)
{
	PrtTHAmtTwoMnem(usAdr, 0, lAmount, 0);
}


/*
*===========================================================================
** Format  : VOID  PrtTHMAmtShift(UCHAR uchAdr, LONG lAmout,  USHOT usColumn);
*               
*    Input : UCHAR uchAdr          -Transacion mnemonics address
*            LONG  lAmount         -Amount 
*            USHORT usColumn       -shift column
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints prints transaction mnemonics
*            and Amount line. V3.3
*===========================================================================
*/
VOID  PrtTHMAmtShift(USHORT usTranAdr, DCURRENCY lAmount, USHORT usColumn)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};
    TCHAR  uchNull = 0;

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usTranAdr);

    /* -- send mnemoncis and amount -- */
    PrtPrintf(PMG_PRT_RECEIPT, aszPrtAmtMnemShift, aszTranMnem, lAmount, usColumn, &uchNull);
}

/*
*===========================================================================
** Format  : VOID  PrtTHAmtMnemMnem(UCHAR uchAdr1, UCHAR uchAdr2, LONG lAmout);
*               
*    Input : UCHAR uchAdr1         -Transacion mnemonics address
*            UCHAR uchAdr2         -Transacion mnemonics address
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
VOID  PrtTHAmtMnemMnem(USHORT usTranAdr1, USHORT usTranAdr2, DCURRENCY lAmount)
{
	TCHAR  aszTrcnMnem[PARA_TRANSMNEMO_LEN + 1] = {0};  /* PARA_... defined in "paraequ.h" */
    TCHAR  aszMnemD[PARA_TRANSMNEMO_LEN * 2 + 1] = {0}; /* PARA_... defined in "paraequ.h" */
    TCHAR  aszCnclMnem[PARA_TRANSMNEMO_LEN + 1] = {0};  /* PARA_... defined in "paraequ.h" */
	TCHAR  aszSpecMnem[STD_SPEMNEMO_LEN + 1] = {0}; /* PARA_... defined in "paraequ.h" */

	RflGetSpecMnem(aszSpecMnem, SPC_CNSYMNTV_ADR);
	RflGetTranMnem(aszTrcnMnem, usTranAdr1);
    PrtDouble(aszMnemD, (PARA_TRANSMNEMO_LEN * 2 + 1), aszTrcnMnem); /* -- convert to double -- */

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszCnclMnem, usTranAdr2);

    /* -- send mnemoncis and amount -- */
    PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHAmtMnemMnem, aszMnemD, aszCnclMnem, aszSpecMnem, lAmount);
}

/*
*===========================================================================
** Format  : VOID  PrtTHAmtSymEuro(UCHAR uchAdr1, UCHAR uchAdr2, LONG lAmount, UCHAR fbStatus)
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
VOID  PrtTHAmtSymEuro(USHORT usTranAdr1, UCHAR uchAdr2, DCURRENCY lAmount, UCHAR fbStatus)
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
	RflGetTranMnem(aszTranMnem, usTranAdr1);

    /* -- adjust native mnemonic and amount sign(+.-) -- */
    if (lAmount < 0) {
        aszFMnem[0] = '-';
        /* -- get foreign symbol -- */
		RflGetSpecMnem(&aszFMnem[1], uchAdr2);
    } else {
        /* -- get foreign symbol -- */
		RflGetSpecMnem(aszFMnem, uchAdr2);
    }

    PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHEuroAmtMnem, aszTranMnem, aszFMnem, sDecPoint, RflLLabs(lAmount));
}

/*
*===========================================================================
** Format  : VOID  PrtTHMnem(UCHAR uchAdr, BOOL fsType);
*               
*   Input  : UCHAR uchAdr          -Transacion mnemonics address
*            SHORT fsType          -characvter type
*                                       PRT_SINGLE: single character
*                                       PRT_DOUBLE: double character
*   Output : none
*   InOut  : none
*
** Return  : none
*
** Synopsis: This function prints transaction mnemonics  line.
*===========================================================================
*/
VOID  PrtTHMnem(USHORT usAdr, BOOL fsType)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0}; /* PARA_... defined in "paraequ.h" */
    TCHAR  aszPrintBuff[PRT_THCOLUMN + 1] = {0};

	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- double wide chara? -- */
    if (fsType == PRT_DOUBLE) {
        /* -- convert single to double -- */
        PrtDouble(aszPrintBuff, (PRT_THCOLUMN + 1), aszTranMnem);
    } else {
        _tcsncpy(aszPrintBuff, aszTranMnem, PARA_TRANSMNEMO_LEN);
    }

    /* -- send mnemoncis -- */
    PrtPrint(PMG_PRT_RECEIPT, aszPrintBuff, tcharlen(aszPrintBuff)); /* ### Mod (2171 for Win32) V1.0 */
}

/*
*===========================================================================
** Format  : VOID  PrtTHAmtTaxMnem(UCHAR uchAdr, USHORT fsTax, UCHAR  fbMod, 
*                                       LONG lAmount);
*               
*   Input  : UCHAR uchAdr          -Transacion mnemonics address
*            USHORT fsTax,        -US or Canadian Tax
*            UCHAR  fbMod         -modifier status
*            LONG  lAmount         -Amount
*   Output : none
*   InOut  : none
*
** Return  : none
*
** Synopsis: This function prints transaction mnemonics  line.
*===========================================================================
*/
VOID  PrtTHAmtTaxMnem(USHORT usAdr, USHORT fsTax, USHORT  fbMod, DCURRENCY lAmount)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0}; /* PARA_... defined in "paraequ.h" */
    TCHAR  aszTaxMod[PRT_THCOLUMN + 1] = {0};

	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- get tax modifier mnemonics -- */
    if ( PRT_WITHOUTDATA == PrtGetTaxMod(aszTaxMod, TCHARSIZEOF(aszTaxMod), fsTax, fbMod) ) {
        *aszTaxMod = '\0';
    }

    /* -- send mnemoncis -- */
    PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHAmtTaxMnem, aszTranMnem, aszTaxMod, lAmount);
}

/*
*===========================================================================
** Format  : VOID  PrtTHTaxMod(USHORT fsTax, UCHAR  fbMod);
*               
*   Input  : USHORT fsTax,        -US or Canadian Tax
*            UCHAR  fbMod         -modifier status
*   Output : none
*   InOut  : none
*
** Return  : none
*
** Synopsis: This function prints Tax Modifier line.
*===========================================================================
*/
VOID  PrtTHTaxMod(USHORT fsTax, USHORT  fbMod)
{
    TCHAR  aszTaxMod[PRT_THCOLUMN + 1];

    /* -- get tax modifier mnemonics -- */
    if ( PRT_WITHDATA == PrtGetTaxMod(aszTaxMod, TCHARSIZEOF(aszTaxMod), fsTax, fbMod) ) {
          PrtPrint(PMG_PRT_RECEIPT, aszTaxMod, tcharlen(aszTaxMod));  /* ### Mod (2171 for Win32) V1.0 */
    }
}

/*
*===========================================================================
** Format  : VOID  PrtTHTaxRateAmt(UCHAR uchAdr, LONG lRate, LONG lAmout,  USHOT usColumn);
*               
*    Input : UCHAR uchAdr          -Transacion mnemonics address
*            LONG  lRate           -tax rate   
*            LONG  lAmount         -Amount 
*            USHORT usColumn       -shift column
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints prints transaction mnemonics, rate
*            and Amount line. V3.3
*===========================================================================
*/
VOID  PrtTHTaxRateAmt(USHORT usTranAdr, LONG lRate, DCURRENCY lAmount,  USHORT usColumn)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};
    TCHAR  uchNull = 0;

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usTranAdr);

    /* -- send mnemoncis and amount -- */
    PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHTaxRateAmt, aszTranMnem, lRate / 100, lAmount, usColumn, &uchNull);
}

/*
*===========================================================================
** Format  : VOID  PrtTHVatMnem(ITEMAFFECTION  *pItem);
*
*   Input  : ITEMSALES *pItem     -item data address  
*
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints vat appl.mnem, vat mnem., total mnem..
*===========================================================================
*/
VOID  PrtTHVatMnem(ITEMAFFECTION  *pItem)
{
    TCHAR   aszVATMnem[3][PARA_TRANSMNEMO_LEN + 1];
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

        return;
    }

	RflGetTranMnem(aszVATMnem[0], TRN_TXBL1_ADR);
	RflGetTranMnem(aszVATMnem[1], TRN_TX1_ADR);
	RflGetTranMnem(aszVATMnem[2], TRN_AMTTL_ADR);

    PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHVatMnem, aszVATMnem[0], aszVATMnem[1], aszVATMnem[2]);
                                           
}

/*
*===========================================================================
** Format  : VOID  PrtTHVatAmt(ITEMVAT  ItemVAT);
*
*   Input  : ITEMVAT  ItemVAT     -vat data
*
*   Output : UCHAR  *pszWork      -destination buffer address
*   InOut  : none
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints the vat rate, vat net amount, vat amount,
*                                             vat applied total.
*===========================================================================
*/
VOID  PrtTHVatAmt(ITEMVAT  ItemVAT)
{
    if (ItemVAT.lVATRate == 0L) {
        return;
    }

    if (ItemVAT.lVATable == 0L && ItemVAT.lVATAmt == 0L && ItemVAT.lSum  == 0L) {
        return;
    }

    /* reason of "100L" is  lVATRate has 4digits under  decimal point */
    PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHVatAmt, (DCURRENCY)(ItemVAT.lVATRate/100L), ItemVAT.lVATable,  ItemVAT.lVATAmt, ItemVAT.lSum);
}


/*
*===========================================================================
** Format  : VOID  PrtTHPerDisc(UCHAR uchAdr, UCHAR uchRate, LONG lAmount);
*               
*   Input  : UCHAR  uchAdr,             -Transaction mnemonics address
*            UCHAR  uchRate,            -rate
*            LONG   lAmount             -amount
*   Output : none
*   InOut  : none
*
** Return  : none
*
** Synopsis: This function prints % discount line.
*===========================================================================
*/
VOID  PrtTHPerDisc(USHORT usAdr, UCHAR uchRate, DCURRENCY lAmount)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0}; /* PARA_... defined in "paraequ.h" */
	TCHAR  aszSpecMnem[STD_SPEMNEMO_LEN + 1] = {0}; /* PARA_... defined in "paraequ.h" */

	RflGetSpecMnem(aszSpecMnem, SPC_CNSYMNTV_ADR);
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- send mnemoncis and amount -- */
    if (uchRate == 0) {
        /* not print % */
        if(TrnInformation.TranCurQual.fsCurStatus2 & PRT_GIFT_RECEIPT)
			//SR 525, Gift Receipt functionality JHHJ 9-01-05
		{
			PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHAmtMnemGift, aszTranMnem);
		} else
		{
			// see also function PrtAdjustNative() with its print format.
			PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHAmtMnem, aszTranMnem, aszSpecMnem, lAmount);
		}
    } else {
        if(TrnInformation.TranCurQual.fsCurStatus2 & PRT_GIFT_RECEIPT)
			//SR 525, Gift Receipt functionality JHHJ 9-01-05
		{
			PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHAmtDiscMnemGift, aszTranMnem, uchRate);
		} else
		{
			PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHAmtDiscMnem, aszTranMnem, uchRate, aszSpecMnem, lAmount);
		}
    }
}

/*
*===========================================================================
** Format  : VOID  PrtTHPerDisc(UCHAR uchAdr, UCHAR uchRate, LONG lAmount);
*               
*   Input  : UCHAR  uchAdr,             -Transaction mnemonics address
*            UCHAR  uchRate,            -rate
*            LONG   lAmount             -amount
*   Output : none
*   InOut  : none
*
** Return  : none
*
** Synopsis: This function prints % discount line. 21RFC05437
*===========================================================================
*/
VOID  PrtTHPerDiscSISym(USHORT usAdr, UCHAR uchRate, DCURRENCY lAmount, TCHAR *pszSISym, UCHAR uchMDC)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};
    
    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- send mnemoncis and amount -- */
    if (uchRate == 0) {
		CONST TCHAR *aszPrtTHAmtMnemSI     = _T("%s %s\t %l$");   /* trans.mnem and amount, 21RFC05437 */
		CONST TCHAR *aszPrtTHAmtMnemSIGift = _T("%s %s");         /* trans.mnem and amount, 21RFC05437 */

        /* not print % */
        if (uchMDC) {
			/* si before mnemonics */
        	if(TrnInformation.TranCurQual.fsCurStatus2 & PRT_GIFT_RECEIPT)
			{
				//SR 525, Gift Receipt functionality JHHJ 9-01-05
				PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHAmtMnemSIGift, pszSISym, aszTranMnem);
			} else
			{
				PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHAmtMnemSI, pszSISym, aszTranMnem, lAmount);
			}
	    } else {
			if(TrnInformation.TranCurQual.fsCurStatus2 & PRT_GIFT_RECEIPT)
			{
				//SR 525, Gift Receipt functionality JHHJ 9-01-05
				PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHAmtMnemSIGift, aszTranMnem, pszSISym);
			} else
			{
				PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHAmtMnemSI, aszTranMnem, pszSISym, lAmount);
			}
	    }
    } else {
        if (uchMDC) {
			/* si before mnemonics */
	  		if(TrnInformation.TranCurQual.fsCurStatus2 & PRT_GIFT_RECEIPT)
			{
				//SR 525, Gift Receipt functionality JHHJ 9-01-05
				PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHAmtDiscMnemSIGift, pszSISym, aszTranMnem, uchRate);
			} else
			{
				PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHAmtDiscMnemSI, pszSISym, aszTranMnem, uchRate, lAmount);
			}
	    } else {
			if(TrnInformation.TranCurQual.fsCurStatus2 & PRT_GIFT_RECEIPT)
			{
				//SR 525, Gift Receipt functionality JHHJ 9-01-05
				PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHAmtDiscMnemSIGift, aszTranMnem, pszSISym, uchRate);
			} else
			{
				PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHAmtDiscMnemSI, aszTranMnem, pszSISym, uchRate, lAmount);
			}
	    }
    }
}

/*
*===========================================================================
** Format  : VOID  PrtTHCoupon( ITEMCOUPON *pItem )
*
*   Input  : ITEMCOUPON *pItem  -   coupon item address
*   Output : none
*   InOut  : none
*
** Return  : none
*
** Synopsis: This function prints coupon item.
*===========================================================================
*/
VOID  PrtTHCoupon( ITEMCOUPON *pItem )
{
	TCHAR  aszSpecMnem2[STD_SPEMNEMO_LEN + 1] = {0}; /* PARA_... defined in "paraequ.h" */

	RflGetSpecMnem(aszSpecMnem2, SPC_CNSYMNTV_ADR);

	/* SI symnbol, before mnemonics 21RFC05437 */
	if ( CliParaMDCCheck(MDC_TAX2_ADR, EVEN_MDC_BIT1) ) {
		CONST TCHAR *aszPrtTHAmtMnemSI     = _T("%s %s\t%s %l$"); /* trans.mnem, amount, currency symbol, 21RFC05437 */
		CONST TCHAR *aszPrtTHAmtMnemSIGift = _T("%s %s");         /* trans.mnem and amount, 21RFC05437 */
		TCHAR  aszSpecMnem[PARA_SPEMNEMO_LEN * 2 + 1] = {0};  /*  times 2 for possible double in PrtGetSISymCoupon() */

	    PrtGetSISymCoupon(aszSpecMnem, pItem);
		if (CliParaMDCCheck(MDC_TAX2_ADR, EVEN_MDC_BIT2) ) {
	    	/* -- send mnemoncis and amount -- */
			if(TrnInformation.TranCurQual.fsCurStatus2 & PRT_GIFT_RECEIPT)
			{
				//SR 525, Gift Receipt functionality JHHJ 9-01-05
				PrtPrintf( PMG_PRT_RECEIPT, aszPrtTHAmtMnemSIGift, aszSpecMnem, pItem->aszMnemonic);
			} else
			{
				PrtPrintf( PMG_PRT_RECEIPT, aszPrtTHAmtMnemSI, aszSpecMnem, pItem->aszMnemonic, aszSpecMnem2, pItem->lAmount );
			}
    	} else {
		    /* -- send mnemoncis and amount -- */
			if(TrnInformation.TranCurQual.fsCurStatus2 & PRT_GIFT_RECEIPT)
			{
				//SR 525, Gift Receipt functionality JHHJ 9-01-05
				PrtPrintf( PMG_PRT_RECEIPT, aszPrtTHAmtMnemSIGift, pItem->aszMnemonic, aszSpecMnem);
			} else
			{
				PrtPrintf( PMG_PRT_RECEIPT, aszPrtTHAmtMnemSI, pItem->aszMnemonic, aszSpecMnem, aszSpecMnem2, pItem->lAmount );
			}
		}
	} else {
	    /* -- send mnemoncis and amount -- */
		if(TrnInformation.TranCurQual.fsCurStatus2 & PRT_GIFT_RECEIPT)
		{
			//SR 525, Gift Receipt functionality JHHJ 9-01-05
			PrtPrintf( PMG_PRT_RECEIPT, aszPrtTHAmtMnemGift, pItem->aszMnemonic);
		} else
		{
			PrtPrintf( PMG_PRT_RECEIPT, aszPrtTHAmtMnem, pItem->aszMnemonic, aszSpecMnem2, pItem->lAmount );
		}
	}
}

/*
*===========================================================================
** Format  : VOID  PrtTHAmtSym(UCHAR uchAdr, LONG lAmount, BOOL fsType);
*               
*   Input  : UCHAR  uchAdr,             -Transaction mnemonics address
*            LONG   lAmount             -amount
*            SHORT  fsType              -character type
*                                        PRT_SINGLE: single character
*                                        PRT_DOUBLE: double character
*   Output : none
*   InOut  : none
*
** Return  : none
*
** Synopsis: This function prints transaction mnemonic, native mnemonic  line.
*===========================================================================
*/
VOID  PrtTHAmtSym(USHORT usAdr, DCURRENCY lAmount, BOOL fsType)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};                       /* PARA_... defined in "paraequ.h" */
    TCHAR  aszSpecAmtS[PARA_SPEMNEMO_LEN +PRT_AMOUNT_LEN + 1] = {0};         /* spec. mnem & amount save area */
    TCHAR  aszSpecAmtD[(PARA_SPEMNEMO_LEN + PRT_AMOUNT_LEN) * 2  +1] = {0};  /* spec. mnem. & amount double wide */
    USHORT usMnemLen, usAmtSLen, usAmtDLen;     

    /* -- get transaction mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- adjust native mnemonic and amount sign(+.-) -- */
    PrtAdjustNative(aszSpecAmtS, lAmount);

    /* -- convert to double -- */
    PrtDouble(aszSpecAmtD, ((PARA_SPEMNEMO_LEN + PRT_AMOUNT_LEN) * 2  +1), aszSpecAmtS);

    usMnemLen = tcharlen(aszTranMnem);
    usAmtSLen = tcharlen(aszSpecAmtS);
    usAmtDLen = tcharlen(aszSpecAmtD);

    if ( fsType == PRT_SINGLE )  {

		if(TrnInformation.TranCurQual.fsCurStatus2 & PRT_GIFT_RECEIPT)
		{
			//SR 525, Gift Receipt functionality JHHJ 9-01-05
			PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHMnemMnem, aszTranMnem, _T(" "));
		} else
		{
			PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHMnemMnem, aszTranMnem, aszSpecAmtS);
		}
        return;
    }


    if ( usMnemLen+usAmtDLen+1 <= usPrtTHColumn ) {
		if(TrnInformation.TranCurQual.fsCurStatus2 & PRT_GIFT_RECEIPT)
		{
			//SR 525, Gift Receipt functionality JHHJ 9-01-05
			PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHMnemMnem, aszTranMnem, _T(" "));
		} else
		{
			PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHMnemMnem, aszTranMnem, aszSpecAmtD);
		}
        /* -- double print -- */
        return;
    }

    if ( usMnemLen+usAmtSLen+1 <= usPrtTHColumn ) {
		/* -- single print -- */
		if(TrnInformation.TranCurQual.fsCurStatus2 & PRT_GIFT_RECEIPT)
		{
			//SR 525, Gift Receipt functionality JHHJ 9-01-05
			PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHMnemMnem, aszTranMnem, _T(" "));
		} else
		{
			PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHMnemMnem, aszTranMnem, aszSpecAmtS);
		}
    } else {
        if ( usAmtDLen <= usPrtTHColumn ) {
            /* -- double print -- */
			if(TrnInformation.TranCurQual.fsCurStatus2 & PRT_GIFT_RECEIPT)
			{
				//SR 525, Gift Receipt functionality JHHJ 9-01-05
				PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHMnemMnem, aszTranMnem, _T(" "));
			} else
			{
				PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHMnemMnem, aszTranMnem, aszSpecAmtD);
			}
        } else {
            /* -- single print -- */
			if(TrnInformation.TranCurQual.fsCurStatus2 & PRT_GIFT_RECEIPT)
			{
				//SR 525, Gift Receipt functionality JHHJ 9-01-05
				PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHMnemMnem, aszTranMnem, _T(" "));
			} else
			{
				PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHMnemMnem, aszTranMnem, aszSpecAmtS);
			}
        }
    }
}

/*
*===========================================================================
** Format   : VOID PrtTHTblPerson( USHORT usTblNo,
*                                  USHORT usNoPerson,
*                                  SHORT  sWidthType );
*
*   Input   : USHORT usTblNo    - Table Number or Unique Transaction No.
*             USHORT usNoPerson - No. of Person
*             SHORT  sWidthType - Print Width Type
*                                   PRT_DOUBLE, PRT_SINGLE
*   Output  : none
*   InOut   : none
** Return   : none
*
** Synopsis : This function prints table no., no. of person line.
*           If both TableNo and NoofPerson are "0", function does not work.
*===========================================================================
*/
VOID PrtTHTblPerson( USHORT usTblNo, USHORT usNoPerson, SHORT sWidthType )
{
    TCHAR   aszTranMnem[ PARA_TRANSMNEMO_LEN + 1 ] = {0}; /* PARA_... defined in "paraequ.h" */
    TCHAR   aszNoPer[ PRT_ID_LEN + 1 ] = {0};

    /* --- set up special mnemonic of table no., and table no. --- */
	if ( (usNoPerson != 0 ) && (!CliParaMDCCheck(MDC_TRANNUM_ADR, EVEN_MDC_BIT3))) 
	{		
		_itot( usNoPerson, aszNoPer, 10 );
		RflGetTranMnem( aszTranMnem, TRN_PSN_ADR );
	}

	if ( (usTblNo || usNoPerson) && aszTranMnem[0] != 0 ) {
		TCHAR   aszTransNo[ PRT_ID_LEN * 2 + 1 ] = {0};
		TCHAR   aszSpecMnem[ PARA_SPEMNEMO_LEN + 1 ] = {0};   /* PARA_... defined in "paraequ.h" */
		TCHAR   aszLeadThru[ PARA_LEADTHRU_LEN + 1 ] = {0};
		TCHAR   aszTblNo[ PRT_ID_LEN + 1 ] = {0};

		if ( usTblNo != 0 ) {
			_itot( usTblNo, aszTblNo, 10 );
			if ( sWidthType == PRT_DOUBLE ) {
				/* --- set up unique transaction number string --- */
				PrtSDouble( aszTransNo, (PRT_ID_LEN * 2 + 1 ), aszTblNo );
				PrtGetLead( aszLeadThru, LDT_UNIQUE_NO_ADR );
			} else {
				RflGetSpecMnem( aszSpecMnem, SPC_TBL_ADR );
			}
		}

        if ( sWidthType == PRT_DOUBLE ) {   /* print with double width */
            /* --- print unique trans no. and no. of person --- */
            PrtPrintf( PMG_PRT_RECEIPT, aszPrtTHTransPerson, aszLeadThru, aszTransNo, aszTranMnem, aszNoPer );
        } else {                            /* print with single width */
            /* --- print table no. and no. of person --- */
            PrtPrintf( PMG_PRT_RECEIPT, aszPrtTHTblPerson, aszSpecMnem, aszTblNo, aszTranMnem, aszNoPer );
        }
    }
}

/*
*===========================================================================
** Format   : VOID PrtTHTblPerGC( USHORT usTblNo,
*                                 USHORT usNoPerson,
*                                 USHORT usGuestNo,
*                                 UCHAR  uchCDV,
*                                 SHORT  sWidthType );
*
*   Input   : USHORT usTblNo,   - Table number
*             USHORT usNoPerson - No. of Person
*             USHORT usGuestNo  - Guest Check No.
*             UCHAR  uchCDV     - Check Digits
*             SHORT  sWidthType - Print Width Type
*                                   PRT_DOUBLE, PRT_SINGLE
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints table No., No. of person, and guest check No. line.
*===========================================================================
*/
VOID PrtTHTblPerGC(USHORT usTblNo, USHORT usNoPerson, USHORT usGuestNo,
                            UCHAR uchCDV, TCHAR uchSeatNo, SHORT sWidthType)
{
    TCHAR   aszPsnMnem[ PARA_TRANSMNEMO_LEN + 1 ] = {0};  /* PARA_... defined in "paraequ.h" */
    TCHAR   aszGCMnem[ PARA_LEADTHRU_LEN + 1 ] = {0};   /* PARA_... defined in "paraequ.h" */
    TCHAR   aszSpecMnem[ PARA_SPEMNEMO_LEN + 1 ] = {0};   /* PARA_... defined in "paraequ.h" */
    TCHAR   aszLeadThru[ PARA_LEADTHRU_LEN + 1 ] = {0};
    TCHAR   aszTblNo[ PRT_ID_LEN + 1 ] = {0};
    TCHAR   aszNoPer[ PRT_ID_LEN + 1 ] = {0};
    TCHAR   aszTransNo[ PRT_ID_LEN * 2 + 1 ] = {0};
    TCHAR   aszGuest[ 8 + 1 ] = {0}, aszGuest2[ 8 * 2 + 1 ] = {0};
    ULONG   ulGuest;

    if (( usTblNo == 0 ) && (( usNoPerson == 0 )||(uchSeatNo == 0) ) && ( usGuestNo == 0 )) {
        return;
    }

    /* --- set up special mnemonic of table no., and table no. --- */
    if ( usTblNo != 0 ) {
        _itot( usTblNo, aszTblNo, 10 );
        if ( sWidthType == PRT_DOUBLE ) {
            /* --- set up unique transaction number string --- */
            PrtSDouble( aszTransNo, (PRT_ID_LEN * 2 + 1 ), aszTblNo );
            PrtGetLead( aszLeadThru, LDT_UNIQUE_NO_ADR );
        } else {
			RflGetSpecMnem( aszSpecMnem, SPC_TBL_ADR );
        }
    }

    /* --- set up transaction mnemonic of no. of person, and no. of person --- */
    if (uchSeatNo) {
        /* --- set up sepcial mnemonic of seat no, and seat no --- */
        _itot( uchSeatNo, aszNoPer, 10 );
		RflGetSpecMnem( aszPsnMnem, SPC_SEAT_ADR );    /* seat no. mnemonic */
    } else if( ( usNoPerson != 0 ) && (!CliParaMDCCheck(MDC_TRANNUM_ADR, EVEN_MDC_BIT3))) {
        /* --- set up special mnemonic of no. of person, and no. of person --- */
        _itot( usNoPerson, aszNoPer, 10 );
		RflGetTranMnem( aszPsnMnem, TRN_PSN_ADR );
    }

    /* --- set up transaction mnemonic of GC no., and GC no. string --- */
    if( uchCDV == 0 ) { /* GC No. without Check Digit */
        ulGuest =  usGuestNo;
    } else {            /* GC No. with Check Digit */
        ulGuest =  usGuestNo;
        ulGuest =  ulGuest * 100;
        ulGuest += uchCDV;
    }

    *aszGuest   = '\0';
    if ( ulGuest != 0 ) {
        _ultot( ulGuest, aszGuest, 10 );
//        RflGetTranMnem( aszGCMnem, TRN_GCNO_ADR );
		PrtGetLead( aszGCMnem, LDT_UNIQUE_NO_ADR );
    }

	PrtSDouble( aszGuest2, TCHARSIZEOF(aszGuest2), aszGuest );

    if ( sWidthType == PRT_DOUBLE ) {   /* print with double width */
        /* --- print table no. with double width ( unique tran no. ) --- */
        if ( usTblNo != 0 && aszLeadThru[0] != 0) {
			if(!CliParaMDCCheck(MDC_TRANNUM_ADR, EVEN_MDC_BIT1))//option to show SR as order number SR 54
			{
				PrtPrintf( PMG_PRT_RECEIPT, aszPrtTHTransNo, aszLeadThru, aszTransNo );
			}
        }

        /* --- print no. of person with single width --- */
        if ((( usNoPerson != 0 ) || ( uchSeatNo != 0 )) || ( ulGuest != 0L )) {
			if(CliParaMDCCheck(MDC_TRANNUM_ADR, EVEN_MDC_BIT1))//option to show SR as order number SR 54
			{
				 PrtGetLead( aszLeadThru, LDT_UNIQUE_NO_ADR );
				 if (aszLeadThru[0] != 0) {
					if(!CliParaMDCCheck(MDC_TRANNUM_ADR, EVEN_MDC_BIT3))
					{
						PrtPrintf( PMG_PRT_RECEIPT, aszPrtTHTblPerGC, aszSpecMnem, aszTblNo, aszPsnMnem, aszNoPer, aszLeadThru, aszGuest2);
					} else
					{
						PrtPrintf( PMG_PRT_RECEIPT, aszPrtTHTblPerGCNoPerson, aszSpecMnem, aszTblNo, aszLeadThru, aszGuest2);
					}
				}
			}else
			{
				if (aszGCMnem[0] != 0) {
					if(!CliParaMDCCheck(MDC_TRANNUM_ADR, EVEN_MDC_BIT3))
					{
						PrtPrintf( PMG_PRT_RECEIPT, aszPrtTHTblPerGCUnique, aszSpecMnem, aszTblNo, aszPsnMnem, aszNoPer, aszGCMnem, aszGuest2);
					}
					else
					{
						PrtPrintf( PMG_PRT_RECEIPT, aszPrtTHTblPerGCNoPersonUnique, aszSpecMnem, aszTblNo, aszGCMnem, aszGuest2);
					}
				}
			}
        }
    } else {
        if (( usTblNo != 0 ) || (( usNoPerson != 0 )||( uchSeatNo != 0 )) || ( ulGuest != 0L )) {
			if (aszGCMnem[0] != 0) {
				PrtPrintf( PMG_PRT_RECEIPT, aszPrtTHTblPerGC, aszSpecMnem, aszTblNo, aszPsnMnem, aszNoPer, aszGCMnem, aszGuest);
			}
        }
    }
}

/*
*===========================================================================
** Format   : VOID PrtTHCustomerName( UCHAR *pszCustomerName )
*
*   Input   : UCHAR *pszCustomerName    -   address of customer name
*   Output  : none
*   InOut   : none
** Return   : none
*
** Synopsis : This function prints Tent mnemonic and user input on one line
*			  ***PDINU
*===========================================================================
*/
VOID PrtTHTentName( TCHAR *pszCustomerName )
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};

    if ( *pszCustomerName == '\0' ) {
        return;
    }

    if ( *( pszCustomerName + NUM_NAME - 2 ) == PRT_DOUBLE ) {
        *( pszCustomerName + NUM_NAME - 2 ) = '\0';
    }

	RflGetTranMnem(aszTranMnem, TRN_TENT);
    PrtPrintf( PMG_PRT_RECEIPT, aszPrtTHTent, aszTranMnem, pszCustomerName );
}

/*
*===========================================================================
** Format   : VOID PrtTHCustomerName( UCHAR *pszCustomerName )
*
*   Input   : UCHAR *pszCustomerName    -   address of customer name
*   Output  : none
*   InOut   : none
** Return   : none
*
** Synopsis : This function prints customer name
*===========================================================================
*/
VOID PrtTHCustomerName( TCHAR *pszCustomerName )
{
    if ( *pszCustomerName == '\0' ) {
        return;
    }

    if ( *( pszCustomerName + NUM_NAME - 2 ) == PRT_DOUBLE ) {
        *( pszCustomerName + NUM_NAME - 2 ) = '\0';
    }

    PrtPrintf( PMG_PRT_RECEIPT, aszPrtTHCustomerName, pszCustomerName );
}

/*
*===========================================================================
** Format  : VOID   PrtTHMultiSeatNo(TRANINFORMATION *pTran);
*
*   Input  : ITEMSALES *pItem           -seat no
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function formats seat no. line with list of finalized seats. R3.1
*===========================================================================
*/
VOID    PrtTHMultiSeatNo(TRANINFORMATION *pTran)
{
	TCHAR   aszPrintBuff[PRT_EJCOLUMN + 1] = {0};
	TCHAR   aszSpecMnem[ PARA_SPEMNEMO_LEN + 1 ] = {0};
    USHORT  i, usPos;

    if (pTran->TranGCFQual.auchFinSeat[0] == 0) {
        return;
    }

	RflGetSpecMnem( aszSpecMnem, SPC_SEAT_ADR );    /* seat no. mnemonic */
    usPos = tcharlen(aszSpecMnem);

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

    PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHMultiSeat, aszPrintBuff);
}


/*
*===========================================================================
** Format  : VOID  PrtTHTranNum(UCHAR uchAdr, ULONG ulNumber);
*               
*   Input  : UCHAR uchAdr,           -Transactioh mnemonics Address
*            ULONG ulNumber           -Number
*            
*   Output : none
*   InOut  : none
*
** Return  : none
*
** Synopsis: This function prints transaction mnemonics and number line.
*            
*===========================================================================
*/
VOID  PrtTHTranNum(USHORT usAdr, ULONG ulNumber)
{
    TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};/* PARA_... defined in "paraequ.h" */

    /* -- get transaction mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHMnemNum, aszTranMnem, ulNumber);
}

/*
*===========================================================================
** Format  : VOID  PrtTHKitchen(UCHAR  uchKPNo);
*               
*   Input  : UCHAR   uchKPNo
*   Output : none
*   InOut  : none
*
** Return  : none
*
** Synopsis: This function prints "take to kitchen"  line.
*            
*===========================================================================
*/
VOID  PrtTHKitchen(UCHAR  uchKPNo)
{
	TCHAR  aszMnem[PRT_THCOLUMN + 1] = {0};
    TCHAR  aszLDT[PARA_LEADTHRU_LEN + 1] = {0};   
    TCHAR  aszPrtNo[8 + 1] = {0};

    /* -- get 24 char mnemonics -- */
    PrtGet24Mnem(aszMnem, CH24_KTCHN_ADR);

	/* -- send kitchen information -- */
    PrtPrint(PMG_PRT_RECEIPT, aszMnem, tcharlen(aszMnem)); /* ### Mod (2171 for Win32) V1.0 */

    /* -- inititalize -- */
    memset(aszMnem, '\0', sizeof(aszMnem));

    /* -- get lead through mnemonics -- */
    PrtGetLead(aszLDT, LDT_KTNERR_ADR);

    /* -- convert to double -- */
    PrtDouble(aszMnem, (PRT_THCOLUMN + 1), aszLDT);

	/* -- send kitchen information -- */
    PrtPrint(PMG_PRT_RECEIPT, aszMnem, tcharlen(aszMnem)); /* ### Mod (2171 for Win32) V1.0 */

    /* -- inititalize -- */
    memset(aszMnem, '\0', sizeof(aszMnem));

    /* -- set error kitchen printer mnemonics -- */
    RflSPrintf(aszPrtNo, TCHARSIZEOF(aszPrtNo), aszPrtTHPrinNo, uchKPNo); 

    /* -- convert to double -- */
    PrtDouble(aszMnem, (PRT_THCOLUMN + 1), aszPrtNo);

	/* -- send error kitchen printer -- */
    PrtPrint(PMG_PRT_RECEIPT, aszMnem, tcharlen(aszMnem)); /* ### Mod (2171 for Win32) V1.0 */
}


/*
*===========================================================================
** Format  : VOID  PrtTHWaiAmtMnem(USHORT usWaiID, UCHAR uchAdr, LONG lAmount);
*               
*   Input  : USHORT usWaiID      - waiter ID 
*          : UCHAR  uchAdr       - Transaction Mnemonic address 
*          : LONG   lAmount      - amount 
*   Output : none
*   InOut  : none
*
** Return  : none
*
** Synopsis: This function prints Waiter ID, mnemonic, and amount.
*            
*===========================================================================
*/
VOID  PrtTHWaiAmtMnem(ULONG ulWaiID, USHORT usTranAdr, DCURRENCY lAmount)
{
    TCHAR  aszWaiMnem[PARA_SPEMNEMO_LEN + 1] = {0};  /* PARA_... defined in "paraequ.h" */
    TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};/* PARA_... defined in "paraequ.h" */

    /* -- get waiter mnemonics -- */
	RflGetSpecMnem(aszWaiMnem, SPC_CAS_ADR);

    /* -- get transaction mnemonics -- */
	RflGetTranMnem(aszTranMnem, usTranAdr);

    PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHWaiAmtMnem, aszWaiMnem, ulWaiID, aszTranMnem, lAmount);
}


/*
*===========================================================================
** Format  : VOID  PrtTHWaiGC(UCHAR uchAdr, USHORT usWaiID, USHORT usGuestNo, UCHAR uchCDV);
*               
*   Input  : UCHAR  uchAdr       - Transaction Mnemonic address 
*          : USHORT usWaiID      - waiter ID 
*          : USHORT usGuestNo,      -guest no
*          : UCHAR uchCDV           -check digit
*   Output : none
*   InOut  : none
*
** Return  : none
*
** Synopsis: This function prints Waiter ID, mnemonic, and Guest check No.
*            
*===========================================================================
*/
VOID  PrtTHWaiGC(USHORT usTranAdr, ULONG ulWaiID, USHORT usGuestNo, UCHAR uchCDV)
{
    TCHAR  aszWaiMnem[PARA_TRANSMNEMO_LEN + 1] = {0};/* PARA_... defined in "paraequ.h" */
    TCHAR  aszGCMnem[PARA_TRANSMNEMO_LEN + 1] = {0};/* PARA_... defined in "paraequ.h" */

    /* -- get waiter mnemonics -- */
	RflGetTranMnem(aszWaiMnem, usTranAdr);

    /* -- get guest check No. mnemonics -- */
	RflGetTranMnem(aszGCMnem, TRN_GCNO_ADR);
    if (aszGCMnem[0] != 0) {
		if (uchCDV == 0) {
			PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHWaiGC_WOCD, aszWaiMnem, ulWaiID, aszGCMnem, usGuestNo);
		} else {
			PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHWaiGC_WTCD, aszWaiMnem, ulWaiID, aszGCMnem, usGuestNo, (USHORT)uchCDV);
		}
	}
}

/*
*===========================================================================
** Format  : VOID  PrtTHForeign(LONG lForeign, UCHAR uchAdr,
*                                           UCHAR fbStatus,  ULONG ulRate);
*   Input  : LONG  lForeign,      -Foreign tender amount
*            UCHAR uchAdr         -Foreign symbol address
*            UCHAR fbStatus       -Foreign currency decimal point status
*            ULONG ulRate         -Rate
*
*   Output : none
*   InOut  : none
*
** Return  : none
*
** Synopsis: This function prints foreign tender line.
*            
*===========================================================================
*/
VOID  PrtTHForeign(DCURRENCY lForeign, UCHAR uchMinorClass, UCHAR fbStatus, ULONG ulRate, UCHAR fbStatus2)
{
    SHORT  sDecPoint, sDecPoint2;
	USHORT usTranAdr2;
	UCHAR  uchAdr1;

	// if this is a gift receipt then do not bother trying to print the conversion rate, etc.
	if(TrnInformation.TranCurQual.fsCurStatus2 & PRT_GIFT_RECEIPT)
		return;

    /* -- check decimal point -- */
    if (fbStatus & ITEMTENDER_STATUS_0_DECIMAL_3) {
        if (fbStatus & ITEMTENDER_STATUS_0_DECIMAL_0) {
            sDecPoint = PRT_NODECIMAL;
        } else {
            sDecPoint = PRT_3DECIMAL;
        }
    } else {
        sDecPoint = PRT_2DECIMAL;
    }

	PrtGetMoneyMnem (uchMinorClass, &usTranAdr2, &uchAdr1);

	if (CliParaMDCCheckField(MDC_TAX2_ADR, MDC_PARAM_BIT_A) == 0) {
		TCHAR  aszFMnem[PARA_SPEMNEMO_LEN + 1] = {0};  /* PARA_... defined in "paraequ.h" */
		TCHAR  aszFAmt[PRT_THCOLUMN + 1] = {0};
		USHORT usStrLen;

		/* -- get foreign symbol -- */
		RflGetSpecMnem(aszFMnem, uchAdr1);

		/* -- get string length -- */
		usStrLen = tcharlen(aszFMnem);

		/* 6 digit decimal point for Euro, V3.4 */
		if (fbStatus2 & ODD_MDC_BIT0) {
			sDecPoint2 = PRT_6DECIMAL;
		} else {
			sDecPoint2 = PRT_5DECIMAL;
		}

		/* -- format foreign amount -- */
		if (TCHARSIZEOF(aszFAmt) > usStrLen) {
			CONST TCHAR  *aszPrtTHForeign = _T("%.*l$   / %10.*l$");  /* foreign amount and conversion rate */

			RflSPrintf(&aszFAmt[usStrLen], TCHARSIZEOF(aszFAmt) - usStrLen, aszPrtTHForeign, sDecPoint, lForeign, sDecPoint2, ulRate);
		}

		/* -- adjust sign ( + , - ) -- */
		if (lForeign < 0) {
			aszFAmt[0] = _T('-');
			_tcsncpy(&aszFAmt[1], aszFMnem, usStrLen);
		} else {
			_tcsncpy(aszFAmt, aszFMnem, usStrLen);
		}

		//SR 525, Gift Receipt functionality JHHJ 9-01-05
		PrtPrint(PMG_PRT_RECEIPT, aszFAmt, tcharlen(aszFAmt)); /* ### Mod (2171 for Win32) V1.0 */
	} else {
		CONST TCHAR  *aszPrtTHForeign1  = _T("%s%.*l$");    /* foreign amount without the conversion rate */
		CONST TCHAR  *aszMldForeign5 = _T("%-s\t%s");                 /* foreign currency mnemonic and amount, no conversion rate */

		TCHAR  aszFMnem[PARA_TRANSMNEMO_LEN + 1] = {0};  /* PARA_... defined in "paraequ.h" */
		TCHAR  aszFAmt[PRT_THCOLUMN + 1] = {0};
		TCHAR  aszSpecAmt[PARA_SPEMNEMO_LEN + 1] = {0};
		UCHAR  uchFCAdr = SPC_CNSYMNTV_ADR;

		RflGetTranMnem(aszFMnem, usTranAdr2);

        switch (uchMinorClass) {
			case CLASS_MAINTFOREIGN1:
			   uchFCAdr  = (UCHAR)(SPC_CNSYMFC1_ADR);
			   break;
			case CLASS_MAINTFOREIGN2:
			   uchFCAdr  = (UCHAR)(SPC_CNSYMFC2_ADR);
			   break;
			case CLASS_MAINTFOREIGN3:
			   uchFCAdr  = (UCHAR)(SPC_CNSYMFC3_ADR);
			   break;
			case CLASS_MAINTFOREIGN4:
			   uchFCAdr  = (UCHAR)(SPC_CNSYMFC4_ADR);
			   break;
			case CLASS_MAINTFOREIGN5:
			   uchFCAdr  = (UCHAR)(SPC_CNSYMFC5_ADR);
			   break;
			case CLASS_MAINTFOREIGN6:
			   uchFCAdr  = (UCHAR)(SPC_CNSYMFC6_ADR);
			   break;
			case CLASS_MAINTFOREIGN7:
			   uchFCAdr  = (UCHAR)(SPC_CNSYMFC7_ADR);
			   break;
			case CLASS_MAINTFOREIGN8:
			   uchFCAdr  = (UCHAR)(SPC_CNSYMFC8_ADR);
			   break;
		}
		RflGetSpecMnem(aszSpecAmt, uchFCAdr);
		/* -- format foreign amount -- */
		RflSPrintf(aszFAmt, TCHARSIZEOF(aszFAmt), aszPrtTHForeign1, aszSpecAmt, sDecPoint, lForeign);

		PrtPrintf(PMG_PRT_RECEIPT, aszMldForeign5, aszFMnem, aszFAmt);
	}
}

/*
*===========================================================================
** Format  : VOID  PrtTHEuro(uchAdr1, LONG lForeign, UCHAR uchAdr2, ULONG ulRate)
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
VOID  PrtTHEuro(UCHAR uchAdr1, LONG lForeign, UCHAR uchAdr2, ULONG ulRate, UCHAR fbStatus2)
{
    TCHAR  aszFMnem1[PARA_SPEMNEMO_LEN + 1] = {0};  /* PARA_... defined in "paraequ.h" */
    TCHAR  aszFMnem2[PARA_SPEMNEMO_LEN + 1] = {0};  /* PARA_... defined in "paraequ.h" */
    SHORT  sDecPoint;

    /* -- get foreign symbol -- */
	RflGetSpecMnem(aszFMnem1, uchAdr1);
	RflGetSpecMnem(aszFMnem2, uchAdr2);

    if (fbStatus2 & ODD_MDC_BIT0) {
        sDecPoint = PRT_6DECIMAL;
    } else {
        sDecPoint = PRT_5DECIMAL;
    }

    PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHEuro1, aszFMnem1, lForeign, aszFMnem2, sDecPoint, ulRate);

}

/*
*===========================================================================
** Format  : VOID   PrtTHSeatNo(ITEMSALES *pItem);
*
*   Input  : ITEMSALES *pItem           -seat no
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function formats seat no. line to . R3.1
*===========================================================================
*/
VOID    PrtTHSeatNo(TCHAR uchSeatNo)
{
    TCHAR   aszSpecMnem[ PARA_SPEMNEMO_LEN + 1 ] = {0};   /* PARA_... defined in "paraequ.h" */

    if (uchSeatNo == 0) {
        return;
    }

	RflGetSpecMnem( aszSpecMnem, SPC_SEAT_ADR );    /* seat no. mnemonic */

    PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHSeat, aszSpecMnem, uchSeatNo);
}


/*
*===========================================================================
** Format  : VOID   PrtTHSeatNo2(ITEMSALES *pItem);
*
*   Input  : ITEMSALES *pItem           -seat no
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function formats seat no. line to . R3.1
*===========================================================================
*/
VOID    PrtTHSeatNo2(TCHAR uchSeatNo)
{
    if (uchPrtSeatNo == uchSeatNo) {
        return;
    }

    if (uchSeatNo) {
		TCHAR   aszSpecMnem[ PARA_SPEMNEMO_LEN + 1 ] = {0};   /* PARA_... defined in "paraequ.h" */

		RflGetSpecMnem( aszSpecMnem, SPC_SEAT_ADR );    /* seat no. mnemonic */
        PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHSeat, aszSpecMnem, uchSeatNo);
    } else {
        PrtFeed(PMG_PRT_RECEIPT, 1);
    }

    uchPrtSeatNo = uchSeatNo;
}


/*
*===========================================================================
** Format  : VOID  PrtTHQty(TRANINFORMATION  *pTran, ITEMSALES *pItem);
*
*   Input  : TRANINFORMATION  *pTran        -Transaction information address
*            ITEMSALES        *pItem        -item information
*            
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints Tax modifier, Qty and unit price line.
*            
*===========================================================================
*/
VOID  PrtTHQty(TRANINFORMATION  *pTran, ITEMSALES *pItem)
{
    TCHAR  aszTaxMod[PRT_THCOLUMN + 1] = {0};
    TCHAR  auchDummy[NUM_PLU_LEN] = {0};
	TCHAR  aszSpecMnem2[STD_SPEMNEMO_LEN + 1] = {0}; /* PARA_... defined in "paraequ.h" */
    DCURRENCY lPrice = 0;
    USHORT i;
    USHORT  usNoOfChild;

	RflGetSpecMnem(aszSpecMnem2, SPC_EC_ADR);

    /* -- get tax modifier mnemonics -- */
    if ( PRT_WITHOUTDATA == PrtGetTaxMod(aszTaxMod, TCHARSIZEOF(aszTaxMod), pTran->TranModeQual.fsModeStatus, pItem->fbModifier) ) {
        *aszTaxMod = '\0';
    }

    if (labs(pItem->lQTY) == PLU_BASE_UNIT) {
        PrtPrint(PMG_PRT_RECEIPT, aszTaxMod, tcharlen(aszTaxMod));
    } else {
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

        if ((pItem->uchMinorClass == CLASS_DEPTMODDISC) 
                || (pItem->uchMinorClass == CLASS_PLUMODDISC) 
                || (pItem->uchMinorClass == CLASS_SETMODDISC)
                || (pItem->uchMinorClass == CLASS_OEPMODDISC)) {

           	//SR 144 perform MDC check to determine if Qty prints above item line or on the same line
			if(!CliParaMDCCheck(MDC_QTY2LINE_ADR, EVEN_MDC_BIT3)){
				if(pTran->TranCurQual.fsCurStatus2 & PRT_GIFT_RECEIPT)
				{
					//SR 525, Gift Receipt functionality JHHJ 9-01-05
					PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHQuantGift, aszTaxMod, pItem->lQTY / PLU_BASE_UNIT, aszSpecMnem2);
				} else
				{
					PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHQuant, aszTaxMod, pItem->lQTY / PLU_BASE_UNIT, aszSpecMnem2, lPrice);
				}
			}
        } else if (PrtChkVoidConsolidation(pTran, pItem) == FALSE) {
            if (pItem->uchPPICode) {    /* print "QTY * Product / QTY" */
				if(pTran->TranCurQual.fsCurStatus2 & PRT_GIFT_RECEIPT)
				{
					//SR 525, Gift Receipt functionality JHHJ 9-01-05
					PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHPPIQuantGift, aszTaxMod, pItem->lQTY / PLU_BASE_UNIT,aszSpecMnem2, labs(pItem->lQTY / PLU_BASE_UNIT));
				}else
				{
					PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHPPIQuant, aszTaxMod, pItem->lQTY / PLU_BASE_UNIT,aszSpecMnem2, RflLLabs(lPrice), labs(pItem->lQTY / PLU_BASE_UNIT));
				}
            } else if (pItem->uchPM) {    /* print "QTY * Product / PM", 2172 */
                PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHPPIQuant, aszTaxMod, pItem->lQTY / PLU_BASE_UNIT,aszSpecMnem2, RflLLabs(lPrice), pItem->uchPM);
            } else {
               	//SR 144 perform MDC check to determine if Qty prints above item line or on the same line
				if(!CliParaMDCCheck(MDC_QTY2LINE_ADR, EVEN_MDC_BIT3)){
					if(pTran->TranCurQual.fsCurStatus2 & PRT_GIFT_RECEIPT)
					{
						//SR 525, Gift Receipt functionality JHHJ 9-01-05
						PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHQuantGift, aszTaxMod, pItem->lQTY / PLU_BASE_UNIT, aszSpecMnem2);
					} else
					{
						PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHQuant, aszTaxMod, pItem->lQTY / PLU_BASE_UNIT, aszSpecMnem2, lPrice);
					}
				}
            }
        } else {    /* void consolidation print */
            if (pItem->uchPM) {    /* print "QTY * Product / PM", 2172 */
				if(pTran->TranCurQual.fsCurStatus2 & PRT_GIFT_RECEIPT)
					//SR 525, Gift Receipt functionality JHHJ 9-01-05
				{
					PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHPPIQuantGift, aszTaxMod, pItem->lQTY / PLU_BASE_UNIT,aszSpecMnem2, pItem->uchPM);
				}else
				{
					PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHPPIQuant, aszTaxMod, pItem->lQTY / PLU_BASE_UNIT, aszSpecMnem2, RflLLabs(lPrice), pItem->uchPM);
				}
            } else if (abs(pItem->sCouponQTY) != 1) {
               	//SR 144 perform MDC check to determine if Qty prints above item line or on the same line
				if(!CliParaMDCCheck(MDC_QTY2LINE_ADR, EVEN_MDC_BIT3)){
					if(pTran->TranCurQual.fsCurStatus2 & PRT_GIFT_RECEIPT)
						//SR 525, Gift Receipt functionality JHHJ 9-01-05
					{
						PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHQuantGift, aszTaxMod, (LONG)pItem->sCouponQTY, aszSpecMnem2);
					} else
					{
						PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHQuant, aszTaxMod, (LONG)pItem->sCouponQTY, aszSpecMnem2, lPrice);
					}
				}
            }
        }
    }
}

/*
*===========================================================================
** Format  : VOID  PrtTHQty(TRANINFORMATION  *pTran, ITEMSALES *pItem);
*
*   Input  : TRANINFORMATION  *pTran        -Transaction information address
*            ITEMSALES        *pItem        -item information
*            
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints Tax modifier, Qty and unit price line.
*            
*===========================================================================
*/
VOID  PrtTHLinkQty(TRANINFORMATION  *pTran, ITEMSALES *pItem)
{
    TCHAR  aszTaxMod[PRT_THCOLUMN + 1] = {0};
	TCHAR  aszSpecMnem2[STD_SPEMNEMO_LEN + 1] = {0}; /* PARA_... defined in "paraequ.h" */

	RflGetSpecMnem(aszSpecMnem2, SPC_EC_ADR);

    /* -- get tax modifier mnemonics -- */
    if ( PRT_WITHOUTDATA == PrtGetTaxMod(aszTaxMod, TCHARSIZEOF(aszTaxMod), pTran->TranModeQual.fsModeStatus, pItem->fbModifier) ) {
        *aszTaxMod = '\0';
    }

    if (labs(pItem->lQTY) == PLU_BASE_UNIT) {
        PrtPrint(PMG_PRT_RECEIPT, aszTaxMod, tcharlen(aszTaxMod));
    } else {
		DCURRENCY lPrice = pItem->Condiment[0].lUnitPrice;

        if ((pItem->uchMinorClass == CLASS_DEPTMODDISC) 
                || (pItem->uchMinorClass == CLASS_PLUMODDISC) 
                || (pItem->uchMinorClass == CLASS_SETMODDISC)
                || (pItem->uchMinorClass == CLASS_OEPMODDISC)) {

            PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHQuant, aszTaxMod, pItem->lQTY / PLU_BASE_UNIT, aszSpecMnem2 , lPrice);
        } else if (PrtChkVoidConsolidation(pTran, pItem) == FALSE) {
            PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHQuant, aszTaxMod, pItem->lQTY / PLU_BASE_UNIT, aszSpecMnem2, lPrice);
        } else {    /* void consolidation print */
            if (abs(pItem->sCouponQTY) != 1) {
                PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHQuant, aszTaxMod, (LONG)pItem->sCouponQTY, aszSpecMnem2, lPrice);
            }
        }
    }
}

/*
*===========================================================================
** Format  : VOID  PrtTHWeight(TRANINFORMATION  *pTran, ITEMSALES *pItem);
*
*   Input  : TRANINFORMATION  *pTran        -Transaction information address
*            ITEMSALES *pItem               -Item information address
*            
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints tax modifier, weight and unit price line.
*            
*===========================================================================
*/
VOID  PrtTHWeight(TRANINFORMATION  *pTran, ITEMSALES  *pItem)
{
    TCHAR  aszTaxMod[PRT_THCOLUMN + 1] = {0};
    TCHAR  aszWeightSym[PARA_SPEMNEMO_LEN + 1] = {0}; /* PARA_... defined in "paraequ.h" */
	TCHAR  aszSpecMnem2[STD_SPEMNEMO_LEN + 1] = {0}; /* PARA_... defined in "paraequ.h" */
    SHORT  sDecPoint;                           /* decimal point */
    LONG   lModQty;                             /* modified qty */

    /* -- get tax modifier mnemonics -- */
    if ( PRT_WITHOUTDATA == PrtGetTaxMod(aszTaxMod, TCHARSIZEOF(aszTaxMod), pTran->TranModeQual.fsModeStatus, pItem->fbModifier) ) {
        *aszTaxMod = '\0';
    }

    /* -- get quantity symbol, scalable symbol and adjust weight -- */
	RflGetSpecMnem(aszSpecMnem2, SPC_EC_ADR);
    PrtGetScale(aszWeightSym, &sDecPoint, &lModQty, pTran, pItem);

    /* -- get Manual weight mnemonics -- */
    if (pItem->fsPrintStatus & PRT_MANUAL_WEIGHT) {
		TCHAR  aszManuMnem[PARA_TRANSMNEMO_LEN + 1] = {0}; /* PARA_... defined in "paraequ.h" */

		RflGetTranMnem(aszManuMnem, TRN_MSCALE_ADR);

		/* -- send "weight " and Manual weight mnemonics -- */
		PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHWeight1Manual, aszTaxMod, sDecPoint, (DWEIGHT)lModQty, aszWeightSym, aszManuMnem);
    } else {
		TCHAR  aszEmptyStr [2] = {0};   // no manual weight mnemonic needed.

		/* -- send "weight " and Manual weight mnemonics -- */
		PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHWeight1, aszTaxMod, aszEmptyStr, sDecPoint, (DWEIGHT)lModQty, aszWeightSym);
    }

    PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHWeight2, aszSpecMnem2, (DCURRENCY)pItem->lUnitPrice, pItem->uchPM, aszWeightSym);
}
/*
*===========================================================================
** Format  : VOID  PrtTHChild(UCHAR uchAdj, UCHAR *pszMnem);
*
*   Input  : UCHAR uchAdj           -Adjective number
*            UCHAR *pszMnem         -Adjective mnemonics address
*            
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints child plu line.
*            
*===========================================================================
*/
VOID  PrtTHChild(UCHAR uchAdj, TCHAR *pszMnem)
{
    TCHAR    aszAdjMnem[PARA_ADJMNEMO_LEN + 1] = {0};

    if (uchAdj != 0) {
        /* --get adjective mnemocnics -- */
		RflGetAdj(aszAdjMnem,  uchAdj);
    }

    /* -- send adjective mnemonics and child plu mnemonics -- */
    PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHChild, aszAdjMnem, pszMnem);
}

/*
*===========================================================================
** Format  : VOID  PrtTHChild(UCHAR uchAdj, UCHAR *pszMnem);
*
*   Input  : UCHAR uchAdj           -Adjective number
*            UCHAR *pszMnem         -Adjective mnemonics address
*            
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints child plu line.
*            
*===========================================================================
*/
VOID  PrtTHLinkPLU(USHORT fsModified, UCHAR uchAdj, TCHAR *pszMnem, DCURRENCY lPrice)
{
    if (uchAdj != 0) {
		TCHAR    aszAdjMnem[PARA_ADJMNEMO_LEN + 1] = {0};

        /* --get adjective mnemocnics -- */
		RflGetAdj(aszAdjMnem,  uchAdj);

        /* -- send adjective mnemonics and child plu mnemonics -- */
        if (fsModified) {
            PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHModLinkPLU1, aszAdjMnem, pszMnem, lPrice);
        } else {
            PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHLinkPLU1, aszAdjMnem, pszMnem, lPrice);
        }
    } else {
        /* -- send adjective mnemonics and child plu mnemonics -- */
        if (fsModified) {
            PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHModLinkPLU2, pszMnem, lPrice);
        } else {
            PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHLinkPLU2, pszMnem, lPrice);
        }
    }
}

/*
*===========================================================================
** Format  : VOID  PrtTHItems(ITEMSALES  *pItem);
*
*   Input  : ITEMSALES  *pItem    -structure "itemsales" pointer
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints adjective, noun mnemo.,  print modifier, conidiment line.
*===========================================================================
*/
VOID  PrtTHItems(TRANINFORMATION *pTran, ITEMSALES  *pItem)
{
	TCHAR  aszItem[PRT_MAXITEM + PRT_THCOLUMN] = {0}; /* "+PRT_THCOLUMN" for PrtGet1Line */
	TCHAR  aszAmt[PRT_AMOUNT_LEN + 5] = {0};     /*  5 (2+1    + 2)( space*2 + terminate) */
	TCHAR  aszSpecMnem[STD_SPEMNEMO_LEN + 1] = {0}; /* PARA_... defined in "paraequ.h" */
	TCHAR  aszSpecMnem2[STD_SPEMNEMO_LEN + 1] = {0}; /* PARA_... defined in "paraequ.h" */
    DCURRENCY   lProduct;
    TCHAR  *pszStart;
    USHORT  usAmtLen;                                    
    USHORT  usMnemLen;                                    
    TCHAR   uchWork;
    SHORT  sPrintType, i;
	BOOL	bVoidCons;

    /* --- set condiment print status by mdc, R3.1 --- */
    sPrintType = PrtChkPrintType(pItem);
    /* -- set item mnenemonics to buffer -- */
    PrtSetItem(aszItem, pItem, &lProduct, sPrintType, 0);
    /* -- change last space into NULL -- */
    if ( ! (sPrintType == PRT_ONLY_NOUN || sPrintType == PRT_ONLY_NOUN_AND_ALT) ) {
        aszItem[tcharlen(aszItem) - 1] = '\0';
    }

	RflGetSpecMnem(aszSpecMnem, SPC_CNSYMNTV_ADR);
	RflGetSpecMnem(aszSpecMnem2, SPC_EC_ADR);
	
	bVoidCons = PrtChkVoidConsolidation(pTran, pItem);

    /* -- print out net sales price -- */
    if ((pItem->uchMinorClass == CLASS_DEPTMODDISC) 
            || (pItem->uchMinorClass == CLASS_PLUMODDISC) 
            || (pItem->uchMinorClass == CLASS_SETMODDISC)
            || (pItem->uchMinorClass == CLASS_OEPMODDISC)
            || (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE)) {
        ;
    } else if (bVoidCons == FALSE) {
        ;
    } else if( pItem->usFor == 0 && pItem->usForQty == 0 ) {
		D13DIGITS d13Work;

        /* void consolidation print */
        /* recalculate sales price from sCouponQTY */
        d13Work = pItem->lUnitPrice;
        for (i = pItem->uchChildNo; i < STD_MAX_COND_NUM; i++) {
           d13Work += pItem->Condiment[i].lUnitPrice;
        }
        /* --- calcurate product * qty --- */
        d13Work *= (LONG)pItem->sCouponQTY;
        lProduct = (DCURRENCY)d13Work;
    }

    pszStart = aszItem;
    for (;;) {
		TCHAR  *pszEnd;

        /* -- get 1 line -- */
        if ( PRT_LAST == PrtGet1Line(pszStart, &pszEnd, (USHORT)(usPrtTHColumn - PRT_AMOUNTLEN)) ) { /* ### Mod (2171 for Win32) V1.0 */
            break;
        }

        /* send adjective, noun mnemo, print modifier, condiment */
        PrtPrint(PMG_PRT_RECEIPT, pszStart, (USHORT)(pszEnd - pszStart)); /* ### Mod (2171 for Win32) V1.0 */

        /* -- "1" for space -- */
        pszStart = pszEnd + 1;
    }

    usMnemLen = tcharlen(pszStart);               /* get mnem. length */
     
	if(pTran->TranCurQual.fsCurStatus2 & PRT_GIFT_RECEIPT)
	{
		//SR 525, Gift Receipt functionality JHHJ 9-01-05
		usAmtLen = RflSPrintf(aszAmt, TCHARSIZEOF(aszAmt), aszPrtTHAmtGift);
	} else
	{
		usAmtLen = RflSPrintf(aszAmt, TCHARSIZEOF(aszAmt), aszPrtTHAmtS, lProduct);
	}

    uchWork = (UCHAR)(   (pItem->uchMinorClass == CLASS_DEPTMODDISC) 
                      || (pItem->uchMinorClass == CLASS_PLUMODDISC)
                      || (pItem->uchMinorClass == CLASS_SETMODDISC)
                      || (pItem->uchMinorClass == CLASS_OEPMODDISC) );

    if (uchWork) {                              /* if mod. disc. , write space */
        aszAmt[usAmtLen] = PRT_SPACE;
        usAmtLen++;
    }

    if (usMnemLen + usAmtLen + 1 > PRT_THCOLUMN) {   /* '+1' is for space b/w mnem and amount */
        /* send mnemonic line */
        PrtPrint(PMG_PRT_RECEIPT, pszStart, PRT_THCOLUMN);

        uchWork = '\0';
        PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHMnemMnem, &uchWork, aszAmt);
    }  else {
         if (uchWork) {
			//SR 144 perform MDC check to determine if Qty prints above item line or on the same line
			if(!CliParaMDCCheck(MDC_QTY2LINE_ADR, EVEN_MDC_BIT3)){
				PrtPrintf(PMG_PRT_RECEIPT,  aszPrtTHAmtMnemSpace, pszStart, lProduct);
			}else {
				if(bVoidCons)
				{
					if(pTran->TranCurQual.fsCurStatus2 & PRT_GIFT_RECEIPT)
					{
						//SR 525, Gift Receipt functionality JHHJ 9-01-05
						PrtPrintf(PMG_PRT_RECEIPT,  aszPrtTHAmtMnemSpace1LineGift, pItem->sCouponQTY, aszSpecMnem2, pItem->lUnitPrice, pszStart);
					} else if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE)
					{
						PrtPrintf(PMG_PRT_RECEIPT,  aszPrtTHAmtMnemSpaceWeight, pszStart, lProduct);
					}else
					{
						PrtPrintf(PMG_PRT_RECEIPT,  aszPrtTHAmtMnemSpace1Line, pItem->sCouponQTY, aszSpecMnem2,pItem->lUnitPrice, pszStart, aszSpecMnem, lProduct);
					}
		
					}else{
					if(pTran->TranCurQual.fsCurStatus2 & PRT_GIFT_RECEIPT)
					{
						//SR 525, Gift Receipt functionality JHHJ 9-01-05
						PrtPrintf(PMG_PRT_RECEIPT,  aszPrtTHAmtMnemSpace1LineGift, pItem->lQTY / PLU_BASE_UNIT, aszSpecMnem2,pItem->lUnitPrice, pszStart);
					}else if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE)
					{
						PrtPrintf(PMG_PRT_RECEIPT,  aszPrtTHAmtMnemSpaceWeight, pszStart, lProduct);
					} else
					{
						PrtPrintf(PMG_PRT_RECEIPT,  aszPrtTHAmtMnemSpace1Line, pItem->lQTY / PLU_BASE_UNIT, aszSpecMnem2,pItem->lUnitPrice, pszStart, aszSpecMnem, lProduct);
					}
				}
			}
        } else {
			//SR 144 perform MDC check to determine if Qty prints above item line or on the same line
			if(!CliParaMDCCheck(MDC_QTY2LINE_ADR, EVEN_MDC_BIT3)){
				if(pTran->TranCurQual.fsCurStatus2 & PRT_GIFT_RECEIPT)
				{
					//SR 525, Gift Receipt functionality JHHJ 9-01-05
					PrtPrintf(PMG_PRT_RECEIPT,  aszPrtTHAmtMnemGift, pszStart);
				}else if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE)
				{
					PrtPrintf(PMG_PRT_RECEIPT,  aszPrtTHAmtMnemSpaceWeight, pszStart, lProduct);
				} else
				{
					PrtPrintf(PMG_PRT_RECEIPT,  aszPrtTHAmtMnem, pszStart, aszSpecMnem, lProduct);
				}
			}else {
				if(bVoidCons)
				{
					if(pTran->TranCurQual.fsCurStatus2 & PRT_GIFT_RECEIPT)
						//SR 525, Gift Receipt functionality JHHJ 9-01-05
					{
						PrtPrintf(PMG_PRT_RECEIPT,  aszPrtTHAmtMnemGift, pszStart);
					}else if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE)
					{
						PrtPrintf(PMG_PRT_RECEIPT,  aszPrtTHAmtMnemSpaceWeight, pszStart, lProduct);
					} else
					{
						PrtPrintf(PMG_PRT_RECEIPT,  aszPrtTHAmtMnem1Line, pItem->sCouponQTY, aszSpecMnem2, (DCURRENCY)pItem->lUnitPrice, pszStart, aszSpecMnem, lProduct );
					}
				}else
				{
					if(pTran->TranCurQual.fsCurStatus2 & PRT_GIFT_RECEIPT)
					{
						//SR 525, Gift Receipt functionality JHHJ 9-01-05
						PrtPrintf(PMG_PRT_RECEIPT,  aszPrtTHAmtMnem1LineGift, pItem->lQTY / PLU_BASE_UNIT, aszSpecMnem2, pszStart);
					}else if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE)
					{
						PrtPrintf(PMG_PRT_RECEIPT,  aszPrtTHAmtMnemSpaceWeight, pszStart, lProduct);
					} else
					{
						PrtPrintf(PMG_PRT_RECEIPT,  aszPrtTHAmtMnem1Line, pItem->lQTY / PLU_BASE_UNIT, aszSpecMnem2, (DCURRENCY)pItem->lUnitPrice, pszStart, aszSpecMnem, lProduct );
					}
				}
			}
        }
    }
}

/*
*===========================================================================
** Format  : VOID  PrtTHZeroAmtMnem(UCHAR uchAddress, LONG lAmount);
*
*   Input  : UCHAR uchAddress    -transaction mnemonics address
*            LONG  lAmount       -change
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints change line.
*            
*===========================================================================
*/
VOID  PrtTHZeroAmtMnem(USHORT usAddress, DCURRENCY lAmount)
{
    if (lAmount != 0L) {
        PrtTHAmtMnem(usAddress, lAmount);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtTHDiscItems(ITEMSALES  *pItem);
*
*   Input  : ITEMSALES  *pItem    -structure "itemsales" pointer
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints adjective, noun mnemo.,  print modifier, conidiment line.
*===========================================================================
*/
VOID  PrtTHDiscItems(ITEMSALES  *pItem)
{
	TCHAR  aszItem[PRT_MAXITEM + PRT_THCOLUMN] = {0};
	TCHAR  aszSpecMnem[STD_SPEMNEMO_LEN + 1] = {0}; /* PARA_... defined in "paraequ.h" */
    DCURRENCY   lProduct;
    TCHAR  *pszStart;

    /* -- set item mnenemonics to buffer -- */
    PrtSetItem(aszItem, pItem, &lProduct, PRT_ALL_ITEM, 0);
	RflGetSpecMnem(aszSpecMnem, SPC_CNSYMNTV_ADR);

    pszStart = aszItem;

    for (;;) {
		TCHAR  *pszEnd;

        /* -- get 1 line -- */
        if ( PRT_LAST == PrtGet1Line(pszStart, &pszEnd, (USHORT)(usPrtTHColumn - PRT_AMOUNTLEN)) ) { /* ### Mod (2171 for Win32) V1.0 */
            break;
        }

        /* send adjective, noun mnemo, print modifier, condiment */
        PrtPrint(PMG_PRT_RECEIPT, pszStart, (USHORT)(pszEnd - pszStart)); /* ### Mod (2171 for Win32) V1.0 */

        /* -- "1" for space -- */
        pszStart = pszEnd + 1;
    }

    if (pItem->uchRate == 0) {    /* not print rate */
		// see also function PrtAdjustNative() with its print format.
        PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHAmtMnem, pszStart, aszSpecMnem, pItem->lDiscountAmount);
    } else {                      /* print rate */
        PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHAmtDiscMnem, pszStart, pItem->uchRate, aszSpecMnem, pItem->lDiscountAmount);
    }
}                       

/*
*===========================================================================
** Format  : VOID  PrtTHHead(TRANINFORMATION  *pTran);
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints thermal header. (with square double width)
*===========================================================================
*/
SHORT PrtTHHead(TRANINFORMATION  *pTran)
{
    UCHAR i;
    UCHAR uchTrgPrt;

    if ( fbPrtTHHead == PRT_HEADER_PRINTED ) {
        return -1;        /* header already printed */
    }

    if ( !( fbPrtLockChk ) ) {                  /* for reg mode */
        /* -- check shared printer or not -- */
        if ( fbPrtAltrStatus & PRT_NEED_ALTR ) {
            uchTrgPrt = PRT_WITHALTR;
        } else {
            uchTrgPrt = PRT_WITHSHR;
        }

        if ( 0 != PrtCheckShr(uchTrgPrt) ) {
            fbPrtShrStatus |= PRT_SHARED_SYSTEM;    /* indicate shared printer system */     
            PrtShrInit(pTran);                      /* initialize shared printer buffer */
        }
    }

	if(CliParaMDCCheck(MDC_PRINTPRI_ADR, EVEN_MDC_BIT3))
	{
		PrtLogo(PMG_PRT_RECEIPT, 1);
	}
    
    for ( i = CH24_1STRH_ADR; i <= CH24_4THRH_ADR; i++) {
		TCHAR asz24Mnem[PARA_CHAR24_LEN + 1] = {0};
		TCHAR aszSDblMn[PARA_CHAR24_LEN * 2 + 1] = {0};

        PrtGet24Mnem(asz24Mnem, i);

        /* -- convert to square double -- */
        PrtSDouble(aszSDblMn, TCHARSIZEOF(aszSDblMn), asz24Mnem);

        /* --  send header -- */
        PrtPrint(PMG_PRT_RECEIPT, aszSDblMn, tcharlen(aszSDblMn)); /* ### Mod (2171 for Win32) V1.0 */
    }

    /* -- 1 line feed -- */
    PrtFeed(PMG_PRT_RECEIPT, 1);    
    
    fbPrtTHHead = PRT_HEADER_PRINTED;           /* header printed */

	return 1;
}

/*
*===========================================================================
** Format  : VOID  PrtTHDoubleHead(VOID);
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : none
*                
** Synopsis: This functions prints Double Header mnemo line. 
*===========================================================================
*/
VOID  PrtTHDoubleHead(VOID)
{
	TCHAR  aszDouble[PARA_TRANSMNEMO_LEN + 1] = {0};

	RflGetTranMnem(aszDouble, TRN_DBLRID_ADR);

    PrtPrint(PMG_PRT_RECEIPT, aszDouble, tcharlen(aszDouble)); /* ### Mod (2171 for Win32) V1.0 */
}


/*
*===========================================================================
** Format  : VOID  PrtTHGuest(USHORT usGuestNo, UCHAR uchCDV);
*
*   Input  : USHORT usGuestNo,      -guest no
*            UCHAR uchCDV           -check digit
*   Output : none
*   InOut  : none
** Return  : none
*                
** Synopsis: This functions prints guest check line. 
*===========================================================================
*/
VOID  PrtTHGuest(USHORT usGuestNo, UCHAR uchCDV)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};/* PARA_... defined in "paraequ.h" */

    if (usGuestNo == 0) {                       /* GCF# 0, doesnot print */
        return ;
    }

    /* -- get transaction mnemonics -- */
	RflGetTranMnem(aszTranMnem, TRN_GCNO_ADR);
    if (aszTranMnem[0] != 0) {
		if (uchCDV == 0) {
			PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHGC_WOCD_S, aszTranMnem, usGuestNo);
		} else {
			PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHGC_WTCD_S, aszTranMnem, usGuestNo, (USHORT)uchCDV);
		}
	}
}


/*
*===========================================================================
** Format  : VOID  PrtTHMulChk(USHORT usGuestNo, UCHAR uchCDV);
*
*   Input  : USHORT usGuestNo,      -guest no
*            UCHAR uchCDV           -check digit
*   Output : none
*   InOut  : none
** Return  : none
*                
** Synopsis: This functions prints guest check line. 
*===========================================================================
*/
VOID  PrtTHMulChk(USHORT usGuestNo, UCHAR uchCDV)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};/* PARA_... defined in "paraequ.h" */

    if (usGuestNo == 0) {                       /* GCF# 0, doesnot print */
        return ;
    }

    /* -- get transaction mnemonics -- */
	RflGetTranMnem(aszTranMnem, TRN_CKPD_ADR);

    if (uchCDV == 0) {
        PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHGC_WOCD_S, aszTranMnem, usGuestNo);
    } else {
        PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHGC_WTCD_S, aszTranMnem, usGuestNo, (USHORT)uchCDV);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtTHWaiTaxMod(USHORT usID, USHORT fsTax, UCHAR  fbMod);
*               
*   Input  : USHORT usID          -waiter ID
*            USHORT fsTax,        -US or Canadian Tax
*            UCHAR  fbMod         -modifier status
*   Output : none
*   InOut  : none
*
** Return  : none
*
** Synopsis: This function prints Waiter & Tax Modifier line.
*===========================================================================
*/
VOID  PrtTHWaiTaxMod(ULONG ulID, USHORT  fsTax, USHORT  fbMod)
{
    TCHAR  aszTaxMod[PRT_THCOLUMN + 1] = {0};
    TCHAR  aszWaiMnem[PARA_SPEMNEMO_LEN + 2] = {0};  /* PARA_... defined in "paraequ.h" */
    SHORT  sRet;

    if (ulID != 0) {
        /* -- get waiter mnemonics -- */
		RflGetSpecMnem(aszWaiMnem, SPC_CAS_ADR);
    }

    /* -- get tax modifier mnemonics -- */
    sRet = PrtGetTaxMod(aszTaxMod, TCHARSIZEOF(aszTaxMod), fsTax, fbMod);

    /* if ID=0 and not exist taxmodifer, not print line */
    if (ulID != 0 && sRet == PRT_WITHDATA) {
        PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHWaiTaxMod, aszWaiMnem, ulID, aszTaxMod);
    } else if (ulID != 0) {
        *aszTaxMod = '\0';
        PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHWaiTaxMod, aszWaiMnem, ulID, aszTaxMod);
    } else if (sRet == PRT_WITHDATA) {
        PrtPrint(PMG_PRT_RECEIPT, aszTaxMod, tcharlen(aszTaxMod)); /* ### Mod (2171 for Win32) V1.0 */
    }
}

/*
*===========================================================================
** Format  : VOID  PrtTHMnemCount(UCHAR uchAdr, SHORT sCount);
*               
*   Input  : UCHAR  uchAdr        -Transaction Mnemnics Address
*            SHORT  sCount        -Counter
*   Output : none
*   InOut  : none
*
** Return  : none
*
** Synopsis: This function prints Trans. Menm.  and short value.
*===========================================================================
*/
VOID  PrtTHMnemCount(USHORT usAdr, SHORT sCount)
{
    TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};  /* PARA_... defined in "paraequ.h" */

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- send mnemoncis and amount -- */
    PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHMnemCount, aszTranMnem, sCount);
}

/*
*===========================================================================
** Format  : VOID  PrtTHTrail1(TRANINFORMATION  *pTran);
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints normal trailer 1st line.
*===========================================================================
*/
VOID  PrtTHTrail1(TRANINFORMATION *pTran)
{
	extern CONST TCHAR FARCONST  aszCasWaiID[];
	CONST TCHAR FARCONST  *aszPrtTHTrail1  = _T("%-4s %8s\t %s"); /* trailer 1st line */
    TCHAR  aszMnem[PARA_SPEMNEMO_LEN + 1] = {0};
    ULONG  ulID;
    TCHAR  *pszName;
    TCHAR   aszID[8 + 1] = {0, 0, 0, 0};

    if ( PRT_CASHIER == PrtChkCasWai(aszMnem, pTran->TranModeQual.ulCashierID, pTran->TranModeQual.ulWaiterID) ) {
        ulID    = pTran->TranModeQual.ulCashierID;
        pszName = pTran->TranModeQual.aszCashierMnem;
    } else {
        ulID    = pTran->TranModeQual.ulWaiterID;
        pszName = pTran->TranModeQual.aszWaiterMnem;
    }

    /* -- convert ID to ascii -- */
	if (ulID != 0)
		RflSPrintf(aszID, TCHARSIZEOF(aszID), aszCasWaiID, RflTruncateEmployeeNumber(ulID));

    PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHTrail1, aszMnem, aszID, pszName);

}

/*
*===========================================================================
** Format  : VOID  PrtTHTrail2(TRANINFORMATION  *pTran, ULONG  ulStRegNo);
*
*   Input  : TRANINFORMATION *pTran         -transaction information
*            ULONG  ulStRegNo               -Store/Reg No.
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints trailer 2nd line.
*===========================================================================
*/
VOID PrtTHTrail2(TRANINFORMATION  *pTran, ULONG  ulStRegNo)
{
	CONST TCHAR FARCONST  *aszPrtTHTrail2 = _T("%04d\t %s   %04lu-%03lu");    /* trailer 2nd line */
    TCHAR   aszDate[PRT_DATETIME_LEN + 1] = {0};

   PrtGetDate(aszDate, TCHARSIZEOF(aszDate), pTran);   /* get date time */

   //SR 124, and 664 fix, instead of looking at the Current Qualifier, we will
   //look at the GCF Qualifier which has the correct number for the Guest check
   //that is being printed.
    PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHTrail2, pTran->TranGCFQual.usConsNo, aszDate, ulStRegNo / 1000, ulStRegNo % 1000 );
}
//End SR 124

/*
*===========================================================================
** Format  : VOID  PrtTHTickTrail1(TRANINFORMATION  *pTran);
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints ticket 1st line.
*===========================================================================
*/
VOID  PrtTHTickTrail1(TRANINFORMATION  *pTran)
{
    TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};/* PARA_... defined in "paraequ.h" */
    TCHAR  aszCasMnem[PARA_SPEMNEMO_LEN + 1] = {0};
    TCHAR  aszTblMnem[PARA_SPEMNEMO_LEN + 1] = {0};
    TCHAR  aszGuest2[2 * 8 + 1] = {0};
    TCHAR  aszTblNo2[2 * 8 + 1] = {0};
    ULONG  ulGuest;
    TCHAR  *pszName;

    /* -- cashier or waiter? -- */
    if ( PRT_CASHIER == PrtChkCasWai(aszCasMnem, pTran->TranModeQual.ulCashierID, pTran->TranModeQual.ulWaiterID) ) {
        pszName = pTran->TranModeQual.aszCashierMnem;
    } else {
        pszName = pTran->TranModeQual.aszWaiterMnem;
    }

    if ( ! ( pTran->TranCurQual.flPrintStatus & CURQUAL_UNIQUE_TRANS_NO ) && ( pTran->TranGCFQual.usTableNo != 0 )) {
		if(!CliParaMDCCheck(MDC_TRANNUM_ADR, EVEN_MDC_BIT1))
		{
			TCHAR aszTblNo[8 + 1] = {0};

			RflGetSpecMnem(aszTblMnem, SPC_TBL_ADR);      /* get table mnemonics */
			_itot(pTran->TranGCFQual.usTableNo, aszTblNo, 10);
			PrtSDouble( aszTblNo2, TCHARSIZEOF(aszTblNo2), aszTblNo );
		}
    }

    /* -- GCF no. without check digit -- */
    if(pTran->TranGCFQual.uchCdv == 0) {
        ulGuest =  pTran->TranGCFQual.usGuestNo;
    } else {
        ulGuest =  pTran->TranGCFQual.usGuestNo;
        ulGuest =  ulGuest * 100;
        ulGuest += pTran->TranGCFQual.uchCdv;
    }

    if (ulGuest != 0) {
        /* -- get transaction mnemonics -- */
        /* -- then convert the guest check number to ascii or double sized ascii -- */
		RflGetTranMnem(aszTranMnem, TRN_GCNO_ADR);

		if(CliParaMDCCheck(MDC_TRANNUM_ADR, EVEN_MDC_BIT1)) //option to show SR as order number SR 54
		{
			TCHAR  aszGuest[8 + 1] = {0};

			_ultot(ulGuest, aszGuest, 10);
			PrtSDouble( aszGuest2, TCHARSIZEOF(aszGuest2), aszGuest );
		} else {
			_ultot(ulGuest, aszGuest2, 10);
		}
    }

    PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHTickTrail1, aszTranMnem, aszGuest2, aszTblMnem, aszTblNo2, pszName);
}

/*
*===========================================================================
** Format  : VOID  PrtTHTickTrail2(TRANINFORMATION  *pTran, ULONG  ulStRegNo);
*
*   Input  : TRANINFORMATION *pTran         -transaction information
*            ULONG  ulStRegNo               -Store/Reg No.
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints ticket trailer 2nd line.
*===========================================================================
*/
VOID PrtTHTickTrail2(TRANINFORMATION  *pTran)
{
	CONST TCHAR FARCONST  *aszPrtTHTickTrail2 = _T("%04d %4s %8.8Mu\t %s   %04lu-%03lu"); /* ticket trailer 2nd line */
    TCHAR  aszMnem[PARA_SPEMNEMO_LEN + 1] = {0};
    TCHAR  aszDate[PRT_DATETIME_LEN + 1] = {0};
    ULONG  ulID;

    if ( PRT_CASHIER == PrtChkCasWai(aszMnem, pTran->TranModeQual.ulCashierID, pTran->TranModeQual.ulWaiterID) ) {
        ulID = RflTruncateEmployeeNumber(pTran->TranModeQual.ulCashierID);
    } else {
        ulID = RflTruncateEmployeeNumber(pTran->TranModeQual.ulWaiterID);
    }

    PrtGetDate(aszDate, TCHARSIZEOF(aszDate), pTran);   /* get date time */

    PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHTickTrail2, pTran->TranCurQual.usConsNo, 
        aszMnem, ulID, aszDate, pTran->TranCurQual.ulStoreregNo / 1000, pTran->TranCurQual.ulStoreregNo % 1000 );
                
}

/*
*===========================================================================
** Format  : VOID  PrtTHEtkCode(ITEMMISC   *pItem, SHORT sType);
*
*   Input  : ITEMMISC *pItem         -structure "itemmisc" pointer
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints ETK employee code and job code.
*===========================================================================
*/
VOID PrtTHEtkCode(ITEMMISC  *pItem, SHORT sType)
{
	extern CONST TCHAR FARCONST  aszEtkJobCode[];
	CONST TCHAR FARCONST  *aszPrtETKCode = _T("%-16s %8.8Mu\t%s");             /* change at R3.1 */
    TCHAR   aszJobCode[12] = {0};

    if ( ( sType == PRT_WTIN) && ( pItem->uchJobCode != 0 ) ) {
        RflSPrintf(aszJobCode, TCHARSIZEOF(aszJobCode), aszEtkJobCode, pItem->uchJobCode);
    } else {
        aszJobCode[0] = '\0';
    }

    PrtPrintf(PMG_PRT_RECEIPT, aszPrtETKCode, pItem->aszMnemonic, RflTruncateEmployeeNumber(pItem->ulEmployeeNo), aszJobCode);
}

/*
*===========================================================================
** Format  : VOID  PrtTHEtkOut(UCHAR uchAdr1, UCHAR uchAdr2);
*
*   Input  : UCHAR  uchAdr1   -Transaction Mnemonics Address
*          : UCHAR  uchAdr2   -Transaction Mnemonics Address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints ETK time-in/out mnemonics.
*===========================================================================
*/
VOID PrtTHEtkOut(USHORT  usTranAdr1, USHORT  usTranAdr2)
{
    TCHAR  aszTimeInMnem[PARA_TRANSMNEMO_LEN + 1] = {0};  /* PARA_... defined in "paraequ.h" */
    TCHAR  aszTimeOutMnem[PARA_TRANSMNEMO_LEN + 1] = {0};  /* PARA_... defined in "paraequ.h" */

    /* -- get time-in mnemonics -- */
	RflGetTranMnem(aszTimeInMnem, usTranAdr1);

    /* -- get time-in mnemonics -- */
	RflGetTranMnem(aszTimeOutMnem, usTranAdr2);

    PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHMnemMnem, aszTimeInMnem, aszTimeOutMnem);
}

/*
*===========================================================================
** Format  : VOID  PrtTHJobTimeIn(TRANINFORMATION   *pTran, ITEMMISC   *pItem);
*
*   Input  : TRANINFORMATION *pTran  -transaction information address
*            ITEMMISC *pItem         -structure "itemmisc" pointer
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints ETK time-in time.
*===========================================================================
*/
VOID PrtTHJobTimeIn(TRANINFORMATION *pTran, ITEMMISC  *pItem)
{
    TCHAR   aszDate[PRT_DATETIME_LEN + 1] = {0};

    /* -- get job time-in code -- */
    PrtGetJobInDate(aszDate, TCHARSIZEOF(aszDate), pTran, pItem);  /* get date time */
  
    PrtPrint(PMG_PRT_RECEIPT, aszDate, tcharlen(aszDate)); /* ### Mod (2171 for Win32) V1.0 */
    
}

/*
*===========================================================================
** Format  : VOID  PrtTHJobTimeOut(TRANINFORMATION   *pTran, ITEMMISC   *pItem, SHORT sType);
*
*   Input  : TRANINFORMATION *pTran  -transaction information address
*            ITEMMISC *pItem         -structure "itemmisc" pointer
*            SHORT      sType           -print type
*                                           PRT_WTIN: with time-in
*                                           PRT_WOIN: without time-in
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints ETK time-out time.
*===========================================================================
*/
VOID PrtTHJobTimeOut(TRANINFORMATION *pTran, ITEMMISC  *pItem, SHORT sType)
{
    TCHAR   aszDateIn[PRT_DATETIME_LEN + 1] = {0};
    TCHAR   aszDateOut[PRT_DATETIME_LEN + 1] = {0};

    if ( sType == PRT_WTIN ) {

        /* -- get job time-in time -- */
        PrtGetJobInDate(aszDateIn, TCHARSIZEOF(aszDateIn), pTran, pItem);  /* get date time */
    } else {
		tcharnset(aszDateIn, _T('*'), PRT_DATETIME_LEN);
        aszDateIn[PRT_DATETIME_LEN] = '\0';  
    }

    /* -- get job time-out time -- */
    PrtGetJobOutDate(aszDateOut, TCHARSIZEOF(aszDateOut), pTran, pItem);   /* get date time */
  
    PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHMnemMnem, aszDateIn, aszDateOut);
    
}

/*
*===========================================================================
** Format  : VOID  PrtTHCPRoomCharge(UCHAR *pRoomNo, UCHAR *pGuestID);
*                                           
*   Input  : UCHAR  *pRoomNo        -Pointer of Room Number 
*            UCHAR  *pGuestID       -Pointer of Guest ID 
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints room no., guest id for charge posting.
*            
*===========================================================================
*/
VOID  PrtTHCPRoomCharge(TCHAR *pszRoomNo, TCHAR *pszGuestID)
{
    if ((*pszRoomNo != 0xff) && (*pszRoomNo != '\0') && (*pszGuestID != '\0')) {
		TCHAR  aszSpecMnem1[PARA_SPEMNEMO_LEN + 1] = {0};  
		TCHAR  aszSpecMnem2[PARA_SPEMNEMO_LEN + 1] = {0};  

		RflGetSpecMnem(aszSpecMnem1, SPC_ROOMNO_ADR);   /* get room no. mnemo */
		RflGetSpecMnem(aszSpecMnem2, SPC_GUESTID_ADR);  /* get guest id */

        PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHCPRoomCharge, aszSpecMnem1, pszRoomNo, aszSpecMnem2, pszGuestID);
     }
}

/*
*===========================================================================
** Format  : VOID  PrtTHCPRspMsgText(UCHAR *pRspMsgText);
*                                   
*   Input  : UCHAR  *pRspMsgText    -Pointer of Response Message Text 
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints response message text for charge posting.
*            
*===========================================================================
*/
USHORT  PrtTHCPRspMsgText(TCHAR *pRspMsgText)
{
	CONST TCHAR *aszPrtTHCPRspMsgText = _T("   %s");         /* charge posting, bank server response text */
	USHORT usRet = 0;

	if ((TrnInformation.TranCurQual.fbNoPrint & CURQUAL_NO_EPT_RESPONSE) == 0) {
		if (*pRspMsgText != '\0') {
			PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHCPRspMsgText, pRspMsgText);   // PrtTHCPRspMsgText()
			usRet = 1;
		}
    }
	return usRet;
}

/*
*===========================================================================
** Format  : VOID  PrtTHOffline(SHORT fsMod, USHORT usDate, UCHAR *aszApproval);
*               
*    Input : SHORT fsMod           -offline modifier
*            USHORT usDate         -expiration date 
*            UCHAR  *aszApproval   -approval code
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints prints EPT offline symbol,
*            expiration date and approval code.
*
*  Print format :
*
*      24-char printer
*
*       0        1         2      
*       123456789012345678901234  
*      |^^^^^^^^^^^^^^^^^^^^^^^^| 
*      |MMMM   MM/YY  123456    |
*      |                        | 
*       ^^^^^^^^^^^^^^^^^^^^^^^^  
*
*===========================================================================
*/
VOID  PrtTHOffline(USHORT fbMod, TCHAR *auchDate, TCHAR *auchApproval)
{
	TCHAR  aszApproval[NUM_APPROVAL + 1] = {0};
    TCHAR  aszDate[NUM_EXPIRA + 1] = {0};
    TCHAR  aszOffMnem[PARA_SPEMNEMO_LEN + 1] = {0};
    TCHAR  aszOffDate[NUM_EXPIRA * 2 + 1] = {0};      // may be additional characters in date other than NUM_EXPIRA
    USHORT  usOffDate;

    _tcsncpy(aszApproval, auchApproval, NUM_APPROVAL);
    _tcsncpy(aszDate, auchDate, NUM_EXPIRA);

    if ( fbMod & OFFEPTTEND_MODIF) {                /* offline modifier */
        /* -- get special mnemonics -- */
		RflGetSpecMnem( aszOffMnem, SPC_OFFTEND_ADR);
    }           

    if ( *aszDate != '\0' ){                        /* expiration date  */
		//we need to have the option to mask the 
		//expiration date, due to a new
		//law put into effect 12.04,06 JHHJ
		if(CliParaMDCCheck(MDC_EPT_MASK3_ADR,ODD_MDC_BIT1))
		{
			_tcsncpy(aszDate, auchDate, NUM_EXPIRA);
		}else
		{
			_tcsncpy(aszDate, _T("XXXX"), NUM_EXPIRA);
		}
		if (aszDate[0] == _T('X')) {			
			_tcsncpy(aszOffDate,_T("XX/XX"),NUM_EXPIRA+1);
		} else {
			usOffDate = _ttoi(aszDate);
			if (usOffDate != 0) {                       /* E-028 corr. 4/20     */
				RflSPrintf( aszOffDate, TCHARSIZEOF(aszOffDate), aszPrtTHOffDate, usOffDate / 100, usOffDate % 100 );
			}
		}
    }

    /* -- send mnemoncis and exp.date -- */
    if (*aszDate != '\0' || *aszApproval != '\0' || *aszOffMnem != '\0') {
        PrtPrintf( PMG_PRT_RECEIPT, aszPrtTHOffline, aszOffMnem, aszOffDate, aszApproval );
    }
}
/*
*===========================================================================
** Format  : VOID  PrtTHEPTError(ITEMPRINT  *pItem);
*               
*    Input : ITEMPRINT pItem       -print data
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints prints EPT status & error code
*
*  Print format :
*
*      24-char printer
*
*       0        1         2      
*       123456789012345678901234  
*      |^^^^^^^^^^^^^^^^^^^^^^^^| 
*      |M 9999                  |
*      |                        | 
*       ^^^^^^^^^^^^^^^^^^^^^^^^  
*
*===========================================================================
*/
VOID  PrtTHEPTError(ITEMPRINT  *pItem)
{
/*  --- fix a glitch (05/15/2001)
    PmgPrintf(PMG_PRT_JOURNAL, aszPrtTHEPTError, pItem->uchStatus, (USHORT)pItem->lAmount ); */
    PrtPrintf(PMG_PRT_JOURNAL, aszPrtTHEPTError, pItem->uchStatus, (USHORT)pItem->lAmount );
}

/*
*===========================================================================
** Format  : VOID  PrtTHPLUNoP (UCHAR *puchPLUNo);
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
*       123456789012345678901234567890123456789012
*      |^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
*      |99999999999999                            |
*      |                                          |
*       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
*===========================================================================
*/
VOID    PrtTHPLUNo(TCHAR *puchPLUCpn)
{
    if (*puchPLUCpn) {
        PrtPrint(PMG_PRT_RECEIPT, puchPLUCpn, tcharlen(puchPLUCpn));
    }
}

/*
*===========================================================================
** Format  : VOID  PrtAdjustForeign(UCHAR *pszDest, LONG lAmount);
*
*   Input  : LONG      lAmount         -amount
*   Output : UCHAR     *pszDest        -destination buffer address
*   InOut  : none
** Return  : none
*
** Synopsis: This function adjusts FC mnemonics and amount sign
*===========================================================================
*/
VOID    PrtAdjustForeign(TCHAR *pszDest, DCURRENCY lAmount, UCHAR uchAdr, UCHAR uchMDC)
{
    static const TCHAR FARCONST  aszPrtRJFC[] = _T("%.*l$");
    TCHAR  aszSpecMnem[PARA_SPEMNEMO_LEN + 1] = {0};  /* PARA_... defined in "paraequ.h" */
    USHORT usStrLen;
    SHORT  sDecPoint;

    /* -- check decimal point -- */
    if ( uchMDC & ITEMTENDER_STATUS_0_DECIMAL_3 ) {
        if ( uchMDC & ITEMTENDER_STATUS_0_DECIMAL_0 ) {
            sDecPoint = PRT_NODECIMAL;
        } else {
            sDecPoint = PRT_3DECIMAL;
        }
    } else {
        sDecPoint = PRT_2DECIMAL;
    }

    /* -- get fc mnemonics -- */
	RflGetSpecMnem(aszSpecMnem, uchAdr);

    /* -- get string length -- */
    usStrLen = tcharlen(aszSpecMnem);

    /* -- format foreign amount -- */
    RflSPrintf( &pszDest[usStrLen], PRT_THCOLUMN+1, aszPrtRJFC, sDecPoint, lAmount );
    /* -- adjust sign ( + , - ) -- */
    if (lAmount < 0) {
        pszDest[0] = _T('-');
        _tcsncpy(&pszDest[1], aszSpecMnem, usStrLen);
    } else {
        _tcsncpy(pszDest, aszSpecMnem, usStrLen);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtTHMoneyForeign (LONG lForeign, UCHAR uchAdr1, UCHAR uchAdr2,
*                                     UCHAR fbStatus);
*
*   Input  : LONG    lForeign      -Foreign tender amount
*            UCHAR   uchAdr1       -Foreign tender mnemonic address
*            UCHAR   uchAdr2       -Foreign symbol address
*            UCHAR   fbStatus      -Foreign currency decimal point status
*
*   Output : none
*   InOut  : none
*
** Return  : none
*
** Synopsis: This function prints foreign tender amount and tender mnemonic.
*
*  Print format :
*
*       0        1         2         3         4
*       123456789012345678901234567890123456789012
*      |^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|
*      |MMMMMMMM                    SMMM999999.999|
*      |                                          |
*       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
*===========================================================================
*/
VOID    PrtTHMoneyForeign (DCURRENCY lForeign, USHORT usTranAdr1, UCHAR uchAdr2, UCHAR fbStatus)
{
    TCHAR   aszFMnem[PARA_SPEMNEMO_LEN + 1] = {0};        /* PARA_... defined in "paraequ.h" */
    TCHAR   aszFAmt[PRT_THCOLUMN + 1] = {0};
    TCHAR   aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};   /* PARA_... defined in "paraequ.h" */
    USHORT  usStrLen;
    SHORT   sDecPoint;

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
	RflGetSpecMnem( aszFMnem, uchAdr2 );

    /* -- get string length -- */
    usStrLen = tcharlen( aszFMnem );

    /* -- format foreign amount -- */
    RflSPrintf( &aszFAmt[usStrLen], TCHARSIZEOF(aszFAmt), aszPrtTHMoneyForeign, sDecPoint, lForeign );

    /* -- adjust sign ( + , - ) -- */
    if ( lForeign < 0 ) {
        aszFAmt[0] = _T('-');
        _tcsncpy( &aszFAmt[1], aszFMnem, usStrLen );
    } else {
        _tcsncpy( aszFAmt, aszFMnem, usStrLen );
    }

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem( aszTranMnem, usTranAdr1 );

    PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHMnemMnem, aszTranMnem, aszFAmt);
}

/*
*===========================================================================
** Format  : VOID  PrtTHVoid(CHAR fbMod);
*               
*    Input : CHAR   fbMod                -Void status      
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints "void" line with double wide chara.
*            if void status is off, does not print.
*===========================================================================
*/
VOID    PrtSupTHVoid(USHORT fbMod, USHORT usReasonCode)
{
    if (fbMod & (VOID_MODIFIER | RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) {
		TCHAR         aszWork[PRT_THCOLUMN + 1] = {0};

		if (0 > PrtGetReturns(fbMod, usReasonCode, aszWork, TCHARSIZEOF(aszWork))) {
			/* -- get void mnemonics instead of returns mnemonic -- */
			PrtGetVoid(aszWork, TCHARSIZEOF(aszWork));
		}

        /* -- print thermal --*/
        PrtPrint(PMG_PRT_RECEIPT, aszWork, usPrtTHColumn);
    }
}

/*===========================================================================
** Format  : VOID  PrtTHTrail1(TRANINFORMATION  *pTran);
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints normal trailer 1st line.
** //SR 217 JHHJ Item Count Option
*===========================================================================
*/
VOID  PrtTHItemCount(SHORT noItems, USHORT usPrtType)
{
    TCHAR  aszMnem[STD_TRANSMNEM_LEN + 1] = {0};

	RflGetTranMnem(aszMnem, TRN_ITEMCOUNT_ADR);

	if(usPrtType == PMG_PRT_SLIP)
	{
		TCHAR  aszBuffer[PRT_SPCOLUMN + 1] = {0};

		RflSPrintf(aszBuffer, TCHARSIZEOF(aszBuffer),aszPrtTHItemCountSP,aszMnem, noItems);
		PrtPrint(usPrtType, aszBuffer, PRT_SPCOLUMN);
	} else {
		PrtPrintf(usPrtType, aszPrtTHItemCount,aszMnem, noItems);
	}

}


/*===========================================================================
** Format  : PrtTHTipTotal(USHORT  usPrtType)
*
*   Input  : USHORT  usPrtType    -print type
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints tip and total line mnemonics on receipt
*===========================================================================
*/
VOID  PrtTHTipTotal(USHORT  usPrtType)
{
    TCHAR  aszMnem[STD_TRANSMNEM_LEN + 1] = {0};
	TCHAR  aszBuffer[PRT_SPCOLUMN + 1] = {0};
	CONST TCHAR aszPrtTHTipSP[]  = _T("%24.20s    _______________");  //Aligned right w/short line right

	RflGetTranMnem(aszMnem, TRN_TIP_LINE_ADR);

	PrtFeed(usPrtType, 1);

	RflSPrintf(aszBuffer, TCHARSIZEOF(aszBuffer), aszPrtTHTipSP,aszMnem);
	PrtPrint(usPrtType, aszBuffer, PRT_SPCOLUMN);

	PrtFeed(usPrtType, 2);	

	RflGetTranMnem(aszMnem, TRN_TTL_LINE_ADR);

	RflSPrintf(aszBuffer, TCHARSIZEOF(aszBuffer), aszPrtTHTipSP,aszMnem);
	PrtPrint(usPrtType, aszBuffer, PRT_SPCOLUMN);

	PrtFeed(usPrtType, 1);

}
/*
*===========================================================================
** Format  : VOID  PrtTHOrderDec(USHORT usAdr);
*               
*   Input  : UCHAR  uchAdr             -Transaction mnemonics address
*   Output : none
*   InOut  : none
*
** Return  : none
*
** Synopsis: This function prints transaction mnemonic, native mnemonic  line.
*===========================================================================
*/
VOID  PrtTHOrderDec(USHORT usAdr)
{
	TCHAR  aszTranMnem[(PARA_TRANSMNEMO_LEN) + 1] = {0};      /* PARA_... defined in "paraequ.h" */
    TCHAR  aszTranMnemD[(PARA_TRANSMNEMO_LEN)* 2 + 1] = {0};  /* PARA_... defined in "paraequ.h" */

    /* -- get transaction mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- convert to double -- */
    PrtDouble(aszTranMnemD, TCHARSIZEOF(aszTranMnemD), aszTranMnem);

 	PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHMnemMnem, aszTranMnemD, _T(" "));
}

/*
*===========================================================================
** Format  : VOID  PrtTHAmtMnem(UCHAR uchAdr, LONG lAmout);
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
VOID  PrtTHAmtGiftMnem(ITEMTENDER  *pItem)
{
    TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};  /* PARA_... defined in "paraequ.h" */
	TCHAR  aszSpecMnem[STD_SPEMNEMO_LEN + 1] = {0}; /* PARA_... defined in "paraequ.h" */

	/* -- get transaction  mnemonics -- */
//	RflGetTranMnem(aszTranMnem, uchAdr);

	_tcscpy(aszTranMnem,pItem->aszTendMnem);

	RflGetSpecMnem(aszSpecMnem, SPC_CNSYMNTV_ADR);

    /* -- send mnemoncis and amount -- */
	if(TrnInformation.TranCurQual.fsCurStatus2 & PRT_GIFT_RECEIPT)
	{
		//SR 525, Gift Receipt functionality JHHJ 9-01-05
		PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHAmtMnemGift, aszTranMnem);
	} else
	{
		// see also function PrtAdjustNative() with its print format.
		PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHAmtMnem, aszTranMnem, aszSpecMnem, pItem->lTenderAmount);
	}
}
/****** End of Source ******/