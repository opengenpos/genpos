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
* Title       : BroadCast/Request to DownLoad All Parameter Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC75.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC75Function()   : A/C No.75 Function Entry Mode 
*               UifAC75EnterKey()   : A/C No.75 Enter AC Key Mode  
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* Jun-16-92: K.You     : initial                                   
*          :           :                                    
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
#include	<stdlib.h>

#include <string.h>
#include <ecr.h>
#include <uie.h>
#include <fsc.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <uifsup.h>
#include <uifpgequ.h>
#include <csstbstb.h>
#include "appllog.h"

#include "uifsupex.h"

/*
*=============================================================================
**  Synopsis:    SHORT UifAC75Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.75 Function Entry Mode   
*===============================================================================
*/

/* Define Next Function */

UIMENU FARCONST aChildAC75EnterKey[] = {{UifAC75EnterKey, CID_AC75ENTERKEY},
                                        {NULL,            0               }};

    
SHORT UifAC75Function(KEYMSG *pKeyMsg) 
{
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC75EnterKey);            /* Open Next Function */

        MaintDisp(AC_ALL_DOWN,                      /* A/C Number */
                  CLASS_MAINTDSPCTL_10N,            /* 10N10D Display */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  0,                                /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_COPYOK_ADR);                  /* " Copy OK? " Lead Through Address */
                  
        return(SUCCESS);

    default:
        return(UifACDefProc(pKeyMsg));              /* Execute Default Procedure */
    }
}

/*
*==============================================================================
**  Synopsis: SHORT UifAC75EnterKey(KEYMSG *pKeyMsg) 
* 
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*       Output: nothing
*
**      Return: SUCCESS         : Successful     
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.75 Enter AC Key Mode  
*===============================================================================
*/
SHORT UifAC75EnterKey(KEYMSG *pKeyMsg) 
{
    SHORT   sError;

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        /*
		 *     W/o Amount Check
		 *  For an AC75 on a Satellite we will allow an argument of CLI_LOAD_AC75_ARG_LAY
		 *  This argument indicates we are to also request layout files if we are a Satellite.
		 */
        if (pKeyMsg->SEL.INPUT.uchLen && pKeyMsg->SEL.INPUT.uchLen != 2) {  /* W/ Amount */
            return(LDT_SEQERR_ADR);
        }

		if (CstIamDisconnectedUnjoinedSatellite() == STUB_SUCCESS)
			return(LDT_PROHBT_ADR);

		switch (pKeyMsg->SEL.INPUT.uchMajor) {                          
        case FSC_AC:

            MaintDisp(AC_ALL_DOWN,                          /* A/C Number */
                      0,                                    /* Descriptor */
                      0,                                    /* Page Number */
                      0,                                    /* PTD Type */
                      0,                                    /* Report Type */
                      0,                                    /* Reset Type */
                      0L,                                   /* Amount Data */
                      LDT_COMUNI_ADR);                      /* " During Comm. " Lead Through Address */

			sError = MaintLoadAll(pKeyMsg->SEL.INPUT.lData);    /* Execute DownLoad All Parameter */

			PifLog (MODULE_PRINT_SUPPRG_ID, LOG_EVENT_UIFUIE_ACTIONCODE);
			PifLog (MODULE_DATA_VALUE(MODULE_PRINT_SUPPRG_ID), 75);                    // Action Code 75, Broadcast Provisioning
			PifLog (MODULE_ERROR_NO(MODULE_PRINT_SUPPRG_ID), (USHORT)abs(sError));     // Action Code 75, Broadcast Provisioning

			UieExit(aACEnter);                              /* Return to UifACEnter() */
            return(sError);                                 /* Return Success or Error Status */

        /* default:
              break; */
    }

    default:
        return(UifACDefProc(pKeyMsg));                      /* Execute Default Procedure */
    }
}




    
    
    
