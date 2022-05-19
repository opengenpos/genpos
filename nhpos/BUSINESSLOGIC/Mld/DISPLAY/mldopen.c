/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Multiline Display Transaction Open                 
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDOPEN.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:
*      MldItemTransOpen() : display items specified class "CLASS_ITEMPRINT"
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
/**------- M S - C -------**/
#include	<tchar.h>
#include <stdlib.h>
#include <string.h>

/**------- 2170 local------**/
#include<ecr.h>
#include<regstrct.h>
#include<transact.h>
#include<paraequ.h> 
#include<para.h>
#include<rfl.h>
#include<prt.h>
#include<mld.h>
#include"mldlocal.h"
#include"mldrin.h"
#include "termcfg.h"

extern CONST TCHAR FARCONST  aszMldSignIn[];

/*
*===========================================================================
** Format  : VOID   MldItemTransOpen(ITEMTRANSOPEN *pItem, USHORT usType
*                                   USHORT usScroll);
*   Input  : ITEMTRANSOPEN    *pItem     -Item Data address
*            USHORT           usType     -First display position
*            USHORT           usScroll   -Destination dispaly
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays transaction open operation.
*===========================================================================
*/
SHORT   MldItemTransOpen(ITEMTRANSOPEN  *pItem, USHORT usType, USHORT usScroll)
{
    SHORT sStatus;

    switch (pItem->uchMinorClass) {
        case CLASS_CASHIER:                /* cashier in */
        case CLASS_CASINTOPEN:             /* R3.3 */
            sStatus = MldCasIn(pItem, usType, usScroll);
            break;

        case CLASS_NEWCHECK:                /* new check */
            sStatus = MldNewChk(pItem, usType, usScroll);
            break;

        case CLASS_REORDER:                 /* reorder */
        case CLASS_MANUALPB:                /* manual P/B */
        case CLASS_CASINTRECALL:           /* R3.3 */
            sStatus = MldReorder(pItem, usType, usScroll);
            /* set reorder status for add check of post check */
            switch(usType) {
            case MLD_NEW_ITEMIZE:
            case MLD_TRANS_DISPLAY:
                MldSetSpecialStatus(usScroll, MLD_REORDER2_STATUS);
            }
            break;

        case CLASS_ADDCHECK:                /* addcheck */
            sStatus = MldAddChk1(pItem, usType, usScroll);
            break;

        case CLASS_STORE_RECALL:
            sStatus = MldStoreRecall(pItem, usType, usScroll);
            break;

        case CLASS_OPENNOSALE:              /* no sales */
        case CLASS_OPENPO:                  /* paid out */
        case CLASS_OPENRA:                  /* received on account */
        case CLASS_OPENTIPSPO:              /* tips paid out */
        case CLASS_OPENTIPSDECLARED:        /* declared tips */
        case CLASS_OPENCHECKTRANSFER:       /* check transfer */
            sStatus = MldMiscOpen(pItem, usType, usScroll);
            break;

        default:
            sStatus = MldDummyScrollDisplay(usType, usScroll);
            break;
    }

    return (sStatus);
}

/*
*===========================================================================
** Format  : VOID  MldMiscOpen(ITEMTRANSOPEN *pItem);   
*   Input  : ITEMTRANSOPEN    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays cashier/waiter on lcd.
*===========================================================================
*/
SHORT   MldMiscOpen(ITEMTRANSOPEN  *pItem, USHORT usType, USHORT usScroll)
{
    if ((uchMldSystem == MLD_PRECHECK_SYSTEM) || (uchMldSystem == MLD_DRIVE_THROUGH_1SCROLL)) {

        return (MldPrechkMiscOpen(pItem, usType, usScroll));
    }

    return (MLD_SUCCESS);
}

/*
*===========================================================================
** Format  : VOID  MldPrechkMiscOpen(ITEMTRANSOPEN *pItem);   
*   Input  : ITEMTRANSOPEN    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays cashier/waiter on lcd.
*===========================================================================
*/
SHORT  MldPrechkMiscOpen(ITEMTRANSOPEN  *pItem, USHORT usType, USHORT usScroll)
{
    TRANMODEQUAL TranModeQual;

    TrnGetModeQual(&TranModeQual);

    MldScrollClear(MLD_SCROLL_2);

    if (TranModeQual.ulCashierID) {
        MldPrintf(MLD_SCROLL_2, MLD_SCROLL_APPEND, aszMldSignIn, TranModeQual.aszCashierMnem, RflTruncateEmployeeNumber(TranModeQual.ulCashierID));
    }

    if (TranModeQual.ulWaiterID) {
        MldPrintf(MLD_SCROLL_2, MLD_SCROLL_APPEND, aszMldSignIn, TranModeQual.aszWaiterMnem, RflTruncateEmployeeNumber(TranModeQual.ulWaiterID));
    }

    return (MLD_SUCCESS);

    pItem = pItem;
    usType = usType;
    usScroll = usScroll;
}

/***** End Of Source *****/
