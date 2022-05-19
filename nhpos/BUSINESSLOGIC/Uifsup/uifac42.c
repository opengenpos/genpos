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
* Title       : Copy Total/Counter File Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC42.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC42Function()   : A/C No.42 Function Entry Mode 
*               UifAC42EnterKey()   : A/C No.42 Enter AC Key Mode   
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* Jul-09-92: K.You     : initial                                   
*          :           :                                    
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

#include <tchar.h>
#include <string.h>
#include <stdlib.h>

#include <ecr.h>
#include <uie.h>
#include <fsc.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <uifsup.h>
#include <uifpgequ.h>
#include "appllog.h"

#include "uifsupex.h"

/*
*=============================================================================
**  Synopsis:    SHORT UifAC42Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.42 Function Entry Mode   
*===============================================================================
*/

/* Define Next Function */
UIMENU CONST aChildAC42EnterKey[] = {{UifAC42EnterKey, CID_AC42ENTERKEY},
                                     {NULL,            0               }};

    
SHORT UifAC42Function(KEYMSG *pKeyMsg) 
{
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC42EnterKey);            /* Open Next Function */
        MaintDisp(AC_COPY_TTL, CLASS_MAINTDSPCTL_10N, 0, 0, 0, 0, 0L, LDT_COPYOK_ADR);  /* " Copy OK? " Lead Through Address */
        return(SUCCESS);

    default:
        return(UifACDefProc(pKeyMsg));              /* Execute Default Procedure */
    }
}

/*
*==============================================================================
**  Synopsis: SHORT UifAC42EnterKey(KEYMSG *pKeyMsg) 
* 
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*       Output: nothing
*
**      Return: SUCCESS         : Successful     
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.42 Enter AC Key Mode     
*===============================================================================
*/
SHORT UifAC42EnterKey(KEYMSG *pKeyMsg) 
{
    SHORT   sError;

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {                          
        case FSC_AC:
            /* W/o Amount Check */
            if (pKeyMsg->SEL.INPUT.uchLen) {                /* W/ Amount */
                return(LDT_SEQERR_ADR);
            }

            MaintDisp(AC_COPY_TTL, 0, 0, 0, 0, 0, 0L, LDT_COMUNI_ADR);    /* " During Comm. " Lead Through Address */
            sError = MaintCopyTtl();                        /* Execute Copy Total/Counter File */
			PifLog (MODULE_PRINT_SUPPRG_ID, LOG_EVENT_UIFUIE_ACTIONCODE);
			PifLog (MODULE_DATA_VALUE(MODULE_PRINT_SUPPRG_ID), 42);                    // Action Code 42, Broadcast Totals
			PifLog (MODULE_ERROR_NO(MODULE_PRINT_SUPPRG_ID), (USHORT)abs(sError));     // Action Code 42, Broadcast Totals

            UieExit(aACEnter);                              /* Return to UifACEnter() */
            return(sError);                                 /* Return Success or Error Status */
    }

    default:
        return(UifACDefProc(pKeyMsg));                      /* Execute Default Procedure */
    }
}