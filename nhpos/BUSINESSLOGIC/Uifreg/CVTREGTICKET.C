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
* --------------------------------------------------------------------------
  

  
	
*===========================================================================
*   Include File Declarations
*===========================================================================
*/
#include <windows.h>
#include <tchar.h>
#include <ecr.h>
#include <uie.h>
#include <fsc.h>
#include <cvt.h>




CVTTBL FARCONST CvtRegTicket[] =             /* Pif */
    {{0x40, 0x00},            /* 40h */
     {0x41, 0x00},            /* 41h */
     {0x42, 0x00},            /* 42h */
     {0x43, 0x00},            /* 43h */
     {0x44, 0x00},            /* 44h */
     {0x45, 0x00},            /* 45h */
     {0x46, 0x00},            /* 46h */
     {0x47, 0x00},            /* 47h */
     {0x48, 0x00},            /* 48h */
     {0x49, 0x00},            /* 49h */
     {0x4A, 0x00},            /* 4Ah */
     {0x4B, 0x00},            /* 4Bh */
     {0x4C, 0x00},            /* 4Ch */
     {0x4D, 0x00},            /* 4Dh */
     {0x4E, 0x00},            /* 4Eh */
     {0x4F, 0x00},            /* 4Fh */
     {0x50, 0x00},            /* 50h */
     {0x51, 0x00},			  /* 51h */
     {0x52, 0x00},            /* 52h */
     {0x53, 0x00},            /* 53h */
     {0x54, 0x00},            /* 54h */
     {0x55, 0x00},            /* 55h */
     {0x56, 0x00},            /* 56h */
     {0x57, 0x00},            /* 57h */
     {0x58, 0x00},            /* 58h */
     {0x59, 0x00},            /* 59h */
     {0x5A, 0x00},            /* 5Ah */

	 //Begin to Assign actual values that are allowed
     {0x00, UIE_FSC_BIAS + 0},            /* 5Bh */
     {0x00, UIE_FSC_BIAS + 1},            /* 5Ch */
     {0x00, UIE_FSC_BIAS + 2},            /* 5Dh */
     {0x00, UIE_FSC_BIAS + 3},            /* 5Eh */
     {0x00, UIE_FSC_BIAS + 4},            /* 5Fh */
     {0x00, UIE_FSC_BIAS + 5},            /* 60h */
     {0x00, UIE_FSC_BIAS + 6},            /* 61h */
     {0x00, UIE_FSC_BIAS + 7},            /* 62h */
     {0x00, UIE_FSC_BIAS + 8},            /* 63h */
     {0x00, UIE_FSC_BIAS + 9},            /* 64h */
     {0x00, UIE_FSC_BIAS + 10},           /* 65h */
     {0x00, UIE_FSC_BIAS + 11},           /* 66h */  
     {0x00, UIE_FSC_BIAS + 12},           /* 67h */
     {0x00, UIE_FSC_BIAS + 13},           /* 68h */
     {0x00, UIE_FSC_BIAS + 14},           /* 69h */
     {0x00, UIE_FSC_BIAS + 15},           /* 6Ah */
     {0x00, UIE_FSC_BIAS + 16},           /* 6Bh FT 1*/
     {0x00, UIE_FSC_BIAS + 17},           /* 6Ch */
     {0x00, UIE_FSC_BIAS + 18},           /* 6Dh */
     {0x00, UIE_FSC_BIAS + 19},            /* 6Eh */
     {0x00, UIE_FSC_BIAS + 20},            /* 6Fh */
     {0x00, UIE_FSC_BIAS + 21},            /* 70h */
     {0x00, UIE_FSC_BIAS + 22},            /* 71h */
     {0x00, UIE_FSC_BIAS + 23},            /* 72h */
     {0x00, UIE_FSC_BIAS + 24},            /* 73h */
	 //End actual values that are assignable


     {VK_ESCAPE, FSC_CLEAR},            /* 74h */
     {VK_INSERT, UIE_FSC_BIAS + 9},            /* 75h */
     {VK_END, UIE_FSC_BIAS + 10},           /* 76h */
     {VK_DOWN, UIE_FSC_BIAS + 11},           /* 77h */
     {VK_NEXT, UIE_FSC_BIAS + 12},             /* 78h */
     {VK_LEFT, UIE_FSC_BIAS + 13},             /* 79h */
     {VK_CLEAR, UIE_FSC_BIAS + 14},             /* 7Ah */
     {VK_RIGHT, UIE_FSC_BIAS + 15},             /* 7Bh */
     {VK_HOME, UIE_FSC_BIAS + 16},             /* 7Ch */
     {VK_UP, UIE_FSC_BIAS + 17},             /* 7Dh */
     {VK_PRIOR, UIE_FSC_BIAS + 18},             /* 7Eh */
     {VK_RETURN, UIE_FSC_BIAS + 19},             /* 7Fh */
     {VK_ADD, UIE_FSC_BIAS + 20},             /* 80h */
     {VK_DIVIDE, UIE_FSC_BIAS + 21},             /* 81h */                           
     {VK_DELETE, UIE_FSC_BIAS + 22},             /* 82h */
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
     {0x00, UIE_FSC_BIAS + 99},             /* CFh */
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

	 //PLEASE NOTE: when adding new FSC's to this table, please update these to
	 //the last position after the FSC. They are to be changed in frameworkwndbutton.cpp
	 {0x00, FSC_PRICE_GUARD},				/* FF07h */
	 {0x00, FSC_QTY_GUARD},					/* FF08 */
	
};


