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
* Title       : Action Code Security Module                         
* Category    : User Interface For Program, NCR 2170 US Hospitality Application
* Program Name: UIFPG9.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifPG9Function()   : Program No.9 Entry Mode
*               UifPG9IssueRpt()   : Program No.9 Issue Report 
*               UifPG9EnterAddr()  : Program No.9 Enter Address Mode 
*               UifPG9EnterData()  : Program No.9 Enter Data Mode 
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
**  Synopsis: SHORT UifPG9Function(KEYMSG *pKeyMsg) 
*               
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               
**  Description: Program No.9 Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildPG9Init[] = {{UifPG9IssueRpt, CID_PG9ISSUERPT},
                                   {UifPG9EnterAddr, CID_PG9ENTERADDR},
                                   {UifPG9EnterData, CID_PG9ENTERDATA},
                                   {NULL,            0               }};

/* Define Next Function at UIM_ACCEPTED */

UIMENU FARCONST aChildPG9Accept[] = {{UifPG9EnterAddr, CID_PG9ENTERADDR},
                                     {UifPG9EnterData, CID_PG9ENTERDATA},
                                     {NULL,            0               }};


SHORT UifPG9Function(KEYMSG *pKeyMsg) 
{
    
    PARAACTCODESEC   ActCodeSecData;   

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildPG9Init);                                 /* Open Next Function */

        /* Display Address 1 of This Function */

        ActCodeSecData.uchStatus = 0;                               /* Set W/ Amount Status */
        ActCodeSecData.usAddress = 1;                               /* Set Address 1 */
        ActCodeSecData.uchMajorClass = CLASS_PARAACTCODESEC;        /* Set Major Class */

        MaintActCodeSecRead(&ActCodeSecData);                       /* Execute Read Action Code Security Procedure */
        break;

    case UIM_ACCEPTED:
        UieNextMenu(aChildPG9Accept);                               /* Open Next Function */
        break;

    default:
        return(UifPGDefProc(pKeyMsg));                              /* execute Default Procedure */
    }                                                               
    return(SUCCESS);
}


/*
*=============================================================================
**  Synopsis: SHORT UifPG9IssueRpt(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: Program No.9 Issue Report 
*===============================================================================
*/
    


SHORT UifPG9IssueRpt(KEYMSG *pKeyMsg) 
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
            MaintActCodeSecReport();                /* Execute Report Procedure */
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
**  Synopsis: SHORT UifPG9EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.9 Enter Address Mode 
*===============================================================================
*/
    


SHORT UifPG9EnterAddr(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARAACTCODESEC  ActCodeSecData;   
                       


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P2);                        /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P2:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT3) {           /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            ActCodeSecData.uchStatus = 0;                           /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount Input Case */
                ActCodeSecData.uchStatus |= MAINT_WITHOUT_DATA;     /* Set W/o Amount Status */
            } else {                                                /* W/ Amount Input Case */
                ActCodeSecData.usAddress = ( USHORT)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Read Action Code Security Procedure */
            
            ActCodeSecData.uchMajorClass = CLASS_PARAACTCODESEC;    /* Set Major Class */

            if ((sError = MaintActCodeSecRead(&ActCodeSecData)) == SUCCESS) {    
                UieAccept();                                        /* Return to UifPG9Function() */
            }
            return(sError);                                         /* Set Input Data Error */
            
        /* default:
              break;  */
        }
        /* break; */

    default:
        return(UifPGDefProc(pKeyMsg));                              /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifPG9EnterData(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADDR : Sequence Error
*               
**  Description: Program No.9 Enter Data Mode  
*===============================================================================
*/
    

SHORT UifPG9EnterData(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARAACTCODESEC  ActCodeSecData;   
                       

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P1);                /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P1:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT4) {           /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            ActCodeSecData.uchStatus = 0;                           /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount Input Case */
                ActCodeSecData.uchStatus |= MAINT_WITHOUT_DATA;     /* Set W/o Amount Status */
            } else {                                                /* W/ Amount Input Case */
                memset(ActCodeSecData.aszMDCString,                 /* Full NULL to Own Buffer */
                       '\0',
                       sizeof(ActCodeSecData.aszMDCString));

                _tcsncpy(ActCodeSecData.aszMDCString, pKeyMsg->SEL.INPUT.aszKey, pKeyMsg->SEL.INPUT.uchLen); /* Copy Input Mnemonics to Own Buffer */
                //memcpy(ActCodeSecData.aszMDCString, pKeyMsg->SEL.INPUT.aszKey, pKeyMsg->SEL.INPUT.uchLen); /* Copy Input Mnemonics to Own Buffer */
            }

            /* Execute Write Action Code Security Procedure */
            
            /* ActCodeSecData.uchMajorClass = CLASS_PARAACTCODESEC;     Set Major Class */

            if ((sError = MaintActCodeSecWrite(&ActCodeSecData)) == SUCCESS) {    
                UieAccept();                                        /* Return to UifPG9Function() */
            }
            return(sError);                                         /* Set Success or Input Data Error */
            
        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifPGDefProc(pKeyMsg));                              /* Execute Default Procedure */
    }
}


    
    
    
