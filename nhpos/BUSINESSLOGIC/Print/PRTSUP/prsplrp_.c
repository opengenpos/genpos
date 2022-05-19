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
* Title       : PLU File Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSPLRP_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               PrtSupPLUFile()     : This Function Forms PLU File Print Format.       
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jul-07-92: 00.00.01 : K.You     : initial                                   
* Jul-07-93: 01.00.12 : J.IKEDA   : Maint. Sales Percent Format.(Decimal Point)                                   
* Jul-23-93: 01.00.12 : J.IKEDA   : Maint. Counter Format. (%4ld->%6ld)
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
** Synopsis:    VOID  PrtSupPLUFile( RPTPLU *pData )  
*               
*     Input:    *pData  : Pointer to Structure for RPTPLU 
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This Function Forms PLU File Print Format.
*===========================================================================
*/

VOID  PrtSupPLUFile( RPTPLU *pData )                                
{
    static const TCHAR FARCONST auchPrtSupPLUFile1[] = _T("%4u\t%s");     /* Saratoga */
    static const TCHAR FARCONST auchPrtSupPLUFile2A0[] = _T("%-s\t%s\n*****%%\t%6ld%s\n\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile2A1[] = _T("%-s\t%s\n*****%%\t%.*l$%s\n\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile2B0[] = _T("%-s\t%s\n%6.2$%%\t%6ld%s\n\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile2B1[] = _T("%-s\t%s\n%6.2$%%\t%.*l$%s\n\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile2C0[] = _T("%-s\t%s\n\t%6ld%s\n\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile3A0[] = _T("%-s\t%s\n*****%%\t%6ld%s\n@%8l$\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile3A1[] = _T("%-s\t%s\n*****%%\t%.*l$%s\n@%8l$\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile3B0[] = _T("%-s\t%s\n%6.2$%%\t%6ld%s\n@%8l$\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile3B1[] = _T("%-s\t%s\n%6.2$%%\t%.*l$%s\n@%8l$\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile3C0[] = _T("%-s\t%s\n\t%6ld%s\n@%8l$\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile4A10[] = _T("%-s\t%s\n%-s\n*****%%\t%6ld%s\n@%8l$\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile4A11[] = _T("%-s\t%s\n%-s\n*****%%\t%.*l$%s\n@%8l$\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile4A20[] = _T("%-s\n*****%%\t%6ld%s\n@%8l$\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile4A21[] = _T("%-s\n*****%%\t%.*l$%s\n@%8l$\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile4B10[] = _T("%-s\t%s\n%-s\n%6.2$%%\t%6ld%s\n@%8l$\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile4B11[] = _T("%-s\t%s\n%-s\n%6.2$%%\t%.*l$%s\n@%8l$\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile4B20[] = _T("%-s\n%6.2$%%\t%6ld%s\n@%8l$\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile4B21[] = _T("%-s\n%6.2$%%\t%.*l$%s\n@%8l$\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile4C10[] = _T("%-s\t%s\n%-s\n\t%6ld%s\n@%8l$\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile4C20[] = _T("%-s\n\t%6ld%s\n@%8l$\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile50[] = _T("%-s\t%6ld%s\n\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile51[] = _T("%-s\t%.*l$%s\n\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile6[] = _T("%-10s%1u");
    static const TCHAR FARCONST auchNumber[] = _T("%s");                    /* Saratoga */

    UCHAR   uchDecimal = 2;
    TCHAR   aszPrtNull[1] = {'\0'};
    /* UCHAR   aszDoubRepoNumb[4 * 2 + 1]; */
    TCHAR   aszRepoNumb[PLU_MAX_DIGIT + 1];
    USHORT  usPrtType;

	memset(aszRepoNumb, 0x00, sizeof(aszRepoNumb));
    /* Initialize aszDoubRepoNumb */
    /* memset(aszDoubRepoNumb, '\0', sizeof(aszDoubRepoNumb)); */

    /* Check Print Control */
    usPrtType = PrtSupChkType(pData->usPrintControl);  

    /* Check Minor Class */
    switch(pData->uchMinorClass) {
    case CLASS_RPTPLU_PRTDEPT:
        PmgPrintf(usPrtType,                    /* Printer Type */
                  auchPrtSupPLUFile1,           /* Format */
                  /* aszPrtNull,                   / Null */
                  pData->usDEPTNumber,          /* Department Number,   Saratoga */
                  pData->aszMnemo);             /* Department Name */
        break;

    case CLASS_RPTPLU_PRTTTLOPEN:        
        /* Convert PLU No. to Double Wide,  Saratoga */
        RflConvertPLU(aszRepoNumb, pData->auchPLUNumber);

        /* check if PERCENT is over 600.00% */
        if (pData->usPercent > PRT_MAX_PERCENT) {
            if (pData->uchStatus == RPT_DECIMAL_0) {
                PmgPrintf(usPrtType,                        /* Printer Type */
                          auchPrtSupPLUFile2A0,             /* Format */
                          aszRepoNumb,                  /* PLU Number */
                          pData->aszMnemo,                  /* PLU Name */
                          pData->PLUTotal.lCounter,         /* Counter */
                          pData->aszSpeMnemo,               /* Special Mnemonics */
                          pData->PLUTotal.lAmount);         /* Amount */
            } else {
                if (pData->uchStatus == RPT_DECIMAL_1) {
                    uchDecimal = 1;
                }
                PmgPrintf(usPrtType,                        /* Printer Type */
                          auchPrtSupPLUFile2A1,             /* Format */
                          aszRepoNumb,                  /* PLU Number */
                          pData->aszMnemo,                  /* PLU Name */
                          uchDecimal,                       /* Decimal Value */
                          pData->PLUTotal.lCounter,         /* Counter */
                          pData->aszSpeMnemo,               /* Special Mnemonics */
                          pData->PLUTotal.lAmount);         /* Amount */
            }
        } else {
            if (pData->uchStatus == RPT_DECIMAL_0) {
                PmgPrintf(usPrtType,                        /* Printer Type */
                          auchPrtSupPLUFile2B0,             /* Format */
                          aszRepoNumb,                  /* PLU Number */
                          pData->aszMnemo,                  /* PLU Name */
                          (DCURRENCY)pData->usPercent,                 /* Percent */
                          pData->PLUTotal.lCounter,         /* Counter */
                          pData->aszSpeMnemo,               /* Special Mnemonics */
                          pData->PLUTotal.lAmount);         /* Amount */
            } else {
                if (pData->uchStatus == RPT_DECIMAL_1) {
                    uchDecimal = 1;
                }
                PmgPrintf(usPrtType,                        /* Printer Type */
                          auchPrtSupPLUFile2B1,             /* Format */
                          aszRepoNumb,                  /* PLU Number */
                          pData->aszMnemo,                  /* PLU Name */
                          (DCURRENCY)pData->usPercent,                 /* Percent */
                          uchDecimal,                       /* Decimal Value */
                          pData->PLUTotal.lCounter,         /* Counter */
                          pData->aszSpeMnemo,               /* Special Mnemonics */
                          pData->PLUTotal.lAmount);         /* Amount */
            }
        }
        break;                                          

    case CLASS_RPTPLU_PRTTTLCREDIT:        
        /* Convert PLU No. to Double Wide,  Saratoga */
        RflConvertPLU(aszRepoNumb, pData->auchPLUNumber);

		PmgPrintf(usPrtType,                            /* Printer Type */
                  auchPrtSupPLUFile2C0,                 /* Format */
                  aszRepoNumb,                      /* PLU Number */
                  pData->aszMnemo,                      /* PLU Name */
                  pData->PLUTotal.lCounter,             /* Counter */
                  pData->aszSpeMnemo,                   /* Special Mnemonics */
                  pData->PLUTotal.lAmount);             /* Amount */
        break;                                          

    case CLASS_RPTPLU_PRTTTLPRSET:
        /* Convert PLU No. to Double Wide,  Saratoga */
        RflConvertPLU(aszRepoNumb, pData->auchPLUNumber);

        /* check if PERCENT is over 600.00% */
        if (pData->usPercent > PRT_MAX_PERCENT) {
            if (pData->uchStatus == RPT_DECIMAL_0) {
                PmgPrintf(usPrtType,                        /* Printer Type */
                          auchPrtSupPLUFile3A0,             /* Format */
                          aszRepoNumb,                  /* PLU Number */
                          pData->aszMnemo,                  /* PLU Name */
                          pData->PLUTotal.lCounter,         /* Counter */
                          pData->aszSpeMnemo,               /* Special Mnemonics */
                          pData->lPresetPrice,              /* Unit Price */
                          pData->PLUTotal.lAmount);         /* Amount */
            } else {
                if (pData->uchStatus == RPT_DECIMAL_1) {
                    uchDecimal = 1;
                }
                PmgPrintf(usPrtType,                        /* Printer Type */
                          auchPrtSupPLUFile3A1,             /* Format */
                          aszRepoNumb,                  /* PLU Number */
                          pData->aszMnemo,                  /* PLU Name */
                          uchDecimal,                       /* Decimal Value */
                          pData->PLUTotal.lCounter,         /* Counter */
                          pData->aszSpeMnemo,               /* Special Mnemonics */
                          pData->lPresetPrice,              /* Unit Price */
                          pData->PLUTotal.lAmount);         /* Amount */
            }
        } else {
            if (pData->uchStatus == RPT_DECIMAL_0) {
                PmgPrintf(usPrtType,                        /* Printer Type */
                          auchPrtSupPLUFile3B0,             /* Format */
                          aszRepoNumb,                  /* PLU Number */
                          pData->aszMnemo,                  /* PLU Name */
                          (DCURRENCY)pData->usPercent,                 /* Percent */
                          pData->PLUTotal.lCounter,         /* Counter */
                          pData->aszSpeMnemo,               /* Special Mnemonics */
                          pData->lPresetPrice,              /* Unit Price */
                          pData->PLUTotal.lAmount);         /* Amount */
            } else {
                if (pData->uchStatus == RPT_DECIMAL_1) {
                    uchDecimal = 1;
                }
                PmgPrintf(usPrtType,                        /* Printer Type */
                          auchPrtSupPLUFile3B1,             /* Format */
                          aszRepoNumb,                  /* PLU Number */
                          pData->aszMnemo,                  /* PLU Name */
                          (DCURRENCY)pData->usPercent,                 /* Percent */
                          uchDecimal,                       /* Decimal Value */
                          pData->PLUTotal.lCounter,         /* Counter */
                          pData->aszSpeMnemo,               /* Special Mnemonics */
                          pData->lPresetPrice,              /* Unit Price */
                          pData->PLUTotal.lAmount);         /* Amount */
            }
        }
        break;                                          

    case CLASS_RPTPLU_PRTTTLCOUPON:
        /* Convert PLU No. to Double Wide,  Saratoga */
        RflConvertPLU(aszRepoNumb, pData->auchPLUNumber);

        PmgPrintf(usPrtType,                            /* Printer Type */
                  auchPrtSupPLUFile3C0,                 /* Format */
                  aszRepoNumb,                      /* PLU Number */
                  pData->aszMnemo,                      /* PLU Name */
                  pData->PLUTotal.lCounter,             /* Counter */
                  pData->aszSpeMnemo,                   /* Special Mnemonics */
                  pData->lPresetPrice,                  /* Unit Price */
                  pData->PLUTotal.lAmount);             /* Amount */
        break;                                          

    case CLASS_RPTPLU_PRTTTLADJ1:
        /* Convert PLU No. to Double Wide,  Saratoga */
        RflConvertPLU(aszRepoNumb, pData->auchPLUNumber);

        /* check if PERCENT is over 600.00% */
        if (pData->usPercent > PRT_MAX_PERCENT) {
            if (pData->uchStatus == RPT_DECIMAL_0) {
                PmgPrintf(usPrtType,                        /* Printer Type */
                          auchPrtSupPLUFile4A10,            /* Format */
                          aszRepoNumb,                  /* PLU Number */
                          pData->aszMnemo,                  /* PLU Name */
                          pData->aszAdjMnemo,               /* Adjective Mnemonics */
                          pData->PLUTotal.lCounter,         /* Counter */
                          pData->aszSpeMnemo,               /* Special Mnemonics */
                          pData->lPresetPrice,              /* Unit Price */
                          pData->PLUTotal.lAmount);         /* Amount */
            } else {
                if (pData->uchStatus == RPT_DECIMAL_1) {
                    uchDecimal = 1;
                }
                PmgPrintf(usPrtType,                        /* Printer Type */
                          auchPrtSupPLUFile4A11,            /* Format */
                          aszRepoNumb,                  /* PLU Number */
                          pData->aszMnemo,                  /* PLU Name */
                          pData->aszAdjMnemo,               /* Adjective Mnemonics */
                          uchDecimal,                       /* Decimal Value */
                          pData->PLUTotal.lCounter,         /* Counter */
                          pData->aszSpeMnemo,               /* Special Mnemonics */
                          pData->lPresetPrice,              /* Unit Price */
                          pData->PLUTotal.lAmount);         /* Amount */
            }
        } else {
            if (pData->uchStatus == RPT_DECIMAL_0) {
                PmgPrintf(usPrtType,                        /* Printer Type */
                          auchPrtSupPLUFile4B10,            /* Format */
                          aszRepoNumb,                  /* PLU Number */
                          pData->aszMnemo,                  /* PLU Name */
                          pData->aszAdjMnemo,               /* Adjective Mnemonics */
                          (DCURRENCY)pData->usPercent,                 /* Percent */
                          pData->PLUTotal.lCounter,         /* Counter */
                          pData->aszSpeMnemo,               /* Special Mnemonics */
                          pData->lPresetPrice,              /* Unit Price */
                          pData->PLUTotal.lAmount);         /* Amount */
            } else {
                if (pData->uchStatus == RPT_DECIMAL_1) {
                    uchDecimal = 1;
                }
                PmgPrintf(usPrtType,                        /* Printer Type */
                          auchPrtSupPLUFile4B11,            /* Format */
                          aszRepoNumb,                  /* PLU Number */
                          pData->aszMnemo,                  /* PLU Name */
                          pData->aszAdjMnemo,               /* Adjective Mnemonics */
                          (DCURRENCY)pData->usPercent,                 /* Percent */
                          uchDecimal,                       /* Decimal Value */
                          pData->PLUTotal.lCounter,         /* Counter */
                          pData->aszSpeMnemo,               /* Special Mnemonics */
                          pData->lPresetPrice,              /* Unit Price */
                          pData->PLUTotal.lAmount);         /* Amount */
            }
        }
        break;

    case CLASS_RPTPLU_PRTTTLADJ2:
        /* check if PERCENT is over 600.00% */
        if (pData->usPercent > PRT_MAX_PERCENT) {
            if (pData->uchStatus == RPT_DECIMAL_0) {
                PmgPrintf(usPrtType,                        /* Printer Type */
                          auchPrtSupPLUFile4A20,            /* Format */
                          pData->aszAdjMnemo,               /* Adjective Mnemonics */
                          pData->PLUTotal.lCounter,         /* Counter */
                          pData->aszSpeMnemo,               /* Special Mnemonics */
                          pData->lPresetPrice,              /* Unit Price */
                          pData->PLUTotal.lAmount);         /* Amount */
            } else {
                if (pData->uchStatus == RPT_DECIMAL_1) {
                    uchDecimal = 1;
                }
                PmgPrintf(usPrtType,                        /* Printer Type */
                          auchPrtSupPLUFile4A21,            /* Format */
                          pData->aszAdjMnemo,               /* Adjective Mnemonics */
                          uchDecimal,                       /* Decimal Value */
                          pData->PLUTotal.lCounter,         /* Counter */
                          pData->aszSpeMnemo,               /* Special Mnemonics */
                          pData->lPresetPrice,              /* Unit Price */
                          pData->PLUTotal.lAmount);         /* Amount */
            }
        } else {
            if (pData->uchStatus == RPT_DECIMAL_0) {
                PmgPrintf(usPrtType,                        /* Printer Type */
                          auchPrtSupPLUFile4B20,            /* Format */
                          pData->aszAdjMnemo,               /* Adjective Mnemonics */
                          (DCURRENCY)pData->usPercent,                 /* Percent */
                          pData->PLUTotal.lCounter,         /* Counter */
                          pData->aszSpeMnemo,               /* Special Mnemonics */
                          pData->lPresetPrice,              /* Unit Price */
                          pData->PLUTotal.lAmount);         /* Amount */
            } else {
                if (pData->uchStatus == RPT_DECIMAL_1) {
                    uchDecimal = 1;
                }
                PmgPrintf(usPrtType,                        /* Printer Type */
                          auchPrtSupPLUFile4B21,            /* Format */
                          pData->aszAdjMnemo,               /* Adjective Mnemonics */
                          (DCURRENCY)pData->usPercent,                 /* Percent */
                          uchDecimal,                       /* Decimal Value */
                          pData->PLUTotal.lCounter,         /* Counter */
                          pData->aszSpeMnemo,               /* Special Mnemonics */
                          pData->lPresetPrice,              /* Unit Price */
                          pData->PLUTotal.lAmount);         /* Amount */
            }
        }
        break;

    case CLASS_RPTPLU_PRTTTLMINUSADJ1:
        /* Convert PLU No. to Double Wide,  Saratoga */
        RflConvertPLU(aszRepoNumb, pData->auchPLUNumber);

        PmgPrintf(usPrtType,                            /* Printer Type */
                  auchPrtSupPLUFile4C10,                /* Format */
                  aszRepoNumb,                      /* PLU Number */
                  pData->aszMnemo,                      /* PLU Name */
                  pData->aszAdjMnemo,                   /* Adjective Mnemonics */
                  pData->PLUTotal.lCounter,             /* Counter */
                  pData->aszSpeMnemo,                   /* Special Mnemonics */
                  pData->lPresetPrice,                  /* Unit Price */
                  pData->PLUTotal.lAmount);             /* Amount */
        break;

    case CLASS_RPTPLU_PRTTTLMINUSADJ2:
        PmgPrintf(usPrtType,                            /* Printer Type */
                  auchPrtSupPLUFile4C20,                /* Format */
                  pData->aszAdjMnemo,                   /* Adjective Mnemonics */
                  pData->PLUTotal.lCounter,             /* Counter */
                  pData->aszSpeMnemo,                   /* Special Mnemonics */
                  pData->lPresetPrice,                  /* Unit Price */
                  pData->PLUTotal.lAmount);             /* Amount */
        break;

    case CLASS_RPTPLU_PRTTTLCNT:
        if (pData->uchStatus == RPT_DECIMAL_0) {
            PmgPrintf(usPrtType,                            /* Printer Type */
                      auchPrtSupPLUFile50,                  /* Format */
                      pData->aszMnemo,                      /* Transaction Name */
                      pData->PLUTotal.lCounter,             /* Counter */
                      pData->aszSpeMnemo,                   /* Special Mnemonics */
                      pData->PLUTotal.lAmount);             /* Amount */
        } else {
            if (pData->uchStatus == RPT_DECIMAL_1) {
                uchDecimal = 1;
            }
            PmgPrintf(usPrtType,                            /* Printer Type */
                      auchPrtSupPLUFile51,                  /* Format */
                      pData->aszMnemo,                      /* Transaction Name */
                      uchDecimal,                           /* Decimal Value */
                      pData->PLUTotal.lCounter,             /* Counter */
                      pData->aszSpeMnemo,                   /* Special Mnemonics */
                      pData->PLUTotal.lAmount);             /* Amount */
        }
        break;

    case CLASS_RPTPLU_PRTCODE:
        PmgPrintf(usPrtType,                            /* Printer Type */
                  auchPrtSupPLUFile6,                   /* Format */
                  aszPrtNull,
                  pData->usReportCode);                 /* Report Code, Saratoga */
        break;

    default:
/*      PifLog(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_ERROR); */
        break;
    }
}

/* --- End of Source File --- */