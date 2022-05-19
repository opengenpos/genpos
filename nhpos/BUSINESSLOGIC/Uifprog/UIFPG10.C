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
* Title       : Transaction HALO Module                         
* Category    : User Interface For Program, NCR 2170 US Hospitality Application
* Program Name: UIFPG10.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifPG10Function()   : Program No.10 Entry Mode
*               UifPG10IssueRpt()   : Program No.10 Issue Report 
*               UifPG10EnterAddr()  : Program No.10 Enter Address Mode 
*               UifPG10EnterData()  : Program No.10 Enter Data Mode 
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
**  Synopsis: SHORT UifPG10Function(KEYMSG *pKeyMsg) 
*               
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               
**  Description: Program No.10 Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildPG10Init[] = {{UifPG10IssueRpt, CID_PG10ISSUERPT},
                                    {UifPG10EnterAddr, CID_PG10ENTERADDR},
                                    {UifPG10EnterData, CID_PG10ENTERDATA},
                                    {NULL,             0                }};

/* Define Next Function at UIM_ACCEPTED */

UIMENU FARCONST aChildPG10Accept[] = {{UifPG10EnterAddr, CID_PG10ENTERADDR},
                                      {UifPG10EnterData, CID_PG10ENTERDATA},
                                      {NULL,             0                }};


SHORT UifPG10Function(KEYMSG *pKeyMsg) 
{
    
    PARATRANSHALO   HALOData;   

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildPG10Init);                                /* Open Next Function */

        /* Display Address 1 of This Function */

        HALOData.uchStatus = 0;                                     /* Set W/ Amount Status */
        HALOData.uchAddress = 1;                                    /* Set Address 1 */
        HALOData.uchMajorClass = CLASS_PARATRANSHALO;               /* Set Major Class */

        MaintTransHALORead(&HALOData);                              /* Execute Read HALO Procedure */
        break;

    case UIM_ACCEPTED:
        UieNextMenu(aChildPG10Accept);                              /* Open Next Function */
        break;

    default:
        return(UifPGDefProc(pKeyMsg));                              /* execute Default Procedure */
    }
    return(SUCCESS);
}


/*
*=============================================================================
**  Synopsis: SHORT UifPG10IssueRpt(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: Program No.10 Issue Report 
*===============================================================================
*/
    


SHORT UifPG10IssueRpt(KEYMSG *pKeyMsg) 
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
            MaintTransHALOReport();                 /* Execute Report Procedure */
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
**  Synopsis: SHORT UifPG10EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.10 Enter Address Mode 
*===============================================================================
*/
    


SHORT UifPG10EnterAddr(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARATRANSHALO   HALOData;   
                    


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P2);                /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P2:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            HALOData.uchStatus = 0;                         /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {               /* W/o Amount Input Case */
                HALOData.uchStatus |= MAINT_WITHOUT_DATA;   /* Set W/o Amount Status */
            } else {                                        /* W/ Amount Input Case */
                HALOData.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Read HALO Procedure */
            
            HALOData.uchMajorClass = CLASS_PARATRANSHALO;   /* Set Major Class */

            if ((sError = MaintTransHALORead(&HALOData)) == SUCCESS) {    
                UieAccept();                                /* Return to UifPG10Function() */
            }
            return(sError);                                 /* Set Success or Input Data Error */
            
        /* default:
              break;  */
        }
        /* break; */

    default:
        return(UifPGDefProc(pKeyMsg));                      /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifPG10EnterData(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.10 Enter Data Mode  
*===============================================================================
*/
    

SHORT UifPG10EnterData(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARATRANSHALO   HALOData;   
                    

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P1);                /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P1:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            HALOData.uchStatus = 0;                         /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {               /* W/o Amount Input Case */
                HALOData.uchStatus |= MAINT_WITHOUT_DATA;   /* Set W/o Amount Status */
            } else {                                        /* W/ Amount Input Case */
                HALOData.uchHALO = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Write HALO Procedure */
            
            HALOData.uchMajorClass = CLASS_PARATRANSHALO;   /* Set Major Class */

            if ((sError = MaintTransHALOWrite(&HALOData)) == SUCCESS) {    
                UieAccept();                                /* Return to UifPG10Function() */
                return(SUCCESS);
            } else {
                return(sError);                             /* Set Input Data Error */
            }
            
        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifPGDefProc(pKeyMsg));                      /* Execute Default Procedure */
    }
}


    
    
    
