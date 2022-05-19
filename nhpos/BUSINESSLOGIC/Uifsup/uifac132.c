/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995-8          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Service Time Reset Report Module     
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC132.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Gs1 /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC132Function()       : A/C No.132 Function Entry Mode
*               UifAC132EnterDataType()  : A/C No.132 Enter Data Type Mode
*               UifAC132EnterRstType()   : A/C No.132 Enter Reset Type Mode
*               UifAC132EnterKey()       : A/C No.132 Reset Confirm Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.    :   Name    : Description
* Dec-05-95: 03.01.00    : M.Ozawa   : initial   
* Aug-27-98: 03.03.00    : hrkato    : V3.3
* Aug-16-99: 03.05.00    : K.Iwata   : V3.5 merge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
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

#include "uifsupex.h"                       /* Unique Extern Header for UI */



/*
==============================================================================
;                      C O M M O N   R O M   T A B L E s                         
;=============================================================================
*/


/* Define Next Function */

UIMENU FARCONST aChildAC132EnterDataType[] = {{UifAC132EnterRstType, CID_AC132ENTERRSTTYPE},
                                             {NULL,                0                   }};


/*
*=============================================================================
**  Synopsis: SHORT UifAC132Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.132 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC132Init[] = {{UifAC132EnterDataType, CID_AC132ENTERDATATYPE},
                                    {NULL,                 0                    }};



SHORT UifAC132Function(KEYMSG *pKeyMsg) 
{



    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC132Init);                            /* Open Next Function */
        return(SUCCESS);                                        

    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC132EnterDataType(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.132 Enter Data Type Mode 
*===============================================================================
*/
    

SHORT UifAC132EnterDataType(KEYMSG *pKeyMsg) 
{
    
                    

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC23EnterDataType);   /* Register Key Sequence */

        UifACPGMLDDispCom(DaylyPTDSelect);

        MaintDisp(AC_SERVICE_RESET_RPT,                 /* A/C Number */
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
            
            UieNextMenu(aChildAC132EnterDataType);
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
**  Synopsis: SHORT UifAC132EnterRstType(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.132 Enter Reset Type Mode 
*===============================================================================
*/
    
/* Define Next Function */

UIMENU FARCONST aChildAC132EnterRstType[] = {{UifAC132EnterKey, CID_AC132ENTERKEY},
                                            {NULL,            0               }};




SHORT UifAC132EnterRstType(KEYMSG *pKeyMsg) 
{
    
                    
    SHORT   sError;


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC23EnterDataType);   /* Register Key Sequence */

        UifACPGMLDDispCom(ResetTypeSelect);

        MaintDisp(AC_SERVICE_RESET_RPT,             /* A/C Number */
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

            if (!pKeyMsg->SEL.INPUT.uchLen) {                                   /* W/o Amount */
                return(LDT_SEQERR_ADR);
            }
        
            /* Check Input Data */

            if (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_ONLY_PRT) {        /* Only Print Type */

                /* Clear Lead Through */

                UifACPGMLDClear();

                MaintDisp(AC_SERVICE_RESET_RPT,             /* A/C Number */
                          0,                                /* Descriptor */
                          0,                                /* Page Number */
                          uchUifACDataType,                 /* PTD Type */
                          0,                                /* Report Type */
                          ( UCHAR)RPT_TYPE_ONLY_PRT,        /* Reset Type */
                          0L,                               /* Amount Data */
                          0);                               /* "Request Function Type Entry" Lead Through Address */

                if (uchUifACDataType == ( UCHAR)RPT_TYPE_DALY) {        /* Daily File Type */
                    RptServiceDayRead(CLASS_TTLSAVDAY,
                                    RPT_ALL_READ);
                } else {
                    RptServicePTDRead(CLASS_TTLSAVPTD,
                                    RPT_ALL_READ);
                }
                UieExit(aACEnter);                                      /* Return to UifACEnter() */
                return(SUCCESS);
            } else if((pKeyMsg->SEL.INPUT.lData == RPT_TYPE_RST_PRT) || /* Reset and Print Type */
                (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_ONLY_RST)) {      /* Only Reset Type */
                if ((sError = RptServiceLock()) == SUCCESS) {
                    uchUifACRstType = ( UCHAR)pKeyMsg->SEL.INPUT.lData; /* Save Reset Type */
                    UieNextMenu(aChildAC132EnterRstType);                /* Open Next Function */
                } else {
                    UieExit(aACEnter);                                  /* Return to UifACEnter() */
                }
                return(sError);
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
**  Synopsis: SHORT UifAC132EnterKey(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.132 Reset Confirm Mode 
*===============================================================================
*/
    


SHORT UifAC132EnterKey(KEYMSG *pKeyMsg) 
{
    
                            
    SHORT   sError; 


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC3EnterKey);                             /* Register Key Sequence */

        UifACPGMLDClear();

        MaintDisp(AC_SERVICE_RESET_RPT,             /* A/C Number */
                  CLASS_MAINTDSPCTL_10N,            /* 10N10D Display */
                  0,                                /* Page Number */
                  uchUifACDataType,                 /* PTD Type */
                  0,                                /* Report Type */
                  uchUifACRstType,                  /* Reset Type */
                  0L,                               /* Amount Data */
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

            MaintDisp(AC_SERVICE_RESET_RPT,             /* A/C Number */
                      0,                                /* Descriptor */
                      0,                                /* Page Number */
                      uchUifACDataType,                 /* PTD Type */
                      0,                                /* Report Type */
                      uchUifACRstType,                  /* Reset Type */
                      0L,                               /* Amount Data */
                      0);                               /* Disable Lead Through */

            if (uchUifACDataType == ( UCHAR)RPT_TYPE_DALY) {                /* Daily File Type */
                if (uchUifACRstType == ( UCHAR)RPT_TYPE_RST_PRT) {          /* Reset and Print Type  */
                    if ((sError = RptServiceDayReset(RPT_PRT_RESET)) == SUCCESS) {          
                        RptServiceDayRead(CLASS_TTLSAVDAY,
                                        RPT_ALL_READ);
                    }
                } else {
                    sError = RptServiceDayReset(RPT_ONLY_RESET);          
                }
            } else {
                if (uchUifACRstType == ( UCHAR)RPT_TYPE_RST_PRT) {          /* Reset and Print Type  */
                    if ((sError = RptServicePTDReset(RPT_PRT_RESET)) == SUCCESS) {          
                        RptServicePTDRead(CLASS_TTLSAVPTD,
                                        RPT_ALL_READ);
                    }
                } else {
                    sError = RptServicePTDReset(RPT_ONLY_RESET);          
                }
            }
            RptServiceUnLock();
            UieExit(aACEnter);                                              /* Return to UifACEnter() */
            return(sError);

        case FSC_CANCEL:
            RptServiceUnLock();
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
