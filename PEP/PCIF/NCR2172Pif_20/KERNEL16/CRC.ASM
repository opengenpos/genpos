;===========================================================================
; Title       : CRC-CCITT
; Category    : Kernel, NCR 2170 Operating System
; Program Name: CRCCCITT.ASM
; --------------------------------------------------------------------------
; Compiler    : MASM Ver. 6.00A/7.00 by Microsoft Corp.
; Memory Model: Compact/Small Model
; Options     : _WINDOWS=1 - MS-WINDOWS Version
; --------------------------------------------------------------------------
; Abstract:     CRC-CCITT = X16 + X12 + X5 + 1
;               MSB side = HIGH-BIT
;
; --------------------------------------------------------------------------
; Update Histories
;
; Date       Ver.      Name         Description
; Nov-16-92  00.00.01  O.Nakada     Initial
;
;===========================================================================
;===========================================================================
; PVCS Entry
; --------------------------------------------------------------------------
; $Revision$
; $Date$
; $Author$
; $Log$
;===========================================================================


;===========================================================================
;   Compile Declarations
;===========================================================================
ifdef   _WINDOWS
memC    EQU     1
else
        .MODEL  COMPACT
endif


;===========================================================================
;   Include File Declarations
;===========================================================================
ifdef   _WINDOWS
include cmacros.inc
endif


;===========================================================================
;   Work Area Declarations
;===========================================================================
;===========================================================================
;   Public Work Area
;===========================================================================


;===========================================================================
;   Static Work Area 
;===========================================================================


;===========================================================================
;   Code Area Declarations
;===========================================================================
ifdef   _WINDOWS
sBegin  CODE
else
        .CODE
endif


;===========================================================================
;       Assembler Language I/F
;===========================================================================
;     Entry:    SS:[BP + 4]  - pointer of CRC data
;               SS:[BP + 8]  - length of CRC data
;               SS:[BP + 10] - initial CRC data
;
;     Exit:     AX           - new CRC data
;===========================================================================
;       C Language I/F
;===========================================================================
;* Synopsis:    USHORT XinuCrcCcitt(VOID FAR *pBuffer, USHORT sLength,
;                                   USHORT usInitCrc);
;     Input:    pBuffer   - pointer of CRC data
;               usLength  - length of CRC data
;               usInitCrc - inital CRC data
;
;* Return:      new CRC data
;===========================================================================
ifdef   _WINDOWS
cProc   _XinuCrc, <PUBLIC, NEAR>
cBegin
else
        public  _XinuCrc
_XinuCrc        proc    near
endif

        push    bp
        mov     bp, sp

        push    bx
        push    cx
        push    dx
        push    ds
        push    es
        push    si
        push    di

        les     di, ss:[bp + 4]            
        push    es
        pop     ds
        mov     cx, ss:[bp + 8]
        mov     bx, ss:[bp + 10]

ccitt_byte_1:
        xor     bh, [di]                ;bh = data + crc_high
        inc     di                      ;pointer next
        xchg    bh, bl                  ;setup initial data
        mov     al, bl                  ;
        shr     al, 1                   ;al >> 4
        shr     al, 1                   ;
        shr     al, 1                   ;
        shr     al, 1                   ;
        xor     bl, al                  ;bl = bl + ah
        mov     ah, bl                  ;
        xor     al, al                  ;ax = bl * ff
        shr     ax, 1                   ;ax  >> 3
        shr     ax, 1                   ;
        shr     ax, 1                   ;
        xor     bx, ax                  ;bx = bx + ax
        shr     ax, 1                   ;ax >> 1
        xor     bh, al                  ;bx = new_crc
        loop    ccitt_byte_1            ;

        mov     ax, bx                  ;
    
        pop     di
        pop     si
        pop     es
        pop     ds
        pop     dx
        pop     cx
        pop     bx

        pop     bp

ifdef   _WINDOWS
cEnd
sEnd    Code
else
        ret
_XinuCrc        endp
endif

        END
