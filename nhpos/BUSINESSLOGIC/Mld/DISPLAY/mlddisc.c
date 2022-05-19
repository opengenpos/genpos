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
* Title       : Multiline Display discount                 
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDDISC.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct:
*      MldItemDisc() : print items specified class "CLASS_ITEMDISC"
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Mar-02-95 : 03.00.00 : M.Ozawa    : Initial
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
/**------- 2170 local------**/
#include	<tchar.h>
#include<ecr.h>
#include<regstrct.h>
#include<transact.h>
#include<mld.h>
#include"mldlocal.h"
#include"mldrin.h"
#include "termcfg.h"

/*
*===========================================================================
** Format  : VOID   MldItemDisc(ITEMDISC *pItem, USHORT usType);      
*
*   Input  : ITEMDISC         *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints item classfied "CLASS_ITEMDISC" .
*===========================================================================
*/
SHORT  MldItemDisc(ITEMDISC  *pItem, USHORT usType, USHORT usScroll)
{
    SHORT sStatus;

    if (pItem->fbReduceStatus & REDUCE_ITEM) {
        return (MldDummyScrollDisplay(usType, usScroll));
    }
    /* V3.3 */
    if (pItem->fsPrintStatus & PRT_NO_UNBUFFING_PRINT) {
        return (MldDummyScrollDisplay(usType, usScroll));
    }

    switch (pItem->uchMinorClass) {

    case CLASS_ITEMDISC1:               /* item discount */
        if (usType == MLD_NEW_ITEMIZE) usType = MLD_NEW_ITEMIZE2;
        if (usType == MLD_UPDATE_ITEMIZE) usType = MLD_UPDATE_ITEMIZE2;
    case CLASS_REGDISC1:                /* regular discount */
    case CLASS_REGDISC2:
    case CLASS_REGDISC3:
    case CLASS_REGDISC4:
    case CLASS_REGDISC5:
    case CLASS_REGDISC6:

        sStatus = MldItemRegDisc(pItem, usType, usScroll);
        break;

    case CLASS_CHARGETIP:               /* charge Tips */
    case CLASS_CHARGETIP2:              /* charge Tips, V3.3 */
    case CLASS_CHARGETIP3:              /* charge Tips, V3.3 */
    case CLASS_AUTOCHARGETIP:           /* charge Tips, V3.3 */
    case CLASS_AUTOCHARGETIP2:          /* charge Tips, V3.3 */
    case CLASS_AUTOCHARGETIP3:          /* charge Tips, V3.3 */
        sStatus = MldChrgTip(pItem, usType, usScroll);
        break;

    default:

        sStatus = MldDummyScrollDisplay(usType, usScroll);
        break;

    }

    return (sStatus);
}

/***** End Of Source *****/
