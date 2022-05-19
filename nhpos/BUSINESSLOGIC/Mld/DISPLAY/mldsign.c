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
* Title       : Multiline Display Operator Sign-In Data
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDSIGN.C                        
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               MldCashierSignIn() : display cashier sign-in operation
*               MldWaiterSignIn() : display server sign-in operation
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Mar-14-92 : 03.00.00 : M.Ozawa    : Initial
*           :          :            :                                    
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
#include <paraequ.h>
#include <para.h>
#include <regstrct.h>
#include <transact.h>
#include "mld.h"
#include "mldlocal.h"
#include "mldrin.h"
#include "termcfg.h"
#include "rfl.h"

CONST TCHAR FARCONST  aszMldSignIn[] = _T("%-8.8s\t%8.8Mu"); 

/*
*===========================================================================
** Format  : VOID  MldCashierSignIn(ITEMTRANSOPEN *pItem);   
*   Input  : ITEMTRANSOPEN    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays sign in operation.
*===========================================================================
*/
SHORT  MldCashierSignIn(CONST ITEMOPEOPEN  *pItem, USHORT usType, USHORT usScroll)
{
    if (uchMldSystem == MLD_PRECHECK_SYSTEM) {

        return (MldPrechkCashierSignIn(pItem, usType, usScroll));

    } else if ((uchMldSystem == MLD_DRIVE_THROUGH_1SCROLL) &&
               (uchMldSystem2 != MLD_SPLIT_CHECK_SYSTEM)) {    /* not 3 scroll */

        return (MldPrechkCashierSignIn(pItem, usType, usScroll));
    }

    return(SUCCESS);
}

/*
*===========================================================================
** Format  : VOID  MldCashierSignIn(ITEMTRANSOPEN *pItem);   
*   Input  : ITEMTRANSOPEN    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays sign in operation.
*===========================================================================
*/
SHORT  MldPrechkCashierSignIn(CONST ITEMOPEOPEN  *pItem, USHORT usType, USHORT usScroll)
{
    TRANMODEQUAL TranModeQual;

    TrnGetModeQual(&TranModeQual);

    MldPrintf(MLD_SCROLL_2, MLD_SCROLL_TOP, aszMldSignIn, TranModeQual.aszCashierMnem, RflTruncateEmployeeNumber(TranModeQual.ulCashierID));

    return (MLD_SUCCESS);

    pItem = pItem;
    usType = usType;
    usScroll = usScroll;
}

/*
*===========================================================================
** Format  : VOID  MldWaiterSignIn(ITEMTRANSOPEN *pItem);   
*   Input  : ITEMTRANSOPEN    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays sign in operation.
*===========================================================================
*/
SHORT   MldWaiterSignIn(CONST ITEMOPEOPEN  *pItem, USHORT usType, USHORT usScroll)
{
    if (uchMldSystem == MLD_PRECHECK_SYSTEM) {

        return (MldPrechkWaiterSignIn(pItem, usType, usScroll));

    } else if ((uchMldSystem == MLD_DRIVE_THROUGH_1SCROLL) && (uchMldSystem2 != MLD_SPLIT_CHECK_SYSTEM)) {    /* not 3 scroll */

        return (MldPrechkWaiterSignIn(pItem, usType, usScroll));
    }

    return(SUCCESS);
}

/*
*===========================================================================
** Format  : VOID  MldWaiterIn(ITEMTRANSOPEN *pItem);   
*   Input  : ITEMTRANSOPEN    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays sign in operation.
*===========================================================================
*/
SHORT  MldPrechkWaiterSignIn(CONST ITEMOPEOPEN  *pItem, USHORT usType, USHORT usScroll)
{
    TRANMODEQUAL TranModeQual;
    USHORT  usTop=1;
	TCHAR   aszMnemonic[NUM_OPERATOR] = {0};

    TrnGetModeQual(&TranModeQual);
	wcsncpy (aszMnemonic, pItem->aszMnemonic, STD_CASHIERNAME_LEN);  // make a copy to preserve constness

    if (TranModeQual.ulCashierID) {
        MldPrintf(MLD_SCROLL_2, MLD_SCROLL_TOP, aszMldSignIn, TranModeQual.aszCashierMnem, RflTruncateEmployeeNumber(TranModeQual.ulCashierID));
        usTop=0;
    }
    MldPrintf(MLD_SCROLL_2, (SHORT)(usTop ? MLD_SCROLL_TOP : MLD_SCROLL_APPEND), aszMldSignIn, aszMnemonic, RflTruncateEmployeeNumber(pItem->ulID));

    return (MLD_SUCCESS);

    usType = usType;
    usScroll = usScroll;
}

/***** End Of Source *****/

