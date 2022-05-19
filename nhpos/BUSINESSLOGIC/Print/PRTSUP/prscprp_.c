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
* Title       : Coupon File Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSDERP_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               PrtSupCPNFile()    : This Function Forms Coupon File Print Format.       
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
*    -  -95: 00.00.01 : Waki      : initial                                   
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
#include <string.h>
#include <ecr.h>
/* #include <pif.h> */
#include <rfl.h>
/* #include <log.h> */
#include <paraequ.h> 
#include <para.h>
#include <maint.h> 
#include <regstrct.h>
#include <transact.h>
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>
/* #include <appllog.h> */

#include "prtrin.h"
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtSupCPNFile( RPTCPN *pData )  
*               
*     Input:    *pData  : Pointer to Structure for RPTCPN 
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This Function Forms Coupon File Print Format.
*===========================================================================
*/
VOID  PrtSupCPNFile( RPTCPN *pData )  
{
    static const TCHAR FARCONST auchPrtSupCPNFile1[] = _T("  %6s  %-12s\n%12ld\t%l$");
    static const TCHAR FARCONST auchPrtSupCPNFile2[] = _T("%-8s\n%12ld\t%l$");
    static const TCHAR FARCONST auchNumber[] = _T("%3d");

    TCHAR  aszDoubRepoNumb[8 * 2 + 1];
    TCHAR  aszRepoNumb[8 + 1];
    USHORT usPrtType;

    /* Initialize aszDoubRepoNumb */
    memset(aszDoubRepoNumb, '\0', sizeof(aszDoubRepoNumb));

    /* Check Print Control */
    usPrtType = PrtSupChkType(pData->usPrintControl);  

    /* Check Minor Class */
    switch(pData->uchMinorClass) {

    case CLASS_RPTCPN_ITEM:                      
        /* Convert CPN No. to Double Wide */
        RflSPrintf(aszRepoNumb, TCHARSIZEOF(aszRepoNumb), auchNumber, pData->usCpnNumber);
        PrtDouble(aszDoubRepoNumb, TCHARSIZEOF(aszDoubRepoNumb), aszRepoNumb);

        PmgPrintf(usPrtType, auchPrtSupCPNFile1, aszDoubRepoNumb, pData->aszMnemo, pData->CpnTotal.lCounter, pData->CpnTotal.lAmount);
        break;

    case CLASS_RPTCPN_TOTAL:
        PmgPrintf(usPrtType, auchPrtSupCPNFile2, pData->aszMnemo, pData->CpnTotal.lCounter, pData->CpnTotal.lAmount);
        break;

    default:
/*      PifLog(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_ERROR); */
        break;
    }
}
/*
*===========================================================================
** Synopsis:    VOID  PrtSupCPNFile_s( RPTCPN *pData )  
*               
*     Input:    *pData  : Pointer to Structure for RPTCPN 
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This Function Forms Coupon File Print Format For 21.
*===========================================================================
*/

VOID  PrtSupCPNFile_s( RPTCPN *pData )  
{
    static const TCHAR FARCONST auchPrtSupCPNFile1[] = _T("  %6s %-12s\n\t%12ld\n\t%l$");
    static const TCHAR FARCONST auchPrtSupCPNFile2[] = _T("%-8s \t%12ld\n\t%l$");
    static const TCHAR FARCONST auchNumber[] = _T("%3d");

    TCHAR  aszDoubRepoNumb[8 * 2 + 1];
    TCHAR  aszRepoNumb[8 + 1];
    USHORT usPrtType;

    /* Initialize aszDoubRepoNumb */
    memset(aszDoubRepoNumb, '\0', sizeof(aszDoubRepoNumb));

    /* Check Print Control */
    usPrtType = PrtSupChkType(pData->usPrintControl);  

    /* Check Minor Class */
    switch(pData->uchMinorClass) {

    case CLASS_RPTCPN_ITEM:                      
        /* Convert CPN No. to Double Wide */
        RflSPrintf(aszRepoNumb, TCHARSIZEOF(aszRepoNumb), auchNumber, pData->usCpnNumber);
        PrtDouble(aszDoubRepoNumb, TCHARSIZEOF(aszDoubRepoNumb), aszRepoNumb);

        PmgPrintf(usPrtType, auchPrtSupCPNFile1, aszDoubRepoNumb, pData->aszMnemo, pData->CpnTotal.lCounter, pData->CpnTotal.lAmount);
        break;

    case CLASS_RPTCPN_TOTAL:
        PmgPrintf(usPrtType, auchPrtSupCPNFile2, pData->aszMnemo, pData->CpnTotal.lCounter, pData->CpnTotal.lAmount);
        break;

    default:
/*      PifLog(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_ERROR); */
        break;
    }
}

/**** End of File ****/
