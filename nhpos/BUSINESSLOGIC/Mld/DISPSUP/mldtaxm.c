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
* Title       : Muiti Line Display For Tax Module ( SUPER MODE )
* Category    : Print, NCR 2170 US Hospitality Application Program
* Program Name: MLDTAX.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Midium Model
* Options     : /c /AM /W4 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:
*
*        MldParaTaxDisp()  : This Function Forms Tax Table Print Format.
*
* --------------------------------------------------------------------------
* Update Histories
*    Date  : Ver.Rev. :   Name    : Description
* May-29-95: 03.00.00 : T.Satoh   : initial
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
=============================================================================
;                      I N C L U D E     F I L E s
=============================================================================
**/
#include	<tchar.h>
#include <stdlib.h>
#include <string.h>
#include <ecr.h>
#include <uie.h>
#include <pif.h>
#include <rfl.h>
#include <string.h>
#include <log.h>
#include <paraequ.h> 
#include <para.h>
#include <maint.h> 
#include <display.h> 
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <csop.h>
#include <csstbpar.h>
#include <csstbopr.h>
#include <csgcs.h>
#include <csstbgcf.h>
#include <csttl.h>
#include <csstbttl.h>
#include <appllog.h>

#include <report.h>
#include <mld.h>
#include <mldsup.h> 

#include "maintram.h" 

/*
*===========================================================================
*   MLD Format of U.S. TAX Maintenance Modeule
*===========================================================================
*/

/*
*===========================================================================
** Synopsis:    VOID  MldParaTaxDisp( MAINTTAXTBL *pTax )  
*
*     Input:    *pTax  : Pointer to Structure for MAINTTAXTBL 
*                        pTax->uchMinorClass : CLASS_MAINTTAXTBL_PRTWRTINT
*                                              CLASS_MAINTTAXTBL_PRTTAXABLE
*                                              CLASS_MAINTTAXTBL_PRTRPTINT
*                                              CLASS_MAINTTAXTBL_PRTTAX
*
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This Function sets data and display to MLD.
*               Target A/C Number are 124,125 and 126.
*===========================================================================
*/

VOID  MldParaTaxDisp( MAINTTAXTBL *pTax )
{
	static CONST TCHAR FARCONST aszMldAC124E[][64] = {
		_T("....*....1....*....2....*....3....*....4"),
		/*                          "TAX RATE %4u.%02u%02u\n",                    */
		_T("%s %s\n"),
		/*                            "TAX RATE %s\n", */
		_T("\t%6l$       %6$ "),
		_T("")
	};

	static CONST TCHAR FARCONST aszMldAC124R[][64] = {
		_T("....*....1....*....2....*....3....*....4"),
		/*                          "TAX RATE %4u.%02u%02u\n",                    */
		_T("%s %s\n"),
		/*                            "TAX RATE %s\n", */
		_T("                   %s\t%s\n"),
		/*                            "                 SALES AMOUNT      TAX\n", */
		_T("               %6l$ -%6l$\t%6$ "),
		_T("")
	};

	TCHAR   aszTaxRate[16] = {0};     /* Tax Rate character strings               */
    TCHAR   aszInteger[8] = {0};      /* Part of Integer     strings of Tax Rate  */
    TCHAR   aszNumerator[8] = {0};    /* Part of Numerator   strings of Tax Rate  */
    TCHAR   aszDenominator[8] = {0};  /* Part of Denomimator strings of Tax Rate  */
	TCHAR   aszRateAmtMnem[PARA_TRANSMNEMO_LEN + 1] = {0};
    TCHAR   aszTaxMnem[PARA_TRANSMNEMO_LEN + 1] = {0};
    USHORT  i,usCpoint;

    /*************************************/
    /* Making Tax Rate Character Strings */
    /*************************************/
    _itot((USHORT)pTax->uchInteger,aszInteger,10);
    _itot((USHORT)pTax->uchNumerator,aszNumerator,10);
    _itot((USHORT)pTax->uchDenominator,aszDenominator,10);

    usCpoint = 0;

    /*
     * Part of Integer
     */
    if((USHORT)pTax->uchInteger == 0 ||
       (0 < (USHORT)pTax->uchInteger && (USHORT)pTax->uchInteger < 10)){
        aszTaxRate[usCpoint++] = aszInteger[0];
    }
    else{
        for(i=0;i<2;i++){
            if(aszInteger[i] == '\0')   break;
            aszTaxRate[usCpoint++] = aszInteger[i];
        }
    }

    aszTaxRate[usCpoint++] = _T('.');                   /* Add Dot character    */

    /*
     * Part of Numerator
     */
    if((USHORT)pTax->uchNumerator == 0){
        tcharnset(&(aszTaxRate[usCpoint]),_T('0'),2);
        //memset(&(aszTaxRate[usCpoint]),'0',2);
        usCpoint += 2;
    }
    else if(0 < (USHORT)pTax->uchNumerator && (USHORT)pTax->uchNumerator < 10){
        aszTaxRate[usCpoint++] = _T('0');
        aszTaxRate[usCpoint++] = aszNumerator[0];
    }
    else{
        for(i=0;i<2;i++){
            if(aszNumerator[i] == '\0')   break;
            aszTaxRate[usCpoint++] = aszNumerator[i];
        }
    }

    /*
     * Part of Denominator
     */
    if((USHORT)pTax->uchDenominator == 0){
        tcharnset(&(aszTaxRate[usCpoint]),_T('0'),2);
        //memset(&(aszTaxRate[usCpoint]),'0',2);
        usCpoint += 2;
    }
    else if(0 < (USHORT)pTax->uchDenominator && (USHORT)pTax->uchDenominator < 10){
        aszTaxRate[usCpoint++] = _T('0');
        aszTaxRate[usCpoint++] = aszDenominator[0];
    }
    else{
        for(i=0;i<2;i++){
            if(aszDenominator[i] == '\0')   break;
            aszTaxRate[usCpoint++] = aszDenominator[i];
        }
    }

    aszTaxRate[usCpoint] = '\0';                    /* Set Terminator       */

    /*************************/
    /* Branch by Minor Class */
    /*************************/
    switch(pTax->uchMinorClass){
                                            /********************************/
        case CLASS_MAINTTAXTBL_PRTWRTINT:   /* During Edit Table (1)        */
                                            /********************************/
            /********************/
            /* Display TAX RATE */
            /********************/
            MldScrollClear(MLD_SCROLL_1);           /* Clear Scroll Display */

            /*
            MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP+1,
                      &(aszMldAC124E[1][0]),(USHORT)pTax->uchInteger,
                      (USHORT)pTax->uchNumerator,(USHORT)pTax->uchDenominator);
            */

            /* get programmable mnemonics, V3.3 */
            RflGetTranMnem (aszRateAmtMnem, TRN_TAX_RATE_ADR);

            MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP+1,
                        &(aszMldAC124E[1][0]), aszRateAmtMnem, aszTaxRate);
            break;

                                            /********************************/
        case CLASS_MAINTTAXTBL_PRTTAXABLE:  /* During Edit Table (2)        */
                                            /********************************/
            /********************************************/
            /* Display HIGH SALES AMOUNT and TAX AMOUNT */
			// Using format of _T("\t%6l$       %6$ ") requires that the
			// ulEndTaxableAmount be a DCURRENCY.
            /********************************************/
            MldPrintf(MLD_SCROLL_1,MLD_SCROLL_APPEND,&(aszMldAC124E[2][0]),
                                  (DCURRENCY)pTax->ulEndTaxableAmount, (SHORT)pTax->usTaxAmount);
            break;

                                            /********************************/
        case CLASS_MAINTTAXTBL_PRTRPTINT:   /* During Show Report (1)       */
                                            /********************************/
            MldScrollClear(MLD_SCROLL_1);           /* Clear Scroll Display */

            /********************/
            /* Display TAX RATE */
            /********************/
            /*
            MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP,
                      &(aszMldAC124R[1][0]),(USHORT)pTax->uchInteger,
                      (USHORT)pTax->uchNumerator,(USHORT)pTax->uchDenominator);
            */
            /* get programmable mnemonics, V3.3 */
            RflGetTranMnem (aszRateAmtMnem, TRN_TAX_RATE_ADR);

            MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP+1,
                      &(aszMldAC124R[1][0]),aszRateAmtMnem, aszTaxRate);

            /* get programmable mnemonics, V3.3 */
            RflGetTranMnem (aszRateAmtMnem, TRN_AMTTL_ADR);
            RflGetTranMnem (aszTaxMnem, TRN_TAX_ADR);

            MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP+2,
                            &(aszMldAC124R[2][0]),
                            aszRateAmtMnem, aszTaxMnem); /* Display Telop        */
            break;

                                            /********************************/
        case CLASS_MAINTTAXTBL_PRTTAX:      /* During Show Report (2)       */
                                            /********************************/
            /*************************************************************/
            /* Display LOW SALES AMOUNT-HIGH SALES AMOUNT and TAX AMOUNT */
			// Using format of _T("               %6l$ -%6l$\t%6$ ") requires that both
			// ulStartTaxableAmount and ulEndTaxableAmount be a DCURRENCY.
			/*************************************************************/
            MldPrintf(MLD_SCROLL_1,MLD_SCROLL_APPEND,
                      &(aszMldAC124R[3][0]), (DCURRENCY)pTax->ulStartTaxableAmount,
                      (DCURRENCY)pTax->ulEndTaxableAmount, (SHORT)pTax->usTaxAmount);
            break;

        default:
            /*PifLog(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_ERROR);*/
            break;
    }

    return;
}
