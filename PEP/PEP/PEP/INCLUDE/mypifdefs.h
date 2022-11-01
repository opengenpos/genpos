
#ifndef MYPIFDEFS_INCLUDED

#define MYPIFDEFS_INCLUDED
/*
#define PifReleaseSem Pif32ReleaseSem
#define PifRequestSem Pif32RequestSem
#define PifCreateSem Pif32CreateSem
#define PifDeleteSem Pif32DeleteSem


#define PifOpenFile Pif32OpenFile
#define PifCloseFile Pif32CloseFile
#define PifSeekFile Pif32SeekFile
#define PifDeleteFile Pif32DeleteFile
#define PifReadFile Pif32ReadFile
#define PifWriteFile Pif32WriteFile


#define PifControlFile Pif32ControlFile

#define PifLog Pif32Log
#define PifSysConfig Pif32SysConfig
#define PifSleep Pif32Sleep

*/
#define PifAbort(x,y) PifAbortEx((x),(y),1)

//#ifdef junkyjunk
/*
SHORT	PifInit();
SHORT	PifUninit();
USHORT	PifCreateSem(USHORT usCount);
VOID	PifDeleteSem(USHORT usSem);
VOID	PifRequestSem(USHORT usSem);
VOID	PifReleaseSem(USHORT usSem);
SHORT	PifOpenFile(CONST WCHAR FAR * pszFileName, CONST UCHAR FAR * pszMode);
USHORT	PifReadFile(USHORT usFile, VOID* pvBuffer, USHORT usBytes);
VOID	PifWriteFile(USHORT usFile, CONST VOID* pvBuffer, USHORT usBytes);
SHORT	PifSeekFile(USHORT usFile, ULONG ulPosition, ULONG* pulActualPosition);
VOID	PifCloseFile(USHORT usFile);
VOID	PifDeleteFile(CONST WCHAR FAR * lpszFileName);
SHORT	PifControlFile(USHORT usFile, USHORT usFunc, ...);
VOID	PifAbortEx(USHORT usFaultModule, USHORT usFaultCode, BOOL fAppl);
VOID	PifSleep(USHORT usMsec);
VOID    PifLog(USHORT usModuleId, USHORT usExceptionCode);
//#endif*/
#endif
