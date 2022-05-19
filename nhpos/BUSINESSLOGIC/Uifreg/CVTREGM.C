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
* Title       : Conversion Table for Register/Supervisor Mode (Micromotion K/B)
* Category    : NCR 2170 US Hospitality Application         
* Program Name: CVTREGM.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Jun-17-92:00.00.01:M.SUZUKI   : Initiale
*          :        :           :                                    
*===========================================================================
*===========================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
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
CVTTBL FARCONST CvtRegM[] =                 /* Pif */
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
     {0x00, UIE_FSC_BIAS + 164},            /* 6Bh */

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
     {0x00, UIE_FSC_BIAS + 156}};           /* E5h */



/*
*===========================================================================
*   Static Work Area 
*===========================================================================
*/


/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
