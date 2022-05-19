/*
*===========================================================================
* Title       : Conversion Table for Register/Supervisor Mode (NCR 5932 Wedge 78 key K/B)
* Category    : NCR 2170 US Hospitality Application         
* Program Name: CVTREGW78.C
* --------------------------------------------------------------------------
* Abstract:	This file contains the translation table used with the NCR 5932
		Wedge 78 key keyboard.  This translation table is used by the
		function UieConvertFsc () in file uiekey.c to determine if a
		key press is mapped to an FSC code or is to be treated as an
		alphanumeric key press instead.

		The 5932-7000 Wedge 78 key keyboard has 6 rows of keys, each row
		containing 13 keys.  It also has a keylock and an MSR reader across
		the top left of the keyboard parallel to the top of the keyboard.

		The menu page data structures are designed for a keyboard whose
		largest size is 11 keys across and 13 rows, the NCR 7448 POS terminal
		with the Micromotion keyboard.

* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :Ver.Rev.:   Name    : Description
* Jun-17-92:00.00.01:M.SUZUKI   : Initiale
*          :        :           :                                    
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


// Framework translates key presses into messages sent to BusinessLogic
// The Keypress Code that is provided in the message is translated into an
// FSC Code using the FSC Table below.  The Keypress Code sent by Framework
// to Business Logic is commented to the right of each entry in the conversion
// table allowing you to match Keypress Code to the FSC in the FSC Table.
// There are two types of entries in this table.
// Changable FSC codes are provisioned using the PEP utility (P03 dialog), 
// the terminal Program 3 in Program Mode, or a 3rd Party application such
// as Manager's Work Station (MWS).
// Other Keypress Codes are Hard Coded FSC's like FSC_CLEAR which are set to
// a particular location on the keyboard
// For a complete list of FSC codes see "FSC.h"

// The CVTTBL data structure is made up of two pieces, uchASCII and uchScan.
// If uchASCII has a value then the key is considered to be an Alpha Numeric Key
// and the code provided in uchASCII is used.
// If uchASCII is 0 then the code provided in uchScan/"FSC Code" is used.

// See also the logic in DeviceEngine.cpp which contains the conversion tables
// used to convert from a keyboard scan code message to a Keypress Code which is
// then used to lookup the FSC code in the table below.

// For the NCR 78 key wedge keyboard, we are swapping the keyboard rows and
// columns.  In other words, for most keyboards, the keys are numbered in row
// order.  For instance for the NCR 68 Wedge keyboard the first 8 items in the
// conversion table for the 68 key keyboard, CvtReg5932_68 [], are in the first
// 8 locations of that array.  For the NCR 78 key keyboard, the keys are numbered
// in column order so the first, left most column of keys are in the first 6
// locations of the array CvtReg5932_78 [] below.

// This must agree with the way that PEP in the P03 dialog puts FSC codes into
// the FSC table.

// The values in this table are used by function UieConvertFsc () in file uiekey.c
// in the UIE subsystem of BusinessLogic to determine the index into the actual
// FSC table provided by PEP or MWS.

// Basically the function ConvertKey () in DeviceEngine.cpp converts a Windows key
// press event into a businesslogic which function KeyboardDataMessage () then uses
// to put a businesslogic keypress event into function BlNotifyData () providing the
// key press event into the UIE subsystem.  Function UieConvertFsc () then converts
// the businesslogic keypress event into either an FSC or an alphanumeric value.

// The businesslogic key press is then used to index into the CVTTBL to obtain the
// offset within the FSC table to access the actual FSC information assigned to the key.

// WARNING: Make sure that the PEP P03 dialog button IDs which are shown in the P03
//          keyboard dialog are in the same order as the offsets in the CVTTBL table
//          below.  Also make sure that the key conversion table in DeviceEngine.cpp
//          provides offsets into this table correctly.

// Typically, you will want the CONVERT_KEY table, which converts the key press into
// a CVTTBL offset to be in row order and this table is also in row order.  Then in
// the body of this table for the FSC offset, you would specify an offset which
// corresponds to the P03 dialog control ID which is used in PEP to figure the offset
// into the FSC table.
CVTTBL FARCONST CvtReg5932_78 [] =          /* Code Sent From Framework to BL */
//Row 1
    {{0x00, UIE_FSC_BIAS +   0},             /* 40h or IDD_P03_KEY_00 */
     {0x00, UIE_FSC_BIAS +  11},             /* 41h or IDD_P03_KEY_01 */
     {0x00, UIE_FSC_BIAS +  22},             /* 42h or IDD_P03_KEY_02 */
     {0x00, UIE_FSC_BIAS +  33},             /* 43h or IDD_P03_KEY_03 */
     {0x00, UIE_FSC_BIAS +  44},             /* 44h or IDD_P03_KEY_04 */
     {0x00, UIE_FSC_BIAS +  55},             /* 45h or IDD_P03_KEY_05 */
     {0x00, UIE_FSC_BIAS +  66},             /* 46h or IDD_P03_KEY_06 */
     {0x00, UIE_FSC_BIAS +  77},             /* 47h or IDD_P03_KEY_07 */
     {0x00, UIE_FSC_BIAS +  88},             /* 48h or IDD_P03_KEY_08 */
     {0x00, UIE_FSC_BIAS +  99},             /* 49h or IDD_P03_KEY_09 */
     {0x00, UIE_FSC_BIAS + 110},             /* 4Ah or IDD_P03_KEY_0A */
     {0x00, UIE_FSC_BIAS + 121},             /* 4Bh or IDD_P03_KEY_0B */
     {0x00, UIE_FSC_BIAS + 132},             /* 4Ch or IDD_P03_KEY_0C */

//Row 2
     {0x00, UIE_FSC_BIAS +   1},             /* 4Dh or IDD_P03_KEY_10 */
     {0x00, UIE_FSC_BIAS +  12},             /* 4Eh or IDD_P03_KEY_11 */
     {0x00, UIE_FSC_BIAS +  23},             /* 4Fh or IDD_P03_KEY_12 */
     {0x00, UIE_FSC_BIAS +  34},             /* 50h or IDD_P03_KEY_13 */
     {0x00, UIE_FSC_BIAS +  45},             /* 51h or IDD_P03_KEY_14 */
     {0x00, UIE_FSC_BIAS +  56},             /* 52h or IDD_P03_KEY_15 */
     {0x00, UIE_FSC_BIAS +  67},             /* 53h or IDD_P03_KEY_16 */
     {0x00, UIE_FSC_BIAS +  78},             /* 54h or IDD_P03_KEY_17 */
     {0x00, UIE_FSC_BIAS +  89},             /* 55h or IDD_P03_KEY_18 */
     {0x00, UIE_FSC_BIAS + 100},             /* 56h or IDD_P03_KEY_19 */
     {0x00, UIE_FSC_BIAS + 111},             /* 57h or IDD_P03_KEY_1A */
     {0x00, UIE_FSC_BIAS + 122},             /* 58h or IDD_P03_KEY_1B */
     {0x00, UIE_FSC_BIAS + 133},             /* 59h or IDD_P03_KEY_1C */

//Row 3
     {0x00, UIE_FSC_BIAS +   2},             /* 5Ah or IDD_P03_KEY_20 */
     {0x00, UIE_FSC_BIAS +  13},             /* 5Bh or IDD_P03_KEY_21 */
     {0x00, UIE_FSC_BIAS +  24},             /* 5Ch or IDD_P03_KEY_22 */
     {0x00, UIE_FSC_BIAS +  35},             /* 5Dh or IDD_P03_KEY_23 */
     {0x00, UIE_FSC_BIAS +  46},             /* 5Eh or IDD_P03_KEY_24 */
     {0x00, UIE_FSC_BIAS +  57},             /* 5Fh or IDD_P03_KEY_25 */
     {0x00, UIE_FSC_BIAS +  68},             /* 60h or IDD_P03_KEY_26 */
     {0x00, UIE_FSC_BIAS +  79},             /* 61h or IDD_P03_KEY_27 */
     {0x00, UIE_FSC_BIAS +  90},             /* 62h or IDD_P03_KEY_28 */
     {0x00, UIE_FSC_BIAS + 101},             /* 63h or IDD_P03_KEY_29 */
     {0x00, UIE_FSC_BIAS + 112},             /* 64h or IDD_P03_KEY_2A */
     {0x00, UIE_FSC_BIAS + 123},             /* 65h or IDD_P03_KEY_2B */
     {0x00, UIE_FSC_BIAS + 134},             /* 66h or IDD_P03_KEY_2C */

//Row 4
     {0x00, UIE_FSC_BIAS +   3},             /* 67h or IDD_P03_KEY_30 */
     {0x00, UIE_FSC_BIAS +  14},             /* 68h or IDD_P03_KEY_31 */
     {0x00, UIE_FSC_BIAS +  25},             /* 69h or IDD_P03_KEY_32 */
     {0x00, UIE_FSC_BIAS +  36},             /* 6Ah or IDD_P03_KEY_33 */
     {0x00, UIE_FSC_BIAS +  47},             /* 6Bh or IDD_P03_KEY_34 */
     {0x00, UIE_FSC_BIAS +  58},             /* 6Ch or IDD_P03_KEY_35 */
     {0x00, UIE_FSC_BIAS +  69},             /* 6Dh or IDD_P03_KEY_36 */
     {0x00, UIE_FSC_BIAS +  80},             /* 6Eh or IDD_P03_KEY_37 */
     {0x00, UIE_FSC_BIAS +  91},             /* 6Fh or IDD_P03_KEY_38 */
     {0x00, UIE_FSC_BIAS + 102},             /* 70h or IDD_P03_KEY_39 */
     {0x00, UIE_FSC_BIAS + 113},             /* 71h or IDD_P03_KEY_3A */
     {0x00, UIE_FSC_BIAS + 124},             /* 72h or IDD_P03_KEY_3B */
     {0x00, UIE_FSC_BIAS + 135},             /* 73h or IDD_P03_KEY_3C */

//Row 5
     {0x00, UIE_FSC_BIAS +   4},             /* 74h or IDD_P03_KEY_40 */
     {0x00, UIE_FSC_BIAS +  15},             /* 75h or IDD_P03_KEY_41 */
     {0x00, UIE_FSC_BIAS +  26},             /* 76h or IDD_P03_KEY_42 */
     {0x00, UIE_FSC_BIAS +  37},             /* 77h or IDD_P03_KEY_43 */
     {0x00, UIE_FSC_BIAS +  48},             /* 78h or IDD_P03_KEY_44 */
     {0x00, UIE_FSC_BIAS +  59},             /* 79h or IDD_P03_KEY_45 */
     {0x00, UIE_FSC_BIAS +  70},             /* 7Ah or IDD_P03_KEY_46 */
     {0x00, UIE_FSC_BIAS +  81},             /* 7Bh or IDD_P03_KEY_47 */
     {0x00, UIE_FSC_BIAS +  92},             /* 7Ch or IDD_P03_KEY_48 */
     {0x00, UIE_FSC_BIAS + 103},             /* 7Dh or IDD_P03_KEY_49 */
     {0x00, UIE_FSC_BIAS + 114},             /* 7Eh or IDD_P03_KEY_4A */
     {0x00, UIE_FSC_BIAS + 125},             /* 7Fh or IDD_P03_KEY_4B */
     {0x00, UIE_FSC_BIAS + 136},             /* 80h or IDD_P03_KEY_4C */

//Row 6
     {0x00, UIE_FSC_BIAS +   5},             /* 81h or IDD_P03_KEY_50 */
     {0x00, UIE_FSC_BIAS +  16},             /* 82h or IDD_P03_KEY_51 */
     {0x00, UIE_FSC_BIAS +  27},             /* 83h or IDD_P03_KEY_52 */
     {0x00, UIE_FSC_BIAS +  38},             /* 84h or IDD_P03_KEY_53 */
     {0x00, UIE_FSC_BIAS +  49},             /* 85h or IDD_P03_KEY_54 */
     {0x00, UIE_FSC_BIAS +  60},             /* 86h or IDD_P03_KEY_55 */
     {0x00, UIE_FSC_BIAS +  71},             /* 87h or IDD_P03_KEY_56 */
     {0x00, UIE_FSC_BIAS +  82},             /* 88h or IDD_P03_KEY_57 */
     {0x00, UIE_FSC_BIAS +  93},             /* 89h or IDD_P03_KEY_58 */
     {0x00, UIE_FSC_BIAS + 104},             /* 8Ah or IDD_P03_KEY_59 */
     {0x00, UIE_FSC_BIAS + 115},             /* 8Bh or IDD_P03_KEY_5A */
     {0x00, UIE_FSC_BIAS + 126},             /* 8Ch or IDD_P03_KEY_5B */
     {0x00, UIE_FSC_BIAS + 137}              /* 8Dh or IDD_P03_KEY_5C */

};

