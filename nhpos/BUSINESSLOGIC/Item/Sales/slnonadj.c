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
* Title       : Dept./ PLU sales common process                          
* Category    : Item Module, NCR 2170 US Hsopitality Model Application         
* Program Name: SLNONADJ.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: ItemSalesNonAdj() : Item sales Non adjective preset PLU process   
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver. Rev. :   Name    : Description
* May-30-92: 00.00.01  : K.Maeda   : Initial                                   
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
#include    <stdlib.h>
#include    <ecr.h>
#include    <uie.h> 
#include    <pif.h> 
#include    <regstrct.h>
#include    <transact.h>
#include    <item.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csstbpar.h>
#include    <csop.h>
#include    <csstbopr.h>
#include    <display.h>
#include    <csetk.h>
#include    <csstbetk.h>
#include    <cpm.h>
#include    <eept.h>
#include    "itmlocal.h"

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesNonAdj(ITEMSALES *pItemSales);
*
*    Input: *pItemSales
*           
*   Output: None
*
*    InOut: None
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_SEQERR_ADR : Function aborted with Illegal operation sequense error
*
** Description: This function  will be called at Non adjective preset PLU 
*               item soled and executes the following process.
*               - Prohibit 0 amount entry.
*
*               - Prohibits adjective valiation entry.
*
*===========================================================================
*/

SHORT   ItemSalesNonAdj(UIFREGSALES *pUifRegSales)
{


    /*----- CHECK 0 AMOUNT ENTRY -----*/

    if (ItemSalesLocal.fbSalesStatus & SALES_ZERO_AMT) { 
                                            /* 0 amount entry for non open PLU */
        return(LDT_SEQERR_ADR);
    }

    /*----- CHECK ADJ. VALIATION ENTRY -----*/

    if (pUifRegSales->uchAdjective != 0) {  /* adjective valiation was entered */
                                            /* for non adjective item          */
        return(LDT_SEQERR_ADR);
    }

    return(ITM_SUCCESS);

}



