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
* Title       : Assertion
* Category    : PLU Module, NCR 2172 Rel1.0 or later
* Program Name: pluasrt.c
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
* Nov-20-99 : 1.00.00  : K.Iwata    : initial (for 2172)
*           :          :            :
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

#if		(_WIN32_WCE || WIN32) && _MSC_VER >= 800 && _DEBUG

#include	<windows.h>
#include	<winuser.h>
#include	<string.h>
#include	<stdlib.h>
#include	"ecr.h"
#include	"plu.h"
#include	"pludefin.h"

static	int	nPluTraceCnt = 0;

#pragma message("### DEBUG:__wPluAssert ENABLED!!!")


void	_PLU_ASSERT(char * pchProcName,char * pchMsg,int nVal){
	TCHAR	achProcNm[128];
	TCHAR	achMsg[512];
	TCHAR	achBuf[1024];

	if(!nVal){
		/* edit message */
#if defined(_WIN32_WCE) || defined(XP_PORT)
		mbstowcs(achProcNm, pchProcName,strlen(pchProcName));
#else
		memcpy(achProcNm, pchProcName,strlen(pchProcName));
#endif
		if(pchMsg == NULL){
			wsprintf(achBuf,TEXT("[%s] "),achProcNm);
		}else{
#ifdef _WIN32_WCE
			mbstowcs(achMsg,pchMsg,strlen(pchMsg));
#else
			memcpy(achMsg,pchMsg,strlen(pchMsg));
#endif
			wsprintf(achBuf,TEXT("[%s] %s"),achProcNm,achMsg);
		}
		MessageBox(NULL,achBuf,TEXT("### ASSERT (PLU MODULE)"),  MB_OK);
		__debugbreak();   // replaced _asm {  int 3   }; to allow ARM compiles for Windows on Arm
	}
}

void	_PLU_TRACE(char * pchProcName,char * pchMsg,int nVal){
	TCHAR   achMsg[512];
	TCHAR   achBuf[256];
	TCHAR   achBuf2[256];

	memset(achMsg, 0,sizeof(achMsg));
	memset(achBuf, 0,sizeof(achBuf));
	memset(achBuf2,0,sizeof(achBuf2));

#if defined(_WIN32_WCE) || defined(XP_PORT)
	mbstowcs(achBuf, pchProcName,strlen(pchProcName));
	mbstowcs(achBuf2, pchMsg,strlen(pchMsg));
#else
	memcpy(achBuf, pchProcName,strlen(pchProcName));
	memcpy(achBuf2, pchMsg,strlen(pchMsg));
#endif
	wsprintf(achMsg,TEXT("### PLU TRACE[%d] : [%s] %s (%d)\n"),nPluTraceCnt,achBuf,achBuf2,nVal);

	OutputDebugString(achMsg);
	nPluTraceCnt++;
}

void	_PLU_BREAKPOINT(int nVal){
	if (nVal){
		__debugbreak();   // replaced _asm {  int 3   }; to allow ARM compiles for Windows on Arm
	}
}

#endif

