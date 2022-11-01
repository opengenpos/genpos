/*
* ---------------------------------------------------------------------------
* Title         :   File Migration Module
* Category      :   File, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   FILEMIGR.C
* Copyright (C) :   1995, NCR Corp. E&M OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract : This module provides the following functions
*               FileMigrate():   migrate old version file (Ver 1.0) to hotel file
* ---------------------------------------------------------------------------
* Update Histories
*
*   Date    : Ver.Rev. :   Name     : Description
* Dec-13-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
* Apr-24-95 : 03.00.00 : H.Ishida   : R3.0
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
#include    <windows.h>
#include    <string.h>
#include    <ecr.h>
#include    <plu.h>
#include    <paraequ.h>
#include    <para.h>

/* NCR2172 */
#include <pararam.h>

#include    "csop.h"
#include    <cscas.h>
#include    <csetk.h>

#include    "pep.h"
#include    "pepcomm.h"
#include    "pepflag.h"
#include    "trans.h"
#include    "file.h"
#include    "filel.h"
#include    "filemigr.h"
#include    "filemgtb.c"

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
/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/

/*
* ===========================================================================
**  Synopsis:   BOOL    FileMigrate()
*
*   Input   :   HWND    hDlg   - handle of dialogbox procedure
*               int     iFile  - handle of open file
**
*   Return  :   TRUE           - user process is executed
*               FALSE          - default process is expected
*
**  Description:
*       This procedure migrate old version file to enhance file.
* ===========================================================================
*/
BOOL    FileMigrate(HWND hDlg, HANDLE iFile)
{
    int     iResult;
    DLGPROC InstDlgProc;
    WCHAR    szErrMsg[FILE_MAXLEN_TEXT];

    /* store handle */
    iFileHandle = iFile;

    /* create dialog    */
    InstDlgProc = MakeProcInstance(FileMigrateDlgProc, hPepInst);
    if ((iResult = DialogBoxPopup(hResourceDll,
                             MAKEINTRESOURCEW(IDD_FILE_DLG_MIGRATE),
                             hDlg,
                             InstDlgProc)) == -1) {
        MessageBeep(MB_ICONEXCLAMATION);    /* make a tone  */
        LoadString(hResourceDll,
                   IDS_FILE_ERR_CREATE, szErrMsg, PEP_STRING_LEN_MAC(szErrMsg));
        MessageBoxPopUpFileCaption(hDlg,            /* output error message */
                   szErrMsg,
                   MB_ICONEXCLAMATION | MB_OK);
        FreeProcInstance(InstDlgProc);
        return FALSE;
    }
    FreeProcInstance(InstDlgProc);

    if (iResult == TRUE) {
        return TRUE;
    } else {
        return FALSE;
    }

}


/*
* ===========================================================================
**  Synopsis:   BOOL    WINAPI  FileMigrateDlgProc()
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
*       This is a dialgbox procedure for migration.
* ===========================================================================
*/
BOOL WINAPI FileMigrateDlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  RECT        rRect;              /* dialog box area      */
    static  FILEMIGRIDX Index;              /* checkbox list index  */
    static  FILEMIGRCHK Check;              /* select check item    */

    WCHAR        szFname[FILE_MAXLEN_TEXT];
    HGLOBAL     hMem;
    VOID FAR    *lpParam;
    VOID FAR    *lpBufStart;
    /*** NCR2172
    VOID FAR    *lpBufEnd;
    ***/
    VOID FAR    *lpWork;
    FSCDATA FAR *lpFsc = 0;
    USHORT  FAR *lpPluKeySouc;
    USHORT  FAR *lpPluKeyDes;
    WORD        wI;
    WORD        wJ;
    WORD        wLoop;
    HDC         hDC;
    WCHAR        szWork[FILE_LEN_ID];
    FHEADEROLD  Fheaderold;
	DWORD		bytesRead, fileError;

    switch (wMsg) {
    case WM_INITDIALOG:

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- make a tone for questionm ----- */

        MessageBeep(MB_ICONQUESTION);

        /* ----- get client area ----- */

        GetWindowRect(hDlg, &rRect);

        /* ----- show dialogbox as small (hide parameter list) ----- */

        SetWindowPos(hDlg,
                     (HWND)NULL,
                     0,
                     0,
                     (short)(rRect.right  - rRect.left),
                     (short)((rRect.bottom - rRect.top) * FILE_MIGR_MULTIPLY / FILE_MIGR_DIVIDE),
                     SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

        /* ----- set checkbox status all on ----- */

        memset((LPSTR)&Check, TRUE, sizeof(FILEMIGRCHK));

        return TRUE;

    case WM_PAINT:
    {
        PAINTSTRUCT Paint;

        /* ----- get device context handle ----- */
        hDC = BeginPaint(hDlg, &Paint);

        /* ----- draw question icon ----- */
        DrawIcon(hDC, FILE_MIGR_ICONX, FILE_MIGR_ICONY, LoadIcon(NULL, IDI_QUESTION));

        /* ----- release device context handle ----- */
        EndPaint(hDlg, &Paint);
    }

        return TRUE;

    case WM_VSCROLL:
        if (GetDlgCtrlID((HWND)lParam) == IDD_FILE_MIGR_SCRL) {

            /* ----- control vartival scroll-bar of listbox ----- */

            FileMigrCtrlScrl(hDlg, wParam, lParam, &Check, &Index);

        }
        return FALSE;


	case WM_SETFONT:

		if (hResourceFont) {
			int j;
			for(j=IDD_FILE_MIGR_SELECT; j<=IDD_FILE_CAPTION10; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDYES, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDNO, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDD_FILE_MIGR_SELECT:

            /* ----- extend dialogbox for select parameter list ----- */

            SetWindowPos(hDlg,
                         (HWND)NULL,
                         0,
                         0,
                         (short)(rRect.right  - rRect.left),
                         (short)(rRect.bottom - rRect.top),
                         SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

            /* ----- set index for selct parameter list ----- */

            Index.wTop = FILE_MIGR_SCRL_MIN;
            Index.wBtm = FILE_MIGR_ONE_PAGE;

            /* ----- Set description for select parameter list ----- */

            FileMigrSetListDscrb(hDlg, &Index);

            /* ----- Set data for select parameter list ----- */

            FileMigrSetListData(hDlg, &Check, &Index);

            /* ----- set min. & max. position of vertical scroll ----- */

            SetScrollRange(GetDlgItem(hDlg, IDD_FILE_MIGR_SCRL),
                           SB_CTL,
                           FILE_MIGR_SCRL_MIN,
                           FILE_MIGR_SCRL_MAX,
                           TRUE);

            /* ----- set thumb position of vertical scroll ----- */

            SetScrollPos(GetDlgItem(hDlg, IDD_FILE_MIGR_SCRL),
                         SB_CTL,
                         FILE_MIGR_SCRL_MIN,
                         FALSE);

            return FALSE;

        case IDD_FILE_MIGR_CTRL1:
        case IDD_FILE_MIGR_CTRL2:
        case IDD_FILE_MIGR_CTRL3:
        case IDD_FILE_MIGR_CTRL4:
        case IDD_FILE_MIGR_CTRL5:
        case IDD_FILE_MIGR_CTRL6:
        case IDD_FILE_MIGR_CTRL7:

            /* ----- control checkbox of listbox ----- */

            FileMigrCtrlChkbox(hDlg, LOWORD(wParam), &Check, &Index);

            return TRUE;

        case IDYES:

            /* delete old file  */
            FileTempDelete(hPepInst);

            /* move seek pointer    */
            SetFilePointer(iFileHandle, 0L, NULL, FILE_BEGIN);
			fileError = GetLastError();
			if ( fileError != 0) {
                EndDialog(hDlg, FALSE);
                return TRUE;
            }

            /* read header  */
            ReadFile(iFileHandle, &Fheaderold, sizeof(FHEADEROLD), &bytesRead, NULL);
			if ( bytesRead != sizeof(FHEADEROLD)) {
                EndDialog(hDlg, FALSE);
                return TRUE;
            }
            LoadString(hResourceDll,
                       IDS_FILE_ID_NAME,
                       szWork,
                       PEP_STRING_LEN_MAC(szWork));
            memcpy(Fheaderold.Fid.szName,
                     (LPCSTR)szWork,
                     FILE_LEN_ID);

            /* divide each file */
            LoadString(hResourceDll, IDS_FILE_NAME_DEPT, szFname, PEP_STRING_LEN_MAC(szFname));
            if (FileToDeptFile(iFileHandle,
                               Fheaderold.File.aRoom[IDS_FILE_NAME_DEPT - IDS_FILE_NAME_DEPT].lOffset,
                               Fheaderold.File.aRoom[IDS_FILE_NAME_DEPT - IDS_FILE_NAME_DEPT].lLen,
                               szFname) == FALSE) {
                EndDialog(hDlg, FALSE);
                return TRUE;
            }
            LoadString(hResourceDll, IDS_FILE_NAME_PLUTMP, szFname, PEP_STRING_LEN_MAC(szFname));
            if (FileToEachFile(iFileHandle,
                               Fheaderold.File.aRoom[IDS_FILE_NAME_PLU - IDS_FILE_NAME_DEPT].lOffset,
                               Fheaderold.File.aRoom[IDS_FILE_NAME_PLU - IDS_FILE_NAME_DEPT].lLen,
                               szFname) == FALSE) {
                EndDialog(hDlg, FALSE);
                return TRUE;
            }
            OpPluMigrateFile();     /* migrate PLU file */
            LoadString(hResourceDll, IDS_FILE_NAME_CAS, szFname, PEP_STRING_LEN_MAC(szFname));
            if (FileToEachFile(iFileHandle,
                               Fheaderold.File.aRoom[IDS_FILE_NAME_SER - IDS_FILE_NAME_DEPT].lOffset,
                               Fheaderold.File.aRoom[IDS_FILE_NAME_SER - IDS_FILE_NAME_DEPT].lLen,
                               szFname) == FALSE) {
                EndDialog(hDlg, FALSE);
                return TRUE;
            }
            LoadString(hResourceDll, IDS_FILE_NAME_SER, szFname, PEP_STRING_LEN_MAC(szFname));
            if (FileToEachFile(iFileHandle,
                               Fheaderold.File.aRoom[IDS_FILE_NAME_CAS - IDS_FILE_NAME_DEPT].lOffset,
                               Fheaderold.File.aRoom[IDS_FILE_NAME_CAS - IDS_FILE_NAME_DEPT].lLen,
                               szFname) == FALSE) {
                EndDialog(hDlg, FALSE);
                return TRUE;
            }
            LoadString(hResourceDll, IDS_FILE_NAME_DIRECT, szFname, PEP_STRING_LEN_MAC(szFname));
            if (FileToEachFile(iFileHandle,
                               Fheaderold.File.aRoom[IDS_FILE_NAME_ETK - IDS_FILE_NAME_DEPT].lOffset,
                               Fheaderold.File.aRoom[IDS_FILE_NAME_ETK - IDS_FILE_NAME_DEPT].lLen,
                               szFname) == FALSE) {
                EndDialog(hDlg, FALSE);
                return TRUE;
            }

            /* transfer             */
            SetFilePointer(iFileHandle,
                        Fheaderold.Mem.Trans.lOffset,
                        NULL, FILE_BEGIN);
			fileError = GetLastError();
			if ( fileError != NO_ERROR/*Fheaderold.Mem.Trans.lOffset*/) {
                EndDialog(hDlg, FALSE);
                return TRUE;
            }
            if (Check.bTrans == TRUE) {
                ReadFile(iFileHandle,
                           TransGetDataAddr(),
                           (WORD)Fheaderold.Mem.Trans.lLen, &bytesRead, NULL);
				if ( bytesRead != (UINT)Fheaderold.Mem.Trans.lLen) {
                    EndDialog(hDlg, FALSE);
                    return TRUE;
                }
            }else{
                TransInitialize();
            }

            lpWork = TransGetDataAddr();
            ((TRANSFER FAR *)lpWork)->fwTransTerm   = 0;
            ((TRANSFER FAR *)lpWork)->fwTransLoader = TRANS_ACCESS_PARA;
            ((TRANSFER FAR *)lpWork)->fwUpdate      = 0;
            ((TRANSFER FAR *)lpWork)->fwFlexMem     = 0;

            /* ----- allocate global memory for memory copy ----- */

            if ((hMem = GlobalAlloc(GHND, PEP_PARAM_LEN)) == 0) {
                EndDialog(hDlg, FALSE);
                return TRUE;
            }
            if ((lpParam = GlobalLock(hMem)) == NULL) {
                GlobalFree(hMem);
                EndDialog(hDlg, FALSE);
                return TRUE;
            }

            /* devide parameter */
            SetFilePointer(iFileHandle,
                        Fheaderold.Mem.Para.lOffset,
                        NULL, FILE_BEGIN);
			fileError = GetLastError();
			if ( fileError != NO_ERROR/*Fheaderold.Mem.Para.lOffset*/) {
                GlobalUnlock(hMem);
                GlobalFree(hMem);
                EndDialog(hDlg, FALSE);
                return TRUE;
            }
            ReadFile(iFileHandle,
                       lpParam,
                       (WORD)Fheaderold.Mem.Para.lLen, &bytesRead, NULL);
			if ( bytesRead != (UINT)Fheaderold.Mem.Para.lLen) {
                GlobalUnlock(hMem);
                GlobalFree(hMem);
                EndDialog(hDlg, FALSE);
                return TRUE;
            }
            /*** NCR2172
            lpBufStart = ParaStart;
            lpBufEnd   = ParaEnd;
            ***/
            lpBufStart = &Para;

            /* copy default parameter */

            /*** NCR2172
            memcpy(lpBufStart, lpPepParam, FP_OFF(lpBufEnd) - FP_OFF(lpBufStart));
            ***/
            memcpy(lpBufStart, lpPepParam, sizeof(Para));

            /* copy old parameter to new */
            for(wI = 0; wI < FILE_MIGR_NUM_PARA; wI++) {
                if (Check.abPara[wI] == TRUE) {
                    switch (wI) {
                    case FILE_MIGR_PARA_FLEXMEM:
                        memcpy(lpBufStart,
                                 lpParam,
                                 sizeof(FLEXMEM) * FLEX_CAS_ADR);

                        /*NCR2172*/
                        /*((FLEXMEM FAR *)lpBufStart + (FLEX_GC_ADR  - 1))->usRecordNumber = */
                        /*       ((FLEXMEM FAR *)lpParam + FLEX_CAS_ADR)->usRecordNumber;    */

                        ((FLEXMEM FAR *)lpBufStart + (FLEX_GC_ADR  - 1))->ulRecordNumber =
                               ((FLEXMEM FAR *)lpParam + FLEX_CAS_ADR)->ulRecordNumber;

                        ((FLEXMEM FAR *)lpBufStart + (FLEX_GC_ADR  - 1))->uchPTDFlag = 
                               ((FLEXMEM FAR *)lpParam + FLEX_CAS_ADR)->uchPTDFlag;

                        /*** NCR2172
                        if (((FLEXMEM FAR *)lpBufStart + (FLEX_PLU_ADR  - 1))->usRecordNumber != 0) {
                            OpPluCreatFile(((FLEXMEM FAR *)lpBufStart + (FLEX_PLU_ADR  - 1))->usRecordNumber, 0);
                        } ***/

                        if (((FLEXMEM FAR *)lpBufStart + (FLEX_PLU_ADR  - 1))->ulRecordNumber != 0L) {
/*                            FileMigrPLUFile();  * Saratoga *
                            OpPluCreatFile(
                                (USHORT)(((FLEXMEM FAR *)lpBufStart + (FLEX_PLU_ADR  - 1))->ulRecordNumber),
                                  0);*/
                        }

                        /* ===== NCR2172
                        if (((FLEXMEM FAR *)lpBufStart + (FLEX_WT_ADR  - 1))->usRecordNumber != 0) {
                            WaiCreatFile((UCHAR)(((FLEXMEM FAR *)lpBufStart + (FLEX_WT_ADR  - 1))->usRecordNumber));
                        }
                        */
#if defined(POSSIBLE_DEAD_CODE)
                        if (((FLEXMEM FAR *)lpBufStart + (FLEX_WT_ADR  - 1))->ulRecordNumber != 0L) {
                            WaiCreatFile((UCHAR)(((FLEXMEM FAR *)lpBufStart + (FLEX_WT_ADR  - 1))->ulRecordNumber));
                        }
#endif

                        /* NCR2172 */
                        /*if (((FLEXMEM FAR *)lpBufStart + (FLEX_CAS_ADR  - 1))->usRecordNumber != 0) {*/
                        /*    CasCreatFile((UCHAR)(((FLEXMEM FAR *)lpBufStart + (FLEX_CAS_ADR  - 1))->usRecordNumber));*/
                        /*}                        */

                        if (((FLEXMEM FAR *)lpBufStart + (FLEX_CAS_ADR  - 1))->ulRecordNumber != 0L) {
                            CasCreatFile((UCHAR)(((FLEXMEM FAR *)lpBufStart + (FLEX_CAS_ADR  - 1))->ulRecordNumber));
                        }
                        lpWork  = lpBufStart;
                        for (wLoop = 0; wLoop < FLEX_CAS_ADR; wLoop++) {
                        /* NCR2172 */
                        /*    if (((FLEXMEM FAR *)lpWork)->usRecordNumber != 0) {*/
                            if (((FLEXMEM FAR *)lpWork)->ulRecordNumber != 0L) {
                                PepSetModFlag(hwndPepMain, uiPep_Mf[wLoop], 0);
                                TransAccessFlexMem(wPep_Acc[wLoop]);
                            }
                            lpWork = (char FAR *)lpWork + sizeof(FLEXMEM);
                        }
                        /* NCR2172 */
                        /*((FLEXMEM FAR *)lpBufStart + (FLEX_ITEMSTORAGE_ADR - 1))->usRecordNumber = 30;*/
                        /*((FLEXMEM FAR *)lpBufStart + (FLEX_CONSSTORAGE_ADR - 1))->usRecordNumber = 30;*/
                        
                        ((FLEXMEM FAR *)lpBufStart + (FLEX_ITEMSTORAGE_ADR - 1))->ulRecordNumber = 30L;
                        ((FLEXMEM FAR *)lpBufStart + (FLEX_CONSSTORAGE_ADR - 1))->ulRecordNumber = 30L;
                        break;

                    case FILE_MIGR_PARA_FSC:
                        lpFsc = (FSCDATA FAR *)lpBufStart;
                        lpWork  = lpBufStart;
                        for (wLoop = 0; wLoop < 5; wLoop++) {
                            memcpy(lpWork,
                                     lpParam,
                                     nFileMigrOldLen[wI]);
                            lpWork = (char FAR *)lpWork + MAX_FSC_NO * MAX_FSC_DATA;
                        }
                        break;

                    case FILE_MIGR_PARA_TAXTBL:
                        memcpy(lpBufStart, lpParam, FILE_MIGR_TAXHED_OLD);
                        memcpy((char FAR *)lpBufStart + FILE_MIGR_TAXHED_NEW,
                                 (char FAR *)lpParam + FILE_MIGR_TAXHED_OLD,
                                 (WORD)(nFileMigrOldLen[wI] - FILE_MIGR_TAXHED_OLD));
                        break;

                    case FILE_MIGR_PARA_TRANSMNE:
                        memcpy(lpBufStart, lpParam, FILE_MIGR_COPY_TRANSMNE);
                        break;

                    case FILE_MIGR_PARA_LEADTHRU:
                        memcpy(lpBufStart, lpParam, FILE_MIGR_COPY_LEADTHRU);
                        break;

                    case FILE_MIGR_PARA_TKCTL:
                        lpWork  = lpBufStart;
                        for (wLoop = 0; wLoop < MAX_TTLKEY_NO; wLoop++) {
                            memcpy(lpWork,
                                     (char FAR *)lpParam + wLoop * 5,
                                     4);
                            lpWork = (char FAR *)lpWork + MAX_TTLKEY_DATA;
                        }
                        break;

                    case FILE_MIGR_PARA_PLUKEY:
                        lpPluKeySouc = (USHORT FAR *)lpParam;
                        lpPluKeyDes = (USHORT FAR *)lpBufStart;
                        /* ----- clear menu plu parameter ----- */
                        memset(lpBufStart, 0, sizeof(USHORT) * FSC_PLU_MAX * 5);
                        for (wLoop = 0; wLoop < 5; wLoop++) {
                            for (wJ = 0; wJ < MAX_FSC_NO;
                              wJ++, lpFsc++, lpPluKeyDes++) {
                                if (lpFsc->uchFsc == FILE_MIGR_PLUKEY) {
                                    *lpPluKeyDes = *(lpPluKeySouc
                                                     + (lpFsc->uchExt - 1));
                                    lpFsc->uchExt = (UCHAR)(wJ + 1);
                                }
                            }
                            lpPluKeySouc += 150;
                        }
                        break;

                    case FILE_MIGR_PARA_SLIPFEED:
                    case FILE_MIGR_PARA_RPTNAME:
                    case FILE_MIGR_PARA_CHAR24:
                    default:
                        memcpy(lpBufStart, lpParam, nFileMigrOldLen[wI]);
                    }
                }
                lpBufStart = (char FAR *)lpBufStart + nFileMigrNewLen[wI];
                lpParam    = (char FAR *)lpParam + nFileMigrOldLen[wI];

            }

            /* ----- free global memory for memory copy ----- */

            GlobalUnlock(hMem);
            GlobalFree(hMem);

            /* configuration        */
            SetFilePointer(iFileHandle,
                        Fheaderold.Mem.Config.lOffset,
                        NULL, FILE_BEGIN);
			fileError = GetLastError();
			if ( fileError != NO_ERROR/*Fheaderold.Mem.Config.lOffset*/) {
                EndDialog(hDlg, FALSE);
                return TRUE;
            }
            if (Check.bFile == TRUE) {
                ReadFile(iFileHandle,
                           &Fconfig,
                           (WORD)Fheaderold.Mem.Config.lLen, &bytesRead, NULL);
				if ( bytesRead != (UINT)Fheaderold.Mem.Config.lLen) {
                    EndDialog(hDlg, FALSE);
                    return TRUE;
                }
                /* --- Correct FCONFIG Migration, 9/12/95 --- */
//                Fconfig.usMemory = (FILE_SIZE_64KB * FILE_SIZE_8MBTOKB);  /* Flash memory 8MB */
				Fconfig.chKBType = FILE_KB_TOUCH;  // set default keyboard to be touch keyboard

                Fconfig.chNoOfSatellite = 0;                /* None */

                if (Fconfig.chTerminal) {   /* satellite terminal   */
                    Fconfig.ulStartAddr = (unsigned long)FILE_SADDR;
                } else {                    /* master terminal      */
                    Fconfig.ulStartAddr = (unsigned long)FILE_MADDR;
                }
                Fconfig.ulStartAddr += (unsigned long)FILE_FAT_ADDR;
                /* --- Correct FCONFIG Migration, 9/12/95 --- */
            }

			Fconfig.usMemory = (FILE_SIZE_64KB * FILE_SIZE_48MBTOKB);  // set default Flash memory 48MB

            PepSetModFlag(hwndPepMain, PEP_MF_PROG | PEP_MF_ACT, 0);

            EndDialog(hDlg, TRUE);
            return TRUE;

        case IDNO:
            EndDialog(hDlg, FALSE);
            return TRUE;
        }
        return FALSE;

    default:
        return FALSE;
    }
}

/*
* ===========================================================================
**  Synopsis:   BOOL    FileMigrCtrlScrl();
*
**  Input   :   HWND            hDlg   - handle of a dialogbox procedure
*               WORD            wParam - 16 bits message parameter
*               LONG            lParam - 32 bits message parameter
*               LPFILEMIGRCHK   lpChk  - address of checkbox status
*               LPFILEMIGRIDX   lpIdx  - address of list index
*
**  Return  :   TRUE               - user process is executed
*               FALSE              - default process is expected
*
**  Description:
*       This function is vertical scroll procedure.
* ===========================================================================
*/
BOOL    FileMigrCtrlScrl(HWND hDlg,
                         WPARAM wParam,
                         LPARAM lParam,
                         LPFILEMIGRCHK lpChk,
                         LPFILEMIGRIDX lpIdx)
{
    BOOL    fRet;

    switch (LOWORD(wParam)) {
    case SB_LINEUP:
    case SB_PAGEUP:
    case SB_LINEDOWN:
    case SB_PAGEDOWN:
        if (FileMigrCalcListIndex(wParam, lpIdx) == FILE_MIGR_NOT_SCROLL) {
                            /* calculate thumb position & top/bottom index */

            /* ----- set description by new index ----- */

            FileMigrSetListDscrb(hDlg, lpIdx);

            /* ----- set data by new index ----- */

            FileMigrSetListData(hDlg, lpChk, lpIdx);

            /* ----- set scroll box (Thumb) ----- */

            SetScrollPos((HWND)lParam,
                         SB_CTL,
                         (int)lpIdx->wTop,
                         TRUE);

            fRet = TRUE;

        } else {
            fRet = FALSE;
        }
        break;

    case SB_THUMBTRACK:

        /* ----- calculate thumb position & top/bottom index ----- */

        lpIdx->wTop = HIWORD(wParam);
        lpIdx->wBtm = lpIdx->wTop + FILE_MIGR_ONE_PAGE;

        /* ----- set description by new index ----- */

        FileMigrSetListDscrb(hDlg, lpIdx);

        /* ----- set data by new index ----- */

        FileMigrSetListData(hDlg, lpChk, lpIdx);

        /* ----- set scroll box (thumb) ----- */

        SetScrollPos((HWND)lParam,
                     SB_CTL,
                     (int)lpIdx->wTop,
                     TRUE);

        fRet = TRUE;
        break;

    default:
        fRet = FALSE;
    }

    return (fRet);
}

/*
* ===========================================================================
**  Synopsis:   VOID    FileMigrSetListDscrb();
*
**  Input   :   HWND            hDlg  - handle of a dialogbox procedure
*               LPFILEMIGRIDX   lpIdx - address of list index
*
**  Return  :   None
*
**  Description:
*       This function redraws description & price to each control.
* ===========================================================================
*/
VOID    FileMigrSetListDscrb(HWND hDlg, LPFILEMIGRIDX lpIdx)
{
    WORD    wI;
    WCHAR    szBuff[FILE_MIGR_BUFF_LEN];

    for (wI = 0; wI < FILE_MIGR_MAX_PAGE; wI++) {

        /* ----- Set description to checkbox ----- */

        LoadString(hResourceDll,
                   IDS_FILE_MIGR_DSC01 + wI + lpIdx->wTop,
                   szBuff,
                   PEP_STRING_LEN_MAC(szBuff));
        DlgItemRedrawText(hDlg, IDD_FILE_MIGR_CTRL1 + wI, szBuff);

    }
}

/*
* ===========================================================================
**  Synopsis:   VOID    FileMigrSetListData();
*
**  Input   :   HWND            hDlg  - handle of a dialogbox procedure
*               LPFILEMIGRCHK   lpChk - address of checkbox status
*               LPFILEMIGRIDX   lpIdx - address of list index
*
**  Return  :   None
*
**  Description:
*       This function redraws description & price to each control.
* ===========================================================================
*/
VOID    FileMigrSetListData(HWND hDlg,
                            LPFILEMIGRCHK lpChk,
                            LPFILEMIGRIDX lpIdx)
{
    WORD    wI;

    for (wI = 0; wI < FILE_MIGR_MAX_PAGE; wI++) {

        /* ----- Set data to checkbox ----- */

        SendDlgItemMessage(hDlg,
                           IDD_FILE_MIGR_CTRL1 + wI,
                           BM_SETCHECK,
                           FileMigrMakeListData((WORD)(wI + lpIdx->wTop), lpChk),
                           0L);

    }
}

/*
* ===========================================================================
**  Synopsis:   BOOL    FileMigrMakeListData();
*
**  Input   :   WORD            wOfs    - offset in scroll list
*               LPFILEMIGRCHK   lpChk   - address of checkbox status
*
**  Return  :   TRUE            - control code is on
*               FALSE           - control code is off
*
**  Description:
*       This function make checkbox on/off data.
* ===========================================================================
*/
BOOL    FileMigrMakeListData(WPARAM wOfs, LPFILEMIGRCHK lpChk)
{
    BOOL    fRet = FALSE;

    /* ----- Make checkbox on/off data ----- */

    switch (wOfs) {
    case FILE_MIGR_LIST_CONFIG:
        fRet = (BOOL)lpChk->bFile;
        break;

    case FILE_MIGR_LIST_MDC:
        fRet = (BOOL)lpChk->abPara[FILE_MIGR_PARA_MDC];
        break;
        
    case FILE_MIGR_LIST_FLEXMEM:
        fRet = (BOOL)lpChk->abPara[FILE_MIGR_PARA_FLEXMEM];
        break;

    case FILE_MIGR_LIST_FSC:
        fRet = (BOOL)lpChk->abPara[FILE_MIGR_PARA_FSC];
        break;
        
    case FILE_MIGR_LIST_SECNO:
        fRet = (BOOL)lpChk->abPara[FILE_MIGR_PARA_SECNO];
        break;
        
    case FILE_MIGR_LIST_SUPLV:
        fRet = (BOOL)lpChk->abPara[FILE_MIGR_PARA_SUPLV];
        break;
        
    case FILE_MIGR_LIST_ACSEC:
        fRet = (BOOL)lpChk->abPara[FILE_MIGR_PARA_ACSEC];
        break;
        
    case FILE_MIGR_LIST_HALO:
        fRet = (BOOL)lpChk->abPara[FILE_MIGR_PARA_HALO];
        break;
        
    case FILE_MIGR_LIST_HOURLY:
        fRet = (BOOL)lpChk->abPara[FILE_MIGR_PARA_HOURLY];
        break;
        
    case FILE_MIGR_LIST_SLIPFEED:
        fRet = (BOOL)lpChk->abPara[FILE_MIGR_PARA_SLIPFEED];
        break;
        
    case FILE_MIGR_LIST_TRANSMNE:
        fRet = (BOOL)lpChk->abPara[FILE_MIGR_PARA_TRANSMNE];
        break;
        
    case FILE_MIGR_LIST_LEADTHRU:
        fRet = (BOOL)lpChk->abPara[FILE_MIGR_PARA_LEADTHRU];
        break;
        
    case FILE_MIGR_LIST_RPTNAME:
        fRet = (BOOL)lpChk->abPara[FILE_MIGR_PARA_RPTNAME];
        break;
        
    case FILE_MIGR_LIST_SPEMNE:
        fRet = (BOOL)lpChk->abPara[FILE_MIGR_PARA_SPEMNE];
        break;
        
    case FILE_MIGR_LIST_ADJMNE:
        fRet = (BOOL)lpChk->abPara[FILE_MIGR_PARA_ADJMNE];
        break;
        
    case FILE_MIGR_LIST_PMOD:
        fRet = (BOOL)lpChk->abPara[FILE_MIGR_PARA_PMOD];
        break;
        
    case FILE_MIGR_LIST_MDEPT:
        fRet = (BOOL)lpChk->abPara[FILE_MIGR_PARA_MDEPT];
        break;
        
    case FILE_MIGR_LIST_AUTOKTCH:
        fRet = (BOOL)lpChk->abPara[FILE_MIGR_PARA_AUTOKTCH];
        break;
        
    case FILE_MIGR_LIST_CHAR24:
        fRet = (BOOL)lpChk->abPara[FILE_MIGR_PARA_CHAR24];
        break;
        
    case FILE_MIGR_LIST_TKTYP:
        fRet = (BOOL)lpChk->abPara[FILE_MIGR_PARA_TKTYP];
        break;
        
    case FILE_MIGR_LIST_TKCTL:
        fRet = (BOOL)lpChk->abPara[FILE_MIGR_PARA_TKCTL];
        break;
        
    case FILE_MIGR_LIST_PLUKEY:
        fRet = (BOOL)lpChk->abPara[FILE_MIGR_PARA_PLUKEY];
        break;
        
    case FILE_MIGR_LIST_CTLMENU:
        fRet = (BOOL)lpChk->abPara[FILE_MIGR_PARA_CTLMENU];
        break;
        
    case FILE_MIGR_LIST_CASHAB:
        fRet = (BOOL)lpChk->abPara[FILE_MIGR_PARA_CASHAB];
        break;
        
    case FILE_MIGR_LIST_ROUND:
        fRet = (BOOL)lpChk->abPara[FILE_MIGR_PARA_ROUND];
        break;
        
    case FILE_MIGR_LIST_DISC:
        fRet = (BOOL)lpChk->abPara[FILE_MIGR_PARA_DISC];
        break;
        
    case FILE_MIGR_LIST_PROMOT:
        fRet = (BOOL)lpChk->abPara[FILE_MIGR_PARA_PROMOT];
        break;
        
    case FILE_MIGR_LIST_FC:
        fRet = (BOOL)lpChk->abPara[FILE_MIGR_PARA_FC];
        break;
        
    case FILE_MIGR_LIST_TARE:
        fRet = (BOOL)lpChk->abPara[FILE_MIGR_PARA_TARE];
        break;

    case FILE_MIGR_LIST_MENUPLU:
        fRet = (BOOL)lpChk->abPara[FILE_MIGR_PARA_MENUPLU];
        break;
        
    case FILE_MIGR_LIST_TAXTBL:
        fRet = (BOOL)lpChk->abPara[FILE_MIGR_PARA_TAXTBL];
        break;
        
    case FILE_MIGR_LIST_TAXRATE:
        fRet = (BOOL)lpChk->abPara[FILE_MIGR_PARA_TAXRATE];
        break;
        
    case FILE_MIGR_LIST_DIRECT:
        fRet = (BOOL)lpChk->bTrans;
        break;
    }

    return (fRet);
}

/*
* ===========================================================================
**  Synopsis:   BOOL    FileMigrCalcListIndex();
*
**  Input   :   WORD            wParam - 16 bits message parameter
*               LPFILEMIGRIDX   lpIdx  - address of list index
*
**  Return  :   FILE_MIGR_NOT_SCROLL   - not scroll up to top/bottom of listbox
*               FILE_MIGR_SCROLL       - scroll up to top/bottom of listbox
*
**  Description:
*       This function compute top/bottom index.
* ===========================================================================
*/
BOOL    FileMigrCalcListIndex(WPARAM wParam, LPFILEMIGRIDX lpIdx)
{
    BOOL    fRet = 0;

    switch (wParam) {
    case SB_LINEUP:
        lpIdx->wBtm -= FILE_MIGR_ONE_LINE;

        if (lpIdx->wBtm < FILE_MIGR_ONE_PAGE) {       /* scroll to top */
            lpIdx->wBtm = FILE_MIGR_ONE_PAGE;
            lpIdx->wTop = FILE_MIGR_SCRL_MIN;
            fRet = FILE_MIGR_SCROLL;
        } else {                            /* scroll to one line up */
            lpIdx->wTop -= FILE_MIGR_ONE_LINE;
            fRet = FILE_MIGR_NOT_SCROLL;
        }
        break;

    case SB_LINEDOWN:
        lpIdx->wBtm += FILE_MIGR_ONE_LINE;

        if (lpIdx->wBtm >= FILE_MIGR_NUM_LIST) {      /* scroll to bottom */
            lpIdx->wBtm = FILE_MIGR_NUM_LIST - 1;
            lpIdx->wTop = FILE_MIGR_SCRL_MAX;
            fRet = FILE_MIGR_SCROLL;
        } else {                            /* scroll to one line down */
            lpIdx->wTop += FILE_MIGR_ONE_LINE;
            fRet = FILE_MIGR_NOT_SCROLL;
        }
        break;

    case SB_PAGEUP:
        if (lpIdx->wTop == FILE_MIGR_SCRL_MIN) {     /* current position is top */

            fRet = FILE_MIGR_SCROLL;

        } else {                            /* current position isn't top */

            lpIdx->wBtm -= FILE_MIGR_ONE_PAGE;
            fRet = FILE_MIGR_NOT_SCROLL;

            if (lpIdx->wBtm <= FILE_MIGR_ONE_PAGE) {  /* scroll to top */
                lpIdx->wBtm = FILE_MIGR_ONE_PAGE;
                lpIdx->wTop = FILE_MIGR_SCRL_MIN;
            } else {                        /* scroll to one page */
                lpIdx->wTop -= FILE_MIGR_ONE_PAGE;
            }

        }
        break;

    case SB_PAGEDOWN:
        if (lpIdx->wTop == FILE_MIGR_SCRL_MAX) {     /* current position is bottom */

            fRet = FILE_MIGR_SCROLL;

        } else {                            /* current position isn't bottom */

            lpIdx->wBtm += FILE_MIGR_ONE_PAGE;
            fRet = FILE_MIGR_NOT_SCROLL;

            if (lpIdx->wBtm >= FILE_MIGR_NUM_LIST) {  /* scroll to bottom */
                lpIdx->wBtm = FILE_MIGR_NUM_LIST - 1;
                lpIdx->wTop = FILE_MIGR_SCRL_MAX;
            } else {                        /* scroll to one line down */
                lpIdx->wTop += FILE_MIGR_ONE_PAGE;
            }

        }
    }

    return (fRet);
}

/*
* ===========================================================================
**  Synopsis:   VOID    FileMigrCtrlChkbox();
*
**  Input   :   HWND            hDlg   - handle of a dialogbox procedure
*               WORD            wEditId - control ID
*               LPFILEMIGRCHK   lpChk  - address of checkbox status
*               LPFILEMIGRIDX   lpIdx  - address of list index
*
**  Return  :   None
*
**  Description:
*       This function control checkbox.
* ===========================================================================
*/
VOID    FileMigrCtrlChkbox(HWND hDlg,
                           WORD wEditId,
                           LPFILEMIGRCHK lpChk,
                           LPFILEMIGRIDX lpIdx)
{   
    BOOL    fChk;

    /* ----- get previous status ----- */

    fChk = (BOOL)SendDlgItemMessage(hDlg, wEditId, BM_GETCHECK, 0, 0L);

    /* ----- set new status ----- */

    if (fChk == TRUE) {
        fChk = FALSE;
    } else {            /* fChk == FALSE */
        fChk = TRUE;
    }

    /* ----- Set checkbox by new status ----- */

    SendDlgItemMessage(hDlg, wEditId, BM_SETCHECK, fChk, 0L);

    /* ----- set checkbox status ----- */

    switch ((WORD)(wEditId - IDD_FILE_MIGR_CTRL1 + lpIdx->wTop)) {
    case FILE_MIGR_LIST_CONFIG:
        lpChk->bFile = (BYTE)fChk;
        break;

    case FILE_MIGR_LIST_MDC:
        lpChk->abPara[FILE_MIGR_PARA_MDC] = (BYTE)fChk;
        break;
        
    case FILE_MIGR_LIST_FLEXMEM:
        lpChk->abPara[FILE_MIGR_PARA_FLEXMEM] = (BYTE)fChk;
        break;

    case FILE_MIGR_LIST_FSC:
        lpChk->abPara[FILE_MIGR_PARA_FSC] = (BYTE)fChk;
        break;
        
    case FILE_MIGR_LIST_SECNO:
        lpChk->abPara[FILE_MIGR_PARA_SECNO] = (BYTE)fChk;
        break;
        
    case FILE_MIGR_LIST_SUPLV:
        lpChk->abPara[FILE_MIGR_PARA_SUPLV] = (BYTE)fChk;
        break;
        
    case FILE_MIGR_LIST_ACSEC:
        lpChk->abPara[FILE_MIGR_PARA_ACSEC] = (BYTE)fChk;
        break;
        
    case FILE_MIGR_LIST_HALO:
        lpChk->abPara[FILE_MIGR_PARA_HALO] = (BYTE)fChk;
        break;
        
    case FILE_MIGR_LIST_HOURLY:
        lpChk->abPara[FILE_MIGR_PARA_HOURLY] = (BYTE)fChk;
        break;
        
    case FILE_MIGR_LIST_SLIPFEED:
        lpChk->abPara[FILE_MIGR_PARA_SLIPFEED] = (BYTE)fChk;
        break;
        
    case FILE_MIGR_LIST_TRANSMNE:
        lpChk->abPara[FILE_MIGR_PARA_TRANSMNE] = (BYTE)fChk;
        break;
        
    case FILE_MIGR_LIST_LEADTHRU:
        lpChk->abPara[FILE_MIGR_PARA_LEADTHRU] = (BYTE)fChk;
        break;
        
    case FILE_MIGR_LIST_RPTNAME:
        lpChk->abPara[FILE_MIGR_PARA_RPTNAME] = (BYTE)fChk;
        break;
        
    case FILE_MIGR_LIST_SPEMNE:
        lpChk->abPara[FILE_MIGR_PARA_SPEMNE] = (BYTE)fChk;
        break;
        
    case FILE_MIGR_LIST_ADJMNE:
        lpChk->abPara[FILE_MIGR_PARA_ADJMNE] = (BYTE)fChk;
        break;
        
    case FILE_MIGR_LIST_PMOD:
        lpChk->abPara[FILE_MIGR_PARA_PMOD] = (BYTE)fChk;
        break;
        
    case FILE_MIGR_LIST_MDEPT:
        lpChk->abPara[FILE_MIGR_PARA_MDEPT] = (BYTE)fChk;
        break;
        
    case FILE_MIGR_LIST_AUTOKTCH:
        lpChk->abPara[FILE_MIGR_PARA_AUTOKTCH] = (BYTE)fChk;
        break;
        
    case FILE_MIGR_LIST_CHAR24:
        lpChk->abPara[FILE_MIGR_PARA_CHAR24] = (BYTE)fChk;
        break;
        
    case FILE_MIGR_LIST_TKTYP:
        lpChk->abPara[FILE_MIGR_PARA_TKTYP] = (BYTE)fChk;
        break;
        
    case FILE_MIGR_LIST_TKCTL:
        lpChk->abPara[FILE_MIGR_PARA_TKCTL] = (BYTE)fChk;
        break;
        
    case FILE_MIGR_LIST_PLUKEY:
        lpChk->abPara[FILE_MIGR_PARA_PLUKEY] = (BYTE)fChk;
        break;
        
    case FILE_MIGR_LIST_CTLMENU:
        lpChk->abPara[FILE_MIGR_PARA_CTLMENU] = (BYTE)fChk;
        break;
        
    case FILE_MIGR_LIST_CASHAB:
        lpChk->abPara[FILE_MIGR_PARA_CASHAB] = (BYTE)fChk;
        break;
        
    case FILE_MIGR_LIST_ROUND:
        lpChk->abPara[FILE_MIGR_PARA_ROUND] = (BYTE)fChk;
        break;
        
    case FILE_MIGR_LIST_DISC:
        lpChk->abPara[FILE_MIGR_PARA_DISC] = (BYTE)fChk;
        break;
        
    case FILE_MIGR_LIST_PROMOT:
        lpChk->abPara[FILE_MIGR_PARA_PROMOT] = (BYTE)fChk;
        break;
        
    case FILE_MIGR_LIST_FC:
        lpChk->abPara[FILE_MIGR_PARA_FC] = (BYTE)fChk;
        break;
        
    case FILE_MIGR_LIST_TARE:
        lpChk->abPara[FILE_MIGR_PARA_TARE] = (BYTE)fChk;
        break;

    case FILE_MIGR_LIST_MENUPLU:
        lpChk->abPara[FILE_MIGR_PARA_MENUPLU] = (BYTE)fChk;
        break;
        
    case FILE_MIGR_LIST_TAXTBL:
        lpChk->abPara[FILE_MIGR_PARA_TAXTBL] = (BYTE)fChk;
        break;
        
    case FILE_MIGR_LIST_TAXRATE:
        lpChk->abPara[FILE_MIGR_PARA_TAXRATE] = (BYTE)fChk;
        break;
        
    case FILE_MIGR_LIST_DIRECT:
        lpChk->bTrans = (BYTE)fChk;
        break;
    }
}

#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
/*
* ===========================================================================
**  Synopsis:   static  USHORT  FileMigrPluFile()
*
*   Input   :   HWND    hDlg   - handle of a dialog procedure
**
*   Return  :   USHORT  TRUE   - successed to migrate
*                       FALSE  - failed to migrate
*
**  Description:
*       This procedure migrate PLU file from R1.0/1.1 version
* ===========================================================================
*/
BOOL  FileMigrPLUFile(VOID)
{
    HGLOBAL     hMem;
    LPSTR       lpszBuff;
    WCHAR        szTempFile[FILE_MIGR_PATH_LEN],
                szPath[FILE_MIGR_PATH_LEN],
                szFile[FILE_MIGR_TEMP_LEN];
    ECMBLOCK    EcmBlock;
    ECMCOND     EcmCond;
    USHORT      usHandle, usMirror, usRead, usStat;
    OFSTRUCT    TempBuff;
    HFILE       hTempFile;
    RECPLU      RecPlu;
    RECPLU10    RecPlu10;
    MFINFO      MfInfo;
    UINT        unSize;

    usStat = PluSenseFile(FILE_PLU, &MfInfo);
    if (usStat != SPLU_COMPLETED) {   /* PLU file is not prepared */

        return TRUE;
    }

    if ((hMem = GlobalAlloc(GHND, FILE_MIGR_GLOBAL_LEN)) == 0) {
                                /* allocate memory from global area */

        return FALSE;
    }

    if ((lpszBuff = GlobalLock(hMem)) == NULL) { /* lock memory area */

        /* ----- Free memory area ----- */

        GlobalFree(hMem);

        return FALSE;
    }

    /* ----- Set block access mode ----- */

    memset((LPSTR)&EcmBlock, 0, sizeof(ECMBLOCK));
    EcmBlock.fsOption = BLOCK_READ;

    /* ----- Enter critical mode (to open plu file) ----- */

    if ((usStat = PluEnterCritMode(FILE_PLU,
                                   FUNC_BLOCK_ACCESS,
                                   &usHandle,
                                   &EcmBlock)) != SPLU_COMPLETED) {

        /* ----- Free memory area ----- */

        GlobalUnlock(hMem);
        GlobalFree(hMem);

        return FALSE;
    }

    /* ----- Get windows path ----- */

    GetWindowsDirectoryPep(szPath, sizeof(szPath));

    /* ----- Load string from resource ----- */

    LoadString(hResourceDll,
                   IDS_FILE_MIGR_FILE, szFile, sizeof(szFile));

    /* ----- Make full path file name ----- */

    wsPepf(szTempFile, WIDE("%s\\%s"), szPath, szFile);

    /* ----- Open temporary file ----- */

    if ((hTempFile = OpenFilePep(szTempFile,
                              &TempBuff,
                              (UINT)(OF_SHARE_DENY_NONE | OF_WRITE | OF_CREATE)))
                                   == 0) {

        /* ----- Exit critical mode ----- */

        PluExitCritMode(usHandle, 0);

        /* ----- Free memory area ----- */

        GlobalUnlock(hMem);
        GlobalFree(hMem);

        return FALSE;
    }

    for (;;) {

        /* ----- Read plu file by block ----- */

        if ((usStat = PluReadBlock(usHandle,
                                   (UCHAR *)lpszBuff,
                                   FILE_MIGR_GLOBAL_LEN,
                                   &usRead)) != SPLU_COMPLETED) {
            break;
        }

        /* ----- Copy file to mirror file----- */

        unSize = WriteFile(hTempFile, lpszBuff, usRead, &bytesWritten, NULL);

        if (unSize != usRead) {

            /* ----- Store error code ----- */

            usStat = SPLU_DEVICE_FULL;

            /* ----- Close temporary file ----- */

            CloseHandle(hTempFile);

            /* ----- Exit critical mode ----- */

            PluExitCritMode(usHandle, 0);

            /* ----- Free memory area ----- */

            GlobalUnlock(hMem);
            GlobalFree(hMem);

            return FALSE;
        }
    }

    /* ----- Exit critical mode ----- */

    usStat = PluExitCritMode(usHandle, 0);

    /* ----- Close temporary file ----- */

    CloseHandle(hTempFile);

    /* ----- Create New PLU file ----- */

    if ((usStat = PluCreateFile(FILE_PLU, MfInfo.usMaxRec)) != SPLU_COMPLETED) {

        /* ----- Delete temporary file ----- */

        DeleteFilePep(szTempFile);
		//OpenFile((LPCSTR)szTempFile, &TempBuff, OF_DELETE);

        /* ----- Free memory area ----- */

        GlobalUnlock(hMem);
        GlobalFree(hMem);

        return FALSE;
    }

    /* ----- Open mirror file ----- */

    if ((usStat = PluOpenMirror((UCHAR FAR *)szFile,
                                &usMirror)) != SPLU_COMPLETED) {

        /* ----- Delete temporary file ----- */

        DeleteFilePep(szTempFile);
		//OpenFile((LPCSTR)szTempFile, &TempBuff, OF_DELETE);

        /* ----- Free memory area ----- */

        GlobalUnlock(hMem);
        GlobalFree(hMem);

        return FALSE;
    }

    /* ----- Set report mode ----- */

    memset((LPSTR)&EcmCond, 0, sizeof(ECMCOND));
    EcmCond.fsOption = (REPORT_ACTIVE | REPORT_INACTIVE);

    /* ----- Enter critical mode (to get plu report) ----- */

    if ((usStat = PluEnterCritMode(usMirror,
                                   FUNC_REPORT_R10,
                                   &usHandle,
                                   &EcmCond)) != SPLU_COMPLETED) {

        /* ----- Close mirror file ----- */

        PluCloseMirror(usMirror);

        /* ----- Delete temporary file ----- */

        DeleteFilePep(szTempFile);
		//OpenFile((LPCSTR)szTempFile, &TempBuff, OF_DELETE);

        /* ----- Free memory area ----- */

        GlobalUnlock(hMem);
        GlobalFree(hMem);

        return FALSE;
    }

    for (;;) {

        /* ----- Read record from mirror file ----- */

        if ((usStat = PluReportRecord(usHandle,
                                      &RecPlu10,
                                      &usRead)) != SPLU_COMPLETED) {
            break;
        }

        memset(&RecPlu, 0, sizeof(RecPlu));
        memcpy(&RecPlu, &RecPlu10, sizeof(RecPlu10));

        /* ----- Assign record to PLU regular file ----- */

        if ((usStat = PluAddRecord(FILE_PLU, &RecPlu)) != SPLU_COMPLETED) {

            /* ----- Exit critical mode ----- */

            PluExitCritMode(usHandle, 0);

            /* ----- Close mirror file ----- */

            PluCloseMirror(usMirror);

            /* ----- Delete temporary file ----- */

            DeleteFilePep(szTempFile);
			//OpenFile((LPCSTR)szTempFile, &TempBuff, OF_DELETE);

            /* ----- Free memory area ----- */

            GlobalUnlock(hMem);
            GlobalFree(hMem);

            return FALSE;
        }
    }

    /* ----- Exit critical mode ----- */

    usStat = PluExitCritMode(usHandle, 0);

    /* ----- Close mirror file ----- */

    usStat = PluCloseMirror(usMirror);

    /* ----- Delete temporary file ----- */

    DeleteFilePep(szTempFile);
	//OpenFile((LPCSTR)szTempFile, &TempBuff, OF_DELETE);

    /* ----- Free memory area ----- */

    GlobalUnlock(hMem);
    GlobalFree(hMem);

    return TRUE;
}
#endif

/* ===== End of FILEMIGR.C ===== */