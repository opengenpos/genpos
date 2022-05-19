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
* Title       : Security Number for Program Mode Module                         
* Category    : User Interface For Program, NCR 2170 US Hospitality Application
* Program Name: UIFPG6.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifPG6Function()   : Program No.6 Entry Mode
*               UifPG6IssueRpt()   : Program No.6 Issue Report 
*               UifPG6EnterAddr()  : Program No.6 Enter Address Mode 
*               UifPG6EnterData()  : Program No.6 Enter Data Mode 
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
**  Synopsis: SHORT UifPG6Function(KEYMSG *pKeyMsg) 
*               
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               
**  Description: Program No.6 Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildPG6Init[] = {{UifPG6IssueRpt, CID_PG6ISSUERPT},
                                   {UifPG6EnterAddr, CID_PG6ENTERADDR},
                                   {UifPG6EnterData, CID_PG6ENTERDATA},
                                   {NULL,            0               }};

/* Define Next Function at UIM_ACCEPTED */

UIMENU FARCONST aChildPG6Accept[] = {{UifPG6EnterAddr, CID_PG6ENTERADDR},
                                     {UifPG6EnterData, CID_PG6ENTERDATA},
                                     {NULL,            0               }};


SHORT UifPG6Function(KEYMSG *pKeyMsg) 
{
    
    PARASECURITYNO   SecurityData;   

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildPG6Init);                                 /* Open Next Function */

        /* Display Address 1 of This Function */

        SecurityData.uchStatus = 0;                                 /* Set W/ Amount Status */
        SecurityData.uchAddress = 1;                                /* Set Address 1 */
        SecurityData.uchMajorClass = CLASS_PARASECURITYNO;          /* Set Major Class */

        MaintSecurityNoRead(&SecurityData);                         /* Execute Read Security No Procedure */
        break;

    case UIM_ACCEPTED:
        UieNextMenu(aChildPG6Accept);                               /* Open Next Function */
        break;

    default:
        return(UifPGDefProc(pKeyMsg));                              /* execute Default Procedure */
    }
    return(SUCCESS);
}


/*
*=============================================================================
**  Synopsis: SHORT UifPG6IssueRpt(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: Program No.6 Issue Report  
*===============================================================================
*/
    


SHORT UifPG6IssueRpt(KEYMSG *pKeyMsg) 
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
            MaintSecurityNoReport();                /* Execute Report Procedure */
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
**  Synopsis: SHORT UifPG6EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful                 
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.6 Enter Address Mode 
*===============================================================================
*/
    


SHORT UifPG6EnterAddr(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARASECURITYNO   SecurityData;   
                    


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
            SecurityData.uchStatus = 0;                         /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                   /* W/o Amount Input Case */
                SecurityData.uchStatus |= MAINT_WITHOUT_DATA;   /* Set W/o Amount Status */
            } else {                                            /* W/ Amount Input Case */
                SecurityData.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Read Security No Procedure */
            
            SecurityData.uchMajorClass = CLASS_PARASECURITYNO;  /* Set Major Class */

            if ((sError = MaintSecurityNoRead(&SecurityData)) == SUCCESS) {    
                UieAccept();                                    /* Return to UifPG6Function() */
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
**  Synopsis: SHORT UifPG6EnterData(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.6 Enter Data Mode 
*===============================================================================
*/
    

SHORT UifPG6EnterData(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARASECURITYNO  SecurityData;   
                       

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P1);                /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P1:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT4) {       /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            SecurityData.uchStatus = 0;                         /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                   /* W/o Amount Input Case */
                SecurityData.uchStatus |= MAINT_WITHOUT_DATA;   /* Set W/o Amount Status */
            } else {                                            /* W/ Amount Input Case */
                SecurityData.usSecurityNumber = ( USHORT)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Write Security No Procedure */
            
            /* SecurityData.uchMajorClass = CLASS_PARASECURITYNO;   Set Major Class */

            if ((sError = MaintSecurityNoWrite(&SecurityData)) == SUCCESS) {    
                UieAccept();                                    /* Return to UifPG6Function() */
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


    
    
    
