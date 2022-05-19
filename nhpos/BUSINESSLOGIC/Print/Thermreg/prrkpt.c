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
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print  Kitchen printer
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRKP_.C
* --------------------------------------------------------------------------
* Abstract:
*   PrtKPInit()         :   initialize kitchen printer function
*   PrtKPHeader()       :   edit KP's header
**  PrtKPCustomerName() :   edit KP's customer name
*   PrtKPTrailer()      :   edit KP's trailer
*   PrtKPTotal()        :   edit KP's total item
**  PrtKPOperatorName() :   edit KP's operator name
*   PrtKPItem()         :   edit KP's items ( dept, plu )
*   PrtKPSetMenuItem()  :   edit KP's set menu items
*   PrtKPChild()        :   edit set menu's child items 
*   PrtKPWeight()       :   edit weight line
*   PrtKPQty()          :   edit qty line
*   PrtKPSeatNo()       :   edit item's seat no.
*   PrtKPNoun()         :   edit item's noun line
*   PrtKPPM()           :   edit item's condiment, printmodifier line
*   PrtChkKPStorage()   :   check KP's storage
*   PrtKPEnd()          :   edit KP's end information
*   PrtSetFailKP()      :   set failure KP
*   PrtChkTotalKP()     :   check total key status for KP
*   PrtChkTotalKP2()    :   check total key status for KP #5-8 R3.1
*   PrtChkKPPort()      :   write item's print portion for KP (main)
*   PrtChkKPPortSub()   :   write item's print portion for KP (sub)
*   PrtChkKPPortSub2()  :   write item's print portion for KP #5-8 R3.1(sub)
*   PrtChkKPItem()      :   check "take to kitchen" item
*   PrtTakeToKP()       :   take to kitchen print
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
* Jun-23-92 : 00.00.01 : K.Nakajima :
* Jun-29-93 : 01.00.12 : K.You      : mod. PrtKPSetMenuItem, PrtChkKPItem for US enhance.
* Mar-17-94 : 00.00.04 : K.You      : add flex GF file feature(mod. PrtTakeToKP).
* Mar-07-95 : 03.00.00 : T.Nakahata : Print Condiment of Promotional PLU and
*                                     Unique Trans No.(mod. PrtKPHeader)
* Jun-14-95 : 03.00.00 : T.Nakahata : add Total Mnemonic to Kitchen Feature
* Aug-29-95 : 03.00.04 : T.Nakahata : customer name to kitchen printer (FVT)
* Nov-15-95 : 03.01.00 : M.Ozawa    : print out cashier/server name
* Jul-11-96 : 03.01.09 : M.Ozawa    : correct condiment print position
* Dec-02-96 : 03.01.11 : M.Takeyama : Modify Kitchen Print Out 
* Jan-06-98 : 03.01.16 : M.Ozawa    : add non-consolidated kp print by seat no.
* Dec-16-99 : 01.00.00 : hrkato     : Saratoga
* May-01-00 :          : M.Teraki   : MDC #258, bit2 swept away.
* May-08-00 :          : M.Teraki   : fbPrtKPTarget removed.
* Jul-31-00 : V1.0.0.3 : M.Teraki   : Changed KP frame number to be managed for each KP
*
** GenPOS **
* Jan-11-18 : v2.2.2.0 : R.Chambers  : implementing Alt PLU Mnemonic in Kitchen Printing.
*============================================================================
*/

/*
=============================================================================
+                        I N C L U D E   F I L E s
=============================================================================
*/
/**------- MS - C ---------**/
#include	<tchar.h>
#include <stdlib.h>
#include <string.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <pif.h>
#include <regstrct.h>
#include <transact.h>
#include <paraequ.h>
#include <para.h>
#include <cskp.h>
#include <csstbkps.h>
#include <csstbstb.h>
#include <csstbpar.h>
#include <rfl.h>
#include <uie.h>
#include <fsc.h>
#include <pif.h>
#include <pmg.h>
#include "prt.h"
#include "prtTherm.h"
#include "prtrin.h"
#include "prrcolm_.h"

#include "display.h"
/**
;========================================================================
;+                    S T A T I C    R A M s
;========================================================================
**/
CONST TCHAR FARCONST  aszPrtKPGuest_CD[]= _T(" %4d%02d");         /* guest check no and check digit */
CONST TCHAR FARCONST  aszPrtKPGuest[]   = _T(" %4d");             /* guest check no */
CONST TCHAR FARCONST  aszPrtKPHeader[]  = _T("%-4s %-4s %-4s%s"); /* kitchen printer header */
CONST TCHAR FARCONST  aszPrtKPCustName[] = _T("%-19s");          /* customer name */
CONST TCHAR FARCONST  aszPrtKPTrailer[] = _T("%04d %-4s%8.8Mu %04lu-%03lu %s"); /* kitchen printer trailer */
CONST TCHAR FARCONST  aszPrtKPWeight[]  = _T("%.*l$%s");        /* weight */
CONST TCHAR FARCONST  aszPrtKPQuant[]   = _T("%ld X");          /* quantity */
CONST TCHAR FARCONST  aszPrtKPSeat[]    = _T("%-4s %1d"); //SR206 //_T("%-4s %2d");       /* seat no. */
CONST TCHAR FARCONST  aszPrtKPTotal[]   = _T("%s  %s");       /* total, V3.3 */
/* CONST TCHAR FARCONST  aszPrtKPTotal[]   = _T("%s    %s");       / total */
CONST TCHAR FARCONST  aszPrtKPOperator[] = _T("%-4s %-s");      /* operator */
CONST TCHAR FARCONST  aszPrtKPOperator2[] = _T("%-4s\n%-10s"); /* operator */
UCHAR	fchPrintDown;		/* save status of printer up or down */

/**
;============================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;============================================================================
**/

/* -- for message (sent Kitchen printer) control -- */  
UCHAR   uchPrtKPFrameNo[MAX_DEST_SIZE];                     /* message frame counter (seq. #) */     /* V1.0.0.3 */
USHORT  usPrtKPOffset[MAX_DEST_SIZE];                       /* message area offset in bytes not characters,    Saratoga */
TCHAR   auchPrtKPMessage[MAX_DEST_SIZE][KPS_FRAME_SIZE];    /* message area, Saratoga, should be same size as KPS_FRAME_SIZE */
UCHAR   fbPrtKPState;                                               /* save outputed kp number */
UCHAR   fbPrtKPSeatState;                                           /* save outputed kp number for Seat No, R3.1 */
UCHAR   fbPrtKPAbortState;                                          /* save outputed kp number */

/*
*===========================================================================
** Format  : VOID   PrtKPInit(VOID);
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function initialize kitchen printer.
*===========================================================================
*/
VOID   PrtKPInit(VOID)
{
    USHORT  i;

    fbPrtKPState = 0;                       /* outputed kp number */
    fbPrtKPAbortState = 0;                  /* kp is aborted  R3.1 */

    /* -- initialize message area -- */
    memset(auchPrtKPMessage, '\0', sizeof(auchPrtKPMessage));

    /* uchPrtKPFrameNo = 1; */         /* message frame counter(sequence no.) */ /* V1.0.0.3 */
    for (i = 0; i < MAX_DEST_SIZE; i++) {   /* Saratoga */
        uchPrtKPFrameNo[i] = 1;        /* message frame counter(sequence no.) */ /* V1.0.0.3 */
        auchPrtKPMessage[i][0] = KPS_START_FRAME;      /* set start status */
        auchPrtKPMessage[i][1] = uchPrtKPFrameNo[i];   /* set sequence number */ /* V1.0.0.3 */

        usPrtKPOffset[i] = 4;                       /* 4byte set to message area */
    }
}

/*
*===========================================================================
** Format  : VOID PrtKPOperatorName( TRANINFORMATION *pTran )
*
*   Input  : TRANINFORMATION    *pTran -   transaction information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function set operator mnemonic to kitchen printer buffer.
*===========================================================================
*/
VOID    PrtKPOperatorName(TRANINFORMATION *pTran)
{
    TCHAR   aszMnem[PARA_SPEMNEMO_LEN + 1] = {0};
    TCHAR   aszOpeMnem[NUM_OPERATOR + 1] = {0};
    USHORT  i;

    /*  --  get cashier or waiter's name and mnemonics -- */
    if ( PRT_CASHIER == PrtChkCasWai(aszMnem, pTran->TranModeQual.ulCashierID, pTran->TranModeQual.ulWaiterID) ) {
        _tcsncpy(aszOpeMnem, pTran->TranModeQual.aszCashierMnem, NUM_OPERATOR);
    } else {
        _tcsncpy(aszOpeMnem, pTran->TranModeQual.aszWaiterMnem, NUM_OPERATOR);
    }

    if (_tcslen(aszOpeMnem) > PRT_CASNAMEKP_LEN) {
        for (i = 0; i < MAX_DEST_SIZE; i++) {
            RflSPrintf(&auchPrtKPMessage[i][(usPrtKPOffset[i]/sizeof(TCHAR))],
                    (TCHARSIZEOF(auchPrtKPMessage[i]) - (usPrtKPOffset[i]/sizeof(TCHAR))),
                    aszPrtKPOperator2,  /* with CR */
                    aszMnem, aszOpeMnem);
        }

    } else {
        for (i = 0; i < MAX_DEST_SIZE; i++) {
            RflSPrintf(&auchPrtKPMessage[i][(usPrtKPOffset[i]/sizeof(TCHAR))],
                    (TCHARSIZEOF(auchPrtKPMessage[i]) - (usPrtKPOffset[i]/sizeof(TCHAR))),
                    aszPrtKPOperator, aszMnem, aszOpeMnem);
        }
    }
    /* -- fill space with kp column number, Saratoga -- */
    for (i = 0; i < MAX_DEST_SIZE; i++) {
        PrtFillSpace(&auchPrtKPMessage[i][(usPrtKPOffset[i]/sizeof(TCHAR))], PRT_KPS_OPERATOR_LEN);
        auchPrtKPMessage[i][(usPrtKPOffset[i]/sizeof(TCHAR)) + PARA_SPEMNEMO_LEN + 1 + (UCHAR)_tcslen(aszOpeMnem)] = '\0';
        usPrtKPOffset[i] += PRT_KPS_OPERATOR_LEN * sizeof(TCHAR);
    }
}

/*
*===========================================================================
** Format  : VOID   PrtKPHeader(TRANINFORMATION *pTran);
*
*   Input  : TRANINFORMATION *pTran      -transaction information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function set kitchen printer's header.
*
*            The kitchen printer header is used by the cooks and wait staff to
*            identify which kitchen request goes with which order.
*
*            At a minimum the kichen print indicates the waiter and an identifying
*            guest check number. Since much of the staff thinks in terms of table
*            number then if a table number is included in the transaction that is
*            printed as well.
*
*            If there is not a guest check number, pTran->TranGCFQual.usGuestNo == 0,
*            then we create an identifying number using the transactio consecutive number
*            to provide some way of separating out kitchen printer chits.
*            A guest check number is not provided when using Post Guest Check System
*            and the Operator just starts a transaction by entering an item rather
*            than using the New Check Key to start a transaction.
*===========================================================================
*/
VOID    PrtKPHeader(TRANINFORMATION *pTran)
{
	TCHAR   aszTableMnem[PARA_SPEMNEMO_LEN + 1] = {0};  /* PARA_... defined in "paraequ.h" */
    TCHAR   aszGuestMnem[PARA_SPEMNEMO_LEN + 1] = {0};  /* PARA_... defined in "paraequ.h" */
    TCHAR   aszTableNo[PRT_ID_LEN + 1] = {0};
    TCHAR   aszGuestNo[PRT_KPCOLUMN + 1] = {0};
    TCHAR   aszWork[PRT_KPCOLUMN + 1] = {0};
    USHORT  i;

    if (pTran->TranGCFQual.usTableNo != 0) {
        _itot(pTran->TranGCFQual.usTableNo, aszTableNo, 10);    /* convert table number to ascii, PrtKPHeader() */
		RflGetSpecMnem(aszTableMnem, SPC_TBL_ADR);              /* get table number mnemonic, PrtKPHeader()  */
    }
    if (pTran->TranModeQual.fsModeStatus & MODEQUAL_CASINT) {
        ;   /* not print GCF No. (inner use only), R3.3 */
    } else if (pTran->TranGCFQual.usGuestNo != 0) {             /* GCF#0, does not print */
		RflGetSpecMnem(aszGuestMnem, SPC_GC_ADR);               /* get GC no. menm */

        /* -- GCF no. with check digit -- */
        if (pTran->TranGCFQual.uchCdv != 0) {
            RflSPrintf(aszGuestNo, TCHARSIZEOF(aszGuestNo), aszPrtKPGuest_CD, pTran->TranGCFQual.usGuestNo, pTran->TranGCFQual.uchCdv);
        } else {
            RflSPrintf(aszGuestNo, TCHARSIZEOF(aszGuestNo), aszPrtKPGuest, pTran->TranGCFQual.usGuestNo);    // PrtKPHeader()
        }
	} else {
		// if there is no guest check number then we will use the consecutive number for this transaction
		// as an identifier for the kitchen printer chit. a guest check number of zero is usually due to a
		// temporary guest check such as when using Post Guest Check System and an immediate Cashier transaction
		// is rung up rather than starting a guest check using the New Check key.
		RflGetSpecMnem(aszGuestMnem, SPC_GUESTID_ADR);        /* get GC no. menm for PrtKPHeader() */
        RflSPrintf(aszGuestNo, TCHARSIZEOF(aszGuestNo), aszPrtKPGuest, pTran->TranCurQual.usConsNo);    // PrtKPHeader()
	}

    if ((pTran->TranGCFQual.usTableNo != 0) || (aszGuestNo[0] != 0)) {
        /* -- write table no & no of person,    Saratoga */
        for (i = 0; i < MAX_DEST_SIZE; i++) {
            RflSPrintf(&auchPrtKPMessage[i][(usPrtKPOffset[i]/sizeof(TCHAR))],
                   (TCHARSIZEOF(auchPrtKPMessage[i]) - (usPrtKPOffset[i]/sizeof(TCHAR))),
                   aszPrtKPHeader, aszTableMnem, aszTableNo, aszGuestMnem, aszGuestNo);
        }
    }

    /* -- fill space with kp column number -- */ /* if space fills header area, kpmanager doesnot print header */
    for (i = 0; i < MAX_DEST_SIZE; i++) {
        PrtFillSpace(&auchPrtKPMessage[i][(usPrtKPOffset[i]/sizeof(TCHAR))], PRT_KPCOLUMN);

        /* -- increment buffer write offset -- */
        usPrtKPOffset[i] += PRT_KPCOLUMN * sizeof(TCHAR);
    }
}

/*
*===========================================================================
** Format  : VOID   PrtKPCustomerName( UCHAR *pszCustomerName )
*
*   Input  : UCHAR *pszCustomerName - address of customer name buffer
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function sets customer name to KP's message buffer.
*===========================================================================
*/
VOID    PrtKPCustomerName(TCHAR *pszCustomerName)
{
    USHORT  i;

    if ( *( pszCustomerName + NUM_NAME - 2 ) == PRT_DOUBLE ) {
        *( pszCustomerName + NUM_NAME - 2 ) = '\0';
    }
    for (i = 0; i < MAX_DEST_SIZE; i++) {               /* Saratoga */
        RflSPrintf(&auchPrtKPMessage[i][(usPrtKPOffset[i]/sizeof(TCHAR))],
                (TCHARSIZEOF(auchPrtKPMessage[i]) - (usPrtKPOffset[i]/sizeof(TCHAR))),
                aszPrtKPCustName, pszCustomerName);
        PrtFillSpace(&auchPrtKPMessage[i][usPrtKPOffset[i]/sizeof(TCHAR)], NUM_NAME - 1);
        usPrtKPOffset[i] += (NUM_NAME - 1) * sizeof(TCHAR);
    }
}

/*
*===========================================================================
** Format  : VOID   PrtKPTrailer(TRANINFORMATION *pTran, ULONG  ulStReg);
*
*   Input  : TRANINFORMATION *pTran     -transaction information
*            ULONG           ulStReg    -store  reg number
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function set KP trailer.
*===========================================================================
*/
VOID    PrtKPTrailer(TRANINFORMATION *pTran, ULONG ulStReg)
{
    TCHAR  aszMnem[PARA_SPEMNEMO_LEN + 1] = {0};
    TCHAR  aszDate[PRT_DATETIME_LEN + 1] = {0};
    ULONG  ulID;
    USHORT i;

    /*  --  get cashier or waiter's name and mnemonics -- */
    if ( PRT_CASHIER == PrtChkCasWai(aszMnem, pTran->TranModeQual.ulCashierID, pTran->TranModeQual.ulWaiterID) ) {
        ulID = RflTruncateEmployeeNumber(pTran->TranModeQual.ulCashierID);
    } else {
        ulID = RflTruncateEmployeeNumber(pTran->TranModeQual.ulWaiterID);
    }

    PrtGetDate(aszDate, TCHARSIZEOF(aszDate), pTran);   /* get date time */

    for (i = 0; i < MAX_DEST_SIZE; i++) {
        RflSPrintf(&auchPrtKPMessage[i][(usPrtKPOffset[i]/sizeof(TCHAR))], (TCHARSIZEOF(auchPrtKPMessage[i]) - (usPrtKPOffset[i]/sizeof(TCHAR))),
               aszPrtKPTrailer,  pTran->TranCurQual.usConsNo, aszMnem, ulID, ulStReg / 1000L, ulStReg % 1000L, aszDate);
        /* -- fill space with kp column number -- */
        PrtFillSpace(&auchPrtKPMessage[i][(usPrtKPOffset[i]/sizeof(TCHAR))], KPS_TRAILER_SIZE);

        /* -- increment buffer write offset -- */
        usPrtKPOffset[i] += KPS_TRAILER_SIZE * sizeof(TCHAR);
    }
}

/*
*===========================================================================
** Format  : VOID PrtKPTotal( ITEMTOTAL *pItem )
*
*   Input  : ITEMTOTAL       *pItem -   total item information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function set total mnemonic to kitchen printer buffer.
*===========================================================================
*/
VOID    PrtKPTotal(ITEMTOTAL *pItem)
{
    USHORT  usTtlAddr;
    USHORT  fchKP, fchMask = 0x01;
    TCHAR   szTranMnem[ PARA_TRANSMNEMO_LEN + 1 ] = {0};
    TCHAR   szAmount[ PARA_SPEMNEMO_LEN + PRT_AMOUNT_LEN + 1 ] = {0};
    TCHAR   szItem[(PRT_KPCOLUMN + 1) * 2] = {0};
    USHORT  usItemLen, i;

    if ((( pItem->auchTotalStatus[ 0 ] / CHECK_TOTAL_TYPE ) == PRT_TRAY1 ) || (( pItem->auchTotalStatus[ 0 ] / CHECK_TOTAL_TYPE ) == PRT_TRAY2 )) {
        /* --- tray total key is not print on kitchen printer --- */
        return;
    }

    /* -- get transaction mnemonics -- */
    usTtlAddr = RflChkTtlAdr( pItem );
	RflGetTranMnem( szTranMnem, usTtlAddr );

    /* -- adjust native mnemonic and amount sign(+.-) -- */
    PrtAdjustNative( szAmount, pItem->lMI );

    memset( szItem, '\0', sizeof( szItem ));
    RflSPrintf(szItem, TCHARSIZEOF(szItem), aszPrtKPTotal, szTranMnem, szAmount);

    /* -- fill space with kp column number -- */
    usItemLen = tcharlen( szItem );
    szItem[ usItemLen++ ] = PRT_RETURN;

    fchKP  = ITM_TTL_FLAG_CRT_5_8(pItem) << 4 | ITM_TTL_FLAG_CRT_1_4(pItem);

    for (i = 0, fchMask = 0x01; i < MAX_DEST_SIZE; i++, fchMask <<= 1) {
        if (fchKP & fchMask) {
            PrtChkKPStorageEx(szItem, usItemLen, fbPrtKPState, 0, i);
        }
    }
}

/*
*===========================================================================
** Format  : VOID   PrtKPItem(TRANINFORMATION *pTran, ITEMSALES *pItem);
*
*   Input  : TRANINFORMATION *pTran     -transaction information
*            ITEMSALES       *pItem     -item information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function set KP sales items(PLU, DEPT).
*===========================================================================
*/
VOID    PrtKPItem(TRANINFORMATION *pTran, ITEMSALES *pItem)
{
    TCHAR   aszItem[PRT_KPMAXITEM] = {0};
    USHORT  usItemOffset = 0;
    UCHAR   fbPrtStatus;                        /* print status for kp */
    UCHAR   fbPrtStatus2 = 0;                   /* print status for void */
    USHORT  usWork = 0, i;
    USHORT  fchKP, fchMask = 0x01;
    UCHAR   fbStatWork;                          /* print status for kp */
    SHORT   sReturn, sReturn2;

    if (fbPrtKPAbortState & PRT_KPABORT || 
		(pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_7] & PLU_GIFT_CARD) ||
		(pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_7] & PLU_EPAYMENT)) {
        return ;
    }

    /* -- Check status -- */
    if ( PRT_NOTKPITEM == PrtChkKPPort(pTran, pItem, &fbPrtStatus, PRT_MDC) ) {
        return ;
    }

    /* -- Not print voided item at k/p R3.1, V3.3 */
    if ( pTran->TranCurQual.uchKitchenStorage == PRT_KIT_NET_STORAGE) {
        if (pItem->sCouponQTY == 0) {
            return;
        }
    }

    fbPrtKPState |= fbPrtStatus;                /* store output kp  */

    /* -- preselect void? -- */
    if ( pTran->TranCurQual.fsCurStatus & CURQUAL_PVOID ) {
        fbPrtStatus2 |= PRT_KPVOID;              /* set void status */
    }

    /* -- void? -- */
    if ( pItem->fbModifier & VOID_MODIFIER ) {
        fbPrtStatus2 |= PRT_KPVOID;              /* set void status */
        if ( pTran->TranCurQual.fsCurStatus & CURQUAL_PRESELECT_MASK ) {
            fbPrtStatus2 &= ~PRT_KPVOID;         /* reset void status */
        }
    }

    memset(aszItem, '\0', sizeof(aszItem));     /* initialize */

    /*fchKP  = pItem->ControlCode.auchStatus[6];*/
    fchKP  = ((pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_6] & PLU_SALES_KP_NO_5_8) << 4); /* ### MOD 2172 Rel1.0 (02/22/00) */
    fchKP |= ((pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2] & PLU_SALES_KP_NO_1_4) >> 4);

    /* -- print seat no. R3.1 -- */
    if ( CliParaMDCCheck(MDC_KPTR_ADR, ODD_MDC_BIT0) ) {
        /* each item print */
        if (pItem->uchSeatNo) {
             usItemOffset += PrtKPSeatNo(aszItem, PRT_KPCOLUMN+1, pItem); /* seat no. */
        }
    } else {
        /* transaction print */
        usItemOffset += PrtKPSeatNo(aszItem, PRT_KPCOLUMN+1, pItem); /* seat no. */

        if (usItemOffset) {
            if (uchPrtSeatNo != pItem->uchSeatNo) {
                fbPrtKPSeatState = 0;
            }
            if (~fbPrtKPSeatState & fbPrtStatus) {  /* have not printed to kp yet */
                for (i = 0, fchMask = 0x01; i < MAX_DEST_SIZE; i++, fchMask <<= 1) {
                    if (fchKP & fchMask) {
                        PrtChkKPStorageEx(aszItem, usItemOffset, (UCHAR)(~fbPrtKPSeatState & fbPrtStatus), 0, i);
                    }
                }
            }

            memset(aszItem, '\0', sizeof(aszItem));     /* initialize */
            usItemOffset = 0;
        }
        uchPrtSeatNo = pItem->uchSeatNo;
        fbPrtKPSeatState |= fbPrtStatus;
    }

    /* -- form qty or weight line -- */
    if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
        usItemOffset += PrtKPWeight(&aszItem[usItemOffset], PRT_KPCOLUMN+1, pTran, pItem);
    } else {
        if (( pTran->TranCurQual.uchKitchenStorage == PRT_KIT_NET_STORAGE) &&
            (!(pTran->TranCurQual.auchTotalStatus[4] & CURQUAL_TOTAL_NOCONSOLIDATE_PRINT))) { /* consolidation print */
            usItemOffset += PrtKPQty(&aszItem[usItemOffset], PRT_KPCOLUMN+1, (LONG)pItem->sCouponQTY);
        } else {
            usItemOffset += PrtKPQty(&aszItem[usItemOffset], PRT_KPCOLUMN+1, pItem->lQTY / PLU_BASE_UNIT);
        }
    }
	
	if ((!CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)) && (CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT1))) {
		// if US Customs #/type entry bit turned off and we are printing #/Type entry to kitchen printer
		// then lets print any #/Type or Ref. Num. associated with this item.
		for(i=0; i<NUM_OF_NUMTYPE_ENT; i++) {//US Customs
			if(pItem->aszNumber[i][0] == '\0'){
				break;
			}
			usItemOffset += PrtKPNumber(&aszItem[usItemOffset],PRT_KPCOLUMN+1,pItem->aszNumber[i]);
		}
	}

    usItemOffset += PrtKPNoun(&aszItem[usItemOffset], pItem); /* noun mnem. */
	NHPOS_ASSERT(usItemOffset < sizeof(aszItem)/sizeof(aszItem[0]));
    usItemOffset += PrtKPPM(&aszItem[usItemOffset], pItem);   /* PM & condiment */
	NHPOS_ASSERT(usItemOffset < sizeof(aszItem)/sizeof(aszItem[0]));

    if (pItem->usLinkNo) {
        fbStatWork=0;
        sReturn = PrtChkKPPortSub(&fbStatWork, (UCHAR)((pItem->Condiment[0].ControlCode.auchPluStatus[2]) >> 4), pTran, PRT_MDC);
        sReturn2 = PrtChkKPPortSub2(&fbStatWork, (UCHAR)(pItem->Condiment[0].ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_6] << 4), pTran, PRT_MDC);

        if ((sReturn == PRT_KPITEM) || (sReturn2 == PRT_KPITEM)) {
            usItemOffset += PrtKPChild(&aszItem[usItemOffset], pItem, 0);
        }
    }
    
	if ((CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)) && (CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT1))) {
		// if US Customs #/type entry bit turned off and we are printing #/Type entry to kitchen printer
		// then lets print any #/Type or Ref. Num. associated with this item.
		for(i=0; i<NUM_OF_NUMTYPE_ENT; i++) {//US Customs
			if(pItem->aszNumber[i][0] == '\0'){
				break;
			}
			usItemOffset += PrtKPNumber(&aszItem[usItemOffset],PRT_KPCOLUMN+1,pItem->aszNumber[i]);
		}
	}

    /*-- form a blank line after every PLU --*/
    if ( CliParaMDCCheck(MDC_KP2_ADR, EVEN_MDC_BIT0) ) {    /* V3.3 */
        aszItem[usItemOffset] = PRT_RETURN; /* write return */ 
        usItemOffset++;
    }

    for (i = 0, fchMask = 0x01; i < MAX_DEST_SIZE; i++, fchMask <<= 1) {
        if (fchKP & fchMask) {
            PrtChkKPStorageEx(aszItem, usItemOffset, fbPrtStatus, fbPrtStatus2, i);
        }
    }
}

/*
*===========================================================================
** Format  : VOID   PrtKPSetMenuItem(TRANINFORMATION *pTran, ITEMSALES *pItem);
*
*   Input  : TRANINFORMATION *pTran     -transaction information
*            ITEMSALES       *pItem     -item information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function set KP sales items (Setmenu).
*===========================================================================
*/
VOID   PrtKPSetMenuItem(TRANINFORMATION *pTran, ITEMSALES *pItem)
{
    TCHAR   aszItem[PRT_KPMAXITEM] = {0};
    TCHAR   auchDummy[NUM_PLU_LEN] = {0};
    USHORT  usItemOffset;
    UCHAR   fbPrtPort=0;                         /* print status for kp */
    UCHAR   fbCurPrtState2 = 0;                  /* current print status for void */
    USHORT  usWork = 0;
    USHORT  i,j;
    USHORT  fchKP, fchMask;

    if (fbPrtKPAbortState & PRT_KPABORT) {
        return;
    }

    if ( (pItem->fsPrintStatus & PRT_SPCL_PRINT) && (pItem->uchPrintModNo == 0) ) {
        return;
    }

    /* -- not print voided item to k/p R3.1 -- */
    if ((pTran->TranCurQual.uchKitchenStorage == PRT_KIT_NET_STORAGE) &&
        (!(pTran->TranCurQual.auchTotalStatus[4] & CURQUAL_TOTAL_NOCONSOLIDATE_PRINT))) { /* consolidation print */
        if (pItem->sCouponQTY == 0) {
            return;
        }
    }

    /* check setmenu children plu's kp portion */
    for ( j = 0; j < pItem->uchChildNo; j++ ) {
		UCHAR   fbStatWork;                          /* print status for kp */

        /* check child PLU exist or not */
        if (_tcsncmp(pItem->Condiment[j].auchPLUNo, auchDummy, NUM_PLU_LEN) == 0) {
            continue;
        }

        fbStatWork=0;
        PrtChkKPPortSub(&fbStatWork, (UCHAR)( (pItem->Condiment[j].ControlCode.auchPluStatus[2]) >> 4), pTran, PRT_MDC);

        /* ---- set printer port #5 - #8    R3.1 ---- */
        PrtChkKPPortSub2(&fbStatWork, (UCHAR)( (pItem->Condiment[j].ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_6]) << 4), pTran, PRT_MDC);

        fbPrtPort |= fbStatWork;
    }

    /* fchKP  = (pItem->ControlCode.auchStatus[6]); */
    fchKP  = ((pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_6] & PLU_SALES_KP_NO_5_8) << 4); /* ### MOD 2172 Rel1.0 (02/22/00) */
    fchKP |= ((pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2] & PLU_SALES_KP_NO_1_4) >> 4);

    for ( i = 0; i < MAX_DEST_SIZE;  i++ ) {
		UCHAR   fbCurPrtState = (1 << i);                       /* current print status*/

        /* check setmenu children plu's kp portion */
        if ( !(fbPrtPort & fbCurPrtState) ) {
            continue;
        }

        fbPrtKPState |= fbCurPrtState;               /* store output kp  */

        /* -- preselect void? -- */
        if ( pTran->TranCurQual.fsCurStatus & CURQUAL_PRESELECT_MASK ) {
            fbCurPrtState2 |= PRT_KPVOID;              /* set void status */
        }

        /* -- void? -- */
        if ( pItem->fbModifier & VOID_MODIFIER ) {
            fbCurPrtState2 |= PRT_KPVOID;              /* set void status */

            if ( pTran->TranCurQual.fsCurStatus & CURQUAL_PRESELECT_MASK ) {
                fbCurPrtState2 &= ~PRT_KPVOID;         /* reset void status */   
            }
        }

        usItemOffset = 0;
        memset(aszItem, '\0', sizeof(aszItem));     /* initialize */

        /* -- print seat no. R3.1 -- */
        if ( CliParaMDCCheck(MDC_KPTR_ADR, ODD_MDC_BIT0) ) {
            /* each item print */
            if (pItem->uchSeatNo) {
                usItemOffset += PrtKPSeatNo(aszItem, PRT_KPCOLUMN+1, pItem); /* seat no. */
            }

        } else {
            /* transaction print */
            usItemOffset += PrtKPSeatNo(aszItem, PRT_KPCOLUMN+1, pItem); /* seat no. */

            if (usItemOffset) {
                if (uchPrtSeatNo != pItem->uchSeatNo) {
                    fbPrtKPSeatState = 0;
                }
                if (~fbPrtKPSeatState & fbCurPrtState) {  /* have not printed to kp yet */

                    for (i = 0, fchMask = 0x01; i < MAX_DEST_SIZE; i++, fchMask <<= 1) {
                        if (fchKP & fchMask) {
                            PrtChkKPStorageEx(aszItem, usItemOffset, (UCHAR)(~fbPrtKPSeatState & fbCurPrtState), 0, i);
                        }
                    }
                }

                memset(aszItem, '\0', sizeof(aszItem));     /* initialize */
                usItemOffset = 0;
            }
            uchPrtSeatNo = pItem->uchSeatNo;
            fbPrtKPSeatState |= fbCurPrtState;
        }

        /* -- form qty or weight line -- */
        if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
            usItemOffset += PrtKPWeight(&aszItem[usItemOffset], PRT_KPCOLUMN+1,  pTran, pItem);
        } else {
            if ((pTran->TranCurQual.uchKitchenStorage == PRT_KIT_NET_STORAGE) &&
                (!(pTran->TranCurQual.auchTotalStatus[4] & CURQUAL_TOTAL_NOCONSOLIDATE_PRINT))) { /* consolidation print */

                usItemOffset += PrtKPQty(&aszItem[usItemOffset], PRT_KPCOLUMN+1, (LONG)pItem->sCouponQTY);
            } else {
                usItemOffset += PrtKPQty(&aszItem[usItemOffset], PRT_KPCOLUMN+1, pItem->lQTY / PLU_BASE_UNIT);
            }
        }

		if ((!CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)) && (CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT1))) {
			// if US Customs #/type entry bit turned off and we are printing #/Type entry to kitchen printer
			// then lets print any #/Type or Ref. Num. associated with this item.
			for(i=0; i<NUM_OF_NUMTYPE_ENT; i++) {//US Customs
				if(pItem->aszNumber[i][0] == '\0'){
					break;
				}
				usItemOffset += PrtKPNumber(&aszItem[usItemOffset],PRT_KPCOLUMN+1,pItem->aszNumber[i]);
			}
		}

        usItemOffset += PrtKPNoun(&aszItem[usItemOffset], pItem); /* noun mnem. */
		NHPOS_ASSERT(usItemOffset < sizeof(aszItem)/sizeof(aszItem[0]));
        usItemOffset += PrtKPPM(&aszItem[usItemOffset], pItem);   /* PM & condiment */
		NHPOS_ASSERT(usItemOffset < sizeof(aszItem)/sizeof(aszItem[0]));

        for ( j = 0; j < pItem->uchChildNo; j++ ) {
			UCHAR   fbStatWork = 0;                          /* print status for kp */

            /* check child PLU exist or not */
            if (_tcsncmp(pItem->Condiment[j].auchPLUNo, auchDummy, NUM_PLU_LEN) == 0) {
                continue;
            }

            if ( (pTran->TranCurQual.flPrintStatus) & CURQUAL_SETMENU_KITCHEN ) {
                break;
            }

            PrtChkKPPortSub(&fbStatWork, (UCHAR)((pItem->Condiment[j].ControlCode.auchPluStatus[2]) >> 4), pTran, PRT_MDC);
            PrtChkKPPortSub2(&fbStatWork, (UCHAR)(pItem->Condiment[j].ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_6] << 4), pTran, PRT_MDC);

            if (fbStatWork & fbCurPrtState) {
                usItemOffset += PrtKPChild(&aszItem[usItemOffset], pItem, j);
            }
        }

		if ((!CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)) && (CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT1))) {
			for (i = 0; i < NUM_OF_NUMTYPE_ENT; i++) {    //US Customs
				if(pItem->aszNumber[i][0] == '\0'){
					break;
				}
				usItemOffset += PrtKPNumber(&aszItem[usItemOffset],PRT_KPCOLUMN+1,pItem->aszNumber[i]);
			}
		}

        /*-- form a blank line after every PLU --*/
        if ( CliParaMDCCheck(MDC_KP2_ADR, EVEN_MDC_BIT0) ) {    /* V3.3 */
            aszItem[usItemOffset] = PRT_RETURN; /* write return */ 
            usItemOffset++;
        }

        for (i = 0, fchMask = 0x01; i < MAX_DEST_SIZE; i++, fchMask <<= 1) {
            if (fchKP & fchMask) {
                PrtChkKPStorageEx(aszItem, usItemOffset, fbCurPrtState, fbCurPrtState2, i);
            }
        }
    }
}

/*
*===========================================================================
** Format  : USHORT   PrtKPChild(UCHAR *pszDest, ITEMSALES  *pItem,
*                                                USHORT usCondNo);
*
*   Input  : UCHAR      *pszDest        -destination buffer
*            ITEMSALES  *pItem          -item information
*            USHORT      usCondNo       -condiment no.
*   Output : none
*   InOut  : none
** Return  : USHORT usWriteLen          -written length
*
** Synopsis: This function write settmenu's children PLU.
*===========================================================================
*/
USHORT  PrtKPChild(TCHAR *pszDest, ITEMSALES  *pItem, USHORT usCondNo)
{
    TCHAR   *pszStart;
    USHORT  usMnemLen;
    USHORT  i;  /* saratoga */
    USHORT  usOverLen = 0;  /* char length overflowed from 1st line */

    pszStart = pszDest;
    i = 0;

    /* indent 2 character width */
    *pszStart = PRT_SPACE;
    pszStart++;
    i++;
    *pszStart = PRT_SPACE;
    pszStart++;
    i++;

    /* --get adjective mnemocnics -- */
    if (pItem->Condiment[usCondNo].uchAdjective != 0) {
		TCHAR   aszAdjMnem[PARA_ADJMNEMO_LEN + 1] = {0};

        /* --get adjective mnemocnics -- */
		RflGetAdj(aszAdjMnem, pItem->Condiment[usCondNo].uchAdjective);

        usMnemLen = tcharlen(aszAdjMnem);

        _tcsncpy(pszStart, aszAdjMnem, usMnemLen);
        pszStart += usMnemLen;
        i += usMnemLen;

        *pszStart = PRT_SPACE;
        pszStart++;
        i++;
    }

    usMnemLen = tcharlen(pItem->Condiment[usCondNo].aszMnemonic);
    
    /* saratoga */
    if ((i + usMnemLen) > PRT_KPCOLUMN) {   /* overs kp column */
        usOverLen = (i + usMnemLen) - PRT_KPCOLUMN;
        usMnemLen = (PRT_KPCOLUMN - i);
    }
    
    _tcsncpy(pszStart, pItem->Condiment[usCondNo].aszMnemonic, usMnemLen);
    pszStart += usMnemLen;

    *pszStart = PRT_RETURN;           /* write return */
    pszStart++;

    /* return if all character was placed on 1st line */
    if (usOverLen == 0) {
        return ((USHORT)(pszStart - pszDest));
    }

    /* indent 2nd line 2 char width */
    *pszStart = PRT_SPACE;
    pszStart++;
    *pszStart = PRT_SPACE;
    pszStart++;

    /* place characters remained */
    _tcsncpy(pszStart, pItem->Condiment[usCondNo].aszMnemonic + usMnemLen, usOverLen);
    pszStart += usMnemLen;

    *pszStart = PRT_RETURN;           /* write return */
    pszStart++;

    return ( (USHORT)(pszStart - pszDest) );
}

/*
*===========================================================================
** Format  : USHORT   PrtKPWeight(UCHAR *pszDest, USHORT usDestLen,
*                                 TRANINFORMATION *pTran, ITEMSALES *pItem);
*   Input  : USHORT usDestLen           -destination buffer length
*            TRANINFORMATION *pTran,    -transaction information
*            ITEMSALES *pItem           -item information
*   Output : UCHAR  *pszDest            -destination buffer
*   InOut  : none
** Return  : USHORT usWriteLen          -write length
*
** Synopsis: This function formats kp's weight line to specified buffer.
*===========================================================================
*/
USHORT   PrtKPWeight(TCHAR *pszDest, USHORT usDestLen,
                        TRANINFORMATION *pTran, ITEMSALES *pItem)
{
	TCHAR  aszWeightSym[PARA_SPEMNEMO_LEN + 1] = {0};  /* PARA_... defined in "paraequ.h" */
    LONG   lModQty;                              /* modified qty */
    SHORT  sDecPoint;                            /* decimal point */
    USHORT usStrLen;

    /* -- get scalable symbol and adjust qty -- */
    PrtGetScale(aszWeightSym, &sDecPoint, &lModQty, pTran, pItem);

    /* -- format string -- */
    RflSPrintf(pszDest, usDestLen, aszPrtKPWeight, sDecPoint, lModQty, aszWeightSym);

    usStrLen = tcharlen(pszDest);

    pszDest[usStrLen] = PRT_RETURN;   /* write return */

    return (usStrLen + 1);            /* return written length, 1 for return character */
}

/*
*===========================================================================
** Format  : PrtKPNumber(TCHAR *pszDest, USHORT usDestLen,TCHAR  *pszNumber);
*   Input  : USHORT usDestLen            -destination buffer length
*            TCHAR  *pszNumber           -item number
*   Output : UCHAR  *pszDest            -destination buffer
*   InOut  : none
** Return  : USHORT usWriteLen          -write length
*
** Synopsis: This function formats kp number.
*===========================================================================
*/
USHORT PrtKPNumber(TCHAR *pszDest, USHORT usDestLen,TCHAR  *pszNumber)
{
    if (*pszNumber != 0) {
		TCHAR   aszNumLine[PRT_THCOLUMN * 2 + 1] = {0};
		USHORT  usMnemoLen, usNoLen;

		RflGetTranMnem(aszNumLine, TRN_NUM_ADR );
        usMnemoLen = tcharlen(aszNumLine);
        aszNumLine[usMnemoLen] = PRT_SPACE;
        usNoLen = tcharlen(pszNumber);

		_tcsncpy(&aszNumLine[usMnemoLen + 1], pszNumber, usNoLen);
		_tcsncpy(pszDest,aszNumLine,usMnemoLen+usNoLen+1);

		pszDest[usMnemoLen+usNoLen+1] = PRT_RETURN;
		return (usMnemoLen+usNoLen+2);
    }

	return 0;
}

/*
*===========================================================================
** Format  : USHORT  PrtKPQty(UCHAR *pszDest, USHORT usDestLen, LONG lQty);
*
*   Input  : USHORT usDestLen           -destination buffer length
*            LONG   lQty                -Qty
*   Output : UCHAR  *pszDest            -destination buffer
*   InOut  : none
** Return  : USHORT usWriteLen          -write length
*
** Synopsis: This function formats kp's Qty line to specified buffer.
*===========================================================================
*/
USHORT   PrtKPQty(TCHAR *pszDest, USHORT usDestLen, LONG lQty)
{
    USHORT usStrLen;

    if (labs(lQty) == 1L) { /* changed at R3.1 */
        return (0);
    }

    RflSPrintf(pszDest, usDestLen, aszPrtKPQuant, lQty);

    usStrLen = tcharlen(pszDest);
	
	//Perform MDC check to determine if Qty should be printed above or on the same line as
	//the item. SR 243 ESMITH
	if(!CliParaMDCCheck(MDC_QTY2LINE_ADR, EVEN_MDC_BIT3)){
		pszDest[usStrLen] = PRT_RETURN;   /* write return */
	} else {
		pszDest [usStrLen] = PRT_SPACE;	  /* write space  */
	}

    return (usStrLen + 1);            /* return written length, 1 for return character */
}

/*
*===========================================================================
** Format  : USHORT   PrtKPSeatNo(UCHAR *pszDest, USHORT usDestLen, ITEMSALES *pItem);
*
*   Input  : USHORT usDestLen           -destination buffer length
*            ITEMSALES *pItem           -seat no
*   Output : UCHAR *pszDest             -destination buffer address
*   InOut  : none
** Return  : USHORT usWriteLen          -write length
*
** Synopsis: This function formats seat no. line to . R3.1
*===========================================================================
*/
USHORT    PrtKPSeatNo(TCHAR *pszDest, USHORT usDestLen, ITEMSALES *pItem)
{
    USHORT usStrLen = 0;

    if (pItem->uchSeatNo) {
		TCHAR   aszSpecMnem[ PARA_SPEMNEMO_LEN + 1 ] = {0};   /* PARA_... defined in "paraequ.h" */

		RflGetSpecMnem( aszSpecMnem, SPC_SEAT_ADR );    /* seat no. mnemonic */
        RflSPrintf(pszDest, usDestLen, aszPrtKPSeat, aszSpecMnem, pItem->uchSeatNo);
        usStrLen = tcharlen(pszDest);
    }

    pszDest[usStrLen] = PRT_RETURN;   /* write return */

    return (usStrLen + 1);            /* return written length, 1 for return character */
}

/*
*===========================================================================
** Format  : USHORT   PrtKPNoun(UCHAR *pszDest, ITEMSALES *pItem);
*
*   Input  : ITEMSALES *pItem           -noun mnemonics
*   Output : UCHAR *pszDest             -destination buffer address
*   InOut  : none
** Return  : USHORT usWriteLen          -write length
*
** Synopsis: This function formats noun line to .
*===========================================================================
*/
USHORT    PrtKPNoun(TCHAR *pszDest, ITEMSALES *pItem)
{
    DCURRENCY lProduct;
    USHORT usStrLen;

    if ((_tcsncmp(pItem->auchPLUNo, MLD_NO_DISP_PLU_LOW, NUM_PLU_LEN) >= 0) &&
        (_tcsncmp(pItem->auchPLUNo, MLD_NO_DISP_PLU_HIGH, NUM_PLU_LEN) <= 0)) {
        if ((pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2] & PLU_SALES_KP_NO_1_4) == 0 &&
			(pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_6] & PLU_SALES_KP_NO_5_8) == 0)
		{       /* 50RFC05513 */
            /* check whether condiment/print modifier exist or not */
            if ((pItem->uchPrintModNo + pItem->uchCondNo ) == 0) {
                return (0);     /* not print plu no 9950 - 9999 */
            }
        }
    }

    PrtSetItem(pszDest, pItem, &lProduct, PRT_ONLY_NOUN_AND_ALT, 0);

    usStrLen = tcharlen(pszDest);

    pszDest[usStrLen] = PRT_RETURN;   /* write return */

    return (usStrLen + 1);            /* return written length, 1 for return character */
}

/*
*===========================================================================
** Format  : USHORT   PrtKPPM(UCHAR *pszDest, ITEMSALES  *pItem);
*
*   Input  : UCHAR      *pszDest        -destination buffer
*            ITEMSALES  *pItem          -item information
*   Output : none
*   InOut  : none
** Return  : USHORT usWriteLen          -written length
*
** Synopsis: This function write printmodifier and condiment to buffer.
*===========================================================================
*/
USHORT  PrtKPPM(TCHAR *pszDest, ITEMSALES  *pItem)
{
    TCHAR  *pszStart;
    DCURRENCY   lProduct;
    TCHAR  aszWork[PRT_KPMAXITEM] = {0};
    USHORT usWrite = 0;
    USHORT usColumn = 0;

    pszStart = aszWork;

    /* separated print option of condiment, V3.3 */
    if ( CliParaMDCCheck( MDC_KP2_ADR, EVEN_MDC_BIT1 )) {
        usColumn = PRT_KPCOLUMN - 1;
    }

    PrtSetItem(aszWork, pItem, &lProduct, PRT_ONLY_PMCOND_AND_ALT, usColumn); /* set item's mnemonics to buffer */

    if (*aszWork == '\0') {
        return(0);
    }

    pszDest[0] = KPS_RED_CODE;                      /* set RED */
    usWrite++;                                      /* inc. write length */

    for (;;) {
		USHORT usWork;
		TCHAR  *pszEnd;

        /*---- correct condiment print position 07/11/96 ----*/
        pszDest[usWrite] = PRT_SPACE;
        usWrite++;

        /* -- get 1 line -- */
        if ( PRT_LAST == PrtGet1Line(pszStart, &pszEnd, PRT_KPCOLUMN-1) ) {
            usWork = tcharlen(pszStart);

            if (usWork != 0) {
                _tcsncpy(&pszDest[usWrite], pszStart, usWork);
                usWrite += usWork;
                pszDest[usWrite] = PRT_RETURN;           /* write return */
                usWrite ++;                              /* for "return"'s length */
            }
            break;
        }

        usWork = pszEnd - pszStart;
        _tcsncpy(&pszDest[usWrite], pszStart, usWork);
        usWrite += usWork;
        pszDest[usWrite] = PRT_RETURN;           /* write return */
        usWrite ++;                              /* for "return"'s length */
        pszDest[usWrite] = KPS_RED_CODE;         /* write RED */
        usWrite ++;                              /* for "RED"'s length */
        pszStart = pszEnd + 1;                   /* 1 for space */
    }

/*** Delete from this Line for (R3.1 V11) ******
    if ( (usWrite != 0) && (pszDest[usWrite-1] == KPS_RED_CODE) ) {

        usWrite--;
    }
 *** Delete to this Line for (R3.1 V11) ******/
/*** Add from this Line for (R3.1 V11) ******/
    if ( (usWrite != 0) && (pszDest[usWrite-2] == KPS_RED_CODE) ) {
        usWrite -= 2;
    }
/*** Add to this Line for (R3.1 V11) ******/

    return (usWrite);
}

/*
*===========================================================================
** Format  : SHORT   PrtChkKPStorageEx(UCHAR *pszBuff, 
*                           UCHAR uchBuffLen, CHAR fbState, USHORT usTerm)
*
*   Input  : UCHAR *pszBuff                  -buffer pointer
*            UCHAR uchBuffLen                -buffer length
*            CHAR  fbState                   -kp print status
*   Output : SHORT PRT_SUCCESS               -kp manager returns success
*                  PRT_FAILE                 -kp manager returns fail
*   InOut  : none
** Return  : none
*
** Synopsis: This function write to kp storage.
*===========================================================================
*/
SHORT   PrtChkKPStorageEx(TCHAR *pszBuff, 
                      USHORT usBuffLen, CHAR fbPrtState, CHAR fbState, USHORT usTerm)
{
    PARASHAREDPRT   SharedPrt = {0};

    SharedPrt.uchMajorClass = CLASS_PARASHRPRT;
    SharedPrt.uchAddress = (UCHAR)(SHR_KP1_DEF_ADR + usTerm);
    CliParaRead(&SharedPrt);
#if !defined(USE_2170_ORIGINAL)
    /* --- Fix a glitch (05/22/2001)
        To avoid sending print data to invalid numbered KP device,
        exit the function when shared printer is not specified...
    --- */
    if (SharedPrt.uchTermNo == 0) {
        return (PRT_SUCCESS);
    }
#endif
    /* -- check rest of kp storage, Saratoga -- */
    if (usBuffLen * sizeof(TCHAR) + usPrtKPOffset[usTerm] > sizeof(auchPrtKPMessage[0])) {
		USHORT          usPrevious;
		USHORT          usKPState;
		USHORT          usRet;
		SHORT           sReturn;

        /* ### MOD 2172 Rel1.0 (Saratoga) CliKpsPrintEx -> PrtSendKps */

        /* allow power switch at error display */
        usPrevious = PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */

        while (1) {

            sReturn = PrtSendKps(&auchPrtKPMessage[usTerm][0], usPrtKPOffset[usTerm], CliReadUAddr(), &usKPState, SharedPrt.uchTermNo, (SHORT)(usTerm+1));

            if (KPS_TIMER_ON == sReturn) {
                break;
            }

            if (KPS_PRINTER_DOWN == sReturn) {      /* Added (V1.0.0.4) */
                break;
            }

            if (KPS_BUFFER_FULL == sReturn) {
                fbPrtKPAbortState |= PRT_KPABORT;
                PrtSetFailKP(0);                    /* "0" means  "all kp error" */
                PifControlPowerSwitch(usPrevious);  /* V1.0.04 */
                return (PRT_FAIL);
            }
            /* allow power switch at error display */
            PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */
            usRet = UieErrorMsg(KpsConvertError(sReturn), UIE_ABORT);

            /* if abort key entered, set receipt compul and exit */
            if (usRet == FSC_CANCEL || usRet == FSC_P5) {
                fbPrtKPAbortState |= PRT_KPABORT;
                PrtSetFailKP(0);                    /* "0" means  "all kp error" */
                PifControlPowerSwitch(usPrevious);  /* V1.0.04 */
                return (PRT_FAIL);
            }
        }
        PifControlPowerSwitch(usPrevious);  /* V1.0.04 */

        uchPrtKPFrameNo[usTerm] += 1;   /* increment frame counter */   /* V1.0.0.4 */

        auchPrtKPMessage[usTerm][0] = KPS_CONTINUE_FRAME;   /* write comm. frame information */

        /* write comm. frame counter(sequence number) */
        auchPrtKPMessage[usTerm][1] = uchPrtKPFrameNo[usTerm];  /* V1.0.0.4 */
        usPrtKPOffset[usTerm] = 4;
    }

    /* -- write to kp storage -- */
    //divide usPrtKPOffset by the sizeof(TCHAR) to get the index because usPrtKPOffset is the number of bytes
	//memcpy the buffer because it contains data that could be 0 in addition to the text in the buffer
	//a string copy might not get the whole buffer
	{
		USHORT  usPrtOffset = usPrtKPOffset[usTerm] / sizeof(TCHAR);

		/* increment for "data length" and "status" */
		auchPrtKPMessage[usTerm][usPrtOffset] = (usBuffLen + 3) * sizeof(TCHAR);  /* write length */
		usPrtOffset++;
		auchPrtKPMessage[usTerm][usPrtOffset] = fbPrtState;               /* write status for printer */
		usPrtOffset++;
		auchPrtKPMessage[usTerm][usPrtOffset] = fbState;                  /* void status R3.1 */
		usPrtOffset++;
		memcpy(&auchPrtKPMessage[usTerm][usPrtOffset], pszBuff, usBuffLen * sizeof(TCHAR));
		usPrtKPOffset[usTerm] += (usBuffLen + 3) * sizeof(TCHAR);         /* increment storage write offset */
	}

    return(PRT_SUCCESS);
}

/*
*===========================================================================
** Format  : VOID   PrtKPEnd(VOID);
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function send message to kp and set fail kp.
*===========================================================================
*/
VOID PrtKPEnd(VOID)
{
    USHORT          fchMask;
    USHORT          i;
    USHORT          usKPStateWork;       /* work area to update usKPState */
    USHORT          usKPState = 0x0000;  /* status save area for all KP# */

    if (fbPrtKPState == 0) {             /* "fbPrtKPState" is storeage area of output kp number */
        return ;                         /* "== 0" means that there is no item to send. */
    }

    for (i = 0, fchMask = 0x01; i < MAX_DEST_SIZE; i++, fchMask <<= 1) {
		PARASHAREDPRT   SharedPrt = {0};
		USHORT          usRet;
		SHORT           sReturn, sError;

		SharedPrt.uchMajorClass = CLASS_PARASHRPRT;
        SharedPrt.uchAddress = (UCHAR)(SHR_KP1_DEF_ADR + i);
        CliParaRead(&SharedPrt);

        if (SharedPrt.uchTermNo == 0) {
            continue;
        }

        if (fbPrtKPState & fchMask) {
			USHORT          usPrevious1;

            auchPrtKPMessage[i][0] |= KPS_END_FRAME;/* write end status */

            /* allow power switch at error display */
            usPrevious1 = PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */

            while (1) {

                /* do print */
                /* ### MOD 2172 Rel1.0 (Saratoga) CliKpsPrintEx -> PrtSendKps */
                sReturn = PrtSendKps(&auchPrtKPMessage[i][0], usPrtKPOffset[i], CliReadUAddr(),
                                     &usKPStateWork, SharedPrt.uchTermNo, (SHORT)(i+1));

                /* update status */
                usKPState |= usKPStateWork;

                if (sReturn == KPS_PERFORM) {
					fchPrintDown = 0;
					flDispRegDescrControl &= ~PRT_OFFLINE_CNTRL;
					flDispRegKeepControl &= ~PRT_OFFLINE_CNTRL;
                    break;
                }
                if (sReturn & KPS_DOWN_PRINTER) { //SR456
					if (fchPrintDown == 0) {
						USHORT          usPrevious2;
						REGDISPMSG		regDispMsg = {0};

						/* Display Error and turn on descriptor */
						regDispMsg.uchMajorClass = CLASS_UIFREGDISP;
						regDispMsg.fbSaveControl = 1;

						/* allow power switch at error display */
						usPrevious2 = PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE);
						switch(sReturn) {
						case KPS_DOWN_AND_OPEN:
							sError = LDT_PRT_COVEROPEN_ADR;
							break;
						case KPS_DOWN_AND_OUT:
							sError = LDT_PRT_PAPERCHNG_ADR;
							break;
						default:
							sError = LDT_PRTTBL_ADR;
							break;
						}

						UieErrorMsg(sError, UIE_WITHOUT);
						PifControlPowerSwitch(usPrevious2);
						
						flDispRegDescrControl |= PRT_OFFLINE_CNTRL;
						flDispRegKeepControl |= PRT_OFFLINE_CNTRL;
						DispWrite(&regDispMsg);
						fchPrintDown = 1;
					}
                    /* if already known the printer is dead, error message is not needed. */
                    break;
                }

                if (KPS_BUFFER_FULL == sReturn) {
                    fbPrtKPAbortState |= PRT_KPABORT;
                    PrtSetFailKP(0);                    /* "0" means  "all kp error" */
                    PifControlPowerSwitch(usPrevious1);  /* V1.0.04 */
                    return;
                }
                /* allow power switch at error display */
                PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */
                usRet = UieErrorMsg(KpsConvertError(sReturn), UIE_ABORT);
                
                /* if abort key entered, set receipt compul and exit */
                if (usRet == FSC_CANCEL || usRet == FSC_P5) {
                        fbPrtKPAbortState |= PRT_KPABORT;
                        PrtSetFailKP(0);                    /* "0" means  "all kp error" */
                        PifControlPowerSwitch(usPrevious1);  /* V1.0.04 */
                        return;
                }
            }
            PifControlPowerSwitch(usPrevious1);  /* V1.0.04 */
        }
    }

    PrtSetFailKP(usKPState);             /* check kp status and set fail kp */
    fbPrtKPState = 0;                    /* initialize outputed kp number */
    fbPrtKPAbortState = 0;               /* initialize outputed kp number */
}

/*
*===========================================================================
** Format  : SHORT  PrtSetFailKP(USHORT usKPState)
*
*   Input  : USHORT  usKPState               -kitchen printer status
*   Output : none
*   InOut  : none
** Return  : SHORT  PRT_SUCCESS              -success
*                   PRT_FAIL                 -kitchen printer error
*
** Synopsis: This function checks kp status and write fail kp.
*
*            See function PrtPrintTran() which clears the auchPrtFailKP[]
*            array before starting to print the transaction data on the
*            receipt printer as well as sending the transaction data to the
*            kitchen printing functionality.
*===========================================================================
*/
SHORT  PrtSetFailKP(USHORT usKPState)
{
    usKPState = ~usKPState;           /* reverse bit */
    usKPState &= fbPrtKPState;        /* print portion check */
/*    usKPState &= PRT_KPMASK;          / mask */

    if (usKPState) {
		int  i;
		USHORT usBit = 0x01;
		for (i = 0; i < MAX_DEST_SIZE; i++, usBit <<= 1) {
			// look over the bit map of failed kitchen printers and mark those
			// that are failed or clear those that are not failed.
			// See also PrtPrintTran() which clears the auchPrtFailKP[] array.
			if (usKPState & usBit) {
				auchPrtFailKP[i] = i + 1;
			}
		}

        return (PRT_FAIL);
    }
    return (PRT_SUCCESS);
}

/*
*===========================================================================
** Format  : VOID PrtChkTotalKP(TRANINFORMATION *pTran, UCHAR *puchState, UCHAR fbType)
*
*   Input  : TRANINFORMATION *pTran,         -transaction information
*            UCHAR *puchState                -kitchen printer status address
*            UCHAR fbType                    -set type
*                                               PRT_MDC: refer to MDC control
*                                               PRT_NOT_MDC: does not refer
*
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function write total key's kp status & Check MDC.
*            MDC(1010) Item(1001) -> PORT(0100)
*            MDC(0000) Item(1001) -> PORT(1001)
*            MDC(1100) Item(1001) -> PORT(0100)
*            MDC(0110) Item(1001) -> PORT(1100)
*===========================================================================
*/
VOID PrtChkTotalKP(TRANINFORMATION *pTran, UCHAR *puchState, UCHAR  fbType)
{
    /* -- check total keys status -- */
    if (pTran->TranCurQual.auchTotalStatus[3] & CURQUAL_TOTAL_KITPRT_MENU_SEND) {   /* "and" logic */
        *puchState &= (pTran->TranCurQual.auchTotalStatus[3] & CURQUAL_TOTAL_KITPRT_PRINTER_MASK);

    } else {                                                 /* "or" logic  */
        *puchState |= (pTran->TranCurQual.auchTotalStatus[3] & CURQUAL_TOTAL_KITPRT_PRINTER_MASK);
    }

    return;

    /*---- check mdc of kds at PrtChkTotalKP2(), R3.1 ----*/
    fbType = fbType;
}

/*
*===========================================================================
** Format  : VOID PrtChkTotalKP2(TRANINFORMATION *pTran, UCHAR *puchState, UCHAR fbType)
*
*   Input  : TRANINFORMATION *pTran,         -transaction information
*            UCHAR *puchState                -kitchen printer status address
*            UCHAR fbType                    -set type
*                                               PRT_MDC: refer to MDC control
*                                               PRT_NOT_MDC: does not refer
*
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function write total key's kp status & Check MDC
*            for printer #5 - #8. R3.1
*===========================================================================
*/
VOID PrtChkTotalKP2(TRANINFORMATION *pTran, UCHAR *puchState, UCHAR  fbType)
{
    UCHAR fbWork1;
    PARAMDC MDC;

    /* -- check total keys status -- */
    fbWork1 = (UCHAR)(*puchState & 0xF0);  /* save kp#1 - #4 */
    if (pTran->TranCurQual.auchTotalStatus[3] & CURQUAL_TOTAL_KITPRT_MENU_SEND) {   /* "and" logic */

        fbWork1 &= ((pTran->TranCurQual.auchTotalStatus[5] & CURQUAL_TOTAL_KITPRT_PRINTER_MASK) << 4);

    } else {                                                 /* "or" logic  */

        fbWork1 |= ((pTran->TranCurQual.auchTotalStatus[5] & CURQUAL_TOTAL_KITPRT_PRINTER_MASK) << 4);
    }
    *puchState = (UCHAR)(*puchState & 0x0F);
    *puchState |= fbWork1;

    if (fbType == PRT_NOT_MDC) {
        return;
    }

    /* -- check MDC of KDS-- */
    MDC.uchMajorClass = CLASS_PARAMDC;
    MDC.usAddress = MDC_DSP_ON_FLY;
    CliParaRead(&MDC);

    fbWork1 = (UCHAR)~(MDC.uchMDCData);
    *puchState = *puchState & fbWork1;  /* clear each bit associated to KDS */
}

/*
*===========================================================================
** Format  : SHORT PrtChkKPPort(TRANIFORMATION  *pTran, ITEMSALES *pItem,
*                                           UCHAR *puchPort, UCHAR fbType)
*
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMSALES        *pItem     -Item information address
*            UCHAR            *puchPort  -KP Port
*            UCHAR            fbType     -set type
*                                               PRT_MDC: refer to MDC control
*                                               PRT_NOT_MDC: does not refer
*   Output : none
*    InOut : none
** Return  : PRT_KPITEM                -Kitchen item
*            PRT_NOTKPITEM             -not kitchen item
*
** Synopsis: This function writes item's print portion (kitchen printer).
*
*===========================================================================
*/
SHORT  PrtChkKPPort(TRANINFORMATION *pTran, ITEMSALES *pItem, UCHAR *puchPort, UCHAR fbType)
{
    /* normal item without condiment or PM,  does not print */
    if (  (pItem->fsPrintStatus & PRT_SPCL_PRINT) && (pItem->uchCondNo + pItem->uchPrintModNo == 0) ) {

	/* 1998/10/16 Yamamoto Change start */
		/*
				return(PRT_NOTKPITEM);
		*/
		// instead of using the PLU status  along with the total key status to determine the
		// remote devices, use only the total key settings for remote devices.
        *puchPort = 0;

        /* check kitchen printer #1 - 4 */
        PrtChkTotalKP(pTran, puchPort, fbType);

        /* check kitchen printer #5 - 8 */
        PrtChkTotalKP2(pTran, puchPort, fbType);

        if (*puchPort == 0) {
            return (PRT_NOTKPITEM);
        }

        return (PRT_KPITEM);

	/* Change end */
    }

	*puchPort = PrtChkKPPortToUse (pItem->ControlCode.auchPluStatus, pTran, fbType);

	return (*puchPort) ? PRT_KPITEM : PRT_NOTKPITEM;
}

/*
*===========================================================================
** Format  : SHORT   PrtChkKPPortSub(UCHAR *puchPortion,  UCHAR  fbPrtState,
*                                   TRANINFORMATION  *pTran, fbType)
*
*    Input : CHAR             *puchPortion   -modified KP Port
*            UCHAR            fbPrtState     -Item's KP Portion
*            TRANINFORMATION  *pTran         -Transaction information address
*            UCHAR            fbType         -set type
*                                               PRT_MDC: refer to MDC control
*                                               PRT_NOT_MDC: does not refer
*   Output : none
*    InOut : none
** Return  : PRT_KPITEM                -Kitchen item
*            PRT_NOTKPITEM             -not kitchen item
*
** Synopsis: This function writes item's print portion (kitchen printer).
*            Check is for a remote printer #1 through #4
*
*===========================================================================
*/
SHORT   PrtChkKPPortSub(UCHAR *puchPortion,  UCHAR  fbPrtState,
                                    TRANINFORMATION  *pTran, UCHAR fbType)
{
    *puchPortion = (UCHAR)(fbPrtState & PRT_KPMASK);

    PrtChkTotalKP(pTran, puchPortion, fbType);   /* check total key status (for KP) */

    if (*puchPortion == 0) {             /* no output portion */
        return(PRT_NOTKPITEM);
    }

    return(PRT_KPITEM);
}

/*
*===========================================================================
** Format  : USHORT   PrtChkKPPortToUse (UCHAR  *auchPluStatus, TRANINFORMATION  *pTran, fbType)
*
*    Input : UCHAR            *auchPluStatus   -PLU status array which contains remote device indicators
*            TRANINFORMATION  *pTran           -Transaction information address
*            UCHAR            fbType           -set type
*                                                PRT_MDC: remove any remote CRTs
*                                                PRT_NOT_MDC: do not remove any remote CRTs
*   Output : none
*    InOut : none
** Return  : usKpPorts             - bit map of which remote devices are kitchen printers.
*
** Synopsis: This function determines which remote devices are used for kitchen printing
*            for a PLU. The PLU status array contains which remote devices are to be used
*            when processing the PLU and generating kitchen printer output.
*
*            This function generates a bit map of which remote devices to output to by
*            looking at the remote devices specified in the PLU status array then filtering
*            based on the Total Key settings specified in the P60 Total Key settings.
*
*            A Total Key can be provisioned to include additional remote printers by either
*            using a logical AND with the PLU status array or a logical OR with the PLU status array.
*
*            A logical AND means that a PLU is only sent to a remote device if BOTH the
*            PLU status array and the Total Key provisioning indicates the remote device.
*
*            A logical OR means that a PLU is sent to a remote device if EITHER the
*            PLU status array and the Total Key provisioning indicates a remote device.

**===========================================================================
*/
USHORT   PrtChkKPPortToUse (UCHAR  *auchPluStatus, TRANINFORMATION  *pTran, UCHAR fbType)
{
	USHORT  usKpPorts = 0;
	USHORT  usKpTotalPorts = 0;

	usKpPorts  = auchPluStatus[2] >> 4;
	usKpPorts |= auchPluStatus[6] << 4;

    usKpTotalPorts = (pTran->TranCurQual.auchTotalStatus[3] & CURQUAL_TOTAL_KITPRT_PRINTER_MASK);
    usKpTotalPorts |= ((pTran->TranCurQual.auchTotalStatus[5] & CURQUAL_TOTAL_KITPRT_PRINTER_MASK) << 4);

	/* -- check total keys status -- */
    if (pTran->TranCurQual.auchTotalStatus[3] & CURQUAL_TOTAL_KITPRT_MENU_SEND) {   /* "and" logic */
        usKpPorts &= usKpTotalPorts;
    } else {                                                 /* "or" logic  */
        usKpPorts |= usKpTotalPorts;
    }

    if (fbType == PRT_MDC) {
		PARAMDC MDC = {0};

		/* -- check MDC of KDS-- */
		MDC.uchMajorClass = CLASS_PARAMDC;
		MDC.usAddress = MDC_DSP_ON_FLY;     // getting full byte on odd MDC address so both MDC_DSP_ON_FLY and MDC_DSP_ON_FLY2
		CliParaRead(&MDC);

		// clear each bit associated to remote CRT (KDS).
		// invert MDC data to make remote printer 1 and remote CRT 0
		usKpPorts = usKpPorts & ( ~ MDC.uchMDCData );
    }

    return(usKpPorts);
}

/*
*===========================================================================
** Format  : SHORT   PrtChkKPPortSub2(UCHAR *puchPortion,  UCHAR  fbPrtState,
*                                   TRANINFORMATION  *pTran, fbType)
*
*    Input : CHAR             *puchPortion   -modified KP Port
*            UCHAR            fbPrtState     -Item's KP Portion
*            TRANINFORMATION  *pTran         -Transaction information address
*            UCHAR            fbType         -set type
*                                               PRT_MDC: refer to MDC control
*                                               PRT_NOT_MDC: does not refer
*   Output : none
*    InOut : none
** Return  : PRT_KPITEM                -Kitchen item
*            PRT_NOTKPITEM             -not kitchen item
*
** Synopsis: This function writes item's print portion (kitchen printer).
*            Check is for a remote printer #5 - #8. R3.1
*
*===========================================================================
*/
SHORT   PrtChkKPPortSub2(UCHAR *puchPortion,  UCHAR  fbPrtState,
                                    TRANINFORMATION  *pTran, UCHAR fbType)
{
    *puchPortion |= (UCHAR)(fbPrtState & PRT_KPMASK2);

    PrtChkTotalKP2(pTran, puchPortion, fbType);   /* check total key status (for KP) */

    if (*puchPortion == 0) {             /* no output portion */
        return(PRT_NOTKPITEM);
    }

    return(PRT_KPITEM);
}


/*
*===========================================================================
** Format  : SHORT PrtChkKPItem(TRANIFORMATION  *pTran, ITEMSALES *pItem)
*
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*            UCHAR            *puchItem  -sales item data
*   Output : none
*    InOut : none
** Return  : PRT_KPITEM                -Kitchen item
*            PRT_NOTKPITEM             -not kitchen item
*
** Synopsis: This function check "take to kitchen" item.
*
*===========================================================================
*/
SHORT PrtChkKPItem(TRANINFORMATION *pTran, ITEMSALES *pItem)
{
    UCHAR    fbStatus = 0;

    switch ( pItem->uchMajorClass ) {
    case CLASS_ITEMTOTAL:       /* total item */
        if (( ! CliParaMDCCheck( MDC_KPTR_ADR, ODD_MDC_BIT0 )) && ( CliParaMDCCheck( MDC_KPTR_ADR, ODD_MDC_BIT3 ))) {
            if ( pItem->uchMinorClass == CLASS_TOTAL1 ) {
				/* --- sub total key is not print on kitchen printer --- */
                return (PRT_NOTKPITEM);
            }
            return (PRT_KPITEM);
        }
        return (PRT_NOTKPITEM);

    case CLASS_ITEMSALES:       /* sales item */
        break;

    default:                    /* others */
        return (PRT_NOTKPITEM);
    }

    if (( pItem->uchMinorClass == CLASS_SETMENU ) || ( pItem->uchMinorClass == CLASS_SETITEMDISC ) ) {
		USHORT   i;
		TCHAR    auchDummy[NUM_PLU_LEN] = {0};
		UCHAR    uchMaxChild;

        if (( pItem->fsPrintStatus & PRT_SPCL_PRINT ) && ( pItem->uchPrintModNo == 0 )) {
            return (PRT_NOTKPITEM);
        }

        /* check setmenu children's kp print portion */
        uchMaxChild = pItem->uchChildNo + pItem->uchCondNo + pItem->uchPrintModNo;
        for ( i = 0; i < uchMaxChild; i++ ) {
			UCHAR    fbStatWork = 0;

            /* check condiment PLU exist or not */
			// If the Plu exists, whether it is a condiment/child or Print Modifier
			// then pItem->Condiment[i].auchPLUNo will contain data that needs to be
			// printed. If there is no value in pItem->Condiment[i].auchPLUNo, ie the
			// data that is set in pItem->Condiment[i].auchPLUNo == "", then this value
			// will match the same value which is in the auchDummy variable, which is
			// "". When the tcsncmp operation is performed it will return 0 if the values
			// match (no data), > 0 when there is a value in pItem->Condiment[i].auchPLUNo,
			// and the operation will never be < 0 (which would mean auchPLUNo's substring
			// would have to be less than auchDummy, which is "").
            if (_tcsncmp(pItem->Condiment[i].auchPLUNo, auchDummy, NUM_PLU_LEN) == /*!=*/ 0) {
                continue;
            }
            if ( pItem->Condiment[ i ].ControlCode.auchPluStatus[ 2 ] & PLU_CONDIMENT ) {
                continue;
            }

            PrtChkKPPortSub(&fbStatWork, (UCHAR)((pItem->Condiment[i].ControlCode.auchPluStatus[2]) >> 4), pTran, PRT_MDC);
            PrtChkKPPortSub2(&fbStatWork, (UCHAR)((pItem->Condiment[i].ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_6]) << 4), pTran, PRT_MDC);
            fbStatus |= fbStatWork;
        }
/****
        if (( pItem->uchMinorClass == CLASS_OEPPLU ) ||
            ( pItem->uchMinorClass == CLASS_OEPITEMDISC )) {
            PrtChkKPPortSub( &fbStatWork,
                            ( UCHAR )( pItem->fsPrintStatus >> 4 ),
                            pTran,
                            PRT_MDC);
            fbStatus |= fbStatWork;
        }
****/
    } else {
        /* check if kp item or not */
        PrtChkKPPort(pTran, pItem, &fbStatus, PRT_MDC);
    }

    /* check "take to kitchen" item */
    /* "uchPrtCurKP" set at PrtTakeToKP() */
    if ( fbStatus & uchPrtCurKP ) {
        return (PRT_KPITEM);
    }

    return (PRT_NOTKPITEM);
}

/*
*===========================================================================
** Format  : VOID PrtTakeToKP(TRANIFORMATION *pTran, SHORT hsStorage)
*
*    Input : TRANINFORMATION  *pTran    - Transaction information address
*            SHORT            hsStorage - storage file handle
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints "take to kitchen" item.
*
*===========================================================================
*/
VOID PrtTakeToKP(TRANINFORMATION *pTran, SHORT hsStorage)
{
    /* -- not print "take to kitchen"(depend on MDC) -- */
    if (!(pTran->TranCurQual.flPrintStatus & CURQUAL_TAKEKITCHEN)) {
        UieErrorMsg(LDT_KTNERR_ADR, UIE_ABORT);
        return ;
    }

    if (( PifSysConfig()->uchOperType == DISP_2X20 ) ||
        ( PifSysConfig()->uchOperType == DISP_LCD  )) {
        UieDescriptor(UIE_OPER, 3, UIE_DESC_ON);/* take to k/p desc. ON */
    }

	if (CliParaMDCCheckField(MDC_PLU_TICKET, MDC_PARAM_BIT_B)) {
		// MDC set to over ride take to kitchen in order to print individual PLU tickets
		// we process the transaction data to generate a list of PLU tickets to print and
		// then we print them.
		BOOL            freeMalloced;
		PluChitInfoAll  PluChit;
		PluChitInfo     PluChitInfoList[25] = {0};

		fsPrtStatus = PRT_TAKETOKIT;      /* take to kitchen status */
		fsPrtCompul = 0;                  /* no printing of the receipt, only collect list of PLU tickets */
		fsPrtNoPrtMask =  0;              /* no printing of the receipt, only collect list of PLU tickets */
		fsPrtPrintPort = 0;               // no printing of the receipt, only collect list of PLU tickets

		uchPrtSeatNo = 0;                 /* continuous seat no print */
		uchPrtCurKP = (UCHAR)0x01;        /* for interface to "PrtChkKPItem, KP Printer #1*/

		PluChit.nPluChitInfoSize = 75;
		freeMalloced = ((PluChit.pPluChitInfo = calloc (PluChit.nPluChitInfoSize, sizeof(PluChitInfo))) != 0);
		if (! freeMalloced) {
			PluChit.pPluChitInfo = PluChitInfoList;
			PluChit.nPluChitInfoSize = sizeof(PluChitInfoList)/sizeof(PluChitInfoList[0]);
		}
		PluChit.iOrderDeclareAdr = 0;
		PrtCallIPDTicketInfo ( pTran, hsStorage, &PluChit );

		fsPrtCompul = PRT_RECEIPT;                  /* receipt compulsory */
		fsPrtNoPrtMask =  PRT_RECEIPT;              /* not print except receipt */ 
		PrtPortion(0);                              /* set print portion to         */
		PrtPluTicketList (pTran, 0, PluChit.pPluChitInfo, PluChit.nPluChitInfoSize, PluChit.iOrderDeclareAdr);
		if (freeMalloced) {
			free (PluChit.pPluChitInfo);
		}
	} else {
		USHORT  i;
		TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };

		fsPrtStatus = PRT_TAKETOKIT;                /* take to kitchen status */
		fsPrtCompul = PRT_RECEIPT;                  /* receipt compulsory */
		fsPrtNoPrtMask =  PRT_RECEIPT;              /* not print except receipt */ 

		PrtPortion(0);                              /* set print portion to         */
													/* static area "fsPrtPrintPort" */

		RflGetTranMnem(aszTranMnem, TRN_TENT);  /* get tent mnemo */

		for (i = 0; i < MAX_DEST_SIZE; i++) {
			uchPrtSeatNo = 0;                           /* continuous seat no print */
			fbPrtKPSeatState = 0xFF;                /* not feed first item at non seat# trans. */

			/* -- take to kitchen management -- */
			if (auchPrtFailKP[i]) {

				PrtTransactionBegin(PRT_KITCHENPRINTING,pTran->TranGCFQual.usGuestNo);

				uchPrtCurKP = (UCHAR)(0x01 << i);   /* for interface to "PrtChkKPItem */

				PrtTHHead(pTran);

				if (pTran->TranCurQual.fsCurStatus & CURQUAL_PVOID) {
					PrtPVoid(pTran, PRT_SINGLE_RECPT);  /* print preselect void line */
				}

				if (pTran->TranCurQual.fsCurStatus & (CURQUAL_PRETURN | CURQUAL_TRETURN)) {
					PrtPReturn(pTran, PRT_SINGLE_RECPT);   /* print returns transaction line */
				}

				PrtTHKitchen(auchPrtFailKP[i]);     /* print take to kitchen line */
				//prints the tent mnemonic first, then prints the user input on one line
				//PDINU
				if (pTran->TranModeQual.aszTent[0] > 0){
					PrtTHTentName( pTran->TranGCFQual.aszName );
				}
				else{
					PrtTHCustomerName( pTran->TranGCFQual.aszName );
				}

				/* --- print unique trans # --- */

				if ( pTran->TranCurQual.flPrintStatus & CURQUAL_UNIQUE_TRANS_NO ) {
					PrtTHTblPerGC( pTran->TranGCFQual.usTableNo, 0, 0, 0, 0, PRT_DOUBLE );
				}

				PrtCallIPD(pTran, hsStorage);

#if 0
				// Changes for Order Declare printing on the Take To Kitchen receipt
				// in Rel 2.2.2.017 on Dec-17-2018 caused a change in the printing logic
				// used for the Take To Kitchen receipt. We commented out the check using
				// function PrtChkKPItem() in function PrtCallIPDTicketInfo() which
				// filtered out any transaction item that was not a CLASS_ITEMSALES.
				//
				// Since this filtered out any of the transaction items that would
				// trigger a trailer and paper cut, those were done here.
				//
				// Now that we are no longer doing the filtering, we no longer need the
				// following lines of code as they are done for us. Keeping these lines
				// of code result in an unnwanted duplicate trailer and paper cut.
				//    Richard Chambers, Jan-03-2019
				PrtTrail2(pTran);                   /* print trailer */
				PrtTransactionEnd(PRT_KITCHENPRINTING,pTran->TranGCFQual.usGuestNo);
#endif
				PrtShrEnd();
			}
		}
	}

    PrtPmgWait();                               /* wait print buffer becomes empty */

    if (( PifSysConfig()->uchOperType == DISP_2X20 ) ||
        ( PifSysConfig()->uchOperType == DISP_LCD  )) {
        UieDescriptor(UIE_OPER, 3, UIE_DESC_OFF);/* take to k/p desc. OFF */
    }
}
/* === End of File ( PrRKpt.C ) ===== */
