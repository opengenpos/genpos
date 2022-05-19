/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1998            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print Data Setting Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRTSDATA.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function forms common data format.
*                         
*           The provided function names are as follows: 
* 
*                 VOID PrtSupComData( VOID *pData );     
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* May-14-92: 00.00.01 : J.Ikeda   : initial                                   
* Jun-23-93: 01.00.12 : j.IKEDA   : Adds PARASHARESPRT                                   
* Aug-04-98: 03.03.00 : hrkato    : V3.3
* Aug-13-99: 03.05.00 : M.Teraki  : Merge STORE_RECALL_MODEL/GUEST_CHECK_MODEL
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
#include <stdlib.h>
#include <ecr.h>
/* #include <pif.h> */
#include <paraequ.h> 
#include <para.h>
#include <csttl.h>
#include <csop.h>
#include <report.h> 
#include <maint.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <pmg.h>
/* #include <appllog.h> */
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID PrtSupComData( VOID *pData )
*               
*     Input:    *pData
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function format data.
*===========================================================================
*/

VOID PrtSupComData( VOID *pData )
{
    static const TCHAR FARCONST auchPrtSupCom[] = _T("%8u / %8u");
    static const TCHAR FARCONST auchPrtSupComLong[] = _T("%8u / %8lu");

    ULONG               ulData = 0;
    USHORT              usAddress;
    USHORT              usPrtType;
    USHORT              usPrintControl;
    UCHAR               uchMajorClass = *(( UCHAR *)(pData));

    switch(uchMajorClass) {
    case CLASS_PARATONECTL:
		{
			PARATONECTL  *pParaToneCtl = pData;

			usAddress = pParaToneCtl->uchAddress;
			ulData = pParaToneCtl->uchToneCtl;
			usPrintControl = pParaToneCtl->usPrintControl;
		}
        break;

    case CLASS_PARADISCTBL:
		{
			PARADISCTBL  *pParaDiscTbl = pData;

			usAddress = pParaDiscTbl->uchAddress;
			ulData = pParaDiscTbl->uchDiscRate;
			usPrintControl = pParaDiscTbl->usPrintControl;
		}
        break;

    case CLASS_PARASECURITYNO:
		{
			PARASECURITYNO  *pParaSecurityNo = pData;

			usAddress = pParaSecurityNo->uchAddress;
			ulData = pParaSecurityNo->usSecurityNumber;
			usPrintControl = pParaSecurityNo->usPrintControl;
		}
        break;

    case CLASS_PARATRANSHALO:
		{
			PARATRANSHALO  *pParaTransHalo = pData;

			usAddress = pParaTransHalo->uchAddress;
			ulData = pParaTransHalo->uchHALO;
			usPrintControl = pParaTransHalo->usPrintControl;
		}
        break;

    case CLASS_PARATEND:                                    /* V3.3 */
		{
			PARATEND  *pParaTend = pData;
			usAddress = pParaTend->uchAddress;
			ulData = pParaTend->uchTend;
			usPrintControl = pParaTend->usPrintControl;
		}
        break;

    case CLASS_PARASLIPFEEDCTL:
		{
			PARASLIPFEEDCTL  *pParaSlipFeedCtl = pData;

			usAddress = pParaSlipFeedCtl->uchAddress;
			ulData = pParaSlipFeedCtl->uchFeedCtl;
			usPrintControl = pParaSlipFeedCtl->usPrintControl;
		}
        break;

    case CLASS_PARACTLTBLMENU:
		{
			PARACTLTBLMENU  *pParaCtlTblMenu = pData;

			usAddress = pParaCtlTblMenu->uchAddress;
			ulData = pParaCtlTblMenu->uchPageNumber;
			usPrintControl = pParaCtlTblMenu->usPrintControl;
		}
        break;

    case CLASS_PARAAUTOALTKITCH:
		{
			PARAALTKITCH  *pParaAltKitch = pData;

			usAddress = pParaAltKitch->uchSrcPtr;
			ulData = pParaAltKitch->uchDesPtr;
			usPrintControl = pParaAltKitch->usPrintControl;
		}
        break;

    case CLASS_PARAMANUALTKITCH:
		{
			PARAALTKITCH  *pParaAltKitch = pData;

			usAddress = pParaAltKitch->uchSrcPtr;
			ulData = pParaAltKitch->uchDesPtr;
			usPrintControl = pParaAltKitch->usPrintControl;
		}
        break;

    case CLASS_PARATTLKEYTYP:
		{
			PARATTLKEYTYP  *pParaTtlKeyTyp = pData;

			usAddress = pParaTtlKeyTyp->uchAddress;
			ulData = pParaTtlKeyTyp->uchTypeIdx;
			usPrintControl = pParaTtlKeyTyp->usPrintControl;
		}
        break;

    case CLASS_PARACASHABASSIGN:
		{
			PARACASHABASSIGN  *pParaCashABAssign = pData;

			usAddress = pParaCashABAssign->uchAddress;
			ulData = pParaCashABAssign->ulCashierNo;
			usPrintControl = pParaCashABAssign->usPrintControl;
		}
        break;

    case CLASS_PARASHRPRT:
		{
			PARASHAREDPRT  *pParaSharedPrt = pData;

			usAddress = pParaSharedPrt->uchAddress;
			ulData = pParaSharedPrt->uchTermNo;
			usPrintControl = pParaSharedPrt->usPrintControl;
		}
        break;

    case CLASS_PARASERVICETIME:
		{
			PARASERVICETIME  *pParaServiceTime = pData;

			usAddress = pParaServiceTime->uchAddress;
			ulData = pParaServiceTime->usBorder;
			usPrintControl = pParaServiceTime->usPrintControl;
		}
        break;

    case CLASS_PARALABORCOST:
		{
			PARALABORCOST *pParaLaborCost = pData;

			usAddress = pParaLaborCost->uchAddress;
			ulData = pParaLaborCost->usLaborCost;
			usPrintControl = pParaLaborCost->usPrintControl;
		}
        break;

    default:
/*      PifLog(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_ERROR); */
        break;
    }

    /* check print control */

    switch(usPrintControl) {

    case PRT_RECEIPT:
        usPrtType = PMG_PRT_RECEIPT;
        break;

    case PRT_JOURNAL:
        usPrtType = PMG_PRT_JOURNAL;
        break;

    case PRT_RECEIPT | PRT_JOURNAL:
        usPrtType = PMG_PRT_RCT_JNL;
        break;

    default:
/*      PifLog(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_ERROR); */
        break;
    }

	PmgPrintf(usPrtType, auchPrtSupComLong, usAddress, ulData);
}

/*
*===========================================================================
** Synopsis:    VOID PrtSupOepData( PARAOEPTBL *pData )
*               
*     Input:    *pData
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function format data.
*===========================================================================
*/

VOID PrtSupOepData( PARAOEPTBL *pData )
{

    static const TCHAR FARCONST auchPrtSupOep[] = _T("%4s  %4u / %4u");

    USHORT              usPrtType;
    USHORT              usPrintControl;
    TCHAR               aszTblNumStr[4] = {0,0,0,0};

    if(pData->uchTblNumber){
        _itot(pData->uchTblNumber, aszTblNumStr, 10);
    }
    usPrintControl = pData->usPrintControl;

    /* check print control */

    switch(pData->usPrintControl) {

    case PRT_RECEIPT:
        usPrtType = PMG_PRT_RECEIPT;
        break;

    case PRT_JOURNAL:
        usPrtType = PMG_PRT_JOURNAL;
        break;

    case PRT_RECEIPT | PRT_JOURNAL:
        usPrtType = PMG_PRT_RCT_JNL;
        break;

    default:
/*      PifLog(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_ERROR); */
        break;
    }

    PmgPrintf(usPrtType,       /* printer type */
              auchPrtSupOep,   /* format */
              aszTblNumStr,       /* Table Number */
              pData->uchTblAddr,         /* Table Address */
              pData->uchOepData[0]);        /* OEP data */

}

/* --- End of Source --- */
