/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1996            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*==========================================================================
* Title       : Set Dispenser Parameter Module( SUPER & PROGRAM MODE )                       
* Category    : Display-2x20, NCR 2170 US Hospitality Application Program        
* Program Name: DSPDSP20.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*               
*               DispSupDispPara20()    : Display Dispenser Parameter 
*                                                   
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Feb-02-96: 03.00.00 : M.Ozawa   : initial                                   
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

#include "ecr.h"
#include "uie.h"
#include "paraequ.h"
#include "para.h"
#include "maint.h"
#include "display.h"
#include "disp20.h"

/*
*===========================================================================
** Synopsis:    VOID DispSupDispPara20( PARADISPPARA *pData )
*               
*     Input:    *pData          : Pointer to Structure for PARADISPPARA
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Displays Beverage Dispenser Parameter
*===========================================================================
*/

VOID DispSupDispPara20( PARADISPPARA *pData )
{
    /* Define Display Format */ 
    static const TCHAR  auchDispSupDispPara20A[] = _T("%3u %3u\t%4u-%2u\n%-16s\t%1u");
    static const TCHAR  auchDispSupDispPara20B[] = _T("%3u %3u\t%10s\n%-16s\t%1u");
    static const TCHAR  auchDispSupDispPara20C[] = _T("%3u %3u\t%5u\n%-16s\t%1u");

    TCHAR   aszDspNull[1] = {0};

    DispSupSaveData.DispPara = *pData;      /* Save Data for Redisplay if needed. */

    /* Display ForeGround */
    if (pData->usAddress < DISPENS_OFFSET_ADR) {
        UiePrintf(UIE_OPER,                                     /* Operator Display */
                  0,                                            /* Row */
                  0,                                            /* Column */
                  auchDispSupDispPara20A,                       /* Format */
                  AC_DISPENSER_PARA,                            /* A/C Number */
                  pData->usAddress,                             /* Address */
                  pData->usPluNumber,                           /* PLU Number */
                  pData->uchAdjective,                          /* Adjective */
                  pData->aszMnemonics,                          /* Lead Thru data */
                  uchMaintMenuPage);                            /* Menu Page Number */
    } else {
        UiePrintf(UIE_OPER,                                     /* Operator Display */
                  0,                                            /* Row */
                  0,                                            /* Column */
                  auchDispSupDispPara20C,                       /* Format */
                  AC_DISPENSER_PARA,                            /* A/C Number */
                  pData->usAddress,                             /* Address */
                  pData->usPluNumber,                           /* PLU Number */
                  pData->aszMnemonics,                          /* Lead Thru data */
                  uchMaintMenuPage);                            /* Menu Page Number */
    }

    /* Display BackGround */
    UieEchoBackGround(UIE_KEEP,                             /* Descriptor Control */
                      UIE_ATTR_NORMAL,                      /* Attribute */
                      auchDispSupDispPara20B,               /* Format */
                      AC_DISPENSER_PARA,                    /* A/C Number */
                      pData->usAddress,                     /* Address */
                      aszDspNull,                           /* Null */
                      pData->aszMnemonics,                  /* Lead Thru data */
                      uchMaintMenuPage);                    /* Menu Page Number */

}
