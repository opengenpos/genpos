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
* Title       : PLU File Format for 21 Column ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSPLRPS.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               PrtSupPLUFile_s()     : This Function Forms PLU File Print Format.       
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Sep-12-92: 00.00.01 : K.You     : initial                                   
* Nov-26-92: 01.00.00 : K.You     : Chg Function Name                                   
* Jul-07-93: 01.00.12 : J.IKEDA   : Maint. Sales Percent Format.(Decinmal Point)
* Jul-23-93: 01.00.12 : J.IKEDA   : Maint. Counter Format.(%4ld->%6ld)
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
** Synopsis:    VOID  PrtSupPLUFile_s( RPTPLU *pData )  
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

VOID  PrtSupPLUFile_s( RPTPLU *pData )  
{
    static const TCHAR FARCONST auchPrtSupPLUFile1[] = _T("%5s%su %s");         /* Saratoga */
    static const TCHAR FARCONST auchPrtSupPLUFile2A0[] = _T("%-s %s\n\t%6ld%s\n*****%%\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile2A1[] = _T("%-s %s\n\t%.*l$%s\n*****%%\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile2B0[] = _T("%-s %s\n\t%6ld%s\n%6.2$%%\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile2B1[] = _T("%-s %s\n\t%.*l$%s\n%6.2$%%\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile2C0[] = _T("%-s %s\n\t%6ld%s\n\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile3A0[] = _T("%-s %s\n\t@%8l$\n\t%6ld%s\n*****%%\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile3A1[] = _T("%-s %s\n\t@%8l$\n\t%.*l$%s\n*****%%\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile3B0[] = _T("%-s %s\n\t@%8l$\n\t%6ld%s\n%6.2$%%\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile3B1[] = _T("%-s %s\n\t@%8l$\n\t%.*l$%s\n%6.2$%%\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile3C0[] = _T("%-s %s\n\t@%8l$\n\t%6ld%s\n\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile4A10[] = _T("%-s %s\n%-s\t@%8l$\n\t%6ld%s\n*****%%\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile4A11[] = _T("%-s %s\n%-s\t@%8l$\n\t%.*l$%s\n*****%%\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile4A20[] = _T("%-s\t@%8l$\n\t%6ld%s\n*****%%\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile4A21[] = _T("%-s\t@%8l$\n\t%.*l$%s\n*****%%\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile4B10[] = _T("%-s %s\n%-s\t@%8l$\n\t%6ld%s\n%6.2$%%\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile4B11[] = _T("%-s %s\n%-s\t@%8l$\n\t%.*l$%s\n%6.2$%%\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile4B20[] = _T("%-s\t@%8l$\n\t%6ld%s\n%6.2$%%\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile4B21[] = _T("%-s\t@%8l$\n\t%.*l$%s\n%6.2$%%\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile4C10[] = _T("%-s %s\n%-s\t@%8l$\n\t%6ld%s\n\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile4C20[] = _T("%-s\t@%8l$\n\t%6ld%s\n\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile50[] = _T("%-s\t%6ld%s\n\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile51[] = _T("%-s\t%.*l$%s\n\t%l$");
    static const TCHAR FARCONST auchPrtSupPLUFile6[] = _T("%-10s%1u");
    static const TCHAR FARCONST auchNumber[] = _T("%4d");

    UCHAR   uchDecimal = 2;
    TCHAR   aszPrtNull[1] = {'\0'};
	TCHAR	aszDoubRepoNumb[8 * 2 + 1];
	TCHAR	aszRepoNumb[8 + 1];
    USHORT  usPrtType;

    /* Initialize aszDoubRepoNumb */
    memset(aszDoubRepoNumb, '\0', sizeof(aszDoubRepoNumb));

    /* Check Print Control */
    usPrtType = PrtSupChkType(pData->usPrintControl);  

    /* Check Minor Class */
    switch(pData->uchMinorClass) {
    case CLASS_RPTPLU_PRTDEPT:
        PmgPrintf(usPrtType,                    /* Printer Type */
                  auchPrtSupPLUFile1,           /* Format */
                  aszPrtNull,                   /* Null */
                  pData->usDEPTNumber,          /* Department Number,   Saratoga */
                  pData->aszMnemo);             /* Department Name */
        break;

    case CLASS_RPTPLU_PRTTTLOPEN:        
        /* Convert PLU No. to Double Wide,  Saratoga */
        RflSPrintf(aszRepoNumb, PLU_MAX_DIGIT+1, auchNumber, pData->auchPLUNumber);
        PrtDouble(aszDoubRepoNumb, TCHARSIZEOF(aszDoubRepoNumb), aszRepoNumb);

        /* check if PERCENT is over 600.00% */
        if (pData->usPercent > PRT_MAX_PERCENT) {
            if (pData->uchStatus == RPT_DECIMAL_0) {
                PmgPrintf(usPrtType,                        /* Printer Type */
                          auchPrtSupPLUFile2A0,             /* Format */
                          aszDoubRepoNumb,                  /* PLU Number */
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
                          aszDoubRepoNumb,                  /* PLU Number */
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
                          aszDoubRepoNumb,                  /* PLU Number */
                          pData->aszMnemo,                  /* PLU Name */
                          pData->PLUTotal.lCounter,         /* Counter */
                          pData->aszSpeMnemo,               /* Special Mnemonics */
                          (DCURRENCY)pData->usPercent,                 /* Percent */
                          pData->PLUTotal.lAmount);         /* Amount */
            } else {
                if (pData->uchStatus == RPT_DECIMAL_1) {
                    uchDecimal = 1;
                }
                PmgPrintf(usPrtType,                        /* Printer Type */
                          auchPrtSupPLUFile2B1,             /* Format */
                          aszDoubRepoNumb,                  /* PLU Number */
                          pData->aszMnemo,                  /* PLU Name */
                          uchDecimal,                       /* Decimal Value */
                          pData->PLUTotal.lCounter,         /* Counter */
                          pData->aszSpeMnemo,               /* Special Mnemonics */
                          (DCURRENCY)pData->usPercent,                 /* Percent */
                          pData->PLUTotal.lAmount);         /* Amount */
            }
        }
        break;                                          

    case CLASS_RPTPLU_PRTTTLCREDIT:
        /* Convert PLU No. to Double Wide,  Saratoga */
        RflSPrintf(aszRepoNumb, PLU_MAX_DIGIT+1, auchNumber, pData->auchPLUNumber);
        PrtDouble(aszDoubRepoNumb, (4 * 2 + 1), aszRepoNumb);
        PmgPrintf(usPrtType,                            /* Printer Type */
                  auchPrtSupPLUFile2C0,                 /* Format */
                  aszDoubRepoNumb,                      /* PLU Number */
                  pData->aszMnemo,                      /* PLU Name */
                  pData->PLUTotal.lCounter,             /* Counter */
                  pData->aszSpeMnemo,                   /* Special Mnemonics */
                  pData->PLUTotal.lAmount);             /* Amount */
        break;                                          

    case CLASS_RPTPLU_PRTTTLPRSET:
        /* Convert PLU No. to Double Wide,  Saratoga */
        RflSPrintf(aszRepoNumb, PLU_MAX_DIGIT+1, auchNumber, pData->auchPLUNumber);
        PrtDouble(aszDoubRepoNumb, (4 * 2 + 1), aszRepoNumb);

        /* check if PERCENT is over 600.00% */
        if (pData->usPercent > PRT_MAX_PERCENT) {
            if (pData->uchStatus == RPT_DECIMAL_0) {
                PmgPrintf(usPrtType,                        /* Printer Type */
                          auchPrtSupPLUFile3A0,             /* Format */
                          aszDoubRepoNumb,                  /* PLU Number */
                          pData->aszMnemo,                  /* PLU Name */
                          pData->lPresetPrice,              /* Unit Price */
                          pData->PLUTotal.lCounter,         /* Counter */
                          pData->aszSpeMnemo,               /* Special Mnemonics */
                          pData->PLUTotal.lAmount);         /* Amount */
            } else {
                if (pData->uchStatus == RPT_DECIMAL_1) {
                    uchDecimal = 1;
                }
                PmgPrintf(usPrtType,                        /* Printer Type */
                          auchPrtSupPLUFile3A1,             /* Format */
                          aszDoubRepoNumb,                  /* PLU Number */
                          pData->aszMnemo,                  /* PLU Name */
                          pData->lPresetPrice,              /* Unit Price */
                          uchDecimal,                       /* Decimal Value */
                          pData->PLUTotal.lCounter,         /* Counter */
                          pData->aszSpeMnemo,               /* Special Mnemonics */
                          pData->PLUTotal.lAmount);         /* Amount */
            }
        } else {
            if (pData->uchStatus == RPT_DECIMAL_0) {
                PmgPrintf(usPrtType,                        /* Printer Type */
                          auchPrtSupPLUFile3B0,             /* Format */
                          aszDoubRepoNumb,                  /* PLU Number */
                          pData->aszMnemo,                  /* PLU Name */
                          pData->lPresetPrice,              /* Unit Price */
                          pData->PLUTotal.lCounter,         /* Counter */
                          pData->aszSpeMnemo,               /* Special Mnemonics */
                          (DCURRENCY)pData->usPercent,                 /* Percent */
                          pData->PLUTotal.lAmount);         /* Amount */
            } else {
                if (pData->uchStatus == RPT_DECIMAL_1) {
                    uchDecimal = 1;
                }
                PmgPrintf(usPrtType,                        /* Printer Type */
                          auchPrtSupPLUFile3B1,             /* Format */
                          aszDoubRepoNumb,                  /* PLU Number */
                          pData->aszMnemo,                  /* PLU Name */
                          pData->lPresetPrice,              /* Unit Price */
                          uchDecimal,                       /* Decimal Value */
                          pData->PLUTotal.lCounter,         /* Counter */
                          pData->aszSpeMnemo,               /* Special Mnemonics */
                          (DCURRENCY)pData->usPercent,                 /* Percent */
                          pData->PLUTotal.lAmount);         /* Amount */
            }
        }
        break;                                          

    case CLASS_RPTPLU_PRTTTLCOUPON:
        /* Convert PLU No. to Double Wide,  Saratoga */
        RflSPrintf(aszRepoNumb, PLU_MAX_DIGIT+1, auchNumber, pData->auchPLUNumber);
        PrtDouble(aszDoubRepoNumb, (4 * 2 + 1), aszRepoNumb);

        PmgPrintf(usPrtType,                            /* Printer Type */
                  auchPrtSupPLUFile3C0,                 /* Format */
                  aszDoubRepoNumb,                      /* PLU Number */
                  pData->aszMnemo,                      /* PLU Name */
                  pData->lPresetPrice,                  /* Unit Price */
                  pData->PLUTotal.lCounter,             /* Counter */
                  pData->aszSpeMnemo,                   /* Special Mnemonics */
                  pData->PLUTotal.lAmount);             /* Amount */
        break;                                          

    case CLASS_RPTPLU_PRTTTLADJ1:
        /* Convert PLU No. to Double Wide,  Saratoga */
        RflSPrintf(aszRepoNumb, PLU_MAX_DIGIT+1, auchNumber, pData->auchPLUNumber);
        PrtDouble(aszDoubRepoNumb, (4 * 2 + 1), aszRepoNumb);

        /* check if PERCENT is over 600.00% */
        if (pData->usPercent > PRT_MAX_PERCENT) {
            if (pData->uchStatus == RPT_DECIMAL_0) {
                PmgPrintf(usPrtType,                        /* Printer Type */
                          auchPrtSupPLUFile4A10,            /* Format */
                          aszDoubRepoNumb,                  /* PLU Number */
                          pData->aszMnemo,                  /* PLU Name */
                          pData->aszAdjMnemo,               /* Adjective Mnemonics */
                          pData->lPresetPrice,              /* Unit Price */
                          pData->PLUTotal.lCounter,         /* Counter */
                          pData->aszSpeMnemo,               /* Special Mnemonics */
                          pData->PLUTotal.lAmount);         /* Amount */
            } else {
                if (pData->uchStatus == RPT_DECIMAL_1) {
                    uchDecimal = 1;
                }
                PmgPrintf(usPrtType,                        /* Printer Type */
                          auchPrtSupPLUFile4A11,            /* Format */
                          aszDoubRepoNumb,                  /* PLU Number */
                          pData->aszMnemo,                  /* PLU Name */
                          pData->aszAdjMnemo,               /* Adjective Mnemonics */
                          pData->lPresetPrice,              /* Unit Price */
                          uchDecimal,                       /* Decimal Value */
                          pData->PLUTotal.lCounter,         /* Counter */
                          pData->aszSpeMnemo,               /* Special Mnemonics */
                          pData->PLUTotal.lAmount);         /* Amount */
            }
        } else {
            if (pData->uchStatus == RPT_DECIMAL_0) {
                PmgPrintf(usPrtType,                        /* Printer Type */
                          auchPrtSupPLUFile4B10,            /* Format */
                          aszDoubRepoNumb,                  /* PLU Number */
                          pData->aszMnemo,                  /* PLU Name */
                          pData->aszAdjMnemo,               /* Adjective Mnemonics */
                          pData->lPresetPrice,              /* Unit Price */
                          pData->PLUTotal.lCounter,         /* Counter */
                          pData->aszSpeMnemo,               /* Special Mnemonics */
                          (DCURRENCY)pData->usPercent,                 /* Percent */
                          pData->PLUTotal.lAmount);         /* Amount */
            } else {
                if (pData->uchStatus == RPT_DECIMAL_1) {
                    uchDecimal = 1;
                }
                PmgPrintf(usPrtType,                        /* Printer Type */
                          auchPrtSupPLUFile4B11,            /* Format */
                          aszDoubRepoNumb,                  /* PLU Number */
                          pData->aszMnemo,                  /* PLU Name */
                          pData->aszAdjMnemo,               /* Adjective Mnemonics */
                          pData->lPresetPrice,              /* Unit Price */
                          uchDecimal,                       /* Decimal Value */
                          pData->PLUTotal.lCounter,         /* Counter */
                          pData->aszSpeMnemo,               /* Special Mnemonics */
                          (DCURRENCY)pData->usPercent,                 /* Percent */
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
                          pData->lPresetPrice,              /* Unit Price */
                          pData->PLUTotal.lCounter,         /* Counter */
                          pData->aszSpeMnemo,               /* Special Mnemonics */
                          pData->PLUTotal.lAmount);         /* Amount */
            } else {
                if (pData->uchStatus == RPT_DECIMAL_1) {
                    uchDecimal = 1;
                }
                PmgPrintf(usPrtType,                        /* Printer Type */
                          auchPrtSupPLUFile4A21,            /* Format */
                          pData->aszAdjMnemo,               /* Adjective Mnemonics */
                          pData->lPresetPrice,              /* Unit Price */
                          uchDecimal,                       /* Decimal Value */
                          pData->PLUTotal.lCounter,         /* Counter */
                          pData->aszSpeMnemo,               /* Special Mnemonics */
                          pData->PLUTotal.lAmount);         /* Amount */
            }
        } else {
            if (pData->uchStatus == RPT_DECIMAL_0) {
                PmgPrintf(usPrtType,                        /* Printer Type */
                          auchPrtSupPLUFile4B20,            /* Format */
                          pData->aszAdjMnemo,               /* Adjective Mnemonics */
                          pData->lPresetPrice,              /* Unit Price */
                          pData->PLUTotal.lCounter,         /* Counter */
                          pData->aszSpeMnemo,               /* Special Mnemonics */
                          (DCURRENCY)pData->usPercent,                 /* Percent */
                          pData->PLUTotal.lAmount);         /* Amount */
            } else {
                if (pData->uchStatus == RPT_DECIMAL_1) {
                    uchDecimal = 1;
                }
                PmgPrintf(usPrtType,                        /* Printer Type */
                          auchPrtSupPLUFile4B21,            /* Format */
                          pData->aszAdjMnemo,               /* Adjective Mnemonics */
                          pData->lPresetPrice,              /* Unit Price */
                          uchDecimal,                       /* Decimal Value */
                          pData->PLUTotal.lCounter,         /* Counter */
                          pData->aszSpeMnemo,               /* Special Mnemonics */
                          (DCURRENCY)pData->usPercent,                 /* Percent */
                          pData->PLUTotal.lAmount);         /* Amount */
            }
        }
        break;

    case CLASS_RPTPLU_PRTTTLMINUSADJ1:
        /* Convert PLU No. to Double Wide,  Saratoga */
        RflSPrintf(aszRepoNumb, PLU_MAX_DIGIT+1, auchNumber, pData->auchPLUNumber);
        PrtDouble(aszDoubRepoNumb, (4 * 2 + 1), aszRepoNumb);

        PmgPrintf(usPrtType,                            /* Printer Type */
                  auchPrtSupPLUFile4C10,                /* Format */
                  aszDoubRepoNumb,                      /* PLU Number */
                  pData->aszMnemo,                      /* PLU Name */
                  pData->aszAdjMnemo,                   /* Adjective Mnemonics */
                  pData->lPresetPrice,                  /* Unit Price */
                  pData->PLUTotal.lCounter,             /* Counter */
                  pData->aszSpeMnemo,                   /* Special Mnemonics */
                  pData->PLUTotal.lAmount);             /* Amount */
        break;

    case CLASS_RPTPLU_PRTTTLMINUSADJ2:
        PmgPrintf(usPrtType,                            /* Printer Type */
                  auchPrtSupPLUFile4C20,                /* Format */
                  pData->aszAdjMnemo,                   /* Adjective Mnemonics */
                  pData->lPresetPrice,                  /* Unit Price */
                  pData->PLUTotal.lCounter,             /* Counter */
                  pData->aszSpeMnemo,                   /* Special Mnemonics */
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