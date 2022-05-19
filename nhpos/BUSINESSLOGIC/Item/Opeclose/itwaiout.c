/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Waiter sign out by waiter No key                         
* Category    : Item Module, NCR 2170 US Hsopitality Model Application         
* Program Name: ITWAIOUT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Gs /Zp /G1s \I                               
* --------------------------------------------------------------------------
* Abstract: ItemWaiSignOut() : waiter sign out module
*           ItemWaiOutDisplay() : dsplay sign out message
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver. Rev. : Name      : Description
* Jul-31-92: 00.00.01  : K.Maeda   : initial  
* Mar-17-94: 00.00.04  : K.You     : del. waiter lock feature.
* Jun-21-95: 03.00.00  : hkato     : R3.0
* Nov-14-95: 03.01.00  : hkato     : R3.1
* Aug-11-99: 03.05.00  : M.Teraki  : Remove WAITER_MODEL
* Apr-19-99: 03.05.00  : M.Teraki  : Merge STORE_RECALL_MODEL
*          :           :           :       /GUEST_CHECK_MODEL
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

#include	<tchar.h>
#include    <memory.h>
#include    <string.h>
#include    <ecr.h>
#include    <pif.h>
#include    <log.h>
#include    <uie.h>
#include    <regstrct.h>
#include    <transact.h>
#include    <uireg.h>
#include    <item.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csstbpar.h>
#include    <cscas.h>
#include    <cswai.h>
#include    <csstbwai.h>
#include    <csstbcas.h>
#include    <csstbstb.h>
#include    <csttl.h>
#include    <csstbttl.h>
#include    <display.h>
#include    <appllog.h>
#include    <csetk.h>
#include    <csstbetk.h>
#include    <cpm.h>
#include    <eept.h>
#include    "itmlocal.h"

#include    <BlFwIf.h>

/*
*===========================================================================
**Synopsis: SHORT   ItemBarSignOut(ITEMOPECLOSE *pItemOpeClose)
*
*    Input: ITEMOPECLOSE *pItemOpeClose
*   Output: Nothing
*    InOut: Nothing
*
**Return: ITM_SUCCESS : Return when the function performed successfully.
*         LDT_SEQERR_ADR : Return when key sequence error detected. 
*         sReturnStatus : Return value when TrnSendTotal() returns error. 
*
** Description: Bartender out function.                        R3.1
*===========================================================================
*/
SHORT   ItemBarSignOut(UIFREGOPECLOSE *pUifRegOpeClose, ITEMOPECLOSE *pItemOpeClose)
{
    SHORT  sReturnStatus;
	ULONG  ulWaiterIdSave = TranModeQualPtr->ulWaiterID;

    /*--- ERROR CHECK ---*/
    if (ulWaiterIdSave == 0) {
        return(LDT_SEQERR_ADR);
    }

    /*--- MAKE ITEMOPECLOSE DATA ---*/
    pItemOpeClose->uchMajorClass = CLASS_ITEMOPECLOSE;
    pItemOpeClose->uchMinorClass = CLASS_BARTENDEROUT;
    pItemOpeClose->uchUniqueAdr = CliReadUAddr();
    pItemOpeClose->ulID = ulWaiterIdSave;

    /*----- CALL TRANSACTION MODULE TO CLOSE -----*/
    TrnClose(pItemOpeClose);                    
    for (;;) {
		CASIF   CasData = {0}; /* V3.3 */

        CasData.ulCashierNo = ulWaiterIdSave;
        /* R3.3 */
        if ((sReturnStatus = CliCasPreSignOut(&CasData)) != CAS_PERFORM ) {
            /* wait for previous cashier sign-out for cashier interrupt */                
            /* UieErrorMsg(CasConvertError(sReturnStatus), UIE_WITHOUT); */
            CliSleep();
            continue;
        } else {
            break;
        }
    }
    for (;;) {
        if ((sReturnStatus = TtlConvertError(TrnSendTotal())) != TRN_SUCCESS) {  /* send sign out info. to trans. module */
            UieErrorMsg(sReturnStatus, UIE_WITHOUT);    /* loop with error message while error condition */
        } else {
            break;
        }
    }

    TrnInitialize(TRANI_ITEM + TRANI_CONSOLI);

    /*----- INITIALIZE MODIFIER LOCAL DATA -----*/
    ItemCasOutInitModLocal();

	// hide/display any group permission windows that should be hidden/revealed at Sign-in.
	// these windows are windows that show or hide based on the group permissions
	// of the operator so we want to hide everything so marked.
	// we first hide everything and then show only those things this operator
	// should be allowed to see.
	BlFwIfGroupWindowHide (0xffffffff);
	if (TranModeQualPtr->ulCashierID) {
		BlFwIfGroupWindowShow (TranModeQualPtr->ulGroupAssociations);  // ItemBarSignOut() so revert to current operator.
	//	BlFwIfWindowTranslate (1);
	}

	if (pUifRegOpeClose->uchMinorClass != CLASS_UIWAITERLOCKOUT) {  /* keep display by server lock, V3.3 */
        ItemWaiOutDisplay();                /* display sign out message */
    }

    return(UIF_WAI_SIGNOUT);
}

/*
*===========================================================================
**Synopsis: VOID   ItemWaiOutDisplay(VOID)
*
*    Input: Nothing
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return: Nothing
* 
** Description: Display sign out message.
*
*===========================================================================
*/
VOID   ItemWaiOutDisplay(VOID)
{
	if(!(flDispRegDescrControl & PM_CNTRL))	//Delayed Balance JHHJ
	{
		flDispRegDescrControl = 0L;
	} else {
		flDispRegDescrControl = 0;
		flDispRegDescrControl |= PM_CNTRL;
	}

	if( !(flDispRegKeepControl & PM_CNTRL))
	{
		flDispRegKeepControl = 0L;
	} else {
		flDispRegDescrControl = 0;
		flDispRegDescrControl |= PM_CNTRL;
	} 

    if (TranModeQualPtr->ulCashierID == 0) {                      /* no cashier sign in */
		UIFREGMODEIN   UifRegModeInData = {0}; 

        /*----- DISPLAY MODE IN MESSAGE -----*/
        UifRegModeInData.uchMajorClass = CLASS_UIFREGMODE; 
        UifRegModeInData.uchMinorClass = CLASS_UIREGMODEIN;
    
        ItemModeInDisplay(&UifRegModeInData);
    } else {                                                     /* cashier still sign in */
		UIFREGOPEOPEN  UifRegOpeOpenData = {0};
		ITEMOPEOPEN    ItemOpeOpenData = {0};

        UifRegOpeOpenData.uchMajorClass = CLASS_UIFREGOPEN; 
        UifRegOpeOpenData.uchMinorClass = CLASS_UICASHIERIN;

        ItemOpeOpenData.uchMajorClass =  CLASS_ITEMOPEOPEN;
        ItemOpeOpenData.uchMinorClass =  CLASS_CASHIERIN;
        _tcsncpy(ItemOpeOpenData.aszMnemonic, TranModeQualPtr->aszCashierMnem, PARA_CASHIER_LEN);
        ItemOpeOpenData.fbCashierStatus = TranModeQualPtr->fbCashierStatus;
        
        ItemSignInDisplay(&UifRegOpeOpenData, &ItemOpeOpenData, 0);  /* display sign in data */
    }
}
/****** End of Source ******/
