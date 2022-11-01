/*
* ---------------------------------------------------------------------------
* Title         :   Open File
* Category      :   File, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   FILEOPEN.C
* Copyright (C) :   1995, NCR Corp. E&M OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract : This module provides the following functions
*               FileOpen():         opens an old file
*               FileOpenProc():     procedure to open file
*               FileCheckHeader():  checks PEP's file
* ---------------------------------------------------------------------------
* Update Histories
*
*   Date    : Ver.Rev. :   Name     : Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
* Mar-13-95 : 03.00.00 : H.Ishida   : R3.0 enhance
* Feb-07-96 : 03.00.05 : M.Suzuki   : R3.0 enhanc1
* Sep-28-98 : 03.03.00 : A.Mitsui   : V3.3
* Jun-18-99 : 03.04.01 : hrkato     : Euro Enhancements & Windows 98
* Jan-28-99 : 01.00.00 : hrkato     : Saratoga
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
#include    <commdlg.h>
#include	<time.h>

#define NO_PARA_DEFINES
#define FILE_MIGRATION_PROTOTYPES

#include	<ecr.h>
#include	<pif.h>
#include    "pep.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "trans.h"
#include	"plu.h"
#include    "file.h"
#include	"paraequ.h"
#include	"para.h"
#include	"pararam.h"
#include    "filel.h"
#include    "filemigr.h"
#include    "filemgtb.c"



/************************************\
|*  Global Work Area Declarations   *|
\************************************/

WCHAR       szFileSaveName[FILE_MAXLEN];

static WCHAR		writeInfo[FILE_MAXERROR];
static WCHAR  wszFileVersion[128];

USHORT  usFileVersionInfo[4] = {0};


// -----------------------------------------------
//  General purpose dialog routines used to popup
//  a dialog informing the user that the PEP file
//  needs to be converted and asking whether to
//  to do the conversion or not.

BOOL WINAPI FileMigratePWDlgProc (HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT Paint;
    HDC         hDC;
    static  RECT          rRect;              /* dialog box area      */
    static  FILEMIGRIDX   Index;              /* checkbox list index  */
    static  FILEMIGRCHKGP Check;              /* select check item    */

    switch (wMsg) {
    case WM_INITDIALOG:

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

		SendThatUnicodeString(hDlg, IDD_FILE_CAPTION_VER, WM_SETTEXT, 0, (LPARAM) wszFileVersion);
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
                     (short)((rRect.bottom - rRect.top) * FILE_MIGRGP_MULTIPLY / FILE_MIGRGP_DIVIDE),
                     SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

        /* ----- set checkbox status all on ----- */

        memset((LPSTR)&Check, TRUE, sizeof(FILEMIGRCHKGP));

        return TRUE;

    case WM_PAINT:

        /* ----- get device context handle ----- */

        hDC = BeginPaint(hDlg, (PAINTSTRUCT FAR *)&Paint);

        /* ----- draw question icon ----- */

        DrawIcon(hDC,
                 FILE_MIGRGP_ICONX,
                 FILE_MIGRGP_ICONY,
                 LoadIcon(NULL, IDI_QUESTION));

        /* ----- release device context handle ----- */

        EndPaint(hDlg, (const PAINTSTRUCT FAR *)&Paint);

        return TRUE;

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
        case IDYES:
            EndDialog(hDlg, IDYES);
            return TRUE;

        case IDNO:
            EndDialog(hDlg, IDNO);
            return TRUE;
        }
        return FALSE;

    case WM_CLOSE:
       EndDialog(hDlg, IDNO);
       return TRUE;

    default:
        return FALSE;
    }
}


int FileMigratePieceWise (HWND hDlg, WCHAR* szPathFileName, int fRet)
{
    WCHAR    szFname[FILE_MAXLEN+1];
    int     iResult;
    DLGPROC InstDlgProc;
    WCHAR    szErrMsg[FILE_MAXLEN_TEXT];

    /* ----- Initialize ----- */
    memset(szFname, 0x00, sizeof(szFname));
    memcpy(szFname, szPathFileName, FILE_MAXLEN);

    /* create dialog    */
    InstDlgProc = MakeProcInstance(FileMigratePWDlgProc, hPepInst);
    iResult = DialogBoxPopup(hResourceDll, MAKEINTRESOURCEW(IDD_FILE_DLG_MIGRATEGP), hDlg, InstDlgProc);
    if (iResult == -1) {
        MessageBeep(MB_ICONEXCLAMATION);    /* make a tone  */
        LoadString(hResourceDll, IDS_FILE_ERR_CREATE, szErrMsg, PEP_STRING_LEN_MAC(szErrMsg));
        MessageBoxPopUpFileCaption(hDlg, szErrMsg, MB_ICONEXCLAMATION | MB_OK);
		iResult = IDNO;
    }

    FreeProcInstance(InstDlgProc);

    return iResult;
}


BOOL FileMigratePieceWiseFinish (HANDLE iFile)
{
	DWORD	   bytesRead, fileError;
	
	/* configuration        */
	fileError = SetFilePointer(iFile, Fheader.Mem.Config.lOffset, NULL, FILE_BEGIN);
	//fileError = GetLastError();
	if (fileError == -1) {
		return FALSE;
	}
	ReadFile(iFile, &Fconfig, Fheader.Mem.Config.lLen, &bytesRead, NULL);
	if (bytesRead != (DWORD)Fheader.Mem.Config.lLen) {
		return FALSE;
	}

	/* --- correct RAM DISK start address for previous version, because it
		did not calculate FAT address. (9/20/95) --- */

	Fconfig.ulStartAddr = (unsigned long)( FILE_MADDR + FILE_FAT_ADDR );
	Fconfig.usMemory = (FILE_SIZE_64KB * FILE_SIZE_48MBTOKB);  // set default Flash memory 48MB
	Fconfig.chKBType = FILE_KB_TOUCH;  // set default keyboard to be touch keyboard

	/* transfer             */
	fileError = SetFilePointer(iFile, Fheader.Mem.Trans.lOffset, NULL, FILE_BEGIN);
	//fileError = GetLastError();
	if ( fileError == -1) {
		return FALSE;
	}
	ReadFile(iFile, TransGetDataAddr(), Fheader.Mem.Trans.lLen, &bytesRead, NULL);
	if (bytesRead != (UINT)Fheader.Mem.Trans.lLen) {
		return FALSE;
	}

	/* --- set progarmmable report ini. file name R3.1 --- */
	wcsncpy(szCommRptSaveName, Fheader.Fid.szRptName, PEP_FILENAME_LEN);

	/* --- set Layout Manager file name Added JHHJ for release 2.0--- */
	wcsncpy(szCommTouchLayoutSaveName, Fheader.Fid.szTouchLayoutName, PEP_FILENAME_LEN); //ESMITH LAYOUT
	wcsncpy(szCommKeyboardLayoutSaveName, Fheader.Fid.szKeyboardLayoutName, PEP_FILENAME_LEN);
	wcsncpy(szCommLogoSaveName, Fheader.szReceiptLogoName, PEP_FILENAME_LEN);

	PepResetModFlag(PEP_MF_ALL, 0);                 /* reset change flag    */
	PepSetModFlag(hwndPepMain, PEP_MF_OPEN, 0);     /* set open flag        */

	return TRUE;
}



/************************************************************\
|*                                                          *|
|*  Synopsis:   BOOL FileOpen(HANDLE hInst, HWND hwnd)      *|
|*              hInst:   instance handle                    *|
|*              hwnd:    handle of the main window          *|
|*                                                          *|
|* Description: This function creates the "Open" dialog.    *|
|*                                                          *|
\************************************************************/
BOOL FileOpen(HANDLE hInst, HWND hwnd)
{
    int     iResult;
    WCHAR    szErrMsg[FILE_MAXLEN_TEXT];
	WCHAR    szVersion[FILE_LEN_ID];


    LoadFileCaption(hResourceDll, IDS_FILE_CAP_OPEN);
    if (PepQueryModFlag(PEP_MF_ALL, 0) == TRUE) {
                                        /* already changed file */
        MessageBeep(MB_ICONQUESTION);   /* make a tone          */
        LoadString(hResourceDll, IDS_FILE_ERR_CONT, szErrMsg, PEP_STRING_LEN_MAC(szErrMsg));
        if (MessageBoxPopUpFileCaption(hwnd, szErrMsg, MB_ICONQUESTION | MB_YESNO) == IDNO) {
            return FALSE;               /* cancel   */
        }
    }

    iResult = FileOpenProc(hInst, hwnd);

    if (iResult == TRUE) {
		LoadString(hResourceDll, IDS_FILE_ID_VER, szVersion, PEP_STRING_LEN_MAC(szVersion));
		wcsncpy(Para.szVer, szVersion,FILE_LEN_ID);
        return TRUE;
    } else {
        return FALSE;
    }
}

void FileMigrationFollowUp(HANDLE iFileHandle, WCHAR *szFileName, WCHAR *szWork, WCHAR *szFileTitle){
	FileMigratePieceWiseFinish (iFileHandle);
	CloseHandle(iFileHandle);						/* close file */
	lstrcpyW(szFileSaveName, szFileName);			/* save file name */
	wsPepf(szWork, WIDE(" - %s"), szFileTitle);
	PepSetFileName(hwndPepMain, szWork);			/* set file name */
	PepSetModFlag(hwndPepMain, PEP_MF_OPEN, 0);		/* set open flag */
}

//------------------------------------------------------------
// create a specific FileOpenFile procedure that can be used
// in other places. This function will open a file containing
// a PEP dataset, read in the dataset doing any conversions
// needed and then initialize the data structures so as
// to allow the user to modify the dataset.
//
BOOL FileReadInFile (HANDLE hInst, HWND hwnd, WCHAR *szFileName)
{
    WCHAR    szErrMsg[FILE_MAXLEN_TEXT];
	WCHAR	 szBuff[156];
	WCHAR	 szDesc[156];
	WCHAR	 szCutFile[FILE_MAXLEN];
    WCHAR    szFileTitle[FILE_MAXLEN];
    WCHAR    szWork[FILE_MAXLEN], szLogPath[FILE_MAXLEN];
    int      fRet;
    HANDLE   iFileHandle, iConvHandle;
    OFSTRUCT DummyData;
	SHORT	 sSaveOrNot;
	DWORD		bytesWritten, bytesToWrite;
	UCHAR    ParaArrayStorage[sizeof(UNINIRAM) * 4];
	UNINIRAM    *OldParaArray[2];

	CHAR     auchOP_DEPT2[]	=  "PARAMDEP";

	memset(writeInfo,'\0',sizeof(writeInfo));

	OldParaArray[0] = (UNINIRAM *)ParaArrayStorage;
	OldParaArray[1] = (UNINIRAM *)(ParaArrayStorage + (sizeof(UNINIRAM) * 2));

	//------
	// Modify the provided pathname so that if it is a
	// pathname rather than just the filename, we will
	// remove the path information leaving just the filename
	// to put it in the caption bar of our window.
	{
		WCHAR *p = szFileName + wcslen(szFileName);
		for (;p >= szFileName; p--) {
			switch (*p) {
			case '\\':
			case '/':
			case ':':
			//case '"':
				wcscpy(szFileTitle, ++p);
				p = szFileName;
			break;
			}
		}
	}
	{
		WCHAR *p = szFileSaveName + wcslen(szFileSaveName);
		for (;p >= szFileSaveName; p--) {
			switch (*p) {
			case '\\':
			case '/':
			case ':':
			//case '"':
				wcscpy(szCutFile, ++p);
				p = szFileSaveName;
			break;
			}
		}
	}


	// Modifies the provided pathname so that when log
	// file is created it uses the filename.log format
	// for the new log file that will be saved in the
	// same folder that the pep file originated in.
	//Creates the log file for write in and
#if 1
	GetPepTemporaryFolder(NULL, szLogPath, PEP_STRING_LEN_MAC(szLogPath));
#else
	GetPepModuleFileName(NULL, szLogPath, sizeof(szLogPath));
	szLogPath[wcslen(szLogPath)-8] = '\0';  //remove pep.exe from path
	wcscat(szLogPath, WIDE("\\"));
#endif
	wcsncat(szLogPath, PEPLOGPATH, 10);
	iConvHandle = CreateFileW(szLogPath,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL, OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	SetFilePointer(iConvHandle,FILE_TIMELEN,NULL,FILE_BEGIN);

    if ((iFileHandle = CreateFilePep(szFileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
						OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE) {

        MessageBeep(MB_ICONEXCLAMATION);    /* make a tone  */
        LoadString(hResourceDll, IDS_FILE_ERR_OPEN, szErrMsg, PEP_STRING_LEN_MAC(szErrMsg));
        MessageBoxPopUpFileCaption(hwnd, szErrMsg, MB_ICONEXCLAMATION | MB_OK);
        return FALSE;
    }

	/*SR 448,487 we now check to see if the PARAMDEP file exists.  This is the first file
	created when a pep file is opened.  By checking to see if this
	exists, we will know if there is a file open, or if PEP closed down improperly*/
	if( (PepQueryModFlag(PEP_MF_ALL, 0) == TRUE) &&
            (PepQueryModFlag(PEP_MF_OPEN, 0) == TRUE) &&(OpenFile(auchOP_DEPT2, &DummyData, OF_EXIST) != HFILE_ERROR))
	{
		memset(szBuff,0,sizeof(szBuff));
		MessageBeep(MB_ICONEXCLAMATION);    /* make a tone  */
        LoadString(hResourceDll, IDS_FILE_ERR_PREV_OPEN, szBuff, PEP_STRING_LEN_MAC(szBuff));
		wsPepf(szErrMsg, szBuff, szCutFile);
        sSaveOrNot = MessageBoxPopUpFileCaption(hwnd, szErrMsg, MB_ICONQUESTION | MB_YESNOCANCEL);
		switch(sSaveOrNot)
		{
			case IDYES:
				FileSave(hInst, hwnd);
			//no break statement because we still want to delete the files after we save
			case IDNO:
				FileTempDelete(hPepInst);
				break;
			case IDCANCEL: //Do nothing
				return FALSE;
			default:
				break;
		}
		memset(szBuff,0,sizeof(szBuff));
	}

    /*
		check the file header information to determine the version
		of PEP that created this data file.
	*/
	fRet = FileCheckHeader(iFileHandle);

	//Writes header for log file out
	wcscat(writeInfo,Para.szVer);
	bytesToWrite = FILE_NEWLINE - FILE_TIMELEN;
	WriteFile(iConvHandle,writeInfo,bytesToWrite,&bytesWritten,NULL);
	//Formatting for log file implementation
	WriteFile(iConvHandle,FORMATSTRNG,sizeof(FORMATSTRNG),&bytesWritten,NULL);
	SetFilePointer(iConvHandle,FILE_NEWLINE - sizeof(FORMATSTRNG) ,NULL,FILE_END);
	SetEndOfFile(iConvHandle);
	CloseHandle(iConvHandle);

	//Clearing the buffer and setting the default mnemonics back
	//This prevents unwanted items from being inserted into the
	//the Parameter buffer.
	memset(&Para, '\0', sizeof(Para));
	memcpy(&Para, lpPepParam, sizeof(Para));

	if (fRet == FILE_HEAD_VER_UNKNOWN) {
		// this is an unknown version of PEP file.
		memset(szBuff,0,sizeof(szBuff));
		MessageBeep(MB_ICONEXCLAMATION);    /* make a tone  */
        LoadString(hResourceDll, IDS_FILE_ERR_VERSION, szBuff, PEP_STRING_LEN_MAC(szBuff));
		wsPepf(szErrMsg, szBuff, szCutFile);
        MessageBoxPopUpFileCaption(hwnd, szErrMsg, MB_ICONQUESTION | MB_OK);
		return FALSE;
	} else if  ((fRet == FILE_HEAD_VER_2   )||	/* enhance file (Ver2.0) */
				(fRet == FILE_HEAD_VER_2_5 )||	/* enhance file (Ver2.5) */
				(fRet == FILE_HEAD_VER_3_0 )||	/* enhance file (Ver3.0) */
				(fRet == FILE_HEAD_VER_3_3 )||	/* enhance file (Ver3.3) */
				(fRet == FILE_HEAD_VER_3_4 )||	/* enhance file (Ver3.4) */
				(fRet == FILE_HEAD_VER_GP20)||
				(fRet == FILE_HEAD_VER_GP21)||
				(fRet == FILE_HEAD_VER_GP22)||
				(fRet == FILE_HEAD_VER_GP40)||
				(fRet == FILE_HEAD_VER_GP41)||
				(fRet == FILE_HEAD_VER_GP42)||
				(fRet == FILE_HEAD_VER_GP201)||
				(fRet == FILE_HEAD_VER_GP203)||
				(fRet == FILE_HEAD_VER_GP204)||
				(fRet == FILE_HEAD_OLDVER  )){
		CloseHandle( iFileHandle);         /* close file   */
		LoadString (hResourceDll, IDS_FILE_MIGR_UPGDT, szDesc, PEP_STRING_LEN_MAC(szDesc));
		LoadString (hResourceDll, IDS_FILE_MIGR_UPGRD, szBuff, PEP_STRING_LEN_MAC(szBuff));

		MessageBeep (MB_ICONEXCLAMATION);	/* make a tone  */
		MessageBoxPopUp (hwnd, szBuff, szDesc,MB_ICONEXCLAMATION | MB_OK);
		return FALSE;
 	} else if(fRet == FILE_HEAD_VER_GP205){
         if(FileMigratePieceWise(hwnd, szFileName, fRet) == IDYES) {
		    SetCursor(LoadCursor(NULL, IDC_WAIT));  /* set wait cursor  */

			FileMigrateGP_205_Files (iFileHandle, (UNINIRAM_HGV_2_0_5 *)OldParaArray[0], fRet);
			FileMigrateGP_205_212_Para ((UNINIRAM_HGV_2_0_5 *)OldParaArray[0], (UNINIRAM_HGV_2_1_2 *)OldParaArray[1]);
			FileMigrateGP_212_220_Para ((UNINIRAM_HGV_2_1_2 *)OldParaArray[1], (UNINIRAM_HGV_2_2_0 *)OldParaArray[0]);
			FileMigrateGP_220_221_Para ((UNINIRAM_HGV_2_2_0 *)OldParaArray[0], &Para);
			FileMigrationFollowUp(iFileHandle,szFileName,szWork,szFileTitle);

	        SetCursor(LoadCursor(NULL, IDC_ARROW)); /* set normal cursor*/
			return TRUE;
         } else {
           CloseHandle(iFileHandle);                         /* close file */
           return FALSE;
         }
	} else if(fRet == FILE_HEAD_VER_GP210){
         if(FileMigratePieceWise(hwnd, szFileName, fRet) == IDYES) {
			SetCursor(LoadCursor(NULL, IDC_WAIT));  /* set wait cursor  */

			FileMigrateGP_210_Files (iFileHandle, (UNINIRAM_HGV_2_1_0 *)OldParaArray[0], fRet);
			FileMigrateGP_210_220_Para ((UNINIRAM_HGV_2_1_0 *)OldParaArray[0], (UNINIRAM_HGV_2_2_0 *)OldParaArray[1]);
			FileMigrateGP_220_221_Para ((UNINIRAM_HGV_2_2_0 *)OldParaArray[1], &Para);
			FileMigrationFollowUp(iFileHandle,szFileName,szWork,szFileTitle);

	        SetCursor(LoadCursor(NULL, IDC_ARROW)); /* set normal cursor*/
			return TRUE;
         } else {
           CloseHandle(iFileHandle);                         /* close file */
           return FALSE;
         }
	} else if(fRet == FILE_HEAD_VER_GP212){
         if(FileMigratePieceWise(hwnd, szFileName, fRet) == IDYES) {

			SetCursor(LoadCursor(NULL, IDC_WAIT));  /* set wait cursor  */
			
			FileMigrateGP_212_Files (iFileHandle, OldParaArray[0], fRet);

			FileMigrateGP_212_220_Para ((UNINIRAM_HGV_2_1_2 *)OldParaArray[0], (UNINIRAM_HGV_2_2_0 *)OldParaArray[1]);
			FileMigrateGP_220_221_Para ((UNINIRAM_HGV_2_2_0 *)OldParaArray[1], &Para);
			FileMigrationFollowUp(iFileHandle,szFileName,szWork,szFileTitle);
			
	        SetCursor(LoadCursor(NULL, IDC_ARROW)); /* set normal cursor*/
			return TRUE;
         } else {
           CloseHandle(iFileHandle);                         /* close file */
           return FALSE;
         }
	} else if(fRet == FILE_HEAD_VER_GP220){
         if(FileMigratePieceWise(hwnd, szFileName, fRet) == IDYES) {

			SetCursor(LoadCursor(NULL, IDC_WAIT));  /* set wait cursor  */
			
			FileMigrateGP_212_Files (iFileHandle, (UNINIRAM_HGV_2_2_0 *)OldParaArray[0], fRet);
			FileMigrateGP_220_221_Para ((UNINIRAM_HGV_2_2_0 *)OldParaArray[0], &Para);

			FileMigrationFollowUp(iFileHandle,szFileName,szWork,szFileTitle);
			
	        SetCursor(LoadCursor(NULL, IDC_ARROW)); /* set normal cursor*/
			return TRUE;
         } else {
           CloseHandle(iFileHandle);                         /* close file */
           return FALSE;
         }
	}
	else if (fRet == FILE_HEAD_VER_GP221 || fRet == FILE_HEAD_VER_GP222 || fRet == FILE_HEAD_VER_GP230) {
		;   // drop through and divide up the file as this is the current version.
	} else if (fRet == FILE_HEAD_VER_GP231) {
		;   // drop through and divide up the file as this is the current version.
	} else {
		CloseHandle( iFileHandle);         /* close file   */
		LoadString (hResourceDll, IDS_FILE_ERR_NOTFILE, szErrMsg, PEP_STRING_LEN_MAC(szErrMsg));

		MessageBeep (MB_ICONEXCLAMATION);    /* make a tone  */
		MessageBoxPopUpFileCaption (hwnd, szErrMsg, MB_ICONEXCLAMATION | MB_OK);	/* output error message */
		return FALSE;
	}

 /*******************************************************************************/
    SetCursor(LoadCursor(NULL, IDC_WAIT));  /* set wait cursor  */


    /* delete old file  */
    FileTempDelete(hPepInst);
	wcscpy(szWork, WIDE(" - "));

	// Modify the provided pathname so that if it is a
	// pathname rather than just the filename, we will
	// remove the path information leaving just the filename
	// to put it in the caption bar of our window.
	{
		WCHAR *p = szFileName + wcslen(szFileName);
		for (;p >= szFileName; p--) {
			switch (*p) {
			case '\\':
			case '/':
			case ':':
				wcscat(szWork, ++p);
				p = szFileName;
			break;
			}
		}
	}

    if (FileDivide(hwnd, iFileHandle)) {  /* divide file  */
        CloseHandle(iFileHandle);                         /* close file       */
        lstrcpyW(szFileSaveName, szFileName);    /* save file name   */

        SetCursor(LoadCursor(NULL, IDC_ARROW)); /* set normal cursor*/
        PepSetFileName(hwndPepMain, szWork);    /* set file name    */
    } else {
        CloseHandle(iFileHandle);         /* close file   */

		/* ----- Finalize mass memory module, Saratoga ----- */
		PluFinalize();

        SetCursor(LoadCursor(NULL, IDC_ARROW)); /* set normal cursor*/

        MessageBeep(MB_ICONEXCLAMATION);    /* make a tone  */
        LoadString(hResourceDll,
                   IDS_FILE_ERR_LOAD,
                   szErrMsg, PEP_STRING_LEN_MAC(szErrMsg));
        MessageBoxPopUpFileCaption(hwnd,    /* output error message */
                   szErrMsg,
                   MB_ICONEXCLAMATION | MB_OK);

    }
//-----
	return TRUE;
}

/****************************************************************\
|*                                                              *|
|*  Synopsis:   BOOL FileOpenProc(HANDLE hInst, HWND hwnd)      *|
|*              hInst:   instance handle                        *|
|*              hwnd:    handle of the main window              *|
|*                                                              *|
|* Description: This function is the procedure at               *|
|*              selecting "Open...".                            *|
\****************************************************************/
BOOL FileOpenProc(HANDLE hInst, HWND hwnd)
{
    static  WCHAR    szFileName[FILE_MAXLEN];

    WCHAR    szErrMsg[FILE_MAXLEN_TEXT];
    WCHAR    szFileTitle[FILE_MAXLEN];
    WCHAR    szFilter[FILE_MAXLEN];
	WCHAR	szInitialPath[FILE_MAXLEN];
    WCHAR    chReplace;
    UINT    cbString, i;
    OPENFILENAMEW    ofn;
	WCHAR	*p;
	int		loopCount, loopFlag;

    if ((cbString = LoadString(hResourceDll, IDS_FILE_FILTER, szFilter,
          PEP_STRING_LEN_MAC(szFilter))) == 0) {
        MessageBeep(MB_ICONEXCLAMATION);    /* make a tone  */
        LoadString(hResourceDll,
                   IDS_FILE_ERR_LOAD, szErrMsg, PEP_STRING_LEN_MAC(szErrMsg));
        MessageBoxPopUpFileCaption(hwnd,            /* output error message */
                   szErrMsg,
                   MB_ICONEXCLAMATION | MB_OK);
        return FALSE;
    }
	// Modify the filter string to make it a proper filter string by replacing
	// the special character with a binary 0 ('\0') so that the filter
	// will work properly.  Normally the special character is a |
    chReplace = 0x7c;//szFilter[cbString - 1];
    for (i=0; szFilter[i] != '\0'; i++) {
        if (szFilter[i] == chReplace) {
            szFilter[i] = '\0';
        }
    }

	//RPH SR# 102, 103 1/10/03
	//set the path to the open PEP file by setting the initial path
	//in ofn parse the open file name to get the path if no file is open
	//defaults to the last directory file open occured.
	wcscpy(szInitialPath, szFileName);

	if(szInitialPath[0] != '\0')
	{
		loopCount = 0; //counts loop at end and gives the amount to copy

		for(loopFlag = 1, p = szInitialPath + wcslen(szInitialPath); loopFlag && p > szInitialPath; p--)
		{
			switch(*p) {
			case '/':
			case '\\':
				szInitialPath[wcslen(szInitialPath) - wcslen(p)] = '\0';
				loopFlag = 0;
				break;
			}
			loopCount++;
		}
	}

    memset(&ofn, 0, sizeof(OPENFILENAMEW));
    szFileName[0] = '\0';
    ofn.lStructSize = sizeof(OPENFILENAMEW);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = szFilter;
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = sizeof(szFileName);
    ofn.lpstrFileTitle = szFileTitle;
	ofn.lpstrInitialDir = szInitialPath;
    ofn.nMaxFileTitle = sizeof(szFileTitle);
    ofn.Flags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
    if (!GetPepOpenFileName(&ofn)) {
        return FALSE;
    }

    return FileReadInFile (hInst, hwnd, ofn.lpstrFile);
}

/****************************************************************\
|*                                                              *|
|*  Synopsis:   BOOL FileCheckHeader(int iFile)                 *|
|*                                                              *|
|* Description: This function checks PEP's file                 *|
|*                                                              *|
\****************************************************************/
ULONG FileCheckHeader(HANDLE iFile)
{
    FILEID  Fid;
	FILEID_OLD Fid_old;
    WCHAR    szWork[FILE_LEN_ID];
    WCHAR    szModel[FILE_LEN_ID];
	WCHAR    szVerTemp [32], *pszVerTemp;
	DWORD	bytesRead;
	UINT	myCodePage;
	int     nVI = 0;

	memset(usFileVersionInfo, 0, sizeof(usFileVersionInfo));

    /* read ID  */
	//If success then Multilingual Version File
    ReadFile(iFile, &Fid, sizeof(FILEID), &bytesRead, NULL);
	wcscpy (szVerTemp, Fid.szVer);
	wcscpy (wszFileVersion, Fid.szVer);
	if ( bytesRead == sizeof(FILEID))
	{
		if ( lstrcmpiW(Fid.szModel, WIDE("HG V2.0")) == 0)
		{
			/* Fid.szVer is used to distinguish between the 2.0 builds prior to the
				ability of multiple layouts and after. If the file is prior to
				version 2.0.0.26, then the file header must be converted to take into
				effect another layout and an undefined data section which is used to
				offset the file header from the data just incase more information is
				to be added into the FHEADER structure. ESMITH
			*/
			if( lstrcmpiW(Fid.szVer, WIDE("v2.00.00")) == 0)
			{
				usFileVersionInfo[0] = 2;
				usFileVersionInfo[3] = 0;
				wcscat(writeInfo,Fid.szVer);
				return FILE_HEAD_VER_GP40; /* NHPOS Release 2.0 multi-lingual */
			}
			if( lstrcmpiW(Fid.szVer, WIDE("v2.0.0.26")) == 0)
			{
				usFileVersionInfo[0] = 2;
				usFileVersionInfo[3] = 26;
				wcscat(writeInfo,Fid.szVer);
				return FILE_HEAD_VER_GP41; /* NHPOS Release 2.0.0.26 multi-lingual */
			}
			if( lstrcmpiW(Fid.szVer, WIDE("v2.0.0.31")) == 0)
			{
				usFileVersionInfo[0] = 2;
				usFileVersionInfo[3] = 31;
				wcscat(writeInfo,Fid.szVer);
				return FILE_HEAD_VER_GP42; /* NHPOS Release 2.0.0.26 multi-lingual */
			}

			/*
				If we have gotten this far then the data file is of some version of PEP
				that is not one of the milestone versions.  So now lets figure out the
				Build Number and the Issue Number.  This suffices for Rel 2.0.3 but for
				later releases such as Rel 2.1.0 and others where the Minor Number and the
				Major Number will change, we will need to add additional logic for file
				conversion.

				nVersionNumbers contains the various numbers in the release: nVersionNumbers[0]
				contains the Major Number, nVersionNumbers[1] the Minor Number, nVersionNumbers[2]
				the Issue Number, and nVersionNumbers[3] the build number.

				We process the dot (.) numbers first then we need to process the vN with the
				Major Number last.
			 */
			wcscpy (szVerTemp, Fid.szVer);
			pszVerTemp = wcsrchr (szVerTemp, L'.');
			for (nVI = 3; pszVerTemp && (nVI > 0); nVI--)
			{
				pszVerTemp++;
				usFileVersionInfo[nVI] = _wtoi(pszVerTemp);
				pszVerTemp--; *pszVerTemp = 0;
				pszVerTemp = wcsrchr (szVerTemp, L'.');
			}
			if (nVI == 0)
			{
				pszVerTemp = szVerTemp + 1;
				usFileVersionInfo[nVI] = _wtoi(pszVerTemp);
			}

			// check if all 4 numbers found and Major Number and Build Number are
			// non-zero.  These two numbers should always be non-zero whereas the
			// Minor Number and the Issue Number could possibly be zero.

			if (nVI == 0 && usFileVersionInfo[0]){
				if (FILE_HEAD_VER_MAJOR == 2){
					if (FILE_HEAD_VER_MINOR == 0){
						// checks for Rel 2.0.x to determine if we have a file that is
						//   - Rel 2.0.0 or Rel 2.0.1
						//   - Rel 2.0.3
						if (FILE_HEAD_VER_ISSUE < 3)
							return FILE_HEAD_VER_GP201;
						if (FILE_HEAD_VER_ISSUE == 3)					
							return FILE_HEAD_VER_GP203;
						if (FILE_HEAD_VER_ISSUE == 4)
							return FILE_HEAD_VER_GP204;
						if (FILE_HEAD_VER_ISSUE == 5)
							return FILE_HEAD_VER_GP205;
					} else if(FILE_HEAD_VER_MINOR == 1){
						if (FILE_HEAD_VER_ISSUE == 0)
							return FILE_HEAD_VER_GP210;
						else if (FILE_HEAD_VER_ISSUE == 2)
							return FILE_HEAD_VER_GP212;
						else if (FILE_HEAD_VER_ISSUE == 4)
							return FILE_HEAD_VER_GP214;
						else
							return FILE_HEAD_VER_UNKNOWN;
					} else if(FILE_HEAD_VER_MINOR == 2){
						if(FILE_HEAD_VER_ISSUE == 0)
							return FILE_HEAD_VER_GP220;
						else if(FILE_HEAD_VER_ISSUE == 1)
							return FILE_HEAD_VER_GP221;
						else if(FILE_HEAD_VER_ISSUE == 2)
							return FILE_HEAD_VER_GP222;
					}
					else if (FILE_HEAD_VER_MINOR == 3) {
						if (FILE_HEAD_VER_ISSUE == 0)
							return FILE_HEAD_VER_GP230;
						if (FILE_HEAD_VER_ISSUE == 1)
							return FILE_HEAD_VER_GP231;
					} else {
						return FILE_HEAD_VER_UNKNOWN;
					}
				}
			}
		}
		else
		{
			myCodePage = getCodePage();
			//If the File is not Multilingual then Check if Old Pep file
			memcpy(&Fid_old, &Fid, sizeof(FILEID_OLD));
			MultiByteToWideChar(myCodePage, 0, Fid_old.szClass, -1, Fid.szClass, sizeof(Fid_old.szClass));
			MultiByteToWideChar(myCodePage, 0, Fid_old.szModel, -1, Fid.szModel, sizeof(Fid_old.szModel));
			MultiByteToWideChar(myCodePage, 0, Fid_old.szName, -1, Fid.szName, sizeof(Fid_old.szName));
			MultiByteToWideChar(myCodePage, 0, Fid_old.szRptName, -1, Fid.szRptName, sizeof(Fid_old.szRptName));
			MultiByteToWideChar(myCodePage, 0, Fid_old.szVer, -1, Fid.szVer, sizeof(Fid_old.szVer));

			LoadString(hResourceDll, IDS_FILE_ID_NAME, szWork, PEP_STRING_LEN_MAC(szWork));
			LoadString(hResourceDll, IDS_FILE_ID_MODEL, szModel, PEP_STRING_LEN_MAC(szModel));

			wcscat(writeInfo,Fid.szVer);

			if (! lstrcmpiW(Fid.szModel, szModel)) {            /* success  */
				return FILE_HEAD_SUCCESS;
			} else if (! lstrcmpiW(Fid.szModel, WIDE("HP R3.3"))) {
				  return FILE_HEAD_VER_3_3;
			} else if (! lstrcmpiW(Fid.szModel, WIDE("HP R3.1"))) {   /* enhance file (Ver3.1) , V3.3*/
				return FILE_HEAD_VER_3_1;
			} else if (! lstrcmpiW(Fid.szModel, WIDE("HP V3.4"))) {   /* enhance file (Ver3.1) , V3.4*/
				  return FILE_HEAD_VER_3_4;
			} else if (! lstrcmpiW(Fid.szModel, WIDE("GP V22"))) {   /* enhance file (Ver3.1) , V3.4*/
				  return FILE_HEAD_VER_GP22;
			} else if (! lstrcmpiW(Fid.szModel, WIDE("GP V21"))) {   /* enhance file (Ver3.1) , V3.4*/
				  return FILE_HEAD_VER_GP21;
			} else if (! lstrcmpiW(Fid.szModel, WIDE("GP R20"))) {   /* enhance file (Ver3.1) , V3.4*/
				  return FILE_HEAD_VER_GP20;
			} else if (! lstrcmpiW(Fid.szModel, WIDE("HG V1.0"))) {   /* NHPOS Release 1.x for 7448 */
				  return FILE_HEAD_VER_GP30;
			}
		}
	}

    return FILE_HEAD_INVALID;                 /* This file isn't PEP's file */
}

/* ===== End of FILEOPEN.C ===== */
