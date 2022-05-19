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
* Title       : Print common routine
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRComn_.C
*               | ||   |
*               | ||   +- S:21Char L:24Char _:not influcenced by print column
*               | |+----- comon routine
*               | +------ R:reg mode S:super mode
*               +-------- Pr:print module
* --------------------------------------------------------------------------
* Abstract:
*    PrtSlipInitial()
*    PrtSlpCompul()
*    PrtRectCompul()
*    PrtDouble()
*    PrtPortion()
*    PrtGetVoid()
*    PrtGetOffTend()
*    PrtFeed()
*    PrtCut()
*    PrtElec()
*    PrtChkPort()
*    PrtGetScale()
*    PrtGetDate()
*    PrtGetJobInDate()
*    PrtGetJobOutDate()
*    PrtGetTaxMod()
*    PrtGetTranMnem()
*    PrtGet24Mnem()
*    PrtGetLead()
*    PrtGetSpecMnem()
*    PrtGetAdj()
*    PrtSetNumber()
*    PrtSetItem()
*    PrtSetChildNotOrder();
*    PrtSetChildPriority();
*    PrtGet1Line()
*    PrtChkCasWai()
*    PrtChkTtlAdr()         replaced by RflChkTtlAdr() by Richard Chambers Nov-16-2020
*    PrtChkTtlStubAdr()     replaced by RflChkTtlStubAdr() by Richard Chambers Nov-16-2020
*    PrtChkTendAdr()
*    PrtGetAllProduct()
*    PrtAdjustNative()
*    PrtFillSpace()
*    PrtPrint()
*    PrtEndValidation()
*    PrtSetRJColumn()
*    PrtPmgWait()
*    PrtSlipPmgWait()
*    PrtChkFullSP()
*    PrtSPVLInit()
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
* Jun-01-92 : 00.00.01 : K.Nakajima : initial
* Jun-30-93 : 01.00.12 : K.You      : mod. PrtSetItem
*           :          :            : PrtSlipInitial for US enhance.
* Jul-06-93 : 01.00.12 : K.You      : Mod. PrtChkCasWai for US enhance.
* Jul-13-93 : 01.00.12 : K.Nakajima : add PrtPmgWait(), PrtSlipPmgWait()
* Jul-13-93 : 01.00.13 : R.Itoh     : del PrtGet44Mnem(), add PrtChkFullSP()
* Oct-13-93 : 02.00.02 : K.You      : bug fixed E-105 (mod. PrtGetJobOutDate())
* Oct-19-93 : 02.00.02 : K.You      : del. canada tax feature.
* Apr-08-94 : 00.00.04 : K.You      : add validation slip print feature.(add. PrtSPVLInit)
*           :          :            : (mod. PrtSlipInitial())
* Apr-24-94 : 00.00.05 : K.You      : bug fixed E-32 (mod. PrtGetOffTend)
* Mar-17-94 : 02.05.00 : K.You      : bug fixed S-16 (mod. PrtGet24Mnem)
* Apr-27-95 : 03.00.00 : T.Nakahata : Add Total to Kitchen (mod. PrtChkTtlAdr)
* Jun-06-95 : 03.00.00 : T.Nakahata : Print Condiment of Promotional PLU
* Jul-11-95 : 03.00.04 : T.Nakahata : fix PrtSetItem (add PRT_SEPARATOR)
* Jul-19-95 : 03.00.04 : T.Nakahata : unique tran# mnemonic (PrtGetLead)
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
* Nov-29-99 : 01.00.00 : hrkato     : Saratoga
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
#include <tchar.h>
#include <stdlib.h>
#include <string.h>
/**------- 2170 local------**/
#include <ecr.h>
#include <pif.h>
#include <regstrct.h>
#include <transact.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <uie.h>
#include <fsc.h>
#include <pmg.h>
#include <rfl.h>
#include <csop.h>
#include "prtcom.h"
#include "prtrin.h"
#include "prrcolm_.h"
#include "item.h"

/**
;========================================================================
;+                    S T A T I C    R A M s
;========================================================================
**/
CONST TCHAR aszPrtNull[] = _T("");                         /* null */
CONST TCHAR FARCONST aszPrtTime[] = _T("%2d:%02d%s ");      /* time */
CONST TCHAR FARCONST aszPrtTimeWithZone[] = _T("%2d:%02d%s %-3.3s ");      /* time with a time zone */
CONST TCHAR FARCONST aszPrtDate[] = _T("%02d/%02d/%02d");    /* date */
CONST TCHAR FARCONST aszPrtSPNext[] = _T("\t%s");          /* continue to next line */
CONST TCHAR FARCONST aszPrtSPHead1[] = _T("%-4s%03d    %-4s%03d    %8s %d%d");     /* Slip Header */
CONST TCHAR FARCONST aszPrtSPHead2[] = _T("%-8s   %s%d\t%s ");     /* Slip Header */

extern USHORT usShrSemDrawer;

/**
;========================================================================
;+             P R O G R A M    D E C L A R A T I O N s
;========================================================================
**/
/*
*===========================================================================
** Format  : VOID  PrtSlipInitial(VOID)
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function initialize current slip's pages and lines,
*            checks slip form in or out, decides print portion.
*===========================================================================
*/
VOID PrtSlipInitial(VOID)
{
//    USHORT usRet;                      /* return value */
    USHORT  fbPrtStatus;                /* printer status */
    USHORT usPrevious;
    USHORT  usColumn = 0;
    USHORT  fbStatus;

    if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) { /* validation slip print */
        PrtRectCompul();                            /* set receipt compulsory */
        return;
    }

	/* if slip printer is not prepaired, not call slip function, 09/11/01 */
    if (PmgPrtConfig(PMG_PRT_SLIP, &usColumn, &fbStatus) != PMG_SUCCESS) {

        usColumn = 0;   /* assume as thermal printer by default */
    }
    if (usColumn == 0) {
        PrtRectCompul();                            /* set receipt compulsory */
        return;
    }

/*    usPrtSlipPageLine = 0;             / clear current slip's pages and lines */

    PrtSlipPmgWait();                  /* waits until spool buffer is empty */

    /* -- slip compulsory? -- */
    if ( fsPrtNoPrtMask & (fsPrtCompul & PRT_SLIP) ) {

        if ((fsPrtStatus & PRT_SPLIT_PRINT) &&
            (!auchPrtSeatCtl[0]) &&             /* at individual seat# print */
            (auchPrtSeatCtl[1])) {              /* after secondary print */

            /* -- require paper change -- */
            UieErrorMsg(LDT_PAPERCHG_ADR, UIE_WITHOUT);
        }

        /* allow power switch at error display */
        usPrevious = PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */

        for (;;) {

            /* -- get printer status -- */
//            usRet = PmgGetStatus(PMG_PRT_SLIP, &fbPrtStatus);

        //    if (usRet == PMG_SUCCESS ) {

                /* -- form in? -- */
             //   if ((fbPrtStatus & PRT_STAT_FORM_IN) && (fbPrtStatus & PRT_STAT_ERROR)) {

                    PrtSlpCompul();             /* set slip compulsory */
                    PifControlPowerSwitch(usPrevious);  /* V1.0.04 */
                    return;

              //  } else {

                    /* allow power switch at error display */
                  //  PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */
                    /* -- require form in -- */
                  //  UieErrorMsg(LDT_SLPINS_ADR, UIE_WITHOUT);
               // }


           // } else {

                /* allow power switch at error display */
               // PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */
                /* display "printer trouble"  and wait clear */
               // usRet = UieErrorMsg(LDT_PRTTBL_ADR, UIE_ABORT);

                /* if abort key entered, set receipt compul and exit */
               // if (usRet == FSC_CANCEL || usRet == FSC_P5) {

                 //   PrtRectCompul();             /* set receipt compulsory */
                 ///   PifControlPowerSwitch(usPrevious);  /* V1.0.04 */
                 //   return;
            //    }
//
          //  }

        }
        PifControlPowerSwitch(usPrevious);  /* V1.0.04 */
    }

    /* -- case of "receipt or slip" -- */
    if (fsPrtNoPrtMask & PRT_SLIP) {

        if (PMG_SUCCESS == PmgGetStatus(PMG_PRT_SLIP, &fbPrtStatus) ) {

            /* -- form in? -- */
            if ((fbPrtStatus & PRT_STAT_FORM_IN) && (fbPrtStatus & PRT_STAT_ERROR)) {

                PrtSlpCompul();                   /* set slip compulsory */
                return ;

            }
        }
    }
    PrtRectCompul();                              /* set receipt compulsory */
}

/*
*===========================================================================
** Format  : VOID  PrtSlpCompul(VOID);
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function sets slip compulsory and reset receipt compulsory.
*===========================================================================
*/
VOID   PrtSlpCompul(VOID)
{
    fsPrtCompul |= PRT_SLIP;            /* set slip */
    fsPrtCompul &= ~(PRT_RECEIPT);      /* reset receipt */
    fsPrtNoPrtMask &= ~(PRT_RECEIPT);   /* set receipt */
}
/*
*===========================================================================
** Format  : VOID  PrtRectCompul(VOID);
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function sets receipt compulsory and reset slip compusory.
*===========================================================================
*/
VOID   PrtRectCompul(VOID)
{
    fsPrtCompul |= PRT_RECEIPT;         /* set receipt */
    fsPrtCompul &= ~(PRT_SLIP);         /* reset slip */
    fsPrtNoPrtMask &= ~(PRT_SLIP);      /* set slip */
}

/*
*===========================================================================
** Format  : VOID  PrtDouble(UCHAR *pszDest, USHORT  usDestLen,
*                                            UCHAR *pszSource)
*
*   Input  : USHORT       usDestLen      -destination buffer length
*            UCHAR        *pszSource     -source buffer address
*   Output : UCHAR        *pszDest       -destination buffer address
*   InOut  : none
** Return  : none
*
** Synopsis: This function converts ASCII strings to double wide.
*            (insert "0x12"  behind character.)
*===========================================================================
*/
VOID  PrtDouble(TCHAR *pszDest, USHORT usDestLen, TCHAR *pszSource)
{
    TCHAR  *pszDst;
    TCHAR  *pszSorc;


    pszDst  = pszDest;                               /* initialize */
    pszSorc = pszSource;                             /* initialize */

    if (usDestLen == 0) {
        return ;
    }

    for ( ;
         (*pszSorc != '\0') && (usDestLen > 2);
         pszSorc++ , usDestLen -= 2) {

        *pszDst++ = PRT_DOUBLE;

        if ( *pszSorc == PRT_DOUBLE ) {
            *pszDst++ = PRT_SPACE;
        } else {
            *pszDst++ = *pszSorc;
        }

    }

    *pszDst = '\0';
}

/*
*===========================================================================
** Format  : VOID  PrtSDouble(UCHAR *pszDest, USHORT  usDestLen,
*                                            UCHAR *pszSource)
*
*   Input  : USHORT       usDestLen      -destination buffer length
*            UCHAR        *pszSource     -source buffer address
*   Output : UCHAR        *pszDest       -destination buffer address
*   InOut  : none
** Return  : none
*
** Synopsis: This function converts ASCII strings to 'square double'
*            (vertical double, and horizontal double) wide.
*            (insert "0x13"  behind character.)
*===========================================================================
*/
VOID  PrtSDouble(TCHAR *pszDest, USHORT usDestLen, TCHAR *pszSource)
{
    TCHAR  *pszDst;
    TCHAR  *pszSorc;


    pszDst  = pszDest;                               /* initialize */
    pszSorc = pszSource;                             /* initialize */

    if (usDestLen == 0) {
        return ;
    }

    for ( ;
         (*pszSorc != '\0') && (usDestLen > 2);
         pszSorc++ , usDestLen -= 2) {

        *pszDst++ = PRT_SDOUBLE;

        *pszDst++ = *pszSorc;

    }

    *pszDst = '\0';
}

/*
*===========================================================================
** Format  : VOID  PrtPortion(SHORT fsPrtState);
*
*   Input  : SHORT  fsPrtState        -Item's Print Portion
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function sets print portion to static area "fsPrtPrintPort"
*===========================================================================
*/
VOID  PrtPortion(SHORT fsPrtState)
{
    /* -- if ticket, portion is "receipt" -- */
    if ((fsPrtState & PRT_SINGLE_RECPT) || (fsPrtState & PRT_DOUBLE_RECPT)) {

        fsPrtPrintPort = PRT_RECEIPT;
        return ;
    }

    fsPrtPrintPort = fsPrtState;
    fsPrtPrintPort |= fsPrtCompul;
    fsPrtPrintPort &= fsPrtNoPrtMask;

    if (fsPrtPrintPort & (PRT_RECEIPT|PRT_SLIP)) {

        fsPrtStatus |= PRT_REC_SLIP;    /* set receipt/slip print status for total print R3.1 */
    }
}

/*
*===========================================================================
** Format  : VOID  PrtPortion2(ITEMSALES);
*
*   Input  : ITEMSALES        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function sets print portion to static area "fsPrtPrintPort"
*            (for void consolidation)
*===========================================================================
*/
VOID  PrtPortion2(ITEMSALES *pItem)
{
    if ( fsPrtStatus & PRT_TAKETOKIT ) {     /* kitchen print */
        if ((CliParaMDCCheck(MDC_DEPT2_ADR, EVEN_MDC_BIT3) == 0) ||
            (pItem->uchPM) ||   /* saratoga */
            (pItem->uchPPICode)) {

            PrtPortion(pItem->fsPrintStatus);   /* print out void item */
            return;
        }

    } else if ((CliParaMDCCheck(MDC_DEPT3_ADR, ODD_MDC_BIT2) == 0) || (pItem->uchPM) || (pItem->uchPPICode) ||
		(pItem->fbModifier & (RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)))
	{
        PrtPortion(pItem->fsPrintStatus);   /* print out void item */
        return;
    }

    /* -- if ticket, portion is "receipt" -- */
    if (pItem->fsPrintStatus & (PRT_SINGLE_RECPT|PRT_DOUBLE_RECPT)) {

        fsPrtPrintPort = PRT_RECEIPT;
        return ;
    }

    fsPrtPrintPort = pItem->fsPrintStatus;
    fsPrtPrintPort |= fsPrtCompul;
    fsPrtPrintPort &= fsPrtNoPrtMask;

    /* -- not print voided item, except jounal -- */
    if (pItem->sCouponQTY == 0) {

        fsPrtPrintPort &= ~(PRT_RECEIPT|PRT_SLIP);
    }

    if (fsPrtPrintPort & (PRT_RECEIPT|PRT_SLIP)) {

        fsPrtStatus |= PRT_REC_SLIP;    /* set receipt/slip print status for total print R3.1 */
    }
}

/*
*===========================================================================
** Format  : VOID  PrtPortion3(SHORT fsPrtState);
*
*   Input  : SHORT  fsPrtState        -Item's Print Portion
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function sets print portion to static area "fsPrtPrintPort"
*            (for total line)
*===========================================================================
*/
VOID  PrtPortion3(SHORT fsPrtState)
{
    fsPrtPrintPort = fsPrtState;
    fsPrtPrintPort |= fsPrtCompul;
    fsPrtPrintPort &= fsPrtNoPrtMask;

    /* -- if ticket, portion is "receipt" -- */
    if ((fsPrtState & PRT_SINGLE_RECPT) || (fsPrtState & PRT_DOUBLE_RECPT)) {

        fsPrtPrintPort = PRT_RECEIPT;
    }

    /* -- validation  print -- */
    if (fsPrtState & PRT_VALIDATION) {

        fsPrtPrintPort = PRT_VALIDATION;
    }

    if (!(fsPrtStatus & PRT_REC_SLIP)) {

        fsPrtPrintPort &= ~(PRT_RECEIPT|PRT_SLIP);
    }

}

/*
*===========================================================================
** Format  : VOID  PrtGetVoid(UCHAR *aszVoidMnem, USHORT usStrLen);
*
*   Input  : USHORT usStrLen          -destination buffer length
*   Output : UCHAR  *aszVoidMnem      -void mnemonic address
*   InOut  : none
** Return  : none
*
** Synopsis: This function gets void mnemonic with double wide char
*===========================================================================
*/
VOID  PrtGetVoid(TCHAR *pszVoidMnem, USHORT usStrLen)
{
    TCHAR  aszSpecMnem[PARA_SPEMNEMO_LEN + 1]; /* PARA_... defined in "paraequ.h" */

	RflGetSpecMnem(aszSpecMnem, SPC_VOID_ADR);
    PrtDouble(pszVoidMnem, usStrLen, aszSpecMnem);     /* convert character to double chara. */
}

SHORT  PrtGetReturns(USHORT fbMod, USHORT usReasonCode, TCHAR *pszVoidMnem, USHORT usStrLen)
{
	USHORT usAddr = 0;
	SHORT  sRetStatus;

	if (fbMod & RETURNS_MODIFIER_1) {
		usAddr = SPC_RETURNS_1;
	} else if (fbMod & RETURNS_MODIFIER_2) {
		usAddr = SPC_RETURNS_2;
	} else if (fbMod & RETURNS_MODIFIER_3) {
		usAddr = SPC_RETURNS_3;
	}

	if (usAddr != 0 && usReasonCode != 0) {
		TCHAR  aszSpecMnem[PARA_SPEMNEMO_LEN + 1] = { 0 }; /* PARA_... defined in "paraequ.h" */
		TCHAR  aszLineBuffer[68] = { 0 };

		pszVoidMnem[0] = 0;
		if (usReasonCode > 0) {
			OPMNEMONICFILE MnemonicFile = { 0 };

			MnemonicFile.usMnemonicFileId = OPMNEMONICFILE_ID_REASON;
			MnemonicFile.usMnemonicAddress = usReasonCode;
			OpMnemonicsFileIndRead(&MnemonicFile, 0);

			RflGetSpecMnem(aszSpecMnem, usAddr);
			_tcscpy (aszLineBuffer, aszSpecMnem);
			_tcscat (aszLineBuffer, _T(": "));
			_tcscat (aszLineBuffer, MnemonicFile.aszMnemonicValue);
			_tcsncpy (pszVoidMnem, aszLineBuffer, usStrLen);
			pszVoidMnem[usStrLen-1] = 0;
		}
//		PrtDouble(pszVoidMnem, usStrLen, aszSpecMnem);     /* convert character to double chara. */
		sRetStatus = 1;
	} else {
		sRetStatus = -1;
	}

	return sRetStatus;
}

/*
*===========================================================================
** Format  : VOID  PrtGetOffTend(CHAR fbMod, UCHAR *aszOffMnem, USHORT usStrLen);
*
*   Input  : CHAR   fbMod           -Off line status
*          : USHORT usStrLen        -destination buffer length
*   Output : UCHAR  *aszOffMnem     -off line tender mnemonic address
*   InOut  : none
** Return  : none
*
** Synopsis: This function gets off line tender mnemonic with double wide char
*===========================================================================
*/
VOID  PrtGetOffTend(USHORT fbMod, TCHAR *pszOffMnem, USHORT usStrLen)
{
	TCHAR  aszTransMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 }; /* PARA_... defined in "paraequ.h" */

    if (fbMod & OFFCPAUTTEND_MODIF) {
		RflGetTranMnem(aszTransMnem, TRN_OFFTNDAUT_ADR);
    } else {
		RflGetTranMnem(aszTransMnem, TRN_OFFTNDCHG_ADR);
    }

    PrtDouble(pszOffMnem, usStrLen, aszTransMnem);      /* convert character to double chara. */

}

/*
*===========================================================================
** Format  : VOID  PrtFeed(USHORT  usPrtType, USHORT  usLineNum);
*
*   Input  : USHORT  usPrtType      -Printer Type(PMG_PRT_RECEIPT
*                                                 PMG_PRT_JOURNAL
*                                                 PMG_PRT_REC_JOU
*                                                 PMG_PRT_VALIDATION
*                                                 PMG_PRT_SLIP)
*            USHORT  usLineNum      -No of feed line
*   Output : UCHAR  *aszVoidMnem      -void mnemonic address
*   InOut  : none
** Return  : none
*
** Synopsis: This function feeds specified printer's paper.
*===========================================================================
*/
USHORT  PrtFeed(USHORT  usPrtType, USHORT  usLineNum)
{
	TCHAR aszWork[12] = { 0 };
	USHORT i = 0;

    if (usLineNum == 0) {
        return PMG_SUCCESS;
    }

    aszWork[i++] = ESC;      /* ESC: printer escape sequence */
    aszWork[i++] = _T('|');
    _ultot(usLineNum, &aszWork[i++], 10);
	if(usLineNum > 9)i++;//usLineNum will take two characters
	if(usLineNum > 99)i++;//usLineNum will take three characters
    aszWork[i++] = _T('l');
	aszWork[i++] = _T('F');
	aszWork[i] = 0x00;

    return PmgPrint( usPrtType, aszWork, i);
}

/*
*===========================================================================
** Format  : VOID  PrtCut(VOID);
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function cuts receipt.
*===========================================================================
*/
USHORT  PrtCut(VOID)
{
	TCHAR aszWork[8] = { 0 };

    aszWork[0] = ESC;      /* ESC: printer escape sequence */
    aszWork[1] = _T('|');
    aszWork[2] = _T('7');
    aszWork[3] = _T('5');
    aszWork[4] = _T('f');
    aszWork[5] = _T('P');
    aszWork[6] = 0x00;

    return PmgPrint( PMG_PRT_RECEIPT, aszWork, 6 );
}

/*
*===========================================================================
** Format  : VOID  PrtElec(VOID);
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints electro.
*===========================================================================
*/
VOID  PrtElec(VOID)
{
	TCHAR aszWork[8] = { 0 };

    aszWork[0] = ESC;      /* ESC: printer escape sequence */
    aszWork[1] = _T('|');
    aszWork[2] = _T('s');
    aszWork[3] = _T('L');
    aszWork[4] = 0x00;

    PmgPrint( PMG_PRT_RECEIPT, aszWork, 3 );
}

/*
*===========================================================================
** Format  : SHORT  PrtChkPort(VOID);
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : SHORT sPrintPort     -print portion
*
** Synopsis: This function checks print portion (Receipt, Journal, Receipt&Journal).
*===========================================================================
*/
SHORT  PrtChkPort(VOID)
{

    if ( (fsPrtPrintPort & PRT_RECEIPT)            /* Receipt and Journal */
        && (fsPrtPrintPort & PRT_JOURNAL) ) {

        return(PMG_PRT_RCT_JNL);

    } else if ( fsPrtPrintPort & PRT_RECEIPT ) {     /* Only Receipt */

        return(PMG_PRT_RECEIPT);

    } else if ( fsPrtPrintPort & PRT_JOURNAL ) {     /* Only Journal */

        return(PMG_PRT_JOURNAL);

    } else {                                       /* not print */

        return(PRT_NOPORTION);

    }
}

/*
*===========================================================================
** Format  : VOID  PrtGetScale(UCHAR *pszMnem, SHORT *psDecPoint,
                               LONG  *plWeight, LONG  lWeight );
*
*   Input  : LONG  lWeight        -Oreiginal Weight Value
*
*   Output : UCHAR *pszMnem       -Weight symbole mnemonic
*            SHORT *psDecPoint,   -decimal point
*            LONG  *plQty,        -adjusted weiqht
*
*   InOut  : none
*
** Return  : none
*
** Synopsis: This function gets Scale Mnemonic and checks Weight.
*===========================================================================
*/
VOID  PrtGetScale(TCHAR *pszMnem, SHORT *psDecPoint, LONG  *plWeight,
                                  TRANINFORMATION  *pTran, ITEMSALES *pItem)
{
    /* -- get weight symbol -- */
	RflGetSpecMnem(pszMnem, SPC_LB_ADR);


    if (pTran->TranCurQual.flPrintStatus & CURQUAL_SCALE3DIGIT) {

        *plWeight = pItem->lQTY;
        *psDecPoint = PRT_3DECIMAL;

    } else {

        *plWeight = pItem->lQTY / 10L;
        *psDecPoint = PRT_2DECIMAL;
    }

}

/**fh
;============================================================================
;
;   function : Get Date and Time Mnemonics
;
;   format   : VOID  PrtGetDate(UCHAR *pszDest, USHORT usDestLen,
;                               TRANINFORMATION *pTran );
;
;   input    : USHORT usDestLen       -destination buffer length
;              TRANINFORMATION *pTran  -transaction information
;
;   output   : UCHAR *pszDest         -destination buffer address
;
;   synopis  : This function get date & time.
;
;============================================================================
fh**/
VOID  PrtGetDate(TCHAR *pszDest, USHORT usDestLen, TRANINFORMATION *pTran)
{
	TCHAR  aszDate[PRT_DATE_LEN + 1] = { 0 };      /* date mnemomics  */
	TCHAR  aszTime[PRT_TIME_LEN + 1 + 1] = { 0 };    /* time mnemomics "1" for space */
    DATE_TIME   *DT = &(pTran->TranGCFQual.DateTimeStamp);                         /* date & time */
    USHORT usWorkHour;

	if (pTran->TranGCFQual.DateTimeStamp.usYear == 0) {
		/* -- get date time if not already available -- */
		// we are using the TRANINFORMATION *pTran here to make it easy to find
		// this location for where the date is set.
		PifGetDateTime(&(pTran->TranGCFQual.DateTimeStamp));
	}

    /* -- Check date status and get -- */
    if (pTran->TranCurQual.flPrintStatus & CURQUAL_DDMMYY) {
        RflSPrintf(aszDate, TCHARSIZEOF(aszDate), aszPrtDate, DT->usMDay, DT->usMonth,  DT->usYear % 100);
    } else {
        RflSPrintf(aszDate, TCHARSIZEOF(aszDate), aszPrtDate, DT->usMonth, DT->usMDay, DT->usYear % 100);
    }

    /* -- Check time status -- */
    if (pTran->TranCurQual.flPrintStatus & CURQUAL_MILITARY) {

        RflSPrintf(aszTime, TCHARSIZEOF(aszTime), aszPrtTime, DT->usHour, DT->usMin, aszPrtNull);
    } else {
        if (DT->usHour == 12 || DT->usHour == 0) {
            usWorkHour = 12;
        } else {
            usWorkHour = (DT->usHour >= 12)?  DT->usHour - 12 : DT->usHour;
        }

        RflSPrintf(aszTime, TCHARSIZEOF(aszTime), aszPrtTime, usWorkHour, DT->usMin, (DT->usHour >= 12)?  aszPrtPM : aszPrtAM );
    }

    memset(pszDest, '\0', usDestLen * sizeof(TCHAR));

    /* -- check write length ("1" for  Null) --*/
    if ( (_tcslen(aszTime) + _tcslen(aszDate) + 1) <= usDestLen) {
        _tcscpy(pszDest, aszTime);     /* write time to destination buffer */
        _tcscat(pszDest, aszDate);     /* write date to destination buffer */
    }
    return ;
}

/**fh
;============================================================================
;
;   function : Get Job In Date and Time Mnemonics
;
;   format   : VOID  PrtGetJobInDate(UCHAR *pszDest, USHORT usDestLen,
;                               TRANINFORMATION *pTran, ITEMMISC *pItem );
;
;   input    : USHORT usDestLen       -destination buffer length
;              TRANINFORMATION *pTran -transaction information
;              ITEMMISC *pItem        -item data address
;
;   output   : UCHAR *pszDest         -destination buffer address
;
;   synopis  : This function get job in date & time.
;
;============================================================================
fh**/
VOID  PrtGetJobInDate(TCHAR *pszDest, USHORT usDestLen, TRANINFORMATION *pTran, ITEMMISC *pItem)
{
	TCHAR  aszDate[PRT_DATE_LEN + 1] = { 0 };      /* date mnemomics  */
	TCHAR  aszTime[PRT_TIME_LEN + 1 + 1] = { 0 };    /* time mnemomics "1" for space */
    USHORT usWorkHour;

    /* -- Check date status and get -- */
    if (pTran->TranCurQual.flPrintStatus & CURQUAL_DDMMYY) {
        RflSPrintf(aszDate, TCHARSIZEOF(aszDate), aszPrtDate, pItem->usDay, pItem->usMonth,  pItem->usYear % 100);
    } else {
        RflSPrintf(aszDate, TCHARSIZEOF(aszDate), aszPrtDate, pItem->usMonth, pItem->usDay, pItem->usYear % 100);
    }

    /* -- Check time status -- */
    if (pTran->TranCurQual.flPrintStatus & CURQUAL_MILITARY) {
        RflSPrintf(aszTime, TCHARSIZEOF(aszTime), aszPrtTime, pItem->usTimeinTime, pItem->usTimeinMinute, aszPrtNull);
    } else {
        if (pItem->usTimeinTime == 12 || pItem->usTimeinTime == 0) {
            usWorkHour = 12;
        } else {
            usWorkHour = (pItem->usTimeinTime >= 12)?  pItem->usTimeinTime - 12 : pItem->usTimeinTime;
        }

        RflSPrintf(aszTime, TCHARSIZEOF(aszTime), aszPrtTime, usWorkHour, pItem->usTimeinMinute, (pItem->usTimeinTime >= 12)?  aszPrtPM : aszPrtAM );
    }

    memset(pszDest, '\0', usDestLen);

    /* -- check write length ("1" for  Null) --*/
    if ( (_tcslen(aszTime) + _tcslen(aszDate) + 1) <= usDestLen) {
        _tcscpy(pszDest, aszTime);     /* write time to destination buffer */
        _tcscat(pszDest, aszDate);     /* write date to destination buffer */
    }
    return ;
}

/**fh
;============================================================================
;
;   function : Get Job Out Date and Time Mnemonics
;
;   format   : VOID  PrtGetJobOutDate(UCHAR *pszDest, USHORT usDestLen,
;                               TRANINFORMATION *pTran, ITEMMISC *pItem );
;
;   input    : USHORT usDestLen       -destination buffer length
;              TRANINFORMATION *pTran -transaction information
;              ITEMMISC *pItem        -item data address
;
;   output   : UCHAR *pszDest         -destination buffer address
;
;   synopis  : This function get job out date & time.
;
;============================================================================
fh**/
VOID  PrtGetJobOutDate(TCHAR *pszDest, USHORT usDestLen, TRANINFORMATION *pTran, ITEMMISC *pItem)
{
	TCHAR       aszDate[PRT_DATE_LEN + 1] = { 0 };     /* date mnemomics  */
	TCHAR       aszTime[PRT_TIME_LEN + 1 + 1] = { 0 };   /* time mnemomics "1" for space */
    USHORT      usWorkHour;
	DATE_TIME   DateTime = { 0 };                       /* for TOD read */

    PifGetDateTime(&DateTime);

    /* -- Check date status and get -- */
    if (pTran->TranCurQual.flPrintStatus & CURQUAL_DDMMYY) {
        RflSPrintf(aszDate, TCHARSIZEOF(aszDate), aszPrtDate, DateTime.usMDay, DateTime.usMonth, DateTime.usYear % 100);
    } else {
        RflSPrintf(aszDate, TCHARSIZEOF(aszDate), aszPrtDate, DateTime.usMonth, DateTime.usMDay, DateTime.usYear % 100);
    }

    /* -- Check time status -- */
    if (pTran->TranCurQual.flPrintStatus & CURQUAL_MILITARY) {

        RflSPrintf(aszTime, TCHARSIZEOF(aszTime), aszPrtTime, pItem->usTimeOutTime, pItem->usTimeOutMinute, aszPrtNull);
    } else {
        if (pItem->usTimeOutTime == 12 || pItem->usTimeOutTime == 0) {
            usWorkHour = 12;
        } else {
            usWorkHour = (pItem->usTimeOutTime >= 12)?  pItem->usTimeOutTime - 12 : pItem->usTimeOutTime;
        }

        RflSPrintf(aszTime, TCHARSIZEOF(aszTime), aszPrtTime, usWorkHour, pItem->usTimeOutMinute, (pItem->usTimeOutTime >= 12)?  aszPrtPM : aszPrtAM );
    }

    memset(pszDest, '\0', usDestLen);

    /* -- check write length ("1" for  Null) --*/
    if ( (_tcslen(aszTime) + _tcslen(aszDate) + 1) <= usDestLen) {
        _tcscpy(pszDest, aszTime);     /* write time to destination buffer */
        _tcscat(pszDest, aszDate);     /* write date to destination buffer */
    }
    return ;
}

/*
*===========================================================================
** Format  : SHORT PrtGetTaxMod(UCHAR *pszDest,
*                               USHORT usDestLen,
*                               BOOL   fTax,
*                               UCHAR  fbMod);
*
*   Input  : USHORT usDestLen       -destination buffer length
*            USHORT fsTax,          -US or Canadian Tax
*            UCHAR  fbMod           -modifier status
*
*   Output : UCHAR *pszDest         -destination buffer address
*   InOut  : none
*
** Return  : SHORT  PRT_WITHDATA    -tax modifier status on
*                   PRT_WITHOUTDATA -all tax modifiers' statuses  are off.
*
** Synopsis: This function gets Tax Modifier mnemonics
*===========================================================================
*/
SHORT  PrtGetTaxMod(TCHAR *pszDest, USHORT usDestLen, SHORT fsTax, USHORT fbMod)
{
    PARASPEMNEMO   SM;                      /* special mnemonics */
    USHORT         usWriteLen = 0;          /* written length */

    memset(pszDest, '\0', usDestLen);
    memset(SM.aszMnemonics, '\0', sizeof(SM.aszMnemonics));

    SM.uchMajorClass = CLASS_PARASPECIALMNEMO;

/*--------------
   canadian tax
  -------------- */
    if (fsTax & MODEQUAL_CANADIAN) {
        switch (PRT_TAX_MASK & fbMod) {

        case  CANTAX_MEAL << 1:
            SM.uchAddress    = SPC_TXMOD1_ADR;
            break;

        case  CANTAX_CARBON << 1:
            SM.uchAddress    = SPC_TXMOD2_ADR;
            break;

        case  CANTAX_SNACK << 1:
            SM.uchAddress    = SPC_TXMOD3_ADR;
            break;

        case  CANTAX_BEER << 1:
            SM.uchAddress    = SPC_TXMOD4_ADR;
            break;

        case  CANTAX_LIQUOR << 1:
            SM.uchAddress    = SPC_TXMOD5_ADR;
            break;

        case  CANTAX_GROCERY << 1:
            SM.uchAddress    = SPC_TXMOD6_ADR;
            break;

        case  CANTAX_TIP << 1:
            SM.uchAddress    = SPC_TXMOD7_ADR;
            break;

        case  CANTAX_PST1 << 1:
            SM.uchAddress    = SPC_TXMOD8_ADR;
            break;

        case  CANTAX_BAKED << 1:
            SM.uchAddress    = SPC_TXMOD9_ADR;
            break;

        default:
            return (PRT_WITHOUTDATA);
        }

        /* -- get tax  modifier mnemonics -- */
        CliParaRead( &SM );

        if (PARA_SPEMNEMO_LEN < usDestLen) {
            _tcsncpy(pszDest, SM.aszMnemonics, PARA_SPEMNEMO_LEN + 1);
        } else {
            return (PRT_WITHOUTDATA);
        }

        return (PRT_WITHDATA);

    }

/*--------------
     U S  tax
  -------------- */

    /* get taxmodifier1 mnemo -- */
    if (fbMod & TAX_MODIFIER1) {

        SM.uchAddress    = SPC_TXMOD1_ADR;
        CliParaRead( &SM );

        /* -- check destination buffer length ("1" for space) -- */
        if ( ( usWriteLen += (_tcslen(SM.aszMnemonics)+1) ) < usDestLen ) {
            _tcscpy(pszDest, SM.aszMnemonics);
            *(pszDest+usWriteLen-1) = PRT_SPACE;

        } else {
            return (PRT_WITHOUTDATA);
        }

/*        memset(SM.aszMnemonics, '\0', sizeof(SM.aszMnemonics)); */

    }

    /* get taxmodifier2 mnemo -- */
    if (fbMod & TAX_MODIFIER2) {

        SM.uchAddress    = SPC_TXMOD2_ADR;
        CliParaRead( &(SM) );

        /* -- check destination buffer length ("1" for space) -- */
        if ( ( usWriteLen += (_tcslen(SM.aszMnemonics)+1) ) < usDestLen ) {
            _tcscat(pszDest, SM.aszMnemonics);
            *(pszDest+usWriteLen-1) = PRT_SPACE;

        } else {
            return (PRT_WITHOUTDATA);
        }
/*        memset(SM.aszMnemonics, '\0', sizeof(SM.aszMnemonics)); */
    }

    /* get taxmodifier3 mnemo -- */
    if (fbMod & TAX_MODIFIER3) {

        SM.uchAddress    = SPC_TXMOD3_ADR;
        CliParaRead( &(SM) );

        if ( (usWriteLen += (_tcslen(SM.aszMnemonics)+1)) < usDestLen ) {
            _tcscat(pszDest, SM.aszMnemonics);
            *(pszDest+usWriteLen-1) = PRT_SPACE;

        } else {
            return (PRT_WITHOUTDATA);
        }
    }

    /* --- get FS Mod,  Saratoga --- */
    if (fbMod & FOOD_MODIFIER) {
        SM.uchAddress    = SPC_FSMOD_ADR;
        CliParaRead(&SM);
        if ((usWriteLen += _tcslen(SM.aszMnemonics)) < usDestLen) {
            _tcscat(pszDest, SM.aszMnemonics);
        } else {
            return (PRT_WITHOUTDATA);
        }
    }

    /* -- delete last space characeter --*/
    if (pszDest[usWriteLen] == PRT_SPACE) {

        pszDest[usWriteLen] = '\0';
    }

    if ( *pszDest != '\0' ) {
        return (PRT_WITHDATA);

    } else {
        return (PRT_WITHOUTDATA);

    }

}


/*
*===========================================================================
** Format  : VOID  PrtGet24Mnem(UCHAR   *pszDest, UCHAR uchAddress);
*
*   Input  : UCHAR uchAddress          -24Char Mnemonics address
*   Output : UCHAR  *pszDest,          -destination buffer address
*   InOut  : none
** Return  : none
*
** Synopsis: This function gets 24Char mnemonics.
*===========================================================================
*/
VOID  PrtGet24Mnem(TCHAR  *pszDest, USHORT usAddress)
{
	RflGet24Mnem (pszDest, usAddress);

    if ((usPrtRJColumn == PRT_21CHAR) && (pszDest[20] == 0x12)) {
        pszDest[20] = '\0';
    }
}

/*
*===========================================================================
** Format  : VOID  PrtGetLead(UCHAR   *pszDest, UCHAR uchAddress);
*
*   Input  : UCHAR uchAddress          -LeadThrough Mnemonics address
*   Output : UCHAR  *pszDest,          -destination buffer address
*   InOut  : none
** Return  : none
*
** Synopsis: This function gets Lead through mnemonics.
*===========================================================================
*/
VOID  PrtGetLead(TCHAR  *pszDest, USHORT usAddress)
{
	RflGetLead (pszDest, usAddress);

    if ( usAddress == LDT_UNIQUE_NO_ADR ) {
        *( pszDest +  12 ) = '\0';
    }
}


/*
*===========================================================================
** Format  : TCHAR *PrtSetNumber(TCHAR  *pszDest, UCHAR  *pszNumber);
*
*   Input  : UCHAR  *pszNumber                  -number address
*   Output : UCHAR  *pszDest,                   -destination buffer
*   InOut  :
** Return  : pszDest or address of destination buffer.
*
** Synopsis: This function formats number & number mnemonics with column num.
*
*            Destination buffer must be initialized to binary zeros.
*===========================================================================
*/
TCHAR *PrtSetNumber(TCHAR  *pszDest, TCHAR  *pszNumber)
{
    USHORT     usMnemLen;           /* number mnemonics length */
    USHORT     usNumLen;            /* number length */
    USHORT     usFieldLen;          /* rest  columns */

    usMnemLen = _tcslen(pszDest);
    usNumLen = _tcslen(pszNumber);

    /* -- write space charcter -- */
    *(pszDest + usMnemLen) = PRT_SPACE;

    /* -- get rest columns ( " 1" for space) -- */
    usFieldLen = usPrtRJColumn - usMnemLen - 1;

    /* -- within 1 line? -- */
    if ( usFieldLen >= usNumLen) {
        /* -- write number( "1" for space)-- */
        _tcsncpy(&pszDest[usMnemLen + 1], pszNumber, usNumLen);
        return pszDest;
    }

    /* -- write number( "1" for space)-- */
    _tcsncpy(&pszDest[usMnemLen + 1], pszNumber, usFieldLen);

    /* -- write space on 2nd line -- */
    tcharnset(&pszDest[usPrtRJColumn], PRT_SPACE, usMnemLen + 1);

    /* -- write 2nd line -- */
    if ( usFieldLen >= (usNumLen-usFieldLen) ) {
        /* -- write number( "1" for space)-- */
        _tcsncpy(&pszDest[usPrtRJColumn + usMnemLen +1], &pszNumber[usFieldLen], usNumLen-usFieldLen);
    } else {
        /* -- write number( "1" for space)-- */
        _tcsncpy(&pszDest[usPrtRJColumn + usMnemLen +1], &pszNumber[usFieldLen], usFieldLen);
    }

	return pszDest;
}

/*
*===========================================================================
** Format   : VOID PrtSetItem( UCHAR     *puchDest,
*                              ITEMSALES *pItem,
*                              DCURRENCY *plAllPrice,
*                              SHORT     sFormatType )
*
*   Input   : ITEMSALES *pItem      - address of source sales item
*             SHORT     sType       - type of print format
*                                       PRT_ONLY_NOUN
*                                       PRT_ONLY_PMCOND
*                                       PRT_ALL_ITEM
*
*   Output  : UCHAR     *pDest      - destination buffer address
*             DCURRENCY *plAllPrice - price of all products
*
*   InOut   : none
** Return   : none
*
** Synopsis : This function prepares a print format of a sales item with
*           adjective and mnemoic and print modifiers, condiments.
*===========================================================================
*/
VOID PrtSetItem( TCHAR *puchDest, ITEMSALES *pItem, DCURRENCY *plAllPrice,
                                        SHORT sFormatType, USHORT usColumn )
{
    BOOL        fIsPLUItem;
    TCHAR       *puchWrtPtr;
    USHORT      usWrtLen;

    puchWrtPtr = puchDest;
    *plAllPrice = pItem->lProduct;

    /* --- prepare main sales item's mnemonic (and adjective) --- */
    if ( (sFormatType == PRT_ONLY_NOUN) ||
         (((sFormatType == PRT_ALL_ITEM)||(sFormatType == PRT_NOT_ZERO_COND))&&
            ((_tcsncmp(pItem->auchPLUNo, MLD_NO_DISP_PLU_LOW,  NUM_PLU_LEN) < 0) ||   /* not print if plu no is over 9950 */
             (_tcsncmp(pItem->auchPLUNo, MLD_NO_DISP_PLU_HIGH, NUM_PLU_LEN) > 0)) ) ) {   /* not print if plu no is over 9950 */
            /* (pItem->usPLUNo < PRT_NOT_PRINT_PLU_NO)) ) {   / not print if plu no is over 9950 */

        /* --- determine source item is PLU item or department item --- */
        fIsPLUItem = ( ! (( pItem->uchMinorClass == CLASS_DEPT ) ||
                          ( pItem->uchMinorClass == CLASS_DEPTITEMDISC ) ||
                          ( pItem->uchMinorClass == CLASS_DEPTMODDISC )));

		/* SI symnbol, before mnemonics 21RFC05437 */
		if ( CliParaMDCCheck(MDC_TAX2_ADR, EVEN_MDC_BIT1) &&
			 CliParaMDCCheck(MDC_TAX2_ADR, EVEN_MDC_BIT2) ) {

		    if (((fsPrtStatus & PRT_REQKITCHEN) == 0) &&	/* not KP, KDS */
                !(pItem->fsPrintStatus & (PRT_VALIDATION | PRT_SINGLE_RECPT | PRT_DOUBLE_RECPT))) {  /* not validation, tichet */

				if (PrtGetSISymSales( puchWrtPtr, pItem)) {

    		        /* --- make a space between si mnemonic and main sales
        		        item's mnemonic --- */
	        	    usWrtLen = _tcslen( puchWrtPtr );
    	        	*( puchWrtPtr + usWrtLen ) = PRT_SPACE;

	        	    puchWrtPtr += ( usWrtLen + 1 );
	        	}
			}
		}

        if (( 0 < pItem->uchAdjective ) && fIsPLUItem && pItem->uchMinorClass != CLASS_ITEMORDERDEC ) {

            /* --- prepare adjective mnemonic to destination buffer,
                if main item is adjective item --- */
			RflGetAdj( puchWrtPtr, pItem->uchAdjective );

            /* --- make a space between adjective mnemonic and main sales
                item's mnemonic --- */
            usWrtLen = _tcslen( puchWrtPtr );
            *( puchWrtPtr + usWrtLen ) = PRT_SPACE;

            puchWrtPtr += ( usWrtLen + 1 );
        }

        /* --- prepare main item mnemonic --- */
        usWrtLen = _tcslen( pItem->aszMnemonic );
        _tcsncpy( puchWrtPtr, pItem->aszMnemonic, usWrtLen );

        if (( sFormatType == PRT_ALL_ITEM ) || (sFormatType == PRT_NOT_ZERO_COND )){    /* format all items */

			/* SI symnbol, after mnemonics 21RFC05437 */
			if ( CliParaMDCCheck(MDC_TAX2_ADR, EVEN_MDC_BIT1) &&
				!CliParaMDCCheck(MDC_TAX2_ADR, EVEN_MDC_BIT2) ) {

		    	if (((fsPrtStatus & PRT_REQKITCHEN) == 0) &&	/* not KP, KDS */
                	!(pItem->fsPrintStatus & (PRT_VALIDATION | PRT_SINGLE_RECPT | PRT_DOUBLE_RECPT))) {  /* not validation, tichet */

   		        	*( puchWrtPtr + usWrtLen ) = PRT_SPACE;
		            puchWrtPtr += ( usWrtLen + 1 );

					PrtGetSISymSales( puchWrtPtr, pItem);
	        	    usWrtLen = _tcslen( puchWrtPtr );
				}
			}
            /* --- make a space between main sales item's mnemonic and
                print modifier's mnemonic --- */
            *( puchWrtPtr + usWrtLen ) = PRT_SEPARATOR;
        } else {                                /* format only main item */
			/* SI symnbol, after mnemonics 21RFC05437 */
			if ( CliParaMDCCheck(MDC_TAX2_ADR, EVEN_MDC_BIT1) &&
				!CliParaMDCCheck(MDC_TAX2_ADR, EVEN_MDC_BIT2) ) {

		    	if (((fsPrtStatus & PRT_REQKITCHEN) == 0) &&	/* not KP, KDS */
                	!(pItem->fsPrintStatus & (PRT_VALIDATION | PRT_SINGLE_RECPT | PRT_DOUBLE_RECPT))) {  /* not validation, tichet */

   		        	*( puchWrtPtr + usWrtLen ) = PRT_SPACE;
		            puchWrtPtr += ( usWrtLen + 1 );

					PrtGetSISymSales( puchWrtPtr, pItem);
	        	    usWrtLen = _tcslen( puchWrtPtr );
	        	}
			}
            /* --- make a null terminator --- */
            *( puchWrtPtr + usWrtLen ) = '\0';
        }
        puchWrtPtr += ( usWrtLen + 1 );
    }

    /* --- prepare print modifiers and condiment items --- */
    if (( sFormatType == PRT_ALL_ITEM ) || ( sFormatType == PRT_ONLY_PMCOND ) || (sFormatType == PRT_NOT_ZERO_COND )) {

        if ( CliParaMDCCheck( MDC_PRINTPRI_ADR, EVEN_MDC_BIT0 ) &&
             CliParaMDCCheck( MDC_PRINTPRI_ADR, EVEN_MDC_BIT2 )) {

            /* --- condiment items are printed with print priority --- */
            usWrtLen = PrtSetChildPriority( puchWrtPtr, pItem, plAllPrice, sFormatType, usColumn);

            if (( sFormatType != PRT_ONLY_PMCOND ) || ( 0 < pItem->uchPrintModNo )) {
                /* --- erase SEPARATOR --- */
                *( puchWrtPtr + usWrtLen - 1 ) = PRT_SPACE;
            }
        } else {
            /* --- condiment items are printed with entry order --- */
            usWrtLen = PrtSetChildNotOrder( puchWrtPtr, pItem, plAllPrice, sFormatType, usColumn );
        }

        puchWrtPtr += usWrtLen;
    }

    if (( sFormatType != PRT_ONLY_PMCOND ) ||
        ( 0 < pItem->uchPrintModNo ) ||
        ( 0 < pItem->uchCondNo )) {

        /* --- delete last space chara, and make null terminator --- */
        *( puchWrtPtr - 1 ) = PRT_SPACE;
    }
    /* --- make a null terminator --- */

    *puchWrtPtr = '\0';
}

/*
*===========================================================================
** Format   : USHORT PrtSetChildPriority( UCHAR     *puchDest,
*                                         ITEMSALES *pItem,
*                                         DCURRENCY *plAllPrice )
*
*   Input   : ITEMSALES *pItem      - address of source sales item
*
*   Output  : UCHAR     *puchDest   - destination buffer address
*             DCURRENCY *plAllPrice - price of all products
*
*   InOut   : none
** Return   : USHORT    length of write buffer
*
** Synopsis : This function prepares a print format of print modifiers and
*           condiments with priority order.
*===========================================================================
*/
USHORT PrtSetChildPriority( TCHAR *puchDest, ITEMSALES *pItem, DCURRENCY *plAllPrice,
                             SHORT sFormatType, USHORT usColumn)
{
    BOOL        fIsPLUItem;
    PARAPRTMODI PrtMod;
    USHORT      usWrtLen, usAdjLen;
    USHORT      usMaxChild;
    USHORT      usI;
    TCHAR       *puchStart;
	TCHAR       auchDummy[NUM_PLU_LEN] = {0};

    puchStart = puchDest;
    PrtMod.uchMajorClass = CLASS_PARAPRTMODI;

    usMaxChild = ( pItem->uchChildNo + pItem->uchCondNo + pItem->uchPrintModNo );

    if ( sFormatType != PRT_NOT_ZERO_COND) {    /* not print zero price */
        for ( usI = 0; usI < usMaxChild; usI++ ) {
            if ( pItem->auchPrintModifier[ usI ] ) {
                /* --- prepare print modifier mnemonic --- */
                PrtMod.uchAddress = pItem->auchPrintModifier[ usI ];
                CliParaRead( &PrtMod );

                usWrtLen = _tcslen( PrtMod.aszMnemonics );
                _tcsncpy( puchDest, PrtMod.aszMnemonics, usWrtLen );

                /* --- make a space between mnemonic --- */
                if (usColumn) { /* if separeted print option, V3.3 */
                    if (usColumn-usWrtLen) {
                        tcharnset(puchDest+usWrtLen, PRT_SPACE, (usColumn-usWrtLen));
                    }
                    *( puchDest + usColumn ) = PRT_SEPARATOR;
                    puchDest += ( usColumn + 1 );
                } else {
                    *( puchDest + usWrtLen ) = PRT_SEPARATOR;
                    puchDest += ( usWrtLen + 1 );
                }
            }
        }
    }

    /* --- determine source item is PLU item or Department Item --- */
    fIsPLUItem = ( ! (( pItem->uchMinorClass == CLASS_DEPT ) ||
                      ( pItem->uchMinorClass == CLASS_DEPTITEMDISC ) ||
                      ( pItem->uchMinorClass == CLASS_DEPTMODDISC )));

    if ( fIsPLUItem ) {
        for ( usI = ( USHORT )pItem->uchChildNo; usI < usMaxChild; usI++ ) {
            if (_tcsncmp(pItem->Condiment[ usI ].auchPLUNo, auchDummy, NUM_PLU_LEN) != 0 ) {
            /* if ( pItem->Condiment[ usI ].usPLUNo ) { */
                if (( sFormatType == PRT_NOT_ZERO_COND) && ( pItem->Condiment[ usI ].lUnitPrice == 0L )) {
                    /* not print zero price */
                    continue;
                } else if (((_tcsncmp(pItem->Condiment[ usI ].auchPLUNo, MLD_NO_DISP_PLU_LOW, NUM_PLU_LEN) >= 0) &&
                            (_tcsncmp(pItem->Condiment[ usI ].auchPLUNo, MLD_NO_DISP_PLU_HIGH, NUM_PLU_LEN) <= 0))
                /* } else if ((pItem->Condiment[ usI ].usPLUNo >= PRT_NOT_PRINT_PLU_NO) */
                            && !((fsPrtStatus & (PRT_REQKITCHEN|PRT_TAKETOKIT)) ||  /* kitchen print */
                                 (pItem->fsPrintStatus & (PRT_SINGLE_RECPT|PRT_DOUBLE_RECPT)))) {
                                 /* ticket print status */

                    /* --- get all product price --- */
                    *plAllPrice += ( pItem->Condiment[ usI ].lUnitPrice * ( pItem->lQTY / 1000 ));

                    /* ---- not print special plu no ---- */
                    continue;
                } else {
                    if ( pItem->Condiment[ usI ].uchAdjective ) {
                        /* --- prepare adjective mnemonic to destination buffer,
                            if condiment item is adjective item --- */
						RflGetAdj( puchDest, pItem->Condiment[ usI ].uchAdjective );

                        /* --- make a space between adjective mnemonic and main
                            item's mnemonic --- */
                        usWrtLen = _tcslen( puchDest );
                        *( puchDest + usWrtLen ) = PRT_SPACE;

                        puchDest += ( usWrtLen + 1 );
                        usAdjLen = usWrtLen+1;
                    } else {
                        usAdjLen = 0;
                    }

                    usWrtLen = _tcslen( pItem->Condiment[ usI ].aszMnemonic );
                    _tcsncpy( puchDest, pItem->Condiment[ usI ].aszMnemonic, usWrtLen );

                    /* --- make a space between mnemonic --- */
                    if (usColumn) { /* if separeted print option, V3.3 */
                        if (usColumn-usWrtLen-usAdjLen) {
                            tcharnset(puchDest+usWrtLen+usAdjLen, PRT_SPACE, (usColumn-usWrtLen-usAdjLen));
                        }
                        *( puchDest + usColumn ) = PRT_SEPARATOR;
                        puchDest += ( usColumn + 1 );
                    } else {
                        usWrtLen = _tcslen( puchDest );
                        *( puchDest + usWrtLen ) = PRT_SEPARATOR;

                        puchDest += ( usWrtLen + 1 );
                    }

                    /* --- get all product price --- */
                    *plAllPrice += ( pItem->Condiment[ usI ].lUnitPrice * ( pItem->lQTY / 1000 ));
                }
            }
        }   /* end for */
    }
    return (( USHORT )( puchDest - puchStart ));
}

/*
*===========================================================================
** Format   : USHORT PrtSetChildNotOrder( UCHAR     *puchDest,
*                                         ITEMSALES *pItem,
*                                         DCURRENCY *plAllPrice )
*
*   Input   : ITEMSALES *pItem      - address of source sales item
*
*   Output  : UCHAR     *pDest      - destination buffer address
*             DCURRENCY *plAllPrice - price of all products
*
*   InOut   : none
** Return   : USHORT    length of write buffer
*
** Synopsis : This function prepares a print format of print modifiers and
*           condiments with no order (input order).
*===========================================================================
*/
USHORT PrtSetChildNotOrder( TCHAR *puchDest, ITEMSALES *pItem, DCURRENCY *plAllPrice,
                             SHORT sFormatType, USHORT usColumn)
{
    BOOL        fIsPLUItem;
    PARAPRTMODI PrtMod;
    USHORT      usWrtLen, usAdjLen;
    USHORT      usMaxChild;
    USHORT      usI;
    TCHAR       *puchStart;
	TCHAR       auchDummy[NUM_PLU_LEN] = {0};

    puchStart = puchDest;

    /* --- determine source item is PLU item or Department Item --- */
    fIsPLUItem = ( ! (( pItem->uchMinorClass == CLASS_DEPT ) ||
                      ( pItem->uchMinorClass == CLASS_DEPTITEMDISC ) ||
                      ( pItem->uchMinorClass == CLASS_DEPTMODDISC )));

    PrtMod.uchMajorClass = CLASS_PARAPRTMODI;
    usMaxChild = ( pItem->uchChildNo + pItem->uchCondNo + pItem->uchPrintModNo );

    for ( usI = 0; usI < usMaxChild; usI++ ) {
        if ( pItem->auchPrintModifier[ usI ] ) {
            if ( sFormatType == PRT_NOT_ZERO_COND) {
                /* not print zero price */
                continue;
            } else {
                /* --- prepare print modifier mnemonic --- */
                PrtMod.uchAddress = pItem->auchPrintModifier[ usI ];
                CliParaRead( &PrtMod );

                usWrtLen = _tcslen( PrtMod.aszMnemonics );
                _tcsncpy( puchDest, PrtMod.aszMnemonics, usWrtLen );

                /* --- make a space between mnemonic --- */
                if (usColumn) { /* if separeted print option, V3.3 */
                    if (usColumn-usWrtLen) {
                        tcharnset(puchDest+usWrtLen, PRT_SPACE, (usColumn-usWrtLen));
                    }
                    *( puchDest + usColumn ) = PRT_SEPARATOR;
                    puchDest += ( usColumn + 1 );
                } else {
                    *( puchDest + usWrtLen ) = PRT_SEPARATOR;
                    puchDest += ( usWrtLen + 1 );
                }
            }
        }

        if (_tcsncmp(pItem->Condiment[ usI ].auchPLUNo, auchDummy, NUM_PLU_LEN) != 0 ) {
            /* --- prepare condiment item's mnemonic --- */
            if ((( USHORT )pItem->uchChildNo <= usI ) && fIsPLUItem ) {
                if (( sFormatType == PRT_NOT_ZERO_COND) && ( pItem->Condiment[ usI ].lUnitPrice == 0L )) {
                    /* not print zero price */
                    continue;
                } else if (((_tcsncmp(pItem->Condiment[ usI ].auchPLUNo, MLD_NO_DISP_PLU_LOW, NUM_PLU_LEN) >= 0) &&
                            (_tcsncmp(pItem->Condiment[ usI ].auchPLUNo, MLD_NO_DISP_PLU_HIGH, NUM_PLU_LEN) <= 0))
                /* } else if ((pItem->Condiment[ usI ].usPLUNo >= PRT_NOT_PRINT_PLU_NO)  */
                            && !((fsPrtStatus & (PRT_REQKITCHEN|PRT_TAKETOKIT)) ||  /* kitchen print */
                                 (pItem->fsPrintStatus & (PRT_SINGLE_RECPT|PRT_DOUBLE_RECPT)))) {
                                 /* ticket print status */

                    /* --- get all product price --- */
                    *plAllPrice += ( pItem->Condiment[ usI ].lUnitPrice * ( pItem->lQTY / 1000 ));
                    /* ---- not print special plu no ---- */
                    continue;
                } else {
                    if ( pItem->Condiment[ usI ].uchAdjective ) {

                        /* --- prepare adjective mnemonic,
                            if condiment item is adjective --- */
						RflGetAdj( puchDest, pItem->Condiment[ usI ].uchAdjective );

                        /* --- make a space between mnemonic --- */
                        usWrtLen = _tcslen( puchDest );
                        *( puchDest + usWrtLen ) = PRT_SPACE;
                        puchDest += ( usWrtLen + 1 );
                    } else {
                        usAdjLen = 0;
                    }

                    usWrtLen = _tcslen( pItem->Condiment[ usI ].aszMnemonic );
                    _tcsncpy( puchDest, pItem->Condiment[ usI ].aszMnemonic, usWrtLen );

                    /* --- make a space between mnemonic --- */
                    if (usColumn) { /* if separeted print option, V3.3 */
                        if (usColumn-usWrtLen-usAdjLen) {
                            tcharnset(puchDest+usWrtLen+usAdjLen, PRT_SPACE, (usColumn-usWrtLen-usAdjLen));
                        }
                        *( puchDest + usColumn ) = PRT_SEPARATOR;
                        puchDest += ( usColumn + 1 );
                        usAdjLen = usWrtLen+1;
                    } else {
                        usWrtLen = _tcslen( puchDest );
                        *( puchDest + usWrtLen ) = PRT_SEPARATOR;
                        puchDest += ( usWrtLen + 1 );
                    }

                    /* --- get all product price --- */
                    *plAllPrice += ( pItem->Condiment[ usI ].lUnitPrice * ( pItem->lQTY / 1000 ));
                }
            }
        }
    }   /* end for */

    return (( USHORT )( puchDest - puchStart ));
}

/*
*===========================================================================
** Format  : SHORT  PrtGet1Line(UCHAR *pStart, UCHAR *pEnd, USHORT usColumn);
*
*   Input  : UCHAR      *pStart        -search start address
*            USHORT     usColumn       -Column Number
*
*   Output : UCHAR      **pEnd,        -end address
*
*   InOut  : none
** Return  : SHORT      PRT_CONTINUE   -string continue
*                       PRT_LAST       -string end
*
** Synopsis: This function searches last space or null with specified column no.
*            when function searches space or null, it moves current pointer to the point
*            that current pointer + usColumn.
*
*            The function works by following rules.
*            -moved pointer points "space"
*               function returns immediatery
*            -moved pointer points "NULL"
*               function decides last strings
*            -moved pointer points other character
*               function moves pointer back until pointer points "space"
*
*            So when function searches last 1 line, there needs enough
*            work space (ALL ITEM's bytes + Column's bytes) filled with NULL
*            to point NULL.
*
*===========================================================================
*/
SHORT  PrtGet1Line(TCHAR *pStart, TCHAR **pEnd, USHORT usColumn)
{
    TCHAR  *puchWork;
    USHORT i;

    puchWork = pStart + usColumn;

    if (( *puchWork == PRT_SPACE ) || ( *puchWork == PRT_SEPARATOR )) {

        *pEnd = puchWork;
        return (PRT_CONTINUE);

    } else if ( *puchWork == '\0' ) {

        return (PRT_LAST);
    }

    for ( i = 0; ; i++ ) {

        if (( *(puchWork - i)  == PRT_SPACE ) ||
            ( *(puchWork - i)  == PRT_SEPARATOR )) {

            *pEnd = puchWork - i;
            return (PRT_CONTINUE);

        }
    }
}

/*
*===========================================================================
** Format  : SHORT  PrtChkCasWai(UCHAR *pszMnem, USHORT usCasID,
*                                USHORT usWaiID );
*
*   Input  : USHORT usCasID            -Cashier ID
*            USHORT usWaiID            -Waiter  ID
*
*   Output : UCHAR  *pszMnem           -Cashier or Waiter Mnemonics
*
*   InOut  : none
** Return  : SHORT      PRT_CASHIER    -cashier
*                       PRT_WAITER     -waiter
*
** Synopsis: This function checks cashier's and waiter's ID , gets Mnemonics
*===========================================================================
*/
SHORT  PrtChkCasWai(TCHAR *pszMnem, ULONG ulCasID, ULONG ulWaiID)
{
    if ( ulWaiID != 0) {
        /* V3.3 */
		RflGetSpecMnem(pszMnem, SPC_CAS_ADR);
        /* RflGetSpecMnem(pszMnem, SPC_WT_ADR); */
        return (PRT_WAITER);
    } else if ( ulCasID != 0 ) {
		RflGetSpecMnem(pszMnem, SPC_CAS_ADR);
        return (PRT_CASHIER);
    } else {
        *pszMnem = '\0';
        return (PRT_CASHIER);
    }
}


/*
*===========================================================================
** Format  : VOID  PrtGetAllProduct(ITEMSALES *pItem, DCURRENCY *plProduct);
*
*   Input  : ITEMSALES *pItem          -iten information
*   Output : DCURRENCY *plProduct      -all product
*   InOut  : none
** Return  : none
*
** Synopsis: This function  get all product price(noun own and condiments')
*===========================================================================
*/
VOID  PrtGetAllProduct(ITEMSALES *pItem, DCURRENCY *plProduct)
{
    USHORT      i;
    USHORT      usNoOfChild;
    DCURRENCY   lChildPrice;
	TCHAR       auchDummy[NUM_PLU_LEN] = {0};

    usNoOfChild = pItem->uchCondNo + pItem->uchPrintModNo + pItem->uchChildNo;

    for ( i = pItem->uchChildNo, lChildPrice = 0L; i < usNoOfChild; i++) {
        if (_tcsncmp(pItem->Condiment[ i ].auchPLUNo, auchDummy, NUM_PLU_LEN) != 0 ) {
            lChildPrice += pItem->Condiment[ i ].lUnitPrice;
        }
    }

    *plProduct = pItem->lProduct + (( pItem->lQTY / 1000 ) * lChildPrice );
}

/*
*===========================================================================
** Format  : VOID  PrtAdjustNative(UCHAR *pszDest, LONG lAmount);
*
*   Input  : LONG      lAmount         -amount
*   Output : UCHAR     *pszDest        -destination buffer address
*   InOut  : none
** Return  : none
*
** Synopsis: This function adjusts native mnemonics and amount sign
*===========================================================================
*/
VOID  PrtAdjustNative(TCHAR *pszDest, DCURRENCY lAmount)
{
	CONST TCHAR FARCONST aszPrtAmount[]  = _T("%l$");          /* amount */
	TCHAR  aszSpecMnem[PARA_SPEMNEMO_LEN + 1] = {0};  /* PARA_... defined in "paraequ.h" */
    USHORT usStrLen;

    /* -- get native mnemonics -- */
	RflGetSpecMnem(aszSpecMnem, SPC_CNSYMNTV_ADR);

    /* -- get string length -- */
    usStrLen = _tcslen(aszSpecMnem);

    /* -- convert long to ASCII -- */
    RflSPrintf(&pszDest[usStrLen], PRT_RJCOLUMN + 1, aszPrtAmount, lAmount);

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
** Format  : VOID  PrtFillSpace(UCHAR  *pszString, USHORT  usSize);
*
*   Input  : UCHAR   *pszString          -string address
*            USHORT  usSize              -size
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function fills "space" behind pszString with usSize.
*===========================================================================
*/
VOID  PrtFillSpace(TCHAR   *pszString, USHORT  usSize)
{

    USHORT   usStrLen;

    usStrLen = _tcslen(pszString);

    if (usStrLen >= usSize) {
        return ;
    }

    tcharnset(&pszString[usStrLen], PRT_SPACE, usSize - usStrLen);
    //memset(&pszString[usStrLen], PRT_SPACE, usSize - usStrLen);

}

/*
**************************************************************************
*
**Synopsis:  VOID   PrtPrint(USHORT usPrtType, UCHAR *puchBuff, USHORT usLen)
*
*   Input  : USHORT usPrtType;  : type of printer
*            UCHAR  *pszFormat; : pointer to print buffer
*            USHORT usLen       : buffer length
*   Output : none
*   InOut  : none
** Return  : none
*
**Description: call pmg function
*
**************************************************************************
*/

USHORT    PrtPrint(USHORT usPrtType, TCHAR *puchBuff, USHORT usLen)
{
    return PmgPrint(usPrtType, puchBuff, usLen);
}

/*
*===========================================================================
** Format  : VOID  PrtEndValidation(USHORT usPort);
*
*   Input  : USHORT     usPort                  -print portion
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function calls "PmgEndValidation" and waits.
*===========================================================================
*/
VOID  PrtEndValidation(USHORT usPort)
{

    PmgEndValidation(usPort);                   /* end validation */

    PrtPmgWait();                               /* wait print spooler become empty */

}

/*
*===========================================================================
** Format  : VOID   PrtSetRJColumn(VOID);
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function sets receipt and journal's columns to
*            static area " usPrtRJColumn"
*===========================================================================
*/
VOID   PrtSetRJColumn(VOID)
{

    USHORT  fbStatus;

    PmgPrtConfig(PMG_PRT_RECEIPT, &usPrtRJColumn, &fbStatus);

    /* -- get printer type, and set validation type -- */

    if ( usPrtRJColumn == PRT_24CHAR ) {
        if (fbStatus & PMG_VALIDATION) {
            usPrtVLColumn = PRT_VL_24;
        } else {
            usPrtVLColumn = PRT_VL_21;
        }
    }

    if ( usPrtRJColumn == PRT_21CHAR ) {
        usPrtVLColumn = PRT_VL_21;
    }

}

/*
*===========================================================================
** Format  : VOID  PrtPmgWait(VOID)
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function calls pmgwait()
*===========================================================================
*/
VOID  PrtPmgWait(VOID)
{

    PifRequestSem(usShrSemDrawer);              /* request sem. for PmgWait() */

    PmgWait();                                  /* wait print spooler becomes empty */

    PifReleaseSem(usShrSemDrawer);              /* release sem. for PmgWait() */

}

/*
*===========================================================================
** Format  : VOID  PrtSlipPmgWait(VOID)
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function calls pmgwait()
*===========================================================================
*/
VOID  PrtSlipPmgWait(VOID)
{
    if (PifSysConfig()->auchComPort[2] != DEVICE_SLIP_PRINTER) {
        return ;
    }

    PrtPmgWait();
}

/*
*===========================================================================
** Format  : SHORT  PrtChkFullSP(UCHAR  *pszWork)
*
*   Input  : UCHAR  *pszWork     -pointer of the target line
*   Output : none
*   InOut  : none
** Return  : PRT_FULL_SP:   filled with SP/NULL
*                      0:   not filled with SP/NULL
*
** Synopsis: This function checks if 1 line is filled with space or NULL.
*===========================================================================
*/
SHORT  PrtChkFullSP(TCHAR   *pszWork)
{
    USHORT  usI, usLen;

    usLen = _tcslen(pszWork);
    for ( usI = 0; usI < usLen; usI ++, pszWork ++ ) {

        if ( ( *pszWork != PRT_SPACE ) && ( *pszWork != '\0' ) ) {

            return(0);
        }

    }

    return(PRT_FULL_SP);

}

/*
*===========================================================================
** Format  : VOID PrtSPVLInit(VOID)
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function initialize current slip's pages and lines,
*            checks slip form in or out, decides print portion.
*===========================================================================
*/
USHORT PrtSPVLInit(VOID)
{

//    UCHAR  fbPrtStatus;                /* printer status */
//    USHORT usRet;                      /* return value */
    USHORT usPrevious;
    USHORT  usColumn = 0;
    USHORT  fbStatus;

	/* if slip printer is not prepaired, not call slip function, 09/11/01 */
    if (PmgPrtConfig(PMG_PRT_SLIP, &usColumn, &fbStatus) != PMG_SUCCESS) {

        usColumn = 0;   /* assume as thermal printer by default */
    }
    if (usColumn == 0) {
        return PRT_FAIL;
    }


    PrtSlipPmgWait();                  /* waits until spool buffer is empty */

    /* allow power switch at error display */
    usPrevious = PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */

    for (;;) {

        /* -- get printer status -- */
//        usRet = PmgGetStatus(PMG_PRT_SLIP, &fbPrtStatus);

       // if (usRet == PMG_SUCCESS ) {

            /* -- form in? -- */
           // if ((fbPrtStatus & PRT_STAT_FORM_IN) && (fbPrtStatus & PRT_STAT_ERROR)) {

                PifControlPowerSwitch(usPrevious);  /* V1.0.04 */
                return PRT_SUCCESS;

            //} else {

                /* allow power switch at error display */
            //    PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */
                /* -- require form in -- */
             //   UieErrorMsg(LDT_SLPINS_ADR, UIE_WITHOUT);
           // }
       // } else {

            /* allow power switch at error display */
        //    PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */
            /* display "printer trouble"  and wait clear */
        //    usRet = UieErrorMsg(LDT_PRTTBL_ADR, UIE_ABORT);

            /* if abort key entered, set receipt compul and exit */
          //  if (usRet == FSC_CANCEL || usRet == FSC_P5) {

          //      PrtRectCompul();             /* set receipt compulsory */
          //      PifControlPowerSwitch(usPrevious);  /* V1.0.04 */
           //     return;
           // }
        //}
    }
    PifControlPowerSwitch(usPrevious);  /* V1.0.04 */
}

/*
*===========================================================================
** Format  : SHORT PrtChkVoidConsolidation(TRANINFORMATION  *pTran, ITEMSALES *pItem);
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMSALES        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : TRUE   -enable consolidate
*            FALSE  -not consolidate
*
** Synopsis: This function checks void consolidation is available or not.
*===========================================================================
*/
SHORT PrtChkVoidConsolidation(TRANINFORMATION  *pTran, ITEMSALES *pItem)
{

    if ((pTran->TranCurQual.auchTotalStatus[4] & CURQUAL_TOTAL_NOCONSOLIDATE_PRINT) ||
        ((pTran->TranCurQual.flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_UNBUFFER) ||        /* unbuffering print */
        (pItem->fsPrintStatus & (PRT_VALIDATION | PRT_SINGLE_RECPT | PRT_DOUBLE_RECPT)) ||
        (pItem->uchPM) ||   /* saratoga */
        (pItem->uchPPICode)) {

        return(FALSE);

    } else {

        if ( fsPrtStatus & PRT_TAKETOKIT ) {     /* kitchen print */
            if ( pTran->TranCurQual.uchKitchenStorage == PRT_KIT_NET_STORAGE) {

                return(TRUE);  /* print void item by mdc */
            }

            return(FALSE);
        }

        /* receipt/slip print */
        if ( CliParaMDCCheck(MDC_DEPT3_ADR, ODD_MDC_BIT2) == 0) {

            return(FALSE);  /* mdc option */
        }

        return(TRUE);
    }
}

/*
*===========================================================================
** Format  : SHORT PrtChkPrintPluNo(ITEMSALES *pItem);
*
*   Input  : ITEMSALES        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : TRUE   -enable consolidate
*            FALSE  -not consolidate
*
** Synopsis:
*===========================================================================
*/
SHORT PrtChkPrintPluNo(ITEMSALES *pItem)
{
    SHORT i;
    UCHAR uchCondNo;

    if ((_tcsncmp(pItem->auchPLUNo, MLD_NO_DISP_PLU_LOW, NUM_PLU_LEN) >= 0) &&
        (_tcsncmp(pItem->auchPLUNo, MLD_NO_DISP_PLU_HIGH, NUM_PLU_LEN) <= 0)) {
    /* if (pItem->usPLUNo >= PRT_NOT_PRINT_PLU_NO) { */
        switch (PrtChkPrintType(pItem)) {
        case PRT_ONLY_NOUN:
            return (FALSE);

        case PRT_NOT_ZERO_COND:
            for (i = pItem->uchChildNo; i < STD_MAX_COND_NUM; i++) {
                if (pItem->Condiment[i].lUnitPrice) {
                    if ((_tcsncmp(pItem->Condiment[ i ].auchPLUNo, MLD_NO_DISP_PLU_LOW, NUM_PLU_LEN) < 0) ||
                        (_tcsncmp(pItem->Condiment[ i ].auchPLUNo, MLD_NO_DISP_PLU_HIGH, NUM_PLU_LEN) > 0)) {
                    /* if (pItem->Condiment[i].usPLUNo < PRT_NOT_PRINT_PLU_NO) { */
                        break;
                    }
                }
            }
            if (i == STD_MAX_COND_NUM) {
                return (FALSE);
            }
        default:
            if ((fsPrtStatus & (PRT_REQKITCHEN|PRT_TAKETOKIT)) ||  /* kitchen print */
                    (pItem->fsPrintStatus & (PRT_SINGLE_RECPT | PRT_DOUBLE_RECPT)) ) {
                    /* ticket print status */

                uchCondNo = pItem->uchCondNo;

            } else {    /* R/J, Slip Print */

                for (i = pItem->uchChildNo, uchCondNo = pItem->uchCondNo; i < STD_MAX_COND_NUM; i++) {
                    if ((_tcsncmp(pItem->Condiment[ i ].auchPLUNo, MLD_NO_DISP_PLU_LOW, NUM_PLU_LEN) >= 0) &&
                        (_tcsncmp(pItem->Condiment[ i ].auchPLUNo, MLD_NO_DISP_PLU_HIGH, NUM_PLU_LEN) <= 0)) {
                    /* if (pItem->Condiment[i].usPLUNo >= PRT_NOT_PRINT_PLU_NO) { */
                        uchCondNo--;
                    }
                }
            }
            if ((pItem->uchPrintModNo + uchCondNo ) == 0) {
                return (FALSE);
            }
        }
    }

    return(TRUE);
}

/*
*===========================================================================
** Format  : SHORT PrtChkPrintType(ITEMSALES *pItem);
*
*   Input  : ITEMSALES        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  :
*
*
** Synopsis:
*===========================================================================
*/
SHORT PrtChkPrintType(ITEMSALES *pItem)
{

    if ((fsPrtStatus & (PRT_REQKITCHEN|PRT_TAKETOKIT)) ||  /* kitchen print */
            (pItem->fsPrintStatus & (PRT_SINGLE_RECPT | PRT_DOUBLE_RECPT)) ) {
            /* ticket print status */

        return(PRT_ALL_ITEM);

    } else {
        if (CliParaMDCCheck(MDC_DEPT3_ADR, ODD_MDC_BIT0)) {
            if (CliParaMDCCheck(MDC_DEPT3_ADR, ODD_MDC_BIT1)) {

                return(PRT_ONLY_NOUN);      /* not print condiment */

            } else {

                return(PRT_NOT_ZERO_COND);  /* print if not 0.00 */
            }
        } else {

            return(PRT_ALL_ITEM);
        }
    }
}


/*
*===========================================================================
** Synopsis:    VOID   PrtParaRead(VOID *pDATA);
*                                       
*     InPut:    pDATA
*               
*
** Return:      Masked Bit Patern
*
** Description: This function reads MDC parameter on system RAM.
*===========================================================================
*/
VOID   PrtParaRead(VOID *pDATA)
{
    CliParaRead(pDATA);
}

/*
*===========================================================================
** Format  : VOID  PrtSetQtyDecimal( SHORT  *psDecPoint LONG  *lQty);
*
*   Input  : LONG lQty
*
*   Output :
*
*   InOut  : none
*
** Return  : none
*
** Synopsis: This function sets Qty decimal position.
*===========================================================================
*/
VOID  PrtSetQtyDecimal(SHORT *psDecPoint, LONG  *plQty)
{
    *psDecPoint = PRT_NODECIMAL;

    if ((*plQty % PLU_BASE_UNIT) == 0) {

        *plQty = *plQty / PLU_BASE_UNIT;

    } else if ((*plQty % 100) == 0) {

        *psDecPoint = PRT_1DECIMAL;
        *plQty = *plQty / 100;

    } else if ((*plQty % 10) == 0) {

        *psDecPoint = PRT_2DECIMAL;
        *plQty = *plQty / 10;

    } else {

        *psDecPoint = PRT_3DECIMAL;

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
VOID PrtAdjustForeign( TCHAR *pszDest, DCURRENCY lAmount, USHORT usAdr, UCHAR uchMDC)
{
    static TCHAR CONST  aszPrtRJFC[] = _T("%.*l$");
	TCHAR  aszSpecMnem[PARA_SPEMNEMO_LEN + 1] = {0};
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
	RflGetSpecMnem(aszSpecMnem, usAdr);

    /* -- get string length -- */
    usStrLen = _tcslen(aszSpecMnem);

    /* -- format foreign amount -- */
    RflSPrintf( &pszDest[usStrLen], PRT_RJCOLUMN+1, aszPrtRJFC, sDecPoint, lAmount );
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
** Format  : VOID  PrtResetQtyDecimal( SHORT  *psDecPoint LONG  *lQty);
*
*   Input  : LONG lQty
*
*   Output :
*
*   InOut  : none
*
** Return  : none
*
** Synopsis: This function resets Qty decimal position.
*===========================================================================
*/
VOID  PrtResetQtyDecimal(SHORT *psDecPoint, LONG  *plQty)
{
    switch (*psDecPoint) {
    case PRT_NODECIMAL:
        *plQty = *plQty * PLU_BASE_UNIT;
        break;

    case PRT_1DECIMAL:
        *plQty = *plQty * 100;
        break;

    case PRT_2DECIMAL:
        *plQty = *plQty * 10;
        break;

    case PRT_3DECIMAL:
        break;

    default:
        break;

    }
}

/*
*===========================================================================
** Format  : SHORT PrtGetTaxMod2(UCHAR *pszDest,
*                               USHORT usDestLen,
*                               UCHAR  uchMinorClass);
*
*   Input  : USHORT usDestLen       -destination buffer length
*            USHORT fsTax,          -US or Canadian Tax
*            UCHAR  fbMod           -modifier status
*
*   Output : UCHAR *pszDest         -destination buffer address
*   InOut  : none
*
** Return  : SHORT  PRT_WITHDATA    -tax modifier status on
*                   PRT_WITHOUTDATA -all tax modifiers' statuses  are off.
*
** Synopsis: This function gets Tax Modifier mnemonics for KDS, 2172
*===========================================================================
*/
USHORT  PrtGetTaxMod2(TCHAR *pszDest, USHORT usDestLen, UCHAR uchMinorClass)
{

    PARASPEMNEMO   SM;                      /* special mnemonics */

    memset(pszDest, '\0', usDestLen);
    memset(SM.aszMnemonics, '\0', sizeof(SM.aszMnemonics));

    SM.uchMajorClass = CLASS_PARASPECIALMNEMO;

    switch ( uchMinorClass ) {
    case CLASS_UITAXMODIFIER1:
        SM.uchAddress = SPC_TXMOD1_ADR;
        break;

    case CLASS_UITAXMODIFIER2:
        SM.uchAddress = SPC_TXMOD2_ADR;
        break;

    case CLASS_UITAXMODIFIER3:
        SM.uchAddress = SPC_TXMOD3_ADR;
        break;

    case CLASS_UITAXMODIFIER4:
        SM.uchAddress = SPC_TXMOD4_ADR;
        break;

    case CLASS_UITAXMODIFIER5:
        SM.uchAddress = SPC_TXMOD5_ADR;
        break;

    case CLASS_UITAXMODIFIER6:
        SM.uchAddress = SPC_TXMOD6_ADR;
        break;

    case CLASS_UITAXMODIFIER7:
        SM.uchAddress = SPC_TXMOD7_ADR;
        break;

    case CLASS_UITAXMODIFIER8:
        SM.uchAddress = SPC_TXMOD8_ADR;
        break;

    case CLASS_UITAXMODIFIER9:
        SM.uchAddress = SPC_TXMOD9_ADR;
        break;

    case CLASS_UITAXEXEMPT1:          /* tax exempt */
        SM.uchAddress = SPC_TXMOD10_ADR;
        break;

    case CLASS_UITAXEXEMPT2:
        SM.uchAddress = SPC_TXMOD11_ADR;
        break;

    default:
        return (PRT_WITHOUTDATA);

    }

    /* -- get tax  modifier mnemonics -- */
    CliParaRead( &SM );

    if (PARA_SPEMNEMO_LEN < usDestLen) {
        _tcsncpy(pszDest, SM.aszMnemonics, PARA_SPEMNEMO_LEN + 1);
    } else {
        return (PRT_WITHOUTDATA);
    }

    return (PRT_WITHDATA);

}

/*
*===========================================================================
** Format  : VOID   PrtGetSISymSales( UCHAR  *aszSISym, ITEMSALES *pItem )
*
*   Input  : UCHAR  *puchStatus         -
*   Output : UCHAR  *aszSISym           -mnemonic address
*   InOut  : none
** Return  : none
*
** Synopsis: This function gets SI symbol for Sales 21RFC05437
*===========================================================================
*/
SHORT    PrtGetSISymSales( TCHAR  *aszSISym, ITEMSALES *pItem )
{
	switch(PrtCheckTaxSystem()) {
	case PRT_TAX_US:
		return PrtGetSISymSalesUS(aszSISym, pItem);
		break;

	case PRT_TAX_INTL:
		return PrtGetSISymSalesIntl(aszSISym, pItem);
		break;

	default:
		return PrtGetSISymSalesCanada(aszSISym, pItem);
		break;
	}

	return 0;
}

/*
*===========================================================================
** Format  : VOID   PrtGetSISymSale( UCHAR  *aszSISym, ITEMSALES *pItem )
*
*   Input  : UCHAR  *puchStatus         -
*   Output : UCHAR  *aszSISym           -mnemonic address
*   InOut  : none
** Return  : none
*
** Synopsis: This function gets SI symbol for Sales US, 21RFC05437
*===========================================================================
*/
SHORT    PrtGetSISymSalesUS( TCHAR  *aszSISym, ITEMSALES *pItem )
{
    UCHAR   uchWorkMDC, uchWorkBit, uchTaxState, fchFood;
    USHORT  uchWorkMod;
    SHORT   i;
    UCHAR   uchOffset1 = 0, uchOffset2 = 0;

	/* US Tax */

    uchWorkMDC = pItem->ControlCode.auchPluStatus[1];
    uchWorkMod = pItem->fbModifier;
    uchWorkMod >>= 1;
    uchWorkBit = 0x01;
	uchTaxState = 0;
    for (i = 0; i < 3; i++, uchWorkBit <<= 1) {
        uchTaxState |= ((uchWorkMDC ^ uchWorkMod) & uchWorkBit);
	}
    fchFood = pItem->ControlCode.auchPluStatus[1] & PLU_AFFECT_FS;


    if ((uchTaxState & PLU_AFFECT_TAX1)					/* Affect Taxable Itemizer #1 */
      && (uchTaxState & PLU_AFFECT_TAX2)          /* Affect Taxable Itemizer #2 */
      && (uchTaxState & PLU_AFFECT_TAX3 )) {      /* Affect Taxable Itemizer #3 */

        uchOffset1 = SPC_TX123_ADR;                         /* Taxable #1,2&3 */

    } else
    if ((uchTaxState & PLU_AFFECT_TAX1)     /* Affect Taxable Itemizer #1 */
        && (uchTaxState & PLU_AFFECT_TAX2)) {      /* Affect Taxable Itemizer #2 */

        uchOffset1 = SPC_TX12_ADR;                          /* Taxable #1,2 */

    } else
    if ((uchTaxState & PLU_AFFECT_TAX1)     /* Affect Taxable Itemizer #1 */
        && (uchTaxState & PLU_AFFECT_TAX3)) {      /* Affect Taxable Itemizer #3 */

        uchOffset1 = SPC_TX13_ADR;                          /* Taxable #1,3 */

    } else
    if ((uchTaxState & PLU_AFFECT_TAX2)     /* Affect Taxable Itemizer #2 */
        && (uchTaxState & PLU_AFFECT_TAX3 )) {      /* Affect Taxable Itemizer #3 */

        uchOffset1 = SPC_TX23_ADR;                          /* Taxable #2,3 */

    } else
    if ( uchTaxState & PLU_AFFECT_TAX1 ) { /* Affect Taxable Itemizer #1 */

        uchOffset1 = SPC_TX1_ADR;                           /* Taxable #1 */

    } else
    if ( uchTaxState & PLU_AFFECT_TAX2 ) { /* Affect Taxable Itemizer #2 */

        uchOffset1 = SPC_TX2_ADR;                           /* Taxable #2 */

    } else
    if ( uchTaxState & PLU_AFFECT_TAX3 ) { /* Affect Taxable Itemizer #3 */

        uchOffset1 = SPC_TX3_ADR;                           /* Taxable #3 */
    }

	if (fchFood & PLU_AFFECT_FS) {

        uchOffset2 = SPC_FS_ADR;                            /* F.S. */
	}

	return PrtGetSISym(aszSISym, uchOffset1, uchOffset2);

}

/*
*===========================================================================
** Format  : VOID   PrtGetSISymSale( UCHAR  *aszSISym, ITEMSALES *pItem )
*
*   Input  : UCHAR  *puchStatus         -
*   Output : UCHAR  *aszSISym           -mnemonic address
*   InOut  : none
** Return  : none
*
** Synopsis: This function gets SI symbol for Sales US, 21RFC05437
*===========================================================================
*/
SHORT    PrtGetSISymSalesCanada( TCHAR  *aszSISym, ITEMSALES *pItem )
{
    USHORT  uchWorkMod;
    UCHAR   uchOffset1 = 0, uchOffset2 = 0;

	/* Canadian Tax */
    uchWorkMod = pItem->fbModifier;
    uchWorkMod >>= 1;
    uchWorkMod &= PRT_MASKSTATCAN;

    if (uchWorkMod == 0) {
        uchWorkMod = pItem->ControlCode.auchPluStatus[1];
        uchWorkMod &= PRT_MASKSTATCAN;
        if (uchWorkMod >STD_PLU_ITEMIZERS_MOD) {
            uchWorkMod = STD_PLU_ITEMIZERS_MOD;
        }
        uchWorkMod++;
    }

	switch(uchWorkMod) {
	case CANTAX_MEAL:
	case CANTAX_CARBON:
	case CANTAX_SNACK:
        uchOffset1 = SPC_TX12_ADR;                          /* Taxable #1,2 */
		break;

	case CANTAX_BEER:
        uchOffset1 = SPC_TX13_ADR;                          /* Taxable #1,3 */
		break;

	case CANTAX_LIQUOR:
        uchOffset1 = SPC_TX14_ADR;                          /* Taxable #1,3 */
		break;

	case CANTAX_GROCERY:
        uchOffset1 = 0;
        break;

	case CANTAX_TIP:
        uchOffset1 = SPC_TX1_ADR;                           /* Taxable #1 */
		break;

	case CANTAX_PST1:
        uchOffset1 = SPC_TX2_ADR;                           /* Taxable #2 */
		break;

	case CANTAX_BAKED:
	case CANTAX_SPEC_BAKED:
        uchOffset1 = SPC_TX13_ADR;                          /* Taxable #1,3 */
		break;
	}

	return PrtGetSISym(aszSISym, uchOffset1, uchOffset2);

}

/*
*===========================================================================
** Format  : VOID   PrtGetSISymSale( UCHAR  *aszSISym, ITEMSALES *pItem )
*
*   Input  : UCHAR  *puchStatus         -
*   Output : UCHAR  *aszSISym           -mnemonic address
*   InOut  : none
** Return  : none
*
** Synopsis: This function gets SI symbol for Sales
*===========================================================================
*/
SHORT    PrtGetSISymSalesIntl( TCHAR  *aszSISym, ITEMSALES *pItem )
{
    UCHAR   uchWorkMDC;
    UCHAR   uchOffset1 = 0, uchOffset2 = 0;

	/* Intl VAT */

    uchWorkMDC = pItem->ControlCode.auchPluStatus[1];
    uchWorkMDC &= 0x03;

    if (uchWorkMDC == PLU_AFFECT_TAX1) {
        uchOffset1 = SPC_TX1_ADR;                           /* VAT #1 */
    } else if ( uchWorkMDC == PLU_AFFECT_TAX2 ) {
        uchOffset1 = SPC_TX2_ADR;                           /* VAT #2 */
    } else if ( uchWorkMDC == (PLU_AFFECT_TAX1 | PLU_AFFECT_TAX2) ) {
        uchOffset1 = SPC_TX3_ADR;                           /* VAT #3 */

    }

	return PrtGetSISym(aszSISym, uchOffset1, uchOffset2);
}

/*
*===========================================================================
** Format  : VOID   PrtGetSISymDisc( UCHAR  *aszSISym, ITEMDISC *pItem )
*
*   Input  : UCHAR  *puchStatus         -
*   Output : UCHAR  *aszSISym           -mnemonic address
*   InOut  : none
** Return  : none
*
** Synopsis: This function gets SI symbol for Discount 21RFC05437
*===========================================================================
*/
SHORT    PrtGetSISymDisc( TCHAR  *aszSISym, ITEMDISC *pItem )
{
	if (pItem->uchMinorClass == CLASS_ITEMDISC1) {

	    if (((fsPrtStatus & PRT_REQKITCHEN) == 0) &&	/* not KP, KDS */
            !(pItem->fsPrintStatus & (PRT_VALIDATION | PRT_SINGLE_RECPT | PRT_DOUBLE_RECPT))) {  /* not validation, tichet */

			switch(PrtCheckTaxSystem()) {
			case PRT_TAX_US:
				return PrtGetSISymItemDiscUS(aszSISym, pItem);
				break;

			case PRT_TAX_INTL:
				return PrtGetSISymItemDiscIntl(aszSISym, pItem);
				break;

			default:
				return PrtGetSISymItemDiscCanada(aszSISym, pItem);
				break;
			}
		}

	}
	/* not set si symbol at reg. disc, charge tips, like gp */

	return 0;
}

/*
*===========================================================================
** Format  : VOID   PrtGetSISymDisc( UCHAR  *aszSISym, ITEMDISC *pItem )
*
*   Input  : UCHAR  *puchStatus         -
*   Output : UCHAR  *aszSISym           -mnemonic address
*   InOut  : none
** Return  : none
*
** Synopsis: This function gets SI symbol for Discount US, 21RFC05437
*===========================================================================
*/
SHORT    PrtGetSISymItemDiscUS( TCHAR  *aszSISym, ITEMDISC *pItem )
{

    UCHAR   uchWorkMDC, uchWorkBit, fchFood, uchTaxState;
    SHORT   i;
    UCHAR   uchOffset1 = 0, uchOffset2 = 0;

    uchWorkMDC = pItem->auchDiscStatus[0];
    uchWorkBit = 0x04;
    fchFood = pItem->auchDiscStatus[1] & 0x04;
    fchFood <<= 1;

	uchTaxState = 0;
    for (i = 0; i < 3; i++, uchWorkBit <<= 1) {
        uchTaxState |= (uchWorkMDC & uchWorkBit);
	}
	uchTaxState = (UCHAR)(uchTaxState >> 2);

    if ((uchTaxState & PLU_AFFECT_TAX1)					/* Affect Taxable Itemizer #1 */
      && (uchTaxState & PLU_AFFECT_TAX2)          /* Affect Taxable Itemizer #2 */
      && (uchTaxState & PLU_AFFECT_TAX3 )) {      /* Affect Taxable Itemizer #3 */

        uchOffset1 = SPC_TX123_ADR;                         /* Taxable #1,2&3 */

    } else
    if ((uchTaxState & PLU_AFFECT_TAX1)     /* Affect Taxable Itemizer #1 */
        && (uchTaxState & PLU_AFFECT_TAX2)) {      /* Affect Taxable Itemizer #2 */

        uchOffset1 = SPC_TX12_ADR;                          /* Taxable #1,2 */

    } else
    if ((uchTaxState & PLU_AFFECT_TAX1)     /* Affect Taxable Itemizer #1 */
        && (uchTaxState & PLU_AFFECT_TAX3)) {      /* Affect Taxable Itemizer #3 */

        uchOffset1 = SPC_TX13_ADR;                          /* Taxable #1,3 */

    } else
    if ((uchTaxState & PLU_AFFECT_TAX2)     /* Affect Taxable Itemizer #2 */
        && (uchTaxState & PLU_AFFECT_TAX3 )) {      /* Affect Taxable Itemizer #3 */

        uchOffset1 = SPC_TX23_ADR;                          /* Taxable #2,3 */

    } else
    if ( uchTaxState & PLU_AFFECT_TAX1 ) { /* Affect Taxable Itemizer #1 */

        uchOffset1 = SPC_TX1_ADR;                           /* Taxable #1 */

    } else
    if ( uchTaxState & PLU_AFFECT_TAX2 ) { /* Affect Taxable Itemizer #2 */

        uchOffset1 = SPC_TX2_ADR;                           /* Taxable #2 */

    } else
    if ( uchTaxState & PLU_AFFECT_TAX3 ) { /* Affect Taxable Itemizer #3 */

        uchOffset1 = SPC_TX3_ADR;                           /* Taxable #3 */
    }

	if (fchFood & PLU_AFFECT_FS) {

        uchOffset2 = SPC_FS_ADR;                            /* F.S. */
	}

	return PrtGetSISym(aszSISym, uchOffset1, uchOffset2);

}

/*
*===========================================================================
** Format  : VOID   PrtGetSISymDisc( UCHAR  *aszSISym, ITEMDISC *pItem )
*
*   Input  : UCHAR  *puchStatus         -
*   Output : UCHAR  *aszSISym           -mnemonic address
*   InOut  : none
** Return  : none
*
** Synopsis: This function gets SI symbol for Discount US, 21RFC05437
*===========================================================================
*/
SHORT    PrtGetSISymItemDiscCanada( TCHAR  *aszSISym, ITEMDISC *pItem )
{
    UCHAR   uchWorkMDC;
    UCHAR   uchOffset1 = 0, uchOffset2 = 0;

    uchWorkMDC = pItem->auchDiscStatus[2];
    uchWorkMDC >>= 4;
    uchWorkMDC &= PRT_MASKSTATCAN;
    uchWorkMDC++;

	switch(uchWorkMDC) {
	case CANTAX_MEAL:
	case CANTAX_CARBON:
	case CANTAX_SNACK:
        uchOffset1 = SPC_TX12_ADR;                          /* Taxable #1,2 */
		break;

	case CANTAX_BEER:
        uchOffset1 = SPC_TX13_ADR;                          /* Taxable #1,3 */
		break;

	case CANTAX_LIQUOR:
        uchOffset1 = SPC_TX14_ADR;                          /* Taxable #1,3 */
		break;

	case CANTAX_GROCERY:
        uchOffset1 = 0;
        break;

	case CANTAX_TIP:
        uchOffset1 = SPC_TX1_ADR;                           /* Taxable #1 */
		break;

	case CANTAX_PST1:
        uchOffset1 = SPC_TX2_ADR;                           /* Taxable #2 */
		break;

	case CANTAX_BAKED:
	case CANTAX_SPEC_BAKED:
        uchOffset1 = SPC_TX13_ADR;                          /* Taxable #1,3 */
		break;
	}

	return PrtGetSISym(aszSISym, uchOffset1, uchOffset2);

}

/*
*===========================================================================
** Format  : VOID   PrtGetSISymDisc( UCHAR  *aszSISym, ITEMDISC *pItem )
*
*   Input  : UCHAR  *puchStatus         -
*   Output : UCHAR  *aszSISym           -mnemonic address
*   InOut  : none
** Return  : none
*
** Synopsis: This function gets SI symbol for Discount US, 21RFC05437
*===========================================================================
*/
SHORT    PrtGetSISymItemDiscIntl( TCHAR  *aszSISym, ITEMDISC *pItem )
{

    UCHAR   uchWorkMDC;
    UCHAR   uchOffset1 = 0, uchOffset2 = 0;

    uchWorkMDC = pItem->auchDiscStatus[0];
    uchWorkMDC >>= 2;
    uchWorkMDC &= 0x03;

    if (uchWorkMDC == PLU_AFFECT_TAX1) {

        uchOffset1 = SPC_TX1_ADR;                           /* VAT #1 */

    } else
    if ( uchWorkMDC == PLU_AFFECT_TAX2 ) {

        uchOffset1 = SPC_TX2_ADR;                           /* VAT #2 */

    } else
    if ( uchWorkMDC == (PLU_AFFECT_TAX1|PLU_AFFECT_TAX2) ) {

        uchOffset1 = SPC_TX3_ADR;                           /* VAT #3 */

    }

	return PrtGetSISym(aszSISym, uchOffset1, uchOffset2);
}

/*
*===========================================================================
** Format  : VOID   PrtGetSISymCoupon( UCHAR  *aszSISym, ITEMSALES *pItem )
*
*   Input  : UCHAR  *puchStatus         -
*   Output : UCHAR  *aszSISym           -mnemonic address
*   InOut  : none
** Return  : none
*
** Synopsis: This function gets SI symbol for Coupon 21RFC05437
*===========================================================================
*/
SHORT    PrtGetSISymCoupon( TCHAR  *aszSISym, ITEMCOUPON *pItem )
{
    if (((fsPrtStatus & PRT_REQKITCHEN) == 0) &&	/* not KP, KDS */
        !(pItem->fsPrintStatus & (PRT_VALIDATION | PRT_SINGLE_RECPT | PRT_DOUBLE_RECPT))) {  /* not validation, tichet */

		switch(PrtCheckTaxSystem()) {
		case PRT_TAX_US:
			return PrtGetSISymCouponUS(aszSISym, pItem);
			break;

		case PRT_TAX_INTL:
			return PrtGetSISymCouponIntl(aszSISym, pItem);
			break;

		default:
			return PrtGetSISymCouponCanada(aszSISym, pItem);
			break;
		}
	}
	return 0;
}

/*
*===========================================================================
** Format  : VOID   PrtGetSISymCoupon( UCHAR  *aszSISym, ITEMCOUPON *pItem )
*
*   Input  : UCHAR  *puchStatus         -
*   Output : UCHAR  *aszSISym           -mnemonic address
*   InOut  : none
** Return  : none
*
** Synopsis: This function gets SI symbol for Coupon US, 21RFC05437
*===========================================================================
*/
SHORT    PrtGetSISymCouponUS( TCHAR  *aszSISym, ITEMCOUPON *pItem )
{
    UCHAR   uchCheckBit, uchTaxableSts, uchTaxState, fchFood;
    USHORT  uchModifier;
    SHORT   i;
    UCHAR   uchOffset1 = 0, uchOffset2 = 0;

	/* US Tax */
    uchCheckBit    = TAX_MODIFIER1;
    uchModifier    = pItem->fbModifier;
    uchTaxableSts  = pItem->fbStatus[ 0 ];
	uchTaxState    = 0;

    for ( i = 0; i < 3; i++, uchCheckBit <<= 1 ){
        uchTaxState |= (( uchTaxableSts ^ uchModifier ) & uchCheckBit );
    }
    uchTaxState >>= 1;

    fchFood = pItem->fbStatus[0];

    if ((uchTaxState & PLU_AFFECT_TAX1)					/* Affect Taxable Itemizer #1 */
      && (uchTaxState & PLU_AFFECT_TAX2)          /* Affect Taxable Itemizer #2 */
      && (uchTaxState & PLU_AFFECT_TAX3 )) {      /* Affect Taxable Itemizer #3 */

        uchOffset1 = SPC_TX123_ADR;                         /* Taxable #1,2&3 */

    } else
    if ((uchTaxState & PLU_AFFECT_TAX1)     /* Affect Taxable Itemizer #1 */
        && (uchTaxState & PLU_AFFECT_TAX2)) {      /* Affect Taxable Itemizer #2 */

        uchOffset1 = SPC_TX12_ADR;                          /* Taxable #1,2 */

    } else
    if ((uchTaxState & PLU_AFFECT_TAX1)     /* Affect Taxable Itemizer #1 */
        && (uchTaxState & PLU_AFFECT_TAX3)) {      /* Affect Taxable Itemizer #3 */

        uchOffset1 = SPC_TX13_ADR;                          /* Taxable #1,3 */

    } else
    if ((uchTaxState & PLU_AFFECT_TAX2)     /* Affect Taxable Itemizer #2 */
        && (uchTaxState & PLU_AFFECT_TAX3 )) {      /* Affect Taxable Itemizer #3 */

        uchOffset1 = SPC_TX23_ADR;                          /* Taxable #2,3 */

    } else
    if ( uchTaxState & PLU_AFFECT_TAX1 ) { /* Affect Taxable Itemizer #1 */

        uchOffset1 = SPC_TX1_ADR;                           /* Taxable #1 */

    } else
    if ( uchTaxState & PLU_AFFECT_TAX2 ) { /* Affect Taxable Itemizer #2 */

        uchOffset1 = SPC_TX2_ADR;                           /* Taxable #2 */

    } else
    if ( uchTaxState & PLU_AFFECT_TAX3 ) { /* Affect Taxable Itemizer #3 */

        uchOffset1 = SPC_TX3_ADR;                           /* Taxable #3 */
    }

	if (fchFood & FOOD_MODIFIER) {

        uchOffset2 = SPC_FS_ADR;                            /* F.S. */
	}

	return PrtGetSISym(aszSISym, uchOffset1, uchOffset2);

}

/*
*===========================================================================
** Format  : VOID   PrtGetSISymCoupon( UCHAR  *aszSISym, ITEMCOUPON *pItem )
*
*   Input  : UCHAR  *puchStatus         -
*   Output : UCHAR  *aszSISym           -mnemonic address
*   InOut  : none
** Return  : none
*
** Synopsis: This function gets SI symbol for Coupon US, 21RFC05437
*===========================================================================
*/
SHORT    PrtGetSISymCouponCanada( TCHAR  *aszSISym, ITEMCOUPON *pItem )
{
    USHORT  uchWorkMDC;
    UCHAR   uchOffset1 = 0, uchOffset2 = 0;

	/* Canadian Tax */
    uchWorkMDC = pItem->fbModifier;
    uchWorkMDC >>= 1;
    uchWorkMDC &= TRN_MASKSTATCAN;

    if (uchWorkMDC == 0) {
        uchWorkMDC = pItem->fbStatus[0];
        uchWorkMDC &= PRT_MASKSTATCAN;
        if (uchWorkMDC >STD_PLU_ITEMIZERS_MOD) {
            uchWorkMDC = STD_PLU_ITEMIZERS_MOD;
        }
        uchWorkMDC++;
    }

	switch(uchWorkMDC) {
	case CANTAX_MEAL:
	case CANTAX_CARBON:
	case CANTAX_SNACK:
        uchOffset1 = SPC_TX12_ADR;                          /* Taxable #1,2 */
		break;

	case CANTAX_BEER:
        uchOffset1 = SPC_TX13_ADR;                          /* Taxable #1,3 */
		break;

	case CANTAX_LIQUOR:
        uchOffset1 = SPC_TX14_ADR;                          /* Taxable #1,3 */
		break;

	case CANTAX_GROCERY:
        uchOffset1 = 0;
        break;

	case CANTAX_TIP:
        uchOffset1 = SPC_TX1_ADR;                           /* Taxable #1 */
		break;

	case CANTAX_PST1:
        uchOffset1 = SPC_TX2_ADR;                           /* Taxable #2 */
		break;

	case CANTAX_BAKED:
	case CANTAX_SPEC_BAKED:
        uchOffset1 = SPC_TX13_ADR;                          /* Taxable #1,3 */
		break;
	}

	return PrtGetSISym(aszSISym, uchOffset1, uchOffset2);

}

/*
*===========================================================================
** Format  : VOID   PrtGetSISymCoupon( UCHAR  *aszSISym, ITEMCOUPON *pItem )
*
*   Input  : UCHAR  *puchStatus         -
*   Output : UCHAR  *aszSISym           -mnemonic address
*   InOut  : none
** Return  : none
*
** Synopsis: This function gets SI symbol for Coupon
*===========================================================================
*/
SHORT    PrtGetSISymCouponIntl( TCHAR  *aszSISym, ITEMCOUPON *pItem )
{
    UCHAR   uchWorkMDC;
    UCHAR   uchOffset1 = 0, uchOffset2 = 0;

	/* Intl VAT */

    uchWorkMDC = pItem->fbStatus[0];
    uchWorkMDC >>= 1;
    uchWorkMDC &= 0x03;

    if (uchWorkMDC == PLU_AFFECT_TAX1) {

        uchOffset1 = SPC_TX1_ADR;                           /* VAT #1 */

    } else
    if ( uchWorkMDC == PLU_AFFECT_TAX2 ) {

        uchOffset1 = SPC_TX2_ADR;                           /* VAT #2 */

    } else
    if ( uchWorkMDC == (PLU_AFFECT_TAX1|PLU_AFFECT_TAX2) ) {

        uchOffset1 = SPC_TX3_ADR;                           /* VAT #3 */

    }

	return PrtGetSISym(aszSISym, uchOffset1, uchOffset2);
}

/*
*===========================================================================
** Format  : VOID   PrtGetSISymSale( UCHAR  *aszSISym, ITEMSALES *pItem )
*
*   Input  : UCHAR  *puchStatus         -
*   Output : UCHAR  *aszSISym           -mnemonic address
*   InOut  : none
** Return  : none
*
** Synopsis: This function gets SI symbol for Sales
*===========================================================================
*/
SHORT    PrtGetSISym( TCHAR  *aszSISym, UCHAR uchOffset1, UCHAR uchOffset2 )
{
    PARAFLEXMEM     WorkFlex;
    UCHAR i = 0;

    WorkFlex.uchMajorClass = CLASS_PARAFLEXMEM;
    WorkFlex.uchAddress = FLEX_GC_ADR;
    CliParaRead(&WorkFlex);

    if (WorkFlex.uchPTDFlag == FLEX_PRECHK_UNBUFFER) {
		/* not use si symbol, at precheck unbuffering printing */
		return 0;
	}

    if ( uchOffset1 != 0 ) {
		RflGetSpecMnem( aszSISym, uchOffset1 );
    } else {
		*aszSISym = _T(' ');	/* set space for before mnemonics position */
	}
    i++;

    if ( uchOffset2 != 0 ) {
		RflGetSpecMnem( aszSISym + i , uchOffset2 );
    } else {
		*(aszSISym + 1) = _T(' ');	/* set space for before mnemonics position */
	}
    i++;
    *( aszSISym + i ) = _T('\0');

	return (SHORT)i;
}

/*
*===========================================================================
**Synopsis: SHORT   PrtCheckTaxSystem(VOID)
*
*    Input:
*   Output: None
*    InOut:
**  Return:
** Description:
*===========================================================================
*/
SHORT   PrtCheckTaxSystem(VOID)
{
    if (CliParaMDCCheck(MDC_TAX_ADR, ODD_MDC_BIT0)) {
        if (CliParaMDCCheck(MDC_TAXINTL_ADR, ODD_MDC_BIT0)) {
            return(PRT_TAX_INTL);
        } else {
            return(PRT_TAX_CANADA);
        }
    } else {
        return(PRT_TAX_US);
    }
}


/****** End of Source ******/