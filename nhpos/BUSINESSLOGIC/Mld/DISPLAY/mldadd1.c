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
* Program Name: MLDADD1.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*   MldAddChk1()    : display add check operation
*
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
#include <mld.h>
#include "mldlocal.h"
#include "mldrin.h"
#include "termcfg.h"

/*
*===========================================================================
** Format  : VOID   MldAddChk1( ITEMTRANSOPEN *pItem, USHORT usType,
*                               USHORT usScroll);
*
*   Input  : ITEMTRANSOPEN   *pItem     -   Transaction open data
*            USHORT          usType     -   First display position
*            USHORT          usScroll   -   Destination scroll display
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays add check operation
*===========================================================================
*/
SHORT MldAddChk1( ITEMTRANSOPEN *pItem , USHORT usType, USHORT usScroll)
{

    if (uchMldSystem == MLD_PRECHECK_SYSTEM) {

        MldPrechkAddChk1(pItem, usType, usScroll);

    } 

    return (MldDummyScrollDisplay(usType, usScroll));
}

/*
*===========================================================================
** Format  : VOID   MldAddChk1( ITEMTRANSOPEN *pItem, USHORT usType,
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
SHORT MldPrechkAddChk1( ITEMTRANSOPEN *pItem , USHORT usType, USHORT usScroll)
{
    /* display guest no on display#2 */
    switch (usType) {
    case MLD_NEW_ITEMIZE:
        MldWriteGuestNo(pItem);
    }

    return (MLD_SUCCESS);

    usScroll = usScroll;
}

/***** End Of Source *****/
