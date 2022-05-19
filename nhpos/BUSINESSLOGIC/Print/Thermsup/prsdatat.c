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
* Title       : Thermal Print Data Setting Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRTSDATAT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*                     PrtThrmSupComData() : form common print format    
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-16-93: 01.00.12 : J.IKEDA   : Initial                                   
* Jun-23-93: 01.00.12 : J.IKEDA   : Adds PARASHAREDPRT                                  
* Aug-04-98: 03.03.00 : hrkato    : V3.3
* Aug-12-99: 03.05.00 : K.Iwata   : Marge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
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
#include <cswai.h>
#include <maint.h> 
#include <csop.h>
#include <report.h> 
#include <regstrct.h>
#include <transact.h>
#include <pmg.h>
/* #include <appllog.h> */

#include "prtcom.h"
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID PrtThrmSupComData( VOID *pData )
*               
*     Input:    *pData    : pointer to structure for PARADISCTBL     / PARACTLTBLMENU 
*                                                    PARASECURITYNO  / PARAALTKITCH   
*                                                    PARATRANSHALO   / PARATONECTL    
*                                                    PARASLIPFEEDCTL / PARATTLKEYTYP
*                                                    PARACASHABASSIGN          
*                                                    
*    Output:    Nothing                              
*     InOut:    Nothing                              
*                                                    
** Return:      Nothing
*
** Description: This function forms common format data in prog/super mode.
*
*                  : SECURITY NUMBER FOR PROGRAM MODE
*                  : TRANSACTION HALO
*                  : SLIP FEED CONTROL
*                  : AUTO ALTERNATIVE KITCHEN PRINTER ASSIGNMENT
*                  : SET CONTROL TABLE OF MENU PAGE
*                  : MANUAL ALTERNATIVE KITCHEN PRINTER ASSIGNMENT
*                  : SET DISCOUNT / BONUS % RATES
*                  : ADJUSTMENT TONE VOLUME
*                  : TOTAL KEY TYPE ASSIGNMENT
*                  : CASHIER A/B KEYS ASSIGNMENT
*                  : ASSIGNMENT TERMINAL NO. INSTALLING SHARED PRINTER
*
*===========================================================================
*/

VOID PrtThrmSupComData( VOID *pData )
{
    static const TCHAR FARCONST auchPrtThrmSupCom[] = _T("%18u  /  %8lu");    /* define thermal print format */
    static const TCHAR FARCONST auchPrtSupCom[] = _T("%8u / %8lu");           /* define EJ print format */

    UCHAR      uchMajorClass = *(( UCHAR *)(pData));  /* get major class */
    USHORT     usAddress;
    ULONG      ulData = 0;
    USHORT     usPrintControl = 0;

    /* distinguish major class */
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
			PARASECURITYNO   *pParaSecurityNo = pData;
			usAddress = pParaSecurityNo->uchAddress;
			ulData = pParaSecurityNo->usSecurityNumber;
			usPrintControl = pParaSecurityNo->usPrintControl;
		}
        break;

    case CLASS_PARATRANSHALO:
		{
			PARATRANSHALO   *pParaTransHalo = pData;
			usAddress = pParaTransHalo->uchAddress;
			ulData = pParaTransHalo->uchHALO;
			usPrintControl = pParaTransHalo->usPrintControl;
		}
        break;

    case CLASS_PARATEND:                                        /* V3.3 */
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
			PARAALTKITCH  *pParaAltKitch = ( PARAALTKITCH *)pData;
			usAddress = pParaAltKitch->uchSrcPtr;
			ulData = pParaAltKitch->uchDesPtr;
			usPrintControl = pParaAltKitch->usPrintControl;
		}
        break;

    case CLASS_PARATTLKEYTYP:
		{
			PARATTLKEYTYP  *pParaTtlKeyTyp = ( PARATTLKEYTYP *)pData;
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
			PARALABORCOST  *pParaLaborCost = pData;
			usAddress = pParaLaborCost->uchAddress;
			ulData = pParaLaborCost->usLaborCost;
			usPrintControl = pParaLaborCost->usPrintControl;
		}
        break;

    default:
/*         PifLog(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_ERROR); */
        break;
    }

    /* check print control */
    if (usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */
        /* print ADDRESS / DATA */
		PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupCom, usAddress, ulData);
    } 
    
    if (usPrintControl & PRT_JOURNAL) {  /* EJ */
        /* print ADDRESS / DATA */
		PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupCom, usAddress, ulData);
    }
}
/*
*===========================================================================
** Synopsis:    VOID PrtThrmSupOepData( PARAOEPTBL *pData )
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

VOID PrtThrmSupOepData( PARAOEPTBL *pData )
{
    static const TCHAR FARCONST auchPrtSupOep[] = _T("%4s  %4u / %4u");
    static const TCHAR FARCONST auchPrtSupOepThrm[] = _T("%8s    %8u   /   %8u");

    USHORT              usPrintControl;
    TCHAR               aszTblNumStr[4] = {0,0,0,0};

    if(pData->uchTblNumber){
        _itot(pData->uchTblNumber, aszTblNumStr, 10);
    }
    usPrintControl = pData->usPrintControl;

    /* check print control */
    switch(pData->usPrintControl) {

    case PRT_RECEIPT:
        PrtPrintf(PMG_PRT_RECEIPT, auchPrtSupOepThrm, aszTblNumStr, pData->uchTblAddr, pData->uchOepData[0]);
        break;

    case PRT_JOURNAL:
        PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupOep, aszTblNumStr, pData->uchTblAddr, pData->uchOepData[0]);
        break;

    case PRT_RECEIPT | PRT_JOURNAL:
        PrtPrintf(PMG_PRT_RECEIPT, auchPrtSupOepThrm, aszTblNumStr, pData->uchTblAddr, pData->uchOepData[0]);

        PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupOep, aszTblNumStr, pData->uchTblAddr, pData->uchOepData[0]);
        break;

    default:
        break;
    }

}

/***** End of Source *****/
