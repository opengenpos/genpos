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
* Title       : Thermal Print Mnemonics Setting Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRTMNET.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function forms common data format.
*                         
*           The provided function names are as follows: 
* 
*             PrtThrmSupMnemData() : form common format for mnemonic setting    
*                                    print format
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-16-93: 01.00.12 : J.IKEDA   : Initial                                   
* Aug-23-93: 01.00.13 : J.IKEDA   : Add PARASOFTCHK, Del PARACHAR44                                   
* Aug-24-93: 01.00.13 : J.IKEDA   : Chg. print format for PARAPCIF
* Aug-11-95: 03.00.04 : M.Ozawa   : Chg. print format for PARALEADTHOU
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
/* #include <log.h> */
#include <paraequ.h> 
#include <para.h>
#include <csttl.h>
#include <csop.h>
/* #include <cswai.h> */
#include <report.h> 
#include <maint.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <pmg.h>
/* #include <appllog.h> */

#include "prtcom.h"
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID PrtThrmSupMnemData( VOID *pData )
*               
*       Input:  *pData          : pointer to structure for PARAPROMOTION, PARATRANSMNEMO
*                                                          PARALEADTHRU, PARAREPORTNAME
*                                                          PARASPECIALMNEMO, PARAADJMNEMO
*                                                          PARAPRTMODI, PARAMAJORDEPT
*                                                          PARACHAR24, PARAPCIF
*    Output:    Nothing                                    PARASOFTCHK
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms mnemonic data print format.
*
*                     : TRANSACTION MNEMONICS
*                     : LEAD THROUGH MNEMONICS
*                     : REPORT NAME
*                     : SPECIAL MNEMONICS
*                     : PRINT MODIFIER MNEMONICS
*                     : MAJOR DEPARTMENT MNEMONICS
*                     : 24 CHARACTERS MNEMONICS
*                     : PASSWORD FOR P/C INTERFACE
*                     : SET PROMOTION MESSAGE
*                     : SET TRAILER PRINT MESSAGE FOR SOFT CHECK
*                     : ADJECTIVE MNEMONICS
*
*               See as well the similar function DispSupMnemo20() in the Display subsystem.
*
*===========================================================================
*/

VOID PrtThrmSupMnemData( VOID *pData )
{
	TCHAR               aszBuffer[PARA_SOFTCHK_LEN + 1] = { 0 };
    UCHAR               uchMajorClass = *((UCHAR *)(pData));   /* get major class */
    USHORT              usAddress = 0;
    USHORT              usPrtType = 0;
                    
    PARAPROMOTION       * const pParaPromotion = pData;
    PARATRANSMNEMO      * const pParaTransMnemo = pData;
    PARALEADTHRU        * const pParaLeadThru = pData;
    PARAREPORTNAME      * const pParaReportName = pData;
    PARASPEMNEMO        * const pParaSpecialMnemo = pData;
    PARAADJMNEMO        * const pParaAdjMnemo = pData;
    PARAPRTMODI         * const pParaPrtModi = pData;
    PARAMAJORDEPT       * const pParaMajorDEPT = pData;
    PARACHAR24          * const pParaChar24 = pData;
    PARAPCIF            * const pParaPCIF = pData;
    PARASOFTCHK         * const pParaSoftChk = pData;
    PARAHOTELID         * const pParaHotelId = pData;
    PARAMLDMNEMO        * const pParaMldMnemo = pData;

    /* distinguish major class */ 
    switch(uchMajorClass) {
    case CLASS_PARAPROMOTION:
        usAddress = pParaPromotion->uchAddress;                             /* set address */
        _tcsncpy(aszBuffer, pParaPromotion->aszProSales, PARA_PROMOTION_LEN);    /* set mnemonics */
        usPrtType = pParaPromotion->usPrintControl;                         /* get printer control */
        break;

    case CLASS_PARATRANSMNEMO:
        usAddress = pParaTransMnemo->uchAddress;                            /* set address */
        _tcsncpy(aszBuffer, pParaTransMnemo->aszMnemonics, PARA_TRANSMNEMO_LEN);  /* set mnemonics */
        usPrtType = pParaTransMnemo->usPrintControl;                        /* get printer control */
        break;

    case CLASS_PARALEADTHRU:
        usAddress = pParaLeadThru->uchAddress;                              /* set address */
        _tcsncpy(aszBuffer, pParaLeadThru->aszMnemonics, PARA_LEADTHRU_LEN);  /* set mnemonics */
        usPrtType = pParaLeadThru->usPrintControl;                          /* get printer control */
        break;

    case CLASS_PARAREPORTNAME:
        usAddress = pParaReportName->uchAddress;                            /* set address */
        _tcsncpy(aszBuffer, pParaReportName->aszMnemonics, PARA_REPORTNAME_LEN);  /* set mnemonics */
        usPrtType = pParaReportName->usPrintControl;                        /* get printer control */
        break;

    case CLASS_PARASPECIALMNEMO:
        usAddress = pParaSpecialMnemo->uchAddress;                          /* set address */
        _tcsncpy(aszBuffer, pParaSpecialMnemo->aszMnemonics, PARA_SPEMNEMO_LEN);  /* set mnemonics */
        usPrtType = pParaSpecialMnemo->usPrintControl;                      /* get printer control */
        break;

    case CLASS_PARAADJMNEMO:
        usAddress = pParaAdjMnemo->uchAddress;                              /* set address */
        _tcsncpy(aszBuffer, pParaAdjMnemo->aszMnemonics, PARA_ADJMNEMO_LEN);  /* set mnemonics */
        usPrtType = pParaAdjMnemo->usPrintControl;                          /* get printer control */
        break;

    case CLASS_PARAPRTMODI:
        usAddress = pParaPrtModi->uchAddress;                               /* set address */
        _tcsncpy(aszBuffer, pParaPrtModi->aszMnemonics, PARA_PRTMODI_LEN);    /* set mnemonics */
        usPrtType = pParaPrtModi->usPrintControl;                           /* get printer control */
        break;

    case CLASS_PARAMAJORDEPT:
        usAddress = pParaMajorDEPT->uchAddress;                             /* set address */
        _tcsncpy(aszBuffer, pParaMajorDEPT->aszMnemonics, PARA_MAJORDEPT_LEN);    /* set mnemonics */
        usPrtType = pParaMajorDEPT->usPrintControl;                         /* get printer control */
        break;

    case CLASS_PARACHAR24:
        usAddress = pParaChar24->uchAddress;                              /* set address */
        _tcsncpy(aszBuffer, pParaChar24->aszMnemonics, PARA_CHAR24_LEN);  /* set mnemonics */
        usPrtType = pParaChar24->usPrintControl;                          /* get printer control */
        break;

    case CLASS_PARAPCIF:
        usAddress = pParaPCIF->uchAddress;                                /* set Address */
        _tcsncpy(aszBuffer, pParaPCIF->aszMnemonics, PARA_PCIF_LEN);      /* set Mnemonics */
        usPrtType = pParaPCIF->usPrintControl;                            /* get printer control */
        break;

   case CLASS_PARASOFTCHK: 
        usAddress = pParaSoftChk->uchAddress;                                /* set address */
        _tcsncpy(aszBuffer, pParaSoftChk->aszMnemonics, PARA_SOFTCHK_LEN);   /* set mnemonics */
        usPrtType = pParaSoftChk->usPrintControl;                            /* get printer control */
        break;                                                            
                                                 
   case CLASS_PARAHOTELID:
        usAddress = pParaHotelId->uchAddress;                                /* set address */
        _tcsncpy(aszBuffer, pParaHotelId->aszMnemonics, MAX_HOTELID_SIZE);   /* set mnemonics */
        usPrtType = pParaHotelId->usPrintControl;                            /* get printer control */
        break;                                                             

   case CLASS_PARAMLDMNEMO:  /* V3.3 */
        usAddress = pParaMldMnemo->usAddress;                                  /* set address */
        _tcsncpy(aszBuffer, pParaMldMnemo->aszMnemonics, PARA_MLDMNEMO_LEN);   /* set mnemonics */
        usPrtType = pParaMldMnemo->usPrintControl;                             /* get printer control */
        break;                                                             

    default:
/*        PifAbort(MODULE_PRINT_SUPPRG_ID, FAULT_INVALID_DATA); */
        return;
    }

    /* check print control */
    if (usPrtType & PRT_RECEIPT) {  /* THERMAL PRINTER */
		/* define thermal print format */
		static const TCHAR  auchPrtThrmSupMnemData1[] = _T("%15u / %-24s");
		static const TCHAR  auchPrtThrmSupMnemData2[] = _T("%1u /\n%s");

		/* print ADDRESS / MNEMONIC */
        if ((uchMajorClass == CLASS_PARASOFTCHK) || (uchMajorClass == CLASS_PARAMLDMNEMO) || (uchMajorClass == CLASS_PARAPCIF)) {

            PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupMnemData2, usAddress, aszBuffer);
       	} else {
            PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupMnemData1, usAddress, aszBuffer);
        }
    } 
    
    if (usPrtType & PRT_JOURNAL) {  /* EJ */
		/* define EJ print format */
		static const TCHAR  auchPrtSupMnemData1[] = _T("%7d / %-10s");
		static const TCHAR  auchPrtSupMnemData2[] = _T("%2d /\n%s");
		static const TCHAR  auchPrtSupMnemData3[] = _T("%3d/ %-16s");

		/* print ADDRESS / MNEMONIC */
        if ((uchMajorClass == CLASS_PARACHAR24) || (uchMajorClass == CLASS_PARAPROMOTION) || (uchMajorClass == CLASS_PARAPCIF) ||
            (uchMajorClass == CLASS_PARAMLDMNEMO) || (uchMajorClass == CLASS_PARASOFTCHK)) {   

            PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupMnemData2, usAddress, aszBuffer);
		} else {
		    PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupMnemData3, usAddress, aszBuffer);
		}
    }
}
/***** End of Source *****/
