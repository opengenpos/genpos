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
*==========================================================================
* Title       : Hourly Activity Time Module( SUPER & PROGRAM MODE )                       
* Category    : Display-2x20, NCR 2170 US Hospitality Application Program        
* Program Name: DSPHUR20.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*               
*               DispSupHourlyTime20()       : Display Hourly Activity Time
*                                                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-22-92: 00.00.01 : K.You     : initial                                   
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

#include "ecr.h"
#include "uie.h"
/* #include <pif.h> */
/* #include <log.h> */
#include "paraequ.h"
#include "para.h"
#include "maint.h"
/* #include <appllog.h> */
#include "display.h"
#include "disp20.h"


/*
*===========================================================================
** Synopsis:    VOID DispSupHourlyTime20( PARAHOURLYTIME *pData )
*               
*     Input:    *pData          : Pointer to Structure for PARAHOURLYTIME
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Displays Hourly Activity Time
*===========================================================================
*/

VOID DispSupHourlyTime20( PARAHOURLYTIME *pData )
{
    /* Define Display Format */ 
    static const TCHAR  auchDispSupHourlyTime20A[] = _T("%3u  %3u\t%2u\n\t%s");
    static const TCHAR  auchDispSupHourlyTime20B1[] = _T("%3u  %3u\t%1u\n\t%s");
    static const TCHAR  auchDispSupHourlyTime20B2[] = _T("%3u  %3u\t%2u%02u\n\t%s");
    static const TCHAR  auchDispSupHourlyTime20C[] = _T("%3u  %3u\t%10s\n\t%s");

    TCHAR   aszDspNull[1] = {0};

    DispSupSaveData.HourlyTime = *pData;      /* Save Data for Redisplay if needed */

    /* Distingush Minor Class */
    switch (pData->uchMinorClass) {
    case CLASS_PARAHOURLYTIME_BLOCK:
        /* Display ForeGround */
        UiePrintf(UIE_OPER,                         /* Operator Display */
                  0,                                /* Row */
                  0,                                /* Column */
                  auchDispSupHourlyTime20A,         /* Format */
                  PG_HOURLY_TIME,                   /* Program Number */
                  ( USHORT)pData->uchAddress,       /* Address */
                  ( USHORT)pData->uchBlockNo,       /* Block Number */
                 aszDspNull);                      /* Null */
        break;

    case CLASS_PARAHOURLYTIME_TIME:
        /* Display ForeGround */
        if (pData->uchHour == 0 && pData->uchMin == 0) {
            UiePrintf(UIE_OPER,                         /* Operator Display */
                      0,                                /* Row */
                      0,                                /* Column */
                      auchDispSupHourlyTime20B1,        /* Format */
                      PG_HOURLY_TIME,                   /* Program Number */
                      ( USHORT)pData->uchAddress,       /* Address */
                      0,                                /* Zero */
                      aszDspNull);                      /* Null */
        } else {
            UiePrintf(UIE_OPER,                         /* Operator Display */
                      0,                                /* Row */
                      0,                                /* Column */
                      auchDispSupHourlyTime20B2,        /* Format */
                      PG_HOURLY_TIME,                   /* Program Number */
                      ( USHORT)pData->uchAddress,       /* Address */
                      ( USHORT)pData->uchHour,          /* Hour */
                      ( USHORT)pData->uchMin,           /* Minute */
                      aszDspNull);                      /* Null */
        }
        break;

    default:
/*        PifAbort(MODULE_DISPSUP_ID, FAULT_INVALID_DATA); */
        break;
    }


    /* Display BackGround */
    UieEchoBackGround(UIE_KEEP,                     /* Descriptor Control */
                      UIE_ATTR_NORMAL,              /* Attribute */
                      auchDispSupHourlyTime20C,     /* Format */
                      PG_HOURLY_TIME,               /* Program Number */
                      ( USHORT)pData->uchAddress,   /* Address */
                      aszDspNull,                   /* Null */
                      aszDspNull);                  /* Null */    
}
