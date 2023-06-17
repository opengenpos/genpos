/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2172     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1999            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Evs
* Category    : Event Sub System Dynamic Link Library, NCR 2172 for Win32
* Program Name: EvsInit.c
* --------------------------------------------------------------------------
* Compiler    : VC++ Ver. 6.00  by Microsoft Corp.                         
* Memory Model: 
* Options     : 
* --------------------------------------------------------------------------
* Abstruct:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
** NCR2172 **
* Oct-07-99 : 1.00.00  : K.Iwata    : initial (for 2172)
* May-17-15 : 2.02.01  : R.Chambers : change __wEvsASSERT() to __wEvsASSERT_func() for ASSRTLOG log
*===========================================================================
*===========================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===========================================================================
*/


#include <windows.h>
#include <winuser.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "ECR.h"
#include "pif.h"
#include "termcfg.h"

#include "Evs.h"
#include "EvsL.h"

static int nEvsDbgMsgCnt = 0;
static int nEvsDbgTraceCnt = 0;

/*
*===========================================================================
** Synopsis:    VOID __wEvsPostMessage(ULONG ulMsgNo,LONG lDataTblIdx,
*                                      ULONG wParam,ULONG lParam);
*
*     Input:    ulMsgNo    - message No.(windows)
*               lDataTblIdx- data src table INDEX
*               wPram/lParam
*
** Return:      nothing
*
*===========================================================================
*/
VOID __wEvsPostMessage(ULONG ulMsgNo,LONG lDataTblIdx,ULONG wParam,ULONG lParam){
    int     cnt;
    HWND    hWnd;
    BOOL    status;
	DWORD   dwLastError;

    for(cnt = 0;cnt < EVS_MAX_SRCUSERS;cnt++){
        hWnd = (HWND)DataSourceTable[lDataTblIdx].ulUsers[cnt];
        if(hWnd != (HWND)EVS_USER_DEFAULT){
            /* send message */
            status = PostMessage(hWnd,(UINT)ulMsgNo,(WPARAM)wParam,(LPARAM)lParam);

			dwLastError = GetLastError();
			// dwLastError == 1400 means Invalid window handle.  ERROR_INVALID_WINDOW_HANDLE 
			// dwLastError == 1816 means Window Message queue getting full, ERROR_NOT_ENOUGH_QUOTA

			_wEvsDebugMsgTrace(ulMsgNo,(ULONG)hWnd,(ULONG)wParam,(ULONG)lParam,NULL);
			
            __wEvsASSERT(status,"__wEvsPostMessage","PostMessage ERROR");
        }
    }
}

SHORT __wEvsSendMessage(ULONG ulMsgNo,LONG lDataTblIdx,ULONG wParam,ULONG lParam){ /* used to get button dimensions from Framework - CSMALL */
    int     cnt;
    HWND    hWnd;
    BOOL    status;
	DWORD  dwResult;

	for(cnt = 0;cnt < EVS_MAX_SRCUSERS;cnt++){
        hWnd = (HWND)DataSourceTable[lDataTblIdx].ulUsers[cnt];
        if(hWnd != (HWND)EVS_USER_DEFAULT){
            /* send message */
			if (hWnd) {
				// Put in Pif sleep and increased timeout number because the amount 
				// before was allowing this thread and the thread that is was passing 
				// messages to, to become unsynchronized. This allowed for this thread to 
				// send messages to the other thread before it should have. The puase give 
				// the other thread which is found in OnEvsPutParam and DisplayPopUp in the 
				// CFrameworkWndtext class in Framework. RLZ
				status = SendMessageTimeout(hWnd,(UINT)ulMsgNo,(WPARAM)wParam,(LPARAM)lParam, SMTO_BLOCK, 100, &dwResult);
				//PifSleep(15);
				_wEvsDebugMsgTrace(ulMsgNo,(ULONG)hWnd,(ULONG)wParam,(ULONG)lParam,NULL);
			}
		}
    }
	if (status != 0)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}



/*
*===========================================================================
** Synopsis:    VOID __wEvsASSERT(int nVal,char *pchProcName,char *pchMsg);
*
*     Input:    nVal       - 
*               - 
*               wPram/lParam
*
** Return:      nothing
*
*===========================================================================
*/
#if defined(_DEBUG)
#pragma message("### DEBUG:__wEvsASSERT ENABLED!!!")
#if 1
VOID __wEvsASSERT_func (int nSert, char *pchAssert, char *pchFile, int nLine)
{

    if(!nSert){
		int    iLen = strlen(pchFile);
		int    jLen = strlen(pchAssert);
		CHAR   achBuf[1024] = {0};

		if (iLen > 30) iLen -= 30; else iLen = 0;    // only end of the file name is needed not entire path.
		// truncate the assert text if it is too long
		if (jLen > 40) pchAssert[40] = 0;
        sprintf(achBuf, "**ASSERT: %s at %s %d", pchAssert, pchFile + iLen, nLine);
		NHPOS_ASSERT_TEXT(0, achBuf);
//        MessageBox(NULL,achBuf,TEXT("### ASSERT (EVENT SUB SYSTEM)"),  MB_OK);
//        _asm int 3
    }
    return;
}
#else
VOID __wEvsASSERT(int nVal,char *pchProcName,char *pchMsg){
    TCHAR   achProcNm[128];
    TCHAR   achMsg[512];
    TCHAR   achBuf[1024];

    if(!nVal){
        /* edit message */
		memset(achProcNm, 0x00, sizeof(achProcNm));
		memset(achMsg, 0x00, sizeof(achMsg));
		memset(achBuf, 0x00, sizeof(achBuf));

        mbstowcs(achProcNm, pchProcName,strlen(pchProcName));
        mbstowcs(achMsg,pchMsg,strlen(pchMsg));
        wsprintf(achBuf,TEXT("[%s] %s"),achProcNm,achMsg);
        MessageBox(NULL,achBuf,TEXT("### ASSERT (EVENT SUB SYSTEM)"),  MB_OK);
		__debugbreak();   // replaced _asm {  int 3   }; to allow ARM compiles for Windows on Arm
    }
    return;
    pchMsg = pchMsg;
}
#endif

VOID _wEvsDebugMsgTrace(ULONG ulMsg,ULONG ulUser,ULONG wParam,ULONG lParam,char * pMsg){
#if defined(EVS_DEBUGMSG)
	TCHAR   achMsg[512];
	TCHAR   achBuf[1024];
    
	memset(achMsg, 0x00, sizeof(achMsg));
	memset(achBuf, 0x00, sizeof(achBuf));

	if(pMsg != NULL)
		mbstowcs(achMsg, pMsg,strlen(pMsg));
	else
		wcscpy(achMsg,TEXT("---"));

	wsprintf(achBuf,TEXT("### Evs Msg[%d] : MSG=%ld  USR=%ld  wP=%ld  lP=%ld [%s]\n"),nEvsDbgMsgCnt,ulMsg,ulUser,wParam,lParam,achMsg);
    OutputDebugString(achBuf);
	nEvsDbgMsgCnt++;
#endif
}

VOID _wEvsDebugTrace(char * pMsg){
#if defined(EVS_DEBUGMSG)
	TCHAR   achMsg[512];
	TCHAR   achBuf[1024];

    memset(achMsg, 0x00, sizeof(achMsg));
	memset(achBuf, 0x00, sizeof(achBuf));
	mbstowcs(achMsg, pMsg,strlen(pMsg));
	wsprintf(achBuf,TEXT("### Evs TRACE[%d] : [%s]\n"),nEvsDbgTraceCnt,achMsg);
  OutputDebugString(achBuf);
	nEvsDbgTraceCnt++;
#endif
}

#endif

/* ========================================================================== */

