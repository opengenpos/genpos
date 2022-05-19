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
* Nov-16-95 : 03.01.00 : M.Ozawa    : Initial
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

#if defined(POSSIBLE_DEAD_CODE)

/**
;========================================================================
;+                  I N C L U D E     F I L E s                         +
;========================================================================
**/
/**------- 2170 local------**/
#include	<tchar.h>
#include <ecr.h>
#include <rfl.h>
#include <regstrct.h>
#include <transact.h>
#include <paraequ.h>
#include <para.h>
#include <mld.h>
#include "mldlocal.h"
#include "mldrin.h"
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
        return (MLD_NOT_DISPLAY);
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
    case CLASS_CHARGETIP:               /* charge Tips */

        sStatus = MldItemRegDisc(pItem, usType, usScroll);
        break;

    default:

        sStatus = MLD_NOT_DISPLAY;
        break;

    }

    return (sStatus);
}
#endif
/***** End Of Source *****/
