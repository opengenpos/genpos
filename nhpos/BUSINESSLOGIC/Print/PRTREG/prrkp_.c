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
*===========================================================================
* Title       : Print  Kitchen printer
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRKP_.C
* --------------------------------------------------------------------------
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
* Apr-27-00 :          : M.Teraki   : MDC #258, bit2 swept away.
* May-08-00 :          : M.Teraki   : fbPrtKPTarget removed.
* Jul-31-00 : V1.0.0.4 : M.Teraki   : Changed KP frame number to be managed for each KP
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
#include "prtRj.h"
#include "prtrin.h"
#include "prrcolm_.h"

/**
;========================================================================
;+                    S T A T I C    R A M s
;========================================================================
**/
CONST TCHAR FARCONST  aszPrtKPGuest_CD[]= _T("%4d%02d");        /* guest check no and check digit */
CONST TCHAR FARCONST  aszPrtKPGuest[]   = _T("%4d");            /* guest check no */
CONST TCHAR FARCONST  aszPrtKPHeader[]  = _T("%-4s %-4s %-4s%s"); /* kitchen printer header */
CONST TCHAR FARCONST  aszPrtKPCustName[] = _T("%-19s");          /* customer name */
CONST TCHAR FARCONST  aszPrtKPTrailer[] = _T("%04d %-4s%8.8Mu %04lu-%03lu %s"); /* kitchen printer trailer */
CONST TCHAR FARCONST  aszPrtKPWeight[]  = _T("%.*l$%s");        /* weight */
CONST TCHAR FARCONST  aszPrtKPQuant[]   = _T("%ld X");          /* quantity */
CONST TCHAR FARCONST  aszPrtKPSeat[]    = _T("%-4s %1d");       /* seat no. */
CONST TCHAR FARCONST  aszPrtKPTotal[]   = _T("%s  %s");       /* total, V3.3 */
/* CONST UCHAR FARCONST  aszPrtKPTotal[]   = _T("%s    %s");       / total */
CONST TCHAR FARCONST  aszPrtKPOperator[] = _T("%-4s %-s");      /* operator */
CONST TCHAR FARCONST  aszPrtKPOperator2[] = _T("%-4s\n%-10s"); /* operator */

/**
;============================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;============================================================================
**/

/* -- for message (sent Kitchen printer) control -- */  
UCHAR   uchPrtKPFrameNo[KPS_NUMBER_OF_PRINTER];                    /* message frame counter (seq. #) */     /* V1.0.0.4 */
USHORT  usPrtKPOffset[KPS_NUMBER_OF_PRINTER];                      /* message area offset,   Saratoga */
TCHAR   auchPrtKPMessage[KPS_NUMBER_OF_PRINTER][KPS_FRAME_SIZE];   /* message area, Saratoga, should be same size as KPS_FRAME_SIZE */

UCHAR   fbPrtKPState;                                              /* save outputed kp number */
UCHAR   fbPrtKPSeatState;                                          /* save outputed kp number for seat no, R3.1 */
UCHAR   fbPrtKPAbortState;                                         /* save kp aborted status R3.1 */

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

    /* uchPrtKPFrameNo = 1; */         /* message frame counter(sequence no.) */ /* V1.0.0.4 */
    for (i = 0; i < MAX_DEST_SIZE; i++) {   /* Saratoga */
        uchPrtKPFrameNo[i] = 1;        /* message frame counter(sequence no.) */ /* V1.0.0.4 */
        auchPrtKPMessage[i][0] = KPS_START_FRAME;   /* set start status */

        auchPrtKPMessage[i][1] = uchPrtKPFrameNo[i];   /* set sequence number */ /* V1.0.0.4 */
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
    TCHAR   aszMnem[PARA_SPEMNEMO_LEN + 1];
    TCHAR   aszOpeMnem[NUM_OPERATOR + 1];
    USHORT  i;

    memset(aszMnem, '\0', sizeof(aszMnem));
    memset(aszOpeMnem, '\0', sizeof(aszOpeMnem));

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
        auchPrtKPMessage[i][(usPrtKPOffset[i]/sizeof(TCHAR)) + PARA_SPEMNEMO_LEN + 1 + _tcslen(aszOpeMnem)] = '\0';
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
*===========================================================================
*/
VOID    PrtKPHeader(TRANINFORMATION *pTran)
{
    TCHAR   aszTableMnem[PARA_SPEMNEMO_LEN + 1];/* PARA_... defined in "paraequ.h" */
    TCHAR   aszGuestMnem[PARA_SPEMNEMO_LEN + 1];  /* PARA_... defined in "paraequ.h" */
    TCHAR   aszTableNo[PRT_ID_LEN + 1];
    TCHAR   aszGuestNo[PRT_KPCOLUMN + 1];
    TCHAR   aszWork[PRT_KPCOLUMN + 1];
    USHORT  i;

    *aszTableMnem = '\0';
    *aszGuestMnem = '\0';
    *aszTableNo = '\0';
    *aszGuestNo = '\0';
    *aszWork = '\0';

    if (pTran->TranGCFQual.usTableNo != 0) {
        _itot(pTran->TranGCFQual.usTableNo, aszTableNo, 10);/* convert int to ascii */
		RflGetSpecMnem(aszTableMnem, SPC_TBL_ADR);        /* get table mnemo */
    }
    if (pTran->TranModeQual.fsModeStatus & MODEQUAL_CASINT) {
        ;   /* not print GCF No. (inner use only), R3.3 */
    } else
    if (pTran->TranGCFQual.usGuestNo != 0) {             /* GCF#0, doesnot print */
		RflGetSpecMnem(aszGuestMnem, SPC_GC_ADR);        /* get GC no. menm */

        /* -- GCF no. with check digit -- */
        if (pTran->TranGCFQual.uchCdv != 0) {
            RflSPrintf(aszGuestNo, TCHARSIZEOF(aszGuestNo), aszPrtKPGuest_CD,
                                   pTran->TranGCFQual.usGuestNo,
                                   pTran->TranGCFQual.uchCdv);
        } else {
            RflSPrintf(aszGuestNo, TCHARSIZEOF(aszGuestNo), aszPrtKPGuest,
                                   pTran->TranGCFQual.usGuestNo);
        }
    }

    if ((pTran->TranGCFQual.usTableNo != 0) ||(pTran->TranGCFQual.usGuestNo != 0)) {
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
        PrtFillSpace(&auchPrtKPMessage[i][(usPrtKPOffset[i]/sizeof(TCHAR))], NUM_NAME - 1);
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
    TCHAR  aszMnem[PARA_SPEMNEMO_LEN + 1];
    USHORT i;
    ULONG  ulID;
    TCHAR  aszDate[PRT_DATETIME_LEN + 1];

    /*  --  get cashier or waiter's name and mnemonics -- */
    if ( PRT_CASHIER == PrtChkCasWai(aszMnem, pTran->TranModeQual.ulCashierID, pTran->TranModeQual.ulWaiterID) ) {
        ulID    = RflTruncateEmployeeNumber(pTran->TranModeQual.ulCashierID);
    } else {
        ulID    = RflTruncateEmployeeNumber(pTran->TranModeQual.ulWaiterID);
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
    UCHAR   fchKP, fchMask = 0x01;
    USHORT  usTtlAddr;
    USHORT  usItemLen, i;
	TCHAR   szTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };
	TCHAR   szAmount[PARA_SPEMNEMO_LEN + PRT_AMOUNT_LEN + 1] = { 0 };
	TCHAR   szItem[(PRT_KPCOLUMN + 1) * 2] = { 0 };

    if ((( pItem->auchTotalStatus[ 0 ] / CHECK_TOTAL_TYPE ) == PRT_TRAY1 ) ||
        (( pItem->auchTotalStatus[ 0 ] / CHECK_TOTAL_TYPE ) == PRT_TRAY2 )) {

        /* --- tray total key is not print on kitchen printer --- */
        return;
    }

    /* -- get transaction mnemonics -- */
    usTtlAddr = RflChkTtlAdr( pItem );
	RflGetTranMnem( szTranMnem, usTtlAddr );

    /* -- adjust native mnemonic and amount sign(+.-) -- */
    PrtAdjustNative( szAmount, pItem->lMI );

    RflSPrintf(szItem, TCHARSIZEOF(szItem), aszPrtKPTotal, szTranMnem, szAmount);

    /* -- fill space with kp column number -- */
    usItemLen = _tcslen( szItem );
    szItem[ usItemLen++ ] = PRT_RETURN;

    fchKP  = pItem->auchTotalStatus[5] << 4;
    fchKP |= pItem->auchTotalStatus[3];

    for (i = 0; i < MAX_DEST_SIZE; i++, fchMask <<= 1) {
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
    TCHAR   aszItem[PRT_KPMAXITEM];
    USHORT  usItemOffset = 0;
    UCHAR   fbPrtStatus;                        /* print status for kp */
    UCHAR   fbPrtStatus2 = 0;                   /* print status for void */
    USHORT  usWork = 0, i;
    UCHAR   fchKP, fchMask = 0x01;
    UCHAR   fbStatWork;                          /* print status for kp */
    SHORT   sReturn, sReturn2;

    if (fbPrtKPAbortState & PRT_KPABORT) {
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

        if ( pTran->TranCurQual.fsCurStatus & CURQUAL_PVOID ) {
            fbPrtStatus2 &= ~PRT_KPVOID;         /* reset void status */
        }
    }

    memset(aszItem, '\0', sizeof(aszItem));     /* initialize */

    fchKP  = ((pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_6] & PLU_SALES_KP_NO_5_8) << 4);
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

    usItemOffset += PrtKPNoun(&aszItem[usItemOffset], pItem); /* noun mnem. */
    usItemOffset += PrtKPPM(&aszItem[usItemOffset], pItem);   /* PM & condiment */

    if (pItem->usLinkNo) {
        fbStatWork=0;
        sReturn = PrtChkKPPortSub(&fbStatWork, (UCHAR)((pItem->Condiment[0].ControlCode.auchPluStatus[2]) >> 4), pTran, PRT_MDC);
        sReturn2 = PrtChkKPPortSub2(&fbStatWork, (UCHAR)(pItem->Condiment[0].ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_6] << 4), pTran, PRT_MDC);

        if ((sReturn == PRT_KPITEM) || (sReturn2 == PRT_KPITEM)) {
            usItemOffset += PrtKPChild(&aszItem[usItemOffset], pItem, 0);
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
    TCHAR   aszItem[PRT_KPMAXITEM];
    USHORT  usItemOffset;
    UCHAR   fbPrtPort=0;                         /* print status for kp */
    UCHAR   fbStatWork;                          /* print status for kp */
    UCHAR   fbCurPrtState;                       /* current print status*/
    UCHAR   fbCurPrtState2 = 0;                  /* current print status for void */
    USHORT  usWork = 0;
    USHORT  i,j;
    TCHAR   auchDummy[NUM_PLU_LEN] = {0};
    UCHAR   fchKP, fchMask = 0x01;

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

    /* fchKP  = (pItem->ControlCode.auchPluStatus[6]); */
    fchKP  = ((pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_6] & PLU_SALES_KP_NO_5_8) << 4);
    fchKP |= ((pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2] & PLU_SALES_KP_NO_1_4) >> 4);

    /* check setmenu children plu's kp portion */
    for ( j = 0; j < pItem->uchChildNo; j++ ) {
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

    for ( i = 0; i < KPS_NUMBER_OF_PRINTER;  i++ ) {
        fbCurPrtState = (UCHAR)(1<<i);

        /* check setmenu children plu's kp portion */
        if ( !(fbPrtPort & fbCurPrtState) ) {
            continue;
        }

        fbPrtKPState |= fbCurPrtState;               /* store output kp  */

        /* -- preselect void? -- */
        if ( pTran->TranCurQual.fsCurStatus & CURQUAL_PVOID ) {
            fbCurPrtState2 |= PRT_KPVOID;              /* set void status */
        }

        /* -- void? -- */
        if ( pItem->fbModifier & VOID_MODIFIER ) {
            fbCurPrtState2 |= PRT_KPVOID;              /* set void status */

            if ( pTran->TranCurQual.fsCurStatus & CURQUAL_PVOID ) {
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
            /* usItemOffset += PrtKPWeight(&aszItem[usItemOffset], sizeof(aszItem),  pTran, pItem); */
        } else {
            if ((pTran->TranCurQual.uchKitchenStorage == PRT_KIT_NET_STORAGE) &&
                (!(pTran->TranCurQual.auchTotalStatus[4] & CURQUAL_TOTAL_NOCONSOLIDATE_PRINT))) { /* consolidation print */

                usItemOffset += PrtKPQty(&aszItem[usItemOffset], PRT_KPCOLUMN+1, (LONG)pItem->sCouponQTY);
                /* usItemOffset += PrtKPQty(&aszItem[usItemOffset], sizeof(aszItem), (LONG)pItem->sCouponQTY); */
            } else {
                usItemOffset += PrtKPQty(&aszItem[usItemOffset], PRT_KPCOLUMN+1, pItem->lQTY / PLU_BASE_UNIT);
                /* usItemOffset += PrtKPQty(&aszItem[usItemOffset], sizeof(aszItem), pItem->lQTY/1000L); */
            }
        }

        usItemOffset += PrtKPNoun(&aszItem[usItemOffset], pItem); /* noun mnem. */
        usItemOffset += PrtKPPM(&aszItem[usItemOffset], pItem);   /* PM & condiment */

        for ( j = 0; j < pItem->uchChildNo; j++ ) {

            /* check child PLU exist or not */
            if (_tcsncmp(pItem->Condiment[j].auchPLUNo, auchDummy, NUM_PLU_LEN) == 0) {
                continue;
            }

            if ( (pTran->TranCurQual.flPrintStatus) & CURQUAL_SETMENU_KITCHEN ) {
                break;
            }

            fbStatWork=0;
            PrtChkKPPortSub(&fbStatWork, (UCHAR)((pItem->Condiment[j].ControlCode.auchPluStatus[2]) >> 4), pTran, PRT_MDC);
            PrtChkKPPortSub2(&fbStatWork, (UCHAR)(pItem->Condiment[j].ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_6] << 4), pTran, PRT_MDC);

            if (fbStatWork & fbCurPrtState) {
                usItemOffset += PrtKPChild(&aszItem[usItemOffset], pItem, j);
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
		TCHAR   aszAdjMnem[PARA_ADJMNEMO_LEN + 1] = { 0 };

        /* --get adjective mnemocnics -- */
		RflGetAdj(aszAdjMnem, pItem->Condiment[usCondNo].uchAdjective);

        usMnemLen = _tcslen(aszAdjMnem);

        _tcsncpy(pszStart, aszAdjMnem, usMnemLen);
        //memcpy(pszStart, aszAdjMnem, usMnemLen);
        pszStart += usMnemLen;
        i += usMnemLen;

        *pszStart = PRT_SPACE;
        pszStart++;
        i++;

    }

    usMnemLen = _tcslen(pItem->Condiment[usCondNo].aszMnemonic);

    /* saratoga */
    if ((i + usMnemLen) > PRT_KPCOLUMN) {   /* overs kp column */
        usOverLen = (i + usMnemLen) - PRT_KPCOLUMN;
        usMnemLen = (PRT_KPCOLUMN-i);
    }
    
    _tcsncpy(pszStart, pItem->Condiment[usCondNo].aszMnemonic, usMnemLen);
    //memcpy(pszStart, pItem->Condiment[usCondNo].aszMnemonic, usMnemLen);
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
    //memcpy(pszStart, pItem->Condiment[usCondNo].aszMnemonic + usMnemLen, usOverLen);
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
    TCHAR  aszWeightSym[PARA_SPEMNEMO_LEN + 1];  /* PARA_... defined in "paraequ.h" */
    SHORT  sDecPoint;                            /* decimal point */
    LONG   lModQty;                              /* modified qty */
    USHORT usStrLen;

    /* -- get scalable symbol and adjust qty -- */
    PrtGetScale(aszWeightSym, &sDecPoint, &lModQty, pTran, pItem);

    /* -- format string -- */
    RflSPrintf(pszDest, usDestLen, aszPrtKPWeight, sDecPoint, lModQty, aszWeightSym);

    usStrLen = _tcslen(pszDest);

    pszDest[usStrLen] = PRT_RETURN;   /* write return */

    return (usStrLen + 1);            /* return written length, 1 for return character */
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

    usStrLen = _tcslen(pszDest);

    pszDest[usStrLen] = PRT_RETURN;   /* write return */

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
		TCHAR   aszSpecMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };   /* PARA_... defined in "paraequ.h" */

		RflGetSpecMnem( aszSpecMnem, SPC_SEAT_ADR );    /* seat no. mnemonic */
        RflSPrintf(pszDest, usDestLen, aszPrtKPSeat, aszSpecMnem, pItem->uchSeatNo);
        usStrLen = _tcslen(pszDest);
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
    DCURRENCY  lProduct;
    USHORT     usStrLen;

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

    PrtSetItem(pszDest, pItem, &lProduct, PRT_ONLY_NOUN, 0);

    usStrLen = _tcslen(pszDest);

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
    TCHAR      *pszStart;
    TCHAR      *pszEnd;
    DCURRENCY  lProduct;
    TCHAR      aszWork[(PRT_KPCOLUMN+1)*11];
/*    UCHAR  aszWork[PRT_MAXITEM + PRT_KPCOLUMN]; */
    USHORT     usWrite = 0;
    USHORT     usWork;
    USHORT     usColumn;

    pszStart = aszWork;

    memset(aszWork, '\0', sizeof(aszWork));

    /* separated print option of condiment, V3.3 */
    if ( CliParaMDCCheck( MDC_KP2_ADR, EVEN_MDC_BIT1 )) {
        usColumn = PRT_KPCOLUMN-1;
    } else {
        usColumn = 0;
    }

    PrtSetItem(aszWork, pItem, &lProduct, PRT_ONLY_PMCOND, usColumn); /* set item's mnemonics to buffer */

    if (*aszWork == '\0') {

        return(0);
    }

    pszDest[0] = KPS_RED_CODE;                      /* set RED */
    usWrite++;                                      /* inc. write length */

    for (;;) {
        /*---- correct condiment print position 07/11/96 ----*/
        pszDest[usWrite] = PRT_SPACE;
        usWrite++;

        /* -- get 1 line -- */
        if ( PRT_LAST == PrtGet1Line(pszStart, &pszEnd, PRT_KPCOLUMN-1) ) {
            usWork = _tcslen(pszStart);

            if (usWork != 0) {
                _tcsncpy(&pszDest[usWrite], pszStart, usWork);
                //memcpy(&pszDest[usWrite], pszStart, usWork);
                usWrite += usWork;
                pszDest[usWrite] = PRT_RETURN;           /* write return */
                usWrite ++;                              /* for "return"'s length */
            }
            break;
        }

        usWork = pszEnd - pszStart;
        _tcsncpy(&pszDest[usWrite], pszStart, usWork);
        //memcpy(&pszDest[usWrite], pszStart, usWork);
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
    USHORT          usKPState;
    USHORT          usRet;
    SHORT           sReturn;
    PARASHAREDPRT   SharedPrt;
    TCHAR           szWork[PRT_KPMAXITEM];  /* ### ADD 2172 Rel1.0 */
    USHORT          usPrevious;

    memset(szWork,0,PRT_KPMAXITEM);         /* ### MOD 2172 Rel1.0 */
    _tcsncpy(&szWork[3],pszBuff,usBuffLen);

    usBuffLen += 3;                       /* increment for "data length" and "status" */

    szWork[0] = usBuffLen * sizeof(TCHAR);               /* write length */
    szWork[1] = fbPrtState;               /* write status for printer */
    szWork[2] = fbState;                  /* void status R3.1 */

    memset(&SharedPrt, 0, sizeof(SharedPrt));
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

        /* allow power switch at error display */
        usPrevious = PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */

        /* ### MOD 2172 Rel1.0 (Saratoga) CliKpsPrintEx -> PrtSendKps */
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
    //usPrtKPOffset is in bytes divide by sizeof TCHAR to get index
	//use memcpy because the buffer can contain null positions for data values that are
	//also included in the buffer with the text
    memcpy(&auchPrtKPMessage[usTerm][usPrtKPOffset[usTerm]/sizeof(TCHAR)], szWork, usBuffLen * sizeof(TCHAR));
    usPrtKPOffset[usTerm] += (usBuffLen) * sizeof(TCHAR);            /* increment storage write offset */

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
    UCHAR           fchMask;
    USHORT          i;
    USHORT          usKPStateWork;       /* work area to update usKPState */
    USHORT          usKPState = 0x0000;  /* status save area for all KP# */
    USHORT          usRet;
    SHORT           sReturn;
    PARASHAREDPRT   SharedPrt;
    USHORT          usPrevious;

    if (fbPrtKPState == 0) {             /* "fbPrtKPState" is storeage area of output kp number */
        return ;                         /* "== 0" means that there is no item to send. */
    }

    memset(&SharedPrt, 0, sizeof(SharedPrt));
    SharedPrt.uchMajorClass = CLASS_PARASHRPRT;

    for (i = 0, fchMask = 0x01; i < MAX_DEST_SIZE; i++, fchMask <<= 1) {

        SharedPrt.uchAddress = (UCHAR)(SHR_KP1_DEF_ADR + i);
        CliParaRead(&SharedPrt);

        if (SharedPrt.uchTermNo == 0) {
            continue;
        }

        if ((UCHAR)fbPrtKPState & fchMask) {
            auchPrtKPMessage[i][0] |= KPS_END_FRAME;/* write end status */

            /* allow power switch at error display */
            usPrevious = PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */

            while (1) {

                /* do print */
                /* ### MOD 2172 Rel1.0 (Saratoga) CliKpsPrintEx -> PrtSendKps */
                sReturn = PrtSendKps(&auchPrtKPMessage[i][0], usPrtKPOffset[i], CliReadUAddr(),
                                     &usKPStateWork, SharedPrt.uchTermNo, (SHORT)(i+1));

                /* update status */
                usKPState |= usKPStateWork;

                if (sReturn == KPS_PERFORM) {
                    break;
                }
                if (sReturn == KPS_PRINTER_DOWN) {
                    /* if already known the printer is dead, error message is not needed. */
                    break;
                }

                if (KPS_BUFFER_FULL == sReturn) {
                    fbPrtKPAbortState |= PRT_KPABORT;
                    PrtSetFailKP(0);                    /* "0" means  "all kp error" */
                    PifControlPowerSwitch(usPrevious);  /* V1.0.04 */
                    return;
                }

                /* allow power switch at error display */
                PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */
                usRet = UieErrorMsg(KpsConvertError(sReturn), UIE_ABORT);

                /* if abort key entered, set receipt compul and exit */
                if (usRet == FSC_CANCEL || usRet == FSC_P5) {
                    fbPrtKPAbortState |= PRT_KPABORT;
                    PrtSetFailKP(0);                    /* "0" means  "all kp error" */
                    PifControlPowerSwitch(usPrevious);  /* V1.0.04 */
                    return;
                }
            }
            PifControlPowerSwitch(usPrevious);  /* V1.0.04 */
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
*===========================================================================
*/
SHORT  PrtSetFailKP(USHORT usKPState)
{
    usKPState = ~usKPState;           /* reverse bit */
    usKPState &= fbPrtKPState;        /* print portion check */
/*    usKPState &= PRT_KPMASK;          / mask */


    if (usKPState) {

        if (usKPState & 0x01) {
            auchPrtFailKP[0] = 1;
        }

        if (usKPState & 0x02) {
            auchPrtFailKP[1] = 2;
        }

        if (usKPState & 0x04) {
            auchPrtFailKP[2] = 3;
        }

        if (usKPState & 0x08) {
            auchPrtFailKP[3] = 4;
        }

        if (usKPState & 0x10) {     /* R3.1 */
            auchPrtFailKP[4] = 5;
        }

        if (usKPState & 0x20) {     /* R3.1 */
            auchPrtFailKP[5] = 6;
        }

        if (usKPState & 0x40) {     /* R3.1 */
            auchPrtFailKP[6] = 7;
        }

        if (usKPState & 0x80) {     /* R3.1 */
            auchPrtFailKP[7] = 8;
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

    fbWork1    = (UCHAR)~(MDC.uchMDCData);
    *puchState = *puchState & fbWork1;      /* clear each bit associated to KDS */
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
    SHORT sReturn, sReturn2;

    /* normal item without condiment or PM,  does not print */
    if (  (pItem->fsPrintStatus & PRT_SPCL_PRINT)
        &&(pItem->uchCondNo+pItem->uchPrintModNo == 0) ) {

/* 1998/10/16 Yamamoto Change start */
/*
        return(PRT_NOTKPITEM);
*/
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

    sReturn = PrtChkKPPortSub(puchPort, (UCHAR)( pItem->fsPrintStatus >> 4), pTran, fbType);

    /* check kitchen printer #5 - 8    R3.1 */
    sReturn2 = PrtChkKPPortSub2(puchPort, (UCHAR)( pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_6] << 4), pTran, fbType);

    if ((sReturn == PRT_NOTKPITEM) && (sReturn2 == PRT_NOTKPITEM)) {
        return (PRT_NOTKPITEM);

    } else {
        return (PRT_KPITEM);
    }
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
*            #5 - #8. R3.1
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
    UCHAR    fbStatWork;
    USHORT   i;
    UCHAR   uchMaxChild;
    TCHAR    auchDummy[NUM_PLU_LEN] = {0};

    switch ( pItem->uchMajorClass ) {
    case CLASS_ITEMTOTAL:       /* total item */
        if (( ! CliParaMDCCheck( MDC_KPTR_ADR, ODD_MDC_BIT0 )) &&
            ( CliParaMDCCheck( MDC_KPTR_ADR, ODD_MDC_BIT3 ))) {
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

        if (( pItem->fsPrintStatus & PRT_SPCL_PRINT ) && ( pItem->uchPrintModNo == 0 )) {
            return (PRT_NOTKPITEM);
        }

        /* check setmenu children's kp print portion */
        uchMaxChild = pItem->uchChildNo + pItem->uchCondNo + pItem->uchPrintModNo;

        for ( i = 0; i < uchMaxChild; i++ ) {

            /* check condiment PLU exist or not */
            if (_tcsncmp(pItem->Condiment[i].auchPLUNo, auchDummy, NUM_PLU_LEN) != 0) {
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
    USHORT  i;

    /* -- not print "take to kitchen"(depend on MDC) -- */
    if (!(pTran->TranCurQual.flPrintStatus & CURQUAL_TAKEKITCHEN)) {

        UieErrorMsg(LDT_KTNERR_ADR, UIE_ABORT);
        return ;
    }

    fsPrtStatus = PRT_TAKETOKIT;                /* take to kitchen status */

    fsPrtCompul = PRT_RECEIPT;                  /* receipt compulsory */

    fsPrtNoPrtMask =  PRT_RECEIPT;              /* not print except receipt */ 

    PrtPortion(0);                              /* set print portion to         */
                                                /* static area "fsPrtPrintPort" */

    if (( PifSysConfig()->uchOperType == DISP_2X20 ) ||
        ( PifSysConfig()->uchOperType == DISP_LCD  )) {
        UieDescriptor(UIE_OPER, 3, UIE_DESC_ON);/* take to k/p desc. ON */
    }

    for (i = 0; i < KPS_NUMBER_OF_PRINTER; i++) {

        uchPrtSeatNo = 0;                           /* continuous seat no print */
        fbPrtKPSeatState = 0xFF;                /* not feed first item at non seat# trans. */

        /* -- take to kitchen management -- */
        if (auchPrtFailKP[i]) {

            uchPrtCurKP = (UCHAR)(0x01 << i);   /* for interface to "PrtChkKPItem */

            if (pTran->TranCurQual.fsCurStatus & CURQUAL_PVOID) {
                                                /* print preselect void line */
                PrtPVoid_RJ(pTran);
            }

            PrtRJKitchen(auchPrtFailKP[i]);     /* print take to kitchen line */

            PrtRJCustomerName( pTran->TranGCFQual.aszName );

            /* --- print unique trans # --- */

            if ( pTran->TranCurQual.flPrintStatus & CURQUAL_UNIQUE_TRANS_NO ) {
                PrtRJTblPerson( pTran->TranGCFQual.usTableNo, 0, 0, PRT_DOUBLE );
            }

            PrtCallIPD(pTran, hsStorage);

            PrtTrail2(pTran);                   /* print trailer */
        }
    }

    PrtPmgWait();                               /* wait print buffer becomes empty */

    if (( PifSysConfig()->uchOperType == DISP_2X20 ) ||
        ( PifSysConfig()->uchOperType == DISP_LCD  )) {
        UieDescriptor(UIE_OPER, 3, UIE_DESC_OFF);/* take to k/p desc. OFF */
    }
}

/****** End of Source ******/
