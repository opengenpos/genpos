/*
*===========================================================================
* Title       : Reg default Process Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFRGDEF.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               SHORT UifRegDefProc(KEYMSG *pData)    : Reg Default Process
*				SHORT UifRegString(KEYMSG *pData)
*				SHORT UifRegCallString(USHORT *pusSize)
*				SHORT UifRegPauseString(USHORT *pusSize)
*				UCHAR UifCheckScannerCDV(TCHAR *aszPluNo, SHORT sLen)
*				SHORT UifCheckScannerData(KEYMSG *pData)
*				SHORT UifDiaAlphaNameEntry(KEYMSG *pData)
*				SHORT UifDiaAlphaName(UIFDIADATA *pData)
*				SHORT UifRegSearchKey(UCHAR *pData, UCHAR uchKey, USHORT usLen)
*				VOID  UifRegPauseDisp(UCHAR uchLead)
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date   : Ver.Rev. :   Name      : Description
* May-11-92 : 00.00.01 : M.Suzuki    : initial                                   
* Jun-21-93 : 01.00.12 : K.You       : add direct menu shift.                                   
* Jul-26-93 : 01.00.12 : K.You       : memory compaction.                                   
* Feb-23-95 : 03.00.00 : hkato       : R3.0
* Nov-08-95 : 03.01.00 : hkato       : R3.1
* Aug-07-98 : 03.03.00 : hrkato      : V3.3 (Support Scanner)
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata     : V1.0 Initial
* Jan-20-00 : 01.00.00 : hrkato      : Saratoga
* GenPOS
* Jun-19-15 : 02.02.01 : R.Chambers  : add FSC_P1 processing as if FSC_NUM_TYPE allowing easy keyboard
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
#include <stdlib.h>
#include <stdio.h>

#include "ecr.h"
#include "pif.h"
#include "uie.h"
#include "fsc.h"
#include "cvt.h" 
#include "plu.h" 
#include "pifmain.h"
#include "regstrct.h"
#include "item.h"
#include "paraequ.h"
#include "para.h"
#include "pararam.h"
#include "display.h"
#include "uireg.h"
#include "csop.h"
#include "csstbpar.h"
#include "csstbopr.h"
#include "uiregloc.h"
#include "../sa/uie/uieseq.h"
#include <rfl.h>
#include <cscas.h>
#include <BLFwif.h>
#include <appllog.h>



UIMENU FARCONST aChildRegDiaAlphaName[] = {{UifDiaAlphaNameEntry, CID_DIAALPHAENTRY},
                                          {NULL, 0}};
#if 0
// removed this as used only for the old style Check No Purchase, MDC_CHK_NO_PURCH,
// which introduced problems with #/Type under some circumstances using uchIsNoCheckOn as
// an indicator that #/Type had been entered and Check No Purchase was in effect.
//   Richard Chambers, GenPOS Rel 2.2.1.146, March 10, 2015
UIMENU FARCONST aRegAddCheckTran[] = {{UifAddCheckEntry, CID_ADDCHECKENTRY},    //Modified Menu array for use with the Check No Purchase
                                           {UifItem, CID_ITEM},					//functionality, it has the standard choices that are allowed
                                           {UifSeat, CID_SEATENTRY},			//when you press #/Type only now UifTender is allowed only if the 
                                           {UifNewKeySeqGC, CID_NEWKEYSEQGC},   //MDC bit is chosen for Check No Purchase SR 18 JHHJ 8-21-03 
                                           {UifTransfItem, CID_TRANSFITEM},
                                           {UifGCInformation, CID_GCINFORMATION},
                                           {UifTotal, CID_TOTAL},
                                           {UifChargeTips, CID_CHARGETIPS},
                                           {UifFeedRelease, CID_FEEDRELEASE},
                                           {UifDiscount, CID_DISCOUNT},
                                           {UifPrintOnDemand, CID_PRINTONDEMAND},
                                           {UifPayment, CID_PAYMENT},
                                           {UifCancel, CID_CANCEL},
                                           {UifCursorVoid, CID_CURSORVOID},/* R3.1 */
                                           {UifDispenser, CID_DISPENSER}, /* R3.1 */
											{UifTender, CID_TENDER}, 
										   {NULL, 0}};
#endif

#define UIFREG_NUM_STRING       2048                /* Control Stirng Buffer, R3.0 */

static UCHAR       auchUifString[UIFREG_NUM_STRING];   /* Control String Buffer, R3.0 */
//USHORT       auchUifString[UIFREG_NUM_STRING];   /* Control String Buffer, R3.0 */


/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifRegDefProc(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Default Process Module
*===========================================================================
*/
SHORT UifRegDefProc(KEYMSG *pData)
{
    SHORT              sRetStatus;
	REGDISPMSG         UifDisp = { 0 };
    union {                                /* Reg Data work */
        UIFREGMODIFIER  regmodifier;                /* Modifier */
        UIFREGOTHER     regother;                   /* Other Transaction */
		UIFREGDISC		regdisc;					/* Discount Transaction GSU SR 6 CRW*/
		UIFREGSALES		regitem;			// Item work area US Customs SCER cwunn
    } ItemData;

    memset(&ItemData, 0, sizeof(ItemData));         /* Clear Work */

    switch (pData->uchMsg) {
    case UIM_INPUT:
        switch (pData->SEL.INPUT.uchMajor) {
        case FSC_TAX_MODIF:                         /* tax modifier */
            if (!pData->SEL.INPUT.uchMinor || (pData->SEL.INPUT.uchMinor > FSC_TAXMD_MAX)) { /* check tax # */
                return(LDT_PROHBT_ADR);
            }
            if (pData->SEL.INPUT.uchLen >= NUM_NUMBER) {
                return(LDT_KEYOVER_ADR);
            }

            ItemData.regmodifier.uchMajorClass = CLASS_UIFREGMODIFIER;  /* set tax modifier class */
            ItemData.regmodifier.uchMinorClass =
             (UCHAR)((CLASS_UITAXMODIFIER1 - 1) + pData->SEL.INPUT.uchMinor);
            if (pData->SEL.INPUT.uchLen > 0) {      
                _tcsncpy(ItemData.regmodifier.aszNumber, pData->SEL.INPUT.aszKey, pData->SEL.INPUT.uchLen);   /* set input data */
            }                                                   
            sRetStatus = ItemModTax(&ItemData.regmodifier);     /* Modifier Module */
            if (sRetStatus == UIF_CANCEL) {             /* cancel ? */
                UieReject();                            /* send reject to parent */
                break;
            }
            if (sRetStatus != UIF_SUCCESS) {            /* error ? */
                return(sRetStatus);
            }
            flUifRegStatus |= UIFREG_MODIFIER;      /* set modifier status */
            UieKeepMessage();                       /* keep message */
            break;

        case FSC_VOID:                              /* item void modifier */
            if (uchUifRegVoid != 0) {               /* input item void ? */
                return(LDT_SEQERR_ADR);             /* error */
            }

            if (pData->SEL.INPUT.uchLen >= NUM_NUMBER) {
                return(LDT_KEYOVER_ADR);
            }

			sRetStatus = ItemSalesOrderDeclaration(UifRegData.regsales,0, CLASS_ORDER_DEC_NEW);
			if(sRetStatus != SUCCESS)
			{
				return sRetStatus;
			}

            uchUifRegVoid = 1;                      /* set item void flag */
            flUifRegStatus |= UIFREG_MODIFIER;      /* set modifier status */
            UifDisp.uchMajorClass = CLASS_UIFREGDISP;   /* set display class */
            UifDisp.uchMinorClass = CLASS_UIFDISP_VOID;   
            if (pData->SEL.INPUT.uchLen > 0) {      
                _tcsncpy(UifDisp.aszStringData, pData->SEL.INPUT.aszKey,
                        pData->SEL.INPUT.uchLen);   /* set input data */
            }
            DispWrite(&UifDisp);                        /* display */
            UieKeepMessage();                       /* keep message */
            ItemOtherDifDisableStatus();        /* R3.1 for Beverage Dispenser */
            break;

        case FSC_MENU_SHIFT:                        /* menu shift */
            if ((sRetStatus = UifRegInputCheck(pData, UIFREG_MAX_DIGIT2/*UIFREG_MAX_DIGIT1 RPH 12-2-3 TOUCHSCREEN*/)) != SUCCESS) {  /* check input data */
                return(sRetStatus);
            }
            ItemData.regmodifier.uchMajorClass = CLASS_UIFREGMODIFIER;  /* set tax modifier class */
            ItemData.regmodifier.uchMinorClass = CLASS_UIMENUSHIFT;
            ItemData.regmodifier.uchMenuPage = (UCHAR)pData->SEL.INPUT.lData; /* set input page # */
            return(ItemModMenu(&ItemData.regmodifier));     /* Modifier Module */
                                                            
        case FSC_D_MENU_SHIFT:                      /* direct menu shift */
            if (pData->SEL.INPUT.uchLen != 0) {     /* with amount ? */
                return(LDT_SEQERR_ADR);
            }
            if (pData->SEL.INPUT.uchMinor == 0) {   /* minor FSC is 0 ? */
                return(LDT_PROHBT_ADR);
            }                                                               
            ItemData.regmodifier.uchMajorClass = CLASS_UIFREGMODIFIER;  /* set tax modifier class */
            ItemData.regmodifier.uchMinorClass = CLASS_UIMENUSHIFT;         
            ItemData.regmodifier.uchMenuPage = pData->SEL.INPUT.uchMinor;   /* set input page # */
            return(ItemModMenu(&ItemData.regmodifier));     /* Modifier Module */

        case FSC_P1:                                // P1 key is same as Num Type key (FSC_NUM_TYPE) as used in several places so be consistent
        case FSC_NUM_TYPE:                          /* number */
            if ((sRetStatus = UifRegInputCheckWOAmount(pData, UIFREG_MAX_DIGIT25)) != SUCCESS) {//US Customs SCER cwunn 4/8/03 increased digit #
                return(sRetStatus);
            }
            if (aszUifRegNumber[0] != 0) {         /* already input number ? */
                return(LDT_SEQERR_ADR);
            }

            if (pData->SEL.INPUT.uchLen >= NUM_NUMBER) {
                return(LDT_KEYOVER_ADR);
            }

			sRetStatus = ItemSalesOrderDeclaration(UifRegData.regsales,0, CLASS_ORDER_DEC_NEW);
			if(sRetStatus != SUCCESS)
			{
				return sRetStatus;
			}

            _tcsncpy(aszUifRegNumber, pData->SEL.INPUT.aszKey, STD_NUMBER_LEN);   /* set #/Type text string */
			aszUifRegNumber[STD_NUMBER_LEN] = 0;                                  // ensure terminating end of string.

            UifDisp.uchMajorClass = CLASS_UIFREGDISP;   /* set display class */
            UifDisp.uchMinorClass = CLASS_UIFDISP_NUMBER;   
            _tcsncpy(UifDisp.aszStringData, pData->SEL.INPUT.aszKey, pData->SEL.INPUT.uchLen);       /* set number */
            DispWrite(&UifDisp);                    /* display */
     
			uchIsNoCheckOn = 0;											// default value
			if (CliParaMDCCheck(MDC_CHK_NO_PURCH, ODD_MDC_BIT0))//SR18, if this bit is on, use the 
			{													//menu that allows UifTender to be called 8-18-03
//				UieNextMenu(aRegAddCheckTran);
				uchIsNoCheckOn = 1; // Flag that tells us that #/Type has been pressed for check no purchase 8-18-03 JHHJ
//				fsPrtCompul = 1;    //Print values that need to be set here, because when doing Check no Purchase, they never 
//				fsPrtNoPrtMask = -9;//get set
			}

			return(SUCCESS);
			//following line removed beacuse ItemModNumber returns success immediately, no operations performed
			//US Customs SCER 4/1/03 by cwunn
            /* for kds display, 2172 */
//			ItemData.regmodifier.uchMajorClass = CLASS_UIFREGMODIFIER;  /* set tax modifier class */
//			ItemData.regmodifier.uchMinorClass = CLASS_UIMODNUMBER;
//			_tcsncpy(ItemData.regmodifier.aszNumber, aszUifRegNumber, NUM_NUMBER);
//			flUifRegStatus |= UIFREG_MODIFIER;      /* set modifier status */
//			return(ItemModNumber(&ItemData.regmodifier));     /* Modifier Module */
			break;

        /* --- Scanner, V3.3 --- */
#if 0
        case FSC_SCANNER:
            /* --- Already Input Number ? --- */
            if (aszUifRegNumber[0] != 0) {
                return(LDT_SEQERR_ADR);
            }
            if ((sRetStatus = UifCheckScannerData(pData)) != UIF_SUCCESS) {
                return(sRetStatus);
            }
            flUifRegStatus |= UIFREG_MODIFIER;
            UifDisp.uchMajorClass = CLASS_UIFREGDISP;
            UifDisp.uchMinorClass = CLASS_UIFDISP_NUMBER;
            _tcsncpy(UifDisp.aszStringData, aszUifRegNumber, NUM_ACCT_NUMBER);
            DispWrite(&UifDisp);
            break;
#endif

        case FSC_RECEIPT_FEED:                      /* receipt feed */
        case FSC_JOURNAL_FEED:                      /* journal feed */
            /* ItemData.regother.uchMajorClass = CLASS_UIFREGOTHER;  set receipt/journal feed class */
            if (pData->SEL.INPUT.uchMajor == FSC_RECEIPT_FEED) {
                /* ItemData.regother.uchMinorClass = CLASS_UIRECEIPTFEED; */
                ItemOtherRecFeed();                 /* Other Module */
            } else {
                /* ItemData.regother.uchMinorClass = CLASS_UIJOURNALFEED; */
                ItemOtherJouFeed();                 /* Other Module */
            }
            UieKeepMessage();                       /* keep message */
            return(SUCCESS);

        case FSC_KEYED_STRING:                      /* Keyed Control String Key */
			sRetStatus = ItemSalesOrderDeclaration(UifRegData.regsales,0, CLASS_ORDER_DEC_NEW);
			if(sRetStatus != SUCCESS)
			{
				return sRetStatus;
			}

            sRetStatus = UifRegString(pData);
            if (sRetStatus != UIF_SUCCESS) {
                return(sRetStatus);
            }
            UieKeepMessage();                       /* keep message */
            return(SUCCESS);

        case FSC_STRING:                            /* Control String No Key */
		//	sRet = ItemSalesOrderDeclaration(UifRegData.regsales,0, CLASS_ORDER_DEC_NEW);
		//	if(sRet != SUCCESS)
		//	{
		//		return sRet;
		//	}
		//	Problem identified where the first element of a command string may include
		//	the order declaration and would never be processed by these rules.
		//	Eliminating	this check allows these declaration to happen or the catch to
		//	occur while	the string is being processed as is the desired behavior	--SMurray
            return(UifRegString(pData));

        case FSC_WAIT:
            if (flUifRegStatus & UIFREG_MODIFIER) {           /* modifier status */
                return(LDT_SEQERR_ADR);
            }
            if (pData->SEL.INPUT.uchLen != 0) {     /* with amount ? */
                return(LDT_SEQERR_ADR);
            }
            sRetStatus = ItemOtherWait();
            if (sRetStatus == UIF_CANCEL) {             /* cancel ? */
                UieReject();                            /* send reject to parent */
                break;
            }
            if (sRetStatus != UIF_SUCCESS) {            /* error ? */
                return(sRetStatus);
            }
            break;

        case FSC_PAID_RECALL:
            if (flUifRegStatus & UIFREG_MODIFIER) {           /* modifier status */
                return(LDT_SEQERR_ADR);
            }
            if ((sRetStatus = UifRegInputCheck(pData, UIFREG_MAX_DIGIT2)) != SUCCESS) {
                return(sRetStatus);
            }
            ItemData.regother.uchMajorClass = CLASS_UIFREGOTHER;
            ItemData.regother.uchMinorClass = CLASS_UIPAIDORDERRECALL;
            ItemData.regother.lAmount = pData->SEL.INPUT.lData;
            return(ItemOtherPaidRecall(&ItemData.regother));
                                                            
        case FSC_SCROLL_UP:
            if ((pData->SEL.INPUT.uchLen != 0) ||
                (aszUifRegNumber[0] != 0) ||
                (UifRegData.regsales.lQTY != 0) ||
                (UifRegData.regsales.uchAdjective != 0)) {
                return(LDT_SEQERR_ADR);
            }
            ItemOtherScrollUp();
            return(SUCCESS);

        case FSC_SCROLL_DOWN:
            if ((pData->SEL.INPUT.uchLen != 0) ||
                (aszUifRegNumber[0] != 0) ||
                (UifRegData.regsales.lQTY != 0) ||
                (UifRegData.regsales.uchAdjective != 0)) {
                return(LDT_SEQERR_ADR);
            }
            ItemOtherScrollDown();
            return(SUCCESS);

        case FSC_RIGHT_ARROW:
            if ((pData->SEL.INPUT.uchLen != 0) ||
                (aszUifRegNumber[0] != 0) ||
                (UifRegData.regsales.lQTY != 0) ||
                (UifRegData.regsales.uchAdjective != 0)) {
                return(LDT_SEQERR_ADR);
            }
            sRetStatus = ItemOtherRightArrow();
            return(sRetStatus);

        case FSC_LEFT_ARROW:
            if ((pData->SEL.INPUT.uchLen != 0) ||
                (aszUifRegNumber[0] != 0) ||
                (UifRegData.regsales.lQTY != 0) ||
                (UifRegData.regsales.uchAdjective != 0)) {
                return(LDT_SEQERR_ADR);
            }
            sRetStatus = ItemOtherLeftArrow();
            return(sRetStatus);

        case FSC_LEFT_DISPLAY:
            ItemOtherLeftDisplay();
            UieKeepMessage();
            return(SUCCESS);

        case FSC_WAITER_PEN:                            /* waiter lock, V3.3 */
            if (pData->SEL.INPUT.uchMinor != 0) {       /* insert waiter lock ? */
                UifRegData.regopeopen.uchMajorClass = CLASS_UIFREGOPEN;  /* set cashier sign in class */
                UifRegData.regopeopen.uchMinorClass = CLASS_UIWAITERLOCKIN;
                UifRegData.regopeopen.ulID = (ULONG)pData->SEL.INPUT.uchMinor; /* set cashier number */
                sRetStatus = ItemSignIn(&UifRegData.regopeopen);    /* Operator Sign In Modele */
                UifRegWorkClear();                              /* clear work area */
                if (sRetStatus != UIF_SUCCESS) {            /* error ? */
                    if (sRetStatus == UIF_CANCEL) {             /* cancel ? */
                        UieReject();                            /* send reject to parent */
                        return (SUCCESS);
                    }
                    return(sRetStatus);
                }
                UieEchoBack(UIE_ECHO_ROW0_REG, UIFREG_MAX_INPUT);  /* set echo back */
                UieAccept();                                /* send accepted to parent */

            } else {                                                /* remove waiter lock */

                UifRegData.regopeclose.uchMajorClass = CLASS_UIFREGOPECLOSE;  /* set waiter sign out class */
                UifRegData.regopeclose.uchMinorClass = CLASS_UIWAITERLOCKOUT;
                sRetStatus = ItemSignOut(&UifRegData.regopeclose);    /* Operator Sign Out Modele */
                UifRegWorkClear();                              /* clear work area */
                if (sRetStatus == UIF_CAS_SIGNOUT) { 
                    flUifRegStatus |= UIFREG_CASHIERSIGNOUT;    /* set cashier sign out status */
                    flUifRegStatus |= UIFREG_WAITERSIGNOUT;     /* set waiter sign out status simultaniously */
                } else if (sRetStatus == UIF_CANCEL) {          /* error ? */
                    UieReject();                                /* send reject to parent */
                    return(sRetStatus);
                } else {
                    return(sRetStatus);
                }
				BlFwIfDefaultUserScreen();				/* call function to bring up default window(s) */
                UieAccept();                                /* send accepted to parent */
            }
            break;

        /* --- PC i/F Reset Log,    V3.3 --- */
        case FSC_RESET_LOG:            // in UifRegDefProc(): call ItemOtherResetLog() to issue PCIF reset log.  see IspWriteResetLog()
            if (uchResetLog != 0) {
                if (ItemOtherResetLog(uchResetLog) == UIF_SUCCESS) {
                    uchResetLog = 0;
                }
            }
            UieKeepMessage();
            break;

        /* --- Print Power Down Log,    Saratoga --- */
        case FSC_POWER_DOWN:
            if (uchPowerDownLog != 0) {
                uchPowerDownLog = pData->SEL.INPUT.uchMinor;
                if (ItemOtherPowerDownLog(uchPowerDownLog) == 0) {
                    uchPowerDownLog = 0;
                }
            }
            UieKeepMessage();
            break;

        case FSC_ADJ_SHIFT:                      /* adjective shift, 2172 */
            if (pData->SEL.INPUT.uchLen != 0) {     /* with amount ? */
                return(LDT_SEQERR_ADR);
            }
            if (pData->SEL.INPUT.uchMinor == 0) {   /* minor FSC is 0 ? */
                return(LDT_PROHBT_ADR);
            }                                                               
            ItemData.regmodifier.uchMajorClass = CLASS_UIFREGMODIFIER;  /* set tax modifier class */
            ItemData.regmodifier.uchMinorClass = CLASS_UIADJSHIFT;         
            ItemData.regmodifier.uchMenuPage = pData->SEL.INPUT.uchMinor;   /* set input page # */
            return(ItemModAdjMenu(&ItemData.regmodifier));     /* Modifier Module */

        case FSC_FOODSTAMP:                         /* food stamp modifier */
            if (pData->SEL.INPUT.uchMinor != 0) {   /* minor FSC is 0 ? */
                return(LDT_SEQERR_ADR);
            }

            if (pData->SEL.INPUT.uchLen >= NUM_NUMBER) {
                return(LDT_KEYOVER_ADR);
            }

            ItemData.regmodifier.uchMajorClass = CLASS_UIFREGMODIFIER;
            ItemData.regmodifier.uchMinorClass = CLASS_UIFDISP_FSMOD;
            if (pData->SEL.INPUT.uchLen > 0) {      
                _tcsncpy(ItemData.regmodifier.aszNumber, pData->SEL.INPUT.aszKey, pData->SEL.INPUT.uchLen);
            }
            sRetStatus = ItemModFS(&ItemData.regmodifier);
            if (sRetStatus == UIF_CANCEL) {
                UieReject();
                break;
            }
            if (sRetStatus != UIF_SUCCESS) {
                return(sRetStatus);
            }
            flUifRegStatus |= UIFREG_MODIFIER;
            UieKeepMessage();
            break;

		//SR 219, when cancelling an Item that is age restricted AND that item is the
		//first item rang up, when you hit cancel, it does not clear the ItemSalesLocal 
		// value of the age.  When you try to ring up the next person in the line, the age
		// value would still remain as the age for the person before.
		case FSC_CANCEL:
			ItemMiscAgeCancel();
            return(UieDefProc(pData));                  /* default process */

		case FSC_AUTO_SIGN_OUT:
			sRetStatus = CasForceAutoSignOut(0);
            return(UieDefProc(pData));                  /* default process */

		case FSC_DOC_LAUNCH:
			// Handle the FSC to perform a document launch action.
			// We issue the tone with Supervisor Intervention and then call
			// use one of the spawn primitives to spawn a new processs.
			// We require the system setup to have put a link or shortcut
			// that is linked to the document or application to launch.
			// The shortcut is put into the Database folder allowing people to
			// download and update links.
			sRetStatus = UieShelExecuteLinkFile (pData->SEL.INPUT.aszKey, pData->SEL.INPUT.uchLen);

			// Clear the leadthru area as we are done.
			RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGOTHER, CLASS_UICLEAR, DISP_SAVE_TYPE_0);

			// just return rather than doing any kind of UIE menu tree action or cleanup.
			// the document launch is basically transparent to the UIE functionality.
			return UIE_SUC_KEY_TRANSPARENT;
			break;

		case FSC_WINDOW_DISPLAY:
			sRetStatus = UIE_SUC_KEY_TRANSPARENT;
			if (pData->SEL.INPUT.uchLen < 1) {
				sRetStatus = LDT_SEQERR_ADR;
			}
			else {
				if (pData->SEL.INPUT.uchLen > 4 && pData->SEL.INPUT.aszKey[0] == _T('9')) {
					BlFwIfPopupWindowGroup(pData->SEL.INPUT.aszKey+4);	//the sequence of digits previous to this button press should be the window we want to popup
				} else {
					BlFwIfPopupWindowPrefix(pData->SEL.INPUT.aszKey);	//the sequence of digits previous to this button press should be the window we want to popup
				}
			}

			// Clear the leadthru area as we are done.
			RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGOTHER, CLASS_UICLEAR, DISP_SAVE_TYPE_0);

			// just return rather than doing any kind of UIE menu tree action or cleanup.
			// the window display is basically transparent to the UIE functionality.
			return(sRetStatus);
			break;

		case FSC_WINDOW_DISMISS:
			sRetStatus = UIE_SUC_KEY_TRANSPARENT;
			if (pData->SEL.INPUT.uchLen < 1) {
				sRetStatus = LDT_SEQERR_ADR;
			}
			else {
				if (pData->SEL.INPUT.uchLen > 4 && pData->SEL.INPUT.aszKey[0] == _T('9')) {
					BlFwIfPopdownWindowGroup(pData->SEL.INPUT.aszKey+4);	//the sequence of digits previous to this button press should be the window we want to popup
				} else {
					BlFwIfPopdownWindowPrefix(pData->SEL.INPUT.aszKey);
				}
			}

			RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGOTHER, CLASS_UICLEAR, DISP_SAVE_TYPE_0);

			// just return rather than doing any kind of UIE menu tree action or cleanup.
			// the window display is basically transparent to the UIE functionality.
			return(sRetStatus);
			break;

        default:                                        /* not use */
            return(UieDefProc(pData));                  /* default process */
        }
        break; // end of UIM_INPUT case statement

    case UIM_CANCEL:
        memset(&UifRegData, 0, sizeof(UifRegData));     /* Clear UI Work */
        memset(&UifRegDiaWork, 0, sizeof(UifRegDiaWork));
        /*return(sRetStatus);*/
		//SR 143 HALO Override cwunn Reset status variable for RflHALO
		RflSetHALOStatus(0x00);
        return(SUCCESS); /* ### Mod (2171 for Win32) V1.0 */

    case UIM_REDISPLAY:
        if (pData->SEL.usFunc == 0) {               /* w/o amount ? */
            if (!(flUifRegStatus & UIFREG_MODIFIER)) {  /* already input modifier ? */
                return(UieDefProc(pData));          /* default process */
            }
        }
        /* ItemData.regother.uchMajorClass = CLASS_UIFREGOTHER;  set clear key class 
        ItemData.regother.uchMinorClass = CLASS_UICLEAR; */
        ItemOtherClear();                               /* Other Module */
        UifRegWorkClear();                              /* clear work area */
        ItemOtherDifRewriteRing();                      /* rewrite 6400 data to ring buffer R3.1 */
        if ((flUifRegStatus & (UIFREG_FINALIZE | UIFREG_BUFFERFULL)) == 0) {
            ItemOtherDifEnableStatus(1);        /* R3.1 for Beverage Dispenser */
        }
        return(SUCCESS);

    case UIM_QUIT:
        break;

    default:                                        /* not use */
        return(UieDefProc(pData));                  /* default process */
    }
    return (SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT UifRegString(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Control String,        R3.0
*===========================================================================
*/
SHORT   UifRegString(KEYMSG *pData)
{
    SHORT   sStatus;
    USHORT  usSize = 0;
	CSTRIF  WorkCstr = { 0 };

	if (UieMacroRunning()) {
		NHPOS_ASSERT_TEXT((!UieMacroRunning()), "==WARNING: UifRegString() - Control string start ignored.");
		return SUCCESS;
	}

    memset(auchUifString, 0, sizeof(auchUifString));

    if (pData->SEL.INPUT.uchMajor == FSC_STRING) {
        WorkCstr.usCstrNo = (USHORT)pData->SEL.INPUT.lData;
    } else {
        WorkCstr.usCstrNo = pData->SEL.INPUT.uchMinor;
    }

    /*----- Get String Parameter -----*/
    if ((sStatus = CliOpCstrIndRead(&WorkCstr, 0)) != OP_PERFORM) {
		char  xBuff [128];
		sprintf (xBuff, "LOG_EVENT_STRING_LOOKUP_ERROR CliOpCstrIndRead() returned %d, usCstrNo = %d", sStatus, WorkCstr.usCstrNo);
		NHPOS_ASSERT_TEXT((CliOpCstrIndRead(&WorkCstr, 0) == OP_PERFORM), xBuff);
		PifLog (MODULE_UIC_LOG, LOG_EVENT_STRING_LOOKUP_ERROR);
		PifLog (MODULE_DATA_VALUE(MODULE_UIC_LOG), WorkCstr.usCstrNo);
		PifLog (MODULE_ERROR_NO(MODULE_UIC_LOG), (USHORT)abs(sStatus));
        return(LDT_KEYOVER_ADR);
    } else {
        if (WorkCstr.IndexCstr.usCstrLength == 0) {
			char  xBuff [128];
			sprintf (xBuff, "LOG_EVENT_STRING_EMPTY_ERROR usCstrNo = %d", WorkCstr.usCstrNo);
			NHPOS_ASSERT_TEXT((WorkCstr.IndexCstr.usCstrLength != 0), xBuff);
			PifLog (MODULE_UIC_LOG, LOG_EVENT_STRING_EMPTY_ERROR);
			PifLog (MODULE_DATA_VALUE(MODULE_UIC_LOG), WorkCstr.usCstrNo);
			PifLog (MODULE_DATA_VALUE(MODULE_UIC_LOG), WorkCstr.IndexCstr.usCstrOffset);
			PifLog (MODULE_DATA_VALUE(MODULE_UIC_LOG), WorkCstr.IndexCstr.usCstrLength);
            return(LDT_PROHBT_ADR);
        }
    }

	NHPOS_ASSERT( sizeof(auchUifString) >= WorkCstr.IndexCstr.usCstrLength);

	memcpy(auchUifString, WorkCstr.ParaCstr, WorkCstr.IndexCstr.usCstrLength);

    usSize += WorkCstr.IndexCstr.usCstrLength;

#if 0
	// this call string evaluation step removed so as to delay the evaluation of
	// an called control string.  This delay allows for a control string that
	// calls another control string to abort before any pause messages in the
	// called control string are executed.  previous to this change since all
	// of the called control strings were first concatenated together and then
	// all of the pause messages executed and then the control string actually
	// executed, if a step in the control string before the first pause message
	// aborted, the user still would end up having to do data entry.
	// this change requested by Amtrak who is using some fairly involved
	// control strings.  see use of function UieAddToEndMacro() to perform the
	// delayed evaluation once an FSC_CALL_STRING is found during initial
	// control string execution. Aug-09-2013, GenPOS Rel 2.2.1
    /*----- Call Control String -----*/
    if ((sStatus = UifRegCallString(&usSize)) < 0) {
		PifLog (MODULE_UIC_LOG, LOG_EVENT_STRING_CALL_ERROR);
		PifLog (MODULE_DATA_VALUE(MODULE_UIC_LOG), WorkCstr.usCstrNo);
		PifLog (MODULE_ERROR_NO(MODULE_UIC_LOG), (USHORT)abs(sStatus));
        return(LDT_KEYOVER_ADR);
    }
#endif

    /*----- Pause Control String -----*/
    sStatus = UifRegPauseString(&usSize, 0);
    if (sStatus == -1) {
        ItemOtherClear();
        UifRegWorkClear();
        return(SUCCESS);
    } else if (sStatus != 0) {
		PifLog (MODULE_UIC_LOG, LOG_EVENT_STRING_PAUSE_ERROR);
		PifLog (MODULE_DATA_VALUE(MODULE_UIC_LOG), WorkCstr.usCstrNo);
		PifLog (MODULE_ERROR_NO(MODULE_UIC_LOG), (USHORT)abs(sStatus));
        return(LDT_KEYOVER_ADR);
    }

    /*----- Key Sequence -----*/
    UieSetMacro(WorkCstr.usCstrNo, (CONST FSCTBL *)auchUifString, (USHORT)(usSize / sizeof(FSCTBL)));

    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT UifRegCallString(USHORT *pusSize)
*     Input:    
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*
** Description: Edit Control String Data by processing call control string and
*               replacing the FSC_CALL_STRING in the control string with the
*               actual text from the called control string.
*
*               This control string evaluation step merges a series of
*               several control strings into a single control string to be
*               executed.
*===========================================================================
*/
SHORT   UifRegCallString(USHORT *pusSize)
{
    SHORT   sStatus, i;
	CSTRIF  WorkCstr = { 0 };

    for (i = 0; i < OP_CSTR_PARA_SIZE; i++) {
        if ((sStatus = UifRegSearchKey(auchUifString, FSC_CALL_STRING, *pusSize)) >= 0) {
			SHORT   sTempStatus;
            /*----- Get String Parameter -----*/
            WorkCstr.usCstrNo = auchUifString[sStatus + 1];
            if ((sTempStatus = CliOpCstrIndRead(&WorkCstr, 0)) != OP_PERFORM) {
				PifLog (MODULE_UIC_LOG, LOG_EVENT_STRING_LOOKUP_ERROR);
				PifLog (MODULE_DATA_VALUE(MODULE_UIC_LOG), WorkCstr.usCstrNo);
				PifLog (MODULE_ERROR_NO(MODULE_UIC_LOG), (USHORT)abs(sTempStatus));
				PifLog (MODULE_LINE_NO(MODULE_UIC_LOG), (USHORT)__LINE__);
                return(-1);
            }
            if ((*pusSize + WorkCstr.IndexCstr.usCstrLength) > UIFREG_NUM_STRING) {
				char xBuff[128];
				sprintf (xBuff, "**ERROR: UifRegCallString() call to %d exceeds UIFREG_NUM_STRING.", WorkCstr.usCstrNo);
				NHPOS_ASSERT_TEXT(0, xBuff);
                return(-2);
            }
            memmove(&auchUifString[sStatus] + WorkCstr.IndexCstr.usCstrLength, &auchUifString[sStatus + 2], (SHORT)(*pusSize - sStatus - 2));
            memcpy(&auchUifString[sStatus], WorkCstr.ParaCstr, WorkCstr.IndexCstr.usCstrLength);
            *pusSize += WorkCstr.IndexCstr.usCstrLength - 2;
            continue;
        } else {
            return(0);
        }
    }
    return(-1);
}

/*
*===========================================================================
** Synopsis:    SHORT   UifRegPauseString(USHORT *pusSize)
*     Input:    
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*
** Description: Edit Control String Data by processing pause messages and
*               replacing the FSC_PAUSE with the actual text entered by the
*               user.  This is called during control string evaluation so
*               as to allow user input to modify what a control string does.
*
*               Flags are used to control some behavior of the pause message evaluation.
*                 0x0001 - indicates do not clear existing pause message text
*===========================================================================
*/
USHORT  UifRegPauseStringNumber = 0;

SHORT   UifRegPauseString(USHORT *pusSize, ULONG  ulFlags)  // FSC_PAUSE, handle pause key data entry
{
	static TCHAR   auchWork[20][(UIFREG_MAX_DIGIT25*2)+1];  // 20 Possible Pause Strings, Each one can have 25 values. Each value has a major and minor class
	static USHORT  ausSize[20];			 
    static CHAR    auchPause[20];
	extern USHORT  UifRegPauseStringNumber;
    UCHAR   *puchPtr;
    SHORT   i = 0, j = 0, sStatus;			 
	SHORT   sDiaStatus;
	USHORT  usSize;			 

	if ((ulFlags & 0x0001) == 0) {
		// clear the pause message working data so that only new
		// data will be used in the control string.
		memset(auchWork, 0, sizeof(auchWork));
		memset(ausSize, 0, sizeof(ausSize));
		memset(auchPause, 0, sizeof(auchPause));
	}

	UifRegPauseStringNumber = 0;

    for (;;) {
        if ((sStatus = UifRegSearchKey(auchUifString, FSC_PAUSE, *pusSize)) >= 0) {
			REGDISPMSG      DispMsg;

            if ((puchPtr = strchr(auchPause, auchUifString[sStatus + 1])) == 0L) {
				UIFDIADATA  WorkUI = { 0 };

                if (iuchUieFuncSts != 0) {  /* 5/17/2000 TAR137170 */
                    return (-2);
                }

				// if this is an event type of pause then skip it
                if (auchUifString[sStatus + 1] > 20) {
					auchUifString[sStatus] = FSC_CNTRL_STRING_EVENT;  // if this FSC_PAUSE is really a FSC_CNTRL_STRING_EVENT then replace the opcode
					continue;
				}

                auchPause[j] = (CHAR) auchUifString[sStatus + 1]; //ESMITH

                UifRegPauseDisp(auchUifString[sStatus + 1]);
				//if(alphanumeric pause mesage) US Customs SCER cwunn (additional messages are offset from the original group)
                if (((auchPause[j] > 5) && (auchPause[j] <= 10) ) || (auchPause[j] > 15) ){  // LDT_PAUSEMSG5_ADR, LDT_PAUSEMSG10_ADR, LDT_PAUSEMSG15_ADR

                    memset(&DispMsg, 0, sizeof(REGDISPMSG));
                    DispMsg.uchMajorClass = CLASS_REGDISPCOMPLSRY;
                    DispMsg.uchMinorClass = CLASS_REGDISP_PAUSE;
                    RflGetPauseMsg (DispMsg.aszMnemonic, auchPause[j]);

                    DispMsg.fbSaveControl = DISP_SAVE_TYPE_4;
                    DispMsg.uchFsc = auchPause[j];  /* for 10N10D */
                    flDispRegDescrControl |= (COMPLSRY_CNTRL | CRED_CNTRL);
                    flDispRegKeepControl  |= (COMPLSRY_CNTRL | CRED_CNTRL);
                    DispWrite(&DispMsg);
                    flDispRegDescrControl &= ~COMPLSRY_CNTRL;
                    flDispRegKeepControl  &= ~COMPLSRY_CNTRL;

					UifRegPauseStringNumber = auchPause[j];    // remember the last pause key entry

					if (auchPause[j] == 20) {    // LDT_PAUSEMSG20_ADR special case for asterisk substitution
						// We are using alphanumeric pause #20 as a special case to not echo data entry
						// We are doing this for Amtrak to allow PIN entry.
						sDiaStatus = UifDiaAlphaNameNoEcho(&WorkUI, UIE_ECHO_NO);
					} else {
						sDiaStatus = UifDiaAlphaName(&WorkUI);
					}
                    if (sDiaStatus != UIF_SUCCESS) {
                        memset(&DispMsg, 0, sizeof(REGDISPMSG));
                        DispMsg.uchMajorClass = CLASS_UIFREGMODIFIER;               /* dummy */
                        DispMsg.uchMinorClass = CLASS_UITABLENO;                    /* dummy */
                        DispMsg.lAmount = 0L;                                       /* dummy */
                        DispMsg.aszMnemonic[0] = 0;                                 /* dummy */
                        DispMsg.fbSaveControl = DISP_SAVE_ECHO_ONLY;
                        DispWrite(&DispMsg);
                                                
                        flDispRegDescrControl &= ~CRED_CNTRL; 
                        flDispRegKeepControl  &= ~CRED_CNTRL; 
						RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGOTHER, CLASS_UICLEAR, DISP_SAVE_TYPE_0);
						return(-1);
                    }

					memset(&DispMsg, 0, sizeof(REGDISPMSG));
                    DispMsg.uchMajorClass = CLASS_UIFREGMODIFIER;
                    DispMsg.uchMinorClass = CLASS_UIALPHANAME;
                    flDispRegDescrControl &= ~TAXEXMPT_CNTRL;
                    flDispRegKeepControl  &= ~TAXEXMPT_CNTRL;
                    flDispRegDescrControl &= ~CRED_CNTRL; 
                    flDispRegKeepControl  &= ~CRED_CNTRL; 
                    DispMsg.fbSaveControl  = DISP_SAVE_TYPE_0;
                    DispWrite(&DispMsg);

                } else { //pause message is numeric only
					SHORT   sDiaStatus;
					USHORT  usUieCtrlDevicePrevious = 0;

					UieCtrlDevice(UIE_ENA_MSR);                     /* enable MSR,  R3.1 */
					UieCtrlDevice(UIE_ENA_VIRTUALKEY);              // enable Virtual Keyboard such as FreedomPay
					UieCtrlDevice(UIE_ENA_SCANNER);                 // enable scanner

					if (auchPause[j] == 1 || auchPause[j] == 2) {            // LDT_PAUSEMSG1_ADR or LDT_PAUSEMSG2_ADR
						if (CliParaMDCCheckField (MDC_PAUSE_1_2_ADR, MDC_PARAM_BIT_D)) {
							WorkUI.ulOptions |= UIF_PAUSE_OPTION_ALLOW_EMPTY;
						}
					} else if (auchPause[j] == 3 || auchPause[j] == 4) {     // LDT_PAUSEMSG3_ADR or LDT_PAUSEMSG4_ADR
						if (CliParaMDCCheckField (MDC_PAUSE_3_4_ADR, MDC_PARAM_BIT_D)) {
							WorkUI.ulOptions |= UIF_PAUSE_OPTION_ALLOW_EMPTY;
						}
					}

					if (auchPause[j] == 15)    // LDT_PAUSEMSG15_ADR special case for asterisk substitution
						// We are using numeric pause #15 as a special case to not echo data entry
						// We are doing this for Amtrak to allow PIN entry.
						sDiaStatus = UifDiaCP14(&WorkUI, UIFREG_MAX_DIGIT25, UIE_ECHO_NO, 0);
					else
						sDiaStatus = UifDiaCP4(&WorkUI, UIFREG_MAX_DIGIT25);

//					UieCtrlDevice(UIE_DIS_MSR);                     /* disable MSR */
//					UieCtrlDevice(UIE_DIS_VIRTUALKEY);              // disable Virtual Keyboard such as FreedomPay
//					UieCtrlDevice(UIE_DIS_SCANNER);                 /* disable Scanner, V3.3 */

                    if (sDiaStatus != UIF_SUCCESS) {
                        return(-1);
                    }
                }
                for (i = 0; WorkUI.aszMnemonics[i] != 0; i++) {
                    auchWork[auchPause[j] - 1][2 * i] = FSC_AN;
                    auchWork[auchPause[j] - 1][2 * i + 1] = WorkUI.aszMnemonics[i];
                }

				ausSize[auchPause[j] - 1] = 2 * i;

                if ((*pusSize + ausSize[auchPause[j] - 1]) > UIFREG_NUM_STRING) {
					char xBuff[128];
					sprintf (xBuff, "**ERROR: UifRegPauseString() pause message exceeds UIFREG_NUM_STRING.");
					NHPOS_ASSERT_TEXT(0, xBuff);
                    return(-3);
                }

				UifRegPauseStringNumber = auchPause[j];    // remember the last pause key entry

				memmove(&auchUifString[sStatus] + ausSize[auchPause[j] - 1], &auchUifString[sStatus + 2], (SHORT)(*pusSize - sStatus - 2));
				
				/*Implemented a for loop to run through the new information entered 
				during the Pause Message, where it gets the information until it gets 
				to a null terminated. JHHJ*/
				for( i = 0; auchWork[auchPause[j]-1][i] != 0; i++)
				{
					auchUifString[sStatus + i] = (UCHAR)auchWork[auchPause[j] - 1][i];
				}

				/* CALLSTRING*/
                
				*pusSize += ausSize[auchPause[j] - 1] - 2;
                
                j++;
                continue;

            } else {
                if ((*pusSize + ausSize[auchUifString[sStatus + 1] - 1]) > UIFREG_NUM_STRING) {
                    return(-4);
                }
                usSize = ausSize[auchUifString[sStatus + 1] - 1];
                memmove(&auchUifString[sStatus] + usSize, &auchUifString[sStatus + 2], (SHORT)(*pusSize - sStatus - 2));

				/*Implemented a for loop to run through the new information entered 
				during the Pause Message, where it gets the information until it gets 
				to a null terminated. JHHJ*/
				for( i = 0; i < usSize, auchWork[(*puchPtr) - 1][i] !=0; i++) {
					auchUifString[sStatus + i] = (UCHAR)auchWork[(*puchPtr) - 1][i];
				}

                *pusSize += usSize - 2;

                continue;
            }
        } else {
            return(0);
        }
    }
}

/*
*===========================================================================
** Synopsis:    VOID    UifRegPauseDisp(UCHAR uchLead)
*     Input:    
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*
** Description: Edit Control String Data
*===========================================================================
*/
//VOID    UifRegPauseDisp(USHORT uchLead) //CALLSTRING
VOID    UifRegPauseDisp(UCHAR uchLead)
{
	REGDISPMSG  DispMsg = {0};

    DispMsg.uchMajorClass = CLASS_REGDISPCOMPLSRY;
    DispMsg.uchMinorClass = CLASS_REGDISP_PAUSE;
    RflGetPauseMsg (DispMsg.aszMnemonic, uchLead);
    DispMsg.uchFsc = uchLead;   /* for 10N10D */
    DispMsg.fbSaveControl = DISP_SAVE_TYPE_4;
    DispWrite(&DispMsg);
}

/*
*===========================================================================
** Synopsis:    SHORT   UifRegSearchKey(UCHAR *pData,
*                                   UCHAR uchKey, USHORT usLen)
*     Input:    
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*
** Description: Search Target Key.
*===========================================================================
*/
//SHORT   UifRegSearchKey(USHORT *pData, UCHAR uchKey, USHORT usLen) //CALLSTRING
SHORT   UifRegSearchKey(UCHAR *pData, UCHAR uchKey, USHORT usLen)
{
    USHORT  i;

    for (i = 0; i < usLen; i += 2) {
        if (*(pData + i) == uchKey) {
            return(i);
        }
    }

    return(-1);
}

/*
*===========================================================================
** Synopsis:    SHORT   UifDiaAlphaName(UIFDIADATA *pData)
*
*       Input:  *pKeyMsg        : Pointer to Structure for UIFDIADATA
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*
** Description: Customer Name Entry.
*===========================================================================
*/
SHORT   UifDiaAlphaNameNoEcho (UIFDIADATA *pData, UCHAR uchEchoType)
{
    SHORT            sRetStatus, sStatusSave;
    USHORT           usCtrlDevice;
    KEYMSG           Data;
    SYSCONFIG CONST  *pSysConfig = PifSysConfig();

    /* set K/B mode to alpha mode */
    UieEchoBack(uchEchoType, UIFREG_MAX_DIGIT25);//US Customs 4/24/03 cwunn migrated from 19 to 25

    /* Reset Single/Double Wide Position */
    /*    UieEchoBackWide(255, 0);*/

    //UieEchoBackWide(1, 19);                                     /* RFC 01/30/2001 */
    
    if (pSysConfig->uchOperType == DISP_2X20) {

        UieEchoBackWide(1, 19);

    } else if (pSysConfig->uchOperType == DISP_LCD) {

        UieEchoBackWide(1,39);   // LCD
    }

    if (pSysConfig->uchKeyType == KEYBOARD_CONVENTION) {        /* conventional type */
        UieSetCvt(CvtAlphC1);                                   /* set standard conventional K/B table */                        
	} else if (pSysConfig->uchKeyType == KEYBOARD_MICRO) {     // 7448 Micromotion keyboard
        UieSetCvt(CvtAlphM);                                    /* set micromotion K/B table */
    } else if (pSysConfig->uchKeyType == KEYBOARD_TOUCH) {      // Touchscreen terminal uses Micromotion tables
        UieSetCvt(CvtAlphM);                                    /* set micromotion K/B table */
	} else if (pSysConfig->uchKeyType == KEYBOARD_WEDGE_68) {  // 64 key 5932 Wedge keyboard
        UieSetCvt(CvtAlph5932_68_1);                                   /* set 5932 Wedge 64 K/B table */                        
	} else if (pSysConfig->uchKeyType == KEYBOARD_WEDGE_78) {  // 78 key 5932 Wedge keyboard
        UieSetCvt(CvtAlph5932_78_1);                                   /* set 5932 Wedge 78 K/B table */                        
	} else {
        __debugbreak();   // replaced _asm {  int 3   }; to allow ARM compiles for Windows on Arm
	}

    UieSetFsc(NULL);                                            /* set FSC table pointer to NULL */
    UieCreateDialog();                                          /* create dialog */
    usCtrlDevice = UieCtrlDevice(UIE_DIS_SCANNER|UIE_DIS_WAITER);   /* disable waiter lock */
    sStatusSave = ItemOtherDifDisableStatus();      /* R3.1 for Beverage Dispenser */
    UieInitMenu(aChildRegDiaAlphaName);

    for (;;) {
        UieGetMessage(&Data, 0x0000);                                   /* wait key entry */
        sRetStatus = UieCheckSequence(&Data);                   /* check key */
        if (Data.SEL.INPUT.uchMajor == FSC_P5) {                /* cancel ? */
            break;
        }
        if (sRetStatus == UIFREG_ABORT) {       /* cancel ? */
            break;
		}
        else if (sRetStatus == UIF_DIA_SUCCESS) {
            *pData = UifRegDiaWork;
            sRetStatus = UIF_SUCCESS;       
            break;
        }
    }
    if (sStatusSave) {                  /* if previous status is enable */
        ItemOtherDifEnableStatus(1);    /* R3.1 for Beverage Dispenser */
    }
    UieCtrlDevice(usCtrlDevice);            /* enable waiter lock */
    UieDeleteDialog();                                          /* delete dialog */
    
    /* set K/B mode to neumeric mode */
    if (pSysConfig->uchKeyType == KEYBOARD_CONVENTION) {        /* conventional type */
        UieSetCvt(CvtRegC);                         
	} else if (pSysConfig->uchKeyType == KEYBOARD_MICRO) {     // 7448 Micromotion keyboard
        UieSetCvt(CvtRegM);                         
    } else if (pSysConfig->uchKeyType == KEYBOARD_TOUCH) {      // Touchscreen terminal uses Micromotion tables
        UieSetCvt(CvtRegTouch);                         
	} else if (pSysConfig->uchKeyType == KEYBOARD_WEDGE_68) {  // 64 key 5932 Wedge keyboard
        UieSetCvt(CvtReg5932_68);                         
	} else if (pSysConfig->uchKeyType == KEYBOARD_WEDGE_78) {  // 78 key 5932 Wedge keyboard
        UieSetCvt(CvtReg5932_78);                         
	} else {
        __debugbreak();   // replaced _asm {  int 3   }; to allow ARM compiles for Windows on Arm
	}

    /* bug fixed at 04/15/96 */
    /* UieSetFsc((FSCTBL FAR *)&ParaFSC1[uchDispCurrMenuPage-1]);  */ 
    UieSetFsc((FSCTBL FAR *)&ParaFSC[uchDispCurrMenuPage-1]);  /* set FSC table pointer */
    UieEchoBack(UIE_ECHO_ROW0_REG, UIFREG_MAX_INPUT);           /* set echo back */
    return(sRetStatus);
}

SHORT   UifDiaAlphaName(UIFDIADATA *pData)
{
	return UifDiaAlphaNameNoEcho (pData, UIE_ECHO_ROW0_REG);
}
/*
*===========================================================================
** Synopsis:    SHORT UifDiaAlphaNameEntry(KEYMSG *pData)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*
** Description: Customer Name Entry.
*===========================================================================
*/
SHORT   UifDiaAlphaNameEntry(KEYMSG *pData)
{
    switch (pData->uchMsg) {
    case UIM_INPUT:
        switch (pData->SEL.INPUT.uchMajor) {
        case FSC_P1:
            /* check double wide */
            if (pData->SEL.INPUT.uchLen == 0) {
                return(LDT_KEYOVER_ADR);
            }
/*          if (strchr(pData->SEL.INPUT.aszKey, 0x12) != 0) {        double wide exist
                return(LDT_KEYOVER_ADR);
            }
*/
            if (pData->SEL.INPUT.uchLen >= NUM_NUMBER) {
                return(LDT_KEYOVER_ADR);
            }

			memset(&UifRegDiaWork, '\0', sizeof(UifRegDiaWork));

            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;

            _tcsncpy(UifRegDiaWork.aszMnemonics,          /* copy input mnemonics to own buffer */
                   pData->SEL.INPUT.aszKey,
                   pData->SEL.INPUT.uchLen);

            return(UIF_DIA_SUCCESS);

        case FSC_P5:
            if (pData->SEL.INPUT.uchLen != 0) {             /* exist input data ? */
                return(LDT_SEQERR_ADR);
            }
            return(UIFREG_ABORT);

        case FSC_AUTO_SIGN_OUT:
            return(UIFREG_ABORT);

        case FSC_RECEIPT_FEED:
        case FSC_JOURNAL_FEED:
        case FSC_RESET_LOG:							/* avoid key sequence error by PCIF function, 11/12/01 */
        case FSC_POWER_DOWN:
            return(UifRegDefProc(pData));           /* reg default process */

        default:                                        
            return(LDT_SEQERR_ADR);
        }

    case UIM_REDISPLAY:
    case UIM_DRAWER:
        return(UieDefProc(pData));                  /* engine default process */

    case UIM_INIT:
    case UIM_CANCEL:
        break;

    default:                                        /* not use */
        return(LDT_SEQERR_ADR);
    }
    return (SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT   UifCheckScannerData(KEYMSG *pData)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*
** Description: Check Scanner Data by MDC Option (Label Analaysis) 2172
*===========================================================================
*/
SHORT   UifCheckScannerData(KEYMSG *pData)
{
    UIFREGOTHER UifRegOther;
    SHORT       sRetStatus;
    
	NHPOS_ASSERT_ARRAYSIZE(aszUifRegNumber, NUM_NUMBER);
	NHPOS_ASSERT_ARRAYSIZE(aszUifRegNumber, NUM_ACCT_NUMBER);
	NHPOS_ASSERT_ARRAYSIZE(UifRegOther.aszNumber, NUM_ACCT_NUMBER);
	NHPOS_ASSERT_ARRAYSIZE(UifRegOther.aszLabelNo, UIFREG_MAX_DIGIT14);

	if (pData->SEL.INPUT.uchMajor == FSC_VIRTUALKEYEVENT) {
		// Transform scanner date from a virtual keyboard event scanner data to a standard scanner data
		// We do this to make the virtual keyboard event look like just another scanner event.
		if (pData->SEL.INPUT.DEV.VIRTUALKEY.usKeyEventType == UIE_VIRTUALKEY_EVENT_SCANNER) {
			UIE_SCANNER  tempbuf;

			memcpy (&tempbuf, &(pData->SEL.INPUT.DEV.VIRTUALKEY.u.scanner_data), sizeof(UIE_SCANNER));
			memcpy (&(pData->SEL.INPUT.DEV.SCA), &tempbuf, sizeof(UIE_SCANNER));
		} else {
			/* --- return lead thru message number --- */
			return(LDT_KEYOVER_ADR);
		}
		pData->SEL.INPUT.uchMajor = FSC_SCANNER;
	}

    /* --- check scanned data length --- */
    if (pData->SEL.INPUT.DEV.SCA.uchStrLen > UIFREG_MAX_DIGIT13+1) {

        /* --- return lead thru message number --- */
        return(LDT_KEYOVER_ADR);
    }
    
    memset(&UifRegOther, 0, sizeof(UifRegOther));
    
    UifRegOther.uchMajorClass = CLASS_UIFREGOTHER;
    UifRegOther.uchMinorClass = CLASS_UIFLABEL;
    UifRegOther.uchFsc = FSC_SCANNER; /* set plu fsc */
    _tcsncpy(UifRegOther.aszLabelNo, pData->SEL.INPUT.DEV.SCA.auchStr, pData->SEL.INPUT.DEV.SCA.uchStrLen);
    
    sRetStatus = ItemOtherLabelProc(&UifRegOther);
    
    if (sRetStatus == UIF_SUCCESS) {
        memset(aszUifRegNumber, 0, sizeof(aszUifRegNumber));
        _tcsncpy(aszUifRegNumber, UifRegOther.aszNumber, NUM_ACCT_NUMBER);
        return(UIF_SUCCESS);
    } else {
        return (sRetStatus);
    }
}
#if 0
/*
*===========================================================================
** Synopsis:    SHORT   UifCheckScannerData(KEYMSG *pData)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*
** Description: Check Scanner Data,                             V3.3
*===========================================================================
*/
SHORT   UifCheckScannerData(KEYMSG *pData)
{
    TCHAR   aszCPMErrorDef[3], aszNumber[NUM_SCAN_NUMBER + 1];
    SHORT   sError;

    /* --- Check Entry Digit --- */
    if (pData->SEL.INPUT.uchLen != 0) {
        return(LDT_KEYOVER_ADR);
    }
    /* --- Scanner type 7814, 7852/7870/7880 --- */
    if (pData->SEL.INPUT.DEV.SCA.uchStrLen != NUM_SCAN_NUMBER + 1
        && pData->SEL.INPUT.DEV.SCA.uchStrLen != NUM_SCAN_NUMBER) {
        return(LDT_KEYOVER_ADR);
    }
    if (pData->SEL.INPUT.DEV.SCA.uchStrLen == NUM_SCAN_NUMBER + 1) {
        _tcsncpy(aszNumber, pData->SEL.INPUT.DEV.SCA.auchStr, pData->SEL.INPUT.DEV.SCA.uchStrLen);

    /* --- Scanner type 7852/7870/7880 --- */
    } else {
        aszNumber[0] = pData->SEL.INPUT.DEV.SCA.auchStr[0];
        aszNumber[1] = _T('0');
        _tcsncpy(&aszNumber[2], &pData->SEL.INPUT.DEV.SCA.auchStr[1], pData->SEL.INPUT.DEV.SCA.uchStrLen - 1);
    }

    aszCPMErrorDef[2] = '\0';
    /* --- EAN-13 Digits Label --- */
    if (aszNumber[0] == _T('A')|| aszNumber[0] == _T('F')) {
        aszCPMErrorDef[0] = aszNumber[1];
        aszCPMErrorDef[1] = aszNumber[2];

    /* --- UPC-A 12 Digits Label --- */
    } else if (aszNumber[1] == _T('A')) {
        aszCPMErrorDef[0] = _T('0');
        aszCPMErrorDef[1] = aszNumber[2];

    } else {
        return(LDT_PROHBT_ADR);
    }
    sError = _ttoi(aszCPMErrorDef);
    switch (sError) {
    case    2:
    case    4:
    case    20:
    case    21:
    case    22:
    case    23:
    case    24:
    case    25:
    case    26:
    case    27:
    case    28:
    case    29:
        break;

    default:
        return(LDT_KEYOVER_ADR);
    }

    /* --- Check Digit OK ? --- */
    if (aszNumber[13] != 0) {
        if (aszNumber[13] != UifCheckScannerCDV(&aszNumber[1], 12)) {
            return(LDT_PROHBT_ADR);
        }
    }

    memset(aszUifRegNumber, 0, sizeof(aszUifRegNumber));
    memmove(aszUifRegNumber, &aszNumber[3], NUM_ACCT_NUMBER *sizeof(TCHAR));

    return(UIF_SUCCESS);
    
}

/*
*===========================================================================
**Synopsis: UCHAR   UifCheckScannerCDV(UCHAR *aszPluNo, SHORT sLen)
*
*    Input: 
*   Output: None
*    InOut: None
**Return:
*
** Description:     Check CDV
*===========================================================================
*/
UCHAR   UifCheckScannerCDV(TCHAR *aszPluNo, SHORT sLen)
{
    SHORT   i, sEven, sOdd, cdigit;

    for (sEven = sOdd = i = 0 ; i < sLen; ++i) {
        if (i % 2) {
            sOdd += aszPluNo[i] & 0x0f;             /* Odd */
        } else {
            sEven += aszPluNo[i] & 0x0f;            /* Even */
        }
    }
    cdigit = (sEven + sOdd*3) % 10;
    cdigit = (10 - cdigit) % 10;
    return((UCHAR)(cdigit | 0x30));
}
#endif

/****** End of Source ******/
