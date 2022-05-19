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
* Title       : Thermal Print PLU Setting Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSPLUT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function forms PLU print format.
*
*           The provided function names are as follows: 
* 
*               PrtThrmSupPLU()     : form PLU Maintenance print format
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-17-93: 01.00.12 : J.IKEDA   : Initial                                    
* Mar-07-95:   .  .   : Waki      : Add T-No. G-No. P-Priority
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

/**
==============================================================================
;                      I N C L U D E     F I L E s                         
=============================================================================
**/
#include	<tchar.h>
#include <string.h>
#include <stdlib.h>

#include <ecr.h>
/* #include <pif.h> */
#include <rfl.h>
#include <paraequ.h> 
#include <para.h>
#include <csop.h>
/* #include <cswai.h> */
#include <maint.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <csttl.h>
#include <report.h>
#include <pmg.h>

#include "prtcom.h"
/* #include "prtrin.h" */
#include "prtsin.h"
#include "prrcolm_.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtThrmSupPLU( PARAPLU *pData )
*               
*     Input:    *pData      : pointer to structure for PARAPLU
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms PLU print format.
*
*                    : PLU PRICE CHANGE
*                    : PLU ADDITION / DELETION
*                    : PLU FILE READ REPORT
*                    : PLU MNEMONIC CHANGE
*
*===========================================================================
*/

VOID  PrtThrmSupPLU( PARAPLU *pData )
{

    /* define thermal print format */

    static const TCHAR FARCONST auchPrtThrmSupPLU1[] = _T(" %s\t%-20s");
    static const TCHAR FARCONST auchPrtThrmSupPLU2[] = _T(" %04u  %01u  %2d  %4s  %4s  %4s  %4s  %4s");
    static const TCHAR FARCONST auchPrtThrmSupPLU3[] = _T("\t%4s     %s  %4s  %4s  %4s");
    static const TCHAR FARCONST auchPrtThrmSupPLU4[] = _T("\t@ 02d/%8l$");   
    static const TCHAR FARCONST auchPrtThrmSupPLU5[] = _T("\t@ %8l$   @ %8l$");   
    static const TCHAR FARCONST auchPrtThrmSupPLU6[] = _T("   %3u   %3u   %3u   %3u  %4s %4s");
    static const TCHAR FARCONST auchPrtThrmSupPLU7[] = _T("   %4u   %1d   %3d");

    /* define EJ print format */

    static const TCHAR FARCONST auchPrtSupPLU1[] = _T("%s\t%-s");
    static const TCHAR FARCONST auchPrtSupPLU11[] = _T("%s\n\t%-s");
    static const TCHAR FARCONST auchPrtSupPLU2[] = _T("%04d %d %2d %4s %4s");
    static const TCHAR FARCONST auchPrtSupPLU3[] = _T("%4s %4s %4s");
    static const TCHAR FARCONST auchPrtSupPLU4[] = _T("%4s %s %4s %4s %4s");
    static const TCHAR FARCONST auchPrtSupPLU5[] = _T("\t@ %02d %8l$");
    static const TCHAR FARCONST auchPrtSupPLU6[] = _T("\t@ %8l$  @ %8l$");
    static const TCHAR FARCONST auchPrtSupPLU7[] = _T(" %2d  %2d  %2d  %3d %4s");
    static const TCHAR FARCONST auchPrtSupPLU8[] = _T(" %4s %4u  %1d %3d");

    DCURRENCY  ulPrice1;
    DCURRENCY  ulPrice2;
    DCURRENCY  ulPrice3;
    DCURRENCY  ulPrice4;
    DCURRENCY  ulPrice5;

    /* initialize buffer */

    TCHAR  aszAddress3[5];
    TCHAR  aszAddress4[5];
    TCHAR  aszAddress5[5];
    TCHAR  aszAddress6[5];
    TCHAR  aszAddress7[5];
    TCHAR  aszAddress8[5];
    TCHAR  aszAddress9[5] = { 0, 0, 0, 0, 0 };
    TCHAR  aszAddress10[5];
    TCHAR  aszAddress11[5];
    TCHAR  aszAddress12[5];
    TCHAR  aszAddress18[5];
    TCHAR  aszAddress19[5];
    TCHAR  aszPLUNo[PLU_MAX_DIGIT+1];

	memset(aszPLUNo, 0x00, sizeof(aszPLUNo));
    /* convert PLU status data to binary ASCII data */

    PrtSupItoa(( UCHAR)(pData->auchPLUStatus[0] & 0x0F), aszAddress3);
    PrtSupItoa(( UCHAR)((pData->auchPLUStatus[0] & 0xF0) >> 4), aszAddress4);
    PrtSupItoa(( UCHAR)(pData->auchPLUStatus[1] & 0x0F), aszAddress5);
    PrtSupItoa(( UCHAR)((pData->auchPLUStatus[1] & 0xF0) >> 4), aszAddress6);
    PrtSupItoa(( UCHAR)(pData->auchPLUStatus[2] & 0x0F), aszAddress7);
    PrtSupItoa(( UCHAR)((pData->auchPLUStatus[2] & 0xF0) >> 4), aszAddress8);
    _itot(pData->auchPLUStatus[3] & 0x0F, aszAddress9, 10);
    PrtSupItoa(( UCHAR)((pData->auchPLUStatus[3] & 0xF0) >> 4), aszAddress10);
    PrtSupItoa(( UCHAR)(pData->auchPLUStatus[4] & 0x0F), aszAddress11);
    PrtSupItoa(( UCHAR)((pData->auchPLUStatus[4] & 0xF0) >> 4), aszAddress12);
    PrtSupItoa(( UCHAR)(pData->auchPLUStatus[5] & 0x0F), aszAddress18);
    PrtSupItoa(( UCHAR)((pData->auchPLUStatus[5] & 0xF0) >> 4), aszAddress19);
    RflConvertPLU(aszPLUNo, pData->auchPLUNumber);

    /* check print control */

    if (pData->usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */

        /* print PLU No./MNEMONIC */
        
        PrtPrintf(PMG_PRT_RECEIPT,                      /* printer type */
                  auchPrtThrmSupPLU1,                   /* format */
                  aszPLUNo,                             /* PLU number */
                  pData->aszMnemonics);                 /* mnemonic */
        
        /* print DEPT No./PLU TYPE/REPORT CODE/STATUS */
        
        PrtPrintf(PMG_PRT_RECEIPT,                      /* printer type */
                  auchPrtThrmSupPLU2,                   /* format */
                  ( USHORT)(pData->usDEPTNumber),      /* dept number */
                  ( USHORT)(pData->uchPLUType),         /* PLU type */
                  ( USHORT)(pData->uchReportCode),      /* report code */
                  aszAddress3,                          /* status of address 3 */
                  aszAddress4,                          /* status of address 4 */
                  aszAddress5,                          /* status of address 5 */
                  aszAddress6,                          /* status of address 6 */
                  aszAddress7);                         /* status of address 7 */
                                                    
        /* print STATUS */
        
        PrtPrintf(PMG_PRT_RECEIPT,                      /* printer type */
                  auchPrtThrmSupPLU3,                   /* format */
                  aszAddress8,                          /* status of address 8 */
                  aszAddress9,                          /* status of address 9 */
                  aszAddress10,                         /* status of address 10 */
                  aszAddress11,                         /* status of address 11 */
                  aszAddress12);                        /* status of address 12 */
        
        /* print Table No. Gruop No. Print Priority */
        
        PrtPrintf(PMG_PRT_RECEIPT,                      /* printer type */
                  auchPrtThrmSupPLU6,                   /* format */
                  ( USHORT)(pData->uchTableNumber),     /* Table number */
                  ( USHORT)(pData->uchGroupNumber),     /* Group number */
                  ( USHORT)(pData->uchPrintPriority),   /* Print Priority */
                  ( USHORT)(pData->uchPPICode),         /* PPI Code */
                  aszAddress18,                         /* status of address 18 */
                  aszAddress19);                        /* status of address 19 */
                  
        /* print Table No. Gruop No. Print Priority */
        
        PrtPrintf(PMG_PRT_RECEIPT,                      /* printer type */
                  auchPrtThrmSupPLU7,                   /* format */
                  ( USHORT)(pData->usLinkNumber),       /* Link No. */
                  ( USHORT)(pData->uchSalesRestriction),  /* Sales Restriction */
                  ( USHORT)(pData->usFamilyCode));      /* Family Code */
        
        /* check minor class and distingush PLU type */
        
        switch (pData->uchMinorClass) {
        case CLASS_PARAPLU_NONADJ:

            /* convert 3byte data to ULONG data */
            ulPrice1 = RflFunc3bto4b(pData->auchPresetPrice[0]);

            /* print PRICE1 */
            PrtPrintf(PMG_PRT_RECEIPT, auchPrtSupPLU5, (USHORT)(pData->uchPriceMultiple), ulPrice1);  /* price multiple  Preset price 1 */
            break;

        case CLASS_PARAPLU_ONEADJ:

            /* convert 3byte data to ULONG data */
            ulPrice1 = RflFunc3bto4b(pData->auchPresetPrice[0]);
            ulPrice2 = RflFunc3bto4b(pData->auchPresetPrice[1]);
            ulPrice3 = RflFunc3bto4b(pData->auchPresetPrice[2]);
            ulPrice4 = RflFunc3bto4b(pData->auchPresetPrice[3]);
            ulPrice5 = RflFunc3bto4b(pData->auchPresetPrice[4]);

            /* print PRICE 1 */
            PrtPrintf(PMG_PRT_RECEIPT, auchPrtSupPLU5, (USHORT)(pData->uchPriceMultiple), ulPrice1);  /* price multiple  Preset price 1 */

            /* print PRICE 2/PRICE 3 */
            PrtPrintf(PMG_PRT_RECEIPT, auchPrtSupPLU6, ulPrice2, ulPrice3);  /* Preset price 2  Preset price 3 */

            /* print PRICE 4/PRICE 5 */
            PrtPrintf(PMG_PRT_RECEIPT, auchPrtSupPLU6, ulPrice4, ulPrice5); /* Preset price 4  Preset price 5 */
            break;

        default:
            break;
        }
    }

    if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */

        /* print PLU No./MNEMONIC */
        
        /* 2172 */
        if ((_tcslen(pData->aszMnemonics) + _tcslen(aszPLUNo)) > (PRT_EJCOLUMN -1)) {

            PrtPrintf(PMG_PRT_JOURNAL,                      /* printer type */
                      auchPrtSupPLU11,                       /* format */
                      aszPLUNo,                             /* PLU number */
                      pData->aszMnemonics);                 /* mnemonic */
                      
        } else {
            
            PrtPrintf(PMG_PRT_JOURNAL,                      /* printer type */
                      auchPrtSupPLU1,                       /* format */
                      aszPLUNo,                             /* PLU number */
                      pData->aszMnemonics);                 /* mnemonic */
        }
        /* print DEPT No./PLU TYPE/REPORT CODE/STATUS */
        
        PrtPrintf(PMG_PRT_JOURNAL,                      /* printer type */
                  auchPrtSupPLU2,                       /* format */
                  ( USHORT)(pData->usDEPTNumber),      /* dept number */
                  ( USHORT)(pData->uchPLUType),         /* PLU type */
                  ( USHORT)(pData->uchReportCode),      /* report code */
                  aszAddress3,                          /* status of address 3 */
                  aszAddress4);                         /* status of address 4 */
                                                    
        /* print STATUS */
        
        PrtPrintf(PMG_PRT_JOURNAL,                      /* printer type */
                  auchPrtSupPLU3,                       /* format */
                  aszAddress5,                          /* status of address 5 */
                  aszAddress6,                          /* status of address 6 */
                  aszAddress7);                         /* status of address 7 */
        
        /* print STATUS */
        
        PrtPrintf(PMG_PRT_JOURNAL,                      /* printer type */
                  auchPrtSupPLU4,                       /* format */
                  aszAddress8,                          /* status of address 8 */
                  aszAddress9,                          /* status of address 9 */
                  aszAddress10,                         /* status of address 10 */
                  aszAddress11,                         /* status of address 11 */
                  aszAddress12);                        /* status of address 12 */
        
        /* print Table No. Gruop No. Print Priority */
        
        PrtPrintf(PMG_PRT_JOURNAL,                      /* printer type */
                  auchPrtSupPLU7,                       /* format */
                  ( USHORT)(pData->uchTableNumber),     /* Table number */
                  ( USHORT)(pData->uchGroupNumber),     /* Group number */
                  ( USHORT)(pData->uchPrintPriority),   /* Print Priority */
                  ( USHORT)(pData->uchPPICode),         /* PPI Code */
                  aszAddress18);                        /* status of address 18 */
        
        /* print status, link no, sales restriction */
        
        PrtPrintf(PMG_PRT_JOURNAL,                      /* printer type */
                  auchPrtSupPLU8,                       /* format */
                  aszAddress19,                        /* status of address 19 */
                  ( USHORT)(pData->usLinkNumber),       /* Link No. */
                  ( USHORT)(pData->uchSalesRestriction), /* Sales Restriction */
                  ( USHORT)(pData->usFamilyCode));      /* Family Code */
        
        /* check minor class and distingush PLU type */

        switch (pData->uchMinorClass) {
        case CLASS_PARAPLU_NONADJ:

            /* convert 3byte data to ULONG data */
            ulPrice1 = RflFunc3bto4b(pData->auchPresetPrice[0]);

            /* print PRICE 1 */
            PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupPLU5, (USHORT)(pData->uchPriceMultiple), ulPrice1); /* price multiple  Preset price 1 */
            break;

        case CLASS_PARAPLU_ONEADJ:
 
            /* convert 3byte data to ULONG data */
            ulPrice1 = RflFunc3bto4b(pData->auchPresetPrice[0]);
            ulPrice2 = RflFunc3bto4b(pData->auchPresetPrice[1]);
            ulPrice3 = RflFunc3bto4b(pData->auchPresetPrice[2]);
            ulPrice4 = RflFunc3bto4b(pData->auchPresetPrice[3]);
            ulPrice5 = RflFunc3bto4b(pData->auchPresetPrice[4]);

            /* pirnt PRICE 1 */
            PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupPLU5, (USHORT)(pData->uchPriceMultiple), ulPrice1);                         /* price multiple  Preset price 1 */

            /* print PRICE 2/PRICE 3 */
            PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupPLU6, ulPrice2, ulPrice3); /* Preset price 2  Preset price 3 */

            /* print PRICE 4/PRICE5 */
            PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupPLU6, ulPrice4, ulPrice5);                         /* Preset price 4  Preset price 5 */
            break;

        default:
            break;
        }
    }
}
/***** End of Source *****/

