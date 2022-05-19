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
* Title       : Multiline Display VAT
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDVAT.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               MldVATServ() : diplays VAT Service
*               MldVAT()     : diplays VAT
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Aug-21-98 : 03.03.00 : M.Ozawa    : Initial
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
#include <pmg.h>
#include <regstrct.h>
#include <transact.h>
#include <mld.h>
#include "mldlocal.h"
#include "mldrin.h"
#include "termcfg.h"

/*
*===========================================================================
** Format  : VOID MldVATSubtotal(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll);
*            
*   Input  : ITEMAFFECTION    *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays VAT Subtotal
*===========================================================================
*/
SHORT MldVATSubtotal(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll)
{
    if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {
        return (MldDriveVATSubtotal(pItem, usType, usScroll));
    } else {
        return (MldPrechkVATSubtotal(pItem, usType, usScroll));
    }
}

/*
*===========================================================================
** Format  : VOID MldVATSubtotl(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll);
*            
*   Input  : ITEMAFFECTION    *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays VAT Subtotal
*===========================================================================
*/
SHORT MldPrechkVATSubtotal(ITEMAFFECTION  *pItem, USHORT usType, USHORT usScroll)
{
    TCHAR  aszScrollBuff[1][MLD_PRECOLUMN1 + 1]; /* print data save area */
    LONG   alAttrib;                     /* reverse video control flag area */
    USHORT  usScrollLine = 0;            /* number of lines to be printed */

    if (pItem->lAmount == 0L) {
        return(MldDummyScrollDisplay(usType, usScroll));
    }

    /* -- initialize the buffer -- */
    memset(aszScrollBuff[0], '\0', sizeof(aszScrollBuff));
    alAttrib = 0L;

    /* -- set subtotal mnem. and amount.  -- */
    usScrollLine += MldMnemAmt(aszScrollBuff[0], TRN_TTL1_ADR, pItem->lAmount, MLD_PRECHECK_SYSTEM);

    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_PRECOLUMN1, &alAttrib));
}

/*
*===========================================================================
** Format  : VOID MldVATSubtotal(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll);
*            
*   Input  : ITEMAFFECTION    *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays VAT Subtotal
*===========================================================================
*/
SHORT MldDriveVATSubtotal(ITEMAFFECTION  *pItem, USHORT usType, USHORT usScroll)
{
    TCHAR  aszScrollBuff[2][MLD_DR3COLUMN + 1]; /* print data save area */
    LONG   alAttrib[2][MLD_PRECOLUMN1 + 1];                  /* reverse video control flag area */
    USHORT  usScrollLine = 0;            /* number of lines to be printed */

    if (pItem->lAmount == 0L) {
        return(MldDummyScrollDisplay(usType, usScroll));
    }

    /* -- initialize the buffer -- */
    memset(aszScrollBuff[0], '\0', sizeof(aszScrollBuff));
    memset(&alAttrib[0][0], 0x00, sizeof(alAttrib));

    /* -- set subtotal mnem. and amount.  -- */
    usScrollLine += MldMnemAmt(aszScrollBuff[0], TRN_TTL1_ADR, pItem->lAmount, MLD_DRIVE_THROUGH_3SCROLL);

    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_DR3COLUMN, &alAttrib[0][0]));
}

/*
*===========================================================================
** Format  : VOID MldVATService(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll);
*            
*   Input  : ITEMAFFECTION    *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays VAT Service
*===========================================================================
*/
SHORT MldVATServ(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll)
{
    if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {
        return (MldDriveVATServ(pItem, usType, usScroll));
    } else {
        return (MldPrechkVATServ(pItem, usType, usScroll));
    }
}

/*
*===========================================================================
** Format  : VOID MldVATServ(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll);
*            
*   Input  : ITEMAFFECTION    *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays VAT Service
*===========================================================================
*/
SHORT MldPrechkVATServ(ITEMAFFECTION  *pItem, USHORT usType, USHORT usScroll)
{
    TCHAR  aszScrollBuff[1][MLD_PRECOLUMN1 + 1]; /* print data save area */
    LONG   alAttrib;                     /* reverse video control flag area */
    USHORT  usScrollLine = 0;            /* number of lines to be printed */

    if (pItem->lAmount == 0L) {
        return(MldDummyScrollDisplay(usType, usScroll));
    }

    /*--- NOT PRINT SERVICE (at included service) ---*/
    if ( !CliParaMDCCheck( MDC_VAT1_ADR, ODD_MDC_BIT3 ) &&
         !CliParaMDCCheck( MDC_VAT2_ADR, EVEN_MDC_BIT2 ) ) {
        return(MldDummyScrollDisplay(usType, usScroll));
    }

    /* -- initialize the buffer -- */
    memset(aszScrollBuff[0], '\0', sizeof(aszScrollBuff));
    alAttrib = 0L;

    /* -- set service mnem. and amount.  -- */
    /*--- Excluded Service ---*/
    if ( CliParaMDCCheck( MDC_VAT1_ADR, ODD_MDC_BIT3 ) ) {
        usScrollLine += MldMnemAmt(aszScrollBuff[0], TRN_TX4_ADR, pItem->lAmount, MLD_PRECHECK_SYSTEM);
    } else {
        usScrollLine += MldMnemAmtShift(aszScrollBuff[0], TRN_TX4_ADR, pItem->lAmount, MLD_PRECHECK_SYSTEM);
    }

    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_PRECOLUMN1, &alAttrib));
}

/*
*===========================================================================
** Format  : VOID MldVATServ(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll);
*            
*   Input  : ITEMAFFECTION    *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays VAT Service
*===========================================================================
*/
SHORT MldDriveVATServ(ITEMAFFECTION  *pItem, USHORT usType, USHORT usScroll)
{
    TCHAR  aszScrollBuff[2][MLD_DR3COLUMN + 1]; /* print data save area */
    LONG   alAttrib[2][MLD_PRECOLUMN1 + 1];                  /* reverse video control flag area */
    USHORT  usScrollLine = 0;            /* number of lines to be printed */

    if (pItem->lAmount == 0L) {
        return(MldDummyScrollDisplay(usType, usScroll));
    }

    /*--- NOT PRINT SERVICE (at included service) ---*/
    if ( !CliParaMDCCheck( MDC_VAT1_ADR, ODD_MDC_BIT3 ) &&
         !CliParaMDCCheck( MDC_VAT2_ADR, EVEN_MDC_BIT2 ) ) {
        return(MldDummyScrollDisplay(usType, usScroll));
    }

    /* -- initialize the buffer -- */
    memset(aszScrollBuff[0], '\0', sizeof(aszScrollBuff));
    memset(&alAttrib[0][0], 0x00, sizeof(alAttrib));

    /* -- set service mnem. and amount.  -- */
    /*--- Excluded Service ---*/
    if ( CliParaMDCCheck( MDC_VAT1_ADR, ODD_MDC_BIT3 ) ) {
        usScrollLine += MldMnemAmt(aszScrollBuff[0], TRN_TX4_ADR, pItem->lAmount, MLD_DRIVE_THROUGH_3SCROLL);
    } else {
        usScrollLine += MldMnemAmtShift(aszScrollBuff[0], TRN_TX4_ADR, pItem->lAmount, MLD_DRIVE_THROUGH_3SCROLL);
    }

    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_DR3COLUMN, &alAttrib[0][0]));
}

/*
*===========================================================================
** Format  : VOID MldVAT(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll);
*            
*   Input  : ITEMAFFECTION    *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays VAT
*===========================================================================
*/
SHORT MldVAT(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll)
{
    if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {
        return (MldDriveVAT(pItem, usType, usScroll));
    } else {
        return (MldPrechkVAT(pItem, usType, usScroll));
    }
}

/*
*===========================================================================
** Format  : VOID MldVATServ(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll);
*            
*   Input  : ITEMAFFECTION    *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays VAT
*===========================================================================
*/
SHORT MldPrechkVAT(ITEMAFFECTION  *pItem, USHORT usType, USHORT usScroll)
{
    TCHAR  aszScrollBuff[6][MLD_PRECOLUMN1 + 1]; /* print data save area */
    LONG   alAttrib[6][MLD_PRECOLUMN1 + 1];                  /* reverse video control flag area */
    USHORT  usScrollLine = 0;            /* number of lines to be printed */
    UCHAR i,j;

    /* -- initialize the buffer -- */
    memset(aszScrollBuff[0], '\0', sizeof(aszScrollBuff));
    memset(&alAttrib[0][0], 0x00, sizeof(alAttrib));

    /* -- execluded vat -- */
    if ( CliParaMDCCheck( MDC_VAT1_ADR, ODD_MDC_BIT2 ) ) {
        /* print on receipt */
        /* if ( CliParaMDCCheck( MDC_VAT2_ADR, EVEN_MDC_BIT0 ) ) { */
            for (i=j=0; i < NUM_VAT; i++, j+=3) {
                if ((pItem->USCanVAT.ItemVAT[i].lVATRate == 0) || (pItem->USCanVAT.ItemVAT[i].lVATAmt == 0)) {
                    continue;
                }
                /* print vat applicable total */
                if (CliParaMDCCheck( MDC_VAT2_ADR, EVEN_MDC_BIT1 ) &&
                    (pItem->USCanVAT.ItemVAT[i].lVATable)) {
                    usScrollLine += MldMnemAmtShift(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], (UCHAR)(TRN_TXBL1_ADR + j), pItem->USCanVAT.ItemVAT[i].lVATable, MLD_PRECHECK_SYSTEM);
                }
                usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], (UCHAR)(TRN_TX1_ADR + j), pItem->USCanVAT.ItemVAT[i].lVATAmt, MLD_PRECHECK_SYSTEM);
            }
        /* } */
    } else {
        /* -- included vat -- */
        /*--- NOT PRINT VAT, VATABLE (at included vat) ---*/
        if (CliParaMDCCheck( MDC_VAT2_ADR, EVEN_MDC_BIT0 ) ) {
            for (i=j=0; i < NUM_VAT; i++, j+=3) {
                if ((pItem->USCanVAT.ItemVAT[i].lVATRate == 0) || (pItem->USCanVAT.ItemVAT[i].lVATAmt == 0)) {
                    continue;
                }
                /* print vat applicable total */
                if (CliParaMDCCheck( MDC_VAT2_ADR, EVEN_MDC_BIT1 ) &&
                    (pItem->USCanVAT.ItemVAT[i].lVATable)) {
                    usScrollLine += MldMnemAmtShift(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], (UCHAR)(TRN_TXBL1_ADR + j), pItem->USCanVAT.ItemVAT[i].lVATable, MLD_PRECHECK_SYSTEM);
                }
                usScrollLine += MldMnemAmtShift(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], (UCHAR)(TRN_TX1_ADR + j), pItem->USCanVAT.ItemVAT[i].lVATAmt, MLD_PRECHECK_SYSTEM);
            }
        }
    }

    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_PRECOLUMN1, &alAttrib[0][0]));
}

/*
*===========================================================================
** Format  : VOID MldVATServ(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll);
*            
*   Input  : ITEMAFFECTION    *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays VAT
*===========================================================================
*/
SHORT MldDriveVAT(ITEMAFFECTION  *pItem, USHORT usType, USHORT usScroll)
{
    TCHAR  aszScrollBuff[12][MLD_DR3COLUMN + 1]; /* print data save area */
    LONG   alAttrib[12][MLD_PRECOLUMN1 + 1];                  /* reverse video control flag area */
    USHORT  usScrollLine = 0;            /* number of lines to be printed */
    UCHAR i,j;

    /* -- initialize the buffer -- */
    memset(aszScrollBuff[0], '\0', sizeof(aszScrollBuff));
    memset(&alAttrib[0][0], 0x00, sizeof(alAttrib));

    /* -- execluded vat -- */
    if ( CliParaMDCCheck( MDC_VAT1_ADR, ODD_MDC_BIT2 ) ) {
        /* print on receipt */
        /* if ( CliParaMDCCheck( MDC_VAT2_ADR, EVEN_MDC_BIT0 ) ) { */
            for (i=j=0; i < NUM_VAT; i++, j+=3) {
                if ((pItem->USCanVAT.ItemVAT[i].lVATRate == 0) || (pItem->USCanVAT.ItemVAT[i].lVATAmt == 0)) {
                    continue;
                }
                /* print vat applicable total */
                if (CliParaMDCCheck( MDC_VAT2_ADR, EVEN_MDC_BIT1 ) &&
                    (pItem->USCanVAT.ItemVAT[i].lVATable)) {
                    usScrollLine += MldMnemAmtShift(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], (UCHAR)(TRN_TXBL1_ADR + j), pItem->USCanVAT.ItemVAT[i].lVATable, MLD_DRIVE_THROUGH_3SCROLL);
                }
                usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], (UCHAR)(TRN_TX1_ADR + j), pItem->USCanVAT.ItemVAT[i].lVATAmt, MLD_DRIVE_THROUGH_3SCROLL);
            }
        /* } */
    } else {
        /* -- included vat -- */
        /*--- NOT PRINT VAT, VATABLE (at included vat) ---*/
        if (CliParaMDCCheck( MDC_VAT2_ADR, EVEN_MDC_BIT0 ) ) {
            for (i=j=0; i < NUM_VAT; i++, j+=3) {
                if ((pItem->USCanVAT.ItemVAT[i].lVATRate == 0) || (pItem->USCanVAT.ItemVAT[i].lVATAmt == 0)) {
                    continue;
                }
                /* print vat applicable total */
                if (CliParaMDCCheck( MDC_VAT2_ADR, EVEN_MDC_BIT1 ) &&
                    (pItem->USCanVAT.ItemVAT[i].lVATable)) {
                    usScrollLine += MldMnemAmtShift(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], (UCHAR)(TRN_TXBL1_ADR + j), pItem->USCanVAT.ItemVAT[i].lVATable, MLD_DRIVE_THROUGH_3SCROLL);
                }
                usScrollLine += MldMnemAmtShift(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], (UCHAR)(TRN_TX1_ADR + j), pItem->USCanVAT.ItemVAT[i].lVATAmt, MLD_DRIVE_THROUGH_3SCROLL);
            }
        }
    }

    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_DR3COLUMN, &alAttrib[0][0]));
}

/***** End Of Source *****/
