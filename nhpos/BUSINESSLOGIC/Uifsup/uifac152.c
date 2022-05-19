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
* Title       : Employee Number Assignment Module  A/C 152.
* Category    : User Interface For Supervisor, NCR 2170 US Enhanced Hospitality Application         
* Program Name: UIFAC152.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC152Function()   : A/C No.152 Function Entry Mode
*               UifAC152EnterData1() : A/C No.152 Enter Data1 Mode
*               UifAC152EnterData2() : A/C No.152 Enter Data2 Mode
*               UifAC152EnterData3() : A/C No.152 Enter Data3 Mode
*               UifAC152EnterData4() : A/C No.152 Enter Data4 Mode
*               UifAC152EnterALPHA() : A/C No.152 Enter Alpha Mnemonics Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
*    Date  : Ver.Rev.   :   Name    : Description
* Jun-21-92:00.00.01    :M.Yamamoto : initial                                   
* Apr-14-95:03.00.00    :T.Satoh    : Adds clear MLD
* Nov-27-95:03.01.00    :M.Ozawa    : Adds alpha mnemonic entry
*          :            :                                    
*===========================================================================
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
*       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
*       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
*       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
*       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
*       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
*       2016  -> GenPOS Rel 2.3.0 (Windows 7, completely Out of Scope, Visual Studio 2013.)
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
*    moved from Visual Studio 2005 to Visual Studio 2013 with Rel 2.3.0
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
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <uifsup.h>
#include <uifpgequ.h>
#include <cvt.h>
#include <mldmenu.h>

#include "uifsupex.h"                       /* Unique Extern Header for UI */
                        
/*
==============================================================================
                      L O C A L   D E F I N I T I O N s                         
==============================================================================
*/

#define UIF_DESC_NOT_CLR    1;     

/*
*=============================================================================
**  Synopsis: SHORT UifAC152Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               
**  Description: A/C No.152 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC152Init[] = {{UifAC152EnterData1, CID_AC152ENTERDATA1},
                                     {UifAC153CancelFunction, CID_AC153CANCELFUNCTION},
                                     {NULL,                   0                      }};


/* Define Next Function at UIM_ACCEPTED */

UIMENU FARCONST aChildAC152Accept[] = {{UifAC152EnterData3, CID_AC152ENTERDATA3},
                                      {UifAC153CancelFunction, CID_AC153CANCELFUNCTION},
                                      {NULL,                   0                      }};



SHORT UifAC152Function(KEYMSG *pKeyMsg) 
{

    MAINTREDISP     ReDisplay;

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        switch (pKeyMsg->SEL.usFunc) {
        case CID_AC152ENTERDATA3:
        case CID_AC152ENTERDATA2:
            usUifACControl = 0;                                  /* Clear Control Flag */

            /* Reset All Descriptor */

            ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
            ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLR;
            MaintReDisp(&ReDisplay);

            UieNextMenu(aChildAC152Accept);                       /* Open Next Function */
            return(SUCCESS);

        default:
            usUifACControl = 0;                     /* Clear Control Flag */
            UieNextMenu(aChildAC152Init);           /* Open Next Function */
            return(SUCCESS);
        }

    case UIM_ACCEPTED:
        usUifACControl = 0;                                     /* Clear Control Flag */

        /* Reset All Descriptor */

        ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
        ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLR;
        MaintReDisp(&ReDisplay);

        UieNextMenu(aChildAC152Accept);                          /* Open Next Function */
        break;

    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC152EnterData1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.152 Enter Data1 Mode 
*===============================================================================
*/
    
/* Define Next Function and Key Sequence */


UIMENU FARCONST aChildAC152EnterData1[] = {{UifAC152EnterData2, CID_AC152ENTERDATA2},
                                           {UifAC153CancelFunction, CID_AC153CANCELFUNCTION},
                                           {NULL,              0                 }};

UISEQ FARCONST aszSeqAC152EnterData1[] = {FSC_VOID, FSC_AC, 0};


SHORT UifAC152EnterData1(KEYMSG *pKeyMsg)
{
    
    SHORT           sError;
    MAINTREDISP     ReDisplay;
    PARAEMPLOYEENO  EmpNoData;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC152EnterData1);         /* Register Key Sequence */
        uchUifACDescCtl = 0;                            /* Initialize Uif Descriptor Control Save Area */

        UifACPGMLDClear();                              /* Clear MLD */

        /* Display Entry Display */

        MaintDisp(AC_ETK_ASSIN,                         /* A/C Number */
                  0,                                    /* Descriptor */
                  0,                                    /* Page Number */
                  0,                                    /* PTD Type */
                  0,                                    /* Report Type */
                  0,                                    /* Reset Type */
                  0L,                                   /* Amount Data */
                  LDT_NUM_ADR);                         /* " Enter Employee #" Lead Through Address */

        return(SUCCESS);
        break;

    case UIM_CANCEL:

        if (!uchUifACDescCtl) {                         /* Clear Descriptor Case */
            usUifACControl = 0;                         /* Initialize Uif Control Save Area */

            /* Reset All Descriptor W/o ALPHA */
            ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
            ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLRWOALPH;
            MaintReDisp(&ReDisplay);
        }
        return(SUCCESS);
        break;

    case UIM_ACCEPTED:

        UieNextMenu(aChildAC152EnterData1);
        return(SUCCESS);
        break;

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Case */
                if (usUifACControl & (UIF_VOID )) {                         /* VOID,# TYPE Key Already Put */
                    return(LDT_SEQERR_ADR);
                }
                MaintEmployeeNoReport(CLASS_PARAEMPLOYEENO);                /* Execute Report Procedure */
                RptETKUnLock();                                             /* Unlock ETK file */
                UieExit(aACEnter);                                          /* Return to UifACEnter() */
                return(SUCCESS);
            } else {                                                        /* W/ Amount Case */
                /* Check Digit */

                if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT9) {               /* Over Digit */
                    return(LDT_KEYOVER_ADR);
                }
                EmpNoData.uchStatus = 0;                                    /* Initialize Employee Status */ 
                EmpNoData.uchMajorClass = CLASS_PARAEMPLOYEENO;             /* Set Major Class */
                EmpNoData.ulEmployeeNo = (ULONG)pKeyMsg->SEL.INPUT.lData;
                if (usUifACControl & UIF_VOID) {                            /* VOID Key Already Put */
                    if ((sError = MaintEmployeeNoDelete(&EmpNoData)) == SUCCESS) {
                        UieAccept();                                        /* Return UifAC152Function() */
                    }
                    return(sError);
                } else {
                    EmpNoData.uchMinorClass = CLASS_PARAEMPLOYEENO_INT;     /* Set Minor Class */
                    EmpNoData.uchFieldAddr  = 1;                            /* Field Addres 1 */
                    if ((sError = MaintEmployeeNoRead(&EmpNoData)) == SUCCESS) { 
                        UieNextMenu(aChildAC152EnterData1);                 /* Open Next Function */
                    }
                    return(sError);
                }
            }

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
                UieReject();                                                /* Shift Current Func. to Active Func. */ 
                return(SUCCESS);
            }


        case FSC_NUM_TYPE:
            return(LDT_SEQERR_ADR);
            break;

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
**  Synopsis: SHORT UifAC152EnterData2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.152 Enter Data2 Mode 
*===============================================================================
*/

/* Define Previous Function */

UIMENU FARCONST aParentAC152EnterData2[] = {{UifAC152Function, CID_AC152FUNCTION},
                                           {NULL,            0               }};
    
/* Define Next Function */

UIMENU FARCONST aChildAC152EnterData2[] = {{UifACShift, CID_ACSHIFT},
                                           {UifAC152EnterALPHA, CID_AC152ENTERALPHA},
                                           {UifACChangeKB1, CID_ACCHANGEKB1},
                                           {NULL,           0              }};

/* UIMENU FARCONST aChildAC152EnterData2[] = {{UifAC152EnterData2, CID_AC152ENTERDATA2},
                                              {UifAC153CancelFunction, CID_AC153CANCELFUNCTION},
                                              {NULL,              0                 }};

UIMENU FARCONST aChildAC152EnterData3[] = {{UifAC152EnterData3, CID_AC152ENTERDATA3},
                                           {UifAC153CancelFunction, CID_AC153CANCELFUNCTION},
                                           {NULL,              0                 }}; */

/* Define Key Sequence */

UISEQ FARCONST aszSeqAC152EnterData2[] = {FSC_AC, FSC_NUM_TYPE, FSC_VOID, 0};


SHORT UifAC152EnterData2(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARAEMPLOYEENO  EmpNoData;   
                       

    switch(pKeyMsg->uchMsg) {
    case UIM_CANCEL:
        /* usUifACControl = 0;                                 Initialize Uif Control Save Area */

        /* Reset All Descriptor 

        ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
        ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLR;
        MaintReDisp(&ReDisplay); */
        return(SUCCESS);    

    case UIM_ACCEPTED:
        switch(pKeyMsg->SEL.usFunc) {
        case CID_ACCHANGEKB1:
            UieOpenSequence(aszSeqAC152EnterData2);                              /* Register Key Sequence */

            /* Display Next Address */
            MaintEmployeeNoIncJob(&EmpNoData);

        default:
            break;
        }
        UieAccept();                                            /* Return to UifAC152Function() */
        return(SUCCESS);

/*    case UIM_ACCEPTED:
        if (usUifACControl & UIF_VOID) {                                    
            UieOpenSequence(aszSeqAC152EnterData2);                          
            MaintEmployeeNoDisp(&EmpNoData);                            
            return(SUCCESS);
        } else {
            UieAccept();
            return(SUCCESS);
        }
*/            
    case UIM_INIT:
        UieOpenSequence(aszSeqAC152EnterData2);                             /* Register Key Sequence */
        /* Control Display */
        MaintEmployeeNoDisp(&EmpNoData);                                    /* A/C Number */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_VOID:
            return(LDT_SEQERR_ADR);
        case FSC_AC:
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Case */
                sError = MaintEmployeeNoWrite();
                UieExit(aParentAC152EnterData2);
                return(sError);
            } else {                                                        /* W/ Amount Case */
                /* Check Digit */
                if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {               /* Over Digit */
                    return(LDT_KEYOVER_ADR);
                }
                EmpNoData.uchJobCode1 = (UCHAR)pKeyMsg->SEL.INPUT.lData;    /* save input data */
                EmpNoData.uchStatus = 0;                                    /* Set W/ Amount Status */
                EmpNoData.uchMajorClass = CLASS_PARAEMPLOYEENO;             /* Set Major Class */
                EmpNoData.uchMinorClass = CLASS_PARAEMPLOYEENO_2ND;         /* Set Minor Class */
                if ((sError = MaintEmployeeNoEdit(&EmpNoData)) == SUCCESS) {
                    UieAccept();                                            /* Return UifAC152Function() */
                } else if (sError == MAINT_EMPLOYEE_MNEMO_ADR) {
                    UieNextMenu(aChildAC152EnterData2);                     /* Open Next Function */                                        
                    return(SUCCESS);
                }
                return(sError);
            }
        case FSC_NUM_TYPE:
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Input Case */
                if ((sError = MaintEmployeeNoIncJob(&EmpNoData)) == SUCCESS) {         /* A/C Number */
                    UieAccept();
                    return(SUCCESS);
                } else if (sError == MAINT_EMPLOYEE_MNEMO_ADR) {            /* Mnemonics Setting Case */
                    UieNextMenu(aChildAC152EnterData2);                     /* Open Next Function */
                    return(SUCCESS);
                }
            } else {
                if ((pKeyMsg->SEL.INPUT.lData > (LONG)ETK_FIELD4_ADR) ||
                   (pKeyMsg->SEL.INPUT.lData <= 0L)) {
                    return(LDT_KEYOVER_ADR);
                } else if (pKeyMsg->SEL.INPUT.lData == (LONG)ETK_FIELD4_ADR) {
                    EmpNoData.uchFieldAddr = (UCHAR)pKeyMsg->SEL.INPUT.lData;
                    MaintEmployeeNoDispJob(&EmpNoData);                        /* A/C Number */
                    UieNextMenu(aChildAC152EnterData2);                     /* Open Next Function */
                    return(SUCCESS);
                } else {
                    EmpNoData.uchFieldAddr = (UCHAR)pKeyMsg->SEL.INPUT.lData;
                    MaintEmployeeNoDispJob(&EmpNoData);                        /* A/C Number */
                    UieAccept();
                    return(SUCCESS);
                }
            }
            break;
        default:
            return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
            break;
        }
        /* break; */
    default:
        return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}
                    
/*
*=============================================================================
**  Synopsis: SHORT UifAC152EnterData3(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.152 Enter Data3 Mode 
*===============================================================================
*/

/* Define Previous Function */


SHORT UifAC152EnterData3(KEYMSG *pKeyMsg) 
{
    SHORT           sError;
    PARAEMPLOYEENO  EmpNoData;   
    MAINTREDISP     ReDisplay;
                    
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC152EnterData1);         /* Register Key Sequence */
        uchUifACDescCtl = 0;                            /* Initialize Uif Descriptor Control Save Area */

        UifACPGMLDClear();                              /* Clear MLD */

        /* Display Entry Display */
        MaintDisp(AC_ETK_ASSIN,                         /* A/C Number */
                  0,                                    /* Descriptor */
                  0,                                    /* Page Number */
                  0,                                    /* PTD Type */
                  0,                                    /* Report Type */
                  0,                                    /* Reset Type */
                  0L,                                   /* Amount Data */
                  LDT_NUM_ADR);                         /* " Enter Employee #" Lead Through Address */

        return(SUCCESS);

    case UIM_ACCEPTED:

        UieNextMenu(aChildAC152EnterData1);
        return(SUCCESS);
        break;

    case UIM_CANCEL:
        if (!uchUifACDescCtl) {                         /* Clear Descriptor Case */
            usUifACControl = 0;                         /* Initialize Uif Control Save Area */

            /* Reset All Descriptor W/o ALPHA */
            ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
            ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLRWOALPH;
            MaintReDisp(&ReDisplay);
        }
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Case */
                if (usUifACControl & UIF_VOID) {                            /* VOID Already Put */
                    return(LDT_SEQERR_ADR);
                }
                RptETKUnLock();                                             /* Unlock ETK file */
                MaintFin(CLASS_MAINTTRAILER_PRTSUP);                        /* Execute Exit Procedure */
                UieExit(aACEnter);                                          /* Return to UifACEnter() */
                return(SUCCESS);
            } else {                                                        /* W/ Amount Case */
                /* Check Digit */

                if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT9) {               /* Over Digit */
                    return(LDT_KEYOVER_ADR);
                }
                EmpNoData.uchStatus = 0;                                     /* Initialize Employee Status */ 
                EmpNoData.uchMajorClass = CLASS_PARAEMPLOYEENO;              /* Set Major Class */
                EmpNoData.ulEmployeeNo = (ULONG)pKeyMsg->SEL.INPUT.lData;
                if (usUifACControl & UIF_VOID) {                            /* VOID Key Already Put */
                    if ((sError = MaintEmployeeNoDelete(&EmpNoData)) == SUCCESS) {     
                        UieAccept();                                        /* Return UifAC152Function() */
                    }
                    return(sError);
                } else {
                    EmpNoData.uchMinorClass = CLASS_PARAEMPLOYEENO_INT;      /* Set Minor Class */
                    EmpNoData.uchFieldAddr  = 1;                            /* Field Addres 1 */
                    if ((sError = MaintEmployeeNoRead(&EmpNoData)) == SUCCESS) {
                        UieNextMenu(aChildAC152EnterData1);                 /* Open Next Function */
                    }
                    return(sError);
                }
            }

        case FSC_VOID:
            /* Check W/ Amount */
            if (pKeyMsg->SEL.INPUT.uchLen) {                        /* W/ Amount */
                return(LDT_SEQERR_ADR);
            }
            if (usUifACControl & UIF_NUM_TYPE) {                    /* # TYPE Key Already Put */
                return(LDT_SEQERR_ADR);
            } 
            if (usUifACControl & UIF_VOID) {                        /* VOID Key Already Put */
                return(LDT_SEQERR_ADR);
            } else {
                usUifACControl |= UIF_VOID;                         /* Set VOID Key Put Status */

                /* Set VOID Descriptor */
                ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
                ReDisplay.uchMinorClass = CLASS_MAINTREDISP_VOID;
                MaintReDisp(&ReDisplay);
                UieReject();                                                /* Shift Current Func. to Active Func. */ 
                return(SUCCESS);
            }

        case FSC_NUM_TYPE:
            return(LDT_SEQERR_ADR);
            
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
**  Synopsis: SHORT UifAC152EnterALPHA(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: A/C No.152 Enter Employee Mnemonics Mode 
*===============================================================================
*/
SHORT UifAC152EnterALPHA(KEYMSG *pKeyMsg) 
{
    SHORT       sError;
    PARAEMPLOYEENO  EmpNoData;   

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_P1);                            /* Register Key Sequence */
        UieEchoBack(UIE_ECHO_ROW1_AN, PARA_LEADTHRU_LEN);           /* Define Display Format and Max. Input Data */

        /* Set KB Mode to ALPHA Mode */
        UifACChgKBType(UIF_ALPHA_KB);
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P1:
            EmpNoData.uchStatus = 0;                             /* Set W/ Amount Status */

            if (pKeyMsg->SEL.INPUT.uchLen < 1) {                    /* W/o Amount Input Case */
                return(LDT_SEQERR_ADR);                             /* Set W/o Amount Status */
            } else {                                                /* W/ Amount Input Case */
                memset(EmpNoData.aszMnemonics, '\0', (PARA_LEADTHRU_LEN+1) * sizeof(TCHAR));

                _tcsncpy(EmpNoData.aszMnemonics, pKeyMsg->SEL.INPUT.aszKey, pKeyMsg->SEL.INPUT.uchLen); /* Copy Input Mnemonics to Own Buffer */
            }

            /* Execute Write Employee Mnemonics Procedure */
            EmpNoData.uchMajorClass = CLASS_PARAEMPLOYEENO;             /* Set Major Class */
            EmpNoData.uchMinorClass = CLASS_PARAEMPLOYEENO_2ND;         /* Set Minor Class */

            if ((sError = MaintEmployeeNoEdit(&EmpNoData)) != SUCCESS) {
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


/*====== End of Source ======*/
