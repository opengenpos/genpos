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
* Title       : 24 Characters Mnemonics Module                         
* Category    : User Interface For Program, NCR 2170 US Hospitality Application           
* Program Name: UIFPG57.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifPG57Function()   : Program No.57 Entry Mode
*               UifPG57EnterALPHA() : Program No.57 Enter Mnemonics Data Mode
*               UifPG57IssueRpt()   : Program No.57 Issue Report
*               UifPG57EnterAddr()  : Program No.57 Enter Address Mode
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
**  Synopsis: SHORT UifPG57Function(KEYMSG *pKeyMsg) 
*               
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               
**  Description: Program No.57 Entry Mode 
*===============================================================================
*/

/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildPG57Init[] = {{UifPGShift, CID_PGSHIFT},
                                    {UifPG57EnterALPHA, CID_PG57ENTERALPHA},
                                    {UifPGChangeKB1, CID_PGCHANGEKB1},
                                    {NULL,           0              }};


/* Define Next Function at UIM_ACCEPTED from CID_PGCHANGEKB1 */

UIMENU FARCONST aChildPG57Accept1[] = {{UifPG57IssueRpt, CID_PG57ISSUERPT},
                                       {UifPG57EnterAddr, CID_PG57ENTERADDR},
                                       {NULL,             0                }};


/* Define Next Function at UIM_ACCEPTED from CID_PGCHANGEKB2 */

UIMENU FARCONST aChildPG57Accept2[] = {{UifPG57EnterAddr, CID_PG57ENTERADDR},
                                       {NULL,             0                }};


/* Define Next Function at UIM_ACCEPTED from CID_PG57ENTERALPHA,CID_PG57ENTERADDR */

UIMENU FARCONST aChildPG57Accept3[] = {{UifPGShift, CID_PGSHIFT},
                                       {UifPG57EnterALPHA, CID_PG57ENTERALPHA},
                                       {UifPGChangeKB2, CID_PGCHANGEKB2},
                                       {NULL,           0              }};



SHORT UifPG57Function(KEYMSG *pKeyMsg) 
{

    PARACHAR24  Char24Data;


    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        /* Display Address 1 of This Function */

        Char24Data.uchStatus = 0;                               /* Set W/ Amount Status */
        Char24Data.uchAddress = 1;                              /* Set Address 1 */
        Char24Data.uchMajorClass = CLASS_PARACHAR24;            /* Set Major Class */
        MaintChar24Read(&Char24Data);                           /* Execute Read Procedure */

        UieNextMenu(aChildPG57Init);                            /* Open Next Function */
        break;

    case UIM_ACCEPTED:
        switch (pKeyMsg->SEL.usFunc) {
        case CID_PGCHANGEKB1:
            UieNextMenu(aChildPG57Accept1);                     /* Open Next Function */
            break;

        case CID_PGCHANGEKB2:
            UieNextMenu(aChildPG57Accept2);                     /* Open Next Function */
            break;

        default:                                                /* CID_PG57ENTERALPHA */
            UieNextMenu(aChildPG57Accept3);                     /* CID_PG57ENTERADDR */
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
**  Synopsis: SHORT UifPG57EnterALPHA(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong data Error
*               
**  Description: Program No.57 Enter Mnemonics Data Mode  
*===============================================================================
*/
    


SHORT UifPG57EnterALPHA(KEYMSG *pKeyMsg) 
{
    
    SHORT       sError;
    PARACHAR24  Char24Data;   


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P1);                            /* Register Key Sequence */
        UieEchoBack(UIE_ECHO_ROW1_AN, PARA_CHAR24_LEN);             /* Define Display Format and Max. Input Data */

        /* Set KB Mode to ALPHA Mode */

        UifACChgKBType(UIF_ALPHA_KB);
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P1:
            Char24Data.uchStatus = 0;                               /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount Input Case */
                Char24Data.uchStatus |= MAINT_WITHOUT_DATA;         /* Set W/o Amount Status */
            } else {                                                /* W/ Amount Input Case */
                memset(Char24Data.aszMnemonics,                     /* Full NULL to Own Buffer */
                       '\0',
                       (PARA_CHAR24_LEN+1) * sizeof(TCHAR));

                _tcsncpy(Char24Data.aszMnemonics, pKeyMsg->SEL.INPUT.aszKey, pKeyMsg->SEL.INPUT.uchLen); /* Copy Input Mnemonics to Own Buffer */
                //memcpy(Char24Data.aszMnemonics, pKeyMsg->SEL.INPUT.aszKey, pKeyMsg->SEL.INPUT.uchLen); /* Copy Input Mnemonics to Own Buffer */
            }

            /* Execute Write Tansaction Mnemonic Procedure */
            
            /* Char24Data.uchMajorClass = CLASS_PARACHAR24;             Set Major Class */

            if ((sError = MaintChar24Write(&Char24Data)) == SUCCESS) {
                UieAccept();                                        /* Return to UifPG57Function() */
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
**  Synopsis: SHORT UifPG57IssueRpt(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: Program No.57 Issue Report  
*===============================================================================
*/
    

SHORT UifPG57IssueRpt(KEYMSG *pKeyMsg) 
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
                MaintChar24Report();                /* Execute Report Procedure */
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
**  Synopsis: SHORT UifPG57EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error 
*               
**  Description: Program No.57 Enter Address Mode 
*===============================================================================
*/
    


SHORT UifPG57EnterAddr(KEYMSG *pKeyMsg) 
{
    
    SHORT       sError;
    PARACHAR24  Char24Data;   
                        


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P2);                /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P2:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {           /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            Char24Data.uchStatus = 0;                               /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount Input Case */
                Char24Data.uchStatus |= MAINT_WITHOUT_DATA;         /* Set W/o Amount Status */
            } else {                                                /* W/ Amount Input Case */
                Char24Data.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Read 24 Characters Mnemonics Procedure */
            
            Char24Data.uchMajorClass = CLASS_PARACHAR24;            /* Set Major Class */

            if ((sError = MaintChar24Read(&Char24Data)) == SUCCESS) {    
                UieAccept();                                        /* Return to UifPG57Function() */
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




    
    
    
