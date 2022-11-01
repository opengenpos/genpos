/*
* ---------------------------------------------------------------------------
* Title         :   Server Maintenance (AC 9, 50)
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A050.C
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
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
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
#include    <stdlib.h>

#include    <ecr.h>
#include    <paraequ.h>
#include    <para.h>
#include    <cswai.h>

#include    "action.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "file.h"
#include    "a050.h"

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
static  BYTE    bConf = A50_CONF_CHK;   /* Delete Confirmation Flag */

/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  A050DlgProc()
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
*                       Server Maintenance (AC 9, 50)
* ===========================================================================
*/
BOOL    WINAPI  A050DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  USHORT  unMaxSer,       /* Maximum No. of Server Records    */
                    unCurSer;       /* Current No. of Server Records    */
    static  WAIIF   SerData;        /* Current Maint. Server Data       */
    static  PEPSPIN PepSpin;
    int     idEdit;

    BOOL    fRet;

    switch (wMsg) {
    case WM_INITDIALOG:

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- Initialize dialogbox ----- */

        unCurSer = A050InitDlg(hDlg, (LPUSHORT)&unMaxSer);

        /* ----- Initialize Common Spin Mode Flag ----- */

        PepSpin.nStep      = A50_SPIN_STEP;
        PepSpin.fSpinStyle = PEP_SPIN_NOROLL;

        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=IDD_A50_MAX; j<=IDD_A50_CAPTION4; j++)
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

        idEdit -= A50_SPIN_OFFSET;

        /* ----- Initialize Configulation of DialogBox ----- */

        switch (idEdit) {

        case    IDD_A50_NO:
            PepSpin.lMin       = (long)A50_SER_NO_MIN;
            PepSpin.lMax       = (long)A50_SER_NO_MAX;
            PepSpin.nTurbo     = 0;
            PepSpin.nTurStep   = 0;
            break;

        default:
            PepSpin.lMin       = (long)A50_GC_MIN;
            PepSpin.lMax       = (long)A50_GC_MAX;
            PepSpin.nTurbo     = A50_SPIN_TURBO;
            PepSpin.nTurStep   = A50_SPIN_TURSTEP;
        }

        /* ----- Common SpinButton Procedure ----- */

        PepSpinProc(hDlg, wParam, idEdit, (LPPEPSPIN)&PepSpin);

        SendMessage(hDlg, WM_COMMAND, MAKELONG(idEdit, EN_CHANGE), 0L);

        return FALSE;   

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDD_A50_CONF:

            if (HIWORD(wParam) == BN_CLICKED) {

                /* ----- Set confirmation flag ----- */

                if (IsDlgButtonChecked(hDlg, wParam)) {

                    bConf |= A50_CONF_CHK;

                } else {

                    bConf &= ~A50_CONF_CHK;
                }
                return TRUE;
            }
            return FALSE;

        case IDD_A50_LIST:

            if (HIWORD(wParam) == LBN_SELCHANGE) {

                /* ----- Set data to maintenance-box ----- */

                A050SetData(hDlg, (LPWAIIF)&SerData, unCurSer);

                return TRUE;
            }
            return FALSE;

        case IDD_A50_ADDCHG:
        case IDD_A50_DEL:

            if (HIWORD(wParam) == BN_CLICKED) {

                /* ----- Check Whether G.C No. Exist or Not ----- */

                if (A050ChkGCData(hDlg)) {

                    /* ----- get server data ----- */

                    fRet = A050GetData(hDlg,
                                   wParam,
                                   (LPWAIIF)&SerData,
                                   (LPUSHORT)&unCurSer,
                                   unMaxSer);

                    if ((LOWORD(wParam) == IDD_A50_DEL) && (fRet == TRUE)) {

                        /* ----- Set data to maintenance-box ----- */

                        A050SetData(hDlg, (LPWAIIF)&SerData, unCurSer);

                    }

                    SetFocus(GetDlgItem(hDlg, IDD_A50_LIST));
                }
                return TRUE;
            }
            return FALSE;

        case    IDD_A50_NO:

            if (HIWORD(wParam) == EN_CHANGE) {

                /* ----- Check record existance ----- */    

                A050ChkExist(hDlg);

                return TRUE;
            }
            return FALSE;

        case IDOK:
        case IDCANCEL:

            if (HIWORD(wParam) == BN_CLICKED) {

                /* ----- Finalize DialogBox ----- */

                A050FinDlg(hDlg, wParam);

                return TRUE;
            }
        }
        return FALSE;

    default:
        return FALSE;
    }
}

/*
* ===========================================================================
**  Synopsis    :   USHORT  A050InitDlg()
*
**  Input       :   HWND        hDlg        -   Window Handle of a DialogBox
*                   LPUSHORT    lpunMax     -   Address of Maximum Records No.
*
**  Return      :   USHORT      unServer    -   Current Server Records No.
*
**  Description :
*       This function creates the backup file of server file. And then it reads
*   all server ID No. and creates the strings for insert listbox. If the max.
*   records no. is 0, it destroys this dialogbox.
* ===========================================================================
*/
USHORT  A050InitDlg(HWND hDlg, LPUSHORT lpunMax)
{
    USHORT  ausSerNo[MAX_NO_WAIT],
            unI,
            unCur = 0;
    WCHAR    szMsg[A50_MEM_LEN],
            szCap[PEP_CAPTION_LEN];
    WAIIF   SerData;

    /* ----- Get max. value of server records from Prog.#2 ----- */

    *lpunMax = A050ReadMax();

    /* ----- Check user sets the No. of server in Prog.#2 ----- */

    if (*lpunMax == 0) {

        /* ----- Load String from Resource ----- */

        LoadString(hResourceDll, IDS_PEP_CAPTION_A50, szCap, PEP_STRING_LEN_MAC(szCap));
        LoadString(hResourceDll,     IDS_A50_FLEX_EMPTY,  szMsg, PEP_STRING_LEN_MAC(szMsg));

        /* ---- Display Caution Message ----- */

        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg,
                   szMsg,
                   szCap,
                   MB_OK | MB_ICONEXCLAMATION);

		//Return Cancel to indicate failure
        EndDialog(hDlg, LOWORD(IDCANCEL));

        return TRUE;
    }

    /* ----- Load Server File Name from Resorece ----- */

    LoadString(hResourceDll, IDS_FILE_NAME_SER, szCap, PEP_STRING_LEN_MAC(szCap));

    /* ----- Create Backup File for Cancel Key ----- */

    PepFileBackup(hDlg, szCap, PEP_FILE_BACKUP);

    /* ----- Read All Server No. ----- */

    unCur = WaiAllIdRead(sizeof(ausSerNo), ausSerNo);

    for (unI = 0; unI < unCur; unI++) {

        /* ----- Read Current Exists Server Name from Server File ----- */

        SerData.usWaiterNo = ausSerNo[unI];

        WaiIndRead(&SerData);

        /* ----- Insert Loaded server Name to ListBox ----- */

        A050InitList(hDlg, (LPWAIIF)&SerData);

    }

    /* ----- Set maximum server records to static text ----- */

    A050SetMaxRec(hDlg, *lpunMax);

    /* ----- Set current server records to static text ----- */

    A050SetCurRec(hDlg, unCur);

    /* ----- Disable Add/Change & Delete Button ----- */

    EnableWindow(GetDlgItem(hDlg, IDD_A50_ADDCHG), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDD_A50_DEL),    FALSE);

    /* ----- Set Fixed Font to EditText & ListBox ----- */

    PepSetFont(hDlg, IDD_A50_NAME);
    PepSetFont(hDlg, IDD_A50_LIST);

    /* ----- Limit Length of Input Data to Each EditText ----- */

    SendDlgItemMessage(hDlg, IDD_A50_NO,      EM_LIMITTEXT, A50_NO_LEN,      0L);
    SendDlgItemMessage(hDlg, IDD_A50_NAME,    EM_LIMITTEXT, A50_NAME_LEN, 0L);
    SendDlgItemMessage(hDlg, IDD_A50_GCSTART, EM_LIMITTEXT, A50_GC_LEN,      0L);
    SendDlgItemMessage(hDlg, IDD_A50_GCEND,   EM_LIMITTEXT, A50_GC_LEN,      0L);

    /* ----- Set Focus to ListBox ----- */

    SetFocus(GetDlgItem(hDlg, IDD_A50_LIST));

    /* ----- Check Delete Confirmation Button ----- */

    CheckDlgButton(hDlg, IDD_A50_CONF, (UINT)bConf);

    return (unCur);
}

/*
* ===========================================================================
**  Synopsis    :   USHORT  A050ReadMax()
*
**  Input       :   None
*
**  Return      :   USHORT  - address of maximum records
*
**  Description :
*       This function retrieves the number of sever records that user sets
*   in Program Mode # 2.
* ===========================================================================
*/
USHORT  A050ReadMax(VOID)
{
    PARAFLEXMEM FlexData;

    /* ----- Set Target Address to Server ----- */

    FlexData.uchAddress = FLEX_WT_ADR;

    /* ----- Read the Flexible Memory Data of Server ----- */

    ParaFlexMemRead(&FlexData);

    /* ----- Return the No. of Current Server ----- */

    /* return (FlexData.usRecordNumber);  NCR2172   */
    return ((USHORT)FlexData.ulRecordNumber);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A050InitList()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   LPWAIIF lpData  -   Address of Server Data
*
**  Return      :   No return value.
*
**  Description :
*       This procedure sets specified server data to listbox.
* ===========================================================================
*/
VOID    A050InitList(HWND hDlg, LPWAIIF lpData)
{
    WCHAR    szName[A50_MNE_LEN],
            szWork[A50_MNE_LEN];

    /* ----- Replace Double Width Key (0x12 => '~') ----- */

    PepReplaceMnemonic((WCHAR *)lpData->auchWaiterName,
                       (WCHAR *)szName,
                       (PARA_WAITER_LEN ),
                       PEP_MNEMO_READ);

    /* ----- Create String for Insert ListBox ----- */

    wsPepf(szWork, WIDE("%3u : %s"),
             lpData->usWaiterNo, szName);

    /* ----- Set data in buffer to listbox ----- */

    DlgItemSendTextMessage(hDlg,
                       IDD_A50_LIST,
                       LB_INSERTSTRING,
                       (WPARAM)-1,
                       (LPARAM)(szWork));

}

/*
* ===========================================================================
**  Synopsis    :   VOID    A050SetMaxRec()
*
**  Input       :   HWND    hDlg    -   handle of dialogbox
*                   USHORT  unMax   -   maximum server records
*
**  Return      :   No return value.
*
**  Description :
*       This procedure set maximum server records.
* ===========================================================================
*/
VOID    A050SetMaxRec(HWND hDlg, USHORT unMax)
{
    WCHAR    szWork[A50_MAX_LEN],
            szMax[A50_MAX_LEN];

    /* ----- Make maximum server eecords description ----- */

    LoadString(hResourceDll, IDS_A50_MAXSER, szMax, PEP_STRING_LEN_MAC(szMax));
    wsPepf(szWork, szMax, unMax);

    /* ----- Set description to static text ----- */

    DlgItemRedrawText(hDlg, IDD_A50_MAX, szWork);

}

/*
* ===========================================================================
**  Synopsis    :   VOID    A050SetCurRec()
*
**  Input       :   HWND    hDlg    -   handle of dialogbox
*                   USHORT  unCur   -   current server records
*
**  Return      :   No return value.
*
**  Description :
*       This procedure set current server records.
* ===========================================================================
*/
VOID    A050SetCurRec(HWND hDlg, USHORT unCur)
{
    WCHAR    szWork[A50_MAX_LEN],
            szCur[A50_MAX_LEN];

    /* ----- Make current server eecords description ----- */

    LoadString(hResourceDll, IDS_A50_CURSER, szCur, PEP_STRING_LEN_MAC(szCur));
    wsPepf(szWork, szCur, unCur);

    /* ----- Set description to static text ----- */

    DlgItemRedrawText(hDlg, IDD_A50_CUR, szWork);

}

/*
* ===========================================================================
**  Synopsis    :   VOID    A050SetData()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   LPWAIIF lpData  -   Address of Target server Record
*                   USHORT  unCur   -   No. of Current Server Record
*
**  Return      :   No return value.
*
**  Description :
*       This procedure sets target server data to each controls.
* ===========================================================================
*/
VOID    A050SetData(HWND hDlg, LPWAIIF lpData, USHORT unCur)
{
    DWORD   dwOff;
    BOOL    fChk;
    WCHAR    szBuff[A50_MNE_LEN];

    /* ----- Enable/Disable to Push Button ----- */

    EnableWindow(GetDlgItem(hDlg, IDD_A50_ADDCHG), (BOOL)unCur);
    EnableWindow(GetDlgItem(hDlg, IDD_A50_DEL),    (BOOL)unCur);

    if (unCur != 0) {   /* server Record Exists */

        /* ----- Get Selected Item Order from ListBox ----- */

        dwOff = SendDlgItemMessage(hDlg, IDD_A50_LIST, LB_GETCURSEL, 0, 0L);

        /* ----- Get Selected String from ListBox ----- */

        SendDlgItemMessage(hDlg,
                           IDD_A50_LIST,
                           LB_GETTEXT,
                           (WPARAM)dwOff,
                           (LPARAM)(szBuff));

        /* ----- Calculate Server No. ----- */

        lpData->usWaiterNo = (USHORT)_wtoi(szBuff);

        /* ----- Set Server No. to EditText ----- */

        SetDlgItemInt(hDlg, IDD_A50_NO, (UINT)lpData->usWaiterNo, FALSE);

        /* ----- Retrieve Target Server Data from Server File ----- */

        WaiIndRead((WAIIF *)lpData);

        /* ----- Set Training Mode Button ----- */

        fChk = (lpData->fbWaiterStatus & A50_TRAIN_CHK) ? TRUE : FALSE;
        CheckDlgButton(hDlg, IDD_A50_TRAIN, fChk);

        /* ----- Replace Double Key Code (0x12 => '~') ----- */

        PepReplaceMnemonic((WCHAR *)lpData->auchWaiterName,
                           (WCHAR *)szBuff,
                           (A50_NAME_LEN ),
                           PEP_MNEMO_READ);

        /* ----- Set server No. to EditText ----- */

        DlgItemRedrawText(hDlg, IDD_A50_NAME, szBuff);

        /* ----- Set GuestCheck No. to EditText ----- */

        SetDlgItemInt(hDlg,
                      IDD_A50_GCSTART,
                      (UINT)lpData->usGstCheckStartNo,
                      FALSE);
        SetDlgItemInt(hDlg,
                      IDD_A50_GCEND,
                      (UINT)lpData->usGstCheckEndNo,
                      FALSE);

    } else {            /* server record not exist */

        /* ----- Clear Each Controls ----- */

        SendDlgItemMessage(hDlg, IDD_A50_NO,      EM_SETSEL, 0, MAKELONG(0, -1));
        SendDlgItemMessage(hDlg, IDD_A50_NO,      WM_CLEAR,  0, 0L);
        SendDlgItemMessage(hDlg, IDD_A50_NAME,    EM_SETSEL, 0, MAKELONG(0, -1));
        SendDlgItemMessage(hDlg, IDD_A50_NAME,    WM_CLEAR,  0, 0L);
        CheckDlgButton(hDlg, IDD_A50_TRAIN, FALSE);
        SendDlgItemMessage(hDlg, IDD_A50_GCSTART, EM_SETSEL, 0, MAKELONG(0, -1));
        SendDlgItemMessage(hDlg, IDD_A50_GCSTART, WM_CLEAR,  0, 0L);
        SendDlgItemMessage(hDlg, IDD_A50_GCEND,   EM_SETSEL, 0, MAKELONG(0, -1));
        SendDlgItemMessage(hDlg, IDD_A50_GCEND,   WM_CLEAR,  0, 0L);

        /* ----- Set Focus to ListBox ----- */
    
        SetFocus(GetDlgItem(hDlg, IDD_A50_LIST));
    }
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A050GetData()
*
**  Input       :   HWND        hDlg    -   Window Handle of a DialogBox
*                   WORD        wID     -   Selected Button ID
*                   LPWAIIF     lpData  -   Address of Server Data Structure
*                   LPUSHORT    lpunCur -   Address of Current Server Records
*                   USHORT      unMax   -   Maximum Server Records
*
**  Return      :   BOOL    TRUE    -   Add/Change or Delete is Executed
*                           FALSE   -   Delete is not Executed
*
**  Description:
*       This procedure gets inputed data from each controls and set them to
*   buffer, and then saves them to server file, if user pressed Add/Change
*   button. Otherwise it deletes server data of specified server no. from
*   file.
*       It returns TRUE, if function was successful. Otherwise it returns
*   FALSE (User selects CANCEL button when delete confirmation is ON).
* ===========================================================================
*/
BOOL    A050GetData(HWND hDlg,
                    WPARAM wID,
                    LPWAIIF lpData,
                    LPUSHORT lpunCur,
                    USHORT unMax)
{
    BOOL    fRet = TRUE;
    WCHAR    szWork[PEP_LOADSTRING_LEN];

    /* ----- Get Inputed Server No. from EditText ----- */

    lpData->usWaiterNo = (USHORT)GetDlgItemInt(hDlg, IDD_A50_NO, NULL, FALSE);

    /* ----- Check No. of Current Record is less than Max. Records ----- */

    if (A050ChkMax(hDlg, wID, lpData->usWaiterNo, *lpunCur, unMax) == FALSE) {

        /* ----- Check Whether Start/End G.C. No. is valid or Not ----- */

        if (A050GetGCNo(hDlg, (LPWAIIF)lpData) == TRUE) {

            /* ----- Get Server Name from EditText ----- */

            memset(szWork, 0, sizeof(szWork));
            DlgItemGetText(hDlg, IDD_A50_NAME, szWork, PEP_STRING_LEN_MAC(szWork));

            /* ----- Replace Double Key Code ('~' => 0x12) ----- */

            PepReplaceMnemonic((WCHAR *)szWork,
                               (WCHAR *)lpData->auchWaiterName,
                               (PARA_WAITER_LEN ),
                               PEP_MNEMO_WRITE);

            /* ----- Get status(trainning mode) ----- */

            if (IsDlgButtonChecked(hDlg, IDD_A50_TRAIN) == TRUE) {
                lpData->fbWaiterStatus |= A50_TRAIN_CHK;
            } else {
                lpData->fbWaiterStatus &= ~A50_TRAIN_CHK;
            }

            /* ----- Refresh ListBox ----- */

            if (A050RefreshList(hDlg, (LPWAIIF)lpData, (LPUSHORT)lpunCur, wID) == TRUE) {

                /* ----- User Select Cancel for Delete Cashier Data ----- */

                fRet = FALSE;

            } else {

                if (wID == IDD_A50_ADDCHG) {

                    /* ----- Add/Change Record to Server File ----- */

                    WaiAssign(lpData);

                } else {

                    /* ----- Delete Record ----- */

                    WaiDelete(lpData->usWaiterNo);

                }
            }
        }
    }
    return (fRet);
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A050ChkMax()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wParam  -   User Selected Button ID
*                   USHORT  unNo    -   Current Maintenance Server No.
*                   USHORT  unCur   -   No. of Current Server Records
*                   USHORT  unMax   -   No. of Maximum Server Records
*
**  Return      :   BOOL    TRUE    -   Over Maximum Server Records
*                           FALSE   -   Not Over Miximum Server Records
*
**  Description :
*       This procedure determines whether number of current cashier records
*   over than maximum cashier records.
*       It returns TRUE, if number of cashier records over than maximum cashier
*   records. Otherwise it returns FALSE.
* ===========================================================================
*/
BOOL    A050ChkMax(HWND hDlg,
                   WPARAM wParam,
                   USHORT unNo,
                   USHORT unCur,
                   USHORT unMax)
{
    WCHAR    szCap[PEP_CAPTION_LEN],
            szMsg[PEP_LOADSTRING_LEN];
    DWORD   dwIndex;
    BOOL    fRet = FALSE;

    if ((LOWORD(wParam) == IDD_A50_ADDCHG) && (unCur == unMax)) {
                                                    /* check over maximum */

        /* ----- Search Corresponding Data from Current Server Data ----- */

        dwIndex = A050FindNo(hDlg, (UINT)unNo);

        if (dwIndex == LB_ERR) {            /* add new data */

            /* ----- Load String from Resource ----- */

            LoadString(hResourceDll, IDS_PEP_CAPTION_A50, szCap, PEP_STRING_LEN_MAC(szCap));
            LoadString(hResourceDll, IDS_A50_OVERRECORD,  szMsg, PEP_STRING_LEN_MAC(szMsg));

            /* ----- Display Error Message with MessageBox ----- */

            MessageBeep(MB_ICONEXCLAMATION);
            MessageBoxPopUp(hDlg, szMsg, szCap,
                       MB_OK | MB_ICONEXCLAMATION);

            /* ----- Set Selected server No. to EditText ----- */

            SendDlgItemMessage(hDlg, IDD_A50_NO, EM_SETSEL, 1, MAKELPARAM(0, -1));

            fRet = TRUE;
        }
    }
    return (fRet);
}

/*
* ===========================================================================
**  Synopsis    :   DWORD   A050FindNo()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*                   UINT    unSerNo -   Target Server No.
*
**  Return      :   DWORD   dwIndex -   Zero Based Index No. of ListBox
*
**  Description:
*       This function is looking for the specified server number in listbox.
*   It returns the zero based index of listbox, if the specified server number
*   is found. Otherwise it returns LB_ERR.
* ===========================================================================
*/
DWORD   A050FindNo(HWND hDlg, UINT unSerNo)
{
    WCHAR    szTargetNo[PEP_LOADSTRING_LEN];
    DWORD   dwIndex;

    /* ----- Make the String for Search for Target Server No. ---- */

    wsPepf(szTargetNo, WIDE("%3u : "), unSerNo);

    /* ----- Search Corresponding Data from Current Server Data ----- */
		//SR 445 Fix.  Although the SR calls for AC 152, I discovered that most
		//dialog messages that use the LB_FINDSTRING, we have made a new function 
		//called DlgItemSendTextMessage which now passes the information through 
		//the unicode utilities to SendDlgItemMessage 
    dwIndex = DlgItemSendTextMessage(hDlg,
                                 IDD_A50_LIST,
                                 LB_FINDSTRING,
                                 (WPARAM)-1,
                                 (LPARAM)(szTargetNo));

    return (dwIndex);
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A050GetGCNo()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*                   LPWAIIF lpData    -   Address of Server Data Structure
*
**  Return      :   BOOL    TRUE        -   GC No. is valid data.
*                           FALSE       -   GC No. is illeagal.
*
**  Description :
*       This function gets the guest check start/end No. from each edittext.
*   And then it checks whether the start No. is less than the end No. or not,
*   and the checks the end no. is 0 when the start no. is 0.
*       It returns FALSE, if an error occured. Otherwise it returns TRUE.
* ===========================================================================
*/
BOOL    A050GetGCNo(HWND hDlg, LPWAIIF lpData)
{
    BOOL    fRet = TRUE;
    UINT    wStrID = 0;
    WORD    wEditID = 0;
    USHORT  unStart,
            unEnd;
    WCHAR    szCap[PEP_CAPTION_LEN],
            szMsg[PEP_LOADSTRING_LEN];

    /* ----- Get Start / End GC No. from Each EditText ----- */

    unStart = (USHORT)GetDlgItemInt(hDlg, IDD_A50_GCSTART, NULL, FALSE);
    unEnd   = (USHORT)GetDlgItemInt(hDlg, IDD_A50_GCEND,   NULL, FALSE);

    /* ----- Determine Whether Start No. is less than End No. or Not ----- */
    
    if (unStart > unEnd) {

        wStrID  = IDS_A50_OVERGCEND;
        wEditID = IDD_A50_GCSTART;

        fRet = FALSE;

    /* ----- Determine End No. is not 0, when Start No. is 0 ----- */

    } else if ((! unStart) && (unEnd)) {

        wStrID  = IDS_A50_GCEND_0;
        wEditID = IDD_A50_GCEND;

        fRet = FALSE;
    }

    if (fRet == FALSE) {

        /* ----- Load String from Resource ----- */
        
        LoadString(hResourceDll, IDS_PEP_CAPTION_A50, szCap, PEP_STRING_LEN_MAC(szCap));
        LoadString(hResourceDll, wStrID, szMsg, PEP_STRING_LEN_MAC(szMsg));

        /* ----- Display Error Message with MessageBox ----- */

        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg, szMsg, szCap, MB_OK | MB_ICONEXCLAMATION);

        /* ----- Set Focus to Error Occured EditText ----- */

        SetFocus(GetDlgItem(hDlg, wEditID));
        SendDlgItemMessage(hDlg, wEditID, EM_SETSEL, 1, MAKELPARAM(0, -1));

    } else {
    
        /* ----- Save Start/End GC No. to Buffer ----- */

        lpData->usGstCheckStartNo = unStart;
        lpData->usGstCheckEndNo   = unEnd;
    }
    return fRet;
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A050RefreshList()
*
**  Input       :   HWND        hDlg    -   Window Handle of a DialogBox
*                   LPWAIIF     lpData  -   Address of Server Structure
*                   LPUSHORT    lpunCur -   Address of No. of Current Records
*                   WORD        wParam  -   User Selected Button ID
*
**  Return      :   TRUE    -   Delete is Fault
*                   FALSE   -   Add/Change or Delete is Executed
*
**  Description :
*       This procedure reset contents of listbox. It returns FALSE (User selects
*   CANCEL button when delete confirmation is ON). Otherwise it returns TRUE.
* ===========================================================================
*/
BOOL    A050RefreshList(HWND hDlg,
                        LPWAIIF lpData,
                        LPUSHORT lpunCur,
                        WPARAM wParam)
{
    BOOL    fChk = FALSE;
    DWORD   dwIndex;
    WCHAR    szWork[PEP_LOADSTRING_LEN],
            szBuff[PEP_LOADSTRING_LEN];

    /* ----- Search corresponding data from current server data ----- */

    dwIndex = A050FindNo(hDlg, (UINT)lpData->usWaiterNo);

    /* ----- Replace Double Width Key (0x12 => '~') ----- */

    PepReplaceMnemonic((WCHAR *)lpData->auchWaiterName,
                       (WCHAR *)szBuff,
                       (A50_NAME_LEN ),
                       PEP_MNEMO_READ);

    /* ----- make description for insert listbox ----- */

    wsPepf(szWork, WIDE("%3u : %s"),
             lpData->usWaiterNo, szBuff);

    if (dwIndex == LB_ERR) {
        if (wParam == IDD_A50_ADDCHG) { /* add record */

            /* ----- Insert New Data into ListBox ----- */

            DlgItemSendTextMessage(hDlg,
                               IDD_A50_LIST,
                               LB_INSERTSTRING,
                               (WPARAM)-1,
                               (LPARAM)(szWork));

            /* ----- Set Cursor to Inserted String ----- */

            SendDlgItemMessage(hDlg,
                               IDD_A50_LIST,
                               LB_SETCURSEL,
                               (WPARAM)*lpunCur,
                               0L);

            /* ----- Reset Secret Code ----- */

            lpData->uchWaiterSecret = 0;

            /* ----- Enable Delete Button ----- */

            EnableWindow(GetDlgItem(hDlg, IDD_A50_DEL), TRUE);

            /* ----- Set Check Status ----- */

            fChk = TRUE;
        }

    } else {
        if (wParam == IDD_A50_DEL) {    /* delete record */

            /* ----- Check Delete Confirmation Flag ----- */

            if (A050DispDelConf(hDlg, lpData->usWaiterNo) == IDOK) {

                /* ----- Delete Target String drom ListBox ----- */

                SendDlgItemMessage(hDlg,
                                   IDD_A50_LIST,
                                   LB_DELETESTRING,
                                   (WPARAM)dwIndex,
                                   0L);

                /* ----- Adjust ListBox Index ----- */

                dwIndex = (DWORD)(! dwIndex) ? (dwIndex  + 1) : (dwIndex);

                /* ----- Set HighLight Bar to Next Item of ListBox ----- */

                SendDlgItemMessage(hDlg,
                                   IDD_A50_LIST,
                                   LB_SETCURSEL,
                                   (WPARAM)(dwIndex - 1),
                                   0L);

                /* ----- Set Check Status ----- */

                fChk = TRUE;

            } else {

                return TRUE;
            }

        } else {                        /* change record */

            /* ----- Delete Old String from ListBox ----- */

            SendDlgItemMessage(hDlg,
                               IDD_A50_LIST,
                               LB_DELETESTRING,
                               (WPARAM)dwIndex,
                               0L);

            /* ----- Insert New Description to ListBox ----- */

            DlgItemSendTextMessage(hDlg,
                               IDD_A50_LIST,
                               LB_INSERTSTRING,
                               (WPARAM)dwIndex,
                               (LPARAM)(szWork));

            /* ----- Set Cursor to Inserted String ----- */

            SendDlgItemMessage(hDlg,
                               IDD_A50_LIST,
                               LB_SETCURSEL,
                               (WPARAM)dwIndex,
                               0L);

            /* ----- Set Check Status ----- */

            fChk = FALSE;
        }
    }

    if (fChk == TRUE) {     /* ListBox is Updated   */

        /* ----- Compute the Number of Current Server Records ----- */

        if (wParam == IDD_A50_ADDCHG) { /* Add/Change Button is Clicked */
            (*lpunCur)++;
        } else {                        /* Delete Button is Clicked     */
            (*lpunCur)--;
        }

        /* ----- Set No. of Current Server Records ----- */

        LoadString(hResourceDll, IDS_A50_CURSER, szBuff, PEP_STRING_LEN_MAC(szBuff));
        wsPepf(szWork, szBuff, *lpunCur);
        DlgItemRedrawText(hDlg, IDD_A50_CUR, szWork);

    }
    return FALSE;
}

/*
* ===========================================================================
**  Synopsis    :   int A050DispDelConf()
*
**  Input       :   HWND    hdlg    -   Window Handle of a DailogBox
*                   USHORT  usNo    -   Current Selected server No.
*
**  Return      :   IDOK        -   User Selection is OK
*                   IDCANCEL    -   User Selection is CANCEL
*
**  Description :
*       This function shows confirmation message for delete server record.
*   It returns IDOK, if user selects server data deletion. Otherwise it 
*   return FALSE.
* ===========================================================================
*/
int A050DispDelConf(HWND hdlg, USHORT usNo)
{
    WCHAR    szCap[PEP_CAPTION_LEN],
            szMsg[PEP_OVER_LEN],
            szWork[PEP_OVER_LEN];
    int     nRet;

    if (bConf & A50_CONF_CHK) {         /* Confirmation is ON */

        /* ----- Load Caution Message from Resource ----- */

        LoadString(hResourceDll, IDS_PEP_CAPTION_A50, szCap, PEP_STRING_LEN_MAC(szCap));
        LoadString(hResourceDll, IDS_A50_DELCONF, szWork, PEP_STRING_LEN_MAC(szWork));
        wsPepf(szMsg, szWork, usNo);

        /* ----- Display Caution Message ----- */

        MessageBeep(MB_ICONEXCLAMATION);
        nRet = MessageBoxPopUp(hdlg,
                          szMsg,
                          szCap,
                          MB_OKCANCEL | MB_ICONEXCLAMATION);

    } else {                            /* Confirmation is OFF */

        /* ----- No Confirmation ----- */

        nRet = IDOK;

    }
    return (nRet);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A050ChkExist()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value
*
**  Description :
*       This procedure controls push button status. If cashier no. exists in
*   edittext, it enables Add/Change button, otherwise it disables. And if
*   target cashier data is found in listbox, it enables Delete button, 
*   otherwise it disables.
* ===========================================================================
*/
VOID    A050ChkExist(HWND hDlg)
{
    UINT    unNo;
    BOOL    fChk,
            fTrans;

    /* ----- Get Cashier No. from EditText ----- */

    unNo = GetDlgItemInt(hDlg, IDD_A50_NO, (LPBOOL)&fTrans, FALSE);

    if ((! unNo) || (fTrans == 0)) {

        fTrans = FALSE;
        unNo   = 0;

    } else {

        fTrans = TRUE;
    }

    /* ----- Search Corresponding Data from ListBox ----- */

    fChk = (A050FindNo(hDlg, unNo) == LB_ERR) ? FALSE : TRUE;

    /* ----- Enable/Disable to Add/Change Button ----- */

    EnableWindow(GetDlgItem(hDlg, IDD_A50_ADDCHG), fTrans);
    EnableWindow(GetDlgItem(hDlg, IDD_A50_DEL),    fChk);

}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A050ChkGCData
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   BOOL    TRUE    -   GC No. is Valid.
*                           FALSE   -   GC No. is Invalid.
*
**  Description :
*       This procedure determines whether the inputed GC No is valid or not.
*   If GC No is invalid, it displays error message. And it returns TRUE.
*   Otherwise it returns FALSE.
* ===========================================================================
*/
BOOL    A050ChkGCData(HWND hDlg)
{
    WCHAR    szCap[128],
            szDesc[128],
            szWork[128];
    UINT    unStart,
            unEnd;
    WORD    wID;
    BOOL    fRet = TRUE,
            fStart,
            fEnd;

    /* ----- Get Inputed GC No from EditText ----- */

    unStart = GetDlgItemInt(hDlg, IDD_A50_GCSTART, (LPBOOL)&fStart, FALSE);
    unEnd   = GetDlgItemInt(hDlg, IDD_A50_GCEND,   (LPBOOL)&fEnd,   FALSE);

    /* ----- Check Inputed Data is Valid or Not ----- */

    if ((fStart == 0) || (fEnd == 0)) {

        /* ----- Load Error Message from Resource ----- */

        LoadString(hResourceDll, IDS_PEP_CAPTION_A50, szCap, PEP_STRING_LEN_MAC(szCap));

        LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_STRING_LEN_MAC(szWork));

        wsPepf(szDesc, szWork, A50_GC_MIN, A50_GC_MAX);

        /* ----- Display Error Message ----- */

        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg,
                   szDesc, szCap, MB_OK | MB_ICONEXCLAMATION);

        /* ----- Set Input Focus to EditText ----- */

        wID = (WORD)((fStart == 0) ? IDD_A50_GCSTART : IDD_A50_GCEND);

        SendDlgItemMessage(hDlg, wID, EM_SETSEL, TRUE, MAKELONG(0, -1));
        SetFocus(GetDlgItem(hDlg, wID));

        fRet = FALSE;
    }
    return (fRet);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A050FinDlg()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wID     -   User Selected Button ID
*
**  Return      :   No return value.
*
**  Description :
*       This procedure sets file modify flag and deletes backupfile, if
*   user selects OK Button. Otherwise it stores server file with backup
*   file and then deletes backup file.
* ===========================================================================
*/
VOID    A050FinDlg(HWND hDlg, WPARAM wID)
{
    WCHAR    szFile[PEP_CAPTION_LEN];
    HCURSOR hCursor;

    /* ----- Change cursor (arrow -> hour-glass) ----- */

    hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
    ShowCursor(TRUE);

    if (wID == IDOK) {

        /* ----- Set File Status Flag ----- */

        PepSetModFlag(hwndActMain, PEP_MF_SERV, 0);

    } else {

        /* ----- Load Cashier File Name from Resorece ----- */

        LoadString(hResourceDll, IDS_FILE_NAME_SER, szFile, PEP_STRING_LEN_MAC(szFile));

        /* ----- Restore Cashier File with Backup File ----- */

        PepFileBackup(hDlg, szFile, PEP_FILE_RESTORE);
    }

    /* ----- Delete Backup File ----- */

    PepFileDelete();

    /* ----- Restore cursor (hour-glass -> arrow) ----- */

    ShowCursor(FALSE);
    SetCursor(hCursor);

    /* ----- Destroy DialogBox ----- */

    EndDialog(hDlg, LOWORD(wID));
}

/* ===== End of A050.C ===== */
