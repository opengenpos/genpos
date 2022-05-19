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
* Title       : Start / Stop Charge Posting.
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application
* Program Name: UIFAC78.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs1 /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC78Function()    : AC No.78 Function Entry Mode.
*               UifAC78EnterData()   : AC No.78 Enter Data Mode.
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* 11.11.93 : M.Yamamoto: initial                                   
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
;=============================================================================
*/

#include	<tchar.h>
#include <string.h>
#include <ecr.h>
#include <uie.h>
#include <fsc.h>
#include <pif.h>
#include <plu.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <eept.h>
#include <uifsup.h>
#include <uifpgequ.h>
#include <csstbfcc.h>
#include <csstbstb.h>
#include <cpm.h>
#include <csstbcpm.h>
#include <mldmenu.h>

#include "uifsupex.h"

/*
*=============================================================================
**  Synopsis:    SHORT UifAC78Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: AC No.78 Function Entry Mode   
*===============================================================================
*/

/* Define Next Function */

static UIMENU CONST aChildAC78EnterData[] = {{UifAC78EnterData,CID_AC78ENTERDATA},
                                        {NULL,           0               }};

SHORT UifAC78Function(KEYMSG *pKeyMsg) 
{
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC78EnterData);            /* Open Next Function */

        UifACPGMLDDispCom(TypeSelectReport7);       /* display type on LCD */

        MaintDisp(AC_START_STOP_CPM,                /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  0,                                /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount */
                  LDT_FUNCTYPE_ADR);                /* "Request Function Type Entry" Lead Through Address */
                  
        return(SUCCESS);

    default:
        return(UifACDefProc(pKeyMsg));
    }
}

/*
*==============================================================================
**  Synopsis: SHORT UifAC78EnterData(KEYMSG *pKeyMsg) 
* 
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*       Output: nothing
*
**      Return: SUCCESS         : Successful     
*               LDT_KEYOVER_ADR : Wrong Data Error 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: AC No.78 Enter Data Mode    
*===============================================================================
*/

SHORT UifAC78EnterData(KEYMSG *pKeyMsg) 
{
    SHORT           sError;
    
    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {                          
        case FSC_AC:                                              
			// Remove legacy Charge Post code for OpenGenPOS.
			// This is to eliminate the possibility of not meeting PCI-DSS account holder information
			// security requirements. GenPOS Rel 2.3.0 retained legacy source but was using the
			// Datacap Out Of Scope control for electronic payment.
			//
			//    OpenGenPOS Rel 2.4.0  3/22/2021  Richard Chambers
			sError = LDT_PROHBT_ADR;
            return(sError);                                 /* Set Success, Busy Data Error */

        /* default:
              break; */
    }

    default:
        return(UifACDefProc(pKeyMsg));                      /* Execute Default Procedure */
    }
}
/**** End of File ****/
