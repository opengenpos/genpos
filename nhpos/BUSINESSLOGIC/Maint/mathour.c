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
* Title       : Maint Hourly Time Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATHOUR.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function read & write & report DISCOUNT RATE & BONUS RATE.
*
*           The provided function names are as follows: 
*                                       
*              MaintHourlyTimeRead()    : Read & Display Hourly Time Block 
*              MaintHourlyTimeWrite()   : Set & Print Hourly Time Block
*              MaintHourlyTimeReport()  : Report Hourly Time Block   
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

#include <ecr.h>
#include <uie.h>
#include <paraequ.h> 
#include <para.h>
#include <maint.h> 
#include <display.h> 
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <csstbpar.h>

#include "maintram.h" 

/*
*===========================================================================
** Synopsis:    SHORT MaintHourlyTimeRead( MAINTHOURLYTIME *pData )
*               
*     Input:    *pData          : pointer to structure for MAINTHOURLYTIME
*    Output:    Nothing 
*
** Return:      SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*
** Description: This Function Reads & Displays Hourly Time Block.
*===========================================================================
*/

SHORT MaintHourlyTimeRead( MAINTHOURLYTIME *pData )
{

    UCHAR uchAddress;


    /* Check Status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* W/o Amount */
        uchAddress = ++MaintWork.HourlyTime.uchAddress;         

        /* Check Address */

        if (uchAddress > HOUR_ADR_MAX) { 
            uchAddress = 1;                                     /* Initialize Address */
        } 
    } else {                                                    /* W/ Amount */
        uchAddress = pData->uchAddress;
    }

    /* Check Address */

    if (uchAddress < 1 || HOUR_ADR_MAX < uchAddress) {
        return(LDT_KEYOVER_ADR);
    }
    MaintWork.HourlyTime.uchMajorClass = pData->uchMajorClass;  /* Copy Major Class */
    MaintWork.HourlyTime.uchAddress = uchAddress;
    CliParaRead(&(MaintWork.HourlyTime));

    /* Set Minor Class */

    if (uchAddress == HOUR_ACTBLK_ADR ) {
        MaintWork.HourlyTime.uchMinorClass = CLASS_PARAHOURLYTIME_BLOCK;
    } else {
        MaintWork.HourlyTime.uchMinorClass = CLASS_PARAHOURLYTIME_TIME;
    }

    DispWrite(&(MaintWork.HourlyTime));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintHourlyTimeWrite( MAINTHOURLYTIME *pData )
*               
*     Input:    *pData             : Pointer to Structure for MAINTHOURLYTIME
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS            : Successful
*               LDT_SEQERR_ADR     : Sequence Error
*               LDT_KEYOVER_ADR    : Wrong Data Error
*
** Description: This Function Sets & Prints Hourly Time Block.
*===========================================================================
*/

SHORT MaintHourlyTimeWrite( MAINTHOURLYTIME *pData )
{

    UCHAR           uchHour;
    UCHAR           uchMin;
    MAINTHOURLYTIME MaintHourly;


    /* Check Status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* W/o Amount */
        return(LDT_SEQERR_ADR);                                 
    }

    /* Distinguish Address */

    switch (MaintWork.HourlyTime.uchAddress) {
    case HOUR_ACTBLK_ADR:

        /* Check Range */

        if (MAINT_HOURBLK_MAX < pData->usHourTimeData) {        /* Out of Range */ 
            return(LDT_KEYOVER_ADR);                                                        
        }

        MaintWork.HourlyTime.uchBlockNo = ( UCHAR)pData->usHourTimeData;
        CliParaWrite(&(MaintWork.HourlyTime));                         
        break;

    default:
        uchHour = ( UCHAR)((pData->usHourTimeData)/100);
        uchMin = ( UCHAR)((pData->usHourTimeData)%100);

        /* Check Range */

        if (23 < uchHour) {                     /* Out of Range */ 
            return(LDT_KEYOVER_ADR);                                                 
        }
        if (59 < uchMin) {                      /* Out of Range */ 
            return(LDT_KEYOVER_ADR);                                         
        }

        MaintWork.HourlyTime.uchHour = uchHour;
        MaintWork.HourlyTime.uchMin = uchMin;
        CliParaWrite(&(MaintWork.HourlyTime));                         
        break;

    }

    /* Control Header Item */
        
    MaintHeaderCtl(PG_HOURLY_TIME, RPT_PRG_ADR);

    /* Set Journal Bit & Receipt Bit */

    MaintWork.HourlyTime.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.HourlyTime));
    
    /* Advanced Next Address */

    MaintWork.HourlyTime.uchAddress++;

    if (MaintWork.HourlyTime.uchAddress > HOUR_ADR_MAX) { 
        MaintWork.HourlyTime.uchAddress = 1;                                     /* Initialize Address */
    }

    /* Display Next Address Item */

    MaintHourly.uchMajorClass = CLASS_PARAHOURLYTIME;
    MaintHourly.uchAddress = MaintWork.HourlyTime.uchAddress;
    MaintHourly.uchStatus = 0;
    MaintHourlyTimeRead(&MaintHourly);
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintHourlyTimeReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*
** Return:      Nothing 
*
** Description: This Function Prints All Hourly Time Block.
*===========================================================================
*/

VOID MaintHourlyTimeReport( VOID )
{


    UCHAR               i;
    PARAHOURLYTIME      ParaHourlyTime;


    /* Control Header Item */
        
    MaintHeaderCtl(PG_HOURLY_TIME, RPT_PRG_ADR);
    ParaHourlyTime.uchMajorClass = CLASS_PARAHOURLYTIME;

    /* Set Journal Bit & Receipt Bit */

    ParaHourlyTime.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* Print Data for Every Address */

    for (i = 1; i <= HOUR_ADR_MAX; i++) {

        /* Check Abort Key */

        if (UieReadAbortKey() == UIE_ENABLE) {                  /* Depress Abort Key */
            MaintMakeAbortKey();                                
            break;
        }
        ParaHourlyTime.uchAddress = i;

        /* Set Minor Class */

        if (i == HOUR_ACTBLK_ADR ) {
            ParaHourlyTime.uchMinorClass = CLASS_PARAHOURLYTIME_BLOCK;
        } else {
            ParaHourlyTime.uchMinorClass = CLASS_PARAHOURLYTIME_TIME;
        }
        CliParaRead(&ParaHourlyTime);                              
        PrtPrintItem(NULL, &ParaHourlyTime);
    }

    /* Make Trailer */
    
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTPRG);

}

    
    
    
    
    
