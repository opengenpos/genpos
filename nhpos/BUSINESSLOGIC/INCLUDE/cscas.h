/*========================================================================*\
*   Title              : Client/Server CASHIER module, Header file for user
*   Category           : Client/Server CASHIER module, NCR2170 US HOSPITALITY APPLICATION
*   Program Name       : CSCAS.H
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*   Update Histories   :
*
*   Date        Ver.Rev  : NAME        : Description
*   May-06-92 : 00.00.01 : M.YAMAMOTO  : Initial
*   Jun-06-93 : 00.00.01 : H.YAMAGUCHI : Change No of cashier from 16 to 32
*   Nov-06-95 : 03.01.00 : T.Nakahata  : R3.1 Initial
*   Mar-14-96 : 03.01.00 : M.Ozawa     : Add CasIndTermLock/CasIndTermUnLock
*                                        CasAllTermLock/CasAllTermUnLock
*   Aug-03-98 : 03.03.00 : M.Ozawa     : Enhanced to R3.3 Cashier Feature
*
*** NCR2172 ***
*
*   Oct-04-99 : 01.00.00 : M.Teraki    : Initial
*   Oct-05-99 : 01.00.00 : M.Teraki    : Add #pragma pack(...)
*   Dec-01-99 : 01.00.00 : hrkato      : Saratoga 
*
** GenPOS **
*
*   Apr-02-15 : 02.02.01 : R.Chambers  : moved Operator Message function prototypes from servmin.h
\*=======================================================================*/
/*=======================================================================*\
:   PVCS ENTRY
:-------------------------------------------------------------------------
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

/*
===========================================================================
    DEFINE
===========================================================================
*/

#include <paraequ.h>
#include <para.h>

//  See also defines in file csstbfcc.h which defines CLI_ values
#define CAS_NUMBER_OF_MAX_CASHIER  300          /* number of cashier (max) */
                                                /* cashier empty table size */
#define CAS_TERMINAL_NO_EXTENDED    256         // Used for extended number of terminals to support more than 16, Disconnected Satellite

// #define CAS_BACKUP_WORK_SIZE        450         /* same as OP_BACKUP_WORK_SIZE, Buffer size for back up */

/*------------------------------
    DEFINE FOR CASHIER STATUS
	    used in both Cashier record and memory resident mode data
        CasIf.fbCashierStatus[0] and WorkMode.auchCasStatus[0]
------------------------------*/ 
        
#define CAS_CASHIERSTATUS_0			0			//PDINU
#define CAS_NOT_CLOSE_OWN_CHECK     0x01        /* not close own check; same as MODE_NOT_CLOSE_OWN_CHECK */
#define CAS_NOT_NON_GUEST_CHECK     0x02        /* not non-guest check transaction */
#define CAS_NOT_GUEST_CHECK_OPEN    0x04        /* not guest check open */
#define CAS_TRAINING_CASHIER        0x08        /* training cashier status; same as MODE_TRAINING_CASHIER */
#define CAS_NOT_SINGLE_ITEM         0x10        /* not single item sales */
#define CAS_NOT_TRANSFER_TO         0x20        /* not check transfer from */
#define CAS_NOT_TRANSFER_FROM       0x40        /* not check transfer to */
#define CAS_NOT_CLOSE_ANY_CHECK     0x80        /* not close any check; same as MODE_NOT_CLOSE_ANY_CHECK */

#define CAS_OPERATOR_DENIED_AM      (CAS_TRAINING_CASHIER | CAS_NOT_NON_GUEST_CHECK | CAS_NOT_CLOSE_OWN_CHECK)  // indicates Operator denied and can not be enabled
#define CAS_OPERATOR_DISABLED_AM    (CAS_TRAINING_CASHIER | CAS_NOT_NON_GUEST_CHECK)                            // indicates Operator disabled and can be enabled again

/*------------------------------
    DEFINE FOR CASHIER STATUS
	    used in both Cashier record and memory resident mode data
        CasIf.fbCashierStatus[1] and WorkMode.auchCasStatus[1]
------------------------------*/ 
#define CAS_CASHIERSTATUS_1			1			//PDINU
#define CAS_NOT_SURROGATE           0x01        /* not surrogate sign-in */
#define CAS_FINALIZE_NON_GCF        0x02        /* release on finalize non-gcf */
#define CAS_FINALIZE_GCF            0x04        /* release on finalize gcf */
#define CAS_SECRET_CODE             0x08        /* requeset secret code */
#define CAS_NOT_CAS_INT             0x10        /* not cashier interrupt */
#define CAS_USE_TEAM                0x20        /* use team, by FVT comment, CasIf.uchTeamNo contains team number if not head operator */
#define CAS_NOT_DRAWER_B            0x40        /* not drawer A, 2172 */
#define CAS_NOT_DRAWER_A            0x80        /* not drawer B, 2172 */

/*------------------------------
    DEFINE FOR CASHIER STATUS
	    used in both Cashier record and memory resident mode data
        CasIf.fbCashierStatus[2] and WorkMode.auchCasStatus[2]
------------------------------*/ 
#define CAS_CASHIERSTATUS_2				2			//PDINU
#define CAS_OPEN_CASH_DRAWER_SIGN_IN    0x01        /* Opens the cash drawer on sign-in ***PDINU*/
#define CAS_ORDER_DEC_DEFAULT			0x02		/* We use terminal defaults order dec*/
#define CAS_ORDER_DEC_FORCE				0x04		/* We require order declare before itemization*/
#define CAS_OPEN_CASH_DRAWER_SIGN_OUT   0x08		// Opens the cash drawer on sign-Out ***PDINU
#define CAS_MSR_MANUAL_SIGN_IN			0x10		// JMASON Flag to prevent manual signing when using MSR
#define CAS_AUTO_SIGN_OUT				0x20		//this checks to see of the Auto Sign out is disabled
#define CAS_PIN_REQIRED_FOR_SIGNIN		0x40		// PIN or Secret Code is required as part of Sign-in
#define CAS_PIN_REQIRED_FOR_TIMEIN		0x80		// PIN or Secret Code is required as part of Time-in

/*------------------------------
    DEFINE FOR CASHIER STATUS
	    used in both Cashier record and memory resident mode data
        CasIf.fbCashierStatus[3] and WorkMode.auchCasStatus[3]
------------------------------*/ 
#define CAS_CASHIERSTATUS_3				3			//PDINU
#define CAS_PIN_REQIRED_FOR_SUPR		0x01		// PIN or Secret Code is required when Sign-in at SUPR Mode
#define CAS_PIN_AGING_ON				0x02		// Perform PIN or Secret Code aging requiring new PIN periodically
#define CAS_OPERATOR_DISABLED			0x04		// Disable the Operator not allowing Sign-in or Time-in
#define CAS_PROG_MODE_ALLOWED			0x08		// Allow access to Program Mode.  PIN is required for access
#define CAS_FUTURE_USE_1				0x10		// **this bit open for future use
#define CAS_FUTURE_USE_2				0x20		// **this bit open for future use
#define CAS_REG_MODE_PROHIBITED			0x40		// REG mode prohibited
#define CAS_REPEAT_CNTRL_STRING			0x80		// If Control String Id is set then Repeat Control String at beginning of each transaction

/*------------------------------
    DEFINE FOR CASHIER STATUS
        fbCashierStatus[4]
------------------------------*/ 
#define CAS_CASHIERSTATUS_4				4			//PDINU
#define CAS_REPEAT_WIN_DISPLAY			0x01		// If Window Id is set then Repeat the Window Display at beginning of each transaction


/*------------------------------
 *   DEFINE FOR CASHIER STATUS
 *       pCasif->ulCashierOption
 *   These flags indicate special processing in function CasSignIn()
 *   for special cases such as bar sign-in or surrgate sign-in.
------------------------------*/ 
#define CAS_WAI_SIGN_IN             0x01        /* waiter/surrogate sign-in */
#define CAS_MISC_SIGN_IN            0x02        /* misc. sign-in */

/*------------------------------
    DEFINE FOR RETURN VALUE
------------------------------*/

/* External error code for User interface

	These should be synchronized with the Employee Time Keeping codes as well.
**/

#define CAS_PERFORM              0         
#define CAS_FILE_NOT_FOUND      (-1)
#define CAS_CASHIER_FULL        (-2)
#define CAS_NOT_IN_FILE         (-3)        // same as ETK_NOT_IN_FILE
#define CAS_LOCK                (-5)    
#define CAS_NO_MAKE_FILE        (-6) 
#define CAS_NO_COMPRESS_FILE    (-7)

#define CAS_ALREADY_LOCK        (-10)       // same as ETK_LOCK
#define CAS_OTHER_LOCK          (-11)
#define CAS_DURING_SIGNIN       (-12)
#define CAS_DIFF_NO             (-13)
#define CAS_SECRET_UNMATCH      (-14)
#define CAS_NO_SIGNIN           (-15)
#define CAS_ALREADY_OPENED      (-16)
#define CAS_DELETE_FILE         (-17)
#define CAS_REQUEST_SECRET_CODE (-18)
#define CAS_NOT_ALLOWED         (-19)

#define CAS_NOT_MASTER          (-20)    // same as ETK_NOT_MASTER
#define CAS_CAS_DISABLED        (-22)
#define CAS_NOT_REGISTERED		(-177)   // This must be same value as UNREGISTERED_COPY in BlfWif.h


/* Internal error code  */

#define CAS_NOT_SIGNIN          (-31)
#define CAS_UNLOCK              (-32)
#define CAS_RESIDENT            (-33)
#define CAS_REGISTED            (-34)
#define CAS_ALLLOCK             (-35)
#define CAS_EOF                 (-36)
#define CAS_COMERROR            (-37)

#define CAS_CONTINUE            (-40)
#define CAS_NO_READ_SIZE        (-41)        

// Following are used to transform Network Layer (STUB_) errors into CAS_ errors
// Notice that error code is same ast STUB_ plus STUB_RETCODE.
#define  CAS_M_DOWN_ERROR        (STUB_RETCODE+STUB_M_DOWN)
#define  CAS_BM_DOWN_ERROR       (STUB_RETCODE+STUB_BM_DOWN)
#define  CAS_BUSY_ERROR          (STUB_RETCODE+STUB_BUSY)
#define  CAS_TIME_OUT_ERROR      (STUB_RETCODE+STUB_TIME_OUT)
#define  CAS_UNMATCH_TRNO        (STUB_RETCODE+STUB_UNMATCH_TRNO)
#define  CAS_DUR_INQUIRY         (STUB_RETCODE+STUB_DUR_INQUIRY)

#define MAINT_DBON			0x01	  /* Delayed Balance flag for EOD JHHJ */
#define MAINT_DBOFF			0x00
/*
===========================================================================
    TYPEDEF
===========================================================================
*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

// changes to this struct may also need changes in struct CASIF and CLIRESCASHIER
// and vice versa.
// see also functions Cas_AssignCasparaToCasif() and Cas_AssignCasifToCaspara().
// see also function IspRecvCas()
typedef struct{
    UCHAR   fbCashierStatus[PARA_LEN_CASHIER_STATUS];	//instances of CAS_NUMBER_OF_CASHIER_STATUS should be adjusted soon
    USHORT  usGstCheckStartNo;      // start of guest check number range assigned to cashier
    USHORT  usGstCheckEndNo;        // last of guest check number range assigned to cashier
    UCHAR   uchChgTipRate;
    UCHAR   uchTeamNo;              // team number if CasIf.fbCashierStatus[CAS_CASHIERSTATUS_1] & CAS_USE_TEAM
    UCHAR   uchTerminal;            // terminal number if specific terminal number assigned this cashier.
    WCHAR   auchCashierName[PARA_CASHIER_LEN];
    ULONG   ulCashierSecret;
	USHORT	usSupervisorID;         // Supervisor ID for Supervisor Mode if cashier has Supervisor privileges.
	USHORT	usCtrlStrKickoff;
	USHORT	usStartupWindow;
	ULONG	ulGroupAssociations;
}CAS_PARAENTRY;

typedef  struct {
	USHORT  usTermNo;
	ULONG   ulCashierNo;
	DATE_TIME   dtSignInDateTime;
} CAS_TERM_SIGNIN;

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
/*
===========================================================================
    PROTOTYPE
===========================================================================
*/

/*----------------------
    File Name
----------------------*/

/*----------------------
    USER I/F
----------------------*/

VOID    CasInit(VOID);
SHORT   CasCreatFile(USHORT usNumberOfCashier);
SHORT   CasCheckAndCreatFile(USHORT usNumberOfCashier);
SHORT   CasCheckAndDeleteFile(USHORT usNumberOfCashier);
SHORT   CasSignIn(CASIF *Casif);
SHORT   CasSignOut(CASIF *Casif);
SHORT   CasClose(ULONG ulCashierNo);
SHORT   CasAssign(CASIF *Casif);
SHORT   CasDelete(ULONG ulCashierNo);
SHORT   CasSecretClr(ULONG ulCashierNo);
SHORT   CasAllIdRead(USHORT usRcvBufferSize,ULONG *ausRcvBuffer);
SHORT   CasAllSignedInRead(USHORT usRcvBufferLen, CAS_TERM_SIGNIN *aulRcvBuffer);
SHORT   CasIndRead(CASIF *Casif);
SHORT   CasIndLock(ULONG ulCashierNo);
SHORT   CasIndUnLock(ULONG ulCashierNo);
SHORT   CasAllLock(VOID);
VOID    CasAllUnLock(VOID);
SHORT   CasIndTermLock(USHORT usTerminalNo);    /* R3.1 */
SHORT   CasIndTermUnLock(USHORT usTerminalNo);  /* R3.1 */
SHORT   CasAllTermLock(VOID);                   /* R3.1 */
VOID    CasAllTermUnLock(VOID);                 /* R3.1 */
SHORT   CasChkSignIn(ULONG *ausRcvBuffer);
SHORT   CasSendFile(VOID);
SHORT   CasRcvFile(UCHAR  *puchRcvData, USHORT  usRcvLen, UCHAR  *puchSndData, USHORT  *pusSndLen);
USHORT   CasConvertErrorWithLineNumber(SHORT sError, ULONG ulCalledFromLineNo);
#define CasConvertError(sError)  CasConvertErrorWithLineNumber(sError,__LINE__)
SHORT   CasPreSignOut(CASIF CONST *Casif);
SHORT   CasOpenDrawer(CASIF CONST *pCasIf, USHORT usDrawer);
SHORT   CasOpenPickupLoan(CASIF *pCasif);
SHORT   CasClosePickupLoan(CASIF *pCasif);
SHORT   CasDelayedBalance(UCHAR uchTermNo, ULONG ulState);

VOID	CasAutoSignOutReset();
VOID	ItemCasAutoOut(VOID);
SHORT	CasForceAutoSignOut(USHORT usMinorFsc);

// servopr.c functions that are also used by cscas.c as well
// so prototypes moved here from servmin.h to reduce compiler warnings.
SHORT SerOpClearOperatorMessageStatusTable (ULONG ulIndicator);
LONG SerOpSetOperatorMessageStatusTable (USHORT usTerminalNo, USHORT usFlags);
LONG SerOpGetOperatorMessageStatusTable (USHORT usTerminalNo, ULONG *pulStatus);

/*======= End of Define ========*/