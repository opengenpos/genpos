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
* Mar-13-95 : 03.00.00 : M.Ozawa    : Initial
* Dec-15-15 : 02.02.01 : R.Chambers : replace MLD_ total type defines with standard PRT_ defines.
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
#include <csstbpar.h>
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
        sStatus = MldDummyScrollDisplay(usType, usScroll);
        break;

    case CLASS_TOTAL2:               /* check total */
    case CLASS_TOTAL2_FS:            /* Saratoga */
    case CLASS_TOTAL9:               /* FS Total */
        sStatus = MldTotal(pItem, usType, usScroll);
        break;

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
        switch (ITM_TTL_FLAG_TOTAL_TYPE(pItem)) {

        case PRT_FINALIZE1:                       /* finalize total */
        case PRT_FINALIZE2:                       
        case PRT_NOFINALIZE:                      /* no finalize total */
            sStatus = MldTotal(pItem, usType, usScroll); 
            break;

        case PRT_SERVICE1:
        case PRT_SERVICE2:                        /* service total */
        case PRT_CASINT1:
        case PRT_CASINT2:                         /* cashier interrupt, R3.3 */
            sStatus = MldServTotal(pItem, usType, usScroll);
            break;

        case PRT_TRAY1:
        case PRT_TRAY2:                           /* tray total */
            if (CliParaMDCCheck(MDC_DISPLAY_ADR, ODD_MDC_BIT0)) {
                /* if consolidate by total key, not display tray total */
                sStatus = MldDummyScrollDisplay(usType, usScroll);
            } else {
                sStatus = MldTrayTotal(pItem, usType, usScroll);
            }
            break;

        default:
            sStatus = MldDummyScrollDisplay(usType, usScroll);
            break;
        }
        break;

    case CLASS_TOTALPOSTCHECK:
        sStatus = MldServTotal(pItem, usType, usScroll);
/*        sStatus = MldTotal(pItem, usType, usScroll); */
        break;

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
        sStatus = MldServTotal(pItem, usType, usScroll);
/*        sStatus = MldSoftCheckTotal(pItem, usType, usScroll); */
        break;

    default:
        sStatus = MldDummyScrollDisplay(usType, usScroll);
        break;

    }

    return (sStatus);
}

/***** End Of Source *****/
