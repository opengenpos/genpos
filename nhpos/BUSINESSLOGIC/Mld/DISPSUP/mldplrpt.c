/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Multiline Display PLU File Format  ( SUPER & PROGRAM MODE )
* Category    : Multiline Display, NCR 2170 US Hospitality Application Program        
* Program Name: MLDPLRPT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               MldRptSupPLUFile() : forms PLU File print format.       
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jul-26-95: 03.00.03 : M.Ozawa   : Initial
* Dec-27-99: 01.00.00 : hrkato    : Saratoga
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
#include <pif.h>
#include <rfl.h>
#include <log.h>
#include <paraequ.h> 
#include <para.h>
#include <cswai.h>
#include <maint.h> 
#include <regstrct.h>
#include <transact.h>
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>
#include <appllog.h>
#include <mld.h>
#include <mldsup.h>
#include <mldmenu.h>
#include <mldlocal.h>


/*
*===========================================================================
** Synopsis:    VOID  MldRptSupPLUFile( RPTPLU *pData, USHORT usStatus )  
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
*        See also function PrtThrmSupPLUFile() which generates a similar report.
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
SHORT  MldRptSupPLUFile( RPTPLU *pData, USHORT usStatus)
{
    /* define print format */ 
    static const TCHAR FARCONST auchNumber[] = _T("%s");            /* Saratoga */
	//GroupReport - CSMALL

	TCHAR  aszString[4 * (MLD_SUPER_MODE_CLM_LEN + 1)] = { 0 };           /* buffer for formatted data */
    TCHAR  *pszString;
	TCHAR   aszRepoNumb[PLU_MAX_DIGIT + 1] = { 0 };
    UCHAR   uchDecimal = 2;  /* in case of "pData->uchStatus" is RPT_DECIMAL_2 */
    TCHAR   aszPrtNull[1] = {'\0'};
    USHORT usRow=0;

    /* distinguish minor class */
    switch(pData->uchMinorClass) {

	//GroupReport - CSMALL
	case CLASS_RPTPLUGROUP_PRTGRP:
		{
			static const TCHAR FARCONST auchMldRptSupPLUGroup1[] = _T("**** OEP #: %d ****\n");

			usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupPLUGroup1, pData->usReportCode);
		}
        break;

	//GroupReport - CSMALL
	case CLASS_RPTPLUGROUP_PRTPLU:
		{
			static const TCHAR FARCONST auchMldRptSupPLUGroup2[] = _T("%14.14s %-20s %4d");

			usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupPLUGroup2, pData->auchPLUNumber, pData->aszMnemo, pData->usDEPTNumber);
		}
        break;

    case CLASS_RPTPLU_PRTDEPT:
        /* print DEPT in PLU report */
		{
			static const TCHAR FARCONST auchMldRptSupPLUFile1[] = _T("%17u %-20s");         /* Saratoga */

			usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupPLUFile1, pData->usDEPTNumber, pData->aszMnemo);
		}
        break;
       
    case CLASS_RPTPLU_PRTTTLOPEN:        
        /* convert PLU No. to double wide */
        RflConvertPLU(aszRepoNumb, pData->auchPLUNumber);
       
        /* check if PERCENT is illegal */
        if (pData->usPercent > PRT_MAX_PERCENT) {           /* In case that percent data is illegal,    */
                                                            /* pData->usPercent is set over 600.00 data */
            /* check if OPEN PLU is SCALABLE */
            if (pData->uchStatus == RPT_DECIMAL_0) {        /* in case of NON-SCALABLE */
				static const TCHAR FARCONST auchMldRptSupPLUFile2A0[] = _T("%1s%13s %-20s\n\t*****%%\n\t%6ld   %12l$");

				/* print OPEN PLU with illegal percent data */
                usRow = MldSPrintf(aszString,          /* display buffer */
                          MLD_SUPER_MODE_CLM_LEN,      /* column length */
                          auchMldRptSupPLUFile2A0,         /* format */
                          aszPrtNull,                       /* null */
                          aszRepoNumb,                      /* PLU number,  Saratoga */
                          pData->aszMnemo,                  /* PLU name */
                          pData->PLUTotal.lCounter,         /* counter */
                          pData->PLUTotal.lAmount);         /* amount */
            } else {                                        /* in case of SCALABLE */
				static const TCHAR FARCONST auchMldRptSupPLUFile2A1[] = _T("%1s%13s %-20s\n\t*****%%\n\t%7.*l$%s   %12l$");

				if (pData->uchStatus == RPT_DECIMAL_1) {
                    uchDecimal = 1;                         
                }

                /* print OPEN PLU(SCALABLE) with illegal percent data */
                usRow = MldSPrintf(aszString,          /* display buffer */
                          MLD_SUPER_MODE_CLM_LEN,      /* column length */
                          auchMldRptSupPLUFile2A1,         /* format */
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
				static const TCHAR FARCONST auchMldRptSupPLUFile2B0[] = _T("%1s%13s %-20s\n\t%6.2$%%\n\t%6ld   %12l$");

				/* print OPEN PLU with legal percent data */
                usRow = MldSPrintf(aszString,          /* display buffer */
                          MLD_SUPER_MODE_CLM_LEN,      /* column length */
                          auchMldRptSupPLUFile2B0,         /* format */
                          aszPrtNull,                       /* null */
                          aszRepoNumb,                      /* PLU number */
                          pData->aszMnemo,                  /* PLU name */
                          (SHORT)pData->usPercent,          /* percent, format is %6.2$ which requires SHORT and not DCURRENCY */
                          pData->PLUTotal.lCounter,         /* counter */
                          pData->PLUTotal.lAmount);         /* amount */
            } else {
				static const TCHAR FARCONST auchMldRptSupPLUFile2B1[] = _T("%1s%13s %-20s\n\t%6.2$%%\n\t%7.*l$%s   %12l$");

				if (pData->uchStatus == RPT_DECIMAL_1) {
                    uchDecimal = 1;                         
                }

                /* print OPEN PLU (SCALABLE) with legal percent data */
                usRow = MldSPrintf(aszString,          /* display buffer */
                          MLD_SUPER_MODE_CLM_LEN,      /* column length */
                          auchMldRptSupPLUFile2B1,         /* format */
                          aszPrtNull,                       /* null */
                          aszRepoNumb,                      /* PLU number */
                          pData->aszMnemo,                  /* PLU name */
					      (SHORT)pData->usPercent,          /* percent, format is %6.2$ which requires SHORT and not DCURRENCY */
                          uchDecimal,                       /* decimal places count */
                          (DCURRENCY)pData->PLUTotal.lCounter,  /* counter, format is %7.*l$ which requires number decimals and DCURRENCY value */
                          pData->aszSpeMnemo,               /* special mnemonics */
                          pData->PLUTotal.lAmount);         /* amount */
            }
        }
        break;                                          

    case CLASS_RPTPLU_PRTTTLCREDIT:        
		{
			static const TCHAR FARCONST auchMldRptSupPLUFile2C0[] = _T("%1s%13s %-20s\n\t%6ld   %12l$");

			/* convert PLU No. to double wide */
			RflSPrintf(aszRepoNumb, TCHARSIZEOF(aszRepoNumb), auchNumber, pData->auchPLUNumber); /* Saratoga */

			/* print CREDIT PLU */
			usRow = MldSPrintf(aszString,          /* display buffer */
					  MLD_SUPER_MODE_CLM_LEN,      /* column length */
					  auchMldRptSupPLUFile2C0,             /* format */
					  aszPrtNull,                           /* null */
					  aszRepoNumb,                          /* PLU number */
					  pData->aszMnemo,                      /* PLU name */
					  pData->PLUTotal.lCounter,             /* counter */
					  pData->PLUTotal.lAmount);             /* amount */
		}
        break;                                          

    case CLASS_RPTPLU_PRTTTLPRSET:
        /* convert PLU No. to double wide */
        RflSPrintf(aszRepoNumb, TCHARSIZEOF(aszRepoNumb), auchNumber, pData->auchPLUNumber); /* Saratoga */

        /* check if PERCENT is illegal */
        if (pData->usPercent > PRT_MAX_PERCENT) {           /* In case that percent data is illegal,     */
                                                            /* pData->usPercent is set over 600.00 data */
            /* check if PLU is SCALABLE */
            if (pData->uchStatus == RPT_DECIMAL_0) {        /* in case of NON-SCALABLE */
				static const TCHAR FARCONST auchMldRptSupPLUFile3A0[] = _T("%1s%13s %-20s\n\t*****%%\n@%9l$\t%6ld   %12l$");

				/* print NON ADJECTIVE PLU with illegal percent data */
                usRow = MldSPrintf(aszString,          /* display buffer */
                          MLD_SUPER_MODE_CLM_LEN,      /* column length */
                          auchMldRptSupPLUFile3A0,         /* format */
                          aszPrtNull,                       /* null */
                          aszRepoNumb,                      /* PLU number */
                          pData->aszMnemo,                  /* PLU name */
						  pData->lPresetPrice,              /* unit price */
                          pData->PLUTotal.lCounter,         /* counter */
                          pData->PLUTotal.lAmount);         /* amount */
            } else {                                        /* in case of SCALABLE */
				static const TCHAR FARCONST auchMldRptSupPLUFile3A1[] = _T("%1s%13s %-20s\n\t*****%%\n@%9l$\t%7.*l$%s   %12l$");

				if (pData->uchStatus == RPT_DECIMAL_1) {
                    uchDecimal = 1;
                }
       
                /* print NON ADJECTIVE PLU (SCALABLE) with illegal percent data */
                usRow = MldSPrintf(aszString,          /* display buffer */
                          MLD_SUPER_MODE_CLM_LEN,      /* column length */
                          auchMldRptSupPLUFile3A1,         /* format */
                          aszPrtNull,                       /* null */
                          aszRepoNumb,                      /* PLU number */
                          pData->aszMnemo,                  /* PLU name */
                          pData->lPresetPrice,              /* unit price, format is %9l$ which requires DCURRENCY value */
                          uchDecimal,                       /* decimal places count */
                          (DCURRENCY)pData->PLUTotal.lCounter,  /* counter, format is %7.*l$ which requires number decimals and DCURRENCY value */
                          pData->aszSpeMnemo,               /* special mnemonics */
                          pData->PLUTotal.lAmount);         /* amount */
            }
        } else {                                            /* in case of legal percent data */
            /* check if PLU is SCALABLE */
            if (pData->uchStatus == RPT_DECIMAL_0) {        /* in case of NON-SCALABLE */
				static const TCHAR FARCONST auchMldRptSupPLUFile3B0[] = _T("%1s%13s %-20s\n\t%6.2$%%\n@%9l$\t%6ld   %12l$");

				/* print NON ADJECTIVE PLU with legal percent data */
                usRow = MldSPrintf(aszString,          /* display buffer */
                          MLD_SUPER_MODE_CLM_LEN,      /* column length */
                          auchMldRptSupPLUFile3B0,         /* format */
                          aszPrtNull,                       /* null */
/*                          aszDoubRepoNumb,                  * PLU number */
                          aszRepoNumb,
                          pData->aszMnemo,                  /* PLU name */
                          ( SHORT)pData->usPercent,         /* percent, format is %6.2$ which requires SHORT and not DCURRENCY value */
                          pData->lPresetPrice,              /* unit price, format is %9l$ which requires DCURRENCY value */
                          pData->PLUTotal.lCounter,         /* counter */
                          pData->PLUTotal.lAmount);         /* amount */
            } else {                                        /* in case of SCALABLE */
				static const TCHAR FARCONST auchMldRptSupPLUFile3B1[] = _T("%1s%13s %-20s\n\t%6.2$%%\n@%9l$\t%7.*l$%s   %12l$");

				if (pData->uchStatus == RPT_DECIMAL_1) {
                    uchDecimal = 1;
                }

                /* print NON ADJECTIVE PLU (SCALABLE) with legal percent data */
                usRow = MldSPrintf(aszString,          /* display buffer */
                          MLD_SUPER_MODE_CLM_LEN,      /* column length */
                          auchMldRptSupPLUFile3B1,         /* format */
                          aszPrtNull,                       /* null */
                          aszRepoNumb,                      /* PLU number */
                          pData->aszMnemo,                  /* PLU name */
						  ( SHORT)pData->usPercent,         /* percent, format is %6.2$ which requires SHORT and not DCURRENCY value */
                          pData->lPresetPrice,              /* unit price, format is %9l$ which requires DCURRENCY value */
                          uchDecimal,                       /* decimal places count */
                          (DCURRENCY)pData->PLUTotal.lCounter, /* counter, format is %7.*l$ which requires number decimals and DCURRENCY value */
                          pData->aszSpeMnemo,               /* special mnemonics */
                          pData->PLUTotal.lAmount);         /* amount */
            }
        }
        break;                                          

    case CLASS_RPTPLU_PRTTTLCOUPON:
		{
			static const TCHAR FARCONST auchMldRptSupPLUFile3C0[] = _T("%1s%13s %-20s\n@%9l$\t%6ld   %12l$");

			/* convert PLU No. to double wide */
			RflSPrintf(aszRepoNumb, TCHARSIZEOF(aszRepoNumb), auchNumber, pData->auchPLUNumber); /* Saratoga */

			/* print NON ADJECTIVE COUPON PLU */
			usRow = MldSPrintf(aszString,          /* display buffer */
					  MLD_SUPER_MODE_CLM_LEN,      /* column length */
					  auchMldRptSupPLUFile3C0,             /* format */
					  aszPrtNull,                           /* null */
					  aszRepoNumb,                          /* PLU number */
					  pData->aszMnemo,                      /* PLU name */
					  pData->lPresetPrice,                  /* unit price, format is %9l$ which requires DCURRENCY value */
					  pData->PLUTotal.lCounter,             /* counter */
					  pData->PLUTotal.lAmount);             /* amount */
		}
        break;                                          

    case CLASS_RPTPLU_PRTTTLADJ1:  /* ONE ADJECTIVE PLU with PLU name/number */
        /* convert PLU No. to double wide */
        RflSPrintf(aszRepoNumb, TCHARSIZEOF(aszRepoNumb), auchNumber, pData->auchPLUNumber); /* Saratoga */
   
        /* check if PERCENT is illegal */
        if (pData->usPercent > PRT_MAX_PERCENT) {           /* In case that percent data is illegal,     */
                                                            /* pData->usPercent is set over 600.00 data */
            /* check if PLU is SCALABLE */
            if (pData->uchStatus == RPT_DECIMAL_0) {        /* in case of NON_SCALABLE */
				static const TCHAR FARCONST auchMldRptSupPLUFile4A10[] = _T("%1s%13s %-20s\n%4s\t*****%%\n@%9l$\t%6ld   %12l$");

				/* print ONE ADJECTIVE PLU with illegal percent data */
                usRow = MldSPrintf(aszString,          /* display buffer */
                          MLD_SUPER_MODE_CLM_LEN,      /* column length */
                          auchMldRptSupPLUFile4A10,        /* format */
                          aszPrtNull,                       /* null */
                          aszRepoNumb,                      /* PLU number */
                          pData->aszMnemo,                  /* PLU name */
                          pData->aszAdjMnemo,               /* adjective mnemonics */
                          pData->lPresetPrice,              /* unit price, format is %9l$ which requires DCURRENCY value */
                          pData->PLUTotal.lCounter,         /* counter */
                          pData->PLUTotal.lAmount);         /* amount */

            } else {                                        /* in case of SCALABLE */
				static const TCHAR FARCONST auchMldRptSupPLUFile4A11[] = _T("%1s%13s %-20s\n%4s\t*****%%\n@%9l$\t%7.*l$%s   %12l$");

				if (pData->uchStatus == RPT_DECIMAL_1) {
                    uchDecimal = 1;
                }

                /* print ONE ADJECTIVE PLU (SCALABLE) with illegal percent data */
                usRow = MldSPrintf(aszString,          /* display buffer */
                          MLD_SUPER_MODE_CLM_LEN,      /* column length */
                          auchMldRptSupPLUFile4A11,        /* format */
                          aszPrtNull,                       /* null */
                          aszRepoNumb,                      /* PLU number */
                          pData->aszMnemo,                  /* PLU name */
                          pData->aszAdjMnemo,               /* adjective mnemonics */
                          pData->lPresetPrice,              /* unit price, format is %9l$ which requires DCURRENCY value */
                          uchDecimal,                       /* decimal places count */
                          (DCURRENCY)pData->PLUTotal.lCounter,  /* counter, format is %7.*l$ which requires number decimals and DCURRENCY value */
                          pData->aszSpeMnemo,               /* special mnemonics */
                          pData->PLUTotal.lAmount);         /* amount */
            }
        } else {
            /* check if PLU is SCALABLE */
            if (pData->uchStatus == RPT_DECIMAL_0) {        /* in case of NON_SCALABLE */
				static const TCHAR FARCONST auchMldRptSupPLUFile4B10[] = _T("%1s%13s %-20s\n%4s\t%6.2$%%\n@%9l$\t%6ld   %12l$");

				/* print ONE ADJECTIVE PLU with legal percent data */
                usRow = MldSPrintf(aszString,          /* display buffer */
                          MLD_SUPER_MODE_CLM_LEN,      /* column length */
                          auchMldRptSupPLUFile4B10,        /* format */
                          aszPrtNull,                       /* null */
                          aszRepoNumb,                      /* PLU number */
                          pData->aszMnemo,                  /* PLU name */
                          pData->aszAdjMnemo,               /* adjective mnemonics */
                          ( SHORT)pData->usPercent,         /* percent, format is %6.2$ which requires SHORT and not DCURRENCY value */
                          pData->lPresetPrice,              /* unit price, format is %9l$ which requires DCURRENCY value */
                          pData->PLUTotal.lCounter,         /* counter */
                          pData->PLUTotal.lAmount);         /* amount */
            } else {                                        /* in case of SCALABLE */
				static const TCHAR FARCONST auchMldRptSupPLUFile4B11[] = _T("%1s%13s %-20s\n%4s\t%6.2$%%\n@%9l$\t%7.*l$%s   %12l$");

				if (pData->uchStatus == RPT_DECIMAL_1) {
                    uchDecimal = 1;
                }

                /* print ONE ADJECTIVE PLU (SCALABLE) with legal percent data */
                usRow = MldSPrintf(aszString,          /* display buffer */
                          MLD_SUPER_MODE_CLM_LEN,      /* column length */
                          auchMldRptSupPLUFile4B11,        /* format */
                          aszPrtNull,                       /* null */
                          aszRepoNumb,                      /* PLU number */
                          pData->aszMnemo,                  /* PLU name */
                          pData->aszAdjMnemo,               /* adjective mnemonics */
                          ( SHORT)pData->usPercent,         /* percent, format is %6.2$ which requires SHORT and not DCURRENCY value */
                          pData->lPresetPrice,              /* unit price, format is %9l$ which requires DCURRENCY value */
                          uchDecimal,                       /* decimal places count */
                          (DCURRENCY)pData->PLUTotal.lCounter,  /* counter, format is %7.*l$ which requires number decimals and DCURRENCY value */
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
				static const TCHAR FARCONST auchMldRptSupPLUFile4A20[] = _T("%4s\t*****%%\n@%9l$\t%6ld   %12l$");

				/* print ONE ADJECTIVE PLU with illegal percent data */
                usRow = MldSPrintf(aszString,          /* display buffer */
                          MLD_SUPER_MODE_CLM_LEN,      /* column length */
                          auchMldRptSupPLUFile4A20,        /* format */
                          pData->aszAdjMnemo,               /* adjective mnemonics */
                          pData->lPresetPrice,              /* unit price, format is %9l$ which requires DCURRENCY value */
                          pData->PLUTotal.lCounter,         /* counter */
                          pData->PLUTotal.lAmount);         /* amount */
            } else {                                        /* in case of SCALABLE */
				static const TCHAR FARCONST auchMldRptSupPLUFile4A21[] = _T("%4s\t*****%%\n@%9l$\t%7.*l$%s   %12l$");

				if (pData->uchStatus == RPT_DECIMAL_1) {
                    uchDecimal = 1;
                }

                /* print ONE ADJECTIVE PLU (SCALABLE) with illegal percent data */
                usRow = MldSPrintf(aszString,          /* display buffer */
                          MLD_SUPER_MODE_CLM_LEN,      /* column length */
                          auchMldRptSupPLUFile4A21,        /* format */
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
				static const TCHAR FARCONST auchMldRptSupPLUFile4B20[] = _T("%4s\t%6.2$%%\n@%9l$\t%6ld   %12l$");

				/* print ONE ADJECTIVE PLU with legal percent data */
                usRow = MldSPrintf(aszString,          /* display buffer */
                          MLD_SUPER_MODE_CLM_LEN,      /* column length */
                          auchMldRptSupPLUFile4B20,        /* format */
                          pData->aszAdjMnemo,               /* adjective mnemonics */
                          ( SHORT)pData->usPercent,         /* percent, format is %6.2$ which requires SHORT and not DCURRENCY value */
						  pData->lPresetPrice,              /* unit price, format is %9l$ which requires DCURRENCY value */
                          pData->PLUTotal.lCounter,         /* counter */
                          pData->PLUTotal.lAmount);         
            } else {                                        /* in case of SCALABLE */
				static const TCHAR FARCONST auchMldRptSupPLUFile4B21[] = _T("%4s\t%6.2$%%\n@%9l$\t%7.*l$%s   %12l$");

				if (pData->uchStatus == RPT_DECIMAL_1) {
                    uchDecimal = 1;
                }

                /* print ONE ADJECTIVE PLU (SCALABLE) with legal percent data */
                usRow = MldSPrintf(aszString,          /* display buffer */
                          MLD_SUPER_MODE_CLM_LEN,      /* column length */
                          auchMldRptSupPLUFile4B21,        /* format */
                          pData->aszAdjMnemo,               /* adjective mnemonics */
                          ( SHORT)pData->usPercent,         /* percent, format is %6.2$ which requires SHORT and not DCURRENCY value */
						  pData->lPresetPrice,              /* unit price, format is %9l$ which requires DCURRENCY value */
                          uchDecimal,                       /* decimal places count */
						  (DCURRENCY)pData->PLUTotal.lCounter,  /* counter, format is %7.*l$ which requires number decimals and DCURRENCY value */
                          pData->aszSpeMnemo,               /* special mnemonics */
                          pData->PLUTotal.lAmount);         /* amount */
            }
        }
        break;

    case CLASS_RPTPLU_PRTTTLMINUSADJ1:  /* ONE ADJECTIVE COUPON PLU with PLU name/number */
		{
			static const TCHAR FARCONST auchMldRptSupPLUFile4C10[] = _T("%1s%13s %-20s\n%4s\n@%9l$\t%6ld   %12l$");

			/* convert PLU No. to double wide */
			RflSPrintf(aszRepoNumb, TCHARSIZEOF(aszRepoNumb), auchNumber, pData->auchPLUNumber); /* Saratoga */
   
			/* print ONE ADJECTIVE COUPON PLU */
			usRow = MldSPrintf(aszString,          /* display buffer */
					  MLD_SUPER_MODE_CLM_LEN,      /* column length */
					  auchMldRptSupPLUFile4C10,                /* format */
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
		{
			static const TCHAR FARCONST auchMldRptSupPLUFile4C20[] = _T("%4s\n@%9l$\t%6ld   %12l$");

			/* print ONE ADJECTIVE COUPON PLU */
			usRow = MldSPrintf(aszString,          /* display buffer */
					  MLD_SUPER_MODE_CLM_LEN,      /* column length */
					  auchMldRptSupPLUFile4C20,                /* format */
					  pData->aszAdjMnemo,                       /* adjective mnemonics */
					  pData->lPresetPrice,                      /* unit price, format is %9l$ which requires DCURRENCY value */
					  pData->PLUTotal.lCounter,                 /* counter */
					  pData->PLUTotal.lAmount);                 /* amount */
		}
        break;

    case CLASS_RPTPLU_PRTTTLCNT:
        /* check if TOTAL is SCALABLE */
        if (pData->uchStatus == RPT_DECIMAL_0) {            /* in case of NON-SCALABLE */
			static const TCHAR FARCONST auchMldRptSupPLUFile50[] = _T("%-8s\t%6ld   %12l$");

            /* print SUBTOTAL/PLUS/MINUS/HASH/GRAND/CIGARETTE TOTAL */
            usRow = MldSPrintf(aszString,          /* display buffer */
                      MLD_SUPER_MODE_CLM_LEN,      /* column length */
                      auchMldRptSupPLUFile50,              /* format */
                      pData->aszMnemo,                      /* transaction mnemonic */
                      pData->PLUTotal.lCounter,             /* counter */
                      pData->PLUTotal.lAmount);             /* amount */
        } else {                                            /* in case of SCALABLE */
			static const TCHAR FARCONST auchMldRptSupPLUFile51[] = _T("%-8s\t%7.*l$%s   %12l$");

			if (pData->uchStatus == RPT_DECIMAL_1) {
                uchDecimal = 1;
            }
   
            /* print SUBTOTAL/PLUS/MINUS/HASH/GRAND/CIGARETTE TOTAL */
            usRow = MldSPrintf(aszString,          /* display buffer */
                      MLD_SUPER_MODE_CLM_LEN,      /* column length */
                      auchMldRptSupPLUFile51,              /* format */
                      pData->aszMnemo,                      /* transaction mnemonic */
                      uchDecimal,                           /* decimal places count */
                      (DCURRENCY)pData->PLUTotal.lCounter,  /* counter, format is %7.*l$ which requires number decimals and DCURRENCY value */
                      pData->aszSpeMnemo,                   /* special mnemonics */
                      pData->PLUTotal.lAmount);             /* amount */
        }
        break;
   
    case CLASS_RPTPLU_PRTCODE:
		{
			static const TCHAR FARCONST auchMldRptSupPLUFile6[] = _T("%21u");

			/* print REPORT CODE in PLU report */
			usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupPLUFile6, pData->usReportCode);
		}
        break;
   
    default:
        PifLog(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_CODE_04);
        PifLog(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_ERROR);
        break;
    }
       
    if (!usRow) {
        return (MLD_SUCCESS);               /* if format is not created */
    }

    /* check format data is displayed in the scroll page */
    if (usStatus == RPT_PLUIND_READ) {
        MldScroll1Buff.uchCurItem = 0;  /* not pause at indivisual read */
    } else if ((usStatus == RPT_TYPE_RPTCODEIND) && (pData->uchMinorClass == CLASS_RPTPLU_PRTCODE)) {
        MldScroll1Buff.uchCurItem = 0;  /* not pause at indivisual read */
    } else if ((usStatus == RPT_TYPE_DEPTIND) && (pData->uchMinorClass == CLASS_RPTPLU_PRTDEPT)) {
        MldScroll1Buff.uchCurItem = 0;  /* not pause at indivisual read */
    } else {
        if (usRow + (USHORT)MldScroll1Buff.uchCurItem > MLD_SUPER_MODE_ROW_LEN) {
            MldScroll1Buff.uchCurItem = 0;  /* new page */
            /* create dialog */
            if (MldContinueCheck() == MLD_ABORTED) {
                return (MLD_ABORTED);
            }
        }

        MldScroll1Buff.uchCurItem += (UCHAR)usRow;
    }

    pszString = &aszString[0];

    while (usRow--) {
        /* display format data */
        MldString(MLD_SCROLL_1, MLD_SCROLL_APPEND, pszString, MLD_SUPER_MODE_CLM_LEN);
        pszString += (MLD_SUPER_MODE_CLM_LEN+1);
    }

    return (MLD_SUCCESS);
}                          

/***** End of Source *****/