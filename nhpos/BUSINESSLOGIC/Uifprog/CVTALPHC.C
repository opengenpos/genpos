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
* Title       : Conversion Table for ALPHA Mode (Conventional K/B)
* Category    : NCR 2170 US Hospitality Application         
* Program Name: CVTALPHC.C
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
* Jun-20-92:00.00.01:M.Suzuki   : Initiale
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
CVTTBL FARCONST CvtAlphC1[] =               /* Pif */
    {{0x41, UIE_FSC_BIAS + 20},             /* 40h */
     {0x53, UIE_FSC_BIAS + 21},             /* 41h */
     {0x44, UIE_FSC_BIAS + 22},             /* 42h */
     {0x00, FSC_CLEAR},                     /* 43h */
     {0x46, UIE_FSC_BIAS + 23},             /* 44h */
     {0x47, UIE_FSC_BIAS + 24},             /* 45h */
     {0x48, UIE_FSC_BIAS + 25},             /* 46h */
     {0x4A, UIE_FSC_BIAS + 26},             /* 47h */
     {0x4B, UIE_FSC_BIAS + 27},             /* 48h */

     {0x5A, UIE_FSC_BIAS + 29},             /* 49h */
     {0x58, UIE_FSC_BIAS + 30},             /* 4Ah */
     {0x43, UIE_FSC_BIAS + 31},             /* 4Bh */
     {0x20, UIE_FSC_BIAS + 32},             /* 4Ch */
     {0x00, FSC_BS},                        /* 4Dh */
     {0x56, UIE_FSC_BIAS + 34},             /* 4Eh */
     {0x42, UIE_FSC_BIAS + 35},             /* 4Fh */
     {0x4E, UIE_FSC_BIAS + 36},             /* 50h */
     {0x4D, UIE_FSC_BIAS + 37},             /* 51h */

     {0x28, UIE_FSC_BIAS + 39},             /* 52h */
     {0x29, UIE_FSC_BIAS + 40},             /* 53h */
     {0x00, FSC_P3},                        /* 54h */
                                            /* '7' */
                                            /* '8' */
                                            /* '9' */
     {0x00, FSC_P4},                        /* 55h */
     {0x2D, UIE_FSC_BIAS + 43},             /* 56h */
     {0x27, UIE_FSC_BIAS + 44},             /* 57h */

     {0x2C, UIE_FSC_BIAS + 46},             /* 58h */
     {0x2E, UIE_FSC_BIAS + 47},             /* 59h */
     {0x3C, UIE_FSC_BIAS + 48},             /* 5Ah */
                                            /* '4' */
                                            /* '5' */
                                            /* '6' */
     {0x3E, UIE_FSC_BIAS + 49},             /* 5Bh */
     {0x2B, UIE_FSC_BIAS + 50},             /* 5Ch */
     {0x3B, UIE_FSC_BIAS + 51},             /* 5Dh */

     {0x5B, UIE_FSC_BIAS + 53},             /* 5Eh */
     {0x5D, UIE_FSC_BIAS + 54},             /* 5Fh */
     {0x7B, UIE_FSC_BIAS + 55},             /* 60h */
                                            /* '1' */
                                            /* '2' */
                                            /* '3' */
     {0x7D, UIE_FSC_BIAS + 56},             /* 61h */
     {0x5F, UIE_FSC_BIAS + 57},             /* 62h */
     {0xAB, UIE_FSC_BIAS + 58},             /* 63h */

     {0x3F, UIE_FSC_BIAS + 60},             /* 64h */
     {0x5C, UIE_FSC_BIAS + 61},             /* 65h */
     {0x00, FSC_P2},                        /* 66h */
                                            /* '0' */
     
     {0x00, FSC_NO_FUNC},                   /* 67h */
     {0x00, FSC_P5},                        /* 68h */
     {0x00, FSC_P1},                        /* 69h */
     {0xBE, UIE_FSC_BIAS + 66},             /* 6Ah */
     {0x95, UIE_FSC_BIAS + 67},             /* 6Bh */
     
     {0x4C, UIE_FSC_BIAS + 28},             /* 6Ch */
     {0x2F, UIE_FSC_BIAS + 38},             /* 6Dh */
     {0x23, UIE_FSC_BIAS + 45},             /* 6Eh */
     {0x3A, UIE_FSC_BIAS + 52},             /* 6Fh */
     {0x22, UIE_FSC_BIAS + 59},             /* 70h */
     {0x96, UIE_FSC_BIAS + 67},             /* 71h */

     {0x00, FSC_RECEIPT_FEED},              /* 72h */
     {0x00, FSC_JOURNAL_FEED},              /* 73h */
     {0x21, UIE_FSC_BIAS + 2},              /* 74h */
     {0x40, UIE_FSC_BIAS + 3},              /* 75h */
     {0x24, UIE_FSC_BIAS + 4},              /* 76h */
     {0x25, UIE_FSC_BIAS + 5},              /* 77h */
     {0x5E, UIE_FSC_BIAS + 6},              /* 78h */
     {0x26, UIE_FSC_BIAS + 7},              /* 79h */
     {0x2A, UIE_FSC_BIAS + 8},              /* 7Ah */
     {0x3D, UIE_FSC_BIAS + 9},              /* 7Bh */

     {0x51, UIE_FSC_BIAS + 10},             /* 7Ch */
     {0x57, UIE_FSC_BIAS + 11},             /* 7Dh */
     {0x45, UIE_FSC_BIAS + 12},             /* 7Eh */
     {0x52, UIE_FSC_BIAS + 13},             /* 7Fh */
     {0x54, UIE_FSC_BIAS + 14},             /* 80h */
     {0x59, UIE_FSC_BIAS + 15},             /* 81h */
     {0x55, UIE_FSC_BIAS + 16},             /* 82h */
     {0x49, UIE_FSC_BIAS + 17},             /* 83h */
     {0x4F, UIE_FSC_BIAS + 18},             /* 84h */
     {0x50, UIE_FSC_BIAS + 19}};            /* 85h */


CVTTBL FARCONST CvtAlphC2[] =               /* Pif */
    {{0x61, UIE_FSC_BIAS + 20},             /* 40h */
     {0x73, UIE_FSC_BIAS + 21},             /* 41h */
     {0x64, UIE_FSC_BIAS + 22},             /* 42h */
     {0x00, FSC_CLEAR},                     /* 43h */
     {0x66, UIE_FSC_BIAS + 23},             /* 44h */
     {0x67, UIE_FSC_BIAS + 24},             /* 45h */
     {0x68, UIE_FSC_BIAS + 25},             /* 46h */
     {0x6A, UIE_FSC_BIAS + 26},             /* 47h */
     {0x6B, UIE_FSC_BIAS + 27},             /* 48h */

     {0x7A, UIE_FSC_BIAS + 29},             /* 49h */
     {0x78, UIE_FSC_BIAS + 30},             /* 4Ah */
     {0x63, UIE_FSC_BIAS + 31},             /* 4Bh */
     {0x20, UIE_FSC_BIAS + 32},             /* 4Ch */
     {0x00, FSC_BS},                        /* 4Dh */
     {0x76, UIE_FSC_BIAS + 34},             /* 4Eh */
     {0x62, UIE_FSC_BIAS + 35},             /* 4Fh */
     {0x6E, UIE_FSC_BIAS + 36},             /* 50h */
     {0x6D, UIE_FSC_BIAS + 37},             /* 51h */

     {0x83, UIE_FSC_BIAS + 39},             /* 52h */
     {0x84, UIE_FSC_BIAS + 40},             /* 53h */
     {0x00, FSC_P3},                        /* 54h */
                                            /* '7' */
                                            /* '8' */
                                            /* '9' */
     {0x00, FSC_P4},                        /* 55h */
     {0x94, UIE_FSC_BIAS + 43},             /* 56h */
     {0x81, UIE_FSC_BIAS + 44},             /* 57h */

     {0x85, UIE_FSC_BIAS + 46},             /* 58h */
     {0x87, UIE_FSC_BIAS + 47},             /* 59h */
     {0x8F, UIE_FSC_BIAS + 48},             /* 5Ah */
                                            /* '4' */
                                            /* '5' */
                                            /* '6' */
     {0xF1, UIE_FSC_BIAS + 49},             /* 5Bh */
     {0xF6, UIE_FSC_BIAS + 50},             /* 5Ch */
     {0x90, UIE_FSC_BIAS + 51},             /* 5Dh */

     {0x60, UIE_FSC_BIAS + 53},             /* 5Eh */
     {0x7E, UIE_FSC_BIAS + 54},             /* 5Fh */
     {0x7F, UIE_FSC_BIAS + 55},             /* 60h */
                                            /* '1' */
                                            /* '2' */
                                            /* '3' */
     {0x86, UIE_FSC_BIAS + 56},             /* 61h */
     {0x8C, UIE_FSC_BIAS + 57},             /* 62h */
     {0x8D, UIE_FSC_BIAS + 58},             /* 63h */

     {0xEB, UIE_FSC_BIAS + 60},             /* 64h */
     {0x80, UIE_FSC_BIAS + 61},             /* 65h */
     {0x00, FSC_P2},                        /* 66h */
                                            /* '0' */
     
     {0x00, FSC_NO_FUNC},                   /* 67h */
     {0x00, FSC_P5},                        /* 68h */
     {0x00, FSC_P1},                        /* 69h */
     {0x91, UIE_FSC_BIAS + 66},             /* 6Ah */
     {0x93, UIE_FSC_BIAS + 67},             /* 6Bh */
     
     {0x6C, UIE_FSC_BIAS + 28},             /* 6Ch */
     {0x82, UIE_FSC_BIAS + 38},             /* 6Dh */
     {0x97, UIE_FSC_BIAS + 45},             /* 6Eh */
     {0x92, UIE_FSC_BIAS + 52},             /* 6Fh */
     {0x8E, UIE_FSC_BIAS + 59},             /* 70h */
     {0x94, UIE_FSC_BIAS + 68},             /* 71h */

     {0x00, FSC_RECEIPT_FEED},              /* 72h */
     {0x00, FSC_JOURNAL_FEED},              /* 73h */
     {0xA1, UIE_FSC_BIAS + 2},              /* 74h */
     {0x88, UIE_FSC_BIAS + 3},              /* 75h */
     {0x8A, UIE_FSC_BIAS + 4},              /* 76h */
     {0x89, UIE_FSC_BIAS + 5},              /* 77h */
     {0xA2, UIE_FSC_BIAS + 6},              /* 78h */
     {0x7C, UIE_FSC_BIAS + 7},              /* 79h */
     {0x9B, UIE_FSC_BIAS + 8},              /* 7Ah */
     {0x8B, UIE_FSC_BIAS + 9},              /* 7Bh */

     {0x71, UIE_FSC_BIAS + 0},              /* 7Ch */
     {0x77, UIE_FSC_BIAS + 1},              /* 7Dh */
     {0x65, UIE_FSC_BIAS + 2},              /* 7Eh */
     {0x72, UIE_FSC_BIAS + 3},              /* 7Fh */
     {0x74, UIE_FSC_BIAS + 4},              /* 80h */
     {0x79, UIE_FSC_BIAS + 5},              /* 81h */
     {0x75, UIE_FSC_BIAS + 6},              /* 82h */
     {0x69, UIE_FSC_BIAS + 7},              /* 83h */
     {0x6F, UIE_FSC_BIAS + 8},              /* 84h */
     {0x70, UIE_FSC_BIAS + 9}};             /* 85h */


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
