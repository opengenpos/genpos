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
* Title       : Conversion Table for ALPHA Mode (Micromotion K/B)
* Category    : NCR 2170 US Hospitality Application         
* Program Name: CVTALPHM.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Jun-16-92:00.00.01:M.Suzuki   : Initiale
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
CVTTBL FARCONST CvtAlphM[] =                /* Pif */
    {{0x8F, UIE_FSC_BIAS + 112},            /* 40h */
     {0x90, UIE_FSC_BIAS + 113},            /* 41h */
     {0x91, UIE_FSC_BIAS + 114},            /* 42h */
     {0x00, FSC_CLEAR},                     /* 43h */
     {0x93, UIE_FSC_BIAS + 115},            /* 44h */
     {0xE3, UIE_FSC_BIAS + 116},            /* 45h */
     {0x98, UIE_FSC_BIAS + 117},            /* 46h */
     {0x99, UIE_FSC_BIAS + 118},            /* 47h */
     {0x9A, UIE_FSC_BIAS + 119},            /* 48h */

     {0x9F, UIE_FSC_BIAS + 122},            /* 49h */
     {0xA0, UIE_FSC_BIAS + 123},            /* 4Ah */
     {0xA3, UIE_FSC_BIAS + 124},            /* 4Bh */
     {0x20, UIE_FSC_BIAS + 125},            /* 4Ch */
     {0xA4, UIE_FSC_BIAS + 126},            /* 4Dh */
     {0xA5, UIE_FSC_BIAS + 127},            /* 4Eh */
     {0xA6, UIE_FSC_BIAS + 128},            /* 4Fh */
     {0xA7, UIE_FSC_BIAS + 129},            /* 50h */
     {0x00, FSC_BS},                        /* 51h */

     {0xAA, UIE_FSC_BIAS + 133},            /* 52h */
     {0xAC, UIE_FSC_BIAS + 134},            /* 53h */
     {0x00, FSC_P3},                        /* 54h */
                                            /* '7' */
                                            /* '8' */
                                            /* '9' */
     {0x00, FSC_P4},                        /* 55h */
     {0xD0, UIE_FSC_BIAS + 137},            /* 56h */
     {0xF5, UIE_FSC_BIAS + 138},            /* 57h */

     {0xAF, UIE_FSC_BIAS + 141},            /* 58h */
     {0xB5, UIE_FSC_BIAS + 142},            /* 59h */
     {0xB6, UIE_FSC_BIAS + 143},            /* 5Ah */
                                            /* '4' */
                                            /* '5' */
                                            /* '6' */
     {0xF1, UIE_FSC_BIAS + 144},            /* 5Bh */
     {0xD1, UIE_FSC_BIAS + 145},            /* 5Ch */
     {0x7B, UIE_FSC_BIAS + 146},            /* 5Dh */

     {0xBD, UIE_FSC_BIAS + 149},            /* 5Eh */
     {0xC6, UIE_FSC_BIAS + 150},            /* 5Fh */
     {0xC7, UIE_FSC_BIAS + 151},            /* 60h */
                                            /* '1' */
                                            /* '2' */
                                            /* '3' */
     {0xF6, UIE_FSC_BIAS + 152},            /* 61h */
     {0x9C, UIE_FSC_BIAS + 153},            /* 62h */
     {0x7D, UIE_FSC_BIAS + 154},            /* 63h */

     {0xD2, UIE_FSC_BIAS + 157},            /* 64h */
     {0xD3, UIE_FSC_BIAS + 158},            /* 65h */
     {0x00, FSC_P2},                        /* 66h */
                                            /* '0' */
     
     {0xD4, UIE_FSC_BIAS + 160},            /* 67h */
     {0x00, FSC_P5},                        /* 68h */
     {0x00, FSC_P1},                        /* 69h */
     {0x92, UIE_FSC_BIAS + 163},            /* 6Ah */
     {0xD6, UIE_FSC_BIAS + 164},            /* 6Bh */
     
     {0x00, FSC_RECEIPT_FEED},              /* 6Ch */
     {0x00, FSC_JOURNAL_FEED},              /* 6Dh */
     {0x7C, UIE_FSC_BIAS + 57},             /* 6Eh */
     {0xD7, UIE_FSC_BIAS + 58},             /* 6Fh */
     {0xD8, UIE_FSC_BIAS + 59},             /* 70h */
     {0x9B, UIE_FSC_BIAS + 60},             /* 71h */
     {0x83, UIE_FSC_BIAS + 61},             /* 72h */
     {0x84, UIE_FSC_BIAS + 62},             /* 73h */
     {0x94, UIE_FSC_BIAS + 63},             /* 74h */
     {0x81, UIE_FSC_BIAS + 64},             /* 75h */
     {0x97, UIE_FSC_BIAS + 65},             /* 76h */

     {0x21, UIE_FSC_BIAS + 0},              /* 77h */
     {0x22, UIE_FSC_BIAS + 1},              /* 78h */
     {0x23, UIE_FSC_BIAS + 2},              /* 79h */
     {0x24, UIE_FSC_BIAS + 3},              /* 7Ah */
     {0x25, UIE_FSC_BIAS + 4},              /* 7Bh */
     {0x26, UIE_FSC_BIAS + 5},              /* 7Ch */
     {0x27, UIE_FSC_BIAS + 6},              /* 7Dh */
     {0x28, UIE_FSC_BIAS + 7},              /* 7Eh */
     {0x29, UIE_FSC_BIAS + 8},              /* 7Fh */
     {0x2D, UIE_FSC_BIAS + 9},              /* 80h */
     {0x3D, UIE_FSC_BIAS + 10},             /* 81h */

     {0x51, UIE_FSC_BIAS + 11},             /* 82h */
     {0x57, UIE_FSC_BIAS + 12},             /* 83h */
     {0x45, UIE_FSC_BIAS + 13},             /* 84h */
     {0x52, UIE_FSC_BIAS + 14},             /* 85h */
     {0x54, UIE_FSC_BIAS + 15},             /* 86h */
     {0x59, UIE_FSC_BIAS + 16},             /* 87h */
     {0x55, UIE_FSC_BIAS + 17},             /* 88h */
     {0x49, UIE_FSC_BIAS + 18},             /* 89h */
     {0x4F, UIE_FSC_BIAS + 19},             /* 8Ah */
     {0x50, UIE_FSC_BIAS + 20},             /* 8Bh */
     {0x40, UIE_FSC_BIAS + 21},             /* 8Ch */
                            
     {0x41, UIE_FSC_BIAS + 22},             /* 8Dh */
     {0x53, UIE_FSC_BIAS + 23},             /* 8Eh */
     {0x44, UIE_FSC_BIAS + 24},             /* 8Fh */
     {0x46, UIE_FSC_BIAS + 25},             /* 90h */
     {0x47, UIE_FSC_BIAS + 26},             /* 91h */
     {0x48, UIE_FSC_BIAS + 27},             /* 92h */
     {0x4A, UIE_FSC_BIAS + 28},             /* 93h */
     {0x4B, UIE_FSC_BIAS + 29},             /* 94h */
     {0x4C, UIE_FSC_BIAS + 30},             /* 95h */
     {0x3F, UIE_FSC_BIAS + 31},             /* 96h */
     {0x2C, UIE_FSC_BIAS + 32},             /* 97h */
                                                 
     {0x2A, UIE_FSC_BIAS + 33},             /* 98h */
     {0x5A, UIE_FSC_BIAS + 34},             /* 99h */
     {0x58, UIE_FSC_BIAS + 35},             /* 9Ah */
     {0x43, UIE_FSC_BIAS + 36},             /* 9Bh */
     {0x56, UIE_FSC_BIAS + 37},             /* 9Ch */
     {0x42, UIE_FSC_BIAS + 38},             /* 9Dh */
     {0x4E, UIE_FSC_BIAS + 39},             /* 9Eh */
     {0x4D, UIE_FSC_BIAS + 40},             /* 9Fh */
     {0x2F, UIE_FSC_BIAS + 41},             /* A0h */
     {0x5C, UIE_FSC_BIAS + 42},             /* A1h */ 
     {0x2E, UIE_FSC_BIAS + 43},             /* A2h */

     {0x3C, UIE_FSC_BIAS + 44},             /* A3h */
     {0x3E, UIE_FSC_BIAS + 45},             /* A4h */
     {0x2B, UIE_FSC_BIAS + 46},             /* A5h */
     {0x3B, UIE_FSC_BIAS + 47},             /* A6h */
     {0x3A, UIE_FSC_BIAS + 48},             /* A7h */
     {0x5B, UIE_FSC_BIAS + 49},             /* A8h */
     {0x5D, UIE_FSC_BIAS + 50},             /* A9h */
     {0xA1, UIE_FSC_BIAS + 51},             /* AAh */
     {0xAB, UIE_FSC_BIAS + 52},             /* ABh */
     {0x88, UIE_FSC_BIAS + 53},             /* ACh */
     {0x8A, UIE_FSC_BIAS + 54},             /* ADh */

     {0x85, UIE_FSC_BIAS + 66},             /* AEh */
     {0x87, UIE_FSC_BIAS + 67},             /* AFh */
     {0xBE, UIE_FSC_BIAS + 68},             /* B0h */
     {0xDE, UIE_FSC_BIAS + 69},             /* B1h */
     {0xE0, UIE_FSC_BIAS + 70},             /* B2h */
     {0xE1, UIE_FSC_BIAS + 71},             /* B3h */
     {0xE2, UIE_FSC_BIAS + 72},             /* B4h */
     {0x5E, UIE_FSC_BIAS + 73},             /* B5h */
     {0x5F, UIE_FSC_BIAS + 74},             /* B6h */
     {0x60, UIE_FSC_BIAS + 75},             /* B7h */
     {0x7E, UIE_FSC_BIAS + 76},             /* B8h */
                            
     {0x71, UIE_FSC_BIAS + 77},             /* B9h */
     {0x77, UIE_FSC_BIAS + 78},             /* BAh */
     {0x65, UIE_FSC_BIAS + 79},             /* BBh */
     {0x72, UIE_FSC_BIAS + 80},             /* BCh */
     {0x74, UIE_FSC_BIAS + 81},             /* BDh */
     {0x79, UIE_FSC_BIAS + 82},             /* BEh */
     {0x75, UIE_FSC_BIAS + 83},             /* BFh */
     {0x69, UIE_FSC_BIAS + 84},             /* C0h */
     {0x6F, UIE_FSC_BIAS + 85},             /* C1h */
     {0x70, UIE_FSC_BIAS + 86},             /* C2h */
     {0x7F, UIE_FSC_BIAS + 87},             /* C3h */

     {0x61, UIE_FSC_BIAS + 88},             /* C4h */
     {0x73, UIE_FSC_BIAS + 89},             /* C5h */
     {0x64, UIE_FSC_BIAS + 90},             /* C6h */
     {0x66, UIE_FSC_BIAS + 91},             /* C7h */
     {0x67, UIE_FSC_BIAS + 92},             /* C8h */
     {0x68, UIE_FSC_BIAS + 93},             /* C9h */
     {0x6A, UIE_FSC_BIAS + 94},             /* CAh */
     {0x6B, UIE_FSC_BIAS + 95},             /* CBh */
     {0x6C, UIE_FSC_BIAS + 96},             /* CCh */
     {0x80, UIE_FSC_BIAS + 97},             /* CDh */
     {0x85, UIE_FSC_BIAS + 98},             /* CEh */
                            
     {0xEB, UIE_FSC_BIAS + 99},             /* CFh */
     {0x7A, UIE_FSC_BIAS + 100},            /* D0h */
     {0x78, UIE_FSC_BIAS + 101},            /* D1h */
     {0x63, UIE_FSC_BIAS + 102},            /* D2h */
     {0x76, UIE_FSC_BIAS + 103},            /* D3h */
     {0x62, UIE_FSC_BIAS + 104},            /* D4h */
     {0x6E, UIE_FSC_BIAS + 105},            /* D5h */
     {0x6D, UIE_FSC_BIAS + 106},            /* D6h */
     {0x89, UIE_FSC_BIAS + 107},            /* D7h */
     {0x8B, UIE_FSC_BIAS + 108},            /* D8h */
     {0x8C, UIE_FSC_BIAS + 109},            /* D9h */

     {0x8D, UIE_FSC_BIAS + 110},            /* DAh */
     {0x8E, UIE_FSC_BIAS + 111},            /* DBh */

     {0x9D, UIE_FSC_BIAS + 120},            /* DCh */
     {0x9E, UIE_FSC_BIAS + 121},            /* DDh */

     {0xA8, UIE_FSC_BIAS + 131},            /* DEh */
     {0xA9, UIE_FSC_BIAS + 132},            /* DFh */

     {0xAD, UIE_FSC_BIAS + 139},            /* E0h */
     {0xAE, UIE_FSC_BIAS + 140},            /* E1h */

     {0xB7, UIE_FSC_BIAS + 147},            /* E2h */
     {0xB8, UIE_FSC_BIAS + 148},            /* E3h */

     {0x82, UIE_FSC_BIAS + 55},             /* E4h */
     {0xA2, UIE_FSC_BIAS + 56}};            /* E5h */


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
