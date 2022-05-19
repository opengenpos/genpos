/*=======================================================================*\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  2170         *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995-9         ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
:   Title          : TENDER MODULE
:   Category       : TENDER MODULE, NCR 2170 US Hospitality Application
:   Program Name   : ITTEND4.C (FC total)
:  ---------------------------------------------------------------------  
:  Abstract:                  
:   ItemTendFCTotal()       ;   foreign currency total
:   ItemTendFCCTotal()      ;   generate check total
:
:  ---------------------------------------------------------------------  
:  Update Histories                                                         
:   Date   : Ver.Rev.  : Name      : Description
: Nov-21-95: 03.01.00  : hkato     : R3.1
: Jun-28-96: 03.01.09  : M.Ozawa   : display foreign currency total on KDS
: Aug-21-98: 03.03.00  : hrkato    : V3.3 (VAT/Service)
: Dec-06-99: 01.00.00  : hrkato    : Saratoga
: Jul-28-17: 02.02.02  : R.Chambers : removed unneeded include files, localized variables
-------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*\
:   PVCS ENTRY
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/
#include	<tchar.h>
#include    <string.h>

#include    "ecr.h"
#include    "uie.h"
#include    "pif.h"
#include    "paraequ.h"
#include    "para.h"
#include    "display.h"
#include    "regstrct.h"
#include    "uireg.h"
#include    "transact.h"
#include    "item.h"
#include    "csstbpar.h"
#include    "rfl.h"
#include    "cpm.h"
#include    "itmlocal.h"


/*==========================================================================
**  Synopsis:   SHORT   ItemTendFCTotal( UIFREGTENDER *UifRegTender )         
*
*   Input:      UIFREGTENDER    *UifRegTender              
*   Output:     none  
*   InOut:      none
*
*   Return:     
*
*   Description:    foreign currency total
==========================================================================*/

SHORT   ItemTendFCTotal( UIFREGTENDER *UifRegTender )
{
    TRANGCFQUAL     *WorkGCF = TrnGetGCFQualPtr();
    DCURRENCY       lAmount, lForeign;
    PARAMDC         WorkMDC, WorkMDC2;
    PARACURRENCYTBL WorkRate;
	REGDISPMSG      DispMsg = {0};
	ITEMTOTAL       ItemTotal = {0};  /* R3.1 */
	USHORT          usUifRegTenderMinorClass = UifRegTender->uchMinorClass;
	USHORT          usTranAddrs;
    UCHAR           uchType;

	if (usUifRegTenderMinorClass >= CLASS_UIFOREIGN1 && usUifRegTenderMinorClass <= CLASS_UIFOREIGN8) {  // convert tender to total type in this function
		usUifRegTenderMinorClass = usUifRegTenderMinorClass - CLASS_UIFOREIGN1 + CLASS_UIFOREIGNTOTAL1;
	}

    if ( TranCurQualPtr->auchTotalStatus[0] == 0 ) {                /* not depress total key */
        return( LDT_SEQERR_ADR );                                   /* sequence error */
    }

    /*----- Split Check,   R3.1 -----*/
    if (TrnSplCheckSplit() == TRN_SPL_SEAT
        || TrnSplCheckSplit() == TRN_SPL_MULTI
        || TrnSplCheckSplit() == TRN_SPL_TYPE2) {
        ItemTotalPrevSpl();                                         /* send previous "item" */

    } else {
        ItemTotalPrev();                                            /* send previous "item" */
    }

    /*----- Split Check,   R3.1 -----*/
    if (ItemTenderLocal.fbTenderStatus[1] & TENDER_SPLIT_TENDER) {
        lAmount = ItemTenderLocal.lSplitMI - ItemTenderLocal.lTenderizer;

    } else {
		SHORT  sSplitStatus = TrnSplCheckSplit();

        if (sSplitStatus == TRN_SPL_SEAT
            || sSplitStatus == TRN_SPL_MULTI
            || sSplitStatus == TRN_SPL_TYPE2) {
            ItemTendCalAmountSpl(&lAmount, UifRegTender);
        } else if (sSplitStatus == TRN_SPL_SPLIT) {
            ItemTendCalAmountSplit(&lAmount, UifRegTender);
        } else {
            ItemTendCalAmount(&lAmount, UifRegTender);
        }
    }
                                    
    lAmount += ItemTransLocalPtr->lWorkMI;                                   /* multi check payment */

    WorkMDC.uchMajorClass = CLASS_PARAMDC;
    WorkRate.uchMajorClass = CLASS_PARACURRENCYTBL;                  
    WorkMDC2.uchMajorClass = CLASS_PARAMDC;							/* 03/02/01 */

    switch (usUifRegTenderMinorClass) {
    case CLASS_UIFOREIGNTOTAL1:
        WorkRate.uchAddress = CNV_NO1_ADR;
        uchType = RND_FOREIGN1_1_ADR;
        usTranAddrs = TRN_FT1_ADR;
        ItemTenderLocal.fbTenderStatus[2] |= TENDER_FC1;
        WorkMDC.usAddress = MDC_FC1_ADR;
        WorkMDC2.usAddress = MDC_EUROFC_ADR;							/* 03/02/01 */
        break;

    case CLASS_UIFOREIGNTOTAL2:
        WorkRate.uchAddress = CNV_NO2_ADR;
        uchType = RND_FOREIGN2_1_ADR;
        usTranAddrs = TRN_FT2_ADR;
        ItemTenderLocal.fbTenderStatus[2] |= TENDER_FC2;
        WorkMDC.usAddress = MDC_FC2_ADR;
        WorkMDC2.usAddress = MDC_EUROFC_ADR;							/* 03/02/01 */
        break;

    case CLASS_UIFOREIGNTOTAL3:
        WorkRate.uchAddress = CNV_NO3_ADR;
        uchType = RND_FOREIGN3_1_ADR;
        usTranAddrs = TRN_FT3_ADR;
        ItemTenderLocal.fbTenderStatus[2] |= TENDER_FC3;
        WorkMDC.usAddress = MDC_FC3_ADR;
        WorkMDC2.usAddress = MDC_EUROFC2_ADR;							/* 03/02/01 */
        break;

    case CLASS_UIFOREIGNTOTAL4:
        WorkRate.uchAddress = CNV_NO4_ADR;
        uchType = RND_FOREIGN4_1_ADR;
        usTranAddrs = TRN_FT4_ADR;
        ItemTenderLocal.fbTenderStatus[2] |= TENDER_FC4;
        WorkMDC.usAddress = MDC_FC4_ADR;
        WorkMDC2.usAddress = MDC_EUROFC2_ADR;							/* 03/02/01 */
        break;

    case CLASS_UIFOREIGNTOTAL5:
        WorkRate.uchAddress = CNV_NO5_ADR;
        uchType = RND_FOREIGN5_1_ADR;
        usTranAddrs = TRN_FT5_ADR;
        ItemTenderLocal.fbTenderStatus[2] |= TENDER_FC5;
        WorkMDC.usAddress = MDC_FC5_ADR;
        WorkMDC2.usAddress = MDC_EUROFC3_ADR;							/* 03/02/01 */
        break;

    case CLASS_UIFOREIGNTOTAL6:
        WorkRate.uchAddress = CNV_NO6_ADR;
        uchType = RND_FOREIGN6_1_ADR;
        usTranAddrs = TRN_FT6_ADR;
        ItemTenderLocal.fbTenderStatus[2] |= TENDER_FC6;
        WorkMDC.usAddress = MDC_FC6_ADR;
        WorkMDC2.usAddress = MDC_EUROFC3_ADR;							/* 03/02/01 */
        break;

    case CLASS_UIFOREIGNTOTAL7:
        WorkRate.uchAddress = CNV_NO7_ADR;
        uchType = RND_FOREIGN7_1_ADR;
        usTranAddrs = TRN_FT7_ADR;
        ItemTenderLocal.fbTenderStatus[2] |= TENDER_FC7;
        WorkMDC.usAddress = MDC_FC7_ADR;
        WorkMDC2.usAddress = MDC_EUROFC4_ADR;							/* 03/02/01 */
        break;

    case CLASS_UIFOREIGNTOTAL8:
        WorkRate.uchAddress = CNV_NO8_ADR;
        uchType = RND_FOREIGN8_1_ADR;
        usTranAddrs = TRN_FT8_ADR;
        ItemTenderLocal.fbTenderStatus[2] |= TENDER_FC8;
        WorkMDC.usAddress = MDC_FC8_ADR;
        WorkMDC2.usAddress = MDC_EUROFC4_ADR;							/* 03/02/01 */
        break;
    }

    CliParaRead( &WorkMDC );                                        /* get target MDC */
    CliParaRead( &WorkRate );                                       /* get FC rate */
    CliParaRead( &WorkMDC2 );                                       /* get target MDC, 03/02/01 */

	// manipulate the raw MDC byte retrieved so that if it is an even numbered
	// MDC address, we will do the shift needed to bring the most significant nibble
	// to the right due to the way MDC information is stored in memory.
    if ((WorkMDC.usAddress & 0x01) == 0) {  /* if even numbered MDC address, right justify upper nibble */
        WorkMDC.uchMDCData >>= 4;
    }

    /* V3.4 */
    /*** deleted, 03/02/01
    WorkMDC2.uchMajorClass = CLASS_PARAMDC;
    WorkMDC2.usAddress = MDC_EUROFC_ADR;
    CliParaRead( &WorkMDC2 );                                        / get target MDC /
	***/
	/* 03/02/01, correct Euro MDC handling */
	// following ensures that the two bits indicating how the rate format and
	// the conversion method are in the least two significant bit positions.
	// this shift normalizes the parameter so that Bit B or Bit D is in the least
	// significant bit position (bit 0) and Bit A or Bit C is in the next position (bit 1).
	switch (usUifRegTenderMinorClass) {
		case CLASS_UIFOREIGNTOTAL1:
		case CLASS_UIFOREIGNTOTAL5:
			WorkMDC2.uchMDCData >>= 4;    // shift upper nibble Bit C and Bit D to least significant bit positions
			break;
		case CLASS_UIFOREIGNTOTAL2:
		case CLASS_UIFOREIGNTOTAL6:
			WorkMDC2.uchMDCData >>= 6;    // shift upper nibble Bit A and Bit B to least significant bit positions
			break;
		case CLASS_UIFOREIGNTOTAL3:
		case CLASS_UIFOREIGNTOTAL7:
			;  // no shift needed as Bit C and Bit D are already in least signifcant bit position
			break;
		case CLASS_UIFOREIGNTOTAL4:
		case CLASS_UIFOREIGNTOTAL8:
			WorkMDC2.uchMDCData >>= 2; // shift lower nibble Bit A and Bit B to least significant bit positions
			break;
	}

    /* --- F/C Rounding,    Saratoga --- */
    if (CliParaMDCCheck(MDC_ROUND2_ADR, EVEN_MDC_BIT0)) {
        RflRound(&lAmount, lAmount, RND_TOTAL1_ADR);
    }

    if (CliParaMDCCheck(MDC_EURO_ADR, ODD_MDC_BIT0)) {
        /* Euro enhancement, V3.4 */
		// Bit C of MDC address 278 governing foreign currency #1 is unused so assume default for it.
        if ((WorkMDC2.uchMDCData & 0x02) && (usUifRegTenderMinorClass != CLASS_UIFOREIGNTOTAL1)) {
             /* convert FC to FC through Euro */
            ItemTendFCRateCalcByEuroB(&lForeign, lAmount, WorkRate.ulForeignCurrency, uchType, FC_EURO_TYPE1, WorkMDC2.uchMDCData);
        } else {
            ItemTendFCRateCalc1(WorkMDC2.uchMDCData, &lForeign, lAmount, WorkRate.ulForeignCurrency, uchType);
        }
    } else {
        RflRateCalc1(&lForeign, lAmount, WorkRate.ulForeignCurrency, uchType);
    }

    ItemTenderLocal.lFCMI = lForeign;                               /* save foreign currency total */

    DispMsg.uchMajorClass = UifRegTender->uchMajorClass;
    DispMsg.uchMinorClass = UifRegTender->uchMinorClass;

    DispMsg.lAmount = lForeign;
    RflGetTranMnem (DispMsg.aszMnemonic, usTranAddrs);

	// Check the MDC settings that have been retrieved for this foreign currency tender key
	// and set the bits for the display correctly.  Default is 2 decimal places.
    if ( WorkMDC.uchMDCData & 0x02 ) {
        DispMsg.fbSaveControl |= DISP_SAVE_DECIMAL_3;               /* save for redisplay (not recovery) */
        if ( WorkMDC.uchMDCData & 0x04 ) {
            DispMsg.fbSaveControl |= DISP_SAVE_DECIMAL_0;           /* save for redisplay (not recovery) */
        }
    }

    flDispRegDescrControl |= TOTAL_CNTRL;                           /* descriptor */
                                                                                                                                            
    if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
        if ( WorkGCF->fsGCFStatus & GCFQUAL_GSTEXEMPT || WorkGCF->fsGCFStatus & GCFQUAL_PSTEXEMPT ) {
            flDispRegDescrControl |= TAXEXMPT_CNTRL;                    /* tax exempt */
            if ( ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL ) {
                flDispRegKeepControl |= TAXEXMPT_CNTRL;                 /* tax exempt */
            } else {
                flDispRegKeepControl &= ~TAXEXMPT_CNTRL;                /* tax exempt */
            }
        }
    } else if (ItemCommonTaxSystem() == ITM_TAX_US) {                   /* V3.3 */
       if ( WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT ) {
            flDispRegDescrControl |= TAXEXMPT_CNTRL;                    /* tax exempt */
            if ( ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL ) {
                flDispRegKeepControl |= TAXEXMPT_CNTRL;                 /* tax exempt */
            } else {
                flDispRegKeepControl &= ~TAXEXMPT_CNTRL;                /* tax exempt */
            }
        }
    }
    DispMsg.fbSaveControl |= 1;                                     /* save for redisplay (not recovery) */
    DispWrite( &DispMsg );                                          /* display check total */

    if (ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL) {
        flDispRegDescrControl &= ~TOTAL_CNTRL;                      /* descriptor */

    } else {
        flDispRegDescrControl &= ~( TOTAL_CNTRL | TAXEXMPT_CNTRL ); /* descriptor */
    }

    /* ---- display foreign currency total on KDS R3.1 ---- */
    ItemTotal.uchMajorClass = CLASS_ITEMTOTAL;
    ItemTotal.uchMinorClass = (UCHAR)(CLASS_FOREIGNTOTAL1 + usUifRegTenderMinorClass - CLASS_UIFOREIGNTOTAL1);
    ItemTotal.lMI = lForeign;
    ItemTotal.auchTotalStatus[0] = WorkMDC.uchMDCData;                 /* tender MDC */
    TrnThroughDisp(&ItemTotal);

    return( ITM_SUCCESS );
}

/*==========================================================================
**  Synopsis:   DCURRENCY    ItemTendFCCTotal( VOID )   
*
*   Input:      none       
*   Output:     none  
*   InOut:      none
*
*   Return:     
*
*   Description:    generate check total data (for display)
==========================================================================*/

DCURRENCY    ItemTendFCCTotal( VOID )
{               
    UIFREGTOTAL UifRegTotal = {0};                        
    ITEMTOTAL   ItemTotal = {0};
    ITEMCOMMONTAX   WorkTax = {0};
	ITEMCOMMONVAT   WorkVAT = {0};

    UifRegTotal.uchMajorClass = CLASS_UIFREGTOTAL;                  /* total key */    
    UifRegTotal.uchMinorClass = CLASS_UITOTAL2;                     /* total key */    

    ItemTotalCTTax(&UifRegTotal, &ItemTotal, &WorkTax, &WorkVAT);   /* V3.3 */
    ItemTotalCTAmount( &ItemTotal );                                /* calculate MI */

    return( ItemTotal.lMI );                                        /* native balance */                                
}        

/****** End of Source ******/