/*
* ---------------------------------------------------------------------------
* Title         :   Set PLU Nu. on Keyboard (AC 4)
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A004.C
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
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
* Apr-20-95 : 03.00.00 : H.Ishida   : Add shift page of FSC
* Mar-02-96 : 03.00.05 : M.Suzuki   : Chg R3.1
* Jan-23-98 : 01.00.08 : M.Ozawa    : Add A004ConvPluKey() for Correct PLU Key Data
* Sep-10-98 : 03.03.00 : A.Mitsui   : V3.3
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
#include    <rfl.h>    /* Saratoga */

#include    "action.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "file.h"
#include    "a004.h"

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
static  HGLOBAL     hMemFsc,        /* Handle of Global Heap (FSC)      */
                    hMemPluMenu,    /* Handle of Global Heap (PLU Key)  */
                    hMemConvPluMenu;/* Handle of Global Heap (PLU Key), 1/23/98 */
static  LPA04FSC    lpFscData;      /* Address of Data Buffer (FSC)     */
static  LPA04DATA   lpPluData;      /* Address of Data Buffer (PLU Key) */
static  LPA04DATA   lpConvPluData;  /* Address of Data Buffer (PLU Key), 1/23/98 */
static  WORD        wPageNo,        /* Current Selected Menu Page No.   */
                    wBeforePage,    /* Before Selected Page No.         */
                    wPluKeyNo;      /* Current Inputed Keyed PLU Key No */
static  BOOL        fConvPluData;   /* Conversion Save Flag, 1/23/98    */

/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  A004DlgProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*                   WORD    wMsg    -   Dispacthed Message
*                   WORD    wParam  -   16 Bits of Additional Message
*                   LONG    lParam  -   32 Bits of Additional Message
*
**  Return      :   BOOL    TRUE    -   The Procedure Processes the Message.
*                           FALSE   -   It Does Not Process the Message.
*
**  Description :
*       This is a dialogbox procedure for 
*                   Set PLU No. on Keyboard (Action Code # 04)
* ===========================================================================
*/
BOOL    WINAPI  A004DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  LPCFCONFIG  lpFile;         /* Address of File Config.  */
    static  PEPSPIN     PepSpin;        /* Structure of SpinButton  */
    static  WORD        wPluKeyMax;     /* Max. of Keyed PLU Key No */

    USHORT  usRetLen;
    UINT    unPluKey;
    WORD    wPage;
    UINT    wI;

    short   nEdit = 0;
    WCHAR    szWork[16];
    BOOL    fRet;                       /* Saratoga */

	WCHAR    szErr[A04_ERR_LEN + 1];         /* Saratoga */
    WCHAR    szMsg[A04_ERR_LEN + 1];         /* Saratoga */
    WCHAR    szCap[A04_CAP_LEN + 1];         /* Saratoga */

    switch (wMsg) {

    case    WM_INITDIALOG:

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        A004InitDlg(hDlg);
        
                /* ----- Disable Main PEP Menu ----- */
            
        SendMessage(hwndActMain, PM_PEPMENU, (WPARAM)PEP_MENU_ALL_OFF, 0L);

        /* ----- Limit Maximum Length of EditText Data ----- */

        SendDlgItemMessage(hDlg, IDD_A04_PLUKEY,
                           EM_LIMITTEXT, (WPARAM)A04_KEYNO_LEN, 0L);

        SendDlgItemMessage(hDlg, IDD_A04_PLUNO,
                           EM_LIMITTEXT, (WPARAM)A04_NO_LEN, 0L);

        /* ----- disable ADD&CHG, DEL button ----- */
	
        EnableWindow(GetDlgItem(hDlg, IDD_A04_ADDCHG), FALSE);     /* Saratoga */
        EnableWindow(GetDlgItem(hDlg, IDD_A04_DEL),    FALSE);     /* Saratoga */

        /* ----- Allocate Data Buffer from Global Heap ----- */

        hMemPluMenu = GlobalAlloc(GHND, MAX_PAGE_NO * FSC_PLU_MAX * sizeof(A04PLU));

        if (! hMemPluMenu) {    /* Failed Allocate Memory */

            A004ExecAbort(hDlg, A04_MEM_ERROR);

        } else {                /* Success Allocate Memory */

            lpPluData = (LPA04DATA)GlobalLock(hMemPluMenu);

            /* ----- Read Initial AC 4 Data ----- */

            ParaAllRead(CLASS_PARAPLUNOMENU,
                        (UCHAR *)lpPluData,
                        (MAX_PAGE_NO * FSC_PLU_MICROMAX * sizeof(A04PLU)),
                        0,
                        &usRetLen);

            /* ----- Insert Page No. to ComboBox ----- */

            for (wI = 0; wI < A04_PAGE_MAX; wI++) {

                wsPepf(szWork, WIDE("%u"), wI + A04_DEF_PAGENO);
                DlgItemSendTextMessage(hDlg,
                                   IDD_A04_PAGE,
                                   CB_INSERTSTRING,
                                   (WPARAM)-1,
                                   (LPARAM)(szWork));
            }

            SendDlgItemMessage(hDlg, IDD_A04_PAGE, CB_SETCURSEL, 0, 0L);

            /* ----- Get Current KeyBoard Type ----- */
			// RJC: no need to send messages anymore, DLLs modified to
			// static libraries as a part of 32 bit conversion.
			// Messaging pointers between DLL and EXE will not work under
			// Win32 anyway as are in separate address spaces.
			lpFile = FileConfigGet();

            if (lpFile->chKBType == FILE_KB_CONVENTION) {   /* NCR 7448 with Conventional Keyboard  */

                wPluKeyMax = A04_PLUKEY_068;

            } else if (lpFile->chKBType == FILE_KB_MICRO) {  /* NCR 7448 with Micromotion Keyboard  */

                wPluKeyMax = A04_PLUKEY_165;

            } else if (lpFile->chKBType == FILE_KB_WEDGE_68) {       /* NCR Wedge 64 key keyboard  */

                wPluKeyMax = A04_PLUKEY_055;

            } else if (lpFile->chKBType == FILE_KB_WEDGE_78) {       /* NCR Wedge 78 key keyboard  */

                wPluKeyMax = A04_PLUKEY_068;

            } else {
                __debugbreak();  // prep for ARM build _asm int 3;
			}   // breakpoint out.

            /* ----- Set Maximum Value of Keyed PLU No. ----- */

            wsPepf(szWork, WIDE("(1 - %hu)"), wPluKeyMax);

            DlgItemRedrawText(hDlg, IDD_A04_KEYMAX, szWork);

            /* ----- Initialize Common SpinButton Configulation ----- */

            PepSpin.nStep      = A04_SPIN_STEP;
            PepSpin.fSpinStyle = PEP_SPIN_NOROLL;
/* Saratoga Start */
            /* ----- Set maintenance type to combo-box ----- */

            for( wI = 0; wI < A04_TYPE_MAX; wI++) {
                 LoadString(hResourceDll, IDS_A04_TYPE_NORMAL + wI, szWork, PEP_STRING_LEN_MAC(szWork));

                DlgItemSendTextMessage(hDlg, IDD_A04_LABEL, CB_INSERTSTRING, (WPARAM)(-1), (LPARAM)(szWork));
            }

            SendDlgItemMessage(hDlg, IDD_A04_LABEL, CB_SETCURSEL, 0, 0L);

/* Saratoga End */

            }        
        
        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=IDD_A04_PAGE; j<=IDD_A04_GROUPBOX1; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_DESTROY:

        SendMessage(hwndActMain, PM_PEPMENU, (WPARAM)PEP_MENU_ALL_ON, 0L);
        return FALSE;

#ifdef junkyjunk_32bitconversion
    case PM_GET_FCONF:

        /* ----- Receive Address of File Configulation from PEP:Main ----- */

        lpFile = (LPCFCONFIG)lParam;
        
        return TRUE;
#endif

    case    PM_KEYENT:      /* Send from Key DialogBox */

        /* ----- Set current Page ----- */

        if (wPageNo) {

            wPage = (WORD)(wPageNo - A04_DEF_PAGENO);

        } else {

            wPage = 0;

        }

        /* ----- Check Current Selected Key is Keyed PLU or Not ----- */

        unPluKey = (UINT)lpFscData->Fsc[wPage].TblFsc[wParam-1].uchFsc;

        if (unPluKey == A04_PLUKEY) {   /* Key is Keyed PLU Key */

            /* ----- Get Extended FSC Data with Current Key ----- */

            wPluKeyNo = (WORD)lpFscData->Fsc[wPage].TblFsc[wParam-1].uchExt;

            /* ----- Set Extended PLU Key Data to  EditText ----- */

            SetDlgItemInt(hDlg, IDD_A04_PLUKEY, wPluKeyNo, FALSE);

            /* ----- Set PLU No. of Selected Keyed PLU Key ----- */

            A004SetPluNo(hDlg, lpPluData);

        }
        return TRUE;

    case WM_VSCROLL:

        /* ----- Set Configlation of EditText ----- */

        if (GetDlgCtrlID((HWND)lParam) == IDD_A04_KEYSPN) {

            nEdit            = IDD_A04_PLUKEY;
            PepSpin.lMax     = (long)wPluKeyMax;
            PepSpin.lMin     = (long)A04_PLUKEY_MIN;
            PepSpin.nTurbo   = 0;
        }
            /* ----- Common Spin Button Procedure ----- */

        PepSpinProc(hDlg, wParam, nEdit, (LPPEPSPIN)&PepSpin);

        return FALSE;

    case    WM_COMMAND:

        switch (LOWORD(wParam)) {

        case    IDD_A04_PLUNO:

            if (HIWORD(wParam) == EN_CHANGE) {

                /* ----- Get Inputed Data and Set them to Buffer ----- */
 
               /*A004SavePluData(hDlg, wParam);*/
                 

/*** Saratoga ***/
                /* ----- check all edit controls ----- */

                A004CtrlMaintButton(hDlg);

                return TRUE;
            }
            return FALSE;

        case    IDD_A04_PAGE:

            if (HIWORD(wParam) == CBN_SELCHANGE) {

                /* ----- Set Before Selected Page No. ----- */

                wBeforePage = wPageNo;

                /* ----- Get Menu Page No. from ComboBox ----- */

                wPageNo = (WORD)(A04_DEF_PAGENO + SendDlgItemMessage(hDlg, IDD_A04_PAGE,
                                                                     CB_GETCURSEL,
                                                                     0, 0L));

                /* ----- Clear Text to Specified Button ----- */

                szWork[0] = 0;

                DlgItemRedrawText(hDlg, IDD_A04_PLUKEY, szWork);

                DlgItemRedrawText(hDlg, IDD_A04_PLUNO, szWork);

                wPluKeyNo = 0;

                /* ----- Send Selected Button ID to Main DialogBox ----- */

                PostMessage(*lphActDlgChk2, PM_PAGE_DSP, wParam, 0L);

                return TRUE;
            }
            return FALSE;

        case    IDD_A04_PLUKEY:

            if (HIWORD(wParam) == EN_CHANGE) {

                /* ----- Check Data Range ----- */

                A004ChkRng(hDlg, LOWORD(wParam), A04_PLUKEY_MIN, wPluKeyMax);

                /* ----- Set PLU No. of Inputed Keyed PLU Key ----- */

                A004SetPluNo(hDlg, lpPluData);
                                                                            
                return TRUE;
            }
            return FALSE;

/* Saratoga */

        case    IDD_A04_ADDCHG:
        case    IDD_A04_DEL:

            /* ----- Numeric All Zero Check ----- */

            fRet = A004ZeroChk(hDlg);

            if (fRet == FALSE) {

                memset(szCap, 0x00, sizeof(szCap));
                memset(szMsg, 0x00, sizeof(szMsg));
                memset(szErr, 0x00, sizeof(szErr));

                /* ----- Load Error Message from Resource ----- */

                LoadString(hResourceDll, IDS_PEP_CAPTION_A04, szCap, PEP_STRING_LEN_MAC(szCap));
                LoadString(hResourceDll, IDS_A04_OVERRANGE, szErr, PEP_STRING_LEN_MAC(szErr));

                /* ----- Make Error Message with Min./Max. Value ----- */

                wsPepf(szMsg, szErr, A04_PLUNO_MIN, A04_PLUNO_MAX);

                /* ----- Display Error Message  ----- */

                MessageBeep(MB_ICONEXCLAMATION);
                MessageBoxPopUp(hDlg, szMsg,
                           szCap, MB_OK | MB_ICONEXCLAMATION);

                /* ----- Set High-light to EditText ----- */

                SendDlgItemMessage(hDlg, IDD_A04_PLUNO, EM_SETSEL, 0, MAKELONG(0, -1));

                /* ----- Set Focus to EditText ----- */

                SetFocus(GetDlgItem(hDlg, IDD_A04_PLUNO));

                return FALSE;

            }

            /* ----- get & save PLU No. ----- */

            A004SavePluData(hDlg, wParam);

            EnableWindow(GetDlgItem(hDlg, IDD_A04_ADDCHG), FALSE);  /* Saratoga */
            EnableWindow(GetDlgItem(hDlg, IDD_A04_DEL),    FALSE);  /* Saratoga */

            return TRUE;

        case    IDOK:
        case    IDCANCEL:

            if (HIWORD(wParam) == BN_CLICKED) {

                if (LOWORD(wParam) == IDOK) {

                     /* ----- Write Inputed Data to Parameter File ----- */

                    ParaAllWrite(CLASS_PARAPLUNOMENU,
                                 (UCHAR *)lpPluData,
                                 (USHORT)(MAX_PAGE_NO * FSC_PLU_MICROMAX * sizeof(A04PLU)),
                                 0,
                                 &usRetLen);

                    /* ----- Write FSC Data (Prog.#3) for Conversion, 1/23/98 ----- */

                    if (fConvPluData) { /* if conversion is done */

                        ParaAllWrite(CLASS_PARAFSC,
                                    (UCHAR *)lpFscData,
                                    sizeof(A04FSC),
                                    0,
                                    &usRetLen);

                        /* ----- Set File Modify Flag ----- */

                        PepSetModFlag(hwndActMain, PEP_MF_PROG, 0);
                    }

                    /* ----- Set File Modify Flag ----- */

                    PepSetModFlag(hwndActMain, PEP_MF_ACT, 0);
                }

                /* ----- Release Allocated Area ----- */

                GlobalUnlock(hMemFsc);
                GlobalUnlock(hMemPluMenu);
                GlobalFree(hMemFsc);
                GlobalFree(hMemPluMenu);

                /* ----- End Dialog Procedures ----- */

                A004ExecAbort(hDlg, A04_NORMAL_END);

                return TRUE;
            }
        }

    default:
        return FALSE;
    }
}


/*
* ===========================================================================
**  Synopsis    :   BOOL    A004ZeroChk()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*
**  Return      :   TRUE or FALSE.
*
**  Description :
*
* ===========================================================================
*/
BOOL A004ZeroChk(HWND hDlg)
{
    BOOL  fPLU;
    UINT  unPluNo;
    WCHAR  szPluNo[PLU_MAX_DIGIT + 1];
    short nZeroCnt, nNumCnt;
    short nI;


    /* ----- Get PLU No ----- */

    memset(szPluNo, 0x00, sizeof(szPluNo));
    unPluNo = DlgItemGetText(hDlg, IDD_A04_PLUNO, szPluNo, sizeof(szPluNo));

    nZeroCnt = nNumCnt = 0;

    for (nI = 0; nI < PLU_MAX_DIGIT; nI++) {

        if (szPluNo[nI] == 0x00)
            continue;
        else if (szPluNo[nI] == '0')
            nZeroCnt++;
        else if ((szPluNo[nI] >= '1') && (szPluNo[nI] <= '9'))
            nNumCnt++;
    }

    if ((nNumCnt == 0) || (nZeroCnt == PLU_MAX_DIGIT))
        fPLU = FALSE;
    else
        fPLU = TRUE;

    return fPLU;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A004InitDlg()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function loads the initial data from Parameter file, and then it
*   sets them to each controls.
* ===========================================================================
*/
VOID    A004InitDlg(HWND hDlg)
{
    WCHAR    szDesc[PEP_LOADSTRING_LEN + 1];

    /* ----- set initial description, V3.3 ----- */

	LoadString(hResourceDll, IDS_A04_CAPTION, szDesc, PEP_STRING_LEN_MAC(szDesc));
    WindowRedrawText(hDlg, szDesc);
	
	LoadString(hResourceDll, IDS_A04_PAGE, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_A04_STRPAGE, szDesc);
	LoadString(hResourceDll, IDS_A04_PAGELEN, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_A04_STRPAGELEN, szDesc);
	LoadString(hResourceDll, IDS_A04_PLUKEY, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_A04_STRPLUKEY, szDesc);
	LoadString(hResourceDll, IDS_A04_PLUNO, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_A04_STRPLUNO, szDesc);
	LoadString(hResourceDll, IDS_A04_PLUNOLEN, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_A04_STRPLUNOLEN, szDesc);
	LoadString(hResourceDll, IDS_A04_DESC, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_A04_STRDESC, szDesc);
	LoadString(hResourceDll, IDS_A04_LABEL, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_A04_STRDESC2, szDesc);
}

/*
* ===========================================================================
**  Synopsis:   static  VOID    CtrlMaintButton()
*
*   Input   :   HWND    hDlg - handle of a dialogbox
**
*   Return  :   None.
*
**  Description:
*       This procedure checkes edit control data. If each edit controls is
*      filled with an effective data, it enables ADD/CHANGE & DELETE buttons.
* ===========================================================================
*/
VOID    A004CtrlMaintButton(HWND hDlg)
{
    WORD    wVal;
    BOOL    fCtl;

    wVal = (WORD)GetDlgItemInt(hDlg, IDD_A04_PLUKEY, NULL, FALSE);
    wVal *= (WORD)ChkPluNo(hDlg);
    if (wVal != 0) {
        fCtl = TRUE;
    } else {
        fCtl = FALSE;
    }

    EnableWindow(GetDlgItem(hDlg, IDD_A04_ADDCHG), fCtl);
    EnableWindow(GetDlgItem(hDlg, IDD_A04_DEL),    fCtl);
}
/* ===========================================================================
**  Synopsis:   BOOL    ChkPluNo()
*
*   Input   :   HWND    hDlg - handle of a dialogbox
**
*   Return  :   TRUE         - PLU No. is entered
*               FALSE        - PLU No. is invalid
*
**  Description:    This procedure check input PLU No.
* ===========================================================================
*/
BOOL    ChkPluNo(HWND hDlg)
{
    WCHAR    szWork[A04_BUFF_LEN];
    UINT    unNum;
    DWORD   dwLen;
    BOOL    fChk = TRUE;
    WORD    wI;
    WCHAR    szPluNo[PLU_MAX_DIGIT + 1];

    memset(szPluNo, 0x00, sizeof(szPluNo));

    /* ----- Get PLU No. length ----- */

    dwLen = DlgItemSendTextMessage(hDlg, IDD_A04_PLUNO, WM_GETTEXTLENGTH, 0, 0L);

    /* ----- Check PLU No. ----- */

    memset(szWork, 0, sizeof(szWork));
    DlgItemGetText(hDlg,
                   IDD_A04_PLUNO,
                   szWork,
                   PLU_MAX_DIGIT + 1);
    for (wI = 0; wI < PLU_MAX_DIGIT + 1; wI++) {
        if (szWork[wI] == 0) {
            break;
        } else if ((szWork[wI] < '0') ||
                   (szWork[wI] > '9')) {
            fChk = FALSE;
            break;
        }
    }

    /* ----- Get PLU No. ----- */
/*
    unNum = GetDlgItemInt(hDlg, IDD_A04_PLUNO, &fTrans, FALSE);
*/
    unNum = DlgItemGetText(hDlg, IDD_A04_PLUNO, szPluNo, sizeof(szPluNo));
    szPluNo[PLU_MAX_DIGIT] = 0x00;

    if (dwLen == 0L) {
        return FALSE;
    } else if (fChk == FALSE) {
        return FALSE;
    } else {
        return TRUE;
    }
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  A004KeyDlgProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*                   WORD    wMsg    -   Dispacthed Message
*                   WORD    wParam  -   16 Bits of Additional Message
*                   LONG    lParam  -   32 Bits of Additional Message
*
**  Return      :   BOOL    TRUE    -   The Procedure Processes the Message.
*                           FALSE   -   It Does Not Process the Message.
*
**  Description :
*       This is a dialogbox procedure for Keyed PLU Keyboard.
* ===========================================================================
*/
BOOL    WINAPI  A004KeyDlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    USHORT  usRetLen;
    HCURSOR hCursor;
    WCHAR    szDesc[PEP_LOADSTRING_LEN + 1];


    switch (wMsg) {

    case    WM_INITDIALOG:

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- Initialize Area ----- */

        wPageNo = 0;
        wBeforePage = 0;
        wPluKeyNo = 0;
        
		LoadString(hResourceDll, IDS_A04_FIXCLR, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A04_FIXCLR, szDesc);
		LoadString(hResourceDll, IDS_A04_FIX00, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A04_FIX00, szDesc);
		LoadString(hResourceDll, IDS_A04_FIX01, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A04_FIX01, szDesc);
		LoadString(hResourceDll, IDS_A04_FIX02, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A04_FIX02, szDesc);
		LoadString(hResourceDll, IDS_A04_FIX03, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A04_FIX03, szDesc);
		LoadString(hResourceDll, IDS_A04_FIX04, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A04_FIX04, szDesc);
		LoadString(hResourceDll, IDS_A04_FIX05, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A04_FIX05, szDesc);
		LoadString(hResourceDll, IDS_A04_FIX06, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A04_FIX06, szDesc);
		LoadString(hResourceDll, IDS_A04_FIX07, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A04_FIX07, szDesc);
		LoadString(hResourceDll, IDS_A04_FIX08, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A04_FIX08, szDesc);
		LoadString(hResourceDll, IDS_A04_FIX09, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_A04_FIX09, szDesc);


        /* ----- Allocate Data Buffer from Global Heap ----- */

        hMemFsc = GlobalAlloc(GHND, sizeof(A04FSC));

        if (! hMemFsc) {    /* Failed to Allocate Memory */

            A004ExecAbort(hDlg, A04_MEM_ERROR);

        } else {            /* Success to Allocate Memory */

            /* ----- Lock Allocated Area ----- */

            lpFscData = (LPA04FSC)GlobalLock(hMemFsc);

            /* ----- Delete "CLOSE" Menu from a System Menu ----- */

            DeleteMenu(GetSystemMenu(hDlg, FALSE), SC_CLOSE, MF_BYCOMMAND);

            /* ----- Change Cursor (Arrow => Hour-Glass) ----- */

            hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
            ShowCursor(TRUE);

            /* ----- Read FSC Data (Prog.#3) from Parameter File ----- */

            ParaAllRead(CLASS_PARAFSC,
                        (UCHAR *)lpFscData,
                        sizeof(A04FSC),
                        0,
                        &usRetLen);

            /* ----- Correct Keyed PLU Key Position, 1/23/98  ----- */

            A004ConvPluKey(hDlg, lpFscData);

            /* ----- Set Keyed PLU Key to Specified Button Position ----- */

            A004SetPluKey(hDlg, lpFscData);

            /* ----- Restore Cursor (Hour-Glass => Arrow) ----- */

            ShowCursor(FALSE);
            SetCursor(hCursor);
        }
        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=IDD_A04_KEY01; j<=IDD_A04_KEY165; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			for(j=IDD_A04_FIXCLR; j<=IDD_A04_FIX09; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDD_A04_FIXED, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case    WM_COMMAND:

        switch (LOWORD(wParam)) {

        case    IDD_A04_FIXED:  /* Numeric Key is Pressed */
        case    IDD_A04_FIXCLR:
        case    IDD_A04_FIX00:
        case    IDD_A04_FIX01:
        case    IDD_A04_FIX02:
        case    IDD_A04_FIX03:
        case    IDD_A04_FIX04:
        case    IDD_A04_FIX05:
        case    IDD_A04_FIX06:
        case    IDD_A04_FIX07:
        case    IDD_A04_FIX08:
        case    IDD_A04_FIX09:

            if (HIWORD(wParam) == BN_CLICKED) {

                /* ----- Set Active Main DialogBox ----- */

                SetActiveWindow(*lphActDlgChk1);

                return TRUE;
            }
            return FALSE;

        default:

            /* ----- Send Selected Button ID to Main DialogBox ----- */

            PostMessage(*lphActDlgChk1, PM_KEYENT, wParam, 0L);
        }

        /* ----- Set Active Main DialogBox ----- */

        SetActiveWindow(*lphActDlgChk1);

        return TRUE;

    case    PM_PAGE_DSP:

        /* ----- Set Keyed PLU Key to Specified Button Position ----- */

        A004SetPluKey(hDlg, lpFscData);

        return TRUE;


    default:
        return FALSE;
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A004SetPluKey()
*
**  Input       :   HWND        hDlg    -   Window Handle of a DialogBox
*                   LPA04FSC    lpFsc   -   Address of FSC Data Buffer
*
**  Return      :   No return value.
*
**  Description :
*       This procedure sets the keyed PLU keys to the specified button on
*   the sub-dialogbox.
* ===========================================================================
*/
VOID    A004SetPluKey(HWND hDlg, LPA04FSC lpFsc)
{
    UINT    unID,
            unPluKey;
    WORD    wI;
    WCHAR    szWork[A04_KEY_LEN];
    WORD    wCurPage,
            wBefPage;

    /* ----- Set display page ----- */

    if (wPageNo) {
        wCurPage = (WORD)(wPageNo - A04_DEF_PAGENO);
    } else {
        wCurPage = 0;
    }

    /* ----- Set Before display page ----- */

    if (wBeforePage) {
        wBefPage = (WORD)(wBeforePage - A04_DEF_PAGENO);
    } else {
        wBefPage = 0;
    }

    for (wI = 0, unID = IDD_A04_KEY01; wI < MAX_FSC_NO; wI++, unID++) {

        /* ----- Check Keyed PLU Key for before selected page ----- */

        if ((UINT)lpFsc->Fsc[wBefPage].TblFsc[wI].uchFsc == A04_PLUKEY) {

            szWork[0] = 0;

            /* ----- Clear Text to Specified Button ----- */

            DlgItemRedrawText(hDlg, unID, szWork);
        }

        /* ----- Check Keyed PLU Key for current selected page ----- */

        if ((UINT)lpFsc->Fsc[wCurPage].TblFsc[wI].uchFsc == A04_PLUKEY) {

            unPluKey = (UINT)lpFsc->Fsc[wCurPage].TblFsc[wI].uchExt;

            wsPepf(szWork, WIDE("K%03u"), unPluKey);

            /* ----- Set Created Text to Specified Button ----- */

            DlgItemRedrawText(hDlg, unID, szWork);
        }
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A004SetPluNo()
*
**  Input       :   HWND        hDlg    -   Window Handle of a DialogBox
*                   LPA04DATA   lpData  -   Address of Data Buffer
*
**  Return      :   WORD        wPage   -   Current Selected Menu Page No.
*
**  Description :
*       This function gets the number of current selected menu page. And
*   sets the PLU Number of a selected keyed PLU key to edittext.
*       It returns the number of current selected menu page.
* ===========================================================================
*/
VOID    A004SetPluNo(HWND hDlg, LPA04DATA lpData)
{
/*    UINT    unPluNo; */
    UINT    fMod;                        /* Saratoga */
    WCHAR    szPluNo[PLU_MAX_DIGIT + 1];  /* Saratoga */

    /* ----- Get Menu Page No. from ComboBox ----- */

    wPageNo = (WORD)(A04_DEF_PAGENO + SendDlgItemMessage(hDlg, IDD_A04_PAGE,
                                                         CB_GETCURSEL,
                                                         0, 0L));

    /* ----- Get Keyed PLU key No. from EditText ----- */

    wPluKeyNo = (WORD)GetDlgItemInt(hDlg, IDD_A04_PLUKEY, NULL, FALSE);

    if (wPluKeyNo) {

        /* ----- Get PLU No. of Specified Keyed PLU Key ----- */

        memset(szPluNo, 0x00, sizeof(szPluNo));
        memcpy(szPluNo, 
                 lpData->PluKey[wPageNo - 1][wPluKeyNo - 1].szPluNo,
                 sizeof(szPluNo));

                       
        /* ----- Set PLU No. to EditText ----- */

       szPluNo[PLU_MAX_DIGIT] = 0x00;
        DlgItemRedrawText(hDlg,
                       IDD_A04_PLUNO,
                       szPluNo);
       
        /* ----- Set Focus to EditText ----- */

        SendDlgItemMessage(hDlg, IDD_A04_PLUNO, EM_SETSEL, 0, MAKELONG(0, -1));

/* Saratoga Start */

        /* ----- set PLU UPC-E or UPC-V flag ---- */
        if (lpData->PluKey[wPageNo - 1][wPluKeyNo - 1].uchModStat & MOD_STAT_EVERSION) {
            fMod = 1;  /* E-Version */
        } else {
            fMod = 0;  /* Normal */
        }
        SendDlgItemMessage(hDlg, IDD_A04_LABEL, CB_SETCURSEL, fMod, 0L);

        /* ----- check all edit controls ----- */

        A004CtrlMaintButton(hDlg);

/* Saratoga End */

    }
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A004ChkRng()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WPARAM  wEditID -   EditText ID for Check Data
*                   WORD    wMin    -   Min. Value of Data Range. 
*                   WORD    wMax    -   Max. Value of Data Range. 
*
**  Return      :   BOOL    TRUE    -   Inputed Value is Out of Range
*                           FALSE   -   Inputed Value is Valid
*
**  Description :
*       This function determines whether the inputed value is out of range or
*   not. It shows the error message with messagebox, if the data is invalid.
*       It returns TRUE, if the data is invalid. Otherwise it returns FALSE.
* ===========================================================================
*/
BOOL    A004ChkRng(HWND hDlg, WORD wEditID, WORD wMin, WORD wMax)
{
    BOOL    fRet = FALSE;
    WORD    wData;
    WCHAR   szBuff[PEP_LOADSTRING_LEN + 1],
            szErr[PEP_LOADSTRING_LEN + 1],
            szCap[PEP_CAPTION_LEN + 1];

    /* ----- Get Inputed Data from Specified EditText ----- */

    wData = (WORD)GetDlgItemInt(hDlg, wEditID, NULL, FALSE);

    /* ----- Determine Inputed Data is Out of Range ----- */

    if (wData > wMax) {         /* Inputed Data is Out of Range */

        /* ----- Load Error Message from Resource ----- */

        LoadString(hResourceDll, IDS_PEP_CAPTION_A04, szCap, PEP_STRING_LEN_MAC(szCap));
        LoadString(hResourceDll, IDS_PEP_OVERRANGE,   szErr, PEP_STRING_LEN_MAC(szErr));

        wsPepf(szBuff, szErr, wMin, wMax);

        /* ----- Display Message ----- */

        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg, szBuff, szCap,
                   MB_OK | MB_ICONEXCLAMATION);

        /* ----- Undo Input Data ----- */

        SendDlgItemMessage(hDlg, wEditID, EM_UNDO, 0, 0L);

        fRet = TRUE;
    }
    return (fRet);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A004ExecAbort()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   short   nType   -   Type of End Code
*
**  Return      :   No return value.
*
**  Description :
*       This function frees the allocated area, and resets dialogbox handle.
* ===========================================================================
*/
VOID    A004ExecAbort(HWND hDlg, short nType)
{
    WCHAR   szCap[PEP_CAPTION_LEN + 1],
            szErr[PEP_LOADSTRING_LEN + 1];

    if (nType == A04_MEM_ERROR) {

        MessageBeep(MB_ICONEXCLAMATION);

        /* ----- Load Caution Message from Resource ----- */

        LoadString(hResourceDll, IDS_PEP_CAPTION_A04, szCap, PEP_STRING_LEN_MAC(szCap));
        LoadString(hResourceDll, IDS_PEP_ALLOC_ERR,   szErr, PEP_STRING_LEN_MAC(szErr));

        /* ----- Display Caution Message ----- */

        MessageBoxPopUp(hDlg, szErr, szCap,
                   MB_OK | MB_ICONEXCLAMATION);
    }
                                    
    /* ----- Destroy Main / Sub DialogBoxes ----- */

    DestroyWindow(*lphActDlgChk1);
    DestroyWindow(*lphActDlgChk2);

    /* ----- Reset DialogBox Handle ----- */

    *lphActDlgChk1 = *lphActDlgChk2 = 0;

    /* ----- Enable PEP Main Menu ----- */

    SendMessage(hwndActMain, PM_PEPMENU, (WPARAM)PEP_MENU_ALL_ON, 0L);
}

/* ===========================================================================
**  Synopsis:   BOOL    A004PluKeyChk()
*
**  Input   :   HWND    hDlg    -   Window Handle of a DialogBox
*
*   Return  :   TRUE            - PLU Key is entered
*               FALSE           - PLU Key is invalid
*
**  Description:    This procedure check input PLU Key
* ===========================================================================
*/
BOOL    A004PluKeyChk(HWND hDlg)
{
    WORD   wPage, wPluKeyNo;
    BOOL   fPLUKey;
    UINT   unPluKey;
    
    /* ----- Get Page No. ----- */

    wPage = (WORD)GetDlgItemInt(hDlg, IDD_A04_PAGE, NULL, FALSE);

    /* ----- Get PLU Key No. ----- */

    wPluKeyNo = (WORD)GetDlgItemInt(hDlg, IDD_A04_PLUKEY, NULL, FALSE);

    unPluKey = (UINT)lpFscData->Fsc[wPage - 1].TblFsc[wPluKeyNo - 1].uchFsc;

    if (unPluKey == A04_PLUKEY)
        fPLUKey = TRUE;
    else
        fPLUKey = FALSE;

    return fPLUKey;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A004SavePluData()
*                                    
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
*   Return      :   No return value.
*
**  Description:
*       This function sets inputed plu data to buffer.
* ===========================================================================
*/
VOID    A004SavePluData(HWND hDlg, WPARAM wParam)
{
    UINT    unPluNo;
    WCHAR    szPluNo[PLU_MAX_DIGIT + 1];

    UINT    unPluLab;
    LABELANALYSIS Label;
    short   sStat;
    WCHAR   szCap[PEP_CAPTION_LEN + 1],
            szMsg[PEP_LOADSTRING_LEN + 1];    /* Saratoga */
    BOOL    fRet;                       /* Saratoga */

    /* ----- Initialize buffer ----- */

    memset(&Label, 0, sizeof(LABELANALYSIS));    /* Saratoga */
    memset(szPluNo, 0x00, sizeof(szPluNo));

    /* ----- Get Inputed PLU No. from EditText ----- */

    memset(szPluNo, 0x00, sizeof(szPluNo));
    unPluNo = DlgItemGetText(hDlg, IDD_A04_PLUNO, szPluNo, sizeof(szPluNo));

    fRet = A004PluKeyChk(hDlg);

    if (fRet == FALSE) 
        return;

    if (wPageNo && wPluKeyNo) {

        if (wParam == IDD_A04_ADDCHG) {

           /* ----- get PLU No. from entry of dialogbox ----- */

           DlgItemGetText(hDlg,
                          IDD_A04_PLUNO,
                          Label.aszScanPlu,
                          PLU_MAX_DIGIT + 1);                    

           /* ----- Get Label Type ----- */

           unPluLab = (UINT)SendDlgItemMessage(hDlg,
                                               IDD_A04_LABEL,
                                               CB_GETCURSEL,
                                               0,
                                               0L);

           if(unPluLab == LA_EMOD) {
              Label.fchModifier = LA_EMOD;      /* Set E Flag */
           } else {
              Label.fchModifier = LA_NORMAL;    /* Set Normal Flag */
           }

           /* ----- Analyze PLU No. ----- */

           sStat = RflLabelAnalysis(&Label);

           if(sStat == LABEL_ERR) {

              LoadString(hResourceDll, IDS_PEP_CAPTION_A04, szCap, PEP_STRING_LEN_MAC(szCap));
              LoadString(hResourceDll, IDS_A04_ERR_PLUNO, szMsg, PEP_STRING_LEN_MAC(szMsg));

              /* ----- Display caution message ----- */

              MessageBeep(MB_ICONEXCLAMATION);
              MessageBoxPopUp(hDlg,
                         szMsg,
                         szCap,
                         MB_OK | MB_ICONEXCLAMATION);

           } else {

           if ((Label.uchType == LABEL_RANDOM) && (Label.uchLookup != LA_EXE_LOOKUP)) {

              LoadString(hResourceDll, IDS_PEP_CAPTION_A04, szCap, PEP_STRING_LEN_MAC(szCap));
              LoadString(hResourceDll, IDS_A04_ERR_LABEL, szMsg, PEP_STRING_LEN_MAC(szMsg));

              /* ----- Display caution message ----- */

              MessageBeep(MB_ICONEXCLAMATION);
              MessageBoxPopUp(hDlg,
                         szMsg,
                         szCap,
                         MB_OK | MB_ICONEXCLAMATION);

           } else {

                memcpy(lpPluData->PluKey[wPageNo - 1][wPluKeyNo - 1].szPluNo,
                         szPluNo,
                         sizeof(szPluNo));

                lpPluData->PluKey[wPageNo - 1][wPluKeyNo - 1].uchModStat = 0;

                /* ----- Set UPC-E modifier flag ----- */

                if (Label.fchModifier & LA_EMOD)
                   lpPluData->PluKey[wPageNo - 1][wPluKeyNo - 1].uchModStat |= MOD_STAT_EVERSION;
           }

          }
        } else {   /* wParam == IDD_A04_DEL*/

          /* ----- reset buffer ----- */

          memset(lpPluData->PluKey[wPageNo - 1][wPluKeyNo - 1].szPluNo,
                   0,
                   (PLU_MAX_DIGIT + 1));
          lpPluData->PluKey[wPageNo - 1][wPluKeyNo - 1].uchModStat = 0x00;

          /* ----- initialize edit control ----- */

          SendDlgItemMessage(hDlg, IDD_A04_PLUNO, EM_SETSEL, 0, MAKELONG(0, -1));
          SendDlgItemMessage(hDlg, IDD_A04_PLUNO, WM_CLEAR, 0, 0L);

          SendDlgItemMessage(hDlg, IDD_A04_LABEL, CB_SETCURSEL, 0, 0L);
        
          EnableWindow(GetDlgItem(hDlg, IDD_A04_ADDCHG), FALSE);
          EnableWindow(GetDlgItem(hDlg, IDD_A04_DEL),    FALSE);
        }

    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A004ConvPluKey()
*
**  Input       :   HWND        hDlg    -   Window Handle of a DialogBox
*                   LPA04FSC    lpFsc   -   Address of FSC Data Buffer
*
**  Return      :   No return value.
*
**  Description :
*       Correct the Extended FSC of PLU Key to the Actual Keyboard Position
*       1/23/98
* ===========================================================================
*/
VOID    A004ConvPluKey(HWND hDlg, LPA04FSC lpFsc)
{
    LABELANALYSIS  Label;                    /* Saratoga */

    USHORT  usRetLen;
    UINT    unPluKey;
    WORD    wPage, wCode;
    BOOL    fConv = 0, fRet = IDYES;
    WCHAR   szCap[PEP_CAPTION_LEN + 1],
            szErr[A04_ERROR_LEN + 1];

    WCHAR    szPluNo[PLU_MAX_DIGIT + 1];     /* Saratoga */
    USHORT   usI;                            /* Saratoga */
    BOOL     bWork = FALSE;                  /* Saratoga */
    BOOL     bNumFlg = TRUE;                 /* Saratoga */
    short    sStat = 0;                      /* Saratoga */

    fConvPluData = 0;

    /* ----- Allocate Data Buffer from Global Heap ----- */

    hMemConvPluMenu = GlobalAlloc(GHND, MAX_PAGE_NO * FSC_PLU_MAX * sizeof(A04PLU));

    if (! hMemConvPluMenu) {    /* Failed Allocate Memory */

        MessageBeep(MB_ICONQUESTION);

        /* ----- Load Caution Message from Resource ----- */

        LoadString(hResourceDll, IDS_PEP_CAPTION_A04, szCap, PEP_STRING_LEN_MAC(szCap));
        LoadString(hResourceDll, IDS_PEP_ALLOC_ERR,   szErr, PEP_STRING_LEN_MAC(szErr));

        /* ----- Display Caution Message ----- */

        MessageBoxPopUp(hDlg, szErr, szCap,
                   MB_OK | MB_ICONQUESTION);

        return;

    }
    
    lpConvPluData = (LPA04DATA)GlobalLock(hMemConvPluMenu);

    if (!lpConvPluData) {
        GlobalFree(hMemConvPluMenu);
        return;
    }

    /* ----- Read Initial AC 4 Data ----- */
    ParaAllRead(CLASS_PARAPLUNOMENU,
                (UCHAR *)lpConvPluData,
                (USHORT)(MAX_PAGE_NO * FSC_PLU_MICROMAX * sizeof(A04PLU)),
                0,
                &usRetLen);

    for (wPage = 0; wPage < MAX_PAGE_NO; wPage++) {
        for (wCode = 0; wCode < MAX_FSC_NO; wCode++) {

            /* ----- Check Keyed PLU Key for current selected page ----- */

            if (!fConv) {

                memset(szPluNo, 0x00, sizeof(szPluNo));
                memcpy(szPluNo,
                         (lpPluData->PluKey[wPage][wCode].szPluNo),
                         sizeof(szPluNo));

                for(usI = 0; usI < PLU_MAX_DIGIT; usI++) {

                    if((szPluNo[usI] == 0x00) ||
                       (szPluNo[usI] == 0x20)) {
                    } else {
                       bWork = TRUE;
                    }
                }

                /* plu key is not arranged */
                if ((((UINT)lpFsc->Fsc[wPage].TblFsc[wCode].uchFsc == A04_PLUKEY) &&
                     ((UINT)lpFsc->Fsc[wPage].TblFsc[wCode].uchExt != (UINT)(wCode+1)))

                /* plu no is not assigned on plu key */
                 || ((bWork == FALSE) &&
                     ((UINT)lpFsc->Fsc[wPage].TblFsc[wCode].uchFsc != A04_PLUKEY))) {

                    MessageBeep(MB_ICONEXCLAMATION);

                    /* ----- Load Caution Message from Resource ----- */

                    LoadString(hResourceDll, IDS_PEP_CAPTION_A04, szCap, PEP_STRING_LEN_MAC(szCap));
                    LoadString(hResourceDll, IDS_A04_CONVPLUKEY, szErr, PEP_STRING_LEN_MAC(szErr));

                    /* ----- Display Caution Message ----- */

                    fRet = MessageBoxPopUp(hDlg, szErr, szCap,
                               MB_YESNO | MB_ICONEXCLAMATION);

                    if (fRet == IDNO){
                        break;
                    } else {
                        fConv = 1;
                    }
                }
            }

            if ((UINT)lpFsc->Fsc[wPage].TblFsc[wCode].uchFsc == A04_PLUKEY) {

                /* relocate plu no. */
                unPluKey = (UINT)lpFsc->Fsc[wPage].TblFsc[wCode].uchExt;
                if (unPluKey) unPluKey--;

                memset(szPluNo, 0x00, sizeof(szPluNo));

                memcpy(szPluNo, 
                         lpConvPluData->PluKey[wPage][unPluKey].szPluNo,
                         sizeof(szPluNo));

                for (usI = 0; usI < PLU_MAX_DIGIT; usI++) {

                   if (((szPluNo[usI] >= '0') && (szPluNo[usI] <= '9')) ||
                       (szPluNo[usI] == 0x20) || (szPluNo[usI] == 0x00)) {

                     continue;

                   } else {

                     bNumFlg = FALSE;
                   }
                }

                if (bNumFlg != FALSE) {   /* ----- PLU number is numeric ----- */

                  /* ----- PLU number C/D ----- */

                  memset(&Label, 0x00, sizeof(Label));

                  Label.fchModifier = lpConvPluData->PluKey[wPage][unPluKey].uchModStat;

                  memcpy(Label.aszScanPlu, szPluNo, sizeof(szPluNo));

                  /* ----- Analyze PLU number ----- */

                  sStat = RflLabelAnalysis(&Label);

                  if (sStat != LABEL_ERR) {

                    memcpy(lpPluData->PluKey[wPage][unPluKey].szPluNo,
                             szPluNo,
                             sizeof(szPluNo));

                    lpPluData->PluKey[wPage][unPluKey].uchModStat = 
                               lpConvPluData->PluKey[wPage][unPluKey].uchModStat;

                  } else {

                    memset(szPluNo, 0, sizeof(szPluNo));

                    memcpy(lpPluData->PluKey[wPage][unPluKey].szPluNo,
                             szPluNo,
                             sizeof(szPluNo));

                    lpPluData->PluKey[wPage][unPluKey].uchModStat = 0;

                  }

                } else {                  /* ----- PLU number is not numeric ----- */


                  memset(szPluNo, 0, sizeof(szPluNo));

                  memcpy(lpPluData->PluKey[wPage][unPluKey].szPluNo,
                           szPluNo,
                           sizeof(szPluNo));

                  lpPluData->PluKey[wPage][unPluKey].uchModStat = LA_NORMAL;

                }
                lpFsc->Fsc[wPage].TblFsc[wCode].uchExt = (UCHAR)(wCode+1);

            } else {

                /* clear plu no. */
                lpPluData->PluKey[wPage][wCode].szPluNo[0] = '0';
            }
        }
        if (fRet == IDNO) break;
    }

    /* ----- Release Allocated Area ----- */

    GlobalUnlock(hMemConvPluMenu);
    GlobalFree(hMemConvPluMenu);

    if (fConv) {
        fConvPluData = 1;
    }
}
/* ===== End of A004.C ===== */

