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
* Title       : Print FSC Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSFSC_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: This function forms FSC FOR REGULAR KEYBOARD print format.
*
*           The provided function names are as follows: 
* 
*               PrtSupFSC() : form FSC print format
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-02-92: 00.00.01 : J.Ikeda   : initial                                   
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
#include <stdlib.h>
#include <ecr.h>
/* #include <pif.h> */
#include <uie.h>
#include <paraequ.h> 
#include <para.h>
#include <maint.h> 
#include <fsc.h>
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>
#include "prtsin.h"
#include "prrcolm_.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtSupFSC( PARAFSC *pData )
*               
*     Input:    *pData      : Pointer to Structure for PARAFSC
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms FSC print format.
*===========================================================================
*/

VOID  PrtSupFSC( PARAFSC *pData )
{

    static const TCHAR FARCONST auchPrtSupFSC[]    = _T("P%2u %5u / %3u-%4u");      /* Define Print Format */
    static const TCHAR FARCONST auchPrtSupFSCPLUA[] = _T("P%2u %4u / %3u-%s %1s");
    static const TCHAR FARCONST auchPrtSupFSCPLUB[] = _T("P%2u %4u / %3u-\n\t%s %1s");

    USHORT usPrtType;
    USHORT usMinorFSCData;

    TCHAR   aszPLUNumber[PLU_MAX_DIGIT+1];
    TCHAR   aszStatus[2] = { 0, 0 };

    /* Check Print Control */
    
    usPrtType = PrtSupChkType(pData->usPrintControl);

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

        if ((_tcslen(aszPLUNumber)+17) > (PRT_RJCOLUMN -1)) {

            PmgPrintf(usPrtType,                          /* Printer Type */
                          auchPrtSupFSCPLUB,                        /* format */
                          ( USHORT)(pData->uchPageNo),          /* Page Number */
                          ( USHORT)(pData->uchAddress),         /* address */
                          ( USHORT)(pData->uchMajorFSCData),    /* FSC data */
                          aszPLUNumber,                         /* PLU Number */
                          aszStatus);                           /* E-Version */
        } else {

            PmgPrintf(usPrtType,                          /* Printer Type */
                          auchPrtSupFSCPLUA,                        /* format */
                          ( USHORT)(pData->uchPageNo),          /* Page Number */
                          ( USHORT)(pData->uchAddress),         /* address */
                          ( USHORT)(pData->uchMajorFSCData),    /* FSC data */
                          aszPLUNumber,                         /* PLU Number */
                          aszStatus);                           /* E-Version */
        }
    } else {
    
        usMinorFSCData = (UCHAR)pData->uchMinorFSCData;

        if (pData->uchMajorFSCData + UIE_FSC_BIAS == FSC_KEYED_DEPT) {
            usMinorFSCData = pData->usDeptNo;
        } else {
            usMinorFSCData = (UCHAR)pData->uchMinorFSCData;
        }

        /* Print Line */

        PmgPrintf(usPrtType,                          /* Printer Type */
                  auchPrtSupFSC,                      /* Format */
                  ( USHORT)(pData->uchPageNo),        /* Page */
                  ( USHORT)(pData->uchAddress),       /* Address */
                  ( USHORT)(pData->uchMajorFSCData),  /* Major FSC Data */
                  usMinorFSCData);                    /* Minor FSC Data */
    }

}
