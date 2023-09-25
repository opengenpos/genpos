/*
* ---------------------------------------------------------------------------
* Title         :   Function Selection Code (Prog. 3)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P003.C
* Copyright (C) :   1995, NCR Corp. E&M OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /W4 /G2 /GEf /GA /Zp /f- /Os /Og /Oe /Gs
* ---------------------------------------------------------------------------
* Abstract :
  There are basically three kinds of key assignment data:
    - Function Selection Code (FSC) with extended FSC
    - PLU FSC with extended FSC info such as PLU number
    - department FSC with extended FSC info such as department number

  The FSC is a command.  Some commands require an option and for those FSCs
  which require an option, the option is put into the extended FSC.  Otherwise
  the extended FSC is 0.  For instance, the A key or A Sign-in key is just a
  command, the sign-in command.  The terminal then looks to the MDC bits to
  determine if other information such as secret code is required.  The Tender key,
  on the other hand, requires an extended FSC to indicate which tender bucket
  is to be affected by the Tender command.

  While the extended FSC is contained in the FSC table along with the FSC
  (the extended FSC is an additional parameter for the FSC such as the tender
  number) other FSC parameters are too large to fit in the space allocated for
  the extended FSC (e.g. PLU number) so the parameters are kept in a separate
  data structure.

  The FSC table is divided up into a series of menu pages which allows
  the physical keyboard to have multiple virtual keyboards.  By pressing
  the menu selection key, the physical keyboard is then mapped to a particular
  keyboard layout.  This makes it possible to provide a large virtual keyboard
  with a limited number of physical keys.

  The FSC table menu page is accessed by computing an offset from the beginning
  of the page to the location where the FSC and extended FSC for a particular key
  is supposed to be.  The dialog which displays the physical key layout of
  a keyboard has a set of dialog buttons which represent the keys on the keyboard.
  Each of the buttons on the dialog has a particular control ID.  This control ID,
  which begins with IDD_P03_KEY_00, is used to determine the offset of within the
  FSC table for a particular physical key on the keyboard.  See function P03DspStr ()
  below as well as function P03RepStr ().

  A keyboard may also have fixed keys.  These fixed key positions are specified in
  a table added to file P003TBL.C as are other tables.

  WARNING:  The button IDs for dialog buttons must be specified in a logical order.
            The order typically used is for each physical keyboard row to begin with
			IDD_P03_KEY_cr where r is the row number counting from 0 and c is the
			column number counting from 0.  This means that the fourth physical key
			on the third row of the keyboard will be IDD_P03_KEY_32.  r and c use
			hex numbers so A represents 10, B represents 11, etc.

            Make sure that you maintain backward compatibility!

            Some keyboards are designed with a key layout in which some keys are
			actually double keys meaning that two keys are combined with a single
			key cap.  An example can be seen on the NCR 64 key Wedge keyboard in
			which the double 0 key is two physical keys combined with a single key cap.
			Make sure that the dialog button IDs, which are used to calculate offsets
			within the FSC table, assigned to the buttons skip these keys in case they
			need to be added in later.

            The FSC table was designed with the NCR 7448 terminal with the Micromotion
			keyboard which has a physical key layout of 11 keys per row with 16 rows.
			If a keyboard has more keys in a row (for instance the NCR 78 key keyboard)
			then the following schemes can be used:
			  - logically rotate the keyboard swapping rows and columns
			     . NCR 78 key keyboard has 13 keys per row with 6 rows
				 . number keys as IDD_P03_KEY_rc rather than IDD_P03_KEY_cr
				 . fourth physical key on third row is IDD_P03_KEY_23 not IDD_P03_KEY_32

* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* Dec-22-93 : 00.00.01 : M.Yamamoto : Initial (Migrate from HP US Model)
* Apr-26-95 : 03.00.00 : H.Ishida   : Add shift page and key no 57-68
* Jul-25-95 : 03.00.00 : hkato      : R3.0 FVT Comment.
* Feb-29-96 : 03.00.05 : M.Suzuki   : Add R3.1 
* Oct-09-96 : 01.00.06 : M.Suzuki   : Correct Bug at compaction compaile(TAR44788)
* Jan-22-98 : 01.00.07 : M.Ozawa    : Correct the Clearance/Exchange of Keyed PLU
* Jan-23-98 : 01.00.08 : M.Ozawa    : Add P03ConvPluKey() for Correct PLU Key Data
* Sep-06-98 : 03.03.00 : A.Mitsui   : V3.3
* 4-21-03				R. Herrington	Mulitlingual PEP
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

#include    <windows.h>
#include    <windowsx.h>

#include    <stdlib.h>
#include    <string.h>
#include    <ctype.h>

//#include	"stringResource.h" //Multilingual PEP
#include    <ecr.h>
#include    <paraequ.h>
#include    <para.h>
#include    <rfl.h>

#include    "prog.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "file.h"
#include    "p003.h"
#include    "p003tbl.c"
#include	"layout.h"


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
static  BYTE    bKBType;            /* Current Keyboard Type            */
static  BYTE    bSavedKBType;		
static  HGLOBAL hMemData;           /* Handle of Global Memory          */
static  HGLOBAL hMemTbl;            /* Handle of Global Memory          */
static  HGLOBAL hPLUTbl;            /* Handle of Global Memory          */
static  HGLOBAL hConvPLUTbl;        /* Handle of Global Memory, 1/23/98 */

static  WORD    wCurBtnID;          /* Current Selected Button ID       */
static  UINT    unExtFscData;       /* Inputed Extended FSC Data        */
static  HWND    hIndKey,            /* Window Handle of Indicating Key  */
                hMoveKey;           /* Window Handle of Moving Key      */
static  BOOL    fIndicateFlag = FALSE;  /* Indicate Flag for WM_CTLCOLOR    */
static  WORD    wPageNo;            /* Current selected Menu Page No.   */
static  BOOL    fMenuPLU;           /* Conversion Save Flag, 1/23/98    */
/* Saratoga */
static  WCHAR   strPluNo[PLU_MAX_DIGIT + 1];
static  UINT    unPLULab;             /* Saratoga */
static  USHORT  usDeptNumber;         /* Saratoga */

#ifdef __P03__
static  LPP03DATA2   lpMenuPLU;      /* Address of Data Buffer (PLU Key) */
static  LPP03DATA2   lpConvMenuPLU;  /* Address of Data Buffer (PLU Key), 1/23/98 */
#else
static  USHORT FAR *lpConvMenuPLU;  /* Address of PLU Menu Buffer, 1/23/98 */
static  USHORT FAR *lpMenuPLU;      /* Address of PLU Menu Buffer */
#endif

static  HGLOBAL hDeptTbl;           /* Handle of Global Memory, Saratoga*/
static  LPP03DEPTDATA  lpMenuDept;  /* Address of Data Buffer (DeptKey), Saratoga */

/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/


/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  P003KeyDlgProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wMsg    -   Dispacthed Message
*                   WORD    wParam  -   16 bits Message Parameter
*                   LONG    lParam  -   32 bits Message Parameter
*
**  Return      :   TRUE            -   User Process is Executed
*                   FALSE           -   Default Process is Expected
*
**  Description :
*       This is a dialogbox procedure to display the FSC data.
* ===========================================================================
*/
BOOL WINAPI  P003KeyDlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static LPP03DATA    lpFscData;      /* Address of one page FSC Data Buffer */
    static LPP03TBLDATA lpFscTbl;       /* Address of all page FSC Data Buffer */
    static POINTS       PtLBtnDown;     /* X/Y Position of Left Button Down */
    static BOOL         fCapture;       /* Mouse Capture Flag               */
    static HCURSOR      hcurDrag;       /* Handle of Dragging Cursor        */
    static HBRUSH       hbrText,        /* Handle of Text Color Brush       */
                        hbrBk;          /* Handle of Background Color Brush */

    POINTS  PtLBtnUp,                   /* X/Y Position of Left Button Down */
            PtRBtnDown;                 /* X/Y Position of Right Button Down*/
    HDC     hdcKey;                     /* Handle of Device Context of Key  */
    HWND    hwndKey;                    /* Window handle of Selected Key    */
	int					nIdex;
	WORD				wID;        /* DialogBox ID of Current Keyboard */
	FCONFIG				*fConfig;

    switch (wMsg) {
    case WM_INITDIALOG:

        SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

		/* ----- Initialize Static Valuables ----- */

        fCapture = FALSE;
        hbrText  = 0;
        hbrBk    = 0;
        wPageNo  = 0;
        wCurBtnID = IDD_P03_NOFUNC;

        /* ----- Initialize Dialogbox ----- */

        if (P03InitDlg(hDlg,
                       (LPHCURSOR)&hcurDrag,
                       (LPP03DATA FAR *)&lpFscData,
                       (LPP03TBLDATA FAR *)&lpFscTbl)) {

            /* ----- Abort This Function, If Momory Allocation Error Occurred ----- */

            P03ExecAbort(hDlg,
                         P03_MEM_ERROR,
                         (LPHBRUSH)&hbrText,
                         (LPHBRUSH)&hbrBk,
                         (LPHCURSOR)&hcurDrag);

        }

        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {

			int j;
			for(j=IDD_P03_TOOL; j<=IDD_P03_PAGE_RNG; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}

			for(j=IDD_P03_KEY_00; j<=IDD_P03_DEPT_NUM; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			
			SendDlgItemMessage(hDlg, IDD_P03_SHIFT, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, -1, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_DESTROY:

        SendMessage(hwndProgMain, PM_PEPMENU, (WPARAM)PEP_MENU_ALL_ON, 0L);
        return FALSE;

    case WM_CTLCOLOR:               /* Draw DialogBox Controls  */ 

		// This is old 16 bit Windows message for owner drawn buttons.
		// It has been replaced by the WM_CTLCOLORBTN message in 32 bit
		// Windows.  However this application uses it with a SendMessage
		(P03DrawWnd(wParam,
                       lParam,
                       fCapture,
                       (LPHBRUSH)&hbrText,
                       (LPHBRUSH)&hbrBk));
		return TRUE;

    case WM_LBUTTONDOWN:            /* Left Button is Down      */

        /* ----- Compute X / Y Position of Left Button Down ----- */

        PtLBtnDown = MAKEPOINTS(lParam);

        /* ----- Determine Whether the Position is Valid or Not ----- */

        if ((hwndKey = P03ChkPos(hDlg, (PPOINTS)&PtLBtnDown)) != NULL) {
                                                /* The Position is Valid */

            /* ----- Set the Mouse Capture to Keyboard DialogBox ----- */

            SetCapture(hDlg);

            /* ----- Set the Capture Flag ----- */

            fCapture = TRUE;

            /* ----- Display 'Moving' Description to Selected Key ----- */

            P03DspMove(hDlg, hwndKey);

            /* ----- Change Cursor to Dragging ----- */

            P03ChgCursor(P03_CUR_HOLD, hcurDrag);

        }
        return TRUE;

    case WM_LBUTTONUP:              /* Left Button is Up        */
        if (fCapture == TRUE) {                 /* Capture Flag is ON   */

            /* ----- set capture flag OFF ----- */

            fCapture = FALSE;

            /* ----- Release the Mouse Capture to Keyboard DialogBox ----- */

            ReleaseCapture();

            /* ----- Change Cursor to Default (ARROW) ----- */

            P03ChgCursor(P03_CUR_RESET, hcurDrag);

            /* ----- Get Device Context Handle of 'Moving' Key ----- */

            hdcKey = GetDC(hMoveKey);

            /* ----- Send Redraw Message to DialogBox ----- */

            SendMessage(hDlg, WM_CTLCOLOR, MAKELONG(CTLCOLOR_STATIC, hMoveKey), (LPARAM)hdcKey);

            /* ----- Release Device Context Handle of 'Moving' Key ----- */

            ReleaseDC(hMoveKey, hdcKey);

            /* ----- Compute X / Y Position of Left Button Up ----- */

            PtLBtnUp = MAKEPOINTS(lParam);

            if ((hwndKey = P03ChkPos(hDlg, (PPOINTS)&PtLBtnUp)) != NULL) {
                                            /* The Position is Valid */

                if (P03Comp2Pos(hDlg,
                                (PPOINTS)&PtLBtnDown,
                                (PPOINTS)&PtLBtnUp)) {
                                    /* compare down position & up position */

                    /* ----- Replace Button Down Data with Button Up Data ----- */

                    P03RepStr(hDlg,
                              hwndKey,
                              (PPOINTS)&PtLBtnDown,
                              (LPP03DATA)lpFscData);

                } else {

                    if (P03MakeExtFsc(hDlg)) {     /* User Select OK Button */

                        /* ----- Display New String ----- */

                        P03DspStr(hDlg, hwndKey, (LPP03DATA)lpFscData);

                    } else {                    /* User Select Cancel Button */

                        /* ----- Display Previous String ----- */

                        P03DspPre(hDlg,
                                  (PPOINTS)&PtLBtnDown,
                                  (LPP03DATA)lpFscData);

                    }
                }
            } else {                        /* The Position is Invalid */

                /* ----- Display Previous String ----- */

                P03DspPre(hDlg, (PPOINTS)&PtLBtnDown, (LPP03DATA)lpFscData);

            }
            return TRUE;
        }
        return FALSE;

    case WM_RBUTTONDOWN:            /* Right Button is Down */

        /* ----- Compute X / Y Position of Right Button Down ----- */

        PtRBtnDown = MAKEPOINTS(lParam);

        /* ----- Determine Whether the Position is Valid or Not ----- */

        if ((hwndKey = P03ChkPos(hDlg, &PtRBtnDown)) != NULL) {

            /* ----- Display Indication DialogBox for FSC Detail ----- */

            P03ShowStat(hDlg, hwndKey, (LPP03DATA)lpFscData);
        }
        return TRUE;

    case WM_ACTIVATE:

        if ((LOWORD(wParam) == WA_ACTIVE) || (LOWORD(wParam) == WA_CLICKACTIVE)) {

            /* ----- Enable the ToolBox Button ----- */

            EnableWindow(GetDlgItem(hDlg, IDD_P03_TOOL), TRUE);

            return TRUE;
        }
        return FALSE;

    case PM_P03HIDETOOL:

        /* ----- Receive from ToolBoxDlg to Disable ToolBox Button ----- */

        EnableWindow(GetDlgItem(hDlg, IDD_P03_TOOL), FALSE);

        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDD_P03_TOOL:          /* ToolBox Button is Clicked   */

            if (HIWORD(wParam) == BN_CLICKED) {

                /* ----- Set ToolBox DialogBox to Active ----- */
				
                SetActiveWindow(*lphProgDlgChk2);

                return TRUE;
            } 
            return FALSE;

		case IDD_P03_CB_KBTYPE:		/*user selected a differnt keyboard layout*/
			//get index of selected keyboard
			nIdex = SendDlgItemMessage(hDlg, IDD_P03_CB_KBTYPE,
									   CB_GETCURSEL, 0, 0L) + FILE_KB_WEDGE_68;
			if(HIWORD(wParam) == CBN_SELCHANGE) {			/*user selected a differnt keyboard layout*/
				BYTE btempKBType;		//used to save the original keyboard type  - used to reset fConfig->chKBType 
										//if user selects cancel

				//map index to the appropriate ID of the keyboard selected
				if (nIdex == FILE_KB_CONVENTION) {         /* NCR 7448 with Conventional Keyboard  */

					wID = IDD_P03_CONV;

				} else if (nIdex == FILE_KB_MICRO) {  /* NCR 7448 with Micromotion Keyboard   */

					wID = IDD_P03_MICRO;
				} else if (nIdex == FILE_KB_WEDGE_68) {    /* NCR Wedge 64 key keyboard  */

					wID = IDD_P03_WEDGE_68;
				} else if (nIdex == FILE_KB_WEDGE_78) {    /* NCR Wedge 78 key keyboard  */

					wID = IDD_P03_WEDGE_78;
				}  else if (nIdex == FILE_KB_BIG_TICKET) { /* NCR Big Ticket Keyboard */
			
					wID = IDD_P03_TICKET ;
				}else if (nIdex== FILE_KB_TOUCH){
					
					LayoutExec(hProgPepInst, hwndProgMain,szCommTouchLayoutSaveName, xLangId);
					return TRUE;
				}   else {
					//This will be called if the terminal is set to PC 102 in the configuration dialog
					return TRUE; // breakpoint out.
				}
				
				//set the keyboard type for FCONFIG struc which is used to set the bKBType static variable when new dialog is created
				fConfig = FileConfigGet();
				fConfig->chKBType = nIdex;
				btempKBType = bSavedKBType;

				//destroy previous dialog and create a new one with the appropriate keyboard layout
				DestroyWindow(*lphProgDlgChk1);
				*lphProgDlgChk1 = DialogCreation(hResourceDll,//hProgInst,
                                   MAKEINTRESOURCEW(wID),
                                   hwndProgMain,
                                   P003KeyDlgProc);
				
				//set the cursor to the selected keyboard
				SendDlgItemMessage(*lphProgDlgChk1,
								   IDD_P03_CB_KBTYPE,
								   CB_SETCURSEL,
								   nIdex - FILE_KB_WEDGE_68,
								   0L);
				bSavedKBType = btempKBType;

				return TRUE;
			}
			return FALSE;

        case    IDD_P03_PAGE:

            if (HIWORD(wParam) == CBN_SELCHANGE) {

                /* ----- Set PLU No. of Selected Keyed PLU Key ----- */

                P03SetPageData(hDlg,
                               lpFscData,
                               lpFscTbl);

                return TRUE;
            }
            return FALSE;

        case IDOK:                  /* OK Button is Clicked        */
        case IDCANCEL:              /* Cancel Button is Clicked    */

            if (HIWORD(wParam) == BN_CLICKED) {

                if (LOWORD(wParam) == IDOK) {

                    /* ----- Get Page Data ----- */

                    P03GetData(lpFscData, (LPP03DATA)&lpFscTbl->FSC[wPageNo]);

                    /* ----- Save FSC Data in Buffer to File ----- */

                    P03SaveData(lpFscTbl);
					//Set the file configuration to the correct keyboard type
					fConfig = FileConfigGet();
					fConfig->chKBType = SendDlgItemMessage(hDlg, IDD_P03_CB_KBTYPE,
														  CB_GETCURSEL, 0, 0L) + FILE_KB_WEDGE_68;

                }
				if(LOWORD(wParam) == IDCANCEL){
					//set the keyboard type back to the original
					fConfig = FileConfigGet();
					fConfig->chKBType = bSavedKBType;
				}
                /* ----- Release Resources ----- */

                P03ExecAbort(hDlg,
                             P03_NORMAL_END,
                             (LPHBRUSH)&hbrText,
                             (LPHBRUSH)&hbrBk,
                             (LPHCURSOR)&hcurDrag);

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
**  Synopsis    :   VOID    P03SetPageData()
*
**  Input       :   HWND         hDlg        -   Window Handle of a DialogBox
*                   LPP03DATA    FAR *lpData -   Address of one page FSC Data Buffer
*                   LPP03TBLDATA FAR *lpTbl  -   Address of all page FSC Data Buffer
*
**  Return      :   WORD        wPage   -   Current Selected Menu Page No.
*
**  Description :
*       This function gets the number of current selected menu page. And
*   sets the PLU Number of a selected keyed PLU key to edittext.
*       It returns the number of current selected menu page.
* ===========================================================================
*/
VOID    P03SetPageData(HWND hDlg, LPP03DATA lpData, LPP03TBLDATA lpTbl)
{
    short       nI, nJ, nX;
    LPP03DATA   lpFscData;

    /* ----- Get Page Data ----- */

    P03GetData(lpData, (LPP03DATA)&lpTbl->FSC[wPageNo]);

    /* ----- Get Menu Page No. from ComboBox ----- */

    wPageNo = (WORD)SendDlgItemMessage(hDlg, IDD_P03_PAGE,
                                       CB_GETCURSEL,
                                       0, 0L);

    /* ----- Set Page Data -----*/

    lpFscData = (LPP03DATA)&lpTbl->FSC[wPageNo];

    for (nI = nJ = nX = 0; nI < P03_MICR_ITEM_NO; nI++, nX++) {

        if (nX >= P03_MICR_XITEM_NO) {  /* Check the End of 1 Line  */
            nX = 0;                         /* Reset X Position     */
        }

        if (! P03ChkFixKey((WORD)(IDD_P03_KEY_00 + nI))) {   /* Normal Key   */

            /* ----- Set Loaded FSC Data to Data Area ----- */

            (lpData+nI)->uchFsc = (lpFscData+nJ)->uchFsc;
            (lpData+nI)->uchExt = (lpFscData+nJ)->uchExt;
            nJ++;

            /* ----- Draw Key with FSC String ----- */

            P03SetStr(hDlg, nI, (lpData+nI));

        }

    }
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    P03InitDlg()
*
**  Input       :   HWND             hDlg    -   Window Handle of a Dialogbox
*                   LPHCURSOR        lphCur  -   Address of Handle of Cursor
*                   LPP03DATA    FAR *lpData -   Address of one page FSC Data Buffer
*                   LPP03TBLDATA FAR *lpTbl  -   Address of all page FSC Data Buffer
*
**  Return      :   TRUE    -   Memory Allocation is Failure.
*                   FALSE   -   Momory Allocation is Success.
*
**  Description :
*       This procedure initialize dialogbox.
* ===========================================================================
*/
BOOL    P03InitDlg(HWND hDlg, LPHCURSOR lphCur, LPP03DATA FAR *lpFscData, LPP03TBLDATA FAR *lpFscTbl)
{
    BOOL        fRet;
    WCHAR    szCap[P03_CAP_LEN];
//	WCHAR    szDesc[128];
	WORD     wI;
	UINT	nID;
	WCHAR    szMsg[128];
//	InitP03DlgStrings(hDlg); //RPH 4-22-03


	/* ----- Change Cursor (Arrow -> Hour-glass) ----- */

    *lphCur = SetCursor(LoadCursor(NULL, IDC_WAIT));
    ShowCursor(TRUE);

    /* ----- Set Popup Menu to Disable ----- */

    SendMessage(hwndProgMain, PM_PEPMENU, (WPARAM)PEP_MENU_ALL_OFF, 0L);

    /* ----- Get Keyboard Configuration from PEP:Main Window ----- */
	bKBType = (BYTE)FileConfigGet()->chKBType;
	if (bKBType < FILE_KB_WEDGE_68) {
		bKBType = FILE_KB_WEDGE_68;
	}
	bSavedKBType = bKBType;

	/*move from file->config to P03*/

   for (wI = FILE_KB_WEDGE_68, nID = IDS_FILE_KEY_WEDGE_68; wI < FILE_NUM_KB ; wI++, nID++) {
    LoadString(hResourceDll, nID, szMsg, PEP_STRING_LEN_MAC(szMsg));
    DlgItemSendTextMessage(hDlg,
                       IDD_P03_CB_KBTYPE,
                       CB_INSERTSTRING,
                       (WPARAM)-1,
                       (LPARAM)szMsg);
   }

	SendDlgItemMessage(hDlg,
                       IDD_P03_CB_KBTYPE,
                       CB_SETCURSEL,
                       bKBType - FILE_KB_WEDGE_68,
                       0L);

    /* ----- Allocate Memory for keyboard setup data from Global Heap ----- */

    hMemData = GlobalAlloc(GHND, (DWORD)(P03_MICR_ITEM_NO * sizeof(P03DATA)));
    *lpFscData  = (LPP03DATA)GlobalLock(hMemData);

    hMemTbl  = GlobalAlloc(GHND, (DWORD)(sizeof(P03TBLDATA)));
    *lpFscTbl   = (LPP03TBLDATA)GlobalLock(hMemTbl);

    hDeptTbl = GlobalAlloc(GHND, MAX_PAGE_NO * FSC_DEPT_MAX * sizeof(P03DEPT));  /* Saratoga */
    lpMenuDept = (LPP03DEPTDATA)GlobalLock(hDeptTbl);                            /* Saratoga */

    hPLUTbl = GlobalAlloc(GHND, MAX_PAGE_NO * FSC_PLU_MAX * sizeof(P03PLU));
    lpMenuPLU = (LPP03DATA2)GlobalLock(hPLUTbl);

    if (*lpFscData != NULL) {              /* Memory Allocation is Success */

        /* ----- Get FSC Data from Parameter File ----- */

        P03InitData(hDlg, *lpFscData, *lpFscTbl);
        
        fRet = FALSE;
        
    } else {                            /* Momory Allocation is Failure */

        fRet = TRUE;

    }

	{
		WORD    wI = 0;
	    WCHAR   szWork[32];

		/*  ----- Initialize Menu Page number ComboBox on dialog -----  */

		SendDlgItemMessage(hDlg, IDD_P03_PAGE, WM_SETFONT, (WPARAM)hResourceFont, 0);

		for (wI = 0; wI < P03_PAGE_MAX; wI++) {
			wsPepf(szWork, WIDE("%u"), (short)(wI + P03_DEF_PAGENO));
			DlgItemSendTextMessage (hDlg, IDD_P03_PAGE, CB_INSERTSTRING, (WPARAM)-1, (LPARAM)szWork);
		}

		SendDlgItemMessage(hDlg, IDD_P03_PAGE, CB_SETCURSEL, 0, 0L);
	}

    /* ----- Restore Cursor (Hour-glass -> Arrow) ----- */

    ShowCursor(FALSE);
    SetCursor(*lphCur);

    /* ----- Load Dragging Cursor from Resource ----- */

    LoadString(hResourceDll, IDS_P03_CUR_HOLD, szCap, PEP_STRING_LEN_MAC(szCap));
    *lphCur = LoadPepCursor(hResourceDll, szCap);

    return (fRet);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P03InitData()
*
**  Input       :   HWND         hDlg    -   Window Handle of KeyBoard DialogBox
*                   LPP03DATA    lpData  -   Address of one page FSC Data Buffer
*                   LPP03TBLDATA lpTbl   -   Address of all page FSC Data Buffer
*
**  Return      :   NOTHING
*
**  Description :
*       This procedure read FSC data from the parameter file into the standard
*		FSC table, lpFscTbl.  It then does some preliminary checks on the FSC
*		data to ensure it is correct while it 
* ===========================================================================
*/
VOID    P03InitData(HWND hDlg, LPP03DATA lpFscData, LPP03TBLDATA lpFscTbl)
{
    short       nI, nX;
    USHORT      usRet;
    LPP03DATA   lpFscDataTbl;
    BOOL        fChk = IDOK,
                fRet = FALSE,
                fSerChk = FALSE;
    WCHAR       szCap[P03_CAP_LEN];
    WCHAR       szErr[P03_DSC_LEN];

    /* ----- Read FSC Data from Parameter File ----- */

    memset(lpFscTbl, 0, sizeof(P03TBLDATA));
    ParaAllRead(CLASS_PARAFSC,
                (UCHAR *)lpFscTbl,
                sizeof(P03TBLDATA),
                0,
                &usRet);

    lpFscDataTbl = (LPP03DATA)lpFscTbl;

    /* ----- Read Dept Menu Data from Parameter File ----- */

    memset(lpMenuDept, 0, 
             (MAX_PAGE_NO * FSC_DEPT_MAX * sizeof(P03DEPT)));

    ParaAllRead(CLASS_PARADEPTNOMENU,
                (UCHAR *)lpMenuDept,
                 MAX_DEPTKEY_SIZE * sizeof(USHORT),
                0,
                &usRet);

    memset(lpMenuPLU, 0, 
             MAX_PAGE_NO * FSC_PLU_MICROMAX * sizeof(P03PLU));

    ParaAllRead(CLASS_PARAPLUNOMENU,
                (UCHAR *)lpMenuPLU,
                MAX_PAGE_NO * FSC_PLU_MICROMAX * sizeof(P03PLU),
                0,
                &usRet);
    fMenuPLU = 0;   /* add 1/23/98 */

    /* ----- Correct Keyed PLU Key Position, 1/23/98  ----- */

    P03ConvPluKey(hDlg, lpFscTbl);

    /* ----- Correct Keyed PLU Key Position  ----- */

    for (nI = nX = 0; nI < P03_MICR_ITEM_NO; nI++, nX++, lpFscData++) {

        if (nX >= P03_MICR_XITEM_NO) {  /* Check the End of 1 Line  */
            nX = 0;                         /* Reset X Position     */
        }

		//Check to see if this key is fixed or does not exist
		//Do not set the FSC to this key unless it is NOT a fixed key And it does exist
        if (! P03ChkFixKey((WORD)(IDD_P03_KEY_00 + nI))) {   /* Normal Key   */

            /* ----- Old Charge Tips No. -> New Charge Tips No. , V3.3 (0 -> 1) ----- */

            if ((lpFscDataTbl->uchFsc == P03_CHARTIP) && (lpFscDataTbl->uchExt == 0)) {

                lpFscDataTbl->uchExt = 1;
            }

            /* ----- Server key is not used. Display Warning Message flag on. , V3.3 ----- */
            
            if ((lpFscDataTbl->uchFsc) == P03_SERVNO) {

                /* ----- ServerCheck flag on ----- */

                fSerChk = TRUE;

                /* ----- Old Server Key -> No.Function Key ----- */

                lpFscDataTbl->uchFsc = P03_NOFUNC;

            }

            /* ----- Set Loaded FSC Data to Data Area ----- */

            lpFscData->uchFsc = lpFscDataTbl->uchFsc;
            lpFscData->uchExt = lpFscDataTbl->uchExt;
            lpFscDataTbl++;

            /* ----- Draw Key with FSC String ----- */

            P03SetStr(hDlg, nI, lpFscData);

        }

    }

    /* ----- Warning message to change server key, V3.3 ----- */

    if (fSerChk) {
        
        MessageBeep(MB_ICONEXCLAMATION);

        /* ----- Load Caution Message from Resource ----- */

        LoadString(hResourceDll, IDS_PEP_CAPTION_P03, szCap, PEP_STRING_LEN_MAC(szCap));
        LoadString(hResourceDll, IDS_P03_CONVSERKEY, szErr, PEP_STRING_LEN_MAC(szErr));

        /* ----- Display Caution Message ----- */

        MessageBoxPopUp(hDlg, szErr, szCap,
                   MB_OK | MB_ICONEXCLAMATION);

    }

}

/*
* ===========================================================================
**  Synopsis    :   BOOL    P03ChkFixKey()
*
**  Input       :   WORD    wKeyID  -   Current Selected Key ID
*
**  Return      :   TRUE    -   Current Key is Fixed Key
*                   FALSE   -   Current Key is Normal Key
*
**  Description :
*       This function determines whether the current selected key is fixed key
*   or not fixed key with current key ID.

	This function determines if this key exists on the keyboard or is a fixed key
	The micromotion keyboard contains all keys, the fixed keys for the micromotion
	keyboard are listed in awMicrFixedID and the number of keys in that list is
	P03_MICR_FIXED_NO
	The conventional keyboard does not have the last column of keys that the micromotion has
	So those keys are returned as "FIXED" meaning do not assign those a value
	The conventional keyboard also has a list of fixed keys in awConvFixedID
	The number of keys in that list is P03_CONV_FIXED_NO
	The Wedge Keyboard eliminates the same column as the Conventional keyboard, but
	also other keys are not available so those keys have been placed in awWedge68FixedID
	along with the "Fixed" keys for that keyboard
	The number of keys in that list is P03_WEDGE68_FIXED_NO

  fRet is set to true and the function returns when a match is found for the key

	
* ===========================================================================
*/
BOOL    P03ChkFixKey(WORD wKeyID)
{
    BOOL    fRet = FALSE;
    LPWORD  lpwFixedKeys;
    WORD    wFixedKeyNo,
            wI;

    /*
		----- Determine the Keyboard Type  -----
		See file P003TBL.C for the tables used and their initialization.
	*/

    if (bKBType == FILE_KB_MICRO) {             /* NCR 7448 terminal with Micromotion Keyboard     */

        lpwFixedKeys = (LPWORD)awMicrFixedID;
        wFixedKeyNo  = P03_MICR_FIXED_NO;

    } else if (bKBType == FILE_KB_CONVENTION) { /* NCR 7448 terminal with Conventional Keyboard    */

        lpwFixedKeys = (LPWORD)awConvFixedID;
        wFixedKeyNo  = P03_CONV_FIXED_NO;

        if (! ((wKeyID - IDD_P03_KEY_00 + 1) % P03_MICR_XITEM_NO)) {
            fRet = TRUE;
			return (fRet);
        }

    } else if (bKBType == FILE_KB_WEDGE_68) {    /* NCR terminal with Wedge 64 Key keyboard */

        lpwFixedKeys = (LPWORD)awWedge68FixedID;
        wFixedKeyNo  = P03_WEDGE68_FIXED_NO;
        
		if (! ((wKeyID - IDD_P03_KEY_00 + 1) % P03_MICR_XITEM_NO)) {
            fRet = TRUE;
			return (fRet);
       }

    } else if (bKBType == FILE_KB_WEDGE_78) {    /* NCR terminal with Wedge 78 Key keyboard */

        lpwFixedKeys = (LPWORD)awWedge78FixedID;
        wFixedKeyNo  = P03_WEDGE78_FIXED_NO;
        
    }else if (bKBType == FILE_KB_BIG_TICKET) {    /* NCR terminal with Wedge 78 Key keyboard */

        lpwFixedKeys = (LPWORD)awTicketFixedID;
        wFixedKeyNo  = P03_TICKET_FIXED_NO;
        
    }else {
        __debugbreak();  // prep for ARM build _asm int 3;
	}   // breakpoint out.

    /* ----- Check Current Key is Fixed Key or Not ----- */

    for (wI = 0; wI < wFixedKeyNo; wI++) {
        if (*(lpwFixedKeys + wI) == wKeyID) {       /* Key is Fixed Key     */
            fRet = TRUE;
            break;
        }
    }

    return (fRet);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P03SaveData()
*
**  Input       :   LPP03TBLDATA   lpTbl  -   Address of all page FSC data buffer
*
**  Return      :   None
*
**  Description :
*       This procedure write all page table data to parameter file.
* ===========================================================================
*/
VOID    P03SaveData(LPP03TBLDATA lpTbl)
{
    USHORT      usRet;

    /* ----- Write Inputed FSC Data to Parameter File ----- */

    ParaAllWrite(CLASS_PARAFSC,
                 (UCHAR *)lpTbl,
                 sizeof(P03TBLDATA),
                 0,
                 &usRet);

    /* ----- Write Inputed DEPT Menu Data to Parameter File, Saratoga ----- */


    ParaAllWrite(CLASS_PARADEPTNOMENU,
                (UCHAR *)lpMenuDept,
                MAX_DEPTKEY_SIZE * sizeof(USHORT),
                0,
                &usRet);

    /* ----- Write Inputed PLU Menu Data to Parameter File ----- */

    if (fMenuPLU) {
        ParaAllWrite(CLASS_PARAPLUNOMENU,
                     (UCHAR *)lpMenuPLU,
                     (MAX_PAGE_NO * FSC_PLU_MICROMAX * sizeof(P03PLU)),
                     0,
                     &usRet);
        PepSetModFlag(hwndProgMain, PEP_MF_ACT, 0);
    }

    /* ----- Set Modify Status ----- */

    PepSetModFlag(hwndProgMain, PEP_MF_PROG, 0);

}

/*
* ===========================================================================
**  Synopsis    :   VOID    P03GetData()
*
**  Input       :   LPP03DATA   lpData  -   Address of one page FSC data buffer
**                  LPP03DATA   lpTbl   -   Address of all page FSC data buffer
*
**  Return      :   None
*
**  Description :
*       This procedure set inputed FSC data to all page table.
* ===========================================================================
*/
VOID    P03GetData(LPP03DATA lpData, LPP03DATA lpTbl)
{
    short       nI, nJ;

    /* ----- Convert Data to Parameter Data ----- */

    for (nJ = nI = 0; nI < P03_MICR_ITEM_NO; nI++) {
        if (! P03ChkFixKey((WORD)(IDD_P03_KEY_00 + nI))) {   /* Not Fixed Key */
            (lpTbl+nJ)->uchFsc = (lpData + nI)->uchFsc;
            (lpTbl+nJ)->uchExt = (lpData + nI)->uchExt;
            nJ++;
        }
    }

}

/*
* ===========================================================================
**  Synopsis    :   VOID    P03DspMove()
*
**  Input       :   HWND    hDlg    -   Window Handle of Keyboard DialogBox
*                   HWND    hwndKey -   Window Handle of Selected Key
*
**  Return      :   None
*
**  Description :
*       This function changes string ID ('Moving') of the specified position.
*           (This position is the point of left button down.)
* ===========================================================================
*/
VOID    P03DspMove(HWND hDlg, HWND hwndKey)
{
    WCHAR       szDesc[P03_DSP_LEN];
    HDC         hdcKey;

    /* ----- Store Window Handle of Key to Static Area ----- */

    hMoveKey = hwndKey;

    /* ----- Get Device Context of Moving Key for Repaint ----- */

    hdcKey = GetDC(hwndKey);

    /* ----- Send Repaint Message to Main DialogBox ----- */

    SendMessage(hDlg, WM_CTLCOLOR, MAKELONG(CTLCOLOR_STATIC, hwndKey), (LPARAM)hdcKey);

    /* ----- Release Device Context of Moving Key ----- */

    ReleaseDC(hwndKey, hdcKey);

    /* ----- Load Dragging String from Resource ----- */

    LoadString(hResourceDll, IDS_P03_DSP_MOVING, szDesc, PEP_STRING_LEN_MAC(szDesc));

    /* ----- Display String to Specified Position ----- */

    WindowRedrawText(hwndKey, szDesc);

}

/*
* ===========================================================================
**  Synopsis    :   VOID    P03RepStr()
*
**  Input       :   HWND      hDlg      -   Window Handle of Keyboard DialogBox
*                   HWND      hwndUp    -   Window Handle of Button Up Key
*                   PPOINTS   lpptPos   -   Address of Left Button Down Pos.
*                   LPP03DATA lpData    -   Address of FSC Data Buffer 
*
**  Return      :   None
*
**  Description :
*       This procedure exchanges 2 position's data, one is the old data
*       which is the mouse down point and another one is the new data
*       whitch is the mouse up point and changes the moving bitmap.
* ===========================================================================
*/
VOID    P03RepStr(HWND hDlg, HWND hwndUp, PPOINTS lpptPos, LPP03DATA lpFscData)
{
    WORD    wDownDataOff,
            wUpDataOff;
    P03DATA DataDown,
            DataUp;
    UCHAR   uchFsc, uchExt;
    HWND    hwndDown;
    UCHAR   uchDownExt, uchUpExt;
    LPWORD  lpwKeyedPLUKeys;

    WCHAR   szPluNo[PLU_MAX_DIGIT + 1];
	POINT  lptPoint;

	lptPoint.x = lpptPos->x;
	lptPoint.y = lpptPos->y;

    /* ----- Get Window Handle of Key (Left Button Down Position) ----- */

    hwndDown = ChildWindowFromPoint(hDlg, lptPoint);

    /*
		----- Get Data Offset and Key ID Offset with Mouse Position -----
		The correct functioning of this code depends on the control IDs used
		in the dialog template for the keyboard.
	*/

    wDownDataOff = (WORD)(GetDlgCtrlID(hwndDown) - IDD_P03_KEY_00);
    wUpDataOff   = (WORD)(GetDlgCtrlID(hwndUp)   - IDD_P03_KEY_00);

    /* ----- Set Down/Up Position's FSC Data to Temporary Area ----- */

    DataDown = *(lpFscData + wDownDataOff);
    DataUp   = *(lpFscData + wUpDataOff);

    /* ----- Exchange Down Position's FSC Data with Up Position's One ----- */

    uchFsc          =   DataDown.uchFsc;
    DataDown.uchFsc =   DataUp.uchFsc;
    DataUp.uchFsc   =   uchFsc;

    /* ----- Exchange Down Position's Extended FSC with Up Position's One ----- */

    uchExt          =   DataDown.uchExt;
    DataDown.uchExt =   DataUp.uchExt;
    DataUp.uchExt   =   uchExt;

    /* ----- Exchange Keyed PLU, 1/22/98 ---- */

    if ((DataDown.uchFsc == P03_KEYEDPLU) ||
        (DataUp.uchFsc == P03_KEYEDPLU)) {

		if (bKBType == FILE_KB_MICRO) {               /* NCR 7448 with Micromotion Keyboard   */
			lpwKeyedPLUKeys = (LPWORD)awMicrKeyedPLU;
		} else  if (bKBType == FILE_KB_CONVENTION) {  /* NCR 7448 with Conventional Keyboard  */
			lpwKeyedPLUKeys = (LPWORD)awConvKeyedPLU;
		} else  if (bKBType == FILE_KB_WEDGE_68) {    /* NCR 5932 Wedge 64 key keyboard  */
			lpwKeyedPLUKeys = (LPWORD)awWedge68KeyedPLU;
		} else  if (bKBType == FILE_KB_WEDGE_78) {    /* NCR 5932 Wedge 78 key keyboard  */
			lpwKeyedPLUKeys = (LPWORD)awWedge78KeyedPLU;
		} else  if (bKBType == FILE_KB_BIG_TICKET) {    /* NCR 5932 Wedge 78 key keyboard  */
			lpwKeyedPLUKeys = (LPWORD)awBigTicket;
		}
		else {
            __debugbreak();  // prep for ARM build _asm int 3;
		}   // breakpoint out.
        /*

			----- set Extended FSC -----
			We lookup in the Keyed PLU table to determine the offset into
			the PLU Key table that is to be used with the selected keys
			on the keyboard (as picked from the dialog by the user).
			The values in the table are one based and 0 is used to indicate
			the key is not allowed to be set.  We then need to decrement
			the table value to change it to a zero based index into the
			PLU key table.
		*/
        uchDownExt = (UCHAR)(*(lpwKeyedPLUKeys + wDownDataOff));
		if (uchDownExt == 0)
			return;
        uchDownExt--;
        uchUpExt   = (UCHAR)(*(lpwKeyedPLUKeys + wUpDataOff));
		if (uchUpExt == 0)
			return;
        uchUpExt--;

        /* ----- exchange Menu page for Keyed PLU ----- */

        memcpy(szPluNo,
                 (lpMenuPLU->PluKey[wPageNo][uchDownExt].szPluNo),
                 sizeof(szPluNo));

        memcpy((lpMenuPLU->PluKey[wPageNo][uchDownExt].szPluNo),
                 (lpMenuPLU->PluKey[wPageNo][uchUpExt].szPluNo),
                 (PLU_MAX_DIGIT + 1) * sizeof(WCHAR));

        memcpy((lpMenuPLU->PluKey[wPageNo][uchUpExt].szPluNo),
                 szPluNo,
                 sizeof(szPluNo));

        if (DataDown.uchFsc == P03_KEYEDPLU) {
            /* --- correct Keyed PLU Ext. FSC --- */
            DataDown.uchExt = uchDownExt;
            DataDown.uchExt++;

        } else {
            /* --- clear previous PLU No. --- */

            memset((lpMenuPLU->PluKey[wPageNo][uchDownExt].szPluNo),
                     0x00,
                     (PLU_MAX_DIGIT + 1) * sizeof(WCHAR));
        }
        if (DataUp.uchFsc == P03_KEYEDPLU) {
            /* --- correct Keyed PLU Ext. FSC --- */
            DataUp.uchExt = uchUpExt;
            DataUp.uchExt++;

        } else {
            /* --- clear previous PLU No. --- */

            memset((lpMenuPLU->PluKey[wPageNo][uchUpExt].szPluNo),
                     0x00,
                     (PLU_MAX_DIGIT + 1) * sizeof(WCHAR));
        }

        fMenuPLU = 1;
    }

    /* ----- Store Up/Down Position's Data to FSC Data Area ----- */

    *(lpFscData + wDownDataOff) = DataDown;
    *(lpFscData + wUpDataOff)   = DataUp;

    /* ----- Display String ----- */

    P03SetStr(hDlg, (short)wDownDataOff, (LPP03DATA)&DataDown);
    P03SetStr(hDlg, (short)wUpDataOff,   (LPP03DATA)&DataUp);

}

/*
* ===========================================================================
**  Synopsis    :   VOID    P03DspStr()
*
**  Input       :   HWND        hDlg    -   Window Handle of Keyboard DialogBox
*                   HWND        hwndKey -   Window Handle of Current Key
*                   LPP03DATA   lpData  -   Address of FSC Data buffer
*
**  Return      :   None
*
**  Description :
*       This procedure saves the new FSC data into Data buffer.
* ===========================================================================
*/
VOID    P03DspStr(HWND hDlg, HWND hwndKey, LPP03DATA lpData)
{
    UCHAR       uchFsc;
    UCHAR       uchOffset;      /* Add 10-9-96 */
    WORD        wDataOffset;
    LPP03DATA   lpTmp;
    LPWORD      lpwKeyedPLUKeys;

    BOOL        bWork = FALSE;
    USHORT      usI;

    /*
		Get the control ID (specified in the dialog template in PEP.RC) for
		the button the user has picked.
		
		Then calculate the offset in the temporary FSC table we are using in
		the P3 dialog for this particular key on the terminal keyboard.

		Derive the FSC code based on the current selected tool in the displayed
		tool box so that we can put the FSC into the FSC table position for the
		selected terminal key.
	 */

    wDataOffset = GetDlgCtrlID(hwndKey);
    wDataOffset = wDataOffset - IDD_P03_KEY_00;

    lpTmp = lpData + wDataOffset;

    uchFsc = P03MAJORFSCCODE(wCurBtnID);

    lpTmp->uchFsc = uchFsc;

    /*
		----- Check Keyed PLU -----
		These tables are defined in file P003TBL.C and contain the offset
		into the PLU structs when assigning PLUs to a key.  The PLU structs
		are separate data structs since the FSC table is designed to hold only
		UCHAR data representing first the FSC and secondly the extended FSC.
		Since the PLU is 14 digits, it needs its own separate data storage area.
	 */

    if (bKBType == FILE_KB_MICRO) {               /* NCR 7448 with Micromotion Keyboard   */
        lpwKeyedPLUKeys = (LPWORD)awMicrKeyedPLU;
    } else  if (bKBType == FILE_KB_CONVENTION) {  /* NCR 7448 with Conventional Keyboard  */
        lpwKeyedPLUKeys = (LPWORD)awConvKeyedPLU;
	} else  if (bKBType == FILE_KB_WEDGE_68) {    /* NCR 5932 Wedge 64 key keyboard  */
        lpwKeyedPLUKeys = (LPWORD)awWedge68KeyedPLU;
	} else  if (bKBType == FILE_KB_WEDGE_78) {    /* NCR 5932 Wedge 78 key keyboard  */
        lpwKeyedPLUKeys = (LPWORD)awWedge78KeyedPLU;
    } else if (bKBType == FILE_KB_BIG_TICKET) {
		lpwKeyedPLUKeys = (LPWORD)awBigTicket;
	}
	else {
        __debugbreak();  // prep for ARM build _asm int 3;
	}   // breakpoint out.

    if (wCurBtnID == IDD_P03_KEYEDPLU) {
		/*
			If the tool selected is the keyed PLU tool then we will need
			to copy the entered PLU into the Menu table and set the type of
			PLU as the user specified in the dialog box.

			In this case, the Extended FSC code is the offset into the
			Menu table where we are going to store the PLU information.
		 */

        /* ----- set Extended FSC ----- */
        lpTmp->uchExt = (UCHAR)(*(lpwKeyedPLUKeys + wDataOffset));

        uchOffset = (UCHAR)(lpTmp->uchExt - 1);

        /* ----- Set Plu No ----- */

        memcpy(lpMenuPLU->PluKey[wPageNo][uchOffset].szPluNo,
                 strPluNo,
                 sizeof(strPluNo));

        /* ----- Set UPC-E modifier flag ----- */
        if(unPLULab == LA_EMOD)
           lpMenuPLU->PluKey[wPageNo][uchOffset].uchModStat = LA_EMOD;
        else 
           lpMenuPLU->PluKey[wPageNo][uchOffset].uchModStat = LA_NORMAL;

        fMenuPLU = 1;   /* 1/23/98 */

    } else {

        if (wCurBtnID == IDD_P03_KEYEDDPT) {

            /* ----- set Extended FSC ----- */
            lpTmp->uchExt = (UCHAR)(*(lpwKeyedPLUKeys + wDataOffset));

            /* ----- set Menu page for Keyed PLU ----- */

            uchOffset = (UCHAR)(unExtFscData - 1);

            /* ----- Set Deot No ----- */

            lpMenuDept->DeptKey[wPageNo][uchOffset].usDeptNumber = usDeptNumber;

        }

        /* ----- Determine Whether Major FSC Code Has Extended FSC or Not ----- */

        lpTmp->uchExt = (UCHAR)((awExtRngTbl[uchFsc] != 0) ? unExtFscData : 0);

        /* ----- Clear Previous Keyed PLU, if other FSC is entered, 1/22/98 ---- */
		// When adding keyboards, make sure that the table containing the PLU
		// offset assignments for each key have been updated to provide the proper
		// offset.  See file P003TBL.C for those tables.

        uchOffset = (UCHAR)(*(lpwKeyedPLUKeys + wDataOffset));
        uchOffset--;

		bWork = FALSE;
        for (usI = 0; usI < PLU_MAX_DIGIT; usI++) {
             if((lpMenuPLU->PluKey[wPageNo][uchOffset].szPluNo[usI] == 0x00) ||
                (lpMenuPLU->PluKey[wPageNo][uchOffset].szPluNo[usI] == L' ')) {
             } else {
                bWork = TRUE;
             }
        }
        
        if(bWork == FALSE) {
           for(usI = 0; usI < PLU_MAX_DIGIT; usI++) {
             lpMenuPLU->PluKey[wPageNo][uchOffset].szPluNo[usI] = L'0';
           }

        }

        lpMenuPLU->PluKey[wPageNo][uchOffset].szPluNo[PLU_MAX_DIGIT] = 0x00;
        fMenuPLU = 1;

    }

    /* ----- Display String to Keyboard ----- */

    P03SetStr(hDlg, (short)wDataOffset, lpTmp);

    /* ----- Set Active ToolBox DialogBox ----- */

    SetActiveWindow(*lphProgDlgChk2);
    
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P03DspPre()
*
**  Input       :   HWND        hDlg     -  Window Handle of Keyboard DialogBox 
*                   PPOINTS     lpptPos  -  Address of L-Button Down Position
*                   LPP03DATA   lpData   -  Address of FSC Data Buffer
*
**  Return      :   None
*
**  Description :
*       This procedure displays previous string of left button down position.
* ===========================================================================
*/
VOID    P03DspPre(HWND hDlg, PPOINTS lpptPos, LPP03DATA lpData)
{
    WORD    wDataOffset;
    HWND    hwndDown;
	POINT  lptPoint;

	lptPoint.x = lpptPos->x;
	lptPoint.y = lpptPos->y;

    /* ----- Get Data Offset and ID Offset with Mouse Position ----- */

    hwndDown = ChildWindowFromPoint(hDlg, lptPoint);
    wDataOffset = (WORD)(GetDlgCtrlID(hwndDown) - IDD_P03_KEY_00);

    /* ----- Display Previous String ----- */

    P03SetStr(hDlg, (short)wDataOffset, (LPP03DATA)(lpData + wDataOffset));

}

/*
* ===========================================================================
**  Synopsis    :   VOID    P03SetStr()
*
**  Input       :   HWND        hDlg    -   Window Handle of Keyboard DialogBox
*                   short       nOff    -   Key ID Offset from IDD_P03_KEY_00
*                   LPP03DATA   lpData  -   Address of FSC Data Buffer
*
**  Return      :   None
*
**  Description :
*       This procedure displays FSC string to specified key position in
*   keyboard dialogbox.
* ===========================================================================
*/
VOID    P03SetStr(HWND hDlg, short nOff, LPP03DATA lpData)
{
    WCHAR    szWork[P03_DSP_LEN * 2],
            szDesc[P03_PLU_LEN];
    UCHAR       uchOffset;      /* Add 10-9-96 */

    WCHAR    szPluNo[PLU_MAX_DIGIT + 1];

    switch (lpData->uchFsc + P03_IDD_OFS) {
    case IDD_P03_KEYEDPLU:
    case IDD_P03_KEYEDDPT:
    case IDD_P03_PRTMOD:
    case IDD_P03_ADJ:
    case IDD_P03_TENDER:
    case IDD_P03_FC:
    case IDD_P03_TTL:
    case IDD_P03_ADDCHK:
    case IDD_P03_IDISC:
    case IDD_P03_RDISC:
    case IDD_P03_TAXMOD:
    case IDD_P03_CHARTIP:
    case IDD_P03_PRECASH:
    case IDD_P03_PRTDEM:
    case IDD_P03_DRCTSHFT:
    case IDD_P03_KEYEDCPN:
    case IDD_P03_KEYEDSTR:
    case IDD_P03_SEAT:
/*** NCR2172 ***/
    case IDD_P03_ASK:
    case IDD_P03_ADJPLS:
    case IDD_P03_ALPHANUM:
	case IDD_P03_GIFT_CARD:
	case IDD_P03_ORDER_DEC:
	case IDD_P03_DOC_LAUNCH:    // FSC_DOC_LAUNCH

        /* ----- Load String from Resource file ----- */

        LoadString(hResourceDll, IDS_P03_DSP_NOFUNC + lpData->uchFsc, szWork, PEP_STRING_LEN_MAC(szWork));

        /* ----- Check Keyed PLU ----- */
        if (lpData->uchFsc == P03_KEYEDPLU) {

            uchOffset = (UCHAR)(lpData->uchExt - 1);

            memcpy(szPluNo, (lpMenuPLU->PluKey[wPageNo][uchOffset].szPluNo), sizeof(szPluNo));
            wsPepf(szDesc, szWork, szPluNo);

        } else if (lpData->uchFsc == P03_KEYEDDPT) {               /* ADD Saratoga */

            uchOffset = (UCHAR)(lpData->uchExt - 1);               /* ADD Saratoga */
                                                                   /* ADD Saratoga */
            wsPepf(szDesc, szWork, 
                     (USHORT)lpMenuDept->DeptKey[wPageNo][uchOffset].usDeptNumber);

        } else {
            /* ----- Make String with Extended FSC Data ----- */

            wsPepf(szDesc, szWork, (short)lpData->uchExt);
        }

        break;

    default:

        /* ----- Load String from Resouce ----- */
		//When using No Function the Buffer contains unwanted data that is placed
		//in the keyboard location a memset is done to clear that buffer before
		//loading the string for the FSC NO Function has no String to be displayed
		//because it is used to remove a funtion from a location
		memset(szDesc, 0, sizeof(szDesc));  //RPH
        LoadString(hResourceDll,IDS_P03_DSP_NOFUNC + lpData->uchFsc,szDesc,PEP_STRING_LEN_MAC(szDesc));

    }

    /* ----- Set Loaded String to Specified Position of Keyboard ----- */

    DlgItemRedrawText(hDlg, IDD_P03_KEY_00 + nOff, szDesc);

}

/*
* ===========================================================================
**  Synopsis    :   VOID    P03ShowStat()
*
**  Input       :   HWND        hDlg    -   Window Handle of Keyborad DialogBox
*                   HWND        hwndKey -   Window Handle of Selected Key
*                   LPP03DATA   lpData  -   Address of FSC Data Buffer
*
**  Return      :   None
*
**  Description :
*       This procedure shows the detail of FSC data with Messagebox when
*   user clickes the right mouse button.
* ===========================================================================
*/
VOID    P03ShowStat(HWND hDlg, HWND hwndKey, LPP03DATA lpData)
{
    P03DATA Tmp;
    HDC     hdcKey;
    WORD    wDataOffset;
    short   nLen;
    WCHAR   szIndKey[P03_DSP_LEN],
            szCaption[P03_CAP_LEN],
            szInfo[P03_DSC_LEN],
            szFsc[P03_DSC_LEN],
            szExt[P03_DSC_LEN];
    UCHAR       uchOffset;         /* Add 10-9-96 */

    WCHAR    szMsg[P03_DSC_LEN],    /* Saratoga */
            szType[P03_DSC_LEN];   /* Saratoga */

    /* ----- Get Data Offset and ID Offset with Mouse Position ----- */

    wDataOffset = (WORD)(GetDlgCtrlID(hwndKey) - IDD_P03_KEY_00);

    /* ----- Store FSC Data to Temporary Buffer ----- */

    Tmp = *(lpData + wDataOffset);

    /* ----- Get Handle of Indicating Key for Repaint ----- */

    hIndKey = hwndKey;

    /* ----- Set Indicating Flag to ON ----- */

    fIndicateFlag = TRUE;

    /* ----- Get Device Context of Indicating Key for Repaint ----- */

    hdcKey = GetDC(hIndKey);

    /* ----- Send Repaint Message to Main DialogBox ----- */

    SendMessage(hDlg, WM_CTLCOLOR, MAKELONG(CTLCOLOR_STATIC, hIndKey), (LPARAM)hdcKey);

    /* ----- Release Device Context ----- */

    ReleaseDC(hIndKey, hdcKey);

    /* ----- Load Indicationg String from Resource ----- */

    LoadString(hResourceDll, IDS_P03_DSP_INDIC, szIndKey, PEP_STRING_LEN_MAC(szIndKey));

    /* ----- Display String ('Indicating') to Specified Position ----- */

    DlgItemRedrawText(hDlg, IDD_P03_KEY_00 + wDataOffset, szIndKey);

    /* ----- Load FSC String from Resource ----- */

    LoadString(hResourceDll, (IDS_P03_INF_NOFUNC + Tmp.uchFsc), szFsc, PEP_STRING_LEN_MAC(szFsc));

    /* ----- Make Information String with Loaded FSC String ----- */

    LoadString(hResourceDll, IDS_P03_INFO, szInfo, PEP_STRING_LEN_MAC(szInfo));
    lstrcatW(szInfo, szFsc);

    if (awExtRngTbl[Tmp.uchFsc] != 0) {     /* With Extended FSC */

        /* ----- Get Length of Information String ----- */

        nLen = (short)lstrlenW(szInfo);

        /* ----- Check Keyed PLU ----- */
        if (Tmp.uchFsc == P03_KEYEDPLU) {

            /* ----- Load PLU Number String from Resource ----- */
            LoadString(hResourceDll, IDS_P03_PLU_NO, szExt, PEP_STRING_LEN_MAC(szExt));

            /* ----- Make Information String with PLU Number String ----- */

            uchOffset = (UCHAR)(Tmp.uchExt - 1);

            wsPepf(&szInfo[nLen], szExt, 
                     lpMenuPLU->PluKey[wPageNo][uchOffset].szPluNo);

            nLen = (short)lstrlenW(szInfo);

            /* ----- Load Label Type String from Resource ----- */
            LoadString(hResourceDll, IDS_P03_LABEL_MSG, szMsg, PEP_STRING_LEN_MAC(szMsg));

            /* ----- set PLU UPC-E or UPC-V flag ---- */
            if (lpMenuPLU->PluKey[wPageNo][uchOffset].uchModStat & MOD_STAT_EVERSION) {

                /* ----- Load Label Type String from Resource(E-Version) ----- */
                LoadString(hResourceDll, IDS_P03_TYPE_EVERSION, szType, PEP_STRING_LEN_MAC(szType));
            } else {

                /* ----- Load Label Type String from Resource(Normal) ----- */
                LoadString(hResourceDll, IDS_P03_TYPE_NORMAL, szType, PEP_STRING_LEN_MAC(szType));
            }

            wsPepf(&szInfo[nLen], szMsg, szType);


        } else if (Tmp.uchFsc == P03_KEYEDDPT) {


            /* ----- Make Information FSC String from Resorce ----- */

            LoadString(hResourceDll, IDS_P03_EXTEND, szExt, PEP_STRING_LEN_MAC(szExt));

            /* ----- Make Information String with Extended FSC String ----- */

            wsPepf(&szInfo[nLen], szExt, (short)Tmp.uchExt);

            /* ----- Load Dept Number String from Resource ----- */

            LoadString(hResourceDll, IDS_P03_DEPT_NO, szExt, PEP_STRING_LEN_MAC(szExt));

            /* ----- Make Information String Dept Number String ----- */

            nLen = (short)lstrlenW(szInfo);

            uchOffset = (UCHAR)(Tmp.uchExt - 1);

            wsPepf(&szInfo[nLen], szExt,
                     lpMenuDept->DeptKey[wPageNo][uchOffset].usDeptNumber);

        } else {
            /* ----- Load Extended FSC String from Resource ----- */

            LoadString(hResourceDll, IDS_P03_EXTEND, szExt, PEP_STRING_LEN_MAC(szExt));

            /* ----- Make Information String with Extended FSC String ----- */

            wsPepf(&szInfo[nLen], szExt, (short)Tmp.uchExt);
        }

    }

    /* ----- Show the Detail of Selected FSC ----- */

    LoadString(hResourceDll, IDS_P03_DETAIL_CAP, szCaption, PEP_STRING_LEN_MAC(szCaption));
    MessageBoxPopUp(hDlg, szInfo, szCaption, MB_OK);

    /* ----- Reset Indicating Flag ----- */

    fIndicateFlag = FALSE;

    /* ----- Get Device Context of Indicating Key for Restore Color ----- */

    hdcKey = GetDC(hIndKey);

    /* ----- Send Restore Color Message to Main DialogBox ----- */

    SendMessage(hDlg, WM_CTLCOLOR, MAKELONG(CTLCOLOR_STATIC, hIndKey), (LPARAM)hdcKey);

    /* ----- Release Device Context ----- */

    ReleaseDC(hIndKey, hdcKey);

    /* ----- Display FSC String ----- */

    P03SetStr(hDlg, (short)wDataOffset, (LPP03DATA)&Tmp);

}

/*
* ===========================================================================
**  Synopsis    :   VOID    P03ChgCursor()
*
**  Input       :   BOOL    fChgType    -   Type of Cursor Changing
*                   HCURSOR hcurDrag    -   Cursor Handle of Dragging Cursor
*
**  Return      :   None
*
**  Description :
*       This procedure changes a cursor (Default <-> Dragging).
* ===========================================================================
*/
VOID    P03ChgCursor(BOOL fChgType, HCURSOR hcurDrag)
{
    static  HCURSOR hSaveCur;

    if (fChgType == P03_CUR_HOLD) {     /* Shows Dragging Cursor */
        hSaveCur = SetCursor(hcurDrag);
        ShowCursor(TRUE);
    } else {                            /* Shows Defualt Cursor */
        ShowCursor(FALSE);
        SetCursor(hSaveCur);
    }
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    P03Comp2Pos()
*
**  Input       :   HWND    hDlg        -   Window Handle of DialogBox
*                   PPOINTS lpptDown    -   Address of X/Y Position
*                   PPOINT lpptUp      -   Address of X/Y Position
*
**  Return      :   FALSE               -   2 Positions are the Same Point
*                   TRUE                -   2 Positions Has a Difference
*
**  Description :
*       This procedure compares 2 positions (one is the mouse down point,
*   another is the mouse up point), and return its status.
*       The return status is TRUE, user operates "drag & drop" feature.
* ===========================================================================
*/
BOOL    P03Comp2Pos(HWND hDlg, PPOINTS lpptDown, PPOINTS lpptUp)
{
    BOOL    fRet;
    HWND    hwndDown,
            hwndUp;
	POINT  lptPoint;

	lptPoint.x = lpptDown->x;
	lptPoint.y = lpptDown->y;

    /* ----- Get Window Handle of Up/Down Position ----- */

    hwndDown = ChildWindowFromPoint(hDlg, lptPoint);

	lptPoint.x = lpptUp->x;
	lptPoint.y = lpptUp->y;
    hwndUp   = ChildWindowFromPoint(hDlg, lptPoint);

    /* ----- Determine Whether Up/Down Position is Same or Not ----- */

    if (hwndDown != hwndUp) {
        fRet = TRUE;
    } else {
        fRet = FALSE;
    }

    return (fRet);
}

/*
* ===========================================================================
**  Synopsis    :   HWND    P03ChkPos()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*                   PPOINTS lpptPos     -   Address of X/Y Mouse Position.
*
**  Return      :   HWND    hwndChild   -   Window Handle of Child Window.
*                           NULL        -   Current Position is Invalid.
*
**  Description :
*       This procedure determines whether the clicked position is valid
*   or not valid.
* ===========================================================================
*/
HWND    P03ChkPos(HWND hDlg, PPOINTS lpptPos)
{
    HWND    hwndChild;
    WORD    wChildID;
	POINT  lptPoint;

	lptPoint.x = lpptPos->x;
	lptPoint.y = lpptPos->y;

    /* ----- Get Handle of Child Window Contains Current Position ----- */


    hwndChild = ChildWindowFromPoint(hDlg, lptPoint);

    if (hwndChild != NULL) {
                        /* check current position lies outside of parent */

        /* ----- get control ID of child window ----- */

        wChildID = GetDlgCtrlID(hwndChild);

        /*
			----- Check Current Position is within valid range or not -----
			If it is within a valid range, check to see if it is a fixed key or not.
		*/

        if ((wChildID < IDD_P03_KEY_00) || (IDD_P03_KEY_AF < wChildID)) {
            hwndChild = NULL;
        }
		else {
			if (P03ChkFixKey (wChildID))
				hwndChild = NULL;
		}

    }

    return (hwndChild);
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    P03MakeExtFsc()
*
**  Input       :   HWND    hDlg    -   Window Handle of Keyboard DialogBox
*
**  Return      :   TRUE            -   Not Find or Set a Extended FSC
*                   FALSE           -   User Pressed Cancel Button
*
**  Description :
*       This procedure checkes the Extended FSC. If the Ext FSC is found,
*           entry dialogbox is appeared to set it. The DLG procedure returns
*           the integer value when the data sets and is pressed a OK button.
* ===========================================================================
*/
BOOL    P03MakeExtFsc(HWND hDlg)
{
    BOOL    fRet;

    switch (wCurBtnID) {
    case IDD_P03_KEYEDPLU:
        /* ----- Create the Dialogbox to Input the Extended FSC, Saratoga ----- */

        fRet = (BOOL)DialogBoxPopup(hResourceDll,
                               MAKEINTRESOURCEW(IDD_P03_PLUENTRY),
                               hDlg,
                               P003ExtPluDlgProc);

        break;

    case IDD_P03_KEYEDDPT:
        /* ----- Create the Dialogbox to Input the Extended FSC, Saratoga ----- */

        fRet = (BOOL)DialogBoxPopup(hResourceDll,
                               MAKEINTRESOURCEW(IDD_P03_DEPTENTRY),
                               hDlg,
                               P003ExtDeptDlgProc);
        break;

    case IDD_P03_PRTMOD:
    case IDD_P03_ADJ:
    case IDD_P03_TENDER:
    case IDD_P03_FC:
    case IDD_P03_TTL:
    case IDD_P03_ADDCHK:
    case IDD_P03_IDISC:
    case IDD_P03_RDISC:
    case IDD_P03_TAXMOD:
    case IDD_P03_CHARTIP:
    case IDD_P03_PRECASH:
    case IDD_P03_DRCTSHFT:
    case IDD_P03_PRTDEM:
    case IDD_P03_KEYEDCPN:
    case IDD_P03_KEYEDSTR:
    case IDD_P03_SEAT:
    case IDD_P03_ASK:                             /*** NCR2172 ***/
    case IDD_P03_ADJPLS:                          /*** NCR2172 ***/
    case IDD_P03_ALPHANUM:
	case IDD_P03_GIFT_CARD:
	case IDD_P03_ORDER_DEC:
	case IDD_P03_DOC_LAUNCH:    // FSC_DOC_LAUNCH

        /* ----- Create the Dialogbox to Input the Extended FSC ----- */

        fRet = (BOOL)DialogBoxPopup(hResourceDll,
                               MAKEINTRESOURCEW(IDD_P03_ENTRY),
                               hDlg,
                               P003ExtDlgProc);

        break;

    default:
        fRet = TRUE;
    }

    return (fRet);
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  P003ExtDlgProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wMsg    -   Dispacthed Message
*                   WORD    wParam  -   16 bits Message Parameter
*                   LONG    lParam  -   32 bits Message Parameter
*
**  Return      :   TRUE            -   User Process is Executed
*                   FALSE           -   Default Process is Expected
*
**  Description:
*       This is a dialgbox procedure for the Extended FSC.
* ===========================================================================
*/
BOOL WINAPI  P003ExtDlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  UINT    unVal;

    PEPSPIN PepSpin;
    BOOL    fRet = FALSE;
    WCHAR    szDesc[P03_DSC_LEN];

    switch (wMsg) {

    case WM_INITDIALOG:

        SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- Get Max. Value of the Extended FSC ----- */

        unVal = awExtRngTbl[P03MAJORFSCCODE(wCurBtnID)];

        /* ----- Set Max. Value Number to StaticText ----- */

        SetDlgItemInt(hDlg, IDD_P03_EXTRANGE, unVal, FALSE);

        /* ----- Load String of Major FSC Data from Resource ----- */
        /* ----- Set Loaded String to StaticText ----- */

		LoadString(hResourceDll, IDS_P03_INF_NOFUNC + P03MAJORFSCCODE(wCurBtnID), szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_P03_EXTMNEMO,szDesc);

        if (wCurBtnID == IDD_P03_KEYEDPLU) {

            /* ----- Load String of PLU Number from Resource ----- */
			LoadString(hResourceDll, IDS_P03_PLU_MSG, szDesc, PEP_STRING_LEN_MAC(szDesc));
			DlgItemRedrawText(hDlg, IDD_P03_EXTEND_MSG, szDesc);
        } else {

            /* ----- Load String of extend message from Resource ----- */
			LoadString(hResourceDll, IDS_P03_EXTEND_MSG, szDesc, PEP_STRING_LEN_MAC(szDesc));
			DlgItemRedrawText(hDlg, IDD_P03_EXTEND_MSG, szDesc);
        }

        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) 
		{
			SendDlgItemMessage(hDlg, IDD_P03_EXTMNEMO, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_P03_EXTEND_MSG, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_P03_DATA_RANGE, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_P03_EXTFSC, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_P03_EXTRANGE, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_VSCROLL:

        PepSpin.lMax       = (long)unVal;
        PepSpin.lMin       = (long)P03_EXT_MIN;
        PepSpin.nStep      = P03_SPIN_STEP;
        PepSpin.nTurbo     = P03_SPIN_TURBO;
        PepSpin.fSpinStyle = PEP_SPIN_NOROLL;

        fRet = PepSpinProc(hDlg, wParam, IDD_P03_EXTFSC, (LPPEPSPIN)&PepSpin);

        return (fRet);

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDD_P03_EXTFSC:    /* Inputed Extended FSC Data    */

            if (HIWORD(wParam) == EN_CHANGE) {

                /* ----- Check Inputed Value with Data Range ----- */

                P03ChkExtRng(hDlg, unVal);

                return TRUE;
            }
            return FALSE;

        case IDOK:              /* Pressed OK Button        */
        case IDCANCEL:          /* Pressed Cancel Button    */

            if (HIWORD(wParam) == BN_CLICKED) {

                if (LOWORD(wParam) == IDOK) {

                    /* ----- Get Extended FSC Data ----- */

                    if (P03GetExtFsc(hDlg, unVal)) {
                                                /* user inputed 0 value */
                        return TRUE;
                    }

                    /* ----- Set Return Status (OK Button is Selected) ----- */

                    fRet = TRUE;
                }

                EndDialog(hDlg, fRet);

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
**  Synopsis    :   BOOL    WINAPI  P003ExtDeptDlgProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wMsg    -   Dispacthed Message
*                   WORD    wParam  -   16 bits Message Parameter
*                   LONG    lParam  -   32 bits Message Parameter
*
**  Return      :   TRUE            -   User Process is Executed
*                   FALSE           -   Default Process is Expected
*
**  Description:
*       This is a dialgbox procedure for the Extended FSC.
* ===========================================================================
*/
BOOL WINAPI  P003ExtDeptDlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{

    static  UINT    unVal;

    int     idEdit = 0;

    PEPSPIN PepSpin;
    BOOL    fRet = FALSE;
    WCHAR   szDesc[P03_DSC_LEN];

    switch (wMsg) {

    case WM_INITDIALOG:

        SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- Get Max. Value of the Extended FSC ----- */

        unVal = awExtRngTbl[P03MAJORFSCCODE(wCurBtnID)];

        /* ----- Set Max. Value Number to StaticText ----- */

        SetDlgItemInt(hDlg, IDD_P03_EXTRANGE, unVal, FALSE);

        /* ----- Load String of Major FSC Data from Resource ----- */
        /* ----- Set Loaded String to StaticText ----- */
		
		LoadString(hResourceDll, IDS_P03_INF_NOFUNC + P03MAJORFSCCODE(wCurBtnID), szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_P03_EXTMNEMO, szDesc);

		/* ----- Load String of extend message from Resource ----- */
        /* ----- Set Loaded String to StaticText ----- */

		LoadString(hResourceDll, IDS_P03_EXTEND_MSG, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_P03_EXTEND_MSG, szDesc);


        /* ----- Limit Length of Input Data to EditText (DEPT No) ----- */
 
        SendDlgItemMessage(hDlg, IDD_P03_DEPTIN, EM_LIMITTEXT,P03_DEPT_LEN, 0L);

        /* ----- Limit Length of Input Data to EditText (FSC) ----- */
 
        SendDlgItemMessage(hDlg, IDD_P03_EXTFSC, EM_LIMITTEXT, P03_FSC_LEN,0L);

        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {
			SendDlgItemMessage(hDlg, IDD_P03_EXTMNEMO, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_P03_EXTEND_MSG, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_P03_DATA_RANGE, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_P03_DATA_RANGE2, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_P03_DEPT_NUM, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_P03_EXTFSC, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_P03_DEPTIN, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_P03_EXTRANGE, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_VSCROLL:

        PepSpin.lMax       = (long)unVal;
        PepSpin.lMin       = (long)P03_EXT_MIN;
        PepSpin.nStep      = P03_SPIN_STEP;
        PepSpin.nTurbo     = P03_SPIN_TURBO;
        PepSpin.fSpinStyle = PEP_SPIN_NOROLL;

        if (GetDlgCtrlID((HWND)lParam) == IDD_P03_SPIN)
           idEdit = IDD_P03_EXTFSC;
        else 
           idEdit = IDD_P03_DEPTIN;

        fRet = PepSpinProc(hDlg, wParam, idEdit, (LPPEPSPIN)&PepSpin);

        return (fRet);

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDD_P03_EXTFSC:    /* Inputed Extended FSC Data    */

            if (HIWORD(wParam) == EN_CHANGE) {

                /* ----- Check Inputed Value with Data Range ----- */

                P03ChkExtRng(hDlg, unVal);

                return TRUE;
            }
            return FALSE;

        case IDD_P03_DEPTIN:    /* Inputed Dept Number 1 - 9999 */

            if (HIWORD(wParam) == EN_CHANGE) {

                /* ----- Check Inputed value with Data Range ----- */

                P03ChkDeptRng(hDlg);

                return TRUE;
            }
          
            return FALSE;


        case IDOK:              /* Pressed OK Button        */
        case IDCANCEL:          /* Pressed Cancel Button    */

            if (HIWORD(wParam) == BN_CLICKED) {

                if (LOWORD(wParam) == IDOK) {

                    /* ----- Get Extended FSC Data ----- */

                    if (P03GetExtFsc(hDlg, unVal)) {
                                                /* user inputed 0 value */
                        return TRUE;
                    }


                    /* ----- Get Extended Dept NO ----- */

                    if (P03GetExtDept(hDlg)) {

                        return TRUE;            /* user inputed 0 value */

                    }

                    /* ----- Set Return Status (OK Button is Selected) ----- */

                    fRet = TRUE;

                }

                EndDialog(hDlg, fRet);

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
**  Synopsis    :   BOOL    WINAPI  P003ExtPluDlgProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wMsg    -   Dispacthed Message
*                   WORD    wParam  -   16 bits Message Parameter
*                   LONG    lParam  -   32 bits Message Parameter
*
**  Return      :   TRUE            -   User Process is Executed
*                   FALSE           -   Default Process is Expected
*
**  Description:
*       This is a dialgbox procedure for the Extended FSC.
* ===========================================================================
*/
BOOL WINAPI  P003ExtPluDlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
//    static  UINT    unVal;

    BOOL    fRet = FALSE;
    WCHAR   szDesc[P03_DSC_LEN];
    UINT    wI;                   /* Saratoga */

    switch (wMsg) {

    case WM_INITDIALOG:

        SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- Get Max. Value of the Extended FSC ----- */

//        unVal = awExtRngTbl[P03MAJORFSCCODE(wCurBtnID)];

        /* ----- Load String of Major FSC Data from Resource ----- */
        /* ----- Set Loaded String to StaticText ----- */

		LoadString(hResourceDll, IDS_P03_INF_NOFUNC + P03MAJORFSCCODE(wCurBtnID), szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_P03_EXTMNEMO, szDesc);

        if (wCurBtnID == IDD_P03_KEYEDPLU) {

            /* ----- Load String of PLU Number from Resource ----- */
            LoadString(hResourceDll,
                       IDS_P03_PLU_MSG,
                       szDesc,
                       PEP_STRING_LEN_MAC(szDesc));  
        }

        /* ----- Set Loaded String to StaticText ----- */

		LoadString(hResourceDll, IDS_P03_PLU_MSG, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, IDD_P03_EXTEND_MSG, szDesc);

/* Saratoga Start */
            /* ----- Set maintenance type to combo-box ----- */

            for( wI = 0; wI < P03_TYPE_MAX; wI++) {
                 LoadString(hResourceDll, IDS_P03_TYPE_NORMAL + wI, szDesc, PEP_STRING_LEN_MAC(szDesc));

				 DlgItemSendTextMessage(hDlg, IDD_P03_LABEL, CB_INSERTSTRING,(WPARAM)(-1), (LPARAM)szDesc);
            }

            SendDlgItemMessage(hDlg, IDD_P03_LABEL, CB_SETCURSEL, 0, 0L);

/* Saratoga End */

        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {
			SendDlgItemMessage(hDlg, IDD_P03_EXTMNEMO, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_P03_EXTEND_MSG, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_P03_DATA_RANGE, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_P03_LABEL_TYPE, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_P03_EXTFSC, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_P03_LABEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {

        case IDOK:              /* Pressed OK Button        */
        case IDCANCEL:          /* Pressed Cancel Button    */

            if (HIWORD(wParam) == BN_CLICKED) {

                if (LOWORD(wParam) == IDOK) {

                    /* ----- Get Extended FSC Data ----- */

                    if (P03GetExtFscPlu(hDlg)) {
                                                /* user inputed 0 value */
                        return TRUE;
                    }

                    /* ----- Set Return Status (OK Button is Selected) ----- */

                    fRet = TRUE;

                }

                EndDialog(hDlg, fRet);

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
**  Synopsis    :   VOID    P03ChkExtRng()
*
**  Input       :   HWND    hDlg    -   Window Handle of Extended FSC Dialog
*                   UINT    unVal   -   Maximum Value of Extended FSC
*
**  Return      :   None
*
**  Description :
*       This procedure checkes inputed Extended FSC Data with data range.
* ===========================================================================
*/
VOID    P03ChkExtRng(HWND hDlg, UINT unVal)
{
    UINT    unData;
    WCHAR    szCap[P03_CAP_LEN],
            szErr[P03_ERR_LEN],
            szWork[P03_ERR_LEN];

    /* ----- Get Extended FSC Data from EditText ----- */

    unData = GetDlgItemInt(hDlg, IDD_P03_EXTFSC, NULL, FALSE);

    if (unData > unVal) {       /* Out of Data Range    */

        /* ----- Load Error Message from Resource ----- */

        LoadString(hResourceDll, IDS_PEP_CAPTION_P03, szCap, PEP_STRING_LEN_MAC(szCap));
        LoadString(hResourceDll, IDS_PEP_OVERRANGE,   szErr, PEP_STRING_LEN_MAC(szErr));

        /* ----- Make Error Message with Maximum Value ----- */

        wsPepf(szWork, szErr, P03_EXT_MIN, unVal);

        /* ----- Display Error Message ----- */

        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg,
                   szWork,
                   szCap,
                   MB_OK | MB_ICONEXCLAMATION);

        /* ----- Undo Data Input ----- */

        SendDlgItemMessage(hDlg, IDD_P03_EXTFSC, EM_UNDO, 0, 0L);

    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P03ChkDeptRng()
*
**  Input       :   HWND    hDlg    -   Window Handle of Extended FSC Dialog
*                   UINT    unVal   -   Maximum Value of Extended FSC
*
**  Return      :   None
*
**  Description :
*       This procedure checkes inputed Extended FSC Data with data range.
* ===========================================================================
*/
VOID    P03ChkDeptRng(HWND hDlg)
{
    UINT    unData;
    WCHAR   szCap[P03_CAP_LEN],
            szErr[P03_ERR_LEN],
            szWork[P03_ERR_LEN];

    /* ----- Get Extended FSC Data from EditText ----- */

    unData = GetDlgItemInt(hDlg, IDD_P03_DEPTIN, NULL, FALSE);

    if (unData > P03_DEPT_MAX) {       /* Out of Data Range    */

        /* ----- Load Error Message from Resource ----- */

        LoadString(hResourceDll, IDS_PEP_CAPTION_P03, szCap, PEP_STRING_LEN_MAC(szCap));
        LoadString(hResourceDll, IDS_P03_DEPTOVERRANGE,   szErr, PEP_STRING_LEN_MAC(szErr));

        /* ----- Make Error Message with Maximum Value ----- */

        wsPepf(szWork, szErr, P03_EXT_MIN, P03_DEPT_MAX);

        /* ----- Display Error Message ----- */

        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg,
                   szWork,
                   szCap,
                   MB_OK | MB_ICONEXCLAMATION);

        /* ----- Undo Data Input ----- */

        SendDlgItemMessage(hDlg, IDD_P03_DEPTIN, EM_UNDO, 0, 0L);

    }
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    P03GetExtFsc()
*
**  Input       :   HWND    hDlg    -   Window handle of Extended FSC Dialog
*                   UINT    unVal   -   Maximum Value of Data Range
*
**  Return      :   TRUE            -   User Inputed 0 Value.
*                   FALSE           -   No Error.
*
**  Description :
*       This procedure gets the Inputed Extended FSC Data, and then checks
*   it with data range. If inputed value is invalid, returns TRUE. Otherwise
*   returns FALSE.
* ===========================================================================
*/
BOOL    P03GetExtFsc(HWND hDlg, UINT unVal)
{
    BOOL    fRet,
            fTrans;
    WCHAR   szCap[P03_CAP_LEN],
            szErr[P03_ERR_LEN],
            szWork[P03_ERR_LEN];

    /* ----- Get Inputed Extended FSC Data from EditText ----- */

    unExtFscData = GetDlgItemInt(hDlg, IDD_P03_EXTFSC, (LPBOOL)&fTrans, FALSE);

    if ((unExtFscData == 0) || (fTrans == 0)) {           /* Inputed Data is 0 */

        /* ----- Load Error Message from Resource ----- */

        LoadString(hResourceDll, IDS_PEP_CAPTION_P03, szCap, PEP_STRING_LEN_MAC(szCap));
        LoadString(hResourceDll, IDS_PEP_OVERRANGE,   szErr, PEP_STRING_LEN_MAC(szErr));

        /* ----- Make Error Message with Maximum Value ----- */

        wsPepf(szWork, szErr, P03_EXT_MIN, unVal);

        /* ----- Display Error Message ----- */

        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg,
                   szWork,
                   szCap,
                   MB_OK | MB_ICONEXCLAMATION);

        /* ----- Set High-light to EditText ----- */

        SendDlgItemMessage(hDlg, IDD_P03_EXTFSC, EM_SETSEL, 0, MAKELONG(0, -1));

        /* ----- Set Focus to EditText ----- */

        SetFocus(GetDlgItem(hDlg, IDD_P03_EXTFSC));

        fRet = TRUE;

    } else {
        fRet = FALSE;
    }

    return (fRet);
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    P03GetExtDept()
*
**  Input       :   HWND    hDlg    -   Window handle of Extended FSC Dialog
*
**  Return      :   TRUE            -   User Inputed 0 Value.
*                   FALSE           -   No Error.
*
**  Description :
*       This procedure gets the Inputed Extended FSC Data, and then checks
*   it with data range. If inputed value is invalid, returns TRUE. Otherwise
*   returns FALSE.
* ===========================================================================
*/
BOOL    P03GetExtDept(HWND hDlg)
{
    BOOL    fRet,
            fTrans;
    WCHAR   szCap[P03_CAP_LEN],
            szErr[P03_ERR_LEN],
            szWork[P03_ERR_LEN];

    /* ----- Get Inputed Extended FSC Data from EditText ----- */

    usDeptNumber = (USHORT)GetDlgItemInt(hDlg, IDD_P03_DEPTIN, (LPBOOL)&fTrans, FALSE);

    if ((usDeptNumber == 0) || (fTrans == 0)) {           /* Inputed Data is 0 */

        /* ----- Load Error Message from Resource ----- */

        LoadString(hResourceDll, IDS_PEP_CAPTION_P03, szCap, PEP_STRING_LEN_MAC(szCap));
        LoadString(hResourceDll, IDS_P03_DEPTOVERRANGE,   szErr, PEP_STRING_LEN_MAC(szErr));

        /* ----- Make Error Message with Maximum Value ----- */

        wsPepf(szWork, szErr, P03_EXT_MIN, P03_DEPT_MAX);

        /* ----- Display Error Message ----- */

        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg,
                   szWork,
                   szCap,
                   MB_OK | MB_ICONEXCLAMATION);

        /* ----- Set High-light to EditText ----- */

        SendDlgItemMessage(hDlg, IDD_P03_DEPTIN, EM_SETSEL, 0, MAKELONG(0, -1));

        /* ----- Set Focus to EditText ----- */

        SetFocus(GetDlgItem(hDlg, IDD_P03_DEPTIN));

        fRet = TRUE;

    } else {
        fRet = FALSE;
    }

    return (fRet);
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    P03GetExtFscPlu()
*
**  Input       :   HWND    hDlg    -   Window handle of Extended FSC Dialog
*
**  Return      :   TRUE            -   User Inputed 0 Value.
*                   FALSE           -   No Error.
*
**  Description :
*       This procedure gets the Inputed Extended FSC Data, and then checks
*   it with data range. If inputed value is invalid, returns TRUE. Otherwise
*   returns FALSE.
* ===========================================================================
*/
BOOL    P03GetExtFscPlu(HWND hDlg)
{
    BOOL    fRet;
    WCHAR   szCap[P03_CAP_LEN],
            szErr[P03_ERR_LEN],
            szWork[P03_ERR_LEN];

    WCHAR   szPluNo[PLU_MAX_DIGIT + 1];
    USHORT  usLen;

    LABELANALYSIS Label;         /* Saratoga */
    short   sStat;               /* Saratoga */
    short   nCount;              /* Saratoga */
    BOOL    bCount = TRUE;       /* Saratoga */
    short   nNumCnt, nZeroCnt;   /* Saratoga */

    /* ----- Get Inputed Extended FSC Data from EditText ----- */

    memset(szPluNo, 0x00, sizeof(szPluNo));
    usLen = (USHORT)DlgItemGetText(hDlg, IDD_P03_EXTFSC, szPluNo, PEP_STRING_LEN_MAC(szPluNo));

    /* ----- Extended FSC Data numeric check ----- */
    
    for (nCount = 0; nCount < PLU_MAX_DIGIT; nCount++) {

       if (!(isdigit(szPluNo[nCount]))) {

         if ((szPluNo[nCount] == 0x20) || (szPluNo[nCount] == 0x00)) {
           continue;
         } else {
           bCount = FALSE;
           break;
         }
       }

   }

    /* ----- All ZERO Check ----- */
    nNumCnt = nZeroCnt = 0;
    if (bCount != FALSE) {

       for (nCount = 0; nCount < PLU_MAX_DIGIT; nCount++) {
          if ((szPluNo[nCount] >= '1') && (szPluNo[nCount] <= '9')) {
             nNumCnt++;
          } else if (szPluNo[nCount] == '0') {
             nZeroCnt++;
          }
       }
    }

    if((usLen == 0) || (bCount == FALSE) || (nNumCnt == 0) || (nZeroCnt == PLU_MAX_DIGIT)) {
            
        /* ----- Load Error Message from Resource ----- */

        LoadString(hResourceDll, IDS_PEP_CAPTION_P03, szCap, PEP_STRING_LEN_MAC(szCap));
        LoadString(hResourceDll, IDS_P03_OVERRANGE,   szErr, PEP_STRING_LEN_MAC(szErr));

        /* ----- Make Error Message with Maximum Value ----- */

        wsPepf(szWork, szErr, P03_EXT_MIN, szExtRngPlu);

        /* ----- Display Error Message ----- */

        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg,
                   szWork,
                   szCap,
                   MB_OK | MB_ICONEXCLAMATION);

        /* ----- Set High-light to EditText ----- */

        SendDlgItemMessage(hDlg, IDD_P03_EXTFSC, EM_SETSEL, 0, MAKELONG(0, -1));

        /* ----- Set Focus to EditText ----- */

        SetFocus(GetDlgItem(hDlg, IDD_P03_EXTFSC));

        fRet = TRUE;

    } else {

        memcpy(strPluNo, szPluNo, sizeof(szPluNo));

        /* ----- Get Label Type ----- */

        unPLULab = (UINT)SendDlgItemMessage(hDlg,
                                            IDD_P03_LABEL,
                                            CB_GETCURSEL,
                                            0,
                                            0L);

/* Saratoga Start */

        /* ----- Initialize LABELANALYSIS buffer ----- */

        memset((LPSTR)&Label, 0x00, sizeof(Label));
        memcpy/*_tchar2uchar*/ (Label.aszScanPlu, strPluNo, sizeof(strPluNo));

        if(unPLULab == LA_EMOD) {
           Label.fchModifier = LA_EMOD;      /* Set E Flag */
        } else {
           Label.fchModifier = LA_NORMAL;    /* Set Normal Flag */
        }

        /* ----- Analyze PLU No. ----- */

        sStat = RflLabelAnalysis(&Label);

        if(sStat == LABEL_ERR) {

           LoadString(hResourceDll, IDS_PEP_CAPTION_P03, szCap, PEP_STRING_LEN_MAC(szCap));
           LoadString(hResourceDll, IDS_P03_ERR_PLUNO, szErr, PEP_STRING_LEN_MAC(szErr));

           /* ----- Display caution message ----- */

           MessageBeep(MB_ICONEXCLAMATION);
           MessageBoxPopUp(hDlg,
                      szErr,
                      szCap,
                      MB_OK | MB_ICONEXCLAMATION);

           /* ----- Set Focus to EditText ----- */

           SetFocus(GetDlgItem(hDlg, IDD_P03_EXTFSC));

           fRet = TRUE;

        } else {

           if ((Label.uchType == LABEL_RANDOM) && (Label.uchLookup != LA_EXE_LOOKUP)) {

              LoadString(hResourceDll, IDS_PEP_CAPTION_P03, szCap, PEP_STRING_LEN_MAC(szCap));
              LoadString(hResourceDll, IDS_P03_ERR_LABEL, szErr, PEP_STRING_LEN_MAC(szErr));

              /* ----- Display caution message ----- */

              MessageBeep(MB_ICONEXCLAMATION);
              MessageBoxPopUp(hDlg,
                         szErr,
                         szCap,
                         MB_OK | MB_ICONEXCLAMATION);

              /* ----- Set Focus to EditText ----- */

              SetFocus(GetDlgItem(hDlg, IDD_P03_EXTFSC));
              fRet = TRUE;
           } else {
             fRet = FALSE;
           }
        }

/* Saratoga End */

    }

    return (fRet);
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  P003ToolDlgProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wMsg    -   Dispacthed Message
*                   WORD    wParam  -   16 bits Message Parameter
*                   LONG    lParam  -   32 bits Message Parameter
*
**  Return      :   TRUE            -   User Process is Executed
*                   FALSE           -   Default Process is Expected
*
**  Description:
*       This is a dialgbox procedure for the FSC Tool Box.
* ===========================================================================
*/
BOOL    WINAPI  P003ToolDlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    switch (wMsg) {
    case WM_INITDIALOG:

        SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

		/* ----- Delete "CLOSE" Menu from System Menu ----- */

        DeleteMenu(GetSystemMenu(hDlg, FALSE), SC_CLOSE, MF_BYCOMMAND);

        
//		InitToolboxDlgStrings(hDlg);  //4-22-03 RPH Multilingual
		
		/* ----- Change the Default Selected Tool Button Style ----- */

        P03ChgBtnStyle(hDlg, IDD_P03_NOFUNC);

        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j = IDD_P03_NOFUNC; j<= IDD_P03_LASTKEY; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
		}
		return FALSE;

    case WM_ACTIVATE:

        if ((wParam == WA_ACTIVE) || (wParam == WA_CLICKACTIVE)) {

            /* ----- Post a Message to Disable ToolBox Button ----- */

            PostMessage(*lphProgDlgChk1, PM_P03HIDETOOL, 0, 0L);

            return TRUE;
        }
        return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDD_P03_NOFUNC:
        case IDD_P03_KEYEDPLU:
        case IDD_P03_KEYEDDPT:
        case IDD_P03_PRTMOD:
        case IDD_P03_ADJ:
        case IDD_P03_TENDER:
        case IDD_P03_FC:
        case IDD_P03_TTL:
        case IDD_P03_ADDCHK:
        case IDD_P03_IDISC:
        case IDD_P03_RDISC:
        case IDD_P03_TAXMOD:
        case IDD_P03_AKEY:
        case IDD_P03_BKEY:
/*        case IDD_P03_SERVNO: */
        case IDD_P03_NEWCHK:
        case IDD_P03_PB:
        case IDD_P03_GCNO:
        case IDD_P03_GCTRANS:
        case IDD_P03_NOPERSON:
        case IDD_P03_TBLNO:
        case IDD_P03_LINENO:
        case IDD_P03_FEED:
        case IDD_P03_RCTNORCT:
        case IDD_P03_TIPSPO:
        case IDD_P03_DECTIP:
        case IDD_P03_NOSALE:
        case IDD_P03_PO:
        case IDD_P03_ROA:
        case IDD_P03_CHGCOMP:
        case IDD_P03_CANCEL:
        case IDD_P03_VOID:
        case IDD_P03_EC:
        case IDD_P03_VTRAN:
        case IDD_P03_QTY:
        case IDD_P03_SHIFT:
        case IDD_P03_NOTYPE:
        case IDD_P03_PRTDEM:
/*        case IDD_P03_ORDERNO: */
        case IDD_P03_CHARTIP:
        case IDD_P03_PLUNO:
        case IDD_P03_00KEY:
        case IDD_P03_AC:
        case IDD_P03_RECFEED:
        case IDD_P03_JOUFEED:
        case IDD_P03_DECPT:
        case IDD_P03_SCALE:
        case IDD_P03_RVSPRI:
        case IDD_P03_PRECASH:
        case IDD_P03_DRCTSHFT:
        case IDD_P03_TIMEIN:
        case IDD_P03_TIMEOUT:
        case IDD_P03_OFFTEND:
        case IDD_P03_KEYEDCPN:
        case IDD_P03_CPNNO:
        case IDD_P03_UP:
        case IDD_P03_DOWN:
        case IDD_P03_RIGHT:
        case IDD_P03_LEFT:
        case IDD_P03_WAIT:
        case IDD_P03_PAID_REC:
        case IDD_P03_KEYEDSTR:
        case IDD_P03_STRNO:
        case IDD_P03_ALPHA:
        case IDD_P03_LEFTDISPLAY:
        case IDD_P03_BARTENDER:
        case IDD_P03_SPLIT:
        case IDD_P03_SEAT:
        case IDD_P03_ITEMTRANS:
        case IDD_P03_REPEAT:
        case IDD_P03_CURSORVOID:
        case IDD_P03_PRICECHECK:
        case IDD_P03_OPRINT_A:
        case IDD_P03_OPRINT_B:
/*** NCR2172 ***/
        case IDD_P03_MONEYDEC:
        case IDD_P03_ASK:
        case IDD_P03_ADJPLS:
        case IDD_P03_DEPTNO:
        case IDD_P03_UPCEVER:
        case IDD_P03_PRICEENT:
/* ### ADD Saratoga (052700) */
        case IDD_P03_FOODSTAMPMOD:
        case IDD_P03_WICTRANS:
        case IDD_P03_WICMOD:
		case IDD_P03_HALO_OVERRIDE:
		case IDD_P03_AT_FOR:
		case IDD_P03_VALIDATION:
        case IDD_P03_ALPHANUM:
        case IDD_P03_CLEARFSC:
		case IDD_P03_EDIT_CONDIMENT:
		case IDD_P03_EDIT_COND_TBL:
		case IDD_P03_TENT:
		case IDD_P03_GIFT_CARD:
		case IDD_P03_ORDER_DEC:
		case IDD_P03_SUPR_INTRVN:    // FSC_SUPR_INTRVN
		case IDD_P03_DOC_LAUNCH:     // FSC_DOC_LAUNCH
		case IDD_P03_OPR_PICKUP:     // FSC_OPR_PICKUP
		case IDD_P03_ADJ_MOD:
		case IDD_P03_AUTO_SIGN_OUT:

            if (HIWORD(wParam) == BN_CLICKED) {

                /* ----- Change Selected Button Style ----- */

                P03ChgBtnStyle(hDlg, LOWORD(wParam));

                /* ----- Set Active Keyboard DialogBox ----- */

                SetActiveWindow(*lphProgDlgChk1);

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
**  Synopsis    :   VOID    P03ChgBtnStyle()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*                   WORD    wBtnID  -   Current Selected Button ID
*
**  Return      :   None
*
**  Description :
*       This procedure changes the style of the selected button, and then
*   saves its identifier to static area for check Extended FSC.
* ===========================================================================
*/
VOID    P03ChgBtnStyle(HWND hDlg, WORD wBtnID)
{
    RECT    rcButton,
            rcClient,
            rcDialog;
    int     cxWindow,
            cyWindow,
            cyCaption,
            cxFlame;

    /* ----- Get the Dimensions of the Rectangle of Dialog ----- */

    GetWindowRect(hDlg, (LPRECT)&rcDialog);

    /* ----- Get the Dimensions of the Rectangle of Client Area ----- */

    GetClientRect(hDlg, (LPRECT)&rcClient);

    /* ----- Get Width and Height of DialogBox ----- */

    cxWindow = rcDialog.right  - rcDialog.left;
    cyWindow = rcDialog.bottom - rcDialog.top;

    /* ----- Get Width/Height of Window Flame and Window Caption ----- */

    cxFlame   = ((cxWindow - rcClient.right) / 2);
    cyCaption = (cyWindow - rcClient.bottom - cxFlame);

    if (wCurBtnID) {

        /* ----- Reset the Previous Button Style, If It is Changed ----- */

        SendDlgItemMessage(hDlg,
                           wCurBtnID,
                           BM_SETSTYLE,
                           (WPARAM)BS_PUSHBUTTON,
                           TRUE);

        /* ----- Get the Dimensions of the Rectangle of Button ----- */

        GetWindowRect(GetDlgItem(hDlg, wCurBtnID), (LPRECT)&rcButton);

        /* ----- Calculate X-Y Position of Button in DilaogBox ----- */

        rcButton.left   -= (rcDialog.left + cxFlame);
        rcButton.top    -= (rcDialog.top + cyCaption);
        rcButton.right  -= (rcDialog.left + cxFlame);
        rcButton.bottom -= (rcDialog.top + cyCaption);

        /* ----- Redraw Specified Button ----- */

        InvalidateRect(hDlg, (const LPRECT)&rcButton, TRUE);
    }

    /* ----- Change Style of Selected Button to GroupBox ----- */

    SendDlgItemMessage(hDlg, wBtnID, BM_SETSTYLE, (WPARAM)BS_GROUPBOX, TRUE);

    /* ----- Get the Dimensions of the Rectangle of Button ----- */

    GetWindowRect(GetDlgItem(hDlg, wBtnID), (LPRECT)&rcButton);

    /* ----- Calculate X-Y Position of Button in DilaogBox ----- */

    rcButton.left   -= (rcDialog.left + cxFlame);
    rcButton.top    -= (rcDialog.top + cyCaption);
    rcButton.right  -= (rcDialog.left + cxFlame);
    rcButton.bottom -= (rcDialog.top + cyCaption);

    /* ----- Redraw Specified Button ----- */

    InvalidateRect(hDlg, (const LPRECT)&rcButton, TRUE);

    /* ----- Save Current Selected Button ID for Check Extended FSC ----- */

    wCurBtnID = wBtnID;

}

/*
* ===========================================================================
**  Synopsis    :   HBRUSH  P03DrawWnd()
*
**  Input       :   WPARAM      wParam      -   Child Window Display Context
*                   LPARAM      lParam      -   Window Handle & Type of Control
*                   BOOL        fCapture    -   Mouse Capture Flag
*                   LPHBRUSH    lphbrText   -   Brush Handle of Text Color
*                   LPHBRUSH    lphbrBk     -   Brush Handle of Back Color
*
**  Return      :   Current Painted Background Color Brush
*
**  Description :
*       This function draws dialogbox controls with current system color.
* ===========================================================================
*/
HBRUSH  P03DrawWnd(WPARAM wParam,
                   LPARAM lParam,
                   BOOL fCapture,
                   LPHBRUSH lphbrText,
                   LPHBRUSH lphbrBk)
{
    HBRUSH  hbrTemp = NULL;             /* Handle of Temporary Brush        */
    DWORD   dwTextColor = 0L;           /* Current Window Text Color        */
    DWORD   dwBkColor = 0L;             /* Current Window Background Color  */

    /* ----- Determine Whether the Indicating Key or Not ----- */

    if (LOWORD(wParam) == hIndKey) {

        /* ----- Get Current System Color ----- */

        dwTextColor = GetSysColor(COLOR_WINDOWTEXT);
        dwBkColor   = GetSysColor(COLOR_WINDOW);

        /* ----- Delete Current Using Brush Handle ----- */

        if (*lphbrText) {
            DeleteObject(*lphbrText);
        }
        if (*lphbrBk) {
            DeleteObject(*lphbrBk);
        }

        /* ----- Create Color Brush for Paint Indicating Key ----- */

        *lphbrText = CreateSolidBrush(dwTextColor);
        *lphbrBk   = CreateSolidBrush(dwBkColor);

        /* ----- Determine Whether the Key is Indicating or Not ----- */

        if (fIndicateFlag == TRUE) {

            /* ----- Exchange Text Color for Back Color ----- */

            SetTextColor((HDC)lParam, dwBkColor);
            SetBkColor((HDC)lParam, dwTextColor);
            hbrTemp = *lphbrText;

        } else {

            /* ----- Restore Text Color and Back Color ----- */

            SetTextColor((HDC)lParam, dwTextColor);
            SetBkColor((HDC)lParam, dwBkColor);
            hbrTemp = *lphbrBk;
            hIndKey = NULL;
        }

    /* ----- Determine Whether the Moving Key or Not ----- */

    } else if (LOWORD(wParam) == hMoveKey) {

        /* ----- Get Current System Color ----- */

        dwTextColor = GetSysColor(COLOR_WINDOWTEXT);
        dwBkColor   = GetSysColor(COLOR_WINDOW);

        /* ----- Delete Current Using Brush Handle ----- */

        if (*lphbrText) {
            DeleteObject(*lphbrText);
        }
        if (*lphbrBk) {
            DeleteObject(*lphbrBk);
        }

        /* ----- Create Color Brush for Paint Moving Key ----- */

        *lphbrText = CreateSolidBrush(dwTextColor);
        *lphbrBk   = CreateSolidBrush(dwBkColor);

        /* ----- Determine Whether the Key is Moving or Not ----- */

        if (fCapture == TRUE) {

            /* ----- Exchange Text Color for Back Color ----- */

            SetTextColor((HDC)lParam, dwBkColor);
            SetBkColor((HDC)lParam, dwTextColor);
            hbrTemp = *lphbrText;

        } else {

            /* ----- Restore Text Color and Back Color ----- */

            SetTextColor((HDC)lParam, dwTextColor);
            SetBkColor((HDC)lParam, dwBkColor);
            hbrTemp = *lphbrBk;
            hMoveKey = NULL;
        }

    } else if (HIWORD(wParam) == CTLCOLOR_STATIC) {

        if (GetDlgCtrlID((HWND)(LOWORD(wParam))) == P03_FIX) {

            /* ----- Get Current System Color ----- */

            dwTextColor = GetSysColor(COLOR_BTNTEXT);
            dwBkColor   = GetSysColor(COLOR_BTNFACE);

            /* ----- Delete Current Using Brush Handle ----- */

            if (*lphbrText) {
                DeleteObject(*lphbrText);
            }
            if (*lphbrBk) {
                DeleteObject(*lphbrBk);
            }

            /* ----- Create Color Brush for Paint Fixed Key ----- */

            *lphbrText = CreateSolidBrush(dwTextColor);
            *lphbrBk   = CreateSolidBrush(dwBkColor);

            /* ----- Change Button Face Color / Button Text Color ----- */

            SetTextColor((HDC)lParam, dwTextColor);
            SetBkColor((HDC)lParam, dwBkColor);
            hbrTemp = *lphbrBk;

        }
    }
    return (hbrTemp);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    P03ExecAbort()
*
**  Input       :   HWND        hDlg        -   Handle of DialogBox
*                   BOOL        fType       -   End Code
*                   LPHBRUSH    lphbrText   -   Address of Text Color Brush
*                   LPHBRUSH    lphbrBk     -   Address of Back Color Brush
*                   LPHCURSOR   lphCursor   -   Address of Handle of Cursor
*
**  Return      :   None.
*
**  Description :
*       This procedure destroys 2 dialogboxes and clears 2 dialog handles.
*   And delete brush handle.
* ===========================================================================
*/
VOID    P03ExecAbort(HWND hDlg,
                     BOOL fType,
                     LPHBRUSH lphbrText,
                     LPHBRUSH lphbrBk,
                     LPHCURSOR lphCursor)
{
    WCHAR   szCap[P03_CAP_LEN],
            szMsg[P03_ERR_LEN];

    /* ----- Determine Whether the End Code is Normal or Memory Error ----- */

    if (fType == P03_NORMAL_END) {

        /* ----- Release Allocated Area ----- */

        GlobalUnlock(hMemData);
        GlobalFree(hMemData);

        GlobalUnlock(hMemTbl);
        GlobalFree(hMemTbl);

        GlobalUnlock(hPLUTbl);
        GlobalFree(hPLUTbl);

    } else {                        /* fType == P03_MEM_ERROR */

        /* ----- Load String from Resource ----- */

        LoadString(hResourceDll, IDS_PEP_CAPTION_P03, szCap, PEP_STRING_LEN_MAC(szCap));

        LoadString(hResourceDll, IDS_PEP_ALLOC_ERR, szMsg, PEP_STRING_LEN_MAC(szMsg));

        /* ----- Display MessageBox ----- */

        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg,
                   szMsg,
                   szCap,
                   MB_OK | MB_ICONEXCLAMATION);

    }

    /* ----- Delete Created Brush for Paint DialogBox ----- */

    DeleteObject(*lphbrText);
    DeleteObject(*lphbrBk);

    /* ----- Destroy Dragging Cursor ----- */

    DestroyCursor(*lphCursor);

    /* ----- Destroy DialogBoxes (Keyboard & Tool Box) ----- */

    DestroyWindow(hDlg);
    DestroyWindow(*lphProgDlgChk2);

    /* ----- Clear Saving Area of DialogBox Handle ----- */

    *lphProgDlgChk1 = *lphProgDlgChk2 = 0;

    /* ----- Set Popup Menu to Enable ----- */

    SendMessage(hwndProgMain, PM_PEPMENU, (WPARAM)PEP_MENU_ALL_ON, 0L);

}


/*
* ===========================================================================
**  Synopsis    :   VOID    P03ConvPluKey()
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
VOID    P03ConvPluKey(HWND hDlg, LPP03TBLDATA lpTbl)
{

    LABELANALYSIS Label;                     /* Saratoga */

    USHORT  usRet;
    UINT    unPluKey;
    WORD    wPage, wCode;
    BOOL    fConv = 0, fRet = IDYES;
    WCHAR   szCap[PEP_CAPTION_LEN],
            szErr[P03_DSC_LEN];

    WCHAR   szPluNo[PLU_MAX_DIGIT + 1];      /* Saratoga */
    BOOL    bWork = FALSE;                   /* Saratoga */
    USHORT  usI;                             /* Saratoga */
    BOOL    bNumFlg = TRUE;                  /* Saratoga */
    short   sStat = 0;                       /* Saratoga */

    /* ----- Allocate Data Buffer from Global Heap ----- */

#ifdef __P03__
    hConvPLUTbl  = GlobalAlloc(GHND, MAX_PAGE_NO * FSC_PLU_MAX * sizeof(P03PLU));
#else
    hConvPLUTbl  = GlobalAlloc(GHND, (DWORD)(sizeof(USHORT) * MAX_PLUKEY_SIZE));
#endif
    if (! hConvPLUTbl) {    /* Failed Allocate Memory */

        /* ----- Load String from Resource ----- */

        LoadString(hResourceDll, IDS_PEP_CAPTION_P03, szCap, PEP_STRING_LEN_MAC(szCap));

        LoadString(hResourceDll, IDS_PEP_ALLOC_ERR, szErr, PEP_STRING_LEN_MAC(szErr));

        /* ----- Display MessageBox ----- */

        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg,
                   szErr,
                   szCap,
                   MB_OK | MB_ICONEXCLAMATION);

        return;

    } else {                /* Success Allocate Memory */

        lpConvMenuPLU = (LPP03DATA2)GlobalLock(hConvPLUTbl);

        /* ----- Read Initial AC 4 Data ----- */

        ParaAllRead(CLASS_PARAPLUNOMENU,
                    (UCHAR *)lpConvMenuPLU,
                    MAX_PAGE_NO * FSC_PLU_MICROMAX/*FSC_PLU_MAX*/ * sizeof(P03PLU),
                    0,
                    &usRet);
    }

    for (wPage = 0; wPage < MAX_PAGE_NO; wPage++) {
        for (wCode = 0; wCode < FSC_PLU_MICROMAX; wCode++) {

            /* ----- Check Keyed PLU Key for current selected page ----- */

            if (!fConv) {


                memset(szPluNo, 0x00, sizeof(szPluNo));

//  RJC - TCHAR and UCHAR may not be same size.
                memcpy/*_uchar2tchar*/(szPluNo,
                         lpConvMenuPLU->PluKey[wPage][wCode].szPluNo,
                         sizeof(szPluNo));

                for (usI = 0; usI < PLU_MAX_DIGIT; usI++) {
                     if((szPluNo[usI] == 0x00) ||
                        (szPluNo[usI] == 0x20)){
                     } else {
                        bWork = TRUE;
                     }
                }
                /* plu key is not arranged */
                if ((((UINT)lpTbl->FSC[wPage].TblFsc[wCode].uchFsc == P03_KEYEDPLU) &&
                     ((UINT)lpTbl->FSC[wPage].TblFsc[wCode].uchExt != (UINT)(wCode+1)))

                /* plu no is not assigned on plu key */
                 || ((bWork == FALSE) &&
                     ((UINT)lpTbl->FSC[wPage].TblFsc[wCode].uchFsc == P03_KEYEDPLU))) {

                    MessageBeep(MB_ICONQUESTION);

                    /* ----- Load Caution Message from Resource ----- */

                    LoadString(hResourceDll, IDS_PEP_CAPTION_P03, szCap, PEP_STRING_LEN_MAC(szCap));
                    LoadString(hResourceDll, IDS_P03_CONVPLUKEY,  szErr, PEP_STRING_LEN_MAC(szErr));

                    /* ----- Display Caution Message ----- */

                    fRet = MessageBoxPopUp(hDlg, szErr, szCap,
                               MB_YESNO | MB_ICONQUESTION);

                    if (fRet == IDNO){
                        break;
                    } else {
                        fConv = 1;
                    }
                }

            }
            if ((UINT)lpTbl->FSC[wPage].TblFsc[wCode].uchFsc == P03_KEYEDPLU) {

                /* relocate plu no. */
                unPluKey = (UINT)lpTbl->FSC[wPage].TblFsc[wCode].uchExt;
                if (unPluKey) unPluKey--;

                memcpy/*_uchar2tchar*/ (szPluNo,
                         lpConvMenuPLU->PluKey[wPage][wCode].szPluNo,
                         sizeof(szPluNo));

                /* ----- Numeric check, Saratoga ----- */

                for (usI = 0; usI < PLU_MAX_DIGIT; usI++) {

                  if (((szPluNo[usI] >= WIDE('0')) && (szPluNo[usI] <= WIDE('9'))) ||
                      (szPluNo[usI] == 0x20) || (szPluNo[usI] == 0x00)) {

                    continue;

                  } else {
                    bNumFlg = FALSE;
                  }

                }

                if (bNumFlg != FALSE) {   /* ----- PLU number is numeric ----- */

                  /* ----- PLU number C/D ----- */

                  memset(&Label, 0x00, sizeof(Label));

                  Label.fchModifier = lpConvMenuPLU->PluKey[wPage][wCode].uchModStat;
                  memcpy/*char2uchar*/(Label.aszScanPlu, szPluNo, sizeof(szPluNo));

                  /* ----- Analyze PLU Number ----- */

                  sStat = RflLabelAnalysis(&Label);

                  if (sStat != LABEL_ERR) {

                    memcpy/*char2uchar*/ (lpMenuPLU->PluKey[wPage][wCode].szPluNo,
                             szPluNo,
                             sizeof(szPluNo));

                    lpMenuPLU->PluKey[wPage][wCode].uchModStat =
                             lpConvMenuPLU->PluKey[wPage][wCode].uchModStat;

                  } else {

                    memset(szPluNo, 0, sizeof(szPluNo));

                    memcpy/*_tchar2uchar*/ (lpMenuPLU->PluKey[wPage][wCode].szPluNo,
                             szPluNo,
                             sizeof(szPluNo));

                    lpMenuPLU->PluKey[wPage][wCode].uchModStat = LA_NORMAL;
                  }

                } else {                  /* ----- PLU number is not numeric ----- */

                   memset(szPluNo, 0, sizeof(szPluNo));

                   memcpy/*_tchar2uchar*/ (lpMenuPLU->PluKey[wPage][wCode].szPluNo,
                            szPluNo,
                            sizeof(szPluNo));

                   lpMenuPLU->PluKey[wPage][wCode].uchModStat = LA_NORMAL;
                }
                lpTbl->FSC[wPage].TblFsc[wCode].uchExt = (UCHAR)(wCode+1);

            } else {

                /* clear plu no. */

                for(usI = 0; usI < PLU_MAX_DIGIT; usI++) {

                    lpMenuPLU->PluKey[wPage][wCode].szPluNo[usI] = '0';
                }
                lpMenuPLU->PluKey[wPage][wCode].szPluNo[PLU_MAX_DIGIT] = 0x00;

                lpMenuPLU->PluKey[wPage][wCode].uchModStat =
                       lpConvMenuPLU->PluKey[wPage][wCode].uchModStat;
    
            }
        }
        if (fRet == IDNO){
            break;
        }
    }

    if (fConv) fMenuPLU = 1;

    /* ----- Release Allocated Area ----- */

    GlobalUnlock(hConvPLUTbl);
    GlobalFree(hConvPLUTbl);

}

//Display Strings properly Multilingual PEP
/*VOID InitP03DlgStrings(HWND hDlg)
{
	WCHAR	szDesc[128];

	LoadString(hResourceDll, IDS_P03_RCLICK, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDC_P03_RCLICK, szDesc);
	
	LoadString(hResourceDll, IDS_PEP_OK, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDOK, szDesc);

	LoadString(hResourceDll, IDS_PEP_CANCEL, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDCANCEL, szDesc);

	LoadString(hResourceDll, IDS_PEP_TOOL, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_P03_TOOL, szDesc);

	LoadString(hResourceDll, IDS_PEP_CLEAR, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_P03_CLEAR, szDesc);

}
*/
//4-22-03 RPH Multilingual
/*VOID InitToolboxDlgStrings(HWND hDlg)
{
	WCHAR	szDesc[128];
	int		i, j;
	
	for(i=IDS_P03_NOFUNC, j=IDD_P03_NOFUNC; j<=IDD_P03_WICMOD; i++,j++)
	{
		LoadString(hResourceDll, i, szDesc, PEP_STRING_LEN_MAC(szDesc));
		DlgItemRedrawText(hDlg, j, szDesc);
	}

}
*/
/* ===== End of P003.C ===== */


