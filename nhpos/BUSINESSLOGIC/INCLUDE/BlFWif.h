/*************************************************************************
 *
 * BlFWif.h
 *
 * $Workfile:: BlFWif.h                                                  $
 *
 * $Revision::                                                           $
 *
 * $Archive::                                                            $
 *
 * $Date::                                                               $
 *
 *************************************************************************
 *
 * Copyright (c) NCR Corporation 2000.  All rights reserved.
 *
 *    Georgia Southern University Research Services Foundation
 *    donated by NCR to the research foundation in 2002 and maintained here
 *    since.
 *
 *    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
 *************************************************************************
 *
 * $History:: BlFWif.h                                                   $
 *
 ************************************************************************/

#if !defined(_INC_BLFWIF)
#define _INC_BLFWIF

// The following #define is used when testing in a release build witin
// the lab.  Under no conditions should a build with this turned on
// allowed to go outside of the lab not even to NCR.
//#define RELEASE_DISABLE_SECURITY 1

#if defined (RELEASE_DISABLE_SECURITY)
#pragma message("  **************************************************************************************");
#pragma message("  *******   RELEASE_DISABLE_SECURITY is enabled.  Should it be?  Are you sure?   *******")
#pragma message("  **************************************************************************************")
#endif


#ifdef __cplusplus
extern "C"{
#endif

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif


/*
/////////////////////////////////////////////////////////////////////////////
//  defines
/////////////////////////////////////////////////////////////////////////////
*/
// SECURITY CONTROL DEFINITIONS
#define UNREGISTERED_COPY	-177    // This must be same value as CAS_NOT_REGISTERED in cscas.h
#define REGISTERED_COPY     -178
#define DEMO_MODE			-179
#define LICENSE_MISSING     -180
// This is the definition that toggles
// NHPOS b/t Softlocx and VBOLOCK
// -- TLDJR
#define VBOLOCK

//The security number is made up of 3 unsigned longs
//the SECGUARD defines are the guard bits of the security number
//the SECREG defines are the Security bits
//the OPTIONS defines will be used for which options will be available
//the COUNTERMASK is used on the third ULONG
//the bits in the COUNTERMASK area
//inorder to cause different bits to be turned on or off security number
//
//A valid Security number is one that has all of the GUARD BITs turned on
//At least two of three SECREG must match up with the security number
//in order for the application to recognize it as Registered or during Eval period
// PLEASE NOTE: the encryption key used is: V20GASOU6tup9lX3bGiQ
// and the License Name is : NHPOS Release 2.0.0

#define SECGUARD1	0xA4206002//10100100001000000110000000000010
#define SECREG1		0x00020010//00000000000000100000000000010000
#define OPTIONS1	0x00000000//00000000000000000000000000000000

#define SECGUARD2	0x04010200//00000100000000010000001000000000
#define SECREG2		0x800041A0//10000000000000000100000110100000
#define OPTIONS2	0x00000000//00000000000000000000000000000000

#define SECGUARD3	0x90021425//10010000000000100001010000100101
#define SECREG3		0x00210000//00000000001000010000000000000000
#define OPTIONS3	0x00000000//00000000000000000000000000000000

#define COUNTERMASK	0x0F000000//00001111000000000000000000000000

#ifdef NDEBUG
#if defined(RELEASE_DISABLE_SOFTLOCX)
#pragma  message("     ****   WARNING:   RELEASE_DISABLE_SECURITY  is ENABLED     WARNING   ****");
#define CHECKGUARDBITS(x, y)
#define CHECKSECBITS(x)
#else
#define CHECKGUARDBITS(x, y) if( ((*x & *y) ^ *x) || ((*(x+1) & *(y+1)) ^ *(x+1))\
						|| ((*(x+2) & *(y+2)) ^ *(x+2))) { NHPOS_ASSERT(!"CHECKGUARDBITS");return UNREGISTERED_COPY; }

//check to see which are set
//if any combination of two are BOTH NOT SET then not registered or in demo mode
#define CHECKSECBITS(x) if( (((SECREG1 & *x) ^ SECREG1) && ((SECREG2 & *(x+1)) ^ SECREG2))\
						|| (((SECREG1 & *x) ^ SECREG1) && ((SECREG3 & *(x+2)) ^ SECREG3))\
						|| (((SECREG2 & *(x+1)) ^ SECREG2) && ((SECREG3 & *(x+2)) ^ SECREG3))) { NHPOS_ASSERT(!"CHECKSECBITS");return UNREGISTERED_COPY; }
#endif
#else
#define CHECKGUARDBITS(x, y)
#define CHECKSECBITS(x)
#endif


#define REGSET(x, a, b, c) (*x |= a); (*(x+1) |= b); (*(x+2) |= c);



/*
/////////////////////////////////////////////////////////////////////////////
//  Function Interface
/////////////////////////////////////////////////////////////////////////////
*/

/* for BlFifDeviceControl and BlFifDeviceVersion*/

#define BLFWIF_KEYLOCK              1
#define BLFWIF_DRAWER               2
#define BLFWIF_MSR                  3
#define BLFWIF_SCANNER              4
#define BLFWIF_SCALE                5
#define BLFWIF_SERVERLOCK           6
#define BLFWIF_PRINTER				7
#define BLFWIF_CDISPLAY				8
#define BLFWIF_COIN					9
//PLEASE NOTE: if you add any new OPOS items, please 
//update the number of controls below. JHHJ

#define BLFWIF_NUM_OPOS_CONTROLS	9 

// used for mode argument of function BlFwIfSetAutoSignOutTime (ULONG time, SHORT mode)
#define	BLFWIF_AUTO_MODE_NO_CHANGE      3      // used for the auto sign out and does not specify A or B key
/* for ddd */

typedef struct _BLFWIF {
    UCHAR   uchUnit;                        /* unit           */
    UCHAR   uchDec;                         /* precision      */
    ULONG   ulData;                         /* data           */
} BLFWIF, *PBLFWIF;

typedef struct _BLFMSRIF {
    UCHAR   uchLength1;                     /* ISO track-1 length      */
    TCHAR   auchTrack1[PIF_LEN_TRACK1];     /* ISO track-1 data        */
    UCHAR   uchLength2;                     /* ISO track-2 length      */
    TCHAR   auchTrack2[PIF_LEN_TRACK2];     /* ISO track-2 data        */
    UCHAR   uchLengthJ;                     /* JIS track-II length     */
    TCHAR   auchTrackJ[PIF_LEN_TRACKJ];     /* JIS track-II data       */
} BLFMSRIF, *PBLFMSRIF;

typedef struct _BLFPINPADIF {
    USHORT      usLength;
    TCHAR       auchEncryptedPin[64];
    USHORT      usLengthKsn;
    TCHAR       auchKeySerialNumber[64];
	UCHAR       uchLength1;
	TCHAR       auchTrack1[PIF_LEN_TRACK1];
	UCHAR       uchLength2;
	TCHAR       auchTrack2[PIF_LEN_TRACK2];
	UCHAR	    uchPaymentType;
	LONG        lAmount;                        // amount to display as part of PIN entry
	USHORT      usOptions;                      // options for PINPad device I/O UIF_PINPAD_OPTION_READ_PIN and others
	SHORT       sPinPadRet;                     // contains EEPT_RET_xxx status from XEptDllCollectStripeAndPin(), XEptDllCollectPin(), etc.
} BLFPINPADIF, *PBLFPINPADIF;

typedef struct _BLFSIGCAPIF
{
	DCURRENCY  lAmount;                        // amount of the purchase; to be displayed
	USHORT     usOptions;                      // options for the I/O request; BLFSIGCAPIF struct
	LONG       m_SigCapMaxX;                   // max X for signature; BLFSIGCAPIF struct
	LONG       m_SigCapMaxY;                   // max X for signature; BLFSIGCAPIF struct
	TCHAR      *m_SigCapPointArray;             // pointer to buffer holding signature; BLFSIGCAPIF struct
	USHORT     m_ArrayLength;                  // length of the array currently in m_SigCapPointArray; BLFSIGCAPIF struct
	USHORT     m_ArrayMaxSize;                 // max buffer size containing signature; BLFSIGCAPIF struct
} BLFSIGCAPIF, *PBLFSIGCAPIF;

typedef struct {
	LONG	lPrintStation;
	ULONG   m_ulPrinter1StatusMap;  // used with DevicePrinterStatus() to contain m_pOPOSControl->m_ulPrinter1StatusMap
	ULONG   dwErrorCode;            // error code, if any, reported by CDeviceEngine::DevicePrinterWrite()
	LONG    resultCode;
	LONG    extResultCode;
	USHORT	fbStatus;		//JHALL Status of Printer
} BLFDEVERROR, *PBLFDEVERROR;

static ULONG ulCheck[3];
static ULONG ulGuard[3] = {SECGUARD1, SECGUARD2, SECGUARD3};

/* for BlFifDeviceControl */

#define BLFWIF_DRAWER_CLOSED        (4) /* 0 */
#define BLFWIF_DRAWER_OPENING       (2) /* 3 */
#define BLFWIF_DRAWER_OPENED        (1) /* 2 */

/* Print log file definitions */ //ESMITH PRTFILE
#define BL_PRTFILE_PROG		TEXT("\\FlashDisk\\NCR\\Saratoga\\Log\\ProgPrtLog.log")
#define BL_PRTFILE_SUPER	TEXT("\\FlashDisk\\NCR\\Saratoga\\Log\\SuperPrtLog.log")
#define BL_PRTFILE_REG		TEXT("\\FlashDisk\\NCR\\Saratoga\\Log\\RegPrtLog.log")
#define BL_PRTFILE_WEB		TEXT("\\FlashDisk\\NCR\\Saratoga\\Web\\")


// Special defines used with function BlFwIfPopupWindowEventTrigger()
// These are the same as are defined in the TouchLibrary in WindowControl.h

#define SpecWinEventTrigger_WId_OrderDec  1    //Window ID: CAS_ORDER_DEC_FORCE, CURQUAL_ORDERDEC_DECLARED


/*
/////////////////////////////////////////////////////////////////////////////
//  Function Prototypes
/////////////////////////////////////////////////////////////////////////////
*/

BOOL    BlFwIfDeviceControl(USHORT usDevice, BOOL fState);
BOOL	BlFwIfDeviceVersion(USHORT usDevice, TCHAR* pVersion); //7-6-05 JHHJ for version of OPOS
USHORT  BlFwIfDrawerOpen(USHORT usNumber);
USHORT  BlFwIfDrawerStatus(ULONG *ulDrawerStatus);
BOOL    BlFwIfScannerNOF(USHORT usControl);
SHORT   BlFwIfScaleReadWeight(PBLFWIF pWeight);
SHORT   BlFwIfReadMSR (PBLFMSRIF pMsrTrackData);
SHORT	BlFwIfReadSignatureCapture (PBLFSIGCAPIF pSigCapData);
SHORT	BlFwIfReadSignatureCustomerDisplay (TCHAR *aszFormName);
SHORT	BlFwIfReadSignatureClear (VOID);
SHORT	BlFwIfReadSignatureLineDisplay (VOID);
SHORT	BlFwIfReadSignatureCaptureRead (PBLFSIGCAPIF pSigCapData, USHORT usFscMajor, USHORT usFscMinor);
SHORT	BlFwIfClearPinPad (VOID);
SHORT   BlFwIfReadPinPad (USHORT usOptions, PBLFPINPADIF pPinPadTrackData);
VOID	BlFwIfWriteToLineDisplay(USHORT usDispId, USHORT usRow, USHORT usColumn,
							  TCHAR *puchString, USHORT usChars);
VOID	BlFwIfGetLineDisplayInformation(TCHAR *puchServiceObject, LONG *lColumns, LONG *lRows);

SHORT   BlFwIfDispenseCoin(DCURRENCY lDispenseAmount);
ULONG	BlFwIfPrint(LONG lPrtType, TCHAR *pucBuff, USHORT usLen, BLFDEVERROR *pArgError);
ULONG	BlFwIfInitLoadLogo(VOID);
VOID	BlFwGetPrinterColumnsStatus(LONG lPrtType, USHORT* pusColumn, USHORT* pfbStatus);
USHORT	BlFwGetPrinterStatus(LONG lPrtType, USHORT* pfbStatus);
VOID	BlFwBeginInsertion(LONG lTimeout);
VOID	BlFwEndInsertion();
VOID	BlFwBeginRemoval(LONG lTimeout);
VOID	BlFwEndRemoval();
VOID	BlFwIfPrintSetFont(LONG lPrtType , USHORT usFont); //ESMITH EJ FONT
LONG	BlFwIfPrintGetSetPrtCap(LONG lPrtType, USHORT usPrtCap, LONG lValue); //ESMITH EJ FONT
ULONG	BlFwIfPrintPrintBarCode(LONG lPrtType, TCHAR *pucBuff, ULONG ulTextFlags, ULONG ulCodeFlags, BLFDEVERROR *pArgError);
// Security Functions
int 	BlFwIfLicStatus();
VOID	BlFwIfReadSerial(TCHAR* szSerial);
VOID	BlFwIfLicReg(TCHAR* szUlCode, TCHAR* szStatus);
VOID	BlFwIfReadExpStr(TCHAR* szExpireVal);
LONG	BlFwIfReadExpProp(VOID);
VOID	BlFwIfSecStatus(ULONG* plStatus);
VOID	BlFwIfSecStatusProp(ULONG* plStatus);
// end Security functions
BOOL	BlFwIfReloadLayout2();
BOOL    BlFwIfReloadOEPGroup ();
VOID	BlFwIfNagScreen();
VOID	BlFwIfPrintFileWrite(TCHAR *szData, USHORT usLen, TCHAR *szFileName); //ESMITH PRTFILE
VOID	BlFwIfDefaultUserScreen();
TCHAR	BlFwIfGetPrintType(VOID);
VOID	BlFwIfGroupWindowShow(ULONG ulMaskGroupPermission);
VOID	BlFwIfGroupWindowHide(ULONG ulMaskGroupPermission);
VOID	BlFwIfWindowTranslate(ULONG ulBoolTranslate);
LONG	BlFwIfDisplayOEPImagery (UCHAR uchCommand, UCHAR uchDeptNo, UCHAR uchTableNo, UCHAR uchGroupNo);


USHORT  BlFwIfPopupWindowEventTrigger (int SpecWinEventTrigger_WId);
USHORT  BlFwIfPopupWindowName (TCHAR *ptcWindowName);
USHORT BlFwIfPopupWindowPrefix (TCHAR *WindowPrefix);
USHORT BlFwIfPopupWindowGroup (TCHAR *WindowPrefix);
USHORT BlFwIfPopupMessageWindow (TCHAR *tcsMessage);
USHORT  BlFwIfPopdownWindowName (TCHAR *ptcWindowName);
USHORT BlFwIfPopdownWindowPrefix (TCHAR *WindowPrefix);
USHORT BlFwIfPopdownWindowGroup (TCHAR *WindowPrefix);
USHORT BlFwIfPopdownMessageWindow (VOID);
USHORT BlFwIfPopdownMessageAllVirtual (VOID);


//windows printer drivers
VOID	BlFwIfPrintWin(TCHAR *pucBuff, USHORT usLen);
VOID	BlFwIfPrintWinStart(VOID);
VOID	BlFwIfPrintWinEnd(VOID);

VOID	BlFwIfPrintTransStart(LONG lPrtType, ULONG ulTransNo);
VOID	BlFwIfPrintTransEnd(LONG lPrtType, ULONG ulTransNo);
BOOL	BlFwIfBlInterface (ULONG ulIoctlId, VOID *pvDataArea, ULONG ulDataAreaSize);
VOID    BlFwIfSetAutoSignOutTime(ULONG time, SHORT mode);
VOID    BlFwIfForceShutdown(VOID);

/*
/////////////////////////////////////////////////////////////////////////////
*/

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

#ifdef __cplusplus
}
#endif

#endif /* !defined(_INC_BLFWIF) */
