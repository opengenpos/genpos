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
* Title       : Tax Calculation                 
* Category    : Reentrant Functions Library, NCR 2170   Application
* Program Name: RflTax.C                                                      
* --------------------------------------------------------------------------
* Abstruct:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* May-08-92 : 00.00.01 : K.Nakajima :                                    
*           :          :            :                                    
* 
*** NCR2171 **
* Aug-30-99 : 01.00.00 : M.Teraki   : initial (for Win32)
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
;+                                                                      +
;+                  I N C L U D E     F I L E s                         +
;+                                                                      +
;========================================================================
**/
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>

#include <ecr.h>
#include <pif.h>
#include <log.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <rfl.h>
#include "rflin.h"


/**
;========================================================================
;+                                                                      +
;+        P R O G R A M    D E C L A R A T I O N s                      +
;+                                                                      +
;========================================================================
**/
/**fh
;============================================================================
;
;   function : Tax Calculation
;
;	Format   : SHORT RflTax1(LONG *plTax,
;                            LONG   lAmount,
;                            USHORT Type);
;
;   input    : LONG lAmount,          -Original value
;              USHORT Type            -TaxTableNum
;
;
;   return   : SHORT   RFL_SUCCESS    - Tax Calc. Success
;                      RFL_FAIL       - Tax Calc. Fail
;
;   output   : SHORT  *psTax          - Tax value adrress
;
;   synopis  : This function calculates Tax with tax table number 
;              
;============================================================================
fh**/
SHORT  RflTax1(DCURRENCY *plTax, DCURRENCY lAmount, USHORT usType)
{
    PARATAXTBL  TaxTbl;         

    TaxTbl.uchMajorClass = CLASS_PARATAXTBL1;    /* class set */
    CliParaRead(&TaxTbl);                        /* taxtable read */
    
	return RflTax2 ( plTax, lAmount, usType, &(TaxTbl.TaxData) );
}


/**fh
;============================================================================
;
;   function : Tax Calculation
;
;	Format   : SHORT RflTax2(LONG *plTax,
;                            LONG lAmount,
;                            USHORT  usType
;                            VOID *ptdata);
;
;   input    : LONG     lAmount,      -Original value
;              USHORT   usType        -TaxTable numer
;              VOID     *ptdata       -Tax Data
;              
;   output   : LONG     *plTax        - Tax value adrress
;
;   return   : SHORT    RFL_SUCCESS   - Tax Calc. Success
;                       RFL_FAIL      - Tax Calc. Fail
;
;   synopis  : This function calculates Tax with taxtable. 
;              
;              
;  Taxtable has following structure.            
; +------------------------------------------------------------- - - --------------------- - -             
; | type1  | type2  | type3  | type1 | type1    |type1  | type1       | type2 | type2    |
; | length | length | length |  WDC  | WDConTax | rate  | taxdata     |  WDC  | WDConTax |
; +------------------------------------------------------------- - - --------------------- - -             
;              
;============================================================================
fh**/
SHORT  RflTax2(DCURRENCY *plTax, DCURRENCY lAmount, USHORT usType, VOID *ptdata)      /* Calc. Tax with TaxTable */
{
    UCHAR       *puchTax;                 /* tax data address */
    USHORT      usTblLen;                 /* tax table length */
    BOOL        fTaxSign;                 /* tax sign flag */
    DCURRENCY   lDiff;                    /* difference */  
    DCURRENCY   lWork;                    /* sum */
    SHORT       sWDC;                     /* whole dollar constant */
    TAXDATA     *pTax = ptdata;

    /* -- get table offset & table length -- */
    switch (usType) {
        case RFL_USTAX_1:
            puchTax  = &pTax->auchTblData[0];
            usTblLen = pTax->ausTblLen[0];
            break;

        case RFL_USTAX_2:
            puchTax  = &pTax->auchTblData[ pTax->ausTblLen[0] ];
            usTblLen = pTax->ausTblLen[1];
            break;

        case RFL_USTAX_3:
            puchTax  = &pTax->auchTblData[ pTax->ausTblLen[0] + pTax->ausTblLen[1] ];
            usTblLen = pTax->ausTblLen[2];
            break;

        case RFL_USTAX_4:
            puchTax  = &pTax->auchTblData[ pTax->ausTblLen[0] + pTax->ausTblLen[1] + pTax->ausTblLen[2] ];
            usTblLen = pTax->ausTblLen[3];
            break;

        default:
			PifLog(MODULE_RFL_TAX, FAULT_INVALID_DATA);
			PifLog(MODULE_DATA_VALUE(MODULE_RFL_TAX), usType);
            return (RFL_FAIL);
    }

    if (usTblLen == 0) {
        *plTax = 0L;
        return(RFL_SUCCESS);
    }
    
    if (lAmount < 0) {
        /* -- change sign -- */
        fTaxSign = RFL_MINUS;
        lAmount = RflLLabs(lAmount);        /* -- get absolute value -- */
    } else {
        fTaxSign = RFL_PLUS;
    }


	/* ------------------------ *\
		 Get  Tax Amount 
	\* ------------------------ */
    
    sWDC = puchTax[RFL_WDC] * 100;

    /* -- in case of "Amount > WDC" -- */
    if (lAmount > sWDC) {
        lDiff  = lAmount - sWDC;
        lWork  = (lDiff % sWDC) + sWDC;
        *plTax = (lDiff / sWDC) * puchTax[RFL_TAXONWDC];
		/* -- in case of "Amount <= WDC" -- */
    } else {
        lWork  = lAmount;
        *plTax = 0L;
    }

	if (usTblLen >= RFL_WDCLEN) {
		USHORT      i;

		for ( i = 0; i < usTblLen - RFL_WDCLEN; i++) {
			if ( (lWork = lWork - puchTax[RFL_WDCLEN + i]) < 0) {
				/* -- restore sign -- */
				if (fTaxSign == RFL_MINUS) {
					*plTax = -(*plTax);
				}
				return(RFL_SUCCESS);
			}

			if ( puchTax[RFL_WDCLEN + i] != 0xFF) {
				(*plTax)++;
			}
		}

		{
			char xBuff[256];

			sprintf (xBuff, "PifAbort() in RflTax2(): usType=%d, ausTblLen[0]=%d, ausTblLen[1]=%d, ausTblLen[2]=%d, ausTblLen[3]=%d", usType, pTax->ausTblLen[0], pTax->ausTblLen[1], pTax->ausTblLen[2], pTax->ausTblLen[3]);
			NHPOS_ASSERT_TEXT(0, xBuff);
		}
		/* -- abort program -- */
		PifLog(MODULE_RFL_TAX, FAULT_INVALID_DATA);
		PifLog(MODULE_DATA_VALUE(MODULE_RFL_TAX), usType);
		PifLog(MODULE_DATA_VALUE(MODULE_RFL_TAX), usTblLen);
		PifLog(MODULE_DATA_VALUE(MODULE_RFL_TAX), i);
		PifAbort(MODULE_RFL_TAX, FAULT_INVALID_DATA);
	}
	else {
		PifLog(MODULE_RFL_TAX, FAULT_INVALID_DATA);
		PifLog(MODULE_DATA_VALUE(MODULE_RFL_TAX), usType);
		PifLog(MODULE_DATA_VALUE(MODULE_RFL_TAX), usTblLen);
	}
    return (RFL_FAIL);
}
