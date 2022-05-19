/*

		NeighborhoodPOS Software Product Development Project
		Department of Information Systems
		College of Information Technology
		Georgia Southern University
		Statesboro, Georgia

		Copyright Georgia Southern University Foundation
*===========================================================================
* Title       : User Interface for Reg, Header File                         
* Category    : User Interface, NCR 2170 US Hospitality Application         
* Program Name: UIREG.H                                                      
* --------------------------------------------------------------------------
* Abstract:        
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-13-92:00.00.01:M.Suzuki   : initial                                   
* Jun-18-93:01.00.12:K.You      : add UifDiaETKJobNo() for ETK Feature.
* Feb-28-95:03.00.00:hkato      : R3.0
* Nov-09-95:03.01.00:hkato      : R3.1
*
*
*** NCR2172 **
*
* Oct-10-05:01.00.00:M.Teraki   : initial (for Win32)
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

#if !defined(UIREG_H_INCLUDED)

#define UIREG_H_INCLUDED

#include <regstrct.h>

/*
*===========================================================================
*   Define Declarations 
*===========================================================================
*/
/*
*---------------------------------------------------------------------------
*   User Interface for Reg Digit Check Value
*---------------------------------------------------------------------------
*/
#define UIFREG_MAX_DIGIT1       1                   /* Max 1 Digits */
#define UIFREG_MAX_DIGIT2       2                   /* Max 2 Digits */
#define UIFREG_MAX_DIGIT3       3                   /* Max 3 Digits */
#define UIFREG_MAX_DIGIT4       4                   /* Max 4 Digits */
#define UIFREG_MAX_DIGIT5       5                   /* Max 5 Digits */
#define UIFREG_MAX_DIGIT6       6                   /* Max 6 Digits */
#define UIFREG_MAX_DIGIT7       7                   /* Max 7 Digits */
#define UIFREG_MAX_DIGIT8       8                   /* Max 8 Digits */
#define UIFREG_MAX_DIGIT9       9                   /* Max 9 Digits */
#define UIFREG_MAX_DIGIT10      10                  /* Max 10 Digits */
#define UIFREG_MAX_DIGIT13      13                  /* Max 13 Digits, 2172 */
#define UIFREG_MAX_DIGIT14		14					/* Max 14 Digits*/
#define UIFREG_MAX_DIGIT16      16                  /* Max 16 Digits */
#define UIFREG_MAX_DIGIT19      19                  /* Max 19 Digits */
#define UIFREG_MAX_DIGIT25      25                  /* Max 25 Digits, US Customs  3/18/03 cwunn */
#define UIFREG_MAX_DIGIT74		74					// Max 74 Digits, RSS14
#define UIFREG_MAX_DIGIT77		77					// RSS14 Expanded + 3 character prefix

#define UIFREG_NO_ECHO          0                   /* no echo back at dialog, V3.3 */
#define UIFREG_ECHO             1                   /* echo back at dialog, V3.3 */

#define UIFREG_MAX_NUMTYPE  0x100                // indicates that #/Type validation and checking should be performed

#if !defined(UIFREG_MAX_ID_DIGITS)
#define UIFREG_MAX_ID_DIGITS    UIFREG_MAX_DIGIT8      // maximum number of digits for an Operator or Employee ID;  UIF_MAX_ID_DIGITS
#define UIFREG_MAX_PIN_DIGITS   UIFREG_MAX_DIGIT8      // maximum number of digits for an Operator or Employee Personal ID or secret code;  UIF_MAX_PIN_DIGITS
#endif

#if !defined(UIFREG_MAX_AMT_DIGITS)
// Set the maximum number of digits that can be entered for an amount
// such as an open PLU price or a tender amount.
#if defined(DCURRENCY_LONGLONG)
#define UIFREG_MAX_AMT_DIGITS   UIFREG_MAX_DIGIT13
#else
#define UIFREG_MAX_AMT_DIGITS   UIFREG_MAX_DIGIT7
#endif
#endif

/*
*---------------------------------------------------------------------------
*   User Interface Engine Interface
*---------------------------------------------------------------------------
*/
#define UIFREG_MAX_INPUT        25                  /* US Customs Max Input Length, 3/18/03 cwunn */
#define UIFREG_NUMERIC_INPUT     0                  // indicates numeric input required
#define UIFREG_ALFANUM_INPUT     1                  // indicates alphanumeric input required
#define UIFREG_FLAG_MACROPAUSE  0x0100              // flag to indicate macro should pause
/*
*---------------------------------------------------------------------------
*   Return Code from Item module and Dialog return status.
*
*  WARNING:  Some functions that use these as return codes will also
*            return other values from a different name space.  These other
*            namespaces may include codes that conflict or that may have
*            a positive value but still indicate an error.  Some of these
*            other defined codes are:
*                ITM_CONT
*                ITM_CANCEL
*                LDT_PROHBT_ADR
*                LDT_KEYOVER_ADR
*---------------------------------------------------------------------------
*/
#define UIF_SUCCESS         0               /* Success */
#define UIF_DIA_SUCCESS     (-1)               /* dialog success */
#define UIF_DIA_ABORT       (-2)               /* cancel dialog, same as UIFREG_ABORT */
#define UIF_DIA_CLEAR       (-3)               /* clear(skip) dialog, same as UIFREG_CLEAR */
#define UIF_NEW_FSC         (-4)              /* handle UIF_DIA_CHANGE_FSC, UIFREG_NEW_FSC status from dialog */
#define UIF_DIA_RETRY_INPUT     (-10)          /* new FSC from dialog, same as UIFREG_NEW_FSC */
#define UIF_CANCEL         (-10)              /* Cancel */
#define UIF_FINALIZE       (-11)              /* Finalize */
#define UIF_CAS_SIGNOUT    (-12)              /* Cashier Sign Out */
#define UIF_WAI_SIGNOUT    (-13)              /* Waiter Sign Out */
#define UIF_DELIV_RECALL   (-14)              /* Success of Recall at Delivery, R3.0 */
#define UIF_MSR_STATUS     (-15)              /* MSR Status,              R3.1 */
#define UIF_SEAT_TRANS     (-16)              /* Seat Transaction Tender, R3.1 */
#define UIF_SEAT_TYPE2     (-17)              /* Type Tender,             R3.1 */
#define UIF_NUMBER         (-18)              /* scanned label is number, 2172 */
#define UIF_SKUNUMBER      (-19)              /* scanned label is SKU number (merchant's Stock Keeping Unit), 2172. see SKU_MODIFIER and LA_F_SKU_NO */
#define UIF_FS2NORMAL      (-20)              /* Proceed FS to Tender */
#define UIF_FILE_EMPTY     (-21)              /* Nothing to display or file is empty LDT_FILE_EMPTY */
#define UIF_STRING_START   (-22)              /* indicates a control string started.  used with dialogs such as UifDiaGuestNo() */

#define UIF_PINPADSUCCESS	 2				// PinPad was successfull
#define UIF_VIRTUALKEYBOARD  3              // Device is a Virtual Keyboard device, no PinPadd

/*
*---------------------------------------------------------------------------
*   Return Code from Item module
*---------------------------------------------------------------------------
*/
#define UIFREG_ABORT       (-2)               /* cancel dialog, same as UIF_DIA_ABORT */
#define UIFREG_CLEAR       (-3)               /* clear(skip) dialog, same as UIF_DIA_CLEAR */
#define UIFREG_NEW_FSC     (-4)               /* handle UIF_DIA_CHANGE_FSC status from dialog */

/*
*---------------------------------------------------------------------------
*   Function Name
*---------------------------------------------------------------------------
*/
#define CID_REG             1               /* mode in function name */

/*
*===========================================================================
*   Structure Type Define Declarations/
*===========================================================================
*/
/*
*===========================================================================
*   Dialog Interface 
*===========================================================================
*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif


typedef struct {                                /* Dialog output */
	DCURRENCY   lAmount;                            // tender amount or similar signed amount value
	ULONG       ulOptions;                          // options, lower nibble used for PinPad options
    ULONG       ulData;                             /* input Data */
    UCHAR       auchFsc[2];                         /* FSC */
    TCHAR       aszMnemonics[NUM_NUMBER];           /* alpha input data was 19 + 1 changed to coincide with US Customs #/type key change */
    UCHAR       uchTrack1Len;                       /* ISO-2 Length */
    TCHAR       auchTrack1[PIF_LEN_TRACK1];         /* ISO-2 data, saratoga */
    UCHAR       uchTrack2Len;                       /* ISO-2 Length */
    TCHAR       auchTrack2[PIF_LEN_TRACK2];         /* ISO-2 data, saratoga */
    USHORT      usDeptNo;                           /* Dept. Number, 2172     */
    UCHAR       fbStatus;                           /* Status Bits, Saratoga Nov/27/2000 */
    UCHAR       fsInputStatus;                      /* Input Status,Saratoga Nov/27/2000 */
	ULONG    	ulFor;								/* @/For key SR 143 cwunn stores the package size in @/For actions */
	UCHAR    	uchPaymentType;						//used for contactless or swipe types of cards
	USHORT      usTranType;                         // used to indicated tender transaction type (TENDERKEY_TRANTYPE_xxxxx)
 	ULONG       ulStatusFlags;                      // status flags to indicate various conditions from UIFDIAEMPLOYEE
} UIFDIADATA;

#define UIFDIAEMPLOYEE_STATUS_METHOD_MASK   UIFREGMISC_STATUS_METHOD_MASK        // Mask to use to find out the type of Sign-in/Time-in method
#define UIFDIAEMPLOYEE_STATUS_FIRST			UIFREGMISC_STATUS_FIRST		// indicates this is the first employee
#define UIFDIAEMPLOYEE_STATUS_SUP_PIN_REQ	0x00000020					// indicates that supervisor mode requires a PIN
#define UIFDIAEMPLOYEE_STATUS_ETK_PIN_REQ	0x00000040					// indicates that Time-in action requires a PIN
#define UIFDIAEMPLOYEE_STATUS_REG_PIN_REQ	0x00000080					// indicates that register mode requires a PIN
#define UIFDIAEMPLOYEE_STATUS_REG_DENY_A	0x00000100					// indicates that cash drawer A is denied for this operator
#define UIFDIAEMPLOYEE_STATUS_REG_DENY_B	0x00000200					// indicates that cash drawer B is denied for this operator
#define UIFDIAEMPLOYEE_STATUS_ETK_PIN_MIN   0x00003000                  // indicates minimum digits required 0, no min, 1, 2, 3 -> 6, 7, 8 digits
#define UIFDIAEMPLOYEE_STATUS_ETK_MIN_SHFT          12                  // indicates number bits to shift to get min value
#define UIFDIAEMPLOYEE_STATUS_ETK_MIN_6     0x00001000                  // indicates minimum digits for PIN entry is 6, see UifEmployePinNumherForSignTime()
#define UIFDIAEMPLOYEE_STATUS_ETK_MIN_7     0x00002000                  // indicates minimum digits for PIN entry is 7, see UifEmployePinNumherForSignTime()
#define UIFDIAEMPLOYEE_STATUS_ETK_MIN_8     0x00003000                  // indicates minimum digits for PIN entry is 8, see UifEmployePinNumherForSignTime()
// see also affect of MDC address 479, Bit A (MDC_EMPLOYEE_ID_ADR, MDC_PARAM_BIT_A) on Time-in behavior.

typedef struct {
	ULONG   ulSwipeEmployeeId;
	ULONG   ulSwipeEmployeePin;
	ULONG   ulSwipeSupervisorNumber;
	ULONG   ulStatusFlags;          // status flags to indicate various conditions
	USHORT  usSwipeEmployeeJobCode;
	USHORT  usSwipeOperation;
	USHORT  usSwipeOperationSecondary;
} UIFDIAEMPLOYEE;

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

extern UCHAR	   uchIsNoCheckOn;					  /* Is No Check functionality on SR 18*/
extern SHORT		  fsPrtCompul;
extern SHORT	   fsPrtNoPrtMask;
extern SHORT       UifSupInterventRetStatus;

/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/
SHORT   UifReg(KEYMSG *pData);                          /* uifreg.c */
SHORT   UifRegString(KEYMSG *pData);                    /* R3.0 */
                                                    
SHORT   UifDiaGetNumericEntry(UIFDIADATA *pData, USHORT usEcho, USHORT usNoDigits, UIMENU *aUieMenu);
SHORT   UifDiaCashier(UIFDIADATA *pData, USHORT usEcho);/* uidcas.c */
SHORT   UifDiaWaiter(UIFDIADATA *pData);                /* uidwai.c */
SHORT   UifDiaGuestNo(UIFDIADATA *pData);               /* uidgcn.c */
SHORT   UifDiaTableNo(UIFDIADATA *pData);               /* uidtbl.c */
SHORT   UifDiaLineNo(UIFDIADATA *pData);                /* uidlin.c */
SHORT   UifDiaNoPerson(UIFDIADATA *pData);              /* uidper.c */
VOID    UifDiaWaiterLock(VOID);                         /* uidwailk.c */
SHORT   UifSupIntervent(VOID);                          /* uidspint.c */
SHORT   UifCashPickup(KEYMSG *Data);                    /* uidspint.c */
SHORT   UifDiaETK(UIFDIADATA *pData, USHORT usEcho);    /* uidetk.c */
SHORT   UifDiaCP1(UIFDIADATA *pData);                   /* uidcp.c */
SHORT   UifDiaCP2(UIFDIADATA *pData);                   /* uidcp.c */
SHORT   UifDiaCP3(UIFDIADATA *pData, UCHAR uchMaxLen);  /* uidcp.c */
SHORT   UifDiaCP4(UIFDIADATA *pData, UCHAR uchMaxLen);  /* uidcp.c */
SHORT   UifDiaCP14(UIFDIADATA *pData, UCHAR uchMaxLen, UCHAR uchEchoType, UCHAR uchMsrEnable);  // same as UifDiaCP4() with added argument for echo type such as UIE_ECHO_NO
SHORT   UifDiaCP5(UIFDIADATA *pData);                   /* uidcp.c, Saratoga */
SHORT	UifDiaCP6(UIFDIADATA *pData, UCHAR uchMaxLen, USHORT usInputType);	// uidcp.c
SHORT   UifDiaOEP(UIFDIADATA *pData, VOID *pDispData);  /* uidoep.c  R3.0 */
SHORT   UifDiaAlphaNameNoEcho (UIFDIADATA *pData, UCHAR uchEchoType);  // no echo version of UifDiaAlphaName() for PIN entry
SHORT   UifDiaAlphaName(UIFDIADATA *pData);             /* R3.0 */
SHORT   UifDiaSeatNo(UIFDIADATA *pData);                /* uidseat.c */
SHORT   UifDiaTransfer(UIFDIADATA *pData);              /* uidtsf.c, R3.1 */
SHORT   UifDiaScroll(UIFDIADATA *pData);                /* uidscrl.c, R3.1 */
SHORT   UifDiaInsertLock(UIFDIADATA *pData);            /* uidwaiin.c, V3.3 */
VOID    UifDiaRemoveLock(VOID);                         /* uidwairv.c, V3.3 */
SHORT   UifDiaOEPKey(UIFDIADATA *pUifDiaData);          /* uidoek.c  2172 */
SHORT   UifDiaPrice(UIFDIADATA *pData);                 /* uidprc.c 2172 */
SHORT   UifDiaDept(UIFDIADATA *pData);
SHORT   UifDiaTare(UIFDIADATA *pData);                 /* uidprc.c 2172 */
SHORT   UifDiaClear(VOID);                              /* uidclr.c 2172 */
SHORT   UifDiaWICMod(VOID);                             /* uifdilog.c  */
//SR 143 @/For Key cwunn ported the following from 2170GP
SHORT UifRegWQtyCheck(KEYMSG *pData, UCHAR uchLen, UCHAR uchDec); /* uifrgcom.c */
SHORT UifRegQtyCheck(KEYMSG *pData, UCHAR uchLen, UCHAR uchDec); /* uifrgcom.c */
//End SR 143

SHORT CheckSuperNumber( KEYMSG *pData, UIFDIAEMPLOYEE  *pUifDiaEmployee);

VOID	UifRedisplaySupInt(VOID);

SHORT  UifTrackToUifDiaEmployee (TCHAR *auchTrack2, USHORT usLength2, UIFDIAEMPLOYEE  *pUifDiaEmployee);
SHORT  UifCheckMsrEmployeeTrackData(TCHAR *auchTrack2, USHORT usLength2);
SHORT  UifEmployeeSignTimeMethodString (ULONG ulStatusFlags, TCHAR * aszNumber);
SHORT  UifEmployeNumherForSignTime (UIFDIAEMPLOYEE  *pUifDiaEmployee);
SHORT  UifRegEtkGetCheckEmployeeData (ULONG ulEmployeeId, UIFDIAEMPLOYEE  *pUifEmployeeData, SHORT *psTimeInStatus);

VOID	UifRegCheckDrawerLimit(USHORT status);							/*JHHJ Cash Drawer Limit SR*/
VOID	UifRegClearDisplay(USHORT usKeyTypeFsc);

SHORT	UifRegCloseDrawerWarning(int iDelayOverride);

VOID	UieEmptyRingBufClearMacro(VOID);

#endif

/****** end of definition ******/
