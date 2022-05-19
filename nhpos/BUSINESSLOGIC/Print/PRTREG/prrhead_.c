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
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995-2000       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print Item  Header trailer                 
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRHead_.C                                                      
* --------------------------------------------------------------------------
* Abstract:
*      PrtItemPrint() : print items specified class "CLASS_ITEMPRINT"
*      PrtDispPrint() : display items specified class "CLASS_ITEMPRINT"
*
*      PrtChkCurSlip()
*      PrtTraining()
*      PrtTrain_RJ()
*      PrtTrain_SP()
*      PrtPVoid()
*      PrtPVoid_RJ()
*      PrtPVoid_SP()
*      PrtPostRct()
*      PrtPostRct_RJ()
*      PrtPostRct_SP()
*      PrtCPGusLine( )
*      PrtCPGusLine_RJ()
*      PrtCPFuncErrCode_RJ()
*      PrtEPTError()
*      PrtEPTStub()
*      PrtEPTStub_RJ()
*      PrtParkReceipt()     :   Add (REL 3.0)
*      PrtParkReceipt_RJ()  :   Add (REL 3.0)
*      PrtParkReceipt_SP()  :   Add (REL 3.0)
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* May-15-92 : 00.00.01 : K.Nakajima :                                    
* Jun-24-93 : 01.00.12 : K.You      : add CLASS_RESTART handling
* Jul-02-93 : 01.00.12 : K.You      : add PrtPostRct, PrtPostRct_RJ, PrtPOSTRct_SP,
*           :          :            : mod. PrtChkCurSlip for post receipt feature.
* Jul-12-93 : 01.00.12 :  R.Itoh    : add PrtDispPrint()
* Aug-23-93 : 01.00.13 :  R.Itoh    : add CLASS_TRAILER4
* Oct-27-93 : 02.00.02 :  K.You     : add charge posting feature.
* Oct-28-93 : 02.00.02 :  K.You     : del. PrtDispPrint()
* Apr-04-94 : 00.00.04 :  Yoshiko.J : add CLASS_EPT_TRAILER, CLASS_EPT_STUB,
*                                   : CLASS_ERROR
* May-23-94 : 02.05.00 :  Yoshiko.J : Q-013 corr. (mod.PrtItemPrint())
* May-25-94 : 02.05.00 :  Yoshiko.J : mod.PrtItemPrint() case of CLASS_EPT_TRAILER)
* May-25-94 : 02.05.00 :  K.You     : mod.PrtItemPrint(), PrtEPTStub(), PrtEPTStub_RJ() for duplicate print
* Jan-17-95 : 03.00.00 :  M.Ozawa   : add PrtDispPrint() for display on the fly
* Mar-09-95 : 03.00.00 : T.Nakahata : add Parking Receipt feature.
* Jul-12-95 : 03.00.04 : T.Nakahata : fix PrtChkCurSlip() (CURQUAL_PARKING) 
* Jul-25-95 : 03.00.04 : T.Nakahata : print customer name on K/P
* Aug-28-95 : 03.00.05 : T.Nakahata : INVALID TRANS. on journal at service ttl
* Aug-16-99 : 03.05.00 : M.Teraki   : Merge STORE_RECALL_MODEL
*           :          :            :       /GUEST_CHECK_MODEL
* Feb-21-00 : 01.00.00 : hrkato     : Saratoga
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
/**------- M S - C --------**/
#include	<tchar.h>
#include    <string.h>
/**------- 2170 local------**/
#include    "ecr.h"
#include    "paraequ.h"
#include    "para.h"
#include    "regstrct.h"
#include    "transact.h"
#include    "csstbpar.h"
#include    "dfl.h"
#include    "pmg.h"
#include    "prtrin.h"
#include    "prtdfl.h"
#include    "prrcolm_.h"
#include    <rfl.h>

/**
;========================================================================
;+                    S T A T I C    R A M s
;========================================================================
**/

CONST TCHAR FARCONST  aszPrtRJCPFuncErrCode[] = _T(" F-CODE%5d E-CODE%3d");
                                                 /* function,error code for charge posting */
CONST TCHAR FARCONST  aszPrtVLFSCredit[] = _T("%-8.8s \t %l$");   /* mnemonic, amount */ /*8 characters JHHJ*/

extern CONST TCHAR FARCONST  aszPrtRJAmtMnem[];
extern CONST TCHAR FARCONST  aszPrtRJMnemMnem[];
extern CONST TCHAR FARCONST  aszPrtSPMnemMnem[];
                                                 /* mnem. and mnem. */
/*
*===========================================================================
** Format  : VOID  PrtItemPrint(TRANINFORMATION *pTran,ITEMPRINT *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMPRINT        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints  Header & Trailer.
*===========================================================================
*/
VOID   PrtItemPrint(TRANINFORMATION  *pTran, ITEMPRINT  *pItem)
{
    switch (pItem->uchMinorClass) {

    case CLASS_START:                           /* Print Start */
        PrtSetRJColumn();                       /* set RJ Col. to static area */

        /* -- check "kp only"(prints to kp or take to kitchen) -- */
        if ( !(fsPrtStatus & PRT_KPONLY) ) {
            /*  set up no-print, compulsory print           */
            /* "fsPrtCompul" & "fsPrtNoPrtMask" initialized */
            /*  in TPM(PrcTran_.c)                          */
            if (fsPrtStatus & PRT_SPLIT_PRINT) {
                if (auchPrtSeatCtl[0]) {    /* at consolidation seat# print */
                    if (auchPrtSeatCtl[1]) {
                        break;  /* not initialize slip at consolidation print */
                    }               /* initialize slip each seat no print */
                }
            }

            if ((fsPrtStatus & PRT_SPLIT_PRINT) && (auchPrtSeatCtl[1])) {              /* after secondary print */
                ;   /* not set compulsory print status */

            } else {
                fsPrtCompul  = pTran->TranCurQual.fbCompPrint;
                fsPrtNoPrtMask = ~(pTran->TranCurQual.fbNoPrint);
            }

            if ( !(fsPrtStatus & PRT_DEMAND) ) {    /* not print on demand */
                PrtSlipInitial();                   /* Slip initialize, Saratoga */
                if ( !(fsPrtStatus & PRT_SPLIT_PRINT) ) {
                    if ( (fsPrtCompul & fsPrtNoPrtMask) & PRT_SLIP ) {    
                       PrtChkCurSlip(pTran);        /* get current slip line and feed */
                    }
                }
            }
        }

        /* -- kitchen printer management -- */
        if (fsPrtStatus & PRT_REQKITCHEN) {
            PrtKPInit();                  /* kp initialize */
            PrtKPOperatorName( pTran);      /* print operator name on kp R3.1 */
            PrtKPHeader(pTran);           /* set kp header */

            /* --- print customer name on KP, if name is specified --- */
            PrtKPCustomerName( pTran->TranGCFQual.aszName );
            PrtKPTrailer(pTran, pTran->TranCurQual.ulStoreregNo);
                                          /* set kp trailer */
        }
        break;

    case CLASS_LOANPICKSTART:                   /* Print Start, Saratoga */
        PrtSetRJColumn();                       /* set RJ Col. to static area */
        fsPrtCompul  = pTran->TranCurQual.fbCompPrint;
        fsPrtNoPrtMask = ~(pTran->TranCurQual.fbNoPrint);
        break;

    case CLASS_PROMOTION:                 /* promotional header */
        if (fsPrtStatus & PRT_SPLIT_PRINT) {
            break;
        }

        PrtPromotion(pTran, pItem->fsPrintStatus);       /* print promotional header */

        PrtRFeed(1);                      /* 1 line feed (Receipt) */

        break;

    case CLASS_MONEY_HEADER:                /* Money Header,    Saratoga */
        if (fsPrtStatus & PRT_SPLIT_PRINT) {
            break;
        }
        PrtMoneyHeader(pTran, pItem->fsPrintStatus);
        break;

    case CLASS_DOUBLEHEADER:              /* double receipt */
        PrtDoubleHeader(pTran);
        break;

    case CLASS_PRINT_TRAIN:               /* training line */
        if (fsPrtStatus & PRT_SPLIT_PRINT) {
            break;
        }

        PrtTraining(pTran, pItem->fsPrintStatus);

        break;

    case CLASS_PRINT_PVOID:               /* preselect void line */
        if (fsPrtStatus & PRT_SPLIT_PRINT) {
            break;
        }

        PrtPVoid(pTran, pItem->fsPrintStatus);
        break;

    case CLASS_PRINT_TRETURN:               /* return transaction line */
        if (fsPrtStatus & PRT_SPLIT_PRINT) {
            break;
        }

        PrtPReturn(pTran, pItem->fsPrintStatus);
        break;

    case CLASS_TRAILER1:                  /* trailer & header (normal) */
    case CLASS_TRAILER_MONEY:               /* Saratoga */
        if (fsPrtStatus & PRT_REQKITCHEN) {
            PrtKPEnd();
        }
        if ((fsPrtStatus & PRT_SPLIT_PRINT) && ((pTran->TranCurQual.fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_NEWCHECK)) {

            PrtSoftProm(pItem);   
        }
        PrtTrail1( pTran, (ITEMPRINT *)pItem);
        break;

    case CLASS_TRAILER2:                  /* trailer & header (ticket) */
        PrtTrail2(pTran);
        break;

    case CLASS_TRAILER3:                  /* trailer & header (newcheck,
                                                                reorder) */
        if (fsPrtStatus & PRT_REQKITCHEN) {
            PrtKPEnd();
        }
        PrtTrail3(pTran, (ITEMPRINT *)pItem);
        break;

    case CLASS_TRAILER4:                  /* promotional message for soft check */ 
        if (fsPrtStatus & PRT_REQKITCHEN) {
            PrtKPEnd();
        }
        PrtSoftProm(pItem);   
        PrtTrail1( pTran, (ITEMPRINT *)pItem);
        break;

    case CLASS_EPT_TRAILER:                             /* EPT Logo Message */ 
        if (fsPrtStatus & PRT_REQKITCHEN) {             /* Q-013 corr. 5/23 */
            PrtKPEnd();
        }
        PrtRFeed(1);                                    /* add '94 5/25     */
        PrtSoftProm(pItem);   
        PrtTrail1( pTran, (ITEMPRINT *)pItem);
        break;

    case CLASS_TRAYTRAILER:               /* trailer & header (tray total) */
        PrtTrayTrail(pTran, (ITEMPRINT *)pItem);
        break;

    case CLASS_CHECKPAID:                 /* multi check */
        PrtChkPaid((ITEMPRINT *)pItem);
        break;

    case CLASS_POST_RECPT:                      /* post receipt mnem. */
        PrtPostRct(pTran, pItem);
        break;

    case CLASS_PARK_RECPT:                      /* parking receipt, R3.0 */
        PrtParkReceipt( pTran, pItem );
        break;

    case CLASS_CP_GUSLINE:                      /* charge posting guest line # */
        PrtCPGusLine(pItem);
        break;

    case CLASS_CP_ERROR:                        /* func, error code for charge posting */
        PrtCPFuncErrCode_RJ(pItem);
        break;

    case CLASS_RESTART:                         /* Print ReStart */
        break;

    case CLASS_ERROR:                           /* EPT error                */
        PrtEPTError(pItem);
        break;

    case CLASS_EPT_STUB:                        /* EPT duplicate(stub) amt  */
        PrtEPTStub( pTran, pItem );
        break;

    case CLASS_NUMBER: /* 2172 */
        PrtNumber(pTran, pItem);
        break;
        
    case CLASS_AGELOG: /* 2172 */
        PrtAgeLog(pTran, pItem);
        break;

    case CLASS_RSPTEXT:                         /* EPT Response Message,    Saratoga */
        PrtCPRespText(pTran, pItem);
        break;

    case CLASS_FOOD_CREDIT:                     /* food stamp credit,   Saratoga */
        PrtFSCredit(pTran, pItem);
        break;

    case CLASS_CHECK_ENDORSE:               /* DTREE#2 Check Endorsement,   Dec/18/2000 */
        PrtEndorse( pTran, pItem);
        break;

    default:
        break;
    }
}

/*
*===========================================================================
** Format  : VOID   PrtChkCurSlip(TRANINFORMATION  *pTran);
*                                     
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTRANSOPEN    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function sets slip page No. and line No. 
*===========================================================================
*/
VOID   PrtChkCurSlip(TRANINFORMATION  *pTran)
{
    PARASLIPFEEDCTL  FeedCtl;

/*** bug fixed (95-7-12) ***
    if ( pTran->TranCurQual.flPrintStatus & CURQUAL_POSTRECT ) {
*** bug fixed (95-7-12) ***/

    if ( pTran->TranCurQual.flPrintStatus & ( CURQUAL_POSTRECT | CURQUAL_PARKING )) {
        usPrtSlipPageLine = 0;
        return;
    }

    if ((pTran->TranGCFQual.uchPageNo == 0) && (pTran->TranGCFQual.uchSlipLine == 0)) {
        FeedCtl.uchMajorClass = CLASS_PARASLIPFEEDCTL;
        FeedCtl.uchAddress = SLIP_NO1ST_ADR;   
        CliParaRead(&FeedCtl);    
        
        /* set the initial page No. and line No. */         
        usPrtSlipPageLine = 0 * PRT_SLIP_MAX_LINES + FeedCtl.uchFeedCtl;    /* start from 0 page and pre-feeded line */
        
        /* feed before print start */        
        PrtFeed(PMG_PRT_SLIP, (USHORT)FeedCtl.uchFeedCtl);
    } else {
        /* set the current page No. and line No. */    
        usPrtSlipPageLine = pTran->TranGCFQual.uchPageNo * PRT_SLIP_MAX_LINES + pTran->TranGCFQual.uchSlipLine;        
        
        PrtFeed(PMG_PRT_SLIP, (USHORT)pTran->TranGCFQual.uchSlipLine);
    }
}


/*
*===========================================================================
** Format  : VOID  PrtTraining(TRANINFORMATION *pTran, ITEMPRINT *pItem);
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*            ITEMPRINT       *pItem    -item information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints training line. 
*===========================================================================
*/
VOID  PrtTraining(TRANINFORMATION *pTran, SHORT fsPrintStatus)
{
    SHORT   fsPortSave;

    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(fsPrintStatus);

    if ( fsPrtPrintPort & PRT_SLIP ) {        /* slip print */
        PrtTrain_SP(pTran);
    }

    if ((fsPrtPrintPort & PRT_RECEIPT) || (fsPrtPrintPort & PRT_JOURNAL)) {

        PrtTrain_RJ(pTran);
    }

    if (( ! ( fsPrtPrintPort & PRT_JOURNAL )) && ( fsPrtStatus & PRT_SERV_JOURNAL )) {
        fsPortSave = fsPrtPrintPort;
        fsPrtPrintPort = PRT_JOURNAL;

        /* --- print INVALID TRANSACTION line on journal --- */
        PrtTrain_RJ( pTran );

        fsPrtPrintPort = fsPortSave;
    }
}

/*
*===========================================================================
** Format  : VOID  PrtTrain_RJ(TRANINFORMATION *pTran);
*
*   Input  : TRANINFORMATION *pTran             -transaction information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints training line. ( Receipt & Journal )
*===========================================================================
*/
VOID  PrtTrain_RJ(TRANINFORMATION *pTran)
{
    TCHAR   aszMnemonics[PARA_CHAR24_LEN + 1];
    USHORT  usLen;
    SHORT   sPortion;

    PrtGet24Mnem(aszMnemonics, CH24_TRNGHED_ADR); /* get 24 char mnem. */

    usLen = _tcslen(aszMnemonics);
    usLen = (usLen > usPrtRJColumn)?  usPrtRJColumn : usLen;

    sPortion = PrtChkPort();

    /* training line printed only once */
    /* on journal */
    if ( pTran->TranCurQual.fsCurStatus & CURQUAL_TRAY ) {
        if ( sPortion == PMG_PRT_RCT_JNL ) {
            sPortion = PMG_PRT_RECEIPT;
        } else if ( sPortion == PMG_PRT_JOURNAL ) {
            sPortion = PRT_NOPORTION;
        }
    }

    PmgPrint(sPortion, aszMnemonics, usLen);

    PrtRFeed(1);                                  /* 1 line feed */
}

/*
*===========================================================================
** Format  : VOID  PrtTrain_SP(TRANINFORMATION *pTran);
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*            ITEMPRINT       *pItem    -item information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints training line. ( Slip )
*===========================================================================
*/
VOID PrtTrain_SP(TRANINFORMATION *pTran)
{

    TCHAR   aszMnemonics[PARA_CHAR24_LEN + 1];
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines to be added */

    /* training line printed only once */
    /* on slip */
    if ( pTran->TranCurQual.fsCurStatus & CURQUAL_TRAY ) {
        return ;
    }

    /* -- set training mode mnemonic -- */
    PrtGet24Mnem(aszMnemonics, CH24_TRNGHED_ADR);
    usSlipLine ++;

    /* -- check if paper change is necessary or not -- */ 
    usSaveLine = PrtCheckLine(usSlipLine, pTran);
    PmgPrint(PMG_PRT_SLIP, aszMnemonics, (USHORT)_tcslen(aszMnemonics)); /* ### Mod (2171 for Win32) V1.0 */

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;        

}


/*
*===========================================================================
** Format  : VOID  PrtPVoid(TRANINFORMATION *pTran, ITEMPRINT *pItem);
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*            ITEMPRINT       *pItem    -item information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints PreVoid line. 
*===========================================================================
*/
VOID  PrtPVoid(TRANINFORMATION *pTran, SHORT fsPrintStatus)
{

    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(fsPrintStatus);

    if ( fsPrtPrintPort & PRT_SLIP ) {        /* slip print */
        PrtPVoid_SP(pTran);
    }

    if ((fsPrtPrintPort & PRT_RECEIPT) || (fsPrtPrintPort & PRT_JOURNAL)) {
        PrtPVoid_RJ(pTran);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtPVoid_RJ(TRANINFORMATION *pTran);
*
*   Input  : pTran                              -transaction information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints PreVoid line. ( Receipt & Journal )
*===========================================================================
*/
VOID  PrtPVoid_RJ(TRANINFORMATION *pTran)
{
    TCHAR   aszMnemonics[PARA_CHAR24_LEN + 1];
    USHORT  usLen;
    SHORT   sPortion;

    PrtGet24Mnem(aszMnemonics, CH24_PVOID_ADR);   /* get 24char mnem. */

    usLen = _tcslen(aszMnemonics);
    usLen = (usLen > usPrtRJColumn)?  usPrtRJColumn : usLen;

    sPortion = PrtChkPort();

    /* p-void line printed only once */
    /* on journal */
    if ( pTran->TranCurQual.fsCurStatus & CURQUAL_TRAY ) {

        if ( sPortion == PMG_PRT_RCT_JNL ) {
            sPortion = PMG_PRT_RECEIPT;
        } else if ( sPortion == PMG_PRT_JOURNAL ) {
            sPortion = PRT_NOPORTION;
        }
    }

    PmgPrint(sPortion, aszMnemonics, usLen);

    PrtRFeed(1);                                  /* 1 line feed */
}

/*
*===========================================================================
** Format  : VOID  PrtPVoid_SP(TRANINFORMATION *pTran);
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*            ITEMPRINT       *pItem    -item information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints PreVoid line. ( Slip )
*===========================================================================
*/
VOID PrtPVoid_SP(TRANINFORMATION *pTran)
{
    TCHAR   aszMnemonics[PARA_CHAR24_LEN + 1];
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines to be added */

    if ( pTran->TranCurQual.fsCurStatus & CURQUAL_TRAY ) {
        return ;                       /* tray total */
    }                                  /* p-void line printed only once */
                                       /* on slip */

    /* -- set preselect void mnemonic -- */
    PrtGet24Mnem(aszMnemonics, CH24_PVOID_ADR);
    usSlipLine ++;

    /* -- check if paper change is necessary or not -- */ 
    usSaveLine = PrtCheckLine(usSlipLine, pTran);
    PmgPrint(PMG_PRT_SLIP, aszMnemonics, (USHORT)_tcslen(aszMnemonics)); /* ### Mod (2171 for Win32) V1.0 */

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;        
}

/*
*===========================================================================
** Format  : VOID  PrtMoneyHeader(TRANINFORMATION *pTran, ITEMPRINT *pItem)
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*            ITEMPRINT       *pItem    -item information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints Money Header.                 Saratoga 
*===========================================================================
*/
VOID    PrtMoneyHeader(TRANINFORMATION *pTran, SHORT fsPrintStatus)
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(fsPrintStatus);

    if (fsPrtPrintPort & PRT_SLIP) {
        PrtMoneyHeader_SP(pTran);
    }
    if (fsPrtPrintPort & PRT_RECEIPT || fsPrtPrintPort & PRT_JOURNAL) {
        PrtMoneyHeader_RJ(pTran);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtMoneyHeader_RJ(TRANINFORMATION *pTran)
*
*   Input  : TRANINFORMATION    *pTran  -Transaction Information address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints money header                  Saratoga
*===========================================================================
*/
VOID    PrtMoneyHeader_RJ(TRANINFORMATION *pTran)
{
    TCHAR   aszMnemonics[PARA_CHAR24_LEN + 1];
    USHORT  usLen;
    SHORT   sPortion;

    PrtGet24Mnem(aszMnemonics, CH24_MONEY_ADR);
    usLen = _tcslen(aszMnemonics);
    usLen = (usLen > usPrtRJColumn)?  usPrtRJColumn : usLen;
    sPortion = PrtChkPort();

    if (pTran->TranCurQual.fsCurStatus & CURQUAL_TRAY) {
        if (sPortion == PMG_PRT_RCT_JNL) {
            sPortion = PMG_PRT_RECEIPT;
        } else if (sPortion == PMG_PRT_JOURNAL) {
            sPortion = PRT_NOPORTION;
        }
    }
    PmgPrint(sPortion, aszMnemonics, usLen);
    PrtRFeed(1);
}

/*
*===========================================================================
** Format  : VOID  PrtMoneyHeader_SP(TRANINFORMATION *pTran)
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*            ITEMPRINT       *pItem    -item information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints money header(Slip).           Saratoga
*===========================================================================
*/
VOID    PrtMoneyHeader_SP(TRANINFORMATION *pTran)
{
    TCHAR   aszMnemonics[PARA_CHAR24_LEN + 1];
    USHORT  usSlipLine = 0;
    USHORT  usSaveLine;

    if (pTran->TranCurQual.fsCurStatus & CURQUAL_TRAY) {
        return;
    }
    PrtGet24Mnem(aszMnemonics, CH24_MONEY_ADR);
    usSlipLine++;
    usSaveLine = PrtCheckLine(usSlipLine, pTran);
    PmgPrint(PMG_PRT_SLIP, aszMnemonics, (USHORT)_tcslen(aszMnemonics));
    usPrtSlipPageLine += usSlipLine + usSaveLine;        
}

/*
*===========================================================================
** Format  : VOID  PrtPostRct(TRANINFORMATION *pTran, ITEMPRINT *pItem);
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*            ITEMPRINT       *pItem    -item information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints tranction void line. 
*===========================================================================
*/
VOID  PrtPostRct(TRANINFORMATION *pTran, ITEMPRINT *pItem)
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( fsPrtPrintPort & PRT_SLIP ) {              /* slip print */
        PrtPostRct_SP(pTran);
    }

    if ( (fsPrtPrintPort & PRT_JOURNAL) || (fsPrtPrintPort & PRT_RECEIPT) ) {         /* journal print */
        PrtPostRct_RJ(pTran);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtPostRct_RJ(TRANINFORMATION *pTran);
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints post receit mnem. line. ( Receipt & Journal )
*===========================================================================
*/
VOID  PrtPostRct_RJ(TRANINFORMATION *pTran)
{
    TCHAR   aszMnemonics[PARA_CHAR24_LEN + 1];
    USHORT  usLen;
    SHORT   sPortion;
    
    PrtGet24Mnem(aszMnemonics, CH24_POSTR_ADR); /* get 24 char mnem. */

    usLen = _tcslen(aszMnemonics);
    usLen = (usLen > usPrtRJColumn)?  usPrtRJColumn : usLen;

    sPortion = PrtChkPort();

    /* post receipt line printed only once */
    /* on journal */
    if ( pTran->TranCurQual.fsCurStatus & CURQUAL_TRAY ) {
        if ( sPortion == PMG_PRT_RCT_JNL ) {
            sPortion = PMG_PRT_RECEIPT;
        } else if ( sPortion == PMG_PRT_JOURNAL ) {
            sPortion = PRT_NOPORTION;
        }

    }

    PmgPrint(sPortion, aszMnemonics, usLen);
}


/*
*===========================================================================
** Format  : VOID  PrtPostRct_SP(TRANINFORMATION *pTran);
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*            ITEMPRINT       *pItem    -item information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints post receit mnem. line. ( Slip )
*===========================================================================
*/
VOID PrtPostRct_SP(TRANINFORMATION *pTran)
{
    TCHAR   aszMnemonics[PARA_CHAR24_LEN + 1];
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines to be added */

    /* -- set post receipt mnemonic -- */
    PrtGet24Mnem(aszMnemonics, CH24_POSTR_ADR);
    usSlipLine ++;

    /* -- check if paper change is necessary or not -- */ 
    usSaveLine = PrtCheckLine(usSlipLine, pTran);
    PmgPrint(PMG_PRT_SLIP, aszMnemonics, (USHORT)_tcslen(aszMnemonics)); /* ### Mod (2171 for Win32) V1.0 */

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;        
}

/*
*===========================================================================
** Format  : VOID  PrtCPGusLine(ITEMPRINT *pItem);
*
*   Input  : ITEMPRINT       *pItem    -item information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints charge posting guest line #. 
*===========================================================================
*/
VOID  PrtCPGusLine(ITEMPRINT *pItem)
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);
                                            
    if ((fsPrtPrintPort & PRT_RECEIPT)) {       /* receipt print */
        PrtCPGusLine_RJ(pItem);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtCPGusLine_RJ(ITEMPRINT *pItem);
*
*   Input  : ITEMPRINT       *pItem    -item information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints charge posting guest line #. ( Receipt & Journal )
*===========================================================================
*/
VOID  PrtCPGusLine_RJ(ITEMPRINT *pItem)
{
    USHORT  usLen;
    SHORT   sPortion;

    usLen = _tcslen(pItem->aszCPText[0]);

    usLen = (usLen > usPrtRJColumn)?  usPrtRJColumn : usLen;

    sPortion = PrtChkPort();

    PmgPrint(sPortion, pItem->aszCPText[0], usLen);
}

/*
*===========================================================================
** Format  : VOID  PrtCPFuncErrCode_RJ(ITEMPRINT *pItem)
*                                    
*   Input  : ITEMPRINT       *pItem    -item information
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints "function and error code number" .
*===========================================================================
*/
VOID  PrtCPFuncErrCode_RJ(ITEMPRINT *pItem)
{
    PmgPrintf(PMG_PRT_JOURNAL, aszPrtRJCPFuncErrCode, pItem->usFuncCode, pItem->sErrorCode);
}

/*
*===========================================================================
** Format  : VOID  PrtEPTError(ITEMPRINT  *pItem);
*
*   Input  : TEMPRINT  *pItem
*             
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints EPT Error. ( Journal )
*===========================================================================
*/
VOID  PrtEPTError(ITEMPRINT  *pItem)
{
    PrtRJEPTError(pItem);
}

/*
*===========================================================================
** Format  : VOID  PrtEPTStub( TRANINFORMATION *pTran, ITEMPRINT  *pItem );
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMPRINT        *pItem
*             
*   Output : none
*   InOut  : none
*            
** Return  : 
*            
** Synopsis: 
*===========================================================================
*/
VOID    PrtEPTStub( TRANINFORMATION  *pTran, ITEMPRINT  *pItem )
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);
 
    PrtEPTStub_RJ( pTran, pItem );
}

/*
*===========================================================================
** Format  : VOID  PrtEPTStub_RJ( TRANINFORMATION  *pTran, ITEMPRINT  *pItem );
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMPRINT        *pItem
*    
*             
*   Output : none
*   InOut  : none
*            
** Return  : 
*            
** Synopsis: 
*===========================================================================
*/
VOID    PrtEPTStub_RJ( TRANINFORMATION  *pTran, ITEMPRINT *pItem )
{
    USHORT  i;

    PrtRJGuest(pTran->TranGCFQual.usGuestNo, pTran->TranGCFQual.uchCdv);
    PrtRJOffTend(pItem->fbModifier);                    /* CP off line      */
    PrtRJZeroAmtMnem( TRN_AMTTL_ADR, pItem->lAmount );  /* total amount     */
    PrtRJOffline( pItem->fbModifier, pItem->auchExpiraDate, pItem->auchApproval );                /* approval code    */

    PrtRJCPRoomCharge(pItem->aszRoomNo, pItem->aszGuestID); /* for charge posting */
    PrtRJNumber(pItem->aszNumber[0]); //US Customs      /* Acct number line */
    for (i = 0; i < NUM_CPRSPCO; i++) {
        PrtRJCPRspMsgText(pItem->aszCPText[i]);         /* for charge posting */
    }
}

/*
*===========================================================================
** Format  : VOID  PrtDispPrint(TRANINFORMATION *pTran,ITEMPRINT *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMPRINT        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints  Header & Trailer.
*===========================================================================
*/
VOID   PrtDispPrint(TRANINFORMATION  *pTran, ITEMPRINT  *pItem)
{
    switch (pItem->uchMinorClass) {

    case CLASS_START:                           /* Print Start */
        /* -- set display initial data -- */
        PrtDflInit(pTran);
        break;

    case CLASS_TRAILER1:                  /* trailer & header (normal) */
    case CLASS_TRAILER4:                  /* promotional message for soft check */ 
    case CLASS_EPT_TRAILER:               /* EPT Logo Message */ 
        /* -- set destination CRT -- */
        PrtDflIf.Dfl.DflHead.auchCRTNo[0] = 0x30;
        PrtDflIf.Dfl.DflHead.auchCRTNo[1] = 0x30;

        /* -- set display data in the buffer -- */ 
        PrtDflIType(0, DFL_END); 

        switch (pItem->uchMinorClass) {
        case CLASS_TRAILER1:
        case CLASS_EPT_TRAILER:
            if (((pTran->TranCurQual.fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_CASHIER) ||       /* not Guest Check operation */
                (((pTran->TranCurQual.fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_ADDCHECK)
                 && !(pTran->TranCurQual.fsCurStatus & CURQUAL_CANCEL)) ||                       /* add check and not cancel operation */
                (((pTran->TranCurQual.fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_NEWCHECK)
                 && (pTran->TranCurQual.fsCurStatus & CURQUAL_CANCEL))) {       
                                                 /* new check and  cancel operation */
                PrtDflIf.Dfl.DflHead.uchFrameType = PRT_DFL_END_FRAME;
            } else {
                PrtDflIf.Dfl.DflHead.uchFrameType = PRT_DFL_SERVICE_END_FRAME;
            }
            break;

        default:
            PrtDflIf.Dfl.DflHead.uchFrameType = PRT_DFL_SERVICE_END_FRAME;
            break;
        }

        /* -- send display data -- */
        PrtDflSend();

    default:
        break;

    }
}

/*
*===========================================================================
** Format  : VOID  PrtDispPrint(TRANINFORMATION *pTran,ITEMPRINT *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMPRINT        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints  Header & Trailer.
*===========================================================================
*/
USHORT   PrtDispPrintForm(TRANINFORMATION  *pTran, ITEMPRINT  *pItem, TCHAR *puchBuffer)
{
    USHORT usLine;
    
    switch (pItem->uchMinorClass) {

    case CLASS_NUMBER: /* 2172 */
        usLine = PrtDflDispNumberForm(pTran, pItem, puchBuffer);
        break;
        
    case CLASS_AGELOG: /* 2172 */
        usLine = PrtDflAgeLogForm(pTran, pItem, puchBuffer);
        break;

    default:
        usLine = 0;
        break;
    }

    return usLine;
}

/*
*===========================================================================
** Format  : VOID PrtParkReceipt( TRANINFORMATION *pTran, ITEMPRINT *pItem )
*
*   Input  : TRANINFORMATION *pTran - transaction information
*            ITEMPRINT       *pItem - item information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints tranction void line. 
*===========================================================================
*/
VOID PrtParkReceipt( TRANINFORMATION *pTran, ITEMPRINT *pItem )
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion( pItem->fsPrintStatus );

    if ( fsPrtPrintPort & PRT_SLIP ) {
        PrtParkReceipt_SP( pTran );
    }

    if ( fsPrtPrintPort & ( PRT_JOURNAL | PRT_RECEIPT )) {
        PrtParkReceipt_RJ( pTran );
    }
}

/*
*===========================================================================
** Format  : VOID PrtParkReceipt_RJ( TRANINFORMATION *pTran )
*
*   Input  : TRANINFORMATION *pTran - transaction information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints parking receipt on receipt & journal.
*===========================================================================
*/
VOID  PrtParkReceipt_RJ( TRANINFORMATION *pTran )
{
	TCHAR   szMnemonic[PARA_TRANSMNEMO_LEN + 1] = { 0 };
    USHORT  usMnemLen;
    SHORT   sPortion;
                                            
    /* --- parking receipt is printed only once on journal --- */
    sPortion = PrtChkPort();

    if ( pTran->TranCurQual.fsCurStatus & CURQUAL_TRAY ) {
        switch ( sPortion ) {

        case PMG_PRT_RCT_JNL:
            sPortion = PMG_PRT_RECEIPT;
            break;

        case PMG_PRT_JOURNAL:
            sPortion = PRT_NOPORTION;
            break;

        default:
            break;
        }
    }

    /* --- retrieve parking receipt mnemonic --- */
	RflGetTranMnem( szMnemonic, TRN_PARKING_ADR );
    usMnemLen = _tcslen( szMnemonic );
    usMnemLen = ( usPrtRJColumn < usMnemLen ) ?  usPrtRJColumn : usMnemLen;

    PmgPrint( sPortion, szMnemonic, usMnemLen );
}

/*
*===========================================================================
** Format  : VOID PrtParkReceipt_SP( TRANINFORMATION *pTran )
*
*   Input  : TRANINFORMATION *pTran - transaction information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints parking receipt on slip.
*===========================================================================
*/
VOID PrtParkReceipt_SP( TRANINFORMATION *pTran )
{
	TCHAR   szMnemonic[PARA_TRANSMNEMO_LEN + 1] = { 0 };
    USHORT  usMnemLen;
    USHORT  usSlipLine = 0;             /* number of lines to be printed */
    USHORT  usSaveLine;                 /* save slip lines to be added */

    /* --- check if paper change is necessary or not --- */ 
    usSlipLine++;
    usSaveLine = PrtCheckLine( usSlipLine, pTran );

	RflGetTranMnem( szMnemonic, TRN_PARKING_ADR );
    usMnemLen = _tcslen( szMnemonic );
    PmgPrint( PMG_PRT_SLIP, szMnemonic, usMnemLen );

    /* --- update current line No. --- */
    usPrtSlipPageLine += ( usSlipLine + usSaveLine );
}

/*
*===========================================================================
** Format  : VOID  PrtNumber(TRANINFORMATION *pTran, ITEMPRINT *pItem);
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*            ITEMPRINT       *pItem    -item information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints number line of random weight label, 2172. 
*===========================================================================
*/
VOID  PrtNumber(TRANINFORMATION *pTran, ITEMPRINT *pItem)
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( fsPrtPrintPort & PRT_SLIP ) {              /* slip print */
        PrtNumber_SP(pTran, pItem);
    }

    if ( (fsPrtPrintPort & PRT_JOURNAL) || (fsPrtPrintPort & PRT_RECEIPT) ) {         /* journal print */
        PrtNumber_RJ(pTran, pItem);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtNumber_RJ(TRANINFORMATION *pTran);
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints number line. ( thermal )
*===========================================================================
*/
VOID  PrtNumber_RJ(TRANINFORMATION *pTran, ITEMPRINT *pItem)
{
    SHORT   sPortion;
	TCHAR   szMnemonic[PARA_TRANSMNEMO_LEN + 1] = { 0 };
    
    sPortion = PrtChkPort();
                                                
	RflGetTranMnem( szMnemonic, pItem->usFuncCode );
    
    PmgPrintf(PMG_PRT_RECEIPT, aszPrtRJMnemMnem, szMnemonic, pItem->aszNumber);
}

/*
*===========================================================================
** Format  : VOID  PrtNumber_SP(TRANINFORMATION *pTran);
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*            ITEMPRINT       *pItem    -item information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints number line. ( Slip )
*===========================================================================
*/
VOID PrtNumber_SP(TRANINFORMATION *pTran, ITEMPRINT *pItem)
{
	TCHAR   szMnemonic[PARA_TRANSMNEMO_LEN + 1] = { 0 };
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines to be added */
                                                
	RflGetTranMnem( szMnemonic, pItem->usFuncCode );
    
    /* -- check if paper change is necessary or not -- */ 
    usSaveLine = PrtCheckLine(usSlipLine, pTran);
    PmgPrintf(PMG_PRT_SLIP, aszPrtSPMnemMnem, szMnemonic, pItem->aszNumber);
    usSlipLine ++;

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;        

}

/*
*===========================================================================
** Format  : VOID  PrtDfDispNumber( TRANINFORMATION *pTran,
*                                     ITEMPRINT    *pItem )
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMPRINT        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays number of random weight label 2172.
*===========================================================================
*/
VOID PrtDflDispNumber( TRANINFORMATION *pTran, ITEMPRINT *pItem )
{
    USHORT  usLineNo;               /* number of lines to be displayed */
    USHORT  usOffset = 0;
    USHORT  i;
	TCHAR   aszDflBuff[6][PRT_DFL_LINE + 1] = { 0 };
	TCHAR   szMnemonic[PARA_TRANSMNEMO_LEN + 1] = { 0 };
                                                
    if ( pItem->aszNumber[0] == '\0' ) {
        return;
    }

	RflGetTranMnem( szMnemonic, pItem->usFuncCode );

    /* -- set header -- */
    usLineNo = PrtDflHeader( aszDflBuff[ 0 ], pTran );

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer( aszDflBuff[ usLineNo ], pTran, pTran->TranCurQual.ulStoreregNo );

    /* -- set item data -- */
    usLineNo += PrtDflRandomNumber( aszDflBuff[ usLineNo ], szMnemonic, pItem->aszNumber[0] ); //US Customs

    /* -- set destination CRT -- */
    PrtDflIf.Dfl.DflHead.auchCRTNo[0] = 0x30;
    PrtDflIf.Dfl.DflHead.auchCRTNo[1] = 0x30;

    /* -- set display data in the buffer -- */ 
    PrtDflIType( usLineNo, DFL_CUSTNAME );

    for ( i = 0; i < usLineNo; i++ ) {
        PrtDflSetData( aszDflBuff[ i ], &usOffset );
        if ( aszDflBuff[ i ][ PRT_DFL_LINE ] != '\0' ) {

            i++;
        }        
    }

    /* -- send display data -- */
    PrtDflSend();
}

/*
*===========================================================================
** Format  : VOID  PrtDfDispNumber( TRANINFORMATION *pTran,
*                                     ITEMPRINT    *pItem )
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMPRINT        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays number of random weight label 2172.
*===========================================================================
*/
USHORT PrtDflDispNumberForm( TRANINFORMATION *pTran, ITEMPRINT *pItem ,TCHAR *puchBuffer)
{
    USHORT  usLineNo=0, i;               /* number of lines to be displayed */
	TCHAR   aszDflBuff[6][PRT_DFL_LINE + 1] = { 0 };
	TCHAR   szMnemonic[PARA_TRANSMNEMO_LEN + 1] = { 0 };
                                                
    if ( pItem->aszNumber[0] == '\0' ) {
        return 0;
    }

	RflGetTranMnem( szMnemonic, pItem->usFuncCode );

    /* -- set header -- */
#if 0
    usLineNo = PrtDflHeader( aszDflBuff[ 0 ], pTran );

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer( aszDflBuff[ usLineNo ], pTran, pTran->TranCurQual.ulStoreregNo );
#endif
    /* -- set item data -- */
    usLineNo += PrtDflRandomNumber( aszDflBuff[ usLineNo ], szMnemonic, pItem->aszNumber[0] );//US Customs

    for (i=0; i<usLineNo; i++) {
        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }
    _tcsncpy(puchBuffer, aszDflBuff[0], usLineNo*(PRT_DFL_LINE+1));
    
    return usLineNo;
}

/*
*===========================================================================
** Format  : VOID  PrtAgeLog(TRANINFORMATION *pTran, ITEMPRINT *pItem);
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*            ITEMPRINT       *pItem    -item information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints boundary age log, 2172. 
*===========================================================================
*/
VOID  PrtAgeLog(TRANINFORMATION *pTran, ITEMPRINT *pItem)
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( fsPrtPrintPort & PRT_JOURNAL ) {           /* electric journal */
        PrtAgeLog_J(pTran, pItem);
    }

    return;

}

/*
*===========================================================================
** Format  : VOID  PrtAgeLog_EJ(TRANINFORMATION *pTran);
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints boundary age log. ( elctric journal )
*===========================================================================
*/
VOID  PrtAgeLog_J(TRANINFORMATION *pTran, ITEMPRINT *pItem)
{
    USHORT usMnemLen;

    usMnemLen = _tcslen( pItem->aszNumber[0] );//US Customs
    
    PmgPrint( PMG_PRT_JOURNAL, pItem->aszNumber[0], usMnemLen );//US Customs
}


/*
*===========================================================================
** Format  : VOID  PrtDflAgeLog( TRANINFORMATION *pTran,
*                                     ITEMPRINT    *pItem )
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMPRINT        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays boundary age log 2172.
*===========================================================================
*/
VOID PrtDflAgeLog( TRANINFORMATION *pTran, ITEMPRINT *pItem )
{
    TCHAR   aszDflBuff[ 6 ][ PRT_DFL_LINE + 1 ];
    USHORT  usLineNo;               /* number of lines to be displayed */
    USHORT  usOffset = 0;
    USHORT  i;
                                                
    if ( pItem->aszNumber[0] == '\0' ) {
        return;
    }

    memset( aszDflBuff, 0x00, sizeof( aszDflBuff ));

    /* -- set header -- */
    usLineNo = PrtDflHeader( aszDflBuff[ 0 ], pTran );

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer( aszDflBuff[ usLineNo ], pTran, pTran->TranCurQual.ulStoreregNo );

    /* -- set item data -- */
    usLineNo += PrtDflMnemonic( aszDflBuff[ usLineNo ], pItem->aszNumber[0] );//US Customs

    /* -- set destination CRT -- */
    PrtDflIf.Dfl.DflHead.auchCRTNo[0] = 0x30;
    PrtDflIf.Dfl.DflHead.auchCRTNo[1] = 0x30;

    /* -- set display data in the buffer -- */ 
    PrtDflIType( usLineNo, DFL_CUSTNAME );

    for ( i = 0; i < usLineNo; i++ ) {
        PrtDflSetData( aszDflBuff[ i ], &usOffset );
        if ( aszDflBuff[ i ][ PRT_DFL_LINE ] != '\0' ) {
            i++;
        }        
    }

    /* -- send display data -- */
    PrtDflSend();
}

/*
*===========================================================================
** Format  : VOID  PrtDflAgeLog( TRANINFORMATION *pTran,
*                                     ITEMPRINT    *pItem )
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMPRINT        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays boundary age log 2172.
*===========================================================================
*/
USHORT PrtDflAgeLogForm( TRANINFORMATION *pTran, ITEMPRINT *pItem, TCHAR *puchBuffer)
{
    TCHAR   aszDflBuff[ 6 ][ PRT_DFL_LINE + 1 ];
    USHORT  usLineNo=0, i;               /* number of lines to be displayed */
                                                
    if ( pItem->aszNumber[0] == '\0' ) {
        return 0;
    }

    memset( aszDflBuff, 0x00, sizeof( aszDflBuff ));

    /* -- set header -- */
#if 0
    usLineNo = PrtDflHeader( aszDflBuff[ 0 ], pTran );

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer( aszDflBuff[ usLineNo ], pTran, pTran->TranCurQual.ulStoreregNo );
#endif
    /* -- set item data -- */
    usLineNo += PrtDflMnemonic( aszDflBuff[ usLineNo ], pItem->aszNumber[0] );//US Customs

    for (i=0; i<usLineNo; i++) {
        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }
    _tcsncpy(puchBuffer, aszDflBuff[0], usLineNo*(PRT_DFL_LINE+1));
    
    return usLineNo;
}

/*
*===========================================================================
** Format  : VOID  PrtCPRespText(TRANINFORMATION *pTran, ITEMPRINT *pItem)
*
*   Input  : TRNINFORMATION  *pTran,    -transaction information
*          : ITEMPRINT       *pItem     -item information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints EPT Response Message Text,    Saratoga
*===========================================================================
*/
VOID    PrtCPRespText(TRANINFORMATION *pTran, ITEMPRINT *pItem)
{
    USHORT  fsPrtStat;

    if (fsPrtStatus & PRT_REQKITCHEN) {         /* kitchen print */
        return;
    }

    fsPrtStat = pItem->fsPrintStatus;

    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if (fsPrtStat & PRT_RECEIPT || fsPrtStat & PRT_JOURNAL) {
        PrtRJCPRespText(pItem);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtRJCPRespText(ITEMPRNT *pItem);
*
*   Input  : ITEMPRINT  *pItem
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints EPT response message,         Saratoga
*===========================================================================
*/
VOID    PrtRJCPRespText(ITEMPRINT *pItem)
{
    TCHAR   *puchStart, aszPrint[NUM_CPRSPTEXT + 1];
    USHORT  i;

    if (pItem->uchPrintSelect != 0) {
        puchStart = (TCHAR *)pItem->aszCPText;
        for (i = 0; i < pItem->uchCPLineNo; i++) {
            memset(aszPrint, 0, sizeof(aszPrint));
            memcpy(aszPrint, puchStart + i * pItem->uchPrintSelect, pItem->uchPrintSelect);
            PrtRJCPRspMsgText(aszPrint);
        }
    }
}

/*
*===========================================================================
** Format  : VOID  PrtFSCredit(TRANINFORMATION  *pTran, ITEMPRINT  *pItem)
*
*   Input  :  TRNINFORMATION  *pTran,
*             ITEMPRINT  *pItem
*
*   Output : none
*   InOut  : none
*
** Return  :
*
** Synopsis:
*
*===========================================================================
*/
VOID  PrtFSCredit(TRANINFORMATION *pTran, ITEMPRINT *pItem)
{
    USHORT  usRet = PRT_FAIL;

    PrtPortion(pItem->fsPrintStatus);
    if (pItem->fsPrintStatus & PRT_VALIDATION) {         /* validation print */
        PrtFSCredit_VL( pTran, pItem );

    } else if (pItem->fsPrintStatus & (PRT_RECEIPT | PRT_SPCL_PRINT)) {    /* Chit */
		TCHAR   aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };

		RflGetTranMnem(aszTranMnem, TRN_FSCRD_ADR);
        PmgPrintf(PMG_PRT_RECEIPT, aszPrtRJAmtMnem, aszTranMnem, pItem->lAmount);

    } else if ((pItem->fsPrintStatus & PRT_RECEIPT) 
        || (pItem->fsPrintStatus & PRT_JOURNAL)) {
        PrtFSCredit_RJ( pItem );
    }
}

/*
*===========================================================================
** Format  : VOID  PrtFSCredit_RJ(ITEMPRINT  *pItem);
*
*   Input  :
*
*   Output : none
*   InOut  : none
*
** Return  :
*
** Synopsis:
*
*===========================================================================
*/
VOID  PrtFSCredit_RJ( ITEMPRINT *pItem )
{
    PrtRJZeroAmtMnem(TRN_FSCRD_ADR, pItem->lAmount);            /* F.S credit */
}

/*
*===========================================================================
** Format  : VOID  PrtFSCredit_VL(TRANINFORMATION  *pTran, ITEMPRINT  *pItem);
*
*   Input  :  TRNINFORMATION  *pTran
*
*   Output : none
*   InOut  : none
*
** Return  :
*
** Synopsis:
*
*===========================================================================
*/
VOID  PrtFSCredit_VL(TRANINFORMATION *pTran, ITEMPRINT *pItem)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };

	RflGetTranMnem(aszTranMnem, TRN_FSCRD_ADR);

    PmgBeginValidation(PMG_PRT_RCT_JNL);        /* begin validation */
    PrtVLHead(pTran, 0);
    PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVLFSCredit, aszTranMnem, pItem->lAmount);
    PrtVLTrail(pTran);
    PrtEndValidation(PMG_PRT_RCT_JNL);          /* end validation */
}


/* dollar tree */

/*
*===========================================================================
** Format  : VOID  PrtEndorse(TRNINFORMATION  *pTran, ITEMPRINT  *pItem);
*
*   Input  :  TRNINFORMATION  *pTran,
*             ITEMPRINT  *pItem
*
*   Output : none
*   InOut  : none
*
** Return  : none
*
** Synopsis: This function prints check endorsement on validation or slip.
*
*===========================================================================
*/
VOID  PrtEndorse( TRANINFORMATION  *pTran, ITEMPRINT  *pItem)
{
    USHORT   usRet = PRT_FAIL;

    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if (pItem->fsPrintStatus & PRT_VALIDATION) {         /* validation print */

		PrtEndorse_VL( pTran, pItem );

    }
}

/*
*===========================================================================
** Format  : VOID  PrtEndorse_VL(TRNINFORMATION  *pTran, ITEMPRINT  *pItem);
*
*   Input  :  TRNINFORMATION  *pTran,
*             ITEMPRINT  *pItem
*
*   Output : none
*   InOut  : none
*
** Return  : none
*
** Synopsis: This function prints check endorsement on validation horizontally
*            or vertically.
*
*
*===========================================================================
*/
VOID  PrtEndorse_VL( TRANINFORMATION  *pTran, ITEMPRINT  *pItem)
{
    UCHAR  uchAdr = 1;

    if ( usPrtVLColumn == PRT_VL_21 ) {
        return;
    }

    PmgBeginValidation(PMG_PRT_RCT_JNL);        /* begin validation */

    if (pItem->uchStatus <= CLASS_TENDER8) {
        uchAdr = (UCHAR)(TRN_TEND1_ADR + pItem->uchStatus - CLASS_TENDER1);
    } else if ((pItem->uchStatus >= CLASS_TENDER9) && (pItem->uchStatus <= CLASS_TENDER11)) {
        uchAdr = (UCHAR)(TRN_TEND9_ADR + pItem->uchStatus - CLASS_TENDER9);
    } else if ((pItem->uchStatus >= CLASS_TENDER12) && (pItem->uchStatus <= CLASS_TENDER20)) {
		uchAdr = (UCHAR)(TRN_TENDER12_ADR + pItem->uchStatus - CLASS_TENDER12);
	}

    /* -- in case of 24 char printer -- */

    if ( usPrtVLColumn == PRT_VL_24 ) {
        /* -- trainin cashier  -- */
	    if ( (pTran->TranCurQual.fsCurStatus) & CURQUAL_TRAINING ) {

            PrtVLEndosTrngHead(pItem->uchPrintSelect);
        } else {
            PrtVLEndosHead(pItem->uchPrintSelect);    /* send header lines */
        }
        PrtVLEndosMnemAmt(pItem->uchPrintSelect, TRN_AMTTL_ADR, pItem->lMI);
        PrtVLEndosMnemCSAmt(pItem->uchPrintSelect, uchAdr, pItem->lAmount);
        PrtVLEndosTrail(pItem->uchPrintSelect, pTran);  /* send trailer lines */
    }

    PrtEndValidation(PMG_PRT_RCT_JNL);          /* end validation */
}

/****** End of Source ******/