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
* Title       : MDC Module                         
* Category    : User Interface For Program, NCR 2170 US Hospitality Application
* Program Name: UIFPG1.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifPG1Function()   : Program No.1 Entry Mode
*               UifPG1IssueRpt()   : Program No.1 Issue Report 
*               UifPG1EnterAddr()  : Program No.1 Enter Address Mode 
*               UifPG1EnterData()  : Program No.1 Enter Data Mode 
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jun-19-92:00.00.01    :K.You      : initial                                   
*          :            :                                    
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
#include <uifprog.h>
#include <uifpgequ.h>

#include "uifpgex.h"                        /* Unique Extern Header for UI */

/*
*=============================================================================
**  Synopsis: SHORT UifPG1Function(KEYMSG *pKeyMsg) 
*               
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               
**  Description: Program No.1 Entry Mode 
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildPG1Init[] = {{UifPG1IssueRpt, CID_PG1ISSUERPT},
                                   {UifPG1EnterAddr,CID_PG1ENTERADDR},
                                   {UifPG1EnterData,CID_PG1ENTERDATA},
                                   {NULL,            0             }};

/* Define Next Function at UIM_ACCEPTED */

UIMENU FARCONST aChildPG1Accept[] = {{UifPG1EnterAddr,CID_PG1ENTERADDR},
                                     {UifPG1EnterData,CID_PG1ENTERDATA},
                                     {NULL,            0             }};


SHORT UifPG1Function(KEYMSG *pKeyMsg) 
{
    
    PARAMDC     MDCData;   
                
    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildPG1Init);                                 /* Open Next Function */
                                                                    
        /* Display Address 1 of This Function */

        MDCData.uchStatus = 0;                                      /* Set W/ Amount Status */
        MDCData.usAddress = 1;                                      /* Set Address 1 */
        MDCData.uchMajorClass = CLASS_PARAMDC;                      /* Set Major Class */

        MaintMDCRead(&MDCData);                                     /* Execute Read MDC Procedure */
        break;

    case UIM_ACCEPTED:
        UieNextMenu(aChildPG1Accept);                               /* Open Next Function */
        break;

    default:
        return(UifPGDefProc(pKeyMsg));                              /* execute Default Procedure */
    }
    return(SUCCESS);
}


/*
*=============================================================================
**  Synopsis: SHORT UifPG1IssueRpt(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Wrong Data Error
*               
**  Description: Program No.1 Issue Report 
*===============================================================================
*/
    


SHORT UifPG1IssueRpt(KEYMSG *pKeyMsg) 
{
    
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P3);            /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        /* Check Digit */

        if (pKeyMsg->SEL.INPUT.uchLen) {            /* W/ Amount Case */
            return(LDT_SEQERR_ADR);
        }
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P3:
            MaintMDCReport();                       /* Execute Report Procedure */
            UieExit(aPGEnter);                      /* Return to UifPGEnter() */
            return(SUCCESS);
        
        /* default:
            break; */

        }
        /* break; */

    default:
        return(UifPGDefProc(pKeyMsg));              /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifPG1EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.1 Enter Address Mode 
*===============================================================================
*/
    


SHORT UifPG1EnterAddr(KEYMSG *pKeyMsg) 
{
    
    SHORT       sError;
    PARAMDC     MDCData;   
                    


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P2);                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P2:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT3) {  /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            MDCData.uchStatus = 0;                          /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {               /* W/o Amount Input Case */
                MDCData.uchStatus |= MAINT_WITHOUT_DATA;    /* Set W/o Amount Status */
            } else {                                        /* W/ Amount Input Case */
                MDCData.usAddress = ( USHORT)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Read MDC Procedure */
            
            MDCData.uchMajorClass = CLASS_PARAMDC;          /* Set Major Class */

            if ((sError = MaintMDCRead(&MDCData)) == SUCCESS) {    
                UieAccept();                                /* Return to UifPG1Function() */
            }
            return(sError);                                 /* Set Input Data Error */
            
        /* default:
              break;  */
        }
        /* break; */

    default:
        return(UifPGDefProc(pKeyMsg));                  /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifPG1EnterData(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.1 Enter Data Mode 
*===============================================================================
*/
    

SHORT UifPG1EnterData(KEYMSG *pKeyMsg) 
{
    
    SHORT     sError;
    PARAMDC   MDCData;   
                    

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P1);                /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P1:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT4) {   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            MDCData.uchStatus = 0;                          /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {               /* W/o Amount Input Case */
                MDCData.uchStatus |= MAINT_WITHOUT_DATA;    /* Set W/o Amount Status */
            } else {                                        /* W/ Amount Input Case */
                memset(MDCData.aszMDCString,                /* Full NULL to Own Buffer */
                       '\0',
                       sizeof(MDCData.aszMDCString));

                _tcsncpy(MDCData.aszMDCString, pKeyMsg->SEL.INPUT.aszKey, pKeyMsg->SEL.INPUT.uchLen); /* Copy Input Mnemonics to Own Buffer */
                //memcpy(MDCData.aszMDCString, pKeyMsg->SEL.INPUT.aszKey, pKeyMsg->SEL.INPUT.uchLen); /* Copy Input Mnemonics to Own Buffer */
            }

            /* Execute Write MDC Procedure */
            
            MDCData.uchMajorClass = CLASS_PARAMDC;          /* Set Major Class */

            if ((sError = MaintMDCWrite(&MDCData)) == SUCCESS) {    
                UieAccept();                                /* Return to UifPG1Function() */
            }
            return(sError);                                 /* Set Input Data Error */
            
        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifPGDefProc(pKeyMsg));                      /* Execute Default Procedure */
    }
}


    
    
    
