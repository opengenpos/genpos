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
* Title       : Total Key Type Assignment Module                         
* Category    : User Interface For Program, NCR 2170 US Hospitality Application
* Program Name: UIFPG60.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifPG60Function()   : Program No.60 Entry Mode
*               UifPG60IssueRpt()   : Program No.60 Issue Report 
*               UifPG60EnterAddr()  : Program No.60 Enter Address Mode 
*               UifPG60EnterData()  : Program No.60 Enter Data Mode 
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
**  Synopsis: SHORT UifPG60Function(KEYMSG *pKeyMsg) 
*               
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               
**  Description: Program No.60 Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildPG60Init[] = {{UifPG60IssueRpt, CID_PG60ISSUERPT},
                                    {UifPG60EnterAddr, CID_PG60ENTERADDR},
                                    {UifPG60EnterData, CID_PG60ENTERDATA},
                                    {NULL,             0                }};

/* Define Next Function at UIM_ACCEPTED */

UIMENU FARCONST aChildPG60Accept[] = {{UifPG60EnterAddr, CID_PG60ENTERADDR},
                                      {UifPG60EnterData, CID_PG60ENTERDATA},
                                      {NULL,             0                }};


SHORT UifPG60Function(KEYMSG *pKeyMsg) 
{
    
    PARATTLKEYTYP   TtlKeyTypData;   

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildPG60Init);                                /* Open Next Function */
                                                                    
        /* Display Address 1 of This Function */

        TtlKeyTypData.uchStatus = 0;                                /* Set W/ Amount Status */
        TtlKeyTypData.uchAddress = 1;                               /* Set Address 1 */
        TtlKeyTypData.uchMajorClass = CLASS_PARATTLKEYTYP;          /* Set Major Class */

        MaintTtlKeyTypRead(&TtlKeyTypData);                         /* Execute Read Total Key Type Procedure */
        break;

    case UIM_ACCEPTED:
        UieNextMenu(aChildPG60Accept);                              /* Open Next Function */
        break;

    default:
        return(UifPGDefProc(pKeyMsg));                              /* execute Default Procedure */
    }
    return(SUCCESS);
}


/*
*=============================================================================
**  Synopsis: SHORT UifPG60IssueRpt(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: Program No.60 Issue Report 
*===============================================================================
*/
    


SHORT UifPG60IssueRpt(KEYMSG *pKeyMsg) 
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
            MaintTtlKeyTypReport();                 /* Execute Report Procedure */
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
**  Synopsis: SHORT UifPG60EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.60 Enter Address Mode 
*===============================================================================
*/
    


SHORT UifPG60EnterAddr(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARATTLKEYTYP   TtlKeyTypData;   
                    


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
            TtlKeyTypData.uchStatus = 0;                        /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                   /* W/o Amount Input Case */
                TtlKeyTypData.uchStatus |= MAINT_WITHOUT_DATA;  /* Set W/o Amount Status */
            } else {                                            /* W/ Amount Input Case */
                TtlKeyTypData.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Read Total Key Type Procedure */
            
            TtlKeyTypData.uchMajorClass = CLASS_PARATTLKEYTYP;  /* Set Major Class */

            if ((sError = MaintTtlKeyTypRead(&TtlKeyTypData)) == SUCCESS) {    
                UieAccept();                                    /* Return to UifPG60Function() */
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
**  Synopsis: SHORT UifPG60EnterData(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.60 Enter Data Mode 
*===============================================================================
*/
    

SHORT UifPG60EnterData(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARATTLKEYTYP   TtlKeyTypData;   
                    

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P1);                        /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P1:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {       /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            TtlKeyTypData.uchStatus = 0;                        /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                   /* W/o Amount Input Case */
                TtlKeyTypData.uchStatus |= MAINT_WITHOUT_DATA;  /* Set W/o Amount Status */
            } else {                                            /* W/ Amount Input Case */
                TtlKeyTypData.uchTypeIdx = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Write Total Key Type Procedure */
            
            /* TtlKeyTypData.uchMajorClass = CLASS_PARATTLKEYTYP;   Set Major Class */

            if ((sError = MaintTtlKeyTypWrite(&TtlKeyTypData)) == SUCCESS) {    
                UieAccept();                                    /* Return to UifPG60Function() */
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


    
    
    
