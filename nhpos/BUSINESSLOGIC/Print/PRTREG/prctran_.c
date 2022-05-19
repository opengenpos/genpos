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
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Transacition Print
* Category    : Print Module, NCR 2170 US Hospitality  application
* Program Name: PrCTran_.c
* --------------------------------------------------------------------------
* Abstract:
:# =  modified or added function at release 3.0
*   PrtInititalize()        : initialize static area
*   PrtPrintTran()          : print transaction
*   PrtPrintSplit()         : print transaction for split guest check
*#  PrtChkStorage()         : check print storage
*#  PrtCallIPD()            : call item print function
*#  PrtIsItemInWorkBuff()   : check item data is in work buffer
*#  PrtIsSalesIDisc()       : check item data is sales with item disc
*#  PrtIsIDiscInWorkBuff()  : check item data is in work buffer
*   PrtPrintDemand()        : print on demand
*   PrtSetSalesCouponQTY()  : set coupon qty for void consolidation
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
* May-15-92 : 00.00.01 : K.Nakajima :
* Jun-24-93 : 01.00.12 : K.You      : add soft check feature.(mod. PrtPrintTran)
* Mar-07-94 : 00.00.04 : K.You      : add Flex GC File Feature.(mod. PrtPrintTran,
*           :          :            : PrtChkStorage, PrtCallIPD)
* May-20-94 : 02.05.00 : K.You      : bug fixed S-23 (mod. PrtCallIPD)
* May-24-94 : 02.05.00 : Yoshiko.Jim: Dupli.Print for CP,EPT (add PRT_CONSOL_CP_EPT)
* May-25-94 : 02.05.00 : K.You      : stop to bug fix S-23 (mod. PrtCallIPD)
* Apr-18-95 : 03.00.00 : T.Nakahata : add total mnemonic to kitchen feature
* Jun-21-95 : 03.00.00 : T.Nakahata : add print priority feature
* Jul-25-95 : 03.00.04 : T.Nakahata : print on journal by service total key
* Aug-28-95 : 03.00.04 : T.Nakahata : no-consolidate by total key option
*    -  -   : 03.01.00 : M.Ozawa    : add split guest check print
* May-28-96 : 03.01.07 : M.Ozawa    : add PrtKitchenCallIPD() for kp by seat #
* Jan-06-98 : 03.01.16 : M.Ozawa    : add non-consolidated kp print by seat no.
* Aug-14-98 : 03.03.00 : M.Ozawa    : remove PrtKitchenCallIPD() for non-cons kp print
* Aug-16-99 : 03.05.00 : M.Teraki   : Merge STORE_RECALL_MODEL/GUEST_CHECK_MODEL
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
* 2172
* Jan-07-00 : 01.00.00 :  H.Endo    : use macro (PrtCallIPD())
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

/** ---- M S   -  C ---- **/
#include	<tchar.h>
#include<memory.h>
#include<string.h>
/** ---- 2170 local ---- **/
#include<ecr.h>
#include<paraequ.h>
/* #include<para.h> */
#include<regstrct.h>
#include<transact.h>
#include<trans.h>
#include<pmg.h>
#include<prt.h>
#include<rfl.h>
/* #include<uie.h> */
#include<prtprty.h>
#include"prtrin.h"
#include "pif.h"

/**
;========================================================================
;+        P R O G R A M    D E C L A R A T I O N s
;========================================================================
**/
/*
*===========================================================================
** Format  : USHORT   PrtInitialize(VOID);      
*               
*    Input : none
*   Output : none
*    InOut : none
** Return  : none       
*
** Synopsis: This function initialize static area R3.1
*===========================================================================
*/
VOID    PrtInitialize(VOID)
{
    /* ---- initialize static area at master reset ---- */
    fsPrtStatus = 0;
	fbPrtSendToStatus = 0; //send prints to printer //ESMITH PRTFILE
	fchPrintDown= 0;		/* save status of printer up or down */
    memset (auchPrtSeatCtl, 0, sizeof(auchPrtSeatCtl));
}

/*
*===========================================================================
** Format  : USHORT PrtPrintTran(TRANINFORMATION *pTran);
*
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*   Output : none
*    InOut : none
** Return  : USHORT    sSlipLine         -current printed pages and lines
*
** Synopsis: This function prints transaction data.
*===========================================================================
*/
USHORT PrtPrintTran(TRANINFORMATION *pTran)
{
    SHORT   sRet, i;
    SHORT   hsRead1, hsRead2;           /* storage file handle */
    UCHAR   uchTotal;

    /* ---- not clear static area at split check printing ---- */
    if (!(fsPrtStatus & PRT_SPLIT_PRINT) || !auchPrtSeatCtl[0]) {
        fsPrtStatus    = 0;                 /* initialize control area */
        fsPrtNoPrtMask = 0;
        fsPrtCompul    = 0;
        usPrtSlipPageLine = 0;              /* clear current slip's pages and lines */

        memset (auchPrtSeatCtl, 0, sizeof(auchPrtSeatCtl));
    }
    uchPrtSeatNo = 0;
    fbPrtKPSeatState = 0xFF;                /* not feed first item at non seat# trans. */

    memset(auchPrtFailKP, '\0', sizeof(auchPrtFailKP));    /* fail kitchen printer no initial. */

    /* -- check storage pointer -- */
    if ( PRT_NOPORTION == (sRet = PrtChkStorage(&hsRead1, &hsRead2, pTran)) ) {
        fsPrtStatus    = 0;                 /* initialize control area */
        fsPrtNoPrtMask = 0;
        fsPrtCompul    = 0;
        usPrtSlipPageLine = 0;              /* clear current slip's pages and lines */

        memset (auchPrtSeatCtl, 0, sizeof(auchPrtSeatCtl));
        return (0);
    }

    /* -- decompress data and call IPD  -- */
    /* V3.3, ICI */
    uchTotal = pTran->TranCurQual.auchTotalStatus[ 4 ];

    if (fsPrtStatus  & ( PRT_KPONLY | PRT_REQKITCHEN )) {
        PrtCallIPD(pTran, hsRead1);
    } else {
        /* split check, V3.3 ICI */
        if ((pTran->TranCurQual.flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK
            && !CliParaMDCCheck( MDC_GCNO_ADR, EVEN_MDC_BIT3)
            && !CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2)) {
            /*----- Not Use "Not Consoli Print" Option -----*/
            pTran->TranCurQual.auchTotalStatus[ 4 ] &= ~CURQUAL_TOTAL_NOCONSOLIDATE_PRINT;
        }

        fsPrtStatus |= PRT_REC_SLIP;
        PrtCallIPD(pTran, hsRead1);
    }

    pTran->TranCurQual.auchTotalStatus[ 4 ] = uchTotal;
    /* V3.3, ICI */

    if ( PRT_DIFFBUFF == sRet ) {
        fsPrtNoPrtMask = 0;
        fsPrtCompul    = 0;
        fsPrtStatus    = PRT_KPONLY;
        fsPrtStatus   |= PRT_REQKITCHEN;      /* set "request kitchen" */
        uchPrtSeatNo = 0;                       /* continuous seat no print */
        fbPrtKPSeatState = 0xFF;                /* not feed first item at non seat# trans. */

        hsRead1 = hsRead2;                    /* copy file handle */
                                              /* for take to kitchen */
        /* -- decompress data and call IPD (for kitchen) -- */
        PrtCallIPD(pTran, hsRead1);
    }

    /* -- take to kitchen management -- */
    for (i=0; i<PRT_KPRT_MAX; i++) {    /* check 8 printers */
        if (auchPrtFailKP[i]) break;
    }
    if (i != PRT_KPRT_MAX) {
        PrtTakeToKP(pTran, hsRead1);
    }

    PrtCompNo.fsComp=fsPrtCompul;           /* used soft check print */
    PrtCompNo.fsNo=fsPrtNoPrtMask;

    fsPrtStatus    = 0;                     /* initialize control area */
    fsPrtNoPrtMask = 0;
    fsPrtCompul    = 0;
    memset (auchPrtSeatCtl, 0, sizeof(auchPrtSeatCtl));

    /* V3.3, ICI */
    pTran->TranCurQual.auchTotalStatus[ 4 ] = uchTotal;

    return (usPrtSlipPageLine);
}

/*
*===========================================================================
** Format  : USHORT PrtPrintSplit(TRANINFORMATION *pTran, TRANINFSPLIT *pSplit);
*
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*          : TRANINFSPLIT     *pSplit    -Split GCF information address
*   Output : none
*    InOut : none
** Return  : USHORT    sSlipLine         -current printed pages and lines
*
** Synopsis: This function prints split guest check data.
*===========================================================================
*/
USHORT PrtPrintSplit(TRANINFORMATION *pTran, TRANINFSPLIT *pSplit,
                                                USHORT usType, USHORT usNo)
{
    SHORT           hsTranConsStorage;      /* consoli storage file handle */
    USHORT          usTranConsStoFSize;     /* consoli storage file size */
    USHORT          usTranConsStoVli;       /* consoli storage Vli */
    SHORT           hsTranConsIndex;        /* handle of cons. storage index */
    SHORT           hsTranNoConsIndex;      /* handle of no cons. storage index */
    UCHAR           auchFinSeat[NUM_SEAT];  //SR206
    ULONG           fsCurStatus;
    SHORT   sRet;
    SHORT   hsRead1, hsRead2;           /* storage file handle */
    UCHAR           uchTotal;

    if (!usNo) {
        fsPrtStatus    = 0;                 /* initialize control area */
        fsPrtNoPrtMask = 0;
        fsPrtCompul    = 0;
/*        usPrtSlipPageLine = 0;              / clear current slip's pages and lines */
    }
    uchPrtSeatNo = 0;

    fsPrtStatus |= PRT_SPLIT_PRINT;

	NHPOS_ASSERT(sizeof(pTran->TranGCFQual.auchFinSeat) >= sizeof(auchFinSeat));

    /* -- save transaction information data to temporary buffer */
    hsTranConsStorage = pTran->hsTranConsStorage;
    usTranConsStoFSize = pTran->usTranConsStoFSize;
    usTranConsStoVli = pTran->usTranConsStoVli;
    hsTranConsIndex = pTran->hsTranConsIndex;
    hsTranNoConsIndex = pTran->hsTranNoConsIndex;
    fsCurStatus = pTran->TranCurQual.fsCurStatus;
    memcpy (&auchFinSeat, pTran->TranGCFQual.auchFinSeat, sizeof(auchFinSeat));

    /* -- change transaction information to split informatin data */
    pTran->hsTranConsStorage = pSplit->hsTranConsStorage;
    pTran->usTranConsStoFSize = pSplit->usTranConsStoFSize;
    pTran->usTranConsStoVli = pSplit->usTranConsStoVli;
    pTran->hsTranConsIndex = pSplit->hsTranConsIndex;
    pTran->hsTranNoConsIndex = pSplit->hsTranConsIndex; /* use only consoli. index */
    memcpy (pTran->TranGCFQual.auchFinSeat, pSplit->TranGCFQual.auchFinSeat, sizeof(auchFinSeat));

    if ((pTran->TranCurQual.fsCurStatus & CURQUAL_OPEMASK) != CURQUAL_ADDCHECK) { /* new/reorder */
        pTran->TranCurQual.fsCurStatus &= ~CURQUAL_OPEMASK;
        pTran->TranCurQual.fsCurStatus |= CURQUAL_NEWCHECK;      /* not print check sum line */
        pTran->TranCurQual.fbNoPrint |= PRT_JOURNAL;   /* not print on journal */
    }

    if ((pTran->TranGCFQual.auchUseSeat[0]) && (usType)) {       /* condolidated seat# print */
        auchPrtSeatCtl[0] = 1;  /* consolidaton seat no print */
        auchPrtSeatCtl[1] = (UCHAR)usNo;
        if (pSplit->TranGCFQual.usGuestNo == 0) {
            auchPrtSeatCtl[1] = 1;
        }
    } else {
        auchPrtSeatCtl[0] = 0;
        auchPrtSeatCtl[1] = (UCHAR)usNo;
    }

    /* -- check storage pointer -- */
    if ( PRT_NOPORTION == (sRet = PrtChkStorage(&hsRead1, &hsRead2, pTran)) ) {
        /* -- restore transaction information data */
        pTran->hsTranConsStorage = hsTranConsStorage;
        pTran->usTranConsStoFSize = usTranConsStoFSize;
        pTran->usTranConsStoVli = usTranConsStoVli;
        pTran->hsTranConsIndex = hsTranConsIndex;
        pTran->hsTranNoConsIndex = hsTranNoConsIndex;
        pTran->TranCurQual.fsCurStatus = fsCurStatus;
        memcpy (pTran->TranGCFQual.auchFinSeat, auchFinSeat, sizeof(auchFinSeat));

        return (0);
    }

    /*----- Not Use "Not Consoli Print" Option, V3.3 ICI -----*/
    uchTotal = pTran->TranCurQual.auchTotalStatus[ 4 ];
    pTran->TranCurQual.auchTotalStatus[ 4 ] &= ~CURQUAL_TOTAL_NOCONSOLIDATE_PRINT;

    /* -- decompress data and call IPD  -- */
    if ((usType) || (pSplit->TranGCFQual.usGuestNo) || (pSplit->TranItemizers.lMI)) {
        fsPrtStatus |= PRT_REC_SLIP;
        PrtCallIPD(pTran, hsRead1);
    }

    PrtCompNo.fsComp=fsPrtCompul;           /* used soft check print */
    PrtCompNo.fsNo=fsPrtNoPrtMask;

    /* -- restore transaction information data */
    pTran->hsTranConsStorage = hsTranConsStorage;
    pTran->usTranConsStoFSize = usTranConsStoFSize;
    pTran->usTranConsStoVli = usTranConsStoVli;
    pTran->hsTranConsIndex = hsTranConsIndex;
    pTran->hsTranNoConsIndex = hsTranNoConsIndex;
    pTran->TranCurQual.fsCurStatus = fsCurStatus;
    memcpy (pTran->TranGCFQual.auchFinSeat, auchFinSeat, sizeof(auchFinSeat));

    if ((pSplit->TranGCFQual.usGuestNo == 0) &&
        ((pTran->TranCurQual.fsCurStatus & CURQUAL_OPEMASK) != CURQUAL_ADDCHECK)) { /* new/reorder */

        fsPrtStatus    = 0;                 /* initialize control area */
        fsPrtNoPrtMask = 0;
        fsPrtCompul    = 0;
        memset (auchPrtSeatCtl, 0, sizeof(auchPrtSeatCtl));
    }

    /* V3.3, ICI */
    pTran->TranCurQual.auchTotalStatus[ 4 ] = uchTotal;

    return (usPrtSlipPageLine);
}

/*
*===========================================================================
** Format  : SHORT  PrtChkStorage(SHORT *hsRead1, SHORT *hsRead2,
*                                                TRANINFORMATION *pTran);
*
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*   Output : SHORT            *hsRead1   -storage file handle save area
*          : SHORT            *hsRead2   -storage file handle save area
*    InOut : none
** Return  : SHORT            PRT_NOBUFF      - no storage
*                             PRT_SAMEBUFF    - use same storage
*                             PRT_DIFFBUFF    - use diffent storage
*                             PRT_ONLY1BUFF   - only 1 storage available
*
** Synopsis: This function gets storage file handle.
*            When both kitchen printer and other printer are requested,
*            TPM prints sends data to kitchen printer, then sends to other printer.
*===========================================================================
*/
SHORT PrtChkStorage( SHORT *hsRead1, SHORT *hsRead2, TRANINFORMATION *pTran )
{
    UCHAR  uchPrtStorage = pTran->TranCurQual.uchPrintStorage;
    UCHAR  uchKitStorage = pTran->TranCurQual.uchKitchenStorage;
    ULONG  fulTrnPrtSts  = pTran->TranCurQual.flPrintStatus;
    ULONG  fusTrnCurSts  = pTran->TranCurQual.fsCurStatus;

    if (uchKitStorage == PRT_KIT_NET_STORAGE) { /* R3.1 */
        uchKitStorage = PRT_ITEMSTORAGE;
    }

    if (( uchPrtStorage == PRT_NOSTORAGE) && ( uchKitStorage == PRT_NOSTORAGE )){
		/* -- no print -- */
       return ( PRT_NOPORTION );
    }  else if (( uchPrtStorage == PRT_ITEMSTORAGE ) && ( uchKitStorage == PRT_ITEMSTORAGE )) {
		/* -- kitchen & other printer / same buffer -- */

/*        fsPrtStatus = PRT_REQKITCHEN; */
        *hsRead1    = pTran->hsTranItemStorage;
        *hsRead2    = pTran->hsTranItemStorage;

        if ((( fulTrnPrtSts & CURQUAL_GC_SYS_MASK ) == CURQUAL_POSTCHECK ) &&
            ((( fusTrnCurSts & CURQUAL_OPEMASK ) != CURQUAL_ADDCHECK ) &&
             (( fusTrnCurSts & CURQUAL_OPEMASK ) != CURQUAL_CASHIER ))) {

            fsPrtStatus |= PRT_SERV_JOURNAL;
        } else if ((( fulTrnPrtSts & CURQUAL_GC_SYS_MASK ) == CURQUAL_STORE_RECALL ) &&
            (( fusTrnCurSts & CURQUAL_OPEMASK ) != CURQUAL_ADDCHECK )) {
            fsPrtStatus |= PRT_SERV_JOURNAL;
        }

        if ( pTran->TranCurQual.flPrintStatus & ( CURQUAL_POSTRECT | CURQUAL_PARKING )) {
            fsPrtStatus &= ~PRT_SERV_JOURNAL;   /* not print journal at post receipt */
        }

        return ( PRT_DIFFBUFF );    /* for different format between RJ and kitchen R3.1 */
/*        return ( PRT_SAMEBUFF ); */

    /* -- kitchen & other printer -- */
    }  else if (( uchPrtStorage == PRT_CONSOLSTORAGE ) && ( uchKitStorage == PRT_ITEMSTORAGE   )) {
        *hsRead2 = pTran->hsTranItemStorage;
        if (( fulTrnPrtSts & CURQUAL_GC_SYS_MASK ) == CURQUAL_POSTCHECK) {
            if (( fusTrnCurSts & CURQUAL_OPEMASK ) != CURQUAL_CASHIER ) {
                *hsRead1 = pTran->hsTranConsStorage;
            } else {
                *hsRead1 = pTran->hsTranItemStorage;
            }
        } else if (( fulTrnPrtSts & CURQUAL_GC_SYS_MASK ) == CURQUAL_STORE_RECALL) {
            *hsRead1 = pTran->hsTranConsStorage;
        } else {    /* precheck system */
            *hsRead1 = pTran->hsTranItemStorage;
        }

        if ((( fulTrnPrtSts & CURQUAL_GC_SYS_MASK ) == CURQUAL_POSTCHECK ) &&
            ((( fusTrnCurSts & CURQUAL_OPEMASK ) != CURQUAL_ADDCHECK ) &&
             (( fusTrnCurSts & CURQUAL_OPEMASK ) != CURQUAL_CASHIER ))) {
            fsPrtStatus |= PRT_SERV_JOURNAL;
        } else if ((( fulTrnPrtSts & CURQUAL_GC_SYS_MASK ) == CURQUAL_STORE_RECALL ) &&
            (( fusTrnCurSts & CURQUAL_OPEMASK ) != CURQUAL_ADDCHECK )) {
            fsPrtStatus |= PRT_SERV_JOURNAL;
        }

        if ( pTran->TranCurQual.flPrintStatus & ( CURQUAL_POSTRECT | CURQUAL_PARKING )) {
            fsPrtStatus &= ~PRT_SERV_JOURNAL;   /* not print journal at post receipt */
        }

        return ( PRT_DIFFBUFF );

    /* -- kitchen -- */
    }  else if ( uchPrtStorage == PRT_NOSTORAGE ) {

        /*  -- set "request kitchen" -- */
        fsPrtStatus  = ( PRT_KPONLY | PRT_REQKITCHEN );
        *hsRead1     = pTran->hsTranItemStorage;
        if ((( fulTrnPrtSts & CURQUAL_GC_SYS_MASK ) == CURQUAL_POSTCHECK ) &&
            ((( fusTrnCurSts & CURQUAL_OPEMASK ) != CURQUAL_ADDCHECK ) &&
             (( fusTrnCurSts & CURQUAL_OPEMASK ) != CURQUAL_CASHIER ))) {

            if (CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT3) || CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2)) {
                fsPrtStatus |= PRT_SERV_JOURNAL;
            } else if (!(pTran->TranCurQual.auchTotalStatus[ 2 ] & CURQUAL_TOTAL_PRINT_BUFFERING)) {
                /* set journal print status at split check not execute print */
                fsPrtStatus |= PRT_SERV_JOURNAL;
            }
        } else if ((( fulTrnPrtSts & CURQUAL_GC_SYS_MASK ) == CURQUAL_STORE_RECALL ) &&
            (( fusTrnCurSts & CURQUAL_OPEMASK ) != CURQUAL_ADDCHECK )) {
            fsPrtStatus |= PRT_SERV_JOURNAL;
        }

        if ( pTran->TranCurQual.flPrintStatus & ( CURQUAL_POSTRECT | CURQUAL_PARKING )) {
            fsPrtStatus &= ~PRT_SERV_JOURNAL;   /* not print journal at post receipt */
        }

        return ( PRT_1PORTION );

    /* -- except kitchen -- */
    }  else if ( uchKitStorage == PRT_NOSTORAGE ) {
        if (( uchPrtStorage == PRT_CONSOLSTORAGE ) || ( uchPrtStorage == PRT_CONSOL_CP_EPT )) {
            /* -- case of print on demand, postcheck -- */
            *hsRead1 = pTran->hsTranConsStorage;
        } else {
            /* -- only case of print on demand, precheck -- */
            *hsRead1 = pTran->hsTranItemStorage;
        }

        if ((( fulTrnPrtSts & CURQUAL_GC_SYS_MASK ) == CURQUAL_POSTCHECK ) &&
            ((( fusTrnCurSts & CURQUAL_OPEMASK ) != CURQUAL_ADDCHECK ) &&
             (( fusTrnCurSts & CURQUAL_OPEMASK ) != CURQUAL_CASHIER ))) {

            fsPrtStatus |= PRT_SERV_JOURNAL;
        } else if ((( fulTrnPrtSts & CURQUAL_GC_SYS_MASK ) == CURQUAL_STORE_RECALL ) &&
            (( fusTrnCurSts & CURQUAL_OPEMASK ) != CURQUAL_ADDCHECK )) {
            fsPrtStatus |= PRT_SERV_JOURNAL;
        }

        if ( pTran->TranCurQual.flPrintStatus & ( CURQUAL_POSTRECT | CURQUAL_PARKING )) {
            fsPrtStatus &= ~PRT_SERV_JOURNAL;   /* not print journal at post receipt */
        }

        return ( PRT_1PORTION );
    }
    return PRT_NOPORTION; /* ### Add (2171 for Win32) V1.0 */
}
/*
*===========================================================================
** Format   : VOID PrtCallIPD( TRANINFORMATION *pTran, SHORT hsStorage )
*
*    Input  : TRANINFORMATION *pTran    - address of transaction information
*             SHORT           hsStorage - handle of transaction storage
*   Output  : none
*    InOut  : none
** Return   : none
*
** Synopsis : This function retrieves an item data in the specified storage,
*             and then calls IPD (Item Print Distributor) for printing item.
*===========================================================================
*/
VOID PrtCallIPD( TRANINFORMATION *pTran, SHORT hsStorage )
{
    SHORT           hsIndexFile;
    ULONG           ulStorageActSize;
    UCHAR           auchIdxWork[ sizeof( PRTIDX ) * FLEX_ITEMSTORAGE_MIN ];
/*    UCHAR           auchIdxWork[ sizeof( PRTIDX ) * (FLEX_ITEMSTORAGE_MIN+PRTIDX_RESERVE) ]; */
    PRTIDXHDR       IdxFileInfo;
    PRTIDX          *pPrtIdxHighest;
    PRTIDX          *pPrtIdxCurrent;
    UCHAR           uchItemCo;
    ULONG           ulStorageReadOffset;
    ULONG           ulStorageReadSize;
    /*UCHAR           auchStorageWork[ 512 ];*/
    TCHAR           auchStorageWork[ TRN_TEMPBUFF_SIZE ];   /* endo update(fig->macro) 2000/1/7 */
    ULONG           ulItemWorkOffset;
    UCHAR           auchItemData[ sizeof (ITEMDATASIZEUNION) ];
    ULONG           ulItemReadLen;
    ITEMSALES       *pItemSales;
    ITEMDISC        *pItemDisc;
    SHORT           sQTYSave;
    ULONG           ulTtlIdxReadSize;
    ULONG           ulCurIdxReadSize;
    UCHAR           uchTtlItemCo;
    UCHAR           uchCurItemCo;
    BOOL            fConsoli;
	ULONG			ulActualBytesRead;

    /* --- determine total key status for consolidate print option */
    fConsoli = ( pTran->TranCurQual.auchTotalStatus[ 4 ] & CURQUAL_TOTAL_NOCONSOLIDATE_PRINT ) ? FALSE : TRUE;

    /* --- retrieve index file handle to print item data --- */
    if ( hsStorage == pTran->hsTranItemStorage ) {
        hsIndexFile      = ( fConsoli ) ? pTran->hsTranItemIndex : pTran->hsTranNoItemIndex;
        ulStorageActSize = sizeof( TRANITEMSTORAGEHEADER ) + pTran->usTranItemStoVli;
    } else if ( hsStorage == pTran->hsTranConsStorage ) {
        hsIndexFile      = ( fConsoli ) ? pTran->hsTranConsIndex : pTran->hsTranNoConsIndex;
        ulStorageActSize = sizeof( TRANCONSSTORAGEHEADER ) + pTran->usTranConsStoVli;
    } else {
        return;
    }

    /* --- retrieve information about index file --- */
	//11-1103- SR 201 JHHJ
    TrnReadFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), hsIndexFile , &ulActualBytesRead);

    ulTtlIdxReadSize = 0;
    uchTtlItemCo     = 0;
    uchCurItemCo     = ( sizeof( auchIdxWork ) / sizeof( PRTIDX ));

    while ( ulTtlIdxReadSize < IdxFileInfo.usIndexVli ) {
        TrnReadFile( (USHORT)(sizeof( PRTIDXHDR ) + ulTtlIdxReadSize), (VOID *)auchIdxWork, (USHORT)sizeof( auchIdxWork ), hsIndexFile, &ulCurIdxReadSize); /* ### Mod (2171 for Win32) V1.0 */
        if ( IdxFileInfo.usIndexVli < ( ulTtlIdxReadSize + ulCurIdxReadSize )) {
            ulCurIdxReadSize = IdxFileInfo.usIndexVli - ulTtlIdxReadSize;
            uchCurItemCo     = ( UCHAR )( ulCurIdxReadSize / sizeof( PRTIDX ));
        }
        ulTtlIdxReadSize += ulCurIdxReadSize;

        /* --- retrieve item data in storage and print its data --- */
        pPrtIdxHighest = ( PRTIDX * )( auchIdxWork );
        pPrtIdxCurrent = ( PRTIDX * )( auchIdxWork );

        if ( IdxFileInfo.uchNoOfItem < ( uchTtlItemCo + uchCurItemCo )) {
            uchCurItemCo = IdxFileInfo.uchNoOfItem - uchTtlItemCo;
        }
        uchTtlItemCo += uchCurItemCo;

        ulStorageReadOffset = 0;
        ulStorageReadSize   = 0;

        for ( uchItemCo = 0; uchItemCo < uchCurItemCo; uchItemCo++, pPrtIdxCurrent++ ) {
            /* --- is current pointed item in the work buffer ? --- */
            if ( ! PrtIsItemInWorkBuff( pPrtIdxCurrent->usItemOffset, auchStorageWork, (USHORT)ulStorageReadOffset, (USHORT)ulStorageReadSize )) {
                /* --- retrieve specified item in storage file --- */
                ulStorageReadOffset = pPrtIdxCurrent->usItemOffset;

                TrnReadFile( (USHORT)ulStorageReadOffset, auchStorageWork, sizeof( auchStorageWork ), hsStorage, &ulStorageReadSize );

                if ( ulStorageActSize < ( ulStorageReadOffset + ulStorageReadSize )) {
                    /* --- function reads data over than end of storage --- */
                    ulStorageReadSize = ulStorageActSize - ulStorageReadOffset;
                }
            }

            ulItemWorkOffset = pPrtIdxCurrent->usItemOffset - ulStorageReadOffset;

            /* --- unzip data and retrieve it's mnemonic --- */
            memset (&auchItemData, 0, sizeof(auchItemData));
            ulItemReadLen = RflGetStorageItem( auchItemData, auchStorageWork + ulItemWorkOffset, RFL_WITH_MNEM );

            if (( auchItemData[ 0 ] == CLASS_ITEMDISC ) && ( auchItemData[ 1 ] == CLASS_ITEMDISC1 )) {
                continue;
            }

            if ( fsPrtStatus & PRT_TAKETOKIT ) {
                if ( PRT_NOTKPITEM == PrtChkKPItem( pTran, ( ITEMSALES * )auchItemData )) {
                    continue;
                }
            }

            if ( fsPrtStatus & PRT_KPONLY ) {
                if ( ! (( auchItemData[ 0 ] == CLASS_ITEMSALES ) ||
                        ( auchItemData[ 0 ] == CLASS_ITEMTOTAL ) ||
                        ( auchItemData[ 0 ] == CLASS_ITEMPRINT ) ||
                        ( auchItemData[ 0 ] == CLASS_ITEMTRANSOPEN ))) {
                    continue;
                }
            }

            /* --- set qty and price for unconsolidated print --- */
            if ( auchItemData[ 0 ] == CLASS_ITEMSALES ) {
                pItemSales   = ( ITEMSALES * )auchItemData;
                pItemSales->lQTY     = pPrtIdxCurrent->lQTY;
                pItemSales->lProduct = pPrtIdxCurrent->lProduct;

                if (fConsoli) {
                    /* --- read partner item for no void print --- */
                    PrtSetSalesCouponQTY(pTran, pItemSales, hsStorage);
                } else {
                    /* set coupon qty for non-void, non-consoli print, V3.3 */
                    if (pItemSales->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
                        pItemSales->sCouponQTY = 1;
                    } else {
                        pItemSales->sCouponQTY = pPrtIdxCurrent->sNetQTY;
                    }
                }
            }

            if ( pTran->TranCurQual.uchPrintStorage == PRT_CONSOL_CP_EPT ) {
                PrtPrintCpEpt( pTran, auchItemData );
            } else {
                PrtPrintItem( pTran, auchItemData );
            }

            /* --- if this is sales item with item discount, print out item discount data --- */
            if (RflIsSalesItemDisc( auchItemData )) {
                if ( ! PrtIsIDiscInWorkBuff( (USHORT)(ulItemWorkOffset + ulItemReadLen), auchStorageWork, (USHORT)ulStorageReadSize )) { /* ### Mod (2171 for Win32) V1.0 */
                    /* read parent plu status for item discount void consolidation */
                    sQTYSave      = pItemSales->sCouponQTY;
                    if ((pItemSales->ControlCode.auchPluStatus[2] & PLU_SCALABLE) ||
                        (pItemSales->uchPM) ||  /* saratoga */
                        (pItemSales->uchPPICode)) {

                        sQTYSave = 1;    /* compulsory void print */
                    }

                    /* --- retrieve specified item in storage file --- */
                    ulStorageReadOffset += ( ulItemWorkOffset + ulItemReadLen );
                    TrnReadFile( ulStorageReadOffset, auchStorageWork, sizeof( auchStorageWork ), hsStorage, &ulStorageReadSize );

                    if ( ulStorageActSize < ( ulStorageReadOffset + ulStorageReadSize )) {
                        /* --- function reads data over than end of storage --- */
                        ulStorageReadSize = ulStorageActSize - ulStorageReadOffset;
                    }

                    ulItemWorkOffset = 0;
                    ulItemReadLen    = 0;
                }

                memset (&auchItemData, 0, sizeof(auchItemData));
                RflGetStorageItem( auchItemData, auchStorageWork + ulItemWorkOffset + ulItemReadLen, RFL_WITH_MNEM );

                /* void consolidation print */
                if ((pTran->TranCurQual.auchTotalStatus[4] & CURQUAL_TOTAL_NOCONSOLIDATE_PRINT) == 0) {
                    if (sQTYSave == 0) {
                        pItemDisc    = ( ITEMDISC * )auchItemData;
                        pItemDisc->fsPrintStatus &= ~(PRT_RECEIPT|PRT_SLIP);
                    }
                }

                if ( pTran->TranCurQual.uchPrintStorage == PRT_CONSOL_CP_EPT ) {
                    PrtPrintCpEpt( pTran, auchItemData );
                } else {
                    PrtPrintItem( pTran, auchItemData );
                }
            }
        }
    }
}
/*
*===========================================================================
** Format   : BOOL PrtIsItemInWorkBuff( USHORT usItemOffset,
*                                       UCHAR  *puchWorkBuff,
*                                       USHORT usBuffOffset,
*                                       USHORT usBuffSize )
*
*    Input  : USHORT    usItemOffset  - offset of target item in storage file
*             UCHAR     *puchWorkBuff - address of work buffer
*             USHORT    usBuffOffset  - offset of work buffer in storage file
*             USHORT    usBuffSize    - size of work buffer
*   Output  : none
*    InOut  : none
** Return   : TRUE  -   item data is in the work buffer
*             FALSE -   item data is not in the work buffer
*
** Synopsis : 
*       determine specified item is in the work buffer.
*===========================================================================
*/
BOOL PrtIsItemInWorkBuff( USHORT usItemOffset,
                          TCHAR  *puchWorkBuff,
                          USHORT usBuffOffset,
                          USHORT usBuffSize )
{
    USHORT  usItemWorkOffset;
    USHORT  usUnzipSize;
    USHORT  usZipSize;

    if ( usItemOffset < usBuffOffset ) {
        return ( FALSE );
    }

    usItemWorkOffset = usItemOffset - usBuffOffset;
    if (usItemWorkOffset > usBuffSize) {    /* saratoga */
        return ( FALSE );
    }

    usUnzipSize = ( USHORT )( *( puchWorkBuff + usItemWorkOffset ));
    if ( usBuffSize < ( usItemWorkOffset + usUnzipSize + sizeof( USHORT ))) {
        return ( FALSE );
    }

    usZipSize =  *(( USHORT * )( puchWorkBuff + usItemWorkOffset + usUnzipSize ));
    if ( usBuffSize < ( usItemWorkOffset + usUnzipSize + usZipSize )) {
        return ( FALSE );
    }
    return ( TRUE );
}

/*
*===========================================================================
** Format   : BOOL PrtIsIDiscInWorkBuff( USHORT usItemOffset,
*                                        UCHAR  *puchWorkBuff,
*                                        USHORT usBuffSize )
*
*    Input  : USHORT    usItemOffset  - offset of target item in work buffer
*             UCHAR     *puchWorkBuff - address of work buffer
*             USHORT    usBuffSize    - size of work buffer
*   Output  : none
*    InOut  : none
** Return   : TRUE  -   item data is in the work buffer
*             FALSE -   item data is not in the work buffer
*
** Synopsis : 
*       determine specified item is in the work buffer.
*===========================================================================
*/
BOOL PrtIsIDiscInWorkBuff( USHORT usItemOffset,
                           TCHAR  *puchWorkBuff,
                           USHORT usBuffSize )
{
    UCHAR   uchUnzipLen;
    USHORT  usZipLen;

    /* --- determine unzipped data is out of work buffer --- */
    uchUnzipLen = /* * */(UCHAR)( puchWorkBuff + usItemOffset );

    if ( usBuffSize <= ( usItemOffset + uchUnzipLen )) {
        return ( FALSE );
    }

    /* --- determine  zipped data is out of work buffer --- */
    usZipLen = *(( USHORT * )( puchWorkBuff + usItemOffset + uchUnzipLen ));

    if ( usBuffSize < ( usItemOffset + uchUnzipLen + usZipLen )) {
        return ( FALSE );
    }
    return ( TRUE );
}

/*
*===========================================================================
** Format  : VOID PrtPrintDemand(TRANINFORMATION *pTran)
*
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints transaction data.(print on demand)
*            Function doesnot do control of compusoly-print to receipt
*            and no-print to other printer.
*===========================================================================
*/
VOID PrtPrintDemand(TRANINFORMATION *pTran)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };/* PARA_... defined in "paraequ.h" */
	TCHAR  aszDemand[PARA_TRANSMNEMO_LEN * 2 + 1] = { 0 };
    SHORT   hsRead1, hsRead2;           /* storage file handle */
    SHORT  sRet;

	RflGetTranMnem(aszTranMnem, TRN_DEMAND_ADR);                  /* get transaction mnem. */
    PrtDouble(aszDemand, TCHARSIZEOF(aszDemand), aszTranMnem);    /* convert mnemo. to double-wide */

    /* -- check storage pointer -- */
    if ( PRT_1PORTION == (sRet = PrtChkStorage(&hsRead1, &hsRead2, pTran)) ) {
        fsPrtStatus |= PRT_DEMAND;              /* set print on demand status */
        PrtCallIPD(pTran, hsRead1);             /* decompress data and call IPD */

        fsPrtStatus    = 0;                     /* initialize control area */
        fsPrtNoPrtMask = 0;
        fsPrtCompul    = 0;
    }

    PmgPrint(PMG_PRT_RECEIPT, aszDemand, (USHORT)_tcslen(aszDemand));    /* "print on demand" line */
}

VOID PrtPrintSummaryReceipt(TRANINFORMATION *pTran, ITEMTENDER *pItemTender, int iPrintItems)
{
    SHORT   hsRead1, hsRead2;           /* storage file handle */
    SHORT  sRet;

    /* -- check storage pointer -- */
    if ( PRT_1PORTION == (sRet = PrtChkStorage(&hsRead1, &hsRead2, pTran)) ) {
        fsPrtStatus |= PRT_DEMAND;              /* set print on demand status */
        PrtCallIPD(pTran, hsRead1);             /* decompress data and call IPD */

        fsPrtStatus    = 0;                     /* initialize control area */
        fsPrtNoPrtMask = 0;
        fsPrtCompul    = 0;
    }
}

/*
*===========================================================================
** Format  : VOID PrtSetSalesCouponQTY(ITEMSALES *pItem, SHORT sFileHandle);
*
*   Input  : SHORT            sFileHandle -Storage File handle
*   Output : none
*   InOut  : ITEMSALES        *pItem     -Item Data address
** Return  : nothing
*
** Synopsis: 
*===========================================================================
*/
VOID PrtSetSalesCouponQTY(TRANINFORMATION *pTran, ITEMSALES *pItem, SHORT sFileHandle)
{
    TCHAR           auchVoidWork[ sizeof(ITEMSALES) + 10];
	ULONG			ulActualBytesRead;

    if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
        pItem->sCouponQTY = 1;      /* always print */
    } else {
        if (pItem->ControlCode.auchPluStatus[0] & PLU_MINUS) {
            pItem->sCouponQTY = (SHORT)(pItem->lQTY / PLU_BASE_UNIT);
        }

        if (pItem->usItemOffset) {
			ITEMSALES       ItemVoidSales = { 0 };

            TrnReadFile( pItem->usItemOffset, auchVoidWork, sizeof( auchVoidWork ), sFileHandle, &ulActualBytesRead );

            RflGetStorageItem( &ItemVoidSales, auchVoidWork, RFL_WITHOUT_MNEM );

            /* minus plu void consolidation */
            if (ItemVoidSales.ControlCode.auchPluStatus[0] & PLU_MINUS) {
                ItemVoidSales.sCouponQTY = (SHORT)(ItemVoidSales.lQTY / PLU_BASE_UNIT);
                pItem->sCouponQTY += ItemVoidSales.sCouponQTY;
                if (pTran->TranCurQual.fsCurStatus & CURQUAL_PRESELECT_MASK) {
                    if (pItem->sCouponQTY >= 0) {
                        ItemVoidSales.sCouponQTY = pItem->sCouponQTY;
                        pItem->sCouponQTY = 0;
                    } else {
                        ItemVoidSales.sCouponQTY = 0;
                    }
                } else {
                    if (pItem->sCouponQTY <= 0) {
                        ItemVoidSales.sCouponQTY = pItem->sCouponQTY;
                        pItem->sCouponQTY = 0;
                    } else {
                        ItemVoidSales.sCouponQTY = 0;
                    }
                }
            }

            if (pTran->TranCurQual.fsCurStatus & CURQUAL_PRESELECT_MASK) { /* transaction void or transaction return */
                if (pItem->fbModifier & VOID_MODIFIER) {
                    if ((ItemVoidSales.lQTY + pItem->lQTY) < 0L) {
                        /* clear sales qty */
                        pItem->sCouponQTY = 0;
                    } else {
                        pItem->sCouponQTY = ItemVoidSales.sCouponQTY;
                    }
                } else {
                    if ((ItemVoidSales.lQTY + pItem->lQTY) >= 0L) {
                        /* clear sales qty */
                        pItem->sCouponQTY = 0;
                    }
                }
            } else {
                if (pItem->fbModifier & VOID_MODIFIER) {
                    if ((ItemVoidSales.lQTY + pItem->lQTY) >= 0L) {
                        /* clear sales qty */
                        pItem->sCouponQTY = 0;
                    } else {
                        pItem->sCouponQTY = ItemVoidSales.sCouponQTY;
                    }
                } else {
                    if ((ItemVoidSales.lQTY + pItem->lQTY) < 0L) {
                        /* clear sales qty */
                        pItem->sCouponQTY = 0;
                    }
                }
            }
        }
    }
}

/* ===== End of File ( PrCTran_.C ) ===== */
