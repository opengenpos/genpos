/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1996            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Programable Reset Report Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC136.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs1 /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC136Function()               : A/C No.136 Function Entry Mode
*               UifAC136EnterForamtType()        : A/C No.136 Enter Format Type Mode
*               UifAC136EnterDailyPTD()          : A/C No.136 Enter PTD Mode
*               UifAC136EnterRstType()           : A/C No.136 Enter Reset Type Mode
*               UifAC136EnterCastype()           : A/C No.136 Enter Cashier Mode
*               UifAC136EnterWaiType()           : A/C No.136 Enter Waiter Type Mode
*               UifAC136EnterIndFinType()        : A/C No.136 Enter Terminal No Mode
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Feb-23-96:03.01.00    :M.Ozawa    : Initial
* Aug-11-99:03.05.00    :K.Iwata    : R3.5 (remove WAITER_MODEL)
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
#include <pif.h>
#include <uie.h>
#include <fsc.h>
#include <plu.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>
#include <maint.h>
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <uifsup.h>
#include <uifpgequ.h>
#include <progrept.h>
#include <csprgrpt.h>
#include <mldmenu.h>
#include <mldsup.h>

#include "uifsupex.h"                       /* Unique Extern Header for UI */


/*
==============================================================================
;                      C O M M O N   R O M   T A B L E s                         
;=============================================================================
*/

extern UCHAR   uchUifProgRptNo;
extern USHORT  usUifProgRptInfo;
extern UCHAR   uchUifMajorCode;
extern UCHAR   uchUifDeptNo;
extern USHORT  usUifLoopKeyType;

/*
*=============================================================================
**  Synopsis: SHORT UifAC136Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.136 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC136Init[] = {{UifAC136EnterFormatType, CID_AC136ENTERFORMATTYPE},
                                    {NULL,                 0                    }};

UIMENU FARCONST aChildAC136EnterDailyPTD[] = {{UifAC136EnterDailyPTD, CID_AC136ENTERDAILYPTD},
                                             {NULL,                0                   }};

UIMENU FARCONST aChildAC136EnterRstType[] = {{UifAC136EnterRstType, CID_AC136ENTERRSTTYPE},
                                             {NULL,                0                   }};

/* Define Next Function */

UIMENU FARCONST aChildAC136EnterCasType[] = {{UifAC136EnterCasType, CID_AC136ENTERCASTYPE},
                                             {NULL,               0                  }};
UIMENU FARCONST aChildAC136EnterIndType[] = {{UifAC136EnterIndFinType, CID_AC136ENTERINDFINTYPE},
                                             {NULL,               0                  }};
    
SHORT UifAC136Function(KEYMSG *pKeyMsg) 
{

    USHORT  usType;    
    SHORT   sError = SUCCESS;

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        usUifLoopKeyType = 0;
        UieNextMenu(aChildAC136Init);
        return(SUCCESS);

    case UIM_ACCEPTED:
        switch (pKeyMsg->SEL.usFunc) {
        case CID_AC136ENTERFORMATTYPE:
            /* Check PTD Exist or Not */
            uchUifACDataType = ( UCHAR)RPT_TYPE_DALY;           /* Set Data Type to Own Save Area */

            switch(usUifProgRptInfo) {
            case PROGRPT_FILE_PLU:
                if (RptPTDChkFileExist(FLEX_PLU_ADR)) {               /* Not PTD Case */
                    UieNextMenu(aChildAC136EnterDailyPTD);
                    return(SUCCESS);
                }
                break;

            case PROGRPT_FILE_CPN:
                if (RptPTDChkFileExist(FLEX_CPN_ADR)) {               /* Not PTD Case */
                    UieNextMenu(aChildAC136EnterDailyPTD);
                    return(SUCCESS);
                }
                break;

            case PROGRPT_FILE_CAS:
                if (RptPTDChkFileExist(FLEX_CAS_ADR)) {                /* Not PTD Case */
                    UieNextMenu(aChildAC136EnterDailyPTD);
                    return(SUCCESS);
                }
                break;

            default:
                uchUifACDataType = 0;
                break;
            }

        case CID_AC136ENTERDAILYPTD:
            UieNextMenu(aChildAC136EnterRstType);
            return(SUCCESS);
            break;

        default:
            /* Clear Lead Through */
                                                                    
            UifACPGMLDClear();

            MaintDisp(AC_PROG_RESET_RPT,            /* A/C Number */
                      0,                            /* Descriptor */
                      0,                            /* Page Number */
                      uchUifACDataType,             /* PTD Type */
                      uchUifACRptType,              /* Report Type */
                      uchUifACRstType,              /* Reset Type */
                      0L,                           /* Amount Data */
                      0);                           /* Disable Lead Through */

            switch(usUifProgRptInfo) {
            case PROGRPT_FILE_PLU:
                /* execute all report */
                if (uchUifACRstType != RPT_TYPE_ONLY_PRT) {
                    if ((sError = RptPLULock()) != SUCCESS) {
                        UieExit(aACEnter);                      
                        break;
                    }
                }
                if (uchUifACDataType == RPT_TYPE_DALY) {
                    sError = ProgRptPluReset(uchUifProgRptNo, 
                                        RPT_TYPE_DALY,
                                        uchUifACRstType);
                } else {
                    sError = ProgRptPluReset(uchUifProgRptNo, 
                                        RPT_TYPE_PTD,
                                        uchUifACRstType);
                }
                if (uchUifACRstType != RPT_TYPE_ONLY_PRT) {
                    RptPLUUnLock();
                }
                UieExit(aACEnter);                      
                break;

            case PROGRPT_FILE_CPN:
                /* execute all report */
                if (uchUifACRstType != RPT_TYPE_ONLY_PRT) {
                    if ((sError = RptCpnLock()) != SUCCESS) {
                        UieExit(aACEnter);                      
                        break;
                    }
                }
                /* Daily Report */
                if(uchUifACDataType == RPT_TYPE_DALY){
                    sError = ProgRptCpnReset(uchUifProgRptNo, 
                                RPT_TYPE_DALY,
                                uchUifACRstType);
                }
                /* PTD Report */
                else{
                    sError = ProgRptCpnReset(uchUifProgRptNo, 
                                RPT_TYPE_PTD,
                                uchUifACRstType);
                }
                if (uchUifACRstType != RPT_TYPE_ONLY_PRT) {
                    RptCpnUnLock();
                }
                UieExit(aACEnter);
                break;

            case PROGRPT_FILE_CAS:
                if (ProgRptGetTtlFileLoopKey(uchUifACRptType, &usType) == PROGRPT_SUCCESS) {

                    if (!usType) {  /* report type is not directed */

                        if (uchUifACRstType != RPT_TYPE_ONLY_PRT) {
                            if ((sError = RptCashierLock(RPT_ALL_LOCK, 0)) != SUCCESS) {
                                UieExit(aACEnter);                      
                                break;
                            }
                        }
                        if (uchUifACDataType == RPT_TYPE_DALY) {
                            sError = ProgRptCasReset(uchUifProgRptNo, RPT_TYPE_DALY, uchUifACRstType, 0);
                        } else {
                            sError = ProgRptCasReset(uchUifProgRptNo, RPT_TYPE_PTD, uchUifACRstType, 0);
                        }
                        if (uchUifACRstType != RPT_TYPE_ONLY_PRT) {
                            RptCashierUnLock(RPT_ALL_UNLOCK, 0);
                        }
                        UieExit(aACEnter);
                    } else {
                        usUifLoopKeyType = usType;
                        UieNextMenu(aChildAC136EnterCasType);                            /* Open Next Function */
                    }
                }
                break;

            case PROGRPT_FILE_INDFIN:
                if (ProgRptGetTtlFileLoopKey(uchUifACRptType, &usType) == PROGRPT_SUCCESS) {

                    if (!usType) {  /* report type is not directed */

                        if (uchUifACRstType != RPT_TYPE_ONLY_PRT) {
                            if ((sError = RptIndFinLock(RPT_ALL_LOCK, 0)) != SUCCESS) {
                                UieExit(aACEnter);                      
                                break;
                            }
                        }
                        sError = ProgRptIndFinReset(uchUifProgRptNo, 
                                                   uchUifACRstType,
                                                   0);
                        if (uchUifACRstType != RPT_TYPE_ONLY_PRT) {
                            RptIndFinUnLock();
                        }
                        UieExit(aACEnter);

                    } else {
                        usUifLoopKeyType = usType;
                        UieNextMenu(aChildAC136EnterIndType);                            /* Open Next Function */
                    }
                }
                break;
            }
        }
        if (sError == RPT_ABORTED) {                        /* Aborted by User */
            sError = SUCCESS;
        }
        if (sError != SUCCESS) {
            UieErrorMsg(sError, UIE_WITHOUT);
        }
        return(SUCCESS);

    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC136EnterFormatType(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.136 Enter Format Type Mode 
*===============================================================================
*/


SHORT UifAC136EnterFormatType(KEYMSG *pKeyMsg) 
{


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */

        MaintDisp(AC_PROG_RESET_RPT,                /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  uchUifACRptType,                  /* Report Type */
                  uchUifACRstType,                  /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_REPOCD_ADR);                  /* "Request Report Code # Entry" Lead Through Address */

        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Check W/ Amount */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount */
                return(LDT_SEQERR_ADR);
            }

            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {               /* Over Digit */
                return(LDT_KEYOVER_ADR);                                
            }

            /* Set Data Type to Own Save Area */
            
            uchUifProgRptNo = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            uchUifACRptType = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            
            if (ProgRptGetTtlFileType(uchUifProgRptNo, &usUifProgRptInfo) != PROGRPT_SUCCESS) {
                return(LDT_PROHBT_ADR);
            }

            /* check flexible file */
            switch(usUifProgRptInfo) {
            case PROGRPT_FILE_DEPT:
            case PROGRPT_FILE_FIN:
                return(LDT_PROHBT_ADR);

            case PROGRPT_FILE_PLU:
                if ((RptEODChkFileExist(FLEX_PLU_ADR) == RPT_FILE_NOTEXIST) ||
                    (RptEODChkFileExist(FLEX_DEPT_ADR) == RPT_FILE_NOTEXIST)) { /* File not Exist */
                    return(LDT_PROHBT_ADR);
                }
                break;

            case PROGRPT_FILE_CPN:
                if (RptEODChkFileExist(FLEX_CPN_ADR) == RPT_FILE_NOTEXIST){
                return(LDT_PROHBT_ADR);
                }
                break;

            case PROGRPT_FILE_CAS:
                if (RptEODChkFileExist(FLEX_CAS_ADR) == RPT_FILE_NOTEXIST) { /* File not Exist */
                    return(LDT_PROHBT_ADR);
                }
                break;

            case PROGRPT_FILE_WAI:
                return(LDT_PROHBT_ADR);
                break;
            }
            UieAccept();

            return(SUCCESS);
        
        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC136EnterDailyPTD(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.136 Enter PTD Mode 
*===============================================================================
*/


SHORT UifAC136EnterDailyPTD(KEYMSG *pKeyMsg) 
{

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC23EnterDataType);   /* Register Key Sequence */

        UifACPGMLDDispCom(DaylyPTDSelect);

        MaintDisp(AC_PROG_RESET_RPT,                /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  uchUifACRptType,                  /* Report Type */
                  uchUifACRstType,                  /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_SLCDORP_ADR);                 /* "Select Daily or PTD Report" Lead Through Address */

        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* allow AC key for PTD, V3.3 */
            /* return(LDT_SEQERR_ADR); */

        case FSC_NUM_TYPE:
            /* Check W/ Amount */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount */
                return(LDT_SEQERR_ADR);
            }

            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {               /* Over Digit */
                return(LDT_KEYOVER_ADR);                                
            }

            /* Check Input Data Range */

            if (pKeyMsg->SEL.INPUT.lData != RPT_TYPE_DALY && pKeyMsg->SEL.INPUT.lData != RPT_TYPE_PTD) {
                return(LDT_KEYOVER_ADR);
            }
            
            /* Set Data Type to Own Save Area */
            
            uchUifACDataType = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            
            UieAccept();
            return(SUCCESS);
        
        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC136EnterRstType(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.136 Enter Reset Type Mode 
*===============================================================================
*/


SHORT UifAC136EnterRstType(KEYMSG *pKeyMsg) 
{


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */

        UifACPGMLDDispCom(ResetTypeSelect);

        MaintDisp(AC_PROG_RESET_RPT,                /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  uchUifACDataType,                 /* PTD Type */
                  uchUifACRptType,                  /* Report Type */
                  uchUifACRstType,                  /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_FUNCTYPE_ADR);                /* "Request Function Type Entry" Lead Through Address */

        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Check W/ Amount */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount */
                return(LDT_SEQERR_ADR);
            }

            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {               /* Over Digit */
                return(LDT_KEYOVER_ADR);                                
            }

            if ((pKeyMsg->SEL.INPUT.lData < RPT_TYPE_ONLY_PRT) ||
                (pKeyMsg->SEL.INPUT.lData > RPT_TYPE_ONLY_RST)) {
                return(LDT_KEYOVER_ADR);                                
            }

            /* Set Data Type to Own Save Area */
            
            uchUifACRstType = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            
            UieAccept();

            return(SUCCESS);
        
        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC136EnterCasType(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.136 Enter Cashier Mode 
*===============================================================================
*/

SHORT UifAC136EnterCasType(KEYMSG *pKeyMsg) 
{
    
    SHORT   sError;
    UCHAR   uchType;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */

        MaintDisp(AC_PROG_RESET_RPT,                /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  uchUifACDataType,                 /* PTD Type */
                  uchUifACRptType,                  /* Report Type */
                  uchUifACRstType,                  /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_NUM_ADR);                     /* "Number Entry" Lead Through Address */

        return(SUCCESS);
                                                                            
    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Check 0 Data and Digit */
            if (pKeyMsg->SEL.INPUT.uchLen == 0) {
                pKeyMsg->SEL.INPUT.lData = 0L;
            } else if (!pKeyMsg->SEL.INPUT.lData || (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT3)) { 
                return(LDT_KEYOVER_ADR);
            }

            /* Clear Lead Through */
            MaintDisp(AC_PROG_RESET_RPT,            /* A/C Number */
                      0,                            /* Descriptor */
                      0,                            /* Page Number */
                      uchUifACDataType,             /* PTD Type */
                      uchUifACRptType,              /* Report Type */
                      uchUifACRstType,              /* Reset Type */
                      0L,                           /* Amount Data */
                      0);                           /* Disable Lead Through */

            uchType = (UCHAR)(pKeyMsg->SEL.INPUT.lData ? RPT_IND_LOCK : RPT_ALL_LOCK);

            if (uchUifACRstType != RPT_TYPE_ONLY_PRT) {
                if ((sError = RptCashierLock(uchType, pKeyMsg->SEL.INPUT.lData)) != SUCCESS) {
                    UieExit(aACEnter);                      
                    break;
                }
            }

            /* Execute Individual Read */
            if (uchUifACDataType == RPT_TYPE_DALY) {
                sError = ProgRptCasReset(uchUifProgRptNo, RPT_TYPE_DALY, uchUifACRstType, pKeyMsg->SEL.INPUT.lData);
            } else {
                sError = ProgRptCasReset(uchUifProgRptNo, RPT_TYPE_PTD, uchUifACRstType, pKeyMsg->SEL.INPUT.lData);
            }
            if (uchUifACRstType != RPT_TYPE_ONLY_PRT) {
                RptCashierUnLock(uchType, pKeyMsg->SEL.INPUT.lData);
            }

            UieExit(aACEnter);                                                       
            if (sError == RPT_ABORTED) {                        /* Aborted by User */
                sError = SUCCESS;
            }
            return(sError);

        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                                  /* Execute Default Procedure */
    }                                                                   
}
/*
*=============================================================================
**  Synopsis: SHORT UifAC136EnterIndFintype(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.136 Enter Individual financial Mode 
*===============================================================================
*/

SHORT UifAC136EnterIndFinType(KEYMSG *pKeyMsg) 
{
    SHORT   sError;
    UCHAR   uchType;


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */

        MaintDisp(AC_PROG_RESET_RPT,                /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  uchUifACRptType,                  /* Report Type */
                  uchUifACRstType,                  /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_NUM_ADR);                     /* "Number Entry" Lead Through Address */

        return(SUCCESS);
                                                                            
    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Check 0 Data and Digit */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount */
                pKeyMsg->SEL.INPUT.lData = 0L;
            } else if (!pKeyMsg->SEL.INPUT.lData || (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT3)) {      
                return(LDT_KEYOVER_ADR);
            }

            /* Clear Lead Through */
            MaintDisp(AC_PROG_RESET_RPT,            /* A/C Number */
                      0,                            /* Descriptor */
                      0,                            /* Page Number */
                      0,                            /* PTD Type */
                      uchUifACRptType,              /* Report Type */
                      uchUifACRstType,              /* Reset Type */
                      0L,                           /* Amount Data */
                      0);                           /* Disable Lead Through */

            uchType = (UCHAR)(pKeyMsg->SEL.INPUT.lData ? RPT_IND_LOCK : RPT_ALL_LOCK);

            if (uchUifACRstType != RPT_TYPE_ONLY_PRT) {
                if ((sError = RptIndFinLock(uchType, (USHORT)pKeyMsg->SEL.INPUT.lData)) != SUCCESS) {  // USHORT terminal number in pKeyMsg->SEL.INPUT.lData
                    UieExit(aACEnter);                      
                    return(sError);
                    break;
                }
            }

            /* Execute Individual Read */
            sError = ProgRptIndFinReset(uchUifProgRptNo, uchUifACRstType, (USHORT)pKeyMsg->SEL.INPUT.lData);  // USHORT terminal number in pKeyMsg->SEL.INPUT.lData

            if (uchUifACRstType != RPT_TYPE_ONLY_PRT) {
                RptIndFinUnLock();
            }

            UieExit(aACEnter);                                                       
            if (sError == RPT_ABORTED) {                        /* Aborted by User */
                sError = SUCCESS;
            }
            return(sError);

        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                                  /* Execute Default Procedure */
    }                                                                   
}

/**** End of File ****/
