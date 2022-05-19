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
* Title       : Print Combination Coupon Setting Format  ( SUPER & PROGRAM MODE )
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSCPN_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function forms Combination Coupon print format.
*
*           The provided function names are as follows: 
* 
*               PrtSupCPN()    : form Combination Coupon print format
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Mar-15-95: 03.00.00 : H.Ishida  : initial
* Jul-20-95: 03.00.01 : M.Ozawa   : change to print coupon group code.
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
#include <rfl.h>
#include <pmg.h>
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtSupCPN( PARACPN *pData )
*               
*     Input:    *pData      : Pointer to Structure for PARACPN
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms Combination Coupon print format.
*===========================================================================
*/

VOID  PrtSupCPN( PARACPN *pData )
{


    /* Define Format Type */

    static const TCHAR FARCONST auchPrtSupCPN1[] = _T("%3d\t%-12s");
    static const TCHAR FARCONST auchPrtSupCPN2[] = _T("%4s %4s %4s %4s");
    static const TCHAR FARCONST auchPrtSupCPN3[] = _T("%13s-%1d-%1d");
    static const TCHAR FARCONST auchPrtSupCPN4[] = _T("\t@ %8l$");

    /* Initialize Buffer */

    TCHAR   aszAddress2[5] = { 0, 0, 0, 0, 0 };
    TCHAR   aszAddress3[5] = { 0, 0, 0, 0, 0 };
    TCHAR   aszAddress4[5] = { 0, 0, 0, 0, 0 };
    TCHAR   aszAddress5[5] = { 0, 0, 0, 0, 0 };

    USHORT usPrtType;
    USHORT usCPNNumber;

	int    iLoop;

	// change to use the standard number of coupons #define STD_MAX_NUM_PLUDEPT_CPN
	TCHAR  aszPLUNoAll[STD_MAX_NUM_PLUDEPT_CPN][PLU_MAX_DIGIT+1];

	memset (aszPLUNoAll, 0, sizeof(aszPLUNoAll));

	for (iLoop = 0; iLoop < STD_MAX_NUM_PLUDEPT_CPN; iLoop++) {
		RflConvertPLU(aszPLUNoAll[iLoop], &(pData->auchCPNTarget[iLoop][0]));
	}
    
    /* Check Print Control */
    
    usPrtType = PrtSupChkType(pData->usPrintControl);

    /* Convert CPN Status Data to Binary ASCII Data */

    PrtSupItoa(( UCHAR)(pData->uchCPNStatus[0] & 0x0F), aszAddress2);
    PrtSupItoa(( UCHAR)((pData->uchCPNStatus[0] & 0xF0) >> 4), aszAddress3);
    PrtSupItoa(( UCHAR)(pData->uchCPNStatus[1] & 0x0F), aszAddress4);
    PrtSupItoa(( UCHAR)((pData->uchCPNStatus[1] & 0xF0) >> 4), aszAddress5);

    /* 1st Line */
    usCPNNumber = 0;
    usCPNNumber = (USHORT)pData->uchCPNNumber;
    PmgPrintf(usPrtType,                        /* printer type */
              auchPrtSupCPN1,                   /* format */
              ( USHORT)(usCPNNumber),           /* CPN number */
              pData->aszMnemonic);              /* mnemonic */

    /* 2nd Line */

    PmgPrintf(usPrtType,                        /* printer type */
              auchPrtSupCPN2,                   /* format */
              aszAddress2,                      /* Status of Address 2 */
              aszAddress3,                      /* Status of Address 3 */
              aszAddress4,                      /* Status of Address 4 */
              aszAddress5);                     /* Status of Address 5 */
                                                
	for (iLoop = 0; iLoop < STD_MAX_NUM_PLUDEPT_CPN; iLoop++) {
		PmgPrintf(usPrtType,                                    /* printer type */
			  auchPrtSupCPN3,                                   /* format */
			  aszPLUNoAll[iLoop],                               /* Coupon Taget #1 number */
			  ( USHORT)(pData->uchCPNAdj[iLoop] & 0x0F),        /* Adjective Code #1      */
			  ( USHORT)((pData->uchCPNAdj[iLoop] & 0xF0) >>4)); /* Group Code #1          */
	}

    /* 10th line */

    PmgPrintf(usPrtType,                                    /* printer type */
              auchPrtSupCPN4,                               /* format */
              ( ULONG)(pData->ulCPNAmount));                /* Coupon Amount */

}

/**** End of File ****/
