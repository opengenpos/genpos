/*
;***************************************************************************
;*                                                                        **
;*        *=*=*=*=*=*=*=*=*                                               **
;*        *  NCR 2170     *             NCR Corporation, E&M OISO         **
;*     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1998            **
;*    <|\/~               ~\/|>                                           **
;*   _/^\_                 _/^\_                                          **
;*                                                                        **
;***************************************************************************
;===========================================================================
; Title       : Un-initialized area deffinition Module
; Category    : Parameter and Server work, NCR 2170 US Hospitality Application
; Program Name: UNINIRAM.ASM
; --------------------------------------------------------------------------
;* This module must be assembled with the following switch,
;*
;* ML /c /Cx
;*
;*           where,  /c      means "compile but do not link"
;*                   /Cx     means "preserve case of identifiers"
;*
; --------------------------------------------------------------------------
; Abstract: The included area are as follows:
;   Parameter:  _ParaMDC        ; Prog. 1
;           _ParaFlexMem        ; Prog. 2
;           _ParaFSC            ; Prog. 3
;           _ParaSecurityNo     ; Prog. 6
;           _ParaStoRegNo       ; Prog. 7
;           _ParaSupLevel       ; Prog. 8
;           _ParaActCodeSec     ; Prog. 9
;           _ParaTransHALO      ; Prog. 10
;           _ParaPresetAmount   ; Prog. 15
;           _ParaHourlyTime     ; Prog. 17
;           _ParaSlipFeedCtl    ; Prog. 18
;           _ParaTransMnemo     ; Prog. 20
;           _ParaLeadThru       ; Prog. 21
;           _ParaReportName     ; Prog. 22
;           _ParaSpeMnemo       ; Prog. 23
;           _ParaPCIF           ; Prog. 39
;           _ParaAdjMnemo       ; Prog. 46
;           _ParaPrtModi        ; Prog. 47
;           _ParaMajorDEPT      ; Prog. 48
;           _ParaAutoAltKitch   ; Prog. 49
;           _ParaSharedPrt      ; Prog. 50
;           _ParaHotelId        ; Prog. 54
;           _ParaChar24         ; Prog. 57
;           _ParaTtlKeyTyp      ; Prog. 60
;           _ParaTtlKeyCtl      ; Prog. 61
;           _ParaTend           ; Prog. 62,     V3.3
;           _ParaPLUNoMenu      ; A/C 4
;           _ParaCtlTblMenu     ; A/C 5
;           _ParaManuAltKitch   ; A/C 6
;           _ParaCashABAssign   ; A/C 7
;           _ParaRound          ; A/C 84
;           _ParaDisc           ; A/C 86
;           _ParaSoftChk        ; A/C 87
;           _ParaPromotion      ; A/C 88
;           _ParaCurrency       ; A/C 89
;           _ParaToneCtl        ; A/C 165
;           _ParaMenuPLU        ; A/C 116
;           _ParaTaxTable       ; A/C 124-126
;           _ParaTaxRate        ; A/C 127
;           _ParaPigRule        ; A/C 130
;           _ParaServieTime     ; A/C 133
;           _ParaOep            ; A/C 160
;
;   Counter _ParaSpcCo          ; Counter
;
;   Server  _usSerCurStatus     ; Server work
;           _ausSerCurTransNo   ; Server work
;           _ausSerPreTransNo   ; Server work
;           _fsSerCurBcasInf    ; Server work
;           _SerCurDate         ; Server work
;
;
; --------------------------------------------------------------------------
; Update Histories
;    Date   : Rev.Ver.  :  Name     : Description
; Jul-15-92 : V00.00.01 :T.Asawa    : initial
; Oct-27-92 : V01.00.00 :K.Terai    : Add TtlUphdata
; Oct-29-92 : V01.00.00 :H.Yamaguchi: Separate uninitialize data, adds Tare
;           :           :           : Expands MDC size and Tranzaction mnemonics.
; Dec-24-92     : V01.00.00 :K.You      : Del MAX_TTBLHEAD_SIZE,MAX_TTBLRATE_SIZE
; Jul-23-93     : V01.00.12 :K.You      : Add US Enhance feature.
; Aug-23-93     : V01.00.13 :J.IKEDA    : Add ParaSoftChk
; Oct-04-93     : V02.00.01 :K.You      : Add Addr. 64 of Prog#21
; Jul-11.93     : v03.00.00 :M.Yamamoto : Add Hotel Id, Sld Id
; Apr-04-94     : V00.00.05 :Yoshiko.Jim: Add EPT feature in GPUS
; Apr-22-94     : V00.00.05 :K.You      : bug fixed E-14 set default value to prog#2 addr 7,8
; Apr-28-94     : V02.05.00 :M.Yamamoto : Add Lead-Through No 85.
; May-19-94     : V02.05.00 :K.You      : Chg. default data of Prog. 23/Addr. 27.
; Feb-23-95     : V03.00.00 :H.Ishida   : Add Coupon & Control String Size by PROG #2
;               :           :           : Chg. MAX_REPO_NO , Add Coupon Report Name
;               :           :           : Add _ParaOep of A/C 160
; Apr-14-95     : V03.00.00 :H.Ishida   : Add Addr. 88,89,90,91,92 of Prog. #21
; Nov-09-95     : V03.01.00 :M.Ozawa    : Change for R3.1
; Aug-04-98     : V03.03.00 :hrkato     : Change for V3.3
;
;** NCR2171 **
; Sep-06-99     ; V01.00.00 :M.Teraki   : Rewrite to "C"
;===========================================================================
;===========================================================================
; PVCS Entry
; --------------------------------------------------------------------------
; $Revision$
; $Date$
; $Author$
; $Log$
;===========================================================================
\*------------------------------------------------------------------------*/

#include "ecr.h"
#include "plu.h"         /* Saratoga */
/*#include "csstbfcc.h"*/
#include "pif.h"
#include "paraequ.h"
#include "para.h"
#include "pararam.h"

#if !defined(_T)
// if text has been copied over from NHPOS that contains the _T() macro, lets define it.
#define _T(x) L ## x
#endif

UNINIRAM Para = {                 /* Uninitialize RAM */
    {   /* File Version Information */
    ' ',' ', ' ', ' ', ' ', ' ',' ', ' ', ' ', ' '
    },
    { /* Prog.1 SET MDC - transform MDC address to ram address use offset = ((usMdcAddress - 1) / 2)
	   *    odd address is lower nibble (0x0f) and even address is upper nibble (0xf0).
	   *    Bit A is upper bit of nibble (odd 0x08, even 0x80), Bit D is lower bit of nibble (odd 0x01, even 0x10).
	   */
		0,   0,   0,   0,   0,0x20,0xc0,   0, /*   [0]-  [7] [MDC_CLI1_ADR -> 1]-[MDC_CANTAX_ADR -> 16] */
		0,0x80,   0,   0,   0,   0,   0,   0, /*   [8]- [15] [MDC_CASIN_ADR -> 17]-[MDC_MODID12_ADR -> 32] */
		0,   0,   0,   0,   0,   0,   0,   0, /*  [16]- [23] [MDC_MODID13_ADR -> 33]-[MDC_PB_ADR -> 48] */
		0,   0,   0,   0,   0,   0,   0,   0, /*  [24]- [31] [MDC_ADDCHK1_ADR -> 49]-[MDC_TEND12_ADR -> 64] */
		0,   0,   0,   0,   0,   0,   0,   0, /*  [32]- [39] [MDC_TEND21_ADR -> 65]-[MDC_FC2_ADR -> 80] */
		0,   0,0x01,   0,   0,   0,   0,   0, /*  [40]- [47] [MDC_GCTRN_ADR -> 81]-[MDC_EODPTD_ADR -> 96] */
		0,   0,   0,   0,   0,   0,   0,   0, /*  [48]- [55] [MDC_WAITER_ADR -> 97]-[MDC_RPTFIN12_ADR -> 112] */
		0,0xfc,0xff,0xfc,0x03,   0,0xe0,0xff, /*  [56]- [63] [MDC_RPTFIN13_ADR -> 113]-[MDC_RPTFIN28_ADR -> 128] */
		0,   0,   0,   0,   0,   0,   0,   0, /*  [64]- [71] [MDC_RPTFIN29_ADR -> 129]-[MDC_RPTCAS14_ADR -> 144] */
		0,   0,   0,   0,   0,   0,   0,   0, /*  [72]- [79] [MDC_RPTCAS15_ADR -> 145]-[MDC_RPTWAI4_ADR -> 160] */
		0,   0,   0,   0,   0,   0,   0,   0, /*  [80]- [87] [MDC_RPTWAI5_ADR -> 161]-[MDC_DFL_SND_TIME -> 176] */
		0,   0,   0,   0,   0,   0,   0,   0, /*  [88]- [95] [MDC_DSP_ON_FLY -> 177]-[MDC_TEND92_ADR -> 192] */
		0,   0,   0,   0,   0,   0,   0,   0, /*  [96]-[103] [MDC_TEND101_ADR -> 193]-[MDC_TEND44_ADR -> 208] */
		0,   0,   0,   0,   0,   0,   0,   0, /* [104]-[111] [MDC_TEND53_ADR -> 209]-[MDC_TEND44_ADR -> 224] */
		0,   0,   0,   0,   0,   0,   0,   0, /* [112]-[119] [       -> 225]-[MDC_RDISC34_ADR -> 240] */
		0,   0,   0,   0,   0,   0,   0,   0, /* [120]-[127] [MDC_RDISC41_ADR -> 241]-[MDC_PTD4_ADR -> 256] */
		0,   0,   0,   0,   0,   0,   0,0xff, /* [128]-[135] [MDC_NEWKEYSEQ_ADR -> 257]-[MDC_HOUR3_ADR -> 272] */
		0,   0,   0,   0,   0,   0,   0,   0, /* [136]-[143] */
		0,   0,   0,   0,   0,   0,   0,   0, /* [144]-[151] *//* 2172 */
		0,   0,   0,   0,   0,   0,   0,   0, /* [152]-[159] */
		0,   0,   0,   0,   0,   0,   0,   0, /* [160]-[167] */
		0,   0,   0,   0,   0,   0,   0,   0, /* [168]-[175] */
		0,   0,   0,   0,   0,   0,   0,0x10, /* [176]-[183] [MDC_FS1_ADR -> 353]-[MDC_SYSTEM4_ADR -> 368]*/
		0,   0,   0,   0,   0,   0,   0,   0, /* [184]-[191] */
		0,   0,   0,   0,   0,   0,   0,   0, /* [192]-[199] */
	/*** NCR2172 ***/
		0,   0,   0,   0,   0,   0,   0,   0, /* [200]-[207] */
		0,   0,   0,   0,   0,   0,   0,   0, /* [208]-[215] */
		0,   0,   0,   0,   0,   0,   0,   0, /* [216]-[223] */
		0,   0,   0,   0,   0,   0,   0,0xff, /* [224]-[231] [MDC_RPTCAS44_ADR -> 449]-[MDC_RPTCAS47_ADR -> 464]*/
	 0xff,0xff,0xff,0xff,0x07,   0,   0,   0, /* [232]-[239] [MDC_RPTCAS48_ADR -> 465]-[MDC_EMPLOYEE_ID_ADR -> 479]*/
		0,   0,   0,   0,   0,   0,   0,   0, /* [240]-[247] */
		0,   0,   0,   0,   0,   0,   0,   0, /* [248]-[255] */
		0,   0,   0,   0,   0,   0,   0,   0, /* [256]-[263] */
		0,   0,   0,   0,   0,   0,   0,   0, /* [264]-[271] */
		0,   0,   0,   0,   0,   0,   0,   0, /* [272]-[279] */
		0,   0,   0,   0,   0,   0,   0,   0, /* [280]-[287] */
		0,   0,   0,   0,   0,   0,   0,   0, /* [288]-[295] */
		0,   0,   0,   0,   0,   0,   0,   0, /* [296]-[303] */
		0,   0,   0,   0,   0,   0,   0,   0, /* [304]-[311] */
		0,   0,   0,   0,   0,   0,   0,   0, /* [312]-[319] */
		0,   0,   0,   0,   0,   0,   0,   0, /* [320]-[327] */
		0,   0,   0,   0,   0,   0,   0,   0, /* [328]-[335] */
		0,   0,   0,   0,   0,   0,   0,   0, /* [336]-[343] */
		0,   0,   0,   0,   0,   0,   0,   0, /* [344]-[351] *//* 2172 */
		0,   0,   0,   0,   0,   0,   0,   0, /* [352]-[359] */
		0,   0,   0,   0,   0,   0,   0,0x01, /* [368]-[367] */ /* Dec/23/2000 */
		0,   0,   0,   0,   0,   0,   0,   0, /* [369]-[375] */
		0,   0,   0,   0,   0,   0,   0,   0, /* [376]-[383] */
		0,   0,   0,   0,   0,   0,   0,   0, /* [384]-[391] */
		0,   0,   0,   0,   0,   0,   0,   0,  /* [392]-[399] */
		0,   0,   0,   0,   0,   0,   0,   0, /*   [400]-  [407] */
		0,	 0,   0,   0,   0,   0,   0,   0, /*   [408]- [415] */
		0,   0,   0,   0,   0,   0,   0,   0, /*  [416]- [423] */
		0,   0,   0,   0,   0,   0,   0,   0, /*  [424]- [431] */
		0,   0,   0,   0,   0,   0,   0,   0, /*  [432]- [439] */
		0,   0,	  0,   0,   0,   0,   0,   0, /*  [440]- [447] */
		0,   0,   0,   0,   0,   0,   0,   0, /*  [448]- [455] */
		0,   0,   0,   0,   0,   0,   0,   0, /*  [456]- [463] */
		0,   0,   0,   0,   0,   0,   0,   0, /*  [464]- [471] */
		0,   0,   0,   0,   0,   0,   0,   0, /*  [472]- [479] */
		0,   0,   0,   0,   0,   0,   0,   0, /*  [480]- [487] */
		0,   0,   0,   0,   0,   0,   0,   0, /*  [488]- [495] */
		0,   0,   0,   0,   0,   0,   0,   0, /*  [496]-[503] */
		0,   0,   0,   0,   0,   0,   0,   0, /* [504]-[511] */
		0,   0,   0,   0,   0,   0,   0,   0, /* [512]-[519] */
		0,   0,   0,   0,   0,   0,   0,   0, /* [520]-[527] */
		0,   0,   0,   0,   0,   0,   0,   0, /* [528]-[535] */
		0,   0,   0,   0,   0,   0,   0,   0, /* [536]-[543] */
		0,   0,   0,   0,   0,   0,   0,   0, /* [544]-[551] */
		0,   0,   0,   0,   0,   0,   0,   0, /* [552]-[559] */
		0,   0,   0,   0,   0,   0,   0,   0, /* [560]-[567] */
		0,   0,   0,   0,   0,   0,   0,   0, /* [568]-[575] */
		0,   0,   0,   0,   0,   0,   0,   0, /* [576]-[583] */
		0   ,0,   0,   0,   0,   0,   0,   0, /* [584]-[591] */
		0,   0,   0,   0,   0,   0,   0,   0  /* [592]-[599] */
    },
    {   /* Prog.2 FLEXIBLE MEMORY ASSIGNMENT, 2172 defaults are zero and must be set by user in P2 to create the files.*/
		{   0L, 0}, /* DEPT */
		{   0L, 0}, /* PLU */
		{   0L, 0}, /* SERVER */
		{   0L, 0}, /* CASHIER */
		{   0L, 0}, /* EJ */
		{   0L, 0}, /* ETK */
		{  30L, 0}, /* ITEM BUFFER */
		{  30L, 0}, /* CONSOL. BUFFER */
		{   0L, 3}, /* GCF, Store/Recall System is default */
		{   0L, 0}, /* COUPON */
		{   0L, 0}, /* CONTROL STRING SIZE */
		{   0L, 0}, /* PROGRAMABLE REPORT */
		{   0L, 0}, /* PPI */
		{ 240L, 0}  /* MLD, V3.3,  LCD_ADR_MAX or MAX_MLDMNEMO_SIZE  */
    },
    {   /* Prog.3 SET FSC */
    {   /* Page #1 Table Address */
        43, 0,       /* Addr.  1 (Recipt Feed) */
        44, 0,       /* Addr.  2 (Journal Feed) */
         2, 1,       /* Addr.  3 (Keyed Dept 1) */
         2, 2,       /* Addr.  4 (Keyed Dept 2) */
         2, 3,       /* Addr.  5 (Keyed Dept 3) */
         2, 4,       /* Addr.  6 (Keyed Dept 4) */
         2, 5,       /* Addr.  7 (Keyed Dept 5) */
         2, 6,       /* Addr.  8 (Keyed Dept 6) */
         2, 7,       /* Addr.  9 (Keyed Dept 7) */
         2, 8,       /* Addr. 10 (Keyed Dept 8) */
         2, 9,       /* Addr. 11 (Keyed Dept 9) */
         2,10,       /* Addr. 12 (Keyed Dept 10) */
         2,41,       /* Addr. 13 (Keyed Dept 41) */
         2,42,       /* Addr. 14 (Keyed Dept 42) */
         2,43,       /* Addr. 15 (Keyed Dept 43) */
         2,44,       /* Addr. 16 (Keyed Dept 44) */
         2,45,       /* Addr. 17 (Keyed Dept 45) */
         2,46,       /* Addr. 18 (Keyed Dept 46) */
         2,47,       /* Addr. 19 (Keyed Dept 47) */
         2,48,       /* Addr. 20 (Keyed Dept 48) */
         2,49,       /* Addr. 21 (Keyed Dept 49) */
         2,50,       /* Addr. 22 (Keyed Dept 50) */
         1,23,       /* Addr. 23 (Keyed PLU 23) */
         1,24,       /* Addr. 24 (Keyed PLU 24) */
         1,25,       /* Addr. 25 (Keyed PLU 25) */
         1,26,       /* Addr. 26 (Keyed PLU 26) */
         1,27,       /* Addr. 27 (Keyed PLU 27) */
         1,28,       /* Addr. 28 (Keyed PLU 28) */
         1,29,       /* Addr. 29 (Keyed PLU 29) */
         1,30,       /* Addr. 30 (Keyed PLU 30) */
         1,31,       /* Addr. 31 (Keyed PLU 31) */
         1,32,       /* Addr. 32 (Keyed PLU 32) */
         1,33,       /* Addr. 33 (Keyed PLU 33) */
         1,34,       /* Addr. 34 (Keyed PLU 34) */
         1,35,       /* Addr. 35 (Keyed PLU 35) */
         1,36,       /* Addr. 36 (Keyed PLU 36) */
         1,37,       /* Addr. 37 (Keyed PLU 37) */
         1,38,       /* Addr. 38 (Keyed PLU 38) */
         1,39,       /* Addr. 39 (Keyed PLU 39) */
         1,40,       /* Addr. 40 (Keyed PLU 40) */
         1,41,       /* Addr. 41 (Keyed PLU 41) */
         1,42,       /* Addr. 42 (Keyed PLU 42) */
         1,43,       /* Addr. 43 (Keyed PLU 43) */
         1,44,       /* Addr. 44 (Keyed PLU 44) */
         1,45,       /* Addr. 45 (Keyed PLU 45) */
         1,46,       /* Addr. 46 (Keyed PLU 46) */
         1,47,       /* Addr. 47 (Keyed PLU 47) */
         1,48,       /* Addr. 48 (Keyed PLU 48) */
         1,49,       /* Addr. 49 (Keyed PLU 49) */
         1,50,       /* Addr. 50 (Keyed PLU 50) */
         1,51,       /* Addr. 51 (Keyed PLU 51) */
         1,52,       /* Addr. 52 (Keyed PLU 52) */
         1,53,       /* Addr. 53 (Keyed PLU 53) */
         1,54,       /* Addr. 54 (Keyed PLU 54) */
         1,55,       /* Addr. 55 (Keyed PLU 55) */
         1,56,       /* Addr. 56 (Keyed PLU 56) */
         1,57,       /* Addr. 57 (Keyed PLU 57) */
        54, 1,       /* Addr. 58 (Direct Menu Shift 1) */
        54, 2,       /* Addr. 59 (Direct Menu Shift 2) */
        54, 3,       /* Addr. 60 (Direct Menu Shift 3) */
        54, 4,       /* Addr. 61 (Direct Menu Shift 4) */
        54, 5,       /* Addr. 62 (Direct Menu Shift 5) */
        53, 1,       /* Addr. 63 (Preset Amount Cash Tender 1) */
        53, 2,       /* Addr. 64 (Preset Amount Cash Tender 2) */
        53, 3,       /* Addr. 65 (Preset Amount Cash Tender 3) */
        53, 4,       /* Addr. 66 (Preset Amount Cash Tender 4) */
         4,11,       /* Addr. 67 (Adjective 3-1) */
        42, 0,       /* Addr. 68 (A/C), Dec/23/2000 */
         4,13,       /* Addr. 69 (Adjective 3-3) */
         4,14,       /* Addr. 70 (Adjective 3-4) */
         4,15,       /* Addr. 71 (Adjective 3-5) */
         4,16,       /* Addr. 72 (Adjective 4-1) */
         4,17,       /* Addr. 73 (Adjective 4-2) */
         4,18,       /* Addr. 74 (Adjective 4-3) */
         4,19,       /* Addr. 75 (Adjective 4-4) */
         4,20,       /* Addr. 76 (Adjective 4-5) */
        55, 0,       /* Addr. 77 (Time In) */
         4, 1,       /* Addr. 78 (Adjective 1-1) */
         4, 2,       /* Addr. 79 (Adjective 1-2) */
         4, 3,       /* Addr. 80 (Adjective 1-3) */
         4, 4,       /* Addr. 81 (Adjective 1-4) */
         4, 5,       /* Addr. 82 (Adjective 1-5) */
         4, 6,       /* Addr. 83 (Adjective 2-1) */
         4, 7,       /* Addr. 84 (Adjective 2-2) */
         4, 8,       /* Addr. 85 (Adjective 2-3) */
         4, 9,       /* Addr. 86 (Adjective 2-4) */
         4,10,       /* Addr. 87 (Adjective 2-5) */
        56, 0,       /* Addr. 88 (Time Out) */
        11, 1,       /* Addr. 89 (Tax Mod 1) */
        11, 2,       /* Addr. 90 (Tax Mod 2) */
        11, 3,       /* Addr. 91 (Tax Mod 3) */
        11, 4,       /* Addr. 92 (Tax Mod 4) */
        11, 5,       /* Addr. 93 (Tax Mod 5) */
        11, 6,       /* Addr. 94 (Tax Mod 6) */
        11, 7,       /* Addr. 95 (Tax Mod 7) */
        11, 8,       /* Addr. 96 (Tax Mod 8) */
        11, 9,       /* Addr. 97 (Tax Mod 9) */
        11,10,       /* Addr. 98 (Tax Exempt 1) */
        11,11,       /* Addr. 99 (Tax Exempt 2) */
         3, 1,       /* Addr.100 (Print Mod 1) */
         3, 2,       /* Addr.101 (Print Mod 2) */
         3, 3,       /* Addr.102 (Print Mod 3) */
         3, 4,       /* Addr.103 (Print Mod 4) */
         3, 5,       /* Addr.104 (Print Mod 5) */
         3, 6,       /* Addr.105 (Print Mod 6) */
         3, 7,       /* Addr.106 (Print Mod 7) */
         3, 8,       /* Addr.107 (Print Mod 8) */
        39, 1,       /* Addr.108 (Charge Tips, V3.3) */
        24, 0,       /* Addr.109 (Tips Paid Out) */
        25, 0,       /* Addr.110 (Declared Tips) */
        12, 0,       /* Addr.111 (Sign-in) */
        15, 0,       /* Addr.112 (New Check) */
        17, 0,       /* Addr.113 (Guest Check #) */
         7, 8,       /* Addr.114 (Toatl 8) */
         7, 4,       /* Addr.115 (Toatl 4) */
        32, 0,       /* Addr.116 (E/C) */
        23, 0,       /* Addr.117 (Receipt/No Receipt) */
        30, 0,       /* Addr.118 (Cancel/Abort) */
        58, 0,       /* Addr.119 (Coupon Number) */
        37, 2,       /* Addr.120 (Print on Demand) */
        13, 0,       /* Addr.121 (B Key) */
        16, 0,       /* Addr.122 (PB) */
        19, 0,       /* Addr.123 (No. of Person) */
         7, 7,       /* Addr.124 (Total 7) */
         7, 3,       /* Addr.125 (Total 3) */
        34, 0,       /* Addr.126 (QTY) */
        31, 0,       /* Addr.127 (Item Void) */
        33, 0,       /* Addr.128 (Transaction Void) */
         6, 1,       /* Addr.129 (F/C 1) */
         6, 2,       /* Addr.130 (F/C 2) */
        28, 0,       /* Addr.131 (R/A) */
         0, 0,       /* Addr.132 (Server #) */
         8, 1,       /* Addr.133 (Add Check 1) */
        20, 0,       /* Addr.134 (Table #) */
         7, 6,       /* Addr.135 (Total 6) */
         7, 2,       /* Addr.136 (Total 2) */
         5, 7,       /* Addr.137 (Tender 7) */
         5, 8,       /* Addr.138 (Tender 8) */
        27, 0,       /* Addr.139 (P/O) */
        22, 0,       /* Addr.140 (Feed/Release) */
         8, 2,       /* Addr.141 (Add Check 2) */
        21, 0,       /* Addr.142 (Line #) */
         7, 5,       /* Addr.143 (Total 5) */
         7, 1,       /* Addr.144 (Total 1) */
         5, 5,       /* Addr.145 (Tender 5) */
         5, 6,       /* Addr.146 (Tender 6) */
        26, 0,       /* Addr.147 (No Sale) */
        29, 0,       /* Addr.148 (Change Computation) */
         8, 3,       /* Addr.149 (Add Check 3) */
        18, 0,       /* Addr.150 (Guest Check Transfer) */
         9, 2,       /* Addr.151 (Mod Item Discount) */
         9, 1,       /* Addr.152 (Item Discount) */
         5, 3,       /* Addr.153 (Tender 3) */
         5, 4,       /* Addr.154 (Tender 4) */
        36, 0,       /* Addr.155 (#/Type) */
        35, 0,       /* Addr.156 (Menu Shift) */
        52, 0,       /* Addr.157 (EJ Reverse) */
        10, 2,       /* Addr.158 (Regular Discount 2) */
        10, 1,       /* Addr.159 (Regular Discount 1) */
        40, 0,       /* Addr.160 (PLU No.) */
        45, 0,       /* Addr.161 (Decimal Point) */
        41, 0,       /* Addr.162 (00) */
         5, 1,       /* Addr.163 (Tender 1) */
         5, 2,       /* Addr.164 (Tender 2) */
        42, 0,       /* Addr.165 (A/C) */
		0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0

    },
    {       /* Page #2 Table Address */
        43, 0,       /* Addr.  1 (Recipt Feed) */
        44, 0,       /* Addr.  2 (Journal Feed) */
         2, 1,       /* Addr.  3 (Keyed Dept 1) */
         2, 2,       /* Addr.  4 (Keyed Dept 2) */
         2, 3,       /* Addr.  5 (Keyed Dept 3) */
         2, 4,       /* Addr.  6 (Keyed Dept 4) */
         2, 5,       /* Addr.  7 (Keyed Dept 5) */
         2, 6,       /* Addr.  8 (Keyed Dept 6) */
         2, 7,       /* Addr.  9 (Keyed Dept 7) */
         2, 8,       /* Addr. 10 (Keyed Dept 8) */
         2, 9,       /* Addr. 11 (Keyed Dept 9) */
         2,10,       /* Addr. 12 (Keyed Dept 10) */
         2,41,       /* Addr. 13 (Keyed Dept 41) */
         2,42,       /* Addr. 14 (Keyed Dept 42) */
         2,43,       /* Addr. 15 (Keyed Dept 43) */
         2,44,       /* Addr. 16 (Keyed Dept 44) */
         2,45,       /* Addr. 17 (Keyed Dept 45) */
         2,46,       /* Addr. 18 (Keyed Dept 46) */
         2,47,       /* Addr. 19 (Keyed Dept 47) */
         2,48,       /* Addr. 20 (Keyed Dept 48) */
         2,49,       /* Addr. 21 (Keyed Dept 49) */
         2,50,       /* Addr. 22 (Keyed Dept 50) */
         1,23,       /* Addr. 23 (Keyed PLU 23) */
         1,24,       /* Addr. 24 (Keyed PLU 24) */
         1,25,       /* Addr. 25 (Keyed PLU 25) */
         1,26,       /* Addr. 26 (Keyed PLU 26) */
         1,27,       /* Addr. 27 (Keyed PLU 27) */
         1,28,       /* Addr. 28 (Keyed PLU 28) */
         1,29,       /* Addr. 29 (Keyed PLU 29) */
         1,30,       /* Addr. 30 (Keyed PLU 30) */
         1,31,       /* Addr. 31 (Keyed PLU 31) */
         1,32,       /* Addr. 32 (Keyed PLU 32) */
         1,33,       /* Addr. 33 (Keyed PLU 33) */
         1,34,       /* Addr. 34 (Keyed PLU 34) */
         1,35,       /* Addr. 35 (Keyed PLU 35) */
         1,36,       /* Addr. 36 (Keyed PLU 36) */
         1,37,       /* Addr. 37 (Keyed PLU 37) */
         1,38,       /* Addr. 38 (Keyed PLU 38) */
         1,39,       /* Addr. 39 (Keyed PLU 39) */
         1,40,       /* Addr. 40 (Keyed PLU 40) */
         1,41,       /* Addr. 41 (Keyed PLU 41) */
         1,42,       /* Addr. 42 (Keyed PLU 42) */
         1,43,       /* Addr. 43 (Keyed PLU 43) */
         1,44,       /* Addr. 44 (Keyed PLU 44) */
         1,45,       /* Addr. 45 (Keyed PLU 45) */
         1,46,       /* Addr. 46 (Keyed PLU 46) */
         1,47,       /* Addr. 47 (Keyed PLU 47) */
         1,48,       /* Addr. 48 (Keyed PLU 48) */
         1,49,       /* Addr. 49 (Keyed PLU 49) */
         1,50,       /* Addr. 50 (Keyed PLU 50) */
         1,51,       /* Addr. 51 (Keyed PLU 51) */
         1,52,       /* Addr. 52 (Keyed PLU 52) */
         1,53,       /* Addr. 53 (Keyed PLU 53) */
         1,54,       /* Addr. 54 (Keyed PLU 54) */
         1,55,       /* Addr. 55 (Keyed PLU 55) */
         1,56,       /* Addr. 56 (Keyed PLU 56) */
         1,57,       /* Addr. 57 (Keyed PLU 57) */
        54, 1,       /* Addr. 58 (Direct Menu Shift 1) */
        54, 2,       /* Addr. 59 (Direct Menu Shift 2) */
        54, 3,       /* Addr. 60 (Direct Menu Shift 3) */
        54, 4,       /* Addr. 61 (Direct Menu Shift 4) */
        54, 5,       /* Addr. 62 (Direct Menu Shift 5) */
        53, 1,       /* Addr. 63 (Preset Amount Cash Tender 1) */
        53, 2,       /* Addr. 64 (Preset Amount Cash Tender 2) */
        53, 3,       /* Addr. 65 (Preset Amount Cash Tender 3) */
        53, 4,       /* Addr. 66 (Preset Amount Cash Tender 4) */
         4,11,       /* Addr. 67 (Adjective 3-1) */
         4,12,       /* Addr. 68 (Adjective 3-2) */
         4,13,       /* Addr. 69 (Adjective 3-3) */
         4,14,       /* Addr. 70 (Adjective 3-4) */
         4,15,       /* Addr. 71 (Adjective 3-5) */
         4,16,       /* Addr. 72 (Adjective 4-1) */
         4,17,       /* Addr. 73 (Adjective 4-2) */
         4,18,       /* Addr. 74 (Adjective 4-3) */
         4,19,       /* Addr. 75 (Adjective 4-4) */
         4,20,       /* Addr. 76 (Adjective 4-5) */
        55, 0,       /* Addr. 77 (Time In) */
         4, 1,       /* Addr. 78 (Adjective 1-1) */
         4, 2,       /* Addr. 79 (Adjective 1-2) */
         4, 3,       /* Addr. 80 (Adjective 1-3) */
         4, 4,       /* Addr. 81 (Adjective 1-4) */
         4, 5,       /* Addr. 82 (Adjective 1-5) */
         4, 6,       /* Addr. 83 (Adjective 2-1) */
         4, 7,       /* Addr. 84 (Adjective 2-2) */
         4, 8,       /* Addr. 85 (Adjective 2-3) */
         4, 9,       /* Addr. 86 (Adjective 2-4) */
         4,10,       /* Addr. 87 (Adjective 2-5) */
        56, 0,       /* Addr. 88 (Time Out) */
        11, 1,       /* Addr. 89 (Tax Mod 1) */
        11, 2,       /* Addr. 90 (Tax Mod 2) */
        11, 3,       /* Addr. 91 (Tax Mod 3) */
        11, 4,       /* Addr. 92 (Tax Mod 4) */
        11, 5,       /* Addr. 93 (Tax Mod 5) */
        11, 6,       /* Addr. 94 (Tax Mod 6) */
        11, 7,       /* Addr. 95 (Tax Mod 7) */
        11, 8,       /* Addr. 96 (Tax Mod 8) */
        11, 9,       /* Addr. 97 (Tax Mod 9) */
        11,10,       /* Addr. 98 (Tax Exempt 1) */
        11,11,       /* Addr. 99 (Tax Exempt 2) */
         3, 1,       /* Addr.100 (Print Mod 1) */
         3, 2,       /* Addr.101 (Print Mod 2) */
         3, 3,       /* Addr.102 (Print Mod 3) */
         3, 4,       /* Addr.103 (Print Mod 4) */
         3, 5,       /* Addr.104 (Print Mod 5) */
         3, 6,       /* Addr.105 (Print Mod 6) */
         3, 7,       /* Addr.106 (Print Mod 7) */
         3, 8,       /* Addr.107 (Print Mod 8) */
        39, 1,       /* Addr.108 (Charge Tips, V3.3) */
        24, 0,       /* Addr.109 (Tips Paid Out) */
        25, 0,       /* Addr.110 (Declared Tips) */
        12, 0,       /* Addr.111 (Sign-in) */
        15, 0,       /* Addr.112 (New Check) */
        17, 0,       /* Addr.113 (Guest Check #) */
         7, 8,       /* Addr.114 (Toatl 8) */
         7, 4,       /* Addr.115 (Toatl 4) */
        32, 0,       /* Addr.116 (E/C) */
        23, 0,       /* Addr.117 (Receipt/No Receipt) */
        30, 0,       /* Addr.118 (Cancel/Abort) */
        58, 0,       /* Addr.119 (Coupon Number) */
        37, 2,       /* Addr.120 (Print on Demand) */
        13, 0,       /* Addr.121 (B Key) */
        16, 0,       /* Addr.122 (PB) */
        19, 0,       /* Addr.123 (No. of Person) */
         7, 7,       /* Addr.124 (Total 7) */
         7, 3,       /* Addr.125 (Total 3) */
        34, 0,       /* Addr.126 (QTY) */
        31, 0,       /* Addr.127 (Item Void) */
        33, 0,       /* Addr.128 (Transaction Void) */
         6, 1,       /* Addr.129 (F/C 1) */
         6, 2,       /* Addr.130 (F/C 2) */
        28, 0,       /* Addr.131 (R/A) */
         0, 0,       /* Addr.132 (Server #) */
         8, 1,       /* Addr.133 (Add Check 1) */
        20, 0,       /* Addr.134 (Table #) */
         7, 6,       /* Addr.135 (Total 6) */
         7, 2,       /* Addr.136 (Total 2) */
         5, 7,       /* Addr.137 (Tender 7) */
         5, 8,       /* Addr.138 (Tender 8) */
        27, 0,       /* Addr.139 (P/O) */
        22, 0,       /* Addr.140 (Feed/Release) */
         8, 2,       /* Addr.141 (Add Check 2) */
        21, 0,       /* Addr.142 (Line #) */
         7, 5,       /* Addr.143 (Total 5) */
         7, 1,       /* Addr.144 (Total 1) */
         5, 5,       /* Addr.145 (Tender 5) */
         5, 6,       /* Addr.146 (Tender 6) */
        26, 0,       /* Addr.147 (No Sale) */
        29, 0,       /* Addr.148 (Change Computation) */
         8, 3,       /* Addr.149 (Add Check 3) */
        18, 0,       /* Addr.150 (Guest Check Transfer) */
         9, 2,       /* Addr.151 (Mod Item Discount) */
         9, 1,       /* Addr.152 (Item Discount) */
         5, 3,       /* Addr.153 (Tender 3) */
         5, 4,       /* Addr.154 (Tender 4) */
        36, 0,       /* Addr.155 (#/Type) */
        35, 0,       /* Addr.156 (Menu Shift) */
        52, 0,       /* Addr.157 (EJ Reverse) */
        10, 2,       /* Addr.158 (Regular Discount 2) */
        10, 1,       /* Addr.159 (Regular Discount 1) */
        40, 0,       /* Addr.160 (PLU No.) */
        45, 0,       /* Addr.161 (Decimal Point) */
        41, 0,       /* Addr.162 (00) */
         5, 1,       /* Addr.163 (Tender 1) */
         5, 2,       /* Addr.164 (Tender 2) */
        42, 0,        /* Addr.165 (A/C) */
		0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
    },
    {       /* Page #3 Table Address */
        43, 0,       /* Addr.  1 (Recipt Feed) */
        44, 0,       /* Addr.  2 (Journal Feed) */
         2, 1,       /* Addr.  3 (Keyed Dept 1) */
         2, 2,       /* Addr.  4 (Keyed Dept 2) */
         2, 3,       /* Addr.  5 (Keyed Dept 3) */
         2, 4,       /* Addr.  6 (Keyed Dept 4) */
         2, 5,       /* Addr.  7 (Keyed Dept 5) */
         2, 6,       /* Addr.  8 (Keyed Dept 6) */
         2, 7,       /* Addr.  9 (Keyed Dept 7) */
         2, 8,       /* Addr. 10 (Keyed Dept 8) */
         2, 9,       /* Addr. 11 (Keyed Dept 9) */
         2,10,       /* Addr. 12 (Keyed Dept 10) */
         2,41,       /* Addr. 13 (Keyed Dept 41) */
         2,42,       /* Addr. 14 (Keyed Dept 42) */
         2,43,       /* Addr. 15 (Keyed Dept 43) */
         2,44,       /* Addr. 16 (Keyed Dept 44) */
         2,45,       /* Addr. 17 (Keyed Dept 45) */
         2,46,       /* Addr. 18 (Keyed Dept 46) */
         2,47,       /* Addr. 19 (Keyed Dept 47) */
         2,48,       /* Addr. 20 (Keyed Dept 48) */
         2,49,       /* Addr. 21 (Keyed Dept 49) */
         2,50,       /* Addr. 22 (Keyed Dept 50) */
         1,23,       /* Addr. 23 (Keyed PLU 23) */
         1,24,       /* Addr. 24 (Keyed PLU 24) */
         1,25,       /* Addr. 25 (Keyed PLU 25) */
         1,26,       /* Addr. 26 (Keyed PLU 26) */
         1,27,       /* Addr. 27 (Keyed PLU 27) */
         1,28,       /* Addr. 28 (Keyed PLU 28) */
         1,29,       /* Addr. 29 (Keyed PLU 29) */
         1,30,       /* Addr. 30 (Keyed PLU 30) */
         1,31,       /* Addr. 31 (Keyed PLU 31) */
         1,32,       /* Addr. 32 (Keyed PLU 32) */
         1,33,       /* Addr. 33 (Keyed PLU 33) */
         1,34,       /* Addr. 34 (Keyed PLU 34) */
         1,35,       /* Addr. 35 (Keyed PLU 35) */
         1,36,       /* Addr. 36 (Keyed PLU 36) */
         1,37,       /* Addr. 37 (Keyed PLU 37) */
         1,38,       /* Addr. 38 (Keyed PLU 38) */
         1,39,       /* Addr. 39 (Keyed PLU 39) */
         1,40,       /* Addr. 40 (Keyed PLU 40) */
         1,41,       /* Addr. 41 (Keyed PLU 41) */
         1,42,       /* Addr. 42 (Keyed PLU 42) */
         1,43,       /* Addr. 43 (Keyed PLU 43) */
         1,44,       /* Addr. 44 (Keyed PLU 44) */
         1,45,       /* Addr. 45 (Keyed PLU 45) */
         1,46,       /* Addr. 46 (Keyed PLU 46) */
         1,47,       /* Addr. 47 (Keyed PLU 47) */
         1,48,       /* Addr. 48 (Keyed PLU 48) */
         1,49,       /* Addr. 49 (Keyed PLU 49) */
         1,50,       /* Addr. 50 (Keyed PLU 50) */
         1,51,       /* Addr. 51 (Keyed PLU 51) */
         1,52,       /* Addr. 52 (Keyed PLU 52) */
         1,53,       /* Addr. 53 (Keyed PLU 53) */
         1,54,       /* Addr. 54 (Keyed PLU 54) */
         1,55,       /* Addr. 55 (Keyed PLU 55) */
         1,56,       /* Addr. 56 (Keyed PLU 56) */
         1,57,       /* Addr. 57 (Keyed PLU 57) */
        54, 1,       /* Addr. 58 (Direct Menu Shift 1) */
        54, 2,       /* Addr. 59 (Direct Menu Shift 2) */
        54, 3,       /* Addr. 60 (Direct Menu Shift 3) */
        54, 4,       /* Addr. 61 (Direct Menu Shift 4) */
        54, 5,       /* Addr. 62 (Direct Menu Shift 5) */
        53, 1,       /* Addr. 63 (Preset Amount Cash Tender 1) */
        53, 2,       /* Addr. 64 (Preset Amount Cash Tender 2) */
        53, 3,       /* Addr. 65 (Preset Amount Cash Tender 3) */
        53, 4,       /* Addr. 66 (Preset Amount Cash Tender 4) */
         4,11,       /* Addr. 67 (Adjective 3-1) */
         4,12,       /* Addr. 68 (Adjective 3-2) */
         4,13,       /* Addr. 69 (Adjective 3-3) */
         4,14,       /* Addr. 70 (Adjective 3-4) */
         4,15,       /* Addr. 71 (Adjective 3-5) */
         4,16,       /* Addr. 72 (Adjective 4-1) */
         4,17,       /* Addr. 73 (Adjective 4-2) */
         4,18,       /* Addr. 74 (Adjective 4-3) */
         4,19,       /* Addr. 75 (Adjective 4-4) */
         4,20,       /* Addr. 76 (Adjective 4-5) */
        55, 0,       /* Addr. 77 (Time In) */
         4, 1,       /* Addr. 78 (Adjective 1-1) */
         4, 2,       /* Addr. 79 (Adjective 1-2) */
         4, 3,       /* Addr. 80 (Adjective 1-3) */
         4, 4,       /* Addr. 81 (Adjective 1-4) */
         4, 5,       /* Addr. 82 (Adjective 1-5) */
         4, 6,       /* Addr. 83 (Adjective 2-1) */
         4, 7,       /* Addr. 84 (Adjective 2-2) */
         4, 8,       /* Addr. 85 (Adjective 2-3) */
         4, 9,       /* Addr. 86 (Adjective 2-4) */
         4,10,       /* Addr. 87 (Adjective 2-5) */
        56, 0,       /* Addr. 88 (Time Out) */
        11, 1,       /* Addr. 89 (Tax Mod 1) */
        11, 2,       /* Addr. 90 (Tax Mod 2) */
        11, 3,       /* Addr. 91 (Tax Mod 3) */
        11, 4,       /* Addr. 92 (Tax Mod 4) */
        11, 5,       /* Addr. 93 (Tax Mod 5) */
        11, 6,       /* Addr. 94 (Tax Mod 6) */
        11, 7,       /* Addr. 95 (Tax Mod 7) */
        11, 8,       /* Addr. 96 (Tax Mod 8) */
        11, 9,       /* Addr. 97 (Tax Mod 9) */
        11,10,       /* Addr. 98 (Tax Exempt 1) */
        11,11,       /* Addr. 99 (Tax Exempt 2) */
         3, 1,       /* Addr.100 (Print Mod 1) */
         3, 2,       /* Addr.101 (Print Mod 2) */
         3, 3,       /* Addr.102 (Print Mod 3) */
         3, 4,       /* Addr.103 (Print Mod 4) */
         3, 5,       /* Addr.104 (Print Mod 5) */
         3, 6,       /* Addr.105 (Print Mod 6) */
         3, 7,       /* Addr.106 (Print Mod 7) */
         3, 8,       /* Addr.107 (Print Mod 8) */
        39, 1,       /* Addr.108 (Charge Tips, V3.3) */
        24, 0,       /* Addr.109 (Tips Paid Out) */
        25, 0,       /* Addr.110 (Declared Tips) */
        12, 0,       /* Addr.111 (Sign-in) */
        15, 0,       /* Addr.112 (New Check) */
        17, 0,       /* Addr.113 (Guest Check #) */
         7, 8,       /* Addr.114 (Toatl 8) */
         7, 4,       /* Addr.115 (Toatl 4) */
        32, 0,       /* Addr.116 (E/C) */
        23, 0,       /* Addr.117 (Receipt/No Receipt) */
        30, 0,       /* Addr.118 (Cancel/Abort) */
        58, 0,       /* Addr.119 (Coupon Number) */
        37, 2,       /* Addr.120 (Print on Demand) */
        13, 0,       /* Addr.121 (B Key) */
        16, 0,       /* Addr.122 (PB) */
        19, 0,       /* Addr.123 (No. of Person) */
         7, 7,       /* Addr.124 (Total 7) */
         7, 3,       /* Addr.125 (Total 3) */
        34, 0,       /* Addr.126 (QTY) */
        31, 0,       /* Addr.127 (Item Void) */
        33, 0,       /* Addr.128 (Transaction Void) */
         6, 1,       /* Addr.129 (F/C 1) */
         6, 2,       /* Addr.130 (F/C 2) */
        28, 0,       /* Addr.131 (R/A) */
         0, 0,       /* Addr.132 (Server #) */
         8, 1,       /* Addr.133 (Add Check 1) */
        20, 0,       /* Addr.134 (Table #) */
         7, 6,       /* Addr.135 (Total 6) */
         7, 2,       /* Addr.136 (Total 2) */
         5, 7,       /* Addr.137 (Tender 7) */
         5, 8,       /* Addr.138 (Tender 8) */
        27, 0,       /* Addr.139 (P/O) */
        22, 0,       /* Addr.140 (Feed/Release) */
         8, 2,       /* Addr.141 (Add Check 2) */
        21, 0,       /* Addr.142 (Line #) */
         7, 5,       /* Addr.143 (Total 5) */
         7, 1,       /* Addr.144 (Total 1) */
         5, 5,       /* Addr.145 (Tender 5) */
         5, 6,       /* Addr.146 (Tender 6) */
        26, 0,       /* Addr.147 (No Sale) */
        29, 0,       /* Addr.148 (Change Computation) */
         8, 3,       /* Addr.149 (Add Check 3) */
        18, 0,       /* Addr.150 (Guest Check Transfer) */
         9, 2,       /* Addr.151 (Mod Item Discount) */
         9, 1,       /* Addr.152 (Item Discount) */
         5, 3,       /* Addr.153 (Tender 3) */
         5, 4,       /* Addr.154 (Tender 4) */
        36, 0,       /* Addr.155 (#/Type) */
        35, 0,       /* Addr.156 (Menu Shift) */
        52, 0,       /* Addr.157 (EJ Reverse) */
        10, 2,       /* Addr.158 (Regular Discount 2) */
        10, 1,       /* Addr.159 (Regular Discount 1) */
        40, 0,       /* Addr.160 (PLU No.) */
        45, 0,       /* Addr.161 (Decimal Point) */
        41, 0,       /* Addr.162 (00) */
         5, 1,       /* Addr.163 (Tender 1) */
         5, 2,       /* Addr.164 (Tender 2) */
        42, 0,        /* Addr.165 (A/C) */
				0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0
    },
    {       /* Page #4 Table Address */
        43, 0,       /* Addr.  1 (Recipt Feed) */
        44, 0,       /* Addr.  2 (Journal Feed) */
         2, 1,       /* Addr.  3 (Keyed Dept 1) */
         2, 2,       /* Addr.  4 (Keyed Dept 2) */
         2, 3,       /* Addr.  5 (Keyed Dept 3) */
         2, 4,       /* Addr.  6 (Keyed Dept 4) */
         2, 5,       /* Addr.  7 (Keyed Dept 5) */
         2, 6,       /* Addr.  8 (Keyed Dept 6) */
         2, 7,       /* Addr.  9 (Keyed Dept 7) */
         2, 8,       /* Addr. 10 (Keyed Dept 8) */
         2, 9,       /* Addr. 11 (Keyed Dept 9) */
         2,10,       /* Addr. 12 (Keyed Dept 10) */
         2,41,       /* Addr. 13 (Keyed Dept 41) */
         2,42,       /* Addr. 14 (Keyed Dept 42) */
         2,43,       /* Addr. 15 (Keyed Dept 43) */
         2,44,       /* Addr. 16 (Keyed Dept 44) */
         2,45,       /* Addr. 17 (Keyed Dept 45) */
         2,46,       /* Addr. 18 (Keyed Dept 46) */
         2,47,       /* Addr. 19 (Keyed Dept 47) */
         2,48,       /* Addr. 20 (Keyed Dept 48) */
         2,49,       /* Addr. 21 (Keyed Dept 49) */
         2,50,       /* Addr. 22 (Keyed Dept 50) */
         1,23,       /* Addr. 23 (Keyed PLU 23) */
         1,24,       /* Addr. 24 (Keyed PLU 24) */
         1,25,       /* Addr. 25 (Keyed PLU 25) */
         1,26,       /* Addr. 26 (Keyed PLU 26) */
         1,27,       /* Addr. 27 (Keyed PLU 27) */
         1,28,       /* Addr. 28 (Keyed PLU 28) */
         1,29,       /* Addr. 29 (Keyed PLU 29) */
         1,30,       /* Addr. 30 (Keyed PLU 30) */
         1,31,       /* Addr. 31 (Keyed PLU 31) */
         1,32,       /* Addr. 32 (Keyed PLU 32) */
         1,33,       /* Addr. 33 (Keyed PLU 33) */
         1,34,       /* Addr. 34 (Keyed PLU 34) */
         1,35,       /* Addr. 35 (Keyed PLU 35) */
         1,36,       /* Addr. 36 (Keyed PLU 36) */
         1,37,       /* Addr. 37 (Keyed PLU 37) */
         1,38,       /* Addr. 38 (Keyed PLU 38) */
         1,39,       /* Addr. 39 (Keyed PLU 39) */
         1,40,       /* Addr. 40 (Keyed PLU 40) */
         1,41,       /* Addr. 41 (Keyed PLU 41) */
         1,42,       /* Addr. 42 (Keyed PLU 42) */
         1,43,       /* Addr. 43 (Keyed PLU 43) */
         1,44,       /* Addr. 44 (Keyed PLU 44) */
         1,45,       /* Addr. 45 (Keyed PLU 45) */
         1,46,       /* Addr. 46 (Keyed PLU 46) */
         1,47,       /* Addr. 47 (Keyed PLU 47) */
         1,48,       /* Addr. 48 (Keyed PLU 48) */
         1,49,       /* Addr. 49 (Keyed PLU 49) */
         1,50,       /* Addr. 50 (Keyed PLU 50) */
         1,51,       /* Addr. 51 (Keyed PLU 51) */
         1,52,       /* Addr. 52 (Keyed PLU 52) */
         1,53,       /* Addr. 53 (Keyed PLU 53) */
         1,54,       /* Addr. 54 (Keyed PLU 54) */
         1,55,       /* Addr. 55 (Keyed PLU 55) */
         1,56,       /* Addr. 56 (Keyed PLU 56) */
         1,57,       /* Addr. 57 (Keyed PLU 57) */
        54, 1,       /* Addr. 58 (Direct Menu Shift 1) */
        54, 2,       /* Addr. 59 (Direct Menu Shift 2) */
        54, 3,       /* Addr. 60 (Direct Menu Shift 3) */
        54, 4,       /* Addr. 61 (Direct Menu Shift 4) */
        54, 5,       /* Addr. 62 (Direct Menu Shift 5) */
        53, 1,       /* Addr. 63 (Preset Amount Cash Tender 1) */
        53, 2,       /* Addr. 64 (Preset Amount Cash Tender 2) */
        53, 3,       /* Addr. 65 (Preset Amount Cash Tender 3) */
        53, 4,       /* Addr. 66 (Preset Amount Cash Tender 4) */
         4,11,       /* Addr. 67 (Adjective 3-1) */
         4,12,       /* Addr. 68 (Adjective 3-2) */
         4,13,       /* Addr. 69 (Adjective 3-3) */
         4,14,       /* Addr. 70 (Adjective 3-4) */
         4,15,       /* Addr. 71 (Adjective 3-5) */
         4,16,       /* Addr. 72 (Adjective 4-1) */
         4,17,       /* Addr. 73 (Adjective 4-2) */
         4,18,       /* Addr. 74 (Adjective 4-3) */
         4,19,       /* Addr. 75 (Adjective 4-4) */
         4,20,       /* Addr. 76 (Adjective 4-5) */
        55, 0,       /* Addr. 77 (Time In) */
         4, 1,       /* Addr. 78 (Adjective 1-1) */
         4, 2,       /* Addr. 79 (Adjective 1-2) */
         4, 3,       /* Addr. 80 (Adjective 1-3) */
         4, 4,       /* Addr. 81 (Adjective 1-4) */
         4, 5,       /* Addr. 82 (Adjective 1-5) */
         4, 6,       /* Addr. 83 (Adjective 2-1) */
         4, 7,       /* Addr. 84 (Adjective 2-2) */
         4, 8,       /* Addr. 85 (Adjective 2-3) */
         4, 9,       /* Addr. 86 (Adjective 2-4) */
         4,10,       /* Addr. 87 (Adjective 2-5) */
        56, 0,       /* Addr. 88 (Time Out) */
        11, 1,       /* Addr. 89 (Tax Mod 1) */
        11, 2,       /* Addr. 90 (Tax Mod 2) */
        11, 3,       /* Addr. 91 (Tax Mod 3) */
        11, 4,       /* Addr. 92 (Tax Mod 4) */
        11, 5,       /* Addr. 93 (Tax Mod 5) */
        11, 6,       /* Addr. 94 (Tax Mod 6) */
        11, 7,       /* Addr. 95 (Tax Mod 7) */
        11, 8,       /* Addr. 96 (Tax Mod 8) */
        11, 9,       /* Addr. 97 (Tax Mod 9) */
        11,10,       /* Addr. 98 (Tax Exempt 1) */
        11,11,       /* Addr. 99 (Tax Exempt 2) */
         3, 1,       /* Addr.100 (Print Mod 1) */
         3, 2,       /* Addr.101 (Print Mod 2) */
         3, 3,       /* Addr.102 (Print Mod 3) */
         3, 4,       /* Addr.103 (Print Mod 4) */
         3, 5,       /* Addr.104 (Print Mod 5) */
         3, 6,       /* Addr.105 (Print Mod 6) */
         3, 7,       /* Addr.106 (Print Mod 7) */
         3, 8,       /* Addr.107 (Print Mod 8) */
        39, 1,       /* Addr.108 (Charge Tips, V3.3) */
        24, 0,       /* Addr.109 (Tips Paid Out) */
        25, 0,       /* Addr.110 (Declared Tips) */
        12, 0,       /* Addr.111 (Sign-in) */
        15, 0,       /* Addr.112 (New Check) */
        17, 0,       /* Addr.113 (Guest Check #) */
         7, 8,       /* Addr.114 (Toatl 8) */
         7, 4,       /* Addr.115 (Toatl 4) */
        32, 0,       /* Addr.116 (E/C) */
        23, 0,       /* Addr.117 (Receipt/No Receipt) */
        30, 0,       /* Addr.118 (Cancel/Abort) */
        58, 0,       /* Addr.119 (Coupon Number) */
        37, 2,       /* Addr.120 (Print on Demand) */
        13, 0,       /* Addr.121 (B Key) */
        16, 0,       /* Addr.122 (PB) */
        19, 0,       /* Addr.123 (No. of Person) */
         7, 7,       /* Addr.124 (Total 7) */
         7, 3,       /* Addr.125 (Total 3) */
        34, 0,       /* Addr.126 (QTY) */
        31, 0,       /* Addr.127 (Item Void) */
        33, 0,       /* Addr.128 (Transaction Void) */
         6, 1,       /* Addr.129 (F/C 1) */
         6, 2,       /* Addr.130 (F/C 2) */
        28, 0,       /* Addr.131 (R/A) */
         0, 0,       /* Addr.132 (Server #) */
         8, 1,       /* Addr.133 (Add Check 1) */
        20, 0,       /* Addr.134 (Table #) */
         7, 6,       /* Addr.135 (Total 6) */
         7, 2,       /* Addr.136 (Total 2) */
         5, 7,       /* Addr.137 (Tender 7) */
         5, 8,       /* Addr.138 (Tender 8) */
        27, 0,       /* Addr.139 (P/O) */
        22, 0,       /* Addr.140 (Feed/Release) */
         8, 2,       /* Addr.141 (Add Check 2) */
        21, 0,       /* Addr.142 (Line #) */
         7, 5,       /* Addr.143 (Total 5) */
         7, 1,       /* Addr.144 (Total 1) */
         5, 5,       /* Addr.145 (Tender 5) */
         5, 6,       /* Addr.146 (Tender 6) */
        26, 0,       /* Addr.147 (No Sale) */
        29, 0,       /* Addr.148 (Change Computation) */
         8, 3,       /* Addr.149 (Add Check 3) */
        18, 0,       /* Addr.150 (Guest Check Transfer) */
         9, 2,       /* Addr.151 (Mod Item Discount) */
         9, 1,       /* Addr.152 (Item Discount) */
         5, 3,       /* Addr.153 (Tender 3) */
         5, 4,       /* Addr.154 (Tender 4) */
        36, 0,       /* Addr.155 (#/Type) */
        35, 0,       /* Addr.156 (Menu Shift) */
        52, 0,       /* Addr.157 (EJ Reverse) */
        10, 2,       /* Addr.158 (Regular Discount 2) */
        10, 1,       /* Addr.159 (Regular Discount 1) */
        40, 0,       /* Addr.160 (PLU No.) */
        45, 0,       /* Addr.161 (Decimal Point) */
        41, 0,       /* Addr.162 (00) */
         5, 1,       /* Addr.163 (Tender 1) */
         5, 2,       /* Addr.164 (Tender 2) */
        42, 0,        /* Addr.165 (A/C) */
				0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0
    },
    {       /* Page #5 Table Address */
        43, 0,       /* Addr.  1 (Recipt Feed) */
        44, 0,       /* Addr.  2 (Journal Feed) */
         2, 1,       /* Addr.  3 (Keyed Dept 1) */
         2, 2,       /* Addr.  4 (Keyed Dept 2) */
         2, 3,       /* Addr.  5 (Keyed Dept 3) */
         2, 4,       /* Addr.  6 (Keyed Dept 4) */
         2, 5,       /* Addr.  7 (Keyed Dept 5) */
         2, 6,       /* Addr.  8 (Keyed Dept 6) */
         2, 7,       /* Addr.  9 (Keyed Dept 7) */
         2, 8,       /* Addr. 10 (Keyed Dept 8) */
         2, 9,       /* Addr. 11 (Keyed Dept 9) */
         2,10,       /* Addr. 12 (Keyed Dept 10) */
         2,41,       /* Addr. 13 (Keyed Dept 41) */
         2,42,       /* Addr. 14 (Keyed Dept 42) */
         2,43,       /* Addr. 15 (Keyed Dept 43) */
         2,44,       /* Addr. 16 (Keyed Dept 44) */
         2,45,       /* Addr. 17 (Keyed Dept 45) */
         2,46,       /* Addr. 18 (Keyed Dept 46) */
         2,47,       /* Addr. 19 (Keyed Dept 47) */
         2,48,       /* Addr. 20 (Keyed Dept 48) */
         2,49,       /* Addr. 21 (Keyed Dept 49) */
         2,50,       /* Addr. 22 (Keyed Dept 50) */
         1,23,       /* Addr. 23 (Keyed PLU 23) */
         1,24,       /* Addr. 24 (Keyed PLU 24) */
         1,25,       /* Addr. 25 (Keyed PLU 25) */
         1,26,       /* Addr. 26 (Keyed PLU 26) */
         1,27,       /* Addr. 27 (Keyed PLU 27) */
         1,28,       /* Addr. 28 (Keyed PLU 28) */
         1,29,       /* Addr. 29 (Keyed PLU 29) */
         1,30,       /* Addr. 30 (Keyed PLU 30) */
         1,31,       /* Addr. 31 (Keyed PLU 31) */
         1,32,       /* Addr. 32 (Keyed PLU 32) */
         1,33,       /* Addr. 33 (Keyed PLU 33) */
         1,34,       /* Addr. 34 (Keyed PLU 34) */
         1,35,       /* Addr. 35 (Keyed PLU 35) */
         1,36,       /* Addr. 36 (Keyed PLU 36) */
         1,37,       /* Addr. 37 (Keyed PLU 37) */
         1,38,       /* Addr. 38 (Keyed PLU 38) */
         1,39,       /* Addr. 39 (Keyed PLU 39) */
         1,40,       /* Addr. 40 (Keyed PLU 40) */
         1,41,       /* Addr. 41 (Keyed PLU 41) */
         1,42,       /* Addr. 42 (Keyed PLU 42) */
         1,43,       /* Addr. 43 (Keyed PLU 43) */
         1,44,       /* Addr. 44 (Keyed PLU 44) */
         1,45,       /* Addr. 45 (Keyed PLU 45) */
         1,46,       /* Addr. 46 (Keyed PLU 46) */
         1,47,       /* Addr. 47 (Keyed PLU 47) */
         1,48,       /* Addr. 48 (Keyed PLU 48) */
         1,49,       /* Addr. 49 (Keyed PLU 49) */
         1,50,       /* Addr. 50 (Keyed PLU 50) */
         1,51,       /* Addr. 51 (Keyed PLU 51) */
         1,52,       /* Addr. 52 (Keyed PLU 52) */
         1,53,       /* Addr. 53 (Keyed PLU 53) */
         1,54,       /* Addr. 54 (Keyed PLU 54) */
         1,55,       /* Addr. 55 (Keyed PLU 55) */
         1,56,       /* Addr. 56 (Keyed PLU 56) */
         1,57,       /* Addr. 57 (Keyed PLU 57) */
        54, 1,       /* Addr. 58 (Direct Menu Shift 1) */
        54, 2,       /* Addr. 59 (Direct Menu Shift 2) */
        54, 3,       /* Addr. 60 (Direct Menu Shift 3) */
        54, 4,       /* Addr. 61 (Direct Menu Shift 4) */
        54, 5,       /* Addr. 62 (Direct Menu Shift 5) */
        53, 1,       /* Addr. 63 (Preset Amount Cash Tender 1) */
        53, 2,       /* Addr. 64 (Preset Amount Cash Tender 2) */
        53, 3,       /* Addr. 65 (Preset Amount Cash Tender 3) */
        53, 4,       /* Addr. 66 (Preset Amount Cash Tender 4) */
         4,11,       /* Addr. 67 (Adjective 3-1) */
         4,12,       /* Addr. 68 (Adjective 3-2) */
         4,13,       /* Addr. 69 (Adjective 3-3) */
         4,14,       /* Addr. 70 (Adjective 3-4) */
         4,15,       /* Addr. 71 (Adjective 3-5) */
         4,16,       /* Addr. 72 (Adjective 4-1) */
         4,17,       /* Addr. 73 (Adjective 4-2) */
         4,18,       /* Addr. 74 (Adjective 4-3) */
         4,19,       /* Addr. 75 (Adjective 4-4) */
         4,20,       /* Addr. 76 (Adjective 4-5) */
        55, 0,       /* Addr. 77 (Time In) */
         4, 1,       /* Addr. 78 (Adjective 1-1) */
         4, 2,       /* Addr. 79 (Adjective 1-2) */
         4, 3,       /* Addr. 80 (Adjective 1-3) */
         4, 4,       /* Addr. 81 (Adjective 1-4) */
         4, 5,       /* Addr. 82 (Adjective 1-5) */
         4, 6,       /* Addr. 83 (Adjective 2-1) */
         4, 7,       /* Addr. 84 (Adjective 2-2) */
         4, 8,       /* Addr. 85 (Adjective 2-3) */
         4, 9,       /* Addr. 86 (Adjective 2-4) */
         4,10,       /* Addr. 87 (Adjective 2-5) */
        56, 0,       /* Addr. 88 (Time Out) */
        11, 1,       /* Addr. 89 (Tax Mod 1) */
        11, 2,       /* Addr. 90 (Tax Mod 2) */
        11, 3,       /* Addr. 91 (Tax Mod 3) */
        11, 4,       /* Addr. 92 (Tax Mod 4) */
        11, 5,       /* Addr. 93 (Tax Mod 5) */
        11, 6,       /* Addr. 94 (Tax Mod 6) */
        11, 7,       /* Addr. 95 (Tax Mod 7) */
        11, 8,       /* Addr. 96 (Tax Mod 8) */
        11, 9,       /* Addr. 97 (Tax Mod 9) */
        11,10,       /* Addr. 98 (Tax Exempt 1) */
        11,11,       /* Addr. 99 (Tax Exempt 2) */
         3, 1,       /* Addr.100 (Print Mod 1) */
         3, 2,       /* Addr.101 (Print Mod 2) */
         3, 3,       /* Addr.102 (Print Mod 3) */
         3, 4,       /* Addr.103 (Print Mod 4) */
         3, 5,       /* Addr.104 (Print Mod 5) */
         3, 6,       /* Addr.105 (Print Mod 6) */
         3, 7,       /* Addr.106 (Print Mod 7) */
         3, 8,       /* Addr.107 (Print Mod 8) */
        39, 1,       /* Addr.108 (Charge Tips, V3.3) */
        24, 0,       /* Addr.109 (Tips Paid Out) */
        25, 0,       /* Addr.110 (Declared Tips) */
        12, 0,       /* Addr.111 (Sign-in) */
        15, 0,       /* Addr.112 (New Check) */
        17, 0,       /* Addr.113 (Guest Check #) */
         7, 8,       /* Addr.114 (Toatl 8) */
         7, 4,       /* Addr.115 (Toatl 4) */
        32, 0,       /* Addr.116 (E/C) */
        23, 0,       /* Addr.117 (Receipt/No Receipt) */
        30, 0,       /* Addr.118 (Cancel/Abort) */
        58, 0,       /* Addr.119 (Coupon Number) */
        37, 2,       /* Addr.120 (Print on Demand) */
        13, 0,       /* Addr.121 (B Key) */
        16, 0,       /* Addr.122 (PB) */
        19, 0,       /* Addr.123 (No. of Person) */
         7, 7,       /* Addr.124 (Total 7) */
         7, 3,       /* Addr.125 (Total 3) */
        34, 0,       /* Addr.126 (QTY) */
        31, 0,       /* Addr.127 (Item Void) */
        33, 0,       /* Addr.128 (Transaction Void) */
         6, 1,       /* Addr.129 (F/C 1) */
         6, 2,       /* Addr.130 (F/C 2) */
        28, 0,       /* Addr.131 (R/A) */
         0, 0,       /* Addr.132 (Server #) */
         8, 1,       /* Addr.133 (Add Check 1) */
        20, 0,       /* Addr.134 (Table #) */
         7, 6,       /* Addr.135 (Total 6) */
         7, 2,       /* Addr.136 (Total 2) */
         5, 7,       /* Addr.137 (Tender 7) */
         5, 8,       /* Addr.138 (Tender 8) */
        27, 0,       /* Addr.139 (P/O) */
        22, 0,       /* Addr.140 (Feed/Release) */
         8, 2,       /* Addr.141 (Add Check 2) */
        21, 0,       /* Addr.142 (Line #) */
         7, 5,       /* Addr.143 (Total 5) */
         7, 1,       /* Addr.144 (Total 1) */
         5, 5,       /* Addr.145 (Tender 5) */
         5, 6,       /* Addr.146 (Tender 6) */
        26, 0,       /* Addr.147 (No Sale) */
        29, 0,       /* Addr.148 (Change Computation) */
         8, 3,       /* Addr.149 (Add Check 3) */
        18, 0,       /* Addr.150 (Guest Check Transfer) */
         9, 2,       /* Addr.151 (Mod Item Discount) */
         9, 1,       /* Addr.152 (Item Discount) */
         5, 3,       /* Addr.153 (Tender 3) */
         5, 4,       /* Addr.154 (Tender 4) */
        36, 0,       /* Addr.155 (#/Type) */
        35, 0,       /* Addr.156 (Menu Shift) */
        52, 0,       /* Addr.157 (EJ Reverse) */
        10, 2,       /* Addr.158 (Regular Discount 2) */
        10, 1,       /* Addr.159 (Regular Discount 1) */
        40, 0,       /* Addr.160 (PLU No.) */
        45, 0,       /* Addr.161 (Decimal Point) */
        41, 0,       /* Addr.162 (00) */
         5, 1,       /* Addr.163 (Tender 1) */
         5, 2,       /* Addr.164 (Tender 2) */
        42, 0,        /* Addr.165 (A/C) */
		0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0
    },
    {       /* Page #6 Table Address */
        43, 0,       /* Addr.  1 (Recipt Feed) */
        44, 0,       /* Addr.  2 (Journal Feed) */
         2, 1,       /* Addr.  3 (Keyed Dept 1) */
         2, 2,       /* Addr.  4 (Keyed Dept 2) */
         2, 3,       /* Addr.  5 (Keyed Dept 3) */
         2, 4,       /* Addr.  6 (Keyed Dept 4) */
         2, 5,       /* Addr.  7 (Keyed Dept 5) */
         2, 6,       /* Addr.  8 (Keyed Dept 6) */
         2, 7,       /* Addr.  9 (Keyed Dept 7) */
         2, 8,       /* Addr. 10 (Keyed Dept 8) */
         2, 9,       /* Addr. 11 (Keyed Dept 9) */
         2,10,       /* Addr. 12 (Keyed Dept 10) */
         2,41,       /* Addr. 13 (Keyed Dept 41) */
         2,42,       /* Addr. 14 (Keyed Dept 42) */
         2,43,       /* Addr. 15 (Keyed Dept 43) */
         2,44,       /* Addr. 16 (Keyed Dept 44) */
         2,45,       /* Addr. 17 (Keyed Dept 45) */
         2,46,       /* Addr. 18 (Keyed Dept 46) */
         2,47,       /* Addr. 19 (Keyed Dept 47) */
         2,48,       /* Addr. 20 (Keyed Dept 48) */
         2,49,       /* Addr. 21 (Keyed Dept 49) */
         2,50,       /* Addr. 22 (Keyed Dept 50) */
         1,23,       /* Addr. 23 (Keyed PLU 23) */
         1,24,       /* Addr. 24 (Keyed PLU 24) */
         1,25,       /* Addr. 25 (Keyed PLU 25) */
         1,26,       /* Addr. 26 (Keyed PLU 26) */
         1,27,       /* Addr. 27 (Keyed PLU 27) */
         1,28,       /* Addr. 28 (Keyed PLU 28) */
         1,29,       /* Addr. 29 (Keyed PLU 29) */
         1,30,       /* Addr. 30 (Keyed PLU 30) */
         1,31,       /* Addr. 31 (Keyed PLU 31) */
         1,32,       /* Addr. 32 (Keyed PLU 32) */
         1,33,       /* Addr. 33 (Keyed PLU 33) */
         1,34,       /* Addr. 34 (Keyed PLU 34) */
         1,35,       /* Addr. 35 (Keyed PLU 35) */
         1,36,       /* Addr. 36 (Keyed PLU 36) */
         1,37,       /* Addr. 37 (Keyed PLU 37) */
         1,38,       /* Addr. 38 (Keyed PLU 38) */
         1,39,       /* Addr. 39 (Keyed PLU 39) */
         1,40,       /* Addr. 40 (Keyed PLU 40) */
         1,41,       /* Addr. 41 (Keyed PLU 41) */
         1,42,       /* Addr. 42 (Keyed PLU 42) */
         1,43,       /* Addr. 43 (Keyed PLU 43) */
         1,44,       /* Addr. 44 (Keyed PLU 44) */
         1,45,       /* Addr. 45 (Keyed PLU 45) */
         1,46,       /* Addr. 46 (Keyed PLU 46) */
         1,47,       /* Addr. 47 (Keyed PLU 47) */
         1,48,       /* Addr. 48 (Keyed PLU 48) */
         1,49,       /* Addr. 49 (Keyed PLU 49) */
         1,50,       /* Addr. 50 (Keyed PLU 50) */
         1,51,       /* Addr. 51 (Keyed PLU 51) */
         1,52,       /* Addr. 52 (Keyed PLU 52) */
         1,53,       /* Addr. 53 (Keyed PLU 53) */
         1,54,       /* Addr. 54 (Keyed PLU 54) */
         1,55,       /* Addr. 55 (Keyed PLU 55) */
         1,56,       /* Addr. 56 (Keyed PLU 56) */
         1,57,       /* Addr. 57 (Keyed PLU 57) */
        54, 1,       /* Addr. 58 (Direct Menu Shift 1) */
        54, 2,       /* Addr. 59 (Direct Menu Shift 2) */
        54, 3,       /* Addr. 60 (Direct Menu Shift 3) */
        54, 4,       /* Addr. 61 (Direct Menu Shift 4) */
        54, 5,       /* Addr. 62 (Direct Menu Shift 5) */
        53, 1,       /* Addr. 63 (Preset Amount Cash Tender 1) */
        53, 2,       /* Addr. 64 (Preset Amount Cash Tender 2) */
        53, 3,       /* Addr. 65 (Preset Amount Cash Tender 3) */
        53, 4,       /* Addr. 66 (Preset Amount Cash Tender 4) */
         4,11,       /* Addr. 67 (Adjective 3-1) */
         4,12,       /* Addr. 68 (Adjective 3-2) */
         4,13,       /* Addr. 69 (Adjective 3-3) */
         4,14,       /* Addr. 70 (Adjective 3-4) */
         4,15,       /* Addr. 71 (Adjective 3-5) */
         4,16,       /* Addr. 72 (Adjective 4-1) */
         4,17,       /* Addr. 73 (Adjective 4-2) */
         4,18,       /* Addr. 74 (Adjective 4-3) */
         4,19,       /* Addr. 75 (Adjective 4-4) */
         4,20,       /* Addr. 76 (Adjective 4-5) */
        55, 0,       /* Addr. 77 (Time In) */
         4, 1,       /* Addr. 78 (Adjective 1-1) */
         4, 2,       /* Addr. 79 (Adjective 1-2) */
         4, 3,       /* Addr. 80 (Adjective 1-3) */
         4, 4,       /* Addr. 81 (Adjective 1-4) */
         4, 5,       /* Addr. 82 (Adjective 1-5) */
         4, 6,       /* Addr. 83 (Adjective 2-1) */
         4, 7,       /* Addr. 84 (Adjective 2-2) */
         4, 8,       /* Addr. 85 (Adjective 2-3) */
         4, 9,       /* Addr. 86 (Adjective 2-4) */
         4,10,       /* Addr. 87 (Adjective 2-5) */
        56, 0,       /* Addr. 88 (Time Out) */
        11, 1,       /* Addr. 89 (Tax Mod 1) */
        11, 2,       /* Addr. 90 (Tax Mod 2) */
        11, 3,       /* Addr. 91 (Tax Mod 3) */
        11, 4,       /* Addr. 92 (Tax Mod 4) */
        11, 5,       /* Addr. 93 (Tax Mod 5) */
        11, 6,       /* Addr. 94 (Tax Mod 6) */
        11, 7,       /* Addr. 95 (Tax Mod 7) */
        11, 8,       /* Addr. 96 (Tax Mod 8) */
        11, 9,       /* Addr. 97 (Tax Mod 9) */
        11,10,       /* Addr. 98 (Tax Exempt 1) */
        11,11,       /* Addr. 99 (Tax Exempt 2) */
         3, 1,       /* Addr.100 (Print Mod 1) */
         3, 2,       /* Addr.101 (Print Mod 2) */
         3, 3,       /* Addr.102 (Print Mod 3) */
         3, 4,       /* Addr.103 (Print Mod 4) */
         3, 5,       /* Addr.104 (Print Mod 5) */
         3, 6,       /* Addr.105 (Print Mod 6) */
         3, 7,       /* Addr.106 (Print Mod 7) */
         3, 8,       /* Addr.107 (Print Mod 8) */
        39, 1,       /* Addr.108 (Charge Tips, V3.3) */
        24, 0,       /* Addr.109 (Tips Paid Out) */
        25, 0,       /* Addr.110 (Declared Tips) */
        12, 0,       /* Addr.111 (Sign-in) */
        15, 0,       /* Addr.112 (New Check) */
        17, 0,       /* Addr.113 (Guest Check #) */
         7, 8,       /* Addr.114 (Toatl 8) */
         7, 4,       /* Addr.115 (Toatl 4) */
        32, 0,       /* Addr.116 (E/C) */
        23, 0,       /* Addr.117 (Receipt/No Receipt) */
        30, 0,       /* Addr.118 (Cancel/Abort) */
        58, 0,       /* Addr.119 (Coupon Number) */
        37, 2,       /* Addr.120 (Print on Demand) */
        13, 0,       /* Addr.121 (B Key) */
        16, 0,       /* Addr.122 (PB) */
        19, 0,       /* Addr.123 (No. of Person) */
         7, 7,       /* Addr.124 (Total 7) */
         7, 3,       /* Addr.125 (Total 3) */
        34, 0,       /* Addr.126 (QTY) */
        31, 0,       /* Addr.127 (Item Void) */
        33, 0,       /* Addr.128 (Transaction Void) */
         6, 1,       /* Addr.129 (F/C 1) */
         6, 2,       /* Addr.130 (F/C 2) */
        28, 0,       /* Addr.131 (R/A) */
         0, 0,       /* Addr.132 (Server #) */
         8, 1,       /* Addr.133 (Add Check 1) */
        20, 0,       /* Addr.134 (Table #) */
         7, 6,       /* Addr.135 (Total 6) */
         7, 2,       /* Addr.136 (Total 2) */
         5, 7,       /* Addr.137 (Tender 7) */
         5, 8,       /* Addr.138 (Tender 8) */
        27, 0,       /* Addr.139 (P/O) */
        22, 0,       /* Addr.140 (Feed/Release) */
         8, 2,       /* Addr.141 (Add Check 2) */
        21, 0,       /* Addr.142 (Line #) */
         7, 5,       /* Addr.143 (Total 5) */
         7, 1,       /* Addr.144 (Total 1) */
         5, 5,       /* Addr.145 (Tender 5) */
         5, 6,       /* Addr.146 (Tender 6) */
        26, 0,       /* Addr.147 (No Sale) */
        29, 0,       /* Addr.148 (Change Computation) */
         8, 3,       /* Addr.149 (Add Check 3) */
        18, 0,       /* Addr.150 (Guest Check Transfer) */
         9, 2,       /* Addr.151 (Mod Item Discount) */
         9, 1,       /* Addr.152 (Item Discount) */
         5, 3,       /* Addr.153 (Tender 3) */
         5, 4,       /* Addr.154 (Tender 4) */
        36, 0,       /* Addr.155 (#/Type) */
        35, 0,       /* Addr.156 (Menu Shift) */
        52, 0,       /* Addr.157 (EJ Reverse) */
        10, 2,       /* Addr.158 (Regular Discount 2) */
        10, 1,       /* Addr.159 (Regular Discount 1) */
        40, 0,       /* Addr.160 (PLU No.) */
        45, 0,       /* Addr.161 (Decimal Point) */
        41, 0,       /* Addr.162 (00) */
         5, 1,       /* Addr.163 (Tender 1) */
         5, 2,       /* Addr.164 (Tender 2) */
        42, 0,        /* Addr.165 (A/C) */
				0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0
    },
    {       /* Page #7 Table Address */
        43, 0,       /* Addr.  1 (Recipt Feed) */
        44, 0,       /* Addr.  2 (Journal Feed) */
         2, 1,       /* Addr.  3 (Keyed Dept 1) */
         2, 2,       /* Addr.  4 (Keyed Dept 2) */
         2, 3,       /* Addr.  5 (Keyed Dept 3) */
         2, 4,       /* Addr.  6 (Keyed Dept 4) */
         2, 5,       /* Addr.  7 (Keyed Dept 5) */
         2, 6,       /* Addr.  8 (Keyed Dept 6) */
         2, 7,       /* Addr.  9 (Keyed Dept 7) */
         2, 8,       /* Addr. 10 (Keyed Dept 8) */
         2, 9,       /* Addr. 11 (Keyed Dept 9) */
         2,10,       /* Addr. 12 (Keyed Dept 10) */
         2,41,       /* Addr. 13 (Keyed Dept 41) */
         2,42,       /* Addr. 14 (Keyed Dept 42) */
         2,43,       /* Addr. 15 (Keyed Dept 43) */
         2,44,       /* Addr. 16 (Keyed Dept 44) */
         2,45,       /* Addr. 17 (Keyed Dept 45) */
         2,46,       /* Addr. 18 (Keyed Dept 46) */
         2,47,       /* Addr. 19 (Keyed Dept 47) */
         2,48,       /* Addr. 20 (Keyed Dept 48) */
         2,49,       /* Addr. 21 (Keyed Dept 49) */
         2,50,       /* Addr. 22 (Keyed Dept 50) */
         1,23,       /* Addr. 23 (Keyed PLU 23) */
         1,24,       /* Addr. 24 (Keyed PLU 24) */
         1,25,       /* Addr. 25 (Keyed PLU 25) */
         1,26,       /* Addr. 26 (Keyed PLU 26) */
         1,27,       /* Addr. 27 (Keyed PLU 27) */
         1,28,       /* Addr. 28 (Keyed PLU 28) */
         1,29,       /* Addr. 29 (Keyed PLU 29) */
         1,30,       /* Addr. 30 (Keyed PLU 30) */
         1,31,       /* Addr. 31 (Keyed PLU 31) */
         1,32,       /* Addr. 32 (Keyed PLU 32) */
         1,33,       /* Addr. 33 (Keyed PLU 33) */
         1,34,       /* Addr. 34 (Keyed PLU 34) */
         1,35,       /* Addr. 35 (Keyed PLU 35) */
         1,36,       /* Addr. 36 (Keyed PLU 36) */
         1,37,       /* Addr. 37 (Keyed PLU 37) */
         1,38,       /* Addr. 38 (Keyed PLU 38) */
         1,39,       /* Addr. 39 (Keyed PLU 39) */
         1,40,       /* Addr. 40 (Keyed PLU 40) */
         1,41,       /* Addr. 41 (Keyed PLU 41) */
         1,42,       /* Addr. 42 (Keyed PLU 42) */
         1,43,       /* Addr. 43 (Keyed PLU 43) */
         1,44,       /* Addr. 44 (Keyed PLU 44) */
         1,45,       /* Addr. 45 (Keyed PLU 45) */
         1,46,       /* Addr. 46 (Keyed PLU 46) */
         1,47,       /* Addr. 47 (Keyed PLU 47) */
         1,48,       /* Addr. 48 (Keyed PLU 48) */
         1,49,       /* Addr. 49 (Keyed PLU 49) */
         1,50,       /* Addr. 50 (Keyed PLU 50) */
         1,51,       /* Addr. 51 (Keyed PLU 51) */
         1,52,       /* Addr. 52 (Keyed PLU 52) */
         1,53,       /* Addr. 53 (Keyed PLU 53) */
         1,54,       /* Addr. 54 (Keyed PLU 54) */
         1,55,       /* Addr. 55 (Keyed PLU 55) */
         1,56,       /* Addr. 56 (Keyed PLU 56) */
         1,57,       /* Addr. 57 (Keyed PLU 57) */
        54, 1,       /* Addr. 58 (Direct Menu Shift 1) */
        54, 2,       /* Addr. 59 (Direct Menu Shift 2) */
        54, 3,       /* Addr. 60 (Direct Menu Shift 3) */
        54, 4,       /* Addr. 61 (Direct Menu Shift 4) */
        54, 5,       /* Addr. 62 (Direct Menu Shift 5) */
        53, 1,       /* Addr. 63 (Preset Amount Cash Tender 1) */
        53, 2,       /* Addr. 64 (Preset Amount Cash Tender 2) */
        53, 3,       /* Addr. 65 (Preset Amount Cash Tender 3) */
        53, 4,       /* Addr. 66 (Preset Amount Cash Tender 4) */
         4,11,       /* Addr. 67 (Adjective 3-1) */
         4,12,       /* Addr. 68 (Adjective 3-2) */
         4,13,       /* Addr. 69 (Adjective 3-3) */
         4,14,       /* Addr. 70 (Adjective 3-4) */
         4,15,       /* Addr. 71 (Adjective 3-5) */
         4,16,       /* Addr. 72 (Adjective 4-1) */
         4,17,       /* Addr. 73 (Adjective 4-2) */
         4,18,       /* Addr. 74 (Adjective 4-3) */
         4,19,       /* Addr. 75 (Adjective 4-4) */
         4,20,       /* Addr. 76 (Adjective 4-5) */
        55, 0,       /* Addr. 77 (Time In) */
         4, 1,       /* Addr. 78 (Adjective 1-1) */
         4, 2,       /* Addr. 79 (Adjective 1-2) */
         4, 3,       /* Addr. 80 (Adjective 1-3) */
         4, 4,       /* Addr. 81 (Adjective 1-4) */
         4, 5,       /* Addr. 82 (Adjective 1-5) */
         4, 6,       /* Addr. 83 (Adjective 2-1) */
         4, 7,       /* Addr. 84 (Adjective 2-2) */
         4, 8,       /* Addr. 85 (Adjective 2-3) */
         4, 9,       /* Addr. 86 (Adjective 2-4) */
         4,10,       /* Addr. 87 (Adjective 2-5) */
        56, 0,       /* Addr. 88 (Time Out) */
        11, 1,       /* Addr. 89 (Tax Mod 1) */
        11, 2,       /* Addr. 90 (Tax Mod 2) */
        11, 3,       /* Addr. 91 (Tax Mod 3) */
        11, 4,       /* Addr. 92 (Tax Mod 4) */
        11, 5,       /* Addr. 93 (Tax Mod 5) */
        11, 6,       /* Addr. 94 (Tax Mod 6) */
        11, 7,       /* Addr. 95 (Tax Mod 7) */
        11, 8,       /* Addr. 96 (Tax Mod 8) */
        11, 9,       /* Addr. 97 (Tax Mod 9) */
        11,10,       /* Addr. 98 (Tax Exempt 1) */
        11,11,       /* Addr. 99 (Tax Exempt 2) */
         3, 1,       /* Addr.100 (Print Mod 1) */
         3, 2,       /* Addr.101 (Print Mod 2) */
         3, 3,       /* Addr.102 (Print Mod 3) */
         3, 4,       /* Addr.103 (Print Mod 4) */
         3, 5,       /* Addr.104 (Print Mod 5) */
         3, 6,       /* Addr.105 (Print Mod 6) */
         3, 7,       /* Addr.106 (Print Mod 7) */
         3, 8,       /* Addr.107 (Print Mod 8) */
        39, 1,       /* Addr.108 (Charge Tips, V3.3) */
        24, 0,       /* Addr.109 (Tips Paid Out) */
        25, 0,       /* Addr.110 (Declared Tips) */
        12, 0,       /* Addr.111 (Sign-in) */
        15, 0,       /* Addr.112 (New Check) */
        17, 0,       /* Addr.113 (Guest Check #) */
         7, 8,       /* Addr.114 (Toatl 8) */
         7, 4,       /* Addr.115 (Toatl 4) */
        32, 0,       /* Addr.116 (E/C) */
        23, 0,       /* Addr.117 (Receipt/No Receipt) */
        30, 0,       /* Addr.118 (Cancel/Abort) */
        58, 0,       /* Addr.119 (Coupon Number) */
        37, 2,       /* Addr.120 (Print on Demand) */
        13, 0,       /* Addr.121 (B Key) */
        16, 0,       /* Addr.122 (PB) */
        19, 0,       /* Addr.123 (No. of Person) */
         7, 7,       /* Addr.124 (Total 7) */
         7, 3,       /* Addr.125 (Total 3) */
        34, 0,       /* Addr.126 (QTY) */
        31, 0,       /* Addr.127 (Item Void) */
        33, 0,       /* Addr.128 (Transaction Void) */
         6, 1,       /* Addr.129 (F/C 1) */
         6, 2,       /* Addr.130 (F/C 2) */
        28, 0,       /* Addr.131 (R/A) */
         0, 0,       /* Addr.132 (Server #) */
         8, 1,       /* Addr.133 (Add Check 1) */
        20, 0,       /* Addr.134 (Table #) */
         7, 6,       /* Addr.135 (Total 6) */
         7, 2,       /* Addr.136 (Total 2) */
         5, 7,       /* Addr.137 (Tender 7) */
         5, 8,       /* Addr.138 (Tender 8) */
        27, 0,       /* Addr.139 (P/O) */
        22, 0,       /* Addr.140 (Feed/Release) */
         8, 2,       /* Addr.141 (Add Check 2) */
        21, 0,       /* Addr.142 (Line #) */
         7, 5,       /* Addr.143 (Total 5) */
         7, 1,       /* Addr.144 (Total 1) */
         5, 5,       /* Addr.145 (Tender 5) */
         5, 6,       /* Addr.146 (Tender 6) */
        26, 0,       /* Addr.147 (No Sale) */
        29, 0,       /* Addr.148 (Change Computation) */
         8, 3,       /* Addr.149 (Add Check 3) */
        18, 0,       /* Addr.150 (Guest Check Transfer) */
         9, 2,       /* Addr.151 (Mod Item Discount) */
         9, 1,       /* Addr.152 (Item Discount) */
         5, 3,       /* Addr.153 (Tender 3) */
         5, 4,       /* Addr.154 (Tender 4) */
        36, 0,       /* Addr.155 (#/Type) */
        35, 0,       /* Addr.156 (Menu Shift) */
        52, 0,       /* Addr.157 (EJ Reverse) */
        10, 2,       /* Addr.158 (Regular Discount 2) */
        10, 1,       /* Addr.159 (Regular Discount 1) */
        40, 0,       /* Addr.160 (PLU No.) */
        45, 0,       /* Addr.161 (Decimal Point) */
        41, 0,       /* Addr.162 (00) */
         5, 1,       /* Addr.163 (Tender 1) */
         5, 2,       /* Addr.164 (Tender 2) */
        42, 0,        /* Addr.165 (A/C) */
				0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
    },
    {       /* Page #8 Table Address */
        43, 0,       /* Addr.  1 (Recipt Feed) */
        44, 0,       /* Addr.  2 (Journal Feed) */
         2, 1,       /* Addr.  3 (Keyed Dept 1) */
         2, 2,       /* Addr.  4 (Keyed Dept 2) */
         2, 3,       /* Addr.  5 (Keyed Dept 3) */
         2, 4,       /* Addr.  6 (Keyed Dept 4) */
         2, 5,       /* Addr.  7 (Keyed Dept 5) */
         2, 6,       /* Addr.  8 (Keyed Dept 6) */
         2, 7,       /* Addr.  9 (Keyed Dept 7) */
         2, 8,       /* Addr. 10 (Keyed Dept 8) */
         2, 9,       /* Addr. 11 (Keyed Dept 9) */
         2,10,       /* Addr. 12 (Keyed Dept 10) */
         2,41,       /* Addr. 13 (Keyed Dept 41) */
         2,42,       /* Addr. 14 (Keyed Dept 42) */
         2,43,       /* Addr. 15 (Keyed Dept 43) */
         2,44,       /* Addr. 16 (Keyed Dept 44) */
         2,45,       /* Addr. 17 (Keyed Dept 45) */
         2,46,       /* Addr. 18 (Keyed Dept 46) */
         2,47,       /* Addr. 19 (Keyed Dept 47) */
         2,48,       /* Addr. 20 (Keyed Dept 48) */
         2,49,       /* Addr. 21 (Keyed Dept 49) */
         2,50,       /* Addr. 22 (Keyed Dept 50) */
         1,23,       /* Addr. 23 (Keyed PLU 23) */
         1,24,       /* Addr. 24 (Keyed PLU 24) */
         1,25,       /* Addr. 25 (Keyed PLU 25) */
         1,26,       /* Addr. 26 (Keyed PLU 26) */
         1,27,       /* Addr. 27 (Keyed PLU 27) */
         1,28,       /* Addr. 28 (Keyed PLU 28) */
         1,29,       /* Addr. 29 (Keyed PLU 29) */
         1,30,       /* Addr. 30 (Keyed PLU 30) */
         1,31,       /* Addr. 31 (Keyed PLU 31) */
         1,32,       /* Addr. 32 (Keyed PLU 32) */
         1,33,       /* Addr. 33 (Keyed PLU 33) */
         1,34,       /* Addr. 34 (Keyed PLU 34) */
         1,35,       /* Addr. 35 (Keyed PLU 35) */
         1,36,       /* Addr. 36 (Keyed PLU 36) */
         1,37,       /* Addr. 37 (Keyed PLU 37) */
         1,38,       /* Addr. 38 (Keyed PLU 38) */
         1,39,       /* Addr. 39 (Keyed PLU 39) */
         1,40,       /* Addr. 40 (Keyed PLU 40) */
         1,41,       /* Addr. 41 (Keyed PLU 41) */
         1,42,       /* Addr. 42 (Keyed PLU 42) */
         1,43,       /* Addr. 43 (Keyed PLU 43) */
         1,44,       /* Addr. 44 (Keyed PLU 44) */
         1,45,       /* Addr. 45 (Keyed PLU 45) */
         1,46,       /* Addr. 46 (Keyed PLU 46) */
         1,47,       /* Addr. 47 (Keyed PLU 47) */
         1,48,       /* Addr. 48 (Keyed PLU 48) */
         1,49,       /* Addr. 49 (Keyed PLU 49) */
         1,50,       /* Addr. 50 (Keyed PLU 50) */
         1,51,       /* Addr. 51 (Keyed PLU 51) */
         1,52,       /* Addr. 52 (Keyed PLU 52) */
         1,53,       /* Addr. 53 (Keyed PLU 53) */
         1,54,       /* Addr. 54 (Keyed PLU 54) */
         1,55,       /* Addr. 55 (Keyed PLU 55) */
         1,56,       /* Addr. 56 (Keyed PLU 56) */
         1,57,       /* Addr. 57 (Keyed PLU 57) */
        54, 1,       /* Addr. 58 (Direct Menu Shift 1) */
        54, 2,       /* Addr. 59 (Direct Menu Shift 2) */
        54, 3,       /* Addr. 60 (Direct Menu Shift 3) */
        54, 4,       /* Addr. 61 (Direct Menu Shift 4) */
        54, 5,       /* Addr. 62 (Direct Menu Shift 5) */
        53, 1,       /* Addr. 63 (Preset Amount Cash Tender 1) */
        53, 2,       /* Addr. 64 (Preset Amount Cash Tender 2) */
        53, 3,       /* Addr. 65 (Preset Amount Cash Tender 3) */
        53, 4,       /* Addr. 66 (Preset Amount Cash Tender 4) */
         4,11,       /* Addr. 67 (Adjective 3-1) */
         4,12,       /* Addr. 68 (Adjective 3-2) */
         4,13,       /* Addr. 69 (Adjective 3-3) */
         4,14,       /* Addr. 70 (Adjective 3-4) */
         4,15,       /* Addr. 71 (Adjective 3-5) */
         4,16,       /* Addr. 72 (Adjective 4-1) */
         4,17,       /* Addr. 73 (Adjective 4-2) */
         4,18,       /* Addr. 74 (Adjective 4-3) */
         4,19,       /* Addr. 75 (Adjective 4-4) */
         4,20,       /* Addr. 76 (Adjective 4-5) */
        55, 0,       /* Addr. 77 (Time In) */
         4, 1,       /* Addr. 78 (Adjective 1-1) */
         4, 2,       /* Addr. 79 (Adjective 1-2) */
         4, 3,       /* Addr. 80 (Adjective 1-3) */
         4, 4,       /* Addr. 81 (Adjective 1-4) */
         4, 5,       /* Addr. 82 (Adjective 1-5) */
         4, 6,       /* Addr. 83 (Adjective 2-1) */
         4, 7,       /* Addr. 84 (Adjective 2-2) */
         4, 8,       /* Addr. 85 (Adjective 2-3) */
         4, 9,       /* Addr. 86 (Adjective 2-4) */
         4,10,       /* Addr. 87 (Adjective 2-5) */
        56, 0,       /* Addr. 88 (Time Out) */
        11, 1,       /* Addr. 89 (Tax Mod 1) */
        11, 2,       /* Addr. 90 (Tax Mod 2) */
        11, 3,       /* Addr. 91 (Tax Mod 3) */
        11, 4,       /* Addr. 92 (Tax Mod 4) */
        11, 5,       /* Addr. 93 (Tax Mod 5) */
        11, 6,       /* Addr. 94 (Tax Mod 6) */
        11, 7,       /* Addr. 95 (Tax Mod 7) */
        11, 8,       /* Addr. 96 (Tax Mod 8) */
        11, 9,       /* Addr. 97 (Tax Mod 9) */
        11,10,       /* Addr. 98 (Tax Exempt 1) */
        11,11,       /* Addr. 99 (Tax Exempt 2) */
         3, 1,       /* Addr.100 (Print Mod 1) */
         3, 2,       /* Addr.101 (Print Mod 2) */
         3, 3,       /* Addr.102 (Print Mod 3) */
         3, 4,       /* Addr.103 (Print Mod 4) */
         3, 5,       /* Addr.104 (Print Mod 5) */
         3, 6,       /* Addr.105 (Print Mod 6) */
         3, 7,       /* Addr.106 (Print Mod 7) */
         3, 8,       /* Addr.107 (Print Mod 8) */
        39, 1,       /* Addr.108 (Charge Tips, V3.3) */
        24, 0,       /* Addr.109 (Tips Paid Out) */
        25, 0,       /* Addr.110 (Declared Tips) */
        12, 0,       /* Addr.111 (Sign-in) */
        15, 0,       /* Addr.112 (New Check) */
        17, 0,       /* Addr.113 (Guest Check #) */
         7, 8,       /* Addr.114 (Toatl 8) */
         7, 4,       /* Addr.115 (Toatl 4) */
        32, 0,       /* Addr.116 (E/C) */
        23, 0,       /* Addr.117 (Receipt/No Receipt) */
        30, 0,       /* Addr.118 (Cancel/Abort) */
        58, 0,       /* Addr.119 (Coupon Number) */
        37, 2,       /* Addr.120 (Print on Demand) */
        13, 0,       /* Addr.121 (B Key) */
        16, 0,       /* Addr.122 (PB) */
        19, 0,       /* Addr.123 (No. of Person) */
         7, 7,       /* Addr.124 (Total 7) */
         7, 3,       /* Addr.125 (Total 3) */
        34, 0,       /* Addr.126 (QTY) */
        31, 0,       /* Addr.127 (Item Void) */
        33, 0,       /* Addr.128 (Transaction Void) */
         6, 1,       /* Addr.129 (F/C 1) */
         6, 2,       /* Addr.130 (F/C 2) */
        28, 0,       /* Addr.131 (R/A) */
         0, 0,       /* Addr.132 (Server #) */
         8, 1,       /* Addr.133 (Add Check 1) */
        20, 0,       /* Addr.134 (Table #) */
         7, 6,       /* Addr.135 (Total 6) */
         7, 2,       /* Addr.136 (Total 2) */
         5, 7,       /* Addr.137 (Tender 7) */
         5, 8,       /* Addr.138 (Tender 8) */
        27, 0,       /* Addr.139 (P/O) */
        22, 0,       /* Addr.140 (Feed/Release) */
         8, 2,       /* Addr.141 (Add Check 2) */
        21, 0,       /* Addr.142 (Line #) */
         7, 5,       /* Addr.143 (Total 5) */
         7, 1,       /* Addr.144 (Total 1) */
         5, 5,       /* Addr.145 (Tender 5) */
         5, 6,       /* Addr.146 (Tender 6) */
        26, 0,       /* Addr.147 (No Sale) */
        29, 0,       /* Addr.148 (Change Computation) */
         8, 3,       /* Addr.149 (Add Check 3) */
        18, 0,       /* Addr.150 (Guest Check Transfer) */
         9, 2,       /* Addr.151 (Mod Item Discount) */
         9, 1,       /* Addr.152 (Item Discount) */
         5, 3,       /* Addr.153 (Tender 3) */
         5, 4,       /* Addr.154 (Tender 4) */
        36, 0,       /* Addr.155 (#/Type) */
        35, 0,       /* Addr.156 (Menu Shift) */
        52, 0,       /* Addr.157 (EJ Reverse) */
        10, 2,       /* Addr.158 (Regular Discount 2) */
        10, 1,       /* Addr.159 (Regular Discount 1) */
        40, 0,       /* Addr.160 (PLU No.) */
        45, 0,       /* Addr.161 (Decimal Point) */
        41, 0,       /* Addr.162 (00) */
         5, 1,       /* Addr.163 (Tender 1) */
         5, 2,       /* Addr.164 (Tender 2) */
        42, 0,        /* Addr.165 (A/C) */
				0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
    },
    {       /* Page #9 Table Address */
        43, 0,       /* Addr.  1 (Recipt Feed) */
        44, 0,       /* Addr.  2 (Journal Feed) */
         2, 1,       /* Addr.  3 (Keyed Dept 1) */
         2, 2,       /* Addr.  4 (Keyed Dept 2) */
         2, 3,       /* Addr.  5 (Keyed Dept 3) */
         2, 4,       /* Addr.  6 (Keyed Dept 4) */
         2, 5,       /* Addr.  7 (Keyed Dept 5) */
         2, 6,       /* Addr.  8 (Keyed Dept 6) */
         2, 7,       /* Addr.  9 (Keyed Dept 7) */
         2, 8,       /* Addr. 10 (Keyed Dept 8) */
         2, 9,       /* Addr. 11 (Keyed Dept 9) */
         2,10,       /* Addr. 12 (Keyed Dept 10) */
         2,41,       /* Addr. 13 (Keyed Dept 41) */
         2,42,       /* Addr. 14 (Keyed Dept 42) */
         2,43,       /* Addr. 15 (Keyed Dept 43) */
         2,44,       /* Addr. 16 (Keyed Dept 44) */
         2,45,       /* Addr. 17 (Keyed Dept 45) */
         2,46,       /* Addr. 18 (Keyed Dept 46) */
         2,47,       /* Addr. 19 (Keyed Dept 47) */
         2,48,       /* Addr. 20 (Keyed Dept 48) */
         2,49,       /* Addr. 21 (Keyed Dept 49) */
         2,50,       /* Addr. 22 (Keyed Dept 50) */
         1,23,       /* Addr. 23 (Keyed PLU 23) */
         1,24,       /* Addr. 24 (Keyed PLU 24) */
         1,25,       /* Addr. 25 (Keyed PLU 25) */
         1,26,       /* Addr. 26 (Keyed PLU 26) */
         1,27,       /* Addr. 27 (Keyed PLU 27) */
         1,28,       /* Addr. 28 (Keyed PLU 28) */
         1,29,       /* Addr. 29 (Keyed PLU 29) */
         1,30,       /* Addr. 30 (Keyed PLU 30) */
         1,31,       /* Addr. 31 (Keyed PLU 31) */
         1,32,       /* Addr. 32 (Keyed PLU 32) */
         1,33,       /* Addr. 33 (Keyed PLU 33) */
         1,34,       /* Addr. 34 (Keyed PLU 34) */
         1,35,       /* Addr. 35 (Keyed PLU 35) */
         1,36,       /* Addr. 36 (Keyed PLU 36) */
         1,37,       /* Addr. 37 (Keyed PLU 37) */
         1,38,       /* Addr. 38 (Keyed PLU 38) */
         1,39,       /* Addr. 39 (Keyed PLU 39) */
         1,40,       /* Addr. 40 (Keyed PLU 40) */
         1,41,       /* Addr. 41 (Keyed PLU 41) */
         1,42,       /* Addr. 42 (Keyed PLU 42) */
         1,43,       /* Addr. 43 (Keyed PLU 43) */
         1,44,       /* Addr. 44 (Keyed PLU 44) */
         1,45,       /* Addr. 45 (Keyed PLU 45) */
         1,46,       /* Addr. 46 (Keyed PLU 46) */
         1,47,       /* Addr. 47 (Keyed PLU 47) */
         1,48,       /* Addr. 48 (Keyed PLU 48) */
         1,49,       /* Addr. 49 (Keyed PLU 49) */
         1,50,       /* Addr. 50 (Keyed PLU 50) */
         1,51,       /* Addr. 51 (Keyed PLU 51) */
         1,52,       /* Addr. 52 (Keyed PLU 52) */
         1,53,       /* Addr. 53 (Keyed PLU 53) */
         1,54,       /* Addr. 54 (Keyed PLU 54) */
         1,55,       /* Addr. 55 (Keyed PLU 55) */
         1,56,       /* Addr. 56 (Keyed PLU 56) */
         1,57,       /* Addr. 57 (Keyed PLU 57) */
        54, 1,       /* Addr. 58 (Direct Menu Shift 1) */
        54, 2,       /* Addr. 59 (Direct Menu Shift 2) */
        54, 3,       /* Addr. 60 (Direct Menu Shift 3) */
        54, 4,       /* Addr. 61 (Direct Menu Shift 4) */
        54, 5,       /* Addr. 62 (Direct Menu Shift 5) */
        53, 1,       /* Addr. 63 (Preset Amount Cash Tender 1) */
        53, 2,       /* Addr. 64 (Preset Amount Cash Tender 2) */
        53, 3,       /* Addr. 65 (Preset Amount Cash Tender 3) */
        53, 4,       /* Addr. 66 (Preset Amount Cash Tender 4) */
         4,11,       /* Addr. 67 (Adjective 3-1) */
         4,12,       /* Addr. 68 (Adjective 3-2) */
         4,13,       /* Addr. 69 (Adjective 3-3) */
         4,14,       /* Addr. 70 (Adjective 3-4) */
         4,15,       /* Addr. 71 (Adjective 3-5) */
         4,16,       /* Addr. 72 (Adjective 4-1) */
         4,17,       /* Addr. 73 (Adjective 4-2) */
         4,18,       /* Addr. 74 (Adjective 4-3) */
         4,19,       /* Addr. 75 (Adjective 4-4) */
         4,20,       /* Addr. 76 (Adjective 4-5) */
        55, 0,       /* Addr. 77 (Time In) */
         4, 1,       /* Addr. 78 (Adjective 1-1) */
         4, 2,       /* Addr. 79 (Adjective 1-2) */
         4, 3,       /* Addr. 80 (Adjective 1-3) */
         4, 4,       /* Addr. 81 (Adjective 1-4) */
         4, 5,       /* Addr. 82 (Adjective 1-5) */
         4, 6,       /* Addr. 83 (Adjective 2-1) */
         4, 7,       /* Addr. 84 (Adjective 2-2) */
         4, 8,       /* Addr. 85 (Adjective 2-3) */
         4, 9,       /* Addr. 86 (Adjective 2-4) */
         4,10,       /* Addr. 87 (Adjective 2-5) */
        56, 0,       /* Addr. 88 (Time Out) */
        11, 1,       /* Addr. 89 (Tax Mod 1) */
        11, 2,       /* Addr. 90 (Tax Mod 2) */
        11, 3,       /* Addr. 91 (Tax Mod 3) */
        11, 4,       /* Addr. 92 (Tax Mod 4) */
        11, 5,       /* Addr. 93 (Tax Mod 5) */
        11, 6,       /* Addr. 94 (Tax Mod 6) */
        11, 7,       /* Addr. 95 (Tax Mod 7) */
        11, 8,       /* Addr. 96 (Tax Mod 8) */
        11, 9,       /* Addr. 97 (Tax Mod 9) */
        11,10,       /* Addr. 98 (Tax Exempt 1) */
        11,11,       /* Addr. 99 (Tax Exempt 2) */
         3, 1,       /* Addr.100 (Print Mod 1) */
         3, 2,       /* Addr.101 (Print Mod 2) */
         3, 3,       /* Addr.102 (Print Mod 3) */
         3, 4,       /* Addr.103 (Print Mod 4) */
         3, 5,       /* Addr.104 (Print Mod 5) */
         3, 6,       /* Addr.105 (Print Mod 6) */
         3, 7,       /* Addr.106 (Print Mod 7) */
         3, 8,       /* Addr.107 (Print Mod 8) */
        39, 1,       /* Addr.108 (Charge Tips, V3.3) */
        24, 0,       /* Addr.109 (Tips Paid Out) */
        25, 0,       /* Addr.110 (Declared Tips) */
        12, 0,       /* Addr.111 (Sign-in) */
        15, 0,       /* Addr.112 (New Check) */
        17, 0,       /* Addr.113 (Guest Check #) */
         7, 8,       /* Addr.114 (Toatl 8) */
         7, 4,       /* Addr.115 (Toatl 4) */
        32, 0,       /* Addr.116 (E/C) */
        23, 0,       /* Addr.117 (Receipt/No Receipt) */
        30, 0,       /* Addr.118 (Cancel/Abort) */
        58, 0,       /* Addr.119 (Coupon Number) */
        37, 2,       /* Addr.120 (Print on Demand) */
        13, 0,       /* Addr.121 (B Key) */
        16, 0,       /* Addr.122 (PB) */
        19, 0,       /* Addr.123 (No. of Person) */
         7, 7,       /* Addr.124 (Total 7) */
         7, 3,       /* Addr.125 (Total 3) */
        34, 0,       /* Addr.126 (QTY) */
        31, 0,       /* Addr.127 (Item Void) */
        33, 0,       /* Addr.128 (Transaction Void) */
         6, 1,       /* Addr.129 (F/C 1) */
         6, 2,       /* Addr.130 (F/C 2) */
        28, 0,       /* Addr.131 (R/A) */
         0, 0,       /* Addr.132 (Server #) */
         8, 1,       /* Addr.133 (Add Check 1) */
        20, 0,       /* Addr.134 (Table #) */
         7, 6,       /* Addr.135 (Total 6) */
         7, 2,       /* Addr.136 (Total 2) */
         5, 7,       /* Addr.137 (Tender 7) */
         5, 8,       /* Addr.138 (Tender 8) */
        27, 0,       /* Addr.139 (P/O) */
        22, 0,       /* Addr.140 (Feed/Release) */
         8, 2,       /* Addr.141 (Add Check 2) */
        21, 0,       /* Addr.142 (Line #) */
         7, 5,       /* Addr.143 (Total 5) */
         7, 1,       /* Addr.144 (Total 1) */
         5, 5,       /* Addr.145 (Tender 5) */
         5, 6,       /* Addr.146 (Tender 6) */
        26, 0,       /* Addr.147 (No Sale) */
        29, 0,       /* Addr.148 (Change Computation) */
         8, 3,       /* Addr.149 (Add Check 3) */
        18, 0,       /* Addr.150 (Guest Check Transfer) */
         9, 2,       /* Addr.151 (Mod Item Discount) */
         9, 1,       /* Addr.152 (Item Discount) */
         5, 3,       /* Addr.153 (Tender 3) */
         5, 4,       /* Addr.154 (Tender 4) */
        36, 0,       /* Addr.155 (#/Type) */
        35, 0,       /* Addr.156 (Menu Shift) */
        52, 0,       /* Addr.157 (EJ Reverse) */
        10, 2,       /* Addr.158 (Regular Discount 2) */
        10, 1,       /* Addr.159 (Regular Discount 1) */
        40, 0,       /* Addr.160 (PLU No.) */
        45, 0,       /* Addr.161 (Decimal Point) */
        41, 0,       /* Addr.162 (00) */
         5, 1,       /* Addr.163 (Tender 1) */
         5, 2,       /* Addr.164 (Tender 2) */
        42, 0,        /* Addr.165 (A/C) */
				0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
    },
	//To Be added for TouchScreen Interface
	//Added Pages for TouchScreen Interface
	//When New FSC Codes are Added (Major and Minor FSC Codes)
	//they must be added to an empty location
	//on one of these pages as well as
	//to the PEP/Layout Manager Toolkit
    {   /* Touchscreen Page #1 Table Address Page #10*/
         3, 1,       /* Print Modifier 1 */
         3, 2,       /* Print Modifier 2 */
         3, 3,       /* Print Modifier 3 */
         3, 4,       /* Print Modifier 4 */
         3, 5,       /* Print Modifier 5 */
         3, 6,       /* Print Modifier 6 */
         3, 7,       /* Print Modifier 7 */
         3, 8,       /* Print Modifier 8 */
         4, 1,       /* Adjective 1 */
         4, 2,       /* Adjective 2 */
         4, 3,       /* Adjective 3 */
         4, 4,       /* Adjective 4 */
         4, 5,       /* Adjective 5 */
         4, 6,       /* Adjective 6 */
         4, 7,       /* Adjective 7 */
         4, 8,       /* Adjective 8 */
         4, 9,       /* Adjective 9 */
         4,10,       /* Adjective 10 */
         4,11,       /* Adjective 11 */
         4,12,       /* Adjective 12 */
         4,13,       /* Adjective 13 */
         4,14,       /* Adjective 14 */
         4,15,       /* Adjective 15 */
         4,16,       /* Adjective 16 */
         4,17,       /* Adjective 17 */
         4,18,       /* Adjective 18 */
         4,19,       /* Adjective 19 */
         4,20,       /* Adjective 20 */
         5, 1,       /* Tender 1 */
         5, 2,       /* Tender 2 */
         5, 3,       /* Tender 3 */
         5, 4,       /* Tender 4 */
         5, 5,       /* Tender 5 */
         5, 6,       /* Tender 6 */
         5, 7,       /* Tender 7 */
         5, 8,       /* Tender 8 */
         5, 9,       /* Tender 9 */
         7, 1,       /* Total 1 */
         7, 2,       /* Total 2 */
         7, 3,       /* Total 3 */
         7, 4,       /* Total 4 */
         7, 5,       /* Total 5 */
         7, 6,       /* Total 6 */
         7, 7,       /* Total 7 */
         7, 8,       /* Total 8 */
         7, 9,       /* Total 9 */
         8, 1,       /* Add Check 1 */
         8, 2,       /* Add Check 2 */
         8, 3,       /* Add Check 3 */
         9, 1,       /* Item Disc 1 */
         9, 2,       /* Item Disc 2 */
        10, 1,       /* Regular Disc 1 */
        10, 2,       /* Regular Disc 2 */
        10, 3,       /* Regular Disc 3 */
        10, 4,       /* Regular Disc 4 */
        10, 5,       /* Regular Disc 5 */
        10, 6,       /* Regular Disc 6 */
        11, 1,       /* Tax Modifier 1 */
        11, 2,       /* Tax Modifier 2 */
        11, 3,       /* Tax Modifier 3 */
        11, 4,       /* Tax Modifier 4 */
        11, 5,       /* Tax Modifier 5 */
        11, 6,       /* Tax Modifier 6 */
        11, 7,       /* Tax Modifier 7 */
        11, 8,       /* Tax Modifier 8 */
        11, 9,       /* Tax Modifier 9 */
        11,10,       /* Tax Modifier 10 */
        11,11,       /* Tax Modifier 11 */
        12, 0,       /* Sign In A */
        13, 0,       /* Sign In B */
        15, 0,       /* New Check */
        16, 0,       /* PB */
        17, 0,       /* Guest Check # */
        18, 0,       /* Guest Check Transfer */
        19, 0,       /* # of People */
        20, 0,       /* Table # */
        21, 0,       /* Line # */
        22, 0,       /* Feed/Release */
        23, 0,       /* Receipt/No Receipt */
        24, 0,       /* Tips Paid Out */
        25, 0,       /* Declare Tips */
        30, 0,       /* Cancel/Abort */
        31, 0,       /* Item Void */
        32, 0,       /* E/C */
        33, 0,       /* Transaction Void */
        34, 0,       /* Qty */
        36, 0,       /* #/Type */
        37, 1,       /* Print on Demand 1 */
        37, 2,       /* Print on Demand 2 */
        37, 3,       /* Print on Demand 3 */
        39, 1,       /* Charge Tips 1 */
        39, 2,       /* Charge Tips 2 */
        39, 3,       /* Charge Tips 3 */
        40, 0,       /* PLU No. */
        42, 0,       /* A/C */
        43, 0,       /* Recipt Feed */
        44, 0,       /* Journal Feed */
        46, 0,       /* Scale/Tare */
        52, 0,       /* Reverse Print */
        53, 1,       /* Preset Cash 1 */
        53, 2,       /* Preset Cash 2 */
        53, 3,       /* Preset Cash 3*/
        53, 4,       /* Preset Cash 4 */
        47, 0,       /* Offline Tender Modifier */
        55, 0,       /* Time In */
        56, 0,       /* Time Out */
        58, 0,       /* Coupon No. */
        59, 0,       /* Up */
        60, 0,       /* Down */
        61, 0,       /* Right */
        62, 0,       /* Left */
        63, 0,       /* Wait */
        64, 0,       /* Paid Order Recall */
        66, 0,       /* Control String No. */
        67, 0,       /* Alpha Entry */
        68, 0,       /* Left Display */
        69, 0,       /* Bartender Sign In */
        70, 0,       /* Split */
        71, 1,       /* Seat No. 1 */
        71, 2,       /* Seat No. 2 */
        72, 0,       /* Item Transfer */
        73, 0,       /* Repeat */
        74, 0,       /* Cursor Void */
        75, 0,       /* Price Check */
        78, 0,       /* Operator Interrupt A */
        79, 0,       /* Operator Interrupt B */
        49, 0,       /* Money */
        76, 1,       /* Ask 1 */
        76, 2,       /* Ask 2 */
        76, 3,       /* Ask 3 */
        76, 4,       /* Ask 4 */
        76, 5,       /* Ask 5 */
        76, 6,       /* Ask 6 */
        76, 7,       /* Ask 7 */
        76, 8,       /* Ask 8 */
        76, 9,       /* Ask 9 */
        76,10,       /* Ask 10 */
        76,11,       /* Ask 11 */
        76,12,       /* Ask 12 */
        76,13,       /* Ask 13 */
        76,14,       /* Ask 14 */
        76,15,       /* Ask 15 */
        76,16,       /* Ask 16 */
        76,17,       /* Ask 17 */
        76,18,       /* Ask 18 */
        76,19,       /* Ask 19 */
        76,20,       /* Ask 20 */
        80, 1,       /* Adjective Shift 1 */
        80, 2,       /* Adjective Shift 2 */
        80, 3,       /* Adjective Shift 3 */
        80, 4,       /* Adjective Shift 4 */
        80, 5,       /* Adjective Shift 5 */
        81, 0,       /* Department No. */
        82, 0,       /* E Version Modifer (UPC-E) */
        83, 0,       /* Price Enter */
        84, 0,       /* Food Stamp Modifier */
        85, 0,       /* WIC Transaction */
        86, 0,       /* WIC Modifier */
        26, 0,       /* No Sale */
        27, 0,       /* Paid Out */
        41, 0,       /* 00 */
        45, 0,       /* Decimal Point */
        28, 0,       /* Received on Account */
        29, 0,       /* Change Computation */
         6, 1,       /* Foreign Tender 1 */
         6, 2,       /* Foreign Tender 2 */
         6, 3,       /* Foreign Tender 3 */
         6, 4,       /* Foreign Tender 4 */
         6, 5,       /* Foreign Tender 5 */
         6, 6,       /* Foreign Tender 6 */
         6, 7,       /* Foreign Tender 7 */
         6, 8,       /* Foreign Tender 8 */
         5,10,       /* Tender 10 */
         5,11,       /* Tender 11 */
        87, 0,       /* HALO Override*/
        88, 0,       /* @/For */
        89, 0,       /* Manual Validation*/
		 5, 12,		 /* Tender 12*/
		 5, 13,		 /* Tender 13*/
		 5, 14,		 /* Tender 14*/
		 5, 15,		 /* Tender 15*/
		 5, 16,		 /* Tender 16*/
		 5, 17,		 /* Tender 17*/
		 5, 18,	     /* Tender 18*/
		 5, 19,		 /* Tender 19*/
		 5, 20,		 /* Tender 20*/
		 7, 10,      /* Total  10*/
		 7, 11,		 /* Total  11*/
		 7, 12,		 /* Total  12*/
		 7, 13,		 /* Total  13*/
		 7, 14,		 /* Total  14*/
		 7, 15,		 /* Total  15*/
		 7, 16,		 /* Total  16*/
		 7, 17,		 /* Total  17*/
		 7, 18,		 /* Total  18*/
		 7, 19,		 /* Total  19*/
		 7, 20,		 /* Total  20*/
		 37, 4,		 /* Print on Demand 4, Gift receipt JHHJ 9-01-05*/
		 9, 3,		 /* Item Discount 3*/
		 9, 4,		 /* Item Discount 4*/
		 9, 5,		 /* Item Discount 5*/
		 9, 6,		 /* Item Discount 6*/
		 92, 0,		 /* Edit Condiment*/
		 1, 1,		/*  Keyed PLU #1 (Touchscreen ONLY)*/
		 1, 2,		/*  Keyed PLU #2 (Touchscreen ONLY)*/
		 1, 3,		/*  Keyed PLU #3 (Touchscreen ONLY)*/
		 1, 4,		/*  Keyed PLU #4 (Touchscreen ONLY)*/
		 1, 5,		/*  Keyed PLU #5 (Touchscreen ONLY)*/
		 1, 6,		/*  Keyed PLU #6 (Touchscreen ONLY)*/
		 1, 7,		/*  Keyed PLU #7 (Touchscreen ONLY)*/
		 1, 8,		/*  Keyed PLU #8 (Touchscreen ONLY)*/
		 1, 9,		/*  Keyed PLU #9 (Touchscreen ONLY)*/
		 1, 10,		/*  Keyed PLU #10 (Touchscreen ONLY)*/
		 93, 0,		/*	Receipt ID (Tent)	***PDINU */
		 94, 1,		/*  Order Declaration 1*/
		 94, 2,		/*  Order Declaration 2*/
		 94, 3,		/*  Order Declaration 3*/
		 94, 4,		/*  Order Declaration 4*/
		 94, 5,		/*  Order Declaration 5*/
		 95, 0,		/*  Adjective Modifier */
		 96, 1,		/*	Gift Card 1*/
		 96, 2,		/*Freedom Pay */   /*	Gift Card 2*/
		 0, 0,		/*Free space */   /*	Gift Card 3*/
		 0, 0,		/*Free space */   /*	Gift Card 4*/
		 0, 0,		/*Free space */   /*	Gift Card 5*/
		 97, 0,     // entry to support FSC_SUPR_INTRVN and CID_SUPR_INTRVN
		 98, 0,     // entry to support FSC_DOC_LAUNCH
		 99, 0,     // entry to support FSC_OPR_PICKUP
		 100, 0,    /* EDIT CONDIMENT */
		 101, 0,    // FSC_WINDOW_DISPLAY, CWindowButton::BATypeDisplayLabeledWindowManual {0xFF, 0x27} 
		 102, 0,    // FSC_WINDOW_DISMISS, CWindowButton::BATypeDismissLabeledWindowManual {0xFF, 0x28} 
		 103, 0,    // Auto Sign Out: FSC_AUTO_SIGN_OUT, CWindowButton::BATypeAutoSignOut  {0xFF, 0x29} 
		 33, 1,     // Transaction Return: FSC_PRE_VOID 1, CWindowButton::BATypeTranVoid  {0xFF, 0x2A} 
		 33, 2,     // Transaction Return: FSC_PRE_VOID 2, CWindowButton::BATypeTranVoid  {0xFF, 0x2B} 
		 33, 3,     // Transaction Return: FSC_PRE_VOID 3, CWindowButton::BATypeTranVoid  {0xFF, 0x2C} 
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0
    },
    {       /* TouchScreen Page #2 Table Address (Page 11)*/
		0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
         0, 0,       /*  */
        35, 0,        /* Menu Shift NOTE: THIS MUST BE ON ALL TOUCHSCREEN PAGE TABLES  */
				0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0,
		 0, 0
    }
	//To Be added for TouchScreen Interface
	//End added pages for Touchscreen Interface
    },
	    0x0005, /* Prog.6 SECURITY NUMBER FOR PROGRAM MODE */
    {       /* Prog.7 STORE/REGISTER NO. */
		0, 0
    },
    {   /* Prog.8 SUPERVISOR NUMBER/LEVEL ASSINMENT */
		{801, 0},   /* Addr.1 */
		{803, 0},   /* Addr.2 */
		{855, 0},   /* Addr.3 */
		{801, 0},   /* Addr.4 */
		{802, 0},   /* Addr.5 */
		{803, 0},   /* Addr.6 */
		{804, 0},   /* Addr.7 */
		{805, 0}    /* Addr.8 */
    },
    {   /* Prog.9 ACTION CODE SECURITY */
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    },
    {   /* Prog.10 TRANSACTION HALO */
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0    /* 2172 */
    },
    {   /* Prog.17 HOURLY ACTIVITY TIME */
		{ 48, 0},        /* Addr. 1 */
		{  0, 0},        /* Addr. 2 */
		{  0,30},        /* Addr. 3 */
		{  1, 0},        /* Addr. 4 */
		{  1,30},        /* Addr. 5 */
		{  2, 0},        /* Addr. 6 */
		{  2,30},        /* Addr. 7 */
		{  3, 0},        /* Addr. 8 */
		{  3,30},        /* Addr. 9 */
		{  4, 0},        /* Addr. 10 */
		{  4,30},        /* Addr. 11 */
		{  5, 0},        /* Addr. 12 */
		{  5,30},        /* Addr. 13 */
		{  6, 0},        /* Addr. 14 */
		{  6,30},        /* Addr. 15 */
		{  7, 0},        /* Addr. 16 */
		{  7,30},        /* Addr. 17 */
		{  8, 0},        /* Addr. 18 */
		{  8,30},        /* Addr. 19 */
		{  9, 0},        /* Addr. 20 */
		{  9,30},        /* Addr. 21 */
		{ 10, 0},        /* Addr. 22 */
		{ 10,30},        /* Addr. 23 */
		{ 11, 0},        /* Addr. 24 */
		{ 11,30},        /* Addr. 25 */
		{ 12, 0},        /* Addr. 26 */
		{ 12,30},        /* Addr. 27 */
		{ 13, 0},        /* Addr. 28 */
		{ 13,30},        /* Addr. 29 */
		{ 14, 0},        /* Addr. 30 */
		{ 14,30},        /* Addr. 31 */
		{ 15, 0},        /* Addr. 32 */
		{ 15,30},        /* Addr. 33 */
		{ 16, 0},        /* Addr. 34 */
		{ 16,30},        /* Addr. 35 */
		{ 17, 0},        /* Addr. 36 */
		{ 17,30},        /* Addr. 37 */
		{ 18, 0},        /* Addr. 38 */
		{ 18,30},        /* Addr. 39 */
		{ 19, 0},        /* Addr. 40 */
		{ 19,30},        /* Addr. 41 */
		{ 20, 0},        /* Addr. 42 */
		{ 20,30},        /* Addr. 43 */
		{ 21, 0},        /* Addr. 44 */
		{ 21,30},        /* Addr. 45 */
		{ 22, 0},        /* Addr. 46 */
		{ 22,30},        /* Addr. 47 */
		{ 23, 0},        /* Addr. 48 */
		{ 23,30}         /* Addr. 49 */
    },
    {   /* Prog.18 SLIP PRINTER FEED CONTROL */
        55, /* Addr.1 */
         4, /* Addr.2 */
         0, /* Addr.3 */
        50, /* Addr.4 */
        20, /* Addr.5 */
		80  /* Addr.6 */
    },
    {   /* Prog.20 TRANSACTION MNEMONICS */
    { 'S','T','A','R','T',' ','T','M','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.  1 Period From */
    { 'E','N','D',' ','T','I','M','E','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.  2 Period To */
    { 'C','U','R','-','G','G','T','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},        /* Addr.  3 Current Gross */
    { 'T','X','B','L',' ','1','T','L','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.  4 Taxable #1 */
    { 'T','A','X',' ','1',' ','T','L','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.  5 Tax #1 */
    { 'E','X','E','M','P','T',' ','1','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.  6 Tax Exempt #1 */
    { 'T','X','B','L',' ','2','T','L','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.  7 Taxable #2 */
    { 'T','A','X',' ','2',' ','T','L','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.  8 Tax #2 */
    { 'E','X','E','M','P','T',' ','2','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.  9 Tax Exempt #2 */
    { 'T','X','B','L',' ','3','T','L','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 10 Taxable #3 */
    { 'T','A','X',' ','3',' ','T','L','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 11 Tax #3 */
    { 'E','X','E','M','P','T',' ','3','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 12 Tax Exempt #3 */
    { 'T','A','X','L',' ','4','T','L','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 13 Taxable #4 */
    { 'T','A','X',' ','4',' ','T','L','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 14 Tax #4 */
    { 'E','X','E','M','P','T',' ','4','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 15 Tax Exempt #4 */
    { 'P','S','T',' ','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},     /* Addr. 16 PST All Tax */
    { 'G','-','T','A','X','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},      /* Addr. 17 Consolidation Tax */
    { 'N','O','N',' ','T','X','B','L','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 18 Non Taxable */
    { 'T','R','A','I','N','I','N','G','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 19 Tarining Gross */
    { 'D','E','C','.','T','I','P','S','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 20 Declared Tips */
    { 'D','A','I','L','Y',' ','G','T','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 21 Daily Gross */
    { 'I','-','V','O','I','D','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},       /* Addr. 22 Plus Void */
    { 'T','R','-','V','O','I','D','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},        /* Addr. 23 Transaction Void */
    { 'C','-','C','O','U','P','O','N','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 24 Consolidation Coupon */
    { 'I','T','E','M','-','D','I','S','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 25 Item Discount */
    { 'I','-','D','I','S','-','M','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},        /* Addr. 26 Mod. Item Discount */
    { 'D','I','S','C','O','N','T','1','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 27 Regular Discount 1 */
    { 'D','I','S','C','O','N','T','2','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 28 Regular Discount 2 */
        { 'P','A','I','D',' ','O','U','T','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 29 Paid Out */
        { 'R','E','C',' ','A','C','C','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},        /* Addr. 30 R/A */
        { 'T','I','P','S',' ','P','/','O','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 31 Tips Paid Out */
        { 'C','A','S','H','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},     /* Addr. 32 Tender 1 */
        { 'C','H','E','C','K','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},      /* Addr. 33 Tender 2 */
        { 'C','H','A','R','G','E','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},       /* Addr. 34 Tender 3 */
        { 'M','I','S','C',' ','#','1','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},        /* Addr. 35 Tender 4 */
        { 'M','I','S','C',' ','#','2','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},        /* Addr. 36 Tender 5 */
        { 'M','I','S','C',' ','#','3','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},        /* Addr. 37 Tender 6 */
        { 'M','I','S','C',' ','#','4','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},        /* Addr. 38 Tender 7 */
        { 'M','I','S','C',' ','#','5','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},        /* Addr. 39 Tender 8 */
        { 'F','O','R','E','I','G','N','1','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 40 F/C 1 */
        { 'F','O','R','E','I','G','N','2','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 41 F/C 2 */
        { 'S','U','B','T','O','T','A','L','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 42 Total #1 */
        { 'T','O','T','A','L','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},      /* Addr. 43 Total #2 */
        { 'T','O','T','A','L',' ','#','3','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 44 Total #3 */
        { 'T','O','T','A','L',' ','#','4','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 45 Total #4 */
        { 'T','O','T','A','L',' ','#','5','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 46 Total #5 */
        { 'T','O','T','A','L',' ','#','6','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 47 Total #6 */
        { 'T','O','T','A','L',' ','#','7','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 48 Total #7 */
        { 'T','O','T','A','L',' ','#','8','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 49 Total #8 */
        { 'S','E','R',' ','T','L',' ','1','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 50 Service Total #1 */
        { 'S','E','R',' ','T','L',' ','2','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 51 Service Total #2 */
        { 'S','E','R',' ','T','L',' ','3','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 52 Service Total #3 */
        { 'A','D','D',' ','C','K',' ','1','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 53 Add Check Total #1 */
        { 'A','D','D',' ','C','K',' ','2','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 54 Add Check Total #2 */
        { 'A','D','D',' ','C','K',' ','3','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 55 Add Check Total #3 */
        { 'C','H','G',' ','T','I','P','S','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 56 Charge Tips */
        { 'C','A','N','C','E','L','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},       /* Addr. 57 Transaction Cancel */
        { 'C','A','N','C','E','L','E','D','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 58 Cancel Total */
        { 'M','-','V','O','I','D','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},       /* Addr. 59 Misc Void */
        { 'A','U','D','A','C','T','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},       /* Addr. 60 Audaction */
        { 'N','O',' ','S','A','L','E','S','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 61 No Sale */
        { 'P','R','O','D','U','C','T','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},        /* Addr. 62 Item Productivity */
        { 'P','E','R','S','O','N','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},       /* Addr. 63 No. of Person */
        { 'O','P','E','N','E','D','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},       /* Addr. 64 No. of Checks Opened */
        { 'C','L','O','S','E','D','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},       /* Addr. 65 No. of Checks Closed */
        { 'C','U','S','T','O','M','E','R','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 66 Customer Counter */
        { 'H','A','S','H',' ','D','P','T','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 67 Hash Dept */
        { 'B','O','N','U','S',' ','#','1','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 68 Bonus #1 */
        { 'B','O','N','U','S',' ','#','2','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 69 Bonus #2 */
        { 'B','O','N','U','S',' ','#','3','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 70 Bonus #3 */
        { 'B','O','N','U','S',' ','#','4','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 71 Bonus #4 */
        { 'B','O','N','U','S',' ','#','5','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 72 Bonus #5 */
        { 'B','O','N','U','S',' ','#','6','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 73 Bonus #6 */
        { 'B','O','N','U','S',' ','#','7','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 74 Bonus #7 */
        { 'B','O','N','U','S',' ','#','8','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 75 Bonus #8 */
        { 'T','O','T','A','L','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},      /* Addr. 76 Total for Report */
        { 'S','U','B','T','O','T','A','L','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 77 Sub Total for Report */
        { 'O','U','T',' ','S','T','A','N','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 78 Outstanding for Report */
        { 'A','D','D','I','T','I','O','N','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 79 Addition */
        { 'D','E','L','E','T','I','O','N','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 80 Deletion */
        { 'C','H','A','N','G','E','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},       /* Addr. 81 Change(Maint) */
        { 'S','A','L','E','/','P','E','R','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 82 Net Sales/Person */
        { 'A','B','O','R','T','E','D','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},        /* Addr. 83 Abort */
        { 'A','M','T','.','T','L','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},       /* Addr. 84 Amount Total */
        { 'D','E','M','A','N','D','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},       /* Addr. 85 Print on Demand */
        { 'G','U','E','S','T',' ','#','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},        /* Addr. 86 Guest Check No. */
        { 'P','.','B','.','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},     /* Addr. 87 P.B. */
        { 'C','H','K',' ','P','A','I','D','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 88 Check Paid */
        { 'E','R','R','.','C','O','R','.','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 89 E/C */
        { '#','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr. 90 Number */
        { ' ',' ',' ',' ',' ',' ',' ',' ','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 91 Reserved */
        { 'T','A','X',' ','S','U','M','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},        /* Addr. 92 Tax Sum */
        { 'C','H','K',' ','S','U','M','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},        /* Addr. 93 Check Sum */
        { 'G','-','F','U','L','L','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},       /* Addr. 94 Reserved */
        { 'G','-','S','T','A','R','T','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},        /* Addr. 95 Time Opened */
        { 'E','L','A','P','S','E','D','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},        /* Addr. 96 Elapsed Time */
        { 'B','A','L','A','N','C','E','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},        /* Addr. 97 Current Balance */
        { 'T','R','A','N','S',' ','T','L','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 98 Transferred Balance */
        { 'O','-','O','P','E','R','A','T','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr. 99 Old Server No. */
        { 'C','H','K',' ','T','O','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},       /* Addr.100 Checks Transferred To */
        { 'C','H','K',' ','F','R','O','M','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.101 Checks Transferred From */
        { 'O','P','E','-','T','T','L','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},        /* Addr.102 Server Total */
        { 'C','A','L',' ','T','I','P','S','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.103 Caluculate Tips */
        { 'Q','U','A','N','T','I','T','Y','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.104 Quantity */
        { 'D','O','U','B','L','E','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},       /* Addr.105 Double Receipt ID */
        { 'C','H','A','N','G','E','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},       /* Addr.106 Change */
        { '(','-',')','T','O','T','A','L','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.107 Minus Total for Report */
        { 'G','-','T','O','T','A','L','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},        /* Addr.108 Grand Total for Report */
        { 'H','A','S','H',' ','T','T','L','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.109 Hash Total for Report */
        { 'M','A','N','U','A','L','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},       /* Addr.110 Manual(Scale) */
        { 'T','R','A','Y',' ','C','O','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},        /* Addr.111 Tray Counter */
        { 'T','I','M','E','-','I','N','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},        /* Addr.112 Time-In(ETK) */
        { 'T','I','M','E','-','O','U','T','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.113 Time-Out(ETK) */
        { 'P','O','S','T',' ','R','E','C','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.114 Post Receipt */
        { 'M','I','S','C',' ','#','6','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},        /* Addr.115 Tender 9 */
        { 'M','I','S','C',' ','#','7','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},        /* Addr.116 Tender 10 */
        { 'M','I','S','C',' ','#','8','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},        /* Addr.117 Tender 11 */
        { 'O','F','F',' ','T','E','N','D','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.118 off line tender */
        { 'O','F','F',' ','C','H','G',' ','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.119 off line charge posting */
        { 'O','F','F',' ','A','U','T',' ','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.120 off line authorization */
        { 'P','R','E',' ','A','U','T','H','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.121 pre-authorization total */
        { 'C','O','M','B','-','C','P','N','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.122 Conbination Coupon */
        { 'P','A','R','K','I','N','G',' ','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.123 Parking Receipt */
        { 'R','E','C','A','L','L',' ',' ','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.124 Recall */
        { 'D','I','S','C','O','N','T','3','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.125 Regular Discount 3 */
        { 'D','I','S','C','O','N','T','4','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.126 Regular Discount 4 */
        { 'D','I','S','C','O','N','T','5','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.127 Regular Discount 5 */
        { 'D','I','S','C','O','N','T','6','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.128 Regular Discount 6 */
        { 'N','E','T',' ','T','T','L',' ','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.129 Net Total */
        { 'S','P','L','I','T',' ',' ',' ','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.130 Split Key */
        { 'L','A','B','O','R',' ','T','L','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.131 Labor Cost Total */
        { 'L','-','C','O','S','T',' ','%','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.132 Labor Cost % */
        { 'O','P','E',' ','I','N','T',' ','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.133 Cashier Interrupt, R3.3 */
        { 'C','H','G',' ','T','I','P','2','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.134 Charge Tips #2, V3.3 */
        { 'C','H','G',' ','T','I','P','3','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.135 Charge Tips #5, V3.3 */
        { 'W','A','I','T','I','N','G',' ','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.136 Waiting Message, V3.3 */
        { 'C','O','N','T','I','N','U','E','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.137 Continue Message, V3.3 */
        { 'M','N','E','M','O','N','I','C','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.138 Mneminic, V3.3 */
        { 'T','A','X',' ','R','A','T','E','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.139 Tax Rate, V3.3 */
        { 'T','A','X',' ',' ',' ',' ',' ','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.140 Tax, V3.3 */
        { 'J','O','B',' ',' ',' ',' ',' ','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.141 Job, V3.3 */
        { 'B','L','O','C','K',' ',' ',' ','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.142 Block, V3.3 */
        { 'D','A','T','E',' ',' ',' ',' ','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.143 Date, V3.3 */
        { 'F','O','R','E','I','G','N','3','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.144 F/C 3, 2172 */
        { 'F','O','R','E','I','G','N','4','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.145 F/C 4, 2172 */
        { 'F','O','R','E','I','G','N','5','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.146 F/C 5, 2172 */
        { 'F','O','R','E','I','G','N','6','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.147 F/C 6, 2172 */
        { 'F','O','R','E','I','G','N','7','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.148 F/C 7, 2172 */
        { 'F','O','R','E','I','G','N','8','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.149 F/C 8, 2172 */
        { 'L','O','A','N',' ',' ',' ',' ','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.150 Loan, 2172 */
        { 'P','I','C','K','U','P',' ',' ','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.151 Pickup, 2172 */
        { 'O','N','H','A','N','D',' ',' ','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.152 On Hand Total, 2172 */
        { 'O','V','R','/','S','H','R','T','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.153 Over/Short, 2172 */
        { 'C','U','N','S','U','M','E','R','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.154 Cunsumer Number, 2172 */
        { 'B','U','I','L','D','I','N','G','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.155 PLU Building, 2172 */
        { 'S','K','U',' ','N','O',' ',' ','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.156 SKU Number, 2172 */
        { 'D','A','T','A','L','O','S','T','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.157 Power Down Log, Saratoga */

/* ### MOD Saratoga (052200) */
        { 'F','S',' ','T','O','T','A','L','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.158 Food Stamp Total  */
        { 'T','O','T','A','L',' ','#','9','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.159 Total #9          */
        { 'F','S','C','H','A','N','G','E','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.160 Food Stamp Change */
        /*** NCR2172***/
        { 'F','S','C','R','E','D','I','T','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},          /* Addr.161 Food Stamp Credit    */
        { 'F','S','T','E','X','P','T','1','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.162 FS Tax Exempt #1 Total */
        { 'F','S','T','E','X','P','T','2','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.163 FS Tax Exempt #2 Total */
        { 'F','S','T','E','X','P','T','3','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.164 FS Tax Exempt #3 Total */
        { 'U','P','C','-','C','P','N','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},         /* Addr.165 UPC Coupon           */
/* ### */

        { '@','/','F','O','R',' ',' ',' ','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},			/* Addr.166 @/For */
        { 'H','A','L','O',' ','O','V','R','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},			/* Addr.167 HALO Override */
        { 'O','V','R','L','I','M','I','T','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},	 /* Addr.168 Drawer Over Limit */
        { 'T','E','N','D','E','R',' ','1','2','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.169 Tender 12*/
        { 'T','E','N','D','E','R',' ','1','3','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.170 Tender 13*/
        { 'T','E','N','D','E','R',' ','1','4','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.171 Tender 14 */
        { 'T','E','N','D','E','R',' ','1','5','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.172 Tender 15 */
        { 'T','E','N','D','E','R',' ','1','6','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.173 Tender 16 */
        { 'T','E','N','D','E','R',' ','1','7','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.174 Tender 17 */
        { 'T','E','N','D','E','R',' ','1','8','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.175 Tender 18 */
        { 'T','E','N','D','E','R',' ','1','9','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.176 Tender 19 */
        { 'T','E','N','D','E','R',' ','2','0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.177 Tender 20 */
        { 'T','O','T','A','L',' ','1','0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.178 Total 10 */
        { 'T','O','T','A','L',' ','1','1','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.179 Total 11 */
        { 'T','O','T','A','L',' ','1','2','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.180 Total 12 */
        { 'T','O','T','A','L',' ','1','3','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.181 Total 13 */
        { 'T','O','T','A','L',' ','1','4','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.182 Total 14 */
        { 'T','O','T','A','L',' ','1','5','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.183 Total 15 */
        { 'T','O','T','A','L',' ','1','6','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.184 Total 16 */
        { 'T','O','T','A','L',' ','1','7','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.185 Total 17 */
        { 'T','O','T','A','L',' ','1','8','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.186 Total 18 */
        { 'T','O','T','A','L',' ','1','9','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.187 Total 19 */
        { 'T','O','T','A','L',' ','2','0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.188 Total 20 */
        { 'P','R','I','C','E',' ','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.189 Price */
        { 'N','O','N','E','/','D','O','N','E','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.190 None/Done(OEP) */
        { 'B','A','C','K','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.191 Back (OEP) */
        { _T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},  /* Addr.192 TRN_ITEMCOUNT_ADR */
        { _T('M'),_T('E'),_T('R'),_T('C'),_T('H'),_T('A'),_T('N'),_T('T'), _T(' '), _T('C'), _T('O'), _T('P'), _T('Y'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},               /* Addr.193 TRN_MERC_CPY_ADR */
        { _T('C'),_T('U'),_T('S'),_T('T'),_T('O'),_T('M'),_T('E'),_T('R'), _T(' '), _T('C'), _T('O'), _T('P'), _T('Y'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},               /* Addr.194 TRN_CUST_CPY_ADR */
        { _T('T'),_T('I'),_T('P'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},     /* Addr.195 TRN_TIP_LINE_ADR */
        { _T('T'),_T('O'),_T('T'),_T('A'),_T('L'),_T('\0'),_T('\0'),_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},       /* Addr.196 TRN_TTL_LINE_ADR */
        { _T('G'),_T('I'),_T('F'),_T('T'),_T(' '),_T('R'),_T('E'),_T('C'), _T('E'), _T('I'), _T('P'), _T('T'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},              /* Addr.197 TRN_GIFT_RECEIPT_ADR */
        { _T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},  /* Addr.198 TRN_OPTIMIZE_FILE_ADR */
		{ _T('D'),_T('I'),_T('S'),_T('C'),_T('O'),_T('U'),_T('N'),_T('T'), _T(' '), _T('3'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},            /* Addr.199 TRN_ITMDISC_ADR_3 */
		{ _T('D'),_T('I'),_T('S'),_T('C'),_T('O'),_T('U'),_T('N'),_T('T'), _T(' '), _T('4'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},            /* Addr.200 TRN_ITMDISC_ADR_4 */
		{ _T('D'),_T('I'),_T('S'),_T('C'),_T('O'),_T('U'),_T('N'),_T('T'), _T(' '), _T('5'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},            /* Addr.201 TRN_ITMDISC_ADR_5 */
		{ _T('D'),_T('I'),_T('S'),_T('C'),_T('O'),_T('U'),_T('N'),_T('T'), _T(' '), _T('6'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},            /* Addr.202 TRN_ITMDISC_ADR_6 */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('0'), _T('9'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.203 TRN_BNS_9_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('1'), _T('0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.204 TRN_BNS_10_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('1'), _T('1'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.205 TRN_BNS_11_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('1'), _T('2'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.206 TRN_BNS_12_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('1'), _T('3'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.207 TRN_BNS_13_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('1'), _T('4'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.208 TRN_BNS_14_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('1'), _T('5'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.209 TRN_BNS_15_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('1'), _T('6'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.210 TRN_BNS_16_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('1'), _T('7'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.211 TRN_BNS_17_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('1'), _T('8'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.212 TRN_BNS_18_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('2'), _T('9'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.213 TRN_BNS_19_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('2'), _T('0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.214 TRN_BNS_20_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('2'), _T('1'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.215 TRN_BNS_21_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('2'), _T('2'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.216 TRN_BNS_22_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('2'), _T('3'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.217 TRN_BNS_23_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('2'), _T('4'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.218 TRN_BNS_24_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('2'), _T('5'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.219 TRN_BNS_25_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('2'), _T('6'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.220 TRN_BNS_26_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('2'), _T('7'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.221 TRN_BNS_27_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('2'), _T('8'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.222 TRN_BNS_28_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('3'), _T('9'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.223 TRN_BNS_29_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('3'), _T('0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.224 TRN_BNS_30_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('3'), _T('1'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.225 TRN_BNS_31_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('3'), _T('2'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.226 TRN_BNS_32_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('3'), _T('3'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.227 TRN_BNS_33_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('3'), _T('4'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.228 TRN_BNS_34_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('3'), _T('5'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.229 TRN_BNS_35_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('3'), _T('6'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.230 TRN_BNS_36_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('3'), _T('7'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.231 TRN_BNS_37_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('3'), _T('8'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.232 TRN_BNS_38_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('3'), _T('9'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.233 TRN_BNS_39_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('4'), _T('0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.234 TRN_BNS_40_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('4'), _T('1'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.235 TRN_BNS_41_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('4'), _T('2'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.236 TRN_BNS_42_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('4'), _T('3'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.237 TRN_BNS_43_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('4'), _T('4'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.238 TRN_BNS_44_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('4'), _T('5'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.239 TRN_BNS_45_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('4'), _T('6'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.240 TRN_BNS_46_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('4'), _T('7'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.241 TRN_BNS_47_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('4'), _T('8'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.242 TRN_BNS_48_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('4'), _T('9'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.243 TRN_BNS_49_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('5'), _T('0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.244 TRN_BNS_50_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('5'), _T('1'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.245 TRN_BNS_51_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('5'), _T('2'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.246 TRN_BNS_52_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('5'), _T('3'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.247 TRN_BNS_53_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('5'), _T('4'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.248 TRN_BNS_54_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('5'), _T('5'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.249 TRN_BNS_55_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('5'), _T('6'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.250 TRN_BNS_56_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('5'), _T('7'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.251 TRN_BNS_57_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('5'), _T('8'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.252 TRN_BNS_58_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('5'), _T('9'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.253 TRN_BNS_59_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('6'), _T('0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.254 TRN_BNS_60_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('6'), _T('1'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.255 TRN_BNS_61_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('6'), _T('2'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.256 TRN_BNS_62_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('6'), _T('3'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.257 TRN_BNS_63_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('6'), _T('4'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.258 TRN_BNS_64_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('6'), _T('5'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.259 TRN_BNS_65_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('6'), _T('6'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.260 TRN_BNS_66_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('6'), _T('7'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.261 TRN_BNS_67_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('6'), _T('8'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.262 TRN_BNS_68_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('6'), _T('9'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.263 TRN_BNS_69_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('7'), _T('0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.264 TRN_BNS_70_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('7'), _T('1'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.265 TRN_BNS_71_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('7'), _T('2'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.266 TRN_BNS_72_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('7'), _T('3'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.267 TRN_BNS_73_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('7'), _T('4'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.268 TRN_BNS_74_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('7'), _T('5'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.269 TRN_BNS_75_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('7'), _T('6'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.270 TRN_BNS_76_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('7'), _T('7'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.271 TRN_BNS_77_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('7'), _T('8'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.272 TRN_BNS_78_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('7'), _T('9'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.273 TRN_BNS_79_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('8'), _T('0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.274 TRN_BNS_80_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('8'), _T('1'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.275 TRN_BNS_81_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('8'), _T('2'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.276 TRN_BNS_82_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('8'), _T('3'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.277 TRN_BNS_83_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('8'), _T('4'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.278 TRN_BNS_84_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('8'), _T('5'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.279 TRN_BNS_85_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('8'), _T('6'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.280 TRN_BNS_86_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('8'), _T('7'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.281 TRN_BNS_87_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('8'), _T('8'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.282 TRN_BNS_88_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('8'), _T('9'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.283 TRN_BNS_89_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('9'), _T('0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.284 TRN_BNS_90_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('9'), _T('1'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.285 TRN_BNS_91_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('9'), _T('2'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.286 TRN_BNS_92_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('9'), _T('3'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.287 TRN_BNS_93_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('9'), _T('4'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.288 TRN_BNS_94_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('9'), _T('5'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.289 TRN_BNS_95_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('9'), _T('6'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.290 TRN_BNS_96_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('9'), _T('7'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.291 TRN_BNS_97_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('9'), _T('8'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.292 TRN_BNS_98_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('9'), _T('9'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                /* Addr.293 TRN_BNS_99_ADR */
        { _T('B'),_T('O'),_T('N'),_T('U'),_T('S'),_T(' '),_T('T'),_T('O'), _T('T'), _T('A'), _T('L'), _T(' '), _T('1'), _T('0'), _T('0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                 /* Addr.294 TRN_BNS_100_ADR */
        { _T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},  /* Addr.295 TRN_PPI_ADD_SETTING */
        { _T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},  /* Addr.296 TRN_PPI_ADD_VALUE */
        { _T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},  /* Addr.297 TRN_PPI_BEFORE_QTY */
        { _T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},  /* Addr.298 TRN_PPI_AFTER_QTY */
        { _T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},  /* Addr.299 TRN_PPI_MINSALE */
        { _T('R'),_T('E'),_T('C'),_T('E'),_T('I'),_T('P'),_T('T'),_T(' '), _T('I'), _T('D'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},            /* Addr.300 TRN_TENT */
        { _T('O'),_T('R'),_T('D'),_T('E'),_T('R'),_T(' '),_T('D'),_T('E'), _T('C'), _T('L'), _T('A'), _T('R'), _T('E'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},               /* Addr.301 TRN_ORDER_DEC_OFFSET */
        { _T('O'),_T('R'),_T('D'),_T('E'),_T('R'),_T(' '),_T('D'),_T('E'), _T('C'), _T('L'), _T('A'), _T('R'), _T('E'), _T(' '), _T('2'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                 /* Addr.302 TRN_ORDER_DEC2 */
        { _T('O'),_T('R'),_T('D'),_T('E'),_T('R'),_T(' '),_T('D'),_T('E'), _T('C'), _T('L'), _T('A'), _T('R'), _T('E'), _T(' '), _T('3'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                 /* Addr.303 TRN_ORDER_DEC3 */
        { _T('O'),_T('R'),_T('D'),_T('E'),_T('R'),_T(' '),_T('D'),_T('E'), _T('C'), _T('L'), _T('A'), _T('R'), _T('E'), _T(' '), _T('4'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                 /* Addr.304 TRN_ORDER_DEC4 */
        { _T('O'),_T('R'),_T('D'),_T('E'),_T('R'),_T(' '),_T('D'),_T('E'), _T('C'), _T('L'), _T('A'), _T('R'), _T('E'), _T(' '), _T('5'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},                 /* Addr.305 TRN_ORDER_DEC5 */
        { _T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},  /* Addr.306 Dummy */
        { _T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},  /* Addr.307 Dummy */
        { _T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},  /* Addr.308 Dummy */
        { _T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},  /* Addr.309 Dummy */
        { _T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},  /* Addr.310 Dummy */
        { _T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},  /* Addr.311 Dummy */
        { _T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},  /* Addr.312 Dummy */
        { _T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},  /* Addr.313 Dummy */
        { _T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},  /* Addr.314 Dummy */
        { _T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},  /* Addr.315 Dummy */
        { _T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},  /* Addr.316 Dummy */
        { _T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},  /* Addr.317 Dummy */
        { _T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},  /* Addr.318 Dummy */
        { _T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},  /* Addr.319 Dummy */
        { _T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},  /* Addr.320 Dummy */
        { _T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},  /* Addr.321 Dummy */
        { _T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},  /* Addr.322 Dummy */
        { _T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},  /* Addr.323 Dummy */
        { _T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},  /* Addr.324 Dummy */
        { _T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},  /* Addr.325 Dummy */
        { _T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},  /* Addr.326 Dummy */
        { _T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},  /* Addr.327 Dummy */
        { _T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},  /* Addr.328 Dummy */
        { _T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},  /* Addr.329 Dummy */
        { _T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},  /* Addr.330 Dummy */
        { _T('S'),_T('I'),_T('G'),_T('N'),_T('-'),_T('I'),_T('N'),_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},         /* Addr.331 TRN_SIGNIN_ADR */
        { _T('S'),_T('I'),_T('G'),_T('N'),_T('-'),_T('O'),_T('U'),_T('T'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},          /* Addr.332 TRN_SIGNOUT_ADR */
        { _T('T'),_T('R'),_T('A'),_T('S'),_T(' '),_T('R'),_T('E'),_T('T'), _T('U'), _T('R'), _T('N'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},             /* Addr.333 TRN_TRETURN_ADR */
        { _T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},  /* Addr.334 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.196 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.197 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.198 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.199 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.200 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.191 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.192 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.193 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.194 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.195 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.196 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.197 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.198 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.199 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.200 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.191 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.192 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.193 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.194 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.195 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.196 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.197 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.198 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.199 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.200 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.191 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.192 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.193 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.194 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.195 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.196 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.197 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.198 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.199 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.200 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.191 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.192 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.193 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.194 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.195 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.196 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.197 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.198 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.199 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.200 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.191 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.192 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.193 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.194 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.195 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.196 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.197 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.198 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.199 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.200 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.191 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.192 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.193 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.194 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.195 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.196 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.197 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.198 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},  /* Addr.199 Dummy */
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}  /* Addr.200 Dummy */


    },
    {   /* Prog.21 LEAD THROUGH TABLE, CLASS_PARALEADTHRU, ParaLeadThru[MAX_LEAD_NO][PARA_LEADTHRU_LEN] */
		// string template for max length if adding mnemonics
		//_T("12345678901234567890"
        { _T("NOT ON FILE")},          // Addr. 1: LDT_NTINFL_ADR, IDS_P21_LDT_DEFAULT_001   - Not in File  
        { _T("FILE FULL")},            // Addr. 2: LDT_FLFUL_ADR, IDS_P21_LDT_DEFAULT_002    - File Full
        { _T("KITCHEN PR ERROR")},     // Addr. 3: LDT_KTNERR_ADR, IDS_P21_LDT_DEFAULT_003   - Kitchen Printer Error
        { _T("JOURNAL LOW")},          // Addr. 4: LDT_JNLLOW_ADR, IDS_P21_LDT_DEFAULT_004   - Journal Low 
        { _T("PLS CHK PRINTER")},      // Addr. 5: LDT_PRTTBL_ADR, IDS_P21_LDT_DEFAULT_005   - Printer Trouble 
        { _T("MASTER BUSY")},          // Addr. 6: LDT_LNKDWN_ADR, IDS_P21_LDT_DEFAULT_006   - Link Down 
        { _T("B-MASTER BUSY")},        // Addr. 7: LDT_SYSBSY_ADR, IDS_P21_LDT_DEFAULT_007   - System Busy 
        { _T("ILLEGAL DATA")},         // Addr. 8: LDT_KEYOVER_ADR, IDS_P21_LDT_DEFAULT_008  - Illegal Data
        { _T("SEQUENCE ERROR")},       // Addr. 9: LDT_SEQERR_ADR, IDS_P21_LDT_DEFAULT_009   - Key Sequence Error 
        { _T("PROHIBITED")},           // Addr. 10: LDT_PROHBT_ADR, IDS_P21_LDT_DEFAULT_010  - Prohibit Operation 
        { _T("TAKE TOTAL B-FUL")},     // Addr. 11: LDT_TAKETL_ADR, IDS_P21_LDT_DEFAULT_011  - Take Total for Transaction Over 
        { _T("GUEST NOT EMPTY")},      // Addr. 12: LDT_GCFNTEMPT_ADR, IDS_P21_LDT_DEFAULT_012 - Guest Check File is not Empty 
        { _T("TL/CO ARE ACTIVE")},     // Addr. 13: LDT_TLCOACT_ADR, IDS_P21_LDT_DEFAULT_013 - TL/CO are Active 
        { _T("OPERATOR OPENED")},      // Addr. 14: LDT_OPNWTIN_ADR, IDS_P21_LDT_DEFAULT_014 - Opened Operator
        { _T("C/S FILE ERROR")},       // Addr. 15: LDT_GCFWFERR_ADR, IDS_P21_LDT_DEFAULT_015 - GCF/Server not be Accessed

        /* Addr. 16: LDT_NOUNMT_ADR - Operator # Unmatch */
        { 'C','/','S',' ','#',' ','I','S',' ','U','N','M','A','T','C','H','\0','\0','\0','\0'},

        /* Addr. 17: LDT_SAMEGCNO_ADR - Same Check # */
        { 'D','O','U','B','L','E',' ','G','U','E','S','T',' ','#','\0','\0','\0','\0','\0','\0'},

        /* Addr. 18: LDT_ILLEGLNO_ADR - Illegal Guest Check # */
        { 'I','L','L','E','G','A','L',' ','G','U','E','S','T',' ','#','\0','\0','\0','\0','\0'},

        /* Addr. 19: LDT_DRAWCLSE_ADR - Request Drawer Closing */
        { 'C','L','O','S','E',' ','D','R','A','W','E','R','\0','\0','\0','\0','\0','\0','\0','\0'},

        /* Addr. 20: LDT_GCFSUNMT_ADR - Guest Check Status Unmatch */
        { 'G','U','E','S','T',' ','S','T','A','T','U','S',' ','N','G','\0','\0','\0','\0','\0'},

        /* Addr. 21: LDT_ERR_ADR - Global Error */
        { 'E','R','R','O','R','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},

        /* Addr. 22: LDT_LOCK_ADR - During Lock */
        { 'D','U','R','I','N','G',' ','L','O','C','K','\0','\0','\0','\0','\0','\0','\0','\0','\0'},

        /* Addr. 23: LDT_NOT_ISSUED_ADR - Saved Block has not been Printed Out */
        { 'D','O',' ','T','Y','P','E','1',' ','R','P','T',' ','1','S','T','\0','\0','\0','\0'},

        /* Addr. 24: LDT_DEPTNO_ADR - Dept. # Entry */
        { 'E','N','T',' ','D','E','P','T','.',' ','N','U','M','B','E','R','\0','\0','\0','\0'},

        /* Addr. 25: LDT_DEPTCTL_ADR - Copy Dept. Control Code */
        { 'E','N','T',' ','C','O','N','T','R','O','L',' ','T','Y','P','E','\0','\0','\0','\0'},

        /* Addr. 26 Func. Type Entry */
        { 'E','N','T',' ','F','U','N','C','T','I','O','N',' ','T','Y','P','\0','\0','\0','\0'},

        /* Addr. 27 Report Type Entry */
        { 'E','N','T','E','R',' ','R','E','P','O','R','T',' ','T','Y','P','\0','\0','\0','\0'},

        /* Addr. 28 Number Entry */
        { 'E','N','T','E','R',' ','N','U','M','B','E','R','\0','\0','\0','\0','\0','\0','\0','\0'},

        /* Addr. 29 PLU Type Entry */
        { 'E','N','T','E','R',' ','P','L','U',' ','T','Y','P','E','#','C','\0','\0','\0','\0'},

        /* Addr. 30 Request Delete Func. */
        { 'N','E','E','D',' ','D','E','L','E','T','E',' ','F','U','N','C','\0','\0','\0','\0'},

        /* Addr. 31 Report Code # Entry */
        { 'E','N','T',' ','R','E','P','O','R','T',' ','C','O','D','E','\0','\0','\0','\0','\0'},

        /* Addr. 32 Status Entry */
        { 'E','N','T','E','R',' ','S','T','A','T','U','S','\0','\0','\0','\0','\0','\0','\0','\0'},

        /* Addr. 33 Preset Price Entry */
        { 'E','N','T','E','R',' ','P','R','E',' ','P','R','I','C','E','\0','\0','\0','\0','\0'},

        /* Addr. 34 Major Dept. No. Entry */
        { 'E','N','T','E','R',' ','M','A','J','O','R',' ','#','\0','\0','\0','\0','\0','\0','\0'},

        /* Addr. 35 HALO Data */
        { 'E','N','T','E','R',' ','H','A','L','O','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},

        /* Addr. 36 Reset OK ? */
        { 'R','E','S','E','T',' ','O','K','?','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},

        /* Addr. 37 Operator # Entry */
        { 'E','N','T',' ','I','D',' ','#','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},

        /* Addr. 38 Sup. Int. */
        { 'N','E','E','D',' ','S','U','P','E','R','V','I','S','O','R','\0','\0','\0','\0','\0'},

        /* Addr. 39 Insert Paper for Val. */
        { 'I','N','S','E','R','T',' ','P','A','P','E','R','\0','\0','\0','\0','\0','\0','\0','\0'},

        /* Addr. 40 Remove Paper for Val. */
        { 'R','E','M','O','V','E',' ','P','A','P','E','R','\0','\0','\0','\0','\0','\0','\0','\0'},

        /* Addr. 41 ID # Entry */
        { 'E','N','T',' ','O','P','E','R','A','T','O','R',' ','I','D','#','\0','\0','\0','\0'},

        /* Addr. 42 A/C Entry */
        { 'E','N','T','E','R',' ','A','/','C',' ','#','\0','\0','\0','\0','\0','\0','\0','\0','\0'},

        /* Addr. 43 # Entry or A/C Key */
        { 'N','E','X','T',' ','#',' ','O','R',' ','F','I','N','A','L','\0','\0','\0','\0','\0'},

        /* Addr. 44 PM/Cond. Entry */
        { 'N','E','E','D',' ','P','M',' ','O','R',' ','C','O','N','D','\0','\0','\0','\0','\0'},

        /* Addr. 45 Enter Server # */
        { 'E','N','T','E','R',' ','S','E','R','V','E','R',' ','#','\0','\0','\0','\0','\0','\0'},

        /* Addr. 46 Progress Transaction */
        { 'P','R','O','G','R','E','S','S',' ','T','R','A','N','S','.','\0','\0','\0','\0','\0'},

        /* Addr. 47 Enter No. of Person */
        { 'N','E','E','D',' ','#',' ','O','F',' ','P','E','R','S','O','N','\0','\0','\0','\0'},

        /* Addr. 48 Enter Table No. */
        { 'N','E','E','D',' ','T','A','B','L','E',' ','#','\0','\0','\0','\0','\0','\0','\0','\0'},

        /* Addr. 49 Insert Paper for Slip */
        { 'I','N','S','E','R','T',' ','S',' ','P','A','P','E','R','\0','\0','\0','\0','\0','\0'},

        /* Addr. 50 Change Paper for Slip */
        { 'C','H','A','N','G','E',' ','S',' ','P','A','P','E','R','\0','\0','\0','\0','\0','\0'},

        /* Addr. 51 Line # Entry */
        { 'N','E','E','D',' ','L','I','N','E',' ','#','\0','\0','\0','\0','\0','\0','\0','\0','\0'},

        /* Addr. 52 Data Entry */
        { 'E','N','T','E','R',' ','D','A','T','A','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},

        /* Addr. 53 A/C or Abort Entry */
        { 'A','/','C',' ','O','R',' ','A','B','O','R','T','\0','\0','\0','\0','\0','\0','\0','\0'},

        /* Addr. 54 Select Daily or PTD */
        { 'S','E','L','E','C','T',' ','D','A','I','L','Y',',','P','T','D','\0','\0','\0','\0'},

        /* Addr. 55 Copy OK? */
        { 'O','K',' ','T','O',' ','S','T','A','R','T',' ','S','Y','N','C','\0','\0','\0','\0'},

        /* Addr. 56 During Comm. */
        { 'C','O','M','M','U','N','I','C','A','T','I','N','G','\0','\0','\0','\0','\0','\0','\0'},

        /* Addr. 57 Balance Due */
        { 'B','A','L','A','N','C','E',' ','D','U','E','\0','\0','\0','\0','\0','\0','\0','\0','\0'},

        /* Addr. 58 Scale Error */
        { 'S','C','A','L','E',' ','E','R','R','O','R','\0','\0','\0','\0','\0','\0','\0','\0','\0'},

        /* Addr. 59 Scale/Tare */
        { 'T','A','R','E','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},

        /* Addr. 60 E/J Near Full Condition */
        { 'E','.','J',' ','N','E','A','R',' ','F','U','L','L','\0','\0','\0','\0','\0','\0','\0'},

        /* Addr. 61 Take To E/J Reset Report */
        { 'E','.','J',' ','F','I','L','E',' ','F','U','L','L','\0','\0','\0','\0','\0','\0','\0'},

        /* Addr. 62 Alternation Message for Shared Printer */
        { 'A','L','T','E','R','N','A','T','E',' ','P','R','I','N','T','\0','\0','\0','\0','\0'},

        /* Addr. 63 Kitchen CRT error */
        { 'K','I','T','C','H','E','N',' ','C','R','T',' ','E','R','R','\0','\0','\0','\0','\0'},

        /* Addr. 64 ETK Entry error */
        { 'N','O','T',' ','T','I','M','E','-','I','N','/','O','U','T','\0','\0','\0','\0','\0'},

        /* Addr. 65 Enter Room # */
        { 'E','N','T','E','R',' ','R','O','O','M',' ','#','\0','\0','\0','\0','\0','\0','\0','\0'},

        /* Addr. 66 Enter Guest Id */
        { 'E','N','T','E','R',' ','G','U','E','S','T',' ','I','D','\0','\0','\0','\0','\0','\0'},

        /* Addr. 67 Enter Guest Line */
        { 'E','N','T','E','R',' ','G','U','E','S','T',' ','L','I','N','E','\0','\0','\0','\0'},

        /* Addr. 68 Override Ok ? */
        { 'O','V','E','R','R','I','D','E',' ','O','K',' ','?','\0','\0','\0','\0','\0','\0','\0'},

        /* Addr. 69 Charge check out */
        { 'C','H','A','R','G','E',' ','C','H','K',' ','O','U','T','\0','\0','\0','\0','\0','\0'},

        /* Addr. 70 Guest check out */
        { 'G','U','E','S','T',' ','C','H','K',' ','O','U','T',' ',' ','\0','\0','\0','\0','\0'},

        /* Addr. 71 Wrong GC # */
        { 'W','R','O','N','G',' ','R','O','O','M',' ','N','O','\0','\0','\0','\0','\0','\0','\0'},

        /* Addr. 72 Wrong guest ID */
        { 'W','R','O','N','G',' ','G','U','E','S','T',' ','I','D','\0','\0','\0','\0','\0','\0'},

        /* Addr. 73 Prohibit Void operation */
        { 'P','R','O','H','I','B','I','T',' ','V','O','I','D',' ','O','P','\0','\0','\0','\0'},

        /* Addr. 74 Wrong acct. # */
        { 'W','R','O','N','G',' ','A','C','C','T','.',' ','N','O','\0','\0','\0','\0','\0','\0'},

        /* Addr. 75 Advise front */
        { 'A','D','V','I','S','E',' ','F','R','O','N','T','\0','\0','\0','\0','\0','\0','\0','\0'},

        /* Addr. 76 Wrong acct. type */
        { 'W','R','O','N','G',' ','A','C','C','T','.',' ','T','Y','P','E','\0','\0','\0','\0'},

        /* Addr. 77 Wrong SLD */
        { 'W','R','O','N','G',' ','S','L','D',' ',' ',' ',' ',' ',' ',' ','\0','\0','\0','\0'},

        /* Addr. 78 Wrong voucher no */
        { 'W','R','O','N','G',' ','V','O','U','C','H','E','R',' ','#','\0','\0','\0','\0','\0'},

        /* Addr. 79 CPM busy */
        { 'C','H','A','R','G','E',' ','P','O','S','T',' ','B','U','S','Y','\0','\0','\0','\0'},

        /* Addr. 80 CPM stop func. */
        { 'C','H','G',' ','P','O','S','T',' ','S','T','O','P','P','E','D','\0','\0','\0','\0'},

        /* Addr. 81 CPM offline */
        { 'C','H','G',' ','P','O','S','T',' ','O','F','F','L','I','N','E','\0','\0','\0','\0'},

        /* Addr. 82 EPT rejected */
        { 'E','P','T',' ','R','E','J','E','C','T',' ',' ',' ',' ',' ',' ','\0','\0','\0','\0'},

        /* Addr. 83 EPT offline */
        { 'E','P','T',' ','O','F','F','L','I','N','E',' ',' ',' ',' ',' ','\0','\0','\0','\0'},

        /* Addr. 84 Cancel by user */
        { 'C','A','N','C','E','L','L','E','D',' ',' ',' ',' ',' ',' ',' ','\0','\0','\0','\0'},

        /* Addr. 85 Enter Expiration Date */
        { 'E','N','T',' ','E','X','P','I','R','E',' ','D','A','T','E',' ','\0','\0','\0','\0'},

        /* Addr. 86 Enter OEP Table No. */
        { 'E','N','T','E','R',' ','T','A','B','L','E',' ','N','O','.',' ','\0','\0','\0','\0'},

        /* Addr. 87 Enter Coupon # */
        { 'E','N','T','E','R',' ','C','O','U','P','O','N',' ','#','\0','\0','\0','\0','\0','\0'},

        /* Addr. 88 Pause Message #1 */
        { 'P','A','U','S','E',' ','M','E','S','S','A','G','E',' ','#','1','\0','\0','\0','\0'},

        /* Addr. 89 Pause Message #2 */
        { 'P','A','U','S','E',' ','M','E','S','S','A','G','E',' ','#','2','\0','\0','\0','\0'},

        /* Addr. 90 Pause Message #3 */
        { 'P','A','U','S','E',' ','M','E','S','S','A','G','E',' ','#','3','\0','\0','\0','\0'},

        /* Addr. 91 Pause Message #4 */
        { 'P','A','U','S','E',' ','M','E','S','S','A','G','E',' ','#','4','\0','\0','\0','\0'},

        /* Addr. 92 Pause Message #5 */
        { 'P','A','U','S','E',' ','M','E','S','S','A','G','E',' ','#','5','\0','\0','\0','\0'},

        /* Addr. 93 Pause Message #6(Alpha) */
        { 'P','A','U','S','E',' ','M','E','S','S','A','G','E',' ','#','6','\0','\0','\0','\0'},

        /* Addr. 94 Pause Message #7(Alpha) */
        { 'P','A','U','S','E',' ','M','E','S','S','A','G','E',' ','#','7','\0','\0','\0','\0'},

        /* Addr. 95 Pause Message #8(Alpha) */
        { 'P','A','U','S','E',' ','M','E','S','S','A','G','E',' ','#','8','\0','\0','\0','\0'},

        /* Addr. 96 Pause Message #9(Alpha) */
        { 'P','A','U','S','E',' ','M','E','S','S','A','G','E',' ','#','9','\0','\0','\0','\0'},

        /* Addr. 97 Pause Message #10(Alpha) */
        { 'P','A','U','S','E',' ','M','E','S','S','A','G','E','#','1','0','\0','\0','\0','\0'},

        /* Addr. 98 Customer Name Entry */
        { 'C','U','S','T','O','M','E','R',' ','N','A','M','E',' ',' ',' ','\0','\0','\0','\0'},

        /* Addr. 99 Unique Transaction No. */
        { 'Y','O','U','R',' ','O','R','D','E','R',' ','#','\0','\0','\0','\0','\0','\0','\0','\0'},

        /* Addr. 100 OEP (99 Option) */
        { 'E','N','T','E','R',' ','N','O','/','S','K','I','P',' ','0','0','\0','\0','\0','\0'},

        /* Addr. 101 OEP (91 Option) */
        { 'E','N','T','E','R',' ','N','O','/','F','R','E','E',' ','1',' ','\0','\0','\0','\0'},

        /* Addr. 102 OEP (92 Option) */
        { 'E','N','T','E','R',' ','N','O','/','F','R','E','E',' ','2',' ','\0','\0','\0','\0'},

        /* Addr. 103 OEP (93 Option) */
        { 'E','N','T','E','R',' ','N','O','/','F','R','E','E',' ','3',' ','\0','\0','\0','\0'},

        /* Addr. 104 OEP (94 Option) */
        { 'E','N','T','E','R',' ','N','O','/','F','R','E','E',' ','4',' ','\0','\0','\0','\0'},

        /* Addr. 105 OEP (95 Option) */
        { 'R','E','S','E','R','V','E','D',' ',' ',' ',' ',' ',' ',' ',' ','\0','\0','\0','\0'},

        /* Addr. 106 OEP (96 Option) */
        { 'R','E','S','E','R','V','E','D',' ',' ',' ',' ',' ',' ',' ',' ','\0','\0','\0','\0'},

        /* Addr. 107 OEP (97 Option) */
        { 'E','N','T','E','R',' ','C','O','N','D','I','M','E','N','T',' ','\0','\0','\0','\0'},

        /* Addr. 108 OEP (98 Option) */
        { 'E','N','T','E','R',' ','N','O','/','N','O','T',' ','0','0',' ','\0','\0','\0','\0'},

        /* Addr. 109 Scroll Up */
        { 'U','P','P','E','R',' ','E','N','D',' ',' ',' ',' ',' ',' ',' ','\0','\0','\0','\0'},

        /* Addr. 110 Scroll Up */
        { 'L','O','W','E','R',' ','E','N','D',' ',' ',' ',' ',' ',' ',' ','\0','\0','\0','\0'},

        /* Addr. 111 Welcome for 2x20 customer */
        { 'W','E','L','C','O','M','E',' ','T','O',' ','O','U','R','S',' ','\0','\0','\0','\0'},

        /* Addr. 112 E/J cluster read error */
        { 'E','.','J',' ','R','E','A','D',' ','E','R','R','O','R',' ',' ','\0','\0','\0','\0'},

        /* Addr. 113 E/J cluster reset error */
        { 'E','.','J',' ','R','E','S','E','T',' ','E','R','R','O','R',' ','\0','\0','\0','\0'},

        /* Addr. 114 Next Entry Message */
        { 'E','N','T','E','R',' ','N','E','X','T',' ','I','T','E','M',' ','\0','\0','\0','\0'},

        /* Addr. 115 Enter Secret Code, V3.3 */
        { 'E','N','T','E','R',' ','S','E','C','R','E','T',' ','#',' ',' ','\0','\0','\0','\0'},

        /* Addr. 116 Register Mode Message, V3.3 */
        { 'R','E','G','I','S','T','E','R',' ','M','O','D','E',' ',' ',' ','\0','\0','\0','\0'},

        /* Addr. 117 Supervisor Mode Message, V3.3 */
        { 'S','U','P','E','R','V','I','S','O','R',' ','M','O','D','E',' ','\0','\0','\0','\0'},

        /* Addr. 118 Closed Customer Message, V3.3 */
        { 'T','E','R','M','I','N','A','L',' ','C','L','O','S','E','D',' ','\0','\0','\0','\0'},

        /* Addr. 119 Remove Server Lock, V3.3 */
        { 'R','E','M','O','V','E',' ','L','O','C','K',' ',' ',' ',' ',' ','\0','\0','\0','\0'},

        /* Addr. 120 Insert Server Lock, V3.3 */
        { 'I','N','S','E','R','T',' ','L','O','C','K',' ',' ',' ',' ',' ','\0','\0','\0','\0'},

        /* Addr. 121 No in File Building, 2172 */
        { 'P','L','U',' ','B','U','I','L','D','I','N','G',' ','U','P',' ','\0','\0','\0','\0'},

        /* Addr. 122 Enter Birthday Entry, 2172 */
        { 'E','N','T','E','R',' ','B','-','D','A','Y',' ','D','A','T','T','\0','\0','\0','\0'},

        /* Addr. 123 Dummy */
        { 'E','N','T','E','R',' ','C','L','E','A','R',' ','K','E','Y',' ','\0','\0','\0','\0'},

/*** NCR2172 ***/
        /* Addr. 124 WIC transaction key *//* ### MOD Saratoga (052200) */
        { 'W','I','C',' ','T','R','A','N','S','A','C','T','I','O','N',' ','\0','\0','\0','\0'},

        /* Addr. 125 Non WIC item *//* ### MOD Saratoga (052200) */
        { 'N','O','N',' ','W','I','C',' ','I','T','E','M',' ',' ',' ',' ','\0','\0','\0','\0'},

        /* Addr. 126 Power Down Process *//* ### MOD Saratoga (052700) */
        { 'P','O','W','E','R',' ','D','O','W','N',' ','P','R','O','C','.','\0','\0','\0','\0'},

        /* Addr. 127 Storage Low */
        { 'D','I','S','K',' ','N','E','A','R',' ','F','U','L','L',' ',' ','\0','\0','\0','\0'},

        /* Addr. 128 Not Enough Storage */
        { 'D','I','S','K',' ','F','U','L','L',' ',' ',' ',' ',' ',' ',' ','\0','\0','\0','\0'},

        /* Addr. 129 Not Enough Memory */
        { 'S','H','O','R','T',' ','M','E','M','O','R','Y',' ',' ',' ',' ','\0','\0','\0','\0'},

//US Customs SCER 10 Additional Pause Messages (5 each, alphanumeric and numeric)
        /* Addr. 130 Pause Message #11 */
        { 'P','A','U','S','E',' ','M','E','S','S','A','G','E','#','1','1','\0','\0','\0','\0'},

        /* Addr. 131 Pause Message #12 */
        { 'P','A','U','S','E',' ','M','E','S','S','A','G','E','#','1','2','\0','\0','\0','\0'},

        /* Addr. 132 Pause Message #13 */
        { 'P','A','U','S','E',' ','M','E','S','S','A','G','E','#','1','3','\0','\0','\0','\0'},

        /* Addr. 133 Pause Message #14 */
        { 'P','A','U','S','E',' ','M','E','S','S','A','G','E','#','1','4','\0','\0','\0','\0'},

        /* Addr. 134 Pause Message #15 */
        { 'P','A','U','S','E',' ','M','E','S','S','A','G','E','#','1','5','\0','\0','\0','\0'},

        /* Addr. 135 Pause Message #16(Alpha) */
        { 'P','A','U','S','E',' ','M','E','S','S','A','G','E','#','1','6','\0','\0','\0','\0'},

        /* Addr. 136 Pause Message #17(Alpha) */
        { 'P','A','U','S','E',' ','M','E','S','S','A','G','E','#','1','7','\0','\0','\0','\0'},

        /* Addr. 137 Pause Message #18(Alpha) */
        { 'P','A','U','S','E',' ','M','E','S','S','A','G','E','#','1','8','\0','\0','\0','\0'},

        /* Addr. 138 Pause Message #19(Alpha) */
        { 'P','A','U','S','E',' ','M','E','S','S','A','G','E','#','1','9','\0','\0','\0','\0'},

        /* Addr. 139 Pause Message #20(Alpha) */
        { 'P','A','U','S','E',' ','M','E','S','S','A','G','E','#','2','0','\0','\0','\0','\0'},
//End US Customs Scer 10 Additional Pause Messages

        /* Addr. 140: LDT_BLOCKEDBYMDC_ADR  -  Blocked by MDC Bit Setting */
        { 'B','L','O','C','K','E','D',' ','B','Y',' ','M','D','C','\0','\0','\0','\0','\0','\0'},

        /* Addr. 141: LDT_TARENONSCALABLE_ADR - Not a scalable item */
        { 'N','O','T',' ','S','C','A','L','E',' ','I','T','E','M','\0','\0','\0','\0','\0','\0'},

        /* Addr. 142 LDT_CPM_UNKNOWNRESPONSE Charge Post responded with an unknown response code */
        { 'C','P','M','/','E','P','T',' ','R','E','S','P',' ','E','R','R','\0','\0','\0','\0'},

        /* Addr. 143 LDT_INVALID_SUP_NUM Invalid Supervisor Number JHHJ*/
        { 'I','N','V','A','L','I','D',' ','S','U','P','.',' ','N','U','M','\0','\0','\0','\0'},

        /* Addr. 144 :LDT_SALESRESTRICTED - Item sales restricted as in age or not on Sunday */
        { 'S','A','L','E','S',' ','R','E','S','T','R','I','C','T','E','D','\0','\0','\0','\0'},

        /* Addr. 145: LDT_EQUIPPROVERROR_ADR - Equipment provisioning error */
        { 'E','Q','U','I','P',' ','P','R','O','V',' ','E','R','R','O','R','\0','\0','\0','\0'},

        // Addr. 146: LDT_KPS_BUSY - Kitchen printer is busy as indicated by KpsCheckPrint ()
        { 'K','I','T','C','H','N',' ','P','R','N','T','R',' ','B','S','Y','\0','\0','\0','\0'},

        // Addr. 147: LDT_REQST_TIMED_OUT - Requested operation timed out before completion
        { 'R','Q','S','T',' ','T','I','M','O','U','T',' ','R','T','R','Y','\0','\0','\0','\0'},

        // Addr. 148: LDT_I_AM_NOT_MASTER - This terminal is not acting as a Master terminal
        { 'T','E','R','M',' ','N','O','T',' ','M','A','S','T','E','R','\0','\0','\0','\0','\0'},

        // Addr. 149 LDT_REFERENCENUM_ADR - Enter Reference Number
        { 'E','N','T','E','R',' ','R','E','F','E','R','E','N','C','E',' ','N','U','M','\0'},

        // Addr. 150 LDT_AUTHCODE_ADR - Enter Authorization Code
        { 'E','N','T','E','R',' ','A','U','T','H',' ','C','O','D','E','\0','\0','\0','\0','\0'},

        // Addr. 151 LDT_REGSITERED_ADR - Not registered with Softlocx
        { 'U','N','R','E','G','I','S','T','E','R','E','D',' ','C','O','P','Y','\0','\0','\0'},

        // Addr. 152 LDT_
        { 'K','D','S',' ',' ','B','A','C','K',' ','U','P',' ','D','O','W','N','\0','\0','\0'},

        // Addr. 153 LDT_NOTSIGNEDIN_ADR - Not Signed In
        { 'N','O','T',' ','S','I','G','N','E','D',' ','I','N',' ','\0','\0','\0','\0','\0','\0'},

        // Addr. 154 LDT_SEQUENCE_NUMBER - Sequence Number
        { 'S','E','Q','U','E','N','C','E',' ','N','U','M','B','E','R','\0','\0','\0','\0','\0'},

        // Addr. 155 LDT_ROUTING_NUMBER - Routing Number
        { 'R','O','U','T','I','N','G',' ','N','U','M','B','E','R','\0','\0','\0','\0','\0','\0'},

        // Addr. 156 LDT_AMT_ENTRY_REQUIRED_ADR - Amount Entry Required
        { _T('A'),_T('M'),_T('T'),_T(' '),_T('E'),_T('N'),_T('T'),_T('R'),_T('Y'),_T(' '),_T('R'),_T('E'),_T('Q'),_T('U'),_T('I'),_T('R'),_T('E'),_T('D'),_T('\0'),_T('\0')},

        // Addr. 157 LDT_PRT_COVEROPEN_ADR - Printer Cover Open
        { _T('P'),_T('R'),_T('I'),_T('N'),_T('T'),_T('E'),_T('R'),_T(' '),_T('C'),_T('O'),_T('V'),_T('E'),_T('R'),_T(' '),_T('O'),_T('P'),_T('E'),_T('N'),_T('\0'),_T('\0')},

        // Addr. 158 LDT_PRT_PAPERCHNG_ADR - Change Paper
        { _T('C'),_T('H'),_T('A'),_T('N'),_T('G'),_T('E'),_T(' '),_T('P'),_T('A'),_T('P'),_T('E'),_T('R'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0')},

        // Addr. 159 LDT_TIP_REQUEST_ADR - No Tips Entered
        { _T('N'),_T('O'),_T(' '),_T('T'),_T('I'),_T('P'),_T('S'),_T(' '),_T('E'),_T('N'),_T('T'),_T('E'),_T('R'),_T('E'),_T('D'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0')},

        // Addr. 160 LDT_PLUGROUPNO_ADR - PLU Group Number Request
        { _T('P'),_T('L'),_T('U'),_T(' '),_T('G'),_T('R'),_T('P'),_T(' '),_T('N'),_T('U'),_T('M'),_T(' '),_T('R'),_T('E'),_T('Q'),_T('U'),_T('E'),_T('S'),_T('T'),_T('\0')},

        // Addr. 161 LDT_TENT - Receipt ID information
        { _T('R'),_T('E'),_T('C'),_T('E'),_T('I'),_T('P'),_T('T'),_T(' '),_T('I'),_T('D'),_T(' '),_T('I'),_T('N'),_T('F'),_T('O'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0')},

        // Addr. 162 LDT_ORDER_DEC_ADR - Require Order Declaration JHHJ
        { _T('R'),_T('E'),_T('Q'),_T('U'),_T('I'),_T('R'),_T('E'),_T(' '),_T('O'),_T('R'),_T('D'),_T('E'),_T('R'),_T(' '),_T('D'),_T('E'),_T('C'),_T('L'),_T('A'),_T('R')},

        // Addr. 163 LDT_NOT_ALLOW_QR - Quantity not allowed for PLU error
        { _T('Q'),_T('N'),_T('T'),_T('Y'),_T(' '),_T('N'),_T('O'),_T('T'),_T(' '),_T('F'),_T('O'),_T('R'),_T(' '),_T('P'),_T('L'),_T('U'),_T(' '),_T('E'),_T('R'),_T('R')},
        
		// Addr. 164 LDT_REQUIRE_QR - Require Quantity required
        { _T('R'),_T('E'),_T('Q'),_T('U'),_T('I'),_T('R'),_T('E'),_T(' '),_T('Q'),_T('U'),_T('A'),_T('N'),_T('T'),_T('I'),_T('T'),_T('Y'),_T(' '),_T('R'),_T('E'),_T('Q')},

        // Addr. 165 LDT_TARE_ENTER - Tare required before weighing JHHJ
        { _T('T'),_T('A'),_T('R'),_T('E'),_T(' '),_T('R'),_T('E'),_T('Q'),_T(' '),_T('P'),_T('R'),_T('E'),_T(' '),_T('W'),_T('E'),_T('I'),_T('G'),_T('H'),_T('\0'),_T('\0')},

        // Addr. 166 LDT_GIFTCARD_BALANCE - GiftCard Balance  SS
        { _T('G'),_T('I'),_T('F'),_T('T'),_T('C'),_T('A'),_T('R'),_T('D'),_T(' '),_T('B'),_T('A'),_T('L'),_T('A'),_T('N'),_T('C'),_T('E'),_T('\0'),_T('\0'),_T('\0'),_T('\0')},

        // Addr. 167 LDT_CARD_NUM - Card number		 SS
        { _T('C'),_T('A'),_T('R'),_T('D'),_T(' '),_T('N'),_T('U'),_T('M'),_T('B'),_T('E'),_T('R'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0')},

        // Addr. 168 LDT_VOID_ISSUE - Gift Card Void Issue		SS
        { _T('G'),_T('I'),_T('F'),_T('T'),_T(' '),_T('C'),_T('A'),_T('R'),_T('D'),_T(' '),_T('V'),_T('O'),_T('I'),_T('D'),_T(' '),_T('I'),_T('S'),_T('S'),_T('U'),_T('E')},

        // Addr. 169 LDT_RELOAD - Gift Card Reload
        { _T('G'),_T('I'),_T('F'),_T('T'),_T(' '),_T('C'),_T('A'),_T('R'),_T('D'),_T(' '),_T('R'),_T('E'),_T('L'),_T('O'),_T('A'),_T('D'),_T('\0'),_T('\0'),_T('\0'),_T('\0')},

        // Addr. 170 LDT_VOID_RELOAD - Gift Card Void Reload
        { _T('G'),_T('I'),_T('F'),_T('T'),_T('C'),_T('A'),_T('R'),_T('D'),_T(' '),_T('V'),_T('O'),_T('I'),_T('D'),_T(' '),_T('R'),_T('E'),_T('L'),_T('O'),_T('A'),_T('D')},

        // Addr. 171 LDT_ISSUE - Gift Card Issue
        { _T('G'),_T('I'),_T('F'),_T('T'),_T(' '),_T('C'),_T('A'),_T('R'),_T('D'),_T(' '),_T('I'),_T('S'),_T('S'),_T('U'),_T('E'),_T('\0'),_T('\0'),_T('\0'),_T('\0'),_T('\0')},

        // Addr. 172 LDT_STORE_FORWARD_STATUS - Store and Forward
        { _T('S'),_T('T'),_T('O'),_T('R'),_T('E'),_T(' '),_T('A'),_T('N'),_T('D'),_T(' '),_T('F'),_T('O'),_T('R'),_T('W'),_T('A'),_T('R'),_T('D'),_T('\0'),_T('\0'),_T('\0')},

        // Addr. 173 LDT_AC_444 - Disconn. Satellite
        { _T('D'),_T('I'),_T('S'),_T('C'),_T('O'),_T('N'),_T('N'),_T('.'),_T(' '),_T('S'),_T('A'),_T('T'),_T('E'),_T('L'),_T('L'),_T('I'),_T('T'),_T('E'),_T('\0'),_T('\0')},

        // Addr. 174 Forwarding Ended
        {_T('S'),_T('T'),_T('O'),_T('R'),_T('E'),_T(' '),_T('A'),_T('N'),_T('D'),_T(' '),_T('F'),_T('O'),_T('R'),_T('W'),_T('A'),_T('R'),_T('D'),_T('\0'),_T('\0'),_T('\0')},

        // Addr. 175 Dummy
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},

        // Addr. 176 Dummy
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},

        // Addr. 177 Dummy
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},

        // Addr. 178 Dummy
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},

        // Addr. 179 Dummy
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},

        // Addr. 180 Dummy
        { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'},

    },
    {   /* Prog.22 REPORT NAME, CLASS_PARAREPORTNAME, ParaReportName[MAX_REPO_NO][PARA_REPORTNAME_LEN] */
		// string template for max length if adding mnemonics
		//_T("123456"
		{ _T('A'), _T('C'), _T('T'), _T('I'), _T('O'), _T('N')},    /* Addr. 1 Action Number */
		{ _T('F'), _T('I'), _T('N'), _T('A'), _T('N'),_T('\0')},    /* Addr. 2 Reg. Fin. Report */
		{ _T('D'), _T('E'), _T('P'), _T('A'), _T('R'), _T('T')},    /* Addr. 3 Dept. Report */
		{ _T('P'), _T('L'), _T('U'), _T('\0'),_T('\0'),_T('\0')},   /* Addr. 4 PLU Report */
		{ _T('H'), _T('O'), _T('U'), _T('R'), _T('L'), _T('Y')},    /* Addr. 5 Hourly Report */
		{ _T('O'), _T('P'), _T('E'), _T('R'), _T('A'), _T('T')},    /* Addr. 6 Cashier Report */
		{ _T(' '), _T(' '), _T(' '), _T(' '), _T(' '), _T(' ')},    /* Addr. 7 Server Report */
		{ _T('E'), _T('-'), _T('O'), _T('-'), _T('D'), _T('\0')},   /* Addr. 8 E-O-D Report */
		{ _T('P'), _T('-'), _T('T'), _T('-'), _T('D'), _T('\0')},   /* Addr. 9 P-T-D Report */
		{ _T('G'), _T('U'), _T('E'), _T('S'), _T('T'), _T('\0')},   /* Addr.10 Guest Check Report */
		{ _T('P'), _T('R'), _T('O'), _T('.'), _T('#'), _T('\0')},   /* Addr.11 Program Number */
		{ _T('F'), _T('L'), _T('A'), _T('S'), _T('H'), _T('\0')},   /* Addr.12 Flash Report */
		{ _T('M'), _T('A'), _T('J'), _T('O'), _T('R'), _T('\0')},   /* Addr.13 Major Dept. Report */
		{ _T('M'), _T('E'), _T('N'), _T('U'), _T('\0'), _T('\0')},  /* Addr.14 Reserved */
		{ _T('R'), _T('E'), _T('A'), _T('D'), _T('\0'), _T('\0')},  /* Addr.15 Read Report */
		{ _T('R'), _T('E'), _T('S'), _T('E'), _T('T'), _T('\0')},   /* Addr.16 Reset Report */
		{ _T('M'), _T('A'), _T('I'), _T('N'), _T('T'), _T('\0')},   /* Addr.17 Maint. Report */
		{ _T('F'), _T('I'), _T('L'), _T('E'), _T('\0'),_T('\0')},   /* Addr.18 File Report */
		{ _T('E'), _T('.'), _T('J'), _T('.'), _T('\0'),_T('\0')},   /* Addr.19 E/J Report */
		{ _T('E'), _T('T'), _T('K'), _T('\0'),_T('\0'),_T('\0')},   /* Addr.20 ETK Report */
		{ _T('C'), _T('P'), _T('\0'), _T('\0'),_T('\0'),_T('\0')},  /* Addr.21 CPM Tally Report */
		{ _T('E'), _T('P'), _T('T'), _T('\0'),_T('\0'),_T('\0')},   /* Addr.22 EPT Tally Report */
		{ _T('C'), _T('P'), _T('N'), _T('\0'),_T('\0'),_T('\0')},   /* Addr.23 Coupon Report */
		{ _T('S'), _T('R'), _T('V'), _T('I'), _T('C'), _T('E')},    /* Addr.24 Service Time Report */
		{ _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},   /* Addr.25  */
		{ _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},   /* Addr.26  */
		{ _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},   /* Addr.257  */
		{ _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},   /* Addr.28  */
		{ _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},   /* Addr.29  */
		{ _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},   /* Addr.30  */
		{ _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')},   /* Addr.31  */
		{ _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')}    /* Addr.32  */
    },
    {   /* Prog.23 SPECIAL MNEMONICS, CLASS_PARASPECIALMNEMO, ParaSpeMnemo[MAX_SPEM_NO][PARA_SPEMNEMO_LEN] */
		// string template for max length if adding mnemonics
		// these constants are actually replaced by the strings in the resource file IDS_P23_SPCL_MNEM_DEFAULT_001, ...
		//_T("1234"
        { _T('G'), _T('S'), _T('T'), _T('#')},       /* Addr. 1 Guest Check # */
        { _T('O'), _T('-'), _T('I'), _T('D')},       /* Addr. 2 Cashier ID */
        { _T(' '), _T(' '), _T(' '), _T(' ')},       /* Addr. 3 Server ID */
        { _T('T'), _T('B'), _T('L'), _T('#')},       /* Addr. 4 Table No. */
        { _T('D'), _T('A'), _T('Y'), _T('Z')},       /* Addr. 5 Daily Reset Counter */
        { _T('P'), _T('T'), _T('D'), _T('Z')},       /* Addr. 6 PTD Reset Counter */
        { _T('P'), _T('R'), _T('G'), _T('.')},       /* Addr. 7 Program Counter */
        { _T('T'), _T('-'), _T('I'), _T('D')},       /* Addr. 8 Training ID */
        { _T('T'), _T('X'), _T('.'), _T('1')},       /* Addr. 9 Tax Mod. #1 */
        { _T('T'), _T('X'), _T('.'), _T('2')},       /* Addr.10 Tax Mod. #2 */
        { _T('T'), _T('X'), _T('.'), _T('3')},       /* Addr.11 Tax Mod. #3 */
        { _T('R'), _T('M'), _T(' '), _T('#')},       /* Addr.12 Roon # */
        { _T('G'), _T('-'), _T('I'), _T('D')},       /* Addr.13 Guest ID */
        { _T('F'), _T('O'), _T(' '), _T('#')},       /* Addr.14 Folio # */
        { _T('P'), _T('O'), _T('T'), _T('#')},       /* Addr.15 Post Transaction # */
        { _T('T'), _T('X'), _T('.'), _T('8')},       /* Addr.16 Tax Mod. #8 */
        { _T('T'), _T('X'), _T('.'), _T('9')},       /* Addr.17 Tax Mod. #9 */
        { _T('E'), _T('X'), _T('P'), _T('1')},       /* Addr.18 Tax Exempt1 */
        { _T('E'), _T('X'), _T('P'), _T('2')},       /* Addr.19 Tax Exempt2 */
        { _T('L'), _T('N'), _T('#'), _T('\0')},      /* Addr.20 Line No. */
        { _T('$'), _T('\0'), _T('\0'), _T('\0')},    /* Addr.21 Currency Symbol for Native */
        { 0x9c, _T('\0'), _T('\0'), _T('\0')},   /* Addr.22 Currency Symbol for F/C 1 */
        { _T('Y'), _T('E'), _T('N'), _T('\0')},      /* Addr.23 Currency Symbol for F/C 2 */
        { _T('K'), _T('C'), _T('H'), _T('\0')},      /* Addr.24 Kitchen Printer */
        { _T(' '), _T('l'), _T('b'), _T('\0')},      /* Addr.25 Kg/LB, V1.0.04 */
        { _T('V'), _T('O'), _T('I'), _T('D')},       /* Addr.26 Void */
        { _T('O'), _T('F'), _T('F'), _T('L')},       /* Addr.27 Off Line Tender */
        { _T('S'), _T('E'), _T('A'), _T('T')},       /* Addr.28 Off Line Tender */
        { _T('T'), _T('X'), _T('.'), _T('4')},       /* Addr.29 Tax Mod. #4 */
        { _T('T'), _T('X'), _T('.'), _T('5')},       /* Addr.30 Tax Mod. #5 */
        { _T('T'), _T('X'), _T('.'), _T('6')},       /* Addr.31 Tax Mod. #6 */
        { _T('T'), _T('X'), _T('.'), _T('7')},       /* Addr.32 Tax Mod. #7 */
        { _T('F'), _T('3'), _T('*'), _T('\0')},      /* Addr.33 Currency Symbol for F/C 3, 2172 */
        { _T('F'), _T('4'), _T('*'), _T('\0')},      /* Addr.34 Currency Symbol for F/C 4, 2172 */
        { _T('F'), _T('5'), _T('*'), _T('\0')},      /* Addr.35 Currency Symbol for F/C 3, 2172 */
        { _T('F'), _T('6'), _T('*'), _T('\0')},      /* Addr.36 Currency Symbol for F/C 4, 2172 */
        { _T('F'), _T('7'), _T('*'), _T('\0')},      /* Addr.37 Currency Symbol for F/C 3, 2172 */
        { _T('F'), _T('8'), _T('*'), _T('\0')},      /* Addr.38 Currency Symbol for F/C 4, 2172 */
        { _T('F'), _T('S'), _T('M'), _T('D')},       /* Addr.39 Food Stamp Modifier */
        { _T(' '), _T(' '), _T(' '), _T(' ')},       /* Addr.40 dummy */
        { _T('A'), _T(' '), _T(' '), _T(' ')},       /* Addr.41 Taxable #1, 21RFC05437 */
        { _T('B'), _T(' '), _T(' '), _T(' ')},       /* Addr.42 Taxable #2 */
        { _T('C'), _T(' '), _T(' '), _T(' ')},       /* Addr.43 Taxable #3 */
        { _T('D'), _T(' '), _T(' '), _T(' ')},       /* Addr.44 Taxable #1&2 */
        { _T('E'), _T(' '), _T(' '), _T(' ')},       /* Addr.45 Taxable #1&3 */
        { _T('F'), _T(' '), _T(' '), _T(' ')},       /* Addr.46 Taxable #2&3 */
        { _T('G'), _T(' '), _T(' '), _T(' ')},       /* Addr.47 Taxable #1&2&3 */
        { _T('H'), _T(' '), _T(' '), _T(' ')},       /* Addr.48 Food Stampable */
        { _T('I'), _T(' '), _T(' '), _T(' ')},       /* Addr.49 Taxable #4 */
        { _T('J'), _T(' '), _T(' '), _T(' ')},       /* Addr.50 Taxable #1&4 */
		{ _T('R'), _T('T'), _T('N'), _T('1')},       /* Addr. 51, Returns #1 */
		{ _T('R'), _T('T'), _T('N'), _T('2')},       /* Addr. 52, Returns #2 */
		{ _T('R'), _T('T'), _T('N'), _T('3')},       /* Addr. 53, Returns #3 */
		{ _T('\0'), _T('\0'), _T('\0'), _T('\0')},       /* Addr. 54, unused */
		{ _T('\0'), _T('\0'), _T('\0'), _T('\0')},       /* Addr. 55, unused */
		{ _T('\0'), _T('\0'), _T('\0'), _T('\0')},       /* Addr. 56, unused */
		{ _T('\0'), _T('\0'), _T('\0'), _T('\0')},       /* Addr. 57, unused */
		{ _T('\0'), _T('\0'), _T('\0'), _T('\0')},       /* Addr. 58, unused */
		{ _T('\0'), _T('\0'), _T('\0'), _T('\0')},       /* Addr. 59, unused */
		{ _T('\0'), _T('\0'), _T('\0'), _T('\0')},       /* Addr. 60, unused */
		{ _T('\0'), _T('\0'), _T('\0'), _T('\0')},       /* Addr. 61, unused */
		{ _T('\0'), _T('\0'), _T('\0'), _T('\0')},       /* Addr. 62, unused */
		{ _T('\0'), _T('\0'), _T('\0'), _T('\0')},       /* Addr. 63, unused */
		{ _T('\0'), _T('\0'), _T('\0'), _T('\0')},       /* Addr. 64, unused */
		{ _T('\0'), _T('\0'), _T('\0'), _T('\0')},       /* Addr. 65, unused */
		{ _T('\0'), _T('\0'), _T('\0'), _T('\0')},       /* Addr. 66, unused */
		{ _T('\0'), _T('\0'), _T('\0'), _T('\0')},       /* Addr. 67, unused */
		{ _T('\0'), _T('\0'), _T('\0'), _T('\0')},       /* Addr. 68, unused */
		{ _T('\0'), _T('\0'), _T('\0'), _T('\0')},       /* Addr. 69, unused */
		{ _T('\0'), _T('\0'), _T('\0'), _T('\0')},       /* Addr. 70, unused */
		{ _T('\0'), _T('\0'), _T('\0'), _T('\0')},       /* Addr. 71, unused */
		{ _T('\0'), _T('\0'), _T('\0'), _T('\0')},       /* Addr. 72, unused */
		{ _T('\0'), _T('\0'), _T('\0'), _T('\0')},       /* Addr. 73, unused */
		{ _T('\0'), _T('\0'), _T('\0'), _T('\0')},       /* Addr. 74, unused */
		{ _T('\0'), _T('\0'), _T('\0'), _T('\0')}        /* Addr. 75, unused */
    },
    {   /* Prog.39 PC I/F */
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '
    },
    {   /* Prog.46 ADJECTIVE MNEMONICS */
        { 'A', '1', '-', '1'},  /* Addr. 1 Adj G1-V1 */
        { 'A', '1', '-', '2'},  /* Addr. 2 Adj G1-V2 */
        { 'A', '1', '-', '3'},  /* Addr. 3 Adj G1-V3 */
        { 'A', '1', '-', '4'},  /* Addr. 4 Adj G1-V4 */
        { 'A', '1', '-', '5'},  /* Addr. 5 Adj G1-V5 */
        { 'A', '2', '-', '1'},  /* Addr. 6 Adj G2-V1 */
        { 'A', '2', '-', '2'},  /* Addr. 7 Adj G2-V2 */
        { 'A', '2', '-', '3'},  /* Addr. 8 Adj G2-V3 */
        { 'A', '2', '-', '4'},  /* Addr. 9 Adj G2-V4 */
        { 'A', '2', '-', '5'},  /* Addr.10 Adj G2-V5 */
        { 'A', '3', '-', '1'},  /* Addr.11 Adj G3-V1 */
        { 'A', '3', '-', '2'},  /* Addr.12 Adj G3-V2 */
        { 'A', '3', '-', '3'},  /* Addr.13 Adj G3-V3 */
        { 'A', '3', '-', '4'},  /* Addr.14 Adj G3-V4 */
        { 'A', '3', '-', '5'},  /* Addr.15 Adj G3-V5 */
        { 'A', '4', '-', '1'},  /* Addr.16 Adj G4-V1 */
        { 'A', '4', '-', '2'},  /* Addr.17 Adj G4-V2 */
        { 'A', '4', '-', '3'},  /* Addr.18 Adj G4-V3 */
        { 'A', '4', '-', '4'},  /* Addr.19 Adj G4-V4 */
        { 'A', '4', '-', '5'}   /* Addr.20 Adj G4-V5 */
    },
    {   /* Prog.47 PRINT MODIFIER MNEMONICS */
        { 'P', 'R', 'T', ' ', 'M', 'O', 'D', ' ', '#', '1'}, /* Addr.1 Print Mod #1 */
        { 'P', 'R', 'T', ' ', 'M', 'O', 'D', ' ', '#', '2'}, /* Addr.2 Print Mod #2 */
        { 'P', 'R', 'T', ' ', 'M', 'O', 'D', ' ', '#', '3'}, /* Addr.3 Print Mod #3 */
        { 'P', 'R', 'T', ' ', 'M', 'O', 'D', ' ', '#', '4'}, /* Addr.4 Print Mod #4 */
        { 'P', 'R', 'T', ' ', 'M', 'O', 'D', ' ', '#', '5'}, /* Addr.5 Print Mod #5 */
        { 'P', 'R', 'T', ' ', 'M', 'O', 'D', ' ', '#', '6'}, /* Addr.6 Print Mod #6 */
        { 'P', 'R', 'T', ' ', 'M', 'O', 'D', ' ', '#', '7'}, /* Addr.7 Print Mod #7 */
        { 'P', 'R', 'T', ' ', 'M', 'O', 'D', ' ', '#', '8'}  /* Addr.8 Print Mod #8 */
    },
    {   /* Prog.48 REPORT CODE MNEMONICS */
    { 'M', ' ', 'D', 'E', 'P', 'T', ' ', '1', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'}, /* Addr. 1 Major Dept. #1 */
    { 'M', ' ', 'D', 'E', 'P', 'T', ' ', '2', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'}, /* Addr. 2 Major Dept. #2 */
    { 'M', ' ', 'D', 'E', 'P', 'T', ' ', '3', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'}, /* Addr. 3 Major Dept. #3 */
    { 'M', ' ', 'D', 'E', 'P', 'T', ' ', '4', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'}, /* Addr. 4 Major Dept. #4 */
    { 'M', ' ', 'D', 'E', 'P', 'T', ' ', '5', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'}, /* Addr. 5 Major Dept. #5 */
    { 'M', ' ', 'D', 'E', 'P', 'T', ' ', '6', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'}, /* Addr. 6 Major Dept. #6 */
    { 'M', ' ', 'D', 'E', 'P', 'T', ' ', '7', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'}, /* Addr. 7 Major Dept. #7 */
    { 'M', ' ', 'D', 'E', 'P', 'T', ' ', '8', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'}, /* Addr. 8 Major Dept. #8 */
    { 'M', ' ', 'D', 'E', 'P', 'T', ' ', '9', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'}, /* Addr. 9 Major Dept. #9 */
    { 'M', ' ', 'D', 'E', 'P', 'T', '1', '0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'}, /* Addr.10 Major Dept. #10 */
        { 'M', ' ', 'D', 'E', 'P', 'T', '1', '1', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'}, /* Addr.11 Major Dept. #11, Saratoga */
        { 'M', ' ', 'D', 'E', 'P', 'T', '1', '2', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'}, /* Addr.12 Major Dept. #12, Saratoga */
        { 'M', ' ', 'D', 'E', 'P', 'T', '1', '3', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'}, /* Addr.13 Major Dept. #13, Saratoga */
        { 'M', ' ', 'D', 'E', 'P', 'T', '1', '4', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'}, /* Addr.14 Major Dept. #14, Saratoga */
        { 'M', ' ', 'D', 'E', 'P', 'T', '1', '5', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'}, /* Addr.15 Major Dept. #15, Saratoga */
        { 'M', ' ', 'D', 'E', 'P', 'T', '1', '6', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'}, /* Addr.16 Major Dept. #16, Saratoga */
        { 'M', ' ', 'D', 'E', 'P', 'T', '1', '7', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'}, /* Addr.17 Major Dept. #17, Saratoga */
        { 'M', ' ', 'D', 'E', 'P', 'T', '1', '8', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'}, /* Addr.18 Major Dept. #18, Saratoga */
        { 'M', ' ', 'D', 'E', 'P', 'T', '1', '9', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'}, /* Addr.19 Major Dept. #19, Saratoga */
        { 'M', ' ', 'D', 'E', 'P', 'T', '2', '0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'}, /* Addr.20 Major Dept. #20, Saratoga */
        { 'M', ' ', 'D', 'E', 'P', 'T', '2', '1', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'}, /* Addr.21 Major Dept. #21, Saratoga */
        { 'M', ' ', 'D', 'E', 'P', 'T', '2', '2', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'}, /* Addr.22 Major Dept. #22, Saratoga */
        { 'M', ' ', 'D', 'E', 'P', 'T', '2', '3', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'}, /* Addr.23 Major Dept. #23, Saratoga */
        { 'M', ' ', 'D', 'E', 'P', 'T', '2', '4', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'}, /* Addr.24 Major Dept. #24, Saratoga */
        { 'M', ' ', 'D', 'E', 'P', 'T', '2', '5', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'}, /* Addr.25 Major Dept. #25, Saratoga */
        { 'M', ' ', 'D', 'E', 'P', 'T', '2', '6', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'}, /* Addr.26 Major Dept. #26, Saratoga */
        { 'M', ' ', 'D', 'E', 'P', 'T', '2', '7', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'}, /* Addr.27 Major Dept. #27, Saratoga */
        { 'M', ' ', 'D', 'E', 'P', 'T', '2', '8', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'}, /* Addr.28 Major Dept. #28, Saratoga */
        { 'M', ' ', 'D', 'E', 'P', 'T', '2', '9', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'}, /* Addr.29 Major Dept. #29, Saratoga */
        { 'M', ' ', 'D', 'E', 'P', 'T', '3', '0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'},  /* Addr.30 Major Dept. #30, Saratoga */
    },
    {   /* Prog.49 AUTO ALTERNATIVE KITCHEN PTR. */
		4, 3, 2, 1, 0, 0, 0, 0
    },
    {   /* Prog 54 */

		/* Hotel Id save Area       */
		{ '0', '0'},

		/* SLD Id save area         */
		{ '0', '0', '0', '0'},

		/* Product Id save area     */
		{ '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'},

		/* Tender 1 to 11 EPT Action code */
		{ '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0','0','0','0','0','0','0','0','0','0'},

		/* Approval code save area  */
		{ 'A', 'P'}
    },
    {   /* Prog.57 24 CHARACTERS MNEMONICS, CLASS_PARACHAR24, ParaChar24[MAX_CH24_NO][PARA_CHAR24_LEN] */
        { 'V', 'A', 'L', 'I', 'D', 'A', 'T', 'I',   /* Addr.1 1st Validation Header */
          'O', 'N', ' ', 'H', 'E', 'A', 'D', 'E',
          'R', ' ', ' ', 'F', 'I', 'R', 'S', 'T'},

        { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',   /* Addr.2 2nd Validation Header */
          'V', 'A', 'L', 'I', 'D', 'A', 'T', 'I',
          'O', 'N', ' ', '2', '\0', '\0', '\0', '\0'},

        { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',   /* Addr.3 3rd Validation Header */
          ' ', ' ', ' ', ' ', ' ', 'V', 'A', 'L',
          ' ', ' ', ' ', ' ', 'L', 'A', 'S', 'T'},

        { 'C', 'O', 'N', 'T', 'I', 'N', 'U', 'E',   /* Addr.4 Continue to Next Slip */
          ' ', 'T', 'O', ' ', 'N', 'E', 'X', 'T',
          ' ', 'B', 'I', 'L', 'L', '\0', '\0', '\0'},

        { '*', '*', '*', 'I', 'N', 'V', 'A', 'L',   /* Addr.5 Training Mode Header */
          'I', 'D', ' ', 'T', 'R', 'A', 'N', 'S',
          'A', 'C', 'T', 'I', 'O', 'N', '*', '*'},

        { '!', '!', '!', ' ', 'T', '-', 'V', 'O',   /* Addr.6 Transaction Void Header */
          'I', 'D', ' ', '!', '!', '!', ' ', 'T',
          '-', 'V', 'O', 'I', 'D', ' ', '!', '!'},

        { 'T', 'A', 'K', 'E', ' ', 'T', 'O', ' ',   /* Addr.7 Take to Kitchen */
          'K', 'I', 'T', 'C', 'H', 'E', 'N', '\0',
          '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'},

        { '*', '*', '*', '*', '*', '*', '*', '*',   /* Addr.8 1st Receipt Header */
          '*', '*', '*', '*', '*', '*', '*', '*',
          '*', '*', '*', '*', '*', '*', '*', '*'},

        { '*', ' ', ' ', ' ', ' ', ' ', ' ', 'N',   /* Addr.9 2nd Receipt Header */
          'C', 'R', '2', '1', '7', '2', ' ', ' ',
          ' ', ' ', ' ', ' ', ' ', ' ', ' ', '*'},

        { '*', ' ', ' ', ' ', ' ', 'S', 'A', 'R',   /* Addr.10 3rd Receipt Header */
          'A', 'T', 'O', 'G', 'A', ' ', 'M', 'O',
          'D', 'E', 'L', ' ', ' ', ' ', ' ', '*'},

        { '*', '*', '*', '*', '*', '*', '*', '*',   /* Addr.11 4th Receipt Header */
          '*', '*', '*', '*', '*', '*', '*', '*',
          '*', '*', '*', '*', '*', '*', '*', '*'},

        { '!', '!', '!', ' ', 'P', 'O', 'S', 'T',   /* Addr.12 Post Receipt Message */
          ' ', '!', '!', '!', ' ', 'P', 'O', 'S',
          'T', ' ', '!', '!', '!', '\0', '\0', '\0'},

        { '*', '*', '*', ' ', 'M', 'O', 'N', 'E',   /* Addr.13 Money Declaration, 2172 */
          'Y', ' ', 'D', 'E', 'C', 'L', 'A', 'R',
          'A', 'T', 'I', 'O', 'N', '*', '*', '*'},

        { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',   /* Addr.14 dummy */
          ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
          ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},

        { 'C', 'H', 'E', 'C', 'K', ' ', 'E', 'N',   /* Addr.15 Check Endorsement Header#1, 21RFC05402 */
          'D', 'O', 'R', 'S', 'E', 'M', 'E', 'N',
          'T', ' ', 'H', 'E', 'A', 'D', '#', '1'},

        { 'C', 'H', 'E', 'C', 'K', ' ', 'E', 'N',   /* Addr.16 Check Endorsement Header#2 */
          'D', 'O', 'R', 'S', 'E', 'M', 'E', 'N',
          'T', ' ', 'H', 'E', 'A', 'D', '#', '2'},

        { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',   /* Addr.17 dummy */
          ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
          ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},

        { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',   /* Addr.18 dummy */
          ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
          ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},

        { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',   /* Addr.19 dummy */
          ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
          ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},

        { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',   /* Addr.20 dummy */
          ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
          ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}

    },
    {   /* Prog.60 TOTAL KEY TYPE ASSIGNMENT - Total Keys #3 - #8 then #10 - #20. #1, #2, and #9 are special purpose. */
		// #03  #04  #05  #06  #07  #08  #10  #11
		    21,  42,  63,  89, 111,  21,   0,   0
    },
    {   /* Prog.61 TOTAL KEY CONTROL */
		{ 0, 0, 0, 0, 0, 0},
		{ 0, 0, 0, 0, 0, 0},
		{ 0, 0, 0, 0, 0, 0},
		{ 0, 0, 0, 0, 0, 0},
		{ 0, 0, 0, 0, 0, 0},
		{ 0, 0, 0, 0, 0, 0},
		{ 0, 0, 0, 0, 0, 0},
		{ 0, 0, 0, 0, 0, 0}
    },
    {   /* Prog.62 TENDER PARAMETER */
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    },
                                                        /* A/C #4, 2172 */
  {/* page 1 */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 20 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'2',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'3',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'4',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'5',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'6',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'7',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'8',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'9',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'2','0',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'3','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'4','0',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'5','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'5','1',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'5','2',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'5','3',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'5','4',0,0,0,0,0,0,0,0,0,0,0,0, /* 40 */

    0,0,'5','5',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'6','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'7','0',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'8','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'9','0',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','0',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','1',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','2',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','3',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','4',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','5',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','6',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','7',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','8',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','9',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','1','0',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','2','0',0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 60 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 80 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 100 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 120 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 140 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 160 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                                    /* 165 */
    /* page 2 */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 20 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'2',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'3',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'4',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'5',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'6',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'7',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'8',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'9',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'2','0',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'3','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'4','0',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'5','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'5','1',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'5','2',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'5','3',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'5','4',0,0,0,0,0,0,0,0,0,0,0,0, /* 40 */

    0,0,'5','5',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'6','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'7','0',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'8','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'9','0',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','0',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','1',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','2',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','3',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','4',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','5',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','6',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','7',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','8',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','9',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','1','0',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','2','0',0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 60 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 80 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 100 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 120 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 140 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 160 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                                    /* 165 */

    /* page 3 */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 20 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'2',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'3',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'4',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'5',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'6',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'7',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'8',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'9',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'2','0',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'3','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'4','0',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'5','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'5','1',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'5','2',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'5','3',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'5','4',0,0,0,0,0,0,0,0,0,0,0,0, /* 40 */

    0,0,'5','5',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'6','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'7','0',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'8','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'9','0',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','0',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','1',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','2',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','3',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','4',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','5',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','6',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','7',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','8',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','9',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','1','0',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','2','0',0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 60 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 80 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 100 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 120 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 140 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 160 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                                    /* 165 */

    /* page 4 */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 20 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'2',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'3',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'4',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'5',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'6',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'7',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'8',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'9',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'2','0',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'3','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'4','0',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'5','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'5','1',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'5','2',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'5','3',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'5','4',0,0,0,0,0,0,0,0,0,0,0,0, /* 40 */

    0,0,'5','5',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'6','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'7','0',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'8','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'9','0',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','0',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','1',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','2',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','3',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','4',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','5',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','6',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','7',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','8',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','9',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','1','0',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','2','0',0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 60 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 80 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 100 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 120 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 140 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 160 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                                    /* 165 */

    /* page 5 */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 20 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'2',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'3',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'4',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'5',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'6',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'7',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'8',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'9',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'2','0',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'3','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'4','0',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'5','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'5','1',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'5','2',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'5','3',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'5','4',0,0,0,0,0,0,0,0,0,0,0,0, /* 40 */

    0,0,'5','5',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'6','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'7','0',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'8','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'9','0',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','0',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','1',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','2',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','3',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','4',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','5',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','6',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','7',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','8',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','9',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','1','0',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','2','0',0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 60 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 80 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 100 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 120 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 140 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 160 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                                    /* 165 */

    /* page 6 */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 20 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'2',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'3',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'4',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'5',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'6',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'7',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'8',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'9',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'2','0',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'3','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'4','0',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'5','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'5','1',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'5','2',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'5','3',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'5','4',0,0,0,0,0,0,0,0,0,0,0,0, /* 40 */

    0,0,'5','5',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'6','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'7','0',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'8','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'9','0',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','0',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','1',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','2',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','3',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','4',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','5',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','6',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','7',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','8',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','9',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','1','0',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','2','0',0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 60 */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 80 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 100 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 120 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 140 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 160 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                                    /* 165 */

    /* page 7 */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 20 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'2',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'3',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'4',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'5',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'6',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'7',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'8',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'9',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'2','0',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'3','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'4','0',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'5','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'5','1',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'5','2',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'5','3',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'5','4',0,0,0,0,0,0,0,0,0,0,0,0, /* 40 */

    0,0,'5','5',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'6','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'7','0',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'8','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'9','0',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','0',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','1',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','2',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','3',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','4',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','5',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','6',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','7',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','8',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','9',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','1','0',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','2','0',0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 60 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 80 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 100 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 120 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 140 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 160 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                                    /* 165 */

    /* page 8 */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 20 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'2',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'3',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'4',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'5',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'6',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'7',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'8',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'9',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'2','0',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'3','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'4','0',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'5','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'5','1',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'5','2',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'5','3',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'5','4',0,0,0,0,0,0,0,0,0,0,0,0, /* 40 */

    0,0,'5','5',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'6','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'7','0',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'8','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'9','0',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','0',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','1',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','2',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','3',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','4',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','5',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','6',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','7',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','8',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','9',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','1','0',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','2','0',0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 60 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 80 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 100 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 120 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 140 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 160 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                                    /* 165 */

    /* page 9 */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 20 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'2',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'3',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'4',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'5',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'6',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'7',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'8',0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'9',0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'2','0',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'3','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'4','0',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'5','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'5','1',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'5','2',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'5','3',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'5','4',0,0,0,0,0,0,0,0,0,0,0,0, /* 40 */

    0,0,'5','5',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'6','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'7','0',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'8','0',0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,'9','0',0,0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','0',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','1',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','2',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','3',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','4',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','5',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','6',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','7',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','0','8',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','0','9',0,0,0,0,0,0,0,0,0,0,0, 0,0,'1','1','0',0,0,0,0,0,0,0,0,0,0,0,
    0,0,'1','2','0',0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 60 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 80 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 100 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 120 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 140 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   /* 160 */

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0                                 /* 165 */

    },

    {   /* A/C 5 SET CONTROL TABLE OF MENU PAGE */
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0    /* 2172 */
    },
    {   /* A/C 6 MANUAL ALTERNATIVE KITCHEN PTR. */
		0, 0, 0, 0, 0, 0, 0, 0
    },
    {   /* A/C 7 CASHIER A/B KEYS ASSIGNMENT */
		0, 0
	},{	// AC #7 Job Code Ranges for A/B cashdrawer assignment
		0, 0, 0, 0
	},
    {   /* A/C 84 SET ROUNDING TABLE */
		{ 0, 0, 0},
		{ 0, 0, 0},
		{ 0, 0, 0},
		{ 0, 0, 0},
		{ 0, 0, 0},
		{ 0, 0, 0},
		{ 0, 0, 0}, /* 2172 */
		{ 0, 0, 0},
		{ 0, 0, 0},
		{ 0, 0, 0},
		{ 0, 0, 0},
		{ 0, 0, 0},
	/*** NCR2172 ***/
		{ 0, 0, 0},
		{ 0, 0, 0},
		{ 0, 0, 0}
    },
    {   /* A/C 86 SET DISCOUNT/BONUS % RATE */
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    },
    {   /* A/C 88 SET SALES PROMOTION MESSAGE */

    /* Addr. 1st Line */
		{'Y','O','U','R',' ','R','E','C','E','I','P','T'},
    /* Addr. 2st Line */
		{' ','T','H','A','N','K',' ',' ','Y','O','U',' '},
    /* Addr. 3st Line */
		{' ','N','C','R',' ',' ','S','T','O','R','E',' '}
    },
    {   /* A/C 89 SET CURRENCY CONVERSION RATE */
		0L, 0L,
		0L, 0L, 0L, 0L, 0L, 0L  /* 2172 */
    },
    {   /* A/C 169 TONE CONTROL */
		3
    },
    {   /* A/C 116 ASSIGN SET MENU OF PLU */
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},/* 1 */
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},/* 2 */
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},/* 3 */
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},/* 4 */
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},/* 5 */
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},/* 6 */
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},/* 7 */
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},/* 8 */
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},/* 9 */
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},/* 10 */
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},/* 11 */
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},/* 12 */
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},/* 13 */
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},/* 14 */
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},/* 15 */
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},/* 16 */
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},/* 17 */
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},/* 18 */
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},/* 19 */
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}/* 20 */
    },
    {   /* A/C 124-126 TAX TABLE PROGRAMMING */
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		0,0,0,0, 0,0,0,0
    },
    {   /* A/C 127 SET TAX/SERVICE/RECIPROCAL RATES */
		0L, 0L, 0L, 0L, 0L
    },
    {   /* MISC. PARAMETER *//* ### ADD Saratoga (V1_0.02)(051800) */
		0L,0L,0L,0L,0L,0L,0L
    },
    {   /* A/C 111 TARE TABLE */
		0,0,0,0, 0,0,0,0, 0,0
    },
    {   /* Prog.15 PRESET AMOUNT CASH TENDER */
		0L, 0L, 0L, 0L
    },
    {   /* Prog. 50 SHARED PRINTER */
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		0,0,0,0, 0,0,0,0,
		0,0,0,0, 0,0,0,0    /* 2172 */
    },
    {   /*  A/C 87 42 CHARACTERS MNEMONICS for SOFT CHECK */
        { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
          0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
          0,0,0,0, 0,0,0,0, 0,0},
        { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
          0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
          0,0,0,0, 0,0,0,0, 0,0},
        { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
          0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
          0,0,0,0, 0,0,0,0, 0,0},
        { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
          0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
          0,0,0,0, 0,0,0,0, 0,0},
        { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
          0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
          0,0,0,0, 0,0,0,0, 0,0},
        { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
          0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
          0,0,0,0, 0,0,0,0, 0,0},
        { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
          0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
          0,0,0,0, 0,0,0,0, 0,0},
        { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
          0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
          0,0,0,0, 0,0,0,0, 0,0},
        { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
          0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
          0,0,0,0, 0,0,0,0, 0,0},
        { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
          0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
          0,0,0,0, 0,0,0,0, 0,0},
    },
    {   /* A/C 130 PIG RULE TABLE */
		0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L
    },
    {   /* A/C 160 SET ORDER ENTRY PROMPT TABLE, NCR2172, AC160 - Para.ParaOep[MAX_TABLE_NO][MAX_TABLE_ADDRESS] */
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
    },
    {   /* A/C 162 SET FLEXIBLE DRIVE THROUGH PARAMETER TABLE, AC162 - Para.ParaFlexDrive[MAX_FLEXDRIVE_SIZE] */
        // AC 162 default for Store/Recall System type - two counter terminals, no drive thru terminals
		0,0, 0,0,                                    // Delivery #1, #2  Payment #1, #2
        0,12, 0,12, 0,12, 0,12, 0,12, 0,12,          // System Type, Terminal Type for terminals 1 through 6
		0,0, 0,0, 0,0, 0,0, 0,0, 0,0,                // System Type, Terminal Type for terminals 7 through 12
        0,0, 0,0, 0,0, 0,0                           // System Type, Terminal Type for terminals 13 through 16
    },
    {   /* A/C 133 SERVICE TIME PARAMETER */
		0, 0
    },
    {   /* A/C 154 LABOR COST VALUE, AC154 - Para.ParaLaborCost[MAX_LABORCOST_SIZE] */
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		0,0,0,0
    },
    {   /* A/C 137 BEVERAGE DISPENSER PARAMETER, AC137 - Para.ParaDispPara[MAX_DISPPARA_SIZE] */
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0},
		{ 0, 0}, { 0, 0}, { 0, 0}
    },
    { /* Prog #51 KDS IP Address, 2172 */
		{ 0, 0, 0, 0},
		{ 0, 0, 0, 0},
		{ 0, 0, 0, 0},
		{ 0, 0, 0, 0},
		{ 0, 0, 0, 0},
		{ 0, 0, 0, 0},
		{ 0, 0, 0, 0},         /* NCR2172 */
		{ 0, 0, 0, 0}          /* NCR2172 */
    },
    /* A/C #70, 2172 */
    {
		0,0,0,0,0,0,
		0,0,0,0,0,0,
		0,0,0,0,0,0,
		0,0,0,0,0,0,
		0,0,0,0,0,0,
		0,0,0,0,0,0
	},
    /* A/C #208, 2172 */
    {
		0,0,0
    },
        {   /* A/C 115 SET Dept NO.ON MENU PAGE, 2172 */
           1,   2,   3,   4,   5,   /* Addr.1 - 5 */
           6,   7,   8,   9,  10,   /* Addr.6 - 10 */
           0,   0,   0,   0,   0,   /* Addr.11 - 15 */
           0,   0,   0,   0,   0,   /* Addr.16 - 20 */
           0,   0,   0,   0,   0,   /* Addr.21 - 25 */
           0,   0,   0,   0,   0,   /* Addr.26 - 30 */
           0,   0,   0,   0,   0,   /* Addr.31 - 35 */
           0,   0,   0,   0,   0,   /* Addr.36 - 40 */
          41,  42,  43,  44,  45,   /* Addr.41 - 45 */
          46,  47,  48,  49,  50,   /* Addr.46 - 50 */
                                    /* Page 2 */
           1,   2,   3,   4,   5,   /* Addr.1 - 5 */
           6,   7,   8,   9,  10,   /* Addr.6 - 10 */
           0,   0,   0,   0,   0,   /* Addr.11 - 15 */
           0,   0,   0,   0,   0,   /* Addr.16 - 20 */
           0,   0,   0,   0,   0,   /* Addr.21 - 25 */
           0,   0,   0,   0,   0,   /* Addr.26 - 30 */
           0,   0,   0,   0,   0,   /* Addr.31 - 35 */
           0,   0,   0,   0,   0,   /* Addr.36 - 40 */
          41,  42,  43,  44,  45,   /* Addr.41 - 45 */
          46,  47,  48,  49,  50,   /* Addr.46 - 50 */
                                    /* Page 3 */
           1,   2,   3,   4,   5,   /* Addr.1 - 5 */
           6,   7,   8,   9,  10,   /* Addr.6 - 10 */
           0,   0,   0,   0,   0,   /* Addr.11 - 15 */
           0,   0,   0,   0,   0,   /* Addr.16 - 20 */
           0,   0,   0,   0,   0,   /* Addr.21 - 25 */
           0,   0,   0,   0,   0,   /* Addr.26 - 30 */
           0,   0,   0,   0,   0,   /* Addr.31 - 35 */
           0,   0,   0,   0,   0,   /* Addr.36 - 40 */
          41,  42,  43,  44,  45,   /* Addr.41 - 45 */
          46,  47,  48,  49,  50,   /* Addr.46 - 50 */
                                    /* Page 4 */
           1,   2,   3,   4,   5,   /* Addr.1 - 5 */
           6,   7,   8,   9,  10,   /* Addr.6 - 10 */
           0,   0,   0,   0,   0,   /* Addr.11 - 15 */
           0,   0,   0,   0,   0,   /* Addr.16 - 20 */
           0,   0,   0,   0,   0,   /* Addr.21 - 25 */
           0,   0,   0,   0,   0,   /* Addr.26 - 30 */
           0,   0,   0,   0,   0,   /* Addr.31 - 35 */
           0,   0,   0,   0,   0,   /* Addr.36 - 40 */
          41,  42,  43,  44,  45,   /* Addr.41 - 45 */
          46,  47,  48,  49,  50,   /* Addr.46 - 50 */
                                    /* Page 5 */
           1,   2,   3,   4,   5,   /* Addr.1 - 5 */
           6,   7,   8,   9,  10,   /* Addr.6 - 10 */
           0,   0,   0,   0,   0,   /* Addr.11 - 15 */
           0,   0,   0,   0,   0,   /* Addr.16 - 20 */
           0,   0,   0,   0,   0,   /* Addr.21 - 25 */
           0,   0,   0,   0,   0,   /* Addr.26 - 30 */
           0,   0,   0,   0,   0,   /* Addr.31 - 35 */
           0,   0,   0,   0,   0,   /* Addr.36 - 40 */
          41,  42,  43,  44,  45,   /* Addr.41 - 45 */
          46,  47,  48,  49,  50,   /* Addr.46 - 50 */
                                    /* Page 6 */
           1,   2,   3,   4,   5,   /* Addr.1 - 5 */
           6,   7,   8,   9,  10,   /* Addr.6 - 10 */
           0,   0,   0,   0,   0,   /* Addr.11 - 15 */
           0,   0,   0,   0,   0,   /* Addr.16 - 20 */
           0,   0,   0,   0,   0,   /* Addr.21 - 25 */
           0,   0,   0,   0,   0,   /* Addr.26 - 30 */
           0,   0,   0,   0,   0,   /* Addr.31 - 35 */
           0,   0,   0,   0,   0,   /* Addr.36 - 40 */
          41,  42,  43,  44,  45,   /* Addr.41 - 45 */
          46,  47,  48,  49,  50,   /* Addr.46 - 50 */
                                    /* Page 7 */
           1,   2,   3,   4,   5,   /* Addr.1 - 5 */
           6,   7,   8,   9,  10,   /* Addr.6 - 10 */
           0,   0,   0,   0,   0,   /* Addr.11 - 15 */
           0,   0,   0,   0,   0,   /* Addr.16 - 20 */
           0,   0,   0,   0,   0,   /* Addr.21 - 25 */
           0,   0,   0,   0,   0,   /* Addr.26 - 30 */
           0,   0,   0,   0,   0,   /* Addr.31 - 35 */
           0,   0,   0,   0,   0,   /* Addr.36 - 40 */
          41,  42,  43,  44,  45,   /* Addr.41 - 45 */
          46,  47,  48,  49,  50,   /* Addr.46 - 50 */
                                    /* Page 8 */
           1,   2,   3,   4,   5,   /* Addr.1 - 5 */
           6,   7,   8,   9,  10,   /* Addr.6 - 10 */
           0,   0,   0,   0,   0,   /* Addr.11 - 15 */
           0,   0,   0,   0,   0,   /* Addr.16 - 20 */
           0,   0,   0,   0,   0,   /* Addr.21 - 25 */
           0,   0,   0,   0,   0,   /* Addr.26 - 30 */
           0,   0,   0,   0,   0,   /* Addr.31 - 35 */
           0,   0,   0,   0,   0,   /* Addr.36 - 40 */
          41,  42,  43,  44,  45,   /* Addr.41 - 45 */
          46,  47,  48,  49,  50,   /* Addr.46 - 50 */
                                    /* Page 9 */
           1,   2,   3,   4,   5,   /* Addr.1 - 5 */
           6,   7,   8,   9,  10,   /* Addr.6 - 10 */
           0,   0,   0,   0,   0,   /* Addr.11 - 15 */
           0,   0,   0,   0,   0,   /* Addr.16 - 20 */
           0,   0,   0,   0,   0,   /* Addr.21 - 25 */
           0,   0,   0,   0,   0,   /* Addr.26 - 30 */
           0,   0,   0,   0,   0,   /* Addr.31 - 35 */
           0,   0,   0,   0,   0,   /* Addr.36 - 40 */
          41,  42,  43,  44,  45,   /* Addr.41 - 45 */
          46,  47,  48,  49,  50    /* Addr.46 - 50 */
    },
    {   /* SPECIAL COUNTER */
		0, 0, 0, 0
    },
	//Auto Combinational Coupon
	{
		0,0,0,0,0,0,0,0
	},
	//Color Ref AC(209)
	{	// foreground color, background color
		  0, 0xffffff,	  0, 0xffffff,	  0xffffff, 0x0000ff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,
		  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff,	  0, 0xffffff
		},
		{
			  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		},
	// Charge Post Manager
	{
		0, 0, 0, 0, 0, 0
	},
	// TERMININFO terminal information
	{
			// Terminal # 1
		{
			1252, 0x09, 0, 1, 1, KEYBOARD_TOUCH, 10,
			{	// Font family name
				_T('A'),  _T('r'),  _T('i'),  _T('a'),  _T('l'),  _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0')
			},
			{	// Unlock code
				_T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')
			},
			{	// Register Serial Number
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')
			},
			{	// Default menu file name for layout
				_T('D'),  _T('e'),  _T('f'),  _T('a'),  _T('u'),  _T('l'),
				_T('t'), _T('.'), _T('d'), _T('a'), _T('t'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0')
			},
			{ // IP address
					192, 168, 0, 1
			}
		},
			// Terminal # 2
		{
			1252, 0x09, 0, 1, 2, KEYBOARD_TOUCH, 10,
			{	// Font family name
				_T('A'),  _T('r'),  _T('i'),  _T('a'),  _T('l'),  _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0')
			},
			{	// Unlock code
				_T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')
			},
			{	// Register Serial Number
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')
			},
			{	// Default menu file name for layout
				_T('D'),  _T('e'),  _T('f'),  _T('a'),  _T('u'),  _T('l'),
				_T('t'), _T('.'), _T('d'), _T('a'), _T('t'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0')
			},
			{ // IP address
					192, 168, 0, 2
			}
		},
			// Terminal # 3
		{
			1252, 0x09, 0, 1, 3, KEYBOARD_TOUCH, 10,
			{	// Font family name
				_T('A'),  _T('r'),  _T('i'),  _T('a'),  _T('l'),  _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0')
			},
			{	// Unlock code
				_T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')
			},
			{	// Register Serial Number
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')
			},
			{	// Default menu file name for layout
				_T('D'),  _T('e'),  _T('f'),  _T('a'),  _T('u'),  _T('l'),
				_T('t'), _T('.'), _T('d'), _T('a'), _T('t'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0')
			},
			{ // IP address
					192, 168, 0, 3
			}
		},
			// Terminal # 4
		{
			1252, 0x09, 0, 1, 4, KEYBOARD_TOUCH, 10,
			{	// Font family name
				_T('A'),  _T('r'),  _T('i'),  _T('a'),  _T('l'),  _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0')
			},
			{	// Unlock code
				_T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')
			},
			{	// Register Serial Number
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')
			},
			{	// Default menu file name for layout
				_T('D'),  _T('e'),  _T('f'),  _T('a'),  _T('u'),  _T('l'),
				_T('t'), _T('.'), _T('d'), _T('a'), _T('t'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0')
			},
			{ // IP address
					192, 168, 0, 4
			}
		},
			// Terminal # 5
		{
			1252, 0x09, 0, 1, 5, KEYBOARD_TOUCH, 10,
			{	// Font family name
				_T('A'),  _T('r'),  _T('i'),  _T('a'),  _T('l'),  _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0')
			},
			{	// Unlock code
				_T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')
			},
			{	// Register Serial Number
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')
			},
			{	// Default menu file name for layout
				_T('D'),  _T('e'),  _T('f'),  _T('a'),  _T('u'),  _T('l'),
				_T('t'), _T('.'), _T('d'), _T('a'), _T('t'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0')
			},
			{ // IP address
					192, 168, 0, 5
			}
		},
			// Terminal # 6
		{
			1252, 0x09, 0, 1, 6, KEYBOARD_TOUCH, 10,
			{	// Font family name
				_T('A'),  _T('r'),  _T('i'),  _T('a'),  _T('l'),  _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0')
			},
			{	// Unlock code
				_T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')
			},
			{	// Register Serial Number
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')
			},
			{	// Default menu file name for layout
				_T('D'),  _T('e'),  _T('f'),  _T('a'),  _T('u'),  _T('l'),
				_T('t'), _T('.'), _T('d'), _T('a'), _T('t'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0')
			},
			{ // IP address
					192, 168, 0, 6
			}
		},
			// Terminal # 7
		{
			1252, 0x09, 0, 1, 7, KEYBOARD_TOUCH, 10,
			{	// Font family name
				_T('A'),  _T('r'),  _T('i'),  _T('a'),  _T('l'),  _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0')
			},
			{	// Unlock code
				_T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')
			},
			{	// Register Serial Number
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')
			},
			{	// Default menu file name for layout
				_T('D'),  _T('e'),  _T('f'),  _T('a'),  _T('u'),  _T('l'),
				_T('t'), _T('.'), _T('d'), _T('a'), _T('t'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0')
			},
			{ // IP address
					192, 168, 0, 7
			}
		},
			// Terminal # 8
		{
			1252, 0x09, 0, 1, 8, KEYBOARD_TOUCH, 10,
			{	// Font family name
				_T('A'),  _T('r'),  _T('i'),  _T('a'),  _T('l'),  _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0')
			},
			{	// Unlock code
				_T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')
			},
			{	// Register Serial Number
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')
			},
			{	// Default menu file name for layout
				_T('D'),  _T('e'),  _T('f'),  _T('a'),  _T('u'),  _T('l'),
				_T('t'), _T('.'), _T('d'), _T('a'), _T('t'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0')
			},
			{ // IP address
					192, 168, 0, 8
			}
		},
			// Terminal # 9
		{
			1252, 0x09, 0, 1, 9, KEYBOARD_TOUCH, 10,
			{	// Font family name
				_T('A'),  _T('r'),  _T('i'),  _T('a'),  _T('l'),  _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0')
			},
			{	// Unlock code
				_T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')
			},
			{	// Register Serial Number
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')
			},
			{	// Default menu file name for layout
				_T('D'),  _T('e'),  _T('f'),  _T('a'),  _T('u'),  _T('l'),
				_T('t'), _T('.'), _T('d'), _T('a'), _T('t'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0')
			},
			{ // IP address
					192, 168, 0, 9
			}
		},
			// Terminal # 10
		{
			1252, 0x09, 0, 1, 10, KEYBOARD_TOUCH, 10,
			{	// Font family name
				_T('A'),  _T('r'),  _T('i'),  _T('a'),  _T('l'),  _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0')
			},
			{	// Unlock code
				_T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')
			},
			{	// Register Serial Number
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')
			},
			{	// Default menu file name for layout
				_T('D'),  _T('e'),  _T('f'),  _T('a'),  _T('u'),  _T('l'),
				_T('t'), _T('.'), _T('d'), _T('a'), _T('t'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0')
			},
			{ // IP address
					192, 168, 0, 10
			}
		},
			// Terminal # 11
		{
			1252, 0x09, 0, 1, 11, KEYBOARD_TOUCH, 10,
			{	// Font family name
				_T('A'),  _T('r'),  _T('i'),  _T('a'),  _T('l'),  _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0')
			},
			{	// Unlock code
				_T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')
			},
			{	// Register Serial Number
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')
			},
			{	// Default menu file name for layout
				_T('D'),  _T('e'),  _T('f'),  _T('a'),  _T('u'),  _T('l'),
				_T('t'), _T('.'), _T('d'), _T('a'), _T('t'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0')
			},
			{ // IP address
					192, 168, 0, 11
			}
		},
			// Terminal # 12
		{
			1252, 0x09, 0, 1, 12, KEYBOARD_TOUCH, 10,
			{	// Font family name
				_T('A'),  _T('r'),  _T('i'),  _T('a'),  _T('l'),  _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0')
			},
			{	// Unlock code
				_T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')
			},
			{	// Register Serial Number
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')
			},
			{	// Default menu file name for layout
				_T('D'),  _T('e'),  _T('f'),  _T('a'),  _T('u'),  _T('l'),
				_T('t'), _T('.'), _T('d'), _T('a'), _T('t'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0')
			},
			{ // IP address
					192, 168, 0, 12
			}
		},
			// Terminal # 13
		{
			1252, 0x09, 0, 1, 13, KEYBOARD_TOUCH, 10,
			{	// Font family name
				_T('A'),  _T('r'),  _T('i'),  _T('a'),  _T('l'),  _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0')
			},
			{	// Unlock code
				_T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')
			},
			{	// Register Serial Number
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')
			},
			{	// Default menu file name for layout
				_T('D'),  _T('e'),  _T('f'),  _T('a'),  _T('u'),  _T('l'),
				_T('t'), _T('.'), _T('d'), _T('a'), _T('t'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0')
			},
			{ // IP address
					192, 168, 0, 13
			}
		},
			// Terminal # 14
		{
			1252, 0x09, 0, 1, 14, KEYBOARD_TOUCH, 10,
			{	// Font family name
				_T('A'),  _T('r'),  _T('i'),  _T('a'),  _T('l'),  _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0')
			},
			{	// Unlock code
				_T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')
			},
			{	// Register Serial Number
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')
			},
			{	// Default menu file name for layout
				_T('D'),  _T('e'),  _T('f'),  _T('a'),  _T('u'),  _T('l'),
				_T('t'), _T('.'), _T('d'), _T('a'), _T('t'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0')
			},
			{ // IP address
					192, 168, 0, 14
			}
		},
			// Terminal # 15
		{
			1252, 0x09, 0, 1, 15, KEYBOARD_TOUCH, 10,
			{	// Font family name
				_T('A'),  _T('r'),  _T('i'),  _T('a'),  _T('l'),  _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0')
			},
			{	// Unlock code
				_T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')
			},
			{	// Register Serial Number
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')
			},
			{	// Default menu file name for layout
				_T('D'),  _T('e'),  _T('f'),  _T('a'),  _T('u'),  _T('l'),
				_T('t'), _T('.'), _T('d'), _T('a'), _T('t'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0')
			},
			{ // IP address
					192, 168, 0, 15
			}
		},
			// Terminal # 16
		{
			1252, 0x09, 0, 1, 16, KEYBOARD_TOUCH, 10,
			{	// Font family name
				_T('A'),  _T('r'),  _T('i'),  _T('a'),  _T('l'),  _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0')
			},
			{	// Unlock code
				_T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),  _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')
			},
			{	// Register Serial Number
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0')
			},
			{	// Default menu file name for layout
				_T('D'),  _T('e'),  _T('f'),  _T('a'),  _T('u'),  _T('l'),
				_T('t'), _T('.'), _T('d'), _T('a'), _T('t'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'), _T('\0'),
				_T('\0'), _T('\0')
			},
			{ // IP address
					192, 168, 0, 16
			}
		}
	},
	// EPT Enhanced Manager - EPT_ENH_Info
	{
		{
			0, 0, 0, 0, 0                  // EPT Enh device number 1
		},
		{
			0, 0, 0, 0, 0                  // EPT Enh device number 2
		},
		{
			0, 0, 0, 0, 0                  // EPT Enh device number 3
		},
		{
			0, 0, 0, 0, 0                  // EPT Enh device number 4
		},
		{
			0, 0, 0, 0, 0                  // EPT Enh device number 5
		},
		{
			0, 0, 0, 0, 0                  // EPT Enh device number 6
		},
		{
			0, 0, 0, 0, 0                  // EPT Enh device number 7
		},
		{
			0, 0, 0, 0, 0                  // EPT Enh device number 8
		},
		{
			0, 0, 0, 0, 0                  // EPT Enh device number 9
		},
		{
			0, 0, 0, 0, 0                  // EPT Enh device number 10
		},
		{
			0, 0, 0, 0, 0                  // EPT Enh device number 11
		},
		{
			0, 0, 0, 0, 0                  // EPT Enh device number 12
		},
		{
			0, 0, 0, 0, 0                  // EPT Enh device number 13
		},
		{
			0, 0, 0, 0, 0                  // EPT Enh device number 14
		},
		{
			0, 0, 0, 0, 0                  // EPT Enh device number 15
		},
		{
			0, 0, 0, 0, 0                  // EPT Enh device number 16
		}
	},
	{  // TenderKeyInformation edited with PEP P98 Tender Key dialog, IDD_P98
		{  // Tender key 1
			0, 0,
			{192,168,0, 99},                  // P98 dialog control IDD_P98_IPADD11, IDD_P98_IPADD12, IDD_P98_IPADD13, IDD_P98_IPADD14
				3200,                         // P98 dialog control IDC_P98_PORTNO
				TENDERKEY_TRANTYPE_CHARGE,    // P98 dialog control IDC_P98_COMBO_TRANTYPE
				TENDERKEY_TRANCODE_SALE,      // P98 dialog control IDC_P98_COMBO_TRANCODE
			{_T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0')},
			_T('0'), 0, 0, 0
		},
		{  // Tender key 2
			0, 0,
			{192,168,0, 99},
				3200,
				TENDERKEY_TRANTYPE_CREDIT, TENDERKEY_TRANCODE_SALE,
			{_T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0')},
			_T('0'), 0, 0, 0
		},
		{  // Tender key 3
			0, 0,
			{192,168,0, 99},
				3200,
				TENDERKEY_TRANTYPE_CREDIT, TENDERKEY_TRANCODE_SALE,
			{_T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0')},
			_T('0'), 0, 0, 0
		},
		{  // Tender key 4
			0, 0,
			{192,168,0, 99},
				9000,
				TENDERKEY_TRANTYPE_CREDIT, TENDERKEY_TRANCODE_SALE,
			{_T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0')},
			_T('0'), 0, 0, 0
		},
		{  // Tender key 5
			0, 0,
			{192,168,0, 99},
				9000,
				TENDERKEY_TRANTYPE_CREDIT, TENDERKEY_TRANCODE_SALE,
			{_T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0')},
			_T('0'), 0, 0, 0
		},
		{  // Tender key 6
			0, 0,
			{192,168,0, 99},
				3200,
				TENDERKEY_TRANTYPE_CREDIT, TENDERKEY_TRANCODE_SALE,
			{_T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0')},
			_T('0'), 0, 0, 0
		},
		{  // Tender key 7
			0, 0,
			{192,168,0, 99},
				3200,
				TENDERKEY_TRANTYPE_CREDIT, TENDERKEY_TRANCODE_SALE,
			{_T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0')},
			_T('0'), 0, 0, 0
		},
		{  // Tender key 8
			0, 0,
			{192,168,0, 99},
				3200,
				TENDERKEY_TRANTYPE_CREDIT, TENDERKEY_TRANCODE_SALE,
			{_T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0')},
			_T('0'), 0, 0, 0
		},
		{  // Tender key 9
			0, 0,
			{192,168,0, 99},
				3200,
				TENDERKEY_TRANTYPE_CREDIT, TENDERKEY_TRANCODE_SALE,
			{_T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0')},
			_T('0'), 0, 0, 0
		},
		{  // Tender key 10
			0, 0,
			{192,168,0, 99},
				3200,
				TENDERKEY_TRANTYPE_CREDIT, TENDERKEY_TRANCODE_SALE,
			{_T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0')},
			_T('0'), 0, 0, 0
		},
		{  // Tender key 11
			0, 0,
			{192,168,0, 99},
				3200,
				TENDERKEY_TRANTYPE_CREDIT, TENDERKEY_TRANCODE_SALE,
			{_T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0')},
			_T('0'), 0, 0, 0
		},
		{  // Tender key 12
			0, 0,
			{192,168,0, 99},
				3200,
				TENDERKEY_TRANTYPE_CREDIT, TENDERKEY_TRANCODE_SALE,
			{_T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0')},
			_T('0'), 0, 0, 0
		},
		{  // Tender key 13
			0, 0,
			{192,168,0, 99},
				3200,
				TENDERKEY_TRANTYPE_CREDIT, TENDERKEY_TRANCODE_SALE,
			{_T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0')},
			_T('0'), 0, 0, 0
		},
		{  // Tender key 14
			0, 0,
			{192,168,0, 99},
				3200,
				TENDERKEY_TRANTYPE_CREDIT, TENDERKEY_TRANCODE_SALE,
			{_T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0')},
			_T('0'), 0, 0, 0
		},
		{  // Tender key 15
			0, 0,
			{192,168,0, 99},
				3200,
				TENDERKEY_TRANTYPE_CREDIT, TENDERKEY_TRANCODE_SALE,
			{_T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0')},
			_T('0'), 0, 0, 0
		},
		{  // Tender key 16
			0, 0,
			{192,168,0, 99},
				3200,
				TENDERKEY_TRANTYPE_CREDIT, TENDERKEY_TRANCODE_SALE,
			{_T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0')},
			_T('0'), 0, 0, 0
		},
		{  // Tender key 17
			0, 0,
			{192,168,0, 99},
				3200,
				TENDERKEY_TRANTYPE_CREDIT, TENDERKEY_TRANCODE_SALE,
			{_T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0')},
			_T('0'), 0, 0, 0
		},
		{  // Tender key 18
			0, 0,
			{192,168,0, 99},
				3200,
				TENDERKEY_TRANTYPE_CREDIT, TENDERKEY_TRANCODE_SALE,
			{_T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0')},
			_T('0'), 0, 0, 0
		},
		{  // Tender key 19
			0, 0,
			{192,168,0, 99},
				3200,
				TENDERKEY_TRANTYPE_CREDIT, TENDERKEY_TRANCODE_SALE,
			{_T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0')},
			_T('0'), 0, 0, 0
		},
		{  // Tender key 20
			0, 0,
			{192,168,0, 99},
				3200,
				TENDERKEY_TRANTYPE_CREDIT, TENDERKEY_TRANCODE_SALE,
			{_T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0'), _T('0')},
			_T('0'), 0, 0, 0
		},
	},
	{  // PARASTOREFORWARD   ParaStoreForward
		0, 0,         // ulStoreForwardFlags, lStoreForwardMaxAmountTrans, lStoreForwardMaxAmountCummulative
		0, 0, 0, 0, 0
	},
	{  // PARASTOREFORWARD   ParaReasonCode[MAX_REASON_CODE_ENTRIES][MAX_REASON_CODE_ITEMS]
		{0, 0}
	},
    {  //  Para.ParaSuggestedTip[MAX_SUGGESTED_TIP]
        0
    }
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
	// removed as not used with PEP.  These are internal areas for NHPOS individual
	// terminal status information such as by notice board, etc.
    0,    // Inquiry Status
    {   // Transaction #
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
    },
    {   // Transaction # *
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
    },
    0,  // Broadcast information *
    {   // Inquiry Date *
		0,0,0,0, 0,0    * usYear, usMonth, usDay, usHour, usMin, usSec *
    },
    {   // Total update header *
		0,0,0,0, 0,0
    },
    {   // Tally counter for CPM *
		0,0,0,0, 0,0,0
    },
    {   // Tally counter for EPT *
		0,0,0,0
    }
#endif
};
