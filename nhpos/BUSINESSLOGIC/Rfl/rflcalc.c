/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2171     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1999            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Calculation for HUGEINT(8 byte data) and EHUGEINT(12 byte data)
* Category    : Reentrant Functions Library, NCR 2171 Application
* Program Name: rflcalc.c
* --------------------------------------------------------------------------
* Compiler    : cl for VC++ Ver 6.0 by Microsoft Corp.                         
* --------------------------------------------------------------------------
* Abstruct:
*       RflAdd64to64()
*       RflMul32by32()
*       RflDiv64by32()
*       RflCast64to32()
*       RflCast32to64() 
*       RflMul64by32()
*       RflDiv96by32()
*       RflAdd96to96()
*       RflCast32to96()
*       RflCast96to64()
*       RflCast64to16()
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Sep-02-99 : 01.00.00 : M.Teraki   : initial (for Win32)
*                                   : migrated from rflcalc.asm
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

/*
============================================================================
              D A T A   A R E A S   A N D   E Q U A T E S		
============================================================================
*/

/*
=============================================================================
+                        I N C L U D E   F I L E s
=============================================================================
*/
#include <tchar.h>
#include "ecr.h"
#include "rfl.h"

/**
============================================================================
+              P R O G R A M    D E C L A R A T I O N s
============================================================================
**/

#if defined(USE_2170_ORIGINAL)
/*
*===========================================================================
** Format  : VOID    RflAdd64to64(HUGEINT *pAdd1, HUGEINT *pAdd2);      
*
*   Input  : HUGEINT     *pAdd1
*            HUGEINT     *pAdd2
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function adds *pAdd1 to *pAdd2, and storage added value to *pAdd1
*===========================================================================
*/
VOID RflAdd64to64(HUGEINT *pAdd1, HUGEINT *pAdd2)
{
    __asm {
	push	esi			;
	push	edi			;
	push	eax			;

	mov	edi, DWORD PTR [pAdd1]	;
	mov	esi, DWORD PTR [pAdd2]	;

	mov	eax, DWORD PTR [edi]	;
	add	eax, DWORD PTR [esi]	;
	mov	DWORD PTR [edi], eax	;

	mov	eax, DWORD PTR 4[edi]	;
	adc	eax, DWORD PTR 4[esi]	;
	mov	DWORD PTR 4[edi], eax	;

	pop	eax			;
	pop	edi			;
	pop	esi			;
    }
}

/*
*===========================================================================
** Format  : VOID    RflMul32by32(HUGEINT *pProduct, LONG lMultiplicant,
*                                                    LONG lMulriplier);
*
*   Input  : LONG lMultiplicant
*            LONG lMultiplier
*   Output : HUGEINT  *pProduct        -product storage area address
*   InOut  : none
** Return  : none
*
** Synopsis: This function multiply long by long and storage  *pProduct
*===========================================================================
*/
VOID RflMul32by32(HUGEINT *pProduct, LONG lMultiplicant, LONG lMulriplier)
{
    __asm {
	push	eax				;
	push	ebx				;
	push	ecx				;
	push	edx				;
	push	edi				;

	mov	edi, dword ptr [pProduct]	;

	mov	ecx, dword ptr [lMultiplicant]	;
	or	ecx, ecx			; minus management
	jns	__m32by32_plus1			;
	neg	dword ptr [lMultiplicant]	;

      __m32by32_plus1:
	xor	ecx, dword ptr [lMulriplier]	; store sign
	mov	eax, dword ptr [lMulriplier]	;
	or	eax, eax			; minus management
	jns	__m32by32_plus2			;
	neg	dword ptr [lMulriplier]		;

      __m32by32_plus2:
	mov	eax, dword ptr [lMultiplicant]	;
	mov	ebx, dword ptr [lMulriplier]	;
	mul	ebx				;
	mov	dword ptr 0[edi], eax		; store lower-32bit
	jnc	__m32by32_res32			;
	mov	dword ptr 4[edi], edx		; store upper-32bit if CF=1
	jmp	__m32by32_adj_sign		;

      __m32by32_res32:
	mov	dword ptr 4[edi], 0		; clear upper-32bit if CF=0

      __m32by32_adj_sign:
	or	ecx, ecx			;
	jns	__m32by32_end			;
	not	dword ptr 4[edi]		; minus management
	neg	dword ptr 0[edi]		;
	sbb	dword ptr 4[edi], -1		;

      __m32by32_end:
	pop	edi				;
	pop	edx				;
	pop	ecx				;
	pop	ebx				;
	pop	eax				;
    }
}

;/*
;*===========================================================================
;** Format  : VOID    RflDiv64by32(HUGEINT *pQuot, LONG *plRemain,
;*                                                 LONG lDivisor);      
;*
;*   Input  : LONG     lDivisor       -divisor
;*   Output : LONG     *plRemain      -Remain
;*   InOut  : HUGEINT  *pQuot         -divident and quot
;** Return  : none
;*
;** Synopsis: This function divides 64bit data that stored in pQuot, 
;*            and writes Quot to pQuot, Remain to plRemain.
;*===========================================================================
;*/
VOID   RflDiv64by32(HUGEINT *pQuot, LONG *plRemain, LONG lDivisor)
{
    __asm {
	push	eax				;
	push	ecx				;
	push	edx				;
	push	esi				;
	push	edi				;

	mov	eax, dword ptr [lDivisor]	; check whether (lDivisor == 0) or not
	or	eax, eax			;
	jnz	__d64by32_10			;
	jmp	__d64by32_end			; do nothing if (lDivisor == 0)

      __d64by32_10:
	mov	esi, dword ptr [pQuot]		; get devident address
	mov	ecx, dword ptr 4[esi]		; store divident's sign
	or	ecx, ecx			; check minus (divident)
	jns	__d64by32_20			;
	not	dword ptr 4[esi]		; negate (*pQuot) (64bit)
	neg	dword ptr 0[esi]		;
	sbb	dword ptr 4[esi], -1		;

      __d64by32_20:
	mov	edx, dword ptr [lDivisor]	; get divisor's value
	or	edx, edx			; check minus (divisor)
	jns	__d64by32_30			;
	neg	edx				; negate divisor

      __d64by32_30:
	push	ecx				; save sign info. of divident
	mov	ecx, 64				; set loop counter
	xor	eax, eax			; initialize remainder area

      ;-----------------------------------------
      ; registers store data as follows
      ;   eax <= remainder
      ;   edx <= |divisor|
      ;-----------------------------------------

      __d64by32_subtloop:
	shl	dword ptr 0[esi], 1		;
	rcl	dword ptr 4[esi], 1		;
	rcl	eax, 1				;
	cmp	eax, edx			; compare remainder area and divisor area
	jb	__d64by32_notsubt		;
	sub	eax, edx			; subtract divisor value from remainder area 
	inc	dword ptr [esi]			; increment quot area

      __d64by32_notsubt:
	loop	__d64by32_subtloop		;

	pop	ecx				; restore divident's sign
	or	ecx, ecx			; check minus
	jns	__d64by32_40			;
	neg	eax		; remainder's sign is always same as devident's sign

      __d64by32_40:
	mov	edi, dword ptr [plRemain]	; write remainder to memory
	mov	dword ptr [edi], eax		;

	mov	edx, dword ptr [lDivisor]	; get divisor's value
	xor	ecx, edx			; check divisor's sign and divident's
	or	ecx, ecx			;
	jns	__d64by32_end			;
	not	dword ptr 4[esi]		; minus management for quot
	neg	dword ptr 0[esi]		;
	sbb	dword ptr 4[esi], -1		;

      __d64by32_end:				;
	pop	edi				;
	pop	esi				;
	pop	edx				;
	pop	ecx				;
	pop	eax				;
    }
}

/*
*===========================================================================
** Format  : VOID    RflCast64to32(LONG *lDest, HUGEINT *pSource);      
*
*   Input  : HUGEINT  *pSource       -source data
*   Output : LONG     lDest          -destination data
*   InOut  : none
** Return  : none
*
** Synopsis: This function converts 64bit data to 32bit data
*===========================================================================
*/
VOID RflCast64to32(LONG *lDest, HUGEINT *pSource)
{
    /* *lDest = *((LONG *)pSource); */
    __asm {
	push	esi				;
	push	edi				;
	push	eax				;

	mov	esi, dword ptr [pSource]	;
	mov	edi, dword ptr [lDest]		;
	mov	eax, dword ptr [esi]		;
	mov	dword ptr [edi], eax		;

	pop	eax				;
	pop	edi				;
	pop	esi				;
    }
}

/*
*===========================================================================
** Format  : VOID    RflCast32to64(HUGEINT *pDest, LONG lSource);      
*
*   Input  : LONG      lSource         -Source data
*   Output : HUGEINT   *pDest          -Destination data
*   InOut  : none
** Return  : none
*
** Synopsis: This function converts 32bit data to 64bit data
*===========================================================================
*/
VOID   RflCast32to64(HUGEINT *pDest, LONG lSource)
{
    __asm {
	push	eax			;
	push	edi			;
	mov	edi, dword ptr [pDest]	;
	mov	eax, dword ptr [lSource];

	or	eax, eax		;
	jns	__c32to64_plus		;

	mov	dword ptr 4[edi], -1	; write high 4byte (minus)
	jmp	__c32to64_common	;

      __c32to64_plus:
	mov	dword ptr 4[edi], 0	; write high 4byte (plus)
      __c32to64_common:			;
	mov	dword ptr [edi], eax	;

	pop	edi			;
	pop	eax			;
    }
}
#endif

#if defined(POSSIBLE_DEAD_CODE)

/*
*===========================================================================
** Format  : VOID    RflMul64by32(EHUGEINT *pProduct, HUGEINT *pMultiplicant,
*                                                    LONG lMulriplier);
*
*   Input  : HUGEINT *plMultiplicant
*            LONG lMultiplier
*   Output : EHUGEINT  *pProduct        -product storage area address
*   InOut  : none
** Return  : none
*
** Synopsis: This function multiply 64bits by long and storage  *pProduct V3.3
*===========================================================================
*/
VOID RflMul64by32(EHUGEINT *pProduct, HUGEINT *pMultiplicant, LONG lMultiplier)
{
    __asm {
	push	esi				;
	push	edi				;
	push	eax				;
	push	ebx				;
	push	ecx				;
	push	edx				;

	mov	esi, dword ptr [pProduct]	;
	mov	dword ptr 0[esi], 0		; Initialize *pProduct
	mov	dword ptr 4[esi], 0		;
	mov	dword ptr 8[esi], 0		;

	mov	edi, dword ptr [pMultiplicant]	; 
	mov	ecx, dword ptr 4[edi]		; store multiplient's sign info.
	or	ecx, ecx			; minus management
	jns	__m64by32_10			;
        not	dword ptr 4[edi]		;
        neg	dword ptr 0[edi]		;
        sbb	dword ptr 4[edi], -1		;

      __m64by32_10:
        xor	ecx, dword ptr [lMultiplier]	;
        mov	eax, dword ptr [lMultiplier]	;
        or	eax, eax			; minus management
        jns     __m64by32_20			;
        neg	dword ptr [lMultiplier]		;

      __m64by32_20:
        mov	eax, dword ptr 0[edi]		;
        mov	ebx, dword ptr [lMultiplier]    ;
        mul	ebx				;
        mov	dword ptr 0[esi], eax		;
        mov	dword ptr 4[esi], edx		;

        mov	eax, dword ptr 4[edi]		;
        mov	ebx, dword ptr [lMultiplier]	;
        mul	ebx				;
        add	dword ptr 4[esi], eax		;
        adc	dword ptr 8[esi], edx		;

        or	ecx, ecx			;
        jns	__m64by32_30			;
        not	dword ptr 4[esi]		; minus management
        not	dword ptr 8[esi]		;
        neg	dword ptr 0[esi]		;
        sbb	dword ptr 4[esi], -1            ;
        sbb	dword ptr 8[esi], -1            ;

      __m64by32_30:
	pop	edx				;
	pop	ecx				;
	pop	ebx				;
	pop	eax				;
	pop	edi				;
	pop	esi				;
    }
}

/*
*===========================================================================
** Format  : VOID    RflDiv96by32(EHUGEINT *pQuot, LONG *plRemain,
*                                                 LONG lDivisor);
*
*   Input  : LONG     lDivisor       -divisor
*   Output : LONG     *plRemain      -Remain
*   InOut  : EHUGEINT  *pQuot         -divident and quot
** Return  : none
*
** Synopsis: This function divides 96bit data that stored in pQuot,
*            and writes Quot to pQuot, Remain to plRemain. V3.3
*===========================================================================
*/
VOID RflDiv96by32(EHUGEINT *pQuot, LONG *plRemain, LONG lDivisor)
{
    __asm {
	push	esi				;
	push	edi				;
	push	eax				;
	push	ebx				;
	push	ecx				;
	push	edx				;

	mov	eax, dword ptr [lDivisor]	; check if (lDivisor == 0) or not
	or	eax, eax			;
	jnz	__d96by32_10			;
	jmp	__d96by32_end			; do nothing if (lDivisor == 0)

      __d96by32_10:
	mov	esi, dword ptr [pQuot]		; get devident address
        mov	ecx, dword ptr 8[esi]		; store divident's sign
        or	ecx, ecx			; check minus (divident)
	jns	__d96by32_20			;
	not	dword ptr 8[esi]		; negate (*pQuot) (96bit)
	not	dword ptr 4[esi]		;
	neg	dword ptr 0[esi]		;
	sbb	dword ptr 4[esi], -1		;
	sbb	dword ptr 8[esi], -1		;

      __d96by32_20:
	mov	edx, dword ptr [lDivisor]	; get divisor's value
	or	edx, edx			; check minus (divisor)
	jns	__d96by32_30			;
	neg	edx				; minus management

      __d96by32_30:
	push	ecx				; save sign info. for divident
	mov	ecx, 96				; set loop counter
	xor	eax, eax			; initialize remainder area

      ;-----------------------------------------
      ; registers store data as follows
      ;   eax <= remainder
      ;   edx <= |divisor|
      ;-----------------------------------------

      __d96by32_subtloop:
	shl	dword ptr 0[esi], 1		;
	rcl	dword ptr 4[esi], 1		;
	rcl	dword ptr 8[esi], 1		;
	rcl	eax, 1				;
	cmp	eax, edx			; compare remainder area and divisor area
	jb	__d96by32_notsubt		;
	sub	eax, edx			; subtract divisor value from remainder area
	inc	dword ptr [esi]			; increment quot area

      __d96by32_notsubt:
	loop	__d96by32_subtloop		;

	pop	ecx				; restore divident's sign
        or	ecx, ecx			; check minus
        jns	__d96by32_40			;
        neg	eax			; remainder's sign is always same as divident's sign.

      __d96by32_40:
        mov	edi, dword ptr [plRemain]	; write remainder to memory
        mov	dword ptr [edi], eax		;

        mov	edx, dword ptr [lDivisor]	; get divisor's value
        xor	ecx, edx			; check divisor's sign and divident's
        or	ecx, ecx			;
        jns	__d96by32_end			;
        not	dword ptr 8[esi]		; minus management for quot
        not	dword ptr 4[esi]		;
        neg	dword ptr 0[esi]		;
        sbb	dword ptr 4[esi], -1		;
        sbb	dword ptr 8[esi], -1		;

      __d96by32_end:
	pop	edx				;
	pop	ecx				;
	pop	ebx				;
	pop	eax				;
	pop	edi				;
	pop	esi				;
    }
}

#if defined(POSSIBLE_DEAD_CODE)
	// Removing unused functions from the GenPOS compile.
	// The following functions were used in functionality that
	// was previously removed. Seems to be part of the
	// rounding calculations.
	//      Richard Chambers, Oct-03-2018

/*
*===========================================================================
** Format  : VOID    RflAdd96to96(EHUGEINT *pAdd1, EHUGEINT *pAdd2);
*
*   Input  : EHUGEINT     *pAdd2
*
*   Output : none
*   InOut  : EHUGEINT     *pAdd1
** Return  : none
*
** Synopsis: This function adds *pAdd1 to *pAdd2, and storage added value to *pAdd1 V3.3
*===========================================================================
*/
VOID RflAdd96to96(EHUGEINT *pAdd1, EHUGEINT *pAdd2)
{
    __asm {
	push	eax			;
	push	esi			;
	push	edi			;

	mov	edi, dword ptr [pAdd1]	;
	mov	esi, dword ptr [pAdd2]	;

	mov	eax, dword ptr 0[esi]	;
	add	dword ptr 0[edi], eax	;

	mov	eax, dword ptr 4[esi]	;
	adc	dword ptr 4[edi], eax	;

	mov	eax, dword ptr 8[esi]	;
	adc	dword ptr 8[edi], eax	;

	pop	edi			;
	pop	esi			;
	pop	eax			;
    }
}

/*
*===========================================================================
** Format  : VOID    RflCast32to96(EHUGEINT *pDest, LONG lSource);
*
*   Input  : LONG      lSource         -Source data
*   Output : EHUGEINT  *pDest          -Destination data
*   InOut  : none
** Return  : none
*
** Synopsis: This function converts 32bit data to 96bit data V3.3
*===========================================================================
*/
VOID RflCast32to96(EHUGEINT *pDest, LONG lSource)
{
    __asm {
	push	eax			;
	push	edi			;
	mov	edi, dword ptr [pDest]	;
	mov	eax, dword ptr [lSource];

	or	eax, eax		;
	jns	__c32to96_plus		;

	mov	dword ptr 4[edi], -1	; write high 8 byte (minus)
	mov	dword ptr 8[edi], -1	;
	jmp	__c32to96_common	;

      __c32to96_plus:
	mov	dword ptr 4[edi], 0	; write high 8 byte (plus)
	mov	dword ptr 8[edi], 0	;

      __c32to96_common:
	mov	dword ptr [edi], eax	;

	pop	edi			;
	pop	eax			;
    }
}

#endif

/*
*===========================================================================
** Format  : VOID    RflCast96to64(HUGEINT *pDest, EHUGEINT *pSource);
*
*   Input  : EHUGEINT  *pSource       -source data
*   Output : HUGEINT   *pDest         -destination data
*   InOut  : none
** Return  : none
*
** Synopsis: This function converts 96bit data to 64bit data V3.3
*===========================================================================
*/
VOID RflCast96to64(HUGEINT *pDest, EHUGEINT *pSource)
{
    __asm {
	push	esi			;
	push	edi			;
	push	eax			;

	mov	edi, dword ptr [pDest]	;
	mov	esi, dword ptr [pSource];

	mov	eax, dword ptr [esi]	; write higher 32bit
	mov	dword ptr [edi], eax	;

	mov	eax, DWORD PTR 4[esi]	; write lower 32bit
	mov	DWORD PTR 4[edi], eax	;

	pop	eax			;
	pop	edi			;
	pop	esi			;
    }
}

#endif
