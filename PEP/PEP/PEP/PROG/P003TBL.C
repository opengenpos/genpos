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

/*
	The following tables indicates which keys on the keyboard are
	fixed keys which are not to be modified through the P03 dialog.

	These are basically the numeric keypad keys and the Clear key.
	These identifiers are used in the dialog template to map buttons
	on the displayed dialog to specific keys on the keyboard type as
	specified from the Configuration dialog.  Then in the function
	P03ChkFixKey () in P003.c, we determine which table to use based
	on the keyboard type specified and then do a fixed key check.

  *** Also Any key that should be 

	** WARNING:  These identifiers must match up with dialog template identifiers.

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

static  WORD    awWedge68FixedID[P03_WEDGE68_FIXED_NO] = {
																			IDD_P03_KEY_80, IDD_P03_KEY_90,
																			IDD_P03_KEY_81, IDD_P03_KEY_91,
            IDD_P03_KEY_32,													IDD_P03_KEY_82, IDD_P03_KEY_92,
																			IDD_P03_KEY_83, IDD_P03_KEY_93,
            IDD_P03_KEY_34, IDD_P03_KEY_44, IDD_P03_KEY_54,									IDD_P03_KEY_94,
            IDD_P03_KEY_35, IDD_P03_KEY_45, IDD_P03_KEY_55,									IDD_P03_KEY_95,
            IDD_P03_KEY_36, IDD_P03_KEY_46, IDD_P03_KEY_56,									IDD_P03_KEY_96,
            IDD_P03_KEY_37, IDD_P03_KEY_47,					IDD_P03_KEY_67,					IDD_P03_KEY_97
                };

static  WORD    awWedge78FixedID[1] = {
			IDD_P03_KEY_90				// dummy placeholder.  No fixed keys
                };

static  WORD    awTicketFixedID[P03_TICKET_FIXED_NO] = {
																			
																			
																			
																			
            IDD_P03_KEY_11, IDD_P03_KEY_21, IDD_P03_KEY_31,							
            IDD_P03_KEY_81, IDD_P03_KEY_91, IDD_P03_KEY_A1,							
            IDD_P03_KEY_42, IDD_P03_KEY_52, IDD_P03_KEY_62,							
            IDD_P03_KEY_03, IDD_P03_KEY_13,	IDD_P03_KEY_23,	
															IDD_P03_KEY_33,

                };

//    -----------------------------------------------------


/*
	The table following is used to determine the range of an Extended FSC.

	If the value of the table entry is 0, then it indicates that that
	type of FSC does not have a range of any kind.  See P03ShowStat ().

	Used in P003ExtDlgProc () and P003ExtDeptDlgProc () to determine
	the maximum value for the FSC value that will be allowed in the
	extended FSC value assignment dialog box.
	
 */
static  WCHAR    szExtRngPlu[] = WIDE("9999999999999");

static  WORD    awExtRngTbl[P03_FSC_NO] = {
                    0,                          /* No Function            0 */
                    9999,                       /* Keyed PLU Key    R3.1  1 */
                    50,                         /* Keyed Department Key   2 */
                    8,                          /* Print Modifier         3 */
                    20,                         /* Adjective              4 */
                    STD_TENDER_MAX,             /* Tender                 5 */
                    8,                          /* Foreign Currency       6 */
                    STD_TOTAL_MAX,                          /* Total                  7 *//* ### MOD Saratoga (8 --> 9)(052700) */
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
                    4,                          /* Print on Demand       37  3->4 Gift Receipt JHHJ 9-01-5 */
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
                    0,                           /* WIC Modifier Key           86 */
					0,							// HALO Override Key	87
					0,							// @/For Key			88
					0,							// Validation Key		89
					127,						// Alphanumeric ASCII key  90
					0,							// Clear key  91 (assignable Clear as opposed to hardcoded Clear key)
					0,							// Condiment Edit 92, P03_EDIT_CONDIMENT
					0,							// Receipt ID	93, P03_TENT
					5,							// Order Declaration 94, P03_ORDER_DEC
					0,							// Adjective Modifier, 95, P03_ADJ_MOD
					4,							// Gift Card, 96, P03_GIFT_CARD
					0,							// Supervisor Intervention, 97, P03_SUPR_INTRVN
					99,							// Launch Application, 98, P03_DOC_LAUNCH
					0							// Cash pickup in Reg Mode, 99, P03_OPR_PICKUP
                };

/*
	Following tables are used to determine which keys can be
	assigned as a PLU key meaning when the key is pressed, it
	will trigger a PLU lookup within NHPOS.

	If the table value is 0, then the key can not be used for PLU assignment.

	If the table value is non-zero, then the value is used to calculate the offset
	to use in the P03PLU PluKey[MAX_PAGE_NO][FSC_PLU_MAX] data structure.

	Notice that these tables are all sized to the size of awMicrKeyedPLU[],
	the table used by the NCR 7448 Micromotion keyboard, since that keyboard
	has the most keys and the FSC tables are sized to that size.

	By convention, the Clear key functionality is assigned to the key that is
	two rows above the 7 key.

	See function P03RepStr () and function P03DspStr () in P003.c
*/

static  WORD    awWedge68KeyedPLU[P03_MICR_ITEM_NO] = {
                     1,   2,   3,   4,   5,   6,   7,   8,   0,   0,   0,
                     9,  10,  11,  12,  13,  14,  15,  16,   0,   0,   0,
                    17,  18,  19,   0,  20,  21,  22,  23,   0,   0,   0,
                    24,  25,  26,  27,  28,  29,  30,  31,   0,   0,   0,
                    32,  33,  34,   0,   0,   0,  35,  36,  37,   0,   0,
                    38,  39,  40,   0,   0,   0,  41,  42,  43,   0,   0,
                    44,  45,  46,   0,   0,   0,  47,  48,  49,   0,   0,
                    50,  51,  52,   0,   0,  53,   0,  54,  55,   0,   0,
                     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
                };

// The Wedge 78 key keyboard is in column major order.  In other words the
// first row of the keyboard, containing 13 keys, is stored as the first column.
static  WORD    awWedge78KeyedPLU[P03_MICR_ITEM_NO] = {
                     1,  14,  27,  40,  53,  66,   0,   0,   0,   0,   0,
                     2,  15,  28,  41,  54,  67,   0,   0,   0,   0,   0,
                     3,  16,  29,  42,  55,  68,   0,   0,   0,   0,   0,
                     4,  17,  30,  43,  56,  69,   0,   0,   0,   0,   0,
                     5,  18,  31,  44,  57,  70,   0,   0,   0,   0,   0,
                     6,  19,  32,  45,  58,  71,   0,   0,   0,   0,   0,
                     7,  20,  33,  46,  59,  72,   0,   0,   0,   0,   0,
                     8,  21,  34,  47,  60,  73,   0,   0,   0,   0,   0,
                     9,  22,  35,  48,  61,  74,   0,   0,   0,   0,   0,
                    10,  23,  36,  49,  62,  75,   0,   0,   0,   0,   0,
                    11,  24,  37,  50,  63,  76,   0,   0,   0,   0,   0,
                    12,  25,  38,  51,  64,  77,   0,   0,   0,   0,   0,
                    13,  26,  39,  52,  65,  78,   0,   0,   0,   0,   0,
                     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
                };

static  WORD    awConvKeyedPLU[P03_MICR_ITEM_NO] = {
                     1,   2,   3,   4,   5,   6,   7,   8,   9,  10,   0,
                    11,  12,  13,  14,  15,  16,  17,  18,  19,  20,   0,
                    21,  22,  23,   0,  24,  25,  26,  27,  28,  29,   0,
                    30,  31,  32,  33,  34,  35,  36,  37,  38,  39,   0,
                    40,  41,  42,   0,   0,   0,  43,  44,  45,  46,   0,
                    47,  48,  49,   0,   0,   0,  50,  51,  52,  53,   0,
                    54,  55,  56,   0,   0,   0,  57,  58,  59,  60,   0,
                    61,  62,  63,   0,   0,  64,  65,  66,  67,  68,   0,
                     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
                };

static  WORD    awMicrKeyedPLU[P03_MICR_ITEM_NO] = {
                     1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,
                    12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,
                    23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,
                    34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,
                    45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,
                    56,  57,  58,  59,  60,  61,  62,  63,  64,  65,  66,
                    67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,
                    78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,
                    89,  90,  91,  92,  93,  94,  95,  96,  97,  98,  99,
                   100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110,
                   111, 112, 113, 114, 115,   0, 116, 117, 118, 119, 120,
                   121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131,
                   132, 133, 134, 135, 136,   0,   0,   0, 137, 138, 139,
                   140, 141, 142, 143, 144,   0,   0,   0, 145, 146, 147,
                   148, 149, 150, 151, 152,   0,   0,   0, 153, 154, 155,
                   156, 157, 158, 159, 160,   0, 161, 162, 163, 164, 165
                };

static  WORD    awBigTicket[P03_MICR_ITEM_NO] = {
                     1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,
                     12,  0,   0,   0,  13,  14,  15,  16,   0,   0,   0, 
					 17, 18,  19,  20,   0,   0,   0,  21,  22,  23,  24,
					 0,   0,   0,   0,   25,   0,   0,   0,   0,   0,   0, 
					 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,                 
                };

/* ===== End of P003TBL.C ===== */
