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
* Title       : Dynamic Window Reload Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC194.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC194Function()       : A/C No.194 Function Entry Mode
* --------------------------------------------------------------------------
/*
==============================================================================
;                      I N C L U D E     F I L E s                         
;=============================================================================
*/

#include	<tchar.h>
#include <ecr.h>
#include <uie.h>
#include <fsc.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <uifsup.h>
#include <BlFWif.h>

#include "uifsupex.h"                       /* Unique Extern Header for UI */
#include "bl.h"
/*
*=============================================================================
**  Synopsis: SHORT UifAC194Function(VOID) 
*
*       Input:  nothing  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.194 Function  

		The addition of this AC code initially provides the ability to reload
		dynamic windows on the terminal that the operation is performed on.  A
		requested function of this AC code is that a sub-menu allow selection
		between two options, reload windows on the current terminal (as currently
		allowed), and reloading windows on all terminals. 12/7/2006 CSMALL

*===============================================================================
*/


SHORT UifAC194Function(KEYMSG *pKeyMsg)
{
	SHORT fsComStatus;

    switch(pKeyMsg->uchMsg){
    case UIM_INIT:

        MaintDisp(AC_RELOADOEP,                     /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  0,                                /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  0);                               /* Lead Through Address */

		fsComStatus = BlIntrnlGetNoticeBoardStatus(1);
		if ( !(fsComStatus & (BLI_STS_INQUIRY | BLI_STS_INQTIMEOUT | BLI_STS_STOP)) &&
			(fsComStatus & (BLI_STS_M_UPDATE | BLI_STS_BM_UPDATE | BLI_STS_M_REACHABLE)))
		{
			BlFwIfReloadOEPGroup();   /* Dynamic Window Reload */ 
		}
		else
		{
			// display error message indicating we do not have communication
			// with the Master terminal.
			/* --- display sequence error message number --- */
			UieErrorMsg(LDT_LNKDWN_ADR, UIE_WITHOUT);

		}

        UieExit(aACEnter);
        return(SUCCESS);
    default:
        return(UifACDefProc(pKeyMsg));
    }
}

