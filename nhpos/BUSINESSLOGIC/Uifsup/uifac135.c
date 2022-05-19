/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Programable Read Report Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC135.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs1 /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC135Function()               : A/C No.135 Function Entry Mode
*               UifAC135EnterForamtType()        : A/C No.135 Enter Format Type Mode
*               UifAC135EnterDailyPTD()          : A/C No.135 Enter PTD Mode
*               UifAC135EnterDeptType()          : A/C No.135 Enter Dept Type Mode
*               UifAC135EnterMDEPT1()            : A/C No.135 Enter Major DEPT 1 Mode
*               UifAC135EnterDEPT1()             : A/C No.135 Enter DEPT 1 Mode
*               UifAC135EnterPluType()           : A/C No.135 Enter PLU Report Type Mode
*               UifAC135EnterDEPTPLU1()          : A/C No.135 Enter DEPT 1 Mode
*               UifAC135EnterPLUCode1()          : A/C No.135 Enter Report Code 1 Mode
*               UifAC135EnterPLU1()              : A/C No.135 Enter PLU 1 Mode
*               UifAC135EnterCpnType()           : A/C No.135 Enter Cpn Type Mode
*               UifAC135EnterCpn()               : A/C No.135 Enter Cpn Mode
*               UifAC135EnterCastype()           : A/C No.135 Enter Cashier Mode
*               UifAC135EnterWaiType()           : A/C No.135 Enter Waiter Type Mode
*               UifAC135EnterWaiNo()             : A/C No.135 Enter Waiter Mode
*               UifAC135EnterIndFinType()        : A/C No.135 Enter Terminal No Mode
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jan-10-96:03.01.00    :M.Ozawa    : Initial
* Aug-11-99:03.05.00    :K.Iwata    : R3.5 (remove WAITER_MODEL)
* Dec-10-99:01.00.00    :hrkato     : Saratoga
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

UCHAR   uchUifProgRptNo;
USHORT  usUifProgRptInfo;
UCHAR   uchUifMajorCode;
USHORT  usUifDeptNo;            /* Saratoga */
USHORT  usUifLoopKeyType;

/*
*=============================================================================
**  Synopsis: SHORT UifAC135Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.135 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC135Init[] = {{UifAC135EnterFormatType, CID_AC135ENTERFORMATTYPE},
                                    {NULL,                 0                    }};

UIMENU FARCONST aChildAC135EnterDailyPTD[] = {{UifAC135EnterDailyPTD, CID_AC135ENTERDAILYPTD},
                                             {NULL,                0                   }};

/* Define Next Function */

UIMENU FARCONST aChildAC135EnterDeptType[] = {{UifAC135EnterDeptType, CID_AC135ENTERDEPTTYPE},
                                             {NULL,               0                  }};
    
UIMENU FARCONST aChildAC135EnterPluType[] = {{UifAC135EnterPLUType, CID_AC135ENTERPLUTYPE},
                                             {NULL,               0                  }};
    
UIMENU FARCONST aChildAC135EnterCpnType[] = {{UifAC135EnterCpnType, CID_AC135ENTERCPNTYPE},
                                             {NULL,               0                  }};
    
UIMENU FARCONST aChildAC135EnterCasType[] = {{UifAC135EnterCasType, CID_AC135ENTERCASTYPE},
                                             {NULL,               0                  }};


UIMENU FARCONST aChildAC135EnterIndType[] = {{UifAC135EnterIndFinType, CID_AC135ENTERINDFINTYPE},
                                             {NULL,               0                  }};
    
SHORT UifAC135Function(KEYMSG *pKeyMsg) 
{
    TCHAR   auchPLUNumber[PLU_MAX_DIGIT+1];
    USHORT  usType;    
    SHORT   sError = SUCCESS;

    memset(auchPLUNumber, 0, sizeof(auchPLUNumber));

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        usUifLoopKeyType = 0;
        UieNextMenu(aChildAC135Init);
        return(SUCCESS);

    case UIM_ACCEPTED:
        switch (pKeyMsg->SEL.usFunc) {
        case CID_AC135ENTERFORMATTYPE:
            /* Check PTD Exist or Not */
            uchUifACDataType = ( UCHAR)RPT_TYPE_DALY;           /* Set Data Type to Own Save Area */

            switch(usUifProgRptInfo) {
            case PROGRPT_FILE_DEPT:
                if (RptPTDChkFileExist(FLEX_DEPT_ADR)) {               /* Not PTD Case */
                    UieNextMenu(aChildAC135EnterDailyPTD);
                    return(SUCCESS);
                }
                break;

            case PROGRPT_FILE_PLU:
                if (RptPTDChkFileExist(FLEX_PLU_ADR)) {               /* Not PTD Case */
                    UieNextMenu(aChildAC135EnterDailyPTD);
                    return(SUCCESS);
                }
                break;

            case PROGRPT_FILE_CPN:
                if (RptPTDChkFileExist(FLEX_CPN_ADR)) {               /* Not PTD Case */
                    UieNextMenu(aChildAC135EnterDailyPTD);
                    return(SUCCESS);
                }
                break;

            case PROGRPT_FILE_CAS:  /* V3.3 */
                if (RptPTDChkFileExist(FLEX_CAS_ADR)) {                /* Not PTD Case */
                    UieNextMenu(aChildAC135EnterDailyPTD);
                    return(SUCCESS);
                }
                break;

            case PROGRPT_FILE_FIN:
                UieNextMenu(aChildAC135EnterDailyPTD);
                return(SUCCESS);
                break;

            default:
                uchUifACDataType = 0;

            }
            /* break */
        default:
            /* Clear Lead Through */
                                                                    
            UifACPGMLDClear();

            MaintDisp(AC_PROG_READ_RPT,             /* A/C Number */
                      0,                            /* Descriptor */
                      0,                            /* Page Number */
                      uchUifACDataType,             /* PTD Type */
                      uchUifACRptType,              /* Report Type */
                      0,                            /* Reset Type */
                      0L,                           /* Amount Data */
                      0);                           /* Disable Lead Through */

            switch(usUifProgRptInfo) {
            case PROGRPT_FILE_DEPT:
                if (ProgRptGetTtlFileLoopKey(uchUifACRptType, &usType) == PROGRPT_SUCCESS) {
                    if (!usType) {  /* report type is not directed */
                        /* execute all report */
                        if (uchUifProgRptNo) {
                            if (uchUifACDataType == RPT_TYPE_DALY) {
                                sError = ProgRptDeptRead(uchUifProgRptNo, RPT_TYPE_DALY, 0, 0);
                            } else {
                                sError = ProgRptDeptRead(uchUifProgRptNo, RPT_TYPE_PTD, 0, 0);
                            }
                        } else {    /* starndard report */
                            if (uchUifACDataType == RPT_TYPE_DALY) {
                                sError = RptDEPTRead(CLASS_TTLCURDAY, RPT_ALL_READ, 0);
                            } else {
                                sError = RptDEPTRead(CLASS_TTLCURPTD,  RPT_ALL_READ, 0);
                            }
                        }

                        UieExit(aACEnter);
                    } else {
                        usUifLoopKeyType = usType;
                        UieNextMenu(aChildAC135EnterDeptType);
                    }
                }
                break;

            case PROGRPT_FILE_PLU:
                if (ProgRptGetTtlFileLoopKey(uchUifACRptType, &usType) == PROGRPT_SUCCESS) {
                    if (!usType) {  /* report type is not directed */
                        /* execute all report */
                        if (uchUifProgRptNo) {
                            if (uchUifACDataType == RPT_TYPE_DALY) {
                                sError = ProgRptPluRead(uchUifProgRptNo, RPT_TYPE_DALY, 0, 0, 0);
                            } else {
                                sError = ProgRptPluRead(uchUifProgRptNo, RPT_TYPE_PTD, 0, 0, 0);
                            }
                        } else {    /* standard report */
                            if (uchUifACDataType == RPT_TYPE_DALY) {
                                sError = RptPLUDayRead(CLASS_TTLCURDAY, RPT_ALL_READ, 0, 0, auchPLUNumber);
                            } else {
                                sError = RptPLUPTDRead(CLASS_TTLCURPTD, RPT_ALL_READ, 0, 0, auchPLUNumber);
                            }
                        }
                        UieExit(aACEnter);                      
                    } else {
                        usUifLoopKeyType = usType;
                        UieNextMenu(aChildAC135EnterPluType);
                    }
                }
                break;

            case PROGRPT_FILE_CPN:
                if (ProgRptGetTtlFileLoopKey(uchUifACRptType, &usType) == PROGRPT_SUCCESS) {
                    if (!usType) {  /* report type is not directed */
                        /* execute all report */
                        if (uchUifProgRptNo) {
                            /* Daily Report */
                            if(uchUifACDataType == RPT_TYPE_DALY){
                                sError = ProgRptCpnRead(uchUifProgRptNo, RPT_TYPE_DALY, 0);
                            }
                            /* PTD Report */
                            else{
                                sError = ProgRptCpnRead(uchUifProgRptNo, RPT_TYPE_PTD, 0);
                            }
                        } else {    /* standard report */
                            /* Daily Report */
                            if(uchUifACDataType == RPT_TYPE_DALY){
                                sError = RptCpnRead(CLASS_TTLCURDAY,RPT_ALL_READ,0);
                            }
                            /* PTD Report */
                            else{
                                sError = RptCpnRead(CLASS_TTLCURPTD,RPT_ALL_READ,0);
                            }
                        }

                        UieExit(aACEnter);
                    } else {
                        usUifLoopKeyType = usType;
                        UieNextMenu(aChildAC135EnterCpnType);                    /* Open Next Function */
                    }
                }
                break;

            case PROGRPT_FILE_CAS:
                if (ProgRptGetTtlFileLoopKey(uchUifACRptType, &usType) == PROGRPT_SUCCESS) {
                    if (!usType) {  /* report type is not directed */
                        if (uchUifProgRptNo) {
                            if (uchUifACDataType == RPT_TYPE_DALY) {    /* V3.3 */
                                sError = ProgRptCasRead(uchUifProgRptNo, RPT_TYPE_DALY, 0);
                            } else {
                                sError = ProgRptCasRead(uchUifProgRptNo, RPT_TYPE_PTD, 0);
                            }
                        } else {    /* standard format */
                            if (uchUifACDataType == RPT_TYPE_DALY) {    /* V3.3 */
                                sError = RptCashierRead(CLASS_TTLCURDAY, RPT_ALL_READ, 0, 0);                         /* Execute All Read Report */
                            } else {
                                sError = RptCashierRead(CLASS_TTLCURPTD, RPT_ALL_READ, 0, 0);                         /* Execute All Read Report */
                            }
                        }
                        UieExit(aACEnter);
                    } else {
                        usUifLoopKeyType = usType;
                        UieNextMenu(aChildAC135EnterCasType);                            /* Open Next Function */
                    }
                }
                break;

            case PROGRPT_FILE_INDFIN:
                if (ProgRptGetTtlFileLoopKey(uchUifACRptType, &usType) == PROGRPT_SUCCESS) {
                    if (!usType) {  /* report type is not directed */
                        if (uchUifProgRptNo) {
                            sError = ProgRptIndFinRead(uchUifProgRptNo, 0);
                        } else {    /* starndard report */
                            sError = RptIndFinDayRead(CLASS_TTLCURDAY, RPT_ALL_READ, 0, 0);                         /* Execute All Read Report */
                        }
                        UieExit(aACEnter);

                    } else {
                        usUifLoopKeyType = usType;
                        UieNextMenu(aChildAC135EnterIndType);                            /* Open Next Function */
                    }
                }
                break;

            case PROGRPT_FILE_FIN:
                if (uchUifProgRptNo) {
                    if (uchUifACDataType == RPT_TYPE_DALY) {
                        sError = ProgRptFinRead(uchUifProgRptNo, RPT_TYPE_DALY);
                    } else {
                        sError = ProgRptFinRead(uchUifProgRptNo, RPT_TYPE_PTD);
                    }
                } else {    /* standard format */
                    if (uchUifACDataType == RPT_TYPE_DALY) {
                        sError = RptRegFinDayRead(CLASS_TTLCURDAY, RPT_ALL_READ);
                    } else {
                        sError = RptRegFinPTDRead(CLASS_TTLCURPTD,  RPT_ALL_READ);
                    }
                }
                UieExit(aACEnter);
                break;

            }
        }
        if (sError == RPT_ABORTED) {                        /* Aborted by User */
            sError = SUCCESS;
        }
        return(sError);

    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC135EnterFormatType(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.135 Enter Format Type Mode 
*===============================================================================
*/


SHORT UifAC135EnterFormatType(KEYMSG *pKeyMsg) 
{


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */

        MaintDisp(AC_PROG_READ_RPT,                 /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  uchUifACRptType,                  /* Report Type */
                  0,                                /* Reset Type */
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

            if (uchUifACRptOnOffMld == RPT_DISPLAY_ON) {
                if (ProgRptGetMLDReportNo(uchUifProgRptNo, &uchUifProgRptNo) != SUCCESS) {
                    uchUifProgRptNo = 0;    /* use starndard format, if lcd format is not prepared */
                }
            }

            /* check flexible file */
            switch(usUifProgRptInfo) {
            case PROGRPT_FILE_DEPT:
                if (RptEODChkFileExist(FLEX_DEPT_ADR) == RPT_FILE_NOTEXIST) { /* File not Exist */
                    return(LDT_PROHBT_ADR);
                }
                break;

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
**  Synopsis: SHORT UifAC135EnterDailyPTD(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.135 Enter PTD Mode 
*===============================================================================
*/


SHORT UifAC135EnterDailyPTD(KEYMSG *pKeyMsg) 
{

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC23EnterDataType);   /* Register Key Sequence */

        UifACPGMLDDispCom(DaylyPTDSelect);

        MaintDisp(AC_PROG_READ_RPT,                 /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  uchUifACRptType,                  /* Report Type */
                  0,                                /* Reset Type */
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
**  Synopsis: SHORT UifAC135EnterDeptType(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.135 Enter Report Type Mode 
*===============================================================================
*/

    
/* Define Next Function */

UIMENU FARCONST aChildAC135EnterDeptType2[] = {{UifAC135EnterMDEPT1, CID_AC135ENTERMDEPT1},
                                             {NULL,               0                  }};


UIMENU FARCONST aChildAC135EnterDeptType3[] = {{UifAC135EnterDEPT1, CID_AC135ENTERDEPT1},
                                             {NULL,              0                 }};

SHORT UifAC135EnterDeptType(KEYMSG *pKeyMsg) 
{

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        uchUifMajorCode = 0;

        if (usUifLoopKeyType & PROGRPT_KEYFLAG_MDEPTNO) {
            UieNextMenu(aChildAC135EnterDeptType2);               /* Open Next Function */
        } else {
            UieNextMenu(aChildAC135EnterDeptType3);               /* Open Next Function */
        }

        return(SUCCESS);
                                                                            
    case UIM_ACCEPTED:
        switch (pKeyMsg->SEL.usFunc) {
        case CID_AC135ENTERMDEPT1:
            UieNextMenu(aChildAC135EnterDeptType2);               /* Open Next Function */
            return(SUCCESS);

        default:                                                /* CID_AC26ENTERDEPT1 case */
            UieNextMenu(aChildAC135EnterDeptType3);               /* Open Next Function */
            return(SUCCESS);
        }

    default:
        return(UifACDefProc(pKeyMsg));                                  /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC135EnterMDEPT1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.135 Enter Major DEPT 1 Mode 
*===============================================================================
*/

SHORT UifAC135EnterMDEPT1(KEYMSG *pKeyMsg) 
{
    SHORT   sError;                    

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */

        MaintDisp(AC_PROG_READ_RPT,                 /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  uchUifACDataType,                 /* PTD Type */
                  uchUifACRptType,                  /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_MDEPT_ADR);                   /* "Request Major Department No. Entry" Lead Through Address */

        return(SUCCESS);
                                                                            
    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
        
            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {                           
                return(LDT_KEYOVER_ADR);
            }

            /* Clear Lead Through */
            MaintDisp(AC_PROG_READ_RPT,             /* A/C Number */
                      0,                            /* Descriptor */
                      0,                            /* Page Number */
                      0,                            /* PTD Type */
                      uchUifACRptType,              /* Report Type */
                      0,                            /* Reset Type */
                      0L,                           /* Amount Data */
                      0);                           /* Disable Lead Through */

            if ((!pKeyMsg->SEL.INPUT.uchLen) && !(usUifLoopKeyType & PROGRPT_KEYFLAG_DEPTNO)) {
                if (uchUifProgRptNo) {
                    if (uchUifACDataType == RPT_TYPE_DALY) {
                        sError = ProgRptDeptRead(uchUifProgRptNo, RPT_TYPE_DALY, 0, 0);
                    } else {
                        sError = ProgRptDeptRead(uchUifProgRptNo, RPT_TYPE_PTD, 0, 0);
                    }
                } else {    /* starndard report */
                    if (uchUifACDataType == RPT_TYPE_DALY) {
                        sError = RptDEPTRead(CLASS_TTLCURDAY, RPT_ALL_READ, 0);
                    } else {
                        sError = RptDEPTRead(CLASS_TTLCURPTD, RPT_ALL_READ, 0);
                    }
                }
                UieExit(aACEnter);
                if (sError == RPT_ABORTED) {
                    sError = SUCCESS;
                }
                return(sError);
            } else if ((!pKeyMsg->SEL.INPUT.uchLen) &&  (usUifLoopKeyType & PROGRPT_KEYFLAG_DEPTNO)) {
                ;
            } else if (pKeyMsg->SEL.INPUT.lData == 0L) {
                return(LDT_KEYOVER_ADR);
            }

            if (usUifLoopKeyType & PROGRPT_KEYFLAG_DEPTNO) {
                uchUifMajorCode = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
                UieNextMenu(aChildAC135EnterDeptType3);               /* Open Next Function */
                return(SUCCESS);
            }

            if (uchUifProgRptNo) {
                if (uchUifACDataType == RPT_TYPE_DALY) {
                    sError = ProgRptDeptRead(uchUifProgRptNo, RPT_TYPE_DALY, 0, ( UCHAR)pKeyMsg->SEL.INPUT.lData);
                } else {
                    sError = ProgRptDeptRead(uchUifProgRptNo, RPT_TYPE_PTD, 0, ( UCHAR)pKeyMsg->SEL.INPUT.lData);
                }
            } else {    /* starndard report */
                if (uchUifACDataType == RPT_TYPE_DALY) {
                    sError = RptDEPTRead(CLASS_TTLCURDAY, RPT_MDEPT_READ, ( UCHAR)pKeyMsg->SEL.INPUT.lData);
                } else {
                    sError = RptDEPTRead(CLASS_TTLCURPTD, RPT_MDEPT_READ, ( UCHAR)pKeyMsg->SEL.INPUT.lData);
                }
                RptDEPTRead(0, RPT_FIN_READ, 0);                     /* Execute Finalize Procedure */
            }

            UieExit(aACEnter);
            if (sError == RPT_ABORTED) {
                sError = SUCCESS;
            }
            return(sError);

        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                              /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC135EnterDEPT1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.135 Enter DEPT 1 Mode 
*===============================================================================
*/
SHORT UifAC135EnterDEPT1(KEYMSG *pKeyMsg) 
{
    SHORT   sError;                    
    USHORT      usDEPTNumber;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC26EnterDEPT1);      /* Register Key Sequence */

        UifACPGMLDClear();

        MaintDisp(AC_PROG_READ_RPT,                 /* A/C Number */
                  CLASS_MAINTDSPCTL_10NPG,          /* Set Minor Class for 10N10D */
                  0,                                /* Page Number */
                  uchUifACDataType,                 /* PTD Type */
                  uchUifACRptType,                  /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_DEPTNO_ADR);                  /* "Request Department # Entry" Lead Through Address */

        return(SUCCESS);
                                                                            
    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {                          
                return(LDT_KEYOVER_ADR);
            }
            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount */
                pKeyMsg->SEL.INPUT.lData = 0L;
            } else if (pKeyMsg->SEL.INPUT.lData == 0L) {
                return(LDT_KEYOVER_ADR);
            }

            /* Clear Lead Through */

            MaintDisp(AC_PROG_READ_RPT,                 /* A/C Number */
                      0,                                /* Descriptor */
                      0,                                /* Page Number */
                      uchUifACDataType,                 /* PTD Type */
                      uchUifACRptType,                  /* Report Type */
                      0,                                /* Reset Type */
                      0L,                               /* Amount Data */
                      0);                               /* Disable  Lead Through */

            if (uchUifProgRptNo) {
                if (uchUifACDataType == RPT_TYPE_DALY) {
                    sError = ProgRptDeptRead(uchUifProgRptNo, RPT_TYPE_DALY, ( UCHAR)pKeyMsg->SEL.INPUT.lData, uchUifMajorCode);
                } else {
                    sError = ProgRptDeptRead(uchUifProgRptNo, RPT_TYPE_PTD, ( UCHAR)pKeyMsg->SEL.INPUT.lData, uchUifMajorCode);
                }
            } else {    /* standard fomrat */
                if (pKeyMsg->SEL.INPUT.lData) {
                    if (uchUifACDataType == RPT_TYPE_DALY) {
                        sError = RptDEPTRead(CLASS_TTLCURDAY, RPT_DEPTIND_READ, ( UCHAR)pKeyMsg->SEL.INPUT.lData);
                    } else {
                        sError = RptDEPTRead(CLASS_TTLCURPTD, RPT_DEPTIND_READ, ( UCHAR)pKeyMsg->SEL.INPUT.lData);
                    }
                    RptDEPTRead(0, RPT_FIN_READ, 0);                     /* Execute Finalize Procedure */
                } else {
                    if (uchUifACDataType == RPT_TYPE_DALY) {
                        sError = RptDEPTRead(CLASS_TTLCURDAY, RPT_ALL_READ, 0);
                    } else {
                        sError = RptDEPTRead(CLASS_TTLCURPTD, RPT_ALL_READ, 0);
                    }
                }
            }
            UieExit(aACEnter);                          /* Return to UifACEnter() */
            if (sError == RPT_ABORTED) {
                sError = SUCCESS;
            }
            return(sError);                                 

        case FSC_KEYED_DEPT:
            /* Check W/ Amount */
            if (pKeyMsg->SEL.INPUT.uchLen) {                                        /* Over Digit */
                return(LDT_SEQERR_ADR);
            }

            /* Clear Lead Through */
            MaintDisp(AC_PROG_READ_RPT,                 /* A/C Number */
                      0,                                /* Descriptor */
                      0,                                /* Page Number */
                      uchUifACDataType,                 /* PTD Type */
                      uchUifACRptType,                  /* Report Type */
                      0,                                /* Reset Type */
                      0L,                               /* Amount Data */
                      0);                               /* Disable  Lead Through */

            /* Get Dept#,   Saratoga */
            if ((sError = MaintGetDEPTNo(pKeyMsg->SEL.INPUT.uchMinor, 
                &usDEPTNumber)) != SUCCESS) {
                return(sError);
            }

            if (uchUifProgRptNo) {
                if (uchUifACDataType == RPT_TYPE_DALY) {
                    sError = ProgRptDeptRead(uchUifProgRptNo, RPT_TYPE_DALY, usDEPTNumber, uchUifMajorCode);
                } else {
                    sError = ProgRptDeptRead(uchUifProgRptNo, RPT_TYPE_PTD, usDEPTNumber, uchUifMajorCode);
                }
            } else {    /* standard format */
                if (uchUifACDataType == RPT_TYPE_DALY) {
                    sError = RptDEPTRead(CLASS_TTLCURDAY, RPT_DEPTIND_READ, pKeyMsg->SEL.INPUT.uchMinor);
                } else {
                    sError = RptDEPTRead(CLASS_TTLCURPTD, RPT_DEPTIND_READ, pKeyMsg->SEL.INPUT.uchMinor);
                }
                RptDEPTRead(0, RPT_FIN_READ, 0);                     /* Execute Finalize Procedure */
            }
            UieExit(aACEnter);                          /* Return to UifACEnter() */
            if (sError == RPT_ABORTED) {
                sError = SUCCESS;
            }
            return(sError);

        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                              /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC135EnterPLUType(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.135 Enter Report Type Mode 
*===============================================================================
*/

    
/* Define Next Function */

UIMENU FARCONST aChildAC135EnterPluType2[] = {{UifAC135EnterDEPTPLU1, CID_AC135ENTERDEPTPLU1},
                                             {NULL,              0                 }};


UIMENU FARCONST aChildAC135EnterPluType3[] = {{UifAC135EnterPLUCode1, CID_AC135ENTERPLUCODE1},
                                             {NULL,                 0                    }};


UIMENU FARCONST aChildAC135EnterPluType4[] = {{UifAC135EnterPLU1, CID_AC135ENTERPLU1},
                                             {NULL,             0                }};


SHORT UifAC135EnterPLUType(KEYMSG *pKeyMsg) 
{
    
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        uchUifMajorCode = 0;
        usUifDeptNo = 0;
        if (usUifLoopKeyType & PROGRPT_KEYFLAG_REPORT) {
            UieNextMenu(aChildAC135EnterPluType3);               /* Open Next Function */
        } else if (usUifLoopKeyType & PROGRPT_KEYFLAG_DEPTNO) {
            UieNextMenu(aChildAC135EnterPluType2);               /* Open Next Function */
        } else {
            UieNextMenu(aChildAC135EnterPluType4);               /* Open Next Function */
        }
        return(SUCCESS);

    case UIM_ACCEPTED:
        switch (pKeyMsg->SEL.usFunc) {
        case CID_AC135ENTERDEPTPLU1:                                /* Accepted from CID_AC29ENTERDEPT1 */
            UieNextMenu(aChildAC135EnterPluType2);               /* Open Next Function */
            return(SUCCESS);

        case CID_AC135ENTERPLUCODE1:                             /* Accepted from CID_AC29ENTERRPTCODE1 */
            UieNextMenu(aChildAC135EnterPluType3);               /* Open Next Function */
            return(SUCCESS);

        default:                                                /* Accepted from CID_AC29ENTERPLU1 */
            UieNextMenu(aChildAC135EnterPluType4);               /* Open Next Function */
            return(SUCCESS);
        }

    default:
        return(UifACDefProc(pKeyMsg));                                  /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC135EnterDEPTPLU1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.135 Enter DEPT 1 Mode 
*===============================================================================
*/

SHORT UifAC135EnterDEPTPLU1(KEYMSG *pKeyMsg) 
{
    SHORT   sError = SUCCESS;
    TCHAR   auchPLUNumber[PLU_MAX_DIGIT+1];
    USHORT  usDept;

    memset(auchPLUNumber, 0, sizeof(auchPLUNumber));

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC29EnterDEPT1);      /* Register Key Sequence */

        UifACPGMLDClear();

        MaintDisp(AC_PROG_READ_RPT,                 /* A/C Number */
                  CLASS_MAINTDSPCTL_10NPG,          /* Set Minor Class for 10N10D */
                  0,                                /* Page Number */
                  uchUifACDataType,                 /* PTD Type */
                  uchUifACRptType,                  /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_DEPTNO_ADR);                  /* "Request Department No. Entry" Lead Through Address */

        return(SUCCESS);
                                                                            
    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Clear Lead Through */

            MaintDisp(AC_PROG_READ_RPT,                 /* A/C Number */
                      0,                                /* Descriptor */
                      0,                                /* Page Number */
                      uchUifACDataType,                 /* PTD Type */
                      uchUifACRptType,                  /* Report Type */
                      0,                                /* Reset Type */
                      0L,                               /* Amount Data */
                      0);                               /* Disable  Lead Through */

            /* Check W/ Amount */

            if ((!pKeyMsg->SEL.INPUT.uchLen) &&                                      /* W/o Amount */
                !(usUifLoopKeyType & PROGRPT_KEYFLAG_PLUNO)) {

                /* execute all report */
                if (uchUifProgRptNo) {
                    if (uchUifACDataType == RPT_TYPE_DALY) {
                        sError = ProgRptPluRead(uchUifProgRptNo, RPT_TYPE_DALY, 0, 0, uchUifMajorCode);
                    } else {
                        sError = ProgRptPluRead(uchUifProgRptNo, RPT_TYPE_PTD, 0, 0, uchUifMajorCode);
                    }
                } else {    /* standard report */
                    if (uchUifACDataType == RPT_TYPE_DALY) {
                        sError = RptPLUDayRead(CLASS_TTLCURDAY, RPT_ALL_READ, 0, 0, auchPLUNumber);
                    } else {
                        sError = RptPLUPTDRead(CLASS_TTLCURPTD, RPT_ALL_READ, 0, 0, auchPLUNumber);
                    }
                }
                if (sError == RPT_ABORTED) {
                    sError = SUCCESS;
                }
                UieExit(aACEnter);                          /* Return to UifACEnter() */
                return(SUCCESS);

            } else if ((!pKeyMsg->SEL.INPUT.uchLen) &&                                      /* W/o Amount */
                (usUifLoopKeyType & PROGRPT_KEYFLAG_PLUNO)) {
                ;
            } else if (pKeyMsg->SEL.INPUT.lData == 0L) {
                return(LDT_KEYOVER_ADR);
            }

            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {                           /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }

            if (usUifLoopKeyType & PROGRPT_KEYFLAG_PLUNO) {
                /* go to plu# entry */
                usUifDeptNo = (USHORT)pKeyMsg->SEL.INPUT.lData;
                UieNextMenu(aChildAC135EnterPluType4);               /* Open Next Function */
                return(SUCCESS);
            }

            if (uchUifProgRptNo) {
                if (uchUifACDataType == RPT_TYPE_DALY) {
                    sError = ProgRptPluRead(uchUifProgRptNo, RPT_TYPE_DALY, 0, ( UCHAR)pKeyMsg->SEL.INPUT.lData, uchUifMajorCode);
                } else {
                    sError = ProgRptPluRead(uchUifProgRptNo, RPT_TYPE_PTD, 0, ( UCHAR)pKeyMsg->SEL.INPUT.lData, uchUifMajorCode);
                }
            } else {    /* standard report */
                if (uchUifACDataType == RPT_TYPE_DALY) {
                    sError = RptPLUDayRead(CLASS_TTLCURDAY, RPT_DEPT_READ, ( USHORT)pKeyMsg->SEL.INPUT.lData, 0, auchPLUNumber);
                } else {
                    sError = RptPLUPTDRead(CLASS_TTLCURPTD, RPT_DEPT_READ, ( USHORT)pKeyMsg->SEL.INPUT.lData, 0, auchPLUNumber);
                }
                RptPLUDayRead(CLASS_TTLIGNORE, RPT_FIN_READ, 0, 0, auchPLUNumber);                /* Execute Finalize Procedure */
            }

            if (sError == RPT_ABORTED) {
                sError = SUCCESS;
            }
            UieExit(aACEnter);
            return(sError);

        case FSC_KEYED_DEPT:
            /* Check W/ Amount */
            if (pKeyMsg->SEL.INPUT.uchLen) {                                        /* Over Digit */
                return(LDT_SEQERR_ADR);
            }

            if (usUifLoopKeyType & PROGRPT_KEYFLAG_PLUNO) {
                /* go to plu# entry */
                usUifDeptNo = pKeyMsg->SEL.INPUT.uchMinor;
                UieNextMenu(aChildAC135EnterPluType4);               /* Open Next Function */
                return(SUCCESS);
            }

            /* Clear Lead Through */
            MaintDisp(AC_PROG_READ_RPT,                 /* A/C Number */
                      0,                                /* Descriptor */
                      0,                                /* Page Number */
                      uchUifACDataType,                 /* PTD Type */
                      uchUifACRptType,                  /* Report Type */
                      0,                                /* Reset Type */
                      0L,                               /* Amount Data */
                      0);                               /* Disable Lead Through */

            if (uchUifProgRptNo) {
                if ((sError = MaintGetDEPTNo(pKeyMsg->SEL.INPUT.uchMinor, &usDept)) != SUCCESS) {
                    return(sError);
                }
                if (uchUifACDataType == RPT_TYPE_DALY) {
                    sError = ProgRptPluRead(uchUifProgRptNo, RPT_TYPE_DALY, 0, usDept, uchUifMajorCode);
                } else {
                    sError = ProgRptPluRead(uchUifProgRptNo, RPT_TYPE_PTD, 0, usDept, uchUifMajorCode);
                }

            } else {
                if ((sError = MaintGetDEPTNo(pKeyMsg->SEL.INPUT.uchMinor, &usDept)) != SUCCESS) {
                    return(sError);
                }
                if (uchUifACDataType == RPT_TYPE_DALY) {
                    sError = RptPLUDayRead(CLASS_TTLCURDAY, RPT_DEPT_READ, usDept, 0, auchPLUNumber);
                } else {
                    sError = RptPLUPTDRead(CLASS_TTLCURPTD, RPT_DEPT_READ, usDept, 0, auchPLUNumber);
                }
                RptPLUDayRead(CLASS_TTLIGNORE, RPT_FIN_READ, 0, 0, auchPLUNumber);        /* Execute Finalize Procedure */
            }

            if (sError == RPT_ABORTED) {
                sError = SUCCESS;
            }
            UieExit(aACEnter);
            return(sError);

        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                                              /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC135EnterPLUCode1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.135 Enter Report Code 1 Mode 
*===============================================================================
*/

SHORT UifAC135EnterPLUCode1(KEYMSG *pKeyMsg) 
{
    TCHAR   auchPLUNumber[PLU_MAX_DIGIT+1];
    SHORT   sError;

    memset(auchPLUNumber, 0, sizeof(auchPLUNumber));


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */

        UifACPGMLDClear();

        MaintDisp(AC_PROG_READ_RPT,                 /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  uchUifACDataType,                 /* PTD Type */
                  uchUifACRptType,                  /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_REPOCD_ADR);                  /* "Request Report Code # Entry" Lead Through Address */

        return(SUCCESS);
                                                                            
    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Clear Lead Through */
            MaintDisp(AC_PROG_READ_RPT,                 /* A/C Number */
                      0,                                /* Descriptor */
                      0,                                /* Page Number */
                      uchUifACDataType,                 /* PTD Type */
                      uchUifACRptType,                  /* Report Type */
                      0,                                /* Reset Type */
                      0L,                               /* Amount Data */
                      0);                               /* Disable Lead Through */

            /* Check W/ Amount */
            if ((!pKeyMsg->SEL.INPUT.uchLen) &&                                      /* W/o Amount */
                !(usUifLoopKeyType & (PROGRPT_KEYFLAG_DEPTNO|PROGRPT_KEYFLAG_PLUNO))) {

                /* execute all report */
                if (uchUifProgRptNo) {
                    if (uchUifACDataType == RPT_TYPE_DALY) {
                        sError = ProgRptPluRead(uchUifProgRptNo, RPT_TYPE_DALY, 0, 0, 0);
                    } else {
                        sError = ProgRptPluRead(uchUifProgRptNo, RPT_TYPE_PTD, 0, 0, 0);
                    }
                } else {    /* standard report */
                    if (uchUifACDataType == RPT_TYPE_DALY) {
                        sError = RptPLUDayRead(CLASS_TTLCURDAY, RPT_ALL_READ, 0, 0, auchPLUNumber);
                    } else {
                        sError = RptPLUPTDRead(CLASS_TTLCURPTD, RPT_ALL_READ, 0, 0, auchPLUNumber);
                    }
                }
                if (sError == RPT_ABORTED) {
                    sError = SUCCESS;
                }
                UieExit(aACEnter);                          /* Return to UifACEnter() */
                return(SUCCESS);
            } else if ((!pKeyMsg->SEL.INPUT.uchLen) &&                                      /* W/o Amount */
                (usUifLoopKeyType & (PROGRPT_KEYFLAG_DEPTNO|PROGRPT_KEYFLAG_PLUNO))) {
                ;
            } else if (pKeyMsg->SEL.INPUT.lData == 0L) {
                return(LDT_KEYOVER_ADR);
            }

            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {                           /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }

            if (usUifLoopKeyType & PROGRPT_KEYFLAG_DEPTNO) {
                /* go to dept# entry */
                uchUifMajorCode = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
                UieNextMenu(aChildAC135EnterPluType2);               /* Open Next Function */
                return(SUCCESS);
            }
            if (usUifLoopKeyType & PROGRPT_KEYFLAG_PLUNO) {
                /* go to plu# entry */
                uchUifMajorCode = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
                UieNextMenu(aChildAC135EnterPluType4);               /* Open Next Function */
                return(SUCCESS);
            }

            if (uchUifProgRptNo) {
                if (uchUifACDataType == RPT_TYPE_DALY) {
                    sError = ProgRptPluRead(uchUifProgRptNo, RPT_TYPE_DALY, 0, 0, ( UCHAR)pKeyMsg->SEL.INPUT.lData);

                } else {
                    sError = ProgRptPluRead(uchUifProgRptNo, RPT_TYPE_PTD, 0, 0, ( UCHAR)pKeyMsg->SEL.INPUT.lData);
                }
            } else {    /* standerd report */
                if (uchUifACDataType == RPT_TYPE_DALY) {
                    sError = RptPLUDayRead(CLASS_TTLCURDAY, RPT_CODE_READ, ( USHORT)pKeyMsg->SEL.INPUT.lData, 0, auchPLUNumber);
                } else {
                    sError = RptPLUPTDRead(CLASS_TTLCURPTD, RPT_CODE_READ, ( USHORT)pKeyMsg->SEL.INPUT.lData, 0, auchPLUNumber);
                }
                RptPLUDayRead(CLASS_TTLIGNORE, RPT_FIN_READ, 0, 0, auchPLUNumber);        /* Execute Finalize Procedure */
            }

            if (sError == RPT_ABORTED) {
                sError = SUCCESS;
            }
            UieExit(aACEnter);
            return(sError);

        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                          /* Execute Default Procedure */
    }
}
/*
*=============================================================================
**  Synopsis: SHORT UifAC135EnterPLU1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.135 Enter PLU 1 Mode 
*===============================================================================
*/

SHORT UifAC135EnterPLU1(KEYMSG *pKeyMsg) 
{
    USHORT          usPLUNumber = 0, sError;
    MAINTSETPAGE    SetPageNo;
    FSCTBL FAR      *pData;
    TCHAR           auchPLUNumber[PLU_MAX_DIGIT+1+1];
	UCHAR			uchMod;
    MAINTPLU        MaintPLU;

    memset(auchPLUNumber, 0, sizeof(auchPLUNumber));

    /* Clear SetPageNo Work */
    memset(&SetPageNo, '\0', sizeof(MAINTSETPAGE));

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC29EnterPLU1);       /* Register Key Sequence */
        UieEchoBack(UIE_ECHO_ROW0_NUMBER,UIF_DIGIT14);

        UifACPGMLDClear();

        MaintDisp(AC_PROG_READ_RPT,                 /* A/C Number */
                  CLASS_MAINTDSPCTL_10NPG,          /* Set Minor Class for 10N10D */
                  MaintGetPage(),                   /* Page Number */
                  uchUifACDataType,                 /* PTD Type */
                  uchUifACRptType,                  /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_NUM_ADR);                     /* "Number Entry" Lead Through Address */

        return(SUCCESS);
                                                                            
    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            if (!pKeyMsg->SEL.INPUT.uchLen) {                                       /* W/o Amount */

                /* Clear Lead Through */
                MaintDisp(AC_PROG_READ_RPT,                 /* A/C Number */
                          0,                                /* Descriptor */
                          0,                                /* Page Number */
                          uchUifACDataType,                 /* PTD Type */
                          uchUifACRptType,                  /* Report Type */
                          0,                                /* Reset Type */
                          0L,                               /* Amount Data */
                          0);                               /* Disable Lead Through */

                /* execute all report */
                if (uchUifProgRptNo) {
                    if (uchUifACDataType == RPT_TYPE_DALY) {
                        sError = ProgRptPluRead(uchUifProgRptNo, RPT_TYPE_DALY, auchPLUNumber, usUifDeptNo, uchUifMajorCode);
                    } else {
                        sError = ProgRptPluRead(uchUifProgRptNo, RPT_TYPE_PTD, auchPLUNumber, usUifDeptNo, uchUifMajorCode);
                    }
                } else {    /* standard report */
                    if (uchUifACDataType == RPT_TYPE_DALY) {
                        sError = RptPLUDayRead(CLASS_TTLCURDAY, RPT_ALL_READ, 0, 0, auchPLUNumber);
                    } else {
                        sError = RptPLUPTDRead(CLASS_TTLCURPTD, RPT_ALL_READ, 0, 0, auchPLUNumber);
                    }
                }
                if (sError == RPT_ABORTED) {
                    sError = SUCCESS;
                }
                UieExit(aACEnter);                          /* Return to UifACEnter() */
                return(sError);
            } else {
                return(LDT_SEQERR_ADR);
            }

        case FSC_MENU_SHIFT:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {                   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            SetPageNo.uchStatus = 0;                                       
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/ Amount Case */
                SetPageNo.uchStatus |= MAINT_WITHOUT_DATA;
            } else {
                SetPageNo.uchPageNumber = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }
            SetPageNo.uchMajorClass = CLASS_MAINTSETPAGE;                   /* Set Major Class */
            SetPageNo.uchACNumber = AC_PROG_READ_RPT;                       /* Set Major Class */
            SetPageNo.uchPTDType = uchUifACDataType;
            SetPageNo.uchRptType = 0;
            SetPageNo.uchLeadAddr = LDT_NUM_ADR;
            if ((sError = MaintSetPage(&SetPageNo)) == SUCCESS) {
                /* pData = (FSCTBL FAR *)&ParaFSC1[uchMaintMenuPage - 1]; */
                pData = (FSCTBL FAR *)&ParaFSC[uchMaintMenuPage - 1];
                UieSetFsc(pData);
                UieReject();                                                /* Shift Cur. Func. to Act. Func. */ 
            }
            return(sError);

        case FSC_D_MENU_SHIFT:
            /* Check W/o Amount */
            if (pKeyMsg->SEL.INPUT.uchLen) {                    /* W/ Amount */
                return(LDT_SEQERR_ADR);
            }
            SetPageNo.uchMajorClass = CLASS_MAINTSETPAGE;               /* Set Major Class */
            SetPageNo.uchACNumber = AC_PROG_READ_RPT;                   /* Set Major Class */
            SetPageNo.uchPTDType = uchUifACDataType;
            SetPageNo.uchRptType = 0;
            SetPageNo.uchLeadAddr = LDT_NUM_ADR;
            SetPageNo.uchPageNumber = pKeyMsg->SEL.INPUT.uchMinor;      /* set page number */
            SetPageNo.uchStatus = 0;                                    /* Set W/ Amount Status */
            if ((sError = MaintSetPage(&SetPageNo)) == SUCCESS) {
                /* pData = (FSCTBL FAR *)&ParaFSC1[uchMaintMenuPage - 1]; */
                pData = (FSCTBL FAR *)&ParaFSC[uchMaintMenuPage - 1];
                UieSetFsc(pData);
                UieReject();                                            /* Shift Current Func. to Active Func. */
            }
            return(sError);

        case FSC_SCANNER:                                   /* Saratoga */
            if((pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen > UIF_DIGIT14) ||  /* Over digit */
               (pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen == 0)) {           /* or no entry */
                return(LDT_KEYOVER_ADR);
            }
            _tcsncpy(auchPLUNumber, pKeyMsg->SEL.INPUT.DEV.SCA.auchStr, pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen);

            if (uchUifProgRptNo) {
                if (uchUifACDataType == RPT_TYPE_DALY) {
                    sError = ProgRptPluRead(uchUifProgRptNo, RPT_TYPE_DALY, auchPLUNumber, usUifDeptNo, uchUifMajorCode);
                } else {
                    sError = ProgRptPluRead(uchUifProgRptNo, RPT_TYPE_PTD, auchPLUNumber, usUifDeptNo, uchUifMajorCode);
                }
            } else {    /* standard report */
                if (uchUifACDataType == RPT_TYPE_DALY) {
                    sError = RptPLUDayRead(CLASS_TTLCURDAY, RPT_PLUIND_READ, 0, 0, auchPLUNumber);
                } else {
                    sError = RptPLUPTDRead(CLASS_TTLCURPTD, RPT_PLUIND_READ, 0, 0, auchPLUNumber);
                }
                RptPLUDayRead(CLASS_TTLIGNORE, RPT_FIN_READ, 0, 0, auchPLUNumber);    /* Execute Finalize Procedure */
            }
            if (sError == RPT_ABORTED) {
                sError = SUCCESS;
            }
            UieExit(aACEnter);
            return(sError);

        case FSC_E_VERSION:                         /* Saratoga */
            if (pKeyMsg->SEL.INPUT.uchLen) {        /* With Amount ? */
                return(LDT_KEYOVER_ADR);
            }
            MaintPLU.uchMajorClass = CLASS_PARAPLU; /* Set Major Class */
            MaintPLU.uchMinorClass = CLASS_PARAPLU_EVERSION;     /* Set Minor Class */
            MaintPLU.uchStatus = 0;                 /* Set W/ Amount Status */
            MaintPLU.usACNumber = AC_PROG_READ_RPT;
            MaintPLU.uchEVersion = 1;               /* Set E-Version */
            return(MaintPLUModifier(&MaintPLU));

        case FSC_PLU:
            /* Check W/ Amount */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                                       /* W/o Amount */
                return(LDT_SEQERR_ADR);
            }
            /* Check 0 Data and Digit */
            if (!pKeyMsg->SEL.INPUT.lData || (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT4)) {                           
                return(LDT_KEYOVER_ADR);
            }

            /* Clear Lead Through */
            MaintDisp(AC_PROG_READ_RPT,                 /* A/C Number */
                      0,                                /* Descriptor */
                      0,                                /* Page Number */
                      uchUifACDataType,                 /* PTD Type */
                      uchUifACRptType,                  /* Report Type */
                      0,                                /* Reset Type */
                      0L,                               /* Amount Data */
                      0);                               /* Disable Lead Through */

            _tcscpy(auchPLUNumber, pKeyMsg->SEL.INPUT.aszKey);   /* Saratoga */

            if (uchUifProgRptNo) {
                if (uchUifACDataType == RPT_TYPE_DALY) {
                    sError = ProgRptPluRead(uchUifProgRptNo, RPT_TYPE_DALY, auchPLUNumber, usUifDeptNo, uchUifMajorCode);
                } else {
                    sError = ProgRptPluRead(uchUifProgRptNo, RPT_TYPE_PTD, auchPLUNumber, usUifDeptNo, uchUifMajorCode);
                }
            } else {    /* standard report */
                if (uchUifACDataType == RPT_TYPE_DALY) {
                    sError = RptPLUDayRead(CLASS_TTLCURDAY, RPT_PLUIND_READ, 0, 0, auchPLUNumber);
                } else {
                    sError = RptPLUPTDRead(CLASS_TTLCURPTD, RPT_PLUIND_READ, 0, 0, auchPLUNumber);
                }
                RptPLUDayRead(CLASS_TTLIGNORE, RPT_FIN_READ, 0, 0, auchPLUNumber);    /* Execute Finalize Procedure */
            }
            if (sError == RPT_ABORTED) {
                sError = SUCCESS;
            }
            UieExit(aACEnter);
            return(sError);

        case FSC_KEYED_PLU:
            /* Check W/O Amount */

            if (pKeyMsg->SEL.INPUT.uchLen) {                    /* W/ Amount */
                return(LDT_KEYOVER_ADR);
            }

            /* Get PLU Number */
            if ((sError = MaintGetPLUNo(pKeyMsg->SEL.INPUT.uchMinor, 
                auchPLUNumber, &uchMod)) != SUCCESS) {
                return(sError);
            }

            /* Clear Lead Through */

            MaintDisp(AC_PROG_READ_RPT,                 /* A/C Number */
                      0,                                /* Descriptor */
                      0,                                /* Page Number */
                      uchUifACDataType,                 /* PTD Type */
                      uchUifACRptType,                  /* Report Type */
                      0,                                /* Reset Type */
                      0L,                               /* Amount Data */
                      0);                               /* Disable Lead Through */

            if (uchUifProgRptNo) {
                if (uchUifACDataType == RPT_TYPE_DALY) {
                    sError = ProgRptPluRead(uchUifProgRptNo, RPT_TYPE_DALY, auchPLUNumber, usUifDeptNo, uchUifMajorCode);
                } else {
                    sError = ProgRptPluRead(uchUifProgRptNo, RPT_TYPE_PTD, auchPLUNumber, usUifDeptNo, uchUifMajorCode);
                }
            } else {    /* standard report */
                if (uchUifACDataType == RPT_TYPE_DALY) {
                    sError = RptPLUDayRead(CLASS_TTLCURDAY, RPT_PLUIND_READ, 0, 0, auchPLUNumber);
                } else {
                    sError = RptPLUPTDRead(CLASS_TTLCURPTD, RPT_PLUIND_READ, 0, 0, auchPLUNumber);
                }
                RptPLUDayRead(CLASS_TTLIGNORE, RPT_FIN_READ, 0, 0, auchPLUNumber);    /* Execute Finalize Procedure */
            }
            if (sError == RPT_ABORTED) {
                sError = SUCCESS;
            }
            UieExit(aACEnter);
            return(sError);

        /* default:
            break; */
        }
        /* break; */

    case UIM_CANCEL:
        MaintPLU.uchMajorClass = CLASS_PARAPLU; /* Set Major Class */
        MaintPLU.uchMinorClass = CLASS_PARAPLU_RESET_EV;     /* Set Minor Class */
        MaintPLU.uchStatus = 0;                 /* Set W/ Amount Status */
        MaintPLU.usACNumber = AC_PROG_READ_RPT;
        MaintPLU.uchEVersion = 0;               /* Set E-Version */
        MaintPLUModifier(&MaintPLU);

    default:
        return(UifACDefProc(pKeyMsg));                                              /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC135EnterCpnType(KEYMSG *pKeyMsg) 
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

SHORT UifAC135EnterCpnType(KEYMSG *pKeyMsg) 
{
    SHORT   sError;

    switch(pKeyMsg->uchMsg) {
        case UIM_INIT:
            UieOpenSequence(aszSeqAC30EnterData);
            
            MaintDisp(AC_PROG_READ_RPT,                  /* A/C Number       */
                      CLASS_MAINTDSPCTL_10NPG,          /* Set Minor Class for 10N10D */
                      0,
                      uchUifACDataType,                 /* PTD Type         */
                      uchUifACRptType,                  /* Report Type */
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
                if (!pKeyMsg->SEL.INPUT.uchLen) {                                       /* W/o Amount */

					/* Clear Lead Through */
					MaintDisp(AC_PROG_READ_RPT,                 /* A/C Number */
							  0,                                /* Descriptor */
							  0,                                /* Page Number */
							  uchUifACDataType,                 /* PTD Type */
							  uchUifACRptType,                  /* Report Type */
							  0,                                /* Reset Type */
							  0L,                               /* Amount Data */
							  0);                               /* Disable Lead Through */

                    if (uchUifProgRptNo) {
                        /* Daily Report Case */
                        if(uchUifACDataType == RPT_TYPE_DALY){
                            sError = ProgRptCpnRead(uchUifProgRptNo, RPT_TYPE_DALY, 0);
                        }
                        /* PTD Report Case */
                        else{
                            sError = ProgRptCpnRead(uchUifProgRptNo, RPT_TYPE_PTD, 0);
                        }
                    } else {    /* standard format */
                        /* Daily Report Case */
                        if(uchUifACDataType == RPT_TYPE_DALY){
                            sError = RptCpnRead(CLASS_TTLCURDAY,RPT_IND_READ, (UCHAR)pKeyMsg->SEL.INPUT.lData);
                        }
                        /* PTD Report Case */
                        else{
                            sError = RptCpnRead(CLASS_TTLCURPTD,RPT_IND_READ, (UCHAR)pKeyMsg->SEL.INPUT.lData);
                        }
                    }
                    if (sError == RPT_ABORTED) {
                        sError = SUCCESS;
                    }
                    UieExit(aACEnter);
                    return(sError);
                } else {

                    return(LDT_SEQERR_ADR);
                }

            case FSC_COUPON:
                /* Without anything keys or Over 3 digit */
                if((pKeyMsg->SEL.INPUT.uchLen <= 0) ||
                   (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT3)) {
                    return(LDT_SEQERR_ADR);
                }

                /* Clear Lead Through */
                MaintDisp(AC_PROG_READ_RPT,                 /* A/C Number */
                          0,                                /* Descriptor */
                          0,                                /* Page Number */
                          uchUifACDataType,                 /* PTD Type */
                          uchUifACRptType,                  /* Report Type */
                          0,                                /* Reset Type */
                          0L,                               /* Amount Data */
                          0);                               /* Disable Lead Through */

                if (!pKeyMsg->SEL.INPUT.uchLen) {                                       /* W/o Amount */
                    /* all report */
                    if (uchUifProgRptNo) {
                        /* Daily Report Case */
                        if(uchUifACDataType == RPT_TYPE_DALY){
                            sError = ProgRptCpnRead(uchUifProgRptNo, RPT_TYPE_DALY, 0);
                        }
                        /* PTD Report Case */
                        else{
                            sError = ProgRptCpnRead(uchUifProgRptNo, RPT_TYPE_PTD, 0);
                        }
                    } else {    /* standard format */
                        /* Daily Report Case */
                        if(uchUifACDataType == RPT_TYPE_DALY){
                            sError = RptCpnRead(CLASS_TTLCURDAY,RPT_IND_READ, (UCHAR)pKeyMsg->SEL.INPUT.lData);
                        }
                        /* PTD Report Case */
                        else{
                            sError = RptCpnRead(CLASS_TTLCURPTD,RPT_IND_READ, (UCHAR)pKeyMsg->SEL.INPUT.lData);
                        }
                    }
                    if (sError == RPT_ABORTED) {
                        sError = SUCCESS;
                    }
                    UieExit(aACEnter);
                    return(sError);
                }

                /* Check 0 Data */
                if(pKeyMsg->SEL.INPUT.lData == 0){
                    return(LDT_KEYOVER_ADR);
                }

                if (uchUifProgRptNo) {
                    /* Daily Report Case */
                    if(uchUifACDataType == RPT_TYPE_DALY){
                        sError = ProgRptCpnRead(uchUifProgRptNo, RPT_TYPE_DALY, (UCHAR)pKeyMsg->SEL.INPUT.lData);
                    }
                    /* PTD Report Case */
                    else{
                        sError = ProgRptCpnRead(uchUifProgRptNo, RPT_TYPE_PTD, (UCHAR)pKeyMsg->SEL.INPUT.lData);
                    }
                } else {    /* standard format */
                    /* Daily Report Case */
                    if(uchUifACDataType == RPT_TYPE_DALY){
                        sError = RptCpnRead(CLASS_TTLCURDAY,RPT_IND_READ, (UCHAR)pKeyMsg->SEL.INPUT.lData);
                    }
                    /* PTD Report Case */
                    else{
                        sError = RptCpnRead(CLASS_TTLCURPTD,RPT_IND_READ, (UCHAR)pKeyMsg->SEL.INPUT.lData);
                    }
                    RptCpnRead(0, RPT_FIN_READ, 0);                       /* Execute Finalize Procedure */
                }
                UieExit(aACEnter);                                                       
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
                MaintDisp(AC_PROG_READ_RPT,                  /* A/C Number  */
                          0, 0,
                          uchUifACDataType,                 /* PTD Type    */
                          uchUifACRptType,                  /* Report Type */
                          0, 0L, 0);

                if (uchUifProgRptNo) {
                    /* Daily Report Case */
                    if(uchUifACDataType == RPT_TYPE_DALY){
                        sError = ProgRptCpnRead(uchUifProgRptNo,  RPT_TYPE_DALY, pKeyMsg->SEL.INPUT.uchMinor);
                    }
                    /* PTD Report Case */
                    else{
                        sError = ProgRptCpnRead(uchUifProgRptNo, RPT_TYPE_PTD, pKeyMsg->SEL.INPUT.uchMinor);
                    }
                } else {    /* standard format */
                    /* Daily Report Case */
                    if(uchUifACDataType == RPT_TYPE_DALY){
                        sError = RptCpnRead(CLASS_TTLCURDAY,RPT_IND_READ, pKeyMsg->SEL.INPUT.uchMinor);
                    }
                    /* PTD Report Case */
                    else{
                        sError = RptCpnRead(CLASS_TTLCURPTD,RPT_IND_READ, pKeyMsg->SEL.INPUT.uchMinor);
                    }
                    RptCpnRead(0, RPT_FIN_READ, 0);                       /* Execute Finalize Procedure */
                }
                UieExit(aACEnter);                                                       

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

/*
*=============================================================================
**  Synopsis: SHORT UifAC135EnterCasType(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.135 Enter Cashier Mode 
*===============================================================================
*/
SHORT UifAC135EnterCasType(KEYMSG *pKeyMsg) 
{
    SHORT   sError;                    

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */

        MaintDisp(AC_PROG_READ_RPT,                 /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  uchUifACDataType,                 /* PTD Type */
                  uchUifACRptType,                  /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_NUM_ADR);                     /* "Number Entry" Lead Through Address */

        return(SUCCESS);
                                                                            
    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Clear Lead Through */
            MaintDisp(AC_PROG_READ_RPT,             /* A/C Number */
                      0,                            /* Descriptor */
                      0,                            /* Page Number */
                      uchUifACDataType,             /* PTD Type */
                      uchUifACRptType,              /* Report Type */
                      0,                            /* Reset Type */
                      0L,                           /* Amount Data */
                      0);                           /* Disable Lead Through */

            /* Check W/ Amount */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount */
                if (uchUifProgRptNo) {
                    if (uchUifACDataType == RPT_TYPE_DALY) {    /* V3.3 */
                        sError = ProgRptCasRead(uchUifProgRptNo, RPT_TYPE_DALY, 0);
                    } else {
                        sError = ProgRptCasRead(uchUifProgRptNo, RPT_TYPE_PTD, 0);
                    }
                } else {    /* standard format */
                    if (uchUifACDataType == RPT_TYPE_DALY) {    /* V3.3 */
                        sError = RptCashierRead(CLASS_TTLCURDAY, RPT_ALL_READ, 0, 0);                         /* Execute All Read Report */
                    } else {
                        sError = RptCashierRead(CLASS_TTLCURPTD, RPT_ALL_READ, 0, 0);                         /* Execute All Read Report */
                    }
                }
                UieExit(aACEnter);
                if (sError == RPT_ABORTED) {                        /* Aborted by User */
                    sError = SUCCESS;
                }
                return(sError);
            }

            /* Check 0 Data and Digit */
            if (!pKeyMsg->SEL.INPUT.lData || (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT3)) {      
                MaintDisp(AC_PROG_READ_RPT,                 /* A/C Number */
                          0,                                /* Descriptor */
                          0,                                /* Page Number */
                          uchUifACDataType,                 /* PTD Type */
                          uchUifACRptType,                  /* Report Type */
                          0,                                /* Reset Type */
                          0L,                               /* Amount Data */
                          LDT_NUM_ADR);                     /* "Number Entry" Lead Through Address */

                return(LDT_KEYOVER_ADR);
            }

            /* Execute Individual Read */
            if (uchUifProgRptNo) {
                if (uchUifACDataType == RPT_TYPE_DALY) {    /* V3.3 */
                    sError = ProgRptCasRead(uchUifProgRptNo, RPT_TYPE_DALY, (ULONG)pKeyMsg->SEL.INPUT.lData);
                } else {
                    sError = ProgRptCasRead(uchUifProgRptNo, RPT_TYPE_PTD, (ULONG)pKeyMsg->SEL.INPUT.lData);
                }
            } else {    /* standard format */
                if (uchUifACDataType == RPT_TYPE_DALY) {    /* V3.3 */
                    sError = RptCashierRead(CLASS_TTLCURDAY, RPT_IND_READ, (ULONG)pKeyMsg->SEL.INPUT.lData, 0);
                } else {
                    sError = RptCashierRead(CLASS_TTLCURPTD, RPT_IND_READ, (ULONG)pKeyMsg->SEL.INPUT.lData, 0);
                }
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
**  Synopsis: SHORT UifAC135EnterIndFintype(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.135 Enter Individual financial Mode 
*===============================================================================
*/
SHORT UifAC135EnterIndFinType(KEYMSG *pKeyMsg) 
{
    SHORT   sError;                    

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */

        MaintDisp(AC_PROG_READ_RPT,                 /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  uchUifACRptType,                  /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_NUM_ADR);                     /* "Number Entry" Lead Through Address */

        return(SUCCESS);
                                                                            
    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Clear Lead Through */
            MaintDisp(AC_PROG_READ_RPT,             /* A/C Number */
                      0,                            /* Descriptor */
                      0,                            /* Page Number */
                      0,                            /* PTD Type */
                      uchUifACRptType,              /* Report Type */
                      0,                            /* Reset Type */
                      0L,                           /* Amount Data */
                      0);                           /* Disable Lead Through */

            /* Check W/ Amount */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount */
                if (uchUifProgRptNo) {
                    sError = ProgRptIndFinRead(uchUifProgRptNo, 
                                           0);
                } else {    /* starndard report */
                    sError = RptIndFinDayRead(CLASS_TTLCURDAY, RPT_ALL_READ, 0, 0);                         /* Execute All Read Report */
                }
                UieExit(aACEnter);
                if (sError == RPT_ABORTED) {                        /* Aborted by User */
                    sError = SUCCESS;
                }
                return(sError);
            }

            /* Check 0 Data and Digit */
            if (!pKeyMsg->SEL.INPUT.lData || (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT3)) {      
                MaintDisp(AC_PROG_READ_RPT,                 /* A/C Number */
                          0,                                /* Descriptor */
                          0,                                /* Page Number */
                          0,                                /* PTD Type */
                          uchUifACRptType,                  /* Report Type */
                          0,                                /* Reset Type */
                          0L,                               /* Amount Data */
                          LDT_NUM_ADR);                     /* "Number Entry" Lead Through Address */

                return(LDT_KEYOVER_ADR);
            }

            /* Execute Individual Read */
            if (uchUifProgRptNo) {
                sError = ProgRptIndFinRead(uchUifProgRptNo, ( USHORT)pKeyMsg->SEL.INPUT.lData);
            } else {    /* starndard report */
                sError = RptIndFinDayRead(CLASS_TTLCURDAY, RPT_IND_READ, ( USHORT)pKeyMsg->SEL.INPUT.lData, 0);
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