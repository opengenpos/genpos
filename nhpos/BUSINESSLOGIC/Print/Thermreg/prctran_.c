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
* Jun-14-93 : 00.00.01 : R.Itoh     :                                    
* Jun-24-93 : 01.00.12 : K.You      : add soft check feature.(mod. PrtPrintTran)
* Mar-07-94 : 00.00.04 : K.You      : add Flex GC File Feature.(mod. PrtPrintTran,
*           :          :            : PrtChkStorage, PrtCallIPD)
* May-20-94 : 02.05.00 : K.You      : bug fixed S-23 (mod. PrtCallIPD)
* May-24-94 : 02.05.00 : Yoshiko.Jim: Dupli.Print for CP,EPT (add PRT_CONSOL_CP_EPT)
* May-25-94 : 02.05.00 : K.You      : stop to bug fix S-23 (mod. PrtCallIPD)
* Apr-27-95 : 03.00.00 : T.Nakahata : add total mnemonic to kitchen feature
* Jun-30-95 : 03.00.00 : T.Nakahata : add print priority feature
* Jul-26-95 : 03.00.04 : T.Nakahata : print on journal by service total key
* Aug-28-95 : 03.00.04 : T.Nakahata : no-consolidate by total key option
*    -  -   : 03.01.00 : M.Ozawa    : add split guest check print
* May-28-96 : 03.01.07 : M.Ozawa    : add PrtKitchenCallIPD() for kp by seat #
* Jan-06-98 : 03.01.16 : M.Ozawa    : add non-consolidated kp print by seat no.
* Aug-14-98 : 03.03.00 : M.Ozawa    : remove PrtKitchenCallIPD() for non-cons kp print
* Aug-12-99 : 03.05.00 : K.Iwata    : Marge GUEST_CHECK_MODEL/STORE_RECALL_MODEL
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
* Jan-13-00 : 01.00.00 : H.Endo     : use Macro to Datasize (PrtCallIPD())
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
/** ---- M S   -  C ---- **/
#include <tchar.h>
#include <memory.h>
#include <malloc.h>
#include <string.h>

/** ---- 2170 local ---- **/
#include"ecr.h"
#include"pif.h"
#include"paraequ.h"
#include<para.h>
#include <csstbpar.h>
#include"regstrct.h"
#include"transact.h"
#include"trans.h"
#include"pmg.h"
#include"shr.h"
#include"prt.h"
#include"rfl.h"
#include"prtprty.h"
#include"prtrin.h"
#include"prtcom.h"
#include "prrcolm_.h"
#include"prtshr.h"

extern USHORT usDbgHandle;
extern UCHAR uchIsNoCheckOn;  //JHHJ SR18
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
#if _DEBUG
    XGHEADER        IPData;
#endif

    /* ---- initialize static area at master reset ---- */

    fsPrtStatus = 0;
    memset (auchPrtSeatCtl, 0, sizeof(auchPrtSeatCtl));
    
#if _DEBUG
    memset(&IPData, 0, sizeof(IPData));
    IPData.auchFaddr[0] = 127;
    IPData.auchFaddr[1] = 0;
    IPData.auchFaddr[2] = 0;
    IPData.auchFaddr[3] = 1; /* localhost */
    IPData.usFport = 5001;  /* terget server port */
    IPData.usLport = 0;     /* use client only */

    usDbgHandle = PifNetOpenEx(&IPData, PIF_NET_DIRECTPORTMODE);
    PifNetControl(usDbgHandle, PIF_NET_SET_MODE, PIF_NET_DMODE);
#endif
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
        fsPrtStatus    = 0;
		// Statement needed so that this doesnt set the Print options back to defaults (zeroes) when doing Check no Purchase
		if(!CliParaMDCCheck(MDC_CHK_NO_PURCH, ODD_MDC_BIT0) || uchIsNoCheckOn == 0)
		{
		   fsPrtNoPrtMask = 0;				/* initialize control area */
		   fsPrtCompul    = 0;
		}             /* clear current slip's pages and lines */

        memset (auchPrtSeatCtl, 0, sizeof(auchPrtSeatCtl));
    }
    uchPrtSeatNo = 0;
    fbPrtKPSeatState = 0xFF;                /* not feed first item at non seat# trans. */

    memset(auchPrtFailKP, '\0', sizeof(auchPrtFailKP));   /* fail kitchen printer no initial. */

    /* -- check storage pointer -- */
    if ( PRT_NOPORTION == (sRet = PrtChkStorage(&hsRead1, &hsRead2, pTran)) ) {

        fsPrtStatus    = 0;                 /* initialize control area */
        fsPrtNoPrtMask = 0;
        fsPrtCompul    = 0;
        usPrtSlipPageLine = 0;              /* clear current slip's pages and lines */

        memset (auchPrtSeatCtl, 0, sizeof(auchPrtSeatCtl));

        /* -- save shared/normal, aborted, alternation flag -- */
        fbPrtAbortSav = fbPrtAbortStatus;
        fbPrtAltrSav = fbPrtAltrStatus;

        /* -- reset shared/normal, aborted, alternation flag -- */
        fbPrtShrStatus = 0; 
        fbPrtAbortStatus = 0;
        fbPrtAltrStatus = 0;
        return (0);
    }

    /* -- decompress data and call IPD  -- */
    /* V3.3, ICI */

	//the code below checks to see if there is no option whether
	//to print buffered or not, also see if auchTotalStatus[0] has 
	//a value set to it, if it does not, then a total has not been
	//pressed, and so we will print the transaction anyways.
	//SR 678 JHHJ
    uchTotal = pTran->TranCurQual.auchTotalStatus[ 4 ];
    if ((TrnInformation.TranCurQual.auchTotalStatus[2] & CURQUAL_TOTAL_PRINT_BUFFERING) ||
	(TrnInformation.TranCurQual.auchTotalStatus[0] == 0) ||       // if not total key pressed
	(pTran->TranCurQual.fsCurStatus2 & PRT_POST_RECEIPT)) 
	{
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
			if (CliParaMDCCheckField(MDC_PLU_TICKET, MDC_PARAM_BIT_A)) {
				BOOL            freeMalloced;
				PluChitInfoAll  PluChit;
				PluChitInfo     PluChitInfoList[25] = {0};

				PluChit.nPluChitInfoSize = 75;
				freeMalloced = ((PluChit.pPluChitInfo = calloc (PluChit.nPluChitInfoSize, sizeof(PluChitInfo))) != 0);
				if (! freeMalloced) {
					PluChit.pPluChitInfo = PluChitInfoList;
					PluChit.nPluChitInfoSize = sizeof(PluChitInfoList)/sizeof(PluChitInfoList[0]);
				}
				PluChit.iOrderDeclareAdr = 0;
				PrtCallIPDTicketInfo ( pTran, hsRead1, &PluChit );
				PrtPluTicketList (pTran, PLUCHITINFO_FLAG_PRINT_TICKET, PluChit.pPluChitInfo, PluChit.nPluChitInfoSize, PluChit.iOrderDeclareAdr);
				if (freeMalloced) {
					free (PluChit.pPluChitInfo);
				}
			} else {
				PrtCallIPD(pTran, hsRead1);
			}
		}
	}else
	{
		fsPrtNoPrtMask = PRT_EJ_JOURNAL;
		fsPrtCompul    = 0;
		if ( (TrnInformation.TranCurQual.flPrintStatus & CURQUAL_POSTRECT) == 0) { // no EJ print on duplicate receipt
			fsPrtStatus    |= PRT_EJ_JOURNAL;
		}
		uchPrtSeatNo = 0;                       /* continuous seat no print */
		PrtCallIPD(pTran, hsRead1);
	}

    /* -- printer alternation needed ? (in shared printer system) -- */ 
    if ( fbPrtAltrStatus & PRT_BUFFERING_ALTR ) {
        fbPrtAltrStatus &= ~PRT_WRITE_ABORT;    /* reset write abort status */
        PrtShrAltr(pTran, hsRead1);              /* retry from the beginning */
    }


    fbPrtAltrStatus &= ~PRT_BUFFERING_ALTR;     /* reset alternation flag (buffring print) */
                                                /* for take to kp stub */
    /* V3.3, ICI */
    pTran->TranCurQual.auchTotalStatus[ 4 ] = uchTotal;

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
		//the code below checks to see if there is no option whether
		//to print buffered or not, also see if auchTotalStatus[0] has 
		//a value set to it, if it does not, then a total has not been
		//pressed, and so we will print the transaction anyways.
		//SR 678 JHHJ
		PrtCallIPD(pTran, hsRead1);
        if ((TrnInformation.TranCurQual.auchTotalStatus[2] & CURQUAL_TOTAL_PRINT_BUFFERING) || (!TrnInformation.TranCurQual.auchTotalStatus[0]))
		{
//			PifSleep(2500);	/* sleep 1 sec. for the next duplicate receipt, 09/08/01 */
		}
    }

    /* -- take to kitchen management -- */
    for (i = 0; i < PRT_KPRT_MAX; i++) {
        if (auchPrtFailKP[i]) break;
    }
    if (i < PRT_KPRT_MAX) {
        PrtTakeToKP(pTran, hsRead1);
//        PifSleep(1000);	/* sleep 1 sec. for the next duplicate receipt, 09/08/01 */
    }

    /* -- save shared/normal, aborted, alternation flag -- */
    fbPrtAbortSav = fbPrtAbortStatus;
    fbPrtAltrSav = fbPrtAltrStatus;

    /* -- reset shared/normal, aborted, alternation flag -- */
    fbPrtShrStatus = 0;  
    fbPrtAbortStatus = 0;
    fbPrtAltrStatus = 0;

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
    ULONG           fsCurStatus;
    UCHAR           auchFinSeat[NUM_SEAT];
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

    /* -- save transaction information data to temporary buffer */
    hsTranConsStorage = pTran->hsTranConsStorage;
    usTranConsStoFSize = pTran->usTranConsStoFSize;
    usTranConsStoVli = pTran->usTranConsStoVli;
    hsTranConsIndex = pTran->hsTranConsIndex;
    hsTranNoConsIndex = pTran->hsTranNoConsIndex;
    fsCurStatus = pTran->TranCurQual.fsCurStatus;
    memcpy (auchFinSeat, pTran->TranGCFQual.auchFinSeat, sizeof(auchFinSeat));

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

        /* -- save shared/normal, aborted, alternation flag -- */
        fbPrtAbortSav = fbPrtAbortStatus;
        fbPrtAltrSav = fbPrtAltrStatus;

        /* -- reset shared/normal, aborted, alternation flag -- */
        fbPrtShrStatus = 0; 
        fbPrtAbortStatus = 0;
        fbPrtAltrStatus = 0;

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

    /* -- printer alternation needed ? (in shared printer system) -- */ 
    if ( fbPrtAltrStatus & PRT_BUFFERING_ALTR ) {

        fbPrtAltrStatus &= ~PRT_WRITE_ABORT;    /* reset write abort status */
        if ((usType) || (pSplit->TranGCFQual.usGuestNo) || (pSplit->TranItemizers.lMI)) {
            PrtShrAltr(pTran, hsRead1);              /* retry from the beginning */
        }
    }

    fbPrtAltrStatus &= ~PRT_BUFFERING_ALTR;     /* reset alternation flag (buffring print) */

                                            /* for take to kp stub */
    PrtCompNo.fsComp=fsPrtCompul;           /* used soft check print */
    PrtCompNo.fsNo=fsPrtNoPrtMask;

    /* -- save shared/normal, aborted, alternation flag -- */
    fbPrtAbortSav = fbPrtAbortStatus;
    fbPrtAltrSav = fbPrtAltrStatus;

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

        /* -- reset shared/normal, aborted, alternation flag -- */
        fbPrtShrStatus = 0;  
        fbPrtAbortStatus = 0;
        fbPrtAltrStatus = 0;
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

    /* -- no print -- */
    if (( uchPrtStorage == PRT_NOSTORAGE) && ( uchKitStorage == PRT_NOSTORAGE )){
       return ( PRT_NOPORTION );
    /* -- kitchen & other printer / same buffer -- */
    }  else if (( uchPrtStorage == PRT_ITEMSTORAGE ) && ( uchKitStorage == PRT_ITEMSTORAGE )) {
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

        return ( PRT_DIFFBUFF );    /* for different print format between RJ and kitchen R3.1 */
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

            if (CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT3) || CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2)) {    /* R3.1 */
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
VOID PrtPluTicketList (TRANINFORMATION *pTran, ULONG ulPrintOptions, PluChitInfo *PluChitInfoList, int PluChitInfoListIndex, int PluChitOrderDeclareAdr)
{
	int i;
	ULONG  ulBarcodeCode = PRT_BARCODE_CODE_CODEEAN13;
	SHORT  sTotalOrderQty = 0, sTotalOrderQtyPrinted = 0;

	if (pTran->TranCurQual.fsCurStatus & (CURQUAL_CANCEL | CURQUAL_PVOID | CURQUAL_DUPLICATE_COPY | CURQUAL_PRETURN) ) {
		return;
	}
	if (pTran->TranCurQual.flPrintStatus & ( CURQUAL_POSTRECT | CURQUAL_PARKING ) ) {
		return;
	}
	if (pTran->TranCurQual.fsCurStatus2 & ( PRT_DEMAND_COPY | PRT_GIFT_RECEIPT | PRT_POST_RECEIPT ) ) {
		return;
	}

	sTotalOrderQty = 0;
	if ((ulPrintOptions & PLUCHITINFO_FLAG_PRINT_TICKET) == 0) {
		for (i = 0; PluChitInfoList[i].aszMnemonic[0]; i++) {
			sTotalOrderQty += PluChitInfoList[i].sQty;
		}
	}

	for (i = 0; PluChitInfoList[i].aszMnemonic[0]; i++) {
		int j;
		SHORT  sQtyLoop;
		TCHAR  aszPrintLine[44];

		if ((ulPrintOptions & PLUCHITINFO_FLAG_PRINT_TICKET) != 0 && PluChitInfoList[i].usFamilyCode < 990) {
			continue;
		}

		for (sQtyLoop = 0; sQtyLoop < PluChitInfoList[i].sQty; sQtyLoop++) {
			PrtTransactionBegin(PRT_REGULARPRINTING, pTran->TranGCFQual.usGuestNo);
			if ((ulPrintOptions & PLUCHITINFO_FLAG_PRINT_TICKET) == 0) {
				USHORT  usGuestNo = 0;
				TCHAR   aszTranMnemOrder[PARA_CHAR24_LEN + 1] = {0};
				TCHAR   aszTranMnemTent[PARA_CHAR24_LEN + 1] = {0};
				TCHAR   aszTranMnem[PARA_CHAR24_LEN + 1] = {0};
				TCHAR   *aszTentName = _T("");

				//prints the tent mnemonic first, then prints the user input on one line
				//PDINU
				if (pTran->TranModeQual.aszTent[0] > 0){
					RflGetTranMnem(aszTranMnemTent, TRN_TENT);
					aszTentName = pTran->TranModeQual.aszTent;
				}
				else if (pTran->TranGCFQual.aszName[0] > 0) {
					aszTentName = pTran->TranGCFQual.aszName;
				}
				/* --- set up transaction mnemonic of GC no., and GC no. string --- */
				if( pTran->TranGCFQual.uchCdv == 0 ) { /* GC No. without Check Digit */
					usGuestNo =  pTran->TranGCFQual.usGuestNo;
				} else {            /* GC No. with Check Digit */
					usGuestNo =  pTran->TranGCFQual.usGuestNo;
					usGuestNo *= 100;
					usGuestNo += pTran->TranGCFQual.uchCdv;
				}

				PrtGetLead( aszTranMnemOrder, LDT_UNIQUE_NO_ADR );
				PrtPrintf(PMG_PRT_RECEIPT, _T("%s %s\t%s %d"), aszTranMnemTent, aszTentName,  aszTranMnemOrder, usGuestNo);

				sTotalOrderQtyPrinted++;
				RflGetTranMnem(aszTranMnem, TRN_ITEMCOUNT_ADR);
				PrtPrintf(PMG_PRT_RECEIPT, _T("%s %d -- %d\t%d -- %d"), aszTranMnem, sQtyLoop + 1, PluChitInfoList[i].sQty, sTotalOrderQtyPrinted, sTotalOrderQty);
				PrtFeed(PMG_PRT_RECEIPT, 1);            /* 1 line feed */
			}

			if (ulPrintOptions & PLUCHITINFO_FLAG_PRINT_TICKET) {
				// Print the standard P57 address 8 and 9 receipt header in double size if set
				for ( j = CH24_1STRH_ADR; j <= CH24_2NDRH_ADR; j++) {
					TCHAR asz24Mnem[PARA_CHAR24_LEN + 1] = {0};
					TCHAR asz24MnemDouble[PARA_CHAR24_LEN * 2 + 1] = {0};

					PrtGet24Mnem(asz24Mnem, j);
					asz24Mnem[21] = '\0';         // truncate the mnemonic to max of 20 characters
					if (asz24Mnem[0]) {
						/* -- convert to square double -- */
						PrtSDouble(asz24MnemDouble, TCHARSIZEOF(asz24MnemDouble), asz24Mnem);
						PrtPrint(PMG_PRT_RECEIPT, asz24MnemDouble, tcharlen(asz24MnemDouble));
					}
				}
				PrtFeed(PMG_PRT_RECEIPT, 1);            /* 1 line feed */
			}

			// if there is an Order Declare then print that next in double size
			if (PluChitOrderDeclareAdr) {
				TCHAR asz24Mnem[PARA_CHAR24_LEN + 1] = {0};
				TCHAR asz24MnemDouble[PARA_CHAR24_LEN * 2 + 1] = {0};

				RflGetTranMnem(asz24Mnem, PluChitOrderDeclareAdr);
				if (asz24Mnem[0]) {
					asz24Mnem[21] = '\0';
					PrtFeed(PMG_PRT_RECEIPT, 1);            /* 1 line feed */
					/* -- convert to square double -- */
					PrtDouble(asz24MnemDouble, TCHARSIZEOF(asz24MnemDouble), asz24Mnem);
					PrtPrint(PMG_PRT_RECEIPT, asz24MnemDouble, tcharlen(asz24MnemDouble));
				}
				PrtFeed(PMG_PRT_RECEIPT, 1);            /* 1 line feed */
			}

			if (ulPrintOptions & PLUCHITINFO_FLAG_PRINT_TICKET) {
				SHORT  iLines = 0;
				TCHAR asz24Mnem[PARA_CHAR24_LEN + 1] = {0};
				TCHAR asz24MnemDouble[PARA_CHAR24_LEN * 2 + 1] = {0};

				// Print the PLU ticket (chit) first header line from P57 address 23 in double size if set
				PrtGet24Mnem(asz24Mnem, CH24_PLU_TICKET_01);
				if (asz24Mnem[0]) {
					asz24Mnem[21] = '\0';
					/* -- convert to square double -- */
					PrtSDouble(asz24MnemDouble, TCHARSIZEOF(asz24MnemDouble), asz24Mnem);
					PrtPrint(PMG_PRT_RECEIPT, asz24MnemDouble, tcharlen(asz24MnemDouble));
					iLines++;
				}

				// Print the PLU ticket (chit) second header line from P57 address 24 in standard size, both sides
				PrtGet24Mnem(asz24Mnem, CH24_PLU_TICKET_02);
				if (asz24Mnem[0]) {
					/* -- convert to square double -- */
					if (ulPrintOptions & PLUCHITINFO_FLAG_PRINT_PLUCOLUMN) {
						asz24Mnem[21] = '\0';
						RflFormatLineTwoColumn (aszPrintLine, asz24Mnem, asz24Mnem, PRT_THCOLUMN);
					} else {
						PrtDouble(asz24MnemDouble, TCHARSIZEOF(asz24MnemDouble), asz24Mnem);
						_tcscpy (aszPrintLine, asz24MnemDouble);
					}
					PrtPrint(PMG_PRT_RECEIPT, aszPrintLine, tcharlen(aszPrintLine));
					iLines++;
				}
				if (iLines > 0)
					PrtFeed(PMG_PRT_RECEIPT, 1);            /* 1 line feed */
			}

			if (ulPrintOptions & PLUCHITINFO_FLAG_PRINT_TICKET) {
				SHORT  iLines = 0;

				// Print the PLU ticket (chit) #/Type entered data in standard size, both sides
				// for admission ticket type prints we print the #/Type information before the PLU info.
				for ( j = 0; j < NUM_OF_NUMTYPE_ENT; j++) {
					if (PluChitInfoList[i].aszNumber[j][0]) {
						if (ulPrintOptions & PLUCHITINFO_FLAG_PRINT_PLUCOLUMN) {
							PluChitInfoList[i].aszNumber[j][21] = 0;
							RflFormatLineTwoColumn (aszPrintLine, PluChitInfoList[i].aszNumber[j], PluChitInfoList[i].aszNumber[j], PRT_THCOLUMN);
						} else {
							_tcscpy (aszPrintLine, PluChitInfoList[i].aszNumber[j]);
						}
						PrtPrint(PMG_PRT_RECEIPT, aszPrintLine, tcharlen(aszPrintLine));
						iLines++;
					}
				}
				if (iLines > 0)
					PrtFeed(PMG_PRT_RECEIPT, 1);            /* 1 line feed */
			}

			// Print the PLU ticket (chit) PLU mnemonic in double size once then standard size, both sides
			{
				TCHAR asz24Mnem[PARA_CHAR24_LEN + 1] = {0};
				TCHAR asz24MnemDouble[PARA_CHAR24_LEN * 2 + 1] = {0};

				if (ulPrintOptions & (PLUCHITINFO_FLAG_PRINT_PLUDOUBLE | PLUCHITINFO_FLAG_PRINT_TICKET)) {
					/* -- convert PLU mnemonic to double width to make it more visible -- */
					if (PluChitInfoList[i].aszAdjMnemonic[0]) {
						PrtDouble(asz24MnemDouble, TCHARSIZEOF(asz24MnemDouble), PluChitInfoList[i].aszAdjMnemonic);
						PrtPrint(PMG_PRT_RECEIPT, asz24MnemDouble, tcharlen(asz24MnemDouble));
					}
					PrtDouble(asz24MnemDouble, TCHARSIZEOF(asz24MnemDouble), PluChitInfoList[i].aszMnemonic);
					PrtPrint(PMG_PRT_RECEIPT, asz24MnemDouble, tcharlen(asz24MnemDouble));
					PrtFeed(PMG_PRT_RECEIPT, 1);            /* 1 line feed */
				} else  {
					TCHAR aszAdjectiveAndPLU [PARA_ADJMNEMO_LEN + NUM_DEPTPLU + 1] = {0};
					if (PluChitInfoList[i].aszAdjMnemonic[0]) {
						RflSPrintf (aszAdjectiveAndPLU, NUM_DEPTPLU, _T("%s "), PluChitInfoList[i].aszAdjMnemonic);
					}
					_tcscat (aszAdjectiveAndPLU, PluChitInfoList[i].aszMnemonic);
					if (ulPrintOptions & PLUCHITINFO_FLAG_PRINT_PLUCOLUMN) {
						RflFormatLineTwoColumn (aszPrintLine, aszAdjectiveAndPLU, aszAdjectiveAndPLU, PRT_THCOLUMN);
					} else {
						_tcsncpy (aszPrintLine, aszAdjectiveAndPLU, 20);
					}
					PrtPrint(PMG_PRT_RECEIPT, aszPrintLine, tcharlen(aszPrintLine));
				}

				for ( j = 0; j < STD_MAX_COND_NUM; j++) {
					if (PluChitInfoList[i].aszCondMnemonic[j][0]) {
						if (ulPrintOptions & PLUCHITINFO_FLAG_PRINT_PLUCOLUMN) {
							RflFormatLineTwoColumn (aszPrintLine, &(PluChitInfoList[i].aszCondMnemonic[j][0]), &(PluChitInfoList[i].aszCondMnemonic[j][0]), PRT_THCOLUMN);
						} else {
							_tcsncpy (aszPrintLine, &(PluChitInfoList[i].aszCondMnemonic[j][0]), 20);
						}
						PrtPrint(PMG_PRT_RECEIPT, aszPrintLine, tcharlen(aszPrintLine));
					}
				}
				PrtFeed(PMG_PRT_RECEIPT, 1);            /* 1 line feed */
			}

			if ((ulPrintOptions & PLUCHITINFO_FLAG_PRINT_TICKET) == 0) {
				SHORT  iLines = 0;

				// Print the PLU ticket (chit) #/Type entered data in standard size, both sides
				// for non admission ticket type prints we print the #/Type information after the PLU info.
				for ( j = 0; j < NUM_OF_NUMTYPE_ENT; j++) {
					if (PluChitInfoList[i].aszNumber[j][0]) {
						if (ulPrintOptions & PLUCHITINFO_FLAG_PRINT_PLUCOLUMN) {
							PluChitInfoList[i].aszNumber[j][21] = 0;
							RflFormatLineTwoColumn (aszPrintLine, PluChitInfoList[i].aszNumber[j], PluChitInfoList[i].aszNumber[j], PRT_THCOLUMN);
						} else {
							_tcscpy (aszPrintLine, PluChitInfoList[i].aszNumber[j]);
						}
						PrtPrint(PMG_PRT_RECEIPT, aszPrintLine, tcharlen(aszPrintLine));
						iLines++;
					}
				}
				if (iLines > 0)
					PrtFeed(PMG_PRT_RECEIPT, 1);            /* 1 line feed */
			}
	//			PmgPrintBarCode (PMG_PRT_RECEIPT, PluChitInfoList[i].auchPLUNo, (PRT_BARCODE_TEXT_CENTER | PRT_BARCODE_TEXT_BELOW), ulBarcodeCode);
	//			PrtFeed(PMG_PRT_RECEIPT, 1);            /* 1 line feed */
			PrtTHTrail1(pTran);                                    /* send 1st line of trailer */
			PrtTHTrail2(pTran, pTran->TranGCFQual.ulStoreregNo);   /* send 2nd line of traile */
			PrtTransactionEnd(PRT_REGULARPRINTING, pTran->TranGCFQual.usGuestNo);
		}
	}
}

VOID PrtCallIPDTicketInfo ( TRANINFORMATION *pTran, SHORT hsStorage, PluChitInfoAll *PluChit )
{
    static UCHAR    auchItemData[ TRN_TEMPBUFF_SIZE ];
	PluChitInfo     *PluChitInfoList = 0;
    PRTIDX          *pPrtIdxHighest;
    PRTIDX          *pPrtIdxCurrent;
    ITEMSALES       *pItemSales;
    ITEMDISC        *pItemDisc;
    BOOL            fConsoli;
	BOOL			fAffectionDetected = FALSE;
	ULONG			ulActualBytesRead;
    ULONG           ulStorageActSize;
    ULONG           ulStorageReadOffset;
    ULONG           ulStorageReadSize;
    ULONG           ulTtlIdxReadSize;
    ULONG           ulCurIdxReadSize;
    SHORT           hsIndexFile;
    USHORT          usItemReadLen;
	SHORT           PluChitInfoListIndex = 0;
    PRTIDXHDR       IdxFileInfo;
    UCHAR           auchStorageWork[ TRN_TEMPBUFF_SIZE ];   /*endo update(fig->macro) 2000/1/7*/
    UCHAR           auchIdxWork[ sizeof( PRTIDX ) * FLEX_ITEMSTORAGE_MIN ];
    UCHAR           uchTtlItemCo;
    UCHAR           uchCurItemCo;
    UCHAR           uchItemCo;
	//11-11-03 SR 201 JHHJ

	if (PluChit) {
		PluChitInfoList = PluChit->pPluChitInfo;
	}
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

	pTran->TranCurQual.fsCurStatus2 &= ~CURQUAL_ORDERDEC_PRINTED;   // clear the order declare printed flag in case this is a reprint or dup

    /* --- retrieve information about index file --- */
    TrnReadFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), hsIndexFile , &ulActualBytesRead);

    ulTtlIdxReadSize = 0;
    uchTtlItemCo     = 0;
    uchCurItemCo     = ( sizeof( auchIdxWork ) / sizeof( PRTIDX ));

    while ( ulTtlIdxReadSize < IdxFileInfo.usIndexVli ) {
        TrnReadFile( (USHORT)(sizeof( PRTIDXHDR ) + ulTtlIdxReadSize), (VOID *)auchIdxWork, (USHORT)sizeof( auchIdxWork ), hsIndexFile, &ulCurIdxReadSize);
        if ( IdxFileInfo.usIndexVli < ( ulTtlIdxReadSize + ulCurIdxReadSize )) {
            ulCurIdxReadSize = IdxFileInfo.usIndexVli - ulTtlIdxReadSize;
            uchCurItemCo     = ( UCHAR )( ulCurIdxReadSize / sizeof( PRTIDX )) + 1;
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
			ulStorageReadOffset = pPrtIdxCurrent->usItemOffset;
			if (ulStorageReadOffset < 1) continue;  // if the offset is zero, invalid, then skip this index and go to next.
			TrnReadFile( ulStorageReadOffset, auchStorageWork, sizeof( auchStorageWork ), hsStorage, &ulStorageReadSize);

            /* --- unzip data and retrieve it's mnemonic --- */
            memset(&auchItemData, 0, sizeof(auchItemData));
            usItemReadLen = RflGetStorageItem( auchItemData, auchStorageWork, RFL_WITH_MNEM );

			// if this is an discount that is an item discount (not a regular discount) then we
			// will skip this transaction record since an item with its item discount are printed
			// in a single piece.  So the function RflIsSalesItemDisc() is used below to determine
			// if an item has a discount and if it does the item discount is printed immediately
			// after the item is printed.
            if (( auchItemData[ 0 ] == CLASS_ITEMDISC ) && ( auchItemData[ 1 ] == CLASS_ITEMDISC1 )) {
                continue;
            }

#if 0
			// ifdefed out for now in order to allow for Order Declare to be printed on the
			// take to kitchen receipt that is printed when a kitchen printer fails and the
			// take to kitchen receipt is printed on the thermal printer.
			// I am not sure why this check on printing a take to kitchen receipt and
			// saving the order declare number is being done however it definitely is
			// causing a problem with the order declare not being printed on the take to kitchen
			// receipt.
			//        Richard Chambers, Dec-17-2018
            if ( fsPrtStatus & PRT_TAKETOKIT ) {
                if ( PRT_NOTKPITEM == PrtChkKPItem( pTran, ( ITEMSALES * )auchItemData )) {
					if (PluChit && auchItemData [0] == CLASS_ITEMSALES ) {
						if (auchItemData[1] == CLASS_ITEMORDERDEC) {
							// if this is an Order Declare then lets remember the transaction mnemonic for later.
							// the order declare number of CLASS_ITEMORDERDEC is stored in the uchAdjective value.
							pItemSales   = ( ITEMSALES * )auchItemData;
							PluChit->iOrderDeclareAdr = TRN_ORDER_DEC_OFFSET;
							PluChit->iOrderDeclareAdr += pItemSales->uchAdjective;
						}
					}
                    continue;
                }
            }
#endif
            if ( fsPrtStatus & PRT_KPONLY ) {
                if ( ! (( auchItemData[ 0 ] == CLASS_ITEMSALES ) ||
                        ( auchItemData[ 0 ] == CLASS_ITEMTOTAL ) ||
                        ( auchItemData[ 0 ] == CLASS_ITEMPRINT ) ||
                        ( auchItemData[ 0 ] == CLASS_ITEMTRANSOPEN ))) {
                    continue;
                }
            }

            if ( fsPrtStatus & PRT_SUMMARY_PRINT ) {
				// a summary print is different from a split check.
				// split check is done in table service to split a check by seat number.
				// summary print is used with a split or partial tender.
                if ( ! (( auchItemData[ 0 ] == CLASS_ITEMSALES ) ||
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
				if (PluChit && auchItemData[1] == CLASS_ITEMORDERDEC) {
					// if this is an Order Declare then lets remember the transaction mnemonic for later.
					// the order declare number of CLASS_ITEMORDERDEC is stored in the uchAdjective value.
					PluChit->iOrderDeclareAdr = TRN_ORDER_DEC_OFFSET;
					PluChit->iOrderDeclareAdr += pItemSales->uchAdjective;
				}

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
#if 1
				if (PluChit && PluChitInfoList && pItemSales->sCouponQTY > 0 && PluChitInfoListIndex < PluChit->nPluChitInfoSize) {
					int  kLoop;
					for (kLoop = 0; kLoop < STD_MAX_COND_NUM; kLoop++) {
						memcpy (&(PluChitInfoList[PluChitInfoListIndex].aszCondMnemonic[kLoop][0]), pItemSales->Condiment[kLoop].aszMnemonic, sizeof(PluChitInfoList[0].aszCondMnemonic[kLoop]));
						PluChitInfoList[PluChitInfoListIndex].aszCondMnemonic[kLoop][STD_DEPTPLUNAME_LEN] = 0;  // ensure zero termination of mnemonic.
					}
					memcpy (PluChitInfoList[PluChitInfoListIndex].aszMnemonic, pItemSales->aszMnemonic, sizeof(PluChitInfoList[0].aszMnemonic));
					memcpy (PluChitInfoList[PluChitInfoListIndex].aszMnemonicAlt, pItemSales->aszMnemonicAlt, sizeof(PluChitInfoList[0].aszMnemonicAlt));
					memcpy (PluChitInfoList[PluChitInfoListIndex].aszNumber, pItemSales->aszNumber, sizeof(PluChitInfoList[0].aszNumber));
					memcpy (PluChitInfoList[PluChitInfoListIndex].auchPLUNo, pItemSales->auchPLUNo, sizeof(PluChitInfoList[0].auchPLUNo));
					PluChitInfoList[PluChitInfoListIndex].aszAdjMnemonic[0] = 0;
					if (pItemSales->uchAdjective != 0) {
						/* --get adjective mnemocnics -- */
						RflGetAdj(PluChitInfoList[PluChitInfoListIndex].aszAdjMnemonic,  pItemSales->uchAdjective);
					}
					PluChitInfoList[PluChitInfoListIndex].auchPLUNo[NUM_PLU_LEN - 1] = 0;
					PluChitInfoList[PluChitInfoListIndex].sQty = pItemSales->sCouponQTY;
					PluChitInfoList[PluChitInfoListIndex].usFamilyCode = pItemSales->usFamilyCode;
					PluChitInfoListIndex++;
				}
#endif
            }
#if 0
			// if defed out for now as we are not sure as to the impact on Amtrak.
			if (auchItemData[ 0 ] == CLASS_ITEMTOTAL) {
				// we have seen instances in which voided items were included in a total line if the subtotal key
				// is pressed before voiding the items.  The behavior is more pronounced when there are several
				// repetitions of entering items, pressing the subtotal, voiding items, pressing the total key.
				// the receipt shows incorrect first few total lines that included the price of voided items
				// so we will not show any total lines leading up to the first affectation except for
				// a food stamp total.
				fAffectionDetected = (fAffectionDetected || (auchItemData[ 1 ] == CLASS_TOTAL9));
				fAffectionDetected = (fAffectionDetected || (auchItemData[ 1 ] == CLASS_TOTAL2_FS));
				if (!fAffectionDetected) continue;
			}
			fAffectionDetected = (fAffectionDetected || (auchItemData[ 0 ] == CLASS_ITEMAFFECTION));
#endif
            if ( pTran->TranCurQual.uchPrintStorage == PRT_CONSOL_CP_EPT ) {
                PrtPrintCpEpt( pTran, auchItemData );
            } else {
                PrtPrintItem( pTran, auchItemData );
            }

            /* --- if this is sales item with item discount, print out item discount data --- */
            if (RflIsSalesItemDisc( auchItemData )) {
				SHORT    sQTYSave = pItemSales->sCouponQTY;

                /* read parent plu status for item discount void consolidation */
                if ((pItemSales->ControlCode.auchPluStatus[2] & PLU_SCALABLE) ||
                    (pItemSales->uchPM) ||  /* saratoga */
                    (pItemSales->uchPPICode)) {

                    sQTYSave = 1;    /* compulsory void print */
                }

                memset(&auchItemData, 0, sizeof(auchItemData));
                RflGetStorageItem( auchItemData, auchStorageWork + usItemReadLen, RFL_WITH_MNEM );

                /* void consolidation print */
                if ((pTran->TranCurQual.auchTotalStatus[4] & CURQUAL_TOTAL_NOCONSOLIDATE_PRINT) == 0) {
					// See the logic in function PrtPortion2() which determines if the item
					// is to be printed.
                    if (sQTYSave == 0 && CliParaMDCCheck(MDC_DEPT3_ADR, ODD_MDC_BIT2) != 0) {
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

VOID PrtCallIPD ( TRANINFORMATION *pTran, SHORT hsStorage )
{
	PrtCallIPDTicketInfo ( pTran, hsStorage, NULL );
}

VOID PrtPrintItemPrints ( TRANINFORMATION *pTran, SHORT hsStorage )
{
    SHORT           hsIndexFile;
    ULONG           ulStorageActSize;
    UCHAR           auchIdxWork[ sizeof( PRTIDX ) * FLEX_ITEMSTORAGE_MIN ];
    PRTIDXHDR       IdxFileInfo;
    PRTIDX          *pPrtIdxHighest;
    PRTIDX          *pPrtIdxCurrent;
    UCHAR           uchItemCo;
    ULONG           ulStorageReadOffset;
    ULONG           ulStorageReadSize;
    UCHAR           auchStorageWork[ TRN_TEMPBUFF_SIZE ];   /*endo update(fig->macro) 2000/1/7*/
    UCHAR           auchItemData[ sizeof( ITEMDATASIZEUNION ) ];
    USHORT          usItemReadLen;
    ULONG           ulTtlIdxReadSize;
    ULONG           ulCurIdxReadSize;
    UCHAR           uchTtlItemCo;
    UCHAR           uchCurItemCo;
    BOOL            fConsoli;
	ULONG			ulActualBytesRead;
	//11-11-03 SR 201 JHHJ

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
    TrnReadFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), hsIndexFile , &ulActualBytesRead);

    ulTtlIdxReadSize = 0;
    uchTtlItemCo     = 0;
    uchCurItemCo     = ( sizeof( auchIdxWork ) / sizeof( PRTIDX ));

    while ( ulTtlIdxReadSize < IdxFileInfo.usIndexVli ) {
        TrnReadFile( (sizeof( PRTIDXHDR ) + ulTtlIdxReadSize), auchIdxWork, sizeof( auchIdxWork ), hsIndexFile, &ulCurIdxReadSize);
        if ( IdxFileInfo.usIndexVli < ( ulTtlIdxReadSize + ulCurIdxReadSize )) {
            ulCurIdxReadSize = IdxFileInfo.usIndexVli - ulTtlIdxReadSize;
            uchCurItemCo     = ( UCHAR )( ulCurIdxReadSize / sizeof( PRTIDX )) + 1;
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
			ulStorageReadOffset = pPrtIdxCurrent->usItemOffset;
			if (ulStorageReadOffset < 1) continue;  // if the offset is zero, invalid, then skip this index and go to next.
			TrnReadFile( ulStorageReadOffset, auchStorageWork, sizeof( auchStorageWork ), hsStorage, &ulStorageReadSize);

			memset(&auchItemData, 0, sizeof(auchItemData));
			usItemReadLen = RflGetStorageItem( auchItemData, auchStorageWork, RFL_WITH_MNEM );
            if (auchItemData[ 0 ] == CLASS_ITEMPRINT) {
				PrtItemPrint(pTran, (ITEMPRINT *)auchItemData);
			} else if (auchItemData[ 0 ] == CLASS_ITEMTRANSOPEN) {
				PrtItemTransOpen(pTran, (ITEMTRANSOPEN *)auchItemData);
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
                          UCHAR  *puchWorkBuff,
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
                           UCHAR  *puchWorkBuff,
                           USHORT usBuffSize )
{
    UCHAR   uchUnzipLen;
    USHORT  usZipLen;

    /* --- determine unzipped data is out of work buffer --- */
    uchUnzipLen = *( puchWorkBuff + usItemOffset );
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
    TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};
	TCHAR  aszDemand[PARA_TRANSMNEMO_LEN * 2 + 1] = {0};
    SHORT   hsRead1, hsRead2;           /* storage file handle */
    SHORT  sRet;

	RflGetTranMnem(aszTranMnem, TRN_DEMAND_ADR);  /* get transaction mnem. */
    PrtDouble(aszDemand, (PARA_TRANSMNEMO_LEN * 2 + 1), aszTranMnem); /* convert mnemo. to double-wide */

    /* -- check storage pointer -- */
    if ( PRT_1PORTION == (sRet = PrtChkStorage(&hsRead1, &hsRead2, pTran)) ) {

        fsPrtStatus     |= PRT_DEMAND;          /* set print on demand status */
        fbPrtShrStatus  |= PRT_DEMAND; 

        PrtCallIPD(pTran, hsRead1);             /* decompress data and call IPD */

        /* -- printer alternation needed ? (in shared printer system) -- */ 
        if ( fbPrtAltrStatus & PRT_NEED_ALTR ) {                                

            fbPrtTHHead = 0;                    /* print from header */
            fbPrtShrStatus &= ~PRT_SHARED_SYSTEM;                                                    
            fbPrtAltrStatus &= ~PRT_WRITE_ABORT;    
                                                /* reset write abort status */
            PrtShrAltr(pTran, hsRead1);         /* retry from the beginning */
        }

        fsPrtStatus    = 0;                     /* initialize control area */
        fsPrtNoPrtMask = 0;   
        fsPrtCompul    = 0;

    }

    PrtPrint(PMG_PRT_RECEIPT, aszDemand, tcharlen(aszDemand)); /* ### Mod (2171 for Win32) V1.0 */
                                                /* "print on demand" line */
    PrtFeed(PMG_PRT_RECEIPT, 2);                /* feed,    R3.0 */

}

/*
 * PrtPrintSummaryReceipt - print a summary check for a split tender.
 * 
 * a summary check is generated when a split tender is performed by using two or more
 * different tenders for a guest check such as partially cash and partially credit.
 * a split tender is different from a split check. splitting a check is a table service
 * operation in which different seats are tendered out separately, the guest check is
 * split into separate checks by seat number.
 *
 * We are also using this when printing a Declined receipt for Electronic Payment. When used
 * we specify iPrintItems as -1 in order to by pass all the convoluted and complex print decision code.
**/
VOID PrtPrintSummaryReceipt(TRANINFORMATION *pTran, ITEMTENDER *pItemTender, int iPrintItems)
{
    SHORT   hsRead1, hsRead2;           /* storage file handle */
    SHORT   sRet;
	SHORT   fsPrtStatusSave = fsPrtStatus;
	SHORT   fsPrtPrintPortSave = fsPrtPrintPort;
	SHORT   fsPrtNoPrtMaskSave = fsPrtNoPrtMask;

	if (iPrintItems >= 0) {
		/* -- check storage pointer -- */
		sRet = PrtChkStorage(&hsRead1, &hsRead2, pTran);

		// Modify the print indicators for a split tender to ensure only receipt is printed.
		// No Electronic Journal printing.
		fsPrtStatus &= ~PRT_SERV_JOURNAL;    // make sure that a split tender receipt does not go into Electronic Journal
		fsPrtStatus |= PRT_SUMMARY_PRINT;
		fsPrtStatus &= ~PRT_TAKETOKIT;       // make sure no take to kitchen chit printed.
		if ( PRT_1PORTION == sRet) {
			if (iPrintItems) {
				PrtCallIPD(pTran, hsRead1);             /* decompress data and call IPD */
			} else {
				PrtPrintItemPrints ( pTran, hsRead1 );
			}
		}
	}
	else {
		// The following is to force a print to the receipt printer regardless of any settings.
		// The print code has so many checks and decisions that it is pratically impossible to
		// follow through especially with the dependency on the shared state in TrnInformation.
		fsPrtStatus = PRT_REC_SLIP;
		fsPrtPrintPort = PRT_RECEIPT;
		fsPrtNoPrtMask = PRT_RECEIPT;
	}

	{
		ITEMTOTAL  ItemTotal = {0};

		ItemTotal.uchMajorClass = CLASS_ITEMTOTAL;
		ItemTotal.uchMinorClass = CLASS_TOTAL1;
		ItemTotal.fsPrintStatus = pItemTender->fsPrintStatus;
		ItemTotal.auchTotalStatus[3] |= CURQUAL_TOTAL_PRINT_DOUBLE;            // indicate we want double size printing
		ItemTotal.lMI = pItemTender->lRound;
		PrtTotal(pTran, &ItemTotal);
	}

    if ( fsPrtPrintPort & PRT_RECEIPT ) {     /* thermal print */
		PrtFeed(PMG_PRT_RECEIPT, 1);              /* 1 line feed(Receipt) */
	}

	PrtItemTender(pTran, pItemTender);

	{
		ITEMPRINT  ItemPrint = {0};

		ItemPrint.fsPrintStatus = pItemTender->fsPrintStatus;
		memcpy (ItemPrint.uchUniqueIdentifier, pTran->TranGCFQual.uchUniqueIdentifier, sizeof(ItemPrint.uchUniqueIdentifier));
		ItemPrint.ulStoreregNo = pTran->TranGCFQual.ulStoreregNo;
		PrtTrail1(pTran, &ItemPrint);
	}

	fsPrtStatus = fsPrtStatusSave;   // return the setting back to what it was.
	fsPrtPrintPort = fsPrtPrintPortSave;
	fsPrtNoPrtMask = fsPrtNoPrtMaskSave;
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
    if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
        pItem->sCouponQTY = 1;      /* always print */
    } else {
        if (pItem->ControlCode.auchPluStatus[0] & PLU_MINUS) {
            pItem->sCouponQTY = (SHORT)(pItem->lQTY / PLU_BASE_UNIT);
        }

        if (pItem->usItemOffset) {
			ULONG			ulActualBytesRead;
			ITEMSALES       ItemVoidSales = {0};
			UCHAR           auchVoidWork[ sizeof(ITEMSALES) + 10];

            TrnReadFile( pItem->usItemOffset, auchVoidWork, sizeof( auchVoidWork ), sFileHandle, &ulActualBytesRead );
            RflGetStorageItem(&ItemVoidSales, auchVoidWork, RFL_WITH_MNEM );
            
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

            if (pTran->TranCurQual.fsCurStatus & CURQUAL_PRESELECT_MASK) {
                /* transaction void */
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
