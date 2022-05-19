/*

		NeighborhoodPOS Software Product Development Project
		Department of Information Systems
		College of Information Technology
		Georgia Southern University
		Statesboro, Georgia

		Copyright Georgia Southern University Foundation

*===========================================================================
* Title       : Charge Posting Entry Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIDCPET.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*                                    
*               UifDiaCPEntry1()    : Room Charge Entry for Room # 
*               UifDiaCPEntry2()    : Room Charge Entry for Guest ID
*               UifDiaCPEntry3()    : Room/Account Charge Entry for Guest Line 
*                                   : and Account Number
*               UifDiaCPEntry4()    : Pre-/Credit Auth. Entry for Expiration Date 
*               UifDiaCPEntry5()    : Guest Line Number for EPT, 
*               UifDiaCPEntry6()    : Entry for Reference # 
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date   : Ver.Rev. :   Name      : Description
* Sep-13-93 : 00.00.01 : K.You       : initial                                   
* Aug-07-98 : 03.03.00 : hrkato      : V3.3 (Support Scanner)
* Feb-16-00 : 01.00.00 : hrkato      : Saratoga
* GenPOS
* Jun-19-15 : 02.02.01 : R.Chambers  : add FSC_P1 processing as if FSC_NUM_TYPE allowing easy keyboard
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
#include <string.h>

#include "ecr.h"
#include "uie.h"
#include "fsc.h"
#include "regstrct.h"
#include "paraequ.h"
#include "uireg.h"
#include "uiregloc.h"


/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifDiaCPEntry1(KEYMSG *pData)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*
** Description: Room Charge Entry for Room # 
*===========================================================================
*/

SHORT UifDiaCPEntry1(KEYMSG *pData)
{
    switch (pData->uchMsg) {
    case UIM_INPUT:
        switch (pData->SEL.INPUT.uchMajor) {
        case FSC_P1:
			NHPOS_ASSERT( pData->SEL.INPUT.uchLen <= NUM_NUMBER);  //JHHJ NHPOS ASSERT
			NHPOS_ASSERT_ARRAYSIZE( UifRegDiaWork.aszMnemonics, pData->SEL.INPUT.uchLen);
			NHPOS_ASSERT_ARRAYSIZE(  pData->SEL.INPUT.aszKey, pData->SEL.INPUT.uchLen);
            if (pData->SEL.INPUT.uchLen < 1) {             /* w/o input data */
                return(LDT_SEQERR_ADR);
            }

            if (pData->SEL.INPUT.uchLen >= NUM_ROOM) {
                return(LDT_KEYOVER_ADR);
            }

            /* check double wide */

            if (_tcschr(pData->SEL.INPUT.aszKey, 0x12) != 0) {       /* double wide exist */
                return(LDT_KEYOVER_ADR);
            }

            memset(&UifRegDiaWork, '\0', sizeof(UifRegDiaWork));
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;
				/* copy input mnemonics to own buffer */
            _tcsncpy(UifRegDiaWork.aszMnemonics, pData->SEL.INPUT.aszKey, pData->SEL.INPUT.uchLen);
            return(UIF_DIA_SUCCESS);

        case FSC_MSR:
			NHPOS_ASSERT_ARRAYSIZE(UifRegDiaWork.auchTrack2, pData->SEL.INPUT.DEV.MSR.uchLength2);
			NHPOS_ASSERT_ARRAYSIZE(pData->SEL.INPUT.DEV.MSR.auchTrack2, pData->SEL.INPUT.DEV.MSR.uchLength2);

            memset(&UifRegDiaWork, '\0', sizeof(UifRegDiaWork));

            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;

            /* copy input ISO-2 data to own buffer */
            UifRegDiaWork.uchTrack1Len = pData->SEL.INPUT.DEV.MSR.uchLength1;
            _tcsncpy(UifRegDiaWork.auchTrack1, pData->SEL.INPUT.DEV.MSR.auchTrack1, pData->SEL.INPUT.DEV.MSR.uchLength1);

            UifRegDiaWork.uchTrack2Len = pData->SEL.INPUT.DEV.MSR.uchLength2;
            _tcsncpy(UifRegDiaWork.auchTrack2, pData->SEL.INPUT.DEV.MSR.auchTrack2, pData->SEL.INPUT.DEV.MSR.uchLength2);
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
** Synopsis:    SHORT UifDiaCPEntry2(KEYMSG *pData)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*
** Description: Room Charge Entry for Guest ID 
*===========================================================================
*/

SHORT UifDiaCPEntry2(KEYMSG *pData)
{
	NHPOS_ASSERT_ARRAYSIZE(UifRegDiaWork.aszMnemonics, NUM_NUMBER);

    switch (pData->uchMsg) {
    case UIM_INPUT:
        switch (pData->SEL.INPUT.uchMajor) {
        case FSC_P1:
            /* check double wide */

            if (_tcschr(pData->SEL.INPUT.aszKey, 0x12) != 0) {       /* double wide exist */
                return(LDT_KEYOVER_ADR);
            }

			// NUM_GUESTID is size of the buffer which is number of digits for
			// the Guest ID + 1 therefore we subtract one to get exact number of digits.
            if (pData->SEL.INPUT.uchLen != NUM_GUESTID - 1) {
                return(LDT_KEYOVER_ADR);
            }

            memset(&UifRegDiaWork, '\0', sizeof(UifRegDiaWork));

            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;

            _tcsncpy(UifRegDiaWork.aszMnemonics, pData->SEL.INPUT.aszKey, pData->SEL.INPUT.uchLen); /* copy input mnemonics to own buffer */

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
** Synopsis:    SHORT UifDiaCPEntry3(KEYMSG *pData)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*
** Description: Room/Account Charge Entry for Guest Line and Account Number
*               We allow both MSR Swipe, scanned account entry, or manual entry
*               to all be used.
*               Manual entry traditionally requires #/Type however we have added
*               some additional enter type FSCs in order to reduce the number of
*               buttons needed on a window for data entry.
*===========================================================================
*/

SHORT UifDiaCPEntry3(KEYMSG *pData)
{
    SHORT   sStatus;

	NHPOS_ASSERT_ARRAYSIZE(UifRegDiaWork.aszMnemonics, NUM_NUMBER);

    switch (pData->uchMsg) {
    case UIM_INPUT:
        switch (pData->SEL.INPUT.uchMajor) {
        case FSC_AC:            // treat AC key is same as #/Type key (FSC_NUM_TYPE) for UifDiaCPEntry3() to reduce number of buttons
        case FSC_P1:            // treat P1 key is same as #/Type key (FSC_NUM_TYPE) for UifDiaCPEntry3() to reduce number of buttons
        case FSC_PRICE_ENTER:   // treat the Price Enter Key same as #/Type key (FSC_NUM_TYPE) for UifDiaCPEntry3() to reduce number of buttons
			pData->SEL.INPUT.uchMajor = FSC_NUM_TYPE;  // turn key press into #/Type for UifDiaCPEntry3() allowing consolidation of button types
        case FSC_NUM_TYPE:      // UifDiaCPEntry3() handle the #/Type Key being pressed at LDT_NUM_ADR
            if (!pData->SEL.INPUT.uchLen) {             /* w/o input data */
                return(LDT_SEQERR_ADR);
            }

            if (pData->SEL.INPUT.uchLen >= NUM_NUMBER) {
                return(LDT_KEYOVER_ADR);
            }

            memset(&UifRegDiaWork, '\0', sizeof(UifRegDiaWork));
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;   
            UifRegDiaWork.ulData = (ULONG)pData->SEL.INPUT.lData;   /* set input data */

            _tcsncpy(UifRegDiaWork.aszMnemonics, pData->SEL.INPUT.aszKey, pData->SEL.INPUT.uchLen);
            return(UIF_DIA_SUCCESS);

        case FSC_SIGNATURECAPTURE:
            memset(&UifRegDiaWork, '\0', sizeof(UifRegDiaWork));
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;   
            UifRegDiaWork.ulData = (ULONG)pData->SEL.INPUT.lData;   /* set input data */
            return(UIF_DIA_SUCCESS);

        case FSC_PINPAD:
            memset(&UifRegDiaWork, '\0', sizeof(UifRegDiaWork));
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;   
            UifRegDiaWork.ulData = (ULONG)pData->SEL.INPUT.lData;   /* set input data */

            UifRegDiaWork.uchTrack1Len = pData->SEL.INPUT.DEV.PINPAD.uchLength1;
            _tcsncpy(UifRegDiaWork.auchTrack1, pData->SEL.INPUT.DEV.PINPAD.auchTrack1, pData->SEL.INPUT.DEV.PINPAD.uchLength1);  /* copy input ISO-1 data to own buffer */

			NHPOS_ASSERT_ARRAYSIZE(UifRegDiaWork.auchTrack2, pData->SEL.INPUT.DEV.PINPAD.uchLength2);
            UifRegDiaWork.uchTrack2Len = pData->SEL.INPUT.DEV.PINPAD.uchLength2;
            _tcsncpy(UifRegDiaWork.auchTrack2, pData->SEL.INPUT.DEV.PINPAD.auchTrack2, pData->SEL.INPUT.DEV.PINPAD.uchLength2);  /* copy input ISO-2 data to own buffer */
            return(UIF_DIA_SUCCESS);

        case FSC_MSR:
            memset(&UifRegDiaWork, '\0', sizeof(UifRegDiaWork));
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;   
            UifRegDiaWork.ulData = (ULONG)pData->SEL.INPUT.lData;   /* set input data */

            UifRegDiaWork.uchTrack1Len = pData->SEL.INPUT.DEV.MSR.uchLength1;
            _tcsncpy(UifRegDiaWork.auchTrack1, pData->SEL.INPUT.DEV.MSR.auchTrack1, pData->SEL.INPUT.DEV.MSR.uchLength1);  /* copy input ISO-1 data to own buffer */

			NHPOS_ASSERT_ARRAYSIZE(UifRegDiaWork.auchTrack2, pData->SEL.INPUT.DEV.MSR.uchLength2);
            UifRegDiaWork.uchTrack2Len = pData->SEL.INPUT.DEV.MSR.uchLength2;
            _tcsncpy(UifRegDiaWork.auchTrack2, pData->SEL.INPUT.DEV.MSR.auchTrack2, pData->SEL.INPUT.DEV.MSR.uchLength2);  /* copy input ISO-2 data to own buffer */
            return(UIF_DIA_SUCCESS);

        case FSC_VIRTUALKEYEVENT:
			// handle Virtual keyboard event such as for FreedomPay.
			// The input buffer is a struct of type VIRTUALKEY_BUFFER
			memset(&UifRegDiaWork, '\0', sizeof(UifRegDiaWork));
			if (pData->SEL.INPUT.DEV.VIRTUALKEY.usKeyEventType == UIE_VIRTUALKEY_EVENT_FREEDOMPAY) {
				UifRegDiaWork.auchFsc[0] = FSC_VIRTUALKEYEVENT;         /* set fsc */
				UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;   
				UifRegDiaWork.ulData = (ULONG)pData->SEL.INPUT.lData;   /* set input data */

				// Put the FreedomPay fob credentials information in to the Track 2 data area to
				// be used by the tender code later similar to Track 2 data from MSR.
				UifRegDiaWork.uchTrack1Len = pData->SEL.INPUT.DEV.VIRTUALKEY.u.freedompay_data.uchLength1;
				_tcsncpy(UifRegDiaWork.auchTrack1, pData->SEL.INPUT.DEV.VIRTUALKEY.u.freedompay_data.auchTrack1, UifRegDiaWork.uchTrack1Len);

				UifRegDiaWork.uchTrack2Len = pData->SEL.INPUT.DEV.VIRTUALKEY.u.freedompay_data.uchLength2;
				_tcsncpy(UifRegDiaWork.auchTrack2, pData->SEL.INPUT.DEV.VIRTUALKEY.u.freedompay_data.auchTrack2, UifRegDiaWork.uchTrack2Len);

				return(UIF_DIA_SUCCESS);
			}
			else if (pData->SEL.INPUT.DEV.VIRTUALKEY.usKeyEventType == UIE_VIRTUALKEY_EVENT_SCANNER) {
				UifRegDiaWork.auchFsc[0] = FSC_SCANNER;   /* set fsc */
				UifRegDiaWork.auchFsc[1] = 0;   
				_tcsncpy(UifRegDiaWork.aszMnemonics, pData->SEL.INPUT.DEV.VIRTUALKEY.u.scanner_data.auchStr, NUM_NUMBER);
				return(UIF_DIA_SUCCESS);
			}
			else if (pData->SEL.INPUT.DEV.VIRTUALKEY.usKeyEventType == UIE_VIRTUALKEY_EVENT_MSR) {
				UifRegDiaWork.auchFsc[0] = FSC_MSR;                     /* set fsc */
				UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;   
				UifRegDiaWork.ulData = (ULONG)pData->SEL.INPUT.lData;   /* set input data */
				UifRegDiaWork.uchPaymentType = SWIPE_PAYMENT;

				UifRegDiaWork.uchTrack1Len = pData->SEL.INPUT.DEV.VIRTUALKEY.u.msr_data.uchLength1;
				_tcsncpy(UifRegDiaWork.auchTrack1, pData->SEL.INPUT.DEV.VIRTUALKEY.u.msr_data.auchTrack1, pData->SEL.INPUT.DEV.VIRTUALKEY.u.msr_data.uchLength1);  /* copy input ISO-1 data to own buffer */

				UifRegDiaWork.uchTrack2Len = pData->SEL.INPUT.DEV.VIRTUALKEY.u.msr_data.uchLength2;
				_tcsncpy(UifRegDiaWork.auchTrack2, pData->SEL.INPUT.DEV.VIRTUALKEY.u.msr_data.auchTrack2, pData->SEL.INPUT.DEV.VIRTUALKEY.u.msr_data.uchLength2);  /* copy input ISO-2 data to own buffer */
				return(UIF_DIA_SUCCESS);
			}
            return(LDT_SEQERR_ADR);

        case FSC_SCANNER:                                           /* V3.3 */
            if ((sStatus = UifCheckScannerData(pData)) != UIF_SUCCESS) {
                return(sStatus);
            }

            memset(&UifRegDiaWork, '\0', sizeof(UifRegDiaWork));
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;   
            _tcsncpy(UifRegDiaWork.aszMnemonics, aszUifRegNumber, NUM_NUMBER);
            return(UIF_DIA_SUCCESS);
                              
        case FSC_P5:                // P5 key is same as Cancel key (FSC_CANCEL) as used in several places so be consistent
			pData->SEL.INPUT.uchMajor = FSC_CANCEL;  // turn key press into Cancel for UifDiaCPEntry4() allowing consolidation of button types
        case FSC_CANCEL:
            if (pData->SEL.INPUT.uchLen != 0) {             /* exist input data ? */
                return(LDT_SEQERR_ADR);
            }
            return(UIFREG_ABORT);

        case FSC_AUTO_SIGN_OUT:
			UieSetMacro(0, NULL, 0);               /* disable macro function */
            return(UIFREG_ABORT);

		case FSC_PRESET_AMT:
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor = CLASS_UIFREGPAMTTENDER;   /* set fsc */
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;   
			UifRegDiaWork.ulData = (ULONG)pData->SEL.INPUT.lData;   /* set input data */
			return UIF_NEW_FSC;

		case FSC_TENDER:    // UifDiaCPEntry3() handle the Tender Key being pressed at LDT_NUM_ADR
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;   
			UifRegDiaWork.ulData = (ULONG)pData->SEL.INPUT.lData;   /* set input data */
			return UIF_NEW_FSC;

		case FSC_ASK:    // UifDiaCPEntry3() handle the Ask Key being pressed at LDT_NUM_ADR
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor = CLASS_UIFREGOTHER;   /* set fsc */
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor = CLASS_UIFASK;   
			UifRegDiaWork.ulData = (ULONG)pData->SEL.INPUT.lData;   /* set input data */
			return UIF_NEW_FSC;

        case FSC_RECEIPT_FEED:
        case FSC_JOURNAL_FEED:
        case FSC_RESET_LOG:							/* avoid key sequence error by PCIF function, 11/12/01 */
        case FSC_POWER_DOWN:
            return(UifRegDefProc(pData));           /* reg default process */

        case FSC_STRING:                            /* Control String No Key */
            sStatus = UifRegString(pData);
			if (sStatus == SUCCESS)
				sStatus = UIF_STRING_START;
			return sStatus;

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
** Synopsis:    SHORT UifDiaCPEntry4(KEYMSG *pData)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*
** Description: Pre-/Credit Auth. Entry for Expiration Date,        V3.3
*
*               Also used with numeric pause messages.  See UifRegPauseString().
*               We have a special case to allow the P1 key, ordinarily used only
*               with alphanumeric data entry to also be used with a numeric pause
*               key.
*===========================================================================
*/
SHORT   UifDiaCPEntry4(KEYMSG *pData)
{
    SHORT   sStatus;

	NHPOS_ASSERT_ARRAYSIZE(UifRegDiaWork.aszMnemonics, NUM_NUMBER);

    switch (pData->uchMsg) {
    case UIM_INPUT:
        switch (pData->SEL.INPUT.uchMajor) {
        case FSC_AC:       // Allows the AC key to be used with Numeric pause messages
		case FSC_ROA:      // Allows the P1 key to be used with Numeric pause messages as well as with Alphanumeric pauses
		case FSC_P1:       // Now designer of layout can choose just one key to be used with either pause message.
        case FSC_NUM_TYPE:
			pData->SEL.INPUT.uchMajor = FSC_NUM_TYPE;  // turn key press into #/Type for UifDiaCPEntry4() allowing consolidation of button types
		case FSC_PRICE_ENTER://US Customs 4/24/03 cwunn 
            if (pData->SEL.INPUT.uchLen >= NUM_NUMBER) {
                return(LDT_KEYOVER_ADR);
            }

            memset(&UifRegDiaWork, '\0', sizeof(UifRegDiaWork));
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;   
            UifRegDiaWork.ulData = (ULONG)pData->SEL.INPUT.lData;   /* set input data */
            _tcsncpy(UifRegDiaWork.aszMnemonics, pData->SEL.INPUT.aszKey, pData->SEL.INPUT.uchLen);
            return(UIF_DIA_SUCCESS);

        case FSC_SCANNER:                                           /* V3.3 */
            if ((sStatus = UifCheckScannerData(pData)) != UIF_SUCCESS) {
                return(sStatus);
            }
            memset(&UifRegDiaWork, '\0', sizeof(UifRegDiaWork));
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;   
            _tcsncpy(UifRegDiaWork.aszMnemonics, aszUifRegNumber, NUM_NUMBER);
            return(UIF_DIA_SUCCESS);

        case FSC_P5:                // P5 key is same as Cancel key (FSC_CANCEL) as used in several places so be consistent
			pData->SEL.INPUT.uchMajor = FSC_CANCEL;  // turn key press into Cancel for UifDiaCPEntry4() allowing consolidation of button types
        case FSC_CANCEL:
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

        case FSC_STRING:                            /* Control String No Key */
            sStatus = UifRegString(pData);
			if (sStatus == SUCCESS)
				sStatus = UIF_STRING_START;
			return sStatus;

        case FSC_PINPAD:
            if (pData->SEL.INPUT.uchLen != 0) {             /* exist input data ? */
                return(LDT_SEQERR_ADR);
            }
            memset(&UifRegDiaWork, '\0', sizeof(UifRegDiaWork));
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;

			NHPOS_ASSERT(pData->SEL.INPUT.uchLen <= NUM_NUMBER);	//NHPOS ASSERT JHHJ
            _tcsncpy(UifRegDiaWork.aszMnemonics, pData->SEL.INPUT.aszKey, pData->SEL.INPUT.uchLen);

			NHPOS_ASSERT(pData->SEL.INPUT.DEV.PINPAD.uchLength2 <= PIF_LEN_TRACK2); //NHPOS ASSERT JHHJ
			//we are skipping the first element in this array because the supervisor number
			//doesnt begin until the 2nd element.
			if (pData->SEL.INPUT.DEV.PINPAD.uchLength2 > 0) {
				_tcsncpy(UifRegDiaWork.aszMnemonics, &pData->SEL.INPUT.DEV.PINPAD.auchTrack2[1], (pData->SEL.INPUT.DEV.PINPAD.uchLength2 - 1));
				_tcsncpy(UifRegDiaWork.auchTrack2, &pData->SEL.INPUT.DEV.PINPAD.auchTrack2[1], (pData->SEL.INPUT.DEV.PINPAD.uchLength2 - 1));
			}

            UifRegDiaWork.uchTrack2Len = (pData->SEL.INPUT.DEV.PINPAD.uchLength2 - 1);
            return(UIF_DIA_SUCCESS);

        case FSC_MSR:			//SR 825 added for supervisor input in control strings.
            if (pData->SEL.INPUT.uchLen != 0) {             /* exist input data ? */
                return(LDT_SEQERR_ADR);
            }
            memset(&UifRegDiaWork, '\0', sizeof(UifRegDiaWork));
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;

			NHPOS_ASSERT(pData->SEL.INPUT.uchLen <= NUM_NUMBER);	//NHPOS ASSERT JHHJ
            _tcsncpy(UifRegDiaWork.aszMnemonics, pData->SEL.INPUT.aszKey, pData->SEL.INPUT.uchLen);

			NHPOS_ASSERT(pData->SEL.INPUT.DEV.MSR.uchLength2 <= PIF_LEN_TRACK2); //NHPOS ASSERT JHHJ
			//we are skipping the first element in this array because the supervisor number
			//doesnt begin until the 2nd element.
			if (pData->SEL.INPUT.DEV.MSR.uchLength2 > 0) {
				_tcsncpy(UifRegDiaWork.aszMnemonics, &pData->SEL.INPUT.DEV.MSR.auchTrack2[1], (pData->SEL.INPUT.DEV.MSR.uchLength2 - 1));
				_tcsncpy(UifRegDiaWork.auchTrack2, &pData->SEL.INPUT.DEV.MSR.auchTrack2[1], (pData->SEL.INPUT.DEV.MSR.uchLength2 - 1));
			}

            UifRegDiaWork.uchTrack2Len = (pData->SEL.INPUT.DEV.MSR.uchLength2 - 1);
            return(UIF_DIA_SUCCESS);

        case FSC_VIRTUALKEYEVENT:
			// handle Virtual keyboard event such as for FreedomPay.
			// The input buffer is a struct of type VIRTUALKEY_BUFFER
			memset(&UifRegDiaWork, '\0', sizeof(UifRegDiaWork));
			if (pData->SEL.INPUT.DEV.VIRTUALKEY.usKeyEventType == UIE_VIRTUALKEY_EVENT_FREEDOMPAY) {
				UifRegDiaWork.auchFsc[0] = FSC_MSR;                     /* set fsc */
				UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;   
				UifRegDiaWork.ulData = (ULONG)pData->SEL.INPUT.lData;   /* set input data */

				// Put the FreedomPay fob credentials information in to the Track 2 data area to
				// be used by the tender code later similar to Track 2 data from MSR.
				UifRegDiaWork.uchTrack2Len = pData->SEL.INPUT.DEV.VIRTUALKEY.u.freedompay_data.uchLength2;
				_tcsncpy(UifRegDiaWork.auchTrack2, pData->SEL.INPUT.DEV.VIRTUALKEY.u.freedompay_data.auchTrack2, UifRegDiaWork.uchTrack2Len);

				return(UIF_DIA_SUCCESS);
			}
			else if (pData->SEL.INPUT.DEV.VIRTUALKEY.usKeyEventType == UIE_VIRTUALKEY_EVENT_SCANNER) {
				if ((sStatus = UifCheckScannerData(pData)) != UIF_SUCCESS) {
					return(sStatus);
				}

				UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
				UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;   
				_tcsncpy(UifRegDiaWork.aszMnemonics, aszUifRegNumber, NUM_NUMBER);
				return(UIF_DIA_SUCCESS);
			}
			else if (pData->SEL.INPUT.DEV.VIRTUALKEY.usKeyEventType == UIE_VIRTUALKEY_EVENT_MSR) {
				UifRegDiaWork.auchFsc[0] = FSC_MSR;                     /* set fsc */
				UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;   
				UifRegDiaWork.ulData = (ULONG)pData->SEL.INPUT.lData;   /* set input data */
				UifRegDiaWork.uchPaymentType = SWIPE_PAYMENT;

				_tcsncpy(UifRegDiaWork.auchTrack1,            /* copy input ISO-2 data to own buffer */
					pData->SEL.INPUT.DEV.VIRTUALKEY.u.msr_data.auchTrack1, pData->SEL.INPUT.DEV.VIRTUALKEY.u.msr_data.uchLength1);

				UifRegDiaWork.uchTrack1Len = pData->SEL.INPUT.DEV.VIRTUALKEY.u.msr_data.uchLength1;

				_tcsncpy(UifRegDiaWork.auchTrack2,            /* copy input ISO-2 data to own buffer */
					pData->SEL.INPUT.DEV.VIRTUALKEY.u.msr_data.auchTrack2, pData->SEL.INPUT.DEV.VIRTUALKEY.u.msr_data.uchLength2);

				UifRegDiaWork.uchTrack2Len = pData->SEL.INPUT.DEV.VIRTUALKEY.u.msr_data.uchLength2;
				return(UIF_DIA_SUCCESS);
			}
            return(LDT_SEQERR_ADR);

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
** Synopsis:    SHORT UifDiaCPEntry5(KEYMSG *pData)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*
** Description: Guest Line Number for EPT,                      Saratoga
*===========================================================================
*/
SHORT   UifDiaCPEntry5(KEYMSG *pData)
{
	NHPOS_ASSERT_ARRAYSIZE(UifRegDiaWork.aszMnemonics, NUM_NUMBER);

    switch (pData->uchMsg) {
    case UIM_INPUT:
        switch (pData->SEL.INPUT.uchMajor) {
        case FSC_P1:                // P1 key is same as Num Type key (FSC_NUM_TYPE) as used in several places so be consistent
 			pData->SEL.INPUT.uchMajor = FSC_NUM_TYPE;  // turn key press into #/Type for UifDiaCPEntry5() allowing consolidation of button types
       case FSC_NUM_TYPE:
            if (pData->SEL.INPUT.uchLen != 0) {
                return(LDT_SEQERR_ADR);
            }
            if (pData->SEL.INPUT.uchDec != NON_DECIMAL){
                return(LDT_KEYOVER_ADR);
            }
            if (pData->SEL.INPUT.uchLen >= NUM_NUMBER) {
                return(LDT_KEYOVER_ADR);
            }

            memset(&UifRegDiaWork, '\0', sizeof(UifRegDiaWork));
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;
            UifRegDiaWork.ulData = pData->SEL.INPUT.lData;
            _tcsncpy(UifRegDiaWork.aszMnemonics, pData->SEL.INPUT.aszKey, pData->SEL.INPUT.uchLen);
            return(UIF_DIA_SUCCESS);

        case    FSC_SCROLL_DOWN:
        case    FSC_SCROLL_UP:
            if (pData->SEL.INPUT.uchLen != 0) {
                return(LDT_SEQERR_ADR);
            }
            if (pData->SEL.INPUT.uchDec != NON_DECIMAL){
                return(LDT_KEYOVER_ADR);
            }
            memset(&UifRegDiaWork, '\0', sizeof(UifRegDiaWork));
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;
            return(UIF_DIA_SUCCESS);

        case FSC_P5:                // P5 key is same as Cancel key (FSC_CANCEL) as used in several places so be consistent
			pData->SEL.INPUT.uchMajor = FSC_CANCEL;  // turn key press into Cancel for UifDiaCPEntry4() allowing consolidation of button types
        case FSC_CANCEL:
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

    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT UifDiaCPEntry6(KEYMSG *pData)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*
** Description: Guest Line Number for EPT,                      Saratoga
*               used by function UifDiaCP6() to process user innput.
*===========================================================================
*/
SHORT   UifDiaCPEntry6(KEYMSG *pData)
{
    SHORT   sStatus;

	NHPOS_ASSERT_ARRAYSIZE(UifRegDiaWork.aszMnemonics, NUM_NUMBER);
	NHPOS_ASSERT_ARRAYSIZE(aszUifRegNumber, NUM_NUMBER);

    switch (pData->uchMsg) {
    case UIM_INPUT:
        switch (pData->SEL.INPUT.uchMajor) {
        case FSC_PRICE_ENTER:
        case FSC_P1:                // P1 key is same as Num Type key (FSC_NUM_TYPE) as used in several places so be consistent
        case FSC_NUM_TYPE:
			pData->SEL.INPUT.uchMajor = FSC_NUM_TYPE;  // turn key press into #/Type key for UifDiaCPEntry6() allowing consolidation of button types
        case FSC_AC:
            if (!pData->SEL.INPUT.uchLen) {             /* w/o input data */
                return(LDT_SEQERR_ADR);
            }

            if (pData->SEL.INPUT.uchLen >= NUM_NUMBER) {
                return(LDT_KEYOVER_ADR);
            }

            memset(&UifRegDiaWork, '\0', sizeof(UifRegDiaWork));
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;   
            UifRegDiaWork.ulData = (ULONG)pData->SEL.INPUT.lData;   /* set input data */
            _tcsncpy(UifRegDiaWork.aszMnemonics, pData->SEL.INPUT.aszKey, pData->SEL.INPUT.uchLen);   /* copy input mnemonics to own buffer */
            return(UIF_DIA_SUCCESS);

        case FSC_SCANNER:                                           /* V3.3 */
            if ((sStatus = UifCheckScannerData(pData)) != UIF_SUCCESS) {
                return(sStatus);
            }
            memset(&UifRegDiaWork, '\0', sizeof(UifRegDiaWork));
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;   
            _tcsncpy(UifRegDiaWork.aszMnemonics, aszUifRegNumber, NUM_NUMBER);
            return(UIF_DIA_SUCCESS);

        case FSC_P5:                // P5 key is same as Cancel key (FSC_CANCEL) as used in several places so be consistent
			pData->SEL.INPUT.uchMajor = FSC_CANCEL;  // turn key press into Cancel for UifDiaCPEntry4() allowing consolidation of button types
        case FSC_CANCEL:
//            if (pData->SEL.INPUT.uchLen != 0) {             /* exist input data ? */
//                return(LDT_SEQERR_ADR);
//            }
            return(UIFREG_ABORT);

        case FSC_AUTO_SIGN_OUT:
            return(UIFREG_ABORT);

//		case FSC_DOC_LAUNCH:             // do not allow for this data entry, modifies leadthru can lead to operator disorientation
//		case FSC_WINDOW_DISPLAY:         // do not allow for this data entry, modifies leadthru can lead to operator disorientation
//		case FSC_WINDOW_DISMISS:         // do not allow for this data entry, modifies leadthru can lead to operator disorientation
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
    return (SUCCESS);}

/****** End of Source ******/