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
* Title       : Print Total Key Control Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSKCTL_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               PrtSupTtlKeyCtl()   : Form Total Key Control Data
*                                   
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-19-92: 00.00.01 : K.You     : initial                                   
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
#include <paraequ.h> 
#include <para.h>
#include <maint.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtSupTtlKeyCtl( PARATTLKEYCTL *pData )
*               
*     Input:    *pData      : Pointer to Structure for PARATTLKEYCTL
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Forms Total Key Control Data.
*===========================================================================
*/

VOID  PrtSupTtlKeyCtl( PARATTLKEYCTL *pData )
{

    

    static const TCHAR FARCONST auchPrtSupTtlKeyCtl[] = _T("%5u - %2u / %s");

    TCHAR           aszBuffer[5];
    USHORT          usPrtType;



    PrtSupItoa(pData->uchTtlKeyMDCData, aszBuffer);             /* Convert Status Data to Binary ASCII Data */
                                                                

    /* Check Print Control */
    
    usPrtType = PrtSupChkType(pData->usPrintControl);


    /* Print Line */

    PmgPrintf(usPrtType,                        /* Printer Type */
              auchPrtSupTtlKeyCtl,              /* Format */
              ( USHORT)pData->uchAddress,       /* Address Data */
              ( USHORT)pData->uchField,         /* Field Data */
              aszBuffer);                       /* Status */

}
