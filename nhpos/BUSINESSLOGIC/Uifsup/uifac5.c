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
* Title       : Set Control Table of Menu Page Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC5.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC5Function()        : A/C No.5 Function Entry Mode
*               UifAC5EnterAddr()       : A/C No.5 Enter Address Mode
*               UifAC5EnterData1()      : A/C No.5 Enter Data1 Mode
*               UifAC5EnterData2()      : A/C No.5 Enter Data2 Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jul-01-92:00.00.01    :K.You      : initial                                   
*          :            :                                    
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
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <uifsup.h>
#include <uifpgequ.h>

#include "uifsupex.h"                       /* Unique Extern Header for UI */


/*
==============================================================================
;                      C O M M O N   R O M   T A B L E s                         
;=============================================================================
*/


/* Define Key Sequence */
UISEQ CONST aszSeqACFSC_NUM_TYPE[] = {FSC_NUM_TYPE, 0};


/*
*=============================================================================
**  Synopsis: SHORT UifAC5Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.5 Function Entry Mode  
*===============================================================================
*/

/* Define Next Function at UIM_INIT */
UIMENU CONST aChildAC5Init[] = {{UifAC5EnterAddr, CID_AC5ENTERADDR},
                                   {UifAC5EnterData1, CID_AC5ENTERDATA1},
                                   {NULL,             0                 }};

/* Define Next Function at UIM_ACCEPTED */
UIMENU CONST aChildAC5Accept[] = {{UifAC5EnterAddr, CID_AC5ENTERADDR},
                                     {UifAC5EnterData2, CID_AC5ENTERDATA2},
                                     {NULL,             0                 }};



SHORT UifAC5Function(KEYMSG *pKeyMsg) 
{
    PARACTLTBLMENU      ParaCtlMenu;   

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC5Init);                                 /* Open Next Function */

        /* Display Address 1 of This Function */
        ParaCtlMenu.uchStatus = 0;                                  /* Set W/ Amount Status */
        ParaCtlMenu.uchAddress = 1;                                 /* Set Address 1 */
        ParaCtlMenu.uchMajorClass = CLASS_PARACTLTBLMENU;           /* Set Major Class */

        MaintCtlTblMenuRead(&ParaCtlMenu);                          
        break;

    case UIM_ACCEPTED:
        UieNextMenu(aChildAC5Accept);                           /* Open Next Function */
        break;

    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
    return(SUCCESS);
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC5EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: A/C No.5 Enter Address Mode 
*===============================================================================
*/
SHORT UifAC5EnterAddr(KEYMSG *pKeyMsg) 
{
    SHORT           sError;
    PARACTLTBLMENU  ParaCtlMenu;
                    
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_NUM_TYPE);                  /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_NUM_TYPE:
            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {       /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }

            ParaCtlMenu.uchStatus = 0;                          /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                   /* W/o Amount Input Case */
                ParaCtlMenu.uchStatus |= MAINT_WITHOUT_DATA;    /* Set W/o Amount Status */
            } else {                                            /* W/ Amount Input Case */
                ParaCtlMenu.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Read Control Table Menu Procedure */
            ParaCtlMenu.uchMajorClass = CLASS_PARACTLTBLMENU;   /* Set Major Class */

            if ((sError = MaintCtlTblMenuRead(&ParaCtlMenu)) == SUCCESS) {    
                UieAccept();                                    /* Return to UifAC5Function() */
            }
            return(sError);                                     /* Set Success or Input Data Error */
        }

    default:
        return(UifACDefProc(pKeyMsg));                          /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC5EnterData1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.5 Enter Data1 Mode 
*===============================================================================
*/
SHORT UifAC5EnterData1(KEYMSG *pKeyMsg) 
{
    SHORT           sError;
    PARACTLTBLMENU  ParaCtlMenu;
                   
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Case */
                MaintCtlTblMenuReport();                                    /* Execute Report Procedure */
                UieExit(aACEnter);                                          /* Return to UifACEnter() */
                return(SUCCESS);
            } else {                                                        /* W/ Amount Case */
                /* Check Digit */
                if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {                   /* Over Digit */
                    return(LDT_KEYOVER_ADR);
                }
                /* ParaCtlMenu.uchMajorClass = CLASS_PARACTLTBLMENU;               Set Major Class */
                ParaCtlMenu.uchStatus = 0;
                ParaCtlMenu.uchPageNumber = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
                if ((sError = MaintCtlTblMenuWrite(&ParaCtlMenu)) == SUCCESS) { 
                    UieAccept();                                                /* Return to UifAC5Function() */
                }
            }
            return(sError);
        }

    default:
        return(UifACDefProc(pKeyMsg));                          /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC5EnterData2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.5 Enter Data2 Mode 
*===============================================================================
*/
SHORT UifAC5EnterData2(KEYMSG *pKeyMsg) 
{
    SHORT           sError;
    PARACTLTBLMENU  ParaCtlMenu;
                   
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                                        /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            if (!pKeyMsg->SEL.INPUT.uchLen) {                                   /* W/o Amount Case */
                MaintFin(CLASS_MAINTTRAILER_PRTSUP);
                UieExit(aACEnter);
                return(SUCCESS);
            } else {                                                            /* W/ Amount Case */
                /* Check Digit */
                if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {                   /* Over Digit */
                    return(LDT_KEYOVER_ADR);
                }
                /* ParaCtlMenu.uchMajorClass = CLASS_PARACTLTBLMENU;               Set Major Class */
                ParaCtlMenu.uchStatus = 0;
                ParaCtlMenu.uchPageNumber = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
                if ((sError = MaintCtlTblMenuWrite(&ParaCtlMenu)) == SUCCESS) { 
                    UieAccept();                                                /* Return to UifAC5Function() */
                }
            }
            return(sError);
        }

    default:
        return(UifACDefProc(pKeyMsg));                          /* Execute Default Procedure */
    }
}