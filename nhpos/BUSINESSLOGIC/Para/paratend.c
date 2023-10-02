/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1998            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Parameter ParaTend Module
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARATEND.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaTendRead()  : read
*               ParaTendWrite() : set
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* Aug-04-98:  03.03.00 : hrkato   : V3.3 (Fast Finalize Improvement)
** GenPOS 
* Aug-06-21:  02.04.01 : R.Chambers : replace (USHORT) _tcslen() with tcharlen()
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
/**
==============================================================================
;                      I N C L U D E     F I L E s                         
;=============================================================================
**/

#include	<tchar.h>
#include <string.h>
#include <stdio.h>

#include <ecr.h>
#include <rfl.h>
#include <plu.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>

#define  PARA_TENDER_RULE_TYPE_RANGE           1     // used with the <range> </range> tag
#define  PARA_TENDER_RULE_TYPE_IGNORETENDER    2     // used with the <ignoretender> </ignoretender> tag

typedef  struct {
	TCHAR   aszAccountNumberRule[48];
	TCHAR   aszAccountNumberRuleLast[48];
	TCHAR   aszCardLabel[28];                // see also TRN_DSI_CARDTYPE and EEPTRSPDATA.aszCardLabel
	USHORT  usRuleType;
	USHORT  usLengthRule;
	USHORT  usLengthEndRule;
	USHORT  usTenderNumber;
	USHORT  usFlags;
} TenderKeyAccountPattern;

static TenderKeyAccountPattern  AccountPatterns[400];
static TenderKeyAccountPattern  ReturnsPatterns[400];
static struct {
	CHAR  aszBatchCloseIniPath[512];
	CHAR  aszBatchCloseKeyword[48];
} ReturnPatternsBatchCloseFile;

/**
;=============================================================================
;    Synopsis:  VOID ParaTendRead(PARATEND *pData)
;       input:  pData->uchAddress
;      output:  pData->uchTend
;
;      Return:  Nothing
;
; Descruption:  This function reads TEND.
;===============================================================================
**/
VOID    ParaTendRead(PARATEND *pData)
{
    UCHAR    i;

    i = (UCHAR)(pData->uchAddress - 1);     /* "-1" fits program address to RAM address */    
	if (i >= STD_TENDER_MAX) {   // ParaTendRead()
		// if this address is out of range of standard tender keys then
		// default to tender 1 (CLASS_TENDER1), normally Cash.  We might see this with Foreign Currency (CLASS_FOREIGN1).
		// see also similar decision in function ItemTenderCheckTenderMdc().
		i = 0;
	}
    pData->uchTend = ParaTend[i];
}


VOID    ParaTendInfoRead(PARATENDERKEYINFO *pData)
{
    UCHAR    i;

    i = (UCHAR)(pData->uchAddress - 1);     /* "-1" fits program address to RAM address */
	if (i >= STD_TENDER_MAX) {    // ParaTendInfoRead ()
		// if this address is out of range of standard tender keys then
		// default to tender 1 (CLASS_TENDER1), normally Cash.  We might see this with Foreign Currency (CLASS_FOREIGN1).
		// see also similar decision in function ItemTenderCheckTenderMdc().
		i = 0;
	}
	pData->TenderKeyInfo = ParaTenderKeyInformation[i];
}

/**
;=============================================================================
;    Synopsis:  VOID    ParaTendWrite(PARATEND *pData)
;       input:  pData->uchAddress
;               pData->uchTend
;      output:  Nothing   
;
;      Return:  Nothing
;
; Descruption:  This function sets TEND.
;===============================================================================
**/
VOID    ParaTendWrite(PARATEND *pData)
{
	if (pData->uchAddress <= STD_TENDER_MAX) {  // ParaTendWrite()
		UCHAR    i = (UCHAR)(pData->uchAddress - 1);     /* "-1" fits program address to RAM address */    
		ParaTend[i] = pData->uchTend;
		ParaTenderKeyInformation[i].uchTend = pData->uchTend;
	}
}


VOID    ParaTendInfoWrite(PARATENDERKEYINFO *pData)
{
    UCHAR    i;

    i = (UCHAR)(pData->uchAddress - 1);     /* "-1" fits program address to RAM address */    
    ParaTend[i] = pData->TenderKeyInfo.uchTend;
	ParaTenderKeyInformation[i] = pData->TenderKeyInfo;
}



VOID    ParaAutoCouponRead(PARAAUTOCPN *pData)
{
    UCHAR    i;

    i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */     

    pData->ulAmount = ParaAutoCoupon[i];
}

VOID    ParaAutoCouponWrite(PARAAUTOCPN *pData)
{
	UCHAR    i;

    i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */     

    ParaAutoCoupon[i] = pData->ulAmount;
}

VOID    ParaColorPaletteRead(PARACOLORPALETTE *pData)
{
    UCHAR    i;

    i = ( UCHAR)(pData->uchAddress);   /* "-1" fits program address to RAM address */     
	pData->crForeground = ParaColorPalette[i][0];
	pData->crBackground = ParaColorPalette[i][1];
}

VOID    ParaColorPaletteWrite(PARACOLORPALETTE *pData)
{
	UCHAR    i;

    i = ( UCHAR)(pData->uchAddress);   /* "-1" fits program address to RAM address */     

	ParaColorPalette[i][0] =	pData->crForeground;
	ParaColorPalette[i][1] = 	pData->crBackground;
}

VOID    ParaTtlKeyOrderDecRead(PARATTLKEYORDDEC *pData)
{
    UCHAR    i;

    i = (UCHAR)(pData->uchAddress) - 1;   /* "-1" fits program address to RAM address */     
	pData->uchOrderDec = Para.ParaTtlKeyOrderDecType[i];
}

VOID    ParaTtlKeyOrderDecWrite(PARATTLKEYORDDEC *pData)
{
	UCHAR    i;

    i = (UCHAR)(pData->uchAddress) - 1;   /* "-1" fits program address to RAM address */     

	Para.ParaTtlKeyOrderDecType[i] =	pData->uchOrderDec;
}

UCHAR  ParaStoreForwardFlag (VOID)
{
	return (Para.uchStoreForwardFlag & PIF_STORE_AND_FORWARD_STAT_MASK);
}

VOID	ParaSetStoreFlag(USHORT StoreFlag)
{
	Para.uchStoreForwardFlag &= ~PIF_STORE_AND_FORWARD_STAT_MASK;
	Para.uchStoreForwardFlag |= ((UCHAR)StoreFlag & PIF_STORE_AND_FORWARD_STAT_MASK);
}

UCHAR  ParaStoreForwardFlagAndStatus (VOID)
{
	return Para.uchStoreForwardFlag;
}

VOID	ParaSetStoreFlagAndStatus(USHORT StoreFlagStatus)
{
	Para.uchStoreForwardFlag = (UCHAR)StoreFlagStatus;
}

UCHAR	ParaSetStoreBitFlags (USHORT StoreFlagStatus)
{
	UCHAR  uchSave = Para.uchStoreForwardFlag;

	Para.uchStoreForwardFlag |= (UCHAR)StoreFlagStatus;

	return uchSave;
}

UCHAR	ParaResetStoreBitFlags (USHORT StoreFlagStatus)
{
	UCHAR  uchSave = Para.uchStoreForwardFlag;

	Para.uchStoreForwardFlag &= ~(UCHAR)StoreFlagStatus;

	return uchSave;
}

UCHAR	ParaSetStoreStatusOnly(USHORT StoreFlagStatus)
{
	UCHAR  uchSave = Para.uchStoreForwardFlag;

	Para.uchStoreForwardFlag |= ((UCHAR)StoreFlagStatus & ~PIF_STORE_AND_FORWARD_STAT_MASK);

	return uchSave;
}

UCHAR	ParaResetStoreStatusOnly(USHORT StoreFlagStatus)
{
	UCHAR  uchSave = Para.uchStoreForwardFlag;

	Para.uchStoreForwardFlag &= ~((UCHAR)StoreFlagStatus & ~PIF_STORE_AND_FORWARD_STAT_MASK);

	return uchSave;
}

static int ParaTendAccountRuleCompare (TCHAR *aszAccountNumber, TCHAR *aszAccountNumberRule)
{
	int     iMatch = 0;
	USHORT  usRuleLen = tcharlen (aszAccountNumberRule);
	TCHAR   *ptcsAccountNumber = aszAccountNumber;
	TCHAR   *ptcsAccountNumberRule = aszAccountNumberRule;

	while (*ptcsAccountNumberRule && *ptcsAccountNumber) {
		if (*ptcsAccountNumberRule == _T('[')) {
			USHORT   usRangeMatch = 0;

			// begin a range of possible values
			ptcsAccountNumberRule++;
			while (*ptcsAccountNumberRule && *ptcsAccountNumberRule != _T(']')) {
				usRangeMatch = (*ptcsAccountNumber == *ptcsAccountNumberRule);
				if (usRangeMatch)
					break;
				ptcsAccountNumberRule++;
			}

			while (*ptcsAccountNumberRule && *ptcsAccountNumberRule++ != _T(']')) ;

			if (! usRangeMatch) {
				iMatch = -1;        // indicate it is not a match.  use of <range> is undefined when using [] range
				break;
			}
		}
		else if (*ptcsAccountNumberRule == _T('.') || *ptcsAccountNumberRule == _T('?')) {
			// any character matches
			ptcsAccountNumberRule++;
		}
		else {
			iMatch = ((int)*ptcsAccountNumber - (int)*ptcsAccountNumberRule);
			ptcsAccountNumberRule++;
			if (iMatch != 0)
				break;
		}
		ptcsAccountNumber++;
	}

	return iMatch;
}

SHORT  ParaTendAccountRuleReadTender (USHORT  usTenderKey, TCHAR *aszAccountNumber, PARATENDERKEYINFO *pData)
{
    SHORT     sTender = -1;
	USHORT    usIndex;

	USHORT  usAccountLen = tcharlen (aszAccountNumber);

	for (usIndex = 0; usIndex < sizeof(AccountPatterns)/sizeof(AccountPatterns[0]); usIndex++) {
		BOOL  bLengthMatch = FALSE;

		// lets see if we have reached the end of the entries in the table.
		if (AccountPatterns[usIndex].usTenderNumber == 0)
			break;

		if ((AccountPatterns[usIndex].usTenderNumber & 0xf00) != 0)
			continue;    // this is a validation, #/Type, or some other rule and not an assignment rule

		if (usTenderKey != 0 && AccountPatterns[usIndex].usTenderNumber == usTenderKey && AccountPatterns[usIndex].usRuleType == PARA_TENDER_RULE_TYPE_IGNORETENDER) {
			sTender = usTenderKey;
			break;
		}

		// this is an ignore tender rule so just skip and try the next one.
		if (AccountPatterns[usIndex].usRuleType == PARA_TENDER_RULE_TYPE_IGNORETENDER) 
			continue;

		// ok, we are not at the end of the table entries yet so we will now see if this table entry
		// will be the rule that will match the account number.  if this rule has a length, see if it matches.
		// length of zero in the rule means any length will match this part of the rule.
		bLengthMatch = (AccountPatterns[usIndex].usLengthRule == usAccountLen || AccountPatterns[usIndex].usLengthRule == 0);
		if (AccountPatterns[usIndex].usLengthEndRule > 0) {
			bLengthMatch = bLengthMatch || (AccountPatterns[usIndex].usLengthRule <= usAccountLen && AccountPatterns[usIndex].usLengthEndRule >= usAccountLen);
		}

		if (bLengthMatch) {
			SHORT   sCompareStatus = 0;

			// length rule is met so now check the account number
			sCompareStatus = ParaTendAccountRuleCompare (aszAccountNumber, AccountPatterns[usIndex].aszAccountNumberRule);
			if (sCompareStatus == 0) {
				// we have a match for the account number rule as well so lets set the tender address and get the info.
				sTender = AccountPatterns[usIndex].usTenderNumber;
				break;
			} else if (sCompareStatus > 0 && AccountPatterns[usIndex].usRuleType == PARA_TENDER_RULE_TYPE_RANGE) {
				sCompareStatus = ParaTendAccountRuleCompare (aszAccountNumber, AccountPatterns[usIndex].aszAccountNumberRuleLast);
				if (sCompareStatus <= 0) {
					// we have a match for the account number rule as well so lets set the tender address and get the info.
					sTender = AccountPatterns[usIndex].usTenderNumber;
					break;
				}
			}
		}
	}

	if (sTender > 0 && pData != NULL) {
		sTender--;   // change to a zero offset address
		pData->TenderKeyInfo = ParaTenderKeyInformation[sTender];
		sTender++;   // increment back so that we return the ParaTenderKey address needed by other functions
	}

	return sTender;
}

static SHORT ParaTendAccountRuleValidateLuhnMod10 (TCHAR *aszAccountNumber)
{
	TCHAR  aszAccountNumberReversed[32];
	SHORT  sReverseIndex = 31;
	SHORT  sIndex = 0;
	SHORT  sSum = 0;

	aszAccountNumberReversed[sReverseIndex] = 0;

	// reverse the order of the digits so that we can process them using the Luhn algorithm for Mod 10 check sum.
	// see http://www.beachnet.com/~hstiles/cardtype.html
	//
	for (sIndex = 0, sReverseIndex--; aszAccountNumber[sIndex] && sReverseIndex >= 0; sIndex++, sReverseIndex--) {
		aszAccountNumberReversed[sReverseIndex] = aszAccountNumber[sIndex];
	}

	sSum = 0;
	sReverseIndex++;
	for (sIndex = 1; aszAccountNumberReversed[sReverseIndex]; sIndex++, sReverseIndex++) {
		SHORT sValue = (aszAccountNumberReversed[sReverseIndex] & 0xf);  // assume ANSI digit 0x30 through 0x39

		if ((sIndex & 1) == 0) {
			// if an even numbered digit in the account number then we do the doubling then add each of
			// the digits from the doubled amount to the sum.  for instance
			// if digit is 8 then times two is 16 so we add 1 to the sum and then add 6 to the sum.
			sValue *= 2;
			// single digit doubled is never more than two digits (max is 9 and 9 times 2 is 18).
			// use mod operator to get lower digit and divide by 10 to get the upper digit (which may be zero).
			sSum += (sValue % 10) + (sValue / 10);
		} else {
			// if an odd numbered digit in the account number then just add the digit to the sum
			sSum += sValue;
		}
	}

	return ((sSum % 10) ? -1 : 0);  // if evenly divisible by 10 then checksum validation works, account number with checksum is correct.
}


SHORT  ParaTendAccountRuleValidateTender (USHORT  usTenderKey, TCHAR *aszAccountNumber, TCHAR *aszCardLabel, PARATENDERKEYINFO *pData)
{
    SHORT     sTender = 0;  // assume that tender is ok unless we find a rule and rule does not match
	USHORT    usIndex;
	USHORT    usAccountLen = tcharlen (aszAccountNumber);
	USHORT    usMatchMap = 0;   // used to determine what matches and what does not

	// for usMatchMap we have the following bit mappings.  upper byte indicates validation type
	// and the lower byte indicates the result of the validation attempt.
	//    0x0001 - indicates that the length validation has passed
	//    0x0002 - indicates that the account number validation has passed
	//    0x0004 - indicates that the Luhn Mod 10 checksum validation has passed
	//    0x0100 - indicates that the length validation was requested at least once
	//    0x0200 - indicates that the account number validation was requested at least once
	//    0x0400 - indicates that the Luhn Mod 10 checksum validation was requested at least once

	usTenderKey |= PARATENDACCOUNTRULE_VALIDATE;   // indicate that this is a validation exercise

	for (usIndex = 0; usIndex < sizeof(AccountPatterns)/sizeof(AccountPatterns[0]); usIndex++) {

		// lets see if we have reached the end of the entries in the table.
		if (AccountPatterns[usIndex].usTenderNumber == 0)
			break;

		if ((AccountPatterns[usIndex].usTenderNumber & PARATENDACCOUNTRULE_VALIDATE) == 0 ) {
			continue;
		}

		// if not a match for this tender and this is not a general rule for all tenders then skip to next rule.
		if (AccountPatterns[usIndex].usTenderNumber != usTenderKey && AccountPatterns[usIndex].usTenderNumber != PARATENDACCOUNTRULE_VALIDATE ) {
			continue;
		}

		if (AccountPatterns[usIndex].usFlags & 0x01) {  // check if <luhnmod10> is indicated.
			// check to see if the Luhn Mod 10 checksum algorithm is valid for this account number.
			// if Luhn Mod 10 checksum fails and this is not a required rule then skip it.
			usMatchMap |= 0x0400;   // indicate that the Luhn Mod 10 checksum validation was requested
			if (ParaTendAccountRuleValidateLuhnMod10 (aszAccountNumber) != 0) {
				if ((AccountPatterns[usIndex].usFlags & 0x02) != 0) {
					sTender = -1;   // we have a required rule to match Luhn Mod 10 algorithm and it did not match.
					break;
				}
			} else {
				usMatchMap |= 0x0004;   // indicate that the Luhn Mod 10 checksum validation has passed
			}
		}

		if (AccountPatterns[usIndex].usLengthRule != 0) {
			BOOL  bLengthMatch = FALSE;

			usMatchMap |= 0x0100;   // indicate that the length validation was requested

			bLengthMatch = (AccountPatterns[usIndex].usLengthRule == usAccountLen);
			if (AccountPatterns[usIndex].usLengthEndRule > 0) {
				bLengthMatch = bLengthMatch || (AccountPatterns[usIndex].usLengthRule <= usAccountLen && AccountPatterns[usIndex].usLengthEndRule >= usAccountLen);
			}

			if (!bLengthMatch) {
				if ((AccountPatterns[usIndex].usFlags & 0x02) != 0) {
					sTender = -1;   // we have a required rule to match account number length and it did not match.
					break;
				}
			} else {
				usMatchMap |= 0x0001;   // indicate that the length validation has passed
			}
		}

		if (AccountPatterns[usIndex].aszAccountNumberRule[0] != 0) {
			SHORT   sCompareStatus = 0;

			usMatchMap |= 0x0200;   // indicate that the account number validation was requested

			sCompareStatus = ParaTendAccountRuleCompare (aszAccountNumber, AccountPatterns[usIndex].aszAccountNumberRule);
			if (sCompareStatus == 0) {
				// we have a match for the account number rule as well so lets indicate this is a valid account number.
				usMatchMap |= 0x0002;   // indicate that the account number validation has passed
			} else if (sCompareStatus > 0 && AccountPatterns[usIndex].usRuleType == PARA_TENDER_RULE_TYPE_RANGE) {
				sCompareStatus = ParaTendAccountRuleCompare (aszAccountNumber, AccountPatterns[usIndex].aszAccountNumberRuleLast);
				if (sCompareStatus <= 0) {
					// we have a match for the account number rule as it is within the range so indicate this is valid account number.
					usMatchMap |= 0x0002;   // indicate that the account number validation has passed
				}
			}

			if ((usMatchMap & 0x0002) == 0 && AccountPatterns[usIndex].usFlags & 0x02) {
				sTender = -1;   // we have a required rule to match account number length and range and it did not match.
				break;          // rule match failed and this is a mandatory or required rule so invalid account number
			}
		}

		// Now check to see if this required or optional rule was satisfied.  If so then we
		// are done.
		if ((usMatchMap & 0x0300) == 0x0300 && ((((usMatchMap & 0x0700) >> 8) & 0x000f) == (usMatchMap & 0x0007))) {
			usMatchMap = 0;
			sTender = 0;
			break;
		}
		sTender = -1;     // we have at least one required or optional rule and we have not found a match yet.
		usMatchMap = 0;   // clear the map for the next validation rule
	}

	if (sTender >= 0 && aszCardLabel) {
		if (AccountPatterns[usIndex].usTenderNumber != 0) {
			_tcscpy (aszCardLabel, AccountPatterns[usIndex].aszCardLabel);
		}
	}

	return sTender;
}

SHORT  ParaTendAccountRuleNumtypeEngine (USHORT usIncludeFilterFlags, USHORT  usTenderKey, TCHAR *aszAccountNumber)
{
    SHORT     sTender = 1;  // assume that tender is ok however we did not find a rule
	USHORT    usIndex;
	USHORT    usAccountLen = tcharlen (aszAccountNumber);
	USHORT    usMatchMap = 0;   // used to determine what matches and what does not

	// for usMatchMap we have the following bit mappings.  upper byte indicates validation type
	// and the lower byte indicates the result of the validation attempt.
	//    0x0001 - indicates that the length validation has passed
	//    0x0002 - indicates that the account number validation has passed
	//    0x0004 - indicates that the Luhn Mod 10 checksum validation has passed
	//    0x0100 - indicates that the length validation was requested at least once
	//    0x0200 - indicates that the account number validation was requested at least once
	//    0x0400 - indicates that the Luhn Mod 10 checksum validation was requested at least once

	usTenderKey |= PARATENDACCOUNTRULE_NUMTYPE;   // indicate that this is a #/Type exercise

	for (usIndex = 0; usIndex < sizeof(AccountPatterns)/sizeof(AccountPatterns[0]); usIndex++) {

		// lets see if we have reached the end of the entries in the table.
		if (AccountPatterns[usIndex].usTenderNumber == 0)
			break;

		if ((AccountPatterns[usIndex].usTenderNumber & PARATENDACCOUNTRULE_NUMTYPE) == 0 ) {
			continue;
		}

		// if this is not a rule for tender key or not a match for this tender then skip to next rule.
		if ((AccountPatterns[usIndex].usFlags & usIncludeFilterFlags) == 0 || AccountPatterns[usIndex].usTenderNumber != usTenderKey) {
			continue;
		}

		if (AccountPatterns[usIndex].usLengthRule != 0) {
			BOOL  bLengthMatch = FALSE;

			usMatchMap |= 0x0100;   // indicate that the length validation was requested

			bLengthMatch = (AccountPatterns[usIndex].usLengthRule == usAccountLen);
			if (AccountPatterns[usIndex].usLengthEndRule > 0) {
				bLengthMatch = bLengthMatch || (AccountPatterns[usIndex].usLengthRule <= usAccountLen && AccountPatterns[usIndex].usLengthEndRule >= usAccountLen);
			}

			if (!bLengthMatch) {
				if ((AccountPatterns[usIndex].usFlags & 0x02) != 0) {
					sTender = -1;   // we have a required rule to match account number length and it did not match.
					break;
				}
			} else {
				usMatchMap |= 0x0001;   // indicate that the length validation has passed
			}
		}

		if (AccountPatterns[usIndex].aszAccountNumberRule[0] != 0) {
			SHORT   sCompareStatus = 0;

			usMatchMap |= 0x0200;   // indicate that the account number validation was requested

			sCompareStatus = ParaTendAccountRuleCompare (aszAccountNumber, AccountPatterns[usIndex].aszAccountNumberRule);
			if (sCompareStatus == 0) {
				// we have a match for the account number rule as well so lets indicate this is a valid account number.
				usMatchMap |= 0x0002;   // indicate that the account number validation has passed
			} else if (sCompareStatus > 0 && AccountPatterns[usIndex].usRuleType == PARA_TENDER_RULE_TYPE_RANGE) {
				sCompareStatus = ParaTendAccountRuleCompare (aszAccountNumber, AccountPatterns[usIndex].aszAccountNumberRuleLast);
				if (sCompareStatus <= 0) {
					// we have a match for the account number rule as it is within the range so indicate this is valid account number.
					usMatchMap |= 0x0002;   // indicate that the account number validation has passed
				}
			}

			if ((usMatchMap & 0x0002) == 0 && AccountPatterns[usIndex].usFlags & 0x02) {
				sTender = -1;   // we have a required rule to match account number length and range and it did not match.
				break;          // rule match failed and this is a mandatory or required rule so invalid account number
			}
		}

		// Now check to see if this required or optional rule was satisfied.  If so then we
		// are done.
		if (((((usMatchMap & 0x0700) >> 8) & 0x000f) == (usMatchMap & 0x0007))) {
			usMatchMap = 0;
			sTender = 0;
			break;
		}
		sTender = -1;     // we have at least one required rule and it did not match.
		break;
	}

	return sTender;
}

SHORT  ParaTendAccountRuleNumtypeTender (USHORT  usTenderKey, TCHAR *aszAccountNumber)
{
	return ParaTendAccountRuleNumtypeEngine (0x0004, usTenderKey, aszAccountNumber);
}

SHORT  ParaTendAccountRuleNumtypeTotal (USHORT  usTotalKey, TCHAR *aszAccountNumber)
{
	return ParaTendAccountRuleNumtypeEngine (0x0008, usTotalKey, aszAccountNumber);
}

SHORT  ParaTendAccountRuleNumtypeOrderDec (USHORT  usOrderDecKey, TCHAR *aszAccountNumber)
{
	return ParaTendAccountRuleNumtypeEngine (0x0010, usOrderDecKey, aszAccountNumber);
}


SHORT  ParaTendAccountRuleWrite (TCHAR *aszAccountNumber, TCHAR *aszAccountNumberLast, TCHAR *aszCardLabel, USHORT usFlags, USHORT usLength, USHORT usTenderKey)
{
	USHORT    usIndex;

	for (usIndex = 0; usIndex < sizeof(AccountPatterns)/sizeof(AccountPatterns[0]) - 1; usIndex++) {
		// lets see if we have reached the end of the entries in the table.
		if (AccountPatterns[usIndex].usTenderNumber == 0) {
			memset (&AccountPatterns[usIndex], 0, sizeof(AccountPatterns[0]));
			if (aszAccountNumber)
				_tcscpy (AccountPatterns[usIndex].aszAccountNumberRule, aszAccountNumber);
			else
				AccountPatterns[usIndex].usRuleType = PARA_TENDER_RULE_TYPE_IGNORETENDER;

			if (aszAccountNumberLast && *aszAccountNumberLast) {
				_tcscpy (AccountPatterns[usIndex].aszAccountNumberRuleLast, aszAccountNumberLast);
				AccountPatterns[usIndex].usRuleType = PARA_TENDER_RULE_TYPE_RANGE;
			}
			if (aszCardLabel) {
				_tcscpy (AccountPatterns[usIndex].aszCardLabel, aszCardLabel);
			}
			AccountPatterns[usIndex].usLengthRule = (usLength & 0xff);
			AccountPatterns[usIndex].usLengthEndRule = ((usLength >> 8) & 0xff);
			AccountPatterns[usIndex].usFlags = usFlags;
			AccountPatterns[usIndex].usTenderNumber = usTenderKey;
			break;
		}
	}

	NHPOS_ASSERT_TEXT((usIndex < sizeof(AccountPatterns)/sizeof(AccountPatterns[0]) - 1), "ParaTendAccountRuleWrite(): ERROR - Out of Space.");
	return (usIndex < sizeof(AccountPatterns)/sizeof(AccountPatterns[0]) - 1) ? (SHORT)usIndex : -1;
}

SHORT  ParaTendAccountRuleReadEnum (USHORT *pusIndex, TCHAR *aszAccountNumber, USHORT *pusLength, USHORT *pusTenderKey)
{
	SHORT  sIndex = -1;

	if (*pusIndex < sizeof(AccountPatterns)/sizeof(AccountPatterns[0])) {
		// lets see if we have reached the end of the entries in the table.
		if (AccountPatterns[*pusIndex].usTenderNumber != 0) {
			if (aszAccountNumber)
				_tcscpy (aszAccountNumber, AccountPatterns[*pusIndex].aszAccountNumberRule);
			if (pusLength)
				*pusLength = AccountPatterns[*pusIndex].usLengthRule ;
			if (pusTenderKey)
				*pusTenderKey = AccountPatterns[*pusIndex].usTenderNumber;
			sIndex = *pusIndex;
			(*pusIndex)++;
		}
	}

	return sIndex;
}

VOID  ParaTendAccountRuleClear (VOID)
{
	memset (AccountPatterns, 0, sizeof(AccountPatterns));
}

//--------------------------------------------------------------------------

// ----------------   Returns Rules table

// we will check to see if we are using the Returns Rules or not and if so,
// whether there is any rules for this tender.
//   Returns         Description of meaning
//    -1             Not using returns rule checking
//     0             Using returns rule checking but no rule for this tender
//     1             There is a rule for this tender
SHORT  ParaTendReturnsRuleCheckUse (USHORT usTenderKey, USHORT *pusFlags, USHORT *pusGlobalRules)
{
	SHORT   sRetValue = -2;  // assume there are no rules including no global rules
	int     iIndex = 0;
	USHORT  usGlobalRules = 0, usFlags = 0;

	for (iIndex = 0; iIndex < sizeof(ReturnsPatterns)/sizeof(ReturnsPatterns[0]); iIndex++) {
		// lets see if we have reached the end of the entries in the table.
		if (ReturnsPatterns[iIndex].usTenderNumber == 0) {
			break;
		}
		if (ReturnsPatterns[iIndex].usFlags & PARATENDRETURNFLAGS_USINGRULES) {
			sRetValue = 0;    // assume that we are using rules but have not found a match yet
		}
		if (ReturnsPatterns[iIndex].usTenderNumber & PARATENDACCOUNTRULE_GLOBALRULES) {
			usGlobalRules = ReturnsPatterns[iIndex].aszAccountNumberRule[0];
			if (sRetValue > 0)
				break;        // we have found a rule and just need to provide the global rules as well
			sRetValue = 0;    // assume that we are using rules but have not found a match yet
			continue;
		}
		if (ReturnsPatterns[iIndex].usTenderNumber == usTenderKey) {
			usFlags = ReturnsPatterns[iIndex].usFlags;
			if (ReturnsPatterns[iIndex].usFlags & PARATENDRETURNFLAGS_IGNORETENDER) {
				// we are to ignore this particular tender so mark as not using rules
				sRetValue = -1;
				break;
			} else {
				sRetValue = 1;  // we are using rules and we have a match for the tender
			}
		}
	}

	if (pusFlags)
		*pusFlags = usFlags;

	if (pusGlobalRules)
		*pusGlobalRules = usGlobalRules;

	return sRetValue;
}

VOID   ParaTendReturnsBatchCloseWrite (TCHAR *tcsBatchCloseIniPath, TCHAR *tcsBatchCloseKeyword)
{
	int i;

	memset (&ReturnPatternsBatchCloseFile, 0, sizeof(ReturnPatternsBatchCloseFile));

	if (tcsBatchCloseIniPath) {
		for (i = 0; tcsBatchCloseIniPath[i] && i < TCHARSIZEOF(ReturnPatternsBatchCloseFile.aszBatchCloseIniPath) - 1; i++) {
			ReturnPatternsBatchCloseFile.aszBatchCloseIniPath[i] = tcsBatchCloseIniPath[i];
		}
	}

	if (tcsBatchCloseKeyword) {
		for (i = 0; tcsBatchCloseKeyword[i] && i < TCHARSIZEOF(ReturnPatternsBatchCloseFile.aszBatchCloseKeyword) - 1; i++) {
			ReturnPatternsBatchCloseFile.aszBatchCloseKeyword[i] = tcsBatchCloseKeyword[i];
		}
	}
}

// the following function is a general purpose function to read the value associated
// with a particular keyword from the traintrip.ini file.  this function can be used
// to search the traiptrip.ini file for a keyword and to then return the string associated
// with that keyword.  the expectation is that the text string format is in the form of:
//     keyword=value
// for example  TERMID=417257
SHORT ParaTendReadTrainTripIniFile (CHAR *aszKeyword, CHAR *aszValueword, int iMaxCharLen)
{
	SHORT  sRetStatus = -1;

	*aszValueword = 0;   // make it an empty string for the return.
	if (ReturnPatternsBatchCloseFile.aszBatchCloseIniPath[0] != 0 && ReturnPatternsBatchCloseFile.aszBatchCloseKeyword[0] != 0) {
		char  aszTextLine[512];
		FILE *hIniFile;
		hIniFile = fopen (ReturnPatternsBatchCloseFile.aszBatchCloseIniPath, "r");
		if (hIniFile) {
			char *aszToken = 0;

			while (fgets (aszTextLine, sizeof(aszTextLine) - 1, hIniFile) != NULL) {
				aszTextLine[sizeof(aszTextLine)-1] = 0;     // ensure end of string is in place
				aszToken = strtok (aszTextLine, "\t =\r\n");
				if (aszToken && strcmp (aszToken, aszKeyword) == 0) {
					aszToken = strtok (NULL, "\t =\r\n");
					if (aszToken && iMaxCharLen > 1) {
						iMaxCharLen--;
						strncpy (aszValueword, aszToken, iMaxCharLen);
						aszValueword[iMaxCharLen] = 0;
						sRetStatus = 0;
					}
					break;
				}
			}
			fclose (hIniFile);
		}
	}

	return sRetStatus;
}

SHORT ParaTendReturnsBatchCloseReadDateTime (DATE_TIME  *pDateTime)
{
	char   aszToken[64];
	SHORT  sRetStatus = -1;

	memset (pDateTime, 0, sizeof(DATE_TIME));

	if (ParaTendReadTrainTripIniFile (ReturnPatternsBatchCloseFile.aszBatchCloseKeyword, aszToken, 64) >= 0) {
		if (aszToken[0]) {
			// the batch close value should have the format of YYYYMMDDHHMMSS in military time.
			if (strlen(aszToken) == 14) {
				size_t  i = 0;
				USHORT  usValue = 0;

				for (i = 0; i < strlen(aszToken); i++) {
					switch (i) {
						case 0:    // beginning of YYYY, the year
							usValue = 0;
							break;
						case 4:    // beginning of MM, the month
							pDateTime->usYear = usValue;
							usValue = 0;
							break;
						case 6:    // beginning of DD, the day of the month
							pDateTime->usMonth = usValue;
							usValue = 0;
							break;
						case 8:    // beginning of HH, the hour of the day
							pDateTime->usMDay = usValue;
							usValue = 0;
							break;
						case 10:   // beginning of MM, the minute of the hour
							pDateTime->usHour = usValue;
							usValue = 0;
							break;
						case 12:   // beginning of SS, the second of the minute
							pDateTime->usMin = usValue;
							usValue = 0;
							break;
						default:
							break;
					}
					if (aszToken[i] >= '0' && aszToken[i] <= '9')
						usValue = usValue * 10 + aszToken[i] - '0';
				}
				if (i == 14) { 
					// end of SS, the second of the minute
					pDateTime->usSec = usValue;
				}
				sRetStatus = 0;
			} else {
				sRetStatus = -4;
			}
		} else {
			sRetStatus = -3;
		}
	} else {
		sRetStatus = -2;
		NHPOS_NONASSERT_TEXT("--WARNING Unable to get batch close from ini file.");
	}

	return sRetStatus;
}

SHORT  ParaTendReturnsRuleWrite (TCHAR *aszAccountNumber, TCHAR *aszAccountNumberLast, USHORT usFlags, USHORT usLength, USHORT usTenderKey)
{
	USHORT    usIndex;

	for (usIndex = 0; usIndex < sizeof(ReturnsPatterns)/sizeof(ReturnsPatterns[0]) - 1; usIndex++) {
		if (usTenderKey & PARATENDACCOUNTRULE_GLOBALRULES) {
			if (ReturnsPatterns[usIndex].usTenderNumber & PARATENDACCOUNTRULE_GLOBALRULES) {
				// we need to redo the global rules for returns
				ReturnsPatterns[usIndex].usTenderNumber = 0;
			}
		}

		// lets see if we have reached the end of the entries in the table.
		if (ReturnsPatterns[usIndex].usTenderNumber == 0) {
			memset (&ReturnsPatterns[usIndex], 0, sizeof(ReturnsPatterns[0]));
			if (aszAccountNumber)
				_tcscpy (ReturnsPatterns[usIndex].aszAccountNumberRule, aszAccountNumber);
			else
				ReturnsPatterns[usIndex].usRuleType = PARA_TENDER_RULE_TYPE_IGNORETENDER;

			if (aszAccountNumberLast && *aszAccountNumberLast) {
				_tcscpy (ReturnsPatterns[usIndex].aszAccountNumberRuleLast, aszAccountNumberLast);
				ReturnsPatterns[usIndex].usRuleType = PARA_TENDER_RULE_TYPE_RANGE;
			}
			ReturnsPatterns[usIndex].usLengthRule = (usLength & 0xff);
			ReturnsPatterns[usIndex].usLengthEndRule = ((usLength >> 8) & 0xff);
			ReturnsPatterns[usIndex].usFlags = usFlags;
			ReturnsPatterns[usIndex].usTenderNumber = usTenderKey;
			break;
		}
	}

	NHPOS_ASSERT_TEXT((usIndex < sizeof(ReturnsPatterns)/sizeof(ReturnsPatterns[0]) - 1), "**ERROR ParaTendAccountRuleWrite(): ERROR - Out of Space.");
	return (usIndex < sizeof(ReturnsPatterns)/sizeof(ReturnsPatterns[0]) - 1) ? (SHORT)usIndex : -1;
}

SHORT  ParaTendReturnsRuleReadEnum (USHORT *pusIndex, TCHAR *aszAccountNumber, USHORT *pusLength, USHORT *pusTenderKey)
{
	SHORT  sIndex = -1;

	if (*pusIndex < sizeof(ReturnsPatterns)/sizeof(ReturnsPatterns[0])) {
		// lets see if we have reached the end of the entries in the table.
		if (ReturnsPatterns[*pusIndex].usTenderNumber != 0) {
			if (aszAccountNumber)
				_tcscpy (aszAccountNumber, ReturnsPatterns[*pusIndex].aszAccountNumberRule);
			if (pusLength)
				*pusLength = ReturnsPatterns[*pusIndex].usLengthRule ;
			if (pusTenderKey)
				*pusTenderKey = ReturnsPatterns[*pusIndex].usTenderNumber;
			sIndex = *pusIndex;
			(*pusIndex)++;
		}
	}

	return sIndex;
}

VOID  ParaTendReturnsRuleClear (VOID)
{
	memset (ReturnsPatterns, 0, sizeof(ReturnsPatterns));
}

// -------------------------------------------------------------------

ULONG  ParaIncrementConnEngineConsecNumber (void)
{
	return ++(Para.ulConnEngineElectronicJournalConsecutiveNumber);
}
/* --- End of Source --- */
