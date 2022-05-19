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
*
*  NeighborhoodPOS Software Development Project
*  Department of Information Systems
*  College of Information Technology
*  Georgia Southern University
*  Statesboro, Georgia
*
*  Copyright 2014 Georgia Southern University Research and Services Foundation
* 
*===========================================================================
* Title       : Print Item  main                 
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrCItemT.C                                                      
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-14-93 : 01.00.12 : R.Itoh     : initial                                   
* Jun-23-93 : 01.00.12 : J.IKEDA    : Adds CLASS_PARASHRPRT
* Jun-30-93 : 01.00.12 : K.You      : Adds UCHAR fbPrtLockChk
* Jun-30-93 : 01.00.12 : J.IKEDA    : Adds CLASS_EMPLOYEENO, CLASS_RPTEMPLOYEE
* Jun-30-93 : 01.00.12 : R.Itoh     : Adds CLASS_ETKIN, CLASS_ETKOUT
* Jul-02-93 : 01.00.12 : K.You      : Adds CLASS_PARAPRESETAMOUNT
* Jul-07-93 : 01.00.12 : R.Itoh     : Adds PrtDispItem() for US Enhance
* Jul-16-93 : 01.00.12 : K.You      : Adds CLASS_PARAPIGRULE
* Aug-23-93 : 01.00.13 : R.Itoh     : Adds CLASS_TRAILER4
* Aug-23-93 : 01.00.13 : J.IKEDA    : Adds CLASS_PARASOFTCHK, Del CLASS_PARACHAR44
* Oct-06-93 : 02.00.01 : J.IKEDA    : Adds CLASS_MAINTETKFL
* Oct-29-93 : 02.00.02 : K.You      : Del. Dfl feature.
* Nov-08-93 : 02.00.02 : K.You      : Del. CLASS_PARAPIGRULE
* Nov-15-93 : 02.00.02 : M.Yamamoto : Add. CLASS_PARACPMEPTTALLY
* Nov-22-93 : 02.00.02 : M.Yamamoto : Add. CLASS_PARAHOTELIDL
*                                     Add. CLASS_MAINTCPM
* Apr-25-94 : 00.00.05 : K.You      : bug fixed E-25 (mod. PrtPrintItem)
* May-24-94 : 02.05.00 : Yoshiko.Jim: Dupli.Print for CP,EPT (add PrtPrintCpEpt())
* Jan-26-95 : 03.00.00 : M.Ozawa    : Recover PrtDispItem() for US R3.0
* Mar-01-95 : 03.00.00 : T.Nakahata : Add Coupon Feature
* Aug-04-98 : 03.03.00 : hrkato     : V3.3
* Aug-08-99 : 03.05.00 : M.Teraki   : Remove WAITER_MODEL
* Aug-12-99 : 03.05.00 : K.Iwata    : Marge GUEST_CHECK_MODEL/STORE_RECALL_MODEL
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
* Dec-03-99 : 01.00.00 : hrkato     : Saratoga
* Dec-14-99 : 00.00.01 : M.Ozawa    : Add PrtDflItemForm() 
* May-08-00 :          : M.Teraki   : fbPrtKPTarget removed.
* Jul-31-00 : V1.0.0.3 : M.Teraki   : Changed KP frame number to be managed for each KP
* Dec-19-02:           :R.Chambers  : SR 15 for Software Security
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
#include <tchar.h>

#include "ecr.h"
#include "rfl.h"
#include "paraequ.h"
#include "para.h"
#include "csstbpar.h"
#include "regstrct.h"
#include "transact.h"
#include "maint.h"
#include "csttl.h"
#include "csop.h"
#include "report.h"
#include "item.h"
#include "prt.h"
#include "ej.h"
#include "shr.h"
#include "dfl.h"
#include "prtcom.h"
#include "prtshr.h"
#include "prtdfl.h"
#include "prtrin.h"
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
USHORT  usPrtTHColumn = 42;   /* thermal's numbers of columns */
USHORT  usPrtEJColumn = 24;   /* journal's numbers of columns */
USHORT  usPrtVLColumn = 47;   /* validation's numbers of columns */
                              /* "47" for safety */

const TCHAR aszPrtAM[] = STD_TIME_AM_SYMBOL;
const TCHAR aszPrtPM[] = STD_TIME_PM_SYMBOL;

UCHAR   fbPrtTHHead;                            /* header printed or not */                        

/* -- for shared printer data -- */
PRTSHRINF PrtShrInf;                            /* shared print information */
UCHAR   uchPrtReceiptCo;                        /* number of receipt/ ticket to be printed */
USHORT  usPrtShrOffset;                         /* print data area offset */
UCHAR   fbPrtLockChk;                           /* lock check */
UCHAR   fbPrtShrStatus;                         /* shared print status */                        
UCHAR   fbPrtAltrStatus;                        /* printer alternation status */
UCHAR   fbPrtAbortStatus;                       /* abort key entry status */
UCHAR   fbPrtAltrSav;                           /* save alternation status */
UCHAR   fbPrtAbortSav;                          /* save abort key entry status */
UCHAR   fbPrtShrPrint;                          /* not print status */                        
USHORT  usPrtShrPmgSem;

PRTCONTROL PrtCompNo;                           /* print status save area */

/* -- for display on the fly -- */
PRTDFLIF    PrtDflIf;                           /* display data buffer */
UCHAR   fbPrtDflErr;                            /* error displayed or not */
UCHAR   uchPrtDflTypeSav;                       /* save transaction type */

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
	USHORT usLen;

	usLen = tcharlen(pLineToPrint);

	if (usLen < 0 || usLen > 255)
		return PMG_SUCCESS;

	return PmgPrint(usPrtControl, pLineToPrint, usLen);
}

// if this is a Zebra receipt printer used for Amtrak then
// lets make sure the printer buffer gets flushed.  Most Zebra
// printers are label type printers rather than receipt type printers
// so we have an agreement with Zebra that their OPOS control will
// see a PrintImmediate() method call and will then print whatever
// has been buffered up.  a paper cut will do the same buffer print.
// an NCR receipt printer sees this as print an empty string so it does nothing.
USHORT   PrtPrintLineImmediate (USHORT usPrtControl, TCHAR *pLineToPrint)
{
	USHORT usLen;

	usLen = tcharlen(pLineToPrint);

	if (usLen < 0 || usLen > 255)
		return PMG_SUCCESS;

	usPrtControl |= PMG_PRT_IMMEDIATE;
	return PmgPrint(usPrtControl, pLineToPrint, usLen);
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
/*      PrtSetRJColumn();   */
        break;

    case CLASS_ITEMPRINT:               /* trailer , header , ticket (ITEMPRINT *) */
        PrtItemPrint(pTran, pItem);
        break;

    case CLASS_ITEMSALES:               /* plu, dept, setmenu, mod.discount (ITEMSALES *) */
        PrtItemSales(pTran, pItem);
        break;

    case CLASS_ITEMDISC:                /* discount (ITEMDISC *) */
        PrtItemDisc(pTran, pItem);
        break;

    case CLASS_ITEMCOUPON:              /* coupon ( ITEMCOUPON * ) */
        PrtItemCoupon( pTran, pItem );
        break;

    case CLASS_ITEMMISC:                /* nosale, ROA, paid out... (ITEMMISC *) */
        PrtItemMisc(pTran, pItem);
        break;

    case CLASS_ITEMTENDER:              /* tender */
    case CLASS_ITEMTENDEREC:            /* EC tender for charge post (ITEMTENDER *) */
        PrtItemTender(pTran, pItem);
        break;

    case CLASS_ITEMAFFECTION:           /* tax (ITEMAFFECTION *) */
        PrtItemAffection(pTran, pItem);
        break;

    case CLASS_ITEMTRANSOPEN:           /* newcheck , reorder (ITEMTRANSOPEN *) */
        PrtItemTransOpen(pTran, pItem);
        break;

    case CLASS_ITEMTOTAL:               /* total (ITEMTOTAL *) */
        PrtItemTotal(pTran, pItem);
        fsPrtStatus |= PRT_REC_SLIP;    /* for grand total print (ITEMOTHER *) */
        break;

    case CLASS_ITEMOTHER:               /* feed */
        PrtItemOther(pTran, pItem);
        break;
    
    case CLASS_ITEMMULTI:               /* check paid (ITEMMULTI *) */
        PrtItemMulti(pTran, pItem);
        break;

    case CLASS_ITEMMODIFIER:            /* modifier (ITEMMODIFIER *) */
        PrtItemModifier(pTran, pItem);
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
        PrtThrmSupMnemData(pItem);
        break;

    case CLASS_PARADISCTBL:
    case CLASS_PARASECURITYNO:
    case CLASS_PARATRANSHALO:
    case CLASS_PARATEND:                                        /* V3.3 */
    case CLASS_PARASLIPFEEDCTL:
    case CLASS_PARACTLTBLMENU:
    case CLASS_PARAAUTOALTKITCH:
    case CLASS_PARAMANUALTKITCH:
    case CLASS_PARATTLKEYTYP:
    case CLASS_PARACASHABASSIGN:
    case CLASS_PARATONECTL:
    case CLASS_PARASHRPRT:
    case CLASS_PARASERVICETIME:
    case CLASS_PARALABORCOST:
        PrtThrmSupComData(pItem);
        break;

    case CLASS_PARAMDC:
    case CLASS_PARAACTCODESEC:
        PrtThrmSupMDC(pItem);
        break;
    
    case CLASS_PARADEPT:         // PrtPrintItem() - ( PARADEPT *)
        PrtThrmSupDEPT(pItem);
        break;

    case CLASS_PARAPLU:          // PrtPrintItem() - ( PARAPLU *)
        PrtThrmSupPLU(pItem);
        break;                                    

    case CLASS_PARAPLUNOMENU:         // PrtPrintItem() - ( PARAPLUNOMENU *)
        PrtThrmSupPLUNoMenu(pItem);
        break;  

    case CLASS_PARAROUNDTBL:         // PrtPrintItem() - ( PARAROUNDTBL *)
        PrtThrmSupRound(pItem);
        break;

    case CLASS_PARAFSC:              // PrtPrintItem() - ( PARAFSC *)
        PrtThrmSupFSC(pItem);
        break;

    case CLASS_PARAHOURLYTIME:       // PrtPrintItem() - ( PARAHOURLYTIME *)
        PrtThrmSupHourly(pItem);
        break;

    case CLASS_PARAFLEXMEM:         // PrtPrintItem() - ( PARAFLEXMEM *)
        PrtThrmSupFlexMem(pItem);
        break;

    case CLASS_PARASTOREGNO:         // PrtPrintItem() - ( PARASTOREGNO *)
        PrtThrmSupStoRegNo(pItem);
        break;

    case CLASS_PARASUPLEVEL:         // PrtPrintItem() - ( PARASUPLEVEL *)
        PrtThrmSupSupLevel(pItem);
        break;

    case CLASS_PARACURRENCYTBL:       // PrtPrintItem() - ( PARACURRENCYTBL *)
        PrtThrmSupCurrency(pItem);
        break;

    case CLASS_PARATAXRATETBL:        // PrtPrintItem() - ( PARATAXRATETBL *)
        PrtThrmSupTaxRate(pItem);
        break;                                         

    case CLASS_PARATTLKEYCTL:         // PrtPrintItem() - ( PARATTLKEYCTL *)
        PrtThrmSupTtlKeyCtl(pItem);
        break;                                         

    case CLASS_PARACASHIERNO:         // PrtPrintItem() - ( MAINTCASHIERIF *)
        PrtThrmSupCashierNo(pItem);
        break;                                         

    case CLASS_MAINTTAXTBL1:         // PrtPrintItem() - ( MAINTTAXTBL *)
    case CLASS_MAINTTAXTBL2:         // PrtPrintItem() - ( MAINTTAXTBL *)
    case CLASS_MAINTTAXTBL3:         // PrtPrintItem() - ( MAINTTAXTBL *)
    case CLASS_MAINTTAXTBL4:         // PrtPrintItem() - ( MAINTTAXTBL *)
        PrtThrmSupTaxTbl(pItem);
        break;
                                                       
    case CLASS_PARAMENUPLUTBL:        // PrtPrintItem() - ( MAINTMENUPLUTBL *)
        PrtThrmSupMenuPLU(pItem);
        break;


    case CLASS_PARAOEPTBL:           // PrtPrintItem() - ( PARAOEPTBL *)
        PrtThrmSupOepData(pItem);
        break;

    case CLASS_MAINTCSTR:            // PrtPrintItem() - ( MAINTCSTR *)
        PrtThrmSupCstrData(pItem);
        break;


    case CLASS_PARAFXDRIVE:           // PrtPrintItem() - ( PARAFXDRIVE *)
        PrtThrmSupFxDrive(pItem);
        break;

	case CLASS_MAINTPRINTLINE:         // PrtPrintItem() - ( MAINTPRINTLINE *)
        PrtThrmSupOutputLine(pItem);
        break;

    case CLASS_MAINTHEADER:           // PrtPrintItem() - ( MAINTHEADER *)
        PrtThrmSupHeader(pItem);
        break;

    case CLASS_MAINTTRAILER:          // PrtPrintItem() - ( MAINTTRAILER *)
        PrtThrmSupTrailer(pItem);
        break;

    case CLASS_MAINTTRANS:            // PrtPrintItem() - ( MAINTTRANS *)
        PrtThrmSupTrans(pItem);
        break;

    case CLASS_MAINTDSPFLINE:          // PrtPrintItem() -  Display Lead Through ( MAINTDSPFLINE *)
        PrtThrmSupDspfline(pItem);
        break;

    case CLASS_MAINTERRORCODE:         // PrtPrintItem() - ( MAINTERRORCODE *)
        PrtThrmSupErrorCode(pItem);
        break;

    case CLASS_RPTREGFIN:         // PrtPrintItem() - ( RPTREGFIN *)
        PrtThrmSupRegFin(pItem);
        break;                                      

    case CLASS_RPTHOURLY:         // PrtPrintItem() - ( RPTHOURLY *)
        PrtThrmSupHourlyAct(pItem);
        break;

    case CLASS_RPTDEPT:            // PrtPrintItem() - ( RPTDEPT *)
        PrtThrmSupDEPTFile(pItem);
        break;

    case CLASS_RPTPLU:             // PrtPrintItem() - ( RPTPLU *)
        PrtThrmSupPLUFile(pItem);
        break;

    case CLASS_RPTCASHIER:
    case CLASS_RPTWAITER:
    case CLASS_RPTEMPLOYEENO:
        PrtThrmSupCashWaitFile(pItem);
        break;

    case CLASS_RPTOPENCAS:
    case CLASS_RPTOPENWAI:
        PrtThrmSupWFClose(pItem);
        break;                                   

    case CLASS_RPTGUEST:           // PrtPrintItem() - ( RPTGUEST *)
        PrtThrmSupGCF(pItem);
        break;

    case CLASS_RPTOPENGCF:          // PrtPrintItem() - ( RPTGUEST *)
        PrtThrmSupGCFClose(pItem);
        break;                                    

    case CLASS_PARAGUESTNO:         // PrtPrintItem() - ( PARAGUESTNO *)
        PrtThrmSupGCNo(pItem);
        break;

    case CLASS_MAINTFEED:            // PrtPrintItem() - ( MAINTFEED *)
        PrtThrmSupRJFeed(pItem);
        break;

    case CLASS_MAINTBCAS:             // PrtPrintItem() - ( MAINTBCAS *)
        PrtThrmSupBcas(pItem);
        break;

    case CLASS_MAINTOPESTATUS:         // PrtPrintItem() - 
        PrtThrmSupOpeStatus(pItem);
        break;                                   

    case CLASS_PARATARE:               // PrtPrintItem() - ( PARATARE *)
        PrtThrmSupTare(pItem);
        break; 

    case CLASS_PARAPRESETAMOUNT:         // PrtPrintItem() - ( PARAPRESETAMOUNT *)
        PrtThrmSupPresetAmount(pItem);
        break;

    case CLASS_PARAEMPLOYEENO:           // PrtPrintItem() - ( PARAEMPLOYEENO *)
        PrtThrmSupEmployeeNo(pItem);
        break;

    case CLASS_PARACPMEPTTALLY:          // PrtPrintItem() -  ( PARACPMTALLY *) 
        PrtThrmSupTally(pItem);
        break;

    case CLASS_MAINTCPM:                  // PrtPrintItem() - CPM START STOP  ( MAINTCPM *)
        PrtThrmSupCpm(pItem);
        break;

    case CLASS_RPTJCSUMMARYTITLES:         // PrtPrintItem() -  (PRPTJCSUMMARY) 
    case CLASS_RPTJCSUMMARY:               // PrtPrintItem() -  (PRPTJCSUMMARY) 
    case CLASS_RPTJCSUMMARYTOTALS:         // PrtPrintItem() -  (PRPTJCSUMMARY) 
		PrtThrmSupETKFileJCSummary (pItem );
        break;

    case CLASS_RPTEMPLOYEE:         // PrtPrintItem() - ( RPTEMPLOYEE *)
        PrtThrmSupETKFile(pItem);
        break;

    case CLASS_RPTEJ:
        PrtSupEJ(( RPTEJ *)pItem);
        break;
                                          
    case CLASS_MAINTETKFL:         // PrtPrintItem() - ( MAINTETKFL *)
        PrtThrmSupMaintETKFl(pItem);  
        break;

    case CLASS_PARACPN:            // PrtPrintItem() - ( PARACPN *)
        PrtThrmSupCPN(pItem);
        break;

    case CLASS_RPTCPN:              // PrtPrintItem() - ( RPTCPN *)
        PrtThrmSupCPNFile(pItem);
        break;

    case CLASS_PARAPPI:              // PrtPrintItem() - ( PARAPPI *)
        PrtThrmSupPPI(pItem);
        break;

    case CLASS_RPTSERVICE:           // PrtPrintItem() - ( RPTSERVICE *)
        PrtThrmSupServiceTime(pItem);
        break;

    case CLASS_PARAPIGRULE:          // PrtPrintItem() - ( PARAPIGRULE *)
        PrtThrmSupPigRule(pItem);
        break;

    case CLASS_PARAMISCPARA:         // PrtPrintItem() - (PARAMISCPARA *)
        PrtThrmSupMiscPara(pItem);
        break;

    case CLASS_PARADISPPARA:         // PrtPrintItem() - ( PARADISPPARA *)
        PrtThrmSupDispPara(pItem);
        break;

    case CLASS_PARADEPTNOMENU:        // PrtPrintItem() - ( PARADEPTNOMENU *)
        PrtThrmSupDeptNoMenu(pItem);
        break;  

    case CLASS_MAINTTOD:              // PrtPrintItem() - tod report 2172 ( PARATOD *)
        PrtThrmSupTOD(pItem);
        break;

    case CLASS_PARAKDSIP:             // PrtPrintItem() - ( PARAKDSIP *)
    case CLASS_MAINTHOSTSIP:          // PrtPrintItem() - ( PARAKDSIP *)
        PrtThrmSupKdsIp(pItem);
        break;  

    case CLASS_PARAUNLOCKNO:          // PrtPrintItem() - ( PARAUNLOCKNO *)
        PrtThrmSupUnlockNo(pItem);
        break;  

    case CLASS_PARARESTRICTION:       // PrtPrintItem() -  plu sales restriction table 2172 ( PARARESTRICTION *)
        PrtThermSupRest(pItem);
        break;

    case CLASS_PARABOUNDAGE:          // PrtPrintItem() - boundary age 2172  ( PARABOUNDAGE *) 
          PrtThermSupAge(pItem);
        break;

    case CLASS_MAINTSPHEADER:         // PrtPrintItem() - slip header for super. mode (MAINTSPHEADER *)
        PrtSupSPHead(pTran, pItem);
        break;

    case CLASS_MAINTSPTRAILER:        // PrtPrintItem() - slip trailer for super. mode (MAINTTRAILER *)
        PrtSupSPTrailer(pItem);
        break;

    case CLASS_MAINTLOAN:             // PrtPrintItem() - loan,   (MAINTLOANPICKUP *)
    case CLASS_MAINTPICKUP:           // PrtPrintItem() - pick up (MAINTLOANPICKUP *)
        PrtSupLoanPickup(pTran, pItem);
        break;

	case CLASS_RPTSTOREFORWARD:
		// PrtStoreForwardrReport(pItem);   // remove legacy Store and Forward for OpenGenPOS Rel 2.4.0  03/30/2021  Richard Chambers
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
    case CLASS_ITEMPRINT:                    // PrtPrintCpEpt() - trailer , header , ticket (ITEMPRINT *)
        if (((PRTITEMDATA *)pItem)->uchMinorClass != CLASS_PROMOTION) {
            PrtItemPrint(pTran, pItem);
        }
        break;

    case CLASS_ITEMTENDER:                    // PrtPrintCpEpt() - tender (ITEMTENDER *)
    case CLASS_ITEMTENDEREC:                  // PrtPrintCpEpt() - EC tender for charge post (ITEMTENDER *)
        usMedia = PrtCheckTenderMedia(pItem);
        if (usMedia != PRT_TEND_OTHER) {      // PrtPrintCpEpt() - print ROOM,ACCT,CRE-AUT,EPT (ITEMTENDER *)
            PrtItemTender(pTran, pItem);
        }
        break;

    case CLASS_ITEMTRANSOPEN:                 // PrtPrintCpEpt() - newcheck , reorder (ITEMTRANSOPEN *)
        PrtItemTransOpen(pTran, pItem);
        break;

    case CLASS_ITEMTOTAL:                     // PrtPrintCpEpt() - total (ITEMTOTAL *)
        if (!auchPrtSeatCtl[0]) {
            fsPrtStatus |= PRT_REC_SLIP;      /* for grand total print */
        }
        PrtItemTotal(pTran, pItem);
        fsPrtStatus |= PRT_REC_SLIP;          /* for grand total print */
        break;

    case CLASS_ITEMOTHER:                     // PrtPrintCpEpt() - feed (ITEMOTHER *)
        PrtItemOther(pTran, pItem);
        break;

    case CLASS_ITEMMODIFIER:                  // PrtPrintCpEpt() - modifier (ITEMMODIFIER *)
        PrtItemModifier(pTran, pItem);
        break;

    default:
        break;                         
    }                                    
    return (usPrtSlipPageLine);         /* return current slip line & page */
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
    if (ItemTenderCheckTenderMdc(pItem->uchMinorClass, 3, MDC_PARAM_BIT_D)) {     /* Room Tender ?    */
        return(PRT_TEND_ROOM);                  /* ret Room Charge  */
    } else if (ItemTenderCheckTenderMdc(pItem->uchMinorClass, 3, MDC_PARAM_BIT_C)) {     /* Acct Tender ?    */
        return(PRT_TEND_ACCT);                  /* ret Acct Charge  */
    } else if (ItemTenderCheckTenderMdc(pItem->uchMinorClass, 3, MDC_PARAM_BIT_B)) {     /* Cre-Auth Tender? */
        return(PRT_TEND_CREAUTH);               /* ret Credit Autho */
    } else if (ItemTenderCheckTenderMdc(pItem->uchMinorClass, 3, MDC_PARAM_BIT_A)) {     /* EPT Tender ?     */
        return(PRT_TEND_EPT);                   /* ret EPT          */
    } else {
        return(PRT_TEND_OTHER);                 /* ret Other Tender */
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
** Synopsis: PrtDispItem for Printer Thermal display on the fly.
*
*            thermal printer entry point for the RS-232 display on the fly functionality
*            when MDC address 363, MDC_DSP_ON_FLY3, is zero or turned off. The alternative
*            is the kitchen display system (KDS) functionality which  flows through
*            a different area of source.
*===========================================================================
*/
VOID   PrtDispItem(TRANINFORMATION  *pTran, VOID *pItem)
{

    switch ( ((PRTITEMDATA *)pItem)->uchMajorClass ) {

    case CLASS_ITEMPRINT:                    // PrtDispItem() - trailer , header , ticket (ITEMPRINT *)
        PrtDispPrint(pTran, pItem);
        break;

    case CLASS_ITEMSALES:                    // PrtDispItem() - plu, dept, setmenu, mod.discount (ITEMSALES *)
        PrtDispSales(pTran, pItem);
        break;

    case CLASS_ITEMDISC:                     // PrtDispItem() - discount (ITEMDISC *)
        PrtDispDisc(pTran, pItem);
        break;

    case CLASS_ITEMCOUPON:                   // PrtDispItem() - coupon ( ITEMCOUPON * )
        PrtDispCoupon( pTran, pItem );
        break;

    case CLASS_ITEMMISC:                     // PrtDispItem() - nosale, ROA, paid out... (ITEMMISC *)
        PrtDispMisc(pTran, pItem);
        break;

    case CLASS_ITEMTENDER:                   // PrtDispItem() - tender (ITEMTENDER *)
        PrtDispTender(pTran, pItem);
        break;

    case CLASS_ITEMAFFECTION:                // PrtDispItem() - tax (ITEMAFFECTION *)
        PrtDispAffection(pTran, pItem);
        break;

    case CLASS_ITEMTOTAL:                    // PrtDispItem() - total (ITEMTOTAL *)
        PrtDispTotal(pTran, pItem);
        break;

	case CLASS_ITEMMULTI:                    // PrtDispItem() - check paid (ITEMMULTI *)
        PrtDispMulti(pTran, pItem);
        break;

    case CLASS_ITEMMODIFIER:                 // PrtDispItem() - modifier (ITEMMODIFIER *)
        PrtDispModifier(pTran, pItem);
        break;

    case CLASS_ITEMTRANSOPEN:                // PrtDispItem() - newcheck , reorder (ITEMTRANSOPEN *)
        PrtDispTransOpen(pTran, pItem);
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

    	    	if (*puchBuffer == _T('\0')) {

        	    	*puchBuffer = _T(' ');
	        	}
	    	    puchBuffer++;
		    }
	    	*puchBuffer = _T('\0'); /* termination */
	    	break;
	    }
	    break;
	    
	default:
    	/* fill space, if NULL exists */
	    for (i=0; i<usLine*(PRT_DFL_LINE+1); i++) {

    	    if (*puchBuffer == _T('\0')) {

        	    *puchBuffer = _T(' ');
	        }
    	    puchBuffer++;
	    }
	    *puchBuffer = _T('\0'); /* termination */
	}

#else
    /* fill space, if NULL exists */
    for (i=0; i<usLine*(PRT_DFL_LINE+1); i++) {

        if (*puchBuffer == _T('\0')) {

            *puchBuffer = _T(' ');
        }
        puchBuffer++;
    }
    *puchBuffer = _T('\0'); /* termination */

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
** Return  : PRT_THERMAL_PRINTER    
*
** Synopsis: This function returns as thermal printer model
*===========================================================================
*/
USHORT   PrtGetStatus(VOID)
{

    return(PRT_THERMAL_PRINTER);
}

/*
*===========================================================================
** Format  : USHORT   PrtGetSlipPageLine(VOID)
*               
*    Input : none
*   Output : none
*    InOut : none
** Return  : 
*
** Synopsis: 
*===========================================================================
*/
USHORT   PrtGetSlipPageLine(VOID)
{

    return (usPrtSlipPageLine);         /* return current slip line & page */
}

/*
*===========================================================================
** Format  : USHORT   PrtSetSlipPageLine(USHORT usLine)
*               
*    Input : none
*   Output : none
*    InOut : none
** Return  : 
*
** Synopsis: 
*===========================================================================
*/
VOID   PrtSetSlipPageLine(USHORT usLine)
{

    usPrtSlipPageLine = usLine;
}

/***** End Of Source *****/
