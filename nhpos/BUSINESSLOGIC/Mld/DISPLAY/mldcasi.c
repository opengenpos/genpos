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
* Title       : Multiline Display  Reorder
* Category    : Multiline, NCR 2170 US Hospitarity Application
* Program Name: MLDCASI.C
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*   MldItemCasIn()    : display modifier key operation
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
*/

/**------- MS - C ---------**/
#include	<tchar.h>
#include <string.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <paraequ.h>
#include <para.h>
#include <pmg.h>
#include <regstrct.h>
#include <transact.h>
#include <vos.h>
#include <mld.h>
#include <rfl.h>
#include "mldlocal.h"
#include "mldrin.h"
#include "termcfg.h"

extern TCHAR   aszMldOrderNo[];

/*
*===========================================================================
** Format  : VOID   MldCasIn( ITEMTRANOPEN *pItem, USHORT usType,
*                               USHORT usScroll);
*
*   Input  : ITEMTRANSOPEN   *pItem     -   Transaction open data
*            USHORT          usType     -   First display position
*            USHORT          usScroll   -   Destination scroll display
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays cashier system transaction open operation
*===========================================================================
*/
SHORT MldCasIn( ITEMTRANSOPEN *pItem , USHORT usType, USHORT usScroll)
{
    if (uchMldSystem == MLD_PRECHECK_SYSTEM) {
        MldPrechkCasIn (pItem, usType, usScroll);
    }
	else if ((uchMldSystem == MLD_DRIVE_THROUGH_1SCROLL) &&
               (uchMldSystem2 != MLD_SPLIT_CHECK_SYSTEM)) {    /* not 3 scroll */
        MldPrechkCasIn (pItem, usType, usScroll);
    }

    /* for charge tips display, R3.3 */
    MldSetSpecialStatus (usScroll, MLD_NEWCHEK_CASHIER);

    return (MldDummyScrollDisplay (usType, usScroll));
}

/*
*===========================================================================
** Format  : VOID   MldItemCasIn( ITEMTRANOPEN *pItem, USHORT usType,
*                               USHORT usScroll);
*
*   Input  : ITEMTRANSOPEN   *pItem     -   Transaction open data
*            USHORT          usType     -   First display position
*            USHORT          usScroll   -   Destination scroll display
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays cashier system transaction open operation
*===========================================================================
*/
SHORT MldPrechkCasIn( ITEMTRANSOPEN *pItem , USHORT usType, USHORT usScroll)
{
	USHORT  usType2 = MLD_TYPE_GET_FLAG(usType);

    switch (usType2) {
		case MLD_NEW_ITEMIZE:
			MldWriteGuestNo(pItem);
			break;
    }
    return (MLD_SUCCESS);

    usScroll = usScroll;
}

/*
*===========================================================================
** Format  : VOID   MldCasIn( ITEMTRANOPEN *pItem, USHORT usType,
*                               USHORT usScroll);
*
*   Input  : ITEMTRANSOPEN   *pItem     -   Transaction open data
*            USHORT          usType     -   First display position
*            USHORT          usScroll   -   Destination scroll display
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays cashier system transaction open operation
*===========================================================================
*/
SHORT MldStoreRecall( ITEMTRANSOPEN *pItem , USHORT usType, USHORT usScroll)
{
    if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {
        return (MldDriveStoreRecall(pItem, usType, usScroll));
    } else if (uchMldSystem == MLD_DRIVE_THROUGH_1SCROLL) {
        /* use precheck scorll #2 display */
        MldPrechkNewChk(pItem, usType, usScroll);
    }

    return (MldDummyScrollDisplay(usType, usScroll));
}

/*
*===========================================================================
** Format  : VOID   MldItemCasIn( ITEMTRANOPEN *pItem, USHORT usType,
*                               USHORT usScroll);
*
*   Input  : ITEMTRANSOPEN   *pItem     -   Transaction open data
*            USHORT          usType     -   First display position
*            USHORT          usScroll   -   Destination scroll display
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays cashier system transaction open operation
*===========================================================================
*/
SHORT MldDriveStoreRecall( ITEMTRANSOPEN *pItem , USHORT usType, USHORT usScroll)
{
	USHORT  usType2 = MLD_TYPE_GET_FLAG(usType);

    if ((usType2 == MLD_NEW_ITEMIZE) || (uchMldSystem2 == MLD_SPLIT_CHECK_SYSTEM)) {    /* only first itemize or at split check */
		UCHAR   uchAttr;
		TCHAR  szMnemonic[PARA_TRANSMNEMO_LEN + 1] = { 0 };

        if (TranModeQualPtr->fbCashierStatus & MODEQUAL_CASHIER_TRAINING) {
			RflGetTranMnem(szMnemonic, TRN_TRNGGT_ADR);
        } else {
			RflGetTranMnem(szMnemonic, TRN_GCNO_ADR);
        }

        uchAttr = (UCHAR)(MldOtherHandle1.usOrderAttrib ? MLD_C_WHITE : MLD_C_BLACK);

        VosPrintfAttr(MldOtherHandle1.usOrderHandle, uchAttr, aszMldOrderNo, szMnemonic, pItem->usGuestNo);
    }

    return (MldDummyScrollDisplay(usType, usScroll));

}

/***** End Of Source *****/
