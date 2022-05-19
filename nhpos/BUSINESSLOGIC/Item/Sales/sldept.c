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
* Title       : Department sales module
* Category    : Item Module, NCR 2170 Hsopitality US Model Application
* Program Name: SLDEPT.C
* --------------------------------------------------------------------------
* Abstruct: ItemSalesDept() : Dept Item sales module main
*
* --------------------------------------------------------------------------
* Update Histories
*    Date  : Ver. Rev. :   Name    : Description
* May-13-92: 00.00.01  : K.Maeda   : initial
* Jun- 5-92: 00.00.01  : K.Maeda   : Apply comments of code inspection held
*          :           :           : on Jun 3
*          :           :           :
** GenPOS **
* Feb-01-18: 02.02.02  : R.Chambers  : removed unneeded includes, DCURRENCY for LONG
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

#include	<tchar.h>
#include    <memory.h>
#include    <string.h>

#include    <ecr.h>
#include    <uie.h>
#include    <pif.h>
#include    <log.h>
#include    <regstrct.h>
#include    <transact.h>
#include    <item.h>
#include    <paraequ.h>
#include    <para.h>
#include    <display.h>
#include    <appllog.h>
#include    "itmlocal.h"

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesDept(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales)
*
*    Input: UIFREGSALES *pItemSales
*
*   Output: SalesData.uchMajorClass : Major class for transaction module
*           SalesData.uchMinorClass : Minor class for transaction module
*           SalesData.uchDeptNo     : Extended FSC code of dept ( = dept. No. )
*           SalesData.lQTY          : Sales QTY or weight
*           SalesData.fbModifier    : Void, Number entry indication flag
*           SalesData.aszNumber     : Number entered
*
*    InOut: None
*
**Return: Return status returned from sub module
*
** Description: This module is the main module of Sales module.
                This module executes the following functions.

*===========================================================================
*/

SHORT   ItemSalesDept(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales)
{
    SHORT  sReturnStatus;
    UCHAR  uchRestrict, uchDummy; /* dummy */
    USHORT usLinkNo;    /* dummy */

    /*---  CHECK PRINT MOD. OR COMDIMENT COMPULSORY STATUS ---*/
    if ((sReturnStatus = ItemCommonCheckComp()) != ITM_SUCCESS) {
        return(sReturnStatus);
    }

    if ((sReturnStatus = ItemSalesPrepare(pUifRegSales,
        pItemSales, &uchRestrict, &usLinkNo, &uchDummy)) != ITM_SUCCESS) {
        return(sReturnStatus);
    }

    /*----- call ItemSalesCommon() -----*/
    if ((sReturnStatus = ItemSalesCommon(pUifRegSales, pItemSales)) != ITM_SUCCESS) {
        return(sReturnStatus);
    }

    /*----- call ItemSalesDeptOpen() -----*/
    if ((sReturnStatus = ItemSalesDeptOpen(pUifRegSales, pItemSales)) != ITM_SUCCESS) {
        return(sReturnStatus);
    }

    /*----- call ItemSalesModifier() -----*/
    if ((sReturnStatus = ItemSalesModifier(pUifRegSales, pItemSales)) != ITM_SUCCESS) {
        return(sReturnStatus);
    }

    /*----- call ItemSalesCalculation() -----*/
	//unless @/for processing, since calculation already preformed SR 143 cwunn
	if(!(pUifRegSales->usFor)){//not using @/For since no for data
		if ((sReturnStatus = ItemSalesCalculation(pItemSales)) != ITM_SUCCESS) {
			return(sReturnStatus);
		}
	}
	else {
		DCURRENCY totalAmount;	//SR281 ESMITH

		//SR281 ESMITH
		ItemCalAmount(pUifRegSales->lQTY, pUifRegSales->usForQty, pUifRegSales->usFor, &totalAmount);

		pItemSales->lUnitPrice = totalAmount / (pItemSales->lQTY / 1000);
		pItemSales->usFor = pUifRegSales->usFor;
		pItemSales->usForQty = pUifRegSales->usForQty;
		pItemSales->lProduct = totalAmount;
	}

	pItemSales->fsLabelStatus |= ITM_CONTROL_NO_CONSOL;  // these are uniqute items in transaction

	pItemSales->usUniqueID = ItemSalesGetUniqueId(); //unique id for condiment editing JHHJ
	pItemSales->usKdsLineNo = 0;                     //reset KDS line number so that a new one will be assigned by ItemSendKdsItemGetLineNumber()

    /*----- call ItemSalesCommonIF() -----*/
    return(ItemSalesCommonIF(pUifRegSales, pItemSales));

}
