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
* Title       : MaintTaxTable Module  ( SUPER & PROGRAM MODE )
* Category    : Maintenance, NCR 2170 US Hospitality Application Program
* Program Name: MATTAX1.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Midium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: This function read & write & report TAX TABLE #1.
*
*           The provided function names are as follows:
*
*               MaintTaxTblInit()              : initialize static data
*               MaintTaxTblReadNo1()           : read & display tax table
*               MaintTaxRateEditNo1()          : edit rate
*               MaintTaxableAmountEditNo1()    : edit taxable amount
*               MaintTaxAmountEditNo1()        : edit tax amount
*               MaintTaxableAmountErrorNo1()   : correct taxable amount
*               MaintTaxTblWriteNo1()          : write tax table in system ram
*               MaintTaxTblDeleteNo1()         : delete tax table from system ram
*               MaintTaxTblCalNo1()            : calculate & print tax
*               MaintTaxTblTmpReportNo1()      : print tax table report during tax table creation
*               MaintTaxTblReportNo1()         : print tax table report
*
*               MaintTaxTblRead()              : read & display tax table
*               MaintTaxRateEdit()             : edit rate
*               MaintTaxableAmountEdit()       : edit taxable amount
*               MaintTaxAmountEdit()           : edit tax amount
*               MaintTaxableAmountError()      : correct taxable amount
*               MaintTaxTblDelete()            : delete tax table from system ram
*               MaintTaxTblCal()               : calculate & print tax
*               MaintTaxTblTmpReport()         : print tax table report during tax table creation
*               MaintTaxTblReport()            : print tax table report
*
* --------------------------------------------------------------------------
* Update Histories
*    Date  : Ver.Rev. :   Name    : Description
* Jun-22-92: 00.00.01 : J.Ikeda   : initial
*          :          :           :
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
==============================================================================
;                      I N C L U D E     F I L E s
=============================================================================
**/
#include	<tchar.h>
#include <string.h>

#include "ecr.h"
#include "uie.h"
#include "pif.h"
/* #include <log.h> */
#include "rfl.h"
#include "paraequ.h"
#include "para.h"
#include "maint.h"
#include "display.h"
#include "regstrct.h"
#include "transact.h"
#include "prt.h"
#include "csstbpar.h"
/* #include <appllog.h> */

#include "mld.h"
/* #include <csttl.h> */
/* #include <report.h> */
/* #include <mldsup.h> */

#include "maintram.h"


/*
*===========================================================================
*   S T A T I C   A R E A  D E C L A R T I O N
*===========================================================================
*/

UCHAR   uchMaintTblOff;             /* tax table offset */
UCHAR   uchMaintTaxCtl;             /* control flag */
USHORT  usMaintTbl1Len;             /* tax table #1 length */
USHORT  usMaintTbl2Len;             /* tax table #2 length */
USHORT  usMaintTbl3Len;             /* tax table #3 length */
USHORT  usMaintTbl4Len;             /* tax table #4 length */

static USHORT  usMaintSavedTax;            /* saved tax amount */
static USHORT  usMaintTaxDiff;             /* the difference of tax amount */
static USHORT  usMaintMaxEditLen;          /* editable length for tax */
static ULONG   ulMaintPreTaxable;          /* previous taxable amount */
static ULONG   ulMaintInputTaxable;        /* taxable amount from UI */


/*
*===========================================================================
** Synopsis:    VOID MaintTaxTblInit(UCHAR uchMajorClass)
*
*     Input:    uchMajorClass : major class
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function initialize static area.
*===========================================================================
*/

VOID MaintTaxTblInit(UCHAR uchMajorClass)
{
	MAINTTAXTBL     MaintTaxTbl = {0};

    uchMaintTblOff = MAX_TTBLRATE_SIZE;
    uchMaintTaxCtl = 0;
    usMaintTbl1Len = 0;
    usMaintTbl2Len = 0;
    usMaintTbl3Len = 0;
    usMaintTbl4Len = 0;
    usMaintSavedTax = 0;
    usMaintTaxDiff = 0;
    ulMaintPreTaxable = 0U;
    ulMaintInputTaxable = 0U;
    usMaintMaxEditLen = 0;

    /* display */
    MaintTaxTbl.uchMajorClass = uchMajorClass;
    MaintTaxTbl.uchMinorClass = CLASS_MAINTTAXTBL_DISP0_INT;
    RflGetLead (MaintTaxTbl.aszMnemonics, LDT_DATA_ADR);
    DispWrite(&MaintTaxTbl);
}

/*
*===========================================================================
** Synopsis:    VOID MaintTaxTblReadNo1( VOID )
*
*     Input:    Nothing
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function reads TAX TABLE No.1.
*===========================================================================
*/

VOID MaintTaxTblReadNo1(VOID)
{
    MaintTaxTblRead(CLASS_PARATAXTBL1);
}

/*
*===========================================================================
** Synopsis:    VOID MaintTaxTblRead( UCHAR uchMajorClass )
*
*     Input:    uchMajorClass : major class
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function reads TAX TABLE.
*===========================================================================
*/

VOID MaintTaxTblRead(UCHAR uchMajorClass)
{
    USHORT      usRestLen;
    PARATAXTBL  ParaTaxTbl;

    /* read tax table */
    ParaTaxTbl.uchMajorClass = uchMajorClass;
    CliParaRead(&ParaTaxTbl);                                   /* call ParaTaxRateRead() */

    /* get table length */
    usMaintTbl1Len = ParaTaxTbl.TaxData.ausTblLen[0];
    usMaintTbl2Len = ParaTaxTbl.TaxData.ausTblLen[1];
    usMaintTbl3Len = ParaTaxTbl.TaxData.ausTblLen[2];
    usMaintTbl4Len = ParaTaxTbl.TaxData.ausTblLen[3];

    /* calculate the rest */
    usRestLen = MAX_TTBLDATA_SIZE - (usMaintTbl1Len + usMaintTbl2Len
                + usMaintTbl3Len + usMaintTbl4Len);

    /* set length */
    memcpy(MaintWork.TaxTbl.TaxData.ausTblLen, ParaTaxTbl.TaxData.ausTblLen,
           sizeof(ParaTaxTbl.TaxData.ausTblLen));

    switch (uchMajorClass) {
    case CLASS_PARATAXTBL1:
        usMaintMaxEditLen = usMaintTbl1Len + usRestLen;

        /* set modified data */
        memcpy(&MaintWork.TaxTbl.TaxData.auchTblData[usMaintTbl1Len + usRestLen],
                &ParaTaxTbl.TaxData.auchTblData[usMaintTbl1Len],
                usMaintTbl2Len + usMaintTbl3Len + usMaintTbl4Len);
        break;

    case CLASS_PARATAXTBL2:
        usMaintMaxEditLen = usMaintTbl1Len + usMaintTbl2Len + usRestLen;

        /* set modified data */
        /* copy tax table #1 */
        memcpy(MaintWork.TaxTbl.TaxData.auchTblData,
               ParaTaxTbl.TaxData.auchTblData,
               usMaintTbl1Len);

        /* copy tax table #3,#4 */
        memcpy(&MaintWork.TaxTbl.TaxData.auchTblData[usMaintTbl1Len + usMaintTbl2Len + usRestLen],
               &ParaTaxTbl.TaxData.auchTblData[usMaintTbl1Len + usMaintTbl2Len],
               usMaintTbl3Len + usMaintTbl4Len);

        uchMaintTblOff += usMaintTbl1Len;     /* rate length has already added to offset */
        break;

    case CLASS_PARATAXTBL3:
        usMaintMaxEditLen = usMaintTbl1Len + usMaintTbl2Len + usMaintTbl3Len+ usRestLen;

        /* set modified data */
        /* copy tax table #1,#2 */
        memcpy(MaintWork.TaxTbl.TaxData.auchTblData,
               ParaTaxTbl.TaxData.auchTblData,
               usMaintTbl1Len + usMaintTbl2Len);

        /* copy tax table #4 */
        memcpy(&MaintWork.TaxTbl.TaxData.auchTblData[usMaintTbl1Len
                + usMaintTbl2Len + usMaintTbl3Len + usRestLen],
               &ParaTaxTbl.TaxData.auchTblData[usMaintTbl1Len
                + usMaintTbl2Len + usMaintTbl3Len],
               usMaintTbl4Len);

        uchMaintTblOff += ( UCHAR)(usMaintTbl1Len + usMaintTbl2Len);     /* rate length has already added to offset */
        break;

    default:
        usMaintMaxEditLen = usMaintTbl1Len + usMaintTbl2Len
                            + usMaintTbl3Len + usMaintTbl4Len + usRestLen;

        /* set modified data */
        memcpy(MaintWork.TaxTbl.TaxData.auchTblData,
               ParaTaxTbl.TaxData.auchTblData,
               usMaintTbl1Len + usMaintTbl2Len + usMaintTbl3Len);

        uchMaintTblOff += ( UCHAR)(usMaintTbl1Len + usMaintTbl2Len
                          + usMaintTbl3Len);     /* rate length has already added to offset */
        break;
    }
}

/*
*===========================================================================
** Synopsis:    SHORT MaintTaxRateEditNo1(MAINTTAXTBL *pData)
*
*     Input:    *pData          : pointer to structure for MAINTTAXTBL
*                  (pData->uchInteger)
*                  (pData->uchDenominator)
*                  (pData->uchNumerator)
*
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               LDT_KEYOVER_ADR : wrong data
*
** Description: This function edits tax no.1 rate.
*===========================================================================
*/

SHORT MaintTaxRateEditNo1(MAINTTAXTBL *pData)
{
    return(MaintTaxRateEdit(pData, CLASS_MAINTTAXTBL1));
}

/*
*===========================================================================
** Synopsis:    SHORT MaintTaxRateEdit(MAINTTAXTBL *pData, UCHAR uchMajorClass)
*
*     Input:    *pData          : pointer to structure for MAINTTAXTBL
*                  (pData->uchInteger)
*                  (pData->uchDenominator)
*                  (pData->uchNumerator)
*               uchMajorClass : major class
*
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               LDT_KEYOVER_ADR : wrong data
*
** Description: This function edits tax rate.
*===========================================================================
*/

SHORT MaintTaxRateEdit(MAINTTAXTBL *pData, UCHAR uchMajorClass)
{

	MAINTTAXTBL     MaintTaxTbl = {0};
    USHORT          usOffset;
    USHORT          usLength;
    UCHAR           uchACCode;

    /* check status (key seqence) */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {    /* without data */
        return(LDT_SEQERR_ADR);                     /* sequence error */
    }

    switch (uchMajorClass) {
    case CLASS_MAINTTAXTBL1:
        usLength = usMaintTbl2Len + usMaintTbl3Len + usMaintTbl4Len;
        usOffset = 0;
        uchACCode = AC_TAXTBL1;
        break;

    case CLASS_MAINTTAXTBL2:
        usLength = usMaintTbl1Len + usMaintTbl3Len + usMaintTbl4Len;
        usOffset = usMaintTbl1Len;
        uchACCode = AC_TAXTBL2;
        break;

    case CLASS_MAINTTAXTBL3:
        usLength = usMaintTbl1Len + usMaintTbl2Len + usMaintTbl4Len;
        usOffset = usMaintTbl1Len + usMaintTbl2Len;
        uchACCode = AC_TAXTBL3;
        break;

    default:
        usLength = usMaintTbl1Len + usMaintTbl2Len + usMaintTbl3Len;
        usOffset = usMaintTbl1Len + usMaintTbl2Len + usMaintTbl3Len;
        uchACCode = AC_TAXTBL4;
        break;
    }

    /* check tax table size */
    if ((MAX_TTBLDATA_SIZE - usLength) < MAINT_TAXTBL_MUSTSIZE) {
        return(LDT_FLFUL_ADR);
    }

    /* set WDC */
    if ((pData->uchDenominator == 0) && (pData->uchNumerator == 0)) {       /* in case of WDC = 0 */
        /* check if integer is '0' */
        if (pData->uchInteger == 0) {
            return(LDT_KEYOVER_ADR);                                    /* wrong data */
        } else {
            MaintWork.TaxTbl.TaxData.auchTblData[usOffset] = 1;                /* WDC = $1.00 */
        }
    } else if ((pData->uchDenominator == 0) || (pData->uchNumerator == 0)) {
            return(LDT_KEYOVER_ADR);                                    /* wrong data */
    } else {
        MaintWork.TaxTbl.TaxData.auchTblData[usOffset] = ( UCHAR)(pData->uchDenominator);
    }

    /* edit tax rate */
    MaintWork.TaxTbl.TaxData.auchTblData[usOffset + 2] = pData->uchInteger;
    MaintWork.TaxTbl.TaxData.auchTblData[usOffset + 3] = pData->uchDenominator;
    MaintWork.TaxTbl.TaxData.auchTblData[usOffset + 4] = pData->uchNumerator;

    /* control header item */
    MaintHeaderCtl(uchACCode, RPT_ACT_ADR);

    /* print tax rate */
    MaintTaxTbl.uchMajorClass = uchMajorClass;
    MaintTaxTbl.uchMinorClass = CLASS_MAINTTAXTBL_PRTWRTINT;
    MaintTaxTbl.uchInteger = MaintWork.TaxTbl.TaxData.auchTblData[usOffset + 2];
    MaintTaxTbl.uchDenominator = MaintWork.TaxTbl.TaxData.auchTblData[usOffset + 3];
    MaintTaxTbl.uchNumerator = MaintWork.TaxTbl.TaxData.auchTblData[usOffset + 4];
    MaintTaxTbl.usPrtControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /******************/
    /* Display to MLD */
    /******************/
    MldDispItem(&MaintTaxTbl,0);

    PrtPrintItem(NULL,&MaintTaxTbl);

    /* Display First Taxable Amount Entry */
    MaintTaxTbl.uchMajorClass = uchMajorClass;
    MaintTaxTbl.uchMinorClass = CLASS_MAINTTAXTBL_DISP2_INT;
    MaintTaxTbl.ulEndTaxableAmount = 0L;
    RflGetLead (MaintTaxTbl.aszMnemonics, LDT_DATA_ADR);
    DispWrite(&MaintTaxTbl);

    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintTaxableAmountEditNo1(MAINTTAXTBL *pData)
*
*     Input:    *pData   : pointer to structure for MAINTTAXTBL
*                  (pData->ulEndTaxableAmount)
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               LDT_KEYOVER_ADR : wrong data
*
** Description: This function edits taxable no.1 amount.
*===========================================================================
*/

SHORT MaintTaxableAmountEditNo1(MAINTTAXTBL *pData)
{
    return(MaintTaxableAmountEdit(pData, CLASS_MAINTTAXTBL1));
}

/*
*===========================================================================
** Synopsis:    SHORT MaintTaxableAmountEdit(MAINTTAXTBL *pData, UCHAR uchMajorClass)
*
*     Input:    *pData   : pointer to structure for MAINTTAXTBL
*                  (pData->ulEndTaxableAmount)
*               uchMajorClass : major class
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               LDT_KEYOVER_ADR : wrong data
*
** Description: This function edits taxable amount.
*===========================================================================
*/

SHORT MaintTaxableAmountEdit(MAINTTAXTBL *pData, UCHAR uchMajorClass)
{
	MAINTTAXTBL    MaintTaxTbl = {0};

    /* check status (key seqence) */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {    /* without data */
        return(LDT_SEQERR_ADR);                     /* sequence error */
    }

    /* check operational times */
    if (uchMaintTaxCtl & MAINT_TAXABLE_CTL) {

        /* check inputed taxable amount with previous taxable amount */
        if (pData->ulEndTaxableAmount <= ulMaintPreTaxable) {
            return(LDT_KEYOVER_ADR);                            /* wrong data */
        }
    } else {  /* 1st operation */
        uchMaintTaxCtl |= MAINT_TAXABLE_CTL;                    /* set flag */
    }
    ulMaintInputTaxable = pData->ulEndTaxableAmount;            /* save taxable amount temporarily */

    /* display taxable amount */
    MaintTaxTbl.uchMajorClass = uchMajorClass;
    MaintTaxTbl.uchMinorClass = CLASS_MAINTTAXTBL_DISP2_TAXABL;
    MaintTaxTbl.ulEndTaxableAmount = pData->ulEndTaxableAmount;
    RflGetLead (MaintTaxTbl.aszMnemonics, LDT_DATA_ADR);
    DispWrite(&MaintTaxTbl);
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintTaxAmountEditNo1(MAINTTAXTBL *pData)
*
*     Input:    *pData   : pointer to structure for MAINTTAXTBL
*                  (pData->usTaxAmount)
*
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               LDT_KEYOVER_ADR : wrong data
*               LDT_FLFUL_ADR   : tax table over flow
*               MAINT_2WDC_OVER : taxable amount is over 2*WDC
*
** Description: This function edits tax no.1 amount.
*===========================================================================
*/

SHORT MaintTaxAmountEditNo1(MAINTTAXTBL *pData)
{
    return(MaintTaxAmountEdit(pData, CLASS_MAINTTAXTBL1, 0, 0));
}

/*
*===========================================================================
** Synopsis:    SHORT MaintTaxAmountEdit(MAINTTAXTBL *pData,
                       UCHAR uchMajorClass, USHORT usOffset, UCHAR uchOffTblLen)
*
*     Input:    *pData   : pointer to structure for MAINTTAXTBL
*                  (pData->usTaxAmount)
*               uchMajorClass : major class
*               usOffset : offset of file
*               uchOffTblLen : offset of table length
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               LDT_KEYOVER_ADR : wrong data
*               LDT_FLFUL_ADR   : tax table over flow
*               MAINT_2WDC_OVER : taxable amount is over 2*WDC
*
** Description: This function edits tax amount.
*===========================================================================
*/

SHORT MaintTaxAmountEdit(MAINTTAXTBL *pData, UCHAR uchMajorClass,
                         USHORT usOffset, UCHAR uchOffTblLen)
{
    USHORT        j;
    USHORT        usCount = 0;
    LONG          lTotal;
    ULONG         i;
    ULONG         ulTaxableDiff = 0U;
    ULONG         ulTaxOnWDC = 0U;
	MAINTTAXTBL   MaintTaxTbl = {0};

    /* check status (key seqence) */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {    /* without data */
        return(LDT_SEQERR_ADR);                     /* sequence error */
    }

    /* check operational times */
    if (uchMaintTaxCtl & MAINT_TAX_CTL) {
        /* check inputed taxable amount with previous taxable amount */
        if (pData->usTaxAmount <= usMaintSavedTax) {
            return(LDT_KEYOVER_ADR);                               /* wrong data */
        }
    } else { /* 1st operation */
        uchMaintTaxCtl |= MAINT_TAX_CTL;                            /* set flag */
    }
    usMaintTaxDiff = pData->usTaxAmount - usMaintSavedTax;          /* set the difference of tax amount */

    /* check tax table size */
    ulTaxableDiff = ulMaintInputTaxable - ulMaintPreTaxable;

    /* check operational times */
    if (!(uchMaintTaxCtl & MAINT_1STTAXTBL)) {   /* in case of 1st set tax table */
        ulTaxableDiff++;
    }

    /* count times the difference of tax amount was over 255 */
    if (ulTaxableDiff >= 255U) {
        usCount = ( USHORT)(ulTaxableDiff / 255U);
    }

    /* check if tax table more create */
    if (( USHORT)(uchMaintTblOff + usMaintTaxDiff + usCount) >= usMaintMaxEditLen) {
        /* check if 1st tax table create */
        if (uchMaintTblOff == (UCHAR)(MAX_TTBLRATE_SIZE + usOffset)) {  /* in case tax table overflow in creating 1st tax table */
            uchMaintTaxCtl &= ~MAINT_TAXABLE_CTL;   /* reset flag */
            uchMaintTaxCtl &= ~MAINT_TAX_CTL;       /* reset flag */
        }

        /* display old data */
        MaintTaxTbl.uchMajorClass = uchMajorClass;
        MaintTaxTbl.uchMinorClass = CLASS_MAINTTAXTBL_DISP2_TAX;
        MaintTaxTbl.ulEndTaxableAmount = ulMaintPreTaxable;
        MaintTaxTbl.usTaxAmount = usMaintSavedTax;
        RflGetLead (MaintTaxTbl.aszMnemonics, LDT_DATA_ADR);
        DispWrite(&MaintTaxTbl);
        return(LDT_FLFUL_ADR);                                      /* tax table overflow */
    }
    uchMaintTaxCtl |= MAINT_1STTAXTBL;
    uchMaintTblOff += ( UCHAR)(usMaintTaxDiff);

    /* set '0' in passed tax table */
    for (i = 1; i < ( UCHAR)(usMaintTaxDiff); i++) {
        MaintWork.TaxTbl.TaxData.auchTblData[uchMaintTblOff - i] = 0;
    }

    /* check the difference of taxable amount */
    if (ulTaxableDiff >= 255U) {
        /* count times the difference of tax amount was over 255 */
        usCount = ( USHORT)(ulTaxableDiff / 255U);
        ulTaxableDiff = ulTaxableDiff % 255U;

        /* set 255 in counted tax table */
        for (j = 0; j < usCount; j++) {
            MaintWork.TaxTbl.TaxData.auchTblData[uchMaintTblOff++] = 255U;
        }
    }
    MaintWork.TaxTbl.TaxData.auchTblData[uchMaintTblOff] = ( UCHAR)(ulTaxableDiff);
    usMaintSavedTax = pData->usTaxAmount;                           /* save tax amount */
    ulMaintPreTaxable = ulMaintInputTaxable;                        /* save taxable amount */

    /* set tax table length */
    MaintWork.TaxTbl.TaxData.ausTblLen[uchOffTblLen] = ( USHORT)(uchMaintTblOff - usOffset + 1);

    /* check if taxable amount is over WDC */
    if (ulMaintPreTaxable >= ( ULONG)(100 * MaintWork.TaxTbl.TaxData.auchTblData[usOffset])) {
        /* check if tax on WDC is set */
        if (!(uchMaintTaxCtl & MAINT_SETTAXONWDC)) {
            /* set tax on WDC */
            MaintWork.TaxTbl.TaxData.auchTblData[usOffset + 1] = ( UCHAR)(usMaintSavedTax);
            uchMaintTaxCtl |= MAINT_SETTAXONWDC;
        }
    }

    /* set common data for display/print */
    MaintTaxTbl.uchMajorClass = uchMajorClass;
    MaintTaxTbl.ulEndTaxableAmount = ulMaintPreTaxable;
    MaintTaxTbl.usTaxAmount = usMaintSavedTax;
    RflGetLead (MaintTaxTbl.aszMnemonics, LDT_DATA_ADR);
    MaintTaxTbl.usPrtControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* check taxtable amount with over 2*WDC */
    if (ulMaintPreTaxable < ( ULONG)(2 * 100 * MaintWork.TaxTbl.TaxData.auchTblData[usOffset])) {
        /* set minor class for display tax/taxable amount */
        MaintTaxTbl.uchMinorClass = CLASS_MAINTTAXTBL_DISP2_TAX;
        DispWrite(&MaintTaxTbl);

        /* set minor class for print taxable/tax amount */
        MaintTaxTbl.uchMinorClass = CLASS_MAINTTAXTBL_PRTTAXABLE;

        /******************/
        /* Display to MLD */
        /******************/
        MldDispItem(&MaintTaxTbl,0);

        PrtPrintItem(NULL,&MaintTaxTbl);
        return(SUCCESS);
    }

    /* check if taxable amount is over 2*WDC */
    if (!(uchMaintTaxCtl & MAINT_OVER2WDC)) {                   /* taxable amount is below 2*WDC */
        PifBeep(MAINT_BEEP);                                    /* tone */

        /* set minor class for display */
        MaintTaxTbl.uchMinorClass = CLASS_MAINTTAXTBL_DISP3_TAXABL;
        DispWrite(&MaintTaxTbl);

        /* set minor class for print taxable/tax amount */
        MaintTaxTbl.uchMinorClass = CLASS_MAINTTAXTBL_PRTTAXABLE;

        /******************/
        /* Display to MLD */
        /******************/
        MldDispItem(&MaintTaxTbl,0);

        PrtPrintItem(NULL,&MaintTaxTbl);
        uchMaintTaxCtl |= MAINT_OVER2WDC;
        return(MAINT_2WDC_OVER);                                /* taxable amount is over 2*WDC */
    }

    /* taxable amount is over 2*WDC and more create tax table */
    /* WDC = WDC + denominator */
    if (MaintWork.TaxTbl.TaxData.auchTblData[usOffset + 3] == 0) {
        MaintWork.TaxTbl.TaxData.auchTblData[usOffset] += 1;
    } else {
        MaintWork.TaxTbl.TaxData.auchTblData[usOffset] += MaintWork.TaxTbl.TaxData.auchTblData[usOffset + 3];
    }

    /* reset flag */
    uchMaintTaxCtl &= ~MAINT_OVER2WDC;

    /* set tax table length */
    MaintWork.TaxTbl.TaxData.ausTblLen[uchOffTblLen] = ( USHORT)(uchMaintTblOff - usOffset + 1);

    /* calculate TAX on WDC */
    lTotal = 100 * MaintWork.TaxTbl.TaxData.auchTblData[usOffset];

    for ( i = 0; i < uchMaintTblOff; i++) {
        lTotal -= MaintWork.TaxTbl.TaxData.auchTblData[usOffset + MAINT_RATE_LEN + i];

        if (lTotal <= 0L) {
            break;
        }
        ulTaxOnWDC++;
    }
    MaintWork.TaxTbl.TaxData.auchTblData[usOffset + 1] = ( UCHAR)(ulTaxOnWDC);

    /* display */
    MaintTaxTbl.uchMinorClass = CLASS_MAINTTAXTBL_DISP2_TAX;
    DispWrite(&MaintTaxTbl);

    /* print */
    MaintTaxTbl.uchMinorClass = CLASS_MAINTTAXTBL_PRTTAXABLE;

    /******************/
    /* Display to MLD */
    /******************/
    MldDispItem(&MaintTaxTbl,0);

    PrtPrintItem(NULL,&MaintTaxTbl);
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintTaxableAmountErrorNo1(MAINTTAXTBL *pData)
*
*     Input:    *pData          : pointer to structure for MAINTTAXTBL
*                  (pData->ulEndTaxableAmount)
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               LDT_KEYOVER_ADR : wrong data
*
** Description: This function corrects taxable no.1 amount.
*===========================================================================
*/

SHORT MaintTaxableAmountErrorNo1(MAINTTAXTBL *pData)
{
    return(MaintTaxableAmountError(pData, CLASS_MAINTTAXTBL1, 0));
}

/*
*===========================================================================
** Synopsis:    SHORT MaintTaxableAmountError(MAINTTAXTBL *pData,
                                      UCHAR uchMajorClass, USHORT usOffset)
*
*     Input:    *pData          : pointer to structure for MAINTTAXTBL
*                  (pData->ulEndTaxableAmount)
*               uchMajorClass : major class
*               usOffset : offset of file
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               LDT_KEYOVER_ADR : wrong data
*
** Description: This function corrects taxable amount.
*===========================================================================
*/

SHORT MaintTaxableAmountError(MAINTTAXTBL *pData, UCHAR uchMajorClass,
                              USHORT usOffset)
{
    UCHAR         i;
    USHORT        usTax = 0;
    ULONG         ulTaxableDiff = 0U;
    ULONG         ulTaxableAmount;
    ULONG         ulWDC = 0U;
	MAINTTAXTBL   MaintTaxTbl = {0};

    /* check status (key seqence) */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {    /* without data */
        return(LDT_SEQERR_ADR);                     /* sequence error */
    }

    /* check input taxable amount */
    if (pData->ulEndTaxableAmount > ulMaintPreTaxable) {
        return(LDT_KEYOVER_ADR);
    }

    ulTaxableAmount =  pData->ulEndTaxableAmount;

    /* search corresponding taxable amount */
    for (i = ( UCHAR)(usOffset + MAX_TTBLRATE_SIZE); i < ( UCHAR)(usMaintMaxEditLen); i++) {
        /* check if the difference of taxable amount is 255 or not */
        if (MaintWork.TaxTbl.TaxData.auchTblData[i] == 255U) {
            ulTaxableDiff += 255U;
            continue;
        }
        ulTaxableDiff += MaintWork.TaxTbl.TaxData.auchTblData[i];

        /* check if input amount is over max setting taxable amount */
        if (ulTaxableAmount + 1 < ulTaxableDiff) {
            return(LDT_KEYOVER_ADR);
        }

        /* check if input amount is corresponding */
        if (ulTaxableAmount + 1 == ulTaxableDiff) {
            ulMaintPreTaxable = pData->ulEndTaxableAmount;
            usMaintSavedTax = usTax;
            uchMaintTblOff = i;
            uchMaintTaxCtl &= ~MAINT_FEEDCTL;                   /* reset flag */

            /* control flag */
            /* in case input amount is below WDC */
            if (ulMaintPreTaxable < ( ULONG)(100 * MaintWork.TaxTbl.TaxData.auchTblData[usOffset])) {
                uchMaintTaxCtl &= ~MAINT_SETTAXONWDC;
            }

            /* in case input amount is below 2*WDC */
            if (ulMaintPreTaxable < ( ULONG)(2 * 100 * MaintWork.TaxTbl.TaxData.auchTblData[usOffset])) {
                uchMaintTaxCtl &= ~MAINT_OVER2WDC;
            }

            /* display the amount that precede the correct taxable amount */
            MaintTaxTbl.uchMajorClass = uchMajorClass;
            MaintTaxTbl.uchMinorClass = CLASS_MAINTTAXTBL_DISP2_TAX;
            MaintTaxTbl.ulEndTaxableAmount = ulMaintPreTaxable;
            MaintTaxTbl.usTaxAmount = usMaintSavedTax;
            RflGetLead (MaintTaxTbl.aszMnemonics, LDT_DATA_ADR);
            MaintTaxTbl.usPrtControl = ( PRT_JOURNAL | PRT_RECEIPT );
            DispWrite(&MaintTaxTbl);
            return(SUCCESS);
        }

        /* in case input amount is over the sum of the difference of taxable */
        usTax++;                                                /* increase tax ammount */
    }
    return(LDT_KEYOVER_ADR);                                    /* wrong data */
}

/*
*===========================================================================
** Synopsis:    VOID MaintTaxTblWriteNo1( VOID )
*
*     Input:    Nothing
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function writes TAX TABLE No.1 in SYSTEM RAM
*===========================================================================
*/

VOID MaintTaxTblWriteNo1( VOID )
{
    UCHAR       uchRestTbl;

    /* calculate rest table size */
    uchRestTbl = ( UCHAR)(MAX_TTBLDATA_SIZE - (usMaintTbl1Len
                        + usMaintTbl2Len + usMaintTbl3Len + usMaintTbl4Len));

    /* set tax table #1 length */
    MaintWork.TaxTbl.TaxData.ausTblLen[0] = ( USHORT)(uchMaintTblOff + 1);

    /* modify tax table */
    memmove(&MaintWork.TaxTbl.TaxData.auchTblData[uchMaintTblOff + 1],
            &MaintWork.TaxTbl.TaxData.auchTblData[usMaintTbl1Len + uchRestTbl],
            usMaintTbl2Len + usMaintTbl3Len + usMaintTbl4Len);

    /* write tax table */
    MaintWork.TaxTbl.uchMajorClass = CLASS_PARATAXTBL1;
    CliParaWrite(&MaintWork.TaxTbl);
}

/*
*===========================================================================
** Synopsis:    VOID MaintTaxTblDeleteNo1( VOID )
*
*     Input:    Nothing
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function delete TAX TABLE #1 from SYSTEM RAM
*===========================================================================
*/

VOID MaintTaxTblDeleteNo1( VOID )
{
    MaintTaxTblDelete(CLASS_PARATAXTBL1);
}

/*
*===========================================================================
** Synopsis:    VOID MaintTaxTblDelete( UCHAR uchMajorClass )
*
*     Input:    uchMajorClass : major class
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function delete TAX TABLE from SYSTEM RAM
*===========================================================================
*/

VOID MaintTaxTblDelete( UCHAR uchMajorClass )
{

	UCHAR           uchRestTbl;
	PARATAXTBL      ParaTaxTbl = {0};
	MAINTTRANS      MaintTrans = {0};
    UCHAR           uchACCode;

    /* calculate rest table size */
    uchRestTbl = ( UCHAR)(MAX_TTBLDATA_SIZE - (usMaintTbl1Len
                          + usMaintTbl2Len + usMaintTbl3Len + usMaintTbl4Len));

    /* set major class */
    ParaTaxTbl.uchMajorClass = uchMajorClass;

    /* set modified tax table */
    switch (uchMajorClass) {
    case CLASS_PARATAXTBL1:
        uchACCode = AC_TAXTBL1;
        MaintWork.TaxTbl.TaxData.ausTblLen[0] = 0;
        memcpy(&ParaTaxTbl.TaxData.auchTblData[0],
               &MaintWork.TaxTbl.TaxData.auchTblData[usMaintTbl1Len + uchRestTbl],
               usMaintTbl2Len + usMaintTbl3Len + usMaintTbl4Len);
        break;

    case CLASS_PARATAXTBL2:
        uchACCode = AC_TAXTBL2;
        MaintWork.TaxTbl.TaxData.ausTblLen[1] = 0;
        memcpy(ParaTaxTbl.TaxData.auchTblData,
               MaintWork.TaxTbl.TaxData.auchTblData,
               usMaintTbl1Len);
        memcpy(&ParaTaxTbl.TaxData.auchTblData[usMaintTbl1Len],
               &MaintWork.TaxTbl.TaxData.auchTblData[usMaintTbl1Len
                    + usMaintTbl2Len + uchRestTbl],
               usMaintTbl3Len + usMaintTbl4Len);
        break;

    case CLASS_PARATAXTBL3:
        uchACCode = AC_TAXTBL3;
        MaintWork.TaxTbl.TaxData.ausTblLen[2] = 0;
        memcpy(ParaTaxTbl.TaxData.auchTblData,
               MaintWork.TaxTbl.TaxData.auchTblData,
               usMaintTbl1Len + usMaintTbl2Len);
        memcpy(&ParaTaxTbl.TaxData.auchTblData[usMaintTbl1Len + usMaintTbl2Len],
               &MaintWork.TaxTbl.TaxData.auchTblData[usMaintTbl1Len
                    + usMaintTbl2Len + usMaintTbl3Len + uchRestTbl],
               usMaintTbl4Len);
        break;

    default:
        uchACCode = AC_TAXTBL4;
        MaintWork.TaxTbl.TaxData.ausTblLen[3] = 0;
        memcpy(ParaTaxTbl.TaxData.auchTblData,
               MaintWork.TaxTbl.TaxData.auchTblData,
               usMaintTbl1Len + usMaintTbl2Len + usMaintTbl3Len);
        break;
    }

    memcpy(&ParaTaxTbl.TaxData.ausTblLen,
           &MaintWork.TaxTbl.TaxData.ausTblLen,
           sizeof(ParaTaxTbl.TaxData.ausTblLen));

    /* write tax table */
    CliParaWrite(&ParaTaxTbl);

    /* control header item */
    MaintHeaderCtl(uchACCode, RPT_ACT_ADR);

    /* print "DELETE" using structure MAINTTRANS */
    MaintTrans.uchMajorClass = CLASS_MAINTTRANS;
    MaintTrans.usPrtControl = ( PRT_JOURNAL | PRT_RECEIPT );
    RflGetTranMnem (MaintTrans.aszTransMnemo, TRN_DEL_ADR);
    PrtPrintItem(NULL, &MaintTrans);

    /* print trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintTaxTblCalNo1(MAINTTAXTBL *pData)
*
*     Input:    *pData          : pointer to structure for MAINTTAXTBL
*                  (pData->ulEndTaxableAmount)
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               LDT_KEYOVER_ADR : wrong data
*
** Description: This function caluculate & print TAX no.1.
*===========================================================================
*/

SHORT MaintTaxTblCalNo1(MAINTTAXTBL *pData)
{
    return(MaintTaxTblCal(pData, CLASS_MAINTTAXTBL1, RFL_USTAX_1));
}

/*
*===========================================================================
** Synopsis:    SHORT MaintTaxTblCal(MAINTTAXTBL *pData, UCHAR uchMajorClass,
*                                    USHORT usType)
*
*     Input:    *pData          : pointer to structure for MAINTTAXTBL
*                  (pData->ulEndTaxableAmount)
*               uchMajorClass : major class
*               usType : Tax No.
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               LDT_KEYOVER_ADR : wrong data
*
** Description: This function caluculate & print TAX.
*===========================================================================
*/

SHORT MaintTaxTblCal(MAINTTAXTBL *pData, UCHAR uchMajorClass, USHORT usType)
{
    DCURRENCY     lTaxAmount;
	MAINTTAXTBL   MaintTaxTbl = {0};

    /* calculate tax */
    if (RflTax2(&lTaxAmount, pData->ulEndTaxableAmount, usType, &MaintWork.TaxTbl.TaxData) == RFL_FAIL) {
/*        PifAbort(MODULE_MAINT_LOG_ID, FAULT_INVALID_DATA); */
        return(LDT_KEYOVER_ADR);
    }

    /* display test data */
    MaintTaxTbl.uchMajorClass = uchMajorClass;
    MaintTaxTbl.uchMinorClass = CLASS_MAINTTAXTBL_DISP4_TAX;
    MaintTaxTbl.ulEndTaxableAmount = pData->ulEndTaxableAmount;
    MaintTaxTbl.usTaxAmount = ( USHORT)(lTaxAmount);
    RflGetLead (MaintTaxTbl.aszMnemonics, LDT_DATA_ADR);
    DispWrite(&MaintTaxTbl);

    /* 1 line feed */
    if (!(uchMaintTaxCtl & MAINT_FEEDCTL)) {
        MaintFeed();
        uchMaintTaxCtl |= MAINT_FEEDCTL;

        MldIRptItemOther(1);    /* feed 1 line on LCD */
    }

    /* print test data */
    MaintTaxTbl.uchMinorClass = CLASS_MAINTTAXTBL_PRTTAXABLE;
    MaintTaxTbl.usPrtControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /******************/
    /* Display to MLD */
    /******************/
    MldDispItem(&MaintTaxTbl,0);

    PrtPrintItem(NULL,&MaintTaxTbl);

    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintTaxTblTmpReportNo1( VOID )
*
*     Input:    Nothing
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function print TAX TABLE No.1 REPORT during tax table creation
*===========================================================================
*/

VOID MaintTaxTblTmpReportNo1( VOID )
{
    MaintTaxTblTmpReport(CLASS_MAINTTAXTBL1, 0, 0);
}

/*
*===========================================================================
** Synopsis:    VOID MaintTaxTblTmpReport(UCHAR uchMajorClass,
*                                         USHORT usOffset, UCHAR uchOffTblLen)
*
*     Input:    uchMajorClass : major class
*               usOffset : offset of file
*               uchOffTblLen : offset of table length
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function print TAX TABLE REPORT during tax table creation
*===========================================================================
*/

VOID MaintTaxTblTmpReport(UCHAR uchMajorClass, USHORT usOffset, UCHAR uchOffTblLen )
{
    UCHAR         uchRptTmpFlag = 0;
    USHORT        usTaxAmount = 0;
    USHORT        i;
    ULONG         ulStartTaxable = 0U;
    ULONG         ulEndTaxable = 0U;
    ULONG         ulPreEndTaxable = 0U;
	MAINTTAXTBL   MaintTaxTbl = {0};

    /* 1 line feed */
    MaintFeed();

    /* set tax rate */
    MaintTaxTbl.uchInteger = MaintWork.TaxTbl.TaxData.auchTblData[usOffset + 2];
    MaintTaxTbl.uchDenominator = MaintWork.TaxTbl.TaxData.auchTblData[usOffset + 3];
    MaintTaxTbl.uchNumerator = MaintWork.TaxTbl.TaxData.auchTblData[usOffset + 4];

    /* print tax rate */
    MaintTaxTbl.uchMajorClass = uchMajorClass;
    MaintTaxTbl.uchMinorClass = CLASS_MAINTTAXTBL_PRTRPTINT;
    MaintTaxTbl.usPrtControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /******************/
    /* Display to MLD */
    /******************/
    MldDispItem(&MaintTaxTbl,0);

    PrtPrintItem(NULL,&MaintTaxTbl);

    /* print all tax table */
    for (i = usOffset + MAX_TTBLRATE_SIZE;
         i < ( USHORT)(MaintWork.TaxTbl.TaxData.ausTblLen[uchOffTblLen]
          + usOffset); i++) {

        /* check tax table */
        /* check if "255" is set */
        if (MaintWork.TaxTbl.TaxData.auchTblData[i] == 255U) {
            ulEndTaxable += MaintWork.TaxTbl.TaxData.auchTblData[i];
            continue;
        }

        /* check if "0" is set */
        if (MaintWork.TaxTbl.TaxData.auchTblData[i] == 0) {
            /* check if "0" is set for the passed tax */
            if (MaintWork.TaxTbl.TaxData.auchTblData[i - 1] != 255U) {
                usTaxAmount++;
                continue;
            }
        }

        /* check 1st tax table */
        if (!uchRptTmpFlag) {
            ulStartTaxable = 0U;
            ulEndTaxable += MaintWork.TaxTbl.TaxData.auchTblData[i] - 1;
            uchRptTmpFlag = 1;
        } else {
            ulStartTaxable = ulPreEndTaxable + 1;
            ulEndTaxable += MaintWork.TaxTbl.TaxData.auchTblData[i];
            usTaxAmount++;
        }
        ulPreEndTaxable = ulEndTaxable;

        /* print tax table */
        MaintTaxTbl.uchMajorClass = uchMajorClass;
        MaintTaxTbl.uchMinorClass = CLASS_MAINTTAXTBL_PRTTAX;
        MaintTaxTbl.ulStartTaxableAmount = ulStartTaxable;
        MaintTaxTbl.ulEndTaxableAmount = ulEndTaxable;
        MaintTaxTbl.usTaxAmount = usTaxAmount;
        MaintTaxTbl.usPrtControl = ( PRT_JOURNAL | PRT_RECEIPT );

        /******************/
        /* Display to MLD */
        /******************/
        MldDispItem(&MaintTaxTbl,0);

        PrtPrintItem(NULL,&MaintTaxTbl);
    }

    /* display for inputing test data */
    MaintTaxTbl.uchMajorClass = uchMajorClass;
    MaintTaxTbl.uchMinorClass = CLASS_MAINTTAXTBL_DISP4_INT;
    RflGetLead (MaintTaxTbl.aszMnemonics, LDT_DATA_ADR);
    DispWrite(&MaintTaxTbl);
}


/*
*===========================================================================
** Synopsis:    VOID MaintTaxTblReportNo1( VOID )
*
*     Input:    Nothing
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function print TAX TABLE NO.1 REPORT
*===========================================================================
*/

VOID MaintTaxTblReportNo1( VOID )
{
    MaintTaxTblReport(CLASS_MAINTTAXTBL1);
}

/*
*===========================================================================
** Synopsis:    VOID MaintTaxTblReport( UCHAR uchMajorClass )
*
*     Input:    uchMajorClass : major class
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function print TAX TABLE REPORT
*===========================================================================
*/

VOID MaintTaxTblReport(UCHAR uchMajorClass)
{
    UCHAR       uchRptFlag = 0;
    USHORT      usTaxAmount = 0;
    USHORT      i;
    ULONG       ulStartTaxable = 0U;
    ULONG       ulEndTaxable = 0U;
    ULONG       ulPreEndTaxable = 0U;
    PARATAXTBL  ParaTaxTbl = {0};
	MAINTTAXTBL MaintTaxTbl = {0};
    USHORT          usOffset;
    USHORT          usOffTblLen;
    UCHAR           uchACCode;

    switch (uchMajorClass) {
    case CLASS_MAINTTAXTBL1:
        usOffset = 0;
        usOffTblLen = 0;
        uchACCode = AC_TAXTBL1;
        ParaTaxTbl.uchMajorClass = CLASS_PARATAXTBL1;
        break;

    case CLASS_MAINTTAXTBL2:
        usOffset = usMaintTbl1Len;
        usOffTblLen = 1;
        uchACCode = AC_TAXTBL2;
        ParaTaxTbl.uchMajorClass = CLASS_PARATAXTBL2;
        break;
	case CLASS_MAINTTAXTBL4:
		usOffset = usMaintTbl1Len + usMaintTbl2Len;
		usOffTblLen = 2;
		uchACCode = AC_TAXTBL4;
		ParaTaxTbl.uchMajorClass = CLASS_PARATAXTBL4;
		break;

    default:
        usOffset = usMaintTbl1Len + usMaintTbl2Len;
        usOffTblLen = 2;
        uchACCode = AC_TAXTBL3;
        ParaTaxTbl.uchMajorClass = CLASS_PARATAXTBL3;
        break;
    }

    /* control header item */
    MaintHeaderCtl(uchACCode, RPT_ACT_ADR);

    /* read tax table */
    CliParaRead(&ParaTaxTbl);                                       /* call ParaTaxRateRead() */

    /* check if tax exist or not */
    if (!(ParaTaxTbl.TaxData.ausTblLen[usOffTblLen])) {             /* tax do not exist */
        MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
        return;
    }

    /* set tax rate */
    MaintTaxTbl.uchInteger = ParaTaxTbl.TaxData.auchTblData[usOffset + 2];
    MaintTaxTbl.uchDenominator = ParaTaxTbl.TaxData.auchTblData[usOffset + 3];
    MaintTaxTbl.uchNumerator = ParaTaxTbl.TaxData.auchTblData[usOffset + 4];

    /* print tax rate */
    MaintTaxTbl.uchMajorClass = uchMajorClass;
    MaintTaxTbl.uchMinorClass = CLASS_MAINTTAXTBL_PRTRPTINT;
    MaintTaxTbl.usPrtControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL,&MaintTaxTbl);

    /* print all tax table */
    for (i = (usOffset + MAX_TTBLRATE_SIZE);
         i < (usOffset + ParaTaxTbl.TaxData.ausTblLen[usOffTblLen]); i++) {

        /* check abort key */
        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            MaintMakeAbortKey();
            break;
        }

        /* check tax table */
        /* check if "255" is set */
        if (ParaTaxTbl.TaxData.auchTblData[i] == 255U) {
            ulEndTaxable += ParaTaxTbl.TaxData.auchTblData[i];
            continue;
        }

        /* check if "0" is set */
        if (ParaTaxTbl.TaxData.auchTblData[i] == 0) {
            /* check if "0" is set for the passed tax */
            if (ParaTaxTbl.TaxData.auchTblData[i - 1] != 255U) {
                usTaxAmount++;
                continue;
            }
        }

        /* check 1st tax table */
        if (!uchRptFlag) {
            ulStartTaxable = 0U;
            ulEndTaxable += ParaTaxTbl.TaxData.auchTblData[i] - 1;
            uchRptFlag = 1;
        } else {
            ulStartTaxable = ulPreEndTaxable + 1;
            ulEndTaxable += ParaTaxTbl.TaxData.auchTblData[i];
            usTaxAmount++;
        }
        ulPreEndTaxable = ulEndTaxable;

        /* print tax table */
        MaintTaxTbl.uchMajorClass = uchMajorClass;
        MaintTaxTbl.uchMinorClass = CLASS_MAINTTAXTBL_PRTTAX;
        MaintTaxTbl.ulStartTaxableAmount = ulStartTaxable;
        MaintTaxTbl.ulEndTaxableAmount = ulEndTaxable;
        MaintTaxTbl.usTaxAmount = usTaxAmount;
        MaintTaxTbl.usPrtControl = ( PRT_JOURNAL | PRT_RECEIPT );
        PrtPrintItem(NULL,&MaintTaxTbl);
    }

    /* print trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
}

/****** End of Source ******/

