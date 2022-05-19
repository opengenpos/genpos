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
* Title       : Multiline Display Operator Sign-In
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDOOPN.c
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:
*      MldItemOpeOpen() : display items specified class "CLASS_ITEMOPEOPEN"
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Mar-14-95 : 03.00.00 : M.Ozawa    : Initial
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
#include<mld.h>
#include"mldlocal.h"
#include"mldrin.h"
#include "termcfg.h"

/*
*===========================================================================
** Format  : VOID   MldItemOpeOpen(ITEMOPEOPEN *pItem, USHORT usType,
*                                   USHORT usScroll;
*   Input  : ITEMTRANSOPEN    *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination display
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays operator sign-in
*===========================================================================
*/
SHORT MldItemOpeOpen(ITEMOPEOPEN  *pItem, USHORT usType, USHORT usScroll)
{
    SHORT sStatus;

    switch (pItem->uchMinorClass) {
        case CLASS_CASHIERIN:
        case CLASS_B_IN:
        case CLASS_CASINTIN:    /* R3.3 */
        case CLASS_CASINTB_IN:  /* R3.3 */
            sStatus = MldCashierSignIn(pItem, usType, usScroll);
            break;

        case CLASS_WAITERIN:
        case CLASS_BARTENDERIN:
            sStatus = MldWaiterSignIn(pItem, usType, usScroll);
            break;

        default:
            sStatus = MLD_SUCCESS;
            break;
    }

    return (sStatus);
}

/***** End Of Source *****/
