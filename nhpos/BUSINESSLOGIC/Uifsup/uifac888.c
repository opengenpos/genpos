/*
*===========================================================================
* Title       : Application Information Module                         
* Category    : User Interface For Supervisor, NHPOS Hospitality Application         
* Program Name: UIFAC888.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name       : Description
*    6-9-4 :            :R. Herrington : Initial                                    
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
//#include "pif.h"
//#include <uie.h>
#include <fsc.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <uifsup.h>
#include <uifpgequ.h>
#include <progrept.h>
#include <csprgrpt.h>
#include <mldmenu.h>

#include "uifsupex.h"                       /* Unique Extern Header for UI */

/*
*=============================================================================
**  Synopsis: SHORT UifAC999Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.999 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */



SHORT UifAC888Function(KEYMSG *pKeyMsg) 
{



        UifACPGMLDClear();
		
		MaintDisp(AC_CONFIG_INFO,                       /* A/C Number */
				  CLASS_MAINTDSPCTL_10N,            /* 10N10D Display */
				  0,                                /* Page Number */
				  0,                                /* PTD Type */
				  0,                                /* Report Type */
				  0,				                /* Reset Type */
				  0L,                               /* Amount Data */
				  0);                   /* "Lead Through for Reset Report"Lead Through Address */

		    MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class */              
                    RPT_ACT_ADR,                            /* Report Name Address */
                    0		,                             /* Report Name Address */
                    0,                                      /* Special Mnemonics Address */
                    RPT_READ_ADR,                           /* Report Name Address */
                    0,                                      /* Read Type */
                    0,                         /* A/C Number */
                    0,                                      /* Reset Type */
                    PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit */


		RptTerminalConfiguration(NULL);
		//MaintFin(CLASS_MAINTTRAILER_PRTSUP);         /* Execute Finalize Procedure */
		MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
		return SUCCESS;
}