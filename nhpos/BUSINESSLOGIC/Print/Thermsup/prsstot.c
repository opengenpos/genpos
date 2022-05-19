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
* Title       : Thermal Print Store/Register No. Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSSTOT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:            
* 
*               PrtThrmSupStoRegNo()  : form Store/Register No. print format     
*                                   
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-17-93: 01.00.12 : J.IKEDA   : Initial                                   
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
/* #include <pif.h> */
#include <paraequ.h> 
#include <para.h>
#include <csttl.h>
/* #include <cswai.h> */
#include <csop.h>
#include <report.h> 
#include <maint.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <pmg.h>

#include "prtcom.h"
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID PrtThrmSupStoRegNo( PARASTOREGNO *pData )
*               
*     Input:    *pData      : pointer to structure for PARASTOREGNO
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This function forms STORE/REGISTER NO. print format.
*
*                : STORE/REGISTER NO.
*
*===========================================================================
*/

VOID PrtThrmSupStoRegNo( PARASTOREGNO *pData )
{

    /* define thermal print format */

    static const TCHAR FARCONST auchPrtThrmSupStoRegNo[] = _T("%18u  /  %04u%03u");

    /* define EJ print format */

    static const TCHAR FARCONST auchPrtSupStoRegNo[] = _T("%8u / %04u%03u");

    /* check print control */

    if (pData->usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */

        /* print ADDRESS / STORE/REGISTER No. */

        PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                  auchPrtThrmSupStoRegNo,           /* format */
                  ( USHORT)pData->uchAddress,       /* address */
                  pData->usStoreNo,                 /* store number */
                  pData->usRegisterNo);             /* register number */
    } 
    
    if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */

        /* print ADDRESS / STORE/REGISTER No. */

        PrtPrintf(PMG_PRT_JOURNAL,                  /* printer type */
                  auchPrtSupStoRegNo,               /* format */
                  ( USHORT)pData->uchAddress,       /* address */
                  pData->usStoreNo,                 /* store number */
                  pData->usRegisterNo);             /* register number */   
    }
}
/***** End of Source *****/
