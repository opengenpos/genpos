/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1998-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print Item  main                 
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrCItem_.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* May-15-92 : 00.00.01 : K.Nakajima :                                    
* Nov-24-92 : 01.00.00 : J.Ikeda    : Adds PrtSupTare()
* Nov-25-92 : 01.00.00 : K.You      : Adds PrtSupOpeStatus().                                   
* Nov-26-92 : 01.00.00 : K.You      : Unify P21/P24 for Super,Prog
* Dec-04-92 : 01.00.03 : K.Nakajima : set R/J column at CLASS_MODEIN
* Dec-10-92 : 01.00.03 : K.Nakajima : chg R/J column at CLASS_MODEIN
* Jun-21-93 : 01.00.12 : J.IKEDA    : Adds CLASS_RPTOPEWAI, CLASS RPTOPECAS
*                      :            : CLASS_RPTOPENGCF
* Jun-23-93 : 01.00.12 : J.IKEDA    : Adds CLASS_PARASHRPRT
* Jun-23-93 : 01.00.12 : K.You      : Adds PRTCONTROL PrtCompNo
* Jun-28-93 : 01.00.12 : K.You      : Adds UCHAR fbPrtLockChk
* Jun-30-93 : 01.00.12 : J.IKEDA    : Adds CLASS_PARAEMPLOYEENO, CLASS_RPTEMPLOYEE
* Jun-30-93 : 01.00.12 : R.Itoh     : Adds CLASS_ETKIN, CLASS_ETKOUT
* Jul-02-93 : 01.00.12 : K.You      : Adds CLASS_PRESETAMOUNT
* Jul-07-93 : 01.00.12 : R.Itoh     : Adds PrtDispItem() for US Enhance
* Jul-13-93 : 01.00.12 : M.Yamamoto : Add CLASS_PARAPIGRULE
* Aug-23-93 : 01.00.13 : J.IKEDA    : Adds CLASS_PARASOFTCHK, Del CLASS_PARACHAR44
* Oct-06-93 : 02.00.01 : J.IKEDA    : Adds CLASS_MAINTETKFL
* Oct-28-93 : 02.00.02 : K.You      : Del. Dfl feature.
* Nov-08-93 : 02.00.02 : K.You      : Del. CLASS_PARAPIGRULE
* Nov-12-93 : 02.00.02 : M.Yamamoto : Adds CLASS_PARAHOTELID
*                                   : Adds CLASS_PARACPMEPTTALY
* Apr-25-94 : 00.00.05 : K.You      : bug fixed E-25 (mod. PrtPrintItem)
* May-24-94 : 02.05.00 : Yoshiko.Jim: Dupli.Print for CP,EPT (add PrtPrintCpEpt())
* Jan-17-95 : 03.00.00 : M.Ozawa    : Add PrtDispItem() for display on the fly.
* Feb-27-95 : 03.00.00 : T.Nakahata : Add Coupon Feature
* Mar-01-95 : 03.00.00 : H.Ishida   : Add process of CLASS_PARAOEPTBL,CLASS_PARACPN
* Dec-18-95 : 03.01.00 : M.Ozawa    : recover CLASS_PARAPIGRULE
* Aug-04-98 : 03.03.00 : hrkato     : V3.3
* Aug-08-99 : 03.05.00 : M.Teraki   : Remove WAITER_MODEL
* Aug-16-99 : 03.05.00 : M.Teraki   : Merge STORE_RECALL_MODEL/GUEST_CHECK_MODEL
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
* Dec-02-99 : 01.00.00 : hrkato     : Saratoga
* Dec-14-99 : 00.00.01 : M.Ozawa   : Add PrtDflItemForm() 
* Aug-02-00 : V1.0.0.4 : M.Teraki   : Changed KP frame number to be managed for each KP
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
/**------- 2170 local------**/
#include	<tchar.h>
#include "ecr.h"
#include "paraequ.h"
#include "para.h"
/* #include "csstbpar.h" */
#include "regstrct.h"
#include "transact.h"
#include "maint.h"
#include "csttl.h"
#include "csop.h"
#include "report.h"
#include "prt.h"
#include "dfl.h"
#include "item.h"
#include "prtrin.h"
#include "prtdfl.h"
#include "prtsin.h"

/*
============================================================================
+                  S T A T I C   R A M s
============================================================================
*/
SHORT   fsPrtCompul;          /* compulsory portion */
SHORT   fsPrtNoPrtMask;       /* No-Print portion Mask */
SHORT   fsPrtStatus;          /* status ( request kitchen, no other, print on demand, 
                                   not initialize condition ) */
SHORT   fsPrtPrintPort;       /* print portion */
UCHAR   auchPrtFailKP[PRT_KPRT_MAX];  /* failed kitchen printer */
UCHAR   uchPrtCurKP;          /* current failed kitchen printer */

TCHAR   uchPrtSeatNo;           /* save seat no for continual slip print R3.1 */
TCHAR   auchPrtSeatCtl[2];      /* save seat no data for split check print control */


USHORT  usPrtSlipPageLine;    /* current slip lines and pages */
USHORT  usPrtRJColumn = 21;   /* receipt and journal's numbers of columns */
                              /* "21" for safety */
USHORT  usPrtVLColumn = 47;   /* validation's numbers of columns */
                              /* "47" for safety */

PRTCONTROL PrtCompNo;

UCHAR   fbPrtLockChk;          /* lock info. save area */
UCHAR   fbPrtAltrStatus;       /* printer alternation status */
UCHAR   fbPrtAbortStatus;      /* abort key entry status */

UCHAR	fbPrtSendToStatus;		/* status ( send to file, send to printer ) */ //ESMITH PRTFILE

const TCHAR aszPrtAM[] = STD_TIME_AM_SYMBOL;
const TCHAR aszPrtPM[] = STD_TIME_PM_SYMBOL;

/* -- for display on the fly -- */
PRTDFLIF    PrtDflIf;          /* display data buffer */
UCHAR    fbPrtDflErr;          /* error displayed or not */
UCHAR   uchPrtDflTypeSav;      /* save transaction type */

//now using MLD_NO_DISP_PLU_LOW and MLD_NO_DISP_PLU_HIGH (ecr.h)
//CONST TCHAR auchPrtNoPrintPLULow[]  = _T("00000000099500"); /* 2172 */
//CONST TCHAR auchPrtNoPrintPLUHigh[] = _T("00000000099999"); /* 2172 */

/**
;========================================================================
;+        P R O G R A M    D E C L A R A T I O N s                      +
;========================================================================
**/
USHORT   PrtPrintLine(USHORT usPrtControl, TCHAR *pLineToPrint)
{
    USHORT usPrtType;
	USHORT usLen;

    /* check print control */
    switch((usPrtControl & 0x0f)) {
    case PRT_RECEIPT:
        usPrtType = PMG_PRT_RECEIPT;
        break;

    case PRT_JOURNAL:
        usPrtType = PMG_PRT_JOURNAL;
        break;

    case PRT_RECEIPT | PRT_JOURNAL:
        usPrtType = PMG_PRT_RCT_JNL;
        break;

    default:
        return 0;
    }

	usLen = _tcslen (pLineToPrint);

	if (usLen < 0 || usLen > 255)
		return 0;

	PmgPrint(usPrtType, pLineToPrint, usLen);

	return 0;
}

USHORT   PrtPrintLineImmediate(USHORT usPrtControl, TCHAR *pLineToPrint)
{
    USHORT usPrtType;
	USHORT usLen;

    /* check print control */
    switch((usPrtControl & 0x0f)) {
    case PRT_RECEIPT:
        usPrtType = PMG_PRT_RECEIPT;
        break;

    case PRT_JOURNAL:
        usPrtType = PMG_PRT_JOURNAL;
        break;

    case PRT_RECEIPT | PRT_JOURNAL:
        usPrtType = PMG_PRT_RCT_JNL;
        break;

    default:
        return 0;
    }

	usLen = _tcslen (pLineToPrint);

	if (usLen < 0 || usLen > 255)
		return 0;

	usPrtType |= PMG_PRT_IMMEDIATE;
	PmgPrint(usPrtType, pLineToPrint, usLen);

	return 0;
}

/*
*===========================================================================
** Format  : USHORT   PrtPrintItem(TRANINFORMATION *pTran, VOID *pItem);      
*               
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*            VOID             *pItem     -Item Data address
*   Output : none
*    InOut : none
** Return  : USHORT  usPrtSlipPageLine   -current printed pages and lines       
*
** Synopsis: This function is called from application user and print item.
*===========================================================================
*/
USHORT   PrtPrintItem(TRANINFORMATION  *pTran, VOID *pItem)
{

    switch ( ((PRTITEMDATA *)pItem)->uchMajorClass ) {

/*-------------------*\
  R E G    M O D E 
\*-------------------*/

    case CLASS_ITEMMODEIN:              /* set R/J Col. to "usPrtRJColumn" */
        PrtSetRJColumn();
        break;

    case CLASS_ITEMPRINT:               /* trailer , header , ticket */
        PrtItemPrint(pTran, (ITEMPRINT *)pItem);
        break;

    case CLASS_ITEMSALES:               /* plu, dept, setmenu, mod.discount */
        PrtItemSales(pTran, (ITEMSALES *)pItem);
        break;

    case CLASS_ITEMDISC:                /* discount */
        PrtItemDisc(pTran, (ITEMDISC *)pItem);
        break;

    case CLASS_ITEMCOUPON:              /* coupon */
        PrtItemCoupon( pTran, ( ITEMCOUPON * )pItem );
        break;

    case CLASS_ITEMMISC:                /* nosale, ROA, paid out... */
        PrtItemMisc(pTran, (ITEMMISC *)pItem);
        break;

    case CLASS_ITEMTENDER:              /* tender */
    case CLASS_ITEMTENDEREC:            /* EC tender for charge post */
        PrtItemTender(pTran, (ITEMTENDER *)pItem);
        break;

    case CLASS_ITEMAFFECTION:           /* tax */
        PrtItemAffection(pTran, (ITEMAFFECTION *)pItem);
        break;

    case CLASS_ITEMTRANSOPEN:           /* newcheck , reorder */
        PrtItemTransOpen(pTran, (ITEMTRANSOPEN *)pItem);
        break;

    case CLASS_ITEMTOTAL:               /* total */
        PrtItemTotal(pTran, (ITEMTOTAL *)pItem);
        fsPrtStatus |= PRT_REC_SLIP;    /* for grand total print */
        break;

    case CLASS_ITEMOTHER:               /* feed */
        PrtItemOther((ITEMOTHER *)pItem);
        break;

    case CLASS_ITEMMULTI:               /* check paid */
        PrtItemMulti(pTran, (ITEMMULTI *)pItem);
        break;

    case CLASS_ITEMMODIFIER:            /* modifier */
        PrtItemModifier(pTran, (ITEMMODIFIER *)pItem);
        break;

/*----------------------------------*\
  S u p e r & P r o g r a m  M O D E 
\*----------------------------------*/
    
    case CLASS_PARAPROMOTION:    
    case CLASS_PARATRANSMNEMO:    
    case CLASS_PARALEADTHRU:    
    case CLASS_PARAREPORTNAME:    
    case CLASS_PARASPECIALMNEMO:    
    case CLASS_PARAADJMNEMO:    
    case CLASS_PARAPRTMODI:    
    case CLASS_PARAMAJORDEPT:    
    case CLASS_PARACHAR24:
    case CLASS_PARAPCIF:
    case CLASS_PARASOFTCHK:
    case CLASS_PARAHOTELID:
    case CLASS_PARAMLDMNEMO:
        PrtSupMnemData(pItem);
        break;

    case CLASS_PARADISCTBL:
    case CLASS_PARASECURITYNO:
    case CLASS_PARATRANSHALO:
    case CLASS_PARASLIPFEEDCTL:
    case CLASS_PARACTLTBLMENU:
    case CLASS_PARAAUTOALTKITCH:
    case CLASS_PARAMANUALTKITCH:
    case CLASS_PARATTLKEYTYP:
    case CLASS_PARATEND:                                /* V3.3 */
    case CLASS_PARACASHABASSIGN:
    case CLASS_PARATONECTL:
    case CLASS_PARASHRPRT:
    case CLASS_PARASERVICETIME:
    case CLASS_PARALABORCOST:
        PrtSupComData(pItem);
        break;

    case CLASS_PARAMDC:
    case CLASS_PARAACTCODESEC:
        PrtSupMDC(pItem);
        break;
    
    case CLASS_PARADEPT:
        PrtSupDEPT(( PARADEPT *)pItem);
        break;

    case CLASS_PARAPLU:
        if (usPrtRJColumn == 24) {              /* 24 Column Printer */
            PrtSupPLU(( PARAPLU *)pItem);
        } else {                                /* 21 Column Printer */
            PrtSupPLU_s(( PARAPLU *)pItem);
        }
        break;

    case CLASS_PARACPN:                         /* Combination Coupon */
        PrtSupCPN(( PARACPN *)pItem);
        break;

    case CLASS_PARAPLUNOMENU:
        PrtSupPLUNoMenu(( PARAPLUNOMENU *)pItem);
        break;  

    case CLASS_PARAROUNDTBL:
        PrtSupRound(( PARAROUNDTBL *)pItem);
        break;

    case CLASS_PARAFSC:
        PrtSupFSC(( PARAFSC *)pItem);
        break;

    case CLASS_PARAHOURLYTIME:
        PrtSupHourly(( PARAHOURLYTIME *)pItem);
        break;

    case CLASS_PARAFLEXMEM:
        PrtSupFlexMem(( PARAFLEXMEM *)pItem);
        break;

    case CLASS_PARASTOREGNO:
        PrtSupStoRegNo(( PARASTOREGNO *)pItem);
        break;

    case CLASS_PARASUPLEVEL:
        PrtSupSupLevel(( PARASUPLEVEL *)pItem);
        break;

    case CLASS_PARACURRENCYTBL:
        PrtSupCurrency(( PARACURRENCYTBL *)pItem);
        break;

    case CLASS_PARATAXRATETBL:
        PrtSupTaxRate(( PARATAXRATETBL *)pItem);
        break;

    case CLASS_PARATTLKEYCTL:
        PrtSupTtlKeyCtl(( PARATTLKEYCTL *)pItem);
        break;

    case CLASS_PARACASHIERNO:
        PrtSupCashierNo(( MAINTCASHIERIF *)pItem);
        break;

    case CLASS_MAINTTAXTBL1:
    case CLASS_MAINTTAXTBL2:
    case CLASS_MAINTTAXTBL3:
    case CLASS_MAINTTAXTBL4:
        PrtSupTaxTbl(( MAINTTAXTBL *)pItem);
        break;

    case CLASS_PARAMENUPLUTBL:
        if (usPrtRJColumn == 24) {              /* 24 Column Printer */
            PrtSupMenuPLU(( MAINTMENUPLUTBL *)pItem);
        } else {                                /* 21 Column Printer */
            PrtSupMenuPLU_s(( MAINTMENUPLUTBL *)pItem);
        }
        break;

    case CLASS_PARAOEPTBL:
        PrtSupOepData(( PARAOEPTBL *)pItem);
        break;

    case CLASS_MAINTCSTR:
        PrtSupCstrData(( MAINTCSTR *)pItem);
        break;

    case CLASS_PARAFXDRIVE:
        PrtSupFxDrive(( PARAFXDRIVE *)pItem);
        break;

    case CLASS_MAINTHEADER:
        PrtSupHeader(( MAINTHEADER *)pItem);
        break;

    case CLASS_MAINTTRAILER:
        PrtSupTrailer(( MAINTTRAILER *)pItem);
        break;

    case CLASS_MAINTTRANS:
        PrtSupTrans(( MAINTTRANS *)pItem);
        break;

    case CLASS_MAINTERRORCODE:
        PrtSupErrorCode(( MAINTERRORCODE *)pItem);
        break;

    case CLASS_RPTREGFIN:
        PrtSupRegFin(( RPTREGFIN *)pItem);
        break;

    case CLASS_RPTHOURLY:
        PrtSupHourlyAct(( RPTHOURLY *)pItem);
        break;

    case CLASS_RPTDEPT:
        if (usPrtRJColumn == 24) {              /* 24 Column Printer */
            PrtSupDEPTFile(( RPTDEPT *)pItem);
        } else {                                /* 21 Column Printer */
            PrtSupDEPTFile_s(( RPTDEPT *)pItem);
        }
        break;

    case CLASS_RPTPLU:
        if (usPrtRJColumn == 24) {              /* 24 Column Printer */
            PrtSupPLUFile(( RPTPLU *)pItem);
        } else {                                /* 21 Column Printer */
            PrtSupPLUFile_s(( RPTPLU *)pItem);
        }
        break;

    case CLASS_RPTCASHIER:
    case CLASS_RPTWAITER:
    case CLASS_RPTEMPLOYEENO:
        PrtSupCashWaitFile(pItem);
        break;

    case CLASS_RPTOPENCAS:
    case CLASS_RPTOPENWAI:
        PrtSupWFClose(pItem);
        break;

    case CLASS_RPTGUEST:
        if (usPrtRJColumn == 24) {              /* 24 Column Printer */
            PrtSupGCF(( RPTGUEST *)pItem);
        } else {                                /* 21 Column Printer */
            PrtSupGCF_s(( RPTGUEST *)pItem);
        }
        break;

    case CLASS_RPTOPENGCF:
        PrtSupGCFClose(( RPTGUEST *)pItem);
        break;

    case CLASS_PARAGUESTNO:
        PrtSupGCNo(( PARAGUESTNO *)pItem);
        break;

    case CLASS_MAINTFEED:
        PrtSupRJFeed(( MAINTFEED *)pItem);
        break;

    case CLASS_MAINTBCAS:
        PrtSupBcas(( MAINTBCAS *)pItem);
        break;

    case CLASS_MAINTOPESTATUS:
        PrtSupOpeStatus(( MAINTOPESTATUS *)pItem);
        break;

    case CLASS_PARATARE:
        PrtSupTare(( PARATARE *)pItem);
        break; 

    case CLASS_PARAPRESETAMOUNT:
        PrtSupPresetAmount(( PARAPRESETAMOUNT *)pItem);
        break; 

    case CLASS_PARAEMPLOYEENO:
        PrtSupEmployeeNo(( PARAEMPLOYEENO *)pItem);
        break;

    case CLASS_PARACPMEPTTALLY:
        PrtSupTally( ( PARACPMTALLY *) pItem);
        break;

    case CLASS_MAINTCPM:                        /* CPM START STOP */
        PrtSupCpm( ( MAINTCPM *) pItem);
        break;

    case CLASS_RPTJCSUMMARYTITLES:
    case CLASS_RPTJCSUMMARY:
    case CLASS_RPTJCSUMMARYTOTALS:
        if (usPrtRJColumn == 24) {              /* 24 Column Printer */
            PrtSupETKFileJCSummary ( (PRPTJCSUMMARY) pItem );
        } else {                                /* 21 Column Printer */
            PrtSupETKFileJCSummary ( (PRPTJCSUMMARY) pItem );
        }
        break;

    case CLASS_RPTEMPLOYEE:
        if (usPrtRJColumn == 24) {              /* 24 Column Printer */
            PrtSupETKFile(( RPTEMPLOYEE *)pItem);
        } else {                                /* 21 Column Printer */
            PrtSupETKFile_s(( RPTEMPLOYEE *)pItem);
        }
        break;

    case CLASS_MAINTETKFL:
        if (usPrtRJColumn == 24) {              /* 24 Column Printer */
            PrtSupMaintETKFl(( MAINTETKFL *)pItem);
        } else {                                /* 21 Column Printer */
            PrtSupMaintETKFl_s(( MAINTETKFL *)pItem);
        }
        break;

    case CLASS_RPTCPN:
        if (usPrtRJColumn == 24) {              /* 24 Column Printer */
            PrtSupCPNFile(( RPTCPN *)pItem);
        } else {                                /* 21 Column Printer */
            PrtSupCPNFile_s(( RPTCPN *)pItem);
        }
        break;

    case CLASS_PARAPPI:
        PrtSupPPI( ( PARAPPI *) pItem);
        break;

    case CLASS_RPTSERVICE:
        PrtSupServiceTime(( RPTSERVICE *)pItem);
        break;

    case CLASS_PARAPIGRULE:
        PrtSupPigRule(pItem);
        break;

    case CLASS_PARAMISCPARA:
        PrtSupMiscPara(pItem);
        break;

    case CLASS_PARADISPPARA:
        PrtSupDispPara(( PARADISPPARA *)pItem);
        break;

    case CLASS_PARADEPTNOMENU: /* 2172 */
        PrtSupDeptNoMenu(( PARADEPTNOMENU *)pItem);
        break;  

    case CLASS_MAINTTOD:               /* tod report 2172 */
        PrtSupTOD(( PARATOD *)pItem);
        break;

    case CLASS_PARAKDSIP: /* 2172 */
    case CLASS_MAINTHOSTSIP:
        PrtSupKdsIp(( PARAKDSIP *)pItem);
        break;  

    case CLASS_PARARESTRICTION:                 /* plu sales restriction table 2172 */
        PrtSupRest(( PARARESTRICTION *)pItem);
        break;

    case CLASS_PARABOUNDAGE:                    /* boundary age 2172 */
        PrtSupAge( ( PARABOUNDAGE *) pItem);
        break;

    case CLASS_MAINTSPHEADER:                   /* slip header for super. mode */
    case CLASS_MAINTSPTRAILER:                  /* slip trailer for super. mode */
        break;

    case CLASS_MAINTLOAN:                       /* loan,    Saratoga */
    case CLASS_MAINTPICKUP:                     /* pick up */
        PrtSupLoanPickup(pTran, (MAINTLOANPICKUP *)pItem);
        break;

    default:
        break;

    }

    return (usPrtSlipPageLine);         /* return current slip line & page */
}

/*
*===========================================================================
** Format  : USHORT   PrtPrintCpEpt(TRANINFORMATION *pTran, VOID *pItem);      
*               
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*            VOID             *pItem     -Item Data address
*   Output : none
*    InOut : none
** Return  : USHORT  usPrtSlipPageLine   -current printed pages and lines       
*
** Synopsis: This function is called in Duplicate Print for CP/EPT.
*===========================================================================
*/
USHORT   PrtPrintCpEpt(TRANINFORMATION  *pTran, VOID *pItem)
{
    USHORT  usMedia;

    switch ( ((PRTITEMDATA *)pItem)->uchMajorClass ) {
    case CLASS_ITEMPRINT:                   /* trailer , header , ticket    */
        if (((PRTITEMDATA *)pItem)->uchMinorClass != CLASS_PROMOTION) {
            PrtItemPrint(pTran, (ITEMPRINT *)pItem);
        }
        break;

    case CLASS_ITEMTENDER:                  /* tender                       */
    case CLASS_ITEMTENDEREC:                /* EC tender for charge post    */
        usMedia = PrtCheckTenderMedia((ITEMTENDER *)pItem);
        if (usMedia != PRT_TEND_OTHER) {    /* print ROOM,ACCT,CRE-AUT,EPT  */
            PrtItemTender(pTran, (ITEMTENDER *)pItem);
        }
        break;

    case CLASS_ITEMTRANSOPEN:               /* newcheck , reorder           */
        PrtItemTransOpen(pTran, (ITEMTRANSOPEN *)pItem);
        break;

    case CLASS_ITEMTOTAL:                   /* total                        */
        if (!auchPrtSeatCtl[0]) {
            fsPrtStatus |= PRT_REC_SLIP;    /* for grand total print */
        }
        PrtItemTotal(pTran, (ITEMTOTAL *)pItem);
        fsPrtStatus |= PRT_REC_SLIP;    /* for grand total print */
        break;

    case CLASS_ITEMOTHER:                   /* feed                         */
        PrtItemOther((ITEMOTHER *)pItem);
        break;

    case CLASS_ITEMMODIFIER:                /* modifier                     */
        PrtItemModifier(pTran, (ITEMMODIFIER *)pItem);
        break;

    default:
        break;

    }

    return (usPrtSlipPageLine);             /* current slip line & page     */
}

/*
*===========================================================================
** Format  : USHORT   PrtCheckTenderMedia(ITEMTENDER *pItem);      
*               
*    Input : ITEMTENDER     *pItem     -Item Data address
*   Output : none
*    InOut : none
*
** Return  : PRT_TEND_ROOM      : room charge tender
*            PRT_TEND_ACCT      : acct. charge tender
*            PRT_TEND_CREAUTH   : credit authorization tender
*            PRT_TEND_EPT       : EPT tender
*            PRT_TEND_OTHER     : other tender
*
** Synopsis: This function check and return tender media from minor class.
*===========================================================================
*/
USHORT   PrtCheckTenderMedia(ITEMTENDER *pItem)
{
    if (ItemTenderCheckTenderMdc(pItem->uchMinorClass, 3, MDC_PARAM_BIT_D)) {            /* Room charge Tender ?    */
        return(PRT_TEND_ROOM);                  /* ret Room Charge  */
    } else if (ItemTenderCheckTenderMdc(pItem->uchMinorClass, 3, MDC_PARAM_BIT_C)) {     /* Acct charge Tender ?    */
        return(PRT_TEND_ACCT);                  /* ret Acct Charge  */
    } else if (ItemTenderCheckTenderMdc(pItem->uchMinorClass, 3, MDC_PARAM_BIT_B)) {     /* Cre-Auth charge Tender? */
        return(PRT_TEND_CREAUTH);               /* ret Credit Autho */
    } else if (ItemTenderCheckTenderMdc(pItem->uchMinorClass, 3, MDC_PARAM_BIT_A)) {     /* EPT Tender ?     */
        return(PRT_TEND_EPT);                   /* ret EPT          */
    }

    return PRT_TEND_OTHER; /* ### Add (2171 for Win32) V1.0 */
}

/*
*===========================================================================
** Format  : VOID   PrtDispItem(TRANINFORMATION *pTran, VOID *pItem);      
*               
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*            VOID             *pItem     -Item Data address
*   Output : none
*    InOut : none
** Return  : USHORT  usPrtSlipPageLine   -current printed pages and lines       
*
** Synopsis: This function is called from application user and display item.
*===========================================================================
*/
VOID   PrtDispItem(TRANINFORMATION  *pTran, VOID *pItem)
{
    switch ( ((PRTITEMDATA *)pItem)->uchMajorClass ) {

    case CLASS_ITEMPRINT:               /* trailer , header , ticket */
        PrtDispPrint(pTran, (ITEMPRINT *)pItem);
        break;

    case CLASS_ITEMSALES:               /* plu, dept, setmenu, mod.discount */
        PrtDispSales(pTran, (ITEMSALES *)pItem);
        break;

    case CLASS_ITEMDISC:                /* discount */
        PrtDispDisc(pTran, (ITEMDISC *)pItem);
        break;

    case CLASS_ITEMCOUPON:              /* coupon */
        PrtDispCoupon( pTran, ( ITEMCOUPON * )pItem );
        break;

    case CLASS_ITEMMISC:                /* nosale, ROA, paid out... */
        PrtDispMisc(pTran, (ITEMMISC *)pItem);
        break;

    case CLASS_ITEMTENDER:              /* tender */
        PrtDispTender(pTran, (ITEMTENDER *)pItem);
        break;

    case CLASS_ITEMAFFECTION:           /* tax */
        PrtDispAffection(pTran, (ITEMAFFECTION *)pItem);
        break;

    case CLASS_ITEMTOTAL:               /* total */
        PrtDispTotal(pTran, (ITEMTOTAL *)pItem);
        break;

    case CLASS_ITEMMULTI:               /* check paid */
        PrtDispMulti(pTran, (ITEMMULTI *)pItem);
        break;

    case CLASS_ITEMMODIFIER:            /* modifier */
        PrtDispModifier(pTran, (ITEMMODIFIER *)pItem);
        break;

    case CLASS_ITEMTRANSOPEN:           /* newcheck , reorder */
        PrtDispTransOpen(pTran, (ITEMTRANSOPEN *)pItem);
        break;

    default:
        break;
    }

}

/*
*===========================================================================
** Format  : VOID   PrtDispItem(TRANINFORMATION *pTran, VOID *pItem);      
*               
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*            VOID             *pItem     -Item Data address
*   Output : none
*    InOut : none
** Return  : USHORT  usPrtSlipPageLine   -current printed pages and lines       
*
** Synopsis: This function is called from application user and display item.
*===========================================================================
*/
USHORT   PrtDispItemForm(TRANINFORMATION  *pTran, VOID *pItem, TCHAR *puchBuffer)
{
    USHORT usLine, i;

    switch ( ((PRTITEMDATA *)pItem)->uchMajorClass ) {

    case CLASS_ITEMPRINT:               /* trailer , header , ticket */
        usLine = PrtDispPrintForm(pTran, (ITEMPRINT *)pItem, puchBuffer);
        break;

    case CLASS_ITEMSALES:               /* plu, dept, setmenu, mod.discount */
        usLine = PrtDispSalesForm(pTran, (ITEMSALES *)pItem, puchBuffer);
        break;

    case CLASS_UIFREGSALES:             /* print modifier */
        usLine = PrtDflPrtModForm(pTran, (UIFREGSALES *)pItem, puchBuffer);
        break;

    case CLASS_ITEMDISC:                /* discount */
        usLine = PrtDispDiscForm(pTran, (ITEMDISC *)pItem, puchBuffer);
        break;

    case CLASS_ITEMCOUPON:              /* coupon */
        usLine = PrtDispCouponForm( pTran, ( ITEMCOUPON * )pItem, puchBuffer);
        break;

    case CLASS_ITEMMISC:                /* nosale, ROA, paid out... */
        usLine = PrtDispMiscForm(pTran, (ITEMMISC *)pItem, puchBuffer);
        break;

    case CLASS_ITEMTENDER:              /* tender */
        usLine = PrtDispTenderForm(pTran, (ITEMTENDER *)pItem, puchBuffer);
        break;

    case CLASS_ITEMAFFECTION:           /* tax */
        usLine = PrtDispAffectionForm(pTran, (ITEMAFFECTION *)pItem, puchBuffer);
        break;

    case CLASS_ITEMTOTAL:               /* total */
        usLine = PrtDispTotalForm(pTran, (ITEMTOTAL *)pItem, puchBuffer);
        break;
/*
    case CLASS_ITEMMULTI:               / check paid /
        PrtDispMultiForm(pTran, (ITEMMULTI *)pItem, puchBuffer);
        break;
*/
    case CLASS_ITEMMODIFIER:            /* modifier */
        usLine = PrtDispModifierForm(pTran, (ITEMMODIFIER *)pItem, puchBuffer);
        break;

    case CLASS_ITEMTRANSOPEN:           /* newcheck , reorder */
        usLine = PrtDispTransOpenForm(pTran, (ITEMTRANSOPEN *)pItem, puchBuffer);
        break;

    default:
        usLine = 0;
        break;
    }

#if 1	/* just debug */
    switch ( ((PRTITEMDATA *)pItem)->uchMajorClass ) {
    case CLASS_ITEMSALES:               /* plu, dept, setmenu, mod.discount */
    case CLASS_UIFREGSALES:             /* print modifier */
    
	    switch ( ((PRTITEMDATA *)pItem)->uchMinorClass ) {
	    case CLASS_DEPTMODDISC:             /* dept modifiered disc. */
    	case CLASS_PLUMODDISC:              /* plu modifiered disc. */
	    case CLASS_SETMODDISC:              /* setmenu modifiered disc. */

	    	/* fill space, if NULL exists */
	    	for (i=0; i<usLine*(PRT_DFL_LINE+1); i++) {

    	    	if (*puchBuffer == '\0') {

        	    	*puchBuffer = _T(' ');
	        	}
	    	    puchBuffer++;
		    }
	    	*puchBuffer = '\0'; /* termination */
	    	break;
	    }
	    break;
	    
	default:
    	/* fill space, if NULL exists */
	    for (i=0; i<usLine*(PRT_DFL_LINE+1); i++) {

    	    if (*puchBuffer == '\0') {

        	    *puchBuffer = _T(' ');
	        }
    	    puchBuffer++;
	    }
	    *puchBuffer = '\0'; /* termination */
	}

#else
    /* fill space, if NULL exists */
    for (i=0; i<usLine*(PRT_DFL_LINE+1); i++) {

        if (*puchBuffer == '\0') {

            *puchBuffer = _T(' ');
        }
        puchBuffer++;
    }
    *puchBuffer = '\0'; /* termination */

#endif
    return usLine;
}

/*
*===========================================================================
** Format  : USHORT   PrtGetStatus(VOID);      
*               
*    Input : none
*   Output : none
*    InOut : none
** Return  : PRT_RJ_PRINTER    
*
** Synopsis: This function returns as R/J printer model
*===========================================================================
*/
USHORT   PrtGetStatus(VOID)
{
    return(PRT_RJ_PRINTER);
}

/***** End Of Source *****/
