/////////////////////////////////////////////////////////////////////////////
// CKSUM.c : 
//
// Copyright (C) 1998 NCR Corporation, All rights reserved.
//
//
// History:
//
// Date         Version  Author       Comments
// ===========  =======  ===========  =======================================
// Jun-12-1998                        initial
//
/////////////////////////////////////////////////////////////////////////////

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <dgram.h>


/////////////////////////////////////////////////////////////////////////////
//	Local Definition
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	SHORT		cksum(pBuff, sWords);
//
// Inputs:		VOID*		pBuff;			-
//				SHORT		sWords;			-
//
// Outputs:		USHORT		usSum;			-
//
/////////////////////////////////////////////////////////////////////////////

SHORT cksum(VOID *pBuff, SHORT sWords)
{
	SHORT	sResult;

	__asm
	{
		push	ebx							; backup registers
		push	ecx							;

		xor		ecx, ecx					; clear ecx = 0

		mov		ebx, DWORD PTR pBuff		; get address of buffer
		mov     cx, sWords					; get count of words in cx
		xor     ax, ax						; clear ax for collecting sum

	next_word:
		add		ax, word ptr [ebx]			; add in next word
		adc		ax, 0						; add in carry bit
#if	1									// Bug !! - 2170 compatible code
		inc		ebx							; get address of the next word
#else									// No Bug - 
		add		ebx, 2						; get address of the next word
#endif
		loop	next_word					; descrement count and continue

		not		ax							; take 1s complement of result

		pop		ecx							; restore registers
		pop		ebx							;

		mov		sResult, ax					; set check sum result
	}

	// exit ...

	return sResult;
}


///////////////////////////////	 End of File  ///////////////////////////////
