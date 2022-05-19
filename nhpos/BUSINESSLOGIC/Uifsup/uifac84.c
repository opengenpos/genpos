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
* Title       : Rounding Table Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC84.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC84Function()       : A/C No.84 Function Entry Mode
*               UifAC84EnterAddr()      : A/C No.84 Enter Address Mode
*               UifAC84EnterDelimit1()  : A/C No.84 Enter Delimitor,Modules 1 Mode
*               UifAC84EnterPosition()  : A/C No.84 Enter Position Mode
*               UifAC84EnterDelimit2()  : A/C No.84 Enter Delimitor,Modules 2 Mode
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jul-10-92:00.00.01    :K.You      : initial                                   
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
#include <fsc.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <uifsup.h>
#include <uifpgequ.h>

#include "uifsupex.h"                       /* Unique Extern Header for UI */




/*
*=============================================================================
**  Synopsis: SHORT UifAC84Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.84 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC84Init1[] = {{UifAC84EnterAddr, CID_AC84ENTERADDR},
                                     {UifAC84EnterDelimit1, CID_AC84ENTERDELIMIT1},
                                     {NULL,                 0                    }};

/* Define Next Function at UIM_INIT from CID_AC84ENTERPOSITION */

UIMENU FARCONST aChildAC84Init2[] = {{UifAC84EnterAddr, CID_AC84ENTERADDR},
                                     {UifAC84EnterDelimit2, CID_AC84ENTERDELIMIT2},
                                     {NULL,                 0                    }};


/* Define Next Function at UIM_ACCEPTED */

UIMENU FARCONST aChildAC84Accept[] = {{UifAC84EnterAddr, CID_AC84ENTERADDR},
                                      {UifAC84EnterPosition, CID_AC84ENTERPOSITION},
                                      {NULL,                 0                    }};


SHORT UifAC84Function(KEYMSG *pKeyMsg) 
{

    PARAROUNDTBL    ParaRoundTbl;   
    MAINTREDISP     ReDisplay;
                         

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        switch (pKeyMsg->SEL.usFunc) {
        case CID_AC84ENTERPOSITION:
        case CID_AC84ENTERADDR:
            /* Reset All Descriptor */

            ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
            ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLR;
            MaintReDisp(&ReDisplay);
            UieNextMenu(aChildAC84Init2);                           /* Open Next Function */
            usUifACControl = 0;                                     /* Clear Control Save Area */
            break;

        default:
            UieNextMenu(aChildAC84Init1);                           /* Open Next Function */

            /* Display Address 1 of This Function */

            ParaRoundTbl.uchStatus = 0;                             /* Set W/ Amount Status */
            ParaRoundTbl.uchAddress = 1;                            /* Set Address 1 */
            ParaRoundTbl.uchMajorClass = CLASS_PARAROUNDTBL;        /* Set Major Class */

            MaintRoundRead(&ParaRoundTbl);                          /* Execute Read HALO Procedure */
            break;
        }
        break;

    case UIM_ACCEPTED:
        UieNextMenu(aChildAC84Accept);                              /* Open Next Function */
        break;

    case UIM_CANCEL:
        usUifACControl = 0;                                         /* Initialize Uif Control Save Area */

        /* Reset All Descriptor */

        ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
        ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLR;
        MaintReDisp(&ReDisplay);
        return(SUCCESS);

    default:
        return(UifACDefProc(pKeyMsg));                              /* execute Default Procedure */
    }
    return(SUCCESS);
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC84EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: A/C No.84 Enter Address Mode 
*===============================================================================
*/


/* Define Previous Function */

UIMENU FARCONST aParentAC84EnterAddr[] = {{UifAC84Function, CID_AC84FUNCTION},
                                          {NULL,            0               }};


SHORT UifAC84EnterAddr(KEYMSG *pKeyMsg) 
{
                    
    SHORT           sError;
    PARAROUNDTBL     ParaRoundTbl;
                    


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_NUM_TYPE);                  /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_NUM_TYPE:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {                   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }

            ParaRoundTbl.uchStatus = 0;                                         /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                                   /* W/o Amount Input Case */
                ParaRoundTbl.uchStatus |= MAINT_WITHOUT_DATA;                   /* Set W/o Amount Status */
            } else {                                                            /* W/ Amount Input Case */
                ParaRoundTbl.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            ParaRoundTbl.uchMajorClass = CLASS_PARAROUNDTBL;                    /* Set Major Class */
            if ((sError = MaintRoundRead(&ParaRoundTbl)) == MAINT_RND_POSI) {   /* Next is Position Input */
                UieAccept();                                                    /* Return to UifAC84Function() */
                return(SUCCESS);
            } else if (sError == MAINT_RND_DELI) {                              /* Next is Delimitor Input */ 
                UieExit(aParentAC84EnterAddr);
                return(SUCCESS);
            }
            return(sError);                                                 /* Set Success or Input Data Error */
            
        /* default:
              break;  */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC84EnterDelimit1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.84 Enter Delimitor Data 1 Mode 
*===============================================================================
*/


    
SHORT UifAC84EnterDelimit1(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARAROUNDTBL    ParaRoundTbl;
                   


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Case */
                MaintRoundReport();                                         /* Execute Report Procedure */
                UieExit(aACEnter);                                          /* Return to UifACEnter() */
                return(SUCCESS);
            } else {                                                        /* W/ Amount Case */
                if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT4) {               /* Over Digit */
                    return(LDT_KEYOVER_ADR);                                    
                }
                ParaRoundTbl.uchStatus = 0;                                     /* Set W/ Amount Status */
                ParaRoundTbl.uchMajorClass = CLASS_PARAROUNDTBL;                /* Set Major Class */
                ParaRoundTbl.uchMinorClass = CLASS_PARAROUNDTBL_DELI;           /* Set Minor Class */
                ParaRoundTbl.uchRoundDelimit = ( UCHAR)(pKeyMsg->SEL.INPUT.lData/100L);
                ParaRoundTbl.uchRoundModules = ( UCHAR)(pKeyMsg->SEL.INPUT.lData%100L);
            
                if ((sError = MaintRoundWrite(&ParaRoundTbl)) == SUCCESS) {         /* Read Function Completed */
                    UieAccept();                                                    /* Return to UifAC84Function() */
                }
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
**  Synopsis: SHORT UifAC84EnterDelimit2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.84 Enter Delimitor Data 2 Mode 
*===============================================================================
*/

    
SHORT UifAC84EnterDelimit2(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARAROUNDTBL    ParaRoundTbl;
                   


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Case */
                MaintFin(CLASS_MAINTTRAILER_PRTSUP);
                UieExit(aACEnter);                                          /* Return to UifACEnter() */
                return(SUCCESS);
            } else {                                                        /* W/ Amount Case */

                /* Check Digit */

                if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT4) {                       /* Over Digit */
                    return(LDT_KEYOVER_ADR);
                }

                ParaRoundTbl.uchStatus = 0;                                     /* Set W/ Amount Status */
                ParaRoundTbl.uchMajorClass = CLASS_PARAROUNDTBL;                    /* Set Major Class */
                ParaRoundTbl.uchMinorClass = CLASS_PARAROUNDTBL_DELI;           /* Set Minor Class */
                ParaRoundTbl.uchRoundDelimit = ( UCHAR)(pKeyMsg->SEL.INPUT.lData/100L);
                ParaRoundTbl.uchRoundModules = ( UCHAR)(pKeyMsg->SEL.INPUT.lData%100L);
            
                if ((sError = MaintRoundWrite(&ParaRoundTbl)) == SUCCESS) {         /* Read Function Completed */
                    UieAccept();                                                    /* Return to UifAC84Function() */
                }
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
**  Synopsis: SHORT UifAC84EnterPosition(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.84 Enter Position Data Mode 
*===============================================================================
*/

/* Define Key Sequence */

UISEQ FARCONST aszSeqAC84EnterPosition[] = {FSC_AC, FSC_VOID, 0};

    
SHORT UifAC84EnterPosition(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARAROUNDTBL    ParaRoundTbl;
    MAINTREDISP     ReDisplay;
                            


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC84EnterPosition);                                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_CANCEL:
        usUifACControl = 0;                                     /* Initialize Uif Control Save Area */

        /* Reset All Descriptor */

        ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
        ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLR;
        MaintReDisp(&ReDisplay);
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Case */
                MaintFin(CLASS_MAINTTRAILER_PRTSUP);
                UieExit(aACEnter);                                          /* Return to UifACEnter() */
                return(SUCCESS);
            } else {                                                        /* W/ Amount Case */
                /* Check Digit */

                if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {                       /* Over Digit */
                    return(LDT_KEYOVER_ADR);
                }

                ParaRoundTbl.uchStatus = 0;                                     /* Set W/ Amount Status */
                ParaRoundTbl.uchMajorClass = CLASS_PARAROUNDTBL;                    /* Set Major Class */
                ParaRoundTbl.uchMinorClass = CLASS_PARAROUNDTBL_POSI;               /* Set Minor Class */                    
                ParaRoundTbl.chRoundPosition = ( CHAR)pKeyMsg->SEL.INPUT.lData;
                if (usUifACControl & UIF_VOID ) {
                    ParaRoundTbl.chRoundPosition = ( CHAR)0 - ParaRoundTbl.chRoundPosition;
                } 
                if ((sError = MaintRoundWrite(&ParaRoundTbl)) == SUCCESS) {         
                    UieExit(aParentAC84EnterAddr);                  /* Return to UifAC84EnterFunction */                      
                }
            }
            return(sError);

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
              break;  */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                          /* Execute Default Procedure */
    }
}


