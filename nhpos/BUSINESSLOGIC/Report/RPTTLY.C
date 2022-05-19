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
* Title       : Report EPT/CPM Tally
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application          
* Program Name: RPTTLY.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs1 /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               RptTallyReadOrReset(TYPE)    : Read or Reset TALLY Report (EPT OR CPM)
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.  :   Name     : Description
* Nov.15.93 :02.00.02  :M.Yamamoto  : Initial
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
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
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <maint.h>
#include <csttl.h>
#include <csop.h>
#include <csstbstb.h>
#include <report.h>
#include <plu.h>
#include <csstbfcc.h>
#include <cpm.h>
#include <eept.h>
#include <csstbcpm.h>
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <plu.h>
#include "rptcom.h"
#include <rfl.h>
                                          /* 123456789012345 */
static TCHAR FARCONST auchPrtSupSendPass[] = _T("SEND PASS\0");
static TCHAR FARCONST auchPrtSupSendFail[] = _T("SEND FAIL\0");
static TCHAR FARCONST auchPrtSupRcvPass[]  = _T("RECEIVE PASS\0");
static TCHAR FARCONST auchPrtSupRcvFail[]  = _T("RECEIVE FAIL\0");
static TCHAR FARCONST auchPrtSupUnsRcv[]   = _T("UNSOLI RECEIVE\0");
static TCHAR FARCONST auchPrtSupInvData[]  = _T("INVALID DATA\0");
static TCHAR FARCONST auchPrtSupIhcFail[]  = _T("IHC SEND FAIL\0");

/*
*=============================================================================
**  Synopsis: SHORT RptTallyReadOrReset( UCHAR uchType ) 
*               
*       Input:  uchType     -   RPT_TYPE_CPM_PRT
*                               RPT_TYPE_CPM_RST
*                               RPT_TYPE_EPT_PRT
*                               RPT_TYPE_EPT_RST
*                           -
*      Output:  Nothing
*
**  Return: SUCCESS             : Successful 
*           LDT_PROHBT_ADR      : This Terminal is not as master.
*
**  Description: Read and Reset tally of EPT or CPM.
*===============================================================================
*/

SHORT RptTallyReadOrReset( UCHAR uchType ) 
{
 
    UCHAR       uchReportName;
    UCHAR       uchReportType;
    SHORT       sReturn = SUCCESS;
    MAINTERRORCODE  MaintErrCode;

   
    /* Print Header Name */

    switch (uchType) {
        case RPT_TYPE_CPM_PRT:    
                                uchReportName = (UCHAR)RPT_CPM_ADR;
                                uchReportType = (UCHAR)RPT_READ_ADR;
                                break;
        case RPT_TYPE_CPM_RST:  
                                uchReportName = (UCHAR)RPT_CPM_ADR;
                                uchReportType = (UCHAR)RPT_RESET_ADR;
                                break;          
        case RPT_TYPE_EPT_PRT:
                                uchReportName = (UCHAR)RPT_EPT_ADR;
                                uchReportType = (UCHAR)RPT_READ_ADR;
                                break;          
        case RPT_TYPE_EPT_RST:
                                uchReportName = (UCHAR)RPT_EPT_ADR;
                                uchReportType = (UCHAR)RPT_RESET_ADR;
                                break;
        default:                return(SUCCESS);
                                break;
    }

    MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class          */              
                    RPT_ACT_ADR,                            /* Report Name Address       */
                    uchReportName,                          /* Report Name Address       */
                    0,                                      /* Special Mnemonics Address */
                    uchReportType,                          /* Report Name Address       */
                    0,                                      /* Report Type               */
                    AC_TALLY_CPM_EPT,                       /* A/C Number                */
                    0,                                      /* Reset Type                */
                    PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit         */
 
    if (uchType == RPT_TYPE_CPM_PRT || uchType == RPT_TYPE_CPM_RST) {
        if (RptCpmTally(uchType) != SUCCESS) {
            sReturn = LDT_PROHBT_ADR;
        }
    } else {
        if (RptEptTally(uchType) != SUCCESS) {
            sReturn = LDT_PROHBT_ADR;
        }
    }

    /* Check Error or Not */

    if (sReturn != SUCCESS) {
        MaintErrCode.uchMajorClass = CLASS_MAINTERRORCODE;
        MaintErrCode.usPrtControl = PRT_RECEIPT | PRT_JOURNAL;
        MaintErrCode.sErrorCode = LDT_PROHBT_ADR;
        PrtPrintItem(NULL, &MaintErrCode);
    }
        
    /* Make Trailer */

    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);

    return(sReturn);
}

/*
*=============================================================================
**  Synopsis: SHORT RptCpmTally( UCHAR uchType ) 
*               
*       Input:  RPT_TYPE_CPM_PRT : Only Report CPM Tally.
*               RPT_TYPE_CPM_RST : Report and Reset CPM Tally.
*
*      Output:  SUCCESS          : SUCCESS
*               LDT_PROHBT_ADR   : this Terminal is not as master.
*
**  Return: SUCCESS              : Successful 
*           LDT_PROHBT_ADR       : This Terminal is not as master.
*
**  Description: Only Report Tally or Report and Reset Tally for CPM.
*===============================================================================
*/

SHORT RptCpmTally( UCHAR uchType ) 
{
    CPMEPT_TALLY        Tally;
    PARACPMTALLY    ParaTally;
    UCHAR          uchCpmType;

    if (uchType == RPT_TYPE_CPM_RST) {
        uchCpmType = CLI_RESET_TLY_CPM;
    } else {
        uchCpmType = CLI_READ_TLY_BOTH;
    }

    if (SerCpmEptReadResetTally(uchCpmType, &Tally) != CPM_RET_SUCCESS) {
        return LDT_PROHBT_ADR;
    }

    memset(&ParaTally, 0, sizeof(ParaTally));

    RptPrtStatusSet(CLASS_PARACPMEPTTALLY);
    ParaTally.usPrintControl = PRT_RECEIPT | PRT_JOURNAL;

    ParaTally.uchMajorClass = CLASS_PARACPMEPTTALLY;

    // SEND PASS    xxxxx 
    _tcsncpy(ParaTally.auchMnemo, auchPrtSupSendPass, _tcslen(auchPrtSupSendPass));
    ParaTally.usTallyData  = Tally.CpmTally.usPMSSendOK;
    PrtPrintItem(NULL, &ParaTally);

    // SEND FAIL    xxxxx 
	memset(&ParaTally.auchMnemo, _T('\0'), sizeof(ParaTally.auchMnemo));
    _tcsncpy(ParaTally.auchMnemo, auchPrtSupSendFail, _tcslen(auchPrtSupSendFail));
    ParaTally.usTallyData  = Tally.CpmTally.usPMSSendErr;
    PrtPrintItem(NULL, &ParaTally);

    // RECV PASS    xxxxx 
	memset(&ParaTally.auchMnemo, _T('\0'), sizeof(ParaTally.auchMnemo));
    _tcsncpy(ParaTally.auchMnemo, auchPrtSupRcvPass, _tcslen(auchPrtSupRcvPass)); 
    ParaTally.usTallyData  = Tally.CpmTally.usPMSRecResp;
    PrtPrintItem(NULL, &ParaTally);

    // RECV FAIL    xxxxx 
	memset(&ParaTally.auchMnemo, _T('\0'), sizeof(ParaTally.auchMnemo));
    _tcsncpy(ParaTally.auchMnemo, auchPrtSupRcvFail, _tcslen(auchPrtSupRcvFail));
    ParaTally.usTallyData  = Tally.CpmTally.usTimeout;
    PrtPrintItem(NULL, &ParaTally);

    // UNSOL. RECV. xxxxx 
	memset(&ParaTally.auchMnemo, _T('\0'), sizeof(ParaTally.auchMnemo));
    _tcsncpy(ParaTally.auchMnemo, auchPrtSupUnsRcv, _tcslen(auchPrtSupUnsRcv));
    ParaTally.usTallyData  = Tally.CpmTally.usPMSRecUnsoli;
    PrtPrintItem(NULL, &ParaTally);

    // INV.  DATA   xxxxx 
	memset(&ParaTally.auchMnemo, _T('\0'), sizeof(ParaTally.auchMnemo));
    _tcsncpy(ParaTally.auchMnemo, auchPrtSupInvData, _tcslen(auchPrtSupInvData));
    ParaTally.usTallyData  = Tally.CpmTally.usPMSThrowResp;
    PrtPrintItem(NULL, &ParaTally);

    // IHC ERROR    xxxxx 
	memset(&ParaTally.auchMnemo, _T('\0'), sizeof(ParaTally.auchMnemo));
    _tcsncpy(ParaTally.auchMnemo, auchPrtSupIhcFail, _tcslen(auchPrtSupIhcFail));
    ParaTally.usTallyData  = Tally.CpmTally.usIHCSendErr;
    PrtPrintItem(NULL, &ParaTally);
    return SUCCESS; /* ### Add (2171 for Win32) V1.0 */
}    
    
/*
*=============================================================================
**  Synopsis: SHORT RptEptTally( UCHAR uchType ) 
*               
*       Input:  RPT_TYPE_EPT_PRT : Only Report EPT Tally.
*               RPT_TYPE_EPT_RST : Report and Reset EPT Tally.
*
*      Output:  SUCCESS          : SUCCESS
*               LDT_PROHBT_ADR   : this Terminal is not as master.
*
**  Return: SUCCESS              : Successful 
*           LDT_PROHBT_ADR       : This Terminal is not as master.
*
**  Description: Only Report Tally or Report and Reset Tally for EPT.
*===============================================================================
*/

SHORT RptEptTally( UCHAR uchType ) 
{
    PARACPMTALLY    ParaTally;
    CPMEPT_TALLY        Tally;
    UCHAR          uchEptType;

    if (uchType == RPT_TYPE_EPT_RST) {
        uchEptType = CLI_RESET_TLY_EPT;
    } else {
        uchEptType = CLI_READ_TLY_BOTH;
    }

    if (SerCpmEptReadResetTally(uchEptType, &Tally) != CPM_RET_SUCCESS) {
        return LDT_PROHBT_ADR;
    }

    memset(&ParaTally, 0, sizeof(ParaTally));

    ParaTally.uchMajorClass = CLASS_PARACPMEPTTALLY;
    RptPrtStatusSet(CLASS_PARACPMEPTTALLY);
    ParaTally.usPrintControl = PRT_RECEIPT | PRT_JOURNAL;


    /* SEND PASS    xxxxx */
    _tcsncpy(ParaTally.auchMnemo, auchPrtSupSendPass, _tcslen(auchPrtSupSendPass));
    ParaTally.usTallyData  = Tally.EptTally.usEPTSendOK;
    PrtPrintItem(NULL, &ParaTally);

    /* SEND FAIL    xxxxx */
    _tcsncpy(ParaTally.auchMnemo, auchPrtSupSendFail, _tcslen(auchPrtSupSendFail));
    ParaTally.usTallyData  = Tally.EptTally.usEPTSendErr;
    PrtPrintItem(NULL, &ParaTally);


    /* RECV PASS    xxxxx */
    _tcsncpy(ParaTally.auchMnemo, auchPrtSupRcvPass, _tcslen(auchPrtSupRcvPass));
    ParaTally.usTallyData  = Tally.EptTally.usEPTRespOK;
    PrtPrintItem(NULL, &ParaTally);

    /* RECV FAIL    xxxxx */
    _tcsncpy(ParaTally.auchMnemo, auchPrtSupRcvFail, _tcslen(auchPrtSupRcvFail));
    ParaTally.usTallyData  = Tally.EptTally.usEPTRespErr;
    PrtPrintItem(NULL, &ParaTally);
   return SUCCESS; /* ### Add (2171 for Win32) V1.0 */
}    
/****** End of Source(RptTly.c) ******/


    
    
    
