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
* Title       : Department Maintenance/Report Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC114.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC114Function()      : A/C No.114 Function Entry Mode
*               UifAC114EnterData1()    : A/C No.114 Enter Data1 Mode
*               UifAC114EnterData2()    : A/C No.114 Enter Data2 Mode
*               UifAC114EnterData3()    : A/C No.114 Enter Data3 Mode
*               UifAC114EnterALPHA()    : A/C No.114 Enter DEPT Mnemonics Mode
*               UifAC114ErrorCorrect()  : A/C No.114 Error Correct Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
*    Date  : Ver.Rev.   :   Name    : Description
* Jun-16-92:00.00.01    :K.You      : initial                                   
* Apr-14-95:03.00.00    :T.Satoh    : Adds clear MLD
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
#include <pif.h>
#include <fsc.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <uifsup.h>
#include <uifpgequ.h>
#include <cvt.h>
#include <mldmenu.h>

#include "uireg.h"
#include "uifsupex.h"                       /* Unique Extern Header for UI */

/*
*=============================================================================
**  Synopsis: SHORT UifAC114Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.114 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC114Init1[] = {{UifAC114EnterData1, CID_AC114ENTERDATA1},
                                      {UifAC114CancelFunction, CID_AC114CANCELFUNCTION},
                                      {NULL,               0                  }};

/* Define Next Function at UIM_INIT from CID_AC114ERRORCORRECT, CID_AC114ENTERDATA2 */

UIMENU FARCONST aChildAC114Init2[] = {{UifAC114EnterData3, CID_AC114ENTERDATA3},
                                      {UifAC114CancelFunction, CID_AC114CANCELFUNCTION},
                                      {NULL,               0                  }};

/* Define Next Function at UIM_ACCEPTED */

UIMENU FARCONST aChildAC114Accept[] = {{UifAC114ErrorCorrect, CID_AC114ERRORCORRECT},
                                       {UifAC114EnterData2, CID_AC114ENTERDATA2},
                                       {UifAC114CancelFunction, CID_AC114CANCELFUNCTION},
                                       {NULL,               0                  }};

UIMENU FARCONST aParentAC114Func[] = {{UifAC114Function, CID_AC114FUNCTION},
                                      {NULL,             0              }};


SHORT UifAC114Function(KEYMSG *pKeyMsg) 
{



    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:

        UifACPGMLDClear();                              /* Clear MLD */

        switch (pKeyMsg->SEL.usFunc) {
        case CID_AC114ERRORCORRECT:
        case CID_AC114ENTERDATA2:
        case CID_AC114ENTERDATA1:
        case CID_AC114ENTERDATA3:
            MaintDisp(AC_DEPT_MAINT,                    /* A/C Number */
                  CLASS_MAINTDSPCTL_10NPG,              /* Set Minor Class for 10N10D */
                      0,                                /* Page Number */
                      0,                                /* PTD Type */
                      0,                                /* Report Type */
                      0,                                /* Reset Type */
                      0L,                               /* Amount Data */
                      LDT_DEPTNO_ADR);                  /* "Request Department # Entry" Lead Through Address */
                                                      
            UieNextMenu(aChildAC114Init2);              /* Open Next Function */
            return(SUCCESS);

        default:

            MaintDisp(AC_DEPT_MAINT,                    /* A/C Number */
                  CLASS_MAINTDSPCTL_10NPG,              /* Set Minor Class for 10N10D */
                      0,                                /* Page Number */
                      0,                                /* PTD Type */
                      0,                                /* Report Type */
                      0,                                /* Reset Type */
                      0L,                               /* Amount Data */
                      LDT_DEPTNO_ADR);                  /* "Request Department # Entry" Lead Through Address */

            UieNextMenu(aChildAC114Init1);              /* Open Next Function */
            return(SUCCESS);
        }

    case UIM_ACCEPTED:                                          /* Accepted */
        UieNextMenu(aChildAC114Accept);                         /* Open Next Function */
        return(SUCCESS);

    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC114EnterData1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.114 Enter Data1 Mode 
*===============================================================================
*/
    
/* Define Key Sequence */

UISEQ FARCONST aszSeqAC114EnterData1[] = {FSC_AC, FSC_KEYED_DEPT, FSC_VOID, 0};


SHORT UifAC114EnterData1(KEYMSG *pKeyMsg) 
{
    
    SHORT       sError;
    PARADEPT    DEPTNoData;   
    USHORT      usDEPTNumber;
    MAINTREDISP ReDisplay;


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC114EnterData1);                              /* Register Key Sequence */
        /* return(SUCCESS); */

    case UIM_CANCEL:
        usUifACControl = 0;                                     /* Initialize Uif Control Save Area */

        /* Reset All Descriptor W/o ALPHA */

        ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
        ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLRWOALPH;
        MaintReDisp(&ReDisplay);
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Case */
                if (usUifACControl & UIF_VOID) {                            /* VOID Key Already Put */
                    return(LDT_SEQERR_ADR);
                }
                MaintDEPTReport();                                          /* Execute Report Procedure */
                MaintPLUDEPTUnLock();                                       /* UnLock PLU File */
                UieExit(aACEnter);                                          /* Return to UifACEnter() */
                return(SUCCESS);
            } else {                                                        /* W/ Amount Case */
                /* Check Digit */

                if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT4) {               /* Over Digit */
                    return(LDT_KEYOVER_ADR);
                }

                DEPTNoData.uchStatus = 0;                                   /* Set W/ Amount */
                DEPTNoData.uchMajorClass = CLASS_PARADEPT;                  /* Set Major Class */
                DEPTNoData.usDEPTNumber = ( USHORT)pKeyMsg->SEL.INPUT.lData;
                DEPTNoData.usACNumber = AC_DEPT_MAINT;                      /* Set DEPT MAINT A/C Number */
                if (usUifACControl & UIF_VOID) {                                /* VOID Key Already Put */
                    if ((sError = MaintDEPTDelete(&DEPTNoData)) == SUCCESS) {     
                        UieExit(aParentAC114Func);                                  /* Return to UifAC114Function() */
                        return(SUCCESS);
                    }
                    return(sError);
                } else {                                                        /* W/ Amount Case */
                    if ((sError = MaintDEPTRead(&DEPTNoData)) == SUCCESS) { 
                        UieAccept();                                                /* Return to UifACXXXFunction() */
                    }
                    return(sError);
                }
            }
            break;
      
        case FSC_KEYED_DEPT:
            /* Check W/O Amount */

            if (pKeyMsg->SEL.INPUT.uchLen) {                                /* W/ Amount */
                return(LDT_KEYOVER_ADR);
            }

            /* Get PLU Number */

            if ((sError = MaintGetDEPTNo(pKeyMsg->SEL.INPUT.uchMinor, &usDEPTNumber)) != SUCCESS) {
                return(sError);
            }

            DEPTNoData.uchStatus = 0;                                       /* Set W/ Amount */
            DEPTNoData.uchMajorClass = CLASS_PARADEPT;                      /* Set Major Class */
            DEPTNoData.usACNumber = AC_DEPT_MAINT;                          /* Set DEPT MAINT A/C Number */
            DEPTNoData.usDEPTNumber = usDEPTNumber;         /* Set DEPT No. */
            if (usUifACControl & UIF_VOID) {                                /* VOID Key Already Put */
                if ((sError = MaintDEPTDelete(&DEPTNoData)) == SUCCESS) {     
                    UieExit(aParentAC114Func);                                  /* Return to UifAC114Function() */
                    return(SUCCESS);
                }
                return(sError);
            } else {                                                        /* W/ Amount Case */
                if ((sError = MaintDEPTRead(&DEPTNoData)) == SUCCESS) {     
                    UieAccept();                                                /* Return to UifACXXXFunction() */
                }
            }
            return(sError);
        
        case FSC_VOID: /* 2172 */
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
        return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC114EnterData2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.114 Enter Data2 Mode 
*===============================================================================
*/
    
/* Define Next/Previous Function and Key Sequence */

UIMENU FARCONST aChildAC114EnterData2[] = {{UifACShift, CID_ACSHIFT},
                                           {UifAC114EnterALPHA, CID_AC114ENTERALPHA},
                                           {UifACChangeKB1, CID_ACCHANGEKB1},
                                           {NULL,           0              }};
/*
UIMENU FARCONST aParentAC114Func[] = {{UifAC114Function, CID_AC114FUNCTION},
                                      {NULL,             0              }};
*/

UISEQ FARCONST aszSeqAC114EnterData2[] = {FSC_NUM_TYPE, FSC_AC, FSC_STRING, FSC_KEYED_STRING, 0};


SHORT UifAC114EnterData2(KEYMSG *pKeyMsg) 
{
    
    SHORT       sError;
    MAINTDEPT   DEPTEditData;   
                    


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC114EnterData2);                 /* Register Key Sequence */
        return(SUCCESS);
                                                                            
    case UIM_ACCEPTED:                                          /* Accepted from UifAC114EnterALPHA() */
        switch (pKeyMsg->SEL.usFunc) {
        case CID_ACCHANGEKB1:
            /* Display after Mnemonics Setting Address ( Address 12:Print Priority) */

            DEPTEditData.uchMajorClass = CLASS_MAINTDEPT;                   /* Set Major Class */
            DEPTEditData.uchMinorClass = CLASS_MAINTDEPT_READ;              /* Set Minor Class */
            DEPTEditData.uchFieldAddr = DPT_FIELD12_ADR;
            DEPTEditData.usACNumber = AC_DEPT_MAINT;                        /* Set DEPT MAINT A/C Number */
            DEPTEditData.uchStatus = 0;                                     /* Set W/ Amount Status */
            MaintDEPTEdit(&DEPTEditData);
            /* break; */

        default:
            UieAccept();                                                /* Return to UifACXXXFunction() */
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

        case FSC_NUM_TYPE:                                      /* Address Key Case */
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {       /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }                                                  
            DEPTEditData.uchStatus = 0;                         /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                   /* W/o Amount Input Case */
                DEPTEditData.uchStatus |= MAINT_WITHOUT_DATA;   /* Set W/o Amount Status */
            } else {                                            /* W/ Amount Input Case */
                DEPTEditData.uchFieldAddr = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Edit DEPT Procedure */
            
            DEPTEditData.uchMajorClass = CLASS_MAINTDEPT;                   /* Set Major Class */
            DEPTEditData.uchMinorClass = CLASS_MAINTDEPT_READ;              /* Set Minor Class */
            DEPTEditData.usACNumber = AC_DEPT_MAINT;                          /* Set DEPT MAINT A/C Number */
            if ((sError = MaintDEPTEdit(&DEPTEditData)) == SUCCESS) {       /* Edit Function Completed */
                UieReject();                                                /* Shift Current Func. to Active Func. */
                return(sError);
            } else if (sError == MAINT_DEPT_MNEMO_ADR) {                    /* Mnemonics Setting Case */
                UieNextMenu(aChildAC114EnterData2);                         /* Open Next Function */                                        
                return(SUCCESS);
            }
            return(sError);
      
        case FSC_AC:                                                        /* Enter Key Case */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Case */

                /* Execute Write Procedure */
                
                if ((sError = MaintDEPTWrite()) == SUCCESS) {                                            
                    UieExit(aParentAC114Func);                                  /* Return to UifAC114Function() */
                }
                return(sError);
            } else {                                                        /* W/ Amount Case */
                /* Check Digit */

                if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT4) {               /* Over Digit */
                    return(LDT_KEYOVER_ADR);
                }
                DEPTEditData.uchStatus = 0;                                 /* Initialize Cashier Status */ 
                DEPTEditData.uchMajorClass = CLASS_MAINTDEPT;               /* Set Major Class */
                DEPTEditData.uchMinorClass = CLASS_MAINTDEPT_WRITE;         /* Set Minor Class */
                DEPTEditData.usACNumber = AC_DEPT_MAINT;                    /* Set DEPT MAINT A/C Number */
                DEPTEditData.ulInputData = pKeyMsg->SEL.INPUT.lData;
                memset(DEPTEditData.aszMnemonics,                           /* Full NULL to Own Buffer */
                       '\0',
                       sizeof(DEPTEditData.aszMnemonics));

                _tcsncpy(DEPTEditData.aszMnemonics,                           /* Copy Input Mnemonics to Own Buffer */
                       pKeyMsg->SEL.INPUT.aszKey,
                       pKeyMsg->SEL.INPUT.uchLen);
                //memcpy(DEPTEditData.aszMnemonics,                           /* Copy Input Mnemonics to Own Buffer */
                //       pKeyMsg->SEL.INPUT.aszKey,
                //       pKeyMsg->SEL.INPUT.uchLen);
                if ((sError = MaintDEPTEdit(&DEPTEditData)) == SUCCESS) {   /* Edit Function Completed */
                    UieReject();                                            /* Shift Current Func. to Active Func. */
                    return(sError);
                } else if (sError == MAINT_DEPT_MNEMO_ADR) {
                    UieNextMenu(aChildAC114EnterData2);                     /* Open Next Function */                                        
                    return(SUCCESS);
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
**  Synopsis: SHORT UifAC114EnterData3(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.114 Enter Data3 Mode 
*===============================================================================
*/
    

SHORT UifAC114EnterData3(KEYMSG *pKeyMsg) 
{
    
    SHORT       sError;
    PARADEPT    DEPTNoData;   
    USHORT      usDEPTNumber;
    MAINTREDISP ReDisplay;


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC114EnterData1);                              /* Register Key Sequence */
        /* return(SUCCESS); */

    case UIM_CANCEL:
        usUifACControl = 0;                                     /* Initialize Uif Control Save Area */

        /* Reset All Descriptor W/o ALPHA */

        ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
        ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLRWOALPH;
        MaintReDisp(&ReDisplay);
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Case */
                if (usUifACControl & UIF_VOID) {                            /* VOID Key Already Put */
                    return(LDT_SEQERR_ADR);
                }
                MaintFin(CLASS_MAINTTRAILER_PRTSUP);
                MaintPLUDEPTUnLock();                                       /* UnLock PLU File */
                UieExit(aACEnter);                                          /* Return to UifACEnter() */
                return(SUCCESS);
            } else {                                                        /* W/ Amount Case */
                /* Check Digit */

                if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT4) {                       /* Over Digit */
                    return(LDT_KEYOVER_ADR);
                }
                DEPTNoData.uchStatus = 0;                                   /* Set W/ Amount */
                DEPTNoData.uchMajorClass = CLASS_PARADEPT;                  /* Set Major Class */
                DEPTNoData.usACNumber = AC_DEPT_MAINT;                      /* Set DEPT MAINT A/C Number */
                DEPTNoData.usDEPTNumber = ( USHORT)pKeyMsg->SEL.INPUT.lData;
                if (usUifACControl & UIF_VOID) {                                /* VOID Key Already Put */
                    if ((sError = MaintDEPTDelete(&DEPTNoData)) == SUCCESS) {     
                        UieExit(aParentAC114Func);                                  /* Return to UifAC114Function() */
                        return(SUCCESS);
                    }
                    return(sError);
                } else {                                                        /* W/ Amount Case */
                    if ((sError = MaintDEPTRead(&DEPTNoData)) == SUCCESS) {     /* Read Function Completed */
                        UieAccept();                                                /* Return to UifACXXXFunction() */
                    }
                }
                return(sError);
            }
            break;
            
        case FSC_KEYED_DEPT:
            /* Check W/O Amount */

            if (pKeyMsg->SEL.INPUT.uchLen) {                                /* W/ Amount */
                return(LDT_SEQERR_ADR);
            }
            /* Get PLU Number */

            if ((sError = MaintGetDEPTNo(pKeyMsg->SEL.INPUT.uchMinor, &usDEPTNumber)) != SUCCESS) {
                return(sError);
            }

            DEPTNoData.uchStatus = 0;                                       /* Set W/ Amount */
            DEPTNoData.uchMajorClass = CLASS_PARADEPT;                      /* Set Major Class */
            DEPTNoData.usACNumber = AC_DEPT_MAINT;                          /* Set DEPT MAINT A/C Number */
            DEPTNoData.usDEPTNumber = usDEPTNumber;         /* Set DEPT No. */
            if (usUifACControl & UIF_VOID) {                                /* VOID Key Already Put */
                if ((sError = MaintDEPTDelete(&DEPTNoData)) == SUCCESS) {     
                    UieExit(aParentAC114Func);                                  /* Return to UifAC114Function() */
                    return(SUCCESS);
                }
                return(sError);
            } else {                                                        /* W/ Amount Case */
                if ((sError = MaintDEPTRead(&DEPTNoData)) == SUCCESS) {         /* Read Function Completed */
                    UieAccept();                                                /* Return to UifACXXXFunction() */
                }
            }
            return(sError);
        
        case FSC_VOID: /* 2172 */
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
        return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC114EnterALPHA(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: A/C No.114 Enter DEPT Mnemonics Mode 
*===============================================================================
*/
    


SHORT UifAC114EnterALPHA(KEYMSG *pKeyMsg) 
{
    
    SHORT       sError;
    MAINTDEPT   DEPTEditData;   


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_P1);                            /* Register Key Sequence */
        UieEchoBack(UIE_ECHO_ROW1_AN, PARA_DEPT_LEN);               /* Define Display Format and Max. Input Data */

        /* Set KB Mode to ALPHA Mode */

        UifACChgKBType(UIF_ALPHA_KB);
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P1:
            DEPTEditData.uchStatus = 0;                             /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount Input Case */
                DEPTEditData.uchStatus |= MAINT_WITHOUT_DATA;       /* Set W/o Amount Status */
            } else {                                                /* W/ Amount Input Case */
                memset(DEPTEditData.aszMnemonics,                   /* Full NULL to Own Buffer */
                       '\0',
                       (PARA_DEPT_LEN+1) * sizeof(TCHAR));

                _tcsncpy(DEPTEditData.aszMnemonics,                   /* Copy Input Mnemonics to Own Buffer */
                       pKeyMsg->SEL.INPUT.aszKey,
                       pKeyMsg->SEL.INPUT.uchLen);
                //memcpy(DEPTEditData.aszMnemonics,                   /* Copy Input Mnemonics to Own Buffer */
                //       pKeyMsg->SEL.INPUT.aszKey,
                //       pKeyMsg->SEL.INPUT.uchLen);
            }

            /* Execute Write DEPT Mnemonics Procedure */
            
            DEPTEditData.uchMajorClass = CLASS_MAINTDEPT;           /* Set Major Class */
            DEPTEditData.uchMinorClass = CLASS_MAINTDEPT_WRITE;     /* Set Minor Class */

            if ((sError = MaintDEPTEdit(&DEPTEditData)) != SUCCESS) {
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


/*
*=============================================================================
**  Synopsis: SHORT UifAC114ErrorCorrect(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Key Sequence Error
*               
**  Description: A/C No.114 Error Correct Mode 
*===============================================================================
*/
    


SHORT UifAC114ErrorCorrect(KEYMSG *pKeyMsg) 
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
                UieExit(aParentAC114Func);                          /* Return to UifAC114Function() */
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
**  Synopsis: SHORT UifAC114CancelFunction(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No. 114 Cancel Function Mode 
*===============================================================================
*/
    

SHORT UifAC114CancelFunction(KEYMSG *pKeyMsg) 
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
            MaintPLUDEPTUnLock();                           /* UnLock PLU File */
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
    
