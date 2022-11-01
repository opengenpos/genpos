/*
* ---------------------------------------------------------------------------
* Title         :   Action Code Security (Prog. 9)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P009.C
* Copyright (C) :   1995, NCR Corp. E&M-OISO, All rights reserved.
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
* Nov-15-93 : 00.00.01 : M.Yamamoto : Initial (Migrate from HP US Model)
* Mar-14-95 : 03.00.00 : H.Ishida   : Add A/C 30,33,40,160,161,162
* Nov-11-99 :          : K.Yanagida : Add A/C 10,11,115,170,218 
* Jul-12-00 : V1.0.0.3 : M.Teraki   : Remove A/C 66, 115
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
/*
* ===========================================================================
*       Include File Declarations
* ===========================================================================
*/
#include    <Windows.h>
#include    <string.h>

#include    <ecr.h>
#include    <paraequ.h>
#include    <para.h>

#include    "prog.h"
#include    "pepcomm.h"
#include    "pepflag.h"
#include    "p009.h"

/*
* ===========================================================================
*       Compiler Message Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Work Area Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Public Work Area Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Static Work Area Declarations
* ===========================================================================
*/
static LPP09STR lpStr;

//------------------------------------------------------
// If making changes to this table make sure that you also
// add the necessary defined constants and resource strings.
// For an example, do a search on IDS_P09_DSC_077 to find
// the various places needing a change.
//
// Entries used in this table should correspond with the address
// used in function MaintEntSup() of the terminal application
// when it is using a PARAACTCODESEC struct to obtain the security level
// for a particular Action Code (AC code):
//
//    ParaActCodeSec.uchMajorClass = CLASS_PARAACTCODESEC;
//    ParaActCodeSec.usAddress = pData->usSuperNo;
//    CliParaRead(&ParaActCodeSec);   /* call ParaActCodeSecRead() */
//
//
// WARNING:  The numbers in this table are the address for the
//           security setting.  They are not the AC or P number
//           for a particular command though in some cases the
//           action code or program menu number may be used.
//           The values in this table are used to calculate
//           the offset of the table ParaActCodeSec[] containing
//           the security information.
//
//           This means that the values must be no greater than
//           twice the value of defined constant MAX_ACSEC_SIZE.
//
//           The terminal application expacts a max of ACSC_ADR_MAX.
//
//           The offset calculated is the nibble offset not the
//           offset for a byte which is why the range is twice the
//           size of the ParaActCodeSec[] array (2 nibbles per byte).
//
//           New codes should be added AT THE END of the array.
//
//           Some Action Codes (AC codes) do not require a security level:
//             AC888, AC_CONFIG_INFO          -> security level of 0
//             AC999, AC_SHUTDOWN             -> security level of 0
//
//           Other Action Codes (AC codes) are the same as an existing code:
//             AC985, AC_FORCE_MAST           -> same as AC85, AC_DISCON_MAST
//             AC777, AC_SET_LAYOUT           -> same as AC63, AC_PLU_MAINT
//             AC276, AC_SET_DELAYED_BAL_OFF  -> same as AC275, AC_SET_DELAYED_BAL

struct  s_SecurityTableStruct {
	USHORT   usActionCode;
	USHORT   usSecurityAddress;
	ULONG    ulMnemonicId;
};

static struct s_SecurityTableStruct  mySecurityTable [] = {
    {AC_GCNO_ASSIN,           AC_GCNO_ASSIN,           IDS_P09_DSC_001},
    {AC_GCFL_READ_RPT,        AC_GCFL_READ_RPT,        IDS_P09_DSC_002},
    {AC_GCFL_RESET_RPT,       AC_GCFL_RESET_RPT,       IDS_P09_DSC_003},
    {AC_PLU_MENU,             AC_PLU_MENU,             IDS_P09_DSC_004},
    {AC_SET_CTLPG,            AC_SET_CTLPG,            IDS_P09_DSC_005},
    {AC_MANU_KITCH,           AC_MANU_KITCH,           IDS_P09_DSC_006},
    {AC_CASH_ABKEY,           AC_CASH_ABKEY,           IDS_P09_DSC_007},
    {AC_LOAN,                 AC_LOAN,                 IDS_P09_DSC_008},
    {AC_PICKUP,               AC_PICKUP,               IDS_P09_DSC_009},
    {AC_SUPSEC_NO,            AC_SUPSEC_NO,            IDS_P09_DSC_010},

    {AC_TOD,                  AC_TOD,                  IDS_P09_DSC_011},
    {AC_PTD_RPT,              AC_PTD_RPT,              IDS_P09_DSC_012},
    {AC_CASH_RESET_RPT,       AC_CASH_RESET_RPT,       IDS_P09_DSC_013},
    {AC_CASH_ASSIN,           AC_CASH_ASSIN,           IDS_P09_DSC_014},
    {AC_CASH_READ_RPT,        AC_CASH_READ_RPT,        IDS_P09_DSC_015},
    {AC_REGFIN_READ_RPT,      AC_REGFIN_READ_RPT,      IDS_P09_DSC_016},
    {AC_HOURLY_READ_RPT,      AC_HOURLY_READ_RPT,      IDS_P09_DSC_017},
    {AC_DEPTSALE_READ_RPT,    AC_DEPTSALE_READ_RPT,    IDS_P09_DSC_018},
    {AC_PLUSALE_READ_RPT,     AC_PLUSALE_READ_RPT,     IDS_P09_DSC_019},
    {AC_CPN_READ_RPT,         AC_CPN_READ_RPT,         IDS_P09_DSC_020},

    {AC_CSTR_MAINT,           AC_CSTR_MAINT,           IDS_P09_DSC_021},
    {AC_PLU_RESET_RPT,        AC_PLU_RESET_RPT,        IDS_P09_DSC_022},
    {AC_CPN_RESET_RPT,        AC_CPN_RESET_RPT,        IDS_P09_DSC_023},
    {AC_COPY_TTL,             AC_COPY_TTL,             IDS_P09_DSC_024},
    {AC_PLU_MAINT,            AC_PLU_MAINT,            IDS_P09_DSC_025},
    {AC_PLU_PRICE_CHG,        AC_PLU_PRICE_CHG,        IDS_P09_DSC_026},
    {AC_PLU_ADDDEL,           AC_PLU_ADDDEL,           IDS_P09_DSC_027},
    {AC_EJ_READ_RPT,          AC_EJ_READ_RPT,          IDS_P09_DSC_028},
    {AC_PPI_MAINT,            AC_PPI_MAINT,            IDS_P09_DSC_029},
    {AC_PLUPARA_READ_RPT,     AC_PLUPARA_READ_RPT,     IDS_P09_DSC_030},

    {AC_ALL_DOWN,             AC_ALL_DOWN,             IDS_P09_DSC_031},
    {AC_START_STOP_CPM,       AC_START_STOP_CPM,       IDS_P09_DSC_032},
    {AC_PLU_MNEMO_CHG,        AC_PLU_MNEMO_CHG,        IDS_P09_DSC_033},
    {AC_ROUND,                AC_ROUND,                IDS_P09_DSC_034},
    {AC_DISCON_MAST,          AC_DISCON_MAST,          IDS_P09_DSC_035},
    {AC_DISC_BONUS,           AC_DISC_BONUS,           IDS_P09_DSC_036},
    {AC_SOFTCHK_MSG,          AC_SOFTCHK_MSG,          IDS_P09_DSC_037},
    {AC_SALES_PROMOTION,      AC_SALES_PROMOTION,      IDS_P09_DSC_038},
    {AC_CURRENCY,             AC_CURRENCY,             IDS_P09_DSC_039},
    {AC_TALLY_CPM_EPT,        AC_TALLY_CPM_EPT,        IDS_P09_DSC_040},

    {AC_EOD_RPT,              AC_EOD_RPT,              IDS_P09_DSC_041},
    {AC_EJ_RESET_RPT,         AC_EJ_RESET_RPT,         IDS_P09_DSC_042},
    {AC_EMG_GCFL_CLS,         AC_EMG_GCFL_CLS,         IDS_P09_DSC_043},
    {AC_TARE_TABLE,           AC_TARE_TABLE,           IDS_P09_DSC_044},
    {AC_DEPT_MAINT,           AC_DEPT_MAINT,           IDS_P09_DSC_045},
    {AC_SET_PLU,              AC_SET_PLU,              IDS_P09_DSC_046},
    {AC_EMG_CASHFL_CLS,       AC_EMG_CASHFL_CLS,       IDS_P09_DSC_047},
    {AC_MAJDEPT_RPT,          AC_MAJDEPT_RPT,          IDS_P09_DSC_048},
    {AC_MEDIA_RPT,            AC_MEDIA_RPT,            IDS_P09_DSC_049},
    {AC_TAXTBL1,              AC_TAXTBL1,              IDS_P09_DSC_050},

    {AC_TAXTBL2,              AC_TAXTBL2,              IDS_P09_DSC_051},
    {AC_TAXTBL3,              AC_TAXTBL3,              IDS_P09_DSC_052},
    {AC_TAX_RATE,             AC_TAX_RATE,             IDS_P09_DSC_053},
    {AC_MISC_PARA,            AC_MISC_PARA,            IDS_P09_DSC_054},
    {AC_TAXTBL4,              AC_TAXTBL4,              IDS_P09_DSC_055},
    {AC_PIGRULE,              AC_PIGRULE,              IDS_P09_DSC_056},
    {AC_SERVICE_READ_RPT,     AC_SERVICE_READ_RPT,     IDS_P09_DSC_057},
    {AC_SERVICE_RESET_RPT,    AC_SERVICE_RESET_RPT,    IDS_P09_DSC_058},
    {AC_SERVICE_MAINT,        AC_SERVICE_MAINT,        IDS_P09_DSC_059},
    {AC_PROG_READ_RPT,        AC_PROG_READ_RPT,        IDS_P09_DSC_060},

    {AC_PROG_RESET_RPT,       AC_PROG_RESET_RPT,       IDS_P09_DSC_061},
    {AC_DISPENSER_PARA,       AC_DISPENSER_PARA,       IDS_P09_DSC_062},
    {AC_ETK_READ_RPT,         AC_ETK_READ_RPT,         IDS_P09_DSC_063},
    {AC_ETK_RESET_RPT,        AC_ETK_RESET_RPT,        IDS_P09_DSC_064},
    {AC_ETK_ASSIN,            AC_ETK_ASSIN,            IDS_P09_DSC_065},
    {AC_ETK_MAINT,            AC_ETK_MAINT,            IDS_P09_DSC_066},
    {AC_ETK_LABOR_COST,       AC_ETK_LABOR_COST,       IDS_P09_DSC_067},
    {AC_OEP_MAINT,            AC_OEP_MAINT,            IDS_P09_DSC_068},
    {AC_CPN_MAINT,            AC_CPN_MAINT,            IDS_P09_DSC_069},
    {AC_FLEX_DRIVE,           AC_FLEX_DRIVE,           IDS_P09_DSC_070},

    {AC_TONE_CTL,             AC_TONE_CTL,             IDS_P09_DSC_071},
    {AC_PLU_SLREST,           AC_PLU_SLREST,           IDS_P09_DSC_072},
    {AC_SUP_DOWN,             AC_SUP_DOWN,             IDS_P09_DSC_073},
    {AC_CSOPN_RPT,            AC_CSOPN_RPT,            IDS_P09_DSC_074},
    {AC_BOUNDARY_AGE,         AC_BOUNDARY_AGE,         IDS_P09_DSC_075},
    {AC_IND_READ_RPT,         AC_IND_READ_RPT,         IDS_P09_DSC_076},
    {AC_IND_RESET_RPT,        AC_IND_RESET_RPT,        IDS_P09_DSC_077},
    {AC_SET_DELAYED_BAL,      PARA_SEC_AC_SET_DELAYED_BAL, IDS_P09_DSC_078},
    {AC_STORE_FORWARD,        AC_STORE_FORWARD,        IDS_P09_DSC_079},
    {AC_JOIN_CLUSTER,         PARA_SEC_AC_JOIN_CLUSTER,    IDS_P09_DSC_080}
};

static WORD     awAddrTbl[P09_TOTALACTION] = {
      1,   2,   3,   4,   5,   6,   7,  10,  11,  15,
     17,  18,  19,  20,  21,  23,  24,  26,  29,  30,
     33,  39,  40,  42,  63,  64,  68,  70,  71,  72,
     75,  78,  82,  84,  85,  86,  87,  88,  89,  97,
     99, 100, 103, 111, 114, 116, 119, 122, 123, 124,
    125, 126, 127, 128, 129, 130, 131, 132, 133, 135,
    136, 137, 150, 151, 152, 153, 154, 160, 161, 162,
    AC_TONE_CTL,                              // AC169, AC_TONE_CTL        -> IDS_P09_DSC_071
	AC_PLU_SLREST,                            // AC170, AC_PLU_SLREST      -> IDS_P09_DSC_072
	AC_SUP_DOWN,                              // AC175, AC_SUP_DOWN        -> IDS_P09_DSC_073
	AC_CSOPN_RPT,                             // AC176, AC_CSOPN_RPT       -> IDS_P09_DSC_074
	AC_BOUNDARY_AGE,                          // AC208, AC_BOUNDARY_AGE    -> IDS_P09_DSC_075
	AC_IND_READ_RPT,                          // AC223, AC_IND_READ_RPT    -> IDS_P09_DSC_076
	AC_IND_RESET_RPT,                         // AC233, AC_IND_RESET_RPT   -> IDS_P09_DSC_077
	PARA_SEC_AC_SET_DELAYED_BAL,              // AC275, AC_SET_DELAYED_BAL -> IDS_P09_DSC_078
	AC_STORE_FORWARD,                         // AC102, AC_STORE_FORWARD   -> IDS_P09_DSC_079
	PARA_SEC_AC_JOIN_CLUSTER                  // AC444, AC_JOIN_CLUSTER    -> IDS_P09_DSC_080
};

static WORD     awAddrTblName[P09_TOTALACTION] = {
      1,   2,   3,   4,   5,   6,   7,  10,  11,  15,
     17,  18,  19,  20,  21,  23,  24,  26,  29,  30,
     33,  39,  40,  42,  63,  64,  68,  70,  71,  72,
     75,  78,  82,  84,  85,  86,  87,  88,  89,  97,
     99, 100, 103, 111, 114, 116, 119, 122, 123, 124,
    125, 126, 127, 128, 129, 130, 131, 132, 133, 135,
    136, 137, 150, 151, 152, 153, 154, 160, 161, 162,
    169, 170, 175, 176, 208, 223, 233,
	275,                                      // AC275, AC_SET_DELAYED_BAL -> IDS_P09_DSC_078 - PARA_SEC_AC_SET_DELAYED_BAL
	AC_STORE_FORWARD,                         // AC102, AC_STORE_FORWARD   -> IDS_P09_DSC_079
	444                                       // AC444, AC_JOIN_CLUSTER    -> IDS_P09_DSC_080 - PARA_SEC_AC_JOIN_CLUSTER
};
/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/


/*
* ===========================================================================
**  Synopsis:   BOOL    WINAPI  P009DlgProc()
*
*   Input   :   HWND    hDlg   - handle of a dialogbox procedure
*               WORD    wMsg   - dispacthed message
*               WORD    wParam - 16 bits message parameter
*               LONG    lParam - 32 bits message parameter
**
*   Return  :   TRUE           - user process is executed
*               FALSE          - default process is expected
*
**  Description:
*       This is a dialgbox procedure for the Action Code Security Maintenance.
* ===========================================================================
*/
BOOL    WINAPI  P009DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static WORD     wIndex;
    static HGLOBAL  hMem;
    static UCHAR    auchParaActCodeSec[MAX_ACSEC_SIZE];

    switch (wMsg) {
    case WM_INITDIALOG: 
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));
		/* ----- Initialize dialogbox ----- */
        wIndex = P09InitDlg(hDlg, auchParaActCodeSec, &hMem);
        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {
			TEXTMETRIC tm;
			HDC myHdc;

			int j;
			for(j=IDD_P09LISTBOX; j<=IDD_P09_LEVDESC; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);

			myHdc = GetDC (hDlg);
			GetTextMetrics (myHdc, &tm);
			ReleaseDC (hDlg, myHdc);
			SendDlgItemMessage(hDlg, IDD_P09LISTBOX, LB_SETITEMHEIGHT, 0, (tm.tmHeight + (tm.tmHeight >> 2)));
		}
		return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDD_P09LISTBOX:
            if (HIWORD(wParam) == LBN_SELCHANGE) {     /* set data to dialogbox */
                /* ----- Set selected item to maintenance box ----- */
                wIndex = P09SetMaintData(hDlg, auchParaActCodeSec);
                return TRUE;
            }
            return FALSE;

        case IDD_P09L00:
        case IDD_P09L01:
        case IDD_P09L10:
        case IDD_P09L11:
        case IDD_P09L20:
        case IDD_P09L21:
            /* ----- Get maintenanced item from maintenance box ----- */
            P09GetMaintData(hDlg, auchParaActCodeSec, wIndex);
            return TRUE;

        case IDOK:
        case IDCANCEL:
            /* ----- Finalize dialogbox ----- */
            P09FinDlg(hDlg, auchParaActCodeSec, hMem, wParam);
            return TRUE;
        }
        return FALSE;

    default:
        return FALSE;
    }
}

/*
* ===========================================================================
**  Synopsis:   WORD    P09InitDlg()
*                       
*   Input   :   HWND        hDlg    - handle of a dialogbox procedure
*               LPBYTE      lpbBuff - address of data buffer
*               LPHGLOBAL   lphMem  - handle of global memory for string
**
*   Return  :   WORD        wIdx    - index of cursor selected
*
**  Description:
*       This procedure initialize dialogbox.
* ===========================================================================
*/
WORD    P09InitDlg(HWND hDlg, LPBYTE lpbBuff, LPHGLOBAL lphMem)
{
    WORD    wI, wIdx;
	UINT    wID;
    USHORT  usReturnLen;
    WCHAR    szCap[PEP_CAPTION_LEN],
            szErr[PEP_ALLOC_LEN];

    /* ----- Read data from file ----- */
    ParaAllRead(CLASS_PARAACTCODESEC, (UCHAR *)lpbBuff, sizeof(UCHAR) * MAX_ACSEC_SIZE, 0, &usReturnLen);

    /* ----- Allocate memory from global area ----- */
    *lphMem = GlobalAlloc(GHND, sizeof(P09STR));

    /* ----- Check whether keep memory or not ----- */
    if (! *lphMem) {
        /* ----- Load String from Resource ----- */
        LoadString(hResourceDll, IDS_PEP_CAPTION_P09, szCap, PEP_STRING_LEN_MAC(szCap));
        LoadString(hResourceDll, IDS_PEP_ALLOC_ERR, szErr, PEP_STRING_LEN_MAC(szErr));

        /* ----- Display caution message ----- */
        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg, szErr, szCap, MB_OK | MB_ICONEXCLAMATION);

        /* ----- Destroy dialogbox ----- */
        EndDialog(hDlg, FALSE);

        return 0;
    }

    /* ----- Lock memory area ----- */
    lpStr = (LPP09STR)GlobalLock(*lphMem);

    /* ----- Load description from resource ----- */
    for (wI = 0, wID = IDS_P09_DSC_001; wI < P09_TOTALACTION; wI++, wID++) {
        LoadString(hResourceDll, wID, lpStr->aszDscrb[wI], P09_DSCRB_LEN);
    }

    /* ----- Set data to listbox ----- */
	P09SetListData(hDlg, lpbBuff);

    /* ----- Set cursor to listbox ----- */
    SendDlgItemMessage(hDlg, IDD_P09LISTBOX, LB_SETCURSEL, 0, 0L);

    /* ----- Set data to maintnance box ----- */
    wIdx = P09SetMaintData(hDlg, lpbBuff);

    return (wIdx);
}

/*
* ===========================================================================
**  Synopsis:   VOID    P09SetListData()
*                       
*   Input   :   HWND    hDlg    - handle of a dialogbox procedure
*               LPBYTE  lpbPara - address of data buffer
**
*   Return  :   None
*
**  Description:
*       This procedure set data to listbox.
* ===========================================================================
*/
VOID    P09SetListData(HWND hDlg, LPBYTE lpbPara)
{
    WORD    wI, wID, wOffset;
    BYTE    bWork;
    WCHAR   szWork[P09_LIST_LEN];
	WCHAR   szIndicatorArea[4], szIndicators[2] = {_T(' '), _T('$')};

    /* ----- Set data into the listbox routine ----- */
    SendDlgItemMessage(hDlg, IDD_P09LISTBOX, WM_SETFONT, (WPARAM)hResourceFont, 0);
	memset (szIndicatorArea, 0, sizeof(szIndicatorArea));

	for (wI = 0, wID = IDS_P09_DSC_001; wI < P09_TOTALACTION; wI++) {
        /* ----- Set offset of address ----- */
        wOffset = (WORD)((awAddrTbl[wI] - 1) / 2);

        /* ----- Odd addresses in high order bits and Even addresses in low order bits ----- */
        if ((awAddrTbl[wI] & 1) == 0) {
            bWork = (*(lpbPara + wOffset) & 0x0f);
        } else {
            bWork = (*(lpbPara + wOffset) & 0xf0);
			bWork >>= 4;
        }

		szIndicatorArea[2] = ((bWork & P09_CHKBIT0) != 0) ? szIndicators[0] : szIndicators[1];
		szIndicatorArea[1] = ((bWork & P09_CHKBIT1) != 0) ? szIndicators[0] : szIndicators[1];
		szIndicatorArea[0] = ((bWork & P09_CHKBIT2) != 0) ? szIndicators[0] : szIndicators[1];

        /* ----- Make mnemonic for setting ----- */
        wsPepf(szWork, WIDE("%s  %03u: %s"), szIndicatorArea, awAddrTblName[wI], lpStr->aszDscrb[wI]);

        /* ----- Set mnemonic into the listbox ----- */
		DlgItemSendTextMessage (hDlg, IDD_P09LISTBOX, LB_INSERTSTRING, (WPARAM)-1,(LPARAM)(szWork));
    }
}

/*
* ===========================================================================
**  Synopsis:   WORD    P09SetMaintData()
*                       
*   Input   :   HWND    hDlg    - handle of a dialogbox procedure
*               LPBYTE  lpbPara - address of data buffer
**
*   Return  :   WORD    wIdx    - offset of selected item
*
**  Description:
*       This procedure set individual data to maintenance box .
* ===========================================================================
*/
WORD    P09SetMaintData(HWND hDlg, LPBYTE lpbPara)
{
    BYTE    bWork;
    WORD    wIDON, wIDOFF, wIdx, wOffset;
    WCHAR   szWork[P09_ADR_LEN], szAddress[P09_ADR_LEN];

    /* ----- Get selected address from listbox ----- */
    wIdx = (WORD)SendDlgItemMessage(hDlg, IDD_P09LISTBOX, LB_GETCURSEL, 0, 0L);

    /* ----- Set title of selected address ----- */ 
    LoadString(hResourceDll, IDS_P09_ADDR, szAddress, PEP_STRING_LEN_MAC(szAddress));

    wsPepf(szWork, szAddress, awAddrTblName[wIdx]);

    DlgItemRedrawText(hDlg, IDD_P09GROUP, szWork);

    /* ----- Set offset of address ----- */
    wOffset = (WORD)((awAddrTbl[wIdx] - 1) / 2);

    /* ----- Odd addresses in high order bits and Even addresses in low order bits ----- */
    if ((awAddrTbl[wIdx] & 1) == 0) {
        bWork = (*(lpbPara + wOffset) & 0x0f);
    } else {
        bWork = (*(lpbPara + wOffset) & 0xf0);
		bWork >>= 4;
    }

    /* ----- Calculate data for setting ----- */
    if ((bWork & P09_CHKBIT0) != 0) {
        wIDON = IDD_P09L01;     // Prohibit
        wIDOFF = IDD_P09L00;
    } else {
        wIDON = IDD_P09L00;     // Allow
        wIDOFF = IDD_P09L01;
    }

    /* ----- Set data to level-0 checkbox ----- */
    SendDlgItemMessage(hDlg, wIDON, BM_SETCHECK, TRUE, 0L);
    SendDlgItemMessage(hDlg, wIDOFF, BM_SETCHECK, FALSE, 0L);

    /* ----- Calculate data for setting ----- */
    if ((bWork & P09_CHKBIT1) != 0) {
        wIDON = IDD_P09L11;     // Prohibit
        wIDOFF = IDD_P09L10;
    } else {
        wIDON = IDD_P09L10;     // Allow
        wIDOFF = IDD_P09L11;
    }

    /* ----- Set data to level-1 checkbox ----- */
    SendDlgItemMessage(hDlg, wIDON, BM_SETCHECK, TRUE, 0L);
    SendDlgItemMessage(hDlg, wIDOFF, BM_SETCHECK, FALSE, 0L);

    /* ----- Calculate data for setting ----- */
    if ((bWork & P09_CHKBIT2) != 0) {
        wIDON = IDD_P09L21;     // Prohibit
        wIDOFF = IDD_P09L20;
    } else {
        wIDON = IDD_P09L20;     // Allow
        wIDOFF = IDD_P09L21;
    }

    /* ----- Set data to level-2 checkbox ----- */
    SendDlgItemMessage(hDlg, wIDON, BM_SETCHECK, TRUE, 0L);
    SendDlgItemMessage(hDlg, wIDOFF, BM_SETCHECK, FALSE, 0L);

    return (wIdx);
}

/*
* ===========================================================================
**  Synopsis:   VOID    P09GetMaintData()
*
*   Input   :   HWND    hDlg    - handle of a dialogbox procedure
*               LPBYTE  lpbPara - address of data buffer
*               WORD    wIdx    - offset of selected item
**
*   Return  :   None
*
**  Description:
*       This procedure get individual data from maintenance box .
* ===========================================================================
*/
VOID    P09GetMaintData(HWND hDlg, LPBYTE lpbPara, WPARAM wIdx)
{
    BYTE    bWork, bWorkMask;
    WORD    wOffset;
    WCHAR   szWork[P09_LIST_LEN];
	WCHAR   szIndicatorArea[4], szIndicators[2] = {_T(' '), _T('$')};

	memset (szIndicatorArea, 0, sizeof(szIndicatorArea));

    /* ----- Get user's selection and store them temporary buffer ----- */
    bWork = 0;
    if (SendDlgItemMessage(hDlg, IDD_P09L01, BM_GETCHECK, 0, 0L)) 
	{
        bWork |= P09_CHKBIT0;
    }
    if (SendDlgItemMessage(hDlg, IDD_P09L11, BM_GETCHECK, 0, 0L)) {
        bWork |= P09_CHKBIT1;
    }
    if (SendDlgItemMessage(hDlg, IDD_P09L21, BM_GETCHECK, 0, 0L)) {
        bWork |= P09_CHKBIT2;
    }

    /* ----- Make mnemonic for setting ----- */
	szIndicatorArea[2] = ((bWork & P09_CHKBIT0) != 0) ? szIndicators[0] : szIndicators[1];
	szIndicatorArea[1] = ((bWork & P09_CHKBIT1) != 0) ? szIndicators[0] : szIndicators[1];
	szIndicatorArea[0] = ((bWork & P09_CHKBIT2) != 0) ? szIndicators[0] : szIndicators[1];

    /* ----- Odd addresses in high order bits and Even addresses in low order bits ----- */
	bWorkMask = 0xf0;
    if ((awAddrTbl[wIdx] & 1) != 0) {
		bWork <<= 4;         // shift the bits to the high order
		bWorkMask = 0x0f;    // set mask to keep the low order bits
    }

    /* ----- Set offset of address ----- */
    wOffset = (WORD)((awAddrTbl[wIdx] - 1) / 2);

	/* ----- Set data to the buffer ----- */
	*(lpbPara + wOffset) &= bWorkMask;     // Remove the current settings for this action code
	*(lpbPara + wOffset) |= bWork;         // Put in our new settings for this action code

    /* ----- Delete previous data in the listbox ----- */
    SendDlgItemMessage(hDlg,IDD_P09LISTBOX,LB_DELETESTRING,(WPARAM)wIdx, 0L);

    wsPepf(szWork, WIDE("%s  %03u: %s"), szIndicatorArea, awAddrTblName[wIdx], lpStr->aszDscrb[wIdx]);

    /* ----- Set data into the listbox ----- */
	DlgItemSendTextMessage (hDlg,IDD_P09LISTBOX,LB_INSERTSTRING,(WPARAM)wIdx,(LPARAM)(szWork));

    /* ----- Set cursor selected ----- */
    SendDlgItemMessage(hDlg,IDD_P09LISTBOX,LB_SETCURSEL,(WPARAM)wIdx,0L);
}

/*
* ===========================================================================
**  Synopsis:   VOID    P09FinDlg()
*                       
*   Input   :   HWND    hDlg    - handle of a dialogbox procedure
*               LPBYTE  lpbBuff - address of data buffer
*               HGLOBAL hMem    - handle of global memory for string
*               WORD    wID     - parameter of dialogbox ID
**
*   Return  :   None
*
**  Description:
*       This procedure finalize dialogbox.
* ===========================================================================
*/
VOID    P09FinDlg(HWND hDlg, LPBYTE lpbBuff, HGLOBAL hMem, WPARAM wID)
{
    USHORT  usReturnLen;

    if (LOWORD(wID) == IDOK) {
        /* ----- Write data to file ----- */
        ParaAllWrite(CLASS_PARAACTCODESEC, (UCHAR *)lpbBuff, sizeof(UCHAR) * MAX_ACSEC_SIZE, 0, &usReturnLen);

        /* ----- Set file status flag ----- */
        PepSetModFlag(hwndProgMain, PEP_MF_PROG, 0);
    }

    /* ----- Free memory area ----- */
    GlobalUnlock(hMem);
    GlobalFree(hMem);

    /* ----- Destroy dialogbox ----- */
    EndDialog(hDlg, FALSE);
}

/* ===== End of P009.C ===== */
