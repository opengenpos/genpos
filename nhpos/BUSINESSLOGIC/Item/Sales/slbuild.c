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
* Title       : PLU building module                          
* Category    : Item Module, NCR 2170 US Hsopitality Model Application         
* Program Name: SLBUILD.C
* --------------------------------------------------------------------------
* Abstract: ItemSalesStartBuilding()    :show building message, main entry point
*           ItemSalesPLUSetupBuilding() : setup plu status for buidling
*           ItemSalesPLUCreatBuilding() : create plu record on PLU module
*           ItemSalesMakeStanza()       : make cyclic tone
*           ItemSalesGetPriceOnly()     : get price only for building
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date   : Ver. Rev. :   Name      : Description
* Jul-24-15 : 02.02.01  : R.Chambers  : added ItemSalesGetPriceOnly()
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
#include    "appllog.h"
#include    "rfl.h"
#include    "fsc.h"
#include    "uireg.h"
#include    "regstrct.h"
#include    "transact.h"
#include    "item.h"
#include    "paraequ.h"
#include    "para.h"
#include    "csstbpar.h"
#include    "csop.h"
#include    "csstbopr.h"
#include    "display.h"
#include    "mld.h"
#include    "itmlocal.h"

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesStartBuilding(VOID);
*
*   Input:  None
*
*   Output: None
*
*   InOut:  UIFDIADATA  *pDiaData
*
**Return: UIFREG_SUCCESS : Function Performed succesfully
*         UIFREG_CANCEL  : Function Canceled by user using cancel key
*
** Description: Display the PLU building lead thru message to warn the operator
*               that the PLU bulding squence has started with the entry of an
*               unknown PLU number and request input.  The operator can either
*               continue the sequence by pressing the Clear key or can cancel
*               the sequence by pressing the Cancel key.
*
*===========================================================================
*/
static SHORT   ItemSalesStartBuilding(VOID)
{
	REGDISPMSG      DispMsg = {0};

    uchDispRepeatCo = 1;                    /* init repeat counter for display */

    /* ----- display "PLU Building" ----- */
    DispMsg.uchMajorClass = CLASS_REGDISPCOMPLSRY;
    DispMsg.uchMinorClass = CLASS_REGDISP_BUILDING;
	RflGetLead (DispMsg.aszMnemonic, LDT_PLUBLDOP_ADR);

    flDispRegDescrControl |= COMPLSRY_CNTRL;
    flDispRegKeepControl |= COMPLSRY_CNTRL;
    DispMsg.fbSaveControl = DISP_SAVE_TYPE_4;
        
    DispWrite(&DispMsg);
         
    flDispRegDescrControl &= ~COMPLSRY_CNTRL;
    flDispRegKeepControl &= ~COMPLSRY_CNTRL;

    if (UifDiaClear() != UIF_SUCCESS) {              /* wait CLEAR-KEY */
		RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGOTHER, CLASS_UICLEAR, DISP_SAVE_TYPE_0);
		return (ITM_CANCEL);                 /* cancel by user */
    }

    return(ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesPLUBuilding(UIFREGSALES *pUifRegSales,
*                                        ITEMSALES   *pItemSales);
*
*   Input:  None
*
*   Output: None
*
*   InOut:  UIFREGSALES *pUifRegSales
*           ITEMSALES   *pItemSales
*
**Return: ITM_SUCCESS        : Function Performed succesfully
*         ITM_CANCEL         : Function Canceled
*
** Description: This function do Building Operation.
*
*===========================================================================
*/
static SHORT ItemSalesPLUSetupBuilding(CONST UIFREGSALES *pUifRegSales, CONST ITEMSALES *pItemSales, PLUIF *pPluIf, CONST DEPTIF *pDeptIf)
{
    UCHAR  uchAdj = 1;    // default is not an adjective, PLU_NON_ADJ_TYP
    LONG   lUnitPrice;

    /* setup plu parameter from dept parameter */
    pPluIf->ParaPlu.ContPlu.usDept = pDeptIf->usDeptNo;
    if (pUifRegSales->uchAdjective) {
        uchAdj = pUifRegSales->uchAdjective;
        uchAdj = (UCHAR)((uchAdj - 1) / 5 + 1); /* adj. group */
        if (!((pDeptIf->ParaDept.auchControlCode[3] & PLU_USE_ADJG_MASK) & 
                (UCHAR) (PLU_USE_ADJG1 << (uchAdj - 1)))) {
            return(LDT_PROHBT_ADR);
        }

        uchAdj = pUifRegSales->uchAdjective;
        pPluIf->ParaPlu.ContPlu.uchItemType = PLU_ADJ_TYP;
    } else {
        uchAdj = 1;
        pPluIf->ParaPlu.ContPlu.uchItemType = PLU_NON_ADJ_TYP;
    }

    pPluIf->uchPluAdj = uchAdj;

    if (CliParaMDCCheckField (MDC_PLU2_ADR, MDC_PARAM_BIT_C)) {
        /* assume as quaintity */
        if ((pItemSales->ControlCode.auchPluStatus[2] & PLU_SCALABLE) == 0) {
            pPluIf->ParaPlu.uchPriceMulWeight = (UCHAR)(pItemSales->lQTY/1000L);
            if (pPluIf->ParaPlu.uchPriceMulWeight > PLU_MAX_PM) {
                return (LDT_KEYOVER_ADR);
            }
        }
    }

    lUnitPrice = pItemSales->lUnitPrice;
    if (lUnitPrice < 0L) {
        lUnitPrice *= -1L;
    }
    if (lUnitPrice > STD_PLU_MAX_PRICE) {

        return (LDT_KEYOVER_ADR);
    }
    RflConv4bto3b(pPluIf->ParaPlu.auchPrice, lUnitPrice);

    /* COPY CONTROL CODE INTO ITEMSALES */
    memcpy(pPluIf->ParaPlu.ContPlu.auchContOther, pDeptIf->ParaDept.auchControlCode, OP_PLU_SHARED_STATUS_LEN);
    pPluIf->ParaPlu.ContPlu.auchContOther[5] = pDeptIf->ParaDept.auchControlCode[4];  /* CRT No5 - No 8 see ItemSendKdsSetCRTNo() */

    /* Print Priority, R3.0 */
    pPluIf->ParaPlu.uchPrintPriority = pDeptIf->ParaDept.uchPrintPriority;
                                            /* set major dept No. */
    /* copy dept's mnemonics to pItemSales*/
    _tcsncpy(pPluIf->ParaPlu.auchPluName, pDeptIf->ParaDept.auchMnemonic, OP_DEPT_NAME_SIZE);
    memset (pPluIf->ParaPlu.auchAltPluName, 0, sizeof(pPluIf->ParaPlu.auchAltPluName));

    /*
		set building report code, V1.0.04
		set building group number, V2.0.4 for Compass dynamic PLU screens
	 */
    pPluIf->ParaPlu.ContPlu.uchRept = PLU_STANDARD_RPT_CODE;
	pPluIf->ParaPlu.ContPlu.usBonusIndex = pDeptIf->ParaDept.usBonusIndex;
	pPluIf->ParaPlu.uchGroupNumber = PLU_STANDARD_GROUP_NO;

    return (ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesPLUBuilding(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales, PLUIF *pPluIf);
*
*   Input:  None
*
*   Output: None
*
*   InOut:  UIFREGSALES *pUifRegSales
*           ITEMSALES   *pItemSales
*
**Return: ITM_SUCCESS        : Function Performed succesfully
*         ITM_CANCEL         : Function Canceled
*
** Description: This function is called when PLU does not exist to perform the PLU building procedure.
*               The PLU building procedure provides a way to create a new PLU based on the data entered
*               combined with the department data of the department number entered during
*               the PLU building sequence. 
*
*               We check that the operating environment allows us to perform the PLU building
*               and if it does we begin the PLU building sequence.
*
*               The first action of the sequence is to present the PLU building warning so
*               that an Operator can either press the Clear key to begin the procedure or
*               to press the Cancel key to end the procedure.  This allows the Operator
*               to cancel the PLU building sequence if an incorrect PLU was entered by mistake.
*
*               The next actions provide for the PLU price entry and the department entry.  The
*               department data is used to fill in the PLU control code information such as the
*               tax and discount itemizers, etc.
*
*===========================================================================
*/
SHORT ItemSalesPLUBuilding(CONST UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales, PLUIF *pData)
/* SHORT ItemSalesPLUBuilding(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales, PLUIF *pPluIf) */
{
    PLUIF       *pPluIf = pData;
    ULONG       ulPrice;
    USHORT      usDeptNo;
    SHORT       sReturnStatus;
    USHORT      usControl;
	DEPTIF      DeptIf = { 0 };

    if (CliParaMDCCheckField (MDC_PLU3_ADR, MDC_PARAM_BIT_B)) {
        /* prohibit building */
		NHPOS_ASSERT_TEXT((pUifRegSales->uchMajorFsc != FSC_KEYED_PLU), "ItemSalesPLUBuilding(): LDT_BLOCKEDBYMDC_ADR blocked by MDC setting");
        return (LDT_NTINFL_ADR);
    }
    if (pUifRegSales->uchMajorFsc == FSC_KEYED_PLU) {
		NHPOS_ASSERT_TEXT((pUifRegSales->uchMajorFsc != FSC_KEYED_PLU), "ItemSalesPLUBuilding(): LDT_NTINFL_ADR");
        return (LDT_NTINFL_ADR);
    }
    if (pUifRegSales->fbModifier & VOID_MODIFIER) {
		NHPOS_ASSERT_TEXT(((pUifRegSales->fbModifier & VOID_MODIFIER) == 0), "ItemSalesPLUBuilding(): LDT_VOID_NOALLOW_ADR with item void not allowed");
        return (LDT_VOID_NOALLOW_ADR);
    }
    if (pUifRegSales->fbModifier2 & PCHK_MODIFIER) {
		NHPOS_ASSERT_TEXT(((pUifRegSales->fbModifier2 & PCHK_MODIFIER) == 0), "ItemSalesPLUBuilding(): LDT_PROHBT_ADR with price check modifier");
        return (LDT_PROHBT_ADR);
    }
    if (TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)) { /* preselect void trans. */
		NHPOS_ASSERT_TEXT(((TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)) == 0), "ItemSalesPLUBuilding(): LDT_VOID_NOALLOW_ADR with trans void not allowed");
        return (LDT_VOID_NOALLOW_ADR);
    }
    if ((TranModeQualPtr->fbCashierStatus & MODEQUAL_CASHIER_TRAINING) || (TranModeQualPtr->fbWaiterStatus & MODEQUAL_WAITER_TRAINING)) {
		NHPOS_ASSERT_TEXT(((TranModeQualPtr->fbCashierStatus & MODEQUAL_CASHIER_TRAINING) == 0), "ItemSalesPLUBuilding() training operator not allowed");
		NHPOS_ASSERT_TEXT(((TranModeQualPtr->fbCashierStatus & MODEQUAL_WAITER_TRAINING) == 0), "ItemSalesPLUBuilding() training waiter not allowed");
        return (LDT_PROHBT_ADR);
    }

    /* ----- make a beep tone (long) and change scanner status ----- */
    if (CliParaMDCCheckField(MDC_PLU4_ADR, MDC_PARAM_BIT_C)) {
		STANZA      Stanza[10];
		PifPlayStanza(ItemSalesMakeStanza(Stanza)); /* make a pulse tone */
    } else {
        PifBeep(ITM_BEEP);                   /* make an error tone */
    }
    usControl = UieNotonFile(UIE_ENABLE);                   /* disable using scanner */

    /*-----
	 *      display PLU building operation message and
	 *      allow the operator to cancel the sequence.
	 *-----*/
    if (ItemSalesStartBuilding() == ITM_CANCEL) {
        UieNotonFile(usControl);                      /* enable scanner */
        return (ITM_CANCEL);
    }
    
    /*----- display lead-thru message and get first price and then department number -----*/
	sReturnStatus = ItemSalesGetPriceOnly (&ulPrice);
	if (sReturnStatus == ITM_SUCCESS) {
		sReturnStatus = ItemSalesGetDeptNoSans (&usDeptNo);
		if (sReturnStatus == UIF_CANCEL) sReturnStatus = ITM_CANCEL;
	}
	if (sReturnStatus != ITM_SUCCESS) {
		UieNotonFile(usControl);                      /* enable scanner */
		return (sReturnStatus);
	}

    /*----- GET DEPT. RECORD -----*/
    memset(&DeptIf, 0, sizeof(DeptIf));
    DeptIf.usDeptNo = usDeptNo;
    if ((sReturnStatus = CliOpDeptIndRead(&DeptIf, 0)) != OP_PERFORM) {
        UieNotonFile(usControl);                      /* enable scanner */
        return(OpConvertError(sReturnStatus));
    }
    
    /* velify unit price vs HALO amount */
    if (RflHALO(ulPrice, *DeptIf.ParaDept.auchHalo) != RFL_SUCCESS) {
        UieNotonFile(usControl);                      /* enable scanner */
        return(LDT_KEYOVER_ADR);        /* return sequence error */
    }

    /* setup plu parameter from dept parameter */
    pItemSales->lUnitPrice = ulPrice;
    if ((sReturnStatus = ItemSalesPLUSetupBuilding(pUifRegSales, pItemSales, pPluIf, &DeptIf)) != ITM_SUCCESS) {
        UieNotonFile(usControl);                      /* enable scanner */
        return (sReturnStatus);
    }
    
    pItemSales->fsLabelStatus |= ITM_CONTROL_NOTINFILE;
    UieNotonFile(usControl);                      /* enable scanner */

    return (ITM_SUCCESS);
}


SHORT   ItemSalesGetPriceOnly(ULONG *pulPrice)
{
	UIFDIADATA      WorkUI = {0};
    REGDISPMSG      DispMsg = {0};

    /* ----- display "ENTER PRICE" ----- */
    DispMsg.uchMajorClass = CLASS_REGDISPCOMPLSRY;
    DispMsg.uchMinorClass = CLASS_REGDISP_PCHECK;
    RflGetLead (DispMsg.aszMnemonic, LDT_PP_ADR);

    flDispRegDescrControl |= COMPLSRY_CNTRL;
    flDispRegKeepControl |= COMPLSRY_CNTRL;
    DispMsg.fbSaveControl = DISP_SAVE_TYPE_4;
        
    DispWrite(&DispMsg);
         
    flDispRegDescrControl &= ~COMPLSRY_CNTRL;
    flDispRegKeepControl &= ~COMPLSRY_CNTRL;

    if (UifDiaPrice(&WorkUI)) {               /* req. price  */
        return (ITM_CANCEL);                 /* cancel by user */
    }

    if (WorkUI.auchFsc[0] == FSC_PRICE_ENTER) {
        *pulPrice = WorkUI.ulData;                    
    } else {
        return (LDT_SEQERR_ADR);
    }   

    return(ITM_SUCCESS);
}


/*
*===========================================================================
**Synopsis: SHORT   ItemSalesPLUBuilding(UIFREGSALES *pUifRegSales,
*                                        ITEMSALES   *pItemSales);
*
*   Input:  None
*
*   Output: None
*
*   InOut:  UIFREGSALES *pUifRegSales
*           ITEMSALES   *pItemSales
*
**Return: ITM_SUCCESS        : Function Performed succesfully
*         ITM_CANCEL         : Function Canceled
*
** Description: This function do Building Operation.
*
*===========================================================================
*/
SHORT ItemSalesPLUCreatBuilding(CONST UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales)
{
    DEPTIF DeptIf = {0};
	PLUIF  PluIf = {0};
    SHORT  sReturn;
    UCHAR  uchAdjMax, uchAdj, i;
    LONG   lUnitPrice;
    
    /*----- GET DEPT. RECORD -----*/
    DeptIf.usDeptNo = pItemSales->usDeptNo;
    if ((sReturn = CliOpDeptIndRead(&DeptIf, 0)) != OP_PERFORM) {
        return(OpConvertError(sReturn));
    }

    _tcsncpy(PluIf.auchPluNo, pItemSales->auchPLUNo, NUM_PLU_LEN);

    if ((sReturn = ItemSalesPLUSetupBuilding(pUifRegSales, pItemSales, &PluIf, &DeptIf)) != ITM_SUCCESS) {
        return(sReturn);
    }
    
    lUnitPrice = pItemSales->lUnitPrice;
    if (lUnitPrice < 0L) {
        lUnitPrice *= -1L;
    }
    if (lUnitPrice > STD_PLU_MAX_PRICE) {
        return (LDT_KEYOVER_ADR);
    }
    uchAdjMax = 1;
    if ((PluIf.ParaPlu.ContPlu.uchItemType & PLU_TYPE_MASK_REG) == PLU_ADJ_TYP) {
        uchAdjMax = 5;
    }
    uchAdj = PluIf.uchPluAdj;
    for (i = 1; i <= uchAdjMax; i++) {
        PluIf.uchPluAdj = i;
        if (i == uchAdj) {
            RflConv4bto3b(PluIf.ParaPlu.auchPrice, lUnitPrice);
        } else {
            RflConv4bto3b(PluIf.ParaPlu.auchPrice, 0L); /* set other adjective price as 0 */
        }
        sReturn = CliOpPluAssign(&PluIf, 0);
        if (sReturn == OP_PLU_FULL) {
            for (i -= 1; i > 0; i--) {
                PluIf.uchPluAdj = i;
                CliOpPluDelete(&PluIf, 0);
            }
            return (LDT_FLFUL_ADR);
        } else if ((sReturn != OP_NEW_PLU) && (sReturn != OP_CHANGE_PLU)) {
            return (OpConvertError(sReturn));
        }
    }

    pItemSales->fsLabelStatus |= ITM_CONTROL_NOTINFILE;
    return (ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: STANZA  *ItemSalesMakeStanza(STANZA *pStanza);
*
*   Input:  STANZA  *pStanza    (STANZA Stanza[10] : prepare 10 item fields)
*
*   Output: None
*
*   InOut:  None
*
**Return: STANZA    *pStanza
*
** Description: This function creates a STANZA parameters suitable for
*               the PifPlayStanza() function in the user provided area
*               and returns the address of the area.
*
*               Typically used as PifPlayStanza(ItemSalesMakeStanza(Stanza));
*               See as well function UieQueueStanzaToPlay() which allows a STANZA
*               to be put into the play queue to be played by the thread of
*               function UiePlayStanzaThread().
*===========================================================================
*/
STANZA  *ItemSalesMakeStanza(STANZA *pStanza)
{
    SHORT   i;

    for (i = 0; i < 9; i++) {
        (pStanza + i)->usDuration = 100;        /* set sound time */
        if ((! i) || (! (i % 2))) {
            (pStanza + i)->usFrequency = 19;    /* set frequency(not sound)*/
        } else {
            (pStanza + i)->usFrequency = 1500;  /* set frequency (sound) */
        }
    }
    (pStanza + i)->usDuration  = 0;             /* end stanza */
    (pStanza + i)->usFrequency = 0;

    return (pStanza);
}

/****** end of file ******/
