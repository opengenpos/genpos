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
* Title       :                           
* Category    : Item Module, NCR 2170 US Hsopitality Model Application         
* Program Name: SLLOCAL.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: ItemSalesGetLocal()  
*         : ItemSalesPutLocal()  
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver. Rev. :   Name    : Description
* May-16-92: 00.00.01  : K.Maeda   : initial                                   
*          :           :           :                         
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

#include    "ecr.h"
#include    "pif.h"
#include    "log.h"
#include    "appllog.h"
#include    "regstrct.h"
#include    "transact.h"
#include    "item.h"
#include    "paraequ.h"
#include    "para.h"
#include    "itmlocal.h"


// for instances where this memory resident data is cleared
// see the functions ItemModeIn() and ItemInit()
ITEMSALESLOCAL   ItemSalesLocal;                /* Sales module local data */


/*
*===========================================================================
**Synopsis: VOID   ItemSalesGetLocal(ITEMSALESLOCAL *pData);
**Synopsis: VOID   ItemSalesPutLocal(ITEMSALESLOCAL *pData);
*
*    Input: Nothing
*
*   Output: ITEMSALESLOCAL *pData
*
*    InOut: Nothing
*
**Return: VOID
*
** Description: Output item sales local data into receive buffer prepared 
*               by caller.  
** Description: Rewrite item sales local data by the datas received from caller. 
*                
*===========================================================================
*/
CONST volatile ITEMSALESLOCAL * CONST ItemSalesLocalPtr = &ItemSalesLocal;

ITEMSALESLOCAL *ItemSalesGetLocalPointer (VOID)
{
    return &ItemSalesLocal;
}

VOID   ItemSalesGetLocal(ITEMSALESLOCAL *pSalesLocal)
{
    *pSalesLocal = ItemSalesLocal;
}

VOID   ItemSalesPutLocal(ITEMSALESLOCAL *pSalesLocal)
{
    ItemSalesLocal = *pSalesLocal;

}

SHORT  ItemTenderGetSetTipReturn (SHORT bReturnTipState)
{
	SHORT  sRetStatus = ((ItemSalesLocal.fbSalesStatus & SALES_RETURN_TIPS) ? 1 : 0);

	if (bReturnTipState >= 0) {
		if (bReturnTipState)
			ItemSalesLocal.fbSalesStatus |= SALES_RETURN_TIPS;
		else
			ItemSalesLocal.fbSalesStatus &= ~SALES_RETURN_TIPS;
	}

	return sRetStatus;
}

/*
*===========================================================================
**Synopsis: BOOL   ItemSalesLocalSaleStarted(VOID)
*
*    Input: Nothing
*
*   Output: TRUE if sale started (ItemSalesLocal.fbSalesStatus & SALES_ITEMIZE)
*           FALSE if sale not yet started
*
*    InOut: Nothing
*
**Return: BOOL
*
** Description: This function hides the actual test done to determine if the
*               SALES_ITEMIZE flag has been set or not. The SALES_ITEMIZE flag
*               is used to indicate that the necessary environment setup for a
*               sales transaction has been initialized.
*
*               This function's declaration is in item.h so that it can be used in
*               subsystems that do not need to include itemlocal.h or access the memory
*               resident database directly to determine if a sales transaction has been
*               started.
*
*               See instances of setting the flag in functions ItemCommonSetupTransEnviron(),
*               ItemOtherCursorVoid(), ItemSalesCursorModDiscIn(), and ItemSalesCursorRepeat().
*
*               For instances where it is cleared see ItemTotalTT(), ItemModeIn(), and ItemInit().
*===========================================================================
*/
BOOL   ItemSalesLocalSaleStarted(VOID)
{
	return (ItemSalesLocal.fbSalesStatus & SALES_ITEMIZE);
}