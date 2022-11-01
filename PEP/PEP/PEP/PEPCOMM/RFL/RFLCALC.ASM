;/*
;****************************************************************************
;**                                                                        **
;**        *=*=*=*=*=*=*=*=*                                               **
;**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
;**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            **
;**    <|\/~               ~\/|>                                           **
;**   _/^\_                 _/^\_                                          **
;**                                                                        **
;****************************************************************************
;*===========================================================================
;* Title       : Calculation for HUGEINT(8 byte data)
;* Category    : Reentrant Functions Library, NCR 2170   Application
;* Program Name: RflCalc.asm
;* --------------------------------------------------------------------------
;* Assembler   : MASM Ver. 6.0 by Microsoft Corp.
;* --------------------------------------------------------------------------
;* Abstruct:
;*      RflAdd64to64()
;*      RflMul32by32()
;*      RflDiv64by32()
;*      RflCast64to32()
;*      RflCast32to64()
;*      RflMul64by32()                                     * R2.0 *
;*      RflDiv96by32()                                     * R2.0 *
;*      RflAdd96to96()                                     * R2.0 *
;*      RflCast32to96()                                    * R2.0 *
;*      RflCast96to64()                                    * R2.0 *
;*      RflCast64to16()                                    * R2.0 *
;* --------------------------------------------------------------------------
;* Update Histories
;*  Date     : Ver.Rev. :   Name     : Description
;* Jul-06-92 : 00.00.01 : K.Nakajima :
;* Oct-15-96 : 02.00.00 : T.Yatuhasi : Adds RflMul64by32(),RflDiv96by32(),
;*                                     RflAdd96to96(),RflCast32to96(),
;*                                     RflCast96to64(),RflCast64to16()
;*                                     for VAT Tax Option
;*           :          :            :
;*===========================================================================
;*===========================================================================
;* PVCS Entry
;* --------------------------------------------------------------------------
;* $Revision$
;* $Date$
;* $Author$
;* $Log$
;*===========================================================================
;*/
;

;/*
;============================================================================
;              D A T A   A R E A S   A N D   E Q U A T E S
;============================================================================
;*/
HEAD            equ     6               ;4(return address) + 2(bp size)


;/*
;=============================================================================
;+                        I N C L U D E   F I L E s
;=============================================================================
;*/
;
;/**
;============================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;============================================================================
;**/
;/*
;*===========================================================================
;** Format  : VOID    RflAdd64to64(HUGEINT *pAdd1, HUGEINT *pAdd2);
;*
;*   Input  : HUGEINT     *pAdd1
;*            HUGEINT     *pAdd2
;*   Output : none
;*   InOut  : none
;** Return  : none
;*
;** Synopsis: This function adds *pAdd1 to *pAdd2, and storage added value to *pAdd1
;*===========================================================================
;*/

          .MODEL   medium, c
          .CODE
RflAdd64to64  PROTO C pAdd1:PTR SBYTE, pAdd2:PTR SBYTE

RFLCALC_PROG SEGMENT BYTE PUBLIC 'CODE'
             ASSUME CS:RFLCALC_PROG

PUBLIC          RflAdd64to64
RflAdd64to64       PROC    FAR

ARGADD STRUCT
        Add1           WORD   ?
        Add2           WORD   ?
ARGADD ENDS


        push    bp
        mov     bp, sp
        push    si
        push    di
        mov     di, [bp + HEAD].ARGADD.Add1
        mov     si, [bp + HEAD].ARGADD.Add2

        mov     ax, word ptr [si]
        add     word ptr [di], ax
        mov     ax, word ptr [si + 2]
        adc     word ptr [di + 2], ax
        mov     ax, word ptr [si + 4]
        adc     word ptr [di + 4], ax
        mov     ax, word ptr [si + 6]
        adc     word ptr [di + 6], ax

        pop     di
        pop     si
        pop     bp
        ret

RflAdd64to64      ENDP


;/*
;*===========================================================================
;** Format  : VOID    RflMul32by32(HUGEINT *pProduct, LONG lMultiplicant,
;*                                                    LONG lMulriplier);
;*
;*   Input  : LONG lMultiplicant
;*            LONG lMultiplier
;*   Output : HUGEINT  *pProduct        -product storage area address
;*   InOut  : none
;** Return  : none
;*
;** Synopsis: This function multiply long by long and storage  *pProduct
;*===========================================================================
;*/

RflMul32by32  PROTO C pProduct:PTR SBYTE, lMultiplicant:SDWORD, lMultiplicant:SDWORD


PUBLIC             RflMul32by32
RflMul32by32       PROC    FAR

ARGMUL STRUCT
        Prod            WORD   ?
        Mul1_low        WORD   ?
        Mul1_high       WORD   ?
        Mul2_low        WORD   ?
        Mul2_high       WORD   ?
ARGMUL ENDS

        push    bp
        mov     bp, sp
        push    si
        mov     si, [bp + HEAD].ARGMUL.Prod
        mov     word ptr [si+4], 0      ;initialize
        mov     word ptr [si+6], 0

        mov     ch, byte ptr [bp + HEAD].ARGMUL.Mul1_high+1
        or      ch, ch                                      ;minus management
        jns     PLUS1
        not     word ptr [bp + HEAD].ARGMUL.Mul1_high
        neg     word ptr [bp + HEAD].ARGMUL.Mul1_low
        sbb     word ptr [bp + HEAD].ARGMUL.Mul1_high, -1
PLUS1:
        xor     ch, byte ptr [bp + HEAD].ARGMUL.Mul2_high+1 ;store sign
        mov     ax, word ptr [bp + HEAD].ARGMUL.Mul2_high
        or      ax, ax                                      ;minus management
        jns     PLUS2
        not     word ptr [bp + HEAD].ARGMUL.Mul2_high
        neg     word ptr [bp + HEAD].ARGMUL.Mul2_low
        sbb     word ptr [bp + HEAD].ARGMUL.Mul2_high, -1
PLUS2:
        mov     ax, word ptr [bp + HEAD].ARGMUL.Mul1_low
        mov     bx, word ptr [bp + HEAD].ARGMUL.Mul2_low
        mul     bx
        mov     word ptr [si], ax
        mov     word ptr [si + 2], dx

        mov     ax, word ptr [bp + HEAD].ARGMUL.Mul1_low
        mov     bx, word ptr [bp + HEAD].ARGMUL.Mul2_high
        mul     bx
        add     word ptr [si + 2], ax
        adc     word ptr [si + 4], dx
        adc     word ptr [si + 6], 0

        mov     ax, word ptr [bp + HEAD].ARGMUL.Mul1_high
        mov     bx, word ptr [bp + HEAD].ARGMUL.Mul2_low
        mul     bx
        add     word ptr [si + 2], ax
        adc     word ptr [si + 4], dx
        adc     word ptr [si + 6], 0

        mov     ax, word ptr [bp + HEAD].ARGMUL.Mul1_high
        mov     bx, word ptr [bp + HEAD].ARGMUL.Mul2_high
        mul     bx
        add     word ptr [si + 4], ax
        adc     word ptr [si + 6], dx

        or      ch, ch
        jns     PLUS3
        not     word ptr [si + 2]               ;minus management
        not     word ptr [si + 4]
        not     word ptr [si + 6]
        neg     word ptr [si]
        sbb     word ptr [si + 2], -1
        sbb     word ptr [si + 4], -1
        sbb     word ptr [si + 6], -1
PLUS3:
        pop     si
        pop     bp
        ret

RflMul32by32      ENDP


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

RflDiv64by32  PROTO C pQuot:PTR SBYTE, plRemain:PTR SDWORD, lDivisor:SDWORD


PUBLIC             RflDiv64by32
RflDiv64by32       PROC    FAR

ARGDIV STRUCT
        Quant           WORD   ?
        Remain          WORD   ?
        Divisor_low     WORD   ?
        Divisor_high    WORD   ?
ARGDIV ENDS

        push    bp
        mov     bp, sp
        push    si
        push    di
        mov     ax, [bp + HEAD].ARGDIV.Divisor_high ;0 divide check
        or      ax, [bp + HEAD].ARGDIV.Divisor_low
        jnz     CONT
        jmp     FINISH
CONT:
        mov     si, [bp + HEAD].ARGDIV.Quant        ;write divident address
        mov     dx, [bp + HEAD].ARGDIV.Divisor_high ;write divisior's high word
        mov     di, [bp + HEAD].ARGDIV.Divisor_low  ;write divisior's low word

        mov     ch, byte ptr [si + 7]   ;store divident's sign
        or      ch, ch                  ;check minus (divident)
        jns     P1
        not     word ptr [si + 2]       ;minus management for divident
        not     word ptr [si + 4]
        not     word ptr [si + 6]
        neg     word ptr [si]
        sbb     word ptr [si + 2], -1
        sbb     word ptr [si + 4], -1
        sbb     word ptr [si + 6], -1
P1:
        or      dh, dh                  ;check minus (divisior)
        jns     P2
        not     dx                      ;minus management
        neg     di
        sbb     dx, -1
P2:
        push    cx                      ;store sign
        mov     cx, 64                  ;set loop counter
        xor     ax, ax                  ;initialize  remainder area
        mov     bx, ax

; ------------------------------------
;  registers store data as follows
;    ax <= remainder low
;    bx <= remainder high
;    dx <= divisor high
;    di <= divisor low
; ------------------------------------

SUBTLOOP:
        shl     word ptr [si], 1
        rcl     word ptr [si + 2], 1
        rcl     word ptr [si + 4], 1
        rcl     word ptr [si + 6], 1
        rcl     ax, 1
        rcl     bx, 1
        cmp     bx, dx                  ;compare high word
        jb      NOTSUBT
        ja      SUBTRACT
        cmp     ax, di                  ;compare low word
        jb      NOTSUBT

SUBTRACT:
        sub     ax, di                  ;subtract low word
        sbb     bx, dx                  ;subtract high word
        inc     word ptr [si]           ;increment quot area
NOTSUBT:
        loop    SUBTLOOP

        pop     cx                      ;restore divident's sign
        or      ch, ch                  ;check minus
        jns     P3
        not     bx                      ;minus management for remainder
        neg     ax                      ;Remainder's sign is always same as
        sbb     bx, -1                  ;  divident's sign.
P3:
        mov     di, [bp + HEAD].ARGDIV.Remain ;write remainder
        mov     word ptr[di], ax
        mov     word ptr[di + 2], bx

        mov     dx, [bp + HEAD].ARGDIV.Divisor_high
        xor     ch, dh                  ;check divisor's sign and divident's
        or      ch, ch
        jns     FINISH
        not     word ptr [si + 2]       ;minus management for quot
        not     word ptr [si + 4]
        not     word ptr [si + 6]
        neg     word ptr [si]
        sbb     word ptr [si + 2], -1
        sbb     word ptr [si + 4], -1
        sbb     word ptr [si + 6], -1

FINISH:
        pop     di
        pop     si
        pop     bp
        ret

RflDiv64by32      ENDP



;/*
;*===========================================================================
;** Format  : VOID    RflCast64to32(LONG *lDest, HUGEINT *pSource);
;*
;*   Input  : HUGEINT  *pSource       -source data
;*   Output : LONG     lDest          -destination data
;*   InOut  : none
;** Return  : none
;*
;** Synopsis: This function converts 64bit data to 32bit data
;*===========================================================================
;*/

RflCast64to32  PROTO C lDest:PTR SDWORD, pSource:PTR BYTE


PUBLIC          RflCast64to32
RflCast64to32       PROC    FAR

A64TO32 STRUCT
        Dest            WORD   ?
        Sourc           WORD   ?
A64TO32 ENDS


        push    bp
        mov     bp, sp
        push    si
        push    di
        mov     di, [bp + HEAD].A64TO32.Dest
        mov     si, [bp + HEAD].A64TO32.Sourc

        mov     ax, word ptr [si]               ;write low word
        mov     word ptr [di], ax

        mov     ax, word ptr [si + 2]           ;write high word
        mov     word ptr [di + 2], ax

        pop     di
        pop     si
        pop     bp
        ret

RflCast64to32       ENDP

;/*
;*===========================================================================
;** Format  : VOID    RflCast32to64(HUGEINT *pDest, LONG lSource);
;*
;*   Input  : LONG      lSource         -Source data
;*   Output : HUGEINT   *pDest          -Destination data
;*   InOut  : none
;** Return  : none
;*
;** Synopsis: This function converts 32bit data to 64bit data
;*===========================================================================
;*/

RflCast32to64  PROTO C hDest:PTR BYTE, lSource:SDWORD

PUBLIC              RflCast32to64
RflCast32to64       PROC    FAR

A32TO64 STRUCT
        Dest            WORD   ?
        Sourc_low       WORD   ?
        Sourc_hi        WORD   ?
A32TO64 ENDS


        push    bp
        mov     bp, sp
        push    si
        mov     si, [bp + HEAD].A32TO64.Dest

        mov     ax, word ptr [bp + HEAD].A32TO64.Sourc_hi
        or      ax, ax
        jns     PLUS
        mov     word ptr [si + 4], -1       ;write hi 4 byte (minus)
        mov     word ptr [si + 6], -1
        jmp     COMMON
PLUS:
        mov     word ptr [si + 4], 0        ;write hi 4 byte (plus)
        mov     word ptr [si + 6], 0
COMMON:
        mov     ax, [bp + HEAD].A32TO64.Sourc_low
        mov     word ptr [si], ax
        mov     ax, [bp + HEAD].A32TO64.Sourc_hi
        mov     word ptr [si + 2], ax
        pop     si
        pop     bp
        ret

RflCast32to64       ENDP

;/*
;*===========================================================================
;** Format  : VOID    RflMul64by32(EHUGEINT *pProduct, HUGEINT *pMultiplicant,
;*                                                    LONG lMulriplier);
;*
;*   Input  : HUGEINT *plMultiplicant
;*            LONG lMultiplier
;*   Output : EHUGEINT  *pProduct        -product storage area address
;*   InOut  : none
;** Return  : none
;*
;** Synopsis: This function multiply 64bits by long and storage  *pProduct
;*===========================================================================
;*/

RflMul64by32  PROTO C pProduct:PTR SBYTE, pMultiplicant:PTR SBYTE, lMultiplier:SDWORD


PUBLIC             RflMul64by32
RflMul64by32       PROC    FAR

EARGMUL STRUCT
        Prod            WORD   ?
        Mul1            WORD   ?
        Mul2_low        WORD   ?
        Mul2_high       WORD   ?
EARGMUL ENDS

        push    bp
        mov     bp, sp
        push    si
        push    di
        mov     si, [bp + HEAD].EARGMUL.Prod
        mov     di, [bp + HEAD].EARGMUL.Mul1
        mov     word ptr [si+4], 0      ;initialize
        mov     word ptr [si+6], 0
        mov     word ptr [si+8], 0
        mov     word ptr [si+10], 0

        mov     ch, byte ptr [di+7]                         ;store Multiplicant's sign
        or      ch, ch                                      ;minus management
        jns     EPLUS1
        not     word ptr [di + 2]
        not     word ptr [di + 4]
        not     word ptr [di + 6]
        neg     word ptr [di]
        sbb     word ptr [di + 2], -1
        sbb     word ptr [di + 4], -1
        sbb     word ptr [di + 6], -1

EPLUS1:
        xor     ch, byte ptr [bp + HEAD].EARGMUL.Mul2_high+1 ;store sign
        mov     ax, word ptr [bp + HEAD].EARGMUL.Mul2_high
        or      ax, ax                                      ;minus management
        jns     EPLUS2
        not     word ptr [bp + HEAD].EARGMUL.Mul2_high
        neg     word ptr [bp + HEAD].EARGMUL.Mul2_low
        sbb     word ptr [bp + HEAD].EARGMUL.Mul2_high, -1
EPLUS2:
        mov     ax, word ptr [di]
        mov     bx, word ptr [bp + HEAD].EARGMUL.Mul2_low
        mul     bx
        mov     word ptr [si], ax
        mov     word ptr [si + 2], dx

        mov     ax, word ptr [di]
        mov     bx, word ptr [bp + HEAD].EARGMUL.Mul2_high
        mul     bx
        add     word ptr [si + 2], ax
        adc     word ptr [si + 4], dx
        adc     word ptr [si + 6], 0
        adc     word ptr [si + 8], 0
        adc     word ptr [si + 10], 0

        mov     ax, word ptr [di+2]
        mov     bx, word ptr [bp + HEAD].EARGMUL.Mul2_low
        mul     bx
        add     word ptr [si + 2], ax
        adc     word ptr [si + 4], dx
        adc     word ptr [si + 6], 0
        adc     word ptr [si + 8], 0
        adc     word ptr [si + 10], 0

        mov     ax, word ptr [di+2]
        mov     bx, word ptr [bp + HEAD].EARGMUL.Mul2_high
        mul     bx
        add     word ptr [si + 4], ax
        adc     word ptr [si + 6], dx
        adc     word ptr [si + 8], 0
        adc     word ptr [si + 10], 0

        mov     ax, word ptr [di+4]
        mov     bx, word ptr [bp + HEAD].EARGMUL.Mul2_low
        mul     bx
        add     word ptr [si + 4], ax
        adc     word ptr [si + 6], dx
        adc     word ptr [si + 8], 0
        adc     word ptr [si + 10], 0

        mov     ax, word ptr [di+4]
        mov     bx, word ptr [bp + HEAD].EARGMUL.Mul2_high
        mul     bx
        add     word ptr [si + 6], ax
        adc     word ptr [si + 8], dx
        adc     word ptr [si + 10], 0

        mov     ax, word ptr [di+6]
        mov     bx, word ptr [bp + HEAD].EARGMUL.Mul2_low
        mul     bx
        add     word ptr [si + 6], ax
        adc     word ptr [si + 8], dx
        adc     word ptr [si + 10], 0

        mov     ax, word ptr [di+6]
        mov     bx, word ptr [bp + HEAD].EARGMUL.Mul2_high
        mul     bx
        add     word ptr [si + 8], ax
        adc     word ptr [si + 10], dx

        or      ch, ch
        jns     EPLUS3
        not     word ptr [si + 2]               ;minus management
        not     word ptr [si + 4]
        not     word ptr [si + 6]
        not     word ptr [si + 8]
        not     word ptr [si + 10]
        neg     word ptr [si]
        sbb     word ptr [si + 2], -1
        sbb     word ptr [si + 4], -1
        sbb     word ptr [si + 6], -1
        sbb     word ptr [si + 8], -1
        sbb     word ptr [si + 10], -1
EPLUS3:
        pop     di
        pop     si
        pop     bp
        ret

RflMul64by32      ENDP


;/*
;*===========================================================================
;** Format  : VOID    RflDiv96by32(EHUGEINT *pQuot, LONG *plRemain,
;*                                                 LONG lDivisor);
;*
;*   Input  : LONG     lDivisor       -divisor
;*   Output : LONG     *plRemain      -Remain
;*   InOut  : EHUGEINT  *pQuot         -divident and quot
;** Return  : none
;*
;** Synopsis: This function divides 96bit data that stored in pQuot,
;*            and writes Quot to pQuot, Remain to plRemain.
;*===========================================================================
;*/

RflDiv96by32  PROTO C pQuot:PTR SBYTE, plRemain:PTR SDWORD, lDivisor:SDWORD


PUBLIC             RflDiv96by32
RflDiv96by32       PROC    FAR

EARGDIV STRUCT
        Quant           WORD   ?
        Remain          WORD   ?
        Divisor_low     WORD   ?
        Divisor_high    WORD   ?
EARGDIV ENDS

        push    bp
        mov     bp, sp
        push    si
        push    di
        mov     ax, [bp + HEAD].EARGDIV.Divisor_high ;0 divide check
        or      ax, [bp + HEAD].EARGDIV.Divisor_low
        jnz     ECONT
        jmp     EFINISH
ECONT:
        mov     si, [bp + HEAD].EARGDIV.Quant        ;write divident address
        mov     dx, [bp + HEAD].EARGDIV.Divisor_high ;write divisior's high word
        mov     di, [bp + HEAD].EARGDIV.Divisor_low  ;write divisior's low word

        mov     ch, byte ptr [si + 11]   ;store divident's sign
        or      ch, ch                  ;check minus (divident)
        jns     EP1
        not     word ptr [si + 2]       ;minus management for divident
        not     word ptr [si + 4]
        not     word ptr [si + 6]
        not     word ptr [si + 8]
        not     word ptr [si + 10]
        neg     word ptr [si]
        sbb     word ptr [si + 2], -1
        sbb     word ptr [si + 4], -1
        sbb     word ptr [si + 6], -1
        sbb     word ptr [si + 8], -1
        sbb     word ptr [si + 10], -1
EP1:
        or      dh, dh                  ;check minus (divisior)
        jns     EP2
        not     dx                      ;minus management
        neg     di
        sbb     dx, -1
EP2:
        push    cx                      ;store sign
        mov     cx, 96                  ;set loop counter
        xor     ax, ax                  ;initialize  remainder area
        mov     bx, ax

; ------------------------------------
;  registers store data as follows
;    ax <= remainder low
;    bx <= remainder high
;    dx <= divisor high
;    di <= divisor low
; ------------------------------------

ESUBTLOP:
        shl     word ptr [si], 1
        rcl     word ptr [si + 2], 1
        rcl     word ptr [si + 4], 1
        rcl     word ptr [si + 6], 1
        rcl     word ptr [si + 8], 1
        rcl     word ptr [si + 10], 1
        rcl     ax, 1
        rcl     bx, 1
        cmp     bx, dx                  ;compare high word
        jb      ENOTSUBT
        ja      ESUBT
        cmp     ax, di                  ;compare low word
        jb      ENOTSUBT

ESUBT:
        sub     ax, di                  ;subtract low word
        sbb     bx, dx                  ;subtract high word
        inc     word ptr [si]           ;increment quot area
ENOTSUBT:
        loop    ESUBTLOP

        pop     cx                      ;restore divident's sign
        or      ch, ch                  ;check minus
        jns     EP3
        not     bx                      ;minus management for remainder
        neg     ax                      ;Remainder's sign is always same as
        sbb     bx, -1                  ;  divident's sign.
EP3:
        mov     di, [bp + HEAD].EARGDIV.Remain ;write remainder
        mov     word ptr[di], ax
        mov     word ptr[di + 2], bx

        mov     dx, [bp + HEAD].EARGDIV.Divisor_high
        xor     ch, dh                  ;check divisor's sign and divident's
        or      ch, ch
        jns     EFINISH
        not     word ptr [si + 2]       ;minus management for quot
        not     word ptr [si + 4]
        not     word ptr [si + 6]
        not     word ptr [si + 8]
        not     word ptr [si + 10]
        neg     word ptr [si]
        sbb     word ptr [si + 2], -1
        sbb     word ptr [si + 4], -1
        sbb     word ptr [si + 6], -1
        sbb     word ptr [si + 8], -1
        sbb     word ptr [si + 10], -1

EFINISH:
        pop     di
        pop     si
        pop     bp
        ret

RflDiv96by32      ENDP

;/*
;*===========================================================================
;** Format  : VOID    RflAdd96to96(EHUGEINT *pAdd1, EHUGEINT *pAdd2);
;*
;*   Input  : EHUGEINT     *pAdd2
;*
;*   Output : none
;*   InOut  : EHUGEINT     *pAdd1
;** Return  : none
;*
;** Synopsis: This function adds *pAdd1 to *pAdd2, and storage added value to *pAdd1
;*===========================================================================
;*/


RflAdd96to96  PROTO C pAdd1:PTR SBYTE, pAdd2:PTR SBYTE

PUBLIC          RflAdd96to96
RflAdd96to96       PROC    FAR

EARGADD STRUCT
        Add1           WORD   ?
        Add2           WORD   ?
EARGADD ENDS


        push    bp
        mov     bp, sp
        push    si
        push    di
        mov     di, [bp + HEAD].EARGADD.Add1
        mov     si, [bp + HEAD].EARGADD.Add2

        mov     ax, word ptr [si]
        add     word ptr [di], ax
        mov     ax, word ptr [si + 2]
        adc     word ptr [di + 2], ax
        mov     ax, word ptr [si + 4]
        adc     word ptr [di + 4], ax
        mov     ax, word ptr [si + 6]
        adc     word ptr [di + 6], ax
        mov     ax, word ptr [si + 8]
        adc     word ptr [di + 8], ax
        mov     ax, word ptr [si + 10]
        adc     word ptr [di + 10], ax

        pop     di
        pop     si
        pop     bp
        ret

RflAdd96to96      ENDP


;/*
;*===========================================================================
;** Format  : VOID    RflCast32to96(EHUGEINT *pDest, LONG lSource);
;*
;*   Input  : LONG      lSource         -Source data
;*   Output : EHUGEINT  *pDest          -Destination data
;*   InOut  : none
;** Return  : none
;*
;** Synopsis: This function converts 32bit data to 96bit data
;*===========================================================================
;*/

RflCast32to96  PROTO C hDest:PTR BYTE, lSource:SDWORD

PUBLIC              RflCast32to96
RflCast32to96       PROC    FAR

A32TO96 STRUCT
        Dest            WORD   ?
        Sourc_low       WORD   ?
        Sourc_hi        WORD   ?
A32TO96 ENDS


        push    bp
        mov     bp, sp
        push    si
        mov     si, [bp + HEAD].A32TO96.Dest

        mov     ax, word ptr [bp + HEAD].A32TO96.Sourc_hi
        or      ax, ax
        jns     EPLUS
        mov     word ptr [si + 4], -1       ;write hi 8 byte (minus)
        mov     word ptr [si + 6], -1
        mov     word ptr [si + 8], -1
        mov     word ptr [si + 10], -1
        jmp     ECOMMON
EPLUS:
        mov     word ptr [si + 4], 0        ;write hi 8 byte (plus)
        mov     word ptr [si + 6], 0
        mov     word ptr [si + 8], 0
        mov     word ptr [si + 10], 0
ECOMMON:
        mov     ax, [bp + HEAD].A32TO96.Sourc_low
        mov     word ptr [si], ax
        mov     ax, [bp + HEAD].A32TO96.Sourc_hi
        mov     word ptr [si + 2], ax
        pop     si
        pop     bp
        ret

RflCast32to96       ENDP

;/*
;*===========================================================================
;** Format  : VOID    RflCast96to64(HUGEINT *pDest, EHUGEINT *pSource);
;*
;*   Input  : EHUGEINT  *pSource       -source data
;*   Output : HUGEINT   *pDest         -destination data
;*   InOut  : none
;** Return  : none
;*
;** Synopsis: This function converts 96bit data to 64bit data
;*===========================================================================
;*/

RflCast96to64  PROTO C lDest:PTR BYTE, pSource:PTR BYTE


PUBLIC          RflCast96to64
RflCast96to64       PROC    FAR

A96TO64 STRUCT
        Dest            WORD   ?
        Sourc           WORD   ?
A96TO64 ENDS


        push    bp
        mov     bp, sp
        push    si
        push    di
        mov     di, [bp + HEAD].A96TO64.Dest
        mov     si, [bp + HEAD].A96TO64.Sourc

        mov     ax, word ptr [si]               ;write 4th word
        mov     word ptr [di], ax

        mov     ax, word ptr [si + 2]           ;write 3rd word
        mov     word ptr [di + 2], ax

        mov     ax, word ptr [si + 4]           ;write 2nd word
        mov     word ptr [di + 4], ax

        mov     ax, word ptr [si + 6]           ;write 1st word
        mov     word ptr [di + 6], ax

        pop     di
        pop     si
        pop     bp
        ret

RflCast96to64       ENDP


;/*
;*===========================================================================
;** Format  : VOID    RflCast64to16(short *sDest, HUGEINT *pSource);
;*
;*   Input  : HUGEINT  *pSource       -source data
;*   Output : short     sDest          -destination data
;*   InOut  : none
;** Return  : none
;*
;** Synopsis: This function converts 64bit data to 16bit data
;*===========================================================================
;*/

RflCast64to16  PROTO C lDest:PTR SDWORD, pSource:PTR BYTE


PUBLIC          RflCast64to16
RflCast64to16       PROC    FAR

A64TO16 STRUCT
        Dest            WORD   ?
        Sourc           WORD   ?
A64TO16 ENDS


        push    bp
        mov     bp, sp
        push    si
        push    di
        mov     di, [bp + HEAD].A64TO16.Dest
        mov     si, [bp + HEAD].A64TO16.Sourc

        mov     ax, word ptr [si]               ;write low word
        mov     word ptr [di], ax

        pop     di
        pop     si
        pop     bp
        ret

RflCast64to16       ENDP

RFLCALC_PROG ENDS
             END
