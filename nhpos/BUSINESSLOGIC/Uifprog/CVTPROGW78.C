/*===========================================================================
* Title       : Conversion Table for Program Mode (NCR 5932 Wedge 68 key K/B)
* Category    : NCR 2170 US Hospitality Application         
* Program Name: CVTPROGW68C.C
* --------------------------------------------------------------------------
* Abstract:	This file contains the translation table used with the NCR 5932
		Wedge 78 key keyboard.  This translation table is used by the
		function UieConvertFsc () in file uiekey.c to determine if a
		key press is mapped to an FSC code or is to be treated as an
		alphanumeric key press instead.

* --------------------------------------------------------------------------
*===========================================================================
*/


/*                                                                                
*===========================================================================
*   Include File Declarations
*===========================================================================
*/
#include <tchar.h>
#include <ecr.h>
#include <uie.h>
#include <fsc.h>
#include <cvt.h>

/*
		Program Mode table for the NCR 5932 Wedge 78 key keyboard

		This key transformation table is used to translate from a key code
		as provided by the keyboard translation within DeviceEngine.cpp
		to either an ASCII code or an offset into the FSC table for the
		current menu page.

		The NCR 5932 Wedge 78 key keyboard contains 6 rows of 13 keys.  Each
		key is the same size.
 */

CVTTBL FARCONST CvtProg5932_78 [] =   /* Code Sent From Framework to BL */
//Row 1
    {{0x00, FSC_CLEAR   /* UIE_FSC_BIAS +   0*/},             /* 40h or IDD_P03_KEY_00 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +  11*/},             /* 41h or IDD_P03_KEY_01 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +  22*/},             /* 42h or IDD_P03_KEY_02 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +  33*/},             /* 43h or IDD_P03_KEY_03 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +  44*/},             /* 44h or IDD_P03_KEY_04 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +  55*/},             /* 45h or IDD_P03_KEY_05 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +  66*/},             /* 46h or IDD_P03_KEY_06 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +  77*/},             /* 47h or IDD_P03_KEY_07 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +  88*/},             /* 48h or IDD_P03_KEY_08 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +  99*/},             /* 49h or IDD_P03_KEY_09 */
     {0x00, FSC_SCROLL_UP /* UIE_FSC_BIAS + 110*/},             /* 4Ah or IDD_P03_KEY_0A */
     {0x00, FSC_P3      /* UIE_FSC_BIAS + 121*/},             /* 4Bh or IDD_P03_KEY_0B */
     {0x00, FSC_P4      /* UIE_FSC_BIAS + 132*/},             /* 4Ch or IDD_P03_KEY_0C */

//Row 2
     {0x31, FSC_NO_FUNC /* UIE_FSC_BIAS +   1*/},             /* 4Dh or IDD_P03_KEY_10 */
     {0x32, FSC_NO_FUNC /* UIE_FSC_BIAS +  12*/},             /* 4Eh or IDD_P03_KEY_11 */
     {0x33, FSC_NO_FUNC /* UIE_FSC_BIAS +  23*/},             /* 4Fh or IDD_P03_KEY_12 */
     {0x34, FSC_NO_FUNC /* UIE_FSC_BIAS +  34*/},             /* 50h or IDD_P03_KEY_13 */
     {0x35, FSC_NO_FUNC /* UIE_FSC_BIAS +  45*/},             /* 51h or IDD_P03_KEY_14 */
     {0x36, FSC_NO_FUNC /* UIE_FSC_BIAS +  56*/},             /* 52h or IDD_P03_KEY_15 */
     {0x37, FSC_NO_FUNC /* UIE_FSC_BIAS +  67*/},             /* 53h or IDD_P03_KEY_16 */
     {0x38, FSC_NO_FUNC /* UIE_FSC_BIAS +  78*/},             /* 54h or IDD_P03_KEY_17 */
     {0x39, FSC_NO_FUNC /* UIE_FSC_BIAS +  89*/},             /* 55h or IDD_P03_KEY_18 */
     {0x30, FSC_NO_FUNC /* UIE_FSC_BIAS + 100*/},             /* 56h or IDD_P03_KEY_19 */
     {0x00, FSC_SCROLL_DOWN /* UIE_FSC_BIAS + 111*/},             /* 57h or IDD_P03_KEY_1A */
     {0x00, FSC_P1      /* UIE_FSC_BIAS + 122*/},             /* 58h or IDD_P03_KEY_1B */
     {0x00, FSC_P2      /* UIE_FSC_BIAS + 133*/},             /* 59h or IDD_P03_KEY_1C */

//Row 3
     {0x00, FSC_RECEIPT_FEED /* UIE_FSC_BIAS +   2*/},             /* 5Ah or IDD_P03_KEY_20 */
     {0x00, FSC_JOURNAL_FEED /* UIE_FSC_BIAS +  13*/},             /* 5Bh or IDD_P03_KEY_21 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +  24*/},             /* 5Ch or IDD_P03_KEY_22 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +  35*/},             /* 5Dh or IDD_P03_KEY_23 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +  46*/},             /* 5Eh or IDD_P03_KEY_24 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +  57*/},             /* 5Fh or IDD_P03_KEY_25 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +  68*/},             /* 60h or IDD_P03_KEY_26 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +  79*/},             /* 61h or IDD_P03_KEY_27 */
     {0x37, FSC_NO_FUNC /* UIE_FSC_BIAS +  90*/},             /* 62h or IDD_P03_KEY_28 */
     {0x38, FSC_NO_FUNC /* UIE_FSC_BIAS + 101*/},             /* 63h or IDD_P03_KEY_29 */
     {0x39, FSC_NO_FUNC /* UIE_FSC_BIAS + 112*/},             /* 64h or IDD_P03_KEY_2A */
     {0x00, FSC_P5      /* UIE_FSC_BIAS + 123*/},             /* 65h or IDD_P03_KEY_2B */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS + 134*/},             /* 66h or IDD_P03_KEY_2C */

//Row 4
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +   3*/},             /* 67h or IDD_P03_KEY_30 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +  14*/},             /* 68h or IDD_P03_KEY_31 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +  25*/},             /* 69h or IDD_P03_KEY_32 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +  36*/},             /* 6Ah or IDD_P03_KEY_33 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +  47*/},             /* 6Bh or IDD_P03_KEY_34 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +  58*/},             /* 6Ch or IDD_P03_KEY_35 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +  69*/},             /* 6Dh or IDD_P03_KEY_36 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +  80*/},             /* 6Eh or IDD_P03_KEY_37 */
     {0x34, FSC_NO_FUNC /* UIE_FSC_BIAS +  91*/},             /* 6Fh or IDD_P03_KEY_38 */
     {0x35, FSC_NO_FUNC /* UIE_FSC_BIAS + 102*/},             /* 70h or IDD_P03_KEY_39 */
     {0x36, FSC_NO_FUNC /* UIE_FSC_BIAS + 113*/},             /* 71h or IDD_P03_KEY_3A */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS + 124*/},             /* 72h or IDD_P03_KEY_3B */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS + 135*/},             /* 73h or IDD_P03_KEY_3C */

//Row 5
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +   4*/},             /* 74h or IDD_P03_KEY_40 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +  15*/},             /* 75h or IDD_P03_KEY_41 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +  26*/},             /* 76h or IDD_P03_KEY_42 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +  37*/},             /* 77h or IDD_P03_KEY_43 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +  48*/},             /* 78h or IDD_P03_KEY_44 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +  59*/},             /* 79h or IDD_P03_KEY_45 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +  70*/},             /* 7Ah or IDD_P03_KEY_46 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +  81*/},             /* 7Bh or IDD_P03_KEY_47 */
     {0x31, FSC_NO_FUNC /* UIE_FSC_BIAS +  92*/},             /* 7Ch or IDD_P03_KEY_48 */
     {0x32, FSC_NO_FUNC /* UIE_FSC_BIAS + 103*/},             /* 7Dh or IDD_P03_KEY_49 */
     {0x33, FSC_NO_FUNC /* UIE_FSC_BIAS + 114*/},             /* 7Eh or IDD_P03_KEY_4A */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS + 125*/},             /* 7Fh or IDD_P03_KEY_4B */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS + 136*/},             /* 80h or IDD_P03_KEY_4C */

//Row 6
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +   5*/},             /* 81h or IDD_P03_KEY_50 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +  16*/},             /* 82h or IDD_P03_KEY_51 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +  27*/},             /* 83h or IDD_P03_KEY_52 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +  38*/},             /* 84h or IDD_P03_KEY_53 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +  49*/},             /* 85h or IDD_P03_KEY_54 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +  60*/},             /* 86h or IDD_P03_KEY_55 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +  71*/},             /* 87h or IDD_P03_KEY_56 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS +  82*/},             /* 88h or IDD_P03_KEY_57 */
     {0x30, FSC_NO_FUNC /* UIE_FSC_BIAS +  93*/},             /* 89h or IDD_P03_KEY_58 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS + 104*/},             /* 8Ah or IDD_P03_KEY_59 */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS + 115*/},             /* 8Bh or IDD_P03_KEY_5A */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS + 126*/},             /* 8Ch or IDD_P03_KEY_5B */
     {0x00, FSC_NO_FUNC /* UIE_FSC_BIAS + 137*/}              /* 8Dh or IDD_P03_KEY_5C */
};

