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
* Title       : Report Name Module                         
* Category    : User Interface For Program, NCR 2170 US Hospitality Application           
* Program Name: UIFPG22.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifPG22Function()   : Program No.22 Entry Mode
*               UifPG22EnterALPHA() : Program No.22 Enter Mnemonics Data Mode
*               UifPG22IssueRpt()   : Program No.22 Issue Report
*               UifPG22EnterAddr()  : Program No.22 Enter Address Mode
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
**  Synopsis: SHORT UifPG22Function(KEYMSG *pKeyMsg) 
*               
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               
**  Description: Program No.22 Entry Mode  
*===============================================================================
*/

/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildPG22Init[] = {{UifPGShift, CID_PGSHIFT},
                                    {UifPG22EnterALPHA, CID_PG22ENTERALPHA},
                                    {UifPGChangeKB1, CID_PGCHANGEKB1},
                                    {NULL,           0              }};


/* Define Next Function at UIM_ACCEPTED from CID_PGCHANGEKB1 */

UIMENU FARCONST aChildPG22Accept1[] = {{UifPG22IssueRpt, CID_PG22ISSUERPT},
                                       {UifPG22EnterAddr, CID_PG22ENTERADDR},
                                       {NULL,             0                }};


/* Define Next Function at UIM_ACCEPTED from CID_PGCHANGEKB2 */

UIMENU FARCONST aChildPG22Accept2[] = {{UifPG22EnterAddr, CID_PG22ENTERADDR},
                                       {NULL,             0                }};


/* Define Next Function at UIM_ACCEPTED from CID_PG22ENTERALPHA,CID_PG22ENTERADDR */
                
UIMENU FARCONST aChildPG22Accept3[] = {{UifPGShift, CID_PGSHIFT},
                                       {UifPG22EnterALPHA, CID_PG22ENTERALPHA},
                                       {UifPGChangeKB2, CID_PGCHANGEKB2},
                                       {NULL,           0              }};



SHORT UifPG22Function(KEYMSG *pKeyMsg) 
{

    PARAREPORTNAME  ReportNameData;


    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        /* Display Address 1 of This Function */

        ReportNameData.uchStatus = 0;                           /* Set W/ Amount Status */
        ReportNameData.uchAddress = 1;                          /* Set Address 1 */
        ReportNameData.uchMajorClass = CLASS_PARAREPORTNAME;    /* Set Major Class */
        MaintReportNameRead(&ReportNameData);                   /* Execute Read Report Name Mnemonics Procedure */

        UieNextMenu(aChildPG22Init);                            /* Open Next Function */
        break;

    case UIM_ACCEPTED:
        switch (pKeyMsg->SEL.usFunc) {
        case CID_PGCHANGEKB1:
            UieNextMenu(aChildPG22Accept1);                     /* Open Next Function */
            break;

        case CID_PGCHANGEKB2:
            UieNextMenu(aChildPG22Accept2);                     /* Open Next Function */
            break;

        default:                                                /* CID_PG22ENTERALPHA */
            UieNextMenu(aChildPG22Accept3);                     /* CID_PG22ENTERADDR */
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
**  Synopsis: SHORT UifPG22EnterALPHA(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.22 Enter Mnemonics Data Mode 
*===============================================================================
*/
    


SHORT UifPG22EnterALPHA(KEYMSG *pKeyMsg) 
{

    SHORT           sError;
    PARAREPORTNAME  ReportNameData;   


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P1);                            /* Register Key Sequence */
        UieEchoBack(UIE_ECHO_ROW1_AN, PARA_REPORTNAME_LEN);         /* Define Display Format and Max. Input Data */

        /* Set KB Mode to ALPHA Mode */

        UifACChgKBType(UIF_ALPHA_KB);
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P1:
            ReportNameData.uchStatus = 0;                           /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount Input Case */
                ReportNameData.uchStatus |= MAINT_WITHOUT_DATA;     /* Set W/o Amount Status */
            } else {                                                /* W/ Amount Input Case */
                memset(ReportNameData.aszMnemonics,                 /* Full NULL to Own Buffer */
                       '\0',
                       (PARA_REPORTNAME_LEN+1) * sizeof(TCHAR));

                _tcsncpy(ReportNameData.aszMnemonics, pKeyMsg->SEL.INPUT.aszKey, pKeyMsg->SEL.INPUT.uchLen); /* Copy Input Mnemonics to Own Buffer */
                //memcpy(ReportNameData.aszMnemonics, pKeyMsg->SEL.INPUT.aszKey, pKeyMsg->SEL.INPUT.uchLen); /* Copy Input Mnemonics to Own Buffer */
            }

            /* Execute Write Tansaction Mnemonic Procedure */
            
            /* ReportNameData.uchMajorClass = CLASS_PARAREPORTNAME;     Set Major Class */

            if ((sError = MaintReportNameWrite(&ReportNameData)) == SUCCESS) {    
                UieAccept();                                        /* Return to UifPG22Function() */
            }
            return(sError);                                        

        /* default:
              break; */

        }
        /* break; */

    default:
        return(UifPGDefProc(pKeyMsg));                  /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifPG22IssueRpt(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDTSEQERR_ADR   : Sequence Error
*               
**  Description: Program No.22 Issue Report  
*===============================================================================
*/
    

SHORT UifPG22IssueRpt(KEYMSG *pKeyMsg) 
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
                MaintReportNameReport();            /* Execute Report Procedure */
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
**  Synopsis: SHORT UifPG22EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.22 Enter Address Mode 
*===============================================================================
*/
    


SHORT UifPG22EnterAddr(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARAREPORTNAME  ReportNameData;   
                    


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P2);                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P2:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {               /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            ReportNameData.uchStatus = 0;                               /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount Input Case */
                ReportNameData.uchStatus |= MAINT_WITHOUT_DATA;         /* Set W/o Amount Status */
            } else {                                                    /* W/ Amount Input Case */
                ReportNameData.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Read Report Name Procedure */
            
            ReportNameData.uchMajorClass = CLASS_PARAREPORTNAME;        /* Set Major Class */

            if ((sError = MaintReportNameRead(&ReportNameData)) == SUCCESS) {    
                UieAccept();                                            /* Return to UifPG22Function() */
            }
            return(sError);                                             /* Set Success or Input Data Error */
            
        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifPGDefProc(pKeyMsg));                                  /* Execute Default Procedure */
    }
}




    
    
    
