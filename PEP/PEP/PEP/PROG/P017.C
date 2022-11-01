/*
* ---------------------------------------------------------------------------
* Title         :   Hourly Activity Block (Prog. 17)
* Category      :   Setup, NCR 2170 PEP for Windows (HP US Model)
* Program Name  :   P017.C
* Copyright (C) :   1995, AT&T GIS Corp. E&M-OISO, All rights reserved.
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
* Sep-21-95 : 01.00.03 : T.Nakahata : correct mouse cursour for Windows 95
* Sep-22-98 : 03.03.00 : A.Mitsui   : V3.3 (increase block 24->49)
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

#include    "prog.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "p017.h"


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
static BYTE      abData[P17_ADDR_MAX][P17_HM_MAX];
                                                /* Inputed Data Area        */
static WORD      wPrevID;                       /* Previous Selected ID     */
static RECT      rTable,                        /* Time Table Redraw Area   */
                 rTri;                          /* Triangle Cursor Area     */
static P17LINE   aLineStatus[P17_SEGMENT_MAX];  /* Line Status Structure    */
static P17CURSOR aCursor[P17_SEGMENT_MAX];      /* Cursor Status Structure  */
static BOOL      fCapture;                      /* Mouse Capture Flag       */
static WORD      xDBU,  /* dialog base unit, x coordinate */
                 yDBU;  /* dialog base unit, y coordinate */

/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  P017DlgProc()
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
*               Hourly Activity Block   (Program Mode # 17)
* ===========================================================================
*/
BOOL    WINAPI  P017DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  WORD    wIndex;     /* Current Selected Index of ComboBox   */
    static  POINTS   ptDown;     /* Left-Button Down Position            */
    static  RECT    rc;

    int     yCap,
            xFrame,
            xMouse,
            yMouse;
    POINTS  ptMousePos;         /* Current Mouse Position               */
    USHORT  usReturnLen;        /* Length of Data Buffer                */
    BOOL    fRet;               /* Return Value of Local Func.          */

    switch (wMsg) {

    case WM_INITDIALOG:

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- Initialize DialogBox ----- */

        fRet = P17InitDlg(hDlg);

        wIndex = P17_DATA_MIN;

        return (fRet);

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=IDD_P17_BLK; j<=IDD_BLOCK_RNG; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
	
		}
		return FALSE;

    case WM_PAINT:

        /* ----- Display Taime Table & Triangle Cursor ----- */

        wIndex = (WORD)SendDlgItemMessage(hDlg, IDD_P17_COMB, CB_GETCURSEL, 0, 0L);

        P17DrawTime(hDlg, abData[P17_BLOCK_ADDR][P17_HOUR_ADDR], wIndex);

        return TRUE;
        
    case WM_LBUTTONDOWN:             /* left mouse button is down */

        if (abData[P17_BLOCK_ADDR][P17_HOUR_ADDR]) {

            /* ----- Calculate Button Down Position -----*/

            ptDown = MAKEPOINTS(lParam);

            ptDown.x = ptDown.x *  8 / xDBU;
            ptDown.y = ptDown.y * 16 / yDBU;

            if (P17ChkDownPos(hDlg, (PPOINTS)&ptDown, (LPWORD)&wIndex)){

                xFrame = GetSystemMetrics(SM_CXDLGFRAME);
                yCap   = ( GetSystemMetrics(SM_CYCAPTION) +
                            GetSystemMetrics(SM_CYDLGFRAME));

                GetWindowRect(hDlg, (LPRECT)&rc);

                yMouse    = P17_MOUSEY_MAX * yDBU / 16;
                xMouse    = P17_MOUSEX_MAX * xDBU /  8;

                rc.bottom = rc.top  + yMouse + yCap;
                rc.right  = rc.left + xMouse + xFrame;

                yMouse    = P17_MOUSEY_MIN * yDBU / 16;
                xMouse    = P17_MOUSEX_MIN * xDBU /  8;

                rc.top    += (yMouse + yCap);
                rc.left   += (xMouse + xFrame);

                ClipCursor((const LPRECT)&rc);

                SetCapture(hDlg);

                fCapture = TRUE;
            }
        }
        return TRUE;

    case WM_LBUTTONUP:               /* left mouse button is up */

        if (fCapture == TRUE) {

            ClipCursor(NULL);

            ReleaseCapture();

            fCapture = FALSE;

            /* ----- Calculate Button Up Position -----*/

            ptMousePos = MAKEPOINTS(lParam);

            ptMousePos.x = ptMousePos.x *  8 / xDBU;
            ptMousePos.y = ptMousePos.y * 16 / yDBU;

            if (P17ChkPos((PPOINTS)&ptMousePos)) {

                PostMessage(hDlg, WM_COMMAND, MAKELONG(IDD_P17_SET, BN_CLICKED), 0L);
            }
            return TRUE;
        }
        return FALSE;

    case WM_MOUSEMOVE:              /* mouse is moving */

        if (fCapture == TRUE) {

            /* ----- Calculate Button Up Position -----*/

            ptMousePos = MAKEPOINTS(lParam);

            ptMousePos.x = ptMousePos.x *  8 / xDBU;
            ptMousePos.y = ptMousePos.y * 16 / yDBU;

            if (P17ChkPos((PPOINTS)&ptMousePos)) {

                /* ----- Display Triangle Cursor ----- */

                P17DispMove(hDlg, (PPOINTS)&ptMousePos, wIndex);

            } else {

                /* ----- Display Previous Triangle Cursor ----- */

                P17DispPre(hDlg);
            }
            return TRUE;
        }
        return FALSE;

    case WM_VSCROLL:

        P17ScrlProc(hDlg, wParam, lParam);

        /* ----- An application should return zero ----- */

        return FALSE;

    case WM_COMMAND:

        if ((wPrevID == IDD_P17_BLK) || (wPrevID == IDD_P17_BLKSCR)) {

            /* ----- Check Current Control ID ----- */

            P17ChkCtrlID(hDlg, LOWORD(wParam), abData[P17_BLOCK_ADDR][P17_HOUR_ADDR]);
        }

        switch (LOWORD(wParam)) {

        case IDD_P17_COMB:

            if (HIWORD(wParam) == CBN_SELCHANGE) {

                /* ----- Reset Time Table & Cursor, and EditText ----- */

                wIndex = P17SelCombo(hDlg, wIndex);

                wPrevID = LOWORD(wParam);

                return TRUE;
            }    
            return FALSE;

        case IDD_P17_BLK:
        case IDD_P17_HH:
        case IDD_P17_MM:

            if (HIWORD(wParam) == EN_CHANGE) {

                /* ----- Check Inputed Value ----- */

                P17ChkData(hDlg, LOWORD(wParam));

                wPrevID = LOWORD(wParam);

                return TRUE;
            }
            return FALSE;

        case IDD_P17_ENTER:

            if (HIWORD(wParam) == BN_CLICKED) {

                /* ---- Reset No. of Time Blocks ----- */

                P17ResetBlk(hDlg);

                wPrevID = LOWORD(wParam);

                return TRUE;
            }
            return FALSE;

        case IDD_P17_SET:

            if (HIWORD(wParam) == BN_CLICKED) {

                /* ----- Set Time Segment Data ----- */

                P17SetTime(hDlg);

                wPrevID = LOWORD(wParam);

                return TRUE;
            }
            return FALSE;

        case IDOK:
        case IDCANCEL:

            if (HIWORD(wParam) == BN_CLICKED) {

                if (LOWORD(wParam) == IDOK) {

                    /* ----- Write Hourly Data to Parameter File ----- */

                    ParaAllWrite(CLASS_PARAHOURLYTIME,
                                 (UCHAR *)abData,
                                 sizeof(abData),
                                 0, &usReturnLen);

                    PepSetModFlag(hwndProgMain, PEP_MF_PROG, 0);
                }

                EndDialog(hDlg, FALSE);

                ClipCursor(NULL);

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
**  Synopsis    :   BOOL    P17InitDlg()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   BOOL    TRUE    -   Input Focus is EditText (No. of Blocks)
*                           FALSE   -   Input Focus is ComboBox
*
**  Description :
*       This Function initilizes each Controls of DialogBox.
* ===========================================================================
*/
BOOL    P17InitDlg(HWND hDlg)
{
    USHORT  usReturnLen;            /* Size of Read Buffer  */
    short   nCount,                 /* Counter              */
            nHour,                  /* Hour Data            */
            nMinute;                /* Minute Data          */
    WCHAR    szMin[P17_MINLEN];      /* Minute Strings       */
    BOOL    fRet = TRUE;            /* Return Value         */
    DWORD   dwDBU;

    dwDBU = GetDialogBaseUnits();
    xDBU = LOWORD( dwDBU );
    yDBU = HIWORD( dwDBU );

    /* ----- Read Initial Data from Parameter File ----- */

    ParaAllRead(CLASS_PARAHOURLYTIME,
                (UCHAR *)abData,
                sizeof(abData),
                0, &usReturnLen);

    for (nCount = 0; nCount < P17_SEGMENT_MAX; nCount++) {

        /* ----- Translate Hourly Data to Minute ( 1:30 -> 90 min) ----- */

        nMinute = P17_CALCMINUTE(abData[nCount + 1][P17_HOUR_ADDR],
                                 abData[nCount + 1][P17_MIN_ADDR]);

        /* ----- Calculate Line & Cursor Position ----- */

        P17SetLinePos((WORD)nCount, nMinute);
    }

    for (nCount = IDD_P17_BLK; nCount <= IDD_P17_MM; nCount++) {

        /* ----- Set Limit Length to EditText ----- */

        SendDlgItemMessage(hDlg,
                           nCount,
                           EM_LIMITTEXT,
                           (WPARAM)P17_DATA_LEN,  0L);
    }

    /* ----- Set Initial Description to ComboBox ----- */

    P17InitCombo(hDlg);

    /* ----- Set No. of Activity Blocks Data to EditText ----- */

    SetDlgItemInt(hDlg,
                  IDD_P17_BLK,
                  (UINT)abData[P17_BLOCK_ADDR][P17_HOUR_ADDR],
                  FALSE);

    if (abData[P17_BLOCK_ADDR][P17_HOUR_ADDR]) {

        /* ----- Get Hour & Minute Data from Buffer ----- */

        nHour   = (short)abData[P17_TIME1_ADDR][P17_HOUR_ADDR];
        nMinute = (short)abData[P17_TIME1_ADDR][P17_MIN_ADDR];

        wsPepf(szMin, WIDE("%02d"), nMinute);
        
        /* ----- Show Time Segment No.1 Data ----- */

        SetDlgItemInt(hDlg, IDD_P17_HH, (UINT)nHour, FALSE);

        DlgItemRedrawText(hDlg, IDD_P17_MM, szMin);

        /* ----- Disable Enter Button ----- */

        EnableWindow(GetDlgItem(hDlg, IDD_P17_ENTER), FALSE);

        /* ----- Set Input Focus to ComboBox ----- */

        SetFocus(GetDlgItem(hDlg, IDD_P17_COMB));

        fRet = FALSE;

    } else {

        /* ----- Disable Set Button ----- */

        EnableWindow(GetDlgItem(hDlg, IDD_P17_SET), FALSE);

        /* ----- Set Input Focus to EditText (No. of Blocks) ----- */

        SendDlgItemMessage(hDlg, IDD_P17_BLK, EM_SETSEL, 1, MAKELONG(0, -1));
    }
    return fRet;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P17InitCombo()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return values
*
**  Description :
*       This function deletes the strings of combobox, if they exists. And then
*   it inserts the new strings into combobox, and sets data of the top index in
*   combobox to edittext.
* ===========================================================================
*/
VOID    P17InitCombo(HWND hDlg)
{
    short   nCount,                 /* Loop Counter             */
            nNoBlock;               /* No. of Time Blocks       */
    WCHAR    szDesc[P17_DESC_LEN],   /* ComboBox Description     */
            szWork[P17_DESC_LEN];   /* Description Work Area    */

    /* ----- Reset Contents of ComboBox ----- */

    SendDlgItemMessage(hDlg, IDD_P17_COMB, CB_RESETCONTENT, 0, 0L);

    nNoBlock = (short)abData[P17_BLOCK_ADDR][P17_HOUR_ADDR];

    for (nCount = 0; nCount < nNoBlock; nCount++) {

        /* ----- Set Line & Cursor Color to White ----- */

        aLineStatus[nCount].chColor = P17_WHITE;
        aCursor[nCount].chColor     = P17_BLACK;

        /* ----- Set Description to ComboBox ----- */

        LoadString(hResourceDll, IDS_P017_NUMBER, szWork, PEP_STRING_LEN_MAC(szWork));

        wsPepf(szDesc, szWork, (nCount + 1));

		DlgItemSendTextMessage(hDlg, IDD_P17_COMB, CB_INSERTSTRING,
                           (WPARAM)-1, (LPARAM)(szDesc));

        /*SendDlgItemMessage(hDlg,
                           IDD_P17_COMB,
                           CB_INSERTSTRING,
                           (WPARAM)-1,
                           (LPARAM)((LPCSTR)szDesc));*/
    }

    if (nNoBlock) {

        SendDlgItemMessage(hDlg, IDD_P17_COMB, CB_SETCURSEL, 0, 0L);
    }

    /* ----- Set Line Color to Yellow ----- */

    aLineStatus[0].chColor = P17_YELLOW;
    aCursor[0].chColor     = P17_YELLOW;
}

/*
* ===========================================================================
**  Synopsis    :   short   P17SelCombo()
*
**  Input       :   HWND    hDlg    -   Window Handle of a Dialogbox
*                   WORD    wIndex  -   Previous Selected Index of ComboBox
*
**  Return      :   New Selected Index of ComboBox
*
**  Description :
*       This Function gets current selected index of combobox, and reset
*   time table and cursor, edittext.
* ===========================================================================
*/
WORD    P17SelCombo(HWND hDlg, WORD wIndex)
{
    short   nHour,              /* Current Selected Hour Data   */
            nMinute;            /* Current Selected Minute Data */
    WCHAR   szMin[P17_MINLEN];  /* Strings for Text Out         */

    /* ----- Set Line Color of Previous Index to White ----- */

    aLineStatus[wIndex].chColor = P17_WHITE;
    aCursor[wIndex].chColor     = P17_BLACK;

    /* ----- Get Current Selected Index  of ComboBox ----- */

    wIndex = (WORD)SendDlgItemMessage(hDlg, IDD_P17_COMB, CB_GETCURSEL, 0, 0L);

    /* ----- Get Time Data of Current Selected Address ----- */

    nHour   = (short)abData[wIndex + 1][P17_HOUR_ADDR];
    nMinute = (short)abData[wIndex + 1][P17_MIN_ADDR];
    
    /* ----- Set Hour & Minute Data to EditText ----- */

    SetDlgItemInt(hDlg, IDD_P17_HH, (UINT)nHour, FALSE);

    wsPepf(szMin, WIDE("%02d"), nMinute);


    DlgItemRedrawText(hDlg, IDD_P17_MM, szMin);

    /* ----- Set Line Color to Yellow ----- */

    aLineStatus[wIndex].chColor = P17_YELLOW;
    aCursor[wIndex].chColor     = P17_YELLOW;

    /* ----- Redraw Time Table Data using 'WM_PAINT' Message ----- */

    InvalidateRect(hDlg, (const LPRECT)&rTable, TRUE);

    return (wIndex);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P17SetLinePos()
*
**  Input       :   WORD    wIndex  -   Current Selected Index
*                   short   nMinute -   Minute Data
*
**  Return      :   No return value
*
**  Description :
*       This Function calculates line and cursor position with Minute data.
* ===========================================================================
*/
VOID    P17SetLinePos(WORD wIndex, short nMinute)
{
    /* ----- Calculate Line Position and Set Them(to Structure ----- */

    aLineStatus[wIndex].xPosition = P17_CALCPOS(nMinute);

    /* ----- Calculate Cursor Position ind Set Them to Structure ----- */

    aCursor[wIndex].ptTri[P17_TRIPOS1].x = aLineStatus[wIndex].xPosition;
    aCursor[wIndex].ptTri[P17_TRIPOS2].x = aLineStatus[wIndex].xPosition
                                            - P17_TRIX_LEN;
    aCursor[wIndex].ptTri[P17_TRIPOS3].x = aLineStatus[wIndex].xPosition
                                            + P17_TRIX_LEN;

    aCursor[wIndex].ptTri[P17_TRIPOS1].y = P17_BLACK_BOTTOM;
    aCursor[wIndex].ptTri[P17_TRIPOS2].y = P17_TRI_Y;
    aCursor[wIndex].ptTri[P17_TRIPOS3].y = P17_TRI_Y;
}


/*
* ===========================================================================
**  Synopsis    :   VOID    P17ChkData()
*
**  Input       :   HWND    hDlg    - Window Handle of a Dialogbox
*                   WORD    wParam  - Control ID of Current EditText
*
**  Return      :   No return values
*
**  Description :
*       This function checks inputed value in edittext. If out of range error
*   occurred, undo data input. 
*       Enable ENTER button, if user inputed in No. of Blocks (IDD_P17_BLK).
* ===========================================================================
*/
VOID    P17ChkData(HWND hDlg, WORD wEditId)
{
    BOOL    fCheck;                     /* Check Data Range Flag    */
    UINT    unValue,                    /* Current Inputed Value    */
            unMin,                      /* Min. Value of Data Range */
            unMax;                      /* Max. Value of Data Range */
    WCHAR   szWork[P17_ERR_LEN],        /* Error Message Work Area  */
            szMessage[P17_ERR_LEN],     /* Over Range Error Message */
            szCaption[P17_CAP_LEN];     /* Caption for MessageBox   */

    /* ----- Set Minimum & Maximum of Data Range ----- */

    unMin = P17_DATA_MIN;

    unMax = (UINT)((wEditId == IDD_P17_BLK) ? P17_SEGMENT_MAX:
                  ((wEditId == IDD_P17_HH)  ? P17_HH_MAX:
                                             P17_MM_MAX));

    /* ----- Get Value of Inputed Data ----- */

    unValue = GetDlgItemInt(hDlg, wEditId, NULL, FALSE);

    /* ----- Check Data Range of Inputed Data ----- */

    fCheck = (unMax < unValue) ? FALSE : TRUE;

    if (fCheck == FALSE) {      /* Out of Range Error Occured */

        /* ----- Load Error Message & Caption from Resource ----- */

        LoadString(hResourceDll, IDS_PEP_CAPTION_P17, szCaption, PEP_STRING_LEN_MAC(szCaption));
        LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_STRING_LEN_MAC(szWork));

        /* ----- Create Error Message for Prog # 17 ----- */

        wsPepf(szMessage, szWork, unMin, unMax);

        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg,
                   szMessage,
                   szCaption,
                   MB_OK | MB_ICONEXCLAMATION);

        /* ----- Set Previous Data ----- */

        SendDlgItemMessage(hDlg, wEditId, EM_UNDO, 0, 0L);

    } else  if ((fCheck == TRUE) && (wEditId == IDD_P17_BLK)) {

        /* ----- Enable Enter Button ----- */

        EnableWindow(GetDlgItem(hDlg, IDD_P17_ENTER), TRUE);
    }           
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P17ResetBlk()
*
**  Input       :   HWND    hDlg    -   Window Handle of a Dialogbox
*
**  Return      :   No return values
*
**  Description :
*       This function resets No. of Time Blocks. And it clears old data in
*   ComboBox and Time Table. And then it reset new data in Buffer, redraws
*   time table data.
* ===========================================================================
*/
VOID    P17ResetBlk(HWND hDlg)
{
    BOOL    fRet = IDOK,                /* MessageBox Return Value  */
            fSet,                       /* Set Button Enable Flag   */
            fEnter;                     /* Enter Button Enable Flag */
    WCHAR    szCaution[P17_MESS_LEN],    /* Delete Caution Message   */
            szCaption[P17_CAP_LEN],     /* Caption for MessageBox   */
            szMin[P17_MINLEN];          /* String for Minute Lata   */
    short   nCount;                     /* Counter                  */
    LPBYTE  lpbBlock;                   /* Points No. of Blocks     */
    DWORD   dwIndex;

    lpbBlock = (LPBYTE)&abData[P17_BLOCK_ADDR][P17_HOUR_ADDR];

    if (*lpbBlock) {

        /* ----- Load Caution Message, Caption ----- */

        LoadString(hResourceDll, IDS_P017_CAUTION, szCaution, PEP_STRING_LEN_MAC(szCaution));
        LoadString(hResourceDll, IDS_PEP_CAPTION_P17, szCaption, PEP_STRING_LEN_MAC(szCaption));

        /* ----- Popup MessageBox ----- */

        MessageBeep(MB_ICONEXCLAMATION);
        fRet = MessageBoxPopUp(hDlg,
                          szCaution,
                          szCaption,
                          MB_OKCANCEL | MB_ICONEXCLAMATION);
    }

    if (fRet == IDOK) {         /* User Selected OK */

        /* ----- Initialize Hourly Data ----- */

        memset((LPSTR)abData, 0, sizeof(abData));

        for (nCount = 0; nCount < P17_SEGMENT_MAX; nCount++) {

            /* ----- Reset Line & Cursor Position ----- */

            P17SetLinePos((WORD)nCount, 0);

            /* ----- Reset Line & Cursor Color ----- */

            aLineStatus[nCount].chColor   = P17_BLACK;
            aCursor[nCount].chColor       = P17_WHITE;
        }

        /* ----- Get Entried Data in EditText ----- */

        *lpbBlock = (BYTE)GetDlgItemInt(hDlg, IDD_P17_BLK, NULL, FALSE);

        /* ----- Reset Contents of ComboBox ----- */

        P17InitCombo(hDlg);

        /* ----- Draw New Time Table using 'WM_PAINT' Message ----- */

        InvalidateRect(hDlg, (const LPRECT)&rTable, TRUE);

    } else {                    /* User Selected Cancel */

        /* ----- Undo Data Input ----- */

        SetDlgItemInt(hDlg, IDD_P17_BLK, (UINT)*lpbBlock, FALSE);
    }

    if (*lpbBlock) {       /* No. of Blocks Data exists */

        dwIndex = SendDlgItemMessage(hDlg, IDD_P17_COMB, CB_GETCURSEL, 0, 0L);

        /* ----- Set Time Segment Data of Top Index ----- */

        SetDlgItemInt(hDlg,
                      IDD_P17_HH,
                      (UINT)abData[dwIndex + 1][P17_HOUR_ADDR],
                      FALSE);

        wsPepf(szMin,
                 WIDE("%02d"),
                 (short)abData[dwIndex + 1][P17_MIN_ADDR]);

        //SetDlgItemText(hDlg, IDD_P17_MM, szMin);

		//StaticBoxRedrawText (hDlg, IDD_P01_DESC_2, wStrID, sizeof(szWork));

        /* ----- Set Focus to ComboBox ----- */

        SetFocus(GetDlgItem(hDlg, IDD_P17_COMB));

        /* ----- Set Button Flag ----- */

        fEnter = FALSE;
        fSet   = TRUE;

    } else {                /* No. of Blocks Data Not Exists */

        /* ----- Erase Hour & Minute Data ----- */

        SendDlgItemMessage(hDlg, IDD_P17_HH, EM_SETSEL, 1, MAKELONG(0, -1));
        SendDlgItemMessage(hDlg, IDD_P17_HH, WM_CLEAR, 0, 0L);
        SendDlgItemMessage(hDlg, IDD_P17_MM, EM_SETSEL, 1, MAKELONG(0, -1));
        SendDlgItemMessage(hDlg, IDD_P17_MM, WM_CLEAR, 0, 0L);

        /* ----- Set Focus to EditText ----- */

        SetFocus(GetDlgItem(hDlg, IDD_P17_BLK));
        SendDlgItemMessage(hDlg, IDD_P17_BLK, EM_SETSEL, 1, MAKELONG(0, -1));

        /* ----- Set Button Flag ----- */

        fEnter = TRUE;
        fSet   = FALSE;
    }

    /* ----- Disable/Enable Enter & Set Button ----- */

    EnableWindow(GetDlgItem(hDlg, IDD_P17_ENTER),  fEnter);
    EnableWindow(GetDlgItem(hDlg, IDD_P17_SET),    fSet);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P17SetTime()
*
**  Input       :   HWND    hDlg    -   Window Handle of a Dialogbox
*
**  Return      :   No return values
*
**  Description :
*       This function displays time table & triangle cursor with new data.
* ===========================================================================
*/
VOID    P17SetTime(HWND hDlg)
{
    WORD    wIndex;                     /* Current Selected Index   */
    UINT    unHour,                     /* Hour Data                */
            unMinute;                   /* Minute Data              */
    WCHAR    szMin[P17_MINLEN];          /* Strings for Display      */

    /* ----- Get Value of Inputed Data ----- */

    unHour   = GetDlgItemInt(hDlg, IDD_P17_HH, NULL, FALSE);
    unMinute = GetDlgItemInt(hDlg, IDD_P17_MM, NULL, FALSE);

    /* ----- Convert Integer to Strings and Set Minute to EditText ----- */

    wsPepf(szMin, WIDE("%02u"), unMinute);
    DlgItemRedrawText(hDlg, IDD_P17_MM,szMin);

    /* ----- Get Current Selected Index of ComboBox ----- */

    wIndex = (WORD)SendDlgItemMessage(hDlg, IDD_P17_COMB, CB_GETCURSEL, 0, 0L);

    /* ----- Set Inputed Data to Buffer ----- */

    abData[wIndex + 1][P17_HOUR_ADDR]   = (BYTE)unHour;
    abData[wIndex + 1][P17_MIN_ADDR]    = (BYTE)unMinute;

    /* ----- Translate Hourly Data to Minute ( 1:30 -> 90 min) ----- */

    unMinute = P17_CALCMINUTE(abData[wIndex + 1][P17_HOUR_ADDR],
                              abData[wIndex + 1][P17_MIN_ADDR]);

    /* ----- Calculate Line & Cursor Position ----- */

    P17SetLinePos(wIndex, (short)unMinute);

    /* ----- Set Line & Cursor Color to Yellow ----- */

    aLineStatus[wIndex].chColor = P17_YELLOW;
    aCursor[wIndex].chColor     = P17_YELLOW;

    /* ----- Draw Time Table Data by using 'WM_PAINT' Message ----- */

    InvalidateRect(hDlg, (const LPRECT)&rTable, TRUE);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P17DrawTime()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   BYTE    bBlock  -   No. of Activity Blocks
*                   WORD    wIndex  -   Current Selected Index
*
**  Return      :   No return values
*
**  Description :
*       This function draws the time table with current time segmant data, if
*   the Dialog Procedure received WM_PAINT message.
* ===========================================================================
*/
VOID    P17DrawTime(HWND hDlg, BYTE bBlock, WORD wIndex)
{
    char        chCount;        /* Counter                          */
    PAINTSTRUCT psPaint;        /* Client Area Information          */
    RECT        rClient;        /* Windows Client Area Position     */
    HDC         hDC;            /* Handle of Device Context         */
    HPEN        hPenWork = 0,   /* Handle of Color Pen              */
                hPenWhite,      /* Handle of White Pen              */
                hPenBlack,      /* Handle of Black Pen              */
                hPenYellow;     /* Handle of Yellow Pen             */
    HBRUSH      hBrushWork = 0, /* Handle of Work Brush             */
                hBrushBlack,    /* Handle of Black Brush            */
                hBrushYellow;   /* Handle of Yellow Brush           */
    LOGPEN      lgpnYellow,     /* Yellow Pen Style Structure       */
                lgpnWhite,      /* White Pen Style Structure        */
                lgpnBlack;      /* Black Pen Style Structure        */
    LOGBRUSH    lbYellow,       /* Yellow Brush Style Structure     */
                lbBlack;        /* Black Brush Style Structure      */
    DWORD       dwBkColor,      /* Current Back Ground Color        */
                dwTextColor;    /* Current Window Text Color        */
    LPPOINT     lpptCur;

    /* ----- Get Handle of Device Context ----- */

    hDC = BeginPaint(hDlg, (LPPAINTSTRUCT)&psPaint);

    /* ----- Set Device Context Attributes ----- */

    SetMapMode(hDC, MM_ANISOTROPIC);
    GetClientRect(hDlg, (LPRECT)&rClient);
    SetViewportExtEx(hDC, rClient.right, rClient.bottom, NULL);
    SetViewportOrgEx(hDC, P17_ORIGIN, P17_ORIGIN, NULL);
    SetWindowExtEx(hDC, P17_WINDOW_RIGHT, P17_WINDOW_BOTTOM, NULL);

    /* ----- Set Drawing Mode ----- */

    SetROP2(hDC, R2_COPYPEN);

    /* ----- Get Current System Color ----- */

    dwBkColor = GetSysColor(COLOR_WINDOW);
    dwTextColor = GetSysColor(COLOR_WINDOWTEXT);

    /* ----- Set Color Pen Style ----- */

    lgpnBlack.lopnStyle   = PS_SOLID;
    lgpnBlack.lopnWidth.x = P17_PEN_WIDTH;
    lgpnBlack.lopnWidth.y = P17_PEN_WIDTH;
    lgpnBlack.lopnColor   = dwTextColor;

    lgpnWhite.lopnStyle   = PS_SOLID;
    lgpnWhite.lopnWidth.x = P17_PEN_WIDTH;
    lgpnWhite.lopnWidth.y = P17_PEN_WIDTH;
    lgpnWhite.lopnColor   = dwBkColor;

    lgpnYellow.lopnStyle   = PS_SOLID;
    lgpnYellow.lopnWidth.x = P17_PEN_WIDTH;
    lgpnYellow.lopnWidth.y = P17_PEN_WIDTH;
    lgpnYellow.lopnColor   = (dwTextColor + dwBkColor) / 2;

    /* ----- Get Color Pen Handle ----- */

    hPenBlack   = CreatePenIndirect((LPLOGPEN)&lgpnBlack);
    hPenWhite   = CreatePenIndirect((LPLOGPEN)&lgpnWhite);
    hPenYellow  = CreatePenIndirect((LPLOGPEN)&lgpnYellow);

    /* ----- Set Color Brush Style ----- */

    lbBlack.lbStyle = BS_SOLID;
    lbBlack.lbColor = dwTextColor;
    lbBlack.lbHatch = 0;

    lbYellow.lbStyle = BS_SOLID;
    lbYellow.lbColor = (dwTextColor + dwBkColor) / 2;
    lbYellow.lbHatch = 0;

    /* ----- Get Color Brush Handle ----- */

    hBrushBlack  = CreateBrushIndirect((LPLOGBRUSH)&lbBlack);
    hBrushYellow = CreateBrushIndirect((LPLOGBRUSH)&lbYellow);

    /* ----- Set Invalidate Rectangle to Table (Redraw Area) ----- */

    rTable.left   = P17_CLIENT_LEFT(rClient.right);
    rTable.right  = P17_CLIENT_RIGHT(rClient.right);
    rTable.top    = P17_CLIENT_TOP(rClient.bottom);
    rTable.bottom = P17_CLIENT_BOTTOM(rClient.bottom);

    rTri.left   = rTable.left;
    rTri.right  = rTable.right;
    rTri.top    = P17_TRI_TOP(rClient.bottom);
    rTri.bottom = rTable.bottom;

    /* ----- Draw Black Box ----- */

    SelectObject(hDC, hBrushBlack);
    Rectangle(hDC,  P17_BLACK_LEFT,     P17_BLACK_TOP,
                    P17_BLACK_RIGHT,    P17_BLACK_BOTTOM);

    if (bBlock) {

        for (chCount = (char)(bBlock - 1); chCount >= 0 ; chCount--) {

            /* ----- Select Color Pen ----- */

            hPenWork = (aLineStatus[chCount].chColor == P17_WHITE) ?
                        hPenWhite : hPenYellow;

            SelectObject(hDC, hPenWork);

            /* ----- Draw Line ----- */

            MoveToEx(hDC, aLineStatus[chCount].xPosition, P17_BLACK_TOP, NULL);
            LineTo(hDC, aLineStatus[chCount].xPosition, P17_BLACK_BOTTOM);

            /* ----- Draw Triangle Cursor ----- */

            if (fCapture != TRUE) {

                lpptCur = aCursor[chCount].ptTri;

                hBrushWork = (aCursor[chCount].chColor == P17_BLACK) ?
                            hBrushBlack : hBrushYellow;

                SelectObject(hDC, hPenBlack);
                SelectObject(hDC, hBrushWork);

                SetPolyFillMode(hDC, ALTERNATE);

                Polygon(hDC, lpptCur, P17_TRI_POINT);
            }
        }

        /* ----- Draw Current Selected Index to Top ----- */

        /* ----- Draw Line  & Cursor ----- */

        SelectObject(hDC, hPenYellow);
        MoveToEx(hDC, aLineStatus[wIndex].xPosition, P17_BLACK_TOP, NULL);
        LineTo(hDC, aLineStatus[wIndex].xPosition, P17_BLACK_BOTTOM);

        SelectObject(hDC, hPenBlack);
        SelectObject(hDC, hBrushYellow);
        SetPolyFillMode(hDC, ALTERNATE);
        Polygon(hDC, aCursor[wIndex].ptTri, P17_TRI_POINT);
    }
    EndPaint(hDlg, (LPPAINTSTRUCT)&psPaint);

    DeleteObject(hBrushYellow);
    DeleteObject(hBrushBlack);
    DeleteObject(hBrushWork);

    DeleteObject(hPenYellow);
    DeleteObject(hPenBlack);
    DeleteObject(hPenWhite);
    DeleteObject(hPenWork);
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    P17ChkPos()
*
**  Input       :   LPPOINT lpPt    -   Address of Mouse Down Position 
*
**  Return      :   BOOL    TRUE    -   effective points
*                           FALSE   -   not effective points
*
**  Description:
*       This procedure checkes the clicked position whether effective or not.
* ===========================================================================
*/
BOOL    P17ChkPos(PPOINTS lpPt)
{
    BOOL    fRet = TRUE;    /* Return Value This Function */

    if (lpPt->x < P17_WINDOW_MIN) {     /* Check Mouse Position (x) */

        fRet = FALSE;

    } else if (lpPt->x > P17_WINDOWX_MAX) {

        fRet = FALSE;
    }

    if (lpPt->y < P17_WINDOW_MIN) {     /* Check Mouse Position (y) */
    
        fRet = FALSE;

    } else if (lpPt->y > P17_WINDOWY_MAX) {

        fRet = FALSE;
    }
    return fRet;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P17DispMove()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   LPPOINT lpPt    -   Address of Mouse Down Position
*                   WORD    wIndex  -   Current Selected Index
*
**  Return      :   No Return Value
*
**  Description :
*       This procedure changes string ID of the specified position.
*           (This position is the point of mouse down.)
* ===========================================================================
*/
VOID    P17DispMove(HWND hDlg, PPOINTS lpPt, WORD  wIndex)
{
    short   nMinute;                /* Time Data by Mouse Position  */
    WCHAR    chHour,                 /* Hour Data for Text Out       */
            chMin,                  /* Minute Data for Text Out     */
            szMin[P17_MINLEN];      /* Strings for Minute Data      */

    if (lpPt->x < P17_MOUSEX_MIN) {     /* Adjust Mouse Position */

        lpPt->x = P17_MOUSEX_MIN;

    } else if (lpPt->x > P17_MOUSEX_MAX) {

        lpPt->x = P17_MOUSEX_MAX;
    }

    /* ----- Set Triangle Position ----- */

    aCursor[wIndex].ptTri[P17_TRIPOS1].x = P17_GETPOS(lpPt->x);
    aCursor[wIndex].ptTri[P17_TRIPOS2].x = P17_GETPOS(lpPt->x) - P17_TRIX_LEN;
    aCursor[wIndex].ptTri[P17_TRIPOS3].x = P17_GETPOS(lpPt->x) + P17_TRIX_LEN;

    /* ----- Calculate Minute Data by Current Mouse Position ----- */

    nMinute = P17CalcMinute(lpPt->x);

    /* ----- Text out Hour / Minute Data ----- */

    chHour = (char)(nMinute / P17_MINDATA);
    chMin  = (char)(nMinute % P17_MINDATA);

    wsPepf(szMin, WIDE("%02d"), (short)chMin);

    SetDlgItemInt(hDlg, IDD_P17_HH, (UINT)chHour, FALSE);
    DlgItemRedrawText(hDlg, IDD_P17_MM, szMin);

    /* ----- Draw Time Table Data by using 'WM_PAINT' Message ----- */

    InvalidateRect(hDlg, (const LPRECT)&rTri, TRUE);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P17DispPre()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This procedure changes string ID of the specified position.
*           (This position is the point of mouse down.)
* ===========================================================================
*/
VOID    P17DispPre(HWND hDlg)
{
    WORD    wIndex;                 /* Current Selected Index       */
    short   nMinute;                /* Minute Data for Position     */
    WCHAR    chHour,                 /* Hour Data for Text Out       */
            chMin,                  /* Minute Data for Text Out     */
            szMin[P17_MINLEN];      /* Strings for Minute Data      */

    wIndex = (WORD)SendDlgItemMessage(hDlg, IDD_P17_COMB, CB_GETCURSEL, 0, 0L);

    /* ----- Get Data from abData ----- */

    chHour = abData[wIndex + 1][P17_HOUR_ADDR];
    chMin  = abData[wIndex + 1][P17_MIN_ADDR];

    nMinute = P17_CALCMINUTE(chHour, chMin);
    
    /* ----- Text out Previous Hour / Minute Data to EditText ----- */

    SetDlgItemInt(hDlg, IDD_P17_HH, (UINT)chHour, FALSE);

    wsPepf(szMin, WIDE("%02d"), chMin);
    DlgItemRedrawText(hDlg, IDD_P17_MM, szMin);

    /* ----- Reset Line & Cursor Position ----- */

    P17SetLinePos(wIndex, nMinute);

    /* ----- Redraw Time Table Data using 'WM_PAINT' Message ----- */

    InvalidateRect(hDlg, (const LPRECT)&rTri, TRUE);
}

/* ===========================================================================
**  Synopsis    :   BOOL    P17ChkDownPos()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*                   LPPOINT lpPt        -   Address of Mouse Down Position
*                   LPWORD  lpwIndex    -   Address of Previouse Selected Index
*
**  Return      :   BOOL    TRUE    -   effective points
*                           FALSE   -   not effective points
*
**  Description:
*       This procedure checkes the clicked position whether effective or not.
* ===========================================================================
*/
BOOL    P17ChkDownPos(HWND hDlg, PPOINTS lpPt, LPWORD lpwIndex)
{
    BOOL    fRet = FALSE;           /* Return Value of This Function    */
    WORD    wIndex;                 /* Current Selected Index           */
    short   nX,                     /* Current Mouse Position (x)       */
            nMinute;                /* Time Data by Mouse Position      */
    WCHAR    chHour,                 /* Hour Data for Text out           */
            chMin,                  /* Minute Data for Text out         */
            szMin[P17_MINLEN];      /* Minute String for Text out       */

    /* ----- Get Current Mouse Position (x) ----- */

    nX = P17_GETPOS(lpPt->x);

    if ((aCursor[*lpwIndex].ptTri[P17_TRIPOS2].x <= nX) &&
        (aCursor[*lpwIndex].ptTri[P17_TRIPOS3].x >= nX)) {

        wIndex = *lpwIndex;

        fRet = TRUE;

    } else {

        for (wIndex = 0; wIndex < P17_SEGMENT_MAX; wIndex++) {

            /* ----- Check Mouse Position in Triangle Cursor ----- */

            if ((aCursor[wIndex].ptTri[P17_TRIPOS2].x <= nX) &&
                (aCursor[wIndex].ptTri[P17_TRIPOS3].x >= nX)) {

                fRet = TRUE;
                break;
            }
        }
    }

    if ((lpPt->y < P17_MOUSEDOWN_Y) || (lpPt->y > P17_MOUSEY_MAX)) {

        fRet = FALSE;
    }

    if (fRet == TRUE) {     /* Mouse Clicked Cursor Position */

        SendDlgItemMessage(hDlg, IDD_P17_COMB, CB_SETCURSEL, wIndex, 0L);

        /* ----- Reset Line & Cursor Color ----- */

        aLineStatus[*lpwIndex].chColor = P17_WHITE;
        aCursor[*lpwIndex].chColor     = P17_BLACK;

        *lpwIndex = wIndex;

        /* ----- Set Current Selected Index Cursor Color to Yellow ----- */

        aLineStatus[wIndex].chColor = P17_YELLOW;
        aCursor[wIndex].chColor     = P17_YELLOW;

        /* ----- Reset Cursor Position to Clicked Position ----- */

        aCursor[wIndex].ptTri[P17_TRIPOS1].x = P17_GETPOS(lpPt->x);
        aCursor[wIndex].ptTri[P17_TRIPOS2].x = P17_GETPOS(lpPt->x) - P17_TRIX_LEN;
        aCursor[wIndex].ptTri[P17_TRIPOS3].x = P17_GETPOS(lpPt->x) + P17_TRIX_LEN;

        /* ----- Get Minute Data by Current Mouse Position ----- */

        nMinute = P17CalcMinute(lpPt->x);

        /* ----- Calculate Hour / Minute Data ----- */

        chHour = (char)(nMinute / P17_MINDATA);
        chMin  = (char)(nMinute % P17_MINDATA);

        /* ----- Text out Hour / Minute Data to EditText ----- */

        wsPepf(szMin, WIDE("%02d"), (short)chMin);

        SetDlgItemInt(hDlg, IDD_P17_HH, (UINT)chHour, FALSE);
        DlgItemRedrawText(hDlg, IDD_P17_MM, szMin);

        /* ----- Draw Time Table Data by using 'WM_PAINT' Message ----- */

        InvalidateRect(hDlg, (const LPRECT)&rTable, TRUE);
    }
    return fRet;
}

/* ===========================================================================
**  Synopsis    :   VOID    P17ChkCtrlID()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WPARAM  wEditId -   Current Control ID
*                   BYTE    bBlock  -   No. of Time Block
*
**  Return      :   No Return Value
*
**  Description :
*       Previous Input Focus is IDD_P17_BLK, Current is not IDD_P17_BLK / 
*   IDD_P17_ENTER. Reset Old Data to IDD_P17_BLK. (Undo Data input)
* ===========================================================================
*/
VOID    P17ChkCtrlID(HWND hDlg, WORD wEditId, BYTE bBlock)
{
    if ((wEditId != IDD_P17_BLK) && (wEditId != IDD_P17_ENTER) && (wEditId != IDD_P17_BLKSCR)) {

        if (bBlock) {

            /* ----- Disable Button ----- */

            EnableWindow(GetDlgItem(hDlg, IDD_P17_ENTER), FALSE);
        }

        /* ----- Reset Old Data to EditText ----- */

        SetDlgItemInt(hDlg, IDD_P17_BLK, (UINT)bBlock, FALSE);
    }
}

/* ===========================================================================
**  Synopsis    :   short   P17CalcMinute()
*
**  Input       :   int     xPos    -   Mouse Position (x)
*
**  Return      :   short   nMinute -   Minute Data
*
**  Description :    
*       This Function calculates minute data by mouse x-position.
* ===========================================================================
*/
short   P17CalcMinute(int  xPos)
{
    short   nMinute,
            nWork;

    /* ----- Calculate Minute ----- */

    nWork = P17_ADJUSTTIME(xPos);

    nMinute = P17_GETTIME(xPos);

    /* ----- Adjust Minute ----- */

    switch (nWork) {

    case P17_REMAIN_3:

        nMinute += P17_MOUSE_ADJ_5;
        break;

    case P17_REMAIN_2:

        nMinute += P17_MOUSE_ADJ_10;
        break;

    case P17_REMAIN_1:

        nMinute += P17_MOUSE_ADJ_15;
        break;

    default:

        break;
    }

    nMinute = (short)((nMinute >= (P17_DATA_MAX * P17_MINDATA)) ?       /* V3.3 */
                        P17_MIN_MAX  : nMinute);

    nMinute = (short)((nMinute <= P17_DATA_MIN) ?
                        P17_DATA_MIN : nMinute);

    return nMinute;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P17ScrlProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WPARAM  wParam  -   Specifies a Scroll Bar Code
*                   LPARAM  lParam  -   Current Position/ Handle of ScrollBar
*
**  Return      :   No return value.
*
**  Description:
*       This function is Vertical Scroll Procedure.
* ===========================================================================
*/
VOID    P17ScrlProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    PEPSPIN     PepSpin;
    HWND        hCtrl;
    int         idEdit = 0;

    /* ----- Initialize spin mode flag ----- */

    PepSpin.nStep  = P17_SPIN_STEP;
    PepSpin.nTurbo = P17_SPIN_TURBO;

    /* ----- Get Handle of Current Selected Vertical Scroll Bar ----- */

    hCtrl = (HWND)lParam;

    /* ----- Set Configlation of EditText ----- */

    if (hCtrl == GetDlgItem(hDlg, IDD_P17_BLKSCR)) {

        wPrevID            = IDD_P17_BLKSCR;
        idEdit             = IDD_P17_BLK;
        PepSpin.lMax       = (long)P17_SEGMENT_MAX;
        PepSpin.lMin       = (long)P17_DATA_MIN;
        PepSpin.fSpinStyle = PEP_SPIN_NOROLL;

        if (! IsWindowEnabled(GetDlgItem(hDlg, IDD_P17_ENTER))) {

            EnableWindow(GetDlgItem(hDlg, IDD_P17_ENTER), TRUE);
        }

    } else  if (hCtrl == GetDlgItem(hDlg, IDD_P17_HHSCR)) {

        if (IsWindowEnabled(GetDlgItem(hDlg, IDD_P17_SET))) {

            idEdit             = IDD_P17_HH;
            PepSpin.lMax       = (long)P17_HH_MAX;
            PepSpin.lMin       = (long)P17_DATA_MIN;
            PepSpin.fSpinStyle = PEP_SPIN_ROLL;
        }

    } else  if (hCtrl == GetDlgItem(hDlg, IDD_P17_MMSCR)) {

        if (IsWindowEnabled(GetDlgItem(hDlg, IDD_P17_SET))) {

            idEdit             = IDD_P17_MM;
            PepSpin.lMax       = (long)P17_MM_MAX;
            PepSpin.lMin       = (long)P17_DATA_MIN;
            PepSpin.fSpinStyle = PEP_SPIN_ROLL;
        }
    }

    if (idEdit != 0) {

        if (PepSpinProc(hDlg, wParam, idEdit, (LPPEPSPIN)&PepSpin)) {

            if ((idEdit == IDD_P17_MM) || (idEdit == IDD_P17_HH)) {

                PostMessage(hDlg, WM_COMMAND, MAKELONG(IDD_P17_SET, BN_CLICKED), 0L);
            }
        }
    }
}

/* ===== End of P017.C ===== */
