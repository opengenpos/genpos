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
* Title       : Mode In Item module                         
* Category    : Item Module, NCR 2170 Hsopitality US Model Application         
* Program Name: ITMODEIN.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: ItemModeIn() : Reg. mode in module function   
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver. Rev. : Name      : Description
* May-12-92: 00.00.01  : K.Maeda   : initial                                   
* May-11-92:    "      : K.Maeda   : Correct comments at 1st code insp. 
*                                    on May-11
* Mar-27-95: 03.00.00  : hkato     : R3.0
* Nov-29-96: 03.01.10  : M.Suzuki  : Chenge Max. Menu Page 5->9
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
#include    <rfl.h>
#include    <regstrct.h>
#include    <transact.h>
#include    <item.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csstbpar.h>
#include    <display.h>
#include    <mld.h>
#include    <csetk.h>
#include    <csstbetk.h>
#include    "itmlocal.h"


/*
*===========================================================================
**Synopsis: SHORT   ItemModeIn(UIFREGMODEIN *pUifRegModeIn)
*
*    Input: UIFREGMODEIN pUifRegModeIn->chMajorClass
*                        pUifRegModeIn->uchMinorClass
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return: ITM_SCCESS
*
** Description: This module executes the following functions.

                - Initialize Item module local datas.
                - Issue transcation open function.
                - Display sign in request leadthrough.
                - Set drawer compulsory status of U.I engine.

*===========================================================================
*/

SHORT   ItemModeIn(UIFREGMODEIN *pUifRegModeIn)
{
	 ITEMTRANSLOCAL     TransDataBuffer = {0};
	 ITEMSALESLOCAL     SalesDataBuffer = {0};
	 ITEMTOTALLOCAL     TotalDataBuffer = {0};
	 ITEMTENDERLOCAL    TenderDataBuffer = {0};
	 ITEMMODLOCAL       ModDataBuffer = {0};
	 ITEMCOMMONLOCAL    CommonDataBuffer = {0};
	 ITEMREGMODEIN      ItemRegModeIn = {0};
	 PARACTLTBLMENU     MenuPage; 

    /* clear local memory resident transaction data areas */
    ItemTransPutLocal(&TransDataBuffer);     // zero out and clear memory resident data
    ItemSalesPutLocal(&SalesDataBuffer);     // zero out and clear memory resident data
    ItemTotalPutLocal(&TotalDataBuffer);     // zero out and clear memory resident data
	TenderDataBuffer.usCheckTenderId = 1;    // initialize the tender id count
    ItemTenderPutLocal(&TenderDataBuffer);   // zero out and clear memory resident data
    ItemModPutLocal(&ModDataBuffer);         // zero out and clear memory resident data
    /* initiaize other local data */
	{
		ITEMOTHERLOCAL      *pOtherLocalData = ItemOtherGetLocalPointer();         /* other module local data */
		memset(pOtherLocalData, 0, sizeof(*pOtherLocalData));  /* initialize data structure */
	}
    ItemCommonPutLocal(&CommonDataBuffer);    // zero out and clear memory resident data

    /*- build structure to make interface with transaction module -----*/
    ItemRegModeIn.uchMajorClass = CLASS_ITEMMODEIN; 
    ItemRegModeIn.uchMinorClass = CLASS_REGMODEIN; 
    
    /*----- call transaction module -----*/
    TrnModeIn(&ItemRegModeIn);              /* reg mode-in, VOID function */

    /*----- display mode in message -----*/
    ItemModeInDisplay(pUifRegModeIn);

    /*----- mamipulate drawer compulsory flag of UI engine -----*/
    if (CliParaMDCCheckField (MDC_DRAWER_ADR, MDC_PARAM_BIT_C)) { 
        UieDrawerComp(UIE_DISABLE);         /* set non compulsory drawer status */
    } else {
        UieDrawerComp(UIE_ENABLE);          /* set compulsory drawer status */
    }

    /*--- Default Shift Page,  R3.0 ---*/
    MenuPage.uchMajorClass = CLASS_PARACTLTBLMENU;                             
    MenuPage.uchAddress = CTLPG_STAND_ADR; 
    CliParaRead(&MenuPage);
    ItemSalesGetLocal(&SalesDataBuffer);

    if (MenuPage.uchPageNumber < 1 || MenuPage.uchPageNumber > MAX_PAGE_NO) {  /* 5->9 11/29/96 */
        SalesDataBuffer.uchMenuShift = 1; 
    } else {
        SalesDataBuffer.uchMenuShift = MenuPage.uchPageNumber;                        
    }
    ItemSalesPutLocal(&SalesDataBuffer);
    ItemCommonShiftPage(SalesDataBuffer.uchMenuShift);
    uchDispCurrMenuPage = SalesDataBuffer.uchMenuShift; /* V3.3 */

    return(ITM_SUCCESS);
}


/*
*===========================================================================
**Synopsis: VOID   ItemModeInDisplay(UIFREGMODEIN *pUifRegModeIn)
*
*    Input: UIFREGMODEIN *pUifRegModeIn
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return: Nothing
*
** Description: This module executes the following functions.

                - Initialize Item module local datas.
                - Issue transcation open function.
                - Display sign in request leadthrough.
                - Set drawer compulsory status of U.I engine.

*===========================================================================
*/

VOID   ItemModeInDisplay(UIFREGMODEIN *pUifRegModeIn)
{
	REGDISPMSG         DispMSGDataBuffer = {0};

    /*----- display mode in message -----*/
    DispMSGDataBuffer.uchMajorClass = pUifRegModeIn->uchMajorClass;
    DispMSGDataBuffer.uchMinorClass = pUifRegModeIn->uchMinorClass;
    RflGetLead (DispMSGDataBuffer.aszMnemonic, LDT_ID_ADR);

	if(!(flDispRegDescrControl & PM_CNTRL))	//Delayed Balance JHHJ
	{
		flDispRegDescrControl = 0L;
	}else
	{
		flDispRegDescrControl = 0;
		flDispRegDescrControl |= PM_CNTRL;
	}
	if( !(flDispRegKeepControl & PM_CNTRL))
	{
		flDispRegKeepControl = 0L;
	}else
	{
		flDispRegKeepControl = 0;
		flDispRegKeepControl |= PM_CNTRL;
	}           
    DispMSGDataBuffer.fbSaveControl = DISP_SAVE_TYPE_3;

    DispWriteSpecial(&DispMSGDataBuffer, &DispMSGDataBuffer);
    MldRefresh();
}

/****** end of file ******/

