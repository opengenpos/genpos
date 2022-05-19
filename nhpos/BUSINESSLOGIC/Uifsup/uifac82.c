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
* Title       : PLU Mnemonics Change Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC82.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC82Function()       : A/C No.82 Function Entry Mode
*               UifAC82EnterData()      : A/C No.82 Enter Data Mode
*               UifAC82EnterALPHA()     : A/C No.82 Enter PLU Mnemonics Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jun-16-92:00.00.01    :K.You      : initial                                   
* Jul-21-93:01.00.12    :J.IKEDA    : Adds Direct Menu Shift Key in UifAC82EnterData()                                    
* Apr-14-95:03.00.00    :T.Satoh    : Adds clear MLD
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
#include <uireg.h>
#include <uifsup.h>
#include <uifpgequ.h>
#include <cvt.h>
#include <mldmenu.h>

#include "uifsupex.h"                       /* Unique Extern Header for UI */
/*
*=============================================================================
**  Synopsis: SHORT UifAC82Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.82 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC82Init[] = {{UifAC82EnterData, CID_AC82ENTERDATA},
                                    {UifAC64ErrorFunction, CID_AC64ERRORFUNCTION},
                                    {NULL,             0               }};

/* Define Next Function at UIM_ACCEPTED */

UIMENU FARCONST aChildAC82Accept[] = {{UifAC82EnterData, CID_AC82ENTERDATA},
                                      {UifACExitFunction, CID_ACEXITFUNCTION},
                                      {NULL,              0                 }};




SHORT UifAC82Function(KEYMSG *pKeyMsg) 
{



    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC82Init);                            /* Open Next Function */
        return(SUCCESS);

    case UIM_ACCEPTED:
        UieNextMenu(aChildAC82Accept);                          /* Open Next Function */
        return(SUCCESS);

    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC82EnterData(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.82 Enter Data Mode 
*===============================================================================
*/
    
/* Define Next Function */

UIMENU FARCONST aChildAC82EnterData[] = {{UifACShift, CID_ACSHIFT},
                                         {UifAC82EnterALPHA, CID_AC82ENTERALPHA},
                                         {UifACChangeKB1, CID_ACCHANGEKB1},
                                         {NULL,           0              }};



SHORT UifAC82EnterData(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    TCHAR           auchPLUNumber[PLU_MAX_DIGIT+1];
    PARAPLU         PLUNoData;
    MAINTSETPAGE    SetPageNo;
    FSCTBL FAR      *pData;
    MAINTPLU        MaintPLU;
    UCHAR           uchModStat;

    /* Clear SetPageNo Work */

    memset(&SetPageNo, '\0', sizeof(MAINTSETPAGE));
    memset(&PLUNoData, 0, sizeof(PLUNoData));
    memset(auchPLUNumber, 0, sizeof(auchPLUNumber));

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UifACPGMLDClear();                              /* Clear MLD */

        MaintDisp(AC_PLU_MNEMO_CHG,                     /* A/C Number */
                  CLASS_MAINTDSPCTL_10NPG,              /* Set Minor Class for 10N10D */
                  MaintGetPage(),                       /* Page Number */
                  0,                                    /* PTD Type */
                  0,                                    /* Report Type */
                  0,                                    /* Reset Type */
                  0L,                                   /* Amount Data */
                  LDT_NUM_ADR);                         /* "Number Entry" Lead Through Address */

        UieOpenSequence(aszSeqAC63EnterData1);                              /* Register Key Sequence */
        UieEchoBack(UIE_ECHO_ROW0_NUMBER,UIF_DIGIT14);

        MaintPLU.uchMajorClass = CLASS_PARAPLU; /* Set Major Class */
        MaintPLU.uchMinorClass = CLASS_PARAPLU_RESET_EV;     /* Set Minor Class */
        MaintPLU.uchStatus = 0;                 /* Set W/ Amount Status */
        MaintPLU.usACNumber = AC_PLU_MNEMO_CHG;    /* Set A/C Number */
        MaintPLU.uchEVersion = 0;               /* Set E-Version */
        MaintPLUModifier(&MaintPLU);

        return(SUCCESS);

    case UIM_ACCEPTED:                                                      /* Accepted from UifAC82ALPHA() */
        UieAccept();                              
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {

        case FSC_KEYED_STRING:          /* Execute Control String Function */
        case FSC_STRING:
            sError = UifRegString(pKeyMsg);
            if (sError != SUCCESS){
                sError = LDT_SEQERR_ADR;
            }
            UifACRedisp();
            return(sError);

        case FSC_MENU_SHIFT:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {                   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            SetPageNo.uchStatus = 0;
            if (!pKeyMsg->SEL.INPUT.uchLen) {
                SetPageNo.uchStatus |= MAINT_WITHOUT_DATA;
            } else {
                SetPageNo.uchPageNumber = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }
            SetPageNo.uchMajorClass = CLASS_MAINTSETPAGE;                   /* Set Major Class */
            SetPageNo.uchACNumber = AC_PLU_MNEMO_CHG;                       /* Set Major Class */
            SetPageNo.uchLeadAddr = LDT_NUM_ADR;
            if ((sError = MaintSetPage(&SetPageNo)) == SUCCESS) {
                /* pData = (FSCTBL FAR *)&ParaFSC1[uchMaintMenuPage - 1]; */
                pData = (FSCTBL FAR *)&ParaFSC[uchMaintMenuPage - 1];
                UieSetFsc(pData);
                UieReject();                                                /* Shift Current Func. to Active Func. */ 
            }
            return(sError);

        case FSC_D_MENU_SHIFT:
            /* Check W/o Amount */

            if (pKeyMsg->SEL.INPUT.uchLen) {                    /* W/ Amount */
                return(LDT_SEQERR_ADR);
            }
            SetPageNo.uchMajorClass = CLASS_MAINTSETPAGE;               /* Set Major Class */
            SetPageNo.uchACNumber = AC_PLU_MNEMO_CHG;                   /* Set Major Class */
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
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT14) {                   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            PLUNoData.uchMajorClass = CLASS_PARAPLU;                        /* Set Major Class */
            PLUNoData.usACNumber = AC_PLU_MNEMO_CHG;                        /* Set PLU MNEMO CHG. A/C Number */
            PLUNoData.uchStatus = 0;                     /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount Input Case */
                PLUNoData.uchStatus |= MAINT_WITHOUT_DATA;              /* Set W/o Amount Status */
            } else {                                                    /* W/ Amount Input Case */
                _tcscpy(PLUNoData.auchPLUNumber,
                   pKeyMsg->SEL.INPUT.aszKey);
            }
            if ((sError = MaintPLURead(&PLUNoData)) == SUCCESS) {           /* Read Function Completed */
                UieNextMenu(aChildAC82EnterData);                           /* Return to UifAC82Function() */                                        
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

            MaintPLU.uchMajorClass = CLASS_PARAPLU; /* Set Major Class */
            MaintPLU.uchStatus = 0;                 /* Set W/ Amount Status */
            MaintPLU.usACNumber = AC_PLU_MNEMO_CHG;    /* Set A/C Number */
            if (uchModStat & MOD_STAT_EVERSION) {
                MaintPLU.uchMinorClass = CLASS_PARAPLU_SET_EV;     /* Set Minor Class */
                MaintPLU.uchEVersion = 1;               /* Set E-Version */
            } else {
                MaintPLU.uchMinorClass = CLASS_PARAPLU_RESET_EV;     /* Set Minor Class */
                MaintPLU.uchEVersion = 0;               /* Set E-Version */
            }
            MaintPLUModifier(&MaintPLU);

            PLUNoData.uchStatus = 0;                     /* Set W/ Amount Status */
            PLUNoData.uchMajorClass = CLASS_PARAPLU;                        /* Set Major Class */
            PLUNoData.usACNumber = AC_PLU_MNEMO_CHG;                        /* Set PLU MNEMO CHG. A/C Number */
            _tcsncpy(PLUNoData.auchPLUNumber, auchPLUNumber, PLU_MAX_DIGIT);
            //memcpy(PLUNoData.auchPLUNumber, auchPLUNumber, PLU_MAX_DIGIT);
            if ((sError = MaintPLURead(&PLUNoData)) == SUCCESS) {           /* Read Function Completed */
                UieNextMenu(aChildAC82EnterData);                           /* Return to UifAC82Function() */                                        
            }
            return(sError);
                
        case FSC_SCANNER:
            if((pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen > UIF_DIGIT14) ||  /* Over digit */
               (pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen == 0)) {           /* or no entry */
                return(LDT_KEYOVER_ADR);
            }

            PLUNoData.uchMajorClass = CLASS_PARAPLU;                        /* Set Major Class */
            PLUNoData.usACNumber = AC_PLU_MNEMO_CHG;                           /* Set PLU MAINT A/C Number */
            PLUNoData.uchStatus = 0;                                    /* Set W/ Amount Status */
            PLUNoData.uchStatus |= MAINT_SCANNER_INPUT;
            _tcsncpy(PLUNoData.auchPLUNumber, pKeyMsg->SEL.INPUT.DEV.SCA.auchStr, pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen);
            //memcpy(PLUNoData.auchPLUNumber, pKeyMsg->SEL.INPUT.DEV.SCA.auchStr, pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen);
            if ((sError = MaintPLURead(&PLUNoData)) == SUCCESS) {           /* Read Function Completed */
                UieNextMenu(aChildAC82EnterData);                           /* Return to UifAC82Function() */                                        
            }
            return(sError);
            
        case FSC_E_VERSION:
            if (pKeyMsg->SEL.INPUT.uchLen) {        /* With Amount ? */
                return(LDT_KEYOVER_ADR);
            }
            MaintPLU.uchMajorClass = CLASS_PARAPLU; /* Set Major Class */
            MaintPLU.uchMinorClass = CLASS_PARAPLU_SET_EV;     /* Set Minor Class */
            MaintPLU.uchStatus = 0;                 /* Set W/ Amount Status */
            MaintPLU.usACNumber = AC_PLU_MNEMO_CHG;    /* Set A/C Number */
            MaintPLU.uchEVersion = 1;               /* Set E-Version */
            return(MaintPLUModifier(&MaintPLU));

        /* default:
            break; */
        }
        /* break; */

    case UIM_CANCEL:
        MaintPLU.uchMajorClass = CLASS_PARAPLU; /* Set Major Class */
        MaintPLU.uchMinorClass = CLASS_PARAPLU_RESET_EV;     /* Set Minor Class */
        MaintPLU.uchStatus = 0;                 /* Set W/ Amount Status */
        MaintPLU.usACNumber = AC_PLU_MNEMO_CHG;    /* Set A/C Number */
        MaintPLU.uchEVersion = 0;               /* Set E-Version */
        MaintPLUModifier(&MaintPLU);
        /* break */
    
    default:
        return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC82EnterALPHA(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.82 Enter PLU Mnemonics Mode 
*===============================================================================
*/
    


SHORT UifAC82EnterALPHA(KEYMSG *pKeyMsg) 
{
    
    SHORT       sError;
    MAINTPLU    PLUEditData;   


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_P1);                    /* Register Key Sequence */
        UieEchoBack(UIE_ECHO_ROW1_AN, PARA_PLU_LEN);        /* Define Display Format and Max. Input Data */

        /* Set KB Mode to ALPHA Mode */

        UifACChgKBType(UIF_ALPHA_KB);

        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P1:
            PLUEditData.uchStatus = 0;                              /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount Input Case */
                PLUEditData.uchStatus |= MAINT_WITHOUT_DATA;        /* Set W/o Amount Status */
            } else {                                                /* W/ Amount Input Case */
                memset(PLUEditData.aszMnemonics,                    /* Full NULL to Own Buffer */
                       '\0',
                       (PARA_PLU_LEN+1) * sizeof(TCHAR));

                _tcsncpy(PLUEditData.aszMnemonics,                    /* Copy Input Mnemonics to Own Buffer */
                       pKeyMsg->SEL.INPUT.aszKey,
                       pKeyMsg->SEL.INPUT.uchLen);
                //memcpy(PLUEditData.aszMnemonics,                    /* Copy Input Mnemonics to Own Buffer */
                //       pKeyMsg->SEL.INPUT.aszKey,
                //       pKeyMsg->SEL.INPUT.uchLen);
            }

            /* Execute Write PLU Mnemonics Procedure */
            
            PLUEditData.uchMajorClass = CLASS_MAINTPLU;             /* Set Major Class */
            PLUEditData.uchMinorClass = CLASS_PARAPLU_MNEMO_WRITE;  /* Set Minor Class */
            PLUEditData.usACNumber = AC_PLU_MNEMO_CHG;              /* Set PLU MNEMO CHG. A/C Number */
            if ((sError = MaintPLUEdit(&PLUEditData)) != SUCCESS) {
                return(LDT_SEQERR_ADR);
            }
            MaintPLUWrite();                                            

            /* Set KB Mode to Neumeric Mode */

            UifACChgKBType(UIF_NEUME_KB);

            UieEchoBack(UIE_ECHO_ROW0_NUMBER, UIF_DIGIT10);     /* Set Display Format and Set Max. Input Digit */
            UieAccept();                                        /* Return to UifACXXXFunction() */
            return(SUCCESS);                                        

        /* default:
              break; */

        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                              /* Execute Default Procedure */
    }
}








                                                      
