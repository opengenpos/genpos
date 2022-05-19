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
* Title       : Service Time Report Module
* Category    : Report For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: RPTSERV.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               RptServiceDayRead()      : Read Daily Service Time File
*               RptServicePTDRead()      : Read PTD Service Time File
*                   RptServiceAllRead()  :
*                   RptServiceAllEdit()
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Dec-04-95: 03.01.00 :  M.Ozawa  : initial                                   
* Aug-12-99: 03.05.00   : K.Iwata   : marge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
** GenPOS
* Jul-11-15 : 02.02.02 : R.Chambers : improve comments, use RflGetTranMnem().
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
#include <stdlib.h>
#include <ecr.h>
#include <uie.h>
#include <pif.h>
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
#include <mld.h>
#include <mldsup.h>
#include <plu.h>

#include "rptcom.h"

/*
==============================================================================
;                      C O M M O N   R A M   A R E A                         
;=============================================================================
*/

static VOID RptServiceConvertTime( LONG *plMinute, CHAR *pchSecond, LONG lTime );
static BOOL RptServiceChkTotal( TOTAL *pTotal );
static DCURRENCY RptServiceCalcAverage( TOTAL *pTotal );

/*
*=============================================================================
**  Synopsis: SHORT RptServiceDayRead(UCHAR uchMinor, UCHAR uchType) 
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
**  Description: Read Daily Service Time Report.  Service Time report is only available
*                with a Store Recall System type.
*
*                See function MaintEntSup() where a check with RptPTDChkFileExist(FLEX_GC_ADR)
*                is done for action codes AC_SERVICE_READ_RPT and AC_SERVICE_RESET_RPT.
*===============================================================================
*/
SHORT RptServiceDayRead(UCHAR uchMinor, UCHAR uchType) 
{
    return(RptServiceAllRead(uchMinor, uchType));
}

/*
*=============================================================================
**  Synopsis: SHORT RptServicePTDRead( UCHAR uchMinor, UCHAR uchType ) 
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
**  Description: Read PTD Service Time Report. Service Time report is only available
*                with a Store Recall System type.
*
*                See function MaintEntSup() where a check with RptPTDChkFileExist(FLEX_GC_ADR)
*                is done for action codes AC_SERVICE_READ_RPT and AC_SERVICE_RESET_RPT.
*===============================================================================
*/
SHORT RptServicePTDRead(UCHAR uchMinor, UCHAR uchType) 
{
    return(RptServiceAllRead(uchMinor, uchType));
}

/*
*=============================================================================
**  Synopsis: SHORT RptServiceAllRead(UCHAR uchMinorClass, UCHAR uchType) 
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
**  Description: Read All Service Time Report.  Service Time report is only available
*                with a Store Recall System type.
*
*                See function MaintEntSup() where a check with RptPTDChkFileExist(FLEX_GC_ADR)
*                is done for action codes AC_SERVICE_READ_RPT and AC_SERVICE_RESET_RPT.
*===============================================================================
*/
SHORT RptServiceAllRead(UCHAR uchMinorClass, UCHAR uchType) 
{
    
    UCHAR           uchRptType;
    UCHAR           uchRptName;
    UCHAR           uchSpecMnemo;
    UCHAR           uchACNumber;
    SHORT           sError;
    SHORT           sReturn;
	TTLSERTIME      TtlData = {0};
    
    /* EOD PTD Report Not Display Out MLD */
    if((uchMinorClass == CLASS_TTLSAVPTD) || (uchMinorClass == CLASS_TTLSAVDAY)){
        uchUifACRptOnOffMld = RPT_DISPLAY_OFF;
    }

    /* Get Hourly Time Activity Total */
    TtlData.uchMajorClass = CLASS_TTLSERVICE;
    TtlData.uchMinorClass = uchMinorClass;
    if ((sError = SerTtlTotalRead(&TtlData)) != TTL_SUCCESS) {
        return(TtlConvertError(sError));
    }

    /* Check Hourly Time Block */
    if (RptHourGetBlock() < 1) {
        return(LDT_PROHBT_ADR);
    }

    if (uchMinorClass == CLASS_TTLCURDAY || uchMinorClass == CLASS_TTLCURPTD) {
		TtlGetNdate (&TtlData.ToDate);      /* Get Current Date/Time and  Set Period To Date */
    }

    RptPrtStatusSet(uchMinorClass);                         /* Set J|R print status */

     /* Select Special Mnemonics for Header */
    switch(uchMinorClass) {
    case CLASS_TTLCURDAY :                                  /* Case of Daily READ */
        uchSpecMnemo = SPC_DAIRST_ADR;                      /* Set Daily Special Mnemonics */
        uchRptName = RPT_READ_ADR;                          /* Set Read Mnemonics */        
        uchACNumber = AC_SERVICE_READ_RPT;                  /* read report */
        break;
   
    case CLASS_TTLSAVDAY :                                  /* Case of Daily RESET */
        uchSpecMnemo = SPC_DAIRST_ADR;                      /* Set Daily Special Mnemonics */
        uchRptName = RPT_RESET_ADR;                         /* Set Reset Mnemonics */
        uchACNumber = AC_SERVICE_RESET_RPT;                 /* read report */
        break;

    case CLASS_TTLCURPTD :                                  /* Case of PTD READ */
        uchSpecMnemo = SPC_PTDRST_ADR;                      /* Set PTD Special Mnemonics */
        uchRptName = RPT_READ_ADR;                          /* Set Read Mnemonics */
        uchACNumber = AC_SERVICE_READ_RPT;                  /* read report */
        break;

    default:                                                /* Case of PTD RESET */
        uchSpecMnemo = SPC_PTDRST_ADR;                      /* Set PTD Special Mnemonics */
        uchRptName = RPT_RESET_ADR;                         /* Set Reset Mnemonics */
        uchACNumber = AC_SERVICE_RESET_RPT;                 /* read report */
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
                    RPT_SERVICE_ADR,                        /* Report Name Address */
                    uchSpecMnemo,                           /* Special Mnemonics Address */
                    uchRptName,                             /* Report Name Address */
                    uchRptType,                             /* Report Type */
                    uchACNumber,                            /* A/C Number */
                    0,                                      /* Reset Type */
                    PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit */                                       
    
    /* Print Period/To Date and Time */
    RptPrtTime(TRN_PFROM_ADR, &TtlData.FromDate);         
    RptPrtTime(TRN_PTO_ADR, &TtlData.ToDate);             
    RptFeed(RPT_DEFALTFEED);                                /* Feed One Line */

    if (uchType == RPT_ALL_READ || uchType == RPT_EOD_ALLREAD || uchType == RPT_PTD_ALLREAD) {
        /* Edit Total Report */
        if ((sReturn = RptServiceAllEdit(&TtlData)) == RPT_ABORTED) {    /* Aborded By User */
            MaintMakeAbortKey();                                        /* Print ABORDED */
            MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);                /* Print Supervisor Trailer */
            return(sReturn);
        } else {
            /* Set Issued Flag for Reset Report */
            if ((uchMinorClass == CLASS_TTLSAVDAY) || 
                (uchMinorClass == CLASS_TTLSAVPTD)) {
                SerTtlIssuedReset(&TtlData, TTL_NOTRESET);
            }
        }
    }

    if (uchType != RPT_EOD_ALLREAD && uchType != RPT_PTD_ALLREAD) {                       /* Not EOD/PTD Function */
        MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);        /* Print Supervisor Trailer */
    } 
    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT RptServiceAllEdit(TTLSERTIME *pTtlData)
*               
*       Input:  pTtlData    : Pointer to Structure for TTLSERTIME
*      Output:  Nothing
*
**  Return: SUCCESS         : Successful  
*           RPT_ABORTED     : Aborted by User
*
**  Description: Prepare Print All Service Time Report. Service Time report is only available
*                with a Store Recall System type.
*
*                See function MaintEntSup() where a check with RptPTDChkFileExist(FLEX_GC_ADR)
*                is done for action codes AC_SERVICE_READ_RPT and AC_SERVICE_RESET_RPT.
*===============================================================================
*/

SHORT RptServiceAllEdit(TTLSERTIME *pTtlData)
{

    USHORT          i;
    USHORT          usBlockNo = RptHourGetBlock();
    RPTREGFIN       RegFin;
	TOTAL           AddTtlService[3] = {0}, DummyTtl[3] = {0};

    /* Print Each Hourly Activity Block Total */
    for (i = 0; i < usBlockNo; i++) {
		PARAHOURLYTIME  ParaHourly = {0};

        if (UieReadAbortKey() == UIE_ENABLE) {          /* Putting Abort Key */                         
            return(RPT_ABORTED);
        }
        if(RptPauseCheck() == RPT_ABORTED){
             return(RPT_ABORTED);
        }

        /* Check Execute Print or Not based on MDC and totals values. */
        if ( ! RptServiceChkTotal(pTtlData->ServiceTime[i]) ) {        /* Not Execute Print */
            continue;
        }

        /* Get Time of Hourly Activity Block */
		ParaHourly.uchMajorClass = CLASS_PARAHOURLYTIME;
        ParaHourly.uchAddress = ( UCHAR)(i + HOUR_TSEG1_ADR);  // calculate correct address for this block
        CliParaRead(&ParaHourly);

        /* Print Time of Hourly Activity Block */
        RptHourlyElement(CLASS_RPTHOURLY_PRTTIME,               /* Set Date  for Print Out      */
                         0,                                     /* |--------------------------| */
                         ( USHORT)ParaHourly.uchMin,            /* |                  Z9:99AM | */
                         ( USHORT)ParaHourly.uchHour,           /* |--------------------------| */
                         0,
                         0L);

        /* Print Hourly Activity Each Element */
        RptServiceTtlEdit(pTtlData->ServiceTime[i], AddTtlService);
    }

    /* Check if aborted by user or Not */
    if (uchRptMldAbortStatus) {
        return(SUCCESS);                           /* aborted by MLD */
    }

    /* Check Execute Print or Not based on MDC and totals values. */
    if (!RptServiceChkTotal(AddTtlService)) {        /* Not Execute Print */
        return(SUCCESS);
    }

    /* Print TOTAL Mnemonics */
    RegFin.uchMajorClass = CLASS_RPTREGFIN;
    RegFin.uchMinorClass = CLASS_RPTREGFIN_MNEMO;
    RegFin.usPrintControl = usRptPrintStatus;
    RflGetTranMnem (RegFin.aszTransMnemo, TRN_TTLR_ADR);       /* "TOTAL" */

    if(uchUifACRptOnOffMld == RPT_DISPLAY_ON){
        uchRptMldAbortStatus = (UCHAR)MldDispItem(&RegFin, 0); /* display on LCD          */ 
        RegFin.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
    }
    PrtPrintItem(NULL, &RegFin);

    /* Print Hourly Activity Total Each Element */
    if (uchRptMldAbortStatus) {
        return(SUCCESS);                           /* aborted by MLD */
    }

    RptServiceTtlEdit(AddTtlService, DummyTtl);

    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: VOID RptServiceTtlEdit( TOTAL *pTotal,
*                                     TOTAL *pAllTotal);
*               
*       Input:  *pTotal             : structer of service time total
*      Output:  *pAllTotal          : structer of total of service time total
*
**  Return: Nothing  
*
**  Description: Print Service Time Each Element and accumulate sum of all totals
*                while doing so if pAllTotal is not NULL.
*===============================================================================
*/

VOID RptServiceTtlEdit(TOTAL *pTotal, TOTAL *pAllTotal)
{
    RPTSERVICE  RptService = {0};
	TOTAL       ElementTtl = {0};
    LONG        lAverage;
    SHORT       i;

    if (uchRptMldAbortStatus) return;                       /* aborted by MLD */

    RptService.uchMajorClass = CLASS_RPTSERVICE;
    RptService.usPrintControl = usRptPrintStatus;
	// There are three Service blocks and there may be a border amount between
	// Service Block #1 and Service Block #2 as well as a border between Service
	// Block #2 and Service Block #3. See AC133 Service Time Parameters.
    for (i = 1; i <= 3; i++) {

        if (uchRptMldAbortStatus) return;                       /* aborted by MLD */

        if (CliParaMDCCheck(MDC_HOUR_ADR, EVEN_MDC_BIT0)) {     /* not print if border time is not set */
            if (i > 1) {
				PARASERVICETIME  ParaServTime = {0};

				ParaServTime.uchMajorClass = CLASS_PARASERVICETIME;
                ParaServTime.uchAddress = (UCHAR)(i-1);
                CliParaRead(&ParaServTime);                              /* call ParaServiceTimeRead() */
                if (ParaServTime.usBorder == 0) {
                    if ((pTotal->sCounter == 0) && (pTotal->lAmount == 0)) {
                        continue;
                    }
                }
            }
        }

        RptService.uchMinorClass = CLASS_RPTSERVICE_TTL;
        RptService.sElement = i;
        RptService.sCounter = pTotal->sCounter;
        lAverage = RptServiceCalcAverage(pTotal);
        RptServiceConvertTime(&RptService.lMinute, &RptService.chSecond, lAverage);

        /* Output Data to Print Module */

        if(uchUifACRptOnOffMld == RPT_DISPLAY_ON){
            uchRptMldAbortStatus = (UCHAR)MldDispItem(&RptService, 0); /* display on LCD          */ 
            RptService.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
        }
        PrtPrintItem(NULL, &RptService);

        ElementTtl.sCounter += pTotal->sCounter;
        ElementTtl.lAmount += pTotal->lAmount;

		if (pAllTotal) {
			// pAllTotal is not NULL so sum up the totals in user supplied totals
			pAllTotal->sCounter += pTotal->sCounter;
			pAllTotal->lAmount += pTotal->lAmount;
			pAllTotal++;
		}

        pTotal++;
    }

    if (uchRptMldAbortStatus) return;                       /* aborted by MLD */

    RptService.uchMinorClass = CLASS_RPTSERVICE_SEGTTL;
    RptService.sCounter = ElementTtl.sCounter;
    lAverage = RptServiceCalcAverage(&ElementTtl);
    RptServiceConvertTime(&RptService.lMinute, &RptService.chSecond, lAverage);

    /* Output Data to Print Module */

    if(uchUifACRptOnOffMld == RPT_DISPLAY_ON){
        uchRptMldAbortStatus = (UCHAR)MldDispItem(&RptService, 0);  /* display on LCD          */ 
        RptService.usPrintControl &= PRT_JOURNAL;             /* Reset Receipt print status so only goes to Electronic Journal if set */
    }
    PrtPrintItem(NULL, &RptService);

    RptFeed(RPT_DEFALTFEED);                                /* Feed Line */

}

/*
*=============================================================================
**  Synopsis: SHORT RptServiceDayReset(UCHAR uchFunc) 
*               
*       Input:  uchFunc     : RPT_PRT_RESET ------ Report and Reset
*                           : RPT_ONLY_RESET ----- Only Reset
*      Output:  nothing
*
**     Return:  SUCCESS         : Successful 
*               LDT_ERR_ADR     : (TTL_NOT_ISSUED)
*
**  Description:  
*       Reset Daily Service Time File.Service Time report is only available
*       with a Store Recall System type.
*
*       See function MaintEntSup() where a check with RptPTDChkFileExist(FLEX_GC_ADR)
*       is done for action codes AC_SERVICE_READ_RPT and AC_SERVICE_RESET_RPT.
*===============================================================================
*/
SHORT RptServiceDayReset(UCHAR uchFunc) 
{
    /* execute Current Daily Reset Report */

    return(RptServiceReset(CLASS_TTLCURDAY, uchFunc));
}

/*
*=============================================================================
**  Synopsis: SHORT RptServicePTDReset(UCHAR uchFunc) 
*               
*       Input:  uchFunc     : RPT_PRT_RESET  ------ Report and Reset
*                           : RPT_ONLY_RESET  ----- Only Reset
*      Output:  nothing
*
**     Return:  SUCCESS         : Successful 
*               LDT_ERR_ADR     : (TTL_NOT_ISSUED)
*
**  Description:  
*       Reset PTD Service File.  Service Time report is only available
*       with a Store Recall System type.
*
*       See function MaintEntSup() where a check with RptPTDChkFileExist(FLEX_GC_ADR)
*       is done for action codes AC_SERVICE_READ_RPT and AC_SERVICE_RESET_RPT.
*===============================================================================
*/
SHORT RptServicePTDReset(UCHAR uchFunc) 
{
    /* execute Current PTD Reset Report */
    return(RptServiceReset(CLASS_TTLCURPTD, uchFunc));
}

/*
*=============================================================================
**  Synopsis: SHORT RptServiceReset(UCHAR uchMinorClass, UCHAR uchFunc) 
*               
*       Input:  uchMinorClass   : CLASS_TTLSAVDAY
*                               : CLASS_TTLSAVPTD
*               uchFunc         : RPT_PRT_RESET
*                               : RPT_ONLY_RESET
*      Output:  nothing
*
**     Return:  SUCCESS             : Successful 
*               LDT_ERR_ADR         : (TTL_NOT_ISSUED)
*
**  Description:  
*        Reset Daily or PTD Service Time File.  Service Time report is only available
*        with a Store Recall System type.
*
*        See function MaintEntSup() where a check with RptPTDChkFileExist(FLEX_GC_ADR)
*        is done for action codes AC_SERVICE_READ_RPT and AC_SERVICE_RESET_RPT.
*===============================================================================
*/
SHORT RptServiceReset(UCHAR uchMinorClass, UCHAR uchFunc) 
{
    UCHAR   uchSpecReset, uchSpecMnemo;
    SHORT   sReturn;
	TTLSERTIME  TtlService = {0};

    /*----- Reset All PLU Total -----*/
    TtlService.uchMajorClass = CLASS_TTLSERVICE;
    TtlService.uchMinorClass = uchMinorClass;
    if ((sReturn = SerTtlIssuedCheck(&TtlService, TTL_NOTRESET)) != TTL_ISSUED) {
        return(TtlConvertError(sReturn));
    }
    
    if ((sReturn = SerTtlTotalReset(&TtlService,0)) != TTL_SUCCESS) {   
        return(TtlConvertError(sReturn));
    }
    if (uchMinorClass == CLASS_TTLCURDAY) {
        uchSpecReset = SPCO_EODRST_ADR;                         /* Set Daily Reset Counter */
        uchSpecMnemo = SPC_DAIRST_ADR;                          /* Set Daily Special Mnemonics */
    } else {
        uchSpecReset = SPCO_PTDRST_ADR;                         /* Set PTD Reset Counter */
        uchSpecMnemo = SPC_PTDRST_ADR;                          /* Set PTD Special Mnemonics */
    }
    MaintIncreSpcCo(uchSpecReset);                          /* Count Up Reset Counter */

    /*----- Only Reset Case -----*/
    if (uchFunc == RPT_ONLY_RESET) {
        /*----- Print Header Name -----*/
        MaintIncreSpcCo(SPCO_CONSEC_ADR);                       /* Count Up Consecuteve Counter */
        MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class */              
                        RPT_ACT_ADR,                            /* Report Name Address */
                        RPT_SERVICE_ADR,                        /* Report Name Address */
                        uchSpecMnemo,                           /* Special Mnemonics Address */
                        RPT_RESET_ADR,                          /* Report Name Address */
                        0,                                      /* Not Print */
                        AC_SERVICE_RESET_RPT,                   /* A/C Number */
                        uchFunc,                                /* Reset Type */
                        PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit         */

                                                                   
        MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);            /* Print Trailer */
    }
    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: BOOL RptServiceChkTotal( TOTAL *pTotal )
*               
*       Input:  pAddTtl     : Pointer to Structure for TOTAL
*      Output:  Nothing
*
**  Return:     0: Not Execute Print 
*               1: Execute Print
*
**  Description: Check Total/Counter and MDC to decide Print Report.  
*===============================================================================
*/
static BOOL RptServiceChkTotal( TOTAL *pTotal )
{
    SHORT i;

    /* Check MDC to see if check for all zero totals is needed and if so indicate if there
	 * is at least one non-zero value in the total array for either the amount or
	 * the count.
	*/
    if (CliParaMDCCheck(MDC_HOUR_ADR, EVEN_MDC_BIT0)) {
        for (i = 0; i < 3; i++) {
            if ((pTotal->sCounter) || (pTotal->lAmount)) return(1);  // we found at least one non-zero total in the array of totals.
            pTotal++;
        }
        return(0);       // no non-zero totals were found
    } else {
        return(1);
    }
}

/*
*=============================================================================
**  Synopsis: LONG RptServiceCalcAverage( TOTAL *pTotal )
*               
*       Input:  pTotal     : Pointer to Structure for TOTAL 
*      Output:  Nothing
*
**  Return:     result of calculation
*
**  Description: Calculate lAmount/sCounter
*===============================================================================
*/
static DCURRENCY RptServiceCalcAverage( TOTAL *pTotal )
{
    if (pTotal->sCounter) {
        return(RptRound(pTotal->lAmount, pTotal->sCounter));
    } else {
        return(pTotal->lAmount);
    }
}

/*
*=============================================================================
**  Synopsis: VOID RptServiceConvertTime( LONG *plMinute, CHAR *pchSecond, LONG lTime ))
*               
*       Input:  LONG lTime      : Service time total
*      Output:  LONG *lMinute   : Service time total(minute)
*               CHAR *chSecond  : Service Time total(second)
*
**  Return:     nothing
*
**  Description: convert service time total to MM:SS
*===============================================================================
*/
static VOID RptServiceConvertTime( LONG *plMinute, CHAR *pchSecond, LONG lTime )
{
    *pchSecond = (CHAR)labs(lTime % 60);
    *plMinute = (lTime / 60);
}

/*
*=============================================================================
**  Synopsis: SHORT RptServiceLock(VOID) 
*               
*       Input:  nothing
*      Output:  nothing
*
**     Return:  SUCCESS             : Successful 
*               LDT_LOCK_ADR        : (CAS_ALREADY_LOCK)
*               LDT_ERR_ADR         : (CAS_DURING_SIGNIN) 
*
**  Description:  
*       Lock Cashier and Waiter File.
*       If File couldn't Lock, Unlock the File(s).    
*===============================================================================
*/
SHORT RptServiceLock(VOID)    
{
    return (RptPLULock());  /* same as plu lock */
}

/*
*=============================================================================
**  Synopsis: VOID RptServiceUnLock(VOID) 
*               
*       Input:  nothing
*      Output:  nothing
*
**     Return:  nothing 
*
**  Description:  
*       UnLock Cashier and Waiter File.    
*===============================================================================
*/
VOID  RptServiceUnLock(VOID)    
{
    RptPLUUnLock();    /* same as plu unlock */ /* ### Mod (2171 for Win32) V1.0 */
}

/**** End of File ****/
