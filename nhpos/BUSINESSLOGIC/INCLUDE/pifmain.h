/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Initialize Module, Header File
* Category    : User Interface Mian, NCR 2170 US Hospitality Application         
* Program Name: UIMAIN.H
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-05-92:00.00.01:M.Suzuki   : initial                                   
*          :        :           :                                    
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

#if !defined(PIFMAIN_H_INCLUDED)

#define PIFMAIN_H_INCLUDED 1

#include    "uie.h"

/*
*===========================================================================
*   Define Declarations 
*===========================================================================
*/
#define UIF_STACK_APPL          7168*2      /* Stack Size for Main Application, Saratoga */

#define UIF_MAX_MODE            6           /* Mode switch position */
/* #define UIF_DEC_POS             2           / decimal position */
#define UIF_DEC_POS2            2           /* .00 decimal position (V3.3) */
#define UIF_DEC_POS3            3           /* .000 decimal position (V3.3) */
#define UIF_DEC_POSNO           255         /* no decimal position (V3.3) */

#define CID_LOCK                1           /* lock main routine function name */

// following defines are used with usOptions member of a UIFDIADATA struct.
// when using the FRAMEWORK_IOCTL_PINPAD_READ functionality through BlFwIfReadPinPad()
// These are bit masks and some such as UIF_PINPAD_OPTION_READ_STRIPEPIN combine multiple bits.
#define UIF_PINPAD_OPTION_READ_PIN         0x0001    // used with UifRequestPinPadDeviceRead() indicating need PIN entry (e.g. Debit or EBT card)
#define UIF_PINPAD_OPTION_READ_STRIPE      0x0002    // used with UifRequestPinPadDeviceRead() indicating need card swipe
#define UIF_PINPAD_OPTION_READ_STRIPEPIN   0x0003    // used with UifRequestPinPadDeviceRead() indicating need card swipe
#define UIF_PINPAD_OPTION_READ_CONTACTLESS 0x0004    // used to indicate contactless EPT such as FreedomPay or VivoTech
#define UIF_PINPAD_OPTION_READ_SIGNATURE   0x0008    // used to indicate signature capture such as Ingenico i6550
#define UIF_PINPAD_OPTION_CLEAR_RESET      0x0080    // used to indicate that current request should be cleared
#define UIF_PAUSE_OPTION_ALLOW_EMPTY       0x0100    // indicates that UifDiaCP14() and other should allow no data entered
#define UIF_PAUSE_OPTION_ALLOW_SCAN        0x0200    // indicates that UifDiaOEPEntry() and other should allow scanned data
#define UIF_RULES_NO_CHECK_NUMBER          0x0400    // indicates that UifDiaOEPEntry() and other should not check the data entered


extern  UCHAR   uchResetLog;                /* V3.3 */
extern  UCHAR   uchPowerDownLog;            /* Saratoga */

/*
*===========================================================================
*   Structure Type Define Declarations
*===========================================================================
*/

/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/
VOID   UifCreateInitialize(VOID);               /* uimain.c */
VOID   UifInitialize(VOID);                     /* uimain.c */
VOID   THREADENTRY UifCreateMain(VOID);         /* uimain.c */
VOID   THREADENTRY UifMain(VOID);               /* uimain.c */
VOID   THREADENTRY UifPowerMonitor(VOID); 
VOID   THREADENTRY UifAbortMonitor(VOID); 
VOID   UifChangeMode(UCHAR uchMode);            /* uimain.c */
SHORT  UifLock(KEYMSG *pData);                  /* uimain.c */
VOID   UifCPPrintUns(VOID); /* V3.3 */
VOID   UifCPRetryUns(VOID); /* V3.3 */
VOID   THREADENTRY SerStartUp(VOID);
VOID   THREADENTRY HttpStartUp(VOID);
VOID   THREADENTRY KpsPrintProcess(VOID);
VOID   THREADENTRY KpsTimerProcess(VOID);
VOID   THREADENTRY FDTMain(VOID);
VOID   THREADENTRY IspMain(VOID);          /* Starts ISP Task          */
SHORT  UifReadAmtrakEmployeeFile (ULONG  ulEmployeeId, TCHAR *aszMnemonic);
VOID   UifEnableVirtualKeyDeviceRead (VOID);
VOID   UifDisableVirtualKeyDeviceRead (VOID);

UCHAR *UifSetUniqueIdWithTenderId (UCHAR *puchUniqueIdentifier, USHORT usTenderId);
USHORT UifClearUniqueIdReturningTenderId (UCHAR *puchUniqueIdentifier);
UCHAR *UifConvertStringReturningUniqueId (TCHAR *ptcsUniqueIdentifer, UCHAR *puchUniqueIdentifier, USHORT usTenderId);
TCHAR *UifConvertUniqueIdReturningString (TCHAR *ptcsUniqueIdentifer, UCHAR *puchUniqueIdentifier);
UCHAR *UifConvertStringReturningUniqueIdRaw (TCHAR *ptcsUniqueIdentifer, UCHAR *puchUniqueIdentifier);
TCHAR *UifConvertUniqueIdReturningStringRaw (TCHAR *ptcsUniqueIdentifer, UCHAR *puchUniqueIdentifier);
UCHAR *UifSetUniqueIdWithTenderId (UCHAR *puchUniqueIdentifier, USHORT usTenderId);
UCHAR *UifCopyUniqueIdSettingTenderId (UCHAR *puchDestUniqueIdentifier, UCHAR *puchSourceUniqueIdentifier, USHORT usTenderId);
int    UifCompareUniqueIdWithoutTenderId (UCHAR *puchUniqueIdentifierLhs, UCHAR *puchUniqueIdentifierRhs);

BOOL  UifDeviceMessageSystemSendWait (ULONG ulIoctlId, VOID *pvDataArea, ULONG ulDataAreaSize);
VOID  UifRequestSharedPrintFile (TCHAR *aszFileName);

#endif