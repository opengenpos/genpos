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
* Title       : Print Item  discount                 
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRDisc_.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct:
*      PrtItemDisc() : print items specified class "CLASS_ITEMDISC"
*      PrtDispDisc() : display items specified class "CLASS_ITEMDISC"
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* May-15-92 : 00.00.01 : K.Nakajima :                                    
* Jul-13-93 : 01.00.12 :  R.Itoh    : add PrtDispDisc()                                   
* Oct-28-93 : 02.00.02 :  K.You     : del. PrtDispDisc()                                   
* Jan-17-95 : 03.00.00 : M.Ozawa    : add PrtDispDisc() for display on the fly
* Nov-15-95 : 03.01.00 : M.Ozawa    : add CLASS_REGDISC3 - CLASS_REGDISC6
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
/**------- 2170 local------**/
#include	<tchar.h>
#include<ecr.h>
#include<regstrct.h>
#include<transact.h>
#include<prt.h>
#include"prtrin.h"

/*
*===========================================================================
** Format  : VOID   PrtItemDisc(TRANINFORMATION  *pTran, ITEMDISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMDISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints item classfied "CLASS_ITEMDISC" .
*===========================================================================
*/
VOID   PrtItemDisc(TRANINFORMATION  *pTran, ITEMDISC  *pItem)
{
    if (pItem->fbReduceStatus & REDUCE_ITEM) {
        return;
    }

    switch (pItem->uchMinorClass) {

    case CLASS_ITEMDISC1:               /* item discount */
    case CLASS_REGDISC1:                /* regular discount */
    case CLASS_REGDISC2:
    case CLASS_REGDISC3:
    case CLASS_REGDISC4:
    case CLASS_REGDISC5:
    case CLASS_REGDISC6:
        PrtItemRegDisc(pTran, pItem);
        break;

    case CLASS_CHARGETIP:               /* charge Tips */
    case CLASS_CHARGETIP2:              /* charge Tips, V3.3 */
    case CLASS_CHARGETIP3:              /* charge Tips, V3.3 */
    case CLASS_AUTOCHARGETIP:           /* charge Tips, V3.3 */
    case CLASS_AUTOCHARGETIP2:          /* charge Tips, V3.3 */
    case CLASS_AUTOCHARGETIP3:          /* charge Tips, V3.3 */
        PrtChrgTip(pTran, pItem);
        break;

    default:
        break;

    }

}

/*
*===========================================================================
** Format  : VOID   PrtDispDisc(TRANINFORMATION  *pTran, ITEMDISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMDISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays item classfied "CLASS_ITEMDISC" .
*===========================================================================
*/
VOID   PrtDispDisc(TRANINFORMATION  *pTran, ITEMDISC  *pItem)
{

    switch (pItem->uchMinorClass) {

    case CLASS_ITEMDISC1:               /* item discount */
    case CLASS_REGDISC1:                /* regular discount */
    case CLASS_REGDISC2:
    case CLASS_REGDISC3:
    case CLASS_REGDISC4:
    case CLASS_REGDISC5:
    case CLASS_REGDISC6:
        PrtDflRegDisc(pTran, pItem);
        break;

    case CLASS_CHARGETIP:               /* charge Tips */
    case CLASS_CHARGETIP2:              /* charge Tips, V3.3 */
    case CLASS_CHARGETIP3:              /* charge Tips, V3.3 */
    case CLASS_AUTOCHARGETIP:           /* charge Tips, V3.3 */
    case CLASS_AUTOCHARGETIP2:          /* charge Tips, V3.3 */
    case CLASS_AUTOCHARGETIP3:          /* charge Tips, V3.3 */
        PrtDflChrgTip(pTran, pItem);
        break;

    default:
        break;

    }

}


/*
*===========================================================================
** Format  : VOID   PrtDispDisc(TRANINFORMATION  *pTran, ITEMDISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMDISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays item classfied "CLASS_ITEMDISC" .
*===========================================================================
*/
USHORT   PrtDispDiscForm(TRANINFORMATION  *pTran, ITEMDISC  *pItem, TCHAR *puchBuffer)
{
    USHORT usLine;

    switch (pItem->uchMinorClass) {

    case CLASS_ITEMDISC1:               /* item discount */
    case CLASS_REGDISC1:                /* regular discount */
    case CLASS_REGDISC2:
    case CLASS_REGDISC3:
    case CLASS_REGDISC4:
    case CLASS_REGDISC5:
    case CLASS_REGDISC6:
        usLine = PrtDflRegDiscForm(pTran, pItem, puchBuffer);
        break;

    case CLASS_CHARGETIP:               /* charge Tips */
    case CLASS_CHARGETIP2:              /* charge Tips, V3.3 */
    case CLASS_CHARGETIP3:              /* charge Tips, V3.3 */
    case CLASS_AUTOCHARGETIP:           /* charge Tips, V3.3 */
    case CLASS_AUTOCHARGETIP2:          /* charge Tips, V3.3 */
    case CLASS_AUTOCHARGETIP3:          /* charge Tips, V3.3 */
        usLine = PrtDflChrgTipForm(pTran, pItem, puchBuffer);
        break;

    default:
        usLine = 0;
        break;

    }

    return usLine;
}


/***** End Of Source *****/
