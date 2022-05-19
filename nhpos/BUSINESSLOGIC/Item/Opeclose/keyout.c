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
* Title       : Sign out by mode key position change                         
* Category    : Item Module, NCR 2170 US Hsopitality Model Application         
* Program Name: KEYOUT.C
* --------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
* --------------------------------------------------------------------------
* Abstruct: ItemModeKeyOut()
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver. Rev. : Name      : Description
* May-29-92: 00.00.01  : K.Maeda   : initial                                   
* Jun- 4-92: 00.00.01  : K.Maeda   : Apply comments of code inspection held 
*                                    on Jun. 4                                   
* Jul-31-92: 00.00.01  : K.Maeda   : Apply comments of code inspection held 
*                                    on Jul. 31                                   
* Aug-11-99: 03.05.00  : M.Teraki  : Remove WAITER_MODEL
* Apr-19-99: 03.05.00  : M.Teraki  : Merge STORE_RECALL_MODEL
*          :           :           :       /GUEST_CHECK_MODEL
*
* Apr-02-15:02.02.01    :R.Chambers : GenPOS, ensure transaction data files are open with TrnICPOpenFile().
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
#include    <csstbcas.h>
#include    <csstbstb.h>
#include    <display.h>
#include    <appllog.h>
#include    <csetk.h>
#include    <csstbetk.h>
#include    <cpm.h>
#include    <eept.h>
#include    "itmlocal.h"

/*
*===========================================================================
**Synopsis: SHORT   ItemModeKeyOut(ITEMOPECLOSE *pItemOpeClose)
*
*    Input: nothing
*
*   Output: None
*
*    InOut: None
*
**Return: ITM_SUCCESS : Return when the function performed successfully.
* 
*         LDT_SEQERR_ADR : Return when key sequence error detected. 
*
*         sReturnStatus : Return when TrnClose() returns error. 
*
** Description: Cashier and/ or waiter sign out function by changing Mode lock key position.
*               If a Cashier or waiter is currently signed in when the mode key
*               is changed out of REG Mode then automatically Sign-out the
*               Cashier or Waiter.
*
*               This code was originally for the NCR 2172 or NCR 7448 terminals
*               which had a physical key lock. The key lock had several positions
*               such as REG mode, SUP mode, PROG mode, or LOCKED and different keys
*               would allow a lock position changes to the different modes.  For
*               instance a supervisor key was needed to change the lock position
*               to SUP mode.  The software detected the key lock position change
*               and then changed to the proper mode such as Supervisor Mode with
*               the Supervisor menus if a Supervisor key was used to change the
*               keylock position to SUP mode.
*
*               Beginning with NHPOS Rel 2.0.0 for touch screen terminals we
*               now have a button FSC or Function Selector Code to simulate a
*               key lock position change.  See CDeviceEngine::ConvertKeylockData()
*               as well as use of function BlGetKeyboardLockPosition().
*===========================================================================
*/
SHORT   ItemModeKeyOut(ITEMOPECLOSE *pItemOpeClose)
{
	UIFREGOPECLOSE     UifRegOpeCloseData = {0};
    SHORT              sReturnStatus;

	TrnICPOpenFile ();    // ensure item, cons. post rec. files are open for post receipt

    if (TranModeQualPtr->ulCashierID == 0 && TranModeQualPtr->ulWaiterID == 0) {
        /* close item, cons. post rec. file for post receipt, V3.3 */
        TrnICPCloseFile();
        return(UIF_SUCCESS);                /* pos. changing isn't for sign out */
    }
    
    if (!(CliParaMDCCheck(MDC_SIGNOUT_ADR, ODD_MDC_BIT1))) {   /* MDC add. 19  0: don't sign out */
        return(LDT_PROHBT_ADR);
    }

    if (TranModeQualPtr->ulWaiterID != 0) {  /* waiter still sign in */
        UifRegOpeCloseData.uchMajorClass = CLASS_UIFREGOPECLOSE;          
        UifRegOpeCloseData.uchMinorClass = CLASS_UIBARTENDEROUT;
        if ((sReturnStatus = ItemBarSignOut(&UifRegOpeCloseData, pItemOpeClose)) != UIF_WAI_SIGNOUT) {
            return(sReturnStatus);
        }
    }
    
    if (TranModeQualPtr->ulCashierID != 0) {                                  /* cashier still sign in */
        /* V3.3 */
        if (TranModeQualPtr->fbCashierStatus & MODEQUAL_WAITER_LOCK) {
            UifRegOpeCloseData.uchMinorClass = CLASS_UIWAITERLOCKOUT;
        } else if (TranModeQualPtr->fbCashierStatus & MODEQUAL_CASHIER_B) {   /* waiter still sign in */
            UifRegOpeCloseData.uchMinorClass = CLASS_UIB_OUT;
        } else {  
            UifRegOpeCloseData.uchMinorClass = CLASS_UICASHIEROUT;
        }
        
        sReturnStatus = ItemCasSignOut(&UifRegOpeCloseData, pItemOpeClose);
    }

    /* close item, cons. post rec. file for post receipt, V3.3 */
    TrnICPCloseFile();

    return(sReturnStatus);
}
