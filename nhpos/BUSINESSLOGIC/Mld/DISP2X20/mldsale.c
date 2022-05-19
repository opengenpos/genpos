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
* Title       : Display on Scroll Display Item  Dept PLU                 
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDSALE.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:
*      MldItemSales() : Display items specified class "CLASS_ITEMSALES"
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Nov-16-95 : 03.01.00 :  M.Ozawa   : Initial
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
#include<ecr.h>
#include<regstrct.h>
#include<transact.h>
#include <paraequ.h> 
#include<mld.h>
#include"mldlocal.h"
#include"mldrin.h"
#include "termcfg.h"

/*
*===========================================================================
** Format  : VOID   MldItemSales(ITEMSALES *pItem, USHORT usType);
*
*   Input  : ITEMSALES        *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays dept/plu/set sales and modified discount
*===========================================================================
*/
SHORT   MldItemSales(ITEMSALES  *pItem, USHORT usType, USHORT usScroll)
{
    SHORT sStatus;

    switch (pItem->uchMinorClass) {

    case CLASS_DEPT:                    /* dept */
    case CLASS_DEPTITEMDISC:
    case CLASS_DEPTMODDISC:
    case CLASS_PLU:                     /* PLU */
    case CLASS_PLUITEMDISC:
    case CLASS_PLUMODDISC:
    case CLASS_OEPPLU:                  /* PLU w/ order entry prompt */
    case CLASS_OEPITEMDISC:
    case CLASS_OEPMODDISC:
    case CLASS_SETMENU:                 /* setmenu */
    case CLASS_SETITEMDISC:
    case CLASS_SETMODDISC:
        sStatus = MldDeptPLU(pItem, usType, usScroll);
        break;

    default:
        sStatus = MLD_NOT_DISPLAY;
        break;

    }

    return (sStatus);
}
#endif
/***** End Of Source *****/
