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
* Title       : Set Currency Conversion Rate Module( SUPER & PROGRAM MODE )                       
* Category    : Display-2x20, NCR 2170 US Hospitality Application Program        
* Program Name: DSPFC20.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*               
*               DispSupCurrency20()     : Display Currency Conversion Rate
*                                           
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-22-92: 00.00.01 : K.You     : initial                                   
* Nov-19-92: 01.00.00 : K.You     : Chg Decimal Point from 6 to 5                                   
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

#include "ecr.h"
#include "uie.h"
/* #include <pif.h> */
#include "paraequ.h"
#include "para.h"
#include "maint.h"
#include "display.h"
#include "disp20.h"


/*
*===========================================================================
** Synopsis:    VOID DispSupCurrency20( PARACURRENCYTBL *pData )
*               
*     Input:    *pData          : Pointer to Structure for PARACURRENCYTBL
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Displays Currency Conversion Rate Data
*===========================================================================
*/

VOID DispSupCurrency20( PARACURRENCYTBL *pData )
{
    /* Define Display Format */ 
    static const TCHAR  auchDispSupCurrency20A1[] = _T("%3u  %3u\t%.*l$\n%-18s\t%u");
    static const TCHAR  auchDispSupCurrency20B[] = _T("%3u  %3u\t%10s\n%-18s\t%u");

    TCHAR   aszDspNull[1] = {0};
    SHORT   sDecPoint;

    /* V3.4 */
    if (pData->uchStatus & MAINT_6DECIMAL_DATA) {
        sDecPoint = MAINT_6DECIMAL;
    } else {
        sDecPoint = MAINT_5DECIMAL;
    }

	DispSupSaveData.CurrencyTbl = *pData;   /* Save Data for Redisplay if needed. */

    /* Display ForeGround */
    UiePrintf(UIE_OPER,                             /* Operator Display */
              0,                                    /* Row */
              0,                                    /* Column */
              auchDispSupCurrency20A1,              /* Format */
              AC_CURRENCY,                          /* A/C Number */
              ( USHORT)pData->uchAddress,           /* Address */
              sDecPoint,
              (DCURRENCY)pData->ulForeignCurrency,             /* Foreign Currency Rate Data */
              pData->aszMnemonics,                  /* Lead Thru Data */
              uchMaintMenuPage);                    /* Page Number */

    /* Display BackGround */
    UieEchoBackGround(UIE_KEEP,                     /* Descriptor Control */
                      UIE_ATTR_NORMAL,              /* Attribute */
                      auchDispSupCurrency20B,       /* Format */
                      AC_CURRENCY,                  /* A/C Number */
                      ( USHORT)pData->uchAddress,   /* Address */
                      aszDspNull,                   /* Null */
                      pData->aszMnemonics,          /* Lead Thru Data */
                      uchMaintMenuPage);            /* Page Number */
}
