/* pifconf.c : Defines the entry point for the DLL application. */
/*

		NeighborhoodPOS Software Product Development Project
		Department of Information Systems
		College of Information Technology
		Georgia Southern University
		Statesboro, Georgia

		Copyright Georgia Southern University Foundation

-----------------------------------------------------------------------
:   Title          : PIF MODULE
:   Category       : PIF MODULE, Platform Independent Functions
:   Program Name   : pifconf.c (intialize PIF data)
:  ---------------------------------------------------------------------
:  Abstract:
:
:  ---------------------------------------------------------------------
:  Update Histories
:   Date   : Ver.Rev.  : Name         : Description
:*GenPOS Rel 2.2
: Dec-19-16: 02.02.01  : R. Chambers  : No longer use Scale and Receipt Printer with SysConfig.auchComPort[]
-------------------------------------------------------------------------*/

#pragma warning( disable : 4201 4214 4514)
#include "stdafx.h"
#include <winioctl.h>
#include "windows.h"
#include "ecr.h"
#include "pif.h"
#include "log.h"
#include "scf.h"
#include "printercaps.h"
#include "piflocal.h"

#define MAX_DEVICE 4

SYSCONFIG   SysConfig;

PIF_FAULT_LOG       FaultLog;
PIF_KEY_TRACK       KeyTrack;
PIF_FUNC_TRACK      FuncTrack;
PIF_ERROR_TRACK     ErrorTrack;
PIF_EXCEPTION_LOG   ExceptionLog;
PIF_UNIQUE_INFO     UniqueInfo;              /* add for 2170 R3 */
PIF_RESOURCE        Resource;
PIF_SYSTEM_HEADER   SystemHeader;
PIF_SOFT_INFO       SoftInfo;
UCHAR               aTallyRJ[32];
UCHAR               aTallySlip[32];
TCHAR               auchModemStrings[PIF_LEN_M0DEM_STR+1];

extern TCHAR wszPifPathName[MAX_PATH];

static const TCHAR auchInitialModemStrings[] = _T("AT&F1E0Q0V1&B1&R1S0=1\r\n\0");
//static const UCHAR auchInitialModemStrings[] = "AT&F1E0Q0V1&B1&R1S0=1\r\n\0";

static USHORT PifGetLanConfig(TCHAR *pachDevice, UCHAR uchDeviceId, USHORT usDefault, UCHAR *ucIP, USHORT *usPort, USHORT *usStoreForward);
static USHORT PifGetSerialConfig(TCHAR *pachDevice, UCHAR uchDeviceId, USHORT usDefault);
/* VOID PifGetSerialConfig(PIFCNFBLK *pCnfBlk, TCHAR *pachDevice, UCHAR uchDeviceId); */
static DWORD PifGetParameter(LPCTSTR lpszDeviceName, LPCTSTR lpszDataName,
                                LPDWORD lpDataType, LPVOID lpData, DWORD dwNumberOfBytesToRead,
                                LPDWORD lpNumberOfBytesRead);

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PifGetConfig(VOID)                             **
**                                                                  **
**  Description:Get configration from SystemConfig.DLL              **
**                                                                  **
**********************************************************************
fhfh*/
VOID PifSetSysConfig(VOID)
{
    // set configration to SysConfig
    UCHAR   *puchWork;
    TCHAR   *pDeviceName;
    TCHAR   *pDllName;
    DWORD   dwData, dwBytes;
    DWORD   dwCount;
    DWORD   dwDisposition, dwType;
    DWORD   dwRet;
    LONG    lRet;
    HKEY    hKey;
    USHORT  usLength;
    UCHAR   uchWork;
    TCHAR   atchDeviceName[SCF_USER_BUFFER] = {0};
    TCHAR   atchDllName[SCF_USER_BUFFER] = {0};
	TCHAR   wszKey[256] = {0};

    memset(SysConfig.auchComPort, DEVICE_NONE, PIF_LEN_PORT);

	SysConfig.usInterfaceDetect = 0;   // Initialize to zero for possible setting for SCF_TYPENAME_MSR1, SCF_TYPENAME_EPT_EX_DLL, etc.

    /* operator display, LCD or 2x20 */
    SysConfig.uchOperType = DISP_LCD;
    pDeviceName = atchDeviceName;
    pDllName    = atchDllName;
	dwCount = 0;
    dwRet = ScfGetActiveDevice(SCF_TYPENAME_ODISPLAY, &dwCount, pDeviceName, pDllName);
    if ((dwRet == SCF_SUCCESS) && (dwCount)) {
        if (PifGetSerialConfig(pDeviceName, DEVICE_ODISPLAY, TRUE) == TRUE) {
            SysConfig.uchOperType = DISP_2X20;
        }
    }

    /* customer display, 2x20, 10N10 or 4x20 */
    SysConfig.uchCustType = DISP_NONE;
    pDeviceName = atchDeviceName;
    pDllName    = atchDllName;
	dwCount = 0;
    dwRet = ScfGetActiveDevice(SCF_TYPENAME_CDISPLAY, &dwCount, pDeviceName, pDllName);
    if ((dwRet == SCF_SUCCESS) && (dwCount)) {
		static const TCHAR wsz1x10Display[] = TEXT("1x10");
		static const TCHAR wsz2x20Display[] = TEXT("2x20");
		static const TCHAR wsz4x20Display[] = TEXT("4x20");
		DWORD   i;
		//Assume the display is 2x20
		SysConfig.uchCustType = DISP_2X20;
        for (i=0; i<dwCount; i++) {
            if (PifGetSerialConfig(pDeviceName, DEVICE_CDISPLAY, TRUE) == TRUE) {
                if (wcsstr(pDeviceName, wsz1x10Display) != NULL) {
                    SysConfig.uchCustType = DISP_10N10D;
                } else if (wcsstr(pDeviceName, wsz2x20Display) != NULL) {
                    SysConfig.uchCustType = DISP_2X20;
                } else if (wcsstr(pDeviceName, wsz4x20Display) != NULL) {
                    SysConfig.uchCustType = DISP_4X20;
                }

                if (SysConfig.uchCustType == DISP_NONE) {
                    SysConfig.uchCustType = DISP_2X20;  /* assume as default */
                }
            }
            // move pointer
            pDeviceName += wcslen(pDeviceName) + 1;
        }
    }

    /*
		default keyboard type
			NCR 7448 terminal micromotion keyboard
			NCR 7448 terminal conventional keyboard
			NCR 5932 of various types

			NCR 7448 was the sucessor to the NCR 2170 for NHPOS Rel 1.4. Both allowed
			two different kinds of keyboards, one with standard motion keys and one with
			micromotion keys. NCR 7448 was manufacture discontinued around 2003 by NCR.

			See pif.h for keyboard types defined.
	 */
	SysConfig.uchKeyType = KEYBOARD_TOUCH;
    pDeviceName = atchDeviceName;
    pDllName    = atchDllName;
	dwCount = 0;
    dwRet = ScfGetActiveDevice(SCF_TYPENAME_KEYBOARD, &dwCount, pDeviceName, pDllName);
    if ((dwRet == SCF_SUCCESS) && (dwCount)) {
		if (wcscmp (KEYBOARD_MICRO_NAME, pDeviceName) == 0)
			SysConfig.uchKeyType = KEYBOARD_MICRO;
		else if (wcscmp (KEYBOARD_CONV_NAME, pDeviceName) == 0)
			SysConfig.uchKeyType = KEYBOARD_CONVENTION;
		else if (wcscmp (KEYBOARD_WEDGE_68_NAME, pDeviceName) == 0)
			SysConfig.uchKeyType = KEYBOARD_WEDGE_68;
		else if (wcscmp (KEYBOARD_NONE_NAME, pDeviceName) == 0)
			SysConfig.uchKeyType = KEYBOARD_NONE;
		else if (wcscmp (KEYBOARD_TOUCH_NAME, pDeviceName) == 0)
			SysConfig.uchKeyType = KEYBOARD_TOUCH;
		else if (wcscmp (KEYBOARD_WEDGE_78_NAME, pDeviceName) == 0)
			SysConfig.uchKeyType = KEYBOARD_WEDGE_78;
	}

    /* drawer, 0, 1 or 2 */
    SysConfig.uchDrawer1 = DRAWER_NONE;
    pDeviceName = atchDeviceName;
    pDllName    = atchDllName;
	dwCount = 0;
    dwRet = ScfGetActiveDevice(SCF_TYPENAME_DRAWER1, &dwCount, pDeviceName, pDllName);
    if (dwRet == SCF_SUCCESS) {
        SysConfig.uchDrawer1 = DRAWER_PROVIDED;
    }

    SysConfig.uchDrawer2 = DRAWER_NONE;
	dwCount = 0;
	dwRet = ScfGetActiveDevice(SCF_TYPENAME_DRAWER2, &dwCount, pDeviceName, pDllName);
    if (dwRet == SCF_SUCCESS) {
        SysConfig.uchDrawer2 = DRAWER_PROVIDED;
    }

    /* MSR, ISO Track 2 or JIS */
    SysConfig.uchIntegMSRType = IMSR_NONE;
    pDeviceName = atchDeviceName;
    pDllName    = atchDllName;
	dwCount = 0;
    dwRet = ScfGetActiveDevice(SCF_TYPENAME_MSR, &dwCount, pDeviceName, pDllName);
    if ((dwRet == SCF_SUCCESS) && (dwCount)) {
		if (_tcscmp (pDllName, SCF_INTERFACE_WEDGE) == 0) {
			// This MSR is a keyboard wedge type of device so we are going to modify
			// the behavior of key code scanning in DeviceEngine to handle wedge key streams.
			// keywords are KEYBOARD_VIRTUAL, PIF_VIRTUALKEYEVENT, SCF_DEVICENAME_EPTEXDLL_FREEDOMPAY
			SysConfig.usInterfaceDetect |= SYSCONFIG_IF_WEDGE_ON;
		}
		else {
			if( _tcsspn(atchDeviceName, VIVOTECH_MSR) == 8){
				SysConfig.uchIntegMSRType |= IMSR_CPT_TRACK2;
			}
			else
				SysConfig.uchIntegMSRType |= IMSR_TRACK12;
		}
    }

	// Remove legacy MSR (Magnetic Stripe Reader) code for OpenGenPOS.
	// Remmove legacy signature capture code for OpenGenPOS.
	// Remove legacy pinpad code for OpenGenPOS.
	// This is to eliminate the possibility of not meeting PCI-DSS account holder information
	// security requirements. GenPOS Rel 2.3.0 retained legacy source but was using the
	// Datacap Out Of Scope control for electronic payment.
	//
	//    OpenGenPOS Rel 2.4.0  3/30/2021  Richard Chambers

    /* waiter lock */
    SysConfig.uchWaiterType = WAITER_NONE;
    pDeviceName = atchDeviceName;
    pDllName    = atchDllName;
	dwCount = 0;
    dwRet = ScfGetActiveDevice(SCF_TYPENAME_SERVERLOCK, &dwCount, pDeviceName, pDllName);
    if ((dwRet == SCF_SUCCESS) && (dwCount)) {
        SysConfig.uchWaiterType = WAITER_LOCK;
    }

	// Remove legacy Charge Post code for OpenGenPOS. DEVICE_CHARGE_POSTING
	// Remove legacy Electronic Payment Terminal code for OpenGenPOS. DEVICE_EPT
	// Remove legacy Enhanced Electronic Payment Terminal code for OpenGenPOS. DEVICE_EPT_EX
	// This is to eliminate the possibility of not meeting PCI-DSS account holder information
	// security requirements. GenPOS Rel 2.3.0 retained legacy source but was using the
	// Datacap Out Of Scope control for electronic payment.
	//
	//    OpenGenPOS Rel 2.4.0  3/22/2021 - 3/30/2021  Richard Chambers

	//  Now we cycle through the Extended EPT interfaces looking for the types of
	//  extended EPT devices.  There are several including the older communications
	//  port (serial) interface, an ethernet interface to a server process, or an
	//  interface through a specialized DLL.
    pDeviceName = atchDeviceName;
    pDllName    = atchDllName;
	dwCount = 0;
	dwRet = ScfGetActiveDevice(SCF_TYPENAME_EPT_EX_LAN, &dwCount, pDeviceName, pDllName);
	if ((dwRet == SCF_SUCCESS) && (dwCount)) {
		SysConfig.usInterfaceDetect |= SYSCONFIG_IF_EEPT_LAN;
		SysConfig.EPT_ENH_Info[PIF_EPT_ENH_EX_LAN].ulDevice = DEVICE_EPT_EX_LAN;
		// We're now using MDC bits instead of these device registry settings. I've left the code because it's not hurting anything,
		// and might be useful in the future.
		PifGetLanConfig(pDeviceName, DEVICE_EPT_EX_LAN, FALSE, SysConfig.EPT_ENH_Info[PIF_EPT_ENH_EX_LAN].ucIpAddress, &SysConfig.EPT_ENH_Info[PIF_EPT_ENH_EX_LAN].usPortNo, &SysConfig.EPT_ENH_Info[PIF_EPT_ENH_EX_LAN].usStoreForward);
	}

	dwCount = 0;
	dwRet = ScfGetActiveDevice(SCF_TYPENAME_EPT_EX_DLL, &dwCount, pDeviceName, pDllName);
	if ((dwRet == SCF_SUCCESS) && (dwCount)) {
		SysConfig.usInterfaceDetect |= SYSCONFIG_IF_EEPT_DLL;
		SysConfig.EPT_ENH_Info[PIF_EPT_ENH_EX_DLL].ulDevice = DEVICE_EPT_EX_DLL;
		// We're now using MDC bits instead of these device registry settings. I've left the code because it's not hurting anything,
		// and might be useful in the future.
		PifGetLanConfig(pDeviceName, DEVICE_EPT_EX_DLL, FALSE, SysConfig.EPT_ENH_Info[PIF_EPT_ENH_EX_DLL].ucIpAddress, &SysConfig.EPT_ENH_Info[PIF_EPT_ENH_EX_DLL].usPortNo, &SysConfig.EPT_ENH_Info[PIF_EPT_ENH_EX_DLL].usStoreForward);
	}

	dwCount = 0;
	dwRet = ScfGetActiveDevice(SCF_TYPENAME_EPT_EX_DLL1, &dwCount, pDeviceName, pDllName);
	if ((dwRet == SCF_SUCCESS) && (dwCount)) {
		// This DLL interface is designated as the DLL interface for virtual
		// keyboard type devices such as the FreedomPay EPT interface.
		// keywords are KEYBOARD_VIRTUAL, PIF_VIRTUALKEYEVENT, SCF_DEVICENAME_EPTEXDLL_FREEDOMPAY
		SysConfig.usInterfaceDetect |= SYSCONFIG_IF_EEPT_DLL1;
		SysConfig.EPT_ENH_Info[PIF_EPT_ENH_EX_DLL1].ulDevice = DEVICE_EPT_EX_DLL1;
		PifGetLanConfig(pDeviceName, DEVICE_EPT_EX_DLL1, FALSE, SysConfig.EPT_ENH_Info[PIF_EPT_ENH_EX_DLL1].ucIpAddress, &SysConfig.EPT_ENH_Info[PIF_EPT_ENH_EX_DLL1].usPortNo, &SysConfig.EPT_ENH_Info[PIF_EPT_ENH_EX_DLL1].usStoreForward);
	}

	dwCount = 0;
	dwRet = ScfGetActiveDevice(SCF_TYPENAME_EPT_EX_DLL2, &dwCount, pDeviceName, pDllName);
	if ((dwRet == SCF_SUCCESS) && (dwCount)) {
		SysConfig.usInterfaceDetect |= SYSCONFIG_IF_EEPT_DLL2;
		SysConfig.EPT_ENH_Info[PIF_EPT_ENH_EX_DLL2].ulDevice = DEVICE_EPT_EX_DLL2;
		PifGetLanConfig(pDeviceName, DEVICE_EPT_EX_DLL2, FALSE, SysConfig.EPT_ENH_Info[PIF_EPT_ENH_EX_DLL2].ucIpAddress, &SysConfig.EPT_ENH_Info[PIF_EPT_ENH_EX_DLL2].usPortNo, &SysConfig.EPT_ENH_Info[PIF_EPT_ENH_EX_DLL2].usStoreForward);
	}

	dwCount = 0;
	dwRet = ScfGetActiveDevice(SCF_TYPENAME_EPT_EX_DLL3, &dwCount, pDeviceName, pDllName);
	if ((dwRet == SCF_SUCCESS) && (dwCount)) {
		SysConfig.usInterfaceDetect |= SYSCONFIG_IF_EEPT_DLL3;
		SysConfig.EPT_ENH_Info[PIF_EPT_ENH_EX_DLL3].ulDevice = DEVICE_EPT_EX_DLL3;
		PifGetLanConfig(pDeviceName, DEVICE_EPT_EX_DLL3, FALSE, SysConfig.EPT_ENH_Info[PIF_EPT_ENH_EX_DLL3].ucIpAddress, &SysConfig.EPT_ENH_Info[PIF_EPT_ENH_EX_DLL3].usPortNo, &SysConfig.EPT_ENH_Info[PIF_EPT_ENH_EX_DLL3].usStoreForward);
	}

    /* Beverage Dispenser */
    pDeviceName = atchDeviceName;
    pDllName    = atchDllName;
	dwCount = 0;
    dwRet = ScfGetActiveDevice(SCF_TYPENAME_BEVERAGE, &dwCount, pDeviceName, pDllName);
    if ((dwRet == SCF_SUCCESS) && (dwCount)) {
		DWORD  i;
        for (i=0; i<dwCount; i++) {
            PifGetSerialConfig(pDeviceName, DEVICE_BEVERAGE, FALSE);
            // move pointer
            pDeviceName += wcslen(pDeviceName) + 1;
        }
    }

    /* Kitchen Display */
    pDeviceName = atchDeviceName;
    pDllName    = atchDllName;
	dwCount = 0;
    dwRet = ScfGetActiveDevice(SCF_TYPENAME_KITCHEN_CRT, &dwCount, pDeviceName, pDllName);
    if ((dwRet == SCF_SUCCESS) && (dwCount)) {
		DWORD  i;
        for (i=0; i<dwCount; i++) {
            PifGetSerialConfig(pDeviceName, DEVICE_KITCHEN_CRT, FALSE);
            // move pointer
            pDeviceName += wcslen(pDeviceName) + 1;
        }
    }

    /* Kitchen Printer */
    pDeviceName = atchDeviceName;
    pDllName    = atchDllName;
	dwCount = 0;
    dwRet = ScfGetActiveDevice(SCF_TYPENAME_KITCHEN_PRINTER, &dwCount, pDeviceName, pDllName);
    if ((dwRet == SCF_SUCCESS) && (dwCount)) {
		DWORD  i;
        for (i=0; i<dwCount; i++) {
            PifGetSerialConfig(pDeviceName, DEVICE_KITCHEN_PRINTER, TRUE);
            // move pointer
            pDeviceName += wcslen(pDeviceName) + 1;
        }
    }

	// Remove legacy PCIF over Serial port code for OpenGenPOS.
	// This was a hold over from NHPOS Rel 1.4 and the NCR 7448.
	// PIF name is DEVICE_PC
	//
	//    OpenGenPOS Rel 2.4.0  3/30/2021  Richard Chambers

#if 0
	// The receipt printer nor scale uses the SysConfig port data for set up and configuration.
	// We are removing this as it is not needed or necessary.
	//   Richard Chambers, Rel 2.2.1 12/19/2016

	// NCR 7448 was the sucessor to the NCR 2170 for NHPOS Rel 1.4.Both used direct
	// printer communications through a serial port, RS-232 with DB-9 connector, and
	// they did not use the OPOS components.
	// NCR 7448 was manufacture discontinued around 2003 by NCR.

	/* printer */
    SysConfig.uchPrinter = DEVICE_THERMAL_PRINTER;
    pDeviceName = atchDeviceName;
    pDllName    = atchDllName;
	dwCount = 0;
    dwRet = ScfGetActiveDevice(SCF_TYPENAME_PRINTER, &dwCount, pDeviceName, pDllName);
    if ((dwRet == SCF_SUCCESS) && (dwCount)) {
		DWORD  i;
        for (i=0; i<dwCount; i++) {
            PifGetSerialConfig(pDeviceName, DEVICE_THERMAL_PRINTER, FALSE);
            // move pointer
            pDeviceName += wcslen(pDeviceName) + 1;
        }
    }


    /* scale */
    pDeviceName = atchDeviceName;
    pDllName    = atchDllName;
	dwCount = 0;
    dwRet = ScfGetActiveDevice(SCF_TYPENAME_SCALE, &dwCount, pDeviceName, pDllName);  // removed RJC 12/12/2016
    if ((dwRet == SCF_SUCCESS) && (dwCount)) {
		DWORD  i;
        for (i=0; i<dwCount; i++) {
            PifGetSerialConfig(pDeviceName, DEVICE_SCALE, TRUE);
            // move pointer
            pDeviceName += wcslen(pDeviceName) + 1;
        }
    }
#endif

    /* coin dispenser */
    pDeviceName = atchDeviceName;
    pDllName    = atchDllName;
	dwCount = 0;
    dwRet = ScfGetActiveDevice(SCF_TYPENAME_COIN, &dwCount, pDeviceName, pDllName);
    if ((dwRet == SCF_SUCCESS) && (dwCount)) {
		DWORD  i;
        for (i=0; i<dwCount; i++) {
            PifGetSerialConfig(pDeviceName, DEVICE_COIN_DISPENSER, TRUE);
            // move pointer
            pDeviceName += wcslen(pDeviceName) + 1;
        }
    }

    /* scanner */
    pDeviceName = atchDeviceName;
    pDllName    = atchDllName;
	dwCount = 0;
    dwRet = ScfGetActiveDevice(SCF_TYPENAME_SCANNER, &dwCount, pDeviceName, pDllName);
    if ((dwRet == SCF_SUCCESS) && (dwCount)) {
		if (_tcscmp (pDllName, SCF_INTERFACE_WEDGE) == 0) {
			// This scanner is a keyboard wedge type of device so we are going to modify
			// the behavior of key code scanning in DeviceEngine to handle wedge key streams.
			// keywords are KEYBOARD_VIRTUAL, PIF_VIRTUALKEYEVENT, SCF_DEVICENAME_EPTEXDLL_FREEDOMPAY
			SysConfig.usInterfaceDetect |= SYSCONFIG_IF_WEDGE_ON;
		}
		else {
			DWORD  i;
			for (i=0; i<dwCount; i++) {
				PifGetSerialConfig(pDeviceName, DEVICE_SCANNER, TRUE);
				// move pointer
				pDeviceName += wcslen(pDeviceName) + 1;
			}
		}
    }

    pDeviceName = atchDeviceName;
    pDllName    = atchDllName;
	dwCount = 0;
    dwRet = ScfGetActiveDevice(SCF_TYPENAME_SCANNER1, &dwCount, pDeviceName, pDllName);
    if ((dwRet == SCF_SUCCESS) && (dwCount)) {
        SysConfig.usInterfaceDetect |= SYSCONFIG_IF_WEDGE_ON;
    }

    SysConfig.uchCom = COM_PROVIDED_NET;
    SysConfig.uchNet = NET_BOARD_PROVIDED;

    /* misc. option */
    wsprintf(wszKey, TEXT("%s\\%s"), PIFROOTKEY, OPTIONKEY);
    lRet = RegCreateKeyEx(HKEY_CURRENT_USER, wszKey, 0, _T(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
    if (lRet == ERROR_SUCCESS) {
        /* power down option */
        dwBytes = sizeof(dwData);

        lRet = RegQueryValueEx(hKey, OPTION7, 0, &dwType, (UCHAR *)&dwData, &dwBytes);
        if (lRet == ERROR_SUCCESS) {
            SysConfig.ausOption[UIE_POWER_OPTION7] = (USHORT)dwData;
        }

        /*
		 *    Set 2172 automated test option also known as UIE_TEST_OPTION8
		 *    There are a number of places where the value is checked against a value of UIE_TEST_PASS
		 *    in order to determine if the automated testing facilities are to be enabled.
		 *
		 *    In these locations if a data item has a mnemonic which starts with "A0A1" then the
		 *    auto-generation of that kind of data will be triggered.
		 *
		 *    Some of these facilities include auto-generation of the maximum number of some types of data
		 *        - PLUs in SerOpPluAssign()
		 *        - Cashiers in SerCasAssign()
		 *        - Departments in SerOpDeptAssign()
		 */
        dwBytes = sizeof(dwData);
        lRet = RegQueryValueEx(hKey, OPTION8, 0, &dwType, (UCHAR *)&dwData, &dwBytes);
        if (lRet == ERROR_SUCCESS) {
			NHPOS_NONASSERT_NOTE("==STATE", "==STATE: PifSetSysConfig() UIE_TEST_OPTION8 set for automation.");
            SysConfig.ausOption[UIE_TEST_OPTION8] = (USHORT)dwData;
        }
		
		//for Option9 used for testing VivoTech and Mercury EPT also known as UIE_TEXT_OPTION9
        dwBytes = sizeof(dwData);
        lRet = RegQueryValueEx(hKey, OPTION9, 0, &dwType, (UCHAR *)&dwData, &dwBytes);
        if (lRet == ERROR_SUCCESS) {
            SysConfig.ausOption[UIE_TEXT_OPTION9] = (USHORT)dwData;
			NHPOS_NONASSERT_NOTE("==STATE", "==STATE: PifSetSysConfig() UIE_TEXT_OPTION9 set for EPT.");
        }
    }

	{
		TCHAR tchEnvVal[10 + 1] = { 0 };

		if (GetEnvironmentVariable(_T("UIE_TEST_OPTION8"), tchEnvVal, 10)) {
			NHPOS_NONASSERT_NOTE("==STATE", "==STATE: PifSetSysConfig() GetEnvironmentVariable() UIE_TEST_OPTION8 set for automation.");
			SysConfig.ausOption[UIE_TEST_OPTION8] = UIE_TEST_PASS;
		}
	}

	SysConfig.pTallyRJ      = aTallyRJ;
    SysConfig.pTallySlip    = aTallySlip;

	memset(auchModemStrings, 0, sizeof(auchModemStrings));
    SysConfig.pModemStrings = auchModemStrings;
	_tcsncpy(auchModemStrings, auchInitialModemStrings, PIF_LEN_M0DEM_STR);

	wsprintf(wszKey, TEXT("%s\\%s"), PIFROOTKEY, NETWORKKEY);
    lRet = RegCreateKeyEx(HKEY_CURRENT_USER, wszKey, 0, _T(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
    if (lRet == ERROR_SUCCESS) {
		TCHAR   wszData[(sizeof(auchModemStrings)/sizeof(auchModemStrings[0]))];

        memset(wszData, 0, sizeof(wszData));

        dwBytes = sizeof(wszData);
        lRet = RegQueryValueEx(hKey, MODEMSTRINGS, 0, &dwType, (UCHAR *)&wszData[0], &dwBytes);
        if (lRet == ERROR_SUCCESS) {
			_tcsncpy(auchModemStrings, wszData, PIF_LEN_M0DEM_STR);
        } else {
            dwBytes = ((wcslen(auchModemStrings)+1) * 2);
            lRet = RegSetValueEx(hKey, MODEMSTRINGS, 0, REG_SZ, (UCHAR *)&auchModemStrings[0], dwBytes);
        }
    }

    /* initialize key track area    */
    KeyTrack.usMark     = 0xffff;
    KeyTrack.usMaxCount = sizeof(KeyTrack.auchKeyTrack);
    SysConfig.pKeyTrack    = &KeyTrack;

    /* initialize function track area   */
    FuncTrack.usMark     = 0xffff;
    FuncTrack.usMaxCount = sizeof(FuncTrack.aFuncTrack)/ sizeof(FuncTrack.aFuncTrack[0]);
    SysConfig.pFuncTrack    = &FuncTrack;

    /* initialize exception log area    */
    ExceptionLog.usMark     = 0xffff;
    ExceptionLog.usMaxCount = PIF_LEN_EXCEPTION_LOG;
    SysConfig.pExceptionLog    = &ExceptionLog;

    /* initialize fault log area    */
    FaultLog.usMark     = 0xffff;
    FaultLog.usMaxCount = PIF_LEN_FAULT_LOG;
    SysConfig.pFaultLog    = &FaultLog;

    /* initialize error track area  */
    ErrorTrack.usMark     = 0xffff;
    ErrorTrack.usMaxCount = PIF_LEN_FUNC_TRACK;
    SysConfig.pErrorTrack    = &ErrorTrack;


    /* unique information   */
	{
		int i = 0;
		TCHAR  szData[32] = {0};
		PifGetSystemVersion(szData);
		for (i = 0; i < PIF_LEN_SOFTWARE_ID; i++) {
			SystemHeader.szSystemId[i] = (UCHAR)szData[i];
		}
		SystemHeader.szSystemId[i] = 0;
	}

    UniqueInfo.pResource = &Resource;
    UniqueInfo.pSystemHeader = &SystemHeader;
    SysConfig.pUniqueInfo = &UniqueInfo;

    /* software information */
    strncpy((CHAR*)SoftInfo.auchSoftwareId, "CQK-???????XXX-XX", PIF_LEN_SOFTWARE_ID);
    strncpy((CHAR*)SoftInfo.auchPackageId, "CQK-PA?????XXX-XX", PIF_LEN_PACKAGE_ID);

    /* set sumcheck data */
    puchWork = (UCHAR *)&SoftInfo;
    uchWork = 0x00;
    for (usLength=0; usLength<sizeof(PIF_SOFT_INFO); usLength++) {
        uchWork ^= *puchWork++;
    }
    SoftInfo.uchSumCheck = uchWork;
    SysConfig.pSoftInfo = &SoftInfo;

	//Set the code page
	//936 is the code page used for converting
	//unicode strings to multibyte strings
	SysConfig.unCodePage = 936;

    PifSaveRegistry();
}

static UCHAR * RflStr2IpAddr (UCHAR *auchIpAddr, TCHAR *aszSource)
{
	int iNdx = 0;

	auchIpAddr[0] = auchIpAddr[1] = auchIpAddr[2] = auchIpAddr[3] = 0;

	for (iNdx = 0; iNdx < 4 && *aszSource; iNdx++) {
		USHORT  ipAddr = 0;
		while (*aszSource >= '0' && *aszSource <= '9') {
			ipAddr = ipAddr * 10 + *aszSource - '0';
			aszSource++;
		}
		auchIpAddr[iNdx] = ipAddr;
		if (*aszSource) aszSource++;
	}

	return auchIpAddr;
}


/*
*******************************************************************************
*
**Synopsis:     BOOL PifGetSerialCinfig(PSERIAL_CONFIG pSerialConf)
*
*               input   USHORT usErrorCode;  : Error Code
*
**Return:       error code
*
**Description:  Notify error code to registered callback address
*
*******************************************************************************
*/
static USHORT PifGetSerialConfig(TCHAR *pachDevice, UCHAR uchDeviceId, USHORT usDefault)
{
    DWORD   dwData = 0, dwDataType;
	DWORD   dwNoBytesRead = 0;
	TCHAR   atchPort[SCF_USER_BUFFER] = {0};
    UCHAR   uchPort=0;

    if (PifGetParameter(pachDevice, _T("Port"), &dwDataType, (LPVOID)atchPort, sizeof(atchPort), &dwNoBytesRead) != SCF_SUCCESS)
    {
        return FALSE;
    }

	uchPort = _ttoi (atchPort);
    if (uchPort < 1 || uchPort >= PIF_LEN_PORT) {
		UCHAR  uchGoodPort = 1;    // port in acceptable range to create assert
		char  xBuff[128];
		sprintf (xBuff, "==WARNING: PifGetSerialConfig() bad port - port %d Device %S", uchPort, pachDevice);
		NHPOS_ASSERT_TEXT((uchPort == uchGoodPort), xBuff);
        return FALSE;
    }

    SysConfig.usfPip[uchPort] = PIF_COM_PROTOCOL_NON;
	dwNoBytesRead = 0;
    if (ScfGetParameter(pachDevice, SCF_DATANAME_IPADDR, &dwDataType, (LPVOID)atchPort, sizeof(atchPort), &dwNoBytesRead) == SCF_SUCCESS && dwNoBytesRead > 6)
    {
		SysConfig.usfPip[uchPort] = PIF_COM_PROTOCOL_TCP;
		SysConfig.auchComPort[uchPort] = uchDeviceId;

		RflStr2IpAddr (SysConfig.xgComHeader[uchPort].auchFaddr, atchPort);
		SysConfig.xgComHeader[uchPort].usFport = 6001;       // this is the default LAN port for a GIANT receipt printer in LAN mode.
		SysConfig.xgComHeader[uchPort].usLport = ANYPORT;    // not used for a TCP type of connection.

		if (PifGetParameter(pachDevice, SCF_DATANAME_IPPORT, &dwDataType, (LPVOID)atchPort, sizeof(atchPort), &dwNoBytesRead) == SCF_SUCCESS)
		{
			SysConfig.xgComHeader[uchPort].usFport = _ttoi (atchPort);
		}
		{
			char  xBuff[128];

			sprintf (xBuff, "==PROVISIONING: COM device %d using TCP LAN %3.3d.%3.3d.%3.3d.%3.3d port %d", uchPort,
					SysConfig.xgComHeader[uchPort].auchFaddr[0], SysConfig.xgComHeader[uchPort].auchFaddr[1], SysConfig.xgComHeader[uchPort].auchFaddr[2], SysConfig.xgComHeader[uchPort].auchFaddr[3],
					SysConfig.xgComHeader[uchPort].usFport);
			NHPOS_NONASSERT_NOTE("==PROVISIONING", xBuff);
		}

		return TRUE;
    }

    if (PifGetParameter(pachDevice, SCF_DATANAME_BAUD, &dwDataType, (LPVOID)&dwData, sizeof(dwData), &dwNoBytesRead) != SCF_SUCCESS)
    {
        if (usDefault == FALSE) {   /* default parameter is not used */
            return FALSE;
        }
    }

    switch (dwData)
    {
    case SCF_CAPS_BAUD_300:
        SysConfig.ausComBaud[uchPort] = 300;
        break;
    case SCF_CAPS_BAUD_600:
        SysConfig.ausComBaud[uchPort] = 600;
        break;
    case SCF_CAPS_BAUD_1200:
        SysConfig.ausComBaud[uchPort] = 1200;
        break;
    case SCF_CAPS_BAUD_2400:
        SysConfig.ausComBaud[uchPort] = 2400;
        break;
    case SCF_CAPS_BAUD_4800:
        SysConfig.ausComBaud[uchPort] = 4800;
        break;
    case SCF_CAPS_BAUD_9600:
        SysConfig.ausComBaud[uchPort] = 9600;
        break;
    case SCF_CAPS_BAUD_19200:
        SysConfig.ausComBaud[uchPort] = 19200;
        break;
    case SCF_CAPS_BAUD_38400:
        SysConfig.ausComBaud[uchPort] = 38400;
        break;
    case SCF_CAPS_BAUD_57600:
        SysConfig.ausComBaud[uchPort] = 57600;
        break;
    case SCF_CAPS_BAUD_115200:
		// transform this to 57600 since anything larger will not fit in a UCHAR.
        SysConfig.ausComBaud[uchPort] = 57600;
		{
			char  xBuff[128];
			sprintf (xBuff, "==NOTE: PifGetSerialConfig() max rate 57600 baud exceeded, set to 57600. port %d Device %S", uchPort, pachDevice);
			NHPOS_NONASSERT_TEXT(xBuff);
		}
        break;
    default:
        SysConfig.ausComBaud[uchPort] = 19200;
		{
			char  xBuff[128];
			sprintf (xBuff, "==NOTE: PifGetSerialConfig() unrecognized baud, set to 19200. port %d Device %S", uchPort, pachDevice);
			NHPOS_NONASSERT_TEXT(xBuff);
		}
        break;
    }

    if (PifGetParameter(pachDevice, SCF_DATANAME_CHARBIT, &dwDataType, (LPVOID)&dwData, sizeof(dwData), &dwNoBytesRead) != SCF_SUCCESS)
    {
        if (usDefault == FALSE) {   /* default parameter is not used */
            return FALSE;
        }
    }

    switch (dwData)
    {
    case SCF_CAPS_CHAR_8:
        SysConfig.auchComByteFormat[uchPort] = COM_BYTE_8_BITS_DATA;
        break;
    case SCF_CAPS_CHAR_7:
        SysConfig.auchComByteFormat[uchPort] = COM_BYTE_7_BITS_DATA;
        break;
    default:
        SysConfig.auchComByteFormat[uchPort] = COM_BYTE_8_BITS_DATA;
        break;
    }

    if (PifGetParameter(pachDevice, SCF_DATANAME_PARITY, &dwDataType, (LPVOID)&dwData, sizeof(dwData), &dwNoBytesRead) != SCF_SUCCESS)
    {
        if (usDefault == FALSE) {   /* default parameter is not used */
            return FALSE;
        }
    }

    switch (dwData)
    {
    case  SCF_CAPS_PARITY_EVEN:
        SysConfig.auchComByteFormat[uchPort] |= COM_BYTE_EVEN_PARITY;
        break;
    case  SCF_CAPS_PARITY_ODD:
        SysConfig.auchComByteFormat[uchPort] |= COM_BYTE_ODD_PARITY;
        break;
    case  SCF_CAPS_PARITY_NONE:
    case  SCF_CAPS_PARITY_MARK:
    case  SCF_CAPS_PARITY_SPACE:
    default:
        break;
    }

    if (PifGetParameter(pachDevice, SCF_DATANAME_STOPBIT, &dwDataType, (LPVOID)&dwData, sizeof(dwData), &dwNoBytesRead) != SCF_SUCCESS)
    {
        if (usDefault == FALSE) {   /* default parameter is not used */
            return FALSE;
        }
    }

    switch (dwData)
    {
    case SCF_CAPS_STOP_2:
        SysConfig.auchComByteFormat[uchPort] |= COM_BYTE_2_STOP_BITS;
        break;
    case SCF_CAPS_STOP_1:
    default:
        break;
    }

    SysConfig.auchComPort[uchPort] = uchDeviceId;

	if(PifGetParameter(pachDevice, SCF_DATANAME_HANDSHAKE, &dwDataType, (LPVOID)&dwData, sizeof(dwData), &dwNoBytesRead) != SCF_SUCCESS)
	{
		if(usDefault == FALSE) {
			return FALSE;
		}
	}

	//new statement to put the handshake option of serial connections
	//into the system configuration structure. JHHJ 9-13-05
	switch (dwData)
	{
	case SCF_CAPS_HANDSHAKE_NONE:
		SysConfig.auchComHandShakePro[uchPort] |= COM_BYTE_HANDSHAKE_NONE;
		break;
	case SCF_CAPS_HANDSHAKE_RTSCTS:
		SysConfig.auchComHandShakePro[uchPort] |= COM_BYTE_HANDSHAKE_RTSCTS;
		break;
	case SCF_CAPS_HANDSHAKE_CTS:
		SysConfig.auchComHandShakePro[uchPort] |= COM_BYTE_HANDSHAKE_CTS;
		break;
	case SCF_CAPS_HANDSHAKE_RTS:  
		SysConfig.auchComHandShakePro[uchPort] |= COM_BYTE_HANDSHAKE_RTS;
		break;
	case SCF_CAPS_HANDSHAKE_XONOFF:
		SysConfig.auchComHandShakePro[uchPort] |= COM_BYTE_HANDSHAKE_XONOFF;
		break;
	case SCF_CAPS_HANDHSAKE_DTRDSR:
		SysConfig.auchComHandShakePro[uchPort] |= COM_BYTE_HANDSHAKE_DTRDSR;
		break;
	default:
		break;
	}
	
	{
		char  xBuff[128];

		sprintf (xBuff, "==PROVISIONING: COM device %d using port %d baud %d no. data %d", uchPort, SysConfig.auchComPort[uchPort],
			SysConfig.ausComBaud[uchPort], SysConfig.auchComByteFormat[uchPort]);
		NHPOS_NONASSERT_NOTE("==PROVISIONING", xBuff);
	}
	return TRUE;
}

/*
*******************************************************************************
*
**Synopsis:     BOOL PifGetLanConfig(PSERIAL_CONFIG pSerialConf)
*
*               input   USHORT usErrorCode;  : Error Code
*
**Return:       error code
*
**Description:  Notify error code to registered callback address
*
*               NOTE:  Should be similar to CScfInterface::PifGetLanConfig().
*
*******************************************************************************
*/
static USHORT PifGetLanConfig(TCHAR *pachDevice, UCHAR uchDeviceId, USHORT usDefault, UCHAR *ucIP, USHORT *usPort, USHORT *usStoreForward)
{
	TCHAR   *pNxt = NULL;
    DWORD   dwDataType, dwDummy;
    TCHAR   atchPort[SCF_USER_BUFFER] = {0};

	memset (ucIP, 0, 4);
	*usPort = 0;
    if (PifGetParameter(pachDevice, SCF_DATANAME_PORT, &dwDataType, (LPVOID)atchPort, sizeof(atchPort), &dwDummy) == SCF_SUCCESS)
    {
		*usPort = _ttoi (atchPort);
    }

    if (PifGetParameter(pachDevice, _T("IPaddr"), &dwDataType, (LPVOID)atchPort, sizeof(atchPort), &dwDummy) == SCF_SUCCESS)
    {
		int     i = 0;
		// convert the IP address to binary.
		// We assume it is in some kind of nnn.nnn.nnn.nnn format
		// first of all set to all binary zeros
		pNxt = atchPort - 1;
		for (i = 0; pNxt && i < 4; i++) {
			pNxt++;
			ucIP[i] = _ttoi (pNxt);
			pNxt = _tcschr (pNxt, _T('.'));
		}
    }
	
    return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
//
// Function:
//
// Prototype:   DWORD       CDevice::GetParameter(lpszDeviceName, lpszDataName,
//                              lpDataType, lpData, dwNumberOfBytesToRead,
//                              lpNumberOfBytesRead);
//
// Inputs:      LPCTSTR     lpszDeviceName;         -
//              LPCTSTR     lpszDataName;           -
//              LPDWORD     lpDataType;             -
//              LPVOID      lpData;                 -
//              DWORD       dwNumberOfBytesToRead;  -
//              LPDWORD     lpNumberOfBytesRead;    -
//
// Outputs:     DWORD       dwResult;               -
//
/////////////////////////////////////////////////////////////////////////////
static DWORD PifGetParameter(LPCTSTR lpszDeviceName, LPCTSTR lpszDataName,
                                LPDWORD lpDataType, LPVOID lpData, DWORD dwNumberOfBytesToRead,
                                LPDWORD lpNumberOfBytesRead)
{
    DWORD   dwResult;
    BOOL    bConverted;
	TCHAR   atchBuffer[SCF_USER_BUFFER] = {0};

    UNREFERENCED_PARAMETER(dwNumberOfBytesToRead);

    bConverted = FALSE;

    dwResult = ScfGetParameter(lpszDeviceName, lpszDataName, lpDataType, (LPVOID)atchBuffer, sizeof(atchBuffer), lpNumberOfBytesRead);
    if (dwResult != SCF_SUCCESS)
    {
        return(dwResult);
    }

    if (*lpDataType == SCF_DATA_STRING)
    {
        bConverted = ScfConvertString2Bit(lpszDataName, (LPDWORD)lpData, atchBuffer);
    }

    if (bConverted)
    {
        *lpDataType = SCF_DATA_BINARY;
        *lpNumberOfBytesRead = sizeof(DWORD);
    }
    else
    {
        memcpy(lpData, (LPVOID)atchBuffer, *lpNumberOfBytesRead);
    }

    return (dwResult);
}


/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifSetModemStrings(pszModemString)    **
**              pszModemString: New modem string to set             **
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:                                                    **
**                                                                  **
**********************************************************************
fhfh*/

SHORT  PIFENTRY PifSetModemStrings(CONST TCHAR FAR *pszModemString)
{
    LONG   lRet;
    HKEY   hKey;
    DWORD  dwDisposition;
    SHORT  sReturn = PIF_ERROR_SYSTEM;
    TCHAR  wszKey[128];

    wsprintf(wszKey, TEXT("%s\\%s"), PIFROOTKEY, NETWORKKEY);
    lRet = RegCreateKeyEx(HKEY_CURRENT_USER, wszKey, 0, _T(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
    if (lRet == ERROR_SUCCESS) {
		DWORD  dwBytes;
		TCHAR  wszData[PIF_LEN_M0DEM_STR + 1] = {0};

		_tcsncpy(wszData, pszModemString, PIF_LEN_M0DEM_STR);
        dwBytes = ((wcslen(wszData)+1) * sizeof(wszData[0]));    // include end of string terminator in data stored.

        lRet = RegSetValueEx(hKey, MODEMSTRINGS, 0, REG_SZ, (const UCHAR *)&wszData[0], dwBytes);

        RegCloseKey(hKey);
    }

    if (lRet == ERROR_SUCCESS) {
        PifSaveRegistry();
        sReturn = PIF_OK;
    }

    return sReturn;
}

SHORT PIFENTRY PifSetKeyboardType (USHORT usKeyBoardType)
{
    TCHAR   *pDeviceName;
    TCHAR   *pDllName;
    DWORD   dwRet;
	SHORT   sRet;
    TCHAR   atchDeviceName[SCF_USER_BUFFER] = {0};
    TCHAR   atchDllName[SCF_USER_BUFFER] = {0};

    pDeviceName = atchDeviceName;
    pDllName    = atchDllName;

	_tcscpy (pDllName, _T("Dummy.dll"));

	if (usKeyBoardType == KEYBOARD_MICRO)
		_tcscpy (pDeviceName, KEYBOARD_MICRO_NAME);
	else if (usKeyBoardType == KEYBOARD_CONVENTION)
		_tcscpy (pDeviceName, KEYBOARD_CONV_NAME);
	else if (usKeyBoardType == KEYBOARD_WEDGE_68)
		_tcscpy (pDeviceName, KEYBOARD_WEDGE_68_NAME);
	else if (usKeyBoardType == KEYBOARD_TOUCH)
		_tcscpy (pDeviceName, KEYBOARD_TOUCH_NAME);
	else if (usKeyBoardType == KEYBOARD_WEDGE_78)
		_tcscpy (pDeviceName, KEYBOARD_WEDGE_78_NAME);
	else
		return SCF_ERROR;

    dwRet = ScfResetActiveDeviceAll(SCF_TYPENAME_KEYBOARD);
    dwRet = ScfSetActiveDevice(SCF_TYPENAME_KEYBOARD, pDeviceName, pDllName);

	if (dwRet == SCF_SUCCESS)
		sRet = 0;
	else
		sRet = -1;

	return sRet;
}

/*
 *  The function PifOverrideHostName() provides a way to bypass the use of the PC name (host name)
 *  for identifying this terminal. The intent of this is to allow for the use of GenPOS on a
 *  mobile device as a Satellite Terminal and being able to easily change the Cluster Name
 *  and the terminal number.  This will probably be suitable only for a Satellite Terminal.
 *
 *  See also usage of the following defines for Windows Registry keys used in DeviceConfig:
 *     SCF_TYPENAME_OVERRIDENAME_SAT
 *     SCF_DEVICENAME_OVERRIDE_HOSTNAME
 */
static  CHAR  szPifOverrideHostName[32] = {0};

SHORT  PIFENTRY PifOverrideHostName(TCHAR *pszHostName)
{
	int    i;
	SHORT  sRetStatus = (szPifOverrideHostName[0]) ? 1 : 0;

	memset (szPifOverrideHostName, 0, sizeof(szPifOverrideHostName));
	if (pszHostName && pszHostName[0]) {
		// if new name specified then use it otherwise calling PifOverrideHostName()
		// with a NULL pointer will just clear the current Override Host Name if any.
		NHPOS_NONASSERT_NOTE("==STATUS", "==STATUS: PifOverrideHostName() called.");
		for (i = 0; i < 30; i++) {
			szPifOverrideHostName[i] = pszHostName[i];
			if (!pszHostName[i]) { break; }
		}
	}
	return sRetStatus;
}

SHORT  PIFENTRY PifGetLocalHostName(TCHAR *pszHostName)
{
    SHORT  sReturn = PIF_ERROR_SYSTEM;
	TCHAR  wszData[PIF_LEN_HOST_NAME + 1] = { 0 };
	CHAR   szData[PIF_LEN_HOST_NAME * 2 + 1] = { 0 };

    if (pszHostName == NULL) {
        return PIF_ERROR_SYSTEM;
    }

	if (szPifOverrideHostName[0]) {
		strcpy (szData, szPifOverrideHostName);
	} else {
		WSASetLastError (0);     // clear any existing error code in Windows Sockets.
		if (SOCKET_ERROR == gethostname(szData, PIF_LEN_HOST_NAME * 2)) {
			char   xBuff[128];
			DWORD  dwError = WSAGetLastError();
			PifLog(MODULE_PIF_NETOPEN, LOG_ERROR_PIFNET_HOSTLOOKUP);
			PifLog(MODULE_ERROR_NO(MODULE_PIF_NETOPEN), (USHORT)dwError);
			sprintf (xBuff, "==NOTE: PifGetLocalHostName() SOCKET_ERROR dwError = %d", dwError);
			NHPOS_NONASSERT_TEXT(xBuff);
		}
	}

	mbstowcs(wszData, szData, PIF_LEN_HOST_NAME);
	wszData[PIF_LEN_HOST_NAME] = _T('\0');
	_tcscpy(pszHostName, wszData);
	sReturn = PIF_OK;

    return sReturn;
}

UCHAR * PIFENTRY PifGetHostAddrByName (CONST TCHAR *pszHostName, UCHAR *puchHostAddr)
{
	CHAR aszHostName[PIF_LEN_HOST_NAME * 2 + 2] = {0};
	HOSTENT  *hp;

	wcstombs(aszHostName, pszHostName, PIF_LEN_HOST_NAME * 2);

	// WARNING: gethostbyname() may fail if a terminal has been turned off for several minutes
	//          or if after being off for some time, the terminal has just been turned on.
	//          there is a chance the host name information in the directory may have gone
	//          stale due to lack of communication with a terminal and been discarded.
    hp = gethostbyname(aszHostName);
	if (0 == hp) {
		char   xBuff[128];
        DWORD  dwError = WSAGetLastError();
		PifLog(MODULE_PIF_NETOPEN, LOG_ERROR_PIFNET_HOSTLOOKUP);
		PifLog(MODULE_ERROR_NO(MODULE_PIF_NETOPEN), (USHORT)dwError);
		sprintf (xBuff, "==NOTE: PifGetHostAddrByName() SOCKET_ERROR dwError = %d", dwError);
		NHPOS_NONASSERT_TEXT(xBuff);
		puchHostAddr = 0;
	}
	else {
		memcpy(puchHostAddr, hp->h_addr, hp->h_length);
	}

	return puchHostAddr;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifGetHostAddress(                    **
**                                    CONST CHAR FAR *pszHostName,  **
**                                    CHAR FAR *pszIPAddress)       **
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:                                                    **
**                                                                  **
**********************************************************************
fhfh*/
SHORT  PIFENTRY PifGetHostsAddress(CONST TCHAR *pszHostName, CHAR *pszIPAddress)
{
    LONG   lRet;
    HKEY hKey;
//    HANDLE hKey;
    TCHAR  wszKey[256];
    CHAR  szData[8];
    DWORD  dwDisposition, dwBytes, dwType;
    SHORT  sReturn = PIF_ERROR_SYSTEM;

    memset(pszIPAddress, 0, PIF_LEN_IP);
    wsprintf(wszKey, TEXT("%s\\%s\\%s"), PIFROOTKEY, HOSTSKEY, pszHostName);
    lRet = RegCreateKeyEx(HKEY_CURRENT_USER, wszKey, 0, _T(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);

    if (lRet == ERROR_SUCCESS) {
        dwBytes = sizeof(szData);

        lRet = RegQueryValueEx(hKey, IPADDRESS, 0, &dwType, (UCHAR*)&szData[0], &dwBytes);

        if (lRet == ERROR_SUCCESS) {
            memcpy(pszIPAddress, szData, PIF_LEN_IP);
            sReturn = PIF_OK;
        }

        RegCloseKey(hKey);

        if (lRet != ERROR_SUCCESS) {
            lRet = RegDeleteKey(HKEY_CURRENT_USER, wszKey);     // address of name of value to query
        }
    }

    return sReturn;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifSetHostAddress(                    **
**                                 CONST CHAR FAR *pszHostName,     **
**                                 CONST CHAR FAR *pszIPAddress)    **
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:                                                    **
**                                                                  **
**********************************************************************
fhfh*/
SHORT  PIFENTRY PifSetHostsAddress(CONST TCHAR *pszHostName, CONST CHAR *pszIPAddress)
{
    LONG   lRet;
    HKEY hKey;
//    HANDLE hKey;
    TCHAR  wszKey[256];
    CHAR   szData[8];
    DWORD  dwDisposition, dwBytes;
    SHORT  sReturn = PIF_ERROR_SYSTEM;

    if (pszHostName == NULL) {
        return PIF_ERROR_SYSTEM;
    }

    wsprintf(wszKey, TEXT("%s\\%s\\%s"), PIFROOTKEY, HOSTSKEY, pszHostName);
    lRet = RegCreateKeyEx(HKEY_CURRENT_USER, wszKey, 0, _T(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);   // address of disposition value buffer

    if (lRet == ERROR_SUCCESS) {
        dwBytes = PIF_LEN_IP;
        lRet = RegSetValueEx(hKey, IPADDRESS, 0, REG_BINARY, (const UCHAR*)pszIPAddress, dwBytes);

        dwBytes = sizeof(WORD);

        if (lRet == ERROR_SUCCESS) {
            sReturn = PIF_OK;
        }
        memset(szData, 0, sizeof(szData));
        lRet = RegSetValueEx(hKey, ALIASES, 0, REG_SZ, (const UCHAR*)&szData[0], dwBytes);

        RegCloseKey(hKey);
        /* V1.0.04, for ip broadcast */
        /* PifSaveRegistry(); */
    }

    return sReturn;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifRemoveHostAddress(                 **
**						CONST CHAR FAR *pszHostName,                **
**						CONST CHAR FAR *pszIPAddress)               **
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:                                                    **
**                                                                  **
**********************************************************************
fhfh*/
VOID  PIFENTRY PifRemoveHostsAddress(CONST TCHAR FAR *pszHostName)
{
    LONG   lRet;
    TCHAR  wszKey[256];

    if (pszHostName == NULL) {
        return;
    }

    wsprintf(wszKey, TEXT("%s\\%s\\%s"), PIFROOTKEY, HOSTSKEY, pszHostName);
    lRet = RegDeleteKey(HKEY_CURRENT_USER, &wszKey[0]);

    if (lRet == ERROR_SUCCESS) {
        /* V1.0.04, for ip broadcast */
        /* PifSaveRegistry(); */
    }
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifSetHostAddress(CONST CHAR *pszHostName, CONST CHAR *pszIPAddress) **
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:                                                    **
**                                                                  **
**********************************************************************
fhfh*/

SHORT  PIFENTRY PifSetServicesPortNo(USHORT usPortNo)
{
    LONG   lRet;
    HKEY hKey;
//    HANDLE hKey;
    TCHAR  wszKey[256];
    TCHAR  wszData[8];
    DWORD  dwDisposition, dwBytes;
    SHORT  sReturn = PIF_ERROR_SYSTEM;

    wsprintf(wszKey, TEXT("%s\\%s"), PIFROOTKEY, NETWORKKEY);
    lRet = RegCreateKeyEx(HKEY_CURRENT_USER, wszKey, 0, _T(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
    if (lRet == ERROR_SUCCESS) {

        if (usPortNo == 0) {
            lRet = RegDeleteValue(hKey, PORT);
        } else {
            wsprintf(wszData, TEXT("%d"), usPortNo);
            dwBytes = sizeof(wszData);

            lRet = RegSetValueEx(hKey, PORT, 0, REG_SZ, (const UCHAR *)&wszData[0], dwBytes);
        }

        RegCloseKey(hKey);
    }

    if (lRet == ERROR_SUCCESS) {
        sReturn = PIF_OK;
        PifSaveRegistry();
    }

    return sReturn;
}

struct _tagDeviceTable {
	USHORT usDevice;
	TCHAR *szRegSymbol;
} stagDeviceTable [] = {
		{DEVICE_PC, NULL },
		{DEVICE_SLIP_PRINTER, SCF_TYPENAME_PRINTER},
		{DEVICE_KITCHEN_PRINTER, SCF_TYPENAME_KITCHEN_PRINTER},
		{DEVICE_SCANNER, SCF_TYPENAME_SCANNER},
		{DEVICE_SCALE, SCF_TYPENAME_SCALE},
		{DEVICE_PREPAY_CARD, NULL},
		{DEVICE_COIN_DISPENSER, SCF_TYPENAME_COIN},
		{DEVICE_KITCHEN_CRT, SCF_TYPENAME_KITCHEN_CRT},
		{DEVICE_CHARGE_POSTING, NULL },
		{DEVICE_CHARGE_POSTINGLAN, NULL },
		{DEVICE_EPT, NULL },
		{DEVICE_EPT_EX, NULL },
		{DEVICE_EPT_EX_LAN, SCF_TYPENAME_EPT_EX_LAN},
		{DEVICE_THERMAL_PRINTER, SCF_TYPENAME_PRINTER},
		{DEVICE_BEVERAGE, SCF_TYPENAME_BEVERAGE},
		{DEVICE_NEW_CHARGE_POSTING, NULL},
		{DEVICE_410_COIN_DISPENSER, NULL},
		{DEVICE_SCANNER_SCALE, SCF_TYPENAME_SCANNER},
		{DEVICE_ODISPLAY, SCF_TYPENAME_ODISPLAY},
		{DEVICE_CDISPLAY, SCF_TYPENAME_CDISPLAY},
		{DEVICE_RJ_PRINTER, SCF_TYPENAME_PRINTER},
		// DEVICE_NONE indicates end of the table. Any new entries should be before this.
		{DEVICE_NONE, NULL}
	};

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PIFENTRY PifGetActiveDevice(
								USHORT usDeviceId,
								USHORT usPortNo,
								TCHAR  *pszDeviceName,
								TCHAR  *pszDllFileName)

**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:                                                    **
**                                                                  **
**********************************************************************
fhfh*/
SHORT PIFENTRY PifGetActiveDevice(
    USHORT usDeviceId,
    USHORT usPortNo,
    TCHAR  *pszDeviceName,
    TCHAR  *pszDllFileName)
{

	struct _tagDeviceTable *pDevTabl;
    DWORD  dwCount = 0, dwRet, dwDataType, dwDummy;
    SHORT  sReturn = PIF_ERROR_SYSTEM;
    TCHAR  wszScfDeviceName[SCF_USER_BUFFER] = {0};
    TCHAR  wszDeviceName[SCF_USER_BUFFER] = {0}, *pDeviceName;
	TCHAR  wszDllName[SCF_USER_BUFFER] = {0};
    TCHAR   atchPort[SCF_USER_BUFFER] = {0};
    UCHAR   uchPort=0;

	for (pDevTabl = stagDeviceTable; pDevTabl->usDevice != DEVICE_NONE; pDevTabl++) {
		if (pDevTabl->usDevice == usDeviceId)
			break;
	}

	if (pDevTabl->szRegSymbol == NULL)
		return sReturn;

	wsprintf(wszScfDeviceName, pDevTabl->szRegSymbol);

	memset(wszDeviceName, 0, sizeof(wszDeviceName));
    dwRet = ScfGetActiveDevice(wszScfDeviceName, &dwCount, wszDeviceName, wszDllName);

    pDeviceName = &wszDeviceName[0];

    if ((dwRet == SCF_SUCCESS) && (dwCount)) {
		DWORD i;
        for (i=0; i<dwCount; i++) {
            if (PifGetParameter(pDeviceName, _T("Port"), &dwDataType, (LPVOID)atchPort, sizeof(atchPort), &dwDummy) == SCF_SUCCESS)
            {
				uchPort = _ttoi (atchPort);
                if (uchPort == usPortNo) {
					wcsncpy(pszDeviceName, pDeviceName, SCF_MAX_NAME);
					wcsncpy(pszDllFileName, wszDllName, SCF_MAX_NAME);
                    sReturn = PIF_OK;
                    break;
                }
            }

            // move pointer
            pDeviceName += wcslen(pDeviceName) + 1;
        }
    }

    return sReturn;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifSetHostAddress(CONST CHAR FAR *pszHostName, CONST CHAR FAR *pszIPAddress) **
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:                                                    **
**                                                                  **
**********************************************************************
fhfh*/

SHORT PIFENTRY PifGetSystemVersion(
    TCHAR  *pInfoBuffer)
{
    //CHAR    szData[PIF_LEN_VERSION];
    SHORT sReturn = PIF_ERROR_SYSTEM;
#if !defined(POSSIBLE_DEAD_CODE)
	_tcscpy(pInfoBuffer, _T("Windows\n"));
#else
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;
	DWORD viSize = 0;

	   // Try calling GetVersionEx using the OSVERSIONINFOEX structure.
   //
   // If that fails, try using the OSVERSIONINFO structure.
//	_tcscat(pInfoBuffer, _T("OS Version:\t"));

   ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
   osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

   if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
   {
      // If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO.
      osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
      if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) )
         return FALSE;
   }

   //we use this to find out the correct Operating System, these defines are listed
   //in the MSDN information for the OSVERSIONINFO structure
   switch( osvi.dwMajorVersion)
   {
	 case 3:
	    _tcscat(pInfoBuffer, _T("Windows NT 3.51\n")); //i doubt anyone is using this, but we have it in case.
	    break;

	 case 4:
		   switch(osvi.dwMinorVersion)
		   {
			   case 0:
				  _tcscat(pInfoBuffer, _T("Windows 95\n"));
				   break;
			   case 10:
				 _tcscat(pInfoBuffer, _T("Windows 98\n"));
				   break;
			   case 90:
				 _tcscat(pInfoBuffer, _T("Windows ME\n"));
				   break;
			   default:
				   break;
		   }
		break;

	 case 5:
		   switch(osvi.dwMinorVersion)
		   {
			   case 0:
				  _tcscat(pInfoBuffer, _T("Windows 2000\n"));
				   break;
			   case 1:
				 _tcscat(pInfoBuffer, _T("Windows XP\n"));
				   break;
			   case 2:
				 _tcscat(pInfoBuffer, _T("Windows 2003\n"));
				   break;
			   default:
				   break;
		   }
		   break;

	 case 6:
		   switch(osvi.dwMinorVersion)
		   {
			   case 0:
				  _tcscat(pInfoBuffer, _T("Windows Vista\n"));
				   break;
			   case 1:
				  _tcscat(pInfoBuffer, _T("Windows 7\n"));
				   break;
			   case 2:
				  _tcscat(pInfoBuffer, _T("Windows 8\n"));
				   break;
			   case 3:
				  _tcscat(pInfoBuffer, _T("Windows 8.1\n"));
				   break;
			   default:
				   break;
		   }
		   break;

	 default:
			break;
	}

   {
	   char xBuff[128];
	   sprintf (xBuff, "==NOTE: OS version Major %d Minor %d Build %d Product %d", osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber, osvi.dwPlatformId);
	   NHPOS_NONASSERT_TEXT(xBuff);
   }
#endif
	sReturn = PIF_OK;
    return sReturn;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifSetHostAddress(CONST CHAR FAR *pszHostName, CONST CHAR FAR *pszIPAddress) **
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:                                                    **
**                                                                  **
**********************************************************************
fhfh*/
#define BUFFER              1024

SHORT PIFENTRY PifGetVersion(TCHAR  *pInfoBuffer)
{
    SHORT sReturn = PIF_ERROR_SYSTEM;
	DWORD dwHandle;
	DWORD viSize = 0;
	TCHAR infobuffer [1028];
	//TCHAR			infoBuffer2[1028];
	VS_FIXEDFILEINFO *lpBuf;
	unsigned int puLen;
	WIN32_FIND_DATA myFoundFile;
	HANDLE fileSearch;
	BOOL doit = TRUE;
	TCHAR		versionNumber[40];

	//this is the first type of file that we are
	//looking for, executables.
	TCHAR *myFileName = _T("*.exe");
	memset(&infobuffer, 0x00, sizeof(infobuffer));
	memset(versionNumber, 0x00, sizeof(versionNumber));

	//we need to change to this directory to look for the files
	_tchdir(STD_FOLDER_PROGRAMFILES);

	fileSearch = FindFirstFile (myFileName, &myFoundFile); //find the first file

	//this while loop will continue as long as there are .exe in the directory
	while (doit && fileSearch != INVALID_HANDLE_VALUE)
	{
		//clean the buffer so we dont get erroneous data
		memset(&versionNumber, 0x00, sizeof(versionNumber));
		//the next two functions get the file name, we must call the first one
		// in order to get the name, i dunno why, ask MSDN.
		viSize = GetFileVersionInfoSize (myFoundFile.cFileName, &dwHandle);
		GetFileVersionInfo (myFoundFile.cFileName, dwHandle, sizeof (infobuffer), infobuffer);
		VerQueryValue (infobuffer, _T("\\"), (void **)&lpBuf, &puLen);

		if(!_tcscmp(myFoundFile.cFileName, _T("Framework.exe")))
		{
			//format the information that we have just received to be printed
			_stprintf(versionNumber, _T("%2.2d.%2.2d.%2.2d.%2.2d\n"),
				HIWORD(lpBuf->dwFileVersionMS), LOWORD(lpBuf->dwFileVersionMS),
				HIWORD(lpBuf->dwFileVersionLS), LOWORD(lpBuf->dwFileVersionLS));

			//place it into the buffer
			_tcscat(pInfoBuffer, versionNumber);

			//find the next file, if there is not one, doit will be FALSE
			doit = FALSE;//FindNextFile (fileSearch, &myFoundFile);
		}

		doit = FindNextFile (fileSearch, &myFoundFile);
	}

	FindClose (fileSearch);

    return sReturn;
}

/* end of pifconf.c */
