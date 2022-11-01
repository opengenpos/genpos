/*
* ---------------------------------------------------------------------------
* Title         :   Local Header File of Transfer
* Category      :   Transfer, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   TRANSL.H
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All rights reserved.
* --------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /c /AL /W4 /G2s /Zp
* --------------------------------------------------------------------------
* Abstract:
*
*
* --------------------------------------------------------------------------
* Update Histories
*
* Date          Ver.       Name             Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
* Apr-06-95 : 03.00.00 : H.Ishida   : Add R3.0
* Feb-07-97 : 03.00.05 : M.Suzuki   : Add R3.1
* Sep-28-98 : 03.03.00 : A.Mitsui   : V3.3
*
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
*   Define Declarations 
*===========================================================================
*/
#define _INC_TRANSFER_LOCAL

#include    "mypifdefs.h"


/*
*===========================================================================
*   Flexible Memory Re-Assignment
*===========================================================================
*/
/* include MLD Mnemonics file, V3.3 */
#define TRANS_FLEX_ADR_MAX          (FLEX_ADR_MAX + 1)


/*
*===========================================================================
*   Port Combo Box
*===========================================================================
*/
#define TRANS_MAX_PORT              4
#define TRANS_MAX_PORT_LEN          5


/*
*===========================================================================
*   Baud Rate Combo Box
*===========================================================================
*/
#define TRANS_MAX_BAUD              7
#define TRANS_MAX_BAUD_LEN          7


/*
*===========================================================================
*   Transfer/Update Flags
*===========================================================================
*/
#define TRANS_UPDATE_PROG       TRANS_ACCESS_PROG       /* Parameter(setup) */
#define TRANS_UPDATE_ACT        TRANS_ACCESS_ACT        /* Parameter(maint) */
#define TRANS_UPDATE_DEPT       TRANS_ACCESS_DEPT       /* Department File  */
#define TRANS_UPDATE_PLU        TRANS_ACCESS_PLU        /* PLU File         */
#define TRANS_UPDATE_WAITER     TRANS_ACCESS_WAITER     /* Server File      */
#define TRANS_UPDATE_CASHIER    TRANS_ACCESS_CASHIER    /* Cashier File     */
#define TRANS_UPDATE_ETK        TRANS_ACCESS_ETK        /* ETK File         */
#define TRANS_UPDATE_CPN        TRANS_ACCESS_CPN        /* Combination Coupon File */
#define TRANS_UPDATE_CSTR       TRANS_ACCESS_CSTR       /* C.String File    */
#define TRANS_UPDATE_RPT        TRANS_ACCESS_RPT        /* Programmable Report File */
#define TRANS_UPDATE_PPI        TRANS_ACCESS_PPI        /* PPI File         */
#define TRANS_UPDATE_MLD        TRANS_ACCESS_MLD        /* MLD File, V3.3   */

#define TRANS_UPDATE_MASK       TRANS_ACCESS_MASK       /* Mask Bit */

#if defined(POSSIBLE_DEAD_CODE)
/*
*===========================================================================
*   Transfer Abort Graph definition
*===========================================================================
*/
#define TRANS_ABORT_AREAHIGHT          20   /* hight of grapgh area                 */
#define TRANS_ABORT_AREAWIDTH          80   /* width of grapgh area                 */
#define TRANS_ABORT_AREALEFT           10    /* left point of graph area             */
#define TRANS_ABORT_AREATOP            10   /* top point of graph area              */
#define TRANS_ABORT_AREARIGHT   (TRANS_ABORT_AREALEFT + TRANS_ABORT_AREAWIDTH)
                                    /* right point of graph area            */
#define TRANS_ABORT_AREABOTTOM  (TRANS_ABORT_AREATOP + TRANS_ABORT_AREAHIGHT)
                                    /* bottom point of graph area           */
#define TRANS_ABORT_AREAEDGE            2   /* edge of bar in graph area            */
#define TRANS_ABORT_PEN_WIDTH           1   /* width of graph area frame            */

#endif


/*
*===========================================================================
*   Transfer memory block size definition
*===========================================================================
*/
#define TRANS_MEMORY_SIZE_TERM       1024   /* transfer size in terminal */
#define TRANS_MEMORY_SIZE_LOADER     1024   /* transfer size in loader board */
#define TRANS_CONVERT_WORK_SIZE      150    /* work size in convert function */
#define TRANS_PRGRPT_BLOCK_SIZE      1024UL /* programmable report 1 block size */



#define TRANS_IPPOS_MIN     0     /* Min. Value of IP Address Position    */
#define TRANS_IPPOS_MAX     255   /* Max. Value of IP Address Position    */

#define TRANS_MAX_ROW      16
#define TRANS_MAX_COL      4


#define TRANS_IPADD1POS     0
#define TRANS_IPADD2POS     1
#define TRANS_IPADD3POS     2
#define TRANS_IPADD4POS     3
#define TRANS_IPADD5POS     4
#define TRANS_IPADD6POS     5
#define TRANS_IPADD7POS     6
#define TRANS_IPADD8POS     7
#define TRANS_IPADD9POS     8
#define TRANS_IPADD10POS    9
#define TRANS_IPADD11POS    10
#define TRANS_IPADD12POS    11
#define TRANS_IPADD13POS    12
#define TRANS_IPADD14POS    13
#define TRANS_IPADD15POS    14
#define TRANS_IPADD16POS    15

#define TRANS_MAX_IPLEN     3   /* Max. Length of Input IP Data(TEXT)     */

#define TRANS_SPIN_STEP     1   /* Normal Step Count of Spin Button       */
#define TRANS_SPIN_TURBO    0   /* Turbo Point of Spin Button (0 = OFF)   */

/*
*===========================================================================
*   Structure Type Define Declarations
*===========================================================================
*/


/*
*===========================================================================
*   External Work Area Declarations
*===========================================================================
*/
/* ===== TRANS.C ===== */
extern TRANSFER     Trans;

extern WCHAR szTransPort[TRANS_MAX_PORT][TRANS_MAX_PORT_LEN];
extern WCHAR szTransBaud[TRANS_MAX_BAUD][TRANS_MAX_BAUD_LEN];

extern WCHAR szTransDept[];
extern WCHAR szTransPlu[];
extern WCHAR szTransWaiter[];
extern WCHAR szTransCashier[];
extern WCHAR szTransEtk[];
extern WCHAR szTransCpn[];
extern WCHAR szTransCstr[];
extern WCHAR szTransProgRpt[];
extern WCHAR szTransPPI[];
extern WCHAR szTransMLD[];       /* V3.3 */
extern WCHAR szTransRSN[];       /* V3.3 */
extern WCHAR szTransPluIndex[];  /* Saratoga */
extern WCHAR szTransOep[];       /* Saratoga */
extern WCHAR szTransTouchLAY[]; //ESMITH LAYOUT
extern WCHAR szTransKeyboardLAY[];
extern WCHAR szTransReceiptLogo[]; //Receipt Logo JHALL
extern WCHAR szTransLogoIndex[];
extern WCHAR szTransLogoFile[];
extern WCHAR szTransFPDB[];

/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/
/* ===== TRANS.C ===== */
void TransResetFlexMem(void);
void TransResetData(void);
int TransMessage(HWND, UINT, UINT, UINT, UINT, ...);
short TransGetIndex(WORD);
void TransCreateFiles(void *, void *);
TERMINAL getTransTerm(void);
const void * getPtrTransTerm(void);
BYTE getTermByPort(void);
void setTermByPort(BYTE byte);
WORD getTermWBaudRate(void);
void setTermWBaudRate(WORD word);
BYTE getTermByByteSize(void);
BYTE getTermByParity(void);
BYTE getTermByStopBits(void);
BYTE getTermByTerm(void);
void setTermByTerm(BYTE byte);
BYTE getTermFByStatus(void);
void andTermFByStatus(BYTE byte);
void orTermFByStatus(BYTE byte);
short getTermIDir(void);
void incrTermIDir(short);
WORD getTransFwTransTerm(void);
const void * getPtrTransFwTransTerm(void);
void orTransFwTransTerm(WORD);
void andTransFwTransTerm(WORD);
WORD getTransFwUpdate(void);

/* ===== TRAABORT.C ===== */
void TransAbortCreateDlg(HWND);
void TransAbortDestroyDlg(void);
short TransAbortDlgCalcFileMsg(HWND, WORD, ULONG, short,WCHAR *);   /* Saratoga */
short TransAbortDlgCalcParaMsg(HWND, WORD, WORD);
short TransAbortDlgCalcLoadMsg(HWND, DWORD, WORD);
short TransAbortDlgFinMessage(HWND);
BOOL TransAbortQueryStatus(void);
short TransAbortDlgMessage(HWND, WPARAM);
BOOL WINAPI TransAbortDlgProc(HWND, UINT, WPARAM, LPARAM);
VOID TransAbortDrawRate(HWND, WPARAM);
VOID TransAbortClearRate(HWND);
VOID TransAbortInitRate(HWND);

/* ===== TRANCONV.C ===== */
short TransConvertToIniFile(HWND);
short TransConvertEODToIniFile(WORD, unsigned long);
short TransConvertPTDToIniFile(WORD, unsigned long);
short TransConvert16CharToIniFile(WORD, unsigned long);
short TransConvert40CharToIniFile(WORD, unsigned long);
short TransConvertRptToIniFile(WORD, unsigned long, short);
short TransConvertToParaFile(HWND);
short TransConvertEODToParaFile(WORD, unsigned long);
short TransConvertPTDToParaFile(WORD, unsigned long);
short TransConvert16CharToParaFile(WORD, unsigned long);
short TransConvert40CharToParaFile(WORD, unsigned long);
LONG TransConvertRptToParaFile(WORD, unsigned long, short);

/* =====  TRNSIP.C  ===== */
VOID    TransIPInitData(HWND);
BOOL    TransIPSetData(HWND);
BOOL    TransIPChkRndPos(HWND, WPARAM);
VOID    TransIPDispErr(HWND, WPARAM);
VOID    TransIPSpinProc(HWND, WPARAM, LPARAM);
VOID    TransIPDefaultData(VOID);

/* ===== End of TRANSL.H ===== */
