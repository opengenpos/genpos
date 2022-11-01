/////////////////////////////////////////////////////////////////////////////
// PIF32.H :
//
// Copyright (C) 1998 NCR Corporation, All rights reserved.
//
//
// History:
//
// Date         Version  Author       Comments
// ===========  =======  ===========  =======================================
// Aug-32-1998                        initial
//
/////////////////////////////////////////////////////////////////////////////

#ifndef	PIF32_H
#define	PIF32_H

#ifdef __cplusplus
extern "C"{
#endif

#pragma	pack(push, 8)


/////////////////////////////////////////////////////////////////////////////
//  Result Code
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//  General
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//  Function Prototypes
/////////////////////////////////////////////////////////////////////////////
#ifdef junkyjunk
SHORT	PIFENTRY PifInit();
SHORT	PIFENTRY PifUninit();
USHORT	PIFENTRY PifCreateSem(USHORT usCount);
VOID	PIFENTRY PifDeleteSem(USHORT usSem);
VOID	PIFENTRY PifRequestSem(USHORT usSem);
VOID	PIFENTRY PifReleaseSem(USHORT usSem);
SHORT	PIFENTRY PifOpenFile(LPCWSTR pszFileName, LPCWSTR pszMode);
BOOL    PIFENTRY PifFileMode(LPCTSTR pszMode, DWORD* pdwAccess, DWORD* pdwCreate, DWORD *pdwDir);
USHORT	PIFENTRY PifReadFile(USHORT usFile, VOID* pvBuffer, ULONG usBytes);
VOID	PIFENTRY PifWriteFile(USHORT usFile, CONST VOID* pvBuffer, ULONG usBytes);
SHORT	PIFENTRY PifSeekFile(USHORT usFile, ULONG ulPosition, ULONG* pulActualPosition);
VOID	PIFENTRY PifCloseFile(USHORT usFile);
VOID	PIFENTRY PifDeleteFile(LPCWSTR lpszFileName);
SHORT	PIFENTRY PifControlFile(USHORT usFile, USHORT usFunc, ...);
VOID	PIFENTRY PifAbortEx(USHORT usFaultModule, USHORT usFaultCode, BOOL fAppl);
VOID    PIFENTRY PifLog(USHORT usModuleId, USHORT usExceptionCode);
//VOID Pif32SysConfig(VOID);  // RJC modified
//VOID * Pif32SysConfig(VOID);

#endif
/////////////////////////////////////////////////////////////////////////////


#pragma	pack(pop)

#ifdef __cplusplus
}
#endif

#endif                                      // ifndef(PIF32_H)
