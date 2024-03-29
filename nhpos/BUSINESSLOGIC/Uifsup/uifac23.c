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
* Title       : Register Financial Report Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC23.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC23Function()       : AC23 Function Entry Mode 
*               UifAC23EnterDataType()  : AC23 Enter Data Type   
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* Jun-16-92: K.You     : Initial                                   
* Dec-22-92: K.You     : Del UISEQ FARCONST aszSequence[] = {FSC_NUM_TYPE,0};
* Jul-22-93: M.Suzuki  : Move aszSeqAC23EnterDataType[] to uifacmod.c.
* Jul-26-95: M.Ozawa   : Modify for Report on LCD
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
#include <stdio.h>

#include <ecr.h>
#include <uie.h>
#include <fsc.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <uifsup.h>
#include <uifpgequ.h>
#include <mldmenu.h>
#include <mldsup.h>

#include "uifsupex.h"

/*
==============================================================================
;                      C O M M O N   R O M   T A B L E s                         
;=============================================================================
*/


/* Define Key Sequence */


/*
*=============================================================================
**  Synopsis:    SHORT UifAC23Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description:  
*===============================================================================
*/

/* Define Next Function */

UIMENU FARCONST aChildAC23EnterDataType[] = {{UifAC23EnterDataType,CID_AC23ENTERDATATYPE},
                                    {NULL,                0                    }};

    
SHORT UifAC23Function(KEYMSG *pKeyMsg) 
{
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC23EnterDataType);               /* Open Next Function */
        UifACPGMLDDispCom(DaylyPTDSaveSelect);
        MaintDisp(AC_REGFIN_READ_RPT, 0, 0, 0, 0, 0, 0L, LDT_SLCDORP_ADR);
        return(SUCCESS);

    default:
        return(UifACDefProc(pKeyMsg));
    }
}

/*
*==============================================================================
**  Synopsis: SHORT UifAC23EnterDataType(KEYMSG *pKeyMsg) 
* 
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*       Output: nothing
*
**      Return: SUCCESS         : Successful     
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description:  
*===============================================================================
*/
SHORT UifAC23EnterDataType(KEYMSG *pKeyMsg) 
{
    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC23EnterDataType);                       /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        /* Digit Check */

        if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {                   /* Over Digit */
            return(LDT_KEYOVER_ADR);
        }

        switch (pKeyMsg->SEL.INPUT.uchMajor) {                          
        case FSC_AC:
            /* allow AC key for PTD, V3.3 */
            /* return(LDT_SEQERR_ADR);  */

        case FSC_NUM_TYPE:                                              
            if (pKeyMsg->SEL.INPUT.uchLen) {                /* W/ Amount Input Case */
				UCHAR uchMinorClass = 0;

				switch (pKeyMsg->SEL.INPUT.lData) {
					case RPT_TYPE_DALY:
					case RPT_TYPE_PTD:
					case RPT_TYPE_DALYSAVE:
					case RPT_TYPE_PTDSAVE:
						// write out the header for the AC23 report
						uchUifACDataType = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
						UifACPGMLDClear();
						MaintDisp(AC_REGFIN_READ_RPT, 0, 0, ( UCHAR)pKeyMsg->SEL.INPUT.lData, 0, 0, 0L, 0);

						switch (pKeyMsg->SEL.INPUT.lData) {
							case RPT_TYPE_DALY:             /* AC23 Daily Function Case */
								uchMinorClass = CLASS_TTLCURDAY;
								break;
							case RPT_TYPE_PTD:              /* AC23 PTD Function Case */
								uchMinorClass = CLASS_TTLCURPTD;
								break;
							case RPT_TYPE_DALYSAVE:         /* AC23 Previous or Saved Daily Function Case */
								uchMinorClass = CLASS_TTLSAVDAY;
								break;
							case RPT_TYPE_PTDSAVE:          /* AC23 Previous or Saved PTD Function Case */
								uchMinorClass = CLASS_TTLSAVPTD;
								break;
						}
						if (uchMinorClass) {
							// save the currently selected display/print option, generate an html version
							// of the report and then restore the user selected display/print option to
							// generate the report to the user selected device.
							UCHAR   uchUifACRptOnOffMldSave = uchUifACRptOnOffMld;
							FILE    *fpFile = ItemOpenHistorialReportsFolder (RPTREGFIN_FOLDER_ANY, AC_REGFIN_READ_RPT, uchMinorClass, RPT_ALL_READ, 0, 0, 0);

							if (fpFile) {
								ItemGenerateAc23Report(CLASS_TTLREGFIN, uchMinorClass, RPT_ALL_READ, fpFile, 0);
								ItemCloseHistorialReportsFolder(fpFile);
							}
							uchUifACRptOnOffMld = uchUifACRptOnOffMldSave;
							RptRegFinRead(uchMinorClass, RPT_ALL_READ);
						}
						UieExit(aACEnter);
						return(SUCCESS);
						break;

					default:
						return(LDT_KEYOVER_ADR);                            /* Return Wrong Data Error */
				}
            }
            return(LDT_SEQERR_ADR);                                    /* Return Wrong Data Error */
		}

		default:
			return(UifACDefProc(pKeyMsg));                                  /* Execute Default Procedure */
    }
}

/**** End of File ****/
