/*
*===========================================================================
*
*   Copyright (c) 1992-1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Error Message and Power Down Recovery Module
* Category    : User Interface Enigne, 2170 System Application
* Program Name: UieError.C
* --------------------------------------------------------------------------
* Abstract:     The provided function names are as follows:
*
*                   UieSetErrorTbl()       - set string of error mnemonic
*                   UieErrorMsg()          - error operation entry
*                   UieErrorDisplay()      - display error code and mnemonic
*                   UieErrorPrintf()       - error mnemonic format
*                   UieErrorDescriptor()   - light descriptor of error
*                                            operation
*                   UieErrorKey()          - wait a keyboard input
*                   UieRefreshDisplay()    - refresh display
*                   UieRefreshDescriptor() - refresh descriptor
*
* --------------------------------------------------------------------------
* Update Histories                                                         

* Date       Ver.      Name         Description
* Mar-20-92  00.00.01  O.Nakada     Initial
* Sep-04-92  00.01.00  O.Nakada     Changed number of feed lines.
* Sep-25-92  00.01.05  O.Nakada     Removed case constant-expression of
*                                   UIE_RING_ERR in UiePopUp().
* Sep-26-92  00.01.05  O.Nakada     Added case constant-expression from
*                                   UIE_RING_SUCCESS to UIE_RING_READ in
*                                   UiePopUp().
* Oct-21-92  00.01.08  O.Nakada     Added UieRequestPrinterDescriptor().
* Feb-19-93  01.00.10  O.Nakada     Modfiy model dependent functions.
* Mar-05-93            O.Nakada     Modify ifdef macro defines.
* May-25-93  C1        O.Nakada     Removed input device control functions.
* Jun-05-93  C1        O.Nakada     Supports NCR 7852, 7870 and 7890.
* Jun-25-93  C2        O.Nakada     Modify model dependent functions.
* Sep-16-93  F1        O.Nakada     Divided the User Interface Engine.
* Mar-15-95  G0        O.Nakada     Modified all modules, correspond to
*                                   hospitality release 3.0 functions.
* Jul-12-95  G1        O.Nakada     Added UieWakeupTrack() function in
*                                   UieErrorMsg() function.
* Oct-31-95  G3        O.Nakada     Added condition type of UieErrorMsg().
*** NHPOS 1.4.x, NCR 7448  ***
* Aug-01-02  NHPOS 1.4 R.Chambers   Product donated to Georgia Southern University
*** NHPOS 2.0.x Touchscreen ***
* Mar-01-04  NHPOS 2.0 R.Chambers   Conversion from NCR 7448 to Touchscreen terminals
*** GenPOS 2.2.x Touchscreen ***
* Nov-19-14  GenPOS 2.2.0 R.Chambers  Source cleanup to remove old, before NCR 7448,
*                                     code that applied to NCR 2170 back in the 1990s.
* May-21-15  GenPOS 2.2.1 R.Chambers  replace UieWakeupTrack() with UieSwitchRingBuf()
*                                     in UieErrorMsgWithText().
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


/*
*===========================================================================
*   Include File Declarations
*===========================================================================
*/
#include <windows.h>
#include <tchar.h>
#include <string.h>
#include <stdio.h>

#include "ecr.h"
#include "pif.h"
#include "fsc.h"
#include "paraequ.h"
#include "pmg.h"
#include "vos.h"
#include "uie.h"
#include "uiel.h"
#include "uielio.h"
#include "uieio.h"
#include "uiering.h"
#include "uiedisp.h"
#include "uieerror.h"
#include "uietrack.h"
#include <rfl.h>
#include "BlFWif.h"
#include "para.h"
#include <csstbpar.h>
#include "display.h"
#include "bl.h"


static int UieMessageBoxExt (USHORT  usMsgBoxHandle, LPCTSTR lpText, LPCTSTR lpCaption, UINT uiFlags);
static SHORT   UieErrorPrintf(USHORT usDisplay, USHORT usRow, USHORT usColumn, CONST TCHAR FAR *pszFmt, ...);
static VOID    UieErrorDescriptor(USHORT usErrorCode, UCHAR uchAttr);
static UCHAR   UieErrorKey(USHORT fsCondition);
static VOID    UieRefreshDescriptor(USHORT usDisplay, ULONG *flDescriptor);

//-----------------------------------------------------------------------
// nEptReturn corresponds to the return code received by the server or
// from the pinpad device. The second column sCpmError is the internal
// error used to display the corresponding commented out error code.
//
// If new error codes are added to the EptErrorCode Table, these changes
// will also effect, DsiClient.cpp, pif.h, cph.h, eept.h
//
// WARNING:  define EEPTDLL_MAX_ERROR may also need to be changed with additions to this table.
//
typedef struct {
	short sCpmError;
	TCHAR auchTextResponse[EEPTDLL_ERROR_TEXT_MAX_LEN + 1];
} EptErrorCodes;

static EptErrorCodes ReturnsErrorCodeTable[] =
{                                         //12345678901234567890123456789012345678901234567
	{ RETURNS_MSG_TENDERMUSTMATCH,      _T("Not allowed. Tender must match original sale.") },        // PARATENDRETURNGLOBAL_LIKETOLIKEONLY
	{ RETURNS_MSG_ALREADYSETTLED,       _T("Not allowed. CC transaction already settled.") },         // PARATENDRETURNGLOBAL_BATCHONLY
	{ RETURNS_MSG_MUSTBEFINALIZED,      _T("Not allowed. CC transaction must be Finalized.") },       // PARATENDRETURNGLOBAL_ALLOWPREAUTH
	{ RETURNS_MSG_MUSTBEAPPROVED,       _T("Not allowed. CC must be Approved.") },                    // PARATENDRETURNGLOBAL_APPROVEDONLY
	{ RETURNS_MSG_MUSTBEFORWARDED,      _T("Not allowed. CC must be Forward.") },                     // PARATENDRETURNGLOBAL_ALLOWSTORED
	{ RETURNS_MSG_MUSTBESAMECARD,       _T("Not allowed. CC must be same card.") },                   // PARATENDRETURNGLOBAL_ACCOUNTTOACCOUNTONLY
	{ RETURNS_MSG_TRANSNOTALLOWED,      _T("Transaction not allowed.") },
	{ RETURNS_MSG_ORIGINALDECLINED,     _T("Not allowed. Original declined.") },                   // PARATENDRETURNGLOBAL_ALLOWDECLINED
	{ RETURNS_MSG_TENDERTOTALEXCEEDED,  _T("Not allowed. Total exceeded for this tender.") },      // PARATENDRETURNGLOBAL_ERROR_TENDERAMT
	{ RETURNS_MSG_TIPREQUIRESCC,        _T("Not allowed. Tip must be used with CC.") },            //
	{ RETURNS_MSG_TIPNOTONCARDSWIPED,   _T("Not allowed. Tip was not on CC swiped.") }             // PARATENDRETURNGLOBAL_TIPNOTONACCOUNT
};

static EptErrorCodes EptErrorCodeTable[] =
{           //12345678901234567890123456789012345678901234567
	//DSIServer Specific
	{-100,_T("Password Verified")},           // EEPTDLL_BASE_ERROR,  EEPTDLL_BASE_ALT_TENDER range begins here for LDT_ALTERNATE_TENDER_RQD
	{-101,_T("Queue Full")},
	{-102,_T("Password Failed - Disconnecting")},
	{-103,_T("System Going Offline")},
	{-104,_T("Disconnecting Socket")},
	{-105,_T("Refused ‘Max Connections")},
	{-106,_T("Duplicate Serial Number")},
	{-107,_T("Password Failed (Client/Server)")},
	{-108,_T("Password failed (Challenge/Response)")},
	{-109,_T("Internal Server Error")},
	
	//DSIClient Specific
	{-110,_T("General Failure")},
	{-111,_T("Invalid Command Format")},
	{-112,_T("Insufficient Fields")},
	{-113,_T("Global API Not Initialized")},
	{-114,_T("Timeout on Response")},
	{-115,_T("Empty Command String")},
	
	{-116,_T("In Process with server")},
	{-117,_T("Socket Error sending request")},
	{-118,_T("Socket already open or in use")},
	{-119,_T("Socket Creation Failed")},
	{-120,_T("Socket Connection Failed")},
	{-121,_T("Connection Lost")},
	{-122,_T("TCP/IP Failed to Initialize")},
	{-123,_T("Password Lookup Failed")},
	{-124,_T("Time Out waiting for server response")},
	{-125,_T("Connect Cancelled")},
	{-126,_T("128 bit CryptoAPI failed")},
	{-127,_T("Threaded Auth Started")},
	{-128,_T("Failed to start Event Thread")},
	{-129,_T("XML Parse Error")},
	{-130,_T("All Connections Failed")},
	{-131,_T("Server Login Failed")},
	{-132,_T("Initialize Failed")},
	
	{-133,_T("Global Response Length Error")},
	{-134,_T("Unable to Parse Response from Global")},
	{-135,_T("Global String Error: Check LAN Connectivity.")},   // EEPTINTERFACE_GLOBAL_STRING_ERROR
	{-136,_T("Weak Encryption Request Not Supported")},
	{-137,_T("Clear Text Request Not Supported")},
	{-138,_T("Error Occurred While Decrypting Request")},
	{-139,_T("Unrecognized Request Format")},
	{-140,_T("Record Not Found")},
	{-141,_T("Invalid Check Digit, Check Account Number")},
	{-142,_T("Merchant ID Missing")},
	{-143,_T("TStream Type Missing")},
	{-144,_T("Could Not Encrypt Response")},                     // EEPTDLL_MAX_ALT_TENDER range ends here for LDT_ALTERNATE_TENDER_RQD
	
	{-145,_T("Unknown Error")},
	
	{-146,_T("Invalid Transaction Type")},             // EEPT_RET_INVALIDTYPE
	{-147,_T("Invalid Operator ID")},
	{-148,_T("Invalid Memo")},
	{-149,_T("Invalid Account Number")},
	{-150,_T("Invalid Expiration Date")},              // EEPTDLL_ERR_EXPIRE_DATE
	{-151,_T("Invalid Authorization Code")},
	{-152,_T("Invalid Reference Number")},
	{-153,_T("Invalid Authorization Amount")},
	{-154,_T("Invalid Cash Back Amount")},
	{-155,_T("Invalid Gratuity Amount")},
	{-156,_T("Invalid Purchase Amount")},
	{-157,_T("Invalid Magnetic Stripe Data")},
	{-158,_T("Invalid PIN Block Data")},
	{-159,_T("Invalid Derived Key Data")},
	{-160,_T("Invalid State Code")},
	{-161,_T("Invalid Date of Birth")},
	{-162,_T("Invalid Check Type")},
	{-163,_T("Invalid Routing Number")},
	{-164,_T("Invalid TranCode")},                    // EEPT_RET_INVALIDCODE
	{-165,_T("Invalid Merchant ID")},
	{-166,_T("Invalid TStream Type")},
	{-167,_T("Invalid Batch Number")},
	{-168,_T("Invalid Batch Item Count")},
	{-169,_T("Invalid MICR Input Type")},
	{-170,_T("Invalid Driver’s License")},
	{-171,_T("Invalid Sequence Number")},
	{-172,_T("Invalid Pass Data")},
	{-173,_T("Invalid Card Type")},
	
	//Ver2000X Code Values
	{-174,_T("General Communication Failure")},
	{-175,_T("Invalid Com Port Value")},
	{-176,_T("Cannot Open Com Port")},
	{-177,_T("Cannot Set Line Signal")},
	{-178,_T("Failed To Set Prompt")},
	{-179,_T("Invalid Working Key Length")},
	{-180,_T("Key Type is Invalid or could not be set")},
	{-181,_T("Disk Write Failed!")},
	{-182,_T("Improper Prompt Length")},
	{-183,_T("The Account Number is Invalid")},
	{-184,_T("An amount of zero(0) is invalid")},
	{-185,_T("The amount is invalid")},
	{-186,_T("Control cannot find Settings")},
	{-187,_T("Cancelled")},
	{-188,_T("Master Key Not Available")},
	{-189,_T("No DUKPT Serial Number")},
	{-190,_T("Clear Key was Hit")},
	{-191,_T("Message too long")},
	{-192,_T("Initialization failed")},
	{-193,_T("Not finished processing request")},
	{-194,_T("Clear Command request sent")},
	{-195,_T("TimeOut occurred")},
	{-196,_T("Card Reader failed")},
	{-197,_T("Time Out")},
	{-198,_T("Time Out")},
	{-199,_T("Key Management Issue")},
	{-200,_T("Master Key Index must be 0-9 inclusive")},
	{-201,_T("Setup In Progress")},
	
	//EverestXReturn Code Values
	{-202,_T("General Failure")},
	{-203,_T("Timeout")},
	{-204,_T("Cancelled at POS")},
	{-205,_T("Cancelled at PinPad")},
	{-206,_T("Failed To Parse XML")},
	{-207,_T("Invalid Com Port Value")},
	{-208,_T("Invalid Command")},
	{-209,_T("Could Not Access Com Port")},
	{-210,_T("In Process!")},
	{-211,_T("Invalid Timeout Value")},
	{-212,_T("Invalid Amount")},
	{-213,_T("Invalid Account")},
	{-214,_T("Pertinent Error Information")},

	//User Defined Errors
	{-215,_T("Mag Stripe no longer valid")},
	{-216,_T("Void Return for postive amounts only")},     // EEPT_RET_VOID_RETURN
	{-217,_T("Last Mag Stripe is not set")},
	{-218,_T("Last Pin Block is not set")},
	{-219,_T("Last Key Serial Number is not set")},
	{-220,_T("DSIClient Not Available.")},
	
	//Extra Undocumented Error Codes
	{-221,_T("No Test Cards on Live Merchant ID")},
	{-222,_T("Merchant ID Not Found")},
	{-223,_T("Test Card used with Live Merchant ID")},
	{-224,_T("EPT Server Error - Check server logs.")},   // EEPTINTERFACE_DATACAP_SERVER_ERROR
	{-225,_T("EPT Other Error reported by Server.")},
	{-226,_T("Invalid/Missing Name data.")},
	{-227,_T("Invalid/Missing SS4 data.")},
	{-228,_T("EPT Server Error - Card batch limit.")},    // EEPTINTERFACE_BATCH_SIZE_LIMIT
	{-229,_T("EPT Server Error - Database error.")},      // EEPTINTERFACE_SERVER_DATABASE_ERROR
	{-230,_T("EPT Error - Account number not accepted.")}, // EEPTINTERFACE_DATACAP_ONLY_MAJOR_ACCEPTED
	{-231,_T("EPT Error - Invalid merchant category.")},   // EEPTINTERFACE_SERVER_DATABASE_ERROR
	{-232,_T("EPT Error - Transaction Not Complete.") },   // EEPTDLL_MULTIUSE_3328, the error text will indicate the cause of this error.
	{-233, 0},                                             // EEPTDLL_USE_ERROR_TEXT indicates to use the error text from the response

	{0, 0}     // EEPTDLL_MAX_ERROR is max value for sCpmError in this lookup table and may need to be changed.
};

/*
*===========================================================================
*   General Declarations
*===========================================================================
*/
PifSemHandle  usUieSyncError = PIF_SEM_INVALID_HANDLE;  /* synchronous semaphore */
PifSemHandle  usUieErrorMsg = PIF_SEM_INVALID_HANDLE;   /* exclusive semaphore   */

static  UIEERRORTBL UieErrorTbl;                        /* error message table   */
static  TCHAR       auchUieErrorOD[UIE_MAX_DISP_BUF];   /* buffer    */

static EptErrorCodes UieEptErrorCode = {EEPTDLL_USE_ERROR_TEXT, {0}};

/*
 * UieSetEptErrorText() is used to provide to the Error Dialog routine of Uie additional text
 * to better describe a problem.
 *
 * Typically used with Electronic Payment to provide to the operator the ErrorResponse or TextResponse element
 * of an Electronic Payment processing XML response from Datacap DSI Client-X, dsiPDCX, dsiEMVUS, etc. control.
 *
 * see also EEPTDLL_MULTIUSE_3328 and EEPTDLL_USE_ERROR_TEXT and LDT_EPT_ERROR_MULTIUSE.
 *
*/
VOID  UieSetEptErrorText (SHORT  sCpmError, TCHAR *tcsErrorText)
{
	_tcsncpy (UieEptErrorCode.auchTextResponse, tcsErrorText, EEPTDLL_ERROR_TEXT_MAX_LEN);
	UieEptErrorCode.auchTextResponse[EEPTDLL_ERROR_TEXT_MAX_LEN] = 0;
	UieEptErrorCode.sCpmError = sCpmError;
}

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    VOID UieSetErrorTbl(CONST UCHAR FAR *puchAddr);
*     Input:    puchAddr - Pointer of Error Message Table 
*
** Return:      nothing
*===========================================================================
*/
VOID  UieSetErrorTbl(CONST UCHAR FAR *puchAddr, UCHAR uchRecordSize)
{
    UieErrorTbl.puchAddr = puchAddr;
    UieErrorTbl.uchSize  = uchRecordSize;
}

/*
*===========================================================================
** Synopsis:    VOID UiePlayStanzaThread(CONST UCHAR FAR *puchAddr);
*     Input:    puchAddr - Pointer of Error Message Table 
*
** Return:      nothing
*
** Description: This function is a thread which takes commands for stanzas or
*               arrays of SOUND_NOTE (STANZA) and plays them.  The purpose of
*               this thread is to allow the playing of a stanza while also
*               displaying some kind of window such as an error dialog.
*===========================================================================
*/
static SHORT  sUieQueueStanzaQueue = -1;
static STANZA  *pCurrentStanza = 0;

VOID  UieQueueStanzaToPlay (CONST STANZA *pStanzaAddr)
{
	static STANZA  StanzaBuffer[200];
	static int     StanzaIndex = 0;
	int            i = 0;
	STANZA         *pCurrentStanza;
	STANZA         StanzaNote;

	for (i = 0; pStanzaAddr[i].usDuration != STANZA_DURATION_END_STANZA && pStanzaAddr[i].usFrequency != STANZA_FREQUENCY_LOOP; i++);

	if (StanzaIndex + i > sizeof(StanzaBuffer)/sizeof(StanzaBuffer[0])) {
		StanzaIndex = 0;
	}

	pCurrentStanza = StanzaBuffer + StanzaIndex;

	i = 0;
	do {

		StanzaNote = StanzaBuffer[StanzaIndex++] = pStanzaAddr[i++];
	} while (StanzaNote.usDuration != STANZA_DURATION_END_STANZA && StanzaNote.usFrequency != STANZA_FREQUENCY_LOOP);
	PifWriteQueue(sUieQueueStanzaQueue, pCurrentStanza);
}

VOID  UieQueueStanzaStopCurrent (void)
{
	STANZA   *pMyCopyCurentStanza = pCurrentStanza;

	if (pMyCopyCurentStanza != 0) {
		int      i;
		for (i = 0; i < 30; i++) {
			if (pMyCopyCurentStanza[i].usFrequency == STANZA_FREQUENCY_LOOP || pMyCopyCurentStanza[i].usDuration == STANZA_DURATION_END_STANZA) {
				break;
			}
			pMyCopyCurentStanza[i].usFrequency = pMyCopyCurentStanza[i].usDuration = STANZA_DURATION_END_STANZA;
		}
	}
}


VOID THREADENTRY UiePlayStanzaThread (void)
{
	if (sUieQueueStanzaQueue < 0)
		return;

    for (;;) {                              /* Endless Loop */
		STANZA  *pStanzaData;
        // Read the queue Semaphore for play stanza requests
        pCurrentStanza = pStanzaData = (STANZA *)PifReadQueue(sUieQueueStanzaQueue);
		PifPlayStanza (pStanzaData);
		pCurrentStanza = 0;
    }
}

VOID  UieQueueStanzaInit ()
{
	static CHAR CONST szUieQueueStanzaName[] = "QUEUESTANZA";

	sUieQueueStanzaQueue = PifCreateQueue(10);

    PifBeginThread(UiePlayStanzaThread, 0, 8000, PRIO_SERVER_T, szUieQueueStanzaName);
}


// ---------------------------------------------------------
// Following functions and structs are used to implement a primitive type
// of stack trace functionality
static  struct {
	USHORT usLdtErrorCode;
	USHORT usLdtExtendedErrorCode;
} myExtendedErrorCodeStack [20];

static int myExtendedErrorCodeStackIndex = 0;

VOID  UieAddExtendedErrorCode (USHORT  usLdtErrorCode, USHORT usLdtExtendedErrorCode)
{
	if (myExtendedErrorCodeStackIndex < sizeof(myExtendedErrorCodeStack)/sizeof(myExtendedErrorCodeStack[0])) {
		myExtendedErrorCodeStack[myExtendedErrorCodeStackIndex].usLdtErrorCode = usLdtErrorCode;
		myExtendedErrorCodeStack[myExtendedErrorCodeStackIndex].usLdtExtendedErrorCode = usLdtExtendedErrorCode;
		myExtendedErrorCodeStackIndex++;
	}
}

VOID  UieClearExtendedErrorCode (VOID)
{
	myExtendedErrorCodeStackIndex = 0;
}

int  UieRemoveExtendedErrorCode (USHORT  *usLdtErrorCode, USHORT *usLdtExtendedErrorCode)
{
	int iRetStatus = 1;

	if (myExtendedErrorCodeStackIndex > 0) {
		myExtendedErrorCodeStackIndex--;
		*usLdtErrorCode = myExtendedErrorCodeStack [myExtendedErrorCodeStackIndex].usLdtErrorCode;
		*usLdtExtendedErrorCode = myExtendedErrorCodeStack [myExtendedErrorCodeStackIndex].usLdtExtendedErrorCode;
	}
	else {
		*usLdtErrorCode = 0;
		*usLdtExtendedErrorCode = 0;
		iRetStatus = 0;
	}
	return iRetStatus;
}

// ---------------------------------------------------------

/*
*===========================================================================
** Synopsis:    UCHAR UieErrorMsg(USHORT usErrorCode);
*     Input:    fsCondition - accept key condition
*
** Return:      FSC_CLEAR  - clear key input
*               FSC_CANCEL - abort key input
*===========================================================================
*/
UCHAR  UieErrorMsgWithText(USHORT usErrorCode, USHORT fsCondition, TCHAR *pText)
{
    UCHAR   uchRet = 0, uchMode;
    USHORT  fsPrevious;

	if (usErrorCode == 0) {
		return FSC_CLEAR;
	}

    PifRequestSem(usUieErrorMsg);

    uchMode = UieMode.uchEngine;            /* backup mode position    */

    PifRequestSem(usUieSyncDisplay);
    fchUieActive &= ~UIE_ACTIVE_GET;        /* active for error        */
    PifReleaseSem(usUieSyncDisplay);

    fsPrevious = UieCtrlDevice(UIE_DIS_SCANNER);    /* disable scanner */
    
    if (! (fsCondition & UIE_EM_TONE)) {    /* enable error tone       */
        PifBeep(UIE_ERR_TONE);
    }

    if (fsCondition & UIE_EM_SUPR_TONE) {    /* enable Supervisor Intervention tone       */
        PifBeep(UIE_SPVINT_TONE);
    }
	UieErrorDisplay(usErrorCode, fsCondition, pText);   /* display error message   */
    UieErrorDescriptor(usErrorCode, UIE_DESC_ON);
	//touchscreen version clears the ring buffer after the OK button is
	//pressed non touchscreen version needs to clear the ring buffer here
	//if the Ring buffer is cleared here it will remove the clear key message
	//sent by pressing the OK on the message box
	if(PifSysConfig()->uchKeyType != KEYBOARD_TOUCH){
		UieClearRingBuf();                      /* clear ring buffer       */
	}

    /* --- if not initialized yet, trash it away ---
	 *  the function UiePutKeyData() is not going to put any key presses into the
	 *  ring buffer if UieMode.uchReally == UIE_INIT_MODE so we only want to look
	 *  for a key press with UieErrorKey() only if there is the possibility of 
	 *  one showing up. we ran into a problem at US Customs in which an EJ Near Full
	 *  condition was detected during GenPOS start up which caused the display of
	 *  an error message at which point GenPOS would hang on the tracking semaphore
	 *  in UieErrorKey() because no key presses were available since GenPOS was not
	 *  fully started and there was no User Interface or Layout displayed.
	 *    Richard Chambers, Jul-27-2015
	**/
    if (UieMode.uchReally != UIE_INIT_MODE) {   /* not initialized */
        /* --- if keyboard is disabled, trash it away --- */
        if ((UieMode.fchStatus & UIE_MODE_LOCK) == 0) {
			// if there is some kind of an error condition we will accept a Cancel Key press
			// no matter what in order to prevent a tracking error resulting in an infinite loop.
			// let us allow the operator to get out no matter what.
			uchRet = UieErrorKey(fsCondition | UIE_ABORT);
        }
	}
    
//    UieClearRingBuf();                      /* clear ring buffer       */
    UieMode.uchEngine = uchMode;            /* restore mode position   */

    /* --- restore display message --- */

    PifRequestSem(usUieSyncDisplay);
    UieBuf2Disp(UIE_OPER, UieRecovery.auchOperDisp, NULL);
    UieBuf2Disp(UIE_CUST, NULL, UieRecovery.auchCustDisp);
    PifReleaseSem(usUieSyncDisplay);
    UieErrorDescriptor(usErrorCode, UIE_DESC_OFF);
    UieCtrlDevice(fsPrevious);
    fchUieActive |= UIE_ACTIVE_GET;         /* active for normal       */

    PifReleaseSem(usUieErrorMsg);

	// UieWakeupTrack() is only used here and it appears it
	// can result in not calling UieSwitchRingBuf() in some cases
	// resulting in the UI semaphores not being released resulting in
	// GenPOS hanging after an error is displayed.  R.Chambers, May-21-2015
//    UieWakeupTrack();
    if (UieMode.uchReally != UIE_INIT_MODE) {   /* not initialized */
        /* --- if keyboard is disabled, trash it away --- */
        if ((UieMode.fchStatus & UIE_MODE_LOCK) == 0) {
			UieSwitchRingBuf();                         /* wake-up user thread */
		}
	}

    return (uchRet);
}

static struct {
	SHORT  sRetStatus;
	TCHAR  *ptString;
} ListOfReturnStatusStrings [] = {
	{STUB_M_DOWN,      _T("\r\nSTUB_M_DOWN: Master Terminal unreachable or Master Inquiry")},
	{STUB_BM_DOWN,     _T("\r\nSTUB_BM_DOWN: Backup Terminal unreachable.")},
	{STUB_BUSY,        _T("\r\nSTUB_BUSY: Request denied, target busy.")},
	{STUB_TIME_OUT,    _T("\r\nSTUB_TIME_OUT: Request failed, message timeout.")},
	{STUB_DUR_INQUIRY, _T("\r\nSTUB_DUR_INQUIRY: During Master Inquiry.")},
	{STUB_NOT_MASTER,  _T("\r\nSTUB_NOT_MASTER: Request denied, target not Master.")},
	{STUB_NO_BACKUP,   _T("\r\nSTUB_NO_BACKUP: Backup Master Terminal not provisioned.")},
	{STUB_BUSY_MULTI,  _T("\r\nSTUB_BUSY_MULTI: Request denied, target busy multi.")},
	{STUB_KPS_BUSY,    _T("\r\nSTUB_KPS_BUSY: Kitchen printer server is busy.")},
	{0, 0}
};

#if defined(UieErrorMsg)
UCHAR UieErrorMsgExtended_Special(USHORT usErrorCode, USHORT fsCondition, SHORT sRetStatus);
UCHAR UieErrorMsgExtended_Debug(USHORT usErrorCode, USHORT fsCondition, SHORT sRetStatus, char *aszFilePath, int nLineNo)
{
	int   iLen = 0;
	char  xBuff[256] = {0};

	iLen = strlen (aszFilePath);
	if (iLen > 30) {
		iLen = iLen - 30;
	}
	else {
		iLen = 0;
	}

	sprintf (xBuff, "UieErrorMsgExtended_Debug(): usErrorCode %d, fsCondition %d, File %s, lineno = %d", usErrorCode, fsCondition, aszFilePath + iLen, nLineNo);
	NHPOS_NONASSERT_TEXT(xBuff);
	return UieErrorMsgExtended_Special(usErrorCode, fsCondition, sRetStatus);
}

UCHAR  UieErrorMsgExtended_Special(USHORT usErrorCode, USHORT fsCondition, SHORT sRetStatus)
#else
UCHAR  UieErrorMsgExtended(USHORT usErrorCode, USHORT fsCondition, SHORT sRetStatus)
#endif
{
	int    iLoop;
	TCHAR  *ptString = 0;

	if (sRetStatus != 0) {
		for (iLoop = 0; ListOfReturnStatusStrings[iLoop].ptString; iLoop++) {
			if (sRetStatus == ListOfReturnStatusStrings[iLoop].sRetStatus) {
				ptString = ListOfReturnStatusStrings[iLoop].ptString;
				break;
			}
			if (sRetStatus == (ListOfReturnStatusStrings[iLoop].sRetStatus + STUB_RETCODE)) {
				ptString = ListOfReturnStatusStrings[iLoop].ptString;
				break;
			}
		}
	}
	return UieErrorMsgWithText(usErrorCode, fsCondition, ptString);
}

#if defined(UieErrorMsg)
UCHAR  UieErrorMsg_Debug(USHORT usErrorCode, USHORT fsCondition, char *aszFilePath, int nLineNo)
{
	int   iLen = 0;
	char  xBuff[256] = {0};

	iLen = strlen (aszFilePath);
	if (iLen > 30) {
		iLen = iLen - 30;
	}
	else {
		iLen = 0;
	}

	sprintf (xBuff, "UieErrorMsg_Debug(): usErrorCode %d, fsCondition %d, File %s, lineno = %d", usErrorCode, fsCondition, aszFilePath + iLen, nLineNo);
	NHPOS_NONASSERT_TEXT(xBuff);
	return UieErrorMsgWithText(usErrorCode, fsCondition, 0);
}
#else
UCHAR  UieErrorMsg(USHORT usErrorCode, USHORT fsCondition)
{
	return UieErrorMsgWithText(usErrorCode, fsCondition, 0);
}
#endif

USHORT UieHandlePopupWindowEventTrigger (USHORT usErrorCode)
{
	if (usErrorCode == LDT_ORDER_DEC_EVENT ){
		if(BlFwIfPopupWindowEventTrigger (SpecWinEventTrigger_WId_OrderDec)) {

			{
				REGDISPMSG      DispMsg = { 0 };

				DispMsg.uchMajorClass = CLASS_REGDISPCOMPLSRY;
				DispMsg.uchMinorClass = CLASS_REGDISP_EVENTTRIGGER;

				RflGetLead(&DispMsg.aszMnemonic[0], LDT_ORDER_DEC_ADR);

				//DispMsg.uchFsc = FSC_ORDER_DEC;   /* for 10N10D */
				DispMsg.fbSaveControl = DISP_SAVE_TYPE_4;
				DispWrite(&DispMsg);
			}

			return SUCCESS;
		}
		else {
			return (LDT_ORDER_DEC_ADR);
		}
	} 
	return usErrorCode;
}
/*
*===========================================================================
** Synopsis:    VOID UieErrorDisplay(USHORT usErrorCode);
*     Input:    usErrorCode - Error Code
*
** Return:      nothing
*===========================================================================
*/
#if defined(UieErrorDisplay)
VOID UieErrorDisplay_Special(USHORT usErrorCode, USHORT fsCondition, TCHAR  *szOptionalText);
VOID UieErrorDisplay_Debug (USHORT usErrorCode, USHORT fsCondition, TCHAR  *szOptionalText, char *aszFilePath, int nLineNo)
{
	char xBuff[128] = {0};
	int  iLen = strlen(aszFilePath);

	if (iLen > 30) iLen -= 30; else iLen = 0;
	sprintf (xBuff, "UieErrorDisplay_Debug(): usErrorCode %d, File %s, Line %d", usErrorCode, aszFilePath + iLen, nLineNo);
	NHPOS_NONASSERT_TEXT(xBuff);
	UieErrorDisplay_Special(usErrorCode, fsCondition, szOptionalText);
}

VOID UieErrorDisplay_Special(USHORT usErrorCode, USHORT fsCondition, TCHAR  *szOptionalText)
#else
VOID UieErrorDisplay(USHORT usErrorCode, USHORT fsCondition, TCHAR  *szOptionalText)
#endif
{
    static  TCHAR CONST UieErrorFmt1[] = _T("\tE%02u\n%-*s"); /* 2X20   */
    static  TCHAR CONST UieErrorFmt2[] = _T("\tE%02u");       /* 10N10D */
    static  TCHAR szMnemo[80];
	SYSCONFIG CONST  *pSysConfig = PifSysConfig(); 

	usErrorCode = UieHandlePopupWindowEventTrigger (usErrorCode);
	if(usErrorCode == SUCCESS && PifSysConfig()->uchKeyType == KEYBOARD_TOUCH)
	{
		CHARDATA  Data;

		//RJC TEMP  The CHARDATA sent depends on the keyboard page
		UieClearRingBuf();                      /* clear ring buffer       */
	//Put button info into ring buffer
		Data.uchFlags = 0;
		Data.uchASCII = 0x43;
		Data.uchScan = 0x00;
		
		UiePutKeyData(2,  &Data, NULL);
		return;
	}

    if (pSysConfig->uchOperType == DISP_2X20 || pSysConfig->uchOperType == DISP_LCD) {  /* 2X20 display or   LCD */
		CONST   TCHAR    *puchMsg;
		USHORT           usLength;

        /* --- make error message --- */
        memset(szMnemo, 0, sizeof(szMnemo));    /* clear buffer        */
		puchMsg = _T("");
		usLength = 2;

        if (UieErrorTbl.puchAddr) {             /* exist error message */

			//The eeptdll interface will return an error code that will be
			//greater than the current available LDT values. This value 
			//should be within the range EEPTDLL_BASE_LDTOFFSET to EEPTDLL_MAX_LDTOFFSET
			if (usErrorCode >= EEPTDLL_BASE_LDTOFFSET && usErrorCode <= EEPTDLL_MAX_LDTOFFSET)
			{
				if (usErrorCode == UieEptErrorCode.sCpmError) {
					puchMsg = UieEptErrorCode.auchTextResponse;
				} else {
					puchMsg = EptErrorCodeTable[usErrorCode - EEPTDLL_BASE_LDTOFFSET].auchTextResponse;
				}
				usLength = EEPTDLL_ERROR_TEXT_MAX_LEN;
			} else if (usErrorCode >= RETURNS_BASE_LDTOFFSET && usErrorCode <= RETURNS_MAX_LDTOFFSET)
			{
				puchMsg = ReturnsErrorCodeTable[usErrorCode - RETURNS_BASE_LDTOFFSET].auchTextResponse;
				usLength = EEPTDLL_ERROR_TEXT_MAX_LEN;
			} else if (usErrorCode < 0x7fff) {
				puchMsg  = (TCHAR*)UieErrorTbl.puchAddr + (usErrorCode - 1) * UieErrorTbl.uchSize;
				usLength = ((USHORT)UieErrorTbl.uchSize < usUieDispCol) ? ((USHORT)UieErrorTbl.uchSize) : (usUieDispCol);
			}
		
			_tcsncpy(szMnemo, puchMsg, usLength);
			if ((usErrorCode == LDT_ERR_ADR || LDT_EPT_ERROR_MULTIUSE == UieEptErrorCode.sCpmError) && UieEptErrorCode.auchTextResponse[0]) {
				_tcsncat(szMnemo, UieEptErrorCode.auchTextResponse, 78);
			}
			szMnemo[78] = 0;   // ensure zero terminator on string for end of string.
			UieEptErrorCode.auchTextResponse[0] = 0;
			UieEptErrorCode.sCpmError = EEPTDLL_USE_ERROR_TEXT;   // reset the use error text code back to the standard value in case changed.
        }

        /* --- display error message --- */
        UieErrorPrintf(DISP_OPER, 0, 0, UieErrorFmt1, usErrorCode, usUieDispCol, szMnemo);
    } else {                                    /* 10N10D display      */
        UieErrorPrintf(DISP_OPER, 0, 0, UieErrorFmt2, usErrorCode);
    }
	//Create Message box to notify user of error
	//and when the OK button is pressed the clear notification is sent
	if(pSysConfig->uchKeyType == KEYBOARD_TOUCH)
	{
		void    *pHwnd = 0;
		int     iRet;
		TCHAR   errorTitle[25] = {0};
		TCHAR   szMnemoTemp[512] = {0};
		CHARDATA  Data;

		// Modify the dialog box that is popped up so that we use the
		// word "Message" rather than the word "Error" for the error number.
		// We are also going to pre-prend the window title text containing
		// the error code number to the message that is displayed.
		wsprintf(errorTitle, _T("Message %hu"), usErrorCode);
		memset (szMnemoTemp, 0, sizeof(szMnemoTemp));
		_tcsncpy (szMnemoTemp, szMnemo, TCHARSIZEOF(szMnemo));
		if (szOptionalText) {
			// if there is optional text then add it to the text to put into the edit control.
			// multiple lines can be added and use STD_DIALOG_NEWLINE to insert a new line to
			// make the string have multiple lines.
			_tcsncat (szMnemoTemp, szOptionalText, sizeof(szMnemoTemp)/sizeof(szMnemoTemp[0]) - TCHARSIZEOF(szMnemo) - 1);
		}

		//SR 540, by adding this to the list of addresses to give
		//the OK and Cancel choices on the list, the user can choose cancel when it says 
		//EJ read error, and it will move onto the next terminal to read information
		//this is useful for if a certain terminal in the cluster is off.
		
		// Ok and Cancel button for leadthrus:
		//LDT_CLRABRT_ADR		Request Clear/Abort Entry
		//LDT_TIP_REQUEST_ADR   Prompt No Tips Entered
		//LDT_EJREAD_ADR        E/J Read Error
		//LDT_KTNERR_ADR		Remote Device Error

		if ((fsCondition & UIE_ABORT) || (usErrorCode == 0x35) ||
			(usErrorCode == 0x9F) ||
			(usErrorCode == 0x70) ||
			(usErrorCode == 0x03)) {
			iRet = UieMessageBoxExt(((fsCondition >> 12) & 0x0f), szMnemoTemp, errorTitle, MB_OKCANCEL | MB_ICONWARNING | MB_SYSTEMMODAL);
		}else{
			iRet = UieMessageBoxExt(((fsCondition >> 12) & 0x0f), szMnemoTemp, errorTitle, MB_OK | MB_ICONWARNING | MB_SYSTEMMODAL);
		}

		//RJC TEMP  The CHARDATA sent depends on the keyboard page
		UieClearRingBuf();                      /* clear ring buffer       */
		//Put button info into ring buffer
		Data.uchFlags = 0;
		if(iRet == IDCANCEL){
			Data.uchASCII = 0xBD;          // IDCANCEL indicated with FSC_CANCEL key press
			Data.uchScan = 0x00;
		} else if(iRet == IDABORT){
			Data.uchASCII = 0xFF;
			Data.uchScan = 0x29;           // FSC_AUTO_SIGN_OUT
		}else {
			// if nothing else then process this as a FSC_CLEAR
			Data.uchASCII = 0x43;          // IDOK indicated with FSC_CLEAR key press
			Data.uchScan = 0x00;
		}
		UiePutKeyData(2,  &Data, NULL);
	}
}


/*
*===========================================================================
** Synopsis:    SHORT UieErrorPrintf(USHORT usDisplay, USHORT usRow,
*                                    USHORT usColumn,
*                                    CONST UCHAR FAR *pszFmt, ...);
*     Input:    usDisplay - type of display
*               usRow     - start row
*               usColumn  - start column
*               pszFmt    - format string
*               ...       - Arguments
*
** Return:      length of format string
*===========================================================================
*/
static SHORT UieErrorPrintf(USHORT usDisplay, USHORT usRow, USHORT usColumn,
                     CONST TCHAR FAR *pszFmt, ...)
{
    SHORT  sResult;
    
    PifRequestSem(usUieSyncDisplay);

    /* --- make error message in auchUieErrorOD --- */
    sResult = UieChooseDisplay(usDisplay, usRow, usColumn, UIE_ATTR_NORMAL, pszFmt, (SHORT *)(&pszFmt + 1), auchUieErrorOD, NULL);

    /* --- display error message --- */
    UieBuf2Disp(usDisplay, auchUieErrorOD, NULL);

    PifReleaseSem(usUieSyncDisplay);
    return (sResult);
}


/*
*===========================================================================
** Synopsis:    VOID UieErrorDescriptor(USHORT usErrorCode, UCHAR uchAttr);
*     Input:    usErrorCode - error code
*               uchAttr     - descriptor attribute
*
** Return:      nothing
*===========================================================================
*/
static VOID UieErrorDescriptor(USHORT usErrorCode, UCHAR uchAttr)
{
    USHORT  usNumber = UIE_DESCRIPTOR_PAPER;

    if (PifSysConfig()->uchOperType != DISP_10N10D ||   /* 10N10D display */
        usNumber == UIE_PAPER_NOT_SUPPORT) {        /* not support    */
        return;                                     /* exit ...       */
    }

    if (usErrorCode == LDT_VSLPINS_ADR ||
        usErrorCode == LDT_VSLPREMV_ADR ||
        usErrorCode == LDT_SLPINS_ADR ||
        usErrorCode == LDT_PAPERCHG_ADR) {
        UieDescriptor(UIE_OPER, usNumber, uchAttr);
    }
}


/*
*===========================================================================
** Synopsis:    UCHAR UieErrorKey(USHORT fsCondition);
*     Input:    fsCondition - accept key condition
*
** Return:      FSC_CLEAR  - clear key input
*               FSC_CANCEL - abort key input
*===========================================================================
*/
static UCHAR UieErrorKey(USHORT fsCondition)
{
    FSCTBL  Ring, Fsc;
    SHORT   sResult;
    USHORT  usLength, usType;
    UCHAR   uchMode, auchBuf[UIE_RING_LEN_BUF];

    for (;;) {
        /* --- read data form ring buffer --- */

        sResult = UieReadRingBuf(&Ring, auchBuf, &usLength);

        if (sResult == UIE_SUCCESS) {           /* data exist           */
            if (Ring.uchMajor == FSC_MODE) {    /* mode change          */
                continue;                       /* next ...             */
#ifdef  FSC_RESET_LOG                                /* TAR #187987		*/
#pragma message("Support for FSC_RESET_LOG.")
            } else if (Ring.uchMajor == FSC_RESET_LOG) { /* not keyboard */
                continue;                       /* next ...             */
#endif
#ifdef  FSC_POWER_DOWN                               /* TAR #187987		*/
#pragma message("Support for FSC_POWER_DOWN.")
            } else if (Ring.uchMajor == FSC_POWER_DOWN) { /* not keyboard */
                continue;                       /* next ...             */
#endif
            } else if (Ring.uchMajor != FSC_KEYBOARD) { /* not keyboard */
                PifBeep(UIE_ERR_TONE);
                continue;                       /* next ...             */
            }

            /* --- convert from char data to FSC --- */

            uchMode = UieMode.uchEngine;
            usType  = UieConvertFsc(UieKbd.pCvt[uchMode][UieKbd.uchCType],
                                    UieKbd.pFsc[uchMode][UieKbd.uchFType],
                                    *(CHARDATA *)auchBuf,
                                    &Fsc);
        } else {                                /* data not exist       */
            /* --- read data from tracking buffer --- */

            sResult = UieReadTrack(&usType, &Ring, (UCHAR *)&Fsc);

            if (sResult != UIE_SUCCESS) {       /* buffer empty         */
                PifRequestSem(usUieSyncError);  /* block ...            */
                continue;                       /* next ...             */
            }

            if (Ring.uchMajor == FSC_MODE) {    /* mode change          */
                continue;                       /* next ...             */
            } else if (Ring.uchMajor != FSC_KEYBOARD) { /* not keyboard */
                PifBeep(UIE_ERR_TONE);
                continue;                       /* next ...             */
            }
        }

        /* --- check FSC data --- */

        switch (Fsc.uchMajor) {
        case FSC_RECEIPT_FEED:
            PmgFeed(PMG_PRT_RECEIPT, UIE_RECEIPT_FEED_ROW);
            continue;                           /* next ...             */

        case FSC_JOURNAL_FEED:
            PmgFeed(PMG_PRT_JOURNAL, UIE_JOURNAL_FEED_ROW);
            continue;                           /* next ...             */

        case FSC_CLEAR:
            return (Fsc.uchMajor);              /* exit ...             */

        case FSC_CANCEL:
            if (fsCondition & UIE_ABORT &&      /* accept abort key     */
                UieCheckAbort(Fsc.uchMajor, UieMode.uchEngine)) {
                return (Fsc.uchMajor);          /* exit ...             */
            }
            break;

		case FSC_AUTO_SIGN_OUT:
			{
				CHARDATA  Data;

				Data.uchFlags = 0;
				Data.uchASCII = 0xFF;
				Data.uchScan = 0x29;            // FSC_AUTO_SIGN_OUT
				UiePutKeyData(2,  &Data, NULL);
			}
            return (Fsc.uchMajor);              /* exit ...             */

        case FSC_AC:
            if (fsCondition & UIE_EM_AC) {      /* accept A/C key       */
                return (Fsc.uchMajor);          /* exit ...             */
            }
            break;

        default:                                /* other FSC            */
            break;
        }

        PifBeep(UIE_ERR_TONE);
    }
}



static struct sMessageBoxQueue {
	LPCTSTR lpText;
	LPCTSTR lpCaption;
	LPCTSTR lpServiceName;
	ULONG   ulEventId;
	UINT    uiFlags;
	HWND    hwndDialog;
	RECT	rectUie;
} MessageBoxQueueItems={0}, MessageBox2QueueItems={0}, MessageBoxQueueItemsExt[4]={{0},{0}};

static HWND hwndUieMessageBoxDlg = 0;
static RECT rectUieMessageBoxDlg = {300, 50, 0, 0};

static HWND hwndUieMessageBox2Dlg = 0;
static RECT rectUieMessageBox2Dlg = {300, 250, 0, 0};

static BOOL CALLBACK MessageBoxProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
	HWND   hDlgItem, hDlgItem_2, hDlgItem_3, hDlgItem_4;
	HICON  hWarningIcon;
	int    iTitleLength = 0, iDialog = 0;
	struct sMessageBoxQueue *pMessageBoxQueue;

    switch (message) 
    {
		case WM_INITDIALOG:
			hDlgItem_2 = GetDlgItem (hwndDlg, 1031 /*IDC_DIALOG_MESSAGEBOX_2*/);
			hDlgItem_3 = GetDlgItem (hwndDlg, 1032 /*IDC_DIALOG_MESSAGEBOX_3*/);
			hDlgItem_4 = GetDlgItem (hwndDlg, 1033 /*IDC_DIALOG_MESSAGEBOX_4*/);

			if (hDlgItem_2) {
				pMessageBoxQueue = &MessageBoxQueueItemsExt[0];
				iDialog = 2;
			} else if (hDlgItem_3) {
				pMessageBoxQueue = &MessageBoxQueueItemsExt[1];
				iDialog = 3;
			} else if (hDlgItem_4) {
				pMessageBoxQueue = &MessageBoxQueueItemsExt[2];
				iDialog = 4;
			} else {
				hwndUieMessageBoxDlg = hwndDlg;
				pMessageBoxQueue = &MessageBoxQueueItems;
				iDialog = 1;
			}
			pMessageBoxQueue->hwndDialog = hwndDlg;
			if (pMessageBoxQueue->rectUie.left == 0) {
				pMessageBoxQueue->rectUie.left = 300;
				pMessageBoxQueue->rectUie.top = iDialog * 75;
			}
			SetWindowText (hwndDlg, pMessageBoxQueue->lpCaption);
			SetDlgItemText (hwndDlg, 1025 /* IDC_EDIT1_LINE */, pMessageBoxQueue->lpCaption);
			SetDlgItemText (hwndDlg, 1021 /* IDC_EDIT_DIALOG_MESSAGE */, pMessageBoxQueue->lpText);

			hDlgItem = GetDlgItem (hwndDlg, IDOK);
			if (hDlgItem) {
				ShowWindow (hDlgItem, SW_SHOW);
			}

			hDlgItem = GetDlgItem (hwndDlg, IDCANCEL);
			if (hDlgItem) {
				if ((pMessageBoxQueue->uiFlags & MB_OKCANCEL) != 0) {
					ShowWindow (hDlgItem, SW_SHOW);
				}
				else {
					ShowWindow (hDlgItem, SW_HIDE);
				}
			}

			hDlgItem = GetDlgItem (hwndDlg, 1024/* IDC_STATIC_ICON */);
			hWarningIcon = LoadIcon (NULL, IDI_WARNING);
			if (hDlgItem) {
				if ((pMessageBoxQueue->uiFlags & MB_ICONWARNING) != 0) {
					ShowWindow (hDlgItem, SW_SHOW);
				}
				else {
					ShowWindow (hDlgItem, SW_HIDE);
				}
			}

			SetWindowPos (hwndDlg, HWND_TOPMOST, rectUieMessageBoxDlg.left, rectUieMessageBoxDlg.top, 0, 0, SWP_NOSIZE);
			break;

        case WM_COMMAND: 
            switch (LOWORD(wParam)) 
            {
				case IDABORT:
                    // Fall through. 
                case IDOK: 
                    // Fall through. 
                case IDCANCEL: 
					hDlgItem_2 = GetDlgItem (hwndDlg, 1031 /*IDC_DIALOG_MESSAGEBOX_2*/);
					hDlgItem_3 = GetDlgItem (hwndDlg, 1032 /*IDC_DIALOG_MESSAGEBOX_3*/);
					hDlgItem_4 = GetDlgItem (hwndDlg, 1033 /*IDC_DIALOG_MESSAGEBOX_4*/);

					if (hDlgItem_2) {
						pMessageBoxQueue = &MessageBoxQueueItemsExt[0];
					} else if (hDlgItem_3) {
						pMessageBoxQueue = &MessageBoxQueueItemsExt[1];
					} else if (hDlgItem_4) {
						pMessageBoxQueue = &MessageBoxQueueItemsExt[2];
					} else {
						hwndUieMessageBoxDlg = hwndDlg;
						pMessageBoxQueue = &MessageBoxQueueItems;
					}
					pMessageBoxQueue->hwndDialog = 0;
					hwndUieMessageBoxDlg = 0;
					GetWindowRect (hwndDlg, &rectUieMessageBoxDlg);
					GetWindowRect (hwndDlg, &(pMessageBoxQueue->rectUie));
                    EndDialog(hwndDlg, LOWORD(wParam)); 
                    return TRUE; 
            } 
    } 
    return FALSE; 
}

#define WU_UPDATE_TEXT   1024 + 2000

static BOOL CALLBACK MessageBox2Proc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
	HWND   hDlgItem;
	HICON  hWarningIcon;
	int    iTitleLength = 0;

    switch (message) 
    {
		case WM_INITDIALOG:
			hwndUieMessageBox2Dlg = hwndDlg;
			SetWindowText (hwndDlg, MessageBox2QueueItems.lpCaption);
			SetDlgItemText (hwndDlg, 1025 /* IDC_EDIT1_LINE */, MessageBox2QueueItems.lpCaption);
			SetDlgItemText (hwndDlg, 1021 /* IDC_EDIT_DIALOG_MESSAGE */, MessageBox2QueueItems.lpText);

			hDlgItem = GetDlgItem (hwndDlg, IDOK);
			if (hDlgItem) {
				if ((MessageBox2QueueItems.uiFlags & MB_OKCANCEL) != 0) {
					ShowWindow (hDlgItem, SW_SHOW);
				}
				else {
					ShowWindow (hDlgItem, SW_HIDE);
				}
			}

			hDlgItem = GetDlgItem (hwndDlg, IDCANCEL);
			if (hDlgItem) {
				if ((MessageBox2QueueItems.uiFlags & MB_OKCANCEL) != 0) {
					ShowWindow (hDlgItem, SW_SHOW);
				}
				else {
					ShowWindow (hDlgItem, SW_HIDE);
				}
			}

			hDlgItem = GetDlgItem (hwndDlg, 1024/* IDC_STATIC_ICON */);
			hWarningIcon = LoadIcon (NULL, IDI_WARNING);
			if (hDlgItem) {

				if ((MessageBox2QueueItems.uiFlags & MB_ICONWARNING) != 0) {
					ShowWindow (hDlgItem, SW_SHOW);
				}
				else {
					ShowWindow (hDlgItem, SW_HIDE);
				}
			}

			SetWindowPos (hwndDlg, HWND_TOPMOST, rectUieMessageBoxDlg.left, rectUieMessageBoxDlg.top, 0, 0, SWP_NOSIZE);
			break;

		case WU_UPDATE_TEXT:
			SetDlgItemText (hwndDlg, 1021 /* IDC_EDIT_DIALOG_MESSAGE */, MessageBox2QueueItems.lpText);
			break;

        case WM_COMMAND: 
            switch (LOWORD(wParam)) 
            {
				case IDABORT:
                    // Fall through. 
                case IDOK:
                    // Fall through. 
                case IDCANCEL: 
					hwndUieMessageBox2Dlg = 0;
                    DestroyWindow(hwndDlg);
					if (MessageBox2QueueItems.lpServiceName && MessageBox2QueueItems.ulEventId) {
						BlSetStateChangeEvent (MessageBox2QueueItems.lpServiceName, MessageBox2QueueItems.ulEventId, _T("ID_OK"), 1, 0);
					}
                    return TRUE; 
            }
    } 
    return FALSE; 
}

int UieMessageBoxSendMessage (ULONG message, ULONG wParam, ULONG lParam)
{
	if (hwndUieMessageBoxDlg) {
		SendMessage (hwndUieMessageBoxDlg, (UINT)message, (WPARAM)wParam, (LPARAM)lParam);
	}
	return 0;
}

static int UieMessageBoxExt (USHORT  usMsgBoxHandle, LPCTSTR lpText, LPCTSTR lpCaption, UINT uiFlags)
{
	HMODULE  hmPifModuleHandle;
	int             iRet = 0;
	DWORD           dwLastError = 0;
	void            *pNhposMainHwnd = 0;

	hmPifModuleHandle = GetModuleHandle (NULL);

	PifGetWindowHandle(&pNhposMainHwnd);

	if (usMsgBoxHandle == 0) {
		MessageBoxQueueItems.lpText = lpText;
		MessageBoxQueueItems.lpCaption = lpCaption;
		MessageBoxQueueItems.uiFlags = uiFlags;
		MessageBoxQueueItems.lpServiceName = 0;
		MessageBoxQueueItems.ulEventId = 0;

		iRet = DialogBox(hmPifModuleHandle, MAKEINTRESOURCE(143) /* IDD_DIALOG_MESSAGEBOX */, pNhposMainHwnd, (DLGPROC)MessageBoxProc);
		dwLastError = GetLastError();
	} else if (usMsgBoxHandle > 0 && usMsgBoxHandle < 4) {
		usMsgBoxHandle--;
		MessageBoxQueueItemsExt[usMsgBoxHandle].lpText = lpText;
		MessageBoxQueueItemsExt[usMsgBoxHandle].lpCaption = lpCaption;
		MessageBoxQueueItemsExt[usMsgBoxHandle].uiFlags = uiFlags;
		MessageBoxQueueItemsExt[usMsgBoxHandle].lpServiceName = 0;
		MessageBoxQueueItemsExt[usMsgBoxHandle].ulEventId = 0;

		iRet = DialogBox(hmPifModuleHandle, MAKEINTRESOURCE((149+usMsgBoxHandle)) /* IDD_DIALOG_MESSAGEBOX_2 */, pNhposMainHwnd, (DLGPROC)MessageBoxProc);
		dwLastError = GetLastError();
	} else {
		NHPOS_ASSERT_TEXT((usMsgBoxHandle < 4), "UieMessageBoxExt(): handle out of range");
	}

	return iRet;
}

int UieMessageBox (LPCTSTR lpText, LPCTSTR lpCaption, UINT uiFlags)
{
	return UieMessageBoxExt (0, lpText, lpCaption, uiFlags);
}

#if defined(UieMessageBoxNonModalPopUp)
VOID UieMessageBoxNonModalPopUp_Special (LPCTSTR lpText, LPCTSTR lpCaption, UINT uiFlags, LPCTSTR lpServiceName, ULONG  ulEventId);
VOID UieMessageBoxNonModalPopUp_Debug (VOID  *lpText, VOID *lpCaption, unsigned int uiFlags, VOID *lpServiceName, ULONG  ulEventId, char *aszFilePath, int nLineNo)
{
	char xBuff[128];
	int  iLen = strlen(aszFilePath);

	if (iLen > 30) iLen -= 30; else iLen = 0;
	sprintf (xBuff, "UieMessageBoxNonModalPopUp_Debug(): lpCaption %S, ulEventId %d, File %s, Line %d", lpCaption, ulEventId, aszFilePath + iLen, nLineNo);
	UieMessageBoxNonModalPopUp_Special (lpText, lpCaption, uiFlags, lpServiceName, ulEventId);
}

VOID UieMessageBoxNonModalPopUp_Special (LPCTSTR lpText, LPCTSTR lpCaption, UINT uiFlags, LPCTSTR lpServiceName, ULONG  ulEventId)
#else
VOID  UieMessageBoxNonModalPopUp (LPCTSTR lpText, LPCTSTR lpCaption, UINT uiFlags, LPCTSTR lpServiceName, ULONG  ulEventId)
#endif
{
	HMODULE  hmPifModuleHandle = GetModuleHandle (NULL);
	void     *pNhposMainHwnd = PifGetWindowHandle(0);
	HWND     hRet = 0;
	DWORD    dwLastError = 0;

	MessageBox2QueueItems.lpText = lpText;
	MessageBox2QueueItems.lpCaption = lpCaption;
	MessageBox2QueueItems.uiFlags = uiFlags;
	MessageBox2QueueItems.lpServiceName = lpServiceName;
	MessageBox2QueueItems.ulEventId = ulEventId;

	hRet = CreateDialog(hmPifModuleHandle, MAKEINTRESOURCE(148) /* IDD_DIALOG_MESSAGEBOX2 */, pNhposMainHwnd, (DLGPROC)MessageBox2Proc);
	dwLastError = GetLastError();
	if (hRet == 0) {
		char xBuff[128];

		sprintf (xBuff, "**WARNING: UieMessageBoxNonModalPopUp() - CreateDialog() GetLastError = %d ", dwLastError);
		NHPOS_ASSERT_TEXT((hRet != 0), xBuff);
	}
}

int UieMessageBoxNonModalPopUpUpdate (VOID  *lpText)
{
    BOOL    status;
	DWORD   dwLastError = 0;
	int     iRet = 0;

	if (hwndUieMessageBox2Dlg) {
		MessageBox2QueueItems.lpText = lpText;

		status = PostMessage(hwndUieMessageBox2Dlg, WU_UPDATE_TEXT, (WPARAM)lpText, 0);

		dwLastError = GetLastError();
		// dwLastError == 1400 means Invalid window handle.  ERROR_INVALID_WINDOW_HANDLE 
		// dwLastError == 1816 means Window Message queue getting full, ERROR_NOT_ENOUGH_QUOTA
	}
	return iRet;
}

int  UieDisplayMessageBoxExt (USHORT  usMsgBoxHandle, TCHAR *lpText, TCHAR *lpCaption, UINT uiFlags)
{
	return UieMessageBoxExt (usMsgBoxHandle, lpText, lpCaption, uiFlags);
}

int UieMessageBoxNonModalPopDown (VOID)
{
	if (hwndUieMessageBox2Dlg) {
		SendMessage (hwndUieMessageBox2Dlg, WM_COMMAND, IDOK, 0);
		hwndUieMessageBox2Dlg = 0;
	}

	return 0;
}

/*
*===========================================================================
** Synopsis:    VOID UieRefreshDisplay(VOID);
*
** Return:      nothing
*               This function refreshes the display area that is used for
*               the system status indicators.  This function has changed
*               as the underlying hardware has changed from the older NCR 7448
*               terminal to the new touch screen and LCD monitor terminals.
*               The main path taken within this function is for the DISP_LCD
*               operator type using function VosRefresh().
*
*               The code for the 2x20 and other displays is a holdover from
*               when this application was used on the now discontinued
*               NCR 7448 terminal and the even earlier NCR 2170 terminal.
*===========================================================================
*/
VOID UieRefreshDisplay(VOID)
{
    PifRequestSem(usUieSyncDisplay);

    /* --- refresh operator display --- */
    if (PifSysConfig()->uchOperType == DISP_LCD) {  /* LCD               */
        VosRefresh();
    } else {                                    /* 2X20 or 10N10D    */
        if (fchUieActive & UIE_ACTIVE_GET) {    /* active for normal */
            UieBuf2Disp(UIE_OPER, UieRecovery.auchOperDisp, NULL);
        } else {                                /* active for error  */
            UieBuf2Disp(UIE_OPER, auchUieErrorOD, NULL);
        }

        UieRefreshDescriptor(DISP_OPER, UieRecovery.flOperDesc);
    }

    /* --- refresh customer display --- */
    UieBuf2Disp(UIE_CUST, NULL, UieRecovery.auchCustDisp);
    UieRefreshDescriptor(DISP_CUST, UieRecovery.flCustDesc);

    PifReleaseSem(usUieSyncDisplay);
}


/*
*===========================================================================
** Synopsis:    VOID UieRefreshDescriptor(USHORT usDisplay,
*                                         ULONG flDescriptor);
*     Input:    usDisplay    - Display Type
*               flDescriptor - Descriptor Buffer 
*
** Return:      nothing
*               This function refreshes the descriptor area for the older
*               2x20 type of display used with the NCR 7448 terminal.
*               This old display uses LED indicators with a particular panel
*               layout with silk screen printed text.  We will allow the use
*               of only the standard indicators that are contained in the
*               first part of the descriptor indicator bit mask since those
*               are the only indicators that the display will allow.
*
*               This code is probably not useful any more but is retained
*               for now.
*===========================================================================
*/
static VOID UieRefreshDescriptor(USHORT usDisplay, ULONG *pulDescriptor)
{
	SYSCONFIG CONST  *pSysConfig = PifSysConfig(); 
    USHORT   usNo, usEnd;
	ULONG    flDescriptorTemp = *pulDescriptor;

    if ((usDisplay == DISP_OPER && pSysConfig->uchOperType == DISP_2X20) ||
        (usDisplay == DISP_CUST && pSysConfig->uchCustType == DISP_2X20)) {
        usEnd = UIE_MAX_2X20_DESC;
    } else {                                /* 10N10D display */
        usEnd = UIE_MAX_10N10D_DESC;
    }

    for (usNo = 0; usNo < usEnd; usNo++) {
        PifLightDescr(usDisplay, usNo, (UCHAR)(flDescriptorTemp & UIE_DESC_MASK));
        flDescriptorTemp >>= UIE_DESC_SHIFT;
    }
}


/* ======================================= */
/* ========== End of UieError.C ========== */
/* ======================================= */
