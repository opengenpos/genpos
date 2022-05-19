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
* Title       : MaintCurrency Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATCUR.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function read & write & report FOREIGN CURRENCY RATE.
*                                  
*           The provided function names are as follows: 
* 
*              MaintCurrencyRead()   : read & display FOREIGN CURRENCY RATE
*              MaintCurrencyWrite()  : set & print FOREIGN CURRENCY RATE
*              MaintCurrencyReport() : print all FOREIGN CURRENCY RATE REPORT  
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-18-92: 00.00.01 : K.You     : initial                                   
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
#include <ecr.h>
#include <uie.h>
#include <rfl.h>
#include <paraequ.h> 
#include <para.h>
#include <maint.h> 
#include <display.h> 
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <csstbpar.h>

#include "maintram.h" 

/*
*===========================================================================
** Synopsis:    SHORT MaintCurrencyRead( PARACURRENCYTBL *pData )
*               
*     Input:    *pData          : pointer to structure for PARACURRENCYTBL
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS          : read data
*               LDT_KEYOVER_ADR  : wrong data
*
** Description: This function reads & displays FOREIGN CURRENCY RATE.
*===========================================================================
*/

SHORT MaintCurrencyRead( PARACURRENCYTBL *pData )
{
    UCHAR   uchAddress;

    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        uchAddress = ++MaintWork.CurrencyTbl.uchAddress;

        /* check address */
        if (uchAddress > CNV_ADR_MAX) { 
            uchAddress = 1;                                     /* initialize address */
        } 
    } else {                                                    /* with data */
        uchAddress = pData->uchAddress;
    }

    /* check address */
    if (uchAddress < 1 || CNV_ADR_MAX < uchAddress) {
        return(LDT_KEYOVER_ADR);                                /* wrong data */
    }
    MaintWork.CurrencyTbl.uchMajorClass = pData->uchMajorClass; /* copy major class */
    MaintWork.CurrencyTbl.uchAddress = uchAddress;
    CliParaRead(&(MaintWork.CurrencyTbl));

	RflGetLead(MaintWork.CurrencyTbl.aszMnemonics, LDT_DATA_ADR);

    /* 6 digit decimal point for Euro, V3.4 */
    if (MaintCurrencyMDCRead(MaintWork.CurrencyTbl.uchAddress) & ODD_MDC_BIT0) {
        MaintWork.CurrencyTbl.uchStatus |= MAINT_6DECIMAL_DATA;
    } else {
        MaintWork.CurrencyTbl.uchStatus &= ~MAINT_6DECIMAL_DATA;
    }
    DispWrite(&(MaintWork.CurrencyTbl));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintCurrencyWrite( PARACURRENCYTBL *pData )
*               
*     Input:    *pData             : pointer to structure for PARACURRENCYTBL
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS            : read data
*               LDT_SEQERR_ADR     : error
*
** Description: This function sets & prints FOREIGN CURRENCY RATE.
*===========================================================================
*/

SHORT MaintCurrencyWrite( PARACURRENCYTBL *pData )
{
    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        return(LDT_SEQERR_ADR);                                 /* sequence error */
    }

    /*  foreign currency range has already checked by UI */
    MaintWork.CurrencyTbl.ulForeignCurrency = pData->ulForeignCurrency;
    CliParaWrite(&(MaintWork.CurrencyTbl));                     /* call ParaCurrencyWrite() */

    /* control header item */
    MaintHeaderCtl(AC_CURRENCY, RPT_ACT_ADR);

    /* set journal bit & receipt bit */
    MaintWork.CurrencyTbl.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* 6 digit decimal point for Euro, V3.4 */
    if (MaintCurrencyMDCRead(MaintWork.CurrencyTbl.uchAddress) & ODD_MDC_BIT0) {
        MaintWork.CurrencyTbl.uchStatus |= MAINT_6DECIMAL_DATA;
    } else {
        MaintWork.CurrencyTbl.uchStatus &= ~MAINT_6DECIMAL_DATA;
    }
    PrtPrintItem(NULL, &(MaintWork.CurrencyTbl));
    
    /* set address for display next address */
    MaintWork.CurrencyTbl.uchAddress++;

    /* check address */
    if (MaintWork.CurrencyTbl.uchAddress > CNV_ADR_MAX) {
        MaintWork.CurrencyTbl.uchAddress = 1;                   /* initialize address */
    }
    MaintCurrencyRead(&(MaintWork.CurrencyTbl));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintCurrencyReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing 
*
** Description: This function prints all FOREIGN CURRENCY RATE REPORT.
*===========================================================================
*/

VOID MaintCurrencyReport( VOID )
{
    UCHAR                i;
	PARACURRENCYTBL      ParaCurrencyTbl = { 0 };

    /* control header item */
    MaintHeaderCtl(AC_CURRENCY, RPT_ACT_ADR);

    /* set major class */
    ParaCurrencyTbl.uchMajorClass = CLASS_PARACURRENCYTBL;

    /* set journal bit & receipt bit */
    ParaCurrencyTbl.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* set data at every address */
    for (i = 1; i <= CNV_ADR_MAX; i++) {

        /* check abort key */
        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            MaintMakeAbortKey();
            break;
        }
        ParaCurrencyTbl.uchAddress = i;
        CliParaRead(&ParaCurrencyTbl);
        /* 6 digit decimal point for Euro, V2.2 */
        if (MaintCurrencyMDCRead(i) & ODD_MDC_BIT0) {
            ParaCurrencyTbl.uchStatus |= MAINT_6DECIMAL_DATA;
        } else {
            ParaCurrencyTbl.uchStatus &= ~MAINT_6DECIMAL_DATA;
        }
        PrtPrintItem(NULL, &ParaCurrencyTbl);
    }

    /* make trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);

}

/*
*===========================================================================
** Synopsis:    VOID MaintCurrencyReadMDC( UCHAR *puchMDC )
*               
*     Input:    *pData          : pointer to structure for PARACURRENCYTBL
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS          : read data
*               LDT_KEYOVER_ADR  : wrong data
*
** Description: This function reads & displays FOREIGN CURRENCY RATE.
*===========================================================================
*/

UCHAR MaintCurrencyMDCRead(UCHAR uchAddress )
{
    SHORT   usClassOffset;
	PARAMDC WorkMDC = { 0 };

    WorkMDC.uchMajorClass = CLASS_PARAMDC;
    usClassOffset     = uchAddress - CNV_NO1_ADR;
    usClassOffset     /= 2;
    WorkMDC.usAddress = MDC_EUROFC_ADR + usClassOffset ;
    CliParaRead( &WorkMDC );

    if (uchAddress == CNV_NO1_ADR) {
        WorkMDC.uchMDCData >>= 4;
    }
    if (uchAddress == CNV_NO2_ADR) {
        WorkMDC.uchMDCData >>= 6;
    }

    return WorkMDC.uchMDCData;
}

