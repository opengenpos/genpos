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
* Title       : Set Tare Module( SUPER & PROGRAM MODE )                       
* Category    : Display-2x20, NCR 2170 US Hospitality Application Program        
* Program Name: DSPTAR20.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*               
*               DispSupTare20()      : Display Tare
*                                                    
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Nov-24-92: 01.00.00 : J.Ikeda   : initial                                   
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
** Synopsis:    VOID DispSupTare20( PARATARE *pData )
*               
*     Input:    *pData          : Pointer to Structure for PARATARE
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This function displays Tare.
*===========================================================================
*/

VOID DispSupTare20( PARATARE *pData )
{
    /* define display format */ 
    static const TCHAR FARCONST auchDispSupTare20A[] = _T("%3u  %3u\t%.2$\n%-18s\t%u");
    static const TCHAR FARCONST auchDispSupTare20B[] = _T("%3u  %3u\t%.3$\n%-18s\t%u");
    static const TCHAR FARCONST auchDispSupTare20C[] = _T("%3u  %3u\t%10s\n%-18s\t%u");

    TCHAR   aszDspNull[1] = {_T('\0')};
   
    /* save data */
	DispSupSaveData.Tare = *pData;

    /* distinguish minor class */

    switch (pData->uchMinorClass) {
    case CLASS_PARATARE_001:    /* minimum unit is 0.01 */

        /* display foreground */
        UiePrintf(UIE_OPER,                     /* operator display */
                  0,                            /* row */
                  0,                            /* column */
                  auchDispSupTare20A,           /* format */
                  AC_TARE_TABLE,                /* A/C number */
                  ( USHORT)pData->uchAddress,   /* address */
                  ( SHORT)pData->usWeight,      /* tare, format is %.2$ (two decimal places) and requires SHORT not LONG */
                  pData->aszMnemonics,          /* lead through data */
                  uchMaintMenuPage);            /* Page Number */
        break;

    case CLASS_PARATARE_0001:   /* minimum unit is 0.001 or 0.005 */

        /* display foreground */
        UiePrintf(UIE_OPER,                     /* operator display */
                  0,                            /* row */
                  0,                            /* column */
                  auchDispSupTare20B,           /* format */
                  AC_TARE_TABLE,                /* A/C number */
                  ( USHORT)pData->uchAddress,   /* address */
                  ( SHORT)pData->usWeight,      /* tare, format is %.3$ (three decimal places) and requires SHORT not LONG */
                  pData->aszMnemonics,          /* lead through data */
                  uchMaintMenuPage);            /* Page Number */
        break;

    default:
/*        PifAbort(MODULE_DISPSUP_ID, FAULT_INVALID_DATA); */
        break;
    }

    /* display background */
    UieEchoBackGround(UIE_KEEP,                     /* descriptor control */
                      UIE_ATTR_NORMAL,              /* attribute */
                      auchDispSupTare20C,           /* format */
                      AC_TARE_TABLE,                /* A/C number */
                      ( USHORT)pData->uchAddress,   /* address */
                      aszDspNull,                   /* null */
                      pData->aszMnemonics,          /* lead through data */
                      uchMaintMenuPage);            /* Page Number */
}

