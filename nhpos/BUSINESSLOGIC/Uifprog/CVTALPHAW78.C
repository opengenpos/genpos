/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Conversion Table for ALPHA Mode (NCR 5932 Wedge 68 key K/B)
* Category    : NCR 2170 US Hospitality Application         
* Program Name: CVTREGC.C
* --------------------------------------------------------------------------
* Abstract:	This file contains the translation table used with the NCR 5932
		Wedge 78 key keyboard.  This translation table is used by the
		function UieConvertFsc () in file uiekey.c to determine if a
		key press is mapped to an FSC code or is to be treated as an
		alphanumeric key press instead.

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
		Program Mode Alphanumeric tables for the NCR 5932 Wedge 78 key keyboard

		This key transformation table is used to translate from a key code
		as provided by the keyboard translation within DeviceEngine.cpp
		to either an ASCII code or an actual FSC.

		The NCR 5932 Wedge 78 key keyboard contains 6 rows of 13 keys.  Each
		key is the same size.
 */

CVTTBL FARCONST CvtAlph5932_78_1 [] = /* Code Sent From Framework to BL */
//Row 1
    {{0x00, FSC_CLEAR   },             /* 40h or IDD_P03_KEY_00 */
     {0x00, FSC_NO_FUNC },             /* 41h or IDD_P03_KEY_01 */
     {0x00, FSC_NO_FUNC },             /* 42h or IDD_P03_KEY_02 */
     {0x00, FSC_NO_FUNC },             /* 43h or IDD_P03_KEY_03 */
     {0x00, FSC_NO_FUNC },             /* 44h or IDD_P03_KEY_04 */
     {0x00, FSC_NO_FUNC },             /* 45h or IDD_P03_KEY_05 */
     {0x00, FSC_NO_FUNC },             /* 46h or IDD_P03_KEY_06 */
     {0x00, FSC_NO_FUNC },             /* 47h or IDD_P03_KEY_07 */
     {0x00, FSC_NO_FUNC },             /* 48h or IDD_P03_KEY_08 */
     {0x00, FSC_NO_FUNC },             /* 49h or IDD_P03_KEY_09 */
     {0x00, FSC_SCROLL_UP },           /* 4Ah or IDD_P03_KEY_0A */
     {0x00, FSC_P3      },             /* 4Bh or IDD_P03_KEY_0B */
     {0x00, FSC_P4      },             /* 4Ch or IDD_P03_KEY_0C */

//Row 2
     {0x31, FSC_NO_FUNC },             /* 4Dh or IDD_P03_KEY_10 */
     {0x32, FSC_NO_FUNC },             /* 4Eh or IDD_P03_KEY_11 */
     {0x33, FSC_NO_FUNC },             /* 4Fh or IDD_P03_KEY_12 */
     {0x34, FSC_NO_FUNC },             /* 50h or IDD_P03_KEY_13 */
     {0x35, FSC_NO_FUNC },             /* 51h or IDD_P03_KEY_14 */
     {0x36, FSC_NO_FUNC },             /* 52h or IDD_P03_KEY_15 */
     {0x37, FSC_NO_FUNC },             /* 53h or IDD_P03_KEY_16 */
     {0x38, FSC_NO_FUNC },             /* 54h or IDD_P03_KEY_17 */
     {0x39, FSC_NO_FUNC },             /* 55h or IDD_P03_KEY_18 */
     {0x30, FSC_NO_FUNC },             /* 56h or IDD_P03_KEY_19 */
     {0x00, FSC_SCROLL_DOWN },         /* 57h or IDD_P03_KEY_1A */
     {0x00, FSC_P1      },             /* 58h or IDD_P03_KEY_1B */
     {0x00, FSC_P2      },             /* 59h or IDD_P03_KEY_1C */

//Row 3
     {0x00, FSC_RECEIPT_FEED },        /* 5Ah or IDD_P03_KEY_20 */
     {0x00, FSC_JOURNAL_FEED },        /* 5Bh or IDD_P03_KEY_21 */
     {0x00, FSC_NO_FUNC },             /* 5Ch or IDD_P03_KEY_22 */
     {0x00, FSC_NO_FUNC },             /* 5Dh or IDD_P03_KEY_23 */
     {0x00, FSC_NO_FUNC },             /* 5Eh or IDD_P03_KEY_24 */
     {0x00, FSC_NO_FUNC },             /* 5Fh or IDD_P03_KEY_25 */
     {0x00, FSC_NO_FUNC },             /* 60h or IDD_P03_KEY_26 */
     {0x00, FSC_NO_FUNC },             /* 61h or IDD_P03_KEY_27 */
     {0x00, FSC_NO_FUNC },             /* 62h or IDD_P03_KEY_28 */
     {0x00, FSC_NO_FUNC },             /* 63h or IDD_P03_KEY_29 */
     {0x00, FSC_NO_FUNC },             /* 64h or IDD_P03_KEY_2A */
     {0x00, FSC_P5      },             /* 65h or IDD_P03_KEY_2B */
     {0x00, FSC_NO_FUNC },             /* 66h or IDD_P03_KEY_2C */

//Row 4
     {0x00, FSC_NO_FUNC },             /* 67h or IDD_P03_KEY_30 */
     {0x51, FSC_NO_FUNC },             /* 68h or IDD_P03_KEY_31 */
     {0x57, FSC_NO_FUNC },             /* 69h or IDD_P03_KEY_32 */
     {0x45, FSC_NO_FUNC },             /* 6Ah or IDD_P03_KEY_33 */
     {0x52, FSC_NO_FUNC },             /* 6Bh or IDD_P03_KEY_34 */
     {0x54, FSC_NO_FUNC },             /* 6Ch or IDD_P03_KEY_35 */
     {0x59, FSC_NO_FUNC },             /* 6Dh or IDD_P03_KEY_36 */
     {0x55, FSC_NO_FUNC },             /* 6Eh or IDD_P03_KEY_37 */
     {0x49, FSC_NO_FUNC },             /* 6Fh or IDD_P03_KEY_38 */
     {0x4F, FSC_NO_FUNC },             /* 70h or IDD_P03_KEY_39 */
     {0x50, FSC_NO_FUNC },             /* 71h or IDD_P03_KEY_3A */
     {0x5B, FSC_NO_FUNC },             /* 72h or IDD_P03_KEY_3B */
     {0x5D, FSC_NO_FUNC },             /* 73h or IDD_P03_KEY_3C */

//Row 5
     {0x00, FSC_NO_FUNC },             /* 74h or IDD_P03_KEY_40 */
     {0x41, FSC_NO_FUNC },             /* 75h or IDD_P03_KEY_41 */
     {0x53, FSC_NO_FUNC },             /* 76h or IDD_P03_KEY_42 */
     {0x44, FSC_NO_FUNC },             /* 77h or IDD_P03_KEY_43 */
     {0x46, FSC_NO_FUNC },             /* 78h or IDD_P03_KEY_44 */
     {0x47, FSC_NO_FUNC },             /* 79h or IDD_P03_KEY_45 */
     {0x48, FSC_NO_FUNC },             /* 7Ah or IDD_P03_KEY_46 */
     {0x4A, FSC_NO_FUNC },             /* 7Bh or IDD_P03_KEY_47 */
     {0x4B, FSC_NO_FUNC },             /* 7Ch or IDD_P03_KEY_48 */
     {0x4C, FSC_NO_FUNC },             /* 7Dh or IDD_P03_KEY_49 */
     {0x3B, FSC_NO_FUNC },             /* 7Eh or IDD_P03_KEY_4A */
     {0x27, FSC_NO_FUNC },             /* 7Fh or IDD_P03_KEY_4B */
     {0x0D, FSC_NO_FUNC },             /* 80h or IDD_P03_KEY_4C */

//Row 6
     {0x00, FSC_NO_FUNC },             /* 81h or IDD_P03_KEY_50 */
     {0x5A, FSC_NO_FUNC },             /* 82h or IDD_P03_KEY_51 */
     {0x58, FSC_NO_FUNC },             /* 83h or IDD_P03_KEY_52 */
     {0x43, FSC_NO_FUNC },             /* 84h or IDD_P03_KEY_53 */
     {0x56, FSC_NO_FUNC },             /* 85h or IDD_P03_KEY_54 */
     {0x42, FSC_NO_FUNC },             /* 86h or IDD_P03_KEY_55 */
     {0x4E, FSC_NO_FUNC },             /* 87h or IDD_P03_KEY_56 */
     {0x4D, FSC_NO_FUNC },             /* 88h or IDD_P03_KEY_57 */
     {0x2C, FSC_NO_FUNC },             /* 89h or IDD_P03_KEY_58 */
     {0x2E, FSC_NO_FUNC },             /* 8Ah or IDD_P03_KEY_59 */
     {0x2F, FSC_NO_FUNC },             /* 8Bh or IDD_P03_KEY_5A */
     {0x00, FSC_NO_FUNC },             /* 8Ch or IDD_P03_KEY_5B */
     {0x00, FSC_NO_FUNC }              /* 8Dh or IDD_P03_KEY_5C */
};


CVTTBL FARCONST CvtAlph5932_78_2 [] = /* Pif */
//Row 1
    {{0x00, FSC_CLEAR   },             /* 40h or IDD_P03_KEY_00 */
     {0x00, FSC_NO_FUNC },             /* 41h or IDD_P03_KEY_01 */
     {0x00, FSC_NO_FUNC },             /* 42h or IDD_P03_KEY_02 */
     {0x00, FSC_NO_FUNC },             /* 43h or IDD_P03_KEY_03 */
     {0x00, FSC_NO_FUNC },             /* 44h or IDD_P03_KEY_04 */
     {0x00, FSC_NO_FUNC },             /* 45h or IDD_P03_KEY_05 */
     {0x00, FSC_NO_FUNC },             /* 46h or IDD_P03_KEY_06 */
     {0x00, FSC_NO_FUNC },             /* 47h or IDD_P03_KEY_07 */
     {0x00, FSC_NO_FUNC },             /* 48h or IDD_P03_KEY_08 */
     {0x00, FSC_NO_FUNC },             /* 49h or IDD_P03_KEY_09 */
     {0x00, FSC_SCROLL_UP },           /* 4Ah or IDD_P03_KEY_0A */
     {0x00, FSC_P3      },             /* 4Bh or IDD_P03_KEY_0B */
     {0x00, FSC_P4      },             /* 4Ch or IDD_P03_KEY_0C */

//Row 2
     {0x31, FSC_NO_FUNC },             /* 4Dh or IDD_P03_KEY_10 */
     {0x32, FSC_NO_FUNC },             /* 4Eh or IDD_P03_KEY_11 */
     {0x33, FSC_NO_FUNC },             /* 4Fh or IDD_P03_KEY_12 */
     {0x34, FSC_NO_FUNC },             /* 50h or IDD_P03_KEY_13 */
     {0x35, FSC_NO_FUNC },             /* 51h or IDD_P03_KEY_14 */
     {0x36, FSC_NO_FUNC },             /* 52h or IDD_P03_KEY_15 */
     {0x37, FSC_NO_FUNC },             /* 53h or IDD_P03_KEY_16 */
     {0x38, FSC_NO_FUNC },             /* 54h or IDD_P03_KEY_17 */
     {0x39, FSC_NO_FUNC },             /* 55h or IDD_P03_KEY_18 */
     {0x30, FSC_NO_FUNC },             /* 56h or IDD_P03_KEY_19 */
     {0x00, FSC_SCROLL_DOWN },         /* 57h or IDD_P03_KEY_1A */
     {0x00, FSC_P1      },             /* 58h or IDD_P03_KEY_1B */
     {0x00, FSC_P2      },             /* 59h or IDD_P03_KEY_1C */

//Row 3
     {0x00, FSC_RECEIPT_FEED },        /* 5Ah or IDD_P03_KEY_20 */
     {0x00, FSC_JOURNAL_FEED },        /* 5Bh or IDD_P03_KEY_21 */
     {0x00, FSC_NO_FUNC },             /* 5Ch or IDD_P03_KEY_22 */
     {0x00, FSC_NO_FUNC },             /* 5Dh or IDD_P03_KEY_23 */
     {0x00, FSC_NO_FUNC },             /* 5Eh or IDD_P03_KEY_24 */
     {0x00, FSC_NO_FUNC },             /* 5Fh or IDD_P03_KEY_25 */
     {0x00, FSC_NO_FUNC },             /* 60h or IDD_P03_KEY_26 */
     {0x00, FSC_NO_FUNC },             /* 61h or IDD_P03_KEY_27 */
     {0x00, FSC_NO_FUNC },             /* 62h or IDD_P03_KEY_28 */
     {0x00, FSC_NO_FUNC },             /* 63h or IDD_P03_KEY_29 */
     {0x00, FSC_NO_FUNC },             /* 64h or IDD_P03_KEY_2A */
     {0x00, FSC_P5      },             /* 65h or IDD_P03_KEY_2B */
     {0x00, FSC_NO_FUNC },             /* 66h or IDD_P03_KEY_2C */

//Row 4
     {0x00, FSC_NO_FUNC },             /* 67h or IDD_P03_KEY_30 */
     {0x71, FSC_NO_FUNC },             /* 68h or IDD_P03_KEY_31 */
     {0x77, FSC_NO_FUNC },             /* 69h or IDD_P03_KEY_32 */
     {0x65, FSC_NO_FUNC },             /* 6Ah or IDD_P03_KEY_33 */
     {0x72, FSC_NO_FUNC },             /* 6Bh or IDD_P03_KEY_34 */
     {0x74, FSC_NO_FUNC },             /* 6Ch or IDD_P03_KEY_35 */
     {0x79, FSC_NO_FUNC },             /* 6Dh or IDD_P03_KEY_36 */
     {0x75, FSC_NO_FUNC },             /* 6Eh or IDD_P03_KEY_37 */
     {0x69, FSC_NO_FUNC },             /* 6Fh or IDD_P03_KEY_38 */
     {0x6F, FSC_NO_FUNC },             /* 70h or IDD_P03_KEY_39 */
     {0x70, FSC_NO_FUNC },             /* 71h or IDD_P03_KEY_3A */
     {0x7B, FSC_NO_FUNC },             /* 72h or IDD_P03_KEY_3B */
     {0x7D, FSC_NO_FUNC },             /* 73h or IDD_P03_KEY_3C */

//Row 5
     {0x00, FSC_NO_FUNC },             /* 74h or IDD_P03_KEY_40 */
     {0x61, FSC_NO_FUNC },             /* 75h or IDD_P03_KEY_41 */
     {0x73, FSC_NO_FUNC },             /* 76h or IDD_P03_KEY_42 */
     {0x64, FSC_NO_FUNC },             /* 77h or IDD_P03_KEY_43 */
     {0x66, FSC_NO_FUNC },             /* 78h or IDD_P03_KEY_44 */
     {0x67, FSC_NO_FUNC },             /* 79h or IDD_P03_KEY_45 */
     {0x68, FSC_NO_FUNC },             /* 7Ah or IDD_P03_KEY_46 */
     {0x6A, FSC_NO_FUNC },             /* 7Bh or IDD_P03_KEY_47 */
     {0x6B, FSC_NO_FUNC },             /* 7Ch or IDD_P03_KEY_48 */
     {0x6C, FSC_NO_FUNC },             /* 7Dh or IDD_P03_KEY_49 */
     {0x3A, FSC_NO_FUNC },             /* 7Eh or IDD_P03_KEY_4A */
     {0x22, FSC_NO_FUNC },             /* 7Fh or IDD_P03_KEY_4B */
     {0x0D, FSC_NO_FUNC },             /* 80h or IDD_P03_KEY_4C */

//Row 6
     {0x00, FSC_NO_FUNC },             /* 81h or IDD_P03_KEY_50 */
     {0x7A, FSC_NO_FUNC },             /* 82h or IDD_P03_KEY_51 */
     {0x78, FSC_NO_FUNC },             /* 83h or IDD_P03_KEY_52 */
     {0x63, FSC_NO_FUNC },             /* 84h or IDD_P03_KEY_53 */
     {0x76, FSC_NO_FUNC },             /* 85h or IDD_P03_KEY_54 */
     {0x62, FSC_NO_FUNC },             /* 86h or IDD_P03_KEY_55 */
     {0x6E, FSC_NO_FUNC },             /* 87h or IDD_P03_KEY_56 */
     {0x6D, FSC_NO_FUNC },             /* 88h or IDD_P03_KEY_57 */
     {0x3C, FSC_NO_FUNC },             /* 89h or IDD_P03_KEY_58 */
     {0x3E, FSC_NO_FUNC },             /* 8Ah or IDD_P03_KEY_59 */
     {0x3F, FSC_NO_FUNC },             /* 8Bh or IDD_P03_KEY_5A */
     {0x00, FSC_NO_FUNC },             /* 8Ch or IDD_P03_KEY_5B */
     {0x00, FSC_NO_FUNC }              /* 8Dh or IDD_P03_KEY_5C */
};

