/*              
* ---------------------------------------------------------------------------
* Title         :   Machine Definition Code (Prog. 1)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P001.C
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
* Dec-22-93 : 00.00.01 : M.Yamamoto : Initial (Migrate from HP US Model)
* Jan-09-93 : 00.00.02 : M.Yamamoto : Add New Requirement.
* Mar-13-95 : 03.00.00 : H.Ishida   : Add DestroyWindow(*lphProgDigChk1) by error
* Sep-16-98 : 03.03.00 : A.Mitsui   : V3.3
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

//#include	"stringResource.h" //4-21-03 RPH
#include    <ecr.h>
#include    <paraequ.h>
#include    <para.h>

#include    "prog.h"
#include    "pepcomm.h"
#include    "pepflag.h"
#include    "p001.h" 

static struct {
	ULONG   ulMnemoicId_0;
	ULONG   ulMnemoicId_1;
	USHORT  usAddr;
	UCHAR   uchMask;
} ListOfMdcMnemonics[] = {
	{29346, 29347, MDC_PLU5_ADR, EVEN_MDC_BIT1},        //  AC170 Sales Restriction MDC
	{26838, 26839, MDC_RPTCAS11_ADR, ODD_MDC_BIT3},     //  Feed One Line After Service Totals.
	{26866, 26867, MDC_RPTCAS13_ADR, ODD_MDC_BIT1},     //  Feed One Line After Add Check Totals.
	{27046, 27047, MDC_RPTCAS24_ADR, EVEN_MDC_BIT3},    //  Feed One Line After Tip Totals.
	{26886, 26887, MDC_RPTCAS14_ADR, EVEN_MDC_BIT3},    //  Feed One Line After Tax Totals.
	{26944, 26945, MDC_RPTCAS18_ADR, EVEN_MDC_BIT0},    //  Feed One Line After Surcharge List.
	{26928, 26929, MDC_RPTCAS17_ADR, ODD_MDC_BIT0},     //  Feed One Line After Bonus Totals List.
	{26932, 26933, MDC_RPTCAS17_ADR, ODD_MDC_BIT2},     //  Feed One Line After Total for Bonus Totals.
	{26950, 26951, MDC_RPTCAS18_ADR, EVEN_MDC_BIT3},    //  Feed One Line After Cancel/Void List.
	{26406, 26407, MDC_RPTFIN14_ADR, EVEN_MDC_BIT3},    //  Feed One Line After Tip Totals.
	{26482, 26483, MDC_RPTFIN19_ADR, ODD_MDC_BIT1},     //  Feed One Line After Tax Totals.
	{26528, 26529, MDC_RPTFIN22_ADR, EVEN_MDC_BIT0},    //  Feed One Line After Bonus Total List.
	{26532, 26533, MDC_RPTFIN22_ADR, EVEN_MDC_BIT2}     //  Feed One Line After Surcharge List.
};

/*
* ===========================================================================
**  Synopsis:   wchar_t *PepFetchMdcMnemonic (USHORT usAddr, UCHAR uchMask,
*                                             wchar_t *wcsBuff, USHORT usMaxLen)
*
**  Input   :   USHORT usAdd     - MDC address as used for ParaMDCCheck()
*               UCHAR uchMask    - MDC bit mask as used for ParaMDCCheck()
*               wchar_t *wcsBuff - address of buffer to receive the mnemonic
*               USHORT usMaxLen  - maximum number of characters, buffer size
*
**  Return  :   wchar_t *  wcsBuff address
*
**  Description: Look up the MDC text as used by the P01 MDC Settings dialog
*                for the designated MDC address and bit mask.
*
*                This function is used in various places where a dialog, usually
*                a Maintenance menu dialog such as AC170, displays the state of
*                an MDC setting which governs how the functionality works.
*
*                We provide a way for the user to identify any MDC settings that
*                modify the settings behavior and their current state. We also
*                will allow the user to change the MDC setting in the displayed
*                dialog rather than having to remember to go to the P01 MDC settings,
*                hunt through the list for the correct MDC, and change its setting.
* ===========================================================================
*/
wchar_t *PepFetchMdcMnemonic (USHORT usAddr, UCHAR uchMask, UCHAR uchVal, wchar_t *wcsBuff, USHORT usMaxLen)
{
	USHORT  i;

	wcsBuff[0] = 0;
	for (i = 0; i < sizeof(ListOfMdcMnemonics)/sizeof(ListOfMdcMnemonics[0]); i++) {
		if (ListOfMdcMnemonics[i].usAddr == usAddr && ListOfMdcMnemonics[i].uchMask == uchMask) {
			if (uchVal)
				LoadString(hResourceDll, ListOfMdcMnemonics[i].ulMnemoicId_1, wcsBuff, usMaxLen);
			else
				LoadString(hResourceDll, ListOfMdcMnemonics[i].ulMnemoicId_0, wcsBuff, usMaxLen);
			break;
		}
	}

	return wcsBuff;
}

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
static  BYTE    abMDCData[P01_ADDR_MAX];    /* Buffer of MDC Data       */
static	WCHAR   szDlgType[P01_TYPE_LEN];    /* Sub-DlgBox Style String  */
static  WORD    wSubDlgID = 0;              /* Dialog ID of Sub-DlgBox  */
static  P01CATESTAT aCategory[P01_CATE_MAX] = {     /* Category Status  */
                        0, P01_CATE_01_NO,
                        0, P01_CATE_02_NO,
                        0, P01_CATE_03_NO,
                        0, P01_CATE_04_NO,
                        0, P01_CATE_05_NO,
                        0, P01_CATE_06_NO,
                        0, P01_CATE_07_NO,
                        0, P01_CATE_08_NO,
                        0, P01_CATE_09_NO,
/*                        0, P01_CATE_10_NO, */
                        0, P01_CATE_11_NO,
                        0, P01_CATE_12_NO,
                        0, P01_CATE_13_NO
                    };

/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/

/*
* ===========================================================================
**  Synopsis:   BOOL    WINAPI  P001DlgProc()
*
**  Input   :   HWND    hDlg   - handle of a dialogbox procedure
*               WORD    wMsg   - dispacthed message
*               WORD    wParam - 16 bits message parameter
*               LONG    lParam - 32 bits message parameter
*
**  Return  :   TRUE           - user process is executed
*               FALSE          - default process is expected
*
**  Description:
*       This is a main dialgbox procedure for the MDC Maintenance.
* ===========================================================================
*/
BOOL    WINAPI  P001DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    UINT    unAddress, unCategory;

    switch (wMsg) {
    case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, 0);
		P01InitDlg(hDlg);
        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			TEXTMETRIC tm;
			HDC myHdc;

			SendDlgItemMessage(hDlg, IDD_P01_LIST, WM_SETFONT, (WPARAM)hResourceFont, 0);
			myHdc = GetDC (hDlg);
			GetTextMetrics (myHdc, &tm);
			ReleaseDC (hDlg, myHdc);
			SendDlgItemMessage(hDlg, IDD_P01_LIST, LB_SETITEMHEIGHT, 0, (tm.tmHeight + (tm.tmHeight >> 2)));
	
			SendDlgItemMessage(hDlg, IDD_P01_MDC, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_KILLFOCUS:
        if (LOWORD(wParam) == IDD_P01_LIST) {
            PostMessage(hDlg, WM_NEXTDLGCTL, (WPARAM)(GetDlgItem(hDlg, LOWORD(wParam))), MAKELONG(TRUE, 0));
        }
        return FALSE;

    case WM_DESTROY:
        SendMessage(hwndProgMain, PM_PEPMENU, (WPARAM)PEP_MENU_ALL_ON, 0L);
        return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDD_P01_LIST:
            if (HIWORD(wParam) == LBN_SELCHANGE) {
                /* ----- Check Selected Index is Address or Category ----- */
                unAddress = P01ChkIndex(hDlg, (LPUINT)&unCategory);
                if (unAddress) {    /* Selected Linked Address */
                    /* ----- Make Edit-DailogBox ----- */
                    P01MakeEditDlg(hDlg, unAddress);
                } else {            /* Selected Category */
                    if (*lphProgDlgChk2) {
                        DestroyWindow(*lphProgDlgChk2);
                        *lphProgDlgChk2 = 0;
                        wSubDlgID = 0;
                    }
                }

                SetActiveWindow(hDlg);
                return TRUE;
            } else if (HIWORD(wParam) == LBN_DBLCLK) {
                /* ----- Check Selected Index is Address or Category ----- */
                unAddress = P01ChkIndex(hDlg, (LPUINT)&unCategory);

                if (! unAddress) {
                    /* ----- Open / Close Category ----- */
                    P01OpenCtgry(hDlg, unCategory);
                }
                return TRUE;
            }
            return FALSE;

        case IDOK:
            if (HIWORD(wParam) == BN_CLICKED) { /* Button was Clicked */
                /* ----- Check Current Input Focus ----- */
                if (GetDlgItem(hDlg, IDD_P01_LIST) == GetFocus()) {
                    PostMessage(hDlg, WM_COMMAND, MAKELONG(IDD_P01_LIST, LBN_DBLCLK), 0L);
                } else {
                    P01FinDlg(hDlg, wParam);
                }
                return TRUE;
            }
            return FALSE;

        case IDCANCEL:
            if (HIWORD(wParam) == BN_CLICKED) { /* Button was Clicked */
                P01FinDlg(hDlg, wParam);
                return TRUE;
            }
            return FALSE;
        }
        return FALSE;

    default:
        return FALSE;
    }
}

/*
* ===========================================================================
**  Synopsis:   VOID    P01InitDlg();
*
**  Input   :   HWND    hDlg   - handle of a dialogbox procedure
*
**  Return  :   No return Value
*
**  Description:
*       This Function initializes configulation of DialogBox.
* ===========================================================================
*/
VOID    P01InitDlg(HWND hDlg)
{
    USHORT  usRet;
    HCURSOR hCursor;
    UINT    wI,
            wID;
    WCHAR   szWork[128],
            szDesc[128];

    /* ----- Change cursor (arrow -> hour-glass) ----- */
    hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
    ShowCursor(TRUE);

    /* ----- Control popup menu (off) ----- */
    SendMessage(hwndProgMain, PM_PEPMENU, (WPARAM)PEP_MENU_ALL_OFF, 0L);

	/* ----- Read Initial Data from Parameter File ----- */
    ParaAllRead(CLASS_PARAMDC, (UCHAR *)abMDCData, sizeof(abMDCData), 0, &usRet);

	/* ----- Set Category String to ListBox ----- */
    for (wI = 0; wI < P01_CATE_MAX; wI++) {
        aCategory[wI].nOpen = P01_CATE_CLOSED;
        wID = (IDS_P01 + (wI * P01_CATE_OFFSET));
        LoadString(hResourceDll, wID, szWork, PEP_STRING_LEN_MAC(szWork));
        wsPepf(szDesc, WIDE("%c %s"), aCategory[wI].nOpen, szWork);
        DlgItemSendTextMessage (hDlg, IDD_P01_LIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)szDesc);
    }

    SendDlgItemMessage(hDlg, IDD_P01_LIST, LB_SETCURSEL, 0, 0L);

    /* ----- Restore cursor (hour-glass -> arrow) ----- */
    ShowCursor(FALSE);

    SetCursor(hCursor);
}

/*
* ===========================================================================
**  Synopsis:   VOID    P01FinDlg();
*
**  Input   :   HWND    hDlg   - handle of a dialogbox procedure
*               WPARAM  wID    - Current Selected Control ID
*
**  Return  :   No return Value
*
**  Description:
*       This Function finishes DailogBox Procedure.
* ===========================================================================
*/
VOID    P01FinDlg(HWND hDlg, WPARAM wID)
{
    USHORT  usRet;

    if (LOWORD(wID) == IDOK) {
        /* ----- Write Current Data to Parameter File ----- */
        ParaAllWrite(CLASS_PARAMDC, (UCHAR *)abMDCData, sizeof(abMDCData), 0, &usRet);
        PepSetModFlag(hwndProgMain, PEP_MF_PROG, 0);
    }

    if (*lphProgDlgChk1) {
        DestroyWindow(*lphProgDlgChk1);
    }

    if (*lphProgDlgChk2) {
        DestroyWindow(*lphProgDlgChk2);
    }

    SendMessage(hwndProgMain, PM_PEPMENU, (WPARAM)PEP_MENU_ALL_ON, 0L);

    *lphProgDlgChk1 = NULL;
    *lphProgDlgChk2 = NULL;

    EndDialog(hDlg, FALSE);
}

/*
* ===========================================================================
**  Synopsis:   UINT    P01ChkIndex();
*
**  Input   :   HWND    hDlg        - handle of a dialogbox procedure
*               LPUINT  lpunCate    - Points to Current Selected Category 
*
**  Output  :   LPUINT  lpunCate    - Points to Current Selected Category
*                                       (Zero Based Category No.)
*
**  Return  :   Current Selected Address No.
*
**  Description:
*       This Function checks current selected item in listbox wheter address
*   or category. If category selected, it will return NULL, *lpunCate is 
*   current category No. Otherwise (Address selected), it will return addrees
*   No., and *lpunCate is current category.
* ===========================================================================
*/
UINT    P01ChkIndex(HWND hDlg, LPUINT lpunCate)
{
    UINT    unIndex, unOffset, unCateNo, unCount = 0;
    int     nAddr;
    WCHAR   szWork[128];

    /* ----- Get Current Selected Index of ListBox ----- */
    unIndex = (UINT)SendDlgItemMessage(hDlg, IDD_P01_LIST, LB_GETCURSEL, 0, 0L);

    /* ----- Get Current Selected String of ListBox ----- */
    DlgItemSendTextMessage(hDlg, IDD_P01_LIST, LB_GETTEXT, (WPARAM)unIndex, (LPARAM)szWork);

    /* ----- Get Address No. of Selected String ----- */
    nAddr = _wtoi(szWork);

    /* ----- Check Zero Based Category No. of Selected Address ------ */
    for (unCateNo = 0, unOffset = 0; unCateNo < P01_CATE_MAX; unCateNo++) {
        unCount = (UINT)((aCategory[unCateNo].nOpen == P01_CATE_CLOSED) ? 1 : aCategory[unCateNo].cAddr + 1);
        unOffset += unCount;
        if (unOffset > unIndex) {
            break;
        }
    }

    /* ----- Set Zero Based Category No. to Output Pointer ----- */
    *lpunCate = unCateNo;

    /* ----- Return Address No. of Selected String ----- */
    return (UINT)(nAddr);
}

/*
* ===========================================================================
**  Synopsis:   VOID    P01OpenCtgry();
*
**  Input   :   HWND    hDlg   - handle of a dialogbox procedure
*               UINT    unCate - Current selected Zero Based Category No.
*
**  Return  :   No return Value
*
**  Description:
*       This Function Openes/Closes a section in the list control of the P01 dialog box.
*       Basically when the user clicks on the section heading, if the section is not
*       displayed then this function will add the list of items in a particular
*       section or category inserting the items into the list box control after the
*       section heading.
*       if the section is currently being displayed then this code will select strings
*       from the list box that correspond to the items in that section and delete the
*       strings from the list box.
* ===========================================================================
*/
VOID    P01OpenCtgry(HWND hDlg, UINT unCate)
{
    WORD    wIndex;
    WCHAR   szWork[128], szDesc[128];
    UINT    nStartID;
    int     nNoOfAddr, nI;

    /* ----- Get Current Selected Index No. of ListBox ----- */
    wIndex = (WORD)SendDlgItemMessage(hDlg, IDD_P01_LIST, LB_GETCURSEL, 0, 0L);

    /* ----- Get No. of Linked Address of Selected Category ----- */
    nNoOfAddr = aCategory[unCate].cAddr;

    /* ----- Set String ID of Selected Category ----- */
    nStartID  = (IDS_P01_CATEGO_01 + (P01_CATE_OFFSET * unCate));

    if (aCategory[unCate].nOpen == P01_CATE_CLOSED) {
        /* ----- Add Description of Linked Address ----- */
        /* ----- Set Open/Close Flag to 'Opened' ----- */
        aCategory[unCate].nOpen = P01_CATE_OPENED;

        /* ----- Insert Description of Linked Address ----- */
        for (nI = 1; nI <= nNoOfAddr; nI++) {
            /* ----- Load Description of Linked Address from Resource ----- */
            if (LoadString(hResourceDll, nStartID + nI, szWork, PEP_STRING_LEN_MAC(szWork)) == 0) {
				wsPepf (szWork, WIDE("String %d not found."), nStartID + nI);
			}

            /* ----- Insert Loaded String to ListBox ----- */
			DlgItemSendTextMessage (hDlg, IDD_P01_LIST, LB_INSERTSTRING, wIndex + nI, (LPARAM)szWork);
        }
    } else {
        /* ----- Delete Description of Linked Address ----- */
        /* ----- Set Open/Close Flag to 'Closed' ----- */
        aCategory[unCate].nOpen = P01_CATE_CLOSED;

        for (nI = 1; nI <= nNoOfAddr; nI++) {
            /* ----- Delete Description of Linked Address ----- */
            SendDlgItemMessage(hDlg, IDD_P01_LIST, LB_DELETESTRING, (WPARAM)(wIndex + 1),0L);
        }
    }

    /* ----- Insert New Description of Seleted Category ----- */
    LoadString(hResourceDll, nStartID, szWork, PEP_STRING_LEN_MAC(szWork));

    wsPepf(szDesc, WIDE("%c %s"), aCategory[unCate].nOpen, szWork);

    SendDlgItemMessage(hDlg, IDD_P01_LIST, LB_DELETESTRING, wIndex, 0L);

	DlgItemSendTextMessage (hDlg, IDD_P01_LIST, LB_INSERTSTRING,wIndex, (LPARAM)szDesc);
    
	SendDlgItemMessage(hDlg, IDD_P01_LIST, LB_SETCURSEL,    wIndex, 0L);
    SendDlgItemMessage(hDlg, IDD_P01_LIST, LB_SETTOPINDEX,  wIndex, 0L);
}

/*
* ===========================================================================
**  Synopsis:   HWND    P01MakeEditDlg();
*                       
**  Input   :   HWND    hDlg   - handle of a dialogbox procedure
*               UINT    unAddr - Address No. of Creation DialogBox
*
**  Return  :   *lphProgDlgChk2 - handle of edit-dialogbox
*               0               - fail to create dialogbox
*
**  Description:
*       This is a create edit-dialgbox(modeless) procedure
* ===========================================================================
*/
HWND    P01MakeEditDlg(HWND hDlg, UINT unAddr)
{
    WORD    wID;
    DLGPROC lpfnProcName;

    /* ----- Load Sub-DilaogBox Style String ----- */
    LoadString(hResourceDll, IDS_P01_STYLE + unAddr, szDlgType, PEP_STRING_LEN_MAC(szDlgType));

    /* ----- Set Sub-DialogBox ID ----- */
    wID = P01_GET_DLG_STYLE(szDlgType[P01_CHK_STYLE]);

    /* ----- Set Sub-DialogBox Procedure-Instance ----- */
    switch (wID) {
    case IDD_P01_DLG_B:
        lpfnProcName = P001EditBDlgProc;
        break;
        
    case IDD_P01_DLG_F:
    case IDD_P01_DLG_G:
        lpfnProcName = P001EditFGDlgProc;
        break;
        
    case IDD_P01_DLG_C:
    case IDD_P01_DLG_D:
    case IDD_P01_DLG_E:
        lpfnProcName = P001EditCDEDlgProc;
        break;

    case IDD_P01_DLG_H:
        lpfnProcName = P001EditHDlgProc;
        break;

    /* add V3.3 */
    case IDD_P01_DLG_259:
        lpfnProcName = P001Addr259DlgProc;
        break;

    case IDD_P01_DLG_367:                                 /*** Saratoga ***/
        lpfnProcName = P001Addr367DlgProc;       /*** Saratoga ***/
        break;                                            /*** Saratoga ***/

    default:
        lpfnProcName = P001EditADlgProc;
    }

    if (unAddr == P01_CAN_TAX_ADDR) {
        lpfnProcName = P001Addr16DlgProc;
    } else if (unAddr == P01_CHAR_TIPS_ADDR) {
        lpfnProcName = P001Addr25DlgProc;
    } else if ((unAddr == P01_DISP_FLY_ADDR1) ||
               (unAddr == P01_DISP_FLY_ADDR2) ||
               (unAddr == P01_DISP_PRT_ADDR)  ||
               (unAddr == MDC_CPM_INTVL_TIME)   ||
               (unAddr == MDC_CPM_WAIT_TIME)  ||
               (unAddr == MDC_EPT_SND_TIME)  ||
               (unAddr == MDC_EPT_RCV_TIME) ||
               (unAddr == MDC_DRAWEROPEN_TIME) ||
               (unAddr == MDC_SF_ALERT_TIME) ||
               (unAddr == MDC_SF_AUTOOFF_TIME) ||
               (unAddr == MDC_SF_TRANRULE_TIME) ||
               (unAddr == MDC_SF_CNTRL_ALERT) ||
               (unAddr == MDC_SF_CNTRL_AUTOOFF) ||
               (unAddr == MDC_RECEIPT_POST_LMT) ||
               (unAddr == MDC_RECEIPT_GIFT_LMT) ||
               (unAddr == MDC_RECEIPT_POD_LMT) ||
			   (unAddr == MDC_AUTO_SIGNOUT_ADR))  {

        lpfnProcName = P001Addr175DlgProc;
    }


    if (*lphProgDlgChk2 != 0) { /* case when already exist edit-dialogbox */
        if (wSubDlgID == wID) { /* case when selected same edit-dialogbox as before */
            /* ----- Post message to inactive edit-dialogbox ----- */
            SendMessage(*lphProgDlgChk2, PM_SELECTCHANGE, 0, 0L);
            return (*lphProgDlgChk2);
        } else {                /* case when selected another edit-dialogbox */
            /* ----- Destroy edit-dialogbox as previous selected */
            DestroyWindow(*lphProgDlgChk2);
        }
    }

    /* ----- Store which edit-dialogbox selected ----- */
    wSubDlgID = wID;

/*** NCR2172 ***/

    /* ----- Create selected edit-dialogbox ----- */
    *lphProgDlgChk2 = DialogCreation(hResourceDll, MAKEINTRESOURCEW(wID), hwndProgMain, lpfnProcName);

    return (*lphProgDlgChk2);

    hDlg = hDlg;
}

/*
* ===========================================================================
**  Synopsis:   BOOL    WINAPI  P001EditADlgProc()
*
**  Input   :   HWND    hDlg   - handle of a dialogbox procedure
*               WORD    wMsg   - dispacthed message
*               WORD    wParam - 16 bits message parameter
*               LONG    lParam - 32 bits message parameter
*
**  Return  :   TRUE           - user process is executed
*               FALSE          - default process is expected
*
**  Description:
*       This is a sub-dialgbox procedure (Type A) for the MDC Maintenance.
* ===========================================================================
*/
BOOL WINAPI P001EditADlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  int     nAddress;
    static  WPARAM  wCheckID;

    switch (wMsg) {

    case PM_SELECTCHANGE:
        SetActiveWindow(hDlg);
        SendDlgItemMessage(hDlg, wCheckID, WM_KILLFOCUS, 0L, 0L);

    case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE,0));

        /* ----- Initialize Configulation of Sub-DialogBox ----- */
        nAddress = P01InitSubDlg(hDlg);

        /* ----- Check RadioButton with Current MDC Data ----- */
        wCheckID = P01SetData(hDlg, nAddress);

        /* ----- Set Focus to the Top Index of RadioButton ----- */
        SetFocus(GetDlgItem(hDlg, wCheckID));

        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j=IDD_P01_DESC_1; j<=IDD_P01_BUTTON_08; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
		}
		return FALSE;

    case WM_COMMAND:
        wCheckID = wParam;

        switch (LOWORD(wParam)) {
        case    IDD_P01_BUTTON_01:
        case    IDD_P01_BUTTON_02:
            if (HIWORD(wParam) == BN_CLICKED) {

                /* ----- Check Current Selected RadioButton ----- */
                P01SetBuff(hDlg, IDD_P01_BUTTON_01, IDD_P01_BUTTON_02, wParam, nAddress, P01_CHK_BIT, 0);

                if ((nAddress == P01_TARE_ADDR) && P01ChkTare(hDlg)) {
                    wCheckID = (wParam == IDD_P01_BUTTON_01) ? IDD_P01_BUTTON_02 : IDD_P01_BUTTON_01;
                    /* ----- Reset Current Selected RadioButton ----- */
                    P01SetBuff(hDlg, IDD_P01_BUTTON_01, IDD_P01_BUTTON_02, wCheckID, nAddress, P01_CHK_BIT, 0);
                    SetFocus(GetDlgItem(hDlg, wCheckID));
                }
                return TRUE;
            }
            return FALSE;

        case    IDD_P01_BUTTON_03:
        case    IDD_P01_BUTTON_04:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Check Current Selected RadioButton ----- */
                P01SetBuff(hDlg, IDD_P01_BUTTON_03, IDD_P01_BUTTON_04, wParam, nAddress, P01_CHK_BIT, P01_1BITSHIFT);
                return TRUE;
            }
            return FALSE;

        case    IDD_P01_BUTTON_05:
        case    IDD_P01_BUTTON_06:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Check Current Selected RadioButton ----- */
                P01SetBuff(hDlg, IDD_P01_BUTTON_05, IDD_P01_BUTTON_06, wParam, nAddress, P01_CHK_BIT, P01_2BITSHIFT);

                if ((nAddress == P01_TARE_ADDR) && P01ChkTare(hDlg)) {
                    wCheckID = (wParam == IDD_P01_BUTTON_05) ? IDD_P01_BUTTON_06 : IDD_P01_BUTTON_05;
                    /* ----- Reset Current Selected RadioButton ----- */
                    P01SetBuff(hDlg, IDD_P01_BUTTON_05, IDD_P01_BUTTON_06, wCheckID, nAddress, P01_CHK_BIT, P01_2BITSHIFT);
                    SetFocus(GetDlgItem(hDlg, wCheckID));
                }
                return TRUE;
            }
            return FALSE;

        case    IDD_P01_BUTTON_07:
        case    IDD_P01_BUTTON_08:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Check Current Selected RadioButton ----- */
                P01SetBuff(hDlg, IDD_P01_BUTTON_07, IDD_P01_BUTTON_08, wParam, nAddress, P01_CHK_BIT, P01_3BITSHIFT);
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
**  Synopsis:   BOOL    WINAPI  P001EditBDlgProc()
*
**  Input   :   HWND    hDlg   - handle of a dialogbox procedure
*               WORD    wMsg   - dispacthed message
*               WORD    wParam - 16 bits message parameter
*               LONG    lParam - 32 bits message parameter
*
**  Return  :   TRUE           - user process is executed
*               FALSE          - default process is expected
*
**  Description:
*       This is a sub-dialgbox procedure (Type B) for the MDC Maintenance.
* ===========================================================================
*/
BOOL WINAPI P001EditBDlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  int     nAddress;
    static  WPARAM  wCheckID;

    switch (wMsg) {

    case PM_SELECTCHANGE:
        SetActiveWindow(hDlg);
        SendDlgItemMessage(hDlg, wCheckID, WM_KILLFOCUS, 0L, 0L);

    case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE,0));

        /* ----- Initialize Configulation of Sub-DialogBox ----- */
        nAddress = P01InitSubDlg(hDlg);

        /* ----- Check RadioButton with Current MDC Data ----- */
        wCheckID = P01SetData(hDlg, nAddress);
        SetFocus(GetDlgItem(hDlg, wCheckID));
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, 1);
        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j=IDD_P01_DESC_1; j<=IDD_P01_BUTTON_08; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
		}
		return FALSE;

    case WM_COMMAND:

        wCheckID = wParam;

        switch (LOWORD(wParam)) {
        case    IDD_P01_BUTTON_01:
        case    IDD_P01_BUTTON_02:
        case    IDD_P01_BUTTON_03:
        case    IDD_P01_BUTTON_04:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Check Current Selected RadioButton ----- */
                P01SetBuff(hDlg, IDD_P01_BUTTON_01, IDD_P01_BUTTON_04, wParam, nAddress, P01_CHK_2BITS, 0);
                return TRUE;
            }
            return FALSE;

        case    IDD_P01_BUTTON_05:
        case    IDD_P01_BUTTON_06:
        case    IDD_P01_BUTTON_07:
        case    IDD_P01_BUTTON_08:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Check Current Selected RadioButton ----- */
                P01SetBuff(hDlg, IDD_P01_BUTTON_05, IDD_P01_BUTTON_08, wParam, nAddress, P01_CHK_2BITS, P01_2BITSHIFT);
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
**  Synopsis:   BOOL    WINAPI  P001EditCDEDlgProc()
*
**  Input   :   HWND    hDlg   - handle of a dialogbox procedure
*               WORD    wMsg   - dispacthed message
*               WORD    wParam - 16 bits message parameter
*               LONG    lParam - 32 bits message parameter
*
**  Return  :   TRUE           - user process is executed
*               FALSE          - default process is expected
*
**  Description:
*       This is a sub-dialgbox procedure (Type C, D, E) for the MDC Maintenance.
* ===========================================================================
*/
BOOL WINAPI P001EditCDEDlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  int     nAddress;
    static  WPARAM  wCheckID;

    BYTE    bShift;

    switch (wMsg) {

    case PM_SELECTCHANGE:
        SetActiveWindow(hDlg);
        SendDlgItemMessage(hDlg, wCheckID, WM_KILLFOCUS, 0L, 0L);

    case WM_INITDIALOG:
 		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE,0));
		/* ----- Initialize Configulation of Sub-DialogBox ----- */
        nAddress = P01InitSubDlg(hDlg);
        /* ----- Check RadioButton with Current MDC Data ----- */
        wCheckID = P01SetData(hDlg, nAddress);
        /* ----- Set Focus to the Top Index of RadioButton ----- */
        SetFocus(GetDlgItem(hDlg, wCheckID));
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, 1);
        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j=IDD_P01_DESC_1; j<=IDD_P01_BUTTON_16; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
		}
		return FALSE;

    case WM_COMMAND:
        wCheckID = wParam;

        switch (LOWORD(wParam)) {
        case    IDD_P01_BUTTON_01:
        case    IDD_P01_BUTTON_02:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Set Bit Shift Count ----- */
                if (wSubDlgID == IDD_P01_DLG_D) {
                    bShift = P01_2BITSHIFT;
                } else {
                    bShift = 0;
                }

                /* ----- Check Current Selected RadioButton ----- */
                P01SetBuff(hDlg, IDD_P01_BUTTON_01, IDD_P01_BUTTON_02, wParam, nAddress, P01_CHK_BIT, bShift);

                return TRUE;
            } 
            return FALSE;

        case    IDD_P01_BUTTON_03:
        case    IDD_P01_BUTTON_04:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Set Bit Shift Count ----- */
                if (wSubDlgID == IDD_P01_DLG_C) {
                    bShift = P01_1BITSHIFT;
                } else {
                    bShift = P01_3BITSHIFT;
                }

                /* ----- Check Current Selected RadioButton ----- */
                P01SetBuff(hDlg, IDD_P01_BUTTON_03, IDD_P01_BUTTON_04, wParam, nAddress, P01_CHK_BIT, bShift);

                return TRUE;
            } 
            return FALSE;

        case    IDD_P01_BUTTON_05:
        case    IDD_P01_BUTTON_06:
        case    IDD_P01_BUTTON_07:
        case    IDD_P01_BUTTON_08:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Set Bit Shift Count ----- */
                if (wSubDlgID == IDD_P01_DLG_C) {
                    bShift = P01_2BITSHIFT;
                } else if (wSubDlgID == IDD_P01_DLG_E) {
                    bShift = P01_1BITSHIFT;
                } else {
                    bShift = 0;
                }

                /* ----- Check Current Selected RadioButton ----- */
                P01SetBuff(hDlg, IDD_P01_BUTTON_05, IDD_P01_BUTTON_08, wParam, nAddress, P01_CHK_2BITS, bShift);
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
**  Synopsis:   BOOL    WINAPI  P001EditFGDlgProc()
*
**  Input   :   HWND    hDlg   - handle of a dialogbox procedure
*               WORD    wMsg   - dispacthed message
*               WORD    wParam - 16 bits message parameter
*               LONG    lParam - 32 bits message parameter
*
**  Return  :   TRUE           - user process is executed
*               FALSE          - default process is expected
*
**  Description:
*       This is a sub-dialgbox procedure (Type F, G) for the MDC Maintenance.
* ===========================================================================
*/
BOOL WINAPI P001EditFGDlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  int     nAddress;
    static  WPARAM  wCheckID;

    BYTE    bShift;

    switch (wMsg) {

    case PM_SELECTCHANGE:
        SetActiveWindow(hDlg);
        SendDlgItemMessage(hDlg, wCheckID, WM_KILLFOCUS, 0L, 0L);

    case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE,0));

        /* ----- Initialize Configulation of Sub-DialogBox ----- */
        nAddress = P01InitSubDlg(hDlg);

        /* ----- Check RadioButton with Current MDC Data ----- */
        wCheckID = P01SetData(hDlg, nAddress);

        /* ----- Set Focus to the Top Index of RadioButton ----- */
        SetFocus(GetDlgItem(hDlg, wCheckID));

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, 1);
        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j=IDD_P01_DESC_1; j<=IDD_P01_BUTTON_10; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
		}
		return FALSE;

    case WM_COMMAND:
        wCheckID = wParam;

        switch (LOWORD(wParam)) {
        case    IDD_P01_BUTTON_01:
        case    IDD_P01_BUTTON_02:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Set Bit Shift Count ----- */
                if (wSubDlgID == IDD_P01_DLG_F) {
                    bShift = P01_3BITSHIFT;
                } else {
                    bShift = 0;
                }

                /* ----- Check Current Selected RadioButton ----- */
                P01SetBuff(hDlg, IDD_P01_BUTTON_01, IDD_P01_BUTTON_02, wParam, nAddress, P01_CHK_BIT, bShift);
                return TRUE;
            }
            return FALSE;

        case    IDD_P01_BUTTON_03:
        case    IDD_P01_BUTTON_04:
        case    IDD_P01_BUTTON_05:
        case    IDD_P01_BUTTON_06:
        case    IDD_P01_BUTTON_07:
        case    IDD_P01_BUTTON_08:
        case    IDD_P01_BUTTON_09:
        case    IDD_P01_BUTTON_10:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Set Bit Shift Count ----- */
                if (wSubDlgID == IDD_P01_DLG_G){
                    bShift = P01_1BITSHIFT;
                } else {
                    bShift = 0;
                }

                /* ----- Check Current Selected RadioButton ----- */
                P01SetBuff(hDlg, IDD_P01_BUTTON_03, IDD_P01_BUTTON_10, wParam, nAddress, P01_CHK_3BITS, bShift);
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
**  Synopsis:   BOOL    WINAPI  P001EditHDlgProc()
*
**  Input   :   HWND    hDlg   - handle of a dialogbox procedure
*               WORD    wMsg   - dispacthed message
*               WORD    wParam - 16 bits message parameter
*               LONG    lParam - 32 bits message parameter
*
**  Return  :   TRUE           - user process is executed
*               FALSE          - default process is expected
*
**  Description:
*       This is a sub-dialgbox procedure (Type H) for the MDC Maintenance.
* ===========================================================================
*/
BOOL WINAPI P001EditHDlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  int     nAddress;
    static  WPARAM  wCheckID;

    switch (wMsg) {
    case PM_SELECTCHANGE:
        SetActiveWindow(hDlg);
        SendDlgItemMessage(hDlg, wCheckID, WM_KILLFOCUS, 0L, 0L);

    case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE,0));

        /* ----- Initialize Configulation of Sub-DialogBox ----- */
        nAddress = P01InitSubDlg(hDlg);

        /* ----- Check RadioButton with Current MDC Data ----- */
        wCheckID = P01SetData(hDlg, nAddress);

        /* ----- Set Focus to the Top Index of RadioButton ----- */
        SetFocus(GetDlgItem(hDlg, wCheckID));
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, 1);
        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j=IDD_P01_DESC_1; j<=IDD_P01_BUTTON_16; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
		}
		return FALSE;

    case WM_COMMAND:
        wCheckID = wParam;
        switch (LOWORD(wParam)) {
        case    IDD_P01_BUTTON_01:
        case    IDD_P01_BUTTON_02:
        case    IDD_P01_BUTTON_03:
        case    IDD_P01_BUTTON_04:
        case    IDD_P01_BUTTON_05:
        case    IDD_P01_BUTTON_06:
        case    IDD_P01_BUTTON_07:
        case    IDD_P01_BUTTON_08:
        case    IDD_P01_BUTTON_09:
        case    IDD_P01_BUTTON_10:
        case    IDD_P01_BUTTON_11:
        case    IDD_P01_BUTTON_12:
        case    IDD_P01_BUTTON_13:
        case    IDD_P01_BUTTON_14:
        case    IDD_P01_BUTTON_15:
        case    IDD_P01_BUTTON_16:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Check Current Selected RadioButton ----- */
                P01SetBuff(hDlg, IDD_P01_BUTTON_01, IDD_P01_BUTTON_16, wParam, nAddress, P01_CHK_4BITS, 0);
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
**  Synopsis:   BOOL    WINAPI  P001Addr367DlgProc()
*
**  Input   :   HWND    hDlg   - handle of a dialogbox procedure
*               WORD    wMsg   - dispacthed message
*               WORD    wParam - 16 bits message parameter
*               LONG    lParam - 32 bits message parameter
*
**  Return  :   TRUE           - user process is executed
*               FALSE          - default process is expected
*
**  Description:
*       This is a sub-dialgbox procedure (Addr. 367) for the MDC Maintenance.
* ===========================================================================
*/
BOOL WINAPI P001Addr367DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  int     nAddress;
    static  WPARAM  wCheckID;

    switch (wMsg) {

    case PM_SELECTCHANGE:
        SetActiveWindow(hDlg);
        SendDlgItemMessage(hDlg, wCheckID, WM_KILLFOCUS, 0L, 0L);

    case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE,0));

        /* ----- Initialize Configulation of Sub-DialogBox ----- */
        nAddress = P01InitSubDlg(hDlg);

        /* ----- Check RadioButton with Current MDC Data ----- */
        wCheckID = P01SetData(hDlg, nAddress);
        SetFocus(GetDlgItem(hDlg, wCheckID));
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, 1);
        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=IDD_P01_DESC_1; j<=IDD_P01_BUTTON_04; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
		}
		return FALSE;

    case WM_COMMAND:
        wCheckID = wParam;
        switch (LOWORD(wParam)) {
        case    IDD_P01_BUTTON_01:
        case    IDD_P01_BUTTON_02:
        case    IDD_P01_BUTTON_03:
        case    IDD_P01_BUTTON_04:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Check Current Selected RadioButton ----- */
                P01SetBuff(hDlg, IDD_P01_BUTTON_01, IDD_P01_BUTTON_04, wParam, nAddress, P01_CHK_2BITS, 0);
                return TRUE;
            }
            return FALSE;
        }
        return FALSE;

    default:
        return FALSE;
    }
}

/*
* ===========================================================================
**  Synopsis:   BOOL    WINAPI  P001Addr16DlgProc()
*
**  Input   :   HWND    hDlg   - handle of a dialogbox procedure
*               WORD    wMsg   - dispacthed message
*               WORD    wParam - 16 bits message parameter
*               LONG    lParam - 32 bits message parameter
*
**  Return  :   TRUE           - user process is executed
*               FALSE          - default process is expected
*
**  Description:
*       This is a sub-dialgbox procedure (Addr. 16) for the MDC Maintenance.
* ===========================================================================
*/
BOOL WINAPI P001Addr16DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    BYTE    bMDCWork;
	USHORT  usBuffAddr;
    WORD    wCheckID;
    
    switch (wMsg) {

    case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE,0));

        /* ----- Delete "Close" from system menu ----- */
        DeleteMenu(GetSystemMenu(hDlg, FALSE), SC_CLOSE, MF_BYCOMMAND);

        /* ----- Get Current MDC Status ----- */
        usBuffAddr = ((P01_CAN_TAX_ADDR - 1) / P01_CHK_ADDR);

        bMDCWork = P01_GET_HIBIT(abMDCData[usBuffAddr]);

        /* ----- Check RadioButton with Current Data ----- */
        wCheckID = (WORD)(((bMDCWork >> P01_2BITSHIFT) & P01_CHK_BIT) ? IDD_P01_BUTTON_05:
                          ((bMDCWork >> P01_1BITSHIFT) & P01_CHK_BIT) ? IDD_P01_BUTTON_04:
                                                                        IDD_P01_BUTTON_03);

        CheckRadioButton(hDlg, IDD_P01_BUTTON_03, IDD_P01_BUTTON_05, wCheckID);

        wCheckID = (WORD)(((bMDCWork >> P01_3BITSHIFT) & P01_CHK_BIT) ? IDD_P01_BUTTON_08: IDD_P01_BUTTON_07);

        CheckRadioButton(hDlg, IDD_P01_BUTTON_07, IDD_P01_BUTTON_08, wCheckID);

        wCheckID = (WORD)((bMDCWork & P01_CHK_BIT) ? IDD_P01_BUTTON_02: IDD_P01_BUTTON_01);

        CheckRadioButton(hDlg, IDD_P01_BUTTON_01, IDD_P01_BUTTON_02, wCheckID);

        /* ----- Set Focus to the Top Index of RadioButton ----- */
        SetFocus(GetDlgItem(hDlg, wCheckID));
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, 1);
        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j=IDD_P01_DESC_1; j<=IDD_P01_BUTTON_08; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
		}
		return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDD_P01_BUTTON_01:
        case IDD_P01_BUTTON_02:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Check Current Selected RadioButton ----- */
                P01SetBuff(hDlg, IDD_P01_BUTTON_01, IDD_P01_BUTTON_02, wParam, P01_CAN_TAX_ADDR, P01_CHK_BIT, 0);
                return TRUE;
            }
            return FALSE;

        case IDD_P01_BUTTON_03:
        case IDD_P01_BUTTON_04:
        case IDD_P01_BUTTON_05:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Check Current Selected RadioButton ----- */
                P01SetBuff(hDlg, IDD_P01_BUTTON_03, IDD_P01_BUTTON_05, wParam, P01_CAN_TAX_ADDR, P01_CHK_2BITS, P01_1BITSHIFT);
                return TRUE;
            }
            return FALSE;

        case IDD_P01_BUTTON_07:
        case IDD_P01_BUTTON_08:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Check Current Selected RadioButton ----- */
                P01SetBuff(hDlg, IDD_P01_BUTTON_07, IDD_P01_BUTTON_08, wParam, P01_CAN_TAX_ADDR, P01_CHK_BIT, P01_3BITSHIFT);
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
**  Synopsis:   BOOL    WINAPI  P001Addr25DlgProc()
*
**  Input   :   HWND    hDlg   - handle of a dialogbox procedure
*               WORD    wMsg   - dispacthed message
*               WORD    wParam - 16 bits message parameter
*               LONG    lParam - 32 bits message parameter
*
**  Return  :   TRUE           - user process is executed
*               FALSE          - default process is expected
*
**  Description:
*       This is a sub-dialgbox procedure (Addr. 25) for the MDC Maintenance.
* ===========================================================================
*/
BOOL WINAPI P001Addr25DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    BYTE    bMDCWork;
	USHORT  usBuffAddr;
    WORD    wCheckID;
    
    switch (wMsg) {

    case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE,0));

        /* ----- Delete "Close" from system menu ----- */
        DeleteMenu(GetSystemMenu(hDlg, FALSE), SC_CLOSE, MF_BYCOMMAND);

        /* ----- Get Current MDC Status ----- */
        usBuffAddr = ((P01_CHAR_TIPS_ADDR - 1) / P01_CHK_ADDR);

        bMDCWork = P01_GET_LOBIT(abMDCData[usBuffAddr]);

        /* ----- Check RadioButton with Current Data ----- */
        if ((bMDCWork >> P01_3BITSHIFT) & P01_CHK_BIT) {
            wCheckID = IDD_P01_BUTTON_09;
        } else {
            wCheckID = (WORD)(IDD_P01_BUTTON_01 + (bMDCWork & P01_CHK_3BITS));
        }

        CheckRadioButton(hDlg, IDD_P01_BUTTON_01, IDD_P01_BUTTON_09, wCheckID);

        /* ----- Set Focus to the Top Index of RadioButton ----- */
        SetFocus(GetDlgItem(hDlg, wCheckID));
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, 1);
        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j=IDD_P01_DESC_1; j<=IDD_P01_BUTTON_09; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
		}
		return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDD_P01_BUTTON_01:
        case IDD_P01_BUTTON_02:
        case IDD_P01_BUTTON_03:
        case IDD_P01_BUTTON_04:
        case IDD_P01_BUTTON_05:
        case IDD_P01_BUTTON_06:
        case IDD_P01_BUTTON_07:
        case IDD_P01_BUTTON_08:
        case IDD_P01_BUTTON_09:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Check Current Selected RadioButton ----- */
                P01SetBuff(hDlg, IDD_P01_BUTTON_01, IDD_P01_BUTTON_09, wParam, P01_CHAR_TIPS_ADDR, P01_CHK_4BITS, 0);
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
**  Synopsis:   BOOL    WINAPI  P001Addr175DlgProc()
*
**  Input   :   HWND    hDlg   - handle of a dialogbox procedure
*               WORD    wMsg   - dispacthed message
*               WORD    wParam - 16 bits message parameter
*               LONG    lParam - 32 bits message parameter
*
**  Return  :   TRUE           - user process is executed
*               FALSE          - default process is expected
*
**  Description:
*       This is a sub-dialgbox procedure (Addr. 175/176) for the MDC Maintenance.
* ===========================================================================
*/
BOOL WINAPI P001Addr175DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  int     nAddress, i;
    static  USHORT  usBuffAddr;
    WCHAR   szWork[128], szDesc[128], szCaption[128];
    BOOL    fTrans;
    WORD    wIndex;
    UINT    wStrID;
    UINT    wStrIDx[2][4];
    UINT    unMDCData, unEditTimerValue;

    switch (wMsg) {
    case PM_SELECTCHANGE:
    case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE,0));

        /* ----- Get Current Selected Index of Main ListBox ----- */
        wIndex = (WORD)SendDlgItemMessage(*lphProgDlgChk1, IDD_P01_LIST, LB_GETCURSEL, 0, 0L);

        /* ----- Get String of Current Selected Index ----- */
        DlgItemSendTextMessage(*lphProgDlgChk1, IDD_P01_LIST, LB_GETTEXT, wIndex, (LPARAM)szWork);

        /* ----- Calculate Address No. with Loaded String ----- */
        nAddress = _wtoi(szWork);

        /* ----- Set Caption to Window Title Bar ----- */
        LoadString(hResourceDll, IDS_P01_SUBCAP, szWork, PEP_STRING_LEN_MAC(szWork));
        wsPepf(szDesc, WIDE("%s %d"), szWork, nAddress);

        WindowRedrawText(hDlg, szDesc);

        /* ----- Set Description to StaticText ----- */
		wStrIDx[0][0] = IDD_P01_DESC_1;
		wStrIDx[0][1] = IDD_P01_DESC_2;
		wStrIDx[0][2] = IDD_P01_DESC_3;
		wStrIDx[0][3] = IDD_P01_DESC_4;
		 switch (nAddress) {
			case P01_DISP_FLY_ADDR1:
				wStrIDx[1][0] = IDS_P01_BIT_175_0;
				wStrIDx[1][1] = IDS_P01_BIT_175_1;
				wStrIDx[1][2] = IDS_P01_BIT_175_2;
				wStrIDx[1][3] = IDS_P01_BIT_175_3;

				break;
			case P01_DISP_FLY_ADDR2:
				wStrIDx[1][0] = IDS_P01_BIT_176_0;
				wStrIDx[1][1] = IDS_P01_BIT_176_1;
				wStrIDx[1][2] = IDS_P01_BIT_176_2;
				wStrIDx[1][3] = IDS_P01_BIT_176_3;
				break;
			case P01_DISP_PRT_ADDR:
				wStrIDx[1][0] = IDS_P01_BIT_179_0;
				wStrIDx[1][1] = IDS_P01_BIT_179_1;
				wStrIDx[1][2] = IDS_P01_BIT_179_2;
				wStrIDx[1][3] = IDS_P01_BIT_179_3;
				break;
			case MDC_CPM_INTVL_TIME:
				wStrIDx[1][0] = IDS_P01_BIT_197_0;
				wStrIDx[1][1] = IDS_P01_BIT_197_1;
				wStrIDx[1][2] = IDS_P01_BIT_197_2;
				wStrIDx[1][3] = IDS_P01_BIT_197_3;
				break;
			case MDC_CPM_WAIT_TIME:
				wStrIDx[1][0] = IDS_P01_BIT_198_0;
				wStrIDx[1][1] = IDS_P01_BIT_198_1;
				wStrIDx[1][2] = IDS_P01_BIT_198_2;
				wStrIDx[1][3] = IDS_P01_BIT_198_3;

				break;
			case MDC_EPT_SND_TIME:
				wStrIDx[1][0] = IDS_P01_BIT_199_0;
				wStrIDx[1][1] = IDS_P01_BIT_199_1;
				wStrIDx[1][2] = IDS_P01_BIT_199_2;
				wStrIDx[1][3] = IDS_P01_BIT_199_3;
				break;

			case MDC_AUTO_SIGNOUT_ADR:
				wStrIDx[1][0] = IDS_P01_BIT_476_0;
				wStrIDx[1][1] = IDS_P01_BIT_476_1;
				wStrIDx[1][2] = IDS_P01_BIT_476_2;
				wStrIDx[1][3] = IDS_P01_BIT_476_3;
				break;

			case MDC_DRAWEROPEN_TIME:
				wStrIDx[1][0] = IDS_P01_BIT_481_0;
				wStrIDx[1][1] = IDS_P01_BIT_481_1;
				wStrIDx[1][2] = IDS_P01_BIT_481_2;
				wStrIDx[1][3] = IDS_P01_BIT_481_3;
				break;

			case MDC_SF_ALERT_TIME:
				wStrIDx[1][0] = IDS_P01_BIT_523_0;
				wStrIDx[1][1] = IDS_P01_BIT_523_1;
				wStrIDx[1][2] = IDS_P01_BIT_523_2;
				wStrIDx[1][3] = IDS_P01_BIT_523_3;
				break;

			case MDC_SF_AUTOOFF_TIME:
				wStrIDx[1][0] = IDS_P01_BIT_525_0;
				wStrIDx[1][1] = IDS_P01_BIT_525_1;
				wStrIDx[1][2] = IDS_P01_BIT_525_2;
				wStrIDx[1][3] = IDS_P01_BIT_525_3;
				break;

			case MDC_SF_CNTRL_ALERT:
				wStrIDx[1][0] = IDS_P01_BIT_531_0;
				wStrIDx[1][1] = IDS_P01_BIT_531_1;
				wStrIDx[1][2] = IDS_P01_BIT_531_2;
				wStrIDx[1][3] = IDS_P01_BIT_531_3;
				break;

			case MDC_SF_CNTRL_AUTOOFF:
				wStrIDx[1][0] = IDS_P01_BIT_533_0;
				wStrIDx[1][1] = IDS_P01_BIT_533_1;
				wStrIDx[1][2] = IDS_P01_BIT_533_2;
				wStrIDx[1][3] = IDS_P01_BIT_533_3;
				break;

			case MDC_SF_TRANRULE_TIME:
				wStrIDx[1][0] = IDS_P01_BIT_527_0;
				wStrIDx[1][1] = IDS_P01_BIT_527_1;
				wStrIDx[1][2] = IDS_P01_BIT_527_2;
				wStrIDx[1][3] = IDS_P01_BIT_527_3;
				break;

			case MDC_RECEIPT_POST_LMT:
				wStrIDx[1][0] = IDS_P01_BIT_536_0;
				wStrIDx[1][1] = IDS_P01_BIT_536_1;
				wStrIDx[1][2] = IDS_P01_BIT_536_2;
				wStrIDx[1][3] = IDS_P01_BIT_536_3;
				break;

			case MDC_RECEIPT_GIFT_LMT:
				wStrIDx[1][0] = IDS_P01_BIT_537_0;
				wStrIDx[1][1] = IDS_P01_BIT_537_1;
				wStrIDx[1][2] = IDS_P01_BIT_537_2;
				wStrIDx[1][3] = IDS_P01_BIT_537_3;
				break;

			case MDC_RECEIPT_POD_LMT:
				wStrIDx[1][0] = IDS_P01_BIT_538_0;
				wStrIDx[1][1] = IDS_P01_BIT_538_1;
				wStrIDx[1][2] = IDS_P01_BIT_538_2;
				wStrIDx[1][3] = IDS_P01_BIT_538_3;
				break;

			default:
				wStrIDx[1][0] = IDS_P01_BIT_200_0;
				wStrIDx[1][1] = IDS_P01_BIT_200_1;
				wStrIDx[1][2] = IDS_P01_BIT_200_2;
				wStrIDx[1][3] = IDS_P01_BIT_199_3;
				break;
		 }

		 for (i = 0; i < 4; i++) {
			LoadString(hResourceDll, wStrIDx[1][i], szWork, PEP_STRING_LEN_MAC(szWork));
			DlgItemRedrawText(hDlg, wStrIDx[0][i], szWork);
		 }

        /* ----- Get Current MDC Status ----- */
        /* ----- Limit Maximum Length of Data to EditText ----- */
        usBuffAddr = ((nAddress - 1) / P01_CHK_ADDR);

		if ((nAddress == MDC_SF_ALERT_TIME) || (nAddress == MDC_SF_AUTOOFF_TIME) || (nAddress == MDC_SF_TRANRULE_TIME) ||
			(nAddress == MDC_SF_CNTRL_ALERT) || (nAddress == MDC_SF_CNTRL_AUTOOFF)) {
			unMDCData = (UINT)(abMDCData[usBuffAddr]);
			SendDlgItemMessage(hDlg, IDD_P01_EDIT, EM_LIMITTEXT, P01_EDIT_LEN3, 0L);
		} else if(nAddress != MDC_AUTO_SIGNOUT_ADR && nAddress != MDC_DRAWEROPEN_TIME){
			unMDCData = (UINT)((nAddress % P01_CHK_ADDR) ? P01_GET_LOBIT(abMDCData[usBuffAddr]): P01_GET_HIBIT(abMDCData[usBuffAddr]));
			SendDlgItemMessage(hDlg, IDD_P01_EDIT, EM_LIMITTEXT, P01_EDIT_LEN, 0L);
		}
		else {
			unMDCData = (UINT)(abMDCData[usBuffAddr]);
			SendDlgItemMessage(hDlg, IDD_P01_EDIT, EM_LIMITTEXT, P01_EDIT_LEN3, 0L);
		}

		SetDlgItemInt(hDlg, IDD_P01_EDIT, unMDCData, FALSE);

        /********** Initial Data Set ******************************/
        if (nAddress == MDC_CPM_INTVL_TIME) {
            wStrID = IDS_P01_BIT_197_3;
			LoadString(hResourceDll, wStrID, szWork, PEP_STRING_LEN_MAC(szWork));

            /* Calculate Edit Timer Value */
            unEditTimerValue = unMDCData + 1;
            wsPepf(szDesc, szWork, unEditTimerValue);
            DlgItemRedrawText(hDlg, IDD_P01_DESC_4, szDesc);
        } else if (nAddress == MDC_CPM_WAIT_TIME) {
            wStrID = IDS_P01_BIT_198_3;
            LoadString(hResourceDll, wStrID, szWork, PEP_STRING_LEN_MAC(szWork));

            /* Calculate Edit Timer Value */
            if (unMDCData  == 0) {
                unEditTimerValue = 90;
            } else {
                unEditTimerValue = unMDCData * 30;
            }

            wsPepf(szDesc, szWork, unEditTimerValue);
			DlgItemRedrawText(hDlg, IDD_P01_DESC_4, szDesc);
        } else if (nAddress == MDC_EPT_SND_TIME) {
            wStrID = IDS_P01_BIT_199_3;
            LoadString(hResourceDll, wStrID, szWork, PEP_STRING_LEN_MAC(szWork));

            /* Calculate Edit Timer Value */
            unEditTimerValue = unMDCData + 1;
            wsPepf(szDesc, szWork, unEditTimerValue);
            DlgItemRedrawText(hDlg, IDD_P01_DESC_4, szDesc);

        } else if (nAddress == MDC_EPT_RCV_TIME) {
            wStrID = IDS_P01_BIT_200_3;
			LoadString(hResourceDll, wStrID, szWork, PEP_STRING_LEN_MAC(szWork));

            /* Calculate Edit Timer Value */
            unEditTimerValue = (unMDCData + 1) * 30;
            wsPepf(szDesc, szWork, unEditTimerValue);
            DlgItemRedrawText(hDlg, IDD_P01_DESC_4, szDesc);
        } else if (nAddress == MDC_AUTO_SIGNOUT_ADR) {
            wStrID = IDS_P01_BIT_476_3;
			LoadString(hResourceDll, wStrID, szWork, PEP_STRING_LEN_MAC(szWork));

            /* Calculate Edit Timer Value */
            unEditTimerValue = (unMDCData + 1) ;
            wsPepf(szDesc, szWork, unEditTimerValue);
            DlgItemRedrawText(hDlg, IDD_P01_DESC_4, szDesc);
        } else if (nAddress == MDC_DRAWEROPEN_TIME) {
            wStrID = IDS_P01_BIT_481_3;
			LoadString(hResourceDll, wStrID, szWork, PEP_STRING_LEN_MAC(szWork));

            /* Calculate Edit Timer Value */
            unEditTimerValue = (unMDCData + 1) ;
            wsPepf(szDesc, szWork, unEditTimerValue);
            DlgItemRedrawText(hDlg, IDD_P01_DESC_4, szDesc);
        }

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, 1);
        return TRUE;

    case WM_VSCROLL:
        /* ----- Spin Button Procedure ----- */
		if ((nAddress == MDC_SF_ALERT_TIME) || (nAddress == MDC_SF_AUTOOFF_TIME) || (nAddress == MDC_SF_TRANRULE_TIME) ||
			(nAddress == MDC_SF_CNTRL_ALERT) || (nAddress == MDC_SF_CNTRL_AUTOOFF)) {
			P01SpinProc475(hDlg, wParam);
		} else if(nAddress != MDC_AUTO_SIGNOUT_ADR && nAddress != MDC_DRAWEROPEN_TIME)
		{
			P01SpinProc(hDlg, wParam);
		}
		else{
			P01SpinProc475(hDlg, wParam);
		}
        return FALSE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j=IDD_P01_DESC_1; j<=IDD_P01_DESC_4; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDD_P01_EDIT, WM_SETFONT, (WPARAM)hResourceFont, 0);

		}
		return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDD_P01_EDIT:
            if (HIWORD(wParam) == EN_CHANGE) {//RPH 1/22/03
                /* ----- Get Inputed MDC Value ----- */
                unMDCData = GetDlgItemInt(hDlg, LOWORD(wParam), NULL, FALSE);

				// Check to see if the data entered is out of range or not.
				// We do range checks and if out of range, display error dialog and then
				// break out of this do loop.  Using a do loop makes the logic easier as we can
				// use break statement to not do the memory update.
				do {
					/* ----- Check Inputed Data is Out of Range ----- */
					if ((nAddress == MDC_SF_ALERT_TIME) || (nAddress == MDC_SF_AUTOOFF_TIME) || (nAddress == MDC_SF_TRANRULE_TIME) ||
						(nAddress == MDC_SF_CNTRL_ALERT) || (nAddress == MDC_SF_CNTRL_AUTOOFF)) {
						if (unMDCData > P01_CHK_255BITS) {    /* Out of Range? */
							/* ----- Load MessageBox Caption from Resource ----- */
							LoadString(hResourceDll, IDS_PEP_CAPTION_P01, szCaption, PEP_STRING_LEN_MAC(szCaption));

							/* ----- Load Error Message from Resource ----- */
							LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_STRING_LEN_MAC(szWork));
							wsPepf(szDesc, szWork, 0, P01_CHK_255BITS);

							MessageBeep(MB_ICONEXCLAMATION);
							MessageBoxPopUp(hDlg, szDesc, szCaption, MB_ICONEXCLAMATION | MB_OK);

							SendDlgItemMessage(hDlg, LOWORD(wParam), EM_UNDO, 0, 0L);
							break;
						}
					}
					else if (nAddress == MDC_AUTO_SIGNOUT_ADR || nAddress == MDC_DRAWEROPEN_TIME) {
						if (unMDCData > P01_CHK_255BITS) {   /* Out of Range ? */
							/* ----- Load MessageBox Caption from Resource ----- */
							LoadString(hResourceDll, IDS_PEP_CAPTION_P01, szCaption, PEP_STRING_LEN_MAC(szCaption));

							/* ----- Load Error Message from Resource ----- */
							LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_STRING_LEN_MAC(szWork));
							wsPepf(szDesc, szWork, 0, P01_CHK_255BITS);

							MessageBeep(MB_ICONEXCLAMATION);
							MessageBoxPopUp(hDlg, szDesc, szCaption, MB_ICONEXCLAMATION | MB_OK);

							SendDlgItemMessage(hDlg, LOWORD(wParam), EM_UNDO, 0, 0L);
							break;
						}
					} else {
						if (unMDCData > P01_CHK_4BITS) {   /* Out of Range? */
							/* ----- Load MessageBox Caption from Resource ----- */
							LoadString(hResourceDll, IDS_PEP_CAPTION_P01, szCaption, PEP_STRING_LEN_MAC(szCaption));

							/* ----- Load Error Message from Resource ----- */
							LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_STRING_LEN_MAC(szWork));
							wsPepf(szDesc, szWork, 0, P01_CHK_4BITS);

							MessageBeep(MB_ICONEXCLAMATION);
							MessageBoxPopUp(hDlg, szDesc, szCaption, MB_ICONEXCLAMATION | MB_OK);

							SendDlgItemMessage(hDlg, LOWORD(wParam), EM_UNDO, 0, 0L);
							break;
						}
					} 

                    /* Address is Odd (Set to Lo-Bit) */
					if ((nAddress == MDC_SF_ALERT_TIME) || (nAddress == MDC_SF_AUTOOFF_TIME) || (nAddress == MDC_SF_TRANRULE_TIME) ||
						(nAddress == MDC_SF_CNTRL_ALERT) || (nAddress == MDC_SF_CNTRL_AUTOOFF)) {
						abMDCData[usBuffAddr] = (BYTE)unMDCData;
					} else if(nAddress != MDC_AUTO_SIGNOUT_ADR && nAddress != MDC_DRAWEROPEN_TIME){
						if (nAddress % P01_CHK_ADDR) {
							abMDCData[usBuffAddr] = P01_SET_LOBIT(abMDCData[usBuffAddr], (BYTE)unMDCData);
						/* Address is Even(Set to Hi-Bit) */
						} else {
							abMDCData[usBuffAddr] = P01_SET_HIBIT(abMDCData[usBuffAddr], (BYTE)unMDCData);
						}
                    }
					else{
						abMDCData[usBuffAddr] = (BYTE)unMDCData;
					}
                    /* ----- Get Current Selected Index of Main ListBox ----- */
                    wIndex = (WORD)SendDlgItemMessage(*lphProgDlgChk1, IDD_P01_LIST, LB_GETCURSEL, 0, 0L);

                    /* ----- Get String of Current Selected Index ----- */
                    DlgItemSendTextMessage(*lphProgDlgChk1, IDD_P01_LIST, LB_GETTEXT, wIndex, (LPARAM)(szWork));

                    /* ----- Calculate Address No. with Loaded String ----- */
                    nAddress = _wtoi(szWork);

                    if (nAddress == MDC_CPM_WAIT_TIME) {
                        wStrID = IDS_P01_BIT_198_3;
						LoadString(hResourceDll, wStrID, szWork, PEP_STRING_LEN_MAC(szWork));

                        /* Calculate Edit Timer Value */
                        if (unMDCData  == 0) {
                            unEditTimerValue = 90;
                        } else {
                            unEditTimerValue = unMDCData * 30;
                        }

                        wsPepf(szDesc, szWork, unEditTimerValue);
                        DlgItemRedrawText(hDlg, IDD_P01_DESC_4, szDesc);
                    } else if (nAddress == MDC_EPT_RCV_TIME) {
                        wStrID = IDS_P01_BIT_200_3;
                        LoadString(hResourceDll, wStrID,szWork, PEP_STRING_LEN_MAC(szWork));

                        /* Calculate Edit Timer Value */
                        unEditTimerValue = unMDCData * 30 + 30;
                        wsPepf(szDesc, szWork, unEditTimerValue);
                        DlgItemRedrawText(hDlg, IDD_P01_DESC_4, szDesc);
                    } else if (nAddress == MDC_EPT_SND_TIME) {
                        wStrID = IDS_P01_BIT_199_3;
						LoadString(hResourceDll, wStrID, szWork, PEP_STRING_LEN_MAC(szWork));

                        /* Caluculate Edit Timer Value */
                        unEditTimerValue = unMDCData + 1;
                        wsPepf(szDesc, szWork, unEditTimerValue);
                        DlgItemRedrawText(hDlg, IDD_P01_DESC_4, szDesc);
                    } else if (nAddress == MDC_CPM_INTVL_TIME) {
                        wStrID = IDS_P01_BIT_197_3;
						LoadString(hResourceDll, wStrID, szWork, PEP_STRING_LEN_MAC(szWork));

                        /* Caluculate Edit Timer Value */
                        unEditTimerValue = unMDCData + 1;
                        wsPepf(szDesc, szWork, unEditTimerValue);
                        DlgItemRedrawText(hDlg, IDD_P01_DESC_4, szDesc);
                    }
                } while (0);  // do this only once.
                return TRUE;
            } else if (HIWORD(wParam) == EN_KILLFOCUS) {
                unMDCData = GetDlgItemInt(hDlg, LOWORD(wParam), (LPBOOL)&fTrans, FALSE);
                if (fTrans == 0) {
                    /* ----- Load Error Message ----- */
                    LoadString(hResourceDll, IDS_PEP_CAPTION_P01, szCaption, PEP_STRING_LEN_MAC(szCaption));
                    LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_STRING_LEN_MAC(szWork));
                    wsPepf(szDesc, szWork, P01_EDIT_MIN, P01_CHK_4BITS);

                    /* ----- Display Error Message ----- */
                    MessageBeep(MB_ICONEXCLAMATION);
                    MessageBoxPopUp(hDlg, szDesc, szCaption, MB_OK | MB_ICONEXCLAMATION);

                    SendMessage((HWND)lParam, EM_SETSEL, 1, MAKELONG(0, 1));
                    SetFocus((HWND)lParam);
                }
                return TRUE;
            }
            return FALSE;
        }
        return FALSE;

    default:
        return FALSE;
    } 
}

/****** add followed function V3.3 ********/
/*
* ===========================================================================
**  Synopsis:   BOOL    WINAPI  P001Addr259DlgProc()
*
**  Input   :   HWND    hDlg   - handle of a dialogbox procedure
*               WORD    wMsg   - dispacthed message
*               WORD    wParam - 16 bits message parameter
*               LONG    lParam - 32 bits message parameter
*
**  Return  :   TRUE           - user process is executed
*               FALSE          - default process is expected
*
**  Description:
*       This is a sub-dialgbox procedure (Addr. 259) for the MDC Maintenance.
* ===========================================================================
*/
BOOL WINAPI P001Addr259DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static int  nAddress;
    BYTE        bMDCWork;
    USHORT      usBuffAddr;
    WORD        wCheckID;

    switch (wMsg) {

    case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE,0));

        /* ----- Initialize Configulation of Sub-DialogBox ----- */
        nAddress = P01InitSubDlg(hDlg);

        /* ----- Get Current MDC Status ----- */
        usBuffAddr = ((P01_VAT_ADDR - 1) / P01_CHK_ADDR);

        bMDCWork = P01_GET_LOBIT(abMDCData[usBuffAddr]);

        /* ----- Check RadioButton with Current Data ----- */
        /* ----- Check RadioButton with Current Data ----- */
        wCheckID = (WORD)(((bMDCWork >> P01_1BITSHIFT) & P01_CHK_BIT) ? IDD_P01_BUTTON_03:
                          ( bMDCWork & P01_CHK_BIT)                   ? IDD_P01_BUTTON_02: IDD_P01_BUTTON_01);

        CheckRadioButton(hDlg, IDD_P01_BUTTON_01, IDD_P01_BUTTON_03, wCheckID);

        wCheckID = (WORD)(((bMDCWork >> P01_2BITSHIFT) & P01_CHK_BIT) ? IDD_P01_BUTTON_06: IDD_P01_BUTTON_05);

        CheckRadioButton(hDlg, IDD_P01_BUTTON_05, IDD_P01_BUTTON_06, wCheckID);

        wCheckID = (WORD)(((bMDCWork >> P01_3BITSHIFT) & P01_CHK_BIT) ? IDD_P01_BUTTON_08: IDD_P01_BUTTON_07);

        CheckRadioButton(hDlg, IDD_P01_BUTTON_07, IDD_P01_BUTTON_08, wCheckID);

        /* ----- Set Focus to the Top Index of RadioButton ----- */
        SetFocus(GetDlgItem(hDlg, wCheckID));

        /* ----- Set Focus to the Top Index of RadioButton ----- */
        SetFocus(GetDlgItem(hDlg, wCheckID));
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, 1);

        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j=IDD_P01_DESC_1; j<=IDD_P01_BUTTON_08; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
		}
		return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDD_P01_BUTTON_01:
        case IDD_P01_BUTTON_02:
        case IDD_P01_BUTTON_03:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Check Current Selected RadioButton ----- */
                P01SetBuff(hDlg, IDD_P01_BUTTON_01, IDD_P01_BUTTON_03, wParam, P01_VAT_ADDR, P01_CHK_2BITS, 0);
                return TRUE;
            }
            return FALSE;

        case IDD_P01_BUTTON_05:
        case IDD_P01_BUTTON_06:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Check Current Selected RadioButton ----- */
                P01SetBuff(hDlg, IDD_P01_BUTTON_05, IDD_P01_BUTTON_06, wParam, P01_VAT_ADDR, P01_CHK_BIT, P01_2BITSHIFT);
                return TRUE;
            }
            return FALSE;

        case IDD_P01_BUTTON_07:
        case IDD_P01_BUTTON_08:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Check Current Selected RadioButton ----- */
                P01SetBuff(hDlg, IDD_P01_BUTTON_07, IDD_P01_BUTTON_08, wParam, P01_VAT_ADDR, P01_CHK_BIT, P01_3BITSHIFT);
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
**  Synopsis:   int     P01InitSubDlg();
*
**  Input   :   HWND    hDlg   - handle of a dialogbox procedure
*
**  Return  :   nAddress    -   Current Selected Address (Not Zero-Based)
*
**  Description:
*       This function initializes configulation of Sub-DialogBox.
* ===========================================================================
*/
int     P01InitSubDlg(HWND hDlg)
{
    WCHAR   szWork[128], szDesc[128];
    int     nAddress;
    WORD    awStrID[P01_SUBDLG_NO];

    /* ----- Delete "Close" from system menu ----- */
    DeleteMenu(GetSystemMenu(hDlg, FALSE), SC_CLOSE, MF_BYCOMMAND);

    /* ----- Initialize Temporary Buffer ----- */
    memset((LPSTR)awStrID, 0, sizeof(awStrID));

    /* ----- Get Description ID of Button Group ----- */
    P01GetGrpStr((LPWORD)awStrID);

    /* ----- Set Description of Button Group to StaticText ----- */
    P01SetGrpStr(hDlg, (LPWORD)awStrID);

    /* ----- Set Description of Each Bits to RadioButton ----- */
    nAddress = P01SetRadioDesc(hDlg);

    /* ----- Create Sub-DialogBox Caption ----- */
	LoadString(hResourceDll, IDS_P01_SUBCAP,  szWork, PEP_STRING_LEN_MAC(szWork));

    wsPepf(szDesc, WIDE("%s %d"), szWork, nAddress);

    WindowRedrawText(hDlg, szDesc);

    return nAddress;
}

/*
* ===========================================================================
**  Synopsis:   VOID    P01GetGrpStr();
*
**  Input   :   LPWORD  lpwID   -   Points to Group Description
*
**  Return  :   No return value
*
**  Description:
*       This function caluculates Group Description ID
* ===========================================================================
*/
VOID    P01GetGrpStr(LPWORD lpwID)
{
    WCHAR   szWork[P01_TYPE_LEN], chNextChar;
    USHORT  usCount;
    USHORT  usWork;
    int     nCategory = 0, nOffset;

    memset(szWork, 0, sizeof(szWork));

    for (usCount = P01_TYPE_OFFSET, usWork = P01_TYPE_OFFSET;
         ((usCount < P01_TYPE_LEN) || (szDlgType[usCount] == 0));
         usCount++) {

        /* ----- Copy DialogBox Type String to Temporary Buffer ----- */
        szWork[usCount] = szDlgType[usCount];

        /* ----- Set Next Character to Temporary ----- */
        chNextChar = szDlgType[usCount + 1];

        if (chNextChar == P01_HYPHEN) {
            /* ----- Calculate Category No. with Copied String ----- */
            nCategory = _wtoi((const WCHAR *)&szWork[usWork]);

            /* ----- Reset Work Area ----- */
            memset((LPSTR)szWork, 0, sizeof(szWork));
            usCount++;
            usWork = (usCount + 1);
        } else if ((chNextChar == P01_PERIOD) || (chNextChar == 0)) {
            /* ----- Calculate Offset No. with Copied String ----- */
            nOffset = _wtoi((const WCHAR *)&szWork[usWork]);

            /* ----- Remove Category10 (Server), Now Category10 is changed Category13. ----- */
            if (nCategory >= 10) {
                nCategory--;
            }

            /* ----- Set Group Description to Buffer ----- */
            *lpwID++ = P01_GET_GRP_DESC(nCategory, nOffset);

            if (chNextChar == 0) {
                break;
            } else {
                /* ----- Reset Work Area ----- */
                memset((LPSTR)szWork, 0, sizeof(szWork));
                usCount++;
                usWork = (usCount + 1);
            }
        }
    }

}

/*
* ===========================================================================
**  Synopsis:   VOID    P01SetGrpStr();
*
**  Input   :   HWND    hDlg    -   Handle of DialogBox
*               LPWORD  lpwID   -   Points to Group Description
*
**  Return  :   No return value
*
**  Description:
*       This function displays description of bit group to statictext. Styles 
*	are used in this function to determine where MDC bit nmemonic titles are
*	to be placed above radio buttons. In the pep resource files, the styles/dialog 
*	determination follow the following layout. 
*	[DialogType][Style#].[Category#]-[IndexFromCateOffest]
*	A1.3-53 translates to Dialog 'A', Style '1', Category '3', and IndexFromCate
*	'53'. A similar explanation can be found in the beginning comments of P001.h.
* ===========================================================================
*/
VOID    P01SetGrpStr(HWND hDlg, LPWORD lpwID)
{
    WORD    awStaticID[P01_SUBDLG_NO], wEraseID, wI;
	WCHAR	szWork[256];

    /* ----- Reset Work Area ----- */
    memset(awStaticID, 0, sizeof(awStaticID));

    /* ----- Set StaticText ID for Text out ----- */
    awStaticID[0] = IDD_P01_DESC_1;

    switch (szDlgType[P01_CHK_GROUP]) {
    case P01_STYLE_2:
        switch (wSubDlgID) {
        case IDD_P01_DLG_A:
            awStaticID[1] = IDD_P01_DESC_3;
            break;
        default:
            awStaticID[1] = IDD_P01_DESC_2;
        }
        break;

    case P01_STYLE_3:
        switch (wSubDlgID) {
        case IDD_P01_DLG_A:
            awStaticID[1] = IDD_P01_DESC_2;
            break;
        default:
            awStaticID[1] = IDD_P01_DESC_3;
        }
        break;

    case P01_STYLE_4:
        switch (wSubDlgID) {
        case IDD_P01_DLG_A:
            awStaticID[1] = IDD_P01_DESC_4;
            break;
        default:
            awStaticID[1] = IDD_P01_DESC_2;
            awStaticID[2] = IDD_P01_DESC_3;
        }
        break;

    case P01_STYLE_5:
        awStaticID[1] = IDD_P01_DESC_2;
        awStaticID[2] = IDD_P01_DESC_3;
        break;

    case P01_STYLE_6:
        awStaticID[1] = IDD_P01_DESC_3;
        awStaticID[2] = IDD_P01_DESC_4;
        break;

    case P01_STYLE_7:
        awStaticID[1] = IDD_P01_DESC_2;
        awStaticID[2] = IDD_P01_DESC_4;
        break;

    case P01_STYLE_8:
        awStaticID[1] = IDD_P01_DESC_2;
        awStaticID[2] = IDD_P01_DESC_3;
        awStaticID[3] = IDD_P01_DESC_4;
        break;

    default:
        break;
    }

    /* ----- Erase Old Description from StaticText ----- */
    for (wEraseID = 0; wEraseID < P01_SUBDLG_NO; wEraseID++) {
		DlgItemRedrawText(hDlg, wEraseID + IDD_P01_DESC_1, NULL);
    }

    /* ----- Set Description of Group Address to StaticText ----- */
    for (wI = 0; wI < P01_SUBDLG_NO; wI++) {
        if (awStaticID[wI]) {
            /* ----- Load String from Resource ----- */
			if (0 == LoadString(hResourceDll, (UINT)*(lpwID + wI), szWork, PEP_STRING_LEN_MAC(szWork))) {
				wsPepf (szWork, WIDE("String %d not found P01SetGrpStr()."), (UINT)*(lpwID + wI));
			}
			DlgItemRedrawText(hDlg, awStaticID[wI], szWork);
        }
    }
}

/*
* ===========================================================================
**  Synopsis:   int     P01SetRadioDesc();
*
**  Input   :   HWND    hDlg    -   Handle of DailogBox
*
**  Return  :   nAddress    -   Current Selected Address No.
*
**  Description:
*       This function displays description of each bits to radiobutton.
* ===========================================================================
*/
int     P01SetRadioDesc(HWND hDlg)
{
    int     nAddress;
    WCHAR   szWork[128];
    WORD    wIndex, wI;

    /* ----- Get Current Selected Index of Main ListBox ----- */
    wIndex = (WORD)SendDlgItemMessage(*lphProgDlgChk1, IDD_P01_LIST, LB_GETCURSEL, 0, 0L);

    /* ----- Get String of Current Selected Index ----- */
    DlgItemSendTextMessage(*lphProgDlgChk1, IDD_P01_LIST, LB_GETTEXT, wIndex, (LPARAM)szWork);

    /* ----- Calculate Address No. with Loaded String ----- */
    nAddress = _wtoi(szWork);

    /* ----- Set Description of Bits to RadioButton ----- */
    switch (wSubDlgID) {
    case IDD_P01_DLG_H:
        for (wI = 0; wI < P01_BIT_DESC_16; wI++) {
            /* ----- Load Descripton of Bit from Resource ----- */
			if (0 == LoadString(hResourceDll, P01_BITDESC_OFFSET(nAddress) + wI, szWork, PEP_STRING_LEN_MAC(szWork))) {
				wsPepf (szWork, WIDE("String %d not found P01SetRadioDesc()."), P01_BITDESC_OFFSET(nAddress) + wI);
			}
			DlgItemRedrawText(hDlg, (WORD)(IDD_P01_BUTTON_01 + wI), szWork);
        }
        break;

    case IDD_P01_DLG_F:
    case IDD_P01_DLG_G:
        for (wI = 0; wI < P01_BIT_DESC_10; wI++) {
            /* ----- Load Descripton of Bit from Resource ----- */
			if (0 == LoadString(hResourceDll, P01_BITDESC_OFFSET(nAddress) + wI, szWork, PEP_STRING_LEN_MAC(szWork))) {
				wsPepf (szWork, WIDE("String %d not found P01SetRadioDesc()."), P01_BITDESC_OFFSET(nAddress) + wI);
			}
			DlgItemRedrawText(hDlg, (WORD)(IDD_P01_BUTTON_01 + wI), szWork);
        }
        break;

    default:
        for (wI = 0; wI < P01_BIT_DESC_8; wI++) {
            /* ----- Load Descripton of Bit from Resource ----- */
			if (0 == LoadString(hResourceDll, P01_BITDESC_OFFSET(nAddress) + wI, szWork, PEP_STRING_LEN_MAC(szWork))) {
				wsPepf (szWork, WIDE("String %d not found P01SetRadioDesc()."), P01_BITDESC_OFFSET(nAddress) + wI);
			}
			DlgItemRedrawText(hDlg, (WORD)(IDD_P01_BUTTON_01 + wI), szWork);
        }
    }

    return (nAddress);
}

/*
* ===========================================================================
**  Synopsis:   WPARAM  P01SetData();
*
**  Input   :   HWND    hDlg        -   Handle of DailogBox
*               int     nAddress    -   Current Selected Address No. 
*                                           (Not Zero-Based Value)
**  Return  :   wTopID    -   the Top Index of Checked RadioButton ID
*
**  Description:
*       This function checks RadioButton with Current MDC Data.
* ===========================================================================
*/
WPARAM  P01SetData(HWND hDlg, int nAddress) 
{
    WPARAM  wTopID = 0;
    BYTE    bMDCWork,
            bShift,
            bWork,
            bWork2,
            bWork3;
    WORD    wI,
            wStartID,
            wEndID,
            wCheckID;
    USHORT  usBuffAddr;

    /* ----- Get Target Address of Buffer ----- */
    usBuffAddr = ((nAddress - 1) / P01_CHK_ADDR);

    if (nAddress % P01_CHK_ADDR) {  /* Address is Odd (Get from Lo-Bit) */
        bMDCWork = P01_GET_LOBIT(abMDCData[usBuffAddr]);
    } else {                        /* Address is Even(Get from Hi-Bit) */
        bMDCWork = P01_GET_HIBIT(abMDCData[usBuffAddr]);
    }

    /* ----- Check RadoButton Each Sub-DialogBox Style ------ */
    switch (wSubDlgID) {

    case    IDD_P01_DLG_A:
        for (wI = 0; wI < P01_BIT_NO; wI++) {

            /* ----- Set RadioButton Group ----- */
            wStartID = (WORD)(IDD_P01_BUTTON_01 + (wI * P01_2GROUPS));
            wEndID   = (WORD)(IDD_P01_BUTTON_02 + (wI * P01_2GROUPS));

            /* ----- Check Current MDC Bit whether ON/OFF ----- */
            bWork = (BYTE)((bMDCWork >> wI) & P01_CHK_BIT);

            /* ----- Set RadioButton ID to Cehcked ----- */
            wCheckID = (WORD)(wStartID + bWork);

            /* ----- Check RadioButton ----- */
            CheckRadioButton(hDlg, wStartID, wEndID, wCheckID);

            if (wStartID == IDD_P01_BUTTON_01) {
                wTopID = wCheckID;
            }
        }
        break;

    case    IDD_P01_DLG_B:
        for (wI = 0; wI < P01_2GROUPS; wI++) {

            /* ----- Set RadioButton Group ----- */
            wStartID = (WORD)(IDD_P01_BUTTON_01 + (wI * P01_4GROUPS));
            wEndID   = (WORD)(IDD_P01_BUTTON_04 + (wI * P01_4GROUPS));

            /* ----- Set Bit Shift Count ----- */
            bShift = (BYTE)(P01_2BITSHIFT * wI);

            /* ----- Get 0 & 1 Bit Status ----- */
            bWork = (BYTE)((bMDCWork >> bShift) & P01_CHK_2BITS);

            /* ----- Set RadioButton ID to Cehcked ----- */
            wCheckID = (WORD)(wStartID + bWork);

            /* ----- Check RadioButton  ----- */
            CheckRadioButton(hDlg, wStartID, wEndID, wCheckID);

            if (wStartID == IDD_P01_BUTTON_01) {
                wTopID = wCheckID;
            }
        }
        break;

    case    IDD_P01_DLG_C:
        /* ----- Get Current MDC Data (Bit 0) ----- */
        bWork  = (BYTE)(bMDCWork & P01_CHK_BIT);

        /* ----- Get Current MDC Data (Bit 1) ----- */
        bWork2 = (BYTE)((bMDCWork >> P01_1BITSHIFT) & P01_CHK_BIT);

        /* ----- Get Current MDC Data (Bit 2 - 3) ----- */
        bWork3 = (BYTE)((bMDCWork >> P01_2BITSHIFT) & P01_CHK_2BITS);

        /* ----- Check RadioButton with Current MDC Data ----- */
        P01ChkTypeCDE(hDlg, bWork, bWork2, bWork3);

        wTopID = (WPARAM)(IDD_P01_BUTTON_01 + bWork);
        break;

    case    IDD_P01_DLG_D:
        /* ----- Get Current MDC Data (Bit 2) ----- */
        bWork  = (BYTE)((bMDCWork >> P01_2BITSHIFT) & P01_CHK_BIT);

        /* ----- Get Current MDC Data (Bit 3) ----- */
        bWork2 = (BYTE)((bMDCWork >> P01_3BITSHIFT) & P01_CHK_BIT);

        /* ----- Get Current MDC Data (Bit 0 - 1) ----- */
        bWork3 = (BYTE)(bMDCWork & P01_CHK_2BITS);

        /* ----- Check RadioButton with Current MDC Data ----- */
        P01ChkTypeCDE(hDlg, bWork, bWork2, bWork3);

        wTopID = (WPARAM)(IDD_P01_BUTTON_05 + bWork3);
        break;

    case    IDD_P01_DLG_E:
        /* ----- Get Current MDC Data (Bit 0) ----- */
        bWork  = (BYTE)(bMDCWork & P01_CHK_BIT);

        /* ----- Get Current MDC Data (Bit 3) ----- */
        bWork2 = (BYTE)((bMDCWork >> P01_3BITSHIFT) & P01_CHK_BIT);

        /* ----- Get Current MDC Data (Bit 1 - 2) ----- */
        bWork3 = (BYTE)((bMDCWork >> P01_1BITSHIFT) & P01_CHK_2BITS);

        /* ----- Check RadioButton with Current MDC Data ----- */
        P01ChkTypeCDE(hDlg, bWork, bWork2, bWork3);

        wTopID = (WPARAM)(IDD_P01_BUTTON_01 + bWork);
        break;

    case    IDD_P01_DLG_F:
        /*  ----- Get Current MDC Data (Bit 3) ----- */
        bWork  = (BYTE)((bMDCWork >> P01_3BITSHIFT) & P01_CHK_BIT);

        /*  ----- Get Current MDC Data (Bit 0 - 2) ----- */
        bWork2 = (BYTE)(bMDCWork & P01_CHK_3BITS);

        /* ----- Check RadioButton with Current MDC Data ----- */
        P01ChkTypeFG(hDlg, bWork, bWork2);

        wTopID = (WPARAM)(IDD_P01_BUTTON_03 + bWork2);
        break;

    case    IDD_P01_DLG_G:
        /*  ----- Get Current MDC Data (Bit 0) ----- */
        bWork  = (BYTE)(bMDCWork & P01_CHK_BIT);

        /*  ----- Get Current MDC Data (Bit 1 - 3) ----- */
        bWork2 = (BYTE)((bMDCWork >> P01_1BITSHIFT) & P01_CHK_3BITS);

        /* ----- Check RadioButton with Current MDC Data ----- */
/*** NCR2172 ***/
        /*  -------- change bit ----------------*/
        /*   |   |   |   |    |   |   |   |     */
        /*   | c | b | a | -> | a | b | c |     */
        /*   |   |   |   |    |   |   |   |     */
 
        bWork2 = (BYTE)(((bWork2 & P01_CHK_BIT) << P01_2BITSHIFT )+
                 (bWork2 & (P01_CHK_BIT << P01_1BITSHIFT)) + 
                 ((bWork2 >> P01_2BITSHIFT) & P01_CHK_BIT ));

        P01ChkTypeFG(hDlg, bWork, bWork2); 

        wTopID = (WPARAM)(IDD_P01_BUTTON_01 + bWork);
        break;

    default:
        /* ----- Set RadioButton Group ----- */
        wStartID = IDD_P01_BUTTON_01;
        wEndID   = IDD_P01_BUTTON_16;

        /* ----- Get Bit 0 - 3 Status ----- */
        bWork = (BYTE)(bMDCWork & P01_CHK_4BITS);

        /* ----- Set RadioButton ID to Cehcked ----- */
        wCheckID = (WORD)(wStartID + bWork);

        /* ----- Check RadioButton  ----- */
        CheckRadioButton(hDlg, wStartID, wEndID, wCheckID);

        wTopID = wCheckID;
        break;
    }
    return wTopID;
}

/*
* ===========================================================================
**  Synopsis:   VOID    P01ChkTypeCDE();
*
**  Input   :   HWND    hDlg        -   Handle of DailogBox
*               BYTE    bCheck1     -   1 Bit Status
*               BYTE    bCheck2     -   1 Bit Status
*               BYTE    bCheck3     -   2 Bits Status
*
**  Return  :   No return value
*
**  Description:
*       This function checks RadioButton (C, D & E Type) with Current MDC Data.
* ===========================================================================
*/
VOID P01ChkTypeCDE(HWND hDlg, BYTE bCheck1, BYTE bCheck2, BYTE bCheck3)
{
    WORD    wI, wCheckID, wStartID, wEndID;
    BYTE    bStatus;

    for (wI = 0; wI < P01_2GROUPS; wI++) {
        /* ----- Set RadioButton Group ----- */
        wStartID = (WORD)(IDD_P01_BUTTON_01 + (wI * P01_2GROUPS));
        wEndID   = (WORD)(IDD_P01_BUTTON_02 + (wI * P01_2GROUPS));

        /* ----- Set MDC Bit Status ----- */
        bStatus = (BYTE)((wI == 0) ? bCheck1 : bCheck2);

        /* ----- Set RadioButton ID to Cehcked ----- */
        wCheckID = (WORD)(wStartID + bStatus);

        /* ----- Check RadioButton ----- */
        CheckRadioButton(hDlg, wStartID, wEndID, wCheckID);
    }

    /* ----- Set RadioButton Group ----- */
    wStartID = IDD_P01_BUTTON_05;
    wEndID   = IDD_P01_BUTTON_08;

    /* ----- Set RadioButton ID to Cehcked ----- */
    wCheckID = (WORD)(wStartID + bCheck3);

    /* ----- Check RadioButton  ----- */
    CheckRadioButton(hDlg, wStartID, wEndID, wCheckID);
}

/*
* ===========================================================================
**  Synopsis:   VOID    P01ChkTypeFG();
*
**  Input   :   HWND    hDlg        -   Handle of DailogBox
*               BYTE    bCheck1     -   1 Bit Status
*               BYTE    bCheck2     -   3 Bits Status
*
**  Return  :   No return value
*
**  Description:
*       This function checks RadioButton (F & G Type) with Current MDC Data.
* ===========================================================================
*/
VOID    P01ChkTypeFG(HWND hDlg, BYTE bCheck1, BYTE bCheck2)
{
    WORD    wStartID, wEndID, wCheckID;

    /* ----- Set RadioButton Group ----- */
    wStartID = IDD_P01_BUTTON_01;
    wEndID   = IDD_P01_BUTTON_02;

    /* ----- Set RadioButton ID to Cehcked ----- */
    wCheckID = (WORD)(wStartID + bCheck1);

    /* ----- Check RadioButton  ----- */
    CheckRadioButton(hDlg, wStartID, wEndID, wCheckID);

    /* ----- Set RadioButton Group ----- */
    wStartID = IDD_P01_BUTTON_03;
    wEndID   = IDD_P01_BUTTON_10;

    /* ----- Set RadioButton ID to Cehcked ----- */
    wCheckID = (WORD)(wStartID + bCheck2);

    /* ----- Check RadioButton  ----- */
    CheckRadioButton(hDlg, wStartID, wEndID, wCheckID);
}

/*
* ===========================================================================
**  Synopsis:   VOID    P01SetBuff();
*
**  Input   :   HWND    hDlg        -   Handle of DailogBox
*               WORD    wStartID    -   Start RadioButton ID of Button Group
*               WORD    wEndID      -   End RadioButton ID of Button Group
*               WPARAM  wCurrent    -   Current Selected Button ID
*               int     nAddress    -   Current Selected Address (Not Zero-Based)
*               BYTE    bChkBit     -   Bit Type for Mask Data
*               BYTE    bShift      -   Bit Shift Count
*
**  Return  :   No return value
*
**  Description:
*    This function checks RadioButton in A, B & H Type Sub-DialogBox. And then
*   set them to Buffer.
* ===========================================================================
*/
VOID    P01SetBuff(HWND hDlg,
                   WORD wStartID,
                   WORD wEndID,
                   WPARAM wCurrent,
                   int nAddress,
                   BYTE bChkBit,
                   BYTE bShift)
{
    BYTE    bWork, bCheck;
	USHORT  usBuffAddr;

    /* ----- Check Current Selected Button ------ */
    CheckRadioButton(hDlg, wStartID, wEndID, wCurrent);

    /* ----- Get Current Selected Address of Data Buffer ----- */
    usBuffAddr = ((nAddress - 1) / P01_CHK_ADDR);

    /* ----- Get Bit Status of Selected Address ----- */
    if (nAddress % P01_CHK_ADDR) {  /* Address is Odd (Get from Lo-Bit) */
        bWork = P01_GET_LOBIT(abMDCData[usBuffAddr]);
    } else {                        /* Address is Even(Get from Hi-Bit) */
        bWork = P01_GET_HIBIT(abMDCData[usBuffAddr]);
    }

    /* ----- Reset Data Buffer ----- */
    bWork &= (BYTE)~(bChkBit << bShift);

    /* ----- Check Inputed Data whether ON or OFF ----- */
    bCheck = (BYTE)(wCurrent - wStartID);

/*** NCR2172 ***/
    if (wSubDlgID == IDD_P01_DLG_G){
        /*  -------- change bit ----------------*/
        /*   |   |   |   |    |   |   |   |     */
        /*   | a | b | c | -> | c | b | a |     */
        /*   |   |   |   |    |   |   |   |     */
        bCheck = (BYTE)((WORD)(((bCheck & P01_CHK_BIT) << P01_2BITSHIFT )+
         (bCheck & (P01_CHK_BIT << P01_1BITSHIFT)) + 
         ((bCheck >> P01_2BITSHIFT) & P01_CHK_BIT )));
    }

    /* ----- Set Inputed Data to Buffer ----- */
    bWork |= (BYTE)(bCheck << bShift);

    if (nAddress % P01_CHK_ADDR) {  /* Address is Odd (Get from Lo-Bit) */
        abMDCData[usBuffAddr] = P01_SET_LOBIT(abMDCData[usBuffAddr], bWork);
    } else {                        /* Address is Even(Get from Hi-Bit) */
        abMDCData[usBuffAddr] = P01_SET_HIBIT(abMDCData[usBuffAddr], bWork);
    }
}

/*
* ===========================================================================
**  Synopsis:   BOOL    P01ChkTare()
*
**  Input   :   HWND    hDlg        -   Handle of DailogBox
*
**  Return  :   TRUE    -   Tare Table Error Occurred
*               FALSE   -   No Error
*
**  Description:
*    This function 
* ===========================================================================
*/
BOOL    P01ChkTare(HWND hDlg)
{
    USHORT  ausTare[TARE_ADR_MAX], usI, usRet;
    BYTE    bWork;
	USHORT  usBuffAddr; 
    WCHAR   szTitle[PEP_CAPTION_LEN],szWork[128];
    BOOL    fCheck = FALSE;

    /* ----- Read Current Tare Data(AC 111) from Parameter File ----- */
    ParaAllRead(CLASS_PARATARE, (UCHAR *)ausTare, sizeof(ausTare), 0, &usRet);

    /* ----- Get Bit Status of Selected Address ----- */
    usBuffAddr = ((P01_TARE_ADDR - 1) / P01_CHK_ADDR);

    bWork = P01_GET_LOBIT(abMDCData[usBuffAddr]);

    /* ----- Get Bit Status ----- */
    if (bWork & P01_CHK_BIT) {  /* Bit-D is ON (Max Value = 9999/9995)  */

        if (! (bWork & (P01_CHK_BIT << P01_2BITSHIFT))) {  /* LSD is 0 / 5 */
            for (usI = 0; usI < TARE_ADR_MAX; usI++) {
                if (ausTare[usI] % P01_TARE_CHK) {
                    fCheck = TRUE;
                    break;
                }
            }
        }
    } else {                    /* Bit-D is OFF (Max Value = 999)       */
        for (usI = 0; usI < TARE_ADR_MAX; usI++) {
            if (ausTare[usI] > P01_TARE_MAX) {
                fCheck = TRUE;
                break;
            }
        }
    }

    /* ----- Check Whether Tare Error Occurred or Not -----*/
    if (fCheck) {
        LoadString(hResourceDll, IDS_PEP_CAPTION_P01, szTitle, PEP_STRING_LEN_MAC(szTitle));
        LoadString(hResourceDll,  IDS_P01_TARE_ERR, szWork, PEP_STRING_LEN_MAC(szWork));

        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg, szWork, szTitle, MB_ICONEXCLAMATION | MB_OK);
    }
    return fCheck;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P01SpinProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*                   WPARAM  wParam  -   Specifies a Scroll Bar Code
*
**  Return      :   No Return Value.
*
**  Description:
*       This function processes WM_VSCROLL message.
* ===========================================================================
*/
VOID    P01SpinProc(HWND hDlg, WPARAM wParam)
{
    PEPSPIN     PepSpin;

    /* ----- Initialize Spin Mode Flag ----- */
    PepSpin.lMin       = 0L;
    PepSpin.lMax       = (long)P01_CHK_4BITS;
    PepSpin.nStep      = P01_SPIN_STEP;
    PepSpin.nTurbo     = 0;
    PepSpin.nTurStep   = 0;
    PepSpin.fSpinStyle = PEP_SPIN_NOROLL;

    /* ----- spin button procedure ----- */
    if (PepSpinProc(hDlg, wParam, IDD_P01_EDIT, (LPPEPSPIN)&PepSpin)) {
        PostMessage(hDlg, WM_COMMAND, MAKELONG(IDD_P01_EDIT, EN_CHANGE), 0L);
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P01SpinProc475()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*                   WPARAM  wParam  -   Specifies a Scroll Bar Code
*
**  Return      :   No Return Value.
*
**  Description:
*       This function processes WM_VSCROLL message.
* ===========================================================================
*/
VOID	P01SpinProc475(HWND hDlg, WPARAM wParam)
{    
	PEPSPIN     PepSpin;

    /* ----- Initialize Spin Mode Flag ----- */
    PepSpin.lMin       = 0L;
    PepSpin.lMax       = (long)P01_CHK_255BITS;
    PepSpin.nStep      = P01_SPIN_STEP;
    PepSpin.nTurbo     = 0;
    PepSpin.nTurStep   = 0;
    PepSpin.fSpinStyle = PEP_SPIN_NOROLL;

    /* ----- spin button procedure ----- */
    if (PepSpinProc(hDlg, wParam, IDD_P01_EDIT, (LPPEPSPIN)&PepSpin)) {
        PostMessage(hDlg, WM_COMMAND, MAKELONG(IDD_P01_EDIT, EN_CHANGE), 0L);
    }
}
/* ===== End of File (P001.C) ===== */
