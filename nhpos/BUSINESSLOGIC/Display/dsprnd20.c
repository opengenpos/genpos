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
* Title       : Set Rounding Table Module( SUPER & PROGRAM MODE )                       
* Category    : Display-2x20, NCR 2170 US Hospitality Application Program        
* Program Name: DSPRND20.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*               
*               DispSupRound20()    : Display Rounding Table
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
** Synopsis:    VOID DispSupRound20( PARAROUNDTBL *pData )
*               
*     Input:    *pData          : Pointer to Structure for PARAROUNDTBL
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Displays Rounding Table Data
*===========================================================================
*/

VOID DispSupRound20( PARAROUNDTBL *pData )
{
    /* Define Display Format */ 
    static const TCHAR  auchDispSupRound20A[] = _T("%3u  %3u\t%4u\n%-18s\t%u");
    static const TCHAR  auchDispSupRound20B[] = _T("%3u  %3u\t%2d\n%-18s\t%u");
    static const TCHAR  auchDispSupRound20C[] = _T("%3u  %3u\t%10s\n%-18s\t%u");

    TCHAR   aszDspNull[1] = {0};
 
	DispSupSaveData.RoundTbl = *pData;     /* Save Data for Redisplay if needed */

    /* Distinguish Minor Class */
    switch (pData->uchMinorClass) {
    case CLASS_PARAROUNDTBL_DELI:               /* Delimitor, Modules Case */

        /* Display ForeGround */
		{
			USHORT  usDeliModu;

			usDeliModu = pData->uchRoundDelimit * 100;                                        
			usDeliModu += pData->uchRoundModules;                                        

			UiePrintf(UIE_OPER,                         /* Operator Display */
					  0,                                /* Row */
					  0,                                /* Column */
					  auchDispSupRound20A,              /* Format */
					  AC_ROUND,                         /* A/C Number */
					  ( USHORT)pData->uchAddress,       /* Address */
					  usDeliModu,                       /* Delimitor, Modules Data */
					  pData->aszMnemonics,              /* Lead Thru Data */
					  uchMaintMenuPage);                /* Page Number */
		}
        break;

    case CLASS_PARAROUNDTBL_POSI:                   /* Decimal Position Case */

        /* Display ForeGround */
        UiePrintf(UIE_OPER,                         /* Operator Display */
                  0,                                /* Row */
                  0,                                /* Column */
                  auchDispSupRound20B,              /* Format */
                  AC_ROUND,                         /* A/C Number */
                  ( USHORT)pData->uchAddress,       /* Address */
                  ( SHORT)pData->chRoundPosition,   /* Decimal Position Data */
                  pData->aszMnemonics,              /* Lead Thru Data */
                  uchMaintMenuPage);                /* Page Number */
        break;

    default:
/*        PifAbort(MODULE_DISPSUP_ID, FAULT_INVALID_DATA); */
        break;
    }

    /* Display BackGround */
    UieEchoBackGround(UIE_KEEP,                     /* Descriptor Control */
                      UIE_ATTR_NORMAL,              /* Attribute */
                      auchDispSupRound20C,          /* Format */
                      AC_ROUND,                     /* A/C Number */
                      ( USHORT)pData->uchAddress,   /* Address */
                      aszDspNull,                   /* Null */
                      pData->aszMnemonics,          /* Lead Thru Data */
                      uchMaintMenuPage);            /* Page Number */
}
