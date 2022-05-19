/*                                                                                
*===========================================================================
* Title       : Conversion Table for Register/Supervisor Mode (Micromotion K/B)
* Category    : NCR 2170 US Hospitality Application         
* Program Name: CVTREGTOUCH.C
* --------------------------------------------------------------------------
* Abstract:	This file contains the translation table used with touchscreen.
		This translation table is used by the
		function UieConvertFsc () in file uiekey.c to determine if a
		key press is mapped to an FSC code or is to be treated as an
		alphanumeric key press instead.

  if a value exists in the uchASCII member the key is Alphanumeric
  if uchASCII is NULL then the value in uchScan is used to get the FSC

  When adding new FSC codes you should review the following other possible changes.
    - add new FSC_ define to file BusinessLogic\Include\fsc.h
	- add new CID_ define to file BusinessLogic\Uifreg\uifregloc.h
	- add new button press handler to CFrameworkWndButton::OnLButtonUp()
	- update table in BusinessLogic\Para\uniniram.c
	- update Layout Manager
	- update PEP
* --------------------------------------------------------------------------
  
: Update Histories
:    Date   : Ver.Rev. :   Name      : Description
: Sep-07-18 : 02.02.02 : R.Chambers  : Add FSC_PRESET_AMT #5, #6, #7, #8
  
	
*===========================================================================
*   Include File Declarations
*===========================================================================
*/
#include <tchar.h>
#include <ecr.h>
#include <uie.h>
#include <fsc.h>
#include <cvt.h>

// The touchscreen functionality for data entry is a layer on top of the previously
// existing keyboard functionality used by the NCR 7448 which had two different types
// of keyboards with different key layouts and different number of keys.
// The touchscreen functionality allows for only a single menu so there is no menu shift
// used to provide several different virtual keyboard layouts from a single physical keyboard.

// Framework translates button presses into messages sent to BusinessLogic
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

CVTTBL FARCONST CvtRegTouch[] =             /* Pif, Code Sent From Framework to BL */
    {{0x00, UIE_FSC_BIAS + 112},            /* 40h */
     {0x00, UIE_FSC_BIAS + 113},            /* 41h */
     {0x00, UIE_FSC_BIAS + 114},            /* 42h */
     {0x00, FSC_CLEAR},                     /* 43h */
     {0x00, UIE_FSC_BIAS + 115},            /* 44h */
     {0x00, UIE_FSC_BIAS + 116},            /* 45h */
     {0x00, UIE_FSC_BIAS + 117},            /* 46h */
     {0x00, UIE_FSC_BIAS + 118},            /* 47h */
     {0x00, UIE_FSC_BIAS + 119},            /* 48h */

     {0x00, UIE_FSC_BIAS + 122},            /* 49h */
     {0x00, UIE_FSC_BIAS + 123},            /* 4Ah */
     {0x00, UIE_FSC_BIAS + 124},            /* 4Bh */
     {0x00, UIE_FSC_BIAS + 125},            /* 4Ch */
     {0x00, UIE_FSC_BIAS + 126},            /* 4Dh */
     {0x00, UIE_FSC_BIAS + 127},            /* 4Eh */
     {0x00, UIE_FSC_BIAS + 128},            /* 4Fh */
     {0x00, UIE_FSC_BIAS + 129},            /* 50h */
     {0x00, UIE_FSC_BIAS + 130},            /* 51h */

     {0x00, UIE_FSC_BIAS + 133},            /* 52h */
     {0x00, UIE_FSC_BIAS + 134},            /* 53h */
     {0x00, UIE_FSC_BIAS + 135},            /* 54h */
                                            /* '7' */
                                            /* '8' */
                                            /* '9' */
     {0x00, UIE_FSC_BIAS + 136},            /* 55h */
     {0x00, UIE_FSC_BIAS + 137},            /* 56h */
     {0x00, UIE_FSC_BIAS + 138},            /* 57h */

     {0x00, UIE_FSC_BIAS + 141},            /* 58h */
     {0x00, UIE_FSC_BIAS + 142},            /* 59h */
     {0x00, UIE_FSC_BIAS + 143},            /* 5Ah */
                                            /* '4' */
                                            /* '5' */
                                            /* '6' */
     {0x00, UIE_FSC_BIAS + 144},            /* 5Bh */
     {0x00, UIE_FSC_BIAS + 145},            /* 5Ch */
     {0x00, UIE_FSC_BIAS + 146},            /* 5Dh */

     {0x00, UIE_FSC_BIAS + 149},            /* 5Eh */
     {0x00, UIE_FSC_BIAS + 150},            /* 5Fh */
     {0x00, UIE_FSC_BIAS + 151},            /* 60h */
                                            /* '1' */
                                            /* '2' */
                                            /* '3' */
     {0x00, UIE_FSC_BIAS + 152},            /* 61h */
     {0x00, UIE_FSC_BIAS + 153},            /* 62h */
     {0x00, UIE_FSC_BIAS + 154},            /* 63h */

     {0x00, UIE_FSC_BIAS + 157},            /* 64h */
     {0x00, UIE_FSC_BIAS + 158},            /* 65h */
     {0x00, UIE_FSC_BIAS + 159},            /* 66h */
                                            /* '0' */    
     {0x00, UIE_FSC_BIAS + 160},            /* 67h */
     {0x00, UIE_FSC_BIAS + 161},            /* 68h */
     {0x00, UIE_FSC_BIAS + 162},            /* 69h */
     {0x00, UIE_FSC_BIAS + 163},            /* 6Ah */

     {0x00, UIE_FSC_BIAS + 164},            /* 6Bh FT 1*/
     {0x00, UIE_FSC_BIAS + 0},              /* 6Ch */
     {0x00, UIE_FSC_BIAS + 1},              /* 6Dh */
     {0x00, UIE_FSC_BIAS + 2},              /* 6Eh */
     {0x00, UIE_FSC_BIAS + 3},              /* 6Fh */
     {0x00, UIE_FSC_BIAS + 4},              /* 70h */
     {0x00, UIE_FSC_BIAS + 5},              /* 71h */
     {0x00, UIE_FSC_BIAS + 6},              /* 72h */
     {0x00, UIE_FSC_BIAS + 7},              /* 73h */
     {0x00, UIE_FSC_BIAS + 8},              /* 74h */
     {0x00, UIE_FSC_BIAS + 9},              /* 75h */
     {0x00, UIE_FSC_BIAS + 10},             /* 76h */

     {0x00, UIE_FSC_BIAS + 11},             /* 77h */
     {0x00, UIE_FSC_BIAS + 12},             /* 78h */
     {0x00, UIE_FSC_BIAS + 13},             /* 79h */
     {0x00, UIE_FSC_BIAS + 14},             /* 7Ah */
     {0x00, UIE_FSC_BIAS + 15},             /* 7Bh */
     {0x00, UIE_FSC_BIAS + 16},             /* 7Ch */
     {0x00, UIE_FSC_BIAS + 17},             /* 7Dh */
     {0x00, UIE_FSC_BIAS + 18},             /* 7Eh */
     {0x00, UIE_FSC_BIAS + 19},             /* 7Fh */
     {0x00, UIE_FSC_BIAS + 20},             /* 80h */
     {0x00, UIE_FSC_BIAS + 21},             /* 81h */
                            
     {0x00, UIE_FSC_BIAS + 22},             /* 82h */
     {0x00, UIE_FSC_BIAS + 23},             /* 83h */
     {0x00, UIE_FSC_BIAS + 24},             /* 84h */
     {0x00, UIE_FSC_BIAS + 25},             /* 85h */
     {0x00, UIE_FSC_BIAS + 26},             /* 86h */
     {0x00, UIE_FSC_BIAS + 27},             /* 87h */
     {0x00, UIE_FSC_BIAS + 28},             /* 88h */
     {0x00, UIE_FSC_BIAS + 29},             /* 89h */
     {0x00, UIE_FSC_BIAS + 30},             /* 8Ah */
     {0x00, UIE_FSC_BIAS + 31},             /* 8Bh */
     {0x00, UIE_FSC_BIAS + 32},             /* 8Ch */

     {0x00, UIE_FSC_BIAS + 33},             /* 8Dh */
     {0x00, UIE_FSC_BIAS + 34},             /* 8Eh */
     {0x00, UIE_FSC_BIAS + 35},             /* 8Fh */
     {0x00, UIE_FSC_BIAS + 36},             /* 90h */
     {0x00, UIE_FSC_BIAS + 37},             /* 91h */
     {0x00, UIE_FSC_BIAS + 38},             /* 92h */
     {0x00, UIE_FSC_BIAS + 39},             /* 93h */
     {0x00, UIE_FSC_BIAS + 40},             /* 94h */
     {0x00, UIE_FSC_BIAS + 41},             /* 95h */
     {0x00, UIE_FSC_BIAS + 42},             /* 96h */ 
     {0x00, UIE_FSC_BIAS + 43},             /* 97h */

     {0x00, UIE_FSC_BIAS + 44},             /* 98h */
     {0x00, UIE_FSC_BIAS + 45},             /* 99h */
     {0x00, UIE_FSC_BIAS + 46},             /* 9Ah */
     {0x00, UIE_FSC_BIAS + 47},             /* 9Bh */
     {0x00, UIE_FSC_BIAS + 48},             /* 9Ch */
     {0x00, UIE_FSC_BIAS + 49},             /* 9Dh */
     {0x00, UIE_FSC_BIAS + 50},             /* 9Eh */
     {0x00, UIE_FSC_BIAS + 51},             /* 9Fh */
     {0x00, UIE_FSC_BIAS + 52},             /* A0h */
     {0x00, UIE_FSC_BIAS + 53},             /* A1h */
     {0x00, UIE_FSC_BIAS + 54},             /* A2h */

     {0x00, UIE_FSC_BIAS + 55},             /* A3h */
     {0x00, UIE_FSC_BIAS + 56},             /* A4h */
     {0x00, UIE_FSC_BIAS + 57},             /* A5h */
     {0x00, UIE_FSC_BIAS + 58},             /* A6h */
     {0x00, UIE_FSC_BIAS + 59},             /* A7h */
     {0x00, UIE_FSC_BIAS + 60},             /* A8h */
     {0x00, UIE_FSC_BIAS + 61},             /* A9h */
     {0x00, UIE_FSC_BIAS + 62},             /* AAh */
     {0x00, UIE_FSC_BIAS + 63},             /* ABh */
     {0x00, UIE_FSC_BIAS + 64},             /* ACh */
     {0x00, UIE_FSC_BIAS + 65},             /* ADh */

     {0x00, UIE_FSC_BIAS + 66},             /* AEh */
     {0x00, UIE_FSC_BIAS + 67},             /* AFh */
     {0x00, UIE_FSC_BIAS + 68},             /* B0h */
     {0x00, UIE_FSC_BIAS + 69},             /* B1h */
     {0x00, UIE_FSC_BIAS + 70},             /* B2h */
     {0x00, UIE_FSC_BIAS + 71},             /* B3h */
     {0x00, UIE_FSC_BIAS + 72},             /* B4h */
     {0x00, UIE_FSC_BIAS + 73},             /* B5h */
     {0x00, UIE_FSC_BIAS + 74},             /* B6h */
     {0x00, UIE_FSC_BIAS + 75},             /* B7h */
     {0x00, UIE_FSC_BIAS + 76},             /* B8h */
                            
     {0x00, UIE_FSC_BIAS + 77},             /* B9h */
     {0x00, UIE_FSC_BIAS + 78},             /* BAh */
     {0x00, UIE_FSC_BIAS + 79},             /* BBh */
     {0x00, UIE_FSC_BIAS + 80},             /* BCh */
     {0x00, UIE_FSC_BIAS + 81},             /* BDh */
     {0x00, UIE_FSC_BIAS + 82},             /* BEh */
     {0x00, UIE_FSC_BIAS + 83},             /* BFh */
     {0x00, UIE_FSC_BIAS + 84},             /* C0h */
     {0x00, UIE_FSC_BIAS + 85},             /* C1h */
     {0x00, UIE_FSC_BIAS + 86},             /* C2h */
     {0x00, UIE_FSC_BIAS + 87},             /* C3h */

     {0x00, UIE_FSC_BIAS + 88},             /* C4h */
     {0x00, UIE_FSC_BIAS + 89},             /* C5h */
     {0x00, UIE_FSC_BIAS + 90},             /* C6h */
     {0x00, UIE_FSC_BIAS + 91},             /* C7h */
     {0x00, UIE_FSC_BIAS + 92},             /* C8h */
     {0x00, UIE_FSC_BIAS + 93},             /* C9h */
     {0x00, UIE_FSC_BIAS + 94},             /* CAh */
     {0x00, UIE_FSC_BIAS + 95},             /* CBh */
     {0x00, UIE_FSC_BIAS + 96},             /* CCh */
     {0x00, UIE_FSC_BIAS + 97},             /* CDh */
     {0x00, UIE_FSC_BIAS + 98},             /* CEh */
                            
     {0x00, UIE_FSC_BIAS + 99},             /* CFh Preset Amount #1, FSC_PRESET_AMT 1 */
     {0x00, UIE_FSC_BIAS + 100},            /* D0h */
     {0x00, UIE_FSC_BIAS + 101},            /* D1h */
     {0x00, UIE_FSC_BIAS + 102},            /* D2h */
     {0x00, UIE_FSC_BIAS + 103},            /* D3h */
     {0x00, UIE_FSC_BIAS + 104},            /* D4h */
     {0x00, UIE_FSC_BIAS + 105},            /* D5h */
     {0x00, UIE_FSC_BIAS + 106},            /* D6h */
     {0x00, UIE_FSC_BIAS + 107},            /* D7h */
     {0x00, UIE_FSC_BIAS + 108},            /* D8h */
     {0x00, UIE_FSC_BIAS + 109},            /* D9h */

     {0x00, UIE_FSC_BIAS + 110},            /* DAh */
     {0x00, UIE_FSC_BIAS + 111},            /* DBh */

     {0x00, UIE_FSC_BIAS + 120},            /* DCh */
     {0x00, UIE_FSC_BIAS + 121},            /* DDh */

     {0x00, UIE_FSC_BIAS + 131},            /* DEh */
     {0x00, UIE_FSC_BIAS + 132},            /* DFh */

     {0x00, UIE_FSC_BIAS + 139},            /* E0h */
     {0x00, UIE_FSC_BIAS + 140},            /* E1h */

     {0x00, UIE_FSC_BIAS + 147},            /* E2h */
     {0x00, UIE_FSC_BIAS + 148},            /* E3h */

     {0x00, UIE_FSC_BIAS + 155},            /* E4h */
     {0x00, UIE_FSC_BIAS + 156},			/* E5h */


			
	 {0x00, UIE_FSC_BIAS + 165},			/*E6 FT 2*/
	 {0x00, UIE_FSC_BIAS + 166},			/*E7 FT 3*/
	 {0x00, UIE_FSC_BIAS + 167},			/*E8 FT 4*/
	 {0x00, UIE_FSC_BIAS + 168},			/*E9 FT 5*/
	 {0x00, UIE_FSC_BIAS + 169},			/*EA FT 6*/
	 {0x00, UIE_FSC_BIAS + 170},			/*EB FT 7*/
	 {0x00, UIE_FSC_BIAS + 171},			/*EC FT 8*/
	 {0x00, UIE_FSC_BIAS + 172},			/*ED Tender 10*/
	 {0x00, UIE_FSC_BIAS + 173},			/*EE Tender 11*/
	 {0x00, UIE_FSC_BIAS + 174},			/*EF HALO Override*/
	 {0x00, UIE_FSC_BIAS + 175},			/*F0 @/For Key*/
	 {0x00, UIE_FSC_BIAS + 176},			/*F1 Manual Validation*/
	 {0x00, UIE_FSC_BIAS + 177},			/*F2 Tender 12*/
	 {0x00, UIE_FSC_BIAS + 178},			/*F3 Tender 13*/
	 {0x00, UIE_FSC_BIAS + 179},			/*F4 Tender 14*/
	 {0x00, UIE_FSC_BIAS + 180},			/*F5 Tender 15*/
	 {0x00, UIE_FSC_BIAS + 181},			/*F6 Tender 16*/
	 {0x00, UIE_FSC_BIAS + 182},			/*F7 Tender 17*/
	 {0x00, UIE_FSC_BIAS + 183},			/*F8 Tender 18*/
	 {0x00, UIE_FSC_BIAS + 184},			/*F9 Tender 19*/
	 {0x00, UIE_FSC_BIAS + 185},			/*FA Tender 20*/
	 {0x00, UIE_FSC_BIAS + 186},			/*FB Total 10*/
	 {0x00, UIE_FSC_BIAS + 187},			/*FC Total 11*/
	 {0x00, UIE_FSC_BIAS + 188},			/*FD Total 12*/
	 {0x00, UIE_FSC_BIAS + 189},			/*FE Total 13*/
	 {0x00, UIE_FSC_BIAS + 190},			/*FF Total 14*/
	 {0x00, UIE_FSC_BIAS + 191},			/*FF01 Total 15*/
	 {0x00, UIE_FSC_BIAS + 192},			/*FF02 Total 16*/
	 {0x00, UIE_FSC_BIAS + 193},			/*FF03 Total 17*/
	 {0x00, UIE_FSC_BIAS + 194},			/*FF04 Total 18*/
	 {0x00, UIE_FSC_BIAS + 195},			/*FF05 Total 19*/
	 {0x00, UIE_FSC_BIAS + 196},			/*FF06 Total 20*/
	 {0x00, UIE_FSC_BIAS + 197},			/*FF07 Print on Demand 4*/
	 {0x00, UIE_FSC_BIAS + 198},			/*FF08 Item Discount 3*/
	 {0x00, UIE_FSC_BIAS + 199},			/*FF09 Item Discount 4*/
	 {0x00, UIE_FSC_BIAS + 200},			/*FF0A Item Discount 5*/
	 {0x00, UIE_FSC_BIAS + 201},			/*FF0B Item Discount 6*/
	 {0x00, UIE_FSC_BIAS + 202},			/*FF0C Edit Condiment */
	 ///The keys below work for preset PLU inforamtion that is stored
	 //in para
	 {0x00, UIE_FSC_BIAS + 203},			/*FF0D Keyed PLU Key 1*/
	 {0x00, UIE_FSC_BIAS + 204},			/*FF0E Keyed PLU Key 2*/
	 {0x00, UIE_FSC_BIAS + 205},			/*FF0F Keyed PLU Key 3*/
	 {0x00, UIE_FSC_BIAS + 206},			/*FF10 Keyed PLU Key 4*/
	 {0x00, UIE_FSC_BIAS + 207},			/*FF11 Keyed PLU Key 5*/
	 {0x00, UIE_FSC_BIAS + 208},			/*FF12 Keyed PLU Key 6*/
	 {0x00, UIE_FSC_BIAS + 209},			/*FF13 Keyed PLU Key 7*/
	 {0x00, UIE_FSC_BIAS + 210},			/*FF14 Keyed PLU Key 8*/
	 {0x00, UIE_FSC_BIAS + 211},			/*FF15 Keyed PLU Key 9*/
	 {0x00, UIE_FSC_BIAS + 212},			/*FF16 Keyed PLU Key 10*/
	 {0x00, UIE_FSC_BIAS + 213},			/*FF17 Receipt ID (Tent) FSC_TENT */
	 {0x00, UIE_FSC_BIAS + 214},			/*FF18 Order Declaration 1*/
	 {0x00, UIE_FSC_BIAS + 215},			/*FF19 Order Declaration 2*/
	 {0x00, UIE_FSC_BIAS + 216},			/*FF1A Order Declaration 3*/
	 {0x00, UIE_FSC_BIAS + 217},			/*FF1B Order Declaration 4*/
	 {0x00, UIE_FSC_BIAS + 218},			/*FF1C Order Declaration 5*/
	 {0x00, UIE_FSC_BIAS + 219},			/*FF1D Adjective Modifier */
	 {0x00, UIE_FSC_BIAS + 220},			/*FF1E Gift Card 1 BATypeGiftCard */
	 {0x00, UIE_FSC_BIAS + 221},			/*FF1F FreedomPay Balance 2*/
	 {0x00, UIE_FSC_BIAS + 222},			/*Free space */   /*FF20 Gift Card 3*/
	 {0x00, UIE_FSC_BIAS + 223},			/*Free space */   /*FF21 Gift Card 4*/
	 {0x00, UIE_FSC_BIAS + 224},			/*Free space */   /*FF22 Gift Card 5*/
	 {0x00, UIE_FSC_BIAS + 225},			/*FF23 Supervisor Intervention BATypeSuprIntrvn  FSC_SUPR_INTRVN */
	 {0x00, UIE_FSC_BIAS + 226},			/*FF24 launch document BATypeDocumentLaunch  FSC_DOC_LAUNCH */
	 {0x00, UIE_FSC_BIAS + 227},			/*FF25 launch document BATypeOperatorPickup  FSC_OPR_PICKUP */
	 {0x00, UIE_FSC_BIAS + 228},            /*FF26 Edit Table Condiment BATypeEditCondiment Table */
	 {0x00, UIE_FSC_BIAS + 229},            //FF27 Display a labeled window, FSC_WINDOW_DISPLAY or BATypeDisplayLabeledWindow
	 {0x00, UIE_FSC_BIAS + 230},            //FF28 Dismiss a labeled window, FSC_WINDOW_DISMISS or BATypeDismissLabeledWindow
	 {0x00, UIE_FSC_BIAS + 231},            //FF29 Auto Sign Out, FSC_AUTO_SIGN_OUT or BATypeAutoSignOut
	 {0x00, UIE_FSC_BIAS + 232},            //FF2A Transaction Return, FSC_PRE_VOID 1 or BATypeTranVoid
	 {0x00, UIE_FSC_BIAS + 233},            //FF2B Transaction Return, FSC_CURSOR_VOID 1 or BATypeCursorVoid
	 {0x00, UIE_FSC_BIAS + 234},            //FF2C Transaction Return, FSC_CURSOR_VOID 2 or BATypeCursorVoid
	 {0x00, UIE_FSC_BIAS + 235},            //FF2D Transaction Return, FSC_CURSOR_VOID 3 or BATypeCursorVoid
	 {0x00, UIE_FSC_BIAS + 236},            //FF2E Transaction Return, FSC_PRE_VOID 2 or BATypeTranVoid
	 {0x00, UIE_FSC_BIAS + 237},            //FF2F Transaction Return, FSC_PRE_VOID 3 or BATypeTranVoid
	 {0x00, UIE_FSC_BIAS + 238},            //FF30 Transaction Return, FSC_CANCEL 1 or BATypeCancel
	 {0x00, UIE_FSC_BIAS + 239},            //FF31 Print on Demand 5, FSC_PRT_DEMAND 5 or BATypePrtDemand
	 {0x00, UIE_FSC_BIAS + 240},            //FF32 Preset Ammount: FSC_PRESET_AMT 5 or BATypePreCash  Sep-07-2018
	 {0x00, UIE_FSC_BIAS + 241},            //FF33 Preset Ammount: FSC_PRESET_AMT 6 or BATypePreCash  Sep-07-2018
	 {0x00, UIE_FSC_BIAS + 242},            //FF34 Preset Ammount: FSC_PRESET_AMT 7 or BATypePreCash  Sep-07-2018
	 {0x00, UIE_FSC_BIAS + 243},            //FF35 Preset Ammount: FSC_PRESET_AMT 8 or BATypePreCash  Sep-07-2018

	 // ** WARNING **:  When adding new FSC's to this table, please update the
	 // following defines in file FrameWorkWndButton.h to the last position after
	 // the FSC you have just added.
	 // They are to be changed for use in in frameworkwndbutton.h
	 // for use in frameworkwndbutton.cpp JHHJ
	 //
	 // If you see a problem with buttons that use the FSC_PRICE_GUARD_SCAN to
	 // guard against data for separate commands running into each other then
	 // check that these defines have not been changed.  One such button is the
	 // department button, FSC_KEYED_DEPT, which when pressed does the function
	 // of the most recently added FSC instead of a department function.

	 // Key functions that are part of the procedure for translating keypress to FSC
	 //  - BlNotifyData()  calls UiePutData() to put a device input into the input queue
	 //  - UieConvertFsc()  converts keypress CHARDATA input to FSC using a conversion table

	 /*
		#define FSC_PRICE_GUARD_ASCII		
		#define FSC_PRICE_GUARD_SCAN		

		#define FSC_QTY_GUARD_ASCII			
		#define FSC_QTY_GUARD_SCAN			

		#define FSC_CNT_STRING_GUARD_ASCII	
		#define FSC_CNT_STRING_GUARD_SCAN	
		*/
	 //They are to be changed for use in in frameworkwndbutton.h
	 //for use in frameworkwndbutton.cpp JHHJ
	 {0x00, FSC_PRICE_GUARD},				// FF2A FSC_PRICE_GUARD_SCAN
	 {0x00, FSC_QTY_GUARD},					// FF2B FSC_QTY_GUARD_SCAN
	 {0x00, FSC_CNTRL_STING_GRD},			// FF2C FSC_CNT_STRING_GUARD_SCAN
};

