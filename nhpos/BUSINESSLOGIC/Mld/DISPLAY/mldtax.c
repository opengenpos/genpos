/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS, E&M OISO                **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Multiline Display Tax                 
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDTAX.C                     
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:
*      MldItemAffection() : display items specified class "CLASS_ITEMAFFECTION"
*      MldTax()           : display tax
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Mar-13-95 : 03.00.00 : M.Ozawa    : Initial
* Aug-16-99 : 03.05.00 : M.Teraki   : Merge STORE_RECALL_MODEL
*           :          :            :       /GUEST_CHECK_MODEL
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
#include<paraequ.h>
#include<para.h>
#include<csstbpar.h>
#include<regstrct.h>
#include<transact.h>
#include<mld.h>
#include"mldlocal.h"
#include"mldrin.h"
#include "termcfg.h"

/*
*===========================================================================
** Format  : VOID   MldItemAffection(ITEMAFFECTION *pItem, USHORT usType,
*                                   USHORT usScroll);
*   Input  : ITEMAFFECTION    *pItem     -Item Data address
*            USHORT           usType     -First display position
*            USHORT           usScroll   -Destination scroll display
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays tax and addcheck .
*            
*===========================================================================
*/
SHORT  MldItemAffection(ITEMAFFECTION  *pItem, USHORT usType, USHORT usScroll)
{
    SHORT sStatus;

    switch (pItem->uchMinorClass) {

    case CLASS_TAXPRINT:                     /* tax */
        sStatus = MldTax(pItem, usType, usScroll);
        break;

    case CLASS_ADDCHECKTOTAL:                /* auto addcheck total */
    case CLASS_MANADDCHECKTOTAL:             /* manual addcheck total */
        sStatus = MldAddChk2(pItem, usType, usScroll);
        break;

    /* V3.3 */
    case CLASS_VAT_SUBTOTAL:                    /* VAT subtotal */
        sStatus = MldVATSubtotal(pItem, usType, usScroll);
        break;

    case CLASS_SERVICE_VAT:                     /* VATable service */
    case CLASS_SERVICE_NON:                     /* non-VATable service */
        sStatus = MldVATServ(pItem, usType, usScroll);
        break;

    case CLASS_VATAFFECT:                       /* VAT */
        sStatus = MldVAT(pItem, usType, usScroll);
        break;

    default:
        sStatus = MldDummyScrollDisplay(usType, usScroll);
        break;
    }
    return (sStatus);
}

/*
*===========================================================================
** Format  : VOID   MldTax(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll);   
*   Input  : ITEMAFFECTION    *pItem     -Item Data address
*            USHORT           usType     -First display position
*            USHORT           usScroll   -Specified scroll display
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays tax.
*===========================================================================
*/
SHORT MldTax(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll)
{
    if (CliParaMDCCheck(MDC_TAX_ADR, ODD_MDC_BIT0)) {

        return (MldCanadaTax(pItem, usType, usScroll));

    } else {

        return (MldUSTax(pItem, usType, usScroll));
    }
}


/***** End Of Source *****/
