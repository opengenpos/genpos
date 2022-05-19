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
*   Program Name : tbcin.h
*  ------------------------------------------------------------------------
*   Compiler     : MS-VC++ Ver.6.00 by Microsoft Corp.
*   Options      : 
*  ------------------------------------------------------------------------
*   Abstract     :  TBC (Totaldata Backup/Background Copy) internal header file
*  ------------------------------------------------------------------------
*    Update Histories   :
*
*   Date        Ver.Rev  :NAME         :Description
*   May-30-00 : 00.00.01 :m.teraki     :Initial
\*=======================================================================*/

/* used as parameter of TbcAbort */
#define TBC_FAULT_AT_WRITEFILE  (100)   /* error at TbcWriteFile() */
#define TBC_FAULT_AT_MALLOC     (101)   /* error at TbcInitCopySpec() */

/* path specification descriptor for single file */
typedef struct {
    TCHAR *szSrcPath;   /* source file path (and \) */
                        /* if file is not exists, points to '\0' */
    TCHAR *szSrcName;   /* source file name */
    TCHAR *szDstName;   /* destination file name */
    TCHAR *szTmpName;   /* temporary file name */
    DWORD  dwBufLen;    /* length of buffer */
    UCHAR *lpBuf;       /* buffer for File */
} TBC_PATHSPEC;

/* copy specification descriptor for multiple file */
typedef struct {
    DWORD         dwCount;     /* count of TBC_PATHSPEC objects */
    TBC_PATHSPEC *tbcPathSpec;
    TCHAR        *szDstPath;   /* destination file path (and \) */
} TBC_COPYSPEC;

extern VOID  TbcSyncInit(VOID);                  /* called from api */
extern DWORD TbcCopyRequest(TBC_COPYSPEC *);     /* called from api */
extern DWORD TbcGetThreadState(VOID);            /* called from api */
extern TBC_COPYSPEC *TbcWaitCopyRequest(VOID);   /* called from thread */
extern VOID  TbcCopyComplete(VOID);              /* called from thread */
extern VOID  TbcCopyFail(VOID);                  /* called from thread */
extern VOID  TbcThreadInit(VOID);                /* called from thread */
extern VOID  TbcDeInitCopySpec(TBC_COPYSPEC *);  /* called from api / thread (dangerous) */

extern VOID  TbcAbort(USHORT, DWORD);
extern VOID  TbcCreateThread(VOID (*)(VOID), int);
/* --- BEGIN: Add for Dollar Tree SCER 12/05/2000 ---*/
extern VOID  TbcLogElapsedTime(USHORT, ULONG);
extern SHORT TbcCreateFile( CONST TCHAR* lpFileName, DWORD dwAccess, DWORD dwAttribute, BOOL fTemporary );
extern VOID  TbcCloseFile( SHORT sPifHandle );
extern VOID  TbcDeleteFile( CONST TCHAR* lpFileName, BOOL fTempDisk );
extern ULONG TbcMoveFile( CONST TCHAR* lpExistFile, BOOL fSrcTempDisk,
                          CONST TCHAR* lpNewFile, BOOL fDstTempDisk );
extern ULONG TbcGetFileSize( SHORT sPifHandle );
extern SHORT TbcReadFileEx( SHORT sPifHandle, VOID* pBuffer,
                            ULONG ulBytesToRead, ULONG* pulActualRead );
extern SHORT TbcWriteFileEx( SHORT sPifHandle, CONST VOID* pBuffer,
                            USHORT usBytesToWrite, ULONG* pulActualWrite );
extern SHORT TbcGetFileSizeEx( CONST TCHAR* lpFileName, BOOL fTempDisk,
                               ULONG* pulFileSizeHigh, ULONG* pulFileSizeLow );
extern SHORT TbcGetDiskFreeSpace( BOOL fTempDisk, PULARGE_INTEGER pulFreeBytesCaller,
                        PULARGE_INTEGER pulTotalBytesDisk, PULARGE_INTEGER pulFreeBytesDisk );
/* --- END:   Add for Dollar Tree SCER 12/05/2000 ---*/

/*************************************************************************************************
 * end of file
 *************************************************************************************************/
