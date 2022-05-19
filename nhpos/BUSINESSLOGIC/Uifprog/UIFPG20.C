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
* Title       : Transaction Mnemonics Module                         
* Category    : User Interface For Program, NCR 2170 US Hospitality Application           
* Program Name: UIFPG20.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifPG20Function()   : Program No.20 Entry Mode
*               UifPG20EnterALPHA() : Program No.20 Enter Mnemonics Data Mode
*               UifPG20IssueRpt()   : Program No.20 Issue Report
*               UifPG20EnterAddr()  : Program No.20 Enter Address Mode
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
#include <uie.h>
#include <pif.h>
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
**  Synopsis: SHORT UifPG20Function(KEYMSG *pKeyMsg) 
*               
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               
**  Description: Program No.20 Entry Mode  
*===============================================================================
*/

/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildPG20Init[] = {{UifPGShift, CID_PGSHIFT},
                                    {UifPG20EnterALPHA, CID_PG20ENTERALPHA},
                                    {UifPGChangeKB1, CID_PGCHANGEKB1},
                                    {NULL,           0              }};


/* Define Next Function at UIM_ACCEPTED from CID_PGCHANGEKB1 */

UIMENU FARCONST aChildPG20Accept1[] = {{UifPG20IssueRpt, CID_PG20ISSUERPT},
                                       {UifPG20EnterAddr, CID_PG20ENTERADDR},
                                       {NULL,             0                }};


/* Define Next Function at UIM_ACCEPTED from CID_PGCHANGEKB2 */

UIMENU FARCONST aChildPG20Accept2[] = {{UifPG20EnterAddr, CID_PG20ENTERADDR},
                                       {NULL,             0                }};


/* Define Next Function at UIM_ACCEPTED from CID_PG20ENTERALPHA,CID_PG20ENTERADDR */

UIMENU FARCONST aChildPG20Accept3[] = {{UifPGShift, CID_PGSHIFT},
                                       {UifPG20EnterALPHA, CID_PG20ENTERALPHA},
                                       {UifPGChangeKB2, CID_PGCHANGEKB2},
                                       {NULL,           0              }};



SHORT UifPG20Function(KEYMSG *pKeyMsg) 
{

    PARATRANSMNEMO  TransMnemoData;


    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        /* Display Address 1 of This Function */

        TransMnemoData.uchStatus = 0;                           /* Set W/ Amount Status */
        TransMnemoData.uchAddress = 1;                          /* Set Address 1 */
        TransMnemoData.uchMajorClass = CLASS_PARATRANSMNEMO;    /* Set Major Class */

        MaintTransMnemoRead(&TransMnemoData);                   /* Execute Read Transaction Mnemonics Procedure */

        UieNextMenu(aChildPG20Init);                            /* Open Next Function */
        break;

    case UIM_ACCEPTED:
        switch (pKeyMsg->SEL.usFunc) {
        case CID_PGCHANGEKB1:
            UieNextMenu(aChildPG20Accept1);                     /* Open Next Function */
            break;

        case CID_PGCHANGEKB2:
            UieNextMenu(aChildPG20Accept2);                     /* Open Next Function */
            break;

        default:                                                /* CID_PG20ENTERALPHA */
            UieNextMenu(aChildPG20Accept3);                     /* CID_PG20ENTERADDR */ 
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
**  Synopsis: SHORT UifPG20EnterALPHA(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.20 Enter Mnemonics Data Mode 
*===============================================================================
*/
    


SHORT UifPG20EnterALPHA(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARATRANSMNEMO  TransMnemoData;   


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P1);                            /* Register Key Sequence */
        UieEchoBack(UIE_ECHO_ROW1_AN, PARA_TRANSMNEMO_LEN);         /* Define Display Format and Max. Input Data */

        /* Set KB Mode to ALPHA Mode */

        UifACChgKBType(UIF_ALPHA_KB);
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P1:
            TransMnemoData.uchStatus = 0;                           /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount Input Case */
                TransMnemoData.uchStatus |= MAINT_WITHOUT_DATA;     /* Set W/o Amount Status */
            } else {                                                /* W/ Amount Input Case */
                memset(TransMnemoData.aszMnemonics,                 /* Full NULL to Own Buffer */
                       '\0',
                       (PARA_TRANSMNEMO_LEN+1) * sizeof(TCHAR));

                _tcsncpy(TransMnemoData.aszMnemonics, pKeyMsg->SEL.INPUT.aszKey, pKeyMsg->SEL.INPUT.uchLen); /* Copy Input Mnemonics to Own Buffer */
                //memcpy(TransMnemoData.aszMnemonics, pKeyMsg->SEL.INPUT.aszKey, pKeyMsg->SEL.INPUT.uchLen); /* Copy Input Mnemonics to Own Buffer */
            }

            /* Execute Write Tansaction Mnemonic Procedure */
            
            /* TransMnemoData.uchMajorClass = CLASS_PARATRANSMNEMO;    Set Major Class */

            if ((sError = MaintTransMnemoWrite(&TransMnemoData)) == SUCCESS) {
                UieAccept();                                        /* Return to UifPG20Function() */
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
**  Synopsis: SHORT UifPG20IssueRpt(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error 
*               
**  Description: Program No.20 Enter Address Mode 
*===============================================================================
*/
    

SHORT UifPG20IssueRpt(KEYMSG *pKeyMsg) 
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
                MaintTransMnemoReport();            /* Execute Report Procedure */
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
**  Synopsis: SHORT UifPG20EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error 
*               
**  Description: Program No.20 Enter Address Mode 
*===============================================================================
*/
    


SHORT UifPG20EnterAddr(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARATRANSMNEMO  TransMnemoData;   
                    


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P2);                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P2:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.lData > 255L) {                  /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            TransMnemoData.uchStatus = 0;                           /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount Input Case */
                TransMnemoData.uchStatus |= MAINT_WITHOUT_DATA;     /* Set W/o Amount Status */
            } else {                                                /* W/ Amount Input Case */
                TransMnemoData.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Read Transaction Mnemonics Procedure */
            
            TransMnemoData.uchMajorClass = CLASS_PARATRANSMNEMO;    /* Set Major Class */

            if ((sError = MaintTransMnemoRead(&TransMnemoData)) == SUCCESS) {    
                UieAccept();                                        /* Return to UifPG20Function() */
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




    
    
    
