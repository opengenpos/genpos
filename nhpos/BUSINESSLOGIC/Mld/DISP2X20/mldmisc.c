/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS, E&M OISO                **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995-2000       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Multiline Display Item  misc
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDMISC.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct:
*      MldItemMisc() : print items specified class "CLASS_ITEMMISC"
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Nov-16-95 : 03.01.00 : M.Ozawa    : Initial
* Jan-02-00 : 01.00.00 : hrkato     : Saratoga
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
#include "ecr.h"
#include "regstrct.h"
#include "transact.h"
#include "paraequ.h"
#include "para.h"
#include "mld.h"
#include "mldrin.h"

/*
*===========================================================================
** Format  : VOID   MldItemMisc(ITEMMISC *pItem, USHORT usType);
*
*   Input  : ITEMMISC         *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays item classfied "CLASS_ITEMMISC"
*===========================================================================
*/
SHORT   MldItemMisc(ITEMMISC  *pItem, USHORT usType, USHORT usScroll)
{
    SHORT sStatus;

    switch (pItem->uchMinorClass) {

    case CLASS_CANCEL:                  /* cancel */
        sStatus = MldCancel(pItem, usType, usScroll);
        break;

    default:
        sStatus = MLD_NOT_DISPLAY;
        break;
    }

    return (sStatus);
}
#endif
/***** End Of Source *****/