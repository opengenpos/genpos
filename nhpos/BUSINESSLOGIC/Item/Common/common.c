/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Item common module                         
* Category    : Item Common Module, NCR 2170 US Hospitality Model Application         
* Program Name: COMMON.C
* --------------------------------------------------------------------------
* Abstract: ItemSPVInt()
*           ItemCurPrintStatus()
*           ItemCountCons()
*           ItemDrawer()
*           ItemTrailer()
*           ItemNormalTrailer()
*           ItemTicketTrailer()
*           ItemPromotion()
*           ItemHeader()
*           ItemInit()
*           ItemFinalize()
*           ItemDispRel()
*           ItemCommonCheckComp()
*           ItemPrintStart()
*           ItemCommonCheckExempt()    R3.0:Change
*           ItemCommonCancelExempt()
*           ItemCommonGetStatus()
*           ItemCommonPutStatus()
*           ItemCommonCheckScaleDiv()
*           ItemCommonGetScaleData()
*           ItemCommonReadScale()
*           ItemCommonCheckScaleData()
*           
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver. Rev. : Name      : Description
*  2/14/95 : 03.00.00  : hkato     : R3.0
* 11/20/95 : 03.01.00  : hkato     : R3.1
* 06/09/98 : 03.03.00  : M.Ozawa   : Add ItemCommonGetSRAutoNo() and
*                                   ItemCommonTransOpen() for transaction open
*                                   migration.
* 
*** NCR2171 **
* 08/26/99 : 01.00.00  : M.Teraki    : initial (for Win32)
* 12/07/99 : 01.00.00  : hrkato      : Saratoga
*** GenPOS **
* 09/13/15 : 02.02.01  : R.Chambers  : clean up based on inspection and VS 2013 analysis tool
* 12/05/16 : 02.02.02  : R.Chambers  : change interface function ItemCommonCheckScaleDiv() to not use command flag.
* 12/05/16 : 02.02.02  : R.Chambers  : moved net weight calculation from ItemCommonCheckScaleData() up to ItemCommonGetScaleData().
* 09/20/16 : 02.02.02  : R.Chambers  : added new function ItemCommonSetDisplayFlagExempt() to consolidate that decision.
* 06/19/18 : 02.02.02  : R.Chambers  : added new function ItemCheckAndSetTransDate() used in ItemSalesSalesRestriction() for date.
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
#include    <stdlib.h>
#include    <stdio.h>
#include    <math.h>

#include    "ecr.h"
#include    "pif.h"
#include    "log.h"
#include    "plu.h"
#include    "appllog.h"
#include    "rfl.h"
#include    "uie.h"
#include    "fsc.h"
#include    "uireg.h"       
#include    "pmg.h"
#include    "regstrct.h"
#include    "transact.h"
#include    "item.h"
#include    "paraequ.h"
#include    "para.h"
#include    "pararam.h"
#include    "csstbpar.h"
#include    "cscas.h"
#include    "csgcs.h"
#include    "csstbgcf.h"
#include    "csop.h"
#include    "csstbopr.h"
#include    "display.h"
#include    "mld.h"
#include    "fdt.h"
#include    "csetk.h"
#include    "csstbetk.h"
#include    "csstbstb.h"
#include    "cpm.h"
#include    "itmlocal.h"
#include    "trans.h"
#include    "BlFWif.h"
#include    "maint.h"

ITEMCOMMONLOCAL ItemCommonLocal;    /* Common module local data */
                
extern  PifSemHandle  usShrSemDrawer;     /* semaphore for shared prit & drawer open */

static SHORT   ItemCommonCheckScaleData(ITEMMODSCALE *pData1, UIE_SCALE *pData2);

/*
*===========================================================================
**Synopsis: VOID   ItemCommonGetLocal(ITEMCOMMONLOCAL *pData)
**Synopsis: VOID   ItemCommonPutLocal(ITEMCOMMONLOCAL *pDataBuffer)
*
*    Input: ITEMCOMMONLOCAL *pDataBuffer 
*
*   Output: pDataBuffer->fbCommonStatus
*           pDataBuffer->uchItemNo
*           pDataBuffer->ItemSales
*           pDataBuffer->ItemDisc
*
*    InOut: Nothing
*
**Return: VOID
*         
** Description: Set common module local datas in read buffer.  
** Description: Set common module local datas in write buffer.  
*                
*===========================================================================
*/

CONST volatile ITEMCOMMONLOCAL * CONST ItemCommonLocalPtr = &ItemCommonLocal;

ITEMCOMMONLOCAL *ItemCommonGetLocalPointer(VOID) 
{
	return &ItemCommonLocal;
}

VOID   ItemCommonGetLocal(ITEMCOMMONLOCAL **pDataBuffer) 
{
	*pDataBuffer = &ItemCommonLocal;
}

VOID   ItemCommonPutLocal(ITEMCOMMONLOCAL *pDataBuffer) 
{
    ItemCommonLocal = *pDataBuffer;     
}

/*
 *  Following const pointers are used to allow functions to reference the current
 *  transaction item data so as to make decisions without the risk of modifying the
 *  data that is stored in the area.
*/
CONST volatile ITEMSALES * CONST ItemCommonLocalItemUnionSalesPtr = &ItemCommonLocal.ItemSales;

/*
 *  ItemCommonGetLocalItemUnionPointer(VOID) returns a pointer to the
 *  anonymous union that contains the current transaction item data of
 *  the last item that has been keyed in but has not yet been stored.
 *
 *  This data may be an ITEMSALES item data or it may one of the other
 *  transaction item types such as ITEMTENDER.
 *
*/
VOID *ItemCommonGetLocalItemUnionPointer(VOID) 
{
	return &ItemCommonLocal.ItemSales;
}


/*
*===========================================================================
**Synopsis: SHORT   ItemSPVInt( UCHAR uchLeadAddr )   
*
*    Input: UCHAR   uchLeadAddr
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return:   UIF_SUCCESS : Supr. Intervention was executed successfully.
*           UIF_CANCEL  : Supr. Intervention was canceled by clear key.
*
** Description: Supervisor intervention process.
*                
*===========================================================================
*/
SHORT   ItemSPVIntDialog( UCHAR uchLeadAddr, UCHAR uchDialogLeadAddr )
{
	ULONG           flDisplaySettings = ( VOID_CNTRL | EC_CNTRL | ITEMDISC_CNTRL | REGDISC_CNTRL
										| CHRGTIP_CNTRL | CRED_CNTRL | TAXMOD_CNTRL | TAXEXMPT_CNTRL | PO_CNTRL | ROA_CNTRL 
										| TIPSPO_CNTRL | DECTIP_CNTRL | CHANGE_CNTRL | ADJ_CNTRL );
	REGDISPMSG      DisplayData = {0};       
	SHORT           sReturnStatus;

    /*--- DISPLAY LEADTHROUGH ---*/
    DisplayData.uchMajorClass = CLASS_REGDISPCOMPLSRY; 
    DisplayData.uchMinorClass = CLASS_REGDISP_SUPERINT;

    /* get leadthrough mnemonics of Supr. Int. */
    RflGetLead (DisplayData.aszMnemonic, uchLeadAddr);

    flDispRegDescrControl |= COMPLSRY_CNTRL;         /* turn on descriptor control flag */
    DisplayData.fbSaveControl = DISP_SAVE_TYPE_4;    /* display previous data at redisplay */
    DispWrite(&DisplayData);                         /* call display module */
    flDispRegDescrControl &= ~COMPLSRY_CNTRL;        /* turn off descriptor control flag */

	if (uchDialogLeadAddr != 0) {
		UieErrorMsg (uchDialogLeadAddr, (UIE_EM_TONE | UIE_EM_SUPR_TONE));
	}
	else {
		PifBeep(UIE_SPVINT_TONE);
	}

    /*--- EXECUTE SUPR INTERVENTION ---*/
	do{
		sReturnStatus = UifSupIntervent();
		if (sReturnStatus == UIFREG_ABORT)
		{
			sReturnStatus = UIF_CANCEL;
			break;
		}
		if (sReturnStatus == UIF_SUCCESS) 
		{
			break;
		} else 
		{
			/*--- DISPLAY  FOR CANCELATION ---*/
			memset(&DisplayData, '\0', sizeof(DisplayData)); 
			DisplayData.uchMajorClass = CLASS_REGDISPCOMPLSRY; 
			DisplayData.uchMinorClass = CLASS_REGDISP_SUPERINT;
			DisplayData.fbSaveControl = (DISP_SAVE_ECHO_ONLY | DISP_SAVE_TYPE_0); 
        
			flDispRegDescrControl &= ~ flDisplaySettings;   /* clear descriptor for flDispRegDescrControl */
			flDispRegKeepControl &= ~ flDisplaySettings;    /* clear descriptor for flDispRegKeepControl */
        
			DispWrite(&DisplayData);                     /* send background data */
			sReturnStatus = UIF_CANCEL;

			RflGetLead (DisplayData.aszMnemonic, uchLeadAddr);
			flDispRegDescrControl |= COMPLSRY_CNTRL;         /* turn on descriptor control flag */
			DisplayData.fbSaveControl = DISP_SAVE_TYPE_4;    /* display previous data at redisplay */
			DispWrite(&DisplayData);                         /* call display module */
			flDispRegDescrControl &= ~COMPLSRY_CNTRL;        /* turn off descriptor control flag */
		}
	} while(1);

	RflClearDisplay (0, 0, 0);
    return(sReturnStatus);
}

SHORT   ItemSPVInt( UCHAR uchLeadAddr )
{
	SHORT   sRetStatus = ITM_SUCCESS;

    /* request once in a transaction */
    if ((CliParaMDCCheckField (MDC_INTERVENE_ADR, MDC_PARAM_BIT_D) == 0) && (ItemCommonLocal.fbCommonStatus & COMMON_SPVINT)) { 
        /* already done */
        return(ITM_SUCCESS);
    }

	sRetStatus = ItemSPVIntDialog(uchLeadAddr, 0);
	if (sRetStatus == UIF_SUCCESS) {
		ItemCommonLocal.fbCommonStatus |= COMMON_SPVINT;
	}

	return sRetStatus;
}

SHORT   ItemSPVIntAlways( UCHAR uchLeadAddr )
{
	SHORT   sRetStatus = ITM_SUCCESS;

	sRetStatus = ItemSPVIntDialog(uchLeadAddr, 0);
	if (sRetStatus == UIF_SUCCESS) {
		ItemCommonLocal.fbCommonStatus |= COMMON_SPVINT;
	}

	return sRetStatus;
}

/*
*===========================================================================
**Synopsis: VOID    ItemCurPrintStatus(VOID)
*
*    Input: VOID 
*   Output: Nothing
*    InOut: Nothing
**Return: VOID
*         
** Description: Set print status bit of current qualifier.          V3.3
*===========================================================================
*/
VOID    ItemCurPrintStatus(VOID)
{
    UCHAR           uchRcvBuff;
    UCHAR           uchSavedMDC;
    TRANCURQUAL     *pCurQualRcvBuff = TrnGetCurQualPtr();
	PARASHAREDPRT   ParaSharedPrt = {0};

    /*--- GET G.C SYSTEM PARAMETER ---*/
    pCurQualRcvBuff->flPrintStatus &= ~ CURQUAL_GC_SYS_MASK;    // clear the area then set the system type.
    pCurQualRcvBuff->flPrintStatus |= (ULONG) RflGetSystemType () << 4;

    /* check shared printer system */
    ParaSharedPrt.uchMajorClass = CLASS_PARASHRPRT;
    ParaSharedPrt.uchAddress = PARASHAREDPRT_SHARED(CliReadUAddr());  // address of shared of shared/alternate printer pair
    CliParaRead(&ParaSharedPrt);
    if ((ParaSharedPrt.uchTermNo) &&
        (ParaSharedPrt.uchTermNo <= MAX_TERMINAL_NO) &&                
        ((pCurQualRcvBuff->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_UNBUFFER)) {    
        pCurQualRcvBuff->flPrintStatus &= ~CURQUAL_PRE_UNBUFFER;      /* Shared Printer and Unbuffer System */
        pCurQualRcvBuff->flPrintStatus |= CURQUAL_PRE_BUFFER;
    }

    if (!CliParaMDCCheckField (MDC_KPTR_ADR, MDC_PARAM_BIT_C)) {  /* issue take to kitchen */
        pCurQualRcvBuff->flPrintStatus |= CURQUAL_TAKEKITCHEN;    /* set "issue take to kitchen status */   
    }
    if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
        uchRcvBuff = CliParaMDCCheck(MDC_CANTAX_ADR, (EVEN_MDC_BIT2 | EVEN_MDC_BIT1));
        if (uchRcvBuff == 0x00) {               /* set canadian tax print status */
            pCurQualRcvBuff->flPrintStatus |= CURQUAL_CANADA_ALL;
        } else if (uchRcvBuff == 0x01) {
            pCurQualRcvBuff->flPrintStatus |= CURQUAL_CANADA_GST_PST;
        } else {
            pCurQualRcvBuff->flPrintStatus |= CURQUAL_CANADA_INDI; 
        }
    }
    if (CliParaMDCCheckField (MDC_DEPT2_ADR, MDC_PARAM_BIT_D)) {   /* print child PLU */ 
        pCurQualRcvBuff->flPrintStatus |= CURQUAL_SETMENU;         /* print child PLU */
    }

    if (CliParaMDCCheckField(MDC_DEPT2_ADR, MDC_PARAM_BIT_C)) {   
        pCurQualRcvBuff->flPrintStatus |= CURQUAL_SETMENU_KITCHEN;  /* print child PLU (K/P) */
    }
    
    /* if (FlexMemRcvBuff.uchPTDFlag == FLEX_PRECHK_UNBUFFER && */
        
	/* only if unbuffering print system and if compulsory slip at unbuffering print */
    if ((pCurQualRcvBuff->flPrintStatus & CURQUAL_PRE_UNBUFFER) && (CliParaMDCCheckField (MDC_SLIP_ADR, MDC_PARAM_BIT_D))) {
        pCurQualRcvBuff->fbCompPrint |= CURQUAL_COMP_S;          /* set compulsory bit of Curr. Qual */
    }                                                                                        

    uchSavedMDC = CliParaMDCCheck(MDC_DRAWER_ADR, (EVEN_MDC_BIT2 | EVEN_MDC_BIT3));
    if (uchSavedMDC & EVEN_MDC_BIT2) {      /* MDC address 14 */
        pCurQualRcvBuff->flPrintStatus |= CURQUAL_DDMMYY;     
    }
    if (uchSavedMDC & EVEN_MDC_BIT3) {      /* MDC address 14 */
        pCurQualRcvBuff->flPrintStatus |= CURQUAL_MILITARY;   /* military time option */
    }

    uchSavedMDC = CliParaMDCCheck(MDC_SLIP_ADR, (EVEN_MDC_BIT1 | EVEN_MDC_BIT2 | EVEN_MDC_BIT3));
    if (uchSavedMDC & EVEN_MDC_BIT1) {
        pCurQualRcvBuff->flPrintStatus |= CURQUAL_NOT_SLIP_PROM;   
    }

    if (uchSavedMDC & EVEN_MDC_BIT2) {      /* MDC address 20, 1: not print promotion */ 
        pCurQualRcvBuff->flPrintStatus |= CURQUAL_NOT_PRINT_PROM;   
    }
    
    if (uchSavedMDC & EVEN_MDC_BIT3) {      /* MDC address 20, 1: not  provide electro */
        pCurQualRcvBuff->flPrintStatus |= CURQUAL_NO_ELECTRO;   
    }
    
    if (CliParaMDCCheckField (MDC_CHKSUM_ADR, MDC_PARAM_BIT_D)) {
        pCurQualRcvBuff->flPrintStatus |= CURQUAL_SINGLE_CHCK_SUM;    /* MDC address 62, 1: print promotion */
    }
    
    if (CliParaMDCCheckField (MDC_GCNO_ADR, MDC_PARAM_BIT_C)) {
        pCurQualRcvBuff->flPrintStatus |= CURQUAL_CD_GCF_NO;          /* MDC address 12, 1: generate CD for G.C No */ 
    }
    
    if (CliParaMDCCheckField(MDC_SCALE_ADR, MDC_PARAM_BIT_D)) { 
        pCurQualRcvBuff->flPrintStatus |= CURQUAL_SCALE3DIGIT;       /* MDC address 29, 0: 0.001 kg/lb autoscale */
    }

    /*--- TURN ON TRAINING STATUS ---*/
    if ( ( TranModeQualPtr->fbCashierStatus & MODEQUAL_CASHIER_TRAINING )
        || ( TranModeQualPtr->fbWaiterStatus & MODEQUAL_WAITER_TRAINING ) )
	{
        pCurQualRcvBuff->fsCurStatus |= CURQUAL_TRAINING;    /* training transaction */
    }
    
    /*--- PRT_RECEIPT/ NO REC DEFAULT ---*/
    pCurQualRcvBuff->fbNoPrint &= ~CURQUAL_NO_R;       /* assume we print receipt unless MDC says otherwise */
    if ( TranModeQualPtr->fsModeStatus & MODEQUAL_NORECEIPT_DEF ) {
        if ( ! ( ItemModLocalPtr->fbModifierStatus & MOD_NORECEIPT ) )
		{
            pCurQualRcvBuff->fbNoPrint |= CURQUAL_NO_R;
        } 
    } else {                                /* default is receipt */
        if ( ItemModLocalPtr->fbModifierStatus & MOD_NORECEIPT) {
            pCurQualRcvBuff->fbNoPrint |= CURQUAL_NO_R;
        }
    }

	if ((pCurQualRcvBuff->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)) != 0) {
		pCurQualRcvBuff->fbNoPrint &= ~CURQUAL_NO_R;  // if a transaction return or transaction void, print anyway.
	}

    if ( CliParaMDCCheckField ( MDC_VALIDATION_ADR, MDC_PARAM_BIT_D ) ) { 
        if ( CliParaMDCCheckField ( MDC_VALIDATION_ADR, MDC_PARAM_BIT_C ) ) { 
            pCurQualRcvBuff->flPrintStatus |= CURQUAL_VAL_TYPE3;      /* cons#, time, date */
        } else {
            pCurQualRcvBuff->flPrintStatus |= CURQUAL_VAL_TYPE2;      /* cons#, id, date */
        }
    } else { 
        pCurQualRcvBuff->flPrintStatus |= CURQUAL_VAL_TYPE1;          /* cons#, id, time */
    }        

    if (CliParaMDCCheckField ( MDC_PRINTPRI_ADR, MDC_PARAM_BIT_D ) && CliParaMDCCheckField ( MDC_PRINTPRI_ADR, MDC_PARAM_BIT_B ) ) {     /* R3.0 */
        pCurQualRcvBuff->flPrintStatus |= CURQUAL_PMCOND_SORT;
    }
    if (CliParaMDCCheckField ( MDC_PRINTPRI_ADR, MDC_PARAM_BIT_D)) {        /* R3.0 */
        pCurQualRcvBuff->flPrintStatus |= CURQUAL_PRINT_PRIORITY;
    }

    pCurQualRcvBuff->flPrintStatus &= ~CURQUAL_UNIQUE_TRANS_NO;    /* unique trans# */
    if ( CliParaMDCCheckField ( MDC_TRANNUM_ADR, MDC_PARAM_BIT_D )) {
		// we are using MDC 234 Bit D to decide if we are to use a Unique Transaction Number or not.
		// however this also controls whether the guest check number is printed double size or not.
		// if MDC 234 Bit C is off then print standard size if MDC 234 Bit D is off as well.
		// if MDC 234 Bit C is on then
		//   - if MDC 234 Bit D is off, no printing of Unique Transaction Number and print single size guest check number
		//   - if MDC 234 Bit D is on, no printing of Unique Transaction Number but print double size guest check number
        pCurQualRcvBuff->flPrintStatus |= CURQUAL_UNIQUE_TRANS_NO;    /* unique trans# */
    }

    /*--- NOT PRINT VAT, VATABLE (at included vat) V3.3---*/
    if (ItemCommonTaxSystem() == ITM_TAX_INTL) {
        if ( !( CliParaMDCCheckField ( MDC_VAT1_ADR, MDC_PARAM_BIT_B ) ) ) {
            if ( !( CliParaMDCCheckField ( MDC_VAT2_ADR, MDC_PARAM_BIT_D ) ) ) {
                pCurQualRcvBuff->flPrintStatus |= CURQUAL_NOT_PRINT_VAT;
                pCurQualRcvBuff->flPrintStatus |= CURQUAL_NOT_PRINT_VATAPP;
            } else {
                if ( !( CliParaMDCCheckField( MDC_VAT2_ADR, MDC_PARAM_BIT_C ) ) ) {
                    pCurQualRcvBuff->flPrintStatus |= CURQUAL_NOT_PRINT_VATAPP;
                }
            }
        } else {
            if ( !( CliParaMDCCheckField ( MDC_VAT2_ADR, MDC_PARAM_BIT_C ) ) ) {
				pCurQualRcvBuff->flPrintStatus |= CURQUAL_NOT_PRINT_VATAPP;
            }
        }
        /*--- Excluded VAT ---*/
        if ( CliParaMDCCheckField ( MDC_VAT1_ADR, MDC_PARAM_BIT_B ) ) {
            pCurQualRcvBuff->flPrintStatus |= CURQUAL_EXCLUDE_VAT;
        }
        /*--- NOT PRINT SERVICE (at included service) ---*/
        if ( !( CliParaMDCCheckField( MDC_VAT1_ADR, MDC_PARAM_BIT_A ) ) ) {
            if ( !( CliParaMDCCheckField( MDC_VAT2_ADR, MDC_PARAM_BIT_B ) ) ) {
                pCurQualRcvBuff->flPrintStatus |= CURQUAL_NOT_PRINT_SER;
            }
        }
        /*--- Excluded Service ---*/
        if ( CliParaMDCCheckField( MDC_VAT1_ADR, MDC_PARAM_BIT_A ) ) {
            pCurQualRcvBuff->flPrintStatus |= CURQUAL_EXCLUDE_SERV;
        }
    }

    /*--- CLEAR SAVED M.I OF TENDER LOCAL DATA ---*/
    ItemTenderGetLocalPointer()->lSavedMI = 0L;
}


/*
*===========================================================================
**Synopsis: VOID    ItemCountCons( VOID )
*
*    Input: VOID 
*
*   Output: None
*
*    InOut: None
*
**Return: 
*         
*
** Description: Increment or initialize with 1 consective No. 
*               Set Store/ Reg No. to Current Qualifier. 
*                
*===========================================================================
*/
VOID    ItemCheckAndSetTransDate (VOID)
{
	// We provide this function to be used in places where a transaction date is needed
	// yet the transaction has not yet been started.
	// A transaction will be started after the first item is rung up.
	// This means some actions such as the Sales Restriction check in
	// function ItemSalesSalesRestriction() may require the transaction time/date when
	// the transaction has not actually been started because the check is being
	// done with the first item of the transaction.
	//    Richard Chambers, Jun-19-2018, Rel 2.2.2.010
    if (!(ItemCommonLocal.fbCommonStatus & COMMON_CONSNO)) {
		DATE_TIME DateTime;

		PifGetDateTime(&DateTime);

		/*Added for SR 215, It seems that whenever a Keyed Dept Number,
		NHPOS never set the TOD for the transaction, which can cause problems for 
		sales restricted items that use the TOD to determine wether or not to sell the item*/
		ItemSalesLocal.Tod = DateTime;      // Set the time and date of the transaction for use with age restriction check.
	}
}

VOID    ItemCountCons( VOID )
{
	TRANGCFQUAL    *TranGCFQualWrk = TrnGetGCFQualPtr();
	TRANCURQUAL    *pCurQualRcvBuff = TrnGetCurQualPtr();

    if (ItemCommonLocal.fbCommonStatus & COMMON_CONSNO) {
		NHPOS_ASSERT_TEXT((TranGCFQualWrk->usConsNo), "(ItemCommonLocal.fbCommonStatus & COMMON_CONSNO)");
    	NHPOS_ASSERT_TEXT((TranGCFQualWrk->ulStoreregNo), "(ItemCommonLocal.fbCommonStatus & COMMON_CONSNO)");
	}

    /* check whether consectine No. already counted up or not */
    if (!(ItemCommonLocal.fbCommonStatus & COMMON_CONSNO)) {  /* consecutive No isn't counted up yet so we need to increment */
		ULONG          ulStoreRegNo;
		PARASTOREGNO   StRegNoRcvBuff; 
		USHORT         usCounter = MaintIncreSpcCo(SPCO_CONSEC_ADR);         /* increment the consective No. and gets its new value */

        StRegNoRcvBuff.uchMajorClass = CLASS_PARASTOREGNO;    /* get store/ reg No. */
        StRegNoRcvBuff.uchAddress = SREG_NO_ADR;
        CliParaRead(&StRegNoRcvBuff);
        
        ulStoreRegNo = (ULONG) StRegNoRcvBuff.usStoreNo * 1000L + (ULONG) StRegNoRcvBuff.usRegisterNo;
        
        TranGCFQualWrk->usConsNo = pCurQualRcvBuff->usConsNo = usCounter;                /* set the consecutive No. in memory resident data */
        TranGCFQualWrk->ulStoreregNo = pCurQualRcvBuff->ulStoreregNo = ulStoreRegNo;     /* set store/ reg. No. in memory resident data */
		TranGCFQualWrk->uchHourlyOffset = (UCHAR)ItemTendGetOffset();                    /* set the hourly total block offset for the guest check */
		TranGCFQualWrk->ulCashierID = TranModeQualPtr->ulCashierID;                      /* set the cashier id in ItemCountCons() as part of starting transaction */

		// make sure that the date time stamp used with receipt trailers is zeroed out so that the
		// proper date and time will be inserted into the trailer.  See function PrtGetDate().
		memset (&TranGCFQualWrk->DateTimeStamp, 0, sizeof(TranGCFQualWrk->DateTimeStamp));  // clear the date time stamp used in trailer.

		{
			// clear the transaction bonus itemizers which accumulate bonus amounts for the current
			// transaction.  GOV PAY and some other systems want to bucketize puchase amounts into
			// categories.
			TRANITEMIZERS   *pWorkTI = TrnGetTIPtr();
		               
			memset (pWorkTI->lTransBonus, 0, sizeof(pWorkTI->lTransBonus));
		}
		/*
			Generate a unique transaction identifier number.  This number is all digits and should provide a unique transaction
			number to be used in a variety of circumstances.  The number is stored as a series of binary digits each digit stored
			in a UCHAR array element.  The binary digits are transformed back into character digits at the time they are needed
			to print.
		**/
		{
			SHORT     i;
			ULONG     ulCombinedDate, ulCombinedTime;
			DATE_TIME DateTime;
			char auchTempBuf[32];

			PifGetDateTime(&DateTime);

			ulCombinedTime = (DateTime.usMin * 60 + DateTime.usSec) * 24 + DateTime.usHour;
			ulCombinedDate = (DateTime.usMonth * 32 + DateTime.usMDay) * 100 + (DateTime.usYear%100);

			// set the uchUniqueIdentifer array with the following format of:
			//    4 digit store number
			//    3 digit register number
			//    4 digit consecutive number (SPCO_CONSEC_ADR)
			//    5 digit compressed date
			//    5 digit compressed time
			//    2 digit tender id which is set to zero initially
			sprintf (auchTempBuf, "%4.4d%3.3d%4.4d%5.5d%5.5d00", (StRegNoRcvBuff.usStoreNo % 10000), (StRegNoRcvBuff.usRegisterNo % 1000),
						(usCounter % 10000), ulCombinedDate, ulCombinedTime);

			memset(TranGCFQualWrk->uchUniqueIdentifier, 0, sizeof(TranGCFQualWrk->uchUniqueIdentifier));
			for (i = 0; auchTempBuf[i] && i < sizeof(TranGCFQualWrk->uchUniqueIdentifier)/sizeof(TranGCFQualWrk->uchUniqueIdentifier[0]) - 1; i++) {
				TranGCFQualWrk->uchUniqueIdentifier[i+1] = auchTempBuf[i] - '0';
				if (TranGCFQualWrk->uchUniqueIdentifier[i+1] > 9)
					TranGCFQualWrk->uchUniqueIdentifier[i+1] = 0;
			}

			// since the returns tender table is updated when doing a transaction, we will
			// need to initialize it to a beginning state when starting any transaction
			// so that the table will be properly updated when doing transactions
			// even though the table is ignored except when doing a return.
			CliGusLoadTenderTableInit(0);  // init the tender table used by returns

			if ((TranGCFQualWrk->fsGCFStatus2 & GCFQUAL_EPT_HASH_MADE) == 0) {
				/*
					Generate the Electronic Payment Interface invoice number used to identify the transaction
					that was involved in the electronic payment action.  This invoice number must be generated
					only once during the life of a transaction no matter how many times it has been serviced.
					The Pre-Auth Capture functionality with its encrypted key requires that the hash key does
					not change once the key has been created and used.
				**/
				TrnGenerateTransactionInvoiceNum (0);

				TranGCFQualWrk->auchCheckOpenTime[0] = TranGCFQualWrk->auchCheckOpen[0] = (UCHAR)DateTime.usHour;
				TranGCFQualWrk->auchCheckOpenTime[1] = TranGCFQualWrk->auchCheckOpen[1] = (UCHAR)DateTime.usMin;
				TranGCFQualWrk->auchCheckOpenTime[2] = TranGCFQualWrk->auchCheckOpen[2] = (UCHAR)DateTime.usSec;
				TranGCFQualWrk->auchCheckOpen[3] = (UCHAR)(DateTime.usYear % 100);
				TranGCFQualWrk->auchCheckOpen[4] = (UCHAR)DateTime.usMonth;
				TranGCFQualWrk->auchCheckOpen[5] = (UCHAR)DateTime.usMDay;

				TranGCFQualWrk->auchCheckHashKey[0] = ( UCHAR )(DateTime.usMonth + DateTime.usSec + '+'); /* month mangled */
				TranGCFQualWrk->auchCheckHashKey[1] = ( UCHAR )(DateTime.usMDay + 'P');  /* day mangled */
				TranGCFQualWrk->auchCheckHashKey[2] = ( UCHAR )(DateTime.usHour + DateTime.usSec + '!');  /* hour mangled */
				TranGCFQualWrk->auchCheckHashKey[3] = ( UCHAR )(DateTime.usMin + 'A');   /* minute mangled */
				TranGCFQualWrk->auchCheckHashKey[4] = ( UCHAR )(DateTime.usSec + '0');   /* Second mangled */
				TranGCFQualWrk->auchCheckHashKey[5] = ( UCHAR )0;                      /* end of string */

				TranGCFQualWrk->fsGCFStatus2 |= GCFQUAL_EPT_HASH_MADE;
			}

			/*Added for SR 215, It seems that whenever a Keyed Dept Number,
			NHPOS never set the TOD for the transaction, which can cause problems for 
			sales restricted items that use the TOD to determine wether or not to sell the item*/
			// See also function ItemCheckAndSetTransDate() above.
			ItemSalesLocal.Tod = DateTime;      // Set the time and date of the transaction for use with age restriction check.
		}
        
		/* ---- unique transaction number R3.0 --- */
		if ( CliParaMDCCheckField( MDC_TRANNUM_ADR, MDC_PARAM_BIT_D ) && ! CliParaMDCCheckField( MDC_TRANNUM_ADR, MDC_PARAM_BIT_C ) ) {
			// if MDC 234 Bit D is on to use Unique Transaction Number and MDC 234 Bit C is off then use the number.
			// if MDC 234 Bit C is on then we are to ignore the MDC 234 Bit D setting.
			TranGCFQualWrk->usTableNo = ItemCommonUniqueTransNo();
		}

        ItemCommonLocal.fbCommonStatus |= COMMON_CONSNO;   /* set "already counted up" flag */
    }
}

/*
*===========================================================================
**Synopsis: VOID    ItemDrawer( VOID )
*
*    Input: VOID 
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return: VOID
*         
** Description: Set print status bit of current qualifier.  
*                
*===========================================================================
*/
VOID    ItemDrawer( VOID )
{
    if ( (PifSysConfig()->uchDrawer1 == DRAWER_NONE) &&
         (PifSysConfig()->uchDrawer2 == DRAWER_NONE) ) {
        return;
    }

    /*--- GET TRAN. QUAL. FOR EDIT ---*/
    PifRequestSem(usShrSemDrawer);              /* semaphore for shared prit & drawer open */
    if (TranModeQualPtr->ulWaiterID == 0) {  
        if (!(TranModeQualPtr->fbCashierStatus & MODEQUAL_CASHIER_TRAINING)) {
            PmgWait();
            if (TranModeQualPtr->fbCashierStatus & MODEQUAL_DRAWER_B) {
                UieOpenDrawer(ASN_BKEY_ADR);
            } else {
                UieOpenDrawer(ASN_AKEY_ADR);
            }
        }
    } else if (!(TranModeQualPtr->fbWaiterStatus & MODEQUAL_WAITER_TRAINING)) {
        PmgWait();
        UieOpenDrawer(ASN_AKEY_ADR);         /* waiter use only drawer A */
    }
    PifReleaseSem(usShrSemDrawer);              /* semaphore for shared prit & drawer open */
}

/*
*===========================================================================
**Synopsis: VOID    ItemTrailer( SHORT sType )    
*
*    Input: SHORT sType 
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return: VOID
*         
** Description: Send trailer data to transaction module.        R3.1
*===========================================================================
*/
VOID    ItemTrailer( SHORT sType )
{
    switch(sType) {
	case TYPE_STORAGE:
    case TYPE_STORAGE2:                                 /* for Dupli. Print */
	case TYPE_STORAGE3:
    case TYPE_THROUGH:
    case TYPE_STB_TRAIL:
    case TYPE_CANCEL:
    case TYPE_POSTCHECK:
    case TYPE_TRANSFER:
    case TYPE_TRAY:
    case TYPE_THROUGH_BOTH:
    case TYPE_SOFTCHECK:
    case TYPE_EPT:                                      /* EPT Logo         */
    case TYPE_EPT2:                                     /* for Dupli. Print */
    case TYPE_SPLITA:                                   /* R3.1 */
    case TYPE_SPLIT_EPT:                                /* 5/28/96 R3.1 */
    case TYPE_SPLIT_EPT2:                               /* R3.1 */
    case TYPE_MONEY:                                    /* Saratoga */
	case TYPE_SPLITA_EPT:								/* Split A with EPT Logo */
        ItemNormalTrailer(sType);
        break;

    case TYPE_SINGLE_TICKET:
    case TYPE_DOUBLE_TICKET:
        ItemTicketTrailer(sType);
        break;

    default:
        break;
    }
}

/*
*===========================================================================
**Synopsis: VOID    ItemNormalTrailer( SHORT sType )    
*
*    Input: SHORT sType 
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return: VOID
*         
** Description: Send trailer data for normal, New check or Re-order operation.  
*                
*===========================================================================
*/
VOID    ItemNormalTrailer(SHORT sType)
{
	ITEMPRINT      ItemPrintData = {0};      

    ItemPrintData.uchMajorClass = CLASS_ITEMPRINT;      /* class for print data */
    ItemPrintData.uchMinorClass = CLASS_TRAILER1;       /* class for normal trailer */

    if (TranModeQualPtr->ulWaiterID == 0) {  /* cashier operation ? */
        ItemPrintData.ulID = TranModeQualPtr->ulCashierID;   
    } else {                                /* waiter operation */
        ItemPrintData.ulID = TranModeQualPtr->ulWaiterID;   
    }

	memmove (ItemPrintData.uchUniqueIdentifier, TranGCFQualPtr->uchUniqueIdentifier, 24);
    
    ItemPrintData.usConsNo = TranCurQualPtr->usConsNo;          /* set consecutive No. */
    ItemPrintData.ulStoreregNo = TranCurQualPtr->ulStoreregNo;  /* set store/ reg. No. */    

    switch(sType) {
    case TYPE_STORAGE:
    case TYPE_STORAGE2:                                 /* for Dupli. print */
    case TYPE_STORAGE3:
        ItemPrintData.fbStorageStatus &= ~(NOT_ITEM_STORAGE | NOT_CONSOLI_STORAGE);  /* store into item and consoli storage */
        if (sType == TYPE_STORAGE) {
            ItemPrintData.fsPrintStatus = (PRT_SLIP | PRT_JOURNAL);  
            TrnPrint(&ItemPrintData);            /* send data to storage     */                                        
        } else if (sType == TYPE_STORAGE2) {
            ItemPrintData.fsPrintStatus = (PRT_RECEIPT | PRT_SINGLE_RECPT);
            TrnThrough(&ItemPrintData);         /* send data to storage     */                                        
        } else if (sType == TYPE_STORAGE3) {
            ItemPrintData.fsPrintStatus = (PRT_JOURNAL);  
            TrnPrint(&ItemPrintData);            /* send data to storage     */                                        
        }
        break;

    case TYPE_MONEY:                        /* Saratoga */
        ItemPrintData.uchMinorClass = CLASS_TRAILER_MONEY;
        ItemPrintData.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
        TrnPrint(&ItemPrintData);
        break;

    case TYPE_POSTCHECK:                    /* postcheck, store/recall */
        ItemPrintData.fsPrintStatus = (PRT_SLIP | PRT_JOURNAL);  /* set receipt print status */
        ItemPrintData.fbStorageStatus = NOT_CONSOLI_STORAGE;     /* store into item and consoli storage */
        TrnPrint(&ItemPrintData);            /* send print data to storage */                                        
        break;

    case TYPE_SOFTCHECK:                    /* soft check */
        ItemPrintData.uchMinorClass = CLASS_TRAILER4;             /* class for soft check trailer */
        ItemPrintData.fsPrintStatus = (PRT_SLIP | PRT_JOURNAL);   /* set receipt print status */
        ItemPrintData.fbStorageStatus &= ~(NOT_ITEM_STORAGE | NOT_CONSOLI_STORAGE);   /* store into item and consoli storage */
        TrnPrint(&ItemPrintData);            /* send print data to storage */                                        
        break;

    case TYPE_THROUGH:
        ItemPrintData.fsPrintStatus = (PRT_RECEIPT);              /* set receipt print status */
        TrnThrough(&ItemPrintData);         /* send print data to print module */                                        
        break;

    case TYPE_TRANSFER:
        ItemPrintData.fsPrintStatus = (PRT_SLIP | PRT_JOURNAL);   /* set receipt print status */
        TrnPrint(&ItemPrintData);            /* send print data to print module */                                        
        break;

    case TYPE_CANCEL:
        ItemPrintData.fsPrintStatus = (PRT_RECEIPT | PRT_JOURNAL); /* set receipt print status */
        ItemPrintData.fbStorageStatus &= ~(NOT_ITEM_STORAGE | NOT_CONSOLI_STORAGE); /* store into item and consoli storage */
        TrnPrint(&ItemPrintData);            /* send print data to storage */                                        
        break;

    case TYPE_STB_TRAIL:
        ItemPrintData.fsPrintStatus = (PRT_RECEIPT | PRT_SINGLE_RECPT);  /* set receipt and single rec. print status */
        TrnThrough(&ItemPrintData);         /* send print data to print module */                                        
        break;

    case TYPE_TRAY:                         /* tray total trailer */
        ItemPrintData.uchMinorClass = CLASS_TRAYTRAILER;
        ItemPrintData.fsPrintStatus = PRT_RECEIPT;        /* set receipt and single rec. print status */
        TrnThrough(&ItemPrintData);         /* send print data to print module */                                        
        break;

    case TYPE_THROUGH_BOTH:
        ItemPrintData.fsPrintStatus = (PRT_RECEIPT | PRT_JOURNAL);   /* set receipt print status */
        TrnThrough(&ItemPrintData);         /* send print data to print module */                                        
        break;

    case TYPE_EPT:                                      /* EPT Logo         */
        ItemPrintData.uchMinorClass = CLASS_EPT_TRAILER;/* EPT logo class   */
        ItemPrintData.fsPrintStatus = (PRT_SLIP | PRT_JOURNAL);  
        TrnPrint(&ItemPrintData);                    /* send to storage  */                                        
        break;

    case TYPE_EPT2:                                     /* for Dupli. print */
        ItemPrintData.uchMinorClass = CLASS_EPT_TRAILER;/* EPT logo class   */
        ItemPrintData.fsPrintStatus = (PRT_RECEIPT | PRT_SINGLE_RECPT);
        TrnThrough(&ItemPrintData);                 /* print trailer    */
        break;

	case TYPE_SPLITA_EPT:
		ItemPrintData.uchMinorClass = CLASS_SPLITA_EPT;
        ItemPrintData.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
        TrnPrintSpl(&ItemPrintData, TRN_TYPE_SPLITA);
		break;

    case TYPE_SPLITA:                                   /* R3.1 */
        ItemPrintData.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
        TrnPrintSpl(&ItemPrintData, TRN_TYPE_SPLITA);
        break;

    case TYPE_SPLIT_EPT:                                /* R3.1 5/28/96 */
        ItemPrintData.uchMinorClass = CLASS_EPT_TRAILER;/* EPT logo class   */
        ItemPrintData.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
        TrnPrintSpl(&ItemPrintData, TRN_TYPE_SPLITA);
        /* 5/28/96 TrnItem(&ItemPrintData); */
        break;

    case TYPE_SPLIT_EPT2:                               /* R3.1 5/28/96 */
        ItemPrintData.uchMinorClass = CLASS_EPT_TRAILER;/* EPT logo class   */
        ItemPrintData.fsPrintStatus = PRT_RECEIPT | PRT_SINGLE_RECPT;
        TrnThrough(&ItemPrintData);
        break;

    default:
        break;
    }
}                                       

/*
*===========================================================================
**Synopsis: VOID    ItemTicketTrailer( SHORT sType )    
*
*    Input: SHORT sType 
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return: VOID
*         
** Description: Send trailer/ header data for Ticket print.  
*                
*===========================================================================
*/
VOID    ItemTicketTrailer(SHORT sType)
{
	ITEMPRINT   ItemPrintData = {0};      
 
    ItemPrintData.uchMajorClass = CLASS_ITEMPRINT;         /* class for print data */
    if (sType == TYPE_SINGLE_TICKET) {
        ItemPrintData.uchMinorClass = CLASS_TRAILER2;      /* class for ticket trailer */
    } else if (sType == TYPE_DOUBLE_TICKET) {
        ItemPrintData.uchMinorClass = CLASS_DOUBLEHEADER; 
    }

	memmove (ItemPrintData.uchUniqueIdentifier, TranGCFQualPtr->uchUniqueIdentifier, 24);

    /* get operator ID and special mnemonics */
    if (TranModeQualPtr->ulWaiterID == 0) {  /* cashier operation ? */
        ItemPrintData.ulID = TranModeQualPtr->ulCashierID;   
    } else {                                /* waiter operation */
        ItemPrintData.ulID = TranModeQualPtr->ulWaiterID;   
    }
    
    /* get GC data from GCF qualifier */
    ItemPrintData.usTableNo = TranGCFQualPtr->usTableNo;   
    ItemPrintData.usGuestNo = TranGCFQualPtr->usGuestNo;      
    ItemPrintData.uchCdv = TranGCFQualPtr->uchCdv;         

    /* get consecutive No. */
    ItemPrintData.usConsNo = TranCurQualPtr->usConsNo;

    ItemPrintData.fsPrintStatus = PRT_RECEIPT;  /* set receipt print status */

    TrnThrough(&ItemPrintData);             /* send print data to print module */                                        
}                                       

 
/*
*===========================================================================
**Synopsis: VOID    ItemPromotion(USHORT usPrintStatus, UCHAR uchPrintType)
*
*    Input: USHORT usPrintStatus, UCHAR uchPrintType 
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return: VOID
*         
** Description: Send Promotional Header data to transaction module.  R3.1
*===========================================================================
*/
VOID    ItemPromotion(USHORT usPrintStatus, UCHAR uchPrintType)
{
	ITEMPRINT      ItemPrintData = {0};           /* print data for trailer/ header */

    if ( uchPrintType != TYPE_TRAY) {       /* tray total */
        ItemPrintStart( uchPrintType );     /* start print service */
    }
                                            
    ItemPrintData.uchMajorClass = CLASS_ITEMPRINT; 
    ItemPrintData.uchMinorClass = CLASS_PROMOTION;
    ItemPrintData.fsPrintStatus = usPrintStatus; 

    if (uchPrintType == TYPE_STORAGE || uchPrintType == TYPE_TRAY) {
        TrnItem(&ItemPrintData);            /* stored print of promotion header */                                        
    } else if (uchPrintType == TYPE_THROUGH){
        TrnThrough(&ItemPrintData);         /* real time print of promotion header */                                        
    } else if (uchPrintType == TYPE_SPLITA) {    /* R3.1 */
        TrnItemSpl(&ItemPrintData, TRN_TYPE_SPLITA);
    }
}                                       
/*
*===========================================================================
**Synopsis: VOID    ItemPromotion(USHORT usPrintStatus, UCHAR uchPrintType)
*
*    Input: USHORT usPrintStatus, UCHAR uchPrintType 
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return: VOID
*         
** Description: Send Promotional Header data to transaction module.  R3.1
*===========================================================================
*/
static VOID    ItemOrderDec(USHORT usPrintStatus, UCHAR uchPrintType)
{
	ITEMPRINT      ItemPrintData = {0};           /* print data for trailer/ header */

    ItemPrintData.uchMajorClass = CLASS_ITEMPRINT; 
    ItemPrintData.uchMinorClass = CLASS_ORDERDEC;
    ItemPrintData.fsPrintStatus = usPrintStatus; 

    if (uchPrintType == TYPE_STORAGE || uchPrintType == TYPE_TRAY) {
        TrnItem(&ItemPrintData);            /* stored print of promotion header */                                        
    } else if (uchPrintType == TYPE_THROUGH){
        TrnThrough(&ItemPrintData);         /* real time print of promotion header */                                        
    } else if (uchPrintType == TYPE_SPLITA) {    /* R3.1 */
        TrnItemSpl(&ItemPrintData, TRN_TYPE_SPLITA);
    }
}         
/*                  
*===========================================================================
**Synopsis: VOID    ItemPrintStart( UCHAR uchType )
*
*    Input: UCHAR   uchType 
*
*   Output: none
*
*    InOut: none
*
**Return:   none
*         
** Description: decide print portion, start print service      R3.1
*===========================================================================
*/
        
VOID    ItemPrintStart( UCHAR uchType )
{               
	ITEMPRINT   ItemPrint = {0};

    ItemPrint.uchMajorClass = CLASS_ITEMPRINT; 
    ItemPrint.uchMinorClass = CLASS_START;

    if ( uchType == TYPE_STORAGE ) {  
        TrnItem( &ItemPrint );                                      /* store */ 
    } else if (uchType == TYPE_SPLITA) {
        TrnItemSpl(&ItemPrint, TRN_TYPE_SPLITA);
    } else if (uchType == TYPE_SPLIT_TENDER) {
        ItemPrint.fbStorageStatus = NOT_ITEM_STORAGE;
        TrnItem(&ItemPrint);
    } else if ( uchType == TYPE_POSTCHECK ) {  
        ItemPrint.fbStorageStatus = NOT_CONSOLI_STORAGE; 
        TrnItem( &ItemPrint );                                      /* store */ 
    } else {
        TrnThrough( &ItemPrint );                                   /* through */
    }
}                                       



/*
*===========================================================================
**Synopsis: VOID    ItemHeader(UCHAR uchPrintType)
*
*    Input: UCHAR uchPrintType 
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return: VOID
*         
** Description: Send Training or Preselect Void Header data to transaction 
*               module.                                        R3.1
*===========================================================================
*/
VOID    ItemHeader(UCHAR uchPrintType)
{
    if (TranCurQualPtr->fsCurStatus & CURQUAL_TRAINING) {
		ITEMPRINT      ItemPrintData = {0};       

        ItemPrintData.uchMajorClass = CLASS_ITEMPRINT;           /* class for print data */
        ItemPrintData.uchMinorClass = CLASS_PRINT_TRAIN;
        ItemPrintData.fsPrintStatus = (PRT_SLIP | PRT_JOURNAL);  /* set slip & journal print status */
        ItemPrintData.fbStorageStatus &= ~(NOT_ITEM_STORAGE | NOT_CONSOLI_STORAGE);  /* store into item and consoli storage */

        if (uchPrintType == TYPE_STORAGE) {
            TrnItem(&ItemPrintData);                     /* stored print of promotion header */                                        
        } else if (uchPrintType == TYPE_THROUGH){
            TrnThrough(&ItemPrintData);                  /* real time print of promotion header */                                        
        } else if ( uchPrintType == TYPE_STB_TRAIL ) {
            ItemPrintData.fsPrintStatus = PRT_RECEIPT | PRT_SINGLE_RECPT;  /* set receipt and single rec. print status */
            TrnThrough(&ItemPrintData);                                    /* send print data to print module */                                        
        } else if ( uchPrintType == TYPE_CANCEL ) {
            ItemPrintData.fbStorageStatus |= NOT_CONSOLI_STORAGE;   /* store into item storage */
            TrnItem(&ItemPrintData);
        } else if (uchPrintType == TYPE_SPLITA) {   /* R3.1 */
            TrnItemSpl(&ItemPrintData, TRN_TYPE_SPLITA);
        }
    }

    if (TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)) {
		ITEMPRINT      ItemPrintData = {0};       

        ItemPrintData.uchMajorClass = CLASS_ITEMPRINT;           /* class for print data */

		if (TranCurQualPtr->fsCurStatus & CURQUAL_PVOID)
			ItemPrintData.uchMinorClass = CLASS_PRINT_PVOID;     // transaction void print
		if (TranCurQualPtr->fsCurStatus & (CURQUAL_TRETURN | CURQUAL_PRETURN))
			ItemPrintData.uchMinorClass = CLASS_PRINT_TRETURN;   // transaction return print

        ItemPrintData.fsPrintStatus = (PRT_SLIP | PRT_JOURNAL);          /* set slip & journal print status */
        ItemPrintData.fbStorageStatus &= ~(NOT_ITEM_STORAGE | NOT_CONSOLI_STORAGE);  /* store into item and consoli storage */

        if (uchPrintType == TYPE_STORAGE) {
            TrnItem(&ItemPrintData);            /* stored print of promotion header */                                        
        } else if (uchPrintType == TYPE_THROUGH){
            TrnThrough(&ItemPrintData);         /* real time print of promotion header */                                        
        } else if ( uchPrintType == TYPE_STB_TRAIL ) {
            ItemPrintData.fsPrintStatus = PRT_RECEIPT | PRT_SINGLE_RECPT;   /* set receipt and single rec. print status */
            TrnThrough(&ItemPrintData);         /* send print data to print module */                                        
        } else if ( uchPrintType == TYPE_CANCEL ) {
            ItemPrintData.fbStorageStatus |= NOT_CONSOLI_STORAGE;   /* store into item storage */
            TrnItem(&ItemPrintData);
        } else if (uchPrintType == TYPE_SPLITA) {   /* R3.1 */
            TrnItemSpl(&ItemPrintData, TRN_TYPE_SPLITA);
        }
    }
}                                       


/*
*===========================================================================
**Synopsis: VOID    ItemInit( VOID )
*
*    Input: VOID 
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return: VOID
*         
** Description: Initialize all local datas.  
*                
*===========================================================================
*/
VOID    ItemInit(VOID)
{
	{
		ITEMSALESLOCAL      *pSalesLocalData = ItemSalesGetLocalPointer();         /* sales module local data */
		ITEMSALESLOCAL      SalesLocalTempData = {0};     /* sales module local data */

		/* initiaize sales local data except for certain struct members */
		SalesLocalTempData.uchMenuShift = pSalesLocalData->uchMenuShift;
		SalesLocalTempData.uchAdjDefShift = pSalesLocalData->uchAdjDefShift; /* 2172 */
		SalesLocalTempData.uchAdjKeyShift = pSalesLocalData->uchAdjKeyShift; /* 2172 */
		SalesLocalTempData.uchAdjCurShift = pSalesLocalData->uchAdjCurShift; /* 2172 */
		*pSalesLocalData = SalesLocalTempData;
	}

    /* initiaize tender local data */
	{
		ITEMTENDERLOCAL     *pTenderLocalData = ItemTenderGetLocalPointer();        /* tender module local data */
		DCURRENCY           lBuffer;

		lBuffer = pTenderLocalData->lSavedMI;     /* don't initialize this data */
		memset(pTenderLocalData, 0, sizeof(*pTenderLocalData)); 
		pTenderLocalData->lSavedMI = lBuffer;
		pTenderLocalData->usCheckTenderId = 1;
	}

    /* initiaize modifier local data */
	{
		ITEMMODLOCAL        *pModLocalData = ItemModGetLocalPtr();           /* modifier module local data */
		UCHAR               uchBuffer;

		NHPOS_ASSERT(sizeof(uchBuffer) == sizeof(pModLocalData->fbModifierStatus));
		uchBuffer = pModLocalData->fbModifierStatus;
		memset(pModLocalData, 0, sizeof(*pModLocalData));  /* initialize data structure */

		if (!CliParaMDCCheckField(MDC_RECEIPT_ADR, MDC_PARAM_BIT_D)) {  /* Saratoga */
			if (uchBuffer & MOD_NORECEIPT) {
				ItemModSetLocal(MOD_NORECEIPT);
			}
		}
		if (CliParaMDCCheckField(MDC_WIC2_ADR, MDC_PARAM_BIT_C)) {     /* Saratoga */
			if (uchBuffer & MOD_WIC) {
				ItemModSetLocal(MOD_WIC);
			}
		}
	}
    
    /* initiaize trans. open local data */
	{
		ITEMTRANSLOCAL      *pTransLocalData = ItemTransGetLocalPointer();         /* transaction module local data */
		memset(pTransLocalData, 0, sizeof(*pTransLocalData));  /* initialize data structure */
	}
    
    /* initiaize other local data */
	{
		ITEMOTHERLOCAL      *pOtherLocalData = ItemOtherGetLocalPointer();         /* other module local data */
		memset(pOtherLocalData, 0, sizeof(*pOtherLocalData));  /* initialize data structure */
	}
    
    /* initiaize total key local data */
	{
		ITEMTOTALLOCAL      *pTotalLocalData = ItemTotalGetLocalPointer();         /* total key module local data */
		memset(pTotalLocalData, 0, sizeof(*pTotalLocalData));  /* initialize data structure */
	}

    /* initiaize common local data */
	{
		ITEMCOMMONLOCAL     *pCommonLocalData = ItemCommonGetLocalPointer();        /* common module local data */
		memset(pCommonLocalData, 0, sizeof(*pCommonLocalData));  /* initialize data structure */
	}

    /*--- MANIPURATE MODE CHANGABLE STATUS ---*/
    if (CliParaMDCCheckField (MDC_SIGNOUT_ADR, MDC_PARAM_BIT_C)) { 
                                            /* close by mode key position change */
        UieModeChange(UIE_ENABLE);          /* make mode changable condition */
    }
 
    /*--- MANIPURATE MENU SHIFT PAGE ---*/
    if (CliParaMDCCheckField (MDC_MENU_ADR, MDC_PARAM_BIT_D)) {
                                            /* if not stay down type */
        uchDispCurrMenuPage = ItemSalesLocal.uchMenuShift;
                                            /* rewrite default menu page into current page */
    }
    if (uchDispCurrMenuPage == 0) {                    /* R3.0 */
        uchDispCurrMenuPage = 1;                       /* R3.0 */
    }
    ItemCommonShiftPage(uchDispCurrMenuPage);          /* R3.0 */
	ItemCommonResetStatus(COMMON_PROHIBIT_SIGNOUT);
}



/*==========================================================================
**  Synopsis:   SHORT   ItemFinalize(VOID)
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:     
*
*   Description:    transaction finalize, V3.3
==========================================================================*/

SHORT   ItemFinalize(VOID)
{           
    ItemMiscLocal.ulCashierID = TranModeQualPtr->ulCashierID;               /* save cashier id for post receipt, V3.3 */
    ItemMiscLocal.ulWaiterID  = TranModeQualPtr->ulWaiterID;                /* save waiter id for post receipt, V3.3 */
    
    flDispRegDescrControl &= ~( TENDER_CNTRL | TOTAL_CNTRL | PTEND_CNTRL
        | CHANGE_CNTRL | NEWCHK_CNTRL | PAYMENT_CNTRL | STORE_CNTRL
        | TVOID_CNTRL | PO_CNTRL | ROA_CNTRL | TIPSPO_CNTRL
        | TRANSFR_CNTRL | DECTIP_CNTRL | PBAL_CNTRL | RECALL_CNTRL );
    
    flDispRegKeepControl &= ~( TENDER_CNTRL | TOTAL_CNTRL | PTEND_CNTRL
        | CHANGE_CNTRL | NEWCHK_CNTRL | PAYMENT_CNTRL | STORE_CNTRL
        | TVOID_CNTRL | PO_CNTRL | ROA_CNTRL | TIPSPO_CNTRL
        | TRANSFR_CNTRL | DECTIP_CNTRL | PBAL_CNTRL | RECALL_CNTRL );

    /* V3.3 */
    if ((TranModeQualPtr->fbCashierStatus & MODEQUAL_WAITER_LOCK) == 0) {       /* do not sign-out by server lock */
		UIFREGOPECLOSE  UifOpeClose = {0};        

        if (ItemCommonLocal.fbCommonStatus & COMMON_NON_GUEST_CHECK) {      /* non-guest check */
            if ( TranModeQualPtr->ulWaiterID ) {                                    /* surrogate operation, V3.3 */
                if (TranModeQualPtr->auchWaiStatus[CAS_CASHIERSTATUS_1] & CAS_FINALIZE_NON_GCF) {
                    ItemDispRel();                                          /* back ground display */   

                    UifOpeClose.uchMajorClass = CLASS_UIFREGOPECLOSE;       /* major class */ 
                    UifOpeClose.uchMinorClass = CLASS_UIWAIKEYRELONFIN;     /* minor class */
                    UifOpeClose.ulID = TranModeQualPtr->ulWaiterID;                 /* waiter ID */             
					UifOpeClose.ulStatusFlags |= UIFREGMISC_STATUS_METHOD_FINAL;
                    ItemSignOut( &UifOpeClose );                            /* relese on finalize */
                    ItemInit();                                             /* initialize item module local data */
                    return( UIF_WAI_SIGNOUT );                              /* UI status */
                } 
            } else if ( TranModeQualPtr->ulCashierID ){                             /* cashier operation */
                if (TranModeQualPtr->auchCasStatus[CAS_CASHIERSTATUS_1] & CAS_FINALIZE_NON_GCF) {
                    ItemDispRel();                                          /* back ground display */   

                    UifOpeClose.uchMajorClass = CLASS_UIFREGOPECLOSE;       /* major class */
                    UifOpeClose.uchMinorClass = CLASS_UICASRELONFIN;        /* minor class */
                    UifOpeClose.ulID = TranModeQualPtr->ulCashierID;                /* cashier ID */
					UifOpeClose.ulStatusFlags |= UIFREGMISC_STATUS_METHOD_FINAL;
                    ItemSignOut( &UifOpeClose );                            /* relese on finalize */
                    ItemInit();                                             /* initialize item module local data */
                    return( UIF_CAS_SIGNOUT );                              /* UI status */
                }
            }
        } else {                                                            /* guest check */
            if ( TranModeQualPtr->ulCashierID ){                             /* cashier operation */
                if (TranModeQualPtr->auchCasStatus[CAS_CASHIERSTATUS_1] & CAS_FINALIZE_GCF) {
                    ItemDispRel();                                          /* back ground display */   

                    UifOpeClose.uchMajorClass = CLASS_UIFREGOPECLOSE;       /* major class */
                    UifOpeClose.uchMinorClass = CLASS_UICASRELONFIN;        /* minor class */
                    UifOpeClose.ulID = TranModeQualPtr->ulCashierID;                /* cashier ID */
					UifOpeClose.ulStatusFlags |= UIFREGMISC_STATUS_METHOD_FINAL;
                    ItemSignOut( &UifOpeClose );                            /* relese on finalize */
                    ItemInit();                                             /* initialize item module local data */
                    return( UIF_CAS_SIGNOUT );                              /* UI status */
                }
            }
        }
    }

    if ( CliParaMDCCheckField ( MDC_RECEIPT_ADR, MDC_PARAM_BIT_D ) ) {       /* receipt key release on finalize */
        flDispRegDescrControl &= ~RECEIPT_CNTRL;
        flDispRegKeepControl &= ~RECEIPT_CNTRL;
    }

    ItemInit();

    return( UIF_FINALIZE );                                         /* transaction finalize */
}



/*==========================================================================
**   Synopsis:  VOID    ItemDispRel( VOID )   
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:     
*
*   Description:    generate release on finalize data
==========================================================================*/

VOID    ItemDispRel( VOID )   
{
	REGDISPMSG      DispMsg = {0}, DispMsgBack = {0};

    DispMsg.uchMajorClass = CLASS_UIFREGTENDER;
    DispMsg.uchMinorClass = CLASS_UITENDER1;

    if (TranModeQualPtr->ulCashierID == 0) {                                  /* no cashier sign in */
        DispMsg.fbSaveControl = (DISP_SAVE_ECHO_ONLY | DISP_SAVE_TYPE_3);     /* save for redisplay (release on finalize) */
        DispMsgBack.uchMajorClass = CLASS_UIFREGMODE;
        DispMsgBack.uchMinorClass = CLASS_UIREGMODEIN;
        RflGetLead (DispMsgBack.aszMnemonic, LDT_ID_ADR );             /* Request Cashier/Waiter/Supervisor Sign-in */
    } else {   
        DispMsg.fbSaveControl = (DISP_SAVE_ECHO_ONLY | DISP_SAVE_TYPE_2);     /* save for redisplay (release on finalize) */
        DispMsgBack.uchMajorClass = CLASS_UIFREGOPEN;
        DispMsgBack.uchMinorClass = CLASS_UICASHIERIN;
        _tcsncpy(DispMsgBack.aszMnemonic, TranModeQualPtr->aszCashierMnem, STD_CASHIERNAME_LEN );
    }    
        
    DispMsgBack.fbSaveControl = DISP_SAVE_TYPE_3;                       /* save for redisplay (release on finalize) */

    flDispRegKeepControl &= ~( TENDER_CNTRL | TOTAL_CNTRL | PTEND_CNTRL
        | CHANGE_CNTRL | NEWCHK_CNTRL | PAYMENT_CNTRL | TRAIN_CNTRL | STORE_CNTRL
        | TVOID_CNTRL | PO_CNTRL | ROA_CNTRL | TIPSPO_CNTRL
        | TRANSFR_CNTRL | DECTIP_CNTRL | PBAL_CNTRL | RECALL_CNTRL );

    DispWriteSpecial( &DispMsg, &DispMsgBack );                     /* display check total */

    flDispRegDescrControl &= ~( TENDER_CNTRL | TOTAL_CNTRL | PTEND_CNTRL
        | CHANGE_CNTRL | NEWCHK_CNTRL | PAYMENT_CNTRL | TRAIN_CNTRL | STORE_CNTRL
        | TVOID_CNTRL | PO_CNTRL | ROA_CNTRL | TIPSPO_CNTRL
        | TRANSFR_CNTRL | DECTIP_CNTRL | PBAL_CNTRL | RECALL_CNTRL );

}



// return the following indications
// -1 no card label in ItemCommonLocal
//  0 card label does not match
//  1 card label does match
SHORT  ItemComonCheckReturnCardLabel (TCHAR *aszCardLabel)
{
	SHORT  sRetValue = -1;

	if (ItemCommonLocal.aszCardLabelReturnsTenderChargeTips[0]) {
		if (_tcscmp (ItemCommonLocal.aszCardLabelReturnsTenderChargeTips, aszCardLabel) != 0) {
			sRetValue = 0;
		} else {
			sRetValue = 1;
		}
	}

	return sRetValue;
}

/*
 *  ItemCheckCustomerSettingsFlags() is used with a set of bit masks to indicate if a particular customer's settings
 *  are to be used during operations.  see function ProcessXmlFileStartUpCustomerSettings() for processing of the
 *  PARAMINI file and the customersettings tag in that file.
**/
SHORT  ItemCheckCustomerSettingsFlags (ULONG ulFlagsToCheck)
{
	return ((PifSysConfig()->ulCustomerSettingsFlags & ulFlagsToCheck) != 0);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemCommonCheckComp( VOID )
*
*    Input: VOID 
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return: : Function performed successfully.
*         
** Description:     Check Compulsory Entry.
*===========================================================================
*/
SHORT   ItemCommonCheckComp( VOID )
{
    SHORT  sReturnStatus;

    if (ItemSalesLocalPtr->fbSalesStatus & SALES_BOTH_COMPLSRY) {
        return(LDT_PMODCNDI_ADR);           /* return compulsory error */
    }

    if (ItemCommonLocal.ItemSales.uchMajorClass == CLASS_ITEMSALES) {
        if (ItemSalesDiscountClassIsMod (ItemCommonLocal.ItemSales.uchMinorClass)) {
            if ((sReturnStatus = ItemPrevModDisc()) != ITM_SUCCESS) {
                ItemCommonLocal.fbCommonStatus &= ~COMMON_VOID_EC;  /* turn off disable E.C bit */
                return(sReturnStatus);
            }
        }
    }

    return(ITM_SUCCESS);                    /* return compulsory error */
}

/*
*===========================================================================
**Synopsis: VOID    ItemCommonCheckExempt(VOID)
*
*    Input: VOID 
*   Output: Nothing
*    InOut: Nothing
*
**Return: : LDT_SEQ_ADR
*         : ITM_SUCCESS
*         
** Description: Check tax exempt key depression for item.      R3.0  R3.1
*===========================================================================
*/
SHORT   ItemCommonCheckExempt(VOID)
{
    if (ItemModLocalPtr->fsTaxable & MOD_CANADAEXEMPT) {
        return(LDT_SEQERR_ADR);
    }
    return(ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemCommonCheckSplit(VOID)
*
*    Input: Nothing
*   Output: Nothing
*    InOut: Nothing
**Return: : Status
*         
** Description:     Check Split Key(Prohibit Itemize at Split Key Status).
*===========================================================================
*/
SHORT   ItemCommonCheckSplit(VOID)
{
    if (TranCurQualPtr->uchSplit != 0) {
        return(LDT_SEQERR_ADR);
    }
    return(ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: VOID    ItemCommonCancelTaxExempt( VOID )
*
*    Input: VOID 
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return: :
*         
** Description: Turn off "Exempt Transaction" flag of current qualifier.   
*                                                              R3.1
*===========================================================================
*/
VOID    ItemCommonCancelExempt(VOID)
{
    TRANGCFQUAL     *pWorkGCF = TrnGetGCFQualPtr();

    if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
        if ( ( TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_CASHIER ) {     
            pWorkGCF->fsGCFStatus &= ~(GCFQUAL_GSTEXEMPT | GCFQUAL_PSTEXEMPT);  /* turn off "Exempt Transaction" flag */
            flDispRegDescrControl &= ~TAXEXMPT_CNTRL;   /* turn off descriptor control flag */  
            flDispRegKeepControl &= ~TAXEXMPT_CNTRL;    /* turn off descriptor keep control flag */
        }
    } else if (ItemCommonTaxSystem() == ITM_TAX_US) {
        if ( ( TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_CASHIER ) {     
            pWorkGCF->fsGCFStatus &= ~GCFQUAL_USEXEMPT;               /* reset USEXEMPT bit */
            flDispRegDescrControl &= ~TAXEXMPT_CNTRL;   /* turn off descriptor control flag */  
            flDispRegKeepControl &= ~TAXEXMPT_CNTRL;    /* turn off descriptor keep control flag */
        }
    }
}

/*
*===========================================================================
**Synopsis: VOID    ItemCommonSetDisplayFlagExempt( int iKeepIndic )
*
*    Input: int iKeepIndic    - flag to modify flDispRegKeepControl or not.
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return: : 
*         
** Description: Turn on "Exempt Transaction" flag display indicators if the
*               current qualifier indicates the transaction is tax exempt.
*
*               In some cases where this function could be used, the original code
*               did not modify the global variable flDispRegKeepControl so we
*               have added a flag argument to indicate if both global descriptor
*               variables should be modified or not.
*===========================================================================
*/
VOID  ItemCommonSetDisplayFlagExempt (int iKeepIndic)
{
    if ( TranModeQualPtr->fsModeStatus & MODEQUAL_CANADIAN ) {          /* R3.1 */
        if ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_GSTEXEMPT            /* GST exempt */
            || TranGCFQualPtr->fsGCFStatus & GCFQUAL_PSTEXEMPT ) {      /* PST exempt */
            flDispRegDescrControl |= TAXEXMPT_CNTRL;            /* tax exempt */
			if (iKeepIndic)
				flDispRegKeepControl |= TAXEXMPT_CNTRL;         /* keep tax exempt */
			else
				flDispRegKeepControl &= ~TAXEXMPT_CNTRL;        /* clear and not keep tax exempt */
        }
    } else {
        if ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_USEXEMPT ) {
            flDispRegDescrControl |= TAXEXMPT_CNTRL;            /* tax exempt */
			if (iKeepIndic)
				flDispRegKeepControl |= TAXEXMPT_CNTRL;         /* keep tax exempt */
			else
				flDispRegKeepControl &= ~TAXEXMPT_CNTRL;        /* clear and not keep tax exempt */
        }
    }
}


/*
*===========================================================================
**Synopsis: SHORT   ItemCommonCheckScaleDiv(ITEMMODSCALE *pData, LONG lScaleData)
*
*    Input: ITEMMODSCALE  *pData
*           LONG          lScaleData    - tare or weight value to be checked
*
*   Output: 
*
*    InOut: 
*
**Return: :   ITM_SUCCESS              if data check successful
*             LDT_KEYOVER_ADR          if scale measurement value is invalid
*         
** Description:   This function checks that the value in lScaleData is consistent
*                 with the scale settings described in pData.
*
*                 This is a data integrity check to ensure that the value is
*                 within the proper data range for the scale provisioning settings.

*                 lScaleData may contain the weight or the tare.
*                
*===========================================================================
*/

SHORT   ItemCommonCheckScaleDiv(ITEMMODSCALE *pItemModScale, LONG lScaleData)
{            
    if (pItemModScale->fbTareStatus & MOD_SCALE_DIV_005_OR_001) {
        if(!(pItemModScale->fbTareStatus & MOD_SCALE_DIV_001)) {    /* min. scale div. is 0.005 */
            if ((lScaleData % 5L) != 0L) {
                return(LDT_KEYOVER_ADR);
            }
        }
    } else {                                                    /* min. weight is 999.99 */
        if ((lScaleData % 10L) != 0L) {
            return(LDT_KEYOVER_ADR);
        }
    }

    return(ITM_SUCCESS);
}


/*
*===========================================================================
**Synopsis: SHORT   ItemCommonGetScaleData(ITEMMODSCALE *pItemModScale)
*
*    Input: ITEMMODSCALE *pItemModScale
*
*   Output: pItemModScale->lWeightScaled
*
*    InOut: 
*
**Return: : ITM_SUCCESS        if successful
*           LDT_SEQERR_ADR     if scale not provided
*           LDT_SCALEERR_ADR   if scale reported an error or data format incorrect
*         
** Description:   Read the scale to get the item weight, check the data, and adjust
*                 using the specified tare weight.
*
*                 WARNING: the tare weight must be specified in  pItemModScale->lTareValue.
*                          if no tare then use a tare weight of zero.
*
*===========================================================================
*/
SHORT   ItemCommonGetScaleData(ITEMMODSCALE *pItemModScale)
{
	UIE_SCALE   ScaleDataBuff = {0};                 /* scale data receive buffer */
	SHORT  sReturnStatus;

    sReturnStatus = ItemCommonReadScale(&ScaleDataBuff);
    if (sReturnStatus < 0) {                /* failed to read */
		char  xBuff[128];
		sprintf (xBuff, "==WARNING: Scale Error ItemCommonReadScale() %d", sReturnStatus);
		NHPOS_ASSERT_TEXT((sReturnStatus >= 0), xBuff);
        if (sReturnStatus == UIE_SCALE_NOT_PROVIDE) {
            sReturnStatus = LDT_SEQERR_ADR;
        } else {
            sReturnStatus = LDT_SCALEERR_ADR;
		}
        return(sReturnStatus);
    }

    pItemModScale->lWeightScaled = ScaleDataBuff.ulData;
 
    /*--- ADJUST WEIGHT DATA ---*/
    if (ScaleDataBuff.uchDec == 2) {        /* 2 decimal data */
        pItemModScale->lWeightScaled *= 10L;
    }

    sReturnStatus = ItemCommonCheckScaleData(pItemModScale, &ScaleDataBuff);

    pItemModScale->lWeightScaled -= pItemModScale->lTareValue;                      /* calculate net weight */
                                                 
    if (pItemModScale->lWeightScaled <= 0) {
		PifLog (MODULE_UIE_SCALE, LOG_ERROR_DEVICE_DATA_RANGE_ERR);
		PifLog (MODULE_ERROR_NO(MODULE_UIE_SCALE), LDT_SCALEERR_ADR);
		PifLog (MODULE_LINE_NO(MODULE_UIE_SCALE), (USHORT)__LINE__);
        sReturnStatus = LDT_SCALEERR_ADR;
    }
    if (sReturnStatus != ITM_SUCCESS) {                /* failed to read */
		char  xBuff[256];
		sprintf (xBuff, "==WARNING: ItemCommonCheckScaleData() %d - fbTareStatus 0x%4.4x lTareValue %d lWeightScaled %d uchUnit %d uchDec %d",
			sReturnStatus, pItemModScale->fbTareStatus, pItemModScale->lTareValue, pItemModScale->lWeightScaled, ScaleDataBuff.uchUnit, ScaleDataBuff.uchDec);
		NHPOS_ASSERT_TEXT((sReturnStatus == ITM_SUCCESS), xBuff);
    }
    return(sReturnStatus);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemCommonCheckScaleData(ITEMMODSCALE *pData, UIE_SCALE *pScaleData)
*
*    Input: ITEMMODSCALE *pData
*
*   Output: 
*
*    InOut: 
*
**Return: :   ITM_SUCCESS              if data check successful
*             LDT_SCALEERR_ADR         if communication failed or data bad
*             LDT_EQUIPPROVERROR_ADR   if scale response does not match MDC 29 settings
*         
** Description:  Check the scale data and modify the weight amount by the specified tare amount.
*
*===========================================================================
*/
static SHORT   ItemCommonCheckScaleData(ITEMMODSCALE *pItemModScale, UIE_SCALE *pScaleData)
{
	SHORT  sReturnStatus;

    if (pItemModScale->lWeightScaled < 0) {
        return(LDT_SCALEERR_ADR);
    }

    /*--- CHECK SCALE DATA VS. MDC ---*/
    if (pScaleData->uchUnit == UIE_SCALE_UNIT_UNKNOWN) {
		PifLog (MODULE_UIE_SCALE, LOG_ERROR_DEVICE_DATA_RANGE_ERR);
		PifLog (MODULE_ERROR_NO(MODULE_UIE_SCALE), LDT_SCALEERR_ADR);
		PifLog (MODULE_LINE_NO(MODULE_UIE_SCALE), (USHORT)__LINE__);
		return(LDT_SCALEERR_ADR);
	}
    else if (pScaleData->uchUnit == UIE_SCALE_UNIT_LB) {
        if (pScaleData->uchDec == 2) {      /* provide 30lb scale */
            if (pItemModScale->fbTareStatus & MOD_SCALE_DIV_005_OR_001) {
				PifLog (MODULE_UIE_SCALE, LOG_ERROR_DEVICE_MDC_CONFLICT);
				PifLog (MODULE_ERROR_NO(MODULE_UIE_SCALE), LDT_EQUIPPROVERROR_ADR);
				PifLog (MODULE_DATA_VALUE(MODULE_UIE_SCALE), pScaleData->uchDec);
				PifLog (MODULE_DATA_VALUE(MODULE_UIE_SCALE), pItemModScale->fbTareStatus);
				PifLog (MODULE_LINE_NO(MODULE_UIE_SCALE), (USHORT)__LINE__);
				NHPOS_ASSERT_TEXT(0, "==PROVISIONING: LOG_ERROR_DEVICE_MDC_CONFLICT Scale MDC 29 settings.");
                return(LDT_EQUIPPROVERROR_ADR);
            }
            if (pItemModScale->lWeightScaled > 30000L) {
				PifLog (MODULE_UIE_SCALE, LOG_ERROR_DEVICE_DATA_RANGE_ERR);
				PifLog (MODULE_ERROR_NO(MODULE_UIE_SCALE), LDT_BLOCKEDBYMDC_ADR);
				PifLog (MODULE_DATA_VALUE(MODULE_UIE_SCALE), (USHORT)pItemModScale->lWeightScaled);
				PifLog (MODULE_LINE_NO(MODULE_UIE_SCALE), (USHORT)__LINE__);
                return(LDT_SCALEERR_ADR);
            }
        } else {                            /* provide other scale */
            if (!(pItemModScale->fbTareStatus & MOD_SCALE_DIV_005_OR_001)) {
				PifLog (MODULE_UIE_SCALE, LOG_ERROR_DEVICE_MDC_CONFLICT);
				PifLog (MODULE_ERROR_NO(MODULE_UIE_SCALE), LDT_EQUIPPROVERROR_ADR);
				PifLog (MODULE_DATA_VALUE(MODULE_UIE_SCALE), pScaleData->uchDec);
				PifLog (MODULE_DATA_VALUE(MODULE_UIE_SCALE), pItemModScale->fbTareStatus);
				PifLog (MODULE_LINE_NO(MODULE_UIE_SCALE), (USHORT)__LINE__);
				NHPOS_ASSERT_TEXT(0, "==PROVISIONING: LOG_ERROR_DEVICE_MDC_CONFLICT Scale MDC 29 settings.");
                return(LDT_EQUIPPROVERROR_ADR);
            }
            if (pItemModScale->lWeightScaled > 10000L) {
				PifLog (MODULE_UIE_SCALE, LOG_ERROR_DEVICE_DATA_RANGE_ERR);
				PifLog (MODULE_ERROR_NO(MODULE_UIE_SCALE), LDT_BLOCKEDBYMDC_ADR);
				PifLog (MODULE_DATA_VALUE(MODULE_UIE_SCALE), (USHORT)pItemModScale->lWeightScaled);
				PifLog (MODULE_LINE_NO(MODULE_UIE_SCALE), (USHORT)__LINE__);
                return(LDT_SCALEERR_ADR);
            }
        }
    } else {                                /* only 3 decimal data */
        if (!(pItemModScale->fbTareStatus & MOD_SCALE_DIV_005_OR_001)) {
			PifLog (MODULE_UIE_SCALE, LOG_ERROR_DEVICE_MDC_CONFLICT);
			PifLog (MODULE_ERROR_NO(MODULE_UIE_SCALE), LDT_EQUIPPROVERROR_ADR);
			PifLog (MODULE_DATA_VALUE(MODULE_UIE_SCALE), pScaleData->uchDec);
			PifLog (MODULE_DATA_VALUE(MODULE_UIE_SCALE), pItemModScale->fbTareStatus);
			PifLog (MODULE_LINE_NO(MODULE_UIE_SCALE), (USHORT)__LINE__);
			NHPOS_ASSERT_TEXT(0, "==PROVISIONING: LOG_ERROR_DEVICE_MDC_CONFLICT Scale MDC 29 settings.");
            return(LDT_EQUIPPROVERROR_ADR);
        }
        if (pItemModScale->lWeightScaled > 15000L) {
			PifLog (MODULE_UIE_SCALE, LOG_ERROR_DEVICE_DATA_RANGE_ERR);
			PifLog (MODULE_ERROR_NO(MODULE_UIE_SCALE), LDT_BLOCKEDBYMDC_ADR);
			PifLog (MODULE_DATA_VALUE(MODULE_UIE_SCALE), (USHORT)pItemModScale->lWeightScaled);
			PifLog (MODULE_LINE_NO(MODULE_UIE_SCALE), (USHORT)__LINE__);
            return(LDT_SCALEERR_ADR);
        }
    }

    sReturnStatus = ItemCommonCheckScaleDiv(pItemModScale, pItemModScale->lWeightScaled);    /* check weight */
    if (sReturnStatus != ITM_SUCCESS) {
        return(sReturnStatus);
    }

    return(ITM_SUCCESS);
}


/*
*===========================================================================
**Synopsis: SHORT   ItemCommonReadScale(UIE_SCALE *pScaleData)
*
*    Input: UIE_SCALE *pScaleData
*
*   Output: 
*
*    InOut: 
*
**Return: :  UIE_SUCCESS            read of scale data was successful.
*            UIE_SCALE_DATAFORMAT   format of scale weight data is invalid.
*            UIE_SCALE_DATAUNITS    units of weight are unknown
*            UIE_SCALE_TIMEOUT      time out while attempting reading from COM port
*            UIE_SCALE_ERROR_COM    error reading from COM port
*            UIE_SCALE_NOT_PROVIDE  scale not provisioned
*            UIE_SCALE_ERROR        an unknown error when trying to read the scale.
*         
** Description:   
*                
*===========================================================================
*/
SHORT   ItemCommonReadScale(UIE_SCALE *pUie_Scale)
{
	SHORT  sReturnStatus = UIE_SCALE_ERROR;
	SHORT  i = 0;
	SHORT  sStat = 0;

    for (i = 0; i < COMMON_NO_OF_RETRY_READ; i++) {
        sReturnStatus = UieReadScale(pUie_Scale);
        if (sReturnStatus >= 0) {           /* success to read */
            return(sReturnStatus);
        }

        switch(sReturnStatus) {
        case UIE_SCALE_MOTION:
        case UIE_SCALE_TIMEOUT:
        case UIE_SCALE_ERROR:
            /* retry as these are transient errors */
            break;

        default:
            sReturnStatus = UIE_SCALE_ERROR;
			/* fall through with modified return status */
        case UIE_SCALE_ERROR_COM:          /* communication error such as Virtual Serial Communication port removed */
        case UIE_SCALE_NOT_PROVIDE:        /* scale not provisioned with DeviceConfig utility */
        case UIE_SCALE_UNDER_CAPACITY:     /* scale measurment error */
        case UIE_SCALE_OVER_CAPACITY:      /* scale measurment error */
        case UIE_SCALE_DATAFORMAT:         /* scale message was invalid or ill formed error */
        case UIE_SCALE_DATAUNITS:
			return(sReturnStatus);
            break;
        }

		PifSleep (50);  // pause for a moment to let the scale debounce
    }

    return(sReturnStatus);
}


/*
*===========================================================================
**Synopsis: SHORT  ItemCommonGetStatus( UCHAR uchName )
*
*    Input: UCHAR uchName 
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return:  0 : The status bit is on. 
*         -1 : The status bit is off.
*
** Description: Return on/ off status of received status bit.  
*                
*===========================================================================
*/
SHORT  ItemCommonGetStatus( UCHAR uchName )
{
    if (ItemCommonLocal.fbCommonStatus & uchName) {
        return(ITM_SUCCESS);                /* bit is on */
    } else {
        return(ITM_ERROR);                  /* bit is off */
    }
}


/*
*===========================================================================
**Synopsis: VOID   ItemCommonPutStatus( USHORT usName )
*
*    Input: USHORT usName 
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return: : Nothing
*         
** Description: Turn on the the status bit received as a argument.  
*                
*===========================================================================
*/
VOID   ItemCommonPutStatus( USHORT usName )
{
    ItemCommonLocal.fbCommonStatus &= ~((UCHAR) usName);
    ItemCommonLocal.fbCommonStatus |= (UCHAR) usName;
}

/*
*===========================================================================
**Synopsis: VOID   ItemCommonResetStatus(USHORT usName)
*
*    Input: USHORT usName 
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return: : Nothing
*         
** Description: Turn on the the status bit received as a argument.  Saratoga
*===========================================================================
*/
VOID    ItemCommonResetStatus(USHORT usName)
{
    ItemCommonLocal.fbCommonStatus &= ~((UCHAR)usName);
}

/*
*===========================================================================
**Synopsis: USHORT   ItemCommonUniqueTransNo( VOID )
*
*    Input: Nothing
*   Output: Nothing
*    InOut: Nothing
**Return: : Nothing
*         
** Description: 
*===========================================================================
*/
USHORT  ItemCommonUniqueTransNo( VOID )
{
    UCHAR          uchUniqueAdr = CliReadUAddr();

    return(uchUniqueAdr * 100 + (TranCurQualPtr->usConsNo % 100));
}

/*
*===========================================================================
**Synopsis: USHORT   ItemCommonMldInit(VOID)
*
*    Input: Nothing
*   Output: Nothing
*    InOut: Nothing
**Return: : Nothing
*         
** Description: 
*===========================================================================
*/
VOID    ItemCommonMldInit( VOID )
{
    USHORT   usDisplay, usAttrib = 0;

    if (RflGetSystemType () == FLEX_STORE_RECALL) {
		FDTPARA        WorkFDT = { 0 };
		FDTParameter(&WorkFDT);

        if (WorkFDT.uchTypeTerm == FX_DRIVE_DELIV_1 || WorkFDT.uchTypeTerm == FX_DRIVE_DELIV_2) {
            usAttrib = 1;
        }
        usDisplay = MLD_DRIVE_1;
    } else {
        usDisplay = MLD_DRIVE_1 | MLD_DRIVE_2;
    }

    MldDisplayInit(usDisplay, usAttrib);
}

/*
*===========================================================================
**Synopsis: VOID    ItemCommonShiftPage(UCHAR uchPage)
*
*    Input: Nothing
*   Output: Nothing
*    InOut: Nothing
**Return: : Nothing
*         
** Description: 
*===========================================================================
*/
VOID    ItemCommonShiftPage(UCHAR uchPage)
{
    FSCTBL *pData = (FSCTBL *)&ParaFSC[uchPage - 1];

    UieSetFsc(pData);
}

#if 0
/*
*===========================================================================
** Synopsis:    VOID    ItemCommonFDTPara(ITEMFDTPARA *pData)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Return Terminal FDT Parameter(System, Terminal, Queue ...).
*               FDTParameter() function is the the same thing in the FDT subsystem.
*               It uses the FDTPARA struct which is the same as ITEMFDTPARA struct.
*               We are eliminating this clone of that function in order
*===========================================================================
*/

VOID    ItemCommonFDTPara(ITEMFDTPARA *pData)
{
    UCHAR   uchTermAddr = CliReadUAddr();
    USHORT  usDummy;
	PARAFXDRIVE     ParaFDT = { 0 };

	/*
	----- Set Terminal Unique Parameter -----
	We use CliReadUAddr to find out our terminal number.
	Terminal number is then used to index into the FDT terminal
	table data which is provisioned using PEP with Action Code 162.
	AC162 uses first four (4) table entries to determine the
	Delivery Terminal (addr 1 and 2) and the Payment Terminal (addr 3 and 4).

	So we take the terminal number, translate it to an odd value between 5 and 35
	to determine the index for the System Type Of Terminal to obtain the System
	Type Of Terminal settings for this terminal.
	Then we take the terminal number, translate it to an even value between 6 and 36
	to determine the index for the Terminal Type Of Terminal to obtain the Terminal
	Type of Terminal settings for this terminal.

	Read this with PEP up displaying Action Code 162 from the Maintenance Menu and
	all will be clear, Grasshopper.  Basically, each terminal has two different settings
	one after the other so the settings for terminal 1 are located in address 5 and 6
	for the System Type Of Terminal and the Terminal Type Of Terminal respectively.
	The settings for terminal 2 are located in address 7 and 8 and so on for each terminal
	up to terminal 16 where the settings are located in address 35 and 36.
	*/

	memset(pData, 0, sizeof(ITEMFDTPARA));

    CliParaAllRead(CLASS_PARAFXDRIVE, ParaFDT.uchTblData, sizeof(ParaFDT.uchTblData), 0, &usDummy);

    /*----- Set Terminal Unique Parameter -----*/
    pData->uchSysTypeTerm = ParaFDT.uchTblData[FX_DRIVE_SYSTERM_1 - 1 + (uchTermAddr - 1) * 2];
    pData->uchTypeTerm = ParaFDT.uchTblData[FX_DRIVE_TERMTYPE_1 - 1 + (uchTermAddr - 1) * 2];

    switch (pData->uchTypeTerm) {
    case    FX_DRIVE_ORDER_TERM_1:              /* Order Termninal (Queue#1) */
    case    FX_DRIVE_ORDER_PAYMENT_TERM_1:      /* Order/Payment Termninal (Queue#1) */
    case    FX_DRIVE_PAYMENT_TERM_1:            /* Payment Termninal (Queue#1) */
    case    FX_DRIVE_DELIV_1:                   /* Delivery Termninal (Queue#1) */
    case    FX_DRIVE_ORDER_FUL_TERM_1:          /* Order Full Screen Terminal #1 */
        pData->uchDelivery = ParaFDT.uchTblData[FX_DRIVE_DELIVERY_1];
        pData->uchPayment = ParaFDT.uchTblData[FX_DRIVE_PAYMENT_1];
        break;

    case    FX_DRIVE_ORDER_TERM_2:              /* Order Termninal (Queue#2) */
    case    FX_DRIVE_ORDER_PAYMENT_TERM_2:      /* Order/Payment Termninal (Queue#2) */
    case    FX_DRIVE_PAYMENT_TERM_2:            /* Payment Termninal (Queue#2) */
    case    FX_DRIVE_DELIV_2:                   /* Delivery Termninal (Queue#2) */
    case    FX_DRIVE_ORDER_FUL_TERM_2:          /* Order Full Screen Terminal #2 */
        pData->uchDelivery = ParaFDT.uchTblData[FX_DRIVE_DELIVERY_2];
        pData->uchPayment = ParaFDT.uchTblData[FX_DRIVE_PAYMENT_2];
        break;

    default:
        break;
    }
}
#endif

/*
*===========================================================================
** Synopsis:    VOID    ItemCommonDispSubTotal(VOID *pData)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Display Subtotal on LCD.
*===========================================================================
*/
VOID    ItemCommonDispSubTotal(CONST VOID *pData)
{
    DCURRENCY         lAmount;
    CONST ITEMSALES   *pSales = pData;
    CONST ITEMDISC    *pDisc = pData;
    CONST ITEMCOUPON  *pCoupon = pData;
    CONST ITEMTENDER  *pTender = pData;

    lAmount = TranItemizersPtr->lMI;
    lAmount += ItemTransLocalPtr->lWorkMI;             /* add multiple check payment amount */
	if (pData) {
		switch (ITEMSTORAGENONPTR(pData)->uchMajorClass) {
		case CLASS_ITEMSALES:
			switch(pSales->uchMinorClass) {
			case CLASS_DEPTMODDISC:
			case CLASS_PLUMODDISC:
			case CLASS_OEPMODDISC:
			case CLASS_SETMODDISC:
				lAmount += pSales->lDiscountAmount; /* set discount amount */
				break;

			default:        /* set noun and condiment amount */
				if ((pSales->fbStorageStatus & IS_CONDIMENT_EDIT) == 0) {
					DCURRENCY   lItemAmount = 0;
					ItemSalesSalesAmtCalc (pSales, &lItemAmount);
					lAmount += lItemAmount;
				}
				break;
			}
			break;

		case CLASS_ITEMDISC:
			lAmount += pDisc->lAmount;

			if (pDisc->uchMinorClass == CLASS_ITEMDISC1) {
				/* calculate parent plu price */
				pSales = &ItemCommonLocal.ItemSales;
				if (pSales->uchMajorClass == CLASS_ITEMSALES) {
					DCURRENCY   lItemAmount = 0;
					ItemSalesSalesAmtCalc (pSales, &lItemAmount);
					lAmount += lItemAmount;
				}
			}
			break;

		case CLASS_ITEMCOUPON:
			lAmount += pCoupon->lAmount;
			break;

		default:
			break;
		}
	}

    MldDispSubTotal(MLD_TOTAL_1, lAmount);
}



/*
*===========================================================================
** Synopsis:    VOID    ItemCommonSubTotalVoid(VOID *pData)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Display Subtotal on LCD.
*===========================================================================
*/
VOID    ItemCommonSubTotalVoid(VOID *pData)
{
	TrnItemSales ((ITEMSALES *)pData);
	return;
}


/*
*===========================================================================
** Synopsis:    VOID    ItemCommonDispECSubTotal(VOID *pData)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Display Subtotal on LCD for E/C.
*===========================================================================
*/
VOID    ItemCommonDispECSubTotal(CONST VOID *pData)
{
    DCURRENCY   lAmount;

    lAmount = TranItemizersPtr->lMI;
    lAmount += ItemTransLocalPtr->lWorkMI;                   /* add multiple check payment amount */

	if (pData) {
		switch (ITEMSTORAGENONPTR(pData)->uchMajorClass) {
		case CLASS_ITEMDISC:
			if (ITEMSTORAGENONPTR(pData)->uchMinorClass == CLASS_ITEMDISC1) {
				/* calculate parent plu price */
				ItemSalesSalesAmtCalc(&ItemCommonLocal.ItemSales, &lAmount);
			}
			break;

		default:
			break;
		}
	}

    MldDispSubTotal(MLD_TOTAL_1, lAmount);
}

/*
*===========================================================================
** Synopsis:    SHORT   ItemCommonCheckSize(VOID *pData, USHORT usSize)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:    compressed size
*             TRN_BUFFER_ILLEGAL   (error that is not storage related)
*             TRN_BUFFER_NEAR_FULL (storage is nearly full but not full)
*             TRN_BUFFER_FULL      (storage is full)
*
** Description: Check Storage Size(Transaction and Display Storage).
*               Uses the existing function TrnCheckSize() with a bit of a tweak
*               to the arguments.
*===========================================================================
*/
SHORT   ItemCommonCheckSize(VOID *pData, USHORT usSize)
{
    SHORT     sStatus;
    ITEMDISC  *pItemDisc = pData;

    /* V3.3 */
    if ((pItemDisc->uchMajorClass == CLASS_ITEMDISC) &&
         ((pItemDisc->uchMinorClass == CLASS_AUTOCHARGETIP) ||
          (pItemDisc->uchMinorClass == CLASS_AUTOCHARGETIP2) ||
          (pItemDisc->uchMinorClass == CLASS_AUTOCHARGETIP3))) {

          /* return always success, because of auto function */
          return(TRN_SUCCESS);
    } else {
        /* add sizeof auto charge tips at the time of other item enty */
        usSize += sizeof(ITEMDISC);
    }

    if (ItemSalesLocalPtr->fbSalesStatus & SALES_ITEMIZE) {
        if ((sStatus = TrnCheckSize(pData, usSize)) < 0) {
			if (sStatus == TRN_BUFFER_FULL) {
				TRANGCFQUAL   *pWorkGCF = TrnGetGCFQualPtr();
				pWorkGCF->fsGCFStatus |= GCFQUAL_BUFFER_FULL;
			}
			return sStatus;
        }
    } else {
        return(TrnCheckSize(pData, usSize));
    }

    if (MldCheckSize(pData, usSize) < 0) {
		MLDTRANSDATA        WorkMLD;

        if (ItemCommonLocal.ItemSales.uchMajorClass != CLASS_ITEMTOTAL) {
			ItemPreviousItem(0, 0);                /* Saratoga */
        }
        /* display from last itemize */
        MldPutTrnToScrollReverse(MldMainSetMldData(&WorkMLD));
    }

    return(sStatus);
}

/*
*===========================================================================
** Synopsis:    VOID    ItemCommonCancelGC(TRANGCFQUAL *pData)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Check Storage Size(Transaction and Display Storage).
*===========================================================================
*/
VOID    ItemCommonCancelGC(TRANGCFQUAL *pData)
{
    SHORT           sStatus;
    USHORT          usStatus, usType;

    if (RflGetSystemType() == FLEX_STORE_RECALL) {
		FDTPARA     WorkFDT = { 0 };

		FDTParameter(&WorkFDT);
        if (WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_TERM_1
            || WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_PAYMENT_TERM_1
            || WorkFDT.uchTypeTerm == FX_DRIVE_PAYMENT_TERM_1
            || WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_FUL_TERM_1) {

            /*----- Drive Through Type GCF -----*/
            if (pData->fsGCFStatus & GCFQUAL_DRIVE_THROUGH) {
                usType = GCF_APPEND_QUEUE1;

            /*----- Counter Type GCF -----*/
            } else {
                usType = GCF_NO_APPEND;
            }
        } else if (WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_TERM_2
            || WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_PAYMENT_TERM_2
            || WorkFDT.uchTypeTerm == FX_DRIVE_PAYMENT_TERM_2
            || WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_FUL_TERM_2) {

            if (pData->fsGCFStatus & GCFQUAL_DRIVE_THROUGH) {
				/*----- Drive Through Type GCF -----*/
                usType = GCF_APPEND_QUEUE2;

            } else {
				/*----- Counter Type GCF -----*/
                usType = GCF_NO_APPEND;
            }
        } else {
            usType = GCF_NO_APPEND;
        }
    } else {
        usType = GCF_NO_APPEND;
    }

	// Try to get the guest check.  If there is an error, then
	// popup a warning dialog.
    while ((sStatus = TrnCancelGC(pData->usGuestNo, usType)) != TRN_SUCCESS) {
        usStatus = GusConvertError(sStatus);
        UieErrorMsg(usStatus, UIE_WITHOUT);
    }
}

/*
*===========================================================================
** Synopsis:    VOID    ItemCommonScrollPause(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Pause for Scroll Display R3.1
*               Allow the transaction displayed in the LCD receipt window
*               to be scrolled up and down using the cursor buttons until such
*               time as the user presses the Cancel key.  This is basically
*               a way of allowing a user to view a list in a scrollable window
*               and scroll the list to view items in the list and nothing more.
*               So it can not be used to select items from a list for instance.
*===========================================================================
*/
VOID    ItemCommonScrollPause(VOID)
{
    for (;;) {
		UIFDIADATA      UI = {0};

        /* --- get target item --- */
        if (UifDiaScroll(&UI) != UIF_SUCCESS) {    /* abort by user     */
            break;
        }

        /* --- scroll up/down ---- */
        if (UI.auchFsc[0] == FSC_SCROLL_DOWN) {
            MldDownCursor();
            continue;

        } else if (UI.auchFsc[0] == FSC_SCROLL_UP) {
            MldUpCursor();
            continue;
        }
    }
}

/*
*===========================================================================
**Synopsis: SHORT   ItemCommonGetSRAutoNo(USHORT *pusGuestNo)
*
*    Input: 
*
*   Output: None
*
*    InOut: *pItemSales
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*
** Description:  Get Order # of Flexible Drive Through System.
*                   (Isolated from ItemSalesCommonIF(), R3.3
*
*                While this function is intended to be used for a Store Recall System
*                we will also allow it to be used with other kinds to create a
*                temporary guest check that has a Guest Check number of zero (0).
*                This allows the creation of a temporary guest check which can
*                be rung up and tendered out.  However Post Guest Check actions
*                such as Service Total to save the current guest check are not
*                allowed since the guest check does not exist in the guest check file.
*===========================================================================
*/
SHORT   ItemCommonGetSRAutoNo(USHORT *pusGuestNo)
{
    USHORT  usStatus = ITM_SUCCESS;

    *pusGuestNo = 0;
    if (RflGetSystemType () == FLEX_STORE_RECALL) {
		SHORT   sStatus = CliGusGetAutoNo(pusGuestNo);
        if (sStatus != GCF_SUCCESS) {
            usStatus = GusConvertError(sStatus);
        }
	}

    return(usStatus);
}

/*
*===========================================================================
**Synopsis: SHORT    ItemCommonCheckNonGCTrans(VOID)
*
*    Input: VOID 
*   Output: Nothing
*    InOut: Nothing
*
**Return: : LDT_SEQ_ADR
*         : ITM_SUCCESS
*         
** Description: Check non-GC transaction is allowed or not, V3.3
*===========================================================================
*/
SHORT   ItemCommonCheckNonGCTrans(VOID)
{
    if ((ItemSalesLocalPtr->fbSalesStatus & SALES_ITEMIZE) == 0) {           /* itemize state */
        /* check whether cashier operation is allowed or not, V3.3 */
        if (TranModeQualPtr->auchCasStatus[CAS_CASHIERSTATUS_0] & CAS_NOT_NON_GUEST_CHECK) {
			NHPOS_NONASSERT_NOTE("==PROVISION", "==PROVISION: Cashier settings prohibit non-check open.");
            return(LDT_PROHBT_ADR);
        }
    }
    return( ITM_SUCCESS );                                          
}

/*
*===========================================================================
**Synopsis: SHORT    ItemCommonCheckCashDrawerOpen(VOID)
*
*    Input: VOID 
*   Output: Nothing
*    InOut: Nothing
*
**Return: : LDT_DRAWCLSE_ADR
*         : ITM_SUCCESS
*         
** Description: Check non-GC transaction is allowed or not, V3.3
*===========================================================================
*/
SHORT  ItemCommonCheckCashDrawerOpen (VOID)
{
	SHORT      sRetStatus = 0;

	/* Check to see if both drawers are closed if compulsory drawer is set */
	if(ParaMDCCheck( MDC_DRAWER_ADR, EVEN_MDC_BIT1 ) == 0 ) {
		ULONG      ulLong[2] = {0};

		BlFwIfDrawerStatus(ulLong);

		if((ulLong[0] == BLFWIF_DRAWER_OPENED) || (ulLong[1] == BLFWIF_DRAWER_OPENED))
		{
			sRetStatus = LDT_DRAWCLSE_ADR;
		}
	}

	return sRetStatus;
}

/*
*===========================================================================
**Synopsis: SHORT    ItemCommonCheckNonGCTrans(VOID)
*
*    Input: VOID 
*   Output: Nothing
*    InOut: Nothing
*
**Return: : LDT_SEQ_ADR
*         : ITM_SUCCESS
*         
** Description: Check non-GC transaction is allowed or not, V3.3
*===========================================================================
*/
SHORT   ItemCommonCheckCloseCheck(VOID)
{
    if (((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_ADDCHECK ) && ((TranModeQualPtr->fsModeStatus & MODEQUAL_CASINT) == 0)) {
        if (TranModeQualPtr->auchCasStatus[CAS_CASHIERSTATUS_0] & CAS_NOT_CLOSE_ANY_CHECK) { /* prohibit any check close */
            /* V3.3 ICI */
            if (TranModeQualPtr->ulCashierID != TranGCFQualPtr->ulCashierID) {
				NHPOS_NONASSERT_NOTE("==PROVISION", "==PROVISION: Cashier settings prohibit any check close.");
                return(LDT_PROHBT_ADR);
            }
        }
        if (TranModeQualPtr->auchCasStatus[CAS_CASHIERSTATUS_0] & CAS_NOT_CLOSE_OWN_CHECK) { /* prohibit own check close */
            if (TranModeQualPtr->ulCashierID == TranGCFQualPtr->ulCashierID) {
				NHPOS_NONASSERT_NOTE("==PROVISION", "==PROVISION: Cashier settings prohibit own check close.");
                return(LDT_PROHBT_ADR);
            }
        }
    }

    return(ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemCommonTransOpen(USHORT usGuestNo)
*
*    Input: 
*
*   Output: None
*
*    InOut: *pItemSales
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*
** Description:  Create transaction open data
*                   (Isolated from ItemSalesCommonIF(), R3.3 
*                
*===========================================================================
*/
void ItemCommonResetLocalCounts (VOID)
{
	ItemMiscLocal.usCountPost = 0;
	ItemMiscLocal.usCountDemand = 0;
	ItemMiscLocal.usCountGift = 0;                 // current count of the number of receipts printed
	ItemMiscLocal.usCountPark = 0;                 // current count of the number of receipts printed
	ItemMiscLocal.usMaxReceiptsPost = CliParaMDCCheckField(MDC_RECEIPT_POST_LMT, MDC_PARAM_NIBBLE_B);
	ItemMiscLocal.usMaxReceiptsGift = CliParaMDCCheckField(MDC_RECEIPT_GIFT_LMT, MDC_PARAM_NIBBLE_B);
	ItemMiscLocal.usMaxReceiptsDemand = CliParaMDCCheckField(MDC_RECEIPT_POD_LMT, MDC_PARAM_NIBBLE_B);
	ItemMiscLocal.usMaxReceiptsPark = 3;
}

/*==========================================================================
**   Synopsis:  SHORT   ItemCommonTransOpen(USHORT usGuestNo)   
*
*   Input:      USHORT usGuestNo
*   Output:     none 
*   InOut:      none
*
*   Return:     
*
*   Description:    transaction open module main which handles starting a Guest
*                   Check by merely entering the first item of a transaction
*                   which will automatically set up the transaction environment.
*
*                   See function ItemTransOpen() which is called when a
*                   an actual Guest Check Key is pressed to start or recall a
*                   Guest Check.
*
*                   This functionality seems to be designed for a Store Recall
*                   System however it is also triggered in a Post Guest Check
*                   System if a first item is entered without pressing a
*                   Guest Check key first.  Creating a Post Guest Check with
*                   this functionality causes a restrictive type of guest check
*                   to be created which can not be Serviced, etc.
==========================================================================*/
SHORT   ItemCommonTransOpen(USHORT usGuestNo)
{
    TRANINFORMATION    *WorkTranInf = TrnGetTranInformationPointer();
	ITEMTRANSOPEN      TransOpenData = {0};
    SHORT              sReturnStatus;

    TransOpenData.uchMajorClass = CLASS_ITEMTRANSOPEN;

    /* check whether cashier operation is allowed or not, V3.3 */
    if (WorkTranInf->TranModeQual.auchCasStatus[CAS_CASHIERSTATUS_0] & CAS_NOT_NON_GUEST_CHECK) {
		NHPOS_NONASSERT_NOTE("==PROVISION", "==PROVISION: Cashier settings prohibit non-check open.");
        return(LDT_PROHBT_ADR);
    }

    ItemTendCPClearFile();                                  /* Saratoga */

    TransOpenData.ulCashierID = WorkTranInf->TranModeQual.ulCashierID;

    if (RflGetSystemType () == FLEX_STORE_RECALL) {         /* R3.0 */
        TransOpenData.uchMinorClass = CLASS_STORE_RECALL;
        TransOpenData.usGuestNo = usGuestNo;
    } else if (WorkTranInf->TranModeQual.fsModeStatus & MODEQUAL_CASINT) {          /* R3.3 */
        TransOpenData.uchMinorClass = CLASS_CASINTOPEN;

        /* set Guest Check No. as Cashier No. + 10000 */
        TransOpenData.usGuestNo = TRANSOPEN_GCNO(WorkTranInf->TranModeQual.ulCashierID);

        /* co-existence of normal guest check and cashier interrupt */
        if ( CliParaMDCCheckField ( MDC_GCNO_ADR, MDC_PARAM_BIT_C ) ) {
            TransOpenData.uchCdv = RflMakeCD( TransOpenData.usGuestNo );
        }
    } else {
        TransOpenData.uchMinorClass = CLASS_CASHIER;
    }
    TransOpenData.usNoPerson = 1;
    TransOpenData.fsPrintStatus = (PRT_SLIP | PRT_JOURNAL);

    if ( !(WorkTranInf->TranCurQual.fsCurStatus & CURQUAL_TRAY) ) {  /* tray total transaction */
        /* not clear display if tray total transaction */

        /*--- LCD Initialize ---*/
        ItemCommonMldInit();

        /*--- Display GCF/Order# ---*/
        MldScrollWrite(&TransOpenData, MLD_NEW_ITEMIZE);
        MldScrollFileWrite(&TransOpenData);

        ItemTransACStoOrderCounterOpen(0);  /* R3.0 */
        ItemTransACStoGetDelivOpen();       /* R3.0 */
        TrnInitialize( TRANI_GCFQUAL | TRANI_ITEMIZERS | TRANI_ITEM | TRANI_CONSOLI );
    }

    /*----- set print status -----*/
    ItemCurPrintStatus();               /* set print status of current qualifier */

    /*----- Count up consective No. -----*/
    ItemCountCons();                    /* increment consective No.*/

	TransOpenData.uchHourlyOffset = TranGCFQualPtr->uchHourlyOffset;

	if ( WorkTranInf->TranCurQual.fsCurStatus & CURQUAL_TRAY ) {  /* tray total transaction */
        ItemPromotion(PRT_RECEIPT, TYPE_TRAY);
    } else {
        ItemPromotion(PRT_RECEIPT, TYPE_STORAGE);
    }
    
    ItemHeader(TYPE_STORAGE);           /* send header print data */

	//we put some information at the beginning of the transaction that will
	//tell the application to go through the transaction, during the printing
	//phase to look for what type of order declaration we are in.
	//we do this so that we can allow the user to press order dec. any time
	//in the transaction, and it will always print to the top of the receipt. JHHJ 1-16-07
	ItemOrderDec(PRT_RECEIPT, TYPE_STORAGE);

    /* ---- unique transaction number R3.0 --- */
//    if ( CliParaMDCCheck( MDC_TRANNUM_ADR, EVEN_MDC_BIT0 ) ) {
//        TransOpenData.usTableNo = ItemCommonUniqueTransNo();
//    }

#if 0
    /*--- Beginning of Service Time,  R3.1 ---*/
	{
		DATE_TIME          WorkDate;        /* R3.1 */

		PifGetDateTime(&WorkDate);
		TransOpenData.auchCheckOpen[0] = (UCHAR)WorkDate.usHour;
		TransOpenData.auchCheckOpen[1] = (UCHAR)WorkDate.usMin;
		TransOpenData.auchCheckOpen[2] = (UCHAR)WorkDate.usSec;
	}
#endif

    memcpy (TransOpenData.auchCheckHashKey, TrnInformation.TranGCFQual.auchCheckHashKey, 6);

	if ((sReturnStatus = TrnOpen(&TransOpenData)) != ITM_SUCCESS) {
        return(sReturnStatus);
    }

    if ( !(WorkTranInf->TranCurQual.fsCurStatus & CURQUAL_TRAY) ) {  /* tray total transaction */
        /* send to enhanced kds, 2172 */
        sReturnStatus = ItemSendKds(&TransOpenData, 0);
        if (sReturnStatus != ITM_SUCCESS) {
            return sReturnStatus;
        }
    }

    /*----- Display Unique Transaction # to LCD, R3.0 -----*/
    if (TransOpenData.usTableNo) {
		ITEMMODIFIER       ItemModifier = {0};    /* R3.0 */

        /* set major/minor class*/
        ItemModifier.uchMajorClass = CLASS_ITEMMODIFIER;
        ItemModifier.uchMinorClass = CLASS_TABLENO;

        ItemModifier.usTableNo = TransOpenData.usTableNo;
        ItemModifier.usNoPerson = 1;

        /* send to enhanced kds, 2172 */
        sReturnStatus = ItemSendKds(&ItemModifier, 0);
        if (sReturnStatus != ITM_SUCCESS) {
            return sReturnStatus;
        }

        MldScrollWrite(&ItemModifier, MLD_NEW_ITEMIZE);
    }

    /*----- set non-guest check status, V3.3 ------*/
    ItemCommonLocal.fbCommonStatus |= COMMON_NON_GUEST_CHECK;

	// indicate that we should not allow a sign-out at this time.
	// this will prevent the operator from accidently pressing the
	// wrong key that will cause them to sign out with a transaction
	// still pending causing a PifAbort() when they try to sign back
	// in and causing problems with the guest check file as well.
	// When the check is tendered, the status bits will be cleared.
	ItemCommonLocal.fbCommonStatus |= COMMON_PROHIBIT_SIGNOUT;

	// Set the Print on Demand, Reprint Receipts limits and initialize
	ItemCommonResetLocalCounts ();

    return(ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: VOID    ItemCommonOpenAsk(EEPTRSPDATA *CPRcvData)
*    Input: UCHAR   
*   Output: None
*    InOut: None
**Return:   None
** Description: Transaction Open process.                       Saratoga
*===========================================================================
*/
VOID    ItemCommonOpenAsk(EEPTRSPDATA *CPRcv)
{
    TRANCURQUAL     *pWorkCur = TrnGetCurQualPtr();
	ITEMTRANSOPEN   TransOpen = {0};

    ItemTendCPClearFile();

    /*--- LCD Initialize ---*/
    ItemCommonMldInit();

    TransOpen.uchMajorClass  = CLASS_ITEMTRANSOPEN;
    TransOpen.uchMinorClass  = CLASS_CASHIER;

    if (CPRcv->auchMsgRecpt == '1' || CPRcv->auchMsgRecpt == '2') {
        pWorkCur->fbCompPrint |= CURQUAL_COMP_R;
    } else {
        pWorkCur->fbNoPrint |= CURQUAL_NO_R;
    }
    if (CPRcv->auchMsgSlip == '1') {
        pWorkCur->fbCompPrint |= CURQUAL_COMP_S;
    } else {
        pWorkCur->fbNoPrint |= CURQUAL_NO_S;
    }

    ItemCurPrintStatus();
    ItemCountCons();
	TransOpen.uchHourlyOffset = TranGCFQualPtr->uchHourlyOffset;
    ItemPrintStart(TYPE_THROUGH);
    TrnOpen(&TransOpen);
    ItemHeader(TYPE_STORAGE);

    ItemSalesGetLocalPointer()->fbSalesStatus |= SALES_ITEMIZE;

    UieModeChange(UIE_DISABLE);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemCommonTaxSystem(VOID)
*
*    Input: 
*   Output: None
*    InOut: 
**  Return: 
** Description:  Determine the tax system in use by looking at the MDC settings.
*
*                There is a similar function PrtCheckTaxSystem() in receipt printing.
*                There is a similar function TrnTaxSystem() in transaction data functionality.
*===========================================================================
*/
SHORT   ItemCommonTaxSystem(VOID)
{ 
    if (CliParaMDCCheckField (MDC_TAX_ADR, MDC_PARAM_BIT_D)) { 
        if (CliParaMDCCheckField (MDC_TAXINTL_ADR, MDC_PARAM_BIT_D)) { 
            return(ITM_TAX_INTL);
        } else {
            return(ITM_TAX_CANADA);
        }
    } else {
        return(ITM_TAX_US);
    }
}


/*
*===========================================================================
**Synopsis: VOID    ItemCommonDiscStatus(ITEMDISC *pItem, SHORT sType)
*
*    Input: 
*   Output: None
*    InOut: 
**  Return: 
** Description:     Set/Reset Discount/Surcharge Flag in TranQual
*                   We need to set or clear the indicator that a Discount/Surcharge
*                   has been used depending on several different conditions.
*                     discount being applied normal transaction
*                     discount being voided normal transaction
*                     discount being error corrected (sType == 1)
*===========================================================================
*/
VOID    ItemCommonDiscStatus(ITEMDISC *pItem, SHORT sType)
{
    ULONG       ulWork = 0, ulWork2 = 0;

    switch(pItem->uchMinorClass) {
    case    CLASS_REGDISC1:
        ulWork = GCFQUAL_REGULAR1;
        break;

    case CLASS_REGDISC2:
        ulWork = GCFQUAL_REGULAR2;
        break;

    case CLASS_REGDISC3:
        ulWork2 = GCFQUAL_REGULAR3;
        break;

    case CLASS_REGDISC4:
        ulWork2 = GCFQUAL_REGULAR4;
        break;

    case CLASS_REGDISC5:
        ulWork2 = GCFQUAL_REGULAR5;
        break;

    case CLASS_REGDISC6:
        ulWork2 = GCFQUAL_REGULAR6;
        break;

    case CLASS_CHARGETIP:           
    case CLASS_CHARGETIP2:          /* V3.3 */
    case CLASS_CHARGETIP3:           
        ulWork = GCFQUAL_CHARGETIP;
        break;

    case CLASS_AUTOCHARGETIP:       /* V3.3 */
    case CLASS_AUTOCHARGETIP2:           
    case CLASS_AUTOCHARGETIP3:           
        ulWork = GCFQUAL_CHARGETIP;
        ulWork2 = GCFQUAL_AUTOCHARGETIP;
        break;

    default:
        break;
    }

    /* --- Set Discount/Surcharge Bit in TranQual,  V3.3 --- */
    if (TrnSplCheckSplit() == TRN_SPL_SEAT
        || TrnSplCheckSplit() == TRN_SPL_MULTI
        || TrnSplCheckSplit() == TRN_SPL_TYPE2)
	{
		TRANGCFQUAL *WorkGCF = TrnGetGCFQualSplPointer(TRN_TYPE_SPLITA);

		NHPOS_ASSERT(sizeof(WorkGCF->fsGCFStatus) == sizeof(ulWork));
		NHPOS_ASSERT(sizeof(WorkGCF->fsGCFStatus2) == sizeof(ulWork2));

        if ((pItem->fbDiscModifier & VOID_MODIFIER) || sType == 1) {
			// if this is a void or if it is an Error Correct from ItemOtherECDisc()
            WorkGCF->fsGCFStatus &= ~ulWork;
            WorkGCF->fsGCFStatus2 &= ~ulWork2;
        } else {
            WorkGCF->fsGCFStatus |= ulWork;
            WorkGCF->fsGCFStatus2 |= ulWork2;
        }
    } else {
		TRANGCFQUAL *WorkGCF = TrnGetGCFQualPtr();

		NHPOS_ASSERT(sizeof(WorkGCF->fsGCFStatus) == sizeof(ulWork));
		NHPOS_ASSERT(sizeof(WorkGCF->fsGCFStatus2) == sizeof(ulWork2));

        /* --- Disc for Each Seat#, V3.3 --- */
        if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK &&
            !CliParaMDCCheckField (MDC_GCNO_ADR, MDC_PARAM_BIT_A) &&
            !CliParaMDCCheckField (MDC_SPLIT_GCF_ADR, MDC_PARAM_BIT_B) &&
            pItem->uchSeatNo != 0)
		{
			UCHAR    uchMaskSeatDisc = GCFQUAL_REG1_SEAT;

            if ((pItem->fbDiscModifier & VOID_MODIFIER) || sType == 1) {
				// if this is a void or if it is an Error Correct from ItemOtherECDisc()
                WorkGCF->auchSeatDisc[pItem->uchSeatNo - 1] &= ~(uchMaskSeatDisc << (pItem->uchMinorClass - CLASS_REGDISC1));
            } else {
                WorkGCF->auchSeatDisc[pItem->uchSeatNo - 1] |= (uchMaskSeatDisc << (pItem->uchMinorClass - CLASS_REGDISC1));
            }
        } else {
            if ((pItem->fbDiscModifier & VOID_MODIFIER) || sType == 1) {
				// if this is a void or if it is an Error Correct from ItemOtherECDisc()
                WorkGCF->fsGCFStatus &= ~ulWork;
                WorkGCF->fsGCFStatus2 &= ~ulWork2;
            } else {
                WorkGCF->fsGCFStatus |= ulWork;
                WorkGCF->fsGCFStatus2 |= ulWork2;
            }
        }
    }
}

/*
*===========================================================================
**Synopsis: SHORT   ItemCommonIncExcVAT(VOID)
*
*    Input: 
*   Output: None
*    InOut: 
**  Return: 
** Description:
*===========================================================================
*/
SHORT   ItemCommonIncExcVAT(VOID)
{ 
    if (CliParaMDCCheckField (MDC_VAT1_ADR, MDC_PARAM_BIT_B) ||
        CliParaMDCCheckField (MDC_VAT1_ADR, MDC_PARAM_BIT_A)) {
        return(1);
    } else {
        return(0);
    }
}

/*
*===========================================================================
**Synopsis: SHORT   ItemCommonLabelPosi(UCHAR *pszPluNumber);
*
* Input:    UCHAR  *pszPluNumber
*
**Return: offset a start position of label except scanner code('A' e.t.c.)
*         if return code is -1, it means a illegal label.
*         Support Scanner type : NCR 2170-K430
*                                NCR 7852
*                                NCR 7870 (only scanner)
*                                NCR 7890
*           not designate a type, these types are 7852, 7870 and 2170-K430. 2172
*===========================================================================
*/
SHORT   ItemCommonLabelPosi(TCHAR *pszPluNumber)
{
    SHORT   sOffset;
    TCHAR   uchTerm;

    uchTerm = *pszPluNumber;

    switch (uchTerm) {
    case    _T('A'): /* A0NSXXXXXXXXXXCD UPC-A *//* AXXXXXXXXXXXXCD EAN/JAN-13 */
    case    _T('B'):    /* BXXXXXXXCD    EAN/JAN-8 */
    case    _T('C'):    /* CXXXXXXCD     UPC-E */
    case    _T('E'):    /* E0XXXXXX      UPC-E (7890 scanner) */
        sOffset = 1;
        break;

    case    _T('F'):                        /* for 7890 scanner */
        uchTerm = *(pszPluNumber + 1);
        if (uchTerm == _T('F')) {
            sOffset = 2;                /* FFXXXXXXXCD     EAN-13 */
        } else {
            sOffset = 1;                /* FXXXXXXXXXXXXCD EAN-8 */
        }
        break;

	case	_T(']'):
		if((*(pszPluNumber + 1) == _T('e')) && (*(pszPluNumber + 2) == _T('0'))){
			sOffset = 5;
		} else {
			sOffset = -1;
		}
		break;

    default:            /* Code-39, Interiaved 2of5... and so on */
        sOffset = -1;                   /* not support code */
        break;
    }
    return (sOffset);
}

/*
*===========================================================================
**Synopsis: UCHAR   ItemCommonCDV(UCHAR *aszPluNo, SHORT sLen)
*
*    Input: 
*   Output: None
*    InOut: None
**Return:
*
** Description:     Check CDV
*===========================================================================
*/
TCHAR   ItemCommonCDV(TCHAR *aszPluNo, SHORT sLen)
{
    SHORT   i, sEven, sOdd, cdigit;

    for (sEven = sOdd = i = 0 ; i < sLen; ++i) {
        if (i % 2) {
            sOdd += aszPluNo[i] & 0x0f;         /* odd */
        } else {
            sEven += aszPluNo[i] & 0x0f;           /* even */
        }
    }
    cdigit = (sEven + sOdd*3) % 10;
    cdigit = (10 - cdigit) % 10;
    return((TCHAR)(cdigit | 0x30));
}

/*
*==========================================================================
**   Synopsis:  VOID    ItemWICRelOnFin(SHORT fsStatus)
*
*   Input:      
*   Output:     none
*   InOut:      none
*   Return:
*
*   Description:    Save WIC bit for release on finalize        Saratoga
*==========================================================================
*/
VOID    ItemWICRelOnFin(ULONG fsStatus)
{
    if ((fsStatus & CURQUAL_WIC) && CliParaMDCCheckField (MDC_WIC2_ADR, MDC_PARAM_BIT_C)) {
        TrnGetCurQualPtr()->fsCurStatus |= fsStatus;
    }
}

/** Check Endorsement, 21RFC05402 */
/*
*===========================================================================
**Synopsis: VOID    ItemCommonCheckEndorse(UCHAR uchMinorClass,
*                                          LONG  lAmount,
*                                          LONG  lSalesAmount)
*
*    Input: UCHAR   uchMinorClass
*           LONG    lAmount
*           LONG    lSalesAmount
*   Output: None
*    InOut: None
*
**Return:   None
*
** Description: Send check endorsement data to transaction module.  DTREE#2
*===========================================================================
*/
VOID    ItemCommonCheckEndorse(UCHAR uchMinorClass, 
                               DCURRENCY lAmount, DCURRENCY lSalesAmount, SHORT *psFlag)
{
	ITEMPRINT       Print = {0};

    if (TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)) {
        return;
    }

	if (!ItemTenderCheckTenderMdc(uchMinorClass, 4, MDC_PARAM_BIT_A)) {
		return;
	}

    if (*psFlag == 1) {
	    if ( CliParaMDCCheckField (MDC_VALIDATION_ADR, MDC_PARAM_BIT_B) ) {	  /* only slip validation, 03/23/01 */
    		if ( CliParaMDCCheckField (MDC_SLIPVAL_ADR, MDC_PARAM_BIT_D) ) {  /* both validation and slip print */
	    	    UieErrorMsg(LDT_VSLPREMV_ADR, UIE_WITHOUT);        /* display slip Paper Change error */
	    	} else {
    	        UieErrorMsg(LDT_PAPERCHG_ADR, UIE_WITHOUT);        /* display Paper Change */
	    	}
	    }
	}
    *psFlag = 1;

    Print.uchMajorClass = CLASS_ITEMPRINT;
    Print.uchMinorClass = CLASS_CHECK_ENDORSE;
    Print.fsPrintStatus |= PRT_VALIDATION;
    Print.lAmount       = lAmount;
    Print.lMI           = lSalesAmount;
    Print.uchStatus     = uchMinorClass;
	
	if (ItemTenderCheckTenderMdc(uchMinorClass, 5, MDC_PARAM_BIT_A)) {
		// print endorsement check inserted horizontally.  The default is check inserted vertical.
		// by vertical we mean that the short edge of the check is inserted into slip station with
		// the long edge against the side so we need a shorter printed line length.
		Print.uchPrintSelect |= PRT_ENDORSE_HORIZNTL;
	}

    TrnThrough(&Print);
}

/* SR 143 cwunn @/For Key ItemCalAmount() ported from 2170GP
*===========================================================================
**Synopsis: LONG    ItemCalAmount(LONG lQty,       USHORT usFor
*                                 LONG lUnitPrice, DCURRENCY   *plAmount)
*
*    Input: LONG    lUnitPrice	(the package price)
*           LONG    lQty		(the number sold)
*           USHORT  usFor		(the number of items in a package)
*   Output: DCURRENCY    *plAmount	(the calculated total price of the sale)
*    InOut: None
*
**Return:   LDT_KEYOVER_ADR, ITM_SUCCESS
*
** Description: Calculates the total price for partial package sales.
*			For example, 5 items sold at 3 for $1 produces a $1.66 total price
*				lQty would = 5, usFor = 3, lUnitPrice = 1.33
*			
*===========================================================================
*/

SHORT ItemCalAmount(LONG lQty, USHORT usFor, LONG lUnitPrice, DCURRENCY *plAmount)
{
    D13DIGITS d13Work;
    SHORT     fsReverse = 0;

    if (lQty < 0) {
        lQty *= -1L;
        fsReverse = 1;
    }

    if (!usFor) {
        /* --- x  Qty  and adjust decimal point --- */
        d13Work = lQty;
		d13Work *= lUnitPrice;

        if (lQty % 1000L) {
            /* --- decimal point used --- */
            /* --- under 1 digits of decimal point for rounding --- */
            d13Work /= 100L;

            /* --- check overflow --- */
            if (d13Work > STD_MAX_TOTAL_AMT || d13Work < - STD_MAX_TOTAL_AMT) {   /* Change for R2.0 */
                 return  (LDT_KEYOVER_ADR);
             } else {
                /* --- rounding by rounding table --- */
                if (RflRoundDcurrency(plAmount, d13Work, 11) != RFL_SUCCESS) {
                    return (LDT_PROHBT_ADR);
                }
             }
        } else {
            /* --- decimal point not used --- */
            d13Work /= 1000L;

            /* --- check overflow --- */
			if (d13Work > STD_MAX_TOTAL_AMT || d13Work < - STD_MAX_TOTAL_AMT) {    /* Change for R2.0 */   /* Added 97/5/22 by Y.Sakuma */
                return  (LDT_KEYOVER_ADR);
            } else {
                *plAmount =  d13Work;
            }
        }
    } else {
		LONG       lQtyWork;
		D13DIGITS  d13Pack;      /* --- Unit price x no. of pack --- */

        lQtyWork = lQty / (usFor * 1000L);
        lQty = lQty - (lQtyWork * usFor * 1000L);

        /* --- Unit price x no. of pack --- */
        d13Pack = lQtyWork;
		d13Pack *= lUnitPrice;

        if (1 //CliParaMDCCheck(MDC_FOR_ADR, ODD_MDC_BIT2) //cwunn SR 143 @/For Key
         || (lQty % 1000L)) {
            /* --- ROUNDING-UP PACKAGE PRICE --- */
            /* --- calcurate Unit Price x Qty / For --- */
            d13Work = lQty;
			d13Work *= lUnitPrice;
            d13Work /= usFor;

            /* --- rounding and adjust decimal point --- */
            d13Work += 500L; /*900L*/; //SR281 ESMITH
            d13Work /= 1000L;
        } else {
			D13DIGITS   d13Remain;

            /* --- calcurate Unit Price x Qty / For --- */
            d13Work = lQty;
			d13Work *= lUnitPrice;
			d13Remain = d13Work;
            d13Work /= (LONG)usFor * 1000L;
			d13Remain -= d13Work * (LONG)usFor * 1000L;

            if (d13Remain) {
				LONG    lWork;

                /* --- calcurate Unit Price / For x Qty --- */
                /* --- Unit Price / For --- */
                lWork = lUnitPrice * 10L;
                lWork = lWork / usFor;
                lWork = (lWork + 9) / 10L;      /* --- rounding --- */

                /* --- x  Qty  and adjust decimal point --- */
                d13Work = lQty;
				d13Work *= lWork;
                d13Work /= 1000L;
            }
        }

        d13Work += d13Pack;

        /* --- check overflow --- */
		if (d13Work > STD_MAX_TOTAL_AMT || d13Work < - STD_MAX_TOTAL_AMT)  {   /* Change for R2.0 */  /* Added 97/5/22 by Y.Sakuma */
            return  (LDT_KEYOVER_ADR);
        } else {
            *plAmount =  d13Work;
        }
    }
    if (fsReverse) {
        *plAmount *= -1L;
    }
    return  (ITM_SUCCESS);
}

/* dollar tree */
/****** End of Source ******/