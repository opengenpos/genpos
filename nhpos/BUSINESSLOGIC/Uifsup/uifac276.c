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
* Title       : Disconnect The Master or Backup Master Terminal                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC276.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC276Function()   : A/C No.276 Function Entry Mode 
*               UifAC276EnterKey()   : A/C No.276 Enter AC Key Mode   
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* Jul-20-92: K.You     : initial                                   
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
#include <string.h>
#include <ecr.h>
#include <uie.h>
#include <fsc.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <uifsup.h>
#include <uifpgequ.h>
#include <ej.h>
#include <csstbej.h>
#include <csstbttl.h>
#include <cscas.h>
#include "uifsupex.h"
#include "plu.h"
#include "pararam.h"
/*
*=============================================================================
**  Synopsis:    SHORT UifAC276Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.276 Function Entry Mode   
*===============================================================================
*/

/* Define Next Function */

UIMENU FARCONST aChildAC276EnterKey[] = {{UifAC276EnterKey, CID_AC276ENTERKEY},
                                        {NULL,            0               }};

    
SHORT UifAC276Function(KEYMSG *pKeyMsg) 
{
    

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC276EnterKey);            /* Open Next Function */

        MaintDisp(AC_SET_DELAYED_BAL_OFF,                   /* A/C Number */
                  CLASS_MAINTDSPCTL_10N,            /* 10N10D Display */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  0,                                /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_CLRABRT_ADR);                 /* " Request Clear/Abort Entry " Lead Through Address */
                  
        return(SUCCESS);

    default:
        return(UifACDefProc(pKeyMsg));              /* Execute Default Procedure */
    }
}

/*
*==============================================================================
**  Synopsis: SHORT UifAC276EnterKey(KEYMSG *pKeyMsg) 
* 
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*       Output: nothing
*
**      Return: SUCCESS         : Successful     
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.276 Enter AC Key Mode     
*===============================================================================
*/


SHORT UifAC276EnterKey(KEYMSG *pKeyMsg) 
{
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

			//we need to turn off the descriptor, and flag that is
			//associated with delay balance JHHJ
			MaintDelayedBalance(MAINT_DBOFF);

			//actually update the total files
			//if we are not the master we will send the master
			//a message that requests to update the 
			SerTtlTumUpdateDelayedBalance();
			
            
            UieExit(aACEnter);                              /* Return to UifACEnter() */
            return SUCCESS;                                 /* Return Success or Error Status */


        /* default:
              break; */
    }

    default:
        return(UifACDefProc(pKeyMsg));                      /* Execute Default Procedure */
    }
}




    
    
    
