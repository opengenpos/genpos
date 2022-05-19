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
* Title       : Special Mnemonics Module                         
* Category    : User Interface For Program, NCR 2170 US Hospitality Application           
* Program Name: UIFPG23.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifPG23Function()   : Program No.23 Entry Mode
*               UifPG23EnterALPHA() : Program No.23 Enter Mnemonics Data Mode
*               UifPG23IssueRpt()   : Program No.23 Issue Report
*               UifPG23EnterAddr()  : Program No.23 Enter Address Mode
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jun-30-92:00.00.01    :K.You      : initial                                   
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
**  Synopsis: SHORT UifPG23Function(KEYMSG *pKeyMsg) 
*               
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               
**  Description: Program No.23 Entry Mode 
*===============================================================================
*/

/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildPG23Init[] = {{UifPGShift, CID_PGSHIFT},
                                    {UifPG23EnterALPHA, CID_PG23ENTERALPHA},
                                    {UifPGChangeKB1, CID_PGCHANGEKB1},
                                    {NULL,           0              }};


/* Define Next Function at UIM_ACCEPTED from CID_PGCHANGEKB1 */

UIMENU FARCONST aChildPG23Accept1[] = {{UifPG23IssueRpt, CID_PG23ISSUERPT},
                                       {UifPG23EnterAddr, CID_PG23ENTERADDR},
                                       {NULL,             0                }};


/* Define Next Function at UIM_ACCEPTED from CID_PGCHANGEKB2 */

UIMENU FARCONST aChildPG23Accept2[] = {{UifPG23EnterAddr, CID_PG23ENTERADDR},
                                       {NULL,             0                }};


/* Define Next Function at UIM_ACCEPTED from CID_PG23ENTERALPHA,CID_PG23ENTERADDR */

UIMENU FARCONST aChildPG23Accept3[] = {{UifPGShift, CID_PGSHIFT},
                                       {UifPG23EnterALPHA, CID_PG23ENTERALPHA},
                                       {UifPGChangeKB2, CID_PGCHANGEKB2},
                                       {NULL,           0              }};



SHORT UifPG23Function(KEYMSG *pKeyMsg) 
{

    PARASPEMNEMO  SpeMnemoData;


    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        /* Display Address 1 of This Function */

        SpeMnemoData.uchStatus = 0;                             /* Set W/ Amount Status */
        SpeMnemoData.uchAddress = 1;                            /* Set Address 1 */
        SpeMnemoData.uchMajorClass = CLASS_PARASPECIALMNEMO;    /* Set Major Class */
        MaintSpeMnemoRead(&SpeMnemoData);                       /* Execute Read Special Mnemonics Procedure */

        UieNextMenu(aChildPG23Init);                            /* Open Next Function */
        break;

    case UIM_ACCEPTED:
        switch (pKeyMsg->SEL.usFunc) {
        case CID_PGCHANGEKB1:
            UieNextMenu(aChildPG23Accept1);                     /* Open Next Function */
            break;

        case CID_PGCHANGEKB2:
            UieNextMenu(aChildPG23Accept2);                     /* Open Next Function */
            break;

        default:                                                /* CID_PG23ENTERALPHA */
            UieNextMenu(aChildPG23Accept3);                     /* CID_PG23ENTERADDR */           
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
**  Synopsis: SHORT UifPG23EnterALPHA(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.23 Enter Mnemonics Data Mode  
*===============================================================================
*/
    


SHORT UifPG23EnterALPHA(KEYMSG *pKeyMsg) 
{

    SHORT           sError;    
    PARASPEMNEMO    SpeMnemoData;   
                          

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P1);                            /* Register Key Sequence */
        UieEchoBack(UIE_ECHO_ROW1_AN, PARA_SPEMNEMO_LEN);           /* Define Display Format and Max. Input Data */

        /* Set KB Mode to ALPHA Mode */

        UifACChgKBType(UIF_ALPHA_KB);
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P1:
            SpeMnemoData.uchStatus = 0;                             /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount Input Case */
                SpeMnemoData.uchStatus |= MAINT_WITHOUT_DATA;       /* Set W/o Amount Status */
            } else {                                                /* W/ Amount Input Case */
                memset(SpeMnemoData.aszMnemonics,                   /* Full NULL to Own Buffer */
                       '\0',
                       (PARA_SPEMNEMO_LEN+1) * sizeof(TCHAR));

                _tcsncpy(SpeMnemoData.aszMnemonics, pKeyMsg->SEL.INPUT.aszKey, pKeyMsg->SEL.INPUT.uchLen); /* Copy Input Mnemonics to Own Buffer */
                //memcpy(SpeMnemoData.aszMnemonics, pKeyMsg->SEL.INPUT.aszKey, pKeyMsg->SEL.INPUT.uchLen); /* Copy Input Mnemonics to Own Buffer */
            }

            /* Execute Write Mnemonic Procedure */
            
            /* SpeMnemoData.uchMajorClass = CLASS_PARASPECIALMNEMO;     Set Major Class */

            if ((sError = MaintSpeMnemoWrite(&SpeMnemoData)) == SUCCESS) {
                UieAccept();                                        /* Return to UifPG23Function() */
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
**  Synopsis: SHORT UifPG23IssueRpt(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error 
*               
**  Description: Program No.23 Issue Report 
*===============================================================================
*/
    

SHORT UifPG23IssueRpt(KEYMSG *pKeyMsg) 
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
                MaintSpeMnemoReport();              /* Execute Report Procedure */
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
**  Synopsis: SHORT UifPG23EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.23 Enter Address Mode 
*===============================================================================
*/
    


SHORT UifPG23EnterAddr(KEYMSG *pKeyMsg) 
{
                    
    SHORT           sError;
    PARASPEMNEMO    SpeMnemoData;   
                    


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P2);                            /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P2:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {           /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            SpeMnemoData.uchStatus = 0;                             /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount Input Case */
                SpeMnemoData.uchStatus |= MAINT_WITHOUT_DATA;       /* Set W/o Amount Status */
            } else {                                                /* W/ Amount Input Case */
                SpeMnemoData.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Read Special Mnemonics Procedure */
            
            SpeMnemoData.uchMajorClass = CLASS_PARASPECIALMNEMO;    /* Set Major Class */

            if ((sError = MaintSpeMnemoRead(&SpeMnemoData)) == SUCCESS) {    
                UieAccept();                                        /* Return to UifPG23Function() */
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




    
    
    
