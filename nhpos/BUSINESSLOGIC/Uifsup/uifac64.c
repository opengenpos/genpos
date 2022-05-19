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
* Title       : PLU Price Change Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC64.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC64Function()       : A/C No.64 Function Entry Mode
*               UifAC64EnterData1()     : A/C No.64 Enter Data1 Mode
*               UifAC64EnterData2()     : A/C No.64 Enter Data2 Mode
*               UifAC64ErrorCorrect()   : A/C No.64 Error Correct Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
*    Date  : Ver.Rev.   :   Name    : Description
* Jun-16-92:00.00.01    :K.You      : initial                                   
* Jul-21-93:01.00.12    :J.IKEDA    : Adds Direct Menu Shift Key in UifAC64EnterData1()                                     
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
#include <pif.h>
#include <uie.h>
#include <fsc.h>
#include <plu.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>
#include <maint.h>
#include <uireg.h>
#include <uifsup.h>
#include <uifpgequ.h>
#include <mldmenu.h>

#include "uifsupex.h"                       /* Unique Extern Header for UI */
/*
*=============================================================================
**  Synopsis: SHORT UifAC64Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.64 Function Entry Mode  
*===============================================================================
*/

static SHORT UifAC64ExitFunction(KEYMSG *pKeyMsg);

/* Define Next Function at UIM_INIT */

static UIMENU CONST aChildAC64Init1[] = {{UifAC64EnterData1, CID_AC64ENTERDATA1},
                                     {UifAC64ErrorFunction, CID_AC64ERRORFUNCTION},
                                     {NULL,              0                 }};

/* Define Next Function at UIM_ACCEPTED from CID_AC64ERRORCORRECT, CID_AC64ENTERDATA2 */

static UIMENU CONST aChildAC64Init2[] = {{UifAC64EnterData1, CID_AC64ENTERDATA1},
                                     {UifACExitFunction, CID_ACEXITFUNCTION},
                                     {NULL,              0                 }};

/* Define Next Function at UIM_ACCEPTED */

static UIMENU CONST aChildAC64Accept[] = {{UifAC64ErrorCorrect, CID_AC64ERRORCORRECT},
                                      {UifAC64EnterData2, CID_AC64ENTERDATA2},
                                      {NULL,              0                 }};



SHORT UifAC64Function(KEYMSG *pKeyMsg) 
{
    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        switch (pKeyMsg->SEL.usFunc) {
        case CID_AC64ERRORCORRECT:
        case CID_AC64ENTERDATA2:
            UieNextMenu(aChildAC64Init2);                       /* Open Next Function */
            return(SUCCESS);

        default:
            UieNextMenu(aChildAC64Init1);                       /* Open Next Function */
            return(SUCCESS);
        }

    case UIM_ACCEPTED:
        UieNextMenu(aChildAC64Accept);                          /* Open Next Function */
        return(SUCCESS);

    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC64EnterData1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.64 Enter Data1 Mode 
*===============================================================================
*/
SHORT UifAC64EnterData1(KEYMSG *pKeyMsg) 
{
    SHORT           sError;
	TCHAR           auchPLUNumber[PLU_MAX_DIGIT + 1 + 1] = { 0 };
    PARAPLU         PLUNoData = { 0 };
    MAINTSETPAGE    SetPageNo = { 0 };
    MAINTPLU        MaintPLU = { 0 };
    FSCTBL          *pData;
    UCHAR           uchModStat;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UifACPGMLDClear();                              /* Clear MLD */

        MaintDisp(AC_PLU_PRICE_CHG,                     /* A/C Number */
                  CLASS_MAINTDSPCTL_10NPG,              /* Set Minor Class for 10N10D */
                  MaintGetPage(),                       /* Page Number */
                  0,                                    /* PTD Type */
                  0,                                    /* Report Type */
                  0,                                    /* Reset Type */
                  0L,                                   /* Amount Data */
                  LDT_NUM_ADR);                         /* "Number Entry" Lead Through Address */

        UieOpenSequence(aszSeqAC63EnterData1);                              /* Register Key Sequence */

        MaintPLU.uchMajorClass = CLASS_PARAPLU; /* Set Major Class */
        MaintPLU.uchMinorClass = CLASS_PARAPLU_RESET_EV;     /* Set Minor Class */
        MaintPLU.uchStatus = 0;                 /* Set W/ Amount Status */
        MaintPLU.usACNumber = AC_PLU_PRICE_CHG;    /* Set A/C Number */
        MaintPLU.uchEVersion = 0;               /* Set E-Version */
        MaintPLUModifier(&MaintPLU);

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
            SetPageNo.uchACNumber = AC_PLU_PRICE_CHG;                       /* Set Major Class */
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
            SetPageNo.uchACNumber = AC_PLU_PRICE_CHG;                   /* Set Major Class */
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

		case FSC_AC:
			if (pKeyMsg->SEL.INPUT.uchLen < 1) {            /* no data entered so we will exit this menu item. */
				return(UifAC64ExitFunction(pKeyMsg));
			}
		case FSC_PLU:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT14) {                   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            PLUNoData.uchMajorClass = CLASS_PARAPLU;                        /* Set Major Class */
            PLUNoData.usACNumber = AC_PLU_PRICE_CHG;                        /* Set PLU PRICE CHG. A/C Number */
            PLUNoData.uchStatus = 0;                     /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount Input Case */
                PLUNoData.uchStatus |= MAINT_WITHOUT_DATA;              /* Set W/o Amount Status */
            } else {                                                    /* W/ Amount Input Case */
                _tcscpy(PLUNoData.auchPLUNumber,
                   pKeyMsg->SEL.INPUT.aszKey);
            }
            if ((sError = MaintPLURead(&PLUNoData)) == SUCCESS) {           /* Read Function Completed */
                UieAccept();                                                /* Return to UifAC64Function() */                                        
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
            MaintPLU.usACNumber = AC_PLU_PRICE_CHG;    /* Set A/C Number */
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
            PLUNoData.usACNumber = AC_PLU_PRICE_CHG;                        /* Set PLU PRICE CHG. A/C Number */
            _tcsncpy(PLUNoData.auchPLUNumber, auchPLUNumber, PLU_MAX_DIGIT);
            //memcpy(PLUNoData.auchPLUNumber, auchPLUNumber, PLU_MAX_DIGIT);
            if ((sError = MaintPLURead(&PLUNoData)) == SUCCESS) {           /* Read Function Completed */
                UieAccept();                                                /* Return to UifAC64Function() */                                       
            }
            return(sError);
        
        case FSC_SCANNER:
            if((pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen > UIF_DIGIT14) ||  /* Over digit */
               (pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen == 0)) {           /* or no entry */
                return(LDT_KEYOVER_ADR);
            }

            PLUNoData.uchMajorClass = CLASS_PARAPLU;                        /* Set Major Class */
            PLUNoData.usACNumber = AC_PLU_PRICE_CHG;                           /* Set PLU MAINT A/C Number */
            PLUNoData.uchStatus = 0;                                    /* Set W/ Amount Status */
            PLUNoData.uchStatus |= MAINT_SCANNER_INPUT;
            _tcsncpy(PLUNoData.auchPLUNumber, pKeyMsg->SEL.INPUT.DEV.SCA.auchStr, pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen);
            //memcpy(PLUNoData.auchPLUNumber, pKeyMsg->SEL.INPUT.DEV.SCA.auchStr, pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen);
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
            MaintPLU.usACNumber = AC_PLU_PRICE_CHG;    /* Set A/C Number */
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
        MaintPLU.usACNumber = AC_PLU_PRICE_CHG;    /* Set A/C Number */
        MaintPLU.uchEVersion = 0;               /* Set E-Version */
        MaintPLUModifier(&MaintPLU);
        /* break */
    
    default:
        return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC64EnterData2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.64 Enter Data2 Mode 
*===============================================================================
*/
    
/* Define Previous Function and Key Sequence */

static UIMENU CONST aParentAC64Func[] = {{UifAC64Function, CID_AC64FUNCTION},
                                 {NULL,            0               }};

static UISEQ CONST aszSeqAC64EnterData2[] = {FSC_NUM_TYPE, FSC_ADJECTIVE, FSC_AC, 0};


SHORT UifAC64EnterData2(KEYMSG *pKeyMsg) 
{
    SHORT       sError;
	MAINTPLU    PLUEditData = { 0 };

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC64EnterData2);                              /* Register Key Sequence */
        return(SUCCESS);
                                                                            
    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
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
            PLUEditData.uchMinorClass = CLASS_PARAPLU_PRICE_READ;           /* Set Minor Class */
            PLUEditData.usACNumber = AC_PLU_PRICE_CHG;                      /* Set PLU PRICE CHG. A/C Number */
            if ((sError = MaintPLUEdit(&PLUEditData)) == SUCCESS) {         /* Edit Function Completed */
                UieReject();                                                /* Shift Current Func. to Active Func. */
            }
            return(sError);

        case FSC_ADJECTIVE:
            if (pKeyMsg->SEL.INPUT.uchLen) {                                /* W/ Amount */
                return(LDT_SEQERR_ADR);
            }

            PLUEditData.uchStatus = 0;                                      /* Set W/ Amount Status */

            /* Convert Adjective Key to Address */

            PLUEditData.uchFieldAddr = ( UCHAR)((pKeyMsg->SEL.INPUT.uchMinor)%5);       /* Set Adjective Number */
            if (!PLUEditData.uchFieldAddr) {                                            /* Adjective Key is #5 */
                PLUEditData.uchFieldAddr = 5;
            }

            /* Execute Edit PLU Procedure */
            
            PLUEditData.uchMajorClass = CLASS_MAINTPLU;                     /* Set Major Class */
            PLUEditData.uchMinorClass = CLASS_PARAPLU_PRICE_READ;           /* Set Minor Class */
            PLUEditData.usACNumber = AC_PLU_PRICE_CHG;                      /* Set PLU PRICE CHG. A/C Number */
            if ((sError = MaintPLUEdit(&PLUEditData)) == SUCCESS) {
                UieReject();                                                /* Shift Current Func. to Active Func. */
            }
            return(sError);

        case FSC_AC:                                                        /* Enter Key Case */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT7) {                   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Case */

                /* Execute Write Procedure */
                
                if ((sError = MaintPLUWrite()) == SUCCESS) {                                            
                    UieExit(aParentAC64Func);                               /* Return to UifAC63Function() */
                    return(SUCCESS);
                }
                return(sError);
            } else {                                                        /* W/ Amount Case */
                PLUEditData.uchStatus = 0;                                  /* Initialize Cashier Status */ 
                PLUEditData.uchMajorClass = CLASS_MAINTPLU;                 /* Set Major Class */
                PLUEditData.uchMinorClass = CLASS_PARAPLU_PRICE_WRITE;      /* Set Minor Class */
                PLUEditData.ulInputData = (ULONG)pKeyMsg->SEL.INPUT.lData;
                if ((sError = MaintPLUEdit(&PLUEditData)) == SUCCESS) {     /* Edit Function Completed */
                    UieAccept();
                }
                return(sError);
            }

        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                                  /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC64ErrorCorrect(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Key Sequence Error
*               
**  Description: A/C No.64 Error Correct Mode 
*===============================================================================
*/
SHORT UifAC64ErrorCorrect(KEYMSG *pKeyMsg) 
{
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_EC);                            /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_EC:
            if (pKeyMsg->SEL.INPUT.uchLen) {                        /* W/ Amount Input Case */
                return(LDT_SEQERR_ADR);
            } else {                                                /* W/o Amount Input Case */
                UieExit(aParentAC64Func);                           /* Return to UifAC64Function() */
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
**  Synopsis: SHORT UifAC64ErrorFunction(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No. 63 Error Function Mode 
*===============================================================================
*/
SHORT UifAC64ErrorFunction(KEYMSG *pKeyMsg) 
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
            MaintFin(CLASS_MAINTTRAILER_PRTSUP);            /* Execute Finalize Procedure */
            UieExit(aACEnter);                              /* Return to UifACEnter() */
            return(SUCCESS);

        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}

static SHORT UifAC64ExitFunction(KEYMSG *pKeyMsg)
{

	switch (pKeyMsg->uchMsg) {
	case UIM_INIT:
		UieOpenSequence(aszSeqAC63Error);                    /* Register Key Sequence */
		return(SUCCESS);

	case UIM_INPUT:
		switch (pKeyMsg->SEL.INPUT.uchMajor) {
		case FSC_AC:
		case FSC_CANCEL:
			if (pKeyMsg->SEL.INPUT.uchLen) {                /* W/ Amount */
				return(LDT_SEQERR_ADR);
			}
			usUifACControl &= ~UIF_SHIFT_PAGE;              /* Reset Shift Page Bit */
			MaintFin(CLASS_MAINTTRAILER_PRTSUP);            /* Execute Finalize Procedure */
			UieExit(aACEnter);                              /* Return to UifACEnter() */
			return(SUCCESS);

			/* default:
			break; */
		}
		/* break; */

	default:
		return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
	}
}
