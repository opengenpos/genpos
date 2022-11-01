
#ifndef MYPIFDEFS_INCLUDED

#define MYPIFDEFS_INCLUDED


//#define PifSysConfig Pif32SysConfig
#define PifAbort(x,y) PifAbortEx((x),(y),1)


#ifdef junkyjunmk
SHORT	PifInit();
SHORT	PifUninit();
USHORT	PifCreateSem(USHORT usCount);
VOID	PifDeleteSem(USHORT usSem);
VOID	PifRequestSem(USHORT usSem);
VOID	PifReleaseSem(USHORT usSem);
SHORT	PifOpenFile(CONST WCHAR FAR * pszFileName, CONST CHAR FAR * pszMode);
USHORT	PifReadFile(USHORT usFile, VOID* pvBuffer, ULONG usBytes);
VOID	PifWriteFile(USHORT usFile, CONST VOID* pvBuffer, ULONG usBytes);
SHORT	PifSeekFile(USHORT usFile, ULONG ulPosition, ULONG* pulActualPosition);
VOID	PifCloseFile(USHORT usFile);
VOID	PifDeleteFile(CONST WCHAR FAR * lpszFileName);
SHORT	PifControlFile(USHORT usFile, USHORT usFunc, ...);
VOID	PifAbortEx(USHORT usFaultModule, USHORT usFaultCode, BOOL fAppl);
VOID	PifSleep(USHORT usMsec);
VOID    PifLog(USHORT usModuleId, USHORT usExceptionCode);
SYSCONFIG * PifSysConfig(VOID);
#endif

#endif

