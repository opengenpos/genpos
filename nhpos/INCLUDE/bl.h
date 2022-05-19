/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2172     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1999-           **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Buisuness Logic, Header File
* Category    : Buisiness Logic, NCR 2172 High Level System Application
* Program Name: bl.h
* --------------------------------------------------------------------------
* Compiler    : MS-VC++ Ver.6.00 by Microsoft Corp.
* --------------------------------------------------------------------------
* Abstract:     This header file defines Business Logic for the 2172
*               system application.
*
*               This file contains:
*                   1. Type Definitions
*                   2. Symbol Definitions
*                   3. Function Prototype Declarations
*
* --------------------------------------------------------------------------
* Update Histories
*
* Date       Ver.      Name         Description
* Sep-29-99  01.00.00  M.Teraki     Initial(for Prototype)
* Oct-08-99            M.Teraki     Add C++ Language support code
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
/* ====================================================================== *
 * Prologue
 * ====================================================================== */
#if !defined(BL_INCLUDED)
#define BL_INCLUDED

#if !defined(_INC_ECR)
#include "ecr.h"
#endif

#include <paraequ.h>

#include <csgcs.h>

#if defined(__cplusplus)
extern "C" {
#endif

#if (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif /* (_WIN32_WCE || WIN32) && _MSC_VER >= 800 */


/* ====================================================================== *
 * Data type / constants definitions
 * ====================================================================== */


#define	CMD_DEVICEENGINE_FUNC	1		//Set the Device Engine Function
#define CMD_AXCONTROL_FUNC		2		//Set the ActiveX Control Function
#define CMD_AXCONTROL_EXP		3		//Set the ActiveX Control Function for Expire Value
#define CMD_AXCONTROL_ST		4		//Set the ActiveX Control Function for Status
#define	CMD_FWRELOADLAY_FUNC	5		//Set the Framework Layout Reload Function
#define CMD_CALL_NAG_SCREEN		6		//Set the Nag Screen to pop up JHHJ
#define CMD_SET_OEP_GROUPS		7		//Set the Set OEP Groups Function
#define CMD_GET_MAX_OEP_BUTTONS	8		//Get the Maximum displayable buttons on OEP window - CSMALL
#define CMD_FWRELOADOEP_FUNC    9       //Set the function that will reload dynamic PLU button windows.
#define CMD_AUTO_SIGN_OUT		10		//Set the auto sign out
#define CMD_FORCE_SHUTDOWN		11		//Perform an application shutdown
#define CMD_DISPLAY_OEP_IMAGE	12		//Set the Set OEP Groups Function

/* Defines for VBOLock functionality TLDJR */
#define CMD_VBOCONTROL_FUNC    22
#define CMD_VBOCONTROL_EXP     23
#define CMD_VBOCONTROL_ST	   24	

#define CMD_FRAMEWORK_MSG	   25		//Process a FRAMEWORK message received vis the Server thread or other means.

/* ---------------------------------------------------------------------- *
 * Device Data Structure definition
 * ---------------------------------------------------------------------- */

typedef struct {
    ULONG ulDeviceId;   /* Device Identifier */
    UCHAR dummy[12];	/* reserved */
    ULONG ulLength;     /* Varid data length (from *pvAddr) [bytes] */
    VOID *pvAddr;       /* Pointer to notification data */
} _DEVICEINPUT;

/* Device Identifier specified in ((_DEVICEINPUT *)pv)->ulDeviceId */
#define BL_DEVICE_DATA_MSR			(0x0010)	/* MSR, pvAddr == MSR_BUFFER */
#define BL_DEVICE_DATA_SCANNER		(0x0020)	/* SCANNER */
#define BL_DEVICE_DATA_KEY			(0x0030)	/* KEYBOARD, KEYLOCK, KEYERROR... */
#define BL_DEVICE_DATA_DRAWER		(0x0040)	/* DRAWER */
#define BL_DEVICE_DATA_SERVERLOCK	(0x0050)	/* SERVER LOCK */
#define BL_WM_CHAR					(0x0060)	/* WM_CHAR message*/
#define BL_DEVICE_DATA_POWERDOWN	(0x0070)	/* POWER BUTTON TOUCHSCREEN*/
#define BL_DEVICE_DATA_VIRTUALKEY	(0x0080)	// Virtual keyboard event, PIF_VIRTUALKEYEVENT, KEYBOARD_VIRTUAL, pvAddr == VIRTUALKEY_BUFFER
#define BL_DEVICE_DATA_PINPAD       (0x0090)	// PINPad device event, PIF_VIRTUALKEYEVENT, pvAddr == KEYBOARD_VIRTUAL
#define BL_DEVICE_DATA_SIGCAP       (0x00A0)	// Signature Capture device event, PIF_VIRTUALKEYEVENT, pvAddr == KEYBOARD_VIRTUAL
#define BL_DEVICE_DATA_OPERATOR     (0x00B0)	// Operator Action event, PIF_VIRTUALKEYEVENT, pvAddr == KEYBOARD_VIRTUAL

/* ---------------------------------------------------------------------- *
 * Notification Data Structure definition
 * ---------------------------------------------------------------------- */

typedef struct {
    ULONG ulType;		/* Type of notification */
    UCHAR dummy[4];		/* reserved */
    union {
		_DEVICEINPUT Data;	/* Device Data */
    } u;
} BL_NOTIFYDATA;

// See PLU structure information in file plu.h
//   RECPLU
#define BL_PLU_INFO_FILTER_ADJ           0x00000001       // adjective filter, adjective PLUs must match usFilterValue, SpecWinFilter_Adj
#define BL_PLU_INFO_FILTER_ADJNONZERO    0x00000002       // adjective filter, adjective PLU varients must have non-zero price, SpecWinFilter_AdjNonZero
#define BL_PLU_INFO_FILTER_ADJ_LOGIC_EQ  0x00000000       // equal to (0), less than (1), greater than (2) for adjective filter value, SpecWinFilter_AdjValueLogic
#define BL_PLU_INFO_FILTER_ADJ_LOGIC_LT  0x00000004       // equal to (0), less than (1), greater than (2) for adjective filter value, SpecWinFilter_AdjValueLogic
#define BL_PLU_INFO_FILTER_ADJ_LOGIC_GT  0x00000008       // equal to (0), less than (1), greater than (2) for adjective filter value, SpecWinFilter_AdjValueLogic

typedef struct _tagBL_PLU_INFO {
	struct _tagBL_PLU_INFO  *pNext;
	WCHAR                    auchAdjName[16]; // Adjective name should be at least as big as PARA_ADJMNEMO_LEN
	WCHAR                    auchPluNo[16];   // PLU number should be at least as big as STD_PLU_NUMBER_LEN + 1
    WCHAR                    uchPluName[24];  // PLU name should be at least as big as STD_PLU_MNEMONIC_LEN + 1
    WCHAR                    uchPluNameAlt[24];  // Alternate PLU name should be at least as big as STD_PLU_MNEMONIC_LEN + 1
    UCHAR                    uchPluAdj;       // adjective for the PLU
    UCHAR                    uchPluAdjMod;    // adjective for the PLU as used with CLASS_PARAADJMNEMO
    UCHAR                    auchContOther[MAX_PLUSTS_SIZE];  // PLU status and control information re Plu.ParaPlu.ContPlu.auchContOther[]
    USHORT                   usDeptNo;        // department number the PLU is assigned to
    USHORT                   usGroupNo;       // the PLU group the PLU is assigned to
    USHORT                   usTableNo;       // the OEP file table number to use for finding the PLU group the PLU is assigned to
    USHORT                   usFilePart;      // the OEP file table number to use for finding the PLU group the PLU is assigned to
    ULONG                    ulUnitPrice[6];  // the PLU unit price for each adjective type should be at least as big as PLU_UNITPRICE_NUM               *//* ### ADD 2172 Rel1.0 (01/17/00) */
    ULONG                    ulCounter;       // counter used for access PLU list with function CliOpOepPluRead()
    ULONG                    ulInternalCounter;       // counter used for access PLU list with function CliOpOepPluRead()
	ULONG                    ulFilterOptions;    // bit map indicating PLU filters to apply to results
	USHORT                   usFilterValue;      // value used with filter if applicable.
} BL_PLU_INFO;

typedef struct _tagBL_CPN_INFO {
    ULONG   ulCouponAmount;
    USHORT  usCpnNo;
    WCHAR   aszCouponMnem[STD_CPNNAME_LEN + 1];
	UCHAR   uchAutoCPNStatus;
} BL_CPN_INFO;

typedef struct _tagBL_DEPT_INFO {
    USHORT  usDeptNo;
    WCHAR   aszDeptMnem[STD_DEPTPLUNAME_LEN + 1];
} BL_DEPT_INFO;

#define FRAMEWORKCONTROLTYPE_BUTTON        1
#define FRAMEWORKCONTROLTYPE_LABEL         2
#define FRAMEWORKCONTROL_INDIC_NOCHANGE    0  // used for control attribute.  indicates no change to attribute
#define FRAMEWORKCONTROL_INDIC_ENABLE      1  // used for control attribute.  indicate to enable/display/change attribute or object
#define FRAMEWORKCONTROL_INDIC_DISABLE     2  // used for control attribute.  indicates to disable/dismiss attribute or object

typedef  struct _tagFRAMEWORK {
	SHORT   sTerminalNumber;         // target terminal, -1, this terminal, 0 all terminals, 1-16 specific terminal number
	ULONG   controlbgcolor;          // COLORREF  0x000000 through 0xFFFFFF, set to 0xFFFFFFFF is no change
	ULONG   controltextcolor;        // COLORREF  0x000000 through 0xFFFFFF, set to 0xFFFFFFFF is no change
	ULONG   windefaultbtncolorbg;    // COLORREF  0x000000 through 0xFFFFFF, set to 0xFFFFFFFF is no change
	ULONG   windefaultbtncolortxt;   // COLORREF  0x000000 through 0xFFFFFF, set to 0xFFFFFFFF is no change
	USHORT  windefaultbtnwidth;      // width of buttons in grid units
	USHORT  windefaultbtnheight;     // height of buttons in grid units
	UINT	extFSC;            //Extended FSC data for one-touch entry keys, tenders, totals, tax mod, etc.
	USHORT  enablecontrol;     // 0 no change, 1 enable, 2 disable
	USHORT  displaywindow;     // 0 no change, 1 display window, 2 dismiss window
	USHORT  displaycontrol;    // 0 no change, 1 display window, 2 dismiss window
	USHORT  changetext;        // 0 no change, 1 change text
	USHORT  changebgcolor;     // 0 no change, 1 change text
	USHORT  changetextcolor;   // 0 no change, 1 change text
	USHORT  changeiconfile;    // 0 no change, 1 change icon file
	USHORT  sendconfirm;
	TCHAR   windowname[30];
	TCHAR   tcsName[30];
	TCHAR   tcsText[256];
	TCHAR   tcsIconfile[64];
	TCHAR   *tcsListOfStrings;    // pointer to a list of zero terminated strings
	USHORT  usCountListOfStrings; // count to how many strings are in list
} FRAMEWORK;

typedef struct _tagPRINTFILE {
	TCHAR    tcsFileName[50];            // contains the name of the file to be printed
}PRINTFILE;

typedef struct _tagEMPLOYEECHANGE {
	ULONG    ulEmployeeId;              // contains the Employee Id
	ULONG    usJobCode;                 // contains the Job Code
	TCHAR    tcsActionType[50];         // contains the name of the file to be printed
	TCHAR    tcsMnemonic[21];           // contains the mnemonic for an employee
	TCHAR    tcsStatus[50];             // contains the name of the file to be printed
}EMPLOYEECHANGE;

typedef struct _tagCASDATA_RTRV {
	ULONG	ulCashierNo;
	WCHAR	wchCashierName[PARA_CASHIER_LEN + 1];			//must synchronize this with CAS_NUMBER_OF_CASHIER_NAME
	USHORT	usStartupWindow;
	ULONG	ulGroupAssociations;
}CASDATA_RTRV;

typedef struct _tagGUESTCHECK_RTRV {
	SHORT    gcnum;              // contains the guest check number which may be negative for special commands
}GUESTCHECK_RTRV;

typedef struct _tagCASHIERACTION {
	TCHAR       tcsActionType[24];    // string indicating the cashier action such as paid-out, loan, etc.
	ULONG       ulEmployeeId;         // the employee id of the cashier doing the action
	DCURRENCY   lAmount;              // contains the amount for the cashier action such as paid-out, etc.
	ULONG       ulEmployeeIdPaidTo;   // the employee id of the cashier receiving the action if needed, see Paid Out
	USHORT      usTotalId;            // contains the total id or minor class identifying which total for loan, pick-up
	USHORT      usVoidIndic;          // indicates if not void (0) or void (1) amount
	TCHAR       tcsReference[24];     // contains any reference number or account number string for paid-out, no-sale, etc.
}CASHIERACTION;

typedef struct _tagEJENTRYACTION {
	TCHAR  tcsActionType[24];    // string indicating the electronic journal action such as first, next, search, etc.
	ULONG  consecnumber;         // the Connection Engine consecutive number
	ULONG  ulFilePositionHigh;   // the offset in the TOTALEJG file, high bits of offset
	ULONG  ulFilePositionLow;    // the offset in the TOTALEJG file, low bits of offset
}EJENTRYACTION;

typedef struct {
	LONG    lErrorCode;
	USHORT  consecnumber;
	SHORT   sTerminalNumber;        // terminal number of terminal to deliver the transaction to
	TCHAR   aszTempFileName [24];
	TCHAR   tcsActionType[32];
	UCHAR   uchUniqueIdentifier[28];
} TRANSACTION;

typedef struct {
	int     iObjectType;            // type of Connection Engine object in the union, BL_CONNENGINE_MSG_aaaaaaa below
	SHORT   sTerminalNumber;        // terminal number of terminal to deliver the transaction to
	ULONG   ulFlags;                // flags indicating special actions to take
	TCHAR   tcsTempFileName[16];    // name of temp file if large amount of data
} CONNENGINEMSG;

#define TCHARSTRINGARRAYSIZE  324
typedef struct {
	USHORT  usLength;            // type of Connection Engine object in the union, BL_CONNENGINE_MSG_aaaaaaa below
	TCHAR   tcsArray[1];         // terminal number of terminal to deliver the transaction to
	TCHAR   tcsArrayExtra[TCHARSTRINGARRAYSIZE];  // extra room to ensure struct is large enough.
} TCHARSTRING;

typedef struct {
	USHORT  usStringNumber;
} MACROACTION;

typedef struct {
	TCHAR  tcsActionType[24];        // string indicating the electronic journal action such as first, next, search, etc.
	ULONG  consecnumber;             // the Connection Engine consecutive number
	TCHAR  tcsQueryStatement[256];   // the actual query to be performed
} DATAQUERY;

typedef struct {
	ULONG  consecnumber;             // the Connection Engine consecutive number
	TCHAR  tcsActionType[24];        // string indicating the electronic journal action such as first, next, search, etc.
	TCHAR  tcsService[48];
	ULONG  uleventid;
	USHORT usStateCount;             // count used to drive a state machine engine
} STATECHANGE;

typedef  struct {
	TCHAR            *ptcString;
	CONNENGINEMSG    ConnEngine;
	int              iMessageTag;
	union {
		TRANSACTION      Transaction;
		FRAMEWORK        FrameWork;
		PRINTFILE        PrintFile;
		EMPLOYEECHANGE   EmployeeChange;
		GUESTCHECK_RTRV  GuestCheck_Rtrv;
		CASHIERACTION    CashierAction;
		EJENTRYACTION    EjEntryAction;
		MACROACTION      MacroAction;
		DATAQUERY        DataQuery;
		STATECHANGE      StateChange;
		TCHARSTRING      TcharString;
	} u;
} AllObjects;

typedef struct {
	ULONG  ulBufferSize;  // size of the buffer in TCHAR characters
	TCHAR  *ptcsBuffer;   // buffer to put the lines into
	FILE   *fpFile;       // if temporary file was created then this contains open file handle.
	int    nLines;        // number of lines in the buffer
	SHORT  sRetStatus;    // return status indicating succes or failure and failure codes
	SHORT  sContentType;  // indicates if result is something other than text such as XML, JSON, 
} DataQueryObject;

#define BL_DATAQUERY_CONTENT_UNKNOWN   0
#define BL_DATAQUERY_CONTENT_TEXT      1
#define BL_DATAQUERY_CONTENT_XML       2
#define BL_DATAQUERY_CONTENT_JSON      3
#define BL_DATAQUERY_CONTENT_HTML      4
#define BL_DATAQUERY_CONTENT_JPEG      5

#define BL_CONNENGINE_MSG_UNKNOWN_MSG      (-1)   // message is for an unknown object
#define BL_CONNENGINE_MSG_BUSINESS_OBJECT    0    // object returned is one of the BusinessLogic type of objects
#define BL_CONNENGINE_MSG_FRAMEWORK          1    // object returned is a FRAMEWORK object
#define BL_CONNENGINE_MSG_PRINTFILE          2    // object returned is a PRINTFILE object
#define BL_CONNENGINE_MSG_EMPLOYEECHANGE     3    // object returned is a EMPLOYEECHANGE object
#define BL_CONNENGINE_MSG_GUESTCHECK_RTRV    4    // object returned is a GUESTCHECK_RTRV object
#define BL_CONNENGINE_MSG_CASHIERACTION      5    // object returned is a CASHIERACTION object
#define BL_CONNENGINE_MSG_EJENTRYACTION      6    // object returned is a EJACTIONACTION object
#define BL_CONNENGINE_MSG_TRANSACTION        7    // object returned is a transaction that is stored in a temp file
#define BL_CONNENGINE_MSG_CONNENGINE         8    // object returned is a CONNENGINEMSG object that contains one of the other kinds.
#define BL_CONNENGINE_MSG_MACROACTION        9    // object returned is a MACROACTION object that specifies a control string to execute.
#define BL_CONNENGINE_MSG_DATAQUERY         10    // object returned is a DATAQUERY object that specifies a data query command to execute.
#define BL_CONNENGINE_MSG_STATECHANGE       11    // object returned is a STATECHANGE object that specifies a state change command to execute.
#define BL_CONNENGINE_MSG_SENDFILE         126    // object returned is a CONNENGINEMSG object that contains one of the other kinds.
#define BL_CONNENGINE_MSG_TCHARSTRING      127    // object returned is a CONNENGINEMSG object that contains one of the other kinds.

#define BL_CONNENGINE_FLAGS_UNIQUE_ID      0x00000001     // indicates make a copy of u.Transaction.uchUniqueIdentifier

/* Type of notification */
#define BL_INPUT_DATA		(0x01)	/* input from the out of BL */

/* button restriction operation code defines used with function BlRestrictButtonPressMask() */
#define BL_BUTTONRESTRICT_OP_READEXISTING    0    // the mask argument is ignored
#define BL_BUTTONRESTRICT_OP_ENABLE          1    // mask is bitwise Ored with existing button restriction mask to turn on bits
#define BL_BUTTONRESTRICT_OP_DISABLEEXCEPT   2    // mask is bitwise Anded with existing button restriction mask to clear all bits except specified bits
#define BL_BUTTONRESTRICT_OP_TOGGLE          3    // mask is bitwise Xored with existing button restriction mask to toggle bits
#define BL_BUTTONRESTRICT_OP_DISABLE         4    // mask is bitwise Not and then bitwise Anded with existing button restriction mask to clear bits

#define BL_BUTTONRESTRICT_OEP_ONLY       0x00000001   // indicates that only OEP CWindowButton::BATypeOEPEntry allowed
#define BL_BUTTONRESTRICT_MASK_ALL       0xffffffff   // used to set all (Enable) or clear all (Disable) restrictions


// Following #defines are used to indicate the operation codes to be
// used in function BOOL CALLBACK BlFwSetOepGroup(UCHAR * pszGroups, UCHAR uchTableNo)
// located in FrameworkWndText.cpp that is used to traverse the the
// array of OEP groups when processing an OEP window such as for
// condiments or for Auto Combination Coupons.  See usage in files
// itcoupon.c and sloep.c for handling dynamically generated OEP windows.
#define BL_OEPGROUP_INCREMENT      100
#define BL_OEPGROUP_RESET          101
#define BL_OEPGROUP_DECREMENT      102
#define BL_OEPGROUP_DEFAULTGROUP   103
#define BL_OEPGROUP_STR_INCREMENT     "\x64\0\0"  // 100, BL_OEPGROUP_INCREMENT to INCREMENT the OEP window functionality
#define BL_OEPGROUP_STR_RESET         "\x65\0\0"  // 101, BL_OEPGROUP_RESET to reset the OEP window functionality
#define BL_OEPGROUP_STR_DECREMENT     "\x66\0\0"  // 102, BL_OEPGROUP_DECREMENT to decrement the OEP window functionality
#define BL_OEPGROUP_STR_DEFAULTGROUP  "\x67\0\0"  // 103, BL_OEPGROUP_DEFAULTGROUP to decrement the OEP window functionality
#define BL_OEPGROUP_STR_PREAUTH       "\xc9\0\0"  // 201, display of Preauth Tenders in function ItemDisplayPreauthChooseDialog(), SLOEP_SPL_ASK
#define BL_OEPGROUP_STR_REASONCODE    "\xca\0\0"  // 202, display of Reason Codes in function ItemDisplayOepChooseDialog()
#define BL_OEPGROUP_STR_AUTOCOUPON    "\xcb\0\0"  // 203, display of Auto Coupon in function ItemAutoCouponCheck()
#define BL_OEPGROUP_STR_OPENCHECK     "\xcc\0\0"  // 204, display of Open Guest Checks in function TrnDisplayFetchGuestCheck()


//--------------------------
// Following defines used with BlUifSupIntervent() to indicate the success
// or failue of the Supervisor entering a code for Supervisor intervention.
// For details as to how this function is used see the FrameworkWndItem class
// which uses it for determining if window requiring Supervisor Intervention
// should be popped up or not.  See method CFrameworkWndItem::PopupWindow().
#define  BLI_SUPR_INTV_APPROVED    0
#define  BLI_SUPR_INTV_FAILED     -1
#define  BLI_SUPR_INTV_CLEARED    -2

// usClear options for function BlUifSupIntervent()
#define  BLI_SUPR_INTV_READ       0
#define  BLI_SUPR_INTV_CLEAR      1     // will clear or reset the Supervisor Intervention indicator
#define  BLI_SUPR_INTV_SET        2     // will set the Supervisor Intervention indicator


// usMnemonicType defines for function BlFwIfGetMemoryResidentMnemonic()
#define BLI_MNEMOTYPE_P20_TRANS        1
#define BLI_MNEMOTYPE_P21_LEADTHRU     2

/* ---------------------------------------------------------------------- *
 * Call Back Function Structure definition
 * ---------------------------------------------------------------------- */

#ifdef	CALLBACK
typedef DWORD (CALLBACK* BLCBPROC)(DWORD, VOID*, DWORD);
typedef DWORD (CALLBACK* BLAXPROC)(ULONG*);
typedef DWORD (CALLBACK* BLAYPROC)(VOID);
typedef DWORD (CALLBACK* BOEPPROC)(UCHAR*, UCHAR uchTableNo);
typedef DWORD (CALLBACK* BOEPIMAGE)(UCHAR uchCommand, UCHAR uchDeptNo, UCHAR uchTableNo, UCHAR uchGroupNo);
typedef DWORD (CALLBACK* BOEPPROC2)(UCHAR*);
typedef DWORD (CALLBACK* BTIMEOUT)(ULONG time, SHORT mode);
typedef DWORD (CALLBACK* BMSGPROC)(VOID*);
#endif


/* ====================================================================== *
 * Function prototype declarations
 * ====================================================================== */

#ifdef __BLNOTIFY_C__
#define BUSINESSLOGIC_API __declspec(dllexport)
#else
#define BUSINESSLOGIC_API __declspec(dllimport)
#endif

BUSINESSLOGIC_API USHORT BlSetFrameworkVersionNumber (CONST TCHAR * ptszFrameworkVersionNumber);

BUSINESSLOGIC_API BOOL BlInitialize(TCHAR *pVersion);
BUSINESSLOGIC_API BOOL BlInitializeOepTable(VOID);
BUSINESSLOGIC_API BOOL BlFinalizeAndClose(VOID);
BUSINESSLOGIC_API BOOL BlInitHttp(VOID *pvDummy);
BUSINESSLOGIC_API BOOL BlProcessTimerList(VOID *pvDummy);
BUSINESSLOGIC_API int  BlInitConnEngine(int nPort, HWND hWinHandle, UINT wReceiveMsgId);
BUSINESSLOGIC_API int  BlConnEngineSendButtonPress (TCHAR *tcsWindowName, TCHAR *tcsButtonName, TCHAR *tcsActionType, int iExtendedFsc, TCHAR *tcsArgument);
BUSINESSLOGIC_API int  BlScriptSendButtonPress (TCHAR *tcsWindowName, TCHAR *tcsButtonName, TCHAR *tcsActionType, int iExtendedFsc, TCHAR *tcsArgument);
BUSINESSLOGIC_API BOOL BlDelayBalance(VOID);

BUSINESSLOGIC_API int  BlProcessConnEngineMessageAndTag(TCHAR **ptcBuffer, VOID *pObject);
BUSINESSLOGIC_API int  BlProcessDataQueryStatement(TCHAR *ptcBuffer, VOID *pObject);

BUSINESSLOGIC_API BOOL BlUninitialize(VOID *pvDummy);
BUSINESSLOGIC_API LONG _cdecl BlNotifyData(CONST BL_NOTIFYDATA *, VOID *);
BUSINESSLOGIC_API USHORT _cdecl BlGetKeyboardLockPosition (VOID);
BUSINESSLOGIC_API ULONG  _cdecl BlRestrictButtonPressMask (int iOp, ULONG  ulMask);
BUSINESSLOGIC_API SHORT  _cdecl BlUifSupIntervent (USHORT usClear);
BUSINESSLOGIC_API USHORT _cdecl BlGetNoticeBoardStatus (int iTestComs);
BUSINESSLOGIC_API SHORT _cdecl BlSubmitFileToFilePrint (TCHAR *aszFileName);
BUSINESSLOGIC_API SHORT _cdecl BlProcessEmployeeChange (EMPLOYEECHANGE  *pEmployeeChange);
BUSINESSLOGIC_API SHORT _cdecl BlProcessGuestCheck_Rtrv (GUESTCHECK_RTRV  *pGuestCheck_Rtrv);
BUSINESSLOGIC_API SHORT _cdecl BlProcessCashierAction (CASHIERACTION  *pCashierAction);
BUSINESSLOGIC_API SHORT _cdecl BlProcessEjEntryAction (EJENTRYACTION  *pEjEntryAction);
BUSINESSLOGIC_API SHORT _cdecl BlProcessMacroAction (MACROACTION  *pMacroAction);
BUSINESSLOGIC_API SHORT _cdecl BlProcessDataQuery (DATAQUERY  *pDataQuery);
BUSINESSLOGIC_API SHORT _cdecl BlProcessStateChange(STATECHANGE  *pStateChange);
BUSINESSLOGIC_API SHORT _cdecl BlProcessTransaction (TRANSACTION  *pTransaction);
BUSINESSLOGIC_API SHORT _cdecl BlProcessConnEngine (AllObjects  *pAllObjects);

BUSINESSLOGIC_API VOID _cdecl BlFwIfDeviceMessageSystemSendWait(VOID *pvDataArea);

#ifdef	CALLBACK
BUSINESSLOGIC_API BOOL BlSetFrameworkFunc(DWORD, VOID* pFunc);
#endif

// Following added for TOUCHSCREEN support
BUSINESSLOGIC_API LONG   _cdecl BlSendKeyboardData(CONST BL_NOTIFYDATA *pData, VOID *pResult);
BUSINESSLOGIC_API SHORT  _cdecl BlCheckAndSetTerminalLock(VOID);
BUSINESSLOGIC_API SHORT  _cdecl BlResetTerminalLock(SHORT sTermLockHandle);
BUSINESSLOGIC_API SHORT  _cdecl BlFwIfMldSetReceiptWindow(USHORT usScroll);
BUSINESSLOGIC_API SHORT  _cdecl BlFwIfMldCursorUp(USHORT usScroll);
BUSINESSLOGIC_API SHORT  _cdecl BlFwIfMldCursorDown(USHORT usScroll);
BUSINESSLOGIC_API SHORT  _cdecl BlFwIfMldNotifyCursorMoved(USHORT usScroll);
BUSINESSLOGIC_API USHORT _cdecl BlFwIfMldGetVosWinHandle(USHORT usScroll);
BUSINESSLOGIC_API VOID   _cdecl BlFwIfDisplayWrite(VOID);
BUSINESSLOGIC_API ULONG  _cdecl BlFwPmgGetLastError (VOID);
BUSINESSLOGIC_API USHORT _cdecl BlFwPmgCallBack(ULONG dwErrorCode, SHORT  usSilent);
BUSINESSLOGIC_API USHORT _cdecl BlFwPmgCallBackCheckOposReceipt(ULONG dwErrorCode);
BUSINESSLOGIC_API SHORT	 _cdecl BlFwIfMDCCheck(SHORT usAddress, UCHAR uchMaskData);
BUSINESSLOGIC_API UCHAR	 _cdecl BlFwIfReadMode(VOID);
BUSINESSLOGIC_API VOID   _cdecl BlFwIfParaRead(VOID *pDATA);
BUSINESSLOGIC_API USHORT _cdecl BlFwIfCheckMldSystemStatus(VOID);
BUSINESSLOGIC_API WCHAR* _cdecl BlFwIfTerminalLayout(VOID *pTermInfo);
BUSINESSLOGIC_API VOID*	 _cdecl BlFwIfSetFrameworkTouchWindow(VOID *pMainFramework);
BUSINESSLOGIC_API VOID*	 _cdecl BlFwIfGetFrameworkTouchWindow(void);
BUSINESSLOGIC_API VOID   _cdecl BlFwIfSetWindowHandle(VOID *pHwnd);
BUSINESSLOGIC_API VOID   _cdecl BlFwIfGetWindowHandle(VOID **pHwnd);
BUSINESSLOGIC_API int    _cdecl BlFwIfGetGroupPluInformationFirst(USHORT usTableNumber, USHORT usGroupNumber, USHORT usFilePart, ULONG ulFilter, USHORT usFilterValue, BL_PLU_INFO *pPluInfo);
BUSINESSLOGIC_API int    _cdecl BlFwIfGetGroupPluInformationNext(BL_PLU_INFO *pPluInfo);
BUSINESSLOGIC_API int    _cdecl BlFwIfGetPluInformation(UCHAR *aszPluNo, UCHAR uchPluAdj, BL_PLU_INFO *pPluInfo);
BUSINESSLOGIC_API int    _cdecl BlFwIfGetCouponInformation(USHORT usCouponNo, BL_CPN_INFO *pCpnInfo);
BUSINESSLOGIC_API int    _cdecl BlFwIfGetMemoryResidentMnemonic(USHORT usMnemonicType, USHORT usAddress, TCHAR tcsMnemonic[32]);
BUSINESSLOGIC_API int    _cdecl BlFwIfGetDepartInformation(USHORT usDepartNo, BL_DEPT_INFO *pDeptInfo);
BUSINESSLOGIC_API int    _cdecl BlFwIfGetAdjectiveMnemonic(USHORT usAddress, TCHAR tcsMnemonic[32]);
BUSINESSLOGIC_API int    _cdecl BlFwIfGetPrintModifierMnemonic(USHORT usAddress, TCHAR tcsMnemonic[32]);
BUSINESSLOGIC_API int    _cdecl BlFwIfGetPresetCashMnemonic(USHORT usAddress, TCHAR tcsMnemonic[32]);
BUSINESSLOGIC_API int    _cdecl BlFwIfGetReasonCodeMnemonic(USHORT usRecordNo, TCHAR tcsMnemonic[32]);
BUSINESSLOGIC_API int    _cdecl BlFwIfConvertPLU (UCHAR *aszPluNo, WCHAR *auchPluNo);
BUSINESSLOGIC_API BOOL   _cdecl BlFwIfIsPluFileUnLocked ();
BUSINESSLOGIC_API USHORT _cdecl BlFwIfUifACChgKBType(USHORT usKBType);
BUSINESSLOGIC_API LONG   _cdecl	BlFwIfGetCodePage(VOID);
BUSINESSLOGIC_API UCHAR  _cdecl BlFwIfSetPresetPLUNumber(UCHAR *ptchPLUName);
BUSINESSLOGIC_API LONG   _cdecl BlSendKeybrdData(CONST BL_NOTIFYDATA *pData, VOID *pResult);
BUSINESSLOGIC_API BOOL	 _cdecl BlFwIfGetCasData(CASDATA_RTRV *data);

// Following functions are used internally within the BusinessLogic subsystem DLL
// so we make sure they are not declared as BUSINESSLOGIC_API in order to
// eliminate compiler warnings.  Some of these have BUSINESSLOGIC_API analogues
// for instance function BlFwIfIntrnlGetGroupPluInformationNext() is the same
// as function BlFwIfGetGroupPluInformationNext().
int	  	BlFwIfGetMaxOEPButtons(UCHAR *pszMaxButtons); // CSMALL
LONG    BlFwIfSetOEPGroups(UCHAR *pszGroups, UCHAR uchTableNo);
int     BlFwIfIntrnlGetGroupPluInformationNext(BL_PLU_INFO *pPluInfo);
int     BlFwIfIntrnlGetGroupPluInformationFirst(USHORT usTableNumber, USHORT usGroupNumber, USHORT usFilePart, ULONG ulFilter, USHORT usFilterValue, BL_PLU_INFO *pPluInfo);
USHORT  BlIntrnlGetNoticeBoardStatus (int iTestComs);

// Following functions are used internally within the BusinessLogic subsystem DLL
// with the BlProcessTimerList() functionality to provide a way for creating a list of
// timers which when elapsed will trigger a function to be called.
LONG BlAddThreadFunctionToTimerList (SHORT  hsSemaphore, LONG (*pFunction)(VOID *pFunctionData), VOID *pFunctionData);
LONG BlRemoveThreadFunctionFromTimerList (LONG iIndex, SHORT  hsSemaphore, LONG (*pFunction)(VOID *pFunctionData), VOID *pFunctionData);

// Following functions are used to access Connection Engine Interface functionality.
int  ConnEngineObjectSendFile (USHORT usTerminalNo, USHORT usNoBytes);
SHORT ConnEngineSendGuestCheckList (GCF_STATUS_STATE *pRcvBuffer, USHORT usRcvActualCount, GCF_STATUS_HEADER *pGcf_FileHed, SHORT gcnRequest);
SHORT ConnEngineStartRequest(USHORT usTerminalNo, TCHAR *tcsTextBuffer);
TCHAR *ConnEngineTransposeSpecialStrings (TCHAR *ptcsOutput, USHORT usElementSize, TCHAR *ptcsInput);

// Following functions are used for the Connection Engine Interface State Change functionality
BOOL BlRegisterStateChangeService (CONST TCHAR *tcsServiceName );
BOOL BlRegisterStateChangeEvent (CONST TCHAR *tcsServiceName, ULONG ulEventId );
int  BlCheckStateChangeEvent (CONST TCHAR *tcsServiceName, ULONG ulEventId, STATECHANGE  *pStateChange );
int BlSetStateChangeEvent (CONST TCHAR *tcsServiceName, ULONG ulEventId, CONST TCHAR *tcsActionType, USHORT usNextState, STATECHANGE  *pStateChange );

/* ====================================================================== *
 * Epilogue
 * ====================================================================== */
#if (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif /* (_WIN32_WCE || WIN32) && _MSC_VER >= 800 */

#if defined(__cplusplus)
}
#endif

#endif	/* BL_INCLUDED */
/***************************************************************************
 * END OF FILE
 ***************************************************************************/
