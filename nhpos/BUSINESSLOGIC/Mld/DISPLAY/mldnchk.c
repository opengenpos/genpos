/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS, E&M OISO                **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Multiline Display  Newcheck
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDNCHK.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*   MldNewChk()     :   display new check operation
*
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
* Mar-14-95 : 03.00.00 : M.Ozawa    : Initial
* Aug-16-99 : 03.05.00 : M.Teraki   : Merge STORE_RECALL_MODEL
*           :          :            :       /GUEST_CHECK_MODEL
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
/**------- MS - C ---------**/
#include	<tchar.h>
#include <string.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <pif.h>
#include <paraequ.h>
#include <para.h>
#include <pmg.h>
#include <regstrct.h>
#include <transact.h>
#include <log.h>
#include <vos.h>
#include <mld.h>
#include "mldlocal.h"
#include "mldrin.h"
#include "termcfg.h"
#include "rfl.h"

extern CONST TCHAR FARCONST  aszMldSignIn[];


CONST TCHAR   aszMldGuest_WTCD[] = _T("%-6s%d%02d");     /* guest no. with check digit */

CONST TCHAR   aszMldGuest_WOCD[] = _T("%-8.8s\t%d");     /* guest no. without check digit */
CONST TCHAR   aszMldTable[] = _T("%-8.8s\t%4d");

CONST TCHAR   aszMldGuest_WTCD2[] = _T(" %-s\t%d%02d");  /* guest no. with check digit */

CONST TCHAR   aszMldGuest_WOCD2[] = _T(" %-s\t%d");      /* guest no. without check digit */

CONST TCHAR   aszMldTent[] = _T("%-11.11s\t");	         //length of Receipt ID  ***PDINU

/*
*===========================================================================
** Format  : VOID   MldNewChk( ITEMTRANSOPEN *pItem, USHORT usType,
*                               USHORT usScroll);
*
*   Input  : ITEMTRANSOPEN   *pItem     -   Transaction open data
*            USHORT          usType     -   First display position
*            USHORT          usScroll   -   Destination scroll display
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays new check operation
*===========================================================================
*/
SHORT MldNewChk( ITEMTRANSOPEN *pItem , USHORT usType, USHORT usScroll)
{
    if ((uchMldSystem == MLD_PRECHECK_SYSTEM) || (uchMldSystem == MLD_DRIVE_THROUGH_1SCROLL)) {
        MldPrechkNewChk(pItem, usType, usScroll);
    }

    if ((uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) && (uchMldSystem2 == MLD_SPLIT_CHECK_SYSTEM)) {
        if (usScroll & MLD_SCROLL_1) {
            MldSplitNewChk(pItem, usType, usScroll);
        }

        if (usScroll & (MLD_SCROLL_2 | MLD_SCROLL_3)) {
            if (usType == MLD_TRANS_DISPLAY) {
				MLDSEAT Seat = { 0 };

                Seat.auchSeatNum[0] = pItem->uchSeatNo;
                MldDispSeatNum(usScroll, &Seat);
            }
        }
    }

    return (MldDummyScrollDisplay(usType, usScroll));
}

/*
*===========================================================================
** Format  : VOID   MldNewChk( ITEMTRANSOPEN *pItem, USHORT usType,
*                               USHORT usScroll);
*
*   Input  : ITEMTRANSOPEN   *pItem     -   Transaction open data
*            USHORT          usType     -   First display position
*            USHORT          usScroll   -   Destination scroll display
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays new check operation
*===========================================================================
*/
SHORT MldPrechkNewChk( ITEMTRANSOPEN *pItem , USHORT usType, USHORT usScroll)
{

    /* display guest no on display#2 */
    switch (usType) {
    case MLD_NEW_ITEMIZE:
        MldWriteGuestNo(pItem);
    };

    return (MLD_SUCCESS);

    usScroll = usScroll;
}

/*
*===========================================================================
** Format  : VOID   MldSplitNewChk( ITEMTRANOPEN *pItem, USHORT usType,
*                               USHORT usScroll);
*
*   Input  : ITEMTRANSOPEN   *pItem     -   Transaction open data
*            USHORT          usType     -   First display position
*            USHORT          usScroll   -   Destination scroll display
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays split check system transaction open operation
*===========================================================================
*/
SHORT MldSplitNewChk( ITEMTRANSOPEN *pItem , USHORT usType, USHORT usScroll)
{
    if ((usScroll == MLD_SCROLL_1) && (usType == MLD_TRANS_DISPLAY)) {
		TCHAR  szMnemonic[PARA_SPEMNEMO_LEN + 1] = { 0 };
		UCHAR   uchAttr;

		RflGetSpecMnem(szMnemonic, SPC_GC_ADR);

        uchAttr = (UCHAR)(MldOtherHandle1.usOrderAttrib ? MLD_C_WHITE : MLD_C_BLACK);

        if (pItem->uchCdv) {
            VosPrintfAttr(MldOtherHandle1.usOrderHandle, uchAttr, aszMldGuest_WTCD2, szMnemonic, pItem->usGuestNo, pItem->uchCdv);
        } else {
            VosPrintfAttr(MldOtherHandle1.usOrderHandle, uchAttr, aszMldGuest_WOCD2, szMnemonic, pItem->usGuestNo);
        }
    }

    return (MldDummyScrollDisplay(usType, usScroll));
}

/*
*===========================================================================
** Format  : VOID   MldNewChk( ITEMTRANSOPEN *pItem, USHORT usType,
*                               USHORT usScroll);
*
*   Input  : ITEMTRANSOPEN   *pItem     -   Transaction open data
*            USHORT          usType     -   First display position
*            USHORT          usScroll   -   Destination scroll display
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays guest check no on scroll display #2
*===========================================================================
*/
VOID MldWriteGuestNo( ITEMTRANSOPEN *pItem)
{
    TRANMODEQUAL * const pTranModeQual = TrnGetModeQualPtr();
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };/* PARA_... defined in "paraequ.h" */
	TCHAR  aszSpecMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };  /* PARA_... defined in "paraequ.h" */
    USHORT  usTop=1;


    if (pTranModeQual->ulCashierID) {
        MldPrintf(MLD_SCROLL_2, MLD_SCROLL_TOP, aszMldSignIn, pTranModeQual->aszCashierMnem, RflTruncateEmployeeNumber(pTranModeQual->ulCashierID));
        usTop = 0;
    }

    if (pTranModeQual->ulWaiterID) {
        MldPrintf(MLD_SCROLL_2, (SHORT)(usTop ? MLD_SCROLL_TOP : MLD_SCROLL_APPEND), aszMldSignIn, pTranModeQual->aszWaiterMnem, RflTruncateEmployeeNumber(pTranModeQual->ulWaiterID));
        usTop = 0;
    }

    /* -- guest check mnem., number, check digit -- */
    if (pTranModeQual->fsModeStatus & MODEQUAL_CASINT) {
        ;   /* not display gcf no. at cashier interrupt, R3.3 */
    } else if (pItem->uchMinorClass == CLASS_STORE_RECALL) {
        /* if delivery recall display training status */
        if (pTranModeQual->fbCashierStatus & MODEQUAL_CASHIER_TRAINING) {
			RflGetTranMnem(aszTranMnem, TRN_TRNGGT_ADR);
        } else {
			RflGetTranMnem(aszTranMnem, TRN_GCNO_ADR);
        }

        MldPrintf(MLD_SCROLL_2, (SHORT)(usTop ? MLD_SCROLL_TOP : MLD_SCROLL_APPEND), aszMldGuest_WOCD, aszTranMnem, pItem->usGuestNo);

        usTop = 0;
    } else {
        if (TranGCFQualPtr->usGuestNo) {
            /* if delivery recall display training status */
            if ((TranGCFQualPtr->fsGCFStatus & GCFQUAL_TRAINING) && (uchMldSystem == MLD_DRIVE_THROUGH_1SCROLL)) {
				RflGetTranMnem(aszTranMnem, TRN_TRNGGT_ADR);
            } else {
				RflGetTranMnem(aszTranMnem, TRN_GCNO_ADR);
            }

            if (TranGCFQualPtr->uchCdv) {
                MldPrintf(MLD_SCROLL_2, (SHORT)(usTop ? MLD_SCROLL_TOP : MLD_SCROLL_APPEND), aszMldGuest_WTCD, aszTranMnem, TranGCFQualPtr->usGuestNo, TranGCFQualPtr->uchCdv);
            } else {
                MldPrintf(MLD_SCROLL_2, (SHORT)(usTop ? MLD_SCROLL_TOP : MLD_SCROLL_APPEND), aszMldGuest_WOCD, aszTranMnem, TranGCFQualPtr->usGuestNo);
            }
            usTop=0;
        }
    }

    /* -- table no., no. of person -- */
	RflGetSpecMnem(aszSpecMnem, SPC_TBL_ADR);  /* get table mnemo */
    if (pItem->usTableNo) {
        MldPrintf(MLD_SCROLL_2, (SHORT)(usTop ? MLD_SCROLL_TOP : MLD_SCROLL_APPEND), aszMldTable, aszSpecMnem, pItem->usTableNo);
        usTop=0;
    } else if (TranGCFQualPtr->usTableNo) {
        MldPrintf(MLD_SCROLL_2, (SHORT)(usTop ? MLD_SCROLL_TOP : MLD_SCROLL_APPEND), aszMldTable, aszSpecMnem, TranGCFQualPtr->usTableNo);
        usTop=0;
    }

	RflGetTranMnem(aszTranMnem, TRN_PSN_ADR);  /* get no of person mnemo */
    if (pItem->usNoPerson) {
        MldPrintf(MLD_SCROLL_2, (SHORT)(usTop ? MLD_SCROLL_TOP : MLD_SCROLL_APPEND), aszMldGuest_WOCD, aszTranMnem, pItem->usNoPerson);
        usTop=0;
    } else if (TranGCFQualPtr->usNoPerson) {
        MldPrintf(MLD_SCROLL_2, (SHORT)(usTop ? MLD_SCROLL_TOP : MLD_SCROLL_APPEND), aszMldGuest_WOCD, aszTranMnem, TranGCFQualPtr->usNoPerson);
        usTop=0;
    /* --- Saratoga V1.0.5, Dec/6/2000 --- */
    } else if (pItem->usNoPerson == 0) {
        MldPrintf(MLD_SCROLL_2, (SHORT)(usTop ? MLD_SCROLL_TOP : MLD_SCROLL_APPEND), aszMldGuest_WOCD, aszTranMnem, pItem->usNoPerson);
        usTop=0;
    }

    /* -- seat no. -- */
	RflGetSpecMnem(aszSpecMnem, SPC_SEAT_ADR);  /* get seat no mnemo */
    if (pItem->uchMinorClass == 0) {
        if (pItem->uchSeatNo) {
            MldPrintf(MLD_SCROLL_2, (SHORT)(usTop ? MLD_SCROLL_TOP : MLD_SCROLL_APPEND), aszMldTable, aszSpecMnem, pItem->uchSeatNo);
            usTop=0;
        } else if (TranCurQualPtr->uchSeat) {
            MldPrintf(MLD_SCROLL_2, (SHORT)(usTop ? MLD_SCROLL_TOP : MLD_SCROLL_APPEND), aszMldTable, aszSpecMnem, TranCurQualPtr->uchSeat);
            usTop=0;
        }
    } else {
        if (pItem->uchSeatNo) {
            MldPrintf(MLD_SCROLL_2, (SHORT)(usTop ? MLD_SCROLL_TOP : MLD_SCROLL_APPEND), aszMldTable, aszSpecMnem, pItem->uchSeatNo);
            usTop=0;
        }
    }
	RflGetTranMnem(aszTranMnem, TRN_TENT);  /* get tent mnemo */
	 
	//If there is a rediept ID input, the following code sends the display
	//to the Mld.  ***PDINU
    if (pTranModeQual->aszTent[0] > 0) {	 //if there is user input
        MldPrintf(MLD_SCROLL_2, (SHORT)(usTop ? MLD_SCROLL_TOP : MLD_SCROLL_APPEND), aszMldTent, aszTranMnem);
		MldPrintf(MLD_SCROLL_2, (SHORT)(usTop ? MLD_SCROLL_TOP : MLD_SCROLL_APPEND), aszMldTent, pTranModeQual->aszTent);
        usTop=0;
	}
}

/* ===== End of File ( MLDNCHK.C ) ===== */
