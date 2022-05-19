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
* Title       : Supervisor Number/Level Assignment Module                         
* Category    : User Interface For Program, NCR 2170 US Hospitality Application
* Program Name: UIFPG8.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifPG8Function()   : Program No.8 Entry Mode
*               UifPG8IssueRpt()   : Program No.8 Issue Report 
*               UifPG8EnterAddr()  : Program No.8 Enter Address Mode 
*               UifPG8EnterData()  : Program No.8 Enter Data Mode 
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
**  Synopsis: SHORT UifPG8Function(KEYMSG *pKeyMsg) 
*               
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               
**  Description: Program No.8 Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildPG8Init[] = {{UifPG8IssueRpt, CID_PG8ISSUERPT},
                                   {UifPG8EnterAddr, CID_PG8ENTERADDR},
                                   {UifPG8EnterData, CID_PG8ENTERDATA},
                                   {NULL,            0               }};

/* Define Next Function at UIM_ACCEPTED */

UIMENU FARCONST aChildPG8Accept[] = {{UifPG8EnterAddr, CID_PG8ENTERADDR},
                                     {UifPG8EnterData, CID_PG8ENTERDATA},
                                     {NULL,            0             }};


SHORT UifPG8Function(KEYMSG *pKeyMsg) 
{
    
    PARASUPLEVEL   SupLevelData;   

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildPG8Init);                                     /* Open Next Function */

        /* Display Address 1 of This Function */

        SupLevelData.uchStatus = 0;                                     /* Set W/ Amount Status */
        SupLevelData.uchAddress = 1;                                    /* Set Address 1 */
        SupLevelData.uchMajorClass = CLASS_PARASUPLEVEL;                /* Set Major Class */

        MaintSupLevelRead(&SupLevelData);                               /* Execute Read Supervisor No. Procedure */
        break;

    case UIM_ACCEPTED:
        UieNextMenu(aChildPG8Accept);                                   /* Open Next Function */
        break;

    default:
        return(UifPGDefProc(pKeyMsg));                                  /* execute Default Procedure */
    }
    return(SUCCESS);
}


/*
*=============================================================================
**  Synopsis: SHORT UifPG8IssueRpt(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: Program No.8 Issue Report  
*===============================================================================
*/
    


SHORT UifPG8IssueRpt(KEYMSG *pKeyMsg) 
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
            MaintSupLevelReport();                  /* Execute Report Procedure */
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
**  Synopsis: SHORT UifPG8EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS             : Successful
*               LDT_KEYOVER_ADDR    : Wrong Data Error
*               
**  Description: Program No.8 Enter Address Mode 
*===============================================================================
*/
    


SHORT UifPG8EnterAddr(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARASUPLEVEL    SupLevelData;   
                         


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P2);                /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P2:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {       /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            SupLevelData.uchStatus = 0;                         /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                   /* W/o Amount Input Case */
                SupLevelData.uchStatus |= MAINT_WITHOUT_DATA;   /* Set W/o Amount Status */
            } else {                                            /* W/ Amount Input Case */
                SupLevelData.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Read Supervisor No. Procedure */
            
            SupLevelData.uchMajorClass = CLASS_PARASUPLEVEL;    /* Set Major Class */

            if ((sError = MaintSupLevelRead(&SupLevelData)) == SUCCESS) {    
                UieAccept();                                    /* Return to UifPG8Function() */
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
**  Synopsis: SHORT UifPG8EnterData(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.8 Enter Data Mode  
*===============================================================================
*/
    

SHORT UifPG8EnterData(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARASUPLEVEL    SupLevelData;   
                    

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
            SupLevelData.uchStatus = 0;                         /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                   /* W/o Amount Input Case */
                SupLevelData.uchStatus |= MAINT_WITHOUT_DATA;   /* Set W/o Amount Status */
            } else {                                            /* W/ Amount Input Case */
                SupLevelData.usSuperNumber = ( USHORT)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Write Supervisor No. Procedure */
            
            /* SupLevelData.uchMajorClass = CLASS_PARASUPLEVEL;    Set Major Class */

            if ((sError = MaintSupLevelWrite(&SupLevelData)) == SUCCESS) {    
                UieAccept();                                    /* Return to UifPG8Function() */
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


    
    
    
