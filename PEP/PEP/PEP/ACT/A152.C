/*
* ---------------------------------------------------------------------------
* Title         :   Employee Number Assignment (AC 152)
* Category      :   Maintenance, NCR 2170 PEP for Windows (HP US Enhance Model)
* Program Name  :   A152.C
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Abstract :
*				Provide the AC152 dialog which allows the user to add, modify,
*				or delete employee time keeping records.
*
*				Employee Time Keeping (ETK) is different from Operator Assignment
*				meaning you may have employees who are not operators.
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* Sep-06-93 : 00.00.01 : T.Yamamoto : Initial
* Sep-17-93 : 00.00.02 : T.Nakahata : Check Minus Data Input
* Jan-25-94 : 00.00.03 : H.Mamiya   : Change insertion string format
* Jun-29-16 : 02.02.01 : R.Chambers : remove blank lines, fix atol() instead of _wtol().
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
#include    <stdlib.h>

#include    <ecr.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csetk.h>

#include    "action.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "file.h"
#include    "a152.h"

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
static  BYTE    bConf;

/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/

/*
* ===========================================================================
**  Synopsis:   BOOL    WINAPI  A152DlgProc()
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
*       This is a dialgbox procedure for the employee number assignment.
* ===========================================================================
*/
BOOL    WINAPI  A152DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static USHORT   usCurEtk;               /* current employee record */
    static USHORT   usMaxEtk;               /* maximum employee record */
    static PEPSPIN PepSpin;
    int    idEdit;
	int		empCheck;
	long	nEmp;
    WCHAR   szWork[128];

    BOOL    fRet;

    switch (wMsg) {
    case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));
        /* ----- Initialize dialogbox ----- */
        usCurEtk = A152InitDlg(hDlg, &usMaxEtk);
        /* ----- Initialize Common Spin Mode Flag ----- */
        PepSpin.lMin       = A152_DATA_MIN;
        PepSpin.nStep      = A152_SPIN_STEP;
        PepSpin.fSpinStyle = PEP_SPIN_NOROLL;
        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j=IDD_A152_MAX; j<=IDD_A152_CODE3_RNG; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_VSCROLL:
        /* ----- Get SpinButton ID ----- */
        idEdit = GetDlgCtrlID((HWND)lParam);
        idEdit -= A152_SPIN_OFFSET;

        /* ----- Initialize Configulation of DialogBox ----- */
        switch (idEdit) {
        case    IDD_A152_NO:
            PepSpin.lMax       = A152_ETK_NOMAX;
            PepSpin.lMin       = A152_DATA_MIN;
            PepSpin.nTurbo     = A152_SPIN_TURBO;
            PepSpin.nTurStep   = A152_SPIN_TURSTEP;
            break;

        default:
            PepSpin.lMax       = A152_CODE_MAX;
            PepSpin.lMin       = A152_CODE_MIN;
            PepSpin.nTurbo     = 0;
            PepSpin.nTurStep   = 0;
        }

        /* ----- Common SpinButton Procedure ----- */
        PepSpinProc(hDlg, wParam, idEdit, (LPPEPSPIN)&PepSpin);
        SendMessage(hDlg, WM_COMMAND, MAKELONG(idEdit, EN_CHANGE), 0L);
        return FALSE;   

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDD_A152_CONF:
            /* ----- Set confirmation flag ----- */
            if (SendDlgItemMessage(hDlg, IDD_A152_CONF, BM_GETCHECK, 0, 0L) == TRUE) {
                bConf &= ~A152_CONF_CHK;
            } else {
                bConf |= A152_CONF_CHK;
            }
            return TRUE;

        case IDD_A152_LIST:
            if (HIWORD(wParam) == LBN_SELCHANGE) {
                /* ----- Set data to maintenance-box ----- */
                A152SetEditRec(hDlg, usCurEtk);
                return TRUE;
            }
            return FALSE;

        case IDD_A152_ADDCHG:
        case IDD_A152_DEL:
            if (DlgItemGetText(hDlg, IDD_A152_NO, szWork, PEP_STRING_LEN_MAC(szWork)) != 0) {
                                                       /* check employee No. */
                /* ----- Save data to buffer and set listbox ----- */
                fRet = A152GetEditRec(hDlg, &usCurEtk, wParam, usMaxEtk);

                if ((LOWORD(wParam) == IDD_A152_DEL) && (fRet == TRUE)) {
                                                /* delete button is clicked */
                    /* ----- Set data to maintenance-box ----- */
                    A152SetEditRec(hDlg, usCurEtk);
                }
            }
            return TRUE;

        case    IDD_A152_NO:
            if (HIWORD(wParam) == EN_CHANGE) {
				empCheck= IDD_A152_NO;
				nEmp = (long)GetDlgItemInt(hDlg, empCheck, NULL, TRUE);
				if(nEmp < A152_DATA_MIN) 
				{
						A152DispError(hDlg, empCheck);
				}                
				/* ----- Check employee exist and control button ----- */
                A152ChkExist(hDlg);
                return TRUE;
            }
            return FALSE;

        case IDOK:
        case IDCANCEL:
            /* ----- Finalize dialogbox ----- */
            A152FinDlg(hDlg, wParam);
            return TRUE;
        }
        return FALSE;

    default:
        return FALSE;
    }
}

/*
* ===========================================================================
**  Synopsis:   USHORT  A152InitDlg()
*
*   Input   :   HWND        hDlg     - handle of a dialogbox
*               LPUSHORT    lpusMax  - address of maximum records
**
*   Return  :   USHORT      usCur    - current employee records
*
**  Description:
*       This procedure initialize dialogbox.
* ===========================================================================
*/
USHORT  A152InitDlg(HWND hDlg, LPUSHORT lpusMax)
{
    ULONG       adwEtkNo[FLEX_ETK_MAX];
    USHORT      usI, usCur = 0;
    WCHAR        szWork[A152_BUFF_LEN];
    WCHAR        szBuff[A152_BUFF_LEN];
    WCHAR        szCur[A152_BUFF_LEN];
    WCHAR        szMax[A152_BUFF_LEN];
    WCHAR        szMsg[A152_BUFF_LEN];
    WCHAR        szCap[PEP_CAPTION_LEN];
    PARAFLEXMEM FlexData;
    ETK_JOB     Job;
    WCHAR        szNameBuff[A152_NAME_LEN + 1];
    WCHAR        szNameReadBuff[A152_NAME_LEN + 1];

    /* ----- Set address to employee ----- */
     FlexData.uchAddress = FLEX_ETK_ADR;

    /* ----- Read the filexible memory data of employee max ----- */
    ParaFlexMemRead(&FlexData);

    /* ----- Store maximum record ----- */
    /* NCR2172 */
    /* *lpusMax = FlexData.usRecordNumber;*/
    *lpusMax = (USHORT)FlexData.ulRecordNumber;

    if (*lpusMax == 0) {        /* not yet be set the number of employee */
        /* ----- Load string from resource ----- */
        LoadString(hResourceDll, IDS_PEP_CAPTION_A152, szCap, PEP_STRING_LEN_MAC(szCap));
        LoadString(hResourceDll, IDS_A152_FLEX_EMPTY, szMsg, PEP_STRING_LEN_MAC(szMsg));

        /* ----- Display caution message ----- */
        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg, szMsg, szCap, MB_OK | MB_ICONEXCLAMATION);

		//Return Cancel to indicate Failure
        EndDialog(hDlg, LOWORD(IDCANCEL));

        return (usCur);
    }

    /* ----- Load employee file name from resorece ----- */
    LoadString(hResourceDll, IDS_FILE_NAME_ETK, szCap, PEP_STRING_LEN_MAC(szCap));

    /* ----- Back up for Cancel Key ----- */
    PepFileBackup(hDlg, szCap, PEP_FILE_BACKUP);

    /* ----- Read all employee No. ----- */
    usCur = EtkCurAllIdRead(sizeof(adwEtkNo), adwEtkNo);

    /* ----- disable add/change & delete button ----- */
    EnableWindow(GetDlgItem(hDlg, IDD_A152_ADDCHG), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDD_A152_DEL),    FALSE);

    /* ----- Set text font ----- */
    PepSetFont(hDlg, IDD_A152_NAME);
    PepSetFont(hDlg, IDD_A152_LIST);

    /* ----- Set max entry data length. ----- */
    SendDlgItemMessage(hDlg, IDD_A152_NO, EM_LIMITTEXT, A152_NO_LEN, 0L);
    SendDlgItemMessage(hDlg, IDD_A152_CODE1, EM_LIMITTEXT, A152_CODE_LEN, 0L);
    SendDlgItemMessage(hDlg, IDD_A152_CODE2, EM_LIMITTEXT, A152_CODE_LEN, 0L);
    SendDlgItemMessage(hDlg, IDD_A152_CODE3, EM_LIMITTEXT, A152_CODE_LEN, 0L);
    SendDlgItemMessage(hDlg, IDD_A152_NAME, EM_LIMITTEXT, A152_NAME_LEN, 0L);

    /* ----- Read all employee name & set them to listbox ----- */

    LoadString(hResourceDll, IDS_A152_LISTSTRING, szBuff, PEP_STRING_LEN_MAC(szBuff));
    for (usI = 0; usI < usCur; usI++) {

        /* ----- Read Employee data ----- */
        EtkIndJobRead(adwEtkNo[usI], &Job, szNameReadBuff);

        /* ----- Replace Double Width Key (0x12 => '~') ----- */
		memset(&szNameBuff, 0x00, sizeof(szNameBuff));
        PepReplaceMnemonic(szNameReadBuff, szNameBuff, A152_NAME_LEN, PEP_MNEMO_READ);

        /* ----- Make insert string ----- */
        wsPepf(szWork, szBuff, adwEtkNo[usI], szNameBuff);

        /* ----- Set data in buffer to listbox ----- */
        DlgItemSendTextMessage(hDlg, IDD_A152_LIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szWork));
    }

    /* ----- Set maximum record number to static text ----- */
    LoadString(hResourceDll, IDS_A152_MAXETK, szMax, PEP_STRING_LEN_MAC(szMax));
    wsPepf(szWork, szMax, *lpusMax);
    DlgItemRedrawText(hDlg, IDD_A152_MAX, szWork);

    /* ----- Set current record number to static text ----- */
    LoadString(hResourceDll, IDS_A152_CURETK, szCur, PEP_STRING_LEN_MAC(szCur));
    wsPepf(szWork, szCur, usCur);
    DlgItemRedrawText(hDlg, IDD_A152_CUR, szWork);

    /* ----- Set focus to listbox ----- */
    SetFocus(GetDlgItem(hDlg, IDD_A152_LIST));

    /* ----- Set delete confirmation checkbox ----- */
    if (bConf & A152_CONF_CHK) {
        CheckDlgButton(hDlg, IDD_A152_CONF, FALSE);
    } else {
        CheckDlgButton(hDlg, IDD_A152_CONF, TRUE);
    }

    return (usCur);
}

/*
* ===========================================================================
**  Synopsis:   VOID    A152SetEditRec()
*
*   Input   :   HWND    hDlg    - handle of a dialogbox
*               LPCASIF lpData  - address of record buffer
*               USHORT  usCur   - current employee records
**
*   Return  :   None
*
**  Description:
*       This procedure set data to maintenance-box.
* ===========================================================================
*/
VOID    A152SetEditRec(HWND hDlg, USHORT usCur)
{
    DWORD   dwOff;
    WCHAR    szBuff[A152_BUFF_LEN];
    DWORD   dwNo;
    ETK_JOB Job;
    WCHAR    szNameBuff[A152_MNE_LEN + 1];
    WCHAR    szNameReadBuff[A152_MNE_LEN+ 1];

    /* ----- enable/disable to push button ----- */
	memset(szNameReadBuff, 0x00, sizeof(szNameReadBuff));
	memset(szNameBuff, 0x00, sizeof(szNameBuff));
    EnableWindow(GetDlgItem(hDlg, IDD_A152_ADDCHG), usCur);
    EnableWindow(GetDlgItem(hDlg, IDD_A152_DEL),    usCur);

    if (usCur != 0) {            /* check current record */

        /* ----- get selected item order from listbox ----- */
        dwOff = SendDlgItemMessage(hDlg, IDD_A152_LIST, LB_GETCURSEL, 0, 0L);

        /* ----- get selected item data from listbox ----- */
        SendDlgItemMessage(hDlg, IDD_A152_LIST, LB_GETTEXT, (WPARAM)dwOff, (LPARAM)(szBuff));

        /* ----- Calculate No. of employee ----- */
        dwNo = (DWORD)_wtol(szBuff); //changed JHHJ

        /* ----- Retrieve item data ----- */
        EtkIndJobRead(dwNo, &Job, szNameReadBuff);

        /* ----- Create String ----- */
        wsPepf(szBuff, WIDE("%lu"), dwNo);

        /* ----- Set employee No. ----- */
        DlgItemRedrawText(hDlg, IDD_A152_NO, szBuff);

        /* ----- Set job code ----- */
        SetDlgItemInt(hDlg, IDD_A152_CODE1, (UINT)Job.uchJobCode1, FALSE);
        SetDlgItemInt(hDlg, IDD_A152_CODE2, (UINT)Job.uchJobCode2, FALSE);
        SetDlgItemInt(hDlg, IDD_A152_CODE3, (UINT)Job.uchJobCode3, FALSE);

        /* ----- Replace Double Key Code (0x12 => '~') ----- */
        PepReplaceMnemonic(szNameReadBuff, szNameBuff, A152_NAME_LEN, PEP_MNEMO_READ);

        /* ----- Set employee No. to EditText ----- */
        DlgItemRedrawText(hDlg, IDD_A152_NAME, szNameBuff);

    } else {
        /* ----- Clear edit control and checkbox ----- */
        SendDlgItemMessage(hDlg, IDD_A152_NO, EM_SETSEL, 0, MAKELONG(0, -1));
        SendDlgItemMessage(hDlg, IDD_A152_NO, WM_CLEAR, 0, 0L);
        SendDlgItemMessage(hDlg, IDD_A152_CODE1, EM_SETSEL, 0, MAKELONG(0, -1));
        SendDlgItemMessage(hDlg, IDD_A152_CODE1, WM_CLEAR,  0, 0L);
        SendDlgItemMessage(hDlg, IDD_A152_CODE2, EM_SETSEL, 0, MAKELONG(0, -1));
        SendDlgItemMessage(hDlg, IDD_A152_CODE2, WM_CLEAR,  0, 0L);
        SendDlgItemMessage(hDlg, IDD_A152_CODE3, EM_SETSEL, 0, MAKELONG(0, -1));
        SendDlgItemMessage(hDlg, IDD_A152_CODE3, WM_CLEAR,  0, 0L);
        SendDlgItemMessage(hDlg, IDD_A152_NAME, EM_SETSEL, 0, MAKELONG(0, -1));
        SendDlgItemMessage(hDlg, IDD_A152_NAME, WM_CLEAR,  0, 0L);

        /* ----- Set focus to listbox ----- */
        SetFocus(GetDlgItem(hDlg, IDD_A152_LIST));
    }
}

/*
* ===========================================================================
**  Synopsis:   BOOL    A152GetEditRec()
*
*   Input   :   HWND        hDlg    - handle of a dialogbox
*               LPUSHORT    lpusCur - address of current employee records
*               WORD        wID     - add/change, delete selection ID
*               USHORT      usMax   - maximum employee records
**
*   Return  :   TRUE            - user process is executed
*               FALSE           - deletion is fault
*
**  Description:
*       This procedure save data to file.
* ===========================================================================
*/
BOOL    A152GetEditRec(HWND hDlg, LPUSHORT lpusCur, WPARAM wID, USHORT usMax)
{
    DWORD   dwNo;
    ETK_JOB Job;
    WCHAR    szCap[128],
            szDesc[128],
            szWork[128];
    WCHAR    szNameWork[A152_MNE_LEN + 1];
    WCHAR    szNameReadWork[A152_MNE_LEN + 1];
    BOOL    fCode1,
            fCode2,
            fCode3;
    WORD    wJobLen1,
            wJobLen2,
            wJobLen3;

    /* ----- Get employee No. ----- */
    DlgItemGetText(hDlg, IDD_A152_NO, szWork, PEP_STRING_LEN_MAC(szWork));

    /* ----- Calculate No. of employee ----- */
    dwNo = (DWORD)_wtol(szWork);

    /* ----- Get job code ----- */
    Job.uchJobCode1 = (UCHAR)GetDlgItemInt(hDlg, IDD_A152_CODE1, (LPBOOL)&fCode1, FALSE);
    Job.uchJobCode2 = (UCHAR)GetDlgItemInt(hDlg, IDD_A152_CODE2, (LPBOOL)&fCode2, FALSE);
    Job.uchJobCode3 = (UCHAR)GetDlgItemInt(hDlg, IDD_A152_CODE3, (LPBOOL)&fCode3, FALSE);

    /* ----- Get job code length ----- */
    wJobLen1 = (WORD)SendDlgItemMessage(hDlg, IDD_A152_CODE1, WM_GETTEXTLENGTH, 0, 0L);
    wJobLen2 = (WORD)SendDlgItemMessage(hDlg, IDD_A152_CODE2, WM_GETTEXTLENGTH, 0, 0L);
    wJobLen3 = (WORD)SendDlgItemMessage(hDlg, IDD_A152_CODE3, WM_GETTEXTLENGTH, 0, 0L);

    if (((fCode1 == 0) && (wJobLen1 != 0)) ||
        ((fCode2 == 0) && (wJobLen2 != 0)) ||
        ((fCode3 == 0) && (wJobLen3 != 0))) {
                                     /* check inputed data is valid or not */
        /* ----- Make error message ----- */
        LoadString(hResourceDll, IDS_PEP_CAPTION_A152, szCap, PEP_STRING_LEN_MAC(szCap));
        LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_STRING_LEN_MAC(szWork));
        wsPepf(szDesc, szWork, (WORD)A152_DATA_MIN, (WORD)A152_CODE_MAX);

        /* ----- Display error message ----- */
        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg, szDesc, szCap, MB_OK | MB_ICONEXCLAMATION);

        /* ----- Set Focus to EditText ----- */
        wID = (WORD)((fCode1 == 0) ? IDD_A152_CODE1:
                     (fCode2 == 0) ? IDD_A152_CODE2:
                                        IDD_A152_CODE3);
        SendDlgItemMessage(hDlg, wID, EM_SETSEL, TRUE, MAKELONG(0, -1));
        SetFocus(GetDlgItem(hDlg, wID));
        return TRUE;
    }

    /* ----- Get Inputed Employee Name from EditText ----- */
    memset(szNameReadWork, 0, sizeof(szNameReadWork));
    DlgItemGetText(hDlg, IDD_A152_NAME, szNameReadWork, PEP_STRING_LEN_MAC(szNameReadWork));

    /* ----- Replace Double Key Code ('~' => 0x12) ----- */
    PepReplaceMnemonic(szNameReadWork, szNameWork, A152_NAME_LEN, PEP_MNEMO_WRITE);

    /* ----- Check over maximum ----- */
    if (A152ChkMax(hDlg, dwNo, *lpusCur, wID, usMax) == TRUE) {
        return TRUE;
    }

    /*
		----- Refresh listbox -----
		If return value is FALSE then we make no change to ETK file.
		If return value is TRUE, then change to ETK file.
	 */
    if (A152RefreshList(hDlg, dwNo, lpusCur, wID, szNameReadWork) == FALSE) {
        return FALSE;
    }

    if (wID == IDD_A152_ADDCHG) {
        /* ----- Add/change record ----- */
        EtkAssign(dwNo, &Job, szNameWork);
    } else if (wID == IDD_A152_DEL) {
        /* ----- Delete record ----- */
        EtkDelete(dwNo);
    }
    return TRUE;
}

/*
* ===========================================================================
**  Synopsis:   BOOL    A152ChkMax()
*
*   Input   :   HWND    hDlg    - handle of a dialogbox
*               DWORD   dwNo    - maintenanced employee record number
*               USHORT  usCur   - current employee records
*               WORD    wParam  - pressed button identifier
*               USHORT  usMax   - maximum employee records
**
*   Return  :   TRUE            - over maximum records 
*               FALSE           - default process is expected
*
**  Description:
*       This procedure check maximum record.
* ===========================================================================
*/
BOOL
A152ChkMax(HWND hDlg, DWORD dwNo, USHORT usCur, WPARAM wParam, USHORT usMax)
{
    DWORD   dwIndex;
    WCHAR    szWork[A152_BUFF_LEN];
    WCHAR    szBuff[A152_BUFF_LEN];
    WCHAR    szCap[PEP_CAPTION_LEN];
    WCHAR    szMsg[PEP_OVER_LEN];

    /* ----- make search data in listbox ----- */
    LoadString(hResourceDll, IDS_A152_NUMBERSTRING, szBuff, PEP_STRING_LEN_MAC(szBuff));
    wsPepf(szWork, szBuff, dwNo);

    /* ----- Search corresponding data from current employee data ----- */
		//SR 445 Fix. We have made a new function 
		//called DlgItemSendTextMessage which now passes the information through 
		//the unicode utilities to SendDlgItemMessage 
    dwIndex = DlgItemSendTextMessage(hDlg, IDD_A152_LIST, LB_FINDSTRING, (WPARAM)-1, (LPARAM)(szWork));
    /* ----- Check over maximum ----- */
    if ((dwIndex == LB_ERR) && (LOWORD(wParam) == IDD_A152_ADDCHG) && (usCur == usMax)) {
        /* ----- Load string from resource ----- */
        LoadString(hResourceDll, IDS_PEP_CAPTION_A152, szCap, PEP_STRING_LEN_MAC(szCap));
        LoadString(hResourceDll, IDS_A152_OVERRECORD, szMsg, PEP_STRING_LEN_MAC(szMsg));

        /* ----- Display caution message ----- */
        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg, szMsg, szCap, MB_OK | MB_ICONEXCLAMATION);
        /* ----- Set edit-box(No.) selected ----- */
        SendDlgItemMessage(hDlg, IDD_A152_NO, EM_SETSEL, 1, MAKELPARAM(0, -1));
        return TRUE;
    }

    return FALSE;
}

/*
* ===========================================================================
**  Synopsis:   BOOL    A152RefreshList()
*
*   Input   :   HWND        hDlg    - handle of a dialogbox
*               DWORD       dwNo    - maintenanced employee record number
*               LPUSHORT    lpusCur - address of current employee records
*               WPARAM      wParam  - pressed button identifier
**
*   Return  :   TRUE            - ETK file requires updating
*               FALSE           - ETK file does not require updating
*
**  Description:
*       This procedure refreshes the listbox by adding a new item, deleting
*		an existing item, or changing an existing item.
* ===========================================================================
*/
BOOL
A152RefreshList(HWND hDlg, DWORD dwNo, LPUSHORT lpusCur, WPARAM wParam, LPWSTR szName)
{
    DWORD   dwIndex = LB_ERR;
    WCHAR   szWork[A152_BUFF_LEN + 1];
    WCHAR   szSearch[A152_BUFF_LEN + 1];
    WCHAR   szBuff[A152_BUFF_LEN + 1];
    BOOL    bUpdateDialog = FALSE;

    /* ----- make search data in listbox ----- */
    LoadString(hResourceDll, IDS_A152_NUMBERSTRING, szBuff, PEP_STRING_LEN_MAC(szBuff));
    wsPepf(szSearch, szBuff, dwNo);

    /* ----- make update data in listbox ----- */
    LoadString(hResourceDll, IDS_A152_LISTSTRING, szBuff, PEP_STRING_LEN_MAC(szBuff));
    wsPepf(szWork, szBuff, dwNo, szName);

    /*
		Search the list box with the displayed list of ETK records looking for
		the requested employee number from current employee data.
	*/
	dwIndex = DlgItemSendTextMessage(hDlg, IDD_A152_LIST, LB_FINDSTRING,(WPARAM)-1, (LPARAM)(szSearch));  //modification for R20  JHHJ
    if (dwIndex == LB_ERR) {            /* not found */
        if (LOWORD(wParam) == IDD_A152_ADDCHG) {  /* add/change button is pressed */
            /*
				The employee number does not currently exist in the list box.

				Insert new line of data into the listbox.
				Increment the number of current employee records.
				Set the list box cursor to the newly inserted record.
				Enable the Delete button on the dialog.
			 */
            DlgItemSendTextMessage(hDlg, IDD_A152_LIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szWork));

            (*lpusCur)++;
            SendDlgItemMessage(hDlg, IDD_A152_LIST, LB_SETCURSEL, (WPARAM)*lpusCur, 0L);
            EnableWindow(GetDlgItem(hDlg, IDD_A152_DEL), TRUE);

            /* ----- we need to update ETK file  ----- */
            bUpdateDialog = TRUE;
        }
		// if its not an add/change then ignore since can't delete what isn't there.
    } else {
        if (LOWORD(wParam) == IDD_A152_DEL) {     /* delete button is pressed */
            if (A152DispDelConf(hDlg, dwNo) == IDOK) {
                /*
					Delete the ETK entry from the listbox.
					Decrement the number of current employee records.
					Set cursor in the list box to the line before the
					deleted item unless the line is the first line in the
					listbox (line 0) in which case we just select line 0.
				*/
                SendDlgItemMessage(hDlg, IDD_A152_LIST, LB_DELETESTRING, (WPARAM)dwIndex, 0L);

	            (*lpusCur)--;
                if (dwIndex > 0) {
                    dwIndex--;
                }
                SendDlgItemMessage(hDlg, IDD_A152_LIST, LB_SETCURSEL, (WPARAM)dwIndex, 0L);

				/* ----- we need to update ETK file  ----- */
                bUpdateDialog = TRUE;
            }
			// if user did not confirm the delete then we ignore the delete request
        } else {                        /* add/change button is pressed */
            /*
				Delete the ETK string from the listbox.
				Then insert the new ETK string into the list box, same place.
				No change in the number of ETK records but we do need to update
				the ETK file.
			*/
            SendDlgItemMessage(hDlg, IDD_A152_LIST, LB_DELETESTRING, (WPARAM)dwIndex, 0L);
            DlgItemSendTextMessage(hDlg, IDD_A152_LIST, LB_INSERTSTRING, (WPARAM)dwIndex, (LPARAM)(szWork));
            SendDlgItemMessage(hDlg, IDD_A152_LIST, LB_SETCURSEL, (WPARAM)dwIndex, 0L);

			/* ----- we need to update ETK file  ----- */
            bUpdateDialog = TRUE;
        }
    }

    /* ----- display the number of current employee records ----- */
    LoadString(hResourceDll, IDS_A152_CURETK, szBuff, PEP_STRING_LEN_MAC(szBuff));
    wsPepf(szWork, szBuff, *lpusCur);
    DlgItemRedrawText(hDlg, IDD_A152_CUR, szWork);

    return bUpdateDialog ;
}

/*
* ===========================================================================
**  Synopsis:   int A152DispDelConf();
*
*   Input   :   HWND    hDlg    - handle of a dialogbox
*               DWORD   dwNo    - target employee No.
*
**  Return  :   IDOK     - user selection is OK
*               IDCANCEL - user selection is CANCEL
*
**  Description:
*       This function shows confirmation message for delete employee record.
* ===========================================================================
*/
int A152DispDelConf(HWND hDlg, DWORD dwNo)
{
    WCHAR    szCap[PEP_CAPTION_LEN];
    WCHAR    szMsg[PEP_OVER_LEN];
    WCHAR    szWork[PEP_OVER_LEN];
    int     nRet;

    if (bConf & A152_CONF_CHK) {         /* Confirmation is OFF */
        /* ----- No confirmation ----- */
        nRet = IDOK;
    } else {                            /* Confirmation is ON */
        /* ----- Load string from resource and make caution message ----- */
        LoadString(hResourceDll, IDS_PEP_CAPTION_A152, szCap, PEP_STRING_LEN_MAC(szCap));
        LoadString(hResourceDll, IDS_A152_DELCONF, szWork, PEP_STRING_LEN_MAC(szWork));
        wsPepf(szMsg, szWork, dwNo);
        /* ----- Display caution message ----- */
        MessageBeep(MB_ICONEXCLAMATION);
        nRet = MessageBoxPopUp(hDlg, szMsg, szCap, MB_OKCANCEL | MB_ICONEXCLAMATION);
    }
    return (nRet);
}

/*
* ===========================================================================
**  Synopsis:   VOID    A152ChkExist()
*
*   Input   :   HWND    hDlg    - handle of a dialogbox
**
*   Return  :   None
*
**  Description:
*       This procedure check employee exist and control button.
* ===========================================================================
*/
VOID    A152ChkExist(HWND hDlg)
{
    DWORD   dwNo;
    BOOL    fCheck;
    WCHAR    szWork[A152_BUFF_LEN];
    WCHAR    szBuff[A152_BUFF_LEN];
    DWORD   dwIndex;

    /* ----- Get employee No. ----- */
    DlgItemGetText(hDlg, IDD_A152_NO, szWork, PEP_STRING_LEN_MAC(szWork));

    /* ----- Calculate No. of employee ----- */
    dwNo = (DWORD)_wtol(szWork);

	//SR 445 Fix. Instead of Checking the pointer to the buffer of the
	//employee number, we will just use the DWORD value to check
    fCheck = (dwNo == 0) ? FALSE : TRUE;

    /* ----- enable/disable to push button ----- */
    EnableWindow(GetDlgItem(hDlg, IDD_A152_ADDCHG), fCheck);
    EnableWindow(GetDlgItem(hDlg, IDD_A152_DEL),    fCheck);

    /* ----- make search data in listbox ----- */
    LoadString(hResourceDll, IDS_A152_NUMBERSTRING, szBuff, PEP_STRING_LEN_MAC(szBuff));
    wsPepf(szWork, szBuff, dwNo);

    /* ----- Search corresponding data from current employee data ----- */
	//SR 445 Fix. We have made a new function 
		//called DlgItemSendTextMessage which now passes the information through 
		//the unicode utilities to SendDlgItemMessage 
    dwIndex = DlgItemSendTextMessage(hDlg, IDD_A152_LIST, LB_FINDSTRING, (WPARAM)-1, (LPARAM)(szWork));

    /* ----- enable/disable to push button ----- */
    if ((dwIndex == LB_ERR) || (fCheck == FALSE)) {
        EnableWindow(GetDlgItem(hDlg, IDD_A152_DEL), FALSE);
    } else {
        EnableWindow(GetDlgItem(hDlg, IDD_A152_DEL), TRUE);
    }

}

/*
* ===========================================================================
**  Synopsis:   VOID    A152FinDlg()
*
*   Input   :   HWND    hDlg    - handle of a dialogbox
*               WORD    wID     - OK, Cancel selection template ID
**
*   Return  :   None
*
**  Description:
*       This procedure initialize dialogbox.
* ===========================================================================
*/
VOID    A152FinDlg(HWND hDlg, WPARAM wID)
{
    WCHAR    szFile[PEP_CAPTION_LEN];
    HCURSOR hCursor;

    /* ----- Change cursor (arrow -> hour-glass) ----- */
    hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
    ShowCursor(TRUE);
    if (LOWORD(wID) == IDOK) {
        /* ----- Set file status flag ----- */
        PepSetModFlag(hwndActMain, PEP_MF_ETK, 0);
    } else {        /* wID == IDCANCEL */
        /* ----- Load employee file name from resorece ----- */
        LoadString(hResourceDll, IDS_FILE_NAME_ETK, szFile, PEP_STRING_LEN_MAC(szFile));
        /* ----- Restore temporary file ----- */
        PepFileBackup(hDlg, szFile, PEP_FILE_RESTORE);
    }

    /* ----- Delete tempory file ----- */
    PepFileDelete();

    /* ----- Restore cursor (hour-glass -> arrow) ----- */
    ShowCursor(FALSE);
    SetCursor(hCursor);

    /* ----- Destroy dialogbox ----- */
    EndDialog(hDlg, LOWORD(wID));
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A152DispError() - LDelhomme
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wID     -   EditText ID of Error Occurred
*
**  Return      :   No return value.
*
**  Description :
*       This function displays the error message of data range over with 
*		MessageBox.
* ===========================================================================
*/
VOID    A152DispError(HWND hDlg, WPARAM wID)
{
    WCHAR   szWork[PEP_OVER_LEN], szMessage[PEP_OVER_LEN];

    /* ----- Load String from Resource ----- */
    LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_OVER_LEN);

    /* ----- Make Error Message ----- */
    wsPepf(szMessage, szWork, A152_DATA_MIN, 999999999L);

    /* ----- Display Error Message with MessageBox ----- */
    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hDlg, szMessage, WIDE("Employee Number"), MB_OK | MB_ICONEXCLAMATION);

    /* ----- Set Focus to Error Occurred EditText ----- */
    SendDlgItemMessage(hDlg, wID, EM_SETSEL, 0, MAKELONG(0, -1));
    SetFocus(GetDlgItem(hDlg, wID));
}

/* ===== End of A152.C ===== */
