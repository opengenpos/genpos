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
* Title       : Thermal Print FSC Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSFSCT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function forms FSC FOR REGULAR KEYBOARD print format.
*
*           The provided function names are as follows: 
* 
*               PrtThrmSupFSC() : form FSC print format
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-16-93: 01.00.12 : J.IKEDA   : Initial                                   
* Nov-28-95: 03.01.00 : M.Ozawa   : print plu no at plu key entry
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
#include <uie.h>
#include <paraequ.h> 
#include <para.h>
#include <fsc.h>
/* #include <cswai.h> */
#include <maint.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>

#include "prtcom.h"
#include "prtsin.h"
#include "prrcolm_.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtThrmSupFSC( PARAFSC *pData )
*               
*     Input:    *pData      : pointer to structure for PARAFSC
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms FSC print format.
*
*               : FSC FOR REGULAR KEYBOARD
*
*===========================================================================
*/

VOID  PrtThrmSupFSC( PARAFSC *pData )
{

    USHORT  usMinorFSCData;
    TCHAR   aszPLUNumber[PLU_MAX_DIGIT+1];
    TCHAR   aszStatus[2] = { 0, 0 };

    if (pData->uchMajorFSCData + UIE_FSC_BIAS == FSC_KEYED_PLU) {
        if ( 0 == _tcslen(pData->PluNo.aszPLUNumber) ) {
            _tcscpy(aszPLUNumber, _T("0"));
        }
        else {
            _tcscpy(aszPLUNumber, pData->PluNo.aszPLUNumber);
        }

        /* Check Print Control */
        if ( pData->PluNo.uchModStat & MOD_STAT_EVERSION ) {      /* E-Version Code */
            aszStatus[0] = _T('E');
        }
#if 0
        else if ( pData->PluNo.uchModStat & MOD_STAT_VELOCITY ) { /* Velocity Code */
            aszStatus[0] = _T('V');
        }
#endif
        else {
            aszStatus[0] = _T(' ');
        }

        /* check print control */
        if (pData->usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */
			const TCHAR FARCONST *auchPrtThrmSupFSCPLU = _T("  P%2u %10u / %3u-%s %1s");

            /* print P PAGE-NUMBER ADDRESS / FSC DATA-EXTENDED FSC CODE */
            PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupFSCPLU, ( USHORT)(pData->uchPageNo), ( USHORT)(pData->uchAddress), ( USHORT)(pData->uchMajorFSCData), aszPLUNumber, aszStatus);
        } 
    
        if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */
			const TCHAR FARCONST *auchPrtSupFSCPLUA = _T("P%2u %4u / %3u-%s %1s");
			const TCHAR FARCONST *auchPrtSupFSCPLUB = _T("P%2u %4u / %3u-\n\t%s %1s");

            /* print P PAGE-NUMBER ADDRESS / FSC DATA-EXTENDED FSC CODE */
            if ((_tcslen(aszPLUNumber)+17) > (PRT_EJCOLUMN -1)) {
                PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupFSCPLUB, ( USHORT)(pData->uchPageNo), ( USHORT)(pData->uchAddress), ( USHORT)(pData->uchMajorFSCData), aszPLUNumber, aszStatus);
            } else {
                PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupFSCPLUA, ( USHORT)(pData->uchPageNo), ( USHORT)(pData->uchAddress), ( USHORT)(pData->uchMajorFSCData), aszPLUNumber, aszStatus);
            }
        }
    } else {
		const TCHAR FARCONST *auchPrtThrmSupFSC  = _T("  P%2u %10u / %3u-%3u");  /* define thermal print format */
		const TCHAR FARCONST *auchPrtSupFSC      = _T("P%2u %4u / %3u-%3u");     /* define EJ print format */

        usMinorFSCData = (UCHAR)pData->uchMinorFSCData;

        if (pData->uchMajorFSCData + UIE_FSC_BIAS == FSC_KEYED_DEPT) {
            usMinorFSCData = pData->usDeptNo;
        } else {
            usMinorFSCData = (UCHAR)pData->uchMinorFSCData;
        }

        /* check print control */
        if (pData->usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */
            /* print P PAGE-NUMBER ADDRESS / FSC DATA-EXTENDED FSC CODE */
            PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupFSC, ( USHORT)(pData->uchPageNo), ( USHORT)(pData->uchAddress), ( USHORT)(pData->uchMajorFSCData), usMinorFSCData);
        } 
    
        if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */
            /* print P PAGE-NUMBER ADDRESS / FSC DATA-EXTENDED FSC CODE */
            PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupFSC, ( USHORT)(pData->uchPageNo), ( USHORT)(pData->uchAddress), ( USHORT)(pData->uchMajorFSCData), usMinorFSCData);                      /* EXTENDED FSC code */
        }
    }
}
/***** End of Source *****/
