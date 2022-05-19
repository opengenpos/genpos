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
* Title       : Thermal Pirnt Hourly Activity Report Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSHORPT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               PrtThrmSupHourlyAct() : forms Hourly Activity Report format        
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-16-93: 01.00.12 : J.IKEDA   : Initial                                   
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
/* #include <cswai.h> */
#include <maint.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <csttl.h>
#include <csstbpar.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>

#include "prtcom.h"
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtThrmSupHourlyAct( RPTHOURLY *pData )  
*               
*     Input:    *pData  : pointer to structure for RPTHOURLY 
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This function forms TIME SEGMENT         
*                                   ILLEGAL SALE/PERSON  formats
*               for Hourly Activity Report.
*
*               The other print formats for Hourly Activity Report
*               are same as REGISTER FINANCIAL REPORT.
*
*               : HOURLY ACTIVITY REPORT
*
*===========================================================================
*/

VOID  PrtThrmSupHourlyAct( RPTHOURLY *pData )  
{
    /* define thermal print format */

    USHORT usHour;

    /* check print control */
    if (pData->usPrintControl & PRT_RECEIPT) {
        /* check minor class */
        switch(pData->uchMinorClass) {  
        case CLASS_RPTHOURLY_PRTTIME:
            if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT3)) {           /* military format */
				static const TCHAR FARCONST auchPrtThrmSupHourlyAct1[] = _T("\t%2u:%02u");

                /* print TIME(MILITARY) */
                PrtPrintf(PMG_PRT_RECEIPT,              /* printer type */
                          auchPrtThrmSupHourlyAct1,     /* format */
                          pData->StartTime.usHour,      /* hour */
                          pData->StartTime.usMin);      /* minutes */
            } else {                                    /* time is AM/PM */
				static const TCHAR FARCONST auchPrtThrmSupHourlyAct2[] = _T("\t%2u:%02u%s");

                /* check if Hour is '0' */
                if (pData->StartTime.usHour == 0) {     /* in case of "0:00 AM" */
                    usHour = 12;
                } else {
                    usHour = pData->StartTime.usHour;
                }
       
                /* print TIME(AM/PM) */
                PrtPrintf(PMG_PRT_RECEIPT,              /* printer type */
                          auchPrtThrmSupHourlyAct2,     /* format */
                          (usHour > 12)? usHour - 12 : usHour, /* hour */
                          pData->StartTime.usMin,                 /* minutes */
                          (pData->StartTime.usHour >= 12)? aszPrtPM : aszPrtAM);  /* AM/PM */
            }
            break;
       
        case CLASS_RPTHOURLY_PERCENT:
            if (pData->Total.lAmount == RPT_OVERFLOW) {
				static const TCHAR FARCONST auchPrtThrmSupHourlyAct5[] = _T("%-8.8s\t*****%%");

                /* print illegal Labor Cost % */
                PrtPrintf(PMG_PRT_RECEIPT,              /* printer type */
                          auchPrtThrmSupHourlyAct5,     /* format */
                          pData->aszTransMnemo);        /* transaction mnemonics */
            } else {
				static const TCHAR FARCONST auchPrtThrmSupHourlyAct4[] = _T("%-8.8s\t%.2l$%%");

                PrtPrintf(PMG_PRT_RECEIPT,              /* printer type */
                          auchPrtThrmSupHourlyAct4,     /* format */
                          pData->aszTransMnemo,         /* transaction mnemonics */
                          pData->Total.lAmount);         /* labor Cost % */
            }
            break;

        default:                                    /* CLASS_RPTHOURLY_PRTASTER */
			{
				static const TCHAR FARCONST auchPrtThrmSupHourlyAct3[] = _T("%-8.8s\t**********");

				/* print illegal SALE/PERSON */
				PrtPrintf(PMG_PRT_RECEIPT,              /* printer type */
						  auchPrtThrmSupHourlyAct3,     /* format */
						  pData->aszTransMnemo);        /* transaction mnemonics */
			}
            break;
        }
    }      

    /* check print control */
    if (pData->usPrintControl & PRT_JOURNAL) {    /* EJ */
        return;
    }
}
/***** End of Source *****/
