/////////////////////////////////////////////////////////////////////////////
// CRC.c : 
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
#include <io.h>


/////////////////////////////////////////////////////////////////////////////
//	Local Definition
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// Function:	CRC-CCITT = X16 + X12 + X5 + 1
//				MSB side = HIGH-BIT, BYTE BLOCK
//
// Prototype:	USHORT		XinuCrc(pvBuffer, usLength, usInitCrc);
//
// Inputs:		VOID FAR *	pvBuffer;		-
//				USHORT		usLength;		-
//				USHORT		usInitCrc;		-
//
// Outputs:		USHORT		usResult;		-
//
/////////////////////////////////////////////////////////////////////////////

USHORT XinuCrc(VOID FAR *pvBuffer, USHORT usLength, USHORT usInitCrc)
{
	USHORT	usResult;

	__asm
	{
		push	ebx							; backup registers
		push	ecx
		push	edx
		push	esi
		push	edi

		xor		ecx, ecx					; clear ecx = 0

		mov		edi, DWORD PTR pvBuffer		; get address of buffer
		mov		cx, usLength				; length of buffer
		mov		bx, usInitCrc				; initial CRC data

	ccitt_byte_1:
		xor		bh, byte ptr [edi]			; bh = data + crc_high
		inc		edi							; pointer next
		xchg	bh, bl						; setup initial data
		mov		al, bl						;
		shr		al, 1						; al >> 4
		shr		al, 1						;
		shr		al, 1						;
		shr		al, 1						;
		xor		bl, al						; bl = bl + ah
		mov		ah, bl						;
		xor		al, al						; ax = bl * ff
		shr		ax, 1						; ax	 >> 3
		shr		ax, 1						;
		shr		ax, 1						;
		xor		bx, ax						; bx = bx + ax
		shr		ax, 1						; ax >> 1
		xor		bh, al						; bx = new_crc
		loop	ccitt_byte_1				;

		mov		usResult, bx				; set CRC result
	
		pop		edi							; restore registers
		pop		esi
		pop		edx
		pop		ecx
		pop		ebx
	}

	// exit ...

	return usResult;
}


///////////////////////////////	 End of File  ///////////////////////////////
