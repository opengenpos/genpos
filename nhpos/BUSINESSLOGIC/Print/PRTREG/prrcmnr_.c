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
* Title       : Print common routine, reg mode, receipt & journal                 
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRCmnR_.C
*               | ||  ||
*               | ||  |+- S:21Char L:24Char _:not influcenced by print column
*               | ||  +-- R:recept & jounal S:slip V:validation K:kitchen 
*               | |+----- comon routine
*               | +------ R:reg mode S:super mode
*               +-------- Pr:print module
* --------------------------------------------------------------------------
* Abstract:
*
*   PrtRJVoid()
*   PrtRJNumber()
*   PrtRJAmtMnem()
*   PrtRJZeroAmtMnemP()
*   PrtRJMnem()
*   PrtRJTaxMod()
*   PrtRJPerDisc()
*#  PrtRJCoupon()
*   PrtRJAmtSym()
*   PrtRJAmtSymP()
*#  PrtRJTblPerson()
*#  PrtRJCustomerName()
*   PrtRJMultiSeatNo()     add at R3.1
*   PrtRJTranNum()
*   PrtRJKitchen()
*   PrtRJWaiter()
*   PrtRJForeign1()
*   PrtRJForeign2()
*   PrtRJSeatNo()          add at R3.1
*   PrtRJQty()
*   PrtRJWeight()
*   PrtRJChild()
*   PrtRJItems()
*   PrtRJTrail2()
*   PrtRJHead()
*   PrtRJDoubleHead()
*   PrtRJZeroAmtMnem() 
*   PrtRJGuest()
*   PrtRJMulChk()
*   PrtRFeed()
*   PrtRJWaiTaxMod()
*   PrtRJMnemCount()
*   PrtRJCPRoomCharge()
*   PrtJFolioPost()
*   PrtRJCPRspMsgText()
*   PrtRJOffline()
*   PrtRJEPTError()
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-10-92 : 00.00.01 : K.Nakajima :                                    
* Jun-29-93 : 01.00.12 : K.You      : mod. PrtRJQty for US enhance.                                   
* Jul-06-93 : 01.00.12 : R.Itoh     : add PrtRJEtkCode(), PrtRJEtkJobTimeIn(),
*                                         PrtRJEtkJobTimeOut()                                   
* Oct-27-93 : 02.00.02 : K.You      : add charge posting feature.                                   
* Apr-04-94 : 00.00.04 : Yoshiko.J  : add PrtRJOffline(), PrtRJEPTError()
* Apr-12-94 : 00.00.04 : K.You      : add PrtRJOffTend(), PrtRJEPTError()
* Apr-24-94 : 00.00.05 : K.You      : bug fixed E-32 (mod. PrtRJOffTend)
* Mar-17-94 : 02.05.00 : K.You      : bug fixed S-6 (mod. PrtJFolioPost)
* Mar-17-94 : 02.05.00 : K.You      : Chg PrtRJHead() (specs154228) 
* Mar-03-95 : 03.00.00 : T.Nakahata : Add Coupon Feature (PrtRJCoupon) and
*                                     Unique Trans No. (mod. PrtRJTblPerson)
* Jun-07-95 : 03.00.00 : T.Nakahata : Add Condiment of Promotional PLU
* Aug-29-95 : 03.00.04 : T.Nakahata : FVT comment (unique tran#, 4 => 12 Char)
*           :          :            : customer name (max 16 chara)
* Nov-15-95 : 03.01.00 : M.Ozawa    : Change ETK format at R3.1
* Aug-08-99 : 03.05.00 : M.Teraki   : Remove WAITER_MODEL
* Aug-16-99 : 03.05.00 : M.Teraki   : Merge STORE_RECALL_MODEL
*           :          :            :       /GUEST_CHECK_MODEL
*
** GenPOS **
* Feb-19-18 : v2.2.2.0 : R.Chambers  : fix buffer size used with function PrtGetSISymCoupon().
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
#include<stdlib.h>
#include<string.h>
/**------- 2170 local------**/
#include<ecr.h>
#include<regstrct.h>
#include<transact.h>
#include<paraequ.h> 
#include<para.h>
#include<csstbpar.h>
/* #include<uie.h> */
#include<pmg.h>
#include<rfl.h>
#include"prtrin.h"
#include"prrcolm_.h"
#include"prtcom.h"
#include"maint.h"

/**
;========================================================================
;+                    S T A T I C    R A M s
;========================================================================
**/

//SR 144 cwunn added to print quantity, mnemonic, and amount on the same line
CONST TCHAR FARCONST  aszPrtRJAmtMnem1Line[] = _T("%ld X $%l$ %s\t%l$");  /* trans.mnem and amount */
//emd SR 144
CONST TCHAR FARCONST  aszPrtRJAmtMnem[] = _T("%.8s\t%l$");            /*JHHJ 8 Characters  trans.mnem and amount */
CONST TCHAR FARCONST  aszPrtAmtMnemShift[] = _T("%8.8s\t %l$%*s");    /*8 characters JHHJ  trans.mnem and amount */
CONST TCHAR FARCONST  aszPrtRJAmtDiscMnem[] = _T("%.8s\t%d%%%11l$");  /* disc. rate and amount */

CONST TCHAR FARCONST  aszPrtRJMnemMnem[] = _T("%8.8s\t %s");          /*8 characters JHHJ  mnem. and mnem. */
CONST TCHAR FARCONST  aszPrtRJEuroAmtMnem[] = _T("%8.8s\t%s%.*l$");   /*8 characters JHHJ  trans.mnem and amount */
/*** Modify for Unique Tran No. (REL 3.0) ***/
CONST TCHAR FARCONST  aszPrtRJTblPerson[] = _T("%-4s %-3s %8.8s %s"); /* 8 characters JHHJ   table and person */
CONST TCHAR FARCONST  aszPrtRJTransNo[] = _T("%-.12s %-8s");          /* unique tran no. */
CONST TCHAR FARCONST  aszPrtRJPerson[] = _T("         %s %s");        /* no. of person */

CONST TCHAR FARCONST  aszPrtRJCustomerName[] = _T("%-19s");  /* customer name    ** Modify for Unique Tran No. (REL 3.0) ***/

CONST TCHAR FARCONST  aszPrtRJMultiSeat[] = _T("%-s");       /* seat no. */

CONST TCHAR FARCONST  aszPrtRJMnemNum[] = _T("%-8.8s %lu");                 /*8 characters JHHJ  mnemo. and number */

CONST TCHAR FARCONST  aszPrtRJPrinNo[]  = _T("PRT%d");                      /* printer no. */

CONST TCHAR FARCONST  aszPrtRJWaiter[]  = _T("%s%8.8Mu");                   /* waiter mnemo. and ID */

CONST TCHAR FARCONST  aszPrtRJWaiTaxMod[] = _T("%s%8.8Mu  %s");             /* waiter mnemo. and ID and taxmodifier */

CONST TCHAR FARCONST  aszPrtRJForeign1[]  = _T("%.*l$");                    /* foreign amount */

CONST TCHAR FARCONST  aszPrtRJForeign2[]  = _T("   / %10.*l$");             /* foreign conversion rate   XXXXX.XXXXX */
CONST TCHAR FARCONST  aszPrtRJEuro1[]  = _T("%s%ld / %s%.*l$");             /* Euro amount */

CONST TCHAR FARCONST  aszPrtRJSeat[] = _T("%-4s %1d");                      //SR206 _T("%-4s %2d");       /* seat no. */

CONST TCHAR FARCONST  aszPrtRJQuant[]   = _T("%8ld X\t %l$ ");              /* qty and unit price */

CONST TCHAR FARCONST  aszPrtRJPPIQuant[]   = _T("%8ld X\t %l$/%ld ");       /* qty, unit price and qty */

CONST TCHAR FARCONST  aszPrtRJWeight1[] = _T("%8.*l$%s\t %.8s");            /*JHHJ only 8 characters  Weight and Manual Mnem. */

CONST TCHAR FARCONST  aszPrtRJWeight2[] = _T("\t@ %l$/%s ");                /* scale sym. and unit price */

CONST TCHAR FARCONST  aszPrtRJChild[]   = _T(" %4s %s");                    /* child plu */

CONST TCHAR FARCONST  aszPrtRJChild2[]   = _T(" %s");                       /* child plu, saratoga */
CONST TCHAR FARCONST  aszPrtRJModLinkPLU1[]   = _T("%4s %s\t%l$ ");         /* child plu */
CONST TCHAR FARCONST  aszPrtRJModLinkPLU2[]   = _T("%s\t%l$ ");
CONST TCHAR FARCONST  aszPrtRJLinkPLU1[]   = _T("%4s %s\t%l$");             /* child plu */
CONST TCHAR FARCONST  aszPrtRJLinkPLU2[]   = _T("%s\t%l$");                 /* child plu, saratoga */

CONST TCHAR FARCONST  aszPrtRJTrail2[] = _T("%04d\t %s");                   /* trailer 2nd line */

CONST TCHAR FARCONST  aszPrtRJGuest_WTCD[] = _T("%-8.8s %4d%02d");          /*8 characters JHHJ   guest no. with check digit */

CONST TCHAR FARCONST  aszPrtRJGuest_WOCD[] = _T("%-8.8s %4d");              /*8 characters JHHJ   guest no. without check digit */

CONST TCHAR FARCONST  aszPrtQty[] = _T("%8ld X");                           /* qty line */

CONST TCHAR FARCONST  aszPrtRJMnemCount[] = _T("%-8.8s\t%d");               /*8 characters JHHJ   mnemo. and counter */

CONST TCHAR FARCONST  aszPrtRJModScale[] = _T("%8.8s\t @ %l$ ");            /*8 characters JHHJ   mod. disc. weight */

CONST TCHAR FARCONST  aszPrtRJModAmtMnem[] = _T("%s\t %l$ ");               /* mod. disc. product */

//SR 144 cwunn added to print quantity, mnemonic, and amount on the same line
CONST TCHAR FARCONST  aszPrtRJModAmtMnem1Line[] = _T("%ld X $%l$ %s\t %l$ ");   /* mod. disc. product */
//end SR 144

CONST TCHAR FARCONST  aszPrtRJCPRoomCharge[] = _T("%-4s%6s\t %-4s%3s");     /* room charge */

CONST TCHAR FARCONST  aszPrtRJCPRspMsgText[] = _T("%-40s");                 /* charge posting */

CONST TCHAR FARCONST  aszPrtJFolioPost[] = _T("%-4s%6s\t %-4s%6s");         /* folio, posted transaction no. */

CONST TCHAR FARCONST  aszPrtRJOffline[] = _T("%-4s   %5s   %6s");           /* mnemo, date, approval code */

CONST TCHAR FARCONST  aszPrtRJOffDate[] = _T("%02d/%02d");                  /* expiration date */

CONST TCHAR FARCONST  aszPrtRJEPTError[] = _T("%c %d");                     /* mnemo. and error code */

CONST TCHAR FARCONST  aszPrtRJResetLog[] = _T(" **PCIF ACTION %3d**");  /* V3.3 */

CONST TCHAR FARCONST  aszPrtRJResetLog2[] = _T("%s %s %s\t%s");         /* V3.3 */
CONST TCHAR FARCONST  aszPrtRJResetLog3[] = _T("%04d  \t%s%s");         /* V3.3 */
CONST TCHAR FARCONST  aszPrtRJResetLog4[] = _T(" ** PCIF ** %s");       /* FVT#5, V3.3 */
CONST TCHAR FARCONST  aszPrtRJResetLog5[] = _T("%-3u\t%04u-%03u");      /* FVT#5, V3.3 */
CONST TCHAR FARCONST  aszPrtRJResetLog6[] = _T("\t%04u-%03u");          /* FVT#5, V3.3 */
CONST TCHAR FARCONST  aszPrtRJPowerDownLog[] = _T(" ** POWER ** %8.8s");              /* Saratoga *//* 8 characters JHHJ*/

CONST TCHAR FARCONST aszPrtRead[]  = _T("READ");
CONST TCHAR FARCONST aszPrtReset[] = _T("RESET");
CONST TCHAR FARCONST aszPrtIssue[] = _T("ISSUE");
CONST TCHAR FARCONST aszPrtCur[]   = _T("CUR");
CONST TCHAR FARCONST aszPrtSave[]  = _T("SAVE");
CONST TCHAR FARCONST aszPrtDay[]   = _T("DAY");
CONST TCHAR FARCONST aszPrtPTD[]   = _T("PTD");
CONST TCHAR FARCONST aszPrtErr[]   = _T("E%3d");

CONST TCHAR FARCONST  aszPrtRJQty[]   = _T("\t%3ld X %s ");             /* qty, unit price, Saratoga */
CONST TCHAR FARCONST  aszPrtFor[] = _T("%d");
CONST TCHAR FARCONST  aszPrtRJSpecialQty[]   = _T("%6.*l$ X %s/%2u");   /* special qty , unit price, for */
CONST TCHAR FARCONST  aszPrtRJQtyFor[]   = _T("\t%3ld X %s/%2u ");      /* qty, unit price, for */
CONST TCHAR FARCONST  aszPrtRJDecQty[]   = _T("\t%6.*l$ X %s ");        /* qty, unit price */
CONST TCHAR FARCONST  aszPrtRJDecQtyFor[]   = _T("%6.*l$ \tX %s/%2u "); /* qty, unit price, for */

extern CONST TCHAR FARCONST aszPrtTime[];
extern CONST TCHAR FARCONST aszPrtTimeZone[];
extern CONST TCHAR FARCONST aszPrtDate[];

extern CONST TCHAR FARCONST  aszEtkCode[];

CONST TCHAR FARCONST  aszPrtJPluBuild[]      = _T("%8.8s\t %13s %04d");         /* Plu no and Dept no.  8 characters JHHJ*/

CONST TCHAR FARCONST  aszPrtRJAmtMnemSI[] = _T("%s %s\t %l$");                  /* trans.mnem and amount, 21RFC05437 */
CONST TCHAR FARCONST  aszPrtRJAmtDiscMnemSI[] = _T("%8.8s %8.8s %d%%\t%11l$");  /*8 characters JHHJ  disc. rate and amount */
/**
;========================================================================
;+             P R O G R A M    D E C L A R A T I O N s
;========================================================================
**/
/*
*===========================================================================
** Format  : VOID  PrtRJVoid(CHAR fbMod);
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
VOID PrtRJVoid(USHORT fbMod)
{
    TCHAR   aszWork[PRT_RJCOLUMN + 1];              /* print work area */
	USHORT usReasonCode = 0;


    if (fbMod & (VOID_MODIFIER | RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) {
        memset(aszWork, '\0', sizeof(aszWork));
		if (0 > PrtGetReturns(fbMod, usReasonCode, aszWork, TCHARSIZEOF(aszWork))) {  
			/* -- get void mnemonics instead of returns mnemonic -- */
			PrtGetVoid(aszWork, TCHARSIZEOF(aszWork));
		}

		/* -- print Receipt & Journal --*/
		PmgPrint(PrtChkPort(), aszWork, usPrtRJColumn);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtRJOffTend(CHAR fbMod);
*               
*    Input : CHAR   fbMod                -Void status      
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints "off line tender" line with double wide chara.
*            if status is off, does not print.
*===========================================================================
*/
VOID PrtRJOffTend(USHORT fbMod)
{
    if (fbMod & OFFCPTEND_MODIF) {          
		TCHAR   aszWork[PRT_RJCOLUMN + 1] = {0};              /* print work area */

        /* -- get off line tender mnemonics -- */
        PrtGetOffTend(fbMod, aszWork, TCHARSIZEOF(aszWork));

        /* -- print Receipt & Journal --*/
        PmgPrint(PrtChkPort(), aszWork, usPrtRJColumn);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtRJNumber(UCHAR  *pszNumber);
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
VOID  PrtRJNumber(TCHAR  *pszNumber)
{
    if (*pszNumber != 0) {
		TCHAR   aszNumLine[PRT_RJCOLUMN * 2 + 1] = {0};

		RflGetTranMnem(aszNumLine, TRN_NUM_ADR );
        PrtSetNumber(aszNumLine, pszNumber);
        PmgPrint(PrtChkPort(), aszNumLine, (USHORT)_tcslen(aszNumLine)); /* ### Mod (2171 for Win32) V1.0 */
    }
}

/*
*===========================================================================
** Format  : VOID  PrtRJNumber(UCHAR  *pszNumber);
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
VOID  PrtRJMnemNumber(USHORT usAdr, TCHAR  *pszNumber)
{
    if (*pszNumber != 0) {
		TCHAR   aszNumLine[PRT_RJCOLUMN * 2 + 1] = {0};

        if (usAdr == 0) usAdr = TRN_NUM_ADR;

		RflGetTranMnem(aszNumLine, usAdr );
        PrtSetNumber(aszNumLine, pszNumber);
        PmgPrint(PrtChkPort(), aszNumLine, (USHORT)_tcslen(aszNumLine)); /* ### Mod (2171 for Win32) V1.0 */
    }
}

/*
*===========================================================================
** Format  : VOID  PrtRJAmtMnem(UCHAR uchAdr, LONG lAmout);
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
VOID  PrtRJAmtMnem(USHORT usAdr, DCURRENCY lAmount)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- send mnemoncis and amount -- */
    PmgPrintf(PrtChkPort(), aszPrtRJAmtMnem, aszTranMnem, lAmount);
}


/*
*===========================================================================
** Format  : VOID  PrtRJZeroAmtMnemP(UCHAR uchAdr, LONG lAmout, SHORT sPortion);
*               
*    Input : UCHAR uchAdr          -Transacion mnemonics address
*            LONG  lAmount         -Amount
*            SHORT sPortion        -print portion 
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints prints transaction mnemonics
*            and Amount line on specified printer.
*===========================================================================
*/
VOID  PrtRJZeroAmtMnemP(USHORT usAdr, DCURRENCY lAmount, SHORT sPortion)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};

    if (lAmount == 0L) {
        return ;
    }

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    if ((fsPrtPrintPort & PRT_RECEIPT) && (sPortion == PRT_RECEIPT)) {
         /* -- send mnemoncis and amount -- */
         PmgPrintf(PMG_PRT_RECEIPT, aszPrtRJAmtMnem, aszTranMnem, lAmount);
    }

    if ((fsPrtPrintPort & PRT_JOURNAL) && (sPortion == PRT_JOURNAL)) {
         /* -- send mnemoncis and amount -- */
         PmgPrintf(PMG_PRT_JOURNAL, aszPrtRJAmtMnem, aszTranMnem, lAmount);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtRJZAMnemShift(UCHAR uchAddress, LONG lAmount, USHORT usColumn);
*
*   Input  : UCHAR uchAddress    -transaction mnemonics address
*            LONG  lAmount       -change
*            USHORT usColumn     -shift column   
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints mnemoncs. and amount
*===========================================================================
*/
VOID  PrtRJZAMnemShift(UCHAR uchAddress, DCURRENCY lAmount, USHORT usColumn)
{
    if (lAmount != 0L) {
        PrtRJAmtMnemShift(uchAddress, lAmount, usColumn);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtRJAmtMnem(UCHAR uchAdr, LONG lAmout, USHORT usColumn);
*               
*    Input : UCHAR uchAdr           -Transacion mnemonics address
*            LONG  lAmount          -Amount
*            USHORT usColumn        -shift column
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints prints transaction mnemonics
*            and Amount line.
*===========================================================================
*/
VOID  PrtRJAmtMnemShift(USHORT usAdr, DCURRENCY lAmount, USHORT usColumn)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};
    TCHAR  uchNull = 0;

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- send mnemoncis and amount -- */
    PmgPrintf(PrtChkPort(), aszPrtAmtMnemShift, aszTranMnem, lAmount, usColumn, &uchNull);
}

/*
*===========================================================================
** Format  : VOID  PrtRJZAMnemPShift(UCHAR uchAdr, LONG lAmout, SHORT sPortion, USHORT usColumn);
*               
*    Input : UCHAR uchAdr          -Transacion mnemonics address
*            LONG  lAmount         -Amount
*            SHORT sPortion        -print portion 
*            USHORT usColumn       -shift column   
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints prints transaction mnemonics
*            and Amount line on specified printer.
*===========================================================================
*/
VOID  PrtRJZAMnemPShift(USHORT usAdr, DCURRENCY lAmount, SHORT sPortion, USHORT usColumn)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};
    TCHAR  uchNull = 0;

    if (lAmount == 0L) {
        return ;
    }

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    if ((fsPrtPrintPort & PRT_RECEIPT) && (sPortion == PRT_RECEIPT)) {
         /* -- send mnemoncis and amount -- */
         PmgPrintf(PMG_PRT_RECEIPT, aszPrtAmtMnemShift, aszTranMnem, lAmount, usColumn, &uchNull);
    }

    if ((fsPrtPrintPort & PRT_JOURNAL) && (sPortion == PRT_JOURNAL)) {
         /* -- send mnemoncis and amount -- */
         PmgPrintf(PMG_PRT_JOURNAL, aszPrtAmtMnemShift, aszTranMnem, lAmount, usColumn, &uchNull);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtRJMnem(UCHAR uchAdr, BOOL fsType);
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
VOID  PrtRJMnem(USHORT usAdr, BOOL fsType)
{
    TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1]; /* PARA_... defined in "paraequ.h" */
    TCHAR  aszPrintBuff[PRT_RJCOLUMN + 1];

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- double wide chara? -- */
    if (fsType == PRT_DOUBLE) {
        /* -- convert single to double -- */
        PrtDouble(aszPrintBuff, (PRT_RJCOLUMN + 1), aszTranMnem);
    } else {
        _tcsncpy(aszPrintBuff, aszTranMnem, PARA_TRANSMNEMO_LEN);
    }

    /* -- send mnemoncis -- */
    PmgPrint(PrtChkPort(), aszPrintBuff, (USHORT)_tcslen(aszPrintBuff)); /* ### Mod (2171 for Win32) V1.0 */
}

/*
*===========================================================================
** Format  : VOID  PrtRJTaxMod(USHORT fsTax, UCHAR  fbMod);
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
VOID  PrtRJTaxMod(USHORT fsTax, USHORT  fbMod)
{
    TCHAR  aszTaxMod[PRT_RJCOLUMN + 1];

    /* -- get tax modifier mnemonics -- */
    if ( PRT_WITHDATA == PrtGetTaxMod(aszTaxMod, TCHARSIZEOF(aszTaxMod), fsTax, fbMod) ) {
          PmgPrint(PrtChkPort(), aszTaxMod, (USHORT)_tcslen(aszTaxMod));  /* ### Mod (2171 for Win32) V1.0 */
    }
}

/*
*===========================================================================
** Format  : VOID  PrtRJPerDisc(UCHAR uchAdr, UCHAR uchRate, LONG lAmount);
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
VOID  PrtRJPerDisc(USHORT usAdr, UCHAR uchRate, DCURRENCY lAmount)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};
    
    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- send mnemoncis and amount -- */
    if (uchRate == 0) {
        /* not print % */
        PmgPrintf(PrtChkPort(), aszPrtRJAmtMnem, aszTranMnem, lAmount);
    } else {
        PmgPrintf(PrtChkPort(), aszPrtRJAmtDiscMnem, aszTranMnem, uchRate, lAmount);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtRJPerDisc(UCHAR uchAdr, UCHAR uchRate, LONG lAmount);
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
VOID  PrtRJPerDiscSISym(USHORT usAdr, UCHAR uchRate, DCURRENCY lAmount, TCHAR *pszSISym, UCHAR uchMDC)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};
    
    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- send mnemoncis and amount -- */
    if (uchRate == 0) {
        /* not print % */
        if (uchMDC) {
			/* si before mnemonics */
	        PmgPrintf(PrtChkPort(), aszPrtRJAmtMnemSI, pszSISym, aszTranMnem, lAmount);
	    } else {
	        PmgPrintf(PrtChkPort(), aszPrtRJAmtMnemSI, aszTranMnem, pszSISym, lAmount);
	    }
    } else {
        if (uchMDC) {
			/* si before mnemonics */
	        PmgPrintf(PrtChkPort(), aszPrtRJAmtDiscMnemSI, pszSISym, aszTranMnem, uchRate, lAmount);
	    } else {
	        PmgPrintf(PrtChkPort(), aszPrtRJAmtDiscMnemSI, aszTranMnem, pszSISym, uchRate, lAmount);
	    }
    }
}

/*
*===========================================================================
** Format   : VOID PrtRJCoupon( ITEMCOUPON *pItem )
*
*   Input   : ITEMCOUPON *pItem - coupon item address
*   Output  : none
*   InOut   : none
*
** Return   : none
*
** Synopsis : This function prints coupon item.
*===========================================================================
*/
VOID PrtRJCoupon( ITEMCOUPON *pItem )
{
	/* SI symnbol, before mnemonics 21RFC05437 */
	if ( CliParaMDCCheck(MDC_TAX2_ADR, EVEN_MDC_BIT1) ) {
		TCHAR  aszSpecMnem[PARA_SPEMNEMO_LEN * 2 + 1] = {0};  /* times 2 for possible double in PrtGetSISymCoupon() */

	    if (PrtGetSISymCoupon(aszSpecMnem, pItem)) {
			if (CliParaMDCCheck(MDC_TAX2_ADR, EVEN_MDC_BIT2) ) {
		    	/* -- send mnemoncis and amount -- */
	    		PmgPrintf( PrtChkPort(), aszPrtRJAmtMnemSI, aszSpecMnem, pItem->aszMnemonic, pItem->lAmount );
	    	} else {
			    /* -- send mnemoncis and amount -- */
    			PmgPrintf( PrtChkPort(), aszPrtRJAmtMnemSI, pItem->aszMnemonic, aszSpecMnem, pItem->lAmount );
			}
		}
	} else {
	    /* -- send mnemoncis and amount -- */
    	PmgPrintf( PrtChkPort(), aszPrtRJAmtMnem, pItem->aszMnemonic, pItem->lAmount );
	}
}

/*
*===========================================================================
** Format  : VOID  PrtRJAmtSym(UCHAR uchAdr, LONG lAmount, BOOL fsType);
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
VOID  PrtRJAmtSym(USHORT usAdr, DCURRENCY lAmount, BOOL fsType)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};
    TCHAR  aszSpecAmtS[PARA_SPEMNEMO_LEN +PRT_AMOUNT_LEN + 1] = {0};         /* spec. mnem & amount save area */
    TCHAR  aszSpecAmtD[(PARA_SPEMNEMO_LEN + PRT_AMOUNT_LEN) * 2  +1] = {0};  /* spec. mnem. & amount double wide */
    USHORT usMnemLen, usAmtSLen, usAmtDLen;     

    /* -- get transaction mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- adjust native mnemonic and amount sign(+.-) -- */
    PrtAdjustNative(aszSpecAmtS, lAmount);

    /* -- convert to double -- */
    PrtDouble(aszSpecAmtD, ((PARA_SPEMNEMO_LEN + PRT_AMOUNT_LEN) * 2  +1), aszSpecAmtS);

    usMnemLen = _tcslen(aszTranMnem);
    usAmtSLen = _tcslen(aszSpecAmtS);
    usAmtDLen = _tcslen(aszSpecAmtD);

    if ( fsType == PRT_SINGLE )  {
        PmgPrintf(PrtChkPort(), aszPrtRJMnemMnem, aszTranMnem, aszSpecAmtS);
        return;
    }

    if ( usMnemLen+usAmtDLen+1 <= usPrtRJColumn ) {
        /* -- double print -- */
        PmgPrintf(PrtChkPort(), aszPrtRJMnemMnem, aszTranMnem, aszSpecAmtD);
        return;
    }

    if ( usMnemLen+usAmtSLen+1 <= usPrtRJColumn ) {
        /* -- single print -- */
        PmgPrintf(PrtChkPort(), aszPrtRJMnemMnem, aszTranMnem, aszSpecAmtS);
    } else {
        if ( usAmtDLen <= usPrtRJColumn ) {
            /* -- double print -- */
            PmgPrintf(PrtChkPort(), aszPrtRJMnemMnem, aszTranMnem, aszSpecAmtD);
        } else {
            /* -- single print -- */
            PmgPrintf(PrtChkPort(), aszPrtRJMnemMnem, aszTranMnem, aszSpecAmtS);
        }
    }
}

/*
*===========================================================================
** Format  : VOID  PrtRJAmtSymP(UCHAR uchAdr, LONG lAmount, BOOL fsType,
*                                                       SHORT sPortion);
*               
*   Input  : UCHAR  uchAdr,             -Transaction mnemonics address
*            LONG   lAmount             -amount
*            SHORT  fsType              -characvter type
*                                        PRT_SINGLE: single character
*                                        PRT_DOUBLE: double character
*            SHORT  sPortion            -print portion
*   Output : none
*   InOut  : none
*
** Return  : none
*
** Synopsis: This function prints transaction mnemonic, native mnemonic  line.
*===========================================================================
*/
VOID  PrtRJAmtSymP(USHORT usAdr, DCURRENCY lAmount, BOOL fsType, SHORT sPortion)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};
    TCHAR  aszSpecAmtS[PARA_SPEMNEMO_LEN +PRT_AMOUNT_LEN + 1] = {0}; /* spec. mnem & amount save area */
    TCHAR  aszSpecAmtD[(PARA_SPEMNEMO_LEN + PRT_AMOUNT_LEN) * 2  +1] = {0}; /* spec. mnem. & amount double wide */
    USHORT usMnemLen, usAmtSLen, usAmtDLen;     

    /* -- get transaction mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- adjust native mnemonic and amount sign(+.-) -- */
    PrtAdjustNative(aszSpecAmtS, lAmount);

    /* -- convert to double -- */
    PrtDouble(aszSpecAmtD, ((PARA_SPEMNEMO_LEN + PRT_AMOUNT_LEN) * 2  +1), aszSpecAmtS);

    usMnemLen = _tcslen(aszTranMnem);
    usAmtSLen = _tcslen(aszSpecAmtS);
    usAmtDLen = _tcslen(aszSpecAmtD);

    if ( fsType == PRT_SINGLE )  {
        PmgPrintf(sPortion, aszPrtRJMnemMnem, aszTranMnem, aszSpecAmtS);
        return;
    }

    if ( usMnemLen+usAmtDLen+1 <= usPrtRJColumn ) {
        /* -- double print -- */
        PmgPrintf(sPortion, aszPrtRJMnemMnem, aszTranMnem, aszSpecAmtD);
        return;
    }

    if ( usMnemLen+usAmtSLen+1 <= usPrtRJColumn ) {
        /* -- single print -- */
        PmgPrintf(sPortion, aszPrtRJMnemMnem, aszTranMnem, aszSpecAmtS);
    } else {
        if ( usAmtDLen <= usPrtRJColumn ) {
            /* -- double print -- */
            PmgPrintf(sPortion, aszPrtRJMnemMnem, aszTranMnem, aszSpecAmtD);
        } else {
            /* -- single print -- */
            PmgPrintf(sPortion, aszPrtRJMnemMnem, aszTranMnem, aszSpecAmtS);
        }
    }
}

/*
*===========================================================================
** Format  : VOID  PrtRJAmtSymEuro(UCHAR uchAdr1, UCHAR uchAdr2, LONG lAmount, UCHAR fbStatus)
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
VOID  PrtRJAmtSymEuro(USHORT usTranAdr1, UCHAR uchAdr2, DCURRENCY lAmount, UCHAR fbStatus)
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
        aszFMnem[0] = _T('-');
        /* -- get foreign symbol -- */
		RflGetSpecMnem(&aszFMnem[1], uchAdr2);
    } else {
        /* -- get foreign symbol -- */
		RflGetSpecMnem(aszFMnem, uchAdr2);
    }

    PmgPrintf(PrtChkPort(), aszPrtRJEuroAmtMnem, aszTranMnem, aszFMnem, sDecPoint, RflLLabs(lAmount));
}

/*
*===========================================================================
** Format   : VOID PrtRJTblPerson( USHORT usTblNo,
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
VOID PrtRJTblPerson( USHORT usTblNo, USHORT usNoPerson, TCHAR uchSeatNo, SHORT sWidthType )
{
	TCHAR   aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 }; /* PARA_... defined in "paraequ.h" */
    TCHAR   aszSpecMnem[ PARA_SPEMNEMO_LEN + 1 ] = { 0 };   /* PARA_... defined in "paraequ.h" */
    TCHAR   aszLeadThru[ PARA_LEADTHRU_LEN + 1 ] = { 0 };
    TCHAR   aszTblNo[ PRT_ID_LEN + 1 ] = { 0 };
    TCHAR   aszNoPer[ PRT_ID_LEN + 1 ] = { 0 };
    TCHAR   aszTransNo[ PRT_ID_LEN * 2 + 1 ] = { 0 };

    /* --- set up special mnemonic of table no., and table no. --- */
    if ( usTblNo != 0 ) {
        _itot( usTblNo, aszTblNo, 10 );
        if ( sWidthType == PRT_DOUBLE ) {
            /* --- set up unique transaction number string --- */
            PrtDouble( aszTransNo, ( PRT_ID_LEN * 2 + 1 ), aszTblNo );
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
        if ( usTblNo ) {
            PmgPrintf( PrtChkPort(), aszPrtRJTransNo, aszLeadThru, aszTransNo );
        }

        /* --- print no. of person with single width --- */
        if ( usNoPerson || uchSeatNo ) {
            PmgPrintf( PrtChkPort(), aszPrtRJPerson, aszTranMnem, aszNoPer );
        }
    } else {                            /* print with single width */
        if ( usTblNo || (usNoPerson || uchSeatNo) ) {
            /* --- print table no. and no. of person with single width --- */
            PmgPrintf( PrtChkPort(), aszPrtRJTblPerson, aszSpecMnem, aszTblNo, aszTranMnem, aszNoPer );
        }
    }
}

/*
*===========================================================================
** Format   : VOID PrtRJCustomerName( UCHAR *pszCustomerName )
*
*   Input   : UCHAR *pszCustomerName    -   address of customer name buffer
*   Output  : none
*   InOut   : none
** Return   : none
*
** Synopsis : This function prints customer name (max. 16 chara).
*===========================================================================
*/
VOID PrtRJCustomerName( TCHAR *pszCustomerName )
{
    if ( *pszCustomerName == '\0' ) {
        return;
    }

    if ( *( pszCustomerName + NUM_NAME - 2 ) == PRT_DOUBLE ) {
        *( pszCustomerName + NUM_NAME - 2 ) = '\0';
    }

    PmgPrintf( PrtChkPort(), aszPrtRJCustomerName, pszCustomerName );
}

/*
*===========================================================================
** Format  : VOID   PrtRJMultiSeatNo(TRANINFORMATION *pTran);
*
*   Input  : ITEMSALES *pItem           -seat no
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function formats seat no. line to . R3.1
*===========================================================================
*/
VOID    PrtRJMultiSeatNo(TRANINFORMATION *pTran)
{
	TCHAR   aszPrintBuff[PRT_RJCOLUMN + 1] = { 0 };
	TCHAR   aszSpecMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };   /* PARA_... defined in "paraequ.h" */
    USHORT  i, usPos;

    if (pTran->TranGCFQual.auchFinSeat[0] == 0) {
        return;
    }

	RflGetSpecMnem( aszSpecMnem, SPC_SEAT_ADR );    /* seat no. mnemonic */
    usPos = _tcslen(aszSpecMnem);

    _tcsncpy(&aszPrintBuff[0], aszSpecMnem, usPos);

    /* ---- set multiple seat no. ---- */
    for (i=0; i<NUM_SEAT; i++) { //SR206
        if (pTran->TranGCFQual.auchFinSeat[i]) {
            aszPrintBuff[usPos++] = (TCHAR)0x20;
            aszPrintBuff[usPos++] = (TCHAR)(pTran->TranGCFQual.auchFinSeat[i] | 0x30);
        } else {
            break;
        }
    }

    aszPrintBuff[usPos++] = '\0';

    PmgPrintf(PrtChkPort(), aszPrtRJMultiSeat, aszPrintBuff);
}


/*
*===========================================================================
** Format  : VOID  PrtRJTranNum(UCHAR uchAdr, ULONG ulNumber);
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
VOID  PrtRJTranNum(USHORT usAdr, ULONG ulNumber)
{
    TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1];/* PARA_... defined in "paraequ.h" */

    /* -- get transaction mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    PmgPrintf(PrtChkPort(), aszPrtRJMnemNum, aszTranMnem, ulNumber);
}

/*
*===========================================================================
** Format  : VOID  PrtRJKitchen(UCHAR  uchKPNo);
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
VOID  PrtRJKitchen(UCHAR  uchKPNo)
{
    TCHAR  aszMnem[PARA_CHAR24_LEN + 1];

    /* -- inititalize -- */
    tcharnset(aszMnem, PRT_SPACE, PARA_CHAR24_LEN + 1);

    /* -- get 24 char mnemonics -- */
    PrtGet24Mnem(aszMnem, CH24_KTCHN_ADR);

	/* -- send kitchen information -- */
    PmgPrint(PMG_PRT_RECEIPT, aszMnem, usPrtRJColumn);

    /* -- inititalize -- */
    tcharnset(aszMnem, PRT_SPACE, PARA_CHAR24_LEN + 1);

    /* -- get lead through mnemonics -- */
    PrtGetLead(aszMnem, LDT_KTNERR_ADR);

	/* -- send kitchen information -- */
    PmgPrint(PMG_PRT_RECEIPT, aszMnem, usPrtRJColumn);

	/* -- send error kitchen printer -- */
    PmgPrintf(PMG_PRT_RECEIPT, aszPrtRJPrinNo, uchKPNo); 
}

/*
*===========================================================================
** Format  : VOID  PrtRJWaiter(USHORT usWaiID);
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
VOID  PrtRJWaiter(ULONG ulWaiID)
{
    if (ulWaiID != 0) {
		TCHAR  aszWaiMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };  /* PARA_... defined in "paraequ.h" */

        /* -- get waiter mnemonics -- */
		RflGetSpecMnem(aszWaiMnem, SPC_CAS_ADR);
        PmgPrintf(PrtChkPort(), aszPrtRJWaiter, aszWaiMnem, RflTruncateEmployeeNumber(ulWaiID));
    }
}

/*
*===========================================================================
** Format  : VOID  PrtRJForeign1(LONG lForeign, LONG lRate, UCHAR uchAdr,
*                                                           UCHAR fbStatus);
*   Input  : LONG  lForeign,      -Foreign tender amount
*            UCHAR uchAdr         -Foreign symbol address
*            UCHAR fbStatus       -Foreign currency decimal point status
*
*   Output : none
*   InOut  : none
*
** Return  : none
*
** Synopsis: This function prints foeign tender line.
*            
*===========================================================================
*/
VOID  PrtRJForeign1(DCURRENCY lForeign, UCHAR uchAdr, UCHAR fbStatus)
{
	TCHAR  aszFMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };  /* PARA_... defined in "paraequ.h" */
	TCHAR  aszFAmt[PRT_RJCOLUMN + 1] = { 0 };
    USHORT usStrLen;
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

    /* -- get foreign symbol -- */
	RflGetSpecMnem(aszFMnem, uchAdr);

    /* -- get string length -- */
    usStrLen = _tcslen(aszFMnem);

    /* -- format foreign amount -- */
    RflSPrintf(&aszFAmt[usStrLen], TCHARSIZEOF(aszFAmt), aszPrtRJForeign1, sDecPoint, lForeign);

    /* -- adjust sign ( + , - ) -- */
    if (lForeign < 0) {
        aszFAmt[0] = _T('-');
        _tcsncpy(&aszFAmt[1], aszFMnem, usStrLen);
    } else {
        _tcsncpy(aszFAmt, aszFMnem, usStrLen);
    }
/***  modified '93-5-27 
    PmgPrint(PrtChkPort(), aszFAmt, usPrtRJColumn);
***/
    PmgPrint(PrtChkPort(), aszFAmt, (USHORT)_tcslen(aszFAmt)); /* ### Mod (2171 for Win32) V1.0 */
}

/*
*===========================================================================
** Format  : VOID  PrtRJForeign2(ULONG ulRate);
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
VOID  PrtRJForeign2(ULONG ulRate, UCHAR fbStatus2)
{
    SHORT sDecPoint;

    /* 6 digit decimal point for Euro, V3.4 */
    if (fbStatus2 & ODD_MDC_BIT0) {
        sDecPoint = PRT_6DECIMAL;
    } else {
        sDecPoint = PRT_5DECIMAL;
    }

    PmgPrintf(PrtChkPort(), aszPrtRJForeign2, sDecPoint, ulRate);
}

/*
*===========================================================================
** Format  : VOID  PrtRJEuro(uchAdr1, LONG lForeign, UCHAR uchAdr2, ULONG ulRate)
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
VOID  PrtRJEuro(UCHAR uchAdr1, DCURRENCY lForeign, UCHAR uchAdr2, ULONG ulRate, UCHAR fbStatus2)
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

    PmgPrintf(PrtChkPort(), aszPrtRJEuro1, aszFMnem1, lForeign, aszFMnem2, sDecPoint, ulRate);
}

/*
*===========================================================================
** Format  : VOID   PrtRJSeatNo(ITEMSALES *pItem);
*
*   Input  : ITEMSALES *pItem           -seat no
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function formats seat no. line to . R3.1
*===========================================================================
*/
VOID    PrtRJSeatNo(TCHAR uchSeatNo)
{
	TCHAR   aszSpecMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };   /* PARA_... defined in "paraequ.h" */

    if (uchSeatNo == 0) {
        return;
    }

	RflGetSpecMnem( aszSpecMnem, SPC_SEAT_ADR );    /* seat no. mnemonic */

    PmgPrintf(PrtChkPort(), aszPrtRJSeat, aszSpecMnem, uchSeatNo);
}


/*
*===========================================================================
** Format  : VOID   PrtRJSeatNo2(ITEMSALES *pItem);
*
*   Input  : ITEMSALES *pItem           -seat no
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function formats seat no. line to . R3.1
*===========================================================================
*/
VOID    PrtRJSeatNo2(TCHAR uchSeatNo)
{
    if (uchPrtSeatNo == uchSeatNo) {
        return;
    }

    if (uchSeatNo) {
		TCHAR   aszSpecMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };   /* PARA_... defined in "paraequ.h" */

		RflGetSpecMnem( aszSpecMnem, SPC_SEAT_ADR );    /* seat no. mnemonic */

        PmgPrintf(PrtChkPort(), aszPrtRJSeat, aszSpecMnem, uchSeatNo);
    } else {
        PrtRFeed(1);
    }

    uchPrtSeatNo = uchSeatNo;
}


/*
*===========================================================================
** Format  : VOID  PrtRJQty(ITEMSALES *pItem);
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
VOID  PrtRJQty(TRANINFORMATION *pTran, ITEMSALES *pItem)
{
    DCURRENCY  lPrice = 0;
    USHORT     i;
    USHORT     usNoOfChild;
	TCHAR      auchDummy[NUM_PLU_LEN] = {0};

    if (labs(pItem->lQTY) == PLU_BASE_UNIT) {   /* not print 1 qty */
        return ;
    }

    /* --- consolidate condiment PLU's price --- */
    usNoOfChild = pItem->uchCondNo + pItem->uchPrintModNo + pItem->uchChildNo;

    lPrice = 0L;
    for ( i = pItem->uchChildNo; i < usNoOfChild; i++ ) {
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

        PmgPrintf(PrtChkPort(), aszPrtRJQuant, pItem->lQTY / PLU_BASE_UNIT, lPrice);
    } else {
       if (PrtChkVoidConsolidation(pTran, pItem) == FALSE) {
            /* not consolidation print */
            if (pItem->uchPPICode) {    /* print "QTY * Product / QTY" */
                PmgPrintf(PrtChkPort(), aszPrtRJPPIQuant, pItem->lQTY / PLU_BASE_UNIT, RflLLabs(lPrice), labs(pItem->lQTY / PLU_BASE_UNIT));
            } else
            if (pItem->uchPM) {    /* print "QTY * Product / PM, 2172" */
                PmgPrintf(PrtChkPort(), aszPrtRJPPIQuant, pItem->lQTY / PLU_BASE_UNIT, RflLLabs(lPrice), pItem->uchPM);
            } else {
				//SR 144 perform MDC check to determine if Qty prints above item line or on the same line
				if(!CliParaMDCCheck(MDC_QTY2LINE_ADR, EVEN_MDC_BIT3)){
					PmgPrintf(PrtChkPort(), aszPrtRJQuant, pItem->lQTY / PLU_BASE_UNIT, lPrice);
				} else {
				}
            }
        } else {
            /* consolidation void print */
            if (pItem->uchPM) {    /* print "QTY * Product / PM, 2172" */
                PmgPrintf(PrtChkPort(), aszPrtRJPPIQuant, pItem->lQTY / PLU_BASE_UNIT, RflLLabs(lPrice), pItem->uchPM);
            } else
            if (abs(pItem->sCouponQTY) != 1) {
				//SR 144 perform MDC check to determine if Qty prints above item line or on the same line
				if(!CliParaMDCCheck(MDC_QTY2LINE_ADR, EVEN_MDC_BIT3)){
					PmgPrintf((USHORT)(PrtChkPort() & PMG_PRT_RECEIPT), aszPrtRJQuant, (LONG)pItem->sCouponQTY, lPrice);
				} else {
				}
            }
			//SR 144 perform MDC check to determine if Qty prints above item line or on the same line
			if(!CliParaMDCCheck(MDC_QTY2LINE_ADR, EVEN_MDC_BIT3)){
				PmgPrintf((USHORT)(PrtChkPort() & PMG_PRT_JOURNAL), aszPrtRJQuant, pItem->lQTY / PLU_BASE_UNIT, lPrice);
			} else {
			}
        }
    }
}

/*
*===========================================================================
** Format  : VOID  PrtRJQty(ITEMSALES *pItem);
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
VOID  PrtRJLinkQty(TRANINFORMATION *pTran, ITEMSALES *pItem)
{
    if (labs(pItem->lQTY) == PLU_BASE_UNIT) {
        return ;
    } else {
		DCURRENCY  lPrice = pItem->Condiment[0].lUnitPrice;

        PmgPrintf((USHORT)PrtChkPort(), aszPrtRJQuant, pItem->lQTY / PLU_BASE_UNIT, lPrice);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtRJWeight(TRANINFORMATION  *pTran, ITEMSALES *pItem);
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
VOID  PrtRJWeight(TRANINFORMATION  *pTran, ITEMSALES  *pItem)
{

	TCHAR  aszWeightSym[PARA_SPEMNEMO_LEN + 1] = { 0 }; /* PARA_... defined in "paraequ.h" */
	TCHAR  aszManuMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 }; /* PARA_... defined in "paraequ.h" */
    SHORT  sDecPoint;                           /* decimal point */
    LONG   lModQty;                             /* modified qty */
    
    /* -- get scalable symbol and adjust weight -- */
    PrtGetScale(aszWeightSym, &sDecPoint, &lModQty, pTran, pItem);

    /* -- get Manual weight mnemonics -- */
    if (pItem->fsPrintStatus & PRT_MANUAL_WEIGHT) {
		RflGetTranMnem(aszManuMnem, TRN_MSCALE_ADR);
    } else {
        aszManuMnem[0] = '\0';
    }

    /* -- send "weight " and Manual weight mnemonics -- */
    PmgPrintf(PrtChkPort(), aszPrtRJWeight1, sDecPoint, (DWEIGHT)lModQty, aszWeightSym, aszManuMnem);

    PmgPrintf(PrtChkPort(), aszPrtRJWeight2, (DWEIGHT)pItem->lUnitPrice, aszWeightSym);
}
/*
*===========================================================================
** Format  : VOID  PrtRJChild(UCHAR uchAdj, UCHAR *pszMnem);
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
VOID  PrtRJChild(UCHAR uchAdj, TCHAR *pszMnem)
{
    if (uchAdj != 0) {
		TCHAR    aszAdjMnem[PARA_ADJMNEMO_LEN + 1] = { 0 };
		TCHAR    aszPLUMnem[PARA_PLU_LEN + 1] = { 0 };

        /* --get adjective mnemocnics -- */
		RflGetAdj(aszAdjMnem,  uchAdj);
        
        /* 2172 */
        _tcsncpy(aszPLUMnem, pszMnem, PARA_PLU_LEN);
        if (_tcslen(aszPLUMnem) > 18) {  /* overs RJ column */
            aszPLUMnem[18] = 0;
        }

        /* -- send adjective mnemonics and child plu mnemonics -- */
        PmgPrintf(PrtChkPort(), aszPrtRJChild, aszAdjMnem, aszPLUMnem);
    } else {
        /* print only child mnemonic, 2172 */
        PmgPrintf(PrtChkPort(), aszPrtRJChild2, pszMnem);
    }
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
VOID  PrtRJLinkPLU(USHORT fsModified, UCHAR uchAdj, TCHAR *pszMnem, DCURRENCY lPrice)
{
    if (uchAdj != 0) {
		TCHAR    aszAdjMnem[PARA_ADJMNEMO_LEN + 1] = {0};
		TCHAR    aszPLUMnem[PARA_PLU_LEN + 1] = {0};

        /* --get adjective mnemocnics -- */
		RflGetAdj(aszAdjMnem,  uchAdj);
        
        /* 2172 */
        _tcsncpy(aszPLUMnem, pszMnem, PARA_PLU_LEN);
        if (_tcslen(aszPLUMnem) > 18) {  /* overs RJ column */
            aszPLUMnem[18] = 0;
        }
        /* -- send adjective mnemonics and child plu mnemonics -- */
        if (fsModified) {
            PmgPrintf(PrtChkPort(), aszPrtRJModLinkPLU1, aszAdjMnem, aszPLUMnem, lPrice);
        } else {
            PmgPrintf(PrtChkPort(), aszPrtRJLinkPLU1, aszAdjMnem, aszPLUMnem, lPrice);
        }
    } else {
        /* print only child mnemonic, 2172 */
        if (fsModified) {
            PmgPrintf(PrtChkPort(), aszPrtRJModLinkPLU2, pszMnem, lPrice);
        } else {
            PmgPrintf(PrtChkPort(), aszPrtRJLinkPLU2, pszMnem, lPrice);
        }
    }
}

/*
*===========================================================================
** Format  : VOID  PrtRJItems(ITEMSALES  *pItem);
*
*   Input  : ITEMSALES  *pItem    -structure "itemsales" pointer
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints adjective, noun mnemo.,  print modifier, conidiment line.
*===========================================================================
*/
VOID  PrtRItems(TRANINFORMATION *pTran, ITEMSALES  *pItem)
{
	TCHAR      aszItem[PRT_MAXITEM + PRT_RJCOLUMN] = {0};   /* "+PRT_RJCOLUMN" for PrtGet1Line */
	DCURRENCY  lProduct;
    TCHAR      *pszStart;
    TCHAR      *pszEnd;
    SHORT      sPrintType, i;
	BOOL	   bVoidCons;

    pszStart = aszItem;

    /* --- set condiment print status by mdc, R3.1 --- */
    sPrintType = PrtChkPrintType(pItem);

    /* -- set item mnenemonics to buffer -- */
    PrtSetItem(aszItem, pItem, &lProduct, sPrintType, 0);
	
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
    } else {
		D13DIGITS d13Work;

        /* void consolidation print */
        /* recalculate sales price from sCouponQTY */
        d13Work = pItem->lUnitPrice;
        for (i = pItem->uchChildNo; i < STD_MAX_COND_NUM; i++) {
           d13Work += pItem->Condiment[i].lUnitPrice;
        }
        /* --- calcurate product * qty --- */
        lProduct *= (LONG)pItem->sCouponQTY;
    }

    for (;;) {
        /* -- get 1 line -- */
        if ( PRT_LAST == PrtGet1Line(pszStart, &pszEnd,
            (USHORT)(usPrtRJColumn - PRT_AMOUNTLEN)) ) { /* ### Mod (2171 for Win32) V1.0 */
            break;
        }

        /* send adjective, noun mnemo, print modifier, condiment */
        PmgPrint((USHORT)(PrtChkPort() & PMG_PRT_RECEIPT), pszStart, (USHORT)(pszEnd - pszStart)); /* ### Mod (2171 for Win32) V1.0 */

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
			PmgPrintf((USHORT)(PrtChkPort() & PMG_PRT_RECEIPT),  aszPrtRJModAmtMnem, pszStart, lProduct); /* ### Mod (2171 for Win32) V1.0 */
		} else {
			if(bVoidCons){
				PmgPrintf((USHORT)(PrtChkPort() & PMG_PRT_RECEIPT),  aszPrtRJModAmtMnem1Line, pItem->sCouponQTY, pItem->lUnitPrice, pszStart, lProduct);
			}else{
				PmgPrintf((USHORT)(PrtChkPort() & PMG_PRT_RECEIPT),  aszPrtRJModAmtMnem1Line, pItem->lQTY, pItem->lUnitPrice, pszStart, lProduct);
			}
		}
    } else {
		//SR 144 perform MDC check to determine if Qty prints above item line or on the same line
		if(!CliParaMDCCheck(MDC_QTY2LINE_ADR, EVEN_MDC_BIT3)){
			PmgPrintf((USHORT)(PrtChkPort() & PMG_PRT_RECEIPT),  aszPrtRJAmtMnem, pszStart, lProduct); /* ### Mod (2171 for Win32) V1.0 */
		} else {
			if(bVoidCons){
				PmgPrintf((USHORT)(PrtChkPort() & PMG_PRT_RECEIPT),  aszPrtRJAmtMnem1Line, pItem->sCouponQTY, pItem->lUnitPrice, pszStart, lProduct);
			}else{
				PmgPrintf((USHORT)(PrtChkPort() & PMG_PRT_RECEIPT),  aszPrtRJAmtMnem1Line, pItem->lQTY, pItem->lUnitPrice, pszStart, lProduct);
			}
		}
    }
}

/*
*===========================================================================
** Format  : VOID  PrtRJItems(ITEMSALES  *pItem);
*
*   Input  : ITEMSALES  *pItem    -structure "itemsales" pointer
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints adjective, noun mnemo.,  print modifier, conidiment line.
*===========================================================================
*/
VOID  PrtJItems(ITEMSALES  *pItem)
{
	TCHAR      aszItem[PRT_MAXITEM + PRT_RJCOLUMN] = {0};   /* "+PRT_RJCOLUMN" for PrtGet1Line */
    DCURRENCY  lProduct;
    TCHAR      *pszStart;
    TCHAR      *pszEnd;

    pszStart = aszItem;

    /* -- set item mnenemonics to buffer -- */
    PrtSetItem(aszItem, pItem, &lProduct, PRT_ALL_ITEM, 0);

    for (;;) {
        /* -- get 1 line -- */
        if ( PRT_LAST == PrtGet1Line(pszStart, &pszEnd,
            (USHORT)(usPrtRJColumn - PRT_AMOUNTLEN)) ) { /* ### Mod (2171 for Win32) V1.0 */
            break;
        }

        /* send adjective, noun mnemo, print modifier, condiment */
        PmgPrint((USHORT)(PrtChkPort() & PMG_PRT_JOURNAL), pszStart, (USHORT)(pszEnd - pszStart)); /* ### Mod (2171 for Win32) V1.0 */

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
			PmgPrintf((USHORT)(PrtChkPort() & PMG_PRT_JOURNAL),  aszPrtRJModAmtMnem, pszStart, lProduct); /* ### Mod (2171 for Win32) V1.0 */
		} else {
			PmgPrintf((USHORT)(PrtChkPort() & PMG_PRT_JOURNAL),  aszPrtRJModAmtMnem1Line, pItem->lQTY, pItem->lUnitPrice, pszStart, lProduct); /* ### Mod (2171 for Win32) V1.0 */
		}
    } else {
		//SR 144 perform MDC check to determine if Qty prints above item line or on the same line
		if(!CliParaMDCCheck(MDC_QTY2LINE_ADR, EVEN_MDC_BIT3)){
			PmgPrintf((USHORT)(PrtChkPort() & PMG_PRT_JOURNAL),  aszPrtRJAmtMnem, pszStart, lProduct);
		} else {
			PmgPrintf((USHORT)(PrtChkPort() & PMG_PRT_JOURNAL),  aszPrtRJAmtMnem1Line, pItem->lQTY, pItem->lUnitPrice, pszStart, lProduct);
		}
    }
}

/*
*===========================================================================
** Format  : VOID  PrtRJZeroAmtMnem(UCHAR uchAddress, LONG lAmount);
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
VOID  PrtRJZeroAmtMnem(UCHAR uchAddress, DCURRENCY lAmount)
{
    if (lAmount != 0L) {
        PrtRJAmtMnem(uchAddress, lAmount);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtRJTrail2(SHORT sPortion, TRANINFORMATION  *pTran);
*
*   Input  : SHORT           sPortion       -print portion 
*            TRANINFORMATION *pTran         -transaction information
*
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints trailer 2nd line.
*===========================================================================
*/
VOID PrtRJTrail2(SHORT sPortion, TRANINFORMATION  *pTran)
{
    TCHAR   aszDate[PRT_DATETIME_LEN + 1];

    PrtGetDate(aszDate, TCHARSIZEOF(aszDate), pTran);   /* get date time */

    PmgPrintf(sPortion, aszPrtRJTrail2, pTran->TranCurQual.usConsNo, aszDate);
}

/*
*===========================================================================
** Format  : VOID  PrtRJHead(VOID);
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function receipt header.
*===========================================================================
*/
VOID PrtRJHead(VOID)
{
    
    UCHAR i, j = 0;
    TCHAR asz24Mnem[PARA_CHAR24_LEN + 1];
    
    for ( i = CH24_1STRH_ADR; i <= CH24_4THRH_ADR; i++) {
     
        PrtGet24Mnem(asz24Mnem, i);

        if (asz24Mnem[0] == '\0') {
            j++;
        } else {
            /* --  send headder -- */
            PmgPrint(PMG_PRT_RECEIPT, asz24Mnem, usPrtRJColumn);
        }
    }
    if (j) {
        PrtFeed(PMG_PRT_RECEIPT, j);            /* feed */
    }
}

/*
*===========================================================================
** Format  : VOID  PrtRJDoubleHead(VOID);
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : none
*                
** Synopsis: This functions prints Double Header mnemo line. 
*===========================================================================
*/
VOID  PrtRJDoubleHead(VOID)
{
    TCHAR  aszDouble[PARA_TRANSMNEMO_LEN + 1];

	RflGetTranMnem(aszDouble, TRN_DBLRID_ADR);

    PmgPrint(PMG_PRT_RECEIPT, aszDouble, (USHORT)_tcslen(aszDouble)); /* ### Mod (2171 for Win32) V1.0 */
}


/*
*===========================================================================
** Format  : VOID  PrtRJGuest(USHORT usGuestNo, UCHAR uchCDV);
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
VOID  PrtRJGuest(USHORT usGuestNo, UCHAR uchCDV)
{
    TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1];/* PARA_... defined in "paraequ.h" */

    if (usGuestNo == 0) {                       /* GCF# 0, doesnot print */
        return ;
    }

    /* -- get transaction mnemonics -- */
	RflGetTranMnem(aszTranMnem, TRN_GCNO_ADR);

    if (uchCDV == 0) {
        PmgPrintf(PrtChkPort(), aszPrtRJGuest_WOCD, aszTranMnem, usGuestNo);
    } else {
        PmgPrintf(PrtChkPort(), aszPrtRJGuest_WTCD, aszTranMnem, usGuestNo, (USHORT)uchCDV);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtRJMulChk(USHORT usGuestNo, UCHAR uchCDV);
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
VOID  PrtRJMulChk(USHORT usGuestNo, UCHAR uchCDV)
{
    TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1];/* PARA_... defined in "paraequ.h" */

    /* -- get transaction mnemonics -- */
	RflGetTranMnem(aszTranMnem, TRN_CKPD_ADR);

    if (usGuestNo == 0) {                       /* GCF# 0, doesnot print */
        return ;
    }

    if (uchCDV == 0) {
        PmgPrintf(PrtChkPort(), aszPrtRJGuest_WOCD, aszTranMnem, usGuestNo);
    } else {
        PmgPrintf(PrtChkPort(), aszPrtRJGuest_WTCD, aszTranMnem, usGuestNo, (USHORT)uchCDV);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtRFeed(USHORT usLineNum);
*
*   Input  : USHORT   usLineNum      -feed line number
*   Output : none
*   InOut  : none
** Return  : none
*                
** Synopsis: This functions make recepit printer feed. 
*===========================================================================
*/
VOID  PrtRFeed(USHORT usLineNum)
{
    if ( fsPrtPrintPort & PRT_RECEIPT ) {
        PrtFeed(PRT_RECEIPT, usLineNum);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtRJWaiTaxMod(USHORT usID, USHORT fsTax, UCHAR  fbMod);
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
VOID  PrtRJWaiTaxMod(ULONG ulID, USHORT  fsTax, USHORT  fbMod)
{
	TCHAR  aszTaxMod[PRT_RJCOLUMN + 1] = { 0 };
	TCHAR  aszWaiMnem[PARA_SPEMNEMO_LEN + 2] = { 0 };  /* PARA_... defined in "paraequ.h" */
    SHORT  sRet;

    if (ulID != 0) {
        /* -- get waiter mnemonics -- */
		RflGetSpecMnem(aszWaiMnem, SPC_CAS_ADR);
    }

    /* -- get tax modifier mnemonics -- */
    sRet = PrtGetTaxMod(aszTaxMod, TCHARSIZEOF(aszTaxMod), fsTax, fbMod);

    /* if ID=0 and not exist taxmodifer, not print line */
    if (ulID != 0 && sRet == PRT_WITHDATA) {
        PmgPrintf(PrtChkPort(), aszPrtRJWaiTaxMod, aszWaiMnem, RflTruncateEmployeeNumber(ulID), aszTaxMod);
    } else if (ulID != 0) {
        PmgPrintf(PrtChkPort(), aszPrtRJWaiter, aszWaiMnem, RflTruncateEmployeeNumber(ulID));
    } else if (sRet == PRT_WITHDATA) {
        PmgPrint(PrtChkPort(), aszTaxMod, (USHORT)_tcslen(aszTaxMod)); /* ### Mod (2171 for Win32) V1.0 */
    }
}

/*
*===========================================================================
** Format  : VOID  PrtRJMnemCount(UCHAR uchAdr, SHORT sCount);
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
VOID  PrtRJMnemCount(USHORT usAdr, SHORT sCount)
{
    TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1];  /* PARA_... defined in "paraequ.h" */

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- send mnemoncis and amount -- */
    PmgPrintf(PrtChkPort(), aszPrtRJMnemCount, aszTranMnem, sCount);
}

/*
*===========================================================================
** Format  : VOID  PrtRJTrail1_24(SHORT sPortion, TRANINFORMATION  *pTran,
*                                                        ULONG  ulStReg);
*
*   Input  : SHORT           sPortion  -print portion
*            TRANINFORMATION *pTran    -transaction information
*            ULONG           ulStReg   -store reg number
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints normal trailer 1st line.
*===========================================================================
*/
VOID  PrtRJTrail1_24(SHORT sPortion, TRANINFORMATION *pTran, ULONG  ulStReg)
{
	CONST TCHAR   *aszPrtRJTrail1_24  = _T("%8.8Mu %s\t%04lu-%03lu");       /* trailer 1st line */
	CONST TCHAR   *aszPrtRJTrail1_24x  = _T("         %s\t%04lu-%03lu");    /* trailer 1st line */

	extern CONST TCHAR FARCONST  aszCasWaiID[];
	TCHAR  aszMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };
    ULONG  ulID;
    TCHAR  aszID[8 + 1] = {0, 0, 0, 0};
	TCHAR  aszName[PARA_CASHIER_LEN + 1] = { 0 };

    if ( PRT_CASHIER == PrtChkCasWai(aszMnem, pTran->TranModeQual.ulCashierID, pTran->TranModeQual.ulWaiterID) ) {
        ulID = pTran->TranModeQual.ulCashierID;
        _tcsncpy(aszName, pTran->TranModeQual.aszCashierMnem, PRT_CASNAME_LEN);
    } else {
        ulID = pTran->TranModeQual.ulWaiterID;
        _tcsncpy(aszName, pTran->TranModeQual.aszWaiterMnem, PRT_CASNAME_LEN);
    }

	if (ulID != 0)
		PmgPrintf(sPortion, aszPrtRJTrail1_24, RflTruncateEmployeeNumber(ulID), aszName, ulStReg / 1000, ulStReg % 1000 );
	else
		PmgPrintf(sPortion, aszPrtRJTrail1_24x, aszName, ulStReg / 1000, ulStReg % 1000 );
}

/*
*===========================================================================
** Format  : VOID  PrtRJTickTrail_24(SHORT sPortion, TRANINFORMATION  *pTran);
*
*   Input  : SHORT           sPortion  -print portion 
*            TRANINFORMATION *pTran    -transaction information
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints ticket 1st line.
*===========================================================================
*/
VOID  PrtRJTickTrail_24(SHORT sPortion, TRANINFORMATION  *pTran)
{
	CONST TCHAR FARCONST  *aszPrtRJTickTrail_24  = _T("%-4s%8.8Mu %-4s%4s\t %s"); /* trailer 1st line(ticket) with CD */

	TCHAR  aszCasMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };
	TCHAR  aszTblMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };
	TCHAR  aszGuest[6 + 1] = { 0 };
	TCHAR  aszTblNo[6 + 1] = { 0 };
    ULONG  ulID = 0;
    ULONG  ulGuest = 0;

    /* -- cashier or waiter? -- */
    if ( PRT_CASHIER == PrtChkCasWai(aszCasMnem, pTran->TranModeQual.ulCashierID, pTran->TranModeQual.ulWaiterID) ) {
        ulID = RflTruncateEmployeeNumber(pTran->TranModeQual.ulCashierID);
    } else {
        ulID = RflTruncateEmployeeNumber(pTran->TranModeQual.ulWaiterID);
    }

    if (pTran->TranGCFQual.usTableNo != 0) {
        if ( ! ( pTran->TranCurQual.flPrintStatus & CURQUAL_UNIQUE_TRANS_NO )) {
			RflGetSpecMnem(aszTblMnem, SPC_TBL_ADR);      /* get table mnemonics */
            _itot(pTran->TranGCFQual.usTableNo, aszTblNo, 10);
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
        _ultot(ulGuest, aszGuest, 10);
    }

    PmgPrintf(sPortion, aszPrtRJTickTrail_24, aszCasMnem, ulID, aszTblMnem, aszTblNo, aszGuest);
}

/*
*===========================================================================
** Format  : VOID  PrtRJTrail1_21(SHORT sPortion, TRANINFORMATION  *pTran,
*                                                        ULONG  ulStReg);
*
*   Input  : SHORT           sPortion  -print portion
*            TRANINFORMATION *pTran    -transaction information
*            ULONG           ulStReg   -store reg number
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints normal trailer 1st line.
*===========================================================================
*/
VOID  PrtRJTrail1_21(SHORT sPortion, TRANINFORMATION *pTran, ULONG  ulStReg)
{
	CONST TCHAR FARCONST  *aszPrtRJTrail1_21  = _T("%8.8Mu %s\t%04lu-%03lu");        /* trailer 1st line */

	extern CONST TCHAR FARCONST  aszCasWaiID[];
	TCHAR  aszMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };
    ULONG  ulID;
    TCHAR  *pszName;
	TCHAR  aszName[PARA_CASHIER_LEN + 1] = { 0 };
    USHORT usLen;
    TCHAR  aszID[8 + 1] = {0, 0, 0, 0};

    if ( PRT_CASHIER == PrtChkCasWai(aszMnem, pTran->TranModeQual.ulCashierID, pTran->TranModeQual.ulWaiterID) ) {
        ulID = RflTruncateEmployeeNumber(pTran->TranModeQual.ulCashierID);
        pszName = pTran->TranModeQual.aszCashierMnem;

    } else {
        ulID = RflTruncateEmployeeNumber(pTran->TranModeQual.ulWaiterID);
        pszName = pTran->TranModeQual.aszWaiterMnem;
    }

    /* -- adjust mnemonics's column -- */
    if ( (usLen = _tcslen(pszName)) >= PRT_CASNAME21_LEN ) { /* V3.3 */
        usLen = PRT_CASNAME21_LEN;
        _tcsncpy(aszName, pszName, usLen);
    } else {
        aszName[0] = PRT_SPACE;
        _tcsncpy(&aszName[1], pszName, usLen);
        usLen++;
    }

    aszName[usLen] = '\0';

    PmgPrintf(sPortion, aszPrtRJTrail1_21, ulID, aszName, ulStReg / 1000, ulStReg % 1000 );
}

/*
*===========================================================================
** Format  : VOID  PrtRJTickTrail_21(SHORT sPortion, TRANINFORMATION  *pTran);
*
*   Input  : SHORT           sPortion  -print portion 
*            TRANINFORMATION *pTran    -transaction information
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints ticket 1st line.
*===========================================================================
*/
VOID  PrtRJTickTrail_21(SHORT sPortion, TRANINFORMATION  *pTran)
{
	CONST TCHAR   *aszPrtRJTickTrail_21  = _T("%8.8Mu %-4s%4s\t %s");     /* trailer 1st line(ticket) with CD */

	TCHAR  aszCasMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };
	TCHAR  aszTblMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };
	TCHAR  aszGuest[6 + 1] = { 0 };
	TCHAR  aszTblNo[6 + 1] = { 0 };
    ULONG  ulID = 0;
    ULONG  ulGuest = 0;

    /* -- cashier or waiter? -- */
    if ( PRT_CASHIER == PrtChkCasWai(aszCasMnem, pTran->TranModeQual.ulCashierID, pTran->TranModeQual.ulWaiterID) ) {
        ulID = RflTruncateEmployeeNumber(pTran->TranModeQual.ulCashierID);
    } else {
        ulID = RflTruncateEmployeeNumber(pTran->TranModeQual.ulWaiterID);
    }

    if (pTran->TranGCFQual.usTableNo != 0) {
        if ( ! ( pTran->TranCurQual.flPrintStatus & CURQUAL_UNIQUE_TRANS_NO )) {
			RflGetSpecMnem(aszTblMnem, SPC_TBL_ADR);      /* get table mnemonics */
            _itot(pTran->TranGCFQual.usTableNo, aszTblNo, 10);
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
        _ultot(ulGuest, aszGuest, 10);
    }

    PmgPrintf(sPortion, aszPrtRJTickTrail_21, ulID, aszTblMnem, aszTblNo, aszGuest);
}

/*
*===========================================================================
** Format  : VOID  PrtRJModScale(LONG lAmount);
*
*   Input  : LONG       lAmount        -amount
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints manual mnem. and unit price
*===========================================================================
*/
VOID  PrtRJModScale(DCURRENCY lAmount)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};

	RflGetTranMnem(aszTranMnem, TRN_MSCALE_ADR);

    PmgPrintf(PrtChkPort(), aszPrtRJModScale, aszTranMnem, lAmount);
}

/*
*===========================================================================
** Format  : VOID  PrtRJEtkCode(ITEMMISC  *pItem, SHORT sType);
*
*   Input  : ITEMMISC   *pItem      -Item Data Address
*            SHORT      sType           -print type
*                                           PRT_WTIN: with time-in
*                                           PRT_WOIN: without time-in
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints ETK employee code and job code.
*===========================================================================
*/
VOID  PrtRJEtkCode(ITEMMISC  *pItem, SHORT sType)
{
	extern CONST TCHAR FARCONST  aszEtkJobCode[];
	CONST TCHAR   *aszPrtRJEtkCode1 = _T("%-16s\n%8.8Mu\t%s");   /* employee, job code */
	CONST TCHAR   *aszPrtRJEtkCode2 = _T("%-s\t%8.8Mu");         /* employee, job code */

    TCHAR   aszJobCode[12];

    if ( ( sType == PRT_WTIN) && ( pItem->uchJobCode != 0 ) ) {
        RflSPrintf(aszJobCode, TCHARSIZEOF(aszJobCode), aszEtkJobCode, pItem->uchJobCode);

        PmgPrintf(PrtChkPort(), aszPrtRJEtkCode1, pItem->aszMnemonic, RflTruncateEmployeeNumber(pItem->ulEmployeeNo), aszJobCode);
    } else {
        PmgPrintf(PrtChkPort(), aszPrtRJEtkCode2, pItem->aszMnemonic, RflTruncateEmployeeNumber(pItem->ulEmployeeNo));
    }
}

/*
*===========================================================================
** Format  : VOID  PrtRJEtkJobTimeIn(TRANINFORMATION  *pTran, ITEMMISC  *pItem, SHORT sType);
*
*   Input  : TRANINFORMATION    *pTran  -Transaction Information Address
*            ITEMMISC   *pItem          -Item Data Address
*            SHORT      sType           -print type
*                                           PRT_WTIN: with time-in
*                                           PRT_WOIN: without time-in
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints ETK time-in time.
*===========================================================================
*/
VOID  PrtRJJobTimeIn(TRANINFORMATION  *pTran, ITEMMISC  *pItem, SHORT sType)
{
	TCHAR   aszDate[PRT_DATETIME_LEN + 1] = { 0 };

    if ( sType == PRT_WTIN ) {
        /* -- get job time-in code -- */
        PrtGetJobInDate(aszDate, TCHARSIZEOF(aszDate), pTran, pItem);   /* get date time */
    } else {
        tcharnset(aszDate, _T('*'), PRT_DATETIME_LEN);
    }

    PmgPrint(PrtChkPort(), aszDate, (USHORT)_tcslen(aszDate)); /* ### Mod (2171 for Win32) V1.0 */
}

/*
*===========================================================================
** Format  : VOID  PrtRJEtkJobTimeOut(TRANINFORMATION  *pTran, ITEMMISC  *pItem)
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
VOID  PrtRJJobTimeOut(TRANINFORMATION  *pTran, ITEMMISC  *pItem)
{
	TCHAR   aszDate[PRT_DATETIME_LEN + 1] = { 0 };

    /* -- get job time-out time -- */
    PrtGetJobOutDate(aszDate, TCHARSIZEOF(aszDate), pTran, pItem);  /* get date time */
  
    PmgPrint(PrtChkPort(), aszDate, (USHORT)_tcslen(aszDate)); /* ### Mod (2171 for Win32) V1.0 */
}


/*
*===========================================================================
** Format  : VOID  PrtRJCPRoomCharge(UCHAR *pRoomNo, UCHAR *pGuestID);
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
VOID  PrtRJCPRoomCharge(TCHAR *pRoomNo, TCHAR *pGuestID)
{
    if ((*pRoomNo != '\0') && (*pGuestID != '\0')) {
		TCHAR  aszSpecMnem1[PARA_SPEMNEMO_LEN + 1] = { 0 };
		TCHAR  aszSpecMnem2[PARA_SPEMNEMO_LEN + 1] = { 0 };

		RflGetSpecMnem(aszSpecMnem1, SPC_ROOMNO_ADR);   /* get room no. mnemo */
		RflGetSpecMnem(aszSpecMnem2, SPC_GUESTID_ADR);  /* get guest id */

        PmgPrintf(PrtChkPort(), aszPrtRJCPRoomCharge, aszSpecMnem1, pRoomNo, aszSpecMnem2, pGuestID);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtJFolioPost(UCHAR *pszFolioNo, UCHAR *pszPostTranNo)
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
VOID  PrtJFolioPost(TCHAR *pszFolioNo, TCHAR *pszPostTranNo)
{
    if ((*pszFolioNo != '\0') && (*pszPostTranNo != '\0')) {
		TCHAR  aszSpecMnem1[PARA_SPEMNEMO_LEN + 1] = { 0 };
		TCHAR  aszSpecMnem2[PARA_SPEMNEMO_LEN + 1] = { 0 };

		RflGetSpecMnem(aszSpecMnem1, SPC_FOLINO_ADR);   /* get folio no. */
		RflGetSpecMnem(aszSpecMnem2, SPC_POSTTRAN_ADR); /* get posted transaction no. */

        if ( fsPrtPrintPort & PRT_JOURNAL ) {     /* journal */
            PmgPrintf(PMG_PRT_JOURNAL, aszPrtJFolioPost, aszSpecMnem1, pszFolioNo, aszSpecMnem2, pszPostTranNo);
        }
    }
}

/*
*===========================================================================
** Format  : VOID  PrtRJCPRspMsgText(UCHAR *pRspMsgText);
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
USHORT  PrtRJCPRspMsgText(TCHAR *pRspMsgText)
{
	USHORT usRet = 0;

    if (*pRspMsgText != '\0') {
        PmgPrintf(PrtChkPort(), aszPrtRJCPRspMsgText, pRspMsgText);
		usRet = 1;
    }
	return usRet;
}

/*
*===========================================================================
** Format  : VOID  PrtRJOffline(SHORT fsMod, USHORT usDate, UCHAR *aszApproval);
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
*      21-char printer
*
*       0        1         2      
*       123456789012345678901  
*      |^^^^^^^^^^^^^^^^^^^^^| 
*      |MMMM   MM/YY  123456 |
*      |                     | 
*       ^^^^^^^^^^^^^^^^^^^^^  
*===========================================================================
*/
VOID  PrtRJOffline(USHORT fbMod, TCHAR *auchDate, TCHAR *auchApproval)
{
	TCHAR  aszApproval[NUM_APPROVAL + 1] = { 0 };
	TCHAR  aszDate[NUM_EXPIRA + 1] = { 0 };
	TCHAR  aszOffMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };
	TCHAR  aszOffDate[5 + 1] = { 0 };
    USHORT  usOffDate;

    _tcsncpy(aszApproval, auchApproval, NUM_APPROVAL);
    _tcsncpy(aszDate, auchDate, NUM_EXPIRA);

    if ( fbMod & OFFEPTTEND_MODIF) {                /* offline modifier */
        /* -- get special mnemonics -- */
		RflGetSpecMnem( aszOffMnem, SPC_OFFTEND_ADR);
    }           

    if ( *aszDate != '\0' ){                        /* expiration date  */
        usOffDate = _ttoi(aszDate);
        if (usOffDate != 0) {                       /* E-028 corr. 4/20     */
            RflSPrintf( aszOffDate, TCHARSIZEOF(aszOffDate), aszPrtRJOffDate, usOffDate / 100, usOffDate % 100 );
        }
    }

    /* -- send mnemoncis and exp.date -- */
    if (*aszDate != '\0' || *aszApproval != '\0' || *aszOffMnem != '\0') {
        PmgPrintf( PrtChkPort(), aszPrtRJOffline, aszOffMnem, aszOffDate, aszApproval );
    }
}
/*
*===========================================================================
** Format  : VOID  PrtRJEPTError(ITEMPRINT  *pItem);
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
*      21-char printer
*
*       0        1         2      
*       123456789012345678901  
*      |^^^^^^^^^^^^^^^^^^^^^| 
*      |M 9999               |
*      |                     | 
*       ^^^^^^^^^^^^^^^^^^^^^  
*===========================================================================
*/
VOID  PrtRJEPTError(ITEMPRINT  *pItem)
{
    PmgPrintf(PMG_PRT_JOURNAL, aszPrtRJEPTError, pItem->uchStatus, (USHORT)pItem->lAmount );
}

/*
*===========================================================================
** Format  : VOID  PrtRJResetLog(ITEMOTHER *pItem)
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
VOID  PrtRJResetLog(ITEMOTHER *pItem)
{
	TCHAR aszBuff1[10] = { 0 };
    TCHAR aszBuff2[10] = { 0 };
    TCHAR aszBuff3[10] = { 0 };
    TCHAR aszBuff4[10] = { 0 };
    TCHAR aszPrtNull[] = _T("");                /* null */

    /* --- Print Mnemonic from PCIF --- */
    if (pItem->uchAction == UIF_UCHRESETLOG_NOACTION) {
        PmgPrint(PMG_PRT_JOURNAL, pItem->aszMnemonic, usPrtRJColumn);

    /* --- Print Reset Log by PCIF --- */
    } else {
        /* reset log first line, Action Code */
        PmgPrintf(PMG_PRT_JOURNAL, aszPrtRJResetLog, pItem->uchAction);

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
            PmgPrintf(PMG_PRT_JOURNAL, aszPrtRJResetLog2, aszBuff1, aszBuff2, aszBuff3, aszBuff4);
        }
    }
}

/*
*===========================================================================
** Format  : VOID  PrtRJResetLog3(ITEMOTHER *pItem)
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
VOID  PrtRJResetLog3(ITEMOTHER *pItem)
{
    PmgPrintf(PMG_PRT_JOURNAL, aszPrtRJResetLog4, pItem->aszMnemonic);
}

/*
*===========================================================================
** Format  : VOID  PrtRJResetLog4(ITEMOTHER *pItem)
*               
*    Input : USHORT usNo
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints PC IF log (Trailer) on journal    V3.3 (FVT#5)
*===========================================================================
*/
VOID  PrtRJResetLog4(ITEMOTHER *pItem)
{
    TCHAR           aszPrtNull[] = _T("");
    TCHAR           aszBuff2[10] = { 0 }, aszBuff3[10] = { 0 };
    PARASTOREGNO    StRg = { 0 };

    /* --- SPV# & Store/Reg# --- */
    StRg.uchMajorClass = CLASS_PARASTOREGNO;
    StRg.uchAddress = SREG_NO_ADR;
    CliParaRead(&StRg);
    if (pItem->lAmount) {
        PmgPrintf(PMG_PRT_JOURNAL, aszPrtRJResetLog5, (USHORT)pItem->lAmount, StRg.usStoreNo, StRg.usRegisterNo);
    } else {
        PmgPrintf(PMG_PRT_JOURNAL, aszPrtRJResetLog6, StRg.usStoreNo, StRg.usRegisterNo);
    }
    /* --- Consecutive, Time, Date --- */
    aszBuff2[0] = '\0';
    aszBuff3[0] = '\0';
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
		PmgPrintf(PMG_PRT_JOURNAL, aszPrtRJResetLog3, usConsecNumber, aszBuff2, aszBuff3);
	}
}

/*
*===========================================================================
** Format  : VOID  PrtRJpowerDownLog(ITEMOTHER *pItem)
*               
*    Input : ITEMOTHER pItem       -print data
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints power down log on journal
*
*  Print format :
*
*      24-char printer
*
*       0        1         2      
*       123456789012345678901234  
*      |^^^^^^^^^^^^^^^^^^^^^^^^| 
*      | DataLost               |
*      |                        | 
*       ^^^^^^^^^^^^^^^^^^^^^^^^                            Saratoga
*===========================================================================
*/
VOID  PrtRJPowerDownLog(ITEMOTHER *pItem)
{
    TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };

    switch (pItem->uchAction) {
    /* case FSC_MINOR_SUSPEND: */
    case FSC_MINOR_POWER_DOWN:
        /* power down message does not printed on journal at power down */
        /* because 7140 printer may be off already */
        PrtPmgWait();
		RflGetTranMnem(aszTranMnem, TRN_POWER_DOWN_ADR);
        PmgPrintf(PMG_PRT_JOURNAL, aszPrtRJPowerDownLog, aszTranMnem);
        /* PrtRJResetLog4(pItem); */
        PrtPmgWait();
        break;
    }
}

/*
*===========================================================================
** Format  : VOID  PrtRPLUNo(UCHAR *puchPLUNo, SHORT sPortion);
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
VOID  PrtRPLUNo(TCHAR *puchPLUCpn)
{
    if (*puchPLUCpn) {
        PmgPrint((USHORT)(PrtChkPort() & PMG_PRT_RECEIPT), puchPLUCpn, (USHORT)_tcslen(puchPLUCpn));
    }
}

/*
*===========================================================================
** Format  : VOID  PrtJPLUNo(UCHAR *puchPLUNo, SHORT sPortion);
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
VOID  PrtJPLUNo(TCHAR *puchPLUCpn)
{
    if (*puchPLUCpn) {
        PmgPrint((USHORT)(PrtChkPort() & PMG_PRT_JOURNAL), puchPLUCpn, (USHORT)_tcslen(puchPLUCpn));
    }
}

/*
*===========================================================================
** Format  : VOID  PrtJPLUBuild (UCHAR *puchPLUNo, UCHAR uchDeptNo);
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
VOID    PrtJPLUBuild (USHORT usAdr, TCHAR *puchPLUNo, USHORT usDeptNo)
{
    TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 }; /* PARA_... defined in "paraequ.h" */

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);
    
    PmgPrintf((USHORT)(PrtChkPort() & PMG_PRT_JOURNAL), aszPrtJPluBuild, aszTranMnem, puchPLUNo, usDeptNo);
}

/*
*===========================================================================
** Format  : VOID  PrtRJForeignQtyFor(LONG lQTY, USHORT usFor, LONG lUnitPrice);
*
*   Input  : LONG   lQTY
*          : USHORT usFor
*          : LONG   lUnitPrice,
*          :
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints Qty, For and unit price line.
*
*  Print format :
*
*      24-char printer
*
*       0        1         2              0        1         2
*       123456789012345678901234          123456789012345678901234
*      |^^^^^^^^^^^^^^^^^^^^^^^^|        |^^^^^^^^^^^^^^^^^^^^^^^^|
*      |     -9999 X $$$9999.99 |  OR    |  -9999 X $$$9999.99/99 |
*      |                        |        |                        |
*       ^^^^^^^^^^^^^^^^^^^^^^^^          ^^^^^^^^^^^^^^^^^^^^^^^^
*                OR                                OR
*       0        1         2              0        1         2
*       123456789012345678901234          123456789012345678901234
*      |^^^^^^^^^^^^^^^^^^^^^^^^|        |^^^^^^^^^^^^^^^^^^^^^^^^|
*      |-99.999 X $$$9999.99/99 |   OR   |-999.999 X $$$9999.99/99|
*      |                        |        |                        |
*       ^^^^^^^^^^^^^^^^^^^^^^^^          ^^^^^^^^^^^^^^^^^^^^^^^^
*===========================================================================
*/
VOID  PrtRJForeignQtyFor(LONG lQTY, DCURRENCY lAmount, UCHAR uchAdr, UCHAR   fbStatus )
{
    TCHAR  aszSpecAmt[PARA_SPEMNEMO_LEN + PRT_AMOUNT_LEN + 1] = { 0 };
    SHORT  sDecPoint;

    if (labs(lQTY) == PLU_BASE_UNIT){
        return ;
    }

    PrtSetQtyDecimal( &sDecPoint, &lQTY);

    /* -- adjust foreign mnemonic and amount sign(+.-) -- */
    PrtAdjustForeign(aszSpecAmt, lAmount, uchAdr, fbStatus);

    PmgPrintf(PrtChkPort(), aszPrtRJQty, lQTY, aszSpecAmt);

    PrtResetQtyDecimal(&sDecPoint, &lQTY);
}

/*
*===========================================================================
** Format  : VOID  PrtRJMoneyForeign(LONG lForeign, UCHAR uchAdr1, UCHAR   uchAdr2,
*                                                    UCHAR fbStatus);
*   Input  : LONG  lForeign,      -Foreign tender amount
*            UCHAR uchAdr         -Foreign symbol address
*            UCHAR fbStatus       -Foreign currency decimal point status
*
*   Output : none
*   InOut  : none
*
** Return  : none
*
** Synopsis: This function prints foeign tender line.
*
*  Print format :
*
*      24-char printer
*
*       0        1         2              0        1         2
*       123456789012345678901234          123456789012345678901234
*      |^^^^^^^^^^^^^^^^^^^^^^^^|        |^^^^^^^^^^^^^^^^^^^^^^^^|
*      | MMMMMMMM SMMM9999999.99|   OR   | MMMMMMMM SMMM999999.999|
*      |                        |        |                        |
*       ^^^^^^^^^^^^^^^^^^^^^^^^          ^^^^^^^^^^^^^^^^^^^^^^^^
*                 OR
*       0        1         2
*       123456789012345678901234
*      |^^^^^^^^^^^^^^^^^^^^^^^^|
*      | MMMMMMMM SMMM9999999999|
*      |                        |
*       ^^^^^^^^^^^^^^^^^^^^^^^^
*
*===========================================================================
*/
VOID    PrtRJMoneyForeign(DCURRENCY lForeign, USHORT usTranAdr1, UCHAR uchAdr2, UCHAR fbStatus)
{
	TCHAR   aszFMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };        /* PARA_... defined in "paraequ.h" */
	TCHAR   aszFAmt[PRT_RJCOLUMN + 1] = { 0 };
	TCHAR   aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };   /* PARA_... defined in "paraequ.h" */
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
    usStrLen = _tcslen( aszFMnem );

    /* -- format foreign amount -- */
    RflSPrintf( &aszFAmt[usStrLen], TCHARSIZEOF(aszFAmt), aszPrtRJForeign1, sDecPoint, lForeign );

    /* -- adjust sign ( + , - ) -- */
    if ( lForeign < 0 ) {
        aszFAmt[0] = _T('-');
        _tcsncpy( &aszFAmt[1], aszFMnem, usStrLen );
    } else {
        _tcsncpy( aszFAmt, aszFMnem, usStrLen );
    }

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem( aszTranMnem, usTranAdr1 );

    PmgPrintf( PrtChkPort(), aszPrtRJMnemMnem, aszTranMnem, aszFAmt );
}

/*
*===========================================================================
** Format  : VOID  PrtRJQtyFor(LONG lQTY, USHORT usFor, LONG lUnitPrice);
*
*   Input  : LONG   lQTY
*          : USHORT usFor
*          : LONG   lUnitPrice,
*          :
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints Qty, For and unit price line.
*
*  Print format :
*
*      24-char printer
*
*       0        1         2              0        1         2
*       123456789012345678901234          123456789012345678901234
*      |^^^^^^^^^^^^^^^^^^^^^^^^|        |^^^^^^^^^^^^^^^^^^^^^^^^|
*      |     -9999 X $$$9999.99 |  OR    |  -9999 X $$$9999.99/99 |
*      |                        |        |                        |
*       ^^^^^^^^^^^^^^^^^^^^^^^^          ^^^^^^^^^^^^^^^^^^^^^^^^
*                OR                                OR
*       0        1         2              0        1         2
*       123456789012345678901234          123456789012345678901234
*      |^^^^^^^^^^^^^^^^^^^^^^^^|        |^^^^^^^^^^^^^^^^^^^^^^^^|
*      |-99.999 X $$$9999.99/99 |   OR   |-999.999 X $$$9999.99/99|
*      |                        |        |                        |
*       ^^^^^^^^^^^^^^^^^^^^^^^^          ^^^^^^^^^^^^^^^^^^^^^^^^
*===========================================================================
*/
VOID  PrtRJQtyFor(LONG lQTY, USHORT usFor, DCURRENCY lUnitPrice)
{
    TCHAR  aszSpecAmt[PARA_SPEMNEMO_LEN +PRT_AMOUNT_LEN + 1] = { 0 }, aszDest1[7] = { 0 }, aszDest2[3] = { 0 };
    SHORT  sDecPoint;
    USHORT usLength;

    if ( labs( lQTY ) == PLU_BASE_UNIT && (( usFor == 0 ) || ( usFor == 1)) ) {
        return ;
    }

    PrtSetQtyDecimal( &sDecPoint, &lQTY);

    /* -- adjust native mnemonic and amount sign(+.-) -- */
    PrtAdjustNative(aszSpecAmt, lUnitPrice);

    /* -- convert long to ASCII -- */
    RflSPrintf(aszDest1, PRT_RJCOLUMN + 1, aszPrtQty, lQTY);

    /* -- convert long to ASCII -- */
    RflSPrintf(aszDest2, PRT_RJCOLUMN + 1, aszPrtFor, usFor);

    if (lQTY < 0L ) {
        usLength = _tcslen(aszDest1) + _tcslen(aszSpecAmt) + _tcslen(aszDest2) + 7;
    } else {
        usLength = _tcslen(aszDest1) + _tcslen(aszSpecAmt) + _tcslen(aszDest2) + 6;
    }

    if ( ( lQTY < 0L ) && ( usLength >= PRT_24CHAR ) ) {
        PmgPrintf( PrtChkPort(), aszPrtRJSpecialQty, sDecPoint, lQTY, aszSpecAmt, usFor);
    } else {
        if (sDecPoint == PRT_NODECIMAL) {
            if (( usFor == 0 ) || ( usFor == 1 )) {
                PmgPrintf( PrtChkPort(), aszPrtRJQty, lQTY, aszSpecAmt);
            } else {
                PmgPrintf( PrtChkPort(), aszPrtRJQtyFor, lQTY, aszSpecAmt, usFor );
            }
        } else {
            if (( usFor == 0 ) || ( usFor == 1 )) {
                PmgPrintf( PrtChkPort(), aszPrtRJDecQty, sDecPoint, lQTY, aszSpecAmt);
            } else {
                PmgPrintf( PrtChkPort(), aszPrtRJDecQtyFor, sDecPoint, lQTY, aszSpecAmt, usFor );
            }
        }
    }
    PrtResetQtyDecimal( &sDecPoint, &lQTY);
}

/*
*===========================================================================
** Format  : VOID  PrtSupRJVoid(CHAR fbMod)
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
VOID    PrtSupRJVoid(USHORT fbMod)
{
    if (fbMod & (VOID_MODIFIER | RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) {
		TCHAR   aszWork[PRT_RJCOLUMN + 1] = { 0 };
		USHORT usReasonCode= 0;

		if (0 > PrtGetReturns(fbMod, usReasonCode, aszWork, TCHARSIZEOF(aszWork))) {
			/* -- get void mnemonics instead of returns mnemonic -- */
			PrtGetVoid(aszWork, TCHARSIZEOF(aszWork));
		}

        /* -- print Receipt & Journal --*/
        PmgPrint(PrtChkPort(), aszWork, usPrtRJColumn);
    }
}

/****** End of Source ******/