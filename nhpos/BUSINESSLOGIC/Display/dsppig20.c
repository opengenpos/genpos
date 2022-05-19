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
* Title       : Set Pig Rule Table Module( SUPER & PROGRAM MODE )                       
* Category    : Display-2x20, NCR 2170 US Hospitality Application Program        
* Program Name: DSPPIG20.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*                                   
*               DispSupPigRule20()  : Display Pig Rule Table
*                                                    
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jul-15-93: 01.00.12 : K.You     : initial                                   
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
** Synopsis:    VOID DispSupPigRule20( PARAPIGRULE *pData )
*               
*     Input:    *pData          : Pointer to Structure for PARAPIGRULE
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Displays Pig Rule Table Data
*===========================================================================
*/

VOID DispSupPigRule20( PARAPIGRULE *pData )
{
    /* Define Display Format */ 
    static const TCHAR FARCONST auchDispSupPigRule20A[] = _T("%3u  %3u\t%8l$\n%-16s\t%1u");
    static const TCHAR FARCONST auchDispSupPigRule20B[] = _T("%3u  %3u\t%2lu\n%-16s\t%1u");
    static const TCHAR FARCONST auchDispSupPigRule20C[] = _T("%3u  %3u\t%10s\n%-16s\t%1u");

    TCHAR   aszDspNull[1] = {_T('\0')};
 
    /* Save Data */
	DispSupSaveData.PigRuleTbl = *pData;

    /* Distinguish Minor Class */
    switch (pData->uchMinorClass) {
    case CLASS_PARAPIGRULE_PRICE:               /* Pig Rule Break Point */
        /* Display ForeGround */
        UiePrintf(UIE_OPER,                     /* Operator Display */
                  0,                            /* Row */
                  0,                            /* Column */
                  auchDispSupPigRule20A,        /* Format */
                  AC_PIGRULE,                   /* A/C Number */
                  ( USHORT)pData->uchAddress,   /* Address */
                  (DCURRENCY)pData->ulPigRule,  /* Pig Rule Break Point, format %8l$ requires DCURRENCY */
                  pData->aszMnemonics,          /* Lead Thru Data */
                  uchMaintMenuPage);            /* Page Number */
        break;

    case CLASS_PARAPIGRULE_COLIMIT:            /* Pig Rule Counter Limit */
        /* Display ForeGround */
        UiePrintf(UIE_OPER,                     /* Operator Display */
                  0,                            /* Row */
                  0,                            /* Column */
                  auchDispSupPigRule20B,        /* Format */
                  AC_PIGRULE,                   /* A/C Number */
                  ( USHORT)pData->uchAddress,   /* Address */
                  pData->ulPigRule,             /* Pig Rule Counter Limit, format %2lu rquires ULONG */
                  pData->aszMnemonics,          /* Lead Thru Data */
                  uchMaintMenuPage);            /* Page Number */
        break;

    default:
/*        PifAbort(MODULE_DISPSUP_ID, FAULT_INVALID_DATA); */
        break;
    }

    /* Display BackGround For 2X20 */
    UieEchoBackGround(UIE_KEEP,                     /* Descriptor Control */
                      UIE_ATTR_NORMAL,              /* Attribute */
                      auchDispSupPigRule20C,        /* Format */
                      AC_PIGRULE,                   /* A/C Number */
                      ( USHORT)pData->uchAddress,   /* Address */
                      aszDspNull,                   /* Null */
                      pData->aszMnemonics,          /* Lead Thru Data */
                      uchMaintMenuPage);            /* Page Number */
}
