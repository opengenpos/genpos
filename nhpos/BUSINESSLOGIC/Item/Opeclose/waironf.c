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
* Program Name: WAIRONF.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: ItemWaiRelOnFinOut()
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver. Rev. : Name      : Description
* Jul-31-92: 00.00.01  : K.Maeda   : initial                                   
* Mar-08-94: 00.00.04  : K.You     : del. waiter lock feature. (mod. ItemWaiRelOnFinOut)
* Jun-21-95: 03.00.00  : hkato     : R3.0
* Oct-06-95: 03.01.00  : hkato     : R3.1
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
#include    <appllog.h>
#include    <uie.h>
#include    <rfl.h>
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
#include    <mld.h>
#include    "itmlocal.h"

/* #ifdef GUEST_CHECK_MODEL */
/*
*===========================================================================
**Synopsis: SHORT   ItemWaiRelOnFinOut(ITEMOPECLOSE *pItemOpeClose)
*
*    Input: ITEMOPECLOSE *pItemOpeClose
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return: UIF_WAI_SIGNOUT : Return when the function performed successfully.
* 
*         LDT_SEQERR_ADR : Return when key sequence error detected. 
*
*         sReturnStatus : Return when TrnClose() returns error. 
*
** Description:                                             R3.1
*===========================================================================
*/
SHORT   ItemWaiRelOnFinOut(ITEMOPECLOSE *pItemOpeClose)
{
    SHORT  sReturnStatus;
    TRANMODEQUAL       * const pModeQualRcvBuff = TrnGetModeQualPtr();
	REGDISPMSG         DispMsg = { 0 }, DispMsgBack = { 0 };
	CASIF              CasData = { 0 };             /* V3.3 */

    memset(pItemOpeClose, '\0', sizeof(ITEMOPECLOSE));
    pItemOpeClose->uchMajorClass = CLASS_ITEMOPECLOSE;

    if (pModeQualRcvBuff->fsModeStatus & MODEQUAL_BARTENDER) {
        pItemOpeClose->uchMinorClass = CLASS_BARTENDEROUT;
    } else {
        pItemOpeClose->uchMinorClass = CLASS_WAITEROUT;
    }
    pItemOpeClose->uchUniqueAdr = CliReadUAddr();
    pItemOpeClose->ulID = pModeQualRcvBuff->ulWaiterID ;

    /*--- CALL TRANSACTION MODULE TO CLOSE ---*/
    TrnClose(pItemOpeClose);                    
    memset( &CasData, 0, sizeof( CASIF ) );
    for (;;) {
        CasData.ulCashierNo = pModeQualRcvBuff->ulWaiterID;
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
        /* send sign out info. to trans. module */
		/* loop with error message while error condition */
        if ((sReturnStatus = TtlConvertError(TrnSendTotal())) != TRN_SUCCESS) {  
            UieErrorMsg(sReturnStatus, UIE_WITHOUT);
        } else {
            break;
        }
    }

    TrnInitialize(TRANI_ITEM + TRANI_CONSOLI);    /* clear strage buffer */

    /*--- DISPLAY SIGN OUT MESSAGE ---*/
    DispMsg.uchMajorClass = CLASS_UIFREGTENDER;
    DispMsg.uchMinorClass = CLASS_UITENDER1;

	if(!(flDispRegDescrControl & PM_CNTRL))	//Delayed Balance JHHJ
	{
		flDispRegDescrControl = 0L;
	}else
	{
		flDispRegDescrControl = 0;
		flDispRegDescrControl |= PM_CNTRL;
	}
	if( !(flDispRegKeepControl & PM_CNTRL))
	{
		flDispRegKeepControl = 0L;
	}else
	{
		flDispRegDescrControl = 0;
		flDispRegDescrControl |= PM_CNTRL;
	}

    if (pModeQualRcvBuff->ulCashierID == 0) { /* no cashier sign in */
		/* 0x2X : don't change foreground     */
        /* 0xX3 : save type 3 for DispMsgBack */
        DispMsg.fbSaveControl = DISP_SAVE_ECHO_ONLY | DISP_SAVE_TYPE_3;
        DispMsgBack.uchMajorClass = CLASS_UIFREGMODE;
        DispMsgBack.uchMinorClass = CLASS_UIREGMODEIN;

		RflGetLead( &DispMsgBack.aszMnemonic[0], LDT_ID_ADR);

        DispMsgBack.fbSaveControl = DISP_SAVE_TYPE_3;      /* save for redisplay (release on finalize) */

    } else {   
		/* 0x2X : don't change foreground     */
        /* 0xX2 : save type 2 for DispMsgBack */
        DispMsg.fbSaveControl = DISP_SAVE_ECHO_ONLY | DISP_SAVE_TYPE_2;
        DispMsgBack.uchMajorClass = CLASS_UIFREGOPEN;
        DispMsgBack.uchMinorClass = CLASS_UICASHIERIN;

        _tcsncpy( &DispMsgBack.aszMnemonic[0], pModeQualRcvBuff->aszCashierMnem, PARA_CASHIER_LEN);

        if (pModeQualRcvBuff->fbCashierStatus & MODEQUAL_CASHIER_TRAINING) {  /* training cashier ? */
            flDispRegDescrControl = TRAIN_CNTRL;
            flDispRegKeepControl = TRAIN_CNTRL;                             /* turn on TRAIN descriptor control flag */
        }
        DispMsgBack.fbSaveControl = DISP_SAVE_TYPE_1;                       /* save for redisplay (release on finalize) */
    }    
        
    DispWriteSpecial(&DispMsg, &DispMsgBack);

    /*--- INITIALIZE MODIFIER LOCAL DATA ---*/
    ItemCasOutInitModLocal();

/*** delete V3.3
    if (ModeQualRcvBuff.usCashierID == 0) { / no cashier sign in /

        / close item, cons. post rec. file /
        TrnICPCloseFile();

        /----- Close Split File, R3.1 -----/
        WorkFlex.uchMajorClass = CLASS_PARAFLEXMEM;
        WorkFlex.uchAddress = FLEX_GC_ADR;
        CliParaRead(&WorkFlex);
        if (WorkFlex.uchPTDFlag == FLEX_POST_CHK
            && !CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2)
            && !CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT3)) {
            TrnCloseFileSpl();
        }

        UieModeChange(UIE_ENABLE);          / make mode changable condition /
    }
****/
    return(UIF_WAI_SIGNOUT);
}
/* #endif */
/****** end of source ******/
