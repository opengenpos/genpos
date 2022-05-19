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
* Title       : Conversion Table for Register/Supervisor Mode (NCR 5932 Wedge 68 key K/B)
* Category    : NCR 2170 US Hospitality Application         
* Program Name: CVTREGC.C
* --------------------------------------------------------------------------
* Abstract:	This file contains the translation table used with the NCR 5932
		Wedge 68 key keyboard.  This translation table is used by the
		function UieConvertFsc () in file uiekey.c to determine if a
		key press is mapped to an FSC code or is to be treated as an
		alphanumeric key press instead.

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
CVTTBL FARCONST CvtReg5932_68 [] =          /* Code Sent From Framework to BL */
//Row 1
    {{0x00, UIE_FSC_BIAS +  0},             /* 40h */
     {0x00, UIE_FSC_BIAS +  1},             /* 41h */
     {0x00, UIE_FSC_BIAS +  2},             /* 42h */
     {0x00, UIE_FSC_BIAS +  3},             /* 43h */
     {0x00, UIE_FSC_BIAS +  4},             /* 44h */
     {0x00, UIE_FSC_BIAS +  5},             /* 45h */
     {0x00, UIE_FSC_BIAS +  6},             /* 46h */
     {0x00, UIE_FSC_BIAS +  7},             /* 47h */

//Row 2
     {0x00, UIE_FSC_BIAS +  8},             /* 48h */
     {0x00, UIE_FSC_BIAS +  9},             /* 49h */
     {0x00, UIE_FSC_BIAS + 10},             /* 4Ah */
     {0x00, UIE_FSC_BIAS + 11},             /* 4Bh */
     {0x00, UIE_FSC_BIAS + 12},             /* 4Ch */
     {0x00, UIE_FSC_BIAS + 13},             /* 4Dh */
     {0x00, UIE_FSC_BIAS + 14},             /* 4Eh */
     {0x00, UIE_FSC_BIAS + 15},             /* 4Fh */

//Row 3
     {0x00, UIE_FSC_BIAS + 16},             /* 50h */
     {0x00, UIE_FSC_BIAS + 17},             /* 51h */
     {0x00, UIE_FSC_BIAS + 18},             /* 52h */
     {0x00, FSC_CLEAR},						/* 53h */
     {0x00, UIE_FSC_BIAS + 19},				/* 54h */
     {0x00, UIE_FSC_BIAS + 20},             /* 55h */
     {0x00, UIE_FSC_BIAS + 21},             /* 56h */
     {0x00, UIE_FSC_BIAS + 22},             /* 57h */

//Row 4
     {0x00, UIE_FSC_BIAS + 23},             /* 58h */
     {0x00, UIE_FSC_BIAS + 24},             /* 59h */
     {0x00, UIE_FSC_BIAS + 25},             /* 5Ah */
     {0x00, UIE_FSC_BIAS + 26},             /* 5Bh */
     {0x00, UIE_FSC_BIAS + 27},             /* 5Ch */
     {0x00, UIE_FSC_BIAS + 28},             /* 5Dh */
     {0x00, UIE_FSC_BIAS + 29},             /* 5Eh */
     {0x00, UIE_FSC_BIAS + 30},             /* 5Fh */

//Row 5
     {0x00, UIE_FSC_BIAS + 31},             /* 60h */
     {0x00, UIE_FSC_BIAS + 32},             /* 61h */
     {0x00, UIE_FSC_BIAS + 33},             /* 62h */
                                            /* '7' */
                                            /* '8' */
                                            /* '9' */
     {0x00, UIE_FSC_BIAS + 34},             /* 63h */
     {0x00, UIE_FSC_BIAS + 35},             /* 64h */
     {0x00, UIE_FSC_BIAS + 36},             /* 65h */

//Row 6
     {0x00, UIE_FSC_BIAS + 37},             /* 66h */
     {0x00, UIE_FSC_BIAS + 38},             /* 67h */
     {0x00, UIE_FSC_BIAS + 39},             /* 68h */
                                            /* '4' */
                                            /* '5' */
                                            /* '6' */
     {0x00, UIE_FSC_BIAS + 40},             /* 69h */
     {0x00, UIE_FSC_BIAS + 41},             /* 6Ah */
     {0x00, UIE_FSC_BIAS + 42},             /* 6Bh */

//Row 7
     {0x00, UIE_FSC_BIAS + 43},             /* 6Ch */
     {0x00, UIE_FSC_BIAS + 44},             /* 6Dh */
     {0x00, UIE_FSC_BIAS + 45},             /* 6Eh */
                                            /* '1' */
                                            /* '2' */
                                            /* '3' */
     {0x00, UIE_FSC_BIAS + 46},             /* 6Fh */
     {0x00, UIE_FSC_BIAS + 47},             /* 70h */
     {0x00, UIE_FSC_BIAS + 48},             /* 71h */

//Row 8
     {0x00, UIE_FSC_BIAS + 49},             /* 72h */
     {0x00, UIE_FSC_BIAS + 50},             /* 73h */
     {0x00, UIE_FSC_BIAS + 51},             /* 74h */
                                            /* '0' */
     {0x00, UIE_FSC_BIAS + 52},             /* 75h */
     {0x00, UIE_FSC_BIAS + 53},             /* 76h */
     {0x00, UIE_FSC_BIAS + 54}};            /* 77h */

