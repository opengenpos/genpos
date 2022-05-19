/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1998            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Tender Key Parameter
* Category    : User Interface For Program, NCR 2170 US Hospitality Application
* Program Name: UIFPG62.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifPG62Function()   : Program No.62 Entry Mode
*               UifPG62IssueRpt()   : Program No.62 Issue Report 
*               UifPG62EnterAddr()  : Program No.62 Enter Address Mode 
*               UifPG62EnterData()  : Program No.62 Enter Data Mode 
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Aug-04-98:03.03.00    : hrkato    : V3.3 (Fast Finalize Improvement)
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
**  Synopsis: SHORT UifPG62Function(KEYMSG *pKeyMsg) 
*               
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               
**  Description: Program No.62 Entry Mode  
*===============================================================================
*/
/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildPG62Init[] = {{UifPG62IssueRpt, CID_PG62ISSUERPT},
                                    {UifPG62EnterAddr, CID_PG62ENTERADDR},
                                    {UifPG62EnterData, CID_PG62ENTERDATA},
                                    {NULL,             0                }};

/* Define Next Function at UIM_ACCEPTED */
UIMENU FARCONST aChildPG62Accept[] = {{UifPG62EnterAddr, CID_PG62ENTERADDR},
                                      {UifPG62EnterData, CID_PG62ENTERDATA},
                                      {NULL,             0                }};

SHORT   UifPG62Function(KEYMSG *pKeyMsg) 
{
    PARATEND    TendData;

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildPG62Init);

        /* Display Address 1 of This Function */
        TendData.uchStatus = 0;                                     /* Set W/ Amount Status */
        TendData.uchAddress = 1;                                    /* Set Address 1 */
        TendData.uchMajorClass = CLASS_PARATEND;
        MaintTendRead(&TendData);
        break;

    case UIM_ACCEPTED:
        UieNextMenu(aChildPG62Accept);                              /* Open Next Function */
        break;

    default:
        return(UifPGDefProc(pKeyMsg));                              /* execute Default Procedure */
    }
    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT UifPG62IssueRpt(KEYMSG *pKeyMsg)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: Program No.62 Issue Report 
*===============================================================================
*/
SHORT   UifPG62IssueRpt(KEYMSG *pKeyMsg)
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
            MaintTendReport();
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
**  Synopsis: SHORT UifPG62EnterAddr(KEYMSG *pKeyMsg)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.62 Enter Address Mode 
*===============================================================================
*/
SHORT   UifPG62EnterAddr(KEYMSG *pKeyMsg)
{
    SHORT           sError;
    PARATEND        TendData;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P2);                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P2:
            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            TendData.uchStatus = 0;                         /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {               /* W/o Amount Input Case */
                TendData.uchStatus |= MAINT_WITHOUT_DATA;   /* Set W/o Amount Status */
            } else {                                        /* W/ Amount Input Case */
                TendData.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }
            /* Execute Read Procedure */
            TendData.uchMajorClass = CLASS_PARATEND;
            if ((sError = MaintTendRead(&TendData)) == SUCCESS) {
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
**  Synopsis: SHORT UifPG62EnterData(KEYMSG *pKeyMsg)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.62 Enter Data Mode
*===============================================================================
*/
SHORT   UifPG62EnterData(KEYMSG *pKeyMsg)
{
    SHORT           sError;
    PARATEND        TendData;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P1);                /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P1:
            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.lData == 1 || pKeyMsg->SEL.INPUT.lData > 8) {   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            TendData.uchStatus = 0;                         /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {               /* W/o Amount Input Case */
                TendData.uchStatus |= MAINT_WITHOUT_DATA;   /* Set W/o Amount Status */
            } else {                                        /* W/ Amount Input Case */
                TendData.uchTend = (UCHAR)pKeyMsg->SEL.INPUT.lData;
            }
            /* Execute Write Procedure */
            TendData.uchMajorClass = CLASS_PARATEND;
            if ((sError = MaintTendWrite(&TendData)) == SUCCESS) {
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

/* --- End of Source --- */
