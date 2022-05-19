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
* Title       : Hourly Activity Report Module                         
* Category    : Report For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: RPTHOUR.C
* --------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
*       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
*       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
*       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
*       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
*       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               RptHourlyDaySet()       : Set Range Time Date  
*               RptHourlyDayRead()      : Read Daily Hourly Activity File
*               RptHourlyPTDRead()      : Read PTD Hourly Activity File
*               RptHourlyAllRead()
*               RptHourlyAllEdit()
*               RptHourlyRngEdit()
*               RptHourlyTtlEdit()
*               RptHourlyElement()
*               RptHourlyCalSalePerson() 
*               RptGetHourlyIndex()
*               RptHourlyAddTotal()
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jul-11-92: 00.00.01 :  K.You    : initial                                   
* Dec-07-92: 01.00.00 :  K.You    : Del <rfl.h>                                   
* Dec-17-92: 01.00.00 :  K.You    : Fix bug of RptGetHourlyIndex()                                   
* Jun-22-93: 01.00.12 : J.IKEDA   : change major class from CLASS_MAINTTRANS 
*          :          :           : to CLASS_RPTREGFIN in RptHourlyAllEdit() 
*          :          :           : & RptHourlyRngEdit()
* Jul-07-93: 01.00.12 : J.IKEDA   : Adds RptRound() in RptHourlyCalSalePerson()
* Oct-07-93: 02.00.01 : M.SUZUKI  : Move No. of Block Check Routine.
* Apr-18-96: 03.01.04 : M.Ozawa   : Enhanced to Labor Cost Calculation
* Aug-19-98: 03.03.00 : H.Iwata   : Add Bonus Data
** GenPOS
* Apr-19-15: 02.02.01 : R.Chambers : replace TTL_MAX_BLOCK with STD_MAX_HOURBLK in ecr.h
* Jul-07-17: 02.02.02 : R.Chambers : localized variables, removed unused variable usHrlyFilePosition
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
/*
==============================================================================
;                      I N C L U D E     F I L E s                         
;=============================================================================
*/

#include	<tchar.h>
#include <string.h>
#include <ecr.h>
#include <uie.h>
#include <pif.h>
#include <log.h>
#include <rfl.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <maint.h>
#include <csop.h>
#include <csstbopr.h>
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <csttl.h>
#include <csstbttl.h>
#include <report.h>
#include <appllog.h>
#include <mld.h>
#include <mldsup.h>
#include <plu.h>
#include <csttl.h>
#include <ttl.h>

#include "rptcom.h"

/*
==============================================================================
;                      C O M M O N   R A M   A R E A                         
;=============================================================================
*/

N_TIME  RptStartTime;                             /* Start Time Save Area of Range Report */
N_TIME  RptEndTime;                               /* End Time Save Area of Range Report */

/****        
SHORT sRptNoOfPersonTtl;                        / Total Save Area of No of Person /
LONG  lRptItemPCountTtl;                        / Total Save Area of Item Producutivity Counter /
LONG  lRptNetSalesTtl;                          / Total Save Area of Net Sales /
LONG  lRptAverageSalesTtl;                      / Total Save Area of Sales/Person /
****/

static VOID RptHourlyTtlEdit(LONG  lItemPCountTtl,
                      SHORT sSalePersonTtl,
                      DCURRENCY  lNetSalesTtl,
                      DCURRENCY  lBonus[],
                      DCURRENCY  lAverageSalesTtl,
                      UCHAR uchMinorClass,
                      LONG  lLaborTtl,
                      LONG  lPercent);

static DCURRENCY RptHourlyCalSalePerson(DCURRENCY lAmount, SHORT sPerson);
static UCHAR RptGetHourlyIndex(N_TIME *pRangeTime);
static VOID RptHourlyAddTotal(HOURLY *pAddTtl, HOURLY *pTtlData);
static BOOL RptHourlyChkTotal( HOURLY *pTotal, ULONG ulWageTotal);
static LONG RptHourlyPercent(DCURRENCY lTotal, DCURRENCY lCost);

/*
*=============================================================================
**  Synopsis: SHORT RptHourlyDaySet(RPTHOURLY *pData) 
*               
*       Input:  *pData      : Pointer to Structure for TTLHOURLY  
*      Output:  Nothing
*
**  Return: SUCCESS         : Successful  
*           LDT_KEYOVER_ADR : Wrong Data
*
**  Description: Set Time of Range Report.
*===============================================================================
*/

SHORT RptHourlyDaySet(RPTHOURLY *pData)
{


    /* Distingush Minor Class */

    switch (pData->uchMinorClass) {
    case CLASS_TTLHOURLY_STSTIME:                       /* Start Time of Range Report */
        /* Check Range */

        if (59 < pData->StartTime.usMin) {              /* Over Limit */
            return(LDT_KEYOVER_ADR);
        }
        if (23 < pData->StartTime.usHour) {             /* Over Limit */
            return(LDT_KEYOVER_ADR);
        }

        /* Save Time Data */

        RptStartTime.usMin = pData->StartTime.usMin;
        RptStartTime.usHour = pData->StartTime.usHour;
        break;

    case CLASS_TTLHOURLY_ENDTIME:                       /* End Time of Range Report */
        /* Check W/o Amount */

        if (pData->uchStatus & MAINT_WITHOUT_DATA) {    /* W/o Amount */
            RptEndTime = RptStartTime;                  /* Copy Start Time to End Time */
            return(SUCCESS);
        }

        /* Check Range */

        if (59 < pData->StartTime.usMin) {              /* Over Limit */
            return(LDT_KEYOVER_ADR);
        }
        if (23 < pData->StartTime.usHour) {             /* Over Limit */
            return(LDT_KEYOVER_ADR);
        }

        /* Save Time Data */

        RptEndTime.usMin = pData->StartTime.usMin;
        RptEndTime.usHour = pData->StartTime.usHour;
        break;

    default:
/*        PifAbort(MODULE_RPT_ID, FAULT_INVALID_DATA); */
        return(LDT_KEYOVER_ADR);
    }
    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT RptHourlyDayRead(UCHAR uchMinor, UCHAR uchType) 
*               
*       Input:  uchMinor    : CLASS_TTLCURDAY   : Current Daily File
*                           : CLASS_TTLSAVDAY   : Saved Daily File
*                           : CLASS_TTLCURPTD   : Current PTD File
*                           : CLASS_TTLSAVPTD   : Saved PTD File
*               uchType     : RPT_ALL_READ      : All Read Type
*                           : RPT_RANGE_READ    : Range Read Type
*                           : PRT_EOD_ALLREAD   : EOD Read Type
*      Output:  Nothing
*
**  Return: SUCCESS         : Successful  
*           LDT_NTINFL_ADR  : Not in File 
*
**  Description: Read Daily Hourly Activity Report.
*===============================================================================
*/

SHORT RptHourlyDayRead(UCHAR uchMinor, UCHAR uchType) 
{
    return(RptHourlyAllRead(uchMinor, uchType));
}

/*
*=============================================================================
**  Synopsis: SHORT RptHourlyPTDRead( UCHAR uchMinor, UCHAR uchType ) 
*               
*       Input:  uchMinor    : CLASS_TTLCURDAY   : Current Daily File
*                           : CLASS_TTLSAVDAY   : Saved Daily File
*                           : CLASS_TTLCURPTD   : Current PTD File
*                           : CLASS_TTLSAVPTD   : Saved PTD File
*               uchType     : RPT_ALL_READ      : All Read Type
*                           : RPT_RANGE_READ    : Range Read Type
*                           : PRT_PTD_ALLREAD   : PTD Read Type
*      Output:  Nothing
*
**  Return: SUCCESS         : Successful  
*           LDT_NTINFL_ADR  : Not in File 
*
**  Description: Read PTD Hourly Activity Report.
*===============================================================================
*/

SHORT RptHourlyPTDRead(UCHAR uchMinor, UCHAR uchType) 
{
    return(RptHourlyAllRead(uchMinor, uchType));
}

/*
*=============================================================================
**  Synopsis: SHORT RptHourlyAllRead(UCHAR uchMinorClass, UCHAR uchType) 
*               
*       Input:  uchMinor    : CLASS_TTLCURDAY   : Current Daily File
*                           : CLASS_TTLSAVDAY   : Saved Daily File
*                           : CLASS_TTLCURPTD   : Current PTD File
*                           : CLASS_TTLSAVPTD   : Saved PTD File
*               uchType     : RPT_ALL_READ      : All Read Type
*                           : RPT_EOD_ALLRAED   : EOD Type
*                           : RPT_PTD_ALLRAED   : PTD Type
*      Output:  Nothing
*
**  Return: SUCCESS         : Successful  
*           LDT_NTINFL_ADR  : Not in File 
*
**  Description: Read All Hourly Activity Report.  
*===============================================================================
*/

SHORT RptHourlyAllRead(UCHAR uchMinorClass, UCHAR uchType) 
{
    
    UCHAR           uchRptType;
    UCHAR           uchRptName;
    UCHAR           uchSpecMnemo;
    SHORT           sError;
    SHORT           sReturn;
	TTLHOURLY       TtlWork = {0};
    RPTHOURLYETK    HourlyEtkWork = {0};

	/* Check number of Hourly Time Blocks to see if Hourly Activity is in use. */
	if (RptHourGetBlock() < 1) {
		return(LDT_PROHBT_ADR);
	}

    /* EOD PTD Report Not Display Out MLD */
    if((uchMinorClass == CLASS_TTLSAVPTD) || (uchMinorClass == CLASS_TTLSAVDAY)){
        uchUifACRptOnOffMld = RPT_DISPLAY_OFF;
    }

	{
		TTLHOURLY       TtlData = {0};

		/* Get Hourly Time Activity Total */
		TtlData.uchMajorClass = CLASS_TTLHOURLY;
		TtlData.uchMinorClass = uchMinorClass;
		TtlData.usBlockRead = 0;

		TtlWork = TtlData;
		// Read in the hourly block sales totals
		for (TtlData.usBlockRead = 0; TtlData.usBlockRead < STD_MAX_HOURBLK; )
		{
			ULONG usCurReadPos = TtlData.usBlockRead;    // remember which block we are requesting so we put it in right Total bucket.
			sError = SerTtlTotalIncrRead(&TtlData);      // read specified block and increment the usBlockRead index
			if(sError == TTL_SUCCESS) {
				TtlWork.Total[usCurReadPos] = TtlData.Total[0];
			} else {
				return(TtlConvertError(sError));
			}
		}

		if (uchMinorClass == CLASS_TTLCURDAY || uchMinorClass == CLASS_TTLCURPTD) {
			//SR 810. During Customs implementation of reading hourly totals from 
			//non master terminals, added a new data type which is used, but forgot
			//to set the to date time in it. JHHJ
			TtlGetNdate (&TtlData.ToDate);      /* Get Current Date/Time and  Set Period To Date */
		}

		// Set the report start and end date/time stamps as well as last block read.
		TtlWork.FromDate = TtlData.FromDate;
		TtlWork.ToDate = TtlData.ToDate;
		TtlWork.usBlockRead = TtlData.usBlockRead;
	}

	RptPrtStatusSet(uchMinorClass);                         /* Set J|R print status */

    /* ---- calculate Labor Cost % on Hourly Total ---- */
    memset(&HourlyEtkWork, 0, sizeof(HourlyEtkWork));
    if ((!CliParaMDCCheck(MDC_HOUR_ADR, EVEN_MDC_BIT1)) &&
        (RptEODChkFileExist(FLEX_ETK_ADR) == RPT_FILE_EXIST) &&
        (uchMinorClass == CLASS_TTLCURDAY || uchMinorClass == CLASS_TTLSAVDAY)) {

        RptETKCalHourlyTotal(CLASS_TTLCURDAY, &TtlWork.FromDate, &TtlWork.ToDate, &HourlyEtkWork);
    }

    /* Select Special Mnemonics for Header */
    switch(uchMinorClass) {
    case CLASS_TTLCURDAY :                                  /* Case of Daily READ */
        uchSpecMnemo = SPC_DAIRST_ADR;                      /* Set Daily Special Mnemonics */
        uchRptName = RPT_READ_ADR;                          /* Set Read Mnemonics */        
        break;
   
    case CLASS_TTLSAVDAY :                                  /* Case of Daily RESET */
        uchSpecMnemo = SPC_DAIRST_ADR;                      /* Set Daily Special Mnemonics */
        uchRptName = RPT_RESET_ADR;                         /* Set Reset Mnemonics */
        break;

    case CLASS_TTLCURPTD :                                  /* Case of PTD READ */
        uchSpecMnemo = SPC_PTDRST_ADR;                      /* Set PTD Special Mnemonics */
        uchRptName = RPT_READ_ADR;                          /* Set Read Mnemonics */
        break;

    default:                                                /* Case of PTD RESET */
        uchSpecMnemo = SPC_PTDRST_ADR;                      /* Set PTD Special Mnemonics */
        uchRptName = RPT_RESET_ADR;                         /* Set Reset Mnemonics */
        break;
    }

    /* Print Header Name */
    uchRptType = uchType;
    if (uchType == RPT_EOD_ALLREAD || uchType == RPT_PTD_ALLREAD) {
        uchRptType = RPT_ALL_READ;
    }

    if (uchType != RPT_EOD_ALLREAD && uchType != RPT_PTD_ALLREAD) {                       /* Not EOD/PTD Function */
        MaintIncreSpcCo(SPCO_CONSEC_ADR);                   /* Count Up Consecutive No. */
    }
    MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class */              
                    RPT_ACT_ADR,                            /* Report Name Address */
                    RPT_HOUR_ADR,                           /* Report Name Address */
                    uchSpecMnemo,                           /* Special Mnemonics Address */
                    uchRptName,                             /* Report Name Address */
                    uchRptType,                             /* Report Type */
                    AC_HOURLY_READ_RPT,                     /* A/C Number */
                    0,                                      /* Reset Type */
                    PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit */                                       
    
    /* Print Period/To Date and Time */

    RptPrtTime(TRN_PFROM_ADR, &TtlWork.FromDate);         
    RptPrtTime(TRN_PTO_ADR, &TtlWork.ToDate);             
    RptFeed(RPT_DEFALTFEED);                                /* Feed One Line */

    if (uchType == RPT_ALL_READ || uchType == RPT_EOD_ALLREAD || uchType == RPT_PTD_ALLREAD) {

        /* Edit Total Report */
        if ((sReturn = RptHourlyAllEdit(&TtlWork, uchMinorClass, &HourlyEtkWork)) == RPT_ABORTED) {    /* Aborded By User */
            MaintMakeAbortKey();                                        /* Print ABORDED */
            MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);                /* Print Supervisor Trailer */
            return(sReturn);
        } else {

            /* Set Issued Flag for Reset Report */
            if ((uchMinorClass == CLASS_TTLSAVDAY) || (uchMinorClass == CLASS_TTLSAVPTD)) {
                SerTtlIssuedReset(&TtlWork, TTL_NOTRESET);
            }
        }
    } else {     // RPT_RANGE_READ
        /* Edit Total Report */
        if ((sReturn = RptHourlyRngEdit(&TtlWork, uchMinorClass, &HourlyEtkWork)) == RPT_ABORTED) {    /* Aborded By User */
            MaintMakeAbortKey();                                        /* Print ABORDED */
            MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);                /* Print Supervisor Trailer */
            return(sReturn);
        }
    }

    if (uchType != RPT_EOD_ALLREAD && uchType != RPT_PTD_ALLREAD) {                       /* Not EOD/PTD Function */
        MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);        /* Print Supervisor Trailer */
    } 
    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT RptHourlyAllEdit(TTLHOURLY *pTtlData)
*               
*       Input:  pTtlData    : Pointer to Structure for TTLHOURLY 
*      Output:  Nothing
*
**  Return: SUCCESS         : Successful  
*           RPT_ABORTED     : Aborted by User
*
**  Description: Prepare Print All Hourly Activity Report. 
*===============================================================================
*/

SHORT RptHourlyAllEdit(TTLHOURLY *pTtlData, UCHAR uchMinorClass, 
                                        RPTHOURLYETK *pRptHourlyEtk)
{

    SHORT           sNoOfPersonTtl = 0;
    USHORT          i;
    LONG            lItemPCountTtl = 0L;
    LONG            lNetSalesTtl = 0L;
    LONG            lAverageSalesTtl = 0L;
	RPTREGFIN       RegFin = {0};
	HOURLY          AddTtlHourly = {0};
    ULONG           ulAllWageTotal = 0L;
	UCHAR           uchRptBlockNo = RptHourGetBlock();

	/* Check number of Hourly Time Blocks to see if Hourly Activity is in use. */
	if (uchRptBlockNo < 1) {
		return RPT_ABORTED;
	}

    /* Print Each Hourly Activity Block Total */
    for (i = 0; i < uchRptBlockNo; i++) {
		PARAHOURLYTIME  ParaHourly = {0};

        if (UieReadAbortKey() == UIE_ENABLE) {          /* Putting Abort Key */                         
            return(RPT_ABORTED);
        }
        if(RptPauseCheck() == RPT_ABORTED){
             return(RPT_ABORTED);
        }

        /* Check Execute Print or Not */
        if (!RptHourlyChkTotal(&(pTtlData->Total[i]), pRptHourlyEtk->ulWageTotal[i])) {        /* Not Execute Print */
            continue;
        }

        /* Get Time of Hourly Activity Block */
		ParaHourly.uchMajorClass = CLASS_PARAHOURLYTIME;
        ParaHourly.uchAddress = ( UCHAR)(i + HOUR_TSEG1_ADR);   // calculate correct address for this block from zero based offset
        CliParaRead(&ParaHourly);

        /* Print Time of Hourly Activity Block */
        RptHourlyElement(CLASS_RPTHOURLY_PRTTIME,               /* Set Date  for Print Out      */
                         0,                                     /* |--------------------------| */
                         ( USHORT)ParaHourly.uchMin,            /* |                  Z9:99AM | */
                         ( USHORT)ParaHourly.uchHour,           /* |--------------------------| */
                         0,
                         0L);

        /* Print Hourly Activity Each Element */
        RptHourlyTtlEdit(pTtlData->Total[i].lItemproductivityCount,
                         pTtlData->Total[i].sDayNoOfPerson,
                         pTtlData->Total[i].lHourlyTotal,
                         &pTtlData->Total[i].lBonus[0],
                         RptHourlyCalSalePerson(pTtlData->Total[i].lHourlyTotal, pTtlData->Total[i].sDayNoOfPerson),
                         uchMinorClass,
                         pRptHourlyEtk->ulWageTotal[i],
                         RptHourlyPercent(pTtlData->Total[i].lHourlyTotal, pRptHourlyEtk->ulWageTotal[i]));

        /* Add Hourly Activity Each Element */
        RptHourlyAddTotal(&AddTtlHourly, &(pTtlData->Total[i]));

        ulAllWageTotal += pRptHourlyEtk->ulWageTotal[i];
    }

    /* Check Execute Print or Not */
    if (uchRptMldAbortStatus) {
        return(SUCCESS);                           /* aborted by MLD */
    }

    if (!RptHourlyChkTotal(&AddTtlHourly, ulAllWageTotal)) {     /* Not Execute Print */
        return(SUCCESS);
    }

    /* Print TOTAL Mnemonics */
    RegFin.uchMajorClass = CLASS_RPTREGFIN;
    RegFin.uchMinorClass = CLASS_RPTREGFIN_MNEMO;
    RegFin.usPrintControl = usRptPrintStatus;
    RflGetTranMnem (RegFin.aszTransMnemo, TRN_TTLR_ADR);
    if(uchUifACRptOnOffMld == RPT_DISPLAY_ON){
        uchRptMldAbortStatus = (UCHAR)MldDispItem(&RegFin, 0); /* display on LCD          */ 
        RegFin.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
    }
    PrtPrintItem(NULL, &RegFin);

    /* Print Hourly Activity Total Each Element */
    if (uchRptMldAbortStatus) {
        return(SUCCESS);                           /* aborted by MLD */
    }

    RptHourlyTtlEdit(AddTtlHourly.lItemproductivityCount,
                     AddTtlHourly.sDayNoOfPerson,
                     AddTtlHourly.lHourlyTotal,
                     &AddTtlHourly.lBonus[0],
                     RptHourlyCalSalePerson(AddTtlHourly.lHourlyTotal, AddTtlHourly.sDayNoOfPerson),
                     uchMinorClass,
                     ulAllWageTotal,
                     RptHourlyPercent(AddTtlHourly.lHourlyTotal, ulAllWageTotal));

    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT RptHourlyRngEdit(TTLHOURLY *pTtlData)
*               
*       Input:  pTtlData    : Pointer to Structure for TTLHOURLY 
*      Output:  Nothing
*
**  Return: SUCCESS         : Successful  
*           RPT_ABORTED     : Aborted by User
*
**  Description: Prepare Print Range Hourly Activity Report. 
*===============================================================================
*/

SHORT RptHourlyRngEdit(TTLHOURLY *pTtlData, UCHAR uchMinorClass,
                                        RPTHOURLYETK *pRptHourlyEtk)
{

    USHORT          usStartIndex;
    USHORT          usEndIndex;
    USHORT          i;
	RPTREGFIN       RegFin = {0};
	HOURLY          AddTtlHourly = {0};
    ULONG           ulAllWageTotal = 0L;

    /* Get Start/End Hourly Time Block zero based Index */
    usStartIndex = RptGetHourlyIndex(&RptStartTime);
    usEndIndex = RptGetHourlyIndex(&RptEndTime);

    if (usStartIndex <= usEndIndex) {                 /* Start Time is Equal or Smaller than End Time */
        /* Print Each Hourly Activity Block Total */
        for (i = usStartIndex; i <= usEndIndex; i++) {
			PARAHOURLYTIME  ParaHourly = {0};

            if (UieReadAbortKey() == UIE_ENABLE) {      /* Putting Abort Key */
                return(RPT_ABORTED);
            }
            if(RptPauseCheck() == RPT_ABORTED){
                 return(RPT_ABORTED);
            }

            /* Check Execute Print or Not */
            if (!RptHourlyChkTotal(&(pTtlData->Total[i]), pRptHourlyEtk->ulWageTotal[i])) {        /* Not Execute Print */
                continue;
            }

            /* Get Time of Hourly Activity Block */
			ParaHourly.uchMajorClass = CLASS_PARAHOURLYTIME;
            ParaHourly.uchAddress = ( UCHAR)(i + HOUR_TSEG1_ADR);   // calculate correct address for this block from zero based offset
            CliParaRead(&ParaHourly);

            /* Print Time of Hourly Activity Block */
            RptHourlyElement(CLASS_RPTHOURLY_PRTTIME,               /* Set Date  for Print Out      */
                             0,                                     /* |--------------------------| */
                             ( USHORT)ParaHourly.uchMin,            /* |                  Z9:99AM | */
                             ( USHORT)ParaHourly.uchHour,           /* |--------------------------| */
                             0,
                             0L);

            /* Print Hourly Activity Each Element */
            RptHourlyTtlEdit(pTtlData->Total[i].lItemproductivityCount,
                             pTtlData->Total[i].sDayNoOfPerson,
                             pTtlData->Total[i].lHourlyTotal,
                             &pTtlData->Total[i].lBonus[0],
                             RptHourlyCalSalePerson(pTtlData->Total[i].lHourlyTotal, pTtlData->Total[i].sDayNoOfPerson),
                             uchMinorClass,
                             pRptHourlyEtk->ulWageTotal[i],
                             RptHourlyPercent(pTtlData->Total[i].lHourlyTotal, pRptHourlyEtk->ulWageTotal[i]));

            /* Add Hourly Activity Each Element */
            RptHourlyAddTotal(&AddTtlHourly, &(pTtlData->Total[i]));

            ulAllWageTotal += pRptHourlyEtk->ulWageTotal[i];
        }
    } else {
		USHORT    usRptBlockNo = RptHourGetBlock();

        /* Print Each Hourly Activity Block Total */
        for (i = usStartIndex; i < usRptBlockNo; i++) {
			PARAHOURLYTIME  ParaHourly = {0};

            if (UieReadAbortKey() == UIE_ENABLE) {                  /* Putting Abort Key */                         
                return(RPT_ABORTED);
            }
            if(RptPauseCheck() == RPT_ABORTED){
                 return(RPT_ABORTED);
            }

            /* Check Execute Print or Not */
            if (!RptHourlyChkTotal(&(pTtlData->Total[i]), pRptHourlyEtk->ulWageTotal[i])) {        /* Not Execute Print */
                continue;
            }

            /* Get Time of Hourly Activity Block */
			ParaHourly.uchMajorClass = CLASS_PARAHOURLYTIME;
            ParaHourly.uchAddress = ( UCHAR)(i + HOUR_TSEG1_ADR);   // calculate correct address for this block from zero based offset
            CliParaRead(&ParaHourly);

            /* Print Time of Hourly Activity Block */
            RptHourlyElement(CLASS_RPTHOURLY_PRTTIME,               /* Set Date  for Print Out      */
                             0,                                     /* |--------------------------| */
                             ( USHORT)ParaHourly.uchMin,            /* |                  Z9:99AM | */
                             ( USHORT)ParaHourly.uchHour,           /* |--------------------------| */
                             0,
                             0L);

            /* Print Hourly Activity Each Element */
            RptHourlyTtlEdit(pTtlData->Total[i].lItemproductivityCount,
                             pTtlData->Total[i].sDayNoOfPerson,
                             pTtlData->Total[i].lHourlyTotal,
                             &pTtlData->Total[i].lBonus[0],
                             RptHourlyCalSalePerson(pTtlData->Total[i].lHourlyTotal, pTtlData->Total[i].sDayNoOfPerson),
                             uchMinorClass,
                             pRptHourlyEtk->ulWageTotal[i],
                             RptHourlyPercent(pTtlData->Total[i].lHourlyTotal, pRptHourlyEtk->ulWageTotal[i]));

            /* Add Hourly Activity Each Element */
            RptHourlyAddTotal(&AddTtlHourly, &(pTtlData->Total[i]));

            ulAllWageTotal += pRptHourlyEtk->ulWageTotal[i];
        }

        for (i = 0; i <= usEndIndex; i++) {
			PARAHOURLYTIME  ParaHourly = {0};

            if (UieReadAbortKey() == UIE_ENABLE) {                  /* Putting Abort Key */                         
                return(RPT_ABORTED);
            }
            if(RptPauseCheck() == RPT_ABORTED){
                 return(RPT_ABORTED);
            }

            /* Check Execute Print or Not */
            if (!RptHourlyChkTotal(&(pTtlData->Total[i]), pRptHourlyEtk->ulWageTotal[i])) {        /* Not Execute Print */
                continue;
            }

            /* Get Time of Hourly Activity Block */
			ParaHourly.uchMajorClass = CLASS_PARAHOURLYTIME;
            ParaHourly.uchAddress = ( UCHAR)(i + HOUR_TSEG1_ADR);   // calculate correct address for this block from zero based offset
            CliParaRead(&ParaHourly);

            /* Print Time of Hourly Activity Block */
            RptHourlyElement(CLASS_RPTHOURLY_PRTTIME,               /* Set Date  for Print Out      */
                             0,                                     /* |--------------------------| */
                             ( USHORT)ParaHourly.uchMin,            /* |                  Z9:99AM | */
                             ( USHORT)ParaHourly.uchHour,           /* |--------------------------| */
                             0,
                             0L);

            /* Print Hourly Activity Each Element */
            RptHourlyTtlEdit(pTtlData->Total[i].lItemproductivityCount,
                             pTtlData->Total[i].sDayNoOfPerson,
                             pTtlData->Total[i].lHourlyTotal,
                             &pTtlData->Total[i].lBonus[0],
                             RptHourlyCalSalePerson(pTtlData->Total[i].lHourlyTotal, pTtlData->Total[i].sDayNoOfPerson),
                             uchMinorClass,
                             pRptHourlyEtk->ulWageTotal[i],
                             RptHourlyPercent(pTtlData->Total[i].lHourlyTotal, pRptHourlyEtk->ulWageTotal[i]));

            /* Add Hourly Activity Each Element */
            RptHourlyAddTotal(&AddTtlHourly, &(pTtlData->Total[i]));

            ulAllWageTotal += pRptHourlyEtk->ulWageTotal[i];
        }
    }

    /* Check Execute Print or Not */
    if (uchRptMldAbortStatus) {
        return(SUCCESS);                           /* aborted by MLD */
    }

    if (!RptHourlyChkTotal(&AddTtlHourly, ulAllWageTotal)) {/* Not Execute Print */
        return(SUCCESS);
    }

    /* Print TOTAL Mnemonics */
    RegFin.uchMajorClass = CLASS_RPTREGFIN;
    RegFin.uchMinorClass = CLASS_RPTREGFIN_MNEMO;
    RegFin.usPrintControl = usRptPrintStatus;
    RflGetTranMnem (RegFin.aszTransMnemo, TRN_TTLR_ADR);
    if(uchUifACRptOnOffMld == RPT_DISPLAY_ON){
        uchRptMldAbortStatus = (UCHAR)MldDispItem(&RegFin, 0); /* display on LCD          */ 
        RegFin.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
    }
    PrtPrintItem(NULL, &RegFin);

    /* Print Hourly Activity Total Each Element */
    if (uchRptMldAbortStatus) {
        return(SUCCESS);                           /* aborted by MLD */
    }

    RptHourlyTtlEdit(AddTtlHourly.lItemproductivityCount,
                     AddTtlHourly.sDayNoOfPerson,
                     AddTtlHourly.lHourlyTotal,
                     &AddTtlHourly.lBonus[0],
                     RptHourlyCalSalePerson(AddTtlHourly.lHourlyTotal, AddTtlHourly.sDayNoOfPerson),
                     uchMinorClass,
                     ulAllWageTotal,
                     RptHourlyPercent(AddTtlHourly.lHourlyTotal, ulAllWageTotal));

    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: VOID RptHourlyTtlEdit( LONG  lItemPCountTtl,
*                                    SHORT sSalePersonTtl,
*                                    LONG  lNetSalesTtl,
                                     LONG  lBonus[],
*                                    LONG  lAverageSalesTtl )
*               
*       Input:  lItemPCountTtl      : Item Productivity Counter
*               sSalePersonTtl      : Number of Person
*               lNetSalesTtl        : Net Sales
*               lAverageSalesTtl    : Sales/Person
*
*      Output:  Nothing
*
**  Return: Nothing  
*
**  Description: Print Hourly Activity Each Element. 
*===============================================================================
*/

static VOID RptHourlyTtlEdit(LONG  lItemPCountTtl,
                      SHORT sSalePersonTtl,
                      DCURRENCY  lNetSalesTtl,
                      DCURRENCY  lBonus[],
                      DCURRENCY  lAverageSalesTtl,
                      UCHAR uchMinorClass,
                      LONG  lLaborTtl,
                      LONG  lPercent)
{

    DCURRENCY    lTotal = 0L;
    UCHAR        uchNo, uchBonusIndex;    /* store any Number      */
    USHORT       usMDCData, usReturnLen;
    PARADISCTBL  Data;

    /* Print Item Productivity Counter */
    RptHourlyElement(CLASS_RPTREGFIN_PRTNO,                 /* Set Date  for Print Out      */
                     TRN_ITMPROCO_ADR,                      /* |--------------------------| */
                     0,                                     /* | ITEM CO.     SZZZZZZZZZ9 | */
                     0,                                     /* |--------------------------| */
                     lItemPCountTtl,
                     0L);

    /* Print Number of Person */
    RptHourlyElement(CLASS_RPTREGFIN_PRTNO,                 /* Set Date  for Print Out      */
                     TRN_PSN_ADR,                           /* |--------------------------| */
                     0,                                     /* | #.PERSON          SZZZZ9 | */
                     0,                                     /* |--------------------------| */
                     ( LONG)sSalePersonTtl,
                     0L);

    /* Print Net Sales */
    RptHourlyElement(CLASS_RPTREGFIN_PRTDGG,                /* Set Date  for Print Out      */
                     TRN_AMTTL_ADR,                         /* |--------------------------| */
                     0,                                     /* | NET SALE    SZZZZZZZ9.99 | */
                     0,                                     /* |--------------------------| */
                     0,
                     lNetSalesTtl);

    /* Print Sales/Person */

    if (sSalePersonTtl <= 0 || lNetSalesTtl < 0L) {         /* Person Counter is 0 */                            
        RptHourlyElement(CLASS_RPTHOURLY_PRTASTER,          /* Set Date  for Print Out      */
                         TRN_NETSP_ADR,                     /* |--------------------------| */
                         0,                                 /* | SALE/PSN    ************ | */
                         0,                                 /* |--------------------------| */
                         0,
                         0L);

    } else {
        RptHourlyElement(CLASS_RPTREGFIN_PRTDGG,            /* Set Date  for Print Out      */
                         TRN_NETSP_ADR,                     /* |--------------------------| */
                         0,                                 /* | SALE/PSN    SZZZZZZZ9.99 | */
                         0,                                 /* |--------------------------| */
                         0,
                         lAverageSalesTtl);

    }

    /* print Bonus Total #1 Hourly Block */
    if (!CliParaMDCCheck(MDC_HOUR2_ADR, ODD_MDC_BIT0)) {
        RptHourlyElement(CLASS_RPTREGFIN_PRTDGG,            /* Set Date  for Print Out      */
                         TRN_BNS1_ADR,                      /* |--------------------------| */
                         0,                                 /* | BONUS #1    SZZZZZZZ9.99 | */
                         0,                                 /* |--------------------------| */
                         0,
                         lBonus[0]);
        lTotal += lBonus[0];
    }

    /* print Bonus Total #2 Hourly Block */
    if (!CliParaMDCCheck(MDC_HOUR2_ADR, ODD_MDC_BIT1)) {
        RptHourlyElement(CLASS_RPTREGFIN_PRTDGG,            /* Set Date  for Print Out      */
                         TRN_BNS2_ADR,                      /* |--------------------------| */
                         0,                                 /* | BONUS #2    SZZZZZZZ9.99 | */
                         0,                                 /* |--------------------------| */
                         0,
                         lBonus[1]);
        lTotal += lBonus[1];
    }

    /* print Bonus Total #3 Hourly Block */
    if (!CliParaMDCCheck(MDC_HOUR2_ADR, ODD_MDC_BIT2)) {
        RptHourlyElement(CLASS_RPTREGFIN_PRTDGG,            /* Set Date  for Print Out      */
                         TRN_BNS3_ADR,                      /* |--------------------------| */
                         0,                                 /* | BONUS #3    SZZZZZZZ9.99 | */
                         0,                                 /* |--------------------------| */
                         0,
                         lBonus[2]);
        lTotal += lBonus[2];
    }

    /* print Bonus Total #4 Hourly Block */
    if (!CliParaMDCCheck(MDC_HOUR2_ADR, ODD_MDC_BIT3)) {
        RptHourlyElement(CLASS_RPTREGFIN_PRTDGG,            /* Set Date  for Print Out      */
                         TRN_BNS4_ADR,                      /* |--------------------------| */
                         0,                                 /* | BONUS #4    SZZZZZZZ9.99 | */
                         0,                                 /* |--------------------------| */
                         0,
                         lBonus[3]);
        lTotal += lBonus[3];
    }

    /* print Bonus Total #5 Hourly Block */
    if (!CliParaMDCCheck(MDC_HOUR3_ADR, EVEN_MDC_BIT0)) {
        RptHourlyElement(CLASS_RPTREGFIN_PRTDGG,            /* Set Date  for Print Out      */
                         TRN_BNS5_ADR,                      /* |--------------------------| */
                         0,                                 /* | BONUS #5    SZZZZZZZ9.99 | */
                         0,                                 /* |--------------------------| */
                         0,
                         lBonus[4]);
        lTotal += lBonus[4];
    }

    /* print Bonus Total #6 Hourly Block */
    if (!CliParaMDCCheck(MDC_HOUR3_ADR, EVEN_MDC_BIT1)) {
        RptHourlyElement(CLASS_RPTREGFIN_PRTDGG,            /* Set Date  for Print Out      */
                         TRN_BNS6_ADR,                      /* |--------------------------| */
                         0,                                 /* | BONUS #6    SZZZZZZZ9.99 | */
                         0,                                 /* |--------------------------| */
                         0,
                         lBonus[5]);
        lTotal += lBonus[5];
    }

    /* print Bonus Total #7 Hourly Block */
    if (!CliParaMDCCheck(MDC_HOUR3_ADR, EVEN_MDC_BIT2)) {
        RptHourlyElement(CLASS_RPTREGFIN_PRTDGG,            /* Set Date  for Print Out      */
                         TRN_BNS7_ADR,                      /* |--------------------------| */
                         0,                                 /* | BONUS #7    SZZZZZZZ9.99 | */
                         0,                                 /* |--------------------------| */
                         0,
                         lBonus[6]);
        lTotal += lBonus[6];
    }

    /* print Bonus Total #8 Hourly Block */
    if (!CliParaMDCCheck(MDC_HOUR3_ADR, EVEN_MDC_BIT3)) {
        RptHourlyElement(CLASS_RPTREGFIN_PRTDGG,            /* Set Date  for Print Out      */
                         TRN_BNS8_ADR,                      /* |--------------------------| */
                         0,                                 /* | BONUS #8    SZZZZZZZ9.99 | */
                         0,                                 /* |--------------------------| */
                         0,
                         lBonus[7]);
        lTotal += lBonus[7];
    }
	uchBonusIndex = 8;
	usMDCData = 0;
    CliParaAllRead(CLASS_PARAMDC,
                   ((UCHAR *)&usMDCData),       /* store MDC Bit status */
                   2,
                   (MDC_HOUR5_ADR - 1)/2,    /* To adjust the offset in MDC Address area*/
                   &usReturnLen);

    Data.uchMajorClass = CLASS_PARADISCTBL;                          
    for (uchNo = 0; uchBonusIndex < STD_NO_BONUS_TTL; uchNo++, uchBonusIndex++) {       /* BONUS TL #1 to #8 CO/TL  */
        if (!(usMDCData & 0x01)) {
            Data.uchAddress = (UCHAR)(RATE_BONUS9_ADR+uchNo);
            CliParaRead(&Data);
			RptHourlyElement(CLASS_RPTREGFIN_PRTDGG,            /* Set Date  for Print Out       */
							 (USHORT)(TRN_BNS_9_ADR + uchNo),   /* |---------------------------| */
							 0,                                 /* | BONUS #10    SZZZZZZZ9.99 | */
							 0,                                 /* |---------------------------| */
							 0,
							 lBonus[uchBonusIndex]);
			lTotal += lBonus[uchBonusIndex];
        }

		switch(uchNo)
		{
			case  0:  //After Bonus  9	NEXT MDC-> 463C
			case  1:  //After Bonus 10	NEXT MDC-> 463B
			case 11:  //After Bonus 20	NEXT MDC-> 463A
			case 21:  //After Bonus 30	NEXT MDC-> 464D
			case 31:  //After Bonus 40	NEXT MDC-> 464C
			case 41:  //After Bonus 50	NEXT MDC-> 464B
			case 51:  //After Bonus 60	NEXT MDC-> 464A
			case 61:  //After Bonus 70	NEXT MDC-> 465D
			case 71:  //After Bonus 80	NEXT MDC-> 465C
			case 81:  //After Bonus 90	NEXT MDC-> 465B
				usMDCData >>= 1;                        /* Shift right 1 bit each time */
				break;
			default:
				break;
		}

        if (UieReadAbortKey() == UIE_ENABLE) {                      /* if Abort ?      */
            return;
        }                                                           /*                 */
        if(RptPauseCheck() == RPT_ABORTED){
             return;
        }
    }

//Print Bonus total 11-20
//Print Bonus total 21-30
//Print Bonus total 31-40
//Print Bonus total 41-50
//Print Bonus total 51-60
//Print Bonus total 61-70
//Print Bonus total 71-80
//Print Bonus total 81-90
//Print Bonus total 91-100


    /* print Bonus Total Hourly Block */
    if (!CliParaMDCCheck(MDC_HOUR4_ADR, ODD_MDC_BIT0)) {
        RptHourlyElement(CLASS_RPTREGFIN_PRTDGG,            /* Set Date  for Print Out      */
                         TRN_TTLR_ADR,                      /* |--------------------------| */
                         0,                                 /* | TOTAL       SZZZZZZZ9.99 | */
                         0,                                 /* |--------------------------| */
                         0,
                         lTotal);
    }


    /* print Labor Cost Percent */
    if ((!CliParaMDCCheck(MDC_HOUR_ADR, EVEN_MDC_BIT1)) &&
        (RptEODChkFileExist(FLEX_ETK_ADR) == RPT_FILE_EXIST) &&
        (uchMinorClass == CLASS_TTLCURDAY || uchMinorClass == CLASS_TTLSAVDAY)) {

        RptHourlyElement(CLASS_RPTREGFIN_PRTDGG,            /* Set Date  for Print Out      */
                         TRN_LABORTTL_ADR,                  /* |--------------------------| */
                         0,                                 /* | LABOR TL    SZZZZZZZ9.99 | */
                         0,                                 /* |--------------------------| */
                         0,
                         lLaborTtl);


        RptHourlyElement(CLASS_RPTHOURLY_PERCENT,           /* Set Date  for Print Out      */
                         TRN_LABORPERCENT_ADR,              /* |--------------------------| */
                         0,                                 /* | L-COST %    SZZZZZZ9.99% | */
                         0,                                 /* |--------------------------| */
                         0,
                         lPercent);

    }

    RptFeed(RPT_DEFALTFEED);                                /* Feed Line */

}

/*
*=============================================================================
**  Synopsis: VOID RptHourlyElement( UCHAR  uchMinorClass,
*                                    UCHAR  uchTransAddr,
*                                    USHORT usMin,
*                                    USHORT usHour,
*                                    LONG   lCounter,
*                                    LONG   lTotal ) 
*                   
*       Input: uchMinorClass    : Minor Class
*              uchTransAddr     : Transaction Mnemonics Address
*              usMin            : Minute of Hourly Block
*              usHour           : Hour of Hourly Block
*              lCounter         : Number of Person/Item Productivity Counter 
*              lTotal           : Total  
*                                 
*      Output:  Nothing
*
**  Return: SUCCESS         : Successful  
*
**  Description: Format Print Format of Hourly Activity Report.  
*===============================================================================
*/
VOID RptHourlyElement( UCHAR  uchMinorClass,
                       USHORT  uchTransAddr,
                       USHORT usMin,
                       USHORT usHour,
                       LONG   lCounter,
                       DCURRENCY   lTotal ) 
{
	RPTREGFIN           RptRegFin = {0};
	RPTHOURLY           RptHourly = {0};

    if (uchRptMldAbortStatus) return;                       /* aborted by MLD */

    /* Initial Mnemonics Buffer */
    /* Preset Each Parameter */
    RptHourly.uchMajorClass = CLASS_RPTHOURLY;
    RptRegFin.uchMajorClass = CLASS_RPTREGFIN;
    RptHourly.usPrintControl = usRptPrintStatus;
    RptRegFin.usPrintControl = usRptPrintStatus;

    /* Distingush Minor Class */
    switch (uchMinorClass) {
    case CLASS_RPTHOURLY_PRTTIME:                                   /* Set Date  for Print Out      */
        /* Set Time Data */                                         /* |--------------------------| */
                                                                    /* |                  Z9:99AM | */
        RptHourly.StartTime.usMin = usMin;                          /* |--------------------------| */
        RptHourly.StartTime.usHour = usHour;

        /* Set Minor Class */
        RptHourly.uchMinorClass = CLASS_RPTHOURLY_PRTTIME;

        /* Output Data to Print Module */
        if(uchUifACRptOnOffMld == RPT_DISPLAY_ON){
            uchRptMldAbortStatus = (UCHAR)MldDispItem(&RptHourly, 0); /* display on LCD          */ 
            RptHourly.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
        }
        PrtPrintItem(NULL, &RptHourly);
        break;

    case CLASS_RPTREGFIN_PRTNO:                                     /* Set Date  for Print Out      */
        /* Get and Set Transaction Mnemonics */                     /* |--------------------------| */
                                                                    /* | OPENED           Z9:99AM | */
                                                                    /* |--------------------------| */
		/* Set Date  for Print Out      */
        RflGetTranMnem (RptRegFin.aszTransMnemo, uchTransAddr);     /* |--------------------------| */
																	/* |--------------------------| */
        /* Set Data */
        RptRegFin.Total.lAmount = lCounter;
        
        /* Set Minor Class */
        RptRegFin.uchMinorClass = CLASS_RPTREGFIN_PRTNO;

        /* Output Data to Print Module */
        if(uchUifACRptOnOffMld == RPT_DISPLAY_ON){
            uchRptMldAbortStatus = (UCHAR)MldDispItem(&RptRegFin, 0); /* display on LCD          */ 
            RptRegFin.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
        }
        PrtPrintItem(NULL, &RptRegFin);
        break;

    case CLASS_RPTREGFIN_PRTDGG:                                    /* Set Date  for Print Out      */
        /* Get and Set Transaction Mnemonics */                     /* |--------------------------| */
                                                                    /* | TRA FROM    SZZZZZZZ9.99 | */
        RflGetTranMnem(RptRegFin.aszTransMnemo, uchTransAddr);      /* |--------------------------| */

        /* Set Total */
        RptRegFin.Total.lAmount = lTotal;

        /* Set Minor Class */
        RptRegFin.uchMinorClass = CLASS_RPTREGFIN_PRTDGG;

        /* Output Data to Print Module */
        if(uchUifACRptOnOffMld == RPT_DISPLAY_ON) {
            uchRptMldAbortStatus = (UCHAR)MldDispItem(&RptRegFin, 0); /* display on LCD          */ 
            RptRegFin.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
        }
        PrtPrintItem(NULL, &RptRegFin);
        break;

    case CLASS_RPTHOURLY_PRTASTER:                                  /* Set Date  for Print Out      */
        /* Get and Set Transaction Mnemonics */                     /* |--------------------------| */
                                                                    /* | TRA FROM    ************ | */
        RflGetTranMnem (RptHourly.aszTransMnemo, uchTransAddr);     /* |--------------------------| */

        /* Set Minor Class */
        RptHourly.uchMinorClass = CLASS_RPTHOURLY_PRTASTER;

        /* Output Data to Print Module */
        if(uchUifACRptOnOffMld == RPT_DISPLAY_ON){
            uchRptMldAbortStatus = (UCHAR)MldDispItem(&RptHourly, 0); /* display on LCD          */ 
            RptHourly.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
        }
        PrtPrintItem(NULL, &RptHourly);
        break;

    case CLASS_RPTHOURLY_PERCENT:                                   /* Set Date  for Print Out      */
        /* Get and Set Transaction Mnemonics */                     /* |--------------------------| */
                                                                    /* | TRA FROM    ************ | */
        RflGetTranMnem (RptHourly.aszTransMnemo, uchTransAddr);     /* |--------------------------| */

        /* Set Total */
        RptHourly.Total.lAmount = lTotal;

        /* Set Minor Class */
        RptHourly.uchMinorClass = CLASS_RPTHOURLY_PERCENT;

        /* Output Data to Print Module */
        if(uchUifACRptOnOffMld == RPT_DISPLAY_ON){
            uchRptMldAbortStatus = (UCHAR)MldDispItem(&RptHourly, 0); /* display on LCD          */ 
            RptHourly.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
        }
        PrtPrintItem(NULL, &RptHourly);
        break;

    default:
/*        PifAbort(MODULE_RPT_ID, FAULT_INVALID_DATA); */
        break;
    }
}

/*
*=============================================================================
**  Synopsis: LONG RptHourlyCalSalePerson( LONG  lAmount, SHORT sPerson ) 
*               
*       Input:  lAmount     : Net Sales Total
*               sPerson     : Number of Person
*      Output:  Nothing
*
**  Return: Net Sales Totals every Person  
*
**  Description: Calculate Net Sales Totals every Person.  
*===============================================================================
*/
static DCURRENCY RptHourlyCalSalePerson(DCURRENCY lAmount, SHORT sPerson)
{
    if (sPerson) {                      /* Not Divide 0 Case */
        return(RptRound(lAmount, sPerson));
    }
    return(0L);
}

/*
*=============================================================================
**  Synopsis: UCHAR RptGetHourlyIndex(TIME *pRangeTime)
*               
*       Input:  pRangeTime  : Pointer to Structure for TIME 
*      Output:  Nothing
*
**  Return: Zero based Index of Hourly Activity Block 
*
**  Description: Calculate zero based Index of Hourly Activity Block.  This is the array
*                offset for a particular Hourly Activity Block.
*
*                This does not provide the address used with the major class CLASS_PARAHOURLYTIME
*                and PARAHOURLYTIME struct with CliParaRead().
*
*                In order to provide the address for CLASS_PARAHOURLYTIME you must
*                add HOUR_TSEG1_ADR which is the address of the first Hourly Activity
*                Block time. See the description of the CLASS_PARAHOURLYTIME data.
*===============================================================================
*/
static UCHAR RptGetHourlyIndex(N_TIME *pRangeTime)
{
    USHORT          usTOD = 0x100 * pRangeTime->usHour + pRangeTime->usMin;
	USHORT          usRptBlockNo = RptHourGetBlock();
	USHORT          usCurrBlockNo = 0;
    USHORT          i;
    USHORT          usTime1;
    PARAHOURLYTIME  WorkHourly;

	/* Check number of Hourly Time Blocks to see if Hourly Activity is in use and at least 3 blocks. */
	if (usRptBlockNo < 3) {
		return 0;
	}

    WorkHourly.uchMajorClass = CLASS_PARAHOURLYTIME;
    WorkHourly.uchAddress = HOUR_TSEG1_ADR;                         /* 1'st hourly block */
    CliParaRead( &WorkHourly );
    usTime1 = 0x100 * WorkHourly.uchHour + WorkHourly.uchMin;       /* 1'st hourly block, starting hourly block of search */
	usCurrBlockNo = 0;

    for ( i = 1; i < usRptBlockNo; i++ ) {         /* search target hourly block */
		USHORT          usTime2;

        WorkHourly.uchAddress = ( UCHAR )( HOUR_TSEG1_ADR + i );        /* calculate correct address for next block */
		CliParaRead( &WorkHourly );
		usTime2 = 0x100 * WorkHourly.uchHour + WorkHourly.uchMin;       /* calculate time of the next hourly block */

        if ( usTime1 <= usTOD && usTOD < usTime2 ) {                /* normal case where start time is less than this hourly block time */
            break;                                                  /* exit loop if we found an ending hourly block */
        }

        if ( usTime1 >= usTime2 ) {                                 /* abnormal case where start time is greater than this hourly block time */
            if ( usTOD >= usTime1 || usTOD < usTime2 ) {
                break;                                              /* exit loop */            
            }
        }   

        usTime1 = usTime2;
		usCurrBlockNo = i;
    }    

	if (usCurrBlockNo >= usRptBlockNo) usCurrBlockNo = usRptBlockNo - 1;  // point to last block if we couldn't find a match.

    return(( UCHAR)usCurrBlockNo);                                 
}

/*
*=============================================================================
**  Synopsis: VOID RptHourlyAddTotal( HOURLY *pAddTtl, HOURLY *pTtlData )
*               
*       Input:  pAddTtl     : Pointer to Structure for HOURLY 
*               pTtlData    : Pointer to Structure for HOURLY 
*      Output:  Nothing
*
**  Return: Nothing 
*
**  Description: Add Total of Hourly Block Total and Counter.  
*===============================================================================
*/

static VOID RptHourlyAddTotal(HOURLY *pAddTtl, HOURLY *pTtlData)
{
	int i;

    pAddTtl->lItemproductivityCount += pTtlData->lItemproductivityCount;
    pAddTtl->sDayNoOfPerson += pTtlData->sDayNoOfPerson;
    pAddTtl->lHourlyTotal += pTtlData->lHourlyTotal;
	for (i = 0; i < STD_NO_BONUS_TTL; i++) {  // Should this be STD_NO_BONUS_TTL?  RJC
        pAddTtl->lBonus[i] += pTtlData->lBonus[i];
	}
}

/*
*=============================================================================
**  Synopsis: BOOL RptHourlyChkTotal( HOURLY *pAddTtl, HOURLY *pTtlData )
*               
*       Input:  pAddTtl     : Pointer to Structure for HOURLY 
*      Output:  Nothing
*
**  Return:     0: Not Execute Print 
*               1: Execute Print
*
**  Description: Check Total/Counter and MDC to decide Print Report.  
*===============================================================================
*/

static BOOL RptHourlyChkTotal( HOURLY *pTotal, ULONG ulWageTotal)
{
	BOOL  bOnlyZeroTotals = TRUE;
	int   iLoop;

    /* Check MDC and Active Total */
	for (iLoop = 0; iLoop < STD_NO_BONUS_TTL && (pTotal->lBonus[iLoop] == 0); iLoop++) ;  // scan bonus totals looking for non-zero one.

    bOnlyZeroTotals = (iLoop >= STD_NO_BONUS_TTL);  // check if we looped through all bonus totals or stopped at a non-zero one.

    if (CliParaMDCCheck(MDC_HOUR_ADR, EVEN_MDC_BIT0) &&
                                (pTotal->lItemproductivityCount == 0L) &&
                                (pTotal->sDayNoOfPerson == 0)  &&
                                bOnlyZeroTotals &&
                                (pTotal->lHourlyTotal== 0L) &&
                                (ulWageTotal == 0L) ) {   /* Do not Print Total/Counter is 0 */
        return(0);                                              
    }
    return(1);
}

static LONG  RptHourlyPercent(DCURRENCY lTotal, DCURRENCY lCost)
{
	D13DIGITS d13Product;
    LONG      lPercent = RPT_OVERFLOW;

    /*----- A Numerator is less than 0 ?   OR A Denominator is less than or equal 0 ? -----*/
    if ((lTotal <= 0L) || (lCost <= 0L)) {
        return(RPT_OVERFLOW);
    }

	d13Product = 100000L;
	d13Product *= lCost;
	d13Product /= lTotal;
    d13Product = (d13Product + 5L) / 10L;
    lPercent = (LONG) d13Product;
    if ((lPercent < 0)||(lPercent > RPT_OVERFLOW)) {
        return(RPT_OVERFLOW);                            /* Print ******% */
   }

   return(lPercent);
}

/**** End of File ****/
