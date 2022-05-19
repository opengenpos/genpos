/*
		Copyright	Georgia Southern University
					Statesboro,  GA 30460

					NCR Project Development Team
*==========================================================================
* Title       : Tax Table Programming Module( SUPER & PROGRAM MODE )                       
* Category    : Display-2x20, NCR 2170 US Hospitality Application Program        
* Program Name: DSPTXT20.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*               
*               DispSupTaxTbl20()   : This Function Displays Tax Table Data. 
*                                   
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-20-92: 00.00.01 : K.You     : initial                                   
* Dec-09-93: 02.00.03 : K.You     : del. tax table #4.                                   
* Dec-18-95: 03.01.00 : M.Ozawa   : recover tax table #4.
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
/* #include <pif.h> */
/* #include <log.h> */
#include "paraequ.h"
#include "para.h"
#include "maint.h"
/* #include <appllog.h> */
#include "display.h"

#include "disp20.h"


/*
*===========================================================================
** Synopsis:    VOID DispSupTaxTbl20( MAINTTAXTBL *pData )
*               
*     Input:    *pData          : Pointer to Structure for MAINTTAXTBL
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Displays Tax Table Data.
*===========================================================================
*/

VOID DispSupTaxTbl20( MAINTTAXTBL *pData )
{
    /* Define Display Format */ 

    /* for Echo Back */
    static const TCHAR FARCONST auchDispSupTaxTblEchoCom[] = _T("%-3u %1u %14s\n%-18s\t%u"); 
                                                            
	TCHAR   aszBuffer[PARA_LEADTHRU_LEN + 1] = {0};
    TCHAR   aszDspNull[1] = {_T('\0')};
    USHORT  usACNumber;
    USHORT  usType;

	NHPOS_ASSERT_ARRAYSIZE(pData->aszMnemonics, PARA_LEADTHRU_LEN);
	NHPOS_ASSERT(sizeof(aszBuffer[0]) == sizeof(pData->aszMnemonics[0]));

    /* Save Data */
	DispSupSaveData.MaintTaxTbl = *pData;
    
    /* distinguish major class */
    switch(pData->uchMajorClass) {
    case CLASS_MAINTTAXTBL1:
        usACNumber = AC_TAXTBL1;
        break;

    case CLASS_MAINTTAXTBL2:
        usACNumber = AC_TAXTBL2;
        break;

    case CLASS_MAINTTAXTBL3:
        usACNumber = AC_TAXTBL3;
        break;

    default:    /* CLASS_MAINTTAXTBL4 */
        usACNumber = AC_TAXTBL4;
        break;
    }

    _tcsncpy(aszBuffer, pData->aszMnemonics, PARA_LEADTHRU_LEN);    /* Set Mnemonics */

    /* set type by minor class */
    switch (pData->uchMinorClass) {
    case CLASS_MAINTTAXTBL_DISP0_INT:
        usType = DISPSUP_TAXTBL_TYPE1;
        break;

    case CLASS_MAINTTAXTBL_DISP2_INT:
    case CLASS_MAINTTAXTBL_DISP2_TAXABL:
    case CLASS_MAINTTAXTBL_DISP2_TAX:
        usType = DISPSUP_TAXTBL_TYPE2;
        break;

    case CLASS_MAINTTAXTBL_DISP3_TAXABL:
        usType = DISPSUP_TAXTBL_TYPE3;
        break;

    /* case CLASS_MAINTTAXTBL_DISP4_INT:
    case CLASS_MAINTTAXTBL_DISP4_TAX:
        usType = DISPSUP_TAXTBL_TYPE4;
        break; */

    default:
        return;
/*        PifAbort(MODULE_DISPSUP_ID, FAULT_INVALID_DATA); */

    }

    /* distinguish minor class */

    switch (pData->uchMinorClass) {
    case CLASS_MAINTTAXTBL_DISP0_INT:
    case CLASS_MAINTTAXTBL_DISP2_INT:
    case CLASS_MAINTTAXTBL_DISP3_TAXABL:
    case CLASS_MAINTTAXTBL_DISP4_INT:
		{
			static const TCHAR FARCONST auchDispSupTaxTbl20A[] = _T("%-3u %1u\t%1u\n%-18s\t%u");

			UiePrintf(UIE_OPER,                         /* Operator Display */
					  0,                                /* Row */
					  0,                                /* Column */
					  auchDispSupTaxTbl20A,             /* Format */
					  usACNumber,                       /* Program number */
					  usType,                           /* Type */
					  0L,                               /* Zero, LONG according to format. */
					  aszBuffer,                        /* Lead through */
					  uchMaintMenuPage);                /* Page Number */
		}
        break;

    case CLASS_MAINTTAXTBL_DISP2_TAXABL:
		{
			static const TCHAR FARCONST auchDispSupTaxTbl20B[] = _T("%-3u %1u %6l$\t%1u\n%-18s\t%u");
			static const TCHAR FARCONST auchDispSupTaxTblEcho20B[] = _T("%-3u %1u %6l$%8s\n%-18s\t%u");

			UiePrintf(UIE_OPER,                         /* Operator Display */
					  0,                                /* Row */
					  0,                                /* Column */
					  auchDispSupTaxTbl20B,             /* Format */
					  usACNumber,                       /* Program number */
					  usType,                           /* Type */
					  (DCURRENCY)pData->ulEndTaxableAmount,        /* End Taxable Amount, format is %6l$ which requires DCURRENCY value */
					  0,                                /* Zero */  
					  aszBuffer,                        /* Lead through */
					  uchMaintMenuPage);                /* Page Number */

			/* Display BackGround */
			UieEchoBackGround(UIE_KEEP,                     /* Descriptor Control */
							 UIE_ATTR_NORMAL,              /* Attribute */
							 auchDispSupTaxTblEcho20B,     /* Format */
							 usACNumber,                   /* AC Number */
							 usType,                       /* Type */
							 (DCURRENCY)pData->ulEndTaxableAmount,    /* End Taxable, format is %6l$ which requires DCURRENCY value */
							 aszDspNull,                   /* Null */
							 aszBuffer,                    /* Lead through */    
							 uchMaintMenuPage);            /* Page Number */
		}
        return;

    case CLASS_MAINTTAXTBL_DISP2_TAX:
    case CLASS_MAINTTAXTBL_DISP4_TAX:
		{
			static const TCHAR FARCONST auchDispSupTaxTbl20C[] = _T("%-3u %1u %6l$\t%4$\n%-18s\t%u");

			UiePrintf(UIE_OPER,                         /* Operator Display */
					  0,                                /* Row */
					  0,                                /* Column */
					  auchDispSupTaxTbl20C,             /* Format */
					  usACNumber,                       /* Program number */
					  usType,                           /* Type */
					  (DCURRENCY)pData->ulEndTaxableAmount,  /* End Taxable Amount, format is %6l$ which requires DCURRENCY value */
					  ( SHORT)pData->usTaxAmount,            /* Tax Amount, format is %4$ which requires SHORT and not DCURRENCY value */                  
					  aszBuffer,                        /* Lead through */
					  uchMaintMenuPage);                /* Page Number */
		}
        break;

    default:
        return;
/*        PifAbort(MODULE_DISPSUP_ID, FAULT_INVALID_DATA); */
    }

    /* Display BackGround */
    UieEchoBackGround(UIE_KEEP,                 /* Descriptor Control */
                      UIE_ATTR_NORMAL,          /* Attribute */
                      auchDispSupTaxTblEchoCom, /* Format */
                      usACNumber,               /* AC Number */
                      usType,                   /* Type */
                      aszDspNull,               /* Null */
                      aszBuffer,                /* Lead through */    
                      uchMaintMenuPage);        /* Page Number */
}
