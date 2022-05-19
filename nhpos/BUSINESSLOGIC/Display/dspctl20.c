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
* Title       : Descriptor/Page/Leadthrough/Program/Action Code Control                       
* Category    : Display-2x20, NCR 2170 US Hospitality Application Program        
* Program Name: DSPCTL20.C
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows: 
*               
*			    DispSupControl20() : display Descriptor/Page/Leadthrough/Action Code
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* May-14-92: 00.00.01 : J.Ikeda   : initial                                   
* Nov-26-92: 01.00.00 : K.You     : Chg from usAmount to ulAmount
*
** NCR2171 **                                         
* Aug-26-99: 01.00.00 : M.Teraki  :initial (for 2171)
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
#include <tchar.h>
#include <string.h>
#include <stdlib.h>

#include "ecr.h"
#include "uie.h"
#include "paraequ.h"
#include "para.h"
#include "maint.h"
#include "display.h"
#include "disp20.h"

/*
*=======================================================================================
** Synopsis:    VOID DispSupControl20( MAINTDSPCTL *pData )
*               
*     Input:    *pData
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function displays Descriptor/Page/Leadthrough/Program/Action Code.
*=======================================================================================
*/

VOID DispSupControl20( MAINTDSPCTL *pData )
{
    TCHAR   aszDspNull[1] = {0};
    TCHAR   aszBuffer1[8] = { 0 };
    TCHAR   aszBuffer2[8] = { 0 };
    TCHAR   aszBuffer3[8] = { 0 };
    TCHAR   aszBuffer4[8] = { 0};
    TCHAR   aszBuffer5[12] = { 0 };
    TCHAR   aszBuffer6[8] = { 0 };

    static const TCHAR  auchDispSupCtl20A[] = _T("%3s %1s %1s %1s\t%s\n%-s\t%1s");
    static const TCHAR  auchDispSupCtl20B[] = _T("%3s %1s %1s %1s\t%10s\n%-s\t%1s");


    pData->uchPageNumber = uchMaintMenuPage;
	DispSupSaveData.MaintDspCtl = *pData;      /* Save Data for Redisplay if needed. */

    if(pData->usPGACNumber != 0) {
        _itot(pData->usPGACNumber, aszBuffer1, 10);
    }

    if (pData->uchPTDType != 0) {
        _itot(pData->uchPTDType, aszBuffer2, 10);
    }

    if (pData->uchRptType != 0) {
        _itot(pData->uchRptType, aszBuffer3, 10);
    }

    if (pData->uchResetType != 0) {
        _itot(pData->uchResetType, aszBuffer4, 10);
    }

	if (pData->usPGACNumber == AC_TOD) {
		// for time of day display from AC17 in Supervisor menu,
		// lets display leading zeros to help user understand the expected
		// data format for parameter entry.  See function MaintTODInit()
		// and other functions used by AC17 to read and write the time of
		// day data.
		switch (pData->uchRptType) {
		case 1:       // date as either MM/DD/YY or as DD/MM/YY
			swprintf(aszBuffer5, 12, L"%6.6lu", pData->ulAmount);
			break;
		case 2:       // time as HH:MM
			swprintf(aszBuffer5, 12, L"%4.4lu", pData->ulAmount);
			break;
		default:
			swprintf(aszBuffer5, 12, L"%4.4lu", pData->ulAmount);
			break;

		}
	}
	else {
		_ultot(pData->ulAmount, aszBuffer5, 10);
	}

    if (pData->uchPageNumber != 0) {
        _itot(pData->uchPageNumber, aszBuffer6, 10);
    }

    /* Display Foreground */
    UiePrintf(UIE_OPER,                     /* operator display */
              0,                            /* row */
              0,                            /* column */
              auchDispSupCtl20A,            /* format */
              aszBuffer1,                   /* data */
              aszBuffer2,                   /* PTD type */
              aszBuffer4,                   /* reset type */
              aszBuffer3,                   /* report type */
              aszBuffer5,                   /* amount */
              pData->aszMnemonics,          /* mnemonics */
              aszBuffer6);                  /* page number */

    /* Display Background */
    UieEchoBackGround(UIE_KEEP,             /* descriptor control */
                      UIE_ATTR_NORMAL,      /* attribute */
                      auchDispSupCtl20B,    /* format */
                      aszBuffer1,           /* data */
                      aszBuffer2,           /* PTD type */
                      aszBuffer4,           /* reset type */
                      aszBuffer3,           /* report type */
                      aszDspNull,           /* null */
                      pData->aszMnemonics,  /* mnemonics */
                      aszBuffer6);          /* page number */
}
