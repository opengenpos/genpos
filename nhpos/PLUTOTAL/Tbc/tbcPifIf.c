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
*   Program Name : tbcPifIf.c
*  ------------------------------------------------------------------------
*   Compiler     : MS-VC++ Ver.6.00 by Microsoft Corp.
*   Options      : 
*  ------------------------------------------------------------------------
*   Abstract     :The provided function names are as follows.
*                  TbcAbort()
*                  TbcCreateThread()
*                
*  ------------------------------------------------------------------------
*    Update Histories   :
*
*   Date        Ver.Rev  :NAME         :Description
*   May-31-00 : 00.00.01 :m.teraki     :Initial
\*=======================================================================*/

/* includes ------------------------------------------------------------------------------------ */
#include <windows.h>
#include <string.h>

#include "ecr.h"
#include "log.h"
#include "plulog.h"
#include "pif.h"
#include "tbc.h"
#include "tbcin.h"

/* local typedefs ------------------------------------------------------------------------------ */

/* local macros -------------------------------------------------------------------------------- */

/* public symbols ------------------------------------------------------------------------------ */

/* local symbols ------------------------------------------------------------------------------- */


/*************************************************************************************************
 * FUNCTION : TbcAbort
 * INPUT    : usModuleId   : Module id which specifies object module
 *          :                (#defined in ../../include/piflog.h)
 *          : dwErrorValue : error code
 * OUTPUT   : NONE
 * RETURNS  : NONE
 * DESC.    : conver win32 error value into fault code defined in "log.h", then aborts immediately
 *************************************************************************************************/
VOID
TbcAbort(USHORT usModuleId, DWORD dwErrorValue)
{
    USHORT usFaultCode = FAULT_GENERAL;

    switch (dwErrorValue) {
    case ERROR_DISK_FULL:
    case ERROR_NOT_ENOUGH_MEMORY:
        usFaultCode = FAULT_SHORT_RESOURCE;
    }

    PifAbort(usModuleId, usFaultCode);
}

#if 0
/*************************************************************************************************
 * FUNCTION : TbcCreateThread
 * INPUT    : usFaultCode : fault code
 * OUTPUT   : NONE
 * RETURNS  : NONE
 * DESC.    : The reason of using PifBeginThread() is ,,,
 *          : PifAbort() shows module name passed at PifBeginThread()
 *************************************************************************************************/
VOID
TbcCreateThread(VOID (*lpfnThread)(VOID), int iPrio)
{
#if 0 /* use win32api only */
    HANDLE hThread;

    /* create */
    hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)lpfnThread,
                           NULL, CREATE_SUSPENDED , NULL);

    /* Set priority */
    SetThreadPriority(hThread, iPrio);

    /* Run it */
    ResumeThread(hThread);
#else
    USHORT usPrio = THREAD_PRIORITY_LOWEST;  // default case

    /* PifBeginThread converts priority constants */
    switch (iPrio) {
    case THREAD_PRIORITY_TIME_CRITICAL: 
        usPrio = PIF_TASK_TIME_CRITICAL;
        break;
    case PIF_TASK_HIGHEST:
        usPrio = PIF_TASK_HIGHEST;
        break;
    case THREAD_PRIORITY_ABOVE_NORMAL:
        usPrio = PIF_TASK_ABOVE_NORMAL;
        break;
    case THREAD_PRIORITY_NORMAL:
        usPrio = PIF_TASK_NORMAL;
        break;
    case THREAD_PRIORITY_LOWEST:
        /* nPrio = PIF_TASK_LOWEST; */
        usPrio = PIF_TASK_ABOVE_IDLE;
        break;
    case THREAD_PRIORITY_IDLE:
        usPrio = PIF_TASK_IDLE;
        break;
    }

    PifBeginThread(lpfnThread, NULL, 0, usPrio, "TTLCOPY", NULL);
#endif
}
#endif

/***************** BEGIN: ADD FOR DOLLAR TREE SCER 12/05/2000 **********************/
/*************************************************************************************************
 * FUNCTION : TbcLogElapsedTime
 * INPUT    : usSectionId   : Section ID
 *          :                (#defined in tbcin.h)
 *          : ulElapsedTime : Elapsed Time (in msec) to log
 * OUTPUT   : NONE
 * RETURNS  : NONE
 * DESC.    : 
 *************************************************************************************************/
VOID
TbcLogElapsedTime(USHORT usSectionId, ULONG ulElapsedTime)
{
#define SECOND_IN_MSEC  1000

    /* --- To pass PifLog, convert ULONG value to 4 digit (XXZZ: XX=second, ZZ=msec) --- */

    USHORT  usSecond;
    USHORT  usMsec;

    /* --- extract upper 2 digits of mili-seconds value --- */

    usMsec = ( USHORT )( ulElapsedTime % SECOND_IN_MSEC );
    usMsec = usMsec / 10;

    /* --- extract lower 2 digits of seconds value --- */

    usSecond = ( USHORT )( ulElapsedTime / SECOND_IN_MSEC);
    usSecond = usSecond % 100;

    /* --- make up elapsed time data (4 digits) to be logged ---*/

    usMsec = usSecond * 100 + usMsec;

    PifLog( usSectionId, usMsec );
}

/*************************************************************************************************
 * FUNCTION : TbcCreateFile
 * INPUT    : 
 * OUTPUT   : 
 * RETURNS  : 
 * DESC.    : 
 *************************************************************************************************/
SHORT
TbcCreateFile( CONST TCHAR* lpFileName, DWORD dwAccess, DWORD dwAttribute, BOOL fTemporary)
{
    //CHAR    szSrcFile[ MAX_PATH ];
    CHAR    szMode[ MAX_PATH ];
    SHORT   sPifHandle;

    /* --- determine desired access --- */

    if ( dwAccess == GENERIC_READ )         /* read only access */
    {
        strcpy( szMode, "r" );
    }
    else if ( dwAccess == GENERIC_WRITE )   /* write only access */
    {
        strcpy( szMode, "w" );
    }
    else                                    /* read & write access (default) */
    {
        strcpy( szMode, "rw" );
    }

    /* --- determine file attribute --- */

    if ( dwAttribute == CREATE_NEW )        /* success only if file does not exist */
    {
        strcat( szMode, "n" );
    }
    else if ( dwAttribute == OPEN_EXISTING) /* success only if file exists */
    {
        strcat( szMode, "o" );
    }
    else                                    /* open always (default) */
    {
        /* do nothing... */
    }

    /* --- determine temporary file or not --- */

    if ( fTemporary )                       /* open file in temporary disk (RAM) */
    {
        strcat( szMode, "t" );
    }
    else                                    /* open file in flash disk (default) */
    {
        /* do nothing... */
    }

    /* --- open alwayt deny share access --- */

    strcat( szMode, "-" );

    /* --- convert wide char (UNICODE) to muli-byte char (MBCS) --- */

	//memset(szSrcFile, 0, sizeof(szSrcFile));
    //wcstombs( szSrcFile, lpFileName, MAX_PATH );

    /* --- call Pifxxx function --- */

    sPifHandle = PifOpenFile( lpFileName, szMode );

    return sPifHandle;
}

/*************************************************************************************************
 * FUNCTION : TbcCloseFile
 * INPUT    : 
 * OUTPUT   : 
 * RETURNS  : 
 * DESC.    : 
 *************************************************************************************************/
VOID
TbcCloseFile( SHORT sPifHandle )
{
    PifCloseFile( sPifHandle );
}

/*************************************************************************************************
 * FUNCTION : TbcDeleteFile
 * INPUT    : 
 * OUTPUT   : 
 * RETURNS  : 
 * DESC.    : 
 *************************************************************************************************/
VOID
TbcDeleteFile( CONST TCHAR* lpFileName, BOOL fTempDisk )
{
    //CHAR    szSrcFile[ MAX_PATH ];

	//memset(szSrcFile, 0, sizeof(szSrcFile));
    //wcstombs( szSrcFile, lpFileName, MAX_PATH );

    PifDeleteFileEx( lpFileName, fTempDisk );
}

/*************************************************************************************************
 * FUNCTION : TbcMoveFile
 * INPUT    : 
 * OUTPUT   : 
 * RETURNS  : 
 * DESC.    : 
 *************************************************************************************************/
ULONG
TbcMoveFile( CONST TCHAR* lpExistFile, BOOL fSrcTempDisk, CONST TCHAR* lpNewFile, BOOL fDstTempDisk )
{
    //CHAR    szExistFile[ MAX_PATH ];
    //CHAR    szNewFile[ MAX_PATH ];
    ULONG   ulRet;

	//memset(szExistFile, 0, sizeof(szExistFile));
	//memset(szNewFile, 0, sizeof(szNewFile));
    //wcstombs( szExistFile, lpExistFile, MAX_PATH );
    //wcstombs( szNewFile,   lpNewFile,   MAX_PATH );

    ulRet = PifMoveFile( lpExistFile, fSrcTempDisk, lpNewFile, fDstTempDisk);

    return ulRet;
}

/*************************************************************************************************
 * FUNCTION : TbcGetFileSize()
 * INPUT    : 
 * OUTPUT   : 
 * RETURNS  : 
 * DESC.    : 
 *************************************************************************************************/
ULONG
TbcGetFileSize( SHORT sPifHandle )
{
    ULONG   ulFileSize;

    ulFileSize = PifGetFileSize( sPifHandle, NULL );

    return ulFileSize;
}

/*************************************************************************************************
 * FUNCTION : TbcReadFileEx()
 * INPUT    : 
 * OUTPUT   : 
 * RETURNS  : 
 * DESC.    : 
 *************************************************************************************************/
SHORT
TbcReadFileEx( SHORT sPifHandle, VOID* pBuffer, ULONG ulBytesToRead, ULONG* pulActualRead )
{
    SHORT  sRet = -1;
    ULONG  ulBytesRead;

    //RPH 11-11-3 SR# 201
	PifReadFile( sPifHandle, pBuffer, ulBytesToRead, &ulBytesRead);

    if ( 0 < ulBytesRead )
    {
        *pulActualRead = ulBytesRead;
        sRet = 0;
    }
    return sRet;
}

/*************************************************************************************************
 * FUNCTION : TbcWriteFileEx()
 * INPUT    : 
 * OUTPUT   : 
 * RETURNS  : 
 * DESC.    : 
 *************************************************************************************************/
SHORT
TbcWriteFileEx( SHORT sPifHandle, CONST VOID* pBuffer, USHORT usBytesToWrite, ULONG* pulActualWrite )
{
    SHORT   sRet = -1;
    USHORT  usBytesWritten;

    usBytesWritten = PifWriteFileEx( sPifHandle, pBuffer, usBytesToWrite );

    if ( 0 < usBytesWritten )
    {
        *pulActualWrite= usBytesWritten;
        sRet = 0;
    }
    return sRet;
}

/*************************************************************************************************
 * FUNCTION : TbcGetFileSizeEx()
 * INPUT    : 
 * OUTPUT   : 
 * RETURNS  : 
 * DESC.    : 
 *************************************************************************************************/
SHORT
TbcGetFileSizeEx( CONST TCHAR* lpFileName, BOOL fTempDisk, ULONG* pulFileSizeHigh, ULONG* pulFileSizeLow )
{
    SHORT   sRet;
    //CHAR    szExistFile[ MAX_PATH ];

	//memset(szExistFile, 0, sizeof(szExistFile));
    //wcstombs( szExistFile, lpFileName, MAX_PATH );

    sRet = PifGetFileSizeEx( lpFileName, fTempDisk, pulFileSizeHigh, pulFileSizeLow );

    return sRet;
}

/*************************************************************************************************
 * FUNCTION : TbcGetDiskFreeSpace()
 * INPUT    : 
 * OUTPUT   : 
 * RETURNS  : 
 * DESC.    : 
 *************************************************************************************************/
SHORT
TbcGetDiskFreeSpace( BOOL fTempDisk, PULARGE_INTEGER pulFreeBytesCaller,
                        PULARGE_INTEGER pulTotalBytesDisk, PULARGE_INTEGER pulFreeBytesDisk )
{
    SHORT   sRet;
    DWORD   dwFreeCallerHigh, dwFreeCallerLow;
    DWORD   dwTotalDiskHigh,  dwTotalDiskLow;
    DWORD   dwFreeDiskHigh,   dwFreeDiskLow;

    sRet = PifGetDiskFreeSpace( fTempDisk,
                                &dwFreeCallerHigh, &dwFreeCallerLow,
                                &dwTotalDiskHigh,  &dwTotalDiskLow,
                                &dwFreeDiskHigh,   &dwFreeDiskLow );

    if ( 0 < sRet )
    {
        if ( pulFreeBytesCaller != NULL )
        {
            pulFreeBytesCaller->HighPart = dwFreeCallerHigh;
            pulFreeBytesCaller->LowPart  = dwFreeCallerLow;
        }
        if ( pulTotalBytesDisk  != NULL )
        {
            pulTotalBytesDisk->HighPart  = dwTotalDiskHigh;
            pulTotalBytesDisk->LowPart   = dwTotalDiskLow;
        }
        if ( pulFreeBytesDisk   != NULL )
        {
            pulFreeBytesDisk->HighPart   = dwFreeDiskHigh;
            pulFreeBytesDisk->LowPart    = dwFreeDiskLow;
        }
    }
    return sRet;
}
/***************** END: ADD FOR DOLLAR TREE SCER 12/05/2000 **********************/

/*************************************************************************************************
 * end of file
 *************************************************************************************************/
