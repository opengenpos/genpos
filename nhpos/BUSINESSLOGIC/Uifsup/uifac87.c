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
* Title       : Set Trailer Print Message for Soft Check Module                         
* Category    : User Interface For Program, NCR 2170 US Hospitality Application           
* Program Name: UIFAC87.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC87Function()   : A/C No.87 Entry Mode
*               UifAC87EnterALPHA() : A/C No.87 Enter Mnemonics Data Mode
*               UifAC87IssueRpt()   : A/C No.87 Issue Report
*               UifAC87EnterAddr()  : A/C No.87 Enter Address Mode
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :  Ver.Rev.   :   Name    : Description
* Aug-23-93: 01.00.13    : J.IKEDA   : initial                                   
*          :             :           :
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
#include <uifpgequ.h>                       
#include <cvt.h>                            /* Key Code Table */

#include "uifsupex.h"                       /* Unique Extern Header for UI */


/*
*=============================================================================
**  Synopsis: SHORT UifAC87Function(KEYMSG *pKeyMsg) 
*               
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: A/C No.87 Entry Mode  
*===============================================================================
*/

/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC87Init[] = {{UifACShift, CID_ACSHIFT},
                                    {UifAC87EnterALPHA, CID_AC87ENTERALPHA},
                                    {UifACChangeKB1, CID_ACCHANGEKB1},
                                    {NULL,           0            }};


/* Define Next Function at UIM_ACCEPTED from CID_ACCHANGEKB1 */

UIMENU FARCONST aChildAC87Accept1[] = {{UifAC87IssueRpt, CID_AC87ISSUERPT},
                                       {UifAC87EnterAddr, CID_AC87ENTERADDR},
                                       {NULL,             0             }};


/* Define Next Function at UIM_ACCEPTED from CID_ACCHANGEKB2 */

UIMENU FARCONST aChildAC87Accept2[] = {{UifAC87EnterAddr, CID_AC87ENTERADDR},
                                       {NULL,             0             }};


/* Define Next Function at UIM_ACCEPTED from CID_AC87ENTERALPHA,CID_AC87ENTERADDR */

UIMENU FARCONST aChildAC87Accept3[] = {{UifACShift, CID_ACSHIFT},
                                       {UifAC87EnterALPHA, CID_AC87ENTERALPHA},
                                       {UifACChangeKB2, CID_ACCHANGEKB2},
                                       {NULL,           0            }};



SHORT UifAC87Function(KEYMSG *pKeyMsg) 
{

    PARASOFTCHK  ParaSoftChk;


    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        /* Display Address 1 of This Function */

        ParaSoftChk.uchStatus = 0;                             /* Set W/ Amount Status */
        ParaSoftChk.uchAddress = 1;                            /* Set Address 1 */
        ParaSoftChk.uchMajorClass = CLASS_PARASOFTCHK;         /* Set Major Class */
        MaintSoftChkMsgRead(&ParaSoftChk);                     /* Execute Read Mnemonics Procedure */

        UieNextMenu(aChildAC87Init);                            /* Open Next Function */
        break;

    case UIM_ACCEPTED:
        switch (pKeyMsg->SEL.usFunc) {
        case CID_ACCHANGEKB1:
            UieNextMenu(aChildAC87Accept1);                     /* Open Next Function */
            break;

        case CID_ACCHANGEKB2:
            UieNextMenu(aChildAC87Accept2);                     /* Open Next Function */
            break;

        default:                                                /* CID_AC87ENTERALPHA */
            UieNextMenu(aChildAC87Accept3);                     /* CID_AC87ENTERADDR */
            break;                                              
        }
        break;

    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC87EnterALPHA(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: A/C No.87 Enter Mnemonics Data Mode 
*===============================================================================
*/

SHORT UifAC87EnterALPHA(KEYMSG *pKeyMsg) 
{

    SHORT           sError;
    PARASOFTCHK     ParaSoftChk;


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_P1);                            /* Register Key Sequence */
        UieEchoBack(UIE_ECHO_ROW1_AN, PARA_SOFTCHK_LEN);            /* Define Display Format and Max. Input Data */

        /* Set KB Mode to ALPHA Mode */

        UifACChgKBType(UIF_ALPHA_KB);
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P1:
            ParaSoftChk.uchStatus = 0;                              /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount Input Case */
                ParaSoftChk.uchStatus |= MAINT_WITHOUT_DATA;        /* Set W/o Amount Status */
            } else {                                                /* W/ Amount Input Case */
                memset(ParaSoftChk.aszMnemonics,                    /* Full NULL to Own Buffer */
                       '\0',
                       (PARA_SOFTCHK_LEN+1) * sizeof(TCHAR));

                _tcsncpy(ParaSoftChk.aszMnemonics,                    /* Copy Input Mnemonics to Own Buffer */
                       pKeyMsg->SEL.INPUT.aszKey,
                       pKeyMsg->SEL.INPUT.uchLen);
                //memcpy(ParaSoftChk.aszMnemonics,                    /* Copy Input Mnemonics to Own Buffer */
                //       pKeyMsg->SEL.INPUT.aszKey,
                //       pKeyMsg->SEL.INPUT.uchLen);
            }

            ParaSoftChk.uchMajorClass = CLASS_PARASOFTCHK;          /* Set Major Class */

            if ((sError = MaintSoftChkMsgWrite(&ParaSoftChk)) == SUCCESS) {    
                UieAccept();                                        /* Return to UifAC87Function() */
            }
            return(sError);                                        

        /* default:
              break; */

        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                              /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC87IssueRpt(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error 
*               
**  Description: A/C No.87 Issue Report Mode 
*===============================================================================
*/

SHORT UifAC87IssueRpt(KEYMSG *pKeyMsg) 
{
    
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        /* Check Digit */

        if (pKeyMsg->SEL.INPUT.uchLen) {                /* W/ Amount Case */
            return(LDT_SEQERR_ADR);
        }
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
            case FSC_AC:
                MaintSoftChkMsgReport();                /* Execute Report Procedure */
                UieExit(aACEnter);                      /* Return to UifACEnter() */
                return(SUCCESS);
            
            /* default:
                  break; */

        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                  /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC87EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error 
*               
**  Description: A/C No.87 Enter Address Mode 
*===============================================================================
*/

/* Define Key Sequence */

UISEQ FARCONST aszSeqAC87EnterAddr[] = {FSC_NUM_TYPE, FSC_AC, 0};

    

SHORT UifAC87EnterAddr(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARASOFTCHK     ParaSoftChk;


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC87EnterAddr);               /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen) {                /* W/ Amount */
                return(LDT_SEQERR_ADR);
            }
            MaintFin(CLASS_MAINTTRAILER_PRTSUP);
            UieExit(aACEnter);                              /* Return to UifACEnter() */
            return(SUCCESS);

        case FSC_NUM_TYPE:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            ParaSoftChk.uchStatus = 0;                      /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                         /* W/o Amount Input Case */
                ParaSoftChk.uchStatus |= MAINT_WITHOUT_DATA;          /* Set W/o Amount Status */
            } else {                                                  /* W/ Amount Input Case */
                ParaSoftChk.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            ParaSoftChk.uchMajorClass = CLASS_PARASOFTCHK;            /* Set Major Class */

            if ((sError = MaintSoftChkMsgRead(&ParaSoftChk)) == SUCCESS) {    
                UieAccept();                                            /* Return to UifAC87Function() */
            }
            return(sError);                                             /* Set Success or Input Data Error */
            
        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                                  /* Execute Default Procedure */
    }
}




    
    
    

