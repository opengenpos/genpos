/*
*===========================================================================
*
*   Copyright (c) 1992-1998, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Local Header File of I/O Module
* Category    : User Interface Enigne, 2170 System Application
* Program Name: UieIO.H
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: 
* Options     : 
* --------------------------------------------------------------------------
* Abstract:     This header file defines User Interface Engine the 2170
*               system application.
*
*               This file contains:
*                   1. Symbol Definitions
*                   2. Data type Definitions
*                   3. External Data Definitions
*                   4. Function Prototypes
* --------------------------------------------------------------------------
* Update Histories
*
* Date       Ver.      Name         Description
* Mar-20-92  00.00.01  O.Nakada     Initial
* Sep-21-92  00.01.03  O.Nakada     Modify stack size of keyboard, drawer
*                                   and waiter lock.
* Feb-26-93  01.00.1?  O.Nakada     Modify MSR sleep timer.
* Mar-31-93  A1        O.Nakada     Modify stack size of scanner No.1, No.2
*                                   and MSR thread.
* May-25-93  C1        O.Nakada     Removed input device control functions.
* Jun-05-93  C1        O.Nakada     Supports NCR 7852, 7870 and 7890.
* Sep-16-93  F1        O.Nakada     Divided the User Interface Engine.
* Mar-15-95  G0        O.Nakada     Modified all declarations, correspond to
*                                   hospitality release 3.0 functions.
* Feb-19-97  G9        M.Suzuki     Added UIE_410_COIN_COM_BAUD,
*                                   UIE_410_COIN_COM_BYTE
* May-29-97  G10       O.Nakada     Supports NCR 7870, 7875 and 7880
*                                   scanner/scale using single cable.
* Sep-17-98  G12       O.Nakada     Supports 6710 and 6720 scale.
*
** NCR2172 **
*
* Oct-05-99  01.00.00  M.Teraki     Added #pragma(...)
*
*** NHPOS 1.4.x, NCR 7448  ***
* Aug-01-02  NHPOS 1.4 R.Chambers   Product donated to Georgia Southern University
*** NHPOS 2.0.x Touchscreen ***
* Mar-01-04  NHPOS 2.0 R.Chambers   Conversion from NCR 7448 to Touchscreen terminals
*** GenPOS 2.2.x Touchscreen ***
* Nov-19-14  GenPOS 2.2.0 R.Chambers  Source cleanup to remove old, before NCR 7448,
*                                     code that applied to NCR 2170 back in the 1990s.
* Oct-27-15  GenPOS 2.2.1 R.Chambers  Get rid of old defines and other cruft,
*                                     move as much as possible into specific device
*                                     interface source files.
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

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

/*
*===========================================================================
*   General Define Declarations
*===========================================================================
*/
#define UIE_INIT_MODE           0xFF        /* not initialization  */

#define UIE_FUNC_KEY            1           /* function key        */
#define UIE_AN_KEY              2           /* alpha/numeric key   */


/*
*===========================================================================
*   Macro Information
*===========================================================================
*/
#define UIE_MACRO_WAIT      0x0001          /* wait for 1 key          */
#define UIE_MACRO_FINAL     0x0002          /* macro finalize          */
#define UIE_MACRO_MODE      0x0004          /* mode changed            */
#define UIE_MACRO_RUNNING   0x0008          /* indicates we are running a control string.  used in debugging strings. */

typedef struct _UIEMACRO {
    CONST   UCHAR *puchMnemo;               /* address of mnemonic     */
    UCHAR   uchRecordSize;                  /* record size of mnemonic */
    CONST   FSCTBL *pFsc;                   /* address of FSC table    */
    USHORT  usNumber;                       /* number of FSC           */
    USHORT  usRead;                         /* offset of reading       */
    USHORT  fsControl;                      /* control flag            */
	USHORT  usControlStringNumber;          /* which control string is being executed */
} UIEMACRO;


/*
*===========================================================================
*   mode lock interface
*===========================================================================
*/
#define UIE_MODE_CHG            0x01        /* on - mode changable */
#define UIE_MODE_LOCK           0x02        /* on - mode lock      */

typedef struct _UIEMODE {
    UCHAR   uchReally;                      /* really position     */
    UCHAR   uchEngine;                      /* engine position     */
    UCHAR   uchAppl;                        /* appl. position      */
    UCHAR   uchMacro;                       /* macro position      */
    UCHAR   uchDialog[3];                   /* dialog funcion  sized to UIE_MAX_FUNC    */
    UCHAR   fchStatus;                      /* control status      */
} UIEMODE;


/*
*===========================================================================
*   Conversion and FSC Table interface
*===========================================================================
*/
#define UIE_NUM_MODEKEY         6           /* number of mode key */

#define UIE_MAX_CTYPE           2           /* default or user    */
#define UIE_MAX_FTYPE           2           /* default or user    */

#define UIE_KBD_DEFAULT         0           /* default            */
#define UIE_KBD_USER            1           /* user               */

typedef struct _UIEKBD {
    UCHAR   uchCType;
    UCHAR   uchFType;
    CONST   CVTTBL *pCvt[UIE_NUM_MODEKEY][UIE_MAX_CTYPE];
    CONST   FSCTBL *pFsc[UIE_NUM_MODEKEY][UIE_MAX_FTYPE];
} UIEKBD;


/*
*===========================================================================
*   drawer interface
*===========================================================================
*/
#define UIE_NUM_DRAWER          2           /* number of drawer         */

#define UIE_DRAWER_ENGINE       0x01        /* drawer opened for engine */
#define UIE_DRAWER_APPL         0x02        /* drawer opened for appl   */
#define UIE_DRAWER_COMP         0x04        /* compulsory drawer        */

#define UIE_DRAWER_TSLEEP       200         /* 200 msec                 */
#define UIE_DRAWER_TOPEN        54          /* 54 msec                  */


/*
*===========================================================================
*   scanner interface FSC_SCANNER, PERIPHERAL_DEVICE_SCANNER and SCF_TYPE_SCANNER
*
*   See also the same definitions in the 78xx Scanner/Scale device interface
*   in file DEVICE78XXSCANNER\UIESCAN.h which need to match these definitions.
*
*===========================================================================
*/
#if defined(PERIPHERAL_DEVICE_SCANNER)

#define UIE_SCANNER1_BAUD       9600        /* Baud Rate                  */
#define UIE_SCANNER1_BYTE       0x0A        /* Odd, 1 Stop, 7 Data        */

#define UIE_SCANNER_OPEN_ERROR  2000        /* sleep timer after open error     */
#define UIE_SCANNER_READ        220         /* read timer of scanner data       */
#define UIE_SCANNER_MONITOR     200         /* monitor timer of scanner disable */
#define UIE_SCANNER_RESPONSE    50          /* response timer                   */

#define UIE_SCANNER_UNKNOWN     (-1)        /* unknown scanner type       */
#define UIE_SCANNER_78SO        0           /* NCR 78xx scanner only      */
#define UIE_SCANNER_78SS        1           /* NCR 78xx scanner/scale     */
#define UIE_SCANNER_78SSBCC     2           /* NCR 78xx scanner/scale BCC */
#define UIE_SCANNER_7852        3           /* NCR 7852                   */
#define UIE_SCANNER_7814        4           /* NCR 7814                   */

#define UIE_SCANNER_REQ_ENA     0x01        /* request scanner enable     */
#define UIE_SCANNER_REQ_DIS     0x02        /* request scanner disable    */
#define UIE_SCANNER_REQ_NOF_ENA 0x04        /* request scanner NOF enable */

#define UIE_SCANNER_SCALE_EXEC  (1 + 1000 / UIE_SCANNER_READ)
#define UIE_SCANNER_SCALE_NONE  (-1)        /* no scale request           */
#define UIE_SCANNER_SCALE_REQ   (-2)        /* wait for request data      */
#define UIE_SCANNER_SCALE_RES   (-3)        /* wait for response data     */

typedef struct _UIECOM {
    UCHAR   uchDevice;                      /* device number              */
    USHORT  usPort;                         /* port number                */
    SHORT   hsHandle;                       /* handle of port             */
    SHORT   sType;                          /* type of scanner            */
    UCHAR   fchRequest;                     /* request status             */
    SHORT   fPip;                           /* type of protocol           */
    USHORT  usBaud;                         /* baud rate                  */
    UCHAR   uchByte;                        /* byte format                */
    UCHAR   uchText;                        /* text format                */
    CHAR    chPhase;                        /* phase of scale function    */
    USHORT  usSem;                          /* exclusive semaphore        */
    USHORT  usSync;                         /* synchronous semaphore      */
    UIE_SCALE   *pData;                     /* address of scale data      */
    SHORT   sResult;                        /* result code of scale       */
} UIECOM;

typedef struct _UIESCANFC {
    SHORT   sLength;                        /* length of function code    */
    UCHAR   *puchCode;                      /* function code              */
} UIESCANFC;
#endif


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
/* ----- UieKey.C ----- */
extern  UIEKBD  UieKbd;                     /* conversion and FSC table */
extern  UIEMODE UieMode;                    /* mode lock status         */

/* ----- UieDev.C ----- */
extern  PifSemHandle  usUieSyncNormal;        /* synchronous semaphore    */
extern  PifSemHandle  fchUieDevice;           /* device status            */
extern  UIEMACRO      UieMacro;               /* macro information        */
extern  USHORT        fsUieKeyDevice;         /* device status            */

/* ----- UieError.C ----- */
extern  PifSemHandle  usUieSyncError;             /* synchronous semaphore    */
extern  PifSemHandle  usUieErrorMsg;              /* exclusive semaphore      */


/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/
/* ----- UieKey.C ----- */
USHORT  DLLENTRY UieModeChange(BOOL fStatus);
VOID    DLLENTRY UieSetStatusKeyboard(VOID);
VOID    DLLENTRY UieSetModeS2A(VOID);
VOID    DLLENTRY UieSetModeS2D(VOID);
VOID    DLLENTRY UieSetModeD2S(VOID);

USHORT  UieGetKeyboardLockMode (VOID);
VOID    UieKeyboardInit(CONST CVTTBL * *pCvt, CONST FSCTBL * *pFsc, UCHAR uchMax);
VOID    THREADENTRY UieKeyboardThread(VOID);
USHORT  UieConvertFsc(CONST CVTTBL *pCvt, CONST FSCTBL *pFsc, CHARDATA Char, FSCTBL *pData);
USHORT  UieCheckAbort(UCHAR uchMajor, UCHAR uchMode);
SHORT   UieCheckMode(VOID);

/* ----- UieDraw.C ----- */
VOID    DLLENTRY UieSetStatusDrawer(VOID);

VOID    UieDrawerInit(VOID);
VOID    UieSetDrawerComp(USHORT usNo, BOOL fStatus);
SHORT   UieCheckDrawerStatus(USHORT usNum);
SHORT   UieReadDrawer(KEYMSG *pMsg);
SHORT   UieDrawerCompulsory(FSCTBL *pFsc);


/* ----- UieWait.C ----- */
VOID    UieWaiterInit(VOID);
VOID    DLLENTRY UieSetStatusWaiter(VOID);
SHORT   UieReadWaiterLock(FSCTBL *pFsc);

/* ----- UieScale.C ----- */
VOID    UieScaleInit(VOID);

/* ----- UieCoin.C ----- */
VOID    UieCoinInit(VOID);

/* ----- UieMsr.C ----- */
VOID    UieMsrInit(VOID);

/* ----- UieScan.C ----- */
VOID    UieScannerInit(VOID);
VOID    UieScannerEnable(VOID);
VOID    UieScannerDisable(VOID);


/* ==================================== */
/* ========== End of UieIO.C ========== */
/* ==================================== */
