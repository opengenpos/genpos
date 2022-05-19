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
* Title       : Print MDC/Action Code Security Setting Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSMDC_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: This function forms DEPT print format.
*
*           The provided function names are as follows: 
* 
*               PrtSupMDC()     : form MDC/Action Code Security print format
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-01-92: 00.00.01 :   K.You   : initial                                   
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
/* #include <pif.h> */
/* #include <log.h> */
#include <paraequ.h> 
#include <para.h>
#include <maint.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>
/* #include <appllog.h> */
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtSupMDC( VOID *pData )
*               
*     Input:    *pData      : Pointer to Structure for PARAMDC or PARAACTCODESEC
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms MDC/Action Code Security print format.
*===========================================================================
*/

VOID  PrtSupMDC( VOID *pData )
{

    

    static const TCHAR FARCONST auchPrtSupMDC[] = _T("%8d / %s");               /* Define 1 Line Format Type */
    TCHAR           aszBuffer[5];
    UCHAR           uchMajorClass;
    USHORT          usPrtType;
    USHORT          usAddress;
    USHORT          usPrintControl;
    PARAMDC         *pParaMDC;
    PARAACTCODESEC  *pParaActCode;


    uchMajorClass = *(( UCHAR *)(pData));                           /* Get Major Class */

    /* Distinguish Major Calss */ 
   
    switch (uchMajorClass) {
    case CLASS_PARAMDC:                                             /* MDC Setting Case */
        pParaMDC = ( PARAMDC *)pData;
        usAddress = pParaMDC->usAddress;
        PrtSupItoa(pParaMDC->uchMDCData, aszBuffer);                /* Convert Status Data to Binary ASCII Data */
        usPrintControl = pParaMDC->usPrintControl;
        break;

    case CLASS_PARAACTCODESEC:                                          /* Action Code Security Case */
        pParaActCode = ( PARAACTCODESEC *)pData;
        usAddress = pParaActCode->usAddress;
        PrtSupItoa(pParaActCode->uchSecurityBitPatern, aszBuffer);      /* Convert Status Data to Binary ASCII Data */
        usPrintControl = pParaActCode->usPrintControl;
        break;

    default:
/*        PifAbort(MODULE_PRINT_SUPPRG_ID, FAULT_INVALID_DATA); */
        return;
    }

    /* Check Print Control */
    
    usPrtType = PrtSupChkType(usPrintControl);


    /* Print Line */

    PmgPrintf(usPrtType,                        /* Printer Type */
              auchPrtSupMDC,                    /* Format */
              usAddress,                        /* Address Data */
              aszBuffer);                       /* Status */

}
