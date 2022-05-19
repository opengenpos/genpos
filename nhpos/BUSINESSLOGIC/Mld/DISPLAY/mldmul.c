/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Multiline Display Multiple Check Payment                
* Category    : Multiline Display, NCR 2170 US Hospitarity Application 
* Program Name: MLDMUL.C                                   
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:
*      MldItemMulti() : display items specified class "CLASS_ITEMMULTI"
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Mar-15-95 : 03.00.00 : M.Ozawa    : Initial
*           :          :            :                                    
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
** Format  : VOID  MldItemMulti(ITEMMULTI *pItem, USHORT usType, USHORT usScroll);
*
*   Input  : ITEMTOTAL        *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination Display
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays multicheck operation.
*===========================================================================
*/
SHORT MldItemMulti(ITEMMULTI *pItem, USHORT usType, USHORT usScroll)
{
    SHORT sStatus;

    switch (pItem->uchMinorClass) {

    case CLASS_MULTICHECKPAYMENT:
        sStatus = MldMulti(pItem, usType, usScroll);
        break;

    case CLASS_MULTIPOSTCHECK:
        sStatus = MldMultiPost(pItem, usType, usScroll);
        break;

    case CLASS_MULTITOTAL:
        sStatus = MldMultiTotal(pItem, usType, usScroll);
        break;

    case CLASS_MULTIPOSTTOTAL:
        sStatus = MldMultiPostTotal(pItem, usType, usScroll);
        break;     

    default:
        sStatus = MldDummyScrollDisplay(usType, usScroll);
        break;

    }

    return (sStatus);
}

/***** End Of Source *****/
