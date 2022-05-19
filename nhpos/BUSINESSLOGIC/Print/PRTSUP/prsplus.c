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
* Program Name: PRSPLUS.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function forms PLU print format.
*
*           The provided function names are as follows: 
* 
*               PrtSupPLU_s()     : form PLU print format
*       
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-13-92: 00.00.01 : K.You     : initial                                   
* Nov-26-92: 01.00.00 : K.You     : Chg Function Name                                   
* Dec-03-92: 01.00.00 : K.You     : Del 24 Printer Handling                  
* Feb-06-95: 03.00.00 : h.ishida  : Adds uchLinkNumber, uchClassNumber
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

/*
*===========================================================================
** Synopsis:    VOID  PrtSupPLU_s( PARAPLU *pData )
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

VOID  PrtSupPLU_s( PARAPLU *pData )
{
    USHORT     usPrtType;
    DCURRENCY  ulPrice1;
    DCURRENCY  ulPrice2;
    DCURRENCY  ulPrice3;
    DCURRENCY  ulPrice4;
    DCURRENCY  ulPrice5;

    /* Define Format Type */
    static const TCHAR FARCONST auchPrtSupPLU1[] = _T("%s\t%-20s");
    static const TCHAR FARCONST auchPrtSupPLU2[] = _T("%04d %d %2d %4s %4s");
    static const TCHAR FARCONST auchPrtSupPLU3[] = _T("%4s %4s %4s");
    static const TCHAR FARCONST auchPrtSupPLU4[] = _T("%4s %s %4s %4s %4s");
    static const TCHAR FARCONST auchPrtSupPLU5[] = _T("\t@ 02d/%8l$");
    static const TCHAR FARCONST auchPrtSupPLU6[] = _T("@ %8l$ @ %8l$");
    static const TCHAR FARCONST auchPrtSupPLU7[] = _T(" %2d  %2d  %2d");
    static const TCHAR FARCONST auchPrtSupPLU8[] = _T(" %4s %4u  %1d");

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
    PrtSupItoa(( UCHAR)((pData->auchPLUStatus[6] & 0xF0) >> 4), aszAddress19);
    RflConvertPLU(aszPLUNo, pData->auchPLUNumber);

    /* 1st Line */

    PmgPrintf(usPrtType,                        /* printer type */
              auchPrtSupPLU1,                   /* format */
              aszPLUNo,                         /* plu number */
              pData->aszMnemonics);             /* mnemonic */

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

    PmgPrintf(usPrtType,                            /* printer type */
              auchPrtSupPLU7,                       /* format */
              ( USHORT)(pData->uchTableNumber),     /* Table number */
              ( USHORT)(pData->uchGroupNumber),     /* Group number */
              ( USHORT)(pData->uchPrintPriority));  /* Print Priority */

    /* 6th Line */
    
    PmgPrintf(usPrtType,                        /* printer type */
              auchPrtSupPLU8,                       /* format */
              aszAddress19,                        /* status of address 19 */
              ( USHORT)(pData->usLinkNumber),       /* Link No. */
              ( USHORT)(pData->uchSalesRestriction)); /* Sales Restriction */
              
    /* Check Minor Class and Distingush PLU Type */

    switch (pData->uchMinorClass) {
    case CLASS_PARAPLU_NONADJ:
        /* RptRegFinFeed(RPT_DEFALTFEED);                           Feed 1 Line */
        ulPrice1 = RflFunc3bto4b(pData->auchPresetPrice[0]);

        /* 6th Line */
        PmgPrintf(usPrtType, auchPrtSupPLU5, (USHORT)(pData->uchPriceMultiple), ulPrice1);                         /* price multiple  Preset price 1 */
        break;

    case CLASS_PARAPLU_ONEADJ:
        /* RptRegFinFeed(RPT_DEFALTFEED);                         Feed 1 Line */
        ulPrice1 = RflFunc3bto4b(pData->auchPresetPrice[0]);
        ulPrice2 = RflFunc3bto4b(pData->auchPresetPrice[1]);
        ulPrice3 = RflFunc3bto4b(pData->auchPresetPrice[2]);
        ulPrice4 = RflFunc3bto4b(pData->auchPresetPrice[3]);
        ulPrice5 = RflFunc3bto4b(pData->auchPresetPrice[4]);

        /* 6th line */
        PmgPrintf(usPrtType, auchPrtSupPLU5, (USHORT)(pData->uchPriceMultiple), ulPrice1);                         /* price multiple  Preset price 1 */

        /* 7th line */
        PmgPrintf(usPrtType, auchPrtSupPLU6, ulPrice2, ulPrice3);   /* Preset price 2  Preset price 3 */

        /* 8th line */
        PmgPrintf(usPrtType, auchPrtSupPLU6, ulPrice4, ulPrice5);   /* Preset price 4  Preset price 5 */
        break;

    default:
        break;
    }
}


