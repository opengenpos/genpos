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
* Title       : Cashier A/B Key Assignment Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC7.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC7Function()        : A/C No.7 Function Entry Mode
*               UifAC7IssueRpt()        : A/C No.7 Issue Report Mode
*               UifAC7EnterData1()      : A/C No.7 Enter Data1 Mode
*               UifAC7EnterData2()      : A/C No.7 Enter Data2 Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jun-16-92:00.00.01    :K.You      : initial                                   
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
#include <uifsup.h>
#include <uireg.h>
#include <uifpgequ.h>

#include "uifsupex.h"                       /* Unique Extern Header for UI */




/*
*=============================================================================
**  Synopsis: SHORT UifAC7Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.7 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC7Init[] = {{UifAC7IssueRpt, CID_AC7ISSUERPT},
                                   {UifAC7EnterData1, CID_AC7ENTERDATA1},
                                   {NULL,             0                 }};


/* Define Next Function at UIM_ACCEPTED */

UIMENU FARCONST aChildAC7Accept[] = {{UifAC7EnterData2, CID_AC7ENTERDATA2},
                                     {NULL,             0                 }};



SHORT UifAC7Function(KEYMSG *pKeyMsg) 
{
    PARACASHABASSIGN    ParaCashAB;   
                        
    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC7Init);                                 /* Open Next Function */

        /* Display Address 1 of This Function */
        ParaCashAB.uchStatus = 0;                                   /* Set W/ Amount Status */
        ParaCashAB.uchAddress = 1;                                  /* Set Address 1 */
        ParaCashAB.uchMajorClass = CLASS_PARACASHABASSIGN;          /* Set Major Class */

        MaintCashABAssignRead(&ParaCashAB);                         
        break;

    case UIM_ACCEPTED:
        UieNextMenu(aChildAC7Accept);                               /* Open Next Function */
        break;

    default:
        return(UifACDefProc(pKeyMsg));                              /* execute Default Procedure */
    }
    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC7IssueRpt(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.7 Issue Report Mode 
*===============================================================================
*/
SHORT UifAC7IssueRpt(KEYMSG *pKeyMsg) 
{
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                                /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            if (pKeyMsg->SEL.INPUT.uchLen) {                            /* W/ Amount Case */
                return(LDT_SEQERR_ADR);
            }

            MaintCashABAssignReport();                                  /* Execute Report Procedure */
            UieExit(aACEnter);                                          /* Return to UifACEnter() */
            return(SUCCESS);

        /* default:
              break;  */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                                  /* Execute Default Procedure */
    }
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC7EnterData1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.7 Enter Data1 Mode 
*===============================================================================
*/

/* Define Key Sequence */

UISEQ FARCONST   aszSeqAC7EnterData1[] = {FSC_SIGN_IN, FSC_B, 0};
    

SHORT UifAC7EnterData1(KEYMSG *pKeyMsg) 
{
    SHORT               sError;
    PARACASHABASSIGN    ParaCashAB;
                   
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC7EnterData1);                                   /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        /* Check W/o Amount */
        if (!pKeyMsg->SEL.INPUT.uchLen) {                                       /* W/o Amount Case */
            return(LDT_SEQERR_ADR);
        }

        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_SIGN_IN:
            /* Check number of digits */
            if (pKeyMsg->SEL.INPUT.uchLen > UIFREG_MAX_ID_DIGITS) {                /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            ParaCashAB.uchMajorClass = CLASS_PARACASHABASSIGN;                  /* Set Major Class */
            ParaCashAB.uchAddress = ASN_AKEY_ADR;
            ParaCashAB.uchStatus = 0;                                   /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount Input Case */
                ParaCashAB.uchStatus |= MAINT_WITHOUT_DATA;             /* Set W/o Amount Status */
            } else {                                                    /* W/ Amount Input Case */
                ParaCashAB.ulCashierNo = (ULONG)pKeyMsg->SEL.INPUT.lData;
            }
            if ((sError = MaintCashABAssignWrite(&ParaCashAB)) == SUCCESS) {    /* Read Function Completed */
                UieAccept();                                                    /* Return to UifAC7Function() */
            }
            return(sError);

        case FSC_B:
            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIFREG_MAX_ID_DIGITS) {                /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            ParaCashAB.uchMajorClass = CLASS_PARACASHABASSIGN;                  /* Set Major Class */
            ParaCashAB.uchAddress = ASN_BKEY_ADR;
            ParaCashAB.uchStatus = 0;                                   /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount Input Case */
                ParaCashAB.uchStatus |= MAINT_WITHOUT_DATA;             /* Set W/o Amount Status */
            } else {                                                    /* W/ Amount Input Case */
                ParaCashAB.ulCashierNo = (ULONG)pKeyMsg->SEL.INPUT.lData;
            }
            if ((sError = MaintCashABAssignWrite(&ParaCashAB)) == SUCCESS) {    /* Read Function Completed */
                UieAccept();                                                    /* Return to UifAC7Function() */
            }
            return(sError);

        /* default:
              break;  */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                              /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC7EnterData2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.7 Enter Data2 Mode 
*===============================================================================
*/

/* Define Key Sequence */

UISEQ FARCONST   aszSeqAC7EnterData2[] = {FSC_AC, FSC_SIGN_IN, FSC_B, 0};
    

SHORT UifAC7EnterData2(KEYMSG *pKeyMsg) 
{
    SHORT               sError;
    PARACASHABASSIGN    ParaCashAB;
                   
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC7EnterData2);                               /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Check W/ Amount */
            if (pKeyMsg->SEL.INPUT.uchLen) {                                    /* W/ Amount Case */
                return(LDT_SEQERR_ADR);
            }
            MaintFin(CLASS_MAINTTRAILER_PRTSUP);
            UieExit(aACEnter);
            return(SUCCESS);
            
        case FSC_SIGN_IN:
            /* Check W/ Amount */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                                   /* W/o Amount Case */
                return(LDT_SEQERR_ADR);
            }

            /* Check number of Digits */
            if (pKeyMsg->SEL.INPUT.uchLen > UIFREG_MAX_ID_DIGITS) {                       /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            ParaCashAB.uchMajorClass = CLASS_PARACASHABASSIGN;                  /* Set Major Class */
            ParaCashAB.uchAddress = ASN_AKEY_ADR;
            ParaCashAB.uchStatus = 0;                     /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount Input Case */
                ParaCashAB.uchStatus |= MAINT_WITHOUT_DATA;              /* Set W/o Amount Status */
            } else {                                                    /* W/ Amount Input Case */
                ParaCashAB.ulCashierNo = (ULONG)pKeyMsg->SEL.INPUT.lData;
            }
            if ((sError = MaintCashABAssignWrite(&ParaCashAB)) == SUCCESS) {    /* Read Function Completed */
                UieAccept();                                                    /* Return to UifAC7Function() */
            }
            return(sError);

        case FSC_B:
            /* Check W/ Amount */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                                   /* W/o Amount Case */
                return(LDT_SEQERR_ADR);
            }

            /* Check number of Digits */
            if (pKeyMsg->SEL.INPUT.uchLen > UIFREG_MAX_ID_DIGITS) {                       /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            ParaCashAB.uchMajorClass = CLASS_PARACASHABASSIGN;                  /* Set Major Class */
            ParaCashAB.uchAddress = ASN_BKEY_ADR;
            ParaCashAB.uchStatus = 0;                     /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount Input Case */
                ParaCashAB.uchStatus |= MAINT_WITHOUT_DATA;              /* Set W/o Amount Status */
            } else {                                                    /* W/ Amount Input Case */
                ParaCashAB.ulCashierNo = (ULONG)pKeyMsg->SEL.INPUT.lData;
            }
            if ((sError = MaintCashABAssignWrite(&ParaCashAB)) == SUCCESS) {    /* Read Function Completed */
                UieAccept();                                                    /* Return to UifAC7Function() */
            }
            return(sError);

        /* default:
              break;  */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                              /* Execute Default Procedure */
    }
}

