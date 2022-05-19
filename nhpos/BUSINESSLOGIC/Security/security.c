/*************************************************************************
 *
 * security.h
 *
 * $Workfile:: security.h                                                $
 *
 * $Revision:: Initial                                                   $
 *
 * $Archive::                                                            $
 *
 * $Date:: 2/20/2003                                                     $
 *
 *************************************************************************
 *
 * Copyright (c) Georgia Sothern University 2003.  All rights reserved.
 *
 *************************************************************************
 *
 * $History:: security.h                                                $
 * Date :		Name :			Version :		Comments				$
 * Feb-20-03	Chris Wunn		Initial
 *
 ************************************************************************/

#include <windows.h>
#include <tchar.h>
#include "ecr.h"
#include "security.h"

/************************************************************************
*
*	File communication Variable Declarations
*
************************************************************************/
static int			nMyOffset2[] = {0,3,6,7,3};
static TCHAR		chMyOffset1[] = { _T('0'), _T('f'), _T('L'), _T('C'), _T('N') };
static TCHAR		chMyIndic[] = { _T('2'), _T('6'), _T('3'), _T('8'), _T('1') };;
static TCHAR		aszSysSerialNo[100];		// stores serial number from EEPROM
static TCHAR		aszSerialNo[100];		// stores user entered serial number
static TCHAR		aszModelNo[100];			// stores modem number from EEPROM
static TCHAR		aszUnlockNo[100];		// stores user entered unlock number
static TCHAR		aszKeyNo[100];			// stores Key Number for display to user
static TCHAR		aszUnlockSerial[100];	// Used in debugging by "unlocking code" cnvrtKey2Serial & cnvrtKey2Unlock	UCHAR
static TCHAR		aszDerivedSerial[100];	// stores S/N derived from unlock code (for comparison)
static int			bUnlocked;			//stores status of software security locked or unlocked


/*===========================================================================
** Synopsis:    bool secCompareSerials()
*
*     Input:    Nothing
*    Output:    BOOL - true -> serials match, false -> serials are different
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: Function compares the serial number written to the registry during
*				bootup and the NHPOS user entered serial number.  If the serials are
*				exact matches (except for hyphens), the function returns true, false otherwise
*===========================================================================
*/
int secCompareSerials(){
	int i, j;
	int match = 1;

	if(!aszSysSerialNo[0])
	{
		match = 0;
	}
	else
	{
		for(i=0, j=0;aszSerialNo[i] && aszSysSerialNo[j];i++, j+=2)
		{
			if(aszSerialNo[i] == aszSysSerialNo[j]) continue;
			else
			{
				if(aszSysSerialNo[j] == _T('-')){
					j++;
					continue;
				}
				match = 0;
				break;
			}
		}
	}
	return match;
}




/*===========================================================================
** Synopsis:    void secReadRegistry()
*
*     Input:    Nothing
*    Output:   	TCHAR	aszSysSerialNo[];	// stores serial number from EEPROM
*				TCHAR	aszSerialNo[];		// stores user entered serial number
*				TCHAR	aszModelNo[];		// stores model number from EEPROM
*				TCHAR	aszUnlockNo[];		// stores user entered unlock number
*				TCHAR	aszKeyNo[];			// stores Key Number for display to user
*
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: Function enumeratest the values of the given registry key, placing the
*				resulting strings into the member variables of this PaneControl object.
*				Member variables are then used by other software security functions
*===========================================================================
*/
void secReadRegistry()
{
    HKEY hKey;
	DWORD openOrCreate;
	TCHAR xValue[200];
	DWORD nValue = 200;
	TCHAR xData[200];
	DWORD nData = 200;
	DWORD dwIndex = 0;
	DWORD nType = REG_SZ;
	int nResult1, nResult2;

	//  Open the registry and obtain the serial #'s & unlock #
#ifdef XP_PORT
	nResult1 = RegCreateKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\NCR\\CCL Client Agent"),
								0,
								_T(""),
                                REG_OPTION_NON_VOLATILE,             // special options flag
                                KEY_ALL_ACCESS,             // desired security access
								NULL,
								&hKey, &openOrCreate);
#else
	nResult1 = RegCreateKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\NCR\\CCL Client Agent"),
								0, 0, 0, 0, NULL, &hKey, &openOrCreate);
#endif

	if (nResult1 == ERROR_SUCCESS) {
		nResult2 = ERROR_SUCCESS;
		while (nResult2 == ERROR_SUCCESS) {

			memset(&xData, 0x00, sizeof(xData));

			nValue = sizeof (xValue);
			nData = sizeof (xData);
			nResult2 = RegEnumValue(hKey, dwIndex, xValue, &nValue, NULL, &nType, (BYTE*)xData, &nData);
			if(nResult2 == ERROR_SUCCESS) {
				if (_tcsstr (xValue, _T("NHPOSSerialNumber"))){
					//Parse out high-byte NULL characters in xData upon copying to xSerialNo.
					int endRegData=2; //used to exit upon 2 consecutive null characters
					DWORD i=0;
					int j=0;
					for(i=0; i<nData; i++){
						if(xData[i]){
							aszSerialNo[j++] = xData[i];
							endRegData=2;
						}
						else {
							if(xData[i] == '\0'){
								endRegData--;
								if(endRegData==0) break;
							}
						}
					}
				}
				if (_tcsstr (xValue, _T("SystemSerialNumber")))
					_tcsncpy(aszSysSerialNo, xData, nData);
					//memcpy(aszSysSerialNo, xData, nData*(sizeof(UCHAR)));
				if (_tcsstr (xValue, _T("SystemClassModel")))
					_tcsncpy(aszModelNo, xData, nData);
					//memcpy(aszModelNo, xData, nData*(sizeof(UCHAR)));
				if (_tcsstr (xValue, _T("NHPOSUnlockCode")))
					_tcsncpy(aszUnlockNo, xData, nData);
					//memcpy(aszUnlockNo, xData, nData*(sizeof(UCHAR)));
				dwIndex++;
			}
		}
		RegCloseKey (hKey);
	}
}

/*===========================================================================
** Synopsis:    SHORT secKeyCodeRegWrite(VOID)
*
*     Input:    Nothing
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      SUCCESS
*
** Description: Function writes the keycode generated by PaneControl to the registry
*				 for businesss logic to use.
*===========================================================================
*/
#define NHPOSKEYCODEVALUE _T("NHPOSKeyCode")
#define X_7448_REG_KEY _T("Software\\NCR\\CCL Client Agent")
SHORT secKeyCodeRegWrite( VOID )
{
    HKEY hKey;
	TCHAR xValue[200];
	DWORD nData = 200;
	DWORD dwIndex = 0;
	DWORD nType = REG_SZ;
	int nResult1, nResult2;

#ifdef XP_PORT
	nResult1 = RegOpenKeyEx(HKEY_LOCAL_MACHINE, X_7448_REG_KEY, 0, KEY_ALL_ACCESS, &hKey);
#else
	nResult1 = RegOpenKeyEx(HKEY_LOCAL_MACHINE, X_7448_REG_KEY, 0, 0, &hKey);
#endif

	if (nResult1 == ERROR_SUCCESS) {
		//value will be written to keycode registry value.
		_tcscpy (xValue, NHPOSKEYCODEVALUE);
		nData = 26;
		//*Cast securityData->aszUnlockNo since WinCE stores null-terminated strings in wide chars
		nResult2 = RegSetValueEx(hKey, xValue, 0, nType, (UCHAR *)aszKeyNo, nData);
		RegCloseKey (hKey);
		return 1;
	}
	return 0;
}

/*==========================================================================
* Synopsis:    void secGenKeyCode()
*
*     Input:    Nothing
*    Output:   	TCHAR aszKeyNo[] - the key number generated for the user to submit for unlcok No.
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: Function uses the user entered serial number to derive a "key code"
*				for display to the user.  The key code is based on the lease sign97ificant
*				six digits of the serial number, since this will provide more unique keys.
*
*===========================================================================*/

void secGenKeyCode()
{
	int iPos = 0, jPos = 0, serialLen;
	unsigned long xChksum = 0;

	memset(&aszKeyNo, 0x00, sizeof(aszKeyNo));
	//aszKeyNo[jPos++] = _T('Z');
	//Encode serial number and version information (Letters change at new version)
	if(secCompareSerials()){
		aszKeyNo[jPos++] = _T('Y');
		//aszKeyNo[jPos++] = _T('Q');	//version 1.00
	}
	else {
		aszKeyNo[jPos++] = _T('B');
		//aszKeyNo[jPos++] = _T('N');	//version 1.00
	}

	serialLen = _tcslen(aszSerialNo);
	for (iPos = 0; iPos < serialLen; iPos++,jPos++) {
		if ((aszSerialNo[iPos] >= _T('0')) && (aszSerialNo[iPos] <= _T('9'))) { //if serial# char is a digit
			aszKeyNo[jPos] = (_T('9') - aszSerialNo[iPos]) + chMyOffset1[(iPos) % 5];//apply encoding to key# digit
			xChksum += aszSerialNo[iPos];										//checksum creation
		}
		else {//if serial# character is not a digit, encode key# to an X
			if(aszSerialNo[iPos] == _T('-')) continue; //move to next if hyphen present (happens w/SysSerial only!)
			aszKeyNo[jPos] = _T('X');
		}
		//Encode every a hyphen after 4th digit of key#
		if(jPos == 3)
			aszKeyNo[++jPos] = _T('-');
		else
			if (jPos == 8)
				aszKeyNo[++jPos] = _T('-');
	}
	aszKeyNo[jPos] = _T('\0');

	//write keycode to the registry, for program 95 to print out.
	secKeyCodeRegWrite();

}


/*==========================================================================
* Synopsis:    void secCnvrtKey2Serial()
*
*     Input:    Nothing
*    Output:   	TCHAR m_MACAddr[] -  serial No derived from key number
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: Function converts a key code into a serial number.
*				This function should only be used in debugging to allow the
*				system to unlock itself using cnvrtKey2Unlock() after calling this function.
*
*===========================================================================*/
void secCnvrtKey2Serial()
{
	int iPos, jPos;
	unsigned long xChksum = 0;
	// convert a key code back into a serial number
	for (iPos = 0, jPos = 1; aszKeyNo[jPos]; jPos++) {
		if (aszKeyNo[jPos] == _T('-')) continue;
		if (aszKeyNo[jPos] != _T('X')) {
			aszUnlockSerial[iPos] = (_T('9') - (aszKeyNo[jPos] - chMyOffset1[iPos % 5]));
			xChksum += aszUnlockSerial[iPos];
		}
		else {
			aszUnlockSerial[iPos] = _T('-');
		}
		iPos++;
	}
	aszUnlockSerial[iPos] = (TCHAR) aszKeyNo[jPos]; //copy null character
}


/*==========================================================================
* Synopsis:    void secCnvrtKey2Unlock()
*
*     Input:    Nothing
*    Output:   	TCHAR aszUnlockNo[] -  unlock No derived from serial number in aszUnlockSerial
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: Function converts a key code into an unlock number.
*				This function should only be used in debugging to allow the
*				system to unlock itself using cnvrtKey2Serial() before calling this function
*
*===========================================================================*/
void secCnvrtKey2Unlock()
{	// convert a key code into an unlock code
	// unlock codes contain only digits  allowing the code to be entered on
	// the 7448 terminal using the standard 7448 keyboard
	// We actually do this conversion using the serial number that is generated
	// from the key code the user has provided.
	// indicate the version number of the key code (will change in each dot release)
	int jPos = 0;
	int iPos;
	int xVal;
	unsigned long xChksum = 0;

	aszUnlockNo[jPos++] = _T('1');
	aszUnlockNo[jPos++] = _T('4');


	for (iPos = 4; aszUnlockSerial[iPos]; iPos++) {
		if ((aszUnlockSerial[iPos] >= _T('0')) && (aszUnlockSerial[iPos] <= _T('9'))) {
			xVal = (aszUnlockSerial[iPos] - _T('0')) + nMyOffset2[jPos % 5];
			aszUnlockNo[jPos] = chMyIndic[jPos % 5];
			if (xVal > 9) {
				aszUnlockNo[jPos]++;
				aszUnlockNo[++jPos] = xVal - 9 + _T('0');
			}
			else {
				aszUnlockNo[++jPos] = xVal + _T('0');
			}
			xChksum += aszUnlockSerial[iPos];
			jPos++;
		}
	}
	aszUnlockNo[jPos] = aszUnlockSerial[iPos];
}

/*==========================================================================
* Synopsis:    void secCnvrtUnlock2Serial()
*
*     Input:    Nothing
*    Output:   	TCHAR aszDerivedSerial[] -  serial No derived from unlock No
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: Function converts a key code into a serial number.
*
*===========================================================================*/
void secCnvrtUnlock2Serial()
{
	int iPos, jPos, xVal;
	for (iPos = 0, jPos = 2; aszUnlockNo[jPos]; iPos++, jPos++) {
		xVal = nMyOffset2[jPos % 5];
		if (aszUnlockNo[jPos] == chMyIndic[jPos % 5]) {
			//unlockNo digit is filler value->move to next digit
			jPos++;
			aszDerivedSerial[iPos] =  _T('0') + ((aszUnlockNo[jPos] - _T('0')) - xVal);
		}
		else {//unlocNo digit is valid->convert it
			jPos++;
			aszDerivedSerial[iPos] =  _T('0') + ((aszUnlockNo[jPos] - _T('0')) - xVal);
			if(xVal)
				aszDerivedSerial[iPos] += 9;
		}
	}
	aszDerivedSerial[iPos] = aszUnlockNo[jPos];
}

/*==========================================================================
* Synopsis:    void secUnlock()
*
*     Input:    Nothing
*    Output:   	TCHAR bUnlocked[] -  serial No derived from unlock No
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: Function compares the derived serial number to the registry's serial No.
*				bUnlocked contains the status, locked or unlocked.
*
*===========================================================================*/
int secUnlock()
{
	int iPos, jPos;

	bUnlocked = FALSE;
	iPos = _tcslen(aszDerivedSerial) - 1;
	jPos = _tcslen(aszSerialNo) - 1;

	if(aszDerivedSerial[iPos] != _T('\0'))
			bUnlocked = TRUE; //at least one character exists, so begin decoding
	for (; aszDerivedSerial[iPos] && aszSerialNo[jPos]  && (iPos >= 0); jPos--) {
		if ((aszSerialNo[jPos] >= _T('0')) && (aszSerialNo[jPos] <= _T('9'))) {
			bUnlocked = (bUnlocked && (aszDerivedSerial[iPos] == aszSerialNo[jPos]));
			iPos--;
		}
	}
	//CRW offset iPos & jPos by one upon shortening the portion of serial number used in unlock code
//	bUnlocked = (bUnlocked && (aszDerivedSerial[iPos-1] == aszSerialNo[jPos-1]));

	return bUnlocked;
}



#undef NHPOSKEYCODEVALUE
#undef X_7448_REG_KEY