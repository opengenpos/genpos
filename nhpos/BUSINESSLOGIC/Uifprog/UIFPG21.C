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
* Title       : Lead Through Mnemonics Module                         
* Category    : User Interface For Program, NCR 2170 US Hospitality Application           
* Program Name: UIFPG21.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifPG21Function()   : Program No.21 Entry Mode
*               UifPG21EnterALPHA() : Program No.21 Enter Mnemonics Data Mode
*               UifPG21IssueRpt()   : Program No.21 Issue Report
*               UifPG21EnterAddr()  : Program No.21 Enter Address Mode
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jun-30-92:00.00.01    :K.You      : initial                                   
* Aug-11-95:03.00.04    :M.Ozawa    : Increment address digits to 3.
*          :            :           :
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
#include <pif.h>
#include <uie.h>
#include <fsc.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <uifsup.h>
#include <uifprog.h>
#include <uifpgequ.h>                       
#include <cvt.h>                            /* Key Code Table */

#include "uifpgex.h"                        /* Unique Extern Header for UI */


/*
*=============================================================================
**  Synopsis: SHORT UifPG21Function(KEYMSG *pKeyMsg) 
*               
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               
**  Description: Program No.21 Entry Mode  
*===============================================================================
*/

/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildPG21Init[] = {{UifPGShift, CID_PGSHIFT},
                                    {UifPG21EnterALPHA, CID_PG21ENTERALPHA},
                                    {UifPGChangeKB1, CID_PGCHANGEKB1},
                                    {NULL,           0              }};


/* Define Next Function at UIM_ACCEPTED from CID_PGCHANGEKB1 */

UIMENU FARCONST aChildPG21Accept1[] = {{UifPG21IssueRpt, CID_PG21ISSUERPT},
                                       {UifPG21EnterAddr, CID_PG21ENTERADDR},
                                       {NULL,             0                }};


/* Define Next Function at UIM_ACCEPTED from CID_PGCHANGEKB2 */

UIMENU FARCONST aChildPG21Accept2[] = {{UifPG21EnterAddr, CID_PG21ENTERADDR},
                                       {NULL,             0                }};


/* Define Next Function at UIM_ACCEPTED from CID_PG21ENTERALPHA,CID_PG21ENTERADDR */

UIMENU FARCONST aChildPG21Accept3[] = {{UifPGShift, CID_PGSHIFT},
                                       {UifPG21EnterALPHA, CID_PG21ENTERALPHA},
                                       {UifPGChangeKB2, CID_PGCHANGEKB2},
                                       {NULL,           0              }};



SHORT UifPG21Function(KEYMSG *pKeyMsg) 
{
    PARALEADTHRU  LeadThruData;

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        /* Display Address 1 of This Function */
        LeadThruData.uchStatus = 0;                             /* Set W/ Amount Status */
        LeadThruData.uchAddress = 1;                            /* Set Address 1 */
        LeadThruData.uchMajorClass = CLASS_PARALEADTHRU;        /* Set Major Class */
        MaintLeadThruRead(&LeadThruData);                       /* Execute Read Lead Through Mnemonics Procedure */

        UieNextMenu(aChildPG21Init);                            /* Open Next Function */
        break;

    case UIM_ACCEPTED:
        switch (pKeyMsg->SEL.usFunc) {
        case CID_PGCHANGEKB1:
            UieNextMenu(aChildPG21Accept1);                     /* Open Next Function */
            break;

        case CID_PGCHANGEKB2:
            UieNextMenu(aChildPG21Accept2);                     /* Open Next Function */
            break;

        default:                                                /* CID_PG21ENTERALPHA */
            UieNextMenu(aChildPG21Accept3);                     /* CID_PG21ENTERADDR */
            break;
        
        }
        break;

    default:
        return(UifPGDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT UifPG21EnterALPHA(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.21 Enter Mnemonics Data Mode 
*===============================================================================
*/
SHORT UifPG21EnterALPHA(KEYMSG *pKeyMsg) 
{
    SHORT           sError;
    PARALEADTHRU    LeadThruData;   

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P1);                            /* Register Key Sequence */
        UieEchoBack(UIE_ECHO_ROW1_AN, PARA_LEADTHRU_LEN);           /* Define Display Format and Max. Input Data */

        /* Set KB Mode to ALPHA Mode */
        UifACChgKBType(UIF_ALPHA_KB);
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P1:
            LeadThruData.uchStatus = 0;                             /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount Input Case */
                LeadThruData.uchStatus |= MAINT_WITHOUT_DATA;       /* Set W/o Amount Status */
            } else {                                                /* W/ Amount Input Case */
                memset(LeadThruData.aszMnemonics, '\0', (PARA_LEADTHRU_LEN+1) * sizeof(TCHAR));

                _tcsncpy(LeadThruData.aszMnemonics, pKeyMsg->SEL.INPUT.aszKey, pKeyMsg->SEL.INPUT.uchLen); /* Copy Input Mnemonics to Own Buffer */
            }

            /* Execute Write Tansaction Mnemonic Procedure */
            
            /* LeadThruData.uchMajorClass = CLASS_PARALEADTHRU;         Set Major Class */

            if ((sError = MaintLeadThruWrite(&LeadThruData)) == SUCCESS) {    
                UieAccept();                                        /* Return to UifPG21Function() */
            }
            return(sError);                                        

        /* default:
              break; */

        }
        /* break; */

    default:
        return(UifPGDefProc(pKeyMsg));                              /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifPG21IssueRpt(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error 
*               
**  Description: Program No.21 Issue Report 
*===============================================================================
*/
SHORT UifPG21IssueRpt(KEYMSG *pKeyMsg) 
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
                MaintLeadThruReport();              /* Execute Report Procedure */
                UieExit(aPGEnter);                  /* Return to UifPGEnter() */
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
**  Synopsis: SHORT UifPG21EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.21 Enter Address Mode 
*===============================================================================
*/
SHORT UifPG21EnterAddr(KEYMSG *pKeyMsg) 
{
    SHORT           sError;
    PARALEADTHRU    LeadThruData;   
                         
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P2);                            /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P2:
            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT3) {           /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            LeadThruData.uchStatus = 0;                             /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount Input Case */
                LeadThruData.uchStatus |= MAINT_WITHOUT_DATA;       /* Set W/o Amount Status */
            } else {                                                /* W/ Amount Input Case */
                LeadThruData.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Read Lead Through Mnemonics Procedure */
            LeadThruData.uchMajorClass = CLASS_PARALEADTHRU;        /* Set Major Class */

            if ((sError = MaintLeadThruRead(&LeadThruData)) == SUCCESS) {    
                UieAccept();                                        /* Return to UifPG21Function() */
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

/***** End of File ****/
