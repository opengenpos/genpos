/*	
	Store and Forward
	User Interface For Supervisor, NCR 2170 US Hospitality Application
*/

#include <tchar.h>
#include <string.h>
#include <ecr.h>
#include <rfl.h>
#include <uie.h>
#include <fsc.h>
#include <paraequ.h>
#include <para.h>
#include <plu.h>
#include <pararam.h>
#include <maint.h>
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <uifsup.h>
#include <uifpgequ.h>
#include <mldmenu.h>
#include "uifsupex.h"                       /* Unique Extern Header for UI */
#include "scf.h"
#include "display.h"
#include "csstbpar.h"
#include <mldsup.h>
#include	"EEPT.H"
#include	"EEptTrans.h"
#include "..\Sa\UIE\UIEERROR.H"
#include <csstbept.h>
#include <csstbopr.h>
#include "appllog.h"


static LONG   lUieAc102_SubMenu = 0;

USHORT  UieAc102_OnArray [] = { 0, PIF_STORE_AND_FORWARD_ONE_ON, PIF_STORE_AND_FORWARD_TWO_ON};
USHORT  UieAc102_ForwardingArray [] = { 0, PIF_STORE_AND_FORWARD_ONE_FORWARDING, PIF_STORE_AND_FORWARD_TWO_FORWARDING};

SHORT UifAC102EnterTypeSubMenu(KEYMSG *pKeyMsg);

/* Define Next Function at UIM_INIT */

//AC 102 menu for Toggle the Store and Forward either on (1) or off (0).
SHORT   UifAC102Toggle (KEYMSG *pKeyMsg)
{
	SHORT             sResultStatus = SUCCESS;
	
	switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
		UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */
		UifACPGMLDClear();

		MaintDisp(AC_STORE_FORWARD,                
			0,                               
			0,                               
			0,                               
			2,                                
			1,                               
			0L,                             
			LDT_STORE_FORWARD_STATUS);                
		
		return(SUCCESS);

	case UIM_INPUT:
		switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            return(LDT_PROHBT_ADR);
			
		default:
			return(LDT_KEYOVER_ADR);
		}
	}
	return(SUCCESS);
}



UIMENU FARCONST aChildAC102Init[] = {{UifAC102EnterTypeSubMenu, CID_AC102ENTERSUBMENU},
                                   {NULL,            0               }};

UIMENU FARCONST aChildAC102SubMenu[] = {{UifAC102EnterType, CID_AC102ENTERTYPE},
                                   {NULL,            0               }};


UIMENU FARCONST aChildAC102EnterType1[] = {{UifAC102Report, CID_AC102ENTERDATA1},
                                         {NULL,             0                }};

UIMENU FARCONST aChildAC102EnterToggleType[] = {{UifAC102Toggle, CID_AC102ENTERDATA1},
                                         {NULL,             0                }};


static  UifDisplayMessage (USHORT usLeadThruNo)
{
	WCHAR  aszMnemonics[PARA_LEADTHRU_LEN + 1] = { 0 };

	RflGetLead(aszMnemonics, usLeadThruNo);

//	BlFwIfPopupMessageWindow (aszMnemonics);
}

SHORT UifAC102Function(KEYMSG *pKeyMsg) 
{
    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
		lUieAc102_SubMenu = 0;
        UieNextMenu(aChildAC102Init);                             /* Open Next Function */
        return(SUCCESS);                                        

    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
}

/*
	Display and process the AC102 subtype menu which is used to determine which host is to be
	managed.  Currently there are two types of hosts, one for Credit/Debit/Gift Card and one for the new
	VCS functionality for Employee Payroll Deduction and E-Meals.
**/
SHORT UifAC102EnterTypeSubMenu(KEYMSG *pKeyMsg) 
{
	SHORT             sResultStatus = 0;	

	switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
		UieOpenSequence(aszSeqACFSC_AC);               /* Register Key Sequence */
		
		UifACPGMLDDispCom(TypeStoreForwardSubMenu);    // Display the first page of the menu tree for AC102

		MaintDisp(AC_STORE_FORWARD,                
			CLASS_MAINTDSPCTL_10N,                               
			0,                               
			0,                               
			0,                                
			0,                               
			0L,                             
			LDT_STORE_FORWARD_STATUS);                
		return(SUCCESS);

	case UIM_INPUT:
		lUieAc102_SubMenu = 0;   // remember which of the different AC102 types this is

		switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
           return(LDT_PROHBT_ADR);

		default:
			return(LDT_KEYOVER_ADR);
		}
	}
	return(SUCCESS);
}


SHORT UifAC102EnterType(KEYMSG *pKeyMsg) 
{
	SHORT             sResultStatus = 0;
	
	switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
		UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */
		
		UifACPGMLDDispCom(TypeStoreForward);

		MaintDisp(AC_STORE_FORWARD,                
			CLASS_MAINTDSPCTL_10N,                               
			0,                               
			0,                               
			0,                                
			0,                               
			0L,                             
			LDT_STORE_FORWARD_STATUS);                
		
		return(SUCCESS);
	case UIM_INPUT:
		switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
           return(LDT_PROHBT_ADR);

		default:
			return(LDT_KEYOVER_ADR);
		}
	}
	return(SUCCESS);
}


//AC 102 menu for reports
SHORT   UifAC102Report(KEYMSG *pKeyMsg){
	SHORT  sResultStatus = SUCCESS;
	switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
		
		UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */
		UifACPGMLDClear();
		//******NEED TO CHANGE TypeStoreReport SO IT USES CORRECT MNEMONICS FOR REPORTS
		UifACPGMLDDispCom(TypeStoreReport);

		MaintDisp(AC_STORE_FORWARD,                
			0,                               
			0,                               
			0,                               
			0,                                
			0,                               
			0L,                             
			LDT_STORE_FORWARD_STATUS);                
		
		return(SUCCESS);
	case UIM_INPUT:
		switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            return(LDT_PROHBT_ADR);
			
		default:
			return(LDT_KEYOVER_ADR);
		}
	}
	return(SUCCESS);
}
