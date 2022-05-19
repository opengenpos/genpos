/*

		NeighborhoodPOS Software Product Development Project
		Department of Information Systems
		College of Information Technology
		Georgia Southern University
		Statesboro, Georgia

		Copyright Georgia Southern University Foundation

*===========================================================================
* Title       : Charge Posting Manager, Header File
* Category    : Charge Posting Manager, NCR 2170 Charge Posting Application
* Program Name: CPM.H
* --------------------------------------------------------------------------
* Abstract:
*               This file contains:
*                   1. Symbol Definitions
*                   2. Data type Definitions
*                   3. Function Prototypes
*
* --------------------------------------------------------------------------
* Update Histories
*                                       
* Date      Ver.        Name            Description
* Nov-01-93 00.00.01 :  Yoshiko. Jim  : Initial
*
*** NCR2172 ***
*
* Oct-04-99 : 01.00.00 : M.Teraki     : Initial
*                                     : Add #pragma pack(...)
*** GenPOS ***
* Jun-17-15 : 02.02.01 : R.Chambers   : added define CPM_RET_SERVICE_ERROR, cleaned up comments
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

#if !defined(CPM_INCLUDED)
#define CPM_INCLUDED

#if !defined(_INC_PIF)
#include "pif.h"
#endif /* _INC_PIF */

/*
*===========================================================================
*   Define Declarations 
*===========================================================================
*/

/* Charge Post subsystem error codes
   These error codes are converted by function CpmConvertError()
   into LDT_ type codes for use by function UieErrorMsg() to
   display errors to the operator.

   These codes should be synchronized with similar values used by
   the lower level STUB_ error codes as well as the Totals
   error codes defined in file csttl.h and the Guest Check
   error codes defined in file csgcs.h
 */
/**     return value    
		These return values, defines beginning with EEPT_RET should
		have the same values and meanings as do the CPM_RET values
		which are defined in the include file CPM.H so if values
		are added here, they should be added to CPM.H,CSSTBEPT.H and vice versa.

		The values -100 to -300 are currently being used by EEPT_DLL interface
		to represent return error codes. If these values need to be changed or
		modified please refer to DataCap.cpp, Uieerror.C, CSSTBEPT.H, EEPT.h,
		and CPM.H
**/
#define     CPM_RET_AUTO_STORED    (1)            //external server error and auto storing is in effect, same as EEPT_RET_AUTO_STORED
#define     CPM_RET_SUCCESS         0             /* process succeed          */
#define		CPM_RET_COM_ERRORS	   (-10)          /* Error opening com port, same as EEPT_RET_COM_ERRORS	 */
#define     CPM_RET_NOT_MASTER     (-20)          /* I'm not master (O/S), same as EEPT_RET_NOT_MASTER     */
#define     CPM_RET_FAIL           (-31)          /* other communication err, same as EEPT_RET_FAIL  */
#define     CPM_RET_BUSY           (-32)          /* CPM is busy, same as EEPT_RET_BUSY   */
#define     CPM_RET_STOP           (-33)          /* CPM is stop process, same as EEPT_RET_STOP   */
#define     CPM_RET_OFFLINE        (-34)          /* CPM & PMS is offline, same as EEPT_RET_OFFLINE  */
#define     CPM_RET_LENGTH         (-35)          /* receive length error, same as EEPT_RET_LENGTH  */
#define     CPM_RET_PORT_NONE      (-36)          /* PMS is not provide, same as EEPT_RET_PORT_NONE  */
#define     CPM_RET_TIMEOUT        (-37)          /* time out error, same as EEPT_RET_TIMEOUT  */
#define     CPM_RET_BADPROV        (-38)          /* equipment provisioning wrong, same as EEPT_RET_BADPROV */
#define     CPM_RET_CANCELLED      (-39)          /* operation cancelled, same as EEPT_RET_CANCELLED  */
#define     CPM_RET_SERVERCOMMS    (-40)          /* server communication issue or TCP/IP issue, same as EEPT_RET_SERVERCOMMS  */
#define     CPM_RET_INVALIDDATA    (-41)          /* invalid data such as acct no, etc sent, same as EEPT_RET_INVALIDDATA  */
#define		CPM_RET_PINNEEDED      (-42)          // Pin is needed for a manual EBT, same as EEPT_RET_PINNEEDED
#define     CPM_RET_SERVICE_ERROR  (-43)          /* service error due to some platform or infrastructure problem such as missing Datacap control, same as EEPT_RET_SERVICE_ERROR   */

#define     CPM_RET_EEPT_START     EEPTDLL_BASE_ERROR     // Start of the EEPT codes

// Following are used to transform Network Layer (STUB_) errors into CPM_ errors
// Notice that error code is same as STUB_ plus STUB_RETCODE.
#define     CPM_M_DOWN_ERROR        (STUB_RETCODE+STUB_M_DOWN)
#define     CPM_BM_DOWN_ERROR       (STUB_RETCODE+STUB_BM_DOWN)
#define     CPM_BUSY_ERROR          (STUB_RETCODE+STUB_BUSY)
#define     CPM_TIME_OUT_ERROR      (STUB_RETCODE+STUB_TIME_OUT)
#define     CPM_UNMATCH_TRNO        (STUB_RETCODE+STUB_UNMATCH_TRNO)
#define     CPM_DUR_INQUIRY         (STUB_RETCODE+STUB_DUR_INQUIRY)

/**		
/**     terminal type   **/
#define     CPM_FROM_SELF           0           /* called from self term.   */
#define     CPM_FROM_NOT_SELF       1           /* called from other term.  */

/**     status of CPM  **/
#define     CPM_CHANGE_STOP         0
#define     CPM_CHANGE_START        1

/** tally required type **/
#define     CPM_TALLY_READ          1           /* read tally               */
#define     CPM_TALLY_RESET         2           /* read & reset tally       */

/* request function code */
#define     CPM_FC2_ROOMCHG         0x29        /* room charge func. code   */
#define     CPM_FC2_ACCTCHG         0x2c        /* account charge func code */
#define     CPM_FC2_PRECREDITA      0x2d        /* pre-credit autho. func code */
#define     CPM_FC2_CREDITA         0x2e        /* credit autho. func code  */
#define     CPM_FC2_ACCEPTED        0x30        /* accepted                 */
#define     CPM_FC2_REJECTED        0x31        /* rejected                 */
#define     CPM_FC2_OVERRIDE        0x32        /* override required        */
#define     CPM_FC2_NEEDGLINE       0x33        /* need guest line number   */
#define     CPM_FC2_STOP_CHARG      0x35        /* stop Charge Post/Credit Auth.*/
#define     CPM_FC2_STOP_POST       0x36        /* stop Posting                 */
#define     CPM_FC2_START           0x37        /* start Charge Post/Credit Auth*/
#define     CPM_FC2_PARTTEND        0x39        /* partial tender */
                
#define     CPM_NORMALTYP           '0'         /* normal message type      */
#define     CPM_OVERTYP             '1'         /* override request msg type*/

#define     CPM_CONT_MSG            '0'         /* continue message         */
#define     CPM_LAST_MSG            '1'         /* last message             */


//  Types of error responses from Charge Post Host
//  These should be encoded as a 2 character ANSI string and
//  the resulting value put into the auchCRTRespMsg member of the response.
#define CPM_PROHBT_ADR           0         /* global error     */
#define CPM_PROHBT_ADR0          0         /* global error     */
#define CPM_PROHBT_ADR1          1         /* global error     */
#define CPM_PROHBT_ADR2          2         /* global error     */
#define CPM_CP_NOALLOW_ADR       3         /* charge not allow */
#define CPM_GC_OUT_ADR           4         /* guest checked out */
#define CPM_WRONG_RM_ADR         5         /* wrong room number */
#define CPM_WRONG_GID_ADR        6         /* wrong guest ID */
#define CPM_VOID_NOALLOW_ADR     9         /* void not allowed */
#define CPM_WRONG_ACNO_ADR      10         /* wrong account number */
#define CPM_CP_ADVISE_ADR       11         /* advise front desk */
#define CPM_WRONG_ACTYPE_ADR    12         /* wrong account type */
#define CPM_WRONG_SLD_ADR       13         /* wrong SLD */
#define CPM_FLFUL_ADR           14         /* charge file full */
#define CPM_NTINFL_ADR          17         /* guest number not found */
#define CPM_WRONG_VOUCH_ADR     18         /* wrong voucher number */


/*
*===========================================================================
*   Structure Type Define Declarations
*===========================================================================
*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

//  ***  WARNING - The struct CPMRCREQDATA must remain similar to the struct 
//					CPMACREQDATA.  The function ItemTendCPEdit () uses both
//					of these structs depending on the value of the argument
//					uchFuncCode == CPM_FC2_ROOMCHG or not.
//
//		We want to maintain backwards compatibility with the previous 3rd
//		party vendors of NeighborhoodPOS such as Gateway with the COMM2170
//		charge post application.  Therefore the structs should remain the same
//		as in Rel 1.4
                                               

/**     TALLY DATA      **/  
typedef struct  {
    USHORT  usPMSSendOK;                            /* send success in PMS              */
    USHORT  usPMSSendErr;                           /* send error in PMS                */
    USHORT  usPMSRecResp;                           /* receive response PMS             */
    USHORT  usPMSRecUnsoli;                         /* receive unsoli. PMS              */
    USHORT  usPMSThrowResp;                         /* throw response PMS               */
    USHORT  usTimeout;                              /* timeout in PifReadCom()          */
    USHORT  usIHCSendErr;                           /* send error in IHC                */
}CPMTALLY;

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
/*
*===========================================================================
*   External Work Area Declarations
*===========================================================================
*/
/**     for Tally       **/  
/* extern  CPMTALLY UNINT  CpmTally; */                   /* tally counter        */

/*
===========================================================================
    MEMORY
===========================================================================
*/

/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/
VOID    CpmInit(VOID);
VOID THREADENTRY    CpmMainReceiveTask(VOID);
VOID THREADENTRY    CpmMainSendTask(VOID);

SHORT   CpmSendMessage(UCHAR *pSendData, USHORT usSendLen, UCHAR uchTerminal);
SHORT   CpmReceiveMessage(UCHAR *pRecData, USHORT usRecSize);
SHORT   CpmChangeStatus(UCHAR uchState);
VOID    CpmReadResetTally(UCHAR uchType, CPMTALLY *pTally);
USHORT  CpmConvertError(SHORT sError);

#endif /* CPM_INCLUDED */
/* ---------- End of CPM.H ---------- */
