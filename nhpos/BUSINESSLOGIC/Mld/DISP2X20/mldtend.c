/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS, E&M OISO                **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Multiline Display  tender key                
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: PrRTend_.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct:
*      PrtItemTender() : display items specified class "CLASS_ITEMTENDER"
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Nov-16-95 : 03.01.00 : M.Ozawa    : Initial
* Nov-30-99 : 01.00.00 : hrkato     : Saratoga
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
/**------- 2170 local------**/
#include	<tchar.h>
#include <ecr.h>
#include <regstrct.h>
#include <transact.h>
#include <paraequ.h>
#include <para.h>
#include <mld.h>
#include "mldrin.h"


/**
;============================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;============================================================================
**/
/*
*===========================================================================
** Format  : VOID  MldItemTender(ITEMTENDER *pItem, USHORT usType);      
*
*   Input  : ITEMTENDER       *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints tender operation
*===========================================================================
*/
SHORT  MldItemTender(ITEMTENDER  *pItem, USHORT usType, USHORT usScroll)
{
    SHORT sStatus;

    switch (pItem->uchMinorClass) {

    case CLASS_TENDER1:                 /* tender */
    case CLASS_TENDER2:
    case CLASS_TENDER3:
    case CLASS_TENDER4:
    case CLASS_TENDER5:
    case CLASS_TENDER6:
    case CLASS_TENDER7:
    case CLASS_TENDER8:
    case CLASS_TENDER9:
    case CLASS_TENDER10:
    case CLASS_TENDER11:
    case CLASS_TENDER12:
    case CLASS_TENDER13:
    case CLASS_TENDER14:
    case CLASS_TENDER15:
    case CLASS_TENDER16:
    case CLASS_TENDER17:
    case CLASS_TENDER18:
    case CLASS_TENDER19:     /* Saratoga */
    case CLASS_TENDER20:
        sStatus = MldTender(pItem, usType, usScroll);
        break;

    case CLASS_FOREIGN1:                /* foreign tender */
    case CLASS_FOREIGN2:                /* Saratoga */
    case CLASS_FOREIGN3:
    case CLASS_FOREIGN4:
    case CLASS_FOREIGN5:
    case CLASS_FOREIGN6:
    case CLASS_FOREIGN7:
    case CLASS_FOREIGN8:
        sStatus = MldForeignTender(pItem, usType, usScroll);
        break;

    default:
        sStatus = MLD_NOT_DISPLAY;
        break;
    }

    return (sStatus);
}
#endif
/***** End Of Source *****/