/*
*===========================================================================
* Title       : Conversion Table for Program Mode (TOUCHSCREEN)
* Category    : NHPOS UIFPROG        
* Program Name: CVTPROGTOUCH.C
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name      : Description
* May-10-04:00.00.01:R.Herrington : Initial
*          :        :             :                                    
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
*===========================================================================
*   Compile Message Declarations
*===========================================================================
*/


/*                                                                                
*===========================================================================
*   Work Area Declarations
*===========================================================================
*/
/*
*===========================================================================
*   Public Work Area
*===========================================================================
*/
CVTTBL FARCONST CvtProgTouch[] =            /* Pif */
    {{0x00, FSC_NO_FUNC},                   /* 40h */
     {0x00, FSC_NO_FUNC},                   /* 41h */
     {0x00, FSC_NO_FUNC},                   /* 42h */
     {0x00, FSC_CLEAR},                     /* 43h */
     {0x00, FSC_NO_FUNC},                   /* 44h */
     {0x00, FSC_NO_FUNC},                   /* 45h */
     {0x00, FSC_NO_FUNC},                   /* 46h */
     {0x00, FSC_NO_FUNC},                   /* 47h */
     {0x00, FSC_NO_FUNC},                   /* 48h */

     {0x00, FSC_NO_FUNC},                   /* 49h */
     {0x00, FSC_NO_FUNC},                   /* 4Ah */
     {0x00, FSC_NO_FUNC},                   /* 4Bh */
     {0x00, FSC_NO_FUNC},                   /* 4Ch */
     {0x00, FSC_NO_FUNC},                   /* 4Dh */
     {0x00, FSC_NO_FUNC},                   /* 4Eh */
     {0x00, FSC_NO_FUNC},                   /* 4Fh */
     {0x00, FSC_NO_FUNC},                   /* 50h */
     {0x00, FSC_NO_FUNC},                   /* 51h */

     {0x00, FSC_NO_FUNC},                   /* 52h */
     {0x00, FSC_NO_FUNC},                   /* 53h */
     {0x00, FSC_P3},                        /* 54h */
                                            /* '7' */
                                            /* '8' */
                                            /* '9' */
     {0x00, FSC_P4},                        /* 55h */
     {0x00, FSC_NO_FUNC},                   /* 56h */
     {0x00, FSC_NO_FUNC},                   /* 57h */

     {0x00, FSC_NO_FUNC},                   /* 58h */
     {0x00, FSC_NO_FUNC},                   /* 59h */
     {0x00, FSC_NO_FUNC},                   /* 5Ah */
                                            /* '4' */
                                            /* '5' */
                                            /* '6' */
     {0x00, FSC_NO_FUNC},                   /* 5Bh */
     {0x00, FSC_NO_FUNC},                   /* 5Ch */
     {0x00, FSC_NO_FUNC},                   /* 5Dh */

     {0x00, FSC_NO_FUNC},                   /* 5Eh */
     {0x00, FSC_NO_FUNC},                   /* 5Fh */
     {0x00, FSC_NO_FUNC},                   /* 60h */
                                            /* '1' */
                                            /* '2' */
                                            /* '3' */
     {0x00, FSC_NO_FUNC},                   /* 61h */
     {0x00, FSC_NO_FUNC},                   /* 62h */
     {0x00, FSC_NO_FUNC},                   /* 63h */

     {0x00, FSC_NO_FUNC},                   /* 64h */
     {0x00, FSC_NO_FUNC},                   /* 65h */
     {0x00, FSC_P2},                        /* 66h */
                                            /* '0' */
     
     {0x00, FSC_NO_FUNC},                   /* 67h */
     {0x00, FSC_P5},                        /* 68h */
     {0x00, FSC_P1},                        /* 69h */
     {0x00, FSC_NO_FUNC},                   /* 6Ah */
     {0x00, FSC_NO_FUNC},                   /* 6Bh */
     
     {0x00, FSC_RECEIPT_FEED},              /* 6Ch */
     {0x00, FSC_JOURNAL_FEED},              /* 6Dh */
     {0x00, FSC_SCROLL_UP},                 /* 6Eh */
     {0x00, FSC_SCROLL_DOWN},               /* 6Fh */
     {0x00, FSC_NO_FUNC},                   /* 70h */
     {0x00, FSC_NO_FUNC},                   /* 71h */
     {0x00, FSC_NO_FUNC},                   /* 72h */
     {0x00, FSC_NO_FUNC},                   /* 73h */
     {0x00, FSC_NO_FUNC},                   /* 74h */
     {0x00, FSC_NO_FUNC},                   /* 75h */
     {0x00, FSC_NO_FUNC},                   /* 76h */

     {0x00, FSC_NO_FUNC},                   /* 77h */
     {0x00, FSC_NO_FUNC},                   /* 78h */
     {0x00, FSC_NO_FUNC},                   /* 79h */
     {0x00, FSC_NO_FUNC},                   /* 7Ah */
     {0x00, FSC_NO_FUNC},                   /* 7Bh */
     {0x00, FSC_NO_FUNC},                   /* 7Ch */
     {0x00, FSC_NO_FUNC},                   /* 7Dh */
     {0x00, FSC_NO_FUNC},                   /* 7Eh */
     {0x00, FSC_NO_FUNC},                   /* 7Fh */
     {0x00, FSC_NO_FUNC},                   /* 80h */
     {0x00, FSC_NO_FUNC},                   /* 81h */
                            
     {0x00, FSC_NO_FUNC},                   /* 82h *///66
     {0x00, FSC_NO_FUNC},                   /* 83h */
     {0x00, FSC_NO_FUNC},                   /* 84h */
     {0x00, FSC_NO_FUNC},                   /* 85h */
     {0x00, FSC_NO_FUNC},                   /* 86h */
     {0x00, FSC_NO_FUNC},                   /* 87h */
     {0x00, FSC_NO_FUNC},                   /* 88h */
     {0x00, FSC_NO_FUNC},                   /* 89h */
     {0x00, FSC_NO_FUNC},                   /* 8Ah */
     {0x00, FSC_NO_FUNC},                   /* 8Bh */
     {0x00, FSC_NO_FUNC},                   /* 8Ch */

     {0x00, FSC_NO_FUNC},                   /* 8Dh */
     {0x00, FSC_NO_FUNC},                   /* 8Eh */
     {0x00, FSC_NO_FUNC},                   /* 8Fh */
     {0x00, FSC_NO_FUNC},                   /* 90h */
     {0x00, FSC_NO_FUNC},                   /* 91h */
     {0x00, FSC_NO_FUNC},                   /* 92h */
     {0x00, FSC_NO_FUNC},                   /* 93h */
     {0x00, FSC_NO_FUNC},                   /* 94h */
     {0x00, FSC_NO_FUNC},                   /* 95h */
     {0x00, FSC_NO_FUNC},                   /* 96h */ 
     {0x00, FSC_NO_FUNC},                   /* 97h */

     {0x00, FSC_NO_FUNC},                   /* 98h */
     {0x00, FSC_NO_FUNC},                   /* 99h */
     {0x00, FSC_NO_FUNC},                   /* 9Ah */
     {0x00, FSC_NO_FUNC},                   /* 9Bh */
     {0x00, FSC_NO_FUNC},                   /* 9Ch */
     {0x00, FSC_NO_FUNC},                   /* 9Dh */
     {0x00, FSC_NO_FUNC},                   /* 9Eh */
     {0x00, FSC_NO_FUNC},                   /* 9Fh */
     {0x00, FSC_NO_FUNC},                   /* A0h */
     {0x00, FSC_NO_FUNC},                   /* A1h */
     {0x00, FSC_NO_FUNC},                   /* A2h */

     {0x00, FSC_NO_FUNC},                   /* A3h */
     {0x00, FSC_NO_FUNC},                   /* A4h */
     {0x00, FSC_NO_FUNC},                   /* A5h */
     {0x00, FSC_NO_FUNC},                   /* A6h */
     {0x00, FSC_NO_FUNC},                   /* A7h */
     {0x00, FSC_NO_FUNC},                   /* A8h */
     {0x00, FSC_NO_FUNC},                   /* A9h */
     {0x00, FSC_NO_FUNC},                   /* AAh */
     {0x00, FSC_NO_FUNC},                   /* ABh */
     {0x00, FSC_NO_FUNC},                   /* ACh */
     {0x00, FSC_NO_FUNC},                   /* ADh */

     {0x00, FSC_NO_FUNC},                   /* AEh */
     {0x00, FSC_NO_FUNC},                   /* AFh */
     {0x00, FSC_NO_FUNC},                   /* B0h */
     {0x00, FSC_NO_FUNC},                   /* B1h */
     {0x00, FSC_NO_FUNC},                   /* B2h */
     {0x00, FSC_NO_FUNC},                   /* B3h */
     {0x00, FSC_NO_FUNC},                   /* B4h */
     {0x00, FSC_NO_FUNC},                   /* B5h */
     {0x00, FSC_NO_FUNC},                   /* B6h */
     {0x00, FSC_NO_FUNC},                   /* B7h */
     {0x00, FSC_NO_FUNC},                   /* B8h */
                            
     {0x00, FSC_NO_FUNC},                   /* B9h */
     {0x00, FSC_NO_FUNC},                   /* BAh */
     {0x00, FSC_NO_FUNC},                   /* BBh */
     {0x00, FSC_NO_FUNC},                   /* BCh */
     {0x00, FSC_NO_FUNC},                   /* BDh */
     {0x00, FSC_NO_FUNC},                   /* BEh */
     {0x00, FSC_NO_FUNC},                   /* BFh */
     {0x00, FSC_NO_FUNC},                   /* C0h */
     {0x00, FSC_NO_FUNC},                   /* C1h */
     {0x00, FSC_NO_FUNC},                   /* C2h */
     {0x00, FSC_NO_FUNC},                   /* C3h */

     {0x00, FSC_NO_FUNC},                   /* C4h */
     {0x00, FSC_NO_FUNC},                   /* C5h */
     {0x00, FSC_NO_FUNC},                   /* C6h */
     {0x00, FSC_NO_FUNC},                   /* C7h */
     {0x00, FSC_NO_FUNC},                   /* C8h */
     {0x00, FSC_NO_FUNC},                   /* C9h */
     {0x00, FSC_NO_FUNC},                   /* CAh */
     {0x00, FSC_RECEIPT_FEED},              /* CBh */
     {0x00, FSC_JOURNAL_FEED},              /* CCh */
     {0x00, FSC_NO_FUNC},                   /* CDh */
     {0x00, FSC_NO_FUNC},                   /* CEh */
                            
     {0x00, FSC_NO_FUNC},                   /* CFh */
     {0x00, FSC_NO_FUNC},                   /* D0h */
     {0x00, FSC_NO_FUNC},                   /* D1h */
     {0x00, FSC_NO_FUNC},                   /* D2h */
     {0x00, FSC_NO_FUNC},                   /* D3h */
     {0x00, FSC_NO_FUNC},                   /* D4h */
     {0x00, FSC_NO_FUNC},                   /* D5h */
     {0x00, FSC_NO_FUNC},                   /* D6h */
     {0x00, FSC_SCROLL_UP},                 /* D7h */
     {0x00, FSC_SCROLL_DOWN},               /* D8h */
     {0x00, FSC_NO_FUNC},                   /* D9h */

     {0x00, FSC_NO_FUNC},                   /* DAh */
     {0x00, FSC_NO_FUNC},                   /* DBh */

     {0x00, FSC_NO_FUNC},                   /* DCh */
     {0x00, FSC_NO_FUNC},                   /* DDh */

     {0x00, FSC_NO_FUNC},                   /* DEh */
     {0x00, FSC_NO_FUNC},                   /* DFh */

     {0x00, FSC_NO_FUNC},                   /* E0h */
     {0x00, FSC_NO_FUNC},                   /* E1h */

     {0x00, FSC_NO_FUNC},                   /* E2h */
     {0x00, FSC_NO_FUNC},                   /* E3h */

     {0x00, FSC_NO_FUNC},                   /* E4h */
     {0x00, FSC_NO_FUNC},					/* E5h */			
	 {0x00, FSC_NO_FUNC},			/*E6 FT 2*/
	 {0x00, FSC_NO_FUNC},			/*E7 FT 3*/
	 {0x00, FSC_NO_FUNC},			/*E8 FT 4*/
	 {0x00, FSC_NO_FUNC},			/*E9 FT 5*/
	 {0x00, FSC_NO_FUNC},			/*EA FT 6*/
	 {0x00, FSC_NO_FUNC},			/*EB FT 7*/
	 {0x00, FSC_NO_FUNC},			/*EC FT 8*/
	 {0x00, FSC_NO_FUNC},			/*ED Tender 10*/
	 {0x00, FSC_NO_FUNC},			/*EE Tender 11*/
	 {0x00, FSC_NO_FUNC},			/*EF HALO Override*/
	 {0x00, FSC_NO_FUNC},			/*F0 @/For Key*/
	 {0x00, FSC_NO_FUNC},			/*F1 Manual Validation*/
	 {0x00, FSC_NO_FUNC},			/*F2 Tender 12*/
	 {0x00, FSC_NO_FUNC},			/*F3 Tender 13*/
	 {0x00, FSC_NO_FUNC},			/*F4 Tender 14*/
	 {0x00, FSC_NO_FUNC},			/*F5 Tender 15*/
	 {0x00, FSC_NO_FUNC},			/*F6 Tender 16*/
	 {0x00, FSC_NO_FUNC},			/*F7 Tender 17*/
	 {0x00, FSC_NO_FUNC},			/*F8 Tender 18*/
	 {0x00, FSC_NO_FUNC},			/*F9 Tender 19*/
	 {0x00, FSC_NO_FUNC},			/*FA Tender 20*/
	 {0x00, FSC_NO_FUNC},			/*FB Total 10*/
	 {0x00, FSC_NO_FUNC},			/*FC Total 11*/
	 {0x00, FSC_NO_FUNC},			/*FD Total 12*/
	 {0x00, FSC_NO_FUNC},			/*FE Total 13*/
	 {0x00, FSC_NO_FUNC},			/*FF00 Total 14*/
	 {0x00, FSC_NO_FUNC},			/*FF01 Total 15*/
	 {0x00, FSC_NO_FUNC},			/*FF02 Total 16*/
	 {0x00, FSC_NO_FUNC},			/*FF03 Total 17*/
	 {0x00, FSC_NO_FUNC},			/*FF04 Total 18*/
	 {0x00, FSC_NO_FUNC},			/*FF05 Total 19*/
	 {0x00, FSC_NO_FUNC},			/*FF06 Total 20*/
	 {0x00, FSC_NO_FUNC},			/*FF07 Print on Demand 4*/
	 {0x00, FSC_NO_FUNC},			/*FF08 Keyed PLU Key 1*/
	 {0x00, FSC_NO_FUNC},			/*FF09 Keyed PLU Key 2*/
	 {0x00, FSC_NO_FUNC},			/*FF0A Keyed PLU Key 3*/
	 {0x00, FSC_NO_FUNC},			/*FF0B Keyed PLU Key 4*/
	 {0x00, FSC_NO_FUNC},			/*FF0C Keyed PLU Key 5*/
	 {0x00, FSC_NO_FUNC},			/*FF0D Keyed PLU Key 6*/
	 {0x00, FSC_NO_FUNC},			/*FF0E Keyed PLU Key 7*/
	 {0x00, FSC_NO_FUNC},			/*FF0F Keyed PLU Key 8*/
	 {0x00, FSC_NO_FUNC},			/*FF10 Keyed PLU Key 9*/
	 {0x00, FSC_NO_FUNC},			/*FF11 Keyed PLU Key 10*/

	 //PLEASE NOTE: when adding new FSC's to this table, please update these to
	 //the last position after the FSC. And update the defines
	 /*
		#define FSC_PRICE_GUARD_ASCII		
		#define FSC_PRICE_GUARD_SCAN		

		#define FSC_QTY_GUARD_ASCII			
		#define FSC_QTY_GUARD_SCAN			

		#define FSC_CNT_STRING_GUARD_ASCII	
		#define FSC_CNT_STRING_GUARD_SCAN	
		*/
	 //They are to be changed for use in in frameworkwndbutton.h
	 //for use inframeworkwndbutton.cpp JHHJ
	 {0x00, FSC_PRICE_GUARD},				/* FF12h */
	 {0x00, FSC_QTY_GUARD},					/* FF13 */
	 {0x00, FSC_CNTRL_STING_GRD},			/* FF14 */
};