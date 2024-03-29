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
*
*  NeighborhoodPOS Software Development Project
*  Department of Information Systems
*  College of Information Technology
*  Georgia Southern University
*  Statesboro, Georgia
*
*  Copyright 2014 Georgia Southern University Research and Services Foundation
* 
*===========================================================================
* Title       : Initialize Module
* Category    : User Interface Main, NCR 2170 US Hospitality Application
* Program Name: UIMAIN.C
* --------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
*       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
*       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
*       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
*       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
*       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
*       2016  -> GenPOS Rel 2.3.0 (Windows 7, completely Out of Scope, Visual Studio 2013.)
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
*    moved from Visual Studio 2005 to Visual Studio 2013 with Rel 2.3.0
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:
*
*               PifMain() : Initialize each Thread
*                   UifInitialize() : Create Main Application
*                   UifMain() : Main Application
*                   UifLock() : Lock Mode
* --------------------------------------------------------------------------
* Update Histories
*    Date   : Ver.Rev. :   Name       : Description
* May-14-92 : 00.00.01 : M.Suzuki     : initial
* Sep-09-93 : 02.00.00 : K.You        : bug fixed E-86 (mod. UifCreateMain)
* Mar-01-95 : 03.00.00 : hkato        : R3.0
* Mar-27-95 : 03.00.00 : M.Suzuki     : Add VosInitialize(), MldInit()
*
*** NCR2171 ***
* Oct-04-99 : 01.00.00 : M.Teraki     : Initial
*
*** GenPOS Rel 2.2.1
* Apr-09-15 : 02.02.01 : R.Chambers   : Cleanup for thread starting, localize variables
*===========================================================================
*===========================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===========================================================================

	Notes on Setting up and Using Automation Testing
	------------------------------------------------
	On setting up and using automation to run a series of terminal actions automatically.

	To use the automation functionality you must have the option turned on in the registry.

	Then you must record the key sequence of actions that you want to run automatically.

	Setting up the Registery for automation.
	In the registry, 
      The registry key is HKEY_LOCAL_MACHINE\SOFTWARE\NCR\SARATOGA\Pif\Option
      Add a new Dword Key labeled "Option8", with the Decimal value of 2172.

	To run automation.
	  Start the recording by entering a delay in seconds followed by pressing the receipt feed button.
	     The delay time in seconds is 4 digits as in 0005 for 5 seconds or 0010 for ten seconds.
		 The delay is the time spent waiting before repeating the automation sequence.
		 You should hear a couple of beeps indicating you are in record mode.
		     If you don't hear the beeps, check that the registry settings exist.

	  Enter the key strokes or button presses that you want to record.
	     Automation will record on key strokes or button presses that haven FSC associated with them.
		 Automation will only work for BusinessLogic type FSC processing.
		 Automation does not drive the actual windowing interface so can not be used for some buttons.

	  End the recording by pressing 00 and the journal feed button.
	     You should hear a couple of beeps indication you are out of record mode and in run mode.

	  To stop the automation run, press the Clear key or button and it will stop momentarily.

	Some considerations.
	  Turn off receipt printing or you will generate lots of paper.
	  Set EJ to override so as to eliminate possibilitye of EJ Full.
	  Any error dialogs will stop automation until the dialog is acknowledged and closed.
	  The fastest usable rate is about 5 seconds.  10 seconds is most commonly used however.

*/

// required to allow VOID THREADENTRY UifWatchPrintFilesFolder (VOID) use ReadDirectoryChangesW()
// this define will exclude this application from running on Windows 95/98/ME.  Oh well not a problem.
#define _WIN32_WINNT 0x0400

#include <windows.h>
#include <tchar.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "ecr.h"
#include "pif.h"
#include "Evs.h"
#include "log.h"
#include "uie.h"
#include "rfl.h"
#include "fsc.h"
#include "plu.h"
#include "applini.h"
#include "paraequ.h"
#include "para.h"
#include "pararam.h"
#include "csstbstb.h"
#include "csstbpar.h"
#include "pmg.h"
#include "display.h"
#include "uireg.h"
#include "uifsup.h"
#include "uifprog.h"
#include "cvt.h"
#include "regstrct.h"
#include "transact.h"
#include "item.h"
#include "ej.h"
#include "csstbej.h"
#include "pifmain.h"
#include "appllog.h"
#include "vos.h"
#include "mld.h"
#include "ttl.h"
#include "uifpgequ.h"
#include "cscas.h"
#include "csstbcas.h"
#include "scf.h"
#include "httpmain.h"
#include    "csetk.h"
#include    "..\Item\Include\itmlocal.h"
#define __BLNOTIFY_C__
#include "bl.h"
#include "uie.h"
#include "BlFWif.h"
#include <maint.h>
#include <prt.h>
#include <uieerror.h>
#include <EEPTL.H>

#if defined(ENABLE_LUA_SCRIPTING)
// If the Lua virtual machine is being used then we need the following includes.
// Lua is a script language we have considered including into GenPOS Rel 2.2
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#endif

/*
*===========================================================================
*   External Work Area Declarations
*===========================================================================
*/
VOID THREADENTRY UifMsrDeviceThread (VOID);
VOID THREADENTRY UifPinPadDeviceThread (VOID);
VOID THREADENTRY UifSignatureCaptureDeviceThread (VOID);
VOID THREADENTRY UifSharedPrintFileThread (VOID);
VOID THREADENTRY UifSharedLuaFileThread (VOID);

USHORT usUifPowerUp = 0;                                  /* Power Up Mode Save Area */

/* moved to start each thread after each file creation, 08/31/01 */

static SHORT  sUifQueueMsrDevice = -1;
static SHORT  sUifQueuePinPadDevice = -1;
static SHORT  sUifQueueSharedLuaFile = -1;
static SHORT  sUifQueueSharedPrintFile = -1;
static SHORT  sUifQueueSignatureCaptureDevice = -1;

#define UIF_PRINT_QUEUE_MAX    160

/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/

static UIMENU CONST aUifReg[] = {{UifReg, CID_REG}, {NULL, 0}};
static UIMENU CONST aUifSuper[] = {{UifACMode, CID_ACMODE}, {NULL, 0}};
static UIMENU CONST aUifProg[] = {{UifPGMode, CID_PGMODE}, {NULL, 0}};
static UIMENU CONST aUifLock[]  = {{UifLock,  CID_LOCK},  {NULL, 0}};

// NOTE: These tables determine if function UiePutKeyData() will actually put the keystrokes into
//       the ring buffer.  If, when adding a new keyboard, you have a NULL rather than a pointer
//       to a conversion table, then UiePutKeyData() will throw away the keypress.  The behavior
//       seen will be a key change indicated in the lead thru area but when trying to use the new
//       mode, key presses will not work.
static CONST CVTTBL * pUifCvtM[UIF_MAX_MODE] = {NULL, NULL, NULL, CvtRegM, CvtRegM, CvtProgM};
static CONST CVTTBL * pUifCvtC[UIF_MAX_MODE] = {NULL, NULL, NULL, CvtRegC, CvtRegC, CvtProgC};
static CONST CVTTBL * pUifCvtTOUCH[UIF_MAX_MODE] = {NULL, NULL, NULL, CvtRegTouch, CvtRegTouch, CvtProgTouch};
static CONST CVTTBL * pUifCvtW68[UIF_MAX_MODE] = {NULL, NULL, NULL, CvtReg5932_68, CvtReg5932_68, CvtProg5932_68};
static CONST CVTTBL * pUifCvtW78[UIF_MAX_MODE] = {NULL, NULL, NULL, CvtReg5932_78, CvtReg5932_78, CvtProg5932_78};
static CONST CVTTBL * pUifCvtBigTicket[UIF_MAX_MODE] = {NULL, NULL, NULL, CvtRegTicket, CvtRegTicket, CvtRegTicket};
static CONST FSCTBL * pUifFsc[UIF_MAX_MODE] = {NULL, NULL, NULL, (FSCTBL *)ParaFSC, (FSCTBL *)ParaFSC, NULL};

static const TCHAR aUifInitialResetPrint[] = _T(" ***INITIAL RESET***");
static const TCHAR aUifLoaderPrint[] = _T("  ***LOAD LOADER***");
static const TCHAR aUifTotalResetPrint[] = _T("  ***CLEAR TOTAL***");
static const TCHAR aUifMasterResetPrint[] = _T("  ***MASTER RESET***");
static const  CHAR aUifMasterItemThing[] = "Rel 2.1.0 Param;";
TCHAR CONST      aszPrintLogo[10][9] = {	_T("PARAMLO1"),
										_T("PARAMLO2"),
										_T("PARAMLO3"),
										_T("PARAMLO4"),
										_T("PARAMLO5"),
										_T("PARAMLO6"),
										_T("PARAMLO7"),
										_T("PARAMLO8"),
										_T("PARAMLO9"),
										_T("PARAMLOA")	};        /* Tmp#1 File Name */

TCHAR CONST		aszPrintLogoIndex[] = _T("PARAMLID");

//UCHAR           uchCPUnsoliStatus;                    /* CP/EPT Unsoli Status,Saratoga */
UCHAR           uchResetLog = 0;                        /* PC i/F Reset Log,    V3.3 */
UCHAR           uchPowerDownLog = 0;                    /* Power Down Log,      Saratoga */

USHORT          usBMOption = 0;        /* B-Master Option,     Saratoga, default is no Backup */

USHORT          semStoreForward = -1;    ///Store and Forwand Semaphore

static  PifSemHandle   husSharedPrintFileSem = PIF_SEM_INVALID_HANDLE;
static  SHORT          sUifQueueDeviceMessageSystem = -1;

static  PifSemHandle   husSharedLuaFileSem = PIF_SEM_INVALID_HANDLE;

UifPrinterBarcodeStruct  UifPrinterBarcodeSettings = { PRT_BARCODE_CODE_CODE128C, PRT_BARCODE_TEXT_CENTER, PRT_BARCODE_TEXT_BELOW};

typedef struct {
	TCHAR  *tcsMnemonic;
	TCHAR  *tcsKeyWord;       // keyword if keyword value pair, tag if just a tag, 0 if end of the list.
	ULONG  ulValue;
} UifLineTagsMnemonic;

typedef struct {
	TCHAR                *tcsKeyWord;       // keyword if keyword value pair, tag if just a tag, 0 if end of the list.
	UifLineTagsMnemonic  *tcsValueList;    // list of possible value strings, 0 if just a tag, 1 if match anything
} UifPrintFileLineTags;


UifLineTagsMnemonic UifLineTags_barcode_halign[] = {
	{_T("left"), 0, PRT_BARCODE_TEXT_LEFT},
	{_T("center"), 0, PRT_BARCODE_TEXT_CENTER},
	{_T("right"), 0, PRT_BARCODE_TEXT_RIGHT},
	{0, 0, 0} };
UifLineTagsMnemonic UifLineTags_barcode_code[] = {
	{_T("code39"), 0, PRT_BARCODE_CODE_CODE39},
	{_T("code128"), 0, PRT_BARCODE_CODE_CODE128},
	{_T("code128C"), 0, PRT_BARCODE_CODE_CODE128C},
	{_T("code128c"), 0, PRT_BARCODE_CODE_CODE128C},
	{_T("codeean128"), 0, PRT_BARCODE_CODE_CODEEAN128},
	{_T("codeEAN128"), 0, PRT_BARCODE_CODE_CODEEAN128},
	{_T("codeupca"), 0, PRT_BARCODE_CODE_CODEUPCA},
	{_T("codeean13"), 0, PRT_BARCODE_CODE_CODEEAN13},
	{0, 0, 0} };
UifLineTagsMnemonic UifLineTags_barcode_text[] = {
	{_T("above"), 0, PRT_BARCODE_TEXT_ABOVE},
	{_T("below"), 0, PRT_BARCODE_TEXT_BELOW},
	{_T("none"), 0, PRT_BARCODE_TEXT_NONE},
	{0, 0, 0} };

UifPrintFileLineTags   UifPrintFileLineTags_barcode [] = {
	{_T("barcode"), 0},           // first entry is always the main tag.  Others are options.
	{_T("halign"), UifLineTags_barcode_halign},
	{_T("code"), UifLineTags_barcode_code},
	{_T("text"), UifLineTags_barcode_text},
	{0, 0}
};


static  VOID (PIFENTRY *pPifSaveFarData)(VOID) = PifSaveFarData; /* saratoga */
extern  TCHAR CONST     aszPrintLogo[10][9];    /* Tmp#1 File Name */
extern  TCHAR CONST		aszPrintLogoIndex[];
/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/

static TCHAR tcsFrameworkVersionNumber [FILE_LEN_ID + 1] = {0, 0};

__declspec(dllexport)
USHORT BlSetFrameworkVersionNumber (CONST TCHAR *  ptszFrameworkVersionNumber)
{
	int i = 0;

	memset (tcsFrameworkVersionNumber, 0, sizeof(tcsFrameworkVersionNumber));

	for (i = 0; i < FILE_LEN_ID && ptszFrameworkVersionNumber[i]; i++) {
		tcsFrameworkVersionNumber[i] = ptszFrameworkVersionNumber[i];
	}
	return 0;
}

//---------------------------------------------------------------
//-----   Queue Initialization functions for various threads
VOID  UifRequestMsrDeviceInit ()
{
	sUifQueueMsrDevice = PifCreateQueue(10);
}

VOID  UifRequestPinPadDeviceInit ()
{
	sUifQueuePinPadDevice = PifCreateQueue(10);
}

VOID  UifRequestSignatureCaptureDeviceInit ()
{
	sUifQueueSignatureCaptureDevice = PifCreateQueue(10);
}

VOID  UifRequestSharedPrintFileInit ()
{
	sUifQueueSharedPrintFile = PifCreateQueue(UIF_PRINT_QUEUE_MAX);
	husSharedPrintFileSem = PifCreateSem( 1 );        // enable the printing at startup
}

VOID  UifRequestSharedLuaFileInit ()
{
	sUifQueueSharedLuaFile = PifCreateQueue(30);
	husSharedLuaFileSem = PifCreateSem( 1 );        // enable the Lua engine at startup
}

VOID UifDeviceMessageSystemInit ()
{
	sUifQueueDeviceMessageSystem = PifCreateQueue(20);
}

BOOL  UifDeviceMessageSystemSendWait (ULONG ulIoctlId, VOID *pvDataArea, ULONG ulDataAreaSize)
{
	BOOL  bResult;

	bResult = BlFwIfBlInterface (ulIoctlId, pvDataArea, ulDataAreaSize);

	if (bResult)
		PifReadQueue(sUifQueueDeviceMessageSystem);

	return bResult;
}

BUSINESSLOGIC_API VOID _cdecl BlFwIfDeviceMessageSystemSendWait(VOID *pvDataArea)
{
	PifWriteQueue(sUifQueueDeviceMessageSystem, pvDataArea);
}


/*
*===========================================================================
** Synopsis:    VOID PifMain(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
**  Return:     nothing
*
** Description: Initialize each thread.
*               - Main Apllication
*               - User Interface Engine
*               - Stub
*               - Print Manager
*===========================================================================
*/
static BOOL CheckForDebugger()
{
	// See the following for a discussion on Structured Exception Handling in Windows.
	//    http://msdn.microsoft.com/en-us/library/windows/desktop/ms680657(v=vs.85).aspx
	//    http://www.codeproject.com/Articles/82701/Win32-Exceptions-OS-Level-Point-of-View
    __try 
    {
        DebugBreak();
    }
    __except(GetExceptionCode() == EXCEPTION_BREAKPOINT ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) 
    {
        // No debugger is attached, so return FALSE 
        // and continue.
        return FALSE;
    }
    return TRUE;
}

VOID PifMain(TCHAR *pVersion)
{
	int i;
    SYSCONFIG *pSysConfig;
	TERMINALINFO *pTermInfo = Para.TerminalInformation;
	PARAFSCTBL ParaTable[2];
	LONG    dwRet, dwCount;
	SHORT   sRetStatusParaLoad = -1;
    TCHAR   atchDeviceName[SCF_USER_BUFFER];
    TCHAR   atchDllName[SCF_USER_BUFFER];
    TCHAR   *pDeviceName;
    TCHAR   *pDllName;
	USHORT keyBoardTable[] = 
	{ KEYBOARD_CONVENTION,
		KEYBOARD_MICRO, 
		KEYBOARD_WEDGE_68, 
		KEYBOARD_PCSTD_102, 
		KEYBOARD_TOUCH,
		KEYBOARD_WEDGE_78,
		KEYBOARD_TICKET
	};
	/*
		We now initialize our memory resident data structures from.
		the saved state on disk.

		SysConfig is initialized by function PifInitialize() in the
		pif subsystem when the pif DLL is first attached (see DllMain in pif.c).

		At this point, the SysConfig data struct has been filled in by PifInitialize
		and we now load in the saved context from disk into Para (UNINIRAM) and then
		begin initialing ourselves and starting up the various threads that we
		use.
	 */

	{
//		BOOL bBugger = CheckForDebugger();
	}

	memcpy (&Para, &ParaDefault, sizeof(Para));
	memcpy(&ParaTable, &ParaFSC[9], sizeof(ParaTable));
    sRetStatusParaLoad = PifLoadFarData (&Para, sizeof(Para), pVersion);      /* load uninitialaize data */
	if (sRetStatusParaLoad < 0) {
		memset (&Para, 0, sizeof(Para));
		memcpy (&Para, &ParaDefault, sizeof(Para));
		memcpy(&ParaTable, &ParaFSC[9], sizeof(ParaTable));
	}

	// set the current offset for the ASSRTLOG file
	PifLogNoAbort (0, 0, 0, Para.ulAssrtlogFileOffset);

    {
		char  xBuff[128];
		sprintf (xBuff, "==NOTE:  PifMain() - startup after PifLoadFarData  sRetStatusParaLoad %d", sRetStatusParaLoad);
		NHPOS_NONASSERT_NOTE("==NOTE", xBuff);

		sprintf (xBuff, "==NOTE:  PifMain() - startup after PifLoadFarData  TtlUphdata.uchResetFlag 0x%x", TtlUphdata.uchResetFlag);
		NHPOS_NONASSERT_NOTE("==NOTE", xBuff);

		NHPOS_ASSERT_TEXT((sRetStatusParaLoad >= 0), "==WARNING: PifMain() pPifLoadFarData() reports error.  using default UNINIRAM.");

		sprintf (xBuff, "==NOTE:  PifMain() - GenPOS version %S", pVersion);
		NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
	}

	for (i = 0; i < FILE_LEN_ID; i++) {
		ParaFileVersion[i] = tcsFrameworkVersionNumber[i];
	}

	NHPOS_ASSERT_TEXT((sRetStatusParaLoad >= 0), "==WARNING: PifMain() pPifLoadFarData() reports error. using default UNINIRAM.")

	// Log the various environmental variables that we use
	{
		int    iLoop;
		TCHAR  *tchEnvVar[] = {
				_T("XEPTLOGGING"),
				_T("PIF_LOG_ON"),
				0
			};

		for (iLoop = 0; tchEnvVar[iLoop]; iLoop++) {
			DWORD  dwJJ = 0;
			TCHAR  tchEnvVal[28] = {0};
			char   xBuff[256];

			dwJJ = GetEnvironmentVariable(tchEnvVar[iLoop], tchEnvVal, 26);
			if (dwJJ > 0) {
				sprintf (xBuff, "==STATE: Env Var \"%S\" set \"%S.\"", tchEnvVar[iLoop], tchEnvVal);
			} else {
				sprintf (xBuff, "==STATE: Env Var \"%S\" NOT set.", tchEnvVar[iLoop]);
			}
			NHPOS_NONASSERT_NOTE("==STATE", xBuff);
		}
	}

	// Adjust the max number of MLD records to what ever we are expecting.
	// With US Customs Mobile Device SCER we have added new MLD mnemonics and
	// the max number of records had to be changed.
	if (ParaFlexMem[FLEX_MLD_ADR - 1].ulRecordNumber < MAX_MLDMNEMO_SIZE) {
		ParaFlexMem[FLEX_MLD_ADR - 1].ulRecordNumber = MAX_MLDMNEMO_SIZE;
	}

	memcpy(&ParaFSC[9], &ParaTable, sizeof(ParaTable));
    VosInitialize(MLD_BACKLIGHT_LIMIT);             /* Add R3.0 */

	UifDeviceMessageSystemInit();

	// because PifSysConfig returns a pointer to a const, the following
	// statement will issue a warning about assignment of a pointer to
	// a const to a non-const pointer variable.  Ignore this warning.
	//		warning C4090: '=' : different 'const' qualifiers
#pragma message(" ---  Following warning C4090 is design intent.")
    pSysConfig = PifSysConfig();                    /* get system config */


	pSysConfig->uchKeyType = KEYBOARD_TOUCH;

    pDeviceName = atchDeviceName;
    pDllName    = atchDllName;

    dwRet = ScfGetActiveDevice(SCF_TYPENAME_KEYBOARD, &dwCount, pDeviceName, pDllName);
    if ((dwRet == SCF_SUCCESS) && (dwCount)) {
		if (wcscmp (KEYBOARD_MICRO_NAME, pDeviceName) == 0) 
			pSysConfig->uchKeyType = KEYBOARD_MICRO;
		else if (wcscmp (KEYBOARD_CONV_NAME, pDeviceName) == 0) 
			pSysConfig->uchKeyType = KEYBOARD_CONVENTION;
		else if (wcscmp (KEYBOARD_WEDGE_68_NAME, pDeviceName) == 0) 
			pSysConfig->uchKeyType = KEYBOARD_WEDGE_68;
		else if (wcscmp (KEYBOARD_NONE_NAME, pDeviceName) == 0) 
			pSysConfig->uchKeyType = KEYBOARD_NONE;
		else if (wcscmp (KEYBOARD_TOUCH_NAME, pDeviceName) == 0) 
			pSysConfig->uchKeyType = KEYBOARD_TOUCH;
		else
		{
			USHORT usTerminalPosition = PIF_CLUSTER_MASK_TERMINAL_NO(pSysConfig->usTerminalPositionFromName);

			if (usTerminalPosition < 1 || usTerminalPosition > PIF_NET_MAX_IP) {
				usTerminalPosition = 1;
			}

			pSysConfig->uchKeyType =  Para.TerminalInformation[usTerminalPosition - 1].uchKeyBoardType;
		}
	}

	/*
		Setup and initialize the correct keyboard type.  We support the following
		configurations and keyboards:
			NCR 7448 Terminal with the Conventional keyboard
			NCR 7448 Terminal with the Micromotion keyboard
			NCR Terminal with the NCR 5932 Wedge keyboard with 68 keys
			NCR Touchscreen terminal with standard 102 keyboard

		The initialization of SysConfig assumes a micromotion keyboard.
		Keyboard type is a terminal dependent parameter set in the terminal
		information within the UNINIRAM.

		We also set the register number and the store number to be what has
		been provisioned in UNINIRAM.

	 */

	// guard the terminal position to ensure it is in correct range, default is we be Master
	if (!(PIF_CLUSTER_MASK_TERMINAL_NO(pSysConfig->usTerminalPositionFromName) >= 1 && PIF_CLUSTER_MASK_TERMINAL_NO(pSysConfig->usTerminalPositionFromName) <= PIF_NET_MAX_IP))
		pSysConfig->usTerminalPositionFromName = 1;

	pTermInfo += PIF_CLUSTER_MASK_TERMINAL_NO(pSysConfig->usTerminalPositionFromName) - 1;

	{
		TCHAR  szHostName[PIF_LEN_HOST_NAME + 1] = {0};
		char  xBuff[128];
		sprintf (xBuff, "==NOTE: System Startup:  Terminal # %d  Terminal position 0x%4.4x  Version %S", PIF_CLUSTER_MASK_TERMINAL_NO(pSysConfig->usTerminalPositionFromName), pSysConfig->usTerminalPosition, tcsFrameworkVersionNumber);
		NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
		sprintf (xBuff, "        System provisioned with %d terminals  System Type %d.", RflGetNoTermsInCluster(), RflGetSystemType());
		NHPOS_NONASSERT_TEXT(xBuff);
		PifGetLocalHostName(szHostName);
		sprintf (xBuff, "        Host Name \"%S\".", szHostName);
		NHPOS_NONASSERT_TEXT(xBuff);
	}


	// We force the code page into SYSCONFIG from the P97 settings
	// but we force SYSCONFIG keyboard into the P97 settings in case they
	// were changed with the DeviceConfig utility.  When P97 is used to download
	// keyboard type settings, we also update SYSCONFIG with the new keyboard type.
	// See IspAfterProcess () in ispopr.c.
#pragma  message("** WARNING:  Forcing SysConfig settings to terminfo settings except keyboard type.")
	pSysConfig->unCodePage = pTermInfo->ulCodePage;
	pTermInfo->uchKeyBoardType = pSysConfig->uchKeyType;

	//we always want to start off in numeric mode
	//the other mode would be alpha enter mode
	//for US CUSTOMS JHHJ 7-25-05
	pSysConfig->uchKBMode = UIF_NEUME_KB;

	// We allow the override of the SysConfig setup data for the EPT Enhanced interface
	// for EPT Enhanced devices 5 through 15.  Device 0 through device 4 is set through DeviceConfig as
	// is the Charge Post Host manager.  Devices 1 through 4 are also known as SCF_TYPENAME_EPT_EX_DLL,
	// SCF_TYPENAME_EPT_EX_DLL1, SCF_TYPENAME_EPT_EX_DLL2, and SCF_TYPENAME_EPT_EX_DLL3 as well as
	// DEVICE_EPT_EX_DLL, DEVICE_EPT_EX_DLL1, DEVICE_EPT_EX_DLL2, and DEVICE_EPT_EX_DLL3.
	// EPT Enhanced Device 0 is used for the old charge post and lan types of EPT.
	// PIF_EPT_ENH_EX_DLL, PIF_EPT_ENH_EX_DLL1, PIF_EPT_ENH_EX_DLL2, PIF_EPT_ENH_EX_DLL3
	for (i = PIF_EPT_ENH_EX_DLLLAST; i < 16; i++) {
		pSysConfig->EPT_ENH_Info[i].usPortNo = Para.EPT_ENH_Information[i].usPortNo;
		memcpy(pSysConfig->EPT_ENH_Info[i].ucIpAddress, Para.EPT_ENH_Information[i].ucIpAddress, 4);
	}

	// Initialize the user interface keyboard engine by setting the appropriate
	// keypress conversion tables.
    if (pSysConfig->uchKeyType == KEYBOARD_CONVENTION) {      // 7448 Conventional keyboard
        UieInitialize(pUifCvtC, pUifFsc, UIF_MAX_MODE);
    } else if (pSysConfig->uchKeyType == KEYBOARD_MICRO){     // 7448 Micromotion keyboard
        UieInitialize(pUifCvtM, pUifFsc, UIF_MAX_MODE);
    } else if (pSysConfig->uchKeyType == KEYBOARD_TOUCH){     // Touchscreen terminal
        UieInitialize(pUifCvtTOUCH, pUifFsc, UIF_MAX_MODE);
    } else if (pSysConfig->uchKeyType == KEYBOARD_WEDGE_68){  // NCR 5932 Wedge 68 key keyboard
        UieInitialize(pUifCvtW68, pUifFsc, UIF_MAX_MODE);
    } else if (pSysConfig->uchKeyType == KEYBOARD_WEDGE_78){  // NCR 5932 Wedge 78 key keyboard
        UieInitialize(pUifCvtW78, pUifFsc, UIF_MAX_MODE);
    } else if (pSysConfig->uchKeyType == KEYBOARD_TICKET){
		UieInitialize(pUifCvtBigTicket, pUifFsc, UIF_MAX_MODE);
	}
	else {                                                    // default to touch
		pSysConfig->uchKeyType = KEYBOARD_TOUCH;
		pTermInfo->uchMenuPageDefault = 10;
        UieInitialize(pUifCvtTOUCH, pUifFsc, UIF_MAX_MODE);
	}

	// Override the default menu page in uniniram with the per terminal menu page number
	ParaCtlTblMenu[0] = pTermInfo->uchMenuPageDefault;

    UieSetErrorTbl((UCHAR *)Para.ParaLeadThru, PARA_LEADTHRU_LEN);/*set error leadthru table */
    UieSetAbort(UIE_POS3 | UIE_POS4 | UIE_POS5);       /* set abort key enable position */
/*    UieSetMessage(0);                        Message Type is Function Key, R3.0 */

    /* 2172 */
	// Initialize the RFL subsystem number of decimal places and decimal separator.
	RflGetSetDecimalCommaType ();

    usUifPowerUp = (USHORT)pSysConfig->uchPowerUpMode; /* get Power Up Mode */
    if (pSysConfig->usInterfacePara & IF_PARA_LOADED_PARA1) {   /* loader ? */
        usUifPowerUp |= IF_PARA_LOADED_PARA1;
    }

    usBMOption = 0;    // default the Backup Master indicator to zero for no Backup.

	{
		int bDoBMOption = 0;

		if (!CliParaMDCCheck(MDC_SYSTEM4_ADR, EVEN_MDC_BIT0)) {
			// Backup Master Terminal is provisioned through the MDC bit setting.
			bDoBMOption = 1;
		}

		if ((pSysConfig->usTerminalPosition & (PIF_CLUSTER_DISCONNECTED_SAT | PIF_CLUSTER_JOINED_SAT)) == PIF_CLUSTER_DISCONNECTED_SAT) {
			// terminal is an Unjoined Disconnected Satellite so we can not have a Backup Master Terminal
			bDoBMOption = 0;
		}

		if (bDoBMOption) {
			usBMOption = 0x01;
			if ( (pSysConfig->ausOption[0] & 0x01) == 0) { /* terminal no.2 found */
				PifLog(MODULE_SYS_STARTUP, LOG_EVENT_BM_DISCOVERY_FAILED);
			}
		}
    }

	{
		char  xBuff[128];
		sprintf (xBuff, "PifMain: Position 0x%x  Backup = 0x%x", pSysConfig->usTerminalPosition, usBMOption);
		NHPOS_NONASSERT_TEXT(xBuff);
	}

	RflConstructEncryptByteStringClass (aUifMasterItemThing);

	if(CliParaMDCCheckField(MDC_STR_FWD_ADR, MDC_PARAM_BIT_D)){
		ParaSetStoreBitFlags(PIF_STORE_AND_FORWARD_ENABLED); // Store and Forward is Enabled so ensure that indicator is On
	}

    PmgInitialize();                        /* Initialize print manager */
    MldInit();                              /* Add R3.0 */
    ApplSetupDevice(APPL_INIT_ALL);         /* Setup all or a degignate device */
    UifCreateInitialize();                  /* initialize create application */
}

/*
*===========================================================================
** Synopsis:    VOID UifCreateInitialize(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Create Create Application Thread
*===========================================================================
*/
VOID UifCreateInitialize(VOID)
{
#if 0
	{
		VOID (THREADENTRY *pFunc)(VOID) = UifCreateMain;  /* Add R3.0 */
		CHAR CONST  *UIFNAMEAPPLCREATE = "CREATAPL";    /* Create Application Name */
		PifBeginThread(pFunc, 0, 0, PRIO_UIE_KEYB, UIFNAMEAPPLCREATE, NULL);
	}
#else
	UifCreateMain();
#endif
    PifBeginThread(UifPowerMonitor, 0, 0, PRIO_POWER_MON, "POWERMON", NULL);
    PifBeginThread(UifAbortMonitor, 0, 0, PRIO_POWER_MON, "ABORTMON", NULL);
}

/*
*===========================================================================
** Synopsis:    VOID THREADENTRY HttpCreateMain(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Create http server thread.
*===========================================================================
*/
VOID HttpCreateMain(VOID)
{
	semHttpServer = PifCreateSem (0);

    PifBeginThread(HttpStartUp, 0, 0, PRIO_SERVER_T, "HTTP_T", NULL);
}

#if 1
typedef struct {
	ULONG  ulBufferEndPoint;
	ULONG  ulActualBytesRead;
	SHORT  hsFileHandle;
	USHORT usFileBufferSizeInBytes;
	UCHAR  *ucFileBuffer;
} ReadFileLineControl;

static SHORT ReadAndProvideFileLineInit (ReadFileLineControl *pControl, SHORT  hsFileHandle, UCHAR *pucFileBuffer, ULONG ulFileBufferSize)
{
	pControl->hsFileHandle = hsFileHandle;
	pControl->ulBufferEndPoint = 0;
	pControl->ulActualBytesRead = 1;
	pControl->ucFileBuffer = pucFileBuffer;
	pControl->usFileBufferSizeInBytes = (USHORT) ulFileBufferSize - 4;
	return 0;
}

static SHORT ReadAndProvideFileLine (ReadFileLineControl *pControl, TCHAR *aszLine, USHORT  usLineMaxChars)
{
	USHORT        usIndex;

	if (pControl->hsFileHandle < 0) {
		pControl->ulBufferEndPoint = 0;
		pControl->ulActualBytesRead = 1;
		return 0;
	}

	if (pControl->ulActualBytesRead == 0 && pControl->ulBufferEndPoint == 0)
		return -1;

	if (pControl->ulBufferEndPoint < pControl->usFileBufferSizeInBytes/2 && pControl->ulActualBytesRead != 0) {
		PifReadFile (pControl->hsFileHandle, pControl->ucFileBuffer + pControl->ulBufferEndPoint, pControl->usFileBufferSizeInBytes - pControl->ulBufferEndPoint, &(pControl->ulActualBytesRead));
		pControl->ulBufferEndPoint += pControl->ulActualBytesRead;
	}

	for (usIndex = 0; usIndex < pControl->ulBufferEndPoint && usIndex < usLineMaxChars; ) {
		aszLine[usIndex] = pControl->ucFileBuffer[usIndex];
		if (pControl->ucFileBuffer[usIndex] == _T('\n')) {
			aszLine[usIndex] = 0;
			usIndex++;
			break;
		}
		
		if (pControl->ucFileBuffer[usIndex] == _T('\r')) {
			aszLine[usIndex] = 0;
			if (pControl->ucFileBuffer[usIndex + 1] != _T('\n')) {
				usIndex++;
				break;
			}
			else {
				usIndex += 2;
				break;
			}
		}

		usIndex++;
		aszLine[usIndex] = 0;
	}

	memmove (pControl->ucFileBuffer, pControl->ucFileBuffer + usIndex, pControl->usFileBufferSizeInBytes - usIndex);

	pControl->ulBufferEndPoint -= usIndex;
	return 0;
}
#endif

static SHORT ReadXmlFileLine (SHORT  hsFileHandle, TCHAR *aszLine, USHORT  usLineMaxChars)
{
	static UCHAR  ucFileBuffer[2048];
	static ULONG  ulBufferEndPoint = 0;
	static ULONG  ulActualBytesRead = 1;

	USHORT        usIndex;

	if (hsFileHandle < 0) {
		ulBufferEndPoint = 0;
		ulActualBytesRead = 1;
		return 0;
	}

	if (ulActualBytesRead == 0 && ulBufferEndPoint == 0)
		return -1;

	if (ulBufferEndPoint < sizeof(ucFileBuffer)/2 && ulActualBytesRead != 0) {
		PifReadFile (hsFileHandle, ucFileBuffer + ulBufferEndPoint, sizeof(ucFileBuffer) - ulBufferEndPoint, &ulActualBytesRead);
		ulBufferEndPoint += ulActualBytesRead;
	}

	for (usIndex = 0; usIndex < ulBufferEndPoint && usIndex < usLineMaxChars; ) {
		aszLine[usIndex] = ucFileBuffer[usIndex];
		if (ucFileBuffer[usIndex] == _T('\n')) {
			aszLine[usIndex] = 0;
			usIndex++;
			break;
		}
		
		if (ucFileBuffer[usIndex] == _T('\r')) {
			aszLine[usIndex] = 0;
			if (ucFileBuffer[usIndex + 1] != _T('\n')) {
				usIndex++;
				break;
			}
			else {
				usIndex += 2;
				break;
			}
		}

		usIndex++;
		aszLine[usIndex] = 0;
	}

	memmove (ucFileBuffer, ucFileBuffer + usIndex, sizeof(ucFileBuffer) - usIndex);

	ulBufferEndPoint -= usIndex;
	return 0;
}

/**
*	This has been cloned from the ReadXmlFileLine function
*	above as a quick fix for colliding buffer areas.
*	This should be redone properly soon.
*/
static SHORT ReadXmlFileEmployee (SHORT  hsFileHandle, TCHAR *aszLine, USHORT  usLineMaxChars)
{ 
	static UCHAR  ucFileBuffer[2048];
	static ULONG  ulBufferEndPoint = 0;
	static ULONG  ulActualBytesRead = 1;

	USHORT        usIndex;

	if (hsFileHandle < 0) {
		ulBufferEndPoint = 0;
		ulActualBytesRead = 1;
		return 0;
	}

	if (ulActualBytesRead == 0 && ulBufferEndPoint == 0)
		return -1;

	if (ulBufferEndPoint < sizeof(ucFileBuffer)/2 && ulActualBytesRead != 0) {
		PifReadFile (hsFileHandle, ucFileBuffer + ulBufferEndPoint, sizeof(ucFileBuffer) - ulBufferEndPoint, &ulActualBytesRead);
		ulBufferEndPoint += ulActualBytesRead;
	}

	for (usIndex = 0; usIndex < ulBufferEndPoint && usIndex < usLineMaxChars; ) {
		aszLine[usIndex] = ucFileBuffer[usIndex];
		if (ucFileBuffer[usIndex] == _T('\n')) {
			aszLine[usIndex] = 0;
			usIndex++;
			break;
		}
		
		if (ucFileBuffer[usIndex] == _T('\r')) {
			aszLine[usIndex] = 0;
			if (ucFileBuffer[usIndex + 1] != _T('\n')) {
				usIndex++;
				break;
			}
			else {
				usIndex += 2;
				break;
			}
		}

		usIndex++;
		aszLine[usIndex] = 0;
	}

	memmove (ucFileBuffer, ucFileBuffer + usIndex, sizeof(ucFileBuffer) - usIndex);

	ulBufferEndPoint -= usIndex;
	return 0;
}

// See the bottom of this file for an example of a PARAMTDR file
// with the various directives that are used.
static SHORT ProcessXmlFileTenderRulesAccountRules (SHORT hsParamtdr, TCHAR *aszLine, USHORT usLineChars)
{
	SHORT  sStatus = 0;
	TCHAR  *ptcsToken = 0, *ptcsTokenEnd = 0;
	TCHAR  aszAccountNumber[32], aszAccountNumberLast[32], aszCardLabel[25];
	USHORT usLength, usTenderKey;
	USHORT usInRule = 0, usInRange = 0, usUseRules = 1, usUseLuhn = 0, usUseRequired = 0;
	USHORT usUseTotal = 0, usUseOrderDec = 0, usUseTender = 0;

	sStatus = ReadXmlFileLine (hsParamtdr, aszLine, usLineChars);
	while (sStatus >= 0) {
		if (aszLine[0] != _T('#')) {
			if (_tcsstr (aszLine, _T("/accountrules")) != 0) {
				break;
			}

			if (0 == _tcschr (aszLine, _T('<'))) {
				// ignore any line that does not have an XML tag on it.
				sStatus = ReadXmlFileLine (hsParamtdr, aszLine, 512);
				continue;
			}

			if ((ptcsToken = _tcsstr (aszLine, _T("<assignment>"))) != 0) {
				usTenderKey = 0;
				usUseTender = 0;
				usUseTotal = 0;
				usUseOrderDec = 0;
				usLength = 0;
				usUseLuhn = 0;
				usUseRequired = 0;
				memset (aszAccountNumber, 0, sizeof(aszAccountNumber));
				memset (aszAccountNumberLast, 0, sizeof(aszAccountNumberLast));
				memset (aszCardLabel, 0, sizeof(aszCardLabel));
				usInRule = 1;
			}
			else if ((ptcsToken = _tcsstr (aszLine, _T("<validation>"))) != 0) {
				usTenderKey = 0;
				usUseTender = 0;
				usUseTotal = 0;
				usUseOrderDec = 0;
				usLength = 0;
				usUseLuhn = 0;
				usUseRequired = 0;
				memset (aszAccountNumber, 0, sizeof(aszAccountNumber));
				memset (aszAccountNumberLast, 0, sizeof(aszAccountNumberLast));
				memset (aszCardLabel, 0, sizeof(aszCardLabel));
				usInRule = 1;
			}
			else if ((ptcsToken = _tcsstr (aszLine, _T("<numtype>"))) != 0) {
				usTenderKey = 0;
				usUseTender = 0;
				usUseTotal = 0;
				usUseOrderDec = 0;
				usLength = 0;
				memset (aszAccountNumber, 0, sizeof(aszAccountNumber));
				memset (aszAccountNumberLast, 0, sizeof(aszAccountNumberLast));
				memset (aszCardLabel, 0, sizeof(aszCardLabel));
				usInRule = 1;
			}
			else if (usUseRules && (ptcsToken = _tcsstr (aszLine, _T("</assignment>"))) != 0) {
				USHORT  usFlags = 0;

				usFlags |= 0x0004;    //  indicates that it is a tender key.
				ParaTendAccountRuleWrite (aszAccountNumber, aszAccountNumberLast, aszCardLabel, usFlags, usLength, usTenderKey);
				usInRule = 0;
			}
			else if (usUseRules && (ptcsToken = _tcsstr (aszLine, _T("</validation>"))) != 0) {
				USHORT  usFlags = 0;

				usTenderKey |= PARATENDACCOUNTRULE_VALIDATE;   // indicate that this is a validation rule
				usFlags |= 0x0004;    //  indicates that it is a tender key.
				if (usUseLuhn)
					usFlags |= 0x0001;    //  <luhnmod10> is indicated.
				if (usUseRequired)
					usFlags |= 0x0002;
				ParaTendAccountRuleWrite (aszAccountNumber, aszAccountNumberLast, aszCardLabel, usFlags, usLength, usTenderKey);
				usInRule = 0;
			}
			else if (usUseRules && (ptcsToken = _tcsstr (aszLine, _T("</numtype>"))) != 0) {
				USHORT  usFlags = 0;

				usTenderKey |= PARATENDACCOUNTRULE_NUMTYPE;   // indicate that this is a #/Type rule
				if (usUseTender)
					usFlags |= 0x0004;    //  indicates that it is a tender key.
				if (usUseTotal)
					usFlags |= 0x0008;    //  indicates that it is a total key.
				if (usUseOrderDec)
					usFlags |= 0x0010;    //  indicates that it is an order declaration key.
				ParaTendAccountRuleWrite (aszAccountNumber, aszAccountNumberLast, 0, usFlags, usLength, usTenderKey);
				usInRule = 0;
			}
			else if ((ptcsToken = _tcsstr (aszLine, _T("<userules>"))) != 0) {
				ptcsToken = _tcschr (ptcsToken, _T('>'));
				ptcsToken++;
				ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
				if (ptcsTokenEnd) {
					*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
				}
				usUseRules = _ttoi (ptcsToken);
			}
			else if ((ptcsToken = _tcsstr (aszLine, _T("<required>"))) != 0) {
				ptcsToken = _tcschr (ptcsToken, _T('>'));
				ptcsToken++;
				ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
				if (ptcsTokenEnd) {
					*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
				}
				usUseRequired = _ttoi (ptcsToken);
			}
			else if ((ptcsToken = _tcsstr (aszLine, _T("<luhnmod10>"))) != 0) {
				ptcsToken = _tcschr (ptcsToken, _T('>'));
				ptcsToken++;
				ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
				if (ptcsTokenEnd) {
					*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
				}
				usUseLuhn = _ttoi (ptcsToken);
			}
			else if (usUseRules && (ptcsToken = _tcsstr (aszLine, _T("<ignoretender>"))) != 0) {
				USHORT  usTenderKeyIgnore = 0;

				ptcsToken = _tcschr (ptcsToken, _T('>'));
				ptcsToken++;
				ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
				if (ptcsTokenEnd) {
					*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
				}
				usTenderKeyIgnore = _ttoi (ptcsToken);
				if (usTenderKeyIgnore > 1)
					ParaTendAccountRuleWrite (NULL, NULL, 0, 0, 0, usTenderKeyIgnore);
			}

			if (usInRule && usUseRules) {
				if ((ptcsToken = _tcsstr (aszLine, _T("<range>"))) != 0) {
					usInRange = 1;
				}
				else if ((ptcsToken = _tcsstr (aszLine, _T("</range>"))) != 0) {
					usInRange = 0;
				}
				else if ((ptcsToken = _tcsstr (aszLine, _T("<tender>"))) != 0) {
					ptcsToken = _tcschr (ptcsToken, _T('>'));
					ptcsToken++;
					ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
					if (ptcsTokenEnd) {
						*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
					}
					usTenderKey = _ttoi (ptcsToken);
					usUseTender = 1;
				}
				else if ((ptcsToken = _tcsstr (aszLine, _T("<total>"))) != 0) {
					ptcsToken = _tcschr (ptcsToken, _T('>'));
					ptcsToken++;
					ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
					if (ptcsTokenEnd) {
						*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
					}
					usTenderKey = _ttoi (ptcsToken);
					usUseTotal = 1;
				}
				else if ((ptcsToken = _tcsstr (aszLine, _T("<orderdec>"))) != 0) {
					ptcsToken = _tcschr (ptcsToken, _T('>'));
					ptcsToken++;
					ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
					if (ptcsTokenEnd) {
						*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
					}
					usTenderKey = _ttoi (ptcsToken);
					if (usTenderKey > 0)
						usTenderKey--;       // order declare has range of 0 - 4 but the number entered is 1 - 5.
					usUseOrderDec = 1;
				}
				else if ((ptcsToken = _tcsstr (aszLine, _T("<length>"))) != 0) {
					USHORT  usLengthEnd = 0;

					ptcsToken = _tcschr (ptcsToken, _T('>'));
					ptcsToken++;
					ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
					if (ptcsTokenEnd) {
						*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
					}
					{
						// process a length range if specified.  range can be open ended so possibly:
						//   n1  meaning single length
						//   n1-n2  or n1-  a range between n1 and n2, or equal to or greater than n1
						TCHAR  *ptcsRange = _tcschr (ptcsToken, _T('-'));
						usLengthEnd = 0;
						if (ptcsRange) {
							ptcsRange++;
							usLengthEnd = 255;
							if (*ptcsRange) {
								usLengthEnd = (_ttoi (ptcsRange) & 0xff);
							}
							usLengthEnd <<= 8;
						}
					}
					usLength = ((_ttoi (ptcsToken) & 0xff) | usLengthEnd);
				}
				else if (usInRange == 0 && (ptcsToken = _tcsstr (aszLine, _T("<number>"))) != 0) {
					ptcsToken = _tcschr (ptcsToken, _T('>'));
					ptcsToken++;
					ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
					if (ptcsTokenEnd) {
						*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
					}
					memset (aszAccountNumber, 0, sizeof(aszAccountNumber));
					_tcsncpy (aszAccountNumber, ptcsToken, 31);
				}
				else if (usInRange == 0 && (ptcsToken = _tcsstr (aszLine, _T("<cardlabel>"))) != 0) {
					ptcsToken = _tcschr (ptcsToken, _T('>'));
					ptcsToken++;
					ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
					if (ptcsTokenEnd) {
						*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
					}
					memset (aszCardLabel, 0, sizeof(aszCardLabel));
					_tcsncpy (aszCardLabel, ptcsToken, 16);          // see also TRN_DSI_CARDTYPE and EEPTRSPDATA.aszCardLabel
				}
				else if (usInRange == 1) {
					TCHAR  *ptcsAccount = 0;

					if ((ptcsToken = _tcsstr (aszLine, _T("<first>"))) != 0) {
						ptcsAccount =  aszAccountNumber;
					}
					else if ((ptcsToken = _tcsstr (aszLine, _T("<last>"))) != 0) {
						ptcsAccount =  aszAccountNumberLast;
					}

					if (ptcsAccount) {
						ptcsToken = _tcschr (ptcsToken, _T('>'));
						ptcsToken++;
						ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
						if (ptcsTokenEnd) {
							*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
						}
						memset (ptcsAccount, 0, sizeof(aszAccountNumber));
						_tcsncpy (ptcsAccount, ptcsToken, 31);
					}
				}
			}
		}
		sStatus = ReadXmlFileLine (hsParamtdr, aszLine, 512);
	}

	return sStatus;
}

// See the bottom of this file for an example of a PARAMTDR file
// with the various directives that are used.
static SHORT ProcessXmlFileTenderRulesReturnsRules (SHORT hsParamtdr, TCHAR *aszLine, USHORT usLineChars)
{
	SHORT  sStatus = 0;
	TCHAR  *ptcsToken = 0, *ptcsTokenEnd = 0;
	TCHAR  aszAccountNumber[32], aszAccountNumberLast[32];
	USHORT usLength, usTenderKey;
	USHORT usInRule = 0, usInRange = 0, usUseRules = 1;
	USHORT usUseTender = 0;
	USHORT usType = 0, usUseType = 0, usGlobalRules = 0;
	TCHAR  tcsBatchCloseIniPath[512], tcsBatchCloseKeyword[48];

	memset (tcsBatchCloseIniPath, 0, sizeof(tcsBatchCloseIniPath));
	memset (tcsBatchCloseKeyword, 0, sizeof(tcsBatchCloseKeyword));

	sStatus = ReadXmlFileLine (hsParamtdr, aszLine, usLineChars);
	while (sStatus >= 0) {
		if (aszLine[0] != _T('#')) {
			if (_tcsstr (aszLine, _T("/returnsrules")) != 0) {
				break;
			}

			if (0 == _tcschr (aszLine, _T('<'))) {
				// ignore any line that does not have an XML tag on it.
				sStatus = ReadXmlFileLine (hsParamtdr, aszLine, 512);
				continue;
			}

			if ((ptcsToken = _tcsstr (aszLine, _T("<userules>"))) != 0) {
				USHORT  usFlags = 0;

				ptcsToken = _tcschr (ptcsToken, _T('>'));
				ptcsToken++;
				ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
				if (ptcsTokenEnd) {
					*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
				}
				usUseRules = _ttoi (ptcsToken);
				if (usUseRules) {
					usFlags |= PARATENDRETURNFLAGS_USINGRULES;    //  indicates that it affects all tenders.
					ParaTendReturnsRuleWrite (NULL, NULL, usFlags, 0, 255);
				}
			}
			else if (usUseRules && (ptcsToken = _tcsstr (aszLine, _T("<ignoretender>"))) != 0) {
				USHORT  usTenderKeyIgnore = 0;
				USHORT  usFlags = 0;

				ptcsToken = _tcschr (ptcsToken, _T('>'));
				ptcsToken++;
				ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
				if (ptcsTokenEnd) {
					*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
				}
				usTenderKeyIgnore = _ttoi (ptcsToken);
				usFlags |= PARATENDRETURNFLAGS_IGNORETENDER;    //  indicates to ignore the specified tender.
				if (usTenderKeyIgnore > 0)
					ParaTendReturnsRuleWrite (NULL, NULL, usFlags, 0, usTenderKeyIgnore);
			}
			else if (usUseRules && (ptcsToken = _tcsstr (aszLine, _T("<consolidatetender>"))) != 0) {
				USHORT  usTenderKeyConsolidate = 0;
				USHORT  usFlags = 0;

				ptcsToken = _tcschr (ptcsToken, _T('>'));
				ptcsToken++;
				ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
				if (ptcsTokenEnd) {
					*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
				}
				usTenderKeyConsolidate = _ttoi (ptcsToken);
				usFlags |= PARATENDRETURNFLAGS_CONSOLIDATETENDER;    //  indicates to consolidate the specified tender.
				if (usTenderKeyConsolidate > 0)
					ParaTendReturnsRuleWrite (NULL, NULL, usFlags, 0, usTenderKeyConsolidate);
			}
			else if ((ptcsToken = _tcsstr (aszLine, _T("<cardtype>"))) != 0) {
				usTenderKey = 0;
				usUseTender = 0;
				usLength = 0;
				memset (aszAccountNumber, 0, sizeof(aszAccountNumber));
				memset (aszAccountNumberLast, 0, sizeof(aszAccountNumberLast));
				usInRule = 1;
			}
			else if (usUseRules && (ptcsToken = _tcsstr (aszLine, _T("</cardtype>"))) != 0) {
				USHORT  usFlags = 0;

				usTenderKey |= PARATENDACCOUNTRULE_CARDTYPE;   // indicate that this is a validation rule
				usFlags |= 0x0004;    //  indicates that it is a tender key.
				ParaTendReturnsRuleWrite (aszAccountNumber, aszAccountNumberLast, usFlags, usLength, usTenderKey);
				usInRule = 0;
			}
			else if ((ptcsToken = _tcsstr (aszLine, _T("<globalrules>"))) != 0) {
				usTenderKey = 0;
				usUseTender = 0;
				usGlobalRules = 0;
				usLength = 0;
				memset (aszAccountNumber, 0, sizeof(aszAccountNumber));
				memset (aszAccountNumberLast, 0, sizeof(aszAccountNumberLast));
				usInRule = 1;
			}
			else if (usUseRules && (ptcsToken = _tcsstr (aszLine, _T("</globalrules>"))) != 0) {
				USHORT  usFlags = 0;

				usTenderKey |= PARATENDACCOUNTRULE_GLOBALRULES;   // indicate that this is a validation rule
				usFlags |= 0x0004;    //  indicates that it is a tender key.
				aszAccountNumber[0] = usGlobalRules;
				ParaTendReturnsRuleWrite (aszAccountNumber, 0, usFlags, usLength, usTenderKey);
				ParaTendReturnsBatchCloseWrite (tcsBatchCloseIniPath, tcsBatchCloseKeyword);
				usInRule = 0;
				{
					DATE_TIME  DateTime;
					SHORT      sTest = ParaTendReturnsBatchCloseReadDateTime (&DateTime);
					char       xBuff[64];

					sprintf (xBuff, "Batch Close %4.4d/%2.2d/%2.2d %2.2d:%2.2d:%2.2d", DateTime.usYear, DateTime.usMonth, DateTime.usMDay,
							DateTime.usHour, DateTime.usMin, DateTime.usSec);
					NHPOS_NONASSERT_TEXT (xBuff);
				}
			}

			if (usInRule && usUseRules) {
				if ((ptcsToken = _tcsstr (aszLine, _T("<tender>"))) != 0) {
					ptcsToken = _tcschr (ptcsToken, _T('>'));
					ptcsToken++;
					ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
					if (ptcsTokenEnd) {
						*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
					}
					usTenderKey = _ttoi (ptcsToken);
					usUseTender = 1;
				}
				else if ((ptcsToken = _tcsstr (aszLine, _T("<type>"))) != 0) {
					ptcsToken = _tcschr (ptcsToken, _T('>'));
					ptcsToken++;
					ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
					if (ptcsTokenEnd) {
						*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
					}
					usType = _ttoi (ptcsToken);
					usUseType = 1;
				}
				else if ((ptcsToken = _tcsstr (aszLine, _T("<batchonly>"))) != 0) {
					ptcsToken = _tcschr (ptcsToken, _T('>'));
					ptcsToken++;
					ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
					if (ptcsTokenEnd) {
						*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
					}
					usType = _ttoi (ptcsToken);
					if (usType)
						usGlobalRules |= PARATENDRETURNGLOBAL_BATCHONLY;
					else
						usGlobalRules &= ~PARATENDRETURNGLOBAL_BATCHONLY;
				}
				else if ((ptcsToken = _tcsstr (aszLine, _T("<batchcloseini>"))) != 0) {
					ptcsToken = _tcschr (ptcsToken, _T('>'));
					ptcsToken++;
					ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
					if (ptcsTokenEnd) {
						*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
					}
					_tcscpy (tcsBatchCloseIniPath, ptcsToken);
					usGlobalRules |= PARATENDRETURNGLOBAL_BATCHONLY;
				}
				else if ((ptcsToken = _tcsstr (aszLine, _T("<batchclosekeyword>"))) != 0) {
					ptcsToken = _tcschr (ptcsToken, _T('>'));
					ptcsToken++;
					ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
					if (ptcsTokenEnd) {
						*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
					}
					_tcscpy (tcsBatchCloseKeyword, ptcsToken);
					usGlobalRules |= PARATENDRETURNGLOBAL_BATCHONLY;
				}
				else if ((ptcsToken = _tcsstr (aszLine, _T("<approvedonly>"))) != 0) {
					ptcsToken = _tcschr (ptcsToken, _T('>'));
					ptcsToken++;
					ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
					if (ptcsTokenEnd) {
						*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
					}
					usType = _ttoi (ptcsToken);
					if (usType)
						usGlobalRules |= PARATENDRETURNGLOBAL_APPROVEDONLY;
					else
						usGlobalRules &= ~PARATENDRETURNGLOBAL_APPROVEDONLY;
				}
				else if ((ptcsToken = _tcsstr (aszLine, _T("<liketolike>"))) != 0) {
					ptcsToken = _tcschr (ptcsToken, _T('>'));
					ptcsToken++;
					ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
					if (ptcsTokenEnd) {
						*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
					}
					usType = _ttoi (ptcsToken);
					if (usType)
						usGlobalRules |= PARATENDRETURNGLOBAL_LIKETOLIKEONLY;
					else
						usGlobalRules &= ~PARATENDRETURNGLOBAL_LIKETOLIKEONLY;
				}
				else if ((ptcsToken = _tcsstr (aszLine, _T("<accounttoaccount>"))) != 0) {
					ptcsToken = _tcschr (ptcsToken, _T('>'));
					ptcsToken++;
					ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
					if (ptcsTokenEnd) {
						*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
					}
					usType = _ttoi (ptcsToken);
					if (usType)
						usGlobalRules |= PARATENDRETURNGLOBAL_ACCOUNTTOACCOUNTONLY;
					else
						usGlobalRules &= ~PARATENDRETURNGLOBAL_ACCOUNTTOACCOUNTONLY;
				}
				else if ((ptcsToken = _tcsstr (aszLine, _T("<allowstored>"))) != 0) {
					ptcsToken = _tcschr (ptcsToken, _T('>'));
					ptcsToken++;
					ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
					if (ptcsTokenEnd) {
						*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
					}
					usType = _ttoi (ptcsToken);
					if (usType)
						usGlobalRules |= PARATENDRETURNGLOBAL_ALLOWSTORED;
					else
						usGlobalRules &= ~PARATENDRETURNGLOBAL_ALLOWSTORED;
				}
				else if ((ptcsToken = _tcsstr (aszLine, _T("<allowdeclined>"))) != 0) {
					ptcsToken = _tcschr (ptcsToken, _T('>'));
					ptcsToken++;
					ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
					if (ptcsTokenEnd) {
						*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
					}
					usType = _ttoi (ptcsToken);
					if (usType)
						usGlobalRules |= PARATENDRETURNGLOBAL_ALLOWDECLINED;
					else
						usGlobalRules &= ~PARATENDRETURNGLOBAL_ALLOWDECLINED;
				}
				else if ((ptcsToken = _tcsstr (aszLine, _T("<allowpreauth>"))) != 0) {
					ptcsToken = _tcschr (ptcsToken, _T('>'));
					ptcsToken++;
					ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
					if (ptcsTokenEnd) {
						*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
					}
					usType = _ttoi (ptcsToken);
					if (usType)
						usGlobalRules |= PARATENDRETURNGLOBAL_ALLOWPREAUTH;
					else
						usGlobalRules &= ~PARATENDRETURNGLOBAL_ALLOWPREAUTH;
				}
				else if ((ptcsToken = _tcsstr (aszLine, _T("<length>"))) != 0) {
					USHORT  usLengthEnd = 0;

					ptcsToken = _tcschr (ptcsToken, _T('>'));
					ptcsToken++;
					ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
					if (ptcsTokenEnd) {
						*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
					}
					{
						// process a length range if specified.  range can be open ended so possibly:
						//   n1  meaning single length
						//   n1-n2  or n1-  a range between n1 and n2, or equal to or greater than n1
						TCHAR  *ptcsRange = _tcschr (ptcsToken, _T('-'));
						usLengthEnd = 0;
						if (ptcsRange) {
							ptcsRange++;
							usLengthEnd = 255;
							if (*ptcsRange) {
								usLengthEnd = (_ttoi (ptcsRange) & 0xff);
							}
							usLengthEnd <<= 8;
						}
					}
					usLength = ((_ttoi (ptcsToken) & 0xff) | usLengthEnd);
				}
				else if (usInRange == 0 && (ptcsToken = _tcsstr (aszLine, _T("<number>"))) != 0) {
					ptcsToken = _tcschr (ptcsToken, _T('>'));
					ptcsToken++;
					ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
					if (ptcsTokenEnd) {
						*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
					}
					memset (aszAccountNumber, 0, sizeof(aszAccountNumber));
					_tcsncpy (aszAccountNumber, ptcsToken, 31);
				}
				else if (usInRange == 1) {
					TCHAR  *ptcsAccount = 0;

					if ((ptcsToken = _tcsstr (aszLine, _T("<first>"))) != 0) {
						ptcsAccount =  aszAccountNumber;
					}
					else if ((ptcsToken = _tcsstr (aszLine, _T("<last>"))) != 0) {
						ptcsAccount =  aszAccountNumberLast;
					}

					if (ptcsAccount) {
						ptcsToken = _tcschr (ptcsToken, _T('>'));
						ptcsToken++;
						ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
						if (ptcsTokenEnd) {
							*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
						}
						memset (ptcsAccount, 0, sizeof(aszAccountNumber));
						_tcsncpy (ptcsAccount, ptcsToken, 31);
					}
				}
			}
		}
		sStatus = ReadXmlFileLine (hsParamtdr, aszLine, 512);
	}

	return sStatus;
}

static SHORT ProcessXmlFileTenderRules (VOID)
{
	TCHAR   *aszAccountTenderRulesFile = _T("PARAMTDR");
	SHORT   hsParamtdr;

	// Always clear the rules table so that if the file does not exist or we
	// are unable to parse it then at least we start with a known state.
	ParaTendAccountRuleClear ();
	ParaTendReturnsRuleClear ();

	hsParamtdr = PifOpenFile (aszAccountTenderRulesFile, "or");  // open PARAMTDR

	if (hsParamtdr >= 0) {
		TCHAR  aszLine[512];
		USHORT usLineChars = 512;
		SHORT  sStatus;

		ReadXmlFileLine (-1, 0, 0);  // init the data structures for the next time it is used.
		sStatus = ReadXmlFileLine (hsParamtdr, aszLine, usLineChars);
		while (sStatus >= 0) {
			if (aszLine[0] != _T('#')) {
				if (_tcsstr (aszLine, _T("accountrules")) != 0) {
					ProcessXmlFileTenderRulesAccountRules (hsParamtdr, aszLine, usLineChars);
				} else if (_tcsstr (aszLine, _T("returnsrules")) != 0) {
					ProcessXmlFileTenderRulesReturnsRules (hsParamtdr, aszLine, usLineChars);
				}
			}
			sStatus = ReadXmlFileLine (hsParamtdr, aszLine, usLineChars);
		}

		PifCloseFile (hsParamtdr);
		ReadXmlFileLine (-1, 0, 0);  // init the data structures for the next time it is used.
	}

	return 0;
}

static SHORT ProcessXmlFileTrainRulesAmtrakRules (SHORT hsParamtdr, TCHAR *aszLine, USHORT usLineChars)
{
	SHORT  sStatus = 0;
	TCHAR  *ptcsToken = 0, *ptcsTokenEnd = 0;
	USHORT usInRule = 0, usInRange = 0, usUseRules = 1;

	sStatus = ReadXmlFileLine (hsParamtdr, aszLine, usLineChars);
	while (sStatus >= 0) {
		if (aszLine[0] != _T('#')) {
			if (_tcsstr (aszLine, _T("/train")) != 0) {
				break;
			}
		}
		sStatus = ReadXmlFileLine (hsParamtdr, aszLine, 512);
	}

	return sStatus;
}

static SHORT ProcessXmlFileAmtrakTrainRules (VOID)
{
	TCHAR   *aszAccountTenderRulesFile = _T("PARAMATR");
	SHORT   hsParamtdr;

	// Always clear the rules table so that if the file does not exist or we
	// are unable to parse it then at least we start with a known state.
	ParaTendAccountRuleClear ();

	hsParamtdr = PifOpenFile (aszAccountTenderRulesFile, "or");  // open PARAMTDR

	if (hsParamtdr >= 0) {
		TCHAR  aszLine[512];
		USHORT usLineChars = 512;
		SHORT  sStatus;

		ReadXmlFileLine (-1, 0, 0);  // init the data structures for the next time it is used.
		sStatus = ReadXmlFileLine (hsParamtdr, aszLine, usLineChars);
		while (sStatus >= 0) {
			if (aszLine[0] != _T('#')) {
				if (_tcsstr (aszLine, _T("train")) != 0) {
					ProcessXmlFileTrainRulesAmtrakRules (hsParamtdr, aszLine, usLineChars);
					break;
				}
			}
			sStatus = ReadXmlFileLine (hsParamtdr, aszLine, usLineChars);
		}

		PifCloseFile (hsParamtdr);
		ReadXmlFileLine (-1, 0, 0);  // init the data structures for the next time it is used.
	}

	return 0;
}

static TCHAR tchmegaXMLBuffer[200000];
static UCHAR uchmegaXMLBuffer[200000];

static SHORT ProcessXmlFileStartUpFrameworkXml (ReadFileLineControl *pMyXmlFileControl, TCHAR *aszLine, USHORT usLineChars)
{
	static AllObjects   myObject[100];
	SHORT        sRetStatus;
	USHORT       usLen;
	USHORT       usObjectIndex = 0;
	TCHAR        *ptcsToken = 0, *ptcsTokenEnd = 0;

	sRetStatus = ReadAndProvideFileLine (pMyXmlFileControl, aszLine, usLineChars);
	while (sRetStatus >= 0) {
		if (aszLine[0] != _T('#')) {
			if (_tcsstr (aszLine, _T("/frameworkxml")) != 0) {
				break;
			}

			if (0 == _tcschr (aszLine, _T('<'))) {
				// ignore any line that does not have an XML tag on it.
				sRetStatus = ReadAndProvideFileLine (pMyXmlFileControl, aszLine, usLineChars);
				continue;
			}

			if ((ptcsToken = _tcsstr (aszLine, _T("<filename>"))) != 0) {
				ptcsToken = _tcschr (ptcsToken, _T('>'));
				ptcsToken++;
				ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
				if (ptcsTokenEnd) {
					*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
				}

				usLen = tcharlen(ptcsToken);
				if (usLen > 0) {
					TCHAR *pMegaBuffer = tchmegaXMLBuffer;
					SHORT hsXmlFile;

					hsXmlFile = PifOpenFile (ptcsToken, "or");  // open PARAMINI
					if (hsXmlFile >= 0) {
						VOID  *pHwnd = 0;
						ULONG  ulBytesRead = 0, ulIndex;
						PifReadFile (hsXmlFile, uchmegaXMLBuffer, sizeof(uchmegaXMLBuffer) - 1, &ulBytesRead);
						PifCloseFile (hsXmlFile);
						uchmegaXMLBuffer[ulBytesRead] = 0;
						for (ulIndex = 0; ulIndex <= ulBytesRead; ulIndex++) {
							tchmegaXMLBuffer[ulIndex] = uchmegaXMLBuffer[ulIndex];
						}
						PifGetWindowHandle (&pHwnd);
						if (pHwnd) {
							while (*pMegaBuffer) {
								if (BlProcessConnEngineMessageAndTag(&pMegaBuffer, &(myObject[usObjectIndex])) < 0)
									break;
								if (myObject[usObjectIndex].iMessageTag == BL_CONNENGINE_MSG_FRAMEWORK) {
									int  iLoop;
									iLoop = PostMessage (pHwnd, WM_APP_XMLMSG_MSG_RCVD, (WPARAM)&(myObject[usObjectIndex]), 0);
									usObjectIndex = ((usObjectIndex + 1) % 100);
									PifSleep (50);
								}
							}
						}
						ulIndex = 0;
					}
				}
			}
		}
		sRetStatus = ReadAndProvideFileLine (pMyXmlFileControl, aszLine, usLineChars);
	}

	return sRetStatus;
}

static SHORT ProcessXmlFileStartUpProcessRules (ReadFileLineControl *pMyXmlFileControl, TCHAR *aszLine, USHORT usLineChars)
{
	SHORT        sRetStatus;
	TCHAR        cFilePath[256];
	USHORT       usLen;
	TCHAR        *ptcsToken = 0, *ptcsTokenEnd = 0;

	sRetStatus = ReadAndProvideFileLine (pMyXmlFileControl, aszLine, usLineChars);
	while (sRetStatus >= 0) {
		if (aszLine[0] != _T('#')) {
			if (_tcsstr (aszLine, _T("/startupprocess")) != 0) {
				break;
			}

			if (0 == _tcschr (aszLine, _T('<'))) {
				// ignore any line that does not have an XML tag on it.
				sRetStatus = ReadAndProvideFileLine (pMyXmlFileControl, aszLine, usLineChars);
				continue;
			}

			// get the database folder path and add trailing backslash to prep for adding file name.
			_tcscpy (cFilePath, STD_FOLDER_DATABASEFILES);
			_tcscat (cFilePath, _T("\\"));

			if ((ptcsToken = _tcsstr (aszLine, _T("<filename>"))) != 0) {
				ptcsToken = _tcschr (ptcsToken, _T('>'));
				ptcsToken++;
				ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
				if (ptcsTokenEnd) {
					*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
				}

				usLen = tcharlen(ptcsToken);
				if (usLen > 0) {
					int i = 0;
					int j = tcharlen(cFilePath);

					for (i = 0; ptcsToken[i]; i++) {
						cFilePath[j + i] = ptcsToken[i];
					}
					cFilePath[j+i] = _T('\0');
					sRetStatus = UieShellExecuteFile (cFilePath);
				}
			}
			else if ((ptcsToken = _tcsstr (aszLine, _T("<exe-monitor>"))) != 0) {
				ptcsToken = _tcschr (ptcsToken, _T('>'));
				ptcsToken++;
				ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
				if (ptcsTokenEnd) {
					*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
				}

				usLen = tcharlen(ptcsToken);
				if (usLen > 0) {
					int i = 0;
					int j;
					TCHAR *pCommandLine, ExeName[256];

					// If the directive has a path name that begins with a quote
					// then we assume that it is a full path name and so we will
					// use that path name rather than our default.
					if (ptcsToken[0] == _T('\"')) {
						ExeName[0] = cFilePath[0] = 0;
						j = 0;
					}
					else {
						_tcscpy (ExeName, cFilePath);
						j = tcharlen(cFilePath);
					}
					pCommandLine = ExeName + tcharlen(ExeName);
					for (i = 0; ptcsToken[i]; i++) {
						ExeName[j + i] = cFilePath[j + i] = ptcsToken[i];
					}
					ExeName[j + i] = cFilePath[j+i] = _T('\0');

					// If the executable path starts with a quote then lets
					// clean up the path name for the executable by removing the
					// quotes around the path name of the executable.
					if (*pCommandLine == _T('\"')) {
						*pCommandLine = *(pCommandLine + 1);
						while (*pCommandLine && *(pCommandLine + 1) != _T('\"')) {
							*pCommandLine = *(pCommandLine + 1);
							pCommandLine++;
						}
						*pCommandLine = _T('\0');
					}
					else {
						while (*pCommandLine && *pCommandLine != _T(' ')) pCommandLine++;
						*pCommandLine = _T('\0');
					}

					sRetStatus = UieShellExecuteFileMonitor (ExeName, cFilePath);
				}
			}
		}
		sRetStatus = ReadAndProvideFileLine (pMyXmlFileControl, aszLine, usLineChars);
	}

	return sRetStatus;
}

static SHORT ProcessXmlFileReportsFolder(ReadFileLineControl * pMyXmlFileControl, TCHAR * endTag, TCHAR * aszLine, USHORT usLineChars)
{
	SHORT        sRetStatus;
	TCHAR        *ptcsToken = 0, *ptcsTokenEnd = 0;
    SYSCONFIG    *pSysConfig = PifSysConfig();                    /* get system config */

	sRetStatus = ReadAndProvideFileLine (pMyXmlFileControl, aszLine, usLineChars);
	while (sRetStatus >= 0) {
		if (aszLine[0] != _T('#')) {
			if (_tcsstr (aszLine, endTag) != 0) {
				break;
			}

			if (0 == _tcschr (aszLine, _T('<'))) {
				// ignore any line that does not have an XML tag on it.
				sRetStatus = ReadAndProvideFileLine (pMyXmlFileControl, aszLine, usLineChars);
				continue;
			}

			// Process directives for the historial folder which describes the path as well as what can be put there.
			// The historical folder can be accessed with PifOpenFile (fn, "h");  which indicates FLHISFOLDER.
			if ((ptcsToken = _tcsstr (aszLine, _T("<folder>"))) != 0) {
				ptcsToken = _tcschr (ptcsToken, _T('>'));
				ptcsToken++;
				ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
				if (ptcsTokenEnd) {
					*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
				}
				tcharTrimLeading(ptcsToken);
				tcharTrimRight(ptcsToken);
				memset (pSysConfig->tcsReportsHistoricalFolder, 0, sizeof(pSysConfig->tcsReportsHistoricalFolder));
				_tcsncpy (pSysConfig->tcsReportsHistoricalFolder, ptcsToken, TCHARSIZEOF(pSysConfig->tcsReportsHistoricalFolder) - 1);
			} else if ((ptcsToken = _tcsstr (aszLine, _T("<output>"))) != 0) {
				// the strings that are allowed for the different output types must agree with the
				// output types that are supported by the functionality in function ItemOpenHistorialReportsFolder().
				ptcsToken = _tcschr (ptcsToken, _T('>'));
				ptcsToken++;
				ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
				if (ptcsTokenEnd) {
					*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
				}
				tcharTrimLeading(ptcsToken);
				tcharTrimRight(ptcsToken);
				memset (pSysConfig->tcsReportsHistoricalType, 0, sizeof(pSysConfig->tcsReportsHistoricalType));
				_tcsncpy (pSysConfig->tcsReportsHistoricalType, ptcsToken, TCHARSIZEOF(pSysConfig->tcsReportsHistoricalType) - 1);
			} else if ((ptcsToken = _tcsstr(aszLine, _T("<mnemonics>"))) != 0) {
				ULONG   ulMnemonicsType = 0;
				ptcsToken = _tcschr(ptcsToken, _T('>'));
				ptcsToken++;
				ptcsTokenEnd = _tcschr(ptcsToken, _T('<'));
				if (ptcsTokenEnd) {
					*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
					ulMnemonicsType = _ttoi(ptcsToken);
				}
				pSysConfig->ulReportsHistoricalMnemonics = ulMnemonicsType;
			}
		}
		sRetStatus = ReadAndProvideFileLine (pMyXmlFileControl, aszLine, usLineChars);
	}

	return sRetStatus;
}

static SHORT ProcessXmlFileHistoricalFolder(ReadFileLineControl* pMyXmlFileControl, TCHAR* aszLine, USHORT usLineChars)
{
	TCHAR* endTag = _T("</historicalfolder>");
	return ProcessXmlFileReportsFolder(pMyXmlFileControl, endTag, aszLine, usLineChars);
}

static SHORT ProcessXmlFileWebFolder(ReadFileLineControl* pMyXmlFileControl, TCHAR* aszLine, USHORT usLineChars)
{
	TCHAR* endTag = _T("</webfolder>");
	return ProcessXmlFileReportsFolder(pMyXmlFileControl, endTag, aszLine, usLineChars);
}

static SHORT ProcessXmlFilePrintFolder(ReadFileLineControl* pMyXmlFileControl, TCHAR* aszLine, USHORT usLineChars)
{
	TCHAR* endTag = _T("</printfolder>");
	return ProcessXmlFileReportsFolder(pMyXmlFileControl, endTag, aszLine, usLineChars);
}


static SHORT ProcessXmlFileStartUpBarcodeRules (ReadFileLineControl *pMyXmlFileControl, TCHAR *aszLine, USHORT usLineChars)
{
	SHORT        sRetStatus;
	TCHAR        *ptcsToken = 0, *ptcsTokenEnd = 0;

	sRetStatus = ReadAndProvideFileLine (pMyXmlFileControl, aszLine, usLineChars);
	while (sRetStatus >= 0) {
		if (aszLine[0] != _T('#')) {
			if (_tcsstr (aszLine, _T("/barcode")) != 0) {
				break;
			}

			if (0 == _tcschr (aszLine, _T('<'))) {
				// ignore any line that does not have an XML tag on it.
				sRetStatus = ReadAndProvideFileLine (pMyXmlFileControl, aszLine, usLineChars);
				continue;
			}

			if ((ptcsToken = _tcsstr (aszLine, _T("<code"))) != 0) {
				ptcsToken = _tcschr (ptcsToken, _T('>'));
				ptcsToken++;
				ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
				if (ptcsTokenEnd) {
					int  iLoop;

					*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
					for (iLoop = 0; UifLineTags_barcode_code[iLoop].tcsMnemonic; iLoop++) {
						if (_tcscmp (UifLineTags_barcode_code[iLoop].tcsMnemonic, ptcsToken) == 0) {
							UifPrinterBarcodeSettings.ulSymbology = UifLineTags_barcode_code[iLoop].ulValue;
							break;
						}
					}
				}
			}
			else if ((ptcsToken = _tcsstr (aszLine, _T("<halign"))) != 0) {
				ptcsToken = _tcschr (ptcsToken, _T('>'));
				ptcsToken++;
				ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
				if (ptcsTokenEnd) {
					int  iLoop;

					*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
					for (iLoop = 0; UifLineTags_barcode_halign[iLoop].tcsMnemonic; iLoop++) {
						if (_tcscmp (UifLineTags_barcode_halign[iLoop].tcsMnemonic, ptcsToken) == 0) {
							UifPrinterBarcodeSettings.ulAlignment = UifLineTags_barcode_halign[iLoop].ulValue;
							break;
						}
					}
				}
			}
		}
		sRetStatus = ReadAndProvideFileLine (pMyXmlFileControl, aszLine, usLineChars);
	}

	return sRetStatus;
}

static int ishexdigit (TCHAR tcsValue)
{
	int iRetStatus = 1;
	if ( !isdigit (tcsValue)) {
		iRetStatus = 0;
		if (tcsValue >= _T('A') && tcsValue <= _T('F'))
			iRetStatus = 1;
		else if (tcsValue >= _T('a') && tcsValue <= _T('f'))
			iRetStatus = 1;
	}
	return iRetStatus;
}

static TCHAR *ProcessXmlReceiptPrinterInitString (TCHAR *cTempBuffer, TCHAR *ptcsToken)
{
	while (*ptcsToken) {
		switch (*ptcsToken) {
			case 0:
				*cTempBuffer++ = *ptcsToken;
				return ptcsToken;
			case _T('\\'):
				// escape sequence begins
				ptcsToken++;
				switch (*ptcsToken) {
					case 0:
						*cTempBuffer++ = *ptcsToken;
						return ptcsToken;
					
					case _T('x'):
					case _T('X'):
						// begin hex string of 2 hex digits
						{
							TCHAR tcValue = 0;
							ptcsToken++;
							if (ishexdigit (*ptcsToken)) {
								if (isdigit(*ptcsToken))
									tcValue += ((*ptcsToken++ & 0x3f) - _T('0'));
								else if (*ptcsToken < _T('Z'))
									tcValue += ((*ptcsToken++ & 0x4f) - _T('A')) + 10;
								else if (*ptcsToken < _T('z'))
									tcValue += ((*ptcsToken++ & 0x6f) - _T('a')) + 10;
								if (ishexdigit (*ptcsToken)) {
									tcValue *= 16;
									if (isdigit(*ptcsToken))
										tcValue += ((*ptcsToken++ & 0x3f) - _T('0'));
									else if (*ptcsToken < _T('Z'))
										tcValue += ((*ptcsToken++ & 0x4f) - _T('A')) + 10;
									else if (*ptcsToken < _T('z'))
										tcValue += ((*ptcsToken++ & 0x6f) - _T('a')) + 10;
								}
							}
							*cTempBuffer++ = tcValue;
						}
						break;
					case _T('0'):
						// begin octal string of 4 octal digits
						{
							TCHAR tcValue = 0;
							ptcsToken++;
							if (isdigit (*ptcsToken)) {
								tcValue += ((*ptcsToken++ & 0x3f) - _T('0'));
								if (isdigit (*ptcsToken)) {
									tcValue *= 8;
									tcValue += ((*ptcsToken++ & 0x3f) - _T('0'));
									if (isdigit (*ptcsToken)) {
										tcValue *= 8;
										tcValue += ((*ptcsToken++ & 0x3f) - _T('0'));
									}
								}
							}
							*cTempBuffer++ = tcValue;
						}
						break;

					case _T('n'):
					case _T('N'):
						ptcsToken++;
						*cTempBuffer++ = _T('\n');
						break;

					case _T('t'):
					case _T('T'):
						ptcsToken++;
						*cTempBuffer++ = _T('\t');
						break;

					case _T('r'):
					case _T('R'):
						ptcsToken++;
						*cTempBuffer++ = _T('\r');
						break;

					default:
						*cTempBuffer++ = *ptcsToken++;
						break;
				}
				break;
			default:
				*cTempBuffer++ = *ptcsToken++;
				break;
		}
	}

	*cTempBuffer = 0;
	return ptcsToken;
}

static SHORT ProcessXmlFileStartUpReceiptPrinterRules (ReadFileLineControl *pMyXmlFileControl, TCHAR *aszLine, USHORT usLineChars)
{
	TCHAR        *ptcsToken = 0, *ptcsTokenEnd = 0;
    SYSCONFIG    *pSysConfig = PifSysConfig();                    /* get system config */
	SHORT        sRetStatus;
	TCHAR        cTempBuffer[256];


	// assume there is no receipt printer initialization string
	memset (pSysConfig->tcsReceiptPrinterInitString, 0, sizeof(pSysConfig->tcsReceiptPrinterInitString));

	sRetStatus = ReadAndProvideFileLine (pMyXmlFileControl, aszLine, usLineChars);
	while (sRetStatus >= 0) {
		if (aszLine[0] != _T('#')) {
			if (_tcsstr (aszLine, _T("/receiptprinter")) != 0) {
				break;
			}

			if (0 == _tcschr (aszLine, _T('<'))) {
				// ignore any line that does not have an XML tag on it.
				sRetStatus = ReadAndProvideFileLine (pMyXmlFileControl, aszLine, usLineChars);
				continue;
			}

			if ((ptcsToken = _tcsstr (aszLine, _T("<initstring"))) != 0) {
				ptcsToken = _tcschr (ptcsToken, _T('>'));
				ptcsToken++;
				ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
				if (ptcsTokenEnd) {
					*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
					ProcessXmlReceiptPrinterInitString (cTempBuffer, ptcsToken);
					cTempBuffer[32] = 0;
					_tcscpy (pSysConfig->tcsReceiptPrinterInitString, cTempBuffer);
					PrtPrint(PMG_PRT_RECEIPT, cTempBuffer, tcharlen(cTempBuffer));
				}
			}
		}
		sRetStatus = ReadAndProvideFileLine (pMyXmlFileControl, aszLine, usLineChars);
	}

	return sRetStatus;
}

static SHORT ProcessXmlFileStartUpCustomerSettings (ReadFileLineControl *pMyXmlFileControl, TCHAR *aszLine, USHORT usLineChars)
{
	SHORT        sRetStatus;
	TCHAR        *ptcsToken = 0, *ptcsTokenEnd = 0;
    SYSCONFIG    *pSysConfig = PifSysConfig();                    /* get system config */

	// assume there is no receipt printer initialization string
	memset (pSysConfig->tcsReceiptPrinterInitString, 0, sizeof(pSysConfig->tcsReceiptPrinterInitString));

	sRetStatus = ReadAndProvideFileLine (pMyXmlFileControl, aszLine, usLineChars);
	while (sRetStatus >= 0) {
		if (aszLine[0] != _T('#')) {
			if (_tcsstr (aszLine, _T("/customersettings")) != 0) {
				break;
			}

			if (0 == _tcschr (aszLine, _T('<'))) {
				// ignore any line that does not have an XML tag on it.
				sRetStatus = ReadAndProvideFileLine (pMyXmlFileControl, aszLine, usLineChars);
				continue;
			}

			if ((ptcsToken = _tcsstr (aszLine, _T("<amtraksettings"))) != 0) {
				USHORT  usType = 0;

				ptcsToken = _tcschr (ptcsToken, _T('>'));
				ptcsToken++;
				ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
				if (ptcsTokenEnd) {
					*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
				}
				usType = _ttoi (ptcsToken);
				if (usType) {
					pSysConfig->ulCustomerSettingsFlags |= SYSCONFIG_CUSTOMER_ENABLE_AMTRAK;
					NHPOS_NONASSERT_NOTE ("==STATE", "==STATE: ProcessXmlFileStartUpCustomerSettings(): SYSCONFIG_CUSTOMER_ENABLE_AMTRAK set.");
				} else {
					pSysConfig->ulCustomerSettingsFlags &= ~SYSCONFIG_CUSTOMER_ENABLE_AMTRAK;
				}
			} else if ((ptcsToken = _tcsstr (aszLine, _T("<uscbpsettings"))) != 0) {
				USHORT  usType = 0;

				ptcsToken = _tcschr (ptcsToken, _T('>'));
				ptcsToken++;
				ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
				if (ptcsTokenEnd) {
					*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
				}
				usType = _ttoi (ptcsToken);
				if (usType) {
					pSysConfig->ulCustomerSettingsFlags |= SYSCONFIG_CUSTOMER_ENABLE_USCBP;
					NHPOS_NONASSERT_NOTE ("==STATE", "==STATE: ProcessXmlFileStartUpCustomerSettings(): SYSCONFIG_CUSTOMER_ENABLE_USCBP set.");
				} else {
					pSysConfig->ulCustomerSettingsFlags &= ~SYSCONFIG_CUSTOMER_ENABLE_USCBP;
				}
			} else if ((ptcsToken = _tcsstr (aszLine, _T("<rel_21_stylesettings"))) != 0) {
				USHORT  usType = 0;

				ptcsToken = _tcschr (ptcsToken, _T('>'));
				ptcsToken++;
				ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
				if (ptcsTokenEnd) {
					*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
				}
				usType = _ttoi (ptcsToken);
				if (usType) {
					pSysConfig->ulCustomerSettingsFlags |= SYSCONFIG_CUSTOMER_ENABLE_REL_21_STYLE;
					NHPOS_NONASSERT_NOTE ("==STATE", "==STATE: ProcessXmlFileStartUpCustomerSettings(): SYSCONFIG_CUSTOMER_ENABLE_REL_21_STYLE set.");
				} else {
					pSysConfig->ulCustomerSettingsFlags &= ~SYSCONFIG_CUSTOMER_ENABLE_REL_21_STYLE;
				}
			}
			else if ((ptcsToken = _tcsstr(aszLine, _T("<mercurypaymenttest"))) != 0) {
				// added new PARAMINI customer setting for Mercury Payment testing. Must be enabled for testing with
				// Mercury Payments development kit to trigger special operator id of "test" in XML messages.
				// New defines for flags: SYSCONFIG_CUSTOMER_ENABLE_MERCURYPAYTEST and EEPTREQDATA_SPECIALFLAGS_MERCURYPAYTEST.
				// NOTE: per conversation with Paul Stewart, Vantiv/WorldPay/Mercury this is no longer necessary for testing.
				//       Richard Chambers, May-23-2018
				USHORT  usType = 0;

				ptcsToken = _tcschr(ptcsToken, _T('>'));
				ptcsToken++;
				ptcsTokenEnd = _tcschr(ptcsToken, _T('<'));
				if (ptcsTokenEnd) {
					*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
				}
				usType = _ttoi(ptcsToken);
				if (usType) {
					pSysConfig->ulCustomerSettingsFlags |= SYSCONFIG_CUSTOMER_ENABLE_MERCURYPAYTEST;
					NHPOS_NONASSERT_NOTE("==STATE", "==STATE: ProcessXmlFileStartUpCustomerSettings(): SYSCONFIG_CUSTOMER_ENABLE_MERCURYPAYTEST set.");
				}
				else {
					pSysConfig->ulCustomerSettingsFlags &= ~SYSCONFIG_CUSTOMER_ENABLE_MERCURYPAYTEST;
				}
			}
			else if ((ptcsToken = _tcsstr(aszLine, _T("<notallowpartialauth"))) != 0) {
				// added new PARAMINI customer setting for turning off EPT Partial Authorization.
				// New defines for flags: SYSCONFIG_CUSTOMER_ENABLE_NO_PARTIAL_AUTH and EEPTREQDATA_SPECIALFLAGS_NO_PARTIAL_AUTH.
				USHORT  usType = 0;

				ptcsToken = _tcschr(ptcsToken, _T('>'));
				ptcsToken++;
				ptcsTokenEnd = _tcschr(ptcsToken, _T('<'));
				if (ptcsTokenEnd) {
					*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
				}
				usType = _ttoi(ptcsToken);
				if (usType) {
					pSysConfig->ulCustomerSettingsFlags |= SYSCONFIG_CUSTOMER_ENABLE_NO_PARTIAL_AUTH;
					NHPOS_NONASSERT_NOTE("==STATE", "==STATE: ProcessXmlFileStartUpCustomerSettings(): SYSCONFIG_CUSTOMER_ENABLE_NO_PARTIAL_AUTH set.");
				}
				else {
					pSysConfig->ulCustomerSettingsFlags &= ~SYSCONFIG_CUSTOMER_ENABLE_NO_PARTIAL_AUTH;
				}
			}
		}
		sRetStatus = ReadAndProvideFileLine (pMyXmlFileControl, aszLine, usLineChars);
	}

	return sRetStatus;
}

static SHORT ProcessXmlFileLoggingRules (ReadFileLineControl *pMyXmlFileControl, TCHAR *aszLine, USHORT usLineChars)
{
	SHORT        sRetStatus;
	TCHAR        *ptcsToken = 0, *ptcsTokenEnd = 0;
    SYSCONFIG    *pSysConfig = PifSysConfig();                    /* get system config */

	// This variable indicates various kinds of logging to be done.
	// this variable is actually a bit map whose bits indicate various settings.
	// we initialize it to zero and then add in bits depending on directives within
	// the loggingrules section of the PARAMINI file.
	pSysConfig->ausOption[UIE_LOG_OPTION6] = 0;

	sRetStatus = ReadAndProvideFileLine (pMyXmlFileControl, aszLine, usLineChars);
	while (sRetStatus >= 0) {
		if (aszLine[0] != _T('#')) {
			if (_tcsstr (aszLine, _T("/loggingrules")) != 0) {
				break;
			}

			if (0 == _tcschr (aszLine, _T('<'))) {
				// ignore any line that does not have an XML tag on it.
				sRetStatus = ReadAndProvideFileLine (pMyXmlFileControl, aszLine, usLineChars);
				continue;
			}

			if ((ptcsToken = _tcsstr (aszLine, _T("<transactions>"))) != 0) {
				ptcsToken = _tcschr (ptcsToken, _T('>'));
				ptcsToken++;
				ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
				if (ptcsTokenEnd) {
					*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
					pSysConfig->ausOption[UIE_LOG_OPTION6] |= (_ttoi (ptcsToken) & 0x000f);
				}
			}
		}
		sRetStatus = ReadAndProvideFileLine (pMyXmlFileControl, aszLine, usLineChars);
	}

	return sRetStatus;
}

SHORT  RflLoggingRulesTransactions (void)
{
    CONST SYSCONFIG *pSysConfig = PifSysConfig();                    /* get system config */
	return (pSysConfig->ausOption[UIE_LOG_OPTION6] & 0x000f);
}

static SHORT ProcessXmlFileStartUpSpecialDirectives (ReadFileLineControl *pMyXmlFileControl, TCHAR *aszLine, USHORT usLineChars)
{
	SYSCONFIG    *pSysConfig = PifSysConfig();                    /* get system config */
	SHORT        sRetStatus = 0;
	USHORT       usLen, usLoadLogoDelay = 0;
	TCHAR        *ptcsToken = 0, *ptcsTokenEnd = 0;

	if ((ptcsToken = _tcsstr (aszLine, _T("addlogoext"))) != 0) {
		ptcsToken = _tcschr (ptcsToken, _T('>'));
		ptcsToken++;
		ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
		if (ptcsTokenEnd) {
			*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
		}

		memset (pSysConfig->tcsLogoExtension, 0, sizeof(pSysConfig->tcsLogoExtension));
		usLen = tcharlen(ptcsToken);
		if (usLen > 0) {
			pSysConfig->tcsLogoExtension[0] = _T('.');
			_tcsncpy (pSysConfig->tcsLogoExtension + 1, ptcsToken, 5);
		}
	} else if ((ptcsToken = _tcsstr (aszLine, _T("logoloaddelay"))) != 0) {
		ptcsToken = _tcschr (ptcsToken, _T('>'));
		ptcsToken++;
		ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
		if (ptcsTokenEnd) {
			*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
		}

		usLen = tcharlen(ptcsToken);
		pSysConfig->usLoadLogoDelay = 0;
		if (usLen > 0) {
			pSysConfig->usLoadLogoDelay = _ttoi (ptcsToken);
		}
	} else if ((ptcsToken = _tcsstr (aszLine, _T("<terminalidkeyword>"))) != 0) {
		ptcsToken = _tcschr (ptcsToken, _T('>'));
		ptcsToken++;
		ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
		if (ptcsTokenEnd) {
			*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
		}
		memset (pSysConfig->tcsTerminalIdKeyword, 0, sizeof(pSysConfig->tcsTerminalIdKeyword));
		_tcsncpy (pSysConfig->tcsTerminalIdKeyword, ptcsToken, TCHARSIZEOF(pSysConfig->tcsTerminalIdKeyword) - 1);
	}

	return sRetStatus;
}


// The following functionality is to provide a thread which reads a file
// processing the directives in the file.
// Basic directives in the file are:
//   <startupprocess>
VOID THREADENTRY UifProcessIniFile (VOID)
{
	ReadFileLineControl  myXmlFileControl;
	TCHAR   *aszIniFile = _T("PARAMINI");
	SHORT   hsParamini;
	USHORT  usProcessingDelay = 5000;

	hsParamini = PifOpenFile (aszIniFile, "or");  // open PARAMINI

	if (hsParamini >= 0) {
		UCHAR  myFileBuffer[4098];
		TCHAR  aszLine[512];
		USHORT usLineChars = 512;
		SHORT  sStatus;
		USHORT usLen;
		TCHAR  *ptcsToken = 0, *ptcsTokenEnd = 0;

		// The first step in this process of the PARAMINI file is to look to see if the
		// file contains a directive to change the standard delay before starting to process
		// the file.  If the directive is not the first one found, ignoring comments and blank lines,
		// then we will just use the default time out.  If we find the directive then we will
		// make sure that it is a reasonable number and them change it from seconds to milliseconds
		// and then use this in the argument for PifSleep() for our startup delay.
		ReadAndProvideFileLineInit (&myXmlFileControl, hsParamini, myFileBuffer, sizeof(myFileBuffer));

		sStatus = ReadAndProvideFileLine (&myXmlFileControl, aszLine, usLineChars);
		while (sStatus >= 0) {
			if (aszLine[0] != _T('#')) {
				if ((ptcsToken = _tcsstr (aszLine, _T("<inidelay>"))) != 0) {
					ptcsToken = _tcschr (ptcsToken, _T('>'));
					ptcsToken++;
					ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
					if (ptcsTokenEnd) {
						*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
					}

					usLen = tcharlen(ptcsToken);
					if (usLen > 0) {
						USHORT  usDelayTemp = _ttoi (ptcsToken);

						if (usDelayTemp > 0 && usDelayTemp < 20)
							usProcessingDelay = usDelayTemp * 1000;
					}
					break;
				}

				// If we find a directive that is not a delay directive then we are done.
				// The delay directive should be the first one found.
				if (_tcsstr (aszLine, _T("<")) != 0)
					break;
			}
			sStatus = ReadAndProvideFileLine (&myXmlFileControl, aszLine, usLineChars);
		}

		PifCloseFile (hsParamini);
	}

	// give application time to complete startup and stablize
	// then begin processing the initalization file
	PifSleep (usProcessingDelay);

	hsParamini = PifOpenFile (aszIniFile, "or");  // open PARAMINI

	if (hsParamini >= 0) {
		UCHAR  myFileBuffer[4098];
		TCHAR  aszLine[512];
		USHORT usLineChars = 512;
		SHORT  sStatus;

		ReadAndProvideFileLineInit (&myXmlFileControl, hsParamini, myFileBuffer, sizeof(myFileBuffer));

		sStatus = ReadAndProvideFileLine (&myXmlFileControl, aszLine, usLineChars);
		while (sStatus >= 0) {
			if (aszLine[0] != _T('#') && aszLine[0] != 0) {
				if (_tcsstr (aszLine, _T("startupprocess")) != 0) {
					ProcessXmlFileStartUpProcessRules (&myXmlFileControl, aszLine, usLineChars);
				} else if (_tcsstr (aszLine, _T("barcode")) != 0) {
					ProcessXmlFileStartUpBarcodeRules (&myXmlFileControl, aszLine, usLineChars);
				} else if (_tcsstr (aszLine, _T("receiptprinter")) != 0) {
					ProcessXmlFileStartUpReceiptPrinterRules (&myXmlFileControl, aszLine, usLineChars);
				} else if (_tcsstr (aszLine, _T("customersettings")) != 0) {
					ProcessXmlFileStartUpCustomerSettings (&myXmlFileControl, aszLine, usLineChars);
				} else if (_tcsstr (aszLine, _T("frameworkxml")) != 0) {
					ProcessXmlFileStartUpFrameworkXml (&myXmlFileControl, aszLine, usLineChars);
				} else if (_tcsstr (aszLine, _T("loggingrules")) != 0) {
					ProcessXmlFileLoggingRules (&myXmlFileControl, aszLine, usLineChars);
				} else if (_tcsstr (aszLine, _T("historicalfolder")) != 0) {
					ProcessXmlFileHistoricalFolder (&myXmlFileControl, aszLine, usLineChars);
				} else if (_tcsstr(aszLine, _T("webfolder")) != 0) {
					ProcessXmlFileWebFolder(&myXmlFileControl, aszLine, usLineChars);
				} else if (_tcsstr(aszLine, _T("printfolder")) != 0) {
					ProcessXmlFilePrintFolder(&myXmlFileControl, aszLine, usLineChars);
				} else {
					ProcessXmlFileStartUpSpecialDirectives (&myXmlFileControl, aszLine, usLineChars);
				}
			}
			sStatus = ReadAndProvideFileLine (&myXmlFileControl, aszLine, usLineChars);
		}

		PifCloseFile (hsParamini);
	}

	if (RflLoggingRulesTransactions()) {
		CONST SYSCONFIG    *pSysConfig = PifSysConfig();                    /* get system config */
		char          transBuff[256];

		sprintf (transBuff, "System Startup|%S|%d", tcsFrameworkVersionNumber, PIF_CLUSTER_MASK_TERMINAL_NO(pSysConfig->usTerminalPositionFromName));
		PifTransactionLog (0, transBuff, __FILE__, __LINE__);
	}

	BlFwIfInitLoadLogo ();
}


VOID  UifRequestSharedPrintFile (TCHAR *aszFileName)
{
	static  TCHAR aszFileNameBuffer[UIF_PRINT_QUEUE_MAX * 64];
	static  TCHAR *aszFileNamePtr = aszFileNameBuffer;
	int           iLen;

	_tcscpy (aszFileNamePtr, aszFileName);

	PifWriteQueue(sUifQueueSharedPrintFile, aszFileNamePtr);

	iLen = _tcslen(aszFileName) + 1;  // get the string length plus the terminating end of string character
	aszFileNamePtr += iLen;           // advance pointer past the end of string of last file name put on queue

	if (aszFileNamePtr + 56 >= aszFileNameBuffer + sizeof(aszFileNameBuffer)/sizeof(aszFileNameBuffer[0]))
		aszFileNamePtr = aszFileNameBuffer;
}

VOID  UifRequestSharedLuaFile (TCHAR *aszFileName)
{
#if defined(ENABLE_LUA_SCRIPTING)

	static  TCHAR aszFileNameBuffer[2048];
	static  TCHAR *aszFileNamePtr = aszFileNameBuffer;
	int           iLen;

	_tcscpy (aszFileNamePtr, aszFileName);

	PifWriteQueue(sUifQueueSharedLuaFile, aszFileNamePtr);

	iLen = _tcslen(aszFileName) + 1;  // get the string length plus the terminating end of string character
	aszFileNamePtr += iLen;           // advance pointer past the end of string of last file name put on queue

	if (aszFileNamePtr + 56 >= aszFileNameBuffer + sizeof(aszFileNameBuffer)/sizeof(aszFileNameBuffer[0]))
		aszFileNamePtr = aszFileNameBuffer;
#endif
}

#define _WIN32_WINNT 0x0400
VOID THREADENTRY UifWatchPrintFilesFolder (VOID)
{
	FILE_NOTIFY_INFORMATION  *pFileInfo;
	TCHAR      *szDirectory = STD_FOLDER_PRINTFOLDER;
	DWORD      dwNotifyFilter = (FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE);
	DWORD      dwBufferBytesReturned = 0;
	HANDLE     hDir;
	int        iListIndex;
	TCHAR      myFileList[UIF_PRINT_QUEUE_MAX][64];
	BYTE       szBuffer[UIF_PRINT_QUEUE_MAX * 1024];

	hDir = CreateFile(szDirectory, FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

	// We have attached to the folder that we want to watch for events and notifications.
	// Next we begin a loop to accept Windows file system notifications for the PrintFiles folder.
	// If there is an error then we will issue an ASSRTLOG.
	// The reason for the FlushFileBuffers() function is to persuade Windows to send any pending notifications.
	// On the Internet there appears to be some concerns that Windows will not always send notifications
	// for every file change or that Windows will not immediately send notifications and that the use
	// of the FlushFileBuffers() function does help flush any pending notifications.
	while (hDir != INVALID_HANDLE_VALUE) {
		if (0 == ReadDirectoryChangesW (hDir, szBuffer, sizeof(szBuffer), FALSE, dwNotifyFilter, &dwBufferBytesReturned, NULL, NULL)) {
			char  xBuff[128];

			sprintf (xBuff, "UifWatchPrintFilesFolder(): ReadDirectoryChangesW() failed. GetLastError = %d", GetLastError());
			NHPOS_ASSERT_TEXT(0, xBuff);
			FlushFileBuffers(hDir);
			continue;
		}
		iListIndex = 0;
		pFileInfo = (FILE_NOTIFY_INFORMATION *)szBuffer;
		while (1) {
			if (pFileInfo->Action == FILE_ACTION_ADDED || pFileInfo->Action == FILE_ACTION_MODIFIED) {
				USHORT i;
				for (i = 0; i < pFileInfo->FileNameLength/sizeof(WCHAR); i++) {
					myFileList[iListIndex][i] = pFileInfo->FileName[i];
				}
				myFileList[iListIndex][i] = 0;
				iListIndex++;
			}
			if (pFileInfo->NextEntryOffset == 0)
				break;
			pFileInfo = (FILE_NOTIFY_INFORMATION  *)((BYTE *)pFileInfo + pFileInfo->NextEntryOffset);
		}
		if (iListIndex > 0) {
			int i;
			for (i = 0; i < iListIndex; i++) {
				UifRequestSharedPrintFile (myFileList[i]);
			}
		}
		FlushFileBuffers(hDir);
	}
}

/*
*===========================================================================
** Synopsis:    VOID THREADENTRY UifCreateMain(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Create Application
*===========================================================================
*/
VOID THREADENTRY UifCreateMain(VOID)
{
	SHORT           sError = 0;
    USHORT          usReturnLen;
    USHORT          usReturn;
	USHORT          usConsecNo;
    FLEXMEM         FlexMemPara[MAX_FLXMEM_SIZE];
    USHORT          i;

	NHPOS_NONASSERT_NOTE("==STATE", "==STARTUP STATE:  UifCreateMain() start.");

    usReturn = CliEJGetStatus();
	if(!( 0 == _tchdir(STD_FOLDER_LOGFOLDER)))
	{
		_tmkdir(STD_FOLDER_LOGFOLDER);
	}
	if(!( 0 == _tchdir(STD_FOLDER_WEBFOLDER)))
	{
		_tmkdir(STD_FOLDER_WEBFOLDER);
	}
	if(!( 0 == _tchdir(STD_FOLDER_PRINTFOLDER)))
	{
		_tmkdir(STD_FOLDER_PRINTFOLDER);
	}
	if (!(0 == _tchdir(STD_FOLDER_QUERYFOLDER)))
	{
		_tmkdir(STD_FOLDER_QUERYFOLDER);
	}

	//  Log the fact that we are starting up into the PifLog
	//  Next we count up the Consecutive Number in case we are recovering from a power fail
	//  and we want to make sure that we are not using the same consecutive number from a
	//  previous transaction or action because the consecutive number counter was not stored
	//  before the power failure or other sudden event.
	PifLog(MODULE_SYS_STARTUP, MODULE_SYS_STARTUP);
    usConsecNo = MaintIncreSpcCo(SPCO_CONSEC_ADR);        /* Count Up Consecutive No. to prevent duplicate numbers from power fail recovery */

    /* --- Create/Open Temporary File for CP/EPT,   Saratoga --- */
	/* correct the order, 08/31/01 */
    if (ItemTendCPCreateFile() < 0) {
        PifAbort(MODULE_UI, LOG_ERROR_UIC_TF_CP_CREATE_FILE);
    } else {
        if (ItemTendCPOpenFile() != SUCCESS) {
            PifAbort(MODULE_UI, LOG_ERROR_UIC_TF_CP_OPEN_FILE);
        }
		ItemTendCPClearFile();
    }

#if 0
	// This change to create the additional PARAM files will probably be needed
	// for the future.  however we are unsure how it would affect Amtrak at this
	// point.
	// Check to see if the Additional PARAM files exist.  If not then create them
	// with a basic comment line to indicate that they are a placeholder.
	// See function OpReqAdditionalParams() which handle Additional PARAM file transfer.
	{
		int     i = 0;
		SHORT   hsFileHandle;
		TCHAR   *tcsParamFileNames[] = {
					_T("PARAMINI"),      // parameter INI file with initialization data
					_T("PARAMTDR"),      // tender parameter data file with tender rules
					0
				};
		for (i = 0; tcsParamFileNames[i]; i++) {
			hsFileHandle = PifOpenFile(tcsParamFileNames[i], auchNEW_FILE_READ_WRITE);    /* saratoga */
			if (hsFileHandle >= 0) {
				char  xBuff[128];
				sprintf (xBuff, "# create placeholder new file %S\n", tcsParamFileNames[i]);
				PifWriteFile (hsFileHandle, xBuff, strlen(xBuff));
				PifCloseFile (hsFileHandle);
			}
		}
	}
#endif

	// Process the PARAMTDR file for the tender account rules
	ProcessXmlFileTenderRules ();

    CliInitialize(usUifPowerUp);            /* initialize Stub */

	// The Flexible memory settings retrieved with CLASS_PARAFLEXMEM and the file sizes checked, MAX_FLXMEM_SIZE
	// See the defines for the Flexible Memory table indices starting with FLEX_DEPT_ADR.
	// See the defines for max sizes, max value for ulRecordNumber, starting with FLEX_DEPT_MAX.
	// For remote application interface see functions IspCheckAndCreate() and IspCreateFiles() and
	// IspCheckDataFlexMem().
	//
	// Following assignment is a throwaway to allow using IDE to assist with finding these defines.
	i = FLEX_DEPT_ADR; i = FLEX_DEPT_MAX;    // throwaway assigns to allow for use of "Go To Declaration" in IDE
    CliParaAllRead(CLASS_PARAFLEXMEM, (UCHAR *)FlexMemPara, sizeof(FlexMemPara), 0, &usReturnLen);       /* get flex parameter */
    for (i = 0; i < MAX_FLXMEM_SIZE; i++) {
        sError = CliCheckDeleteFile((UCHAR)(i + 1), FlexMemPara[i].ulRecordNumber, FlexMemPara[i].uchPTDFlag);
        if (sError < 0) { /* delete file */
            PifLog(MODULE_UI, LOG_ERROR_UIC_TF_FLEX_FILE);
            PifLog(MODULE_DATA_VALUE(MODULE_UI), i);
            PifLog(MODULE_ERROR_NO(MODULE_UI), (USHORT)abs(sError));
			PifLog(MODULE_LINE_NO(MODULE_UI), (USHORT)__LINE__);
            PifAbort(MODULE_UI, LOG_ERROR_UIC_TF_CHECK_DELETE);
        }
    }
    for (i = 0; i < MAX_FLXMEM_SIZE; i++) {
        sError = CliCheckCreateFile((UCHAR)(i + 1), FlexMemPara[i].ulRecordNumber, FlexMemPara[i].uchPTDFlag);
		{
			char  xBuff[128];

			sprintf (xBuff, "  CliCheckCreateFile: File %d  Size = %d  PTD %d  sError %d", i + 1, FlexMemPara[i].ulRecordNumber, FlexMemPara[i].uchPTDFlag, sError);
			NHPOS_NONASSERT_NOTE("==STATE", xBuff);
		}
        if (sError < 0) { /* create file */
            PifLog(MODULE_UI, LOG_ERROR_UIC_TF_FLEX_FILE);
            PifLog(MODULE_DATA_VALUE(MODULE_UI), i);
            PifLog(MODULE_ERROR_NO(MODULE_UI), (USHORT)abs(sError));
			PifLog(MODULE_LINE_NO(MODULE_UI), (USHORT)__LINE__);
            PifAbort(MODULE_UI, LOG_ERROR_UIC_TF_CHECK_CREATE);
        }
    }

	{
		MAINTHEADER    MaintHeader = {0};
		SHORT  sRetStatus = 0;

		sRetStatus = CliEjConsistencyCheck ();

		if (usReturn == EJ_THERMAL_NOT_EXIST) {             /* Not Thermal System */
			ItemOtherPowerDownLog(FSC_MINOR_POWER_DOWN);    /* write AC Lost Log, saratoga */
		}

		/* initialize structure */
		MaintHeader.uchMajorClass = CLASS_MAINTHEADER;
		MaintHeader.usPrtControl = PRT_JOURNAL;
		MaintHeader.uchMinorClass = 0;
		_tcscpy (MaintHeader.aszReportName1, _T("RESET"));
		PrtPrintItem(NULL, &MaintHeader);

        if (usUifPowerUp & POWER_UP_CLEAR_WHOLE_MEMORY) { /* whole memory clear ? */
            PrtPrintf(PMG_PRT_JOURNAL, aUifInitialResetPrint);  /* print initial reset */
        } else if (usUifPowerUp & IF_PARA_LOADED_PARA1) { /* loader ? */
            PrtPrintf(PMG_PRT_JOURNAL, aUifLoaderPrint);    /* print loader */
        } else if (usUifPowerUp & POWER_UP_CLEAR_TOTAL) { /* total clear ? */
			USHORT   ausParaCounter[MAX_CO_SIZE] = {0};

            PrtPrintf(PMG_PRT_JOURNAL, aUifTotalResetPrint);    /* print total reset */
            CliParaAllWrite(CLASS_PARASPCCO, (UCHAR *)ausParaCounter, sizeof(ausParaCounter), 0, &usReturnLen);       /* clear special counter */
        } else {
            PrtPrintf(PMG_PRT_JOURNAL, aUifMasterResetPrint);   /* print master reset */
        }
	}

	{
		MAINTTRAILER   MaintTrailer = {0};
		DATE_TIME      DateAndTime = {0};
		PARASTOREGNO   ParaStoRegNox = {0};

		ParaStoRegNox.uchMajorClass = CLASS_PARASTOREGNO;
		CliParaRead(&ParaStoRegNox);  /* call ParaStoRegNoRead() */

		/* initialize structure */
		MaintTrailer.uchMajorClass = CLASS_MAINTTRAILER;
		MaintTrailer.usPrtControl = PRT_JOURNAL;
		MaintTrailer.uchMinorClass = 0;
		MaintTrailer.usConsCount = usConsecNo;
		MaintTrailer.usStoreNumber = ParaStoRegNox.usStoreNo;
		MaintTrailer.usRegNumber = ParaStoRegNox.usRegisterNo;

		PifGetDateTime (&DateAndTime);
		MaintTrailer.usMin = DateAndTime.usMin;
		MaintTrailer.usHour = DateAndTime.usHour;
		MaintTrailer.usMDay = DateAndTime.usMDay;
		MaintTrailer.usMonth = DateAndTime.usMonth;
		MaintTrailer.usYear = DateAndTime.usYear;
		PrtPrintItem(NULL, &MaintTrailer);
	}

	/* create total update thread */
	PifBeginThread(TtlTUMMain, 0, 0, PRIO_TOTALS, "TOTALS", NULL);

	/*	TtlTUMJoinDisconnectedSatellite() Transfer Totals for Join of AC444
		create total update thread for Join command from a Disconnected Satellite
		rather than the totals update thread priority we are going to use the
		standard priority in order for it to finish quickly. */
	PifBeginThread(TtlTUMJoinDisconnectedSatellite, 0, 0, PRIO_MAIN, "TOTALSJOIN", NULL);

	/* create total update thread for update totals messages used by Store and Forward */
	PifBeginThread(TtlTUMUpdateTotalsMsg, 0, 0, PRIO_TOTALS, "TOTALSUPDATE", NULL);

	/* correct pifbeginthread order, 08/31/01 */
	PifBeginThread(KpsPrintProcess, 0, 0, PRIO_KPS_PRT, "KPS_PRT", NULL);  /* Add R3.0 */

	PifBeginThread(KpsTimerProcess, 0, 0, PRIO_KPS_MON, "KPS_MON", NULL);  /* Add R3.0 */

	PifBeginThread(FDTMain, 0, 0, PRIO_FDT, "FDT", NULL);    /* Flexible Drive Thru Thread Name */

	/* correct pifbeginthread order, 08/31/01 */
	PifBeginThread(IspMain, 0, 0, PRIO_SERVER_W, "SERVER_W", NULL);  /* Add R3.0 */

	PifBeginThread(SerStartUp, 0, 0, PRIO_SERVER_T, "SERVER_T", NULL);    /* Add R3.0 */

	// Remove MSR and PinPad event handlers as part of eliminating legacy MSR code for OpenGenPOS.
	// This is to eliminate the possibility of not meeting PCI-DSS account holder information
	// security requirements. GenPOS Rel 2.3.0 retained legacy source but was using the
	// Datacap Out Of Scope control for electronic payment.
	//
	//    OpenGenPOS Rel 2.4.0  3/22/2021  Richard Chambers

	UifCreateQueueStringWaitEvent();

	UifRequestSharedPrintFileInit();
	PifBeginThread(UifSharedPrintFileThread, 0, 0, PRIO_SERVER_T, "PRINTFILE", NULL);

#if defined(ENABLE_LUA_SCRIPTING)
	{
		VOID (THREADENTRY *pFunc)(VOID) = UifSharedLuaFileThread;
		CHAR CONST  *szUifSharedLuaFileName = "LUAFILE";

		UifRequestSharedLuaFileInit();
		PifBeginThread(pFunc, 0, 0, PRIO_SERVER_T, szUifSharedLuaFileName);
	}
#endif

	/* Total Update for Join from Disconnected Satellite */
	PifBeginThread(UifProcessIniFile, 0, 0, PRIO_TOTALS, "INI_FILE", NULL);   /* Add R3.0 */

	/* Add GenPOS Rel 2.2.0 for Amtrak and Inventory */
	/* print watch for files dropped into Printfiles folder */
	PifBeginThread(UifWatchPrintFilesFolder, 0, 0, PRIO_TOTALS, "PRINTWATCH", NULL);   /* create print folder watch thread */

    /* --- Create/Open Temporary File for CP/EPT,   Saratoga --- */
    /* if (ItemTendCPCreateFile() < 0) {
        PifAbort(MODULE_UI, FAULT_ERROR_CREATE_FILE);
    } else {
        if (ItemTendCPOpenFile() != SUCCESS) {
            PifAbort(MODULE_UI, FAULT_ERROR_CREATE_FILE);
        }
    }*/

	/* Main Application thread name */
	PifBeginThread(UifMain, 0, 0, PRIO_MAIN, "MAIN", NULL);        /* Add R3.0 */

	{
		// Lets now initialize the printer logo files.  If they do not exist
		// we will create a dummy version of the files.
		SHORT  sTempFileHandle = 0;

		for( i = 0; i < 10; i++)
		{
			sTempFileHandle = PifOpenFile(aszPrintLogo[i], auchNEW_FILE_READ_WRITE);
			if (sTempFileHandle >= 0)
				PifCloseFile (sTempFileHandle);
		}
		sTempFileHandle = PifOpenFile(aszPrintLogoIndex, auchNEW_FILE_READ_WRITE);
		if (sTempFileHandle >= 0)
			PifCloseFile (sTempFileHandle);
		sTempFileHandle = PifOpenFile(_T("PARAMLOA"), auchNEW_FILE_READ_WRITE);
		if (sTempFileHandle >= 0)
			PifCloseFile (sTempFileHandle);
	}
	
	//We check to see if there is a debug file, and if there is, we will make a backup of it,
	//and then delete the original dump file. JHHJ 10-14-05
	PifDebugFileCleanUp();

	NHPOS_NONASSERT_NOTE("==STATE", "==STARTUP STATE:  UifCreateMain() completed.");

	CliParaSaveFarData();    // BlFinalizeAndClose() update memory resident data snapshot and the ASSRTLOG file offset pointers.

// RJC - not using this as a thread anymore    PifEndThread();
}

/*
*===========================================================================
** Synopsis:    VOID THREADENTRY UifMain(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Main Application Idle
*===========================================================================
*/
VOID THREADENTRY UifMain(VOID)
{
    KEYMSG Data;

	PifLog(MODULE_UI, LOG_EVENT_THREAD_START);
    PifLog(MODULE_LINE_NO(MODULE_UI), (USHORT)__LINE__);

	UieQueueStanzaInit();

    for (;;) {                              /* Endless Loop */
        UieGetMessage(&Data, 0x0000);
        if (Data.uchMsg == UIM_MODE) {      /* Mode Change */
            UifChangeMode(Data.SEL.uchMode);
        } else {

            UieCheckSequence(&Data);
        }
    }
}

// Remove MSR event handlers as part of eliminating legacy MSR code for OpenGenPOS.
// This is to eliminate the possibility of not meeting PCI-DSS account holder information
// security requirements. GenPOS Rel 2.3.0 retained legacy source but was using the
// Datacap Out Of Scope control for electronic payment.
//
//    OpenGenPOS Rel 2.4.0  3/22/2021  Richard Chambers


/*
*===========================================================================
** Synopsis:    VOID UifChangeMode(UCHAR uchMode)
*     Input:    uchMode - mode number
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Mode Change Routine
*===========================================================================
*/
VOID UifChangeMode(UCHAR uchMode)
{
    switch (uchMode) {
    case UIE_POSITION0:                                 /* Menu */
    case UIE_POSITION1:                                 /* Power Off */
    case UIE_POSITION2:                                 /* Lock */
        PifControlPowerSwitch(PIF_POWER_SWITCH_DISABLE);    /* V1.0.04 */
        UieInitMenu(aUifLock);
        break;

    case UIE_POSITION3:                                 /* Reg */
        PifControlPowerSwitch(PIF_POWER_SWITCH_DISABLE);    /* V1.0.04 */
        UieInitMenu(aUifReg);
        break;

    case UIE_POSITION4:                                 /* Supervisor */
        PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */
        UieInitMenu(aUifSuper);
        break;

    case UIE_POSITION5:                                 /* Program */
        PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */
        UieInitMenu(aUifProg);
        break;

    default:
		PifLog(MODULE_UI, FAULT_INVALID_DATA);
        PifLog(MODULE_LINE_NO(MODULE_UI), (USHORT)__LINE__);
        PifAbort(MODULE_UI, FAULT_INVALID_DATA);
    }
}

/*
*===========================================================================
** Synopsis:    SHORT UifLock(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     InOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: lock Mode Main Routine
*===========================================================================
*/
SHORT UifLock(KEYMSG *pData)
{
    switch (pData->uchMsg) {
    case UIM_INIT:
        UieCtrlDevice(UIE_DIS_WAITER);              /* set disable input device */
        MldRefresh();                               /* R3.0 */
		RFLUpdateDisplayByMajorMinorClass (CLASS_UIFREGDISP, CLASS_UIFDISP_LOCK, DISP_SAVE_TYPE_0);
        break;

    case UIM_INPUT:                                 /* V3.3 */
        switch (pData->SEL.INPUT.uchMajor) {
        /* --- PC i/F Reset Log Print,  V3.3 --- */
        case FSC_RESET_LOG:
            if (uchResetLog != 0) {
                if (ItemOtherResetLog(uchResetLog) == 0) {
                    uchResetLog = 0;
                }
            }
            break;

        case FSC_POWER_DOWN:                        /* Saratoga */
            if (uchPowerDownLog != 0) {
                uchPowerDownLog = pData->SEL.INPUT.uchMinor;
                if (ItemOtherPowerDownLog(uchPowerDownLog) == 0) {
                    uchPowerDownLog = 0;
                }
            }
            break;

        default:
			PifLog(MODULE_PIF,LOG_ERROR_PIF_WRNG_CLSS);
            break;
        }

        break;

    case UIM_ACCEPTED:
    case UIM_REJECT:
    case UIM_MODE:
		PifLog(MODULE_UI, FAULT_INVALID_DATA);
        PifLog(MODULE_LINE_NO(MODULE_UI), (USHORT)__LINE__);
        PifAbort(MODULE_UI, FAULT_INVALID_DATA);
        break;

    default:
		PifLog(MODULE_PIF,LOG_ERROR_PIF_WRNG_LOCK);/* not use */
        break;
    }
    return (SUCCESS);
}

/*
*==========================================================================
**  Synopsis:   VOID    UifCPPrintUns(VOID)
*   Input:
*   Output:     nothing
*   InOut:      nothing
*   Return:     nothing
*   Description:    Print Response Text, V3.3
*==========================================================================
*/
VOID    UifCPPrintUns(VOID)
{
    /* --- PC i/F Reset Log,    V3.3 --- */
    if (uchResetLog != 0) {
        if (ItemOtherResetLog(uchResetLog) == UIF_SUCCESS) {
            uchResetLog = 0;
        }
    }
}

/*
*==========================================================================
**  Synopsis:   VOID    UifCPRetryUns(VOID)
*   Input:
*   Output:     nothing
*   InOut:      nothing
*   Return:     nothing
*   Description:    Set Exsit Unsoli Data Status,   V3.3    Saratoga
*==========================================================================
*/
VOID    UifCPRetryUns(VOID)
{
    FSCTBL      Ring;
    UCHAR       uchDummy = 0;

    /* --- Retry for PC i/F Reset Log --- */
    if (uchResetLog != 0) {
        Ring.uchMajor = FSC_RESET_LOG;
        Ring.uchMinor = uchResetLog;
        UieWriteRingBuf(Ring, &uchDummy, sizeof(uchDummy));
    }
}

/*
*===========================================================================
** Synopsis:    VOID THREADENTRY UifCreateMain(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Create Application
*===========================================================================
*/
VOID THREADENTRY UifPowerMonitor(VOID)
{
    PifSemHandle  usPowerSem;

    usPowerSem = PifCreatePowerSem();

	PifLog(MODULE_UI, LOG_EVENT_THREAD_START);
    PifLog(MODULE_LINE_NO(MODULE_UI), (USHORT)__LINE__);

    for (;;) {
		USHORT      usEvent = PifRequestPowerSem(usPowerSem);

        if (usEvent == PIF_RETURN_FROM_SUSPEND) {
			USHORT      fsDevice = UIE_ENA_WAITER|UIE_ENA_DRAWER|UIE_ENA_SCANNER|UIE_ENA_MSR;
			PARAMDC     WorkMDC = { 0 };

            /* reset power down status */
            WorkMDC.uchMajorClass = CLASS_PARAMDC;
            WorkMDC.usAddress = MDC_POWERSTATE_INDIC;                   /* MDC 400 */
            CliParaRead( &WorkMDC );                                    /* get target MDC */
            WorkMDC.uchMDCData &= ~EVEN_MDC_BIT3;
            CliParaWrite(&WorkMDC);                                     /* call ParaMDCWrite() */
            if (pPifSaveFarData != NULL) {               /* saratoga */
                (*pPifSaveFarData)();
            }

            TtlResume();    /* plu total open */

            ApplSetupDevice(APPL_AUDIBLE_TONE);     /* reset tone */
            UieCtrlDevice(fsDevice);
            UieCtrlDevice(UIE_ENA_KEYBOARD|UIE_ENA_KEYLOCK);              /* recover input device */

        } else if (usEvent == PIF_AC_LOST) {
			USHORT      fsDevice =  UieCtrlDevice(UIE_DIS_WAITER|UIE_DIS_DRAWER|UIE_DIS_SCANNER|UIE_DIS_MSR|UIE_DIS_KEYBOARD|UIE_DIS_KEYLOCK);              /* set disable input device */
            PifToneVolume(0);

            ItemOtherPowerDownLog(0);
            PifLog(MODULE_UI, LOG_EVENT_UI_AC_LOST);
            //PifLog(MODULE_UI, 23);

            TtlShutDown();   /* Save plu-total into persistent storage */
        } else {

            /* power switch or AC lost */
			RFLUpdateDisplayByMajorMinorClass (CLASS_UIFREGDISP, CLASS_UIFDISP_POWERDOWN, DISP_SAVE_TYPE_0);

            ItemOtherPowerDownLog(0);
            PifLog(MODULE_UI, LOG_EVENT_UI_POWER_DOWN);
            //PifLog(MODULE_UI, 23);

            TtlShutDown();   /* Save plu-total into persistent storage */

            PifSleep(2000);
			/* clear server current status when BM option is changed */
			if (usBMOption == 0x00) {
			    if (!CliParaMDCCheck(MDC_SYSTEM4_ADR, EVEN_MDC_BIT0)) {
					/* option is changed from Non-BM to BM */
		    	    usSerCurStatus = 0;  // clear the inquiry status, will be set later using SstChangeInqStat().
        		    if (pPifSaveFarData != NULL) {               /* saratoga */
		                (*pPifSaveFarData)();
        		    }
				}
			} else {
	    		if (CliParaMDCCheck(MDC_SYSTEM4_ADR, EVEN_MDC_BIT0)) {
					/* option is changed from BM to Non-BM */
	    		    usSerCurStatus = 0;  // clear the inquiry status, will be set later using SstChangeInqStat().
		            if (pPifSaveFarData != NULL) {               /* saratoga */
        		        (*pPifSaveFarData)();
		            }
        		}
		    }
        }
        PifReleasePowerSem(usPowerSem);
    }
}

/*
*===========================================================================
** Synopsis:    VOID THREADENTRY UifCreateMain(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Create Application
*===========================================================================
*/
VOID THREADENTRY UifAbortMonitor(VOID)
{
    static  const TCHAR FARCONST    format20_OP[] = _T("%s");

    TCHAR       aszDisplay[PIF_LEN_ABORT_DISPLAY_LEN+1];

    PifCreateAbortSem();

	PifLog(MODULE_UI, LOG_EVENT_THREAD_START);
    PifLog(MODULE_LINE_NO(MODULE_UI), (USHORT)__LINE__);

    for (;;) {

        PifRequestAbortSem(&aszDisplay[0]);

        /*
        	When a PifAbort Message is called the previous semaphore
        	is resumed calling the next UiePrintf function which displays
        	the same PifAbort Error message that is given in a pop up
        	message dialog. Inside of this function it calls another
        	semaphore which hangs and causes NHPOS to wait in a continuous
        	loop for the release of the semaphore. This is a temporary
        	fix and should be uncommented and examined if the PifAbort
        	message needs to be redisplayed in the LDT.
        */

                                    /* for 2x20 operator display */
 //       UiePrintf(UIE_OPER, 0, 0, _T("%s"), aszDisplay);

#if 0
        TtlPLUCloseDB();    /* close plu total data base */
#endif
        PifSleep(1000);

        PifReleaseAbortSem();

    }
}
/*
*===========================================================================
** Synopsis:    VOID THREADENTRY PifCheckDelayBalance(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Create Application
*===========================================================================
*/
VOID THREADENTRY PifCheckDelayBalance(VOID)
{

	USHORT     usTermNo;

	usTermNo = PIF_CLUSTER_MASK_TERMINAL_NO(PifSysConfig()->usTerminalPositionFromName);

	if(uchMaintDelayBalanceFlag)
	{
		/*The following PM_CNTRL was not being used, so we use this one
		to display the D-B message on the display*/
		RFLUpdateDisplayedStatusDescriptors (0x03, PM_CNTRL, 0);

		CliCasDelayedBalance((UCHAR)usTermNo, MAINT_DBON);
	}
}

static  USHORT  usUifSharedPrintFileStatus = 0;

USHORT  UifRequestStatusAndSetSharedPrintFile (SHORT sSharedPrint)
{
	if (sSharedPrint == 1)
		PifRequestSem(husSharedPrintFileSem);   /* UifRequestStatusAndSetSharedPrintFile(): Request Semaphore */
	else if (sSharedPrint == -1)
		PifReleaseSem(husSharedPrintFileSem);

	return usUifSharedPrintFileStatus;
}

static  USHORT  usUifSharedLuaFileStatus = 0;

USHORT  UifRequestStatusAndSetSharedLuaFile (SHORT sSharedLua)
{
	if (sSharedLua == 1)
		PifRequestSem(husSharedLuaFileSem);   /* UifRequestStatusAndSetSharedLuaFile(): Request Semaphore */
	else if (sSharedLua == -1)
		PifReleaseSem(husSharedLuaFileSem);

	return usUifSharedLuaFileStatus;
}

typedef struct {
	TCHAR   *tcsText;
	TCHAR   *tcsLastToken;
	ULONG   ulFlags;
	TCHAR   tcsLastDelim;
} Rfl_tcstokIterator;

TCHAR  *Rfl_tcstok (Rfl_tcstokIterator *pTokenIterator, TCHAR *tcsDelimiters)
{
	TCHAR *tcsText = pTokenIterator->tcsText;
	TCHAR  *tcsTokenStart = 0;

	if (tcsText && tcsDelimiters && *tcsText && *tcsDelimiters) {
		TCHAR  *tcsDelim;

		// First of all we are going to go through the text buffer eating up any initial delimiters
		// looking for the beginning of the first token.
		while (*tcsText) {
			tcsDelim = tcsDelimiters;
			while (*tcsDelim) {
				if (*tcsDelim == *tcsText)
					break;
				tcsDelim++;
			}
			if (*tcsDelim == 0) {
				// this character of the string did not match a delimiter.
				// we have found the beginning of our token.
				break;
			}
			tcsText++;
		}

		if (*tcsText == 0) {
			pTokenIterator->tcsText = 0;
			pTokenIterator->tcsLastDelim = 0;
			// We have reached the end of the text string and have found no beginning of token
			// in the string.  So lets indicate so by returning a NULL pointer.
			return 0;
		}

		// we have found the beginning of a token so we need to find the end of it.
		pTokenIterator->tcsLastToken = tcsTokenStart = tcsText;
		while (*tcsText) {
			tcsDelim = tcsDelimiters;
			while (*tcsDelim) {
				if (*tcsDelim == *tcsText)
					break;
				tcsDelim++;
			}
			if (*tcsDelim != 0) {
				// found a delimiter for the token so lets finish up
				break;
			}
			tcsText++;
		}

		pTokenIterator->tcsLastDelim = *tcsDelim;
		if (*tcsText) {
			pTokenIterator->tcsText = tcsText+1;
			*tcsText = 0;     // replace the found delimiter with an end of string
		} else {
			pTokenIterator->tcsText = 0;
		}
	}

	return tcsTokenStart;
}
static TCHAR  *UifPrintFileLineParser (TCHAR *tcsBuff, UifPrintFileLineTags *pLineTags, UifPrintFileLineTags *pTags)
{
	TCHAR  *tcsTokenSeparators = _T(" =<>");
	TCHAR  *tcsBuffSave = tcsBuff;
	TCHAR  *ptcsToken;
	SHORT   sRetStatus = 0;
	Rfl_tcstokIterator   TokenIterator;
	UifPrintFileLineTags DummyEnd;

	memset(&DummyEnd, 0, sizeof(DummyEnd));
	memset (&TokenIterator, 0, sizeof(TokenIterator));
	*pTags = DummyEnd;
	if ((ptcsToken = _tcsstr (tcsBuff, _T("<"))) != 0) {
		TCHAR   *tcsKeyWord;
		TCHAR   *tcsValue;

		TokenIterator.tcsText = ptcsToken + 1;

		ptcsToken = Rfl_tcstok (&TokenIterator, tcsTokenSeparators);
		if (ptcsToken && _tcscmp (ptcsToken, pLineTags[0].tcsKeyWord) == 0) {
			// There is a token and the token matches the initial tag word for this tag
			// find the end of the tag string, put an end of string there to stop the iterator
			// and remember where the string continues.
			while (TokenIterator.tcsLastDelim != _T('>')) {
				int  i;
				tcsKeyWord = Rfl_tcstok (&TokenIterator, tcsTokenSeparators);
				tcsValue = 0;
				if (TokenIterator.tcsLastDelim == _T('<')) {
					ptcsToken = tcsKeyWord;
					break;
				}
				if (TokenIterator.tcsLastDelim == _T('=')) {
					tcsValue = Rfl_tcstok (&TokenIterator, tcsTokenSeparators);
				}
				if (tcsKeyWord == 0 || *tcsKeyWord == 0)
					break;
				for (i = 1; pLineTags[i].tcsKeyWord; i++) {
					if (_tcscmp (tcsKeyWord, pLineTags[i].tcsKeyWord) == 0) {
						int j;
						for (j = 0; pLineTags[i].tcsValueList[j].tcsMnemonic; j++) {
							if (_tcscmp (tcsValue, pLineTags[i].tcsValueList[j].tcsMnemonic) == 0) {
								pTags->tcsValueList = pLineTags[i].tcsValueList + j;
								pTags->tcsKeyWord = pLineTags[i].tcsKeyWord;
								pTags++;
								*pTags = DummyEnd;
								break;
							}
						}
						break;
					}
				}
			}
			if (TokenIterator.tcsLastDelim == _T('>')) {
				ptcsToken = Rfl_tcstok (&TokenIterator, tcsTokenSeparators);
			}
		}
	}

	return ptcsToken;
}

static USHORT PrintFeedHelper (int iLineCount)
{
	USHORT  usCheckOposReceipt = 0;
	USHORT  usPrtStatus = 0;

		usPrtStatus = PrtFeed((PMG_PRT_REPORT_ERROR | PMG_PRT_RECEIPT), iLineCount);                /* feed,    R3.0 */
		if (usPrtStatus != PMG_SUCCESS) {
			usCheckOposReceipt = BlFwPmgCallBackCheckOposReceipt (0);
		}

	return usPrtStatus;
}

static USHORT PrintLineImmediateHelper (TCHAR *tchBuff)
{
	USHORT  usCheckOposReceipt = 0;
	USHORT  usPrtStatus = 0;

		usPrtStatus = PrtPrintLineImmediate((PMG_PRT_REPORT_ERROR | PMG_PRT_RECEIPT), tchBuff);
		if (usPrtStatus != PMG_SUCCESS) {
			usCheckOposReceipt = BlFwPmgCallBackCheckOposReceipt (0);
		}

	return usPrtStatus;
}

static USHORT PrintLineHelper (TCHAR *tchBuff)
{
	USHORT  usCheckOposReceipt = 0;
	USHORT  usPrtStatus = 0;

		usPrtStatus = PrtPrintLine((PMG_PRT_REPORT_ERROR | PMG_PRT_RECEIPT), tchBuff);
		if (usPrtStatus != PMG_SUCCESS) {
			usCheckOposReceipt = BlFwPmgCallBackCheckOposReceipt (0);
		}

	return usPrtStatus;
}

VOID THREADENTRY UifSharedPrintFileThread (VOID)
{
	TCHAR    *aszPaperCut = _T("   \r\x1B|75fP");
	TCHAR    *aszFileNamePtr;
    SHORT     hsHandle;              /* Handle Area */
	SHORT     sStatus;
	TCHAR     tchBuff[256];

	if (sUifQueueSharedPrintFile < 0)
		return;

	PifLog(MODULE_UI, LOG_EVENT_THREAD_START);
    PifLog(MODULE_LINE_NO(MODULE_UI), (USHORT)__LINE__);

	BlRegisterStateChangeService (L"printspooler");

    for (;;) {                              /* Endless Loop */

        // Read the queue Semaphore for read MSR data from device such as Zebra RW 220 Printer
        aszFileNamePtr = (TCHAR *)PifReadQueue(sUifQueueSharedPrintFile);

		// Under some circumstances, a file that appears in the PrintFiles folder will be open by some
		// other application resulting in a Sharing Violation.  So take a bit of time before opening the file.
		PifSleep (100);  // add a bit of delay in case the file copy into the Printfiles folder is a bit slow.

		PifRequestSem(husSharedPrintFileSem);   /* UifRequestStatusAndSetSharedPrintFile(): Request Semaphore */

		usUifSharedPrintFileStatus = 1;

		// attempt to open print file in print spooler folder - "orp" indicates FLOLD | FLREAD | FLPRINT
		if (aszFileNamePtr && (hsHandle = PifOpenFile (aszFileNamePtr, "orp")) >= 0) {
			LONG    lLineSpacing = 0xfff;
			USHORT  usPrtStatus = 0, usCheckOposReceipt = 0;

			lLineSpacing = PmgGetSetCap((PMG_PRT_REPORT_ERROR | PMG_PRT_RECEIPT), (PRT_CAP_LINE_SPACING | PRT_CAP_GET_ONLY), 0);
			if (lLineSpacing > 0)
				PmgGetSetCap((PMG_PRT_REPORT_ERROR | PMG_PRT_RECEIPT), PRT_CAP_LINE_SPACING, lLineSpacing + 5);

			usPrtStatus = PrintFeedHelper (2);                /* feed,    R3.0 */

			ReadXmlFileLine (-1, 0, 0);  // init the data structures for the next time it is used.
			sStatus = ReadXmlFileLine (hsHandle, tchBuff, 255);
			while (sStatus >= 0) {
				TCHAR *ptcsToken, *ptcsTokenEnd, tcsBarCodeString[64];
				UifPrintFileLineTags  MyTags[20];

				if (tchBuff[0] == 0) {
					tchBuff[0] = _T(' ');
					tchBuff[1] = 0;
				}

				if ((ptcsToken = _tcsstr (tchBuff, _T("<"))) != 0) {
					if (_tcsstr (ptcsToken, _T("<papercut")) != 0) {
						PifSleep (150);
						_tcscpy (tchBuff, aszPaperCut); //Receipt Paper cut 75% see Opos Help Doc
						usPrtStatus = PrintLineHelper (tchBuff);
					} else  if (_tcsstr (ptcsToken, _T("<endpage")) != 0) {
						// if this is a Zebra receipt printer used for Amtrak then
						// lets make sure the printer buffer gets flushed.  Most Zebra
						// printers are label type printers rather than receipt type printers
						// so we have an agreement with Zebra that their OPOS control will
						// see a PrintImmediate() method call and will then print whatever
						// has been buffered up.  a paper cut will do the same buffer print.
						// an NCR receipt printer sees this as print an empty string so it does nothing.
						PifSleep (150);
						tchBuff[0] = 0;   // empty line for printImmediate()
						usPrtStatus = PrintLineImmediateHelper(tchBuff);
					} else {
						ptcsToken = UifPrintFileLineParser (tchBuff, UifPrintFileLineTags_barcode, MyTags);
						ptcsTokenEnd = _tcschr (ptcsToken, _T('<'));
						if (ptcsTokenEnd) {
							*ptcsTokenEnd = 0;    // put an end of string indicator at end of the token.
						}
						memset (tcsBarCodeString, 0, sizeof(tcsBarCodeString));
						_tcsncpy (tcsBarCodeString, ptcsToken, 31);
						{
							int i;
							ULONG  ulPrtOptions = 0;
							ULONG  ulCodeOptions = 0;

							for (i = 0; MyTags[i].tcsKeyWord; i++) {
								if (_tcscmp (_T("code"), MyTags[i].tcsKeyWord) == 0) {
									ulCodeOptions = MyTags[i].tcsValueList->ulValue;
								} else {
									ulPrtOptions |= MyTags[i].tcsValueList->ulValue;
								}
							}
								usPrtStatus = PmgPrintBarCode ((PMG_PRT_REPORT_ERROR | PMG_PRT_RECEIPT), tcsBarCodeString, ulPrtOptions, ulCodeOptions);
								if (usPrtStatus != PMG_SUCCESS) {
									usCheckOposReceipt = BlFwPmgCallBackCheckOposReceipt (0);
								}
						}
					}
				}
				else {
					usPrtStatus = PrintLineHelper (tchBuff);
				}
				sStatus = ReadXmlFileLine (hsHandle, tchBuff, 255);
			}

			PifCloseFile(hsHandle);
			PifDeleteFileNew(aszFileNamePtr, "p");
			ReadXmlFileLine (-1, 0, 0);  // init the data structures for the next time it is used.
			usPrtStatus = PrintFeedHelper (2);                /* feed,    R3.0 */
			PifSleep (150);
			_tcscpy (tchBuff, aszPaperCut); //Receipt Paper cut 75% see Opos Help Doc
			usPrtStatus = PrintLineImmediateHelper(tchBuff);
			if (lLineSpacing > 0)
				PmgGetSetCap((PMG_PRT_REPORT_ERROR | PMG_PRT_RECEIPT), PRT_CAP_LINE_SPACING, lLineSpacing);
		}
		else {
			char xBuff[128];

			sprintf (xBuff, "UifSharedPrintFileThread(): Open %S failed %d.", aszFileNamePtr, hsHandle);
			NHPOS_NONASSERT_TEXT(xBuff);
		}

		usUifSharedPrintFileStatus = 0;
		PifReleaseSem(husSharedPrintFileSem);
		// The following added to check to see if there was an error with the file printing.
		// Amtrak uses the Zebra printer rather than an NCR printer.  The Zebra printer is a
		// label type of printer which buffers up text rather than printing each line as
		// soon as the line is presented.  We do the sleep for enough time to allow the
		// printing to complete or run into an error and then do an error check.
		if (hsHandle >= 0) {
			// file was opened successfully so lets pause a moment.
			PifSleep (3000);
		}
		BlFwPmgCallBackCheckOposReceipt (0);
    }
}

#if defined(ENABLE_LUA_SCRIPTING)

static int getTransactionMnemonic (lua_State *lua)
{
	int nPushCount = 0;
	int argc = lua_gettop(lua);
	int iLoop;
	char aszMnemonic[48];

	if (argc > 1) {
		lua_Integer nAddressType = lua_tointeger (lua, 1);
		unsigned short usAddressType = (unsigned short)nAddressType;

		lua_Integer nAddress = lua_tointeger (lua, 2);
		unsigned short usAddress = (unsigned short)nAddress;

		switch (usAddressType) {
			case 20:
				{
					PARATRANSMNEMO   TM;

					memset(TM.aszMnemonics, '\0', sizeof(TM.aszMnemonics));
					/* -- read trancation mnemonics -- */
					TM.uchMajorClass = CLASS_PARATRANSMNEMO;
					TM.uchAddress    = usAddress;
					CliParaRead(&TM);

					/* -- write transaction mnemonics -- */
					for (iLoop = 0; iLoop < sizeof(TM.aszMnemonics); iLoop++) {
						aszMnemonic[iLoop] = TM.aszMnemonics[iLoop];
					}
					lua_pushstring (lua, aszMnemonic);
					nPushCount++;
				}
				break;
			case 21:
				{
					PARACHAR24  Mnem24;

					memset(Mnem24.aszMnemonics, '\0', sizeof(Mnem24.aszMnemonics));
					/* -- get 24 char mnemonics -- */
					Mnem24.uchMajorClass = CLASS_PARACHAR24;
					Mnem24.uchAddress    = usAddress;
					CliParaRead(&Mnem24);

					/* -- write transaction mnemonics -- */
					for (iLoop = 0; iLoop < sizeof(Mnem24.aszMnemonics); iLoop++) {
						aszMnemonic[iLoop] = Mnem24.aszMnemonics[iLoop];
					}
					lua_pushstring (lua, aszMnemonic);
					nPushCount++;
				}
				break;
			default:
				break;
		}
	}

	return  nPushCount;
}

void l_pushtablestringCharKey(lua_State *lua, char* key , char* value) {
    lua_pushstring(lua, key);
    lua_pushstring(lua, value);
    lua_settable(lua, -3);
}

void l_pushtablefunctionCharKey(lua_State *lua, char* key , void* value) {
    lua_pushstring(lua, key);
	lua_pushcclosure (lua, value, 0);
    lua_settable(lua, -3);
}

void l_pushtablestringIntKey(lua_State *lua, int key , char* value) {
    lua_pushnumber(lua, key);
    lua_pushstring(lua, value);
    lua_settable(lua, -3);
}
static int getPromtionalMessage (lua_State *lua)
{
	int  nPushCount = 0;

	lua_newtable(lua);
	l_pushtablestringIntKey(lua, 1, "Promotional #1");
	l_pushtablestringIntKey(lua, 2, "Promotional #2");
	nPushCount++;
	return nPushCount;
}

static TCHAR  tcsLuaBuffer[2048];

static AllObjects  UifLuaAllObjects;

static int UifLuaSendConnectionEngineMessage (lua_State *lua)
{
	struct {
		char  *aszMnemonic;
		SHORT  iTypeLabel;
	} aszDictionary [] = {
		{"FRAMEWORK", 2},
		{"PRINTFILE", 3},
		{"EMPLOYEECHANGE", 4},
		{"GUESTCHECK_RTRV", 5},
		{"CASHIERACTION", 6},
		{"EJENTRYACTION", 7},
		{"MACROACTION", 8},
		{"TCHARSTRING", 9},
		{0, 0}
	};
	int  argc = lua_gettop(lua);
	int  iMessageTag;
	int iLoop;
	VOID  *pHwnd = 0;
	char  *paszMessage = lua_tostring(lua, 1);

	memset (&UifLuaAllObjects, 0, sizeof(UifLuaAllObjects));
	for (iLoop = 0; paszMessage[iLoop]; iLoop++) {
		tcsLuaBuffer[iLoop] = paszMessage[iLoop];
	}
	tcsLuaBuffer[iLoop] = 0;
	PifGetWindowHandle (&pHwnd);
	iLoop = PostMessage (pHwnd, WM_APP_CONNENGINE_MSG_RCVD, (VOID *)tcsLuaBuffer, 0);
	return  0;
}


static int UifLuaOnEvent (lua_State *lua)
{
	int  nPushCount = 0;

	return  0;
}

static int UifLuaCreateFrame (lua_State *lua)
{
	int  nPushCount = 0;

	lua_newtable(lua);
	l_pushtablefunctionCharKey(lua, "OnEvent", (void *)UifLuaOnEvent);
	l_pushtablefunctionCharKey(lua, "SendMessage", (void *)UifLuaSendConnectionEngineMessage);
	lua_setglobal (lua, "MainFrame");
	nPushCount++;
	return nPushCount;
}

VOID THREADENTRY UifSharedLuaFileThread (VOID)
{
	TCHAR    *aszFileNamePtr;
    SHORT     hsHandle;              /* Handle Area */
	SHORT     sStatus;
	TCHAR     tchBuff[256];

	if (sUifQueueSharedLuaFile < 0)
		return;

	PifLog(MODULE_UI, LOG_EVENT_THREAD_START);
    PifLog(MODULE_LINE_NO(MODULE_UI), (USHORT)__LINE__);

    for (;;) {                              /* Endless Loop */
		lua_State *L = 0;

        // Read the queue Semaphore for read MSR data from device such as Zebra RW 220 Printer
        aszFileNamePtr = (TCHAR *)PifReadQueue(sUifQueueSharedLuaFile);
		PifRequestSem(husSharedLuaFileSem);   /* UifRequestStatusAndSetSharedLuaFile(): Request Semaphore */

		usUifSharedLuaFileStatus = 1;

			// initialize the lua interpreter for this thread
			// we could use lua_State *L = luaL_newstate(); instead
			// however with lua_newstate() we have additional options.
		L = luaL_newstate();
		if (L) {
			char myFileName[512];
			int  iLoop, iFileIndex;
			int  iStatus;

			luaL_openlibs (L);

			strcpy (myFileName, "C:/FlashDisk/NCR/Saratoga/PrintFiles/");
			iFileIndex = strlen (myFileName);
			for (iLoop = 0; iFileIndex < 510 && aszFileNamePtr[iLoop]; iFileIndex++,iLoop++) {
				myFileName[iFileIndex] = aszFileNamePtr[iLoop];
			}
			myFileName[iFileIndex] = 0;

			// should return 0 if successful otherwise will return the following:
			//   LUA_ERRFILE  unable to open the file from luaL_loadfile()
			//   LUA_ERRSYNTAX  syntax error in the lua code in the file from lua_load()
			//   LUA_ERRMEM  if there is a memory allocation error from lua_load()
			// the loadfile function loads a lua script chunk or section of source
			// into the lua script engine along with any other pieces that are there.
			// it does not execute any script, it only load the chunk.
			iStatus = luaL_loadfile(L, myFileName);
			if (iStatus != 0) {
				printf ("\nError %d from luaL_loadfile() file %s\n", iStatus, myFileName);
				printf("Couldn't load file: %s\n", lua_tostring(L, -1));
			}

			// get a mnemonic from the memory resident database
			// two arguments, mnemonic type and the address of the mnemonic
			lua_pushcclosure (L, getTransactionMnemonic, 0);
			lua_setglobal (L, "GetMnemonic");

			lua_pushcclosure (L, UifLuaCreateFrame, 0);
			lua_setglobal (L, "CreateFrame");

			if (iStatus == 0) {
				// should return 0 if successful otherwise will return the following:
				//   LUA_ERRRUN unable to run the script.
				iStatus = lua_pcall(L, 0, 0, 0);
				if (iStatus != 0) {
					char  xBuff[128];
					sprintf (xBuff, "Error %d from lua_pcall()", iStatus);
					NHPOS_ASSERT_TEXT((iStatus == 0), xBuff);
					sprintf(xBuff, "Couldn't run file: %s\n", lua_tostring(L, -1));
					NHPOS_ASSERT_TEXT((iStatus == 0), xBuff);
				}
				while (1) {
					PifSleep (10000);
				}
			}
			lua_close(L);
		}
		else {
			char xBuff[128];

			sprintf (xBuff, "UifSharedLuaThread(): Open %S failed.", aszFileNamePtr);
			NHPOS_NONASSERT_TEXT(xBuff);
		}

		usUifSharedLuaFileStatus = 0;
		PifReleaseSem(husSharedLuaFileSem);
    }
}
#endif

/**
*	Reads the Amtrak supplied employee file, searching for the given
*	ulEmployeeId and, if found, assigning the mnemonic associated with 
*	that employee to aszMnemonic
*
*	Returns:	 0	successful
*				-1	record not found
*				-2	file not found
*				>0	record found but mnemonic not parsable
*/
SHORT UifReadAmtrakEmployeeFile (ULONG  ulEmployeeId, TCHAR *aszMnemonic){
	TCHAR	*aszFileNamePtr = _T("username.txt");	//filename for the config file
	SHORT	hsHandle;								//file handle once the file is loaded
	SHORT	sStatus, sReturn = -1;					//-1 indicates record not found
	TCHAR	tchBuff[256], *tchEmpIDRec, *tchEmpRecTemp, *delim = _T("|");
	int		x;

	aszMnemonic[0] = 0;								//initialize to empty string

	// open Amtrak personnel file in Amtrak configuration folder - "orc" indicates FLOLD | FLREAD | FLCONFIG
	if((hsHandle = PifOpenFile(aszFileNamePtr, "orc")) >= 0) {
		ReadXmlFileEmployee(-1, 0, 0);	// clear out the data structures to be used for the next read
		sStatus = ReadXmlFileEmployee(hsHandle, tchBuff, 255);
		while(sStatus >= 0){
			if((x = _tcslen(tchBuff)) > 0){		//handle blank mnemonic situation...
				if(tchBuff[x-1] == _T('|')){	//if last character is a | then 
					_tcscat(tchBuff, _T(" "));	//add a space to be parsed as the mnemonic (instead of failing)
				}
			}
			tchEmpIDRec = _tcstok(tchBuff,delim);
			if(tchEmpIDRec != NULL){	//successful parse of an ID
				if(ulEmployeeId == _ttol(tchEmpIDRec)){	//and it matches the given
					x = 3;
					while(((tchEmpRecTemp = _tcstok(NULL,delim)) != NULL) && (--x > 0));	//parse to the mnemomic, skipping all else
					if(x == 0){		//successfully parsed to the mnemonic
						_tcsncpy(aszMnemonic, tchEmpRecTemp, 20);
						aszMnemonic[20] = 0;
						sReturn = UIF_SUCCESS;
					}else{
						sReturn = x;	//failure, could not parse the line with the proper empID ( 0 < error < 10 )
					}
					break;				//break out of the file reading loop as we've found the proper record already
				}
			}
			sStatus = ReadXmlFileEmployee(hsHandle, tchBuff, 255);		//read next line from file
		}
		PifCloseFile (hsHandle);
	}else{
		sReturn = -2;		//if the file doesn't exist
	}
	return sReturn;
}

// the format of the uchUniqueIdentifer array as set in function ItemCountCons() is:
//    4 digit store number
//    3 digit register number
//    4 digit consecutive number (SPCO_CONSEC_ADR)
//    5 digit compressed date
//    5 digit compressed time
//    2 digit tender id which is set to zero initially
UCHAR *UifSetUniqueIdWithTenderId (UCHAR *puchUniqueIdentifier, USHORT usTenderId)
{
	puchUniqueIdentifier[21] = ((usTenderId /10) % 10);
	puchUniqueIdentifier[22] = (usTenderId % 10);
	return puchUniqueIdentifier;
}

UCHAR *UifCopyUniqueIdSettingTenderId (UCHAR *puchDestUniqueIdentifier, UCHAR *puchSourceUniqueIdentifier, USHORT usTenderId)
{
	memcpy (puchDestUniqueIdentifier, puchSourceUniqueIdentifier, 24);
	puchDestUniqueIdentifier[21] = ((usTenderId /10) % 10);
	puchDestUniqueIdentifier[22] = (usTenderId % 10);
	return puchDestUniqueIdentifier;
}

int UifCompareUniqueIdWithoutTenderId (UCHAR *puchUniqueIdentifierLhs, UCHAR *puchUniqueIdentifierRhs)
{
	return memcmp (puchUniqueIdentifierLhs, puchUniqueIdentifierRhs, 21);
}

USHORT UifClearUniqueIdReturningTenderId (UCHAR *puchUniqueIdentifier)
{
	USHORT  usTenderId = 0;

	usTenderId = puchUniqueIdentifier[21] * 10 + puchUniqueIdentifier[22];

	puchUniqueIdentifier[21] = 0;
	puchUniqueIdentifier[22] = 0;

	return usTenderId;
}

UCHAR *UifConvertStringReturningUniqueIdRaw (TCHAR *ptcsUniqueIdentifer, UCHAR *puchUniqueIdentifier)
{
	int iLoop;

	memset (puchUniqueIdentifier, 0, 24);
	for (iLoop = 0; iLoop < 24 && ptcsUniqueIdentifer[iLoop]; iLoop++) {
		puchUniqueIdentifier[iLoop] = ptcsUniqueIdentifer[iLoop] - _T('0');
	}

	return puchUniqueIdentifier;
}

UCHAR *UifConvertStringReturningUniqueId (TCHAR *ptcsUniqueIdentifer, UCHAR *puchUniqueIdentifier, USHORT usTenderId)
{
	UifConvertStringReturningUniqueIdRaw (ptcsUniqueIdentifer, puchUniqueIdentifier);

	return UifSetUniqueIdWithTenderId (puchUniqueIdentifier, usTenderId);
}

TCHAR *UifConvertUniqueIdReturningStringRaw (TCHAR *ptcsUniqueIdentifer, UCHAR *puchUniqueIdentifier)
{
	int iLoop;

	for (iLoop = 0; iLoop < 24; iLoop++) {
		ptcsUniqueIdentifer[iLoop] = (puchUniqueIdentifier[iLoop] & 0x0f) + _T('0');
	}
	ptcsUniqueIdentifer[iLoop] = 0;

	return ptcsUniqueIdentifer;
}

TCHAR *UifConvertUniqueIdReturningString (TCHAR *ptcsUniqueIdentifer, UCHAR *puchUniqueIdentifier)
{
	UifConvertUniqueIdReturningStringRaw (ptcsUniqueIdentifer, puchUniqueIdentifier);
	// replace the usTenderId port of the unique identifier with zero digit characters
	ptcsUniqueIdentifer[21] = _T('0');
	ptcsUniqueIdentifer[22] = _T('0');

	return ptcsUniqueIdentifer;
}

/**
*	Checks whether employee id masking is turned or or not and if it is, flags
*   the returned employee id with the mask bit.
*
*   WARNING:  Should be used with employee ids only for masking by function _RflFormStr().
*
*	Returns:	 Modified employee id indicating if masking is on or not
*                The indicator is interpreted by function _RflFormStr()
*                when processing the M or mask format indicator.
*/
ULONG RflTruncateEmployeeNumber (ULONG ulEmployeeNumber)
{
	UCHAR  ucMdcMasking;

	ucMdcMasking = CliParaMDCCheckField(MDC_EMPLOYEE_ID_ADR, MDC_PARAM_BIT_D);

	if (ucMdcMasking) {
		ulEmployeeNumber |= 0x80000000;
	}
	return ulEmployeeNumber;
}

FLEXMEM  RflGetFlexMemByType(UCHAR uchAddress)
{
	FLEXMEM     retFlex = { 0 };

	if (uchAddress <= FLEX_ADR_MAX) {
		PARAFLEXMEM WorkFlex = { 0 };

		WorkFlex.uchMajorClass = CLASS_PARAFLEXMEM;
		WorkFlex.uchAddress = uchAddress;
		ParaFlexMemRead(&WorkFlex);
		retFlex.uchPTDFlag = WorkFlex.uchPTDFlag;
		retFlex.ulRecordNumber = WorkFlex.ulRecordNumber;
	}
	else {
		NHPOS_ASSERT(uchAddress <= FLEX_ADR_MAX);
	}
	return retFlex;
}

// RflGetSystemType() - read the current system type from the memory resident
//                      database.
//
//    FLEX_PRECHK_BUFFER           pre-check buffering print
//    FLEX_PRECHK_UNBUFFER         pre-check unbuffering print
//    FLEX_POST_CHK                post check buffering printprint
//    FLEX_STORE_RECALL           store/ recall buffering print system
//
//  See also Transaction Qualifier bits set in function ItemCurPrintStatus()
//    CURQUAL_PRE_BUFFER          precheck/buffering, FLEX_PRECHK_BUFFER
//    CURQUAL_PRE_UNBUFFER        precheck/unbuffering, FLEX_PRECHK_UNBUFFER
//    CURQUAL_POSTCHECK           postcheck, FLEX_POST_CHK
//    CURQUAL_STORE_RECALL        store/recall, FLEX_STORE_RECALL
//
USHORT  RflGetSystemType (VOID)
{
    return (RflGetFlexMemByType(FLEX_GC_ADR).uchPTDFlag);
}

// RflGetMaxRecordNumberByType() - read the max number of records for a given Flexible Memory
//                              data file (Cashiers, Guest Checks, etc.) from the memory resident
//                              database.
ULONG  RflGetMaxRecordNumberByType (UCHAR uchAddress)
{
	return (RflGetFlexMemByType(uchAddress).ulRecordNumber);
}

// RflGetPtdFlagByType() - read the Period To Day flag for a given Flexible Memory
//                         data file (Cashiers, Guest Checks, etc.) from the memory resident
//                         database. For FLEX_GC_ADR address and FLEX_EJ_ADR address the
//                         uchPTDFlag indicates system type and EJ Override respectively.
USHORT  RflGetPtdFlagByType (UCHAR uchAddress)
{
	return (RflGetFlexMemByType(uchAddress).uchPTDFlag);
}

/*
*===========================================================================
** Format  : USHORT RflGetScaleUnits (VOID);
*
*   Input      : none
*   Output     : none
*   InOut      : none
** Return      : USHORT     - bit map indicating settings of MDC Address 29.
*
** Synopsis: This function reads the MDC Address 29 in order to determine how
*            to interpret a scale weight value and its number of decimal places.
*
*            In most cases the function return value is checked to determine
*            if the weight units are two decimal places (0.01 LB/Kg) or are
*            three decimal places (0.0005 LB/Kg or 0.001 LB/Kg). This check
*            can be done easily by Bitwise AND with a value of 0x0002 as that
*            bit will not be set if the minimum unit is 0.01 LB/Kg and will be
*            set if the minum unit is either 0.005 LB/Kg or 0.001 LB/Kg.
*===========================================================================
*/
USHORT RflGetScaleUnits(VOID)
{
	USHORT  usScale = 0;

	// determine the minimum unit of the scale. This is also used to indicate
	// whether units are with two decimal places or three decimal places.
	if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT0) == 0) {         /* MDC #29 BIT0 is 0 else check BIT2 */
		usScale |= RFL_SCALE_UNITS_01;                               /* minimum unit of auto scale is  0.01  LB/Kg, two decimal places */
	}
	else if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT2) == 0) {    /* MDC #29 BIT2 is 0 */
		usScale |= RFL_SCALE_UNITS_005;                              /* minimum unit of auto scale is 0.005 LB/Kg, three decimal places */
	}
	else {
		usScale |= RFL_SCALE_UNITS_001;                              /* minimum unit of auto scale is 0.001 LB/Kg, three decimal places */
	}

	// determine how Scale Weight units are accumulated. MDC Address 29 Bit C setting
	if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT1) == 0) {    /* MDC #29 BIT1 is 0 */
		usScale |= RFL_SCALE_ACCUM_1;                           /* scale weight accumulated by 0.1 LB/Kg */
	}
	else {                                                      /* MDC #29 BIT1 is 1 */
		usScale |= RFL_SCALE_ACCUM_01;                          /* scale weight accumulated by 0.01 LB/Kg */
	}

	return usScale;
}

/*
*===========================================================================
** Format  : USHORT RflGetSetDecimalCommaType (VOID);
*
*   Input      : none
*   Output     : none
*   InOut      : none
** Return      : USHORT     - ANSI character of '0', '2', or '3'
** Side Effects:            - sets decimal places and decimal separator for RFL subsystem.
*                           - sets decimal place indicator in SYSCONFIG ausOption[1] member.
*
** Synopsis: This function reads the MDC Address 367 in order to determine how
*            to interpret a currency value and its number of decimal places as
*            well as the decimal separator character.  Some countries use the
*            comma as the decimal separator rather than the decimal point.
*
*            WARNING: This function has the side effect of setting the RFL subsystem
*                     settings for number of decimal places and the decimal separator.
*                     This function also modifies ausOption[1] of SYSCONFIG.
*===========================================================================
*/
USHORT  RflGetSetDecimalCommaType (VOID)
{
    PARAMDC WorkMDC = {0};
	USHORT  usDecimalType = '0';
    SYSCONFIG *pSysConfig = PifSysConfig();

	WorkMDC.uchMajorClass = CLASS_PARAMDC;
    WorkMDC.usAddress = MDC_SYSTEM3_ADR;
    CliParaRead(&WorkMDC);
	// MDC address 367 () is odd so we use the lower nibble.
	// MDC address 367 uses bits D and C to indicate the decimal point as a 2 bit value
	// so the range of values are from 0 to 3.
	//  - 0 means 2 decimal points
	//  - 1 means 3 decimal points
	//  - 2 or 3 means 0 decimal points
	// As a side effect we also set the value of the ausOption[1] in SYSCONFIG SysConfig
	// in order to maintain compatibility with any parts of the source depending on the
	// value rather than using the MDC.
    if  (WorkMDC.uchMDCData & 0x02) {
        _RflSetDecimal(UIF_DEC_POSNO);      /* set no decimal position */
		pSysConfig->ausOption[1] = 2;
		usDecimalType = '0';
    } else if (WorkMDC.uchMDCData & 0x01) {
        _RflSetDecimal(UIF_DEC_POS3);       /* set 3 decimal position */
		pSysConfig->ausOption[1] = 1;
		usDecimalType = '3';
    } else {
        _RflSetDecimal(UIF_DEC_POS2);       /* set 2 decimal position */
		pSysConfig->ausOption[1] = 0;
		usDecimalType = '2';
    }

	/* SR118 */ //ESMITH CURRENCY
	// This comparison is done on the already retrieved MDC_SYSTEM3_ADR
	if (WorkMDC.uchMDCData & ODD_MDC_BIT2) {
		_RflSetCommaDecimal(_RFL_DECIMAL_SEP,_RFL_USE_COMMA);	/* set comma as decimal seperator for currency */
	} else {
		_RflSetCommaDecimal(_RFL_DECIMAL_SEP,_RFL_USE_DECIMAL);	/* set decimal as decimal seperator for currency */
	}

	return usDecimalType;
}

/*
*===========================================================================
** Format  : USHORT RflGetNoTermsInCluster (VOID);
*
*   Input      : none
*   Output     : none
*   InOut      : none
** Return      : USHORT     - Number of terminals as set by MDC Address 365
*
** Synopsis: This function reads the MDC Address 365 in order to determine how
*            many terminals are in the cluster. This count is used in a variety
*            of places for actions in which the Master Terminal communicates with
*            all Satellite Terminals in the cluster.  An example is resetting the EJ.
*
*            Previously number of terminals was in ausOption[3] of SYSCONFIG however
*            that usage is now deprecated and should not be used.
*
*            TTL_MAX_INDFIN is used for max number individual financial reports (one per terminal).
*            See also MAX_TERMINAL_NO which should equal TTL_MAX_INDFIN.
*===========================================================================
*/
USHORT  RflGetNoTermsInCluster (VOID)
{
    PARAMDC WorkMDC = {0};

	WorkMDC.uchMajorClass = CLASS_PARAMDC;
    WorkMDC.usAddress = MDC_SYSTEM1_ADR;
    CliParaRead(&WorkMDC);
    return (USHORT)(WorkMDC.uchMDCData & 0x0F) + 1;  // number of terminals value 0 to 15 translated to 1 to 16, TTL_MAX_INDFIN
}

/*
*===========================================================================
** Format  : UCHAR  RflGetMdcData (USHORT usAddr);
*
*   Input      : MDC address for the data to fetch
*   Output     : none
*   InOut      : none
** Return      : UCHAR data associated with the given MDC address
*
** Synopsis: This function reads the MDC Address specified and returns the
*            UCHAR value associated with the MDC address.
*===========================================================================
*/
UCHAR  RflGetMdcData (USHORT usAddr)
{
	PARAMDC   WorkMDC = { 0 };

	WorkMDC.uchMajorClass = CLASS_PARAMDC;
	WorkMDC.usAddress = usAddr;
	ParaMDCRead (&WorkMDC);

	return WorkMDC.uchMDCData;
}

/*
*    RflCleanupMnemonic (TCHAR *tcsmnemonic)
*
*  Description: Accepts a zero terminated string and removes any non-printing
*               characters as well as tilde (~) characters.
*
*               This function is normally used to clean up a mnemonic such as a
*               P20 Transaction Mnemonic that is being used with a button caption.
*
*               Some databases will use the tilde (~) character in transaction mnemonics
*               so that when the mnemonic is printed on the receipt, it will be printed
*               in double wide characters making it more prominent and visible. However
*               these characters cause a problem when the Transaction Mnemonic is used
*               in the caption for a button.
*/
TCHAR *RflCleanupMnemonic (TCHAR *tcsmnemonic)
{
	TCHAR  *tcsSave = tcsmnemonic;
	TCHAR  *tcsCur;

	for (tcsCur = tcsmnemonic; *tcsmnemonic; tcsmnemonic++) {
		if (*tcsmnemonic >= _T(' ') && *tcsmnemonic != _T('~')) {
			*tcsCur = *tcsmnemonic; tcsCur++;
		}
	}
	*tcsCur = 0;

	return tcsSave;
}

/*
*===========================================================================
** Format  : TCHAR *RflCopyMnemonicTerminate (TCHAR *tcsDest, TCHAR *tcsSource, int nMaxCount)
*
*   Input  : TCHAR *tcsSource           -source buffer address
*            int nMaxCount              -max number of characters to copy
*   Output : TCHAR   *tcsDest,          -destination buffer address
*   InOut  : none
** Return  : TCHAR   *tcsDest,          -destination buffer address
*
** Synopsis: This function copies a text string from the source to the destination
*            and makes sure that a specified maximum count is enforced.
*
*            This function assumes the destination buffer is sized to be
*            nMaxCount + 1 so the logic is that nMaxCount is the maximum count of
*            text characters and the destination buffer is sized for that maximum
*            count of characters plus one more character for an end of string terminator.
*
*            This function is designed to be used when copying the various mnemonics
*            from one place to another such as when copying a transation mnemonic into
*            the mnemonic area of a struct that is then used to print a data item and
*            its mnemonic.
*===========================================================================
*/
TCHAR *RflCopyMnemonic (TCHAR *tcsDest, CONST TCHAR *tcsSource, int nMaxCharCount)
{
    _tcsncpy(tcsDest, tcsSource, nMaxCharCount);    /* Copy mnemonic with max number of characters    */
	tcsDest[nMaxCharCount] = 0;                     // ensure mnemonic is zero terminated. assumes buffer is nMaxCharCount + 1

	return tcsDest;
}

/*
*===========================================================================
** Format  : TCHAR *RflGetMnemonicByClass (USHORT usClass, TCHAR *pszDest, USHORT usAddress);
*
*   Input  : USHORT  usClass            - class of mnemonic, uchMajorClass such as CLASS_PARATRANSMNEMO
*            USHORT  usAddress          - address of Mnemonics, 1 thru n
*   Output : TCHAR   *pszDest,          - destination buffer address
*   InOut  : none
** Return  : TCHAR   *pszDest,          -destination buffer address
*
** Synopsis: This function gets the specified type of mnemonics at the specified address.
*            This is a general function that we expect to use throughout the code when
*            retrieving a mnemonic from the memory resident database for a particular
*            class of mnemonic.
*
*            The range of an address is one based and starts at 1 so if an address value
*            of zero (0) is specified, we just return an empty string.
*            Also if the class specified in usClass is unknown then we will also
*            return an empty string but will issue a log as well.
*===========================================================================
*/
TCHAR *RflGetMnemonicByClass (USHORT usClass, TCHAR *pszDest, USHORT usAddress)
{
	if (pszDest) {
		if (usAddress == 0) {
			*pszDest = '\0';
		} else {
			usAddress &= STD_FORMATMASK_REMOVE;   // remove any special formatting flags such as STD_FORMATMASK_INDENT_4
			switch (usClass) {
				case CLASS_PARATRANSMNEMO:      /* TRANSACTION MNEMONICS - P20 Transaction Mnemonics */
					{
						PARATRANSMNEMO   Mnemo = {0};

						Mnemo.uchMajorClass = CLASS_PARATRANSMNEMO;
						Mnemo.uchAddress    = usAddress;
						ParaTransMnemoRead (&Mnemo);
						_tcsncpy(pszDest, Mnemo.aszMnemonics, PARA_TRANSMNEMO_LEN + 1);
					}
					break;

				case CLASS_PARALEADTHRU:        /* LEAD THROUGH TABLE - P21 Lead-thru Mnemonics */
					{
						PARALEADTHRU   Mnemo = {0};

						Mnemo.uchMajorClass = CLASS_PARALEADTHRU;
						Mnemo.uchAddress    = usAddress;
						ParaLeadThruRead (&Mnemo);
						_tcsncpy(pszDest, Mnemo.aszMnemonics, PARA_LEADTHRU_LEN + 1);
					}
					break;

				case CLASS_PARAREPORTNAME:      /* REPORT NAME - P22 Report Mnemonics */
					{
						PARAREPORTNAME   Mnemo = {0};

						Mnemo.uchMajorClass = CLASS_PARAREPORTNAME;
						Mnemo.uchAddress    = (UCHAR)usAddress;
						ParaReportNameRead (&Mnemo);
						_tcsncpy(pszDest, Mnemo.aszMnemonics, PARA_REPORTNAME_LEN + 1);
					}
					break;

				case CLASS_PARASPECIALMNEMO:    /* SPECIAL MNEMONICS - P23 Special Mnemonics */
					{
						PARASPEMNEMO Mnemo = {0};                      /* special mnemonics */

						Mnemo.uchMajorClass = CLASS_PARASPECIALMNEMO;
						Mnemo.uchAddress    = (UCHAR)usAddress;
						ParaSpeMnemoRead (&Mnemo);
						_tcsncpy(pszDest, Mnemo.aszMnemonics, PARA_SPEMNEMO_LEN + 1);
					}
					break;

				case CLASS_PARAADJMNEMO:        /* ADJECTIVE MNEMONICS - P46 Adjective Mnemonics */
					{
						PARAADJMNEMO   Mnemo = {0};

						Mnemo.uchMajorClass = CLASS_PARAADJMNEMO;
						Mnemo.uchAddress    = (UCHAR)usAddress;
						ParaAdjMnemoRead (&Mnemo);
						_tcsncpy(pszDest, Mnemo.aszMnemonics,  PARA_ADJMNEMO_LEN + 1);
					}
					break;

				case CLASS_PARAPRTMODI:         /* PRINT MODIFIER MNEMONICS - P47 Print Modifier Mnemonics */
					{
						PARAPRTMODI   Mnemo = {0};

						Mnemo.uchMajorClass = CLASS_PARAPRTMODI;
						Mnemo.uchAddress = (UCHAR)usAddress;
						ParaPrtModiRead (&Mnemo);
						_tcsncpy(pszDest, Mnemo.aszMnemonics,  PARA_PRTMODI_LEN + 1);
					}
					break;

				case CLASS_PARAMAJORDEPT:       /* REPORT CODE MNEMONICS - P48 Major Department Mnemonics */
					{
						PARAMAJORDEPT   Mnemo = {0};

						Mnemo.uchMajorClass = CLASS_PARAMAJORDEPT;
						Mnemo.uchAddress = (UCHAR)usAddress;
						ParaMajorDEPTRead (&Mnemo);
						_tcsncpy(pszDest, Mnemo.aszMnemonics,  PARA_MAJORDEPT_LEN + 1);
					}
					break;

				case CLASS_PARACHAR24:          /* 24 CHARACTERS MNEMONICS - P57 Header/Special Mnemonics  */
					{
						PARACHAR24  Mnemo = {0};

						Mnemo.uchMajorClass = CLASS_PARACHAR24;
						Mnemo.uchAddress    = (UCHAR)usAddress;
						ParaChar24Read (&Mnemo);
						_tcsncpy(pszDest, Mnemo.aszMnemonics, PARA_CHAR24_LEN + 1);
					}
					break;

				case CLASS_PARASOFTCHK:          /* Trailer Print for Soft Check AC87 */
					{
						PARASOFTCHK  Mnemo = {0};

						Mnemo.uchMajorClass = CLASS_PARASOFTCHK;
						Mnemo.uchAddress    = (UCHAR)usAddress;
						ParaSoftChkMsgRead (&Mnemo);
						_tcsncpy(pszDest, Mnemo.aszMnemonics, PARA_SOFTCHK_LEN + 1);
					}
					break;

				default:
					NHPOS_ASSERT_TEXT(0, "==ERROR: RflGetMnemonicByClass() unknown uchMajorClass for mnemonic.");
					*pszDest = '\0';
					break;
			}
		}
	}

	return  pszDest;
}


/*
*===========================================================================
** Format  : TCHAR *RflGetTranMnem (TCHAR *pszDest, USHORT usAddress);
*
*   Input  : USHORT  usAddress          -Special Mnemonics address
*   Output : TCHAR   *pszDest,          -destination buffer address
*   InOut  : none
** Return  : TCHAR   *pszDest,          -destination buffer address
*
** Synopsis: This function gets transaction  mnemonics.
*===========================================================================
*/
TCHAR *RflGetTranMnem(TCHAR *pszDest, USHORT usAddress)
{
	return RflGetMnemonicByClass (CLASS_PARATRANSMNEMO, pszDest, usAddress);
}

/*
*===========================================================================
** Format  : TCHAR *RflGetReportMnem (TCHAR *pszDest, USHORT usAddress);
*
*   Input  : USHORT  usAddress          -Special Mnemonics address
*   Output : TCHAR   *pszDest,          -destination buffer address
*   InOut  : none
** Return  : TCHAR   *pszDest,          -destination buffer address
*
** Synopsis: This function gets report mnemonics.
*===========================================================================
*/
TCHAR *RflGetReportMnem(TCHAR *pszDest, USHORT usAddress)
{
	return RflGetMnemonicByClass (CLASS_PARAREPORTNAME, pszDest, usAddress);
}

/*
*===========================================================================
** Format  : TCHAR *RflGet24Mnem (TCHAR *pszDest, USHORT usAddress);
*
*   Input  : USHORT  usAddress          -Special Mnemonics address
*   Output : TCHAR   *pszDest,          -destination buffer address
*   InOut  : none
** Return  : TCHAR   *pszDest,          -destination buffer address
*
** Synopsis: This function gets 24Char mnemonics.
*===========================================================================
*/
TCHAR *RflGet24Mnem(TCHAR  *pszDest, USHORT usAddress)
{
	return RflGetMnemonicByClass (CLASS_PARACHAR24, pszDest, usAddress);
}

/*
*===========================================================================
** Format  : TCHAR *RflGetLead (TCHAR *pszDest, USHORT usAddress);
*
*   Input  : USHORT  usAddress          -Special Mnemonics address
*   Output : TCHAR   *pszDest,          -destination buffer address
*   InOut  : none
** Return  : TCHAR   *pszDest,          -destination buffer address
*
** Synopsis: This function gets Lead through mnemonics.
*===========================================================================
*/
TCHAR *RflGetLead(TCHAR  *pszDest, USHORT usAddress)
{
	return RflGetMnemonicByClass (CLASS_PARALEADTHRU, pszDest, usAddress);
}

/*
*===========================================================================
** Format  : TCHAR *RflGetSpecMnem (TCHAR *pszDest, USHORT usAddress);
*
*   Input  : USHORT  usAddress          -Special Mnemonics address
*   Output : TCHAR   *pszDest,          -destination buffer address
*   InOut  : none
** Return  : TCHAR   *pszDest,          -destination buffer address
*
** Synopsis: This function gets special mnemonics.
*===========================================================================
*/
TCHAR *RflGetSpecMnem(TCHAR *pszDest, USHORT usAddress)
{
	return RflGetMnemonicByClass (CLASS_PARASPECIALMNEMO, pszDest, usAddress);
}

/*
*===========================================================================
** Format  : TCHAR *RflGetAdj(TCHAR *pszDest, USHORT usAddress);
*
*   Input  : USHORT  usAddress          -Special Mnemonics address
*   Output : TCHAR   *pszDest,          -destination buffer address
*   InOut  : none
** Return  : TCHAR   *pszDest,          -destination buffer address
*
** Synopsis: This function gets adjective mnemonics.
*===========================================================================
*/
TCHAR *RflGetAdj(TCHAR *pszDest, USHORT usAddress)
{
	return RflGetMnemonicByClass (CLASS_PARAADJMNEMO, pszDest, usAddress);
}

/*
*===========================================================================
** Format  : TCHAR *RflGetMajorDeptMnem(TCHAR *pszDest, USHORT usAddress);
*
*   Input  : USHORT  usAddress          -major department address
*   Output : TCHAR   *pszDest,          -destination buffer address
*   InOut  : none
** Return  : TCHAR   *pszDest,          -destination buffer address
*
** Synopsis: This function gets major department mnemonics.
*===========================================================================
*/
TCHAR *RflGetMajorDeptMnem(TCHAR *pszDest, USHORT usAddress)
{
	return RflGetMnemonicByClass(CLASS_PARAMAJORDEPT, pszDest, usAddress);
}

/*
*===========================================================================
** Format  : RflMoneyMnem   PrtGetMoneyMnem(UCHAR uchTendMinor)
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function determines the Transaction mnemonic and the symbol mnemonic
*            based on the tender type.
*
*            This function also will handle a Foreign Total key press as well
*            so long as CLASS_FOREIGNTOTAL1 is not in the same range as tender classes.
*
*            Unknown tender types or tender types that don't have a special mnemonic
*            will return an address value of zero to be compatible with the standard
*            mnemonic retrieve function RflGetMnemonicByClass().
*===========================================================================
*/
RflMoneyMnem    RflGetMoneyMnem(UCHAR uchTendMinor)
{
	RflMoneyMnem  myMnemo = { 0 };  // initialize to address values of zero

	if (uchTendMinor >= CLASS_FOREIGN1 && uchTendMinor <= CLASS_FOREIGN2) {
		myMnemo.usTran = (uchTendMinor - CLASS_FOREIGN1 + TRN_FT1_ADR);
		myMnemo.usSym = (uchTendMinor - CLASS_FOREIGN1 + SPC_CNSYMFC1_ADR);
	}
	else if (uchTendMinor >= CLASS_FOREIGN3 && uchTendMinor <= CLASS_FOREIGN8) {
		myMnemo.usTran = (uchTendMinor - CLASS_FOREIGN3 + TRN_FT3_ADR);
		myMnemo.usSym = (UCHAR)(uchTendMinor - CLASS_FOREIGN3 + SPC_CNSYMFC3_ADR);
	}
	else if (uchTendMinor >= CLASS_TENDER1 && uchTendMinor <= CLASS_TENDER8) {
		myMnemo.usTran = (uchTendMinor - CLASS_TENDER1 + TRN_TEND1_ADR);
	}
	else if (uchTendMinor >= CLASS_TENDER9 && uchTendMinor <= CLASS_TENDER11) {
		myMnemo.usTran = (uchTendMinor - CLASS_TENDER9 + TRN_TEND9_ADR);
	}
	else if (uchTendMinor >= CLASS_TENDER12 && uchTendMinor <= CLASS_TENDER20) {
		myMnemo.usTran = (uchTendMinor - CLASS_TENDER12 + TRN_TENDER12_ADR);
	}
	else if (uchTendMinor >= CLASS_FOREIGNTOTAL1 && uchTendMinor <= CLASS_FOREIGNTOTAL8) {
		myMnemo.usTran = (uchTendMinor - CLASS_FOREIGNTOTAL1 + TRN_FT1_ADR);
		myMnemo.usSym = (UCHAR)(uchTendMinor - CLASS_FOREIGNTOTAL1 + SPC_CNSYMFC1_ADR);
	}
	else {
		NHPOS_ASSERT(uchTendMinor <= CLASS_TENDER20);
	}

	return myMnemo;
}

/*
*===========================================================================
** Format  : TCHAR *RflGetPauseMsg(TCHAR *pszDest, USHORT usPauseMsgId);
*
*   Input  : USHORT  usPauseMsgId       - Pause Message identifier or number
*   Output : TCHAR   *pszDest,          - destination buffer address
*   InOut  : none
** Return  : TCHAR   *pszDest,          - destination buffer address
*
** Synopsis: This function gets a Pause Message leadthru mnemonic.
*            A Pause Message number is used in a Control String to prompt the
*            user with one of the Pause Message mnemonics that are specified in
*            the LeadThru Mnemonics.
*
*            There were originally 10 different Pause Messages, 5 were for
*            numeric mode entry and 5 for alphanumeric mode entry. These were
*            in NHPOS 1.4 and the original source.
*
*            We then added 10 additional Pause Messages, 5 for numeric mode
*            entry and 5 for alphanumeric mode entry. These were added for
*            NHPOS 2.x and added by Georgia Southern for US Customs SCER cwunn
*===========================================================================
*/
TCHAR *RflGetPauseMsg(TCHAR *pszDest, USHORT usPauseMsgId)
{
	USHORT  usAddress = 0;

	if(usPauseMsgId > 10) { //redirect if pause message id is higher than 10
		usAddress = (LDT_PAUSEMSG11_ADR + usPauseMsgId - 11);
	}
	else {  //maintain original pause message id behavior
		usAddress = (LDT_PAUSEMSG1_ADR + usPauseMsgId - 1);
	}
	return RflGetMnemonicByClass (CLASS_PARALEADTHRU, pszDest, usAddress);
}

/*
*===========================================================================
** Format  : USHORT  RflGetHaloData(USHORT usAddress);
*
*   Input  : USHORT uchAddress          -Special Mnemonics address
*   Output : none
*   InOut  : none
** Return  : HALO value
*
** Synopsis: This function gets the HALO value for a particular HALO address.
*            There are a number of different types of High Amount Lock Out (HALO)
*            data items including various tender keys and discounts.
*
*            See function RflHALO() for how the HALO amount is calculated.
*
*            Address range begins with defined constant HALO_TEND1_ADR and
*            ends with defined constant MAX_HALOKEY_NO.
*===========================================================================
*/
USHORT  RflGetHaloData (USHORT usAddress)
{
	PARATRANSHALO   ParaTransHaloL = {0};

    ParaTransHaloL.uchMajorClass =  CLASS_PARATRANSHALO;
    ParaTransHaloL.uchAddress = usAddress;
    ParaTransHALORead (&ParaTransHaloL);

    return ParaTransHaloL.uchHALO;
}

/*
*===========================================================================
** Format  : USHORT  RflGetLimitFloor(USHORT usAddress);
*
*   Input  : USHORT uchAddress          - AC128 address
*   Output : none
*   InOut  : none
** Return  : limit or floor value for the specified address in AC128.
*
** Synopsis: This function gets the AC128 limit or floor value for a particular
*            AC128 address. See MISC_FOODDENOMI_ADR, MISC_TRANSCREDIT_ADR, etc.
*            We return a ULONG since sometimes it is status bits that are stored
*            in the ParaMiscPara[] array in the memory resident database so
*            when fetching a limit, probably will need to cast to signed.
*===========================================================================
*/
ULONG  RflGetLimitFloor (USHORT usAddress)
{
	PARAMISCPARA   ParaTransMiscLimit = {0};

    ParaTransMiscLimit.uchMajorClass =  CLASS_PARAMISCPARA;
    ParaTransMiscLimit.uchAddress = usAddress;
    ParaMiscParaRead (&ParaTransMiscLimit);

    return ParaTransMiscLimit.ulMiscPara;
}


SHORT  RflGetSlipMaxLines (VOID)
{
	PARASLIPFEEDCTL  FeedCtl = {0};

    FeedCtl.uchMajorClass = CLASS_PARASLIPFEEDCTL;    // retrieve data from P18 Printer Feed and EJ Control
    FeedCtl.uchAddress = SLIP_MAXLINE_ADR;
    ParaSlipFeedCtlRead (&FeedCtl);

	return (SHORT)((USHORT)FeedCtl.uchFeedCtl);
}

/*
*===========================================================================
** Format  : USHORT  RflChkTtlAdr(ITEMTOTAL *pItem);
*
*   Input  : ITEMTOTAL *pItem
*   Output : none
*   InOut  : none
** Return  : USHORT    usAddress             -transaction mnemonic address
*
** Synopsis: This function checks total # and returns the appropriate transaction mnemonic id.
*            This function may be called using an argument of ITEMTOTAL or UIFREGTOTAL as these
*            structs are the same layout in the first couple of members, uchMajorClass and uchMinorClass.
*===========================================================================
*/
USHORT  RflChkTtlStandardAdr(UCHAR uchMinorClass)
{
    USHORT   usTrnMnemAddr = TRN_TTL1_ADR;

    switch ( uchMinorClass ) {
    case CLASS_TOTAL1:
    case CLASS_TOTAL2:
    case CLASS_TOTAL3:
    case CLASS_TOTAL4:
    case CLASS_TOTAL5:
    case CLASS_TOTAL6:
    case CLASS_TOTAL7:
    case CLASS_TOTAL8:
        usTrnMnemAddr = ( uchMinorClass - CLASS_TOTAL1 + TRN_TTL1_ADR );
        break;

    case CLASS_TOTAL9:                      /* Saratoga */
        usTrnMnemAddr = TRN_TTL9_ADR;
        break;

	case CLASS_TOTAL10:
	case CLASS_TOTAL11:
	case CLASS_TOTAL12:
	case CLASS_TOTAL13:
	case CLASS_TOTAL14:
	case CLASS_TOTAL15:
	case CLASS_TOTAL16:
	case CLASS_TOTAL17:
	case CLASS_TOTAL18:
	case CLASS_TOTAL19:
	case CLASS_TOTAL20:
		usTrnMnemAddr = ( uchMinorClass - CLASS_TOTAL10 + TRN_TOTAL10_ADR );
		break;
	default:
		NHPOS_ASSERT_TEXT(0, "**ERROR: Unknown total uchMinorClass in RflChkTtlStandardAdr()");
		break;
	}
    return ( usTrnMnemAddr );
}


USHORT  RflChkTtlAdr(ITEMTOTAL *pItem)
{
    USHORT   usTrnMnemAddr = TRN_TTL1_ADR;

    switch ( pItem->uchMinorClass ) {
    case CLASS_TOTAL1:
    case CLASS_TOTAL2:
    case CLASS_TOTAL3:
    case CLASS_TOTAL4:
    case CLASS_TOTAL5:
    case CLASS_TOTAL6:
    case CLASS_TOTAL7:
    case CLASS_TOTAL8:
    case CLASS_TOTAL9:                      /* Saratoga */
	case CLASS_TOTAL10:
	case CLASS_TOTAL11:
	case CLASS_TOTAL12:
	case CLASS_TOTAL13:
	case CLASS_TOTAL14:
	case CLASS_TOTAL15:
	case CLASS_TOTAL16:
	case CLASS_TOTAL17:
	case CLASS_TOTAL18:
	case CLASS_TOTAL19:
	case CLASS_TOTAL20:
		usTrnMnemAddr = RflChkTtlStandardAdr( pItem->uchMinorClass );
		break;

    case CLASS_TOTAL2_FS:                   /* Saratoga */
        if (pItem->ulID != 0) {
            usTrnMnemAddr = RflChkTtlStandardAdr((UCHAR)(pItem->ulID & 0xff));
        } else {
            usTrnMnemAddr = TRN_TTL2_ADR;
        }
        break;

    case CLASS_STUB3:
    case CLASS_STUB4:
    case CLASS_STUB5:
    case CLASS_STUB6:
    case CLASS_STUB7:
    case CLASS_STUB8:
	case CLASS_STUB9:
	case CLASS_STUB10:
	case CLASS_STUB11:
	case CLASS_STUB12:
	case CLASS_STUB13:
	case CLASS_STUB14:
	case CLASS_STUB15:
	case CLASS_STUB16:
	case CLASS_STUB17:
	case CLASS_STUB18:
	case CLASS_STUB19:
	case CLASS_STUB20:
        usTrnMnemAddr = RflChkTtlStandardAdr (pItem->uchMinorClass - CLASS_STUB_OFFSET);
        break;

    case CLASS_ORDER3:
    case CLASS_ORDER4:
    case CLASS_ORDER5:
    case CLASS_ORDER6:
    case CLASS_ORDER7:
    case CLASS_ORDER8:
	case CLASS_ORDER9:
	case CLASS_ORDER10:
	case CLASS_ORDER11:
	case CLASS_ORDER12:
	case CLASS_ORDER13:
	case CLASS_ORDER14:
	case CLASS_ORDER15:
	case CLASS_ORDER16:
	case CLASS_ORDER17:
	case CLASS_ORDER18:
	case CLASS_ORDER19:
	case CLASS_ORDER20:
		usTrnMnemAddr = RflChkTtlStandardAdr(pItem->uchMinorClass - CLASS_ORDER_OFFSET);
        break;

    case CLASS_TOTALAUDACT:
        usTrnMnemAddr = TRN_AMTTL_ADR;
        break;

    case CLASS_TOTALPOSTCHECK:
    case CLASS_STUBPOSTCHECK:
        usTrnMnemAddr = TRN_CKSUM_ADR;
        break;

    case CLASS_PREAUTHTOTAL:
        usTrnMnemAddr = TRN_PREAUTHTTL_ADR;
        break;

	// Though the following defines are in order, the transaction mnemonics are not
	// so we need to break these up into several sections in order to map the
	// soft check total to the correct transaction mnemonic.
    case CLASS_SOFTCHK_TOTAL1:
    case CLASS_SOFTCHK_TOTAL2:
    case CLASS_SOFTCHK_TOTAL3:
    case CLASS_SOFTCHK_TOTAL4:
    case CLASS_SOFTCHK_TOTAL5:
    case CLASS_SOFTCHK_TOTAL6:
    case CLASS_SOFTCHK_TOTAL7:
    case CLASS_SOFTCHK_TOTAL8:
        usTrnMnemAddr = ( pItem->uchMinorClass - CLASS_SOFTCHK_TOTAL1 + TRN_TTL1_ADR );
        break;

    case CLASS_SOFTCHK_TOTAL9:
        usTrnMnemAddr = TRN_TTL9_ADR;
        break;

    case CLASS_SOFTCHK_TOTAL10:
    case CLASS_SOFTCHK_TOTAL11:
    case CLASS_SOFTCHK_TOTAL12:
    case CLASS_SOFTCHK_TOTAL13:
    case CLASS_SOFTCHK_TOTAL14:
    case CLASS_SOFTCHK_TOTAL15:
    case CLASS_SOFTCHK_TOTAL16:
    case CLASS_SOFTCHK_TOTAL17:
    case CLASS_SOFTCHK_TOTAL18:
    case CLASS_SOFTCHK_TOTAL19:
    case CLASS_SOFTCHK_TOTAL20:
        usTrnMnemAddr = ( pItem->uchMinorClass - CLASS_SOFTCHK_TOTAL10 + TRN_TOTAL10_ADR );
        break;

    case CLASS_FOREIGNTOTAL1:
    case CLASS_FOREIGNTOTAL2:
        usTrnMnemAddr = (pItem->uchMinorClass - CLASS_FOREIGNTOTAL1 + TRN_FT1_ADR);
        break;

    case CLASS_FOREIGNTOTAL3:
    case CLASS_FOREIGNTOTAL4:
    case CLASS_FOREIGNTOTAL5:
    case CLASS_FOREIGNTOTAL6:
    case CLASS_FOREIGNTOTAL7:
    case CLASS_FOREIGNTOTAL8:
        usTrnMnemAddr = (pItem->uchMinorClass - CLASS_FOREIGNTOTAL3 + TRN_FT3_ADR);
        break;

    default:
		NHPOS_ASSERT_TEXT(0, "**ERROR: Unknown total uchMinorClass in RflChkTtlAdr()");
        break;
    }
    return ( usTrnMnemAddr );
}


/*
*===========================================================================
** Format  : USHORT  RflChkTendAdr(ITEMTENDER *pItem);
*
*   Input  : ITEMTENDER *pItem
*   Output : none
*   InOut  : none
** Return  : USHORT    usAddress             -transaction mnemonic address
*
** Synopsis: This function checks tender # returning the correct transaction mnemonic
*            for the tender minor code.
*===========================================================================
*/
USHORT  RflChkTendAdr(ITEMTENDER *pItem)
{
	USHORT  usAddress = 0;
	UCHAR   uchMinorClassTemp = pItem->uchMinorClass;

	if (pItem->uchMinorClass == CLASS_TEND_TIPS_RETURN) {
		uchMinorClassTemp = ITEMTENDER_CHARGETIPS(pItem).uchMinorTenderClass;
		if (uchMinorClassTemp < CLASS_TENDER1) uchMinorClassTemp = CLASS_TENDER1;
	}

    if (uchMinorClassTemp <= CLASS_TENDER8) {
        usAddress = ((uchMinorClassTemp - (UCHAR)CLASS_TENDER1) + TRN_TEND1_ADR);
    } else if((uchMinorClassTemp >= CLASS_TENDER9) && (uchMinorClassTemp <= CLASS_TENDER11)) {
        usAddress = ((uchMinorClassTemp - (UCHAR)CLASS_TENDER9) + TRN_TEND9_ADR);
    } else if((uchMinorClassTemp >= CLASS_TENDER12) && (uchMinorClassTemp <= CLASS_TENDER20)) {
		usAddress = ((uchMinorClassTemp - (UCHAR)CLASS_TENDER12) + TRN_TENDER12_ADR);
    } else if((uchMinorClassTemp >= CLASS_FOREIGN1) && (uchMinorClassTemp <= CLASS_FOREIGN2)) {
		usAddress = ((uchMinorClassTemp - (UCHAR)CLASS_FOREIGN1) + TRN_FT1_ADR);
    } else if((uchMinorClassTemp >= CLASS_FOREIGN3) && (uchMinorClassTemp <= CLASS_FOREIGN8)) {
		usAddress = ((uchMinorClassTemp - (UCHAR)CLASS_FOREIGN3) + TRN_FT3_ADR);
    } else if(uchMinorClassTemp == CLASS_TEND_FSCHANGE) {
        if (pItem->lFSChange || pItem->lFoodStamp) {
            usAddress = TRN_FSCHNG_ADR;
        }
        if (pItem->lChange) {
            usAddress = TRN_REGCHG_ADR;
		}
	} else {
		NHPOS_ASSERT(uchMinorClassTemp <= CLASS_FOREIGN8);
        usAddress = TRN_TEND1_ADR;
	}

	return usAddress;
}

/*
*===========================================================================
** Format  : USHORT  RflChkDiscAdr(ITEMDISC *pItem);
*
*   Input  : ITEMDISC *pItem
*   Output : none
*   InOut  : none
** Return  : USHORT    usAddress             -transaction mnemonic address
*
** Synopsis: This function checks discount # and returns the addresses
*            used for fetching the appropriate transaction mnemonic.
*            This code is similar to function MldChkDiscAdr().
*===========================================================================
*/
USHORT  RflChkDiscAdr(ITEMDISC *pItem)
{
	USHORT  usAddress = 0;

    switch (pItem->uchMinorClass) {
    case CLASS_CHARGETIP:
    case CLASS_AUTOCHARGETIP:   /* V3.3 */
        usAddress =  (TRN_CHRGTIP_ADR);
        break;

    case CLASS_CHARGETIP2:      /* V3.3 */
    case CLASS_AUTOCHARGETIP2:  /* V3.3 */
        usAddress =  (TRN_CHRGTIP2_ADR);
        break;

    case CLASS_CHARGETIP3:      /* V3.3 */
    case CLASS_AUTOCHARGETIP3:  /* V3.3 */
        usAddress =  (TRN_CHRGTIP3_ADR);
        break;

    case CLASS_ITEMDISC1:
		usAddress = ItemDiscGetMnemonic_ITEMDISC1 (pItem->uchDiscountNo);
        break;

    case CLASS_REGDISC1:
        usAddress = (TRN_RDISC1_ADR);
        break;

    case CLASS_REGDISC2:
        usAddress = (TRN_RDISC2_ADR);
        break;

    case CLASS_REGDISC3:
        usAddress = (TRN_RDISC3_ADR);
        break;

    case CLASS_REGDISC4:
        usAddress = (TRN_RDISC4_ADR);
        break;

    case CLASS_REGDISC5:
        usAddress = (TRN_RDISC5_ADR);
        break;

    case CLASS_REGDISC6:
        usAddress = (TRN_RDISC6_ADR);
        break;

    default:
		NHPOS_ASSERT_TEXT(0, "**ERROR: Unknown total uchMinorClass in RflChkDiscAdr()");
        break;
    }

    return usAddress;
}

/*
*===========================================================================
** Format  : RFLUpdateStatusDescriptors (USHORT usMaps, ULONG  ulDescripToSetMap, ULONG  ulDescripToClearMap)
*
*   Input  : USHORT usMaps   - indicates which of the two status indicator bitmaps to update
*            ULONG  ulDescripToSetMap - bit map indicating which indicator bits to set
*            ULONG  ulDescripToClearMap - bit map indicating which indicator bits to clear
*   Output : none - side effect of displayed indicators may change
*   InOut  : none
** Return  : none
*
** Synopsis: This function will update the displayed status indicators in the descriptor
*            window of the user interface and then update the displayed status visible to
*            the user with the modified status indicators.
*
*            The status indicators are a bit map which indicates which displayed status indicators
*            are to be lit or not lit (the status descriptor functionality is a hold over from
*            the days of the 2170 which used a 2X20 LED display to display transaction data to
*            the operator. The display also had two rows of status indicators, one at the top
*            and one at the bottom of the 2X20 display, which displayed an indication as to
*            the status of different functional areas such as tender, communications, etc.
*
*            If no indicators are to be set then specify zero for ulDescripToSetMap.
*            If no indicators are to be cleared then specify zero for ulDescripToClearMap.
*===========================================================================
*/
VOID RflUpdateDisplayDescriptorMaps (USHORT usMaps, ULONG  ulDescripToSetMap, ULONG  ulDescripToClearMap)
{
	switch (usMaps & 0x0f) {
		case 0x00:
			break;
		case 0x01:
			flDispRegDescrControl &= ~ulDescripToClearMap;   // RflSetDescriptorMaps() usMaps == 0x01
			flDispRegDescrControl |= ulDescripToSetMap;
			break;
		case 0x02:
			flDispRegKeepControl &= ~ulDescripToClearMap;    // RflSetDescriptorMaps() usMaps == 0x02
			flDispRegKeepControl |= ulDescripToSetMap;
			break;
		case 0x03:
			flDispRegDescrControl &= ~ulDescripToClearMap;   // RflSetDescriptorMaps() usMaps == 0x03
			flDispRegDescrControl |= ulDescripToSetMap;
			flDispRegKeepControl &= ~ulDescripToClearMap;    // RflSetDescriptorMaps() usMaps == 0x03
			flDispRegKeepControl |= ulDescripToSetMap;
			break;
	}
}

/*
*===========================================================================
** Format  : RFLUpdateDisplayByMajorMinorClass(UCHAR uchMajorClass, UCHAR uchMinorClass, UCHAR fbSaveControl)
*
*   Input  : UCHAR uchMajorClass  - major class for the DispWrite() indicating action to perform
*            UCHAR uchMinorClass  - minor class for the DispWrite() indicating action to perform
*            UCHAR fbSaveControl  - display management option such as DISP_SAVE_TYPE_0
*   Output : none - displayed UI may be modified depending on value of fbSaveControl
*   InOut  : none
** Return  : none
*
** Synopsis: This function will format a REGDISPMSG variable and then use the DispWrite()
*            function to trigger a change in the displayed User Interface of GenPOS.
*
*===========================================================================
*/
VOID RFLUpdateDisplayByMajorMinorClass (UCHAR uchMajorClass, UCHAR uchMinorClass, UCHAR fbSaveControl)
{
	REGDISPMSG regDispMsg = {0};

	regDispMsg.uchMajorClass = uchMajorClass;
	regDispMsg.uchMinorClass = uchMinorClass;
	regDispMsg.fbSaveControl = fbSaveControl;
	DispWrite(&regDispMsg);
}

VOID RFLUpdateDisplayedStatusDescriptors (USHORT usMaps, ULONG  ulDescripToSetMap, ULONG  ulDescripToClearMap)
{
	if (usMaps) RflUpdateDisplayDescriptorMaps (usMaps, ulDescripToSetMap, ulDescripToClearMap);

	// see logic in function DispRegDisp20() for uchMajorClass of CLASS_UIFREGDISP
	RFLUpdateDisplayByMajorMinorClass (CLASS_UIFREGDISP, CLASS_UIFDISP_DESCONLY, DISP_SAVE_TYPE_4);
}

VOID RflClearDisplay (USHORT usMaps, ULONG  ulDescripToSetMap, ULONG  ulDescripToClearMap)
{
	if (usMaps) RflUpdateDisplayDescriptorMaps (usMaps, ulDescripToSetMap, ulDescripToClearMap);

	RFLUpdateDisplayByMajorMinorClass (CLASS_UIFREGOTHER, CLASS_UICLEAR, DISP_SAVE_TYPE_0);
}


#if 0
//  an example of a PARAMTDR file as of July 16, 2012
// #  tender key validation and assignment for auto processing of account numbers
// #
// #  the assignment tag is used to indicate an assignment rule.
// #    assignment rules map account numbers to tender keys.  tags are case sensitive.
// #
// #  the validation tag is used to indicate a validation rule.
// #    validation rule indicates when an account number is valid.
// #
// #  the numtype tag is used to indicate a #/Type validation rule.
// #     this rule is used to validate certain cases of #/Type data entry for Amtrak.
// #
// #  userules keyword indicates if rules following are to be used (1) or ignored (0).  can be used multiple times.
// #      Default is to use rules.  userules keyword can be used to cause a section of rules to be ignored.
// #       <userules>n</userules>
// #  required keyword indicates if a validation rule is to be required (1) or optional (0).
// #      Default is rule is optional.  required keyword can be used to cause a section of rules to be optional
// #      or required as a part of validation.
// #       <required>n</required>
// #  ignoretender keyword indicates that a tender key should not use automatic account rules.  this is used
// #      to indicate that when a tender specifying a particular tender key is being processed that the
// #      auto account type determination rules should not be used.  just use the tender key instead.
// #      <ignoretender>n</ignoretender>
// #  tender indicates the tender key 2 through 20 to assign to the rule.  Tender key 1 is for cash only.
// #       <tender>nn</tender>
// #  number indicates the pattern for matching an account number.  Any digit is represented by either a
// #       period (.) or a question mark (?).  so 4? matches 40 through 49.  ranges can be specified by enclosing
// #       the list of possible digits in square brackets.  4[0128] matches 40, 41, 42, and 48.
// #       <number>nnnn</number>
// #  length indicates the number of digits for the account type.  specify a range using a dash.
// #       for instance a range of 12 to 16 digits would be 12-16.  If you leave off the second
// #       number after the dash then a default of 255 is used so more than 11 digits would be 12-
// #       <length>xx</length>
// #
// #  a range of account numbers can be specified by using the <range> and </range> tags.  The format
// #  is as follows:
// #      <range>
// #      <first>nnnn</first>
// #      <last>nnnn</last>
// #      </range>
// #
// #  patterns are processed in the order they are in the file so the first match found is used.
// #  for instance if patterns are 909 with length of 7 then 90 with length of 7 then 9091444 will match
// #  first pattern and 9081234 would match the second pattern.  if patterns are in different order
// #  as in 90 with length of 7 then 909 with length of 7, both account numbers would match the first
// #  pattern.  So patterns should be listed in the file from most specific to most general.
// #
// #  Version:  .03
// #  Date:     03/23/2012
// #
// #  Update log
// # 01/19/2010  D. Parliman  updated rules for different types of cards
// #  Following assumes any CC Tender rules are defined prior to EPD\Emeal
// # 03/23/2012  R. Chambers updated comments for new account number validation functionality for Amtrak.
// #
// # Rename this file to PARAMTDR and put in \Database folder under \Saratoga, prior to Ap start
// <accountrules>
// <userules>1</userules>
// 
// # Following ignoretender rules are used with tender assignment only.
// # The validation processing ignores any ignoretender rules specified.
// <ignoretender>1</ignoretender>
// <ignoretender>2</ignoretender>
// <ignoretender>3</ignoretender>
// <ignoretender>4</ignoretender>
// <ignoretender>5</ignoretender>
// <ignoretender>6</ignoretender>
// <ignoretender>9</ignoretender>
// <ignoretender>10</ignoretender>
// <ignoretender>11</ignoretender>
// <ignoretender>12</ignoretender>
// <ignoretender>13</ignoretender>
// <ignoretender>14</ignoretender>
// <ignoretender>15</ignoretender>
// <ignoretender>16</ignoretender>
// <ignoretender>17</ignoretender>
// <ignoretender>18</ignoretender>
// <ignoretender>19</ignoretender>
// <ignoretender>20</ignoretender>
// 
// # #/Type validation rules follow.  These are assigned to a specific
// # tender key or total key.
// <numtype>
// 	<tender>10</tender>
// 	<length>10</length>
// </numtype>
// 
// #  Tender 8 defined for Emeals
// <assignment>
// 	<tender>8</tender>
// 	<number>999</number>
// 	<length>12-16</length>
// </assignment>
// <assignment>
// 	<tender>8</tender>
// 	<number>991</number>
// 	<length>14</length>
// </assignment>
// #  Tender 7 defined for EPD
// <assignment>
// 	<tender>7</tender>
// 	<number>[12345678]</number>
// 	<length>12-</length>
// </assignment>
// 
// # Tender key account number validation starts here.
// # These rules are used to determine if an account number is valid for a tender key.
// # If the tender tag is not specified then the rule applies to all tenders.
// #
// # The rules are processed in the order in which they are listed in the file so the
// # first validation rule is checked first and then the next and then the next.
// #
// # If the rule specifies a tender then the rule applies only to that tender
// # whether it is required or optional.
// #
// # More general rules that are required should be used first because the first
// # required rule that is not passed will cause validation to fail.
// #
// # Next should come more specific required rules especially rules for a particular tender.
// #
// # Next should come optional rules.
// #
// # If it is desired to use the Luhn Mod 10 check sum validation on
// # account numbers then use the luhnmode10 tag and specify a value of one (1).
// #
// # General required rule to validate all account numbers for checksum, beginning digit, and length.
// <validation>
// 	<required>1</required>
// 	<luhnmod10>1</luhnmod10>
// </validation>
// #<validation>
// #	<tender>3</tender>
// #	<required>1</required>
// #	<number>[3456]</number>
// #	<length>16</length>
// #</validation>
// 
// # Next check the specific length is one of the following for all account numbers
// # All optional rules must specify both number and length tags and fields.
// # The first optional rule that validates the account number will stop processing.
// <validation>
// 	<required>0</required>
// 	<number>[3456]</number>
// 	<length>12</length>
// </validation>
// <validation>
// 	<required>0</required>
// 	<number>[3456]</number>
// 	<length>14</length>
// </validation>
// <validation>
// 	<required>0</required>
// 	<number>[3456]</number>
// 	<length>16</length>
// </validation>
// <validation>
// 	<required>0</required>
// 	<number>[3456]</number>
// 	<length>18</length>
// </validation>
// </accountrules>
// 
// # Following are the rules that are used with Returns/Exchanges/Refunds to
// # enforce specific business rules about Returns/Exchanges/Refunds.
// #  - userules indicates if return rules are to be enforced, value of 1, or not, value of 0.
// <returnsrules>
// <userules>1</userules>
// 
// # Following ignoretender rules are used with tender assignment only.
// # The returns processing ignores any ignoretender rules specified.
// #<ignoretender>1</ignoretender>
// #<ignoretender>2</ignoretender>
// #<ignoretender>3</ignoretender>
// 
// # Some tenders such as cash or Train Delay Voucher should be consolidated
// # in the case of split tenders.  Those tenders to be consolidated need
// # to be so indicated.
// <consolidatetender>1</consolidatetender>
// <consolidatetender>2</consolidatetender>
// 
// <globalrules>
// # indicate if credit must be in batch of NetePay server or not.
// # if 1 then any credit must be in batch and not settled.  if 0 then
// # whether in batch or not is ignored.
// <batchonly>1</batchonly>
// 
// # indicate if credit must be approved or not.
// # if 1 then any credit must be EPT processed and approved.  if 0 then
// # whether approved or not is ignored.
// <approvedonly>1</approvedonly>
// 
// # indicate if credit in Stored file is allowed or not.
// # if 1 then any credit tender that has been Stored but not forwarded
// # may still be used for a return
// <allowstored>0</allowstored>
// 
// # indicate if Preauth credit not yet Captured is allowed or not.
// # if 1 then any Preauth credit tender that has not yet been Captured
// # may still be used for a return.
// <allowpreauth>0</allowpreauth>
// 
// # indicate if only same type of tender can be used in a return or not.
// # if 1 then only a tender that has been used in the transaction may be
// # used in the return.  if 0 then any tender may be used.
// # indicate if cash must go to cash and credit must go to credit
// # if 1 then amounts of the tenders are checked and limits enforced.
// <liketolike>1</liketolike>
// </globalrules>
// 
// <creditcheck>
// 	<lastdigits>4</lastdigits>
// 	<cardtype>
// </creditcheck>
// 
// #  Visa card type specification
// <cardtype>
// 	<type>1</type>
// 	<number>4</number>
// 	<length>16</length>
// #	<trailing>4</trailing>
// </cardtype>
// 
// </returnsrules>
#endif
/****** End of Source ******/