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
* Title       : Server Maintenance/Report Module                         
* Category    : User Interface For Supervisor, NCR 2170 Int'l Hospitality Application         
* Program Name: UIFAC20.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC20Function()      : A/C No.20 Function Entry Mode
*               UifAC20EnterData1()    : A/C No.20 Enter Data1 Mode
*               UifAC20EnterData2()    : A/C No.20 Enter Data2 Mode
*               UifAC20EnterData3()    : A/C No.20 Enter Data3 Mode
*               UifAC20EnterData4()    : A/C No.20 Enter Data4 Mode
*               UifAC20EnterALPHA()    : A/C No.20 Enter Mnemonics Mode
*               UifAC20ErrorCorrect()  : A/C No.20 Error Correct Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Dec-28-92:00.00.01    :K.You      : Initial                                   * Aug-24-98:03.03.00    :H.Iwata    :                              
* Aug-18-98:03.03.00    :H.Iwata    :
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
#include <paraequ.h>
#include <para.h>
#include <cscas.h>
#include <maint.h>
#include <uifsup.h>
#include <uireg.h>
#include <uifpgequ.h>
#include <cvt.h>

#include <mldmenu.h>

#include "uifsupex.h"                       /* Unique Extern Header for UI */


/*
==============================================================================
;                      C O M M O N   R O M   T A B L E s                         
;=============================================================================
*/

/* Define Previous Function and Key Sequence */

UIMENU FARCONST aParentAC20Func[] = {{UifAC20Function, CID_AC20FUNCTION},
                                     {NULL,            0               }};
    
UISEQ FARCONST aszSeqAC20EnterData1[] = {FSC_VOID, FSC_NUM_TYPE, FSC_AC, 0};
UISEQ FARCONST aszSeqAC20EnterData2[] = {FSC_NUM_TYPE, FSC_AC, 0};

/*
*=============================================================================
**  Synopsis: SHORT UifAC20Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.20 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC20Init1[] = {{UifAC20EnterData1, CID_AC20ENTERDATA1},
                                     {UifAC20CancelFunction, CID_AC20CANCELFUNCTION},
                                     {NULL,               0                  }};

/* Define Next Function at UIM_INIT from CID_AC20ERRORCORRECT, CID_AC20ENTERDATA2 */

UIMENU FARCONST aChildAC20Init2[] = {{UifAC20EnterData3, CID_AC20ENTERDATA3},
                                     {UifAC20CancelFunction, CID_AC20CANCELFUNCTION},
                                     {NULL,               0                  }};

/* Define Next Function at UIM_ACCEPTED */

UIMENU FARCONST aChildAC20Accept[] = {{UifAC20ErrorCorrect, CID_AC20ERRORCORRECT},
                                      {UifAC20EnterData2, CID_AC20ENTERDATA2},
                                      {UifAC20CancelFunction, CID_AC20CANCELFUNCTION},
                                      {NULL,               0                  }};



SHORT UifAC20Function(KEYMSG *pKeyMsg) 
{

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:

        UifACPGMLDClear();

        switch (pKeyMsg->SEL.usFunc) {
        case CID_AC20ERRORCORRECT:
        case CID_AC20ENTERDATA1:
        case CID_AC20ENTERDATA2:
        case CID_AC20ENTERDATA3:
        case CID_AC20ENTERDATA4:
        case CID_AC20GCSNUMBER:
        case CID_AC20GCENUMBER:
        case CID_AC20CHARGETIP:
        case CID_AC20ENTERTEAMNO:
        case CID_AC20ENTERTERMINAL:
            MaintDisp(AC_CASH_ASSIN,                    /* A/C Number */
                      0,                                /* Descriptor */
                      0,                                /* Page Number */
                      0,                                /* PTD Type */
                      0,                                /* Report Type */
                      0,                                /* Reset Type */
                      0L,                               /* Amount Data */
                      LDT_NUM_ADR);                     /* "Enter Number" Lead Through Address */
                                                      
            UieNextMenu(aChildAC20Init2);              /* Open Next Function */
            return(SUCCESS);

        default:

            MaintDisp(AC_CASH_ASSIN,                    /* A/C Number */
                      0,                                /* Descriptor */
                      0,                                /* Page Number */
                      0,                                /* PTD Type */
                      0,                                /* Report Type */
                      0,                                /* Reset Type */
                      0L,                               /* Amount Data */
                      LDT_NUM_ADR);                     /* "Enter Number" Lead Through Address */

            UieNextMenu(aChildAC20Init1);              /* Open Next Function */
            return(SUCCESS);
        }

    case UIM_ACCEPTED:                                          /* Accepted */
        UieNextMenu(aChildAC20Accept);                         /* Open Next Function */
        return(SUCCESS);

    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC20EnterData1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.20 Enter Data1 Mode 
*===============================================================================
*/
SHORT UifAC20EnterData1(KEYMSG *pKeyMsg) 
{
    SHORT               sError;
    MAINTCASHIERIF      CastNoData = {0};   
    MAINTREDISP         ReDisplay = {0};
                       
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC20EnterData1);              /* Register Key Sequence */
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
                MaintCashierReport();                                    /* Execute Report Procedure */
                UieExit(aACEnter);                                          /* Return to UifACEnter() */
                return(SUCCESS);
            }

            if (pKeyMsg->SEL.INPUT.uchLen > UIFREG_MAX_ID_DIGITS) {            /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }

            CastNoData.uchStatus = 0;                                       /* Set W/ Amount */
            CastNoData.uchMajorClass = CLASS_PARACASHIERNO;                  /* Set Major Class */
            CastNoData.CashierIf.ulCashierNo = (ULONG)pKeyMsg->SEL.INPUT.lData;
            if (usUifACControl & UIF_VOID) {                                /* VOID Key Already Put */
                if ((sError = MaintCashierDelete(&CastNoData)) == SUCCESS) {     
                    UieExit(aParentAC20Func);                               /* Return to UifAC20Function() */
                    return(SUCCESS);
                }
                return(sError);
            } else {                                                        /* W/ Amount Case */
                /* Check Digit */
                if ((sError = MaintCashierRead(&CastNoData)) == SUCCESS) { 
                    UieAccept();                                           
                }
                return(sError);
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
**  Synopsis: SHORT UifAC20EnterData2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.20 Enter Data2 Mode 
*===============================================================================
*/
    
/* Define Next/Previous Function and Key Sequence */

UIMENU FARCONST aChildAC20EnterData21[] = {{UifACShift, CID_ACSHIFT},
                                           {UifAC20EnterALPHA, CID_AC20ENTERALPHA},
                                           {UifACChangeKB1, CID_ACCHANGEKB1},
                                           {NULL,           0              }};

UIMENU FARCONST aChildAC20EnterData22[] = {{UifAC20EnterData4, CID_AC20ENTERDATA4},
                                           {NULL,           0              }};

SHORT UifAC20EnterData2(KEYMSG *pKeyMsg) 
{
    SHORT               sError;
    MAINTCASHIERIF      CastEditData = {0};   

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC20EnterData2);                  /* Register Key Sequence */
        return(SUCCESS);
                                                                            
    case UIM_ACCEPTED:                                          /* Accepted from UifAC20EnterALPHA() */
        switch (pKeyMsg->SEL.usFunc) {
        case CID_ACCHANGEKB1:
            /* Display after Mnemonics Setting Address ( Address 10) */

            CastEditData.uchMajorClass = CLASS_PARACASHIERNO;                   /* Set Major Class */
            CastEditData.uchMinorClass = CLASS_PARACASHIERNO_READ;              /* Set Minor Class */
            CastEditData.uchFieldAddr = CAS_FIELD1_ADR;
            /* CastEditData.uchFieldAddr = CAS_FIELD10_ADR; */
            CastEditData.uchStatus = 0;                                 /* Set W/ Amount Status */
            MaintCashierEdit(&CastEditData);
            /* break */

        default:
            UieAccept();                                                /* Return to UifACXXXFunction() */
            return(SUCCESS);
        }
                                                           
    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_NUM_TYPE:                                      /* Address Key Case */
            /* Check Digit */

            if ( (SHORT)pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {       /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }

            CastEditData.uchStatus = 0;                         /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                   /* W/o Amount Input Case */
                CastEditData.uchStatus |= MAINT_WITHOUT_DATA;   /* Set W/o Amount Status */
            } else {                                            /* W/ Amount Input Case */
                CastEditData.uchFieldAddr = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            if (pKeyMsg->SEL.INPUT.lData == 0){
                if(pKeyMsg->SEL.INPUT.uchLen >= UIF_DIGIT1){
                    return(LDT_KEYOVER_ADR);
                }
            }else if ( (pKeyMsg->SEL.INPUT.lData < CAS_FIELD1_ADR) || (pKeyMsg->SEL.INPUT.lData > CAS_FIELD_MAX) ) {
                return(LDT_KEYOVER_ADR);
            }

            /* Execute Edit Cashier Procedure */
            CastEditData.uchMajorClass = CLASS_PARACASHIERNO;                    /* Set Major Class */
            CastEditData.uchMinorClass = CLASS_PARACASHIERNO_READ;               /* Set Minor Class */

            sError = MaintCashierEdit(&CastEditData);
            if (sError == SUCCESS) {     /* Edit Function Completed */
                UieReject();                                                /* Shift Current Func. to Active Func. */
            } else if (sError == MAINT_CAS_MNEMO_ADR) {                     /* Mnemonics Setting Case */
                UieNextMenu(aChildAC20EnterData21);                         /* Open Next Function */                                        
                return(SUCCESS);                                            
            } else if (sError == MAINT_CAS_STSGC_ADR) {                 
                UieNextMenu(aChildAC20EnterData22);                         /* Open Next Function */                                        
                return(SUCCESS);
            }
            return(sError);
      
        case FSC_AC:                                                        /* Enter Key Case */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Case */
                /* Execute Write Procedure */
                if ((sError = MaintCashierWrite()) == SUCCESS) {
                    UieExit(aParentAC20Func);                               /* Return to UifAC20Function() */
                }
                return(sError);
            } else {                                                        /* W/ Amount Case */
                /* Check Digit */
                if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT4) {               /* Over Digit */
                    return(LDT_KEYOVER_ADR);
                }
                CastEditData.uchStatus = 0;                                 /* Initialize Cashier Status */ 
                CastEditData.uchMajorClass = CLASS_PARACASHIERNO;               /* Set Major Class */
                CastEditData.uchMinorClass = CLASS_PARACASHIERNO_WRITE;         /* Set Minor Class */
                CastEditData.CashierIf.usGstCheckStartNo = ( USHORT)pKeyMsg->SEL.INPUT.lData;

				/* Copy Input Mnemonics to Own Buffer */
                _tcsncpy(CastEditData.aszMnemonics, pKeyMsg->SEL.INPUT.aszKey, pKeyMsg->SEL.INPUT.uchLen);

                sError = MaintCashierEdit(&CastEditData);
                if (sError == SUCCESS) {   /* Edit Function Completed */
                    UieReject();                                            /* Shift Current Func. to Active Func. */
                } else if (sError == MAINT_CAS_MNEMO_ADR) {
                    UieNextMenu(aChildAC20EnterData21);                     /* Open Next Function */                                        
                    return(SUCCESS);
                } else if (sError == MAINT_CAS_STSGC_ADR) {
                    UieNextMenu(aChildAC20EnterData22);                     /* Open Next Function */                                        
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
**  Synopsis: SHORT UifAC20EnterData3(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.20 Enter Data3 Mode 
*===============================================================================
*/
SHORT UifAC20EnterData3(KEYMSG *pKeyMsg) 
{
    SHORT         	sError;
    MAINTCASHIERIF  CastNoData = {0};   
    MAINTREDISP     ReDisplay = {0};


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC20EnterData1);              /* Register Key Sequence */
        /* return(SUCCESS); */

    case UIM_CANCEL:
        usUifACControl = 0;                                 /* Initialize Uif Control Save Area */
                                                            
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
                UieExit(aACEnter);                                          /* Return to UifACEnter() */
                return(SUCCESS);
            } 
                                                        
            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIFREG_MAX_ID_DIGITS) {           /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            CastNoData.uchStatus = 0;                                  /* Set W/ Amount */
            CastNoData.uchMajorClass = CLASS_PARACASHIERNO;             /* Set Major Class */
            CastNoData.CashierIf.ulCashierNo = (ULONG)pKeyMsg->SEL.INPUT.lData;
            if (usUifACControl & UIF_VOID) {                                /* VOID Key Already Put */
                if ((sError = MaintCashierDelete(&CastNoData)) == SUCCESS) {     
                    UieExit(aParentAC20Func);                               /* Return to UifAC20Function() */
                    return(SUCCESS);
                }
                return(sError);
            } else {                                                        /* W/ Amount Case */
                /* Check Digit */
                if ((sError = MaintCashierRead(&CastNoData)) == SUCCESS) { 
                    UieAccept();                                           /* Return to UifACXXXFunction() */
                }
                return(sError);
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
**  Synopsis: SHORT UifAC20EnterData4(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.20 Enter Data4 Mode 
*===============================================================================
*/
SHORT UifAC20EnterData4(KEYMSG *pKeyMsg) 
{
    SHORT        	sError;
	MAINTCASHIERIF  CastEditData = {0};   
                    
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC20EnterData2);              /* Register Key Sequence */
        return(SUCCESS);                                         
                                                                            
    case UIM_ACCEPTED:                                          
        UieAccept();                                            
        return(SUCCESS);
                                                           
    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_NUM_TYPE:                                  /* Address Key Case */
            return(LDT_SEQERR_ADR);

        case FSC_AC:                                        /* Enter Key Case */
            if (!pKeyMsg->SEL.INPUT.uchLen) {               /* W/o Amount Case */
                return(LDT_SEQERR_ADR);
            }
 
            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT4) {           /* Over Digit */
                return(LDT_KEYOVER_ADR);                            
            }

            CastEditData.uchStatus = 0;                             /* Initialize Cashier Status */ 
            CastEditData.uchMajorClass = CLASS_PARACASHIERNO;                /* Set Major Class */
            CastEditData.uchMinorClass = CLASS_PARACASHIERNO_WRITE;          /* Set Minor Class */
            CastEditData.CashierIf.usGstCheckEndNo = ( USHORT)pKeyMsg->SEL.INPUT.lData;
            if ((sError = MaintCashierEdit(&CastEditData)) == SUCCESS) {   /* Edit Function Completed */
                UieAccept();                                                /* Return to UifAC20EnterData2() */
            }
            return(sError);

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
**  Synopsis: SHORT UifAC20EnterALPHA(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: A/C No.20 Enter Mnimonics Mode 
*===============================================================================*/
SHORT UifAC20EnterALPHA(KEYMSG *pKeyMsg) 
{
    SHORT         	sError;
    MAINTCASHIERIF  CastEditData = {0};   

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_P1);                            /* Register Key Sequence */
        UieEchoBack(UIE_ECHO_ROW1_AN, PARA_CASHIER_LEN);            /* Define Display Format and Max. Input Data */

        /* Set KB Mode to ALPHA Mode */
        UifACChgKBType(UIF_ALPHA_KB);
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P1:
            CastEditData.uchStatus = 0;                             /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount Input Case */
                CastEditData.uchStatus |= MAINT_WITHOUT_DATA;       /* Set W/o Amount Status */
            } else {                                                /* W/ Amount Input Case */
                /* Full NULL to Own Buffer */
                /* Copy Input Mnemonics to Own Buffer */
                _tcsncpy(CastEditData.CashierIf.auchCashierName, pKeyMsg->SEL.INPUT.aszKey, pKeyMsg->SEL.INPUT.uchLen);
            }

            /* Execute Write Mnemonics Procedure */
            CastEditData.uchMajorClass = CLASS_PARACASHIERNO;           /* Set Major Class */
            CastEditData.uchMinorClass = CLASS_PARACASHIERNO_WRITE;     /* Set Minor Class */

            if ((sError = MaintCashierEdit(&CastEditData)) != SUCCESS) {
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
**  Synopsis: SHORT UifAC20ErrorCorrect(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Key Sequence Error
*               
**  Description: A/C No.20 Error Correct Mode 
*===============================================================================
*/
SHORT UifAC20ErrorCorrect(KEYMSG *pKeyMsg) 
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
                UieExit(aParentAC20Func);                           /* Return to UifAC20Function() */
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
**  Synopsis: SHORT UifAC20CancelFunction(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No. 20 Cancel Function Mode 
*===============================================================================
*/
SHORT UifAC20CancelFunction(KEYMSG *pKeyMsg) 
{
    
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_CANCEL);                /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_CANCEL:
            if (pKeyMsg->SEL.INPUT.uchLen) {                /* W/ Amount */
                return(LDT_SEQERR_ADR);
            }
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
