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
* Title       : Assign Set Menu Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC116.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC116Function()      : A/C No.116 Function Entry Mode
*               UifAC116IssueRpt()      : A/C No.116 Issue Report Mode
*               UifAC116EnterData1()    : A/C No.116 Enter Data1 Mode
*               UifAC116EnterData2()    : A/C No.116 Enter Data2 Mode
*               UifAC116EnterAdject1()  : A/C No.116 Enter Adjective Data1 Mode
*               UifAC116EnterAdject2()  : A/C No.116 Enter Adjective Data2 Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
*    Date  : Ver.Rev.  :   Name    : Description
* Jul-10-92:00.00.01   :K.You      : initial                                   
* Jul-21-93:01.00.12   :J.IKEDA    : Adds Direct Menu Shift in UifAC116EnterData1()    
*          :           :           : & UifAC116EnterData2()                                
* Apr-14-95:03.00.00   :T.Satoh    : Adds clear MLD
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
#include <mldmenu.h>

#include "uifsupex.h"                       /* Unique Extern Header for UI */

/*
*=============================================================================
**  Synopsis: SHORT UifAC116Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.116 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC116Init1[] = {{UifAC116EnterData1, CID_AC116ENTERDATA1},
                                      {UifAC116IssueRpt, CID_AC116ISSUERPT},
                                      {NULL,             0                }};

/* Define Next Function at UIM_INIT from CID_AC116ENTERDATA1 */

UIMENU FARCONST aChildAC116Init2[] = {{UifAC116EnterData1, CID_AC116ENTERDATA1},
                                      {UifACExitFunction, CID_ACEXITFUNCTION},
                                      {NULL,              0                 }};

/* Define Next Function at UIM_ACCEPTED from CID_AC116ENTERDATA1 */

UIMENU FARCONST aChildAC116Accept1[] = {{UifAC116EnterAdject1, CID_AC116ENTERADJECT1},
                                       {NULL,                  0                       }};

/* Define Next Function at UIM_ACCEPTED from CID_AC116ENTERADJECT1,2 */

UIMENU FARCONST aChildAC116Accept2[] = {{UifAC116EnterData2, CID_AC116ENTERDATA2},
                                       {NULL,                0                  }};

/* Define Next Function at UIM_ACCEPTED from CID_AC116ENTERDATA2 */

UIMENU FARCONST aChildAC116Accept3[] = {{UifAC116EnterAdject2, CID_AC116ENTERADJECT2},
                                       {NULL,                  0                       }};


SHORT UifAC116Function(KEYMSG *pKeyMsg) 
{

    MAINTREDISP     ReDisplay;


    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        usUifACControl = 0;                     /* Initialize Uif Control Save Area */

        /* Reset All Descriptor */

        ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
        ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLR;
        MaintReDisp(&ReDisplay);
        
        UifACPGMLDClear();                              /* Clear MLD */

        MaintDisp(AC_SET_PLU,                   /* A/C Number */
                  CLASS_MAINTDSPCTL_10NPG,      /* Set Minor Class for 10N10D */
                  MaintGetPage(),               /* Page Number */
                  0,                            /* PTD Type */
                  0,                            /* Report Type */
                  0,                            /* Reset Type */
                  0L,                           /* Amount Data */
                  LDT_NUM_ADR);                 /* "Request Number Entry" Lead Through Address */

        switch (pKeyMsg->SEL.usFunc) {
        case CID_AC116ENTERDATA1:
        case CID_AC116ENTERDATA2:
            UieNextMenu(aChildAC116Init2);                  /* Open Next Function */
            return(SUCCESS);
            
        default:                                    
            UieNextMenu(aChildAC116Init1);          /* Open Next Function */

            return(SUCCESS);
        }

    case UIM_ACCEPTED:
        switch (pKeyMsg->SEL.usFunc) {
        usUifACControl = 0;                                     /* Clear Control Area */
        case CID_AC116ENTERDATA1:
            usUifACControl = 0;                                 /* Initialize Uif Control Save Area */                                
            UieNextMenu(aChildAC116Accept1);                    /* Open Next Function */
            return(SUCCESS);

        case CID_AC116ENTERDATA2:
            UieNextMenu(aChildAC116Accept3);                    /* Open Next Function */
            return(SUCCESS);

        default:                                                /* CID_AC116ENTERADJECT1 Case */
            usUifACControl = 0;                                 /* Initialize Uif Control Save Area */                                
            UieNextMenu(aChildAC116Accept2);                    /* Open Next Function */
            return(SUCCESS);
        }

    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC116IssueRpt(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Wrong Data Error
*               
**  Description: A/C No. 116 Issue Report 
*===============================================================================
*/
    


SHORT UifAC116IssueRpt(KEYMSG *pKeyMsg) 
{
    
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        /* Check Digit */

        if (pKeyMsg->SEL.INPUT.uchLen) {            /* W/ Amount Case */
            return(LDT_SEQERR_ADR);
        }
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            MaintMenuPLUReport();                   /* Execute Report Procedure */
            UieExit(aACEnter);                      /* Return to UifPGEnter() */
            return(SUCCESS);
        
        /* default:
            break; */

        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));              /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC116EnterData1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.116 Enter Data 1 Mode 
*===============================================================================
*/
    
/* Define Previous Function and Key Sequence */

UIMENU FARCONST aParentAC116EnterData1[] = {{UifAC116Function, CID_AC116FUNCTION},
                                            {NULL,             0                     }};

UISEQ FARCONST aszSeqAC116EnterData1[] = {FSC_MENU_SHIFT, FSC_D_MENU_SHIFT, FSC_PLU, FSC_KEYED_PLU, FSC_VOID, FSC_SCANNER, FSC_E_VERSION, 0};


SHORT UifAC116EnterData1(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    TCHAR           auchPLUNumber[PLU_MAX_DIGIT+1];
    MAINTSETPAGE    SetPageNo;
    PARAMENUPLUTBL  MenuPLUTbl;                   
    MAINTREDISP     ReDisplay;
    FSCTBL FAR      *pData;
    UCHAR           uchModStat;

    /* Clear SetPageNo Work */

    memset(&SetPageNo, '\0', sizeof(MAINTSETPAGE));
    memset(&MenuPLUTbl, 0, sizeof(MenuPLUTbl));
    memset(auchPLUNumber, 0, sizeof(auchPLUNumber));

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC116EnterData1);                             /* Register Key Sequence */
        UieEchoBack(UIE_ECHO_ROW0_NUMBER, UIF_DIGIT14); /* Saratoga */
        return(SUCCESS);

    case UIM_CANCEL:
        usUifACControl = 0;                                     /* Initialize Uif Control Save Area */                                

        /* Reset All Descriptor */

        ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
        ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLR;
        MaintReDisp(&ReDisplay);

        MaintMenuPLUModifier(CLASS_PARAPLU_RESET_EV);
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_MENU_SHIFT:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {                   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            SetPageNo.uchStatus = 0;                                       
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/ Amount Case */
                SetPageNo.uchStatus |= MAINT_WITHOUT_DATA;
            } else {
                SetPageNo.uchPageNumber = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }
            SetPageNo.uchMajorClass = CLASS_MAINTSETPAGE;                   /* Set Major Class */
            SetPageNo.uchACNumber = AC_SET_PLU;                             /* Set Major Class */
            SetPageNo.uchLeadAddr = LDT_NUM_ADR;
            if ((sError = MaintSetPage(&SetPageNo)) == SUCCESS) {
                /* pData = (FSCTBL FAR *)&ParaFSC1[uchMaintMenuPage - 1]; */
                pData = (FSCTBL FAR *)&ParaFSC[uchMaintMenuPage - 1];
                UieSetFsc(pData);
                UieReject();                                                /* Shift Cur. Func. to Act. Func. */ 
            }
            return(sError);

        case FSC_D_MENU_SHIFT:
            /* Check W/o Amount */

            if (pKeyMsg->SEL.INPUT.uchLen) {                    /* W/ Amount */
                return(LDT_SEQERR_ADR);
            }
            SetPageNo.uchMajorClass = CLASS_MAINTSETPAGE;               /* Set Major Class */
            SetPageNo.uchACNumber = AC_SET_PLU;                         /* Set Major Class */
            SetPageNo.uchLeadAddr = LDT_NUM_ADR;
            SetPageNo.uchPageNumber = pKeyMsg->SEL.INPUT.uchMinor;      /* set page number */
            SetPageNo.uchStatus = 0;                                    /* Set W/ Amount Status */
            if ((sError = MaintSetPage(&SetPageNo)) == SUCCESS) {
                /* pData = (FSCTBL FAR *)&ParaFSC1[uchMaintMenuPage - 1]; */
                pData = (FSCTBL FAR *)&ParaFSC[uchMaintMenuPage - 1];
                UieSetFsc(pData);
                UieReject();                                            /* Shift Current Func. to Active Func. */
            }
            return(sError);

        case FSC_PLU:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT14) {                   /* Over Digit */
            /* if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT4) {                   / Over Digit */
                return(LDT_KEYOVER_ADR);
            }

            MenuPLUTbl.uchStatus = 0;                                       
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/ Amount Case */
                MenuPLUTbl.uchStatus |= MAINT_WITHOUT_DATA;
            } else {
                _tcsncpy(MenuPLUTbl.SetPLU[0].aszPLUNumber, pKeyMsg->SEL.INPUT.aszKey, pKeyMsg->SEL.INPUT.uchLen);
                //memcpy(MenuPLUTbl.SetPLU[0].aszPLUNumber, pKeyMsg->SEL.INPUT.aszKey, pKeyMsg->SEL.INPUT.uchLen);
                /* MenuPLUTbl.SetPLU[0].usPLU = ( USHORT)pKeyMsg->SEL.INPUT.lData; */
            }

            MenuPLUTbl.uchMajorClass = CLASS_PARAMENUPLUTBL;                /* Set Major Class */
            if (usUifACControl & UIF_VOID) {                                /* VOID Key Already Put */
                if ((sError = MaintMenuPLUDelete(&MenuPLUTbl)) == SUCCESS) {
                    UieExit(aParentAC116EnterData1);
                }
            } else {
                if ((sError = MaintMenuPLURead(&MenuPLUTbl)) == SUCCESS) {
                    UieAccept();
                }
            }
            return(sError);

        case FSC_KEYED_PLU:
            /* Check W/O Amount */

            if (pKeyMsg->SEL.INPUT.uchLen) {                                /* W/ Amount */
                return(LDT_KEYOVER_ADR);
            }

            /* Get PLU Number */

            if ((sError = MaintGetPLUNo(pKeyMsg->SEL.INPUT.uchMinor, auchPLUNumber, &uchModStat)) != SUCCESS) {
            /* if ((sError = MaintGetPLUNo(pKeyMsg->SEL.INPUT.uchMinor, &usPLUNumber)) != SUCCESS) { */
                return(sError);
            }

            if (uchModStat & MOD_STAT_EVERSION) { /* E-version status */
                MaintMenuPLUModifier(CLASS_PARAPLU_SET_EV);
            } else {
                MaintMenuPLUModifier(CLASS_PARAPLU_RESET_EV);
            }

            MenuPLUTbl.uchStatus = 0;                                       
            _tcsncpy(MenuPLUTbl.SetPLU[0].aszPLUNumber, auchPLUNumber, PLU_MAX_DIGIT);
            //memcpy(MenuPLUTbl.SetPLU[0].aszPLUNumber, auchPLUNumber, PLU_MAX_DIGIT);
            /* MenuPLUTbl.SetPLU[0].usPLU = usPLUNumber; */
            MenuPLUTbl.uchMajorClass = CLASS_PARAMENUPLUTBL;                /* Set Major Class */
            if (usUifACControl & UIF_VOID) {                                /* VOID Key Already Put */
                if ((sError = MaintMenuPLUDelete(&MenuPLUTbl)) == SUCCESS) {
                    UieExit(aParentAC116EnterData1);
                }
            } else {
                if ((sError = MaintMenuPLURead(&MenuPLUTbl)) == SUCCESS) {
                    UieAccept();
                }
            }
            return(sError);

        case FSC_SCANNER:
            if((pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen > UIF_DIGIT14) ||  /* Over digit */
               (pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen == 0)) {           /* or no entry */
                return(LDT_KEYOVER_ADR);
            }

            MenuPLUTbl.uchStatus = 0;                                       
            _tcsncpy(MenuPLUTbl.SetPLU[0].aszPLUNumber, pKeyMsg->SEL.INPUT.DEV.SCA.auchStr, pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen);
            //memcpy(MenuPLUTbl.SetPLU[0].aszPLUNumber, pKeyMsg->SEL.INPUT.DEV.SCA.auchStr, pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen);
            MenuPLUTbl.uchStatus |= MAINT_SCANNER_INPUT;

            MenuPLUTbl.uchMajorClass = CLASS_PARAMENUPLUTBL;                /* Set Major Class */
            if (usUifACControl & UIF_VOID) {                                /* VOID Key Already Put */
                if ((sError = MaintMenuPLUDelete(&MenuPLUTbl)) == SUCCESS) {
                    UieExit(aParentAC116EnterData1);
                }
            } else {
                if ((sError = MaintMenuPLURead(&MenuPLUTbl)) == SUCCESS) {
                    UieAccept();
                }
            }
            return(sError);
            
        case FSC_E_VERSION:
            if (pKeyMsg->SEL.INPUT.uchLen) {        /* With Amount ? */
                return(LDT_KEYOVER_ADR);
            }
            return(MaintMenuPLUModifier(CLASS_PARAPLU_EVERSION));

        case FSC_VOID:
            /* Check W/ Amount */

            if (pKeyMsg->SEL.INPUT.uchLen) {                        /* W/ Amount */
                return(LDT_SEQERR_ADR);
            }
            if (usUifACControl & UIF_VOID) {                        /* VOID Key Already Put */
                return(LDT_SEQERR_ADR);
            } else {
                usUifACControl |= UIF_VOID;                         /* Set VOID Key Put Status */

                /* Set Void Descriptor */

                ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
                ReDisplay.uchMinorClass = CLASS_MAINTREDISP_VOID;
                MaintReDisp(&ReDisplay);
                return(SUCCESS);
            }

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
**  Synopsis: SHORT UifAC116EnterData2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.116 Enter Data 2 Mode 
*===============================================================================
*/
    
/* Define Key Sequence */

UISEQ FARCONST aszSeqAC116EnterData2[] = {FSC_MENU_SHIFT, FSC_D_MENU_SHIFT, FSC_PLU, FSC_KEYED_PLU, FSC_AC, FSC_NUM_TYPE, FSC_CANCEL, FSC_SCANNER, FSC_E_VERSION, 0};

UIMENU FARCONST aParentAC116Func[] = {{UifAC116Function, CID_AC116FUNCTION},
                                     {NULL,            0               }};

SHORT UifAC116EnterData2(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    TCHAR           auchPLUNumber[PLU_MAX_DIGIT+1];
    MAINTSETPAGE    SetPageNo;
    MAINTMENUPLUTBL MenuPLUTbl;                   
    FSCTBL FAR      *pData;
    UCHAR           uchModStat;

    /* Clear SetPageNo Work */

    memset(&SetPageNo, '\0', sizeof(MAINTSETPAGE));
    memset(&MenuPLUTbl, 0, sizeof(MenuPLUTbl));
    memset(auchPLUNumber, 0, sizeof(auchPLUNumber));

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC116EnterData2);                     /* Register Key Sequence */
        UieEchoBack(UIE_ECHO_ROW0_NUMBER,UIF_DIGIT14);
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            if (pKeyMsg->SEL.INPUT.uchLen) {                        /* W/ Amount Case */
                return(LDT_SEQERR_ADR);
            }

            /* Execute Write Data Procedure */
 
            MaintMenuPLUWrite();                                                                        
            UieExit(aParentAC116Func);                               /* Return to UifAC116Function() */
            return(SUCCESS);

        case FSC_NUM_TYPE:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {                   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }

            MenuPLUTbl.uchStatus = 0;                                           /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                                   /* W/o Amount Input Case */
                MenuPLUTbl.uchStatus |= MAINT_WITHOUT_DATA;                     /* Set W/o Amount Status */
            } else {                                                            /* W/ Amount Input Case */
                MenuPLUTbl.uchArrayAddr = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            MenuPLUTbl.uchMajorClass = CLASS_MAINTMENUPLUTBL;
            MenuPLUTbl.uchMinorClass = CLASS_PARAMENUPLU_READ;
            if ((sError = MaintMenuPLUEdit(&MenuPLUTbl)) == SUCCESS) {   
                UieReject();
            }
            return(sError);                                                 
            
        case FSC_MENU_SHIFT:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {                   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            MenuPLUTbl.uchStatus = 0;                                           /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                                   /* W/o Amount Input Case */
                MenuPLUTbl.uchStatus |= MAINT_WITHOUT_DATA;                     /* Set W/o Amount Status */
            } else {                                                            /* W/ Amount Input Case */
                MenuPLUTbl.uchPageNumber = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            MenuPLUTbl.uchMajorClass = CLASS_MAINTMENUPLUTBL;
            MenuPLUTbl.uchMinorClass = CLASS_PARAMENUPLU_PAGE;
            if ((sError = MaintMenuPLUEdit(&MenuPLUTbl)) == SUCCESS) {   
                /* pData = (FSCTBL FAR *)&ParaFSC1[uchMaintMenuPage - 1]; */
                pData = (FSCTBL FAR *)&ParaFSC[uchMaintMenuPage - 1];
                UieSetFsc(pData);
                UieReject();
            }
            return(sError);                                                 

        case FSC_D_MENU_SHIFT:
            /* Check W/o Amount */

            if (pKeyMsg->SEL.INPUT.uchLen) {                    /* W/ Amount */
                return(LDT_SEQERR_ADR);
            }
            MenuPLUTbl.uchMajorClass = CLASS_MAINTMENUPLUTBL;
            MenuPLUTbl.uchMinorClass = CLASS_PARAMENUPLU_PAGE;
            MenuPLUTbl.uchPageNumber = pKeyMsg->SEL.INPUT.uchMinor;     /* set page number */
            MenuPLUTbl.uchStatus = 0;                                   /* Set W/ Amount Status */
            if ((sError = MaintMenuPLUEdit(&MenuPLUTbl)) == SUCCESS) {   
                /* pData = (FSCTBL FAR *)&ParaFSC1[uchMaintMenuPage - 1]; */
                pData = (FSCTBL FAR *)&ParaFSC[uchMaintMenuPage - 1];
                UieSetFsc(pData);
                UieReject();
            }
            return(sError);                                                 

        case FSC_PLU:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT14) {                   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }

            MenuPLUTbl.uchStatus = 0;                                       
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/ Amount Case */
                MenuPLUTbl.uchStatus |= MAINT_WITHOUT_DATA;
            } else {
                _tcsncpy(MenuPLUTbl.SetPLU.aszPLUNumber, pKeyMsg->SEL.INPUT.aszKey, pKeyMsg->SEL.INPUT.uchLen);
                //memcpy(MenuPLUTbl.SetPLU.aszPLUNumber, pKeyMsg->SEL.INPUT.aszKey, pKeyMsg->SEL.INPUT.uchLen);
            }

            MenuPLUTbl.uchMajorClass = CLASS_MAINTMENUPLUTBL;               
            MenuPLUTbl.uchMinorClass = CLASS_PARAMENUPLU_PLUWRITE;                               
            if ((sError = MaintMenuPLUEdit(&MenuPLUTbl)) == SUCCESS) {
                UieAccept();
            }                                                                
            return(sError);

        case FSC_KEYED_PLU:
            /* Check W/O Amount */

            if (pKeyMsg->SEL.INPUT.uchLen) {                                /* W/ Amount */
                return(LDT_KEYOVER_ADR);
            }

            /* Get PLU Number */

            if ((sError = MaintGetPLUNo(pKeyMsg->SEL.INPUT.uchMinor, auchPLUNumber, &uchModStat)) != SUCCESS) {
                return(sError);
            }

            if (uchModStat & MOD_STAT_EVERSION) { /* E-version status */
                MaintMenuPLUModifier(CLASS_PARAPLU_SET_EV);
            } else {
                MaintMenuPLUModifier(CLASS_PARAPLU_RESET_EV);
            }

            MenuPLUTbl.uchStatus = 0;                                       
            _tcsncpy(MenuPLUTbl.SetPLU.aszPLUNumber, auchPLUNumber, PLU_MAX_DIGIT);
            //memcpy(MenuPLUTbl.SetPLU.aszPLUNumber, auchPLUNumber, PLU_MAX_DIGIT);
            MenuPLUTbl.uchMajorClass = CLASS_MAINTMENUPLUTBL;               
            MenuPLUTbl.uchMinorClass = CLASS_PARAMENUPLU_PLUWRITE;                               
            if ((sError = MaintMenuPLUEdit(&MenuPLUTbl)) == SUCCESS) {
                UieAccept();
            }                                                                
            return(sError);

        case FSC_SCANNER:
            if((pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen > UIF_DIGIT14) ||  /* Over digit */
               (pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen == 0)) {           /* or no entry */
                return(LDT_KEYOVER_ADR);
            }

            MenuPLUTbl.uchStatus = 0;                                       
            _tcsncpy(MenuPLUTbl.SetPLU.aszPLUNumber, pKeyMsg->SEL.INPUT.DEV.SCA.auchStr, pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen);
            //memcpy(MenuPLUTbl.SetPLU.aszPLUNumber, pKeyMsg->SEL.INPUT.DEV.SCA.auchStr, pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen);
            MenuPLUTbl.uchStatus |= MAINT_SCANNER_INPUT;

            MenuPLUTbl.uchMajorClass = CLASS_PARAMENUPLUTBL;                /* Set Major Class */
            MenuPLUTbl.uchMinorClass = CLASS_PARAMENUPLU_PLUWRITE;                               
            if ((sError = MaintMenuPLUEdit(&MenuPLUTbl)) == SUCCESS) {
                UieAccept();
            }                                                                
            return(sError);
            
        case FSC_E_VERSION:
            if (pKeyMsg->SEL.INPUT.uchLen) {        /* With Amount ? */
                return(LDT_KEYOVER_ADR);
            }
            return(MaintMenuPLUModifier(CLASS_PARAPLU_EVERSION));

        case FSC_CANCEL:
            MaintMenuPLUModifier(CLASS_PARAPLU_RESET_EV);
            MaintMakeAbortKey();
            break;

        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC116EnterAdject1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.116 Enter adjective Data 1 Mode 
*===============================================================================
*/


    
SHORT UifAC116EnterAdject1(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    MAINTMENUPLUTBL MenuPLUTbl;                   
                   


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {               /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }

            MenuPLUTbl.uchStatus = 0;                                       
            if (!pKeyMsg->SEL.INPUT.uchLen) {                                   /* W/o Amount Input Case */
                MenuPLUTbl.uchStatus |= MAINT_WITHOUT_DATA;                     /* Set W/o Amount Status */
            } else {                                                            /* W/ Amount Input Case */
                MenuPLUTbl.SetPLU.uchAdjective = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }
            MenuPLUTbl.uchMajorClass = CLASS_MAINTMENUPLUTBL;               
            MenuPLUTbl.uchMinorClass = CLASS_PARAMENUPLU_ADJWRITE;                               
            if ((sError = MaintMenuPLUEdit(&MenuPLUTbl)) == SUCCESS) {
                UieAccept();
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
**  Synopsis: SHORT UifAC116EnterAdject2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.116 Enter adjective Data 2 Mode 
*===============================================================================
*/

/* Define Key Sequence */

UISEQ FARCONST aszSeqAC116EnterAdject2[] = {FSC_AC, FSC_CANCEL, 0};


SHORT UifAC116EnterAdject2(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    MAINTMENUPLUTBL MenuPLUTbl;                   
                   


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:                                                  
        UieOpenSequence(aszSeqAC116EnterAdject2);                   /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Check Amount */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {               /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }

            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount Case */
                pKeyMsg->SEL.INPUT.lData = 1L;
            }                                                         
            MenuPLUTbl.uchStatus = 0;                                       
            if (!pKeyMsg->SEL.INPUT.uchLen) {                                   /* W/o Amount Input Case */
                MenuPLUTbl.uchStatus |= MAINT_WITHOUT_DATA;                     /* Set W/o Amount Status */
            } else {                                                            /* W/ Amount Input Case */
                MenuPLUTbl.SetPLU.uchAdjective = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }
            MenuPLUTbl.uchMajorClass = CLASS_MAINTMENUPLUTBL;               
            MenuPLUTbl.uchMinorClass = CLASS_PARAMENUPLU_ADJWRITE;                               
            if ((sError = MaintMenuPLUEdit(&MenuPLUTbl)) == SUCCESS) {
                UieAccept();
            }                                                                
            return(sError);

        case FSC_CANCEL:
            MaintMakeAbortKey();
            break;

        /* default:
              break;  */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                          /* Execute Default Procedure */
    }
}
