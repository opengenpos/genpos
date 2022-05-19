/*
*===========================================================================
* Title       : Reg Item Sales Module
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application
* Program Name: UIFITEM.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:
*
*               UifItem() : Reg Item Slaes
*               UifRegShiftQty() : Shift Qty Amount
* --------------------------------------------------------------------------
* Update Histories
*    Data  :Ver.Rev.:   Name    : Description
* May-09-92:00.00.01:M.Suzuki   : initial
* Jul-26-93:01.00.12:K.You      : memory compaction
* Apr-26-94:00.00.05:K.You      : add trace routine for test
* Feb-23-95:03.00.00:hkato      : R3.0
* Nov-08-95:03.01.00:hkato      : R3.1
*
** NCR 2171
* Sep-22-99:01.00.00:M.Teraki   : Remove some codes for debug in UifItem()
* Dec-08-99:01.00.00:hrkato     : Saratoga
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
#include <windows.h>
#include <tchar.h>
#include <string.h>

#include "ecr.h"
#include "pif.h"
#include "uie.h"
#include "fsc.h"
#include "paraequ.h"
#include "para.h"
#include "regstrct.h"
#include "item.h"
#include "display.h"
#include "uireg.h"
#include "uiregloc.h"
//SR 143 cwunn @/For & HALO Override
#include "cpm.h"
#include "csetk.h"
#include "transact.h"
#include "rfl.h"
//SR 143 @/FOR Cancel Mechanism
#include	"..\Sa\UIE\uieseq.h"
#include <mld.h>
#include <BlFWif.h>
#include <csstbpar.h>
#include <cscas.h>
#include	"..\Sa\UIE\uiedev.h"


/*
*===========================================================================
	FSC code data tables.

	if(ParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)) { //if US Customs #/type bit turned on
		// open item sales key sequence for US Customs SCER cwunn 4/8/03
		UieOpenSequence(aszSeqRegItemCustoms);
	}
	else { //if US Customs #/type bit turned off
		// open item sales key sequence for everyone else
		UieOpenSequence(aszSeqRegItem);
	}

*===========================================================================
*/
static UISEQ FARCONST aszSeqRegItem[] = {FSC_KEYED_DEPT, FSC_KEYED_PLU, FSC_PLU,
                                  FSC_ADJECTIVE, FSC_PRINT_MODIF, FSC_QTY, FSC_RECEIPT,
                                  FSC_KEYED_COUPON, FSC_COUPON, FSC_REPEAT,
                                  FSC_PRICE_CHECK, FSC_ITEM_DISC, FSC_TARE,FSC_TOTAL,
                                  FSC_DEPT, FSC_PRICE_ENTER, FSC_E_VERSION,
                                  FSC_SCANNER, FSC_VALIDATION /*SR 141 jhall*/, FSC_ASK, /*SR 143 cwunn*/ FSC_FOR,
								  /*SR 143 cwunn*/FSC_HALO, /*QTY GUARD RPH 2-2-4*/FSC_QTY_GUARD, FSC_TIME_IN /*PDINU*/, FSC_TIME_OUT /*PDINU*/, FSC_SIGN_IN /*PDINU*/, FSC_B /*PDINU*/,
								  FSC_EDIT_CONDIMENT, FSC_EDIT_COND_TBL, FSC_TENT,FSC_ORDER_DEC,FSC_ADJ_MOD,FSC_GIFT_CARD,
								  FSC_MSR, FSC_PINPAD, FSC_SIGNATURECAPTURE, FSC_SUPR_INTRVN, FSC_DOC_LAUNCH, FSC_OPR_PICKUP, 
								  FSC_WINDOW_DISPLAY, FSC_WINDOW_DISMISS, FSC_VIRTUALKEYEVENT, FSC_AUTO_SIGN_OUT,0};

static UISEQ FARCONST aszSeqRegItemCustoms[] = {FSC_KEYED_DEPT, FSC_KEYED_PLU, FSC_PLU,
                                  FSC_ADJECTIVE, FSC_PRINT_MODIF, FSC_QTY, FSC_RECEIPT,
                                  FSC_KEYED_COUPON, FSC_COUPON, FSC_REPEAT,
                                  FSC_PRICE_CHECK, FSC_ITEM_DISC, FSC_TARE,FSC_TOTAL,
                                  FSC_DEPT, FSC_PRICE_ENTER, FSC_E_VERSION,
                                  FSC_SCANNER, FSC_ASK,  FSC_VALIDATION /*SR 141 jhall*/,
								  /*SR 143 cwunn*/ FSC_FOR,
								  /*SR 143 cwunn*/ FSC_HALO, FSC_NUM_TYPE, FSC_P1,
								  /*QTY GUARD RPH 2-2-4*/FSC_QTY_GUARD,
								   FSC_TIME_IN /*PDINU*/, FSC_TIME_OUT /*PDINU*/, FSC_SIGN_IN /*PDINU*/, FSC_B /*PDINU*/,
								  FSC_EDIT_CONDIMENT,FSC_EDIT_COND_TBL,FSC_TENT,FSC_ORDER_DEC,FSC_ADJ_MOD,FSC_GIFT_CARD,
								  FSC_MSR, FSC_PINPAD, FSC_SIGNATURECAPTURE, FSC_SUPR_INTRVN, FSC_DOC_LAUNCH, FSC_OPR_PICKUP,
								  FSC_WINDOW_DISPLAY, FSC_WINDOW_DISMISS, FSC_VIRTUALKEYEVENT, FSC_AUTO_SIGN_OUT,0};

static SHORT sForKeyPressed = 0;

/*
*===========================================================================
** Synopsis:    SHORT UifItem(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Item Sales Module
*===========================================================================
*/
SHORT UifItem(KEYMSG *pData)
{
    SHORT   sRetStatus = UIF_SUCCESS, sRet;
    SHORT   sStatusSave;
	REGDISPMSG        UifDisp = {0};
	UIFREGMODIFIER    regmodifier = {0};             /* Modifier, GCA */
	PARATERMINALINFO  myTerminalInfo = {0};

	NHPOS_ASSERT_ARRAYSIZE(UifDisp.aszStringData, NUM_NUMBER);
	NHPOS_ASSERT_ARRAYSIZE(UifDisp.aszStringData, UIFREG_MAX_DIGIT25);

    NHPOS_ASSERT_ARRAYSIZE(UifRegData.regsales.aszNumber, UIFREG_MAX_DIGIT25);

    NHPOS_ASSERT_ARRAYSIZE(aszUifRegNumber, STD_NUMBER_LEN + 1);
    NHPOS_ASSERT(sizeof(UifRegData.regsales.aszNumber[0]) == sizeof(aszUifRegNumber[0]));

    NHPOS_ASSERT_ARRAYSIZE(UifRegData.regsales.aszPLUNo, UIFREG_MAX_DIGIT14);
    NHPOS_ASSERT(sizeof(UifRegData.regcoupon.aszNumber[0]) == sizeof(aszUifRegNumber[0]));

    NHPOS_ASSERT_ARRAYSIZE(UifRegData.regsales.aszPLUNo, UIFREG_MAX_DIGIT77);
    NHPOS_ASSERT_ARRAYSIZE(pData->SEL.INPUT.DEV.SCA.auchStr, UIFREG_MAX_DIGIT77);
    NHPOS_ASSERT(sizeof(UifRegData.regsales.aszPLUNo[0]) == sizeof(pData->SEL.INPUT.DEV.SCA.auchStr[0]));

	// assertions for the length of the temporary work space global aszUifRegNumber
	NHPOS_ASSERT_ARRAYSIZE(aszUifRegNumber, NUM_ACCT_NUMBER);

	myTerminalInfo.uchMajorClass = CLASS_PARATERMINALINFO;
	myTerminalInfo.uchAddress = (UCHAR)PIF_CLUSTER_MASK_TERMINAL_NO(PifSysConfig()->usTerminalPositionFromName);
	ParaTerminalInfoParaRead (&myTerminalInfo);

    switch (pData->uchMsg) {
    case UIM_INIT:
		if(ParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)) { //if US Customs #/type bit turned on
			UieOpenSequence(aszSeqRegItemCustoms);     // open item sales key sequence for US Customs SCER cwunn 4/8/03
		}
		else { //if US Customs #/type bit turned off
			UieOpenSequence(aszSeqRegItem);     // open item sales key sequence for Saratoga
		}
        break;

    case UIM_INPUT:
        sStatusSave = ItemOtherDifDisableStatus();        /* R3.1 for Beverage Dispenser */
        UifRegData.regsales.uchMajorClass = CLASS_UIFREGSALES;  /* set salea major class */
        switch (pData->SEL.INPUT.uchMajor) {
			//-----------------------------------------------------------
			// follow types of FSC are codes that we want to be ignored as if they did not
			// happen if we are not in a transaction.  Otherwise we will just pass them on
			// to the rest of the menu processing functionality to see if they have been
			// enabled by some other part of the menu tree.
			case FSC_TOTAL:
				sRetStatus = CheckAllowFunction();		// Looks to see if transaction has been itemized
				if (sRetStatus == SUCCESS) {
					// we are not in a transaction so make this total key transparent
					// basically this just ignores the total key press.
					return(UIE_SUC_KEY_TRANSPARENT);
				} else {
					// we are in a transaction so make this total key visible to the menu tree
					// we are ignoring this total key press ourself in order to pass it on.
					// if there is not some other part of the menu tree waiting for this key press
					// then it will issue a Sequence Error dialog.
					return(UIE_SUC_KEY_IGNORE);
				}
			//-----------------------------------------------------------
		/*This case restricts the user of signing out while
		  a PLU is entered.   ***PDINU
		*/
		case FSC_SIGN_IN:
		case FSC_B:
			//JMASON -- if usCashierID > 0 then a GCF is open and an
			// operator should NOT be allowed to sign out until the
			// GCF is closed or transaction is completed.
			if (TranGCFQualPtr->ulCashierID > 0) {
				return(LDT_PROHBT_ADR);
			}
			sRetStatus = CheckAllowFunction();		// Looks to see if transaction has been itemized
			if (sRetStatus == SUCCESS) {
				if ((pData->SEL.INPUT.uchMajor == FSC_SIGN_IN) || (pData->SEL.INPUT.uchMajor == FSC_B)) {
					if (pData->SEL.INPUT.uchLen != 0) {             /* exist input data ? */
						return(LDT_SEQERR_ADR);
					}
					if (aszUifRegNumber[0] != 0) {                  /* input number ? */
						return(LDT_SEQERR_ADR);
					}
					UifRegData.regopeclose.uchMajorClass = CLASS_UIFREGOPECLOSE;   /* set cashier sign out class */
					if (pData->SEL.INPUT.uchMajor == FSC_SIGN_IN) {                /* sign in key ? */
						UifRegData.regopeclose.uchMinorClass = CLASS_UICASHIEROUT;
					} else {
						UifRegData.regopeclose.uchMinorClass = CLASS_UIB_OUT;
					}
					UifRegData.regopeclose.ulStatusFlags |= UIFREGMISC_STATUS_METHOD_MANUAL;
					sRetStatus = ItemSignOut(&UifRegData.regopeclose);    /* Operator Sign Out Modele */
					UifRegWorkClear();                                    /* clear work area */
					if (sRetStatus == UIF_CANCEL) {                       /* cancel ? */
						UieReject();                                      /* send reject to parent */
						return(SUCCESS);
					}
					if (sRetStatus == UIF_CAS_SIGNOUT) {           /* success or sign out ? */
						flUifRegStatus |= UIFREG_CASHIERSIGNOUT;   /* set cashier sign out status */
						BlFwIfDefaultUserScreen();				/* call function to bring up default window(s) */
						UieAccept();                               /* send accepted to parent */
						return(SUCCESS);
					}
					return(sRetStatus);
				}
			}else{
				UieEmptyRingBufClearMacro();
				return(sRetStatus);
			}
			break;

		/*This case allows the user to time-out while a user is signed in.
		  This case also restricts the time-out depending
		  on whether or not a PLU is entered.  If a transaction has
		  begun, The user must either total out the transaction
		  or cancell the transaction to be able to time in/out ***PDINU
		*/
		case FSC_TIME_OUT:
			sRetStatus = CheckAllowFunction();		// Looks to see if transaction has been itemized
			if (sRetStatus == SUCCESS) {			

				/* JMASON -- If MSR required prompt for supervisor intervention */
				if(CliParaMDCCheck(MDC_MSRSIGNIN_ADR,ODD_MDC_BIT3))
				{
					if ( ItemSPVInt(LDT_SUPINTR_ADR) != 0 )
						return( UIF_CANCEL );
				}

                UifRegData.regmisc.uchMajorClass = CLASS_UIFREGMISC;
				UifRegData.regmisc.uchMinorClass = CLASS_UIETKOUT;
				UifRegData.regmisc.ulEmployeeNo = (ULONG)pData->SEL.INPUT.lData;
				UifRegData.regmisc.uchJobCode = 0;
				UifRegData.regmisc.sTimeInStatus = 0;
				if (pData->SEL.INPUT.lData != 0) {
					UifRegData.regmisc.ulStatusFlags |= UIFREGMISC_STATUS_METHOD_MANUAL;
				}
				sRetStatus = ItemMiscETKSignIn(&UifRegData.regmisc);

    			ItemSignInClearDisplay(1);                          //Clears the display
				UieEchoBack(UIE_ECHO_ROW0_REG, UIFREG_MAX_INPUT);	//Allows echo for row 0
				UieAccept();
            }else{
				return(sRetStatus);
			}
			break;

		/*This case allows the user to time-in while a user is signed in.
		  This case also restricts the time-in depending
		  on whether or not a PLU is entered.  If a transaction has
		  begun, The user must either total out the transaction
		  or cancell the transaction to be able to time in/out ***PDINU
		*/
		case FSC_TIME_IN:
			sRetStatus = CheckAllowFunction();		// Looks to see if transaction has been itemized
			if (sRetStatus == SUCCESS) {
				/* JMASON -- If MSR required prompt for supervisor intervention */
				if(CliParaMDCCheck(MDC_MSRSIGNIN_ADR,ODD_MDC_BIT3))
				{
					if ( ItemSPVInt(LDT_SUPINTR_ADR) != 0 )
						return( UIF_CANCEL );
				}

                UifRegData.regmisc.uchMajorClass = CLASS_UIFREGMISC;
				UifRegData.regmisc.uchMinorClass = CLASS_UIETKIN;
				UifRegData.regmisc.ulEmployeeNo = (ULONG)pData->SEL.INPUT.lData;
				UifRegData.regmisc.uchJobCode = 0;
				UifRegData.regmisc.sTimeInStatus = 0;
				if (pData->SEL.INPUT.lData != 0) {
					UifRegData.regmisc.ulStatusFlags |= UIFREGMISC_STATUS_METHOD_MANUAL;
				}
				sRetStatus = ItemMiscETKSignIn(&UifRegData.regmisc);

				ItemSignInClearDisplay(1);                          //Clears the display
				UieEchoBack(UIE_ECHO_ROW0_REG, UIFREG_MAX_INPUT);	//Allows echo for for row 0
				UieAccept();
			}
			break;

        case FSC_KEYED_DEPT:
            if (!pData->SEL.INPUT.uchMinor || (pData->SEL.INPUT.uchMinor > FSC_DEPT_MAX)) {  /* check dept # */
                return(LDT_PROHBT_ADR);
            }
            if (pData->SEL.INPUT.uchDec != 0xFF) {                /* input decimal point ? */
                return(LDT_KEYOVER_ADR);
            }
            if (pData->SEL.INPUT.uchLen > UIFREG_MAX_AMT_DIGITS) {    /* input digit over ? */
                return(LDT_KEYOVER_ADR);
            }
            if (UifRegData.regsales.uchAdjective != 0) {          /* input adjective key ? */
                return(LDT_SEQERR_ADR);
            }
            if (uchUifRegVoid != 0) {                             /* input item void ? */
                UifRegData.regsales.fbModifier |= VOID_MODIFIER;  /* set item void bit */
            }
            if (fchUifSalesMod & EVER_MODIFIER) {
                return(LDT_SEQERR_ADR);
            }
            if (fchUifSalesMod & PCHK_MODIFIER) {
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.lUnitPrice) {                /* price key */
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.usDeptNo) {
                return(LDT_SEQERR_ADR);
            }

			sRet = ItemSalesOrderDeclaration(UifRegData.regsales,0, CLASS_ORDER_DEC_NEW);
			if(sRet != SUCCESS)
			{
				return sRet;
			}
            _tcsncpy(UifRegData.regsales.aszNumber, aszUifRegNumber, NUM_NUMBER);

            if (UifRegData.regsales.uchMinorClass != CLASS_UIMODDISC) {  /* input modifiered discount ? */
                if ((pData->SEL.INPUT.uchLen == 0) && !sForKeyPressed) {         /* repeat ? */
                    if (uchUifRegVoid != 0
                        || UifRegData.regsales.lQTY != 0
                        || UifRegData.regsales.uchAdjective != 0
                        || UifRegData.regsales.aszNumber[0] != 0) {  /* R3.0 */
                        return(LDT_SEQERR_ADR);
                    }
                    UifRegData.regsales.uchMinorClass = CLASS_UIDEPTREPEAT; /* set dept repeat minor class */
                } else {
                    UifRegData.regsales.uchMinorClass = CLASS_UIDEPT;   /* set dept minor class */
                }
            }
            if ((pData->SEL.INPUT.uchLen != 0) && (pData->SEL.INPUT.lData == 0L)) {
                                                        /* '0' input ? */
                UifRegData.regsales.fbInputStatus |= INPUT_0_ONLY;  /* set '0' input status */
            }
            UifRegData.regsales.uchMajorFsc = pData->SEL.INPUT.uchMajor; /* set dept fsc */
            UifRegData.regsales.uchFsc = pData->SEL.INPUT.uchMinor; /* set dept extend fsc */
            /* UifRegData.regsales.uchDeptNo = pData->SEL.INPUT.uchMinor; / set dept extend fsc */
            UifRegData.regsales.lUnitPrice = (DUNITPRICE)pData->SEL.INPUT.lData;  /* set input amount */

			ItemPreviousGetForKeyData(&UifRegData.regsales); // for FOR DATA

            sRetStatus = ItemSales(&UifRegData.regsales); /* Sales Modele */

			if((sRetStatus == UIF_SUCCESS) && sForKeyPressed)
			{
				ItemPreviousClearForKeyData();    // Clear @/For data for a FSC_KEYED_DEPT entry
				sForKeyPressed = FALSE;
			}

            break;

		case FSC_ADJ_MOD:
            if (pData->SEL.INPUT.uchDec != 0xFF) {          /* input decimal point ? */
                return(LDT_KEYOVER_ADR);
            }
            if (pData->SEL.INPUT.uchLen) {  /* input digit over ? */
                return(LDT_KEYOVER_ADR);
            }
            if (UifRegData.regsales.uchAdjective != 0) {    /* input adjective key ? */
                return(LDT_SEQERR_ADR);
            }
            if (uchUifRegVoid != 0) {                   /* input item void ? */
                UifRegData.regsales.fbModifier |= VOID_MODIFIER;  /* set item void bit */
            }
            if (fchUifSalesMod & EVER_MODIFIER) {
                return(LDT_SEQERR_ADR);
            }
            if (fchUifSalesMod & PCHK_MODIFIER) {
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.lUnitPrice) {      /* price key */
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.usDeptNo) {
                return(LDT_SEQERR_ADR);
            }

			sRet = ItemSalesAdjModify(&UifRegData.regsales);
			if(sRet != SUCCESS)
			{
				return(sRet);
			}

			break;


        case FSC_KEYED_PLU:
            if (!pData->SEL.INPUT.uchMinor || (pData->SEL.INPUT.uchMinor > FSC_PLU_MAX)) {  /* check PLU # */
                return(LDT_PROHBT_ADR);
            }
            if (UifRegData.regsales.usDeptNo) {
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.lUnitPrice) {      /* price key */
                return(LDT_SEQERR_ADR);
            }

            if (pData->SEL.INPUT.uchDec != 0xFF) {       /* input decimal point ? */
                if (UifRegData.regsales.lQTY != 0L) {    /* input qty ? */
                    return(LDT_SEQERR_ADR);
                }
                if ((pData->SEL.INPUT.uchLen - pData->SEL.INPUT.uchDec - 1) > UIFREG_MAX_DIGIT3) {  /* input digit over ? */
                    return(LDT_KEYOVER_ADR);
                }
                if (pData->SEL.INPUT.uchDec > UIFREG_MAX_DIGIT3) {  /* input digit over ? */
                    return(LDT_KEYOVER_ADR);
                }

                UifRegData.regsales.lQTY = UifRegShiftQty((LONG)pData->SEL.INPUT.lData, pData->SEL.INPUT.uchDec);
                                                        /* set input amount to qty */
            } else {
                if (pData->SEL.INPUT.uchLen > UIFREG_MAX_AMT_DIGITS) {  /* input digit over ? */
                    return(LDT_KEYOVER_ADR);
                }
                if ((pData->SEL.INPUT.uchLen != 0) && (pData->SEL.INPUT.lData == 0L)) {
                                                            /* '0' input ? */
                    UifRegData.regsales.fbInputStatus |= INPUT_0_ONLY;  /* set '0' input status */
                }
				if(pData->SEL.INPUT.lData)
				{
					PARATRANSHALO     HALOInfo = {0};
					ULONG			  ulHaloData;
										/* check data(HALO) */
					HALOInfo.uchMajorClass =  CLASS_PARATRANSHALO;
					HALOInfo.uchAddress = HALO_PLU_DIGIT_ADR;
					CliParaRead( &HALOInfo );
					ulHaloData = HALOInfo.uchHALO;

					if((ulHaloData) && (pData->SEL.INPUT.lData > (LONG)ulHaloData))
					{
						return(LDT_PROHBT_ADR);
					}
				}
                UifRegData.regsales.lUnitPrice = (DUNITPRICE)pData->SEL.INPUT.lData;  /* set input amount */
            }
            if (uchUifRegVoid != 0) {                   /* input item void ? */
                UifRegData.regsales.fbModifier |= VOID_MODIFIER;  /* set item void bit */
            }
            if (fchUifSalesMod & EVER_MODIFIER) {
                return(LDT_SEQERR_ADR);
            }
            if (fchUifSalesMod & PCHK_MODIFIER) {
                UifRegData.regsales.fbModifier2 |= PCHK_MODIFIER;  /* set item void bit */
            }
			sRet = ItemSalesOrderDeclaration(UifRegData.regsales,0, CLASS_ORDER_DEC_NEW);
			if(sRet != SUCCESS)
			{
				return sRet;
			}
            _tcsncpy(UifRegData.regsales.aszNumber, aszUifRegNumber, NUM_NUMBER);

            if(pData->SEL.INPUT.uchMajor == FSC_ORDER_DEC)
			{
				UifRegData.regsales.uchMinorClass = CLASS_ITEMORDERDEC;
			} else if (UifRegData.regsales.uchMinorClass != CLASS_UIMODDISC) {  /* input modifiered discount ? */
                UifRegData.regsales.uchMinorClass = CLASS_UIPLU;   /* set plu minor class */
            }
            UifRegData.regsales.uchMajorFsc = pData->SEL.INPUT.uchMajor; /* set plu fsc */
            UifRegData.regsales.uchFsc = pData->SEL.INPUT.uchMinor; /* set plu extend fsc */
            sRetStatus = ItemSales(&UifRegData.regsales); /* Sales Modele */

			if((sRetStatus == UIF_SUCCESS) && sForKeyPressed)
			{
				ItemPreviousClearForKeyData();      // Clear @/For data for FSC_KEYED_PLU entry
				sForKeyPressed = FALSE;
			}

            if ((sRetStatus == UIF_NUMBER) || (sRetStatus == UIF_SKUNUMBER)) {
                /* scanned label is number */
                if (aszUifRegNumber[0] != 0) {         /* already input number ? */
                    return(LDT_SEQERR_ADR);
                }
                if (sRetStatus == UIF_SKUNUMBER) {
                    UifRegData.regsales.fbModifier2 |= SKU_MODIFIER;
                }
                /* set number */
                _tcsncpy(aszUifRegNumber, UifRegData.regsales.aszNumber, NUM_NUMBER);

                flUifRegStatus |= UIFREG_MODIFIER;      /* set modifier status */
                UifDisp.uchMajorClass = CLASS_UIFREGDISP;   /* set display class */
                if (sRetStatus == UIF_SKUNUMBER) {
                    UifDisp.uchMinorClass = CLASS_UIFDISP_SKUNUMBER;
                } else {
                    UifDisp.uchMinorClass = CLASS_UIFDISP_NUMBER;
                }
                _tcsncpy(UifDisp.aszStringData, pData->SEL.INPUT.aszKey, pData->SEL.INPUT.uchLen);       /* set number */
                DispWrite(&UifDisp);                    /* display */
                /* clear plu no. for next entry */
                memset(UifRegData.regsales.aszPLUNo, 0, sizeof(UifRegData.regsales.aszPLUNo));
                return (SUCCESS);
            }
            break;

	case FSC_ORDER_DEC:
            if (UifRegData.regsales.usDeptNo) {
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.lUnitPrice) {      /* price key */
                return(LDT_SEQERR_ADR);
            }

			//do a switch on the type of status the operator has, whether it
			//be force a choice of order declaration, or use the terminal default
			//if we have not already chosen an order declaration
			//then we will find out the default set for this terminal
			//and use that JHHJ
			if(ItemSalesLocalSaleStarted ())
			{
				sRet = ItemSalesOrderDeclaration(UifRegData.regsales,pData->SEL.INPUT.uchMinor, CLASS_ORDER_DEC_MID);
				if(sRet != SUCCESS)
				{
					return sRet;
				}
			} else
			{
				if((TranCurQualPtr->fsCurStatus2 & CURQUAL_ORDERDEC_DECLARED) == 0)
				{
					sRet = ItemSalesOrderDeclaration(UifRegData.regsales,pData->SEL.INPUT.uchMinor, CLASS_ORDER_DEC_FSC);
					if(sRet != SUCCESS)
					{
						return sRet;
					}else
					{
						if(sRet == SUCCESS)
						{
							// return lead through for post order declaration.
							// this will let the software know that it needs to
							// retrieve the event that triggered order declaration
							// and process the event.
							return (LDT_POST_ODRDEC_EVENT);
						}
					}
				}
			}

			break;

        case FSC_PLU:   /* 2172 */
			if ((sRetStatus = UifRegInputCheck(pData, UIFREG_MAX_DIGIT14)) != SUCCESS) {  /* input data check */
                return(sRetStatus);
            }
            if (UifRegData.regsales.lUnitPrice) {      /* price key */
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.usDeptNo) {
                return(LDT_SEQERR_ADR);
            }
            if (uchUifRegVoid != 0) {                   /* input item void ? */
                UifRegData.regsales.fbModifier |= VOID_MODIFIER;  /* set item void bit */
            }
            if (fchUifSalesMod & EVER_MODIFIER) {
                UifRegData.regsales.fbModifier2 |= EVER_MODIFIER;  /* set item void bit */
            }
            if (fchUifSalesMod & PCHK_MODIFIER) {
                UifRegData.regsales.fbModifier2 |= PCHK_MODIFIER;  /* set item void bit */
            }

			sRet = ItemSalesOrderDeclaration(UifRegData.regsales,0, CLASS_ORDER_DEC_NEW);
			if(sRet != SUCCESS)
			{
				return sRet;
			}
            _tcsncpy(UifRegData.regsales.aszNumber, aszUifRegNumber, NUM_NUMBER);

            if (UifRegData.regsales.uchMinorClass != CLASS_UIMODDISC) {  /* input modifiered discount ? */
                if (pData->SEL.INPUT.uchLen == 0) {         /* repeat ? */
                    if (uchUifRegVoid != 0
                        || UifRegData.regsales.lQTY != 0
                        || UifRegData.regsales.uchAdjective != 0
                        || UifRegData.regsales.aszNumber[0] != 0  /* R3.0 */
                        || UifRegData.regsales.lUnitPrice != 0) {  /* Saratoga */
                        return(LDT_SEQERR_ADR);
                    }
                    UifRegData.regsales.uchMinorClass = CLASS_UIPLUREPEAT; /* set plu repeat minor class */
                } else {
                    UifRegData.regsales.uchMinorClass = CLASS_UIPLU;    /* set plu minor class */
                }
            }
            _tcsncpy(UifRegData.regsales.aszPLUNo, pData->SEL.INPUT.aszKey, pData->SEL.INPUT.uchLen);   /* 2172 */

            UifRegData.regsales.uchMajorFsc = pData->SEL.INPUT.uchMajor; /* set plu fsc */

			//SR 143 @/For Key cwunn 8/20
			//If previous item was an @/For entry,
			// get unit price, quantity, etc and send to processing function
			ItemPreviousGetForKeyData(&UifRegData.regsales);
			//END SR 143

            sRetStatus = ItemSales(&UifRegData.regsales); /* Sales Modele */

			if((sRetStatus == UIF_SUCCESS) && sForKeyPressed)
			{
				ItemPreviousClearForKeyData();    // Clear @/For data for FSC_PLU entry
				sForKeyPressed = FALSE;
			}

            if ((sRetStatus == UIF_NUMBER) || (sRetStatus == UIF_SKUNUMBER)) {
                /* scanned label is number */
                if (aszUifRegNumber[0] != 0) {         /* already input number ? */
                    return(LDT_SEQERR_ADR);
                }
                if (sRetStatus == UIF_SKUNUMBER) {
                    UifRegData.regsales.fbModifier2 |= SKU_MODIFIER;
                }
                /* set number */
                _tcsncpy(aszUifRegNumber, UifRegData.regsales.aszNumber, NUM_NUMBER);

                flUifRegStatus |= UIFREG_MODIFIER;      /* set modifier status */
                UifDisp.uchMajorClass = CLASS_UIFREGDISP;   /* set display class */
                if (sRetStatus == UIF_SKUNUMBER) {
                    UifDisp.uchMinorClass = CLASS_UIFDISP_SKUNUMBER;
                } else {
                    UifDisp.uchMinorClass = CLASS_UIFDISP_NUMBER;
                }
                _tcsncpy(UifDisp.aszStringData, aszUifRegNumber, NUM_NUMBER);

                DispWrite(&UifDisp);                    /* display */
                /* clear plu no. for next entry */
                memset(UifRegData.regsales.aszPLUNo, 0, sizeof(UifRegData.regsales.aszPLUNo));
                return (SUCCESS);
            }
            break;

		case FSC_RECEIPT:
            if (pData->SEL.INPUT.uchLen != 0) {             /* exist input data ? */
                return(LDT_SEQERR_ADR);
            }
            if ((sRetStatus = UifRegVoidNoCheck()) != SUCCESS) {  /* check item void,number */
                return(sRetStatus);
            }
            /* UifRegData.regmodifier.uchMajorClass = CLASS_UIFREGMODIFIER;    set receipt/noreceipt class 
            UifRegData.regmodifier.uchMinorClass = CLASS_UINORECEIPT; */
            sRetStatus = ItemModRec();                  /* Modifier Module */
            break;

        case FSC_PRINT_MODIF:
            if (!pData->SEL.INPUT.uchMinor || (pData->SEL.INPUT.uchMinor > FSC_PM_MAX)) {   /* check print modifier # */
                return(LDT_PROHBT_ADR);
            }
            if (pData->SEL.INPUT.uchLen != 0) {             /* exist input data ? */
                return(LDT_SEQERR_ADR);
            }
            if ((sRetStatus = UifRegVoidNoCheck()) != SUCCESS) {  /* check item void or number */
                return(sRetStatus);
            }
            if (UifRegData.regsales.lQTY != 0L) {       /* input qty ? */
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.uchAdjective != 0) {    /* input adjective ? */
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.uchMinorClass == CLASS_UIMODDISC) {  /* input modifiered discount ? */
                return(LDT_SEQERR_ADR);
            }
            if (fchUifSalesMod & EVER_MODIFIER) {
                return(LDT_SEQERR_ADR);
            }
            if (fchUifSalesMod & PCHK_MODIFIER) {
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.usDeptNo) {
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.lUnitPrice) {      /* price key */
                return(LDT_SEQERR_ADR);
            }
            UifRegData.regsales.uchMinorClass = CLASS_UIPRINTMODIFIER;  /* set print modifier minor class */
            UifRegData.regsales.uchPrintModifier = pData->SEL.INPUT.uchMinor;  /* set print modifier # */
            sRetStatus = ItemSales(&UifRegData.regsales); /* Sales Modele */
            break;

        case FSC_ITEM_DISC:
            if (!pData->SEL.INPUT.uchMinor || (pData->SEL.INPUT.uchMinor > FSC_IDISC_MAX)) { /* check item discount # */
                return(LDT_PROHBT_ADR);
            }
            if ((sRetStatus = UifRegInputCheck(pData, UIFREG_MAX_AMT_DIGITS)) != SUCCESS) {  /* check input data */
                return(sRetStatus);
            }
            if (UifRegData.regsales.lQTY != 0L) {       /* input qty ? */
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.uchAdjective != 0) {    /* input adjective ? */
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.uchMinorClass == CLASS_UIMODDISC) {  /* input modifiered discount ? */
                return(LDT_SEQERR_ADR);
            }
            if (fchUifSalesMod & EVER_MODIFIER) {
                return(LDT_SEQERR_ADR);
            }
            if (fchUifSalesMod & PCHK_MODIFIER) {
                return(LDT_SEQERR_ADR);
            }
            if (pData->SEL.INPUT.uchMinor == FSC_MODIFIER_DISCOUNT) {   /* modifiered discount ? */
                UifRegData.regsales.uchMinorClass = CLASS_UIMODDISC; /* set modifiered discount minor class */
                UifRegData.regsales.lDiscountAmount = pData->SEL.INPUT.lData; /* set amount */
                /* V3.3 */
                sRetStatus = ItemSales(&UifRegData.regsales); /* Sales Modele */
                if (sRetStatus == LDT_CANCEL_ADR) {
                    flUifRegStatus |= UIFREG_MODIFIER;          /* set modifier status */
                    UifDisp.uchMajorClass = CLASS_UIFREGDISP;   /* set display class */
                    UifDisp.uchMinorClass = CLASS_UIFDISP_MODDISC;
                    UifDisp.lAmount = pData->SEL.INPUT.lData;
                    DispWrite(&UifDisp);                        /* display */
                    return(SUCCESS);
                } else
                if (sRetStatus == UIF_SUCCESS) {
                    break;
                } else {
                    UifRegData.regsales.uchMinorClass = 0; /* clear modifiered discount minor class */
                    UifRegData.regsales.lDiscountAmount = 0L;
                    break;
                }
            }
            /**** V1.0.04 ***/
            if (UifRegData.regsales.lUnitPrice) {      /* price key */
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.usDeptNo) {
                return(LDT_SEQERR_ADR);
            }
            /******/
            if (uchUifRegVoid != 0) {                   /* input item void ? */
                UifRegData.regdisc.fbModifier |= VOID_MODIFIER;  /* set item void bit */
            }
            /***
            if (UifRegData.regsales.ulUnitPrice) {      / price key /
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.usDeptNo) {
                return(LDT_SEQERR_ADR);
            }
            ***/
            _tcsncpy(UifRegData.regdisc.aszNumber, aszUifRegNumber, NUM_NUMBER);

            UifRegData.regdisc.uchMajorClass = CLASS_UIFREGDISC;    /* set item discount major class */
            UifRegData.regdisc.uchMinorClass = CLASS_UIITEMDISC1;   /* set item discount minor class */
            UifRegData.regdisc.uchDiscountNo = pData->SEL.INPUT.uchMinor;   /* set which item discount requested */
            UifRegData.regdisc.lAmount = pData->SEL.INPUT.lData;  /* set input data */
            sRetStatus = ItemDiscount(&UifRegData.regdisc); /* Discount Modele */
            break;

        case FSC_TARE:                                              /* scale/tare, GCA */
            if ((sRetStatus = UifRegVoidNoCheck()) != SUCCESS) {    /* check item void or number */
                return(sRetStatus);
            }
            if ((sRetStatus = UifRegInputCheck(pData, UIFREG_MAX_DIGIT2)) != SUCCESS) {  /* check input data */
                return(sRetStatus);
            }
            if (UifRegData.regsales.lQTY != 0L) {                   /* input qty ? */
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.uchAdjective != 0) {            /* input adjective ? */
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.uchMinorClass == CLASS_UIMODDISC) {  /* input modifiered discount ? */
                return(LDT_SEQERR_ADR);
            }
            if (pData->SEL.INPUT.lData == 0L && pData->SEL.INPUT.uchLen != 0) {
                return(LDT_SEQERR_ADR);
            }
            if (fchUifSalesMod & EVER_MODIFIER) {
                return(LDT_SEQERR_ADR);
            }
            if (fchUifSalesMod & PCHK_MODIFIER) {
                return(LDT_SEQERR_ADR);
            }
            memset( &regmodifier, 0, sizeof( UIFREGMODIFIER ) );
            regmodifier.uchMajorClass = CLASS_UIFREGMODIFIER;       /* set class */
            regmodifier.uchMinorClass = CLASS_UITARE;
            regmodifier.lAmount = pData->SEL.INPUT.lData;           /* set input tare */
            if ( ( sRetStatus = ItemModTare( &regmodifier ) ) != UIF_SUCCESS ) {
                return(sRetStatus);
            }
            flUifRegStatus |= UIFREG_MODIFIER;                      /* set modifier status */
            return(SUCCESS);                                        /* GCA */

        case FSC_ADJECTIVE:
            if (!pData->SEL.INPUT.uchMinor || (pData->SEL.INPUT.uchMinor > FSC_ADJ_MAX)) {  /* check adjectove # */
                return(LDT_PROHBT_ADR);
            }
            if (pData->SEL.INPUT.uchLen != 0) {             /* exist input data ? */
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.uchAdjective != 0) {    /* input adjective ? */
                return(LDT_SEQERR_ADR);
            }

			// If we are a forced Order Declare then we need to save this
			// adjective information until after the Order Declare is processed
			// then we can process the adjective and the item.
			// We do the basic error checks above first because we don't want to
			// save bad data.
			if((TranModeQualPtr->auchCasStatus[CAS_CASHIERSTATUS_2] & CAS_ORDER_DEC_FORCE) != 0 && (TranCurQualPtr->fsCurStatus2 & CURQUAL_ORDERDEC_DECLARED) == 0)
			{
				UieCopyKeyMsg();
			}

            UifRegData.regsales.uchAdjective = pData->SEL.INPUT.uchMinor;  /* set adjectove # */
            flUifRegStatus |= UIFREG_MODIFIER;          /* set modifier status */
            UifDisp.uchMajorClass = CLASS_UIFREGDISP;   /* set display class */
            UifDisp.uchMinorClass = CLASS_UIFDISP_ADJ;
            UifDisp.uchFsc = pData->SEL.INPUT.uchMinor; /* set fsc */
            DispWrite(&UifDisp);                        /* display */
            return(SUCCESS);

        case FSC_QTY:
            if (UifRegData.regsales.lQTY != 0L) {    /* input qty ? */
                return(LDT_SEQERR_ADR);
            }
            if (pData->SEL.INPUT.uchLen == 0) {     /* w/o amount ? */
                return(LDT_SEQERR_ADR);
            }
            if (pData->SEL.INPUT.lData == 0L) {     /* '0' input ? */
                return(LDT_KEYOVER_ADR);
            }
            if (pData->SEL.INPUT.uchDec != 0xFF) {       /* input decimal point ? */
                if ((pData->SEL.INPUT.uchLen - pData->SEL.INPUT.uchDec - 1) > UIFREG_MAX_DIGIT3) {  /* input digit over ? */
                    return(LDT_KEYOVER_ADR);
                }
                if (pData->SEL.INPUT.uchDec > UIFREG_MAX_DIGIT3) {  /* input digit over ? */
                    return(LDT_KEYOVER_ADR);
                }

                UifRegData.regsales.lQTY = UifRegShiftQty((LONG)pData->SEL.INPUT.lData, pData->SEL.INPUT.uchDec);
                                                        /* set input amount to qty */
            } else {
                if (pData->SEL.INPUT.uchLen > UIFREG_MAX_DIGIT3) {  /* input digit over ? */
                    return(LDT_KEYOVER_ADR);
                }
                UifRegData.regsales.lQTY = (LONG)pData->SEL.INPUT.lData * 1000L;
                                                        /* set input amount to qty */
            }

			// If we are a forced Order Declare then we need to save this
			// quantity information until after the Order Declare is processed
			// then we can process the quantity and the item.
			// We do the basic error checks above first because we don't want to
			// save bad data.
			if((TranModeQualPtr->auchCasStatus[CAS_CASHIERSTATUS_2] & CAS_ORDER_DEC_FORCE) != 0 && (TranCurQualPtr->fsCurStatus2 & CURQUAL_ORDERDEC_DECLARED) == 0)
			{
				UieCopyKeyMsg();
			}

            flUifRegStatus |= UIFREG_MODIFIER;          /* set modifier status */
            UifDisp.uchMajorClass = CLASS_UIFREGDISP;   /* set display class */
            UifDisp.uchMinorClass = CLASS_UIFDISP_QTY;
            UifDisp.lAmount = UifRegData.regsales.lQTY;
            DispWrite(&UifDisp);                        /* display */
            ItemOtherDifDisableStatus();        /* R3.1 for Beverage Dispenser */
            return(SUCCESS);

        case FSC_KEYED_COUPON:
            UifRegData.regcoupon.uchMajorClass = CLASS_UIFREGCOUPON;
            UifRegData.regcoupon.uchMinorClass = CLASS_UICOMCOUPON;
            if (!pData->SEL.INPUT.uchMinor || (pData->SEL.INPUT.uchMinor > FLEX_CPN_MAX)) {
                return(LDT_PROHBT_ADR);
            }
            if (pData->SEL.INPUT.uchLen != 0) {     /* with amount ? */
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.lQTY != 0L) {       /* input qty ? */
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.uchAdjective != 0) {    /* input adjective ? */
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.uchMinorClass == CLASS_UIMODDISC) {  /* input modifiered discount ? */
                return(LDT_SEQERR_ADR);
            }
            /**** V1.0.04 ***/
            if (UifRegData.regsales.usDeptNo) {
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.lUnitPrice) {      /* price key */
                return(LDT_SEQERR_ADR);
            }
            /******/
            if (uchUifRegVoid != 0) {                   /* input item void ? */
                UifRegData.regcoupon.fbModifier |= VOID_MODIFIER;  /* set item void bit */
            }
            if (fchUifSalesMod & EVER_MODIFIER) {
                return(LDT_SEQERR_ADR);
            }
            if (fchUifSalesMod & PCHK_MODIFIER) {
                return(LDT_SEQERR_ADR);
            }
            /***
            if (UifRegData.regsales.usDeptNo) {
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.ulUnitPrice) {      / price key /
                return(LDT_SEQERR_ADR);
            }
            ***/
            _tcsncpy(UifRegData.regcoupon.aszNumber, aszUifRegNumber, NUM_NUMBER);

            UifRegData.regcoupon.uchFSC = pData->SEL.INPUT.uchMinor; /* set coupon extend FSC */
            sRetStatus = ItemCoupon(&UifRegData.regcoupon);    /* Coupon Modele */
            break;

        case FSC_COUPON:
            UifRegData.regcoupon.uchMajorClass = CLASS_UIFREGCOUPON;
            if (pData->SEL.INPUT.lData > FLEX_CPN_MAX) {
                return(LDT_KEYOVER_ADR);
            }
            if (pData->SEL.INPUT.uchLen != 0 && pData->SEL.INPUT.lData == 0L) {
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.lQTY != 0L) {       /* input qty ? */
                return(LDT_SEQERR_ADR);
            }
            /*** V1.0.04 ***/
            if (UifRegData.regsales.usDeptNo) {
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.lUnitPrice) {      /* price key */
                return(LDT_SEQERR_ADR);
            }
            /****/
            if (uchUifRegVoid != 0) {                   /* input item void ? */
                UifRegData.regcoupon.fbModifier |= VOID_MODIFIER;  /* set item void bit */
            }
            if (UifRegData.regsales.uchAdjective != 0) {    /* input adjective ? */
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.uchMinorClass == CLASS_UIMODDISC) {  /* input modifiered discount ? */
                return(LDT_SEQERR_ADR);
            }
            if (fchUifSalesMod & EVER_MODIFIER) {
                return(LDT_SEQERR_ADR);
            }
            if (fchUifSalesMod & PCHK_MODIFIER) {
                return(LDT_SEQERR_ADR);
            }
            /***
            if (UifRegData.regsales.usDeptNo) {
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.ulUnitPrice) {      / price key /
                return(LDT_SEQERR_ADR);
            }
            ***/
            _tcsncpy(UifRegData.regcoupon.aszNumber, aszUifRegNumber, NUM_NUMBER);

            if (pData->SEL.INPUT.uchLen == 0) {
                if (uchUifRegVoid != 0
                    || UifRegData.regsales.lQTY != 0
                    || UifRegData.regsales.uchAdjective != 0
                    || UifRegData.regsales.aszNumber[0] != 0) {  /* R3.0 */
                    return(LDT_SEQERR_ADR);
                }
                UifRegData.regcoupon.uchMinorClass = CLASS_UICOMCOUPONREPEAT;
            } else {
                UifRegData.regcoupon.uchMinorClass = CLASS_UICOMCOUPON;
            }
			// coupon number is between 1 and FLEX_CPN_MAX and range is checked above.
            UifRegData.regcoupon.uchFSC = (USHORT)(pData->SEL.INPUT.lData);  /* set coupon # */
            sRetStatus = ItemCoupon(&UifRegData.regcoupon);   /* Coupon Modele */
            break;

        case FSC_REPEAT:                                      /* R3.1 */
            /*---- allow qty entry for cursor repeat ----*/
            if (pData->SEL.INPUT.uchLen) {              /* exist input data ? */
                if (UifRegData.regsales.lQTY) {         /* input qty ? */
                    return(LDT_SEQERR_ADR);
                }

                if ((sRetStatus = UifRegInputCheck(pData, UIFREG_MAX_DIGIT3)) != SUCCESS) {
                    return(sRetStatus);
                }
                UifRegData.regsales.lQTY = (LONG)pData->SEL.INPUT.lData * 1000L;
                                                        /* set input amount to qty */
            }
            if ((sRetStatus = UifRegVoidNoCheck()) != SUCCESS) {  /* check item void or number */
                return(sRetStatus);
            }
            if (UifRegData.regsales.uchAdjective != 0) {    /* input adjective ? */
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.uchMinorClass == CLASS_UIMODDISC) {  /* input modifiered discount ? */
                return(LDT_SEQERR_ADR);
            }
            if (fchUifSalesMod & EVER_MODIFIER) {
                return(LDT_SEQERR_ADR);
            }
            if (fchUifSalesMod & PCHK_MODIFIER) {
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.usDeptNo) {
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.lUnitPrice) {      /* price key */
                return(LDT_SEQERR_ADR);
            }
            UifRegData.regsales.uchMinorClass = CLASS_UIREPEAT;
            sRetStatus = ItemSales(&UifRegData.regsales);
            break;

        case FSC_PRICE_CHECK:                                 /* R3.1 */
            if (pData->SEL.INPUT.uchLen != 0) {               /* exist input data ? */
                return(LDT_SEQERR_ADR);
            }
            if (flUifRegStatus & UIFREG_MODIFIER) {           /* modifier status */
                return(LDT_SEQERR_ADR);
            }
            if ((sRetStatus = UifRegVoidNoCheck()) != SUCCESS) {  /* check item void or number */
                return(sRetStatus);
            }
            if (UifRegData.regsales.lQTY != 0L) {       /* input qty ? */
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.uchAdjective != 0) {    /* input adjective ? */
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.uchMinorClass == CLASS_UIMODDISC) {  /* input modifiered discount ? */
                return(LDT_SEQERR_ADR);
            }
            if (fchUifSalesMod & PCHK_MODIFIER) {
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.usDeptNo) {
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.lUnitPrice) {      /* price key */
                return(LDT_SEQERR_ADR);
            }
            UifRegData.regsales.uchMinorClass = CLASS_UIPRICECHECK;
            sRetStatus = ItemSales(&UifRegData.regsales);
            /* 2172 */
            if (sRetStatus == LDT_CANCEL_ADR) {
                flUifRegStatus |= UIFREG_MODIFIER;          /* set modifier status */
                fchUifSalesMod |= PCHK_MODIFIER;
                UifDisp.uchMajorClass = CLASS_UIFREGDISP;   /* set display class */
                UifDisp.uchMinorClass = CLASS_UIFDISP_PRICECHECK;
                UifDisp.lAmount = 0L;
                DispWrite(&UifDisp);                        /* display */
                ItemOtherDifDisableStatus();        /* R3.1 for Beverage Dispenser */
                return(SUCCESS);
            }
            break;

        case FSC_DEPT:  /* 2172 */
            if ((sRetStatus = UifRegInputCheck(pData, UIFREG_MAX_DIGIT4)) != SUCCESS) {  /* input data check */
                return(sRetStatus);
            }
            if (uchUifRegVoid != 0) {                   /* input item void ? */
                UifRegData.regsales.fbModifier |= VOID_MODIFIER;  /* set item void bit */
            }
            if (fchUifSalesMod & EVER_MODIFIER) {
                return(LDT_SEQERR_ADR);
            }
            if (fchUifSalesMod & PCHK_MODIFIER) {
                return(LDT_SEQERR_ADR);
            }
            if(UifRegData.regsales.fbInputStatus & INPUT_DEPT) {
                return(LDT_SEQERR_ADR);
            }
            _tcsncpy(UifRegData.regsales.aszNumber, aszUifRegNumber, NUM_NUMBER);

            if (UifRegData.regsales.uchMinorClass != CLASS_UIMODDISC) {  /* input modifiered discount ? */
                if (pData->SEL.INPUT.uchLen == 0) {         /* repeat ? */
                    if (uchUifRegVoid != 0
                        || UifRegData.regsales.lQTY != 0
                        || UifRegData.regsales.uchAdjective != 0
                        || UifRegData.regsales.aszNumber[0] != 0) {  /* R3.0 */
                        return(LDT_SEQERR_ADR);
                    }
                    UifRegData.regsales.uchMinorClass = CLASS_UIDEPTREPEAT; /* set plu repeat minor class */
                    UifRegData.regsales.uchFsc = FSC_DEPT;
                } else {
                    UifRegData.regsales.uchMinorClass = CLASS_UIDEPT;    /* set plu minor class */
                }
            }
			sRet = ItemSalesOrderDeclaration(UifRegData.regsales,0, CLASS_ORDER_DEC_NEW);
			if(sRet != SUCCESS)
			{
				return sRet;
			}

            UifRegData.regsales.uchMajorFsc = pData->SEL.INPUT.uchMajor; /* set dept fsc */
            UifRegData.regsales.usDeptNo = (USHORT)pData->SEL.INPUT.lData;  /* set PLU # */
			//SR 143 @/For Key cwunn 8/20
			//If previous item was an @/For entry,
			// get unit price, quantity, etc and send to processing function
			ItemPreviousGetForKeyData(&UifRegData.regsales);
			//END SR 143

            if ((UifRegData.regsales.lUnitPrice) ||
                (UifRegData.regsales.fbInputStatus & INPUT_0_ONLY) ||
                (UifRegData.regsales.uchMinorClass == CLASS_UIDEPTREPEAT))
			{
                sRetStatus = ItemSales(&UifRegData.regsales); /* Sales Modele */
				if((sRetStatus == UIF_SUCCESS) && sForKeyPressed)
				{
					ItemPreviousClearForKeyData();    // Clear @/For data for a FSC_DEPT entry
					sForKeyPressed = FALSE;
				}
            } else {
                UifRegData.regsales.usDeptNo =(USHORT)pData->SEL.INPUT.lData;
                UifRegData.regsales.fbInputStatus |= INPUT_DEPT;  /* set dept input status */
				_tcsncpy(UifRegData.regsales.aszNumber, aszUifRegNumber, NUM_NUMBER);
				UifRegData.regsales.lUnitPrice = 0;  /* set input amount */
				UifRegData.regsales.uchMajorFsc = FSC_DEPT; /* set dept fsc */

				sRetStatus = ItemSales(&UifRegData.regsales); /* Sales Modele */
				if((sRetStatus == UIF_SUCCESS) && sForKeyPressed)
				{
					ItemPreviousClearForKeyData();    // Clear @/For data for a FSC_PRICE_ENTER entry
					sForKeyPressed = FALSE;
				}
            }
            break;

        /* --- Price Enter key ---, 2172 */
        case FSC_PRICE_ENTER:
            if (pData->SEL.INPUT.uchDec != 0xFF) {          /* input decimal point ? */
                return(LDT_KEYOVER_ADR);
            }
            if (pData->SEL.INPUT.uchLen > UIFREG_MAX_AMT_DIGITS) {  /* input digit over ? */
                return(LDT_KEYOVER_ADR);
            }
            if (UifRegData.regsales.uchAdjective != 0) {    /* input adjective key ? */
                return(LDT_SEQERR_ADR);
            }
            if (uchUifRegVoid != 0) {                   /* input item void ? */
                UifRegData.regsales.fbModifier |= VOID_MODIFIER;  /* set item void bit */
            }
            if (fchUifSalesMod & EVER_MODIFIER) {
                return(LDT_SEQERR_ADR);
            }
            if (fchUifSalesMod & PCHK_MODIFIER) {
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.lUnitPrice) {
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.fbInputStatus & INPUT_0_ONLY) {  /* set '0' input status */
                return(LDT_SEQERR_ADR);
            }

            _tcsncpy(UifRegData.regsales.aszNumber, aszUifRegNumber, NUM_NUMBER);

            if (pData->SEL.INPUT.uchLen == 0) {         /* repeat ? */
                return(LDT_SEQERR_ADR);
            }

			// If we are a forced Order Declare then we need to save this
			// price information until after the Order Declare is processed
			// then we can process the price and the item.
			// We do the basic error checks above first because we don't want to
			// save bad data.
			if((TranModeQualPtr->auchCasStatus[CAS_CASHIERSTATUS_2] & CAS_ORDER_DEC_FORCE) != 0 && (TranCurQualPtr->fsCurStatus2 & CURQUAL_ORDERDEC_DECLARED) == 0)
			{
				UieCopyKeyMsg();
			}

            if (UifRegData.regsales.uchMinorClass != CLASS_UIMODDISC) {  /* input modifiered discount ? */
                UifRegData.regsales.uchMinorClass = CLASS_UIDEPT;    /* set plu minor class */
            }
            UifRegData.regsales.lUnitPrice = (DUNITPRICE)pData->SEL.INPUT.lData;  /* set input amount */
            if ((pData->SEL.INPUT.uchLen != 0) && (pData->SEL.INPUT.lData == 0L)) {
                                                        /* '0' input ? */
                UifRegData.regsales.fbInputStatus |= INPUT_0_ONLY;  /* set '0' input status */
            }
            /* --- if Dept No. is not entered --- */
            if(!(UifRegData.regsales.fbInputStatus & INPUT_DEPT)) {
				// If a department number has not been entered before the price entry
				// then we need trigger a request for a department number as part of
				// processing this price entry.
				// Normally price entry is done after entering a department number.


                ItemOtherDifDisableStatus();        /* R3.1 for Beverage Dispenser */
                flUifRegStatus |= UIFREG_MODIFIER;  /* set modifier status */
				UifRegData.regsales.fbInputStatus |= INPUT_PRICE;
			}
			else {
				UifRegData.regsales.uchMajorFsc = FSC_DEPT; /* set dept fsc */
			}
            sRetStatus = ItemSales(&UifRegData.regsales); /* Sales Modele */

			if((sRetStatus == UIF_SUCCESS) && sForKeyPressed)
			{
				ItemPreviousClearForKeyData();    // Clear @/For data for a FSC_PRICE_ENTER entry
				sForKeyPressed = FALSE;
			}

            break;

        /* --- E Version Modifier key 2172 --- */
        case FSC_E_VERSION:
            /* --- set E Version status bit --- */
            fchUifSalesMod |= EVER_MODIFIER;
            flUifRegStatus |= UIFREG_MODIFIER;      /* set modifier status */

            /* --- setup "E Version Mod." display data class --- */
            UifDisp.uchMajorClass = CLASS_UIFREGDISP;
            UifDisp.uchMinorClass = CLASS_UIFDISP_E_VERSION;
            if (pData->SEL.INPUT.uchLen > 0) {
                memcpy(UifDisp.aszStringData, pData->SEL.INPUT.aszKey, pData->SEL.INPUT.uchLen);   /* set input data */
            }

            /* --- execute Display Module --- */
            DispWrite(&UifDisp);

            /* --- keep display message --- */
            UieKeepMessage();
            ItemOtherDifDisableStatus();        /* R3.1 for Beverage Dispenser */

            /* --- exit, return SUCCESS status --- */
            return(SUCCESS);
            break;

        case FSC_VIRTUALKEYEVENT:
			if (pData->SEL.INPUT.DEV.VIRTUALKEY.usKeyEventType != UIE_VIRTUALKEY_EVENT_SCANNER) {
				return(LDT_KEYOVER_ADR);
			}
			else {
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
					tempbuf.uchStrLen++;
				}
				else if (tempbuf.uchStrLen == 14) {
					// label is LABEL_RSS14 with length of LEN_RSS14
					tempbuf.auchStr[0] = _T(']');
					tempbuf.uchStrLen++;
				}
				else if (tempbuf.uchStrLen == 8) {
					// label is LABEL_EAN8 with length of LEN_EAN8
					tempbuf.auchStr[0] = _T('B');
					tempbuf.uchStrLen++;
				}
				else if (tempbuf.uchStrLen == 6) {
					// label is LABEL_UPCE with length of LEN_UPCE
					tempbuf.auchStr[0] = _T('C');
					tempbuf.uchStrLen++;
				}
				else {
					// not a known type of bar code so just put things back.
					tempbuf = pData->SEL.INPUT.DEV.VIRTUALKEY.u.scanner_data;
					uchMajor = pData->SEL.INPUT.uchMajor;
				}
				pData->SEL.INPUT.DEV.SCA = tempbuf;
				pData->SEL.INPUT.uchMajor = uchMajor;
			}
//            break;  // drop through to the scanner code.

        /* --- Scanner, 2172 --- */
        case FSC_SCANNER:
			// Two kinds of scanners are supported. The first is the older NCR scanner or scanner/scale
			// which is a legacy from the NCR 2170 and NCR 7448 with NHPOS. The second is the newer
			// handheld keyboard wedge type of scanner such as the Symbol hand held scanner.
			//
			// For older NCR scanner or scanner/scale see CUieScan::UieScannerThread() which contains the
			// necessary logic to read in from a COM port scanner data and to then parse it in order to
			// read the bar code digits.
			//
			// For newer USB keyboard wedge type scanners such as the Symbol, that logic is in 
			// function CDeviceEngine::ConvertKey(). See also UIE_VIRTUALKEY_EVENT_SCANNER
			//
			// For the older NCR scanner, the scanner must be provisioned to transform the raw digits of
			// the bar code label into an acceptable format for the CUieScan class. This means that:
			//   - Enable SANYO ID Characters must be turned on (analyses bar code as to type and adds letter prefix)
			//   - provide a single prefix character with the STX (0x02) or Start of Transmission character
			//   - provide a single suffix character with the ETX (0x03) or End of Transmission character
			//
			// For the newer USB keyboard wedge scanner, the scanner must be provisioned to provide a prefix
			// character and a suffix character so that the keyboard wedge functionality can isolate the bar
			// code digits from the incoming stream of data.
			//   - provide a single prefix character of capital letter M (0x4D)
			//   - provide a single suffic character of carriage return (0x0D)

            /* --- if indexed data is entered --- */
            if (pData->SEL.INPUT.uchLen != 0) {
                if (UifRegData.regsales.lQTY != 0L) {    /* input qty ? */
                    /* --- return sequence error message number --- */
                    return(LDT_SEQERR_ADR);
                }
            }

            /* --- check scanned data length --- */
            if (pData->SEL.INPUT.DEV.SCA.uchStrLen > UIFREG_MAX_DIGIT77/*RPH RSS14 UIFREG_MAX_DIGIT13+1*/) {
                /* --- return lead thru message number --- */
                PifBeep(1000);
				return(LDT_KEYOVER_ADR);
            }
            if (UifRegData.regsales.lUnitPrice) {      /* price key */
                return(LDT_SEQERR_ADR);
            }
            if (uchUifRegVoid != 0) {                   /* input item void ? */
                UifRegData.regsales.fbModifier |= VOID_MODIFIER;  /* set item void bit */
            }
            if (fchUifSalesMod & EVER_MODIFIER) {
                return(LDT_SEQERR_ADR);
            }
            if (fchUifSalesMod & PCHK_MODIFIER) {
                UifRegData.regsales.fbModifier2 |= PCHK_MODIFIER;  /* set item void bit */
            }
            if (UifRegData.regsales.usDeptNo) {
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.lUnitPrice) {      /* price key */
                return(LDT_SEQERR_ADR);
            }
			sRet = ItemSalesOrderDeclaration(UifRegData.regsales,0, CLASS_ORDER_DEC_NEW);
			if(sRet != SUCCESS)
			{
				return sRet;
			}
            _tcsncpy(UifRegData.regsales.aszNumber, aszUifRegNumber, NUM_NUMBER);

            if (UifRegData.regsales.uchMinorClass != CLASS_UIMODDISC) {  /* input modifiered discount ? */
                UifRegData.regsales.uchMinorClass = CLASS_UIPLU;    /* set plu minor class */
            }

            UifRegData.regsales.uchMajorFsc = pData->SEL.INPUT.uchMajor; // FSC_SCANNER; /* set plu fsc */
            _tcsncpy(UifRegData.regsales.aszPLUNo, pData->SEL.INPUT.DEV.SCA.auchStr, pData->SEL.INPUT.DEV.SCA.uchStrLen);

            if (pData->SEL.INPUT.uchLen != 0) {
                /* --- setup quantity value --- */

                if (pData->SEL.INPUT.uchDec != 0xFF) {       /* input decimal point ? */
                    if ((pData->SEL.INPUT.uchLen - pData->SEL.INPUT.uchDec - 1) > UIFREG_MAX_DIGIT3) {  /* input digit over ? */
                        return(LDT_KEYOVER_ADR);
                    }
                    if (pData->SEL.INPUT.uchDec > UIFREG_MAX_DIGIT3) {  /* input digit over ? */
                        return(LDT_KEYOVER_ADR);
                    }

                    UifRegData.regsales.lQTY = UifRegShiftQty((LONG)pData->SEL.INPUT.lData, pData->SEL.INPUT.uchDec);
                                                        /* set input amount to qty */
                } else {
                    if (pData->SEL.INPUT.uchLen > UIFREG_MAX_DIGIT3) {  /* input digit over ? */
                        return(LDT_KEYOVER_ADR);
                    }
                    UifRegData.regsales.lQTY = (LONG)pData->SEL.INPUT.lData * 1000L;
                                                        /* set input amount to qty */
                }
            }

            sRetStatus = ItemSales(&UifRegData.regsales); /* Sales Modele */

			if((sRetStatus == UIF_SUCCESS) && sForKeyPressed)
			{
				ItemPreviousClearForKeyData();    // Clear any @/For data for FSC_SCANNER entry
				sForKeyPressed = FALSE;
			}

            if ((sRetStatus == UIF_NUMBER) ||
                (sRetStatus == UIF_SKUNUMBER)) {

                /* scanned label is number */
                if (aszUifRegNumber[0] != 0) {         /* already input number ? */
                    return(LDT_SEQERR_ADR);
                }
                if (sRetStatus == UIF_SKUNUMBER) {
                    UifRegData.regsales.fbModifier2 |= SKU_MODIFIER;
                }
                /* set number */
                _tcsncpy(aszUifRegNumber, UifRegData.regsales.aszNumber, NUM_NUMBER);

                flUifRegStatus |= UIFREG_MODIFIER;      /* set modifier status */
                UifDisp.uchMajorClass = CLASS_UIFREGDISP;   /* set display class */
                if (sRetStatus == UIF_SKUNUMBER) {
                    UifDisp.uchMinorClass = CLASS_UIFDISP_SKUNUMBER;
                } else {
                    UifDisp.uchMinorClass = CLASS_UIFDISP_NUMBER;
                }
                _tcsncpy(UifDisp.aszStringData, aszUifRegNumber, NUM_NUMBER);

                DispWrite(&UifDisp);                    /* display */
                /* clear plu no. for next entry */
                memset(UifRegData.regsales.aszPLUNo, 0, sizeof(UifRegData.regsales.aszPLUNo));
                return (SUCCESS);
            }
            break;

        case FSC_ASK:                                   /* Ask Key, Saratoga */
			if (pData->SEL.INPUT.uchMinor == ITM_ASK_RETURNS) {
				//  This functionality is for the Returns functionality.  The guest check
				//  data may be loaded either from the Connection Engine Interface or from
				//  the Guest Check File depending on the setting of MDC_RECEIPT_RTN_ADR, MDC_PARAM_BIT_D
				if ((sRetStatus = CheckAllowFunction()) != SUCCESS) {
					UieEmptyRingBufClearMacro();
					return sRetStatus;
				}

				if (pData->SEL.INPUT.lData < 1) {
					ItemTransDispGCFNo(TRN_GCNO_RETURNS);
					do {
						UIFDIADATA  WorkUI = {0};

						if (UifDiaGuestNo(&WorkUI) != UIF_SUCCESS) {
							UifRegWorkClear();                          /* clear work area */
							flUifRegStatus |= UIFREG_MODIFIER;          /* set modifier status for display clear*/
							RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGOTHER, CLASS_UICLEAR, DISP_SAVE_TYPE_0);

							return(UIF_CANCEL);
						}       

						pData->SEL.INPUT.uchMajor = FSC_SCANNER;
						pData->SEL.INPUT.uchMinor = 0;
						_tcsncpy (pData->SEL.INPUT.DEV.SCA.auchStr, WorkUI.auchTrack2, TCHARSIZEOF(pData->SEL.INPUT.DEV.SCA.auchStr));
						pData->SEL.INPUT.lData = WorkUI.ulData;
						if (pData->SEL.INPUT.lData < 1)
							UieErrorMsg(LDT_KEYOVER_ADR, UIE_WITHOUT);
					} while (pData->SEL.INPUT.lData < 1);
				}

				memset(&(UifRegData.regtender), 0, sizeof(UifRegData.regtender));

				UifRegData.regtender.uchMajorClass = CLASS_UIFREGTENDER;    /* set tender class */
				UifRegData.regtender.uchMinorClass = pData->SEL.INPUT.uchMinor;
				UifRegData.regtender.lTenderAmount = (LONG)pData->SEL.INPUT.lData;  /* set input amount */

				sRetStatus = ItemReturnsLoadGuestCheck (pData);

				if (sRetStatus < 0) {             /* cancel ? */
					// If there was some kind of an error then we will perform a cancel of
					// the transaction that was started and then report the original
					// error code for display in an error message.
					SHORT  sRetStatusCancel;

					UifRegData.regmisc.uchMajorClass = CLASS_UIFREGMISC;  /* set cancel class */
					UifRegData.regmisc.uchMinorClass = CLASS_UICANCEL;
					sRetStatusCancel = ItemMiscCancel(&UifRegData.regmisc);     /* Misc Modele */
					UifRegWorkClear();                          /* clear work area */
					flUifRegStatus |= UIFREG_MODIFIER;          /* set modifier status for display clear*/
					UieEmptyRingBufClearMacro();
					return sRetStatus;
				}
				break;
			} else if (pData->SEL.INPUT.uchMinor == ITM_ASK_PREAUTH) {
				// This is the implementation of Preauth Batch Settle (Preauth Capture) for Amtrak using the Ask #7 keys
				// The Ask #7 display list of Preauths needing capture requires that an OEP Window exists
				// in the layout which contains OEP Group number 201 (BL_OEPGROUP_STR_PREAUTH).
				if ((sRetStatus = CheckAllowFunction()) != SUCCESS) {
					UieEmptyRingBufClearMacro();
					return sRetStatus;
				}

				sRetStatus = LDT_PROHBT_ADR;
				break;
			} else if (pData->SEL.INPUT.uchMinor == ITM_ASK_PREAUTH_BY_GCN) {
				// This is the implementation of Preauth Batch Settle (Preauth Capture) for Amtrak using the Ask #12 keys
				if ((sRetStatus = CheckAllowFunction()) != SUCCESS) {
					UieEmptyRingBufClearMacro();
					return sRetStatus;
				}

				if (pData->SEL.INPUT.lData < 1) {
					ItemTransDispGCFNo(TRN_GCNO_RETURNS);
					do {
						UIFDIADATA  WorkUI = {0};

						if (UifDiaGuestNo(&WorkUI) != UIF_SUCCESS) {
							UifRegWorkClear();                          /* clear work area */
							flUifRegStatus |= UIFREG_MODIFIER;          /* set modifier status for display clear*/
							RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGOTHER, CLASS_UICLEAR, DISP_SAVE_TYPE_0);

							return(UIF_CANCEL);
						}       

						pData->SEL.INPUT.uchMajor = FSC_SCANNER;
						pData->SEL.INPUT.uchMinor = 0;
						_tcsncpy (pData->SEL.INPUT.DEV.SCA.auchStr, WorkUI.auchTrack2, TCHARSIZEOF(pData->SEL.INPUT.DEV.SCA.auchStr));
						pData->SEL.INPUT.lData = WorkUI.ulData;
						if (pData->SEL.INPUT.lData < 1)
							UieErrorMsg(LDT_KEYOVER_ADR, UIE_WITHOUT);
					} while (pData->SEL.INPUT.lData < 1);
				}

				memset(&(UifRegData.regtender), 0, sizeof(UifRegData.regtender));

				UifRegData.regtender.uchMajorClass = CLASS_UIFREGTENDER;    /* set tender class */
				UifRegData.regtender.uchMinorClass = pData->SEL.INPUT.uchMinor;
				UifRegData.regtender.lTenderAmount = (LONG)pData->SEL.INPUT.lData;  /* set input amount */

				sRetStatus = LDT_SEQERR_ADR;
				if (sRetStatus > 0)
					return sRetStatus;
				break;
			} else if (pData->SEL.INPUT.uchMinor == ITM_ASK_OPENCHECKS_BY_OPID || pData->SEL.INPUT.uchMinor == ITM_ASK_OPENCHECKS_BY_GCN) {
				// This is the implementation of display open checks by operator or table using the Ask #14 keys
				// This is the implementation of fetch a guest check that has been serviced using an
				// external style of bar code as for Amtrak.
				if ((sRetStatus = CheckAllowFunction()) != SUCCESS) {
					UieEmptyRingBufClearMacro();
					return sRetStatus;
				}

				if (pData->SEL.INPUT.uchMinor == ITM_ASK_OPENCHECKS_BY_OPID) {
					// The Ask #14 display open guest checks requires that an OEP Window exists
					// in the layout which contains OEP Group number 204 (BL_OEPGROUP_STR_OPENCHECK).
					if (pData->SEL.INPUT.uchLen < 1) {
						pData->SEL.INPUT.lData = TranModeQualPtr->ulCashierID;
					}
					sRetStatus = TrnDisplayFetchGuestCheck (_T("Open Checks"), pData->SEL.INPUT.lData);
				} else if (pData->SEL.INPUT.uchMinor == ITM_ASK_OPENCHECKS_BY_GCN) {
					if (pData->SEL.INPUT.lData < 1) {
						ItemTransDispGCFNo(TRN_GCNO_RETURNS);
						do {
							UIFDIADATA  WorkUI = {0};

							if (UifDiaGuestNo(&WorkUI) != UIF_SUCCESS) {
								UifRegWorkClear();                          /* clear work area */
								flUifRegStatus |= UIFREG_MODIFIER;          /* set modifier status for display clear*/
								RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGOTHER, CLASS_UICLEAR, DISP_SAVE_TYPE_0);

								return(UIF_CANCEL);
							}       

							pData->SEL.INPUT.uchMajor = FSC_SCANNER;
							pData->SEL.INPUT.uchMinor = 0;
							_tcsncpy (pData->SEL.INPUT.DEV.SCA.auchStr, WorkUI.auchTrack2, TCHARSIZEOF(pData->SEL.INPUT.DEV.SCA.auchStr));
							pData->SEL.INPUT.lData = WorkUI.ulData;
							if (pData->SEL.INPUT.lData < 1)
								UieErrorMsg(LDT_KEYOVER_ADR, UIE_WITHOUT);
						} while (pData->SEL.INPUT.lData < 1);
					}

					sRetStatus = TrnFetchServicedGuestCheck (pData);
				}

				if (sRetStatus < 1) {
					if (sRetStatus == UIF_CANCEL || sRetStatus == UIF_DELIV_RECALL) {
						UieReject();                            /* send reject to parent */
						break;
					}
					return LDT_ILLEGLNO_ADR;
				} else {
					memset(&(UifRegData.regtender), 0, sizeof(UifRegData.regtender));

					UifRegData.regtransopen.uchMajorClass = CLASS_UIFREGTRANSOPEN;  /* set recall class */
					UifRegData.regtransopen.uchMinorClass = CLASS_UINEWKEYSEQ;      /* V3.3 */
					UifRegData.regtransopen.ulAmountTransOpen = (ULONG)sRetStatus;  /* set guest check number */
					sRetStatus = ItemTransOpen(&UifRegData.regtransopen);   /* Transaction Open Modele */
					UifRegWorkClear();                              /* clear work area */
					if (sRetStatus == UIF_CANCEL || sRetStatus == UIF_DELIV_RECALL) {
						UieReject();                            /* send reject to parent */
						break;
					}
					if (sRetStatus == LDT_TAKETL_ADR) {         /* buffer full ? */
						flUifRegStatus |= UIFREG_BUFFERFULL;    /* set buffer full status */
						UieAccept();                            /* send accepted to parent */
						return(sRetStatus);
					}
					if (sRetStatus != UIF_SUCCESS) {            /* error ? */
						return(sRetStatus);
					}
				}
				break;
			}

			if (pData->SEL.INPUT.uchMinor >= ITM_ASK_CHECK_NO_PUR_1 && pData->SEL.INPUT.uchMinor <= ITM_ASK_CHECK_NO_PUR_3) {
				// This is the implementation of Check No Purchase for VCS using the Ask #8, #9, and #10 keys
				// Must specify a tender amount.
				if (pData->SEL.INPUT.uchLen == 0) {
					return(LDT_SEQERR_ADR);
				}

				memset(&(UifRegData.regtender), 0, sizeof(UifRegData.regtender));

				UifRegData.regtender.uchMajorClass = CLASS_UIFREGTENDER;    /* set tender class */
				UifRegData.regtender.uchMinorClass = pData->SEL.INPUT.uchMinor;
				UifRegData.regtender.lTenderAmount = (LONG)pData->SEL.INPUT.lData;  /* set input amount */

				sRetStatus = ItemCheckNoPurchase (&(UifRegData.regtender));

				if (sRetStatus == UIF_CANCEL) {             /* cancel ? */
					UifRegData.regmisc.uchMajorClass = CLASS_UIFREGMISC;  /* set cancel class */
					UifRegData.regmisc.uchMinorClass = CLASS_UICANCEL;
					sRetStatus = ItemMiscCancel(&UifRegData.regmisc);     /* Misc Modele */
					UifRegWorkClear();                          /* clear work area */
					flUifRegStatus |= UIFREG_MODIFIER;          /* set modifier status for display clear*/
					UieReject();                            /* send reject to parent */
					return SUCCESS;
				}
				break;
			}

            if (pData->SEL.INPUT.uchLen != 0) {
                return(LDT_SEQERR_ADR);
            }
            if (uchUifRegVoid != 0 || fchUifSalesMod != 0) {
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.uchMinorClass == CLASS_UIMODDISC) {  /* input modifiered discount ? */
                return(LDT_SEQERR_ADR);
            }
            if (fchUifSalesMod & EVER_MODIFIER) {
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.usDeptNo) {
                return(LDT_SEQERR_ADR);
            }
            if (UifRegData.regsales.lUnitPrice) {      /* price key */
                return(LDT_SEQERR_ADR);
            }
            if (fchUifSalesMod & PCHK_MODIFIER) {
                return(LDT_SEQERR_ADR);
            }

			sRetStatus = CheckAllowFunction();		// Looks to see if transaction has been itemized
			if (sRetStatus == SUCCESS) {
				UifRegData.regother.uchMajorClass = CLASS_UIFREGOTHER;
				UifRegData.regother.uchMinorClass = CLASS_UIFASK;
				UifRegData.regother.uchFsc = pData->SEL.INPUT.uchMinor;
				sRetStatus = ItemOtherAsk(&UifRegData.regother);

				if (sRetStatus == UIF_CANCEL) {             /* cancel ? */
					UifRegData.regmisc.uchMajorClass = CLASS_UIFREGMISC;  /* set cancel class */
					UifRegData.regmisc.uchMinorClass = CLASS_UICANCEL;
					sRetStatus = ItemMiscCancel(&UifRegData.regmisc);     /* Misc Modele */
					UifRegWorkClear();                          /* clear work area */
					flUifRegStatus |= UIFREG_MODIFIER;          /* set modifier status for display clear*/
					UieReject();                            /* send reject to parent */
					return SUCCESS;
				}
				UifRegWorkClear();
				if ((pData->SEL.INPUT.uchMinor >= EXTFSC_ASK_REPORT16) && (pData->SEL.INPUT.uchMinor <= EXTFSC_ASK_REPORT20)) {
					if (sRetStatus == UIFREG_FINALIZE) {
						flUifRegStatus |= UIFREG_FINALIZE;
						BlFwIfDefaultUserScreen();				/* call function to bring up default window(s) */
						UieAccept();
					} else if (sRetStatus == UIF_CAS_SIGNOUT) {
						flUifRegStatus |= UIFREG_CASHIERSIGNOUT | UIFREG_FINALIZE;
						BlFwIfDefaultUserScreen();				/* call function to bring up default window(s) */
						UieAccept();
					} else if (sRetStatus == UIF_CANCEL) {
						UieReject();
						break;
					} else {
						return(sRetStatus);
					}
					break;
				} else {
					if (sRetStatus == UIF_SUCCESS) {
						break;
					} else if (sRetStatus == UIF_CANCEL) {
						UieReject();
						break;
					} else {
						return(sRetStatus);
					}
				}
			}
            break;
//Begin US Customs SCER cwunn 4/9/2003
        case FSC_P1:           // P1 key is same as Num Type key (FSC_NUM_TYPE) as used in several places so be consistent
		case FSC_NUM_TYPE:
			/* As per SCER issue request 5/21/03, remove data checking for #/type entries.
            if ((sRetStatus = UifRegInputCheckWOAmount(pData, STD_NUMBER_LEN)) != SUCCESS) {//US Customs SCER cwunn 4/8/03 increased digit #
                return(sRetStatus);
            }
			*/

			// check if user entered too many digits to fit into the data structs we use
			// if nothing was entered, uchLen < 0, we will prompt for the entry of a reference number.
            if (pData->SEL.INPUT.uchLen > STD_NUMBER_LEN) {
                return(LDT_KEYOVER_ADR);
            }

			//build display object and display on 2x20
			UifDisp.uchMajorClass = CLASS_UIFREGDISP;
			UifDisp.uchMinorClass = CLASS_UIFDISP_NUMBER;
			_tcsncpy(UifDisp.aszStringData, pData->SEL.INPUT.aszKey, STD_NUMBER_LEN);
            DispWrite(&UifDisp);                    // display

			{
				//UifRegData is an UIFREGSALES object, which does not align with ITEMSALES of ItemPrevItem
				//  So instead of using it, we must build an ITEMSALES object to pass.
				//build sales object for ItemPreviousItem to store #/Type entry in previous item
				ITEMSALES         itemSales = { 0 };	//required for call to ItemPreviousItem US Customs SCER cwunn 4/9/03

				itemSales.uchMajorClass = UifRegData.regsales.uchMajorClass = CLASS_ITEMSALES;
				itemSales.uchMinorClass = UifRegData.regsales.uchMinorClass = 0;
				_tcsncpy(UifRegData.regsales.aszNumber, pData->SEL.INPUT.aszKey, STD_NUMBER_LEN);
				_tcsncpy(itemSales.aszNumber[0], pData->SEL.INPUT.aszKey, STD_NUMBER_LEN);

				do {
					sRetStatus = ItemSalesCommonIF(&UifRegData.regsales, &itemSales);
					if (sRetStatus == LDT_KEYOVER_ADR) {
						extern USHORT  UifRegPauseStringNumber;
						SHORT sRetPromptEntry;

						UieErrorMsgExtended((USHORT)sRetStatus, UIE_WITHOUT, 0);
						sRetPromptEntry = ItemLinePausePromptEntry (UifRegPauseStringNumber, itemSales.aszNumber[0], 25);
						if (sRetPromptEntry == UIF_CANCEL) {
							sRetStatus = UIF_CANCEL;
						}
					}
				} while (sRetStatus == LDT_KEYOVER_ADR);
			}

			//return message if prohibited (too many entry attempts) otherwise continue
			if(sRetStatus == LDT_PROHBT_ADR)
				return sRetStatus;

			if (sRetStatus == UIF_CANCEL) {             /* cancel ? */
				UifRegData.regmisc.uchMajorClass = CLASS_UIFREGMISC;  /* set cancel class */
				UifRegData.regmisc.uchMinorClass = CLASS_UICANCEL;
				sRetStatus = ItemMiscCancel(&UifRegData.regmisc);     /* Misc Modele */
				UifRegWorkClear();                          /* clear work area */
				flUifRegStatus |= UIFREG_MODIFIER;          /* set modifier status for display clear*/
				UieReject();                            /* send reject to parent */
				return SUCCESS;
			}

			break;
//End US Customs SCER
			//SR 143 cwunn @/For Key port from 2170GP
        case FSC_FOR:
		{//new scope, to allow local variable declarations
			UIFREGSALES ForData = { 0 };

			// Ensure valid package size boundaries
			sRetStatus = UifRegWQtyCheck(pData, UIFREG_MAX_DIGIT3, 0);
			if (sRetStatus != SUCCESS) {
				return(sRetStatus);
			}
			// Ensure no quantity data exists
			if(!UifRegData.regsales.lQTY)
				return LDT_SEQERR_ADR;

			//Setup "FOR" Item Data Class
			ForData.uchMajorClass = CLASS_UIFREGOTHER;
			ForData.uchMinorClass = CLASS_UIFFOR;
			ForData.lQTY = pData->SEL.INPUT.lData;
			_tcsncpy(ForData.aszNumber, pData->SEL.INPUT.aszKey, pData->SEL.INPUT.uchLen);

			// Execute @/For module
			sRetStatus = ItemModFor(&ForData, &UifRegData.regsales);

			//capture cancel key message when returned from ItemModFor (user cancelled in UifDiaPrice)
			//if item's return status is unsuccessful
			if (sRetStatus != UIF_SUCCESS) {
				pData->SEL.INPUT.uchMajor = FSC_CANCEL;
				//Return Lead thru error message number --- */
				return(sRetStatus);
			}

			sForKeyPressed = TRUE;
	        break;
		}
//End SR 143
//SR 143 cwunn HALO override key port from 2170GP
		case FSC_HALO:
		{ //new scope, to allow local variable declarations
			REGDISPMSG   RegDisplay = { 0 };

			if(ParaMDCCheck(MDC_HALOOVR_ADR, EVEN_MDC_BIT0)){
				//Supervisor Intervention required to perform Halo Override
				sRetStatus = ItemSPVInt(LDT_SUPINTR_ADR);
				if ( sRetStatus != STD_RET_SUCCESS) {
					//Supervisor Intervention declined, inform user of prohibition
					UieErrorMsg(LDT_PROHBT_ADR, UIE_WITHOUT);
                    MldECScrollWrite();
					return sRetStatus;
				}
			}
			//set status variable to indicate HALO override is active
			RflSetHALOStatus(0x01);

			// Display HALO Override Notification
			RegDisplay.uchMajorClass = CLASS_UIFREGOTHER;
			RegDisplay.uchMinorClass = CLASS_HALO_OVR;
			RegDisplay.lAmount = pData->SEL.INPUT.lData * 1000;
			UifRegData.regsales.lUnitPrice = (DUNITPRICE)pData->SEL.INPUT.lData;
			RflGetTranMnem ( RegDisplay.aszMnemonic, TRN_HALO_ADR);

			DispWrite( &RegDisplay );

			return (SUCCESS);
			break;
		}
//End SR 143

//SR 47 JHHJ added manual validation key
		case FSC_VALIDATION:
			if (pData->SEL.INPUT.uchLen != 0) {
                return(LDT_SEQERR_ADR);
            }
            return(ItemOtherValidation());

			break;
//END SR 47

//SR 192 Condiment Editing
		case FSC_EDIT_CONDIMENT:
		/*case FSC_SCROLL_UP:
		case FSC_SCROLL_DOWN:*/

			if((pData->SEL.INPUT.uchMajor == FSC_EDIT_CONDIMENT) &&
				(!CliParaMDCCheck(MDC_DEPT3_ADR, ODD_MDC_BIT3)))
			{
				return(LDT_PROHBT_ADR);
			}

			if(pData->SEL.INPUT.uchLen !=0)
			{
				return (LDT_SEQERR_ADR);

			}
			sRetStatus = ItemSalesEditCondiment(pData->SEL.INPUT.uchMajor);

			if((sRetStatus == MLD_CURSOR_CHANGE_UP )|| (sRetStatus == MLD_CURSOR_CHANGE_DOWN))
			{
				return(UifRegDefProc(pData));
			}else if( sRetStatus != SUCCESS)
			{
				return sRetStatus;
			}
			break;
// END SR 192
//SR 192 Condiment Editing
		case FSC_EDIT_COND_TBL:
		/*case FSC_SCROLL_UP:
		case FSC_SCROLL_DOWN:*/

			if((pData->SEL.INPUT.uchMajor == FSC_EDIT_COND_TBL) &&
				(!CliParaMDCCheck(MDC_DEPT3_ADR, ODD_MDC_BIT3)))
			{
				return(LDT_PROHBT_ADR);
			}

			if(pData->SEL.INPUT.uchLen !=0)
			{
				return (LDT_SEQERR_ADR);

			}
			sRetStatus = ItemSalesEditCondiment(pData->SEL.INPUT.uchMajor);

			if((sRetStatus == MLD_CURSOR_CHANGE_UP )|| (sRetStatus == MLD_CURSOR_CHANGE_DOWN))
			{
				return(UifRegDefProc(pData));
			}else if( sRetStatus != SUCCESS)
			{
				return sRetStatus;
			}
			break;
// END SR 192
//SR 1085 Tent implementation  ***PDINU
		case FSC_TENT:
			// Call for tent or receipt id input.  may already be entered as a digit string
			// or may need to be entered via alpha-numeric keyboard.
			UifTent( pData );

			RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGOTHER, CLASS_UICLEAR, DISP_SAVE_TYPE_0);
			return (SUCCESS);
			break;

		case FSC_SUPR_INTRVN:
			// Handle the FSC to perform a Supervisor Intervention action.
			// We issue the tone and display the leadthru for Supervisor
			// Intervention then call UifSupIntervent () which will do
			// do a loop processing keyboard input until either a correct
			// Supervisor number is entered or a Clear key is pressed.
			{
				/*--- BEEP TONE ---*/
				PifBeep(UIE_SPVINT_TONE);

				// Display the leadthru to inform the operator what is expected
				UifRedisplaySupInt();

				// Process the Supervisor Intervention action
				sRetStatus = UifSupIntervent ();

				// Clear the leadthru area as we are done.
				RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGOTHER, CLASS_UICLEAR, DISP_SAVE_TYPE_0);
			}
			return sRetStatus;
			break;

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
			return(UIE_SUC_KEY_TRANSPARENT);
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

			// Clear the leadthru area as we are done.
			RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGOTHER, CLASS_UICLEAR, DISP_SAVE_TYPE_0);

			// just return rather than doing any kind of UIE menu tree action or cleanup.
			// the window dismiss is basically transparent to the UIE functionality.
			return(sRetStatus);
			break;

		case FSC_AUTO_SIGN_OUT:
			sRetStatus = CasForceAutoSignOut(0);
			break;

		case FSC_MSR:
			sRetStatus = CheckAllowFunction();		// Looks to see if transaction has been itemized
			if (sRetStatus == SUCCESS) {
				if (UifCheckMsrEmployeeTrackData (pData->SEL.INPUT.DEV.MSR.auchTrack2, pData->SEL.INPUT.DEV.MSR.uchLength2) != 0) {
					NHPOS_NONASSERT_TEXT("MSR swipe credit card ignored.");
					UieReject();                            /* send reject to parent */
					return (SUCCESS);
				}
				UifRegData.regopeclose.uchMajorClass = CLASS_UIFREGOPECLOSE;
                UifRegData.regopeclose.uchMinorClass = CLASS_UICASHIEROUT;
                UifRegData.regopeclose.fbInputStatus |= INPUT_MSR;
                sRetStatus = ItemSignOut(&UifRegData.regopeclose);
                UifRegWorkClear();
                if (sRetStatus == UIF_CANCEL) {
                    UieReject();
                    return (SUCCESS);
                } else if (sRetStatus == UIF_CAS_SIGNOUT) {
                    flUifRegStatus |= UIFREG_CASHIERSIGNOUT;
					BlFwIfDefaultUserScreen();				/* call function to bring up default window(s) */
                    UieAccept();
                    return (SUCCESS);
                }
			}
			else
			{
				return(sRetStatus);
			}
			break;

		case FSC_OPR_PICKUP:
			// Handle the FSC to perform a Supervisor Intervention action.
			// We issue the tone and display the leadthru for Supervisor
			// Intervention then call UifSupIntervent () which will do
			// do a loop processing keyboard input until either a correct
			// Supervisor number is entered or a Clear key is pressed.
			{
			sRetStatus = CheckAllowFunction();		// Looks to see if transaction has been itemized
			if (sRetStatus == SUCCESS) {
				/*--- BEEP TONE ---*/
				PifBeep(UIE_SPVINT_TONE);

				// Display the leadthru to inform the operator what is expected
				UifRedisplaySupInt();

				// Process the Supervisor Intervention action
				sRetStatus = UifSupIntervent ();
				if (sRetStatus == UIF_SUCCESS) {
					// Process the Cash Pickup action
					sRetStatus = UifCashPickup ( pData );
				}

				// Clear the leadthru area as we are done.
				RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGOTHER, CLASS_UICLEAR, DISP_SAVE_TYPE_0);

				return sRetStatus;
			}else{
				return(sRetStatus);
			}
			}
		break;

		case FSC_GIFT_CARD:
			if((sRetStatus = CheckAllowFunction()) == SUCCESS){
				switch (pData->SEL.INPUT.uchMinor) {
					case CLASS_UIBALANCE:
						sRetStatus = ItemSalesBalance(IS_GIFT_CARD_GENERIC);
						break;
					case CLASS_EPAYMENT:
						sRetStatus = ItemSalesBalance(IS_GIFT_CARD_EPAYMENT);
						break;
					case CLASS_FREEDOMPAY:
						sRetStatus = ItemSalesBalance(IS_GIFT_CARD_FREEDOMPAY);
						break;
					default:
						sRetStatus = LDT_EQUIPPROVERROR_ADR;
						NHPOS_ASSERT_TEXT((sRetStatus != LDT_EQUIPPROVERROR_ADR), "**ERROR: FSC_GIFT_CARD - SEL.INPUT.uchMinor invalid.");
						break;
				}
				if (sRetStatus == UIF_CANCEL) {             /* cancel ? */
					UifRegData.regmisc.uchMajorClass = CLASS_UIFREGMISC;  /* set cancel class */
					UifRegData.regmisc.uchMinorClass = CLASS_UICANCEL;
					sRetStatus = ItemMiscCancel(&UifRegData.regmisc);     /* Misc Modele */
					UifRegWorkClear();                          /* clear work area */
					flUifRegStatus |= UIFREG_MODIFIER;          /* set modifier status for display clear*/
					UieReject();                            /* send reject to parent */
					return SUCCESS;
				}
			}
			else
				return(sRetStatus);
			break;

        default:
            return(UifRegDefProc(pData));               /* default process */
        }

		// clean up and return the proper status of the FSC being processed.
		// we will also move to the next step in the menu tree sequence by
		// using the UieAccept() call or stay where we are in the tree sequence
		// by using the UieReject() function.
		//
		// not all FSC process will want to do this so in some cases we just
		// return when done with that FSC rather than doing the following.
		if (sRetStatus == LDT_SUC_KEY_TRANSPARENT) {
            return(UIE_SUC_KEY_TRANSPARENT);
		}
        UifRegWorkClear();                          /* clear work area */
        if (sRetStatus == UIF_CANCEL) {             /* cancel ? */
            flUifRegStatus |= UIFREG_MODIFIER;          /* set modifier status for display clear*/
            if (sStatusSave) {                  /* if previous status is enable */
                ItemOtherDifEnableStatus(1);    /* R3.1 for Beverage Dispenser */
            }
            UieReject();                            /* send reject to parent */
            break;
        }
        if (sRetStatus == LDT_TAKETL_ADR) {         /* buffer full ? */
            flUifRegStatus |= UIFREG_BUFFERFULL;    /* set buffer full status */
            UieAccept();                            /* send accepted to parent */
            return(sRetStatus);
        }
        if (sRetStatus != UIF_SUCCESS) {            /* error ? */
            return(sRetStatus);
        }
        UieAccept();                               /* send accepted to parent */
        break;

    default:
        return(UifRegDefProc(pData));              /* default process */
    }
    return (SUCCESS);
}

/*
*===========================================================================
** Synopsis:    LONG UifRegShiftQty(LONG lData, UCHAR uchDigit)
*     Input:    lData - qty amount
*               uchDigit - input counter
*     Output:   nothing
*     INOut:    nothing
*
** Return:      shift amount
*
** Description: Reg Shift Qty Amount
*===========================================================================
*/
LONG UifRegShiftQty(LONG lData, UCHAR uchDigit)
{
    switch (uchDigit) {
    case 0:
        return(lData * 1000L);
    case 1:
        return(lData * 100L);
    case 2:
        return(lData * 10L);
    default:
        return(lData);
    }
}

/****** End of Source ******/