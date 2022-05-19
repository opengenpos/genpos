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
* Title       : Multiline Display  total key
* Category    : Multiline Display, NCR 2170 US Hospitarity Application 
* Program Name: MLDTTL.C                                 
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:
*      MldItemTotal() : display items specified class "CLASS_ITEMTOTAL"
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Nov-16-95 : 03.01.00 : M.Ozawa    : Initial
*============================================================================
*============================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*============================================================================
*/

#if defined(POSSIBLE_DEAD_CODE)

/*
=============================================================================
+                        I N C L U D E   F I L E s
=============================================================================
*/
/**------- MS - C ---------**/
#include	<tchar.h>
#include <string.h>

/**------- 2170 local------**/
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


/**
;============================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;============================================================================
**/
/*
*===========================================================================
** Format  : VOID   MldItemTotal(ITEMTOTAL *pItem, USHORT usType, USHORT usScroll);
*
*   Input  : ITEMTOTAL        *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays totalkey operation.
*===========================================================================
*/
SHORT MldItemTotal(ITEMTOTAL  *pItem, USHORT usType, USHORT usScroll)
{
    SHORT sStatus;

    switch (pItem->uchMinorClass) {

    case CLASS_TOTAL1:               /* sub total */
        sStatus = MLD_NOT_DISPLAY;
        break;

    case CLASS_TOTAL2:               /* check total */
    case CLASS_TOTAL3:
    case CLASS_TOTAL4:
    case CLASS_TOTAL5:
    case CLASS_TOTAL6:
    case CLASS_TOTAL7:
    case CLASS_TOTAL8:
	case CLASS_TOTAL10:
	case CLASS_TOTAL11:
	case CLASS_TOTAL12:
	case CLASS_TOTAL13:
	case CLASS_TOTAL14:
	case CLASS_TOTAL15:
	case CLASS_TOTAL16:
	case CLASS_TOTAL17:
	case CLASS_TOTAL18:
	case CLASS_TOTAL19:
	case CLASS_TOTAL20:
        if (((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == MLD_TRAY1) ||   /* tray total */
            ((pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) == MLD_TRAY2)) {
            sStatus = MLD_NOT_DISPLAY;
            break;            
        }
    case CLASS_TOTALPOSTCHECK:
    case CLASS_SOFTCHK_TOTAL3:
    case CLASS_SOFTCHK_TOTAL4:
    case CLASS_SOFTCHK_TOTAL5:
    case CLASS_SOFTCHK_TOTAL6:
    case CLASS_SOFTCHK_TOTAL7:
    case CLASS_SOFTCHK_TOTAL8:
    case CLASS_SOFTCHK_TOTAL9:
    case CLASS_SOFTCHK_TOTAL10:
    case CLASS_SOFTCHK_TOTAL11:
    case CLASS_SOFTCHK_TOTAL12:
    case CLASS_SOFTCHK_TOTAL13:
    case CLASS_SOFTCHK_TOTAL14:
    case CLASS_SOFTCHK_TOTAL15:
    case CLASS_SOFTCHK_TOTAL16:
    case CLASS_SOFTCHK_TOTAL17:
    case CLASS_SOFTCHK_TOTAL18:
    case CLASS_SOFTCHK_TOTAL19:
    case CLASS_SOFTCHK_TOTAL20:
        sStatus = MldTotal(pItem, usType, usScroll);
        break;


    default:
        sStatus = MLD_NOT_DISPLAY;
        break;

    }

    return (sStatus);
}
#endif
/***** End Of Source *****/
