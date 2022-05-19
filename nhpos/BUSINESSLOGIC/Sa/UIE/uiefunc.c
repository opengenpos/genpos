/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1995       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Key Seqeunce Module
* Category    : User Interface Enigne, NCR 2170 High Level System Application
* Program Name: UIEFUNC.C
* --------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
*       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
*       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
*       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
*       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
*       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*
* Date       Ver.      Name         Description
* Mar-20-92  00.00.01  O.Nakada     Initial
* May-25-93  C1        O.Nakada     Removed input device control functions.
* Sep-16-93  F1        O.Nakada     Divided the User Interface Engine.
* Mar-22-95  G0        O.Nakada     Added UieModeChange() in UieInitMenu().
* Apr-07-95  G0        O.Nakada     Changed clear retry buffer function in
*                                   UieInitMenu().
* 2171 for Win32
* Aug-26-99  01.00.00  K.Iwata      V1.0 
*** NHPOS 1.4.x, NCR 7448  ***
* Aug-01-02  NHPOS 1.4 R.Chambers   Product donated to Georgia Southern University
*** NHPOS 2.0.x Touchscreen ***
* Mar-01-04  NHPOS 2.0 R.Chambers   Conversion from NCR 7448 to Touchscreen terminals
*** GenPOS 2.2.x Touchscreen ***
* Nov-19-14  GenPOS 2.2 R.Chambers  Source cleanup to remove old, before NCR 7448,
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
*===========================================================================
*   Include File Declarations
*===========================================================================
*/
#include <tchar.h>

#include "ecr.h"
#include "log.h"
#include "pif.h"
#include "uie.h"
#include "uiel.h"
#include "mld.h"
#include "fsc.h"
#include "uiefunc.h"
#include "uieseq.h"
#include "uiedialo.h"


static UCHAR iuchUieMaxFunc = UIE_MAX_FUNC_TBL;    /* Max Function Link Table */

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    VOID DLLENTRY UieInitMenu(UIMENU FAR *pMenu)
*
** Return:      nothing
*
** Description: 
*===========================================================================
*/
VOID  UieQuitAndInitMenu(UIMENU CONST *pMenu)
{
	iuchUieFuncSts = UIE_NORMAL;
	MldForceDeletePopUp();  // Processing Auto and Forced Sign-out so delete any popups displayed.

	UieInitMenu(pMenu);
}

VOID  UieInitMenu(UIMENU CONST *pMenu)
{
    UCHAR     iuchPos;
	UIESUBMSG SubMsg = {0};

    if (iuchUieFuncSts == UIE_NORMAL) {                     /* Normal Function */
        if (UieQuitFunc() == UIE_FUNC_EXIT) {               /* Quit Message Error */
            UieModeChange(UIE_DISABLE);                     /* Mode Unchangable */
            return;
        }

        UieSetModeS2A();                                    /* Set to Application Mode */
        UieSetStatusDrawer();                               /* Set to Drawer Open Flag */

#if defined(PERIPHERAL_DEVICE_WAITER_LOCK)
#pragma message("Support for Waiter Lock.")
        UieSetStatusWaiter();                               /* Reset to Waiter Pen Status */
#endif
        
        UieInitLinkTbl();                                   /* Initialize to Function Link Table */
    }

    iuchPos = UieCheckFuncTbl(MODULE_UIE_INITMENU);         /* Check to Function Link Table */

    UieFuncTbl[iuchPos].pFunction      = pMenu;             /* Set to Function Pointer */
    iuchUieRootFunc[iuchUieFuncSts]    = iuchPos;           /* Set to Root Function */
    iuchUieActiveFunc[iuchUieFuncSts]  = iuchPos;           /* Set to Active Function */
    iuchUieCurrentFunc[iuchUieFuncSts] = iuchPos;           /* Set to Current Function */

    if (iuchUieFuncSts == UIE_NORMAL) {                     /* Normal Function */
        UieSetStatusKeyboard();
    }

    SubMsg.uchMsg = UIM_INIT;                           	/* Set to Message Data */
    SubMsg.usFunc = 0;

    (*UieFuncTbl[iuchPos].pFunction->sFunc)((KEYMSG *)&SubMsg); /* Call to pMenu */

    UieClearRetryBuf();
}


/*
*===========================================================================
** Synopsis:    SHORT UieQuitFunc(VOID)
*
** Return:      Function Loop Status
*               UIE_FUNC_EXIT     - Request to Function Exit
*               UIE_FUNC_CONTINUE - Request to Function Continue    
*
** Description: 
*===========================================================================
*/
SHORT UieQuitFunc(VOID)
{
    UCHAR     iuchIndex;                    /* Temporary Index */
    UCHAR     iuchActive;                   /* Backup Active Function Index */ 
    UCHAR     iuchCurrent;                  /* Backup Current Function Index */
    UIESUBMSG SubMsg = {0};
    
    SubMsg.uchMsg = UIM_QUIT;                       		    /* Quit Message */
    SubMsg.usFunc = 0;

    iuchActive  = iuchUieActiveFunc[UIE_NORMAL];                /* Backup Active Function */
    iuchCurrent = iuchUieCurrentFunc[UIE_NORMAL];               /* Backup Current Function */

    iuchIndex = iuchActive;                                     /* Current Function */

    for (;iuchUieRootFunc[UIE_NORMAL] != UIE_LINK_EMPTY;) {
        iuchUieActiveFunc[UIE_NORMAL]  = iuchIndex;             /* Move to Acitve Function */
        iuchUieCurrentFunc[UIE_NORMAL] = iuchIndex;             /* Move to Current Function */
        
        if ((*UieFuncTbl[iuchIndex].pFunction->sFunc)((KEYMSG *)&SubMsg) != (SHORT)NULL) {  /* Mode Change Error ? */
            iuchUieActiveFunc[UIE_NORMAL]  = iuchActive;        /* Restore Active Function */
            iuchUieCurrentFunc[UIE_NORMAL] = iuchCurrent;       /* Restore Current Function */
            return (UIE_FUNC_EXIT);
        }

        if (iuchIndex == iuchUieRootFunc[UIE_NORMAL]) {         /* Roor Function */
            break;
        }

        iuchIndex = UieFuncTbl[iuchIndex].iuchParent;          /* Move to Parent Function */
    }
    return (UIE_FUNC_CONTINUE);
}


/*
*===========================================================================
** Synopsis:    VOID UieInitLinkTbl(VOID)
*
** Return:      no
*
** Description: 
*===========================================================================
*/
VOID UieInitLinkTbl(VOID)
{
    USHORT iuchPos;

    for (iuchPos = 0; iuchPos < iuchUieMaxFunc; iuchPos++) {       /* All Function Table  */
        UieFuncTbl[iuchPos].iuchParent = UIE_LINK_EMPTY;           /* Clear Parent Index */
        UieFuncTbl[iuchPos].iuchChild  = UIE_LINK_EMPTY;           /* Clear Child Index */
        UieFuncTbl[iuchPos].iuchOBro   = UIE_LINK_EMPTY;           /* Clear Old Brother Index */
        UieFuncTbl[iuchPos].iuchYBro   = UIE_LINK_EMPTY;           /* Clear Young Brother Index */
        UieFuncTbl[iuchPos].pFunction  = NULL;                     /* Clear Function Pointer */
        UieFuncTbl[iuchPos].pSequence  = NULL;                     /* Clear Key Sequence Pointer */
    }

    iuchUieMaxFunc = 0;
}


/*
*===========================================================================
** Synopsis:    UCHAR UieCheckFuncTbl(USHORT usModuleId)
*     Input:    usModuleId - Module ID
*
** Return:      Index to Function Link Table of Empty
*
** Description: 
*===========================================================================
*/
UCHAR UieCheckFuncTbl(USHORT usModuleId)
{
    UCHAR iuchPos;

    for (iuchPos = 0; iuchPos < iuchUieMaxFunc; iuchPos++) {
        if (UieFuncTbl[iuchPos].pFunction == NULL) {           /* not Used */
            return (iuchPos);                               /* Empty Index */
        }
    }

    if (iuchUieMaxFunc < UIE_MAX_FUNC_TBL) {
        iuchUieMaxFunc++;
        return (iuchPos);
    }
    else {
        PifAbort(usModuleId, FAULT_BUFFER_OVERFLOW);
    }
    return (UCHAR)0; /* dummy *//* ### Add (2171 for Win32) V1.0 */
}


/*
*===========================================================================
** Synopsis:    VOID DLLENTRY UieNextMenu(UIMENU FAR *pMenu)
*     Input:    pMenu - Function Menu
*
** Return:      nothing
*
** Description: 
*===========================================================================
*/
VOID  UieNextMenu(UIMENU CONST *pMenu)
{
    UCHAR     iuchPos;                      /* Position of Empty Function Link Table */
    UCHAR     iuchParent;                   /* Parent Function */
    UCHAR     iuchCurrent;                  /* Current Function */
    BOOL      fFirst = UIE_ENABLE;          /* Loop Flag */
	UIESUBMSG SubMsg = {0};

    SubMsg.uchMsg = UIM_INIT;                           	/* Request to Function Initialize */
    SubMsg.usFunc = 0;      

    iuchParent  = iuchUieCurrentFunc[iuchUieFuncSts];          
    iuchCurrent = iuchParent;                               

    if (UieFuncTbl[iuchCurrent].iuchChild != UIE_LINK_EMPTY) {     
        UieDeleteFunc(UieFuncTbl[iuchCurrent].iuchChild, UIE_LINK_EMPTY);   /* Delete to Child Function */
    }

    UieFuncTbl[iuchCurrent].pSequence = NULL;                  /* Delete to Current Sequence */

    while (pMenu->sFunc && pMenu->usName) {                 
        iuchPos = UieCheckFuncTbl(MODULE_UIE_NEXTMENU);     /* Check Function Link Table */         

        if (fFirst == UIE_ENABLE) {                         
            fFirst = UIE_DISABLE;

            UieFuncTbl[iuchCurrent].iuchChild = iuchPos;       /* Set to Child Function */
            UieFuncTbl[iuchPos].iuchParent = iuchParent;       /* Set to Parent Function */
            UieFuncTbl[iuchPos].pFunction  = pMenu;            /* Set to Function Pointer */
        }
        else {
            UieFuncTbl[iuchCurrent].iuchYBro = iuchPos;        /* Set to Young Brother Function */
            UieFuncTbl[iuchPos].iuchParent = iuchParent;       /* Set to Parent Funciton */
            UieFuncTbl[iuchPos].iuchOBro   = iuchCurrent;      /* Set to Old Brother Function */
            UieFuncTbl[iuchPos].pFunction  = pMenu;            /* Set tot Function Pointer */
        }

        iuchUieCurrentFunc[iuchUieFuncSts] = iuchPos;          /* Set to Current Function */

        (*UieFuncTbl[iuchUieCurrentFunc[iuchUieFuncSts]].pFunction->sFunc)((KEYMSG *)&SubMsg);  /* Call to pMenu */

        iuchCurrent = iuchUieCurrentFunc[iuchUieFuncSts];
        ++pMenu;
    }

    iuchUieCurrentFunc[iuchUieFuncSts] = UieFuncTbl[iuchCurrent].iuchParent;

	UieRetry.fchStatus |= UIE_CLEAR_RETRY_BUF;					/* Set to Clear Retry Buffer Flag */	
}


/*
*===========================================================================
** Synopsis:    VOID UieDeleteFunc(UCHAR iuchExit, UCHAR iuchExclude)
*     Input:    iuchExit    - Exit Function Index
*               iuchExclude - Exclude Function Index
*
** Return:      no
*
** Description: 
*===========================================================================
*/
VOID UieDeleteFunc(UCHAR iuchExit, UCHAR iuchExclude)
{
    UCHAR iuchIndex;
    UCHAR iuchCurrent = iuchExit;
    UCHAR iuchPrevious;

    for (;;) {
        iuchIndex = UieFuncTbl[iuchCurrent].iuchYBro;
        if (iuchIndex != UIE_LINK_EMPTY) {
            iuchCurrent = iuchIndex;                                        /* Move to Young Brother Function */
            continue;
        }

        for (;;) {
            iuchIndex = UieFuncTbl[iuchCurrent].iuchChild;
            if (iuchIndex != UIE_LINK_EMPTY && iuchCurrent != iuchExclude) {
                iuchCurrent = iuchIndex;                                    /* Move to Child Function */
                break;
            }

            if (UieFuncTbl[iuchCurrent].iuchOBro != UIE_LINK_EMPTY) {
                iuchPrevious = UieFuncTbl[iuchCurrent].iuchOBro;            /* Backup Previous Index */
                UieFuncTbl[iuchPrevious].iuchYBro = UIE_LINK_EMPTY;         /* Delete Previous Function of Young Brother */
                UieFuncTbl[iuchCurrent].iuchOBro  = UIE_LINK_EMPTY;         /* Delete Current Function of Old Brother */
            }
            else {
                iuchPrevious = UieFuncTbl[iuchCurrent].iuchParent;          /* Backup Previous Index */
                if (iuchCurrent != iuchExclude && iuchPrevious != UIE_LINK_EMPTY) {
                    UieFuncTbl[iuchPrevious].iuchChild = UIE_LINK_EMPTY;    /* Delete Previous Function of Child */
                }
            }

            if (iuchCurrent != iuchExclude) {
                UieFuncTbl[iuchCurrent].iuchParent = UIE_LINK_EMPTY;        /* Delete Current Function of Parent */ 
                UieFuncTbl[iuchCurrent].pFunction  = NULL;                  /* Delete to Current Index of Function */
                UieFuncTbl[iuchCurrent].pSequence  = NULL;                  /* Delete to Current Index of Sequence */
            }
            
            if (iuchCurrent == iuchExit) {                                  /* Exit Function */
                return;
            }

            iuchCurrent = iuchPrevious;                                     /* Move to Current Index */
        }   
    }
}


/*
*===========================================================================
** Synopsis:    VOID DLLENTRY UieAccept(VOID)
*
** Return:      nothing
*
** Description: 
*===========================================================================
*/
VOID  UieAccept(VOID)
{
    UCHAR     iuchCurrent = iuchUieCurrentFunc[iuchUieFuncSts];
	UIESUBMSG SubMsg = {0};

    if (iuchCurrent == iuchUieRootFunc[iuchUieFuncSts]) {  /* Root Function */
        PifAbort(MODULE_UIE_ACCEPT, FAULT_INVALID_DATA);
    }

    SubMsg.uchMsg = UIM_ACCEPTED;                               	/* Set to Message */
    SubMsg.usFunc = UieFuncTbl[iuchCurrent].pFunction->usName;

    iuchCurrent = UieFuncTbl[iuchCurrent].iuchParent;                  /* Move to Parent Function */

    UieDeleteRoot2Crt(&SubMsg, iuchCurrent);
}


/*
*===========================================================================
** Synopsis:    VOID UieDeleteRoot2Crt(UIESUBMSG *SubMsg, UCHAR iuchCurrent)
*     Input:    iuchCurrent - Current Function Index
*
** Return:      nothing
*
** Description: 
*===========================================================================
*/
VOID UieDeleteRoot2Crt(UIESUBMSG *SubMsg, UCHAR iuchCurrent)
{
    UCHAR  iuchDelete  = iuchCurrent;
    UCHAR  iuchExclude;

    UieDeleteFunc(UieFuncTbl[iuchCurrent].iuchChild, UIE_LINK_EMPTY);     /* Delete Child Function */
    
    while (iuchDelete != iuchUieRootFunc[iuchUieFuncSts]) {
        iuchExclude = iuchDelete;
        iuchDelete  = UieFuncTbl[iuchDelete].iuchParent;
        
        UieDeleteFunc(UieFuncTbl[iuchDelete].iuchChild, iuchExclude);

        UieFuncTbl[iuchDelete].iuchChild = iuchExclude;
    }

    iuchUieActiveFunc[iuchUieFuncSts]  = iuchCurrent;
    iuchUieCurrentFunc[iuchUieFuncSts] = iuchCurrent;

    (*UieFuncTbl[iuchCurrent].pFunction->sFunc)((KEYMSG *)SubMsg);

	UieRetry.fchStatus |= UIE_CLEAR_RETRY_BUF;							/* Set to Clear Retry Buffer Flag */	
}


/*
*===========================================================================
** Synopsis:    VOID DLLENTRY UieReject(VOID)
*
** Return:      nothing
*
** Description: 
*===========================================================================
*/
VOID  UieReject(VOID)
{
    UCHAR     iuchCurrent = iuchUieCurrentFunc[iuchUieFuncSts];
	UIESUBMSG SubMsg = {0};
    
    if (iuchCurrent == iuchUieRootFunc[iuchUieFuncSts]) { /* Root Function */
        PifAbort(MODULE_UIE_REJECT, FAULT_INVALID_DATA);
    }

    SubMsg.uchMsg = UIM_REJECT;                                 /* Set to Message */
    SubMsg.usFunc = UieFuncTbl[iuchCurrent].pFunction->usName;
 
    iuchUieCurrentFunc[iuchUieFuncSts] = UieFuncTbl[iuchCurrent].iuchParent;

    (*UieFuncTbl[UieFuncTbl[iuchCurrent].iuchParent].pFunction->sFunc)((KEYMSG *)&SubMsg);

    iuchCurrent = iuchUieCurrentFunc[iuchUieFuncSts] = iuchUieActiveFunc[iuchUieFuncSts];

    SubMsg.uchMsg = UIM_REDISPLAY;                              /* Set to Message */
    SubMsg.usFunc = 0;
 
    (*UieFuncTbl[iuchCurrent].pFunction->sFunc)((KEYMSG *)&SubMsg);

	UieRetry.fchStatus |= UIE_CLEAR_RETRY_BUF;				    /* Set to Clear Retry Buffer Flag */	
}

#if 0
// This function appears to have been added to handle the use of CANCEL key
// with Balance Due during Tender for Amtrak however allowing a CANCEL introduced
// problems with financials balancing however the change in uiftend.c was commented out.
// Comment from Subversion on 10/2/2012 says:
//    allow a balance due transaction to be canceled.  used mainly for
//    returns in which partial payments are made and operator is at a
//    balance due but has no other tender to use.

VOID  UieCancel(VOID)
{
    UCHAR     iuchCurrent = iuchUieCurrentFunc[iuchUieFuncSts];
	UIESUBMSG SubMsg = {0};
    
    if (iuchCurrent == iuchUieRootFunc[iuchUieFuncSts]) { /* Root Function */
        PifAbort(MODULE_UIE_REJECT, FAULT_INVALID_DATA);
    }

    SubMsg.uchMsg = UIM_CANCEL;                                 /* Set to Message */
    SubMsg.usFunc = UieFuncTbl[iuchCurrent].pFunction->usName;
 
    iuchUieCurrentFunc[iuchUieFuncSts] = UieFuncTbl[iuchCurrent].iuchParent;

    (*UieFuncTbl[UieFuncTbl[iuchCurrent].iuchParent].pFunction->sFunc)((KEYMSG *)&SubMsg);

    iuchCurrent = iuchUieCurrentFunc[iuchUieFuncSts] = iuchUieActiveFunc[iuchUieFuncSts];

    SubMsg.uchMsg = UIM_REDISPLAY;                              /* Set to Message */
    SubMsg.usFunc = 0;
 
    (*UieFuncTbl[iuchCurrent].pFunction->sFunc)((KEYMSG *)&SubMsg);

	UieRetry.fchStatus |= UIE_CLEAR_RETRY_BUF;				    /* Set to Clear Retry Buffer Flag */	
}
#endif

/*
*===========================================================================
** Synopsis:    VOID DLLENTRY UieExit(UIMENU FAR *pMenu)
*     Input:    pMenu - Function Menu
*
** Return:      nothing
*
** Description: 
*===========================================================================
*/
VOID  UieExit(UIMENU CONST *pMenu)
{
    UCHAR     iuchCurrent = iuchUieCurrentFunc[iuchUieFuncSts];
    BOOL      fAbort = UIE_ENABLE;
	UIESUBMSG SubMsg = {0};

    SubMsg.uchMsg = UIM_INIT;                                   		/* Set to Message */
    SubMsg.usFunc =	UieFuncTbl[iuchCurrent].pFunction->usName;
 
    while (iuchCurrent != iuchUieRootFunc[iuchUieFuncSts]) {
        iuchCurrent = UieFuncTbl[iuchCurrent].iuchParent;
    
        if (UieFuncTbl[iuchCurrent].pFunction->sFunc == pMenu->sFunc &&
            UieFuncTbl[iuchCurrent].pFunction->usName == pMenu->usName) {
            fAbort = UIE_DISABLE;
            break;
        }
    }
    if (fAbort == UIE_ENABLE) {
        PifAbort(MODULE_UIE_EXIT, FAULT_INVALID_DATA);
    }

    UieDeleteRoot2Crt(&SubMsg, iuchCurrent);
}


/*
*===========================================================================
** Synopsis:    VOID DLLENTRY UieOpenSequence(UISEQ *pszSeq)
*     Input:    pszSeq - Key Seqnece Table
*
** Return:      nothing
*
** Description: 
*===========================================================================
*/
VOID  UieOpenSequence(UISEQ CONST *pszSeq)
{
    UCHAR iuchCurrent = iuchUieCurrentFunc[iuchUieFuncSts];

    if (UieFuncTbl[iuchCurrent].pSequence || UieFuncTbl[iuchCurrent].iuchChild != UIE_LINK_EMPTY) {
        PifAbort(MODULE_UIE_OPENSEQ, FAULT_INVALID_FUNCTION);
    }

    UieFuncTbl[iuchCurrent].pSequence = pszSeq;
}


/*
*===========================================================================
** Synopsis:    VOID DLLENTRY UieCloseSequence(VOID)
*
** Return:      nothing
*
** Description: 
*===========================================================================
*/
VOID  UieCloseSequence(VOID)
{
    UCHAR iuchCurrent = iuchUieCurrentFunc[iuchUieFuncSts];

    if (UieFuncTbl[iuchCurrent].pSequence == NULL) {
        PifAbort(MODULE_UIE_CLOSESEQ, FAULT_INVALID_FUNCTION);
    }

    UieFuncTbl[iuchCurrent].pSequence = NULL;          /* Delete to Key Sequence */
}

/*
*===========================================================================
** Synopsis:    VOID  UieVirtualKeyToPhysicalDev (SHORT usRequiredKeyEventType, KEYMSG *pData)
*
** Return:      nothing
*
** Description: 
*===========================================================================
*/SHORT  UieVirtualKeyToPhysicalDev (SHORT usRequiredKeyEventType, KEYMSG *pData)
{
	SHORT  sResult = -1;

	if (pData->SEL.INPUT.uchMajor != FSC_VIRTUALKEYEVENT) return -2;

	if (usRequiredKeyEventType && pData->SEL.INPUT.DEV.VIRTUALKEY.usKeyEventType != usRequiredKeyEventType) return -3;

	if (pData->SEL.INPUT.DEV.VIRTUALKEY.usKeyEventType == UIE_VIRTUALKEY_EVENT_SCANNER) {
		UIE_SCANNER  tempbuf = {0};
		UCHAR        uchMajor = FSC_SCANNER;

		// In order to handle the label processing for a bar code we need to specify the type of
		// bar code that it is.  See the label processing logic in Rfl\rFLLABEL.c and functions such
		// as RflLabelAnalysis(LABELANALYSIS *pData) and LaMain_PreCheck(LABELANALYSIS *pData)
		tempbuf = pData->SEL.INPUT.DEV.VIRTUALKEY.u.scanner_data;
		memmove (tempbuf.auchStr+1, tempbuf.auchStr, sizeof(tempbuf.auchStr) - sizeof(tempbuf.auchStr[0]));
		if (tempbuf.uchStrLen == 12) {
			// label is LABEL_UPCA with length of LEN_UPCA
			tempbuf.auchStr[0] = _T('A');
			tempbuf.uchStrLen = 13;
		}
		else if (tempbuf.uchStrLen == 14) {
			// label is LABEL_RSS14 with length of LEN_RSS14
			tempbuf.auchStr[0] = _T(']');
			tempbuf.uchStrLen = 15;
		}
		else if (tempbuf.uchStrLen == 8) {
			// label is LABEL_EAN8 with length of LEN_EAN8
			tempbuf.auchStr[0] = _T('B');
			tempbuf.uchStrLen = 9;
		}
		else if (tempbuf.uchStrLen == 6) {
			// label is LABEL_UPCE with length of LEN_UPCE
			tempbuf.auchStr[0] = _T('C');
			tempbuf.uchStrLen = 7;
		}
		else {
			// not a known type of bar code so just put things back.
			memcpy (&tempbuf, &(pData->SEL.INPUT.DEV.VIRTUALKEY.u.scanner_data), sizeof(UIE_SCANNER));
			uchMajor = pData->SEL.INPUT.uchMajor;
		}
		pData->SEL.INPUT.DEV.SCA = tempbuf;
		pData->SEL.INPUT.uchMajor = uchMajor;
		sResult = 0;                             // indicate we did something.
	} else  if (pData->SEL.INPUT.DEV.VIRTUALKEY.usKeyEventType == UIE_VIRTUALKEY_EVENT_MSR) {
		// If this virtual keyboard event is an MSR event then we are going to create
		// an FSC_MSR event using the data from the virtual key event.  Doing this for
		// sign-in and other things that may involve an MSR swipe.
		UIE_MSR     MSR = {0};
		UCHAR       uchMajor = FSC_MSR;

		MSR = pData->SEL.INPUT.DEV.VIRTUALKEY.u.msr_data;
		MSR.uchPaymentType = SWIPE_PAYMENT;

		pData->SEL.INPUT.uchLen = 0;
		pData->SEL.INPUT.DEV.MSR = MSR;
		pData->SEL.INPUT.uchMajor = uchMajor;
		sResult = 0;                             // indicate we did something.
	}

	return sResult;
}

/* ====================================== */
/* ========== End of UIEFUNC.C ========== */
/* ====================================== */
