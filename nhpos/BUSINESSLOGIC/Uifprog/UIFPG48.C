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
* Title       : Major Department Mnemonics Module                         
* Category    : User Interface For Program, NCR 2170 US Hospitality Application           
* Program Name: UIFPG48.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifPG48Function()   : Program No.48 Entry Mode
*               UifPG48EnterALPHA() : Program No.48 Enter Mnemonics Data Mode
*               UifPG48IssueRpt()   : Program No.48 Issue Report
*               UifPG48EnterAddr()  : Program No.48 Enter Address Mode
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
**  Synopsis: SHORT UifPG48Function(KEYMSG *pKeyMsg) 
*               
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               
**  Description: Program No.48 Entry Mode  
*===============================================================================
*/

/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildPG48Init[] = {{UifPGShift, CID_PGSHIFT},
                                    {UifPG48EnterALPHA, CID_PG48ENTERALPHA},
                                    {UifPGChangeKB1, CID_PGCHANGEKB1},
                                    {NULL,           0              }};


/* Define Next Function at UIM_ACCEPTED from CID_PGCHANGEKB1 */

UIMENU FARCONST aChildPG48Accept1[] = {{UifPG48IssueRpt, CID_PG48ISSUERPT},
                                       {UifPG48EnterAddr, CID_PG48ENTERADDR},
                                       {NULL,             0                }};


/* Define Next Function at UIM_ACCEPTED from CID_PGCHANGEKB2 */

UIMENU FARCONST aChildPG48Accept2[] = {{UifPG48EnterAddr, CID_PG48ENTERADDR},
                                       {NULL,             0                }};


/* Define Next Function at UIM_ACCEPTED from CID_PG48ENTERALPHA,CID_PG48ENTERADDR */

UIMENU FARCONST aChildPG48Accept3[] = {{UifPGShift, CID_PGSHIFT},
                                       {UifPG48EnterALPHA, CID_PG48ENTERALPHA},
                                       {UifPGChangeKB2, CID_PGCHANGEKB2},
                                       {NULL,           0              }};



SHORT UifPG48Function(KEYMSG *pKeyMsg) 
{

    PARAMAJORDEPT  MajorDEPTData;


    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        /* Display Address 1 of This Function */

        MajorDEPTData.uchStatus = 0;                                /* Set W/ Amount Status */
        MajorDEPTData.uchAddress = 1;                               /* Set Address 1 */
        MajorDEPTData.uchMajorClass = CLASS_PARAMAJORDEPT;          /* Set Major Class */
        MaintMajorDEPTRead(&MajorDEPTData);                         /* Execute Read Major Department Mnemonics Procedure */

        UieNextMenu(aChildPG48Init);                                /* Open Next Function */
        break;

    case UIM_ACCEPTED:
        switch (pKeyMsg->SEL.usFunc) {
        case CID_PGCHANGEKB1:
            UieNextMenu(aChildPG48Accept1);                         /* Open Next Function */
            break;

        case CID_PGCHANGEKB2:
            UieNextMenu(aChildPG48Accept2);                         /* Open Next Function */
            break;

        default:                                                    /* CID_PG48ENTERALPHA */
            UieNextMenu(aChildPG48Accept3);                         /* CID_PG48ENTERADDR */
            break;
        
        }
        break;

    default:
        return(UifPGDefProc(pKeyMsg));                              /* Execute Default Procedure */
    }
    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT UifPG48EnterALPHA(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error 
*               
**  Description: Program No.48 Enter Mnemonics Data Mode  
*===============================================================================
*/
    


SHORT UifPG48EnterALPHA(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARAMAJORDEPT   MajorDEPTData;   
                    

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P1);                            /* Register Key Sequence */
        UieEchoBack(UIE_ECHO_ROW1_AN, PARA_MAJORDEPT_LEN);          /* Define Display Format and Max. Input Data */

        /* Set KB Mode to ALPHA Mode */

        UifACChgKBType(UIF_ALPHA_KB);
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P1:
            MajorDEPTData.uchStatus = 0;                            /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount Input Case */
                MajorDEPTData.uchStatus |= MAINT_WITHOUT_DATA;      /* Set W/o Amount Status */
            } else {                                                /* W/ Amount Input Case */
                memset(MajorDEPTData.aszMnemonics,                  /* Full NULL to Own Buffer */
                       '\0',
                       (PARA_MAJORDEPT_LEN+1) * sizeof(TCHAR));

                _tcsncpy(MajorDEPTData.aszMnemonics, pKeyMsg->SEL.INPUT.aszKey, pKeyMsg->SEL.INPUT.uchLen); /* Copy Input Mnemonics to Own Buffer */
                //memcpy(MajorDEPTData.aszMnemonics, pKeyMsg->SEL.INPUT.aszKey, pKeyMsg->SEL.INPUT.uchLen); /* Copy Input Mnemonics to Own Buffer */
            }

            /* Execute Write Tansaction Mnemonic Procedure */
            
            /* MajorDEPTData.uchMajorClass = CLASS_PARAMAJORDEPT;       Set Major Class */

            if ((sError = MaintMajorDEPTWrite(&MajorDEPTData)) == SUCCESS) {
                UieAccept();                                        /* Return to UifPG48Function() */
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
**  Synopsis: SHORT UifPG48IssueRpt(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: Program No.48 Issue Report 
*===============================================================================
*/
    

SHORT UifPG48IssueRpt(KEYMSG *pKeyMsg) 
{
    
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P3);                /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        /* Check Digit */

        if (pKeyMsg->SEL.INPUT.uchLen) {                /* W/ Amount Case */
            return(LDT_SEQERR_ADR);
        }
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
            case FSC_P3:
                MaintMajorDEPTReport();                 /* Execute Report Procedure */
                UieExit(aPGEnter);                      /* Return to UifPGEnter() */
                return(SUCCESS);
            
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
**  Synopsis: SHORT UifPG48EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong data Error 
*               
**  Description: Program No.48 Enter Address Mode 
*===============================================================================
*/
    


SHORT UifPG48EnterAddr(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARAMAJORDEPT   MajorDEPTData;   
                    


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
            MajorDEPTData.uchStatus = 0;                                /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount Input Case */
                MajorDEPTData.uchStatus |= MAINT_WITHOUT_DATA;          /* Set W/o Amount Status */
            } else {                                                    /* W/ Amount Input Case */
                MajorDEPTData.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Read Major Department Mnemonics Procedure */
            
            MajorDEPTData.uchMajorClass = CLASS_PARAMAJORDEPT;          /* Set Major Class */

            if ((sError = MaintMajorDEPTRead(&MajorDEPTData)) == SUCCESS) {    
                UieAccept();                                            /* Return to UifPG48Function() */
            }
            return(sError);                                             /* Set Input Data Error */
            
        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifPGDefProc(pKeyMsg));                              /* Execute Default Procedure */
    }
}




    
    
    
