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
* Title       : Print CPM\EPT Tally Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSTLY_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function forms Flexible Memory Assignment print format.
*
*           The provided function names are as follows: 
* 
*               PrtSupTally() : form Flexible Memory Assignment
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
/* #include <pif.h> */
#include <paraequ.h> 
#include <para.h>
#include <maint.h>
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtSupTally( PARACPMTALLY *pTally)
*               
*     Input:    *pTally     : Pointer to Data
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms CPM/EPT tally print format.
*===========================================================================
*/

VOID  PrtSupTally(PARACPMTALLY * pTally)
{
    USHORT usPrtType;
    TCHAR  auchData[6] = {0,0,0,0,0,0};             /* ushort to decimal */

   static TCHAR FARCONST auchPrtSupTally[] = _T("%-15s %5s");

    /* Check Print Control */
    
    usPrtType = PrtSupChkType(pTally->usPrintControl);

    /* convert hex to decimal */

    _ultot( (ULONG)pTally->usTallyData, auchData, 10);

    /* Print Line */

    PmgPrintf(usPrtType,                            /* Printer Type */
              auchPrtSupTally,                      /* Format */
              pTally->auchMnemo,                    /* Mnemonics */
              auchData);                            /* Data */
}

