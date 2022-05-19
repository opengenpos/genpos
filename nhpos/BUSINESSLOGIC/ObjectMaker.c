
#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#include    <ecr.h>
#include    <pif.h>
#include    <FrameworkIO.h>
#include    <DeviceIOPrinter.h>
#include    <log.h>
#include    <plu.h>
#include    <paraequ.h>
#include    <para.h>
#include    <regstrct.h>
#include    <transact.h>
#include    <storage.h>
#include    <csgcs.h>
#include    <csstbstb.h>
#include    <prtprty.h>
#include    <trans.h>
#include    <rfl.h>
#define __BLNOTIFY_C__
#include    <bl.h>

#include "ObjectMaker.h"
#include "..\..\ConnEngineMfc\ConnEngine.h"

static UCHAR   auchCP_TEMP_NEW_FILE_READ_WRITE[]  = "tnwr";

static TCHAR  *pTokenDelimitersTag = _T("<> =\t\n\r");
static TCHAR  *pTokenDelimitersValue = _T("<>");

static void TokenToSignedNumeric (void *pItem, USHORT usSize, TCHAR *tcTokenValue)
{
	LONG   lItem;

	lItem = _ttol (tcTokenValue);

	switch (usSize) {
	case 1:
		*(CHAR *)pItem = (CHAR)lItem;
		break;
	case 2:
		*(SHORT *)pItem = (SHORT)lItem;
		break;
	case 4:
		*(LONG *)pItem = lItem;
		break;
	default:
		break;
	}
}

static TCHAR *GenerateTempFileName ()
{
	static  TCHAR TempFileName[6] = _T("JJJJA");

	TempFileName[4]++;
	if (TempFileName[4] > _T('Y')) TempFileName[4] = _T('A');

	return TempFileName;
}

// This function converts from text digits to an unsigned numeric value.
// If the number begins with x, X or # or a 0X or 0x then we assume it is
// a hexidecimal representation that needs to be converted.
static void TokenToUnsignedNumeric (void *pItem, USHORT usSize, TCHAR *tcTokenValue)
{
	ULONG   ulItem = 0;
	USHORT  usType = 10;

	for ( ; *tcTokenValue; tcTokenValue++) {
		// Do lower case to upper case conversion
		if (*tcTokenValue >= _T('a') && *tcTokenValue <= _T('z'))
			*tcTokenValue = *tcTokenValue - _T('a') + _T('A');

		// Check to see if this is a hexadecimal number or not.
		if (*tcTokenValue == _T('X') || *tcTokenValue == _T('#'))
			usType = 16;

		if (*tcTokenValue >= _T('0') && *tcTokenValue <= _T('9') )
			ulItem = ulItem * usType + (*tcTokenValue - _T('0'));
		if (usType == 16 && *tcTokenValue >= _T('A') && *tcTokenValue <= _T('F'))
			ulItem = ulItem * usType + (*tcTokenValue - _T('A') + 10);
	}

	switch (usSize) {
	case 1:
		*(UCHAR *)pItem = (UCHAR)ulItem;
		break;
	case 2:
		*(USHORT *)pItem = (USHORT)ulItem;
		break;
	case 4:
		*(ULONG *)pItem = ulItem;
		break;
	default:
		break;
	}
}

static void TokenToString (void *pItem, USHORT usElementSize, USHORT usSize, TCHAR *tcTokenValue)
{
	int    iIndex;

	usSize /= usElementSize;

	switch (usElementSize) {
		case 1:
			for (iIndex = 0; iIndex < usSize; iIndex++) {
				((CHAR *)pItem)[iIndex] = tcTokenValue[iIndex];
			}
			break;
		case 2:
			for (iIndex = 0; iIndex < usSize; iIndex++) {
				((TCHAR *)pItem)[iIndex] = tcTokenValue[iIndex];
			}
		break;

		default:
			break;
	}
}


typedef 	struct {
		TCHAR  *tcsLabel;
		USHORT  usOffset;
} LabelListItem;


int ObjectMakerStartParsing (TCHAR **tcsTokenName, TCHAR **tcBuffer, AllObjects *pAllObjects, int iDirection);

int ObjectMakerITEMCONTCODE (TCHAR **tcsTokenName, TCHAR **tcBuffer, ITEMCONTCODE *pItemContCode, int iDirection)
{
	TCHAR *ptcBuffer = *tcBuffer;
	TCHAR *pLastGood;
	TCHAR tcTokenNameEndTag [64];
	LabelListItem myList [] = {
		{_T("uchItemType"),   1},
		{_T("uchReportCode"), 2},
		{_T("usBonusIndex"),  3},
		{0,                   0}
	};

	if (iDirection == 1) {
		// translate from an XML string to an ITEMCONTCODE struct
		TCHAR *tcsTokenValue, *tcsToken;
		int i = 0;

		_stprintf (tcTokenNameEndTag, _T("/%s"), *tcsTokenName);
		pLastGood = ptcBuffer;

		while (1) {
			*tcsTokenName = _tcstok (NULL, pTokenDelimitersTag);
			if (*tcsTokenName == NULL || **tcsTokenName == 0)
				break;

			pLastGood = *tcsTokenName;

			if (_tcscmp (*tcsTokenName, tcTokenNameEndTag) == 0)
				break;

			for (i = 0; myList[i].tcsLabel; i++) {
				if (_tcscmp (myList[i].tcsLabel, *tcsTokenName) == 0) {
					break;
				}
			}

			tcsTokenValue = _tcstok (NULL, pTokenDelimitersValue);

			// if there is a problem with parsing then lets just bail out at this point.
			if (tcsTokenValue == 0)
				break;

			// check to see if the text string between the tags is empty
			if (tcsTokenValue[0] == _T('/')) {
				if (_tcscmp (tcsTokenValue+1, *tcsTokenName) == 0)
					continue;
			}

			tcsToken = _tcstok (NULL, pTokenDelimitersTag);
			if (tcsTokenValue == NULL || tcsToken == NULL)
				break;

			pLastGood = tcsToken;

			// If the tag is not known, then just skip it and go to the next one.
			if (myList[i].tcsLabel == 0)
				continue;

			switch (myList[i].usOffset) {
				case 1:
					TokenToUnsignedNumeric (&(pItemContCode->uchItemType), sizeof(pItemContCode->uchItemType), tcsTokenValue);
					break;
				case 2:
					TokenToUnsignedNumeric (&(pItemContCode->uchReportCode), sizeof(pItemContCode->uchReportCode), tcsTokenValue);
					break;
				case 3:
					TokenToUnsignedNumeric (&(pItemContCode->usBonusIndex), sizeof(pItemContCode->usBonusIndex), tcsTokenValue);
					break;
				default:
					break;
			}
		}
	}

		*tcBuffer = pLastGood + _tcslen(pLastGood) + 1;  // update the buffer pointer so that we can handle multiple messages in the buffer.

	return BL_CONNENGINE_MSG_BUSINESS_OBJECT;
}

int ObjectMakerITEMCOND (TCHAR **tcsTokenName, TCHAR **tcBuffer, ITEMCOND *pItemCond, int iDirection)
{
	TCHAR *ptcBuffer = *tcBuffer;
	TCHAR *pLastGood;
	TCHAR tcTokenNameEndTag [64];
	LabelListItem myList [] = {
		{_T("usDeptNo"),   1},
		{_T("uchPrintPriority"), 2},
		{_T("lUnitPrice"),  3},
		{_T("uchAdjective"), 4},
		{_T("ControlCode"),  5},
		{_T("uchCondColorPaletteCode"),  6},
		{_T("auchPLUNo"),    7},
		{_T("aszMnemonic"),  8},
		{0,                   0}
	};

	if (iDirection == 1) {
		// translate from an XML string to an ITEMCOND struct
		TCHAR *tcsTokenValue, *tcsToken;
		int i = 0;

		_stprintf (tcTokenNameEndTag, _T("/%s"), *tcsTokenName);
		pLastGood = ptcBuffer;

		while (1) {
			*tcsTokenName = _tcstok (NULL, pTokenDelimitersTag);
			if (*tcsTokenName == NULL)
				break;

			pLastGood = *tcsTokenName;

			if (_tcscmp (*tcsTokenName, tcTokenNameEndTag) == 0)
				break;

			for (i = 0; myList[i].tcsLabel; i++) {
				if (_tcscmp (myList[i].tcsLabel, *tcsTokenName) == 0) {
					break;
				}
			}

			tcsTokenValue = _tcstok (NULL, pTokenDelimitersValue);

			// if there is a problem with parsing then lets just bail out at this point.
			if (tcsTokenValue == 0)
				break;

			// check to see if the text string between the tags is empty
			if (tcsTokenValue[0] == _T('/')) {
				if (_tcscmp (tcsTokenValue+1, *tcsTokenName) == 0)
					continue;
			}

			// check to see if this is an XML singleton type tag such as <tag /> which
			// indicates an empty tag.  While we do not send this type out, we may get them.
			if (_tcscmp (_T("/"), tcsTokenValue) == 0)
				continue;

			tcsToken = _tcstok (NULL, pTokenDelimitersTag);
			if (tcsTokenValue == NULL || tcsToken == NULL)
				break;

			pLastGood = tcsToken;

			// If the tag is not known, then just skip it and go to the next one.
			if (myList[i].tcsLabel == 0)
				continue;

			switch (myList[i].usOffset) {
				case 1:
					TokenToUnsignedNumeric (&(pItemCond->usDeptNo), sizeof(pItemCond->usDeptNo), tcsTokenValue);
					break;
				case 2:
					TokenToUnsignedNumeric (&(pItemCond->uchPrintPriority), sizeof(pItemCond->uchPrintPriority), tcsTokenValue);
					break;
				case 3:
					TokenToSignedNumeric (&(pItemCond->lUnitPrice), sizeof(pItemCond->lUnitPrice), tcsTokenValue);
					break;
				case 4:
					TokenToUnsignedNumeric (&(pItemCond->uchAdjective), sizeof(pItemCond->uchAdjective), tcsTokenValue);
					break;
				case 5:
					ObjectMakerITEMCONTCODE (&tcsTokenValue, tcBuffer, &(pItemCond->ControlCode), iDirection);
					break;
				case 6:
					TokenToUnsignedNumeric (&(pItemCond->uchCondColorPaletteCode), sizeof(pItemCond->uchCondColorPaletteCode), tcsTokenValue);
					break;
				case 7:
					TokenToString (&(pItemCond->auchPLUNo), sizeof(pItemCond->auchPLUNo[0]), sizeof(pItemCond->auchPLUNo), tcsTokenValue);
					break;
				case 8:
					{
						TCHAR  tcsTemp[64];
						TokenToString (tcsTemp, sizeof(tcsTemp[0]), sizeof(tcsTemp), tcsTokenValue);
						ConnEngineTransposeSpecialStrings (&(pItemCond->aszMnemonic[0]), sizeof(pItemCond->aszMnemonic), tcsTemp);
					}
					break;
				default:
					break;
			}
		}

		*tcBuffer = pLastGood + _tcslen(pLastGood) + 1;  // update the buffer pointer so that we can handle multiple messages in the buffer.
	}

	return BL_CONNENGINE_MSG_BUSINESS_OBJECT;
}

int ObjectMakerITEMAFFECTION (TCHAR **tcsTokenName, TCHAR **tcBuffer, ITEMAFFECTION *pItemAffect, int iDirection)
{
	TCHAR *ptcBuffer = *tcBuffer;
	TCHAR *pLastGood;
	TCHAR tcTokenNameEndTag [64];
	LabelListItem myList [] = {
		{_T("uchMajorClass"),   1},
		{_T("uchMinorClass"),   2},
		{_T("lAmount"),         3},
		{_T("lCancel"),         4},
		{_T("ulId"),            5},
		{_T("fbModifier"),      6},  // pItemDisc->fbModifier
		{_T("uchOffset"),       7},   // pItemDisc->uchOffset which is named uchHourlyOffset in other structs
		{0,                     0}
	};

	if (iDirection == 1) {
		// translate from an XML string to an ITEMCOND struct
		TCHAR *tcsTokenValue, *tcsToken;
		int i = 0;

		_stprintf (tcTokenNameEndTag, _T("/%s"), *tcsTokenName);
		pLastGood = ptcBuffer;

		while (1) {
			*tcsTokenName = _tcstok (NULL, pTokenDelimitersTag);
			if (*tcsTokenName == NULL)
				break;

			pLastGood = *tcsTokenName;

			if (_tcscmp (*tcsTokenName, tcTokenNameEndTag) == 0)
				break;

			for (i = 0; myList[i].tcsLabel; i++) {
				if (_tcscmp (myList[i].tcsLabel, *tcsTokenName) == 0) {
					break;
				}
			}

			tcsTokenValue = _tcstok (NULL, pTokenDelimitersValue);

			// if there is a problem with parsing then lets just bail out at this point.
			if (tcsTokenValue == 0)
				break;

			// check to see if the text string between the tags is empty
			if (tcsTokenValue[0] == _T('/')) {
				if (_tcscmp (tcsTokenValue+1, *tcsTokenName) == 0)
					continue;
			}

			tcsToken = _tcstok (NULL, pTokenDelimitersTag);
			if (tcsTokenValue == NULL || tcsToken == NULL)
				break;

			pLastGood = tcsToken;

			// If the tag is not known, then just skip it and go to the next one.
			if (myList[i].tcsLabel == 0)
				continue;

			switch (myList[i].usOffset) {
				case 1:
					TokenToUnsignedNumeric (&(pItemAffect->uchMajorClass), sizeof(pItemAffect->uchMajorClass), tcsTokenValue);
					break;
				case 2:
					TokenToUnsignedNumeric (&(pItemAffect->uchMinorClass), sizeof(pItemAffect->uchMinorClass), tcsTokenValue);
					break;
				case 3:
					TokenToSignedNumeric (&(pItemAffect->lAmount), sizeof(pItemAffect->lAmount), tcsTokenValue);
					break;
				case 4:
					TokenToSignedNumeric (&(pItemAffect->lCancel), sizeof(pItemAffect->lCancel), tcsTokenValue);
					break;
				case 5:
					TokenToUnsignedNumeric (&(pItemAffect->ulId), sizeof(pItemAffect->ulId), tcsTokenValue);
					break;
				case 6:
					TokenToUnsignedNumeric (&(pItemAffect->fbModifier), sizeof(pItemAffect->fbModifier), tcsTokenValue);
					break;
				case 7:
					TokenToUnsignedNumeric (&(pItemAffect->uchOffset), sizeof(pItemAffect->uchOffset), tcsTokenValue);
					break;
				default:
					break;
			}
		}

		pItemAffect->fbModifier |= RETURNS_ORIGINAL;
		*tcBuffer = pLastGood + _tcslen(pLastGood) + 1;  // update the buffer pointer so that we can handle multiple messages in the buffer.
	}

	return BL_CONNENGINE_MSG_BUSINESS_OBJECT;
}

int ObjectMakerITEMDISC (TCHAR **tcsTokenName, TCHAR **tcBuffer, ITEMDISC *pItemDisc, int iDirection)
{
	TCHAR *ptcBuffer = *tcBuffer;
	TCHAR *pLastGood;
	TCHAR tcTokenNameEndTag [64];
	LabelListItem myList [] = {
		{_T("uchMajorClass"),   1},
		{_T("uchMinorClass"),   2},
		{_T("uchDiscountNo"),   3},
		{_T("lAmount"),         4},
		{_T("lQTY"),            5},
		{_T("uchRate"),         6},
		{_T("usReasonCode"),    7},
		{_T("aszMnemonic"),     8},
		{_T("auchPLUNo"),       9},
		{_T("uchSeatNo"),      10},
		{_T("auchStatus_00"),  11},  // pItemDisc->auchDiscStatus[0]
		{_T("auchStatus_01"),  12},  // pItemDisc->auchDiscStatus[1]
		{_T("auchStatus_02"),  13},  // pItemDisc->auchDiscStatus[2]
		{_T("fbModifier"),     14},  // pItemDisc->fbDiscModifier
		{_T("uchHourlyOffset"), 15},  // pItemDisc->uchHourlyOffset
		{0,                     0}
	};

	if (iDirection == 1) {
		// translate from an XML string to an ITEMCOND struct
		TCHAR *tcsTokenValue, *tcsToken;
		int i = 0;

		_stprintf (tcTokenNameEndTag, _T("/%s"), *tcsTokenName);
		pLastGood = ptcBuffer;

		while (1) {
			*tcsTokenName = _tcstok (NULL, pTokenDelimitersTag);
			if (*tcsTokenName == NULL)
				break;

			pLastGood = *tcsTokenName;

			if (_tcscmp (*tcsTokenName, tcTokenNameEndTag) == 0)
				break;

			for (i = 0; myList[i].tcsLabel; i++) {
				if (_tcscmp (myList[i].tcsLabel, *tcsTokenName) == 0) {
					break;
				}
			}

			tcsTokenValue = _tcstok (NULL, pTokenDelimitersValue);

			// if there is a problem with parsing then lets just bail out at this point.
			if (tcsTokenValue == 0)
				break;

			// check to see if the text string between the tags is empty
			if (tcsTokenValue[0] == _T('/')) {
				if (_tcscmp (tcsTokenValue+1, *tcsTokenName) == 0)
					continue;
			}

			// check to see if this is an XML singleton type tag such as <tag /> which
			// indicates an empty tag.  While we do not send this type out, we may get them.
			if (_tcscmp (_T("/"), tcsTokenValue) == 0)
				continue;

			tcsToken = _tcstok (NULL, pTokenDelimitersTag);
			if (tcsTokenValue == NULL || tcsToken == NULL)
				break;

			pLastGood = tcsToken;

			// If the tag is not known, then just skip it and go to the next one.
			if (myList[i].tcsLabel == 0)
				continue;

			switch (myList[i].usOffset) {
				case 1:
					TokenToUnsignedNumeric (&(pItemDisc->uchMajorClass), sizeof(pItemDisc->uchMajorClass), tcsTokenValue);
					break;
				case 2:
					TokenToUnsignedNumeric (&(pItemDisc->uchMinorClass), sizeof(pItemDisc->uchMinorClass), tcsTokenValue);
					break;
				case 3:
					TokenToUnsignedNumeric (&(pItemDisc->uchDiscountNo), sizeof(pItemDisc->uchDiscountNo), tcsTokenValue);
					break;
				case 4:
					TokenToSignedNumeric (&(pItemDisc->lAmount), sizeof(pItemDisc->lAmount), tcsTokenValue);
					break;
				case 5:
					TokenToSignedNumeric (&(pItemDisc->lQTY), sizeof(pItemDisc->lQTY), tcsTokenValue);
					break;
				case 6:
					TokenToUnsignedNumeric (&(pItemDisc->uchRate), sizeof(pItemDisc->uchRate), tcsTokenValue);
					break;
				case 7:
					TokenToUnsignedNumeric (&(pItemDisc->usReasonCode), sizeof(pItemDisc->usReasonCode), tcsTokenValue);
					break;
				case 8:
					{
						TCHAR  tcsTemp[64];
						TokenToString (tcsTemp, sizeof(tcsTemp[0]), sizeof(tcsTemp), tcsTokenValue);
						ConnEngineTransposeSpecialStrings (&(pItemDisc->aszMnemonic[0]), sizeof(pItemDisc->aszMnemonic), tcsTemp);
					}
					break;
				case 9:
					TokenToString (&(pItemDisc->auchPLUNo), sizeof(pItemDisc->auchPLUNo[0]), sizeof(pItemDisc->auchPLUNo), tcsTokenValue);
					break;
				case 10:
					TokenToUnsignedNumeric (&(pItemDisc->uchSeatNo), sizeof(pItemDisc->uchSeatNo), tcsTokenValue);
					break;
				case 11:
					TokenToUnsignedNumeric (&(pItemDisc->auchDiscStatus[0]), sizeof(pItemDisc->auchDiscStatus[0]), tcsTokenValue);
					break;
				case 12:
					TokenToUnsignedNumeric (&(pItemDisc->auchDiscStatus[1]), sizeof(pItemDisc->auchDiscStatus[1]), tcsTokenValue);
					break;
				case 13:
					TokenToUnsignedNumeric (&(pItemDisc->auchDiscStatus[2]), sizeof(pItemDisc->auchDiscStatus[2]), tcsTokenValue);
					break;
				case 14:
					TokenToUnsignedNumeric (&(pItemDisc->fbDiscModifier), sizeof(pItemDisc->fbDiscModifier), tcsTokenValue);
					break;
				case 15:
					TokenToUnsignedNumeric (&(pItemDisc->uchHourlyOffset), sizeof(pItemDisc->uchHourlyOffset), tcsTokenValue);
					break;
				default:
					break;
			}
		}

		pItemDisc->fbDiscModifier |= RETURNS_ORIGINAL;
		*tcBuffer = pLastGood + _tcslen(pLastGood) + 1;  // update the buffer pointer so that we can handle multiple messages in the buffer.
	}

	return BL_CONNENGINE_MSG_BUSINESS_OBJECT;
}

int ObjectMakerITEMTRANSOPEN (TCHAR **tcsTokenName, TCHAR **tcBuffer, ITEMTRANSOPEN *pItemTransOpen, int iDirection)
{
	TCHAR *ptcBuffer = *tcBuffer;
	TCHAR *pLastGood;
	TCHAR tcTokenNameEndTag [64];
	LabelListItem myList [] = {
		{_T("uchMajorClass"),   1},
		{_T("uchMinorClass"),   2},
		{_T("ulCashierID"),     3},
		{_T("usGuestNo"),       4},
		{_T("usTableNo"),       5},
		{_T("uchHourlyOffset"), 6},  // pItemTransOpen->uchHourlyOffset
		{0,                     0}
	};

	if (iDirection == 1) {
		// translate from an XML string to an ITEMCOND struct
		TCHAR *tcsTokenValue, *tcsToken;
		int i = 0;

		_stprintf (tcTokenNameEndTag, _T("/%s"), *tcsTokenName);
		pLastGood = ptcBuffer;

		while (1) {
			*tcsTokenName = _tcstok (NULL, pTokenDelimitersTag);
			if (*tcsTokenName == NULL)
				break;

			pLastGood = *tcsTokenName;

			if (_tcscmp (*tcsTokenName, tcTokenNameEndTag) == 0)
				break;

			for (i = 0; myList[i].tcsLabel; i++) {
				if (_tcscmp (myList[i].tcsLabel, *tcsTokenName) == 0) {
					break;
				}
			}

			tcsTokenValue = _tcstok (NULL, pTokenDelimitersValue);

			// if there is a problem with parsing then lets just bail out at this point.
			if (tcsTokenValue == 0)
				break;

			// check to see if the text string between the tags is empty
			if (tcsTokenValue[0] == _T('/')) {
				if (_tcscmp (tcsTokenValue+1, *tcsTokenName) == 0)
					continue;
			}

			tcsToken = _tcstok (NULL, pTokenDelimitersTag);
			if (tcsTokenValue == NULL || tcsToken == NULL)
				break;

			pLastGood = tcsToken;

			// If the tag is not known, then just skip it and go to the next one.
			if (myList[i].tcsLabel == 0)
				continue;

			switch (myList[i].usOffset) {
				case 1:
					TokenToUnsignedNumeric (&(pItemTransOpen->uchMajorClass), sizeof(pItemTransOpen->uchMajorClass), tcsTokenValue);
					break;
				case 2:
					TokenToUnsignedNumeric (&(pItemTransOpen->uchMinorClass), sizeof(pItemTransOpen->uchMinorClass), tcsTokenValue);
					break;
				case 3:
					TokenToUnsignedNumeric (&(pItemTransOpen->ulCashierID), sizeof(pItemTransOpen->ulCashierID), tcsTokenValue);
					break;
				case 4:
					TokenToSignedNumeric (&(pItemTransOpen->usGuestNo), sizeof(pItemTransOpen->usGuestNo), tcsTokenValue);
					break;
				case 5:
					TokenToUnsignedNumeric (&(pItemTransOpen->usTableNo), sizeof(pItemTransOpen->usTableNo), tcsTokenValue);
					break;
				case 6:
					TokenToUnsignedNumeric (&(pItemTransOpen->uchHourlyOffset), sizeof(pItemTransOpen->uchHourlyOffset), tcsTokenValue);
					break;
				default:
					break;
			}
		}

		*tcBuffer = pLastGood + _tcslen(pLastGood) + 1;  // update the buffer pointer so that we can handle multiple messages in the buffer.
	}

	return BL_CONNENGINE_MSG_BUSINESS_OBJECT;
}

int ObjectMakerEPTRESPONSE (TCHAR **tcsTokenName, TCHAR **tcBuffer, ITEMTENDER *pItemTender, int iDirection)
{
	TCHAR *ptcBuffer = *tcBuffer;
	TCHAR *pLastGood;
	TCHAR tcTokenNameEndTag [64];
	LabelListItem myList [] = {
		{_T("line1"),   1},
		{_T("line2"),   2},
		{_T("line3"),   3},
		{_T("line4"),   4},
		{_T("line5"),   5},
		{_T("line6"),   6},
		{_T("line7"),   7},
		{_T("line8"),   8},
		{_T("line9"),   9},
		{_T("auchStatus"),        10},
		{_T("cardlabel"),         11},
		{_T("auchRecordNumber"),  12},
		{0,                     0}
	};

	if (iDirection == 1) {
		// translate from an XML string to an ITEMCOND struct
		TCHAR *tcsTokenValue, *tcsToken;
		int i = 0;
		SHORT            sRetBatchClose;
		DATE_TIME        DateTimeLastBatchSettlement;

		sRetBatchClose = ParaTendReturnsBatchCloseReadDateTime (&DateTimeLastBatchSettlement);

		_stprintf (tcTokenNameEndTag, _T("/%s"), *tcsTokenName);
		pLastGood = ptcBuffer;

		memset (pItemTender->aszCPMsgText, 0, sizeof(pItemTender->aszCPMsgText));

		while (1) {
			*tcsTokenName = _tcstok (NULL, pTokenDelimitersTag);
			if (*tcsTokenName == NULL)
				break;

			// check to see if this is an XML singleton type tag such as <tag /> which
			// indicates an empty tag.  While we do not send this type out, we may get them.
			if (_tcscmp (_T("/"), *tcsTokenName) == 0)
				break;

			pLastGood = *tcsTokenName;

			if (_tcscmp (*tcsTokenName, tcTokenNameEndTag) == 0)
				break;

			for (i = 0; myList[i].tcsLabel; i++) {
				if (_tcscmp (myList[i].tcsLabel, *tcsTokenName) == 0) {
					break;
				}
			}

			tcsTokenValue = _tcstok (NULL, pTokenDelimitersValue);

			// if there is a problem with parsing then lets just bail out at this point.
			if (tcsTokenValue == 0)
				break;

			// check to see if the text string between the tags is empty
			if (tcsTokenValue[0] == _T('/')) {
				if (_tcscmp (tcsTokenValue+1, *tcsTokenName) == 0)
					continue;
			}

			// check to see if this is an XML singleton type tag such as <tag /> which
			// indicates an empty tag.  While we do not send this type out, we may get them.
			if (_tcscmp (_T("/"), tcsTokenValue) == 0)
				continue;

			tcsToken = _tcstok (NULL, pTokenDelimitersTag);
			if (tcsTokenValue == NULL || tcsToken == NULL)
				break;

			pLastGood = tcsToken;

			// If the tag is not known, then just skip it and go to the next one.
			if (myList[i].tcsLabel == 0)
				continue;

			switch (myList[i].usOffset) {
				case 1:
					for (i = 0; tcsTokenValue[i] && i < ARRAYSIZEOF(pItemTender->aszCPMsgText[0]) - 1; i++) {
						pItemTender->aszCPMsgText[0][i] = tcsTokenValue[i];
					}
					break;
				case 2:
					for (i = 0; tcsTokenValue[i] && i < ARRAYSIZEOF(pItemTender->aszCPMsgText[0]) - 1; i++) {
						pItemTender->aszCPMsgText[1][i] = tcsTokenValue[i];
					}
					break;
				case 3:
					for (i = 0; tcsTokenValue[i] && i < ARRAYSIZEOF(pItemTender->aszCPMsgText[0]) - 1; i++) {
						pItemTender->aszCPMsgText[2][i] = tcsTokenValue[i];
					}
					break;
				case 4:
					for (i = 0; tcsTokenValue[i] && i < ARRAYSIZEOF(pItemTender->aszCPMsgText[0]) - 1; i++) {
						pItemTender->aszCPMsgText[3][i] = tcsTokenValue[i];
					}
					break;
				case 5:
					for (i = 0; tcsTokenValue[i] && i < ARRAYSIZEOF(pItemTender->aszCPMsgText[0]) - 1; i++) {
						pItemTender->aszCPMsgText[4][i] = tcsTokenValue[i];
					}
					break;
				case 6:
					for (i = 0; tcsTokenValue[i] && i < ARRAYSIZEOF(pItemTender->aszCPMsgText[0]) - 1; i++) {
						pItemTender->aszCPMsgText[5][i] = tcsTokenValue[i];
					}
					break;
				case 7:
					for (i = 0; tcsTokenValue[i] && i < ARRAYSIZEOF(pItemTender->aszCPMsgText[0]) - 1; i++) {
						pItemTender->aszCPMsgText[6][i] = tcsTokenValue[i];
					}
					break;
				case 8:
					for (i = 0; tcsTokenValue[i] && i < ARRAYSIZEOF(pItemTender->aszCPMsgText[0]) - 1; i++) {
						pItemTender->aszCPMsgText[7][i] = tcsTokenValue[i];
					}
					break;
				case 9:
					for (i = 0; tcsTokenValue[i] && i < ARRAYSIZEOF(pItemTender->aszCPMsgText[0]) - 1; i++) {
						pItemTender->aszCPMsgText[8][i] = tcsTokenValue[i];
					}
					break;
				case 10:
					{
						TCHAR *tcsApproved = _T("Approved");
						TCHAR *tcsStored = _T("Stored");
						TCHAR *tcsDeclined = _T("Declined");

						if (_tcsncmp (tcsTokenValue, tcsApproved, _tcslen(tcsApproved)) == 0) {
							pItemTender->fbModifier |= APPROVED_EPT_MODIF;
						} else if (_tcsncmp (tcsTokenValue, tcsStored, _tcslen(tcsStored)) == 0) {
							pItemTender->fbModifier |= STORED_EPT_MODIF;
						} else if (_tcsncmp (tcsTokenValue, tcsDeclined, _tcslen(tcsStored)) == 0) {
							pItemTender->fbModifier |= DECLINED_EPT_MODIF;
						}
					}
					break;
				case 11:
					for (i = 0; tcsTokenValue[i] && i < ARRAYSIZEOF(pItemTender->aszCPMsgText[0]) - 1; i++) {
						pItemTender->aszCPMsgText[NUM_CPRSPCO_CARDLABEL][i] = tcsTokenValue[i];
					}
					break;
				case 12:
					for (i = 0; tcsTokenValue[i] && i < ARRAYSIZEOF(pItemTender->srno.auchRecordNumber) - 1; i++) {
						pItemTender->srno.auchRecordNumber[i] = tcsTokenValue[i];
					}
					break;
				default:
					break;
			}
		}

		*tcBuffer = pLastGood + _tcslen(pLastGood) + 1;  // update the buffer pointer so that we can handle multiple messages in the buffer.
	}

	return BL_CONNENGINE_MSG_BUSINESS_OBJECT;
}


int ObjectMakerULSTOREREGNO (TCHAR **tcsTokenName, TCHAR **tcBuffer, ITEMTENDER *pItemTender, int iDirection)
{
	TCHAR *ptcBuffer = *tcBuffer;
	TCHAR *pLastGood;
	TCHAR tcTokenNameEndTag [64];
	LabelListItem myList [] = {
		{_T("usStoreNo"),   1},
		{_T("usRegNo"),     2},
		{0,                     0}
	};

	if (iDirection == 1) {
		// translate from an XML string to an ITEMCOND struct
		TCHAR *tcsTokenValue, *tcsToken;
		int i = 0;

		_stprintf (tcTokenNameEndTag, _T("/%s"), *tcsTokenName);
		pLastGood = ptcBuffer;

		while (1) {
			*tcsTokenName = _tcstok (NULL, pTokenDelimitersTag);
			if (*tcsTokenName == NULL)
				break;

			// check to see if this is an XML singleton type tag such as <tag /> which
			// indicates an empty tag.  While we do not send this type out, we may get them.
			if (_tcscmp (_T("/"), *tcsTokenName) == 0)
				break;

			pLastGood = *tcsTokenName;

			if (_tcscmp (*tcsTokenName, tcTokenNameEndTag) == 0)
				break;

			for (i = 0; myList[i].tcsLabel; i++) {
				if (_tcscmp (myList[i].tcsLabel, *tcsTokenName) == 0) {
					break;
				}
			}

			tcsTokenValue = _tcstok (NULL, pTokenDelimitersValue);

			// if there is a problem with parsing then lets just bail out at this point.
			if (tcsTokenValue == 0)
				break;

			// check to see if the text string between the tags is empty
			if (tcsTokenValue[0] == _T('/')) {
				if (_tcscmp (tcsTokenValue+1, *tcsTokenName) == 0)
					continue;
			}

			// check to see if this is an XML singleton type tag such as <tag /> which
			// indicates an empty tag.  While we do not send this type out, we may get them.
			if (_tcscmp (_T("/"), tcsTokenValue) == 0)
				continue;

			tcsToken = _tcstok (NULL, pTokenDelimitersTag);
			if (tcsTokenValue == NULL || tcsToken == NULL)
				break;

			pLastGood = tcsToken;

			// If the tag is not known, then just skip it and go to the next one.
			if (myList[i].tcsLabel == 0)
				continue;

			switch (myList[i].usOffset) {
				case 1:
					break;
				case 2:
					break;
				default:
					break;
			}
		}

		*tcBuffer = pLastGood + _tcslen(pLastGood) + 1;  // update the buffer pointer so that we can handle multiple messages in the buffer.
	}

	return BL_CONNENGINE_MSG_BUSINESS_OBJECT;
}

int ObjectMakerCHARGETIPS (TCHAR **tcsTokenName, TCHAR **tcBuffer, ITEMTENDER *pItemTender, int iDirection)
{
	TCHAR *ptcBuffer = *tcBuffer;
	TCHAR *pLastGood;
	TCHAR tcTokenNameEndTag [64];
	ITEMTENDERCHARGETIPS *pItemTenderChargeTips = &ITEMTENDER_CHARGETIPS(pItemTender);
	LabelListItem myList [] = {
		{_T("uchMinorClass"),         1},
		{_T("lGratuity"),             2},
		{_T("uchMinorTenderClass"),   3},
		{0,                           0}
	};

	if (iDirection == 1) {
		// translate from an XML string to an ITEMCOND struct
		TCHAR *tcsTokenValue, *tcsToken;
		int i = 0;

		_stprintf (tcTokenNameEndTag, _T("/%s"), *tcsTokenName);
		pLastGood = ptcBuffer;

		while (1) {
			*tcsTokenName = _tcstok (NULL, pTokenDelimitersTag);
			if (*tcsTokenName == NULL)
				break;

			// check to see if this is an XML singleton type tag such as <tag /> which
			// indicates an empty tag.  While we do not send this type out, we may get them.
			if (_tcscmp (_T("/"), *tcsTokenName) == 0)
				break;

			pLastGood = *tcsTokenName;

			if (_tcscmp (*tcsTokenName, tcTokenNameEndTag) == 0)
				break;

			for (i = 0; myList[i].tcsLabel; i++) {
				if (_tcscmp (myList[i].tcsLabel, *tcsTokenName) == 0) {
					break;
				}
			}

			tcsTokenValue = _tcstok (NULL, pTokenDelimitersValue);

			// if there is a problem with parsing then lets just bail out at this point.
			if (tcsTokenValue == 0)
				break;

			// check to see if the text string between the tags is empty
			if (tcsTokenValue[0] == _T('/')) {
				if (_tcscmp (tcsTokenValue+1, *tcsTokenName) == 0)
					continue;
			}

			// check to see if this is an XML singleton type tag such as <tag /> which
			// indicates an empty tag.  While we do not send this type out, we may get them.
			if (_tcscmp (_T("/"), tcsTokenValue) == 0)
				continue;

			tcsToken = _tcstok (NULL, pTokenDelimitersTag);
			if (tcsTokenValue == NULL || tcsToken == NULL)
				break;

			pLastGood = tcsToken;

			// If the tag is not known, then just skip it and go to the next one.
			if (myList[i].tcsLabel == 0)
				continue;

			switch (myList[i].usOffset) {
				case 1:
					TokenToUnsignedNumeric (&(pItemTenderChargeTips->uchMinorClass), sizeof(pItemTenderChargeTips->uchMinorClass), tcsTokenValue);
					pItemTenderChargeTips->uchSignature = 0xff;
					break;
				case 2:
					TokenToSignedNumeric (&(pItemTenderChargeTips->lGratuity), sizeof(pItemTenderChargeTips->lGratuity), tcsTokenValue);
					pItemTenderChargeTips->uchSignature = 0xff;
					break;
				case 3:
					TokenToUnsignedNumeric (&(pItemTenderChargeTips->uchMinorTenderClass), sizeof(pItemTenderChargeTips->uchMinorTenderClass), tcsTokenValue);
					pItemTenderChargeTips->uchSignature = 0xff;
					break;
				default:
					break;
			}
		}

		*tcBuffer = pLastGood + _tcslen(pLastGood) + 1;  // update the buffer pointer so that we can handle multiple messages in the buffer.
	}

	return BL_CONNENGINE_MSG_BUSINESS_OBJECT;
}

int ObjectMakerITEMCOUPON (TCHAR **tcsTokenName, TCHAR **tcBuffer, ITEMCOUPON *pItemCoupon, int iDirection)
{
	TCHAR *ptcBuffer = *tcBuffer;
	TCHAR *pLastGood;
	TCHAR tcTokenNameEndTag [64];
	LabelListItem myList [] = {
		{_T("uchMajorClass"),   1},
		{_T("uchMinorClass"),   2},
		{_T("uchCouponNo"),     3},
		{_T("lAmount"),         4},
		{_T("usReasonCode"),    5},
		{_T("aszMnemonic"),     6},
		{_T("lDoubleAmount"),   7},
		{_T("fbModifier"),      8},
		{_T("fbStatus_0"),      9},
		{_T("fbStatus_1"),     10},
		{_T("uchHourlyOffset"), 11},    // pItemCoupon->uchHourlyOffset
		{_T("usBonusIndex"),    12},    // pItemCoupon->usBonusIndex
		{0,                     0}
	};

	if (iDirection == 1) {
		// translate from an XML string to an ITEMCOND struct
		TCHAR *tcsTokenValue, *tcsToken;
		int i = 0;

		_stprintf (tcTokenNameEndTag, _T("/%s"), *tcsTokenName);
		pLastGood = ptcBuffer;

		while (1) {
			*tcsTokenName = _tcstok (NULL, pTokenDelimitersTag);
			if (*tcsTokenName == NULL)
				break;

			pLastGood = *tcsTokenName;

			if (_tcscmp (*tcsTokenName, tcTokenNameEndTag) == 0)
				break;

			for (i = 0; myList[i].tcsLabel; i++) {
				if (_tcscmp (myList[i].tcsLabel, *tcsTokenName) == 0) {
					break;
				}
			}

			tcsTokenValue = _tcstok (NULL, pTokenDelimitersValue);

			// if there is a problem with parsing then lets just bail out at this point.
			if (tcsTokenValue == 0)
				break;

			// check to see if the text string between the tags is empty
			if (tcsTokenValue[0] == _T('/')) {
				if (_tcscmp (tcsTokenValue+1, *tcsTokenName) == 0)
					continue;
			}

			// check to see if this is an XML singleton type tag such as <tag /> which
			// indicates an empty tag.  While we do not send this type out, we may get them.
			if (_tcscmp (_T("/"), tcsTokenValue) == 0)
				continue;

			tcsToken = _tcstok (NULL, pTokenDelimitersTag);
			if (tcsTokenValue == NULL || tcsToken == NULL)
				break;

			pLastGood = tcsToken;

			// If the tag is not known, then just skip it and go to the next one.
			if (myList[i].tcsLabel == 0)
				continue;

			switch (myList[i].usOffset) {
				case 1:
					TokenToUnsignedNumeric (&(pItemCoupon->uchMajorClass), sizeof(pItemCoupon->uchMajorClass), tcsTokenValue);
					break;
				case 2:
					TokenToUnsignedNumeric (&(pItemCoupon->uchMinorClass), sizeof(pItemCoupon->uchMinorClass), tcsTokenValue);
					break;
				case 3:
					TokenToUnsignedNumeric (&(pItemCoupon->uchCouponNo), sizeof(pItemCoupon->uchCouponNo), tcsTokenValue);
					break;
				case 4:
					TokenToSignedNumeric (&(pItemCoupon->lAmount), sizeof(pItemCoupon->lAmount), tcsTokenValue);
					break;
				case 5:
					TokenToUnsignedNumeric (&(pItemCoupon->usReasonCode), sizeof(pItemCoupon->usReasonCode), tcsTokenValue);
					break;
				case 6:
					{
						TCHAR  tcsTemp[64];
						TokenToString (tcsTemp, sizeof(tcsTemp[0]), sizeof(tcsTemp), tcsTokenValue);
						ConnEngineTransposeSpecialStrings (&(pItemCoupon->aszMnemonic[0]), sizeof(pItemCoupon->aszMnemonic), tcsTemp);
					}
					break;
				case 7:
					TokenToSignedNumeric (&(pItemCoupon->lDoubleAmount), sizeof(pItemCoupon->lDoubleAmount), tcsTokenValue);
					break;
				case 8:
					TokenToUnsignedNumeric (&(pItemCoupon->fbModifier), sizeof(pItemCoupon->fbModifier), tcsTokenValue);
					break;
				case 9:
					TokenToUnsignedNumeric (&(pItemCoupon->fbStatus[0]), sizeof(pItemCoupon->fbStatus[0]), tcsTokenValue);
					break;
				case 10:
					TokenToUnsignedNumeric (&(pItemCoupon->fbStatus[1]), sizeof(pItemCoupon->fbStatus[1]), tcsTokenValue);
					break;
				case 11:
					TokenToUnsignedNumeric (&(pItemCoupon->uchHourlyOffset), sizeof(pItemCoupon->uchHourlyOffset), tcsTokenValue);
					break;
				case 12:
					TokenToUnsignedNumeric (&(pItemCoupon->usBonusIndex), sizeof(pItemCoupon->usBonusIndex), tcsTokenValue);
					break;
				default:
					break;
			}
		}

		pItemCoupon->fbModifier |= RETURNS_ORIGINAL;
		*tcBuffer = pLastGood + _tcslen(pLastGood) + 1;  // update the buffer pointer so that we can handle multiple messages in the buffer.
	}

	return BL_CONNENGINE_MSG_BUSINESS_OBJECT;
}

int ObjectMakerITEMSALES (TCHAR **tcsTokenName, TCHAR **tcBuffer, ITEMSALES *pItemSales, ITEMDISC *pItemDisc, int iDirection)
{
	TCHAR *ptcBuffer = *tcBuffer;
	TCHAR *pLastGood;
	TCHAR tcTokenNameEndTag [64];
	LabelListItem myList [] = {
		{_T("uchMajorClass"), 1},
		{_T("uchMinorClass"), 2},
		{_T("uchSeatNo"),     3},
		{_T("usDeptNo"),      4},
		{_T("lQTY"),          5},
		{_T("lProduct"),      6},
		{_T("ControlCode"),   7},
		{_T("condiment"),     8},
		{_T("auchPLUNo"),     9},
		{_T("aszMnemonic"),  10},
		{_T("lUnitPrice"),   11},
		{_T("uchAdjective"), 12},
		{_T("usUniqueID"),   13},
		{_T("usReasonCode"), 14},
		{_T("sCouponQTY"),   15},
		{_T("discount"),     16},
		{_T("auchStatus_00"),   17},  // pItemSales->ControlCode.auchStatus[PLU_CONTROLCODE_ADR_0]
		{_T("auchStatus_01"),   18},  // pItemSales->ControlCode.auchStatus[PLU_CONTROLCODE_ADR_1]
		{_T("fbModifier"),      19},  // pItemSales->fbModifier
		{_T("aszNumber_0"),     20},  // pItemSales->aszNumber[0]
		{_T("aszNumber_1"),     21},  // pItemSales->aszNumber[1]
		{_T("aszNumber_2"),     22},  // pItemSales->aszNumber[2]
		{_T("aszNumber_3"),     23},  // pItemSales->aszNumber[3]
		{_T("aszNumber_4"),     24},  // pItemSales->aszNumber[4]
		{_T("fsLabelStatus"),   25},
		{_T("usBonusIndex"),    26},
		{_T("uchSeatNo"),       27},
		{_T("uchHourlyOffset"), 28},  // pItemSales->uchHourlyOffset
		{0,                   0}
	};

	if (iDirection == 1) {
		// translate from an XML string to an ITEMSALES struct
		TCHAR *tcsTokenValue, *tcsToken;
		int i = 0;
		int iCondCount = 0;

		pItemSales->uchMajorClass = CLASS_ITEMSALES;
		pItemSales->uchColorPaletteCode = 1;

		_stprintf (tcTokenNameEndTag, _T("/%s"), *tcsTokenName);
		pLastGood = ptcBuffer;

		while (1) {
			*tcsTokenName = _tcstok (NULL, pTokenDelimitersTag);
			if (*tcsTokenName == NULL || **tcsTokenName == 0)
				break;

			pLastGood = *tcsTokenName;

			if (_tcscmp (*tcsTokenName, tcTokenNameEndTag) == 0)
				break;

			for (i = 0; myList[i].tcsLabel; i++) {
				if (_tcscmp (myList[i].tcsLabel, *tcsTokenName) == 0) {
					break;
				}
			}

			switch (myList[i].usOffset) {
				case 7:
					ObjectMakerITEMCONTCODE (tcsTokenName, tcBuffer, &(pItemSales->ControlCode), iDirection);
					continue;
				case 8:
					ObjectMakerITEMCOND (tcsTokenName, tcBuffer, &(pItemSales->Condiment[iCondCount]), iDirection);
					iCondCount++;
					pItemSales->uchCondNo = (UCHAR)iCondCount;
					continue;
				case 16:
					if (pItemDisc) {
						ObjectMakerITEMDISC (tcsTokenName, tcBuffer, pItemDisc, iDirection);
					}
					continue;
				default:
					break;
			}
			tcsTokenValue = _tcstok (NULL, pTokenDelimitersValue);

			// if there is a problem with parsing then lets just bail out at this point.
			if (tcsTokenValue == 0)
				break;

			// check to see if the text string between the tags is empty
			if (tcsTokenValue[0] == _T('/')) {
				if (_tcscmp (tcsTokenValue+1, *tcsTokenName) == 0)
					continue;
			}

			// check to see if this is an XML singleton type tag such as <tag /> which
			// indicates an empty tag.  While we do not send this type out, we may get them.
			if (_tcscmp (_T("/"), tcsTokenValue) == 0)
				continue;

			tcsToken = _tcstok (NULL, pTokenDelimitersTag);
			if (tcsTokenValue == NULL || tcsToken == NULL)
				break;

			pLastGood = tcsToken;

			// If the tag is not known, then just skip it and go to the next one.
			if (myList[i].tcsLabel == 0)
				continue;

			switch (myList[i].usOffset) {
				case 1:
					TokenToUnsignedNumeric (&(pItemSales->uchMajorClass), sizeof(pItemSales->uchMajorClass), tcsTokenValue);
					break;
				case 2:
					TokenToUnsignedNumeric (&(pItemSales->uchMinorClass), sizeof(pItemSales->uchMinorClass), tcsTokenValue);
					break;
				case 3:
					TokenToSignedNumeric (&(pItemSales->uchSeatNo), sizeof(pItemSales->uchSeatNo), tcsTokenValue);
					break;
				case 4:
					TokenToUnsignedNumeric (&(pItemSales->usDeptNo), sizeof(pItemSales->usDeptNo), tcsTokenValue);
					break;
				case 5:
					TokenToSignedNumeric (&(pItemSales->lQTY), sizeof(pItemSales->lQTY), tcsTokenValue);
					break;
				case 6:
					TokenToSignedNumeric (&(pItemSales->lProduct), sizeof(pItemSales->lProduct), tcsTokenValue);
					break;
				case 9:
					TokenToString (&(pItemSales->auchPLUNo), sizeof(pItemSales->auchPLUNo[0]), sizeof(pItemSales->auchPLUNo), tcsTokenValue);
					break;
				case 10:
					{
						TCHAR  tcsTemp[64];
						TokenToString (tcsTemp, sizeof(tcsTemp[0]), sizeof(tcsTemp), tcsTokenValue);
						ConnEngineTransposeSpecialStrings (&(pItemSales->aszMnemonic[0]), sizeof(pItemSales->aszMnemonic), tcsTemp);
					}
					break;
				case 11:
					TokenToSignedNumeric (&(pItemSales->lUnitPrice), sizeof(pItemSales->lUnitPrice), tcsTokenValue);
					break;
				case 12:
					TokenToUnsignedNumeric (&(pItemSales->uchAdjective), sizeof(pItemSales->uchAdjective), tcsTokenValue);
					break;
				case 13:
					TokenToUnsignedNumeric (&(pItemSales->usUniqueID), sizeof(pItemSales->usUniqueID), tcsTokenValue);
					break;
				case 14:
					TokenToUnsignedNumeric (&(pItemSales->usReasonCode), sizeof(pItemSales->usReasonCode), tcsTokenValue);
					break;
				case 15:
					TokenToUnsignedNumeric (&(pItemSales->sCouponQTY), sizeof(pItemSales->sCouponQTY), tcsTokenValue);
					break;
				case 17:
					TokenToUnsignedNumeric (&(pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_0]), sizeof(pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_0]), tcsTokenValue);
					break;
				case 18:
					TokenToUnsignedNumeric (&(pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1]), sizeof(pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_1]), tcsTokenValue);
					break;
				case 19:
					TokenToUnsignedNumeric (&(pItemSales->fbModifier), sizeof(pItemSales->fbModifier), tcsTokenValue);
					break;
				case 20:
					{
						TCHAR  tcsTemp[64];
						TokenToString (tcsTemp, sizeof(tcsTemp[0]), sizeof(tcsTemp), tcsTokenValue);
						ConnEngineTransposeSpecialStrings (&(pItemSales->aszNumber[0][0]), NUM_NUMBER * sizeof(TCHAR), tcsTemp);
					}
					break;
				case 21:
					{
						TCHAR  tcsTemp[64];
						TokenToString (tcsTemp, sizeof(tcsTemp[0]), sizeof(tcsTemp), tcsTokenValue);
						ConnEngineTransposeSpecialStrings (&(pItemSales->aszNumber[1][0]), NUM_NUMBER * sizeof(TCHAR), tcsTemp);
					}
					break;
				case 22:
					{
						TCHAR  tcsTemp[64];
						TokenToString (tcsTemp, sizeof(tcsTemp[0]), sizeof(tcsTemp), tcsTokenValue);
						ConnEngineTransposeSpecialStrings (&(pItemSales->aszNumber[2][0]), NUM_NUMBER * sizeof(TCHAR), tcsTemp);
					}
					break;
				case 23:
					{
						TCHAR  tcsTemp[64];
						TokenToString (tcsTemp, sizeof(tcsTemp[0]), sizeof(tcsTemp), tcsTokenValue);
						ConnEngineTransposeSpecialStrings (&(pItemSales->aszNumber[3][0]), NUM_NUMBER * sizeof(TCHAR), tcsTemp);
					}
					break;
				case 24:
					{
						TCHAR  tcsTemp[64];
						TokenToString (tcsTemp, sizeof(tcsTemp[0]), sizeof(tcsTemp), tcsTokenValue);
						ConnEngineTransposeSpecialStrings (&(pItemSales->aszNumber[4][0]), NUM_NUMBER * sizeof(TCHAR), tcsTemp);
					}
					break;
				case 25:
					TokenToUnsignedNumeric (&(pItemSales->fsLabelStatus), sizeof(pItemSales->fsLabelStatus), tcsTokenValue);
					break;
				case 26:
					TokenToUnsignedNumeric (&(pItemSales->ControlCode.usBonusIndex), sizeof(pItemSales->ControlCode.usBonusIndex), tcsTokenValue);
					break;
				case 27:
					TokenToUnsignedNumeric (&(pItemSales->uchSeatNo), sizeof(pItemSales->uchSeatNo), tcsTokenValue);
					break;
				case 28:
					TokenToUnsignedNumeric (&(pItemSales->uchHourlyOffset), sizeof(pItemSales->uchHourlyOffset), tcsTokenValue);
					break;
				default:
					break;
			}
		}

		pItemSales->fbModifier |= RETURNS_ORIGINAL;
		pItemSales->fsLabelStatus |= ITM_CONTROL_RETURN_OLD;   // ensure we know this is original item to prevent consolidation with new items.
		*tcBuffer = pLastGood + _tcslen(pLastGood) + 1;  // update the buffer pointer so that we can handle multiple messages in the buffer.
	}

	return BL_CONNENGINE_MSG_BUSINESS_OBJECT;
}


int ObjectMakerITEMTOTAL (TCHAR **tcsTokenName, TCHAR **tcBuffer, ITEMTOTAL *pItemTotal, int iDirection)
{
	TCHAR *ptcBuffer = *tcBuffer;
	TCHAR *pLastGood;
	TCHAR tcTokenNameEndTag [64];
	LabelListItem myList [] = {
		{_T("uchMajorClass"),    1},
		{_T("uchMinorClass"),    2},
		{_T("lMI"),              3},
		{_T("lService"),         4},
		{_T("lTax"),             5},
		{_T("aszNumber_0"),      6},
		{0,                   0}
	};

	if (iDirection == 1) {
		// translate from an XML string to an ITEMSALES struct
		TCHAR *tcsTokenValue, *tcsToken;
		int i = 0;
		int iCondCount = 0;

		pItemTotal->uchMajorClass = CLASS_ITEMTOTAL;
		_stprintf (tcTokenNameEndTag, _T("/%s"), *tcsTokenName);
		pLastGood = ptcBuffer;

		while (1) {
			*tcsTokenName = _tcstok (NULL, pTokenDelimitersTag);
			if (*tcsTokenName == NULL || **tcsTokenName == 0)
				break;

			pLastGood = *tcsTokenName;

			if (_tcscmp (*tcsTokenName, tcTokenNameEndTag) == 0)
				break;

			for (i = 0; myList[i].tcsLabel; i++) {
				if (_tcscmp (myList[i].tcsLabel, *tcsTokenName) == 0) {
					break;
				}
			}

			// check to see if this tag has an empty or not specified value.
			{
				int    j;
				for (j = 0; (*tcsTokenName)[j]; j++) {;}
				j++;
				if ((*tcsTokenName)[j] != _T('<')) {
					tcsTokenValue = _tcstok (NULL, pTokenDelimitersValue);
					tcsToken = _tcstok (NULL, pTokenDelimitersTag);
					if (tcsTokenValue == NULL || tcsToken == NULL)
						break;
				} else {
					// this is an empty tag pair so just eat the ending tag and continue.
					tcsToken = _tcstok (NULL, pTokenDelimitersTag);
					continue;
				}
			}

			pLastGood = tcsToken;

			// If the tag is not known, then just skip it and go to the next one.
			if (myList[i].tcsLabel == 0)
				continue;

			switch (myList[i].usOffset) {
				case 1:
					TokenToUnsignedNumeric (&(pItemTotal->uchMajorClass), sizeof(pItemTotal->uchMajorClass), tcsTokenValue);
					break;
				case 2:
					TokenToUnsignedNumeric (&(pItemTotal->uchMinorClass), sizeof(pItemTotal->uchMinorClass), tcsTokenValue);
					break;
				case 3:
					TokenToSignedNumeric (&(pItemTotal->lMI), sizeof(pItemTotal->lMI), tcsTokenValue);
					break;
				case 4:
					TokenToSignedNumeric (&(pItemTotal->lService), sizeof(pItemTotal->lService), tcsTokenValue);
					break;
				case 5:
					TokenToSignedNumeric (&(pItemTotal->lTax), sizeof(pItemTotal->lTax), tcsTokenValue);
					break;
				case 6:
					{
						TCHAR  tcsTemp[64];
						TokenToString (tcsTemp, sizeof(tcsTemp[0]), sizeof(tcsTemp), tcsTokenValue);
						ConnEngineTransposeSpecialStrings (&(pItemTotal->aszNumber[0]), NUM_NUMBER * sizeof(TCHAR), tcsTemp);
					}
					break;
				default:
					break;
			}
		}

		pItemTotal->fbModifier |= RETURNS_ORIGINAL;
		*tcBuffer = pLastGood + _tcslen(pLastGood) + 1;  // update the buffer pointer so that we can handle multiple messages in the buffer.
	}

	return BL_CONNENGINE_MSG_BUSINESS_OBJECT;
}

int ObjectMakerITEMTENDER (TCHAR **tcsTokenName, TCHAR **tcBuffer, ITEMTENDER *pItemTender, int iDirection)
{
	TCHAR *ptcBuffer = *tcBuffer;
	TCHAR *pLastGood;
	TCHAR tcTokenNameEndTag [64];
	LabelListItem myList [] = {
		{_T("uchMajorClass"),    1},
		{_T("uchMinorClass"),    2},
		{_T("lTenderAmount"),    3},
		{_T("lForeignAmount"),   4},
		{_T("lBalanceDue"),      5},
		{_T("lChange"),          6},
		{_T("usReasonCode"),     7},
		{_T("tchAuthCode"),      8},
		{_T("tchRefNum"),        9},
		{_T("tchInvoiceNum"),   10},
		{_T("eptresponse"),     11},
		{_T("ulStoreregNo"),    12},
		{_T("aszNumber_0"),     13},
		{_T("usCheckTenderId"), 14},
		{_T("aszPostTransNo"),  15},
		{_T("fbModifier"),      16},
		{_T("tenderdate"),      17},
		{_T("lGratuity"),       18},
		{_T("chargetips"),      19},
		{_T("tchCardType"),     20},
		{0,                   0}
	};

	if (iDirection == 1) {
		// translate from an XML string to an ITEMSALES struct
		TCHAR *tcsTokenValue, *tcsToken;
		int i = 0;
		int iCondCount = 0;

		pItemTender->uchMajorClass = CLASS_ITEMTENDER;
		_stprintf (tcTokenNameEndTag, _T("/%s"), *tcsTokenName);
		pLastGood = ptcBuffer;

		while (1) {
			*tcsTokenName = _tcstok (NULL, pTokenDelimitersTag);
			if (*tcsTokenName == NULL || **tcsTokenName == 0)
				break;

			pLastGood = *tcsTokenName;

			if (_tcscmp (*tcsTokenName, tcTokenNameEndTag) == 0)
				break;

			for (i = 0; myList[i].tcsLabel; i++) {
				if (_tcscmp (myList[i].tcsLabel, *tcsTokenName) == 0) {
					break;
				}
			}

			switch (myList[i].usOffset) {
				case 11:
					ObjectMakerEPTRESPONSE (tcsTokenName, tcBuffer, pItemTender, iDirection);
					continue;
				case 12:
					ObjectMakerULSTOREREGNO (tcsTokenName, tcBuffer, pItemTender, iDirection);
					continue;
				case 19:
					ObjectMakerCHARGETIPS (tcsTokenName, tcBuffer, pItemTender, iDirection);
					continue;
				default:
					break;
			}


			// check to see if this tag has an empty or not specified value.
			{
				int    j;
				for (j = 0; (*tcsTokenName)[j]; j++) {;}
				j++;
				if ((*tcsTokenName)[j] != _T('<')) {
					tcsTokenValue = _tcstok (NULL, pTokenDelimitersValue);

					// if there is a problem with parsing then lets just bail out at this point.
					if (tcsTokenValue == 0)
						break;

					// check to see if the text string between the tags is empty
					if (tcsTokenValue[0] == _T('/')) {
						if (_tcscmp (tcsTokenValue+1, *tcsTokenName) == 0)
							continue;
					}

					// check to see if this is an XML singleton type tag such as <tag /> which
					// indicates an empty tag.  While we do not send this type out, we may get them.
					if (_tcscmp (_T("/"), tcsTokenValue) == 0)
						continue;

					tcsToken = _tcstok (NULL, pTokenDelimitersTag);
					if (tcsTokenValue == NULL || tcsToken == NULL)
						break;
				} else {
					// this is an empty tag pair so just eat the ending tag and continue.
					tcsToken = _tcstok (NULL, pTokenDelimitersTag);
					continue;
				}
			}

			pLastGood = tcsToken;

			// If the tag is not known, then just skip it and go to the next one.
			if (myList[i].tcsLabel == 0)
				continue;

			switch (myList[i].usOffset) {
				case 1:
					TokenToUnsignedNumeric (&(pItemTender->uchMajorClass), sizeof(pItemTender->uchMajorClass), tcsTokenValue);
					break;
				case 2:
					TokenToUnsignedNumeric (&(pItemTender->uchMinorClass), sizeof(pItemTender->uchMinorClass), tcsTokenValue);
					break;
				case 3:
					TokenToSignedNumeric (&(pItemTender->lTenderAmount), sizeof(pItemTender->lTenderAmount), tcsTokenValue);
					break;
				case 4:
					TokenToSignedNumeric (&(pItemTender->lForeignAmount), sizeof(pItemTender->lForeignAmount), tcsTokenValue);
					break;
				case 5:
					TokenToSignedNumeric (&(pItemTender->lBalanceDue), sizeof(pItemTender->lBalanceDue), tcsTokenValue);
					break;
				case 6:
					TokenToSignedNumeric (&(pItemTender->lChange), sizeof(pItemTender->lChange), tcsTokenValue);
					break;
				case 7:
					TokenToUnsignedNumeric (&(pItemTender->usReasonCode), sizeof(pItemTender->usReasonCode), tcsTokenValue);
					break;
				case 8:
					TokenToString (&(pItemTender->authcode.auchAuthCode), sizeof(pItemTender->authcode.auchAuthCode[0]), sizeof(pItemTender->authcode.auchAuthCode), tcsTokenValue);
					break;
				case 9:
					TokenToString (&(pItemTender->refno.auchReferncNo), sizeof(pItemTender->refno.auchReferncNo[0]), sizeof(pItemTender->refno.auchReferncNo), tcsTokenValue);
					break;
				case 10:
					TokenToString (&(pItemTender->invno.auchInvoiceNo), sizeof(pItemTender->invno.auchInvoiceNo[0]), sizeof(pItemTender->invno.auchInvoiceNo), tcsTokenValue);
					break;
				case 13:
					{
						TCHAR  tcsTemp[64];
						TokenToString (tcsTemp, sizeof(tcsTemp[0]), sizeof(tcsTemp), tcsTokenValue);
						ConnEngineTransposeSpecialStrings (&(pItemTender->aszNumber[0]), NUM_NUMBER * sizeof(TCHAR), tcsTemp);
//						RflEncryptByteString ((UCHAR *)&(pItemTender->aszNumber[0]), sizeof(pItemTender->aszNumber));
					}
					break;
				case 14:
					TokenToUnsignedNumeric (&(pItemTender->usCheckTenderId), sizeof(pItemTender->usCheckTenderId), tcsTokenValue);
					break;
				case 15:
					TokenToString (&(pItemTender->aszPostTransNo), sizeof(pItemTender->aszPostTransNo[0]), sizeof(pItemTender->aszPostTransNo), tcsTokenValue);
					break;
				case 16:
					TokenToUnsignedNumeric (&(pItemTender->fbModifier), sizeof(pItemTender->fbModifier), tcsTokenValue);
					break;
				case 17:
					{
						USHORT  usValue = 0;
						int     iLoop = 0;
						for (iLoop = 0; tcsTokenValue[iLoop] && iLoop < 12; iLoop++) {
							switch (iLoop) {
								case 0:
									usValue = 0;
									break;
								case 2:
									pItemTender->uchTenderYear = (UCHAR)usValue;
									usValue = 0;
									break;
								case 4:
									pItemTender->uchTenderMonth = (UCHAR)usValue;
									usValue = 0;
									break;
								case 6:
									pItemTender->uchTenderDay = (UCHAR)usValue;
									usValue = 0;
								case 8:
									pItemTender->uchTenderHour = (UCHAR)usValue;
									usValue = 0;
								case 10:
									pItemTender->uchTenderMinute = (UCHAR)usValue;
									usValue = 0;
									break;
								case 12:
									pItemTender->uchTenderSecond = (UCHAR)usValue;
									usValue = 0;
									break;
								default:
									break;
							}
							if (tcsTokenValue[iLoop] >= '0' && tcsTokenValue[iLoop] <= '9')
								usValue = usValue * 10 + tcsTokenValue[iLoop] - '0';
						}
						if (iLoop == 12) {
							pItemTender->uchTenderSecond = (UCHAR)usValue;
						}
					}
					break;
				case 18:
					TokenToSignedNumeric (&(pItemTender->lGratuity), sizeof(pItemTender->lGratuity), tcsTokenValue);
					break;
				case 20:
					{
						TCHAR  tcsTemp[64];
						TokenToString (tcsTemp, sizeof(tcsTemp[0]), sizeof(tcsTemp), tcsTokenValue);
						ConnEngineTransposeSpecialStrings (pItemTender->ITEMTENDER_CARDTYPE, sizeof(pItemTender->ITEMTENDER_CARDTYPE), tcsTemp);
					}
					break;
				default:
					break;
			}
		}

		pItemTender->fbModifier |= RETURNS_ORIGINAL;
		*tcBuffer = pLastGood + _tcslen(pLastGood) + 1;  // update the buffer pointer so that we can handle multiple messages in the buffer.
	}

	return BL_CONNENGINE_MSG_BUSINESS_OBJECT;
}


int ObjectMakerTRANSACTION (TCHAR **tcsTokenName, TCHAR **tcBuffer, TRANSACTION *pTransaction, int iDirection)
{
	TCHAR *ptcBuffer = *tcBuffer;
	TCHAR *pLastGood;
	TCHAR tcTokenNameEndTag [64];
	LabelListItem myList [] = {
		{_T("consecnumber"),           1},    // transaction memory resident data base flags used with discounts
		{_T("tcsActionType"),          2},
		{_T("uchUniqueIdentifier"),    3},    // transaction memory resident data base flags used with discounts
		{_T("item"),                   4},
		{_T("tender"),                 5},
		{_T("discount"),               6},
		{_T("coupon"),                 7},
		{_T("lErrorCode"),            10},
		{_T("tranopen"),              11},
		{_T("total"),                 12},
		{_T("affection"),             13},
		{_T("fsGCFStatus"),           14},    // transaction memory resident data base flags used with discounts
		{_T("fsGCFStatus2"),          15},    // transaction memory resident data base flags used with discounts
		{_T("tipsreturn"),            16},
		{0,                            0}
	};

	if (iDirection == 1) {
		// translate from an XML string to an TRANSACTION struct
		TCHAR	*tcsTokenValue, *tcsToken;
		ULONG   ulActualBytesRead;
		int		i = 0;
		int		iCondCount = 0;
		SHORT   sTempFileHandle;
		TRANGCFQUAL      WorkQual;
		TRANITEMIZERS    WorkItemizers;
		USHORT           usTranConsStoVli = 0;
		USHORT           usCheckTenderIdMax = 0;

		memset (pTransaction, 0, sizeof(TRANSACTION));  // Init the TRANSACTION to zeros to prepare for processing the XML message.

		_tcscpy (pTransaction->aszTempFileName, _T("OBJTEMP"));

		sTempFileHandle = PifOpenFile (pTransaction->aszTempFileName, auchCP_TEMP_NEW_FILE_READ_WRITE);
		if (sTempFileHandle < 0) {
			PifDeleteFile (pTransaction->aszTempFileName);
			sTempFileHandle = PifOpenFile (pTransaction->aszTempFileName, auchCP_TEMP_NEW_FILE_READ_WRITE);
		}

		// Prepare the output file that will contain the transaction data.
		// The file structure
		memset (&WorkQual, 0, sizeof(WorkQual));
		memset (&WorkItemizers, 0, sizeof(WorkItemizers));
		PifSeekFile (sTempFileHandle, sizeof(USHORT), &ulActualBytesRead);  // SERTMPFILE_DATASTART
		PifWriteFile(sTempFileHandle, &WorkQual, sizeof(WorkQual));
		PifWriteFile(sTempFileHandle, &WorkItemizers, sizeof(WorkItemizers));
		PifWriteFile(sTempFileHandle, &usTranConsStoVli, sizeof(usTranConsStoVli));

		_stprintf (tcTokenNameEndTag, _T("/%s"), *tcsTokenName);
		pLastGood = ptcBuffer;

		while (1) {
			*tcsTokenName = _tcstok (NULL, pTokenDelimitersTag);
			if (*tcsTokenName == NULL || **tcsTokenName == 0)
				break;

			pLastGood = *tcsTokenName;

			if (_tcscmp (*tcsTokenName, tcTokenNameEndTag) == 0)
				break;

			for (i = 0; myList[i].tcsLabel; i++) {
				if (_tcscmp (myList[i].tcsLabel, *tcsTokenName) == 0) {
					break;
				}
			}

			switch (myList[i].usOffset) {
				case 4:
					{
						ITEMSALES ItemSales;
						ITEMDISC  ItemDisc;

						memset (&ItemSales, 0, sizeof(ItemSales));
						memset (&ItemDisc, 0, sizeof(ItemDisc));
						ObjectMakerITEMSALES (tcsTokenName, tcBuffer, &ItemSales, &ItemDisc, iDirection);
						PifWriteFile(sTempFileHandle, &ItemSales, sizeof(ItemSales));
						if (ItemDisc.uchMajorClass)
							PifWriteFile(sTempFileHandle, &ItemDisc, sizeof(ItemDisc));
					}
					break;

				case 5:
					{
						ITEMTENDER ItemTender;

						memset (&ItemTender, 0, sizeof(ItemTender));
						ObjectMakerITEMTENDER (tcsTokenName, tcBuffer, &ItemTender, iDirection);
						PifWriteFile(sTempFileHandle, &ItemTender, sizeof(ItemTender));
						if (ItemTender.usCheckTenderId > usCheckTenderIdMax) usCheckTenderIdMax = ItemTender.usCheckTenderId;
					}
					break;

				case 6:
					{
						ITEMDISC       ItemDisc;

						memset (&ItemDisc, 0, sizeof(ItemDisc));
						ObjectMakerITEMDISC (tcsTokenName, tcBuffer, &ItemDisc, iDirection);
						PifWriteFile(sTempFileHandle, &ItemDisc, sizeof(ItemDisc));
					}
					break;

				case 7:
					{
						ITEMCOUPON     ItemCoupon;

						memset (&ItemCoupon, 0, sizeof(ItemCoupon));
						ObjectMakerITEMCOUPON (tcsTokenName, tcBuffer, &ItemCoupon, iDirection);
						PifWriteFile(sTempFileHandle, &ItemCoupon, sizeof(ItemCoupon));
					}
					break;

				case 11:
					{
						ITEMTRANSOPEN     ItemTransOpen;

						memset (&ItemTransOpen, 0, sizeof(ItemTransOpen));
						ObjectMakerITEMTRANSOPEN (tcsTokenName, tcBuffer, &ItemTransOpen, iDirection);
//						PifWriteFile(sTempFileHandle, &ItemCoupon, sizeof(ItemCoupon));
					}
					break;

				case 12:
					{
						ITEMTOTAL ItemTotal;

						memset (&ItemTotal, 0, sizeof(ItemTotal));
						ObjectMakerITEMTOTAL (tcsTokenName, tcBuffer, &ItemTotal, iDirection);
						PifWriteFile(sTempFileHandle, &ItemTotal, sizeof(ItemTotal));
					}
					break;

				case 13:
					{
						ITEMAFFECTION ItemAffect;

						memset (&ItemAffect, 0, sizeof(ItemAffect));
						ObjectMakerITEMAFFECTION (tcsTokenName, tcBuffer, &ItemAffect, iDirection);
//						PifWriteFile(sTempFileHandle, &ItemAffect, sizeof(ItemAffect));
					}
					break;

				case 16:     // tag tipsreturn
					{
						ITEMTENDER ItemTender;

						memset (&ItemTender, 0, sizeof(ItemTender));
						ObjectMakerITEMTENDER (tcsTokenName, tcBuffer, &ItemTender, iDirection);
						PifWriteFile(sTempFileHandle, &ItemTender, sizeof(ItemTender));
						if (ItemTender.usCheckTenderId > usCheckTenderIdMax) usCheckTenderIdMax = ItemTender.usCheckTenderId;
					}
					break;

				default:
					tcsTokenValue = _tcstok (NULL, pTokenDelimitersValue);

					// if there is a problem with parsing then lets just bail out at this point.
					if (tcsTokenValue == 0)
						break;

					// check to see if the text string between the tags is empty
					if (tcsTokenValue[0] == _T('/')) {
						if (_tcscmp (tcsTokenValue+1, *tcsTokenName) == 0)
							continue;
					}

					// check to see if this is an XML singleton type tag such as <tag /> which
					// indicates an empty tag.  While we do not send this type out, we may get them.
					if (_tcscmp (_T("/"), tcsTokenValue) == 0)
						continue;

					tcsToken = _tcstok (NULL, pTokenDelimitersTag);
					if (tcsTokenValue == NULL || tcsToken == NULL) {
						break;
					}

					pLastGood = tcsToken;

					// If the tag is not known, then just skip it and go to the next one.
					if (myList[i].tcsLabel == 0)
						continue;

					switch (myList[i].usOffset) {
						case 1:
							TokenToSignedNumeric (&(pTransaction->consecnumber), sizeof(pTransaction->consecnumber), tcsTokenValue);
							break;

						case 2:
							TokenToString (&(pTransaction->tcsActionType), sizeof(pTransaction->tcsActionType[0]), sizeof(pTransaction->tcsActionType), tcsTokenValue);
							break;

						case 3:
							{
								SHORT  i;
								TCHAR  tchUniqueIdentifier[32];

								memset (tchUniqueIdentifier, 0, sizeof(tchUniqueIdentifier));
								memset (pTransaction->uchUniqueIdentifier, 0, sizeof(pTransaction->uchUniqueIdentifier));
								memset (WorkQual.uchUniqueIdentifier, 0, sizeof(WorkQual.uchUniqueIdentifier));
								TokenToString (tchUniqueIdentifier, sizeof(tchUniqueIdentifier[0]), sizeof(tchUniqueIdentifier), tcsTokenValue);
								for (i = 0; tchUniqueIdentifier[i] && i < sizeof(WorkQual.uchUniqueIdentifier)/sizeof(WorkQual.uchUniqueIdentifier[0]); i++) {
									pTransaction->uchUniqueIdentifier[i] = WorkQual.uchUniqueIdentifier[i] = (tchUniqueIdentifier[i] & 0x0f);
								}
							}
							break;

						case 10:
							TokenToSignedNumeric (&(pTransaction->lErrorCode), sizeof(pTransaction->lErrorCode), tcsTokenValue);
							break;

						case 14:
							// Since these values are going into the memory resident data base, they also need to be processed
							// in function CliGusLoadFromConnectionEngine() so that the memory resident data base is updated
							// with these global flags.
							TokenToUnsignedNumeric (&(WorkQual.fsGCFStatus), sizeof(WorkQual.fsGCFStatus), tcsTokenValue);
							break;

						case 15:
							// Since these values are going into the memory resident data base, they also need to be processed
							// in function CliGusLoadFromConnectionEngine() so that the memory resident data base is updated
							// with these global flags.
							TokenToUnsignedNumeric (&(WorkQual.fsGCFStatus2), sizeof(WorkQual.fsGCFStatus2), tcsTokenValue);
							break;

						default:
							break;
					}
					break;
			}
		}

		usCheckTenderIdMax++;
		WorkQual.usCheckTenderId = usCheckTenderIdMax;
		PifSeekFile (sTempFileHandle, sizeof(USHORT), &ulActualBytesRead);
		PifWriteFile(sTempFileHandle, &WorkQual, sizeof(WorkQual));

		*tcBuffer = pLastGood + _tcslen(pLastGood) + 1;  // update the buffer pointer so that we can handle multiple messages in the buffer.
		PifCloseFile (sTempFileHandle);
	}

	return BL_CONNENGINE_MSG_TRANSACTION;
}

int ObjectMakerEMPLOYEECHANGE (TCHAR **tcsTokenName, TCHAR **tcBuffer, EMPLOYEECHANGE *pEmployeeChange, int iDirection)
{
	TCHAR *ptcBuffer = *tcBuffer;
	TCHAR *pLastGood;
	TCHAR tcTokenNameEndTag [64];
	LabelListItem myList [] = {
		{_T("tcsActionType"),      1},
		{_T("ulEmployeeId"),       2},
		{_T("tcsStatus"),          3},
		{_T("tcsMnemonic"),        4},
		{_T("usJobCode"),          5},
		{0,                        0}
	};

	if (iDirection == 1) {
		// translate from an XML string to an EMPLOYEECHANGE struct
		TCHAR *tcsTokenValue, *tcsToken;
		int i = 0;
		int iCondCount = 0;

		memset (pEmployeeChange, 0, sizeof(EMPLOYEECHANGE));  // Init the EMPLOYEECHANGE to zeros to prepare for processing the XML message.

		_stprintf (tcTokenNameEndTag, _T("/%s"), *tcsTokenName);
		pLastGood = ptcBuffer;

		while (1) {
			*tcsTokenName = _tcstok (NULL, pTokenDelimitersTag);
			if (*tcsTokenName == NULL || **tcsTokenName == 0)
				break;

			pLastGood = *tcsTokenName;

			if (_tcscmp (*tcsTokenName, tcTokenNameEndTag) == 0)
				break;

			for (i = 0; myList[i].tcsLabel; i++) {
				if (_tcscmp (myList[i].tcsLabel, *tcsTokenName) == 0) {
					break;
				}
			}

			tcsTokenValue = _tcstok (NULL, pTokenDelimitersValue);

			// if there is a problem with parsing then lets just bail out at this point.
			if (tcsTokenValue == 0)
				break;

			// check to see if the text string between the tags is empty
			if (tcsTokenValue[0] == _T('/')) {
				if (_tcscmp (tcsTokenValue+1, *tcsTokenName) == 0)
					continue;
			}

			// check to see if this is an XML singleton type tag such as <tag /> which
			// indicates an empty tag.  While we do not send this type out, we may get them.
			if (_tcscmp (_T("/"), tcsTokenValue) == 0)
				continue;

			tcsToken = _tcstok (NULL, pTokenDelimitersTag);
			if (tcsTokenValue == NULL || tcsToken == NULL)
				break;

			pLastGood = tcsToken;

			// If the tag is not known, then just skip it and go to the next one.
			if (myList[i].tcsLabel == 0)
				continue;

			switch (myList[i].usOffset) {
				case 1:
					TokenToString (&(pEmployeeChange->tcsActionType), sizeof(pEmployeeChange->tcsActionType[0]), sizeof(pEmployeeChange->tcsActionType), tcsTokenValue);
					break;

				case 2:
					TokenToUnsignedNumeric (&(pEmployeeChange->ulEmployeeId), sizeof(pEmployeeChange->ulEmployeeId), tcsTokenValue);
					break;

				case 3:
					TokenToString (&(pEmployeeChange->tcsStatus), sizeof(pEmployeeChange->tcsStatus[0]), sizeof(pEmployeeChange->tcsStatus), tcsTokenValue);
					break;

				case 4:
					{
						TCHAR  tcsTemp[64];
						TokenToString (tcsTemp, sizeof(tcsTemp[0]), sizeof(tcsTemp), tcsTokenValue);
						ConnEngineTransposeSpecialStrings (&(pEmployeeChange->tcsMnemonic[0]), sizeof(pEmployeeChange->tcsMnemonic), tcsTemp);
					}
					break;

				case 5:
					TokenToUnsignedNumeric (&(pEmployeeChange->usJobCode), sizeof(pEmployeeChange->usJobCode), tcsTokenValue);
					break;

				default:
					break;
			}
		}

		*tcBuffer = pLastGood + _tcslen(pLastGood) + 1;  // update the buffer pointer so that we can handle multiple messages in the buffer.
	}

	return BL_CONNENGINE_MSG_EMPLOYEECHANGE;
}


int ObjectMakerFRAMEWORK (TCHAR **tcsTokenName, TCHAR **tcBuffer, FRAMEWORK *pFrameWork, int iDirection)
{
	TCHAR *ptcBuffer = *tcBuffer;
	TCHAR *pLastGood;
	TCHAR tcTokenNameEndTag [64];
	LabelListItem myList [] = {
		{_T("windowname"),      1},
		{_T("controlname"),     2},
		{_T("controltext"),     3},
		{_T("controliconfile"), 4},
		{_T("enablecontrol"),   5},
		{_T("displaywindow"),   6},
		{_T("displaycontrol"),  7},
		{_T("controlbgcolor"),  8},
		{_T("terminal"),        9},
		{_T("controltextcolor"),  10},
		{_T("extendedfsc"),       11},
		{_T("windefaultbtncolorbg"),  12},
		{_T("windefaultbtncolortxt"), 13},
		{_T("windefaultbtnwidth"),    14},
		{_T("windefaultbtnheight"),   15},
		{_T("!--"),              255},
		{0,                     0}
	};

	if (iDirection == 1) {
		// translate from an XML string to an FRAMEWORK struct
		TCHAR *tcsTokenValue, *tcsToken;
		int i = 0;
		int iCondCount = 0;

		memset (pFrameWork, 0, sizeof(FRAMEWORK));  // Init the FRAMEWORK to zeros to prepare for processing the XML message.
		pFrameWork->controlbgcolor = 0xFFFFFFFF;    // default to no color change
		pFrameWork->controltextcolor = 0xFFFFFFFF;  // default to no color change
		pFrameWork->windefaultbtncolorbg = 0xFFFFFFFF;   // default to no color change
		pFrameWork->windefaultbtncolortxt = 0xFFFFFFFF;  // default to no color change
		pFrameWork->sTerminalNumber = -1;           // default to this terminal only

		_stprintf (tcTokenNameEndTag, _T("/%s"), *tcsTokenName);
		pLastGood = ptcBuffer;

		while (1) {
			*tcsTokenName = _tcstok (NULL, pTokenDelimitersTag);
			if (*tcsTokenName == NULL || **tcsTokenName == 0)
				break;

			pLastGood = *tcsTokenName;

			if (_tcscmp (*tcsTokenName, tcTokenNameEndTag) == 0)
				break;

			for (i = 0; myList[i].tcsLabel; i++) {
				if (_tcscmp (myList[i].tcsLabel, *tcsTokenName) == 0) {
					break;
				}
			}

			if (myList[i].usOffset == 255) {
				// we have found a comment so we need to scan through until we find the end of the comment
				// and then restart our parsing.
				TCHAR *tcsStart = pLastGood + _tcslen(pLastGood) + 1;
				TCHAR *tcsStart2;

				tcsStart2 = _tcsstr (tcsStart, _T("-->"));
				if (tcsStart2 && tcsStart2 > tcsStart) {
					TCHAR *tcsComment = pLastGood + _tcslen(pLastGood) + 1;
					*(tcsStart2 - 1) = 0;  // insert an end of string before the end of comment
					// a valid XML comment with an end, see if we should issue an ASSRTLOG log.
					tcsComment = _tcsstr (tcsComment, _T("#note"));
					if (tcsComment) {
						char xBuff[128];
						if (_tcslen (tcsComment) > 50)
							*(tcsComment + 50) = 0;
						sprintf (xBuff, "%S", tcsComment);
						NHPOS_NONASSERT_TEXT(xBuff);
					}
				}

				*tcsTokenName = _tcstok (tcsStart2, pTokenDelimitersTag);
				// we just throw away this token since it should be -- where we are starting.
				// if the end of comment text is not found, tcsStart will be NULL so the
				// the next token found will returned instead which will be thrown away.

				continue;
			}

			tcsTokenValue = _tcstok (NULL, pTokenDelimitersValue);

			// if there is a problem with parsing then lets just bail out at this point.
			if (tcsTokenValue == 0)
				break;

			// check to see if the text string between the tags is empty
			if (tcsTokenValue[0] == _T('/')) {
				if (_tcscmp (tcsTokenValue+1, *tcsTokenName) == 0)
					continue;
			}

			tcsToken = _tcstok (NULL, pTokenDelimitersTag);
			if (tcsTokenValue == NULL || tcsToken == NULL)
				break;

			pLastGood = tcsToken;

			// If the tag is not known, then just skip it and go to the next one.
			if (myList[i].tcsLabel == 0)
				continue;

			switch (myList[i].usOffset) {
				case 1:
					TokenToString (&(pFrameWork->windowname), sizeof(pFrameWork->windowname[0]), sizeof(pFrameWork->windowname), tcsTokenValue);
					break;

				case 2:
					TokenToString (&(pFrameWork->tcsName), sizeof(pFrameWork->tcsName[0]), sizeof(pFrameWork->tcsName), tcsTokenValue);
					break;

				case 3:
					TokenToString (&(pFrameWork->tcsText), sizeof(pFrameWork->tcsText[0]), sizeof(pFrameWork->tcsText), tcsTokenValue);
					pFrameWork->changetext = 1;
					break;

				case 4:
					TokenToString (&(pFrameWork->tcsIconfile), sizeof(pFrameWork->tcsIconfile[0]), sizeof(pFrameWork->tcsIconfile), tcsTokenValue);
					pFrameWork->changeiconfile = 1;
					break;

				case 5:
					TokenToUnsignedNumeric (&(pFrameWork->enablecontrol), sizeof(pFrameWork->enablecontrol), tcsTokenValue);
					pFrameWork->enablecontrol = (pFrameWork->enablecontrol) ? FRAMEWORKCONTROL_INDIC_ENABLE : FRAMEWORKCONTROL_INDIC_DISABLE;
					break;

				case 6:
					TokenToUnsignedNumeric (&(pFrameWork->displaywindow), sizeof(pFrameWork->displaywindow), tcsTokenValue);
					pFrameWork->displaywindow = (pFrameWork->displaywindow) ? FRAMEWORKCONTROL_INDIC_ENABLE : FRAMEWORKCONTROL_INDIC_DISABLE;
					break;

				case 7:
					TokenToUnsignedNumeric (&(pFrameWork->displaycontrol), sizeof(pFrameWork->displaycontrol), tcsTokenValue);
					pFrameWork->displaycontrol = (pFrameWork->displaycontrol) ? FRAMEWORKCONTROL_INDIC_ENABLE : FRAMEWORKCONTROL_INDIC_DISABLE;
					break;

				case 8:
					{
						ULONG  ulColorValue = 0;
						TokenToUnsignedNumeric (&ulColorValue, sizeof(ulColorValue), tcsTokenValue);
						pFrameWork->changebgcolor = 1;
						// rearrange the hex string into a COLORREF or rgb() value
						pFrameWork->controlbgcolor = RGB (((ulColorValue >> 16) & 0xff), ((ulColorValue >> 8) & 0xff), (ulColorValue & 0xff));
					}
					break;

				case 9:
					TokenToSignedNumeric (&(pFrameWork->sTerminalNumber), sizeof(pFrameWork->sTerminalNumber), tcsTokenValue);
					break;

				case 10:
					{
						ULONG  ulColorValue = 0;
						TokenToUnsignedNumeric (&ulColorValue, sizeof(ulColorValue), tcsTokenValue);
						pFrameWork->changetextcolor = 1;
						// rearrange the hex string into a COLORREF or rgb() value
						pFrameWork->controltextcolor = RGB (((ulColorValue >> 16) & 0xff), ((ulColorValue >> 8) & 0xff), (ulColorValue & 0xff));
					}
					break;

				case 11:
					TokenToUnsignedNumeric (&(pFrameWork->extFSC), sizeof(pFrameWork->extFSC), tcsTokenValue);
					break;

				case 12:
					{
						ULONG  ulColorValue = 0;
						TokenToUnsignedNumeric (&ulColorValue, sizeof(ulColorValue), tcsTokenValue);
						// rearrange the hex string into a COLORREF or rgb() value
						pFrameWork->windefaultbtncolorbg = RGB (((ulColorValue >> 16) & 0xff), ((ulColorValue >> 8) & 0xff), (ulColorValue & 0xff));
					}
					break;

				case 13:
					{
						ULONG  ulColorValue = 0;
						TokenToUnsignedNumeric (&ulColorValue, sizeof(ulColorValue), tcsTokenValue);
						// rearrange the hex string into a COLORREF or rgb() value
						pFrameWork->windefaultbtncolortxt = RGB (((ulColorValue >> 16) & 0xff), ((ulColorValue >> 8) & 0xff), (ulColorValue & 0xff));
					}
					break;

				case 14:
					TokenToUnsignedNumeric (&(pFrameWork->windefaultbtnwidth), sizeof(pFrameWork->windefaultbtnwidth), tcsTokenValue);
					break;

				case 15:
					TokenToUnsignedNumeric (&(pFrameWork->windefaultbtnheight), sizeof(pFrameWork->windefaultbtnheight), tcsTokenValue);
					break;

				default:
					break;
			}
		}

		*tcBuffer = pLastGood + _tcslen(pLastGood) + 1;  // update the buffer pointer so that we can handle multiple messages in the buffer.
	}

	return BL_CONNENGINE_MSG_FRAMEWORK;
}

int ObjectMakerPRINTFILE (TCHAR **tcsTokenName, TCHAR **tcBuffer, PRINTFILE *pPrintFile, int iDirection)
{
	TCHAR *ptcBuffer = *tcBuffer;
	TCHAR tcTokenNameEndTag [64];
	TCHAR *pLastGood;
	LabelListItem myList [] = {
		{_T("filename"),   1},
		{0,                   0}
	};

	if (iDirection == 1) {
		// translate from an XML string to an PRINTFILE struct
		TCHAR *tcsTokenValue, *tcsToken;
		int i = 0;
		int iCondCount = 0;

		memset (pPrintFile, 0, sizeof(PRINTFILE));  // Init the PRINTFILE to zeros to prepare for processing the XML message.

		_stprintf (tcTokenNameEndTag, _T("/%s"), *tcsTokenName);
		pLastGood = ptcBuffer;

		while (1) {
			*tcsTokenName = _tcstok (NULL, pTokenDelimitersTag);
			if (*tcsTokenName == NULL || **tcsTokenName == 0)
				break;

			pLastGood = *tcsTokenName;

			if (_tcscmp (*tcsTokenName, tcTokenNameEndTag) == 0)
				break;

			for (i = 0; myList[i].tcsLabel; i++) {
				if (_tcscmp (myList[i].tcsLabel, *tcsTokenName) == 0) {
					break;
				}
			}

			tcsTokenValue = _tcstok (NULL, pTokenDelimitersValue);
			tcsToken = _tcstok (NULL, pTokenDelimitersTag);
			if (tcsTokenValue == NULL || tcsToken == NULL)
				break;

			pLastGood = tcsToken;

			// If the tag is not known, then just skip it and go to the next one.
			if (myList[i].tcsLabel == 0)
				continue;

			switch (myList[i].usOffset) {
				case 1:
					TokenToString (&(pPrintFile->tcsFileName), sizeof(pPrintFile->tcsFileName[0]), sizeof(pPrintFile->tcsFileName), tcsTokenValue);
					break;

				default:
					break;
			}
		}

		*tcBuffer = pLastGood + _tcslen(pLastGood) + 1;  // update the buffer pointer so that we can handle multiple messages in the buffer.
	}

	return BL_CONNENGINE_MSG_PRINTFILE;
}

int ObjectMakerGUESTCHECK_RTRV (TCHAR **tcsTokenName, TCHAR **tcBuffer, GUESTCHECK_RTRV *pGuestCheck_Rtrv, int iDirection)
{
	TCHAR *ptcBuffer = *tcBuffer;
	TCHAR *pLastGood;
	TCHAR tcTokenNameEndTag [64];
	LabelListItem myList [] = {
		{_T("gcnum"),      1},
		{0,                        0}
	};

	if (iDirection == 1) {
		// translate from an XML string to an GUESTCHECK_RTRV struct
		TCHAR *tcsTokenValue, *tcsToken;
		int i = 0;
		int iCondCount = 0;

		memset (pGuestCheck_Rtrv, 0, sizeof(GUESTCHECK_RTRV));  // Init the GUESTCHECK_RTRV to zeros to prepare for processing the XML message.

		_stprintf (tcTokenNameEndTag, _T("/%s"), *tcsTokenName);
		pLastGood = ptcBuffer;

		while (1) {
			*tcsTokenName = _tcstok (NULL, pTokenDelimitersTag);
			if (*tcsTokenName == NULL || **tcsTokenName == 0)
				break;

			pLastGood = *tcsTokenName;

			if (_tcscmp (*tcsTokenName, tcTokenNameEndTag) == 0)
				break;

			for (i = 0; myList[i].tcsLabel; i++) {
				if (_tcscmp (myList[i].tcsLabel, *tcsTokenName) == 0) {
					break;
				}
			}

			tcsTokenValue = _tcstok (NULL, pTokenDelimitersValue);
			tcsToken = _tcstok (NULL, pTokenDelimitersTag);
			if (tcsTokenValue == NULL || tcsToken == NULL)
				break;

			pLastGood = tcsToken;

			// If the tag is not known, then just skip it and go to the next one.
			if (myList[i].tcsLabel == 0)
				continue;

			switch (myList[i].usOffset) {
				case 1:
					TokenToSignedNumeric (&(pGuestCheck_Rtrv->gcnum), sizeof(pGuestCheck_Rtrv->gcnum), tcsTokenValue);
					break;

				default:
					break;
			}
		}

		*tcBuffer = pLastGood + _tcslen(pLastGood) + 1;  // update the buffer pointer so that we can handle multiple messages in the buffer.
	}

	return BL_CONNENGINE_MSG_GUESTCHECK_RTRV;
}

int ObjectMakerCASHIERACTION (TCHAR **tcsTokenName, TCHAR **tcBuffer, CASHIERACTION *pCashierAction, int iDirection)
{
	TCHAR *ptcBuffer = *tcBuffer;
	TCHAR *pLastGood;
	TCHAR tcTokenNameEndTag [64];
	LabelListItem myList [] = {
		{_T("lAmount"),          1},
		{_T("tcsActionType"),    2},
		{_T("ulEmployeeId"),     3},
		{_T("usTotalID"),        4},
		{_T("usVoidIndic"),      5},
		{_T("tcsReference"),     6},
		{_T("ulEmployeeIdPaidTo"),   7},
		{0,                      0}
	};

	if (iDirection == 1) {
		// translate from an XML string to an CASHIERACTION struct
		TCHAR *tcsTokenValue, *tcsToken;
		int i = 0;
		int iCondCount = 0;

		memset (pCashierAction, 0, sizeof(CASHIERACTION));  // Init the GUESTCHECK_RTRV to zeros to prepare for processing the XML message.

		_stprintf (tcTokenNameEndTag, _T("/%s"), *tcsTokenName);
		pLastGood = ptcBuffer;

		while (1) {
			*tcsTokenName = _tcstok (NULL, pTokenDelimitersTag);
			if (*tcsTokenName == NULL || **tcsTokenName == 0)
				break;

			pLastGood = *tcsTokenName;

			if (_tcscmp (*tcsTokenName, tcTokenNameEndTag) == 0)
				break;

			for (i = 0; myList[i].tcsLabel; i++) {
				if (_tcscmp (myList[i].tcsLabel, *tcsTokenName) == 0) {
					break;
				}
			}

			tcsTokenValue = _tcstok (NULL, pTokenDelimitersValue);

			// if there is a problem with parsing then lets just bail out at this point.
			if (tcsTokenValue == 0)
				break;

			// check to see if the text string between the tags is empty
			if (tcsTokenValue[0] == _T('/')) {
				if (_tcscmp (tcsTokenValue+1, *tcsTokenName) == 0)
					continue;
			}

			// check to see if this is an XML singleton type tag such as <tag /> which
			// indicates an empty tag.  While we do not send this type out, we may get them.
			if (_tcscmp (_T("/"), tcsTokenValue) == 0)
				continue;

			tcsToken = _tcstok (NULL, pTokenDelimitersTag);
			if (tcsTokenValue == NULL || tcsToken == NULL) {
				break;
			}

			pLastGood = tcsToken;

			// If the tag is not known, then just skip it and go to the next one.
			if (myList[i].tcsLabel == 0)
				continue;

			switch (myList[i].usOffset) {
				case 1:
					TokenToSignedNumeric (&(pCashierAction->lAmount), sizeof(pCashierAction->lAmount), tcsTokenValue);
					break;

				case 2:
					TokenToString (&(pCashierAction->tcsActionType), sizeof(pCashierAction->tcsActionType[0]), sizeof(pCashierAction->tcsActionType), tcsTokenValue);
					break;

				case 3:
					TokenToUnsignedNumeric (&(pCashierAction->ulEmployeeId), sizeof(pCashierAction->ulEmployeeId), tcsTokenValue);
					break;

				case 4:
					TokenToSignedNumeric (&(pCashierAction->usTotalId), sizeof(pCashierAction->usTotalId), tcsTokenValue);
					break;

				case 5:
					TokenToSignedNumeric (&(pCashierAction->usVoidIndic), sizeof(pCashierAction->usVoidIndic), tcsTokenValue);
					break;

				case 6:
					TokenToString (&(pCashierAction->tcsReference), sizeof(pCashierAction->tcsReference[0]), sizeof(pCashierAction->tcsReference), tcsTokenValue);
					break;

				case 7:
					TokenToUnsignedNumeric (&(pCashierAction->ulEmployeeIdPaidTo), sizeof(pCashierAction->ulEmployeeIdPaidTo), tcsTokenValue);
					break;

				default:
					break;
			}
		}

		*tcBuffer = pLastGood + _tcslen(pLastGood) + 1;  // update the buffer pointer so that we can handle multiple messages in the buffer.
	}

	return BL_CONNENGINE_MSG_CASHIERACTION;
}

int ObjectMakerEJENTRYACTION (TCHAR **tcsTokenName, TCHAR **tcBuffer, EJENTRYACTION *pEjEntryAction, int iDirection)
{
	TCHAR *ptcBuffer = *tcBuffer;
	TCHAR *pLastGood;
	TCHAR tcTokenNameEndTag [64];
	LabelListItem myList [] = {
		{_T("consecnumber"),      1},
		{_T("tcsActionType"),            2},
		{_T("ulFilePositionHigh"),       3},
		{_T("ulFilePositionLow"),        4},
		{0,                              0}
	};

	if (iDirection == 1) {
		// translate from an XML string to an CASHIERACTION struct
		TCHAR *tcsTokenValue, *tcsToken;
		int i = 0;
		int iCondCount = 0;

		memset (pEjEntryAction, 0, sizeof(EJENTRYACTION));  // Init the GUESTCHECK_RTRV to zeros to prepare for processing the XML message.

		_stprintf (tcTokenNameEndTag, _T("/%s"), *tcsTokenName);
		pLastGood = ptcBuffer;

		while (1) {
			*tcsTokenName = _tcstok (NULL, pTokenDelimitersTag);
			if (*tcsTokenName == NULL || **tcsTokenName == 0)
				break;

			pLastGood = *tcsTokenName;

			if (_tcscmp (*tcsTokenName, tcTokenNameEndTag) == 0)
				break;

			for (i = 0; myList[i].tcsLabel; i++) {
				if (_tcscmp (myList[i].tcsLabel, *tcsTokenName) == 0) {
					break;
				}
			}

			tcsTokenValue = _tcstok (NULL, pTokenDelimitersValue);

			// if there is a problem with parsing then lets just bail out at this point.
			if (tcsTokenValue == 0)
				break;

			// check to see if the text string between the tags is empty
			if (tcsTokenValue[0] == _T('/')) {
				if (_tcscmp (tcsTokenValue+1, *tcsTokenName) == 0)
					continue;
			}

			// check to see if this is an XML singleton type tag such as <tag /> which
			// indicates an empty tag.  While we do not send this type out, we may get them.
			if (_tcscmp (_T("/"), tcsTokenValue) == 0)
				continue;

			tcsToken = _tcstok (NULL, pTokenDelimitersTag);
			if (tcsTokenValue == NULL || tcsToken == NULL) {
				break;
			}

			pLastGood = tcsToken;

			// If the tag is not known, then just skip it and go to the next one.
			if (myList[i].tcsLabel == 0)
				continue;

			switch (myList[i].usOffset) {
				case 1:
					TokenToSignedNumeric (&(pEjEntryAction->consecnumber), sizeof(pEjEntryAction->consecnumber), tcsTokenValue);
					break;

				case 2:
					TokenToString (&(pEjEntryAction->tcsActionType), sizeof(pEjEntryAction->tcsActionType[0]), sizeof(pEjEntryAction->tcsActionType), tcsTokenValue);
					break;

				case 3:
					TokenToSignedNumeric (&(pEjEntryAction->ulFilePositionHigh), sizeof(pEjEntryAction->ulFilePositionHigh), tcsTokenValue);
					break;

				case 4:
					TokenToSignedNumeric (&(pEjEntryAction->ulFilePositionLow), sizeof(pEjEntryAction->ulFilePositionLow), tcsTokenValue);
					break;

				default:
					break;
			}
		}

		*tcBuffer = pLastGood + _tcslen(pLastGood) + 1;  // update the buffer pointer so that we can handle multiple messages in the buffer.
	}

	return BL_CONNENGINE_MSG_EJENTRYACTION;
}

int ObjectMakerMACROACTION (TCHAR **tcsTokenName, TCHAR **tcBuffer, MACROACTION *pMacroAction, int iDirection)
{
	TCHAR *ptcBuffer = *tcBuffer;
	TCHAR *pLastGood;
	TCHAR tcTokenNameEndTag [64];
	LabelListItem myList [] = {
		{_T("stringnumber"),      1},
		{0,                       0}
	};

	if (iDirection == 1) {
		// translate from an XML string to an CASHIERACTION struct
		TCHAR *tcsTokenValue, *tcsToken;
		int i = 0;
		int iCondCount = 0;

		memset (pMacroAction, 0, sizeof(MACROACTION));  // Init the GUESTCHECK_RTRV to zeros to prepare for processing the XML message.

		_stprintf (tcTokenNameEndTag, _T("/%s"), *tcsTokenName);
		pLastGood = ptcBuffer;

		while (1) {
			*tcsTokenName = _tcstok (NULL, pTokenDelimitersTag);
			if (*tcsTokenName == NULL || **tcsTokenName == 0)
				break;

			pLastGood = *tcsTokenName;

			if (_tcscmp (*tcsTokenName, tcTokenNameEndTag) == 0)
				break;

			for (i = 0; myList[i].tcsLabel; i++) {
				if (_tcscmp (myList[i].tcsLabel, *tcsTokenName) == 0) {
					break;
				}
			}

			tcsTokenValue = _tcstok (NULL, pTokenDelimitersValue);

			// if there is a problem with parsing then lets just bail out at this point.
			if (tcsTokenValue == 0)
				break;

			// check to see if the text string between the tags is empty
			if (tcsTokenValue[0] == _T('/')) {
				if (_tcscmp (tcsTokenValue+1, *tcsTokenName) == 0)
					continue;
			}

			// check to see if this is an XML singleton type tag such as <tag /> which
			// indicates an empty tag.  While we do not send this type out, we may get them.
			if (_tcscmp (_T("/"), tcsTokenValue) == 0)
				continue;

			tcsToken = _tcstok (NULL, pTokenDelimitersTag);
			if (tcsTokenValue == NULL || tcsToken == NULL) {
				break;
			}

			pLastGood = tcsToken;

			// If the tag is not known, then just skip it and go to the next one.
			if (myList[i].tcsLabel == 0)
				continue;

			switch (myList[i].usOffset) {
				case 1:
					TokenToSignedNumeric (&(pMacroAction->usStringNumber), sizeof(pMacroAction->usStringNumber), tcsTokenValue);
					break;

				default:
					break;
			}
		}

		*tcBuffer = pLastGood + _tcslen(pLastGood) + 1;  // update the buffer pointer so that we can handle multiple messages in the buffer.
	}

	return BL_CONNENGINE_MSG_MACROACTION;
}

int ObjectMakerDATAQUERY (TCHAR **tcsTokenName, TCHAR **tcBuffer, DATAQUERY *pDataQuery, int iDirection)
{
	TCHAR *ptcBuffer = *tcBuffer;
	TCHAR *pLastGood;
	TCHAR tcTokenNameEndTag [64];
	LabelListItem myList [] = {
		{_T("consecnumber"),      1},
		{_T("tcsActionType"),     2},
		{_T("actiontype"),        2},
		{_T("tcsQueryStatement"), 3},
		{0,                       0}
	};

	if (iDirection == 1) {
		// translate from an XML string to a DATAQUERY struct
		TCHAR *tcsTokenValue, *tcsToken;
		int i = 0;
		int iCondCount = 0;

		memset (pDataQuery, 0, sizeof(DATAQUERY));  // Init the DATAQUERY to zeros to prepare for processing the XML message.

		_stprintf (tcTokenNameEndTag, _T("/%s"), *tcsTokenName);
		pLastGood = ptcBuffer;

		while (1) {
			*tcsTokenName = _tcstok (NULL, pTokenDelimitersTag);
			if (*tcsTokenName == NULL || **tcsTokenName == 0)
				break;

			pLastGood = *tcsTokenName;

			if (_tcscmp (*tcsTokenName, tcTokenNameEndTag) == 0)
				break;

			for (i = 0; myList[i].tcsLabel; i++) {
				if (_tcscmp (myList[i].tcsLabel, *tcsTokenName) == 0) {
					break;
				}
			}

			tcsTokenValue = _tcstok (NULL, pTokenDelimitersValue);

			// if there is a problem with parsing then lets just bail out at this point.
			if (tcsTokenValue == 0)
				break;

			// check to see if the text string between the tags is empty
			if (tcsTokenValue[0] == _T('/')) {
				if (_tcscmp (tcsTokenValue+1, *tcsTokenName) == 0)
					continue;
			}

			// check to see if this is an XML singleton type tag such as <tag /> which
			// indicates an empty tag.  While we do not send this type out, we may get them.
			if (_tcscmp (_T("/"), tcsTokenValue) == 0)
				continue;

			tcsToken = _tcstok (NULL, pTokenDelimitersTag);
			if (tcsTokenValue == NULL || tcsToken == NULL) {
				break;
			}

			pLastGood = tcsToken;

			// If the tag is not known, then just skip it and go to the next one.
			if (myList[i].tcsLabel == 0)
				continue;

			switch (myList[i].usOffset) {
				case 1:
					TokenToSignedNumeric (&(pDataQuery->consecnumber), sizeof(pDataQuery->consecnumber), tcsTokenValue);
					break;

				case 2:
					TokenToString (&(pDataQuery->tcsActionType), sizeof(pDataQuery->tcsActionType[0]), sizeof(pDataQuery->tcsActionType), tcsTokenValue);
					break;

				case 3:
					TokenToString (&(pDataQuery->tcsQueryStatement), sizeof(pDataQuery->tcsQueryStatement[0]), sizeof(pDataQuery->tcsQueryStatement), tcsTokenValue);
					break;

				default:
					break;
			}
		}

		*tcBuffer = pLastGood + _tcslen(pLastGood) + 1;  // update the buffer pointer so that we can handle multiple messages in the buffer.
	}

	return BL_CONNENGINE_MSG_DATAQUERY;
}

int ObjectMakerSTATECHANGE (TCHAR **tcsTokenName, TCHAR **tcBuffer, STATECHANGE *pStateChange, int iDirection)
{
	TCHAR *ptcBuffer = *tcBuffer;
	TCHAR tcTokenNameEndTag [64];
	TCHAR *pLastGood;
	LabelListItem myList [] = {
		{_T("tcsActionType"),     1},
		{_T("service"),           2},
		{_T("uleventid"),         3},
		{0,                       0}
	};

	if (iDirection == 1) {
		// translate from an XML string to an STATECHANGE struct
		TCHAR *tcsTokenValue, *tcsToken;
		int i = 0;
		int iCondCount = 0;

		memset (pStateChange, 0, sizeof(STATECHANGE));  // Init the STATECHANGE to zeros to prepare for processing the XML message.

		_stprintf (tcTokenNameEndTag, _T("/%s"), *tcsTokenName);
		pLastGood = ptcBuffer;

		while (1) {
			*tcsTokenName = _tcstok (NULL, pTokenDelimitersTag);
			if (*tcsTokenName == NULL || **tcsTokenName == 0)
				break;

			pLastGood = *tcsTokenName;

			if (_tcscmp (*tcsTokenName, tcTokenNameEndTag) == 0)
				break;

			for (i = 0; myList[i].tcsLabel; i++) {
				if (_tcscmp (myList[i].tcsLabel, *tcsTokenName) == 0) {
					break;
				}
			}

			tcsTokenValue = _tcstok (NULL, pTokenDelimitersValue);
			tcsToken = _tcstok (NULL, pTokenDelimitersTag);
			if (tcsTokenValue == NULL || tcsToken == NULL)
				break;

			pLastGood = tcsToken;

			// If the tag is not known, then just skip it and go to the next one.
			if (myList[i].tcsLabel == 0)
				continue;

			switch (myList[i].usOffset) {
				case 1:
					TokenToString (&(pStateChange->tcsActionType), sizeof(pStateChange->tcsActionType[0]), sizeof(pStateChange->tcsActionType), tcsTokenValue);
					break;

				case 2:
					TokenToString (&(pStateChange->tcsService), sizeof(pStateChange->tcsService[0]), sizeof(pStateChange->tcsService), tcsTokenValue);
					break;

				case 3:
					TokenToSignedNumeric (&(pStateChange->uleventid), sizeof(pStateChange->uleventid), tcsTokenValue);
					break;

				default:
					break;
			}
		}

		*tcBuffer = pLastGood + _tcslen(pLastGood) + 1;  // update the buffer pointer so that we can handle multiple messages in the buffer.
	}

	return BL_CONNENGINE_MSG_STATECHANGE;
}



int ObjectMakerCONNENGINE (TCHAR **tcsTokenName, TCHAR **tcBuffer, AllObjects *pAllObjects, int iDirection)
{
	TCHAR *ptcBuffer = *tcBuffer;
	TCHAR *pLastGood;
	TCHAR tcTokenNameEndTag [64];
	LabelListItem myList [] = {
		{_T("terminalnumber"),     1},
		{_T("TRANSACTION"),        2},
		{_T("FRAMEWORK"),          2},
		{_T("PRINTFILE"),          2},
		{_T("EMPLOYEECHANGE"),     2},
		{_T("GUESTCHECK_RTRV"),    2},
		{_T("CASHIERACTION"),      2},
		{_T("EJENTRYACTION"),      2},
		{_T("STATECHANGE"),        2},
		{0,                        0}
	};

	if (iDirection == 1) {
		// translate from an XML string to an CASHIERACTION struct
		TCHAR *tcsTokenValue, *tcsToken;
		int i = 0;
		int iCondCount = 0;

		memset (pAllObjects, 0, sizeof(AllObjects));

		_stprintf (tcTokenNameEndTag, _T("/%s"), *tcsTokenName);
		pLastGood = ptcBuffer;

		while (1) {
			*tcsTokenName = _tcstok (NULL, pTokenDelimitersTag);
			if (*tcsTokenName == NULL || **tcsTokenName == 0)
				break;

			pLastGood = *tcsTokenName;

			if (_tcscmp (*tcsTokenName, tcTokenNameEndTag) == 0)
				break;

			for (i = 0; myList[i].tcsLabel; i++) {
				if (_tcscmp (myList[i].tcsLabel, *tcsTokenName) == 0) {
					break;
				}
			}

			switch (myList[i].usOffset) {
				case 2:
					pAllObjects->ConnEngine.iObjectType = ObjectMakerStartParsing (tcsTokenName, tcBuffer, pAllObjects, iDirection);
					continue;

				default:
					break;
			}

			tcsTokenValue = _tcstok (NULL, pTokenDelimitersValue);

			// if there is a problem with parsing then lets just bail out at this point.
			if (tcsTokenValue == 0)
				break;

			// check to see if the text string between the tags is empty
			if (tcsTokenValue[0] == _T('/')) {
				if (_tcscmp (tcsTokenValue+1, *tcsTokenName) == 0)
					continue;
			}

			// check to see if this is an XML singleton type tag such as <tag /> which
			// indicates an empty tag.  While we do not send this type out, we may get them.
			if (_tcscmp (_T("/"), tcsTokenValue) == 0)
				continue;

			tcsToken = _tcstok (NULL, pTokenDelimitersTag);
			if (tcsTokenValue == NULL || tcsToken == NULL) {
				break;
			}

			pLastGood = tcsToken;

			// If the tag is not known, then just skip it and go to the next one.
			if (myList[i].tcsLabel == 0) {
				continue;
			}

			switch (myList[i].usOffset) {
				case 1:
					TokenToSignedNumeric (&(pAllObjects->ConnEngine.sTerminalNumber), sizeof(pAllObjects->ConnEngine.sTerminalNumber), tcsTokenValue);
					break;

				default:
					break;
			}
		}

		*tcBuffer = pLastGood + _tcslen(pLastGood) + 1;  // update the buffer pointer so that we can handle multiple messages in the buffer.

		pAllObjects->ConnEngine.tcsTempFileName[0] = 0;
		if (pAllObjects->ConnEngine.iObjectType == BL_CONNENGINE_MSG_TRANSACTION) {
			pAllObjects->ConnEngine.ulFlags |= BL_CONNENGINE_FLAGS_UNIQUE_ID;
			_tcscpy (pAllObjects->ConnEngine.tcsTempFileName, GenerateTempFileName ());
			PifDeleteFileEx (pAllObjects->ConnEngine.tcsTempFileName, TRUE);
			PifMoveFile (_T("OBJTEMP"), TRUE, pAllObjects->ConnEngine.tcsTempFileName, TRUE);
		}
	}
	return BL_CONNENGINE_MSG_CONNENGINE;
}

int ObjectMakerStartParsing (TCHAR **tcsTokenName, TCHAR **tcBuffer, AllObjects *pAllObjects, int iDirection)
{
	int iRetStatus = BL_CONNENGINE_MSG_UNKNOWN_MSG;

	if (tcsTokenName && *tcsTokenName) {
		if (_tcscmp (*tcsTokenName, _T("TRANSACTION")) == 0)
			iRetStatus = ObjectMakerTRANSACTION (tcsTokenName, tcBuffer, &(pAllObjects->u.Transaction), iDirection);

		else if (_tcscmp (*tcsTokenName, _T("FRAMEWORK")) == 0)
			iRetStatus = ObjectMakerFRAMEWORK (tcsTokenName, tcBuffer, &(pAllObjects->u.FrameWork), iDirection);

		else if (_tcscmp (*tcsTokenName, _T("PRINTFILE")) == 0)
			iRetStatus = ObjectMakerPRINTFILE (tcsTokenName, tcBuffer, &(pAllObjects->u.PrintFile), iDirection);

		else if (_tcscmp (*tcsTokenName, _T("EMPLOYEECHANGE")) == 0)
			iRetStatus = ObjectMakerEMPLOYEECHANGE (tcsTokenName, tcBuffer, &(pAllObjects->u.EmployeeChange), iDirection);

		else if (_tcscmp (*tcsTokenName, _T("GUESTCHECK_RTRV")) == 0)
			iRetStatus = ObjectMakerGUESTCHECK_RTRV (tcsTokenName, tcBuffer, &(pAllObjects->u.GuestCheck_Rtrv), iDirection);

		else if (_tcscmp (*tcsTokenName, _T("CASHIERACTION")) == 0)
			iRetStatus = ObjectMakerCASHIERACTION (tcsTokenName, tcBuffer, &(pAllObjects->u.CashierAction), iDirection);

		else if (_tcscmp (*tcsTokenName, _T("EJENTRYACTION")) == 0)
			iRetStatus = ObjectMakerEJENTRYACTION (tcsTokenName, tcBuffer, &(pAllObjects->u.EjEntryAction), iDirection);

		else if (_tcscmp (*tcsTokenName, _T("CONNENGINE")) == 0)
			iRetStatus = ObjectMakerCONNENGINE (tcsTokenName, tcBuffer, pAllObjects, iDirection);

		else if (_tcscmp (*tcsTokenName, _T("MACROACTION")) == 0)
			iRetStatus = ObjectMakerMACROACTION (tcsTokenName, tcBuffer, &(pAllObjects->u.MacroAction), iDirection);

		else if (_tcscmp (*tcsTokenName, _T("DATAQUERY")) == 0)
			iRetStatus = ObjectMakerDATAQUERY (tcsTokenName, tcBuffer, &(pAllObjects->u.DataQuery), iDirection);

		else if (_tcscmp (*tcsTokenName, _T("STATECHANGE")) == 0)
			iRetStatus = ObjectMakerSTATECHANGE (tcsTokenName, tcBuffer, &(pAllObjects->u.StateChange), iDirection);

	}
	return iRetStatus;
}


__declspec(dllexport)
int BlProcessConnEngineMessageAndTag(TCHAR **ptcBuffer, VOID *pObject)
{
	TCHAR      *tcsTokenName;
	int         iDirection = 0;
	int         iRetStatus = -1;
	AllObjects  *myObject = (AllObjects *)pObject;

	iDirection = 1;

	if (ptcBuffer != NULL && *ptcBuffer != NULL && pObject != NULL) {
		AllObjects  *pMyObject = (AllObjects *)pObject;
		pMyObject->iMessageTag = -1;
		tcsTokenName = _tcstok (*ptcBuffer, pTokenDelimitersTag);
		if (tcsTokenName != NULL) {
			myObject->ConnEngine.tcsTempFileName[0] = 0;
			pMyObject->iMessageTag = iRetStatus = ObjectMakerStartParsing (&tcsTokenName, ptcBuffer, pObject, iDirection);
			if (iRetStatus == BL_CONNENGINE_MSG_CONNENGINE) {
				UCHAR uchTermAddr = CliReadUAddr();
				if (myObject->ConnEngine.sTerminalNumber == uchTermAddr || myObject->ConnEngine.sTerminalNumber == (-1)) {
					if (myObject->ConnEngine.tcsTempFileName[0]) {
						PifDeleteFileEx (_T("OBJTEMP"), TRUE);
						PifMoveFile (myObject->ConnEngine.tcsTempFileName, TRUE, _T("OBJTEMP"), TRUE);
					}
					return myObject->ConnEngine.iObjectType;
				}
			}
		}
	}

	return iRetStatus;
}


#if 0
int  testit (void)
{
	TCHAR      *tcsTokenName;
	TCHAR       tcBuffer[512];
	int         iRetStatus = 0;
	int         iDirection = 0;
	AllObjects  allObjects;

//	ItemContCode.auchStatus = 124;
	allObjects.ItemContCode.uchItemType = 23;
	allObjects.ItemContCode.uchReportCode = 44;
	allObjects.ItemContCode.usBonusIndex = 15;

	iRetStatus = ObjectMakerITEMCONTCODE (&tcsTokenName, tcBuffer, &allObjects.ItemContCode, iDirection);

	_tcscpy (tcBuffer, _T("<ITEMCONTCODE><uchItemType>1</uchItemType><uchReportCode>2</uchReportCode><usBonusIndex>3</usBonusIndex></ITEMCONTCODE>"));

	iDirection = 1;
	tcsTokenName = _tcstok (tcBuffer, pTokenDelimitersTag);

	iRetStatus = ObjectMakerStartParsing (&tcsTokenName, &tcBuffer, &allObjects, iDirection);

	return iRetStatus;
}
#endif