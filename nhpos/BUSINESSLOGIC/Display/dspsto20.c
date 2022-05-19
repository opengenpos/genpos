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
* Title       : Store/Register No. Assignment Module( SUPER & PROGRAM MODE )                       
* Category    : Display-2x20, NCR 2170 US Hospitality Application Program        
* Program Name: DSPSTO20.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*               
*               DispSupStoreNo20()  : Display Store/Register No. Assignment
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

#include "ecr.h"
#include "uie.h"
#include "paraequ.h"
#include "para.h"
#include "maint.h"
#include "display.h"
#include "disp20.h"

/*
*===========================================================================
** Synopsis:    VOID DispSupStoreNo20( PARASTOREGNO *pData )
*               
*     Input:    *pData          : Pointer to Structure for PARASTOREGNO
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Displays Store/Register No. Assignment Data
*===========================================================================
*/

VOID DispSupStoreNo20( PARASTOREGNO *pData )
{
    /* Define Display Format */ 
    static const TCHAR  auchDispSupStoreNo20A1[] = _T("%3u  %3u\t%1u\n\t%s");
    static const TCHAR  auchDispSupStoreNo20A2[] = _T("%3u  %3u\t%3u\n\t%s");
    static const TCHAR  auchDispSupStoreNo20A3[] = _T("%3u  %3u\t%4u%03u\n\t%s");
    static const TCHAR  auchDispSupStoreNo20B[] = _T("%3u  %3u\t%10s\n\t%s");

    TCHAR   aszDspNull[1] = {0};

	DispSupSaveData.StoRegNo = *pData;    /* Save Data for Redispaly if needed */

    /* Display ForeGround */
    if (pData->usStoreNo == 0) {
        if (pData->usRegisterNo == 0) {
            UiePrintf(UIE_OPER,                         /* Operator Display */
                      0,                                /* Row */
                      0,                                /* Column */
                      auchDispSupStoreNo20A1,           /* Format */
                      PG_STOREG_NO,                     /* Program Number */
                      ( USHORT)pData->uchAddress,       /* Address */
                      0,                                /* Zero */
                      aszDspNull);                      /* Null */
        } else {
            UiePrintf(UIE_OPER,                         /* Operator Display */
                      0,                                /* Row */
                      0,                                /* Column */
                      auchDispSupStoreNo20A2,           /* Format */
                      PG_STOREG_NO,                     /* Program Number */
                      ( USHORT)pData->uchAddress,       /* Address */
                      pData->usRegisterNo,              /* Register Number */
                      aszDspNull);                      /* Null */
        }
    } else {
        UiePrintf(UIE_OPER,                         /* Operator Display */
                  0,                                /* Row */
                  0,                                /* Column */
                  auchDispSupStoreNo20A3,           /* Format */
                  PG_STOREG_NO,                     /* Program Number */
                  ( USHORT)pData->uchAddress,       /* Address */
                  pData->usStoreNo,                 /* Store Number */
                  pData->usRegisterNo,              /* Register Number */
                  aszDspNull);                      /* Null */
    }

    /* Display BackGround */
    UieEchoBackGround(UIE_KEEP,                     /* Descriptor Control */
                      UIE_ATTR_NORMAL,              /* Attribute */
                      auchDispSupStoreNo20B,        /* Format */
                      PG_STOREG_NO,                 /* Program Number */
                      ( USHORT)pData->uchAddress,   /* Address */
                      aszDspNull,                   /* Null */
                      aszDspNull);                  /* Null */    
}
