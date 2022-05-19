/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Set PLU No. of Menu Page Module
* Category    : User Interface For Supervisor, NCR 2170 US GP Application
* Program Name: UIFAC4.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Midium Model
* Options     : /c /AM /W4 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:
*
*               UifAC4Function()        : A/C No.4 Function Entry Mode
*               UifAC4IssueRpt()        : A/C No.4 Issue Report Mode
*               UifAC4EnterData1()      : A/C No.4 Enter Data1 Mode
*               UifAC4EnterData2()      : A/C No.4 Enter Data2 Mode
* --------------------------------------------------------------------------
* Update Histories
*    Date  : Ver.Rev.   :   Name    : Description
* Feb-16-93:00.00.01    :M.Ozawa    : initial
* Jun-23-93:00.00.02    :M.Ozawa    : Change Keyed Dept -> Keyed PLU
* Jul-12-93:00.00.03    :S.Liu      : Add PLU(13-Line), Scanner
*                                     and E-Version function
* Oct-03-96:02.00.00    :Y.Sakuma   : Change declaration for UPC Velocity Code
*                                   : Change UifAC4EnterData1() for
*                                     UPC Velociy Code
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
#include <plu.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>
#include <maint.h>
#include <uifsup.h>
#include <uifpgequ.h>
#include <rfl.h>

#include "uifsupex.h"                       /* Unique Extern Header for UI */

/*
==============================================================================
;                      C O M M O N   R O M   T A B L E s
;=============================================================================
*/


/* Define Key Sequence */

UISEQ FARCONST aszSeqACFSC_SHIFT[] = {FSC_MENU_SHIFT, FSC_D_MENU_SHIFT, 0};


/*
*=============================================================================
**  Synopsis: SHORT UifAC4Function(KEYMSG *pKeyMsg)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*
**  Description: A/C No.4 Function Entry Mode
*===============================================================================
*/

/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC4Init[] = {{UifAC4IssueRpt, CID_AC4ISSUERPT},
                                   {UifAC4EnterData1, CID_AC4ENTERDATA1},
                                   {UifAC4EnterData2, CID_AC4ENTERDATA2},
                                   {NULL,             0                }};


/* Define Next Function at UIM_ACCEPTED */

UIMENU FARCONST aChildAC4Accept[] = {{UifAC4EnterData1, CID_AC4ENTERDATA1},
                                     {UifAC4EnterData2, CID_AC4ENTERDATA2},
                                     {UifACExitFunction, CID_AC4EXITFUNCTION},
                                     {NULL,              0                }};


SHORT UifAC4Function(KEYMSG *pKeyMsg)
{

    MAINTSETPAGE        SetPage;
    PARAPLUNOMENU       PLUNoMenu;

    /* Clear Work */

    memset(&SetPage, '\0', sizeof(MAINTSETPAGE));
    memset(&PLUNoMenu, '\0', sizeof(PARAPLUNOMENU));

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
    
        /* relocate plu no., 1/26/98 */
        MaintPLUNoMenuReplace();

        UieNextMenu(aChildAC4Init);                                 /* Open Next Function */

        /* Set Menu Page to 1 */
#if 0
        SetPage.uchStatus = 0;                                      /* Set W/ Amount Status */
        SetPage.uchPageNumber = 1;                                  /* Set Page */
        SetPage.uchMajorClass = CLASS_MAINTSETPAGE;                 /* Set Major Class */
        MaintSetPageOnly(&SetPage);                                 /* Execute Set Procedure */
#endif
        /* Display Address 1 of This Function */

        PLUNoMenu.uchStatus = 0;                                    /* Set W/ Amount Status */
        PLUNoMenu.uchMinorFSCData = 1;                              /* Set MinorFSCData 1 */
        PLUNoMenu.uchMajorClass = CLASS_PARAPLUNOMENU;              /* Set Major Class */
        PLUNoMenu.uchMinorClass = CLASS_PARAPLUNOMENU_ADDRESS;      /* Set Minor Class */
        MaintPLUNoMenuRead(&PLUNoMenu);                             /* Execute Read Procedure */
        break;

    case UIM_ACCEPTED:
        UieNextMenu(aChildAC4Accept);                               /* Open Next Function */
        break;
/******
    case UIM_CANCEL:
        MaintPLUNoMenuRedisplay();                                 / Redisplay Initial Data /
        break;
******/
    default:
        return(UifACDefProc(pKeyMsg));                              /* execute Default Procedure */
    }
    return(SUCCESS);
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC4IssueRpt(KEYMSG *pKeyMsg)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*
**  Description: A/C No.4 Issue Report Mode
*===============================================================================
*/

SHORT UifAC4IssueRpt(KEYMSG *pKeyMsg)
{

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);        /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            if (pKeyMsg->SEL.INPUT.uchLen) {    /* W/ Amount */
                return(LDT_SEQERR_ADR);
            }
            MaintPLUNoMenuReport();             /* Issue Report */
            UieExit(aACEnter);                  /* Return to UifACEnter() */
            return(SUCCESS);

        /* default:
              break;  */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));          /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC4EnterData1(KEYMSG *pKeyMsg)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message
*      Output:  nothing
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*
**  Description: A/C No.4 Enter Data1 Mode
*===============================================================================
*/

/* Define Key Sequence */

/* --- Change for R2.0 (Add FSC_VELOCITY) --- */
UISEQ FARCONST aszSeqAC4EnterData1[] = {FSC_SCANNER, FSC_E_VERSION,
                                        FSC_KEYED_PLU, FSC_NUM_TYPE, FSC_PLU,
                                        0};
/* UISEQ FARCONST aszSeqAC4EnterData1[] = {FSC_SCANNER, FSC_ADJECTIVE,FSC_E_VERSION,
                                        FSC_KEYED_PLU, FSC_NUM_TYPE, FSC_PLU,
                                        FSC_VELOCITY, 0}; */

SHORT UifAC4EnterData1(KEYMSG *pKeyMsg)
{

    SHORT           sError;
/*    PARAPLU         ParaPLU; */
    PARAPLUNOMENU   PLUNoMenu;

/*  memset(&ParaPLU,0,sizeof(PARAPLU)); */
    memset(&PLUNoMenu,0,sizeof(PARAPLUNOMENU));

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC4EnterData1);                           /* Register Key Sequence */
        UieEchoBack(UIE_ECHO_ROW0_NUMBER,UIF_DIGIT14);
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_SCANNER:
            if((pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen > UIF_DIGIT14) ||  /* Over digit */
               (pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen == 0)) {           /* or no entry */
                return(LDT_KEYOVER_ADR);
            }

            PLUNoMenu.uchMajorClass = CLASS_PARAPLUNOMENU;              /* Set Major Class */
            PLUNoMenu.uchMinorClass = CLASS_PARAPLUNOMENU_PLUNO;        /* Set Minor Class */

            _tcsncpy(PLUNoMenu.PluNo.aszPLUNumber, pKeyMsg->SEL.INPUT.DEV.SCA.auchStr, pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen);
            //memcpy(PLUNoMenu.PluNo.aszPLUNumber, pKeyMsg->SEL.INPUT.DEV.SCA.auchStr, pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen);

            PLUNoMenu.uchStatus = 0;                                    /* Set W/ Amount Status */
            PLUNoMenu.uchStatus |= MAINT_SCANNER_INPUT;

            if ((sError=MaintPLUNoMenuWrite(&PLUNoMenu)) == SUCCESS) {  /* Read Function Completed */
                UieAccept();                        /* return to parent function with UIM_ACCEPTED */
            }
            return(sError);
#if 0
        case FSC_ADJECTIVE:
            if (pKeyMsg->SEL.INPUT.uchLen) {
                return(LDT_SEQERR_ADR);
            }
            PLUNoMenu.uchMajorClass = CLASS_PARAPLUNOMENU;
            PLUNoMenu.uchMinorClass = CLASS_PARAPLUNOMENU_ADJECTIVE;
            PLUNoMenu.PluNo.uchAdjective = pKeyMsg->SEL.INPUT.uchMinor;
            PLUNoMenu.uchStatus = MAINT_WITHOUT_DATA;
            if ((sError = MaintPLUNoMenuEdit(&PLUNoMenu)) == SUCCESS) {
                UieReject();                                    /* Return to UifAC4Function() */
            }
            return(sError);
            break;
#endif
        case FSC_E_VERSION:
            if (pKeyMsg->SEL.INPUT.uchLen) {                            /* With Amount ? */
                return(LDT_SEQERR_ADR);
            }
            PLUNoMenu.uchMajorClass = CLASS_PARAPLUNOMENU;
            PLUNoMenu.uchMinorClass = CLASS_PARAPLUNOMENU_EVERSION;
            PLUNoMenu.PluNo.uchModStat=MOD_STAT_EVERSION;
            PLUNoMenu.uchStatus = MAINT_WITHOUT_DATA;
            if ((sError = MaintPLUNoMenuEdit(&PLUNoMenu)) == SUCCESS) {
                UieReject();                                    /* Return to UifAC4Function() */
            }
            return(sError);
            break;

        case FSC_KEYED_PLU:
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT14) {              /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            PLUNoMenu.uchMajorClass = CLASS_PARAPLUNOMENU;              /* Set Major Class */
            PLUNoMenu.uchMinorClass = CLASS_PARAPLUNOMENU_KEYEDPLU;     /* Set Minor Class */
            PLUNoMenu.uchMinorFSCData = pKeyMsg->SEL.INPUT.uchMinor;    /* Set Minor FSC */
            PLUNoMenu.uchStatus = 0;                            /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                   /* W/o Amount Input Case */
                PLUNoMenu.uchStatus |= MAINT_WITHOUT_DATA;      /* Set W/o Amount Status */
            } else {                                            /* W/ Amount Input Case */
                _tcscpy(PLUNoMenu.PluNo.aszPLUNumber,pKeyMsg->SEL.INPUT.aszKey);
            }
            if ((sError = MaintPLUNoMenuWrite(&PLUNoMenu)) == SUCCESS) {
                UieAccept();                                    /* Return to UifAC4Function() */
            }
            return(sError);

        case FSC_NUM_TYPE:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT3) {           /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            if (pKeyMsg->SEL.INPUT.lData > 255L) {                  /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            PLUNoMenu.uchMajorClass = CLASS_PARAPLUNOMENU;          /* Set Major Class */
            PLUNoMenu.uchMinorClass = CLASS_PARAPLUNOMENU_ADDRESS;  /* Set Minor Class */
            PLUNoMenu.uchStatus = 0;                                /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                   /* W/o Amount Input Case */
                PLUNoMenu.uchStatus |= MAINT_WITHOUT_DATA;      /* Set W/o Amount Status */
            } else {                                            /* W/ Amount Input Case */
                PLUNoMenu.uchMinorFSCData = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }
            if ((sError = MaintPLUNoMenuRead(&PLUNoMenu)) == SUCCESS) {
                UieAccept();                                    /* Return to UifAC4Function() */
            }
            return(sError);
#if 0
        /* --- Addition for R2.0 --- */
        case FSC_VELOCITY:
            PLUNoMenu.uchMajorClass = CLASS_PARAPLUNOMENU;
            PLUNoMenu.uchMinorClass = CLASS_PARAPLUNOMENU_VELOCITY;
            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/O Amount ? */
                PLUNoMenu.uchStatus = MAINT_WITHOUT_DATA;
            }
            else if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT6) {          /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            if ((sError = MaintPLUNoMenuEdit(&PLUNoMenu)) != SUCCESS) {
                return(sError);
            }
            if (!pKeyMsg->SEL.INPUT.uchLen) {                   /* W/O Amount ? */
                UieReject();                                    /* Return to UifAC4Function() */
                return(SUCCESS);
                break;
            }
            /* break; */
        /* --- End of addition for R2.0 --- */
#endif
        case FSC_PLU:
            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT14) {              /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            PLUNoMenu.uchMajorClass = CLASS_PARAPLUNOMENU;              /* Set Major Class */
            PLUNoMenu.uchMinorClass = CLASS_PARAPLUNOMENU_PLUNO;        /* Set Minor Class */
            PLUNoMenu.uchStatus = 0;                                    /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount Input Case */
                PLUNoMenu.uchStatus |= MAINT_WITHOUT_DATA;              /* Set W/o Amount Status */
            } else {                                                    /* W/ Amount Input Case */
                _tcscpy(PLUNoMenu.PluNo.aszPLUNumber,pKeyMsg->SEL.INPUT.aszKey);
            }
            if ((sError = MaintPLUNoMenuWrite(&PLUNoMenu)) == SUCCESS) {
                UieAccept();                                    /* Return to UifAC4Function() */
            }
            return(sError);

        /* default:
              break;  */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                          /* Execute Default Procedure */

    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC4EnterData2(KEYMSG *pKeyMsg)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message
*      Output:  nothing
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*
**  Description: A/C No.4 Enter Data2 Mode
*===============================================================================
*/

SHORT UifAC4EnterData2(KEYMSG *pKeyMsg)
{

    SHORT              sError;
    MAINTSETPAGE       SetPage;
    PARAPLUNOMENU      PLUNoMenu;
    FSCTBL FAR     *pData;

    memset(&PLUNoMenu,0,sizeof(PARAPLUNOMENU));

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_SHIFT);                                     /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_MENU_SHIFT:
#ifdef	___DEL_2172	/* ### 2172 Rel1.0 (01/13/00) */
            if (pKeyMsg->SEL.INPUT.uchLen) {    /* not amount entry */
                return(LDT_SEQERR_ADR);
            }
#endif	/* __DEL2172 */
            /* Clear SetPage Work */

            memset(&SetPage, '\0', sizeof(MAINTSETPAGE));
            SetPage.uchMajorClass = CLASS_MAINTSETPAGE;                 /* Set Major Class */
            SetPage.uchStatus = 0;                                      /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount Input Case */
                SetPage.uchStatus |= MAINT_WITHOUT_DATA;                /* Set W/o Amount Status */
            } else {                                                    /* W/ Amount Input Case */
                SetPage.uchPageNumber = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }
            if ((sError = MaintSetPageOnly(&SetPage)) == SUCCESS) {
                PLUNoMenu.uchStatus = 0;                                       /* Set W/ Amount Status */
                PLUNoMenu.uchMinorFSCData = 1;                                 /* Set MinorFSCData 1 */
                PLUNoMenu.uchMajorClass = CLASS_PARAPLUNOMENU;                /* Set Major Class */
                PLUNoMenu.uchMinorClass = CLASS_PARAPLUNOMENU_ADDRESS;      /* Set Minor Class */
                MaintPLUNoMenuRead(&PLUNoMenu);                               /* Execute Read Procedure */
                pData = (FSCTBL FAR *)&ParaFSC[uchMaintMenuPage - 1];
                UieSetFsc(pData);
                UieAccept();                                                    /* Return to UifAC4Function() */
            }
            return(sError);

        case FSC_D_MENU_SHIFT:
            /* Check W/o Amount */

            if (pKeyMsg->SEL.INPUT.uchLen) {                    /* W/ Amount */
                return(LDT_SEQERR_ADR);
            }

            /* Clear SetPage Work */
            
            memset(&SetPage, '\0', sizeof(MAINTSETPAGE));
            SetPage.uchMajorClass = CLASS_MAINTSETPAGE;                 /* Set Major Class */
            SetPage.uchPageNumber = pKeyMsg->SEL.INPUT.uchMinor;        /* set page number */
            SetPage.uchStatus = 0;                                      /* Set W/ Amount Status */

            if ((sError = MaintSetPageOnly(&SetPage)) == SUCCESS) { 
                PLUNoMenu.uchStatus = 0;                                        /* Set W/ Amount Status */
                PLUNoMenu.uchMinorFSCData = 1;                                  /* Set MinorFSCData 1 */
                PLUNoMenu.uchMajorClass = CLASS_PARAPLUNOMENU;                  /* Set Major Class */
                MaintPLUNoMenuRead(&PLUNoMenu);                                 /* Execute Read Procedure */
                pData = (FSCTBL FAR *)&ParaFSC[uchMaintMenuPage - 1];
                UieSetFsc(pData);
                UieAccept();                                                    /* Return to UifAC4Function() */
            }
            return(sError);

        /* default:
              break;  */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                                          /* Execute Default Procedure */
    }
}

/*****  End of uifac4.c  *****/
