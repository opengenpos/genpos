/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print Tax                 
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRTaxT.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:
*      PrtItemAffection() : print items specified class "CLASS_ITEMAFFECTION"
*      PrtTax()           : print tax
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-11-93 : 01.00.12 : R.Itoh     : initial                                   
* Jul-15-93 : 01.00.12 : R.Itoh     : add PrtDispAffection(), PrtDflTax()                                   
* Oct-29-93 : 02.00.02 : K.You      : del. PrtDispAffection(), PrtDflTax()                                   
* Jan-26-95 : 03.00.00 : M.Ozawa    : add PrtDispAffection(), PrtDflTax()                                   
* Aug-12-99 : 03.05.00 : K.Iwata    : Marge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
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
#include<ecr.h>
#include<regstrct.h>
#include<transact.h>
#include<prt.h>
#include"prtrin.h"

/*
*===========================================================================
** Format  : VOID   PrtItemAffection(TRANINFORMATION  *pTran,
*                                     ITEMAFFECTION *pItem);   
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints tax and addcheck .
*            
*===========================================================================
*/
VOID   PrtItemAffection(TRANINFORMATION  *pTran, ITEMAFFECTION  *pItem)
{

    switch (pItem->uchMinorClass) {

    case CLASS_TAXPRINT:                     /* tax */
        PrtTax(pTran, pItem);
        break;


    
    case CLASS_ADDCHECKTOTAL:                /* auto addcheck total */
    case CLASS_MANADDCHECKTOTAL:             /* manual addcheck total */
        PrtAddChk2(pTran, pItem);
        break;

    /* V3.3 */
    case CLASS_VAT_SUBTOTAL:                   /* VAT subtotal */
        PrtVATSubtotal(pTran, pItem);
        break;

    case CLASS_SERVICE_VAT:                     /* VATable service */
    case CLASS_SERVICE_NON:                     /* non-VATable service */
        PrtVATServ(pTran, pItem);
        break;

    case CLASS_VATAFFECT:                       /* VAT */
        PrtVAT(pTran, pItem);
        break;

    default:
        break;
    }

}

/*
*===========================================================================
** Format  : VOID   PrtTax(TRANINFORMATION  *pTran,
*                           ITEMAFFECTION *pItem);   
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints tax.
*===========================================================================
*/
VOID PrtTax(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem)
{

    SHORT   fsStatusSave;

    fsStatusSave = fsPrtStatus;

    if (pTran->TranModeQual.fsModeStatus & MODEQUAL_CANADIAN) {

        PrtCanadaTax(pTran, pItem);

    } else {

        PrtUSTax(pTran, pItem);

    }
    
    /* --- set fsPrtStatus flag to control "0.00" total line print --- */

    if (pItem->USCanVAT.USCanTax.lTaxAmount[0] ||   /* V3.3 */
        pItem->USCanVAT.USCanTax.lTaxAmount[1] ||
        pItem->USCanVAT.USCanTax.lTaxAmount[2] ||
        pItem->USCanVAT.USCanTax.lTaxAmount[3]) {

        fsPrtStatus |= PRT_REC_SLIP;   /* print total line */

    } else
    if (!(fsStatusSave & PRT_REC_SLIP)) {   /* anything is printed */

        fsPrtStatus &= ~PRT_REC_SLIP;   /* not print "0.00" total line */
    }

}


/*
*===========================================================================
** Format  : VOID   PrtDispAffection(TRANINFORMATION  *pTran,
*                                     ITEMAFFECTION *pItem);   
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays tax and addcheck .
*            
*===========================================================================
*/
VOID   PrtDispAffection(TRANINFORMATION  *pTran, ITEMAFFECTION  *pItem)
{

    switch (pItem->uchMinorClass) {

    case CLASS_TAXPRINT:                     /* tax */
        PrtDflTax(pTran, pItem);
        break;


    
    case CLASS_ADDCHECKTOTAL:                /* auto addcheck total */
    case CLASS_MANADDCHECKTOTAL:             /* manual addcheck total */
        PrtDflAddChk2(pTran, pItem);
        break;

    /* V3.3 */
    case CLASS_VAT_SUBTOTAL:                     /* VAT subtotal */
        PrtDflVATSubtotal(pTran, pItem);
        break;

    case CLASS_SERVICE_VAT:                     /* VATable service */
    case CLASS_SERVICE_NON:                     /* non-VATable service */
        PrtDflVATServ(pTran, pItem);
        break;

    case CLASS_VATAFFECT:                       /* VAT */
        PrtDflVAT(pTran, pItem);
        break;

    default:
        break;
    }

}

/*
*===========================================================================
** Format  : VOID   PrtDflTax(TRANINFORMATION  *pTran,
*                           ITEMAFFECTION *pItem);   
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays tax.
*===========================================================================
*/
VOID PrtDflTax(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem)
{

    if (pTran->TranModeQual.fsModeStatus & MODEQUAL_CANADIAN) {

        PrtDflCanadaTax(pTran, pItem);

    } else {

        PrtDflUSTax(pTran, pItem);

    }
    
}

/*
*===========================================================================
** Format  : VOID   PrtDispAffection(TRANINFORMATION  *pTran,
*                                     ITEMAFFECTION *pItem);   
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays tax and addcheck .
*            
*===========================================================================
*/
USHORT   PrtDispAffectionForm(TRANINFORMATION  *pTran, ITEMAFFECTION  *pItem, TCHAR *puchBuffer)
{
    USHORT usLine;

    switch (pItem->uchMinorClass) {

    case CLASS_TAXAFFECT:
    /* case CLASS_TAXPRINT:                     / tax */
        usLine = PrtDflTaxForm(pTran, pItem, puchBuffer);
        break;

/*
    case CLASS_ADDCHECKTOTAL:                / auto addcheck total /
    case CLASS_MANADDCHECKTOTAL:             / manual addcheck total /
        usLine = PrtDflAddChk2Form(pTran, pItem, puchBuffer);
        break;
}*/
    /* V3.3 */
/*
    case CLASS_VAT_SUBTOTAL:                     / VAT subtotal /
        usLine = PrtDflVATSubtotal(pTran, pItem, puchBuffer);
        break;
*/
    case CLASS_SERVICE_VAT:                     /* VATable service */
    case CLASS_SERVICE_NON:                     /* non-VATable service */
        usLine = PrtDflVATServForm(pTran, pItem, puchBuffer);
        break;

    case CLASS_VATAFFECT:                       /* VAT */
        usLine = PrtDflVATForm(pTran, pItem, puchBuffer);
        break;

    default:
        usLine = 0;
        break;
    }
    
    return usLine;

}

/*
*===========================================================================
** Format  : VOID   PrtDflTax(TRANINFORMATION  *pTran,
*                           ITEMAFFECTION *pItem);   
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays tax.
*===========================================================================
*/
USHORT PrtDflTaxForm(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem, TCHAR *puchBuffer)
{
    USHORT usLine;
    
    usLine = PrtDflUSCanTaxForm(pTran, pItem, puchBuffer);
    
    return usLine;

/*
    if (pTran->TranModeQual.fsModeStatus & MODEQUAL_CANADIAN) {

        PrtDflCanadaTax(pTran, pItem);

    } else {

        PrtDflUSTax(pTran, pItem);

    }
*/
}

/***** End Of Source *****/
