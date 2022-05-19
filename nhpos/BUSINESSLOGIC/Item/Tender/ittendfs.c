/*=======================================================================*\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  2170         *             NCR Corporation, E&M OISO        ||
||        * << ECR-90 >>  *                                              ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1996-2000      ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
:   Title          : TENDER MODULE
:   Category       : TENDER MODULE, NCR 2170 US GP Application
:   Program Name   : ITTENDFS.C (food stamp (even/over) tender)
* --------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
*       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
*       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
*       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
*       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
*       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
* --------------------------------------------------------------------------
:  Abstract:
:   ItemTendFS()                ;   food stamp tender and comments about Food Stamps
:   ItemTendFSCheck()           ;   check F.S tender
:   ItemTendFSAmount()          ;   calculate F.S tender amount
:   ItemTendFSChange()          ;   generate F.S/F.S credit data
:   ItemTendFSChangeOver()      ;   generate F.S/F.S credit data (over tender)
:   ItemTendFSChit()            ;   generate F.S credit (chit, voucher)
:
:   ItemTendFSNonFoodPart()     ;   Proceed non-food partial tender
:   ItemTendFSNonFoodPartTax()  ;   calculate and save tax for no total key
:   ItemTendFSPart()            ;   F.S partial tener
:   ItemTendFSDisp()            ;   display F.S tender amount
:
:   ItemTendFSOver()            ;   F.S over tender (FMI > MI)
:   ItemTendFSOverTender()      ;   generate F.S tender class (FMI > MI)
:   ItemTendFSTax()             ;   generate tax/total key class
:
:  ---------------------------------------------------------------------
:  Update Histories
:   Date    :   Version/Revision  :  Name     :   Description
:   93/3/04     00.00.01          : hkato
:   94/2/22     00.00.01          : hkato     :
:   95/9/26     01.01.06          : hkato     :
:   96/11/15    02.00.00          : hrkato    :   R2.0
:   00/5/15     01.00.01          : hrkato    :   FVT
:   15/03/30    02.02.01          : rchamber  :   replace memcpy(), use TrnInformation pointers
-------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*\
:   PVCS ENTRY
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

#include	<tchar.h>
#include    <stdlib.h>
#include    <string.h>

#include    "ecr.h"
#include    "uie.h"
#include    "pif.h"
#include    "paraequ.h"
#include    "para.h"
#include    "regstrct.h"
#include    "uireg.h"
#include    "transact.h"
#include    "trans.h"
#include    "item.h"
#include    "csstbpar.h"
#include    "csstbgcf.h"
#include    "csttl.h"
#include    "rfl.h"
#include    "display.h"
#include    "mld.h"
#include    "cpm.h"
#include    "itmlocal.h"
#include    "prt.h"
#include    "pmg.h"

static SHORT   ItemTendFSCheck( UIFREGTENDER *pData, ITEMTENDER *pItemTender );
static SHORT   ItemTendFSAmount( UIFREGTENDER *pData1, ITEMTENDER *pData2 );
static VOID    ItemTendFSChange( DCURRENCY  lAmount );
static VOID    ItemTendFSChangeOver( DCURRENCY lAmount );
static VOID    ItemTendFSTax( ITEMTENDER   *pData );

static VOID    ItemTendGetMDC(UCHAR uchClass, UCHAR *auchStatus);

/*
*==========================================================================
**  Synopsis:   SHORT   ItemTendCheckFS(UIFREGTENDER *pUifTender)
*
*   Input:      UIFREGTENDER    *UifRegTender
*   Output:     none
*   InOut:      none
*
*   Return:		ITM_SUCESS if the tender key is a food stamp tender key
*				ITM_ERROR if the tender key is not a food stamp tender key
*
*   Description:    Check to see if a tender key is provisioned as a
*					food stamp tender or not.
*==========================================================================
*/
SHORT   ItemTendCheckFS(UIFREGTENDER *pUifTender)
{
	SHORT   sRetStatus = ITM_ERROR;

	/* add major class check for preset cash */
	if (pUifTender->uchMajorClass == CLASS_UIFREGTENDER) {
	    if (pUifTender->uchMinorClass == CLASS_UITENDER2 && CliParaMDCCheckField (MDC_FS2_ADR, MDC_PARAM_BIT_D)) {
            sRetStatus = ITM_SUCCESS;
        }
	    if (pUifTender->uchMinorClass == CLASS_UITENDER11 && CliParaMDCCheckField (MDC_FS2_ADR, MDC_PARAM_BIT_C)) {
            sRetStatus = ITM_SUCCESS;
        }
    }
    	
	return (sRetStatus);
}

SHORT  ItemTendDetermineSignFS (VOID)
{
	SHORT           sSign = 1;

	if (TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)) {
		if (TranItemizersPtr->lMI < 0) {
			sSign = -1;
		}
	}

	return sSign;
}

/*
*==========================================================================
**  Synopsis:   VOID    ItemTendSendFS( VOID )
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    generate Food Stamp total class
*==========================================================================
*/
VOID    ItemTendSendFS(VOID)
{
	UCHAR       uchTenderMinorClass = ItemTenderLocal.ItemFSTender.uchMinorClass;

    if (ItemTenderLocal.ItemFSTotal.uchMajorClass != 0              /* FS partial tender */
        && ItemTenderLocal.ItemFSTender.uchMajorClass != 0          /* FS partial tender */
        && !(ItemTenderLocal.fbTenderStatus[1] & TENDER_PARTIAL)) { /* not partial tender */

        /*--- Second Food Stamp Tender, V2.01.04 --- */
        if (!(ItemTenderLocal.fbTenderStatus[1] & TENDER_FS_PARTIAL2)) {
			ITEMTOTAL   WorkTotal = {0};

            ItemTendFSTotal(&WorkTotal);
			WorkTotal.lMI -= ItemTenderLocal.lFSTenderizer - ItemTenderLocal.ItemFSTender.lTenderAmount;
            TrnItem(&WorkTotal);
            MldScrollWrite(&WorkTotal, MLD_NEW_ITEMIZE);
            MldScrollFileWrite(&WorkTotal);
            /* send to enhanced kds, 2172 */
            ItemSendKds(&WorkTotal, 0);

			WorkTotal = ItemTenderLocal.ItemFSTotal;
			WorkTotal.lMI -= ItemTenderLocal.lFSTenderizer - ItemTenderLocal.ItemFSTender.lTenderAmount;
            TrnItem(&WorkTotal);
            MldScrollWrite(&WorkTotal, MLD_NEW_ITEMIZE);
            MldScrollFileWrite(&WorkTotal);
            /* send to enhanced kds, 2172 */
            ItemSendKds(&WorkTotal, 0);

            TrnItem(&ItemTenderLocal.ItemFSTender);
            MldScrollWrite(&ItemTenderLocal.ItemFSTender, MLD_NEW_ITEMIZE);
            MldScrollFileWrite(&ItemTenderLocal.ItemFSTender);
            /* send to enhanced kds, 2172 */
            ItemSendKds(&ItemTenderLocal.ItemFSTender, 0);
        }
        memset(&ItemTenderLocal.ItemFSTotal, 0, sizeof(ITEMTOTAL));
        memset(&ItemTenderLocal.ItemFSTender, 0, sizeof(ITEMTENDER));

		// ----
		// following code was added for Amtrak which uses food stamp for Train Delay Voucher
		// so that speciality items such as alcoholic drinks can not be used with a voucher.
		// it was also following the ending brace of the if.
		// changed added May-29-2013.
		//
		// replace this check for Tender Key #2, normally used for Food Stamps, with a check
		// for partial tender so that if the transaction is a combination of food stamp and non-food stamp
		// which is taxable, we will get the taxable portion on our receipt.
		// also moved from after the ending brace of the if statement to here to eliminated
		// repeated Total lines on printed receipt.
		//       note added Richard Chambers, Feb-20-2018
		//	if (uchTenderMinorClass != CLASS_TENDER2) {
		if (ItemTenderLocal.fbTenderStatus[1] & TENDER_FS_PARTIAL) {/* FS Tender */
			if (ItemTenderLocal.ItemTotal.uchMajorClass == 0        /* w/o amout tender */
				&& ! (ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL)) {
				ItemTendTotal();
			}
			ItemTendTax();                                          /* tax */
			ItemTendSendTotal();
			ItemPreviousItem (0, 0 );                     /* Saratoga, Dec/5/2000 */
		}
		// ----
    }

}

/*
*==========================================================================
**  Synopsis:   VOID    ItemTendFSTotal(ITEMTOTAL *pItem)
*
*   Input:      ITEMTOTAL   *ItemTotal
*   Output:     ITEMTOTAL   *ItemTotal
*   InOut:      none
*
*   Return:
*
*   Description:    calculate sales total (MI + tax)
*==========================================================================
*/
VOID    ItemTendFSTotal(ITEMTOTAL *pItem)
{
    DCURRENCY   lMI = 0L;

    pItem->uchMajorClass = CLASS_ITEMTOTAL;
    pItem->uchMinorClass = CLASS_TOTAL2_FS;
    pItem->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
    ItemTendAddTaxSum(&lMI);
    pItem->lMI           = TranItemizersPtr->lMI + lMI;
}

/*
*==========================================================================
**  Synopsis:   LONG    ItemTendAddTaxSum(LONG *plTax)
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:     LONG tax sum
*
*   Description:    calculate tax sum ( in ItemTenderLocal )
*==========================================================================
*/
VOID    ItemTendAddTaxSum(DCURRENCY *plTax)
{
	ITEMAFFECTION   Affect = {0};

    if (TranGCFQualPtr->usGuestNo == 0) {
        ItemTotalCalTax(&Affect, 0);
    } else {
        ItemTotalCalTax(&Affect, 1);
    }

    /* WIC Transaction   R2.1 */
    if ((TranCurQualPtr->fsCurStatus & CURQUAL_WIC)) {
		SHORT  i;

        for (i = 0; i < STD_TAX_ITEMIZERS_US; i++) {
            Affect.USCanVAT.USCanTax.lTaxExempt[i] = Affect.USCanVAT.USCanTax.lTaxable[i];
            Affect.USCanVAT.USCanTax.lTaxAmount[i] = Affect.USCanVAT.USCanTax.lTaxable[i] = 0L;
        }
    }

    if (!(TranGCFQualPtr->fsGCFStatus & GCFQUAL_USEXEMPT)) {
		SHORT  i;

        for (i = 0; i < STD_TAX_ITEMIZERS_US; i++) {
            *plTax += Affect.USCanVAT.USCanTax.lTaxAmount[i];
        }
    }
}

/*
*==========================================================================
**  Synopsis:   VOID    ItemTendTotal(VOID)
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    generate total class
*==========================================================================
*/
VOID    ItemTendTotal(VOID)
{
    DCURRENCY       lRound = 0;
    UIFREGTENDER    UifTender = {0};
	ITEMTOTAL       ItemTotal = {0};

    ItemTotal.uchMajorClass = CLASS_ITEMTOTAL;
    ItemTotal.uchMinorClass = CLASS_TOTAL2_FS;

    ItemTendTotalTax(&ItemTotal);
    ItemTendCalFSAmount(&lRound);

    ItemTotal.lMI = lRound;
    ItemTotal.fsPrintStatus = PRT_JOURNAL | PRT_SLIP;

    ItemTotal.ulID = (TranCurQualPtr->auchTotalStatus[0] % CHECK_TOTAL_TYPE);  // ITM_TTL_FLAG_TOTAL_KEY()

    ItemTenderLocal.ItemTotal = ItemTotal;
}

/*
*==========================================================================
**  Synopsis:   VOID    ItemTendTotalTax(ITEMTOTAL *pItem)
*
*   Input:      ITEMTOTAL    *ItemTotal
*   Output:     ITEMTOTAL   *ItemTotal
*   InOut:      none
*
*   Return:
*
*   Description:    calculate tax (auto)
*=========================================================================
*/
VOID    ItemTendTotalTax(ITEMTOTAL *pItem)
{
	ITEMAFFECTION       Affect = {0};

    if (TranGCFQualPtr->usGuestNo == 0) {
        ItemTotalCalTax(&Affect, 0);
    } else {
        ItemTotalCalTax(&Affect, 1);
    }

    if (TranCurQualPtr->fsCurStatus & CURQUAL_WIC) {
		SHORT   i;

        for (i = 0; i < STD_TAX_ITEMIZERS_US; i++) {
            ItemTenderLocal.lWorkExempt[i]  = Affect.USCanVAT.USCanTax.lTaxable[i];
            ItemTenderLocal.lWorkTaxable[i] = ItemTenderLocal.lWorkTax[i] = ItemTenderLocal.lWorkFSExempt[i] = 0L;
        }
    } else {
        if (!(TranGCFQualPtr->fsGCFStatus & GCFQUAL_USEXEMPT)) {
			SHORT   i;

            for (i = 0; i < STD_TAX_ITEMIZERS_US; i++) {
                ItemTenderLocal.lWorkTaxable[i]  = Affect.USCanVAT.USCanTax.lTaxable[i];
                ItemTenderLocal.lWorkTax[i]      = Affect.USCanVAT.USCanTax.lTaxAmount[i];
                ItemTenderLocal.lWorkExempt[i]   = Affect.USCanVAT.USCanTax.lTaxExempt[i];
                ItemTenderLocal.lWorkFSExempt[i] = Affect.USCanVAT.USCanTax.lFSExempt[i];
                pItem->lMI += ItemTenderLocal.lWorkTax[i];
            }
        } else {
			SHORT   i;

            for (i = 0; i < STD_TAX_ITEMIZERS_US; i++) {
                ItemTenderLocal.lWorkExempt[i]   = Affect.USCanVAT.USCanTax.lTaxable[i];
                ItemTenderLocal.lWorkFSExempt[i] = Affect.USCanVAT.USCanTax.lTaxable[i];
                ItemTenderLocal.lWorkTaxable[i]  = 0L;
                ItemTenderLocal.lWorkTax[i]      = 0L;
            }
        }
    }
}

/*
*==========================================================================
**  Synopsis:   VOID    ItemTendTax(VOID)
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    generate tax class
*==========================================================================
*/
VOID    ItemTendTax(VOID)
{
    if ((ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL) == 0) {
		ITEMAFFECTION   Affect = {0};

		Affect.uchMajorClass = CLASS_ITEMAFFECTION;
        Affect.uchMinorClass = CLASS_TAXAFFECT;
        Affect.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;

        if (TranGCFQualPtr->usGuestNo == 0) {
            ItemTotalCalTax(&Affect, 0);
        } else {
            ItemTotalCalTax(&Affect, 1);
        }

        if (TranGCFQualPtr->fsGCFStatus & GCFQUAL_USEXEMPT) {
			SHORT   i;

            for (i = 0; i < STD_TAX_ITEMIZERS_US; i++) {
                Affect.USCanVAT.USCanTax.lTaxExempt[i] = Affect.USCanVAT.USCanTax.lTaxable[i];
                Affect.USCanVAT.USCanTax.lTaxable[i]   = 0L;
                Affect.USCanVAT.USCanTax.lTaxAmount[i] = 0L;
            }
        }

        TrnAffection (&Affect);

        /* --- Dec/6/2000 --- */
        if (CliParaMDCCheck(MDC_TAX2_ADR, EVEN_MDC_BIT0) == 0) {
			TRANITEMIZERS   *WorkTITax = TrnGetTIPtr();

            WorkTITax->lHourly += Affect.USCanVAT.USCanTax.lTaxAmount[0] 
                + Affect.USCanVAT.USCanTax.lTaxAmount[1] + Affect.USCanVAT.USCanTax.lTaxAmount[2];
        }
        /* --- Dec/6/2000 --- */

        /* send to enhanced kds, 2172 */
        ItemSendKds(&Affect, 0);

        Affect.uchMinorClass = CLASS_TAXPRINT;
		{
			TRANITEMIZERS   *WorkTI = TrnGetTIPtr();
			SHORT   i;

			for (i = 0; i < STD_TAX_ITEMIZERS_US; i++) {
				Affect.USCanVAT.USCanTax.lTaxable[i] += WorkTI->Itemizers.TranUsItemizers.lAffectTaxable[i];
				Affect.USCanVAT.USCanTax.lTaxAmount[i] += WorkTI->Itemizers.TranUsItemizers.lTaxItemizer[i];
				Affect.USCanVAT.USCanTax.lTaxExempt[i] += WorkTI->Itemizers.TranUsItemizers.lAffectTaxExempt[i];
			}
		}
        TrnAffection (&Affect);
        MldScrollWrite(&Affect, MLD_NEW_ITEMIZE);
        MldScrollFileWrite(&Affect);
    }
}

/*
*==========================================================================
**  Synopsis:   VOID    ItemTendSendTotal(VOID)
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    send total class
*==========================================================================
*/
VOID    ItemTendSendTotal(VOID)
{
    if ((ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL) == 0) {
        if (ItemTenderLocal.ItemTotal.uchMajorClass != 0) {
            TrnItem(&ItemTenderLocal.ItemTotal);
            MldScrollWrite(&ItemTenderLocal.ItemTotal, MLD_NEW_ITEMIZE);
            MldScrollFileWrite(&ItemTenderLocal.ItemTotal);

            /* send to enhanced kds, 2172 */
            ItemSendKds(&ItemTenderLocal.ItemTotal, 0);

            memset(&ItemTenderLocal.ItemTotal, 0, sizeof(ITEMTOTAL));
            memset(&ItemTenderLocal.ItemFSTotal, 0, sizeof(ITEMTOTAL));
        }
    }
}

/*
*==========================================================================
**  Synopsis:   SHORT   ItemTendECFS(VOID)
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    E/C (food stamp tender)
*==========================================================================
*/
SHORT   ItemTendECFS(VOID)
{
    SHORT           sStatus;

    /* --- Second Food Stamp Tender, V2.01.04 --- */
    if (ItemTenderLocal.fbTenderStatus[1] & TENDER_FS_PARTIAL2) {
		ITEMCOMMONLOCAL  *WorkCommon = ItemCommonGetLocalPointer();
		ITEMTENDER       WorkTendLocal;

        /* --- get previous item data --- */
        memcpy(&WorkTendLocal, &WorkCommon->ItemSales, sizeof(WorkTendLocal));
        WorkTendLocal.lTenderAmount = ItemTenderLocal.lFSTenderizer2;   /* F.S amount */

        if ((sStatus = ItemTendCPECTend(&WorkTendLocal)) != ITM_SUCCESS) {
            return(sStatus);
        }

        WorkTendLocal.lTenderAmount *= -1L;
        WorkTendLocal.fsPrintStatus = PRT_JOURNAL | PRT_SLIP;
        WorkTendLocal.fbStorageStatus |= NOT_SEND_KDS;
        TrnItem(&WorkTendLocal);

        /* send partial tender data to KDS directory R3.1 */
        WorkTendLocal.fbStorageStatus &= ~NOT_SEND_KDS;
        TrnThroughDisp(&WorkTendLocal);

        /* send to enhanced kds, 2172 */
        ItemSendKds(&WorkTendLocal, 1);
    }

	{
		REGDISPMSG      Disp = {0};
		UIFREGTENDER    UifTender = {0};
		SHORT           i;

		/* --- FS Total --- */
		UifTender.uchMinorClass = (CLASS_UITENDER1 + (ItemTenderLocal.ItemFSTender.uchMinorClass - CLASS_TENDER1));

		/* --- Second Food Stamp Tender, V2.01.05 --- */
		if (ItemTenderLocal.fbTenderStatus[1] & TENDER_FS_PARTIAL2) {
			ITEMTENDER      WorkTend = {0};

			/* for E/C validation */
			WorkTend = ItemTenderLocal.ItemFSTender;
			WorkTend.lTenderAmount = ItemTenderLocal.lFSTenderizer * -1L;

			/* recover tender #2 data */
			WorkTend.uchMinorClass = CLASS_TENDER2;
			WorkTend.lTenderAmount += ItemTenderLocal.lFSTenderizer2;   /* F.S amount */

			if (WorkTend.lTenderAmount) {
				WorkTend.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
				TrnItem(&WorkTend);
			}
		}

		ItemTenderLocal.lFSTenderizer = 0L;
		ItemTenderLocal.lFS           = 0L;
		ItemTenderLocal.lFSChange     = 0L;
		ItemTenderLocal.lCashChange   = 0L;
		ItemTenderLocal.lFSTenderizer2 = 0L;
		for (i = 0; i < STD_TAX_ITEMIZERS_MAX; i++) {
			ItemTenderLocal.lWorkTaxable[i]  = 0L;
			ItemTenderLocal.lWorkTax[i]      = 0L;
			ItemTenderLocal.lWorkExempt[i]   = 0L;
			ItemTenderLocal.lWorkFSExempt[i] = 0L;
		}

		ItemTenderLocal.fbTenderStatus[1] &= ~(TENDER_FS_PARTIAL | TENDER_FS_PARTIAL2 | TENDER_FS_TOTAL);

		/* --- proceed non-food partial tender --- */
		if (! CliParaMDCCheckField(MDC_FS1_ADR, MDC_PARAM_BIT_D)) {
			memset(&ItemTenderLocal.ItemTotal, 0, sizeof(ITEMTOTAL));
		}
		memset(&ItemTenderLocal.ItemFSTotal, 0, sizeof(ITEMTOTAL));
		memset(&ItemTenderLocal.ItemFSTender, 0, sizeof(ITEMTENDER));


		Disp.uchMajorClass = CLASS_UIFREGTENDER;
		Disp.uchMinorClass = CLASS_UITENDER1;
		RflGetLead (Disp.aszMnemonic, LDT_BD_ADR);
		ItemTendCalFSAmount(&Disp.lAmount);        /* MI + tax */
		Disp.fbSaveControl = 5;                /* save for recovery, DISP_SAVE_TYPE_5 */

		flDispRegDescrControl |= TENDER_CNTRL;
		flDispRegKeepControl |= TENDER_CNTRL;
		flDispRegDescrControl &= ~FOODSTAMP_CTL;
		flDispRegKeepControl &= ~FOODSTAMP_CTL;
		DispWrite(&Disp);
	}
    return(ITM_SUCCESS);
}


/*
*==========================================================================
**  Synopsis:   SHORT   ItemTendFS(UIFREGTENDER *pUifTender)
*
*   Input:      UIFREGTENDER    *UifRegTender
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    Food Stamp Tender Main
*                   Generate Food Stamp and Food Stamp credit data to determine
*                   the type of change to provide along with how much change.
*                   Some of this is source to support the older Food Stamp
*                   program which used coupons in various denominations ($1, $5, etc.)
*                   however the modern Supplemental Nutrition Assissance Program
*                   or SNAP which replaced the Food Stamp program in 2008
*                   uses a kind of debit card which was slowly phased induring 1990s.
*                      see http://en.wikipedia.org/wiki/Electronic_Benefit_Transfer
*
*                   To use the modern SNAP debit card, Electronic Payment must be
*                   turned on for the tender key with the necessary provisioning for
*                   the Electronic Payment server of the EBT processor.
*
*                   For a description of the older Food Stamp program before SNAP
*                   look at the National Bureau of Economic Research Working Paper No. 1231
*                   issued in November 1983 by Daniel S. Hamermesh and James M. Johannes,
*                   Food Stamps as Money and Income.
*                     http://www.nber.org/papers/w1231
*==========================================================================
*/
SHORT   ItemTendFS(UIFREGTENDER *UifRegTender)
{
    SHORT           sStatus;
	ITEMTENDER      ItemTender = {0};

    ItemTendGetMDC(UifRegTender->uchMinorClass, &ItemTender.auchTendStatus[0]);
    if ((sStatus = ItemTendFSCheck(UifRegTender, &ItemTender)) != UIF_SUCCESS) {
        return(sStatus);
    }

    if ((sStatus = ItemTendPartSPVInt(UifRegTender)) != UIF_SUCCESS) {
        return(sStatus);
    }

	{
		ULONG   ulfsCurStatus = 0;
		ULONG   fbModifier = TrnQualModifierReturnTypeTest ();

		// Indicate the returns type so that totals will be updated properly
		ItemTender.fbModifier |= (fbModifier & (RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3));
		UifRegTender->fbModifier |= (fbModifier & (RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3));

		if ((ulfsCurStatus = TrnQualModifierReturnData (0, 0)) & (CURQUAL_PRETURN | CURQUAL_TRETURN)) {
			ItemTender.fbReduceStatus |= REDUCE_RETURNED;
		}

		if (TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)) {
			if (TranItemizersPtr->lMI < 0) {
				SHORT   sGusRetValue;

				ItemTender.uchMajorClass = CLASS_ITEMTENDER;                   /* major class */
				ItemTender.uchMinorClass = UifRegTender->uchMinorClass;        /* minor class */
				if (0 > (sGusRetValue = CliGusLoadTenderTableLookupAndCheckRules (&ItemTender))) {
					return CliGusLoadTenderTableConvertError (sGusRetValue);
				}
			}
		}
	}
	{
		DATE_TIME  TenderDateTime;

		PifGetDateTime (&TenderDateTime);

		ItemTender.uchTenderYear = (UCHAR)(TenderDateTime.usYear % 100);
		ItemTender.uchTenderMonth = (UCHAR)TenderDateTime.usMonth;
		ItemTender.uchTenderDay = (UCHAR)TenderDateTime.usMDay;
		ItemTender.uchTenderHour = (UCHAR)TenderDateTime.usHour;
		ItemTender.uchTenderMinute = (UCHAR)TenderDateTime.usMin;
		ItemTender.uchTenderSecond = (UCHAR)TenderDateTime.usSec;
	}

	ItemTender.usCheckTenderId = ItemTenderLocalCheckTenderid (3, 0);

	/* --- Charge, EPT, Account#/CDV/NCF --- */
    sStatus = ItemTendCP(UifRegTender, &ItemTender);
    if (sStatus == UIF_CANCEL) {
        ItemTendCPCancel();
    }

    if (sStatus == UIF_CANCEL || sStatus > 0) {
        return(sStatus);
    }
    if (sStatus == ITM_PARTTEND || sStatus == ITM_OVERTEND) {
        UifRegTender->lTenderAmount = RflLLabs(ItemTender.lTenderAmount);
    }
    ItemTender.lTenderAmount = 0L;
    ItemTender.lBalanceDue   = 0L;
    ItemTender.lChange       = 0L;

    switch (ItemTendFSAmount(UifRegTender, &ItemTender)) {  /* FS state */
		case    1:
			/* --- FS partial tender --- */
			ItemTendFSPart(UifRegTender, &ItemTender);
			sStatus = (ITM_SUCCESS); 
			break;

		case    2:
			/* --- proceed non-food partial tender --- */
			sStatus = ItemTendFSNonFoodPart(UifRegTender, &ItemTender);
			break;

		/* --- FS over tender --- */
		case    4:
			sStatus = ItemTendFSOver(UifRegTender, &ItemTender);
			break;

		default:
			return(LDT_SEQERR_ADR);
    }

	switch (sStatus) {
		case UIF_SUCCESS:                               /* success */
		case UIF_FS2NORMAL:								// partial tender with remaining non-food stamp items.
		case UIF_FINALIZE:                              /* finalize */
		case UIF_CAS_SIGNOUT:                           /* cashier sign out */
		case UIF_WAI_SIGNOUT:                           /* waiter sign out */
			{
				ItemTenderLocalCheckTenderid (1, 0);
				CliGusLoadTenderTableAdd (&ItemTender);
			}
			break;
	}

	return sStatus;
}

/*
*==========================================================================
**  Synopsis:   SHORT   ItemTendFSCheck(UIFREGTENDER *pUifTender)
*
*   Input:      UIFREGTENDER    *UifRegTender
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    check F.S tender
*==========================================================================
*/
static SHORT   ItemTendFSCheck(UIFREGTENDER *pUifTender, ITEMTENDER *pItemTender)
{
    TRANITEMIZERS   *WorkTI = TrnGetTIPtr();
    TRANGCFQUAL     *WorkGCF = TrnGetGCFQualPtr();
    DCURRENCY       lMI = 0, lFMI = 0, lTax = 0;
    PARATRANSHALO   WorkHALO;
	ITEMAFFECTION   Affect = {0};
    SHORT           i;

	if (!CliParaMDCCheckField(MDC_FS2_ADR, MDC_PARAM_BIT_B)) {
		// this is a normal food stamp tender so we need to enforce no voids
		if (pUifTender->fbModifier & VOID_MODIFIER) {
			return(LDT_SEQERR_ADR);
		}
	}

    if (ItemTenderLocal.ItemFSTotal.uchMajorClass == 0) {   /* no F.S total key */
        return(LDT_SEQERR_ADR);
    }

	/* 07/25/01 */
    if ((ItemTenderLocal.fbTenderStatus[1] & TENDER_FS_TOTAL) == 0) {
		NHPOS_NONASSERT_NOTE ("==NOTE", "==NOTE: Food Stamp Tender without Food Stamp Total.");
       	return(LDT_SEQERR_ADR);
    }

    if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) != CURQUAL_STORE_RECALL) {
        if (WorkGCF->usGuestNo != 0 ||
            (TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_ADDCHECK) {
            return(LDT_SEQERR_ADR);
        }
    }

    if (ItemTenderLocal.fbTenderStatus[2] & (TENDER_FC1 | TENDER_FC2 | TENDER_FC3 |
        TENDER_FC4 | TENDER_FC5 | TENDER_FC6 | TENDER_FC7 | TENDER_FC8)) {
        return(LDT_SEQERR_ADR);
    }

    if (ItemModLocalPtr->fsTaxable != 0 || (ItemModLocalPtr->fbModifierStatus & MOD_FSMOD)) {
		/* sequence error if tax modifier key has been used */
        return(LDT_SEQERR_ADR);
    }

    /*---- Second Food Stamp Tender, V2.01.05 ----*/
    WorkHALO.uchMajorClass = CLASS_PARATRANSHALO;
    if (pUifTender->uchMinorClass == CLASS_UITENDER2) {
        WorkHALO.uchAddress = HALO_TEND2_ADR;
    } else {
        WorkHALO.uchAddress = HALO_TEND11_ADR;
    }
    CliParaRead(&WorkHALO);
    if (RflHALO(pUifTender->lTenderAmount, WorkHALO.uchHALO) != RFL_SUCCESS) {
        return(LDT_KEYOVER_ADR);
    }

    if (WorkGCF->usGuestNo == 0) {
        ItemTotalCalTax(&Affect, 0);
    } else {
        ItemTotalCalTax(&Affect, 1);
    }

	lTax = 0;
    for (i = 0; i < STD_TAX_ITEMIZERS_US; i++) {
        lTax += Affect.USCanVAT.USCanTax.lTaxAmount[i] - WorkTI->Itemizers.TranUsItemizers.lTaxItemizer[i];    /* for Store/Recall */
    }

    lMI = WorkTI->lMI + lTax;
    lFMI = WorkTI->Itemizers.TranUsItemizers.lFoodStampable;

    if (TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)) {
        lMI *= -1L;
        lFMI *= -1L;
    }

	if (!CliParaMDCCheckField(MDC_FS2_ADR, MDC_PARAM_BIT_B)) {
		// this is a normal food stamp tender so we need to enforce no voids
		if (lMI <= 0L || lFMI <= 0L) {                  /* illegal case */
			return(LDT_PROHBT_ADR);
		}
	}

    /*---- Second Food Stamp Tender, V2.01.05 ----*/
    if (ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL) {   /* partial tender */
        return(LDT_SEQERR_ADR);
    }

    /*---- Second Food Stamp Tender, V2.01.04 ----*/
    if (pUifTender->fbModifier & OFFCPTEND_MODIF) {
		if (pUifTender->uchMinorClass == CLASS_TENDER2) {		
			if (!(CliParaMDCCheckField(MDC_TEND23_ADR, MDC_PARAM_BIT_D))) {
				return(LDT_SEQERR_ADR);
			}
		} else {
			if (!(CliParaMDCCheckField(MDC_TEND113_ADR, MDC_PARAM_BIT_D))) {
				return(LDT_PROHBT_ADR);
			}
		}
	}

    /* amount check of second food stamp tender */
	{
		UCHAR   auchStatus[4] = {0};

		ItemTendGetMDC(pUifTender->uchMinorClass, &auchStatus[0]);
		if (pUifTender->lTenderAmount == 0L) {
			if (((auchStatus[0] & ODD_MDC_BIT0) != 0) && ((auchStatus[0] & ODD_MDC_BIT1) == 0)) {
				return(LDT_AMT_ENTRY_REQUIRED_ADR);                         /* prohibit fast tender */
			}

			/* allow fast finalize for CPM/EPT */
			if (RflLLabs(WorkTI->Itemizers.TranUsItemizers.lFoodStampable) <= RflLLabs(ItemTenderLocal.lFSTenderizer)) {
				return(LDT_SEQERR_ADR);
			}

			if (!CliParaMDCCheckField(MDC_FS2_ADR, MDC_PARAM_BIT_B)) {
				pUifTender->lTenderAmount = RflLLabs(WorkTI->Itemizers.TranUsItemizers.lFoodStampable - ItemTenderLocal.lFSTenderizer);
			} else {
				pUifTender->lTenderAmount = WorkTI->Itemizers.TranUsItemizers.lFoodStampable - ItemTenderLocal.lFSTenderizer;
			}
		} else {
			/* amount entry */
			if (RflLLabs(WorkTI->Itemizers.TranUsItemizers.lFoodStampable) <= RflLLabs(ItemTenderLocal.lFSTenderizer)) {
				/* food stamp tender had already completed */
				return(LDT_PROHBT_ADR);
			}
			/* check amount entry option of second food stamp tender */
			if ((auchStatus[0] & ODD_MDC_BIT0) && (auchStatus[0] & ODD_MDC_BIT1)) {            /* prohibit amount */
				return(LDT_PROHBT_ADR);
			}
			if (RflLLabs(WorkTI->Itemizers.TranUsItemizers.lFoodStampable) < RflLLabs(ItemTenderLocal.lFSTenderizer) + RflLLabs(pUifTender->lTenderAmount) )  {
				if (auchStatus[0] & ODD_MDC_BIT2) {				/* prohibit over amount entry */
					return(LDT_KEYOVER_ADR);
				}
			} else if (RflLLabs(WorkTI->Itemizers.TranUsItemizers.lFoodStampable) > RflLLabs(ItemTenderLocal.lFSTenderizer) + RflLLabs(pUifTender->lTenderAmount)) {
				if (auchStatus[0] & ODD_MDC_BIT3) {             /* prohibit partial tender */
					return(LDT_PROHBT_ADR);
				}
			}
		}
	}

    /* prohibit duplicated second partial tender by FVT comment */
    if (ItemTenderLocal.fbTenderStatus[1] & TENDER_FS_PARTIAL2) {
        return(LDT_PROHBT_ADR);
    }

	pItemTender->lRound = lMI;    // needed to pass non-zero check in ItemTendEPTEx() to prevent zero tender EPT for Amtrak

    /* set tender amount for cpm/ept, V2.01.04 */
    pItemTender->lTenderAmount = RflLLabs(pUifTender->lTenderAmount);
    pItemTender->lTenderAmount *= ItemTendDetermineSignFS();

    return(UIF_SUCCESS);
}


/*
==========================================================================
**  Synopsis:   ItemTendCPCancel(VOID)
*   Input:
*   Output:
*   InOut:      none
*   Return:
*   Description:    Recover Display
==========================================================================
*/
VOID    ItemTendCPCancel(VOID)
{
	REGDISPMSG       Disp = {0};

    Disp.uchMajorClass  = CLASS_UIFREGTOTAL;
    Disp.uchMinorClass  = CLASS_UITOTAL9;
    Disp.lAmount        = ItemTenderLocal.ItemFSTotal.lMI;
    RflGetTranMnem (Disp.aszMnemonic, TRN_TTL9_ADR);

    flDispRegDescrControl |= TOTAL_CNTRL;
    Disp.fbSaveControl = DISP_SAVE_TYPE_1;
    DispWrite(&Disp);
}

/*
*==========================================================================
**   Synopsis:  SHORT   ItemTendFSAmount(UIFREGTENDER *pUifTender, ITEMTENDER *pItem)
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender
*   Output:     ITEMTENDER  *ItemTender
*   InOut:      none
*
*   Return:
*
*   Description:    generate various Food Stamp tender data based on the
*                   current transaction amount, tender amount entered by
*                   the Operator, and taxes.
*==========================================================================
*/
static SHORT   ItemTendFSAmount(UIFREGTENDER *pUifTender, ITEMTENDER *pItem)
{
    TRANITEMIZERS   *WorkTI = TrnGetTIPtr();
    DCURRENCY       lTax = 0L;
    SHORT           sSign = ItemTendDetermineSignFS();
	SHORT           sRetState = -1;   // return state indicating final handling of the tender such as partial, etc.
	ITEMAFFECTION   Affect = {0};

    ItemTenderLocal.fbTenderStatus[1] |= TENDER_FS_PARTIAL;
    ItemTenderLocal.lFSTenderizer += pUifTender->lTenderAmount * sSign;

    if (TranGCFQualPtr->usGuestNo == 0) {
        ItemTotalCalTax(&Affect, 0);
    } else {
        ItemTotalCalTax(&Affect, 1);
    }

	{
		SHORT  i;

		lTax = 0L;
		for (i = 0; i < STD_TAX_ITEMIZERS_US; i++) {
			lTax += Affect.USCanVAT.USCanTax.lTaxAmount[i];
		}
	}

    pItem->uchMajorClass = CLASS_ITEMTENDER;
    pItem->uchMinorClass = pUifTender->uchMinorClass;
	pItem->lTenderAmount = pUifTender->lTenderAmount;
    pItem->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;

    if ((WorkTI->lMI + lTax) * sSign > WorkTI->Itemizers.TranUsItemizers.lFoodStampable * sSign) {
		/* --- MI > FMI --- */
        /* --- FMI > FS tender (FS partial) --- */
        if (WorkTI->Itemizers.TranUsItemizers.lFoodStampable * sSign > ItemTenderLocal.lFSTenderizer * sSign) {
            pItem->lBalanceDue = WorkTI->Itemizers.TranUsItemizers.lFoodStampable - ItemTenderLocal.lFSTenderizer;
            /* FVT, V2.01.05 */
            /* compulsory normal tender after second food stamp tender */
            sRetState = 1;                                              /* partial tender state */
            if (pUifTender->uchMinorClass != CLASS_UITENDER2) {
                if (! CliParaMDCCheck(MDC_FS1_ADR, ODD_MDC_BIT0)) { /* proceed non-food partial tender */
                    pItem->lBalanceDue = WorkTI->lMI + lTax - ItemTenderLocal.lFSTenderizer;
					sRetState = 2;                                      /* partial tender state */
                }
            }
        } else {
			/* --- FS tender (FS partial) >= FMI --- */
            ItemTendFSChange(lTax);
            sRetState = 4;                                      /* over tender state */
            if (! CliParaMDCCheck(MDC_FS1_ADR, ODD_MDC_BIT0)) {     /* proceed non-food partial tender */
                if ((WorkTI->lMI + lTax) * sSign > (ItemTenderLocal.lFSTenderizer - ItemTenderLocal.lFS - ItemTenderLocal.lFSChange) * sSign) {
                    pItem->lBalanceDue = WorkTI->lMI + lTax - ItemTenderLocal.lFSTenderizer + ItemTenderLocal.lFS + ItemTenderLocal.lFSChange;
                    sRetState = 2;                                      /* partial tender state */
                }
            } else {
				/* --- compulsory total key --- */
                if ((WorkTI->lMI + lTax) * sSign > (ItemTenderLocal.lFSTenderizer - ItemTenderLocal.lFS - ItemTenderLocal.lFSChange) * sSign) {
                    pItem->lBalanceDue = 0L;
                    sRetState = 1;                                      /* partial tender state */
                }
            }
        }
    } else {
		/* --- FMI >= MI --- */
        /* --- FMI > FS tender (FS partial) --- */
        sRetState = 4;                                          /* over tender state */
        if (WorkTI->Itemizers.TranUsItemizers.lFoodStampable * sSign > ItemTenderLocal.lFSTenderizer * sSign) {
            /* --- MI > FS tender --- */
            if ((WorkTI->lMI + lTax) * sSign > ItemTenderLocal.lFSTenderizer * sSign) {
                pItem->lBalanceDue = WorkTI->Itemizers.TranUsItemizers.lFoodStampable - ItemTenderLocal.lFSTenderizer;
                /* FVT, V2.01.05 */
                /* compulsory normal tender after second food stamp tender */
				/* TAR177387 */
				sRetState = 1;   /* default to partial tender state */
                if (pUifTender->uchMinorClass != CLASS_UITENDER2) {	/* 07/25/01 */
                /* if (pUifTender->uchMinorClass != CLASS_UITENDER11) { */
                    if (! CliParaMDCCheck(MDC_FS1_ADR, ODD_MDC_BIT0)) { /* proceed non-food partial tender */
                        pItem->lBalanceDue = WorkTI->lMI + lTax - ItemTenderLocal.lFSTenderizer;
                        sRetState = 2;                                  /* partial tender state */
                    }
                }
            } else {
				/* --- FS tender > MI --- */
                ItemTendFSChangeOver(WorkTI->lMI + lTax);
            }
        } else {
			/* --- FS tender > FMI --- */
            ItemTendFSChangeOver(WorkTI->lMI + lTax);
        }
    }
	ItemTenderLocal.ItemFSTender = *pItem;
	NHPOS_ASSERT_TEXT((sRetState > 0), "**WARNING: ItemTendFSAmount() invalid state.");
	return  sRetState;
}

/*
*==========================================================================
**  Synopsis:   VOID    ItemTendFSPart(UIFREGTENDER *pUifTender, ITEMTENDER *pItem)
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    Food Stamp partial tender
*==========================================================================
*/
VOID    ItemTendFSPart(UIFREGTENDER *pUifTender, ITEMTENDER *pItem)
{
    SHORT     sSign = 1;
    SHORT     sValPrintFlag = 0;  // validation print required flag
    USHORT    fsPrintStatus;      /* 51:print status */

    sSign = ItemTendDetermineSignFS();

    /* --- FS partial display --- */
    ItemTendFSDisp(pUifTender, pItem);

    pItem->uchMajorClass = CLASS_ITEMTENDER;
    pItem->uchMinorClass = pUifTender->uchMinorClass;
    pItem->fbModifier    = pUifTender->fbModifier;
	// if a food stamp tender amount is entered then use that amount for the
	// tender.  Otherwise use what is in the food stamp tenderizer.
	pItem->lTenderAmount = pUifTender->lTenderAmount;
	if (pItem->lTenderAmount == 0) {
		pItem->lTenderAmount = ItemTenderLocal.lFSTenderizer;
	}

    /* --- Validation --- */
    if (pItem->auchTendStatus[0] & ITEMTENDER_STATUS_0_VALIDATION) {
		ITEMTENDER      WorkTendLocal = *pItem;
        WorkTendLocal.fsPrintStatus = PRT_VALIDATION;
        WorkTendLocal.lTenderAmount = pUifTender->lTenderAmount * sSign;
        TrnThrough(&WorkTendLocal);
        sValPrintFlag = 1;
    }

    pItem->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;

    /* V2.01.04 */
    if (pItem->uchMinorClass == CLASS_TENDER2) {
        ItemTenderLocal.lFSTenderVal = pUifTender->lTenderAmount * sSign;
        ItemTenderLocal.ItemFSTender = *pItem;
		ItemTendSendFS();
    } else {
		/* --- Second Food Stamp Tender, V2.01.04 --- */
        ItemTenderLocal.lFSTenderVal = 0L;
        ItemTenderLocal.ItemFSTender.uchMajorClass = pItem->uchMajorClass;
        ItemTenderLocal.ItemFSTender.uchMinorClass = pItem->uchMinorClass;
        if (ItemTendCheckEPT(pUifTender) == ITM_EPT_TENDER) {
            ItemTendCPPrintRsp(pItem);
	    	if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_UNBUFFER) {
      			pItem->fsPrintStatus &= ~PRT_SLIP;             		/* avoid duplicated slip validation */
			} else {
        		/* execute slip validation for partial ept tender, 09/12/01 */
				if ((pItem->fsPrintStatus & PRT_SLIP) &&        /* Saratoga */
       				(pItem->uchCPLineNo != 0)) {                /* Saratoga */
					if (sValPrintFlag == 1) {
	    				if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) {	/* only slip validation, 03/23/01 */
    						if ( CliParaMDCCheck(MDC_SLIPVAL_ADR, EVEN_MDC_BIT0) ) { /* both validation and slip print */
								UieErrorMsg(LDT_VSLPREMV_ADR, UIE_WITHOUT);        /* Paper Change */
    						} else {
		        				UieErrorMsg(LDT_PAPERCHG_ADR, UIE_WITHOUT);        /* Paper Change */
							}
	    				}
					}
		       		fsPrintStatus = pItem->fsPrintStatus;
           			pItem->fsPrintStatus = PRT_SLIP;             			/* print slip/journal */
    				TrnThrough( pItem );
	      			pItem->fsPrintStatus = fsPrintStatus;
    	  			pItem->fsPrintStatus &= ~PRT_SLIP;             		/* avoid duplicated slip validation */
					sValPrintFlag = 1;	/* 06/22/01 */
	     		}
	     	}
       }

       if (ItemTenderLocal.ItemFSTotal.uchMajorClass != 0) {
            if (! (ItemTenderLocal.fbTenderStatus[1] & TENDER_FS_PARTIAL2)) {
				ITEMTENDER      WorkTendLocal = {0};

                TrnItem(&ItemTenderLocal.ItemFSTotal);

                /* print out first food stamp tender */
                WorkTendLocal.uchMajorClass = CLASS_ITEMTENDER;
                WorkTendLocal.uchMinorClass = CLASS_TENDER2;
                WorkTendLocal.lTenderAmount = ItemTenderLocal.lFSTenderizer;
                WorkTendLocal.lTenderAmount -= ItemTenderLocal.lFSTenderizer2;
                WorkTendLocal.lTenderAmount -= pUifTender->lTenderAmount * sSign;
                _tcsncpy(&WorkTendLocal.aszNumber[0], &ItemTenderLocal.ItemFSTender.aszNumber[0], /*sizeof(WorkTend.aszNumber)*/NUM_NUMBER);

                if (WorkTendLocal.lTenderAmount) {
                    WorkTendLocal.fsPrintStatus = pItem->fsPrintStatus;
                    TrnItem(&WorkTendLocal);
                    MldScrollWrite(&WorkTendLocal, MLD_NEW_ITEMIZE);
                    MldScrollFileWrite(&WorkTendLocal);

                    /* send to enhanced kds, 2172 */
                    ItemSendKds(&WorkTendLocal, 0);

                }
            }
        }
	   {
			ITEMTENDER      WorkTendLocal = *pItem;
			WorkTendLocal.lTenderAmount = pUifTender->lTenderAmount * sSign;
			WorkTendLocal.fbStorageStatus |= NOT_SEND_KDS;
			TrnItem(&WorkTendLocal);

			/* send partial tender data to KDS directory R3.1 */
			WorkTendLocal.fbStorageStatus &= ~NOT_SEND_KDS;
			TrnThroughDisp(&WorkTendLocal);

			MldScrollWrite(&WorkTendLocal, MLD_NEW_ITEMIZE);
			MldScrollFileWrite(&WorkTendLocal);

			/* send to enhanced kds, 2172 */
			ItemSendKds(&WorkTendLocal, 0);

			ItemTenderLocal.lFSTenderizer2 += WorkTendLocal.lTenderAmount;
			ItemTenderLocal.fbTenderStatus[1] |= TENDER_FS_PARTIAL2;
	   }
    }

	ItemTenderLocal.fbTenderStatus[1] &= ~(TENDER_FS_TOTAL);
}

/*
*==========================================================================
**  Synopsis:   VOID    ItemTendFSDisp(UIFREGTENDER *pUifTender, ITEMTENDER *pItem)
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    display of F.S partial tender
*==========================================================================
*/
VOID    ItemTendFSDisp(UIFREGTENDER *pUifTender, ITEMTENDER *pItem)
{
	REGDISPMSG      Disp = {0};
    ITEMTOTAL       ItemTotal = {0};

    Disp.uchMajorClass  = pUifTender->uchMajorClass;
    Disp.uchMinorClass  = CLASS_UITENDER1;
    Disp.lAmount        = pItem->lBalanceDue;

    flDispRegDescrControl |= TENDER_CNTRL | FOODSTAMP_CTL;
    flDispRegKeepControl  |= TENDER_CNTRL | FOODSTAMP_CTL;
    Disp.fbSaveControl     = 1;
    flDispRegDescrControl &= ~(TENDER_CNTRL | FOODSTAMP_CTL);
    flDispRegKeepControl  &= ~(TENDER_CNTRL | FOODSTAMP_CTL);
    DispWrite(&Disp);

    /* send to enhanced kds, 2172 */
    ItemTotal.uchMajorClass = CLASS_ITEMTOTAL;
    ItemTotal.uchMinorClass = CLASS_TOTAL9;
    ItemTotal.lMI = pItem->lBalanceDue;
    TrnThroughDisp(&ItemTotal);
    ItemSendKds(&ItemTotal, 0);
}

/*
*==========================================================================
**  Synopsis:   SHORT   ItemTendFSNonFoodPart(UIFREGTENDER *pUifgTender, ITEMTENDER *pItem)
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    F.S partial tender
==========================================================================
*/
SHORT   ItemTendFSNonFoodPart(UIFREGTENDER *pUifTender, ITEMTENDER *pItem)
{
    SHORT           sSign = 1;
    SHORT           sValPrintFlag = 0;  // validation print required flag
    USHORT          fsPrintStatus;                  /* 51:print status */

    sSign = ItemTendDetermineSignFS();

    /* --- Normal Partial Tender Display --- */
    ItemTendPartDisp(pUifTender, pItem);

    pItem->uchMajorClass = CLASS_ITEMTENDER;
    pItem->uchMinorClass = pUifTender->uchMinorClass;
    pItem->fbModifier    = pUifTender->fbModifier;
	// if a food stamp tender amount is entered then use that amount for the
	// tender.  Otherwise use what is in the food stamp tenderizer.
	pItem->lTenderAmount = pUifTender->lTenderAmount;
	if (pItem->lTenderAmount == 0) {
		pItem->lTenderAmount = ItemTenderLocal.lFSTenderizer;
	}

    /* --- Validation --- */
    if (pItem->auchTendStatus[0] & ITEMTENDER_STATUS_0_VALIDATION) {
		ITEMTENDER      WorkTend = *pItem;

        WorkTend.fsPrintStatus = PRT_VALIDATION;
        WorkTend.lTenderAmount = pUifTender->lTenderAmount * sSign;
        TrnThrough(&WorkTend);
        sValPrintFlag = 1;
    }

    pItem->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;

    /* V2.01.04 */
    if (pItem->uchMinorClass == CLASS_TENDER2) {
        ItemTenderLocal.ItemFSTender = *pItem;
    } else {
        /* for fs total line print control */
        ItemTenderLocal.ItemFSTender.uchMajorClass = pItem->uchMajorClass;
        ItemTenderLocal.ItemFSTender.uchMinorClass = pItem->uchMinorClass;
        if (ItemTendCheckEPT(pUifTender) == ITM_EPT_TENDER) {
            ItemTendCPPrintRsp(pItem);
	    	if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_UNBUFFER) {
      			pItem->fsPrintStatus &= ~PRT_SLIP;             		/* avoid duplicated slip validation */
			} else {
        	/* execute slip validation for partial ept tender, 09/12/01 */
	        if ((pItem->fsPrintStatus & PRT_SLIP) &&        /* Saratoga */
       		    (pItem->uchCPLineNo != 0)) {                /* Saratoga */
		        if (sValPrintFlag == 1) {
	    		    if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) {	/* only slip validation, 03/23/01 */
    					if ( CliParaMDCCheck(MDC_SLIPVAL_ADR, EVEN_MDC_BIT0) ) { /* both validation and slip print */
					        UieErrorMsg(LDT_VSLPREMV_ADR, UIE_WITHOUT);        /* Paper Change */
    					} else {
		        		    UieErrorMsg(LDT_PAPERCHG_ADR, UIE_WITHOUT);        /* Paper Change */
				        }
	    		    }
		        }
		       	fsPrintStatus = pItem->fsPrintStatus;
           		pItem->fsPrintStatus = PRT_SLIP;             			/* print slip/journal */
    		    TrnThrough( pItem );
	      		pItem->fsPrintStatus = fsPrintStatus;
    	  		pItem->fsPrintStatus &= ~PRT_SLIP;             		/* avoid duplicated slip validation */
		        sValPrintFlag = 1;	/* 06/22/01 */
	     	}
	     }
        }
    }

	// print split receipts.  needs to be here so that we have the EPT reponse text
	ItemTendSplitReceipts (pUifTender, pItem);          //If doing split tender, need multiple receipts

    ItemTenderLocal.ItemTotal.uchMajorClass = CLASS_ITEMTOTAL;
    ItemTenderLocal.ItemTotal.uchMinorClass = CLASS_TOTAL2;
    ItemTenderLocal.ItemTotal.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;

    ItemTendCalFSAmount(&ItemTenderLocal.ItemTotal.lMI);

    ItemTendFSNonFoodPartTax();

    /*--- Second Food Stamp Tender, V2.01.04 ---*/
    if (pItem->uchMinorClass != CLASS_TENDER2) {
		ITEMTENDER      WorkTend = {0};

		// Prepare the tender information by copying common data into our work area.
        WorkTend.uchMajorClass = CLASS_ITEMTENDER;
		WorkTend.lRound = ItemTenderLocal.ItemTotal.lMI;
        WorkTend.fsPrintStatus  = pItem->fsPrintStatus;
        WorkTend.uchTenderYear = pItem->uchTenderYear;
        WorkTend.uchTenderMonth = pItem->uchTenderMonth;
        WorkTend.uchTenderDay = pItem->uchTenderDay;
        WorkTend.uchTenderHour = pItem->uchTenderHour;
        WorkTend.uchTenderMinute = pItem->uchTenderMinute;
        WorkTend.uchTenderSecond = pItem->uchTenderSecond;

        if (ItemTenderLocal.ItemFSTotal.uchMajorClass != 0) {
            if (! (ItemTenderLocal.fbTenderStatus[1] & TENDER_FS_PARTIAL2)) {
				ITEMTOTAL   WorkTotal = {0};

                ItemTendFSTotal(&WorkTotal);
				WorkTotal.lMI -= ItemTenderLocal.lFSTenderizer - ItemTenderLocal.ItemFSTender.lTenderAmount;
                TrnItem(&WorkTotal);
                MldScrollWrite(&WorkTotal, MLD_NEW_ITEMIZE);
                MldScrollFileWrite(&WorkTotal);
                /* send to enhanced kds, 2172 */
                ItemSendKds(&WorkTotal, 0);

				WorkTotal = ItemTenderLocal.ItemFSTotal;
				WorkTotal.lMI -= ItemTenderLocal.lFSTenderizer - ItemTenderLocal.ItemFSTender.lTenderAmount;
                TrnItem(&WorkTotal);
                MldScrollWrite(&WorkTotal, MLD_NEW_ITEMIZE);
                MldScrollFileWrite(&WorkTotal);
                /* send to enhanced kds, 2172 */
                ItemSendKds(&WorkTotal, 0);

            }

            /* print out first food stamp tender */
            WorkTend.uchMinorClass = CLASS_TENDER2;
            WorkTend.lTenderAmount = ItemTenderLocal.lFSTenderizer;
            WorkTend.lTenderAmount -= ItemTenderLocal.lFSTenderizer2;
            WorkTend.lTenderAmount -= pUifTender->lTenderAmount * sSign;
			WorkTend.lRound = ItemTenderLocal.ItemTotal.lMI;
            memcpy(WorkTend.aszNumber, pItem->aszNumber, sizeof(WorkTend.aszNumber));
            memcpy(WorkTend.aszFolioNumber, pItem->aszFolioNumber, sizeof(WorkTend.aszFolioNumber));
            memcpy(WorkTend.aszCPMsgText, pItem->aszCPMsgText, sizeof(WorkTend.aszCPMsgText));
            memcpy(WorkTend.auchExpiraDate, pItem->auchExpiraDate, sizeof(WorkTend.auchExpiraDate));
            memcpy(WorkTend.auchApproval, pItem->auchApproval, sizeof(WorkTend.auchApproval));
            WorkTend.uchCPLineNo    = pItem->uchCPLineNo;
            WorkTend.uchPrintSelect = pItem->uchPrintSelect;
            memcpy(WorkTend.auchMSRData, pItem->auchMSRData, sizeof(WorkTend.auchMSRData));
            WorkTend.uchTenderOff   = pItem->uchTenderOff;
            WorkTend.uchBuffOff     = pItem->uchBuffOff;

            if (WorkTend.lTenderAmount) {
                WorkTend.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
                TrnItem(&WorkTend);
                MldScrollWrite(&WorkTend, MLD_NEW_ITEMIZE);
                MldScrollFileWrite(&WorkTend);
                
                /* send to enhanced kds, 2172 */
                ItemSendKds(&WorkTend, 0);
            }
        }

        WorkTend.uchMinorClass = CLASS_TENDER11;
        WorkTend.lTenderAmount = pUifTender->lTenderAmount * sSign;    /* FS amount */
        WorkTend.fbModifier = pItem->fbModifier;
        WorkTend.fbStorageStatus |= NOT_SEND_KDS;
        WorkTend.uchPrintSelect = pItem->uchPrintSelect;

        TrnItem(&WorkTend);

        /* send partial tender data to KDS directory R3.1 */
        WorkTend.fbStorageStatus &= ~NOT_SEND_KDS;
        TrnThroughDisp(&WorkTend);

        MldScrollWrite(&WorkTend, MLD_NEW_ITEMIZE);
        MldScrollFileWrite(&WorkTend);

        /* send to enhanced kds, 2172 */
        ItemSendKds(&WorkTend, 0);

        ItemTenderLocal.lFSTenderizer2 += WorkTend.lTenderAmount;
        ItemTenderLocal.fbTenderStatus[1] |= TENDER_FS_PARTIAL2;
    }

    ItemTenderLocal.fbTenderStatus[1] &= ~TENDER_FS_TOTAL;

    return(UIF_FS2NORMAL);
}

/*
*==========================================================================
**  Synopsis:   VOID    ItemTendFSNonFoodPartTax(VOID)
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    calculate and save tax (auto)
*=========================================================================
*/
VOID    ItemTendFSNonFoodPartTax(VOID)
{
	ITEMAFFECTION   Affect = {0};
    SHORT           i;

    if (TranGCFQualPtr->usGuestNo == 0) {
        ItemTotalCalTax(&Affect, 0);
    } else {
        ItemTotalCalTax(&Affect, 1);
    }

    for (i = 0; i < STD_TAX_ITEMIZERS_MAX; i++) {
        ItemTenderLocal.lWorkTaxable[i] = Affect.USCanVAT.USCanTax.lTaxable[i];
        ItemTenderLocal.lWorkTax[i]     = Affect.USCanVAT.USCanTax.lTaxAmount[i];
        ItemTenderLocal.lWorkExempt[i]  = Affect.USCanVAT.USCanTax.lTaxExempt[i];
        ItemTenderLocal.lWorkFSExempt[i]= Affect.USCanVAT.USCanTax.lFSExempt[i];
    }
}

/*
*==========================================================================
**  Synopsis:   SHORT   ItemTendFSOver(UIFREGTENDER *pUifTender, ITEMTENDER *pItem)
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    F.S even/over tender
*==========================================================================
*/
SHORT   ItemTendFSOver(UIFREGTENDER *pUifTender, ITEMTENDER *pItem)
{
    ULONG       fsCurStatus;
	ITEMTENDER  WorkTender = {0};
    ITEMTENDER  FSTender = {0};
	PARATENDERKEYINFO TendKeyInfo;

    /* V2.01.05 */
    if ((pItem->auchTendStatus[0] & ITEMTENDER_STATUS_0_DRAWEROPEN) != 0) {
        ItemDrawer();
    }
    if (ItemTenderLocal.ItemFSTotal.uchMajorClass != 0) {
		ITEMTOTAL   WorkTotal = {0};

        ItemTendFSTotal(&WorkTotal);
		WorkTotal.lMI -= ItemTenderLocal.lFSTenderizer - ItemTenderLocal.ItemFSTender.lTenderAmount;
        MldScrollWrite(&WorkTotal, MLD_NEW_ITEMIZE);
        MldScrollFileWrite(&WorkTotal);
        TrnItem(&WorkTotal);
        /* send to enhanced kds, 2172 */
        ItemSendKds(&WorkTotal, 0);

        if (! (ItemTenderLocal.fbTenderStatus[1] & TENDER_FS_PARTIAL2)) {
			WorkTotal = ItemTenderLocal.ItemFSTotal;
			WorkTotal.lMI -= ItemTenderLocal.lFSTenderizer - ItemTenderLocal.ItemFSTender.lTenderAmount;
            TrnItem(&WorkTotal);
            MldScrollWrite(&WorkTotal, MLD_NEW_ITEMIZE);
            MldScrollFileWrite(&WorkTotal);

            /* send to enhanced kds, 2172 */
            ItemSendKds(&WorkTotal, 0);
        }

        memset(&ItemTenderLocal.ItemFSTotal, 0, sizeof(ITEMTOTAL));
    }

    ItemTendFSOverTender(pUifTender, pItem);                /* FS even/over tender */

    FSTender = *pItem;
    ItemTendFSTax(pItem);                                   /* tax/total/change class */

    ItemTendAffection();                                    /* Hourly etc. Dec/4/2000 */

    fsCurStatus = TranCurQualPtr->fsCurStatus;                             /* Saratoga */
    /* if (!(fsStatus & CURQUAL_PVOID)) { */
        ItemTendPrintFSChange();
    /* } */

	TendKeyInfo.uchMajorClass = CLASS_PARATENDERKEYINFO;
	TendKeyInfo.uchAddress = pUifTender->uchMinorClass;
	CliParaRead(&TendKeyInfo);

	if (TendKeyInfo.TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_PREPAID) {
		SetReceiptPrint(SET_RECEIPT_TRAN_PREPAID);
	}
	else {
		SetReceiptPrint(SET_RECEIPT_TRAN_EPT);
	}

    if (ItemTendIsDup()) {
		SetReceiptPrint(SET_RECEIPT_FIRST_COPY);
	}

    if (ItemTenderLocal.fbTenderStatus[0] & TENDER_EPT      /* exist EPT tender ? */
        && CliParaMDCCheck(MDC_CPPARA2_ADR, EVEN_MDC_BIT1)) {
        ItemTrailer(TYPE_EPT);

    } else {
        ItemTrailer(TYPE_STORAGE);
    }

    ItemTendFSClose();                                          /* transaction close */
    ItemTendDupRec();                                           /* EPT duplicate    */
    if (!(fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN))) {
        ItemTendFSChit();                                       /* generate F.S chit */
    }
    ItemTendDelGCF();                                           /* delete GCF */
    ItemTendInit();                                             /* initialize transaction information */
    return(ItemFinalize());                                     /* tender finalize */
}

/*
*==========================================================================
**  Synopsis:   VOID    ItemTendFSOverTender(UIFREGTENDER *pUifTender, ITEMTENDER *pItem)
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    generate F.S even/over tender class
*==========================================================================
*/
VOID    ItemTendFSOverTender(UIFREGTENDER *pUifTender, ITEMTENDER *pItem)
{
    SHORT     sSign = 1;
    SHORT     sValPrintFlag = 0;  // validation print required flag
    USHORT    fsPrintStatus;      /* 51:print status */

    sSign = ItemTendDetermineSignFS();

    /* --- Second Food Stamp Tender, V2.01.04 ---*/
    if (pUifTender->uchMinorClass != CLASS_TENDER2) {
		ITEMTENDER      WorkTend = {0};

        /* print out first food stamp tender */
        WorkTend.uchMajorClass = CLASS_ITEMTENDER;
        WorkTend.uchMinorClass = CLASS_TENDER2;
        WorkTend.lTenderAmount = ItemTenderLocal.lFSTenderizer;
        WorkTend.lTenderAmount -= ItemTenderLocal.lFSTenderizer2;
        WorkTend.lTenderAmount -= pUifTender->lTenderAmount * sSign;
        memcpy(&WorkTend.aszNumber[0], &ItemTenderLocal.ItemFSTender.aszNumber[0], sizeof(WorkTend.aszNumber));

        if (WorkTend.lTenderAmount) {
            WorkTend.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
            TrnItem(&WorkTend);
            MldScrollWrite(&WorkTend, MLD_NEW_ITEMIZE);
            MldScrollFileWrite(&WorkTend);
            
            /* send to enhanced kds, 2172 */
            ItemSendKds(&WorkTend, 0);
        }
    }

    pItem->uchMajorClass = CLASS_ITEMTENDER;
    pItem->uchMinorClass = pUifTender->uchMinorClass;
    pItem->fbModifier    = pUifTender->fbModifier;

    /*--- Second Food Stamp Tender, V2.01.04 ---*/
    if (pUifTender->uchMinorClass == CLASS_TENDER2) {
		// if a food stamp tender amount is entered then use that amount for the
		// tender.  Otherwise use what is in the food stamp tenderizer.
		pItem->lTenderAmount = pUifTender->lTenderAmount;
		if (pItem->lTenderAmount == 0) {
			pItem->lTenderAmount = ItemTenderLocal.lFSTenderizer;
			pItem->lTenderAmount -= ItemTenderLocal.lFSTenderizer2;
		}
    } else {
        pItem->lTenderAmount = pUifTender->lTenderAmount * sSign;
    }

    /* --- Dec/6/2000 --- */
    if (pItem->auchTendStatus[0] & ITEMTENDER_STATUS_0_VALIDATION) {  /* validation print */
		ITEMTENDER      WorkTend = *pItem;

        WorkTend.fsPrintStatus = PRT_VALIDATION;
        WorkTend.lTenderAmount = pUifTender->lTenderAmount * sSign;
        TrnThrough(&WorkTend);
        sValPrintFlag = 1;
    }

    /* --- Dec/6/2000 --- */
    if (CliParaMDCCheck(MDC_PSELVD_ADR, EVEN_MDC_BIT2)) {
        if (TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)) {
            /* --- Dec/6/2000 --- */
	        if (sValPrintFlag == 1) {
		        if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) {	/* only slip validation, 03/23/01 */
    				if ( CliParaMDCCheck(MDC_SLIPVAL_ADR, EVEN_MDC_BIT0) ) { /* both validation and slip print */
				        UieErrorMsg(LDT_VSLPREMV_ADR, UIE_WITHOUT);        /* Paper Change */
    				} else {
		        	    UieErrorMsg(LDT_PAPERCHG_ADR, UIE_WITHOUT);        /* Paper Change */
			        }
		        }
        	}
            ItemTendAudVal(ItemTendWholeTransAmount());            /* audaction validation */
	        sValPrintFlag = 1;	/* 06/22/01 */
        }
    }

    /* --- Dec/6/2000 --- */
    pItem->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;

	{
		ITEMTENDER      WorkTend = *pItem;

		WorkTend.lChange = ItemTenderLocal.lCashChange;
		ItemTendFSOverDisp(pUifTender, &WorkTend);
		ItemTendCoin(&WorkTend);
	}

    if (ItemTendCheckEPT(pUifTender) == ITM_EPT_TENDER) {
        ItemTendCPPrintRsp(pItem);
    	if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_UNBUFFER) {
			pItem->fsPrintStatus &= ~PRT_SLIP;             		/* avoid duplicated slip validation */
		} else {
   			/* execute slip validation for partial ept tender, 09/12/01 */
			if ((pItem->fsPrintStatus & PRT_SLIP) && (pItem->uchCPLineNo != 0)) {                /* Saratoga */
				if (sValPrintFlag == 1) {
					if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) {	/* only slip validation, 03/23/01 */
    					if ( CliParaMDCCheck(MDC_SLIPVAL_ADR, EVEN_MDC_BIT0) ) { /* both validation and slip print */
							UieErrorMsg(LDT_VSLPREMV_ADR, UIE_WITHOUT);        /* Paper Change */
    					} else {
		            		UieErrorMsg(LDT_PAPERCHG_ADR, UIE_WITHOUT);        /* Paper Change */
						}
					}
        		}
	       		fsPrintStatus = pItem->fsPrintStatus;
       			pItem->fsPrintStatus = PRT_SLIP;             			/* print slip/journal */
   				TrnThrough( pItem );
      			pItem->fsPrintStatus = fsPrintStatus;
      			pItem->fsPrintStatus &= ~PRT_SLIP;             		/* avoid duplicated slip validation */
				sValPrintFlag = 1;	/* 06/22/01 */

      			/* release slip to prevent the optional listing slip */
				if ( !CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) {	/* only slip validation, 03/23/01 */
	    			TrnSlipRelease();
	   			}
     		}
     	}
    }

    TrnItem(pItem);
    MldScrollWrite(pItem, MLD_NEW_ITEMIZE);
    MldScrollFileWrite(pItem);

    if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) { /* validation slip print */
    	if ( CliParaMDCCheck(MDC_SLIPVAL_ADR, EVEN_MDC_BIT0) ) { /* both validation and slip print */
            if (sValPrintFlag == 1) {
		        UieErrorMsg(LDT_VSLPREMV_ADR, UIE_WITHOUT);        /* Paper Change */
			}
		}
	}

    /* send to enhanced kds, 2172 */
    ItemSendKds(pItem, 0);

    /*--- Second Food Stamp Tender, V2.01.04 ---*/
    if (pUifTender->uchMinorClass != CLASS_TENDER2) {
#if 0
        fsTenderKey = ItemTendCheckTenderKey( UifRegTender->uchMinorClass, &ItemTender->auchStatus[0] );
        if ( (fsTenderKey & KEY_CPM )                                      /* R2.0GCA */
            && !(ItemTender->fsModifier & OFFL_MODIFIER)) {
            ItemTendCPPrintRsp();                       /* Print response R2.0 */
            ItemTendSaveEPT(ItemTender);
        }
#endif
    }
}

/*
*==========================================================================
**  Synopsis:   VOID    ItemTendFSOverDisp(UIFREGTENDER *pUifTender, ITEMTENDER *pItem)
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender          
*   Output:     none 
*   InOut:      none
*
*   Return:     
*
*   Description:    display of even/over tender data            Saratoga
*==========================================================================
*/
VOID    ItemTendFSOverDisp(UIFREGTENDER *pUifTender, ITEMTENDER *pItem)
{
	REGDISPMSG      DispMsg = {0}, DispMsgBack = {0};

    DispMsg.uchMajorClass = pUifTender->uchMajorClass;
    DispMsg.uchMinorClass = CLASS_UIFFSCHANGE;
    RflGetTranMnem ( DispMsg.aszMnemonic, TRN_REGCHG_ADR );
    flDispRegDescrControl |= CHANGE_CNTRL;                          /* descriptor */
    DispMsg.fbSaveControl = 2;                                      /* save for redisplay (not recovery) */

	if ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_USEXEMPT ) {
        flDispRegDescrControl |= TAXEXMPT_CNTRL;                /* tax exempt */
    }

    ItemTendDispIn( &DispMsgBack );                                 /* generate display mnemonic data */
    flDispRegDescrControl &= ~( TVOID_CNTRL | TENDER_CNTRL );
    flDispRegKeepControl &= ~( TVOID_CNTRL | PAYMENT_CNTRL | RECALL_CNTRL | TAXEXMPT_CNTRL | TENDER_CNTRL | FOODSTAMP_CTL);

    if ( CliParaMDCCheck( MDC_RECEIPT_ADR, ODD_MDC_BIT0 ) ) {       /* receipt control */
        flDispRegKeepControl &= ~RECEIPT_CNTRL;
        flDispRegDescrControl &= ~RECEIPT_CNTRL;
    }

    if ( CliParaMDCCheck( MDC_MENU_ADR, ODD_MDC_BIT0) != 0 ) {
		ITEMSALESLOCAL  *pWorkSales = ItemSalesGetLocalPointer();       /* get sales local data */
        uchDispCurrMenuPage = pWorkSales->uchMenuShift;                 /* set default menu no. */
        if (pWorkSales->uchAdjKeyShift == 0) {                          /* not use adj. shift key */
            pWorkSales->uchAdjCurShift = pWorkSales->uchAdjDefShift;    /* set default adjective, 2172 */
        }
    }

    DispMsg.lAmount = ItemTenderLocal.lCashChange;
    DispMsg.lQTY = ItemTenderLocal.lFS * 10L;

    DispWriteSpecial( &DispMsg, &DispMsgBack );                     /* display check total */

    flDispRegDescrControl &= ~( CHANGE_CNTRL | FOODSTAMP_CTL |TAXEXMPT_CNTRL );
}

/*
*==========================================================================
**   Synopsis:  VOID    ItemTendFSChange(LONG lTax)
*
*   Input:      LONG    lTax
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    Generate Food Stamp and Food Stamp credit data to determine
*                   the type of change to provide along with how much change.
*                   Some of this is source to support the older Food Stamp
*                   program which used coupons in various denominations ($1, $5, etc.)
*                   however the modern Supplemental Nutrition Assissance Program
*                   or SNAP which replaced the Food Stamp program in 2008
*                   uses a kind of debit card which was slowly phased induring 1990s.
*==========================================================================
*/
static VOID    ItemTendFSChange(DCURRENCY lTax)
{
    TRANITEMIZERS   *WorkTI = TrnGetTIPtr();
    LONG            lDen, lLimit;
    SHORT           sSign = 1;
	PARAMISCPARA    WorkDen = {0}, WorkLimit = {0};
    TRANCURQUAL     WorkCur;

    TrnGetCurQual(&WorkCur);
    sSign = ItemTendDetermineSignFS();

	// Read the Food Stamp Transaction Limit data from AC128 to determine the
	// limit for a Food Stamp purchase as well as the smallest denomination amount.
    WorkDen.uchMajorClass = WorkLimit.uchMajorClass = CLASS_PARAMISCPARA;
    WorkDen.uchAddress = MISC_FOODDENOMI_ADR;
    CliParaRead(&WorkDen);
    lDen = (LONG)WorkDen.ulMiscPara;

    WorkLimit.uchAddress = MISC_FOODLIMIT_ADR;
    CliParaRead(&WorkLimit);
    lLimit = (LONG)WorkLimit.ulMiscPara;

    if (lDen != 0L) {               /* Food Stamp denomination or coupons */
		DCURRENCY          lAmount = 0;

		// There is a Smallest Denomination Amount set in the AC128 Transaction Limits
		// Probably no longer used with the phase in of debit type cards during the 1990s
		// to replace physical paper coupons of various denominations ($1, $5, etc.).
		// The idea is to provide change with Food Stamp coupons when possible providing US currency
		// for the coins part of the change.
		//   ItemTenderLocal.lFS is put into ITEMTENDER.lFoodStamp
		//   ItemTenderLocal.lFSChange is put into ITEMTENDER.lFSChange
        /* TAR 111547 */
        ItemTenderLocal.lFS = ((ItemTenderLocal.lFSTenderizer - WorkTI->Itemizers.TranUsItemizers.lFoodStampable) / lDen * lDen);
        lAmount = ItemTenderLocal.lFSTenderizer - ItemTenderLocal.lFS;

        /* --- even/over tender --- */
        if (lAmount * sSign >= (WorkTI->lMI + lTax) * sSign) {
            /* --- cash change --- */
            if (WorkTI->Itemizers.TranUsItemizers.lFoodStampable * sSign >= lLimit) {
                ItemTenderLocal.lCashChange = lAmount - WorkTI->lMI - lTax;
                /* --- chit change --- */
            } else {
                if (WorkCur.fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)) {
                    ItemTenderLocal.lCashChange = lAmount - WorkTI->lMI - lTax;
                } else {
                    ItemTenderLocal.lFSChange = lAmount - WorkTI->Itemizers.TranUsItemizers.lFoodStampable;
                    ItemTenderLocal.lCashChange = WorkTI->lMI - lTax - WorkTI->Itemizers.TranUsItemizers.lFoodStampable;
                }
            }
        } else {
            if (ItemTenderLocal.lFSTenderizer * sSign > WorkTI->Itemizers.TranUsItemizers.lFoodStampable * sSign) {
                /* --- cash change --- */
                if (WorkTI->Itemizers.TranUsItemizers.lFoodStampable * sSign >= lLimit) {
                    ItemTenderLocal.lCashChange = lAmount - WorkTI->Itemizers.TranUsItemizers.lFoodStampable;
                /* --- chit change --- */
                } else {
                    ItemTenderLocal.lFSChange = lAmount - WorkTI->Itemizers.TranUsItemizers.lFoodStampable;
                }
            }
        }
    } else {
        if (ItemTenderLocal.lFSTenderizer * sSign >= (WorkTI->lMI + lTax) * sSign) {
            if (WorkTI->Itemizers.TranUsItemizers.lFoodStampable * sSign >= lLimit) {
               /* --- cash change --- */
               ItemTenderLocal.lCashChange = ItemTenderLocal.lFSTenderizer - WorkTI->lMI - lTax;
            } else {
				/* --- chit change --- */
               if (WorkCur.fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)) {
                    ItemTenderLocal.lCashChange = ItemTenderLocal.lFSTenderizer - WorkTI->lMI - lTax;
                } else {
                    ItemTenderLocal.lFSChange = ItemTenderLocal.lFSTenderizer - WorkTI->Itemizers.TranUsItemizers.lFoodStampable;
                    ItemTenderLocal.lCashChange = WorkTI->lMI - lTax - WorkTI->Itemizers.TranUsItemizers.lFoodStampable;
                }
            }
        } else {
            if (ItemTenderLocal.lFSTenderizer * sSign > WorkTI->Itemizers.TranUsItemizers.lFoodStampable * sSign) {
                if (WorkTI->Itemizers.TranUsItemizers.lFoodStampable * sSign >= lLimit) {
					/* --- cash change --- */
                    ItemTenderLocal.lCashChange = ItemTenderLocal.lFSTenderizer - WorkTI->Itemizers.TranUsItemizers.lFoodStampable;
                } else {
					/* --- chit change --- */
                    ItemTenderLocal.lFSChange = ItemTenderLocal.lFSTenderizer - WorkTI->Itemizers.TranUsItemizers.lFoodStampable;
                }
            }
        }
    }

    if (WorkCur.fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)) {
        ItemTenderLocal.lCashChange += ItemTenderLocal.lFSChange;
        ItemTenderLocal.lFSChange = 0L;
    }
}

/*
*==========================================================================
**   Synopsis:  VOID    ItemTendFSChangeOver(LONG lAmount)
*
*   Input:      LONG    lAmount
*   Output:     none
*   InOut:      none
*
*   Return: 
*
*   Description:    Generate Food Stamp and Food Stamp credit data (over tender) to
*                   determine the type of change to provide along with how much change.
*                   Some of this is source to support the older Food Stamp
*                   program which used coupons in various denominations ($1, $5, etc.)
*                   however the modern Supplemental Nutrition Assissance Program
*                   or SNAP which replaced the Food Stamp program in 2008
*                   uses a kind of debit card which was slowly phased induring 1990s.
*==========================================================================
*/
static VOID    ItemTendFSChangeOver(DCURRENCY lAmount)
{
    TRANITEMIZERS   *WorkTI = TrnGetTIPtr();
    LONG            lDen, lLimit;
	PARAMISCPARA    WorkMisc = {0};
    SHORT           sSign = 1;

    sSign = ItemTendDetermineSignFS();

    WorkMisc.uchMajorClass = CLASS_PARAMISCPARA;
    WorkMisc.uchAddress = MISC_FOODDENOMI_ADR;
    CliParaRead(&WorkMisc);
    lDen = (LONG)WorkMisc.ulMiscPara;

    WorkMisc.uchAddress = MISC_FOODLIMIT_ADR;
    CliParaRead(&WorkMisc);
    lLimit = (LONG)WorkMisc.ulMiscPara;

    /* --- FTEND > FMI --- */
    if (WorkTI->Itemizers.TranUsItemizers.lFoodStampable * sSign < ItemTenderLocal.lFSTenderizer * sSign) {
        /* --- cash change --- */
        ItemTenderLocal.lCashChange = WorkTI->Itemizers.TranUsItemizers.lFoodStampable - lAmount;
    } else {
        ItemTenderLocal.lCashChange = ItemTenderLocal.lFSTenderizer - lAmount;
    }

    if (lDen != 0L) {
		// There is a Smallest Denomination Amount set in the AC128 Transaction Limits
		// Probably no longer used with the phase in of debit type cards during the 1990s
		// to replace physical paper coupons of various denominations ($1, $5, etc.).
		// The idea is to provide change with Food Stamp coupons when possible providing US currency
		// for the coins part of the change.
        /* --- FTEND > FMI --- */
        if (WorkTI->Itemizers.TranUsItemizers.lFoodStampable * sSign < ItemTenderLocal.lFSTenderizer * sSign) {
			DCURRENCY       lChange = 0;

            if (WorkTI->Itemizers.TranUsItemizers.lFoodStampable * sSign >= lLimit) {
				/* --- cash change --- */
                lChange = (ItemTenderLocal.lFSTenderizer - WorkTI->Itemizers.TranUsItemizers.lFoodStampable ) % lDen;
                ItemTenderLocal.lCashChange += lChange;
            } else {
				/* --- chit change --- */
                lChange = (ItemTenderLocal.lFSTenderizer - WorkTI->Itemizers.TranUsItemizers.lFoodStampable) % lDen;
                ItemTenderLocal.lFSChange += lChange;
            }

            /* TAR 111547 */
            ItemTenderLocal.lFS = (ItemTenderLocal.lFSTenderizer - WorkTI->Itemizers.TranUsItemizers.lFoodStampable - lChange);
        }
    } else {
        /* --- FTEND > FMI --- */
        if (WorkTI->Itemizers.TranUsItemizers.lFoodStampable * sSign < ItemTenderLocal.lFSTenderizer * sSign ) {

            if (WorkTI->Itemizers.TranUsItemizers.lFoodStampable * sSign >= lLimit) {
				/* --- cash change --- */
                ItemTenderLocal.lCashChange += ItemTenderLocal.lFSTenderizer - WorkTI->Itemizers.TranUsItemizers.lFoodStampable;
            } else {
				/* --- chit change --- */
                ItemTenderLocal.lFSChange += ItemTenderLocal.lFSTenderizer - WorkTI->Itemizers.TranUsItemizers.lFoodStampable;
            }
        }
    }

    if (TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)) {
        ItemTenderLocal.lCashChange += ItemTenderLocal.lFSChange;
        ItemTenderLocal.lFSChange = 0L;
    }
}

/*
*==========================================================================
**  Synopsis:   VOID    ItemTendFSChit(VOID)
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    generate F.S chit (credit/voucher) class
*==========================================================================
*/
VOID    ItemTendFSChit(VOID)
{
/* for P-void
    if (ItemTenderLocal.lFSChange != 0 || ItemTenderLocal.lFS != 0) {
		ITEMTENDER      WorkTend;
        memset(&WorkTend, 0, sizeof(WorkTend));
        WorkTend.uchMajorClass  = CLASS_ITEMTENDER;
        WorkTend.uchMinorClass  = CLASS_TEND_FSCHANGE;
        WorkTend.lFoodStamp     = ItemTenderLocal.lFS;
        WorkTend.lFSChange      = ItemTenderLocal.lFSChange;
        MldScrollWrite(&WorkTend, MLD_NEW_ITEMIZE);
        MldScrollFileWrite(&WorkTend);
    }
*/

    /* --- FS credit amount --- */
    if (ItemTenderLocal.lFSChange != 0) {
		ITEMPRINT       Print = {0};
		Print.uchMajorClass = CLASS_ITEMPRINT;
		Print.uchMinorClass = CLASS_FOOD_CREDIT;
        Print.lAmount = ItemTenderLocal.lFSChange;
        if (CliParaMDCCheck(MDC_FS1_ADR, ODD_MDC_BIT1)) {
            Print.fsPrintStatus = PRT_RECEIPT | PRT_SPCL_PRINT;
            TrnThrough(&Print);
            ItemTrailer(TYPE_STB_TRAIL);
        } else {
	        if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) {	/* only slip validation, 03/23/01 */
    			if ( CliParaMDCCheck(MDC_SLIPVAL_ADR, EVEN_MDC_BIT0) ) { /* both validation and slip print */
			        UieErrorMsg(LDT_VSLPINS_ADR, UIE_WITHOUT);        /* Paper Change */
    			} else {
		            UieErrorMsg(LDT_SLPINS_ADR, UIE_WITHOUT);        /* Paper Change */
		        }
	        }
			
            Print.fsPrintStatus = PRT_VALIDATION;
            TrnThrough(&Print);
        }
    }
}

/*
*==========================================================================
**  Synopsis:   VOID    ItemTendPrintFSChange(VOID)
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    generate F.S chit (credit/voucher) class
*==========================================================================
*/
VOID    ItemTendPrintFSChange(VOID)
{

    if (ItemTenderLocal.lFSChange != 0 || ItemTenderLocal.lFS != 0) {
		ITEMTENDER      WorkTend = {0};

        WorkTend.uchMajorClass  = CLASS_ITEMTENDER;
        WorkTend.uchMinorClass  = CLASS_TEND_FSCHANGE;      // ItemTendPrintFSChange()
        WorkTend.lFoodStamp     = ItemTenderLocal.lFS;
        WorkTend.lFSChange      = ItemTenderLocal.lFSChange;
        WorkTend.fsPrintStatus  = PRT_SLIP | PRT_JOURNAL;
        /* for P-void */
        MldScrollWrite(&WorkTend, MLD_NEW_ITEMIZE);
        MldScrollFileWrite(&WorkTend);
        TrnItem(&WorkTend);
    }
}

/*
*==========================================================================
**  Synopsis:   VOID    ItemTendFSTax(ITEMTENDER *pItem)
*
*   Input:      ITEMTENDER  *ItemTender
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    generate tax/total/change class
*==========================================================================
*/
static VOID    ItemTendFSTax(ITEMTENDER *pItem)
{
    SHORT           i, sSign = 0;
	ITEMAFFECTION   Tax = {0}, TaxPrint = {0};
    ITEMTOTAL       WorkTotal = {0};
    ITEMTENDER      WorkTend = *pItem;

    sSign = ItemTendDetermineSignFS();

    WorkTend.uchMajorClass = CLASS_ITEMTENDER;
    WorkTend.uchMinorClass = CLASS_TEND_FSCHANGE;

    /*--- Second Food Stamp Tender, V2.01.04 ---*/
    /* do not print at food stamp change */
    /* WorkTend.fsModifier &= ~OFFL_MODIFIER; */
    WorkTend.aszNumber[0] = 0;
    WorkTend.auchExpiraDate[0] = 0;
    WorkTend.auchApproval[0] = 0;
    WorkTend.auchMSRData[0] = 0;
    WorkTend.aszFolioNumber[0] = 0;
    WorkTend.aszPostTransNo[0] = 0;
    WorkTend.aszTendMnem[0] = 0;
    WorkTend.lFoodStamp = ItemTenderLocal.lFS;

    if (TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)) {
        WorkTend.lChange = ItemTenderLocal.lFSChange + ItemTenderLocal.lCashChange;
    } else {
        WorkTend.lChange = ItemTenderLocal.lCashChange;
        WorkTend.lFSChange = ItemTenderLocal.lFSChange;
    }

    if (TranGCFQualPtr->usGuestNo == 0) {
        ItemTotalCalTax(&Tax, 0);
        for (i = 0; i < STD_TAX_ITEMIZERS_MAX; i++) {
            TaxPrint.USCanVAT.USCanTax.lTaxable[i] = Tax.USCanVAT.USCanTax.lTaxable[i];
            TaxPrint.USCanVAT.USCanTax.lTaxAmount[i] = Tax.USCanVAT.USCanTax.lTaxAmount[i];
            TaxPrint.USCanVAT.USCanTax.lTaxExempt[i] = Tax.USCanVAT.USCanTax.lTaxExempt[i];
            TaxPrint.USCanVAT.USCanTax.lFSExempt[i] = Tax.USCanVAT.USCanTax.lFSExempt[i];
        }
    } else {
		TRANITEMIZERS   *WorkTI = TrnGetTIPtr();
        ItemTotalCalTax(&Tax, 1);
        for (i = 0; i < STD_TAX_ITEMIZERS_MAX; i++) {
            TaxPrint.USCanVAT.USCanTax.lTaxable[i] = Tax.USCanVAT.USCanTax.lTaxable[i] 
                + WorkTI->Itemizers.TranUsItemizers.lAffectTaxable[i];
            TaxPrint.USCanVAT.USCanTax.lTaxAmount[i] = Tax.USCanVAT.USCanTax.lTaxAmount[i] 
                + WorkTI->Itemizers.TranUsItemizers.lTaxItemizer[i];
            TaxPrint.USCanVAT.USCanTax.lTaxExempt[i] = Tax.USCanVAT.USCanTax.lTaxExempt[i] 
                + WorkTI->Itemizers.TranUsItemizers.lAffectTaxExempt[i];
            TaxPrint.USCanVAT.USCanTax.lFSExempt[i] = Tax.USCanVAT.USCanTax.lFSExempt[i]; 
        }
    }

    if (TaxPrint.USCanVAT.USCanTax.lTaxAmount[0] != 0L
        || TaxPrint.USCanVAT.USCanTax.lTaxAmount[1] != 0L
        || TaxPrint.USCanVAT.USCanTax.lTaxAmount[2] != 0L
        || TaxPrint.USCanVAT.USCanTax.lTaxable[0] != 0L
        || TaxPrint.USCanVAT.USCanTax.lTaxable[1] != 0L
        || TaxPrint.USCanVAT.USCanTax.lTaxable[2] != 0L
        || TaxPrint.USCanVAT.USCanTax.lTaxExempt[0] != 0L
        || TaxPrint.USCanVAT.USCanTax.lTaxExempt[1] != 0L
        || TaxPrint.USCanVAT.USCanTax.lTaxExempt[2] != 0L
        || TaxPrint.USCanVAT.USCanTax.lFSExempt[0] != 0L
        || TaxPrint.USCanVAT.USCanTax.lFSExempt[1] != 0L
        || TaxPrint.USCanVAT.USCanTax.lFSExempt[2] != 0L) {

        Tax.uchMajorClass = CLASS_ITEMAFFECTION;
        Tax.uchMinorClass = CLASS_TAXAFFECT;
        Tax.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
        TrnItem(&Tax);

        /* --- Dec/6/2000 --- */
        if (CliParaMDCCheck(MDC_TAX2_ADR, EVEN_MDC_BIT0) == 0) {
            TRANITEMIZERS   *WorkTITax = TrnGetTIPtr();
            WorkTITax->lHourly += Tax.USCanVAT.USCanTax.lTaxAmount[0] 
                + Tax.USCanVAT.USCanTax.lTaxAmount[1] + Tax.USCanVAT.USCanTax.lTaxAmount[2];
        }
        /* --- Dec/6/2000 --- */

        /* send to enhanced kds, 2172 */
        ItemSendKds(&Tax, 0);

        TaxPrint.uchMajorClass = CLASS_ITEMAFFECTION;
        TaxPrint.uchMinorClass = CLASS_TAXPRINT;
        TaxPrint.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
        TrnItem(&TaxPrint);
        MldScrollWrite(&TaxPrint, MLD_NEW_ITEMIZE);
        MldScrollFileWrite(&TaxPrint);

        WorkTotal.uchMajorClass = CLASS_ITEMTOTAL;
        WorkTotal.uchMinorClass = CLASS_TOTAL2;
        WorkTotal.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
        WorkTotal.lMI -= ItemTenderLocal.lFSChange + ItemTenderLocal.lCashChange + ItemTenderLocal.lFS;
        if (WorkTotal.lMI != 0L) {
            TrnItem(&WorkTotal);
            MldScrollWrite(&WorkTotal, MLD_NEW_ITEMIZE);
            MldScrollFileWrite(&WorkTotal);

            /* send to enhanced kds, 2172 */
            ItemSendKds(&WorkTotal, 0);
        }
    }

    /* send to enhanced kds, 2172 */
    if (WorkTend.lFSChange || WorkTend.lFoodStamp) {
        ItemSendKds(&WorkTend, 0);
    }

    if (WorkTend.lChange != 0L) {
        WorkTend.lFSChange  = 0L;
        WorkTend.lFoodStamp = 0L;
        TrnItem(&WorkTend);
        MldScrollWrite(&WorkTend, MLD_NEW_ITEMIZE);
        MldScrollFileWrite(&WorkTend);
    }
}

/*
==========================================================================
**  Synopsis:   VOID    ItemTendGetMDC(UCHAR uchClass, UCHAR *auchStatus)
*   Input:      UIFREGTENDER *  UifRegTender
*   Output:     ITEMTENDER *    ItemTender
*   InOut:      none
*   Return:
*   Description:    get tender MDC from MinorClass of UifRegTender
==========================================================================
*/
static VOID    ItemTendGetMDC(UCHAR uchClass, UCHAR *auchStatus)
{
	if (uchClass == CLASS_UITENDER2) {
		auchStatus[0] = RflGetMdcData(MDC_TEND21_ADR);
		auchStatus[1] = RflGetMdcData(MDC_TEND23_ADR);
	}
	else if (uchClass == CLASS_UITENDER11) {
		auchStatus[0] = RflGetMdcData(MDC_TEND111_ADR);
		auchStatus[1] = RflGetMdcData(MDC_TEND113_ADR);
	}
}
/*
*==========================================================================
**  Synopsis:   VOID    ItemTendPrtFSChange(ITEMTENDER *ItemTender)
*
*   Input:      ITEMTENDER  *ItemTender
*   Output:     ITEMTENDER  *ItemTender
*   InOut:      none
*
*   Return:
*
*   Description:    generate FS change data for print
*==========================================================================
*/
VOID    ItemTendPrtFSChange(ITEMTENDER *ItemTender)
{
    if (TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)) {
        ItemTender->lChange  += ItemTenderLocal.lFSChange;
    }
}


/*
*==========================================================================
**  Synopsis:   VOID    ItemTendFSClose(VOID)
*
*   Input:      none           
*   Output:     none
*   InOut:      none
**   Return:     
*
*   Description:    generate transaction close data
*==========================================================================
*/
VOID    ItemTendFSClose(VOID)
{
    TRANCURQUAL     *pWorkCur = TrnGetCurQualPtr();
	ITEMTRANSCLOSE  ItemTransClose = {0};
    SHORT           sStatus;

    pWorkCur->auchTotalStatus[0] = CLASS_TOTAL9;
	ItemTotalAuchTotalStatus (pWorkCur->auchTotalStatus, CLASS_TOTAL9);

    if (pWorkCur->flPrintStatus & CURQUAL_POSTCHECK) {
        pWorkCur->uchPrintStorage = PRT_CONSOLSTORAGE;                    /* postheck, store/recall */
    } else if ((pWorkCur->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_UNBUFFER) {
        pWorkCur->uchPrintStorage = PRT_NOSTORAGE;                        /* unbuffering */
    } else {
        pWorkCur->uchPrintStorage = PRT_ITEMSTORAGE;                      /* precheck */
    }

    if (CliParaMDCCheck(MDC_DEPT2_ADR, EVEN_MDC_BIT3)) {    /* Not Print Void Item, R3.1 */
        pWorkCur->uchKitchenStorage = PRT_KIT_NET_STORAGE;
    } else {
        pWorkCur->uchKitchenStorage = PRT_ITEMSTORAGE;        /* for kitchen print */
    }

    if ((CliParaMDCCheck(MDC_PSELVD_ADR, EVEN_MDC_BIT1) == 0/* kitchen send at pvoid */
        && ( pWorkCur->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN) ) )        /* preselect void */
        || ( pWorkCur->fsCurStatus & CURQUAL_TRAINING ) ) {   /* training */
        pWorkCur->uchKitchenStorage = PRT_NOSTORAGE;          /* for kitchen print */
    }

    if ( pWorkCur->auchTotalStatus[3] & CURQUAL_TOTAL_COMPULSRY_SLIP ) { /* compulsory slip */
        pWorkCur->fbCompPrint = PRT_SLIP;                   
    } else {                                                    /* optional slip */
        pWorkCur->fbCompPrint &= ~PRT_SLIP;
    }
                                                                    
    ItemTransClose.uchMajorClass = CLASS_ITEMTRANSCLOSE;
    ItemTransClose.uchMinorClass = CLASS_CLSTENDER1;
    TrnClose( &ItemTransClose );

    /* send to enhanced kds, 2172 */
    ItemSendKds(&ItemTransClose, 0);
    
    while ( ( sStatus = TrnSendTotal() ) !=  TRN_SUCCESS ) {
		USHORT   usStatus = TtlConvertError( sStatus );        
        UieErrorMsg( usStatus, UIE_WITHOUT );
    }
}

/* --- End of Source File --- */