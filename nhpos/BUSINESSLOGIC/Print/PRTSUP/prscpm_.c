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
* Title       : Print  CPM Start & Stop  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSCPM_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function forms Flexible Memory Assignment print format.
*
*           The provided function names are as follows: 
* 
*               PrtSupCpm() : form Flexible Memory Assignment
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Nov.15.93:02.00.02  : M.Yamamoto: Init.
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
==============================================================================
;                      I N C L U D E     F I L E s                         
=============================================================================
**/
#include	<tchar.h>
#include <ecr.h>
#include <stdlib.h>
#include <pif.h>
#include <paraequ.h> 
#include <para.h>
#include <maint.h>
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>
#include <cpm.h>
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtSupCpm( MAINTCPM *pCpm)
*               
*     Input:    *pCpm       : Pointer to Data
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms CPM Start or Stop.
*===========================================================================
*/

VOID  PrtSupCpm(MAINTCPM *pCpm)
{
    USHORT usPrtType;

    static TCHAR FARCONST auchPrtSupCpm[] = _T("%-5s");

    /* Check Print Control */
    
    usPrtType = PrtSupChkType(pCpm->usPrtControl);

    if (pCpm->uchType == CPM_CHANGE_START) {            /* uifac78.c UISUP_START_CPM */
        PmgPrintf(usPrtType,                            /* Printer Type */
                  auchPrtSupCpm,                        /* Format */
                  _T("START"));                             /* Mnemonics */
    } else {
        PmgPrintf(usPrtType,                            /* Printer Type */
                  auchPrtSupCpm,                        /* Format */
                  _T("STOP"));                              /* Mnemonics */
    }    
}

