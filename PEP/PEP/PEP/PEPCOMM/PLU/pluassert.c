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
#include	"AW_Interfaces.h"

static	int	nPluTraceCnt = 0;

#ifndef _PLU_ASSERT
#pragma message("### DEBUG:__wPluAssert ENABLED!!!")
void	_PLU_ASSERT(char * pchProcName,char * pchMsg,int nVal){
	WCHAR	achProcNm[128];
	WCHAR	achMsg[512];
	WCHAR	achBuf[1024];

	if(!nVal){
		/* edit message */
		mbstowcs(achProcNm, pchProcName,strlen(pchProcName));
		if(pchMsg == NULL){
			wsPepf(achBuf,WIDE("[%s] "),achProcNm);
		}else{
			mbstowcs(achMsg,pchMsg,strlen(pchMsg));
			wsPepf(achBuf,WIDE("[%s] %s"),achProcNm,achMsg);
		}
		MessageBoxPopUp(NULL,achBuf,WIDE("### ASSERT (PLU MODULE)"),  MB_OK);
		__debugbreak();  // prep for ARM build _asm int 3;
	}
}
#endif

#ifndef _PLU_TRACE
#pragma message("### DEBUG:__wPluTrace ENABLED!!!")
void	_PLU_TRACE(char * pchProcName,char * pchMsg,int nVal){
	WCHAR   achMsg[512];
	WCHAR   achBuf[256];
	WCHAR   achBuf2[256];

	memset(achMsg, 0,sizeof(achMsg));
	memset(achBuf, 0,sizeof(achBuf));
	memset(achBuf2,0,sizeof(achBuf2));

	mbstowcs(achBuf, pchProcName,strlen(pchProcName));
	mbstowcs(achBuf2, pchMsg,strlen(pchMsg));
	wsPepf(achMsg,WIDE("### PLU TRACE[%d] : [%s] %s (%d)\n"),nPluTraceCnt,achBuf,achBuf2,nVal);

	OutputDebugStringW(achMsg);
	nPluTraceCnt++;
}
#endif

#ifndef _PLU_BREAKPOINT
#pragma message("### DEBUG:__wPluBreakPoint ENABLED!!!")
void	_PLU_BREAKPOINT(int nVal){
	if (nVal){
		__debugbreak();  // prep for ARM build _asm int 3;
	}
}
#endif

#endif

