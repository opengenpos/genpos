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
* Title       : Set TOD Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATTOD.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
*                                           
*              MaintTODInit()   : Initialize Own Control Save Area  
*              MaintTODEdit()   : Edit TOD Data
*              MaintTODWrite()  : Set TOD Data   
*
* Notice  : This Module Supports 1993 - 2092 TOD Caluculation.
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jul-01-92: 00.00.01 : K.You     : initial 
* Mar-15-93: 00.00.02 : M.Ozawa   : Add Day of Week Caluculaton (for GP)
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
#include "pif.h"
#include "log.h"
#include "appllog.h"
#include "paraequ.h" 
#include "para.h"
#include "maint.h" 
#include "regstrct.h"
#include "display.h" 
#include "transact.h"
#include "prt.h"
#include "rfl.h"
#include "csstbpar.h"

#include "maintram.h" 

/*
==============================================================================
;                      L O C A L   D E F I N I T I O N
;=============================================================================
*/

#define  YEAR_BASE 1993              /* Day of Week Calculation Base */
#define  WEEK_BASE TOD_WEEK_FRI      /* 01/01/1993 (next year of leap year)*/

/*
==============================================================================
;                      C O M M O N   R A M   A R E A                         
;=============================================================================
*/

static PARATOD     MaintParaTOD;                    /* TOD Data Save Area */
static UCHAR       uchMaintTODControl;              /* TOD Control Save Area */
                                                    /* TOD_DATE_ADR : Only Set Date Data */
                                                    /* TOD_TIME_ADR : Only Set Time Data */
                                                    /* TOD_DATE_ADR | TOD_TIME_ADR : Set Both Data */

static ULONG  MainTODuAmount(PARATOD *pMainTod)
{
	ULONG       ulAmount = 0;
	switch (pMainTod->uchAddress) {
	case TOD_DATE_ADR:
		if (pMainTod->usMDay == 0) {
			// initialize the Maintenance Time and Date struct to the current
			// Date and Time. The only time that the Month Day member of this struct
			// should be zero is when this is the first time this function is called
			// since this is a global used to contain the date and time whan doing
			// an AC17 to set the date and time and the user entered data is kept.
			DATE_TIME   DateTime = { 0 };

			PifGetDateTime(&DateTime);

			pMainTod->usMDay = DateTime.usMDay;
			pMainTod->usMonth = DateTime.usMonth;
			pMainTod->usYear = DateTime.usYear;
			pMainTod->usHour = DateTime.usHour;
			pMainTod->usMin = DateTime.usMin;
		}

		if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {           /* DD/MM/YY Case */
			ulAmount = pMainTod->usMDay * 10000L + pMainTod->usMonth * 100L + (pMainTod->usYear % 100L);
		}
		else {                                                        /* MM/DD/YY Case */
			ulAmount = pMainTod->usMonth * 10000L + pMainTod->usMDay * 100L + (pMainTod->usYear % 100L);
		}
		break;
	case TOD_TIME_ADR:
		ulAmount = pMainTod->usHour * 100L + pMainTod->usMin;
		break;
	}

	return ulAmount;
}

/*
*===========================================================================
** Synopsis:    VOID MaintTODInit( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Initialize Own Control Save Area.
*===========================================================================
*/
VOID MaintTODInit( VOID )
{
	MAINTDSPCTL     MaintDspCtl = { 0 };

    /* Initialize Own Control Area and Save Area */
    memset(&MaintParaTOD, 0, sizeof(PARATOD));
    MaintParaTOD.uchAddress = 1;

	uchMaintTODControl = 0;

    /* Display Address 1 of This Function */
	MaintDspCtl.uchMajorClass = CLASS_MAINTDSPCTL;          /* Set Major Calss */
    MaintDspCtl.usPGACNumber = AC_TOD;                      /* Set AC Number */
    MaintDspCtl.uchRptType = MaintParaTOD.uchAddress;       /* Set Address */
	RflGetLead(MaintDspCtl.aszMnemonics, LDT_DATA_ADR);     /* Set Lead Through Mnemonics */

	MaintDspCtl.ulAmount = MainTODuAmount(&MaintParaTOD);

	DispWrite(&MaintDspCtl);                                /* Display. See function DispSupControl20() */
}

/*
*===========================================================================
** Synopsis:    SHORT MaintTODEdit( MAINTTOD *pData )
*               
*     Input:   *pData             : Pointer to Structure for MAINTTOD
*    Output:    Nothing 
*
** Return:      SUCCESS            : Successful
*               LDT_KEYOVER_ADR    : Wrong Data Error
*
** Description: This Function Edits TOD Data.
*===========================================================================
*/
SHORT MaintTODEdit( MAINTTOD *pData )
{
    UCHAR           uchAddress;
    USHORT          usMin;
    USHORT          usHour;
    USHORT          usMDay;
    USHORT          usMonth;
    USHORT          usYear;
	MAINTDSPCTL     MaintDspCtl = { 0 };

    switch (pData->uchMinorClass) {                             
    case CLASS_MAINTTOD_READ:                                   
        /* Check W/ Amount */
        if (pData->uchStatus & MAINT_WITHOUT_DATA) {            /* W/o Amount */
            uchAddress = ++MaintParaTOD.uchAddress;

            if (uchAddress > TOD_ADR_MAX) {                     /* Over Limit */
                uchAddress = 1;                                 /* Initialize Address */
            }
        } else {                                                /* W/ Amount */
            uchAddress = pData->uchAddress;
        }

        /* Check Address */
        if (uchAddress < 1 || TOD_ADR_MAX < uchAddress) {
            return(LDT_KEYOVER_ADR);                                
        }
        MaintParaTOD.uchAddress = uchAddress;
        break;

    case CLASS_MAINTTOD_WRITE:                                   
        switch (MaintParaTOD.uchAddress) {
        case TOD_DATE_ADR:

            /* Check Digit */
            if (pData->uchStrLen != 6 && pData->uchStrLen != 5 ) {                                           /* Not 6 Digits */
                return(LDT_KEYOVER_ADR);
            }

            /* Check Date/Time System */
            if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {           /* DD/MM/YY Case */
                usMDay = ( USHORT)(pData->lTODData/10000L);
                usMonth = ( USHORT)((pData->lTODData - 10000L*( LONG)usMDay)/100L);
                usYear = ( USHORT)(pData->lTODData - 10000L*( LONG)usMDay - 100L*( LONG)usMonth);
			} else {                                                        /* MM/DD/YY Case */
                usMonth = ( USHORT)(pData->lTODData/10000L);
                usMDay = ( USHORT)((pData->lTODData - 10000L*( LONG)usMonth)/100L);
                usYear = ( USHORT)(pData->lTODData - 10000L*( LONG)usMonth - 100L*( LONG)usMDay);
			}

            /* Check Range */
            if (99 < usYear) {
                return(LDT_KEYOVER_ADR);
            }
            if (usMonth < 1 || 12 < usMonth) {
                return(LDT_KEYOVER_ADR);
            }

            /* Check Number of Day by Each Month */
            if (MaintTODChkDay(usYear, usMonth, usMDay)) {
                return(LDT_KEYOVER_ADR);
            }

            /* Save Data and Set Control Flag */
            if (usYear >= 93) {
                usYear += 1900;
            } else {
                usYear += 2000;
            }

            MaintParaTOD.usWDay = MaintTODCalWeek(usYear, usMonth, usMDay);    /* Day of Week Caluculation */
            MaintParaTOD.usYear = usYear;
            MaintParaTOD.usMonth = usMonth;
            MaintParaTOD.usMDay = usMDay;
            uchMaintTODControl |= TOD_DATE_ADR;			
			break;

        case TOD_TIME_ADR:
            /* Check Digit */
            if (pData->uchStrLen > 4) {                                           /* Not 4 Digit */
                return(LDT_KEYOVER_ADR);
            }

            usHour = ( USHORT)(pData->lTODData / 100L);
            usMin = ( USHORT)((pData->lTODData) % 100L);

            /* Check Date/Time System and Range */
            if (23 < usHour) {
                return(LDT_KEYOVER_ADR);
            }
            if (59 < usMin) {
                return(LDT_KEYOVER_ADR);
            }

            /* Save Data and Set Control Flag */
            MaintParaTOD.usHour = usHour;
            MaintParaTOD.usMin = usMin;
            uchMaintTODControl |= TOD_TIME_ADR;
            break;

        /* default: 
            break; */

        }

        /* Advance Next Address */
        MaintParaTOD.uchAddress++;
        if (MaintParaTOD.uchAddress > TOD_ADR_MAX) {
            MaintParaTOD.uchAddress = 1;                        /* Initialize Address */    
        }                                                       
		break;

    default:
        PifAbort(MODULE_MAINT_LOG_ID, FAULT_INVALID_DATA);
    }

    /* Display Next Address of This Function */
    MaintDspCtl.uchMajorClass = CLASS_MAINTDSPCTL;          /* Set Major Calss */
    MaintDspCtl.usPGACNumber = AC_TOD;                      /* Set AC Number */
    MaintDspCtl.uchRptType = MaintParaTOD.uchAddress;       /* Set Address */
	RflGetLead(MaintDspCtl.aszMnemonics, LDT_DATA_ADR);     /* Set Lead Through Mnemonics */
	MaintDspCtl.ulAmount = MainTODuAmount(&MaintParaTOD);

    DispWrite(&MaintDspCtl);                                /* Display. See function DispSupControl20() */

    return(SUCCESS);                                            
}

/*
*===========================================================================
** Synopsis:    VOID MaintTODWrite( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Writes TOD Data to Pif Module.
*===========================================================================
*/
VOID MaintTODWrite( VOID )
{
	DATE_TIME   DateTime = { 0 };

    PifGetDateTime(&DateTime);

    /* Check Set Data Condition */
    switch (uchMaintTODControl) {
    case TOD_DATE_ADR:                          /* Set Date Data */
        DateTime.usMDay = MaintParaTOD.usMDay;
        DateTime.usMonth = MaintParaTOD.usMonth;
        DateTime.usYear = MaintParaTOD.usYear;
        DateTime.usWDay = MaintParaTOD.usWDay;

        MaintParaTOD.usMin = DateTime.usMin;   /* set for report */
        MaintParaTOD.usHour = DateTime.usHour;
        break;

    case TOD_TIME_ADR:                          /* Set Time Data */
        DateTime.usSec = 0;
        DateTime.usMin = MaintParaTOD.usMin;
        DateTime.usHour = MaintParaTOD.usHour;

        MaintParaTOD.usMDay = DateTime.usMDay;  /* set for report */
        MaintParaTOD.usMonth= DateTime.usMonth;
        MaintParaTOD.usYear = DateTime.usYear;
        MaintParaTOD.usWDay = DateTime.usWDay;
        break;

    case TOD_DATE_ADR | TOD_TIME_ADR:           /* Set Date/Time Data */
        DateTime.usSec = 0;
        DateTime.usMin = MaintParaTOD.usMin;
        DateTime.usHour = MaintParaTOD.usHour;
        DateTime.usMDay = MaintParaTOD.usMDay;
        DateTime.usMonth = MaintParaTOD.usMonth;
        DateTime.usYear = MaintParaTOD.usYear;
        DateTime.usWDay = MaintParaTOD.usWDay;
        break;
        
    default:                                    /* set current date for report */
        MaintParaTOD.usMin = DateTime.usMin;
        MaintParaTOD.usHour = DateTime.usHour;
        MaintParaTOD.usMDay = DateTime.usMDay;
        MaintParaTOD.usMonth= DateTime.usMonth;
        MaintParaTOD.usYear = DateTime.usYear;
        MaintParaTOD.usWDay = DateTime.usWDay;
        break;
    }

    /* Clear Own Control Area */
    uchMaintTODControl = 0;

    /* Set Date/Time Date to Pif Module */
    PifSetDateTime(&DateTime);
    
    /* Print Header and Trailer */
    MaintHeaderCtl(AC_TOD, RPT_ACT_ADR);

    MaintParaTOD.uchMajorClass = CLASS_MAINTTOD;
    MaintParaTOD.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintParaTOD));

    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
}
    
/*
*===========================================================================
** Synopsis:    SHORT MaintTODChkDay( USHORT usYear, USHORT usMonth, USHORT usMDay)
*               
*     Input:    usYear          : Year Data
*               usMonth         : Month Data
*               usMDay          : Day Data 
*    Output:    Nothing 
*
** Return:      SUCCESS            : Successful
*               LDT_KEYOVER_ADR    : Wrong Data Error
*
** Description: This Function Check Number of Day by Each Month.
*===========================================================================
*/
SHORT MaintTODChkDay( USHORT usYear, USHORT usMonth, USHORT usMDay)
{
    if (usMDay < 1) {
        return(LDT_KEYOVER_ADR);
    }

    switch (usMonth) {
    case 2:
        if (usYear%4) {                             /* Not Leap Year */ 
            if(usMDay > 28) {
                return(LDT_KEYOVER_ADR);
            }
        } else {                                    /* Leap Year */
            if(usMDay > 29) {
                return(LDT_KEYOVER_ADR);
            }
        }
        break;

    case 4:
    case 6:
    case 9:
    case 11:
        if (usMDay > 30) {
            return(LDT_KEYOVER_ADR);
        }
        break;

    default:
        if (usMDay > 31) {
            return(LDT_KEYOVER_ADR);
        }
        break;
    }

    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintTODCalWeek( USHORT usYear, USHORT usMonth, USHORT usMDay)
*               
*     Input:    usYear          : Year Data
*               usMonth         : Month Data
*               usMDay          : Day Data 
*    Output:    Nothing
*
** Return:      Day of Week
*               TOD_WEEK_SUN       : Sunday
*               TOD_WEEK_MON       : Monday
*               TOD_WEEK_TUS       : Tuesday
*               TOD_WEEK_WED       : Wednesday
*               TOD_WEEK_THR       : Thursday
*               TOD_WEEK_FRI       : Friday
*               TOD_WEEK_SAT       : Saturday
*               LDT_KEYOVER_ADR    : Wrong Data Error
*
** Description: This Function Caluculates Day of the Week from Date and Year.
*===========================================================================
*/
SHORT MaintTODCalWeek( USHORT usYear, USHORT usMonth, USHORT usMDay)
{
    USHORT usYears;         /* years from next year of leap(1993) */
    USHORT usLeapYears;     /* years of leap year */
    USHORT usDays;          /* days from Jan. 1'st */

    /* set "day of week" of Jan. 1'st of the year */
    usYears = usYear - YEAR_BASE;
    usLeapYears = usYears/4;
    usDays = WEEK_BASE + usYears + usLeapYears;
    
    /* caluculate days from Jan. 1'st */
    switch(usMonth){
    case 1:
        break;

    case 2:
        usDays += 31;
        break;

    case 3:
        usDays += 59;       /* 31+28 */
        break;

    case 4:
        usDays += 90;       /* 31+28+31 */
        break;

    case 5:
        usDays += 120;      /* 31+28+31+30 */
        break;

    case 6:
        usDays += 151;      /* 31+28+31+30+31 */
        break;

    case 7:
        usDays += 181;      /* 31+28+31+30+31+30 */
        break;

    case 8:
        usDays += 212;      /* 31+28+31+30+31+30+31 */
        break;

    case 9:
        usDays += 243;      /* 31+28+31+30+31+30+31+31 */
        break;

    case 10:
        usDays += 273;      /* 31+28+31+30+31+30+31+31+30 */
        break;

    case 11:
        usDays += 304;      /* 31+28+31+30+31+30+31+31+30+31 */
        break;

    case 12:
        usDays += 334;      /* 31+28+31+30+31+30+31+31+30+31+30 */
        break;

    }

    if ((usYear%4) == 0) {       /* Leap Year */ 
        if(usMonth > 2){
            usDays++;        /* add Feb. 29th */
        }
    }

    usDays += usMDay;

    /* caluculate "day of week" of the day */ 
    return((usDays-1) % 7);

}
