/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Multiline Display PPI Maintenance Format ( SUPER & PROGRAM MODE ) 
* Category    : Multiline Display, NCR 2170 US Hospitality Application Program    
* Program Name: MLDPPIT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function controls Employee No.
*
*    The provided function names are as follows: 
* 
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jul-29-95: 03.00.03 : M.Ozawa   : initial
* 
*** NCR2171 **
* Aug-26-99: 01.00.00 : M.Teraki  : initial (for Win32)
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
#include <stdlib.h>
#include <string.h>

#include <ecr.h>
#include <paraequ.h> 
#include <para.h>
#include <rfl.h> 
#include <maint.h> 
#include <regstrct.h>
#include <transact.h>
#include <csstbpar.h>
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <mld.h>
#include <mldsup.h>
#include <mldmenu.h>


/*
;==============================================================================
;                      C O M M O N   R A M   A R E A                         
;=============================================================================
*/

/*------------------------------------------------------------------------*\
P P I   M a i n t e n a n c e
\*------------------------------------------------------------------------*/
static CONST TCHAR  aszMldAC71[][64] = {
	_T("....*....1....*....2....*....3....*....4"),
	_T("\t%s %s"),
	/*                            "\tQTY PRICE  ", */
	_T("%2d\t%3d %6lu "),
	_T("")
};

static CONST TCHAR  aszMldAC71_AddSettingsTitle[][64] = {
	_T("....*....1....*....2....*....3....*....4"),
	_T(" %s \t%s"),
	/*                            "\tQTY PRICE  ", */
	_T(" %s \t%6lu "),
	_T("")
};

static CONST TCHAR  aszMldAC71_AddSettings[][64] = {
	_T("....*....1....*....2....*....3....*....4"),
	_T("\t%s %s"),
	/*                            "\tQTY PRICE  ", */
	_T("%2d %3s \t%6lu "),
	_T("")
};

/*
*===========================================================================
** Synopsis:    VOID  MldParaPPI( PARAPPI *pData )  
*               
*     Input:    *pData  : pointer to structure for PARAPPI
*               UCHAR          uchClear ( 0 : During Edit, 1 : When Read )
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function sets data and display to MLD.
*===========================================================================
*/

VOID  MldParaPPI( PARAPPI *pData, USHORT usClear)
{
    /********************************************************/
    /* Clear Scroll Display and Display Item telop,         */
    /*                      when ONLY after Parameter Read. */
    /********************************************************/
    if(usClear != 0){
        MldScrollClear(MLD_SCROLL_1);

		if (usClear == 1 || usClear == 2) // if page 1 or 2		-	PPI Enhancement - CSMALL
		{
			USHORT  i;
			TCHAR   aszQty[PARA_TRANSMNEMO_LEN + 1] = { 0 };
			TCHAR   aszPrice[PARA_TRANSMNEMO_LEN + 1] = { 0 };

			/* get programmable mnemonics, V3.3 */
			RflGetTranMnem (aszQty, TRN_QTY_ADR);
			RflGetTranMnem(aszPrice, TRN_AMTTL_ADR);

			MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP,&(aszMldAC71[1][0]), aszQty, aszPrice);

			for (i=((usClear == 1) ? 0 : 20); i<pData->uchPPILength; i++) {

				/*********************/
				/* Display Parameter */
				/*********************/
				if (i >= 20)
				{
					MldPrintf(MLD_SCROLL_1,(SHORT)(MLD_SCROLL_TOP+(i-20)+1), &(aszMldAC71[2][0]), (USHORT)(i+1), (USHORT)(pData->ParaPPI[i].uchQTY), pData->ParaPPI[i].ulPrice);
				} else {

					MldPrintf(MLD_SCROLL_1,(SHORT)(MLD_SCROLL_TOP+i+1), &(aszMldAC71[2][0]), (USHORT)(i+1), (USHORT)(pData->ParaPPI[i].uchQTY), pData->ParaPPI[i].ulPrice);
				}
				if (i==19)	// PPI Enhancement - CSMALL
				{
					return;
				} else if (i==39)
				{
					return;
				}
			}
		}
		else if (usClear == 3) {  // if page 3		-	PPI Enhancement CSMALL
			TCHAR	aszAddSetting[PARA_TRANSMNEMO_LEN + 1] = { 0 };
			TCHAR	aszAddSettingValue[PARA_TRANSMNEMO_LEN + 1] = { 0 };

			TCHAR	aszBeforeQTY[PARA_TRANSMNEMO_LEN + 1] = { 0 };
			TCHAR	aszAfterQTY[PARA_TRANSMNEMO_LEN + 1] = { 0 };
			TCHAR	aszMinPrice[PARA_TRANSMNEMO_LEN + 1] = { 0 };

			/* get programmable mnemonics, V3.3 */
			RflGetTranMnem(aszAddSetting, TRN_PPI_ADD_SETTING);
			RflGetTranMnem(aszAddSettingValue, TRN_PPI_ADD_VALUE);

			MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP,&(aszMldAC71_AddSettingsTitle[1][0]), aszAddSetting, aszAddSettingValue);

			RflGetTranMnem(aszBeforeQTY, TRN_PPI_BEFORE_QTY);
			RflGetTranMnem(aszAfterQTY, TRN_PPI_AFTER_QTY);
			RflGetTranMnem(aszMinPrice, TRN_PPI_MINSALE);
			
			MldPrintf(MLD_SCROLL_1,(SHORT)(MLD_SCROLL_TOP+1), &(aszMldAC71_AddSettings[2][0]), (USHORT)(1), aszBeforeQTY, (USHORT)(pData->ParaPPI[0].uchQTY));
			
			MldPrintf(MLD_SCROLL_1,(SHORT)(MLD_SCROLL_TOP+2), &(aszMldAC71_AddSettings[2][0]), (USHORT)(2), aszAfterQTY, (USHORT)(pData->ParaPPI[1].uchQTY));
			
			MldPrintf(MLD_SCROLL_1,(SHORT)(MLD_SCROLL_TOP+3), &(aszMldAC71_AddSettings[2][0]), (USHORT)(3), aszMinPrice, (USHORT)(pData->ParaPPI[2].ulPrice));

		}// end else if
    }
    else {
        /*********************/
        /* Display Parameter */
        /*********************/
		if (pData->uchAddr > 20)
		{
			MldPrintf(MLD_SCROLL_1,(SHORT)(MLD_SCROLL_TOP+(pData->uchAddr-20)), &(aszMldAC71[2][0]), (USHORT)pData->uchAddr, (USHORT)(pData->ParaPPI[pData->uchAddr-1].uchQTY), pData->ParaPPI[pData->uchAddr-1].ulPrice);
		} else {
			MldPrintf(MLD_SCROLL_1,(SHORT)(MLD_SCROLL_TOP+pData->uchAddr), &(aszMldAC71[2][0]), (USHORT)pData->uchAddr, (USHORT)(pData->ParaPPI[pData->uchAddr-1].uchQTY), pData->ParaPPI[pData->uchAddr-1].ulPrice);
		}
    }
    return;
}

/***** End of Source *****/

