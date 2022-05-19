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
* Title       : Slip Printer Feed Control Module                         
* Category    : User Interface For Program, NCR 2170 US Hospitality Application
* Program Name: UIFPG18.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifPG18Function()   : Program No.18 Entry Mode
*               UifPG18IssueRpt()   : Program No.18 Issue Report 
*               UifPG18EnterAddr()  : Program No.18 Enter Address Mode 
*               UifPG18EnterData()  : Program No.18 Enter Data Mode 
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jun-19-92:00.00.01    :K.You      : initial                                   
* Jun-27-92:01.00.12    :K.You      : add EJ feature                                   
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
**  Synopsis: SHORT UifPG18Function(KEYMSG *pKeyMsg) 
*               
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               
**  Description: Program No.18 Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildPG18Init[] = {{UifPG18IssueRpt, CID_PG18ISSUERPT},
                                    {UifPG18EnterAddr, CID_PG18ENTERADDR},
                                    {UifPG18EnterData, CID_PG18ENTERDATA},
                                    {NULL,             0                }};

/* Define Next Function at UIM_ACCEPTED */

UIMENU FARCONST aChildPG18Accept[] = {{UifPG18EnterAddr, CID_PG18ENTERADDR},
                                      {UifPG18EnterData, CID_PG18ENTERDATA},
                                      {NULL,             0                }};


SHORT UifPG18Function(KEYMSG *pKeyMsg) 
{
    
    PARASLIPFEEDCTL   SlipCtlData;


    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildPG18Init);                                    /* Open Next Function */

        /* Display Address 1 of This Function  */

        SlipCtlData.uchStatus = 0;                                      /* Set W/ Amount Status */
        SlipCtlData.uchAddress = 1;                                     /* Set Address 1 */
        SlipCtlData.uchMajorClass = CLASS_PARASLIPFEEDCTL;              /* Set Major Class */

        MaintSlipFeedCtlRead(&SlipCtlData);                             /* Execute Read Slip Feed Control Procedure */
        break;

    case UIM_ACCEPTED:
        UieNextMenu(aChildPG18Accept);                                  /* Open Next Function */
        break;

    default:
        return(UifPGDefProc(pKeyMsg));                                  /* execute Default Procedure */
    }
    return(SUCCESS);
}


/*
*=============================================================================
**  Synopsis: SHORT UifPG18IssueRpt(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: Program No.18 Issue Report 
*===============================================================================
*/
    


SHORT UifPG18IssueRpt(KEYMSG *pKeyMsg) 
{
    
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P3);                /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        /* Check Digit */

        if (pKeyMsg->SEL.INPUT.uchLen) {                /* W/ Amount Case */
            return(LDT_SEQERR_ADR);
        }
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P3:
            MaintSlipFeedCtlReport();                   /* Execute Report Procedure */
            UieExit(aPGEnter);                          /* Return to UifPGEnter() */
            return(SUCCESS);
        
        /* default:
            break; */

        }
        /* break; */

    default:
        return(UifPGDefProc(pKeyMsg));                  /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifPG18EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.18 Enter Address Mode 
*===============================================================================
*/
    


SHORT UifPG18EnterAddr(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARASLIPFEEDCTL SlipCtlData;   
                    


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P2);                        /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P2:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {       /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            SlipCtlData.uchStatus = 0;                          /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                   /* W/o Amount Input Case */
                SlipCtlData.uchStatus |= MAINT_WITHOUT_DATA;    /* Set W/o Amount Status */
            } else {                                            /* W/ Amount Input Case */
                SlipCtlData.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Read Slip Feed Control Procedure */
            
            SlipCtlData.uchMajorClass = CLASS_PARASLIPFEEDCTL;  /* Set Major Class */

            if ((sError = MaintSlipFeedCtlRead(&SlipCtlData)) == SUCCESS) {    
                UieAccept();                                    /* Return to UifPG18Function() */
            }
            return(sError);                                     /* Set Success or Input Data Error */
            
        /* default:
              break;  */
        }
        /* break; */

    default:
        return(UifPGDefProc(pKeyMsg));                          /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifPG18EnterData(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.18 Enter Data Mode  
*===============================================================================
*/
    

SHORT UifPG18EnterData(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARASLIPFEEDCTL SlipCtlData;   
                    

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P1);                        /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P1:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT3) {       /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            SlipCtlData.uchStatus = 0;                          /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                   /* W/o Amount Input Case */
                SlipCtlData.uchStatus |= MAINT_WITHOUT_DATA;    /* Set W/o Amount Status */
            } else {                                            /* W/ Amount Input Case */
                SlipCtlData.uchFeedCtl = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Write Slip Feed Control Procedure */
            
            /* SlipCtlData.uchMajorClass = CLASS_PARASLIPFEEDCTL;   Set Major Class */

            if ((sError = MaintSlipFeedCtlWrite(&SlipCtlData)) == SUCCESS) {    
                UieAccept();                                    /* Return to UifPG18Function() */
            }
            return(sError);                                     /* Set Success or Input Data Error */
            
        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifPGDefProc(pKeyMsg));                          /* Execute Default Procedure */
    }
}

/****** End of Source ******/  
    
    
