/*
*===========================================================================
* Title       : Set Control Table of Menu Page Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC777.C
 --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC77Function()        : A/C No.777 Function Entry Mode
*               UifAC777EnterData1()     : A/C No.777 Enter Data1 Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jul-01-92:00.00.01    :K.You      : initial                                   
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
#include <stdio.h>
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

#include <BlFWif.h>
#include <mldmenu.h>

#include "uifsupex.h"                       /* Unique Extern Header for UI */


/*
==============================================================================
;                      C O M M O N   R O M   T A B L E s                         
;=============================================================================
*/

// prototypes used in the UIMENU tables
static SHORT UifAC777EnterData1(KEYMSG *pKeyMsg);


// defines used to handle specific menu actions
// these defines are used to test against the selected function
// which is included in the keymsg struct as pKeyMsg->SEL.usFunc
#define CID_AC777ENTERDATA1     1              // UifAC777EnterData1()

/*
	Define Key Sequence tables

	The following tables provide the proper functions to implement the
	menu walking machine which is used by the uifsup subsystem.
	In our case we have only a single step to perform, when the user enters
	our action code, we then do a single data entry step.

	The beginning entry point for our action code is the entry point UifAC777Function()
	which is provided to the function UifACEnter ().

	The function UieNextMenu() is used in the code below to indicate to the uifsup
	subsystem the address of the next UIMENU to go to when traversing the menu tree.

*/

static UIMENU FARCONST aChildAC777Init[] = {
	{UifAC777EnterData1, CID_AC777ENTERDATA1},
    {NULL, 0}
};


/* Define Next Function at UIM_ACCEPTED */

static UIMENU FARCONST aChildAC777Accept[] = {
	{NULL, 0}
};

/*
*=============================================================================
**  Synopsis: SHORT UifAC777Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.777 Function Entry Mode  
*===============================================================================
*/

SHORT UifAC777Function(KEYMSG *pKeyMsg) 
{
    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC777Init);                                 /* Open Next Function */
        break;

    case UIM_ACCEPTED:
        UieNextMenu(aChildAC777Accept);                           /* Open Next Function */
        break;

    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
    return(SUCCESS);
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC777EnterData1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_PROHBT_ADR  : Could not open the specified layout file
*				LDT_EQUIPPROVERROR_ADR: terminal number in sysconfig is not good
*               
**  Description: A/C No.777 Enter Data1 Mode 
*===============================================================================
*/

static SHORT UifAC777EnterData1(KEYMSG *pKeyMsg) 
{
    SHORT            sError = SUCCESS;
	PARATERMINALINFO TermInfo;
	SYSCONFIG CONST *pSysConfig;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                                    /* Register Key Sequence */
        /* Clear Lead Through */
        UifACPGMLDClear();

        MaintDisp(AC_SET_LAYOUT,                    /* A/C Number */
                  CLASS_MAINTDSPCTL_10N,            /* 10N10D Display */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  0,                                /* Report Type */
                  0,				                /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_FUNCTYPE_ADR);                      // "Lead Through requesting ID number
        return(SUCCESS);

    case UIM_INPUT:
		pSysConfig = PifSysConfig();  // obtain the system configuration getting our terminal number
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Case */
                UieExit(aACEnter);                                          /* Return to UifACEnter() */
            } else {                                                        /* W/ Amount Case */
                /*
					Check the data entered to ensure it is valid.

					The number entered should be within the range of 0 through 26.

					A value of 0 indicates to change to the default layout display behavior.
					The default layout behavior is PARAMLA for Touchscreen and PARAMLB for keyboard.

					A value of 1 through 26 indicates which layout file to use,
					1 -> PARAMLA, 2 -> PARAMLB, 3 -> PARAMLC, ..., 26 -> PARAMLZ.
				*/

                if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {               /* Over Digit */
                    return(LDT_KEYOVER_ADR);
                }
                if (pKeyMsg->SEL.INPUT.lData > 26) {               /* Invalid data, range is 0 through 26 */
                    return(LDT_KEYOVER_ADR);
                }
				TermInfo.uchMajorClass = CLASS_PARATERMINALINFO;
				TermInfo.uchAddress = PIF_CLUSTER_MASK_TERMINAL_NO(pSysConfig->usTerminalPosition);

				NHPOS_ASSERT(TermInfo.uchAddress > 0 && TermInfo.uchAddress < 17);

				if (TermInfo.uchAddress > 16) {
                    return(LDT_EQUIPPROVERROR_ADR);
				}

				/*
					Thus far, it looks like the user has entered an acceptable value and
					our terminal number is within expected bounds.
					Initialize the layout file name string to binary zeros (end of string character)
					and then generate the new layout file name.
					If the user entered 0, this means default so we don't put a PARAML name into
					the layout area.
					If the user entered 1 through 26 then we create the proper PARAML name, put
					it into the layout file name of the TERMINFO struct ready for the provisioning
					action.  However, we do one last check, we check that the requested layout
					file does actually exist.  If not exist, return prohibitted other wise we
					accept the change.
				 */
				ParaTerminalInfoParaRead (&TermInfo);
				memset (TermInfo.TerminalInfo.aszDefaultMenuFileName, 0, sizeof(TermInfo.TerminalInfo.aszDefaultMenuFileName));
				if (pKeyMsg->SEL.INPUT.lData > 0) {
					WCHAR  szBuffer[128];
					FILE *pFile = 0;
					memcpy(TermInfo.TerminalInfo.aszDefaultMenuFileName, _T("PARAMLA"), 14);
					TermInfo.TerminalInfo.aszDefaultMenuFileName[6] += pKeyMsg->SEL.INPUT.lData - 1;
					wcscpy (szBuffer, L"\\FlashDisk\\NCR\\Saratoga\\Database\\");
					wcscat (szBuffer,  TermInfo.TerminalInfo.aszDefaultMenuFileName);
					pFile = _wfopen (szBuffer, L"r");
					NHPOS_ASSERT_TEXT(pFile != 0, "Couldn't test open layout file.");
					if (pFile == 0) {
						return (LDT_PROHBT_ADR);
					}
					fclose (pFile);
				}

				ParaTerminalInfoParaWrite (&TermInfo);
				BlFwIfReloadLayout2();
				UieExit(aACEnter);                                          /* Return to UifACEnter() */
			}
            return(SUCCESS);
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                          /* Execute Default Procedure */
    }
}

