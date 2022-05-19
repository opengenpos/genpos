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
* Title       : Thermal Print CPN Setting Format  ( SUPER & PROGRAM MODE )
* Category    : Print, NCR 2170 US Hospitality Application Program
* Program Name: PRSCPNT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: This function forms CPN print format.
*
*           The provided function names are as follows:
*
*               PrtThrmSupCPN()     : form CPN Maintenance print format
*
* --------------------------------------------------------------------------
* Update Histories
*    Date  : Ver.Rev. :   Name    : Description
* May-07-95: 03.00.00 : T.Satoh   : Initial
* Jul-21-95: 03.00.01 : M.Ozawa   : Add coupon group code print
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
#include <stdlib.h>

#include <ecr.h>
/* #include <pif.h> */
/* #include <rfl.h> */
#include <paraequ.h> 
#include <para.h>
/* #include <csop.h> */
/* #include <cswai.h> */
#include <maint.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <rfl.h>
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>

#include "prtcom.h"
/* #include "prtrin.h" */
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtThrmSupCPN( PARACPN *pData )
*               
*     Input:    *pData      : pointer to structure for PARACPN
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms CPN print format.
*
*                    : COUPON MAINTENANCE
*                    : COUPON FILE READ REPORT
*
*===========================================================================
*/

VOID  PrtThrmSupCPN( PARACPN *pData )
{
    /*
     * define thermal print format
     */
    static const TCHAR FARCONST auchPrtThrmSupCPN1[] = _T("  %3d    %-12s");
    static const TCHAR FARCONST auchPrtThrmSupCPN2[] = _T("    %4s %4s %4s %4s");
    static const TCHAR FARCONST auchPrtThrmSupCPN3[] = _T("    %13s-%1d-%1d %13s-%1d-%1d");
    static const TCHAR FARCONST auchPrtThrmSupCPN4[] = _T("    %13s-%1d-%1d");
    static const TCHAR FARCONST auchPrtThrmSupCPN5[] = _T("\t@ %8l$");

    /*
     * define EJ print format
     */
    static const TCHAR FARCONST auchPrtSupCPN1[] = _T("%3d\t%-12s");
    static const TCHAR FARCONST auchPrtSupCPN2[] = _T("%4s %4s %4s %4s");
    static const TCHAR FARCONST auchPrtSupCPN3[] = _T("%13s-%1d-%1d");
    static const TCHAR FARCONST auchPrtSupCPN4[] = _T("\t@ %8l$");

    /*
     * work status character buffers
     */
    TCHAR  aszAddress1[5];
    TCHAR  aszAddress2[5];
    TCHAR  aszAddress3[5];
    TCHAR  aszAddress4[5];

	int    iLoop;

	// change to use the standard number of coupons #define STD_MAX_NUM_PLUDEPT_CPN
	TCHAR  aszPLUNoAll[STD_MAX_NUM_PLUDEPT_CPN][PLU_MAX_DIGIT+1];

	memset (aszPLUNoAll, 0, sizeof(aszPLUNoAll));

	for (iLoop = 0; iLoop < STD_MAX_NUM_PLUDEPT_CPN; iLoop++) {
		RflConvertPLU(aszPLUNoAll[iLoop], &(pData->auchCPNTarget[iLoop][0]));
	}
    

    /*
     * convert CPN status data to binary ASCII data
     */
    PrtSupItoa((UCHAR)( pData->uchCPNStatus[0] & 0x0F),    aszAddress1);
    PrtSupItoa((UCHAR)((pData->uchCPNStatus[0] & 0xF0)>>4),aszAddress2);
    PrtSupItoa((UCHAR)( pData->uchCPNStatus[1] & 0x0F),    aszAddress3);
    PrtSupItoa((UCHAR)((pData->uchCPNStatus[1] & 0xF0)>>4),aszAddress4);

    /*
     * print to THERMAL PRINTER
     */
    if(pData->usPrintControl & PRT_RECEIPT){
        /*
         * print COUPON NO. and COUPON MNEMONIC
         */
        PrtPrintf(PMG_PRT_RECEIPT,auchPrtThrmSupCPN1, pData->uchCPNNumber, pData->aszMnemonic);

        /*
         * print STATUS
         */
        PrtPrintf(PMG_PRT_RECEIPT,auchPrtThrmSupCPN2, aszAddress1,aszAddress2,aszAddress3,aszAddress4);

        /*
         * print president PLU Number and Adjective Code
         */
		for (iLoop = 0; iLoop < STD_MAX_NUM_PLUDEPT_CPN; iLoop += 2) {
			PrtPrintf(PMG_PRT_RECEIPT,auchPrtThrmSupCPN3,
					  aszPLUNoAll[iLoop], ( USHORT)(pData->uchCPNAdj[iLoop] & 0x0F),
					  ( USHORT)((pData->uchCPNAdj[iLoop] & 0xF0) >>4),
					  aszPLUNoAll[iLoop + 1], ( USHORT)(pData->uchCPNAdj[iLoop + 1] & 0x0F),
					  ( USHORT)((pData->uchCPNAdj[iLoop + 1] & 0xF0) >>4));
		}
		// if STD_MAX_NUM_PLUDEPT_CPN is odd meaning there are an odd number of coupon entries then
		// the above loop will have printed all except the last coupon entry so we need to
		// print the last one.  STD_MAX_NUM_PLUDEPT_CPN is odd if least significant bit is turned on.
		// We have to subtract one to get the last element since arrays are zero based.
		if ((STD_MAX_NUM_PLUDEPT_CPN & 0x01)) {
			PrtPrintf(PMG_PRT_RECEIPT,auchPrtThrmSupCPN4,
					  aszPLUNoAll[STD_MAX_NUM_PLUDEPT_CPN - 1], ( USHORT)(pData->uchCPNAdj[STD_MAX_NUM_PLUDEPT_CPN - 1] & 0x0F),
					  ( USHORT)((pData->uchCPNAdj[STD_MAX_NUM_PLUDEPT_CPN - 1] & 0xF0) >>4));
		}

        /*
         * print AMOUNT (PRICE)
         */
        PrtPrintf(PMG_PRT_RECEIPT,auchPrtThrmSupCPN5, (DCURRENCY)pData->ulCPNAmount); // format of %8l$ requires DCURRENCY
    }

    /*
     * print to EJ
     */
    if(pData->usPrintControl & PRT_JOURNAL){
        /*
         * print COUPON NO. and COUPON MNEMONIC
         */
        PrtPrintf(PMG_PRT_JOURNAL,auchPrtSupCPN1, pData->uchCPNNumber, pData->aszMnemonic);

        /*
         * print STATUS
         */
        PrtPrintf(PMG_PRT_JOURNAL,auchPrtSupCPN2, aszAddress1,aszAddress2,aszAddress3,aszAddress4);

        /*
         * print present PLU Number and Adjective Code
         */
		for (iLoop = 0; iLoop < STD_MAX_NUM_PLUDEPT_CPN; iLoop++) {
			PrtPrintf(PMG_PRT_JOURNAL,                              /* printer type */
				  auchPrtSupCPN3,                                   /* format */
				  aszPLUNoAll[iLoop],                               /* Coupon Taget #1 number */
				  ( USHORT)(pData->uchCPNAdj[iLoop] & 0x0F),        /* Adjective Code #1      */
				  ( USHORT)((pData->uchCPNAdj[iLoop] & 0xF0) >>4)); /* Group Code #1          */
		}

        /*
         * print AMOUNT (PRICE)
         */
        PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupCPN4, (DCURRENCY)(pData->ulCPNAmount));   /* Coupon Amount, format of %8l$ requires DCURRENCY */

    }

    return;
}

/**** End of File ****/
