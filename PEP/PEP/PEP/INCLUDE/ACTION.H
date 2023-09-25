/*
* ---------------------------------------------------------------------------
* Title         :   2170 PEP Action Code Header File
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   ACTION.H
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /W4 /G2 /GEf /GA /Zp /f- /Os /Og /Oe /Gs
* ---------------------------------------------------------------------------
* Abstract :
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* Dec-07-93 : 00.00.01 : M.Yamamoto : Initial (Migrate from HP US Model)
* Feb-20-95 : 03.00.00 : H.Ishida   : Add Set Order Entry Prompt Table (AC 160)
*           :          :            : Add Coupon Maintenance (AC 161)
* Apr-06-95 : 03.00.00 : H.Ishida   : Add Control String Table (AC 33)
* May-11-95 : 03.00.00 : H.Ishida   : Add Flexible Drivethru (AC 162)
* Jan-30-96 : 03.00.05 : M.Suzuki   : Add R3.1
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
#ifndef ACTION_INCLUDED_DEFINED
#define ACTION_INCLUDED_DEFINED


//RJC  null out PEPENTRY for 32 bit conversion
#ifndef PEPENTRY
#define     PEPENTRY
#endif


#ifndef __DEBUG__
#define __DEBUG__
char    szDebug[526];
#endif

/*
* ===========================================================================
*       DialogBox ID Definision
* ===========================================================================
*/
#define IDD_A04_MAIN    2001        /* Set PLU Number on Menu Page          */
#define IDD_A04_CONV    2002        /* Set PLU Number on Menu Page          */
#define IDD_A04_MICR    2004        /* Set PLU Number on Menu Page          */
#define IDD_A05         2005        /* Set Menu Page Control Table          */
#define IDD_A07         2007        /* Cashier A/B Key Assignment           */
#define IDD_A20         2020        /* Cashier Maintenance                  */
#define IDD_A33         2033        /* Set Control String Table             */
#define IDD_A50         2050        /* Server Maintenance                   */
#define IDD_A68         2068        /* PLU Maintenance                      */
#define IDD_A71         2071        /* Set PPI Table                        */
#define IDD_A84         2084        /* Set Rounding Table                   */
#define IDD_A86         2086        /* Set Discount/Surcharge/Bonus % Rates */
#define IDD_A87         2087        /* Set Slip Promotion Header            */
#define IDD_A88         2088        /* Set Sales Promotion Message          */
#define IDD_A89         2089        /* Set Currency Conversion Rate         */
#define IDD_A111        2111        /* Set Tare Table                       */
#define IDD_A114        2114        /* Department Maintenance               */
#define IDD_A115        2115        /* Set Department No. on Keyed          */
                                    /*       Department Key       NCR2172   */
#define IDD_A116        2116        /* Set Promotional PLU                  */
#define IDD_A124        2124        /* Set Tax Table                        */
#define IDD_A127        2127        /* Set Tax Rate                         */
#define IDD_A130        2130        /* Set Pig Rule Table          R3.1     */
#define IDD_A133        2133        /* Service Time Parameter Maintenance R3.1     */
#define IDD_A137        2137        /* Set Beverage Dispenser Parameter R3.1*/
#define IDD_A152        2152        /* Employee number assignment           */
#define IDD_A154        2154        /* Set Wage Rate for Job Code  R3.1     */
#define IDD_A160        2160        /* Set Order Entry Prompt Table         */
#define IDD_A161        2161        /* Coupon Maintenance                   */
#define IDD_A162        2162        /* Flexible Drivethru                   */
#define IDD_A170        2170        /* Set Sales Restriction Table of PLU   */
                                    /*                            NCR2172   */
#define IDD_A208        2171        /* Set Boundary Age           NCR2172   */
#define IDD_A128        2128        /* ### ADD Saratoga (051900)            */
#define	IDD_A209		2174		/* Color Palette						*/
#define IDD_A156        2175        /* suggested tip amount Maintenance R2.4.1   */

#define IDD_A04_WEDGE_68   2172     /* Set PLU Number on Menu Page NCR Wedge 64 key */
#define IDD_A04_WEDGE_78   2173     /* Set PLU Number on Menu Page NCR Wedge 78 key */

/*
* ===========================================================================
*       Strings ID Definision
* ===========================================================================
*/
    /* ----- Each Procedure String Offset ----- */

#define IDS_ACTION  50000
#define IDS_A04     (IDS_ACTION + 512)
#define IDS_A05     (IDS_A04  + 512)
#define IDS_A07     (IDS_A05  + 512)
#define IDS_A20     (IDS_A07  + 512)
#define IDS_A33     (IDS_A20  + 512)
#define IDS_A50     (IDS_A33  + 512)
#define IDS_A68     (IDS_A50  + 512)
#define IDS_A71     (IDS_A68  + 512)
#define IDS_A84     (IDS_A71  + 512)
#define IDS_A86     (IDS_A84  + 512)
#define IDS_A87     (IDS_A86  + 512)
#define IDS_A88     (IDS_A87  + 512)
#define IDS_A89     (IDS_A88  + 512)
#define IDS_A111    (IDS_A89  + 512)
#define IDS_A114    (IDS_A111 + 532)  // RJC this was 512 rather than 532 but I was getting a RC compiler error
#define IDS_A116    (IDS_A114 + 512)
#define IDS_A124    (IDS_A116 + 512)
#define IDS_A127    (IDS_A124 + 512)
#define IDS_A130    (IDS_A127 + 512)
#define IDS_A133    (IDS_A130 + 512)
#define IDS_A137    (IDS_A133 + 512)
#define IDS_A152    (IDS_A137 + 512)
#define IDS_A154    (IDS_A152 + 512)
#define IDS_A160    (IDS_A154 + 512)
#define IDS_A161    (IDS_A160 + 512)
#define IDS_A162    (IDS_A161 + 512)

/* NCR2172 */
#define IDS_A208    (IDS_A162 + 512)
#define IDS_A115    (IDS_A208 + 512)
#define IDS_A170    (IDS_A115 + 512)
/* ### ADD Saratoga (051900) */
#define IDS_A128    (IDS_A170 + 512)
/* Color Palette Enhancement */
#define IDS_A209	(IDS_A128 + 100)


    /* ----- Common DLL File (PEPCOMM.DLL) Name ----- */

#define IDS_ACT_DLL_FILE   (IDS_ACTION + 1)

    /* ----- DLL File Load Error Message ----- */

#define IDS_ACT_DLL_ERR    (IDS_ACTION + 2)

/*
* ===========================================================================
*       External Valiables
* ===========================================================================
*/
#if defined(POSSIBLE_DEAD_CODE)
// never used
extern  HANDLE      hActInst;       /* Instance Handle of ACTION Window     */
extern  HANDLE      hActCommInst;   /* Instance Handle of PEPCOMM.DLL       */
extern  HANDLE      hActPepInst;    /* Instance Handle of PEP.EXE           */
#endif
extern  HWND        hwndActMain;    /* Window Handle of PEP Window          */
extern  LPHANDLE    lphActDlgChk1;  /* Address of Modeless DialogBox Handle */
extern  LPHANDLE    lphActDlgChk2;  /* Address of Modeless DialogBox Handle */
/*
extern  FARPROC     lpfnA004;
extern  FARPROC     lpfnA004Key;
*/

/*
* ===========================================================================
*       Function Prototypes
* ===========================================================================
*/
VOID    PEPENTRY    ActInitialize(HANDLE, HANDLE, HANDLE);
VOID    PEPENTRY    ActFinalize(VOID);
BOOL    PEPENTRY    DoAction(HWND, DLGPROC, WPARAM, HINSTANCE);
BOOL    PEPENTRY    DoAction04(HWND, LPHANDLE, LPHANDLE, WORD);

    /* ----- DialogBox Procedure of Action Code ----- */

BOOL    WINAPI  A004DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  A004KeyDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  A005DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  A007DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  A020DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  A020CtrlDlgProc(HWND, UINT, WPARAM, LPARAM);    /* V3.3 */
BOOL    WINAPI  A020GroupDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  A033DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  A033CtrlDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  A033StrDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  A050DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  A068MaintDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  A068CtrlDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  A071DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  A071EditDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  A084DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  A086DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  A087DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  A088DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  A089DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  A111DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  A111ErrorDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  A114DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  A114CtrlDlgProc(HWND, UINT, WPARAM, LPARAM);
/* DEL Saratoga */
/* BOOL    WINAPI  A115DlgProc(HWND, UINT, WPARAM, LPARAM);  */     /* NCR2172 */
BOOL    WINAPI  A116DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  A124DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  A124TestDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  A127DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  A130DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  A133DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  A137DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  A152DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  A154DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  A160DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  A161DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  A161CtrlDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL    WINAPI  A162DlgProc(HWND, UINT, WPARAM, LPARAM);

/* NCR2172 */
BOOL    WINAPI  A170DlgProc(HWND, UINT, WPARAM, LPARAM);      /* NCR2172 */
BOOL    WINAPI  A208DlgProc(HWND, UINT, WPARAM, LPARAM);      /* NCR2172 */

BOOL	WINAPI	A209DlgProc(HWND, UINT, WPARAM, LPARAM);	/* Color Palette Enhancement - CSMALL */

/* ### Saratoga (051900) */
BOOL    WINAPI  A128DlgProc(HWND, UINT, WPARAM, LPARAM);      /* NCR2172 */

#endif
