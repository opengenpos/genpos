/*
		Copyright	Georgia Southern University
					Statesboro,  GA 30460

					NCR Project Development Team
*===========================================================================
* Title       : DATA SETTING 1( SUPER & PROGRAM MODE )                       
* Category    : Display-2x20, NCR 2170 US Hospitality Application Program        
* Program Name: DSPCOM20.C
* --------------------------------------------------------------------------
* Abstract: This function displays data.
*
*           The provided function names are as follows: 
*               
*           VOID DispSupComData20( VOID *pData );
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* May-12-92: 00.00.01 : J.Ikeda   : initial                                   
* Jun-23-93: 01.00.12 : J.IKEDA   : Adds CLASS_PARASHRPRT                                   
* Aug-04-98: 03.03.00 : hrkato    : V3.3
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
#include <stdlib.h>

#include "ecr.h"
#include "uie.h"
/* #include <pif.h> */
/* #include <log.h> */
#include "paraequ.h"
#include "para.h"
#include "maint.h"
#include <appllog.h>
#include "display.h"
#include "disp20.h"

/*
*===========================================================================
** Synopsis:    VOID DispSupComData20( VOID *pData )
*               
*     Input:    *Data
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function displays data.
*===========================================================================
*/

VOID DispSupComData20( VOID *pData )
{
    static const TCHAR  auchDispSupCom20A[] = _T("%3u  %3u\t%4u\n%-18s\t%s");
    static const TCHAR  auchDispSupCom20Along[] = _T("%3u  %3u\t%8lu\n%-18s\t%s");
    static const TCHAR  auchDispSupCom20B[] = _T("%3u  %3u\t%10s\n%-18s\t%s");

    ULONG  ulData = 0;
    TCHAR  aszDspNull[1] = {0};
    TCHAR  aszDspPage[4] = {0};
    TCHAR  *pDspMnemo = aszDspNull;    // default mnemonic is none but some structs do have a mnemonic to print.
    USHORT usNoData = 0;
    USHORT usAddress = 0;
    UCHAR  uchMajorClass = *((UCHAR *)(pData));

	NHPOS_ASSERT(uchMaintMenuPage < 99);

    switch(uchMajorClass) {
    case CLASS_PARATONECTL:
		{
			PARATONECTL       *pParaToneCtl = pData;

			usAddress = pParaToneCtl->uchAddress;
			usNoData = AC_TONE_CTL;
			ulData = pParaToneCtl->uchToneCtl;
			pDspMnemo = pParaToneCtl->aszLeadMnemonics;
			DispSupSaveData.Tone = *pParaToneCtl;        /* Save Data for Redisplay if needed. */
			_itot(uchMaintMenuPage, aszDspPage, 10);
		}
        break;                                                                 

    case CLASS_PARADISCTBL:
		{
			PARADISCTBL  *pParaDiscTbl = pData;
			usAddress = pParaDiscTbl->uchAddress;
			usNoData = AC_DISC_BONUS;
			ulData = pParaDiscTbl->uchDiscRate;
			pDspMnemo = pParaDiscTbl->aszMnemonics;
			DispSupSaveData.DiscTbl = *pParaDiscTbl;    /* Save Data for Redisplay if needed. */ 
			_itot(uchMaintMenuPage, aszDspPage, 10);
		}
        break;                                                          

    case CLASS_PARASECURITYNO:
		{
			PARASECURITYNO    *pParaSecurityNo = pData;

			usAddress = pParaSecurityNo->uchAddress;
			usNoData = PG_SEC_PROG;
			ulData = pParaSecurityNo->usSecurityNumber;
			DispSupSaveData.SecurityNo = *pParaSecurityNo;   /* Save Data for Redisplay if needed. */
		}
        break;                                                                        

    case CLASS_PARATRANSHALO:
		{
			PARATRANSHALO  *pParaTransHalo = pData;

			usAddress = pParaTransHalo->uchAddress;
			usNoData = PG_TRAN_HALO;
			ulData = pParaTransHalo->uchHALO;
			DispSupSaveData.TransHALO = *pParaTransHalo;    /* Save Data for Redisplay if needed. */
		}
        break;                                                                  

    case CLASS_PARATEND:                                    /* V3.3 */
		{
			PARATEND  *pParaTend = pData;

			usAddress = pParaTend->uchAddress;
			usNoData = PG_TEND_PARA;
			ulData = pParaTend->uchTend;
			DispSupSaveData.Tend = *pParaTend;
		}
        break;                                                                  

    case CLASS_PARASLIPFEEDCTL:
		{
			PARASLIPFEEDCTL   *pParaSlipFeedCtl = pData;
			usAddress = pParaSlipFeedCtl->uchAddress;
			usNoData = PG_SLIP_CTL;
			ulData = pParaSlipFeedCtl->uchFeedCtl;                           
			DispSupSaveData.SlipFeedCtl = *pParaSlipFeedCtl;    /* Save Data for Redisplay if needed. */
		}
        break;                                                                      

    case CLASS_PARACTLTBLMENU:
		{
			PARACTLTBLMENU    *pParaCtlTblMenu = pData;

			usAddress = pParaCtlTblMenu->uchAddress;
			usNoData = AC_SET_CTLPG;
			ulData = pParaCtlTblMenu->uchPageNumber;
			pDspMnemo = pParaCtlTblMenu->aszMnemonics;
			DispSupSaveData.CtlTblMenu = *pParaCtlTblMenu;   /* Save Data for Redisplay if needed. */
			_itot(uchMaintMenuPage, aszDspPage, 10);
		}
        break;                                                                        

    case CLASS_PARAAUTOALTKITCH:
		{
			PARAALTKITCH  *pParaAutoAltKitch = pData;

			usAddress = pParaAutoAltKitch->uchSrcPtr;
			usNoData = PG_AUTO_KITCH;
			ulData = pParaAutoAltKitch->uchDesPtr;
			DispSupSaveData.AltKitch = *pParaAutoAltKitch;   /* Save Data for Redisplay if needed. */
		}
        break;                                                                             

    case CLASS_PARAMANUALTKITCH:
		{
			PARAALTKITCH  *pParaManuAltKitch = pData;

			usAddress = pParaManuAltKitch->uchSrcPtr;
			usNoData = AC_MANU_KITCH;
			ulData = pParaManuAltKitch->uchDesPtr;
			pDspMnemo = pParaManuAltKitch->aszMnemonics;
			DispSupSaveData.AltKitch = *pParaManuAltKitch;   /* Save Data for Redisplay if needed. */
			_itot(uchMaintMenuPage, aszDspPage, 10);
		}
        break;                                                                      

    case CLASS_PARATTLKEYTYP:
		{
			PARATTLKEYTYP  *pParaTtlKeyTyp = pData;

			usAddress = pParaTtlKeyTyp->uchAddress;
			usNoData = PG_TTLKEY_TYP;
			ulData = pParaTtlKeyTyp->uchTypeIdx;
			DispSupSaveData.TtlKeyTyp = *pParaTtlKeyTyp;  /* Save Data for Redisplay if needed. */
		}
        break;                                                                     

    case CLASS_PARACASHABASSIGN:
		{
			PARACASHABASSIGN  *pParaCashABAssign = pData;

			usAddress = pParaCashABAssign->uchAddress;
			usNoData = AC_CASH_ABKEY;
			ulData = pParaCashABAssign->ulCashierNo;
			pDspMnemo = pParaCashABAssign->aszMnemonics;
			DispSupSaveData.CashABAssign = *pParaCashABAssign;  /* Save Data for Redisplay if needed. */
			_itot(uchMaintMenuPage, aszDspPage, 10);
		}
		break;                                                                                  

    case CLASS_PARASHRPRT:
		{
			PARASHAREDPRT  *pParaSharedPrt = pData;

			usAddress = pParaSharedPrt->uchAddress;
			usNoData = PG_SHR_PRT;
			ulData = pParaSharedPrt->uchTermNo;
			DispSupSaveData.SharedPrt = *pParaSharedPrt;    /* Save Data for Redisplay if needed. */
		}
        break;

    case CLASS_PARASERVICETIME:
		{
			PARASERVICETIME   *pParaServiceTime = pData;

			usAddress = pParaServiceTime->uchAddress;
			usNoData = AC_SERVICE_MAINT;
			ulData = pParaServiceTime->usBorder;
			pDspMnemo = pParaServiceTime->aszMnemonics;
			DispSupSaveData.ServTime = *pParaServiceTime;    /* Save Data for Redisplay if needed. */
			_itot(uchMaintMenuPage, aszDspPage, 10);
		}
        break;                                                          

    case CLASS_PARALABORCOST:
		{
			PARALABORCOST  *pParaLaborCost = pData;

			usAddress = pParaLaborCost->uchAddress;
			usNoData = AC_ETK_LABOR_COST;
			ulData = pParaLaborCost->usLaborCost;
			pDspMnemo = pParaLaborCost->aszMnemonics;
			DispSupSaveData.LaborCost = *pParaLaborCost;   /* Save Data for Redisplay if needed. */
			_itot(uchMaintMenuPage, aszDspPage, 10);
		}
        break;                                                          

    case CLASS_PARABOUNDAGE: /* 2172 */
		{
			PARABOUNDAGE  *pParaBoundAge = pData;

			usAddress = pParaBoundAge->uchAddress;
			usNoData = AC_BOUNDARY_AGE;
			ulData = pParaBoundAge->uchAgePara;
			pDspMnemo = pParaBoundAge->aszMnemonics;
			DispSupSaveData.BoundAge = *pParaBoundAge;   /* Save Data for Redisplay if needed. */
		}
        break;

    default:
        PifLog(MODULE_DISPSUP_ID, LOG_EVENT_UI_DISPLAY_ERR_01);
		return;
        break;

    }

	// The mnemonic needs to fit within the display area which
	// is 40 characters wide.
	NHPOS_ASSERT(_tcslen(pDspMnemo) < 41);

	/* Display ForeGround */
	UiePrintf(UIE_OPER,                     /* operator display */
			  0,                            /* row */
			  0,                            /* column */
			  auchDispSupCom20Along,        /* format */
			  usNoData,                     /* super/program number */
			  usAddress,                    /* address */
			  ulData,                       /* data */
			  pDspMnemo,                    /* Mnemonics Data */    
			  aszDspPage);                  /* Page Number */

    /* Display Background */
    UieEchoBackGround(UIE_KEEP,             /* descriptor control */
                      UIE_ATTR_NORMAL,      /* attribute */
                      auchDispSupCom20B,    /* format */
                      usNoData,             /* super/program number */
                      usAddress,            /* address */
                      aszDspNull,           /* null */
                      pDspMnemo,            /* Mnemonics Data */    
                      aszDspPage);          /* Page Number */
}

/* --- End of Source --- */
