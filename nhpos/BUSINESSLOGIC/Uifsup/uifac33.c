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
* Title       : Control String Maintenance Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC33.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC33Function()       : A/C No.33 Function Entry Mode
*               UifAC33EnterData1()     : A/C No.33 Enter Data1 Mode
*               UifAC33EnterData2()     : A/C No.33 Enter Data2 Mode
*               UifAC33Intervation()    : A/C No.33 Intervation Mode
*               UifAC33ErrorCorrect()   : A/C No.33 Error Correct Mode
*               UifAC33EnterALPHA()     : A/C No.33 Enter CSTR Mnemonics Mode
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
*    -  -95:00.00.01    :M.Waki     : initial                                   
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
#include <uifpgequ.h>
#include <uifsup.h>
#include <cvt.h>

#include "uifsupex.h"                       /* Unique Extern Header for UI */


/*
==============================================================================
;                      C O M M O N   R O M   T A B L E s                         
;=============================================================================
*/


/* Define Key Sequence */

UISEQ FARCONST aszSeqAC33EnterData1[] = {FSC_VOID, FSC_AC, FSC_CANCEL, 0};
UISEQ FARCONST aszSeqAC33EnterData2[] = {FSC_NUM_TYPE, FSC_AC, 0};
UISEQ FARCONST aszSeqAC33EnterData3[] = {FSC_AC, 0};

UIMENU FARCONST aParentAC33Func[] = {{UifAC33Function, CID_AC33FUNCTION},
                                     {NULL,            0               }};


/*
*=============================================================================
**  Synopsis: SHORT UifAC33Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.33 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC33Init1[] = {{UifAC33EnterData1, CID_AC33ENTERDATA1},
                                     {UifAC33CancelFunction, CID_AC33CANCELFUNCTION},
                                     {NULL,              0                 }};

/* Define Next Function at UIM_ACCEPTED from CID_AC33ENTERDATA1 */

UIMENU FARCONST aChildAC33Accept1[] = {{UifAC33EnterData2, CID_AC33ENTERDATA2},
                                       {UifAC33CancelFunction, CID_AC33CANCELFUNCTION},
                                       {NULL,              0                 }};

UIMENU FARCONST aChildAC33Accept2[] = {{UifAC33EnterData3, CID_AC33ENTERDATA3},
                                       {UifAC33CancelFunction, CID_AC33CANCELFUNCTION},
                                       {NULL,              0                 }};


SHORT UifAC33Function(KEYMSG *pKeyMsg) 
{

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        usUifACControl &= ~STS_CSTR_VOID;                /* control area VOID bit initial */
        UieNextMenu(aChildAC33Init1);                   /* Open Next Function */
        return(SUCCESS);

    case UIM_REJECT:
        return(SUCCESS);

    case UIM_ACCEPTED:
        switch (pKeyMsg->SEL.usFunc) {
        case CID_AC33ENTERDATA2:
            UieNextMenu(aChildAC33Accept2);                       /* Open Next Function */
            return(SUCCESS);

        default:
            UieNextMenu(aChildAC33Accept1);                     /* Open Next Function */
            return(SUCCESS);
        }

    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC33EnterData1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.33 Enter Data1 Mode 
*===============================================================================
*/
    

SHORT UifAC33EnterData1(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    MAINTCSTR       CSTRData;
    MAINTREDISP     ReDisplay;


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        MaintDisp(AC_CSTR_MAINT,                         /* A/C Number */
                  0,                                    /* Set Minor Class for 10N10D */
                  0,                                    /* Page Number */
                  0,                                    /* PTD Type */
                  0,                                    /* Report Type */
                  0,                                    /* Reset Type */
                  0L,                                   /* Amount Data */
                  LDT_NUM_ADR);                         /* "Number Entry" Lead Through Address */

        UieOpenSequence(aszSeqAC33EnterData1);                              /* Register Key Sequence */
        return(SUCCESS);

    case UIM_CANCEL:
        if ( usUifACControl & STS_CSTR_VOID ){   /* VOID Function CANCEL */
            /* Reset All Descriptor W/o ALPHA */
            ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
            ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLRWOALPH;
            MaintReDisp(&ReDisplay);
            usUifACControl &= ~STS_CSTR_VOID;    /* VOID Function CANCEL */
        }
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT3) {                   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }

            if (!pKeyMsg->SEL.INPUT.uchLen) {       /* W/O Amount case */
                if (!usUifACControl){                /* Report Out */
                    MaintCSTRReport();
                    MaintCstrUnLock();                           /* UnLock CSTR File */
                    MaintFin(CLASS_MAINTTRAILER_PRTSUP);         /* Execute Finalize Procedure */
                    UieExit(aACEnter);
                    return(SUCCESS);
                }
                MaintCstrUnLock();                           /* UnLock CSTR File */
                MaintFin(CLASS_MAINTTRAILER_PRTSUP);            /* Execute Finalize Procedure */
                UieExit(aACEnter);
                return(SUCCESS);
            }
            if ((pKeyMsg->SEL.INPUT.lData < A33_CSTR_MIN)||
                (pKeyMsg->SEL.INPUT.lData > A33_CSTR_MAX)) {
                return(LDT_KEYOVER_ADR);
            }
            usUifACControl |= STS_CSTR_ACCESSED;     /* Already Proccessed */
            if ( usUifACControl & STS_CSTR_VOID ){   /* Delete Function */
                CSTRData.uchMajorClass = CLASS_MAINTCSTR;        /* Set Major Class */
                CSTRData.uchStatus = 0;                         /* Set W/ Amount Status */
                CSTRData.usCstrNumber = ( USHORT)pKeyMsg->SEL.INPUT.lData;
                if ((sError = MaintCSTRDelete(&CSTRData)) == SUCCESS) { /* Delete Function Completed */
                    /* Reset All Descriptor W/o ALPHA */
                    ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
                    ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLRWOALPH;
                    MaintReDisp(&ReDisplay);
                    UieExit(aParentAC33Func);
                }
                return(sError);
            }

            CSTRData.uchMajorClass = CLASS_MAINTCSTR;        /* Set Major Class */
            CSTRData.uchAddr = 1;                           /* set initial address */
            CSTRData.uchStatus = 0;                         /* Set W/ Amount Status */
            CSTRData.usCstrNumber = ( USHORT)pKeyMsg->SEL.INPUT.lData;
            if ((sError = MaintCSTRRead(&CSTRData)) == SUCCESS) {           /* Read Function Completed */
                UieAccept();                                                /* Return to UifAC33Function() */
            }
            return(sError);

        case FSC_VOID:
            /* Check W/ Amount */
            if (pKeyMsg->SEL.INPUT.uchLen) {        /* W/ Amount */
                return(LDT_SEQERR_ADR);
            }
            if(usUifACControl & STS_CSTR_VOID){      /* Already VOID */
                return(LDT_SEQERR_ADR);
            }
            usUifACControl |= STS_CSTR_VOID;         /* Set VOID Status */
            /* Set VOID Descriptor */

            ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
            ReDisplay.uchMinorClass = CLASS_MAINTREDISP_VOID;
            MaintReDisp(&ReDisplay);
/*            UieReject(); */
            return(SUCCESS);
        
        case FSC_CANCEL:
            if (pKeyMsg->SEL.INPUT.uchLen) {                /* W/ Amount */
                return(LDT_SEQERR_ADR);
            }
            MaintCstrUnLock();                           /* UnLock CSTR File */
            if (uchMaintOpeCo){
                if (!(uchMaintOpeCo & MAINT_WRITECOMPLETE)) {
                    MaintMakeAbortKey();                     /* "ABORTED" Print */
                }
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

/*
*=============================================================================
**  Synopsis: SHORT UifAC33EnterData2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.33 Enter Data2 Mode 
*===============================================================================
*/

SHORT UifAC33EnterData2(KEYMSG *pKeyMsg) 
{
    
    SHORT       sError;
    MAINTCSTR   CSTREditData;   

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC33EnterData2);                              /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_NUM_TYPE:                                                  /* Address Key Case */
            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT3) {                   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            CSTREditData.uchStatus = 0;                                      /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Input Case */
                CSTREditData.uchStatus |= MAINT_WITHOUT_DATA;                /* Set W/o Amount Status */
            } else {                                                        /* W/ Amount Input Case */
                if (pKeyMsg->SEL.INPUT.lData > 255L){
                    return(LDT_KEYOVER_ADR);
                }
                CSTREditData.uchAddr = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Edit CSTR Procedure */
            
            CSTREditData.uchMajorClass = CLASS_MAINTCSTR;                     /* Set Major Class */
            CSTREditData.uchMinorClass = CLASS_PARACSTR_MAINT_READ;           /* Set Minor Class */
            if ((sError = MaintCSTREdit(&CSTREditData)) == SUCCESS) {
                UieReject();
                return(SUCCESS);
            }
            return(sError);

        case FSC_AC:                                                        /* Enter Key Case */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Case */
                /* Execute Write Procedure */
                
                if ((sError = MaintCSTRWrite()) == SUCCESS) {                                            
                    UieExit(aParentAC33Func);                               /* Return to UifAC33Function() */
                }
                return(sError);
            } else {                                                        /* W/ Amount Case */
                /* Check Digit */
                if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT3) {               /* Over Digit */
                    return(LDT_KEYOVER_ADR);
                }
                if (pKeyMsg->SEL.INPUT.lData > (LONG)(FSC_MAJOR_MAX + MAINT_FSC_BIAS)){
                    return(LDT_KEYOVER_ADR);
                }
                CSTREditData.uchStatus = 0;                                  /* Initialize Status */ 
                CSTREditData.uchMajorClass = CLASS_MAINTCSTR;                 /* Set Major Class */
                CSTREditData.uchMinorClass = CLASS_PARACSTR_MAINT_FSC_WRITE;      /* Set Minor Class */
                CSTREditData.uchInputData = (UCHAR)pKeyMsg->SEL.INPUT.lData;
                if ((sError = MaintCSTREdit(&CSTREditData)) == SUCCESS) {
                    UieReject();
                    return(SUCCESS);
                } else if (sError == MAINT_FSC_EXIST) {                     /* Extend FSC Exist Case */
                    UieAccept();                                            /* Return to UifAC33Function */
                    return(SUCCESS);
                }
                return(sError);
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
**  Synopsis: SHORT UifAC33EnterData3(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.33 Enter Data3 Mode 
*===============================================================================
*/

SHORT UifAC33EnterData3(KEYMSG *pKeyMsg) 
{
    
    SHORT       sError;
    MAINTCSTR   CSTREditData;   

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC33EnterData3);                              /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:                                                        /* Enter Key Case */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Case */
                return(LDT_SEQERR_ADR);
            } else {                                                        /* W/ Amount Case */
                /* Check Digit */
                if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT3) {               /* Over Digit */
                    return(LDT_KEYOVER_ADR);
                }
                if (pKeyMsg->SEL.INPUT.lData > 255L){
                    return(LDT_KEYOVER_ADR);
                }
                CSTREditData.uchStatus = 0;                                  /* Initialize Status */ 
                CSTREditData.uchMajorClass = CLASS_MAINTCSTR;                 /* Set Major Class */
                CSTREditData.uchMinorClass = CLASS_PARACSTR_MAINT_EXT_WRITE;  /* Set Minor Class */
                CSTREditData.uchInputData = (UCHAR)pKeyMsg->SEL.INPUT.lData;
                if ((sError = MaintCSTREdit(&CSTREditData)) == SUCCESS) {
                    UieAccept();                                /* Return to UifAC33Function */
                    return(SUCCESS);
                }
                return(sError);
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
**  Synopsis: SHORT UifAC33CancelFunction(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No. 33 Exit Function Mode 
*===============================================================================
*/
    

SHORT UifAC33CancelFunction(KEYMSG *pKeyMsg) 
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
            MaintCstrUnLock();                           /* UnLock CSTR File */
            if (uchMaintOpeCo){
                if (!(uchMaintOpeCo & MAINT_WRITECOMPLETE)) {
                    MaintMakeAbortKey();                     /* "ABORTED" Print */
                }
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
    
/**** End of File ****/
