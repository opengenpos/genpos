/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print Item  misc                 
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRMiscT.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:
*      PrtItemMisc() : print items specified class "CLASS_ITEMMISC"
*      PrtDispMisc() : display items specified class "CLASS_ITEMMISC"
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-10-93 : 01.00.12 :  R.Itoh    : initial                                   
* Jun-30-93 : 01.00.12 :  R.Itoh    : ADD CLASS_ETKIN, CLASS_ETKOUT                                   
* Jul-13-93 : 01.00.12 :  R.Itoh    : add PrtDispMisc()                                   
* Oct-29-93 : 02.00.02 :  K.You     : del. PrtDispMisc()                                   
* Jan-26-94 : 03.00.00 :  M.Ozawa   : recover PrtDispMisc() 
* Sep-16-98 : 03.03.00 :  hrkato    : V3.3(Check Transfer Print)
* Aug-12-99 : 03.05.00 :  K.Iwata   : marge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
* Nov-29-99 : 01.00.00 :  hrkato    : Saratoga (Money)
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
/**------- 2170 local------**/
#include "ecr.h"
#include "regstrct.h"
#include "transact.h"
#include "prt.h"
#include "prtrin.h"

/*
*===========================================================================
** Format  : VOID   PrtItemMisc(TRANINFORMATION  *pTran, ITEMMISC *pItem);
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints item classfied "CLASS_ITEMMISC"   V3.3
*===========================================================================
*/
VOID   PrtItemMisc(TRANINFORMATION  *pTran, ITEMMISC  *pItem)
{
    switch (pItem->uchMinorClass) {
    case CLASS_PO:                      /* paid out */
        PrtPaidOut(pTran, pItem);
        break;

    case CLASS_RA:                      /* received on acount */
        PrtRecvOnAcnt(pTran, pItem);
        break;

    case CLASS_TIPSDECLARED:            /* tips declared */
        PrtTipsDecld(pTran, pItem);
        break;

    case CLASS_TIPSPO:                  /* tips paid out */
        PrtTipsPaidOut(pTran, pItem);
        break;

    case CLASS_CHECKTRANSFER:           /* check transfer */
    case CLASS_CHECKTRANS_FROM:         /* check transfer from, V3.3 */
    case CLASS_CHECKTRANS_TO:           /* check transfer to,   V3.3 */
        PrtCheckTran(pTran, pItem);
        break;

    case CLASS_NOSALE:                  /* no sale */
        PrtNoSale(pTran, pItem);
        break;

    case CLASS_CANCEL:                  /* cancel */
        PrtCancel(pTran, pItem);
        break;

    case CLASS_SIGNIN:                 /* Operator Sign in */
		fsPrtStatus |= PRT_EJ_JOURNAL;   // if printing CLASS_SIGNIN then we force to EJ only
    case CLASS_ETKIN:                  /* ETK time in */
        PrtEtkIn(pTran, pItem);
        break;

    case CLASS_SIGNOUT:                /* Operator Sign out */
		fsPrtStatus |= PRT_EJ_JOURNAL;   // if printing CLASS_SIGNOUT then we force to EJ only
    case CLASS_ETKOUT:                 /* ETK time out */
        PrtEtkOut(pTran, pItem, PRT_WTIN);
        break;

    case CLASS_ETKOUT_WOETKIN:         /* ETK time out w/o time-in */
        PrtEtkOut(pTran, pItem, PRT_WOIN);
        break;

    case CLASS_MONEY:                   /* money,   Saratoga */
        PrtMoney(pTran, pItem);
        break;

    default:
        break;
    }
}

/*
*===========================================================================
** Format  : VOID   PrtDispMisc(TRANINFORMATION  *pTran, ITEMMISC *pItem);
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints item classfied "CLASS_ITEMMISC"
*===========================================================================
*/
VOID   PrtDispMisc(TRANINFORMATION  *pTran, ITEMMISC  *pItem)
{
    switch (pItem->uchMinorClass) {
    case CLASS_PO:                      /* paid out */
        PrtDflPaidOut(pTran, pItem);
        break;

    case CLASS_RA:                      /* received on acount */
        PrtDflRecvOnAcnt(pTran, pItem); 
        break;

    case CLASS_TIPSDECLARED:            /* tips declared */
        PrtDflTipsDecld(pTran, pItem); 
        break;

    case CLASS_TIPSPO:                  /* tips paid out */
        PrtDflTipsPaidOut(pTran, pItem); 
        break;

    case CLASS_CHECKTRANSFER:           /* check transfer */
    case CLASS_CHECKTRANS_FROM:         /* check transfer from, V3.3 */
    case CLASS_CHECKTRANS_TO:           /* check transfer to,   V3.3 */
        PrtDflCheckTran(pTran, pItem); 
        break;

    case CLASS_NOSALE:                  /* no sale */
        PrtDflNoSale(pTran, pItem); 
        break;

    case CLASS_CANCEL:                  /* cancel */
        PrtDflCancel(pTran, pItem); 
        break;

    default:
        break;
    }
}


/*
*===========================================================================
** Format  : VOID   PrtDispMiscForm(TRANINFORMATION  *pTran, ITEMMISC *pItem)
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints item classfied "CLASS_ITEMMISC"
*===========================================================================
*/
USHORT   PrtDispMiscForm(TRANINFORMATION  *pTran, ITEMMISC  *pItem, TCHAR *puchBuffer)
{
    USHORT usLine;
    
    switch (pItem->uchMinorClass) {

    case CLASS_PO:                      /* paid out */
        usLine = PrtDflPaidOutForm(pTran, pItem, puchBuffer);
        break;

    case CLASS_RA:                      /* received on acount */
        usLine = PrtDflRecvOnAcntForm(pTran, pItem, puchBuffer);
        break;
    
    case CLASS_TIPSDECLARED:            /* tips declared */
        usLine = PrtDflTipsDecldForm(pTran, pItem, puchBuffer); 
        break;

    case CLASS_TIPSPO:                  /* tips paid out */
        usLine = PrtDflTipsPaidOutForm(pTran, pItem, puchBuffer);
        break;

/*
    case CLASS_CHECKTRANSFER:           / check transfer /
    case CLASS_CHECKTRANS_FROM:         / check transfer from, V3.3 /
    case CLASS_CHECKTRANS_TO:           / check transfer to,   V3.3 /
        PrtDflCheckTran(pTran, pItem); 
        break;
*/
    case CLASS_NOSALE:                  /* no sale */
        usLine = PrtDflNoSaleForm(pTran, pItem, puchBuffer); 
        break;

    case CLASS_CANCEL:                  /* cancel */
        usLine = PrtDflCancelForm(pTran, pItem, puchBuffer);
        break;

    default:
        usLine = 0;
        break;
    }
    
    return usLine;
}

/***** End Of Source *****/