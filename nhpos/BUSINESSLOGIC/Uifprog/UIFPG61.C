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
* Title       : Total Key Control Module                         
* Category    : User Interface For Program, NCR 2170 US Hospitality Application        
* Program Name: UIFPG61.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifPG61Function()   : Program No.61 Entry Mode
*               UifPG61IssueRpt()   : Program No.61 Issue Report 
*               UifPG61EnterAddr()  : Program No.61 Enter Address Mode 
*               UifPG61EnterField() : Program No.61 Enter Field Mode 
*               UifPG61EnterData()  : Program No.61 Enter Data Mode 
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
**  Synopsis: SHORT UifPG61Function(KEYMSG *pKeyMsg) 
*               
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               
**  Description: Program No.61 Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildPG61Init[] = {{UifPG61IssueRpt, CID_PG61ISSUERPT},
                                    {UifPG61EnterAddr, CID_PG61ENTERADDR},
                                    {UifPG61EnterField, CID_PG61ENTERFIELD},
                                    {UifPG61EnterData, CID_PG61ENTERDATA},
                                    {NULL,             0                }};

/* Define Next Function at UIM_ACCEPTED */

UIMENU FARCONST aChildPG61Accept[] = {{UifPG61EnterAddr, CID_PG61ENTERADDR},
                                      {UifPG61EnterField, CID_PG61ENTERFIELD},
                                      {UifPG61EnterData, CID_PG61ENTERDATA},
                                      {NULL,             0                }};


SHORT UifPG61Function(KEYMSG *pKeyMsg) 
{
    
    PARATTLKEYCTL     TtlKeyCtl;   
                
    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildPG61Init);                            /* Open Next Function */
                                                                        
        /* Display Address 1 of This Function */

        TtlKeyCtl.uchStatus = 0;                                /* Set W/ Amount Status */
        TtlKeyCtl.uchAddress = 1;                               /* Set Address 1 */
        TtlKeyCtl.uchField = 1;                                 /* Set Field 1 */
        TtlKeyCtl.uchMajorClass = CLASS_PARATTLKEYCTL;          /* Set Major Class */
        TtlKeyCtl.uchMinorClass = CLASS_PARATTLKEYCTL_ALL;      /* Set Minor Class */
        MaintTtlKeyCtlRead(&TtlKeyCtl);                         /* Execute Read MDC Procedure */
        break;

    case UIM_ACCEPTED:
        UieNextMenu(aChildPG61Accept);                          /* Open Next Function */
        break;

    default:
        return(UifPGDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
    return(SUCCESS);
}


/*
*=============================================================================
**  Synopsis: SHORT UifPG61IssueRpt(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: Program No.61 Issue Report  
*===============================================================================
*/
    


SHORT UifPG61IssueRpt(KEYMSG *pKeyMsg) 
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
            MaintTtlKeyCtlReport();                 /* Execute Report Procedure */
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
**  Synopsis: SHORT UifPG61EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.61 Enter Address Mode 
*===============================================================================
*/
    


SHORT UifPG61EnterAddr(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARATTLKEYCTL   TtlKeyCtl;   
                    


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
            TtlKeyCtl.uchStatus = 0;                            /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                   /* W/o Amount Input Case */
                TtlKeyCtl.uchStatus |= MAINT_WITHOUT_DATA;      /* Set W/o Amount Status */
            } else {                                            /* W/ Amount Input Case */
                TtlKeyCtl.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Read Total Key Control Procedure */
            
            TtlKeyCtl.uchMajorClass = CLASS_PARATTLKEYCTL;      /* Set Major Class */
            TtlKeyCtl.uchMinorClass = CLASS_PARATTLKEYCTL_ADR;  /* Set Minor Class */
            if ((sError = MaintTtlKeyCtlRead(&TtlKeyCtl)) == SUCCESS) {    
                UieAccept();                                    /* Return to UifPG61Function() */
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
**  Synopsis: SHORT UifPG61EnterField(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.61 Enter Field Mode 
*===============================================================================
*/
    


SHORT UifPG61EnterField(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARATTLKEYCTL   TtlKeyCtl;   
                    


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P4);                        /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P4:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {       /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            TtlKeyCtl.uchStatus = 0;                            /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                   /* W/o Amount Input Case */
                TtlKeyCtl.uchStatus |= MAINT_WITHOUT_DATA;      /* Set W/o Amount Status */
            } else {                                            /* W/ Amount Input Case */
                TtlKeyCtl.uchField = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Read MDC Procedure */
            
            TtlKeyCtl.uchMajorClass = CLASS_PARATTLKEYCTL;          /* Set Major Class */
            TtlKeyCtl.uchMinorClass = CLASS_PARATTLKEYCTL_FLD;      /* Set Minor Class */
            if ((sError = MaintTtlKeyCtlRead(&TtlKeyCtl)) == SUCCESS) {    
                UieAccept();                                    /* Return to UifPG61Function() */
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
**  Synopsis: SHORT UifPG61EnterData(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.61 Enter Data Mode 
*===============================================================================
*/
    

SHORT UifPG61EnterData(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARATTLKEYCTL   TtlKeyCtl;
                    

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P1);                            /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P1:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT4) {           /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            TtlKeyCtl.uchStatus = 0;                                /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount Input Case */
                TtlKeyCtl.uchStatus |= MAINT_WITHOUT_DATA;          /* Set W/o Amount Status */
            } else {                                                /* W/ Amount Input Case */
                TtlKeyCtl.uchTtlKeyMDCData = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
                memset(TtlKeyCtl.aszTtlKeyMDCString,                /* Full NULL to Own Buffer */
                       '\0',
                       sizeof(TtlKeyCtl.aszTtlKeyMDCString));

                _tcsncpy(TtlKeyCtl.aszTtlKeyMDCString, pKeyMsg->SEL.INPUT.aszKey, pKeyMsg->SEL.INPUT.uchLen); /* Copy Input Mnemonics to Own Buffer */
                //memcpy(TtlKeyCtl.aszTtlKeyMDCString, pKeyMsg->SEL.INPUT.aszKey, pKeyMsg->SEL.INPUT.uchLen); /* Copy Input Mnemonics to Own Buffer */
            }

            /* Execute Write MDC Procedure */
            
            /* TtlKeyCtl.uchMajorClass = CLASS_PARATTLKEYCTL;          Set Major Class */

            if ((sError = MaintTtlKeyCtlWrite(&TtlKeyCtl)) == SUCCESS) {    
                UieAccept();                                        /* Return to UifPG61Function() */
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


    
    
    
