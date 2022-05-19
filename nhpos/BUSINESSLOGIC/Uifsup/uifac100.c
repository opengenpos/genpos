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
* Title       : EJ File Read Report Module                         
* Category    : User Interface For Supervisor, NCR 2170 Int'l Hospitality Application         
* Program Name: UIFAC100.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC100Function()      : A/C No.100 Function Entry Mode
*               UifAC100EnterKey()      : A/C No.100 Enter AC Key Mode   
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Feb-01-93:00.00.01    :K.You      : initial                                   
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
#include <cswai.h>
#include <maint.h>
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <uifsup.h>
#include "appllog.h"

#include "uifsupex.h"                       /* Unique Extern Header for UI */
/*
*=============================================================================
**  Synopsis: SHORT UifAC100Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               
**  Description: A/C No.100 Function Entry Mode  
*===============================================================================
*/

/* Define Next Function */

UIMENU FARCONST aChildAC100EnterKey[] = {{UifAC100EnterKey, CID_AC100ENTERKEY},
                                         {NULL,             0                }};

SHORT UifAC100Function(KEYMSG *pKeyMsg) 
{
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC100EnterKey);           /* Open Next Function */
        MaintDisp(AC_EJ_RESET_RPT, CLASS_MAINTDSPCTL_10N, 0, 0, 0, 0, 0L, LDT_RESET_ADR);
        return(SUCCESS);

    default:
        return(UifACDefProc(pKeyMsg));              /* Execute Default Procedure */
    }
}

/*
*==============================================================================
**  Synopsis: SHORT UifAC100EnterKey(KEYMSG *pKeyMsg) 
* 
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*       Output: nothing
*
**      Return: SUCCESS         : Successful     
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.100 Enter AC Key Mode     
*===============================================================================
*/
SHORT UifAC100EnterKey(KEYMSG *pKeyMsg) 
{
    SHORT   sError;

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        /* W/o Amount Check */
        if (pKeyMsg->SEL.INPUT.uchLen) {                    /* W/ Amount */
            return(LDT_SEQERR_ADR);
        }

        switch (pKeyMsg->SEL.INPUT.uchMajor) {                          
        case FSC_AC:
			PifLog (MODULE_PRINT_SUPPRG_ID, LOG_EVENT_UIFUIE_ACTIONCODE);
			PifLog (MODULE_DATA_VALUE(MODULE_PRINT_SUPPRG_ID), 100);
            sError = RptEJReset();                      /* Execute EJ All Reset Report */ 
            UieExit(aACEnter);                          /* Return to UifACEnter() */
            return(sError);

        /* default:
              break; */
    }

    default:
        return(UifACDefProc(pKeyMsg));                      /* Execute Default Procedure */
    }
}


