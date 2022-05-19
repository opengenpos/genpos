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
* Title       : Emergent Cashier File Close Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC119.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC119Function()      : A/C No.119 Function Entry Mode 
*               UifAC119EnterData()     : A/C No.119 Enter Data Mode  
*               UifAC119ExitFunction()  : A/C No.119 Exit Function Mode  
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
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

#include "uifsupex.h"

/*
*=============================================================================
**  Synopsis:    SHORT UifAC119Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.119 Function Entry Mode   
*===============================================================================
*/

/* Define Next Function */

UIMENU FARCONST aChildAC119Init[] = {{UifAC119EnterData, CID_AC119ENTERDATA},
                                     {NULL,              0                 }};

    
SHORT UifAC119Function(KEYMSG *pKeyMsg) 
{
    
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC119Init);               /* Open Next Function */

        MaintDisp(AC_EMG_CASHFL_CLS,                /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  0,                                /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_NUM_ADR);                     /* " Number Entry " Lead Through Address */
                  
        return(SUCCESS);

    default:
        return(UifACDefProc(pKeyMsg));              /* Execute Default Procedure */
    }
}

/*
*==============================================================================
**  Synopsis: SHORT UifAC119EnterData(KEYMSG *pKeyMsg) 
* 
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*       Output: nothing
*
**      Return: SUCCESS         : Successful     
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: AC No.119 Enter Data Mode  
*===============================================================================
*/


/* Define Next Function */

UIMENU FARCONST aChildAC119EnterData[] = {{UifAC119ExitFunction, CID_AC119EXITFUNCTION},
                                          {NULL,                 0                    }};


SHORT UifAC119EnterData(KEYMSG *pKeyMsg) 
{
    SHORT   sError;

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {                          
        case FSC_AC:
            /* W/ Amount Check */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                   /* W/o Amount */
                return(LDT_SEQERR_ADR);
            }

            /* Check 0 Data and Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIFREG_MAX_ID_DIGITS) {       
                return(LDT_KEYOVER_ADR);
            }

            /* Check Cashier No. */
			if (pKeyMsg->SEL.INPUT.lData == 0) {
				sError = SUCCESS;
                ulUifACRptCashierNo = (ULONG)pKeyMsg->SEL.INPUT.lData;                   /* Save Cashier Number */                        
                UieNextMenu(aChildAC119EnterData);
			} else if ((sError = RptChkCashierNo(pKeyMsg->SEL.INPUT.lData)) == SUCCESS) {
                /* Save Cashier No. */
                ulUifACRptCashierNo = (ULONG)pKeyMsg->SEL.INPUT.lData;                   /* Save Cashier Number */                        
                UieNextMenu(aChildAC119EnterData);
            } else {
                UieExit(aACEnter);                                                  
            }
            return(sError);    

        /* default:
              break; */
    }

    default:
        return(UifACDefProc(pKeyMsg));                      /* Execute Default Procedure */
    }
}

/*
*==============================================================================
**  Synopsis: SHORT UifAC119ExitFunction(KEYMSG *pKeyMsg) 
* 
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*       Output: nothing
*
**      Return: SUCCESS         : Successful     
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: AC No.119 Exit Function Mode  
*===============================================================================
*/
SHORT UifAC119ExitFunction(KEYMSG *pKeyMsg) 
{
    SHORT   sError;

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);        /* Register Key Sequence */
                                                
        MaintDisp(AC_EMG_CASHFL_CLS,            /* A/C Number */
                  CLASS_MAINTDSPCTL_10N,        /* 10N10D Display */
                  0,                            /* Page Number */
                  0,                            /* PTD Type */
                  0,                            /* Report Type */
                  0,                            /* Reset Type */
                  ulUifACRptCashierNo,          /* Cashier Number */
                  LDT_RESET_ADR);               /* " Lead Through for Reset Report " Lead Through Address */
        
        return(SUCCESS);

    case UIM_INPUT:
        /* W/ Amount Check */
        if (pKeyMsg->SEL.INPUT.uchLen) {                    /* W/ Amount */
            return(LDT_SEQERR_ADR);
        }

        switch (pKeyMsg->SEL.INPUT.uchMajor) {                          
        case FSC_AC:
			if (ulUifACRptCashierNo)
				sError = RptCashierReset(0, RPT_OPEN_RESET, 0, ulUifACRptCashierNo);  /* Execute Close File Function */
			else
				sError = RptCashierReset(0, RPT_ALL_RESET, 0, ulUifACRptCashierNo);  /* Execute Close File Function */
            UieExit(aACEnter);                                                
            return(sError);    

        case FSC_CANCEL:
            UieExit(aACEnter);                                                
            return(SUCCESS);

        /* default:
              break; */
    }

    default:
        return(UifACDefProc(pKeyMsg));                      /* Execute Default Procedure */
    }
}



    
    
    
