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
* Title       : Hourly Activity Report Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSHORP_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               PrtSupHourlyAct()   : This Function Forms Hourly Activity Report Format.        
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-20-92: 00.00.01 : K.You     : initial                                   
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
** Synopsis:    VOID  PrtSupHourlyAct( RPTHOURLY *pData )  
*               
*     Input:    *pData  : Pointer to Structure for RPTHOURLY 
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Forms Hourly Activity Report Format.
*===========================================================================
*/

VOID  PrtSupHourlyAct( RPTHOURLY *pData )  
{
    
    static const TCHAR FARCONST auchPrtSupHourlyAct1[] = _T("\t%2u:%02u");
    static const TCHAR FARCONST auchPrtSupHourlyAct2[] = _T("\t%2u:%02u%s");
    static const TCHAR FARCONST auchPrtSupHourlyAct3[] = _T("%-8.8s\t**********");/*8 characters JHHJ*/
    static const TCHAR FARCONST auchPrtSupHourlyAct4[] = _T("%-8.8s\t%.2l$%%");/*8 characters JHHJ*/
    static const TCHAR FARCONST auchPrtSupHourlyAct5[] = _T("%-8.8s\t*****%%");/*8 characters JHHJ*/

    USHORT usPrtType;
    USHORT usHour;


    /* Check Print Control */

    usPrtType = PrtSupChkType(pData->usPrintControl);  

    /* check minor class */

    switch(pData->uchMinorClass) {
    case CLASS_RPTHOURLY_PRTTIME:
        if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT3)) {           /* Military Format */

            PmgPrintf(usPrtType,                    /* Printer */
                      auchPrtSupHourlyAct1,         /* Format */
                      pData->StartTime.usHour,      /* Hour */
                      pData->StartTime.usMin);      /* Minutes */

        } else {                                    /* Time is AM/PM */

            /* check if Hour is '0' */

            if (pData->StartTime.usHour == 0) {     /* in case of "0:00 AM" */
                usHour = 12;
            } else {
                usHour = pData->StartTime.usHour;
            }

            PmgPrintf(usPrtType,                    /* Printer */
                      auchPrtSupHourlyAct2,         /* Format */
                      (usHour > 12)? usHour - 12 : usHour, /* hour */
                      pData->StartTime.usMin,                 /* Minutes */
                      (pData->StartTime.usHour >= 12)? aszPrtPM : aszPrtAM);  /* AM/PM */
                      
        }
        break;

        case CLASS_RPTHOURLY_PERCENT:

            if (pData->Total.lAmount == RPT_OVERFLOW) {

                /* print illegal Labor Cost % */
       
                PmgPrintf(usPrtType,                    /* Printer */
                          auchPrtSupHourlyAct5,         /* format */
                          pData->aszTransMnemo);        /* transaction mnemonics */

            } else {

                PmgPrintf(usPrtType,                    /* Printer */
                          auchPrtSupHourlyAct4,         /* format */
                          pData->aszTransMnemo,         /* transaction mnemonics */
                          pData->Total.lAmount);         /* labor Cost % */
            }
            break;

    default:                                    /* CLASS_RPTHOURLY_PRTASTER */
        PmgPrintf(usPrtType,                    /* Printer Type */
                  auchPrtSupHourlyAct3,         /* Format */
                  pData->aszTransMnemo);        /* Transaction Mnemonics */

        break;
    }
}

