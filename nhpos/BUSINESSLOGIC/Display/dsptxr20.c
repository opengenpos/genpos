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
*==========================================================================
* Title       : Set Tax Rate Module( SUPER & PROGRAM MODE )                       
* Category    : Display-2x20, NCR 2170 US Hospitality Application Program        
* Program Name: DSPTXR20.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*               
*               DispSupTaxRate20()      : Display Tax Rate Rate
*                                                    
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-22-92: 00.00.01 : K.You     : initial                                   
*          :          :           :                                    
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
#include <ecr.h>
#include "uie.h"
/* #include <pif.h> */
/* #include <log.h> */
#include "paraequ.h"
#include "para.h"
#include "maint.h"
/* #include <appllog.h> */
#include "display.h"
#include "disp20.h"

/*
*===========================================================================
** Synopsis:    VOID DispSupTaxRate20( PARATAXRATETBL *pData )
*               
*     Input:    *pData          : Pointer to Structure for PARATAXRATETBL
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Displays Tax Rate Data
*===========================================================================
*/

VOID DispSupTaxRate20( PARATAXRATETBL *pData )
{
    /* Define Display Format */ 
    static const TCHAR FARCONST auchDispSupTaxRate20A[] = _T("%3u  %3u\t%.4l$\n%-18s\t%u");
    static const TCHAR FARCONST auchDispSupTaxRate20B[] = _T("%3u  %3u\t%5lu\n%-18s\t%u");
    static const TCHAR FARCONST auchDispSupTaxRate20C[] = _T("%3u  %3u\t%10s\n%-18s\t%u");

    TCHAR   aszDspNull[1] = {_T('\0')};
 
	DispSupSaveData.TaxRateTbl = *pData;

    /* Distinguish Minor Class */
    switch (pData->uchMinorClass) {
    case CLASS_PARATAXRATETBL_RATE:

        /* Display ForeGround */
        UiePrintf(UIE_OPER,                     /* Operator Display */
                  0,                            /* Row */
                  0,                            /* Column */
                  auchDispSupTaxRate20A,        /* Format */
                  AC_TAX_RATE,                  /* A/C Number */
                  ( USHORT)pData->uchAddress,   /* Address */
                  (DCURRENCY)pData->ulTaxRate,  /* Tax Rate Data */
                  pData->aszMnemonics,          /* Lead Thru Data */
                  uchMaintMenuPage);            /* Page Number */
        break;

    case CLASS_PARATAXRATETBL_LIMIT:
        /* Display ForeGround */
        UiePrintf(UIE_OPER,                     /* Operator Display */
                  0,                            /* Row */
                  0,                            /* Column */
                  auchDispSupTaxRate20B,        /* Format */
                  AC_TAX_RATE,                  /* A/C Number */
                  ( USHORT)pData->uchAddress,   /* Address */
                  pData->ulTaxRate,             /* Tax Exempt Limit Data */
                  pData->aszMnemonics,          /* Lead Thru Data */
                  uchMaintMenuPage);            /* Page Number */
        break;

    default:
/*        PifAbort(MODULE_DISPSUP_ID, FAULT_INVALID_DATA); */
        break;
    }

    /* Display BackGround */
    UieEchoBackGround(UIE_KEEP,                     /* Descriptor Control */
                      UIE_ATTR_NORMAL,              /* Attribute */
                      auchDispSupTaxRate20C,        /* Format */
                      AC_TAX_RATE,                  /* A/C Number */
                      ( USHORT)pData->uchAddress,   /* Address */
                      aszDspNull,                   /* Null */
                      pData->aszMnemonics,          /* Lead Thru Data */
                      uchMaintMenuPage);            /* Page Number */
}


/*
*===========================================================================
** Synopsis:    VOID DispSupMiscPara20( PARAMISCPARA *pData )
*
*     Input:    *pData          : Pointer to Structure for PARAMISCPARA
*    Output:    Nothing
*
** Return:      Nothing
*
** Description: This Function Displays Misc. Parameter Data
*===========================================================================
*/
VOID DispSupMiscPara20( PARAMISCPARA *pData )
{
    static const TCHAR FARCONST auchDispSupMisc20A[] = _T("%3u  %3u\t%ld\n%-20s");
    static const TCHAR FARCONST auchDispSupMisc20B[] = _T("%3u  %3u\t%10s\n%-20s");

    TCHAR   aszDspNull[1] = {_T('\0')};
    USHORT  usNoData = AC_MISC_PARA;
    USHORT  usAddress = ( USHORT)(pData->uchAddress);
    ULONG   ulData = pData->ulMiscPara;


	DispSupSaveData.MiscPara = *pData;

    /* Display ForeGround */
    UiePrintf(UIE_OPER,                             /* Operator Display */
              0,                                    /* Row */
              0,                                    /* Column */
              auchDispSupMisc20A,                   /* Format */
              usNoData,                             /* A/C Number */
              usAddress,                            /* Address */
              ulData,                               /* Misc. Parameter Data */
              pData->aszMnemonics);                 /* Lead Thru Data */

    /* Display BackGround */
    UieEchoBackGround(UIE_KEEP,                     /* Descriptor Control */
                      UIE_ATTR_NORMAL,              /* Attribute */
                      auchDispSupMisc20B,           /* Format */
                      usNoData,                     /* A/C Number */
                      ( USHORT)pData->uchAddress,   /* Address */
                      aszDspNull,                   /* Null */
                      pData->aszMnemonics);         /* Lead Thru Data */
}

/* --- End of Source File --- */