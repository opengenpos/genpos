/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Department Report Module                         
* Category    : Report For Supervisor, NCR 2170 US Hospitality Application          
* Program Name: RPTDEPT.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*         RptDEPTRead()               : Read Daily and PTD DEPT report
*           RptDEPTEditInd()          : Edit individual DEPT report
*           RptMDEPTTtlEdit()         : Print out All Major DEPT Total. A/C #122
*           RptDEPTEditByMDEPT()      : Edit DEPT report by MDEPT
*             RptDEPTTableInd()       : Set Individual Major DEPT Table
*             RptDEPTTableAll()       : Set All Major DEPT Table
*             RptMDEPTNamePrt()       : Print Major DEPT mnemonics
*             RptDEPT()               : Classify the status to OPEN, CREDIT and HASH
*             RptDEPTPrt()            : Print DEPT element
*             RptDEPTHeader()         : Print Header
*             RptDEPTPercent()        : Calculate Percent based on MDC 
*             RptMDEPTGrandTtlPrt()   : Print Total or Grand Total
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jun-30-93: 01.00.12   :  J.IKEDA  : Delete Feed Function after Header print in RptDEPTHeader()                                 
* Aug-25-93: 01.00.13   :  J.IKEDA  : Add PrtShrPartialSend()
* Mar-17-94 : 02.05.00  :  K.You    : bug fixed S-19 (mod. RptDEPTPercent) 
* Jul-24-95 : 03.00.01  :  M.Ozawa  : modify for scroll control.
* Dec-11-99 : 01.00.00  :  krkato   : Saratoga
** GenPOS
* Jul-06-17: 02.02.02   : rchambers : code cleanup, initialize, localize variables.
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
============================================================================
;                      I N C L U D E     F I L E s                         
;===========================================================================
*/

#include <tchar.h>
#include <string.h>

#include <ecr.h>
#include <regstrct.h>
#include <uie.h>
#include <pif.h>
#include <log.h>
#include <appllog.h>
#include <rfl.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <csop.h>
#include <csstbopr.h>
#include <csttl.h>
#include <csstbttl.h>
#include <csstbstb.h>
#include <report.h>
#include <transact.h>
#include <prt.h>
#include <mld.h>
#include <mldsup.h>
#include <plu.h>

#include "rptcom.h"

/*
*               What is a Hash Department
*
*       A Hash Department PLU is indicated by with the PLU_HASH bit flag in the control codes:
*          department control code  ->  auchControlCode[2] & PLU_HASH
*          PLU control code         -> ContPlu.auchContOther[2] & PLU_HASH
*
*       A brief discussion as to what a hash department is.
*          Hash Departments are typically considered as non-item revenues not counted
*          towards item sales but reflected in money (forms of payment) collected.
*          Typical to basic cash registers, hash departments are departments not items.
*          When programming a PLU on a basic cash register you normally link the PLU to
*          a department.
*
*          It should be stated that there may be instances where items in hash departments
*          may be taxable and will have to be reflected in tax categories.
*          Hash Departments can also have a positive or negative value (e.g. bottle deposit
*          refunds).
*          Special consideration should be given when implementing Hash Departments to
*          incorporate tax possibilities and +/- values.
*/

/*************************************************************/

static LTOTAL    RptGTotal;           /* Store Grand Total for RPT_ALL_READ */
static LTOTAL    RptDEPTAllTotal;     /* Store All DEPT total */

static SHORT  RptDEPTEditInd( UCHAR uchMinorClass, UCHAR uchType, USHORT usDEPTNumber);
static SHORT  RptMDEPTTtlEdit( UCHAR uchMinorClass, UCHAR uchType);
static SHORT  RptDEPTEditByMDEPT( UCHAR uchMinorClass, UCHAR uchType, USHORT usDEPTNumber);
static VOID   RptMDEPTNamePrt(UCHAR uchMDEPTNo, UCHAR uchType);
static SHORT  RptDEPTHeader( UCHAR uchMinorClass, UCHAR uchType);
static VOID   RptMDEPTGrandTtlPrt( USHORT  usTransAddr, LTOTAL *pTotal, UCHAR  uchType);

static SHORT  RptDEPTTableInd( UCHAR uchMinorClass, DEPTTBL *pTable, USHORT *pusIndx, USHORT  usMDEPTNumber);
static VOID   RptDEPTPrt( UCHAR uchMinorClass, DEPTTBL *pTable, UCHAR uchType);
static USHORT RptDEPTPercent( LTOTAL *pDeptTtl);
static SHORT  RptDEPT( UCHAR   uchType, USHORT  usStatus, USHORT  usMDEPTNo, DEPTTBL *pTable, LTOTAL  *pTotal, USHORT  *pusIndx, UCHAR  *puchFlag);              

/*
*===========================================================================
**  Synopsis: SHORT RptDEPTRead(UCHAR uchMinorClass, UCHAR uchType, USHORT usNumber) 
*               
*       Input:  UCHAR  uchMinorClass   : CLASS_TTLCURDAY
*                                      : CLASS_TTLSAVDAY  
*                                      : CLASS_TTLCURPTD
*                                      : CLASS_TTLSAVPTD
*               UCHAR  uchType         : RPT_DEPTIND_READ  
*                                      : RPT_ALL_MDEPT_READ 
*                                      : RPT_FIN_READ
*                                      : RPT_ALL_READ
*                                      : RPT_MDEPT_READ
*                                      : RPT_EODALL_READ
*                                      : RPT_PTDALL_READ 
*               USHORT usNumber        : DEPT number or MDEPT number
*
*      Output:  nothing
*
**     Return:  SUCCESS     
*               RPT_ABORTED
*               LDT_ERR_ADR     (TTL_FAIL)
*               LDT_PROHBT_ADR  (OP_FILE_NOT_FOUND)
*
**  Description:              
*       uchRptOpeCount is already initialized in Rptcom.c from UI.
*       Read Daily or PTD DEPT report. 
*===========================================================================
*/
SHORT  RptDEPTRead(UCHAR uchMinorClass, UCHAR uchType, USHORT usNumber) 
{
    SHORT           sReturn = SUCCESS;

    /* EOD PTD Report Not Display Out MLD */
    if((uchMinorClass == CLASS_TTLSAVPTD) || (uchMinorClass == CLASS_TTLSAVDAY)){
        uchUifACRptOnOffMld = RPT_DISPLAY_OFF;
    }

    /* Edit Total Report */
    switch(uchType) {
    case RPT_DEPTIND_READ :                                         /* Individual read ? */
        sReturn = RptDEPTEditInd(uchMinorClass, uchType, usNumber);    
        break;

    case RPT_ALL_MDEPT_READ:                                    /* MDEPT All read ? */
        sReturn = RptMDEPTTtlEdit(uchMinorClass, uchType);               
        if ((uchRptOpeCount & RPT_HEADER_PRINT) &&
            (sReturn != SUCCESS) && (sReturn != RPT_ABORTED)) { 
            RptPrtError(sReturn);
        }
        break;

    case RPT_FIN_READ:                                          /* Final read ? */
        if (UieReadAbortKey() == UIE_ENABLE) {                  /* if Abort ?      */
                return(sReturn);
       }                                                                     
       if(RptPauseCheck() == RPT_ABORTED){
            return(RPT_ABORTED);
       }
        RptFeed(RPT_DEFALTFEED);                                /* line Feed  */
        RptMDEPTGrandTtlPrt(TRN_TTLR_ADR, &RptGTotal, uchType); /* Print All Total  */ 
        break;

    default:                                                    /* case of RPT_ALL_READ */
        sReturn = RptDEPTEditByMDEPT(uchMinorClass,             /* case of RPT_MDEPT_READ */
                                     uchType,                   /* case of RPT_EODALL_READ */
                                     usNumber);                 /* case of RPT_PTDALL_READ */
        break;
    }
    if (sReturn == RPT_ABORTED) {
        RptFeed(RPT_DEFALTFEED);                                /* line Feed  */
        MaintMakeAbortKey();                                    /* Print ABORTED   */
        MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);            /* Print Trailer */
        return(sReturn);
    }

    if ((uchType == RPT_EOD_ALLREAD) || (uchType == RPT_PTD_ALLREAD)) {
		TTLDEPT         TtlDept = {0};

        TtlDept.uchMajorClass = CLASS_TTLDEPT;
        TtlDept.uchMinorClass = uchMinorClass;  
        if (sReturn == SUCCESS) {
            if ((sReturn = SerTtlIssuedReset(&TtlDept, TTL_NOTRESET)) != TTL_SUCCESS) {
                sReturn = TtlConvertError(sReturn);
                RptPrtError(sReturn);
            }
        }
    }
    if (uchRptOpeCount & RPT_HEADER_PRINT) { 
        if ((uchType != RPT_EOD_ALLREAD) &&                         /* Not EOD ? */
           (uchType != RPT_DEPTIND_READ) &&                             /* Not IND ? */
           (uchType != RPT_PTD_ALLREAD) &&                          /* Not PTD ? */  
           (uchType != RPT_MDEPT_READ)) {                           /* Not MDEPT IND ? */
            MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);            /* Print Trailer    */
        }
        if ((uchType == RPT_EOD_ALLREAD) ||
            (uchType == RPT_PTD_ALLREAD)) {
            RptFeed(RPT_DEFALTFEED);                                /* Feed             */
        }
    }
    return(sReturn);
}

/*
*===========================================================================
**  Synopsis: SHORT RptDEPTEditInd(UCHAR uchMinorClass, 
*                                  UCHAR uchType,
*                                  USHORT usDEPTNumber) 
*               
*       Input:  UCHAR  uchMinorClass   : CLASS_TTLCURDAY
*                                      : CLASS_TTLCURPTD
*               UCHAR  uchType         :
*               USHORT usDEPTNumber    : DEPT number or MDEPT number
*
*      Output:  nothing
*
**  Return: SUCCESS      
*           RPT_ABORTED
*           LDT_ERR_ADR     (TTL_FAIL)
*           LDT_PROHBT_ADR  (OP_FILE_NOT_FOUND)
* 
**  Description:  
*       Edit individual DEPT report.
*
*===========================================================================
*/
static SHORT  RptDEPTEditInd(UCHAR uchMinorClass, UCHAR uchType, USHORT usDEPTNumber)
{    
    SHORT    sReturn;
    DEPTIF   Dpi = {0};
	RPTDEPT  RptDept = {0};
    TTLDEPT  TtlDept = {0};

    Dpi.usDeptNo = usDEPTNumber;                                        /* Set DEPT No for Dpi */
    if ((sReturn = CliOpDeptIndRead(&Dpi, 0)) != OP_PERFORM) {          /* Read OK ? */
        return(OpConvertError(sReturn));                                /* Return Error */
    }

	TtlDept.uchMajorClass = CLASS_TTLDEPT;                              /* Set Major Class for TtlDept */
    TtlDept.uchMinorClass = uchMinorClass;                              /* Set Minor Class for TtlDept */
    TtlDept.usDEPTNumber  = usDEPTNumber;                               /* Set DEPT No for TtlDept     */
    sReturn = SerTtlTotalRead(&TtlDept);                                /* Call Func for Total data    */
    if (sReturn != TTL_SUCCESS && sReturn != TTL_NOTINFILE) {
        return(TtlConvertError(sReturn));                               /* Return Error */
    }

	RptDEPTHeader(uchMinorClass, uchType);                              /* Call Header Func. */
    RptDept.DEPTTotal = TtlDept.DEPTTotal;                              /* Store DEPT total  for print */
    RptDept.uchMajorClass = CLASS_RPTDEPT;                              /* Set Major Class for print   */ 
    RptDept.usDEPTNumber = usDEPTNumber;                                /* Set DEPT No for print       */

    /* Calculate Percent based on MDC */                                                         
    if (!(Dpi.ParaDept.auchControlCode[0] & PLU_MINUS) &&               /* OPEN DEPT ?       */
       !(Dpi.ParaDept.auchControlCode[2] & PLU_HASH)) {     
        RptDept.uchMinorClass = CLASS_RPTDEPT_PRTTTLCNT1;               /* Set Minor Class     */
        RptDEPTAllTotal = TtlDept.DEPTAllTotal;                         /* Store to DeptAllTotal */
        RptDept.usPercent = RptDEPTPercent(&TtlDept.DEPTTotal);         /* Get Percent         */
    } else {                                                            /* CREDIT DEPT ?       */
        RptDept.uchMinorClass = CLASS_RPTDEPT_PRTTTLCNT2;               /* HASH DEPT ?         */ 
    }         
    RptFeed(RPT_DEFALTFEED);                                            /* Line Feed           */
    _tcsncpy(RptDept.aszMnemo, Dpi.ParaDept.auchMnemonic, PARA_DEPT_LEN); /* Copy DEPT Mnemo     */
	RptDept.aszMnemo[PARA_DEPT_LEN] = 0;                                // ensure mnemonic is zero terminated.
    RptDept.usPrintControl = usRptPrintStatus;                          /* Set Print Status    */
    if (RptCheckReportOnMld()) {
        uchRptMldAbortStatus = (UCHAR)MldDispItem(&RptDept, (USHORT)uchType);  /* Print each Element  */
        RptDept.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
    }

	PrtPrintItem(NULL, &RptDept);                                       /* Print each Element  */
    /* send print data to shared module */
    PrtShrPartialSend(RptDept.usPrintControl);

    RptGTotal.lAmount += RptDept.DEPTTotal.lAmount;                     /* Store GTotal in Static area */
    RptGTotal.lCounter += RptDept.DEPTTotal.lCounter;
    if (UieReadAbortKey() == UIE_ENABLE) {                              /* if Abort ?      */
        return(RPT_ABORTED);
    }                                                                   /*                 */
    if(RptPauseCheck() == RPT_ABORTED){
         return(RPT_ABORTED);
    }
    return(SUCCESS);
}                                                                        

/*
==========================================================================
**  Synopsis:  SHORT     RptMDEPTTtlEdit(UCHAR uchMinorClass,
*                                        UCHAR uchType);
*               
*       Input:  UCHAR  uchMinorClass
*               UCHAR  uchType 
*
*      Output:  nothing
*
**  Return:  SUCCESS
*            RPT_ABORTED
*
**  Description:  
*       Print out All Major DEPT Total. A/C #122
*===========================================================================
*/
static SHORT RptMDEPTTtlEdit(UCHAR uchMinorClass, UCHAR uchType)
{   
    SHORT           sReturn1,
                    sReturn2;
    UCHAR           uchFlag;             
    MDEPTIF         MDeptIf = {0};
	RPTDEPT         RptDept = {0};

    if (uchRptMldAbortStatus) {                         /* aborted by MLD */
        return (RPT_ABORTED);
    }

    RptDept.uchMajorClass = CLASS_RPTDEPT;                                  /* Set Major Class for print */
    RptDept.uchMinorClass = CLASS_RPTDEPT_PRTMDEPTTTL;                      /* Set Minor Class for print */
    uchFlag = 0;
                                                                            /* Loop 1 to 10 */
    for (MDeptIf.uchMajorDeptNo = 0; MDeptIf.uchMajorDeptNo <= MAX_MDEPT_NO; MDeptIf.uchMajorDeptNo++) {  
        MDeptIf.ulCounter = 0;                                              /* Initialize Dept counter */
		MDeptIf.usDeptNo = 0;                                               /* Initialize Dept # */
        while ((sReturn1 = SerOpMajorDeptRead(&MDeptIf, 0)) == OP_PERFORM) {  /* Do until Different MDEPT */
			TTLDEPT         TtlDept = {0};

            uchFlag = 1;
			TtlDept.uchMajorClass = CLASS_TTLDEPT;                              /* Set Major for Ttl */
			TtlDept.uchMinorClass = uchMinorClass;                              /* Set Major for Ttl */
            TtlDept.usDEPTNumber = MDeptIf.usDeptNo;                        /* Set DEPT # for Ttl */
            sReturn2 = SerTtlTotalRead(&TtlDept);                           /* Get Total, Saratoga */
            if (sReturn2 != TTL_SUCCESS && sReturn2 != TTL_NOTINFILE) {     /* Saratoga */
                return(TtlConvertError(sReturn2));
            }
            if (sReturn2 == TTL_SUCCESS) {  /* 09/26/00 */
                RptDept.DEPTTotal.lAmount += TtlDept.DEPTTotal.lAmount;         /* Add to SubTotal          */
                RptDept.DEPTTotal.lCounter += TtlDept.DEPTTotal.lCounter;   /* Set Counter for print    */
            }
        }   
        if (sReturn1 != OP_EOF) {
            return(OpConvertError(sReturn1));
        }
        if ((uchFlag != 0) && (MDeptIf.uchMajorDeptNo != 0)) { 
            RptDEPTHeader(uchMinorClass, uchType);
            RptFeed(RPT_DEFALTFEED);                                        /* Line Feed            */
            if(RptPauseCheck() == RPT_ABORTED){
                return(RPT_ABORTED);
            }
			RflGetMajorDeptMnem( RptDept.aszMnemo, MDeptIf.uchMajorDeptNo);
            RptDept.usPrintControl = usRptPrintStatus;
            if (RptCheckReportOnMld()) {
                uchRptMldAbortStatus = (UCHAR)MldDispItem(&RptDept, (USHORT)uchType);/* Print out            */
                RptDept.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
            }
            PrtPrintItem(NULL, &RptDept);                                   /* Print out            */
            uchFlag = 0;                                                    /* Initialize Flag */
            RptDept.DEPTTotal.lAmount = 0L;
            RptDept.DEPTTotal.lCounter = 0L;
            if (UieReadAbortKey() == UIE_ENABLE) {                          /* if Abort ?           */
                return(RPT_ABORTED);
            }                                                               
            if(RptPauseCheck() == RPT_ABORTED){
                return(RPT_ABORTED);
            }
        }
    }
    return(SUCCESS);          
} 
 
/*
*===========================================================================
**  Synopsis: SHORT RptDEPTEditByMDEPT( UCHAR uchMinorClass,
*                                       UCHAR uchType,
*                                       USHORT usMDEPTNumber)
*
*       Input:  UCHAR  uchMinorClass   :
*               UCHAR  uchType         : RPT_ALL_READ
*                                      : RPT_MDEPT_READ
*                                      : RPT_DEPTIND_READ
*                                      : RPT_MDEPT_ALL_READ
*                                      : RPT_EODALL_READ
*               USHORT usMDEPTNumber   : MDEPT number
*               
*      Output:  nothing
*
**     Return:  SUCCESS    
*               RPT_ABORTED
*
**  Description:  
*       Edit DEPT report by Major DEPT.
*       This function covers DEPT by Major, All DEPT by Major and
*       All DEPT Total by Major.
*===========================================================================
*/
static SHORT  RptDEPTEditByMDEPT(UCHAR uchMinorClass, UCHAR uchType, USHORT usMDEPTNumber)
{    
    UCHAR   uchFlagIn, uchFlagOut;
    USHORT  usIndx;                 /* Create to get matched DEPT */
    USHORT  usStatus;
    SHORT   sReturn;
    LTOTAL  Total;
	DEPTTBL Table[FLEX_DEPT_MAX + 1] = {0};   // plus one to hold the RPT_EOF record indicating end of list.

    usIndx = 0;                                                        /* Initialize Index */
    switch(uchType) {    
    case RPT_MDEPT_READ:                                                /* MDEPT read ? */
        if ((usMDEPTNumber < 1) || (usMDEPTNumber > MAX_MDEPT_NO)) {
            return(LDT_KEYOVER_ADR);
        }
        if (usMDEPTNumber == 1) {
            for (usMDEPTNumber = 0; usMDEPTNumber < 2; usMDEPTNumber++) {
                if ((sReturn = RptDEPTTableInd(uchMinorClass, &Table[usIndx], &usIndx, usMDEPTNumber)) != SUCCESS) {   /* Fail to make Table ? */
                    return(sReturn);
                }
            }
            usMDEPTNumber = 1;                                              /* Reset the number from 2 to 1 */     

        } else {
            sReturn = RptDEPTTableInd(uchMinorClass, &Table[usIndx], &usIndx, usMDEPTNumber);
            if (sReturn != SUCCESS && sReturn != LDT_NTINFL_ADR) {
                return(sReturn);
            }
        }
        if (usIndx == 0) {                                             /* Saratoga */
            return(LDT_NTINFL_ADR);
        }
        RptDEPTHeader(uchMinorClass, uchType);                          /* Call Header Func */
        RptFeed(RPT_DEFALTFEED);                                        /* Line Feed */
        RptMDEPTNamePrt((UCHAR)usMDEPTNumber, uchType);                 /* Print MDEPT name */
        memset(&Total, 0, sizeof(Total));                               /* Clear Buffer */
        uchFlagIn = 0x00;
        for (usStatus = RPT_OPEN; usStatus <= RPT_HASH; usStatus++) {   /* normal,credit,hash */
            uchFlagOut = 0x00;
            usIndx = 0;
            sReturn = RptDEPT(uchType, usStatus, usMDEPTNumber, &Table[usIndx], &Total, &usIndx, &uchFlagOut);

            if (sReturn == RPT_ABORTED) {
                return(sReturn);
            }
            uchFlagIn |= uchFlagOut;

            if ((uchFlagIn == 0x01) && (usStatus == RPT_CREDIT)) {
                RptFeed(RPT_DEFALTFEED);                                /* Feed            */
                RptMDEPTGrandTtlPrt(TRN_GTTLR_ADR, &Total, uchType);    /* Print Grand Ttl */
            }
            if (UieReadAbortKey() == UIE_ENABLE) {                      /* if Abort ?      */
                return(RPT_ABORTED);
            }                                                           /*                 */
            if(RptPauseCheck() == RPT_ABORTED){
                 return(RPT_ABORTED);
            }
        }           
        break;

    default:                                                            /* case of RPT_ALL_READ    */
                                                                        /* case of RPT_EOD_ALLREAD */
                                                                        /* case RPT_PTD_ALLREAD    */                
        sReturn = RptDEPTTableAll(uchMinorClass, &Table[0]);            /* Saratoga */
        if (sReturn != SUCCESS && sReturn != LDT_NTINFL_ADR) {
            return(sReturn);
        }
        RptDEPTHeader(uchMinorClass, uchType);                          /* Call Header Func. */
        for (usStatus = RPT_OPEN; usStatus <= RPT_HASH; usStatus++) {   /* Normal,Credit,Hash      */
			USHORT  usMDEPTNo;

            memset(&Total, 0, sizeof(Total));                           /* Clear for RptTotal      */
            usIndx = 0;                                                /* Initialize Index */
            uchFlagIn = 0x00;
            for (usMDEPTNo = 1; usMDEPTNo <= MAX_MDEPT_NO; usMDEPTNo++) {    /* MDEPT 1 to 10   */
                uchFlagOut = 0x00;
                sReturn = RptDEPT(uchType, usStatus, usMDEPTNo, &Table[usIndx], &Total, &usIndx, &uchFlagOut);        
                if (sReturn == RPT_ABORTED) {       
                    return(sReturn);
                }
                uchFlagIn |= uchFlagOut;                                   
            }
            if (uchFlagIn == 0x01) {                                    /* Exist printed item ? */
				USHORT  usTransAddr;            /* Address of Transaction Mnemonics */

                RptFeed(RPT_DEFALTFEED);                                /* Feed            */
                if (usStatus == RPT_OPEN) {
                    usTransAddr = TRN_TTLR_ADR;                         /* Set Plus Ttl Mnemo Address */
                } else if (usStatus == RPT_CREDIT) {
                    usTransAddr = TRN_MTTLR_ADR;                        /* Set Minus Ttl Mnemo Address */
                } else {
                    usTransAddr = TRN_HTTLR_ADR;                        /* Set Hash Ttl Mnemo Address */
                }
                RptMDEPTGrandTtlPrt(usTransAddr, &Total, uchType);      /* Print 3-Total     */
            }
            if ((uchFlagIn == 0x01) &&                                  /* Exist printed item ? */
               (usStatus == RPT_CREDIT)) {                              /* after Credit DEPT finished ?  */
                RptFeed(RPT_DEFALTFEED);                                /* Feed            */
                RptMDEPTGrandTtlPrt(TRN_GTTLR_ADR, &RptGTotal, uchType);/* Print GrandTotal   */
            }
        }
    }
    return(SUCCESS);
}

/*
*===========================================================================
**  Synopsis: SHORT RptDEPTTableInd(UCHAR uchMinorClass,
*                                   DEPTTBL *pTable, 
*                                   UCHAR   *puchIndx,  
*                                   USHORT  usMDEPTNumber)
*               
*       Input:  UCHAR    uchMinorClass
*               DEPTTBL  *pTable 
*               UCHAR    *puchIndx
*               USHORT   usMDEPTNumber
*
*      Output:  DEPTTBL *pTable
*
**  Return:     SUCCESS   
*               LDT_ERR_ADR ----- (TTL_FAIL)
*               LDT_LOCK_ADR ---- (TTL_LOCKED)
*                            ---- (OP_LOCK)
*               LDT_PROHBT_ADR -- (OP_FILE_NOT_FOUND) 
*
**  Description:  
*       Save One Serched DEPT data in DEPT Table which is created in local area.
*       Caluculate All Major DEPT total.
*       MDC #89  BIT0 check. Do not store the data in the Table if Total
*       and counter are zero.
*===========================================================================
*/
static SHORT RptDEPTTableInd(UCHAR uchMinorClass, DEPTTBL *pTable, USHORT *pusIndx, USHORT usMDEPTNumber)
{
    SHORT  sReturn; 
	MDEPTIF MDeptIf = {0};
                                                                                                                                              
	memset (&RptDEPTAllTotal, 0, sizeof(RptDEPTAllTotal));  // zero out the cummulative totals which we are about to calculate.

    MDeptIf.uchMajorDeptNo = (UCHAR)usMDEPTNumber; 
    MDeptIf.ulCounter = MDeptIf.usDeptNo =  0;
    while ((sReturn = SerOpMajorDeptRead(&MDeptIf, 0)) == OP_PERFORM) { /* MDEPT read OK ? */
		TTLDEPT TtlDept = {0};

		TtlDept.uchMajorClass = CLASS_TTLDEPT;
        TtlDept.uchMinorClass = uchMinorClass;
        TtlDept.usDEPTNumber = MDeptIf.usDeptNo;                        /* Set Major for Ttl */
        sReturn = SerTtlTotalRead(&TtlDept);                            /* Saratoga */
        if (sReturn != TTL_SUCCESS && sReturn != TTL_NOTINFILE) {
            return(TtlConvertError(sReturn));                           /* Error */
        }
        /* Set 3-Type status */
        /* Negative Hash Dept, V3.3 */
        if ((MDeptIf.ParaDept.auchControlCode[2] & PLU_HASH) &&             /* HASH ?           */     
            (MDeptIf.ParaDept.auchControlCode[0] & PLU_MINUS)) {            /* CREDIT ?         */
            /* hash affect to gross total */
            if (CliParaMDCCheck(MDC_DEPT1_ADR, ODD_MDC_BIT1)) {
                pTable->uchStatus = RPT_CREDIT;                     /* Status = 2       */
            } else {
                pTable->uchStatus = RPT_HASH;                       /* Status = 3       */
            }
        } else if (MDeptIf.ParaDept.auchControlCode[2] & PLU_HASH) {               /* HASH ?           */     
            pTable->uchStatus = RPT_HASH;                       /* Status = 3       */
        } else if (MDeptIf.ParaDept.auchControlCode[0] & PLU_MINUS) {       /* CREDIT ?         */
            pTable->uchStatus = RPT_CREDIT;                     /* Status = 2       */
        } else {                                                            /* OPEN ?           */
            pTable->uchStatus = RPT_OPEN;                       /* Status = 1       */
        }

        if (usMDEPTNumber == 0) {
            pTable->uchMDEPT = 1;
        } else {
            pTable->uchMDEPT = MDeptIf.uchMajorDeptNo;          /* Set MDEPT No     */ 
        }
        pTable->usDEPT = MDeptIf.usDeptNo;                    /* Set Dept No      */ 
        pTable->DEPTTotal.lAmount = TtlDept.DEPTTotal.lAmount;
        pTable->DEPTTotal.lCounter = TtlDept.DEPTTotal.lCounter;           
        _tcsncpy(pTable->auchMnemo, MDeptIf.ParaDept.auchMnemonic, OP_DEPT_NAME_SIZE);
        pTable++;
        (*pusIndx)++;
        RptDEPTAllTotal.lAmount += TtlDept.DEPTTotal.lAmount;
        RptDEPTAllTotal.lCounter += TtlDept.DEPTTotal.lCounter;           
    }
    if (sReturn != OP_EOF) {
        return(OpConvertError(sReturn));
    }
    if (usMDEPTNumber != 0) {
        pTable->uchMDEPT = RPT_EOF;                             /* Store 0xff for EOF     */
    }
    return(SUCCESS);
}

/*
*===========================================================================
**  Synopsis: SHORT RptDEPTTableAll(UHCAR uchMinorClass,
*                                   DEPTTBL *pTable)
* 
*               
*       Input:  UCHAR    uchMinorClass
*               DEPTTBL  *pTable
*
*      Output:  DEPTTBL *DptTbl
*
**  Return:     SUCCESS   
*               LDT_ERR_ADR   (TTL_FAIL)
*
**  Description:  
*       Save All DEPT data in DEPT Table which is created in local area.
*       Caluculate All Major DEPT total.
*       MDC #89  BIT0 check. Do not storuchMajorDeptNo;e the data in the Table if Total
*       and counter are zero.
*===========================================================================
*/
static SHORT RptDEPTTableAllVar(UCHAR uchMinorClass, DEPTTBL *pTable, SHORT (*totalFunc)(void *d))
{
    USHORT   usIndx = 0;
    SHORT    sReturn; 
	MDEPTIF  MDeptIf = {0};
                                                                                                                                              
	memset (&RptDEPTAllTotal, 0, sizeof(RptDEPTAllTotal));  // zero out the cummulative totals which we are about to calculate.

//	MDeptIf.uchMajorDeptNo = MDPT_NO1_ADR;                /*  by MDEPT from first to last Major Department  */
	MDeptIf.uchMajorDeptNo = 0;                /*  by MDEPT from first to last Major Department  */
	for ( ; MDeptIf.uchMajorDeptNo <= MAX_MDEPT_NO; MDeptIf.uchMajorDeptNo++) {
		MDeptIf.ulCounter = MDeptIf.usDeptNo = 0;
        while ((sReturn = SerOpMajorDeptRead(&MDeptIf, 0)) == OP_PERFORM) {  
			TTLDEPT TtlDept = {0};

			TtlDept.uchMajorClass = CLASS_TTLDEPT;
            TtlDept.uchMinorClass = uchMinorClass;
            TtlDept.usDEPTNumber = MDeptIf.usDeptNo;             /* Set department number for Total */
            TtlDept.uchMajorDEPTNo = MDeptIf.uchMajorDeptNo;     /* Set Major department number for Total */
            sReturn = totalFunc(&TtlDept);                            /* Saratoga */
            if (sReturn != TTL_SUCCESS && sReturn != TTL_NOTINFILE) {
                if (sReturn == TTL_SIZEOVER) {
                    continue;
                } else {
					(pTable + usIndx)->uchMDEPT = RPT_EOF;                         /* Store 0xff for EOF     */
                    return(TtlConvertError(sReturn));
                }
            }
            /* Set 3-Type status */
            /* Negative Hash Dept, V3.3 */
            if ((MDeptIf.ParaDept.auchControlCode[2] & PLU_HASH) &&             /* HASH ?           */     
                (MDeptIf.ParaDept.auchControlCode[0] & PLU_MINUS)) {            /* CREDIT ?         */
                /* hash affect to gross total */
                if (CliParaMDCCheck(MDC_DEPT1_ADR, ODD_MDC_BIT1)) {
                    (pTable + usIndx)->uchStatus = RPT_CREDIT;                 /* Status = 2       */
                } else {
                    (pTable + usIndx)->uchStatus = RPT_HASH;                   /* Status = 3       */
                }
            } else if (MDeptIf.ParaDept.auchControlCode[2] & PLU_HASH) {           /* HASH ?           */     
                (pTable + usIndx)->uchStatus = RPT_HASH;                   /* Status = 3       */
            } else if (MDeptIf.ParaDept.auchControlCode[0] & PLU_MINUS) {   /* CREDIT ?         */
                (pTable + usIndx)->uchStatus = RPT_CREDIT;                 /* Status = 2       */
            } else {                                                        /* OPEN ?           */
                (pTable + usIndx)->uchStatus = RPT_OPEN;                   /* Status = 1       */
            }

            if (MDeptIf.uchMajorDeptNo == 0) {
                (pTable + usIndx)->uchMDEPT = 1;                           /* Set MDEPT No     */
            } else {
                (pTable + usIndx)->uchMDEPT = MDeptIf.uchMajorDeptNo;      /* Set MDEPT No     */
            }
            (pTable + usIndx)->usDEPT = MDeptIf.usDeptNo;                  /* Set Dept No      */ 
            (pTable + usIndx)->DEPTTotal = TtlDept.DEPTTotal;
            _tcsncpy((pTable + usIndx)->auchMnemo, MDeptIf.ParaDept.auchMnemonic, OP_DEPT_NAME_SIZE);
			RptDEPTAllTotal.lAmount += TtlDept.DEPTTotal.lAmount;
			RptDEPTAllTotal.lCounter += TtlDept.DEPTTotal.lCounter;
            usIndx++;                                             
        } /*----- End of While Loop -----*/

        if (sReturn != OP_EOF) {
			(pTable + usIndx)->uchMDEPT = RPT_EOF;                         /* Store 0xff for EOF     */
            return(OpConvertError(sReturn));
        }
    } /*----- End of For Loop -----*/

    (pTable + usIndx)->uchMDEPT = RPT_EOF;                                 /* Store 0xff for EOF     */
    return(SUCCESS);
}

SHORT RptDEPTTableAll(UCHAR uchMinorClass, DEPTTBL *pTable)
{
	return RptDEPTTableAllVar(uchMinorClass, pTable, SerTtlTotalRead);
}

// helper function to provide an adapter to the function TtlTotalReadNoSemCheck()
// which is used to read the department totals without performing the semaphore
// checks that are used the function TtlTotalRead() which is used by the function
// SerTtlTotalRead() when reading totals.
//
// This logic assumes we are on the Master Terminal. It is provided as a way to
// allow End of Day from a remote application through the PCIF interface and generate
// the proper Saved Total File format for the Deaprtment Totals.
static SHORT totalFunc(void *pTotal)
{
	USHORT  usDataLen = 0;

	return TtlTotalReadNoSemCheck(pTotal, &usDataLen);
}

SHORT RptDEPTTableAllLocal(UCHAR uchMinorClass,	DEPTTBL *pTable)
{
	return RptDEPTTableAllVar(uchMinorClass, pTable, totalFunc);
}

/*
*===========================================================================
**  Synopsis:  VOID     RptMDEPTNamePrt(UCHAR uchMDEPTNo)
*               
*       Input:  UCHAR   uchMDEPTNo     :
*  
*      Output:  nothing
*
**  Return:   UCHAR    uchDEPTIndx   0        : Not Found
*                                    Number   : Found Index
*                    
**  Description:  
*       Print out Major DEPT name.
*===========================================================================
*/
static VOID RptMDEPTNamePrt(UCHAR  uchMDEPTNo, UCHAR uchType)
{
	RPTDEPT        RptDept = {0};

    if (uchRptMldAbortStatus) {                         /* aborted by MLD */
        return;
    }

    RptDept.uchMajorClass = CLASS_RPTDEPT;                              /* Set Major Class */
    RptDept.uchMinorClass = CLASS_RPTDEPT_PRTMAJOR;                     /* Set Minor Class */ 
    RptDept.uchMajorDEPTNo = uchMDEPTNo;                                /* Set Major DEPT */
	RflGetMajorDeptMnem(RptDept.aszMnemo, uchMDEPTNo);
    RptDept.usPrintControl = usRptPrintStatus;
    if (RptCheckReportOnMld()) {
        uchRptMldAbortStatus = (UCHAR)MldDispItem(&RptDept, (USHORT)uchType);
        RptDept.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
    }
    PrtPrintItem(NULL, &RptDept);
}

/*
*===========================================================================
**  Synopsis:  SHORT     RptDEPT(UCHAR uchType,  
*                                USHORT usStatus, 
*                                UCHAR uchMDEPTNo,
*                                DEPTTBL *pTable, 
*                                LTOTAL *pTotal,
*                                UCHAR  *puchIndx,
*                                UCHAR  *puchFlagOut,
*                                UCHAR  *puchWgtFlag) 
*
*               
*       Input:    UCHAR   uchType       :
*                 USHORT  usStatus      :
*                 UCHAR   uchMDEPTNo    :
*                 DEPTTBL *pTable       :
*                 LTOTAL  *pTotal       :
*                 UCHAR   *puchIndx     :
*                 UCHAR   *puchFlagOut  :
*                 UCHAR   *puchWgtFlag  :
*
*      Output:    DEPTTBL *pTable       :
*                 LTOTAL  *pTotal       :
*                 UCHAR   *puchFlag     : 
*
**  Return: SUCCESS        :
*           RPT_ABORTED    :
*
**  Description:  
*       Classify the status to OPEN, CREDIT and HASH.
*       Print  Total and/or Grand Total.
*===========================================================================
*/                                                                          
static SHORT RptDEPT(UCHAR   uchType, 
              USHORT  usStatus, 
              USHORT  usMDEPTNo,
              DEPTTBL *pTable, 
              LTOTAL  *pTotal,
              USHORT  *pusIndx,
              UCHAR   *puchFlagOut) 
{
    UCHAR       uchMinor;
    DCURRENCY   lSubTotal;             /* Create to store SubTotal Ttl in each MDEPT */
    LONG        lSubCounter;           /* Create to store SubTotal Co in each MDEPT  */
    
    if (uchRptMldAbortStatus) {                         /* aborted by MLD */
        return (RPT_ABORTED);
    }

    lSubTotal = 0L;                                                     /* Clear lSubTotal */
    lSubCounter = 0L;                                                   /* Clear lSubCounter */
    while (pTable->uchMDEPT == (UCHAR)usMDEPTNo) {                      /* Do until Different MDEPT */
        switch(usStatus) {
        case RPT_OPEN:                                                  /* OPEN Dept ?       */
            uchMinor = CLASS_RPTDEPT_PRTTTLCNT1;
            break;

        default:                                                        /* case of RPT_CREDIT */
                                                                        /* case of RPT_HASH */
            uchMinor = CLASS_RPTDEPT_PRTTTLCNT2;
            break;
        }
        if (pTable->uchStatus == (UCHAR)usStatus) {
            if (*puchFlagOut == 0x00) {
                if (uchType != RPT_MDEPT_READ) {
                    RptFeed(RPT_DEFALTFEED);                            /* Line Feed */
                    RptMDEPTNamePrt((UCHAR)usMDEPTNo, uchType);         /* Print MDEPT name */
                }
                RptFeed(RPT_DEFALTFEED);                                /* Line Feed */
            }
            /*----- NOT PRINT  MDC #89 BIT0 is ON (Total is 0) -----*/
            if ((pTable->DEPTTotal.lAmount == 0L) && (pTable->DEPTTotal.lCounter == 0L) &&     
                (CliParaMDCCheck(MDC_DEPTREPO_ADR, ODD_MDC_BIT0))) {    /* Not Print out ?      */
                  ;   /* Do nothing */
            } else {  /*----- PRINT even Total is 0 -----*/
                RptDEPTPrt(uchMinor, pTable, uchType);
            }
            lSubTotal += pTable->DEPTTotal.lAmount;                     /* Add to SubTotal   */
            lSubCounter += pTable->DEPTTotal.lCounter;                  /* Add to SubCounter */
            *puchFlagOut |= 0x01;
        } /*----- End of Case -----*/
        pTable++;
        (*pusIndx)++;
        if (UieReadAbortKey() == UIE_ENABLE) {                      /* if Abort ?      */
            return(RPT_ABORTED);
        }                                                           /*                 */
        if(RptPauseCheck() == RPT_ABORTED){
             return(RPT_ABORTED);
        }
    } /*----- End of While Loop -----*/

    if (usStatus != RPT_HASH) {
        RptGTotal.lAmount += lSubTotal;                                 /* Store to Grand Total */    
        RptGTotal.lCounter += lSubCounter;
    }
    pTotal->lAmount += lSubTotal;                                        /* Store to Total  */ 
    pTotal->lCounter += lSubCounter;
    if (*puchFlagOut == 0x01) {
		RPTDEPT    RptDept = {0};

        RptFeed(RPT_DEFALTFEED);
        if (uchRptMldAbortStatus) {                         /* aborted by MLD */
            return (RPT_ABORTED);
        }
        RptDept.uchMajorClass = CLASS_RPTDEPT;
        RptDept.uchMinorClass = CLASS_RPTDEPT_PRTTTLCNT3;               /* Set to Minor Class   */
        RptDept.DEPTTotal.lAmount = lSubTotal;                          /* Store to DEPT ttl for Prt */
        RptDept.DEPTTotal.lCounter =  lSubCounter;      
        RflGetTranMnem (RptDept.aszMnemo,TRN_STTLR_ADR);                /* Copy Mnemo      */
        RptDept.usPrintControl = usRptPrintStatus;
        if (RptCheckReportOnMld()) {
            uchRptMldAbortStatus = (UCHAR)MldDispItem(&RptDept, (USHORT)uchType);/* print subtotal */
            RptDept.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
        }
        PrtPrintItem(NULL, &RptDept);                                   /* print subtotal */
        /* send print data to shared module */
        PrtShrPartialSend(RptDept.usPrintControl);
    }

    if (uchRptMldAbortStatus) {                         /* aborted by MLD */
        return (RPT_ABORTED);
    }

    return(SUCCESS);
}


/*
*===========================================================================
**  Synopsis:   VOID  RptDEPTPrt(UHCAR uchMinorClass, DEPTTBL *pTable,
*                                UCHAR *puchWgtFlag)
*               
*       Input:    UCHAR   uchMinorClass    :
*                 DEPTTBL *pTable          :
*
*      Output:    DEPTTBL *pTable          :
*
**  Return:     nothing
*
**  Description:  
*       Print out all DEPT element by OPEN, CREDIT or HASH.
*       Also print out Sub Total.
*
*===========================================================================
*/
static VOID   RptDEPTPrt(UCHAR uchMinorClass, DEPTTBL *pTable, UCHAR uchType)
{
	RPTDEPT RptDept = {0};

    if (uchRptMldAbortStatus) {                         /* aborted by MLD */
        return;
    }

    RptDept.uchMajorClass = CLASS_RPTDEPT;
    RptDept.uchMinorClass = uchMinorClass;
    RptDept.usDEPTNumber = pTable->usDEPT;
    RptDept.usPercent = RptDEPTPercent(&pTable->DEPTTotal);
    RptDept.DEPTTotal.lAmount = pTable->DEPTTotal.lAmount;      /* Set Amount for print */     
    RptDept.DEPTTotal.lCounter = pTable->DEPTTotal.lCounter; 
    _tcsncpy(RptDept.aszMnemo, pTable->auchMnemo, PARA_DEPT_LEN); /* Copy Mnemo       */
    RptDept.usPrintControl = usRptPrintStatus;
    if (RptCheckReportOnMld()) {
        uchRptMldAbortStatus = (UCHAR)MldDispItem(&RptDept, (USHORT)uchType);
        RptDept.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
    }
    PrtPrintItem(NULL, &RptDept);
    /* send print data to shared module */
 
    PrtShrPartialSend(RptDept.usPrintControl);

}

/*
*===========================================================================
**  Synopsis: SHORT RptDEPTHeader(UCHAR uchMinorClass, 
*                                 UCHAR uchType) 
*               
*       Input:  UCHAR uchMinorClass
*               UCHAR uchType
*
*      Output:  nothing
*
**  Return:     SUCCESS      :
*               RPT_ABORTED  :   
*               
**  Description:  
*       Print out header 
*===========================================================================
*/
static SHORT RptDEPTHeader(UCHAR uchMinorClass, UCHAR uchType) 
{
    /* Check First Operation */
    if (!(uchRptOpeCount & RPT_HEADER_PRINT)) {                 /* First Operation ?     */
		SHORT	   sReturn;
		UCHAR      uchSpecMnemo, uchRptName, uchRptType, uchTmpType, uchACNo;
		TTLDEPT    TtlDept = {0};

        RptPrtStatusSet(uchMinorClass);                         /* Check and set Print Status */
        memset(&RptGTotal, 0, sizeof(RptGTotal));               /* Clear GrandTotal      */

        /* Select Special Mnemonics for Header */
        switch(uchMinorClass) {
        case CLASS_TTLCURDAY :                                  /* Case of Daily READ */
            uchSpecMnemo = SPC_DAIRST_ADR;                      /* Set Daily Special Mnemonics */
            uchRptType = RPT_READ_ADR;                          /* Set Read Mnemonics */        
            break;
   
        case CLASS_TTLSAVDAY :                                  /* Case of Daily RESET */
            uchSpecMnemo = SPC_DAIRST_ADR;                      /* Set Daily Special Mnemonics */
            uchRptType = RPT_RESET_ADR;                         /* Set Reset Mnemonics */
            break;

        case CLASS_TTLCURPTD :                                  /* Case of PTD READ */
            uchSpecMnemo = SPC_PTDRST_ADR;                      /* Set Daily Special Mnemonics */
            uchRptType = RPT_READ_ADR;                          /* Set Read Mnemonics */
            break;

        case CLASS_TTLSAVPTD :                                  /* Case of PTD RESET */
            uchSpecMnemo = SPC_PTDRST_ADR;                      /* Set PTD Special Mnemonics */
            uchRptType = RPT_RESET_ADR;                         /* Set Reset Mnemonics */
            break;

        default:            
            return(RPT_ABORTED);
/*            PifAbort(MODULE_RPT_ID, FAULT_INVALID_DATA); */
        }

        /* Set Report name & A/C number */
        if (uchType == RPT_ALL_MDEPT_READ) {
            uchRptName = RPT_MDEPT_ADR;                         /* Set A/C name  */
            uchACNo = AC_MAJDEPT_RPT;                           /* Set A/C # 122 */
            uchType = 0;
        } else {
            uchRptName = RPT_DEPT_ADR;                          /* Set A/C name  */
            uchACNo = AC_DEPTSALE_READ_RPT;                     /* Set A/C #26   */
        }

        /* Print Header Name */
        uchTmpType = uchType;
        if (uchType == RPT_EOD_ALLREAD || uchType == RPT_PTD_ALLREAD) {
            uchTmpType =  RPT_ALL_READ;
        }

        if ((uchType != RPT_EOD_ALLREAD) && (uchType != RPT_PTD_ALLREAD)) {                                                           
            MaintIncreSpcCo(SPCO_CONSEC_ADR);                   /* Count Up Consecutive No. */
        }
        MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class          */              
                        RPT_ACT_ADR,                            /* Report Name Address       */
                        uchRptName,                             /* Report Name Address       */
                        uchSpecMnemo,                           /* Special Mnemonics Address */
                        uchRptType,                             /* Report Name Address       */
                        uchTmpType,                             /* Report Name Address       */
                        uchACNo,                                /* A/C Number                */
                        0,                                      /* Reset Type                */                                       
                        PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit         */                                       
                                                                    
        if (UieReadAbortKey() == UIE_ENABLE) {                  /* if Abort ?      */
            return(RPT_ABORTED);
        }                                                       /*                 */
        if(RptPauseCheck() == RPT_ABORTED){
             return(RPT_ABORTED);
        }

		TtlDept.uchMajorClass = CLASS_TTLDEPT;                  /* Set Major for Ttl */
        TtlDept.uchMinorClass = uchMinorClass;                  /* Set Minor for Ttl */
        TtlDept.usDEPTNumber = 1;                               /* Set Number for Ttl */
        sReturn = SerTtlTotalRead(&TtlDept);                              /* Get Total */
		if (sReturn != TTL_SUCCESS && sReturn != TTL_NOTINFILE) {
			return(TtlConvertError(sReturn));                               /* Return Error */
	    }
        if ((uchMinorClass == CLASS_TTLCURDAY) || (uchMinorClass == CLASS_TTLCURPTD)) {  /* Current Daily report or Current PTD report ? */
			TtlGetNdate (&TtlDept.ToDate);      /* Get Current Date/Time and  Set Period To Date */
        }
        RptPrtTime(TRN_PFROM_ADR, &TtlDept.FromDate);           /* Print PERIOD FROM         */
        RptPrtTime(TRN_PTO_ADR, &TtlDept.ToDate);               /* Print PERIOD TO           */

        if (UieReadAbortKey() == UIE_ENABLE) {                  /* if Abort ?      */
            return(RPT_ABORTED);
        }                                                       
        if(RptPauseCheck() == RPT_ABORTED){
             return(RPT_ABORTED);
        }

        uchRptOpeCount |= RPT_HEADER_PRINT;                     /* Header already prints     */
    }   
    return(SUCCESS);
}

/*
*===========================================================================
**  Synopsis:  USHORT     RptDEPTPercent(LTOTAL *pDeptTtl)
*               
*       Input:    LTOTAL  *pDeptTtl
*
*      Output:    LTOTAL  *pDeptTtl
*
**  Return:     Data of Percent
*           
**  Description:  Calculate Percent for departmental total. The percent is
*                 either percent of currency amount or percent of unit count
*                 depending on MDC 89 Bit C setting.
*===========================================================================
*/    
static USHORT  RptDEPTPercent(LTOTAL *pDeptTtl)
{
	D13DIGITS    d13Product;
	DCURRENCY    lTotalAmt = 0, lRptAmt = 0;
    LONG         lPercent;
                        
    if (CliParaMDCCheckField (MDC_DEPTREPO_ADR, MDC_PARAM_BIT_C)) {    /* MDC #89 check */
		lTotalAmt = pDeptTtl->lAmount;
		lRptAmt = RptDEPTAllTotal.lAmount;
    } else {
		lTotalAmt = pDeptTtl->lCounter;
		lRptAmt = RptDEPTAllTotal.lCounter;
    }

    /* A Numerator is less than 0 ?   OR A Denominator is less than or equal 0 ? */
    if ((lTotalAmt < 0L) || (lRptAmt <= 0L)) {
        return(RPT_OVERFLOW);                 
    }

	d13Product = 100000L;
	d13Product *= lTotalAmt;
	d13Product /= lRptAmt;
    lPercent = (d13Product + 5) / 10L;
    if (lPercent > 32767L) {
        return(RPT_OVERFLOW);                                               /* Print ******% */
    } else {
        return((USHORT)lPercent);
    }
}
    
/*
*===========================================================================
**  Synopsis:  VOID     RptMDEPTGrandTtlPrt(UCHAR uchTransAddr
*                                           LTOTAL *pTotal, 
*                                           USHORT usStatus);
*               
*       Input:  UCHAR    uchTransAddr   : RPT_TTLR_ADR --- Plus Total
*                                       : RPT_MTTLR_ADR -- Minus Total
*                                       : RPT_HTTLR_ADR -- Hash Total
*                                       : RPT_GTTLR_ADR -- Grand Total (Plus + Minus)  
*               LTOTAL   *pTotal        : 
*
*      Output:  LTOTAL   *pTotal        :
*
**  Return:     nothing
*
**  Description:  
*       Print out Total or GrandTotal depends on the condition of 2nd formal
*       parameter : RptTotal is Total, RptGTotal is Grand Total.
*
*===========================================================================
*/
static VOID RptMDEPTGrandTtlPrt(USHORT usTransAddr, LTOTAL *pTotal, UCHAR uchType)
{
	RPTDEPT   RptDept = {0};

    if (uchRptMldAbortStatus) {                         /* aborted by MLD */
        return;
    }

    RptDept.uchMajorClass = CLASS_RPTDEPT;
    RptDept.uchMinorClass = CLASS_RPTDEPT_PRTTTLCNT3;                                   
    RptDept.DEPTTotal = *pTotal;                        /* Store GrandTotal */
    RflGetTranMnem (RptDept.aszMnemo, usTransAddr);     /* Get the transaction Mnemonics   */
    RptDept.usPrintControl = usRptPrintStatus;
    if (RptCheckReportOnMld()) {
        uchRptMldAbortStatus = (UCHAR)MldDispItem(&RptDept, (USHORT)uchType);/* Print GrandTotal */
        RptDept.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
    }
    PrtPrintItem(NULL, &RptDept);                                   /* Print GrandTotal */
 
    /* send print data to shared module */
    PrtShrPartialSend(RptDept.usPrintControl);
}

/**** End of File ****/