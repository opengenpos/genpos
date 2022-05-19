/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-8          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Cashier Reset Report Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC19.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC19Function()       : A/C No.19 Function Entry Mode
*               UifAC19EnterDataType()  : A/C No.19 Enter Data Type Mode
*               UifAC19EnterRstType()   : A/C No.19 Enter Reset Type Mode
*               UifAC19EnterRptType()   : A/C No.19 Enter Report Type Mode
*               UifAC19EnterData()      : A/C No.19 Enter Cashier No. Mode
*               UifAC19EnterKey()       : A/C No.19 Reset Confirm Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jul-10-92:00.00.01    :K.You      : initial                                   
* Nov-26-92:01.00.00    :K.You      : Display Cashier No. at Ind. Reset                                    
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
#include <uireg.h>
#include <uifpgequ.h>
#include <mldmenu.h>

#include "uifsupex.h"                       /* Unique Extern Header for UI */

/*
==============================================================================
;                      C O M M O N   R O M   T A B L E s                         
;=============================================================================
*/

/* Define Next Function */

UIMENU FARCONST aChildAC19EnterDataType[] = {{UifAC19EnterRstType, CID_AC19ENTERRSTTYPE},
                                             {NULL,                0                   }};

/*
*=============================================================================
**  Synopsis: SHORT UifAC19Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.19 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC19Init[] = {{UifAC19EnterDataType, CID_AC19ENTERDATATYPE},
                                    {NULL,                 0                    }};


SHORT UifAC19Function(KEYMSG *pKeyMsg) 
{
    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:

        /* Check PTD Exist or Not, V3.3 */
        if (!RptPTDChkFileExist(FLEX_CAS_ADR)) {         /* Not PTD Case */
            uchUifACDataType = ( UCHAR)RPT_TYPE_DALY;   /* Set Data Type to Own Save Area */
            UieNextMenu(aChildAC19EnterDataType);
        } else {
            UieNextMenu(aChildAC19Init);                /* Open Next Function */
        }
        return(SUCCESS);                                        

    default:
        return(UifACDefProc(pKeyMsg));                  /* execute Default Procedure */
    }
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC19EnterDataType(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.19 Enter Data Type Mode  V3.3
*===============================================================================
*/
    
SHORT UifAC19EnterDataType(KEYMSG *pKeyMsg) 
{
    

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC23EnterDataType);   /* Register Key Sequence */

        UifACPGMLDDispCom(DaylyPTDSelect);

        MaintDisp(AC_CASH_RESET_RPT,                 /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  0,                                /* Report Type */
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
            if (!pKeyMsg->SEL.INPUT.uchLen) {                                   /* W/o Amount */
                return(LDT_SEQERR_ADR);
            }       
        
            /* Check Input Data Range */
            if (pKeyMsg->SEL.INPUT.lData != RPT_TYPE_DALY && pKeyMsg->SEL.INPUT.lData != RPT_TYPE_PTD) {
                return(LDT_KEYOVER_ADR);
            }
            
            /* Set Data Type to Own Save Area */
            uchUifACDataType = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            
            UieNextMenu(aChildAC19EnterDataType);
            return(SUCCESS);
        
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
**  Synopsis: SHORT UifAC19EnterRstType(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.19 Enter Reset Type Mode 
*===============================================================================
*/
    
/* Define Next Function */

UIMENU FARCONST aChildAC19EnterRstType[] = {{UifAC19EnterRptType, CID_AC19ENTERRPTTYPE},
                                            {NULL,               0                   }};




SHORT UifAC19EnterRstType(KEYMSG *pKeyMsg) 
{
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC23EnterDataType);   /* Register Key Sequence */

        UifACPGMLDDispCom(ResetTypeSelect);

        MaintDisp(AC_CASH_RESET_RPT,                 /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  uchUifACDataType,                 /* PTD Type */
                  0,                                /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_FUNCTYPE_ADR);                /* "Request Function Type Entry" Lead Through Address */

        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {      
        case FSC_NUM_TYPE:
            return(LDT_SEQERR_ADR);

        case FSC_AC:
            /* Check W/ Amount */
            if (!pKeyMsg->SEL.INPUT.uchLen) {           /* W/o Input Data */                                   /* W/o Amount */
                return(LDT_SEQERR_ADR);
            }       
        
            /* Check Input Data */
            if ((pKeyMsg->SEL.INPUT.lData == RPT_TYPE_ONLY_PRT) ||      /* Only Print Type */
                (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_RST_PRT)  ||      /* Reset and Print Type */
                (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_ONLY_RST)) {      /* Only Reset Type */
                uchUifACRstType = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
                UieNextMenu(aChildAC19EnterRstType);                    /* Open Next Function */
                return(SUCCESS);
            } else {
               return(LDT_KEYOVER_ADR);
            }

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
**  Synopsis: SHORT UifAC19EnterRptType(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.19 Enter Report Type Mode 
*===============================================================================
*/
    

/* Define Next Function */

UIMENU FARCONST aChildAC19EnterRptType1[] = {{UifAC19EnterKey, CID_AC19ENTERKEY},
                                             {NULL,            0               }};


UIMENU FARCONST aChildAC19EnterRptType2[] = {{UifAC19EnterData, CID_AC19ENTERDATA},
                                    {NULL,             0                }};


SHORT UifAC19EnterRptType(KEYMSG *pKeyMsg) 
{
    SHORT   sError;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */

        UifACPGMLDDispCom(TypeSelectReport2);

        MaintDisp(AC_CASH_RESET_RPT,                 /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  uchUifACDataType,                 /* PTD Type */
                  0,                                /* Report Type */
                  uchUifACRstType,                  /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_REPOTYPE_ADR);                /* "Request Report Type Entry" Lead Through Address */

        return(SUCCESS);
                                                                            
    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Check W/ Amount */
            if (!pKeyMsg->SEL.INPUT.uchLen) {           /* W/o Input Data */                                   /* W/o Amount */
                return(LDT_SEQERR_ADR);
            }       
        
            /* Check Input Data */
            if (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_ALL) {             /* Reset All Type */
                uchUifACRptType = ( UCHAR)RPT_TYPE_ALL;                 /* Set Report Type to Save Area */
                if (uchUifACRstType != ( UCHAR)RPT_TYPE_ONLY_PRT) {     /* Not Only Print Type */
                    if ((sError = RptCashierLock(RPT_ALL_LOCK, 0)) != SUCCESS) {
                        UieExit(aACEnter);                              /* Return to UifACEnter() */
                        return(sError);
                    } 
                    UieNextMenu(aChildAC19EnterRptType1);               /* Open Next Function */
                    return(SUCCESS);
                } else {
                    /* Clear Lead Through */
                    UifACPGMLDClear();

                    MaintDisp(AC_CASH_RESET_RPT,                         /* A/C Number */
                              0,                                        /* Descriptor */
                              0,                                        /* Page Number */
                              uchUifACDataType,                         /* PTD Type */
                              ( UCHAR)RPT_TYPE_ALL,                     /* Report Type */
                              uchUifACRstType,                          /* Reset Type */
                              0L,                                       /* Amount Data */
                              0);                                       /* Disable Lead Through */

                    /* V3.3 */
                    if (uchUifACDataType == ( UCHAR)RPT_TYPE_DALY) {    /* Daily File Type */
                        sError = RptCashierRead(CLASS_TTLSAVDAY, RPT_ALL_READ, 0, RPT_ONLY_PRT_RESET); 
                    } else {
                        sError = RptCashierRead(CLASS_TTLSAVPTD, RPT_ALL_READ, 0, RPT_ONLY_PRT_RESET); 
                    }
                    UieExit(aACEnter);                                              /* Return to UifACEnter() */
                    if (sError == RPT_ABORTED) {                                    /* Aborted by User */
                        sError = SUCCESS;
                    }
                    return(sError);
                }
            } else if (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_IND) {      /* Individual Reset Type */
                uchUifACRptType = ( UCHAR)RPT_TYPE_IND;                 /* Set Report Type to Save Area */
                UieNextMenu(aChildAC19EnterRptType2);                   /* Open Next Function */
                return(SUCCESS);
            } else {
                return(LDT_KEYOVER_ADR);
            }

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
**  Synopsis: SHORT UifAC19EnterData(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.19 Enter Cashier No. Mode 
*===============================================================================
*/
SHORT UifAC19EnterData(KEYMSG *pKeyMsg) 
{
    SHORT   sError;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */

        UifACPGMLDClear();

        MaintDisp(AC_CASH_RESET_RPT,                 /* A/C Number */
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
            /* Check W/ Amount */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount */
                return(LDT_SEQERR_ADR);
            }
        
            /* Check 0 Data and Digit */
            if (!pKeyMsg->SEL.INPUT.lData || (pKeyMsg->SEL.INPUT.uchLen > UIFREG_MAX_ID_DIGITS)) {   
                return(LDT_KEYOVER_ADR);
            }       

            if (uchUifACRstType == ( UCHAR)RPT_TYPE_ONLY_PRT) {         /* Only Print Reset Type  */

                /* Clear Lead Through */
                UifACPGMLDClear();

                MaintDisp(AC_CASH_RESET_RPT,                             /* A/C Number */
                          0,                                            /* Descriptor */
                          0,                                            /* Page Number */
                          uchUifACDataType,                             /* PTD Type */
                          uchUifACRptType,                              /* Report Type */
                          uchUifACRstType,                              /* Reset Type */
                          0L,                                           /* Amount Data */
                          0);                                           /* Disable Lead Through */

                /* V3.3 */
                if (uchUifACDataType == ( UCHAR)RPT_TYPE_DALY) {
                    sError = RptCashierRead(CLASS_TTLSAVDAY, RPT_IND_READ, (ULONG)pKeyMsg->SEL.INPUT.lData, RPT_ONLY_PRT_RESET); 
                } else {
                    sError = RptCashierRead(CLASS_TTLSAVPTD, RPT_IND_READ, (ULONG)pKeyMsg->SEL.INPUT.lData, RPT_ONLY_PRT_RESET); 
                }
                UieExit(aACEnter);                                              /* Return to UifACEnter() */
                if (sError == RPT_ABORTED) {                                    /* Aborted by User */
                    sError = SUCCESS;
                }
                return(sError);
            }

            if ((sError = RptCashierLock(RPT_IND_LOCK, (ULONG)pKeyMsg->SEL.INPUT.lData)) == SUCCESS) {
                ulUifACRptCashierNo = (ULONG)pKeyMsg->SEL.INPUT.lData;   /* Save Cashier Number to Own Save Area */
                UieNextMenu(aChildAC19EnterRptType1);                   /* Open Next Function */
                return(SUCCESS);
            }
            UieExit(aACEnter);                                              
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
**  Synopsis: SHORT UifAC19EnterKey(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.19 Reset Confirm Mode 
*===============================================================================
*/
SHORT UifAC19EnterKey(KEYMSG *pKeyMsg) 
{
    SHORT   sError;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC3EnterKey);         /* Register Key Sequence */

        UifACPGMLDClear();

        MaintDisp(AC_CASH_RESET_RPT,                 /* A/C Number */
                  CLASS_MAINTDSPCTL_10N,            /* 10N10D Display */
                  0,                                /* Page Number */
                  uchUifACDataType,                 /* PTD Type */
                  uchUifACRptType,                  /* Report Type */
                  uchUifACRstType,                  /* Reset Type */
                  ulUifACRptCashierNo,              /* Server Number */
                  LDT_RESET_ADR);                   /* "Lead Through for Reset Report"Lead Through Address */

        return(SUCCESS);
                                                                            
    case UIM_INPUT:
        /* Check W/ Amount */
        if (pKeyMsg->SEL.INPUT.uchLen) {                                /* W/ Amount */
            return(LDT_SEQERR_ADR);
        }
        
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:

            /* Clear Lead Through */
            UifACPGMLDClear();

            MaintDisp(AC_CASH_RESET_RPT,                 /* A/C Number */
                      0,                                /* Descriptor */
                      0,                                /* Page Number */
                      uchUifACDataType,                 /* PTD Type */
                      uchUifACRptType,                  /* Report Type */
                      uchUifACRstType,                  /* Reset Type */
                      0L,                               /* Amount Data */
                      0);                               /* Disable Lead Through */

            if (uchUifACRptType == ( UCHAR)RPT_TYPE_ALL) {                  /* All Reset Type */
                if (uchUifACRstType == ( UCHAR)RPT_TYPE_RST_PRT) {          /* Reset and Print Type */
                    /* V3.3 */
                    if (uchUifACDataType == ( UCHAR)RPT_TYPE_DALY) {        /* Daily File Type */
                        if ((sError = RptCashierReset(CLASS_TTLCURDAY, RPT_ALL_RESET, RPT_PRT_RESET, 0)) == SUCCESS) {    
                            sError = RptCashierRead(CLASS_TTLSAVDAY, RPT_ALL_READ, 0, RPT_PRT_RESET);
                        }
                    } else {
                        if ((sError = RptCashierReset(CLASS_TTLCURPTD, RPT_ALL_RESET, RPT_PRT_RESET, 0)) == SUCCESS) {     
                            sError = RptCashierRead(CLASS_TTLSAVPTD, RPT_ALL_READ, 0, RPT_PRT_RESET);
                        }
                    }
                } else {                                                    /* Only Reset Type */
                    /* V3.3 */
                    if (uchUifACDataType == ( UCHAR)RPT_TYPE_DALY) {
                        sError = RptCashierReset(CLASS_TTLCURDAY, RPT_ALL_RESET, RPT_ONLY_RESET, 0);                      /* Only Reset All Cashier File */
                    } else {
                        sError = RptCashierReset(CLASS_TTLCURPTD, RPT_ALL_RESET, RPT_ONLY_RESET, 0);                      /* Only Reset All Cashier File */
                    }
                }
                RptCashierUnLock(RPT_ALL_UNLOCK, 0);
            } else if (uchUifACRptType == ( UCHAR)RPT_TYPE_IND) {           /* Individual Reset Type */
                if (uchUifACRstType == ( UCHAR)RPT_TYPE_RST_PRT) {          /* Reset and Print Type */
                    /* V3.3 */
                    if (uchUifACDataType == ( UCHAR)RPT_TYPE_DALY) {
                        if ((sError = RptCashierReset(CLASS_TTLCURDAY, RPT_IND_RESET, RPT_PRT_RESET, ulUifACRptCashierNo)) == SUCCESS) {    /* Reset and Print Cashier File */
                            sError = RptCashierRead(CLASS_TTLSAVDAY, RPT_IND_READ, ulUifACRptCashierNo, RPT_PRT_RESET);
                        }
                    } else {
                        if ((sError = RptCashierReset(CLASS_TTLCURPTD, RPT_IND_RESET, RPT_PRT_RESET, ulUifACRptCashierNo)) == SUCCESS) {    /* Reset and Print Cashier File */
                            sError = RptCashierRead(CLASS_TTLSAVPTD, RPT_IND_READ, ulUifACRptCashierNo, RPT_PRT_RESET);
                        }
                    }
                } else {                                                    /* Only Reset Type */
                    /* V3.3 */
                    if (uchUifACDataType == ( UCHAR)RPT_TYPE_DALY) {
                        sError = RptCashierReset(CLASS_TTLCURDAY, RPT_IND_RESET, RPT_ONLY_RESET, ulUifACRptCashierNo);  /* Only Reset Cashier File */
                    } else {
                        sError = RptCashierReset(CLASS_TTLCURPTD, RPT_IND_RESET, RPT_ONLY_RESET, ulUifACRptCashierNo);  /* Only Reset Cashier File */
                    }
                }
                RptCashierUnLock(RPT_IND_UNLOCK, ulUifACRptCashierNo);
            }
            UieExit(aACEnter);                                              /* Return to UifACEnter() */
            if (sError == RPT_ABORTED) {                                    /* Aborted by User */
                sError = SUCCESS;
            }
            return(sError);

        case FSC_CANCEL:
            if (uchUifACRptType == ( UCHAR)RPT_TYPE_ALL) {                  /* All Reset Type */
                RptCashierUnLock(RPT_ALL_UNLOCK, 0);
            } else {                                                        /* Individual Reset Type */
                RptCashierUnLock(RPT_IND_UNLOCK, ulUifACRptCashierNo);
            }
            UieExit(aACEnter);                                              /* Return to UifACEnter() */
            return(SUCCESS);

        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}
/* --- End of Source --- */
