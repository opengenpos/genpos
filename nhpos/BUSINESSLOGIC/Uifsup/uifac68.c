/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-2000       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : PLU Addition/Deletion Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC68.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC68Function()       : A/C No.68 Function Entry Mode
*               UifAC68EnterData1()     : A/C No.68 Enter Data1 Mode
*               UifAC68EnterPLUType()   : A/C No.68 Enter PLU Type Mode
*               UifAC68EnterData3()     : A/C No.68 Enter Data3 Mode
*               UifAC68Intervation()    : A/C No.68 Intervation Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jun-16-92:00.00.01    :K.You      : initial                                   
* Jul-21-93:01.00.12    :J.IKEDA    : Adds Direct Menu Shift Key in UifAC68EnterData1()                                   
* Apr-14-95:03.00.00    :T.Satoh    : Adds clear MLD
* Jan-12-00:01.00.00    :hrkato     : Saratoga
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
#include <tchar.h>
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
#include <cvt.h>
#include <mldmenu.h>

#include "uireg.h"
#include "uifsupex.h"                       /* Unique Extern Header for UI */
/*
*=============================================================================
**  Synopsis: SHORT UifAC68Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.68 Function Entry Mode  
*===============================================================================
*/

static SHORT   UifAC68EnterData1(KEYMSG *pKeyMsg);
static SHORT   UifAC68EnterPLUType(KEYMSG *pKeyMsg);
static SHORT   UifAC68EnterData3(KEYMSG *pKeyMsg);
static SHORT   UifAC68Intervation(KEYMSG *pKeyMsg);
static SHORT   UifAC68EnterALPHA(KEYMSG *pKeyMsg);

/* Define Next Function at UIM_INIT */

static UIMENU  aChildAC68Init1[] = {{UifAC68EnterData1, CID_AC68ENTERDATA1},
                                     {UifAC63ErrorFunction, CID_AC63ERRORFUNCTION},
                                     {NULL,              0                 }};

/* Define Next Function at UIM_INIT from CID_AC68ENTERDATA3 */

static UIMENU  aChildAC68Init2[] = {{UifAC63ExitFunction, CID_AC63EXITFUNCTION},
                                     {UifAC68EnterData1, CID_AC68ENTERDATA1},
                                     {UifAC63CancelFunction, CID_AC63CANCELFUNCTION},
                                     {NULL,              0                 }};

/* Define Next Function at UIM_ACCEPTED from CID_AC68ENTERDATA1 */

static UIMENU  aChildAC68Accept1[] = {{UifAC68EnterPLUType, CID_AC68ENTERPLUTYPE},
                                       {UifAC63CancelFunction, CID_AC63CANCELFUNCTION},
                                       {NULL,                0                 }};

/* Define Next Function at UIM_ACCEPTED from CID_AC68ENTERPLUTYPE */

static UIMENU  aChildAC68Accept2[] = {{UifAC68EnterData1, CID_AC68ENTERDATA1},
                                       {UifAC63ExitFunction, CID_AC63EXITFUNCTION},
                                       {UifAC63CancelFunction, CID_AC63CANCELFUNCTION},
                                       {NULL,              0                 }};

static UIMENU  aParentAC68Func[] = {{UifAC68Function, CID_AC68FUNCTION},
                                     {NULL,            0               }};



SHORT UifAC68Function(KEYMSG *pKeyMsg) 
{
	MAINTREDISP ReDisplay = {0};
	SHORT sError;

    switch (pKeyMsg->uchMsg) {
    case UIM_CANCEL:
        usUifACControl &= UIF_AC68_DEL;                         /* Clear Control Save Area */

        /* Reset All Descriptor W/o ALPHA */
        ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
        ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLRWOALPH;
        MaintReDisp(&ReDisplay);
        return(SUCCESS);

    case UIM_INIT:
        switch (pKeyMsg->SEL.usFunc) {
        case CID_AC68ENTERDATA3:
        case CID_AC68ENTERPLUTYPE:
            UieNextMenu(aChildAC68Init2);                       /* Open Next Function */
            return(SUCCESS);

        default:
            usUifACControl = 0;                                 /* Clear Control Flag */
            UieNextMenu(aChildAC68Init1);                       /* Open Next Function */
            return(SUCCESS);
        }

    case UIM_ACCEPTED:
        switch (pKeyMsg->SEL.usFunc) {
        case CID_AC68ENTERDATA1:
            UieNextMenu(aChildAC68Accept1);                     /* Open Next Function */
            return(SUCCESS);

        default:
            UieNextMenu(aChildAC68Accept2);                     /* Open Next Function */
            return(SUCCESS);
        }
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
		}

    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC68EnterData1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.68 Enter Data1 Mode 
*===============================================================================
*/
SHORT UifAC68EnterData1(KEYMSG *pKeyMsg) 
{
    SHORT           sError;
    TCHAR           auchPLUNumber[PLU_MAX_DIGIT + 1] = {0};
    PARAPLU         PLUNoData = {0};
	MAINTSETPAGE    SetPageNo = {0};
    MAINTPLU        MaintPLU = {0};
    UCHAR           uchModStat;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UifACPGMLDClear();                              /* Clear MLD */

        MaintDisp(AC_PLU_ADDDEL,                        /* A/C Number */
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
        MaintPLU.usACNumber = AC_PLU_ADDDEL;    /* Set A/C Number */
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
            SetPageNo.uchACNumber = AC_PLU_ADDDEL;                          /* Set Major Class */
            SetPageNo.uchLeadAddr = LDT_NUM_ADR;
            if ((sError = MaintSetPage(&SetPageNo)) == SUCCESS) {
				FSCTBL   *pData = (FSCTBL *)&ParaFSC[uchMaintMenuPage - 1];

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
            SetPageNo.uchACNumber = AC_PLU_ADDDEL;                      /* Set Major Class */
            SetPageNo.uchLeadAddr = LDT_NUM_ADR;
            SetPageNo.uchPageNumber = pKeyMsg->SEL.INPUT.uchMinor;      /* set page number */
            SetPageNo.uchStatus = 0;                                    /* Set W/ Amount Status */
            if ((sError = MaintSetPage(&SetPageNo)) == SUCCESS) {
				FSCTBL   *pData = (FSCTBL *)&ParaFSC[uchMaintMenuPage - 1];

                UieSetFsc(pData);
                UieReject();                                            /* Shift Current Func. to Active Func. */
            }
            return(sError);

        case FSC_PLU:
            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT14) {                   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            PLUNoData.uchMajorClass = CLASS_PARAPLU;                        /* Set Major Class */
            PLUNoData.usACNumber = AC_PLU_ADDDEL;                           /* Set PLU MAINT A/C Number */
            PLUNoData.uchStatus = 0;                                    /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount Input Case */
                PLUNoData.uchStatus |= MAINT_WITHOUT_DATA;              /* Set W/o Amount Status */
            } else {                                                    /* W/ Amount Input Case */
                _tcsncpy(PLUNoData.auchPLUNumber, pKeyMsg->SEL.INPUT.aszKey, PLU_MAX_DIGIT);
            }
            if ((sError = MaintPLURead(&PLUNoData)) == SUCCESS) {           /* Read Function Completed */
                usUifACControl |= UIF_AC68_DEL;                             /* Set Deletion Case Status */
                UieAccept();                                                /* Return to UifAC68Function() */                   
            } else if (sError == LDT_NTINFL_ADR) {                            /* Not In File Case */
                UieAccept();                                                /* Return to UifAC68Function() */                   
                return(SUCCESS);
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
            PLUNoData.usACNumber = AC_PLU_ADDDEL;                           /* Set A/C Number */
            _tcsncpy(PLUNoData.auchPLUNumber, auchPLUNumber, PLU_MAX_DIGIT);
            if ((sError = MaintPLURead(&PLUNoData)) == SUCCESS) {           /* Read Function Completed */
                usUifACControl |= UIF_AC68_DEL;                             /* Set Deletion Case Status */
                UieAccept();                                                /* Return to UifAC68Function() */                   
            } else if (sError == LDT_NTINFL_ADR) {                            /* Not In File Case */
                UieAccept();                                                /* Return to UifAC68Function() */                   
                return(SUCCESS);
            }
            return(sError);
        
        case FSC_SCANNER:
            if((pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen > UIF_DIGIT14) ||  /* Over digit */
               (pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen == 0)) {           /* or no entry */
                return(LDT_KEYOVER_ADR);
            }

            PLUNoData.uchMajorClass = CLASS_PARAPLU;                        /* Set Major Class */
            PLUNoData.usACNumber = AC_PLU_ADDDEL;                           /* Set PLU MAINT A/C Number */
            PLUNoData.uchStatus = 0;                                    /* Set W/ Amount Status */
            PLUNoData.uchStatus |= MAINT_SCANNER_INPUT;
            _tcsncpy(PLUNoData.auchPLUNumber, pKeyMsg->SEL.INPUT.DEV.SCA.auchStr, pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen);
            if ((sError = MaintPLURead(&PLUNoData)) == SUCCESS) {           /* Read Function Completed */
                usUifACControl |= UIF_AC68_DEL;                             /* Set Deletion Case Status */
                UieAccept();                                                /* Return to UifAC68Function() */                   
            }
            else if (sError == LDT_NTINFL_ADR) {                            /* Not In File Case */
                UieAccept();                                                /* Return to UifAC68Function() */                   
                return(SUCCESS);
            }
            return(sError);
            
        case FSC_E_VERSION:
            if (pKeyMsg->SEL.INPUT.uchLen) {        /* With Amount ? */
                return(LDT_KEYOVER_ADR);
            }
            MaintPLU.uchMajorClass = CLASS_PARAPLU; /* Set Major Class */
            MaintPLU.uchMinorClass = CLASS_PARAPLU_SET_EV;     /* Set Minor Class */
            MaintPLU.uchStatus = 0;                 /* Set W/ Amount Status */
            MaintPLU.usACNumber = AC_PLU_ADDDEL;    /* Set A/C Number */
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
        MaintPLU.usACNumber = AC_PLU_ADDDEL;    /* Set A/C Number */
        MaintPLU.uchEVersion = 0;               /* Set E-Version */
        MaintPLUModifier(&MaintPLU);
        /* break; */
    
    default:
        return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC68EnterPLUType(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.68 Enter PLU Type Mode 
*===============================================================================
*/
    
/* Define Next Function at UIM_ACCEPTED from CID_AC68ENTERALPHA */

static UIMENU  aChildAC68EnterPLUType[] = {{UifAC68EnterData3, CID_AC68ENTERDATA3},
                                            {UifAC63CancelFunction, CID_AC63CANCELFUNCTION},
                                            {NULL,              0                 }};


static UISEQ  aszSeqAC68EnterPLUType[] = {FSC_AC, FSC_VOID, FSC_EC, FSC_STRING, FSC_KEYED_STRING, 0};



SHORT UifAC68EnterPLUType(KEYMSG *pKeyMsg) 
{
    SHORT       sError;
    MAINTPLU    PLUEditData = {0};   
	MAINTREDISP ReDisplay = {0};
                            
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC68EnterPLUType);                /* Register Key Sequence */
        return(SUCCESS);
                                                                            
    case UIM_ACCEPTED:
        UieNextMenu(aChildAC68EnterPLUType);                    /* Open Next Function */
        return(SUCCESS);

    case UIM_CANCEL:
        usUifACControl &= UIF_AC68_DEL;                         /* Clear Control Save Area */

        /* Reset All Descriptor W/o ALPHA */
        ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
        ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLRWOALPH;
        MaintReDisp(&ReDisplay);
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

        case FSC_AC:                                                                        /* Enter Key Case */
            /* Delete Operation and W/o VOID */
            if (!(usUifACControl & UIF_VOID) && (usUifACControl & UIF_AC68_DEL)) {          
                return(LDT_SEQERR_ADR);
            }

            /* W/o Amount and VOID */
            if (!pKeyMsg->SEL.INPUT.uchLen && (usUifACControl & UIF_VOID)) {         /* W/ Amount Case */
                if ((sError = MaintPLUDelete()) == SUCCESS) {
                    UieAccept();
                    usUifACControl = 0;                                     /* Clear Control Flag */
     
                    /* Reset All Descriptor W/o ALPHA */
                    ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
                    ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLRWOALPH;
                    MaintReDisp(&ReDisplay);
                }
                return(sError);
            }

            /* W/ Amount and VOID */
            if ((pKeyMsg->SEL.INPUT.uchLen) && (usUifACControl & UIF_VOID)) {
                return(LDT_SEQERR_ADR);
            }

            /* W/o Amount and Addtion Operation */
            if (!pKeyMsg->SEL.INPUT.uchLen && !(usUifACControl & UIF_AC68_DEL)) {               
                return(LDT_SEQERR_ADR);
            } else {                                                        /* W/ Amount */
                 /* Check Digit */
                 if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {              /* Over Digit */
                    return(LDT_KEYOVER_ADR);
                }
                PLUEditData.uchStatus = 0;                                  /* Initialize Cashier Status */ 
                PLUEditData.uchMajorClass = CLASS_MAINTPLU;                 /* Set Major Class */
                PLUEditData.uchMinorClass = CLASS_PARAPLU_ADD_WRITE;        /* Set Minor Class */
                PLUEditData.usACNumber = AC_PLU_ADDDEL;                     /* Set PLU ADD A/C Number */
                PLUEditData.ulInputData = (ULONG)pKeyMsg->SEL.INPUT.lData;
                if ((sError = MaintPLUEdit(&PLUEditData)) == SUCCESS) {
                    UieNextMenu(aChildAC68EnterPLUType);                    /* Open Next Function */
                }
                return(sError);
            }

        case FSC_VOID:
            /*  Check W/ Amount */
            if (pKeyMsg->SEL.INPUT.uchLen) {                        /* W/ Amount */
                return(LDT_SEQERR_ADR);
            }
            if (usUifACControl & UIF_VOID) {                        /* VOID Key Already Put */
                return(LDT_SEQERR_ADR);
            }
            if (!(usUifACControl & UIF_AC68_DEL)) {                 /* Delete Operation Case */
                return(LDT_SEQERR_ADR);
            } else {
                usUifACControl |= UIF_VOID;                         /* Set VOID Key Put Status */
                UieReject();                                        /* Clear Uie Retry Buffer */

                /* Set VOID Descriptor */
                ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
                ReDisplay.uchMinorClass = CLASS_MAINTREDISP_VOID;
                MaintReDisp(&ReDisplay);
                return(SUCCESS);
            }

        case FSC_EC:
            if (pKeyMsg->SEL.INPUT.uchLen) {                        /* W/ Amount Input Case */
                return(LDT_SEQERR_ADR);
            } else {                                                /* W/o Amount Input Case */
                usUifACControl = 0;                                 /* Clear Control Save Area */

                /* Reset All Descriptor W/o ALPHA */
                ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
                ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLRWOALPH;
                MaintReDisp(&ReDisplay);
                UieExit(aParentAC68Func);                           /* Return to UifAC63Function() */
                return(SUCCESS);
            }

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
**  Synopsis: SHORT UifAC68EnterData3(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.68 Enter Data3 Mode 
*===============================================================================
*/
    
/* Define Next/Previous Function and Key Sequence */

static UIMENU  aChildAC68EnterData31[] = {{UifACShift, CID_ACSHIFT},
                                           {UifAC68EnterALPHA, CID_AC68ENTERALPHA},
                                           {UifACChangeKB1, CID_ACCHANGEKB1},
                                           {NULL,           0              }};

static UIMENU  aChildAC68EnterData32[] = {{UifAC68Intervation, CID_AC68INTERVATION},
                                           {NULL,           0              }};




SHORT UifAC68EnterData3(KEYMSG *pKeyMsg) 
{
    SHORT       sError;
	MAINTPLU    PLUEditData = {0};   
                            
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC63EnterData2);                  /* Register Key Sequence */
        return(SUCCESS);
                                                                            
    case UIM_ACCEPTED:
        switch(pKeyMsg->SEL.usFunc) {
        case CID_ACCHANGEKB1:
            /* Display Next Address */
            PLUEditData.uchMajorClass = CLASS_MAINTPLU;
            PLUEditData.uchMinorClass = CLASS_PARAPLU_ADD_READ;
            PLUEditData.uchStatus = MAINT_WITHOUT_DATA;         /* Set W/o Amount Status */
            MaintPLUEdit(&PLUEditData);

        default:
            break;
        }
        UieAccept();                                            /* Return to UifAC68EnterPLUType() */
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

        case FSC_NUM_TYPE:                                      /* Address Key Case */
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {       /* Over Digit */
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
            PLUEditData.uchMinorClass = CLASS_PARAPLU_ADD_READ;             /* Set Minor Class */
            PLUEditData.usACNumber = AC_PLU_ADDDEL;                         /* Set PLU MAINT A/C Number */

            if ((sError = MaintPLUEdit(&PLUEditData)) == MAINT_PLU_MNEMO_ADR) {     /* Next Edit is Mnemonic Set */
                UieNextMenu(aChildAC68EnterData31);                         /* Open Next Function() */
                return(SUCCESS);
            }
            if (sError == SUCCESS) {
                UieAccept();                                                
            }
            return(sError);

        case FSC_AC:                                                        /* Enter Key Case */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Case */
                /* Execute Write Procedure */
                if ((sError = MaintPLUWrite()) == SUCCESS || sError == LDT_FLFUL_ADR) { /* Success or File Full */                                            
                    UieExit(aParentAC68Func);                                   /* Return to UifAC68Function() */
                }
                return(sError);
            } else {                                                            /* W/ Amount Case */
                /* Check Digit */
                if (pKeyMsg->SEL.INPUT.uchLen > PARA_PLU_LEN) {                 /* Over Digit */
                    return(LDT_KEYOVER_ADR);
                }
                PLUEditData.uchStatus = 0;                                  /* Initialize Cashier Status */ 
                PLUEditData.uchMajorClass = CLASS_MAINTPLU;                 /* Set Major Class */
                PLUEditData.uchMinorClass = CLASS_PARAPLU_ADD_WRITE;        /* Set Minor Class */
                PLUEditData.usACNumber = AC_PLU_ADDDEL;                     /* Set PLU MAINT A/C Number */
                PLUEditData.ulInputData = (ULONG)pKeyMsg->SEL.INPUT.lData;

                _tcsncpy(PLUEditData.aszMnemonics, pKeyMsg->SEL.INPUT.aszKey, PARA_PLU_LEN);
                if ((sError = MaintPLUEdit(&PLUEditData)) == SUCCESS) {     /* Edit Function Completed */
                    UieAccept();                                            /* Return to UifAC68EnterPLUType() */
                    return(SUCCESS);
                } else if (sError == MAINT_PLU_CPYDEPT_ADR) {               /* Copy Department Ope. Case */
                    UieNextMenu(aChildAC68EnterData32);
                    return(SUCCESS);
                } else if (sError == MAINT_PLU_MNEMO_ADR) {                 /* Mnemonics Set Case */
                    UieNextMenu(aChildAC68EnterData31);                     /* Open Next Function */                                       
                    return(SUCCESS);
                }
                return(sError);
            }
        case FSC_ADJECTIVE:
            if (pKeyMsg->SEL.INPUT.uchLen) {                                /* W/ Amount */
                return(LDT_SEQERR_ADR);
            }

            /* Check FSC Minor Data Range */
            if (0 == pKeyMsg->SEL.INPUT.uchMinor || FSC_ADJ_MAX < pKeyMsg->SEL.INPUT.uchMinor) { /* Over Range */
                return(LDT_KEYOVER_ADR);
            }

            /* Convert Adjective Key to Address */
            PLUEditData.uchStatus = 0;                                      /* Set W/ Amount Status */
            PLUEditData.uchFieldAddr = ( UCHAR)((pKeyMsg->SEL.INPUT.uchMinor) % 5);   /* Set Adjective Number */
            if (!PLUEditData.uchFieldAddr) {                                        /* Adjective Key is #5 */
                PLUEditData.uchFieldAddr = 5;
            }
            PLUEditData.uchFieldAddr += PLU_FIELD18_ADR;

            /* Execute Edit PLU Procedure */
            PLUEditData.uchMajorClass = CLASS_MAINTPLU;                     /* Set Major Class */
            PLUEditData.uchMinorClass = CLASS_PARAPLU_ADD_READ;             /* Set Minor Class */
            PLUEditData.usACNumber = AC_PLU_ADDDEL;                         /* Set A/C Number */
            if ((sError = MaintPLUEdit(&PLUEditData)) == SUCCESS) {
                UieAccept();                                                /* Return to UifAC68EnterPLUType() */
            }
            return(sError);
        
        case FSC_EC:
            if (pKeyMsg->SEL.INPUT.uchLen) {                        /* W/ Amount Input Case */
                return(LDT_SEQERR_ADR);
            } else {                                                /* W/o Amount Input Case */
                UieExit(aParentAC68Func);                           /* Return to UifAC63Function() */
                return(SUCCESS);
            }

        /* ---- scroll control for plu maintenance R3.1 ---- */
        case FSC_SCROLL_UP:
            if (pKeyMsg->SEL.INPUT.uchLen) {                        /* W/ Amount Input Case */
                return(LDT_SEQERR_ADR);
            } else {                                                /* W/o Amount Input Case */
                MaintPLUDisplayFirstPage();                         /* display 1st page */
            }
            UieAccept();                                                
            return(SUCCESS);

        case FSC_SCROLL_DOWN:
            if (pKeyMsg->SEL.INPUT.uchLen) {                        /* W/ Amount Input Case */
                return(LDT_SEQERR_ADR);
            } else {                                                /* W/o Amount Input Case */
                MaintPLUDisplaySecondPage();                        /* display 2nd page */
            }
            UieAccept();                                                
            return(SUCCESS);

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
**  Synopsis: SHORT UifAC68Intervation(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.68 Intervation Mode 
*===============================================================================
*/
    
/* Define Key Sequence */

static UISEQ  aszSeqAC68Intervation[] = {FSC_NUM_TYPE, FSC_ADJECTIVE, FSC_AC, FSC_CANCEL, FSC_EC, 0};


SHORT UifAC68Intervation(KEYMSG *pKeyMsg) 
{
	MAINTPLU    PLUEditData = {0};
    SHORT       sReturn;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC68Intervation);                              /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        /* Check Digit */
        if (pKeyMsg->SEL.INPUT.uchLen) {                                    /* W/ Amount */
            return(LDT_SEQERR_ADR);
        }
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            if ((sReturn = MaintPLUCopy()) != SUCCESS) {
                return(sReturn);
            }
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
**  Synopsis: SHORT UifAC68EnterALPHA(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: A/C No.68 Enter PLU Mnemonics Mode 
*===============================================================================
*/
SHORT UifAC68EnterALPHA(KEYMSG *pKeyMsg) 
{    
    SHORT       sError;
	MAINTPLU    PLUEditData = {0};   

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
                _tcsncpy(PLUEditData.aszMnemonics,  pKeyMsg->SEL.INPUT.aszKey, PARA_PLU_LEN);
            }

            /* Execute Write PLU Mnemonics Procedure */
            PLUEditData.uchMajorClass = CLASS_MAINTPLU;             /* Set Major Class */
            PLUEditData.uchMinorClass = CLASS_PARAPLU_ADD_WRITE;    /* Set Minor Class */
            PLUEditData.usACNumber = AC_PLU_ADDDEL;                 /* Set A/C Number */

            if ((sError = MaintPLUEdit(&PLUEditData)) != SUCCESS) {
                return(LDT_SEQERR_ADR);
            }

            /* Set KB Mode to Neumeric Mode */
            UifACChgKBType(UIF_NEUME_KB);
            UieEchoBack(UIE_ECHO_ROW0_NUMBER, UIF_DIGIT10);             /* Set Display Format and Set Max. Input Digit */
            UieAccept();                                                /* Return to UifACXXXFunction() */
            return(SUCCESS);                                        

        /* default:
              break; */

        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                              /* Execute Default Procedure */
    }
}

/* --- End of Source File --- */