/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1997            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : ETK Report Module
* Category    : Report For Supervisor, NCR 2170 US Hospitality Application
* Program Name: RPTETK.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:
*
*               RptETKRead()          : Read ETK Report Module
*                  RptETKEdit()       : Edit ETK Report
*               RptETKReset()         : Reset ETK Report Module
*                  RptETKAllReset()   : Reset ETK all
*                  RptEtkAllOnlyReset() : Only reset all etk
*                  RptEtkAllRstReport() : Reset and report all etk
*               RptETKLock()          : Lock and Save ETK No. Module
*               RptETKUnLock()        : UnLock ETK Module
*                    RptEtkPrint()         :
*                    RptDateAndTimeElement() : Date And Time Print
*                    RptCalWorkTime()      : Calucuration Work Time
*                    RptRndWorkTime()      : Rounding Work Time
*                    RptResetTimePrint()   : Reset Time Print
*                    RptEmployeeNoPrint()  : Employee No Print
*
* Explanations about this file.
*
*
* If you are adding a new class or data structure, you should be aware of the following:
*  The class types used in the struct RPTEMPLOYEE come from report.h
*  The defines for CLASS_ types are in paraequ.h
*  The function PrtPrintItem in prcitemt.c and prcitem_.c needs to be modified
*     to handle any new CLASS_ types you create.  These two files are in
*     BusinessLogic\Print in separate areas for regular and thermal printers.
*
* You need to be aware that there are two main types of printers used with NHPOS
* regular (impact or dot matrix printers) and thermal printers.  The printer code
* is designed to isolate the source code at this level with the lower level
* printer interfaces.  This way you don't care if you are talking to an impact
* or a thermal printer.
*
* --------------------------------------------------------------------------
* Update Histories
*    Date   : Ver.Rev.   :   Name    : Description
* Jun-23-93 : 00.00.01   : M.Yamamoto: initial
* Jul-27-95 : 03.00.03   : M.Ozawa   : Add Report on Display
* Nov-27-95 : 03.01.00   : M.Ozawa   : Add mnemonic report
* Apr-18-96 : 03.01.04   : M.Ozawa   : Add wage calculation by hourly block
* Oct-21-97 : 03.01.15   : hrkato    : ETK Bug Fix
* 2171 for Win32
* Aug-26-99 : 01.00.00   : K.Iwata   : V1.0 Initial
* Jul-11-15 : 02.02.02   : R.Chambers : localize variables, ulEtkNo to sEtkNo in loops, use TtlGetNdate()
*===========================================================================
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
*       2016  -> GenPOS Rel 2.3.0 (Windows 7, completely Out of Scope, Visual Studio 2013.)
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
*    moved from Visual Studio 2005 to Visual Studio 2013 with Rel 2.3.0
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

#include <tchar.h>
#include <string.h>

#include <ecr.h>
#include <uie.h>
#include <pif.h>
#include <rfl.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <maint.h>
#include <csop.h>
#include <csttl.h>
#include <csstbttl.h>
#include <regstrct.h>
#include <transact.h>
#include <csetk.h>
#include <csstbetk.h>
#include <prt.h>
#include <report.h>
#include <mld.h>
#include <mldsup.h>
#include <plu.h>

#include "rptcom.h"

/*
==============================================================================
                      C O M M O N  R A M   E R E A s
==============================================================================
*/

static VOID RptETKCalHourlyTotalSub(LONG ulEmployeeNo,
                        UCHAR uchMinorClass, 
                        N_DATE *pFromDate, N_DATE *pToDate,
                        RPTHOURLYETK *pRptHourlyEtk);
static VOID RptETKCalHourlyTotalField(RPTETKWORKFIELD *pWorkEtk,
                        RPTHOURLYETK *pRptHourlyEtk);
static ULONG   RptETKCalHourlyTotalSub1(DATE_TIME *pDate);

static VOID    RptETKCalHourlyTotalSub2(USHORT usWorkMinute, USHORT i, 
                RPTETKWORKFIELD *pWorkEtk, RPTHOURLYETK *pRptHourlyEtk);

static SHORT   RptEtkPrint(UCHAR uchType, RPTEMPLOYEE *pRptEmployee, ULONG ulEmployeeNo);
static VOID    RptDateAndTimeElement( UCHAR uchMinorClass, RPTEMPLOYEE *pRptEmployee);
static VOID    RptCalWorkTime( RPTEMPLOYEE *RptEmployee, USHORT *usWorkTime, USHORT *usWorkMinute);
static VOID    RptRndWorkTime( USHORT *pusWorkTime, USHORT *pusWorkMinute);
static VOID    RptResetTimePrint( ULONG ulEmployeeNo, UCHAR uchMinorClass );
static VOID    RptEmployeeNoPrint( ULONG ulEmployeeNo );
static VOID    RptCalTotalTime( RPTEMPLOYEE *pRptEmployee );
static VOID    RptCalWageTotal(UCHAR  uchJobCode, USHORT usWorkTime, USHORT usWorkMinute, ULONG  *pulWorkWage, USHORT *pusLaborCost);

/*
*=============================================================================
**  Synopsis: SHORT RptETKRead(UCHAR uchMinorClass,
*                                    UCHAR uchType,
*                                    ULONG ulEmployeeNo,
*                                    UCHAR uchResetType)
*
*       Input:  UCHAR   uchMinorClass : CLASS_TTLCURDAY
*                                     : CLASS_TTLSAVDAY
*               UCHAR   uchType       : RPT_ALL_READ
*                                     : RPT_IND_READ
*               ULONG   ulEmployeeNo  :
*               UCHAR   uchResetType  : 0
*                                     : RPT_ONLY_PRT_RESET
*                                     : RPT_PRT_RESET
*      Output:  nothing
*
**     Return:  SUCCESS             : Successful
*               RPT_ABORTED         : Aborted by User
*
**  Description:
*       Read the etk report individualy or all.
*       Prepare for report Header status and print out Header.
*       Edit ETK Read Report A/C 21 and ETK Reset Report A/C 19 Type 1,
*       and Type 2 (Report).
*       Print out Trailer.
*       There is no Header printing, if the etk file does't exist.
*==============================================================================
*/

SHORT RptETKRead(UCHAR uchMinorClass,
                 UCHAR uchType,
                 ULONG ulEmployeeNo,
                 UCHAR uchResetType) 
{
    UCHAR       uchACNo, uchRdRst;
    SHORT       sReturn = 0;
    SHORT       sNo = 0;
	ULONG       aulRcvBuffer[FLEX_ETK_MAX] = {0};

    /* Print Control Set */
    RptPrtStatusSet(uchMinorClass);                                   /* Set J|R print status */

    if (uchMinorClass == CLASS_TTLCURDAY)  {                    /* Read  report ? */
        uchACNo = AC_ETK_READ_RPT;                              /* Set A/C number */
        uchRdRst = RPT_READ_ADR;                                /* Read Type      */
    } else {                                                    /* Reset report ? */
        uchACNo = AC_ETK_RESET_RPT;                             /* Set A/C number */
        uchRdRst = RPT_RESET_ADR;                               /* Reset Type */
    }

    if (uchType == RPT_IND_READ) {                              /* Ind. read report ? */
		ETK_FIELD   EtkField = {0};
		ETK_TIME    EtkTime = {0};
		USHORT      usFieldNo = 0;     // first field only as this is a test to see if ETK exists.

        if ((sReturn = SerEtkCurIndRead(ulEmployeeNo, &usFieldNo, &EtkField, &EtkTime)) != ETK_SUCCESS) { /* File exist ?  */
            if ((sReturn != ETK_CONTINUE) && (sReturn != ETK_NOT_IN_FIELD)) {
                return( EtkConvertError(sReturn) );
            }
        }
		// set up for printing a single employee time report
		sNo = 1;
		aulRcvBuffer[0] = ulEmployeeNo;
    } else if (uchType == RPT_ALL_READ) {                       /* all read ?     */
        if (uchMinorClass == CLASS_TTLCURDAY)  {                /* Read  report ? */
            if ((sNo = SerEtkCurAllIdRead(sizeof(aulRcvBuffer), aulRcvBuffer)) < 0) {
                return( EtkConvertError(sNo) );
            }
        } else {
            if ((sNo = SerEtkSavAllIdRead(sizeof(aulRcvBuffer), aulRcvBuffer)) < 0) {
                return( EtkConvertError(sNo) );
            }
        }
    }

    /* Print Header Name */
    MaintIncreSpcCo(SPCO_CONSEC_ADR);                   /* Count Up Consecutive No. */
    MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class */
                    RPT_ACT_ADR,                            /* Report Name Address */
                    RPT_ETK_ADR,                            /* Report Name Address */
                    SPC_DAIRST_ADR,                         /* Special Mnemonics Address */
                    uchRdRst,                               /* Report Name Address */
                    uchType,                                /* Report Type */
                    uchACNo,                                /* A/C Number */
                    uchResetType,                           /* Reset Type */
                    PRT_RECEIPT | PRT_JOURNAL); /* Print Control Bit */

    /* Edit Total Report */
    RptResetTimePrint( ulEmployeeNo, uchMinorClass );

	{
		RPTEMPLOYEE  RptEmployee = {0};
		SHORT        sEtkNo;

        RptEmployee.uchMinorClass = CLASS_RPTEMPLOYEE_PRTTOTAL;
        for (sEtkNo = 0; sEtkNo < sNo; sEtkNo++) {
            if ((sReturn = RptETKEdit(uchMinorClass, &RptEmployee, aulRcvBuffer[sEtkNo])) == RPT_ABORTED) {
                MaintMakeAbortKey();                                            /* Print ABORTED */
                MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);                    /* Print Trailer */
                return(RPT_ABORTED);
            }

			// keep the grand total adjusted by rounding to nearest 15 minute interval if indicated by MDC 95.
            RptRndWorkTime(&RptEmployee.usTotalTime, &RptEmployee.usTotalMinute);
        }
        if (uchType == RPT_ALL_READ && sNo > 0) {
			// print out the grand totals summary line for an All ETK report.
            RptFeed(RPT_DEFALTFEED);
            RptDateAndTimeElement(RptEmployee.uchMinorClass, &RptEmployee);
        }
    }

    if (uchMinorClass == CLASS_TTLSAVDAY) {
        SerEtkIssuedSet();
    }
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);                            /* Print Trailer */
    return(sReturn);
}

/*
*=============================================================================
**  Synopsis: SHORT RptETKEdit(UCHAR uchMinorClass,
*                              RPTEMPLOYEE pRptEmployee, ULONG ulEmployeeNo)
*               
*       Input:  UCHAR    uchMinorClass
*               ULONG    ulEmployeeNo
*               RPTEMPLOYEE RptEmployee
*      Output:  
*
**     Return:  SUCCESS         : Successful
*               RPT_ABORTED     : Aborted by User
*               RPT_END         : Reported
*
**  Description:
*       Set the data for an element of print format each time.
*       This function is just same shape of printing format.
*===============================================================================
*/
SHORT RptETKEdit(UCHAR uchMinorClass, RPTEMPLOYEE *pRptEmployee,
                 ULONG ulEmployeeNo)
{
	USHORT   usField = 0;
	SHORT    sStatus;

    /*----- Print ETK Mnimonics and Number -----*/
    if (UieReadAbortKey() == UIE_ENABLE) {                      /* if Abort ?      */
        return(RPT_ABORTED);
    }
    if(RptPauseCheck() == RPT_ABORTED){
        return(RPT_ABORTED);
    }
    
    if (CliParaMDCCheck(MDC_ETK_ADR, ODD_MDC_BIT2)) {             /* Check MDC#95-BIT2  */
		ETK_FIELD   EtkField = {0};
		ETK_TIME    EtkTime = {0};
		USHORT      usFieldNo = 0;     // first field only as this is a test to see if ETK exists.

        if (uchMinorClass == CLASS_TTLCURDAY) {
            sStatus = SerEtkCurIndRead(ulEmployeeNo, &usFieldNo, &EtkField, &EtkTime);
        } else {
            sStatus = SerEtkSavIndRead(ulEmployeeNo, &usFieldNo, &EtkField, &EtkTime);
        }
        if (sStatus == ETK_NOT_IN_FIELD) {
            return(SUCCESS);
        }
    }

    RptFeed(RPT_DEFALTFEED);                                    /* Feed            */
    RptEmployeeNoPrint( ulEmployeeNo );

    if ((sStatus = RptEtkPrint(uchMinorClass, pRptEmployee, ulEmployeeNo)) == RPT_ABORTED) { /* ### MOD (2172 Rel.0) */
        return(RPT_ABORTED);
    } else if (sStatus != ETK_CONTINUE) { /* ### MOD (2172 Rel.0) */
        return(RPT_END);
    }
    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT RptETKReset(UCHAR uchType, UCHAR uchFunc) 
*               
*       Input:  UCHAR  uchType  : RPT_IND_RESET
*                               : RPT_ALL_RESET
*                               : RPT_OPEN_RESET
*                               : RPT_EODALL_RESET 
*               UCHAR  uchFunc  : RPT_PRT_RESET
*                               : RPT_ONLY_RESET
*      Output:  nothing
*
**     Return:  sReturn  : SUCCESS
*                          FAIL (FILE NOT FOUND)
*
**  Description:  
*       Check Individual, All or Open reset.
*===============================================================================
*/

SHORT RptETKReset(UCHAR uchType, UCHAR uchFunc) 
{                   
    SHORT sReturn;                

    /* Print Control Set */
    RptPrtStatusSet(CLASS_TTLSAVDAY);                             /* Set J|R print status */

    sReturn = RptETKAllReset(uchType, uchFunc); 
    return(sReturn);
}

/*
*=============================================================================
**  Synopsis: SHORT RptETKAllReset( UCHAR uchType,
*                                   UCHAR uchFunc)
*               
*       Input:  UCHAR  uchType  : RPT_ALL_RESET
*                               : RPT_EODALL_RESET
*               UCHAR  uchFunc  : RPT_PRT_RESET
*                               : RPT_ONLY_RESET
*      Output:  nothing
*
**  Return: TTL_SUCCESS             : Successful 
*           TTL_FAIL                : not success   
*           TTL_NOTINFILE           :
*           TTL_NOT_ISSUED          :
*           TTL_LOCKED              :
*
**  Description:  
*       Reset for ETK All Reset A/C 15 Function Type 2 or 3, Report Type 1.
*       Check Error before Header printing.  Classify Only Reset or Reset & Report.
*===============================================================================
*/

SHORT RptETKAllReset(UCHAR uchType, UCHAR uchFunc)
{
    SHORT           sReturn;

    /* Check Saved buffer already print out */
    if ((sReturn = SerEtkIssuedCheck()) != ETK_ISSUED) {
        return(EtkConvertError(sReturn));                           /* Error   */
    }
    if (uchFunc == RPT_ONLY_RESET) {                                /* Only Reset Report ? */
        sReturn = RptETKAllOnlyReset(uchType, uchFunc);
    } else {    /*----- RPT_PRT_RESET -----*/                       /* Reset and Report ? */
        sReturn = RptETKAllRstReport(uchType, uchFunc);
    }  
    return(sReturn);                                                /* Return SUCCESS      */
}

/*
*=============================================================================
**  Synopsis: SHORT RptETKAllOnlyReset(UCHAR uchType,
*                                      SHORT sNo)
*
*       Input:  UCHAR   uchType
*               SHORT   sNo
*               USHORT  *ausResetBuffer
*
*      Output:  *ausResetBuffer
*
**  Return: SUCCESS         : Reset Success
*         : RPT_RESET_FAIL  : Reset Failed in EOD Etk
*
**  Description:  
*       Only Reset All etk files.
*       Prepare data for Reset Header and print out.
*       Print out Reset Header.  Control Reset Fail Error - if SerEtkReset fails,
*       Return Error to Caller When Reset Failed in EOD Etk.
*===============================================================================
*/
SHORT  RptETKAllOnlyReset(UCHAR uchType,
                          UCHAR uchFunc)
{
    UCHAR       uchRptType = uchType, uchResetType = uchFunc;
    SHORT       sEtkNo, sNo;
    SHORT       sStatus;
    ULONG       aulResetBuffer[FLEX_ETK_MAX];

    /* All ETK Reset */
    if ((sStatus = SerEtkAllReset()) < 0) {
        return(EtkConvertError(sStatus));                               /* Error   */
    }

    /* Print Header Name */
    MaintIncreSpcCo(SPCO_CONSEC_ADR);                           /* Count Up Consecutive Counter */
    MaintMakeHeader(CLASS_MAINTHEADER_RPT,                          /* Minor Data Class    */
                    RPT_ACT_ADR,                                    /* Report Name Address */
                    RPT_ETK_ADR,                                    /* Report Name Address */
                    SPC_DAIRST_ADR,                                 /* Special Mnemonics Address */
                    RPT_RESET_ADR,                                  /* Report Name Address */
                    uchRptType,                                     /* Report Type         */
                    AC_ETK_RESET_RPT,                               /* A/C Number          */
                    uchResetType,                                   /* Reset Type          */
                    PRT_RECEIPT|PRT_JOURNAL);           /* Print Control Bit   */
    
    MaintIncreSpcCo(SPCO_EODRST_ADR);                           /* Reset counter up    */

    /* Check Existance of the etk file */
    if ((sNo = SerEtkSavAllIdRead(sizeof(aulResetBuffer), aulResetBuffer)) < 0) {     /* ETK File not exist ? */
        return(EtkConvertError(sNo));                               /* Error   */
    }

    for (sEtkNo = 0; sEtkNo < sNo; sEtkNo++) {                      /* Get All ETK files */
		RPTEMPLOYEE RptETK = {0};

		RptETK.uchMajorClass = CLASS_RPTEMPLOYEE;                       /* Set Major         */
        RptETK.ulEmployeeNumber = *(aulResetBuffer + sEtkNo);
        RptDateAndTimeElement(CLASS_RPTEMPLOYEE_RESET, &RptETK);    /* print out          */
    }

    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);                /* Print Trailer       */
    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT RptETKAllRstReport(UCHAR  uchType,
*                                      UCHAR  uchFunc)
*               
*       Input:  UCHAR   uchType         : RPT_ALL_RESET
*                                       : RPT_EODALL_RESET
*               UCHAR   uchFunc         : RPT_PRT_RESET
*               SHORT   sNo             :
*               USHORT  *ausResetBuffer :
*
*      Output:  USHORT  *ausResetBuffer
*
**  Return: SUCCESS         : Reset Success 
*         : RPT_RESET_FAIL  : Reset Failed in EOD Etke
*
**  Description:  
*       Reset All etk files.
*       Prepare data for Reset Header and print out.
*       Print out Reset Header.
*       print ETK name & ID and Error Number. The Secret code is clear
*===============================================================================
*/ 
SHORT  RptETKAllRstReport(UCHAR  uchType,
                          UCHAR  uchFunc)
{
    UCHAR        uchResetType = uchFunc, uchRptType = uchType;
    SHORT        sStatus;
    SHORT        sEtkNo, sNo;
	RPTEMPLOYEE  RptETK = {0};
    ULONG        aulResetBuffer[FLEX_ETK_MAX];

    /* Print Control Set */
    RptPrtStatusSet(CLASS_TTLSAVDAY);                                   /* Set J|R print status */

    /* All ETK Reset */
    if ((sStatus = SerEtkAllReset()) < 0) {
        return(EtkConvertError(sStatus));                           /* Error   */
    }
    /* Reset Issued */
    SerEtkIssuedSet();

    /* Check Existance of the etk file */
    if ((sNo = SerEtkSavAllIdRead(sizeof(aulResetBuffer), aulResetBuffer)) < 0) {    /* ETK File not exist ? */
        return(EtkConvertError(sNo));                               /* Error   */
    }

    RptETK.uchMajorClass = CLASS_RPTEMPLOYEE;                   /* Set Major      */
    RptETK.uchMinorClass = CLASS_RPTEMPLOYEE_PRTTOTAL;
    for (sEtkNo = 0; sEtkNo < sNo; sEtkNo++) { 
        RptETK.ulEmployeeNumber = aulResetBuffer[sEtkNo];
        if (!(uchRptOpeCount & RPT_HEADER_PRINT)) {                 /* First Operation Etke */
            MaintIncreSpcCo(SPCO_CONSEC_ADR);                           /* Count Up Consecutive Counter */
            MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class     */
                            RPT_ACT_ADR,                            /* Report Name Address  */
                            RPT_ETK_ADR,                            /* Report Name Address  */
                            SPC_DAIRST_ADR,                         /* Special Mnemonics Address */
                            RPT_RESET_ADR,                          /* Report Name Address  */
                            uchRptType,                             /* Report Type          */
                            AC_ETK_RESET_RPT,                      /* A/C Number           */
                            uchResetType,                           /* Reset Type           */
                            PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit    */
            uchRptOpeCount |= RPT_HEADER_PRINT;
            RptResetTimePrint( RptETK.ulEmployeeNumber, CLASS_TTLSAVDAY );
        }
        if (RptETKEdit(CLASS_TTLSAVDAY, &RptETK, aulResetBuffer[sEtkNo]) == RPT_ABORTED) {
            MaintMakeAbortKey();                                            /* Print ABORTED */
            MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);                    /* Print Trailer */
            return(RPT_ABORTED);
        }
		// keep the grand total adjusted by rounding to nearest 15 minute interval if indicated by MDC 95.
        RptRndWorkTime(&RptETK.usTotalTime, &RptETK.usTotalMinute);
    }
    if (!(uchRptOpeCount & RPT_HEADER_PRINT)) {                 /* First Operation Etke */
        MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class     */
                        RPT_ACT_ADR,                            /* Report Name Address  */
                        RPT_ETK_ADR,                            /* Report Name Address  */
                        SPC_DAIRST_ADR,                         /* Special Mnemonics Address */
                        RPT_RESET_ADR,                          /* Report Name Address  */
                        uchRptType,                             /* Report Type          */
                        AC_ETK_RESET_RPT,                       /* A/C Number           */
                        uchResetType,                           /* Reset Type           */
                        PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit    */
    }
    if (sNo != 0) {
        RptFeed(RPT_DEFALTFEED);
        RptDateAndTimeElement(RptETK.uchMinorClass, &RptETK);
    }
    MaintIncreSpcCo(SPCO_EODRST_ADR);                               /* Reset counter up    */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);                            /* Print Trailer */
    return(SUCCESS);
}
                                                                                          
/*
*=============================================================================
**  Synopsis: SHORT RptETKLock(VOID) 
*               
*       Input:  nothing
*               
*      Output:  nothing
*
**     Return:  ETK_SUCCESS 
*               ETK_DURING_SIGNIN
*               ETK_ALREADY_LOCK
*               ETK_NOT_IN_FILE              
*
**  Description:  
*       Lock certain etk file for reset report.
*===============================================================================
*/

SHORT RptETKLock(VOID) 
{
    SHORT  sReturn;  
                                                                     
    sReturn = SerEtkAllLock();                                  /* Lock Complete */
    return EtkConvertError(sReturn);
}

/*
*=============================================================================
**  Synopsis: VOID RptETKUnLock(VOID) 
*               
*       Input:  nothing
*      Output:  nothing
*
**     Return:  nothing 
*                 
**  Description:  
*       Unlock certain etk file.
*===============================================================================
*/

VOID RptETKUnLock(VOID) 
{
    SerEtkAllUnLock();                                  
}
/*
*=============================================================================
**  Synopsis: SHORT RptEtkPrint(UCHAR uchMinorClass, RPTEMPOLYEE *pRptEmployee,
*                               ULONG ulEmployeeNo)
*               
*       Input:  UCHAR   uchMinorClass   -   CURRENT of SAVED
*               ULONG   ulEmployeeNo    -   Employee No 
*
*      Output:  nothing
*
**     Return:  RPT_ABORTED 
*                 
**  Description:  
*       Report employee time keeping for one employee.
*		In this function we process the Employee Time Keeping records for
*		the specified employee to provide an ETK report (150 from the
*		Supervisor menu).
*
*       If the appropriate MDC bit is set, we also print out the employee
*       summary listing job codes along with labor costs for the employee.
*===============================================================================
*/
// the following is a temporary define that is undefined at the end
// of this function.  We are going to create an array of RPTJCSUMMARY
// types of this size.  We will use the first ETK_JOBCODE_RECORD_SIZE
// elements of the array to hold the job code line item summaries.
// The last element of the array (referenced with ETK_JOBCODE_RECORD_SIZE)
// will be used to contain the totals of the line items.
#define RPT_MAX_NO_JOB_CODES  ETK_JOBCODE_RECORD_SIZE + 1
// Special value indicating that the timeinjobtotals element is unused
// This value is placed into the uchJobCode part of the struct.
// Normally the range for job codes is 0 to 99.
#define RPT_JCSUMMARY_EMPTY   0x80

static SHORT RptEtkPrint(UCHAR uchMinorClass, RPTEMPLOYEE *pRptEmployee,
                  ULONG ulEmployeeNo)
{
    UCHAR         uchWorkClass;
    SHORT         sStatus = ETK_SUCCESS;
    USHORT        usCount;
	USHORT        usFieldNo = 0;
    USHORT        usTotalTime, usTotalMinute;
    ULONG         ulTotalWage;
	RPTJCSUMMARY  timeinjobtotals[RPT_MAX_NO_JOB_CODES];

    /* Initialize */
    usTotalTime   = pRptEmployee->usTotalTime;
    usTotalMinute = pRptEmployee->usTotalMinute;
    ulTotalWage  = pRptEmployee->ulTotalWage;

    memset(pRptEmployee, '\0', sizeof(RPTEMPLOYEE));

    pRptEmployee->usTotalTime   = usTotalTime;
    pRptEmployee->usTotalMinute = usTotalMinute;
    pRptEmployee->ulTotalWage   = ulTotalWage;

    MaintCalcIndTtl(0, 0, 0, NULL, NULL);                  /* Clear Individual Ttl */
    pRptEmployee->usWorkTime = ETK_TIME_NOT_IN;            /* '**'  */
    pRptEmployee->usWorkMinute = ETK_TIME_NOT_IN;          /* '**'  */
    uchWorkClass = CLASS_RPTEMPLOYEE_PRTWKNOTTL;
    ulTotalWage = 0L;

	// Initialize the job code line items summary array
	// We zero it all out, set the job code to a flag value
	// indicating the array element is not in use.
	// We also go ahead and set the print control which is
	// used by the lower level print functions that provide
	// the interface to the print device.
	// Later as we process the employee's time keeping records, each
	// time we can't find the current job code in the list of
	// line item job codes, we will use the next available empty
	// element in the timeinjobtotals array.
	memset (timeinjobtotals, 0, sizeof(timeinjobtotals));
	{
		int i;
		for (i = 0; i < RPT_MAX_NO_JOB_CODES; i++) {
			timeinjobtotals[i].uchJobCode = RPT_JCSUMMARY_EMPTY;
			timeinjobtotals[i].uchMajorClass = CLASS_RPTJCSUMMARY;
			timeinjobtotals[i].usPrintControl = pRptEmployee->usPrintControl;
		}
	}

	// lets loop through the employee time keeping records that we have
	// in the data base.  As we go through the ETK records, we will calculate
	// totals for all periods as well as print out each line item.
	// As we go through the employee time records, we will also collect our
	// job code summary information for this employee.
	usFieldNo = 0;   // this value is incremented for each field to iterate over the fields.
	for (usCount = 0; usCount < ETK_MAX_FIELD; usCount++) {
		ETK_FIELD   EtkField = {0};
		ETK_TIME    EtkTime = {0};
		USHORT      usLaborCost = 0;

        if (uchMinorClass == CLASS_TTLCURDAY) {
            sStatus = SerEtkCurIndRead(ulEmployeeNo, &usFieldNo, &EtkField, &EtkTime);
        } else {
            sStatus = SerEtkSavIndRead(ulEmployeeNo, &usFieldNo, &EtkField, &EtkTime);
        }
        if (sStatus == ETK_NOT_IN_FIELD) {
            break;                      
        }
        if ((sStatus != ETK_SUCCESS) && (sStatus != ETK_CONTINUE)) {
            return sStatus;
        }
        pRptEmployee->usMonth = EtkField.usMonth;
        pRptEmployee->usDay = EtkField.usDay;
        pRptEmployee->usTimeInTime = EtkField.usTimeinTime;
        pRptEmployee->usTimeInMinute = EtkField.usTimeinMinute;
        pRptEmployee->usTimeOutTime = EtkField.usTimeOutTime;
        pRptEmployee->usTimeOutMinute = EtkField.usTimeOutMinute;
        pRptEmployee->uchJobCode = EtkField.uchJobCode;

		// In the following we determine if we have the time in and time out
		// for a complete shift.  If we have a partial shift then we can't
		// calculate the shift hours and costs.
        if (pRptEmployee->usTimeInTime == ETK_TIME_NOT_IN) {
            pRptEmployee->uchMinorClass = CLASS_RPTEMPLOYEE_PRTNOIN;
        } else if (pRptEmployee->usTimeOutTime == ETK_TIME_NOT_IN) {
            pRptEmployee->uchMinorClass = CLASS_RPTEMPLOYEE_PRTNOOUT;
        } else {
            pRptEmployee->uchMinorClass = CLASS_RPTEMPLOYEE_PRTTIME;
            uchWorkClass = CLASS_RPTEMPLOYEE_PRTWKTTL;
        }
        if (pRptEmployee->uchMinorClass == CLASS_RPTEMPLOYEE_PRTTIME) {
            RptCalWorkTime(pRptEmployee, &pRptEmployee->usWorkTime, &pRptEmployee->usWorkMinute);
            RptCalTotalTime( pRptEmployee );
            RptRndWorkTime(&pRptEmployee->usWorkTime, &pRptEmployee->usWorkMinute);
            RptCalWageTotal(pRptEmployee->uchJobCode, pRptEmployee->usWorkTime, pRptEmployee->usWorkMinute, &pRptEmployee->ulWorkWage, &usLaborCost);
            MaintCalcIndTtl(1, pRptEmployee->usWorkTime, pRptEmployee->usWorkMinute, &usTotalTime, &usTotalMinute);
            ulTotalWage += pRptEmployee->ulWorkWage;
        }
        if (UieReadAbortKey() == UIE_ENABLE) {
            return RPT_ABORTED;
        }
        if(RptPauseCheck() == RPT_ABORTED){
            return(RPT_ABORTED);
        }
        RptDateAndTimeElement(pRptEmployee->uchMinorClass, pRptEmployee);

		// Calculate the job code specific subtotals as requested by SR0007
		// Lookup the job code in our table and increment the values for
		// the job code if it is found.
		// If the job code is not found, its a new one for us, so we initialize
		// a new job code element.
		// lets make sure we do this only if we have a complete shift with
		// both time in and time out.
        if (pRptEmployee->uchMinorClass == CLASS_RPTEMPLOYEE_PRTTIME)
		{
			int i;
			for (i = 0; i < ETK_JOBCODE_RECORD_SIZE && timeinjobtotals[i].uchJobCode != RPT_JCSUMMARY_EMPTY; i++) {
				if (timeinjobtotals[i].uchJobCode == pRptEmployee->uchJobCode) {
					timeinjobtotals[i].ulTotalWage += pRptEmployee->ulWorkWage;
					timeinjobtotals[i].usTotalTime += pRptEmployee->usWorkTime;
					timeinjobtotals[i].usTotalMinute += pRptEmployee->usWorkMinute;
					break;
				}
			}
			if ( i < ETK_JOBCODE_RECORD_SIZE && timeinjobtotals[i].uchJobCode == RPT_JCSUMMARY_EMPTY) {
				timeinjobtotals[i].uchJobCode = pRptEmployee->uchJobCode;
				timeinjobtotals[i].ulTotalWage = pRptEmployee->ulWorkWage;
				timeinjobtotals[i].usLaborCost = usLaborCost;
				timeinjobtotals[i].usTotalTime = pRptEmployee->usWorkTime;
				timeinjobtotals[i].usTotalMinute = pRptEmployee->usWorkMinute;
			}
		}
    }

	// We have now completed processing all of this employees time keeping records.
	// We will now calculate the totals and print them out.
    pRptEmployee->uchMinorClass = uchWorkClass;
    pRptEmployee->usWorkTime   = usTotalTime;
    pRptEmployee->usWorkMinute = usTotalMinute;
    pRptEmployee->ulWorkWage = ulTotalWage;
    pRptEmployee->ulTotalWage += ulTotalWage;
    RptDateAndTimeElement(pRptEmployee->uchMinorClass, pRptEmployee);

	// if the job code labor cost print MDC bit is set (LePeeps job cost summary per
	// SR07), then print out the job code totals that we have accumulated.
	if (CliParaMDCCheck(MDC_ETK_ADR, ODD_MDC_BIT3)){
		int i;

		// we need to print out the headings, summaries, and the totals
		// because there is at least one job code with information.
		// we will temporarily modify the major class of the first item
		// in the timeinjobtotals array so as to force a title print.
		// the actual prints are done in the files prsetk.c and prsetk_.c
		// which are in BusinessLogic in folders Print\Thermsup and
		// Print\PRTSUP respectively.
		if (timeinjobtotals[0].uchJobCode != RPT_JCSUMMARY_EMPTY) {
			timeinjobtotals[0].uchMajorClass = CLASS_RPTJCSUMMARYTITLES;
			PrtPrintItem(NULL, &timeinjobtotals[0]);
			timeinjobtotals[0].uchMajorClass = CLASS_RPTJCSUMMARY;

			for (i = 0; i < ETK_JOBCODE_RECORD_SIZE && timeinjobtotals[i].uchJobCode != RPT_JCSUMMARY_EMPTY; i++) {
				timeinjobtotals[i].usPrintControl = usRptPrintStatus;   /* Seted RptPrtStatusSet */
				// check to see if we have more than 59 minutes in the minutes accumulator when
				// we were totaling out the job code.  We could have more than 59 minutes if multiple
				// sign ins on the same job code each of which have fewer than 60 minutes but when
				// added together total more than 59 minutes.
				if (timeinjobtotals[i].usTotalMinute > 59) {
					USHORT hours;
					hours = timeinjobtotals[i].usTotalMinute / 60;
					timeinjobtotals[i].usTotalTime += hours;
					timeinjobtotals[i].usTotalMinute %= 60;
				}
				// Keep running totals for this employee so that we can print them out
				// after doing the line item summary for each job code.
				// We are using the last item in our timeinjobtotals array for convienence.
				timeinjobtotals[ETK_JOBCODE_RECORD_SIZE].ulTotalWage += timeinjobtotals[i].ulTotalWage;
				timeinjobtotals[ETK_JOBCODE_RECORD_SIZE].usTotalTime += timeinjobtotals[i].usTotalTime;
				timeinjobtotals[ETK_JOBCODE_RECORD_SIZE].usTotalMinute += timeinjobtotals[i].usTotalMinute;
				PrtPrintItem(NULL, &timeinjobtotals[i]);
			}  // end for

			// Do minute promotion if minutes are more than 59 so that hours and minutes
			// will print correctly.
			// The special class CLASS_RPTJCSUMMARYTOTALS is used to indicate to the
			// print functions that we want the totals to print.
			if (timeinjobtotals[ETK_JOBCODE_RECORD_SIZE].usTotalMinute > 59) {
				USHORT hours;
				hours = timeinjobtotals[ETK_JOBCODE_RECORD_SIZE].usTotalMinute / 60;
				timeinjobtotals[ETK_JOBCODE_RECORD_SIZE].usTotalTime += hours;
				timeinjobtotals[ETK_JOBCODE_RECORD_SIZE].usTotalMinute %= 60;
			}
			timeinjobtotals[ETK_JOBCODE_RECORD_SIZE].uchMajorClass = CLASS_RPTJCSUMMARYTOTALS;
			PrtPrintItem(NULL, &timeinjobtotals[ETK_JOBCODE_RECORD_SIZE]);
		}
	}  // endif  (CliParaMDCCheck(MDC_ETK_ADR, ODD_MDC_BIT3))
    return sStatus;
}

#undef RPT_MAX_NO_JOB_CODES
#undef RPT_JCSUMMARY_EMPTY

/*
*=============================================================================
**  Synopsis: VOID RptDateAndTimeElement( uchMinorClass,
*                                         RPTEMPLOYEE *pRptEmployee)
*                   
*       Input: uchMinorClass    : Minor Class
*              pRptEmployee     :                    
*      Output:  Nothing
*
**  Return: SUCCESS         : Successful  
*
**  Description: Format Print Format of Hourly Activity Report.  
*===============================================================================
*/

static VOID RptDateAndTimeElement( UCHAR uchMinorClass, RPTEMPLOYEE *pRptEmployee)
{
    if (uchRptMldAbortStatus) {                         /* aborted by MLD */
        return;
    }

    pRptEmployee->usPrintControl = usRptPrintStatus;   /* Seted RptPrtStatusSet */
    pRptEmployee->uchMajorClass = CLASS_RPTEMPLOYEE;

    /* Distingush Minor Class */
    switch (uchMinorClass) {
    case CLASS_RPTEMPLOYEE_NUMBER:
    case CLASS_RPTEMPLOYEE_RESET:
		{
			RPTEMPLOYEENO   RptEmployeeNo = {0};

			RptEmployeeNo.uchMajorClass = CLASS_RPTEMPLOYEENO;
			RptEmployeeNo.usPrintControl = usRptPrintStatus; 
			if (uchMinorClass == CLASS_RPTEMPLOYEE_RESET) {
				RptEmployeeNo.usPrintControl |= PRT_JOURNAL;
				RptEmployeeNo.uchMinorClass = CLASS_RPTEMPLOYEE_RESET;
			} else {
				RptEmployeeNo.uchMinorClass = CLASS_RPTEMPLOYEE_READ;
			}
			RptEmployeeNo.ulEmployeeNumber = pRptEmployee->ulEmployeeNumber; /* Set Cashier No     */
			_tcsncpy(RptEmployeeNo.aszEmpMnemo, pRptEmployee->aszEmpMnemo, PARA_LEADTHRU_LEN);
			if (RptCheckReportOnMld()) {
				uchRptMldAbortStatus = (UCHAR)MldDispItem(&RptEmployeeNo, 0);  /* Print each Element  */
				RptEmployeeNo.usPrintControl &= PRT_JOURNAL;             /* Reset Receipt print status so only goes to Electronic Journal if set */
			}
			PrtPrintItem(NULL, &RptEmployeeNo);                           /* print out            */
		}
        break;

    case CLASS_RPTEMPLOYEE_PRTMNEMO:                                /* Set Date  for Print Out      */
        /* Get and Set Transaction Mnemonics */                     /* |--------------------------| */
                                                                    /* |     TIME IN   TIME OUT   | */
                                                                    /* |--------------------------| */
        RflGetTranMnem (pRptEmployee->aszTimeinMnemo, TRN_ETKIN_ADR);
        RflGetTranMnem(pRptEmployee->aszTimeoutMnemo, TRN_ETKOUT_ADR);
        if (RptCheckReportOnMld()) {
            uchRptMldAbortStatus = (UCHAR)MldDispItem(pRptEmployee, 0);  /* Print each Element  */
            pRptEmployee->usPrintControl &= PRT_JOURNAL;           /* Reset Receipt print status so only goes to Electronic Journal if set */
        }
        PrtPrintItem(NULL, pRptEmployee);
        break;

    case CLASS_RPTEMPLOYEE_PRTTIME:
    case CLASS_RPTEMPLOYEE_PRTNOIN:
    case CLASS_RPTEMPLOYEE_PRTNOOUT:
        /* Get and Set Transaction Mnemonics */                     /* |--------------------------| */
                                                                    /* |Z9/99  Z9:99AM    Z9:99PM | */
                                                                    /* |              99  Z9:99   | */
                                                                    /* |--------------------------| */
        if (RptCheckReportOnMld()) {
            uchRptMldAbortStatus = (UCHAR)MldDispItem(pRptEmployee, 0);  /* Print each Element  */
            pRptEmployee->usPrintControl &= PRT_JOURNAL;           /* Reset Receipt print status so only goes to Electronic Journal if set */
        }
        PrtPrintItem(NULL, pRptEmployee);
        break;

    case CLASS_RPTEMPLOYEE_PRTNOTOTAL:
    case CLASS_RPTEMPLOYEE_PRTWKNOTTL:
    case CLASS_RPTEMPLOYEE_PRTTOTAL:
    case CLASS_RPTEMPLOYEE_PRTWKTTL:
        /* Get and Set Transaction Mnemonics */                     /* |--------------------------| */
                                                                    /* |TOTAL          ZZZZ9:99   | */
                                                                    /* |--------------------------| */
        RflGetTranMnem (pRptEmployee->aszTimeinMnemo, TRN_TTLR_ADR);
        if (RptCheckReportOnMld()) {
            uchRptMldAbortStatus = (UCHAR)MldDispItem(pRptEmployee, 0);  /* Print each Element  */
            pRptEmployee->usPrintControl &= PRT_JOURNAL;           /* Reset Receipt print status so only goes to Electronic Journal if set */
        }
        PrtPrintItem(NULL, pRptEmployee);
        break;
    }
}
/*
=============================================================================
**  Synopsis: VOID RptCalWorkTime( RPTEMPLOYEE *pRptEmployee,
*                                  USHORT *pusWorkTime,
*                                  USHORT *pusWorkMinute);
*                   
*       Input: RPTEMPLOYEE RptEmployee      -   
*              USHORT      *usWorkTime      -
*              USHORT      *usWorkMinute    -
*                                 
*      Output:  Nothing
*
**  Return: SUCCESS         : Successful  
*
**  Description: Calculate work hours.
*============================================================================
*/
static VOID RptCalWorkTime( RPTEMPLOYEE *pRptEmployee,
                     USHORT *pusWorkTime,
                     USHORT *pusWorkMinute)
{
    if (pRptEmployee->usTimeInTime > pRptEmployee->usTimeOutTime) {
        /* Over 1 day work */
        *pusWorkTime = (pRptEmployee->usTimeOutTime + 24) - pRptEmployee->usTimeInTime;
    } else if ((pRptEmployee->usTimeInTime == pRptEmployee->usTimeOutTime) &&
              (pRptEmployee->usTimeInMinute > pRptEmployee->usTimeOutMinute)) {
        *pusWorkTime = 24;
    } else {
        *pusWorkTime = pRptEmployee->usTimeOutTime - pRptEmployee->usTimeInTime;
    }
    if (pRptEmployee->usTimeInMinute > pRptEmployee->usTimeOutMinute) {
        *pusWorkMinute = (pRptEmployee->usTimeOutMinute + 60) - pRptEmployee->usTimeInMinute;
        (*pusWorkTime)--;
    } else {
        *pusWorkMinute =  pRptEmployee->usTimeOutMinute - pRptEmployee->usTimeInMinute;
    }
}
/*
=============================================================================
**  Synopsis: VOID RptCalTotalTime( RPTEMPLOYEE *pRptEmployee )
*                   
*       Input: RPTEMPLOYEE RptEmployee->usWorkTime
*                          RptEmployee->usWorkMinute
*                                 
*       Output: RPTEMPLOYEE RptEmployee->usTotalTime
*                           RptEmployee->usTotalMinute
*
**  Return: SUCCESS         : Successful  
*
**  Description: Calculate total work hours.
*============================================================================
*/
static VOID RptCalTotalTime( RPTEMPLOYEE *pRptEmployee )
{
    if (pRptEmployee->usWorkTime != ETK_TIME_NOT_IN) {
		USHORT  usWorkTime;

        pRptEmployee->usTotalTime += pRptEmployee->usWorkTime;
        pRptEmployee->usTotalMinute += pRptEmployee->usWorkMinute;
        if ((usWorkTime = (pRptEmployee->usTotalMinute / 60)) != 0) {
            pRptEmployee->usTotalTime += usWorkTime;
            pRptEmployee->usTotalMinute = (pRptEmployee->usTotalMinute % 60);
        }
    }
}
/*
=============================================================================
**  Synopsis: VOID RptRndWorkTime( USHORT *usWorkTime,
*                                  USHORT *usWorkMinute);
*                   
*       Input:         -   
*      Output:   USHORT      *usWorkTime      -
*                USHORT      *usWorkMinute    -
*
**  Return: SUCCESS         : Successful  
*
**  Description: Check MDC 95 to see if Rounding work hours to nearest 15 minute
*                interval.
*============================================================================
*/
static VOID RptRndWorkTime( USHORT *pusWorkTime,
                     USHORT *pusWorkMinute)
{
    if (!CliParaMDCCheck(MDC_ETK_ADR, ODD_MDC_BIT1)) {             /* Check MDC#95-BIT1  */
        return;
    }
    if ((*pusWorkMinute % 15) > 7) {
        *pusWorkMinute = ((*pusWorkMinute / 15) * 15) + 15;
        if (*pusWorkMinute == 60) {
            (*pusWorkTime)++;
            *pusWorkMinute = 0;
        }
    } else {
        *pusWorkMinute = ((*pusWorkMinute / 15) * 15);
        if (*pusWorkMinute == 60) {
            (*pusWorkTime)++;
            *pusWorkMinute = 0;
        }
    }
}
/*
=============================================================================
**  Synopsis: VOID RptResetTimePrint( ULONG ulEmployeeNo,
*                                     UCHAR uchMinorClass );
*
*       Input:  ULONG       ulEmployeeNo
*               UCHAR       uchMinorClass
*      Output:
*
**  Return: SUCCESS         : Successful
*
**  Description: Set for Totals Reset time. If the employee number does exist
*                or does not exist, we will still get the time stamps for the
*                totals.
*============================================================================
*/
static VOID RptResetTimePrint( ULONG ulEmployeeNo, UCHAR uchMinorClass )
{
    N_DATE          FromDate, ToDate;
    ETK_FIELD       EtkField = {0};
	ETK_TIME        EtkTime = {0};
    USHORT          usFieldNo = 0;

    SerEtkCurIndRead(ulEmployeeNo, &usFieldNo, &EtkField, &EtkTime);

    if (uchMinorClass == CLASS_TTLCURDAY || uchMinorClass == CLASS_TTLCURPTD)  {
		TtlGetNdate (&ToDate);                 /* Get Current Date/Time and  Set Period To Date */
        FromDate.usMin = EtkTime.usFromMinute;
        FromDate.usHour = EtkTime.usFromTime;
        FromDate.usMDay = EtkTime.usFromDay;
        FromDate.usMonth = EtkTime.usFromMonth;
    } else {
        ToDate.usMin = EtkTime.usToMinute;                     /* Set Period To Date    */
        ToDate.usHour = EtkTime.usToTime;
        ToDate.usMDay = EtkTime.usToDay;
        ToDate.usMonth = EtkTime.usToMonth;
        FromDate.usMin = EtkTime.usFromMinute;
        FromDate.usHour = EtkTime.usFromTime;
        FromDate.usMDay = EtkTime.usFromDay;
        FromDate.usMonth = EtkTime.usFromMonth;
    }
    RptPrtTime(TRN_PFROM_ADR, &FromDate);       	            /* PERIOD FROM     */
    RptPrtTime(TRN_PTO_ADR, &ToDate);		                    /* PERIOD TO       */
}

/*
=============================================================================
**  Synopsis: VOID RptEmployeeNoPrint( ULONG ulEmployeeNo );
*                   
*       Input:  UCHAR      uchType         -   CLASS_TTLCURDAY
*                                              CLASS_TTLSAVDAY
*               ULONG      ulEmployeeNo    -   Employee's No
*      Output:   
*
**  Return: SUCCESS         : Successful  
*
**  Description:    Employee's No Print.
*============================================================================
*/
static VOID RptEmployeeNoPrint( ULONG ulEmployeeNo )
{
	RPTEMPLOYEE   RptEmployee = {0};
    ETK_JOB       EtkJob = {0};

    /* set employee mnemonic */
    if (SerEtkIndJobRead(ulEmployeeNo, &EtkJob, RptEmployee.aszEmpMnemo) == ETK_SUCCESS) {
        RptEmployee.aszEmpMnemo[PARA_LEADTHRU_LEN] = '\0';
    } else {
        RptEmployee.aszEmpMnemo[0] = '\0';
    }
    RptEmployee.ulEmployeeNumber = ulEmployeeNo;
    RptDateAndTimeElement(CLASS_RPTEMPLOYEE_NUMBER, &RptEmployee);
    RptFeed(RPT_DEFALTFEED);                                      /* Feed */
    RptEmployee.uchMinorClass = CLASS_RPTEMPLOYEE_PRTMNEMO;
    RptDateAndTimeElement(RptEmployee.uchMinorClass, &RptEmployee);
}
/*
=============================================================================
**  Synopsis: VOID RptCalWageTotal(UCHAR uchJobCode,
*                                  USHORT usWorkTime,
*                                  USHORT usWorkMinute,
*                                  ULONG  *pulWorkWage,
*                                  USHORT *pusLaborCost );
*                   
*       Input: UCHAR       uchJobCode       - contains job code
*              USHORT      usWorkTime       - contains hours worked
*              USHORT      usWorkMinute     - contains minutes worked
*              ULONG       *pulWorkWage     - returns calculated wages for time
*              USHORT      *pusLaborCost    - returns job labor cost from database
*                                 
*      Output:  Nothing
*
**  Return: SUCCESS         : Successful
*				returns calculated WorkWage in *pulWorkWage
*				returns job labor cost in *pusLaborCost
*
**  Description: Calculate wage from work hours and job code labor cost.
*============================================================================
*/
static VOID RptCalWageTotal(UCHAR  uchJobCode,
                     USHORT usWorkTime,
                     USHORT usWorkMinute,
                     ULONG  *pulWorkWage,
					 USHORT *pusLaborCost)
{
	PARALABORCOST  ParaLaborCost = {0};
    D13DIGITS      d13Work;
    
    ParaLaborCost.uchMajorClass = CLASS_PARALABORCOST;
    if (uchJobCode) {
        ParaLaborCost.uchAddress = uchJobCode;
    } else {
        ParaLaborCost.uchAddress = LABOR_COST100_ADR;
    }
    ParaLaborCostRead (&ParaLaborCost);

    /* calculate wage total by hour */
	d13Work = (LONG)ParaLaborCost.usLaborCost;
	d13Work *= (LONG)usWorkTime;
    *pulWorkWage = (LONG) d13Work;

    /* calculate wage total by minute then divide by 60 minutes and round to calculate hourly wage */
	d13Work = (LONG)ParaLaborCost.usLaborCost;
    usWorkMinute *= 10;
	d13Work *= (LONG)usWorkMinute;
	d13Work += 5L;
	d13Work /= 600L;

    *pulWorkWage +=  (LONG)d13Work;
	if (pusLaborCost) {
		*pusLaborCost = ParaLaborCost.usLaborCost;
	}
}
/*
=============================================================================
**  Synopsis: VOID RptETKCalHourlyTotal(UCHAR uchMinorClass
*                                   DATE *pFromDate, DATE *pToDate,
*                                   RPTHOURLYETK *pRptHourlyEtk)
*                   
*       Input: UCHAR       uchMinorClass    -
*              DATE        *pFromDate       -
*              DATE        *pToDate         -
*                                 
*      Output: RPTHOURLYETK *pRptHourlyEtk  -
*
**  Return: 
*
**  Description: Calculate wage by houry block R3.1
*============================================================================
*/
SHORT RptETKCalHourlyTotal(UCHAR uchMinorClass,
                        N_DATE *pFromDate, N_DATE *pToDate,
                        RPTHOURLYETK *pRptHourlyEtk)
{
    ULONG       aulRcvBuffer[FLEX_ETK_MAX];
    SHORT       sEtkNo, sNo = 0;

    if (uchMinorClass == CLASS_TTLCURDAY)  {                /* Read  report ? */
        if ((sNo = SerEtkCurAllIdRead(sizeof(aulRcvBuffer), aulRcvBuffer)) < 0) {
            return( EtkConvertError(sNo) );
        }
    } else {
        if ((sNo = SerEtkSavAllIdRead(sizeof(aulRcvBuffer), aulRcvBuffer)) < 0) {
            return( EtkConvertError(sNo) );
        }
    }

    for (sEtkNo = 0; sEtkNo < sNo; sEtkNo++) {
        RptETKCalHourlyTotalSub(aulRcvBuffer[sEtkNo], uchMinorClass, pFromDate, pToDate, pRptHourlyEtk);
    }

    return(SUCCESS);
}

/*
=============================================================================
**  Synopsis:   VOID    RptETKCalHourlyTotalSub(LONG ulEmployeeNo,
*                           UCHAR uchMinorClass, 
*                           DATE *pFromDate, DATE *pToDate,
*                           RPTHOURLYETK *pRptHourlyEtk)
*       Input: 
*      Output:
**  Return: 
*
**  Description: Calculate wage by houry block  R3.1(Oct/21/97)
*============================================================================
*/
static VOID    RptETKCalHourlyTotalSub(LONG ulEmployeeNo, UCHAR uchMinorClass,
                N_DATE *pFromDate, N_DATE *pToDate, RPTHOURLYETK *pRptHourlyEtk)
{
    USHORT  usCount;
	USHORT  usFieldNo = 0;
    SHORT   sStatus;
    ULONG   ulEtkFrom, ulEtkTo, ulHourlyFrom, ulHourlyTo, ulNow, ulEtk1 = 0UL;
    DATE_TIME   Date1, Date2;

    PifGetDateTime(&Date1);
    Date2 = Date1;

    ulNow = RptETKCalHourlyTotalSub1(&Date2);

    Date2.usMonth   = pToDate->usMonth;
    Date2.usMDay    = pToDate->usMDay;
    Date2.usHour    = pToDate->usHour;
    Date2.usMin     = pToDate->usMin;
    ulHourlyTo = RptETKCalHourlyTotalSub1(&Date2);
    if (ulNow < ulHourlyTo) {
        Date2.usYear--;
        ulHourlyTo = RptETKCalHourlyTotalSub1(&Date2);
    }

    Date2.usYear    = Date1.usYear;
    Date2.usMonth   = pFromDate->usMonth;
    Date2.usMDay    = pFromDate->usMDay;
    Date2.usHour    = pFromDate->usHour;
    Date2.usMin     = pFromDate->usMin;
    ulHourlyFrom = RptETKCalHourlyTotalSub1(&Date2);
    if (ulNow < ulHourlyFrom) {
        Date2.usYear--;
        ulHourlyFrom = RptETKCalHourlyTotalSub1(&Date2);
    }

    if (ulHourlyTo < ulHourlyFrom) {
        Date2.usYear--;
        ulHourlyFrom = RptETKCalHourlyTotalSub1(&Date2);
    }

	usFieldNo = 0;   // this value is incremented for each field to iterate over the fields.
    for (usCount = 0; usCount < ETK_MAX_FIELD; usCount++, ulEtk1 = 0UL) {
		RPTETKWORKFIELD  WorkEtk;
		ETK_FIELD   EtkField = {0};
		ETK_TIME    EtkTime = {0};

        if (uchMinorClass == CLASS_TTLCURDAY) {
            sStatus = SerEtkCurIndRead(ulEmployeeNo, &usFieldNo, &EtkField, &EtkTime);
        } else {
            sStatus = SerEtkSavIndRead(ulEmployeeNo, &usFieldNo, &EtkField, &EtkTime);
        }
        if (sStatus == ETK_NOT_IN_FIELD) {
            break;                      
        }
        if ((sStatus != ETK_SUCCESS) && (sStatus != ETK_CONTINUE)) {
            return;
        }

        if (EtkField.usTimeinTime == ETK_TIME_NOT_IN) {
            continue;
        } else if (EtkField.usTimeOutTime == ETK_TIME_NOT_IN) {
            /* set current time */
            EtkField.usTimeOutTime = pToDate->usHour;
            EtkField.usTimeOutMinute = pToDate->usMin;
        }

        /* --- these data are used for "compare" --- */
        Date2.usYear    = Date1.usYear;
        Date2.usMonth   = EtkField.usMonth;
        Date2.usMDay    = EtkField.usDay;
        if ((EtkField.usTimeOutTime * 60 + EtkField.usTimeOutMinute) < (EtkField.usTimeinTime * 60 + EtkField.usTimeinMinute)) {
            ulEtk1 = 24 * 60;
        }
        Date2.usHour    = EtkField.usTimeOutTime;
        Date2.usMin     = EtkField.usTimeOutMinute;
        ulEtkTo = RptETKCalHourlyTotalSub1(&Date2) + ulEtk1;
        if (ulNow < ulEtkTo) {
            Date2.usYear--;
            ulEtkTo = RptETKCalHourlyTotalSub1(&Date2) + ulEtk1;
        }

        Date2.usYear    = Date1.usYear;
        Date2.usMonth   = EtkField.usMonth;
        Date2.usMDay    = EtkField.usDay;
        Date2.usHour    = EtkField.usTimeinTime;
        Date2.usMin     = EtkField.usTimeinMinute;
        ulEtkFrom = RptETKCalHourlyTotalSub1(&Date2);
        if (ulNow < ulEtkFrom) {
            Date2.usYear--;
            ulEtkFrom = RptETKCalHourlyTotalSub1(&Date2);
        }

        if (ulEtkTo < ulEtkFrom) {
            Date2.usYear--;
            ulEtkFrom = RptETKCalHourlyTotalSub1(&Date2);
        }

        if ((ulEtkTo <= ulHourlyFrom) || (ulEtkFrom >= ulHourlyTo)) {
            continue;
        }

        if ((ulEtkFrom < ulHourlyFrom) && (ulHourlyFrom < ulEtkTo)) {
            EtkField.usTimeinTime = pFromDate->usHour;
            EtkField.usTimeinMinute = pFromDate->usMin;
        }
        if ((ulEtkTo > ulHourlyTo) && (ulHourlyTo > ulEtkFrom)) {
            EtkField.usTimeOutTime = pToDate->usHour;
            EtkField.usTimeOutMinute = pToDate->usMin;
        }

        WorkEtk.uchJobCode  = EtkField.uchJobCode;
        WorkEtk.usInMinute  = EtkField.usTimeinTime * 60 + EtkField.usTimeinMinute;
        WorkEtk.usOutMinute = EtkField.usTimeOutTime * 60 + EtkField.usTimeOutMinute;

        if ((EtkField.usTimeOutTime < EtkField.usTimeinTime) ||
            ((EtkField.usTimeOutTime == EtkField.usTimeinTime) &&
             (EtkField.usTimeOutMinute < EtkField.usTimeinMinute))) {
			RPTETKWORKFIELD  WorkEtk2 = WorkEtk;

            /* ---- over 0:00 case ---- */
            WorkEtk.usInMinute = 0;
            RptETKCalHourlyTotalField(&WorkEtk, pRptHourlyEtk);

            WorkEtk2.usOutMinute = 24 * 60;
            RptETKCalHourlyTotalField(&WorkEtk2, pRptHourlyEtk);

        } else {
            RptETKCalHourlyTotalField(&WorkEtk, pRptHourlyEtk);
        }
    }
}


static VOID RptETKCalHourlyTotalField(RPTETKWORKFIELD *pWorkEtk,
                        RPTHOURLYETK *pRptHourlyEtk)
{
    UCHAR   auchParaHourly[MAX_HOUR_NO+1][MAX_HM_DATA];
    USHORT  ausHour[MAX_HOUR_NO];
    USHORT  usDummy;
    USHORT  usHourlyBlock, i;
    USHORT  usCalcEnd = 0;
    USHORT  usWorkMinute = 0;

    CliParaAllRead(CLASS_PARAHOURLYTIME, &auchParaHourly[0][0], sizeof(auchParaHourly), 0, &usDummy);

    usHourlyBlock = (USHORT)auchParaHourly[0][0];
    
    /* --- Terminater --- */
    auchParaHourly[usHourlyBlock+1][0] = auchParaHourly[1][0];
    auchParaHourly[usHourlyBlock+1][1] = auchParaHourly[1][1];

    /* convert HH:MM to minute */
    for (i = 0; i < (usHourlyBlock + 1); i++) {
        ausHour[i] = (USHORT)(auchParaHourly[i+1][0] * 60 + auchParaHourly[i+1][1]);
    }

    for (i = 0; i < usHourlyBlock; i++, usWorkMinute = 0) {
        if (ausHour[i+1] < ausHour[i]) {
            if (pWorkEtk->usInMinute <= ausHour[i+1] && pWorkEtk->usOutMinute > ausHour[i+1]) {
                usWorkMinute = ausHour[i+1] - pWorkEtk->usInMinute;
                pWorkEtk->usInMinute = ausHour[i+1];
            } else if (pWorkEtk->usInMinute <= ausHour[i+1] && pWorkEtk->usOutMinute <= ausHour[i+1]) {
                usWorkMinute = pWorkEtk->usOutMinute - pWorkEtk->usInMinute;
                usCalcEnd = 1;
            }
            if (pWorkEtk->usOutMinute > ausHour[i] && pWorkEtk->usInMinute <= ausHour[i]) {
                usWorkMinute += pWorkEtk->usOutMinute - ausHour[i];
                pWorkEtk->usOutMinute = ausHour[i];
                
            } else if (pWorkEtk->usOutMinute > ausHour[i] && pWorkEtk->usInMinute >= ausHour[i]) {
                usWorkMinute += pWorkEtk->usOutMinute - pWorkEtk->usInMinute;
                usCalcEnd = 1;
            }
            if (usWorkMinute == 0) {
                continue;
            }

        } else {
            if (pWorkEtk->usInMinute < ausHour[i]) {
                if (pWorkEtk->usOutMinute >= ausHour[i]) {
                    usWorkMinute = ausHour[i] - pWorkEtk->usInMinute;
                    pWorkEtk->usInMinute = ausHour[i];
                    RptETKCalHourlyTotalSub2(usWorkMinute, (USHORT)(usHourlyBlock - 1), pWorkEtk, pRptHourlyEtk); /* ### Mod (2171 for Win32) V1.0 */
                } else {
                    usWorkMinute = pWorkEtk->usOutMinute - pWorkEtk->usInMinute;
                    RptETKCalHourlyTotalSub2(usWorkMinute, (USHORT)(usHourlyBlock - 1), pWorkEtk, pRptHourlyEtk);  /* ### Mod (2171 for Win32) V1.0 */
                    return;
                }
            }
            if (pWorkEtk->usOutMinute < ausHour[i+1]
                && pWorkEtk->usOutMinute >= ausHour[i]
                && pWorkEtk->usInMinute <= ausHour[i]) {
                usWorkMinute = pWorkEtk->usOutMinute - ausHour[i];
                pWorkEtk->usOutMinute = ausHour[i];

            } else if (pWorkEtk->usInMinute >= ausHour[i]
                && pWorkEtk->usInMinute < ausHour[i+1]
                && pWorkEtk->usOutMinute > ausHour[i+1]) {
                usWorkMinute = ausHour[i+1] - pWorkEtk->usInMinute;
                pWorkEtk->usInMinute = ausHour[i+1];

            } else if (pWorkEtk->usInMinute >= ausHour[i]
                && pWorkEtk->usOutMinute <= ausHour[i+1]) {
                usWorkMinute = pWorkEtk->usOutMinute - pWorkEtk->usInMinute;
                usCalcEnd = 1;

            } else {
                continue;
            }
        }
        
        RptETKCalHourlyTotalSub2(usWorkMinute, i, pWorkEtk, pRptHourlyEtk);

        if (usCalcEnd) break;
    }
}


/*
=============================================================================
**  Synopsis:   ULONG   RptETKCalHourlyTotalSub1(DATE_TIME *pEtkDate)
*       Input: 
*      Output:
**  Return: 
*
**  Description:    Calculate ETK Sign-out "YY/MM/DD"   (Oct/21/97)
*============================================================================
*/
static ULONG   RptETKCalHourlyTotalSub1(DATE_TIME *pDate)
{
    USHORT  usLeap = 0, usDay;
    ULONG   ulTotal;
    USHORT  usTable[12] = {0,
                           0+31,
                           0+31+28,
                           0+31+28+31,
                           0+31+28+31+30,
                           0+31+28+31+30+31,
                           0+31+28+31+30+31+30,
                           0+31+28+31+30+31+30+31,
                           0+31+28+31+30+31+30+31+31,
                           0+31+28+31+30+31+30+31+31+30,
                           0+31+28+31+30+31+30+31+31+30+31,
                           0+31+28+31+30+31+30+31+31+30+31+30};

    usDay = usTable[pDate->usMonth - 1];
    usDay += pDate->usMDay - 1;

    if ((pDate->usYear % 4) == 0 && pDate->usMonth >= 3) {
        usDay++; 
    }

    ulTotal = usDay * 24UL * 60UL + pDate->usHour * 60UL + pDate->usMin;
    ulTotal += (pDate->usYear - 1996) * 365UL * 24UL * 60UL;

    if ((pDate->usYear % 4) != 0) {             /* leap year */
        usLeap = 1;
    }    
    ulTotal += 24 * 60 * ((pDate->usYear - 1996) / 4 + usLeap);

    return(ulTotal);
}

static VOID    RptETKCalHourlyTotalSub2(USHORT usWorkMinute, USHORT i, 
                RPTETKWORKFIELD *pWorkEtk, RPTHOURLYETK *pRptHourlyEtk)
{
    USHORT  usTime, usMinute;
    LONG    ulWage;
	USHORT  usLaborCost;

    /* convert HH:MM for rounding & wage calculation */
    usMinute = usWorkMinute;
    usTime = usWorkMinute / 60;
    usMinute -= usTime * 60;

    RptRndWorkTime(&usTime, &usMinute);             /* rounding by 15 minutes */
    RptCalWageTotal(pWorkEtk->uchJobCode, usTime, usMinute, &ulWage, &usLaborCost);
    pRptHourlyEtk->ulWageTotal[i] += ulWage;
}

/*====== End of Source ======*/
