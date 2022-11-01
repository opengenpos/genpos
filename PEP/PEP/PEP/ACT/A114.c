/*
* ---------------------------------------------------------------------------
* Title         :   Department Maintenance (AC 114)
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A114.C
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
* Mar-14-95 : 03.00.00 : H.Ishida   : Add Print Priorty
* Sep-18-98 : 03.03.00 : A.Mitsui   : V3.3
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
#include    <csop.h>

#include    "action.h"
#include    "pepflag.h"
#include    "pepcomm.h"

#include    "file.h"    /* NCR2172 */
#include    "a114.h"

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
static  LPA114DESC   lpCtrlDesc;    /* Address of Control Code Description  */
static  LPA114CANADA lpCanDesc;     /* Address of Canada Tax Description    */
static  UINT         unDeptIndex;   /* Current Selected Index of Department */
static  BYTE         bTaxMode,      /* Current Tax Mode (US / Canada / VAT) */
                     abCtrlCode[OP_DEPT_CONT_CODE_SIZE];
                                    /* Control Code Work Area               */

/* NCR2172 */
static  DEPTIF       tempDept;      /* Temporary Dept Data Structure             */
static  UINT         unDeptMax;     /* Max. Record No. of Department (Prog. #2) */
static  UINT         unCurDept;     /* Current Max. No. of Department Records   */

static  HWND  hWndModeLess = NULL;


/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/


static BOOL A114HandleCmdMsg (HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{

    switch (LOWORD(wParam)) {
    case IDD_A114DEPT_LIST:                           // handle LBN_SELCHANGE message
        if (HIWORD(wParam) == LBN_SELCHANGE) {
            /* ----- Set Selected Dept. Data to Each Control ----- */
            A114SetData(hDlg);

            /* ----- Enable / Disable PushButton ----- */
            A114CtrlButton(hDlg);
            return TRUE;
        }
        return FALSE;

    case IDD_A114DEPT:                           // handle EN_CHANGE message
        if (HIWORD(wParam) == EN_CHANGE) {
            /* ----- Check Inputed Data with Data Range ----- */
            A114ChkRng(hDlg, LOWORD(wParam), A114_DEPTMAX);

            /* ----- Enable / Disable PushButton ----- */
            A114CtrlButton(hDlg);
            return TRUE;
        }
        return FALSE;

    case IDD_A114HALO:                           // handle EN_CHANGE message
        if (HIWORD(wParam) == EN_CHANGE) {
            /* ----- Check Inputed Data with Data Range ----- */
            A114ChkRng(hDlg, LOWORD(wParam), A114_HALOMAX);
            return TRUE;
        }
        return FALSE;

    case IDD_A114BONUS:                           // handle EN_CHANGE message
        if (HIWORD(wParam) == EN_CHANGE) {
            /* ----- Check Inputed Data with Data Range ----- */
            A114ChkRng(hDlg, LOWORD(wParam), A114_BONUSMAX);
            return TRUE;
        }
        return FALSE;

    case IDD_A114PRTPRY:                           // handle EN_CHANGE message
        if (HIWORD(wParam) == EN_CHANGE) {
            /* ----- Check Inputed Data with Data Range ----- */
            A114ChkRng(hDlg, LOWORD(wParam), A114_PRTPRYMAX);
            return TRUE;
        }
        return FALSE;

    case IDD_A114TARE:                           // handle EN_CHANGE message
        if (HIWORD(wParam) == EN_CHANGE) {
            /* ----- Check Inputed Data with Data Range ----- */
            A114ChkRng(hDlg, LOWORD(wParam), A114_TAREMAX);
            return TRUE;
        }
        return FALSE;

    case IDD_SET:
        if (HIWORD(wParam) == BN_CLICKED) {
            /* ----- Set DialogBox ID by Current Tax Mode ----- */
			WORD    wDlgID = (WORD)((bTaxMode == A114_TAX_US)  ? IDD_A114_CTRL :
                            (bTaxMode == A114_TAX_CAN) ? IDD_A114_CTRL_CAN  : IDD_A114_CTRL_VAT);

            /* ----- Create control code dialogbox ----- */
            DialogBoxPopup(hResourceDll, MAKEINTRESOURCEW(wDlgID), hDlg, A114CtrlDlgProc);
        
            /* ----- Set control code listbox content ----- */
            A114SetCtrlList(hDlg, A114_LBENTER);
            return TRUE;
        }
        return FALSE;

    case IDD_ENTER:
    case IDD_A114_DEL:
        if (HIWORD(wParam) == BN_CLICKED) {
			UINT    unHalo, unPrtpry, unBonus;
			UINT    unDept;      /* NCR2172 */
			BOOL    fHalo, fPrtpry, fBonus;
			BOOL    fDept;       /* NCR2172 */

			/* ----- Check Minus Data Input ----- */
            unHalo  = GetDlgItemInt(hDlg, IDD_A114HALO, (LPBOOL)&fHalo, FALSE);
            unBonus = GetDlgItemInt(hDlg, IDD_A114BONUS, (LPBOOL)&fBonus, FALSE);
            unPrtpry = GetDlgItemInt(hDlg, IDD_A114PRTPRY, (LPBOOL)&fPrtpry, FALSE);
            unDept   = GetDlgItemInt(hDlg, IDD_A114DEPT, (LPBOOL)&fDept, FALSE);         
            if ((fHalo == 0) || (fBonus == 0) || (fDept == 0) ||(fPrtpry == 0)) {
				WORD    wID, wMax;
				WCHAR   szCap[128], szWork[128], szDesc[128];

                /* ---- Load Error Message from Resource ----- */
                LoadString(hResourceDll, IDS_PEP_TITLE, szCap, PEP_STRING_LEN_MAC(szCap));
                LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_STRING_LEN_MAC(szWork));

                wMax = (WORD)((fHalo == 0) ? A114_HALOMAX :
                              (fPrtpry == 0) ? A114_PRTPRYMAX :
                              (fDept == 0) ? A114_DEPTMAX: A114_BONUSMAX);

                wsPepf(szDesc, szWork, A114_DATAMIN, wMax);

                /* ----- Display Error Message ----- */
                MessageBeep(MB_ICONEXCLAMATION);
                MessageBoxPopUp(hDlg, szDesc, szCap, MB_OK | MB_ICONEXCLAMATION);

                /* ----- Set Input Focus to EditText ----- */
                wID = (WORD)((fHalo == 0) ? IDD_A114HALO :
                             (fPrtpry == 0) ? IDD_A114PRTPRY :
                             (fDept == 0) ? IDD_A114DEPT : IDD_A114BONUS);

                SendDlgItemMessage(hDlg, wID, EM_SETSEL, TRUE, MAKELONG(0, -1));
                SetFocus(GetDlgItem(hDlg, wID));
                return TRUE;
            }

            /* ----- Save Current Dept Data to Temporary Area ----- */
            A114SaveRec(hDlg, IDD_A114, &tempDept);

            /* ----- Add/Change Inputed Data to Dept File ----- */
            if (LOWORD(wParam) == IDD_A114_DEL) {
                if (! A114DeleteData()) {
                   /* ----- Refresh Dept List Box ----- */
                   A114RefreshList(hDlg, wParam);
                }
            } else {
                if(! A114RegistData(hDlg, &tempDept)) {
                   /* ----- Refresh Dept List Box ----- */
                   A114RefreshList(hDlg, wParam);
                }
            }
        }

        A114CtrlButton(hDlg);

        SetFocus(GetDlgItem(hDlg, IDD_A114DEPT_LIST));
        return FALSE;

	default:
		break;
    }

    return FALSE;
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  A114DlgProc()
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
*                       Department Maintenance (AC 114)
* ===========================================================================
*/
BOOL    WINAPI  A114DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  PEPSPIN PepSpin;    /* Configulation of SpinButton          */
/*    static  UINT    unMaxDept;  / Maximum Number of Dapartment Data    */
/*    static  HGLOBAL hDeptMem, */  /* Handle of Global Heap (Dept. Data)   */
    static    HGLOBAL hCtrlDesc,  /* Handle of Global Heap (Description)  */
                      hCanDesc;   /* Handle of Global Heap (Canada Tax)   */

    switch (wMsg) {
    case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- Read No. of Dept Memory from Prog. #2 ----- */
        A114ReadMax(hDlg, &unDeptMax);
        if(! unDeptMax)
        {
			//Return Cancel to indicate failure
            EndDialog(hDlg, LOWORD(IDCANCEL));
        } else {
            /* ----- Allocate Global Heap ----- */
            if (A114AllocMem(hDlg, &hCtrlDesc, &hCanDesc)) {
				//Return Cancel to indicate failure
                EndDialog(hDlg, LOWORD(IDCANCEL));
            } else {
                /* ----- Initialize Configulation of DialogBox ----- */
                A114InitDlg(hDlg);

                /* ----- Initialize Configulation of SpinButton ----- */
                PepSpin.nStep      = A114_SPIN_STEP;
                PepSpin.nTurbo     = A114_SPIN_TURBO;
                PepSpin.fSpinStyle = PEP_SPIN_NOROLL;
            }
        }
        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j=IDD_A114_CTRL; j<=IDD_A114_PRTPRY_RNG; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDD_A114_CUR, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_ENTER, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_A114_DEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_SET, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_VSCROLL:
		{
			/* ----- Set Target EditText ID ----- */
			int idEdit = (int)(GetDlgCtrlID((HWND)lParam) - A114_SPIN_OFFSET);

			/* ----- Set Maximum Value of Data Range by Selected Button ----- */
			switch (idEdit) {
				case IDD_A114HALO:               // handle WM_VSCROLL message, min and max range
					PepSpin.lMin = A114_DATAMIN;
					PepSpin.lMax = A114_HALOMAX;
					break;
				case IDD_A114PRTPRY:             // handle WM_VSCROLL message, min and max range
					PepSpin.lMin = A114_DATAMIN;
					PepSpin.lMax = A114_PRTPRYMAX;
					break;
				case IDD_A114DEPT:               // handle WM_VSCROLL message, min and max range
					PepSpin.lMin = A114_DEPTMIN;   
					PepSpin.lMax = A114_DEPTMAX;
					break;
				case IDD_A114TARE:               // handle WM_VSCROLL message, min and max range
					PepSpin.lMin = A114_TAREMIN;   
					PepSpin.lMax = A114_TAREMAX;
					break;

				default:
					PepSpin.lMin = A114_DATAMIN;
					PepSpin.lMax = A114_BONUSMAX;
					break;
			}

			if (PepSpinProc(hDlg, wParam, idEdit, (LPPEPSPIN)&PepSpin)) {
				BOOL bPMStatus;
				bPMStatus = PostMessage(hDlg, WM_COMMAND, MAKELONG(LOWORD(idEdit), EN_CHANGE), 0L);
			}
		}
        return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
			case IDOK:
			case IDCANCEL:
				if (HIWORD(wParam) == BN_CLICKED) {
					HCURSOR hCursor;

					/* ----- Change cursor (arrow -> hour-glass) ----- */
					hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
					ShowCursor(TRUE);

					/* ----- Finalize DialogBox ----- */
					A114FinDlg(hDlg, wParam);

					/* ----- Frees Allocated Memory ----- */
					A114FreeMem(&hCtrlDesc, &hCanDesc);

					/* ----- Restore cursor (hour-glass -> arrow) ----- */
					ShowCursor(FALSE);
					SetCursor(hCursor);

					hWndModeLess = NULL;

					EndDialog(hDlg, LOWORD(wParam));
					return TRUE;
				}
				break;

			default:
				return A114HandleCmdMsg (hDlg, wMsg, wParam, lParam);
        }
        return FALSE;

	case WM_DESTROY:
		hWndModeLess = NULL;
        return FALSE;

    default:
        return FALSE;
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A068RefreshList()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wParam  -   User Selected Button ID
*                   UINT    unType  -   PLU Type of Current Selected PLU
*
**  Return      :   No return value.
*
**  Description :
*       This procedure redraws a PLU listbox.
* ===========================================================================
*/
VOID    A114RefreshList(HWND hDlg, WPARAM wParam)
{
    BOOL    fCheck = FALSE;
    DWORD   dwIndex;
    WCHAR   szBuff[48], szWork[48];            /* Saratoga */

    /* ----- Reform Description for Insert ListBox ----- */
	memset(&szWork, 0x00, sizeof(szWork));
    PepReplaceMnemonic(tempDept.ParaDept.auchMnemonic, szWork, (A114_MNEMO_SIZE ), PEP_MNEMO_READ);

    wsPepf(szBuff, WIDE("%04u : %s"), tempDept.usDeptNo, szWork);

    /* ----- Create String for Search Dept No. in ListBox ----- */
    wsPepf(szWork, WIDE("%04u "), tempDept.usDeptNo);

    /* ----- Search for Corresponding Dept in Dept ListBox ----- */
    dwIndex = DlgItemSendTextMessage(hDlg, IDD_A114DEPT_LIST, LB_FINDSTRING, (WPARAM)-1, (LPARAM)(szWork));

    /* ----- Determine Whether Target Dept Data Exist or Not ----- */
    if (dwIndex == LB_ERR) {    /* Not Found Target PLU No. */
        /* ----- Insert New Data into Dept ListBox ----- */
        DlgItemSendTextMessage(hDlg, IDD_A114DEPT_LIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szBuff));

        /* ----- Get No. of Current Records ----- */
        dwIndex = SendDlgItemMessage(hDlg, IDD_A114DEPT_LIST, LB_GETCOUNT, 0, 0L);

        /* ----- Adjust Cursor Position ----- */
        -- dwIndex;

        /* ----- No. of PLU Records is Changed (One Record Increment) ----- */
        fCheck = TRUE;
    } else {                    /* Found in Dept Records */
        if (LOWORD(wParam) == IDD_A114_DEL) {     /* Delete Button is Pressed */
            /* ----- Delete a Target String from PLU ListBox ----- */
            SendDlgItemMessage(hDlg, IDD_A114DEPT_LIST, LB_DELETESTRING, (WPARAM)dwIndex, 0L);

            if (dwIndex) {                  /* Remain Dept Records */
                /* ----- Adjust Cursor Position ----- */
                -- dwIndex;
            } else {                        /* Delete All Dept Record */
                /* ----- Clear Temporary Dept Data Area ----- */
                memset(&tempDept, 0, sizeof(DEPTIF));
            }

            /* ----- No. of Dept Record is Changed (One Record Decrement) ----- */
            fCheck = TRUE;
        } else {                        /* Change Dept Record */
            /* ----- Delete Old String from PLU ListBox ----- */
            SendDlgItemMessage(hDlg, IDD_A114DEPT_LIST, LB_DELETESTRING, (WPARAM)dwIndex, 0L);

            /* ----- Insert New Description to Dept ListBox ----- */
            DlgItemSendTextMessage(hDlg, IDD_A114DEPT_LIST, LB_INSERTSTRING, (WPARAM)dwIndex, (LPARAM)(szBuff));

            /* ----- No. of Dept Record is Not Changed ----- */
            fCheck = FALSE;
        }
    }

    /* ----- Determine Whether No. of Dept Record is Changed or Not ----- */
    if (fCheck) {
        /* ----- Compute No. of Dept Records ----- */
        if (LOWORD(wParam) == IDD_A114_DEL) {     /* Delete Button is Pressed     */
            unCurDept -= 1;

            /* ----- Set No. of Current PLU Records ----- */
            LoadString(hResourceDll, IDS_A114_CUR, szWork, PEP_STRING_LEN_MAC(szWork));
            wsPepf(szBuff, szWork, unCurDept);
            DlgItemRedrawText(hDlg, IDD_A114_CUR, szBuff);
        }
    }

    /* ----- Set Cursor to Inserted String ----- */
    SendDlgItemMessage(hDlg, IDD_A114DEPT_LIST, LB_SETCURSEL, (WPARAM)dwIndex, 0L);

    if (LOWORD(wParam) == IDD_A114_DEL) {     /* Delete Button is Pressed */
        /* ----- Show Next PLU Records to Each Control ----- */
        PostMessage(hDlg, WM_COMMAND, IDD_A114DEPT_LIST, MAKELONG(0, LBN_SELCHANGE));
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A068FinDlg()
*
**  Input       :   HWND        hDlg    -   Window Handle of a DialogBox
*                   WORD        wParam  -   User Selected Button ID
*
**  Return      :   No return value.
*
**  Description :
*       This procedure saves inputed Dept data, and sets file status flag, if
*   user selects OK button. Otherwise it restores Dept file with backup file.
*   And then deletes backup file.
* ===========================================================================
*/
VOID    A114FinDlg(HWND hDlg, WPARAM wParam)
{
    if (LOWORD(wParam) == IDOK) {
        /* ----- Set Dept File Status Flag to ON ----- */
        PepSetModFlag(hwndActMain, PEP_MF_DEPT, 0);
    } else {
		WCHAR    szFile[PEP_FILE_BACKUP_LEN];

        /* ----- Load DEPT File Name from Resource ----- */
        LoadString(hResourceDll, IDS_FILE_NAME_DEPT, szFile, PEP_STRING_LEN_MAC(szFile));

        /* ----- Restore Dept File with Backup File ----- */
        PepDeptFileBackup(hDlg, szFile, PEP_FILE_RESTORE);
    }

    /* Control Code Work Area clear */
    memset(abCtrlCode, 0, sizeof(abCtrlCode));       /* Saratoga */

    /* ----- Delete Backup File ----- */
    PepDeptFileDelete();
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A114SaveRec()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wDlgID  -   Current Editting DialogBox ID
*
**  Return      :   No return value
*
**  Description :
*       This procedure gets inputed Dept Data from each controls, and saves them
*   to a static work area.
* ===========================================================================
*/
VOID    A114SaveRec(HWND hDlg, WPARAM wDlgID, DEPTIF *pDpi)
{
    WORD    wI;
    UINT    unDept, unHalo, unBonus, unPrtpry;
    BOOL    fHalo, fBonus, fPrtpry, fDept;
    BYTE    bBonus;
    WCHAR   szWork[PEP_OVER_LEN];

    /* ----- Determine Current Editting DialogBox ----- */
    if (LOWORD(wDlgID) == IDD_A114) {    /* Dept Main DialogBox */
        /* ----- Check Minus Data Input ----- */
        unHalo  = GetDlgItemInt(hDlg, IDD_A114HALO, (LPBOOL)&fHalo, FALSE);
        unBonus = GetDlgItemInt(hDlg, IDD_A114BONUS, (LPBOOL)&fBonus, FALSE);
        unPrtpry = GetDlgItemInt(hDlg, IDD_A114PRTPRY, (LPBOOL)&fPrtpry, FALSE);
        unDept   = GetDlgItemInt(hDlg, IDD_A114DEPT, (LPBOOL)&fDept, FALSE);         

        /* ----- Get Inputed Dept No. from Dept No. EditText ----- */
        pDpi->usDeptNo = (USHORT)unDept;

        /* ----- Get HALO Data from EditText ----- */
        pDpi->ParaDept.auchHalo[A114_HALO] = (UCHAR)unHalo;

        /* ----- Get Major Department Data from EditText ----- */
        pDpi->ParaDept.uchMdept = (UCHAR)SendDlgItemMessage(hDlg, IDD_A114MAJOR, CB_GETCURSEL, 0, 0L);

        /* ----- Get Bonus Total Index Data from EditText ----- */
        bBonus = (BYTE)GetDlgItemInt(hDlg, IDD_A114BONUS, NULL, FALSE);

        /* ----- Initialize Area of Bonus Total Index in Control Code ----- */
		pDpi->ParaDept.usBonusIndex &= ~0x00ff;

        /* ----- Set Inputed Bonus Total Index to Dept. Data ----- */
        pDpi->ParaDept.usBonusIndex |= bBonus;

        /* ----- Get Mnemonic Data from EditText ----- */
        DlgItemGetText(hDlg, IDD_A114MNEMO, szWork, sizeof(szWork));

        /* ----- Replace Double Width Key ('~' -> 0x12) ----- */
        PepReplaceMnemonic(szWork, pDpi->ParaDept.auchMnemonic, (PARA_DEPT_LEN ), PEP_MNEMO_WRITE);
		
		//tare information for weighted goods.  Now an item can
		//have a specified tare, and will automatically add the 
		//tare to the transaction.  In order to save space
		//we save the flag and the number in the same location
		//the flag is set on the high end of the ushort while the
		//other information, the tare number will be set in the
		//low end, this number will never go past 255, at least
		//while im alive. JHHJ
		unDept = GetDlgItemInt(hDlg, IDD_A114TARE, NULL, FALSE);
        pDpi->ParaDept.usTareInformation = (UCHAR)unDept;

		if(IsDlgButtonChecked(hDlg, IDD_A114_STSTARECCHECK))
		{
			pDpi->ParaDept.usTareInformation |= PLU_REQ_TARE;
		}

        /* ----- Get Print Priorty from EditText ----- */
        pDpi->ParaDept.uchPrintPriority = (UCHAR)unPrtpry;

       for (wI = 0; wI < OP_DEPT_CONT_CODE_SIZE; wI++) {
           /* ----- Set Control Code Data to Dept. Data Buffer ----- */
           pDpi->ParaDept.auchControlCode[wI] = abCtrlCode[wI];
       }

       /* ----- Set HighLight Bar to Current Inserted Description ----- */
       SendDlgItemMessage(hDlg, IDD_A114DEPT_LIST, LB_SETCURSEL, (WPARAM)unDeptIndex, 0L);
    }
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  A114CtrlDlgProc()
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
*               Department Maintenance Control Code (AC 114)
* ===========================================================================
*/
BOOL    WINAPI  A114CtrlDlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    UINT    unI;

    switch (wMsg) {
    case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));
        /* ----- Set data to control code dialogbox ----- */
        A114SetCtrlData(hDlg);
        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j = IDD_A114ST01; j <= IDD_A114_CUR; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			for(j = IDD_A114_AGETYPE; j <= IDD_A114_VAT; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDD_A114_TYPEKEY, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK:
        case IDCANCEL:
            if (HIWORD(wParam) == BN_CLICKED) {
                if (LOWORD(wParam) == IDOK) {
                    /* ----- Get Inputed Control Data to Buffer ----- */
                    A114GetCtrlData(hDlg);
                }

                for (unI = 0; unI < OP_DEPT_CONT_CODE_SIZE; unI++) {
                   /* ----- Set Control Code Data to Work Area ----- */
                   (BYTE)tempDept.ParaDept.auchControlCode[unI] = abCtrlCode[unI];
                }

                EndDialog(hDlg, LOWORD(wParam));

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
**  Synopsis    :   BOOL    A114ReadMax()
*
**  Input       :   HWND        hDlg    -   Window Handle of DialogBox
*                   LPUINT  lpunMaxDept -   Address of Max. Value of Dept. Data   
*
**  Return      :   BOOL    TRUE        -   Max. of Dept. Data is 0.
*                           FALSE       -   Max. of Dept. Data is valid.
*
**  Description :
*       This function gets the maximum number of department data. It returns
*   TRUE, if the maximum number of department records is invalid(0).
*   Otherwise it returns FALSE.
* ===========================================================================
*/
BOOL    A114ReadMax(HWND hDlg, LPUINT lpunMaxDept)
{
    BOOL        fRet = FALSE;
    PARAFLEXMEM FlexData;
    WCHAR       szCap[PEP_CAPTION_LEN], szMsg[256];

    /* ----- Get Max. No. of Department Records from Prog.#2 ----- */
    /* ----- Set Maximum Record No. of Department to Data Area ----- */
    FlexData.uchAddress = FLEX_DEPT_ADR;
    ParaFlexMemRead(&FlexData);
    *lpunMaxDept = (UINT)FlexData.ulRecordNumber;

    /* ----- Determine Whether Max. of Dept. Records is valid or Not ----- */
    if (! FlexData.ulRecordNumber) {
        LoadString(hResourceDll, IDS_PEP_CAPTION_A114, szCap, PEP_STRING_LEN_MAC(szCap));
        LoadString(hResourceDll, IDS_A114_EMPTY, szMsg, PEP_STRING_LEN_MAC(szMsg));

        /* ----- Display Error Message ----- */
        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg, szMsg, szCap, MB_OK | MB_ICONEXCLAMATION);

        fRet = TRUE;
    }
    return (fRet);
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A114AllocMem()
*
**  Input       :   HWND        hDlg    -   Window Handle of DialogBox
*                   LPHGLOBAL   lphDesc -   Address of Global Heap Handle
*                   LPHGLOBAL   lphCan  -   Address of Global Heap Handle
*
**  Return      :   BOOL    TRUE    -   Memory Allocation is Failed.
*                           FALSE   -   Momory Allocation is Success.
*
**  Description :
*       This function allocates the data buffer from the global heap.
*   It returns TRUE, if it failed in momory allocation. Otherwise it returns 
*   FALSE.
* ===========================================================================
*/
BOOL
A114AllocMem(HWND hDlg, LPHGLOBAL lphDesc, LPHGLOBAL lphCan)
{
    BOOL    fRet = FALSE;
    WCHAR   szCaption[PEP_CAPTION_LEN], szErrMsg[PEP_ALLOC_LEN];

    /* ----- Allocate Memory from Global Heap ----- */
    *lphDesc = GlobalAlloc(GHND, (sizeof(A114DESC)   * A114_CTRLCODE_NO));
    *lphCan  = GlobalAlloc(GHND, (sizeof(A114CANADA) * A114_CANADA_MAX));

    /* ----- Determine Whether Memory Allocation is Success or Not ----- */
    if ((! *lphDesc) || (! *lphCan)) {
        /* ----- Load String from Resource ----- */
        LoadString(hResourceDll, IDS_PEP_CAPTION_A114, szCaption, PEP_STRING_LEN_MAC(szCaption));
        LoadString(hResourceDll, IDS_PEP_ALLOC_ERR,    szErrMsg,  PEP_STRING_LEN_MAC(szErrMsg));

        /* ----- Display Error Message ----- */
        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg, szErrMsg, szCaption, MB_OK | MB_ICONEXCLAMATION);

        fRet = TRUE;

        /* ----- Set the Allocated Area Free ----- */
        if (*lphDesc) {
            GlobalFree(*lphDesc);
        }

        if (*lphCan) {
            GlobalFree(*lphCan);
        }
    } else {
        /* ----- Lock the Allocated Area ----- */
        lpCtrlDesc = (LPA114DESC)GlobalLock(*lphDesc);
        lpCanDesc  = (LPA114CANADA)GlobalLock(*lphCan);
    }
    return fRet;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A068CtrlButton()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This procedure gets PLU No. from EditText, and then it controls
*   pushbuttons status. It enables pushbuttons, if PLU No. exists in edittext.
*   Otherwise it disables pushbuttons.
* ===========================================================================
*/
VOID    A114CtrlButton(HWND hDlg)
{
    UINT    unDeptNo;
    BOOL    fCheck, fDept;

    /* ----- Check Data Exist in Dept No. EditText ----- */
    unDeptNo = GetDlgItemInt(hDlg, IDD_A114DEPT, &fDept, FALSE);

    /* ----- Enabel / Disable PushButtons ----- */
    fCheck = (fDept == 0) ? FALSE : TRUE;
    EnableWindow(GetDlgItem(hDlg, IDD_SET), fCheck);
    EnableWindow(GetDlgItem(hDlg, IDD_ENTER), fCheck);
    EnableWindow(GetDlgItem(hDlg, IDD_A114_DEL), fCheck);

    if (fCheck == TRUE) {           /* Inputed Value is Valid   */
		DWORD   dwIndex;
		WCHAR   szDesc[16] = {0};

        /* ----- Create String for Search DEPT No. into ListBox ----- */
        wsPepf(szDesc, WIDE("%04u "),  unDeptNo);

		//SR 445 Fix.  Although the SR calls for AC 152, I discovered that most
		//dialog messages that use the LB_FINDSTRING, we have made a new function 
		//called DlgItemSendTextMessage which now passes the information through 
		//the unicode utilities to SendDlgItemMessage 
        dwIndex = DlgItemSendTextMessage(hDlg, IDD_A114DEPT_LIST, LB_FINDSTRING, (WPARAM)-1, (LPARAM)(szDesc));
        fCheck = (dwIndex == LB_ERR) ? FALSE : TRUE;
        EnableWindow(GetDlgItem(hDlg, IDD_A114_DEL), fCheck);
        if (! unDeptNo) {
            /* ----- Set Focus to Dept No. EditText ----- */
            SetFocus(GetDlgItem(hDlg, IDD_A114NO));
        }
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A114FreeMem()
*
**  Input       :   LPHGLOBAL   lphDesc -   Address of Global Heap Handle
*                   LPHGLOBAL   lphCan  -   Address of Global Heap Handle
*
**  Return      :   No return value.
*
**  Description :
*       This function frees the allocated memory from global heap.
* ===========================================================================
*/
VOID
A114FreeMem(LPHGLOBAL lphDesc, LPHGLOBAL lphCan)
{
    /* ----- Unlock the Allocated Area ----- */
    GlobalUnlock(*lphDesc);
    GlobalUnlock(*lphCan);

    /* ----- Free the Allocated Area ----- */
    GlobalFree(*lphDesc);
    GlobalFree(*lphCan);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A114InitCtrlDesc()
*
**  Input       :   No input value.
*
**  Return      :   No return value.
*
**  Description :
*       This function loads a control code description and sets it to the
*   allocated area.
* ===========================================================================
*/
VOID    A114InitCtrlDesc(VOID)
{
    UINT            wI, wOnStrID  = IDS_A114CTRL101, wOffStrID = IDS_A114CTRL001;
    LPA114DESC      lpDesc;
    LPA114CANADA    lpCanada;

    /* ----- Initialize Address of Control Code String Buffer ----- */
    lpDesc = lpCtrlDesc;

    /* ----- Load Control Code String from Resorce ----- */
    for (wI = 0; wI < A114_CTRLCODE_NO; wI++, wOnStrID++, wOffStrID++, lpDesc++) {
        LoadString(hResourceDll, wOnStrID,  lpDesc->szDescON,  A114_DESC_LEN);
        LoadString(hResourceDll, wOffStrID, lpDesc->szDescOFF, A114_DESC_LEN);
    }

    /* ----- Initialize Address of Canadian Tax String Buffer ----- */
    lpCanada = lpCanDesc;

    /* ----- Load Canadian Tax String from Resource ----- */
    for (wI = 0, wOnStrID = IDS_A114CAN_0; wI < A114_CANADA_MAX; wI++, lpCanada++) {
        LoadString(hResourceDll, wOnStrID++, lpCanada->szCanDesc, A114_DESC_LEN);
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A114SetData()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function sets current selected department data to each contols.
* ===========================================================================
*/
VOID    A114SetData(HWND hDlg)
{
    UINT        unI;
    WCHAR       szWork[64];
    WCHAR       szBuf[64];                 
    WCHAR       szDeptNo[64];
    UINT        unDeptNo;

    /* ----- Get Current Selected Index of ListBox ----- */
    memset(szBuf, 0x00, sizeof(szBuf));
    unDeptIndex = (UINT)SendDlgItemMessage(hDlg, IDD_A114DEPT_LIST, LB_GETCURSEL, 0, 0L);
    DlgItemSendTextMessage(hDlg, IDD_A114DEPT_LIST, LB_GETTEXT, (WORD)unDeptIndex, (LPARAM)(szBuf));

    /* ----- Set Address of Current Selected Dept. Data to Work Area ----- */
    memset(szDeptNo, 0x00, sizeof(szDeptNo));
    memcpy(szDeptNo, szBuf, A114_DEPTLEN * sizeof(WCHAR));
    unDeptNo = (UINT)_wtoi(szDeptNo);
    tempDept.usDeptNo = (USHORT)unDeptNo;
	tempDept.ParaDept.usTareInformation = 0;
    OpDeptIndRead(&tempDept, 0);

	SendMessage(GetDlgItem(hDlg, IDD_A114_STSTARECCHECK), BM_SETCHECK, BST_UNCHECKED, 0); 

    /* ----- Set Dept Data to EditText ----- */
    SetDlgItemInt(hDlg, IDD_A114DEPT, (UINT)tempDept.usDeptNo, FALSE);

    /* ----- Set HALO Data to EditText ----- */
    SetDlgItemInt(hDlg, IDD_A114HALO, (UINT)tempDept.ParaDept.auchHalo[A114_HALO], FALSE);

    /* ----- Set Major Department Number ----- */
    SendDlgItemMessage(hDlg, IDD_A114MAJOR, CB_SETCURSEL, (WORD)tempDept.ParaDept.uchMdept, FALSE);

    /* ----- Set Bonus Total Index to EditText ----- */
    SetDlgItemInt(hDlg, IDD_A114BONUS, tempDept.ParaDept.usBonusIndex, FALSE);

    /* ----- Replace Double Width Key (0x12 -> '~') ----- */
	//tare information for weighted goods.  Now an item can
	//have a specified tare, and will automatically add the 
	//tare to the transaction.  In order to save space
	//we save the flag and the number in the same location
	//the flag is set on the high end of the ushort while the
	//other information, the tare number will be set in the
	//low end, this number will never go past 255, at least
	//while im alive. JHHJ
	unDeptNo = (UINT)tempDept.ParaDept.usTareInformation;
	SetDlgItemInt(hDlg,IDD_A114TARE, (unDeptNo & PLU_REQ_TARE_MASK),FALSE);
	if(tempDept.ParaDept.usTareInformation & PLU_REQ_TARE)
	{
		SendMessage(GetDlgItem(hDlg,IDD_A114_STSTARECCHECK), BM_SETCHECK, BST_CHECKED, 0); 
	}
	memset(szWork, 0, sizeof(szWork));
    PepReplaceMnemonic(tempDept.ParaDept.auchMnemonic, szWork, (PARA_DEPT_LEN ), PEP_MNEMO_READ);

    /* ----- Set Department Mnemonic to EditText ----- */
    DlgItemRedrawText(hDlg, IDD_A114MNEMO, szWork);

    /* ----- Set PrintPriorty to EditText ----- */
    SetDlgItemInt(hDlg, IDD_A114PRTPRY, (UINT)tempDept.ParaDept.uchPrintPriority, FALSE);

    /* ----- Set Contents of Control Code ListBox ----- */
    A114SetCtrlList(hDlg, A114_LBSET);

    /* ----- Set Control Code Data to Work Area ----- */
    for (unI = 0; unI < OP_DEPT_CONT_CODE_SIZE; unI++) {
        abCtrlCode[unI] = (BYTE)tempDept.ParaDept.auchControlCode[unI];
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A114SetCtrlList()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   BOOL    fMode   -   Set / Enter Mode Flag
*
**  Return      :   No return value.
*
**  Description :
*       This function sets the description of control codes to control code
*   listbox by current selected department's control code data.
* ===========================================================================
*/
VOID    A114SetCtrlList(HWND hDlg, BOOL fMode)
{
    WCHAR   szWork[128];
    UINT    wI;
    BYTE    bCtrlData;

    /* ----- Reset Contents of Control Code ListBox ----- */
    SendDlgItemMessage(hDlg, IDD_A114CTRL_LIST, LB_RESETCONTENT, 0, 0L);

    /* ----- Set Content for Address 2 & 3 ----- */
    A114SetListContent(hDlg, A114_CHKBIT2, A114_CTRLCODE1, A114_DSCRB1, fMode);

    /* ----- Determine Current Tax Mode  ----- */
    if (bTaxMode == A114_TAX_CAN) {
        /* ----- Set Content for Address 4 & 5 (Canadian Tax) ----- */
        A114SetCanadaDesc(hDlg, fMode);
    } else if (bTaxMode == A114_TAX_US) {
        /* ----- Set Content for Address 4 & 5 (U.S. Tax) ----- */
        A114SetListContent(hDlg, A114_CHKBIT3, A114_CTRLCODE2, A114_DSCRB2, fMode);
    } else {
        /* ----- Set Content for Address 4 & 5 (International VAT) ----- */
        A114SetVATDesc(hDlg, fMode);
    }

    /* ----- Set listbox content for address 6 & 7 ----- */
    A114SetListContent(hDlg, A114_CHKBIT4, A114_CTRLCODE3, A114_DSCRB3, fMode);

    /* ----- Set listbox content for address 6 & 7 ----- */
    A114SetListContent(hDlg, A114_CHKBIT7, A114_CTRLCODE5, A114_DSCRB4, fMode);

    /* ----- Set listbox content for address 9 ----- */
    A114SetListContent(hDlg, A114_CHKBIT6, A114_CTRLCODE4, A114_DSCRB5, fMode);

    /* ----- Set listbox content for address 14 ,Saratoga----- */
    bCtrlData = (BYTE)((fMode == A114_LBENTER) ? abCtrlCode[A114_CTRLCODE5] : tempDept.ParaDept.auchControlCode[A114_CTRLCODE5]);

    wI = (IDS_A114_TYPE + ((WORD)((bCtrlData & (BYTE)A114_AGETYPE_BIT) >> 4)));
    LoadString(hResourceDll, wI, szWork, PEP_STRING_LEN_MAC(szWork));

    /* ----- Insert Loaded String to Control Code Listbox ----- */
    DlgItemSendTextMessage(hDlg, IDD_A114CTRL_LIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szWork));

    /* ### ADD Saratoga WIC (052900) */
    /* ----- WIC ---- for address 14 , Saratoga ----- */
    if(bCtrlData & A114_WIC_BIT) {
        DlgItemSendTextMessage(hDlg, IDD_A114CTRL_LIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)(WIDE("WIC item")));
	}
	else {
        DlgItemSendTextMessage(hDlg, IDD_A114CTRL_LIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)(WIDE("Non WIC item")));
	}
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A114SetListContent()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*                   BYTE    bChkBit     -   Bit Pattern without Reserved Area
*                   BYTE    bCtrlAddr   -   Data Address of Control Code
*                   WORD    wOffset     -   String Offset of Control Code
*                   BOOL    fMode       -   Set / Enter Mode Flag
*
**  Return      :   No return value.
*
**  Description :
*       This function sets the description of control code to control code
*   listbox by current selected department's control code data.
* ===========================================================================
*/
VOID
A114SetListContent(HWND hDlg, BYTE bChkBit, BYTE bCtrlAddr, WORD wOffset, BOOL fMode)
{
    BYTE     bI, bCtrlData;

    /* ----- Get Target Control Code to Work Area ----- */
    bCtrlData = (BYTE)((fMode == A114_LBENTER) ? abCtrlCode[bCtrlAddr] :
                        tempDept.ParaDept.auchControlCode[bCtrlAddr]);

    for(bI = 0; bI < A114_8BITS; bI++) { 
        /* ----- Determine Whether Current Bit is Reserved Area or Not ----- */
        if (bChkBit & (A114_BITMASK << bI)) {
			LPCWSTR  lpszDesc;

            /* ----- Check Current Bit is ON or OFF ----- */
            if (bCtrlData & (A114_BITMASK << bI)) {
                lpszDesc = (lpCtrlDesc + wOffset)->szDescON;
            } else {
                lpszDesc = (lpCtrlDesc + wOffset)->szDescOFF;
            }

            /* ----- Increment Descriptin Offset ----- */
            wOffset++;

            /* ----- Insert Description to Control Code ListBox ----- */
            DlgItemSendTextMessage(hDlg, IDD_A114CTRL_LIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)lpszDesc);
        }
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A114SetCanadaDesc()
*
**  Input       :   HWND        hDlg    -   Window Handle of a DialogBox
*                   LPDEPTPARA  lpDept  -   Address of Target Dept. Data
*                   BOOL        fMode   -   Set / Enter Mode Flag
*
**  Return      :   No return value.
*
**  Description :
*       This function sets the description of Canadian tax to control code
*   listbox by current selected department's control code data.
* ===========================================================================
*/
/* Saratoga
VOID    A114SetCanadaDesc(HWND hDlg, LPDEPTPARA lpDept, BOOL fMode)
*/
VOID    A114SetCanadaDesc(HWND hDlg, BOOL fMode)
{
    BYTE     bDisc, bCanTax, bWork;
    LPCWSTR  lpszDesc;

    /* ----- Get Control Code Data (Address 4 & 5) ----- */
    bWork = (BYTE)((fMode == A114_LBENTER) ? abCtrlCode[A114_CTRLCODE2] :
                                tempDept.ParaDept.auchControlCode[A114_CTRLCODE2]);

    /* ----- Get Current Canadian Tax Data ----- */
    bCanTax = PEP_HIMASK(bWork);

    /* ----- Adjust Canadian Tax Data ----- */
    bCanTax = (BYTE)((bCanTax > A114_TAX_MAX) ? A114_TAX_MAX : bCanTax);

    /* ----- Insert Description to Control Code ListBox ----- */
    DlgItemSendTextMessage(hDlg, IDD_A114CTRL_LIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)((lpCanDesc + bCanTax)->szCanDesc));

    /* ----- Get Current Discount Item #1 & #2 Data ----- */
    bDisc = PEP_LOMASK(bWork);

    /* ----- Set Address of Discount Item #1 String ----- */
    lpszDesc = ((bDisc & A114_DISC_ITEM1) ?
                        (lpCtrlDesc + A114_DSCITEM1)->szDescON :
                        (lpCtrlDesc + A114_DSCITEM1)->szDescOFF);
                
    /* ----- Insert Description to Control Code ListBox ----- */
    DlgItemSendTextMessage(hDlg, IDD_A114CTRL_LIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)lpszDesc);

    /* ----- Set Address of Discount Item #2 String ----- */
    lpszDesc = ((bDisc & A114_DISC_ITEM2) ?
                        (lpCtrlDesc + A114_DSCITEM2)->szDescON :
                        (lpCtrlDesc + A114_DSCITEM2)->szDescOFF);
                
    /* ----- Insert Description to Control Code ListBox ----- */
    DlgItemSendTextMessage(hDlg, IDD_A114CTRL_LIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)lpszDesc);
}


/*
* ===========================================================================
**  Synopsis    :   VOID    A114SetVATDesc()
*
**  Input       :   HWND        hDlg    -   Window Handle of a DialogBox
*                   LPDEPTPARA  lpDept  -   Address of Target Dept. Data
*                   BOOL        fMode   -   Set / Enter Mode Flag
*
**  Return      :   No return value.
*
**  Description :
*       This function sets the description of International VAT to control code
*   listbox by current selected department's control code data.
* ===========================================================================
*/
VOID    A114SetVATDesc(HWND hDlg, BOOL fMode)
{
    BYTE      bDisc, bVAT, bWork;
    WCHAR     szDesc[A114_DESC_LEN], szWork[A114_DESC_LEN];
    LPCWSTR   lpszDesc;

    /* ----- Get Control Code Data (Address 4 & 5) ----- */
    bWork = (BYTE)((fMode == A114_LBENTER) ? abCtrlCode[A114_CTRLCODE2] : tempDept.ParaDept.auchControlCode[A114_CTRLCODE2]);

    /* ----- Get Current International VAT Data ----- */
    bVAT = (BYTE)(bWork & A114_TAX_BIT_VAT);

    /* ----- Insert Description to Control Code ListBox ----- */
    LoadString(hResourceDll, IDS_A114_VAT0 + bVAT, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemSendTextMessage(hDlg, IDD_A114CTRL_LIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szDesc));
                       
    /* ----- Get Current VAT Serviceable Data ----- */
    bVAT = (BYTE)(bWork & A114_TAX_BIT_SERVICE);
    
    /* ----- Insert Description to Control Code ListBox ----- */
    LoadString(hResourceDll, IDS_A114_VATSERON, szDesc, PEP_STRING_LEN_MAC(szDesc));
    LoadString(hResourceDll, IDS_A114_VATSEROFF, szWork, PEP_STRING_LEN_MAC(szWork));
    
    lpszDesc = ((bVAT) ? szDesc : szWork);
    
    /* ----- Insert Description to Control Code ListBox ----- */
    DlgItemSendTextMessage(hDlg, IDD_A114CTRL_LIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)lpszDesc);

    /* ----- Get Current Discount Item #1 & #2 Data ----- */
    bDisc = PEP_LOMASK(bWork);

    /* ----- Set Address of Discount Item #1 String ----- */
    lpszDesc = ((bDisc & A114_DISC_ITEM1) ?
                        (lpCtrlDesc + A114_DSCITEM1)->szDescON :
                        (lpCtrlDesc + A114_DSCITEM1)->szDescOFF);
                
    /* ----- Insert Description to Control Code ListBox ----- */
    DlgItemSendTextMessage(hDlg, IDD_A114CTRL_LIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)lpszDesc);

    /* ----- Set Address of Discount Item #2 String ----- */
    lpszDesc = ((bDisc & A114_DISC_ITEM2) ?
                        (lpCtrlDesc + A114_DSCITEM2)->szDescON :
                        (lpCtrlDesc + A114_DSCITEM2)->szDescOFF);
                
    /* ----- Insert Description to Control Code ListBox ----- */
    DlgItemSendTextMessage(hDlg, IDD_A114CTRL_LIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)lpszDesc);
}


/*
* ===========================================================================
**  Synopsis    :   VOID    A114ChkRng()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wEditID -   Current Inputed EditText ID
*                   UINT    unMax   -   Max. Value of Input Data
*
**  Return      :   No return value.
*
**  Description :
*       This function checks whether the inputed data is out of range or not.
*   It displays a error message by messagebox, if the inputed data is invalid.
* ===========================================================================
*/
VOID    A114ChkRng(HWND hDlg, WORD wEditID, UINT unMax)
{
    UINT    unValue;

    /* ----- Get Inputed Data from EditText ----- */
    /* ----- Check Whether Inputed Data is Out of Range or Not ----- */
    unValue = GetDlgItemInt(hDlg, wEditID, NULL, FALSE);
    if (unValue > unMax) {
		SHORT   nMin = A114_DATAMIN;
		WCHAR   szCap[PEP_CAPTION_LEN], szWork[PEP_OVER_LEN], szMsg[PEP_OVER_LEN];

        /* ----- Load Error Message / Caption from Resource ----- */
        LoadString(hResourceDll, IDS_PEP_CAPTION_A114, szCap,  PEP_STRING_LEN_MAC(szCap));
        LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_STRING_LEN_MAC(szWork));
        wsPepf(szMsg, szWork, nMin, unMax);

        /* ----- Display Error Message by MessageBox ----- */
        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg, szMsg, szCap, MB_ICONEXCLAMATION | MB_OK);
    
        /* ----- Undo Data Input ----- */
        SendDlgItemMessage(hDlg, wEditID, EM_UNDO, 0, 0L);

        /* ----- Set Focus to Error Occurred EditText ----- */
        SetFocus(GetDlgItem(hDlg, wEditID));
        SendDlgItemMessage(hDlg, wEditID, EM_SETSEL, 1, MAKELONG(0, -1));
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A114SetCtrlData()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function checks control of DialogBox by current control code.
* ===========================================================================
*/
VOID    A114SetCtrlData(HWND hDlg)
{
    WCHAR   szDesc[48];             /* Saratoga */
    BYTE   abI;                    /* Saratoga */
    UINT   wID;                    /* Saratoga */

    /* ----- Set Control Code Data to CheckButton for Address 2 & 3 ----- */
    A114SetChkbox(hDlg, IDD_A114ST01, A114_CHKBIT2, (BYTE)A114_CTRLCODE1);

    /* ----- Determine Current Tax Mode  ----- */
    if (bTaxMode == A114_TAX_CAN) {     /* Canadian Tax */
        /* ----- Set Canadian Tax Data to RadioButton ----- */
        A114SetCanadaBtn(hDlg);
    } else if (bTaxMode == A114_TAX_US) {            /* U.S. Tax     */
        /* ----- Set Control Code to CheckButton for Address 4 & 5 ----- */
        A114SetChkbox(hDlg, IDD_A114ST09, A114_CHKBIT3, (BYTE)A114_CTRLCODE2);
    } else {                    /* International VAT */
        /* ----- Set International VAT Data to RadioButton ----- */
        A114SetVATBtn(hDlg);
    }

    /* ----- Set Control Code Data to CheckButton for Address 6 & 7 ----- */
    A114SetChkbox(hDlg, IDD_A114ST14, A114_CHKBIT4, (BYTE)A114_CTRLCODE3);

    /* ----- Set Control Code Data to CheckButton for Address 13 ----- */
    A114SetChkbox(hDlg, IDD_A114ST21, A114_CHKBIT7, (BYTE)A114_CTRLCODE5);

    /* ----- Set Control Code Data to CheckButton for Address 9 ----- */
    A114SetChkbox(hDlg, IDD_A114ST25, A114_CHKBIT6, (BYTE)A114_CTRLCODE4);

   /* ----- Set Initial Address 14 ----- */
   for(wID = IDS_A114_TYPE; wID <= IDS_A114_TYPE3; wID++) {
       /* ----- Load String for Insert ComboBox ----- */
       LoadString(hResourceDll, wID, szDesc, PEP_STRING_LEN_MAC(szDesc));
       /* ----- Insert Loaded Description to ComboBox ----- */
       DlgItemSendTextMessage(hDlg, IDD_A114_TYPEKEY, CB_INSERTSTRING, -1, (LPARAM)(szDesc)); //RPH 12/16/02 SR#84
   }

   /* ----- Set Control Code Data to Combobox for Address 14 ----- */
   abI = (BYTE)((abCtrlCode[A114_CTRLCODE5] & A114_AGETYPE_BIT) >> 4); /* ### MOD Saratoga (052900) */
   SendDlgItemMessage(hDlg, IDD_A114_TYPEKEY, CB_SETCURSEL, (WORD)abI, 0L);

    /* ### ADD Saratoga WIC (052900) */
    if(abCtrlCode[A114_CTRLCODE5] & A114_WIC_BIT)
        CheckDlgButton(hDlg, IDD_A114ST30, TRUE);
    else
        CheckDlgButton(hDlg, IDD_A114ST30, FALSE);

}

/*
* ===========================================================================
**  Synopsis    :   VOID    A114SetChkbox()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*                   WORD    wBtnID      -   Start Button ID
*                   BYTE    bChkBit     -   Bit Pattern without Reserved Area
*                   BYTE    bCtrlAddr   -   Data Address of Control Code
*
**  Return      :   No return value.
*
**  Description :
*       This function checks the checkbutton by current control code of 
*   selected department data.
* ===========================================================================
*/
VOID    A114SetChkbox(HWND hDlg, WPARAM wBtnID, BYTE bChkBit, BYTE bCtrlAddr)
{
    BYTE    bI;

    for (bI = 0; bI < A114_8BITS; bI++) {
        /* ----- Determine Whether Current Bit is Reserved Area or Not ----- */
        if (bChkBit & (A114_BITMASK << bI)) {
			BOOL    fCheck;
            /* ----- Get Current Bit Status ----- */
            fCheck = (abCtrlCode[bCtrlAddr] & (A114_BITMASK << bI)) ? TRUE : FALSE;

            /* ----- Check Target CheckButton ----- */
            CheckDlgButton(hDlg, wBtnID++, fCheck);
        }
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A114SetCanadaBtn()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function checks control of DialogBox by canadian tax.
* ===========================================================================
*/
VOID    A114SetCanadaBtn(HWND hDlg)
{
    WORD    wBtnID;
    BOOL    fCheck1, fCheck2;

    /* ----- Get Button ID of Current Canadian Tax ----- */
    wBtnID = (WORD)((abCtrlCode[A114_CTRLCODE2] & A114_TAX_BIT) + IDD_A114_CAN0);

    /* ----- Check RadioButton by Current Canadian Tax ----- */
    CheckRadioButton(hDlg, IDD_A114_CAN0, IDD_A114_CAN9, wBtnID);

    /* ----- Get Status of Address 5 (Discount Item #1 / #2) ----- */
    fCheck1 = (abCtrlCode[A114_CTRLCODE2] & A114_DISC_ITEM1) ? TRUE : FALSE;
    fCheck2 = (abCtrlCode[A114_CTRLCODE2] & A114_DISC_ITEM2) ? TRUE : FALSE;

    /* ----- Set Status of Addr.5 to CheckButton ----- */
    CheckDlgButton(hDlg, IDD_A114ST12, fCheck1);
    CheckDlgButton(hDlg, IDD_A114ST13, fCheck2);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A114SetVATBtn()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function checks control of DialogBox by canadian tax.
* ===========================================================================
*/
VOID    A114SetVATBtn(HWND hDlg)
{
    WORD    wBtnID;
    BOOL    fCheck1, fCheck2;

    /* ----- Get Button ID of Current International VAT ----- */
    wBtnID = (WORD)((abCtrlCode[A114_CTRLCODE2] & A114_TAX_BIT_VAT) + IDD_A114_VAT0);

    /* ----- Check RadioButton by Current International VAT ----- */
    CheckRadioButton(hDlg, IDD_A114_VAT0, IDD_A114_VAT3, wBtnID);
    
    /* ----- GetStatus of VAT serviceable (Addr.4 Bit 3) ----- */
    fCheck1 = (abCtrlCode[A114_CTRLCODE2] & A114_TAX_BIT_SERVICE) ? TRUE : FALSE;
    
    /* ----- Set Status of Addr.4 to CheckButton ----- */
    CheckDlgButton(hDlg, IDD_A114_VATSERVICE, fCheck1);

    /* ----- Get Status of Address 5 (Discount Item #1 / #2) ----- */
    fCheck1 = (abCtrlCode[A114_CTRLCODE2] & A114_DISC_ITEM1) ? TRUE : FALSE;
    fCheck2 = (abCtrlCode[A114_CTRLCODE2] & A114_DISC_ITEM2) ? TRUE : FALSE;

    /* ----- Set Status of Addr.5 to CheckButton ----- */
    CheckDlgButton(hDlg, IDD_A114ST12, fCheck1);
    CheckDlgButton(hDlg, IDD_A114ST13, fCheck2);
}



/*
* ===========================================================================
**  Synopsis    :   VOID    A114GetCtrlData()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function gets inputed control code data from each controls, and
*   then it sets them to static data buffer.
* ===========================================================================
*/
VOID    A114GetCtrlData(HWND hDlg)
{
    WORD   wI;      /* Saratoga */
    BYTE   abI;     /* Saratoga */

    /* ----- Get Data from Checkbox for Address 2 & 3 ----- */
    A114GetChkbox(hDlg, IDD_A114ST01, A114_CHKBIT2, (BYTE)A114_CTRLCODE1);

    if (bTaxMode == A114_TAX_CAN) {
        /* ----- Get Canadian Tax Data from RadioButton ----- */
        A114GetCanadaBtn(hDlg);
    } else if (bTaxMode == A114_TAX_US) {
        /* ----- Get Data from Checkbox for Address 4 & 5 ----- */                         
        A114GetChkbox(hDlg, IDD_A114ST09, A114_CHKBIT3, (BYTE)A114_CTRLCODE2);
    } else {
        /* ----- Get International VAT Data from RadioButton ----- */
		A114GetVATBtn(hDlg);
    }
    
    /* ----- Get Data from Checkbox for Address 6 & 7 ----- */
    A114GetChkbox(hDlg, IDD_A114ST14, A114_CHKBIT4, (BYTE)A114_CTRLCODE3);

    /* ----- Get Data from Checkbox for Address 13 ----- */
    A114GetChkbox(hDlg, IDD_A114ST21, A114_CHKBIT7, (BYTE)A114_CTRLCODE5);

    /* ----- Get Data from Checkbox for Address 9 ----- */
    A114GetChkbox(hDlg, IDD_A114ST25, A114_CHKBIT6, (BYTE)A114_CTRLCODE4);

/* Saratoga Start */

   /* ----- Get Control Code Data to Combobox for Address 14 ----- */
   wI = (BYTE)SendDlgItemMessage(hDlg, IDD_A114_TYPEKEY, CB_GETCURSEL, 0, 0L);

   abI = abCtrlCode[A114_CTRLCODE5];
   abI &= A114_CHKBIT;
   abI |= ((BYTE)wI << 4); 
   abCtrlCode[A114_CTRLCODE5] = abI;

    /* ### ADD Saratoga WIC (052900) */
    if (IsDlgButtonChecked(hDlg, IDD_A114ST30) == TRUE)
       abCtrlCode[A114_CTRLCODE5] |= A114_WIC_BIT;
	else
       abCtrlCode[A114_CTRLCODE5] &= (~(A114_WIC_BIT));

/* Saratoga End */

}

/*
* ===========================================================================
**  Synopsis    :   VOID    A114GetChkbox()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*                   WORD    wBtnID      -   Start Button ID
*                   BYTE    bChkBit     -   Bit Pattern without Reserved Area
*                   BYTE    bCtrlAddr   -   Data Address of Control Code
*
**  Return      :   No return value.
*
**  Description :
*       This function gets inputed control code data from checkbox. And then
*   it sets them to the static control code data buffer.
* ===========================================================================
*/
VOID    A114GetChkbox(HWND hDlg, WPARAM wBtnID, BYTE bChkBit, BYTE bCtrlAddr)
{
    BYTE    bI;

    /* ----- Reset Target Control Code Area without Reserved Area ----- */
    abCtrlCode[bCtrlAddr] &= (~bChkBit);

    for (bI = 0; bI < A114_8BITS; bI++) {
        /* ----- Determine Whether Current Bit is Reserved Area or Not ----- */
        if (bChkBit & (A114_BITMASK << bI)) {    
            /* ----- Get Data from CheckBox ----- */
            if (IsDlgButtonChecked(hDlg, wBtnID++) == TRUE) {
                abCtrlCode[bCtrlAddr] |= (A114_BITMASK << bI);
            }
        }
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A114GetCanadaBtn()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function gets inputed canadian tax data from radiobutton. And
*   then it sets them to static control code data buffer.
* ===========================================================================
*/
VOID    A114GetCanadaBtn(HWND hDlg)
{
    BYTE    bCanTax;
    UINT    unCheck;

    /* ----- Reset Address 4 & 5 Area without Reserved Area ----- */
    abCtrlCode[A114_CTRLCODE2] &= A114_TAX_MASK;

    for (bCanTax = 0; bCanTax <= A114_TAX_MAX; bCanTax++) {
        /* ----- Determine Whether Target Button is Checked or Not ----- */
        unCheck = IsDlgButtonChecked(hDlg, (int)(IDD_A114_CAN0 + bCanTax));

        if (unCheck == TRUE) {
            break;
        }
    }

    /* ----- Set Inputed Canadian Tax Data to Data Buffer ----- */
    abCtrlCode[A114_CTRLCODE2] |= bCanTax;

    /* ----- Determine Discount Item #1 Data from CheckBox ----- */
    if (IsDlgButtonChecked(hDlg, IDD_A114ST12) == TRUE) {
        /* ----- Set Discount Item #1 Data to Buffer ----- */
        abCtrlCode[A114_CTRLCODE2] |= A114_DISC_ITEM1;
    }

    /* ----- Determine Discount Item #2 Data from CheckBox ----- */
    if (IsDlgButtonChecked(hDlg, IDD_A114ST13) == TRUE) {
        /* ----- Set Discount Item #2 Data to Buffer ----- */
        abCtrlCode[A114_CTRLCODE2] |= A114_DISC_ITEM2;
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A114GetVATBtn()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function gets inputed canadian tax data from radiobutton. And
*   then it sets them to static control code data buffer.
* ===========================================================================
*/
VOID    A114GetVATBtn(HWND hDlg)
{
    BYTE    bVAT;
    UINT    unCheck;

    /* ----- Reset Address 4 & 5 Area without Reserved Area ----- */
    abCtrlCode[A114_CTRLCODE2] &= A114_TAX_MASK;

    for (bVAT = 0; bVAT < A114_TAX_MAX_VAT; bVAT++) {
        /* ----- Determine Whether Target Button is Checked or Not ----- */
        unCheck = IsDlgButtonChecked(hDlg, (int)(IDD_A114_VAT0 + bVAT));

        if (unCheck == TRUE) {
            break;
        }
    }

    /* ----- Set Inputed Canadian Tax Data to Data Buffer ----- */
    abCtrlCode[A114_CTRLCODE2] |= bVAT;

    /* ----- Determine VAT Serviceable Data from CheckBox ----- */
    if (IsDlgButtonChecked(hDlg, IDD_A114_VATSERVICE) == TRUE) {
        /* ----- Set Discount Item #1 Data to Buffer ----- */
        abCtrlCode[A114_CTRLCODE2] |= A114_TAX_BIT_SERVICE;
    }

    /* ----- Determine Discount Item #1 Data from CheckBox ----- */
    if (IsDlgButtonChecked(hDlg, IDD_A114ST12) == TRUE) {
        /* ----- Set Discount Item #1 Data to Buffer ----- */
        abCtrlCode[A114_CTRLCODE2] |= A114_DISC_ITEM1;
    }

    /* ----- Determine Discount Item #2 Data from CheckBox ----- */
    if (IsDlgButtonChecked(hDlg, IDD_A114ST13) == TRUE) {
        /* ----- Set Discount Item #2 Data to Buffer ----- */
        abCtrlCode[A114_CTRLCODE2] |= A114_DISC_ITEM2;
    }
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A114RegistData()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*
**  Return      :   BOOL    TRUE        -   Fail to Register PLU Data
*                           FALSE       -   Succeed in Register PLU Data
*
**  Description :
*       This procedure register inputed PLU data to PLU file. If No. of PLU
*   records is over than maximum No. of PLU records, it return TRUE. And If
*   inputed PLU type is different from previous PLU type, and user selects
*   CANCEL in MessageBox, it returns TRUE.
*       Otherwise it returns FALSE.
* ===========================================================================
*/
BOOL A114RegistData(HWND hDlg, DEPTIF *pDpi)
{
	DEPTIF  DeptWork = {0};
    BOOL    fRet = FALSE;
    WCHAR   szDesc[128], szWork[128];

    /* ----- Determine Whether Target Data Already Exists in PLU File ----- */
    DeptWork.usDeptNo = pDpi->usDeptNo;
    if (OpDeptIndRead(&DeptWork, 0) == OP_NOT_IN_FILE) {    /* Not Found in DEPT File    */
        /* ----- Check Max. Record No. of PLU Records ----- */
        if (! A114ChkMaxRec()) {       /* Not Over Max. Record */
            /* ----- Assign Dept Record ----- */
            OpDeptAssign(pDpi, 0);
            /* ----- Increment Current Exist Dept Data in Static Area ----- */
            unCurDept++;
        } else {                            /* Over Max. Dept Record */
            /* ----- Load String from Resource ----- */
            LoadString(hResourceDll, IDS_PEP_CAPTION_A114, szWork, PEP_STRING_LEN_MAC(szWork));
            LoadString(hResourceDll, IDS_A114_OVERREC, szDesc, PEP_STRING_LEN_MAC(szDesc));

            /* ----- Display Error Message ----- */
            MessageBeep(MB_ICONEXCLAMATION);
            MessageBoxPopUp(hDlg, szDesc, szWork, MB_OK | MB_ICONEXCLAMATION);
                       
            fRet = TRUE;
        }
    } else {                            /* Found in Dept Records */
       OpDeptAssign(pDpi, 0);
    }

    /* ----- Create Current Record No. String ----- */
    /* ----- Set Current PLU Record No. (Prog. #2) ----- */
    LoadString(hResourceDll, IDS_A114_CUR, szWork, PEP_STRING_LEN_MAC(szWork));
    wsPepf(szDesc, szWork, unCurDept);
    DlgItemRedrawText(hDlg, IDD_A114_CUR, szDesc);

    return (fRet);
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A114ChkMaxRec()
*
**  Input       :   Nothing
*
**  Return      :   BOOL    TRUE    -   No. of PLU Record is Over Maximum.
*                           FALSE   -   No. of PLU Record is within Maximum.
*
**  Description :
*       This procedure compares No. of current records with maximum no. of
*   PLU records. It returns TRUE, if No. of Current records is over than
*   maximum No. of PLU records. Otherwise it returens FALSE.
* ===========================================================================
*/
BOOL    A114ChkMaxRec(VOID)
{
    BOOL    fRet;

    fRet = (unDeptMax <= unCurDept) ? TRUE : FALSE;

    return (fRet);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A114SetListDesc()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value
*
**  Description :
*       This procedure sets the data in the dialogbox.
*       Replace Double Key Code (0x12 -> '~')
*       Create Description for Insert ListBox
*       Insert Created Description to PLU ListBox
* ===========================================================================
*/
VOID    A114SetListDesc(HWND hDlg, DEPTIF *pDpi)
{
    if (pDpi->usDeptNo) {
		WCHAR    szDesc[48] = {0}, szName[A114_MNEMO_SIZE + 1] = {0};

        PepReplaceMnemonic(pDpi->ParaDept.auchMnemonic, szName, (A114_MNEMO_SIZE ), PEP_MNEMO_READ);
        wsPepf(szDesc, WIDE("%04u : %s"), pDpi->usDeptNo, szName);
        DlgItemSendTextMessage(hDlg, IDD_A114DEPT_LIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szDesc));
    }
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A114DeleteData()
*
*
**  Return      :   BOOL    TRUE        -   Fail to Delete Dept Data
*                           FALSE       -   Succeed in Delete Dept Data
*
**  Description :
*       This procedure deletes a specified Dept record from Dept file.
* ===========================================================================
*/
BOOL    A114DeleteData(VOID)
{
    BOOL    fRet = TRUE;
	DEPTIF  DeptWork = {0};

    /* ----- Determine Whether Target Data Exists in Dept File ----- */
    DeptWork.usDeptNo   = tempDept.usDeptNo;
    if (OpDeptIndRead(&DeptWork, 0) != OP_NOT_IN_FILE) {    /* Target Dept is Found in Dept File  */
        /* ----- Delete a Specified Dept Record ----- */
        OpDeptDelete(&tempDept, 0);
        fRet = FALSE;
    }
    return (fRet);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A114InitDlg()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*                   UINT    unMaxDept   -   Max. Record No. of Department
*
**  Return      :   No return value.
*
**  Description :
*       This function gets current tax mode. And then it loads the initial
*   department data, and sets them to listbox.
* ===========================================================================
*/
VOID    A114InitDlg(HWND hDlg)
{
    WORD    wI;
    BYTE    bTaxCheck1, bTaxCheck2;
    WCHAR   szFile[PEP_FILE_BACKUP_LEN];  /* NCR2172 */
    short   nRet= OP_PERFORM;
    UINT    unCur = 0;
    WCHAR   szWork[64], szDesc[64];

    /* ----- LOAD Dept File Name from Resorce ----- */
    LoadString(hResourceDll, IDS_FILE_NAME_DEPT, szFile, PEP_STRING_LEN_MAC(szFile));

    /* ----- Create Backup File for Cancel Key ----- */
    PepDeptFileBackup(hDlg, szFile, PEP_FILE_BACKUP);

    /* ----- Set Control Code Description to Allocated Area ----- */
    A114InitCtrlDesc();

   /* ----- Limit Max. Length of Input Data to Each EditText ----- */
    SendDlgItemMessage(hDlg, IDD_A114DEPT, EM_LIMITTEXT, A114_DEPTLEN, 0L);
    SendDlgItemMessage(hDlg, IDD_A114HALO,  EM_LIMITTEXT, A114_HALOLEN,  0L);
    SendDlgItemMessage(hDlg, IDD_A114BONUS, EM_LIMITTEXT, A114_BONUSLEN, 0L);
    SendDlgItemMessage(hDlg, IDD_A114MNEMO, EM_LIMITTEXT, PARA_DEPT_LEN, 0L);
    SendDlgItemMessage(hDlg, IDD_A114PRTPRY,EM_LIMITTEXT, A114_PRTPRYLEN, 0L);
    SendDlgItemMessage(hDlg, IDD_A114TARE,  EM_LIMITTEXT, A114_TARELEN, 0L);

    /* ----- Set Fixed Font ----- */
    PepSetFont(hDlg, IDD_A114MNEMO);
    PepSetFont(hDlg, IDD_A114DEPT_LIST);

    /* ----- Get Current Tax Mode Status from Prog. #1 (MDC) ----- */
	bTaxCheck1 = ParaMDCCheck(MDC_TAX_ADR, ODD_MDC_BIT0);
	bTaxCheck2 = ParaMDCCheck(MDC_TAXINTL_ADR, ODD_MDC_BIT0);

    bTaxMode = (BYTE)(!(bTaxCheck1) ? A114_TAX_US  : !(bTaxCheck2) ? A114_TAX_CAN : A114_TAX_VAT);

    /* ----- Reset Dept Work Area Counter ----- */
    tempDept.ulCounter = 0L;
    do{
       /* ----- Load Dept Records from Dept File until End of File ----- */
       nRet = OpDeptAllRead(&tempDept, 0);

       /* ----- Check End of Dept File ----- */
       if((nRet == OP_FILE_NOT_FOUND) || (nRet == OP_EOF)) {
         break;
       } else {
		   /* ----- Increment Current No. of Dept Records ----- */
		   A114SetListDesc(hDlg, &tempDept);
		   unCur++;
       }
    } while (nRet == OP_PERFORM);

    /* ----- Set HighLight Bar to the Top Index of ListBox ----- */
    /* ----- Insert Strings into ComboBox ----- */
    for (wI = 0; wI <= A114_MDEPTMAX; wI++) {
        if (! wI) {
            /* ----- Load "None" Description ----- */
            LoadString(hResourceDll, IDS_A114_NONE, szDesc, PEP_STRING_LEN_MAC(szDesc));
        } else {
            wsPepf(szDesc, WIDE("%u"), wI);
        }

        DlgItemSendTextMessage(hDlg, IDD_A114MAJOR, CB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szDesc));
    }

    /* ----- Set Dept. Data of the Top Index to Each Control ----- */
    SetDlgItemInt(hDlg, IDD_A114HALO,    A114_DATAMIN,  FALSE);
/*    SetDlgItemInt(hDlg, IDD_A114MAJOR,   A114_MDEPTMIN, FALSE);*/
    SendDlgItemMessage(hDlg, IDD_A114MAJOR, CB_SETCURSEL, A114_DATAMIN, FALSE);

    SetDlgItemInt(hDlg, IDD_A114BONUS,   A114_DATAMIN,  FALSE);
    SetDlgItemInt(hDlg, IDD_A114PRTPRY,  A114_DATAMIN,  FALSE);
    SetDlgItemInt(hDlg, IDD_A114TARE,    A114_DATAMIN,  FALSE);

    A114CtrlButton(hDlg);

    /* ----- Create Max. Record Number Description ----- */
    /* ----- Set Created Description to StaticText ----- */
    LoadString(hResourceDll, IDS_A114_NOREC, szWork, PEP_STRING_LEN_MAC(szWork));
    wsPepf(szDesc, szWork, unDeptMax);
    DlgItemRedrawText(hDlg, IDD_A114MAX, szDesc);

    unCurDept = unCur;

    /* ----- Create Current Record No. String ----- */
    /* ----- Set Current PLU Record No. (Prog. #2) ----- */
    LoadString(hResourceDll, IDS_A114_CUR, szWork, PEP_STRING_LEN_MAC(szWork));
    wsPepf(szDesc, szWork, unCurDept);
    DlgItemRedrawText(hDlg, IDD_A114_CUR, szDesc);

	// Initialize the static current department information
	memset(&tempDept, 0x00, sizeof( DEPTIF ));
}

HWND A114ModeLessDialog (HWND hParentWnd, int nCmdShow)
{
	hWndModeLess = DialogCreation(hResourceDll/*hActInst*/,  //RPH 4-23-03 Multilingual
                                   MAKEINTRESOURCEW(IDD_A114),
                                   hParentWnd,
                                   A114DlgProc);
	hWndModeLess && ShowWindow (hWndModeLess, nCmdShow);
	return hWndModeLess;
}

HWND  A114ModeLessDialogWnd (void)
{
	return hWndModeLess;
}

/* ===== End of File (A114.C) ===== */
