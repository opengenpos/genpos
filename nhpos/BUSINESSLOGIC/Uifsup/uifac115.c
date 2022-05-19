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
* Title       : Set Dept No. of Menu Page Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC115.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC115Function()        : A/C No.115 Function Entry Mode
*               UifAC115IssueRpt()        : A/C No.115 Issue Report Mode
*               UifAC115EnterData1()      : A/C No.115 Enter Data1 Mode
*               UifAC115EnterData2()      : A/C No.115 Enter Data2 Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Nov-26-99:00.00.01    :M.Ozawa    : initial                                   
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

#include "uifsupex.h"                       /* Unique Extern Header for UI */

/*
*=============================================================================
**  Synopsis: SHORT UifAC115Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.115 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC115Init[] = {{UifAC115IssueRpt, CID_AC115ISSUERPT},
                                   {UifAC115EnterData1, CID_AC115ENTERDATA1},
                                   {UifAC115EnterData2, CID_AC115ENTERDATA2},
                                   {NULL,             0                }};


/* Define Next Function at UIM_ACCEPTED */

UIMENU FARCONST aChildAC115Accept[] = {{UifACExitFunction, CID_AC115EXITFUNCTION},
                                     {UifAC115EnterData1, CID_AC115ENTERDATA1},
                                     {UifAC115EnterData2, CID_AC115ENTERDATA2},
                                     {NULL,             0                }};



SHORT UifAC115Function(KEYMSG *pKeyMsg) 
{


    MAINTSETPAGE        SetPage;
    PARADEPTNOMENU       DeptNoMenu;
    
    
    /* Clear SetPage Work */
            
    memset(&SetPage, '\0', sizeof(MAINTSETPAGE));

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:

        /* relocate plu no., 1/26/98 */
        MaintDeptNoMenuReplace();
        
        UieNextMenu(aChildAC115Init);                                 /* Open Next Function */

        /* Set Menu Page to 1 */

/*        SetPage.uchStatus = 0;                                    */  /* Set W/ Amount Status */
/*        SetPage.uchPageNumber = 1;                                */  /* Set Page */
/*        SetPage.uchMajorClass = CLASS_MAINTSETPAGE;               */  /* Set Major Class */
/*        MaintSetPageOnly(&SetPage);                               */  /* Execute Set Procedure */

        /* Display Address 1 of This Function */

        DeptNoMenu.uchStatus = 0;                                    /* Set W/ Amount Status */
        DeptNoMenu.uchMinorFSCData = 1;                              /* Set MinorFSCData 1 */
        DeptNoMenu.uchMajorClass = CLASS_PARADEPTNOMENU;              /* Set Major Class */
        MaintDeptNoMenuRead(&DeptNoMenu);                             /* Execute Read Procedure */
        break;

    case UIM_ACCEPTED:
        UieNextMenu(aChildAC115Accept);                               /* Open Next Function */
        break;

    default:
        return(UifACDefProc(pKeyMsg));                              /* execute Default Procedure */
    }
    return(SUCCESS);
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC115IssueRpt(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.115 Issue Report Mode 
*===============================================================================
*/


SHORT UifAC115IssueRpt(KEYMSG *pKeyMsg) 
{
    


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                        /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Check W/o Amount */

            if (pKeyMsg->SEL.INPUT.uchLen) {                    /* W/ Amount */
                return(LDT_SEQERR_ADR);
            }

            /* Issue Report */

            MaintDeptNoMenuReport();
            UieExit(aACEnter);                                  /* Return to UifACEnter() */
            return(SUCCESS);                                    
            
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
**  Synopsis: SHORT UifAC115EnterData1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.115 Enter Data1 Mode 
*===============================================================================
*/

    
/* Define Key Sequence */

UISEQ FARCONST aszSeqAC115EnterData1[] = {FSC_KEYED_DEPT, FSC_NUM_TYPE, FSC_DEPT, 0};


SHORT UifAC115EnterData1(KEYMSG *pKeyMsg) 
{
    
    SHORT          sError;
    PARADEPTNOMENU  DeptNoMenu;
                   


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC115EnterData1);                                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_KEYED_DEPT:
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT4) {                   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            DeptNoMenu.uchMajorClass = CLASS_PARADEPTNOMENU;                  /* Set Major Class */
            DeptNoMenu.uchMinorClass = CLASS_PARADEPTNOMENU_KEYEDDEPT;         /* Set Minor Class */
            DeptNoMenu.uchMinorFSCData = pKeyMsg->SEL.INPUT.uchMinor;        /* Set Minor FSC */
            DeptNoMenu.uchStatus = 0;                     /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount Input Case */
                DeptNoMenu.uchStatus |= MAINT_WITHOUT_DATA;              /* Set W/o Amount Status */
            } else {                                                    /* W/ Amount Input Case */
                DeptNoMenu.usDeptNumber = ( USHORT)pKeyMsg->SEL.INPUT.lData;
            }
            if ((sError = MaintDeptNoMenuWrite(&DeptNoMenu)) == SUCCESS) {                                                                            
                UieAccept();                                                /* Return to UifAC115Function() */                                        
            }
            return(sError);

        case FSC_NUM_TYPE:
            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.lData > 50L) {                       /* Over Digit, Saratoga */
                return(LDT_KEYOVER_ADR);
            }
            DeptNoMenu.uchMajorClass = CLASS_PARADEPTNOMENU;                  /* Set Major Class */
            DeptNoMenu.uchStatus = 0;                     /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount Input Case */
                DeptNoMenu.uchStatus |= MAINT_WITHOUT_DATA;              /* Set W/o Amount Status */
            } else {                                                    /* W/ Amount Input Case */
                DeptNoMenu.uchMinorFSCData = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }
            if ((sError = MaintDeptNoMenuRead(&DeptNoMenu)) == SUCCESS) {     
                UieAccept();                                                /* Return to UifAC115Function() */
            }
            return(sError);

        case FSC_DEPT:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT4) {                   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            DeptNoMenu.uchMajorClass = CLASS_PARADEPTNOMENU;                  /* Set Major Class */
            DeptNoMenu.uchMinorClass = CLASS_PARADEPTNOMENU_DEPTNO;            /* Set Minor Class */
            DeptNoMenu.uchStatus = 0;                     /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount Input Case */
                DeptNoMenu.uchStatus |= MAINT_WITHOUT_DATA;              /* Set W/o Amount Status */
            } else {                                                    /* W/ Amount Input Case */
                DeptNoMenu.usDeptNumber = ( USHORT)pKeyMsg->SEL.INPUT.lData;
            }
            if ((sError = MaintDeptNoMenuWrite(&DeptNoMenu)) == SUCCESS) {        
                UieAccept();                                                /* Return to UifAC115Function() */
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
**  Synopsis: SHORT UifAC115EnterData2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: A/C No.115 Enter Data2 Mode 
*===============================================================================
*/
    
/* Define Key Sequence */

UISEQ FARCONST aszSeqAC115EnterData2[] = {FSC_MENU_SHIFT, FSC_D_MENU_SHIFT, 0};


SHORT UifAC115EnterData2(KEYMSG *pKeyMsg) 
{
    
    SHORT          sError;
    MAINTSETPAGE   SetPage;
    PARADEPTNOMENU  DeptNoMenu;
    FSCTBL FAR     *pData;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC115EnterData2);                                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_MENU_SHIFT:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {                       /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }

            /* Clear SetPage Work */
            
            memset(&SetPage, '\0', sizeof(MAINTSETPAGE));
            SetPage.uchMajorClass = CLASS_MAINTSETPAGE;                         /* Set Major Class */
            SetPage.uchStatus = 0;                                      /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount Input Case */
                SetPage.uchStatus |= MAINT_WITHOUT_DATA;                /* Set W/o Amount Status */
            } else {                                                    /* W/ Amount Input Case */
                SetPage.uchPageNumber = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }
            if ((sError = MaintSetPageOnly(&SetPage)) == SUCCESS) { 
                DeptNoMenu.uchStatus = 0;                                        /* Set W/ Amount Status */
                DeptNoMenu.uchMinorFSCData = 1;                                  /* Set MinorFSCData 1 */
                DeptNoMenu.uchMajorClass = CLASS_PARADEPTNOMENU;                  /* Set Major Class */
                MaintDeptNoMenuRead(&DeptNoMenu);                                 /* Execute Read Procedure */
                /* pData = (FSCTBL FAR *)&ParaFSC1[uchMaintMenuPage - 1]; */
                pData = (FSCTBL FAR *)&ParaFSC[uchMaintMenuPage - 1];
                UieSetFsc(pData);
                UieAccept();                                                    /* Return to UifAC115Function() */
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
                DeptNoMenu.uchStatus = 0;                                        /* Set W/ Amount Status */
                DeptNoMenu.uchMinorFSCData = 1;                                  /* Set MinorFSCData 1 */
                DeptNoMenu.uchMajorClass = CLASS_PARADEPTNOMENU;                  /* Set Major Class */
                MaintDeptNoMenuRead(&DeptNoMenu);                                 /* Execute Read Procedure */
                /* pData = (FSCTBL FAR *)&ParaFSC1[uchMaintMenuPage - 1]; */
                pData = (FSCTBL FAR *)&ParaFSC[uchMaintMenuPage - 1];
                UieSetFsc(pData);
                UieAccept();                                                    /* Return to UifAC115Function() */
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

