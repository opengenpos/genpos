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
* Title       : Print PLU Setting Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSPLU_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function forms PLU print format.
*
*           The provided function names are as follows: 
* 
*               PrtSupPLU()     : form PLU print format
*               PrtSupChkType() : Check and Return Printer Type
*       
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Nov-30-92: 01.00.00 : K.You     : Initial                                   
* Dec-22-92: 01.00.06 : K.You     : Adds break for case CLASS_PARAPLU_ONEADJ 
* Feb-06-95: 03.00.00 : h.ishida  : Adds uchLinkNumber, uchClassNumber
* Nov-22-95: 03.01.00 : M.Ozawa   : Adds uchPPICode, status code
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
#include <maint.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>
#include "prtsin.h"
#include "prrcolm_.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtSupPLU( PARAPLU *pData )
*               
*     Input:    *pData      : Pointer to Structure for PARAPLU
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms PLU print format.
*===========================================================================
*/

VOID  PrtSupPLU( PARAPLU *pData )
{

    USHORT     usPrtType;
    DCURRENCY  ulPrice1;
    DCURRENCY  ulPrice2;
    DCURRENCY  ulPrice3;
    DCURRENCY  ulPrice4;
    DCURRENCY  ulPrice5;



    /* Define Format Type */

    static const TCHAR FARCONST auchPrtSupPLU1[] = _T("%s\t%-s");
    static const TCHAR FARCONST auchPrtSupPLU11[] = _T("%s\n\t%-s");
    static const TCHAR FARCONST auchPrtSupPLU2[] = _T("%04d %d %2d %4s %4s");
    static const TCHAR FARCONST auchPrtSupPLU3[] = _T("%4s %4s %4s");
    static const TCHAR FARCONST auchPrtSupPLU4[] = _T("%4s %s %4s %4s %4s");
    static const TCHAR FARCONST auchPrtSupPLU5[] = _T("\t@ %02d/%8l$");
    static const TCHAR FARCONST auchPrtSupPLU6[] = _T("\t@ %8l$  @ %8l$");
    static const TCHAR FARCONST auchPrtSupPLU7[] = _T(" %2d  %2d  %2d  %3d %4s");
    static const TCHAR FARCONST auchPrtSupPLU8[] = _T(" %4s %4u  %1d %3d");

    /* Initialize Buffer */

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

    /* Check Print Control */
    
    usPrtType = PrtSupChkType(pData->usPrintControl);

    /* Convert PLU Status Data to Binary ASCII Data */
 
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

    /* 1st Line */

    /* 2172 */
    if ((_tcslen(pData->aszMnemonics) + _tcslen(aszPLUNo)) > (PRT_RJCOLUMN -1)) {
        
        PmgPrintf(usPrtType,                        /* printer type */
                  auchPrtSupPLU11,                   /* format */
                  aszPLUNo,                         /* PLU number */
                  pData->aszMnemonics);             /* mnemonic */
                  
    } else {
        
        PmgPrintf(usPrtType,                        /* printer type */
                  auchPrtSupPLU1,                   /* format */
                  aszPLUNo,                         /* PLU number */
                  pData->aszMnemonics);             /* mnemonic */
    }

    /* 2nd Line */

    PmgPrintf(usPrtType,                        /* printer type */
              auchPrtSupPLU2,                   /* format */
              ( USHORT)(pData->usDEPTNumber),  /* dept number */
              ( USHORT)(pData->uchPLUType),     /* PLU type */
              ( USHORT)(pData->uchReportCode),  /* report code */
              aszAddress3,                      /* Status of Address 3 */
              aszAddress4);                     /* Status of Address 4 */
                                                
    /* 3rd Line */

    PmgPrintf(usPrtType,                        /* printer type */
              auchPrtSupPLU3,                   /* format */
              aszAddress5,                      /* Status of Address 5 */
              aszAddress6,                      /* Status of Address 6 */
              aszAddress7);                     /* Status of Address 7 */

    /* 4th Line */

    PmgPrintf(usPrtType,                        /* printer type */
              auchPrtSupPLU4,                   /* format */
              aszAddress8,                      /* Status of Address 8 */
              aszAddress9,                      /* Status of Address 9 */
              aszAddress10,                     /* Status of Address 10 */
              aszAddress11,                     /* Status of Address 11 */
              aszAddress12);                    /* Status of Address 12 */

    /* 5th Line */

    PmgPrintf(usPrtType,                        /* printer type */
              auchPrtSupPLU7,                   /* format */
              ( USHORT)(pData->uchTableNumber),     /* Table number */
              ( USHORT)(pData->uchGroupNumber),     /* Group number */
              ( USHORT)(pData->uchPrintPriority),   /* Print Priority */
              ( USHORT)(pData->uchPPICode),         /* PPI Code */
              aszAddress18);                    /* Status of Address 18 */

    /* 6th Line */
    
    PmgPrintf(usPrtType,                        /* printer type */
              auchPrtSupPLU8,                       /* format */
              aszAddress19,                        /* status of address 19 */
              ( USHORT)(pData->usLinkNumber),       /* Link No. */
              ( USHORT)(pData->uchSalesRestriction),  /* Sales Restriction */
              ( USHORT)(pData->usFamilyCode));      /* Sales Restriction */
              
    /* Check Minor Class and Distingush PLU Type */

    switch (pData->uchMinorClass) {
    case CLASS_PARAPLU_NONADJ:
        /* RptRegFinFeed(RPT_DEFALTFEED);                           Feed 1 Line */
        ulPrice1 = RflFunc3bto4b(pData->auchPresetPrice[0]);

        /* 6th Line */

        PmgPrintf(usPrtType,                         /* printer type */
                  auchPrtSupPLU5,                    /* format */                 
                  (USHORT)(pData->uchPriceMultiple),/* price multiple */
                  ulPrice1);                         /* Preset price 1 */
        break;

    case CLASS_PARAPLU_ONEADJ:
        /* RptRegFinFeed(RPT_DEFALTFEED);                         Feed 1 Line */
        ulPrice1 = RflFunc3bto4b(pData->auchPresetPrice[0]);
        ulPrice2 = RflFunc3bto4b(pData->auchPresetPrice[1]);
        ulPrice3 = RflFunc3bto4b(pData->auchPresetPrice[2]);
        ulPrice4 = RflFunc3bto4b(pData->auchPresetPrice[3]);
        ulPrice5 = RflFunc3bto4b(pData->auchPresetPrice[4]);

        /* 6th line */
        PmgPrintf(usPrtType,                         /* printer type */
                  auchPrtSupPLU5,                    /* format */                  
                  (USHORT)(pData->uchPriceMultiple),/* price multiple */
                  ulPrice1);                         /* Preset price 1 */

        /* 7th line */

        PmgPrintf(usPrtType,                         /* printer type */
                  auchPrtSupPLU6,                    /* format */                 
                  ulPrice2,                          /* Preset price 2 */
                  ulPrice3);                         /* Preset price 3 */

        /* 8th line */

        PmgPrintf(usPrtType,                         /* printer type */
                  auchPrtSupPLU6,                    /* format */                 
                  ulPrice4,                          /* Preset price 4 */
                  ulPrice5);                         /* Preset price 5 */
        break;

    default:
        break;
    }
}


/*
*===========================================================================
** Synopsis:    USHORT  PrtSupChkType( USHORT usPrtintControl )
*               
*     Input:    usPrintControl  : Printer Type
*    Output:    Nothing          
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: Check and Return Printer Type.
*===========================================================================
*/

USHORT  PrtSupChkType( USHORT usPrintControl )
{

    USHORT  usPrtType = 0;


    /* Distinguish Printer Type */

    switch(usPrintControl) {             
    case PRT_RECEIPT:
        usPrtType = PMG_PRT_RECEIPT;
        break;

    case PRT_JOURNAL:
        usPrtType = PMG_PRT_JOURNAL;
        break;

    case PRT_RECEIPT | PRT_JOURNAL:
        usPrtType = PMG_PRT_RCT_JNL;
        break;

    default:
        break;
    }
    return(usPrtType);                          /* Return Printer Type */
}


/*
*===========================================================================
** Synopsis:    VOID  PrtSupItoa( UCHAR uchData, UCHAR auchString[] )
*               
*     Input:    uchData     : Binary Data to be Converted
*    Output:    auchStirng  : Saved Area for Converted Data 
*
** Return:      Nothing
*
** Description: Convert 4 Bit Binary Data to ASCII.
*===========================================================================
*/

VOID  PrtSupItoa( UCHAR uchData, TCHAR auchString[] )
{

    USHORT  i;
    USHORT  j;

    for (i = 0, j = 3; i < 4; i++, j--) {
        auchString[j]  = ( TCHAR)(((uchData >> i) & 0x01) + _T('0'));
    }
    auchString[i] = '\0';
}
