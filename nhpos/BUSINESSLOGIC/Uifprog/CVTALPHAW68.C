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
		Wedge 68 key keyboard.  This translation table is used by the
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
//Framework translates key presses into messages sent to BusinessLogic
//The code that is sent gets translated into a FSC Code in the FSC Table
//The Code sent by Framework to Business Logic is commented to the right of
//The conversion table matching up to Which FSC in the FSC Table to use
//These FSC Codes can also be Hard Coded FSC's like FSC_CLEAR
//For a complete list "FSC.h"
//CVTTBL is made up of uchASCII and uchScan
//if uchASCII has a value then that is an Alpha Numeric Key
//And that code is used
//If uchASCII is NULL then uchScan/"FSC Code" is used

CVTTBL FARCONST CvtAlph5932_68_1 [] = /* Code Sent From Framework to BL */
//Row 1
    {{0x2F, FSC_NO_FUNC},             /* 40h */
     {0x41, FSC_NO_FUNC},             /* 41h */
     {0x42, FSC_NO_FUNC},             /* 42h */
     {0x43, FSC_NO_FUNC},             /* 43h */
     {0x44, FSC_NO_FUNC},             /* 44h */
     {0x45, FSC_NO_FUNC},             /* 45h */
     {0x46, FSC_NO_FUNC},             /* 46h */
     {0x47, FSC_NO_FUNC},             /* 47h */

//Row 2
     {0x5C, FSC_NO_FUNC},             /* 48h */
     {0x48, FSC_NO_FUNC},             /* 49h */
     {0x49, FSC_NO_FUNC},             /* 4Ah */
     {0x4A, FSC_NO_FUNC},             /* 4Bh */
     {0x4B, FSC_NO_FUNC},             /* 4Ch */
     {0x4C, FSC_NO_FUNC},             /* 4Dh */
     {0x4D, FSC_NO_FUNC},             /* 4Eh */
     {0x4E, FSC_NO_FUNC},             /* 4Fh */

//Row 3
     {0x2C, FSC_NO_FUNC},             /* 50h */
     {0x4F, FSC_NO_FUNC},             /* 51h */
     {0x50, FSC_NO_FUNC},             /* 52h */
     {0x00, FSC_CLEAR},				  /* 53h */
     {0x52, FSC_NO_FUNC},			  /* 54h */
     {0x53, FSC_NO_FUNC},             /* 55h */
     {0x54, FSC_NO_FUNC},             /* 56h */
     {0x55, FSC_NO_FUNC},             /* 57h */

//Row 4
     {0x5F, FSC_NO_FUNC},             /* 58h */
     {0x00, FSC_RECEIPT_FEED},        /* 59h */
     {0x00, FSC_JOURNAL_FEED},        /* 5Ah */
     {0x51, FSC_NO_FUNC},             /* 5Bh */
     {0x2A, FSC_NO_FUNC},             /* 5Ch */
     {0x2D, FSC_NO_FUNC},             /* 5Dh */
     {0x00, FSC_NO_FUNC},             /* 5Eh */
     {0x56, FSC_NO_FUNC},             /* 5Fh */

//Row 5
     {0x22, FSC_NO_FUNC},             /* 60h */
     {0x00, FSC_NO_FUNC},             /* 61h */
     {0x00, FSC_P3},				  /* 62h */
                                      /* '7' */
                                      /* '8' */
                                      /* '9' */
     {0x00, FSC_P4},				  /* 63h */
     {0x00, FSC_NO_FUNC},             /* 64h */
     {0x57, FSC_NO_FUNC},             /* 65h */

//Row 6
     {0x3A, FSC_NO_FUNC},             /* 66h */
     {0x00, FSC_NO_FUNC},             /* 67h */
     {0x00, FSC_SCROLL_UP},           /* 68h */
                                      /* '4' */
                                      /* '5' */
                                      /* '6' */
     {0x2B, FSC_NO_FUNC},             /* 69h */
     {0x00, FSC_NO_FUNC},             /* 6Ah */
     {0x58, FSC_NO_FUNC},             /* 6Bh */

//Row 7
     {0x00, FSC_BS},				  /* 6Ch */
     {0x00, FSC_NO_FUNC},             /* 6Dh */
     {0x00, FSC_SCROLL_DOWN},         /* 6Eh */
                                      /* '1' */
                                      /* '2' */
                                      /* '3' */
     {0x00, FSC_P1},				  /* 6Fh */
     {0x00, FSC_NO_FUNC},             /* 70h */
     {0x59, FSC_NO_FUNC},             /* 71h */

//Row 8
     {0x20, FSC_NO_FUNC},             /* 72h */
     {0x00, FSC_NO_FUNC},             /* 73h */
     {0x00, FSC_P2},				  /* 74h */
                                      /* '0' */
     {0x00, FSC_P5},				  /* 75h */
     {0x00, FSC_NO_FUNC},             /* 76h */
     {0x5A, FSC_NO_FUNC}};            /* 77h */


CVTTBL FARCONST CvtAlph5932_68_2 [] = /* Pif */
//Row 1
    {{0x2F, FSC_NO_FUNC},             /* 40h */
     {0x61, FSC_NO_FUNC},             /* 41h */
     {0x62, FSC_NO_FUNC},             /* 42h */
     {0x63, FSC_NO_FUNC},             /* 43h */
     {0x64, FSC_NO_FUNC},             /* 44h */
     {0x65, FSC_NO_FUNC},             /* 45h */
     {0x66, FSC_NO_FUNC},             /* 46h */
     {0x67, FSC_NO_FUNC},             /* 47h */

//Row 2
     {0x5C, FSC_NO_FUNC},             /* 48h */
     {0x68, FSC_NO_FUNC},             /* 49h */
     {0x69, FSC_NO_FUNC},             /* 4Ah */
     {0x6A, FSC_NO_FUNC},             /* 4Bh */
     {0x6B, FSC_NO_FUNC},             /* 4Ch */
     {0x6C, FSC_NO_FUNC},             /* 4Dh */
     {0x6D, FSC_NO_FUNC},             /* 4Eh */
     {0x6E, FSC_NO_FUNC},             /* 4Fh */

//Row 3
     {0x2C, FSC_NO_FUNC},             /* 50h */
     {0x6F, FSC_NO_FUNC},             /* 51h */
     {0x70, FSC_NO_FUNC},             /* 52h */
     {0x00, FSC_CLEAR},				  /* 53h */
     {0x72, FSC_NO_FUNC},			  /* 54h */
     {0x73, FSC_NO_FUNC},             /* 55h */
     {0x74, FSC_NO_FUNC},             /* 56h */
     {0x75, FSC_NO_FUNC},             /* 57h */

//Row 4
     {0x5F, FSC_NO_FUNC},             /* 58h */
     {0x00, FSC_RECEIPT_FEED},        /* 59h */
     {0x00, FSC_JOURNAL_FEED},        /* 5Ah */
     {0x71, FSC_NO_FUNC},             /* 5Bh */
     {0x2A, FSC_NO_FUNC},             /* 5Ch */
     {0x2D, FSC_NO_FUNC},             /* 5Dh */
     {0x00, FSC_NO_FUNC},             /* 5Eh */
     {0x76, FSC_NO_FUNC},             /* 5Fh */

//Row 5
     {0x22, FSC_NO_FUNC},             /* 60h */
     {0x00, FSC_NO_FUNC},             /* 61h */
     {0x00, FSC_P3},				  /* 62h */
                                      /* '7' */
                                      /* '8' */
                                      /* '9' */
     {0x00, FSC_P4},				  /* 63h */
     {0x00, FSC_NO_FUNC},             /* 64h */
     {0x77, FSC_NO_FUNC},             /* 65h */

//Row 6
     {0x3A, FSC_NO_FUNC},             /* 66h */
     {0x00, FSC_NO_FUNC},             /* 67h */
     {0x00, FSC_SCROLL_UP},           /* 68h */
                                      /* '4' */
                                      /* '5' */
                                      /* '6' */
     {0x2B, FSC_NO_FUNC},             /* 69h */
     {0x00, FSC_NO_FUNC},             /* 6Ah */
     {0x78, FSC_NO_FUNC},             /* 6Bh */

//Row 7
     {0x00, FSC_BS},				  /* 6Ch */
     {0x00, FSC_NO_FUNC},             /* 6Dh */
     {0x00, FSC_SCROLL_DOWN},         /* 6Eh */
                                      /* '1' */
                                      /* '2' */
                                      /* '3' */
     {0x0D, FSC_P1},				  /* 6Fh */
     {0x00, FSC_NO_FUNC},             /* 70h */
     {0x79, FSC_NO_FUNC},             /* 71h */

//Row 8
     {0x20, FSC_NO_FUNC},             /* 72h */
     {0x00, FSC_NO_FUNC},             /* 73h */
     {0x00, FSC_P2},				  /* 74h */
                                      /* '0' */
     {0x00, FSC_P5},				  /* 75h */
     {0x00, FSC_NO_FUNC},             /* 76h */
     {0x7A, FSC_NO_FUNC}};            /* 77h */


