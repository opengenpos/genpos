/*
*===========================================================================
*
*   Copyright (c) 1992-1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Global Header File
* Category    : User Interface Enigne, 2170 System Application
* Program Name: UIE.H
* --------------------------------------------------------------------------
* Abstract:     This header file defines User Interface Engine the 2170
*               system application.
*
*               This file contains:
*                   1. Symbol Definitions
*                   2. Data type Definitions
*                   3. Function Prototypes
*
* --------------------------------------------------------------------------
* Update Histories
*
* Date       Ver.      Name         Description
* Mar-20-92  00.00.01  O.Nakada     Initial
* Nov-26-92  01.00.01  O.Nakada     Add Scale I/F function
* May-20-93  B3        O.Nakada     Add Coin Despensor function
* May-25-93  C1        O.Nakada     Supports NCR 7852, 7870 and 7890.
* May-27-93  C1        O.Nakada     Modify UieCtrlDevice() prototype.
* Jun-05-93  C1        O.Nakada     Add UieNotonFile().
* Sep-16-93  F1        O.Nakada     Divided the User Interface Engine.
* Mar-13-95  G0        O.Nakada     Added release 3.0 functions.
* Oct-31-95  G3        O.Nakada     Added condition type of UieErrorMsg().
* Feb-14-96  G4        O.Nakada     Added PERIPHERAL_DEVICE_OTHER.
*                                   Moved UieWriteRingBuf().
* Feb-19-97  G9        M.Suzuki     Added UieCoinSendData2().
*
*** NCR2171 ***
* Sep-29-99  01.00.00  M.Teraki     Initial
*                                   Added UiePutXxxxData()
* Oct-05-99            M.Teraki     Added #pragma pack(...)
*** NHPOS 1.4.x, NCR 7448  ***
* Aug-01-02  NHPOS 1.4 R.Chambers   Product donated to Georgia Southern University
*** NHPOS 2.0.x Touchscreen ***
* Mar-01-04  NHPOS 2.0 R.Chambers   Conversion from NCR 7448 to Touchscreen terminals
*** GenPOS 2.2.x Touchscreen ***
* Nov-19-14  GenPOS 2.2 R.Chambers  Source cleanup to remove old, before NCR 7448,
*                                   code that applied to NCR 2170 back in the 1990s.
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
#ifndef _INC_UIE
#define _INC_UIE

#ifndef _INC_PIF
#include "pif.h"
#endif

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif
/*
*===========================================================================
*   Define Declarations
*===========================================================================
*/
#define UIE_SUCCESS         1               /* successful         */
#define UIE_ERROR           (-1)            /* error              */
#define UIE_ERROR_NEXT      (-2)            /* get next character */
#define UIE_ERROR_QTY_GUARD			(-4)   // indicates FSC_QTY_GUARD or FSC_QTY_GUARD_SCAN or FSC_QTY
#define UIE_ERROR_PRICE_GUARD		(-5)   // indicates FSC_PRICE_GUARD or FSC_PRICE_GUARD_SCAN or FSC_PRICE_ENTER
#define UIE_ERROR_CNTRL_STING_GRD	(-6)   // indicates FSC_CNTRL_STING_GRD or FSC_CNT_STRING_GUARD_SCAN

#define UIE_DISABLE         0               /* disable            */
#define UIE_ENABLE          1               /* enable             */

#define DLLENTRY                            /* dummy define       */
#define DLLARG                              /* dummy define       */



// Following are standard defines to be used with all of the
// devices that are use through the UIE interface.
#define UIE_DEVICE_ERROR             (-11)        /* error when accessing the device */
#define UIE_DEVICE_NOT_PROVIDE       (-12)        /* device is not provided */
#define UIE_DEVICE_ERROR_RANGE       (-13)        /* range error         */
#define UIE_DEVICE_ERROR_COM         (-14)        /* communication error */
#define UIE_DEVICE_TIMEOUT           (-15)        /* communication error */

#define UIE_DEVICE_SPECIFIC          (-20)        /* device specific errors start here */

/*
*===========================================================================
*   Position of Mode Lock
*===========================================================================
*/
#define UIE_POSITION0       0               /* P-2 mode        */
#define UIE_POSITION1       1               /* power off mode  */
#define UIE_POSITION2       2               /* lock mode       */
#define UIE_POSITION3       3               /* register mode   */
#define UIE_POSITION4       4               /* supervisor mode */
#define UIE_POSITION5       5               /* program mode    */


/*
*===========================================================================
*   Abort Key Control
*===========================================================================
*/
#define UIE_POS0            0x01            /* P-2 mode        */
#define UIE_POS1            0x02            /* power off mode  */
#define UIE_POS2            0x04            /* lock mode       */
#define UIE_POS3            0x08            /* register mode   */
#define UIE_POS4            0x10            /* supervisor mode */
#define UIE_POS5            0x20            /* program mode    */


/*
*===========================================================================
*   Input Device Control
*===========================================================================
*/
#define UIE_ENA_DRAWER      0x0001          /* enable drawer message       */
#define UIE_DIS_DRAWER      0x0002          /* disable drawer message      */
#define UIE_ENA_WAITER      0x0004          /* enable waiter lock message  */
#define UIE_DIS_WAITER      0x0008          /* disable waiter lock message */
#define UIE_ENA_SCANNER     0x0010          /* enable scanner message      */
#define UIE_DIS_SCANNER     0x0020          /* disable scanner message     */
#define UIE_ENA_MSR         0x0040          /* enable MSR message          */
#define UIE_DIS_MSR         0x0080          /* disable MSR message         */
#define UIE_ENA_KEYBOARD    0x0100          /* enable keyboard message       */
#define UIE_DIS_KEYBOARD    0x0200          /* disable keyboard message      */
#define UIE_ENA_KEYLOCK     0x0400          /* enable key lock message       */
#define UIE_DIS_KEYLOCK     0x0800          /* disable key lock message      */
#define UIE_ENA_VIRTUALKEY  0x1000          // enable virtual keyboard message, BL_DEVICE_DATA_VIRTUALKEY, VIRTUALKEY_BUFFER
#define UIE_DIS_VIRTUALKEY  0x2000          // disable virtual keyboard message, BL_DEVICE_DATA_VIRTUALKEY, VIRTUALKEY_BUFFER


/*
*===========================================================================
*   Type of Echo Back
*===========================================================================
*/
#define UIE_ECHO_NO             1           /* no echo back              */
#define UIE_ECHO_ROW0_REG       2           /* row 0, register           */
#define UIE_ECHO_ROW1_REG       3           /* row 1, register           */
#define UIE_ECHO_ROW0_AN        4           /* row 0, alpha/numeric      */
#define UIE_ECHO_ROW1_AN        5           /* row 1, alpha/numeric      */
#define UIE_ECHO_ROW0_NUMBER    6           /* row 0, number entry       */
#define UIE_ECHO_ROW1_NUMBER    7           /* row 1, number entry       */
#define UIE_ECHO_ROW0_PRESET    8           /* row 0, preset cash        */
#define UIE_ECHO_ROW1_PRESET    9           /* row 1, preset cash        */
#define UIE_ECHO_DATE_ENTRY		10			/* row 0, date entry.		 */

#define UIE_ECHO_WEIGHT         30          /* weight for register	     */
#define UIE_ECHO_CA             31          /* code/account for register */


/*
*===========================================================================
*   Descriptor of Echo Back Ground
*===========================================================================
*/
#define UIE_KEEP            0               /* keep current descriptor  */

#define UIE_DESC15_OFF      0x40000000L     /* descriptor No.15 - off   */
#define UIE_DESC14_OFF      0x10000000L     /* descriptor No.14 - off   */
#define UIE_DESC13_OFF      0x04000000L     /* descriptor No.13 - off   */
#define UIE_DESC12_OFF      0x01000000L     /* descriptor No.12 - off   */
#define UIE_DESC11_OFF      0x00400000L     /* descriptor No.11 - off   */
#define UIE_DESC10_OFF      0x00100000L     /* descriptor No.10 - off   */
#define UIE_DESC09_OFF      0x00040000L     /* descriptor No.9  - off   */
#define UIE_DESC08_OFF      0x00010000L     /* descriptor No.8  - off   */
#define UIE_DESC07_OFF      0x00004000L     /* descriptor No.7  - off   */
#define UIE_DESC06_OFF      0x00001000L     /* descriptor No.6  - off   */
#define UIE_DESC05_OFF      0x00000400L     /* descriptor No.5  - off   */
#define UIE_DESC04_OFF      0x00000100L     /* descriptor No.4  - off   */
#define UIE_DESC03_OFF      0x00000040L     /* descriptor No.3  - off   */
#define UIE_DESC02_OFF      0x00000010L     /* descriptor No.2  - off   */
#define UIE_DESC01_OFF      0x00000004L     /* descriptor No.1  - off   */
#define UIE_DESC00_OFF      0x00000001L     /* descriptor No.0  - off   */

#define UIE_DESC15_ON       0x80000000L     /* descriptor No.15 - on    */
#define UIE_DESC14_ON       0x20000000L     /* descriptor No.14 - on    */
#define UIE_DESC13_ON       0x08000000L     /* descriptor No.13 - on    */
#define UIE_DESC12_ON       0x02000000L     /* descriptor No.12 - on    */
#define UIE_DESC11_ON       0x00800000L     /* descriptor No.11 - on    */
#define UIE_DESC10_ON       0x00200000L     /* descriptor No.10 - on    */
#define UIE_DESC09_ON       0x00080000L     /* descriptor No.9  - on    */
#define UIE_DESC08_ON       0x00020000L     /* descriptor No.8  - on    */
#define UIE_DESC07_ON       0x00008000L     /* descriptor No.7  - on    */
#define UIE_DESC06_ON       0x00002000L     /* descriptor No.6  - on    */
#define UIE_DESC05_ON       0x00000800L     /* descriptor No.5  - on    */
#define UIE_DESC04_ON       0x00000200L     /* descriptor No.4  - on    */
#define UIE_DESC03_ON       0x00000080L     /* descriptor No.3  - on    */
#define UIE_DESC02_ON       0x00000020L     /* descriptor No.2  - on    */
#define UIE_DESC01_ON       0x00000008L     /* descriptor No.1  - on    */
#define UIE_DESC00_ON       0x00000002L     /* descriptor No.0  - on    */

#define UIE_DESC15_BLINK    0xC0000000L     /* descriptor No.15 - blink */
#define UIE_DESC14_BLINK    0x30000000L     /* descriptor No.14 - blink */
#define UIE_DESC13_BLINK    0x0C000000L     /* descriptor No.13 - blink */
#define UIE_DESC12_BLINK    0x03000000L     /* descriptor No.12 - blink */
#define UIE_DESC11_BLINK    0x00C00000L     /* descriptor No.11 - blink */
#define UIE_DESC10_BLINK    0x00300000L     /* descriptor No.10 - blink */
#define UIE_DESC09_BLINK    0x000C0000L     /* descriptor No.9  - blink */
#define UIE_DESC08_BLINK    0x00030000L     /* descriptor No.8  - blink */
#define UIE_DESC07_BLINK    0x0000C000L     /* descriptor No.7  - blink */
#define UIE_DESC06_BLINK    0x00003000L     /* descriptor No.6  - blink */
#define UIE_DESC05_BLINK    0x00000C00L     /* descriptor No.5  - blink */
#define UIE_DESC04_BLINK    0x00000300L     /* descriptor No.4  - blink */
#define UIE_DESC03_BLINK    0x000000C0L     /* descriptor No.3  - blink */
#define UIE_DESC02_BLINK    0x00000030L     /* descriptor No.2  - blink */
#define UIE_DESC01_BLINK    0x0000000CL     /* descriptor No.1  - blink */
#define UIE_DESC00_BLINK    0x00000003L     /* descriptor No.0  - blink */


/*
*===========================================================================
*   Condition of Error Message
*===========================================================================
*/
#define UIE_WITHOUT         0               /* only clear key         */
#define UIE_ABORT           0x0001          /* enable abort key       */
#define UIE_EM_AC           0x0002          /* enable action code key */
#define UIE_EM_TONE         0x0004          /* disable error tone     */
#define UIE_EM_SUPR_TONE    0x0008          /* enable Supervisor Intervention tone     */
#define UIE_EM_NO_CLEAR     0x0010          // do not perform UieClearRingBuf if IDOK
#define UIE_EM_DIALOG_2     0x1000          // indicates to use dialog handler number 2
#define UIE_EM_DIALOG_3     0x2000          // indicates to use dialog handler number 3
#define UIE_EM_DIALOG_4     0x3000          // indicates to use dialog handler number 4

#define UIE_ERR_TONE           100          /* length of error tone */
#define UIE_ALERT_TONE         250          /* length of alert tone */
#define UIE_CLICK_TONE          20          /* length of click tone */
#define UIE_SPVINT_TONE        500          /* length for a Supervisor Tone, see also COMMON_SPVINT_TONE */

/*
*===========================================================================
*   Coin Despensor Interface
*===========================================================================
*/
#define UIE_COIN_ERROR              UIE_DEVICE_ERROR          /* error when accessing the device */
#define UIE_COIN_ERROR_NOT_PROVIDE  UIE_DEVICE_NOT_PROVIDE    /* not provide         */
#define UIE_COIN_ERROR_RANGE        UIE_DEVICE_ERROR_RANGE    /* range error         */
#define UIE_COIN_ERROR_COM          UIE_DEVICE_ERROR_COM      /* communication error */


/*
*===========================================================================
*   Conversion and FSC Table
*===========================================================================
*/
typedef struct _CVTTBL {
    UCHAR   uchASCII;                       /* ASCII code */
    USHORT   uchScan;                        /* scan code  */
} CVTTBL;

// WARNING:  The struct FSCTBL must have the same layout as struct FSCDATA in para.h
//           See function UieConvertFsc() and Para memory resident data structure
//           member PARAFSCTBL  ParaFSC[MAX_FSC_TBL];
typedef struct _FSCTBL {
    UCHAR   uchMajor;                       /* major code also FSCDATA.uchFsc or FSC Code */
    UCHAR   uchMinor;                       /* minor code also FSCDATA.uchExt or Extend Code */
} FSCTBL;

/*
*===========================================================================
*   Scale Interface
*===========================================================================
*/
#if defined(PERIPHERAL_DEVICE_SCALE)
#define UIE_SCALE_ERROR             UIE_DEVICE_ERROR          /* error          */
#define UIE_SCALE_NOT_PROVIDE       UIE_DEVICE_NOT_PROVIDE    /* not provide    */
#define UIE_SCALE_TIMEOUT           UIE_DEVICE_TIMEOUT        /* time out when reading from scale  */
#define UIE_SCALE_ERROR_COM         UIE_DEVICE_ERROR_COM      /* communications error when reading from scale  */
#define UIE_SCALE_MOTION            (UIE_DEVICE_SPECIFIC-1)   /* motion         */
#define UIE_SCALE_UNDER_CAPACITY    (UIE_DEVICE_SPECIFIC-2)   /* under capacity */
#define UIE_SCALE_OVER_CAPACITY     (UIE_DEVICE_SPECIFIC-3)   /* over capacity  */
#define UIE_SCALE_DATAFORMAT        (UIE_DEVICE_SPECIFIC-4)   /* Data read from scale incorrect format in UieScaleAnalysis()  */
#define UIE_SCALE_DATAUNITS         (UIE_DEVICE_SPECIFIC-5)   /* Units read from scale incorrect in UieScaleAnalysis()  */

// scale units of measurement
#define UIE_SCALE_UNIT_LB               1           /* pound          */
#define UIE_SCALE_UNIT_KG               2           /* kilogram       */
#define UIE_SCALE_UNIT_UNKNOWN        255           /* Unknown units of measurement */

typedef struct _UIE_SCALE {
    UCHAR   uchUnit;                            /* unit           */
    UCHAR   uchDec;                             /* precision      */
    ULONG   ulData;                             /* data           */
} UIE_SCALE;
#endif


/*
*===========================================================================
*   Display Interface
*===========================================================================
*/
/* 
===== Caution!! =====
UIE.H and PIF.H should be the same as this definition.
===== Caution!! =====
*/
#if defined(DISP_OPER)
#define UIE_OPER            DISP_OPER           /* operator display */
#define UIE_CUST            DISP_CUST           /* customer display */

#define UIE_ATTR_NORMAL     DISP_ATTR_NORMAL    /* normal status    */
#define UIE_ATTR_BLINK      DISP_ATTR_BLINK     /* blink status     */

#define UIE_DESC_OFF        DISP_DESCR_OFF      /* descriptor off   */
#define UIE_DESC_ON         DISP_DESCR_ON       /* descriptor on    */
#define UIE_DESC_BLINK      DISP_DESCR_BLINK    /* descriptor blink */
#else
#define UIE_OPER            1                   /* operator display */
#define UIE_CUST            2                   /* customer display */

#define UIE_ATTR_NORMAL     0x00                /* normal status    */
#define UIE_ATTR_BLINK      0x80                /* blink status     */

#define UIE_DESC_OFF        0                   /* descriptor off   */
#define UIE_DESC_ON         1                   /* descriptor on    */
#define UIE_DESC_BLINK      2                   /* descriptor blink */
#endif

typedef struct _UIEDESCRLCD {
    USHORT  usOffset;                       /* offset of buffer     */
    UCHAR   uchLength;                      /* length of descriptor */
} UIEDESCLCD;

// ----------------------
// following defines used with function UieGetMessage() as second argument for optional behavior
#define UIEGETMESSAGE_FLAG_NOOPTIONS     0x0000     // place holder for UieGetMessage() with no options
#define UIEGETMESSAGE_FLAG_EMPTYRINGBUF  0x0004     // when used with UieGetMessage() clears ring buffer
#define UIEGETMESSAGE_FLAG_MACROPAUSE    0x0008     // when used with UieGetMessage() pauses control string until input complete

/*
*===========================================================================
*   User Interface Message
*===========================================================================
*/
#if defined(PERIPHERAL_DEVICE_SCANNER)

#define UIE_SCANNER_DATALENGTH      (UIE_DEVICE_SPECIFIC-1)   /* length of the data read in is incorrect  */
#define UIE_SCANNER_MISSING_STX     (UIE_DEVICE_SPECIFIC-2)   /* Initial STX, Start of Transmission character, missing */
#define UIE_SCANNER_MISSING_ETX     (UIE_DEVICE_SPECIFIC-3)   /* Final ETX, End of Transmission character, missing  */

#define UIE_LEN_SCANNER     (1 + 101)       /* length of scanner       */

typedef struct _UIE_SCANNER {               /* scanner                 */
    UCHAR   uchStrLen;                      /* scanner length          */
    TCHAR   auchStr[UIE_LEN_SCANNER + 1];   /* scanner data            *///RPH WIDE
} UIE_SCANNER;
#endif


#if defined(PERIPHERAL_DEVICE_MSR)
/* 
===== Caution!! =====
UIE.H and PIF.H should be the same as this definition.
===== Caution!! =====
*/
#if defined(MSR_BUFFER)
#define UIE_LEN_TRACK1      PIF_LEN_TRACK1  /* length of ISO track-1   */
#define UIE_LEN_TRACK2      PIF_LEN_TRACK2  /* length of ISO track-2   */
#define UIE_LEN_TRACKJ      PIF_LEN_TRACKJ  /* length of JIS-II        */
#define PIF_LEN_TRACK2_PAD       6   // padding used with function RflEncryptByteString(), RflDecryptByteString().

typedef MSR_BUFFER     UIE_MSR;
typedef PINPAD_BUFFER  UIE_PINPAD;
#else
#define UIE_LEN_TRACK1      80              /* length of ISO track-1, same as PIF_LEN_TRACK1 */
#define UIE_LEN_TRACK2      44              /* length of ISO track-2, same as PIF_LEN_TRACK2 */
#define UIE_LEN_TRACKJ     108              /* length of JIS-II, same as PIF_LEN_TRACKJ */
#define UIE_LEN_TRACK2_PAD   6              // padding used with function RflEncryptByteString(), RflDecryptByteString()

typedef struct _UIE_MSR {
    UCHAR   uchLength1;                     /* ISO track-1 length      */
    TCHAR   auchTrack1[UIE_LEN_TRACK1];     /* ISO track-1 data        */
    UCHAR   uchLength2;                     /* ISO track-2 length      */
    TCHAR   auchTrack2[UIE_LEN_TRACK2];     /* ISO track-2 data        */
    UCHAR   uchLengthJ;                     /* JIS track-II length     */
    TCHAR   auchTrackJ[UIE_LEN_TRACKJ];     /* JIS track-II data       */
	UCHAR	uchPaymentType;
} UIE_MSR;

typedef struct _UIE_PINPAD {
    USHORT      usLength;
    TCHAR       auchEncryptedPin[64];
    USHORT      usLengthKsn;
    TCHAR       auchKeySerialNumber[64];
    UCHAR       uchLength1;                     /* ISO track-1 length      */
    TCHAR       auchTrack1[UIE_LEN_TRACK1];     /* ISO track-1 data        */
    UCHAR       uchLength2;                     /* ISO track-2 length      */
    TCHAR       auchTrack2[UIE_LEN_TRACK2];     /* ISO track-2 data        */
	UCHAR    	uchPaymentType;
	LONG        lAmount;                        // amount to display as part of PIN entry
	USHORT      usOptions;                      // options for PINPad device I/O UIF_PINPAD_OPTION_READ_PIN and others
	SHORT       sPinPadRet;                     // contains EEPT_RET_xxx status from XEptDllCollectStripeAndPin(), XEptDllCollectPin(), etc.
} UIE_PINPAD;                               // this struct must be same as struct PINPAD_BUFFER in size and members

typedef struct _UIE_SIGCAP {
	USHORT   usOptions;                      // options for the I/O request; UIE_SIGCAP struct
	LONG     m_SigCapMaxX;                   // max X for signature; UIE_SIGCAP struct
	LONG     m_SigCapMaxY;                   // max X for signature; UIE_SIGCAP struct
	TCHAR   *m_SigCapPointArray;             // pointer to buffer holding signature; UIE_SIGCAP struct
	USHORT   m_ArrayLength;                  // length of the array currently in m_SigCapPointArray; UIE_SIGCAP struct
	USHORT   m_ArrayMaxSize;                 // max buffer size containing signature; UIE_SIGCAP struct
} UIE_SIGCAP;

typedef struct _UIE_OPERATORACTION {
	USHORT   usOptions;                     // options for the Operator Action request
	USHORT   usClass;						// class of the mnemonic if requested
	USHORT   usAddress;						// address of the mnemonic if requested
	USHORT   usMdcAddress;					// address of the MDC involved assumes use of CliParaMDCCheckField();
	USHORT   usMdcBit;                      // designates bit of the MDC address above assumes use of CliParaMDCCheckField()
	USHORT   usControlStringId;             // designates the identifier of a control string to run if non-zero
} UIE_OPERATORACTION;

#endif
#endif

// Define the UIE version of the Virtual Keyboard buffer, FSC_VIRTUALKEYEVENT
#if defined(PERIPHERAL_DEVICE_VIRTUALKEY)
#if defined(VIRTUALKEY_BUFFER)
#define UIE_LEN_TRACK1      PIF_LEN_TRACK1       /* length of ISO track-1   */
#define UIE_LEN_TRACK2      PIF_LEN_TRACK2       /* length of ISO track-2   */
#define UIE_LEN_TRACKJ      PIF_LEN_TRACKJ       /* length of JIS-II        */
#define UIE_LEN_TRACK2_PAD  PIF_LEN_TRACK2_PAD   // padding used with function RflEncryptByteString(), RflDecryptByteString().

#define UIE_VIRTUALKEY_EVENT_FREEDOMPAY      VIRTUALKEY_EVENT_FREEDOMPAY
#define UIE_VIRTUALKEY_EVENT_SCANNER         VIRTUALKEY_EVENT_SCANNER
#define UIE_VIRTUALKEY_EVENT_MSR             VIRTUALKEY_EVENT_MSR

typedef VIRTUALKEY_BUFFER  UIE_VIRTUALKEY
#else
#define UIE_VIRTUALKEY_EVENT_FREEDOMPAY      1
#define UIE_VIRTUALKEY_EVENT_SCANNER         2
#define UIE_VIRTUALKEY_EVENT_MSR             3

typedef struct {
	USHORT  usKeyEventType;      // Type of event hence type of data in this datastructure, VIRTUALKEY_EVENT_
	union {
		UIE_MSR      freedompay_data;   // data associated with usKeyEventType == UIE_VIRTUALKEY_EVENT_FREEDOMPAY
		UIE_SCANNER  scanner_data;      // data associated with usKeyEventType == UIE_VIRTUALKEY_EVENT_SCANNER
		UIE_MSR      msr_data;          // data associated with usKeyEventType == UIE_VIRTUALKEY_EVENT_MSR
	} u;
} UIE_VIRTUALKEY;
#endif
#endif


#define UIM_INIT            1               /* Initialize Message      */
#define UIM_ACCEPTED        2               /* Accepted Message        */
#define UIM_REJECT          3               /* Reject Message          */
#define UIM_QUIT            4               /* Quit Message            */
#define UIM_CANCEL          5               /* Cancel Message          */
#define UIM_REDISPLAY       6               /* Re-Display Message      */
#define UIM_INPUT           7               /* Input Data Message      */
#define UIM_MODE            8               /* Mode Change Message     */
#define UIM_DRAWER          9               /* Drawer Close Message    */
#define UIM_ERROR           10              /* Error Message           */

#define UIE_MAX_KEY_BUF     (44 + 1)        /* length of keyboard buf. */

// WARNING: dependency on first part of this layout being same as UIESUBMSG in
//          SUP Mode Supervisor Mode menu tree processing handling UIM_REJECT, etc.
typedef struct _KEYMSG {
    UCHAR   uchMsg;                         /* message ID, same as UIESUBMSG.uchMsg */
    union _SEL {
        UCHAR   uchMode;                    /* position of mode lock   */
        USHORT  usFunc;                     /* misc. data, same as UIESUBMSG.usFunc */
        struct _INPUT {
            UCHAR       uchMajor;           /* major code              */
            UCHAR       uchMinor;           /* minor code              */
            UCHAR       uchLen;             /* length of aszKey        */
            UCHAR       uchDec;             /* length of decimal       */
            DCURRENCY   lData;              /* long data of keyboard   */
			ULONG       ulStatusFlags;          // status flags to indicate various conditions
			LONG		lAskKeyPreData;
            TCHAR       aszKey[UIE_MAX_KEY_BUF];/* string data         */
#if defined(PERIPHERAL_DEVICE_SCANNER) || defined(PERIPHERAL_DEVICE_MSR)
            union _DEV {
#if defined(PERIPHERAL_DEVICE_SCANNER)
                UIE_SCANNER SCA;            /* scanner                 */
#endif
#if defined(PERIPHERAL_DEVICE_MSR)
                UIE_MSR     MSR;            /* MSR, SEL.INPUT.DEV.MSR     */
                UIE_PINPAD  PINPAD;         /* PIN pad, SEL.INPUT.DEV.PINPAD      */
                UIE_SIGCAP  SIGCAP;         /* PIN pad, SEL.INPUT.DEV.SIGCAP      */
#endif
#if defined(PERIPHERAL_DEVICE_VIRTUALKEY)
                UIE_VIRTUALKEY     VIRTUALKEY;   //Virtual keyboard device buffer
#endif
#if defined(PERIPHERAL_DEVICE_OTHER)
                UCHAR       auchOther[PERIPHERAL_DEVICE_OTHER];
#endif
				UIE_OPERATORACTION  OPERATOR_ACTION;
            } DEV;
#endif
        } INPUT;
    } SEL;
} KEYMSG;


/*
*===========================================================================
*   Key Sequence Control
*===========================================================================
*/
typedef struct _UIMENU {
    SHORT   (*sFunc)(KEYMSG *pData);        /* function address */
    USHORT  usName;                         /* function name ID */
} UIMENU;

typedef UCHAR   UISEQ;                      /* key seqeunce     */


#if !defined(MB_OK)
/*
 * MessageBox() Flags
 */
#define MB_OK                       0x00000000L
#define MB_OKCANCEL                 0x00000001L
#define MB_ABORTRETRYIGNORE         0x00000002L
#define MB_YESNOCANCEL              0x00000003L
#define MB_YESNO                    0x00000004L
#define MB_RETRYCANCEL              0x00000005L
#define MB_CANCELTRYCONTINUE        0x00000006L

#define MB_ICONHAND                 0x00000010L
#define MB_ICONQUESTION             0x00000020L
#define MB_ICONEXCLAMATION          0x00000030L
#define MB_ICONASTERISK             0x00000040L

#define MB_USERICON                 0x00000080L
#define MB_ICONWARNING              MB_ICONEXCLAMATION
#define MB_ICONERROR                MB_ICONHAND

#define MB_ICONINFORMATION          MB_ICONASTERISK
#define MB_ICONSTOP                 MB_ICONHAND

#define MB_DEFBUTTON1               0x00000000L
#define MB_DEFBUTTON2               0x00000100L
#define MB_DEFBUTTON3               0x00000200L
#define MB_DEFBUTTON4               0x00000300L

#define MB_APPLMODAL                0x00000000L
#define MB_SYSTEMMODAL              0x00001000L
#define MB_TASKMODAL                0x00002000L
#define MB_HELP                     0x00004000L // Help Button
#endif

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/
/* ----- UIEINIT.C ----- */
VOID    UieInitialize(CONST CVTTBL * *pCvt, CONST FSCTBL * *pFsc, UCHAR uchMax);

/* ----- UIEFUNC.C ----- */
VOID    UieInitMenu(UIMENU CONST *pMenu);
VOID    UieQuitAndInitMenu(UIMENU CONST *pMenu);
VOID    UieNextMenu(UIMENU CONST *pMenu);
VOID    UieAccept(VOID);
VOID    UieReject(VOID);
VOID    UieExit(UIMENU CONST *pMenu);

VOID    UieOpenSequence(UISEQ CONST *pszSeq);
VOID    UieCloseSequence(VOID);
SHORT   UieVirtualKeyToPhysicalDev (SHORT usRequiredKeyEventType, KEYMSG *pData);

/* uifrgdef.c */
VOID    UifSignalStringWaitEvent (USHORT usEventNumber);             // send a signal to be received by UifRegStringWaitEvent()
VOID    UifSignalStringWaitEventIfWaiting (USHORT usEventNumber);    // send a signal to be received by UifRegStringWaitEvent() if something is waiting
SHORT   UifCreateQueueStringWaitEvent(VOID);
SHORT   UifRegCallString(USHORT *pusSize);                      /* R3.0 */
SHORT   UifRegPauseString(USHORT *pusSize, ULONG  ulFlags);     /* R3.0 */

/* ----- UIEDIALO.C ----- */
VOID    UieCreateDialog(VOID);
VOID    UieDeleteDialog(VOID);
SHORT   UieShellExecuteFile (TCHAR *aszPathAndName);
SHORT   UieShellExecuteFileMonitor (TCHAR *aszPathAndName, TCHAR *aszCommandLine);
SHORT   UieShelExecuteLinkFile (TCHAR *aszName, UCHAR uchLen);

/* ----- UIESEQ.C ----- */
SHORT   UieCheckSequence(KEYMSG *pData);
SHORT   UieSetExtendedStatus (SHORT sRetStatus);
SHORT   UieDefProc(KEYMSG *pData);

/* ----- UIEKEY.C ----- */
VOID    UieSetCvt(CONST CVTTBL *pCvt);
VOID    UieSetFsc(CONST FSCTBL *pFsc);
UCHAR   UieSetAbort(UCHAR fchAbort);
SHORT   UieReadClearKey(VOID);
SHORT   UieReadAbortKey(VOID);
USHORT  UieModeChange(BOOL fStatus);

LONG     UiePutKeyData(ULONG, CONST CHARDATA *, VOID *);
USHORT   UieSetKBMode (USHORT  usNewMode);

/* ----- UIEDRAW.C ----- */
SHORT   UieOpenDrawer(USHORT usNo);
VOID    UieDrawerComp(BOOL fStatus);

/* ----- UIESCAN.C ----- */
#if defined(PERIPHERAL_DEVICE_SCANNER)
USHORT  UieNotonFile(USHORT usControl);
#endif

/* ----- UIERING.C ----- */
VOID    UieWriteRingBuf(FSCTBL Fsc, CONST VOID *pvData, USHORT usLength);
VOID    UieClearRingBuf(VOID);
UCHAR	UieReadMode(VOID);
SHORT	UiePeekRingBuf(FSCTBL *pFsc, VOID *pvData, USHORT *pusLength);
SHORT   UieReadRingBuf(FSCTBL *pFsc, VOID *pvData, USHORT *pusLength);

/* ----- UIEDEV.C ----- */
USHORT   UieCtrlDevice(USHORT fsDevice);
#if 0
#pragma message("    ****    UieGetMessage_Debug() is turned on \z    *****")
VOID     UieGetMessage_Debug(KEYMSG *pMsg, USHORT usUieGetErrorActionFlags, char *aszFilePath, int nLineNo);
#define UieGetMessage(p,usFlags) UieGetMessage_Debug(p, USHORT usUieGetErrorActionFlags,__FILE__,__LINE__)
#else
VOID     UieGetMessage(KEYMSG *pMsg, USHORT usUieGetErrorActionFlags);
#endif
VOID     UieSetMacroTbl(CONST UCHAR *puchAddr, UCHAR uchRecordSize);
VOID     UieSetMacro(USHORT usControlStringNumber, CONST FSCTBL *pFsc, USHORT usNumber);
SHORT	 UieMacroRunning (VOID);

/* ----- UIEECHO.C ----- */
VOID     UieEchoBack(UCHAR uchType, UCHAR uchMax);
VOID     UieEchoBackGround(ULONG flDescriptor, UCHAR uchAttr, CONST TCHAR *pszFmt, ...);
VOID     UieCtrlBackGround(VOID);
VOID     UieEchoBackWide(USHORT usRow, USHORT usColumn);
VOID     UiePresetCash(DCURRENCY lAmount);
VOID     UieKeepMessage(VOID);
USHORT   UieSetMessage(USHORT usFigure);

/* ----- UIEDISP.C ----- */
SHORT   UiePrintf(USHORT usDisplay, USHORT usRow, USHORT usColumn, CONST TCHAR *pszFmt, ...);
SHORT   UieAttrPrintf(USHORT usDisplay, USHORT usRow, USHORT usColumn, UCHAR uchAttr, CONST TCHAR *pszFmt, ...);
VOID    UieDescriptor(USHORT usDisplay, USHORT usNo, UCHAR uchAttr);
VOID    UieSetDescriptor(CONST UIEDESCLCD *pInfo, CONST TCHAR *puchBuf, USHORT usCount);

/* ----- UIEERROR.C ----- */
VOID    UieSetErrorTbl(CONST UCHAR *puchAddr, UCHAR uchRecordSize);

#if 0
VOID    UieErrorDisplay_Debug(USHORT usErrorCode, USHORT fsCondition, TCHAR  *szOptionalText, char *aszFilePath, int nLineNo);
int     UieMessageBox_Debug (VOID *lpText, VOID *lpCaption, unsigned int uiFlags, char *aszFilePath, int nLineNo);
vOID	UieMessageBoxNonModalPopUp_Debug (VOID  *lpText, VOID *lpCaption, unsigned int uiFlags, VOID *lpServiceName, ULONG  ulEventId, char *aszFilePath, int nLineNo);

#define UieErrorDisplay(usErrorCode,fsCondition,szOptionalText)  UieErrorDisplay_Debug(usErrorCode,fsCondition,szOptionalText,__FILE__,__LINE__)
#define UieMessageBoxNonModalPopUp(lpText,lpCaption, uiFlags,lpServiceName,ulEventId)  UieMessageBoxNonModalPopUp_Debug(lpText,lpCaption,uiFlags,lpServiceName,ulEventId,__FILE__, __LINE__)
#else
VOID    UieErrorDisplay(USHORT usErrorCode, USHORT fsCondition, TCHAR  *szOptionalText);
VOID	UieMessageBoxNonModalPopUp (VOID  *lpText, VOID *lpCaption, unsigned int uiFlags, VOID *lpServiceName, ULONG  ulEventId);
#endif
int     UieMessageBox (VOID *lpText, VOID *lpCaption, unsigned int uiFlags);
int		UieMessageBoxNonModalPopUpUpdate (VOID  *lpText);
int		UieMessageBoxNonModalPopDown (VOID);
int     UieMessageBoxSendMessage (ULONG message, ULONG wParam, ULONG lParam);


#if 0
UCHAR   UieErrorMsg_Debug(USHORT usErrorCode, USHORT fsCondition, char *aszFilePath, int nLineNo);
UCHAR   UieErrorMsgExtended_Debug(USHORT usErrorCode, USHORT fsCondition, SHORT sRetStatus, char *aszFilePath, int nLineNo);
#define UieErrorMsg(usErrorCode, usCondition)  UieErrorMsg_Debug (usErrorCode,usCondition,__FILE__,__LINE__)
#define UieErrorMsgExtended(usErrorCode, usCondition, sRetStatus)  UieErrorMsgExtended_Debug (usErrorCode,usCondition,sRetStatus,__FILE__,__LINE__)
#else
UCHAR   UieErrorMsg(USHORT usErrorCode, USHORT usCondition);
UCHAR   UieErrorMsgExtended(USHORT usErrorCode, USHORT fsCondition, SHORT sRetStatus);
#endif
UCHAR   UieErrorMsgWithText(USHORT usErrorCode, USHORT usCondition, TCHAR *pText);
VOID    UieAddExtendedErrorCode (USHORT  usLdtErrorCode, USHORT usLdtExtendedErrorCode);
VOID    UieClearExtendedErrorCode (VOID);
int     UieRemoveExtendedErrorCode (USHORT  *usLdtErrorCode, USHORT *usLdtExtendedErrorCode);
VOID    UieSetEptErrorText(SHORT  sCpmError, TCHAR *tcsErrorText);
VOID    UieQueueStanzaToPlay (CONST STANZA *pStanzaAddr);
VOID    UieQueueStanzaStopCurrent (void);
VOID THREADENTRY UiePlayStanzaThread (void);
VOID    UieQueueStanzaInit ();

/* ----- UIESCALE.C ----- */
#if defined(PERIPHERAL_DEVICE_SCALE)
SHORT   UieReadScale(UIE_SCALE *pData);
#endif

/* ----- UIECOIN.C ----- */
SHORT   UieCoinSendData(DCURRENCY lChange);
SHORT   UieCoinSendData2(SHORT sChange);
SHORT   DoAutoSignOut(SHORT SignOutKey);

/* ----- UIEMSR.C ----- */
#if defined(PERIPHERAL_DEVICE_MSR)
LONG     UiePutMsrData(ULONG, CONST MSR_BUFFER *, VOID *);
LONG     UiePutPinPadData(ULONG, CONST PINPAD_BUFFER *, VOID *);
LONG     UiePutSignatureCaptureData(ULONG, CONST UIE_SIGCAP *, VOID *);
#endif

LONG   UiePutOperatorActionData(ULONG ulLength, CONST UIE_OPERATORACTION *pData, VOID *pResult);

SHORT  UieCheckMsrDeviceType (USHORT usFlags);

LONG   UiePutVirtualKeyData(ULONG ulLength, CONST VIRTUALKEY_BUFFER *pData, VOID *pResult);

int    UieDisplayMessageBoxExt (USHORT  usMsgBoxHandle, TCHAR *lpText, TCHAR *lpCaption, UINT uiFlags);

#endif    /* _INC_UIE */
/* ================================== */
/* ========== End of UIE.H ========== */
/* ================================== */
