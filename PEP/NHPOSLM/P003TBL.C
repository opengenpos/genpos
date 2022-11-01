/*
* ---------------------------------------------------------------------------
* Title         :   Function Selection Code Table File (Prog. 3)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P003TBL.C
* Copyright (C) :   1995, NCR Corp. E&M OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /W4 /G2 /GEf /GA /Zp /f- /Os /Og /Oe /Gs
* ---------------------------------------------------------------------------
* Abstract :
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
* Apr-19-95 : 03.00.00 : H.Ishida   : Add key no. 57-69
* Aug-08-95 : 03.00.00 : hkato      : R3.0 FVT Comment.
* Jan-22-96 : 03.00.05 : M.Suzuki   : Add R3.1
* Sep-06-98 : 03.03.00 : A.Mitsui   : Add V3.3
* Nov-22-99 :          : K.Yanagida : NCR2172 
* ===========================================================================
* ===========================================================================
* PVCS Entry
* ---------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
* ===========================================================================
*/
/*
* ===========================================================================
*       Include File Declarations
* ===========================================================================
*/
#include "p003.h"

/*
* ===========================================================================
*       Compiler Message Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Work Area Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Public Work Area Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Static Work Area Declarations
* ===========================================================================
*/
static  WORD    awConvFixedID[P03_CONV_FIXED_NO] = {
                    IDD_P03_KEY_32,
                    IDD_P03_KEY_34, IDD_P03_KEY_44, IDD_P03_KEY_54,
                    IDD_P03_KEY_35, IDD_P03_KEY_45, IDD_P03_KEY_55,
                    IDD_P03_KEY_36, IDD_P03_KEY_46, IDD_P03_KEY_56,
                    IDD_P03_KEY_37, IDD_P03_KEY_47
                };

static  WORD    awMicrFixedID[P03_MICR_FIXED_NO] = {
                    IDD_P03_KEY_5A,
                    IDD_P03_KEY_5C, IDD_P03_KEY_6C, IDD_P03_KEY_7C,
                    IDD_P03_KEY_5D, IDD_P03_KEY_6D, IDD_P03_KEY_7D,
                    IDD_P03_KEY_5E, IDD_P03_KEY_6E, IDD_P03_KEY_7E,
                    IDD_P03_KEY_5F
                };

static  char    szExtRngPlu[] = "9999999999999";

static  WORD    awExtRngTbl[P03_FSC_NO] = {
                    0,                          /* No Function            0 */
                    9999,                       /* Keyed PLU Key    R3.1  1 */
                    50,                         /* Keyed Department Key   2 */
                    8,                          /* Print Modifier         3 */
                    20,                         /* Adjective              4 */
                    20,                         /* Tender                 5 */
                    8,                          /* Foreign Currency       6 */
                    9,                          /* Total                  7 *//* ### MOD Saratoga (8 --> 9)(052700) */
                    3,                          /* Add Check              8 */
                    6,                          /* Item Discount          9 */
                    6,                          /* Regular Discount      10 */
                    11,                         /* Tax Modifier,   R3.1  11 */
                    0,0,0,0,0,0,0,0,0,0,        /* FSC No.          12 - 21 *//* 14 is not used */
                    0,0,0,0,0,0,0,0,0,          /* FSC No.          22 - 30 */
                    0,                          /* Void                  31 */
                    0,                          /* E/C                   32 */
                    0,                          /* Preselect Void        33 */
                    0,                          /* Qty                   34 */
                    0,                          /* Menu Shift            35 */
                    0,                          /* # / Type              36 */
                    3,                          /* Print on Demand       37 */
                    0,                          /* Order No. Display (not used) 38 */
                    3,                          /* Charge Tips           39 */
                    0,                          /* PLU No.               40 */
                    0,0,0,0,0,0,0,0,0,0,        /* FSC No.          41 - 50 */
                    0,0,                        /* FSC No.          51 - 52 */
                    4,                          /* Preset Cash Tender    53 */
                    9,                          /* Direct Shift          54 */
                    0,0,                        /* FSC No.          55 - 56 */
                    100,                        /* Keyed Coupon Key      57 */
                    0,                          /* Coupon No Key         58 */
                    0,                          /* Scroll Up Key         59 */
                    0,                          /* Scroll Down Key       60 */
                    0,                          /* Right Arrow Key       61 */
                    0,                          /* Left Arrow Key        62 */
                    0,                          /* Wait Key              63 */
                    0,                          /* Paid Order Recall Key 64 */
                    200,                        /* Keyed C.String Key    65 */
                    0,                          /* Control String No Key 66 */
                    0,                          /* Alpha Name Key, R3.0  67 */
                    0,                          /* Left Display Key, R3.0 68 */
                    0,                          /* Bartender Key, R3.1 69 */
                    0,                          /* Split Key, R3.1 70 */
                    2,                          /* Seat # Key, R3.1 71 */
                    0,                          /* Item Transfer Key, R3.1 72 */
                    0,                          /* Repeat Key, R3.1 73 */
                    0,                          /* Cursor Void Key, R3.1 74 */
                    0,                          /* Price Check Key, R3.1 75 */
                    20,                          /* Money Declaration NCR2172  76*/
                    0,                          /* FSC No.                    77 */
                    0,                          /* Operator Interrupt A, V3.3 78 */
                    0,                          /* Operator Interrupt B, V3.3 79 */
/*** NCR2172 ***/
                    5,                          /* Adjective Price Level Set  80 */
                    0,                          /* Department Number          81 */
                    0,                          /* UPC E-Version              82 */
                    0,                          /* Price Enter Key            83 */
/* ### ADD Saratoga (052700) */
                    0,                          /* Food Stamp Modifier Key    84 */
                    0,                          /* WIC Transaction Key        85 */
                    0                           /* WIC Modifier Key           86 */
                };

static  WORD    awConvKeyedPLU[P03_MICR_ITEM_NO] = {
                    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 0,
                    11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0,
                    21, 22, 23, 0, 24, 25, 26, 27, 28, 29, 0,
                    30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 0,
                    40, 41, 42, 0, 0, 0, 43, 44, 45, 46, 0,
                    47, 48, 49, 0, 0, 0, 50, 51, 52, 53, 0,
                    54, 55, 56, 0, 0, 0, 57, 58, 59, 60, 0,
                    61, 62, 63, 0, 0, 64, 65, 66, 67, 68, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                };

static  WORD    awMicrKeyedPLU[P03_MICR_ITEM_NO] = {
                    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                    12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
                    23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
                    34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44,
                    45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55,
                    56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66,
                    67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77,
                    78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88,
                    89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99,
                    100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110,
                    111, 112, 113, 114, 115, 0, 116, 117, 118, 119, 120,
                    121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131,
                    132, 133, 134, 135, 136, 0, 0, 0, 137, 138, 139,
                    140, 141, 142, 143, 144, 0, 0, 0, 145, 146, 147,
                    148, 149, 150, 151, 152, 0, 0, 0, 153, 154, 155,
                    156, 157, 158, 159, 160, 0, 161, 162, 163, 164, 165
                };

/* ===== End of P003TBL.C ===== */
