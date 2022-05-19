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
* Title       : Guest Check File Reset Report Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC3.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC3Function()        : A/C No.3 Function Entry Mode
*               UifAC3EnterType()       : A/C No.3 Enter Type Mode
*               UifAC3EnterData1()      : A/C No.3 Enter G/C No. Mode
*               UifAC3EnterData2()      : A/C No.3 Enter Waiter No. Mode
*               UifAC3EnterKey()        : A/C No.3 Reset Confirm Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jul-08-92:00.00.01    :K.You      : initial                                   
* Nov-26-92:01.00.00    :K.You      : Display GC No. and Server No. at Ind. Reset                                    
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
#include "appllog.h"

#include "uifsupex.h"                       /* Unique Extern Header for UI */


/*
==============================================================================
;                      C O M M O N   R O M   T A B L E s                         
;=============================================================================
*/


/* Define Key Sequence */
UISEQ CONST aszSeqAC3EnterKey[] = {FSC_AC, FSC_CANCEL, 0};

static USHORT       usUifGCNumber = 0;                  /* GC No. Save Area */


/*
*=============================================================================
**  Synopsis: SHORT UifAC3Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.3 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */
UIMENU CONST aChildAC3Init[] = {{UifAC3EnterType, CID_AC3ENTERTYPE},
                                {NULL,            0               }};


SHORT UifAC3Function(KEYMSG *pKeyMsg) 
{
    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC3Init);                             /* Open Next Function */
        return(SUCCESS);                                        

    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC3EnterType(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.3 Enter Type Mode 
*===============================================================================
*/
    
/* Define Next Function */

UIMENU CONST aChildAC3EnterType1[] = {{UifAC3EnterData1, CID_AC3ENTERDATA1},
                                      {NULL,             0                }};

UIMENU CONST aChildAC3EnterType2[] = {{UifAC3EnterData2, CID_AC3ENTERDATA2},
                                      {NULL,             0                }};

UIMENU CONST aChildAC3EnterType3[] = {{UifAC3EnterKey, CID_AC3ENTERKEY},
                                      {NULL,           0              }};

UIMENU CONST aChildAC3EnterType4[] = {{UifAC3EnterData4, CID_AC3ENTERDATA4},
                                      {NULL,           0              }};


SHORT UifAC3EnterType(KEYMSG *pKeyMsg) 
{
    SHORT   sError;

    switch(pKeyMsg->uchMsg)
	{
		case UIM_INIT:
			UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */
			UifACPGMLDDispCom(TypeSelectReport10);      /* distinguish from AC 2, V3.3 */
			MaintDisp(AC_GCFL_RESET_RPT, 0, 0, 0, 0, 0, 0L, LDT_REPOTYPE_ADR);                /* "Request Report Type Entry" Lead Through Address */
			return(SUCCESS);

		case UIM_INPUT:
			switch (pKeyMsg->SEL.INPUT.uchMajor)
			{      
				case FSC_AC:
					/* Check W/ Amount */
					if (!(pKeyMsg->SEL.INPUT.uchLen)) {             /* W/o Amount */
						return(LDT_SEQERR_ADR);
					}
        
					/* Check Digit */
					if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {   /* Over Digit */
						return(LDT_KEYOVER_ADR);
					}
        
					/* Check Input Data */
					PifLog (MODULE_PRINT_SUPPRG_ID, LOG_EVENT_UIFUIE_ACTIONCODE);
					PifLog (MODULE_DATA_VALUE(MODULE_PRINT_SUPPRG_ID), 3);                                    // Action Code 3, Broadcast Totals
					PifLog (MODULE_DATA_VALUE(MODULE_PRINT_SUPPRG_ID), (USHORT)(pKeyMsg->SEL.INPUT.lData));   // Action Code 3, Broadcast Totals

					if (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_ALL) {             /* All Reset Type */
						sError = RptGuestLockByGuestCheck (RPT_ALL_LOCK, 0);
						if (sError == SUCCESS) {
							uchUifACRptType = ( UCHAR)RPT_TYPE_ALL;             /* Set Report Type to Save Area */
							UieNextMenu(aChildAC3EnterType3);                   /* Open Next Function */
							return(SUCCESS);
						} else {
							UieExit(aACEnter);                                  /* Return to UifACEnter() */
							return(sError);
						}
					} else if (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_IND) {      /* Individual Reset Type */
						uchUifACRptType = ( UCHAR)RPT_TYPE_IND;                 /* Set Report Type to Save Area */
						UieNextMenu(aChildAC3EnterType1);                       /* Open Next Function */
						return(SUCCESS);
					} else if (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_WAIIND) {   /* Individual Reset by Waiter Type */
						uchUifACRptType = ( UCHAR)RPT_TYPE_WAIIND;              /* Set Report Type to Save Area */
						UieNextMenu(aChildAC3EnterType2);                       /* Open Next Function */
						return(SUCCESS);
					} else if (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_GCF_RESET) {   /* Individual Reset by Waiter Type */
						uchUifACRptType = ( UCHAR)RPT_TYPE_GCF_RESET;              /* Set Report Type to Save Area */
						UieNextMenu(aChildAC3EnterType4);                       /* Open Next Function */
						return(SUCCESS);
					}
            
					return(LDT_KEYOVER_ADR);                                /* Return Wrong Error */

				default:
					break;
			}
			/* break; */

		default:
			return(UifACDefProc(pKeyMsg));                                  /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC3EnterData1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.3 Enter G/C No. Mode 
*===============================================================================
*/
SHORT UifAC3EnterData1(KEYMSG *pKeyMsg) 
{
    SHORT   sError;
    USHORT  usGCNumber;

    switch(pKeyMsg->uchMsg)
	{
		case UIM_INIT:
			UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */
			UifACPGMLDClear();
			MaintDisp(AC_GCFL_RESET_RPT, 0, 0, 0, uchUifACRptType, 0, 0L, LDT_NUM_ADR);                     /* "Number Entry" Lead Through Address */
			return(SUCCESS);
                                                                            
		case UIM_INPUT:
			switch (pKeyMsg->SEL.INPUT.uchMajor)
			{
				case FSC_AC:
					/* Check W/ Amount */
					if (!pKeyMsg->SEL.INPUT.uchLen) {                 /* W/o Amount */
						return(LDT_SEQERR_ADR);
					}
        
					/* Check Digit */
					if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT7) {     /* R3.3 */
						return(LDT_KEYOVER_ADR);
					}       

					/* Check GC Number and Execute */
					sError = MaintChkGCNo((ULONG)pKeyMsg->SEL.INPUT.lData, &usGCNumber);
					if (sError != SUCCESS) {
						return(sError);
					}

					sError = RptGuestLockByGuestCheck (RPT_IND_LOCK, usGCNumber);
					if (sError == SUCCESS) {
						usUifGCNumber = (USHORT)pKeyMsg->SEL.INPUT.lData;
						usUifACRptGcfNo = usGCNumber;                /* Save GC Number to Own Save Area */
						UieNextMenu(aChildAC3EnterType3);             /* Open Next Function */
						return(SUCCESS);
					}
					UieExit(aACEnter);                                /* Return to UifACEnter() */
					return(sError);

				default:
					break;
			}
			/* break; */

		default:
			return(UifACDefProc(pKeyMsg));                            /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC3EnterData2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.3 Enter Waiter No. Mode 
*===============================================================================
*/
SHORT UifAC3EnterData2(KEYMSG *pKeyMsg) 
{
    SHORT   sError;

    switch(pKeyMsg->uchMsg)
	{
		case UIM_INIT:
			UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */
			UifACPGMLDClear();
			MaintDisp(AC_GCFL_RESET_RPT, 0, 0, 0, uchUifACRptType, 0, 0L, LDT_WTNO_ADR);   /* "Enter Waiter #" Lead Through Address */
			return(SUCCESS);
                                                                            
		case UIM_INPUT:
			switch (pKeyMsg->SEL.INPUT.uchMajor) {
				case FSC_AC:
					/* Check W/ Amount */
					if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount */
						return(LDT_SEQERR_ADR);
					}
        
					/* Check 0 Data and Digit */
					if (!pKeyMsg->SEL.INPUT.lData || (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT3)) {               
						return(LDT_KEYOVER_ADR);
					}       

					sError = RptGuestLockByCashier (pKeyMsg->SEL.INPUT.lData);
					if (sError == SUCCESS) {
						usUifACRptGcfNo = (USHORT)pKeyMsg->SEL.INPUT.lData;   /* Save GC Number to Own Save Area */
						usUifGCNumber = (USHORT)pKeyMsg->SEL.INPUT.lData;
						UieNextMenu(aChildAC3EnterType3);                       /* Open Next Function */
						return(SUCCESS);
					}
					UieExit(aACEnter);                                          /* Return to UifACEnter() */
					return(sError);

				 default:
					break;
			}
			/* break; */

		default:
			return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC3EnterKey(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.3 Reset Confirm Mode 
*===============================================================================
*/
SHORT UifAC3EnterKey(KEYMSG *pKeyMsg) 
{
    SHORT   sError;

    switch(pKeyMsg->uchMsg)
	{
		case UIM_INIT:
			UieOpenSequence(aszSeqAC3EnterKey);                             /* Register Key Sequence */
			UifACPGMLDClear();
			MaintDisp(AC_GCFL_RESET_RPT, CLASS_MAINTDSPCTL_10N, 0, 0, uchUifACRptType, 0, usUifGCNumber, LDT_RESET_ADR);    /* "Lead Through for Reset Report"Lead Through Address */
			return(SUCCESS);
                                                                            
		case UIM_INPUT:
			/* Check W/ Amount */
			if (pKeyMsg->SEL.INPUT.uchLen) {                                /* W/ Amount */
				return(LDT_SEQERR_ADR);
			}
        
			switch (pKeyMsg->SEL.INPUT.uchMajor)
			{
				case FSC_AC:
					/* Clear Lead Through */
					UifACPGMLDClear();
					MaintDisp(AC_GCFL_RESET_RPT, 0, 0, 0, uchUifACRptType, 0, 0L, 0);                               /* Disable Lead Through */

					if (uchUifACRptType == ( UCHAR)RPT_TYPE_ALL) {              /* All Reset Type */
						sError = RptGuestResetByGuestCheck (RPT_ALL_RESET, 0);
						RptGuestUnlockByGuestCheck(RPT_ALL_UNLOCK, 0);                      /* Unlock All G/C File */
					} else if (uchUifACRptType == ( UCHAR)RPT_TYPE_IND) {       /* Individual Reset Type */
						sError = RptGuestResetByGuestCheck(RPT_IND_RESET, usUifACRptGcfNo);
						RptGuestUnlockByGuestCheck(RPT_IND_UNLOCK, usUifACRptGcfNo);       /* UnLock Individual G/C File */
					} else {                                                    /* Individual Lock by Waiter Type */
						sError = RptGuestResetByCashier (ulUifACRptCashierNo);
						RptGuestUnlockByCashier(ulUifACRptCashierNo);       /* UnLock Individual G/C File by Waiter */
					}
					UieExit(aACEnter);                                          /* Return to UifACEnter() */
					if (sError == RPT_ABORTED) {
						sError = SUCCESS;
					}
					return(sError);

				case FSC_CANCEL:
					if (uchUifACRptType == ( UCHAR)RPT_TYPE_ALL) {              /* All Reset Type */
						RptGuestUnlockByGuestCheck (RPT_ALL_UNLOCK, 0);
					} else if (uchUifACRptType == ( UCHAR)RPT_TYPE_IND) {                                                        /* Individual Reset Type */
						RptGuestUnlockByGuestCheck (RPT_IND_UNLOCK, usUifACRptGcfNo);
					} else {
						RptGuestUnlockByCashier (ulUifACRptCashierNo);
					}
					UieExit(aACEnter);                                              /* Return to UifACEnter() */
					return(SUCCESS);

				default:
					break;
			}
			/* break; */

		default:
			return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC3EnterData4(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: UifAC3EnterData4()  A/C No.3, option 4 Reset the Guest Check File.
*===============================================================================
*/
SHORT UifAC3EnterData4(KEYMSG *pKeyMsg) 
{
    SHORT   sError;

    switch(pKeyMsg->uchMsg)
	{
		case UIM_INIT:
			UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */
			UifACPGMLDClear();
			MaintDisp(AC_GCFL_RESET_RPT, 0, 0, 0, uchUifACRptType, 0, 0L, LDT_RESET_ADR);  /* "Enter Waiter #" Lead Through Address */
			return(SUCCESS);
                                                                            
		case UIM_INPUT:
			switch (pKeyMsg->SEL.INPUT.uchMajor) {
				case FSC_AC:
					/* Check W/ Amount */
					sError = RptGuestOptimizeFile();
					if (sError == SUCCESS) {
						UieExit(aACEnter);                                          /* Return to UifACEnter() */
						return(SUCCESS);
					}
					UieExit(aACEnter);                                          /* Return to UifACEnter() */
					return(sError);

				 default:
					break;
			}
			/* break; */

		default:
			return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}
