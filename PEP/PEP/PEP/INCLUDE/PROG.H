/*
* ---------------------------------------------------------------------------
* Title         :   Program Mode Setup Header File
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   PROG.H
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract :
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* Dec-07-93 : 00.00.01 : M.Yamamoto : Initial (Migrate from HP US Model)
* Jan-23-96 : 03.00.05 : M.Suzuki   : Add R3.1
* Sep-08-98 : 03.03.00 : A.Mitsui   : V3.3
*
* ===========================================================================
* ===========================================================================
* PVCS Entry
* ---------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
* ===========================================================================
*/
#ifndef PROG_INCLUDED_DEFINED
#define PROG_INCLUDED_DEFINED


/*
* ===========================================================================
*       DialogBox ID Definision
* ===========================================================================
*/
#define IDD_P01         2201        /* Machine Definition Code              */
#define IDD_P02         2202        /* Flexible Memory Assignment           */
#define IDD_P03_CONV    2203        /* Function Selection Code 7448 Conventional  */
#define IDD_P03_MICRO   2204        /* Function Selection Code 7448 Micromotion   */
#define IDD_P03_BOX     2205        /* Function Selection Code              */
#define IDD_P06         2206        /* Program Mode Security Number         */
#define IDD_P08         2208        /* Supervisor Number/Level              */
#define IDD_P09         2209        /* Action Code Security                 */
#define IDD_P10         2210        /* High Amount Lock-out(HALO) Limit     */
#define IDD_P15         2215        /* Preset Amount for Preset Cash Key    */
#define IDD_P17         2217        /* Hourly Activity Time                 */
#define IDD_P18         2218        /* Print Lines on Slip                  */
#define IDD_P20         2220        /* Transaction Mnemonic                 */
#define IDD_P21         2221        /* Lead Trough Message                  */
#define IDD_P22         2222        /* Report Mnemonic                      */
#define IDD_P23         2223        /* Special Mnemonic                     */
#define IDD_P46         2246        /* Adjective Mnemonics                  */
#define IDD_P47         2247        /* Print Modifier Mnemonics             */
#define IDD_P48         2248        /* Major Department Mnemonics           */
#define IDD_P49         2249        /* Automatic Alternative Remote Printer */
#define IDD_P50         2250        /* Assignment Terminal # Installing Shared Printer */
#define IDD_P51         2251        /* Assignment IP Address for KDS NCR2172*/
#define IDD_P54         2254        /* Set Hotel Id and SLD                 */
#define IDD_P57         2257        /* Header Message                       */
#define IDD_P60         2260        /* Total Key Type / Function            */

#define IDD_P62				2262        /* Tender Key Parameter , V3.3          */
#define IDD_P65				2265        /* Multi-Line Display Mnemonics , V3.3  */
#define IDD_P03_WEDGE_68	2266        /* Function Selection Code NCR Wedge 68 key */
#define IDD_P97				2267        /* Terminal Information editor  */
#define IDD_P98				2268        /* Terminal Information editor  */

#define IDD_P03_WEDGE_78	2269        /* Function Selection Code NCR Wedge 78 key */
#define IDD_P67				2270		/* Auto Combinational Coupons			*/ //RLZ
#define IDD_P03_TICKET		2271		//Big Ticket Keyboard
#define IDD_P72				2273		/* Reason Code mnemonics			*/ //RLZ

#define IDD_P223_RPT        2274        /* Financial Report setup           */
#define IDD_P223_1_RPT      2275        /* Cashier Report setup           */

/*
* ===========================================================================
*       Strings ID Definision
* ===========================================================================
*/
    /* ----- Each Procedure String Offset ----- */

#define IDS_PROG    0
#define IDS_P01     (IDS_PROG + 512)
#define IDS_P02     (IDS_P01  + 3072)
#define IDS_P03     (IDS_P02  + 512)
#define IDS_P06     (IDS_P03  + 512)
#define IDS_P08     (IDS_P06  + 512)
#define IDS_P09     (IDS_P08  + 512)
#define IDS_P10     (IDS_P09  + 512)
#define IDS_P15     (IDS_P10  + 512)
#define IDS_P17     (IDS_P15  + 512)
#define IDS_P18     (IDS_P17  + 512)
#define IDS_P20     (IDS_P18  + 512)
#define IDS_P21     (IDS_P20  + 512)
#define IDS_P22     (IDS_P21  + 512)
#define IDS_P23     (IDS_P22  + 512)
#define IDS_P46     (IDS_P23  + 512)
#define IDS_P47     (IDS_P46  + 512)
#define IDS_P48     (IDS_P47  + 512)
#define IDS_P49     (IDS_P48  + 512)
#define IDS_P50     (IDS_P49  + 512)
#define IDS_P51     (IDS_P50  + 512)      /* NCR2172 */
#define IDS_P54     (IDS_P51  + 512)
#define IDS_P57     (IDS_P54  + 512)
#define IDS_P60     (IDS_P57  + 512)

#define IDS_P62     (IDS_P60  + 512)
#define IDS_P65     (IDS_P62  + 512)
#define IDS_P67		(IDS_P65  + 512)
#define IDS_P72		(IDS_P67  + 512)



/*
* ===========================================================================
*       External Valiables
* ===========================================================================
*/
#if defined(POSSIBLE_DEAD_CODE)
extern  HANDLE      hProgInst;      /* Instance Handle of PROG Window       */
extern  HANDLE      hProgCommInst;  /* Instance Handle of PEPCOMM.DLL       */
extern  HANDLE      hProgPepInst;   /* Instance Handle of PEP.EXE           */
#endif
extern  HWND        hwndProgMain;   /* Window Handle of PEP Window          */
extern  LPHANDLE    lphProgDlgChk1; /* Address of Modeless DialogBox Handle */
extern  LPHANDLE    lphProgDlgChk2; /* Address of Modeless DialogBox Handle */
/*
extern  FARPROC     lpfnP001;
extern  FARPROC     lpfnP001EditA;
extern  FARPROC     lpfnP001EditB;
extern  FARPROC     lpfnP001EditCDE;
extern  FARPROC     lpfnP001EditFG;
extern  FARPROC     lpfnP001EditH;
extern  FARPROC     lpfnP001Addr16;
extern  FARPROC     lpfnP001Addr25;
extern  FARPROC     lpfnP001Addr175;
extern  FARPROC     lpfnP003;
extern  FARPROC     lpfnP003Key;
*/
/*
* ===========================================================================
*       Function Prototypes
* ===========================================================================
*/
#if defined(POSSIBLE_DEAD_CODE)
void    ProgInitialize(HANDLE, HANDLE, HANDLE);
// doesn't do anything
void    ProgFinalize(void);
#endif
BOOL    DoProgram(HWND, DLGPROC, WPARAM, HINSTANCE);
BOOL    DoProgram01(HWND, LPHANDLE, LPHANDLE);
BOOL    DoProgram03(HWND, LPHANDLE, LPHANDLE, WPARAM);

    /* ----- DialogBox Procedure of Program Mode ----- */

BOOL    WINAPI  P001DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  P001EditADlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  P001EditBDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  P001EditCDEDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  P001EditFGDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  P001EditHDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  P001Addr16DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  P001Addr25DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  P001Addr175DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  P001Addr259DlgProc(HWND, UINT, WPARAM, LPARAM); /* V3.3 */
BOOL    WINAPI  P001Addr367DlgProc(HWND, UINT, WPARAM, LPARAM); /* Saratoga */
BOOL    WINAPI  P002DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  P003KeyDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  P003ToolDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  P003ExtDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  P003ExtPluDlgProc(HWND, UINT, WPARAM, LPARAM);  /* Saratoga */
BOOL    WINAPI  P003ExtDeptDlgProc(HWND, UINT, WPARAM, LPARAM);  /* Saratoga */
BOOL    WINAPI  P006DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  P008DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  P009DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  P010DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  P015DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  P017DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  P018DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  P020DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  P223DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  P223DlgProc_1(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  P021DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  P022DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  P023DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  P046DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  P047DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  P048DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  P049DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  P050DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  P051DlgProc(HWND, UINT, WPARAM, LPARAM);    /* NCR2172 */
BOOL    WINAPI  P054DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  P057DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  P060DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  P060CtrlDlgProc(HWND, UINT, WPARAM, LPARAM);

BOOL    WINAPI  P062DlgProc(HWND, UINT, WPARAM, LPARAM);    /* V3.3 */
BOOL    WINAPI  P065DlgProc(HWND, UINT, WPARAM, LPARAM);    /* V3.3 */
BOOL    WINAPI  P067DlgProc(HWND, UINT, WPARAM, LPARAM);    /* ACC	*/ //RLZ
BOOL    WINAPI  P097DlgProc(HWND, UINT, WPARAM, LPARAM);    /* V3.3 */
BOOL    WINAPI  P098DlgProc(HWND, UINT, WPARAM, LPARAM);    /* V3.3 */
BOOL    WINAPI  P072DlgProc(HWND, UINT, WPARAM, LPARAM);    /* Reason Code mnemonics	*/ //RLZ

#endif
/* ===== End of File (Prog.H) ===== */
