/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Thermal Print PLU File Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSPLRPT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               PrtThrmSupPLUFile() : forms PLU File print format.       
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-16-93: 01.00.12 : J.IKEDA   : Initial
* Aug-25-93: 01.00.13 : J.IKEDA   : Maint. Format 4A21&4B20 Argument
* Dec-28-99: 01.00.00 : hrkato    : Saratoga
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
/* #include <pif.h> */
#include <rfl.h>
/* #include <log.h> */
#include <paraequ.h> 
#include <para.h>
/* #include <cswai.h> */
#include <maint.h> 
#include <regstrct.h>
#include <transact.h>
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>
/* #include <appllog.h> */

#include "prtrin.h"
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtThrmSupPLUFile( RPTPLU *pData )  
*               
*     Input:    *pData  : pointer to structure for RPTPLU 
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms PLU File print format.
*
*                    : PLU SALES READ REPORT (except START/END TIME)
*                    : PLU ALL RESET REPORT  (except START/END TIME)
*
*        cf. START/END TIME formats are same as REGISTER FINANCIAL REPORT.
*
*        See also function MldRptSupPLUFile() which generates a similar report.
*
*    WARNING:    This function uses print formats whose behavior depends on the
*                interpretation of format specifiers of function _RflFormStr().
*                Any changes as to variable type or the behavior of the function
*                _RflFormStr() will affect the displayed output.
*
*                A format specifier such as %6.2$ which requires a SHORT is being used to print
*                a percentage value and a format specifier such as %7.*l$ is being used to print
*                a scale weight (the * in the specifier allows for an additional argument to
*                specify the actual number of decimal places which depends on whether the scale
*                weighs hundredths or thousandths of a weight unit such as pounds or kilos)
*===========================================================================
*/
VOID  PrtThrmSupPLUFile( RPTPLU *pData )  
{
    /* define print format */ 
    static const TCHAR FARCONST auchPrtThrmSupPLUFile1[] = _T("%17u %-12s");
    static const TCHAR FARCONST auchPrtThrmSupPLUFile6[] = _T("%21u");
    static const TCHAR FARCONST auchNumber[] = _T("%s");                /* Saratoga */
	//GroupReport - CSMALL
	static const TCHAR FARCONST auchPrtThrmSupPLUGroup1[] = _T("**** OEP #: %d ****");
	static const TCHAR FARCONST auchPrtThrmSupPLUGroup2[] = _T("%14.14s %-20s %4d");

    /* UCHAR   aszDoubRepoNumb[4 * 2 + 1]; */
	TCHAR   aszRepoNumb[PLU_MAX_DIGIT + 1] = { 0 };
    UCHAR   uchDecimal = 2;  /* in case of "pData->uchStatus" is RPT_DECIMAL_2 */
    TCHAR   aszPrtNull[1] = {'\0'};

    /* check print control */
    if (pData->usPrintControl & PRT_RECEIPT) {
        /* initialize aszDoubRepoNumb */
        /* memset(aszDoubRepoNumb, '\0', sizeof(aszDoubRepoNumb)); */
       
        /* distinguish minor class */
        switch(pData->uchMinorClass) {
		case CLASS_RPTPLUGROUP_PRTGRP:
            PrtPrintf(PMG_PRT_RECEIPT,              /* printer type */
                      auchPrtThrmSupPLUGroup1,       /* format */
                      pData->usReportCode);             /* Group # */
            break;
		case CLASS_RPTPLUGROUP_PRTPLU:
            PrtPrintf(PMG_PRT_RECEIPT,              /* printer type */
                      auchPrtThrmSupPLUGroup2,       /* format */
                      pData->auchPLUNumber,          /* PLU number */
                      pData->aszMnemo,
					  pData->usDEPTNumber);             /* PLU name */
            break;
        case CLASS_RPTPLU_PRTDEPT:
            /* print DEPT in PLU report,    Saratoga */
            PrtPrintf(PMG_PRT_RECEIPT,              /* printer type */
                      auchPrtThrmSupPLUFile1,       /* format */
                      pData->usDEPTNumber,          /* DEPT number, Saratoga */
                      pData->aszMnemo);             /* DEPT name */
            break;

        case CLASS_RPTPLU_PRTTTLOPEN:        
            /* convert PLU No. to double wide,  Saratoga */
            RflConvertPLU(aszRepoNumb, pData->auchPLUNumber);
       
            /* check if PERCENT is illegal */
            if (pData->usPercent > PRT_MAX_PERCENT) {           /* In case that percent data is illegal,    */
                                                                /* pData->usPercent is set over 600.00 data */
                /* check if OPEN PLU is SCALABLE */
                if (pData->uchStatus == RPT_DECIMAL_0) {        /* in case of NON-SCALABLE */
					static const TCHAR FARCONST auchPrtThrmSupPLUFile2A0[] = _T("%1s%13s %-20s\n\t*****%%\n\t%6ld   %12l$");

                    /* print OPEN PLU with illegal percent data */
                    PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                              auchPrtThrmSupPLUFile2A0,         /* format */
                              aszPrtNull,                       /* null */
                              aszRepoNumb,                      /* PLU number */
                              pData->aszMnemo,                  /* PLU name */
                              pData->PLUTotal.lCounter,         /* counter */
                              pData->PLUTotal.lAmount);         /* amount */
                } else {                                        /* in case of SCALABLE */
					static const TCHAR FARCONST auchPrtThrmSupPLUFile2A1[] = _T("%1s%13s %-20s\n\t*****%%\n\t%7.*l$%s   %12l$");

                    if (pData->uchStatus == RPT_DECIMAL_1) {
                        uchDecimal = 1;                         
                    }
       
                    /* print OPEN PLU(SCALABLE) with illegal percent data */
                    PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                              auchPrtThrmSupPLUFile2A1,         /* format */
                              aszPrtNull,                       /* null */
                              aszRepoNumb,                      /* PLU number */
                              pData->aszMnemo,                  /* PLU name */
                              uchDecimal,                       /* decimal places count */
                              (DCURRENCY)pData->PLUTotal.lCounter,  /* counter, format is %7.*l$ which requires number decimals and DCURRENCY value */
                              pData->aszSpeMnemo,               /* special mnemonics */
                              pData->PLUTotal.lAmount);         /* amount */
                }
            } else {                                            /* in case of legal percent data */
                /* check if PLU is SCALABLE */
                if (pData->uchStatus == RPT_DECIMAL_0) {        /* in case of NON-SCALABLE */
					static const TCHAR FARCONST auchPrtThrmSupPLUFile2B0[] = _T("%1s%13s %-20s\n\t%6.2$%%\n\t%6ld   %12l$");

                    /* print OPEN PLU with legal percent data */
                    PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                              auchPrtThrmSupPLUFile2B0,         /* format */
                              aszPrtNull,                       /* null */
                              aszRepoNumb,                      /* PLU number */
                              pData->aszMnemo,                  /* PLU name */
                              ( SHORT)pData->usPercent,         /* percent, format is %6.2$ which requires SHORT and not DCURRENCY */
                              pData->PLUTotal.lCounter,         /* counter */
                              pData->PLUTotal.lAmount);         /* amount */
                } else {
					static const TCHAR FARCONST auchPrtThrmSupPLUFile2B1[] = _T("%1s%13s %-20s\n\t%6.2$%%\n\t%7.*l$%s   %12l$");

                    if (pData->uchStatus == RPT_DECIMAL_1) {
                        uchDecimal = 1;                         
                    }
       
                    /* print OPEN PLU (SCALABLE) with legal percent data */
                    PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                              auchPrtThrmSupPLUFile2B1,         /* format */
                              aszPrtNull,                       /* null */
                              aszRepoNumb,                      /* PLU number */
                              pData->aszMnemo,                  /* PLU name */
                              ( SHORT)pData->usPercent,         /* percent, format is %6.2$ which requires SHORT and not DCURRENCY */
                              uchDecimal,                       /* decimal places count */
                              (DCURRENCY)pData->PLUTotal.lCounter,  /* counter, format is %7.*l$ which requires number decimals and DCURRENCY value */
                              pData->aszSpeMnemo,               /* special mnemonics */
                              pData->PLUTotal.lAmount);         /* amount */
                }
            }
            break;                                          
       
        case CLASS_RPTPLU_PRTTTLCREDIT:        
			{
				static const TCHAR FARCONST auchPrtThrmSupPLUFile2C0[] = _T("%1s%13s %-20s\n\t%6ld   %12l$");

				/* convert PLU No. to double wide,  Saratoga */
				RflConvertPLU(aszRepoNumb, pData->auchPLUNumber);
       
				/* print CREDIT PLU */
				PrtPrintf(PMG_PRT_RECEIPT,                      /* printer type */
						  auchPrtThrmSupPLUFile2C0,             /* format */
						  aszPrtNull,                           /* null */
						  aszRepoNumb,                          /* PLU number */
						  pData->aszMnemo,                      /* PLU name */
						  pData->PLUTotal.lCounter,             /* counter */
						  pData->PLUTotal.lAmount);             /* amount */
			}
            break;                                          
       
        case CLASS_RPTPLU_PRTTTLPRSET:
            /* convert PLU No. to double wide,  Saratoga */
            RflConvertPLU(aszRepoNumb, pData->auchPLUNumber);
       
            /* check if PERCENT is illegal */
            if (pData->usPercent > PRT_MAX_PERCENT) {           /* In case that percent data is illegal,     */
                                                                /* pData->usPercent is set over 600.00 data */
                /* check if PLU is SCALABLE */
                if (pData->uchStatus == RPT_DECIMAL_0) {        /* in case of NON-SCALABLE */
					static const TCHAR FARCONST auchPrtThrmSupPLUFile3A0[] = _T("%1s%13s %-20s\n\t*****%%\n@%9l$\t%6ld   %12l$");

                    /* print NON ADJECTIVE PLU with illegal percent data */
                    PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                              auchPrtThrmSupPLUFile3A0,         /* format */
                              aszPrtNull,                       /* null */
                              aszRepoNumb,                      /* PLU number */
                              pData->aszMnemo,                  /* PLU name */
                              pData->lPresetPrice,              /* unit price, format is %9l$ which requires DCURRENCY value */
                              pData->PLUTotal.lCounter,         /* counter */
                              pData->PLUTotal.lAmount);         /* amount */
                } else {                                        /* in case of SCALABLE */
					static const TCHAR FARCONST auchPrtThrmSupPLUFile3A1[] = _T("%1s%13s %-20s\n\t*****%%\n@%9l$\t%7.*l$%s   %12l$");

                    if (pData->uchStatus == RPT_DECIMAL_1) {
                        uchDecimal = 1;
                    }
       
                    /* print NON ADJECTIVE PLU (SCALABLE) with illegal percent data */
                    PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                              auchPrtThrmSupPLUFile3A1,         /* format */
                              aszPrtNull,                       /* null */
                              aszRepoNumb,                      /* PLU number */
                              pData->aszMnemo,                  /* PLU name */
                              pData->lPresetPrice,              /* unit price, format is %9l$ which requires DCURRENCY value */
                              uchDecimal,                       /* decimal places count */
                              (DCURRENCY)pData->PLUTotal.lCounter, /* counter, format is %7.*l$ which requires number decimals and DCURRENCY value */
                              pData->aszSpeMnemo,               /* special mnemonics */
                              pData->PLUTotal.lAmount);         /* amount */
                }
            } else {                                            /* in case of legal percent data */
				static const TCHAR FARCONST auchPrtThrmSupPLUFile3B0[] = _T("%1s%13s %-20s\n\t%6.2$%%\n@%9l$\t%6ld   %12l$");

                /* check if PLU is SCALABLE */
                if (pData->uchStatus == RPT_DECIMAL_0) {        /* in case of NON-SCALABLE */
                    /* print NON ADJECTIVE PLU with legal percent data */
                    PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                              auchPrtThrmSupPLUFile3B0,         /* format */
                              aszPrtNull,                       /* null */
                              aszRepoNumb,                      /* PLU number */
                              pData->aszMnemo,                  /* PLU name */
                              ( SHORT)pData->usPercent,         /* percent */
                              pData->lPresetPrice,              /* unit price, format is %9l$ which requires DCURRENCY value */
                              pData->PLUTotal.lCounter,         /* counter */
                              pData->PLUTotal.lAmount);         /* amount */
                } else {                                        /* in case of SCALABLE */
					static const TCHAR FARCONST auchPrtThrmSupPLUFile3B1[] = _T("%1s%13s %-20s\n\t%6.2$%%\n@%9l$\t%7.*l$%s   %12l$");

					if (pData->uchStatus == RPT_DECIMAL_1) {
                        uchDecimal = 1;
                    }
       
                    /* print NON ADJECTIVE PLU (SCALABLE) with legal percent data */
                    PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                              auchPrtThrmSupPLUFile3B1,         /* format */
                              aszPrtNull,                       /* null */
                              aszRepoNumb,                      /* PLU number */
                              pData->aszMnemo,                  /* PLU name */
                              ( SHORT)pData->usPercent,         /* percent */
                              pData->lPresetPrice,              /* unit price, format is %9l$ which requires DCURRENCY value */
                              uchDecimal,                       /* decimal places count */
                              (DCURRENCY)pData->PLUTotal.lCounter,  /* counter, format is %7.*l$ which requires number decimals and DCURRENCY value */
                              pData->aszSpeMnemo,               /* special mnemonics */
                              pData->PLUTotal.lAmount);         /* amount */
                }
            }
            break;                                          
       
        case CLASS_RPTPLU_PRTTTLCOUPON:
			{
				static const TCHAR FARCONST auchPrtThrmSupPLUFile3C0[] = _T("%1s%13s %-20s\n@%9l$\t%6ld   %12l$");

				/* convert PLU No. to double wide,  Saratoga */
				RflConvertPLU(aszRepoNumb, pData->auchPLUNumber);

				/* print NON ADJECTIVE COUPON PLU */
				PrtPrintf(PMG_PRT_RECEIPT,                      /* printer type */
						  auchPrtThrmSupPLUFile3C0,             /* format */
						  aszPrtNull,                           /* null */
						  aszRepoNumb,                          /* PLU number */
						  pData->aszMnemo,                      /* PLU name */
						  pData->lPresetPrice,                  /* unit price, format is %9l$ which requires DCURRENCY value */
						  pData->PLUTotal.lCounter,             /* counter */
						  pData->PLUTotal.lAmount);             /* amount */
			}
            break;                                          

        case CLASS_RPTPLU_PRTTTLADJ1:  /* ONE ADJECTIVE PLU with PLU name/number */
            /* convert PLU No. to double wide,  Saratoga */
            RflConvertPLU(aszRepoNumb, pData->auchPLUNumber);
       
            /* check if PERCENT is illegal */
            if (pData->usPercent > PRT_MAX_PERCENT) {           /* In case that percent data is illegal,     */
                                                                /* pData->usPercent is set over 600.00 data */
                /* check if PLU is SCALABLE */
                if (pData->uchStatus == RPT_DECIMAL_0) {        /* in case of NON_SCALABLE */
					static const TCHAR FARCONST auchPrtThrmSupPLUFile4A10[] = _T("%1s%13s %-20s\n%4s\t*****%%\n@%9l$\t%6ld   %12l$");

                    /* print ONE ADJECTIVE PLU with illegal percent data */
                    PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                              auchPrtThrmSupPLUFile4A10,        /* format */
                              aszPrtNull,                       /* null */
                              aszRepoNumb,                      /* PLU number */
                              pData->aszMnemo,                  /* PLU name */
                              pData->aszAdjMnemo,               /* adjective mnemonics */
                              pData->lPresetPrice,              /* unit price, format is %9l$ which requires DCURRENCY value */
                              pData->PLUTotal.lCounter,         /* counter */
                              pData->PLUTotal.lAmount);         /* amount */
                } else {                                        /* in case of SCALABLE */
					static const TCHAR FARCONST auchPrtThrmSupPLUFile4A11[] = _T("%1s%13s %-20s\n%4s\t*****%%\n@%9l$\t%7.*l$%s   %12l$");

                    if (pData->uchStatus == RPT_DECIMAL_1) {
                        uchDecimal = 1;
                    }

                    /* print ONE ADJECTIVE PLU (SCALABLE) with illegal percent data */
                    PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                              auchPrtThrmSupPLUFile4A11,        /* format */
                              aszPrtNull,                       /* null */
                              aszRepoNumb,                      /* PLU number */
                              pData->aszMnemo,                  /* PLU name */
                              pData->aszAdjMnemo,               /* adjective mnemonics */
                              pData->lPresetPrice,              /* unit price, format is %9l$ which requires DCURRENCY value */
                              uchDecimal,                       /* decimal places count */
                              (DCURRENCY)pData->PLUTotal.lCounter, /* counter, format is %7.*l$ which requires number decimals and DCURRENCY value */
                              pData->aszSpeMnemo,               /* special mnemonics */
                              pData->PLUTotal.lAmount);         /* amount */
                }
            } else {
                /* check if PLU is SCALABLE */
                if (pData->uchStatus == RPT_DECIMAL_0) {        /* in case of NON_SCALABLE */
					static const TCHAR FARCONST auchPrtThrmSupPLUFile4B10[] = _T("%1s%13s %-20s\n%4s\t%6.2$%%\n@%9l$\t%6ld   %12l$");

                    /* print ONE ADJECTIVE PLU with legal percent data */
                    PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                              auchPrtThrmSupPLUFile4B10,        /* format */
                              aszPrtNull,                       /* null */
                              aszRepoNumb,                      /* PLU number */
                              pData->aszMnemo,                  /* PLU name */
                              pData->aszAdjMnemo,               /* adjective mnemonics */
                              ( SHORT)pData->usPercent,         /* percent, format is %6.2$ which requires SHORT and not DCURRENCY value */
                              pData->lPresetPrice,              /* unit price, format is %9l$ which requires DCURRENCY value */
                              pData->PLUTotal.lCounter,         /* counter */
                              pData->PLUTotal.lAmount);         /* amount */
                } else {                                        /* in case of SCALABLE */
					static const TCHAR FARCONST auchPrtThrmSupPLUFile4B11[] = _T("%1s%13s %-20s\n%4s\t%6.2$%%\n@%9l$\t%7.*l$%s   %12l$");

                    if (pData->uchStatus == RPT_DECIMAL_1) {
                        uchDecimal = 1;
                    }

                    /* print ONE ADJECTIVE PLU (SCALABLE) with legal percent data */
                    PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                              auchPrtThrmSupPLUFile4B11,        /* format */
                              aszPrtNull,                       /* null */
                              aszRepoNumb,                      /* PLU number */
                              pData->aszMnemo,                  /* PLU name */
                              pData->aszAdjMnemo,               /* adjective mnemonics */
                              ( SHORT)pData->usPercent,         /* percent, format is %6.2$ which requires SHORT and not DCURRENCY value */
                              pData->lPresetPrice,              /* unit price, format is %9l$ which requires DCURRENCY value */
                              uchDecimal,                       /* decimal places count */
                              (DCURRENCY)pData->PLUTotal.lCounter, /* counter, format is %7.*l$ which requires number decimals and DCURRENCY value */
                              pData->aszSpeMnemo,               /* special mnemonics */
                              pData->PLUTotal.lAmount);         /* amount */
                }
            }
            break;

        case CLASS_RPTPLU_PRTTTLADJ2:  /* ONE ADJECTIVE PLU without PLU name/number */
            /* check if PERCENT is illegal */

            if (pData->usPercent > PRT_MAX_PERCENT) {           /* In case that percent data is illegal,    */
                                                                /* pData->usPercent is set over 600.00 data */
                /* check if PLU is SCALABLE */
                if (pData->uchStatus == RPT_DECIMAL_0) {        /* in case of NON_SCALABLE */
					static const TCHAR FARCONST auchPrtThrmSupPLUFile4A20[] = _T("%4s\t*****%%\n@%9l$\t%6ld   %12l$");

                    /* print ONE ADJECTIVE PLU with illegal percent data */
                    PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                              auchPrtThrmSupPLUFile4A20,        /* format */
                              pData->aszAdjMnemo,               /* adjective mnemonics */
                              pData->lPresetPrice,              /* unit price, format is %9l$ which requires DCURRENCY value */
                              pData->PLUTotal.lCounter,         /* counter */
                              pData->PLUTotal.lAmount);         /* amount */
                } else {                                        /* in case of SCALABLE */
					static const TCHAR FARCONST auchPrtThrmSupPLUFile4A21[] = _T("%4s\t*****%%\n@%9l$\t%7.*l$%s   %12l$");

                    if (pData->uchStatus == RPT_DECIMAL_1) {
                        uchDecimal = 1;
                    }

                    /* print ONE ADJECTIVE PLU (SCALABLE) with illegal percent data */
                    PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                              auchPrtThrmSupPLUFile4A21,        /* format */
                              pData->aszAdjMnemo,               /* adjective mnemonics */
                              pData->lPresetPrice,              /* unit price, format is %9l$ which requires DCURRENCY value */
                              uchDecimal,                       /* decimal places count */
                              (DCURRENCY)pData->PLUTotal.lCounter,  /* counter, format is %7.*l$ which requires number decimals and DCURRENCY value */
                              pData->aszSpeMnemo,               /* special mnemonics */
                              pData->PLUTotal.lAmount);         /* amount */
                }
            } else {
                /* check if PLU is SCALABLE */                  
                if (pData->uchStatus == RPT_DECIMAL_0) {        /* in case of SCALABLE */
					static const TCHAR FARCONST auchPrtThrmSupPLUFile4B20[] = _T("%4s\t%6.2$%%\n@%9l$\t%6ld   %12l$");

                    /* print ONE ADJECTIVE PLU with legal percent data */
                    PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                              auchPrtThrmSupPLUFile4B20,        /* format */
                              pData->aszAdjMnemo,               /* adjective mnemonics */
                              ( SHORT)pData->usPercent,         /* percent, format is %6.2$ which requires SHORT and not DCURRENCY value */
                              pData->lPresetPrice,              /* unit price, format is %9l$ which requires DCURRENCY value */
                              pData->PLUTotal.lCounter,         /* counter */
                              pData->PLUTotal.lAmount);         
                } else {                                        /* in case of SCALABLE */
					static const TCHAR FARCONST auchPrtThrmSupPLUFile4B21[] = _T("%4s\t%6.2$%%\n@%9l$\t%7.*l$%s   %12l$");

                    if (pData->uchStatus == RPT_DECIMAL_1) {
                        uchDecimal = 1;
                    }

                    /* print ONE ADJECTIVE PLU (SCALABLE) with legal percent data */
                    PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                              auchPrtThrmSupPLUFile4B21,        /* format */
                              pData->aszAdjMnemo,               /* adjective mnemonics */
                              ( SHORT)pData->usPercent,         /* percent, format is %6.2$ which requires SHORT and not DCURRENCY value */
                              pData->lPresetPrice,              /* unit price, format is %9l$ which requires DCURRENCY value */
                              uchDecimal,                       /* decimal places count */
                              (DCURRENCY)pData->PLUTotal.lCounter,  /* counter, format is %9l$ which requires DCURRENCY value */
                              pData->aszSpeMnemo,               /* special mnemonics */
                              pData->PLUTotal.lAmount);         /* amount */
                }
            }
            break;

        case CLASS_RPTPLU_PRTTTLMINUSADJ1:  /* ONE ADJECTIVE COUPON PLU with PLU name/number */
			{
				static const TCHAR FARCONST auchPrtThrmSupPLUFile4C10[] = _T("%1s%13s %-20s\n%4s\n@%9l$\t%6ld   %12l$");

				/* convert PLU No. to double wide,  Saratoga */
				RflConvertPLU(aszRepoNumb, pData->auchPLUNumber);
       
				/* print ONE ADJECTIVE COUPON PLU */
				PrtPrintf(PMG_PRT_RECEIPT,                          /* printer type */
						  auchPrtThrmSupPLUFile4C10,                /* format */
						  aszPrtNull,                               /* null */
						  aszRepoNumb,                              /* PLU number */
						  pData->aszMnemo,                          /* PLU name */
						  pData->aszAdjMnemo,                       /* adjective mnemonics */
						  pData->lPresetPrice,                      /* unit price, format is %9l$ which requires DCURRENCY value */
						  pData->PLUTotal.lCounter,                 /* counter */
						  pData->PLUTotal.lAmount);                 /* amount */
			}
            break;

        case CLASS_RPTPLU_PRTTTLMINUSADJ2:  /* ONE ADJECTIVE COUPON PLU without PLU name/number */
            /* print ONE ADJECTIVE COUPON PLU */
			{
				static const TCHAR FARCONST auchPrtThrmSupPLUFile4C20[] = _T("%4s\n@%9l$\t%6ld   %12l$");

				PrtPrintf(PMG_PRT_RECEIPT,                          /* printer type */
						  auchPrtThrmSupPLUFile4C20,                /* format */
						  pData->aszAdjMnemo,                       /* adjective mnemonics */
						  pData->lPresetPrice,                      /* unit price, format is %9l$ which requires DCURRENCY value */
						  pData->PLUTotal.lCounter,                 /* counter */
						  pData->PLUTotal.lAmount);                 /* amount */
			}
            break;

        case CLASS_RPTPLU_PRTTTLCNT:
            /* check if TOTAL is SCALABLE */
            if (pData->uchStatus == RPT_DECIMAL_0) {            /* in case of NON-SCALABLE */
				static const TCHAR FARCONST auchPrtThrmSupPLUFile50[] = _T("%-8.8s\t%6ld   %12l$");/*8 characters JHHJ*/

                /* print SUBTOTAL/PLUS/MINUS/HASH/GRAND/CIGARETTE TOTAL */
                PrtPrintf(PMG_PRT_RECEIPT,                      /* printer type */
                          auchPrtThrmSupPLUFile50,              /* format */
                          pData->aszMnemo,                      /* transaction mnemonic */
                          pData->PLUTotal.lCounter,             /* counter */
                          pData->PLUTotal.lAmount);             /* amount */

            } else {                                            /* in case of SCALABLE */
				static const TCHAR FARCONST auchPrtThrmSupPLUFile51[] = _T("%-8.8s\t%7.*l$%s   %12l$");/*8 characters JHHJ*/

                if (pData->uchStatus == RPT_DECIMAL_1) {
                    uchDecimal = 1;
                }
       
                /* print SUBTOTAL/PLUS/MINUS/HASH/GRAND/CIGARETTE TOTAL */
                PrtPrintf(PMG_PRT_RECEIPT,                      /* printer type */
                          auchPrtThrmSupPLUFile51,              /* format */
                          pData->aszMnemo,                      /* transaction mnemonic */
                          uchDecimal,                           /* decimal places count */
                          (DCURRENCY)pData->PLUTotal.lCounter,  /* counter, format is %9l$ which requires DCURRENCY value */
                          pData->aszSpeMnemo,                   /* special mnemonics */
                          pData->PLUTotal.lAmount);             /* amount */
            }
            break;
       
        case CLASS_RPTPLU_PRTCODE:
            /* print REPORT CODE in PLU report */
            PrtPrintf(PMG_PRT_RECEIPT,                      /* printer type */
                      auchPrtThrmSupPLUFile6,               /* format */
                      pData->usReportCode);                 /* report code,sARATOGA */
            break;
       
        default:
/*          PifLog(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_ERROR); */
            break;
        }
    }          
       
    /* check print control */

    if (pData->usPrintControl & PRT_JOURNAL) {
        return;
    }
}

/***** End of Source *****/