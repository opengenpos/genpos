/*
* ---------------------------------------------------------------------------
* Title         :   Binds File
* Category      :   File, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   FILEBIND.C
* Copyright (C) :   1993, NCR Corp. E&M OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract : This module provides the following functions
*               FileBind():         binds each file
*               FileFromEachFile(): binds temporary files to a file
* ---------------------------------------------------------------------------
* Update Histories
*
*   Date    : Ver.Rev. :   Name     : Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
* Feb-28-96 : 03.00.05 : M.Suzuki   : Add R3.1
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
#include    <r20_pif.h>

/* NCR2172 */
#include    <plu.h>
#include    <paraequ.h>
#include    <para.h>
#include    <pararam.h>

#include    "pepcomm.h"
#include    "trans.h"
#include    "file.h"
#include    "filel.h"

#include	"AW_Interfaces.h"

/************************************\
|*  Global Work Area Declarations   *|
\************************************/
FHEADER     Fheader;


/********************************************************************\
|*                                                                  *|
|*  Synopsis:   BOOL FileBind(HANDLE hInst, HWND hDlg, int iFile)   *|
|*              hInst   PEP window instance                         *|
|*              hDlg    handle of dialog box                        *|
|*              iFile   handle of file                              *|
|*                                                                  *|
|*  Return:     TRUE:   success                                     *|
|*              FALSE:  failure                                     *|
|*                                                                  *|
|* Description: This function binds each files.                     *|
|*                                                                  *|
\********************************************************************/
BOOL FileBind(HANDLE hInst, HWND hDlg, HANDLE iFile)
{
    WCHAR            i, szFname[FILE_MAXLEN_TEXT];
    WCHAR           szWork[FILE_LEN_ID];
    LONG            lSize, lOffset;
    UINT            nParaLen, nID;
    void            *pBufStart;
	DWORD			bytesWritten, fileError;

    lOffset = SetFilePointer(iFile, sizeof(FHEADER), NULL, FILE_BEGIN);
	//fileError = GetLastError();
	if (lOffset == -1/*!= NO_ERROR*/) {
        return FALSE;
    }

    /* ----- Finalize mass memory module,   Saratoga ----- */
    PluFinalize();

    /* bind each file   */
    for (i = 0, nID = IDS_FILE_NAME_DEPT; nID <= IDS_FILE_NAME_MLD; i++, nID++) {
        LoadString(hResourceDll, nID, szFname, PEP_STRING_LEN_MAC(szFname));
        if (FileFromEachFile(iFile, szFname, &lSize) == FALSE) {
            return FALSE;
        }
        /* set header   */
        Fheader.File.aRoom[i].lOffset = lOffset;
        Fheader.File.aRoom[i].lLen    = lSize;
        lOffset += lSize;
    }

	i = 10;		//Fingerprint file
	LoadString(hResourceDll, IDS_FILE_NAME_FPDB, szFname, PEP_STRING_LEN_MAC(szFname));
	if(FileFromEachFile( iFile, szFname, &lSize) == FALSE) {
		return FALSE;
	}
    Fheader.File.aRoom[i].lOffset = lOffset;
    Fheader.File.aRoom[i].lLen    = lSize;
    lOffset += lSize;

	i = 11;		//Reason Code Mnemonics file
	LoadString(hResourceDll, IDS_FILE_NAME_RSN, szFname, PEP_STRING_LEN_MAC(szFname));
	if(FileFromEachFile( iFile, szFname, &lSize) == FALSE) {
		return FALSE;
	}
    Fheader.File.aRoom[i].lOffset = lOffset;
    Fheader.File.aRoom[i].lLen    = lSize;
    lOffset += lSize;

    for (i = 14, nID = IDS_FILE_NAME_PLUINDEX; i < 16; i++, nID++) {
        LoadString(hResourceDll, nID, szFname, PEP_STRING_LEN_MAC(szFname));
        if (FileFromEachFile(iFile, szFname, &lSize) == FALSE) {
            return FALSE;
        }
        /* set header   */
        Fheader.File.aRoom[i].lOffset = lOffset;
        Fheader.File.aRoom[i].lLen    = lSize;
        lOffset += lSize;
    }

	i = 16;
	LoadString(hResourceDll, IDS_FILE_NAME_LOGOIDX,szFname, PEP_STRING_LEN_MAC(szFname));
	if(FileFromEachFile( iFile, szFname, &lSize) == FALSE) {
		return FALSE;
	}
    Fheader.File.aRoom[i].lOffset = lOffset;
    Fheader.File.aRoom[i].lLen    = lSize;
    lOffset += lSize;

    /* ----- Initialize mass memory module, Saratoga ----- */
    PluInitialize();

    pBufStart = &ParaMDC;		//we set the pointer to ParaMDC instead of Para, because the first 
								//information in Para is now file version information.
	// modify the save size so that we do not include the version string at the beginning
    nParaLen  = sizeof(Para) - sizeof(Para.szVer);

    WriteFile(iFile, pBufStart, nParaLen, &bytesWritten, NULL);
	if (/*_lwrite(iFile, pBufStart, nParaLen)*/bytesWritten != nParaLen) {
        return FALSE;
    }

    Fheader.Mem.Para.lOffset = lOffset;
    Fheader.Mem.Para.lLen    = nParaLen;
    lOffset += nParaLen;

    /* configuration     - cast to char * for _lwrite  */
    WriteFile(iFile, &Fconfig, sizeof(FCONFIG), &bytesWritten, NULL);
	if (bytesWritten != sizeof(FCONFIG)) {
        return FALSE;
    }

    Fheader.Mem.Config.lOffset = lOffset;
    Fheader.Mem.Config.lLen    = sizeof(FCONFIG);
    lOffset += sizeof(FCONFIG);

    /* transfer     */
    WriteFile(iFile, TransGetDataAddr(), TransGetDataLen(), &bytesWritten, NULL);
	if ( bytesWritten != (UINT)TransGetDataLen()) {
        return FALSE;
    }

    Fheader.Mem.Trans.lOffset = lOffset;
    Fheader.Mem.Trans.lLen    = (LONG)TransGetDataLen();

    /* bind header      */
    fileError = SetFilePointer(iFile, 0L, NULL, FILE_BEGIN);
//	fileError = GetLastError();
	if ( fileError == (DWORD)-1/* != NO_ERROR*/) {
        return FALSE;
    }

	/*
		Set the file identification information.

		This information must correspond to what is checked in function FileCheckHeader
		in file Fileopen.c which reports the PEP data file type.
	 */
    /* set ID   */
    LoadString(hResourceDll, IDS_FILE_ID_NAME, szWork, PEP_STRING_LEN_MAC(szWork));
    lstrcpyW(Fheader.Fid.szName, szWork);
    LoadString(hResourceDll, IDS_FILE_ID_CLASS, szWork, PEP_STRING_LEN_MAC(szWork));
    lstrcpyW(Fheader.Fid.szClass, szWork);
    LoadString(hResourceDll, IDS_FILE_ID_MODEL, szWork, PEP_STRING_LEN_MAC(szWork));
    lstrcpyW(Fheader.Fid.szModel, szWork);
    LoadString(hResourceDll, IDS_FILE_ID_VER, szWork, PEP_STRING_LEN_MAC(szWork));
    lstrcpyW(Fheader.Fid.szVer, szWork);
    wcsncpy(Fheader.Fid.szRptName, szCommRptSaveName, PEP_FILENAME_LEN);

	/* --- set Layout Manager file name Added JHHJ for release 2.0--- */
	wcsncpy(Fheader.Fid.szTouchLayoutName, szCommTouchLayoutSaveName, PEP_FILENAME_LEN); //ESMITH LAYOUT
	wcsncpy(Fheader.Fid.szKeyboardLayoutName, szCommKeyboardLayoutSaveName, PEP_FILENAME_LEN);
	wcsncpy(Fheader.szReceiptLogoName, szCommLogoSaveName,  PEP_FILENAME_LEN);
	wcsncpy(Fheader.Fid.szIconFileDirectory, szCommIconSaveName, PEP_FILENAME_LEN);

    /* save header   - cast to char * for _lwrite  */
    WriteFile(iFile, &Fheader, sizeof(FHEADER), &bytesWritten, NULL);
	if ( bytesWritten != sizeof(FHEADER)) {
        return FALSE;
    }

    return TRUE;

    hDlg = hDlg;
}

/************************************************************\
|*                                                          *|
|*  Synopsis:   BOOL FileFromEachFile(int iFile,            *|
|*                                    char *pszFileName,    *|
|*                                    LONG *plSize)         *|
|*              iFile       handle of file                  *|
|*              pszFileName pointer of file name            *|
|*              plSize      actual moving bytes             *|
|*                                                          *|
|*  Return:     TRUE:   success                             *|
|*              FALSE:  failure                             *|
|*                                                          *|
|* Description: This function creates the file.             *|
|*                                                          *|
\************************************************************/
BOOL FileFromEachFile(HANDLE iFile, WCHAR *pszFileName, LONG *plSize)
{
    SHORT       sDivFile;
    int         iLoop;
    LONG        lFileSize, lTemp;
    WORD        wRestBytes, wBytes;
    HANDLE      hGlobalMem;
    LPSTR       lpGlobalMem;
    BOOL        bReturn;
	DWORD		bytesWritten;

    int         nI = 0;

    *plSize = 0L;

    /* open file    */
    if ((sDivFile = PifOpenFile(pszFileName, "or")) >= 0) {

        /* get file size    */
        nI = PifSeekFile((USHORT)sDivFile, (ULONG)0x7fffffff, (ULONG *)&lFileSize);

        iLoop      = (int)(lFileSize / FILE_SIZE_SHORT);
        wRestBytes = (WORD)(lFileSize % FILE_SIZE_SHORT);

        nI = PifSeekFile((USHORT)sDivFile, (ULONG)0, (ULONG *)&lTemp);

        /* allocate global memory   */
        if ((hGlobalMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT,
                                      FILE_SIZE_SHORT)) != NULL) {

            /* lock global memory   */
            if ((lpGlobalMem = GlobalLock(hGlobalMem)) != NULL) {

                /* file to file */
                for (;;) {
                    if (iLoop == 0) wBytes = wRestBytes;
                    else            wBytes = (WORD)FILE_SIZE_SHORT;

                    /* read file to global memory   */
                    if (PifReadFile((USHORT)sDivFile,
                                    lpGlobalMem,
                                    wBytes) != wBytes) {
                        bReturn = FALSE;
                        break;
                    }
                        
                    /* write global memory to file  */
                    WriteFile(iFile, lpGlobalMem, wBytes, &bytesWritten, NULL);
					if ( bytesWritten != wBytes) {
                        bReturn = FALSE;
                        break;
                    }

                    if (iLoop == 0) {   /* end  */
                        *plSize = lFileSize;
                        bReturn = TRUE;
                        break;
                    } else  {           /* loop */
                        iLoop--;
                    }
                }

                /* unlock global memory */
                GlobalUnlock(hGlobalMem);
            } else {
                bReturn = FALSE;
            }
                
            /* free global memory   */
            GlobalFree(hGlobalMem);

        } else {
            bReturn = FALSE;
        }
        PifCloseFile((USHORT)sDivFile);
    } else {
        bReturn = TRUE;
    }
    return(bReturn);
}

/* ===== End of FILEBIND.C ===== */
