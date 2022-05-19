/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*==========================================================================
* Title       : Preset Amount Cash Tender Module( SUPER & PROGRAM MODE )                       
* Category    : Display-2x20, NCR 2170 US Hospitality Application Program        
* Program Name: DSPPAMT2.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*                                           
*               DispSupPresetAmount20()     : Display Preset Amount Cash Tender
*                                           
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jul-02-93: 01.00.12 : K.You     : initial                                   
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
#include "csstbpar.h"


/*
*===========================================================================
** Synopsis:    VOID DispSupPresetAmount20( PARAPRESETAMOUNT *pData )
*               
*     Input:    *pData          : Pointer to Structure for PARAPRESETAMOUNT
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Displays Preset Amount Cash Tender Data
*===========================================================================
*/

VOID DispSupPresetAmount20( PARAPRESETAMOUNT *pData )
{
    /* Define Display Format */ 
    static const TCHAR  auchDispSupPresetAmount20A[] = _T("%3u  %3u\t%.2l$\n%-18s\t%s");
    static const TCHAR  auchDispSupPresetAmount20B[] = _T("%3u  %3u\t%10s\n%-18s\t%s");

    TCHAR   aszDspNull[1] = {0};
 
	DispSupSaveData.PresetAmount = *pData;    /* Save Data for Redisplay if needed. */

    /* Display ForeGround */
    UiePrintf(UIE_OPER,                             /* Operator Display */
              0,                                    /* Row */
              0,                                    /* Column */
              auchDispSupPresetAmount20A,           /* Format */
              PG_PRESET_AMT,                        /* Prog. Number */
              ( USHORT)pData->uchAddress,           /* Address */
              (DCURRENCY)pData->ulAmount,           /* Preset Amount Data, format %.2l$ requires DCURRENCY */
              pData->aszMnemonics,                  /* Lead Thru Data */
              aszDspNull);                          /* Null */

    /* Display BackGround */
    UieEchoBackGround(UIE_KEEP,                     /* Descriptor Control */
                      UIE_ATTR_NORMAL,              /* Attribute */
                      auchDispSupPresetAmount20B,   /* Format */
                      PG_PRESET_AMT,                /* Prog. Number */
                      ( USHORT)pData->uchAddress,   /* Address */
                      aszDspNull,                   /* Null */
                      pData->aszMnemonics,          /* Lead Thru Data */
                      aszDspNull);                  /* Null */
}


/*
*===========================================================================
** Synopsis:    VOID DispSupPresetAmount20( PARAPRESETAMOUNT *pData )
*               
*     Input:    *pData          : Pointer to Structure for PARAPRESETAMOUNT
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Displays Preset Amount Cash Tender Data
*===========================================================================
*/

VOID DispSupAutoCoupon20( PARAAUTOCPN *pData )
{
    /* Define Display Format */ 
    static const TCHAR  auchDispSupPresetAmount20A[] = _T("%3u  %3u\t%.2l$\n%-18s\t%s");
    static const TCHAR  auchDispSupPresetAmount20B[] = _T("%3u  %3u\t%10s\n%-18s\t%s");

    TCHAR   aszDspNull[1] = {0};

	DispSupSaveData.AutoCoupon = *pData;    /* Save Data for Redisplay if needed.*/

    /* Display ForeGround */
    UiePrintf(UIE_OPER,                             /* Operator Display */
              0,                                    /* Row */
              0,                                    /* Column */
              auchDispSupPresetAmount20A,           /* Format */
              PG_AUTO_CPN,                        /* Prog. Number */
              ( USHORT)pData->uchAddress,           /* Address */
			  (DCURRENCY)pData->ulAmount,           /* Preset Amount Data, format %.2l$ requires DCURRENCY */
              pData->aszMnemonics,                  /* Lead Thru Data */
              aszDspNull);                          /* Null */

    /* Display BackGround */
    UieEchoBackGround(UIE_KEEP,                     /* Descriptor Control */
                      UIE_ATTR_NORMAL,              /* Attribute */
                      auchDispSupPresetAmount20B,   /* Format */
                      PG_AUTO_CPN,                /* Prog. Number */
                      ( USHORT)pData->uchAddress,   /* Address */
                      aszDspNull,                   /* Null */
                     pData->aszMnemonics,          /* Lead Thru Data */
                      aszDspNull);                  /* Null */
}