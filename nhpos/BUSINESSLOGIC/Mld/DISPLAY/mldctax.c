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
* Title       : Multiline Display Canada tax                 
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDCTAX.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*           MldCanadaTax() : display canada tax
*           MldCanGst() : display canada Gst Pst tax
*           MldCanInd() : display canada individual tax 
*           MldCanAll() : display canada CanAll tax
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Dec-20-95 : 03.01.00 : M.Ozawa    : initial
*           :          :            :                                    
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
#include <csstbpar.h>
#include <regstrct.h>
#include <transact.h>
#include <mld.h>
#include "mldlocal.h"
#include "mldrin.h"
#include "termcfg.h"

/*
*===========================================================================
** Format  : SHORT MldCanadaTax(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll);
*            
*   Input  : ITEMAFFECTION    *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays canada tax. 
*===========================================================================
*/
SHORT MldCanadaTax(ITEMAFFECTION  *pItem, USHORT usType, USHORT usScroll)
{
    if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {

        return (MldDriveCanadaTax(pItem, usType, usScroll));

    } else {

        return (MldPrechkCanadaTax(pItem, usType, usScroll));

    }
}

/*
*===========================================================================
** Format  : SHORT   MldCanadaTax(ITEMAFFECTION *pItem, usType, usScroll);   
*   Input  : ITEMAFFECTION    *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays canada tax.
*===========================================================================
*/      
SHORT   MldPrechkCanadaTax(ITEMAFFECTION  *pItem, USHORT usType, USHORT usScroll)
{
    PARAMDC ParaMDC;

    ParaMDC.uchMajorClass = CLASS_PARAMDC;
    ParaMDC.usAddress     = MDC_CANTAX_ADR;
    CliParaRead(&ParaMDC);

    if ((ParaMDC.uchMDCData & (EVEN_MDC_BIT2|EVEN_MDC_BIT1)) == EVEN_MDC_BIT1) {

         return(MldPrechkCanGst(pItem, usType, usScroll));

    } else if ((ParaMDC.uchMDCData & (EVEN_MDC_BIT2|EVEN_MDC_BIT1)) == EVEN_MDC_BIT2) {

         return(MldPrechkCanInd(pItem, usType, usScroll));

    }  else {

         return(MldPrechkCanAll(pItem, usType, usScroll));

    }
}

/*
*===========================================================================
** Format  : SHORT   MldCanGst(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll); 
*            
*   Input  : ITEMAFFECTION    *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays canada tax.
*===========================================================================
*/
SHORT  MldPrechkCanGst(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll)
{
    TCHAR  aszScrollBuff[3][MLD_PRECOLUMN1 + 1]; /* print data save area */
    LONG   alAttrib[3][MLD_PRECOLUMN1 + 1];                /* reverse video control flag area */
    USHORT  usScrollLine = 0;            /* number of lines to be printed */
    USCANTAX    *pUSCanTax;

    /* -- set US tax structuer, V3.3 -- */
    pUSCanTax = &(pItem->USCanVAT.USCanTax);

    /* -- initialize the buffer -- */
    memset(aszScrollBuff[0], '\0', sizeof(aszScrollBuff));
    memset(&alAttrib[0][0], 0x00, sizeof(alAttrib));

    /* -- set subtotal line R3.1 -- */
    if (pItem->lAmount) { 
        if (pUSCanTax->lTaxAmount[0]||pUSCanTax->lTaxAmount[1]) {
            usScrollLine += MldMnemAmt(aszScrollBuff[0], TRN_TTL1_ADR, pItem->lAmount, MLD_PRECHECK_SYSTEM);
        }
    }

    /* -- set GST tax mnemonic and amount -- */
    if (pUSCanTax->lTaxAmount[0]) {
        usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_TX1_ADR, pUSCanTax->lTaxAmount[0], MLD_PRECHECK_SYSTEM);
    }

    /* -- set PST tax mnemonic and amount -- */
    if (pUSCanTax->lTaxAmount[1]) {
        usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_TX2_ADR, pUSCanTax->lTaxAmount[1], MLD_PRECHECK_SYSTEM);
    }

    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_PRECOLUMN1, &alAttrib[0][0]));
}

/*
*===========================================================================
** Format  : SHORT  MldCanInd(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll);
*
*   Input  : ITEMAFFECTION    *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays canada individual tax.
*===========================================================================
*/
SHORT  MldPrechkCanInd(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll)
{
    TCHAR  aszScrollBuff[5][MLD_PRECOLUMN1 + 1]; /* print data save area */
    LONG   alAttrib[5][MLD_PRECOLUMN1 + 1];                /* reverse video control flag area */
    USHORT  usScrollLine = 0;            /* number of lines to be printed */
    USCANTAX    *pUSCanTax;

    /* -- set US tax structuer, V3.3 -- */
    pUSCanTax = &(pItem->USCanVAT.USCanTax);

    /* -- initialize the buffer -- */
    memset(aszScrollBuff[0], '\0', sizeof(aszScrollBuff));
    memset(&alAttrib[0][0], 0x00, sizeof(alAttrib));

    /* -- set subtotal line R3.1 -- */
    if (pItem->lAmount) { 
        if (pUSCanTax->lTaxAmount[0]||pUSCanTax->lTaxAmount[1] || pUSCanTax->lTaxAmount[2]||pUSCanTax->lTaxAmount[3]) {
            usScrollLine += MldMnemAmt(aszScrollBuff[0], TRN_TTL1_ADR, pItem->lAmount, MLD_PRECHECK_SYSTEM);
        }
    }

    /* -- set GST tax mnemonic and amount -- */
    if (pUSCanTax->lTaxAmount[0]) {
        usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_TX1_ADR, pUSCanTax->lTaxAmount[0], MLD_PRECHECK_SYSTEM);
    }

    /* -- PST1 tax mnemonic and amount -- */
    if (pUSCanTax->lTaxAmount[1]) {
        usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_TX2_ADR, pUSCanTax->lTaxAmount[1], MLD_PRECHECK_SYSTEM);
    }

    /* -- PST2 tax mnemonic and amount -- */
    if (pUSCanTax->lTaxAmount[2]) {
        usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_TX3_ADR, pUSCanTax->lTaxAmount[2], MLD_PRECHECK_SYSTEM);
    }

    /* -- PST3 tax mnemonic and amount -- */
    if (pUSCanTax->lTaxAmount[3]) {
        usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_TX4_ADR, pUSCanTax->lTaxAmount[3], MLD_PRECHECK_SYSTEM);
    }

    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_PRECOLUMN1, &alAttrib[0][0]));
}

/*
*===========================================================================
** Format  : SHORT  MldCanAll(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll);
*
*   Input  : ITEMAFFECTION    *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays canada all tax.
*===========================================================================
*/
SHORT  MldPrechkCanAll(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll)
{
    TCHAR  aszScrollBuff[2][MLD_PRECOLUMN1 + 1]; /* print data save area */
    LONG   alAttrib[2][MLD_PRECOLUMN1 + 1];                  /* reverse video control flag area */
    USHORT  usScrollLine = 0;            /* number of lines to be printed */
    USCANTAX    *pUSCanTax;

    /* -- set US tax structuer, V3.3 -- */
    pUSCanTax = &(pItem->USCanVAT.USCanTax);

    /* -- initialize the buffer -- */
    memset(aszScrollBuff, '\0', sizeof(aszScrollBuff));
    memset(&alAttrib[0][0], 0x00, sizeof(alAttrib));

    /* -- set subtotal line R3.1 -- */
    if (pItem->lAmount) { 
        if (pUSCanTax->lTaxAmount[0]) {
            usScrollLine += MldMnemAmt(aszScrollBuff[0], TRN_TTL1_ADR, pItem->lAmount, MLD_PRECHECK_SYSTEM);
        }
    }

    /* -- set Can All Tax mnemonic and amount -- */
    if (pUSCanTax->lTaxAmount[0]) {
        usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_CONSTX_ADR, pUSCanTax->lTaxAmount[0], MLD_PRECHECK_SYSTEM);
    }

    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_PRECOLUMN1, &alAttrib[0][0]));
} 

/*
*===========================================================================
** Format  : SHORT   MldCanadaTax(ITEMAFFECTION *pItem, usType, usScroll);   
*   Input  : ITEMAFFECTION    *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays canada tax.
*===========================================================================
*/      
SHORT   MldDriveCanadaTax(ITEMAFFECTION  *pItem, USHORT usType, USHORT usScroll)
{
    PARAMDC ParaMDC;

    ParaMDC.uchMajorClass = CLASS_PARAMDC;
    ParaMDC.usAddress     = MDC_CANTAX_ADR;
    CliParaRead(&ParaMDC);

    if ((ParaMDC.uchMDCData & (EVEN_MDC_BIT2|EVEN_MDC_BIT1)) == EVEN_MDC_BIT1) {
         return(MldDriveCanGst(pItem, usType, usScroll));
    } else if ((ParaMDC.uchMDCData & (EVEN_MDC_BIT2|EVEN_MDC_BIT1)) == EVEN_MDC_BIT2) {
         return(MldDriveCanInd(pItem, usType, usScroll));
    }  else {
         return(MldDriveCanAll(pItem, usType, usScroll));
    }
}

/*
*===========================================================================
** Format  : SHORT   MldCanGst(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll); 
*            
*   Input  : ITEMAFFECTION    *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays canada tax.
*===========================================================================
*/
SHORT  MldDriveCanGst(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll)
{
    TCHAR  aszScrollBuff[6][MLD_DR3COLUMN + 1]; /* print data save area */
    LONG   alAttrib[6][MLD_PRECOLUMN1 + 1];                /* reverse video control flag area */
    USHORT  usScrollLine = 0;            /* number of lines to be printed */
    USCANTAX    *pUSCanTax;

    /* -- set US tax structuer, V3.3 -- */
    pUSCanTax = &(pItem->USCanVAT.USCanTax);

    /* -- initialize the buffer -- */
    memset(aszScrollBuff[0], '\0', sizeof(aszScrollBuff));
    memset(&alAttrib[0][0], 0x00, sizeof(alAttrib));

    /* -- set subtotal line R3.1 -- */
    if (pItem->lAmount) { 
        if (pUSCanTax->lTaxAmount[0]||pUSCanTax->lTaxAmount[1]) {
            usScrollLine += MldMnemAmt(aszScrollBuff[0], TRN_TTL1_ADR, pItem->lAmount, MLD_DRIVE_THROUGH_3SCROLL);
        }
    }

    /* -- set GST tax mnemonic and amount -- */
    if (pUSCanTax->lTaxAmount[0]) {
        usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_TX1_ADR, pUSCanTax->lTaxAmount[0], MLD_DRIVE_THROUGH_3SCROLL);
    }

    /* -- set PST tax mnemonic and amount -- */
    if (pUSCanTax->lTaxAmount[1]) {
        usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_TX2_ADR, pUSCanTax->lTaxAmount[1], MLD_DRIVE_THROUGH_3SCROLL);
    }

    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_DR3COLUMN, &alAttrib[0][0]));
}

/*
*===========================================================================
** Format  : SHORT  MldCanInd(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll);
*
*   Input  : ITEMAFFECTION    *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays canada individual tax.
*===========================================================================
*/
SHORT  MldDriveCanInd(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll)
{
    TCHAR  aszScrollBuff[10][MLD_DR3COLUMN + 1]; /* print data save area */
    LONG   alAttrib[10][MLD_PRECOLUMN1 + 1];                /* reverse video control flag area */
    USHORT  usScrollLine = 0;            /* number of lines to be printed */
    USCANTAX    *pUSCanTax;

    /* -- set US tax structuer, V3.3 -- */
    pUSCanTax = &(pItem->USCanVAT.USCanTax);

    /* -- initialize the buffer -- */
    memset(aszScrollBuff[0], '\0', sizeof(aszScrollBuff));
    memset(&alAttrib[0][0], 0x00, sizeof(alAttrib));

    /* -- set subtotal line R3.1 -- */
    if (pItem->lAmount) { 
        if (pUSCanTax->lTaxAmount[0]||pUSCanTax->lTaxAmount[1] || pUSCanTax->lTaxAmount[2]||pUSCanTax->lTaxAmount[3]) {
            usScrollLine += MldMnemAmt(aszScrollBuff[0], TRN_TTL1_ADR, pItem->lAmount, MLD_DRIVE_THROUGH_3SCROLL);
        }
    }

    /* -- set GST tax mnemonic and amount -- */
    if (pUSCanTax->lTaxAmount[0]) {
        usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_TX1_ADR, pUSCanTax->lTaxAmount[0], MLD_DRIVE_THROUGH_3SCROLL);
    }

    /* -- PST1 tax mnemonic and amount -- */
    if (pUSCanTax->lTaxAmount[1]) {
        usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_TX2_ADR, pUSCanTax->lTaxAmount[1], MLD_DRIVE_THROUGH_3SCROLL);
    }

    /* -- PST2 tax mnemonic and amount -- */
    if (pUSCanTax->lTaxAmount[2]) {
        usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_TX3_ADR, pUSCanTax->lTaxAmount[2], MLD_DRIVE_THROUGH_3SCROLL);
    }

    /* -- PST3 tax mnemonic and amount -- */
    if (pUSCanTax->lTaxAmount[3]) {
        usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_TX4_ADR, pUSCanTax->lTaxAmount[3], MLD_DRIVE_THROUGH_3SCROLL);
    }

    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_DR3COLUMN, &alAttrib[0][0]));
}

/*
*===========================================================================
** Format  : SHORT  MldCanAll(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll);
*
*   Input  : ITEMAFFECTION    *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays canada all tax.
*===========================================================================
*/
SHORT  MldDriveCanAll(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll)
{
    TCHAR  aszScrollBuff[4][MLD_DR3COLUMN + 1]; /* print data save area */
    LONG   alAttrib[4][MLD_PRECOLUMN1 + 1];                  /* reverse video control flag area */
    USHORT  usScrollLine = 0;            /* number of lines to be printed */
    USCANTAX    *pUSCanTax;

    /* -- set US tax structuer, V3.3 -- */
    pUSCanTax = &(pItem->USCanVAT.USCanTax);

    /* -- initialize the buffer -- */
    memset(aszScrollBuff[0], '\0', sizeof(aszScrollBuff));
    memset(&alAttrib[0][0], 0x00, sizeof(alAttrib));

    /* -- set subtotal line R3.1 -- */
    if (pItem->lAmount) { 
        if (pUSCanTax->lTaxAmount[0]) {
            usScrollLine += MldMnemAmt(aszScrollBuff[0], TRN_TTL1_ADR, pItem->lAmount, MLD_DRIVE_THROUGH_3SCROLL);
        }
    }

    /* -- set Can All Tax mnemonic and amount -- */
    if (pUSCanTax->lTaxAmount[0]) {
        usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_CONSTX_ADR, pUSCanTax->lTaxAmount[0], MLD_DRIVE_THROUGH_3SCROLL);
    }

    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_DR3COLUMN, &alAttrib[0][0]));
} 

/***** End Of Source *****/
