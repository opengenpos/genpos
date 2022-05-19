/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print Item  Dept PLU
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrtRSale_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract:
*      PrtItemSales() : print items specified class "CLASS_ITEMSALES"
*      PrtDispSales() : display items specified class "CLASS_ITEMSALES"
*      PrtDispSalesForm() : display items specified class "CLASS_ITEMSALES"
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
* May-15-92 : 00.00.01 : K.Nakajima :
* Jul-10-93 : 01.00.12 :  R.Itoh    : Add PrtDispSales()
* Oct-28-93 : 02.00.02 :  K.You     : Del. PrtDispSales()
* Jan-17-95 : 03.00.00 :  M.Ozawa   : add PrtDispSales() for display on the fly
* Mar-22-95 : 03.00.00 : T.Nakahata : add OEP item
* Jun-05-96 : 03.01.08 :  M.Ozawa   : add "not print plus item check" for void consolidation
* Dec-16-99 : 01.00.00 : hrkato     : Saratoga
* Dec-14-99 : 00.00.01 :  M.Ozawa   : Add PrtDispSalesForm()
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
;========================================================================
;+                  I N C L U D E     F I L E s                         +
;========================================================================
**/
#include	<tchar.h>
#include<string.h>
/**------- 2170 local------**/
#include <ecr.h>
#include <regstrct.h>
#include <transact.h>
#include <rfl.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include "prtrin.h"

/*
*===========================================================================
** Format  : VOID   PrtItemSales(TRANINFORMATION  *pTran, ITEMSALES *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMSALES        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints dept sales.
*===========================================================================
*/
VOID   PrtItemSales(TRANINFORMATION  *pTran, ITEMSALES  *pItem)
{

    if (pItem->lQTY == 0L) return;  /* for after the split check tender */

    if (pItem->fbReduceStatus & REDUCE_ITEM) {  /* transfered item */

        return;
    }

    /* turn off journal bit, if not print plus item option */
    if (CliParaMDCCheck(MDC_DEPT1_ADR, ODD_MDC_BIT3)) {
        if (!((pItem->ControlCode.auchPluStatus[0] & PLU_MINUS) ||
              (pTran->TranCurQual.fsCurStatus & CURQUAL_PVOID) ||
              (pItem->fbModifier & VOID_MODIFIER) ||
              (pItem->uchMinorClass == CLASS_DEPTMODDISC) ||
              (pItem->uchMinorClass == CLASS_PLUMODDISC) ||
              (pItem->uchMinorClass == CLASS_SETMODDISC)) ) { 

            pItem->fsPrintStatus &= ~PRT_JOURNAL;  /* turn off journal bit */
        }
    }

    switch (pItem->uchMinorClass) {
    case CLASS_PLU:                     /* PLU */
    case CLASS_PLUITEMDISC:
    case CLASS_SETMENU:                 /* setmenu */
    case CLASS_SETITEMDISC:

        if ( !(pItem->fsPrintStatus & PRT_VALIDATION) ) {    /* not validation print */

            /* ---- In case of PLU# 9951-9999 -> only K/P print, 50RFC05513 --- */
            if ((pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2] & PLU_SALES_KP_NO_1_4) == 0  /* Check K/P Status */
                && (pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_6] & PLU_SALES_KP_NO_5_8) == 0) {

                if (PrtChkPrintPluNo(pItem) == FALSE) {
                    return;
                }
            }
        }
    }

    switch (pItem->uchMinorClass) {

    case CLASS_DEPT:                    /* dept */
    case CLASS_DEPTITEMDISC:
        PrtDept(pTran, pItem);
        break;

    case CLASS_PLU:                     /* PLU */
    case CLASS_PLUITEMDISC:
        PrtPLU(pTran, pItem);
        break;

    case CLASS_SETMENU:                 /* setmenu */
    case CLASS_SETITEMDISC:
    case CLASS_OEPPLU:                  /* OEP item */
    case CLASS_OEPITEMDISC:
        PrtSET(pTran, pItem);
        break;

    case CLASS_DEPTMODDISC:             /* dept modifiered disc. */
    case CLASS_PLUMODDISC:              /* plu modifiered disc. */
    case CLASS_SETMODDISC:              /* setmenu modifiered disc. */
    case CLASS_OEPMODDISC:              /* OEP modifiered disc. */
        PrtModDisc(pTran, pItem);
        break;

    default:
        break;

    }

}

/*
*===========================================================================
** Format  : VOID   PrtDispSales(TRANINFORMATION  *pTran, ITEMSALES *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMSALES        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays sales item.
*===========================================================================
*/
VOID   PrtDispSales(TRANINFORMATION  *pTran, ITEMSALES  *pItem)
{

    switch (pItem->uchMinorClass) {
    case CLASS_PLU:                     /* PLU */
    case CLASS_PLUITEMDISC:
    case CLASS_SETMENU:                 /* setmenu */
    case CLASS_SETITEMDISC:

        if ((_tcsncmp(pItem->auchPLUNo, MLD_NO_DISP_PLU_LOW, NUM_PLU_LEN) >= 0) &&
            (_tcsncmp(pItem->auchPLUNo, MLD_NO_DISP_PLU_HIGH, NUM_PLU_LEN) <= 0)) {
        /* if (pItem->usPLUNo >= PRT_NOT_PRINT_PLU_NO) { */
            if ((pItem->uchPrintModNo + pItem->uchCondNo ) == 0) {
                return;
            }
        }
    }

    switch (pItem->uchMinorClass) {

    case CLASS_DEPT:                    /* dept */
    case CLASS_DEPTITEMDISC:
    case CLASS_PLU:                     /* PLU */
    case CLASS_PLUITEMDISC:
    case CLASS_OEPPLU:                  /* OEP item */
    case CLASS_OEPITEMDISC:
        PrtDflDeptPLU(pTran, pItem);
        break;

    case CLASS_SETMENU:                 /* setmenu */
    case CLASS_SETITEMDISC:
        PrtDflSET(pTran, pItem);
        break;

    case CLASS_DEPTMODDISC:             /* dept modifiered disc. */
    case CLASS_PLUMODDISC:              /* plu modifiered disc. */
        PrtDflModDisc(pTran, pItem);
        break;

    case CLASS_SETMODDISC:              /* setmenu modifiered disc. */
    case CLASS_OEPMODDISC:              /* OEP modifiered disc. */
        PrtDflSetModDisc(pTran, pItem);
        break;

    default:
        break;

    }

}

/*
*===========================================================================
** Format  : VOID   PrtDispSales(TRANINFORMATION  *pTran, ITEMSALES *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMSALES        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays sales item.
*===========================================================================
*/
USHORT   PrtDispSalesForm(TRANINFORMATION  *pTran, ITEMSALES  *pItem, TCHAR *puchBuffer)
{
    USHORT usLine;

#if 0
    switch (pItem->uchMinorClass) {
    case CLASS_PLU:                     /* PLU */
    case CLASS_PLUITEMDISC:
    case CLASS_SETMENU:                 /* setmenu */
    case CLASS_SETITEMDISC:

        if ((_tcsncmp(pItem->auchPLUNo, MLD_NO_DISP_PLU_LOW, NUM_PLU_LEN) >= 0) &&
            (_tcsncmp(pItem->auchPLUNo, MLD_NO_DISP_PLU_HIGH, NUM_PLU_LEN) <= 0)) {
        /* if (pItem->usPLUNo >= PRT_NOT_PRINT_PLU_NO) { */
            if ((pItem->uchPrintModNo + pItem->uchCondNo ) == 0) {
                return;
            }
        }
    }
#endif
    switch (pItem->uchMinorClass) {

    case CLASS_DEPT:                    /* dept */
    case CLASS_DEPTITEMDISC:
    case CLASS_PLU:                     /* PLU */
    case CLASS_PLUITEMDISC:
        usLine = PrtDflDeptPLUForm(pTran, pItem, puchBuffer);
        break;

    case CLASS_SETMENU:                 /* setmenu */
    case CLASS_SETITEMDISC:
        usLine = PrtDflSETForm(pTran, pItem, puchBuffer);
        break;

    case CLASS_DEPTMODDISC:             /* dept modifiered disc. */
    case CLASS_PLUMODDISC:              /* plu modifiered disc. */
        usLine = PrtDflModDiscForm(pTran, pItem, puchBuffer);
        break;

    case CLASS_SETMODDISC:              /* setmenu modifiered disc. */
        usLine = PrtDflSetModDiscForm(pTran, pItem, puchBuffer);
        break;

    default:
        usLine = 0;
        break;

    }

    return usLine;
}

/***** End Of Source *****/