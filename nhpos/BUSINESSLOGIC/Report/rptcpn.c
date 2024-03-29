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
* Title       : Coupon Report Module                         
* Category    : Report For Supervisor, NCR 2170 US Hospitality Application          
* Program Name: RPTCPN.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*       RptCpnDayRead()               : Read Daily Coupon Report Module
*       RptCpnPTDRead()               : Read PTD Coupon Report Module
*       RptCpnRead()                  : Read Daily and PTD Coupon report
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
*    -  -  : 00.00.01   : Waki      : initial                                   
* Jul-27-95: 03.00.03   : M.Ozawa   : Add Report on Display
* Aug-11-99: 03.05.00   : M.Teraki  : R3.5 remove WAITER_MODEL
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
============================================================================
;                      I N C L U D E     F I L E s                         
;===========================================================================
*/

#include	<tchar.h>
#include <string.h>
#include <stdio.h>

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
#include <csttl.h>
#include <csstbttl.h>
#include <csstbstb.h>
#include <cscas.h>
#include <cswai.h>
#include <csstbwai.h>
#include <csstbcas.h>
#include <report.h>
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <appllog.h>
#include <mld.h>
#include <mldsup.h>
#include <plu.h>

#include "rptcom.h"

/*************************************************************/
LTOTAL   gTotalCpn;     /* Grand Total Calc Area */

/*
*===========================================================================
**  Synopsis: SHORT RptCpnRead(UCHAR uchMinorClass, UCHAR uchType, UCHAR uchNumber) 
*               
*       Input:  UCHAR  uchMinorClass   : CLASS_TTLCURDAY
*                                      : CLASS_TTLCURPTD
*               UCHAR  uchType         : RPT_IND_READ  
*                                      : RPT_ALL_READ
*               UCHAR  uchNumber       : Cpn number
*
*      Output:  nothing
*
**     Return:  SUCCESS     
*               RPT_ABORTED
*               LDT_ERR_ADR     (TTL_FAIL)
*               LDT_PROHBT_ADR  (OP_FILE_NOT_FOUND)
*               LDT_KEYOVER_ADR
*
**  Description:              
*       Read Daily or PTD Cpn report. 
*===========================================================================
*/
SHORT  RptCpnRead(UCHAR uchMinorClass, UCHAR uchType, UCHAR uchNumber) 
{
    
    SHORT           sReturn = SUCCESS;
    PARAFLEXMEM      FlexMem;

    if(uchType == RPT_IND_READ){
        FlexMem.uchMajorClass = CLASS_PARAFLEXMEM;                          /* Set Major for # of Record */
        FlexMem.uchAddress = FLEX_CPN_ADR;                                 /* Set Address for # of Record */
        CliParaRead(&FlexMem);                                              /* Get # of Record */
        if ((uchNumber < 1) || (uchNumber > (UCHAR)FlexMem.ulRecordNumber)) {   /* Saratoga */
            return(LDT_KEYOVER_ADR);            
        }
    }
    /* Edit Total Report */

    RptCpnHeader(uchMinorClass, uchType);                              /* Call Header Func. */

    switch(uchType) {
    case RPT_IND_READ :                                 /* Individual read ? */
        sReturn = RptCpnIndProc(uchMinorClass, uchNumber, uchType);
        if (sReturn == RPT_ABORTED) {
            RptFeed(RPT_DEFALTFEED);                                /* line Feed  */
            MaintMakeAbortKey();                                    /* Print ABORTED   */
        }
        return(sReturn);
        break;

    case RPT_EOD_ALLREAD:                               /* EOD report */
    case RPT_PTD_ALLREAD:                               /* PTD report */
    case RPT_ALL_READ:                                  /* All read ? */
        sReturn = RptCpnAllProc(uchMinorClass, uchType);
        break;

    case RPT_FIN_READ:                                  /* Individual Read END */
        if(uchRptOpeCount & RPT_HEADER_PRINT){          /* Not First Operation */
            RptFeed(RPT_DEFALTFEED);                        /* Line Feed        */
            RptCpnGrandTtlPrt(&gTotalCpn, uchType);         /* Grand Total Print */
            MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);    /* Print Trailer */
            return(sReturn);
        }
        return(sReturn);
        break;

    default:
        return(LDT_SEQERR_ADR);
                                                                
    }
    if (sReturn == RPT_ABORTED) {
        RptFeed(RPT_DEFALTFEED);                                /* line Feed  */
        MaintMakeAbortKey();                                    /* Print ABORTED   */
    }
    if((uchType != RPT_EOD_ALLREAD) && (uchType != RPT_PTD_ALLREAD)){
        MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);            /* Print Trailer */
    }
    return(sReturn);
}

/*
*===========================================================================
**  Synopsis: SHORT RptCpnIndProc(UCHAR uchMinorClass, 
*                                 UCHAR uchCpnNumber) 
*               
*       Input:  UCHAR  uchMinorClass   : CLASS_TTLCURDAY
*                                      : CLASS_TTLCURPTD
*               UCHAR  uchCpnNumber   : Cpn number
*
*      Output:  nothing
*
**  Return: SUCCESS      
*           RPT_ABORTED
*           LDT_ERR_ADR     (TTL_FAIL)
*           LDT_PROHBT_ADR  (OP_FILE_NOT_FOUND)
* 
**  Description:  
*       individual Cpn report.
*
*===========================================================================
*/
SHORT  RptCpnIndProc(UCHAR uchMinorClass, USHORT usCpnNumber, UCHAR uchType)
{    
    SHORT    sReturn;
    CPNIF    CpnIf;
    RPTCPN   RptCpn;
    TTLCPN   TtlCpn;

    if (uchRptMldAbortStatus) {                         /* aborted by MLD */
        return (RPT_ABORTED);
    }
    memset(&RptCpn, 0, sizeof(RptCpn));
    if(!(CliParaMDCCheck(MDC_PCTL_ADR, EVEN_MDC_BIT0))) {
        RptCpn.usPrintControl |=  PRT_JOURNAL;
    }
    if(!(CliParaMDCCheck(MDC_PCTL_ADR, EVEN_MDC_BIT1))) {
        RptCpn.usPrintControl |=  PRT_RECEIPT;
    }
    usRptPrintStatus = RptCpn.usPrintControl;
                                                                                                        
    TtlCpn.uchMajorClass = CLASS_TTLCPN;                              /* Set Major Class for TtlCpn */
    TtlCpn.uchMinorClass = uchMinorClass;                              /* Set Minor Class for TtlCpn */
    TtlCpn.usCpnNumber = usCpnNumber;                               /* Set Cpn No for TtlCpn     */
    if ((sReturn = SerTtlTotalRead(&TtlCpn)) != TTL_SUCCESS) {         /* Call Func for Total data    */
        return(TtlConvertError(sReturn));                               /* Return Error */
    }
    RptFeed(RPT_DEFALTFEED);                                            /* Line Feed           */

    RptCpn.uchMajorClass = CLASS_RPTCPN;            /* Coupon Report Set */
    RptCpn.uchMinorClass = CLASS_RPTCPN_ITEM;       /* Report Type Set */
    RptCpn.usCpnNumber = usCpnNumber;               /* Coupon Number Set */
    RptCpn.CpnTotal.lAmount = TtlCpn.CpnTotal.lAmount;       /* Amount Set */
    RptCpn.CpnTotal.lCounter =(LONG) TtlCpn.CpnTotal.sCounter;     /* Counter Set */

    CpnIf.uchCpnNo = usCpnNumber;                  /* Coupon Number Set */
    CliOpCpnIndRead(&CpnIf, 0);                     /* Mnemonic Get */
    _tcsncpy(RptCpn.aszMnemo, CpnIf.ParaCpn.aszCouponMnem, PARA_CPN_LEN); /* Copy Mnemonics    */
    
    if (RptCheckReportOnMld()) {
        uchRptMldAbortStatus = (UCHAR)MldDispItem(&RptCpn, (USHORT)uchType);  /* Print each Element  */
        RptCpn.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
    }
    PrtPrintItem(NULL, &RptCpn);                    /* Individual Print For One Coupon */
                                                    /* (Number,Mnemonic,Counter,Total) */

    gTotalCpn.lAmount += TtlCpn.CpnTotal.lAmount;   /* Total Amount Culc */
    gTotalCpn.lCounter += TtlCpn.CpnTotal.sCounter; /* Total Counter Culc */

    /* send print data to shared module */

    PrtShrPartialSend(RptCpn.usPrintControl);

    return(SUCCESS);
}                                                                        

/*
*===========================================================================
**  Synopsis: SHORT RptCpnAllProc(UCHAR uchMinorClass)
*               
*       Input:  UCHAR  uchMinorClass   : CLASS_TTLCURDAY
*                                      : CLASS_TTLCURPTD
*                                      : CLASS_TTLSAVDAY
*                                      : CLASS_TTLSAVPTD
*
*      Output:  nothing
*
**  Return: SUCCESS      
*           RPT_ABORTED
*           LDT_ERR_ADR     (TTL_FAIL)
*           LDT_PROHBT_ADR  (OP_FILE_NOT_FOUND)
* 
**  Description:  
*       All Cpn report.
*
*===========================================================================
*/
SHORT  RptCpnAllProc(UCHAR uchMinorClass, UCHAR uchType)
{    
    SHORT    sReturn;
    CPNIF    CpnIf = { 0 };
    RPTCPN   RptCpn = { 0 };
    TTLCPN   TtlCpn = { 0 };
    USHORT   usCpnNumberMax = RflGetMaxRecordNumberByType(FLEX_CPN_ADR);
    USHORT   usCpnNumber;

    if (uchRptMldAbortStatus) {                         /* aborted by MLD */
        return (RPT_ABORTED);
    }

    RptCpn.uchMajorClass = CLASS_RPTCPN;            /* Coupon Report Set */
    RptCpn.uchMinorClass = CLASS_RPTCPN_ITEM;       /* Coupon Report Set */
    RptFeed(RPT_DEFALTFEED);                        /* Line Feed           */
    if(RptPauseCheck() == RPT_ABORTED){
        return(RPT_ABORTED);
    }

    if((uchMinorClass == CLASS_TTLCURDAY) || (uchMinorClass == CLASS_TTLCURPTD)){
        if(!(CliParaMDCCheck(MDC_PCTL_ADR, EVEN_MDC_BIT0))) {
            RptCpn.usPrintControl |=  PRT_JOURNAL;
        }
        if(!(CliParaMDCCheck(MDC_PCTL_ADR, EVEN_MDC_BIT1))) {
            RptCpn.usPrintControl |=  PRT_RECEIPT;
        }
    }else{
        if(!(CliParaMDCCheck(MDC_PCTL_ADR, EVEN_MDC_BIT2))) {
            RptCpn.usPrintControl |=  PRT_JOURNAL;
        }
        if(!(CliParaMDCCheck(MDC_PCTL_ADR, EVEN_MDC_BIT3))) {
            RptCpn.usPrintControl |=  PRT_RECEIPT;
        }
    }    
    usRptPrintStatus = RptCpn.usPrintControl;

    TtlCpn.uchMajorClass = CLASS_TTLCPN;            /* Set Major Class for TtlCpn */
    TtlCpn.uchMinorClass = uchMinorClass;           /* Set Minor Class for TtlCpn */
    for (usCpnNumber = 1; usCpnNumber <= usCpnNumberMax; usCpnNumber++){  /* Saratoga */
        TtlCpn.usCpnNumber = usCpnNumber;                              /* Set Cpn No for TtlCpn     */
        if ((sReturn = SerTtlTotalRead(&TtlCpn)) != TTL_SUCCESS) {         /* Call Func for Total data    */
            return(TtlConvertError(sReturn));                               /* Return Error */
        }
        if (((TtlCpn.CpnTotal.lAmount==0) && (TtlCpn.CpnTotal.sCounter==0)) &&
        (CliParaMDCCheck(MDC_COUPON_ADR, EVEN_MDC_BIT0))) {    /* Not Print out ?      */
            continue;   /* next Coupon Number */
        }
        gTotalCpn.lAmount += TtlCpn.CpnTotal.lAmount;   /* Total Amount Culc */
        gTotalCpn.lCounter += TtlCpn.CpnTotal.sCounter; /* Total Counter Culc */
        RptCpn.usCpnNumber = usCpnNumber;             /* Coupon Number Set */
        RptCpn.CpnTotal.lAmount = TtlCpn.CpnTotal.lAmount;       /* Amount Set */
        RptCpn.CpnTotal.lCounter = (LONG)TtlCpn.CpnTotal.sCounter;     /* Counter Set */

        CpnIf.uchCpnNo = usCpnNumber;                  /* Coupon Number Set */
        CliOpCpnIndRead(&CpnIf, 0);                     /* Mnemonic Get */
        _tcsncpy(RptCpn.aszMnemo, CpnIf.ParaCpn.aszCouponMnem, PARA_CPN_LEN); /* Copy Mnemonics    */
    
        if (RptCheckReportOnMld()) {
            uchRptMldAbortStatus = (UCHAR)MldDispItem(&RptCpn, (USHORT)uchType);  /* Print each Element  */
            RptCpn.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
        }
        PrtPrintItem(NULL, &RptCpn);                    /* Individual Print For One Coupon */
                                                        /* (Number,Mnemonic,Counter,Total) */
        RptFeed(RPT_DEFALTFEED);                                            /* Line Feed           */
        /* abort check */
        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            return(RPT_ABORTED);
            break;
       }
       if(RptPauseCheck() == RPT_ABORTED){
            return(RPT_ABORTED);
       }
        
        /* send print data to shared module */
        PrtShrPartialSend(RptCpn.usPrintControl);
    }

    RptCpnGrandTtlPrt(&gTotalCpn, uchType);    /* Total Print */

    if((uchMinorClass == CLASS_TTLSAVDAY) || (uchMinorClass == CLASS_TTLSAVPTD)){
        TtlCpn.uchMajorClass = CLASS_TTLCPN;
        TtlCpn.uchMinorClass = uchMinorClass;  
        if ((sReturn = SerTtlIssuedReset(&TtlCpn, TTL_NOTRESET)) != TTL_SUCCESS) {
            sReturn = TtlConvertError(sReturn);
            RptPrtError(sReturn);
            return(sReturn);
        }
    }
    return(SUCCESS);
}                                                                        

SHORT  ItemGenerateAc30Report(UCHAR uchMajorClass, UCHAR uchMinorClass, UCHAR uchType, FILE* fpFile)
{
    static const TCHAR  auchPrtThrmSupCPNFile1[] = _T("  %4d   %-12s   %12ld   %12l$\r\n");

    SHORT       sReturn;

    if (!fpFile) return -1;

    if (fpRptElementStreamFile) {
        TTLCPN   TtlCpn = { 0 };
        USHORT   usCpnNumberMax = RflGetMaxRecordNumberByType(FLEX_CPN_ADR);
        TCHAR  aszBuffer[128] = { 0 };

        // find out the from and to date by pulling the data from the
        // first coupon total and using that for the report heading.
        TtlCpn.uchMajorClass = CLASS_TTLCPN;                  /* Set Major for Ttl */
        TtlCpn.uchMinorClass = uchMinorClass;                  /* Set Minor for Ttl */
        TtlCpn.usCpnNumber = 1;                               /* Set Number for Ttl */
        sReturn = SerTtlTotalRead(&TtlCpn);                              /* Get Total */
        if (sReturn != TTL_SUCCESS && sReturn != TTL_NOTINFILE) {
            return(TtlConvertError(sReturn));                               /* Return Error */
        }
        // Current Daily report or Current PTD report ?
        if ((uchMinorClass == CLASS_TTLCURDAY) || (uchMinorClass == CLASS_TTLCURPTD)) {               /* Current PTD report ? */
            TtlGetNdate(&TtlCpn.ToDate);
        }

        if (RptDescriptionCheckType(RPTREGFIN_OUTPUT_HTML)) {
            wchar_t  aszMnemo[PARA_TRANSMNEMO_LEN + 1] = { 0 };

            switch (uchMinorClass) {
            case CLASS_TTLCURDAY:
                fprintf(fpRptElementStreamFile, "<h2>AC 30 Coupon Report - Current Daily Totals</h2>\n");
                break;
            case CLASS_TTLCURPTD:
                fprintf(fpRptElementStreamFile, "<h2>AC 30 Coupon Report - Current Period To Day Totals</h2>\n");
                break;
            case CLASS_TTLSAVDAY:
                fprintf(fpRptElementStreamFile, "<h2>AC 30 Coupon Report - Saved Daily Totals</h2>\n");
                break;
            case CLASS_TTLSAVPTD:
                fprintf(fpRptElementStreamFile, "<h2>AC 30 Coupon Report - Saved Period To Day Totals</h2>\n");
                break;
            }

            if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {  /* DD/MM/YY */
                fprintf(fpRptElementStreamFile, "<h3>%S: %2.2d/%2.2d/%2.2d %2.2d:%2.2d</br>", RflGetTranMnem(aszMnemo, TRN_PFROM_ADR), TtlCpn.FromDate.usMDay, TtlCpn.FromDate.usMonth,
                    TtlCpn.FromDate.usYear, TtlCpn.FromDate.usHour, TtlCpn.FromDate.usMin);
                fprintf(fpRptElementStreamFile, "%S: %2.2d/%2.2d/%2.2d %2.2d:%2.2d</h3>\r\n", RflGetTranMnem(aszMnemo, TRN_PTO_ADR), TtlCpn.ToDate.usMDay, TtlCpn.ToDate.usMonth,
                    TtlCpn.ToDate.usYear, TtlCpn.ToDate.usHour, TtlCpn.ToDate.usMin);
            } else {  /* MM/DD/YY */
                fprintf(fpRptElementStreamFile, "<h3>%S: %2.2d/%2.2d/%2.2d %2.2d:%2.2d</br>", RflGetTranMnem(aszMnemo, TRN_PFROM_ADR), TtlCpn.FromDate.usMonth, TtlCpn.FromDate.usMDay,
                    TtlCpn.FromDate.usYear, TtlCpn.FromDate.usHour, TtlCpn.FromDate.usMin);
                fprintf(fpRptElementStreamFile, "%S: %2.2d/%2.2d/%2.2d %2.2d:%2.2d</h3>\r\n", RflGetTranMnem(aszMnemo, TRN_PTO_ADR), TtlCpn.ToDate.usMonth, TtlCpn.ToDate.usMDay,
                    TtlCpn.ToDate.usYear, TtlCpn.ToDate.usHour, TtlCpn.ToDate.usMin);
            }

            fprintf(fpRptElementStreamFile, "<table border=\"1\" cellpadding=\"8\">\r\n<tr><th>Name</th><th>Amount</th><th>Count</th></tr>\r\n");
        }

        TtlCpn.uchMajorClass = CLASS_TTLCPN;            /* Set Major Class for TtlCpn */
        TtlCpn.uchMinorClass = uchMinorClass;           /* Set Minor Class for TtlCpn */
        for (USHORT usCpnNumber = 1; usCpnNumber <= usCpnNumberMax; usCpnNumber++) {  /* Saratoga */
            CPNIF    CpnIf = { 0 };
            wchar_t  aszMnemo[PARA_CPN_LEN + 1] = { 0 };

            TtlCpn.usCpnNumber = usCpnNumber;                              /* Set Cpn No for TtlCpn     */
            if ((sReturn = SerTtlTotalRead(&TtlCpn)) != TTL_SUCCESS) {         /* Call Func for Total data    */
                return(TtlConvertError(sReturn));                               /* Return Error */
            }
            if (((TtlCpn.CpnTotal.lAmount == 0) && (TtlCpn.CpnTotal.sCounter == 0)) &&
                (CliParaMDCCheck(MDC_COUPON_ADR, EVEN_MDC_BIT0))) {    /* Not Print out ?      */
                continue;   /* next Coupon Number */
            }
            gTotalCpn.lAmount += TtlCpn.CpnTotal.lAmount;   /* Total Amount Culc */
            gTotalCpn.lCounter += TtlCpn.CpnTotal.sCounter; /* Total Counter Culc */

            CpnIf.uchCpnNo = usCpnNumber;                  /* Coupon Number Set */
            CliOpCpnIndRead(&CpnIf, 0);                     /* Mnemonic Get */
            _tcsncpy(aszMnemo, CpnIf.ParaCpn.aszCouponMnem, PARA_CPN_LEN); /* Copy Mnemonics    */

            RptElementStream2(0, TtlCpn.usCpnNumber, aszMnemo, NULL, 0, TtlCpn.CpnTotal.lAmount, TtlCpn.CpnTotal.sCounter, CLASS_RPTCPN_ITEM, 0);
        }

        {
            wchar_t  aszMnemo[PARA_CPN_LEN + 1] = { 0 };

            RflGetTranMnem(aszMnemo, TRN_TTLR_ADR);                 /* Copy Mnemonics   */
            RptElementStream2(0, 0, aszMnemo, NULL, 0, gTotalCpn.lAmount, gTotalCpn.lCounter, CLASS_RPTCPN_TOTAL, 0);
        }
        if (RptDescriptionCheckType(RPTREGFIN_OUTPUT_HTML)) {
            fprintf(fpRptElementStreamFile, "</table>\r\n");
        }
    }

    return SUCCESS;
}

/*
*===========================================================================
**  Synopsis: SHORT RptCpnHeader(UCHAR uchMinorClass, 
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
SHORT RptCpnHeader(UCHAR uchMinorClass, UCHAR uchType) 
{
    UCHAR      uchSpecMnemo,
               uchRptName,
               uchRptType,
               uchTmpType,
               uchACNo;
    TTLCPN    TtlCpn;
	SHORT		sReturn;

    /* Check First Operation */

    if (!(uchRptOpeCount & RPT_HEADER_PRINT)) {                 /* First Operation ?     */
        RptPrtStatusSet(uchMinorClass);                         /* Check and set Print Status */
        memset(&gTotalCpn, 0, sizeof(gTotalCpn));               /* Grand Total Area Clear */

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

        uchRptName = RPT_CPN_ADR;                         /* Set A/C name  */
        if ((uchMinorClass == CLASS_TTLSAVDAY) || (uchMinorClass == CLASS_TTLSAVPTD)){
            uchACNo = AC_CPN_RESET_RPT;                     /* Set A/C #40   */
        }else {
            uchACNo = AC_CPN_READ_RPT;                      /* Set A/C #30 */
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
        TtlCpn.uchMajorClass = CLASS_TTLCPN;                  /* Set Major for Ttl */
        TtlCpn.uchMinorClass = uchMinorClass;                  /* Set Minor for Ttl */
        TtlCpn.usCpnNumber = 1;                               /* Set Number for Ttl */
        sReturn = SerTtlTotalRead(&TtlCpn);                              /* Get Total */
		if (sReturn != TTL_SUCCESS && sReturn != TTL_NOTINFILE) {
			return(TtlConvertError(sReturn));                               /* Return Error */
		}
		// Current Daily report or Current PTD report ?
        if ((uchMinorClass == CLASS_TTLCURDAY) || (uchMinorClass == CLASS_TTLCURPTD)) {               /* Current PTD report ? */
			TtlGetNdate (&TtlCpn.ToDate);
        }
        RptPrtTime(TRN_PFROM_ADR, &TtlCpn.FromDate);           /* Print PERIOD FROM         */
        RptPrtTime(TRN_PTO_ADR, &TtlCpn.ToDate);               /* Print PERIOD TO           */

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
**  Synopsis:  VOID     RptCpnGrandTtlPrt(LTOTAL *pTotal);
*               
*               LTOTAL   *pTotal        : 
*
*      Output:  LTOTAL   *pTotal        :
*
**  Return:     nothing
*
**  Description:  
*       Print out Total 
*
*===========================================================================
*/
VOID RptCpnGrandTtlPrt(LTOTAL *pTotal, UCHAR uchType)
{
	RPTCPN   RptCpn = {0};

    if (uchRptMldAbortStatus) {                         /* aborted by MLD */
        return;
    }
    RptCpn.uchMajorClass = CLASS_RPTCPN;
    RptCpn.uchMinorClass = CLASS_RPTCPN_TOTAL;
    RptCpn.CpnTotal.lAmount = pTotal->lAmount;                    /* Store GrandTotal */
    RptCpn.CpnTotal.lCounter = pTotal->lCounter;                  /* Store Counter    */
    RflGetTranMnem (RptCpn.aszMnemo, TRN_TTLR_ADR);                 /* Copy Mnemonics   */
    RptCpn.usPrintControl = usRptPrintStatus;
    if (RptCheckReportOnMld()) {
        uchRptMldAbortStatus = (UCHAR)MldDispItem(&RptCpn, (USHORT)uchType);  /* Print each Element  */
        RptCpn.usPrintControl &= PRT_JOURNAL;            /* Reset Receipt print status so only goes to Electronic Journal if set */
    }
    PrtPrintItem(NULL, &RptCpn);                                   /* Print GrandTotal */

    /* send print data to shared module */
    PrtShrPartialSend(RptCpn.usPrintControl);
}


/*
*=============================================================================
**  Synopsis: SHORT RptCpnReset(UCHAR uchMinorClass, UCHAR uchType) 
*               
*       Input:  uchMinorClass   : CLASS_TTLSAVDAY
*                               : CLASS_TTLSAVPTD
*               uchType         : RPT_PRT_RESET
*                               : RPT_ONLY_RESET
*      Output:  nothing
*
**     Return:  SUCCESS             : Successful 
*               LDT_ERR_ADR         : (TTL_NOT_ISSUED)
*
**  Description:  
*        Reset Daily or PTD COUPON File.
*        
*===============================================================================
*/
SHORT RptCpnReset(UCHAR uchMinorClass, UCHAR uchType)
{
    UCHAR   uchSpecReset,
            uchSpecMnemo;
    SHORT   sReturn;
/*    CPNIF   CpnIf; */
    TTLCPN  TtlCpn;

    /*----- Reset All Coupon Total -----*/

    TtlCpn.uchMajorClass = CLASS_TTLCPN;
    TtlCpn.uchMinorClass = uchMinorClass;
    if ((sReturn = SerTtlIssuedCheck(&TtlCpn, TTL_NOTRESET)) != TTL_ISSUED) {
        return(TtlConvertError(sReturn));
    }

    if (uchMinorClass == CLASS_TTLSAVDAY){
        TtlCpn.uchMinorClass = CLASS_TTLCURDAY;
    }else{
        TtlCpn.uchMinorClass = CLASS_TTLCURPTD;
    }
    
    if ((sReturn = SerTtlTotalReset(&TtlCpn,0)) != TTL_SUCCESS) {   
        return(TtlConvertError(sReturn));
    }

    if (uchMinorClass == CLASS_TTLSAVDAY) {
        uchSpecReset = SPCO_EODRST_ADR;                         /* Set Daily Reset Counter */
    } else {
        uchSpecReset = SPCO_PTDRST_ADR;                         /* Set PTD Reset Counter */
    }

    MaintIncreSpcCo(uchSpecReset);                        /* Count Up Reset Counter */

    /*----- Only Reset Case -----*/

    if (uchType == RPT_ONLY_RESET) {

        if (uchMinorClass == CLASS_TTLSAVDAY) {
            uchSpecMnemo = SPC_DAIRST_ADR;                      /* Set Daily Special Mnemonics */
        } else {
            uchSpecMnemo = SPC_PTDRST_ADR;                      /* Set PTD Special Mnemonics */
        }
        /*----- Print Header Name -----*/
                                                        
        MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class */              
                        RPT_ACT_ADR,                            /* Report Name Address */
                        RPT_CPN_ADR,                            /* Report Name Address */
                        uchSpecMnemo,                           /* Special Mnemonics Address */
                        RPT_RESET_ADR,                          /* Report Name Address */
                        0,                                      /* Not Print */
                        AC_CPN_RESET_RPT,                       /* A/C Number */
                        uchType,                                /* Reset Type */
                        PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit         */

                                                                   
        MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);            /* Print Trailer */
    }
    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT RptCpnLock(VOID) 
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
SHORT RptCpnLock(VOID)
{
    SHORT  sReturn;

    if (((sReturn = SerCasAllLock()) != CAS_PERFORM) &&
        (sReturn != CAS_FILE_NOT_FOUND)) {
        return(CasConvertError(sReturn));                           /* Return Error */
    }

    return(SUCCESS);                                                /* Return SUCCESS */
}

/*
*=============================================================================
**  Synopsis: VOID RptCpnUnLock(VOID) 
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
VOID RptCpnUnLock(VOID)
{
    /*----- Unlock files -----*/

    SerCasAllUnLock();                      /* Unlock All Cashier File */
}

/* --- End of Source File --- */