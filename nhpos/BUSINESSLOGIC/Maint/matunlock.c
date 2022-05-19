/*
*===========================================================================
* Title       : MaintUnlockNo Module  ( SUPER & PROGRAM MODE )
* Category    : Maintenance, NCR 2170 US Hospitality Application Program
* Program Name: MATSECU.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Midium Model
* Options     : /c /AM /W4 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstruct: This function read & write & report SECURITY NO.
*
*           The provided function names are as follows:
*
*              MaintUnlockNoRead()   : read & display Unlock NO
*              MaintUnlockNoWrite()  : set & print Unlock NO
*              MaintUnlockNoReport() : print all Unlock NO REPORT
*
* This file contains the source code to implement the maint functions to allow
* the user to read/write the Serial Number and the Unlock code for a terminal.
* This code is being implemented to support the Software Security functionality
* beginning with Release 1.4 of NHPOS.
*
* For Release 1.4 we are using the Windows Registry to keep the information needed
* for software security.  This implementation may change later.
*
* The information that is read/written to the registry comes from the new P95
* functionality which allows the user to set a serial number and unlock code
* from an NHPOS terminal.
* --------------------------------------------------------------------------
* Update Histories
*    Date  : Ver.Rev. :   Name    : Description
* Dec-19-02:          :R.Chambers : SR 15 for Software Security; initial
* Jan-16-03: 1.4      :C.Wunn     : Completion of GSU SR 15
* Mar-24-03  1.4.1.3  :C.Wunn     : Limit data entry for software security info
*June-24-04: 2.0.0	  :R.Herrington Change to use Softlocx for security
*===========================================================================
*/

/**
==============================================================================
;                      I N C L U D E     F I L E s
=============================================================================
**/
#include <windows.h>
#include <winreg.h>
#include <tchar.h>

#include <ecr.h>
#include <uie.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <display.h>
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <rfl.h>
#include <csstbpar.h>
#include "maintram.h"
#include <security.h>
#include "BlFWif.h"

//Following macros are used by maintUnlockNoRegRead & maintUnlockNoRegWrite, undefined afterward
#define NHPOSUNLOCKCODEVALUE _T("NHPOSUnlockCode")
#define NHPOSSERIALNOVALUE _T("NHPOSSerialNumber")
//#define NHPOSKEYCODEVALUE _T("NHPOSKeyCode")
#define SYSTEMSERIALNO _T("SystemSerialNumber")
#define X_7448_REG_KEY _T("Software\\NCR\\CCL Client Agent")
#define NUMUNLOCKADDR 3
#define SERIALNOLENGTH 10
#define UNLOCKNOLENGTH 24

/*===========================================================================
** Synopsis:    VOID maintCheckDataLength(&MaintWork.UnlockNo)
*
*     Input:    UCHAR *aszUnlockNo - standard char destination for copied data
*				UCHAR *registryData - standard source of data to copy (even characters
*									are NULL character b/c of wide->standard conversion)
*				int dataLen - length of the string contained in registryData
*    Output:    aszUnlockNo -holds data copied from registryData,
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function parses high-byte null characters from a uchar array that was
*				filled as though it were a wide char array.  Function processes the number
*				of characters in dataLen until it encounters two consecutive null characters
*===========================================================================
*/
SHORT maintCheckDataLength(PARAUNLOCKNO *UnlockInfo){
	int length;
	for(length=0; length < sizeof(UnlockInfo->aszUnlockNo); length++){
		if(UnlockInfo->aszUnlockNo[length] == '\0') break;
	}

	if((UnlockInfo->uchAddress == 1) && (length != SERIALNOLENGTH)){
		return (LDT_KEYOVER_ADR);	//Serial Number length not correct
	}
	if((UnlockInfo->uchAddress == 2) && (length)){
		return (LDT_KEYOVER_ADR);	//Unlock Number length not correct
	}
	if((UnlockInfo->uchAddress == 3) && (length != UNLOCKNOLENGTH)){
		return (LDT_KEYOVER_ADR);	//Some data entered at key code address
	}

	return(0); //Data entry lengths are okay
}


/*
*===========================================================================
** Synopsis:    SHORT MaintUnlockNoRead( PARAUNLOCKNO *pData )
*
*     Input:    *pData          : pointer to structure for PARAUNLOCKNO
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      SUCCESS          : read data
*               LDT_KEYOVER_ADR  : wrong data
*
** Description: This function reads & displays system's unlock # or serial #.
*===========================================================================
*/

SHORT MaintUnlockNoRead( PARAUNLOCKNO *pData )
{
    UCHAR uchAddress;
	CONST SYSCONFIG * const pSysConfig = PifSysConfig();
	TCHAR	szExpireVal[10 + 1] = { 0 };

	memset(MaintWork.UnlockNo.aszMnemonic, 0x00, sizeof(MaintWork.UnlockNo.aszMnemonic));

    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {//P5 key pressed w/out prior data entry go to next address
		if(++MaintWork.UnlockNo.uchAddress > NUMUNLOCKADDR){			//loopback to first address when past 2
			uchAddress = MaintWork.UnlockNo.uchAddress = 1;
		}
		else {
			uchAddress = MaintWork.UnlockNo.uchAddress;		//increment current address
		}

    } else {							//P5 key pressed with data entered (goto entered address)
		if(pData->uchAddress > NUMUNLOCKADDR){
			uchAddress = pData->uchAddress = 1;
		}
		else {
			uchAddress = pData->uchAddress;
		}
    }

    MaintWork.UnlockNo.uchMajorClass = pData->uchMajorClass;  // copy major class info
    MaintWork.UnlockNo.uchAddress = uchAddress;				  // copy address to read from

	switch(uchAddress){
	case 1:
		//Read the Terminal's Serial Number from the Registry
		maintUnlockNoRegRead(&(MaintWork.UnlockNo));              // call registry reading function
		break;
	case 2:
		memset(MaintWork.UnlockNo.aszUnlockNo, 0x00, sizeof(MaintWork.UnlockNo.aszUnlockNo));
		//Read the Software Serial
		BlFwIfReadSerial(MaintWork.UnlockNo.aszUnlockNo);
		break;
	case 3:
		//Read the Unlock Code from the registry
		//if the Unlock number is Null read from the
		//Parameters
		maintUnlockNoRegRead(&(MaintWork.UnlockNo));
		if(MaintWork.UnlockNo.aszUnlockNo[0] == 0x00){
			int   i = 0, j = 0;
			PARATERMINALINFO TermInfo = { 0 };

			TermInfo.uchMajorClass = CLASS_PARATERMINALINFO;
			//NHPOS_ASSERT(pSysConfig->usTerminalPosition <= 0xFF);//MAXBYTE
			TermInfo.uchAddress = PIF_CLUSTER_MASK_TERMINAL_NO(pSysConfig->usTerminalPositionFromName);
			ParaTerminalInfoParaRead (&TermInfo);
			for(i = 0, j = 0; i < 24 && j < 12; j++){
				//place the first number, which is the HIBYTE of aszUnlockCode
				//then place the second number, which is the LOBYTE of aszUnlockCode
				MaintWork.UnlockNo.aszUnlockNo[i++] = LOBYTE(TermInfo.TerminalInfo.aszUnLockCode[j]);
				MaintWork.UnlockNo.aszUnlockNo[i++] = HIBYTE(TermInfo.TerminalInfo.aszUnLockCode[j]);
			}
		}
		RflGetLead(MaintWork.UnlockNo.aszMnemonic, LDT_DATA_ADR);

		break;
	default:
		return -1;
		break;
	}
	BlFwIfReadExpStr(szExpireVal);

	_tcscpy(MaintWork.UnlockNo.aszExpireVal, szExpireVal);
    DispWrite(&(MaintWork.UnlockNo));				  // call to UI for displaying info on 2x20
	return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintUnlockNoWrite( PARAUNLOCKNO *pData )
*
*     Input:    *pData             : pointer to structure for PARAUNLOCKNO
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      SUCCESS            : read data
*               LDT_SEQERR_ADR     : error
*
** Description: This function sets & prints unlock # or serial #
*===========================================================================
*/

SHORT MaintUnlockNoWrite( PARAUNLOCKNO *pData )
{
	CONST SYSCONFIG * const pSysConfig = PifSysConfig();

    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {	//P1 pressed without prior data entry
        return(LDT_SEQERR_ADR);                                 /* sequence error */
    }

	if(maintCheckDataLength(pData)){
		return(LDT_KEYOVER_ADR);
	}
	if(MaintWork.UnlockNo.uchAddress == 2){
		return(LDT_PROHBT_ADR);
	}

	//  *Unlock number already checked by UIE*
	_tcsncpy (MaintWork.UnlockNo.aszUnlockNo, pData->aszUnlockNo, PARA_UNLOCK_LEN);

    /* control header item */

	if(MaintWork.UnlockNo.uchAddress == 1){
		maintUnlockNoRegWrite(&MaintWork.UnlockNo);       // call function to write to registry
	} else if(MaintWork.UnlockNo.uchAddress == 3){
		BlFwIfLicReg(MaintWork.UnlockNo.aszUnlockNo, MaintWork.UnlockNo.aszMnemonic);
		//Write message to display
		while(MaintWork.UnlockNo.aszMnemonic[0] == 0){
			PifSleep(500);
		}
		if(_tcscmp(MaintWork.UnlockNo.aszMnemonic, _T("Licensed")) == 0){
			PARATERMINALINFO TermInfo = { 0 };
			int i = 0, j = 0;

			//Write the unlock number to the registry
			//then write it to the parameters
			maintUnlockNoRegWrite(&MaintWork.UnlockNo);
			TermInfo.uchMajorClass = CLASS_PARATERMINALINFO;
			//NHPOS_ASSERT(pSysConfig->usTerminalPosition <= 0xFF);//MAXBYTE
			TermInfo.uchAddress = PIF_CLUSTER_MASK_TERMINAL_NO(pSysConfig->usTerminalPositionFromName);
			ParaTerminalInfoParaRead (&TermInfo);
			for(i = 0, j = 0; j < 12;j++, i += 2){
				TermInfo.TerminalInfo.aszUnLockCode[j] = 
					MAKEWORD(LOBYTE(MaintWork.UnlockNo.aszUnlockNo[i]), LOBYTE(MaintWork.UnlockNo.aszUnlockNo[i+1]));
			}
			ParaTerminalInfoParaWrite(&TermInfo);
		}
	}

    MaintHeaderCtl(PG_UNLOCK_NO, RPT_PRG_ADR);

    /* set journal bit & receipt bit */
    MaintWork.UnlockNo.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.UnlockNo));

	if(_tcscmp(MaintWork.UnlockNo.aszMnemonic, _T("Licensed")) && MaintWork.UnlockNo.uchAddress == 3){
		return(LDT_UNREGISTERED_ADR);
	}
	MaintWork.UnlockNo.uchAddress++;	// set address for display next address
	//When passing address 3, roll over to 1
	if(MaintWork.UnlockNo.uchAddress > NUMUNLOCKADDR)
		MaintWork.UnlockNo.uchAddress = 1;
	return(MaintUnlockNoRead(&(MaintWork.UnlockNo)));
}

/*
*===========================================================================
** Synopsis:    VOID MaintUnlockNoReport( VOID )
*
*     Input:    Nothing
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function prints all Unlock Number data.
*===========================================================================
*/

VOID MaintUnlockNoReport( VOID )
{
	PARAUNLOCKNO      UnlockNo = { 0 };
	int i; //counter
	CONST SYSCONFIG * const pSysConfig = PifSysConfig();

    /* control header item */
    MaintHeaderCtl(PG_UNLOCK_NO, RPT_PRG_ADR);
    UnlockNo.uchMajorClass = CLASS_PARAUNLOCKNO;

    /* set journal bit & receipt bit */
    UnlockNo.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

	/* check abort key */
	if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
		MaintMakeAbortKey();
	}
	for(i = 1; i <= NUMUNLOCKADDR; i++){
		UnlockNo.uchAddress = i;
		switch(UnlockNo.uchAddress){
		case 1:
			//Read the Terminal's Serial Number from the Registry
			maintUnlockNoRegRead(&UnlockNo);	//retrieve security data
			break;
		case 2:
			//Read the Software Serial
			BlFwIfReadSerial(UnlockNo.aszUnlockNo);
			break;
		case 3:
			//Read the Unlock Code from the registry
			//if the Unlock number is Null read from the
			//Parameters
			maintUnlockNoRegRead(&UnlockNo);
			if(UnlockNo.aszUnlockNo[0] == 0x00){
				PARATERMINALINFO TermInfo = { 0 };
				int j = 0, k = 0; //counter

				TermInfo.uchMajorClass = CLASS_PARATERMINALINFO;
				//NHPOS_ASSERT(pSysConfig->usTerminalPosition <= 0xFF);//MAXBYTE
				TermInfo.uchAddress = PIF_CLUSTER_MASK_TERMINAL_NO(pSysConfig->usTerminalPositionFromName);
				ParaTerminalInfoParaRead (&TermInfo);
				for(j = 0, k = 0; j < 24;){
					//place the first number, which is the HIBYTE of aszUnlockCode
					//then place the second number, which is the LOBYTE of aszUnlockCode
					UnlockNo.aszUnlockNo[j++] = LOBYTE(TermInfo.TerminalInfo.aszUnLockCode[k]);
					UnlockNo.aszUnlockNo[j++] = HIBYTE(TermInfo.TerminalInfo.aszUnLockCode[k++]);
				}
				UnlockNo.aszUnlockNo[j] = 0x00;
			}
			break;
		default:
			break;
		}
		PrtPrintItem(NULL, &UnlockNo);		//print security data
	}

    /* make trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTPRG);

}


/*
*===========================================================================
** Synopsis:    static maintUnlockNoRegRead( VOID )
*
*     Input:    PARAUNLOCKNO *securityData
*    Output:    Nothing
*     InOut:    scurityData->aszUnlockNo recieves the specified key value's data
*
** Return:      Nothing
*
** Description: This function reads serial # or unlock # directly from registry
*				Performs simple enumeration on the NCR\\CCL Client Agent key's values,
*				writing the specified value to securitydata->aszUnlockNo
*===========================================================================
*/


SHORT maintUnlockNoRegRead (PARAUNLOCKNO *securityData)
{
    HKEY hKey;
	TCHAR xValue[200];
	DWORD nValue = 200;
	UCHAR xData[200];
	DWORD nData = 200;
	DWORD dwIndex = 0;
	DWORD dwInnerIndex = 0;
	DWORD nType = REG_SZ;
	int nResult1, nResult2, nResult3;
	int userSerial = 0;

	//effectively empty the In/Out storage variable so that non existent keys show a cleared display
	//only need to seed a NULL since DispWritew works on null terminated strings  by cwunn SR 15
	securityData->aszUnlockNo[0] = '\0';
#ifdef XP_PORT
	nResult1 = RegOpenKeyEx(HKEY_LOCAL_MACHINE, X_7448_REG_KEY, 0, KEY_READ, &hKey);
#else
	nResult1 = RegOpenKeyEx(HKEY_LOCAL_MACHINE, X_7448_REG_KEY, 0, 0, &hKey);
#endif //XP_PORT
	if (nResult1 == ERROR_SUCCESS) {
		nResult2 = ERROR_SUCCESS;
		while (nResult2 == ERROR_SUCCESS) {
			nValue = sizeof(xValue);
			nData = sizeof(xData);
			nResult2 = RegEnumValue(hKey, dwIndex, xValue, &nValue, NULL, &nType, xData, &nData);
			if(securityData->uchAddress == 3 && _tcsstr(xValue, NHPOSUNLOCKCODEVALUE)){
				maintUnlockNoRegReadCopy(securityData, xData, nData);
				break;
			}
//			if (securityData->uchAddress == 2 && _tcsstr (xValue, NHPOSKEYCODEVALUE)) {
//				maintUnlockNoRegReadCopy(securityData, xData, nData);
//				break;
//			}
			if (securityData->uchAddress == 1 ) {
				//if for no user entered serial number, query system serial number below.
				if(xData[0] != '\0')
				{
					if(_tcsstr (xValue, NHPOSSERIALNOVALUE)){ //if user entered serial, use it here
						if(maintUnlockNoRegReadCopy(securityData, xData, nData) == -1) {
							maintUnlockNoRegWrite(securityData);
							secReadRegistry();
							secGenKeyCode();
						}
						userSerial = 1;
						break;
					}
				}

			}
			dwIndex++;
		}
		if(!userSerial && (securityData->uchAddress == 1)){
			do {
				nResult3 = RegEnumValue(hKey, dwInnerIndex++, xValue, &nValue, NULL, &nType, xData, &nData);
				if(_tcsstr(xValue, SYSTEMSERIALNO)){
					maintUnlockNoRegReadCopy(securityData, xData, nData);
					maintUnlockNoRegWrite(securityData);
					//secReadRegistry();
					//secGenKeyCode();
					break;
				}

				nValue = 200;
				nData = 200;
			} while(nResult3 == ERROR_SUCCESS);
		}
		RegCloseKey (hKey);
	}
	return SUCCESS;
}

/*===========================================================================
** Synopsis:    static maintUnlockNoRegWrite(PARAUNLOCKNO *)
*
*     Input:    PARAUNLOCKNO *securityData
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function writes serial # or unlock # directly to registry
*				Performs simple enumeration on the NCR\\CCL Client Agent key's values,
*				writing the specified value to securitydata->aszUnlockNo
*===========================================================================
*/
SHORT maintUnlockNoRegWrite (PARAUNLOCKNO *securityData)
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
#endif //XP_PORT

	if (nResult1 == ERROR_SUCCESS) {
		//Determine which key value to write to
		if(securityData->uchAddress == 1){
			_tcscpy (xValue, NHPOSSERIALNOVALUE);
		}
		if(securityData->uchAddress == 3){
			_tcscpy (xValue, NHPOSUNLOCKCODEVALUE);
		}

		nData = (_tcslen (securityData->aszUnlockNo) + 1) * 2; //RegSetValueEx needs number of Bytes not characters
		nResult2 = RegSetValueEx(hKey, xValue, 0, nType, (/*BYTE*/UCHAR*)securityData->aszUnlockNo, nData);
		RegCloseKey (hKey);
	}
	return SUCCESS;
}

#undef NHPOSUNLOCKCODEVALUE
#undef X_7448_REG_KEY

/*===========================================================================
** Synopsis:    VOID maintUnlockNoRegReadCopy(PARAUNLOCKNO *securityData, UCHAR *, int )
*
*     Input:    UCHAR *aszUnlockNo - standard char destination for copied data
*				UCHAR *registryData - standard source of data to copy (even characters
*									are NULL character b/c of wide->standard conversion)
*				int dataLen - length of the string contained in registryData
*    Output:    aszUnlockNo -holds data copied from registryData,
*     InOut:    Nothing
*
** Return:      SUCCESS (0) - copy completed, no hyphens found
*				(-1) - copy completed, hyphens found and removed
*
** Description: This function parses high-byte null characters from a uchar array that was
*				filled as though it were a wide char array.  Function processes the number
*				of characters in dataLen until it encounters two consecutive null characters
*===========================================================================
*/
static SHORT maintUnlockNoRegReadCopy (PARAUNLOCKNO *securityData, UCHAR *registryData, int dataLen  )
{
	int endRegData=2; //used to exit upon 2 null characters in a row
	SHORT hyphenFound = SUCCESS;
	int i=0;
	int j=0;
	//clear aszUnlock *necessary to correctly display serial no & unlock no in prog95
	for(i=0;i<=dataLen+1;i++) securityData->aszUnlockNo[i]='\0';
	//following loop parses out high-byte nulls from the registryData array,
	// making aszUnlockNo suitable for use by NHPOS Dispwrite
	for(i=0; i<dataLen; i++){
		if(registryData[i]){
			if((registryData[i] == '-') && (securityData->uchAddress == 1)){
				endRegData=2;
				hyphenFound = -1; //indicate a hyphen was found (used to ensure NHPOS Serial is hyphenless)
				continue; //do not copy hyphens when copying serial number
			}
			securityData->aszUnlockNo[j++] = registryData[i];
			endRegData=2;
		}
		else {
			if(registryData[i] == '\0'){
				endRegData--;
				if(endRegData==0) break;
			}
		}
	}
	return hyphenFound;
}

