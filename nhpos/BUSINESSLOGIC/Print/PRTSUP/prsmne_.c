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
* Title       : Print Mnemonics Setting Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRTMNE_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function forms common data format.
*                         
*           The provided function names are as follows: 
* 
*                 VOID PrtSupMnemData();     
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-02-92: 00.00.01 : K.You     : initial                                   
* Jun-16-93: 01.00.12 : J.IKEDA   : Rel.2 initial, Adds 44char format                                   
* Aug-23-93: 01.00.13 : J.IKEDA   : Add PARASOFTCHK, Del PARACHAR44
* Aug-24-93: 01.00.13 : J.IKEDA   : Chg. print format for PARAPCIF 
* Aug-11-95: 03.00.04 : M.Ozawa   : Chg. print format for PARALEADTHRU
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
#include <report.h> 
#include <maint.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <pmg.h>
#include <appllog.h>
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID PrtSupMnemData( VOID *pData )
*               
*       Input:  *pData          : Pointer to Structure for PARAPROMOTION, PARATRANSMNEMO
*                                                          PARALEADTHRU, PARAREPORTNAME
*                                                          PARASPECIALMNEMO, PARAADJMNEMO
*                                                          PARAPRTMODI, PARAMAJORDEPT
*                                                          PARACHAR24, PARAPCIF, PARASOFTCHK
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This Function Format Mnemonics Data.
*
*               See as well the similar function DispSupMnemo20() in the Display subsystem.
*
*===========================================================================
*/

VOID PrtSupMnemData( VOID *pData )
{
	TCHAR               aszBuffer[PARA_SOFTCHK_LEN + 1] = { 0 };
    UCHAR               uchMajorClass = *((UCHAR *)(pData));                    /* Get Major Class */
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
 
    
    switch(uchMajorClass) {
    case CLASS_PARAPROMOTION:
        usAddress = pParaPromotion->uchAddress;            /* Set Address */
        _tcsncpy(aszBuffer, pParaPromotion->aszProSales, PARA_PROMOTION_LEN);    /* Set Mnemonics */
        usPrtType = PrtSupChkType(pParaPromotion->usPrintControl);  /* Get Printer Control */
        break;

    case CLASS_PARATRANSMNEMO:
        usAddress = pParaTransMnemo->uchAddress;               /* Set Address */
        _tcsncpy(aszBuffer, pParaTransMnemo->aszMnemonics, PARA_TRANSMNEMO_LEN);  /* Set Mnemonics */
        usPrtType = PrtSupChkType(pParaTransMnemo->usPrintControl);     /* Get Printer Control */
        break;

    case CLASS_PARALEADTHRU:
        usAddress = pParaLeadThru->uchAddress;                 /* Set Address */
        _tcsncpy(aszBuffer, pParaLeadThru->aszMnemonics, PARA_LEADTHRU_LEN);  /* Set Mnemonics */
        usPrtType = PrtSupChkType(pParaLeadThru->usPrintControl);       /* Get Printer Control */
        break;

    case CLASS_PARAREPORTNAME:
        usAddress = pParaReportName->uchAddress;               /* Set Address */
        _tcsncpy(aszBuffer, pParaReportName->aszMnemonics, PARA_REPORTNAME_LEN);  /* Set Mnemonics */
        usPrtType = PrtSupChkType(pParaReportName->usPrintControl);     /* Get Printer Control */
        break;

    case CLASS_PARASPECIALMNEMO:
        usAddress = pParaSpecialMnemo->uchAddress;             /* Set Address */
        _tcsncpy(aszBuffer, pParaSpecialMnemo->aszMnemonics, PARA_SPEMNEMO_LEN);  /* Set Mnemonics */
        usPrtType = PrtSupChkType(pParaSpecialMnemo->usPrintControl);   /* Get Printer Control */
        break;

    case CLASS_PARAADJMNEMO:
        usAddress = pParaAdjMnemo->uchAddress;                 /* Set Address */
        _tcsncpy(aszBuffer, pParaAdjMnemo->aszMnemonics, PARA_ADJMNEMO_LEN);  /* Set Mnemonics */
        usPrtType = PrtSupChkType(pParaAdjMnemo->usPrintControl);       /* Get Printer Control */
        break;

    case CLASS_PARAPRTMODI:
        usAddress = pParaPrtModi->uchAddress;                  /* Set Address */
        _tcsncpy(aszBuffer, pParaPrtModi->aszMnemonics, PARA_PRTMODI_LEN);    /* Set Mnemonics */
        usPrtType = PrtSupChkType(pParaPrtModi->usPrintControl);        /* Get Printer Control */
        break;

    case CLASS_PARAMAJORDEPT:
        usAddress = pParaMajorDEPT->uchAddress;                /* Set Address */
        _tcsncpy(aszBuffer, pParaMajorDEPT->aszMnemonics, PARA_MAJORDEPT_LEN);    /* Set Mnemonics */
        usPrtType = PrtSupChkType(pParaMajorDEPT->usPrintControl);      /* Get Printer Control */
        break;

    case CLASS_PARACHAR24:
        usAddress = pParaChar24->uchAddress;                   /* Set Address */
        _tcsncpy(aszBuffer, pParaChar24->aszMnemonics, PARA_CHAR24_LEN);  /* Set Mnemonics */
        usPrtType = PrtSupChkType(pParaChar24->usPrintControl);         /* Get Printer Control */
        break;

    case CLASS_PARAPCIF:
        usAddress = pParaPCIF->uchAddress;                     /* Set Address */
        _tcsncpy(aszBuffer, pParaPCIF->aszMnemonics, PARA_PCIF_LEN);  /* Set Mnemonics */
        usPrtType = PrtSupChkType(pParaPCIF->usPrintControl);           /* Get Printer Control */
        break;

   case CLASS_PARASOFTCHK:
        usAddress = pParaSoftChk->uchAddress;                        /* set address */
        _tcsncpy(aszBuffer, pParaSoftChk->aszMnemonics, PARA_SOFTCHK_LEN);  /* set mnemonics */
        usPrtType = PrtSupChkType(pParaSoftChk->usPrintControl);              /* get printer control */
        break;                                                             

   case CLASS_PARAHOTELID:
        usAddress = pParaHotelId->uchAddress;                         /* set address */
        _tcsncpy(aszBuffer, pParaHotelId->aszMnemonics, MAX_HOTELID_SIZE);     /* set mnemonics */
        usPrtType = PrtSupChkType(pParaHotelId->usPrintControl);              /* get printer control */
        break;                                                             

   case CLASS_PARAMLDMNEMO: /* V3.3 */
        usAddress = pParaMldMnemo->usAddress;                                    /* set address */
        _tcsncpy(aszBuffer, pParaMldMnemo->aszMnemonics, PARA_MLDMNEMO_LEN);     /* set mnemonics */
        usPrtType = PrtSupChkType(pParaMldMnemo->usPrintControl);              /* get printer control */
        break;                                                             

    default:
/*        PifAbort(MODULE_PRINT_SUPPRG_ID, FAULT_INVALID_DATA); */
        return;

    }

    /* Print Line */
    if ((uchMajorClass == CLASS_PARACHAR24) 
		    || (uchMajorClass == CLASS_PARAPROMOTION)
            || (uchMajorClass == CLASS_PARAPCIF)
            || (uchMajorClass == CLASS_PARAMLDMNEMO)
		    || (uchMajorClass == CLASS_PARASOFTCHK)) {
		/* Define Print Format */
		static const TCHAR  auchPrtSupMnemData2[] = _T("%2d /\n%s");

		PmgPrintf(usPrtType, auchPrtSupMnemData2, usAddress, aszBuffer);                       /* Mnemonics Data */
	} else {
		static const TCHAR  auchPrtSupMnemData1[] = _T("%7d / %-10s");
		static const TCHAR  auchPrtSupMnemData3[] = _T("%3d/ %-16s");

		if (uchMajorClass == CLASS_PARALEADTHRU) {
			PmgPrintf(usPrtType, auchPrtSupMnemData3, usAddress, aszBuffer);                   /* Mnemonics Data */
		} else {
	        PmgPrintf(usPrtType, auchPrtSupMnemData1, usAddress, aszBuffer);                   /* Mnemonics Data */
		}

    }
}
