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
*   Title        : Totaldata (background/backup) copy module
*   Category     : Plutotal module, NCR2172 Saratoga application
*   Program Name : tbcthrd.c
*  ------------------------------------------------------------------------
*   Compiler     : MS-VC++ Ver.6.00 by Microsoft Corp.
*   Options      : 
*  ------------------------------------------------------------------------
*   Abstract     : header file for TBC TCHAR support routines
*  ------------------------------------------------------------------------
*    Update Histories   :
*
*   Date        Ver.Rev  :NAME         :Description
*   May-30-00 : 00.00.01 :m.teraki     :Initial
\*=======================================================================*/

/* includes ------------------------------------------------------------------------------------ */
#include <windows.h>
#include <stdio.h>

#include "ecr.h"
#include "pif.h"
#include "plulog.h"
#include "tbc.h"
#include "tbcin.h"
#include "tbctchar.h"

/* local typedefs ------------------------------------------------------------------------------ */

/* local macros -------------------------------------------------------------------------------- */

/* public symbols ------------------------------------------------------------------------------ */
extern TCHAR g_szTbcRAMDiskPath[MAX_PATH];

/* local symbols ------------------------------------------------------------------------------- */
static DWORD TbcWriteFile(CONST TCHAR *, BOOL, UCHAR *, DWORD);

/*************************************************************************************************
 * FUNCTION : TbcThread
 * INPUT    : NONE
 * OUTPUT   : NONE
 * RETURNS  : NONE
 * DESC.    : copy thread (main article of tbc.lib)
 *************************************************************************************************/
static VOID THREADENTRY TbcThread(VOID)
{
    /* --- BEGIN: Add for Dollar Tree Scer 12/05/2000 --- */
    /* dropped...
    TCHAR         szDstFilePath[MAX_PATH];
    TCHAR         szFinalFilePath[MAX_PATH];
    CHAR          szDstFileName[MAX_PATH];
    CHAR          szSrcFileName[MAX_PATH];
    */
    /* --- END: Add for Dollar Tree Scer 12/05/2000 --- */
#if defined(DEBUG) || defined(_DEBUG)
    ULONG   ulStartTime, ulEndTime;
    TCHAR   outbuf[128];
#endif

    while (1) {
        TBC_COPYSPEC *tbcCopySpec;

        /* block until copy request has issued */
        tbcCopySpec = TbcWaitCopyRequest();

#if defined(DEBUG) || defined(_DEBUG)
        PifGetTickCount( &ulStartTime );
        OutputDebugString( TEXT("Background copy START:\n") );
#endif

		if (!tbcCopySpec) {
			TbcCopyFail();
		} else {
            DWORD         dwRet = TBC_RSLT_ERROR;
            TBC_PATHSPEC *tbcPathSpec = tbcCopySpec->tbcPathSpec;

			for (DWORD dwIdx = 0; dwIdx < tbcCopySpec->dwCount; dwIdx++, tbcPathSpec++) {
                BOOL          fSrcTempDisk;
                BOOL          fDstTempDisk;

				/* if api module couldn't find source file, ignore it */
				if (*(tbcPathSpec->szSrcPath) == '\0') {
					continue;
				}

				/* --- BEGIN: Add for Dollar Tree SCER 12/05/2000 --- */
				/* dropped...
				/ constructs distination file path /
				TbcStrcpy(szDstFilePath, tbcCopySpec->szDstPath);
				TbcStrcat(szDstFilePath, tbcPathSpec->szTmpName);

				/ constructs final file path /
				TbcStrcpy(szFinalFilePath, tbcCopySpec->szDstPath);
				TbcStrcat(szFinalFilePath, tbcPathSpec->szDstName);

				/ copy with temporary name /
				dwRet = TbcWriteFile(szDstFilePath, tbcPathSpec->lpBuf, tbcPathSpec->dwBufLen);
				if (dwRet != ERROR_SUCCESS) {
					TbcAbort(MODULE_PLUTOTAL_BACKGROUND_COPY, dwRet);
					break;
				}

				DeleteFile(szFinalFilePath);

				/ rename /
				MoveFile(szDstFilePath, szFinalFilePath);
				*/

				fDstTempDisk = ( TbcStrstr(tbcCopySpec->szDstPath, g_szTbcRAMDiskPath ) != NULL ) ? TRUE : FALSE;

				dwRet = TbcWriteFile(tbcPathSpec->szTmpName, fDstTempDisk, tbcPathSpec->lpBuf, tbcPathSpec->dwBufLen);

				if (dwRet != ERROR_SUCCESS) {
					TbcAbort(MODULE_PLUTOTAL_BACKGROUND_COPY, dwRet);
					break;
				}

				fSrcTempDisk = ( TbcStrstr(tbcCopySpec->szDstPath, g_szTbcRAMDiskPath ) != NULL ) ? TRUE : FALSE;

				TbcDeleteFile(tbcPathSpec->szDstName, fSrcTempDisk);

				dwRet = TbcMoveFile(tbcPathSpec->szTmpName, fDstTempDisk, tbcPathSpec->szDstName, fSrcTempDisk);

				/* --- END: Add for Dollar Tree SCER 12/05/2000 --- */
			}

#if defined(DEBUG) || defined(_DEBUG)
			PifGetTickCount( &ulEndTime );
			swprintf( outbuf, TEXT("Background copy END  : %lu [msec]\n"), ulEndTime - ulStartTime );
			OutputDebugString(outbuf);
#endif
	            
			/* notify to api */
			if (dwRet == TBC_RSLT_ERROR) {
				TbcCopyFail();
			}
			else {
				TbcCopyComplete();
			}
		}
	}
}

/*************************************************************************************************
 * FUNCTION : TbcThreadInit
 * INPUT    : NONE
 * OUTPUT   : NONE
 * RETURNS  : NONE
 * DESC.    : begins thread via TbcPifIf module
 *************************************************************************************************/
VOID TbcThreadInit(VOID)
{
    /* Create thread */
//    TbcCreateThread(TbcThread, THREAD_PRIORITY_IDLE);
	PifBeginThread(TbcThread, NULL, 0, PIF_TASK_IDLE, "TTLCOPY", NULL);
}


/*************************************************************************************************
 * FUNCTION : TbcWriteFile
 * INPUT    : lpDstName : distination file name (without pathname)
 *          : fTempDisk : indicates temporay disk 
 *          : lpBuf     : buffer address to be written
 *          : dwLen     : buffer length
 * OUTPUT   : NONE
 * RETURNS  : ERROR_SUCCESS : success
 *          : other         : other win32 api error value
 * DESC.    : copy file.
 *          : Even if distination file is exists, copy is performed.
 *************************************************************************************************/
#define DT_SCER     1
#ifdef DT_SCER
#define BLOCK_SIZE  1024*16
//#define BLOCK_SIZE  4096
//#define BLOCK_SIZE  2048
#define SLEEP_MSEC  200
//#define SLEEP_MSEC  100
#else
#define BLOCK_SIZE  4096
#endif
static DWORD
TbcWriteFile(CONST TCHAR *lpDstName, BOOL fTempDisk, UCHAR *lpBuf, DWORD dwLen)
{
    DWORD  dwRet = ERROR_SUCCESS;  /* return value to caller */
    /* --- BEGIN: Add for Dollar Tree SCER 12/05/2000 --- */
    /* dropped...
    BOOL   bRet;
    HANDLE hDst;
    */
    SHORT  sPifHandle;
    SHORT  sRet;
    /* --- END: Add for Dollar Tree SCER 12/05/2000 --- */
    DWORD  dwBytesWritten;
    UCHAR *pWork;
    
    /* open distination file */
    /* --- BEGIN: Add for Dollar Tree SCER 12/05/2000 --- */
    /* dropped...
    hDst = CreateFile(lpDstName, GENERIC_WRITE, 0, NULL,
                      CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    if (hDst == INVALID_HANDLE_VALUE) {
        dwRet = GetLastError();
        CloseHandle(hDst);
        return dwRet;
    }
    */
    sPifHandle = TbcCreateFile(lpDstName, GENERIC_WRITE, CREATE_ALWAYS, fTempDisk);
    if (sPifHandle < 0) {
        dwRet = GetLastError();
        return dwRet;
    }
    /* --- END: Add for Dollar Tree SCER 12/05/2000 --- */

    /* do copy */
    for (pWork = lpBuf; dwLen > 0; pWork += BLOCK_SIZE) {
#if DT_SCER
        /* --- To execute "Reg" mode operation, I will nap just a moment---*/
        Sleep( SLEEP_MSEC );
#endif
        /* --- BEGIN: Add for Dollar Tree SCER 12/05/2000 --- */
        /* dropped...
        if (dwLen < BLOCK_SIZE) {
            bRet = WriteFile(hDst, pWork, dwLen, &dwBytesWritten, NULL);
        }
        else {
            bRet = WriteFile(hDst, pWork, BLOCK_SIZE, &dwBytesWritten, NULL);
        }
        if (bRet == 0) {
            dwRet = GetLastError();
            break;
        }
        */
        if (dwLen < BLOCK_SIZE) {
            sRet = TbcWriteFileEx(sPifHandle, pWork, (USHORT)dwLen, &dwBytesWritten);
        }
        else {
            sRet = TbcWriteFileEx(sPifHandle, pWork, BLOCK_SIZE, &dwBytesWritten);
        }

        if (sRet < 0) {
            dwRet = GetLastError();
            break;
        }
        /* --- END: Add for Dollar Tree SCER 12/05/2000 --- */

        dwLen -= dwBytesWritten;
    }

    /* --- BEGIN: Add for Dollar Tree SCER 12/05/2000 --- */
    /* dropped...
    CloseHandle(hDst);
    */
    TbcCloseFile(sPifHandle);
    /* --- END: Add for Dollar Tree SCER 12/05/2000 --- */

    return dwRet;
}
/*************************************************************************************************
 * end of file
 *************************************************************************************************/
