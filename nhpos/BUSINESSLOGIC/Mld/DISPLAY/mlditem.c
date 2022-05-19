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
*
*  NeighborhoodPOS Software Development Project
*  Department of Information Systems
*  College of Information Technology
*  Georgia Southern University
*  Statesboro, Georgia
*
*  Copyright 2014 Georgia Southern University Research and Services Foundation
* 
*===========================================================================
* Title       : Multiline Display Item  main                 
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDITEM.C                                                      
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Mar-15-95 : 03.00.00 : M.Ozawa    : Initial
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
/**------- 2170 local------**/
#include <tchar.h>
#include <ecr.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <regstrct.h>
#include <transact.h>
#include <csttl.h>
#include <mld.h>
#include "mldlocal.h"
#include "mldrin.h"
#include "termcfg.h"

/*
============================================================================
+                  S T A T I C   R A M s
============================================================================
*/

/**
;========================================================================
;+        P R O G R A M    D E C L A R A T I O N s                      +
;========================================================================
**/
/*
*===========================================================================
** Format  : USHORT   MldLocalScrollWrite(VOID *pItem, USHORT usType, USHORT usScroll);
*               
*    Input : VOID             *pItem     -Item Data address
*            USHORT           usType     -First display position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function distribute data structur according to data class
*
*   WARNING:  condiment prices for condiments that are PPI PLUs are updated by using
*             function ItemSalesCalcCondimentPPIReplace() so the condiment prices
*			  in ITEMSALES pItem are updated if there are PPI PLUs specified in the
*			  item data.
*===========================================================================
*/
SHORT   MldLocalScrollWrite(VOID *pItem, USHORT usType, USHORT usScroll)
{
    SHORT sStatus;

    switch ( ((MLDITEMDATA *)pItem)->uchMajorClass ) {

    case CLASS_ITEMOPEOPEN:             /* operator open */
        sStatus = MldItemOpeOpen(pItem, usType, usScroll);
        break;

    case CLASS_ITEMPRINT:               /* header, trailer */
        sStatus = MldItemPrint(pItem, usType, usScroll);
        break;

    case CLASS_ITEMSALES:               /* plu, dept, setmenu, mod.discount */
		sStatus = MldItemSales(pItem, usType, usScroll);
        break;

    case CLASS_ITEMDISC:                /* discount */
        sStatus = MldItemDisc(pItem, usType, usScroll);
        break;

    case CLASS_ITEMCOUPON:              /* coupon */
        sStatus = MldItemCoupon(pItem, usType, usScroll);
        break;

    case CLASS_ITEMMISC:                /* nosale, ROA, paid out... */
        sStatus = MldItemMisc(pItem, usType, usScroll);
        break;

    case CLASS_ITEMTENDER:              /* tender */
        sStatus = MldItemTender(pItem, usType, usScroll);
        break;

    case CLASS_ITEMAFFECTION:           /* tax */
        sStatus = MldItemAffection(pItem, usType, usScroll);
        break;

    case CLASS_ITEMTRANSOPEN:           /* newcheck , reorder */
        sStatus = MldItemTransOpen(pItem, usType, usScroll);
        break;

    case CLASS_ITEMTOTAL:               /* total */
        sStatus = MldItemTotal(pItem, usType, usScroll);
        break;

    case CLASS_ITEMMULTI:               /* check paid */
        sStatus = MldItemMulti(pItem, usType, usScroll);
        break;

    case CLASS_ITEMMODIFIER:            /* modifier */
        sStatus = MldItemModifier(pItem, usType, usScroll);
        break;

    default:
        sStatus = MldDummyScrollDisplay(usType, usScroll);
        break;

    }

    return (sStatus);
}

/***** End Of Source *****/


