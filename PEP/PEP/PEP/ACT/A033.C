/*
* ---------------------------------------------------------------------------
* Title         :   Set Control String Table (AC 33)
* Category      :   Maintenance, AT&T 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A033.C
* Copyright (C) :   1995, AT&T Corp. E&M-OISO, All rights reserved.
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
* Apr-12-95 : 00.00.01 : H.Ishida   : Initial (Migrate from HP US Model)
* Jul-25-95 : 03.00.00 : hkato      : R3.0 FVT Comment.
* Mar-18-96 : 03.00.05 : M.Suzuki   : Add R3.1
* Sep-11-98 : 03.03.00 : A.Mitsui   : V3.3
* Oct-23-98 : 03.03.01 : A.Mitsui   : Server and ChargeTips migration.
* Dec-10-99 :          : K.Yanagida : NCR2172
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

//#include	"stringResource.h" //Mulitlingual PEP RPH 4-21-03
#include    <ecr.h>
#include    <paraequ.h>
#include    <para.h>
#include    "csop.h"
#include    "fsc.h"

#include    "file.h"
#include    "action.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "a033.h"

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
	//	This structure defines the upper range of the radio input in 
	//	the AC 33 "Set Control String Table" buttons that pop up dialoges
	//	for extended FSC data
static  WORD    awExtRngTbl1[A33_FSC_NO] = {
                    0,                          // A33_NOFUNC		- no function key 
                    165,                        // A33_KEYEDPLU		- keyed PLU key
                    50,                         // A33_KEYEDDPT		- keyed dept. key
                    8,                          // A33_PRTMOD		- print modifier key 
                    20,                         // A33_ADJ			- adjective key
                    20,                         // A33_TENDER		- tender key					
                    8,                          // A33_FC			- foreign currency key	| NCR2172 previous value: 2
                    20,                         // A33_TTL			- total key             | ### MOD Saratoga (8 --> 9)(052500)
                    3,                          // A33_ADDCHK		- add check key      
                    6,                          // A33_IDISC		- item discount key
                    6,                          // A33_RDISC		- regular discount key 
                    11,                         // A33_TAXMOD		- tax modifier key		| R3.0
                      0,0,0,0,0,0,0,0,0,        // FSC No.	12 - 20
                    0,0,0,0,0,0,0,0,0,          // FSC No.	21 - 29
					2,                          // A33_CANCEL - Cancel key, range of 0 - 1, zero based
                    0,0,				        // FSC No.	31 - 32
					4,                          // A33_VTRAN - Transaction Void key, range of 0 - 3, zero based
					0,0,0,				        // FSC No.	34 - 36
					3,                          // A33_PRTDEM		- print on demand key
                    0,                          // A33_ORDERNO		- order no. display key	| not active
                    3,                          // A33_CHARTIP		- charge tips key
                    0,                          // A33_PLUNO		- PLU number key		| not active
                    0,0,0,0,0,0,0,0,0,0,        // FSC No.  41 - 50
                    0,0,                        // FSC No.  51 - 52
                    4,                          // A33_PRECASH		- preset cash tender
                    9,                          // A33_DRCTSHFT		- direct shift key
                            0,0,				// FSC No.  55 - 56
                    100,                        // A33_KEYEDCPN		- Keyed Coupon Key
                                  0,0,0,		// FSC No.	58 - 60
                    0,0,0,0,0,0,0,0,0,0,		// FSC No.	61 - 70
                    2,                          // A33_SEAT			- Seat # Key			| R3.1
                      0,0,					    // FSC No.	72 - 73
					4,                          // A33_CURSORVOID - range is 0 to 3, zero based
					0,					        // A33_PRICECHECK
					20,                         // A33_ASK			- Ask (for EPT)			| NCR2172
                                0,0,0,			// FSC No.	77 - 79
                    5,                          // A33_LEVEL		- Adjective Price Level Set
                    0,0,0,0,0,0,0,0,0,0,		// FSC No.	81 - 90
					0,0,0,                      // FSC No.	91 - 93
					5,                          // A33_ORDER_DEC,  range of 0 to 4, zero based
					0,0,0,0,0,0,		        // FSC No.	95 - 100
					0,0,0,0,0,0,0,0,0,0,		// FSC No.	101 - 110
					0,0,0,0,0,0,0,0,0,0,		// FSC No.	111 - 120
					0,0,0,0,0,0,0,0,0,0,		// FSC No.	121 - 130
					0,0,0,0,0,0,0,0,0,0			// FSC No.	131 - 140
                };

static  WORD    awExtRngTbl2[A33_FXK_NO] = {
                    120,                        /* Pause Key, R3.0           100 */
                    400,                        /* Call another c.string Key 101 */
                    0,                          /* numeric 0 key             102 */
                    0,                          /* numeric 1 key             103 */
                    0,                          /* numeric 2 key             104 */
                    0,                          /* numeric 3 key             105 */
                    0,                          /* numeric 4 key             106 */
                    0,                          /* numeric 5 key             107 */
                    0,                          /* numeric 6 key             108 */
                    0,                          /* numeric 7 key             109 */
                    0,                          /* numeric 8 key             110 */
                    0,                          /* numeric 9 key             111 */
                    0,                          /* Clear Key                 112 */
                    0,                          /* P1 Key                    113 */
                    0,                          /* P2 Key                    114 */
                    0,                          /* P3 Key                    115 */
                    0,                          /* P4 Key                    116 */
                    0                           /* P5 Key                    117 */
                };

static  LPCSTRINDEX  lpCstrIndtb;   /* Address of Control String Index Buffer    */
static  LPCSTRPARA   lpCstrPara;    /* Address of Control String Data Buffer     */
static  UINT         unListAdr;     /* Current Selected Adr. of Control String   */
static  UINT         unListNo;      /* Current Selected Number of Control String */
static  WORD         wCurBtnID;     /* Current Selected Button ID                */
static  USHORT       usLength;      /* Length of Edit all data                   */
static  PEPSPIN      PepSpin;       /* Configulation of SpinButton           */
static  UINT         unMaxCstr;     /* Maximum Number of Control String Data */
static  HGLOBAL      hCstrInd;      /* Handle of Global Heap (C.String Index)*/
static  HGLOBAL      hCstrMem;      /* Handle of Global Heap (C.String Data) */
static  BOOL         fAddRec=TRUE;  /* Data Additon/Append Flag              */
static  BOOL         fInit=TRUE;    /* Process Initializer flag              */
static  BOOL         fExtFlg=FALSE; /* Return status    FALSE : no set data  */
                                    /*                  TRUE  : set data     */
static  UCHAR        uchRetExtData; /* Return Extend data                    */
static  BOOL         fUpDate=FALSE; /* Data update flag FALSE : no up date   */
                                    /*                  TRUE  : set data     */
static  BOOL        fValidValue;

/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  A033DlgProc()
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
*       Main Procedure for Dialog Box concerning the Set Control String Table
*   (AC 33). The function takes in the user input at the dialog box and handles
*	the user input and gives it to the appropriate function to handle the action.
*	This function also recioeves the message from the window and moves through a
*	switch statement to determine the correct function for the input to be used by.
* ===========================================================================
*/
BOOL    WINAPI  A033DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    int     idEdit;
    HCURSOR hCursor;
    WCHAR   szAlphaStr[ A33_ALPHA_STR_LEN + 1 ];
    int     fCanceled;

    switch (wMsg) {

    case WM_INITDIALOG:

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- Check Max. Record No. of Control String Data ----- */

        if (A033ReadMax(hDlg, (LPUINT)&unMaxCstr)) {

			//Return cancel to indicate failure
            EndDialog(hDlg, LOWORD(IDCANCEL));

        } else {

            /* ----- Allocate Global Heap ----- */

            if (A033AllocMem(hDlg, (LPHGLOBAL)&hCstrInd, (LPHGLOBAL)&hCstrMem)) {

				//Return cancel to indicate failure
                EndDialog(hDlg, LOWORD(IDCANCEL));

            } else {

                /* ----- Initialize Configulation of DialogBox ----- */

                A033InitDlg(hDlg);

                /* ----- Initialize Configulation of SpinButton ----- */

                PepSpin.nStep      = A33_SPIN_STEP;
                PepSpin.nTurbo     = A33_SPIN_TURBO;
                PepSpin.fSpinStyle = PEP_SPIN_NOROLL;

            }
        }
        return TRUE;

	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=IDD_A33_NOFUNC; j<=IDD_A33_P5; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			for(j=IDD_A33_STRNO; j<=IDD_A33_STRTOOL; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_A33_ADDCHG, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_A33_DEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_A33_STRING, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_A33_ADDREC, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_VSCROLL:

        /* ----- Set Target EditText ID ----- */

        idEdit = (int)(GetDlgCtrlID((HWND)lParam) - A33_SPIN_OFFSET);

        /* ----- Set Maximum Value of Data Range by Selected Button ----- */

        PepSpin.lMin = (long)A33_DATAMIN;
        PepSpin.lMax = (long)A33_NO_MAX;

        if (PepSpinProc(hDlg, wParam, idEdit, (LPPEPSPIN)&PepSpin)) {

            PostMessage(hDlg, WM_COMMAND, MAKELONG(LOWORD(idEdit), EN_CHANGE), 0L);
        }
        return FALSE;

    case WM_COMMAND:

        switch (LOWORD(wParam)) {

        case IDD_A33_LIST:

            if (HIWORD(wParam) == LBN_SELCHANGE) {

                /* ----- Set Selected Control String Data to Each Control ----- */

                A033GetList(hDlg);

                return TRUE;
            }
            return FALSE;

        case IDD_A33_NO:

            if (HIWORD(wParam) == EN_CHANGE) {

                /* --- determine non-numeric char is inputed or not --- */

                GetDlgItemInt( hDlg, LOWORD(wParam), &fValidValue, FALSE );

                if (A033ChkUpdate(hDlg) == TRUE) {

                    /* ----- Check Inputed Data with Data Range ----- */

                    A033ChkRng(hDlg, LOWORD(wParam), A33_NO_MAX);

                    return TRUE;

                }
            }
            return FALSE;

        case IDD_A33_PAUSE:
        case IDD_A33_CALLSTR:
        case IDD_A33_KEYEDPLU:
        case IDD_A33_KEYEDDPT:
        case IDD_A33_PRTMOD:
        case IDD_A33_ADJ:
        case IDD_A33_TENDER:
        case IDD_A33_FC:
        case IDD_A33_TTL:
        case IDD_A33_ADDCHK:
        case IDD_A33_IDISC:
        case IDD_A33_RDISC:
        case IDD_A33_TAXMOD:
        case IDD_A33_CHARTIP:   /* V3.3 */
        case IDD_A33_PRECASH:
        case IDD_A33_DRCTSHFT:
        case IDD_A33_KEYEDCPN:
        case IDD_A33_PRTDEM:
        case IDD_A33_SEAT:
        case IDD_A33_ASK:                      /* NCR2172 */
        case IDD_A33_LEVEL:                    /* NCR2172 */
        case IDD_A33_CANCEL:
        case IDD_A33_CURSORVOID:
        case IDD_A33_VTRAN:
        case IDD_A33_ORDER_DEC:
            if (HIWORD(wParam) == BN_CLICKED) {
                if (unListNo >= A33_DATAMIN) {
                    wCurBtnID = LOWORD(wParam);
                    fExtFlg   = FALSE;

                    /* ----- Create control code dialogbox ----- */
                    DialogBoxPopup(hResourceDll, MAKEINTRESOURCEW(IDD_A33_ENTRY), hDlg, A033CtrlDlgProc);

                    /* ----- Check Extend data ----- */
                    if (fExtFlg == TRUE) {
                        /* ----- Set Extend data ----- */
                        A033SetExtFsc(hDlg);
                    }
                }
                return TRUE;
            }
            return FALSE;

        case IDD_A33_ZERO:
        case IDD_A33_ONE:
        case IDD_A33_TWO:
        case IDD_A33_THREE:
        case IDD_A33_FOUR:
        case IDD_A33_FIVE:
        case IDD_A33_SIX:
        case IDD_A33_SEVEN:
        case IDD_A33_EIGHT:
        case IDD_A33_NINE:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Change Selected Button Style ----- */
                A033SetData(hDlg, (UCHAR)FSC_AN, (UCHAR)(LOWORD(wParam) - IDD_A33_ZERO + A33_ALPHA_BASIC));

                return TRUE;
            }
            return FALSE;

        case IDD_A33_LOCK:
        case IDD_A33_REGS:
        case IDD_A33_SUPER:
        case IDD_A33_PROG:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Change Selected Button Style ----- */
                A033SetData(hDlg, (UCHAR)FSC_MODE, (UCHAR)(LOWORD(wParam) - IDD_A33_LOCK + A33_MODE_EXT));
                return TRUE;
            }
            return FALSE;

        case IDD_A33_P1:
        case IDD_A33_P2:
        case IDD_A33_P3:
        case IDD_A33_P4:
        case IDD_A33_P5:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Change Selected Button Style ----- */
                A033SetData(hDlg, (UCHAR)(LOWORD(wParam) - IDD_A33_P1 + FSC_P1),(UCHAR)0);
                return TRUE;
            }
            return FALSE;

        case IDD_A33_CLEAR:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Change Selected Button Style ----- */
                A033SetData(hDlg, (UCHAR)FSC_CLEAR,(UCHAR)0);
                return TRUE;
            }
            return FALSE;

        case IDD_A33_NOFUNC:
        case IDD_A33_AKEY:
        case IDD_A33_BKEY:
/*        case IDD_A33_SERVNO:  ,V3.3 */
        case IDD_A33_NEWCHK:
        case IDD_A33_PB:
        case IDD_A33_GCNO:
        case IDD_A33_GCTRANS:
        case IDD_A33_NOPERSON:
        case IDD_A33_TBLNO:
        case IDD_A33_LINENO:
        case IDD_A33_FEED:
        case IDD_A33_RCTNORCT:
        case IDD_A33_TIPSPO:
        case IDD_A33_DECTIP:
        case IDD_A33_NOSALE:
        case IDD_A33_PO:
        case IDD_A33_ROA:
        case IDD_A33_CHGCOMP:
        case IDD_A33_VOID:
        case IDD_A33_EC:
        case IDD_A33_QTY:
        case IDD_A33_SHIFT:
        case IDD_A33_NOTYPE:
/*        case IDD_A33_ORDERNO: */
/*        case IDD_A33_CHARTIP: ,V3.3 */
        case IDD_A33_PLUNO:
        case IDD_A33_00KEY:
        case IDD_A33_AC:
        case IDD_A33_RECFEED:
        case IDD_A33_JOUFEED:
        case IDD_A33_DECPT:
        case IDD_A33_SCALE:
        case IDD_A33_RVSPRI:
        case IDD_A33_TIMEIN:
        case IDD_A33_TIMEOUT:
        case IDD_A33_OFFTEND:
        case IDD_A33_CPNNO:
        case IDD_A33_UP:
        case IDD_A33_DOWN:
        case IDD_A33_RIGHT:
        case IDD_A33_LEFT:
        case IDD_A33_WAIT:
        case IDD_A33_PAID_REC:
        case IDD_A33_ALPHA:
        case IDD_A33_LEFTDISPLAY:
        case IDD_A33_BARTENDER:
        case IDD_A33_SPLIT:
        case IDD_A33_ITEMTRANS:
        case IDD_A33_REPEAT:
        case IDD_A33_PRICECHECK:
        case IDD_A33_OPRINT_A:          /* V3.3 */
        case IDD_A33_OPRINT_B:          /* V3.3 */
        case IDD_A33_MONEY:             /* NCR2172 */
        case IDD_A33_DEPTNUMBER:        /* NCR2172 */
        case IDD_A33_UPCVER:            /* NCR2172 */
        case IDD_A33_PRICEKEY:          /* NCR2172 */
        case IDD_A33_FSMOD:             /* ### ADD Saratoga (052500) */
        case IDD_A33_WICTRANS:          /* ### ADD Saratoga (052500) */
        case IDD_A33_WICMOD:            /* ### ADD Saratoga (052500) */
		case IDD_A33_VALIDATION:
		case IDD_A33_ATFOR:
		case IDD_A33_HALO_OVR:
		case IDD_A33_RECEIPT_ID:		//PDINU
		case IDD_A33_DOC_LAUNCH:		//FSC_DOC_LAUNCH
		case IDD_A33_WINDOW_DISPLAY:	//FSC_WINDOW_DISPLAY
		case IDD_A33_WINDOW_DISMISS:	//FSC_WINDOW_DISMISS
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Change Selected Button Style ----- */
                A033SetData(hDlg, (UCHAR) (LOWORD(wParam) - IDD_A33_NOFUNC + UIE_FSC_BIAS), (UCHAR)0);
                return TRUE;
            }
            return FALSE;

        case IDD_A33_ADDCHG:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Change parameter file ----- */
                A033ChgData(hDlg);
                return TRUE;
            }
            return FALSE;

        case IDD_A33_DEL:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Delete control string data ----- */
                A033DelData(hDlg);
                return TRUE;
            }
            return FALSE;

		case IDD_A33_UPSWITCH:
			if(HIWORD(wParam) == BN_CLICKED) {
				A033DataUp(hDlg);
				return TRUE;
			}
			return FALSE;

		case IDD_A33_DOWNSWITCH:
			if(HIWORD(wParam) == BN_CLICKED) {
				A033DataDown(hDlg);
				return TRUE;
			}
			return FALSE;

		case IDD_A33_CLEARALL:
			if(HIWORD(wParam) == BN_CLICKED) {
				A033ClearData(hDlg);
				return TRUE;
			}
			return FALSE;

        case IDD_A33_ADDREC:
            if (HIWORD(wParam) == BN_CLICKED) {
                if (IsDlgButtonChecked(hDlg, LOWORD(wParam))) {
                    fAddRec = TRUE;
                } else {
                    fAddRec = FALSE;
                }
                return TRUE;
            }
            return FALSE;

        case IDD_A33_STRING:
            /* --- create dialog for alpha string entry --- */
            memset(szAlphaStr, 0x00, sizeof( szAlphaStr ));
            fCanceled = DialogBoxPopupParam( hResourceDll, MAKEINTRESOURCEW( IDD_A33_STRDLG ), hDlg, A033StrDlgProc, ( LPARAM )(szAlphaStr ));

            if ( fCanceled == IDOK ) {
                /* --- save inputed alpha string to static buffer --- */
                A033SaveString( hDlg, szAlphaStr );
            }
            return TRUE;

        case IDOK:
        case IDCANCEL:
            if (HIWORD(wParam) == BN_CLICKED) {
                if (A033ChkUpdate(hDlg) != TRUE) {
                    return FALSE;
                }

                /* ----- Change cursor (arrow -> hour-glass) ----- */
                hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
                ShowCursor(TRUE);

                /* ----- Finalize DialogBox ----- */
                A033FinDlg(hDlg, wParam);

                /* ----- Frees Allocated Memory ----- */
                A033FreeMem((LPHGLOBAL)&hCstrInd, (LPHGLOBAL)&hCstrMem);

                /* ----- Restore cursor (hour-glass -> arrow) ----- */
                ShowCursor(FALSE);
                SetCursor(hCursor);

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
**  Synopsis    :   BOOL    A033ReadMax()
*
**  Input       :   HWND        hDlg    -   Window Handle of DialogBox
*                   LPUINT  lpunMaxCstr -   Address of Max. Value of C.String Data
*
**  Return      :   BOOL    TRUE        -   Max. of C.String Data is 0.
*                           FALSE       -   Max. of C.String Data is valid.
*
**  Description :
*		This function gets the maximum number of department pata. It returns
*	TRUE, if the maximum number of department records is equal to invalid(0),
*	and return FALSE otherwise. This function is used for validating the
*	control string data.
*
* ===========================================================================
*/
BOOL    A033ReadMax(HWND hDlg, LPUINT lpunMaxCstr)
{
    BOOL        fRet = FALSE;
    PARAFLEXMEM FlexData;
    WCHAR    szCap[PEP_CAPTION_LEN], /* MessageBox Caption               */
            szMsg[PEP_LOADSTRING_LEN];    /* Data Range Over Error Message    */

    /* ----- Get Max. No. of Control String Records from Prog.#2 ----- */
    FlexData.uchAddress = FLEX_CSTRING_ADR;
    ParaFlexMemRead(&FlexData);

    /* ----- Set Maximum Record No. of Control String to Data Area ----- */
    *lpunMaxCstr = (UINT)FlexData.ulRecordNumber;

    /* ----- Determine Whether Max. of C.String Records is valid or Not ----- */
    if (! FlexData.ulRecordNumber) {
        /* ----- Load String from Resource ----- */
        LoadString(hResourceDll, IDS_PEP_CAPTION_A33, szCap, PEP_STRING_LEN_MAC(szCap));
        LoadString(hResourceDll, IDS_A33_EMPTY, szMsg, PEP_STRING_LEN_MAC(szMsg));

        /* ----- Display Error Message ----- */
        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg, szMsg, szCap, MB_OK | MB_ICONEXCLAMATION);

        fRet = TRUE;
    }
    return (fRet);
}

/*
* ---------------------------------------------------------------------------
*	Synopsis	:	VOID	A033ClearData()
*
*	Input		:	HWND	hDlg	-	Window Handle of DialogBox
*
*	Return		:	VOID	Nothing
*
*	Description	:
*		This function clears all of the control string data that is located in
*	the dialog box. All of the control strings for a given piece of data will
*	be clear out and replaced with NOT USED.
*
* ---------------------------------------------------------------------------
*/
VOID	A033ClearData(HWND hDlg)
{
	BOOL fRet = FALSE;
	CSTRIF CstrData;
	WCHAR	szCap[PEP_CAPTION_LEN],
			szMsg[PEP_LOADSTRING_LEN];
	LPCSTRPARA	lpPara;

	lpPara = lpCstrPara + unListAdr;
	LoadString(hResourceDll, IDS_PEP_CAPTION_A33, szCap, PEP_STRING_LEN_MAC(szCap));
	LoadString(hResourceDll, IDS_A33_CLRMSG, szMsg, PEP_STRING_LEN_MAC(szMsg));

	MessageBeep(MB_ICONEXCLAMATION);
	fRet = MessageBoxPopUp(hDlg, szMsg, szCap, MB_OKCANCEL | MB_ICONQUESTION);
	if( fRet == IDCANCEL){
		return;
	}

	CstrData.usCstrNo = (USHORT)GetDlgItemInt(hDlg, IDD_A33_NO, NULL, FALSE);
	unListNo = CstrData.usCstrNo;

	/* ----- Delete Target control string Data from control string File ----- */
	OpCstrDelete(&CstrData, 0);

	/* ----- deletes current data ----- */
	lpCstrIndtb->usCstrLength = 0;
	memset((LPSTR)lpCstrPara, 0, sizeof(OPCSTR_PARAENTRY) * OP_CSTR_PARA_SIZE);
	unListAdr = 0;

	/* ----- Data update flag FALSE : no up date ----- */
	fUpDate=FALSE;

	A033SetList(hDlg);
}
/*
* ===========================================================================
*	Synopsis	:	VOID	A033DataUp()
*
*	Input		:	HWND	hDlg	-	Window Handle of DialogBox
*
*	Return		:	VOID	Nothing
*
*	Description	:
*		This function switches the position of a given control with the position
*	above it in the control string. It moves the action up along the control
*	string and allows the user to make changes in the actions positioning
*	within the given control string.
* ===========================================================================
*/
VOID A033DataUp(HWND hDlg)
{
	LPCSTRPARA	lpPara, lpTempPara;
	void		*copyPara;
	USHORT		usCstrTempLgth =0;

	lpPara = lpCstrPara + unListAdr;
	usCstrTempLgth = lpCstrIndtb->usCstrLength;

	if(unListAdr == 0){
		return;
	}

	lpTempPara = lpCstrPara + unListAdr - 1;
	copyPara = &lpTempPara;
	memcpy(&copyPara, lpTempPara, sizeof(OPCSTR_PARAENTRY));

	fUpDate = FALSE;

	lpCstrIndtb->usCstrLength = usCstrTempLgth;
	memcpy(lpTempPara, lpPara, sizeof(OPCSTR_PARAENTRY));
	memcpy(lpPara, &copyPara, sizeof(OPCSTR_PARAENTRY));

	unListAdr--;

	A033SetList(hDlg);
}
/*
* ===========================================================================
*	Synopsis	:	VOID	A033DataDown()
*
*	Input		:	HWND	hDlg	-	Window Handle of DialogBox
*
*	Return		:	VOID	Nothing
*
*	Description	:
*		This funtion switches the position of a given control with the position
*	below it in the control string. It moves the action down along the control
*	string and allows the user to make changes in the actions positioning
*	within the given control string.
* ===========================================================================
*/

VOID	A033DataDown(HWND hDlg)
{
	LPCSTRPARA	lpPara, lpTempPara;
	void		*copyPara;
	USHORT		usCstrTempLgth = 0;

	lpPara = lpCstrPara + unListAdr;
	usCstrTempLgth = lpCstrIndtb->usCstrLength;

	if(unListAdr == 199){
		return;
	}

	lpTempPara = lpCstrPara + unListAdr + 1;
	copyPara = &lpTempPara;

	if((lpTempPara->uchMajorCstrData == 0) && (lpTempPara->uchMinorCstrData == 0)){
		return;
	}

	memcpy(&copyPara, lpTempPara,  sizeof(OPCSTR_PARAENTRY));

	fUpDate = FALSE;

	lpCstrIndtb->usCstrLength = usCstrTempLgth;
	memcpy(lpTempPara, lpPara, sizeof(OPCSTR_PARAENTRY));
	memcpy(lpPara, &copyPara, sizeof(OPCSTR_PARAENTRY));

	unListAdr++;

	A033SetList(hDlg);
}
/*
* ===========================================================================
**  Synopsis    :   VOID    A033DelData()
*
**  Input       :   HWND        hDlg    -   Window Handle of DialogBox
*
**  Return      :   No return value.
*
**  Description :
*		This function deletes the current selected control string data and
*	then shifts the remaining control string data below it up.The slected
*	control string data is taken out of the control sting and is replaced by
*	the control string below it and the others are shifted up.
*
* ===========================================================================
*/
VOID    A033DelData(HWND hDlg)
{
    SHORT       wID;
    BOOL        fRet = FALSE;
    WCHAR    szCap[PEP_CAPTION_LEN],  /* MessageBox Caption               */
             szMsg[PEP_LOADSTRING_LEN];     /* Data Range Over Error Message    */
    LPCSTRPARA  lpPara;
    USHORT		usCstrTempLgth = 0;

    /* ----- delete Address for Parameter Buffer ----- */
    lpPara  = lpCstrPara + unListAdr;
    usCstrTempLgth = lpCstrIndtb->usCstrLength;

    if (lpPara->uchMajorCstrData == 0) {
         return;

    }

    /* ----- Load String from Resource ----- */
    LoadString(hResourceDll, IDS_PEP_CAPTION_A33, szCap, PEP_STRING_LEN_MAC(szCap));
    LoadString(hResourceDll, IDS_A33_DELETE, szMsg, PEP_STRING_LEN_MAC(szMsg));

    /* ----- Display Error Message ----- */
    MessageBeep(MB_ICONEXCLAMATION);
    fRet = MessageBoxPopUp(hDlg, szMsg, szCap, MB_OKCANCEL | MB_ICONEXCLAMATION);

    if (fRet == IDCANCEL){
        return;
    }
    /* ----- Data update flag TRUE : set date ----- */
    fUpDate=FALSE;

	lpPara->uchMajorCstrData = 0;
    lpPara->uchMinorCstrData = 0;

	usCstrTempLgth =  usCstrTempLgth - sizeof(OPCSTR_PARAENTRY);
	lpCstrIndtb->usCstrLength = usCstrTempLgth;

	for(wID = (WORD)unListAdr; wID < OP_CSTR_PARA_SIZE - 1; wID++, lpPara++) {
		memmove(lpPara, lpPara + 1, sizeof(OPCSTR_PARAENTRY));
	}

    /* ----- Set Loaded control string Data to ListBox ----- */
    A033SetList(hDlg);

    return;
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A033ChgData()
*
**  Input       :   HWND        hDlg    -   Window Handle of DialogBox
*
**  Return      :   No return value.
*
**  Description :
*          This function changes the control string data with the data that
*	is displayed in the dialog window. This data is moved from lpCstrPara to
*	&CstrData and is stroed there for use elsewhere.
* ===========================================================================
*/
VOID    A033ChgData(HWND hDlg)
{
    BOOL        fRet = FALSE;
    CSTRIF      CstrData;
    WCHAR    szCap[PEP_CAPTION_LEN],  /* MessageBox Caption               */
            szMsg[PEP_LOADSTRING_LEN];     /* Data Range Over Error Message    */

    /* ----- Get Inputed Data from EditText ----- */
    CstrData.usCstrNo = (USHORT)GetDlgItemInt(hDlg, IDD_A33_NO, NULL, FALSE);
    unListNo = CstrData.usCstrNo;
    /* ----- Set Data length ----- */
    CstrData.IndexCstr.usCstrLength = lpCstrIndtb->usCstrLength;
    /* ----- Set The control string data ----- */
    memcpy(&CstrData.ParaCstr, lpCstrPara, sizeof(OPCSTR_PARAENTRY) * OP_CSTR_PARA_SIZE);

    /* ----- Assign Control string Record ----- */
    if (OpCstrAssign(&CstrData, 0) != OP_PERFORM) {
            /* ----- Load String from Resource ----- */
            LoadString(hResourceDll, IDS_PEP_CAPTION_A33, szCap, PEP_STRING_LEN_MAC(szCap));
            LoadString(hResourceDll, IDS_A33_MEMOVR, szMsg, PEP_STRING_LEN_MAC(szMsg));

            /* ----- Display Error Message ----- */
            MessageBeep(MB_ICONEXCLAMATION);
            fRet = MessageBoxPopUp(hDlg, szMsg, szCap, MB_OK | MB_ICONEXCLAMATION);
    }

    /* ----- Data update flag FALSE : no up date ----- */
    fUpDate=FALSE;

    return;
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A033AllocMem()
*
**  Input       :   HWND        hDlg     -   Window Handle of DialogBox
*                   LPHGLOBAL   lphIndtb -   Address of Global Heap Handle
*                   LPHGLOBAL   lphPara  -   Address of Global Heap Handle
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
BOOL    A033AllocMem(HWND hDlg, LPHGLOBAL lphIndtb, LPHGLOBAL lphPara)
{
    BOOL    fRet = FALSE;
    WCHAR    szCaption[PEP_CAPTION_LEN],
            szErrMsg[PEP_LOADSTRING_LEN];

    /* ----- Allocate Memory from Global Heap ----- */

    *lphIndtb = GlobalAlloc(GHND,
                            (sizeof(OPCSTR_INDEXENTRY) * OP_CSTR_INDEX_SIZE));
    *lphPara  = GlobalAlloc(GHND,
                            (sizeof(OPCSTR_PARAENTRY)  * OP_CSTR_PARA_SIZE));

    /* ----- Determine Whether Memory Allocation is Success or Not ----- */

    if ((! *lphIndtb) || (! *lphPara)) {

        /* ----- Load String from Resource ----- */

        LoadString(hResourceDll, IDS_PEP_CAPTION_A33,  szCaption, PEP_STRING_LEN_MAC(szCaption));
        LoadString(hResourceDll, IDS_PEP_ALLOC_ERR,    szErrMsg,  PEP_STRING_LEN_MAC(szErrMsg));

        /* ----- Display Error Message ----- */

        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg,
                   szErrMsg,
                   szCaption,
                   MB_OK | MB_ICONEXCLAMATION);

        fRet = TRUE;

        /* ----- Set the Allocated Area Free ----- */

        if (*lphIndtb) {

            GlobalFree(*lphIndtb);
        }

        if (*lphPara) {

            GlobalFree(*lphPara);
        }

    } else {

        /* ----- Lock the Allocated Area ----- */

        lpCstrIndtb = (LPCSTRINDEX)GlobalLock(*lphIndtb);
        lpCstrPara  = (LPCSTRPARA)GlobalLock(*lphPara);
    }
    return fRet;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A033InitDlg()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function gets current tax mode. And then it loads the initial
*   department data, and sets them to listbox.
* ===========================================================================
*/
VOID    A033InitDlg(HWND hDlg)
{
    WCHAR    szFile[PEP_FILE_BACKUP_LEN];
    WCHAR    szDesc[128];

            /* ----- set initial description, V3.3 ----- */
    LoadString(hResourceDll, IDS_A33_CAPTION, szDesc, PEP_STRING_LEN_MAC(szDesc));
    WindowRedrawText(hDlg, szDesc);

    LoadString(hResourceDll, IDS_A33_NO, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_STRNO, szDesc);
    LoadString(hResourceDll, IDS_A33_NOLEN, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_STRNOLEN, szDesc);
    LoadString(hResourceDll, IDS_A33_DESC, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_STRDESC, szDesc);
    LoadString(hResourceDll, IDS_A33_ADRREC, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_ADDREC, szDesc);
    LoadString(hResourceDll, IDS_A33_TOOL, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_STRTOOL, szDesc);
    LoadString(hResourceDll, IDS_A33_UPSWITCH, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_A33_UPSWITCH, szDesc);
	LoadString(hResourceDll, IDS_A33_DOWNSWITCH, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_A33_DOWNSWITCH, szDesc);
    LoadString(hResourceDll, IDS_A33_ADDCHG, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_ADDCHG, szDesc);
    LoadString(hResourceDll, IDS_A33_DEL, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_DEL, szDesc);
	LoadString(hResourceDll, IDS_A33_CLEARALL, szDesc, PEP_STRING_LEN_MAC(szDesc));
	DlgItemRedrawText(hDlg, IDD_A33_CLEARALL, szDesc);
    LoadString(hResourceDll, IDS_A33_STRING, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_STRING, szDesc);
    LoadString(hResourceDll, IDS_A33_KEYEDPLU, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_KEYEDPLU, szDesc);
    LoadString(hResourceDll, IDS_A33_KEYEDDPT, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_KEYEDDPT, szDesc);
    LoadString(hResourceDll, IDS_A33_PRTMOD, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_PRTMOD, szDesc);
    LoadString(hResourceDll, IDS_A33_ADJ, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_ADJ, szDesc);
    LoadString(hResourceDll, IDS_A33_TENDER, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_TENDER, szDesc);
    LoadString(hResourceDll, IDS_A33_FC, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_FC, szDesc);
    LoadString(hResourceDll, IDS_A33_TTL, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_TTL, szDesc);
    LoadString(hResourceDll, IDS_A33_ADDCHK, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_ADDCHK, szDesc);
    LoadString(hResourceDll, IDS_A33_IDISC, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_IDISC, szDesc);
    LoadString(hResourceDll, IDS_A33_RDISC, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_RDISC, szDesc);
    LoadString(hResourceDll, IDS_A33_TAXMOD, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_TAXMOD, szDesc);
    LoadString(hResourceDll, IDS_A33_AKEY, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_AKEY, szDesc);
    LoadString(hResourceDll, IDS_A33_BKEY, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_BKEY, szDesc);
    LoadString(hResourceDll, IDS_A33_NEWCHK, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_NEWCHK, szDesc);
    LoadString(hResourceDll, IDS_A33_PB, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_PB, szDesc);
    LoadString(hResourceDll, IDS_A33_GCNO, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_GCNO, szDesc);
    LoadString(hResourceDll, IDS_A33_GCTRANS, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_GCTRANS, szDesc);
    LoadString(hResourceDll, IDS_A33_NOPERSON, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_NOPERSON, szDesc);
    LoadString(hResourceDll, IDS_A33_TBLNO, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_TBLNO, szDesc);
    LoadString(hResourceDll, IDS_A33_LINENO, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_LINENO, szDesc);
    LoadString(hResourceDll, IDS_A33_FEED, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_FEED, szDesc);
    LoadString(hResourceDll, IDS_A33_RCTNORCT, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_RCTNORCT, szDesc);
    LoadString(hResourceDll, IDS_A33_TIPSPO, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_TIPSPO, szDesc);
    LoadString(hResourceDll, IDS_A33_DECTIP, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_DECTIP, szDesc);
    LoadString(hResourceDll, IDS_A33_NOSALE, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_NOSALE, szDesc);
    LoadString(hResourceDll, IDS_A33_PO, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_PO, szDesc);
    LoadString(hResourceDll, IDS_A33_ROA, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_ROA, szDesc);
    LoadString(hResourceDll, IDS_A33_CHGCOMP, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_CHGCOMP, szDesc);
    LoadString(hResourceDll, IDS_A33_CANCEL, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_CANCEL, szDesc);
    LoadString(hResourceDll, IDS_A33_VOID, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_VOID, szDesc);
    LoadString(hResourceDll, IDS_A33_EC, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_EC, szDesc);
    LoadString(hResourceDll, IDS_A33_VTRAN, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_VTRAN, szDesc);
    LoadString(hResourceDll, IDS_A33_QTY, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_QTY, szDesc);
    LoadString(hResourceDll, IDS_A33_SHIFT, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_SHIFT, szDesc);
    LoadString(hResourceDll, IDS_A33_NOTYPE, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_NOTYPE, szDesc);
    LoadString(hResourceDll, IDS_A33_PRTDEM, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_PRTDEM, szDesc);
    LoadString(hResourceDll, IDS_A33_CHARTIP, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_CHARTIP, szDesc);
    LoadString(hResourceDll, IDS_A33_PLUNO, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_PLUNO, szDesc);
    LoadString(hResourceDll, IDS_A33_00KEY, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_00KEY, szDesc);
    LoadString(hResourceDll, IDS_A33_AC, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_AC, szDesc);
    LoadString(hResourceDll, IDS_A33_RECFEED, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_RECFEED, szDesc);
    LoadString(hResourceDll, IDS_A33_JOUFEED, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_JOUFEED, szDesc);
    LoadString(hResourceDll, IDS_A33_DECPT, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_DECPT, szDesc);
    LoadString(hResourceDll, IDS_A33_SCALE, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_SCALE, szDesc);
    LoadString(hResourceDll, IDS_A33_OFFTEND, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_OFFTEND, szDesc);
    LoadString(hResourceDll, IDS_A33_RVSPRI, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_RVSPRI, szDesc);
    LoadString(hResourceDll, IDS_A33_PRECASH, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_PRECASH, szDesc);
    LoadString(hResourceDll, IDS_A33_DRCTSHFT, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_DRCTSHFT, szDesc);
    LoadString(hResourceDll, IDS_A33_TIMEIN, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_TIMEIN, szDesc);
    LoadString(hResourceDll, IDS_A33_TIMEOUT, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_TIMEOUT, szDesc);
    LoadString(hResourceDll, IDS_A33_WAIT, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_WAIT, szDesc);
    LoadString(hResourceDll, IDS_A33_KEYEDCPN, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_KEYEDCPN, szDesc);
    LoadString(hResourceDll, IDS_A33_CPNNO, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_CPNNO, szDesc);
    LoadString(hResourceDll, IDS_A33_PAID_REC, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_PAID_REC, szDesc);
    LoadString(hResourceDll, IDS_A33_ALPHA, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_ALPHA, szDesc);
    LoadString(hResourceDll, IDS_A33_LEFTDISPLAY, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_LEFTDISPLAY, szDesc);
    LoadString(hResourceDll, IDS_A33_UP, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_UP, szDesc);
    LoadString(hResourceDll, IDS_A33_CALLSTR, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_CALLSTR, szDesc);
    LoadString(hResourceDll, IDS_A33_PAUSE, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_PAUSE, szDesc);
    LoadString(hResourceDll, IDS_A33_LEFT, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_LEFT, szDesc);
    LoadString(hResourceDll, IDS_A33_DOWN, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_DOWN, szDesc);
    LoadString(hResourceDll, IDS_A33_RIGHT, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_RIGHT, szDesc);
    LoadString(hResourceDll, IDS_A33_BARTENDER, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_BARTENDER, szDesc);
    LoadString(hResourceDll, IDS_A33_SPLIT, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_SPLIT, szDesc);
    LoadString(hResourceDll, IDS_A33_SEAT, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_SEAT, szDesc);
    LoadString(hResourceDll, IDS_A33_ITEMTRANS, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_ITEMTRANS, szDesc);
    LoadString(hResourceDll, IDS_A33_REPEAT, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_REPEAT, szDesc);
    LoadString(hResourceDll, IDS_A33_CURSORVOID, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_CURSORVOID, szDesc);
    LoadString(hResourceDll, IDS_A33_PRICECHECK, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_PRICECHECK, szDesc);
    LoadString(hResourceDll, IDS_A33_P5, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_P5, szDesc);
    LoadString(hResourceDll, IDS_A33_P1, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_P1, szDesc);
    LoadString(hResourceDll, IDS_A33_P2, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_P2, szDesc);
    LoadString(hResourceDll, IDS_A33_P3, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_P3, szDesc);
    LoadString(hResourceDll, IDS_A33_P4, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_P4, szDesc);
    LoadString(hResourceDll, IDS_A33_SEVEN, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_SEVEN, szDesc);
    LoadString(hResourceDll, IDS_A33_EIGHT, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_EIGHT, szDesc);
    LoadString(hResourceDll, IDS_A33_NINE, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_NINE, szDesc);
    LoadString(hResourceDll, IDS_A33_PROG, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_PROG, szDesc);
    LoadString(hResourceDll, IDS_A33_FOUR, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_FOUR, szDesc);
    LoadString(hResourceDll, IDS_A33_FIVE, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_FIVE, szDesc);
    LoadString(hResourceDll, IDS_A33_SIX, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_SIX, szDesc);
    LoadString(hResourceDll, IDS_A33_SUPER, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_SUPER, szDesc);
    LoadString(hResourceDll, IDS_A33_ONE, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_ONE, szDesc);
    LoadString(hResourceDll, IDS_A33_TWO, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_TWO, szDesc);
    LoadString(hResourceDll, IDS_A33_THREE, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_THREE, szDesc);
    LoadString(hResourceDll, IDS_A33_REGS, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_REGS, szDesc);
    LoadString(hResourceDll, IDS_A33_ZERO, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_ZERO, szDesc);
    LoadString(hResourceDll, IDS_A33_NOFUNC, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_NOFUNC, szDesc);
    LoadString(hResourceDll, IDS_A33_CLEAR, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_CLEAR, szDesc);
    LoadString(hResourceDll, IDS_A33_LOCK, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_LOCK, szDesc);
    LoadString(hResourceDll, IDS_A33_OPRINT_A, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_OPRINT_A, szDesc);
    LoadString(hResourceDll, IDS_A33_OPRINT_B, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_OPRINT_B, szDesc);

    LoadString(hResourceDll, IDS_A33_MONEY, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_MONEY, szDesc);
    LoadString(hResourceDll, IDS_A33_ASK, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_ASK, szDesc);
    LoadString(hResourceDll, IDS_A33_LEVEL, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_LEVEL, szDesc);
    LoadString(hResourceDll, IDS_A33_DEPTNUMBER, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_DEPTNUMBER, szDesc);
    LoadString(hResourceDll, IDS_A33_UPCVER, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_UPCVER, szDesc);
    LoadString(hResourceDll, IDS_A33_PRICEKEY, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_PRICEKEY, szDesc);

    /* ### ADD Saratoga (052500) */
    LoadString(hResourceDll, IDS_A33_FSMOD   , szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_FSMOD   , szDesc);
    LoadString(hResourceDll, IDS_A33_WICTRANS, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_WICTRANS, szDesc);
    LoadString(hResourceDll, IDS_A33_WICMOD  , szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_WICMOD  , szDesc);
    LoadString(hResourceDll, IDS_A33_HALO_OVR  , szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_HALO_OVR  , szDesc);
    LoadString(hResourceDll, IDS_A33_ATFOR  , szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_ATFOR  , szDesc);
    LoadString(hResourceDll, IDS_A33_VALIDATION  , szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_VALIDATION  , szDesc);
	LoadString(hResourceDll, IDS_A33_RECEIPT_ID  , szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_RECEIPT_ID  , szDesc);
	LoadString(hResourceDll, IDS_A33_DOC_LAUNCH, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_DOC_LAUNCH, szDesc);		//FSC_DOC_LAUNCH

	// FSC_WINDOW_DISPLAY and FSC_WINDOW_DISMISS
	LoadString(hResourceDll, IDS_A33_WINDOW_DISPLAY, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_WINDOW_DISPLAY, szDesc);
	LoadString(hResourceDll, IDS_A33_WINDOW_DISMISS, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_WINDOW_DISMISS, szDesc);

	LoadString(hResourceDll, IDS_A33_ORDER_DEC, szDesc, PEP_STRING_LEN_MAC(szDesc));
    DlgItemRedrawText(hDlg, IDD_A33_ORDER_DEC, szDesc);		//FSC_ORDER_DEC

    /* ----- Set process initializer flag ----- */

    fInit=TRUE;

    /* ----- Check Data Additon/Append Button ----- */
    CheckDlgButton(hDlg, IDD_A33_ADDREC, fAddRec);

    /* ----- Load Control String File Name from Resorece ----- */
    LoadString(hResourceDll, IDS_FILE_NAME_CSTR, szFile, PEP_STRING_LEN_MAC(szFile));

    /* ----- Create Backup File for Cancel Key ----- */
    PepFileBackup(hDlg, szFile, PEP_FILE_BACKUP);

    /* ----- Set intialzer list number ----- */
    unListNo = 1;

    /* ----- Read Initial C.String Data from Control String File ----- */
    A033ReadData();

    /* ----- Set Loaded control string Data to ListBox ----- */
    A033SetList(hDlg);

    /* ----- Limit Max. Length of Input Data to Each EditText ----- */
    SendDlgItemMessage(hDlg, IDD_A33_NO,  EM_LIMITTEXT, A33_NO_LEN,  0L);

    /* ----- Set Fixed Font ----- */
    PepSetFont(hDlg, IDD_A33_LIST);

    /* ----- Set HighLight Bar to the Top Index of ListBox ----- */
    SendDlgItemMessage(hDlg, IDD_A33_LIST, LB_SETCURSEL, 0, 0L);

    /* ----- Set Control String Number ----- */
    SetDlgItemInt(hDlg, IDD_A33_NO, unListNo, FALSE);
    GetDlgItemInt( hDlg, IDD_A33_NO, &fValidValue, FALSE );
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A033FreeMem()
*
**  Input       :   LPHGLOBAL   lphIndtb -   Address of Global Heap Handle
*                   LPHGLOBAL   lphPara  -   Address of Global Heap Handle
*
**  Return      :   No return value.
*
**  Description :
*       This function frees the allocated memory from global heap.
* ===========================================================================
*/
VOID    A033FreeMem(LPHGLOBAL lphIndtb, LPHGLOBAL lphPara)
{
    /* ----- Unlock the Allocated Area ----- */
    GlobalUnlock(*lphIndtb);
    GlobalUnlock(*lphPara);

    /* ----- Free the Allocated Area ----- */
    GlobalFree(*lphIndtb);
    GlobalFree(*lphPara);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A033GetList()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function sets current selected control string data to each contols.
* ===========================================================================
*/
VOID    A033GetList(HWND hDlg)
{
    BOOL        fCheck;
    LPCSTRPARA  lpPara;

    /* ----- Get Current Selected Index of ListBox ----- */
    unListAdr = (UINT)SendDlgItemMessage(hDlg, IDD_A33_LIST, LB_GETCURSEL, 0, 0L);

    /* ----- Address for Parameter Buffer ----- */
    lpPara = lpCstrPara + unListAdr;
    fCheck = TRUE;

    if (lpPara->uchMajorCstrData < UIE_FSC_BIAS) {
        switch (lpPara->uchMajorCstrData) {
        case FSC_PAUSE:
        case FSC_AN:
        case FSC_CALL_STRING:
        case FSC_CLEAR:
        case FSC_P1:
        case FSC_P2:
        case FSC_P3:
        case FSC_P4:
        case FSC_P5:
        case FSC_MODE:
            break;

        default:
            fCheck = FALSE;
            break;
        }
    }

    /* ----- Enabel / Disable PushButtons ----- */
    fCheck = (( fCheck ) && ( fValidValue )) ? TRUE : FALSE;
    EnableWindow(GetDlgItem(hDlg, IDD_A33_DEL), fCheck);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A033FinDlg()
*
**  Input       :   HWND        hDlg    -   Window Handle of a DialogBox
*                   WORD        wParam  -   User Selected Button ID
*
**  Return      :   No return value.
*
**  Description :
*       This procedure saves inputed control string data, and sets file status
*   flag, if user selects OK button. Otherwise it restores control string file
*   with backup file. And then deletes backup file.
* ===========================================================================
*/
VOID    A033FinDlg(HWND hDlg, WPARAM wParam)
{
    WCHAR    szFile[PEP_FILE_BACKUP_LEN];

    if (LOWORD(wParam) == IDOK) {
        /* ----- Set Control String File Status Flag to ON ----- */
        PepSetModFlag(hwndActMain, PEP_MF_CSTR, 0);
    } else {
        /* ----- Load control string File Name from Resource ----- */
        LoadString(hResourceDll, IDS_FILE_NAME_CSTR, szFile, PEP_STRING_LEN_MAC(szFile));
        /* ----- Restore PLU File with Backup File ----- */
        PepFileBackup(hDlg, szFile, PEP_FILE_RESTORE);
    }

    /* ----- Delete Backup File ----- */
    PepFileDelete();
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A033ChkRng()
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
VOID    A033ChkRng(HWND hDlg, WORD wEditID, UINT unMax)
{
    UINT    unValue;
    short   nMin;
    WCHAR   szCap[PEP_CAPTION_LEN],
            szWork[PEP_LOADSTRING_LEN],
            szMsg[PEP_LOADSTRING_LEN];

    /* ----- Get Inputed Data from EditText ----- */
    unValue = GetDlgItemInt(hDlg, wEditID, NULL, FALSE);

    /* ----- Check Whether Inputed Data is Out of Range or Not ----- */
    if (unValue > unMax) {
        nMin = A33_DATAMIN;

        /* ----- Load Error Message / Caption from Resource ----- */
        LoadString(hResourceDll, IDS_PEP_CAPTION_A33, szCap, PEP_STRING_LEN_MAC(szCap));

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
    } else {
        /* ---- Set control string number ----- */
        unListNo = unValue;

        if (unValue >= A33_DATAMIN) {
            /* ----- Read Initial C.String Data from Control String File ----- */
            A033ReadData();
            /* ----- Set Loaded control string Data to ListBox ----- */
            A033SetList(hDlg);
         }
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A033ReadData()
*
**  Input       :   No input value.
*
**  Return      :   No return value.
*
**  Description :
*       This function reads a control string data from the control string file,
*   and sets it to the data buffer and index buffer.
* ===========================================================================
*/
VOID    A033ReadData()
{
    CSTRIF      CstrData;

    /* ----- Set Initializer List address ----- */
    unListAdr = 0;
    /* ----- Data update flag FALSE : no up date ----- */
    fUpDate=FALSE;

    /* ----- Set Target Control String No. ----- */
    CstrData.usCstrNo = (USHORT)unListNo;

    /* ----- Read Target control string Data from control string File ----- */
    OpCstrIndRead(&CstrData, 0);
    /* ----- Copy Loaded control string Data to Control String Data And Index Buffer ----- */
    lpCstrIndtb->usCstrLength = CstrData.IndexCstr.usCstrLength;

    if (CstrData.IndexCstr.usCstrLength == 0) {
       memset(lpCstrPara, 0, sizeof(OPCSTR_PARAENTRY) * OP_CSTR_PARA_SIZE);
    } else {
        memcpy(lpCstrPara, &CstrData.ParaCstr, sizeof(OPCSTR_PARAENTRY) * OP_CSTR_PARA_SIZE);
    }
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A033ChkUpdate()
*
**  Input       :   HWND        hDlg    -   Window Handle of DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function checks the inputed data is cancel or set.
* ===========================================================================
*/
BOOL    A033ChkUpdate(HWND hDlg)
{
    BOOL    fRet = FALSE;
    WCHAR    szMsg[PEP_LOADSTRING_LEN],     /* Data Range Over Error Message    */
		szCap[PEP_CAPTION_LEN];  /* MessageBox Caption               */

	memset(szCap, 0x00, sizeof(szCap));

    if (fUpDate == TRUE) {
        /* ----- Load String from Resource ----- */
        LoadString(hResourceDll, IDS_PEP_CAPTION_A33, szCap, PEP_STRING_LEN_MAC(szCap));
        LoadString(hResourceDll, IDS_A33_UPDATE, szMsg, PEP_STRING_LEN_MAC(szMsg));

        /* ----- Display Error Message ----- */
        MessageBeep(MB_ICONEXCLAMATION);
        fRet = MessageBoxPopUp(hDlg, szMsg, szCap, MB_OKCANCEL | MB_ICONEXCLAMATION);

        if (fRet == IDCANCEL){
            /* ----- Set Control String Number ----- */
            SetDlgItemInt(hDlg, IDD_A33_NO, unListNo, FALSE);
            return FALSE;
        } else {
            return TRUE;
        }
    }

    return TRUE;
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  A033CtrlDlgProc()
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
*       This is a dialgbox procedure for the Extended Control String.
* ===========================================================================
*/
BOOL WINAPI  A033CtrlDlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  UINT    unVal;

    PEPSPIN PepSpin;
    BOOL    fRet = FALSE;
    WCHAR    szDesc[256];

    switch (wMsg) {
    case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

		/* ----- set initial description, V3.3 ----- */
        LoadString(hResourceDll, IDS_A33_FSCCAPTION, szDesc, PEP_STRING_LEN_MAC(szDesc));
        WindowRedrawText(hDlg, szDesc);

        LoadString(hResourceDll, IDS_A33_FSCNO, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A33_STRFSCNO, szDesc);

        if (wCurBtnID >= IDD_A33_PAUSE) {
            /* ----- Get Max. Value of The Extend Fix Key Code ----- */
            unVal = awExtRngTbl2[A33MAJORFXKCODE(wCurBtnID)];
            /* ----- Load String of Major FSC Data from Resource ----- */
            LoadString(hResourceDll, IDS_A33_INF_PAUSE + A33MAJORFXKCODE(wCurBtnID),
                       szDesc, PEP_STRING_LEN_MAC(szDesc));
         } else {
            /* ----- Get Max. Value of the Extended FSC ----- */
            unVal = awExtRngTbl1[A33MAJORFSCCODE(wCurBtnID)];
            /* ----- Load String of Major FSC Data from Resource ----- */
            LoadString(hResourceDll, IDS_A33_INF_NOFUNC + A33MAJORFSCCODE(wCurBtnID),
                       szDesc, PEP_STRING_LEN_MAC(szDesc));
        }

        /* ----- Set Max. Value Number to StaticText ----- */
		switch (wCurBtnID) {
			case IDD_A33_CANCEL:
			case IDD_A33_CURSORVOID:
			case IDD_A33_VTRAN:
			case IDD_A33_ORDER_DEC:
				LoadString(hResourceDll, IDS_A33_FSCLEN_0, szDesc, PEP_STRING_LEN_MAC(szDesc));
				DlgItemRedrawText(hDlg, IDD_A33_STRFSCLEN, szDesc);
				if (unVal > 0)
					SetDlgItemInt(hDlg, IDD_A33_EXTRANGE, unVal - 1, FALSE);
				else
					SetDlgItemInt(hDlg, IDD_A33_EXTRANGE, unVal, FALSE);
				break;

			default:
				LoadString(hResourceDll, IDS_A33_FSCLEN, szDesc, PEP_STRING_LEN_MAC(szDesc));
				DlgItemRedrawText(hDlg, IDD_A33_STRFSCLEN, szDesc);
				SetDlgItemInt(hDlg, IDD_A33_EXTRANGE, unVal, FALSE);
				break;
		}

        /* ----- Set Loaded String to StaticText ----- */
        DlgItemRedrawText(hDlg, IDD_A33_EXTMNEMO, szDesc);

        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j=IDD_A33_EXTMNEMO; j<=IDD_A33_EXTRANGE; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			for(j=IDD_A33_STRFSCNO; j<=IDD_A33_STRFSCLEN; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);

		}
		return FALSE;

    case WM_VSCROLL:
			PepSpin.lMax       = (long)unVal;
			switch (wCurBtnID) {
				case IDD_A33_CANCEL:
				case IDD_A33_CURSORVOID:
				case IDD_A33_VTRAN:
				case IDD_A33_ORDER_DEC:
					if (unVal > 0)
						PepSpin.lMax = (long)unVal - 1;
					PepSpin.lMin = (long)0;
					break;

				default:
					PepSpin.lMin = (long)A33_EXT_MIN;
					break;
			}

			PepSpin.nStep      = A33_SPIN_STEP;
			PepSpin.nTurbo     = A33_SPIN_TURBO;
			PepSpin.fSpinStyle = PEP_SPIN_NOROLL;

			fRet = PepSpinProc(hDlg, wParam, IDD_A33_EXTFSC, (LPPEPSPIN)&PepSpin);
        return (fRet);

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDD_A33_EXTFSC:    /* Inputed Extended FSC Data    */
            if (HIWORD(wParam) == EN_CHANGE) {
                /* ----- Check Inputed Value with Data Range ----- */
                A33ChkExtRng(hDlg, unVal);
                return TRUE;
            }
            return FALSE;

        case IDOK:              /* Pressed OK Button        */
        case IDCANCEL:          /* Pressed Cancel Button    */
            if (HIWORD(wParam) == BN_CLICKED) {
                if (LOWORD(wParam) == IDOK) {
                    /* ----- Get Extended FSC Data ----- */
                    if (A33GetExtFsc(hDlg, wCurBtnID, unVal)) {
                        /* user inputed 0 value */
                        return TRUE;
                    }

                    /* ----- Set Return Status (OK Button is Selected) ----- */
                    fRet = TRUE;
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
**  Synopsis    :   VOID    A33ChkExtRng()
*
**  Input       :   HWND    hDlg    -   Window Handle of Extended C.String Dialog
*                   UINT    unVal   -   Maximum Value of Extended C.String
*
**  Return      :   None
*
**  Description :
*       This procedure checkes inputed Extended C.String Data with data range.
* ===========================================================================
*/
VOID    A33ChkExtRng(HWND hDlg, UINT unVal)
{
    UINT    unData;
    WCHAR   szCap[PEP_CAPTION_LEN], /* MessageBox Caption               */
            szWork[PEP_LOADSTRING_LEN + 20],   /* Data Range Over Error Message    */
            szErr[PEP_LOADSTRING_LEN];    /* Data Range Over Error Message    */

    /* ----- Get Extended C.String Data from EditText ----- */
    unData = GetDlgItemInt(hDlg, IDD_A33_EXTFSC, NULL, FALSE);

    if (unData > unVal) {       /* Out of Data Range    */
        /* ----- Load Error Message from Resource ----- */
        LoadString(hResourceDll, IDS_PEP_CAPTION_A33, szCap, PEP_STRING_LEN_MAC(szCap));
        LoadString(hResourceDll, IDS_PEP_OVERRANGE,   szErr, PEP_STRING_LEN_MAC(szErr));

        /* ----- Make Error Message with Maximum Value ----- */
        wsPepf(szWork, szErr, A33_EXT_MIN, unVal);

        /* ----- Display Error Message ----- */
        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg, szWork, szCap, MB_OK | MB_ICONEXCLAMATION);

        /* ----- Undo Data Input ----- */
        SendDlgItemMessage(hDlg, IDD_A33_EXTFSC, EM_UNDO, 0, 0L);
    }
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A33GetExtFsc()
*
**  Input       :   HWND    hDlg    -   Window handle of Extended C.String Dialog
*                   UINT    unVal   -   Maximum Value of Data Range
*
**  Return      :   TRUE            -   User Inputed 0 Value.
*                   FALSE           -   No Error.
*
**  Description :
*       This procedure gets the Inputed Extended C.String Data, and then checks
*   it with data range. If inputed value is invalid, returns TRUE. Otherwise
*   returns FALSE.
* ===========================================================================
*/
BOOL    A33GetExtFsc(HWND hDlg, WORD wCurBtnID, UINT unVal)
{
    BOOL    fRet, fTrans;
    UINT    unExtData;       /* Inputed Extended Data        */
    WCHAR   szCap[PEP_CAPTION_LEN], /* MessageBox Caption               */
            szErr[PEP_LOADSTRING_LEN],    /* Data Range Over Error Message    */
            szWork[PEP_LOADSTRING_LEN + 20];   /* Data Range Over Error Message    */

    /* ----- Get Inputed Extended FSC Data from EditText ----- */
    unExtData = GetDlgItemInt(hDlg, IDD_A33_EXTFSC, (LPBOOL)&fTrans, FALSE);

	switch (wCurBtnID) {
		case IDD_A33_CANCEL:
		case IDD_A33_CURSORVOID:
		case IDD_A33_VTRAN:
		case IDD_A33_ORDER_DEC:
			break;

		default:
			// if not a zero based item then make sure value is non-zero
			fTrans = fTrans && (unExtData > 0);
			break;
	}

    if (fTrans == 0) {         /* Inputed Data is 0 */
        /* ----- Load Error Message from Resource ----- */
        LoadString(hResourceDll, IDS_PEP_CAPTION_A33, szCap, PEP_STRING_LEN_MAC(szCap));
        LoadString(hResourceDll, IDS_PEP_OVERRANGE,   szErr, PEP_STRING_LEN_MAC(szErr));

        /* ----- Make Error Message with Maximum Value ----- */
        wsPepf(szWork, szErr, A33_EXT_MIN, unVal);

        /* ----- Display Error Message ----- */
        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg, szWork, szCap, MB_OK | MB_ICONEXCLAMATION);

        /* ----- Set High-light to EditText ----- */
        SendDlgItemMessage(hDlg, IDD_A33_EXTFSC, EM_SETSEL, 0, MAKELONG(0, -1));
        /* ----- Set Focus to EditText ----- */
        SetFocus(GetDlgItem(hDlg, IDD_A33_EXTFSC));

        fRet = TRUE;
    } else {
        /* ----- Set Normal return and  Extend data ----- */
        fExtFlg   = TRUE;
        uchRetExtData = (UCHAR)unExtData;

        fRet = FALSE;
    }

    return (fRet);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A033SetExtFsc()
*
**  Input       :   HWND    hDlg    -   Window handle of Extended C.String Dialog
*
**  Return      :   No return value.
*
*
**  Description :
*       This procedure set the Inputed Extended C.String Data.
* ===========================================================================
*/
VOID    A033SetExtFsc(HWND hDlg)
{
    switch (wCurBtnID) {
    case IDD_A33_PAUSE:
        /* ----- Change Selected Button Style ----- */
        A033SetData(hDlg, (UCHAR)FSC_PAUSE, uchRetExtData);
        break;

    case IDD_A33_CALLSTR:
        /* ----- Change Selected Button Style ----- */
        A033SetData(hDlg, (UCHAR)FSC_CALL_STRING, uchRetExtData);
        break;

    case IDD_A33_KEYEDPLU:
    case IDD_A33_KEYEDDPT:
    case IDD_A33_PRTMOD:
    case IDD_A33_ADJ:
    case IDD_A33_TENDER:
    case IDD_A33_FC:
    case IDD_A33_TTL:
    case IDD_A33_ADDCHK:
    case IDD_A33_IDISC:
    case IDD_A33_RDISC:
    case IDD_A33_TAXMOD:
    case IDD_A33_CHARTIP:       /* V3.3 */
    case IDD_A33_PRECASH:
    case IDD_A33_DRCTSHFT:
    case IDD_A33_KEYEDCPN:
    case IDD_A33_PRTDEM:
    case IDD_A33_SEAT:
    case IDD_A33_ASK:           /* NCR2172 */
    case IDD_A33_LEVEL:         /* NCR2172 */
	case IDD_A33_CANCEL:
	case IDD_A33_CURSORVOID:
	case IDD_A33_VTRAN:
    case IDD_A33_ORDER_DEC:
		/* ----- Change Selected Button Style ----- */
        A033SetData(hDlg, (UCHAR)(wCurBtnID - IDD_A33_NOFUNC + UIE_FSC_BIAS), uchRetExtData);
        break;
    }
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A033SetData()
*
**  Input       :   HWND        hDlg    -   Window Handle of DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This function deketes current selected control string dara.
* ===========================================================================
*/
VOID    A033SetData(HWND hDlg, UCHAR  uchMajorData, UCHAR  uchMinorData)
{
    BOOL    fRet = FALSE;
    BOOL    fCheck;
    LPCSTRPARA  lpPara;
    WCHAR    szCap[PEP_CAPTION_LEN],  /* MessageBox Caption               */
            szMsg[PEP_LOADSTRING_LEN];     /* Data Range Over Error Message    */

    if (unListNo < A33_DATAMIN) {
        return;
    }

    /* ----- delete Address for Parameter Buffer ----- */
    lpPara  = lpCstrPara + unListAdr;

    if ((unListAdr != 0) && ((lpPara - 1)->uchMajorCstrData == 0)) {
         /* ----- Load String from Resource ----- */
        LoadString(hResourceDll, IDS_PEP_CAPTION_A33, szCap, PEP_STRING_LEN_MAC(szCap));
        LoadString(hResourceDll, IDS_A33_BRNDATA, szMsg, PEP_STRING_LEN_MAC(szMsg));

        /* ----- Display Error Message ----- */
        MessageBeep(MB_ICONEXCLAMATION);
        fRet = MessageBoxPopUp(hDlg, szMsg, szCap, MB_OK | MB_ICONEXCLAMATION);
        return;
    }

    /* ----- update control string data ----- */
    lpPara->uchMajorCstrData = uchMajorData;
    lpPara->uchMinorCstrData = uchMinorData;

    /* ----- Data update flag TRUE : set date ----- */
    fUpDate=TRUE;

    /* ----- change data of listbox ----- */
    A033ChgList(hDlg, lpPara);

    if (fAddRec == TRUE) {
        unListAdr++;
        if (unListAdr == OP_CSTR_PARA_SIZE){
            unListAdr = (OP_CSTR_PARA_SIZE - 1);
        } else {
            if (lpCstrIndtb->usCstrLength < (unListAdr * sizeof(OPCSTR_PARAENTRY))) {
               lpCstrIndtb->usCstrLength += sizeof(OPCSTR_PARAENTRY);
            }
        }
    }

    /* ----- Address for Parameter Buffer ----- */
    lpPara = lpCstrPara + unListAdr;

    fCheck = TRUE;

    if (lpPara->uchMajorCstrData < UIE_FSC_BIAS) {
        switch (lpPara->uchMajorCstrData) {
        case FSC_PAUSE:
        case FSC_AN:
        case FSC_CALL_STRING:
        case FSC_CLEAR:
        case FSC_P1:
        case FSC_P2:
        case FSC_P3:
        case FSC_P4:
        case FSC_P5:
        case FSC_MODE:
            break;

        default:
            fCheck = FALSE;
            break;
        }
    }

    /* ----- Enabel / Disable PushButtons ----- */
    fCheck = (( fCheck ) && ( fValidValue )) ? TRUE : FALSE;
    EnableWindow(GetDlgItem(hDlg, IDD_A33_DEL), fCheck);

    /* ----- Set cursor selected ----- */
    SendDlgItemMessage(hDlg, IDD_A33_LIST, LB_SETCURSEL, (WPARAM)unListAdr, 0L);

    return;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A033SetList()
*
**  Input       :   HWND        hDlg        -   Window Handle of a DialogBox
*                   LPCSTRPARA  lpCstr      -
*
**  Return      :   No return value.
*
**  Description :
*       This function Set the configulation of a control string listbox.
* ===========================================================================
*/
VOID    A033SetList(HWND hDlg)
{
    LPCSTRPARA  lpPara;
    UINT        wID;
    USHORT      usI;
    BOOL        fExt, fCheck, fSerChk = FALSE;
    WCHAR       szWork[132], szDesc[132];

    /* ----- Set ListBox Data ----- */
    for (usI = 0, lpPara = lpCstrPara; usI < OP_CSTR_PARA_SIZE; usI++,lpPara++) {
        fExt = FALSE;
        if (usI < (lpCstrIndtb->usCstrLength / sizeof(OPCSTR_PARAENTRY))) {
            if (lpPara->uchMajorCstrData >= UIE_FSC_BIAS) {
                switch (lpPara->uchMajorCstrData - UIE_FSC_BIAS + IDD_A33_NOFUNC) {
                case IDD_A33_KEYEDPLU:
                case IDD_A33_KEYEDDPT:
                case IDD_A33_PRTMOD:
                case IDD_A33_ADJ:
                case IDD_A33_TENDER:
                case IDD_A33_FC:
                case IDD_A33_TTL:
                case IDD_A33_ADDCHK:
                case IDD_A33_IDISC:
                case IDD_A33_RDISC:
                case IDD_A33_TAXMOD:
                case IDD_A33_PRECASH:
                case IDD_A33_DRCTSHFT:
                case IDD_A33_KEYEDCPN:
                case IDD_A33_PRTDEM:
                case IDD_A33_SEAT:
                case IDD_A33_ASK:           /* NCR2172 */
                case IDD_A33_LEVEL:         /* NCR2172 */
				case IDD_A33_CANCEL:
				case IDD_A33_CURSORVOID:
				case IDD_A33_VTRAN:
				case IDD_A33_ORDER_DEC:
                    wID  = (IDS_A33_DSP_NOFUNC + lpPara->uchMajorCstrData - UIE_FSC_BIAS);
                    fExt = TRUE;
                    break;

                case IDD_A33_CHARTIP:

                /* ----- Old Charge Tips No. -> New Charge Tips No. (0 -> 1) ,V3.3 ----- */
					//SR506
					//Charge Tips must have previously been able to have the value '0'
					//Currently, charge tips can range from 1 to 3. If the value of the
					//Charge Tip is 0 then set it to 1 and notify the user, otherwise
					//leave it as the value it is
                    if (0 == lpPara->uchMinorCstrData) {
						lpPara->uchMinorCstrData = 1;
						fUpDate = TRUE;
					}

                    wID  = (IDS_A33_DSP_NOFUNC + lpPara->uchMajorCstrData - UIE_FSC_BIAS);
                    fExt = TRUE;
                    break;

                case IDD_A33_SERVNO:
					/* ----- Server key is not used. Display Warning Message flag on. ,V3.3 ----- */
                    fSerChk = TRUE;

                    /* ----- Old Server Key -> No. Function Key ----- */
                    lpPara->uchMajorCstrData = (UCHAR)FSC_NO_FUNC;
                    wID = (IDS_A33_DSP_NOFUNC + lpPara->uchMajorCstrData - UIE_FSC_BIAS);
                    fUpDate = TRUE;
                    break;

                default:
                    wID  = (IDS_A33_DSP_NOFUNC + lpPara->uchMajorCstrData - UIE_FSC_BIAS);
                    break;
                }
            } else {
                switch (lpPara->uchMajorCstrData) {
                case FSC_PAUSE:
                    wID = IDS_A33_DSP_PAUSE;
                    fExt = TRUE;
                    break;

                case FSC_AN:
                    wID = IDS_A33_DSP_ASCII;
                    fExt = TRUE;
                    break;

                case FSC_CALL_STRING:
                    wID = IDS_A33_DSP_CALLSTR;
                    fExt = TRUE;
                    break;

                case FSC_CLEAR:
                    wID = IDS_A33_DSP_CLEAR;
                    break;

                case FSC_P1:
                    wID = IDS_A33_DSP_P1;
                    break;

                case FSC_P2:
                    wID = IDS_A33_DSP_P2;
                    break;

                case FSC_P3:
                    wID = IDS_A33_DSP_P3;
                    break;

                case FSC_P4:
                    wID = IDS_A33_DSP_P4;
                    break;

                case FSC_P5:
                    wID = IDS_A33_DSP_P5;
                    break;

                case FSC_MODE:
                    wID = (IDS_A33_DSP_LOCK - A33_MODE_EXT + (WORD)lpPara->uchMinorCstrData);
                    break;

                default:
                    wID = IDS_A33_DSP_NOUSE;
                    break;
                }
            }
        } else {
            wID = IDS_A33_DSP_NOUSE;
        }

        if (fExt == TRUE) {
            /* ----- Load String from Resouce ----- */
            LoadString(hResourceDll, wID, szDesc, PEP_STRING_LEN_MAC(szDesc));

            if ( wID == IDS_A33_DSP_ASCII ) {
                if ((WCHAR)lpPara->uchMinorCstrData == PEP_MNEMO_DOUBLE) {
                    wsPepf(szWork, szDesc, (WCHAR)PEP_MNEMO_CTRL);
                } else {
                    wsPepf(szWork, szDesc, (WCHAR)lpPara->uchMinorCstrData);
                }
            } else {
                /* ----- Make String with Extended FSC Data ----- */
                wsPepf(szWork, szDesc, (short)lpPara->uchMinorCstrData);
            }
        } else {
            /* ----- Load String from Resouce ----- */
            LoadString(hResourceDll, wID, szWork, PEP_STRING_LEN_MAC(szWork));
        }

        /* ----- Create Description for Insert ListBox ----- */
        wsPepf(szDesc, WIDE("%3u : %s"), (usI+1), szWork);

        if (fInit == TRUE) {
            /* ----- Set Created Description to ListBox ----- */
            DlgItemSendTextMessage(hDlg, IDD_A33_LIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szDesc));
        } else {
            /* ----- Delete Old String from Menu-ListBox ----- */
            DlgItemSendTextMessage(hDlg, IDD_A33_LIST, LB_DELETESTRING, (WORD)usI, 0L);
            /* ----- Set New String to Menu-ListBox ----- */
            DlgItemSendTextMessage(hDlg, IDD_A33_LIST, LB_INSERTSTRING, (WORD)usI, (LPARAM)(szDesc));
        }
    }

    /* ----- Warning message to change server key, V3.3 ----- */
    if (fSerChk) {
        /* ----- Load Caution Message from Resource ----- */
        LoadString(hResourceDll, IDS_PEP_CAPTION_A33, szWork, PEP_STRING_LEN_MAC(szWork));
        LoadString(hResourceDll, IDS_A33_CONVSERKEY, szDesc, PEP_STRING_LEN_MAC(szDesc));

        /* ----- Display Error Message ----- */
        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hDlg, szDesc, szWork, MB_OK | MB_ICONEXCLAMATION);
    }

    /* ----- Set cursor selected ----- */
    SendDlgItemMessage(hDlg, IDD_A33_LIST, LB_SETCURSEL, (WPARAM)unListAdr, 0L);

    /* ----- Set Process initializer flag FALSE ----- */
    fInit=FALSE;

    /* ----- Enabel / Disable PushButtons ----- */
    fCheck = (lpCstrIndtb->usCstrLength == 0) ? FALSE : TRUE;

    fCheck = (( fCheck ) && ( fValidValue )) ? TRUE : FALSE;
    EnableWindow(GetDlgItem(hDlg, IDD_A33_ADDCHG), fCheck);

    /* ----- Address for Parameter Buffer ----- */
    lpPara = lpCstrPara + unListAdr;

    fCheck = TRUE;

    if (lpPara->uchMajorCstrData < UIE_FSC_BIAS) {
        switch (lpPara->uchMajorCstrData) {
        case FSC_PAUSE:
        case FSC_AN:
        case FSC_CALL_STRING:
        case FSC_CLEAR:
        case FSC_P1:
        case FSC_P2:
        case FSC_P3:
        case FSC_P4:
        case FSC_P5:
        case FSC_MODE:
            break;

        default:
            fCheck = FALSE;
            break;

        }
    }

    /* ----- Enabel / Disable PushButtons ----- */
    fCheck = (( fCheck ) && ( fValidValue )) ? TRUE : FALSE;
    EnableWindow(GetDlgItem(hDlg, IDD_A33_DEL), fCheck);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A033ChgList()
*
**  Input       :   HWND        hDlg        -   Window Handle of a DialogBox
*                   LPCSTRPARA  lpCstr      -
*
**  Return      :   No return value.
*
**  Description :
*       This function Set the configulation of a control string listbox.
* ===========================================================================
*/
VOID    A033ChgList(HWND hDlg, LPCSTRPARA  lpCstr)
{
    UINT        wID;
    BOOL        fExt;
    WCHAR       szWork[132], szDesc[132];

    /* ----- Set ListBox Data ----- */
    fExt = FALSE;
    if (lpCstr->uchMajorCstrData >= UIE_FSC_BIAS) {
        switch (lpCstr->uchMajorCstrData - UIE_FSC_BIAS + IDD_A33_NOFUNC) {
        case IDD_A33_KEYEDPLU:
        case IDD_A33_KEYEDDPT:
        case IDD_A33_PRTMOD:
        case IDD_A33_ADJ:
        case IDD_A33_TENDER:
        case IDD_A33_FC:
        case IDD_A33_TTL:
        case IDD_A33_ADDCHK:
        case IDD_A33_IDISC:
        case IDD_A33_RDISC:
        case IDD_A33_TAXMOD:
        case IDD_A33_CHARTIP:       /* V3.3 */
        case IDD_A33_PRECASH:
        case IDD_A33_DRCTSHFT:
        case IDD_A33_KEYEDCPN:
        case IDD_A33_PRTDEM:
        case IDD_A33_SEAT:
        case IDD_A33_ASK:           /* NCR2172 */
        case IDD_A33_LEVEL:         /* NCR2172 */
		case IDD_A33_CANCEL:
		case IDD_A33_CURSORVOID:
		case IDD_A33_VTRAN:
		case IDD_A33_ORDER_DEC:
            wID  = (IDS_A33_DSP_NOFUNC + lpCstr->uchMajorCstrData - UIE_FSC_BIAS);
            fExt = TRUE;
            break;

        default:
            wID  = (IDS_A33_DSP_NOFUNC + lpCstr->uchMajorCstrData - UIE_FSC_BIAS);
            break;
        }
    } else {
        switch (lpCstr->uchMajorCstrData) {
        case FSC_PAUSE:
            wID = IDS_A33_DSP_PAUSE;
            fExt = TRUE;
            break;

        case FSC_AN:
            wID = IDS_A33_DSP_ASCII;
            fExt = TRUE;
            break;

        case FSC_CALL_STRING:
            wID = IDS_A33_DSP_CALLSTR;
            fExt = TRUE;
            break;

        case FSC_CLEAR:
            wID = IDS_A33_DSP_CLEAR;
            break;

        case FSC_P1:
            wID = IDS_A33_DSP_P1;
            break;

        case FSC_P2:
            wID = IDS_A33_DSP_P2;
            break;

        case FSC_P3:
            wID = IDS_A33_DSP_P3;
            break;

        case FSC_P4:
            wID = IDS_A33_DSP_P4;
            break;

        case FSC_P5:
            wID = IDS_A33_DSP_P5;
            break;

        case FSC_MODE:
            wID = (IDS_A33_DSP_LOCK - A33_MODE_EXT + (WORD)lpCstr->uchMinorCstrData);
            break;

        default:
            wID = IDS_A33_DSP_NOUSE;
            break;
        }
    }

    if (fExt == TRUE) {
        /* ----- Load String from Resouce ----- */
        LoadString(hResourceDll, wID, szDesc, PEP_STRING_LEN_MAC(szDesc));

        if ( wID == IDS_A33_DSP_ASCII ) {
            if ((char)lpCstr->uchMinorCstrData == PEP_MNEMO_DOUBLE) {
                wsPepf(szWork, szDesc, (WCHAR)PEP_MNEMO_CTRL);
            } else {
                wsPepf(szWork, szDesc, (WCHAR)lpCstr->uchMinorCstrData);
            }
        } else {
            /* ----- Make String with Extended FSC Data ----- */
            wsPepf(szWork, szDesc, (short)lpCstr->uchMinorCstrData);
        }
    } else {
            /* ----- Load String from Resouce ----- */
            LoadString(hResourceDll, wID, szWork, PEP_STRING_LEN_MAC(szWork));
    }

    /* ----- Enabel PushButtons ----- */
    EnableWindow(GetDlgItem(hDlg, IDD_A33_ADDCHG), fValidValue);

    /* ----- Create Description for Insert ListBox ----- */
    wsPepf(szDesc, WIDE("%3u : %s"), (unListAdr+1), szWork);

    /* ----- Delete Old String from Menu-ListBox ----- */
    DlgItemSendTextMessage(hDlg, IDD_A33_LIST, LB_DELETESTRING, (WORD)unListAdr, 0L);

    /* ----- Set New String to Menu-ListBox ----- */
    DlgItemSendTextMessage(hDlg, IDD_A33_LIST, LB_INSERTSTRING, (WORD)unListAdr, (LPARAM)(szDesc));

}


/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  A033DlgProc()
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
*       This is a dialogbox procedure for  alpha string entry.
* ===========================================================================
*/
BOOL WINAPI A033StrDlgProc( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
    static LPWSTR    lpszSaveBuff;
    WCHAR            szInputedStr[ (A33_ALPHA_STR_LEN + 1)* 2 ];
    WCHAR    szDesc[128 * 2];

    switch ( wMsg ) {

    case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

		/* ----- set initial description, V3.3 ----- */
        LoadString(hResourceDll, IDS_A33_MNEMOCAPTION, szDesc, PEP_STRING_LEN_MAC(szDesc));
        WindowRedrawText(hDlg, szDesc);
        LoadString(hResourceDll, IDS_A33_MNEMO, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A33_STRMNEMO, szDesc);
        LoadString(hResourceDll, IDS_A33_MNEMOMAX, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A33_STRMNEMOMAX, szDesc);

        PepSetFont( hDlg, IDD_A33_STREDIT );
        SendDlgItemMessage( hDlg, IDD_A33_STREDIT, EM_LIMITTEXT, A33_ALPHA_STR_LEN, 0L );

        lpszSaveBuff = ( LPWSTR )lParam;

        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j=IDD_A33_STRMNEMO; j<=IDD_A33_STRMNEMOMAX; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDD_A33_STREDIT, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_COMMAND:
        switch ( LOWORD(wParam) ) {
        case IDOK:
            memset(szInputedStr, 0x00, sizeof( szInputedStr ));
            DlgItemGetText( hDlg, IDD_A33_STREDIT, szInputedStr, sizeof( szInputedStr ));

            /* ----- Replace Double Key Character ('~' -> 0x12) ----- */
            PepReplaceMnemonic(( WCHAR * )szInputedStr, ( WCHAR * )lpszSaveBuff, (A33_ALPHA_STR_LEN ), PEP_MNEMO_WRITE );

            EndDialog( hDlg, LOWORD(wParam) );
            return TRUE;

        case IDCANCEL:
            EndDialog( hDlg, LOWORD(wParam) );
            return TRUE;

        default:
            break;
        }
        break;

    default:
        break;
    }
    return FALSE;
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A033SaveString()
*
**  Input       :   HWND    hDlg        -   Window Handle of DialogBox
*                   LPSTR   lpszSource  -   address of source string
*
**  Return      :   No return value.
*
**  Description :
*       This function save inputed alpha string to static buffer, and
*   updates list box.
* ===========================================================================
*/
VOID A033SaveString( HWND hDlg, LPWSTR lpszSource )
{
    WCHAR    chI;

    chI = 0;

    while (( *( lpszSource + chI ) != 0 ) && ( chI < A33_ALPHA_STR_LEN )) {
        A033SetData( hDlg, FSC_AN, ( UCHAR )*( lpszSource + chI ));
        chI++;
    }
}


/* ===== End of A033.C ===== */
