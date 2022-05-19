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
* Title       : Password for PC Interface Module                         
* Category    : User Interface For Program, NCR 3970 US Hospitality Application           
* Program Name: UIFPG39.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifPG39Function()   : Program No.39 Entry Mode
*               UifPG39EnterALPHA() : Program No.39 Enter Mnemonics Data Mode
*               UifPG39IssueRpt()   : Program No.39 Issue Report
*               UifPG39EnterAddr()  : Program No.39 Enter Address Mode
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jun-22-92:00.00.01    :K.You      : initial                                   
* Aug-25-93:01.00.13    :J.IKEDA    : Chg UieEchoBack() 2nd argument. 
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
**  Synopsis: SHORT UifPG39Function(KEYMSG *pKeyMsg) 
*               
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               
**  Description: Program No.39 Entry Mode  
*===============================================================================
*/

/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildPG39Init[] = {{UifPGShift, CID_PGSHIFT},
                                    {UifPG39EnterALPHA, CID_PG39ENTERALPHA},
                                    {UifPGChangeKB1, CID_PGCHANGEKB1},
                                    {NULL,           0              }};


/* Define Next Function at UIM_ACCEPTED from CID_PGCHANGEKB1 */

UIMENU FARCONST aChildPG39Accept1[] = {{UifPG39IssueRpt, CID_PG39ISSUERPT},
                                       {UifPG39EnterAddr, CID_PG39ENTERADDR},
                                       {NULL,             0                }};


/* Define Next Function at UIM_ACCEPTED from CID_PGCHANGEKB2 */

UIMENU FARCONST aChildPG39Accept2[] = {{UifPG39EnterAddr, CID_PG39ENTERADDR},
                                       {NULL,             0                }};


/* Define Next Function at UIM_ACCEPTED from CID_PG39ENTERALPHA,CID_PG39ENTERADDR */

UIMENU FARCONST aChildPG39Accept3[] = {{UifPGShift, CID_PGSHIFT},
                                       {UifPG39EnterALPHA, CID_PG39ENTERALPHA},
                                       {UifPGChangeKB2, CID_PGCHANGEKB2},
                                       {NULL,           0              }};



SHORT UifPG39Function(KEYMSG *pKeyMsg) 
{

    PARAPCIF    ParaPCIFData;


    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        /* Display Address 1 of This Function */

        ParaPCIFData.uchStatus = 0;                             /* Set W/ Amount Status */
        ParaPCIFData.uchAddress = 1;                            /* Set Address 1 */
        ParaPCIFData.uchMajorClass = CLASS_PARAPCIF;            /* Set Major Class */
        MaintPCIFRead(&ParaPCIFData);                           /* Execute Read Lead Through Mnemonics Procedure */

        UieNextMenu(aChildPG39Init);                            /* Open Next Function */
        break;

    case UIM_ACCEPTED:
        switch (pKeyMsg->SEL.usFunc) {
        case CID_PGCHANGEKB1:
            UieNextMenu(aChildPG39Accept1);                     /* Open Next Function */
            break;

        case CID_PGCHANGEKB2:
            UieNextMenu(aChildPG39Accept2);                     /* Open Next Function */
            break;

        default:                                                /* CID_PG39ENTERALPHA */
            UieNextMenu(aChildPG39Accept3);                     /* CID_PG39ENTERADDR */
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
**  Synopsis: SHORT UifPG39EnterALPHA(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.39 Enter Mnemonics Data Mode 
*===============================================================================
*/
    


SHORT UifPG39EnterALPHA(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARAPCIF    ParaPCIFData;   


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P1);                            /* Register Key Sequence */
        UieEchoBack(UIE_ECHO_ROW1_AN, PARA_COMMAND_LEN);            /* Define Display Format and Max. Input Data */

        /* Set KB Mode to ALPHA Mode */

        UifACChgKBType(UIF_ALPHA_KB);
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P1:
            ParaPCIFData.uchStatus = 0;                             /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount Input Case */
                ParaPCIFData.uchStatus |= MAINT_WITHOUT_DATA;       /* Set W/o Amount Status */
            } else {                                                /* W/ Amount Input Case */
                memset(ParaPCIFData.aszMnemonics,                   /* Full NULL to Own Buffer */
                       '\0',
                       (PARA_PCIF_LEN+1) * sizeof(TCHAR));

                _tcsncpy(ParaPCIFData.aszMnemonics, pKeyMsg->SEL.INPUT.aszKey, pKeyMsg->SEL.INPUT.uchLen); /* Copy Input Mnemonics to Own Buffer */
                //memcpy(ParaPCIFData.aszMnemonics, pKeyMsg->SEL.INPUT.aszKey, pKeyMsg->SEL.INPUT.uchLen); /* Copy Input Mnemonics to Own Buffer */
                       
                       
            }

            /* Execute Write Tansaction Mnemonic Procedure */
            
            /* ParaPCIFData.uchMajorClass = CLASS_PARAPCIF;            Set Major Class */

            if ((sError = MaintPCIFWrite(&ParaPCIFData)) == SUCCESS) {    
                UieAccept();                                        /* Return to UifPG39Function() */
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
**  Synopsis: SHORT UifPG39IssueRpt(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error 
*               
**  Description: Program No.39 Issue Report 
*===============================================================================
*/
    

SHORT UifPG39IssueRpt(KEYMSG *pKeyMsg) 
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
                MaintPCIFReport();                  /* Execute Report Procedure */
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
**  Synopsis: SHORT UifPG39EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.39 Enter Address Mode 
*===============================================================================
*/
    


SHORT UifPG39EnterAddr(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARAPCIF        ParaPCIFData;   
                         


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P2);                            /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P2:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {           /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            ParaPCIFData.uchStatus = 0;                             /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount Input Case */
                ParaPCIFData.uchStatus |= MAINT_WITHOUT_DATA;       /* Set W/o Amount Status */
            } else {                                                /* W/ Amount Input Case */
                ParaPCIFData.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Read PC IF Password Mnemonics Procedure */
            
            ParaPCIFData.uchMajorClass = CLASS_PARAPCIF;            /* Set Major Class */

            if ((sError = MaintPCIFRead(&ParaPCIFData)) == SUCCESS) {    
                UieAccept();                                        /* Return to UifPG39Function() */
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




    
    
    
