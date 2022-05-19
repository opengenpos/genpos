/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-2000       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Supervisor Intervention Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIDSPINT.C
* --------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen)
*       2012  -> GenPOS Rel 2.2.0
*       2014  -> GenPOS Rel 2.2.1
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               UifSupIntervent() : Supervisor Intervention Dialog
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data   : Ver.Rev. :   Name     : Description
* May-13-92 : 00.00.01 : M.Suzuki   : initial                                   
* May-21-07 : 02.01.00 : P.Dinu     : Added new Pickup functionality to REG Mode
* Apr-03-15 : 02.02.01 : R.Chambers : Clean up and fix usability problems with Pickup functionality.
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
#include <tchar.h>
#include <string.h>
#include <stdio.h>

#include "ecr.h"
#include "uie.h"
#include "fsc.h"
#include "regstrct.h"
#include "paraequ.h"
#include "para.h"
#include <maint.h>
#include "item.h"
#include "uireg.h"
#include "uiregloc.h"
#include <ej.h>
#include <display.h>
#include <rfl.h>
#include "csstbej.h"
#include "csstbpar.h"
#include "uifsup.h"
#include "uifpgequ.h"
#include "transact.h"
#include "pifmain.h"

SHORT       UifSupInterventRetStatus = UIFREG_ABORT;

#define UISUP_DRAWER_A      0x0001                  /* Indicate A drawer (R2.0) */
#define UISUP_DRAWER_B      0x0002                  /* Indicate B drawer (R2.0) */

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifSupIntervent(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      success = 0
*               cancel  = -2
*
** Description: Reg Supervisor Intervention
*===========================================================================
*/
SHORT UifSupIntervent(VOID)
{
    USHORT  usModeStatus, usCtrlDevice;
    SHORT   sRetStatus = UIF_SUCCESS, sStatusSave;
    USHORT  usStatus = 0;

    UieCreateDialog();                      /* create dialog */
	if(PifSysConfig()->uchKeyType == KEYBOARD_TOUCH){
		usModeStatus = UieModeChange(UIE_DISABLE);
		usCtrlDevice = UieCtrlDevice(UIE_DIS_SCANNER|UIE_DIS_WAITER|UIE_DIS_KEYLOCK);   /* disable waiter lock */
	}else{
		usModeStatus = UieModeChange(UIE_ENABLE);   /* enable mode change */
		usCtrlDevice = UieCtrlDevice(UIE_DIS_SCANNER|UIE_DIS_WAITER);   /* disable waiter lock */
	}

	sStatusSave = ItemOtherDifDisableStatus();      /* R3.1 for Beverage Dispenser */

	if(PifSysConfig()->uchKeyType != KEYBOARD_TOUCH){//#ifndef TOUCHSCREEN
		UieEchoBack(UIE_ECHO_ROW0_REG, 0);      /* set echo back */
	}else{
		UieEchoBack(UIE_ECHO_NO, UIFREG_MAX_ID_DIGITS);
	}

    for (;;) {
		KEYMSG  Data = {0};
        UieGetMessage(&Data, UIEGETMESSAGE_FLAG_MACROPAUSE);    /* wait key entry UifSupIntervent() */
        switch (Data.uchMsg) {
        case UIM_MODE:                      /* mode ? */
            if (usStatus) {                 /* already super mode ? */
                if (Data.SEL.uchMode == UIE_POSITION3) { /* reg mode ? */
                    break;
                }
            } else {
                if (Data.SEL.uchMode == UIE_POSITION4) { /* supervisor mode ? */
                    usStatus = 1;           /* set super mode status */
                }
            }
            continue;
        
        case UIM_INPUT:                     /* key input ? */
			if(PifSysConfig()->uchKeyType == KEYBOARD_TOUCH){
				switch(Data.SEL.INPUT.uchMajor){
				case FSC_RESET_LOG:
				case FSC_POWER_DOWN:
					UifRegDefProc(&Data);
					continue;

				case FSC_CLEAR:                 /* clear key ? */
					UifRedisplaySupInt();
					continue;

				case FSC_CANCEL:                /* Cancel key ? */
					sRetStatus = UIFREG_ABORT;          /* set cancel status */
					break;

				case FSC_P1:                    // P1 key is same as Num Type key (FSC_NUM_TYPE) as used in several places so be consistent
				case FSC_NUM_TYPE:
				case FSC_AC:
					Data.SEL.INPUT.DEV.MSR.uchPaymentType = MANUAL_ENTRY;
				case FSC_MSR:
					{
						UIFDIAEMPLOYEE  UifDiaEmployee;
						SHORT           sSuperCheck = CheckSuperNumber(&Data, &UifDiaEmployee);

						if (sSuperCheck == SUCCESS) {
							const TCHAR     tSupLoginFormat[] = _T("Supr Override: %8d");
							DATE_TIME       DT;                         /* date & time */
							PARASUPLEVEL    ParaSupLevel = {0};
							USHORT          usMaintSupNumber;
							UCHAR			usMaintSupLevel;
							UCHAR           auchPrintBuffer[EJ_WRITE_CALC_BUF_SIZE(2)];         //byte buffer (header data + 2 lines of EJ output)

							memset(auchPrintBuffer, 0, sizeof(EJT_HEADER));
							tcharnset((TCHAR*)&auchPrintBuffer[sizeof(EJT_HEADER)], _T(' '), EJ_COLUMN * 2);

							/* ----- insert success into EJ ----- */
							//added to support US Customs SCER for rel 2.0.0
							//convert string and concatenate supervisor login
							PifGetDateTime(&DT);

							ParaSupLevel.uchMajorClass = CLASS_PARASUPLEVEL;
							ParaSupLevel.uchAddress = 0;
							ParaSupLevel.usSuperNumber = MAINT_SUP_GET_SUPER_NUMBER(UifDiaEmployee.ulSwipeSupervisorNumber);

							usMaintSupNumber = ParaSupLevel.usSuperNumber;

							//we try to find which Supervisor level we are at.
							for( usMaintSupLevel= 0; usMaintSupLevel <= SPNO_LEV1_ADR; usMaintSupLevel++)
							{
								
								ParaSupLevel.uchAddress = usMaintSupLevel;
								ParaSupLevel.usSuperNumber = 0;	 /* initialize supervisor number */                    

    							/* read supervisor level*/
								CliParaSupLevelRead(&ParaSupLevel);
								if(usMaintSupNumber <= ParaSupLevel.usSuperNumber)
								{
									break;
								}
							}
							
							//In P8 Supervisor Level can be 0,1,2
							if (usMaintSupLevel > 0) {
								usMaintSupLevel -= 1;
							}
							//SR 634, Fix for EJ Reporting JHHJ
							//we must set up the time and day information in order for the
							//Information entered into the EJ will allow entries after this
							((EJT_HEADER *)auchPrintBuffer)->usConsecutive = MaintCurrentSpcCo(SPCO_CONSEC_ADR);
							((EJT_HEADER *)auchPrintBuffer)->usCVLI = sizeof(auchPrintBuffer);
							((EJT_HEADER *)auchPrintBuffer)->usDate = DT.usMonth*100 + DT.usMDay;
							((EJT_HEADER *)auchPrintBuffer)->usTime = DT.usHour*100 + DT.usMin;
							((EJT_HEADER *)auchPrintBuffer)->usEjSignature = EJT_HEADER_SIGNATURE;
							RflSPrintf((TCHAR*)&auchPrintBuffer[EJ_WRITE_CALC_BUF_OFFSET(1)], EJ_COLUMN, tSupLoginFormat, UifDiaEmployee.ulSwipeEmployeeId);

							{
								ULONG   ulStatusFlags = 0;
								TCHAR   aszNumber[24];

								if (Data.SEL.INPUT.DEV.MSR.uchPaymentType == SWIPE_PAYMENT) {
									ulStatusFlags |= UIFREGMISC_STATUS_METHOD_SWIPE;
								} else if (Data.SEL.INPUT.DEV.MSR.uchPaymentType == BIOMETRICS_ENTRY) {
									ulStatusFlags |= UIFREGMISC_STATUS_METHOD_BIOMET;
								} else if (Data.SEL.INPUT.DEV.MSR.uchPaymentType == MANUAL_ENTRY) {
									ulStatusFlags |= UIFREGMISC_STATUS_METHOD_MANUAL;
								}

								UifEmployeeSignTimeMethodString (ulStatusFlags, aszNumber);

								RflSPrintf((TCHAR*)&auchPrintBuffer[EJ_WRITE_CALC_BUF_OFFSET(2)], EJ_COLUMN, aszNumber);
							}

							//send supervisor intervention to EJ to be inserted
							sRetStatus = CliEJWrite((EJT_HEADER *)auchPrintBuffer, (EJ_COLUMN * 2 * sizeof(TCHAR)), auchPrintBuffer, MINOR_EJ_NEW, 0);
							if (sRetStatus != EJ_PERFORM) {
								char xBuff[128];
								sprintf (xBuff, "**ERROR - UifSupIntervent(): CliEJWrite() sRetStatus = %d", sRetStatus);
								NHPOS_ASSERT_TEXT((sRetStatus == EJ_PERFORM), xBuff);
							}
							sRetStatus = UIF_SUCCESS;           /* set success status */
						}else{
							UieErrorMsg(LDT_INVALID_SUP_NUM, UIE_WITHOUT);   /* error display */
							sRetStatus = UIFREG_ABORT;          /* set cancel status */
						}
					}
					break;

				case FSC_SUPR_INTRVN:
					continue;

				default:
					UieErrorMsg(LDT_SEQERR_ADR, UIE_WITHOUT);   /* error display */
					UifRedisplaySupInt();

					continue;
				}
				break;
			}else{
				if (Data.SEL.INPUT.uchMajor != FSC_CLEAR) {     /* clear key ? */
            		if (Data.SEL.INPUT.uchMajor == FSC_RESET_LOG	/* avoid key sequence error by PCIF function, 11/12/01 */
							|| Data.SEL.INPUT.uchMajor == FSC_POWER_DOWN) {
                		UifRegDefProc(&Data);
                		continue;
					}

					UieErrorMsg(LDT_SEQERR_ADR, UIE_WITHOUT);   /* error display */
				}
				sRetStatus = UIFREG_ABORT;          /* set cancel status */
				break;
			}

        case UIM_ERROR:                     /* error ? */
            UieErrorMsg(LDT_SEQERR_ADR, UIE_WITHOUT);   /* error display */
			
			//SR 550, we now make supervisor intervention continuous so if the user gets a 
			//sequence error it still waits for the supervisor intervention
			UifRedisplaySupInt();

            /* break */                     /* not use */
			continue;

        case UIM_REDISPLAY:                 /* redisplay ? */
			UifRedisplaySupInt();
            continue;

        default:
            continue;
        }
        break;
    }
    if (sStatusSave) {                  /* if previous status is enable */
        ItemOtherDifEnableStatus(1);    /* R3.1 for Beverage Dispenser */
    }
    UieCtrlDevice(usCtrlDevice);            /* enable waiter lock */
    UieEchoBack(UIE_ECHO_ROW0_REG, UIFREG_MAX_INPUT);  /* set echo back */
    UieModeChange(usModeStatus);            /* restore mode change status */
    UieDeleteDialog();                      /* delete dialog */

	UifSupInterventRetStatus = sRetStatus;     // Save the last status
    return(sRetStatus);
}

/*
*--------------------------------------------------------------------------
**  Synopsis:       SHORT   UifDiaWICMod(VOID)
*      Input:       nothing
*     Output:       nothing
*      INOut:       nothing
*
**  Return:         UIFREG_SUCCESS
*                   UIFREG_CANCEL
*                   UIFREG_CLAER
*
**  Description:    WIC modifier key entry Dialog
*--------------------------------------------------------------------------
*/
SHORT   UifDiaWICMod(VOID)
{
    USHORT  usModeStatus;       /* mode lock enable/disable bit save area */
    USHORT  usCtrlDevice;       /* control device save area       */
    SHORT   sRetStatus;         /* return status save area */
    SHORT   sStatusSave;

    /* --- create a dialog --- */
    UieCreateDialog();

    /* --- save and disable mode change --- */
    usModeStatus = UieModeChange(UIE_DISABLE);

    /* --- disable scanner device --- */
    usCtrlDevice = UieCtrlDevice(UIE_DIS_SCANNER|UIE_DIS_WAITER);
    sStatusSave = ItemOtherDifDisableStatus();      /* R3.1 for Beverage Dispenser */

    /* --- prohibit ten key entry --- */
    UieEchoBack(UIE_ECHO_ROW0_REG, 0);

    for (;;) {
		KEYMSG  Data = {0};               /* key message data */

        /* --- wait key entry --- */
        UieGetMessage(&Data, 0x0000);

        switch (Data.uchMsg) {
        case UIM_INPUT:
            if (Data.SEL.INPUT.uchMajor == FSC_CLEAR) {
                sRetStatus = UIFREG_ABORT;
                break;
            } else if (Data.SEL.INPUT.uchMajor == FSC_WIC_MODIF) {
                sRetStatus = UIF_SUCCESS;
                break;
            } else if (Data.SEL.INPUT.uchMajor == FSC_AUTO_SIGN_OUT) {
                sRetStatus = UIFREG_ABORT;
                break;
            } else if (Data.SEL.INPUT.uchMajor == FSC_RECEIPT_FEED || Data.SEL.INPUT.uchMajor == FSC_JOURNAL_FEED) {
                UifRegDefProc(&Data);
            } else if (Data.SEL.INPUT.uchMajor == FSC_RESET_LOG || Data.SEL.INPUT.uchMajor == FSC_POWER_DOWN) {
				/* avoid key sequence error by PCIF function, 11/12/01 */
                UifRegDefProc(&Data);
            } else {
                UieErrorMsg(LDT_SEQERR_ADR, UIE_WITHOUT);
            }
            /* --- continue for(;;) loop --- */
            continue;

        case UIM_ERROR:
            UieErrorMsg(LDT_SEQERR_ADR, UIE_WITHOUT);

        case UIM_REDISPLAY:
            UieDefProc(&Data);

        default:
            continue;
        }
        break;
    }
    if (sStatusSave) {                  /* if previous status is enable */
        ItemOtherDifEnableStatus(1);    /* R3.1 for Beverage Dispenser */
    }
    /* --- allow ten key entry --- */
    UieEchoBack(UIE_ECHO_ROW0_REG, UIFREG_MAX_INPUT);

    /* --- restore scanner device enable/disable condition --- */
    UieCtrlDevice(usCtrlDevice);

    /* --- restore mode change enable/disable bit --- */
    UieModeChange(usModeStatus);

    /* --- delete a dialog --- */
    UieDeleteDialog();

    /* --- return status --- */
    return(sRetStatus);
}

/*
*--------------------------------------------------------------------------
**  Synopsis:       VOID   UifRedisplaySupInt(VOID)
*      Input:       nothing
*     Output:       nothing
*      INOut:       nothing
*
**  Return:     NOTHING
*
**  Description:    Redisplay the Enter supervisor number leadthrough
*
*	Added for SR 550, so that we can redispaly the information needed if
*	the user enters the wrong data while trying to do a supervisor intervention
*--------------------------------------------------------------------------
*/
VOID UifRedisplaySupInt(VOID)
{
	REGDISPMSG    DisplayData = {0};       

	DisplayData.uchMajorClass = CLASS_REGDISPCOMPLSRY; 
	DisplayData.uchMinorClass = CLASS_REGDISP_SUPERINT;
	RflGetLead (DisplayData.aszMnemonic, LDT_SUPINTR_ADR);
	
	flDispRegDescrControl |= COMPLSRY_CNTRL;  /* turn on descriptor control flag */
	DisplayData.fbSaveControl = 4;            /* display previous data at redisplay */
	DispWrite(&DisplayData);                  /* call display module */
}




/*
*===========================================================================
** Synopsis:    SHORT UifCashPickup(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      success = 0
*               cancel  = -2
*
** Description: Reg Cash Pickup triggered by use of FSC_OPR_PICKUP
*               This function assume it is okay to do a cash pickup and
*               will step the operator through the steps to do so.
*               Before this function is called, the caller should do any
*               kind of Supervisor Intervention or other action needed
*               to validate the Cash Pickup action.
*
*               We are using a state machine to guide the user through the
*               various actions needed to handle a cash or check pickup.
*               The actions needed are:
*                  enter amount of money
*                  enter the tender to pull from
*                  verify action with user
*                  execute action
*===========================================================================
*/
#define  UIF_CASHPICKUP_START    0
#define  UIF_CASHPICKUP_AMOUNT   1
#define  UIF_CASHPICKUP_VERIFY   2
#define  UIF_CASHPICKUP_EXECUTE  3

// these defines must be in this ascending numerical order to work properly
#define  UIF_CASHPICKUP_END      4000
#define  UIF_CASHPICKUP_CLEAR    4001
#define  UIF_CASHPICKUP_ERROR    4002

SHORT UifCashPickup(KEYMSG *Data)
{
	DCURRENCY        lAmount = 0;
	MAINTLOANPICKUP  LoanPickupData = {0};
	TRANMODEQUAL     WorkMode;
    USHORT           usModeStatus, usCtrlDevice;
    SHORT            sRetStatus = SUCCESS, sStatusSave;
    USHORT           usStatus = 0;
	USHORT           usCurrentState = UIF_CASHPICKUP_START;
	USHORT           usModifier = 0;
	UCHAR			 uchSwitch;
	
    UieCreateDialog();                      /* create dialog */
	if(PifSysConfig()->uchKeyType == KEYBOARD_TOUCH){
		usModeStatus = UieModeChange(UIE_DISABLE);
		usCtrlDevice = UieCtrlDevice(UIE_DIS_SCANNER|UIE_DIS_WAITER|UIE_DIS_KEYLOCK);   /* disable waiter lock */
	}else{
		usModeStatus = UieModeChange(UIE_ENABLE);   /* enable mode change */
		usCtrlDevice = UieCtrlDevice(UIE_DIS_SCANNER|UIE_DIS_WAITER);   /* disable waiter lock */
	}

	sStatusSave = ItemOtherDifDisableStatus();      /* R3.1 for Beverage Dispenser */

	TrnGetModeQual( &WorkMode);     /* get mode qualifier data */
	Data->SEL.INPUT.uchMajor = WorkMode.uchCashierDrawer;
	uchSwitch = Data->uchMsg;
 	for (usCurrentState = UIF_CASHPICKUP_START; usCurrentState < UIF_CASHPICKUP_END; ) {
		switch(uchSwitch) {
			case UIM_INPUT:
				if (Data->SEL.INPUT.uchMajor == FSC_CLEAR ||
					Data->SEL.INPUT.uchMajor == FSC_CANCEL)
				{
					sRetStatus = UIFREG_CLEAR;          /* set cancel status */
					usCurrentState = UIF_CASHPICKUP_CLEAR;
				}
				else {
					switch (usCurrentState) {
						case UIF_CASHPICKUP_START:
							usCurrentState = UIF_CASHPICKUP_ERROR;
							if (Data->SEL.INPUT.uchMajor == FSC_SIGN_IN ||
								Data->SEL.INPUT.uchMajor == FSC_B)
							{
								LoanPickupData.uchMajorClass = CLASS_MAINTPICKUP;      /* Set Major Class */
								LoanPickupData.uchMinorClass = CLASS_MAINTCASIN;       /* Set Minor Class */
								LoanPickupData.ulCashierNo = WorkMode.ulCashierID;

								if( Data->SEL.INPUT.uchMajor == FSC_SIGN_IN ) {
									sRetStatus = MaintLoanPickupCasSignIn(&LoanPickupData, UISUP_DRAWER_A);
									if( sRetStatus ) {
										break;
									}
								} else if ( Data->SEL.INPUT.uchMajor == FSC_B ) {
									sRetStatus = MaintLoanPickupCasSignIn(&LoanPickupData, UISUP_DRAWER_B);
									if( sRetStatus ) {
										break;
									}
								} else {
									sRetStatus = MaintLoanPickupCasSignIn(&LoanPickupData, 0);
									if( sRetStatus ) {
										break;
									}
								}

								MaintAllPickupAmountRead(LoanPickupData.ulCashierNo);  /* Read Tender Amount for All Pickup */

								UieEchoBack(UIE_ECHO_ROW0_REG, UIF_DIGIT13); /* Echo Back with Decimal Point */
								MaintDisp(AC_PICKUP,                        /* A/C Number */
										  CLASS_MAINTDSPCTL_AMT,            /* Decimal Point */
										  0,                                /* Page Number */
										  0,                                /* PTD Type */
										  0,                                /* Report Type */
										  0,                                /* Reset Type */
										  0L,                               /* Amount Data */
										  LDT_DATA_ADR);                    /* "Amount Entry" Lead Through Address */
								usCurrentState = UIF_CASHPICKUP_AMOUNT;
								/* --- wait key entry --- */
								UieGetMessage(Data, 0x0000);
							}
							break;

						case UIF_CASHPICKUP_AMOUNT:
							sRetStatus = SUCCESS;
							usCurrentState = UIF_CASHPICKUP_ERROR;
							if (Data->SEL.INPUT.uchMajor == FSC_QTY) {
								/* Check W/ Amount */
								if (Data->SEL.INPUT.uchLen == 0) {                     /* W/o Amount */
									sRetStatus = LDT_DATA_ADR;
								}
								if (Data->SEL.INPUT.uchDec != 0xFF) {                  /* W/ Decimal Point */
									sRetStatus = LDT_KEYOVER_ADR;
								}
								if (Data->SEL.INPUT.uchLen > UIFREG_MAX_DIGIT4) {      /* Over Digit for QTY entry */
									sRetStatus = LDT_KEYOVER_ADR;
								}
								if (sRetStatus == SUCCESS) {
									LoanPickupData.uchMajorClass = CLASS_MAINTPICKUP;      /* Set Major Class */
									LoanPickupData.uchMinorClass = CLASS_MAINTOTHER;       /* Set Minor Class */
									LoanPickupData.usModifier |= MAINT_MODIFIER_QTY;
									LoanPickupData.lAmount = Data->SEL.INPUT.lData;
									MaintLoanPickupHeaderCtl(&LoanPickupData);            /* R/J printer Header */
									MaintLoanPickupDisp(&LoanPickupData);                 /* Display For, Qty */
									usModifier |= MAINT_MODIFIER_QTY;
									lAmount = Data->SEL.INPUT.lData;
									UieGetMessage(Data, 0x0000);
									usCurrentState = UIF_CASHPICKUP_VERIFY;    /* return UifAC10EnterCashier() */
								}
							}
							else if (Data->SEL.INPUT.uchMajor == FSC_VOID)
							{
								if (CliParaMDCCheck(MDC_LOANPICK1_ADR, ODD_MDC_BIT2)) { /* prohibit void */
									sRetStatus = LDT_BLOCKEDBYMDC_ADR;
								}
								if (usModifier & (MAINT_MODIFIER_VOID)){
									sRetStatus = LDT_SEQERR_ADR;
								}

								/* Check W/ Amount */
								if (Data->SEL.INPUT.uchLen) {                       /* W/o Amount */
									sRetStatus = LDT_SEQERR_ADR;
								}

								if (sRetStatus == SUCCESS) {
									LoanPickupData.uchMajorClass = CLASS_MAINTPICKUP;  /* Set Major Class */
									LoanPickupData.uchMinorClass = CLASS_MAINTOTHER;   /* Set Minor Class */
									usModifier |= MAINT_MODIFIER_VOID;
									LoanPickupData.usModifier = usModifier;

									if (Data->SEL.INPUT.uchLen) {                      /* display amount */
										LoanPickupData.lAmount = Data->SEL.INPUT.lData;
									} else {
										LoanPickupData.lAmount = lAmount;
									}
									MaintLoanPickupHeaderCtl(&LoanPickupData);         /* R/J printer Header */
									MaintLoanPickupDisp(&LoanPickupData);              /* Display Void */
									UieGetMessage(Data, 0x0000);
									usCurrentState = UIF_CASHPICKUP_VERIFY;            /* return UifAC10EnterCashier() */
								}
							}
							else if (Data->SEL.INPUT.uchMajor == FSC_TENDER || Data->SEL.INPUT.uchMajor == FSC_FOREIGN)
							{
								usCurrentState = UIF_CASHPICKUP_ERROR;
								if (Data->SEL.INPUT.uchDec != 0xFF) {                  /* W/ Decimal Point */
									sRetStatus = LDT_KEYOVER_ADR;
								}
								if (Data->SEL.INPUT.uchLen > UIFREG_MAX_AMT_DIGITS) {  /* Over Digit for amount entry */
									sRetStatus = LDT_KEYOVER_ADR;
								}
								/* Check W/ Amount for All pick up */

								LoanPickupData.uchLoanPickStatus = 0;
								if (Data->SEL.INPUT.uchLen == 0) {                       /* W/o Amount */
									if(usModifier & (MAINT_MODIFIER_QTY)) {
										sRetStatus = LDT_DATA_ADR;
									}
									if ( !CliParaMDCCheckField (MDC_LOANPICK1_ADR, MDC_PARAM_BIT_D)) { /* prohibit all pickup */
										NHPOS_NONASSERT_NOTE("==PROVISION", "==PROVISION: Pickup all prohibit, MDC 331 Bit D");
										sRetStatus = LDT_BLOCKEDBYMDC_ADR;
									}
								}

								if (sRetStatus == SUCCESS) {
									int i;

									for (i = 0; UifAC1011FscLookupTable[i].nFsc != 0; i++) {
										if (Data->SEL.INPUT.uchMajor == UifAC1011FscLookupTable[i].nFsc &&
											Data->SEL.INPUT.uchMinor == UifAC1011FscLookupTable[i].nVal) {
												LoanPickupData.uchMinorClass = UifAC1011FscLookupTable[i].nMaint;
												break;
										}
									}
									if (UifAC1011FscLookupTable[i].nFsc == 0) {
										sRetStatus = LDT_SEQERR_ADR;
									} else {
										if (Data->SEL.INPUT.uchLen == 0) {                              /* W/o Amount */
											LoanPickupData.uchLoanPickStatus |= MAINT_WITHOUT_DATA;     /* All Pickup */
										}
										LoanPickupData.usModifier = usModifier;
										LoanPickupData.lForQty = lAmount;
										LoanPickupData.lAmount = Data->SEL.INPUT.lData;

										MaintLoanPickupHeaderCtl(&LoanPickupData);    /* R/J printer Header */
										if ((sRetStatus = MaintPickupAmountSet(&LoanPickupData)) == SUCCESS) {
											usCurrentState = UIF_CASHPICKUP_VERIFY;            /* return UifAC10EnterCashier() */
										}
									}
								}
							} else {
								sRetStatus = LDT_SEQERR_ADR;
							}
							break;

						case UIF_CASHPICKUP_ERROR:
							UieErrorMsg(sRetStatus, UIE_WITHOUT);
							MaintDisp(AC_PICKUP,                        /* A/C Number */
									  CLASS_MAINTDSPCTL_AMT,            /* Decimal Point */
									  0,                                /* Page Number */
									  0,                                /* PTD Type */
									  0,                                /* Report Type */
									  0,                                /* Reset Type */
									  0L,                               /* Amount Data */
									  LDT_DATA_ADR);                    /* "Amount Entry" Lead Through Address */
							/* --- wait key entry --- */
							UieGetMessage(Data, 0x0000);
							usCurrentState = UIF_CASHPICKUP_AMOUNT;
							break;

						case UIF_CASHPICKUP_VERIFY:
						case UIF_CASHPICKUP_EXECUTE:
							LoanPickupData.uchMinorClass = CLASS_MAINTCASOUT;
							MaintLoanPickupFinalize(&LoanPickupData);
							MaintFin(CLASS_MAINTTRAILER_PRTSUP);            /* Execute Finalize Procedure */
							UieEchoBack(UIE_ECHO_ROW0_NUMBER, UIF_DIGIT13); /* Start Echo Back and Set Max Input Digit */
							usCurrentState = UIF_CASHPICKUP_END;                                          /* return UifAC10EnterCashier() */
							break;
					}
				}
				break;
		}
	}

    UieCtrlDevice(usCtrlDevice);            /* enable waiter lock */
    UieEchoBack(UIE_ECHO_ROW0_REG, UIFREG_MAX_INPUT);  /* set echo back */
    UieModeChange(usModeStatus);            /* restore mode change status */
    UieDeleteDialog();                      /* delete dialog */

    return(sRetStatus);
}

#undef  UIF_CASHPICKUP_START
#undef  UIF_CASHPICKUP_AMOUNT
#undef  UIF_CASHPICKUP_VERIFY
#undef  UIF_CASHPICKUP_EXECUTE

#undef  UIF_CASHPICKUP_END
#undef  UIF_CASHPICKUP_CLEAR
#undef  UIF_CASHPICKUP_ERROR

/* --- End of Source file --- */