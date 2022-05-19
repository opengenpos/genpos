/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Sales module main                         
* Category    : Item Module, NCR 2170 US Hsopitality Model Application         
* Program Name: SLLABEL.C
* --------------------------------------------------------------------------
* Abstract: ItemSalesLabelProc : Label PLU Process   
*           ItemSalesCheckKeyedPLU : Label PLU from Menu Key
* --------------------------------------------------------------------------
* Update Histories                                                         
* Update Histories                                                         
*    Date  : Ver. Rev. :   Name    : Description
* Nov-06-99:           : M.Ozawa   : Initial
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

#include	<tchar.h>
#include    <memory.h>
#include    <string.h>

#include    "ecr.h"
#include    "uie.h"
#include    "pif.h"
#include    "log.h"
#include    "regstrct.h"
#include    "transact.h"
#include    "item.h"
#include    "paraequ.h"
#include    "para.h"
#include    "csstbpar.h"
#include    "csop.h"
#include    "csstbopr.h"
#include    "display.h"
#include    "appllog.h"
#include    "csetk.h"
#include    "csstbetk.h"
#include    "cpm.h"
#include    "eept.h"
#include    "rfl.h"
#include    "fsc.h"
#include    "itmlocal.h"

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesLabelProc(UIFREGSALES   *pRegSales,
*                                      ITEMSALES     *pItemSales,
*                                      LABELANALYSIS *pPluLabel);
*
*   Input:  LABELANALYSIS   *pPluLabel
*
*   Output: None
*
*   InOut:  UIFREGSALES *pUifRegSales
*           ITEMSALES   *pItemSales
*
**Return:   UIF_SUCCESS - function executes successfuly
*           UIF_CANCEL  - cancel this operation
*           LDT_???_ADR - return error code if it has occerred
*
** Description: This function checkes the input label type
*               and executes the appropriate procedure.         R2.0
*===========================================================================
*/
SHORT   ItemSalesLabelKeyedOnlyProc (TCHAR *aszPLUNo)
{
	if (aszPLUNo[0]) {
		LABELANALYSIS   PluLabel = {0};

		/* ----- set plu number for label analysis ----- */
		_tcsncpy(PluLabel.aszScanPlu, aszPLUNo, NUM_PLU_SCAN_LEN);

		/* ----- analyze PLU number from UI ----- */
		if (RflLabelAnalysis(&PluLabel) == LABEL_OK) {
			_tcsncpy(aszPLUNo, PluLabel.aszLookPlu, NUM_PLU_LEN);
		} else {
			return(LDT_KEYOVER_ADR);
		}
	}
	return 0;
}

SHORT   ItemSalesLabelProc(UIFREGSALES *pRegSales, ITEMSALES *pItemSales)
{
	LABELANALYSIS   PluLabel = {0};
    SHORT           sReturnStatus;

    /* get PLU No. from PLU Key */
    if (pRegSales->uchMajorFsc == FSC_KEYED_PLU) {
        if ((sReturnStatus = ItemSalesCheckKeyedPLU(pRegSales)) != ITM_SUCCESS) {
            return(sReturnStatus);
        }
    }
    
    /* ----- set plu number for label analysis ----- */
    _tcsncpy(PluLabel.aszScanPlu, pRegSales->aszPLUNo, NUM_PLU_SCAN_LEN);

    /* ----- check E-version key ----- */
    if (pRegSales->fbModifier2 & EVER_MODIFIER) {
        PluLabel.fchModifier |= LA_EMOD;
    }

    /* ----- check UPC Velocity key ----- */
    if (pRegSales->fbModifier2 & VELO_MODIFIER) {
        PluLabel.fchModifier |= LA_UPC_VEL;
    }

    /* ----- PLU is entered by Scanner or Key ? ----- */
    if (pRegSales->uchMajorFsc == FSC_SCANNER) {
		SHORT           sOffset;

		if (ItemCheckCustomerSettingsFlags (SYSCONFIG_CUSTOMER_ENABLE_AMTRAK)) {
			// for Amtrak, do not allow scanned items if doing a return.
			if (TranCurQualPtr->fsCurStatus & (CURQUAL_TRETURN | CURQUAL_PRETURN)) {
				return(LDT_PROHBT_ADR);
			}
		}

        PluLabel.fchModifier |= LA_SCANNER;             /* by Scanner */
        sOffset = ItemCommonLabelPosi(pRegSales->aszPLUNo);
        if (sOffset < 0) {
            return(LDT_KEYOVER_ADR);
        }
    }

    /* ----- analyze PLU number from UI ----- */
    if (RflLabelAnalysis(&PluLabel) == LABEL_OK) {
        _tcsncpy(pRegSales->aszPLUNo, PluLabel.aszLookPlu, NUM_PLU_LEN);
        _tcsncpy(pItemSales->auchPLUNo, PluLabel.aszLookPlu, NUM_PLU_LEN);
    } else {
        return(LDT_KEYOVER_ADR);
    }
        
    switch (PluLabel.uchType) {
    case    LABEL_RANDOM:
	case	LABEL_RSS14VAR:
#if 0
        if (pPluLabel->fsBitMap & LA_F_FREQ) {
            return(ItemSalesFreq(pRegSales, pPluLabel));

        } else {
#endif
            /* go to random weight label analysis */
            sReturnStatus = ItemSalesLabelPreCheck(pRegSales, pItemSales, &PluLabel);
#if 0
        }
#endif
        if (sReturnStatus) {
            return(sReturnStatus);
        }
        break;

    case    LABEL_UPCA:
    case    LABEL_UPCE:
    case    LABEL_EAN13:
    case    LABEL_EAN8:
    case    LABEL_VELOC:
	case	LABEL_RSS14:
        pItemSales->uchMinorClass = CLASS_PLU;
        break;
        
    case    LABEL_COUPON:
		{
			UIFREGCOUPON UifRegCoupon = {0};

			/* UPC Coupon */
			if (pRegSales->lQTY != 0L) {
				return (LDT_SEQERR_ADR);
			}
			if (pRegSales->lUnitPrice) {
				return (LDT_SEQERR_ADR);
			}
			if (pRegSales->uchAdjective) {
				return (LDT_SEQERR_ADR);
			}
			if (pRegSales->fbModifier2 & (EVER_MODIFIER|PCHK_MODIFIER|SKU_MODIFIER)) {
				return (LDT_SEQERR_ADR);
			}
			if (pRegSales->uchMinorClass == CLASS_UIMODDISC) {
				return (LDT_SEQERR_ADR);
			}
			if (pRegSales->aszNumber[0]) {
				return (LDT_SEQERR_ADR);
			}

			UifRegCoupon.uchMajorClass = CLASS_UIFREGCOUPON;
			UifRegCoupon.uchMinorClass = CLASS_UIUPCCOUPON;
			_tcsncpy(UifRegCoupon.auchUPCCouponNo, PluLabel.aszLookPlu, NUM_PLU_LEN);
			UifRegCoupon.fbModifier = pRegSales->fbModifier;
			return(ItemCoupon(&UifRegCoupon));
		}
        break;
        
    default:
        return(LDT_KEYOVER_ADR);
    }
	
	if(pRegSales->uchMinorClass == CLASS_ITEMORDERDEC)
	{
		pItemSales->uchMinorClass = CLASS_ITEMORDERDEC;
	}

    if (! pItemSales->uchMinorClass) {
        return(ITM_SUCCESS);
    } else if (pItemSales->uchMinorClass == CLASS_DEPT) {
        /* --- execute the Dept procedure for some of random weight --- */
        if (pRegSales->uchMinorClass == CLASS_UIMODDISC) {
            pItemSales->uchMinorClass = CLASS_DEPTMODDISC;
        }
        return(ItemSalesDept(pRegSales, pItemSales));
    } else {
        /* ----- execute the Normal PLU procedure ----- */
        if (pRegSales->uchMinorClass == CLASS_UIMODDISC) {
            pItemSales->uchMinorClass = CLASS_PLUMODDISC;
        }
        sReturnStatus = ItemSalesPLU(pRegSales, pItemSales);
		if(pItemSales->uchMinorClass == CLASS_ITEMORDERDEC && sReturnStatus == SUCCESS)
		{
			TRANCURQUAL		*pWorkCur = TrnGetCurQualPtr();

			pWorkCur->uchOrderDec = pItemSales->uchAdjective;
			pWorkCur->fsCurStatus2 |= CURQUAL_ORDERDEC_DECLARED;
		}
        return sReturnStatus;
    }
}


/*
*===========================================================================
**Synopsis: SHORT   ItemSalesCheckKeyedPLU(UIFREGSALES *pUifRegSales);
*
*   Input:  None
*
*   Output: None
*
*   InOut:  UIFREGSALES *pUifRegSales
*
**Return:   ITM_SUCCESS     - function executes successfuly
*           LDT_SEQERR_ADR  - not set PLU number/ invalid adjective number
*
** Description: This function check keyed PLU and set PLU number.
*
*===========================================================================
*/
SHORT   ItemSalesCheckKeyedPLU(UIFREGSALES *pUifRegSales)
{
    PARAPLUNOMENU   KeyedPLU = {0};
	PARACTLTBLMENU  MenuRcvBuff = {0};    /* Menu page data receive buffer */
    SHORT           sReturnStatus = ITM_SUCCESS;
    TCHAR           auchDummy[NUM_PLU_LEN] = {0}; /* comparison buffer */

    /* ----- get current shift page ----- */
    MenuRcvBuff.uchMajorClass = CLASS_PARACTLTBLMENU;
    MenuRcvBuff.uchAddress = (UCHAR) (uchDispCurrMenuPage + 1);
    CliParaRead(&MenuRcvBuff);

    if (MenuRcvBuff.uchPageNumber == 1) {
        return(LDT_PROHBT_ADR);     /* error if prohibited to use */
    }

    /* ----- get dept number from keyed-plu table ----- */
    KeyedPLU.uchMajorClass     = CLASS_PARAPLUNOMENU;
    KeyedPLU.uchMinorClass     = CLASS_PARAPLUNOMENU_KEYEDPLU;
    KeyedPLU.uchMenuPageNumber = uchDispCurrMenuPage;
    KeyedPLU.uchMajorFSCData   = FSC_KEYED_PLU;
    KeyedPLU.uchMinorFSCData   = pUifRegSales->uchFsc;
    CliParaRead(&KeyedPLU);

    if (_tcsncmp(KeyedPLU.PluNo.aszPLUNumber, auchDummy, NUM_PLU_LEN) == 0) {
        return (LDT_SEQERR_ADR);
    } else {
        _tcscpy(pUifRegSales->aszPLUNo, KeyedPLU.PluNo.aszPLUNumber);
    }
#if 0
    if (pUifRegSales->uchAdjective) {           /* check Adjective number */
        if ((KeyedPLU.PluNo.uchAdjective != 0) &&
                (pUifRegSales->uchAdjective != KeyedPLU.PluNo.uchAdjective)) {
            sReturnStatus = LDT_SEQERR_ADR;
        }
    } else {

        /* ----- set adjective number ----- */

        pUifRegSales->uchAdjective = KeyedPLU.PluNo.uchAdjective;
    }
#endif
    if (KeyedPLU.PluNo.uchModStat & MOD_STAT_EVERSION) { /* E-Version label*/
        pUifRegSales->fbModifier2 |= EVER_MODIFIER; /* set E-Version */
    }
    /* R2.0 Start */
#if 0
    if (KeyedPLU.PluNo.uchModStat & MOD_STAT_VELOCITY) { /* UPC Velocity */
        pUifRegSales->fbModifier2 |= UPC_VEL_MODIFIER;
    }
#endif
    /* R2.0 End   */

    return (sReturnStatus);
}

/****** end of file ******/
