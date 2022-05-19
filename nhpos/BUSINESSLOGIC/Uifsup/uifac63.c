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
* Title       : PLU Maintenance Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC63.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC63Function()       : A/C No.63 Function Entry Mode
*               UifAC63EnterData1()     : A/C No.63 Enter Data1 Mode
*               UifAC63EnterData2()     : A/C No.63 Enter Data2 Mode
*               UifAC63Intervation()    : A/C No.63 Intervation Mode
*               UifAC63ErrorCorrect()   : A/C No.63 Error Correct Mode
*               UifAC63EnterALPHA()     : A/C No.63 Enter PLU Mnemonics Mode
*               UifAC63ExitFunction()   : A/C No.63 Exit Function Mode
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jun-16-92:00.00.01    :K.You      : initial                                   
* Jul-21-93:01.00.12    :J.IKEDA    : Adds Direct Menu Shift Key in UifAC63EnterData1()                                     
* Apr-14-95:03.00.00    :T.Satoh    : Adds clear MLD
*** NHPOS 1.4.x, NCR 7448  ***
* Aug-01-02  NHPOS 1.4 R.Chambers   Product donated to Georgia Southern University
*** NHPOS 2.0.x Touchscreen ***
* Mar-01-04  NHPOS 2.0 R.Chambers   Conversion from NCR 7448 to Touchscreen terminals
*** GenPOS 2.2.x Touchscreen ***
* Nov-19-14  GenPOS 2.2  R.Chambers   Source cleanup to remove old, before NCR 7448,
*                                   code that applied to NCR 2170 back in the 1990s.
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
#include <uifpgequ.h>
#include <uifsup.h>
#include <cvt.h>
#include <mldmenu.h>

#include "uireg.h"
#include "uifsupex.h"                       /* Unique Extern Header for UI */


/*
==============================================================================
;                      C O M M O N   R O M   T A B L E s                         
;=============================================================================
*/


/* Define Key Sequence */
// the following menu FSC key definitions are used by several PLU related Action Codes:
//    AC63 PLU Maintenance
//    AC64 PLU Price Change
//    AC68 PLU Create or Delete
//    AC82 PLU Mnemonic Change
UISEQ CONST aszSeqAC63EnterData1[] = {FSC_MENU_SHIFT, FSC_D_MENU_SHIFT, FSC_PLU, FSC_KEYED_PLU,FSC_SCANNER, FSC_E_VERSION,
                                      FSC_STRING, FSC_KEYED_STRING, FSC_AC, 0};
UISEQ CONST aszSeqAC63EnterData2[] = {FSC_NUM_TYPE, FSC_ADJECTIVE, FSC_AC, FSC_EC,
                                      FSC_SCROLL_UP, FSC_SCROLL_DOWN,FSC_STRING,FSC_KEYED_STRING, 0};


/*
*=============================================================================
**  Synopsis: SHORT UifAC63Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.63 Function Entry Mode  
*===============================================================================
*/

/* Define Next Function at UIM_INIT */
static UIMENU CONST aChildAC63Init1[] = {{UifAC63EnterData1, CID_AC63ENTERDATA1},
                                  {UifAC63ErrorFunction, CID_AC63ERRORFUNCTION},
                                  {NULL,              0                 }};

/* Define Next Function at UIM_INIT from CID_AC63ERRORCORRECT, CID_AC63ENTERDATA2 */
static UIMENU CONST aChildAC63Init2[] = {{UifAC63EnterData1, CID_AC63ENTERDATA1},
                                  {UifAC63ExitFunction, CID_AC63EXITFUNCTION},
                                  {UifAC63CancelFunction, CID_AC63CANCELFUNCTION},
                                  {NULL,                0                   }};

/* Define Next Function at UIM_ACCEPTED from CID_AC63ENTERDATA1 */
static UIMENU CONST aChildAC63Accept1[] = {{UifAC63EnterData2, CID_AC63ENTERDATA2},
                                    {UifAC63CancelFunction, CID_AC63CANCELFUNCTION},
                                    {NULL,              0                 }};


SHORT UifAC63Function(KEYMSG *pKeyMsg) 
{
    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        switch (pKeyMsg->SEL.usFunc) {
        case CID_AC63ENTERDATA2:
            UieNextMenu(aChildAC63Init2);                       /* Open Next Function */
            return(SUCCESS);

        default:
            UieNextMenu(aChildAC63Init1);                   /* Open Next Function */
            return(SUCCESS);
        }

    case UIM_ACCEPTED:
        UieNextMenu(aChildAC63Accept1);                     /* Open Next Function */
        return(SUCCESS);

    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC63EnterData1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.63 Enter Data1 Mode 
*===============================================================================
*/
SHORT UifAC63EnterData1(KEYMSG *pKeyMsg) 
{
    SHORT           sError;
    TCHAR           auchPLUNumber[PLU_MAX_DIGIT+1+1] = {0};
    PARAPLU         PLUNoData = {0};
	MAINTSETPAGE    SetPageNo = {0};
    MAINTPLU        MaintPLU = {0};
    UCHAR           uchModStat;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UifACPGMLDClear();                              /* Clear MLD */

        MaintDisp(AC_PLU_MAINT,                         /* A/C Number */
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
        MaintPLU.usACNumber = AC_PLU_MAINT;    /* Set A/C Number */
        MaintPLU.uchEVersion = 0;               /* Set E-Version */
        MaintPLUModifier(&MaintPLU);
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
            SetPageNo.uchACNumber = AC_PLU_MAINT;                           /* Set A/C Number */
            SetPageNo.uchLeadAddr = LDT_NUM_ADR;
            if ((sError = MaintSetPage(&SetPageNo)) == SUCCESS) {
				FSCTBL  *pData = (FSCTBL *)&ParaFSC[uchMaintMenuPage - 1];
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
            SetPageNo.uchACNumber = AC_PLU_MAINT;                       /* Set A/C Number */
            SetPageNo.uchLeadAddr = LDT_NUM_ADR;
            SetPageNo.uchPageNumber = pKeyMsg->SEL.INPUT.uchMinor;      /* set page number */
            SetPageNo.uchStatus = 0;                                    /* Set W/ Amount Status */
            if ((sError = MaintSetPage(&SetPageNo)) == SUCCESS) {
				FSCTBL  *pData = (FSCTBL *)&ParaFSC[uchMaintMenuPage - 1];
                UieSetFsc(pData);
                UieReject();                                            /* Shift Current Func. to Active Func. */
            }
            return(sError);

		case FSC_AC:
			if (pKeyMsg->SEL.INPUT.uchLen < 1) {            /* no data entered so we will exit this menu item. */
				return(UifAC63ExitFunction(pKeyMsg));
			}
		case FSC_PLU:
            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT14) {                   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            PLUNoData.uchMajorClass = CLASS_PARAPLU;                        /* Set Major Class */
            PLUNoData.usACNumber = AC_PLU_MAINT;                            /* Set PLU MAINT A/C Number */
            PLUNoData.uchStatus = 0;                     /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount Input Case */
                PLUNoData.uchStatus |= MAINT_WITHOUT_DATA;              /* Set W/o Amount Status */
            } else {                                                    /* W/ Amount Input Case */
                _tcscpy(PLUNoData.auchPLUNumber, pKeyMsg->SEL.INPUT.aszKey);
            }
            if ((sError = MaintPLURead(&PLUNoData)) == SUCCESS) {           /* Read Function Completed */
                UieAccept();                                                /* Return to UifAC63Function() */
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
            MaintPLU.usACNumber = AC_PLU_MAINT;    /* Set A/C Number */
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
            PLUNoData.usACNumber = AC_PLU_MAINT;                            /* Set A/C Number */
            _tcsncpy(PLUNoData.auchPLUNumber, auchPLUNumber, PLU_MAX_DIGIT);
            if ((sError = MaintPLURead(&PLUNoData)) == SUCCESS) {           /* Read Function Completed */
                UieAccept();                                                /* Return to UifAC63Function() */                                        
            }
            return(sError);
        
        case FSC_SCANNER:
            if((pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen > UIF_DIGIT14) ||  /* Over digit */
               (pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen == 0)) {           /* or no entry */
                return(LDT_KEYOVER_ADR);
            }

            PLUNoData.uchMajorClass = CLASS_PARAPLU;                        /* Set Major Class */
            PLUNoData.usACNumber = AC_PLU_MAINT;                           /* Set PLU MAINT A/C Number */
            PLUNoData.uchStatus = 0;                                    /* Set W/ Amount Status */
            PLUNoData.uchStatus |= MAINT_SCANNER_INPUT;
            _tcsncpy(PLUNoData.auchPLUNumber, pKeyMsg->SEL.INPUT.DEV.SCA.auchStr, pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen);
            if ((sError = MaintPLURead(&PLUNoData)) == SUCCESS) {           /* Read Function Completed */
                usUifACControl |= UIF_AC68_DEL;                             /* Set Deletion Case Status */
                UieAccept();                                                /* Return to UifAC68Function() */                   
            }
            return(sError);
            
        case FSC_E_VERSION:
            if (pKeyMsg->SEL.INPUT.uchLen) {        /* With Amount ? */
                return(LDT_KEYOVER_ADR);
            }
            MaintPLU.uchMajorClass = CLASS_PARAPLU; /* Set Major Class */
            MaintPLU.uchMinorClass = CLASS_PARAPLU_SET_EV;     /* Set Minor Class */
            MaintPLU.uchStatus = 0;                 /* Set W/ Amount Status */
            MaintPLU.usACNumber = AC_PLU_MAINT;    /* Set A/C Number */
            MaintPLU.uchEVersion = 1;               /* Set E-Version */
            return(MaintPLUModifier(&MaintPLU));
        }
		// drop through and do default processing
    case UIM_CANCEL:
        MaintPLU.uchMajorClass = CLASS_PARAPLU; /* Set Major Class */
        MaintPLU.uchMinorClass = CLASS_PARAPLU_RESET_EV;     /* Set Minor Class */
        MaintPLU.uchStatus = 0;                 /* Set W/ Amount Status */
        MaintPLU.usACNumber = AC_PLU_MAINT;    /* Set A/C Number */
        MaintPLU.uchEVersion = 0;               /* Set E-Version */
        MaintPLUModifier(&MaintPLU);
        /* break */
    
    default:
        return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC63EnterData2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.63 Enter Data2 Mode 
*===============================================================================
*/
    
/* Define Next/Previous Function and Key Sequence */
static UIMENU CONST aChildAC63EnterData2[] = {{UifACShift, CID_ACSHIFT},
                                       {UifAC63EnterALPHA, CID_AC63ENTERALPHA},
                                       {UifACChangeKB1, CID_ACCHANGEKB1},
                                       {NULL,           0              }};

static UIMENU CONST aChildAC63Intervation[] = {{UifAC63Intervation, CID_AC63INTERVATION},
                                       {NULL,               0                  }};

static UIMENU CONST aParentAC63Func[] = {{UifAC63Function, CID_AC63FUNCTION},
                                  {NULL,            0               }};



SHORT UifAC63EnterData2(KEYMSG *pKeyMsg) 
{
    SHORT       sError;
	MAINTPLU    PLUEditData = {0};   

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC63EnterData2);                              /* Register Key Sequence */
        return(SUCCESS);
                                                                            
    case UIM_ACCEPTED:
        switch(pKeyMsg->SEL.usFunc) {
        case CID_ACCHANGEKB1:
            UieOpenSequence(aszSeqAC63EnterData2);                              /* Register Key Sequence */

            /* Display Next Address */
            PLUEditData.uchMajorClass = CLASS_MAINTPLU;
            PLUEditData.uchMinorClass = CLASS_PARAPLU_MAINT_READ;
            PLUEditData.uchStatus = MAINT_WITHOUT_DATA;         /* Set W/o Amount Status */
            MaintPLUEdit(&PLUEditData);

        default:
            break;
        }
        UieAccept();                                            /* Return to UifAC63Function() */
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

        case FSC_NUM_TYPE:                                                  /* Address Key Case */
            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {                   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            PLUEditData.uchStatus = 0;                                      /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Input Case */
                PLUEditData.uchStatus |= MAINT_WITHOUT_DATA;                /* Set W/o Amount Status */
            } else {                                                        /* W/ Amount Input Case */
                PLUEditData.uchFieldAddr = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Edit PLU Procedure */
            PLUEditData.uchMajorClass = CLASS_MAINTPLU;                     /* Set Major Class */
            PLUEditData.uchMinorClass = CLASS_PARAPLU_MAINT_READ;           /* Set Minor Class */
            PLUEditData.usACNumber = AC_PLU_MAINT;                          /* Set A/C Number */
            if ((sError = MaintPLUEdit(&PLUEditData)) == MAINT_PLU_MNEMO_ADR) { /* Next Edit is Mnemonic Set */
                UieNextMenu(aChildAC63EnterData2);                                    /* Open Next Function() */
                return(SUCCESS);
            }
            if (sError == SUCCESS) {                                         
                UieReject();                                                /* Shift Current Func. to Active Func. */
            }
            return(sError);

        case FSC_AC:                                                        /* Enter Key Case */
            if (pKeyMsg->SEL.INPUT.uchLen == 0) {                           /* W/o Amount Case */
                /* Execute Write Procedure */
                if ((sError = MaintPLUWrite()) == SUCCESS) {                                            
					MaintPLUFileUpDate();                                   /* UpDate PLU File Index */
                    UieExit(aParentAC63Func);                               /* Return to UifAC63Function() */
                }
            } else {                                                        /* W/ Amount Case */
                /* Check Digit */
                if (pKeyMsg->SEL.INPUT.uchLen > PARA_PLU_LEN) {             /* Over Digit */
                    return(LDT_KEYOVER_ADR);
                }
                PLUEditData.uchStatus = 0;                                  /* Initialize Cashier Status */ 
                PLUEditData.uchMajorClass = CLASS_MAINTPLU;                 /* Set Major Class */
                PLUEditData.uchMinorClass = CLASS_PARAPLU_MAINT_WRITE;      /* Set Minor Class */
                PLUEditData.usACNumber = AC_PLU_MAINT;                      /* Set A/C Number */
                PLUEditData.ulInputData = (ULONG)pKeyMsg->SEL.INPUT.lData;
				/* Copy Input Mnemonics to Own Buffer */
                memset(PLUEditData.aszMnemonics, '\0', sizeof(PLUEditData.aszMnemonics));
                _tcsncpy(PLUEditData.aszMnemonics, pKeyMsg->SEL.INPUT.aszKey, PARA_PLU_LEN);
                if ((sError = MaintPLUEdit(&PLUEditData)) == MAINT_PLU_CPYDEPT_ADR) {   /* Copy DEPT Ope. Case */     
                    UieNextMenu(aChildAC63Intervation);                     /* Open Next Function */
                    return(SUCCESS);
                } else if (sError == MAINT_PLU_MNEMO_ADR) {                 /* Next Edit is Mnemonics Set Case */
                    UieNextMenu(aChildAC63EnterData2);                      /* Open Next Function */
                    return(SUCCESS);
                }
                if (sError == SUCCESS) {                                         
                    UieReject();                                            /* Shift Current Func. to Active Func. */
                }
            }
            return(sError);

        case FSC_ADJECTIVE:
            if (pKeyMsg->SEL.INPUT.uchLen) {                                /* W/ Amount */
                return(LDT_SEQERR_ADR);
            }

            /* Check FSC Minor Data */
            if (!pKeyMsg->SEL.INPUT.uchMinor) {                             /* FSC Minor Data Not Defined */
                return(LDT_KEYOVER_ADR);
            }

            /* Convert Adjective Key to Address */
            PLUEditData.uchStatus = 0;                                      /* Set W/ Amount Status */
            PLUEditData.uchFieldAddr = ( UCHAR)((pKeyMsg->SEL.INPUT.uchMinor)%5);   /* Set Adjective Number */
            if (!PLUEditData.uchFieldAddr) {                                        /* Adjective Key is #5 */
                PLUEditData.uchFieldAddr = 5;
            }
            PLUEditData.uchFieldAddr += PLU_FIELD18_ADR;

            /* Execute Edit PLU Procedure */
            PLUEditData.uchMajorClass = CLASS_MAINTPLU;                     /* Set Major Class */
            PLUEditData.uchMinorClass = CLASS_PARAPLU_MAINT_READ;           /* Set Minor Class */
            PLUEditData.usACNumber = AC_PLU_MAINT;                          /* Set A/C Number */
            if ((sError = MaintPLUEdit(&PLUEditData)) == SUCCESS) {
                UieReject();                                                /* Shift Current Func. to Active Func. */
            }
            return(sError);

        case FSC_EC:
            if (pKeyMsg->SEL.INPUT.uchLen) {                        /* W/ Amount Input Case */
                return(LDT_SEQERR_ADR);
            } else {                                                /* W/o Amount Input Case */
                UieExit(aParentAC63Func);                           /* Return to UifAC63Function() */
                return(SUCCESS);
            }

        /* ---- scroll control for plu maintenance R3.1 ---- */
        case FSC_SCROLL_UP:
            if (pKeyMsg->SEL.INPUT.uchLen) {                        /* W/ Amount Input Case */
                return(LDT_SEQERR_ADR);
            } else {                                                /* W/o Amount Input Case */
                MaintPLUDisplayFirstPage();                         /* display 1st page */
            }
            UieReject();                                            /* Shift Current Func. to Active Func. */
            return(SUCCESS);

        case FSC_SCROLL_DOWN:
            if (pKeyMsg->SEL.INPUT.uchLen) {                        /* W/ Amount Input Case */
                return(LDT_SEQERR_ADR);
            } else {                                                /* W/o Amount Input Case */
                MaintPLUDisplaySecondPage();                        /* display 2nd page */
            }
            UieReject();                                            /* Shift Current Func. to Active Func. */
            return(SUCCESS);
        }
		// drop through and do default processing
    default:
        return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC63Intervation(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.63 Intervation Mode 
*===============================================================================
*/
    
/* Define Key Sequence */
UISEQ CONST aszSeqAC63Intervation[] = {FSC_NUM_TYPE, FSC_ADJECTIVE, FSC_AC, FSC_CANCEL, FSC_EC, 0};


SHORT UifAC63Intervation(KEYMSG *pKeyMsg) 
{
	MAINTPLU    PLUEditData = { 0 };

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC63Intervation);                             /* Register Key Sequence */
        return(SUCCESS);
                                                                            
    case UIM_INPUT:
        /* Check Digit */
        if (pKeyMsg->SEL.INPUT.uchLen) {                                    /* W/ Amount */
            return(LDT_SEQERR_ADR);
        }
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
			{
				SHORT       sReturn = MaintPLUCopy();
				if (sReturn != SUCCESS) {
					return(sReturn);
				}
			}
			// drop through and do next address.
            /* return(SUCCESS); */

        case FSC_CANCEL:
            /* Display Next Address */
            PLUEditData.uchMajorClass = CLASS_MAINTPLU;
            PLUEditData.uchMinorClass = CLASS_PARAPLU_ADD_READ;
            PLUEditData.uchStatus = 0;
            PLUEditData.usACNumber = AC_PLU_ADDDEL;
            PLUEditData.uchFieldAddr = PLU_FIELD2_ADR;
            MaintPLUEdit(&PLUEditData);
        
            UieAccept();
            return(SUCCESS);
        }
		// drop through and do default processing
    default:
        return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}
/*
*=============================================================================
**  Synopsis: SHORT UifAC63EnterALPHA(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Key Sequence Error
*               
**  Description: A/C No.63 Enter PLU Mnemonics Mode 
*===============================================================================
*/
SHORT UifAC63EnterALPHA(KEYMSG *pKeyMsg) 
{
    SHORT       sError;
	MAINTPLU    PLUEditData = { 0 };

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_P1);                            /* Register Key Sequence */
        UieEchoBack(UIE_ECHO_ROW1_AN, PARA_PLU_LEN);                /* Define Display Format and Max. Input Data */

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
				/* Copy Input Mnemonics to Own Buffer */
                _tcsncpy(PLUEditData.aszMnemonics, pKeyMsg->SEL.INPUT.aszKey, PARA_PLU_LEN);
            }

            /* Execute Write PLU Mnemonics Procedure */
            PLUEditData.uchMajorClass = CLASS_MAINTPLU;             /* Set Major Class */
            PLUEditData.uchMinorClass = CLASS_PARAPLU_MAINT_WRITE;  /* Set Minor Class */
            PLUEditData.usACNumber = AC_PLU_MAINT;                  /* Set A/C Number */

            if ((sError = MaintPLUEdit(&PLUEditData)) != SUCCESS) {
                return(LDT_SEQERR_ADR);
            }

            /* Set KB Mode to Neumeric Mode */
            UifACChgKBType(UIF_NEUME_KB);

            UieEchoBack(UIE_ECHO_ROW0_NUMBER, UIF_DIGIT10);     /* Set Display Format and Set Max. Input Digit */
            UieAccept();                                        /* Return to UifAC63Function() */
            return(SUCCESS);                                        
        }
		// drop through and do default processing
    default:
        return(UifACDefProc(pKeyMsg));                          /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC63ExitFunction(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No. 63 Exit Function Mode 
*===============================================================================
*/
SHORT UifAC63ExitFunction(KEYMSG *pKeyMsg) 
{
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            if (pKeyMsg->SEL.INPUT.uchLen) {                /* W/ Amount */
                return(LDT_SEQERR_ADR);
            }
            usUifACControl &= ~UIF_SHIFT_PAGE;              /* Reset Shift Page Bit */
            MaintPLUFileUpDate();                           /* UpDate PLU File Index */
            MaintPLUDEPTUnLock();                           /* UnLock PLU File */
            MaintFin(CLASS_MAINTTRAILER_PRTSUP);            /* Execute Finalize Procedure */
            UieExit(aACEnter);                              /* Return to UifACEnter() */
            return(SUCCESS);
        }
		// drop through and do default processing
    default:
        return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}
    
/*
*=============================================================================
**  Synopsis: SHORT UifAC63CancelFunction(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No. 63 Exit Function Mode 
*===============================================================================
*/
SHORT UifAC63CancelFunction(KEYMSG *pKeyMsg) 
{
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_CANCEL);                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_CANCEL:
            if (pKeyMsg->SEL.INPUT.uchLen) {                /* W/ Amount */
                return(LDT_SEQERR_ADR);
            }
            usUifACControl &= ~UIF_SHIFT_PAGE;              /* Reset Shift Page Bit */
            MaintPLUFileUpDate();                           /* UpDate PLU File Index */
            MaintPLUDEPTUnLock();                           /* UnLock PLU File */
            MaintFin(CLASS_MAINTTRAILER_PRTSUP);            /* Execute Finalize Procedure */
            UieExit(aACEnter);                              /* Return to UifACEnter() */
            return(SUCCESS);
        }
		// drop through and do default processing
    default:
        return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}
    
/*
*=============================================================================
**  Synopsis: SHORT UifAC63ErrorFunction(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No. 63 Error Function Mode 
*===============================================================================
*/
    
UISEQ CONST aszSeqAC63Error[] = {FSC_AC, FSC_CANCEL, 0};


SHORT UifAC63ErrorFunction(KEYMSG *pKeyMsg) 
{
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC63Error);                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            return(LDT_SEQERR_ADR);

        case FSC_CANCEL:
            if (pKeyMsg->SEL.INPUT.uchLen) {                /* W/ Amount */
                return(LDT_SEQERR_ADR);
            }
            usUifACControl &= ~UIF_SHIFT_PAGE;              /* Reset Shift Page Bit */
            MaintPLUDEPTUnLock();                           /* UnLock PLU File */
            MaintFin(CLASS_MAINTTRAILER_PRTSUP);            /* Execute Finalize Procedure */
            UieExit(aACEnter);                              /* Return to UifACEnter() */
            return(SUCCESS);
        }
		// drop through and do default processing
    default:
        return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}
