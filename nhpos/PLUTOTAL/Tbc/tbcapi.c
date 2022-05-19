/*************************************************************************************************
 *
 *        *=*=*=*=*=*=*=*=*
 *        *  NCR 2172     *             AT&T GIS Corporation, E&M OISO
 *     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 2000-
 *    <|\/~               ~\/|>
 *   _/^\_                 _/^\_
 *
 *************************************************************************************************/
/*========================================================================*\
*   Title              : Totaldata (background/backup) copy module
*   Category           : Plutotal module, NCR2172 Saratoga application
*   Program Name       : tbcapi.c
*  ------------------------------------------------------------------------
*   Compiler    : MS-VC++ Ver.6.00 by Microsoft Corp.
*   Options     :
*  ------------------------------------------------------------------------
*   Abstract           :The provided function names are as follows.
*                        TbcCommand()
*
*  ------------------------------------------------------------------------
*    Update Histories   :
*
*   Date        Ver.Rev  :NAME         :Description
*   May-30-00 : 00.00.01 :m.teraki     :Initial
\*=======================================================================*/

/* includes ------------------------------------------------------------------------------------ */
#include <windows.h>
#include <string.h>

#include "ecr.h"
#include "pif.h"
#include "plulog.h"
#include "tbc.h"
#include "tbcin.h"
#include "tbctchar.h"

/* local typedefs ------------------------------------------------------------------------------ */

/* local macros -------------------------------------------------------------------------------- */

/* public symbols ------------------------------------------------------------------------------ */
LONG  TbcCommand(TBC_CMD *);    /* TBC api entry point */
TCHAR g_szTbcRAMDiskPath[MAX_PATH];

/* local symbols ------------------------------------------------------------------------------- */
static DWORD         TbcInitialize(VOID);   /* TBC initializer */
static TBC_COPYSPEC *TbcInitCopySpec(TBC_CMD *);
static DWORD         TbcReadFile(TBC_PATHSPEC *);
static DWORD         TbcCheckDestinationStorageSpace(CONST TBC_COPYSPEC *);
static VOID          TbcGetFileSizeExists (CONST TBC_COPYSPEC *, ULARGE_INTEGER *);
static VOID          TbcGetPeakSizeDuringCopy(CONST TBC_COPYSPEC *, ULARGE_INTEGER *);
static VOID          TbcGetRAMDiskPath(VOID);

/*************************************************************************************************
 * FUNCTION : TbcCommand
 * INPUT    : cmd : TBC command packet
 * OUTPUT   : cmd : result code, and data (if nessesary)
 * RETURNS  : >= 0 : success
 *          : <  0 : command is not valid
 * DESC.    : entry point from the outside of this .lib file
 *************************************************************************************************/
LONG
TbcCommand(TBC_CMD *tbcCmd)
{
    LONG          lRet = -1;
    TBC_COPYSPEC *tbcCopySpec;  /* copy specification descriptor */
    static BOOL   bInitializedYet = FALSE;  /* guard re-initialize */
    DWORD         dwDestFree;               /* free space in destineaion storage area [bytes] */

    switch (tbcCmd->dwCmdId) {
    case TBC_CMDID_INIT:
        lRet = 0;

        if (bInitializedYet == TRUE) {
            tbcCmd->dwResult = TBC_RSLT_ERROR;
            break;
        }

        tbcCmd->dwResult = TbcInitialize();

        if (tbcCmd->dwResult == TBC_RSLT_COMPLETE) {
            bInitializedYet = TRUE;
        }
        break;

    case TBC_CMDID_STARTCOPY:
        lRet = 0;

        if (bInitializedYet != TRUE) {
            tbcCmd->dwResult = TBC_RSLT_DNY_ERROR;
            break;
        }

        tbcCmd->dwResult = TbcGetThreadState();
        if (tbcCmd->dwResult == TBC_RSLT_BUSY) {    /* another thread is working yet */
            tbcCmd->dwResult = TBC_RSLT_DNY_BUSY;
            break;
        }
        else if (tbcCmd->dwResult == TBC_RSLT_COMPLETE) {   /* acceptable (maybe) */
            ;   /* do nothing here and continue */
        }
        else { /* unknown error has occured */
            tbcCmd->dwResult = TBC_RSLT_DNY_ERROR;
            break;
        }

        /* initialize all path specification descriptor */
        tbcCopySpec = TbcInitCopySpec(tbcCmd);
        if (tbcCopySpec == NULL) {
            DWORD dwErr;
            dwErr = GetLastError();
            if (dwErr == ERROR_NOT_ENOUGH_MEMORY) {
                tbcCmd->dwResult = TBC_RSLT_DNY_NOHEAP;
            }
            else {
                TbcAbort(MODULE_PLUTOTAL_TBCAPI, dwErr);
                tbcCmd->dwResult = TBC_RSLT_DNY_ERROR;
            }
            break;
        }

        /* check if destination storage area is enough or not */
        dwDestFree = TbcCheckDestinationStorageSpace(tbcCopySpec);
        if (dwDestFree == TBC_RSLT_DNY_NOHEAP) {
            tbcCmd->dwResult = TBC_RSLT_DNY_NOHEAP;
            break;
        }
        if (dwDestFree == TBC_RSLT_DNY_NOSTOR) {
            tbcCmd->dwResult = TBC_RSLT_DNY_NOSTOR;
            break;
        }


        /* issue copy request to another thread */
        TbcCopyRequest(tbcCopySpec);

        /* remember result of check destination storage area and set to result code */
        if (dwDestFree == TBC_RSLT_ACC_LOWSTOR) {
            tbcCmd->dwResult = TBC_RSLT_ACC_LOWSTOR;
        }
        else {
            tbcCmd->dwResult = TBC_RSLT_ACC_CLEAR;
        }

        /* tbcCopySpec object will free-ed when copy was accepted */
        break;

    case TBC_CMDID_GETSTATUS:
        lRet = 0;

        if (bInitializedYet != TRUE) {  /* not initialized */
            tbcCmd->dwResult = TBC_RSLT_ERROR;
            break;
        }

        tbcCmd->dwResult = TbcGetThreadState();

        break;

    default:
        break;
    }
    return lRet;
}

/*************************************************************************************************
 * FUNCTION : TbcInitialize
 * INPUT    : NONE
 * OUTPUT   : NONE
 * RETURNS  : TBC_RSLT_COMPLETE : initialized successfully
 *          : TBC_RSLT_ERROR    : error
 * DESC.    : subprocedure of TbcCommand which dispatches a command contains TBC_CMDID_INIT.
 *************************************************************************************************/
static DWORD
TbcInitialize(VOID)
{
    TbcGetRAMDiskPath();

    TbcSyncInit();   /* Initialize sync module */

    TbcThreadInit();    /* Initialize thread module */

    return TBC_RSLT_COMPLETE;
}

/*************************************************************************************************
 * FUNCTION : TbcInitCopySpec
 * INPUT    : tbcCmd      : command descriptor
 * OUTPUT   : NONE
 * RETURNS  : != NULL : success (copy specification descriptor)
 *          : == NULL : error
 * DESC.    : all memory for path/filename string is allocated here
 *          : free() is called if malloc() failed.
 *************************************************************************************************/
static TBC_COPYSPEC *
TbcInitCopySpec(TBC_CMD *tbcCmd)
{
    TBC_COPYSPEC *tbcCopySpec;
    TBC_COPYSPEC *tbcRet;
    DWORD dwIdx;
    TBC_PATHSPEC *tbcPathSpec;
    TCHAR szTmp[MAX_PATH];
    TCHAR *pTmp;

    /* allocate memory for copy specification descriptor */
    tbcCopySpec = malloc(sizeof(TBC_COPYSPEC));

    if (tbcCopySpec == NULL) {
        return NULL;
    }
    memset(tbcCopySpec, TEXT('\0'), sizeof(TBC_COPYSPEC));
    tbcRet = tbcCopySpec;

    /* initialize count of src-dst pair */
    tbcCopySpec->dwCount = (tbcCmd->dwLen / sizeof(TCHAR *)) - 1;

    /* allocate for all path specification descriptor */
    tbcCopySpec->tbcPathSpec = malloc(sizeof(TBC_PATHSPEC) * tbcCopySpec->dwCount);
    if (tbcCopySpec->tbcPathSpec == NULL) {
        TbcDeInitCopySpec(tbcCopySpec);
        return NULL;
    }
    memset(tbcCopySpec->tbcPathSpec, TEXT('\0'), sizeof(TBC_PATHSPEC) * tbcCopySpec->dwCount);

    /* allocate all path strings */
    tbcPathSpec = tbcCopySpec->tbcPathSpec;
    for (dwIdx = 0; dwIdx < tbcCopySpec->dwCount; dwIdx++, tbcPathSpec++) {

        /* create source filename and source path */
        TbcStrcpy(szTmp, ((TCHAR **)tbcCmd->pvData)[dwIdx]);
        pTmp = TbcStrrchr(szTmp, TEXT('\\')) + 1;
        tbcPathSpec->szSrcName = malloc((TbcStrlen(pTmp) + 1) * sizeof(TCHAR));
        if (tbcPathSpec->szSrcName == NULL) {
            tbcRet = NULL;
            break;
        }
        TbcStrcpy(tbcPathSpec->szSrcName, pTmp);

        *pTmp = TEXT('\0'); /* pTmp points to first letter of source filename. put null there */
        tbcPathSpec->szSrcPath = malloc((TbcStrlen(szTmp) + 1) * sizeof(TCHAR));
        if (tbcPathSpec->szSrcPath == NULL) {
            tbcRet = NULL;
            break;
        }
        TbcStrcpy(tbcPathSpec->szSrcPath, szTmp);

        /* create destination filename */
        tbcPathSpec->szDstName = malloc((TbcStrlen(tbcPathSpec->szSrcName) + 1) * sizeof(TCHAR));
        if (tbcPathSpec->szDstName == NULL) {
            tbcRet = NULL;
            break;
        }
        TbcStrcpy(tbcPathSpec->szDstName, tbcPathSpec->szSrcName);

        /* determine temporary filename */
        TbcStrcpy(szTmp, TEXT("$"));
        TbcStrcat(szTmp, tbcPathSpec->szSrcName);
        pTmp = szTmp;
        tbcPathSpec->szTmpName = malloc((TbcStrlen(pTmp) + 1) * sizeof(TCHAR));
        if (tbcPathSpec->szTmpName == NULL) {
            tbcRet = NULL;
            break;
        }
        TbcStrcpy(tbcPathSpec->szTmpName, szTmp);

        /* read all data in source file */
        if (TbcReadFile(tbcPathSpec) == 0) {
            tbcRet = NULL;
            break;
        }
    }

    /* next data is destination path */
    TbcStrcpy(szTmp, ((TCHAR **)tbcCmd->pvData)[dwIdx]);
    tbcCopySpec->szDstPath = malloc((TbcStrlen(szTmp) + 1) * sizeof(TCHAR));
    if (tbcCopySpec->szDstPath == NULL) {
        tbcRet = NULL;
    }
    else {
        TbcStrcpy(tbcCopySpec->szDstPath, szTmp);
    }

    /* if malloc() has failed, free all heap blocks associated to tbcCopySpec */
    if (tbcRet == NULL) {
        TbcDeInitCopySpec(tbcCopySpec);
        tbcCopySpec = NULL;
    }

    return tbcCopySpec;
}

/*************************************************************************************************
 * FUNCTION : TbcReadFile
 * INPUT    : tbcPathSpec : TBC_PATHSPEC object
 * OUTPUT   : NONE
 * RETURNS  : 0        : failed
 *          : non-zero : success
 * DESC.    : do ram -> heap copy and issue request to copy thread
 *************************************************************************************************/
#define DT_SCER     1
#ifdef DT_SCER
#define SLEEP_MSEC  100
#endif
#define BLOCK_SIZE  1024*63
//#define BLOCK_SIZE  4096
static DWORD
TbcReadFile(TBC_PATHSPEC *tbcPathSpec)
{
    /* --- BEGIN: change for doller tree scer 12/05/00 ---*/
    /* deleted...
    TCHAR  szSrcFilePath[MAX_PATH];
    HANDLE hSrc;
    BOOL   bRet;
    */
    BOOL   fTempDisk;
    /* --- END: change for doller tree scer 12/05/00 ---*/
    DWORD  dwBytesRead;
    DWORD  dwRemain;
    UCHAR *pTmp;
    SHORT  sPifHandle;
    USHORT usRead;

    /* --- BEGIN: change for doller tree scer 12/05/00 ---*/
    /* deleted...
    / carete source path+filename /
    TbcStrcpy(szSrcFilePath, tbcPathSpec->szSrcPath);
    TbcStrcat(szSrcFilePath, tbcPathSpec->szSrcName);

    / open source file /
    hSrc = CreateFile(szSrcFilePath, GENERIC_READ, 0, NULL,
                      OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hSrc == INVALID_HANDLE_VALUE) {
    */
    fTempDisk = ( TbcStrstr(tbcPathSpec->szSrcPath, g_szTbcRAMDiskPath ) != NULL ) ? TRUE : FALSE;
    sPifHandle = TbcCreateFile(tbcPathSpec->szSrcName, GENERIC_READ, OPEN_EXISTING, fTempDisk);
    if (sPifHandle < 0) {
    /* --- END: change for doller tree scer 12/05/00 ---*/
        /* -------------------------------------- */
        /* if source file was not found, tell it to thread module */
        /* copy thread ignores it without error */
        if (GetLastError() == ERROR_FILE_NOT_FOUND) {
            *(tbcPathSpec->szSrcPath) = '\0';
            return 1;
        }
        /* -------------------------------------- */
        return 0;
    }

    /* get file length and alloc memory (must be < 4G) */
    /* --- BEGIN: change for doller tree scer 12/05/00 ---*/
    /* deleted...
    tbcPathSpec->dwBufLen = GetFileSize(hSrc, NULL);
    */
    tbcPathSpec->dwBufLen = TbcGetFileSize(sPifHandle);
    if (tbcPathSpec->dwBufLen == 0xffffffff) {
        /* --- BEGIN: change for doller tree scer 12/05/00 ---*/
        /* deleted...
        CloseHandle(hSrc);
        */
        TbcCloseFile(sPifHandle);
        /* --- END: change for doller tree scer 12/05/00 ---*/
        return 0;
    }
    tbcPathSpec->lpBuf = (UCHAR *)malloc(tbcPathSpec->dwBufLen + BLOCK_SIZE);
/*    tbcPathSpec->lpBuf = (UCHAR *)malloc(tbcPathSpec->dwBufLen); */
    if (tbcPathSpec->lpBuf == NULL) {
        /* --- BEGIN: change for doller tree scer 12/05/00 ---*/
        /* deleted...
        CloseHandle(hSrc);
        */
        TbcCloseFile(sPifHandle);
        /* --- END: change for doller tree scer 12/05/00 ---*/
        return 0;
    }

    /* read all */
    dwRemain = tbcPathSpec->dwBufLen;
    pTmp     = tbcPathSpec->lpBuf;
    while (dwRemain > 0) {
        /* --- BEGIN: change for doller tree scer 12/05/00 ---*/
        /* deleted...
        bRet = ReadFile(hSrc, pTmp, BLOCK_SIZE, &dwBytesRead, NULL);
        if (bRet == 0) {
        */
        /* --- END: change for doller tree scer 12/05/00 ---*/
        if (dwRemain > BLOCK_SIZE) {
			usRead = BLOCK_SIZE;
		} else {
			usRead = (USHORT)dwRemain;
		}
        if ( TbcReadFileEx(sPifHandle, pTmp, usRead, &dwBytesRead) < 0 )
        {
            /* --- BEGIN: change for doller tree scer 12/05/00 ---*/
            /* deleted...
            CloseHandle(hSrc);
            */
            TbcCloseFile(sPifHandle);
            /* --- END: change for doller tree scer 12/05/00 ---*/
            return 0;
        }
        pTmp     += dwBytesRead;
        dwRemain -= dwBytesRead;
#if DT_SCER
        /* --- To execute "Reg" mode operation, I will nap just a moment---*/
        Sleep( SLEEP_MSEC );
#endif
    }

    /* --- BEGIN: change for doller tree scer 12/05/00 ---*/
    /* deleted...
    CloseHandle(hSrc);
    */
    TbcCloseFile(sPifHandle);
    /* --- END: change for doller tree scer 12/05/00 ---*/

    return 1;
}

/*************************************************************************************************
 * FUNCTION : TbcCheckDestinationStorageSpace
 * INPUT    : tbcCopySpec : TBC_COPYSPEC object
 * RETURNS  : TBC_RSLT_ACC_CLEAR   : no-probrem for copy process (copy will be performed)
 *          : TBC_RSLT_ACC_LOWSTOR : free space of distination storage device is low.
 *          :                        (copy will be performed)
 *          : TBC_RSLT_DNY_NOSTOR  : not enough free space in destination storage device
 *          :                        (copy will not be performed)
 * DESC.    : check if free space is available or not in destination storage device
 *************************************************************************************************/
#define WARNING_MARGINE (128 * 1024)
static DWORD
TbcCheckDestinationStorageSpace(CONST TBC_COPYSPEC *tbcCopySpec)
{
    ULARGE_INTEGER  ulargeExist;        /* exist size (to be overwritten) */
    ULARGE_INTEGER  ulargePeak;         /* required size to copy */
    ULARGE_INTEGER  ulargeRemain;       /* remain */
    ULARGE_INTEGER  ulargeTemp1;        /* temporary value save area */
    ULARGE_INTEGER  ulargeTemp2;        /* temporary value save area */
    DWORD           dwRet;
    BOOL    fTempDisk;

    /* get size of exist file(s) */
    TbcGetFileSizeExists(tbcCopySpec, &ulargeExist);

    /* get peak size required for copy (emulating copy step) */
    ulargePeak = ulargeExist;
    TbcGetPeakSizeDuringCopy(tbcCopySpec, &ulargePeak);
    ulargePeak.QuadPart -= ulargeExist.QuadPart;

    /* get current free space and judge if free space is enough or not */
    /* --- BEGIN: change for doller tree scer 12/05/00 ---*/
    /* deleted...
    GetDiskFreeSpaceEx(tbcCopySpec->szDstPath,
                       &ulargeRemain, &ulargeTemp1, &ulargeTemp2);
    */
    fTempDisk = ( TbcStrstr(tbcCopySpec->szDstPath, g_szTbcRAMDiskPath ) != NULL ) ? TRUE : FALSE;
    TbcGetDiskFreeSpace(fTempDisk, &ulargeRemain, &ulargeTemp1, &ulargeTemp2);
    /* --- END: change for doller tree scer 12/05/00 ---*/

    if (ulargeRemain.QuadPart < ulargePeak.QuadPart) {
        dwRet = TBC_RSLT_DNY_NOSTOR;
    }
    else if ((ulargeRemain.QuadPart - WARNING_MARGINE) < ulargePeak.QuadPart) {
        dwRet = TBC_RSLT_ACC_LOWSTOR;
    }
    else {
        dwRet = TBC_RSLT_ACC_CLEAR;
    }

    return dwRet;
}

/*************************************************************************************************
 * FUNCTION : TbcGetFileSizeExists
 * INPUT    : tbcCopySpec : [in] TBC_COPYSPEC object
 *          : lpSize      : [out] sum of file size to be overritten
 * RETURNS  : NONE
 * DESC.    : get total file size to be overwritten
 *************************************************************************************************/
static VOID
TbcGetFileSizeExists(CONST TBC_COPYSPEC *tbcCopySpec,
                     ULARGE_INTEGER *lpSize)
{
    TBC_PATHSPEC   *tbcPathSpec;    /* copy path specification descriptor */
    DWORD           dwIdx;          /* loop counter */
    /* --- BEGIN: Add for Dollar Tree Scer 12/05/2000 --- */
    /* dropped...
    TCHAR           szDst[MAX_PATH];
    HANDLE          hFind;          / file find handle /
    WIN32_FIND_DATA findData;       / file data from FindFirstFile /
    ULARGE_INTEGER  ulargeTemp1;    / temporary value save area /
    */
    SHORT           sRet;
    DWORD           dwSizeLow;
    DWORD           dwSizeHigh;
    BOOL            fTempDisk;
    /* --- END:   Add for Dollar Tree Scer 12/05/2000 --- */

    /* get size of exist file(s) */
    lpSize->QuadPart = 0;
    tbcPathSpec = tbcCopySpec->tbcPathSpec;
    for (dwIdx = 0; dwIdx < tbcCopySpec->dwCount; dwIdx++, tbcPathSpec++) {

        /* if api module couldn't find source file, ignore it */
        if (*(tbcPathSpec->szSrcPath) == '\0') {
            continue;
        }

        /* --- BEGIN: Add for Dollar Tree Scer 12/05/2000 --- */
        /* dropped...
        / if distination file exists, get size and sumup /
        TbcStrcpy(szDst, tbcCopySpec->szDstPath);
        TbcStrcat(szDst, tbcPathSpec->szDstName);

        hFind = FindFirstFile(szDst, &findData);
        if (hFind != INVALID_HANDLE_VALUE) {
            ulargeTemp1.LowPart  = findData.nFileSizeLow;
            ulargeTemp1.HighPart = findData.nFileSizeHigh;
            lpSize->QuadPart    += ulargeTemp1.QuadPart;
            CloseHandle(hFind);
        }
        */
        fTempDisk = ( TbcStrstr(tbcCopySpec->szDstPath, g_szTbcRAMDiskPath ) != NULL ) ? TRUE : FALSE;
        sRet = TbcGetFileSizeEx(tbcPathSpec->szDstName, fTempDisk, &dwSizeHigh, &dwSizeLow);
        if (0 <= sRet) {
            /* --- increase total file size --- */
            lpSize->HighPart += dwSizeHigh;
            lpSize->LowPart  += dwSizeLow;
        }
        /* --- END: Add for Dollar Tree Scer 12/05/2000 --- */
    }

    return;
}

/*************************************************************************************************
 * FUNCTION : TbcGetPeakSizeDuringCopy
 * INPUT    : tbcCopySpec : [in] TBC_COPYSPEC object
 *          : lpSize      : [in] initial size / [out] peak size during copy
 * RETURNS  : NONE
 * DESC.    :
 *************************************************************************************************/
static VOID
TbcGetPeakSizeDuringCopy(CONST TBC_COPYSPEC *tbcCopySpec,
                      ULARGE_INTEGER *lpSize)
{
    TBC_PATHSPEC   *tbcPathSpec;    /* copy path specification descriptor */
    DWORD           dwIdx;          /* loop counter */
    ULARGE_INTEGER  ulargeTemp1;    /* temporary value save area */
    /* --- BEGIN: Add for Dollar Tree Scer 12/05/2000 --- */
    /* dropped...
    TCHAR           szDst[MAX_PATH];
    HANDLE          hFind;          / file find handle /
    WIN32_FIND_DATA findData;       / file data from FindFirstFile /
    ULARGE_INTEGER  ulargeTemp2;    / temporary value save area /
    */
    SHORT           sRet;
    BOOL            fTempDisk;
    DWORD           dwSizeLow;
    DWORD           dwSizeHigh;
    /* --- END:   Add for Dollar Tree Scer 12/05/2000 --- */

    /* get size of exist file(s) */
    ulargeTemp1 = *lpSize;
    tbcPathSpec = tbcCopySpec->tbcPathSpec;
    for (dwIdx = 0; dwIdx < tbcCopySpec->dwCount; dwIdx++, tbcPathSpec++) {

        /* if api module couldn't find source file, ignore it */
        if (*(tbcPathSpec->szSrcPath) == '\0') {
            continue;
        }

        /* add source file size (copy new one with renaming) */
        ulargeTemp1.QuadPart += tbcPathSpec->dwBufLen;

        /* update remain and peak value */
        if (lpSize->QuadPart < ulargeTemp1.QuadPart) {
            *lpSize = ulargeTemp1;
        }

        /* --- BEGIN: Add for Dollar Tree Scer 12/05/2000 --- */
        /* dropped...
        / if distination file exists, get size and subtract from work area /
        TbcStrcpy(szDst, tbcCopySpec->szDstPath);
        TbcStrcat(szDst, tbcPathSpec->szDstName);

        hFind = FindFirstFile(szDst, &findData);
        if (hFind != INVALID_HANDLE_VALUE) {
            ulargeTemp2.LowPart   = findData.nFileSizeLow;
            ulargeTemp2.HighPart  = findData.nFileSizeHigh;
            ulargeTemp1.QuadPart -= ulargeTemp2.QuadPart;
            CloseHandle(hFind);  / now, ulargeTemp1 is existing file size /
        }
        */
        fTempDisk = ( TbcStrstr(tbcCopySpec->szDstPath, g_szTbcRAMDiskPath ) != NULL ) ? TRUE : FALSE;
        sRet = TbcGetFileSizeEx(tbcPathSpec->szDstName, fTempDisk, &dwSizeHigh, &dwSizeLow);
        if (0 <= sRet) {
            /* --- decrease total file size --- */
            ulargeTemp1.HighPart -= dwSizeHigh;
            ulargeTemp1.LowPart  -= dwSizeLow;
        }
        /* --- END: Add for Dollar Tree Scer 12/05/2000 --- */
    }

    return;
}


/*************************************************************************************************
 * FUNCTION : TbcGetRAMDiskPath()
 * INPUT    : NONE
 * RETURNS  : NONE
 * DESC.    :
 *************************************************************************************************/
static VOID
TbcGetRAMDiskPath(VOID)
{
    HKEY    hRegKey;
    TCHAR   szAppRootKey[255];
    TCHAR   szValueTempPath[255];
    TCHAR   szValueDatabase[255];
    TCHAR   szPath[255];
    TCHAR   szDatabase[100];

    memset( g_szTbcRAMDiskPath, TEXT('\0'), sizeof(g_szTbcRAMDiskPath));

    /* --- prepare registry root of application --- */

    TbcStrcpy(szAppRootKey, PIFROOTKEY);
    TbcStrcat(szAppRootKey, TEXT("\\"));
    TbcStrcat(szAppRootKey, FILEKEY);

    TbcStrcpy(szValueTempPath, TEMPPATH );
    TbcStrcpy(szValueDatabase, DATABASE );

    /* --- open application root key --- */
    if ( RegOpenKeyEx(HKEY_LOCAL_MACHINE, szAppRootKey, 0, KEY_READ, &hRegKey) == ERROR_SUCCESS)
    {
        LONG  lSuccess1, lSuccess2;
		DWORD dwBytes1 = sizeof(szPath), dwBytes2 = sizeof(szDatabase);

        /* --- query registry data --- */

        lSuccess1= RegQueryValueEx(hRegKey, szValueTempPath, NULL, NULL, (LPBYTE)szPath,     &dwBytes1 );
        lSuccess2= RegQueryValueEx(hRegKey, szValueDatabase, NULL, NULL, (LPBYTE)szDatabase, &dwBytes2 );

        if ( ERROR_SUCCESS == lSuccess1 )
        {
            /* --- prepare output string data --- */

            TbcStrcpy(g_szTbcRAMDiskPath, szPath);
        }

        /* --- close application registry handle --- */

        RegCloseKey(hRegKey);
    }
}
/*************************************************************************************************
 * end of file
 *************************************************************************************************/
