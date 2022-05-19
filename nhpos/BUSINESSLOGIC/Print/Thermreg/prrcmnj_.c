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
**     @  *=*=*=*=*=*=*=*  0             (C) Copyright, 1995              **
**    <|\/~             ~\/|>                                             **
**   _/^\_               _/^\_                                            **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print common routine, reg mode, journal                 
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRCmnJ_.C
*               | ||  ||
*               | ||  |+- S:21Char L:24Char _:not influcenced by print column
*               | ||  +-- T:thermal J:jounal S:slip K:kitchen 
*               | |+----- comon routine
*               | +------ R:reg mode S:super mode
*               +-------- Pr:print module
* --------------------------------------------------------------------------
* Abstract:
*
*   PrtEJVoid()
*   PrtEJOffTend()
*   PrtEJNumber()
*   PrtEJAmtMnem()
*   PrtEJMnem()
*   PrtEJTaxMod()
*   PrtEJPerDisc()
*#  PrtEJCoupon()       add REL 3.0
*   PrtEJAmtSym()
*#  PrtEJTblPerson()    mod. REL 3.0
*#  PrtEJCustomerName() add REL 3.0
*   PrtEJMultiSeatNo()  add at R3.1
*   PrtEJTranNum()
*   PrtEJWaiter()
*   PrtEJForeign1()
*   PrtEJForeign2()
*   PrtEJQty()
*   PrtEJWeight()
*   PrtEJChild()
*   PrtEJItems()
*   PrtEJZeroAmtMnem() 
*   PrtEJDiscItems()
*   PrtEJGuest()
*   PrtEJMulChk()
*   PrtEJWaiTaxMod()
*   PrtEJMnemCount()
*   PrtEJTrail1()
*   PrtEJTrail2()
*   PrtEJEtkCode()
*   PrtEJJobTimeIn()
*   PrtEJJobTimeOut()
*   PrtEJInit()
*   PrtEJWrite()
*   PrtEJSend()
*   PrtEJCPRoomCharge()
*   PrtEJFolioPost()
*   PrtEJCPRspMsgText()
*   PrtEJOffline()
*   PrtEJEPTError()
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-14-93 : 01.00.12 : R.Itoh     : initial                                   
* Jun-29-93 : 01.00.12 : K.You      : mod. PrtEJQty for US enhance.                                   
* Jul-06-93 : 01.00.12 : R.Itoh     : add PrtEJEtkCode, PrtEJEtkJobTimeIn.                                   
* Oct-21-93 : 02.00.02 : K.You      : add charge posting feature.                                   
* Apr-08-94 : 00.00.04 : Yoshiko.J  : add PrtEJOffline(), PrtEJEPTError()
* Apr-24-94 : 00.00.05 : K.You      : bug fixed E-32 (mod. PrtEJOffTend)
* Mar-03-95 : 03.00.00 : T.Nakahata : Add coupon feature (PrtEJCoupon) and
*                                     Unique Trans No. (mod. PrtEJTblPerson)
* Jun-07-95 : 03.00.00 : T.Nakahata : add promotional PLU with condiment
* Aug-29-95 : 03.00.04 : T.Nakahata : FVT comment (unique tran#, 4 => 12 Char)
*           :          :            : customer name (max 16 chara)
* Nov-15-95 : 03.01.00 : M.Ozawa    : Change ETK format for R3.1
* Dec-08-95 : 03.01.00 : M.Ozawa    : Change auchPrtEJBuffer to FAR 
* Aug-08-99 : 03.05.00 : M.Teraki   : Remove WAITER_MODEL
* Aug-12-99 : 03.05.00 : K.Iwata    : Marge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
*
** GenPOS **
* Feb-19-18 : v2.2.2.0 : R.Chambers  : fix buffer size used with function PrtGetSISymCoupon().
* Apr-26-18 : v2.2.2.0 : R.Chambers  : clean mnemonics to eliminate tilde for double wide chars.
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
/**------- M S - C -------**/
#include	<tchar.h>
#include    <stdlib.h>
#include    <string.h>
#include	<stdio.h>
/**------- 2170 local------**/
#include    "ecr.h"
#include    "regstrct.h"
#include    "transact.h"
#include    "paraequ.h"
#include    "para.h"
#include    "csstbpar.h"
#include    "uie.h"
#include    "pmg.h"
#include    "rfl.h"
#include    "pif.h"
#include    "ej.h"
#include    "csstbej.h"
#include    "prtcom.h"
#include    "prtrin.h"
#include    "prrcolm_.h"
#include    "maint.h"

/**
;========================================================================
;+                    S T A T I C    R A M s
;========================================================================
**/
//byte array contains header info and character info
static UCHAR   auchPrtEJBuffer[EJ_PRT_BUFFLEN];   /* E/J print buffer R3.1 */
                                                    /* "EJ_PRT_BUFFLEN" defined in ej.h */
static USHORT  usPrtEJOffset;                       /* E/J buffer write offset */


CONST TCHAR FARCONST  aszPrtEJAmtMnem[] = _T("%s\t%l$");    /* trans.mnem and amount */
//SR 144 cwunn added to print quantity, mnemonic, and amount on the same line
CONST TCHAR FARCONST  aszPrtEJAmtMnem1Line[] = _T("%ld X $%l$ %s\t%l$");  /* trans.mnem and amount */
//end SR 144

CONST TCHAR FARCONST  aszPrtAmtMnemShift[] = _T("%8.8s\t %l$%*s"); /*8 characters JHHJ*/  /* trans.mnem and amount, V3.3 */
CONST TCHAR FARCONST  aszPrtEJAmtDiscMnem[] = _T("%8.8s\t%d%%%11l$");    /* disc. rate and amount */

CONST TCHAR FARCONST  aszPrtEJMnemMnem[] = _T("%.8s\t %s");/*8 characters JHHJ*/

/*** Add Unique Transaction Number (REL 3.0) ***/
CONST TCHAR FARCONST  aszPrtEJTblPerson[] = _T("%-4s %-3s %s %s");  /* table and person */
CONST TCHAR FARCONST  aszPrtEJTransNo[] = _T("%-12s %-8s");         /* unique trans no. */
CONST TCHAR FARCONST  aszPrtEJPerson[] = _T("         %s %s");      /* person no. */
CONST TCHAR FARCONST  aszPrtEJCustomerName[] = _T("%-19s");         /* customer name */
/*** Add Unique Transaction Number (REL 3.0) ***/

CONST TCHAR FARCONST  aszPrtEJMultiSeat[] = _T("%-s");          /* seat no. */

CONST TCHAR FARCONST  aszPrtEJMnemNum[] = _T("%-8s %lu");       /* mnemo. and number */

CONST TCHAR FARCONST  aszPrtEJPrinNo[]  = _T("PRT%d");          /* printer no. */

CONST TCHAR FARCONST  aszPrtEJWaiter[]  = _T("%s%8.8Mu");         /* waiter mnemo. and ID */
CONST TCHAR FARCONST  aszPrtEJWaiTaxMod[] = _T("%s%8.8Mu  %s");  /* waiter mnemo. and ID and taxmodifier */

CONST TCHAR FARCONST  aszPrtEJForeign1[]  = _T("%.*l$");         /* foreign amount */

CONST TCHAR FARCONST  aszPrtEJForeign2[]  = _T("   / %10.*l$"); /* V3.4 */
                                                 /* foreign conversion rate */
                                                 /* XXXXX.XXXXX */

CONST TCHAR FARCONST  aszPrtEJSeat[] = _T("%-4s %1d"); //SR206 _T("%-4s %2d");       /* seat no. */

CONST TCHAR FARCONST  aszPrtEJQuant[]   = _T("%8ld X\t %l$ ");         /* qty and unit price */

CONST TCHAR FARCONST  aszPrtEJPPIQuant[]   = _T("%8ld X\t %l$/%ld ");  /* qty, product and qty */

CONST TCHAR FARCONST  aszPrtEJWeight1[] = _T("%8.*l$%s\t %s");         /* Weight and Manual Mnem. */

CONST TCHAR FARCONST  aszPrtEJWeight2[] = _T("\t@ %l$/%s ");           /* scale sym. and unit price */

CONST TCHAR FARCONST  aszPrtEJChild[]   = _T(" %4s %s");               /* child plu */
CONST TCHAR FARCONST  aszPrtEJChild2[]   = _T(" %s");                  /* child plu, saratoga */

CONST TCHAR FARCONST  aszPrtEJModLinkPLU1[]   = _T("%4s %s\t%l$ ");    /* child plu */
CONST TCHAR FARCONST  aszPrtEJModLinkPLU2[]   = _T("%s\t%l$ ");        /* child plu, saratoga */

CONST TCHAR FARCONST  aszPrtEJLinkPLU1[]   = _T("%4s %s\t%l$");        /* child plu */
CONST TCHAR FARCONST  aszPrtEJLinkPLU2[]   = _T("%s\t%l$");            /* child plu, saratoga */

CONST TCHAR FARCONST  aszPrtEJTrail2[] = _T("%04d\t %s");              /* trailer 2nd line */

CONST TCHAR FARCONST  aszPrtEJGuest_WTCD[] = _T("%-8s %4d%02d");       /* guest no. with check digit */

CONST TCHAR FARCONST  aszPrtEJGuest_WOCD[] = _T("%-8s %4d");        /* guest no. without check digit */

CONST TCHAR FARCONST  aszPrtQty[] = _T("%8ld X");                   /* qty line */

CONST TCHAR FARCONST  aszPrtEJMnemCount[] = _T("%-8s\t%d");         /* mnemo. and counter */

CONST TCHAR FARCONST  aszPrtEJModAmtMnem[] = _T("%s\t %l$ ");       /* mod. disc. product */

//SR 144 cwunn added to print quantity, mnemonic, and amount on the same line
CONST TCHAR FARCONST  aszPrtEJModAmtMnem1Line[] = _T("%ld X $%l$ %s\t %l$ ");    /* mod. disc. product */
//end SR 144

CONST TCHAR FARCONST  aszPrtEJCPRoomCharge[] = _T("%-4s%6s\t %-4s%3s"); /* room charge */

CONST TCHAR FARCONST  aszPrtEJFolioPost[] = _T("%-4s%6s\t %-4s%6s");    /* folio, posted transaction no. */

CONST TCHAR FARCONST  aszPrtEJCPRspMsgText[] = _T("%-40s");                /* charge posting */
CONST TCHAR FARCONST  aszPrtEJOffline[] = _T("%-4s   %5s   %6s");       /* mnemo, date, approval code */

CONST TCHAR FARCONST  aszPrtEJOffDate[] = _T("%02d/%02d");              /* expiration date */

CONST TCHAR FARCONST  aszPrtEJEPTError[] = _T("%c %d");                 /* mnemo. and error code */

extern CONST TCHAR FARCONST aszPrtTime[];
extern CONST TCHAR FARCONST aszPrtTimeZone[];
extern CONST TCHAR FARCONST aszPrtDate[];

extern CONST TCHAR FARCONST  aszEtkCode[];


extern CONST TCHAR FARCONST  aszPrtTHEuro1[];         /* Euro amount */
extern CONST TCHAR FARCONST  aszPrtTHEuroAmtMnem[];   /* trans.mnem and amount */

CONST TCHAR FARCONST  aszPrtEJPluBuild[]      = _T("%s\t %13s %04d");     /* Plu no and Dept no. */
CONST TCHAR FARCONST  aszPrtEJMoneyForeign[]  = _T("%.*l$");

CONST TCHAR FARCONST  aszPrtEJAmtMnemSI[] = _T("%s %s\t %l$");            /* trans.mnem and amount, 21RFC05437 */
CONST TCHAR FARCONST  aszPrtEJAmtDiscMnemSI[] = _T("%s %s %d%%\t%11l$");  /* disc. rate and amount */

/**
;========================================================================
;+             P R O G R A M    D E C L A R A T I O N s
;========================================================================
**/
/*
*===========================================================================
** Format  : VOID  PrtEJVoid(CHAR fbMod);
*               
*    Input : CHAR   fbMod                -Void status      
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints "void" line.
*            if void status is off, does not print.
*===========================================================================
*/
VOID PrtEJVoid(USHORT  fbMod, USHORT usReasonCode)
{
    if (fbMod & (VOID_MODIFIER | RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) {
		TCHAR         aszWork[PRT_EJCOLUMN + 1] = {0};             /* print work area */

		if (0 > PrtGetReturns(fbMod, usReasonCode, aszWork, TCHARSIZEOF(aszWork))) {
			/* -- get void mnemonics instead of returns mnemonic -- */
			PrtGetVoid(aszWork, TCHARSIZEOF(aszWork));
		}

        /* -- print Receipt & Journal --*/
		RflCleanupMnemonic (aszWork);
        PrtPrint(PMG_PRT_JOURNAL, aszWork, usPrtEJColumn);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtEJVoid(CHAR fbMod);
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
VOID PrtEJOffTend(USHORT  fbMod)
{
    if (fbMod & OFFCPTEND_MODIF) {
		TCHAR         aszWork[PRT_EJCOLUMN + 1] = {0};             /* print work area */

        /* -- get off line tender mnemonics -- */
        PrtGetOffTend(fbMod, aszWork, TCHARSIZEOF(aszWork));

        /* -- print Receipt & Journal --*/
		RflCleanupMnemonic (aszWork);
        PrtPrint(PMG_PRT_JOURNAL, aszWork, usPrtEJColumn);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtEJNumber(UCHAR  *pszNumber);
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
VOID  PrtEJNumber(TCHAR  *pszNumber)
{
    if (*pszNumber != 0) {
		TCHAR   aszNumLine[PRT_EJCOLUMN * 2 + 1] = {0};

		RflGetTranMnem(aszNumLine, TRN_NUM_ADR );
        PrtSetNumber(aszNumLine, pszNumber);
        PrtPrint(PMG_PRT_JOURNAL, aszNumLine, tcharlen(aszNumLine)); /* ### Mod (2171 for Win32) V1.0 */
    }
}

/*
*===========================================================================
** Format  : VOID  PrtEJNumber(UCHAR  *pszNumber);
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
VOID  PrtEJMnemNumber(USHORT usAdr, TCHAR  *pszNumber)
{
    if (*pszNumber != 0) {
		TCHAR   aszNumLine[PRT_EJCOLUMN * 2 + 1] = {0};

        if (usAdr == 0) usAdr = TRN_NUM_ADR;

		RflGetTranMnem(aszNumLine, usAdr );
        PrtSetNumber(aszNumLine, pszNumber);
        PrtPrint(PMG_PRT_JOURNAL, aszNumLine, tcharlen(aszNumLine)); /* ### Mod (2171 for Win32) V1.0 */
    }
}

/*
*===========================================================================
** Format  : VOID  PrtEJAmtMnem(UCHAR uchAdr, DCURRENCY lAmout);
*               
*    Input : UCHAR      uchAdr          -Transacion mnemonics address
*            DCURRENCY  lAmount         -Amount 
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints prints transaction mnemonics
*            and Amount line.
*===========================================================================
*/
VOID  PrtEJAmtMnem(USHORT usAdr, DCURRENCY lAmount)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 4 + 1] = {0}; // possible 4 characters lead needed.

    /* -- get transaction  mnemonics -- */
	if ((usAdr & STD_FORMATMASK_INDENT_4) != 0) {
		/* -- get transaction  mnemonics -- */
		aszTranMnem [0] = aszTranMnem [1] = aszTranMnem [2] = aszTranMnem [3] = _T(' ');
		RflGetTranMnem(aszTranMnem + 4, usAdr);
	} else {
		/* -- get transaction  mnemonics -- */
		RflGetTranMnem(aszTranMnem, usAdr);
	}

    /* -- send mnemoncis and amount -- */
	RflCleanupMnemonic (aszTranMnem);
    PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJAmtMnem, aszTranMnem, lAmount);
}

/*
*===========================================================================
** Format  : VOID  PrtEJMnem(UCHAR uchAdr, BOOL fsType);
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
VOID  PrtEJMnem(USHORT usAdr, BOOL fsType)
{
#if 1
	// ignore any kind of a double size as indicated by fsType so that
	// the Electronic Journal will have clean mnemonics without the tilde
	// character used to indicate double size print.
	//    Richard Chambers, Apr-26-2018, Rel 2.2.2
	TCHAR  aszPrintBuff[PRT_EJCOLUMN + 1] = {0};

	RflGetTranMnem(aszPrintBuff,  usAdr);
	RflCleanupMnemonic (aszPrintBuff);
#else
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0}; /* PARA_... defined in "paraequ.h" */
	TCHAR  aszPrintBuff[PRT_EJCOLUMN + 1] = {0};

	/* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem,  usAdr);

    /* -- double wide chara? -- */
    if (fsType == PRT_DOUBLE) {
        /* -- convert single to double -- */
        PrtDouble(aszPrintBuff, (PRT_EJCOLUMN + 1), aszTranMnem);
    } else {
        _tcsncpy(aszPrintBuff, aszTranMnem, PARA_TRANSMNEMO_LEN);
    }
#endif

    /* -- send mnemoncis -- */
    PrtPrint(PMG_PRT_JOURNAL, aszPrintBuff, tcharlen(aszPrintBuff)); /* ### Mod (2171 for Win32) V1.0 */
}

/*
*===========================================================================
** Format  : VOID  PrtEJZAMnemShift(UCHAR uchAddress, DCURRENCY lAmount, USHOT usColumn);
*
*   Input  : UCHAR      uchAddress  -transaction mnemonics address
*            DCURRENCY  lAmount     -change
*            USHORT     usColumn    -shift column
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints mnemonics. and amount V3.3
*===========================================================================
*/
VOID  PrtEJZAMnemShift(USHORT usTranAddress, DCURRENCY lAmount, USHORT usColumn)
{
    if (lAmount != 0L) {
        PrtEJAmtMnemShift(usTranAddress, lAmount, usColumn);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtEJAmtMnemShift(UCHAR uchAdr, DCURRENCY lAmount, USHORT usColumn);
*               
*    Input : UCHAR      uchAdr      -Transacion mnemonics address
*            DCURRENCY  lAmount     -Amount
*            USHORT     usColumn    -shift column
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints prints transaction mnemonics
*            and Amount line. V3.3
*===========================================================================
*/
VOID  PrtEJAmtMnemShift(USHORT usTranAdr, DCURRENCY lAmount, USHORT usColumn)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};  /* PARA_... defined in "paraequ.h" */
    TCHAR  uchNull = _T('\0');

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usTranAdr);
	RflCleanupMnemonic (aszTranMnem);

    /* -- send mnemoncis and amount -- */
    PrtPrintf(PMG_PRT_JOURNAL, aszPrtAmtMnemShift, aszTranMnem, lAmount, usColumn, &uchNull);
}

/*
*===========================================================================
** Format  : VOID  PrtEJAmtSymEuro(UCHAR uchAdr1, UCHAR uchAdr2, DCURRENCY lAmount, UCHAR fbStatus)
*               
*   Input  : UCHAR       uchAdr1,      -Transaction mnemonics address
*            UCHAR       uchAdr2       -Euro symbol address
*            DCURRENCY   lAmount       -amount
*            UCHAR       fbStatus      -Foreign currency decimal point status
*   Output : none
*   InOut  : none
*
** Return  : none
*
** Synopsis: This function prints transaction mnemonic, native mnemonic  line. V3.4
*===========================================================================
*/
VOID  PrtEJAmtSymEuro(USHORT usTranAdr1, UCHAR uchAdr2, DCURRENCY lAmount, UCHAR fbStatus)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0}; /* PARA_... defined in "paraequ.h" */
	TCHAR  aszFMnem[PARA_SPEMNEMO_LEN + 1 + 1] = {0};  /* PARA_... defined in "paraequ.h" */
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
	RflCleanupMnemonic (aszTranMnem);

    /* -- adjust native mnemonic and amount sign(+.-) -- */
    if (lAmount < 0) {
        aszFMnem[0] = _T('-');
        /* -- get foreign symbol -- */
		RflGetSpecMnem(&aszFMnem[1], uchAdr2);
    } else {
        /* -- get foreign symbol -- */
		RflGetSpecMnem(aszFMnem, uchAdr2);
    }

	RflCleanupMnemonic (aszFMnem);
    PrtPrintf(PMG_PRT_JOURNAL, aszPrtTHEuroAmtMnem, aszTranMnem, aszFMnem, sDecPoint, RflLLabs(lAmount));
}

/*
*===========================================================================
** Format  : VOID  PrtEJTaxMod(USHORT fsTax, UCHAR  fbMod);
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
VOID  PrtEJTaxMod(USHORT fsTax, USHORT  fbMod)
{
	TCHAR  aszTaxMod[PRT_EJCOLUMN + 1] = {0};

    /* -- get tax modifier mnemonics -- */
    if ( PRT_WITHDATA == PrtGetTaxMod(aszTaxMod, TCHARSIZEOF(aszTaxMod), fsTax, fbMod) ) {
          PrtPrint(PMG_PRT_JOURNAL, aszTaxMod, tcharlen(aszTaxMod));  /* ### Mod (2171 for Win32) V1.0 */
    }
}

/*
*===========================================================================
** Format  : VOID  PrtEJPerDisc(UCHAR uchAdr, UCHAR uchRate, DCURRENCY lAmount);
*               
*   Input  : UCHAR       uchAdr,       -Transaction mnemonics address
*            UCHAR       uchRate,      -rate
*            DCURRENCY   lAmount       -amount
*   Output : none
*   InOut  : none
*
** Return  : none
*
** Synopsis: This function prints % discount line.
*===========================================================================
*/
VOID  PrtEJPerDisc(USHORT usAdr, UCHAR uchRate, DCURRENCY lAmount)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0}; /* PARA_... defined in "paraequ.h" */
    
    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);
	RflCleanupMnemonic (aszTranMnem);

    /* -- send mnemoncis and amount -- */
    if (uchRate == 0) {
        /* not print % */
        PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJAmtMnem, aszTranMnem, lAmount);
    } else {
        PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJAmtDiscMnem, aszTranMnem, uchRate, lAmount);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtEJPerDisc(UCHAR uchAdr, UCHAR uchRate, DCURRENCY lAmount);
*               
*   Input  : UCHAR       uchAdr,      -Transaction mnemonics address
*            UCHAR       uchRate,     -rate
*            DCURRENCY   lAmount      -amount
*   Output : none
*   InOut  : none
*
** Return  : none
*
** Synopsis: This function prints % discount line. 21RFC05437
*===========================================================================
*/
VOID  PrtEJPerDiscSISym(USHORT usAdr, UCHAR uchRate, DCURRENCY lAmount, TCHAR *pszSISym, UCHAR uchMDC)
{
    TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0}; /* PARA_... defined in "paraequ.h" */
    
    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);
	RflCleanupMnemonic (aszTranMnem);

    /* -- send mnemoncis and amount -- */
    if (uchRate == 0) {
        /* not print % */
        if (uchMDC) {
			/* si before mnemonics */
	        PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJAmtMnemSI, pszSISym, aszTranMnem, lAmount);
	    } else {
	        PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJAmtMnemSI, aszTranMnem, pszSISym, lAmount);
	    }
    } else {
        if (uchMDC) {
			/* si before mnemonics */
	        PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJAmtDiscMnemSI, pszSISym, aszTranMnem, uchRate, lAmount);
	    } else {
	        PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJAmtDiscMnemSI, aszTranMnem, pszSISym, uchRate, lAmount);
	    }
    }
}

/*
*===========================================================================
** Format  : VOID  PrtEJCoupon( ITEMCOUPON *pItem )
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
VOID  PrtEJCoupon( ITEMCOUPON *pItem )
{
	/* SI symnbol, before mnemonics 21RFC05437 */
	if ( CliParaMDCCheck(MDC_TAX2_ADR, EVEN_MDC_BIT1) ) {
		TCHAR  aszSpecMnem[PARA_SPEMNEMO_LEN * 2 + 1] = {0};  /*  times 2 for possible double in PrtGetSISymCoupon() */

	    if (PrtGetSISymCoupon(aszSpecMnem, pItem)) {
			if (CliParaMDCCheck(MDC_TAX2_ADR, EVEN_MDC_BIT2) ) {
		    	/* -- send mnemoncis and amount -- */
	    		PrtPrintf( PMG_PRT_JOURNAL, aszPrtEJAmtMnemSI, aszSpecMnem, pItem->aszMnemonic, pItem->lAmount );
	    	} else {
			    /* -- send mnemoncis and amount -- */
    			PrtPrintf( PMG_PRT_JOURNAL, aszPrtEJAmtMnemSI, pItem->aszMnemonic, aszSpecMnem, pItem->lAmount );
    		}
		}
	} else {
	    /* -- send mnemoncis and amount -- */
    	PrtPrintf( PMG_PRT_JOURNAL, aszPrtEJAmtMnem, pItem->aszMnemonic, pItem->lAmount );
	}
}
/*
*===========================================================================
** Format  : VOID  PrtEJWaiter(USHORT usWaiID);
*               
*   Input  : USHORT usWaiID      - waiter ID 
*   Output : none
*   InOut  : none
*
** Return  : none
*
** Synopsis: This function prints Waiter line.
*            
*===========================================================================
*/
VOID  PrtEJOrderDec(USHORT usAdr, DCURRENCY lAmount, BOOL fsType)
{
	CONST TCHAR *aszPrtEJOrderDecMnemMnem = _T("%.s\t %s");    /* 8 characters JHHJ*/  /* mnem. and mnem. */
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};

    /* -- get transaction mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);
	RflCleanupMnemonic (aszTranMnem);
	
    PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJOrderDecMnemMnem, aszTranMnem, _T(" "));
}

/*
*===========================================================================
** Format  : VOID  PrtEJAmtSym(UCHAR uchAdr, DCURRENCY lAmount, BOOL fsType);
*               
*   Input  : UCHAR       uchAdr,      -Transaction mnemonics address
*            DCURRENCY   lAmount      -amount
*            SHORT       fsType       -character type
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
VOID  PrtEJAmtSym(USHORT usAdr, DCURRENCY lAmount, BOOL fsType)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0}; /* PARA_... defined in "paraequ.h" */
	TCHAR  aszSpecAmtS[PARA_SPEMNEMO_LEN + PRT_AMOUNT_LEN + 1] = {0};  /* spec. mnem & amount save area */
	TCHAR  aszSpecAmtD[(PARA_SPEMNEMO_LEN + PRT_AMOUNT_LEN) * 2 + 1] = {0}; /* spec. mnem. & amount double wide */
    USHORT usMnemLen, usAmtSLen, usAmtDLen;     

    /* -- get transaction mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);
	RflCleanupMnemonic (aszTranMnem);

    /* -- adjust native mnemonic and amount sign(+.-) -- */
    PrtAdjustNative(aszSpecAmtS, lAmount);

    /* -- convert to double -- */
    PrtDouble(aszSpecAmtD, ((PARA_SPEMNEMO_LEN + PRT_AMOUNT_LEN) * 2 + 1), aszSpecAmtS);

    usMnemLen = tcharlen(aszTranMnem);
    usAmtSLen = tcharlen(aszSpecAmtS);
    usAmtDLen = tcharlen(aszSpecAmtD);

    if ( fsType == PRT_SINGLE )  {
        PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJMnemMnem, aszTranMnem, aszSpecAmtS);
        return;
    }

    if ( usMnemLen+usAmtDLen+1 <= usPrtEJColumn ) {
        /* -- double print -- */
        PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJMnemMnem, aszTranMnem, aszSpecAmtD);
        return;
    }

    if ( usMnemLen+usAmtSLen+1 <= usPrtEJColumn ) {
        /* -- single print -- */
        PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJMnemMnem, aszTranMnem, aszSpecAmtS);
    } else {
        if ( usAmtDLen <= usPrtEJColumn ) {
            /* -- double print -- */
            PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJMnemMnem, aszTranMnem, aszSpecAmtD);
        } else {
            /* -- single print -- */
            PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJMnemMnem, aszTranMnem, aszSpecAmtS);
        }
    }
}

/*
*===========================================================================
** Format   : VOID PrtEJTblPerson( USHORT usTblNo,
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
VOID PrtEJTblPerson( USHORT usTblNo, USHORT usNoPerson, TCHAR uchSeatNo, SHORT sWidthType )
{
	TCHAR   aszTranMnem[ PARA_TRANSMNEMO_LEN + 1 ] = {0}; /* PARA_... defined in "paraequ.h" */
    TCHAR   aszSpecMnem[ PARA_SPEMNEMO_LEN + 1 ] = {0};   /* PARA_... defined in "paraequ.h" */
    TCHAR   aszLeadThru[ PARA_LEADTHRU_LEN + 1 ] = {0};
    TCHAR   aszTblNo[ PRT_ID_LEN + 1 ] = {0};
    TCHAR   aszNoPer[ PRT_ID_LEN + 1 ] = {0};
    TCHAR   aszTransNo[ PRT_ID_LEN * 2 + 1 ] = {0};

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

    if (uchSeatNo) {
        /* --- set up sepcial mnemonic of seat no, and seat no --- */
        _itot( uchSeatNo, aszNoPer, 10 );
		RflGetSpecMnem( aszTranMnem, SPC_SEAT_ADR );    /* seat no. mnemonic */
    } else if ( (usNoPerson != 0) && (!CliParaMDCCheck(MDC_TRANNUM_ADR, EVEN_MDC_BIT3))) {
        /* --- set up sepcial mnemonic of no. of person, and no. of person --- */
        _itot( usNoPerson, aszNoPer, 10 );
		RflGetTranMnem( aszTranMnem, TRN_PSN_ADR );
    }

    if ( sWidthType == PRT_DOUBLE ) {   /* print with double width */
        /* --- print table no. with double width ( unique tran no. ) --- */
        if ( usTblNo && aszLeadThru[0] != 0 ) {
            PrtPrintf( PMG_PRT_JOURNAL, aszPrtEJTransNo, aszLeadThru, aszTransNo );
        }

        /* --- print no. of person with single width --- */
        if ( (usNoPerson || uchSeatNo) && aszTranMnem[0] != 0 ) {
            PrtPrintf( PMG_PRT_JOURNAL, aszPrtEJPerson, aszTranMnem, aszNoPer );
        }
    } else {                            /* print with single width */
        if ( (usTblNo || usNoPerson || uchSeatNo) && aszTranMnem[0] != 0 ) {
            /* --- print table no. and no. of person with single width --- */
            PrtPrintf( PMG_PRT_JOURNAL, aszPrtEJTblPerson, aszSpecMnem, aszTblNo, aszTranMnem, aszNoPer );
        }
    }
}

/*
*===========================================================================
** Format   : VOID PrtEJCustomerName( UCHAR *pszCustomerName )
*
*   Input   : UCHAR *pszCustomerName    -   address of customer name buffer
*   Output  : none
*   InOut   : none
** Return   : none
*
** Synopsis : This function prints customer name (max. 16 chara).
*===========================================================================
*/
VOID PrtEJCustomerName( TCHAR *pszCustomerName )
{
    if ( *pszCustomerName == _T('\0') ) {
        return;
    }

    if ( *( pszCustomerName + NUM_NAME - 2 ) == PRT_DOUBLE ) {
        *( pszCustomerName + NUM_NAME - 2 ) = _T('\0');
    }

    PrtPrintf( PMG_PRT_JOURNAL, aszPrtEJCustomerName, pszCustomerName );
}

/*
*===========================================================================
** Format  : VOID   PrtEJMultiSeatNo(TRANINFORMATION *pTran);
*
*   Input  : ITEMSALES *pItem           -seat no
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function formats seat no. line to . R3.1
*===========================================================================
*/
VOID    PrtEJMultiSeatNo(TRANINFORMATION *pTran)
{
	TCHAR   aszPrintBuff[PRT_EJCOLUMN +1] = {0};
    TCHAR   aszSpecMnem[ PARA_SPEMNEMO_LEN + 1 ] = {0};   /* PARA_... defined in "paraequ.h" */
    USHORT  i, usPos;

    if (pTran->TranGCFQual.auchFinSeat[0] == 0) {
        return;
    }

	RflGetSpecMnem( aszSpecMnem, SPC_SEAT_ADR );    /* seat no. mnemonic */
    usPos = tcharlen(aszSpecMnem);

    _tcsncpy(&aszPrintBuff[0], aszSpecMnem, usPos);

    /* ---- set multiple seat no. ---- */
    for (i = 0; i < NUM_SEAT; i++) { //SR206
        if (pTran->TranGCFQual.auchFinSeat[i]) {
            aszPrintBuff[usPos++] = (TCHAR)0x20;
            aszPrintBuff[usPos++] = (TCHAR)(pTran->TranGCFQual.auchFinSeat[i] | 0x30);
        } else {
            break;
        }
    }

    aszPrintBuff[usPos++] = _T('\0');

    PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJMultiSeat, aszPrintBuff);

}


/*
*===========================================================================
** Format  : VOID  PrtEJTranNum(UCHAR uchAdr, ULONG ulNumber);
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
VOID  PrtEJTranNum(USHORT usTranAdr, ULONG ulNumber)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};/* PARA_... defined in "paraequ.h" */

    /* -- get transaction mnemonics -- */
	RflGetTranMnem(aszTranMnem, usTranAdr);

    PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJMnemNum, aszTranMnem, ulNumber);

}



/*
*===========================================================================
** Format  : VOID  PrtEJWaiter(USHORT usWaiID);
*               
*   Input  : USHORT usWaiID      - waiter ID 
*   Output : none
*   InOut  : none
*
** Return  : none
*
** Synopsis: This function prints Waiter line.
*            
*===========================================================================
*/
VOID  PrtEJWaiter(ULONG usWaiID)
{
    if (usWaiID != 0) {
		TCHAR  aszWaiMnem[PARA_SPEMNEMO_LEN + 1] = {0};  /* PARA_... defined in "paraequ.h" */

        /* -- get waiter mnemonics -- */
		RflGetSpecMnem(aszWaiMnem, SPC_CAS_ADR);
        PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJWaiter, aszWaiMnem, usWaiID);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtEJForeign1(DCURRENCY lForeign, UCHAR uchAdr, UCHAR fbStatus);
*   Input  : DCURRENCY  lForeign,   -Foreign tender amount
*            UCHAR      uchAdr      -Foreign symbol address
*            UCHAR      fbStatus    -Foreign currency decimal point status
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
VOID  PrtEJForeign1(DCURRENCY lForeign, UCHAR uchAdr, UCHAR fbStatus)
{
	TCHAR  aszFMnem[PARA_SPEMNEMO_LEN + 1] = {0};  /* PARA_... defined in "paraequ.h" */
	TCHAR  aszFAmt[PRT_EJCOLUMN + 1] = {0};
    USHORT usStrLen;
    SHORT  sDecPoint;

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

    /* -- get foreign symbol -- */
	RflGetSpecMnem(aszFMnem, uchAdr);
    /* -- adjust sign ( + , - ) -- */
    if (lForeign < 0) { 
        aszFAmt[0] = _T('-');
        _tcsncpy(&aszFAmt[1], aszFMnem, PARA_SPEMNEMO_LEN);
    } else {
        _tcsncpy(aszFAmt, aszFMnem, PARA_SPEMNEMO_LEN);
    }

    /* -- get string length -- */
    usStrLen = tcharlen(aszFAmt);

    /* -- format foreign amount -- */
	RflSPrintf(&aszFAmt[usStrLen], TCHARSIZEOF(aszFAmt) - usStrLen, aszPrtEJForeign1, sDecPoint, lForeign);

	PrtPrint(PMG_PRT_JOURNAL, aszFAmt, tcharlen(aszFAmt)); /* ### Mod (2171 for Win32) V1.0 */
}

/*
*===========================================================================
** Format  : VOID  PrtEJForeign2(ULONG ulRate);
*
*   Input  : ULONG  ulRate          -rate
*            
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints foreign tender line.
*            
*===========================================================================
*/
VOID  PrtEJForeign2(ULONG ulRate, UCHAR fbStatus2)
{
    SHORT sDecPoint;

    /* 6 digit decimal point for Euro, V3.4 */
    if (fbStatus2 & ODD_MDC_BIT0) {
        sDecPoint = PRT_6DECIMAL;
    } else {
        sDecPoint = PRT_5DECIMAL;
    }

    PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJForeign2, sDecPoint, ulRate);
}

/*
*===========================================================================
** Format  : VOID  PrtEJEuro(uchAdr1, LONG lForeign, UCHAR uchAdr2, ULONG ulRate)
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
VOID  PrtEJEuro(UCHAR uchAdr1, DCURRENCY lForeign, UCHAR uchAdr2, ULONG ulRate, UCHAR fbStatus2)
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

    PrtPrintf(PMG_PRT_JOURNAL, aszPrtTHEuro1, aszFMnem1, lForeign, aszFMnem2, sDecPoint, ulRate);
}

/*
*===========================================================================
** Format  : VOID   PrtEJSeatNo(ITEMSALES *pItem);
*
*   Input  : ITEMSALES *pItem           -seat no
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function formats seat no. line to . R3.1
*===========================================================================
*/
VOID    PrtEJSeatNo(TCHAR uchSeatNo)
{
    if (uchSeatNo != 0) {
		TCHAR   aszSpecMnem[ PARA_SPEMNEMO_LEN + 1 ] = {0};   /* PARA_... defined in "paraequ.h" */

		RflGetSpecMnem( aszSpecMnem, SPC_SEAT_ADR );    /* seat no. mnemonic */
		PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJSeat, aszSpecMnem, uchSeatNo);
    }
}


/*
*===========================================================================
** Format  : VOID  PrtEJQty(ITEMSALES *pItem);
*
*   Input  : ITEMSALES *pItem     -item information
*            
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints Qty and unit price line if the item's quantity
*            is not equal to 1. lQTY is in units of thousandths to allow for scalable
*            items so 1000L is 1 unit for non-scalable items.
*            
*===========================================================================
*/
VOID  PrtEJQty(ITEMSALES *pItem)
{
    if (labs(pItem->lQTY) != PLU_BASE_UNIT) {
		DCURRENCY  lPrice = 0;
		USHORT     i;
		USHORT     usNoOfChild;
		TCHAR      auchDummy[NUM_PLU_LEN] = {0};

        usNoOfChild = pItem->uchCondNo + pItem->uchPrintModNo + pItem->uchChildNo;

        for ( i = pItem->uchChildNo; i < usNoOfChild; i++) {
            if (_tcsncmp(pItem->Condiment[ i ].auchPLUNo, auchDummy, NUM_PLU_LEN) != 0 ) {
                lPrice += pItem->Condiment[i].lUnitPrice;
            }
        }
        if (pItem->uchPPICode) {
            lPrice = lPrice * (pItem->lQTY / PLU_BASE_UNIT);
            lPrice += pItem->lProduct;      /* print whole at ppi sales */

            PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJPPIQuant, pItem->lQTY / PLU_BASE_UNIT, RflLLabs(lPrice), labs(pItem->lQTY / PLU_BASE_UNIT));

        } else if (pItem->uchPM) {
            /* 2172 */
            lPrice += pItem->lUnitPrice;    /* print unit price at normal sales */

            PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJPPIQuant, pItem->lQTY / PLU_BASE_UNIT, RflLLabs(lPrice), pItem->uchPM);
            
        } else {
            lPrice += pItem->lUnitPrice;    /* print unit price at normal sales */

            			//SR 144 perform MDC check to determine if Qty prints above item line or on the same line
			if(!CliParaMDCCheck(MDC_QTY2LINE_ADR, EVEN_MDC_BIT3)){
				PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJQuant, pItem->lQTY / PLU_BASE_UNIT, lPrice);
			}
		}
    }
}

/*
*===========================================================================
** Format  : VOID  PrtEJQty(ITEMSALES *pItem);
*
*   Input  : ITEMSALES *pItem     -item information
*            
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints Qty and unit price line.
*            
*===========================================================================
*/
VOID  PrtEJLinkQty(ITEMSALES *pItem)
{
    if (labs(pItem->lQTY) != PLU_BASE_UNIT) {
		DCURRENCY  lPrice = pItem->Condiment[0].lUnitPrice;

        PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJQuant, pItem->lQTY / PLU_BASE_UNIT, lPrice);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtEJWeight(TRANINFORMATION  *pTran, ITEMSALES *pItem);
*
*   Input  : TRANINFORMATION  *pTran        -Transaction information address
*            ITEMSALES *pItem               -Item information address
*            
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints weight and unit price line.
*            
*===========================================================================
*/
VOID  PrtEJWeight(TRANINFORMATION  *pTran, ITEMSALES  *pItem)
{
	TCHAR  aszWeightSym[PARA_SPEMNEMO_LEN + 1] = {0}; /* PARA_... defined in "paraequ.h" */
	TCHAR  aszManuMnem[PARA_TRANSMNEMO_LEN + 1] = {0}; /* PARA_... defined in "paraequ.h" */
    SHORT  sDecPoint;                           /* decimal point */
    LONG   lModQty;                             /* modified qty */
    
    /* -- get scalable symbol and adjust weight -- */
    PrtGetScale(aszWeightSym, &sDecPoint, &lModQty, pTran, pItem);

    /* -- get Manual weight mnemonics -- */
    if (pItem->fsPrintStatus & PRT_MANUAL_WEIGHT) {
		RflGetTranMnem(aszManuMnem, TRN_MSCALE_ADR);
    }

    /* -- send "weight " and Manual weight mnemonics -- */
    PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJWeight1, sDecPoint, (DWEIGHT)lModQty, aszWeightSym, aszManuMnem);

    PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJWeight2, (DCURRENCY)pItem->lUnitPrice, aszWeightSym);
}

/*
*===========================================================================
** Format  : VOID  PrtEJChild(UCHAR uchAdj, UCHAR *pszMnem);
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
VOID  PrtEJChild(UCHAR uchAdj, TCHAR *pszMnem)
{
    if (uchAdj != 0) {
		TCHAR    aszAdjMnem[PARA_ADJMNEMO_LEN + 1] = {0};
		TCHAR    aszPLUMnem[PARA_PLU_LEN + 1] = {0};

        /* --get adjective mnemocnics -- */
		RflGetAdj(aszAdjMnem,  uchAdj);
        
        /* 2172 */
        _tcsncpy(aszPLUMnem, pszMnem, PARA_PLU_LEN);
        aszPLUMnem[PRT_MNEMONIC_CLIP] = 0;  /* clip mnemonic to RJ column length */

        /* -- send adjective mnemonics and child plu mnemonics -- */
        PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJChild, aszAdjMnem, aszPLUMnem);
    } else {
        /* print only child mnemonic, 2172 */
        PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJChild2, pszMnem);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtEJChild(UCHAR uchAdj, UCHAR *pszMnem, DCURRENCY lPrice);
*
*   Input  : UCHAR     uchAdj       - Adjective number
*            UCHAR     *pszMnem     - Adjective mnemonics address
*            DCURRENCY lPrice       - PLU price
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints child plu line.
*            
*===========================================================================
*/
VOID  PrtEJLinkPLU(USHORT fsModified, UCHAR uchAdj, TCHAR *pszMnem, DCURRENCY lPrice)
{
    if (uchAdj != 0) {
		TCHAR    aszAdjMnem[PARA_ADJMNEMO_LEN + 1] = {0};
		TCHAR    aszPLUMnem[PARA_PLU_LEN + 1] = {0};

        /* --get adjective mnemocnics -- */
		RflGetAdj(aszAdjMnem,  uchAdj);
        
        /* 2172 */
        _tcsncpy(aszPLUMnem, pszMnem, PARA_PLU_LEN);
        aszPLUMnem[PRT_MNEMONIC_CLIP] = 0;  /* clip mnemonic to RJ column length */
        
        /* -- send adjective mnemonics and child plu mnemonics -- */
        if (fsModified) {
            PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJModLinkPLU1, aszAdjMnem, aszPLUMnem, lPrice);
        } else {
            PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJLinkPLU1, aszAdjMnem, aszPLUMnem, lPrice);
        }
    } else {
        /* print only child mnemonic, 2172 */
        if (fsModified) {
            PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJModLinkPLU2, pszMnem, lPrice);
        } else {
            PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJLinkPLU2, pszMnem, lPrice);
        }
    }
}

/*
*===========================================================================
** Format  : VOID  PrtEJItems(ITEMSALES  *pItem);
*
*   Input  : ITEMSALES  *pItem    -structure "itemsales" pointer
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints adjective, noun mnemo.,  print modifier, conidiment line.
*===========================================================================
*/
VOID  PrtEJItems(ITEMSALES  *pItem)
{
	TCHAR  aszItem[PRT_MAXITEM + PRT_EJCOLUMN] = {0};   /* "+PRT_EJCOLUMN" for PrtGet1Line */
    DCURRENCY   lProduct;
    TCHAR  *pszStart;

    pszStart = aszItem;

    /* -- set item mnenemonics to buffer -- */
    PrtSetItem(aszItem, pItem, &lProduct, PRT_ALL_ITEM, 0);

    for (;;) {
		TCHAR  *pszEnd;

        /* -- get 1 line -- */
        if ( PRT_LAST == PrtGet1Line(pszStart, &pszEnd, (USHORT)(usPrtEJColumn - PRT_AMOUNTLEN)) ) { /* ### Mod (2171 for Win32) V1.0 */
            break;
        }

        /* send adjective, noun mnemo, print modifier, condiment */
        PrtPrint(PMG_PRT_JOURNAL, pszStart, (USHORT)(pszEnd - pszStart)); /* ### Mod (2171 for Win32) V1.0 */

        /* -- "1" for space -- */
        pszStart = pszEnd + 1;
    }

    /* -- modified discount -- */
    if ((pItem->uchMinorClass == CLASS_DEPTMODDISC) 
        || (pItem->uchMinorClass == CLASS_PLUMODDISC) 
        || (pItem->uchMinorClass == CLASS_SETMODDISC)
        || (pItem->uchMinorClass == CLASS_OEPMODDISC)) {

		//SR 144 perform MDC check to determine if Qty prints above item line or on the same line
		if(!CliParaMDCCheck(MDC_QTY2LINE_ADR, EVEN_MDC_BIT3)){
			PrtPrintf(PMG_PRT_JOURNAL,  aszPrtEJModAmtMnem, pszStart, lProduct);
		} else {
			PrtPrintf(PMG_PRT_JOURNAL,  aszPrtEJModAmtMnem1Line, pItem->lQTY, (DCURRENCY)pItem->lUnitPrice, pszStart, lProduct);
		}

    } else {

		//SR 144 perform MDC check to determine if Qty prints above item line or on the same line
		if(!CliParaMDCCheck(MDC_QTY2LINE_ADR, EVEN_MDC_BIT3)){
			PrtPrintf(PMG_PRT_JOURNAL,  aszPrtEJAmtMnem, pszStart, lProduct);
		} else {
			PrtPrintf(PMG_PRT_JOURNAL,  aszPrtEJAmtMnem1Line, pItem->lQTY / PLU_BASE_UNIT, (DCURRENCY)pItem->lUnitPrice, pszStart, lProduct);
		}
    }
}

/*
*===========================================================================
** Format  : VOID  PrtEJZeroAmtMnem(UCHAR uchAddress, DCURRENCY lAmount);
*
*   Input  : UCHAR      uchAddress    -transaction mnemonics address
*            DCURRENCY  lAmount       -change
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints change line.
*            
*===========================================================================
*/
VOID  PrtEJZeroAmtMnem(USHORT usAddress, DCURRENCY lAmount)
{
    if (lAmount != 0) {
        PrtEJAmtMnem(usAddress, lAmount);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtEJDiscItems(ITEMSALES  *pItem);
*
*   Input  : ITEMSALES  *pItem    -structure "itemsales" pointer
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints adjective, noun mnemo.,  print modifier, conidiment line.
*===========================================================================
*/
VOID  PrtEJDiscItems(ITEMSALES  *pItem)
{
	TCHAR  aszItem[PRT_MAXITEM + PRT_EJCOLUMN] = {0};
    DCURRENCY   lProduct;
    TCHAR  *pszStart;

    /* -- set item mnenemonics to buffer -- */
    PrtSetItem(aszItem, pItem, &lProduct, PRT_ALL_ITEM, 0);
    pszStart = aszItem;
    for (;;) {
		TCHAR  *pszEnd;

        /* -- get 1 line -- */
        if ( PRT_LAST == PrtGet1Line(pszStart, &pszEnd, (USHORT)(usPrtEJColumn - PRT_AMOUNTLEN)) ) { /* ### Mod (2171 for Win32) V1.0 */
            break;
        }

        /* send adjective, noun mnemo, print modifier, condiment */
        PrtPrint(PMG_PRT_JOURNAL, pszStart, (USHORT)(pszEnd - pszStart)); /* ### Mod (2171 for Win32) V1.0 */

        /* -- "1" for space -- */
        pszStart = pszEnd + 1;
    }

    if (pItem->uchRate == 0) {    /* not print rate */
        PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJAmtMnem, pszStart, pItem->lDiscountAmount);
    } else {                      /* print rate */
        PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJAmtDiscMnem, pszStart, pItem->uchRate, pItem->lDiscountAmount);
    }
}                       

/*
*===========================================================================
** Format  : VOID  PrtEJGuest(USHORT usGuestNo, UCHAR uchCDV);
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
VOID  PrtEJGuest(USHORT usGuestNo, UCHAR uchCDV)
{
    if (usGuestNo != 0) {                       /* GCF# 0, doesnot print */
		TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };/* PARA_... defined in "paraequ.h" */

		/* -- get transaction mnemonics -- */
		RflGetTranMnem(aszTranMnem, TRN_GCNO_ADR);
		RflCleanupMnemonic (aszTranMnem);
		if (aszTranMnem[0] != 0) {
			if (uchCDV == 0) {
				PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJGuest_WOCD, aszTranMnem, usGuestNo);
			} else {
				PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJGuest_WTCD, aszTranMnem, usGuestNo, (USHORT)uchCDV);
			}
		}
    }
}


/*
*===========================================================================
** Format  : VOID  PrtEJMulChk(USHORT usGuestNo, UCHAR uchCDV);
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
VOID  PrtEJMulChk(USHORT usGuestNo, UCHAR uchCDV)
{
    if (usGuestNo != 0) {                       /* GCF# 0, doesnot print */
		TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0}; /* PARA_... defined in "paraequ.h" */

		/* -- get transaction mnemonics -- */
		RflGetTranMnem(aszTranMnem, TRN_CKPD_ADR);
		RflCleanupMnemonic (aszTranMnem);

		if (uchCDV == 0) {
			PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJGuest_WOCD, aszTranMnem, usGuestNo);
		} else {
			PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJGuest_WTCD, aszTranMnem, usGuestNo, (USHORT)uchCDV);
		}
    }
}

/*
*===========================================================================
** Format  : VOID  PrtEJWaiTaxMod(USHORT usID, USHORT fsTax, UCHAR  fbMod);
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
VOID  PrtEJWaiTaxMod(ULONG ulID, USHORT  fsTax, USHORT  fbMod)
{
    TCHAR  aszTaxMod[PRT_EJCOLUMN + 1] = {0};
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
        PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJWaiTaxMod, aszWaiMnem, ulID, aszTaxMod);
    } else if (ulID != 0) {
        PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJWaiter, aszWaiMnem, ulID);
    } else if (sRet == PRT_WITHDATA) {
        PrtPrint(PMG_PRT_JOURNAL, aszTaxMod, tcharlen(aszTaxMod)); /* ### Mod (2171 for Win32) V1.0 */
    }
}

/*
*===========================================================================
** Format  : VOID  PrtEJMnemCount(UCHAR uchAdr, SHORT sCount);
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
VOID  PrtEJMnemCount(USHORT usTranAdr, SHORT sCount)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};  /* PARA_... defined in "paraequ.h" */

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usTranAdr);
	RflCleanupMnemonic (aszTranMnem);

    /* -- send mnemoncis and amount -- */
    PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJMnemCount, aszTranMnem, sCount);

}

/*
*===========================================================================
** Format  : VOID  PrtEJTrail1(TRANINFORMATION  *pTran, ULONG  ulStReg);
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*            ULONG           ulStReg   -store reg number
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints normal trailer 1st line.
*===========================================================================
*/
VOID  PrtEJTrail1(TRANINFORMATION *pTran, ULONG  ulStReg)
{
	extern CONST TCHAR FARCONST  aszCasWaiID[];
	CONST TCHAR   *aszPrtEJTrail1  = _T("%s %s\t%04lu-%03lu"); /* trailer 1st line */

    TCHAR  aszMnem[PARA_SPEMNEMO_LEN + 1] = {0};
    ULONG  ulID = 0;
    TCHAR  aszID[8 + 1] = {0, 0, 0, 0};
	TCHAR  aszName[PARA_CASHIER_LEN + 1] = {0};  

    if ( PRT_CASHIER == PrtChkCasWai(aszMnem, pTran->TranModeQual.ulCashierID, pTran->TranModeQual.ulWaiterID) ) {
        ulID = pTran->TranModeQual.ulCashierID;
        _tcsncpy(aszName, pTran->TranModeQual.aszCashierMnem, PRT_CASNAME_LEN);
    } else {
        ulID = pTran->TranModeQual.ulWaiterID;
        _tcsncpy(aszName, pTran->TranModeQual.aszWaiterMnem, PRT_CASNAME_LEN);
    }

    /* -- convert ID to ascii -- */
	if (ulID != 0)
		RflSPrintf(aszID, TCHARSIZEOF(aszID), aszCasWaiID, ulID);

    PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJTrail1, aszID, aszName, ulStReg / 1000, ulStReg % 1000 );
}

/*
*===========================================================================
** Format  : VOID  PrtEJTrail2(TRANINFORMATION  *pTran);
*
*   Input  : TRANINFORMATION *pTran         -transaction information
*
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints trailer 2nd line.
*===========================================================================
*/
VOID PrtEJTrail2(TRANINFORMATION  *pTran)
{
	TCHAR   aszDate[PRT_DATETIME_LEN + 1] = {0};

    PrtGetDate(aszDate, TCHARSIZEOF(aszDate), pTran);   /* get date time */

    PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJTrail2, pTran->TranCurQual.usConsNo, aszDate);
}


/*
*===========================================================================
** Format  : VOID  PrtEJEtkCode(ITEMMISC  *pItem, SHORT sTpye);
*
*   Input  : ITEMMISC   *pItem      -Item Data Address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints ETK employee code and job code.
*===========================================================================
*/
VOID  PrtEJEtkCode(ITEMMISC  *pItem, SHORT sType)
{
	extern CONST TCHAR FARCONST  aszEtkJobCode[];
	CONST TCHAR   *aszPrtEJETKCode = _T("%-16s\n%8.8Mu\t%s"); /* add at R3.1 */
	TCHAR   aszJobCode[12] = {0};

    if ( ( sType == PRT_WTIN) && ( pItem->uchJobCode != 0 ) ) {
        RflSPrintf(aszJobCode, TCHARSIZEOF(aszJobCode), aszEtkJobCode, pItem->uchJobCode);
    }

    PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJETKCode, pItem->aszMnemonic, pItem->ulEmployeeNo, aszJobCode);
}

/*
*===========================================================================
** Format  : VOID  PrtEJEtkJobTimeIn(TRANINFORMATION  *pTran, ITEMMISC  *pItem, SHORT sType);
*
*   Input  : TRANINFORMATION    *pTran  -Transaction Information Address
*            ITEMMISC   *pItem          -Item Data Address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints ETK employee code and job code.
*===========================================================================
*/
VOID  PrtEJJobTimeIn(TRANINFORMATION  *pTran, ITEMMISC  *pItem, SHORT sType)
{
	TCHAR   aszDate[PRT_DATETIME_LEN + 1] = {0};

    if ( sType == PRT_WTIN ) {
        /* -- get job time-in code -- */
        PrtGetJobInDate(aszDate, TCHARSIZEOF(aszDate), pTran, pItem);    /* get date time */
    } else {
        tcharnset(aszDate, _T('*'), PRT_DATETIME_LEN);
    }
  
    PrtPrint(PMG_PRT_JOURNAL, aszDate, tcharlen(aszDate)); /* ### Mod (2171 for Win32) V1.0 */
}

/*
*===========================================================================
** Format  : VOID  PrtEJEtkJobTimeOut(TRANINFORMATION  *pTran, ITEMMISC  *pItem);
*
*   Input  : TRANINFORMATION    *pTran  -Transaction Information Address
*            ITEMMISC   *pItem          -Item Data Address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints ETK time-out time.
*===========================================================================
*/
VOID  PrtEJJobTimeOut(TRANINFORMATION  *pTran, ITEMMISC  *pItem)
{
	TCHAR   aszDate[PRT_DATETIME_LEN + 1] = {0};

    /* -- get job time-in code -- */
    PrtGetJobOutDate(aszDate, TCHARSIZEOF(aszDate), pTran, pItem);     /* get date time */
  
    PrtPrint(PMG_PRT_JOURNAL, aszDate, tcharlen(aszDate)); /* ### Mod (2171 for Win32) V1.0 */
}

/*
*===========================================================================
** Format  : VOID  PrtEJInit(TRANINFORMATION  *pTran)
*
*   Input  : TRANINFORMATION  *pTran    -transaction information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function initialize print buffer for E/J, initialize
*            write pointer and make header.
*===========================================================================
*/
VOID  PrtMaintEJInit(ULONG ulCashierID)
{
    DATE_TIME   DT;                         /* date & time */
	ULONG       ulStoreRegNo;
	USHORT      usConsNo;

    /* set transaction information */
	{
		PARASTOREGNO   StRegNoRcvBuff = {0};

		StRegNoRcvBuff.uchMajorClass = CLASS_PARASTOREGNO;    /* get store/ reg No. */
		StRegNoRcvBuff.uchAddress = SREG_NO_ADR;
		CliParaRead(&StRegNoRcvBuff);
	    
		ulStoreRegNo = (ULONG) StRegNoRcvBuff.usStoreNo * 1000L + (ULONG) StRegNoRcvBuff.usRegisterNo;
	}

	{
	    PARASPCCO ParaSpcCo = {0};

		ParaSpcCo.uchMajorClass = CLASS_PARASPCCO;
		ParaSpcCo.uchAddress = SPCO_CONSEC_ADR;                    
		CliParaRead(&ParaSpcCo);                                     /* call ParaSpcCoRead() */
		usConsNo = ParaSpcCo.usCounter;
	}
    /* -- get date time -- */
    PifGetDateTime(&DT);

    ((EJT_HEADER *)auchPrtEJBuffer)->usTermNo       = (USHORT)(ulStoreRegNo % 1000L);
    ((EJT_HEADER *)auchPrtEJBuffer)->usConsecutive  = usConsNo;
    ((EJT_HEADER *)auchPrtEJBuffer)->usSeqNo        = 1;
    ((EJT_HEADER *)auchPrtEJBuffer)->usMode         = 0;
    ((EJT_HEADER *)auchPrtEJBuffer)->usOpeNo        = ulCashierID;
    ((EJT_HEADER *)auchPrtEJBuffer)->fsTranFlag     = 0;
    ((EJT_HEADER *)auchPrtEJBuffer)->usGCFNo        = 0;
    ((EJT_HEADER *)auchPrtEJBuffer)->usDate         = DT.usMonth*100 + DT.usMDay;
    ((EJT_HEADER *)auchPrtEJBuffer)->usTime         = DT.usHour*100 + DT.usMin;
    ((EJT_HEADER *)auchPrtEJBuffer)->usEjSignature  = EJT_HEADER_SIGNATURE;

    usPrtEJOffset = sizeof(EJT_HEADER);
}

VOID  PrtForceEJInit(TRANINFORMATION  *pTran)
{
    DATE_TIME   DT;                         /* date & time */

    /* -- get date time -- */
    PifGetDateTime(&DT);

	memset (auchPrtEJBuffer, 0, sizeof (EJT_HEADER));

	if (pTran) {
		ULONG       ulID;

		/* -- cashier or waiter ? See function PrtChkCasWai() -- */
		if ( pTran->TranModeQual.ulWaiterID ) {
			ulID = pTran->TranModeQual.ulWaiterID;
		} else {
			ulID = pTran->TranModeQual.ulCashierID;
		}

		((EJT_HEADER *)auchPrtEJBuffer)->usTermNo       = (USHORT)(pTran->TranCurQual.ulStoreregNo % 1000L);
		((EJT_HEADER *)auchPrtEJBuffer)->usConsecutive  = pTran->TranCurQual.usConsNo;
		((EJT_HEADER *)auchPrtEJBuffer)->usGCFNo        = pTran->TranGCFQual.usGuestNo;
		((EJT_HEADER *)auchPrtEJBuffer)->usOpeNo        = ulID;
	} else {
		PARASTOREGNO   StRegNoRcvBuff;

        StRegNoRcvBuff.uchMajorClass = CLASS_PARASTOREGNO;    /* get store/ reg No. */
        StRegNoRcvBuff.uchAddress = SREG_NO_ADR;
        CliParaRead(&StRegNoRcvBuff);

		((EJT_HEADER *)auchPrtEJBuffer)->usConsecutive  = MaintCurrentSpcCo(SPCO_CONSEC_ADR);
        ((EJT_HEADER *)auchPrtEJBuffer)->usTermNo       = StRegNoRcvBuff.usRegisterNo;
	}

    ((EJT_HEADER *)auchPrtEJBuffer)->usSeqNo        = 1;
    ((EJT_HEADER *)auchPrtEJBuffer)->usDate         = DT.usMonth*100 + DT.usMDay;
    ((EJT_HEADER *)auchPrtEJBuffer)->usTime         = DT.usHour*100 + DT.usMin;
    ((EJT_HEADER *)auchPrtEJBuffer)->usEjSignature  = EJT_HEADER_SIGNATURE;

    usPrtEJOffset = sizeof(EJT_HEADER);
}

VOID  PrtEJInit(TRANINFORMATION  *pTran)
{
    if (usPrtEJOffset < sizeof(EJT_HEADER) || ((EJT_HEADER *)auchPrtEJBuffer)->usEjSignature != EJT_HEADER_SIGNATURE) {
		PrtForceEJInit (pTran);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtEJWrite(VOID  *pWrite, USHORT  usWriteLen)
*
*   Input  : pWrite             -write pointer
*            usWriteLen         -write length 
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function write data to print module's local data area
*            "auchPrtEJBuffer[]".
*===========================================================================
*/
VOID  PrtEJWrite(VOID  *pWrite, USHORT usWriteLen)
{
	TCHAR  aszWork1[PRT_THCOLUMN * (NUM_CPRSPCO_EPT + 1)] = {0};
    TCHAR  aszWork2[PRT_THCOLUMN * (NUM_CPRSPCO_EPT + 1)] = {0};
    USHORT  usStrLen;

    _tcsncpy(aszWork1, pWrite, usWriteLen);
    aszWork1[usWriteLen] = _T('\0');                /* make ASCII string */

    usStrLen = RflStrAdjust(aszWork2, PRT_THCOLUMN*4, aszWork1, PRT_EJCOLUMN, RFL_FEED_ON);  /* adjust string (for tab) */

    usStrLen = PrtAdjustString(aszWork1, aszWork2, usStrLen, PRT_EJCOLUMN);    /* adjust string (for return) */

	PrtEJInit (0);    // to prevent possible Electronic Journal corruption, ensure there is some reasonable header available

	if (usPrtEJOffset < sizeof (EJT_HEADER)) {
		// create the EJ block header information if the block has not been initialized
		PrtMaintEJInit(0);
	}
    if ( usPrtEJOffset + (usStrLen * sizeof(TCHAR)) >= sizeof(auchPrtEJBuffer) ) {
                                                /* can write to E/J buffer ? */
        PrtEJSend();                            /* send to E/J module */
		// This is a continuation so we will reuse the same EJ header.
        usPrtEJOffset = sizeof(EJT_HEADER);
	}
                                                
    _tcsncpy( &(auchPrtEJBuffer[usPrtEJOffset]), aszWork1, usStrLen);  /* write to E/J buffer */
    usPrtEJOffset += usStrLen * sizeof(TCHAR);                 /* increment offset */                                                           
}

/*
*===========================================================================
** Format  : VOID  PrtEJSend(VOID)
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function sned data(auchPrtEJBuffer) to E/J module.
*
*===========================================================================
*/
VOID  PrtEJSend(VOID)
{
    UCHAR  uchType, uchOption=0;
    SHORT  sReturn = EJ_PERFORM;

    ((EJT_HEADER *)auchPrtEJBuffer)->usCVLI = usPrtEJOffset;

	if (((EJT_HEADER *)auchPrtEJBuffer)->usEjSignature != EJT_HEADER_SIGNATURE) {
		NHPOS_ASSERT_TEXT(!EJT_HEADER_SIGNATURE, "PrtEJSend() bad header - usEjSignature  != EJT_HEADER_SIGNATURE.");
		PrtForceEJInit (0);                       /* initialize E/J print buffer */
	}

	if ( ((EJT_HEADER *)auchPrtEJBuffer)->usSeqNo == 1) {
        uchType = MINOR_EJ_NEW;
    } else {
        uchType = 0;
    }

    /* send buffering data to EJ file */
    /* ej full is infinit or once,  V3.3 */
    if (CliParaMDCCheck(MDC_EJ_ADR, ODD_MDC_BIT0)) {
        uchOption = 1;
    }
	if (usPrtEJOffset >= sizeof(EJT_HEADER)) {
	   sReturn = CliEJWrite((EJT_HEADER *)auchPrtEJBuffer, (USHORT)(usPrtEJOffset - sizeof(EJT_HEADER)),
							 auchPrtEJBuffer, uchType, uchOption); /* ### Mod (2171 for Win32) V1.0 */
		if (sReturn != EJ_PERFORM) {
			char xBuff[128];
			sprintf (xBuff, "**ERROR - PrtEJSend(): CliEJWrite() sRetStatus = %d", sReturn);
			NHPOS_NONASSERT_NOTE("sReturn == EJ_PERFORM", xBuff);
		}
	}

    usPrtEJOffset = 0;                          /* initial. write offset */

    /* check EJ file condition */
    if (sReturn == EJ_FILE_FULL || sReturn == EJ_DATA_TOO_LARGE) {
        UieErrorMsg(LDT_FULL_ADR, UIE_WITHOUT);
    }

    if (sReturn == EJ_FILE_NEAR_FULL) {
        UieErrorMsg(LDT_NEARFULL_ADR, UIE_WITHOUT);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtEJCPRoomCharge(UCHAR *pRoomNo, UCHAR *pGuestID);
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
VOID  PrtEJCPRoomCharge(TCHAR *pRoomNo, TCHAR *pGuestID)
{
    if (*pRoomNo != _T('\0') && *pGuestID != _T('\0')) {
		TCHAR  aszSpecMnem1[PARA_SPEMNEMO_LEN + 1] = {0};  
		TCHAR  aszSpecMnem2[PARA_SPEMNEMO_LEN + 1] = {0};
	                                                       
		RflGetSpecMnem(aszSpecMnem1, SPC_ROOMNO_ADR);   /* get room no. mnemo */
		RflGetSpecMnem(aszSpecMnem2, SPC_GUESTID_ADR);  /* get guest id */

        PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJCPRoomCharge, aszSpecMnem1, pRoomNo, aszSpecMnem2, pGuestID);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtEJFolioPost(UCHAR *pszFolioNo, UCHAR *pszPostTranNo)
*                                   
*    Input : UCHAR *pszFolioNo      -folio. number   
*            UCHAR *pszPostTranNo   -posted transaction number   
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints "folio. and posted transaction number" line.
*            if "0", does not print.
*===========================================================================
*/
VOID  PrtEJFolioPost(TCHAR *pszFolioNo, TCHAR *pszPostTranNo)
{
    if (*pszFolioNo != _T('\0') && *pszPostTranNo != _T('\0')) {
		TCHAR  aszSpecMnem1[PARA_SPEMNEMO_LEN + 1] = {0};  
		TCHAR  aszSpecMnem2[PARA_SPEMNEMO_LEN + 1] = {0};

		RflGetSpecMnem(aszSpecMnem1, SPC_FOLINO_ADR);   /* get folio no. */
		RflGetSpecMnem(aszSpecMnem2, SPC_POSTTRAN_ADR); /* get posted transaction no. */

        PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJFolioPost, aszSpecMnem1, pszFolioNo, aszSpecMnem2, pszPostTranNo);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtEJCPRspMsgText(UCHAR *pRspMsgText);
*                                   
*   Input  : UCHAR  *pRspMsgText    -Pointer of Response Message Text 
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints response message text for charge posting.
*            Since this is goint toe the Electronic Journal we are going to save
*            space by doing two things:
*              (1) trim leading spaces so that items are left justified
*              (2) ignore empty lines
*            We are doing these changes to allow for EMV receipts which have
*            quite a bit of text, almost 30 lines.
*===========================================================================
*/
USHORT  PrtEJCPRspMsgText(TCHAR *pRspMsgText)
{
	USHORT usRet = 0;
	TCHAR  aszPrint[NUM_CPRSPTEXT] = {0};

	_tcsncpy (aszPrint, pRspMsgText, STD_CPRSPTEXT_LEN);
	tcharTrimLeading (aszPrint);
    if (aszPrint[0] != _T('\0')) {
        PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJCPRspMsgText, aszPrint); // PrtEJCPRspMsgText()
		usRet = 1;
    }
	return usRet;
}

/*
*===========================================================================
** Format  : VOID  PrtEJOffline(SHORT fsMod, USHORT usDate, UCHAR *aszApproval);
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
*===========================================================================
*/
VOID  PrtEJOffline(USHORT fbMod, TCHAR *auchDate, TCHAR *auchApproval)
{
	TCHAR  aszApproval[NUM_APPROVAL + 1] = {0};
    TCHAR  aszDate[NUM_EXPIRA + 1] = {0};
    TCHAR  aszOffMnem[PARA_SPEMNEMO_LEN + 1] = {0};
    TCHAR  aszOffDate[NUM_EXPIRA * 2 + 1] = {0};         // large enough to handle XX/XX
    USHORT  usOffDate;

    _tcsncpy(aszApproval, auchApproval, NUM_APPROVAL);
    _tcsncpy(aszDate, auchDate, NUM_EXPIRA);

    if ( fbMod & OFFEPTTEND_MODIF) {                /* offline modifier */
        /* -- get special mnemonics -- */
		RflGetSpecMnem( aszOffMnem, SPC_OFFTEND_ADR);
    }           

    if ( *aszDate != _T('\0') ){                        /* expiration date  */
		if (aszDate[0] == _T('X')) {			
			_tcsncpy(aszOffDate,_T("XX/XX"), NUM_EXPIRA + 1);
		} else {
			usOffDate = _ttoi(aszDate);
			if (usOffDate != 0) {                       /* E-028 corr. 4/20     */
				RflSPrintf( aszOffDate, TCHARSIZEOF(aszOffDate), aszPrtEJOffDate, usOffDate / 100, usOffDate % 100 );
			}
		}
    }

    /* -- send mnemoncis and exp.date -- */
    if (*aszDate != _T('\0') || *aszApproval != _T('\0') || *aszOffMnem != _T('\0')) {
        PrtPrintf( PMG_PRT_JOURNAL, aszPrtEJOffline, aszOffMnem, aszOffDate, aszApproval );  /* E-001 corr. 4/21 */
    }
}

/*
*===========================================================================
** Format  : VOID  PrtEJEPTError(ITEMPRINT  *pItem);
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
VOID  PrtEJEPTError(ITEMPRINT  *pItem)
{
    PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJEPTError, pItem->uchStatus, (USHORT)pItem->lAmount );
}

/*
*===========================================================================
** Format  : VOID  PrtEJResetLog(ITEMOTHER *pItem)
*
*    Input : ITEMOTHER pItem       -print data
*   Output : none
*    InOut : none
** Return  : none
** Synopsis: This function prints PC i/F reset log.         V3.3
*  Print format :
*
*      24-char printer
*
*       0        1         2
*       123456789012345678901234
*      |^^^^^^^^^^^^^^^^^^^^^^^^|
*      | PCIF RESET          xxx|
*      | RESET CUR DAYZ     xxxx|
*      | Exxx     12:34  1/23/98|
*       ^^^^^^^^^^^^^^^^^^^^^^^^
*
*      21-char printer
*
*       0        1         2
*       123456789012345678901
*      |^^^^^^^^^^^^^^^^^^^^^|
*      | PCIF RESET       xxx|
*      | RESET CUR DAYZ  xxxx|
*      | Exxx  12:34  1/23/98|
*       ^^^^^^^^^^^^^^^^^^^^^
*===========================================================================
*/
VOID  PrtEJResetLog(ITEMOTHER *pItem)
{
    if (pItem->uchAction == UIF_UCHRESETLOG_NOACTION) {
		/* --- Print Mnemonic from PCIF --- */
        PrtPrint(PMG_PRT_JOURNAL, pItem->aszMnemonic, usPrtEJColumn);
    } else {
		CONST TCHAR aszPrtEJResetLog[] = _T(" **PCIF ACTION %3d**");  /* PrtEJResetLog() V3.3 */
		CONST TCHAR aszPrtRead[]  = _T("READ");
		CONST TCHAR aszPrtReset[] = _T("RESET");
		CONST TCHAR aszPrtIssue[] = _T("ISSUE");
		CONST TCHAR aszPrtCur[]   = _T("CUR");
		CONST TCHAR aszPrtSave[]  = _T("SAVE");
		CONST TCHAR aszPrtDay[]   = _T("DAY");
		CONST TCHAR aszPrtPTD[]   = _T("PTD");
		CONST TCHAR aszPrtErr[]   = _T("E%3d");

		TCHAR aszBuff1[10] = {0};
		TCHAR aszBuff2[10] = {0};
		TCHAR aszBuff3[10] = {0};
		TCHAR aszBuff4[10] = {0};

		/* --- Print Reset Log by PCIF --- */
        /* reset log first line, Action Code */
        PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJResetLog, pItem->uchAction);

        /* reset log second line, reset type */
        if (pItem->uchReset == ISP_RESET_READ) {
            _tcscat(aszBuff1, aszPrtRead);
        } else if (pItem->uchReset == ISP_RESET_RESET) {
            _tcscat(aszBuff1, aszPrtReset);
        } else if (pItem->uchReset == ISP_RESET_ISSUE) {
            _tcscat(aszBuff1, aszPrtIssue);
        }
        if (pItem->uchCurSave == ISP_CURSAVE_CUR) {
            _tcscat(aszBuff2, aszPrtCur);
        } else if (pItem->uchCurSave == ISP_CURSAVE_SAVE) {
            _tcscat(aszBuff2, aszPrtSave);
        }
        if (pItem->uchDayPtd == ISP_DAYPTD_DAY) {
            _tcscat(aszBuff3, aszPrtDay);
        } else if (pItem->uchDayPtd == ISP_DAYPTD_PTD) {
            _tcscat(aszBuff3, aszPrtPTD);
        }
        if (pItem->lAmount) {
            _ultot((ULONG)pItem->lAmount, aszBuff4, 10);
		}
        if (pItem->uchReset || pItem->uchCurSave || pItem->uchDayPtd || pItem->lAmount) {
			CONST TCHAR  aszPrtEJResetLog2[] = _T("%s %s %s\t%s");         /* V3.3 */

            PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJResetLog2, aszBuff1, aszBuff2, aszBuff3, aszBuff4);
        }
    }
}

/*
*===========================================================================
** Format  : VOID  PrtEJResetLog3(ITEMOTHER *pItem)
*               
*    Input : ITEMOTHER pItem       -print data
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints PC IF log (Function Code) on journal
*
*  Print format :
*
*      24-char printer
*
*       0        1         2      
*       123456789012345678901234  
*      |^^^^^^^^^^^^^^^^^^^^^^^^| 
*      | ** PCIF ** xxxx        |
*      |                        | 
*       ^^^^^^^^^^^^^^^^^^^^^^^^                            V3.3 (FVT#5)
*===========================================================================
*/
VOID  PrtEJResetLog3(ITEMOTHER *pItem)
{
	CONST TCHAR  aszPrtEJResetLog4[] = _T(" ** PCIF ** %s");       /* FVT#5, V3.3 */

    PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJResetLog4, pItem->aszMnemonic);
}

/*
*===========================================================================
** Format  : VOID  PrtEJResetLog4(ITEMOTHER *pItem)
*               
*    Input : USHORT usNo
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints PC IF log (Trailer) on journal    V3.3 (FVT#5)
*===========================================================================
*/
VOID  PrtEJResetLog4(ITEMOTHER *pItem)
{
	CONST TCHAR  aszPrtEJResetLog3[] = _T("%04d  \t%s%s");         /* V3.3 */
	CONST TCHAR  aszPrtEJResetLog5[] = _T("%-3u\t%04u-%03u");      /* FVT#5, V3.3 */
	CONST TCHAR  aszPrtEJResetLog6[] = _T("\t%04u-%03u");          /* FVT#5, V3.3 */
    TCHAR           aszPrtNull[10] = {0};
	TCHAR           aszBuff2[10] = {0};
	TCHAR           aszBuff3[10] = {0};
	PARASTOREGNO    StRg = {0}; 

    /* --- SPV# & Store/Reg# --- */
    StRg.uchMajorClass = CLASS_PARASTOREGNO;
    StRg.uchAddress = SREG_NO_ADR;
    CliParaRead(&StRg);
    if (pItem->lAmount) {
        PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJResetLog5, (USHORT)pItem->lAmount, StRg.usStoreNo, StRg.usRegisterNo);
    } else {
        PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJResetLog6, StRg.usStoreNo, StRg.usRegisterNo);
    }
    /* --- Consecutive, Time, Date --- */
    aszBuff2[0] = _T('\0');
    aszBuff3[0] = _T('\0');
    if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT3)) {   /* MILITARY type */
        RflSPrintf(aszBuff2, TCHARSIZEOF(aszBuff2), aszPrtTime, pItem->uchHour, pItem->uchMin, aszPrtNull);
    } else {
		UCHAR uchHour = (pItem->uchHour > 12) ? (pItem->uchHour - 12) : pItem->uchHour;

        if (pItem->uchHour == 0) {
			uchHour = 12;
        }
        RflSPrintf(aszBuff2, TCHARSIZEOF(aszBuff2), aszPrtTime, uchHour, pItem->uchMin, (pItem->uchHour >= 12) ?  aszPrtPM : aszPrtAM);
    }

    if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {    /* DD/MM/YY */
        RflSPrintf(aszBuff3, TCHARSIZEOF(aszBuff3), aszPrtDate, pItem->uchDay, pItem->uchMonth, pItem->uchYear);
    } else {
        RflSPrintf(aszBuff3, TCHARSIZEOF(aszBuff3), aszPrtDate, pItem->uchMonth, pItem->uchDay, pItem->uchYear);
    }

	{
		USHORT  usConsecNumber = 0;
		usConsecNumber = MaintCurrentSpcCo(SPCO_CONSEC_ADR);
		PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJResetLog3, usConsecNumber, aszBuff2, aszBuff3);
	}
}

/*
*===========================================================================
** Format  : VOID   PrtEJPowerDownLog(ITEMOTHER *pItem)
*               
*    Input : ITEMOTHER pItem       -print data
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints power down log on ejournal
*
*  Print format :
*
*      24-char printer
*
*       0        1         2      
*       123456789012345678901234  
*      |^^^^^^^^^^^^^^^^^^^^^^^^| 
*      | DATALOST               |
*      |                        | 
*       ^^^^^^^^^^^^^^^^^^^^^^^^                                Saratoga
*===========================================================================
*/
VOID    PrtEJPowerDownLog(ITEMOTHER *pItem)
{
	CONST TCHAR aszPrtEJPowerDownLog[] = _T(" ** POWER ** %s");              /* Saratoga */
	TCHAR       aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};

    switch (pItem->uchAction) {
    case FSC_MINOR_POWER_DOWN:
		RflGetTranMnem(aszTranMnem, TRN_POWER_DOWN_ADR);
        PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJPowerDownLog, aszTranMnem);
        /* PrtEJResetLog4(pItem); */
        break;

    case FSC_MINOR_SUSPEND:
        break;
    }
}
/*
*===========================================================================
** Format  : VOID  PrtEJPLUNoP(UCHAR *puchPLUNo, SHORT sPortion);
*
*    Input : UCHAR *puchPLUNo      -PLU or coupon No
*            SHORT sPortion        -print portion : PRT_RECEIPT
*                                                   PRT_JOURNAL
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
*       24-char printer
*
*       0        1         2
*       123456789012345678901234
*      |^^^^^^^^^^^^^^^^^^^^^^^^|
*      |99999999999999          |
*      |                        |
*       ^^^^^^^^^^^^^^^^^^^^^^^^
*
*       21-char printer
*
*       0        1         2
*       123456789012345678901
*      |^^^^^^^^^^^^^^^^^^^^^|
*      |99999999999999       |
*      |                     |
*       ^^^^^^^^^^^^^^^^^^^^^
*===========================================================================
*/
VOID  PrtEJPLUNo(TCHAR *puchPLUCpn)
{
    if (*puchPLUCpn) {
        PrtPrint(PMG_PRT_JOURNAL, puchPLUCpn, tcharlen(puchPLUCpn));
    }
}

/*
*===========================================================================
** Format  : VOID  PrtTHPLUBuild (UCHAR *puchPLUNo, UCHAR uchDeptNo);
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
*       0        1         2    
*       123456789012345678901234
*      |^^^^^^^^^^^^^^^^^^^^^^^^^
*      |PLU-BUIL                |
*      |      9999999999999 9999|
*      |                        |
*       ^^^^^^^^^^^^^^^^^^^^^^^^^
*===========================================================================
*/
VOID    PrtEJPLUBuild (USHORT usTranAdr, TCHAR *puchPLUNo, USHORT usDeptNo)
{
    TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0}; /* PARA_... defined in "paraequ.h" */

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usTranAdr);
	RflCleanupMnemonic (aszTranMnem);
    
    PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJPluBuild, aszTranMnem, puchPLUNo, usDeptNo);
}

/*
*===========================================================================
** Format  : VOID  PrtEJMoneyForeign (DCURRENCY lForeign, UCHAR uchAdr1, UCHAR uchAdr2,
*                                     UCHAR fbStatus);
*
*   Input  : DCURRENCY  lForeign    -Foreign tender amount
*            UCHAR      uchAdr1     -Foreign tender mnemonic address
*            UCHAR      uchAdr2     -Foreign symbol address
*            UCHAR      fbStatus    -Foreign currency decimal point status
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
VOID    PrtEJMoneyForeign (DCURRENCY lForeign, USHORT usTranAdr1, UCHAR uchAdr2, UCHAR fbStatus)
{
	TCHAR   aszFMnem[PARA_SPEMNEMO_LEN + 1] = {0};        /* PARA_... defined in "paraequ.h" */
    TCHAR   aszFAmt[PRT_THCOLUMN + 1] = {0};
    TCHAR   aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};   /* PARA_... defined in "paraequ.h" */
    USHORT  usStrLen;
    SHORT   sDecPoint;

    /* -- check decimal point -- */
    if ( fbStatus & ITEMTENDER_STATUS_0_DECIMAL_3 ) {
        if ( fbStatus & ITEMTENDER_STATUS_0_DECIMAL_0 ) {
            sDecPoint = PRT_NODECIMAL;
        } else {
            sDecPoint = PRT_3DECIMAL;
        }
    } else {
        sDecPoint = PRT_2DECIMAL;
    }

    /* -- get foreign symbol -- */
	RflGetSpecMnem( aszFMnem, uchAdr2 );

    /* -- adjust sign ( + , - ) -- */
    if ( lForeign < 0 ) {
        aszFAmt[0] = _T('-');
        _tcsncpy( &aszFAmt[1], aszFMnem, PARA_SPEMNEMO_LEN );
    } else {
        _tcsncpy( aszFAmt, aszFMnem, PARA_SPEMNEMO_LEN );
    }

    /* -- get string length -- */
    usStrLen = tcharlen( aszFAmt );

    /* -- format foreign amount -- */
    RflSPrintf( &aszFAmt[usStrLen], TCHARSIZEOF(aszFAmt) - usStrLen, aszPrtEJMoneyForeign, sDecPoint, lForeign );

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem( aszTranMnem, usTranAdr1 );
	RflCleanupMnemonic (aszTranMnem);

    PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJMnemMnem, aszTranMnem, aszFAmt);
}

/*
*===========================================================================
** Format  : VOID  PrtEJAmtAndMnemonic (ITEMTENDER  *pItemTender);
*               
*    Input : ITEMTENDER  *pItemTender     - tender data
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints prints transaction mnemonics
*            and Amount line. This function will either use the
*            Transaction Mnemonic for the Tender Key or, if there is one,
*            the provided mnemonic instead over riding the default of
*            the Tender Key Transaction Mnemonic.
*===========================================================================
*/
VOID  PrtEJAmtAndMnemonic (ITEMTENDER  *pItemTender)
{
	DCURRENCY  lTenderAmount = pItemTender->lTenderAmount;

	if (pItemTender->uchMinorClass == CLASS_TEND_TIPS_RETURN) lTenderAmount = 0;

	if (pItemTender->aszTendMnem[0] == 0) {
		// no mnemonic with this tender so use the standard one
		if (pItemTender->uchMinorClass !=  CLASS_TEND_FSCHANGE) { /* Saratoga */
			PrtEJAmtMnem(RflChkTendAdr(pItemTender), lTenderAmount); /* tender line */
		}
	} else {
		TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};  /* PARA_... defined in "paraequ.h" */

		_tcsncpy(aszTranMnem, pItemTender->aszTendMnem, PARA_TRANSMNEMO_LEN);
		RflCleanupMnemonic (aszTranMnem);
		PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJAmtMnem, aszTranMnem, lTenderAmount);
	}
}
/****** End of Source ******/
