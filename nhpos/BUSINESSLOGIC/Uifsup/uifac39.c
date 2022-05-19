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
* Title       : PLU All Reset Report Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC39.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs1 /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC39Function()       : A/C No.39 Function Entry Mode
*               UifAC39EnterDataType()  : A/C No.39 Enter Data Type Mode
*               UifAC39EnterRstType()   : A/C No.39 Enter Reset Type Mode
*               UifAC39EnterKey()       : A/C No.39 Reset Confirm Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jul-09-92:00.00.01    :K.You      : initial                                   
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

UIMENU FARCONST aChildAC39EnterDataType[] = {{UifAC39EnterRstType, CID_AC39ENTERRSTTYPE},
                                             {NULL,                0                   }};


/*
*=============================================================================
**  Synopsis: SHORT UifAC39Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.39 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC39Init[] = {{UifAC39EnterDataType, CID_AC39ENTERDATATYPE},
                                    {NULL,                 0                    }};



SHORT UifAC39Function(KEYMSG *pKeyMsg) 
{



    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        /* Check PTD Exist or Not */

        if (!RptPTDChkFileExist(FLEX_PLU_ADR)) {                /* Not PTD Case */
            uchUifACDataType = ( UCHAR)RPT_TYPE_DALY;           /* Set Data Type to Own Save Area */
            UieNextMenu(aChildAC39EnterDataType);
        } else {
            UieNextMenu(aChildAC39Init);                            /* Open Next Function */
        }
        return(SUCCESS);                                        

    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC39EnterDataType(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.39 Enter Data Type Mode 
*===============================================================================
*/
    

SHORT UifAC39EnterDataType(KEYMSG *pKeyMsg) 
{
    
                    

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC23EnterDataType);   /* Register Key Sequence */

        UifACPGMLDDispCom(DaylyPTDSelect);

        MaintDisp(AC_PLU_RESET_RPT,                 /* A/C Number */
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
            
            UieNextMenu(aChildAC39EnterDataType);
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
**  Synopsis: SHORT UifAC39EnterRstType(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.39 Enter Reset Type Mode 
*===============================================================================
*/
    
/* Define Next Function */

UIMENU FARCONST aChildAC39EnterRstType[] = {{UifAC39EnterKey, CID_AC39ENTERKEY},
                                            {NULL,            0               }};




SHORT UifAC39EnterRstType(KEYMSG *pKeyMsg) 
{
    TCHAR   auchPLUNumber[PLU_MAX_DIGIT+1+1];
    SHORT   sError;

    memset(auchPLUNumber, 0, sizeof(auchPLUNumber));

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC23EnterDataType);   /* Register Key Sequence */

        UifACPGMLDDispCom(ResetTypeSelect);

        MaintDisp(AC_PLU_RESET_RPT,                 /* A/C Number */
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

                MaintDisp(AC_PLU_RESET_RPT,                 /* A/C Number */
                          0,                                /* Descriptor */
                          0,                                /* Page Number */
                          uchUifACDataType,                 /* PTD Type */
                          0,                                /* Report Type */
                          ( UCHAR)RPT_TYPE_ONLY_PRT,        /* Reset Type */
                          0L,                               /* Amount Data */
                          0);                               /* "Request Function Type Entry" Lead Through Address */

                if (uchUifACDataType == ( UCHAR)RPT_TYPE_DALY) {        /* Daily File Type */
                    RptPLUDayRead(CLASS_TTLSAVDAY, RPT_ALL_READ, 0, RPT_ONLY_PRT_RESET, auchPLUNumber);
                } else {
                    RptPLUPTDRead(CLASS_TTLSAVPTD, RPT_ALL_READ, 0, RPT_ONLY_PRT_RESET, auchPLUNumber);
                }
                UieExit(aACEnter);                                      /* Return to UifACEnter() */
                return(SUCCESS);

            } else if((pKeyMsg->SEL.INPUT.lData == RPT_TYPE_RST_PRT) || /* Reset and Print Type */
                (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_ONLY_RST)      || /* Only Reset Type */
                (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_OPTIMIZE)) {      /* optimize database */
/*                (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_DELETE)) {        / detete database */
                
/*            } else if((pKeyMsg->SEL.INPUT.lData == RPT_TYPE_RST_PRT) || / Reset and Print Type */
/*                (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_ONLY_RST)) {      / Only Reset Type */
                if ((sError = RptPLULock()) == SUCCESS) {
                    uchUifACRstType = ( UCHAR)pKeyMsg->SEL.INPUT.lData; /* Save Reset Type */
                    UieNextMenu(aChildAC39EnterRstType);                /* Open Next Function */
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
**  Synopsis: SHORT UifAC39EnterKey(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.39 Reset Confirm Mode 
*===============================================================================
*/
    


SHORT UifAC39EnterKey(KEYMSG *pKeyMsg) 
{
    TCHAR   auchPLUNumber[PLU_MAX_DIGIT+1+1];
    SHORT   sError;

    memset(auchPLUNumber, 0, sizeof(auchPLUNumber));

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC3EnterKey);                             /* Register Key Sequence */

        UifACPGMLDClear();

        MaintDisp(AC_PLU_RESET_RPT,                 /* A/C Number */
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

            MaintDisp(AC_PLU_RESET_RPT,                 /* A/C Number */
                      0,                                /* Descriptor */
                      0,                                /* Page Number */
                      uchUifACDataType,                 /* PTD Type */
                      0,                                /* Report Type */
                      uchUifACRstType,                  /* Reset Type */
                      0L,                               /* Amount Data */
                      0);                               /* Disable Lead Through */

            if (uchUifACDataType == ( UCHAR)RPT_TYPE_DALY) {                /* Daily File Type */
                if (uchUifACRstType == ( UCHAR)RPT_TYPE_RST_PRT) {          /* Reset and Print Type  */
                    if ((sError = RptPLUDayReset(RPT_PRT_RESET)) == SUCCESS) {          
                        RptPLUDayRead(CLASS_TTLSAVDAY, RPT_ALL_READ, 0, RPT_PRT_RESET, auchPLUNumber);
                    }
				/***/
                } else
                if (uchUifACRstType == ( UCHAR)RPT_TYPE_DELETE) {          /* Reset and Print Type  */
                    sError = RptPLUDayReset(RPT_DELETE);
                } else
                if (uchUifACRstType == ( UCHAR)RPT_TYPE_OPTIMIZE) {          /* Reset and Print Type  */
                    sError = RptPLUDayOptimize(RPT_TYPE_OPTIMIZE);
                /***/
                } else {
                    sError = RptPLUDayReset(RPT_ONLY_RESET);
                }
            } else {
                if (uchUifACRstType == ( UCHAR)RPT_TYPE_RST_PRT) {          /* Reset and Print Type  */
                    if ((sError = RptPLUPTDReset(RPT_PRT_RESET)) == SUCCESS) {          
                        RptPLUPTDRead(CLASS_TTLSAVPTD, RPT_ALL_READ, 0, RPT_PRT_RESET, auchPLUNumber);
                    }
				/***/
                } else
                if (uchUifACRstType == ( UCHAR)RPT_TYPE_DELETE) {          /* Reset and Print Type  */
                    sError = RptPLUPTDReset(RPT_DELETE);
                } else
                if (uchUifACRstType == ( UCHAR)RPT_TYPE_OPTIMIZE) {          /* Reset and Print Type  */
                    sError = RptPLUPTDOptimize(RPT_TYPE_OPTIMIZE);
                /***/
                } else {
                    sError = RptPLUPTDReset(RPT_ONLY_RESET);          
                }
            }
            RptPLUUnLock();
            UieExit(aACEnter);                                              /* Return to UifACEnter() */
            return(sError);

        case FSC_CANCEL:
            RptPLUUnLock();
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

/* --- End of Source File --- */