/*=======================================================================*\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  2170         *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1998-9         ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
:   Title          : TENDER MODULE
:   Category       : TENDER MODULE, NCR 2170 US Hospitality Application
:   Program Name   : ITTEND.C (main)
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
:   USHORT  ItemTenderCheckTenderMdc (UCHAR uchMinorClass, UCHAR uchPageAddress, UCHAR uchField)
:   SHORT   ItemTendGetTotal(UCHAR *auchTotalStatus, UCHAR uchMinor)
:   ItemTenderEntry()            ;   tender module main
:
:  ---------------------------------------------------------------------
: Update Histories
:    Date  : Ver.Rev. :   Name      : Description
: Mar-23-95: 03.00.00 : H.Kato      : R3.0
: Nov-21-95: 03.01.00 : H.Kato      : R3.1
: Aug-04-98: 03.03.00 : H.Kato      : V3.3
: Aug-11-99: 03.05.00 : K.Iwata     : R3.5 (remove WAITER_MODEL)
: Aug-13-99: 03.05.00 : K.Iwata     : Merge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
:
:** NCR2171 **
: Aug-26-99: 01.00.00 : M.Teraki    : initial (for 2171)
: Dec-06-99: 01.00.00 : hrkato      : Saratoga
:
:** GenPOS **
: Apr-08-15: 02.02.01 : R.Chambers  : source cleanup, use ItemTotalAuchTotalStatus()
: Dec-08-15: 02.02.01 : R.Chambers  : use Tender Key #1 total key for all Preset Tenders fixing tax issue.
: Sep-07-18: 02.02.02 : R.Chambers  : Add FSC_PRESET_AMT #5, #6, #7, #8
-------------------------------------------------------------------------*/

#include	<tchar.h>
#include	<math.h>
#include    <stdio.h>
#include    <string.h>

#include    "ecr.h"
#include    "uie.h"
#include    "pif.h"
#include    "display.h"
#include    "paraequ.h"
#include    "para.h"
#include    "mld.h"
#include    "regstrct.h"
#include    "uireg.h"
#include    "transact.h"
#include    "item.h"
#include    "csstbpar.h"
#include    "csttl.h"
#include    "csstbgcf.h"
#include    "csgcs.h"
#include    "rfl.h"
#include    "cpm.h"
#include    "fdt.h"
#include    "itmlocal.h"
#include    "pmg.h"
#include    "prt.h"
#include    "trans.h"
#include    "pifmain.h"

ITEMTENDERLOCAL     ItemTenderLocal;                        /* tender local data */
/* --- Saratoga --- */


/*==========================================================================
**  Synopsis:   SHORT   ItemTenderGetLocal( ITEMTENDERLOCAL *pData )
**  Synopsis:   SHORT   ItemTenderPutLocal( ITEMTENDERLOCAL *pData )
*
*   Input:      ITEMTENDERLOCAL *pData
*   Output:     ITEMTENDERLOCAL *pData
*   InOut:      none
*
*   Return:
*
*   Description:    get tender local data
*   Description:    put tender local data
==========================================================================*/

CONST volatile ITEMTENDERLOCAL * CONST ItemTenderLocalPtr = &ItemTenderLocal;

ITEMTENDERLOCAL *ItemTenderGetLocalPointer (VOID)
{
    return &ItemTenderLocal;
}

VOID    ItemTenderGetLocal( ITEMTENDERLOCAL *pData )
{
    *pData = ItemTenderLocal;
}

VOID    ItemTenderPutLocal( ITEMTENDERLOCAL *pData )
{
    ItemTenderLocal = *pData;
}


USHORT  ItemTenderLocalCheckTenderid (USHORT usOpCode, USHORT usNewValue)
{
	switch (usOpCode) {
		case 0:
			ItemTenderLocal.usCheckTenderId = 1;   // ItemTenderLocalCheckTenderid(), usOpCode = 0, init value
			break;
		case 1:
			ItemTenderLocal.usCheckTenderId++;     // ItemTenderLocalCheckTenderid(), usOpCode = 1, increment value
			break;
		case 2:
			ItemTenderLocal.usCheckTenderId = usNewValue;     // ItemTenderLocalCheckTenderid(), usOpCode = 2, set value
			break;
		case 3:
			break;                                 // ItemTenderLocalCheckTenderid(), usOpCode = 3, return value of ItemTenderLocal.usCheckTenderId
		default:
			break;
	}
	return ItemTenderLocal.usCheckTenderId;
}
	

/*
		Due to the complexity of the tender key MDC settings which are in several different MDC areas
		depending on when the additional functionality was added, this function accepts the tender key
		minor class (indincating which tender key is being processed, along with an MDC page address
		indicating which of the different MDC addresses or pages is being accessed and then the bit
		field that is being tested.

		uchMinorClass contains the tender key minor class such as CLASS_UITENDER8
		uchPageAddress contains the address of the page containing the MDC bit, range is 1 to 5.
		uchField contains the field as required by CliParaMDCCheckField() such as MDC_PARAM_BIT_B

		If you look in file paraequ.h you will see that the MDC addresses have several different ranges
		depending on when the tender keys were added and when new tender functionality was added.  The
		sequence of additions is roughly as follows:
		  - Tender keys 1 through 8 were the first added with two MDC pages/addresses for each key.
		  - Charge posting for tender keys 1 through 11 was added with a single MDC page/address for each key
		  - Tender keys 9 through 11 were next added with two MDC pages/address for each key.
		  - page/address 3 and 4 were added for tender keys 1 through 11.
		  - Tender keys 12 through 20 were added with four MDC pages/addresses for each key
		    . the new MDC pages/addresses were added in groups of two similar to the older keys
			. there are dependencies in various areas of the source on the MDC pages/addresses numbering

		This is the arguments for this function and the data they provide.
		  - uchMinorClass is the tender key number which is currently 1 through 20
		  - uchPageAddress is the PEP MDC page number, there are 5 pages as follows
		     . page 1 and page 2 are consecutive numbered MDC addresses
			    : tender key 1 uses addresses 63 and 64
			    : tender key 9 uses addresses 191 and 192
			    : tender key 12 uses addresses 385 and 386
			 . page 3 is the charge post MDC address
			    : tender key 1 uses address 180
			    : tender key 9 uses address 188
			    : tender key 12 uses address 403
			 - page 4 and 5 are consecutive numbered MDC addresses
			    : tender key 1 uses addresses 201 and 202
			    : tender key 9 uses addresses 217 and 218
			    : tender key 12 uses addresses 412 and 413
			 . page 6 is the options MDC address using consecutive numbered addresses
			    : tender key 1 uses address 483
			    : tender key 9 uses address 491
			    : tender key 12 uses address 494
		  - uchField is the bit field such as MDC_PARAM_BIT_A

	  WARNING:  The Foreign Currency Tenders have different MDC bit usage than standard Tenders.
	            This function can not be used with Foreign Currency Tenders.

	  NOTE: The Preset Amount tender key, FSC_PRESET_AMT, is indicated by a uchMajorClass of CLASS_UIFREGPAMTTENDER
	        and the uchMinorClass is the preset key number or indicator, 1 through FSC_PREAMT_MAX, which is the
			value used in the P15 table lookup to determine the Preset Tender Key amount assigned to the tender key.
			All Preset Amount Tender Key should use Tender Key #1 for purposes of determining the MDC settings to
			use since Tender Key #1 is the legacy Cash Tender key.

**/
USHORT  ItemTenderCheckTenderMdc (UCHAR uchMinorClass, UCHAR uchPageAddress, UCHAR uchField)
{
	USHORT  usAddress;

	if (uchMinorClass >= CLASS_UIFOREIGN1 && uchMinorClass <= CLASS_UIFOREIGN8) {
		// if this is a foreign tender then use the settings for tender number 1, CLASS_UITENDER1
		// see also similar decision in function ParaTendInfoRead().
		uchMinorClass = CLASS_UITENDER1;  // CLASS_UIFOREIGN1 to CLASS_UIFOREIGN8 treated like CLASS_UITENDER1
	}

	if (uchMinorClass <= CLASS_UITENDER8 ) {
		switch (uchPageAddress) {
			case MDC_TENDER_PAGE_1:
				usAddress = (uchMinorClass - CLASS_UITENDER1) * 2 + MDC_TEND11_ADR;
				break;

			case MDC_TENDER_PAGE_2:
				usAddress = (uchMinorClass - CLASS_UITENDER1) * 2 + MDC_TEND12_ADR;
				break;

			case MDC_TENDER_PAGE_3:
				usAddress = uchMinorClass - CLASS_UITENDER1 + MDC_CPTEND1_ADR;
				break;

			case MDC_TENDER_PAGE_4:
				usAddress = (uchMinorClass - CLASS_UITENDER1) * 2 + MDC_TEND13_ADR;
				break;

			case MDC_TENDER_PAGE_5:
				usAddress = (uchMinorClass - CLASS_UITENDER1) * 2 + MDC_TEND14_ADR;
				break;

			case MDC_TENDER_PAGE_6:    // MDC addresses 483 through 502
				usAddress = (uchMinorClass - CLASS_UITENDER1) + MDC_TEND15_ADR;
				break;

			default:
				NHPOS_ASSERT_TEXT((uchPageAddress <= MDC_TENDER_PAGE_6), "Tender key page address out of range.");
				return 0;
		}
    } else if (uchMinorClass <= CLASS_UITENDER11){
		switch (uchPageAddress) {
			case MDC_TENDER_PAGE_1:
				usAddress = (uchMinorClass - CLASS_UITENDER9) * 2 + MDC_TEND91_ADR;
				break;

			case MDC_TENDER_PAGE_2:
				usAddress = (uchMinorClass - CLASS_UITENDER9) * 2 + MDC_TEND92_ADR;
				break;

			case MDC_TENDER_PAGE_3:
				usAddress = uchMinorClass - CLASS_UITENDER9 + MDC_CPTEND9_ADR;
				break;

			case MDC_TENDER_PAGE_4:
				usAddress = (uchMinorClass - CLASS_UITENDER9) * 2 + MDC_TEND93_ADR;
				break;

			case MDC_TENDER_PAGE_5:
				usAddress = (uchMinorClass - CLASS_UITENDER9) * 2 + MDC_TEND94_ADR;
				break;

			case MDC_TENDER_PAGE_6:    // MDC addresses 483 through 502
				usAddress = (uchMinorClass - CLASS_UITENDER9) + MDC_TEND95_ADR;
				break;

			default:
				NHPOS_ASSERT_TEXT((uchPageAddress <= MDC_TENDER_PAGE_6), "Tender key page address out of range.");
				return 0;
		}
    } else if (uchMinorClass <= CLASS_UITENDER20){
		switch (uchPageAddress) {
			case MDC_TENDER_PAGE_1:
				usAddress = (uchMinorClass - CLASS_UITENDER12) * 2 + MDC_TEND1201_ADR;
				break;

			case MDC_TENDER_PAGE_2:
				usAddress = (uchMinorClass - CLASS_UITENDER12) * 2 + MDC_TEND1202_ADR;
				break;

			case MDC_TENDER_PAGE_3:
				usAddress = uchMinorClass - CLASS_UITENDER12 + MDC_CPTEND12_ADR;
				break;

			case MDC_TENDER_PAGE_4:
				usAddress = (uchMinorClass - CLASS_UITENDER12) * 2 + MDC_TEND1203_ADR;
				break;

			case MDC_TENDER_PAGE_5:
				usAddress = (uchMinorClass - CLASS_UITENDER12) * 2 + MDC_TEND1204_ADR;
				break;

			case MDC_TENDER_PAGE_6:
				usAddress = (uchMinorClass - CLASS_UITENDER12) + MDC_TEND1205_ADR;
				break;

			default:
				NHPOS_ASSERT_TEXT((uchPageAddress <= MDC_TENDER_PAGE_6), "Tender key page address out of range.");
				return 0;
		}
	} else {
		// no log as this may be due to use of foreign currency tender.
		// NHPOS_ASSERT_TEXT((uchMinorClass <= CLASS_UITENDER20), "Tender key minor class out of range.");
		return 0;
	}

	return CliParaMDCCheckField(usAddress, uchField);
}

/*
	The Tender Key status flag bits array, auchStatus, has a particular structure that
	corresponds to the following:
	 - offset 0: contains the first two MDC pages for the tender key
	   . for example for Tender key #3 whose first two pages are MDC address 67 and 68.
	     . low order bits in UCHAR are first page (MDC address 67 for Tender Key #3)
		 . high order bits in UCHAR are second page (MDC address 68 for Tender Key #3)
	 - offset 1: contains third MDC pages for the tender key
	    . for example for Tender key #3 whose third page is MDC address 182
		  . low order bits in UCHAR are third page (MDC address 182 for Tender Key #3)
**/
USHORT  ItemTenderMdcToStatus (UCHAR uchMinorClass, UCHAR auchStatus[3])
{
	UCHAR  ucBitMap = 0;

	ucBitMap = ItemTenderCheckTenderMdc (uchMinorClass, MDC_TENDER_PAGE_1, MDC_PARAM_NIBBLE_B);
	ucBitMap |= (ItemTenderCheckTenderMdc (uchMinorClass, MDC_TENDER_PAGE_2, MDC_PARAM_NIBBLE_B) << 4);

	auchStatus[0] = ucBitMap;

	ucBitMap = ItemTenderCheckTenderMdc (uchMinorClass, MDC_TENDER_PAGE_3, MDC_PARAM_NIBBLE_B);
	auchStatus[1] = ucBitMap;

	return 0;
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTendPartCheck( UIFREGTENDER *UifRegTender ) 
*
*   Input:      UIFREGTENDER    *UifRegTender
*   Output:     none   
*   InOut:      none
*
*   Return:     
*
*   Description:    check if partial tender is allowed with current transaction state
*                   and tender key settings.
==========================================================================*/
static SHORT   ItemTendPartCheck( UIFREGTENDER *UifRegTender )
{
    if ( ItemModLocalPtr->fsTaxable ) {                                      /* depress taxable/exempt */
        return( LDT_SEQERR_ADR );                                   /* sequence error */
    }

    if (UifRegTender->uchMajorClass != CLASS_UIFREGPAMTTENDER) {
		// check if tender amount entry allowed. checked only if not a preset amount tender key press.
		if ( ItemTenderCheckTenderMdc(UifRegTender->uchMinorClass, 1, MDC_PARAM_BIT_D ) && ItemTenderCheckTenderMdc(UifRegTender->uchMinorClass, 1, MDC_PARAM_BIT_C ) ) { /* check prohibit amount */ 
			NHPOS_ASSERT_TEXT(0, "==NOTE: ItemTendPartCheck() amount entry prohibited this tender.");
			return( LDT_PROHBT_ADR );
		}

		if ( ItemTenderCheckTenderMdc(UifRegTender->uchMinorClass, 1, MDC_PARAM_BIT_A ) ) {  /* prohibit partial tender */
			NHPOS_ASSERT_TEXT(0, "==NOTE: ItemTendPartCheck() partial tender prohibited this tender.");
			return( LDT_PROHBT_ADR );                                   /* prohibit operation */
		}
	}

    return( ITM_SUCCESS );
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTendOverCheck( UIFREGTENDER *UifRegTender,
*                           ITEMTENDER *ItemTender ) 
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender        
*   Output:     none   
*   InOut:      none
*
*   Return:     
*
*   Description:    check even/over tender is allowed with current transaction state
*                   and tender key settings.
==========================================================================*/
static SHORT   ItemTendOverCheck( UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender )
{
    if ( ItemModLocalPtr->fsTaxable ) {                                      /* depress taxable/exempt modifier */
        return( LDT_SEQERR_ADR );                                   /* sequence error */
    }

	if (UifRegTender->uchMajorClass != CLASS_UIFREGPAMTTENDER) {        
		// check if tender amount entry allowed. checked only if not a preset amount tender key press.
        if ( ItemTenderCheckTenderMdc(UifRegTender->uchMinorClass, 1, MDC_PARAM_BIT_D ) && ItemTenderCheckTenderMdc(UifRegTender->uchMinorClass, 1, MDC_PARAM_BIT_C ) ) {   /* check prohibit amount? */
			NHPOS_ASSERT_TEXT(0, "==NOTE: ItemTendOverCheck() amount entry prohibited this tender.");
            return( LDT_PROHBT_ADR );
        }

		if ( ItemTenderCheckTenderMdc(UifRegTender->uchMinorClass, 1, MDC_PARAM_BIT_B ) ) {  /* prohibit over amount entry */
			if ( ItemTender->lChange ) {                                /* exist change */
				NHPOS_ASSERT_TEXT(0, "==NOTE: ItemTendOverCheck() over amount prohibited this tender.");
				return( LDT_PROHBT_ADR );                               /* prohibit operation */
			}
		}
    }

    if (ItemTenderLocal.ItemFSTotal.uchMajorClass != 0 && ! (ItemTenderLocal.fbTenderStatus[1] & TENDER_FS_PARTIAL)) {
        return(LDT_SEQERR_ADR);
    }

    return( ITM_SUCCESS );
}

/*==========================================================================
**   Synopsis:  SHORT   ItemTender( UIFREGTENDER *UifRegTender )
*
*   Input:      UIFREGTENDER    *UifRegTender
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    tender module main
==========================================================================*/
SHORT   ItemTenderEntry( UIFREGTENDER *UifRegTender )
{
    SHORT       sEPTTend;
    SHORT       sStatus, sRetStatus;
    USHORT      usStatus;
    ITEMTENDER  ItemTender;

#if 0
	// removed as causes problem with receipt for Amtrak
    if(UifRegTender->uchMajorClass == CLASS_UIFREGPAMTTENDER){
		UIFREGTOTAL UifRegTotal;
		PARATEND        ParaTend;
		memset(&UifRegTotal, 0, sizeof(UifRegTotal));
		ParaTend.uchMajorClass = CLASS_PARATEND;
        ParaTend.uchAddress = CLASS_UITENDER1;//one is for the cash button, which is tender one
		CliParaRead(&ParaTend);
		UifRegTotal.uchMajorClass = CLASS_UIFREGTOTAL;
		UifRegTotal.uchMinorClass = ParaTend.uchTend;

		ItemTotal(&UifRegTotal);
	}
#endif

    if ( ( usStatus = ItemCommonCheckComp() ) != ITM_SUCCESS ) {
        return( ( SHORT )usStatus );
    }

	// check to see if this is a Return transaction and if so then
	// at least one item has been marked as a return with a reason code.
	if (TranGCFQualPtr->fsGCFStatus & GCFQUAL_TRETURN) {
		// this is a return so lets see if any items are being returned.
		// if nothing marked as a return then prohibit.
		if ((ItemSalesLocalPtr->fbSalesStatus & SALES_TRETURN_ITEM) == 0) {
			return(LDT_PROHBT_ADR);           /* return compulsory error */
		}
		// set the indicator so that we can
		// inform other parts of the tender functionality of this fact.
		UifRegTender->fbModifier |= RETURNS_TRANINDIC;

		// do not allow foreign tenders to be used with a return.
		if (UifRegTender->uchMinorClass >= CLASS_UIFOREIGNTOTAL1 && UifRegTender->uchMinorClass <= CLASS_UIFOREIGN8) {
			if (TranItemizersPtr->lMI < 0)        /* check transaction itemizers main total */
				return(LDT_PROHBT_ADR);           /* return compulsory error */
		}
	}

	// Check No Purchase functionality we will check to see if we should allow it or not.
	// Check No Purchase is only allowed for Tender keys 8, 9, and 10 so if this is one of
	// those tender keys and if the tender type is check and if it is a Bad Check Server
	// transaction type then we will just indicate that Check No Purchase is allowed.
	if (uchIsNoCheckOn) {
		if (UifRegTender->lTenderAmount < 1) {  // Check No Purchase requires some tender amount
			return(LDT_DATA_ADR);
		}
		if (CliParaMDCCheck(MDC_CHK_NO_PURCH, ODD_MDC_BIT0)) {
			if (UifRegTender->uchMinorClass >= CLASS_UITENDER8 && UifRegTender->uchMinorClass <= CLASS_UITENDER10) {
				PARATENDERKEYINFO	TendKeyInfo;

				TendKeyInfo.uchMajorClass = CLASS_PARATENDERKEYINFO;
				TendKeyInfo.uchAddress = UifRegTender->uchMinorClass;
				CliParaRead(&TendKeyInfo);

				if (TendKeyInfo.TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_CHECK) {
					switch(UifRegTender->uchMinorClass)
					{
						case CLASS_UITENDER8:												  //If the tender pressed is 8,	
							if(ParaMDCCheck(MDC_TEND84_ADR, EVEN_MDC_BIT2)){	 //check to see if it is assigned to Check no Purchase
								fsPrtCompul = PRT_RECEIPT;      //Print values that need to be set here, because when doing Check no Purchase, they never 
								fsPrtNoPrtMask = ~PRT_SLIP;     //get set.  no slip printing wanted.
							} else {
								return LDT_BLOCKEDBYMDC_ADR;
							}
							break;
						case CLASS_UITENDER9:												  //If the tender pressed is 9
							if(ParaMDCCheck(MDC_TEND94_ADR, EVEN_MDC_BIT2)){	 //check to see if it is assigned to Check No Purchase
								fsPrtCompul = PRT_RECEIPT;      //Print values that need to be set here, because when doing Check no Purchase, they never 
								fsPrtNoPrtMask = ~PRT_SLIP;     //get set.  no slip printing wanted.
							} else {
								return LDT_BLOCKEDBYMDC_ADR;
							}
							break;
						case CLASS_UITENDER10:											  //If the tender pressed is 10	
							if(ParaMDCCheck(MDC_TEND104_ADR,EVEN_MDC_BIT2)){	 //check to see if it is assigned to Check No Purchase
								fsPrtCompul = PRT_RECEIPT;      //Print values that need to be set here, because when doing Check no Purchase, they never 
								fsPrtNoPrtMask = ~PRT_SLIP;     //get set.  no slip printing wanted.
							} else {
								return LDT_BLOCKEDBYMDC_ADR;
							}
							break;
					}
				} else {
					NHPOS_ASSERT_TEXT((TendKeyInfo.TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_CHECK), "==PROVISION:  Check No Purchase Invalid Tender type");
					return(LDT_EQUIPPROVERROR_ADR);
				}
			} else {
				NHPOS_NONASSERT_NOTE("==PROVISION", "==PROVISION:  MDC 379 Bit D prohibits Check No Purchase.");
				return(LDT_BLOCKEDBYMDC_ADR);
			}
		} else {
			NHPOS_ASSERT_TEXT((uchIsNoCheckOn == 0), "==PROVISION:  Check No Purchase Invalid Tender Key");
			return(LDT_SEQERR_ADR);
		}
	} else {
		// check to see if a guest check has been started.  If not then Prohibit.
		// tender should only be used after items entered unless Check No Purchase.
		// check for tray total since Tray Total Key press will clear (ItemSalesLocalPtr->fbSalesStatus & SALES_ITEMIZE).
		if ((ItemSalesLocalPtr->fbSalesStatus & SALES_ITEMIZE) == 0 && (TranCurQualPtr->fsCurStatus & CURQUAL_TRAY) == 0) {
			return(LDT_SEQERR_ADR);
		}
	}

	if(uchIsNoCheckOn && CliParaMDCCheck(MDC_CHK_NO_PURCH, ODD_MDC_BIT1) && CliParaMDCCheck(MDC_CHK_NO_PURCH, ODD_MDC_BIT0)){       // 8-18-03 JHHJ SR18
		if( ItemSPVInt(LDT_SUPINTR_ADR) != ITM_SUCCESS) {
			return(LDT_SEQERR_ADR);
		}
	}

    /*----- check Split Key -> Tender Case 5/28/96 -----*/
    if (TrnSplCheckSplit() & (TRN_SPL_SEAT | TRN_SPL_MULTI | TRN_SPL_TYPE2 | TRN_SPL_SPLIT)) {
        if (TranCurQualPtr->uchSeat == 0 && TranCurQualPtr->auchTotalStatus[0] == 0) {
            return(LDT_SEQERR_ADR);
        }
    }

    /* automatic charge tips, V3.3 */
    if (ItemTotalAutoChargeTips() == ITM_SUCCESS) {
        if (TrnSplCheckSplit() & (TRN_SPL_SEAT | TRN_SPL_MULTI | TRN_SPL_TYPE2))
		{
            ItemPreviousItemSpl(0, 0);               /* Saratoga */
        } else {
            ItemPreviousItem(0, 0 );                 /* Saratoga */
        }
    }

    memset( &ItemTender, 0, sizeof( ITEMTENDER ) );

	if (UifRegTender->uchMinorClass <= CLASS_UITENDER20) {
		// if this is one of the standard native currency tenders then set the ItemTender class information.
		// for other, foreign currency tenders, we will wait.
		ItemTender.uchMajorClass = CLASS_ITEMTENDER;                   /* set major class */
		ItemTender.uchMinorClass = UifRegTender->uchMinorClass;        /* set minor class */
	}

	{
		PARATENDERKEYINFO TendKeyInfo;

		TendKeyInfo.uchMajorClass = CLASS_PARATENDERKEYINFO;
		TendKeyInfo.uchAddress = UifRegTender->uchMinorClass;
		CliParaRead(&TendKeyInfo);

		if (ItemSalesLocalPtr->fbSalesStatus & SALES_PREAUTH_BATCH) {
			if (TendKeyInfo.TenderKeyInfo.usTranCode != TENDERKEY_TRANCODE_PREAUTHCAPTURE) {
				// We are currently doing a Preauth Batch transaction so the only tender key
				// we will allow at this point is a Preauth Capture tender key.
				return(LDT_PROHBT_ADR);
			}
		} else {
			if (TendKeyInfo.TenderKeyInfo.usTranCode == TENDERKEY_TRANCODE_PREAUTHCAPTURE) {
				// We are not currently doing a Preauth Batch transaction so do not allow
				// a Preauth Capture tender key
				return(LDT_PROHBT_ADR);
			}
		}
	}
	{
		ULONG         ulfsCurStatus = 0;
		TRANGCFQUAL   *pWorkGCF = TrnGetGCFQualPtr();

		ItemTender.usCheckTenderId = ItemTenderLocalCheckTenderid (3, 0);
		UifSetUniqueIdWithTenderId (pWorkGCF->uchUniqueIdentifier, ItemTender.usCheckTenderId);

		// removing this for now until we figure out how to handle transaction discounts
		// with returns.  Richard Chambers, Oct 26, 2012 for Amtrak.
		if ((ulfsCurStatus = TrnQualModifierReturnData (0, 0)) & (CURQUAL_PRETURN | CURQUAL_TRETURN)) {
//			ITEMTENDERLOCAL WorkTend;

			ItemTender.fbReduceStatus |= REDUCE_RETURNED;

#if 0
			// if this is the first or only tender key press and this is a return of some kind
			// then we will need to pro rate any transaction discounts that may be part of
			// the original to this part of the transaction that is being returned.
			ItemTenderGetLocal(&WorkTend);
			if ((WorkTend.fbTenderStatus[0] & TENDER_PARTIAL) == 0) {
				ItemProcessRegularDiscountsForReturn();
			}
#endif
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

	sRetStatus = ItemTendGiftCard(UifRegTender);		/* Generates Gift Card Information   SSTOLTZ*/
	if(sRetStatus != UIF_SUCCESS)
		return sRetStatus;

    /* --- Saratoga FVT,    Food Stamp Tender --- */
    if (ItemTendCheckFS(UifRegTender) == ITM_SUCCESS) {
		// For Amtrak we will not allow use of tip with tenders other than Electronic Payment.
		if (ItemCommonLocalPtr->lChargeTip != 0) {
			return (CliGusLoadTenderTableConvertError (RETURNS_MSG_TIPREQUIRESCC));
		}

		// this is a food stamp.  check if this is a return.  if so prohibit.
		if (UifRegTender->fbModifier & RETURNS_TRANINDIC) {
			if (TranItemizersPtr->lMI < 0)            /* check transaction itemizers total */
				return(LDT_PROHBT_ADR);
		}

        sStatus = ItemTendFS(UifRegTender);
		return sStatus;
    }

    if ((sStatus = ItemTendCommon(UifRegTender, &ItemTender)) != ITM_SUCCESS) {
        /* auto charge tips, V3.3 */
        ItemPrevCancelAutoTips();
        return(sStatus);
    }

    if ((sStatus = ItemTendCheckWIC(UifRegTender, &ItemTender)) != UIF_SUCCESS) {
        return(sStatus);
    }

    /* check EPT tender */
    sEPTTend = ItemTendCheckEPT(UifRegTender);
    if (sEPTTend == LDT_PROHBT_ADR) {
        /* auto charge tips, V3.3 */
        ItemPrevCancelAutoTips();
        return(sEPTTend);
    }

	{
		if (ItemCommonLocalPtr->lChargeTip != 0 && sEPTTend != ITM_EPT_TENDER) {
			// For Amtrak we will not allow use of tip with tenders other than Electronic Payment.
			// We also require credit card for a return of a tip.  However if this is an exchange
			// and the customer owes money due to buying more stuff, then any tender is acceptable.
			if (ItemCheckCustomerSettingsFlags (SYSCONFIG_CUSTOMER_ENABLE_AMTRAK)) {
				if ((TranGCFQualPtr->fsGCFStatus & GCFQUAL_TRETURN) == 0) {
					return (CliGusLoadTenderTableConvertError (RETURNS_MSG_TIPREQUIRESCC));
				} else if (ItemTender.lRound < 0) {
					return (CliGusLoadTenderTableConvertError (RETURNS_MSG_TIPREQUIRESCC));
				}
			}
		}

		/* check charge posting system */
		/* if charge posting system, do charge posting function */
		if ( ( sStatus = ItemTendCP( UifRegTender, &ItemTender ) ) != ITM_SUCCESS ) {
			if (sStatus == ITM_PARTTEND) {
				/* auto charge tips, V3.3 */
				ItemTendAutoChargeTips();
				sStatus = ItemTendPart(UifRegTender, &ItemTender);    /* partial tender */
			}
			else if (sStatus == ITM_OVERTEND) {
				ItemTendAutoChargeTips();
				sStatus = ItemTendOver(UifRegTender, &ItemTender);
			}
			else if (sStatus == ITM_REJECT_DECLINED) {
				// Since this is a Declined transaction we need to print a receipt containing the information
				// about the Declined transaction. The customer can use this as a reference document, if desired,
				// when working with their card provider for any reason. Or it can be just thrown away.
				// We are not going to put this into the Electronic Journal.  Paul Stewart of Mercury Payments
				// has this to say:
				//   "The main reason people store the receipts is for chargeback so this would
				//    not ever be used I would not bother to store it."
				ItemTender.fsPrintStatus = (PRT_SINGLE_RECPT | PRT_HEADER24_PRINT | PRT_RECEIPT);
				TrnOtherSummaryReceipt(&ItemTender, -1);

				/* auto charge tips, V3.3 */
				ItemPrevCancelAutoTips();

				// fall on through to finish error processing.
			}
			else {
				// if there is an error with an electronic payment tender attempt then we will save
				// the current invoice number to allow it to be reused in a retry.
				// if the same invoice number and amount is used
				if (sStatus == LDT_REQST_TIMED_OUT && TrnGetTransactionInvoiceNumTenderIfValid(&ItemTender) != 3) {
					ITEMAFFECTION   Affect = { 0 };

					Affect.uchMajorClass = CLASS_ITEMAFFECTION;
					Affect.uchMinorClass = CLASS_EPT_ERROR_AFFECT;
					if (ItemTender.lTenderAmount >= 0) {
						Affect.lBonus[BONUS_ERROR_AFFECT_SALE] = ItemTender.lTenderAmount;
						Affect.sItemCounter = 1;
					}
					else {
						Affect.lBonus[BONUS_ERROR_AFFECT_VOIDSALE] = ItemTender.lTenderAmount;
						Affect.sItemCounter = -1;
					}
					Affect.ulId = TranModeQualPtr->ulCashierID;

					TrnAffection(&Affect);

					TrnSaveTransactionInvoiceNum(&ItemTender);    // save the current invoice number to allow for a retry.
				}
				else if (sStatus != LDT_REQST_TIMED_OUT) {
					TrnClearSavedTransactionInvoiceNum();
				}
				/* auto charge tips, V3.3 */
				ItemPrevCancelAutoTips();
				return( sStatus );
			}
			TrnClearSavedTransactionInvoiceNum();
			switch (sStatus) {
				case UIF_SUCCESS:                               /* success */
				case UIF_FINALIZE:                              /* finalize */
				case UIF_CAS_SIGNOUT:                           /* cashier sign out */
				case UIF_WAI_SIGNOUT:                           /* waiter sign out */
					{
						ItemTenderLocalCheckTenderid (1, 0);
						CliGusLoadTenderTableAdd (&ItemTender);
					}
					break;
			}
			return(sStatus);
		}

	}

	if (TrnGetTransactionInvoiceNumTenderIfValid(&ItemTender) == 3) {
		ITEMAFFECTION   Affect = { 0 };

		Affect.uchMajorClass = CLASS_ITEMAFFECTION;
		Affect.uchMinorClass = CLASS_EPT_ERROR_AFFECT;
		if (ItemTender.lTenderAmount >= 0) {
			Affect.lBonus[BONUS_ERROR_AFFECT_SALE] = ItemTender.lTenderAmount * (-1);
			Affect.sItemCounter = -1;
		}
		else {
			Affect.lBonus[BONUS_ERROR_AFFECT_VOIDSALE] = ItemTender.lTenderAmount * (-1);
			Affect.sItemCounter = -1;
		}
		Affect.ulId = TranModeQualPtr->ulCashierID;

		TrnAffection(&Affect);
	}
	TrnClearSavedTransactionInvoiceNum();
	/* auto charge tips, V3.3 */
	ItemTendAutoChargeTips();

	{
		TRANGCFQUAL   *WorkGCF = TrnGetGCFQualPtr();
		ULONG         fbModifier = TrnQualModifierReturnTypeTest ();
		ULONG         ulCashierIDTemp, ulStoreregNoTemp;
		USHORT        usConsNoTemp, usGuestNoTemp;
		SHORT         sRetStatus;

		// Indicate the returns type so that totals will be updated properly
		ItemTender.fbModifier |= (fbModifier & (RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3));

		// save a copy of the data which may be changed so that we can do our logging below if enabled.
		usConsNoTemp = WorkGCF->usConsNo;
		usGuestNoTemp = WorkGCF->usGuestNo;
		ulStoreregNoTemp = WorkGCF->ulStoreregNo;
		ulCashierIDTemp = TranModeQualPtr->ulCashierID;

		switch( ItemTendState( UifRegTender, &ItemTender ) ) {
		case    0   :                                                   /* w/o amount tender */
			sRetStatus = ItemTendFast( UifRegTender, &ItemTender );
			WorkGCF->lCurBalance = ItemTender.lBalanceDue;    // Save any balance due after tender for display later
			break;

		case    1   :                                                   /* partial tender */
			sRetStatus = ItemTendPart( UifRegTender, &ItemTender );
			WorkGCF->lCurBalance = ItemTender.lBalanceDue;    // Save any balance due after tender for display later
			break;

		case    2   :                                                   /* even/over tender */
		case    3   :                                                   /* even/over tender */
			sRetStatus = ItemTendOver( UifRegTender, &ItemTender );
			WorkGCF->lCurBalance = ItemTender.lBalanceDue;    // Save any balance due after tender for display later
			break;

		case    4   :                                                   /* even/over tender */
			sRetStatus = ItemTendFCTotal( UifRegTender );
			WorkGCF->lCurBalance = ItemTender.lBalanceDue;    // Save any balance due after tender for display later
			break;

		case    5   :                                                   /* w/o amount tender */
			sRetStatus = ItemTendFCFast( UifRegTender, &ItemTender );
			WorkGCF->lCurBalance = ItemTender.lBalanceDue;    // Save any balance due after tender for display later
			break;

		case    6   :                                                   /* partial tender */
			sRetStatus = ItemTendFCPart( UifRegTender, &ItemTender );
			WorkGCF->lCurBalance = ItemTender.lBalanceDue;    // Save any balance due after tender for display later
			break;

		case    7   :                                                   /* even/over tender */
		case    8   :                                                   /* even/over tender */
			sRetStatus = ItemTendFCOver( UifRegTender, &ItemTender );
			WorkGCF->lCurBalance = ItemTender.lBalanceDue;    // Save any balance due after tender for display later
			break;

		default :
			sRetStatus = (LDT_SEQERR_ADR);
			break;
		}

		switch (sRetStatus) {
			case UIF_SUCCESS:                               /* success */
			case UIF_FINALIZE:                              /* finalize */
			case UIF_CAS_SIGNOUT:                           /* cashier sign out */
			case UIF_WAI_SIGNOUT:                           /* waiter sign out */
				{
					ItemTenderLocalCheckTenderid (1, 0);
					CliGusLoadTenderTableAdd (&ItemTender);
				}
				break;
		}

		if (RflLoggingRulesTransactions()) {
			char transBuff[256];
#if defined(DCURRENCY_LONGLONG)
			sprintf(transBuff, "%05d|%05d|%08d|%08d|%03d|%03d|%012lld|%012lld", usConsNoTemp, usGuestNoTemp, ulCashierIDTemp, ulStoreregNoTemp, ItemTender.uchMajorClass, ItemTender.uchMinorClass,
				ItemTender.lRound, ItemTender.lTenderAmount);
#else
			sprintf (transBuff, "%05d|%05d|%08d|%08d|%03d|%03d|%09d|%09d", usConsNoTemp, usGuestNoTemp, ulCashierIDTemp, ulStoreregNoTemp, ItemTender.uchMajorClass, ItemTender.uchMinorClass,
				ItemTender.lRound, ItemTender.lTenderAmount);
#endif
			PifTransactionLog (0, transBuff, __FILE__, __LINE__);
		}

		return sRetStatus;
	}
}

/*==========================================================================
**   Synopsis:  SHORT   ItemTendCommon( UIFREGTENDER *UifRegTender,
*                           ITEMTENDER  *ItemTender )
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender,
*   Output:     ITEMTENDER  *ItemTender
*   InOut:      none
*
*   Return:
*
*   Description:    generate tender data, check common and each tender,
*                   return tender state(fast finalize, even/over, partial, FC)
==========================================================================*/

SHORT   ItemTendCommon( UIFREGTENDER *pUifRegTender, ITEMTENDER *pItemTender )
{
    SHORT           sStatus;

    if ( ( sStatus = ItemTendCheck( pUifRegTender, pItemTender ) ) != ITM_SUCCESS ) {
        return( sStatus );
    }

    ItemTendAmount( pUifRegTender, pItemTender );
	if (pUifRegTender->lTenderAmount > 0 && pItemTender->lRound < 0) {
		// in the case of a split tender return with part of a return going to
		// some tender, make sure that the amount entered is less than or equal to
		// the amount of the return transaction.
		if (pUifRegTender->lTenderAmount > RflLLabs(pItemTender->lRound))
			return(LDT_PROHBT_ADR);
	}

    /* --- Fast Finalize Improvement,   V3.3 --- */
    if (TranCurQualPtr->auchTotalStatus[0] == 0) {      // if not total key pressed then
		if (ItemTenderCheckTenderMdc(pUifRegTender->uchMinorClass, MDC_TENDER_PAGE_4, MDC_PARAM_BIT_B)) {  // check if fast finalize is allowed this tender
			if (pUifRegTender->uchMinorClass == CLASS_UITENDER1) {
				if (TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)) {
					if (pItemTender->lRound < 0L) {
						return(LDT_PROHBT_ADR);
					}
				} else {
					if (pItemTender->lRound >= 0L) {
						return(LDT_PROHBT_ADR);
					}
				}
			} else {
				NHPOS_ASSERT_TEXT(0, "==PROVISION: LDT_PROHBT_ADR Fast finalize prohibit by MDC this tender.");
				return(LDT_PROHBT_ADR);
			}
		}
    }

	// Next check the various HALO settings to see if any apply to this tender or not
	{
 		DCURRENCY       lTenderAmount = pUifRegTender->lTenderAmount;
		PARATRANSHALO   WorkHALO = {0};

		if (lTenderAmount == 0) {
			lTenderAmount = pItemTender->lTenderAmount;
		}

		WorkHALO.uchMajorClass = CLASS_PARATRANSHALO;
		WorkHALO.uchAddress = 0;

		if ( pUifRegTender->uchMajorClass != CLASS_UIFREGPAMTTENDER) { /* 2172 */
			if (pUifRegTender->uchMinorClass >= CLASS_UIFOREIGN1 && pUifRegTender->uchMinorClass <= CLASS_UIFOREIGN2) {
				WorkHALO.uchAddress = (UCHAR)(HALO_FOREIGN1_ADR + pUifRegTender->uchMinorClass - CLASS_UIFOREIGN1);
			} else if (pUifRegTender->uchMinorClass >= CLASS_UIFOREIGN3 && pUifRegTender->uchMinorClass <= CLASS_UIFOREIGN8) {
				WorkHALO.uchAddress = (UCHAR)(HALO_FOREIGN3_ADR + pUifRegTender->uchMinorClass - CLASS_UIFOREIGN3);
			} else {
				if (pUifRegTender->uchMinorClass >= CLASS_UITENDER1 && pUifRegTender->uchMinorClass <= CLASS_UITENDER8) {
					WorkHALO.uchAddress = ( UCHAR )( HALO_TEND1_ADR + pUifRegTender->uchMinorClass - CLASS_UITENDER1 );
				} else if (pUifRegTender->uchMinorClass >= CLASS_UITENDER9 && pUifRegTender->uchMinorClass <= CLASS_UITENDER11) {
					WorkHALO.uchAddress = ( UCHAR )( HALO_TEND9_ADR + pUifRegTender->uchMinorClass - CLASS_UITENDER9 );
				} else if (pUifRegTender->uchMinorClass >= CLASS_UITENDER12 && pUifRegTender->uchMinorClass <= CLASS_UITENDER20) {
					WorkHALO.uchAddress = ( UCHAR )( HALO_TEND12_ADR + pUifRegTender->uchMinorClass - CLASS_UITENDER12 );
				}
			}
			CliParaRead( &WorkHALO );                                       /* get tender key HALO */
			if ( RflHALO( lTenderAmount, WorkHALO.uchHALO ) != RFL_SUCCESS ) {
				return( LDT_KEYOVER_ADR );                                  /* Key Entered Over Limitation */
			}
		}

		if ( pUifRegTender->fbModifier & VOID_MODIFIER ) {               /* void tender */
			WorkHALO.uchAddress = HALO_VOID_ADR;                        /* Void HALO */
			CliParaRead( &WorkHALO );                                   /* get HALO */

			if ( RflHALO( lTenderAmount, WorkHALO.uchHALO ) != RFL_SUCCESS ) {
				return( LDT_KEYOVER_ADR );                              /* Key Entered Over Limitation */
			}
		}

		if (ItemTenderCheckTenderMdc(pUifRegTender->uchMinorClass, MDC_TENDER_PAGE_6, MDC_PARAM_BIT_C)) {  // check if optional tender HALO is set for this tender
			WorkHALO.uchAddress = HALO_OPT_TENDER_ADR;                      /* Optional Tender HALO */
			CliParaRead( &WorkHALO );                                       /* get HALO */
			if ( RflHALO( lTenderAmount, WorkHALO.uchHALO ) != RFL_SUCCESS ) {
				if (ItemSPVInt(LDT_SUPINTR_ADR) != ITM_SUCCESS) {  // Allow for Supervisor Intervention if tender amount is over optional tender HALO
					return( LDT_KEYOVER_ADR );       /* Key Entered Over Limitation */
				}
			}
		}
	}

	//SR 750, when we are doing check no purchase, the EJ header is not included 
	//with the information so when it is sent to the EJ to print, it causes errors that
	//eventually would lead to a pifabort JHHJ
	if(uchIsNoCheckOn)
	{
		ItemPrintStart(TYPE_THROUGH);
	}

	{
		SHORT         sRetStatus;

		/* check each tender(w/o FC) condition */
		switch( ItemTendState( pUifRegTender, pItemTender ) ) {
		case 0:                                             /* w/o amount tender */
			if ( ( sStatus = ItemTendFastCheck( pUifRegTender, pItemTender ) ) != ITM_SUCCESS ) {
				return( sStatus );                                  /* error */
			}
			sRetStatus = ItemTendSPVInt( pItemTender );   /* check to see if Supervisor Intervention required by MDC 19 */
			break;

		case 1:                                             /* partial tender */
			if ( ( sStatus = ItemTendPartCheck( pUifRegTender ) ) != ITM_SUCCESS ) {
				return( sStatus );                                  /* error */
			}
			if ( ( sStatus = ItemTendPartTender( pUifRegTender, pItemTender ) ) != ITM_SUCCESS ) {
				return( sStatus );                                  /* error */
			}
			sRetStatus = ItemTendPartSPVInt (pUifRegTender);   /* check to see if Supervisor Intervention required by MDC 19 */
			break;

		case 2:                                             /* even/over tender */
		case 3:
			sRetStatus = ItemTendOverCheck (pUifRegTender, pItemTender);
			break;

		default:
			sRetStatus = ITM_SUCCESS;
			break;
		}
		
		return sRetStatus;
	}
}

/*==========================================================================
**   Synopsis:  SHORT   ItemTendCheck( UIFREGTENDER *UifRegTender,
*                           ITEMTENDER  *ItemTender )
*
*   Input:      UIFREGTENDER    *UifRegTender,  ITEMTENDER  *ItemTender
*   Output:     ITEMTENDER  *ItemTender
*   InOut:      none
*
*   Return:
*
*   Description:    check various tender data
==========================================================================*/

SHORT   ItemTendCheck( UIFREGTENDER *pUifRegTender, ITEMTENDER *pItemTender )
{
    SHORT           sStatus;

    if ( ItemModLocalPtr->fsTaxable ) {                             /* depress taxable modifier */
		/* sequence error if tax modifier key has been used */
        return( LDT_SEQERR_ADR );                                   /* sequence error (us/Canada)  */
    }
    if (ItemModLocalPtr->fbModifierStatus & MOD_FSMOD) {            /* Saratoga */
        return(LDT_SEQERR_ADR);
    }

    /* --- FS Total State,  Saratoga --- */
/*    if (ItemTendCheckFS(UifRegTender) != ITM_SUCCESS &&
        ItemTenderLocal.ItemFSTotal.uchMajorClass != 0 &&
        ! (ItemTenderLocal.fbTenderStatus[1] & TENDER_FS_PARTIAL)) {
        return(LDT_SEQERR_ADR);
    }*/
	/* 07/25/01 */
    if (ItemTendCheckFS(pUifRegTender) != ITM_SUCCESS) {
    	if (ItemTenderLocal.fbTenderStatus[1] & TENDER_FS_TOTAL) {;
        	return(LDT_SEQERR_ADR);
        }
    }

    /* --- Check New Key Sequence,  V3.3 FVT --- */
    if ((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_NEWCHECK && !CliParaMDCCheck(MDC_NEWKEYSEQ_ADR, ODD_MDC_BIT1)) {
        return(LDT_SEQERR_ADR);
    }

    /*---- guest check close restriction, V3.3 ----*/
    if ( ( sStatus = ItemCommonCheckCloseCheck() ) != ITM_SUCCESS ) {
        return( sStatus );
    }
    if ( uchIsNoCheckOn == 0 && pUifRegTender->uchMajorClass != CLASS_UIFREGPAMTTENDER)  {   //Checks if the No Purchase Check Option is on or not, if it is on, skip this SR18 8-18-03 JHHJ
#if 1
		/* --- Fast Finalize Improvement,   V3.3 --- */
		if ((pUifRegTender->fbModifier & RETURNS_TRANINDIC) == 0 && pUifRegTender->lTenderAmount != 0L && TranCurQualPtr->auchTotalStatus[0] == 0) {
			if (ItemTenderCheckTenderMdc(pUifRegTender->uchMinorClass, MDC_TENDER_PAGE_4, MDC_PARAM_BIT_B)) { // check fast finalize allowed
				// fast finalize is NOT allowed this tender so issue an error message.
				// total key press is required first.
				return( LDT_SEQERR_ADR );                               /* sequence error*/
			}
		}
#else
		if ( (pUifRegTender->fbModifier & RETURNS_TRANINDIC) == 0 && pUifRegTender->lTenderAmount != 0L && TranCurQualPtr->auchTotalStatus[0] == 0 ) {
            return( LDT_SEQERR_ADR );                               /* sequence error*/
        }
#endif
    }

    switch (pUifRegTender->uchMinorClass) {                          /* Saratoga */
    case CLASS_UIFOREIGN1:
        if ((ItemTenderLocal.fbTenderStatus[2] & TENDER_FC1) == 0) {/* not FC1 total state */
            return(LDT_SEQERR_ADR);
        }
        break;
    case CLASS_UIFOREIGN2:
        if ((ItemTenderLocal.fbTenderStatus[2] & TENDER_FC2) == 0) {
            return(LDT_SEQERR_ADR);
        }
        break;
    case CLASS_UIFOREIGN3:
        if ((ItemTenderLocal.fbTenderStatus[2] & TENDER_FC3) == 0) {
            return(LDT_SEQERR_ADR);
        }
        break;
    case CLASS_UIFOREIGN4:
        if ((ItemTenderLocal.fbTenderStatus[2] & TENDER_FC4) == 0) {
            return(LDT_SEQERR_ADR);
        }
        break;
    case CLASS_UIFOREIGN5:
        if ((ItemTenderLocal.fbTenderStatus[2] & TENDER_FC5) == 0) {
            return(LDT_SEQERR_ADR);
        }
        break;
    case CLASS_UIFOREIGN6:
        if ((ItemTenderLocal.fbTenderStatus[2] & TENDER_FC6) == 0) {
            return(LDT_SEQERR_ADR);
        }
        break;
    case CLASS_UIFOREIGN7:
        if ((ItemTenderLocal.fbTenderStatus[2] & TENDER_FC7) == 0) {
            return(LDT_SEQERR_ADR);
        }
        break;
    case CLASS_UIFOREIGN8:
        if ((ItemTenderLocal.fbTenderStatus[2] & TENDER_FC8) == 0) {
            return(LDT_SEQERR_ADR);
        }
        break;
    }

    /* Netherlands rounding,    Saratoga */
    if (CliParaMDCCheck(MDC_ROUND1_ADR, ODD_MDC_BIT3) &&
        pUifRegTender->uchMajorClass != CLASS_UIFREGPAMTTENDER &&
        pUifRegTender->uchMinorClass == CLASS_UITENDER1)
	{
		PARAROUNDTBL    WorkRound;      /* R2.0 */

        WorkRound.uchMajorClass = CLASS_PARAROUNDTBL;
        WorkRound.uchAddress    = RND_TOTAL1_ADR;
        CliParaRead(&WorkRound);

        if (WorkRound.uchRoundModules != 0) {
            if (pUifRegTender->lTenderAmount % WorkRound.uchRoundModules) {
                return(LDT_PROHBT_ADR);
            }
        }
    }

    if (TranCurQualPtr->auchTotalStatus[0] == 0) {        /* no total and fast finalize */
		// if a total key has been pressed then the last item has already been sent to the transaction total logic
		// however if it has not and we are doing fast finalize then we need to do this action.
		// otherwise the total will not be correct when it is calculated before doing tender.
		ITEMTENDER      DummyTender = {0};
		SHORT           sTrnSplCheckSplit = TrnSplCheckSplit();

        if (sTrnSplCheckSplit == TRN_SPL_SEAT
            || sTrnSplCheckSplit == TRN_SPL_MULTI
            || sTrnSplCheckSplit == TRN_SPL_TYPE2)
		{
            ItemPreviousItemSpl(&DummyTender, 0);               /* Saratoga */
        } else {
            ItemPreviousItem(&DummyTender, 0 );                 /* Saratoga */
        }
	}

    ItemTendWICExempt();                                        /* Saratoga */

    if (ItemTenderLocal.fbTenderStatus[1] & TENDER_SPLIT_TENDER) {
        pItemTender->lRound = ItemTenderLocal.lSplitMI - ItemTenderLocal.lTenderizer;
    } else {
		SHORT  sSplitStatus = TrnSplCheckSplit();

        /*----- Split Check,   R3.1 -----*/
        if (sSplitStatus & (TRN_SPL_SEAT | TRN_SPL_MULTI | TRN_SPL_TYPE2)) {
            ItemTendCalAmountSpl(&pItemTender->lRound, pUifRegTender);
        } else if (sSplitStatus == TRN_SPL_SPLIT) {
            if ((sStatus = ItemTendCheckSplitTender()) != ITM_SUCCESS) {
                return(sStatus);
            }
            ItemTendCalAmountSplit(&pItemTender->lRound, pUifRegTender);
        } else {
            if (ItemTenderLocal.fbTenderStatus[1] & TENDER_FS_PARTIAL) {
                ItemTendCalFSAmount(&pItemTender->lRound);
            } else {
                if (ItemTendCalAmount(&pItemTender->lRound, pUifRegTender) != ITM_SUCCESS) {
                   return( LDT_SEQERR_ADR );
                }
            }
        }
    }

    /* make preset amount cash tender data, Saratoga */
    if (pUifRegTender->uchMajorClass == CLASS_UIFREGPAMTTENDER) {
        if ((sStatus = ItemTendPreTenderEdit( pUifRegTender )) != ITM_SUCCESS) {
            return(sStatus);
        }
    }

    pItemTender->lRound += ItemTransLocalPtr->lWorkMI;              /* whole tender amount */

    /* check void tender amount */
    if (pUifRegTender->uchMinorClass < CLASS_UIFOREIGN1 || pUifRegTender->uchMinorClass > CLASS_UIFOREIGN8) {
		// not a foreign currency tender, using normal media tender
        if ( TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN) ) {                /* preselect void */
            if ( pUifRegTender->fbModifier & VOID_MODIFIER && ( -1L * pUifRegTender->lTenderAmount ) < ItemTenderLocal.lTenderizer ) {
                return( LDT_KEYOVER_ADR );
            }
//            if ( pItemTender->lRound <= 0L && pUifRegTender->lTenderAmount != 0L ) {
//                return( LDT_SEQERR_ADR );
//            }
        } else {                                                    /* normal tender */
            if ( (pUifRegTender->fbModifier & VOID_MODIFIER) && pUifRegTender->lTenderAmount > ItemTenderLocal.lTenderizer ) {
                return( LDT_KEYOVER_ADR );
            }
			if ( uchIsNoCheckOn == 0)
			{
				if ( pItemTender->lRound <= 0L && pUifRegTender->lTenderAmount != 0L ) { //Checks if the No Purchase Check Option is on or not,
                  return( LDT_SEQERR_ADR );                              // if it is on, skip this SR18 8-18-03 JHHJ
                }
			}
        }
    } else {
		// foreign currency tender so convert to the native currency
        if ( pUifRegTender->fbModifier & VOID_MODIFIER ) {           /* void tender */
			PARACURRENCYTBL WorkRate;
			PARAMDC         WorkMDC2;
			UCHAR           uchRound;

            WorkRate.uchMajorClass = CLASS_PARACURRENCYTBL;
            WorkRate.uchAddress = (UCHAR)(CNV_NO1_ADR + pUifRegTender->uchMinorClass - CLASS_UIFOREIGN1);  /* Rate 1,  Saratoga */
            uchRound = (UCHAR)(RND_FOREIGN1_1_ADR + pUifRegTender->uchMinorClass - CLASS_UIFOREIGN1);  /* round type */
            CliParaRead( &WorkRate );

            if (CliParaMDCCheck(MDC_EURO_ADR, ODD_MDC_BIT0)) {
                /* Euro enhancement, V3.4 */
                WorkMDC2.uchMajorClass = CLASS_PARAMDC;
				/* 03/02/01, correct Euro MDC handling */
			    if ( pUifRegTender->uchMinorClass == CLASS_UIFOREIGNTOTAL1 ) {        /* FC tender */
                	WorkMDC2.usAddress = MDC_EUROFC_ADR;
			    }
			    if ( pUifRegTender->uchMinorClass == CLASS_UIFOREIGNTOTAL2 ) {        /* FC tender */
                	WorkMDC2.usAddress = MDC_EUROFC_ADR;
			    }
			    if ( pUifRegTender->uchMinorClass == CLASS_UIFOREIGNTOTAL3 ) {        /* FC tender */
                	WorkMDC2.usAddress = MDC_EUROFC2_ADR;
			    }
			    if ( pUifRegTender->uchMinorClass == CLASS_UIFOREIGNTOTAL4 ) {        /* FC tender */
                	WorkMDC2.usAddress = MDC_EUROFC2_ADR;
			    }
			    if ( pUifRegTender->uchMinorClass == CLASS_UIFOREIGNTOTAL5 ) {        /* FC tender */
                	WorkMDC2.usAddress = MDC_EUROFC3_ADR;
			    }
			    if ( pUifRegTender->uchMinorClass == CLASS_UIFOREIGNTOTAL6 ) {        /* FC tender */
                	WorkMDC2.usAddress = MDC_EUROFC3_ADR;
			    }
			    if ( pUifRegTender->uchMinorClass == CLASS_UIFOREIGNTOTAL7 ) {        /* FC tender */
                	WorkMDC2.usAddress = MDC_EUROFC4_ADR;
			    }
			    if ( pUifRegTender->uchMinorClass == CLASS_UIFOREIGNTOTAL8 ) {        /* FC tender */
                	WorkMDC2.usAddress = MDC_EUROFC4_ADR;
			    }
                /* WorkMDC2.usAddress = MDC_EUROFC_ADR; */
                CliParaRead( &WorkMDC2 );                           /* get target MDC */

				/* 03/02/01, correct Euro MDC handling */
	            switch (pUifRegTender->uchMinorClass) {
    	        case CLASS_UIFOREIGN1:
        	        WorkMDC2.uchMDCData >>= 4;
            	    break;
	            case CLASS_UIFOREIGN2:
    	            WorkMDC2.uchMDCData >>= 6;
        	        break;
            	case CLASS_UIFOREIGN3:
	                ;
    	            break;
        	    case CLASS_UIFOREIGN4:
            	    WorkMDC2.uchMDCData >>= 2;
                	break;
	            case CLASS_UIFOREIGN5:
    	            WorkMDC2.uchMDCData >>= 4;
        	        break;
            	case CLASS_UIFOREIGN6:
                	WorkMDC2.uchMDCData >>= 6;
	                break;
    	        case CLASS_UIFOREIGN7:
        	        ;
            	    break;
	            case CLASS_UIFOREIGN8:
    	            WorkMDC2.uchMDCData >>= 2;
        	        break;
            	}
				/*** deleted, 03/02/01
                if (UifRegTender->uchMinorClass == CLASS_UIFOREIGN1 / Saratoga /
                    || UifRegTender->uchMinorClass == CLASS_UIFOREIGN3
                    || UifRegTender->uchMinorClass == CLASS_UIFOREIGN5
                    || UifRegTender->uchMinorClass == CLASS_UIFOREIGN7) {
                    WorkMDC2.uchMDCData >>= 4;
                }
                if (UifRegTender->uchMinorClass == CLASS_UIFOREIGN2 / Saratoga /
                    || UifRegTender->uchMinorClass == CLASS_UIFOREIGN4
                    || UifRegTender->uchMinorClass == CLASS_UIFOREIGN6
                    || UifRegTender->uchMinorClass == CLASS_UIFOREIGN8) {
                    WorkMDC2.uchMDCData >>= 6;
                }
                ***/
                if ((WorkMDC2.uchMDCData & ODD_MDC_BIT1) &&         /* Saratoga */
                    (pUifRegTender->uchMinorClass != CLASS_UIFOREIGN1 ||
                     pUifRegTender->uchMinorClass != CLASS_UIFOREIGN3 ||
                     pUifRegTender->uchMinorClass != CLASS_UIFOREIGN5 ||
                     pUifRegTender->uchMinorClass != CLASS_UIFOREIGN7)) {

                    /* convert FC to FC through Euro */
                    ItemTendFCRateCalcByEuroB(&pItemTender->lTenderAmount, pUifRegTender->lTenderAmount, ( LONG )WorkRate.ulForeignCurrency, uchRound, FC_EURO_TYPE2, WorkMDC2.uchMDCData);
                } else {
                    ItemTendFCRateCalc2(WorkMDC2.uchMDCData, &pItemTender->lTenderAmount, pUifRegTender->lTenderAmount, ( LONG )WorkRate.ulForeignCurrency, uchRound );
                }
            } else {
                RflRateCalc2( &pItemTender->lTenderAmount, pUifRegTender->lTenderAmount, ( LONG )WorkRate.ulForeignCurrency, uchRound );
            }

            if (CliParaMDCCheck(MDC_ROUND2_ADR, EVEN_MDC_BIT0)) {   /* Saratoga */
                RflRound(&pItemTender->lTenderAmount, pItemTender->lTenderAmount, RND_TOTAL1_ADR);
            }

            if ( TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN) ) {            /* preselect void */
                if ( pItemTender->lTenderAmount > ItemTenderLocal.lTenderizer * -1L ) {
                    return( LDT_KEYOVER_ADR );
                }
            } else {                                                /* normal tender */
                if ( pItemTender->lTenderAmount > ItemTenderLocal.lTenderizer ) {
                    return( LDT_KEYOVER_ADR );
                }
            }
        } else {
            if ( TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN) ) {            /* preselect void */
                if ( pItemTender->lRound <= 0L && pUifRegTender->lTenderAmount != 0L ) {
                    return( LDT_SEQERR_ADR );
                }
            } else {                                                /* normal tender */
                if ( pItemTender->lRound <= 0L && pUifRegTender->lTenderAmount != 0L ) {
                      return( LDT_SEQERR_ADR );
                }
            }
        }
    }

    return( ITM_SUCCESS );
}

/*==========================================================================
**   Synopsis:  VOID    ItemTendAmount( UIFREGTENDER *UifRegTender,
*                           ITEMTENDER  *ItemTender )
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender
*   Output:     ITEMTENDER  *ItemTender
*   InOut:      none
*
*   Return:
*
*   Description:    generate various tender data
==========================================================================*/

VOID    ItemTendAmount( UIFREGTENDER *pUifRegTender, ITEMTENDER *pItemTender )
{
    if ( pUifRegTender->lTenderAmount == 0L ) {                          /* w/o amount tender */
        ItemTendAmountWO( pUifRegTender, pItemTender );
    } else {                                                            /*  partial, even/over tender */
        ItemTendAmountWA( pUifRegTender, pItemTender );
    }
}

/*==========================================================================
**   Synopsis:  VOID    ItemTendAmountWO( UIFREGTENDER *UifRegTender,
*                           ITEMTENDER  *ItemTender )
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender,
*   Output:     ITEMTENDER  *ItemTender
*   InOut:      none
*
*   Return:
*
*   Description:    generate w/o amount tender data
==========================================================================*/

VOID    ItemTendAmountWO( UIFREGTENDER *pUifRegTender, ITEMTENDER *pItemTender )
{
    UCHAR           uchRound;
	PARACURRENCYTBL WorkRate = {0};
    PARAMDC         WorkMDC2;

    if (pUifRegTender->uchMinorClass >= CLASS_UIFOREIGNTOTAL1 && pUifRegTender->uchMinorClass <= CLASS_UIFOREIGN8) {    /* Saratoga */
        WorkRate.uchMajorClass = CLASS_PARACURRENCYTBL;
        switch (pUifRegTender->uchMinorClass) {
        case CLASS_UIFOREIGNTOTAL1:
        case CLASS_UIFOREIGN1:
            WorkRate.uchAddress = CNV_NO1_ADR;
            uchRound = RND_FOREIGN1_1_ADR;
            WorkMDC2.usAddress = MDC_EUROFC_ADR;
            break;
        case CLASS_UIFOREIGNTOTAL2:
        case CLASS_UIFOREIGN2:
            WorkRate.uchAddress = CNV_NO2_ADR;
            uchRound = RND_FOREIGN2_1_ADR;
            WorkMDC2.usAddress = MDC_EUROFC_ADR;
            break;
        case CLASS_UIFOREIGNTOTAL3:
        case CLASS_UIFOREIGN3:
            WorkRate.uchAddress = CNV_NO3_ADR;
            uchRound = RND_FOREIGN3_1_ADR;
            WorkMDC2.usAddress = MDC_EUROFC2_ADR;
            break;
        case CLASS_UIFOREIGNTOTAL4:
        case CLASS_UIFOREIGN4:
            WorkRate.uchAddress = CNV_NO4_ADR;
            uchRound = RND_FOREIGN4_1_ADR;
            WorkMDC2.usAddress = MDC_EUROFC2_ADR;
            break;
        case CLASS_UIFOREIGNTOTAL5:
        case CLASS_UIFOREIGN5:
            WorkRate.uchAddress = CNV_NO5_ADR;
            uchRound = RND_FOREIGN5_1_ADR;
            WorkMDC2.usAddress = MDC_EUROFC3_ADR;
            break;
        case CLASS_UIFOREIGNTOTAL6:
        case CLASS_UIFOREIGN6:
            WorkRate.uchAddress = CNV_NO6_ADR;
            uchRound = RND_FOREIGN6_1_ADR;
            WorkMDC2.usAddress = MDC_EUROFC3_ADR;
            break;
        case CLASS_UIFOREIGNTOTAL7:
        case CLASS_UIFOREIGN7:
            WorkRate.uchAddress = CNV_NO7_ADR;
            uchRound = RND_FOREIGN7_1_ADR;
            WorkMDC2.usAddress = MDC_EUROFC4_ADR;
            break;
        case CLASS_UIFOREIGNTOTAL8:
        case CLASS_UIFOREIGN8:
            WorkRate.uchAddress = CNV_NO8_ADR;
            uchRound = RND_FOREIGN8_1_ADR;
            WorkMDC2.usAddress = MDC_EUROFC4_ADR;
            break;
        }
        CliParaRead( &WorkRate );

        /* --- F/C Rounding --- */
        if (CliParaMDCCheck(MDC_ROUND2_ADR, EVEN_MDC_BIT0)) {   /* Saratoga */
            RflRound(&pItemTender->lRound, pItemTender->lRound, RND_TOTAL1_ADR);
        }

        if (CliParaMDCCheck(MDC_EURO_ADR, ODD_MDC_BIT0)) {
            /* Euro enhancement, V3.4 */
            WorkMDC2.uchMajorClass = CLASS_PARAMDC;
            CliParaRead( &WorkMDC2 );                                        /* get target MDC */

			/* 03/02/01, correct Euro MDC handling */
            switch (pUifRegTender->uchMinorClass) {
            case CLASS_UIFOREIGN1:
                WorkMDC2.uchMDCData >>= 4;
                break;
            case CLASS_UIFOREIGN2:
                WorkMDC2.uchMDCData >>= 6;
                break;
            case CLASS_UIFOREIGN3:
                ;
                break;
            case CLASS_UIFOREIGN4:
                WorkMDC2.uchMDCData >>= 2;
                break;
            case CLASS_UIFOREIGN5:
                WorkMDC2.uchMDCData >>= 4;
                break;
            case CLASS_UIFOREIGN6:
                WorkMDC2.uchMDCData >>= 6;
                break;
            case CLASS_UIFOREIGN7:
                ;
                break;
            case CLASS_UIFOREIGN8:
                WorkMDC2.uchMDCData >>= 2;
                break;
            }

            if ((WorkMDC2.uchMDCData & ODD_MDC_BIT1) &&
                (pUifRegTender->uchMinorClass != CLASS_UIFOREIGN1)) {

                /* convert FC to FC through Euro */
                ItemTendFCRateCalcByEuroB(&pItemTender->lForeignAmount, pItemTender->lRound,
                    (LONG)WorkRate.ulForeignCurrency, uchRound, FC_EURO_TYPE1, WorkMDC2.uchMDCData);

            } else {
                ItemTendFCRateCalc1(WorkMDC2.uchMDCData, &pItemTender->lForeignAmount,
                    pItemTender->lRound, WorkRate.ulForeignCurrency, uchRound);
            }

        } else {
            RflRateCalc1(&pItemTender->lForeignAmount, pItemTender->lRound,
                    (LONG)WorkRate.ulForeignCurrency, uchRound);
        }
    }

    pItemTender->ulFCRate = WorkRate.ulForeignCurrency;              /* foreign currency rate */
    pItemTender->lTenderAmount = pItemTender->lRound;                 /* w/o amount tender */

    /* Netherlands Rounding,    Saratoga */
    if (CliParaMDCCheck(MDC_ROUND1_ADR, ODD_MDC_BIT3) &&
        pUifRegTender->uchMinorClass == CLASS_UITENDER1) {
        RflRound(&pItemTender->lRound, pItemTender->lRound, RND_TOTAL1_ADR);
        pItemTender->lTenderAmount = pItemTender->lRound;
    }
}

/*==========================================================================
**   Synopsis:  VOID    ItemTendAmountWA( UIFREGTENDER *UifRegTender,
*                           ITEMTENDER  *ItemTender )
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender,
*   Output:     ITEMTENDER  *ItemTender
*   InOut:      none
*
*   Return:
*
*   Description:    generate w/amount tender data
==========================================================================*/

VOID    ItemTendAmountWA( UIFREGTENDER *pUifRegTender, ITEMTENDER *pItemTender )
{
    if ((TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)) && pItemTender->lRound < 0) {    /* preselect void or a transaction return */
        pItemTender->lRound *= -1;
        ItemTendAmountWANor( pUifRegTender, pItemTender );

        pItemTender->lRound *= -1;                                       /* for p-void */
        pItemTender->lTenderAmount *= -1;                                /* tender amount */
        pItemTender->lForeignAmount *= -1;                               /* foreign amount */
        pItemTender->lBalanceDue *= -1;                                  /* balance due */
        pItemTender->lChange *= -1;                                      /* change */
    } else {                                                            /* normal tender */
        ItemTendAmountWANor( pUifRegTender, pItemTender );
    }
}

/*==========================================================================
**   Synopsis:  VOID    ItemTendAmountWANor( UIFREGTENDER *UifRegTender,
*                           ITEMTENDER  *ItemTender )
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender,
*   Output:     ITEMTENDER  *ItemTender
*   InOut:      none
*
*   Return:
*
*   Description:    generate w/amount tender data (normal tender)
==========================================================================*/

VOID    ItemTendAmountWANor( UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender )
{
    if (UifRegTender->uchMinorClass < CLASS_UIFOREIGN1 || UifRegTender->uchMinorClass > CLASS_UIFOREIGN8) {
		// this is not a foreign tender so we just handle normally
        if ( UifRegTender->fbModifier & VOID_MODIFIER ) {               /* partial tender amount */
            ItemTender->lTenderAmount = ( -1 ) * UifRegTender->lTenderAmount;
            ItemTender->lBalanceDue = ItemTender->lRound + UifRegTender->lTenderAmount;
        } else {
            ItemTender->lTenderAmount = UifRegTender->lTenderAmount;

            if ( ItemTender->lRound > UifRegTender->lTenderAmount ) {   /* partial tender */
                ItemTender->lBalanceDue = ItemTender->lRound - UifRegTender->lTenderAmount;

            } else {                                                       /* even/over tender */
                ItemTender->lChange = UifRegTender->lTenderAmount - ItemTender->lRound;
            }
        }
    } else {
		/* Foreign tender so we have to convert to the native tender currency */
		PARACURRENCYTBL  WorkRate;
		DCURRENCY        lNative;
		SHORT            sPvoid = 1;
		UCHAR            uchRound;

        WorkRate.uchMajorClass = CLASS_PARACURRENCYTBL;
        WorkRate.uchAddress = (UCHAR)(CNV_NO1_ADR + UifRegTender->uchMinorClass - CLASS_UIFOREIGN1);
        uchRound = (UCHAR)(RND_FOREIGN1_1_ADR + UifRegTender->uchMinorClass - CLASS_UIFOREIGN1);
        CliParaRead( &WorkRate );

        if (ItemTenderLocal.fbTenderStatus[1] & TENDER_SPLIT_TENDER) {
            lNative = ItemTenderLocal.lSplitMI - ItemTenderLocal.lTenderizer;
        } else {
			SHORT  sSplitStatus = TrnSplCheckSplit();

            /*----- Split Check,   R3.1 -----*/
            if (sSplitStatus == TRN_SPL_SEAT
                || sSplitStatus == TRN_SPL_MULTI
                || sSplitStatus == TRN_SPL_TYPE2) {
                ItemTendCalAmountSpl(&lNative, UifRegTender);
            } else if (sSplitStatus == TRN_SPL_SPLIT) {
                ItemTendCalAmountSplit(&lNative, UifRegTender);
            } else {
                if (ItemTenderLocal.fbTenderStatus[1] & TENDER_FS_PARTIAL) {
                    ItemTendCalFSAmount(&lNative);
                } else {
                    ItemTendCalAmount(&lNative, UifRegTender);
                }
            }
        }

        lNative += ItemTransLocalPtr->lWorkMI;                                   /* whole tender amount */

        /* --- Netherland rounding, 2172 --- */
        if ( CliParaMDCCheck( MDC_ROUND1_ADR, ODD_MDC_BIT3 ) ) {
			DCURRENCY   lAmount;

            if ( RflRound( &lAmount, lNative, RND_TOTAL1_ADR ) == RFL_SUCCESS ) {
                lNative = lAmount;
            }
        }

        if ( TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN) ) {   /* preselect void */
            sPvoid = -1;
        }

		if ((TranCurQualPtr->fsCurStatus & (CURQUAL_PRETURN | CURQUAL_TRETURN)) != 0 && ItemTender->lRound > 0) {
			// if this is a return yet the amount of the transaction is positive then
			// this is an exchange with additional items and the additional items are more
			// than the items being swapped out.
			sPvoid = 1;
		}

        if ( ItemTenderLocal.lFCMI * sPvoid == UifRegTender->lTenderAmount ) {   /* even tender */
            ItemTender->lTenderAmount = lNative * sPvoid;               /* native amount 93/07/20 */
        } else {                                                        /* not even tender */
            if (CliParaMDCCheck(MDC_EURO_ADR, ODD_MDC_BIT0)) {
				PARAMDC         WorkMDC2 = {0};

                /* Euro enhancement, V3.4 */
                WorkMDC2.uchMajorClass = CLASS_PARAMDC;

				/* 03/02/01, correct Euro MDC handling */
                switch (UifRegTender->uchMinorClass) {
                case CLASS_UIFOREIGN1:
                    WorkMDC2.usAddress = MDC_EUROFC_ADR;
                    break;
                case CLASS_UIFOREIGN2:
                    WorkMDC2.usAddress = MDC_EUROFC_ADR;
                    break;
                case CLASS_UIFOREIGN3:
                    WorkMDC2.usAddress = MDC_EUROFC2_ADR;
                    break;
                case CLASS_UIFOREIGN4:
                    WorkMDC2.usAddress = MDC_EUROFC2_ADR;
                    break;
                case CLASS_UIFOREIGN5:
                    WorkMDC2.usAddress = MDC_EUROFC3_ADR;
                    break;
                case CLASS_UIFOREIGN6:
                    WorkMDC2.usAddress = MDC_EUROFC3_ADR;
                    break;
                case CLASS_UIFOREIGN7:
                    WorkMDC2.usAddress = MDC_EUROFC4_ADR;
                    break;
                case CLASS_UIFOREIGN8:
                    WorkMDC2.usAddress = MDC_EUROFC4_ADR;
                    break;
                }
                CliParaRead( &WorkMDC2 );                                        /* get target MDC */

				/* 03/02/01, correct Euro MDC handling */
                switch (UifRegTender->uchMinorClass) {
                case CLASS_UIFOREIGN1:
                    WorkMDC2.uchMDCData >>= 4;
                    break;
                case CLASS_UIFOREIGN2:
                    WorkMDC2.uchMDCData >>= 6;
                    break;
                case CLASS_UIFOREIGN3:
                    ;
                    break;
                case CLASS_UIFOREIGN4:
                    WorkMDC2.uchMDCData >>= 2;
                    break;
                case CLASS_UIFOREIGN5:
                    WorkMDC2.uchMDCData >>= 4;
                    break;
                case CLASS_UIFOREIGN6:
                    WorkMDC2.uchMDCData >>= 6;
                    break;
                case CLASS_UIFOREIGN7:
                    ;
                    break;
                case CLASS_UIFOREIGN8:
                    WorkMDC2.uchMDCData >>= 2;
                    break;
                }

                if ((WorkMDC2.uchMDCData & ODD_MDC_BIT1) && (UifRegTender->uchMinorClass != CLASS_UIFOREIGN1)) {
                    /* convert FC to FC through Euro */
                    ItemTendFCRateCalcByEuroB(&ItemTender->lTenderAmount, UifRegTender->lTenderAmount, ( LONG )WorkRate.ulForeignCurrency, uchRound, FC_EURO_TYPE2, WorkMDC2.uchMDCData );
                } else {
                    ItemTendFCRateCalc2(WorkMDC2.uchMDCData, &ItemTender->lTenderAmount, UifRegTender->lTenderAmount, ( LONG )WorkRate.ulForeignCurrency, uchRound );
                }
            } else {
                RflRateCalc2( &ItemTender->lTenderAmount, UifRegTender->lTenderAmount, ( LONG )WorkRate.ulForeignCurrency, uchRound );
            }
            /* --- F/C Rounding,    Saratoga --- */
            if (CliParaMDCCheck(MDC_ROUND2_ADR, EVEN_MDC_BIT0)) {
                RflRound(&ItemTender->lTenderAmount, ItemTender->lTenderAmount, RND_TOTAL1_ADR);
            }
        }

        ItemTender->ulFCRate = WorkRate.ulForeignCurrency;              /* foreign currency rate */
        ItemTender->lForeignAmount = UifRegTender->lTenderAmount;       /* foreign amount */

        if ( UifRegTender->fbModifier & VOID_MODIFIER ) {               /* partial tender amount */
            ItemTender->lBalanceDue = ItemTender->lRound + ItemTender->lTenderAmount;

            ItemTender->lTenderAmount *= -1;
            ItemTender->lForeignAmount = ( -1 ) * UifRegTender->lTenderAmount;

        } else {
            if ( ItemTenderLocal.lFCMI * sPvoid > UifRegTender->lTenderAmount ) {   /* partial tender */
                if ( sPvoid * lNative > ItemTender->lTenderAmount ) {
                    ItemTender->lBalanceDue = sPvoid * lNative - ItemTender->lTenderAmount;
                } else {
                    ItemTender->lBalanceDue = 0L;
                }

            } else {                                                    /* even/over tender */
                if ( ItemTenderLocal.lFCMI * sPvoid < UifRegTender->lTenderAmount ) {
                    ItemTender->lChange = ItemTender->lTenderAmount - sPvoid * lNative;

                    /* --- F/C Rounding,    Saratoga --- */
                    if (CliParaMDCCheck(MDC_ROUND2_ADR, EVEN_MDC_BIT0)) {
                        RflRound(&ItemTender->lChange, ItemTender->lChange, RND_TOTAL1_ADR);
                    }

                } else {
                    ItemTender->lChange = 0L;
                }
            }
        }
    }

    /* --- Netherland rounding, 2172 --- */
    if (ItemTender->lBalanceDue && UifRegTender->uchMinorClass == CLASS_UITENDER1) {
        if (CliParaMDCCheck(MDC_ROUND1_ADR, ODD_MDC_BIT3)) {
			DCURRENCY   lAmount;

            if (RflRound(&lAmount, ItemTender->lBalanceDue, RND_TOTAL1_ADR) == RFL_SUCCESS) {
                ItemTender->lBalanceDue = lAmount;
            }
        }
    }

    if (ItemTender->lChange) {
        if (CliParaMDCCheck(MDC_ROUND1_ADR, ODD_MDC_BIT3)) {
			DCURRENCY   lAmount;

            if (RflRound(&lAmount, ItemTender->lChange, RND_TOTAL1_ADR) == RFL_SUCCESS) {
                ItemTender->lChange = lAmount;
            }
        }
    }
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTendState( UIFREGTENDER *UifRegTender,
*                           ITEMTENDER *ItemTender )
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    return tender state
==========================================================================*/

SHORT   ItemTendState( UIFREGTENDER *pUifRegTender, ITEMTENDER *pItemTender )
{
    if ( pUifRegTender->uchMinorClass >= CLASS_UIFOREIGNTOTAL1 && pUifRegTender->uchMinorClass <= CLASS_UIFOREIGNTOTAL8 ) { /* Saratoga */
        return( 4 );                                                /* FC total */
    }

    if ( pUifRegTender->lTenderAmount == 0L ) {                      /* w/o amount tender */
        if ( pUifRegTender->uchMinorClass >= CLASS_UIFOREIGN1 && pUifRegTender->uchMinorClass <= CLASS_UIFOREIGN8 ) {  /* Saratoga */
            return( 5 );
        } else {
            return( 0 );
        }
    }

	if( uchIsNoCheckOn == 1 &&										//If this is 1, we are in Check No Purchase Mode,
		pUifRegTender->lTenderAmount != 0L)							//so we return 0, to say that it is correct
	{																//SR 18 JHHJ 8-21-03
		return( 0 );
	}
    if ( pUifRegTender->fbModifier & VOID_MODIFIER ) {               /* void tender */
        if ( pUifRegTender->uchMinorClass >= CLASS_UIFOREIGN1 && pUifRegTender->uchMinorClass <= CLASS_UIFOREIGN8 ) {  /* foreign currency tender */
            return( 6 );                                            /* FC partial tender */
        } else {
            return( 1 );                                            /* partial tender */
        }
    }

    if ( pUifRegTender->uchMinorClass >= CLASS_UIFOREIGN1 && pUifRegTender->uchMinorClass <= CLASS_UIFOREIGN8 ) {      /* foreign currency tender */
        if ( pItemTender->lBalanceDue ) {                            /* exist balance due */
            return( 6 );                                            /* partial tender */
        } else if ( pItemTender->lChange ) {                         /* exist change */
            return( 8 );                                            /* over tender */
        } else {
            return( 7 );                                            /* even tender */
        }
    } else {                                                        /* native tender */
        if ( pItemTender->lBalanceDue ) {                            /* exist balance due */
            return( 1 );                                            /* partial tender */
        } else if ( pItemTender->lChange ) {                         /* exist change */
            return( 3 );                                            /* over tender */
        } else {
            return( 2 );                                            /* even tender */
        }
    }
}

/*==========================================================================
**  Synopsis:   VOID    ItemTendDelGCF( VOID )
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    delete current GCF,         R3.0
==========================================================================*/

VOID    ItemTendDelGCF( VOID )
{
    TRANGCFQUAL      *WorkGCF = TrnGetGCFQualPtr();     /* get GCF qualifier */
    TRANINFORMATION  *WorkTranInf = TrnGetTranInformationPointer();
    SHORT   sStatus;

    /* cashier interrupt, at no recall case, R3.3 */
    if ((WorkTranInf->TranModeQual.fsModeStatus & MODEQUAL_CASINT) &&
        ((WorkTranInf->TranCurQual.fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_CASHIER)) {
        return;
    }

    if (RflGetSystemType() != FLEX_STORE_RECALL && WorkGCF->usPreauthItemNumber == 0) {
		TrnConnEngineSendGC(WorkGCF->usGuestNo);
		if ( WorkGCF->usGuestNo ) {                                  /* GCF operation ? */
			if ( ( sStatus = CliGusDelete( WorkGCF->usGuestNo ) ) !=  GCF_SUCCESS ) {
				USHORT  usStatus = GusConvertError( sStatus );
                UieErrorMsg( usStatus, UIE_WITHOUT );
				{
					char xBuff[128];
					sprintf (xBuff, "**WARNING ItemTendDelGCF(): Error %d deleting GC %d.", usStatus, WorkGCF->usGuestNo);
					NHPOS_ASSERT_TEXT((sStatus == GCF_SUCCESS), xBuff);
				}
            }
		}
    } else {
        if (WorkGCF->usPreauthItemNumber) {
			// this is a Preauth Capture tender from a Preauth Batch so do not send it out.
		} else if (WorkGCF->usGuestNo) {                                    /* GCF operation ? */
			SHORT       sType = GCF_COUNTER_TYPE;
			FDTPARA     WorkFDT = { 0 };

			FDTParameter(&WorkFDT);
            if (WorkFDT.uchSysTypeTerm == FX_DRIVE_SYSTYPE_2T_OP_D) {     /* Delivery System */
                if (WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_TERM_1          /* Order Termninal (Queue#1) */
                    || WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_PAYMENT_TERM_1
                    || WorkFDT.uchTypeTerm == FX_DRIVE_PAYMENT_TERM_1
                    || WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_FUL_TERM_1) {
                    sType = GCF_DRIVE_DE_TENDER1;

                } else if (WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_TERM_2 /* Order Termninal (Queue#2) */
                    || WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_PAYMENT_TERM_2
                    || WorkFDT.uchTypeTerm == FX_DRIVE_PAYMENT_TERM_2
                    || WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_FUL_TERM_2) {
                    sType = GCF_DRIVE_DE_TENDER2;
                }
            } else {
                if (WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_TERM_1    /* Order Termninal (Queue#1) */
                    || WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_PAYMENT_TERM_1
                    || WorkFDT.uchTypeTerm == FX_DRIVE_PAYMENT_TERM_1
                    || WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_FUL_TERM_1) {
                    sType = GCF_DRIVE_TH_TENDER1;

                } else if (WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_TERM_2 /* Order Termninal (Queue#2) */
                    || WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_PAYMENT_TERM_2
                    || WorkFDT.uchTypeTerm == FX_DRIVE_PAYMENT_TERM_2
                    || WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_FUL_TERM_2) {
                    sType = GCF_DRIVE_TH_TENDER2;
                }
            }
            WorkGCF->fsGCFStatus2 |= GCFQUAL_PAYMENT_TRANS;          /* Payment Transaction */

            if (TranCurQualPtr->fsCurStatus & CURQUAL_TRAINING) {    /* training */
                WorkGCF->fsGCFStatus |= GCFQUAL_TRAINING;
            }

            while ((sStatus = TrnSaveGC(sType, WorkGCF->usGuestNo)) !=  TRN_SUCCESS) {
				USHORT  usStatus = GusConvertError(sStatus);
                UieErrorMsg(usStatus, UIE_WITHOUT);
            }
            if (WorkFDT.uchTypeTerm == FX_DRIVE_COUNTER_STORE_PAY
                || WorkFDT.uchTypeTerm == FX_DRIVE_COUNTER_FUL_STORE_PAY) {
				FDTSaveTransStorePay(TrnInformationPtr->TranGCFQual.usGuestNo);
            }
			FDTPayment(WorkGCF->usGuestNo);
        }
    }

	{
		ITEMTRANSLOCAL   WorkTrans;

		ItemTransGetLocal( &WorkTrans );                                /* get transaction open local data */
		for ( ; WorkTrans.uchCheckCounter != 0; WorkTrans.uchCheckCounter-- ) {
			while ( ( sStatus = CliGusDelete( WorkTrans.ausGuestNo[ WorkTrans.uchCheckCounter - 1 ] ) ) !=  GCF_SUCCESS ) {
				USHORT  usStatus = GusConvertError( sStatus );
				UieErrorMsg( usStatus, UIE_WITHOUT );
			}
		}
	}
}

/*==========================================================================
**  Synopsis:   VOID    ItemTendSaved( ITEMTENDER *ItemTender )
*
*   Input:      ITEMTENDER  *ItemTender
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    set saved MI
==========================================================================*/

VOID    ItemTendSaved( ITEMTENDER *pItemTender )
{
    ItemTenderLocal.lSavedMI = pItemTender->lTenderAmount;       /* saved MI */
}

/*==========================================================================
**  Synopsis:   VOID    ItemTendInit( VOID )
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    initialize transaction information,  R3.0     R3.1
==========================================================================*/

VOID    ItemTendInit( VOID )
{
    ULONG           flMask;
    TRANGCFQUAL     *TranGCFQualWrk = TrnGetGCFQualPtr();

	/*
	 * the function TrnStoGetConsToPostR() must be used to transfer the last transaction
	 * to the Post Receipt storage area in order for Post Receipt or Print on Demand to work
	 * by using TrnStoPutPostRToCons() to retrieve the saved data in the Post Receipt storage area.
	**/
    TrnStoGetConsToPostR();

    flMask = TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK;
    if ( ( flMask == CURQUAL_POSTCHECK ) || ( flMask == CURQUAL_STORE_RECALL ) ) {

        if (( flMask == CURQUAL_POSTCHECK )
            && !CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT3)
            && !CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2)) {

            if (TranGCFQualWrk->usGuestNo == 0 || TranModeQualPtr->fsModeStatus & MODEQUAL_CASINT) {
                /* --- Allow Post Receipt at Cashier Int,   V3.3 FVT#7 --- */
                ItemMiscSetStatus(MISC_POSTRECPT | MISC_PARKING | MISC_GIFTRECEIPT | MISC_PRINTDEMAND);
            }

        } else if ( !( TranGCFQualWrk->fsGCFStatus & GCFQUAL_BUFFER_PRINT ) ) {
            ItemMiscSetStatus( MISC_POSTRECPT | MISC_PARKING | MISC_GIFTRECEIPT | MISC_PRINTDEMAND );
        }

    } else {
        if (TranGCFQualWrk->usGuestNo == 0 || TranModeQualPtr->fsModeStatus & MODEQUAL_CASINT) {
            /* --- Allow Post Receipt at Cashier Int,   V3.3 FVT#7 --- */
            ItemMiscSetStatus( MISC_POSTRECPT | MISC_PARKING | MISC_GIFTRECEIPT | MISC_PRINTDEMAND );
        }
    }

    TrnInitialize( TRANI_GCFQUAL | TRANI_CURQUAL | TRANI_ITEMIZERS | TRANI_ITEM | TRANI_CONSOLI );
    if (flMask == CURQUAL_POSTCHECK
        && !CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT3)
        && !CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2)) {
        TrnInitializeSpl(TRANI_GCFQUAL_SPLA | TRANI_ITEMIZERS_SPLA | TRANI_CONSOLI_SPLA);
        TrnInitializeSpl(TRANI_GCFQUAL_SPLB | TRANI_ITEMIZERS_SPLB | TRANI_CONSOLI_SPLB);
    }
}

/*
*==========================================================================
**  Synopsis:   SHORT   ItemTendCalAmount(LONG *lAmount, UIFREGTENDER *UifTender)
*
*   Input:      LONG    *lAmount
*   Output:     LONG    *lAmount
*   InOut:      none
*
*   Return:
*
*   Description:    calculate current tender amount.    R3.1, V3.3, Saratoga
*==========================================================================
*/
SHORT   ItemTendCalAmount(DCURRENCY *lAmount, CONST UIFREGTENDER *UifTender)
{
    CONST TRANITEMIZERS   *WorkTI = TrnGetTIPtr();
	ITEMCOMMONTAX   WorkTax = {0};
    ITEMCOMMONVAT   WorkVAT = {0};
	UCHAR           auchTotalStatus[NUM_TOTAL_STATUS] = {0};

    *lAmount = WorkTI->lMI;                                          /* MI */

    if ( TranCurQualPtr->fsCurStatus & CURQUAL_TRAY ) {                     /* tray Trans, R3.1*/
		ItemTotalAuchTotalStatus (auchTotalStatus, ItemTotalLocalPtr->uchSaveMinor);
    } else {
		UCHAR  uchMinorClass = UifTender->uchMinorClass;
		SHORT  sRetStatus;

		if (UifTender->uchMajorClass == CLASS_UIFREGPAMTTENDER) {
			uchMinorClass = CLASS_UITENDER1;  // Preset Tender Key, FSC_PRESET_AMT, will use Tender #1 total key settings.
		}

		if ((sRetStatus = ItemTendGetTotal(&auchTotalStatus[0], uchMinorClass)) != ITM_SUCCESS) {  /* get total parameter assigned to this tender key */
			return sRetStatus;
		}
    }

    if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
        ItemCanTax( &auchTotalStatus[1], &WorkTax, ITM_NOT_SPLIT );
#pragma message("Remove this check once we determine TH ISSUE JHHJ")
		if(WorkTax.alTaxable[1] > 100000)
		{
			// Tim Horton's of Canada was having a problem with a tax calculation error
			// which would suddenly happen and then not be seen for a while.
			// This was back in Rel 2.1.0 around 2007 or so.  They are no longer a customer.
			//   Richard Chambers, Apr-09-2015
			NHPOS_ASSERT(!"ItemTendCalAmount TAX ERROR");
		}
    } else if (ItemCommonTaxSystem() == ITM_TAX_US) {
        if (ItemCurTax(&auchTotalStatus[1], &WorkTax) != ITM_SUCCESS) {
            return(LDT_SEQERR_ADR);
        }
    } else {
        if (ItemCurVAT( &auchTotalStatus[1], &WorkVAT, ITM_NOT_SPLIT) != ITM_SUCCESS) {
            return(LDT_SEQERR_ADR);
        }
    }

#if 0
	// This source code eliminated as part of changes for JIRA AMT-2853.
	// There are a number of places where there was a check on the flags for
	// a CURQUAL_STORE_RECALL type system along with processing a TENDER_PARTIAL in which the
	// total calculation depended on the system type and if we were doing a split or partial tender.
	// We started with changes in functions TrnItemTotal() and ItemTotalCTAmount() in order
	// to reduce the complexity of the source code and dependencies on flags as much as possible
	// as part of making source code changes for AMT-2853.
	//    Richard Chambers for Amtrak Rel 2.2.1 project June 25, 2013
    if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_STORE_RECALL
        && (ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL)) {
        if ((! CliParaMDCCheck(MDC_ROUND1_ADR, ODD_MDC_BIT3)) || (CliParaMDCCheck(MDC_ROUND1_ADR, ODD_MDC_BIT3) &&
            UifTender->uchMinorClass == CLASS_UITENDER1)) {
            /* if (!(ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL)) { */
                RflRound(lAmount, *lAmount, RND_TOTAL1_ADR);        /* Saratoga */
            /* } */
        }
		if (ItemTenderLocal.fbTenderStatus[1] & TENDER_FS_PARTIAL) {
			*lAmount -= ItemTenderLocal.lFSTenderizer;
		} else if (ItemTenderLocal.fbTenderStatus[1] & TENDER_FS_PARTIAL2) {
			*lAmount -= ItemTenderLocal.lFSTenderizer2;
		} else {
			*lAmount -= ItemTenderLocal.lTenderizer;
		}
    } else
#endif
	{
        if (ItemCommonTaxSystem() == ITM_TAX_INTL) {
            *lAmount += WorkVAT.lPayment;
            if ((! CliParaMDCCheck(MDC_ROUND1_ADR, ODD_MDC_BIT3)) || (CliParaMDCCheck(MDC_ROUND1_ADR, ODD_MDC_BIT3) &&
                UifTender->uchMinorClass == CLASS_UITENDER1)) {
                /* if (!(ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL)) { */
                    RflRound(lAmount, *lAmount, RND_TOTAL1_ADR);    /* Saratoga */
                /* } */
            }
            *lAmount -= ItemTenderLocalPtr->lTenderizer;
        } else {
			//SR 279 JHHJ
			//This if statement now allows the user to decide whether or not to recalculate
			//tax when two checks are added together.
			if((TranCurQualPtr->fsCurStatus & CURQUAL_MULTICHECK))
			{
				 // we are in multiple check mode.
				if(!CliParaMDCCheck(MDC_TAXINTL_ADR, ODD_MDC_BIT1) || ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK))
				{

					*lAmount += WorkTax.alTax[0] + WorkTax.alTax[1] + WorkTax.alTax[2] + WorkTax.alTax[3];
					if ((! CliParaMDCCheck(MDC_ROUND1_ADR, ODD_MDC_BIT3)) || (CliParaMDCCheck(MDC_ROUND1_ADR, ODD_MDC_BIT3) &&
						UifTender->uchMinorClass == CLASS_UITENDER1))
					{
						/* if (!(ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL)) { */
							RflRound(lAmount, *lAmount, RND_TOTAL1_ADR);    /* Saratoga */
						/* } */
					}
					if (ItemTenderLocalPtr->fbTenderStatus[1] & TENDER_FS_PARTIAL) {
						*lAmount -= ItemTenderLocalPtr->lFSTenderizer;
					} else if (ItemTenderLocalPtr->fbTenderStatus[1] & TENDER_FS_PARTIAL2) {
						*lAmount -= ItemTenderLocalPtr->lFSTenderizer2;
					} else {
						*lAmount -= ItemTenderLocalPtr->lTenderizer;
					}
				}
			}else
			{//one cent tax*/
				*lAmount += WorkTax.alTax[0] + WorkTax.alTax[1] + WorkTax.alTax[2] + WorkTax.alTax[3];
				if ((! CliParaMDCCheck(MDC_ROUND1_ADR, ODD_MDC_BIT3)) || (CliParaMDCCheck(MDC_ROUND1_ADR, ODD_MDC_BIT3) &&
					UifTender->uchMinorClass == CLASS_UITENDER1))
				{
					/* if (!(ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL)) { */
						RflRound(lAmount, *lAmount, RND_TOTAL1_ADR);    /* Saratoga */
					/* } */
				}
				if (ItemTenderLocalPtr->fbTenderStatus[1] & TENDER_FS_PARTIAL) {
					*lAmount -= ItemTenderLocalPtr->lFSTenderizer;
				} else if (ItemTenderLocalPtr->fbTenderStatus[1] & TENDER_FS_PARTIAL2) {
					*lAmount -= ItemTenderLocalPtr->lFSTenderizer2;
				} else {
					*lAmount -= ItemTenderLocalPtr->lTenderizer;
				}
			}
		}
    }

    if ( TranCurQualPtr->fsCurStatus & CURQUAL_TRAY ) {                     /* tray total transaction */
        if (ItemCommonTaxSystem() == ITM_TAX_INTL) {
            *lAmount += ItemTotalLocalPtr->lTrayVatPayment;
        } else {
			// both US Tax and Canadian Tax use the same tax accumulators.
            *lAmount += ItemTotalLocalPtr->lTrayTax[0] + ItemTotalLocalPtr->lTrayTax[1] + ItemTotalLocalPtr->lTrayTax[2] + ItemTotalLocalPtr->lTrayTax[3];
        }
    }

    return( ITM_SUCCESS );                                          /* amount */
}

/*
*==========================================================================
**  Synopsis:   SHORT   ItemTendCalFSAmount(LONG *lAmount, UIFREGTENDER *UifTender)
*
*   Input:      LONG    *lAmount
*   Output:     LONG    *lAmount
*   InOut:      none
*
*   Return:
*
*   Description:    calculate current FS tender amount.    R3.1, V3.3, Saratoga
*==========================================================================
*/
SHORT   ItemTendCalFSAmount(DCURRENCY *lAmount)
{
    DCURRENCY       lTax = 0L;
	ITEMAFFECTION   Tax = {0};

    if (TranGCFQualPtr->usGuestNo == 0) {     /* ITM_CALTAX_TRANS_OPER - Operator Transaction */
        ItemTotalCalTax(&Tax, 0);
    } else {                                  /* ITM_CALTAX_TRANS_SR - Store/Recall Transaction */
        ItemTotalCalTax(&Tax, 1);
    }

    if (!(TranGCFQualPtr->fsGCFStatus & GCFQUAL_USEXEMPT)) {
		SHORT  i;
        for (i = 0; i < 3; i++) {
            lTax += Tax.USCanVAT.USCanTax.lTaxAmount[i];
        }
    }

#if 0
	// This source code eliminated as part of changes for JIRA AMT-2853.
	// There are a number of places where there was a check on the flags for
	// a CURQUAL_STORE_RECALL type system along with processing a TENDER_PARTIAL in which the
	// total calculation depended on the system type and if we were doing a split or partial tender.
	// We started with changes in functions TrnItemTotal() and ItemTotalCTAmount() in order
	// to reduce the complexity of the source code and dependencies on flags as much as possible
	// as part of making source code changes for AMT-2853.
	//    Richard Chambers for Amtrak Rel 2.2.1 project June 25, 2013
    if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_STORE_RECALL && (ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL)) {
		/* 07/26/01 */
        *lAmount = WorkTI->lMI - ItemTenderLocal.lTenderizer;
        /* *lAmount = WorkTI->lMI + lTax - ItemTenderLocal.lTenderizer
                - ItemTenderLocal.lFSTenderizer + ItemTenderLocal.lFSChange
                + ItemTenderLocal.lFS; */
    } else
#endif
	{
        *lAmount = TranItemizersPtr->lMI + lTax - ItemTenderLocal.lTenderizer
                - ItemTenderLocal.lFSTenderizer + ItemTenderLocal.lFSChange + ItemTenderLocal.lFS;
    }

    return(ITM_SUCCESS);
}

/*==========================================================================
**  Synopsis:   VOID    ItemTendCalAmountSpl(LONG *lAmount, UIFREGTENDER *UifRegTender)
*
*   Input:      LONG    *lAmount
*   Output:     LONG    *lAmount
*   InOut:      none
*
*   Return:
*
*   Description:    calculate current tender amount for split.      R3.1
==========================================================================*/

VOID    ItemTendCalAmountSpl(DCURRENCY *lAmount, CONST UIFREGTENDER *UifRegTender)
{
    TRANITEMIZERS   *WorkTI = TrnGetTISplPointer(TRN_TYPE_SPLITA);
	ITEMCOMMONTAX   WorkTax = {0};
    ITEMCOMMONVAT   WorkVAT = {0};
    UCHAR           auchTotalStatus[NUM_TOTAL_STATUS];

	if (UifRegTender->uchMajorClass == CLASS_UIFREGPAMTTENDER) {
		ItemTendGetTotal(auchTotalStatus, CLASS_UITENDER1);    // if Preset Tender then use the Cash Tender key, Tender # 1.
	} else {
		ItemTendGetTotal(auchTotalStatus, UifRegTender->uchMinorClass);
	}

    if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
        ItemCanTax( &auchTotalStatus[1], &WorkTax, ITM_SPLIT);
    } else if (ItemCommonTaxSystem() == ITM_TAX_US) {
        ItemCurTaxSpl(&auchTotalStatus[1], &WorkTax);
    } else {
        ItemCurVAT( &auchTotalStatus[1], &WorkVAT, ITM_SPLIT);
    }

    *lAmount = WorkTI->lMI;
    if (ItemCommonTaxSystem() == ITM_TAX_INTL) {
        *lAmount += WorkVAT.lPayment;
        if ((! CliParaMDCCheck(MDC_ROUND1_ADR, ODD_MDC_BIT3)) ||
            (CliParaMDCCheck(MDC_ROUND1_ADR, ODD_MDC_BIT3) &&
            UifRegTender->uchMinorClass == CLASS_UITENDER1)) {
            /* if (!(ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL)) { */
                RflRound(lAmount, *lAmount, RND_TOTAL1_ADR);        /* Saratoga */
            /* } */
        }
        *lAmount -= ItemTenderLocal.lTenderizer;
    } else {
        *lAmount += WorkTax.alTax[0] + WorkTax.alTax[1] + WorkTax.alTax[2] + WorkTax.alTax[3];
        if ((! CliParaMDCCheck(MDC_ROUND1_ADR, ODD_MDC_BIT3)) ||
            (CliParaMDCCheck(MDC_ROUND1_ADR, ODD_MDC_BIT3) &&
            UifRegTender->uchMinorClass == CLASS_UITENDER1)) {
            /* if (!(ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL)) { */
                RflRound(lAmount, *lAmount, RND_TOTAL1_ADR);        /* Saratoga */
            /* } */
        }
        *lAmount -= ItemTenderLocal.lTenderizer;
    }
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTendCalAmountSplit(LONG *lAmount, UIFREGTENDER *UifTender)
*
*   Input:      LONG    *lAmount
*   Output:     LONG    *lAmount
*   InOut:      none
*
*   Return:
*
*   Description:    calculate current tender amount for split tender.   R3.1
==========================================================================*/

SHORT   ItemTendCalAmountSplit(DCURRENCY *lAmount, CONST UIFREGTENDER *UifTender)
{
    TRANITEMIZERS   *WorkTI = TrnGetTISplPointer(TRN_TYPE_SPLITA);
    TRANGCFQUAL     *WorkGCF = TrnGetGCFQualPtr();
    SHORT           i;
	UCHAR           auchTotalStatus[NUM_TOTAL_STATUS];

	if (UifTender->uchMajorClass == CLASS_UIFREGPAMTTENDER) {
		ItemTendGetTotal(auchTotalStatus, CLASS_UITENDER1);    // if Preset Tender then use the Cash Tender key, Tender # 1.
	} else {
		ItemTendGetTotal(auchTotalStatus, UifTender->uchMinorClass);
	}

    *lAmount = 0L;

    for (i = 0; WorkGCF->auchUseSeat[i] != 0 && i < NUM_SEAT; i++) {
		ITEMCOMMONTAX   WorkTax = {0};
		ITEMCOMMONVAT   WorkVAT = {0};

        /*----- Check Finalize Seat# 5/17/96 -----*/
		// These function calls set up the memory resident split transaction data to reflect a specific seat
        if (TrnSplSearchFinSeatQueue(WorkGCF->auchUseSeat[i]) == TRN_SUCCESS) {
            continue;
        }
        if (TrnSplGetSeatTrans(WorkGCF->auchUseSeat[i], TRN_SPL_NOT_UPDATE, TRN_CONSOLI_STO, TRN_TYPE_SPLITA) != TRN_SUCCESS) {
            return(LDT_PROHBT_ADR);
        }

        /*----- Seat MI -----*/
        *lAmount += WorkTI->lMI;

        /*----- Individual Tax -----*/
        if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
            ItemCanTax(&auchTotalStatus[1], &WorkTax, ITM_SPLIT);
        } else if (ItemCommonTaxSystem() == ITM_TAX_US) {
            ItemCurTaxSpl(&auchTotalStatus[1], &WorkTax);
        } else {
            ItemCurVAT( &auchTotalStatus[1], &WorkVAT, ITM_SPLIT);
        }

        if (ItemCommonTaxSystem() == ITM_TAX_INTL) {
            *lAmount += WorkVAT.lPayment;
        } else {
            *lAmount += WorkTax.alTax[0] + WorkTax.alTax[1] + WorkTax.alTax[2] + WorkTax.alTax[3];
        }
        if ((! CliParaMDCCheck(MDC_ROUND1_ADR, ODD_MDC_BIT3)) ||
            (CliParaMDCCheck(MDC_ROUND1_ADR, ODD_MDC_BIT3) &&
            UifTender->uchMinorClass == CLASS_UITENDER1)) {
            /* if (!(ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL)) { */
                RflRound(lAmount, *lAmount, RND_TOTAL1_ADR);        /* Saratoga */
            /* } */
        }
    }

    /*----- Base Transaction -----*/
    if (TrnSplGetBaseTrans(TRN_SPL_NOT_UPDATE, TRN_CONSOLI_STO) == TRN_ERROR) {
        ItemTenderLocal.lSplitMI = *lAmount;
        return(ITM_SUCCESS);
    }

    /*----- Whole MI -----*/
    *lAmount += WorkTI->lMI;

	{
		ITEMCOMMONTAX   WorkTax = {0};
		ITEMCOMMONVAT   WorkVAT = {0};

		/*----- Tax of Base Transaction -----*/
		if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
			ItemCanTax( &auchTotalStatus[1], &WorkTax, ITM_SPLIT);
		} else if (ItemCommonTaxSystem() == ITM_TAX_US) {
			ItemCurTaxSpl(&auchTotalStatus[1], &WorkTax);
		} else {
			ItemCurVAT( &auchTotalStatus[1], &WorkVAT, ITM_SPLIT);
		}

		if (ItemCommonTaxSystem() == ITM_TAX_INTL) {
			*lAmount += WorkVAT.lPayment;
		} else {
			// STD_TAX_ITEMIZERS_US and STD_TAX_ITEMIZERS_CAN
			*lAmount += WorkTax.alTax[0] + WorkTax.alTax[1] + WorkTax.alTax[2] + WorkTax.alTax[3];
		}
	}

    if ((! CliParaMDCCheck(MDC_ROUND1_ADR, ODD_MDC_BIT3)) ||
        (CliParaMDCCheck(MDC_ROUND1_ADR, ODD_MDC_BIT3) &&
        UifTender->uchMinorClass == CLASS_UITENDER1)) {
        /* if (!(ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL)) { */
            RflRound(lAmount, *lAmount, RND_TOTAL1_ADR);        /* Saratoga */
        /* } */
    }

    ItemTenderLocal.lSplitMI = *lAmount;

    return(ITM_SUCCESS);
}


/*
*==========================================================================
**  Synopsis:   VOID    ItemTendGetTotal(UCHAR *auchTotalStatus, UCHAR uchMinor)
*
*   Input:      UCHAR   *auchTotalStatus
*   Output:     UCHAR   *auchTotalStatus
*   InOut:      none
*   Return:
*
*   Description:    get total key parameter,                    V3.3
*==========================================================================
*/
SHORT   ItemTendGetTotal(UCHAR *auchTotalStatus, UCHAR uchMinor)
{
	SHORT           sRetStatus = ITM_SUCCESS;

    TranCurQualTotalStatusGet (auchTotalStatus);

    /* --- Not Use Any Total Key --- */
    if (auchTotalStatus[0] == 0) {      // if not total key pressed then
        if (uchMinor == 0) {
			ItemTotalAuchTotalStatus (auchTotalStatus, CLASS_TOTAL2);
        } else {
			UIFREGTOTAL     UifTotal = {0};
			PARATEND        ParaTend = {0};

            ParaTend.uchMajorClass = CLASS_PARATEND;
            ParaTend.uchAddress = uchMinor;
            CliParaRead(&ParaTend);
			//We now allow Tenders that arent assigned a total
			//to have the minor class assigned to 0, so that we
			//dont follow Total 2's control code regarding Printing
            if (ParaTend.uchTend == 2 /*|| ParaTend.uchTend == 0*/) {
                UifTotal.uchMinorClass = CLASS_UITOTAL2;
            } else {
                UifTotal.uchMinorClass = ParaTend.uchTend;
            }
			if(ParaTend.uchTend)
			{
				if (ParaTend.uchTend > CLASS_UITOTAL20 || ParaTend.uchTend < CLASS_UITOTAL3 ||
					ParaTend.uchTend == CLASS_UITOTAL9 || ItemTotalType(&UifTotal) != ITM_TYPE_NONFINALIZE)
				{
					/* --- Use Check Total Key --- */
					auchTotalStatus[0] = ParaTend.uchTend;
					ItemTotalAuchTotalStatus (auchTotalStatus, CLASS_TOTAL2);
				} else {
					/* --- Use Other Total Key --- */
					ItemTotalAuchTotalStatus (auchTotalStatus, ParaTend.uchTend);
				}
			} else {
				sRetStatus = LDT_SEQERR_ADR;
			}
        }                                                           /* R3.1 */
    }

    /* V3.3 ICI */
/***
    if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK
        && !CliParaMDCCheck( MDC_GCNO_ADR, EVEN_MDC_BIT3)
        && !CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2)) {
        /----- Not Use "Not Consoli Print" Option -----/
        auchTotalStatus[4] &= ~0x20;
    }
***/

	return sRetStatus;
}


/*==========================================================================
**  Synopsis:   SHORT   ItemTendNoPerson( VOID )
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:     No of person for memory affection
*
*   Description:    generate affection amount of No person
==========================================================================*/

SHORT   ItemTendNoPerson( VOID )
{
    if ( TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN) ) {   /* preselect void */
#if 1
		// If this is a Transaction Void then we do not change the number of persons
		// so just return a value of zero persons.
		return 0;
#else
        if ((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_NEWCHECK
            || ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_STORE_RECALL
                && !((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_REORDER)
                && !((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_ADDCHECK))) {

            return( -1 * WorkGCF->usNoPerson);                           /* affection amount */

        } else if ( ( TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_CASHIER ) {
            if ( WorkGCF->usNoPerson != 0 ) {                            /* No person entry */
                return( -1 * WorkMod.usCurrentPerson );                 /* affection amount */
            }

            if ( TranCurQualPtr->fsCurStatus & CURQUAL_TRAY ) {         /* tray total transaction */
                return( -1 * WorkTotal.usNoPerson );                    /* tray counter */

            } else {
                /* --- Saratoga,    Nov/27/2000 --- */
                if (WorkMod.fbModifierStatus & MOD_ZEROPERSON) {
                    return(0);
                } else {
                    return(-1);
                }
            }

        } else {
            return( 0 );                                                /* no affection */
        }
#endif
    } else {                                                            /* normal */

        if ((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_NEWCHECK
            || ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_STORE_RECALL
                && !((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_REORDER)
                && !((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_ADDCHECK))) {

            return( TranGCFQualPtr->usNoPerson );                               /* affection amount */

        } else if ( ( TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_REORDER
            || ( TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_ADDCHECK ) {

            return( ItemModLocalPtr->usCurrentPerson );                          /* affection amount */

        } else {                                                        /* cashier operation */
            if ( TranGCFQualPtr->usNoPerson != 0 ) {                            /* No person entry */
                return( ItemModLocalPtr->usCurrentPerson );                      /* affection amount */
            }

            if ( TranCurQualPtr->fsCurStatus & CURQUAL_TRAY ) {         /* tray total transaction */
                return( ItemTotalLocalPtr->usNoPerson );                         /* tray counter */

            } else {
                /* --- Saratoga,    Nov/27/2000 --- */
                if (ItemModLocalPtr->fbModifierStatus & MOD_ZEROPERSON) {
                    return(0);
                } else {
                    return(1);
                }
            }
        }
    }
}



/*==========================================================================
**  Synopsis:   SHORT   ItemTendGetOffset( VOID )
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:     hourly block offset
*
*   Description:    get hourly block offset
==========================================================================*/

SHORT   ItemTendGetOffset( VOID )
{
    SHORT   i;
    USHORT  usTOD, usTime1, usTime2;
    DATE_TIME   WorkTod;
    PARAHOURLYTIME  WorkHourly;

    PifGetDateTime( &WorkTod );                                     /* get TOD */
    usTOD = 0x100 * WorkTod.usHour + WorkTod.usMin;                 /* TOD */

    WorkHourly.uchMajorClass = CLASS_PARAHOURLYTIME;
    WorkHourly.uchAddress = HOUR_ACTBLK_ADR;                        /* No of hourly block */
    CliParaRead( &WorkHourly );

    WorkHourly.uchAddress = HOUR_TSEG1_ADR;                         /* 1'st hourly block */
    CliParaRead( &WorkHourly );
    usTime1 = 0x100 * WorkHourly.uchHour + WorkHourly.uchMin;       /* 1'st hourly block */

    WorkHourly.uchAddress = HOUR_TSEG2_ADR;                         /* 2'nd hourly block */
    CliParaRead( &WorkHourly );
    usTime2 = 0x100 * WorkHourly.uchHour + WorkHourly.uchMin;       /* 2'nd hourly block */

    for ( i = 0; i != ( SHORT )( WorkHourly.uchBlockNo - 1 ); i++ ) {   /* search target hourly block */

        if ( usTime1 <= usTOD && usTOD < usTime2 ) {
            return( i );                                            /* exit loop */
        }

        if ( usTime1 >= usTime2 ) {                                 /* normal */
            if ( usTOD >= usTime1 || usTOD < usTime2 ) {
                return( i );                                        /* exit loop */
            }
        }

        usTime1 = usTime2;
        WorkHourly.uchAddress = ( UCHAR )( HOUR_TSEG1_ADR + i + 2 );/* hourly block */
        CliParaRead( &WorkHourly );
        usTime2 = 0x100 * WorkHourly.uchHour + WorkHourly.uchMin;   /* hourly block */

    }

    return( i );
}


/*==========================================================================
**  Synopsis:   VOID    ItemTendMulti( VOID )
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:        0 - printing was done
*                 -1 - printing was not done
*
*   Description:    generate multi check payment data and prints split receipts
*                   return value of 0 indicates printing was done.
*                   return value of -1 indicates no printing was done
==========================================================================*/
SHORT    ItemTendSplitReceipts (CONST UIFREGTENDER *pUifRegTender, ITEMTENDER *pItemTender)
{
	TRANCURQUAL   *pWorkCur = TrnGetCurQualPtr();
	USHORT fbCompPrint = (PRT_RECEIPT | PRT_JOURNAL);

	if ((ItemTenderLocal.fbTenderStatus[1] & TENDER_FS_PARTIAL) == 0 && (ItemTenderLocal.fbTenderStatus[1] & TENDER_FS_PARTIAL2) == 0) {
		if ((ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL) == 0 && (ItemTenderLocal.fbTenderStatus[1] & TENDER_SEAT_PARTIAL) == 0) {
			// If this is not part of a split or partial tender then do not do anything
			// We include partial food stamps in this as well since Amtrak uses food stamp items for Train Delay Voucher (TDV).
			// Amtrak Train Delay Voucher uses food stamp in order to restrict the use of Train Delay Voucher to only
			// some types of PLUs and to not allow a voucher with other items such as alcohol.
			return -1;
		}
	}

	ItemTendSetPrintStatus (pItemTender);

	if (ItemCheckCustomerSettingsFlags (SYSCONFIG_CUSTOMER_ENABLE_USCBP | SYSCONFIG_CUSTOMER_ENABLE_AMTRAK)) {
		// this is default behavior for Amtrak and for US Customs and Border Protection.
		// we want to consolidate the Cash and Check and Foreign Currency on the split tender summary receipt.
		PARATENDERKEYINFO TendKeyInfo;

		TendKeyInfo.uchMajorClass = CLASS_PARATENDERKEYINFO;
		TendKeyInfo.uchAddress = pItemTender->uchMinorClass;
		CliParaRead(&TendKeyInfo);

		if (pItemTender->uchMinorClass >= CLASS_FOREIGN1 && pItemTender->uchMinorClass <= CLASS_FOREIGN1) {
			// Foreign Currency treated like Cash 
			return -1;
		} else if (TendKeyInfo.TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_CHARGE || TendKeyInfo.TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_CHECK) {
			// Cash or Check so do not do a split tender receipt.
			return -1;
		}
	}

	if (ItemTenderCheckTenderMdc (pUifRegTender->uchMinorClass, MDC_TENDER_PAGE_6, MDC_PARAM_BIT_A)) {
		// check if split receipts not wanted this tender key
		return -1;
	}

	if (ItemTendCheckEPT(pUifRegTender) == ITM_EPT_TENDER) {
		ULONG         ulMinTotal;
		ULONG         ulCreditFloorStatusCheck = (A128_CC_ENABLE | A128_CC_CUST | A128_CC_MERC);
		PARAMISCPARA  ParaMiscPara = {0};

		ParaMiscPara.uchMajorClass = CLASS_PARAMISCPARA;
		ParaMiscPara.uchAddress = MISC_CREDITFLOOR_ADR;	// Address for where the floor amount is stored
		ParaMiscParaRead(&ParaMiscPara);
		ulMinTotal = ParaMiscPara.ulMiscPara;
		ParaMiscPara.uchAddress = MISC_CREDITFLOORSTATUS_ADR;	// Address for where the status bits are stored
		ParaMiscParaRead(&ParaMiscPara);
		if ((ParaMiscPara.ulMiscPara & ulCreditFloorStatusCheck) == ulCreditFloorStatusCheck && ulMinTotal > 0 && RflLLabs(pItemTender->lTenderAmount) <= ulMinTotal) {
			// check if split receipts not wanted this tender key
			return -1;
		}

		ItemTendSplitRec (pItemTender, fbCompPrint);    // print the split or partial tender receipt

		if ((pWorkCur->fbNoPrint & CURQUAL_NO_SECONDCOPY) == 0) {
			USHORT        fsPrintStatusSave;              /* print status */

			SetReceiptPrint(SET_RECEIPT_SECOND_COPY);
			fbCompPrint = PRT_RECEIPT;
			fsPrintStatusSave = pItemTender->fsPrintStatus;
			pItemTender->fsPrintStatus &= ~PRT_JOURNAL;           /* avoid duplicated journal */
			ItemTendSplitRec (pItemTender, fbCompPrint);      // print the split or partial tender receipt
			pItemTender->fsPrintStatus = fsPrintStatusSave;
		}

		pWorkCur->fsCurStatus &= ~(CURQUAL_CUST_DUP | CURQUAL_MERCH_DUP | CURQUAL_PRINT_EPT_LOGO);
		ItemTenderLocal.fbTenderStatus[0] &= ~ TENDER_EPT;

		pWorkCur->fbNoPrint &= ~CURQUAL_NO_EPT_LOGO_SIG;     // clear the indicator and set it back below if still necessary
		pWorkCur->fbNoPrint &= ~(CURQUAL_NO_SECONDCOPY);     // assume we are doing two receipts for now.
		pWorkCur->fbNoPrint &= ~(CURQUAL_NO_R);              // assume we want at least one receipt for now.
	}
	else {
		ITEMMODLOCAL  *pModLocalDataRcvBuff = ItemModGetLocalPtr();
		TRANMODEQUAL  *pModeQualRcvBuff = TrnGetModeQualPtr();

		pWorkCur->fbNoPrint &= ~(CURQUAL_NO_R);              // assume we want at least one receipt for now.
		pWorkCur->fbNoPrint |= (CURQUAL_NO_EPT_LOGO_SIG);    // assume we do not want ept logo.
		if (pWorkCur->fsCurStatus2 & PRT_DUPLICATE_COPY) {
			ItemTendSplitRec (pItemTender, fbCompPrint);    // print the split or partial tender receipt
			pWorkCur->fsCurStatus2 &= ~PRT_DUPLICATE_COPY;
		} else {
			if ( pModeQualRcvBuff->fsModeStatus & MODEQUAL_NORECEIPT_DEF ) {
				pWorkCur->fbNoPrint |= CURQUAL_NO_R;       /* MDC says otherwise */
			}
			if ( pModLocalDataRcvBuff->fbModifierStatus & MOD_NORECEIPT ) {
				pWorkCur->fbNoPrint ^= CURQUAL_NO_R;       /* override by Receipt key press so invert */
			}
			if ((pWorkCur->fbNoPrint & CURQUAL_NO_R) == 0) {
				ItemTendSplitRec (pItemTender, fbCompPrint);    // print the split or partial tender receipt
			}
		}
	}

	return 0;
}

VOID    ItemTendMulti( VOID )
{
    TRANITEMIZERS   *WorkTI = TrnGetTIPtr();
    DCURRENCY       lTax = 0;

    if ( ItemTransLocalPtr->uchCheckCounter && ((ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL) == 0)) {   /* exist multi check */
		TRANCURQUAL     *pWorkCur = TrnGetCurQualPtr();
		ITEMMULTI       ItemMulti = {0};
		ITEMCOMMONTAX   WorkTax = {0};
		ITEMCOMMONVAT   WorkVAT = {0};
		UCHAR           j;

		//SR 279, moved this statement below because we are still wanting to know whether it is
		//a multi check payment or not below.

/*       WorkCur.fsCurStatus &= ~CURQUAL_MULTICHECK;                     // terminate multi check payment
        TrnPutCurQual( &WorkCur );*/

        ItemMulti.uchMajorClass = CLASS_ITEMMULTI;

        if ( ( pWorkCur->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_POSTCHECK ) {
            ItemMulti.uchMinorClass = CLASS_MULTIPOSTCHECK;             /* multi check payment */
        } else {                                                        /* precheck system */
            ItemMulti.uchMinorClass = CLASS_MULTICHECKPAYMENT;          /* multi check payment */
        }
        ItemMulti.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;

		//SR 279 JHHJ
		//This if statement now allows the user to decide whether or not to recalculate
		//tax when two checks are added together.
		if((pWorkCur->fsCurStatus & CURQUAL_MULTICHECK))
		{
			 // we are in multiple check mode.
			pWorkCur->fsCurStatus &= ~CURQUAL_MULTICHECK;                     /* terminate multi check payment */
			if(!CliParaMDCCheck(MDC_TAXINTL_ADR, ODD_MDC_BIT1)
				|| ((pWorkCur->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK))//one cent tax
			{
				if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
					ItemCanTax( &pWorkCur->auchTotalStatus[1], &WorkTax, ITM_NOT_SPLIT );
#pragma message("Remove this check once we determine TH ISSUE JHHJ")
					if(WorkTax.alTaxable[1] > 100000)
					{
						// Tim Horton's of Canada was having a problem with a tax calculation error
						// which would suddenly happen and then not be seen for a while.
						// This was back in Rel 2.1.0 around 2007 or so.  They are no longer a customer.
						//   Richard Chambers, Apr-09-2015
						NHPOS_ASSERT(!"ItemTendMulti");
					}
				} else if (ItemCommonTaxSystem() == ITM_TAX_US) {               /* V3.3 */
					ItemCurTax( &pWorkCur->auchTotalStatus[1], &WorkTax );              /* calculate current tax */
				} else {
					ItemCurVAT( &pWorkCur->auchTotalStatus[1], &WorkVAT, ITM_NOT_SPLIT);
				}

				if (ItemCommonTaxSystem() == ITM_TAX_INTL) {
					lTax = WorkVAT.lPayment;
				} else {
					lTax = WorkTax.alTax[0] + WorkTax.alTax[1] + WorkTax.alTax[2] + WorkTax.alTax[3];
				}
			}
		} else
		{
			if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
				ItemCanTax( &pWorkCur->auchTotalStatus[1], &WorkTax, ITM_NOT_SPLIT );
#pragma message("Remove this check once we determine TH ISSUE JHHJ")
				if(WorkTax.alTaxable[1] > 100000)
				{
					// Tim Horton's of Canada was having a problem with a tax calculation error
					// which would suddenly happen and then not be seen for a while.
					// This was back in Rel 2.1.0 around 2007 or so.  They are no longer a customer.
					//   Richard Chambers, Apr-09-2015
					NHPOS_ASSERT(!"ItemTendMulti2 TAX ERROR");
				}

			} else if (ItemCommonTaxSystem() == ITM_TAX_US) {               /* V3.3 */
				ItemCurTax( &pWorkCur->auchTotalStatus[1], &WorkTax );              /* calculate current tax */

			} else {
				ItemCurVAT( &pWorkCur->auchTotalStatus[1], &WorkVAT, ITM_NOT_SPLIT);
			}

			if (ItemCommonTaxSystem() == ITM_TAX_INTL) {
				lTax = WorkVAT.lPayment;
			} else {
				lTax = WorkTax.alTax[0] + WorkTax.alTax[1] + WorkTax.alTax[2] + WorkTax.alTax[3];
			}
		}

        for ( j = 0; ItemTransLocalPtr->uchCheckCounter != j; j++ ) {
			SHORT           i;

            for( i = 0; i < 3; i++ ) {
                ItemMulti.lService[i] = ItemTransLocalPtr->alAddcheck[j][i];
            }
            ItemMulti.usGuestNo = ItemTransLocalPtr->ausGuestNo[j];              /* GCF No */
            ItemMulti.uchCdv = ItemTransLocalPtr->auchCdv[j];                    /* CDV */
            ItemMulti.lMI = ItemTransLocalPtr->alMI[j];                          /* MI */
            TrnItem( &ItemMulti );                                      /* transaction module i/F */

            /*--- Display Multi Check, R3.0 ---*/
            MldScrollWrite(&ItemMulti, MLD_NEW_ITEMIZE);
            MldScrollFileWrite(&ItemMulti);

            if ( j == ( UCHAR )( ItemTransLocalPtr->uchCheckCounter - 1 ) ) {    /* last multi check (grand total) */
				UCHAR           k;

                ItemMulti.uchMinorClass = CLASS_MULTITOTAL;             /* multi check payment */
                if ( ( pWorkCur->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_POSTCHECK ) {
                    ItemMulti.uchMinorClass = CLASS_MULTIPOSTTOTAL;     /* multi check payment */
                }
                ItemMulti.usGuestNo = 0;
                ItemMulti.uchCdv = 0;

                for( i = 0; i < 3; i++ ) {
                    ItemMulti.lService[i] = 0L;
                }
                for ( k = 0; ItemTransLocalPtr->uchCheckCounter != k; k++ ) {    /* whole addcheck total */
                    for( i = 0; i < 3; i++ ) {
                        ItemMulti.lService[i] += ItemTransLocalPtr->alAddcheck[k][i];
                    }
                }
                for( i = 0; i < 3; i++ ) {
                    ItemMulti.lService[i] += WorkTI->lService[i];
                }
                ItemMulti.lMI = WorkTI->lMI + lTax;

                for ( k = 0; ItemTransLocalPtr->uchCheckCounter != k; k++ ) {    /* whole addcheck total */
                    ItemMulti.lMI += ItemTransLocalPtr->alMI[k];
                }
                TrnItem( &ItemMulti );                                  /* transaction module i/F */

                /*--- Display Multi Check, R3.0 ---*/
                MldScrollWrite(&ItemMulti, MLD_NEW_ITEMIZE);
                MldScrollFileWrite(&ItemMulti);
            }
        }
    }
}



/*==========================================================================
**   Synopsis:  VOID    ItemTendDispIn( VOID *DispMsg )
*
*   Input:      VOID    *DispMsg
*   Output:     VOID    *DispMsg
*   InOut:      none
*
*   Return:
*
*   Description:    generate default mnemonic(cashier/waiter)
==========================================================================*/

VOID    ItemTendDispIn( REGDISPMSG  *pDisp )
{
    memset( pDisp, 0, sizeof( REGDISPMSG ) );
    pDisp->uchMajorClass = CLASS_UIFREGOPEN;
    pDisp->uchMinorClass = CLASS_UICASHIERIN;
    pDisp->fbSaveControl = 0x01;                                    /* save for redisplay (release on finalize) */
    _tcsncpy(pDisp->aszMnemonic, TranModeQualPtr->aszCashierMnem, NUM_OPERATOR );
}


/*==========================================================================
**  Synopsis:   VOID    ItemTendAudVal( LONG lAmount )
*
*   Input:      LONG    lAmount
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    generate audaction total data
==========================================================================*/

VOID    ItemTendAudVal( DCURRENCY lAmount )
{
	ITEMTOTAL   WorkTotal = {0};

    WorkTotal.uchMajorClass = CLASS_ITEMTOTAL;                      /* major class */
    WorkTotal.uchMinorClass = CLASS_TOTALAUDACT;                    /* audaction class */
    WorkTotal.lMI = lAmount;                                        /* audaction balance */
    WorkTotal.fsPrintStatus = PRT_VALIDATION;                           /* print validation */

    TrnThrough( &WorkTotal );
}

/*
*===========================================================================
** Synopsis:    SHORT   ItemTendPreTenderEdit( UIFREGTENDER *UifRegTender )
*
*     Input:    *pUifRegTender  : Pointer to Structure for UIFREGTENDER
*    Output:    nothing
*     InOut:    UifRegTender->lTenderAmount : Tender Amount
*               UifRegTender->uchMinorClass : Preset Tender index or address
*               UifRegTender->uchMajorClass  : CLASS_UIFREGPAMTTENDER
*
*
** Return:      ITM_SUCCESS     : Function performed successfully
*               LDT_PROHBT_ADR  : Prohibit error
*               LDT_KEYOVER_ADR : Key entered over limitation
*
** Description: Check and make preset amount cash tender data Module by 
*               looking up the amount specified for the designated Preset Tender
*               address or index or Extended FSC. See also FSC_PRESET_AMT and CWindowButton::BATypePreCash.
*
*               If the Preset Tender amount is not set, value in table is zero, then
*               we will round up the transaction amount and use that.
*===========================================================================
*/
SHORT   ItemTendPreTenderEdit( UIFREGTENDER *UifRegTender )
{
	PARAPRESETAMOUNT    ParaPresetAmount = {0};
	UCHAR   uchMinorClass = ((UifRegTender->uchMinorClass - 1) % MAX_PRESETAMOUNT_SIZE) + 1;

    if (TranCurQualPtr->auchTotalStatus[0] == 0) {      // if not total key pressed then
        //return(LDT_SEQERR_ADR);
    }

    /* check if QTY extension entry for preset amount is allowed */
    if (UifRegTender->lTenderAmount) {
		// MDC address bit will vary depending on which Preset is being used.
		UCHAR   uchMaskPattern[] = {EVEN_MDC_BIT0, EVEN_MDC_BIT0, EVEN_MDC_BIT1, EVEN_MDC_BIT2, EVEN_MDC_BIT3};

		NHPOS_ASSERT(uchMinorClass < sizeof(uchMaskPattern)/sizeof(uchMaskPattern[0]));
        if (CliParaMDCCheck(MDC_PRETEND_ADR, uchMaskPattern[uchMinorClass]) != 0) {    /* prohibit case */
            return(LDT_SEQERR_ADR);
        }
    }

    /* get preset amount */
    ParaPresetAmount.uchMajorClass =  CLASS_PARAPRESETAMOUNT;    // ItemTendPreTenderEdit() - check and make preset amount cash tender.
    ParaPresetAmount.uchAddress = uchMinorClass;
    CliParaRead(&ParaPresetAmount);

    if (CliParaMDCCheckField(MDC_DRAWER2_ADR, MDC_PARAM_BIT_A)) {    /* reinterpret Preset data area as USHORT not ULONG to allow 8 presets. */
		// Steve Dudek has asked for increasing the number of Preset Amounts to allow the provisioning of
		// Preset Amounts for $5, $10, $20, $50, $100, and next dollar amounts which requires at least
		// six Preset Amount slots. All of these values are within the range of 0 to 65000 as the max
		// value desired is 10000 for $100. See also FSC_PREAMT_MAX as well as FSC_PRESET_AMT and CWindowButton::BATypePreCash.
		//      Richard Chambers, Sep-07-2018, GenPOS Rel 2.2.2
		if (UifRegTender->uchMinorClass > MAX_PRESETAMOUNT_SIZE) {
			ParaPresetAmount.ulAmount >>= 16;
		}
		ParaPresetAmount.ulAmount &= 0x0000ffff;
	}

    /* check preset amount */
    if (!ParaPresetAmount.ulAmount) {
		DCURRENCY      lAmount = 0;

        /* whole amount tender, if preset amount is zero, 2172 */
        if (UifRegTender->lTenderAmount) {
            return( LDT_PROHBT_ADR );
        }

		// generate a preset tender amount based on transaction balance
		{
			SHORT          sSplitStatus = TrnSplCheckSplit();
			UIFREGTENDER   UifTender = {0};

			UifTender.uchMajorClass = CLASS_UIFREGTENDER;
			UifTender.uchMinorClass = CLASS_UITENDER1;

			/*----- Split Check,   R3.1 -----*/
			if (sSplitStatus == TRN_SPL_SEAT
				|| sSplitStatus == TRN_SPL_MULTI
				|| sSplitStatus == TRN_SPL_TYPE2)
			{
				ItemTendCalAmountSpl(&lAmount, &UifTender);
			} else if (sSplitStatus == TRN_SPL_SPLIT) {
				ItemTendCalAmountSplit(&lAmount, &UifTender);
			} else {
				if (ItemTenderLocal.fbTenderStatus[1] & TENDER_FS_PARTIAL) {
					ItemTendCalFSAmount(&lAmount);
				} else {
					ItemTendCalAmount(&lAmount, &UifTender);
				}
			}

			// determine number of decimal places for currency and do basic rounding up
			// to the next whole number amount.
			switch (CliParaMDCCheckField (MDC_SYSTEM3_ADR, MDC_PARAM_2_BIT_C)) {
				case 0:        /* 2 decimal rounding */
					lAmount += 99L;
					lAmount /= 100L;
					lAmount *= 100L;
					break;
				case 1:        /* 3 decimal rounding */
					lAmount += 999L;
					lAmount /= 1000L;
					lAmount *= 1000L;
					break;
				case 2:        /* 0 decimal */
				case 3:        /* 0 decimal */
					break;
			}
		}

        UifRegTender->lTenderAmount = lAmount;
        if (UifRegTender->lTenderAmount < 0L) {
            UifRegTender->lTenderAmount *= -1;
        }
		
        /* modify minor class to be standard Cash Tender key, Tender Key #1 */
        UifRegTender->uchMinorClass = CLASS_UITENDER1;

        return(ITM_SUCCESS);
    }

    if (!UifRegTender->lTenderAmount) {
        UifRegTender->lTenderAmount = 1L;
    }

	{
		D13DIGITS   d13ProdWork;

		/* check and make cash tender amount */
		d13ProdWork = UifRegTender->lTenderAmount;
		d13ProdWork *= ParaPresetAmount.ulAmount;

		if (d13ProdWork > STD_MAX_TOTAL_AMT || d13ProdWork < - STD_MAX_TOTAL_AMT) {  /* above max. sales price ? */
			return(LDT_KEYOVER_ADR);
		} else {
			UifRegTender->lTenderAmount = d13ProdWork;
		}
	}

    /* modify minor class to be standard Cash Tender key, Tender Key #1 */
    UifRegTender->uchMinorClass = CLASS_UITENDER1;

    return(ITM_SUCCESS);
}

/*==========================================================================
**   Synopsis:  SHORT   ItemTendCheckEPT( UIFREGTENDER  *UifRegTender)
*
*   Input:      UIFREGTENDER    *UifRegTender
*   Output:     none
*   InOut:      none
*
*   Return:     > 0 : error
*               0   : Not EPT Tender
*               1   : EPT Tender
*
*   Description:    check EPT tender or Not
*===========================================================================
*/
SHORT ItemTendCheckEPT(CONST UIFREGTENDER  *UifRegTender)
{
	if (UifRegTender->uchMajorClass == CLASS_UIFREGPAMTTENDER) {
		// Preset Tender keys are never EPT tender keys.
        return(ITM_SUCCESS);                    /* not EPT Tender   */
    }

    if (! ItemTenderCheckTenderMdc (UifRegTender->uchMinorClass, MDC_TENDER_PAGE_3, MDC_PARAM_BIT_A)) {    /* EPT Tender ?     */
        return(ITM_SUCCESS);                    /* not EPT Tender   */
    }

	if ((UifRegTender->fbModifier & OFFCPTEND_MODIF) || (UifRegTender->fbModifier & OFFEPTTEND_MODIF)) {
		if (! ItemTenderCheckTenderMdc (UifRegTender->uchMinorClass, MDC_TENDER_PAGE_4, MDC_PARAM_BIT_D)) {    /* offline prohibit ?   */
			NHPOS_ASSERT_TEXT(0, "==NOTE: LDT_BLOCKEDBYMDC_ADR offline prohibited by MDC this tender.");
			return( LDT_BLOCKEDBYMDC_ADR );
		} else {
			return(ITM_SUCCESS);                /* not EPT Tender   */
		}
	}

    return(ITM_EPT_TENDER);                     /* EPT Tender           */
}

/*==========================================================================
**  Synopsis:   SHORT ItemTendCompEntry(UCHAR *pDesBuffer, UCHAR uchMaxLen)
*
*   Input:      UCHAR       uchMaxLen
*   Output:     UCHAR       *pDesBuffer
*   InOut:      nothing
*
*   Return:     ITM_SUCCESS     : successful
*               UIF_CANCEL      : canceled
*
*   Description: account # and expiration date entry
==========================================================================*/

SHORT ItemTendCompEntry(TCHAR *pDesBuffer, UCHAR uchMaxLen)
{
    for (;;) {
		UIFDIADATA      UifDiaRtn = {0};
		REGDISPMSG      RegDispMsgD = {0};
		SHORT           sReturn;
		USHORT          usAddress = 0;

        if (uchMaxLen == UIFREG_MAX_DIGIT19) {
            usAddress = LDT_NUM_ADR;
        } else {
            usAddress = LDT_ENT_EXP_ADR;
        }

        RegDispMsgD.uchMajorClass = CLASS_REGDISPCOMPLSRY;
        RegDispMsgD.uchMinorClass = CLASS_REGDISP_CPAN;
        RegDispMsgD.fbSaveControl = 4;
        RflGetLead (RegDispMsgD.aszMnemonic, usAddress);

		flDispRegDescrControl |= TOTAL_CNTRL;
        flDispRegKeepControl |= TOTAL_CNTRL;
        if (TranGCFQualPtr->fsGCFStatus & GCFQUAL_USEXEMPT) {
            flDispRegDescrControl |= TAXEXMPT_CNTRL;        /* tax exempt */
            flDispRegKeepControl |= TAXEXMPT_CNTRL;         /* tax exempt */
        }
        DispWrite(&RegDispMsgD);

        /* execute account,expiration date # entry */
        if ((sReturn = UifDiaCP4(&UifDiaRtn, uchMaxLen)) == UIF_SUCCESS) {
            /* set data */
            _tcsncpy(pDesBuffer, UifDiaRtn.aszMnemonics, uchMaxLen);
            return(ITM_SUCCESS);
        } else if (sReturn == UIFREG_ABORT) {
            return(UIF_CANCEL);
        }
    }
}



/*==========================================================================
**  Synopsis:   VOID    ItemTendCalServTime(VOID)
*
*   Input:      None
*   Output:
*   InOut:      nothing
*
*   Return:
*
*   Description:    Calculate Service Time.                    R3.1
==========================================================================*/
VOID    ItemTendCalServTime(VOID)
{
    SHORT       sFlag = 0;
	FDTPARA     WorkFDT = { 0 };

	FDTParameter(&WorkFDT);
    switch(WorkFDT.uchSysTypeTerm) {
    case    FX_DRIVE_SYSTYPE_1T_OP:     /* Order/Payment System, one terminal */
    case    FX_DRIVE_SYSTYPE_2T_OP:     /* Order, Payment System, two terminals */
        sFlag = 1;
        break;

    case    FX_DRIVE_SYSTYPE_2T_OP_D:   /* Order/Payment, Delivery System, two terminals  */
		// determine if we need to calculate service time for this terminal based on type.
        switch(WorkFDT.uchTypeTerm) {
        case    FX_DRIVE_DELIV_1:
        case    FX_DRIVE_DELIV_2:
            if ((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_CASHIER) {
                sFlag = 1;
            }
            break;

        case    FX_DRIVE_COUNTER_STORE:                         /* 96/4/2 */
        case    FX_DRIVE_COUNTER_STORE_PAY:                     /* 96/4/2 */
        case    FX_DRIVE_COUNTER_FUL_STORE:                     /* 96/4/2 */
        case    FX_DRIVE_COUNTER_FUL_STORE_PAY:                 /* 96/4/2 */
            sFlag = 1;                                          /* 96/4/2 */
            break;                                              /* 96/4/2 */

        default:
            sFlag = 0;
            break;
        }
		break;

	default:
		NHPOS_ASSERT_TEXT(0, "**ERROR: WorkFDT.uchSysTypeTerm out of range.");
        sFlag = 0;
		break;
    }

    if (sFlag != 0) {
		ITEMAFFECTION   ItemAffect = {0};

		ItemAffect.uchMajorClass = CLASS_ITEMAFFECTION;
        ItemAffect.uchMinorClass = CLASS_SERVICETIME;
        ItemTransCalSec(&ItemAffect);
        TrnItem(&ItemAffect);
    }
}



/*==========================================================================
**  Synopsis:   VOID    ItemTendAutoChargeTips(VOID)
*
*   Input:      None
*   Output:
*   InOut:      nothing
*
*   Return:
*
*   Description:    Create automatic charge tips data & display, V3.3
==========================================================================*/
VOID    ItemTendAutoChargeTips(VOID)
{
    ITEMCOMMONLOCAL *pItemCommonL = ItemCommonGetLocalPointer();
    SHORT           sStatus = LDT_PROHBT_ADR;

    /* if auto charge tips is done */
    if ((pItemCommonL->ItemDisc.uchMajorClass == CLASS_ITEMDISC ) &&
        (( pItemCommonL->ItemDisc.uchMinorClass == CLASS_AUTOCHARGETIP ) ||
         ( pItemCommonL->ItemDisc.uchMinorClass == CLASS_AUTOCHARGETIP2 ) ||
         ( pItemCommonL->ItemDisc.uchMinorClass == CLASS_AUTOCHARGETIP3 )) )
	{
        sStatus = ITM_SUCCESS;
    } else {
		UIFREGDISC      UifRegDisc = {0};

        UifRegDisc.uchMajorClass = CLASS_UIFREGDISC;
        UifRegDisc.uchMinorClass = CLASS_UIAUTOCHARGETIP;
        sStatus = ItemDiscount(&UifRegDisc);        /* Discount Modele */
        /* store charge tips class */
        if (sStatus == ITM_SUCCESS) {
			SHORT           sSplitStatus = TrnSplCheckSplit();
			ITEMTENDER      DummyTender = {0};

            if (sSplitStatus == TRN_SPL_SEAT
                || sSplitStatus == TRN_SPL_MULTI
                || sSplitStatus == TRN_SPL_TYPE2)
			{
                ItemPreviousItemSpl(&DummyTender, 0);       /* Saratoga */
            } else {
                ItemPreviousItem(&DummyTender, 0 );         /* Saratoga */
            }
        }
    }

    if (sStatus == ITM_SUCCESS) {
		SHORT           sSplitStatus = TrnSplCheckSplit();

        /* send to enhanced kds, 2172 */
        ItemSendKds(&(pItemCommonL->ItemDisc), 0);

        /* display on lcd by tender execution */

        /* reset unbuffering print status */
        pItemCommonL->ItemDisc.fsPrintStatus &= ~PRT_NO_UNBUFFING_PRINT;

        if (sSplitStatus == TRN_SPL_SEAT
            || sSplitStatus == TRN_SPL_MULTI
            || sSplitStatus == TRN_SPL_TYPE2) {
            MldScrollWrite2(MLD_DRIVE_2, &(pItemCommonL->ItemDisc), MLD_NEW_ITEMIZE);
        } else {
            MldScrollWrite(&(pItemCommonL->ItemDisc), MLD_NEW_ITEMIZE);
        }
        MldScrollFileWrite(&(pItemCommonL->ItemDisc));

        /* print out at unbuffering print */
        TrnPrintType(&(pItemCommonL->ItemDisc));

        /*----- PRINT TICKET FOR PREVIOUS ITEM -----*/
        if (CliParaMDCCheck(MDC_CHRGTIP2_ADR, EVEN_MDC_BIT0)) {
            pItemCommonL->ItemDisc.fsPrintStatus |= PRT_SINGLE_RECPT;      /* issue single receipt */
            if (CliParaMDCCheck(MDC_CHRGTIP2_ADR, EVEN_MDC_BIT1)) {
                pItemCommonL->ItemDisc.fsPrintStatus |= PRT_DOUBLE_RECPT;  /* issue double receipt */
            }

            if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) != CURQUAL_PRE_UNBUFFER) {
                ItemPrevChargeTipTicketPrint(&(pItemCommonL->ItemDisc));
            }
            pItemCommonL->ItemDisc.fsPrintStatus &= ~(PRT_SINGLE_RECPT|PRT_DOUBLE_RECPT);  /* turn off ticket status */
        }
        /*----- MEMORY AFFECTION -----*/
        ItemPrevChargeTipAffection(&(pItemCommonL->ItemDisc));

        /* ---- clear auto charge tips ---- */
        pItemCommonL->ItemDisc.uchMajorClass = 0;
        /* memset(&(pItemCommonL->ItemDisc), 0, sizeof(ITEMDISC)); */
        pItemCommonL->usDiscBuffSize = 0;
    }
}

/*==========================================================================
**   Synopsis:  VOID    ItemTendEuroTender( UIFREGTENDER *UifRegTender,
*                           ITEMTENDER  *ItemTender )
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender,
*   Output:     ITEMTENDER  *ItemTender
*   InOut:      none
*
*   Return:
*
*   Description:    generate Euro trailer amount format, V3.4
==========================================================================*/

VOID    ItemTendEuroTender(CONST ITEMTENDER *pItemTender )
{
    /* print out Euro Trailer */
    if (CliParaMDCCheck(MDC_EURO_ADR, ODD_MDC_BIT0) && CliParaMDCCheck(MDC_EURO_ADR, ODD_MDC_BIT1)) {
		SHORT           sSplitStatus = TrnSplCheckSplit();
		TRANITEMIZERS   *WorkTI;
		DCURRENCY       lAmount;
		ITEMTENDER      ItemTenderEuro = *pItemTender;   // make a copy of the tender and make Euro conversions on the copy.
		PARACURRENCYTBL WorkRate = {0};
		PARAMDC         WorkMDC = {0};

        WorkRate.uchMajorClass = CLASS_PARACURRENCYTBL;
        WorkRate.uchAddress = CNV_NO1_ADR;                      /* Rate 1 */
        CliParaRead( &WorkRate );

        /*----- Split Check,   R3.1 -----*/
        if (sSplitStatus == TRN_SPL_SEAT
            || sSplitStatus == TRN_SPL_MULTI
            || sSplitStatus == TRN_SPL_TYPE2)
		{
            TrnGetTISpl(&WorkTI, TRN_TYPE_SPLITA);
        } else {
            TrnGetTI(&WorkTI);
        }
        lAmount = WorkTI->lMI;
        lAmount += ItemTransLocalPtr->lWorkMI;               /* whole tender amount */
        lAmount += ItemTenderLocalPtr->lPayment;             /* external VAT */

        WorkMDC.uchMajorClass = CLASS_PARAMDC;
        WorkMDC.usAddress = MDC_FC1_ADR;
        CliParaRead( &WorkMDC );                                        /* get target MDC */
        ItemTenderEuro.auchTendStatus[0] = WorkMDC.uchMDCData;          /* Foreign Currency tender MDC, print format, etc. */

        WorkMDC.uchMajorClass = CLASS_PARAMDC;
        WorkMDC.usAddress = MDC_EUROFC_ADR;
        CliParaRead( &WorkMDC );                                        /* get target MDC */

        ItemTenderEuro.auchTendStatus[1] = WorkMDC.uchMDCData;             /* tender MDC */
        ItemTenderEuro.auchTendStatus[1] >>= 4;                            /* shift nibble down since the MDC address is even numbered */

        ItemTendFCRateCalc1(ItemTenderEuro.auchTendStatus[1], &ItemTenderEuro.lForeignAmount, lAmount, WorkRate.ulForeignCurrency, RND_FOREIGN1_1_ADR );

        ItemTenderEuro.ulFCRate = WorkRate.ulForeignCurrency;              /* foreign currency rate */

        ItemTenderEuro.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;             /* print slip/journal */
        ItemTenderEuro.fbStorageStatus = 0;

        /*----- Split Check,   R3.1 -----*/
		sSplitStatus = TrnSplCheckSplit();
        if (sSplitStatus == TRN_SPL_SEAT
            || sSplitStatus == TRN_SPL_MULTI
            || sSplitStatus == TRN_SPL_TYPE2) {
            TrnItemSpl(&ItemTenderEuro, TRN_TYPE_SPLITA);
            /* MldScrollWrite2(MLD_DRIVE_2, &ItemTenderEuro, MLD_NEW_ITEMIZE); */
        } else {
            TrnItem(&ItemTenderEuro);
            /* MldScrollWrite(&ItemTenderEuro, MLD_NEW_ITEMIZE); */
            /* MldScrollFileWrite(&ItemTenderEuro); */
        }
    }
}

/*
*===========================================================================
** Format  : SHORT   ItemTendFCRateCalc1(LONG *plModAmount, LONG lAmount,
*                                                    ULONG ulRate, UCHAR uchType);
*
*   Input  : LONG   lAmount       -Source Data
*            ULONG  ulRate        -Rate(5 digits under decimal point)
*            UCHAR  uchType       -Rounding Type
*   Output : LONG   *plModAmount  -modifierd amount
*   InOut  : none
*
** Return  : RFL_SUCCESS          -function success
*            RFL_FAIL             -function fail
*
** Synopsis: This function multiply amount by rate
*===========================================================================
*/
SHORT   ItemTendFCRateCalc1(UCHAR uchStatus, DCURRENCY *plModAmount, DCURRENCY lAmount, ULONG ulRate, UCHAR uchType)
{
    if (uchStatus & ODD_MDC_BIT0) {
        /* 6 digit conversion */
        return (RflRateCalc3(plModAmount, lAmount, ulRate, uchType));
    } else {
        /* 5 digit conversion */
        return (RflRateCalc1(plModAmount, lAmount, ulRate, uchType));
    }
}

/*
*===========================================================================
** Format  : SHORT   ItemTendFCRateCalc2(LONG *plModAmount, LONG lAmount,
*                                                     ULONG ulRate, UCHAR uchType);
*
*   Input  : LONG   lAmount          -Source Data
*            ULONG  ulRate           -Rate(5 digits under decimal point)
*            UCHAR  uchType          -Rounding Type
*   Output : LONG   *plModAmount     -modifierd amount
*   InOut  : none
*
** Return  : RFL_SUCCESS             -function success
*            RFL_FAIL                -function fail
*
** Synopsis: This function divides lAmount by Rate.
*===========================================================================
*/
SHORT   ItemTendFCRateCalc2(UCHAR uchStatus, DCURRENCY *plModAmount, DCURRENCY lAmount, ULONG ulRate, UCHAR uchType)
{
    if (uchStatus & ODD_MDC_BIT0) {
        /* 6 decimal digits (99.999999) of precision conversion */
        return (RflRateCalc4(plModAmount, lAmount, ulRate, uchType));
    } else {
        /* 5 decimal digits (99.99999) of precision conversion */
        return (RflRateCalc2(plModAmount, lAmount, ulRate, uchType));
    }
}

/*
*===========================================================================
** Format  : SHORT   ItemTendFCRateCalcHuge1(UCHAR uchStatus, HUGEINT *phuModAmount,
*                              HUGEINT *phuAmount, ULONG ulRate, UCHAR uchType);
*
*   Input  : HUGEINT *phuAmount       -Source Data
*            ULONG  ulRate        -Rate(5 digits under decimal point)
*            UCHAR  uchType       -Rounding Type
*   Output : HUGEINT *phuModAmount  -modifierd amount
*   InOut  : none
*
** Return  : RFL_SUCCESS          -function success
*            RFL_FAIL             -function fail
*
** Synopsis: This function multiply amount by rate
*===========================================================================
*/
static SHORT   ItemTendFCRateCalcHuge1(UCHAR uchStatus, D13DIGITS *phuModAmount, D13DIGITS *phuAmount, ULONG ulRate, UCHAR uchType)
{
    if (uchStatus & ODD_MDC_BIT0) {
        /* 6 digit conversion */
        return (RflRateCalcHuge3(phuModAmount, phuAmount, ulRate, uchType));
    } else {
        /* 5 digit conversion */
        return (RflRateCalcHuge1(phuModAmount, phuAmount, ulRate, uchType));
    }
}

/*
*===========================================================================
** Format  : SHORT   ItemTendFCRateCalcHuge2(UCHAR uchStatus, HUGEINT *phuModAmount,
*                              HUGEINT *phuAmount, ULONG ulRate, UCHAR uchType);
*
*   Input  : HUGEINT *phuAmount          -Source Data
*            ULONG  ulRate           -Rate(5 digits under decimal point)
*            UCHAR  uchType          -Rounding Type
*   Output : HUGEINT *phuModAmount     -modifierd amount
*   InOut  : none
*
** Return  : RFL_SUCCESS             -function success
*            RFL_FAIL                -function fail
*
** Synopsis: This function divides lAmount by Rate.
*===========================================================================
*/
static SHORT   ItemTendFCRateCalcHuge2(UCHAR uchStatus, D13DIGITS *phuModAmount, D13DIGITS *phuAmount, ULONG ulRate, UCHAR uchType)
{
    if (uchStatus & ODD_MDC_BIT0) {
        /* 6 digit conversion */
        return (RflRateCalcHuge4(phuModAmount, phuAmount, ulRate, uchType));
    } else {
        /* 5 digit conversion */
        return (RflRateCalcHuge2(phuModAmount, phuAmount, ulRate, uchType));
    }
}

/*
*===========================================================================
** Format  : VOID   ItemTendFCRateCalcByEuroB()
*
**  Input  :
*   Output :
*   InOut  : none
*
**  Return :
*
** Synopsis: before transiton conversion
*===========================================================================
*/
VOID ItemTendFCRateCalcByEuroB(DCURRENCY *plForeign, DCURRENCY lMI, ULONG ulCurrency, UCHAR uchType, USHORT usConvType, UCHAR uchStatus)
{
    D13DIGITS           d13Work;
    D13DIGITS           d13ForeignEuro;
	PARACURRENCYTBL     WorkRateEuro = {0};
    LONG                lWorkRate;
    LONG                lWorkRateStep;
    UCHAR               uchStatusEuro;

    *plForeign = 0L;
    if (ulCurrency == 0L) return;

    if (CliParaMDCCheck(MDC_EURO_ADR, ODD_MDC_BIT2)) {
        /* after transition Euro as primary currency */
        if (usConvType == FC_EURO_TYPE1) {
            ItemTendFCRateCalc1(uchStatus, plForeign, lMI, ulCurrency, uchType);
        } else {
            ItemTendFCRateCalc2(uchStatus, plForeign, lMI, ulCurrency, uchType);
        }
        return;
    }

    /* convert Local to FC through Euro */
    /* step 0, get conversion rate from Euro */
    WorkRateEuro.uchMajorClass = CLASS_PARACURRENCYTBL;
    WorkRateEuro.uchAddress = CNV_NO1_ADR;
    CliParaRead( &WorkRateEuro );
    if (WorkRateEuro.ulForeignCurrency == 0L) return;

    if (CliParaMDCCheck(MDC_EUROFC_ADR, EVEN_MDC_BIT0)) {
        uchStatusEuro = ODD_MDC_BIT0;
    } else {
        uchStatusEuro = 0;
    }
/***
    if (uchStatus & ODD_MDC_BIT0) {
        RflMul32by32(&huWork, (LONG)WorkRateEuro.ulForeignCurrency, 10000000L);
    } else {
        RflMul32by32(&huWork, (LONG)WorkRateEuro.ulForeignCurrency, 1000000L);
    }
    RflDiv64by32(&huWork, &lRemain, (LONG)ulCurrency);
    RflRoundHuge(&lWorkRate, &huWork, RND_RND_NORMAL);
***/
    lWorkRate = (LONG)ulCurrency;

    /* step 1, calculate euro price */
    if (usConvType == FC_EURO_TYPE1) {
        lWorkRateStep = WorkRateEuro.ulForeignCurrency;  /* FC > Euro */
        d13Work = lMI * lWorkRateStep;
        if (uchStatusEuro & ODD_MDC_BIT0) {
            d13Work /= 10000L;      /* x 6 -1 digits */
        } else {
            d13Work /= 1000L;       /* x 5 -1 digits */
        }
    } else {
        lWorkRateStep = lWorkRate;     /* Local > Euro */
        d13Work = lMI * lWorkRateStep;
        if (uchStatus & ODD_MDC_BIT0) {
            d13Work /= 10000L;      /* x 6 -1 digits */
        } else {
            d13Work /= 1000L;       /* x 5 -1 digits */
        }
    }

    /* step 2, round to third decimal place */
    RflRoundHugeHuge(&d13ForeignEuro, &d13Work, RND_RND_NORMAL);

    /* step 3, calculate terget currency price */
    if (usConvType == FC_EURO_TYPE1) {
        lWorkRateStep = lWorkRate;  /* Euro > FC */
        ItemTendFCRateCalcHuge2(uchStatus, &d13Work, &d13ForeignEuro, lWorkRateStep, RND_RND_UP); /* round down */
    } else {
        lWorkRateStep = WorkRateEuro.ulForeignCurrency;  /* Euro > Local */
        ItemTendFCRateCalcHuge2(uchStatusEuro, &d13Work, &d13ForeignEuro, lWorkRateStep, RND_RND_UP); /* round down */
    }

    /* step 4, round to two decimal place */
    RflRoundDcurrency(plForeign, d13Work, uchType);
}


/*
*==========================================================================
**   Synopsis:  SHORT   ItemTendCheckWIC(UIFREGTENDER *pUifTender, ITEMTENDER *pItem)
*
*   Input:      UIFREGTENDER    *UifRegTender,  ITEMTENDER  *ItemTender
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    check WIC tender option                     Saratoga
*==========================================================================
*/
SHORT   ItemTendCheckWIC(UIFREGTENDER *pUifTender, ITEMTENDER *pItem)
{
	USHORT          usCheckFieldWic;

	usCheckFieldWic = 0;   // Tender 1 and Tender 2 both do not allow WIC so we hard code this to be off.
	if (pUifTender->uchMinorClass >= CLASS_UITENDER3 && pUifTender->uchMinorClass <= CLASS_UITENDER20) {
		usCheckFieldWic = ItemTenderCheckTenderMdc(pUifTender->uchMinorClass, MDC_TENDER_PAGE_5, MDC_PARAM_BIT_C);    // check if tender can be used for WIC
	}
 
    if (TranCurQualPtr->fsCurStatus & CURQUAL_WIC) {
		// This is a WIC transaction so we check to make sure that the tender is also a WIC tender
        if (! usCheckFieldWic) {
			NHPOS_NONASSERT_TEXT("==NOTE: LDT_ALTERNATE_TENDER_RQD WIC transaction, tender key not WIC. Prohibit by MDC.");
            return(LDT_ALTERNATE_TENDER_RQD);
        }
    } else {
		// This is not a WIC transaction so we check to make sure that the tender is also not a WIC tender
        if (usCheckFieldWic) {
			NHPOS_NONASSERT_TEXT("==NOTE: LDT_ALTERNATE_TENDER_RQD Not WIC transaction, tender key is WIC. Prohibit by MDC.");
            return(LDT_ALTERNATE_TENDER_RQD);
        }
    }

    return(ITM_SUCCESS);
}

/*
*==========================================================================
**   Synopsis:  VOID    ItemTendWICExempt(VOID)
*
*   Input:
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    check WIC tender option                     Saratoga
*==========================================================================
*/
VOID    ItemTendWICExempt(VOID)
{
    if (TranCurQualPtr->fsCurStatus & CURQUAL_WIC) {
		TRANGCFQUAL     *WorkGCF = TrnGetGCFQualPtr();
        WorkGCF->fsGCFStatus |= GCFQUAL_USEXEMPT;
    }
}

/*
*==========================================================================
**   Synopsis:  LONG    ItemTendWholeTransAmount(VOID)
*
*   Input:
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    check WIC tender option             DTREE   Dec/6/2000
*==========================================================================
*/
DCURRENCY  ItemTendWholeTransAmount(VOID)
{
    DCURRENCY       lWorkMI = 0;
    TRANITEMIZERS   *WorkTI;
    SHORT           sType;
	SHORT           sSplitStatus = TrnSplCheckSplit();

    if (ItemTenderLocal.fbTenderStatus[1] & TENDER_SPLIT_TENDER) {
	    return ItemTenderLocal.lSplitMI;
	}

    /*----- Split Check,   R3.1 -----*/
    if (sSplitStatus == TRN_SPL_SEAT
        || sSplitStatus == TRN_SPL_MULTI
        || sSplitStatus == TRN_SPL_TYPE2)
	{
        TrnGetTISpl(&WorkTI, TRN_TYPE_SPLITA);
        lWorkMI = 0;                                       /* no multi-check total */
		sType = ITM_SPLIT;
    } else {
        TrnGetTI(&WorkTI);
        lWorkMI = ItemTransLocalPtr->lWorkMI;              /* get multi-check total */
		sType = ITM_NOT_SPLIT;
    }

	/* V1.00.12 */
    if (ItemCommonTaxSystem() == ITM_TAX_INTL) {
	    if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_STORE_RECALL) {
	    	return(WorkTI->lMI + lWorkMI);
		} else {
#if 1
			return (ItemTenderLocal.ItemTotal.lMI + lWorkMI);
#else
    		ITEMCOMMONVAT   WorkVAT;
			memset(&WorkVAT, 0, sizeof(WorkVAT));
        	ItemCurVAT( &ItemTenderLocal.ItemTotal.auchTotalStatus[1], &WorkVAT, sType);
        	lWorkMI += (WorkVAT.lPayment + WorkTI->lMI);
        	lMI = lWorkMI;
            /* --- Netherland rounding, 2172 --- */
            if ( ! CliParaMDCCheck( MDC_ROUND1_ADR, ODD_MDC_BIT3 ) ) {
                if ( RflRound( &lAmount, lWorkMI, RND_TOTAL1_ADR ) == RFL_SUCCESS ) {
                    lMI = lAmount;
                }
            }
            return (lMI);
#endif
        }
	} else if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
	    if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_STORE_RECALL) {
	    	return(WorkTI->lMI + lWorkMI);
		} else {
#if 1
			return (ItemTenderLocal.ItemTotal.lMI + lWorkMI);
#else
		    ITEMCOMMONTAX   WorkTax;
		    memset(&WorkTax, 0, sizeof(WorkTax));
        	ItemCanTax( &ItemTenderLocal.ItemTotal.auchTotalStatus[1], &WorkTax, sType );
	    	for(i = 0; i < 4; i++) {
    	    	lMI += WorkTax.alTax[i];
			}
		    return(WorkTI->lMI + lMI + lWorkMI);
#endif
		}
	} else {
		DCURRENCY   lMI = 0;

		/* V1.00.12 */
	    if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_STORE_RECALL) {
        	lMI = ItemTenderLocal.lFSTenderizer - (ItemTenderLocal.lFSChange + ItemTenderLocal.lFS);	/* 07/26/01 */
	    	/* TrnGetTI(&WorkTI); */
		    return(WorkTI->lMI + lMI);
		} else {
		    ItemTendAddTaxSum(&lMI);
    		/* TrnGetTI(&WorkTI); */
	    	return(WorkTI->lMI + lMI + lWorkMI);
	    }
	}
}

/****** End of Source ******/