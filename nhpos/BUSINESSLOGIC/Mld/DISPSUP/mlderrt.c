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
* Title       : Multiline Display Report Error Format  ( SUPER & PROGRAM MODE )                       
* Category    : Multiline Display, NCR 2170 US Hospitality Application Program        
* Program Name: MLDERRT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function forms Error print format.
*
*           The provided function names are as follows: 
* 
*               MldRptSupErrorCode() : form Error print format
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
*    -  -95: 03.00.00 :           : Initial 
*          :          :           :                                    
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
#include <cswai.h>
#include <maint.h> 
#include <regstrct.h>
#include <transact.h>
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>
#include <appllog.h>
#include <mld.h>
#include <mldsup.h>
#include <mldlocal.h>

#include <mldmenu.h>

/*
*===========================================================================
** Synopsis:    VOID  MldRptSupErrorCode( MAINTERRORCODE *pData )
*               
*     Input:    *pData      : pointer to structure for MAINTERRORCODE
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms Error Code print format.
*===========================================================================
*/

VOID  MldRptSupErrorCode( MAINTERRORCODE *pData )
{

    /*  define display format */

    static const TCHAR FARCONST auchMldRptSupError[] = _T("E- %4d");

    TCHAR  aszString[MLD_SUPER_MODE_CLM_LEN+1];           /* buffer for formatted data */

    /* display ERROR CODE */

    MldSPrintf(aszString,                   /* display buffer */
              MLD_SUPER_MODE_CLM_LEN,       /* column length */
              auchMldRptSupError,           /* format */
              pData->sErrorCode);           /* error code */

    MldString(MLD_SCROLL_1, MLD_SCROLL_APPEND, aszString, MLD_SUPER_MODE_CLM_LEN);
}

/***** End of Source *****/

