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
* Title       : Multiline Display US Tax                 
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDTAX.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               MldUSTax() : diplays tax and addcheck
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Mar-13-95 : 03.00.00 : M.Ozawa    : Initial
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
/**------- MS - C ---------**/
#include	<tchar.h>
#include <string.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <paraequ.h>
#include <para.h>
#include <pmg.h>
#include <regstrct.h>
#include <transact.h>
#include <mld.h>
#include "mldlocal.h"
#include "mldrin.h"
#include "termcfg.h"

/*
*===========================================================================
** Format  : VOID MldUSTax(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll);
*            
*   Input  : ITEMAFFECTION    *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays us tax. 
*===========================================================================
*/
SHORT MldUSTax(ITEMAFFECTION  *pItem, USHORT usType, USHORT usScroll)
{
    if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {

        return (MldDriveUSTax(pItem, usType, usScroll));

    } else {

        return (MldPrechkUSTax(pItem, usType, usScroll));

    }
}

/*
*===========================================================================
** Format  : VOID MldUSTax(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll);
*            
*   Input  : ITEMAFFECTION    *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays us tax. 
*===========================================================================
*/
SHORT MldPrechkUSTax(ITEMAFFECTION  *pItem, USHORT usType, USHORT usScroll)
{
    TCHAR  aszScrollBuff[4][MLD_PRECOLUMN1 + 1]; /* print data save area */
    LONG   alAttrib[4][MLD_PRECOLUMN1 + 1];                /* reverse video control flag area */
    USHORT  usScrollLine = 0;            /* number of lines to be printed */
    USCANTAX    *pUSCanTax;

    /* -- set US tax structuer, V3.3 -- */
    pUSCanTax = &(pItem->USCanVAT.USCanTax);

    /* -- initialize the buffer -- */
    memset(aszScrollBuff[0], '\0', sizeof(aszScrollBuff));
    memset(&alAttrib[0][0], 0x00, sizeof(alAttrib));

    /* -- set subtotal line R3.1 -- */
    if (pItem->lAmount) { 
        if (pUSCanTax->lTaxAmount[0]||pUSCanTax->lTaxAmount[1]||pUSCanTax->lTaxAmount[2]) {
            usScrollLine += MldMnemAmt(aszScrollBuff[0], TRN_TTL1_ADR, pItem->lAmount, MLD_PRECHECK_SYSTEM);
        }
    }

    /* -- set tax mnemonic and amount -- */
    if (pUSCanTax->lTaxAmount[0]) { 
        usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_TX1_ADR, pUSCanTax->lTaxAmount[0], MLD_PRECHECK_SYSTEM);
    }
    if (pUSCanTax->lTaxAmount[1]) { 
        usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_TX2_ADR, pUSCanTax->lTaxAmount[1], MLD_PRECHECK_SYSTEM);
    }
    if (pUSCanTax->lTaxAmount[2]) { 
        usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_TX3_ADR, pUSCanTax->lTaxAmount[2], MLD_PRECHECK_SYSTEM);
    }

    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_PRECOLUMN1, &alAttrib[0][0]));
}

/*
*===========================================================================
** Format  : VOID MldUSTax(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll);
*            
*   Input  : ITEMAFFECTION    *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays us tax. 
*===========================================================================
*/
SHORT MldDriveUSTax(ITEMAFFECTION  *pItem, USHORT usType, USHORT usScroll)
{
    TCHAR  aszScrollBuff[8][MLD_DR3COLUMN + 1]; /* print data save area */
    LONG   alAttrib[8][MLD_PRECOLUMN1 + 1];                /* reverse video control flag area */
    USHORT  usScrollLine = 0;            /* number of lines to be printed */
    USCANTAX    *pUSCanTax;

    /* -- set US tax structuer, V3.3 -- */
    pUSCanTax = &(pItem->USCanVAT.USCanTax);

    /* -- initialize the buffer -- */
    memset(aszScrollBuff[0], '\0', sizeof(aszScrollBuff));
    memset(&alAttrib[0][0], 0x00, sizeof(alAttrib));

    /* -- set subtotal line R3.1 -- */
    if (pItem->lAmount) { 
        if (pUSCanTax->lTaxAmount[0]||pUSCanTax->lTaxAmount[1]||pUSCanTax->lTaxAmount[2]) {
            usScrollLine += MldMnemAmt(aszScrollBuff[0], TRN_TTL1_ADR, pItem->lAmount, MLD_DRIVE_THROUGH_3SCROLL);
        }
    }

    /* -- set tax mnemonic and amount -- */
    if (pUSCanTax->lTaxAmount[0]) { 
        usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_TX1_ADR, pUSCanTax->lTaxAmount[0], MLD_DRIVE_THROUGH_3SCROLL);
    }
    if (pUSCanTax->lTaxAmount[1]) { 
        usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_TX2_ADR, pUSCanTax->lTaxAmount[1], MLD_DRIVE_THROUGH_3SCROLL);
    }
    if (pUSCanTax->lTaxAmount[2]) { 
        usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_TX3_ADR, pUSCanTax->lTaxAmount[2], MLD_DRIVE_THROUGH_3SCROLL);
    }

    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_DR3COLUMN, &alAttrib[0][0]));
}

/***** End Of Source *****/
