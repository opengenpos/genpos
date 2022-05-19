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
* Title       : Dept./ Open PLU sales module                          
* Category    : Item Module, NCR 2170 US Hsopitality Model Application         
* Program Name: DEPTOPEN.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: ItemSalesDeptOpen() : Dept Open PLU sales function   
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
#include    <ecr.h>
#include    <pif.h>
#include    <log.h>
#include    <uie.h>
#include    <rfl.h>
#include    <regstrct.h>
#include    <transact.h>
#include    <item.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csstbpar.h>
#include    <csop.h>
#include    <csstbopr.h>
#include    <display.h>
#include    <appllog.h>
#include    <csetk.h>
#include    <csstbetk.h>
#include    <cpm.h>
#include    <eept.h>
#include    "itmlocal.h"

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesDeptOpen(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales)
*
*    Input: *pUifRegSales
*
*   Output: None
*
*    InOut: *pItemSales
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_SEQERR_ADR : Function aborted with Illegal operation sequense error
*
** Description:  
*                
*===========================================================================
*/
SHORT   ItemSalesDeptOpen(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales)
{
    SHORT      sReturnStatus;
    DEPTIF     DeptRecRcvBuffer;


    if (pUifRegSales->uchAdjective != 0) {
        return(LDT_SEQERR_ADR);             /* return sequence error */
    }

	//US Customs cwunn 4/24/03 commented out check to allow use of price check on Open PLU
	//SCER issue #2 "Visual Verify with Preset Price Override"
    /* Was 0 sales amount entered ? 
    if (!(ItemSalesLocal.fbSalesStatus & SALES_ZERO_AMT) && pUifRegSales->ulUnitPrice == 0) {
            return(LDT_SEQERR_ADR);         /* return sequence error */
    //} 
    
    memset(&DeptRecRcvBuffer, '\0', sizeof(DEPTIF)); 
                                            /* initialize receive buffer */
    DeptRecRcvBuffer.usDeptNo = pItemSales->usDeptNo;
    /* DeptRecRcvBuffer.uchDeptNo = (UCHAR) pItemSales->uchDeptNo; */

    /*--- HALO CHECK ---*/
    /* get dept record */
    if ((sReturnStatus = CliOpDeptIndRead(&DeptRecRcvBuffer, 0)) != OP_PERFORM) {
        return(OpConvertError(sReturnStatus));
    } else {
        /* velify unit price vs HALO amount */
        if (RflHALO(pItemSales->lUnitPrice, *DeptRecRcvBuffer.ParaDept.auchHalo) != RFL_SUCCESS) {
            return(LDT_KEYOVER_ADR);        /* return sequence error */
        }
    }

	pItemSales->fsLabelStatus |= ITM_CONTROL_NO_CONSOL;  // do not consolidate open department and open PLU items.
    return(ITM_SUCCESS);
}
    
