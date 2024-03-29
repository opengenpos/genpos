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
* Title       : Coupon Read Report Module
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application
* Program Name: UIFAC30.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC30Function()       : A/C No.30 Function Entry Mode
*               UifAC30EnterDataType()  : A/C No.30 Enter Data Type Mode
*               UifAC30EnterRptType()   : A/C No.30 Enter Report Type Mode
*               UifAC30EnterData()      : A/C No.30 Enter Data Mode
* --------------------------------------------------------------------------
* Update Histories
*    Date  : Ver.Rev.   :   Name    : Description
* May-08-95: 03.00.00   : T.Satoh   : initial
* Jul-27-95: 03.00.03   : M.Ozawa   : Modify to report on Scroll Display
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
#include <fsc.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <uifsup.h>
#include <uifpgequ.h>
#include <mldmenu.h>
#include <mldsup.h>

#include "uifsupex.h"                       /* Unique Extern Header for UI */


/*
==============================================================================
;                      C O M M O N   R O M   T A B L E s
;=============================================================================
*/

UIMENU CONST aChildAC30EnterDataType[] = {{UifAC30EnterRptType, CID_AC30ENTERRPTTYPE},
                                          {NULL,                0                   }};

UIMENU CONST aChildAC30Init[] = {{UifAC30EnterDataType, CID_AC30ENTERDATATYPE},
                                 {NULL,                 0                    }};

UIMENU CONST aChildAC30EnterRptType[]  = {{UifAC30EnterData, CID_AC30ENTERDATA},
                                          {NULL,           0                  }};
/* reduce key data, V3.3
UISEQ FARCONST
aszSeqAC30EnterDataType[] = {FSC_NUM_TYPE, FSC_AC, 0};
*/

UISEQ CONST aszSeqAC30EnterData[] = {FSC_AC, FSC_COUPON, FSC_KEYED_COUPON, FSC_CANCEL, 0};

/*
*=============================================================================
**  Synopsis: SHORT UifAC30Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.30 Function Entry
*==============================================================================
*/

SHORT UifAC30Function(KEYMSG *pKeyMsg) 
{
    switch(pKeyMsg->uchMsg){
        case UIM_INIT:
            /*
             * Case : PTD is not Existed
             */
            if(!RptPTDChkFileExist(FLEX_CPN_ADR)){
                /*
                 * Set Data Type to Own Save Area
                 */
                uchUifACDataType = (UCHAR)RPT_TYPE_DALY;
                UieNextMenu(aChildAC30EnterDataType);
            }
            /*
             * Case : PTD is Existed
             */
            else{
                UieNextMenu(aChildAC30Init);
            }
            return(SUCCESS);

        default:
            return(UifACDefProc(pKeyMsg));     /* execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC30EnterDataType(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: IF PTD file is Existed, then get DATA TYPE.
*==============================================================================
*/

SHORT UifAC30EnterDataType(KEYMSG *pKeyMsg) 
{
    switch(pKeyMsg->uchMsg) {
        case UIM_INIT:
            UieOpenSequence(aszSeqAC23EnterDataType);
            /* UieOpenSequence(aszSeqAC30EnterDataType); */

            UifACPGMLDDispCom(DaylyPTDSelect);

            MaintDisp(AC_CPN_READ_RPT,      /* A/C Number                   */
                      0, 0, 0, 0, 0, 0L,
                      LDT_SLCDORP_ADR);     /* "Select Daily or PTD Report" */
            return(SUCCESS);

        case UIM_INPUT:
            switch (pKeyMsg->SEL.INPUT.uchMajor) {
                case FSC_AC:
                    /* allow AC key for PTD, V3.3 */
                    /* return(LDT_SEQERR_ADR); */

                case FSC_NUM_TYPE:
                    /* Without anything keys */
                    if(!pKeyMsg->SEL.INPUT.uchLen){
                        return(LDT_SEQERR_ADR);
                    }

                    /* Check Input Data Range */
                    if(pKeyMsg->SEL.INPUT.lData != RPT_TYPE_DALY &&
                                   pKeyMsg->SEL.INPUT.lData != RPT_TYPE_PTD){
                        return(LDT_KEYOVER_ADR);
                    }

                    /* Set Data Type to Own Save Area */
                    uchUifACDataType = (UCHAR)pKeyMsg->SEL.INPUT.lData;
                    UieNextMenu(aChildAC30EnterDataType);
                    return(SUCCESS);

                /* default: break; */
                }
                /* break; */
                                                                
        default:
            return(UifACDefProc(pKeyMsg));   /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC30EnterRptType(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: IF PTD file is Existed, or DATA TYPE already inputed,
*                 then get REPORT TYPE.
*==============================================================================
*/

SHORT UifAC30EnterRptType(KEYMSG *pKeyMsg) 
{
    switch(pKeyMsg->uchMsg) {
        case UIM_INIT:
            UieOpenSequence(aszSeqAC23EnterDataType);
            /* UieOpenSequence(aszSeqAC30EnterDataType); */

            UifACPGMLDDispCom(TypeSelectReport6);

            MaintDisp(AC_CPN_READ_RPT,       /* A/C Number                  */
                      0, 0,
                      uchUifACDataType,      /* PTD Type                    */
                      0, 0, 0L,
                      LDT_REPOTYPE_ADR);     /* "Request Report Type Entry" */
            return(SUCCESS);
                                                                            
        case UIM_ACCEPTED:
            UieNextMenu(aChildAC30EnterRptType);
            return(SUCCESS);

        case UIM_INPUT:
            switch (pKeyMsg->SEL.INPUT.uchMajor) {
                case FSC_NUM_TYPE:
                    return(LDT_SEQERR_ADR);

                case FSC_AC:
                    /* Without anything keys */
                    if(!pKeyMsg->SEL.INPUT.uchLen){
                        return(LDT_SEQERR_ADR);
                    }

                    switch(pKeyMsg->SEL.INPUT.lData){
                         /************************/
                         /* All Read Report Case */
                         /************************/
                         case RPT_TYPE_ALL:
                            /* Clear Lead Through */
                            
                             uchUifACRptType = ( UCHAR)RPT_TYPE_ALL;     /* Set Report Type to Own Save Area */

                             UifACPGMLDClear();

                             MaintDisp(AC_CPN_READ_RPT,     /* A/C Number   */
                                       0, 0,
                                       uchUifACDataType,    /* PTD Type     */
                                       (UCHAR)RPT_TYPE_ALL, /* Report Type  */
                                       0, 0L, 0);

                             {
                                UCHAR uchMinorClass = CLASS_TTLCURDAY;
                                 /* Daily Report */
                                 if(uchUifACDataType == RPT_TYPE_DALY){
                                     uchMinorClass = CLASS_TTLCURDAY;
                                 }
                                 else {
                                    /* PTD Report */
                                    uchMinorClass = CLASS_TTLCURPTD;
                                 }
                                 // save the currently selected display/print option, generate an html version
                                 // of the report and then restore the user selected display/print option to
                                 // generate the report to the user selected device.
                                 UCHAR   uchUifACRptOnOffMldSave = uchUifACRptOnOffMld;
                                 FILE* fpFile = ItemOpenHistorialReportsFolder(RPTREGFIN_FOLDER_ANY, AC_CPN_READ_RPT, uchMinorClass, RPT_ALL_READ, 0, 0, 0);

                                 if (fpFile) {
                                     ItemGenerateAc30Report(CLASS_TTLCPN, uchMinorClass, RPT_ALL_READ, fpFile);
                                     ItemCloseHistorialReportsFolder(fpFile);
                                 }
                                 uchUifACRptOnOffMld = uchUifACRptOnOffMldSave;
                                 RptCpnRead(uchMinorClass, RPT_ALL_READ, 0);
                             }

                             UieExit(aACEnter);   /* Return to UifACEnter() */
                             return(SUCCESS);

                        /*******************************/
                        /* Individual Read Report Case */
                        /*******************************/
                        case RPT_TYPE_IND:
                            uchUifACRptType = (UCHAR)RPT_TYPE_IND;

                            UifACPGMLDClear();

                            UieNextMenu(aChildAC30EnterRptType);
                            return(SUCCESS);

                        default:
                            return(LDT_KEYOVER_ADR);
                    }
                /* default: break; */
            }
            /* break; */

        default:
            return(UifACDefProc(pKeyMsg));     /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC30EnterData(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: IF REPORT TYPE is INDIVIDUAL PRINT, then get Coupon Number.
*==============================================================================
*/

SHORT UifAC30EnterData(KEYMSG *pKeyMsg) 
{
    SHORT   sError;

    switch(pKeyMsg->uchMsg) {
        case UIM_INIT:
            UieOpenSequence(aszSeqAC30EnterData);
            
            MaintDisp(AC_CPN_READ_RPT,                  /* A/C Number       */
                      CLASS_MAINTDSPCTL_10NPG,          /* Set Minor Class for 10N10D */
                      0,
                      uchUifACDataType,                 /* PTD Type         */
                      uchUifACRptType,                  /* Report Type      */
                      0, 0L,
                      LDT_CPNNO_ADR);                    /* "Enter Coupon #" */
            return(SUCCESS);
                                                                            
        case UIM_INPUT:
            switch (pKeyMsg->SEL.INPUT.uchMajor) {
            case FSC_CANCEL:
                if(uchRptOpeCount & RPT_HEADER_PRINT){          /* Not First Operation */
                    MaintMakeAbortKey();
                }
                break;

            case FSC_AC:
                /* Check Digit */

                if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount */
                    RptCpnRead(0, RPT_FIN_READ, 0);                       /* Execute Finalize Procedure */
                    UieExit(aACEnter);                                          /* Return to UifACEnter() */
                    return(SUCCESS);
                } else {
                    return(LDT_SEQERR_ADR);
                }

            case FSC_COUPON:
                /* Without anything keys or Over 3 digit */
                if((pKeyMsg->SEL.INPUT.uchLen <= 0) ||
                   (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT3)){
                    return(LDT_SEQERR_ADR);
                }
                /* Check 0 Data */
                if(pKeyMsg->SEL.INPUT.lData == 0){
                    return(LDT_KEYOVER_ADR);
                }
                /* Clear Lead Through */
                MaintDisp(AC_CPN_READ_RPT,                  /* A/C Number  */
                          0, 0,
                          uchUifACDataType,                 /* PTD Type    */
                          uchUifACRptType,                  /* Report Type */
                          0, 0L, 0);
                /* Daily Report Case */
                if(uchUifACDataType == RPT_TYPE_DALY){
                    sError = RptCpnRead(CLASS_TTLCURDAY,RPT_IND_READ,
                                           (UCHAR)pKeyMsg->SEL.INPUT.lData);
                }
                /* PTD Report Case */
                else{
                    sError = RptCpnRead(CLASS_TTLCURPTD,RPT_IND_READ,
                                           (UCHAR)pKeyMsg->SEL.INPUT.lData);
                }
                UieAccept();
                /* Aborted by User */
                if(sError == RPT_ABORTED){
                    sError = SUCCESS;
                }
                return(sError);

            case FSC_KEYED_COUPON:

                if(pKeyMsg->SEL.INPUT.uchLen){
                    return(LDT_SEQERR_ADR);
                }

                /* Clear Lead Through */
                
                MaintDisp(AC_CPN_READ_RPT,                  /* A/C Number  */
                          CLASS_MAINTDSPCTL_10NPG,          /* Set Minor Class for 10N10D */
                          0,
                          uchUifACDataType,                 /* PTD Type    */
                          uchUifACRptType,                  /* Report Type */
                          0, 0L, 0);

                /* Daily Report Case */
                if(uchUifACDataType == RPT_TYPE_DALY){
                    sError = RptCpnRead(CLASS_TTLCURDAY,RPT_IND_READ,
                                           pKeyMsg->SEL.INPUT.uchMinor);
                }
                /* PTD Report Case */
                else{
                    sError = RptCpnRead(CLASS_TTLCURPTD,RPT_IND_READ,
                                           pKeyMsg->SEL.INPUT.uchMinor);
                }

                UieAccept();

                /* Aborted by User */
                if(sError == RPT_ABORTED){
                    sError = SUCCESS;
                }
                return(sError);

            /* default: break; */
        }
        /* break; */

        default:                                                            
            return(UifACDefProc(pKeyMsg));     /* Execute Default Procedure */
    }
}

/**** End of File ****/
