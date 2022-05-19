/*
*===================================================================================
* Title       : REG MODE MODIFIER MODULE - ItemMODIFIER Module                         
* Category    : REG MODE Application Program - NCR 2170 US Hospitality Application      
* Program Name: itmodif.c                
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
* ----------------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
* ----------------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name      : Description
* Feb-14-95: 03.00.00 :   hkato     : R3.0
* Nov-09-95: 03.01.00 :   hkato     : R3.1
* Jun-09-98: 03.03.00  : M.Ozawa   : change transaction open to common routine
* 
*** NCR2171 **
* 08/26/99 : 01.00.00  : M.Teraki  : initial (for Win32)
*===================================================================================
*===================================================================================
* PVCS Entry
* ----------------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===================================================================================
mhmh*/
/*
*===================================================================================
* INCLUDE FILES
* ----------------------------------------------------------------------------------
*/
#include	<tchar.h>
#include    <String.h>

#include    <ecr.h>
#include    "uie.h"
#include    "uireg.h"
#include    "regstrct.h"
#include    "Paraequ.h"
#include    "Para.h"
#include    "display.h"
#include    "mld.h"
#include    "rfl.h"
#include    "csstbpar.h"
#include    "csgcs.h"
/* #include    <csstbgcf.h> */
#include    "transact.h"
#include    "pif.h"
#include    "log.h"
#include    "appllog.h"
#include	"item.h"
#include    "cpm.h"
#include    "eept.h"
#include    "rfl.h"
#include    "itmlocal.h"
//SR 143 @/For cwunn
#include	"fsc.h"
#include	"..\..\Uifreg\uiregloc.h"
#include	"..\..\Sa\UIE\uieseq.h" //SR 143 @/FOR Cancel Mechanism cwunn
/*
*===================================================================================
* REFERNCE SYSTEM RAM
* ----------------------------------------------------------------------------------
*/

ITEMMODLOCAL ItemModLocal; /* MOdifier Local Data */

/*fhfh
*===========================================================================
** Synopsis:    VOID   ItemModGetLocal(UIFREGMODIFIER *ItemGetLocal) 
** Synopsis:    VOID   ItemModPutLocal(UIFREGMODIFIER *ItemPutLocal) 
*                                     
*     Input:    ItemGetLocal  -- pointer form UI
*               
*    Output:    nothing 
*
** Return:      nothing
*
** Description: ItemModGetLocal executes get Modifier local data and set it to pointer 
** Description: ItemModPutLocal executes writing data( pointed by ItemPutLocal)
*                to Modifier local data.
*===========================================================================
fhfh*/
CONST volatile ITEMMODLOCAL * CONST ItemModLocalPtr = &ItemModLocal;

ITEMMODLOCAL *ItemModGetLocalPtr(VOID)
{
    return &ItemModLocal;
}

ITEMMODSCALE *ItemModGetLocalScaleDataPtr(VOID)
{
	return &ItemModLocal.ScaleData;
}

VOID   ItemModGetLocal(ITEMMODLOCAL *pItemGetLocal)
{
    *pItemGetLocal = ItemModLocal;
}

VOID   ItemModPutLocal(ITEMMODLOCAL *pItemPutLocal)
{
    ItemModLocal = *pItemPutLocal;
}

/*fhfh
*===========================================================================
** Synopsis:    BOOL   ItemModCheckLocal(UCHAR fchBit)
*                                     
*     Input:    ItemPutLocal  -- pointer form UI
*               
*    Output:    nothing 
*
** Return:      nothing
*
** Description: ItemModPutLocal executes writing data( pointed by ItemPutLocal)
*                to Modifier local data.
*===========================================================================
fhfh*/
BOOL    ItemModCheckLocal(UCHAR fchBit)
{
    if (ItemModLocal.fbModifierStatus & fchBit) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/*fhfh
*===========================================================================
** Synopsis:    VOID   ItemModSetLocal(UCHAR fchBit)
*                                     
*     Input:    ItemPutLocal  -- pointer form UI
*               
*    Output:    nothing 
*
** Return:      nothing
*
** Description: ItemModPutLocal executes writing data( pointed by ItemPutLocal)
*                to Modifier local data.
*===========================================================================
fhfh*/
VOID   ItemModSetLocal(UCHAR fchBit)
{
    ItemModLocal.fbModifierStatus |= fchBit;
}


/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemModPvoid( VOID ) 
*                                     
*     Input:    nothing
*    Output:    nothing 
*
** Return:      LDT_SEQERR_ADR, ITM_SUCCESS
*
** Description: ItemModPvoid executes preselect void  
*===========================================================================
fhfh*/
SHORT   ItemModPvoid(UCHAR uchMinor )
{
    ITEMMODIFIER    TrnItemModifier = { 0 };
	REGDISPMSG  RegDispMsgD = { 0 };
    SHORT   sStatus;  

	sStatus = ItemCommonCheckCashDrawerOpen ();
	if (sStatus != 0)
		return sStatus;

    /* check fsTaxble=0 or not */
    if (ItemModLocal.fsTaxable != 0) {
        return(LDT_SEQERR_ADR);
    }
    if (ItemModLocal.fbModifierStatus & MOD_FSMOD) {    /* Saratoga */
        return(LDT_SEQERR_ADR); 
    }

    TrnItemModifier.uchMajorClass = CLASS_ITEMMODIFIER;
    RegDispMsgD.uchMajorClass = CLASS_UIFREGMODIFIER;

	/* set major/minor class*/
	if (uchMinor == 0) {
		// check to see if we are already in a Transaction Return of some kind
		if (TranCurQualPtr->fsCurStatus & (CURQUAL_PRETURN | CURQUAL_TRETURN)) {  // status check in ItemModPvoid()
			// not allow Transaction Void if Transaction Return started
			return(LDT_SEQERR_ADR); 
		}
		TrnItemModifier.uchMinorClass = CLASS_PVOID;
		/* check Supervisor intervention cancel or success */
		if ( CliParaMDCCheckField( MDC_PSELVD_ADR, MDC_PARAM_BIT_D) != 0 && ItemCommonGetStatus( COMMON_SPVINT ) != ITM_SUCCESS) {
			if ( (sStatus = ItemSPVInt(LDT_SUPINTR_ADR) ) != ITM_SUCCESS ) {
				return( sStatus );
			}
		}
		/* set and display mnemonic p-void */
		RegDispMsgD.uchMinorClass = CLASS_UIPVOID;
		if (TranCurQualPtr->fsCurStatus & CURQUAL_PVOID) {
			flDispRegDescrControl &= ~ TVOID_CNTRL ; /* CURQUAL_PVOID on->TVOID_CNTRL off */
			flDispRegKeepControl &= ~ TVOID_CNTRL ; /* CURQUAL_PVOID on->TVOID_CNTRL off */
			ItemCommonResetStatus (COMMON_SPVINT);    // in case of Supvisor intervention, turn off indicator it has been done
		} else {
			/* display preselect void */
			/* set and display mnemonic p-void */
			RflGetTranMnem ( RegDispMsgD.aszMnemonic, TRN_PSELVD_ADR);

			flDispRegDescrControl |=  TVOID_CNTRL ; /* CURQUAL_PVOID off->TVOID_CNTRL on */
			flDispRegKeepControl |=  TVOID_CNTRL ; /* CURQUAL_PVOID off->TVOID_CNTRL on */
		}
	} else if (uchMinor >= 1 && uchMinor <= 3) {
		// check to see if we are already in a Transaction Void
		if (TranCurQualPtr->fsCurStatus & CURQUAL_PVOID) {  // status check in ItemModPvoid()
			// not allow Transaction Return if Transaction Void started
			return(LDT_SEQERR_ADR); 
		}
		TrnItemModifier.uchMinorClass = CLASS_PRETURN;
		TrnItemModifier.usTableNo = uchMinor;            // indicate the type of return that is the default

		// Check to see if this return type has been selected for Supervisor Intervention.  If so then we will
		// do the Supervisor Intervention step before continuing with processing the return
		if (uchMinor == 1) {
			if (CliParaMDCCheckField(MDC_RETURNS_RTN1_ADR, MDC_PARAM_BIT_D) && ItemCommonGetStatus( COMMON_SPVINT ) != ITM_SUCCESS) {
				if ((sStatus = ItemSPVIntAlways(LDT_SUPINTR_ADR)) != ITM_SUCCESS) {  // Allow for Supervisor Intervention if MDC blocks Return type
					return(sStatus);
				}
			}
		}
		else if (uchMinor == 2) {
			if (CliParaMDCCheckField(MDC_RETURNS_RTN2_ADR, MDC_PARAM_BIT_D) && ItemCommonGetStatus( COMMON_SPVINT ) != ITM_SUCCESS) {
				if ((sStatus = ItemSPVIntAlways(LDT_SUPINTR_ADR)) != ITM_SUCCESS) {  // Allow for Supervisor Intervention if MDC blocks Return type
					return(sStatus);
				}
			}
		}
		else if (uchMinor == 3) {
			if (CliParaMDCCheckField(MDC_RETURNS_RTN3_ADR, MDC_PARAM_BIT_D) && ItemCommonGetStatus( COMMON_SPVINT ) != ITM_SUCCESS) {
				if ((sStatus = ItemSPVIntAlways(LDT_SUPINTR_ADR)) != ITM_SUCCESS) {  // Allow for Supervisor Intervention if MDC blocks Return type
					return(sStatus);
				}
			}
		}

		/* set and display mnemonic p-void */
		RegDispMsgD.uchMinorClass = CLASS_UIFTRETURN;
		if (TranCurQualPtr->fsCurStatus & CURQUAL_PRETURN) {
			flDispRegDescrControl &= ~ TVOID_CNTRL ; /* CURQUAL_PVOID on->TVOID_CNTRL off */
			flDispRegKeepControl &= ~ TVOID_CNTRL ; /* CURQUAL_PVOID on->TVOID_CNTRL off */
			ItemCommonResetStatus (COMMON_SPVINT);    // in case of Supvisor intervention, turn off indicator it has been done
		} else {
			/* display preselect void */
			/* set and display mnemonic p-return */
			RflGetTranMnem ( RegDispMsgD.aszMnemonic, TRN_PRETURN1_ADR + (uchMinor - 1));

			flDispRegDescrControl |=  TVOID_CNTRL ; /* CURQUAL_PVOID off->TVOID_CNTRL on */
			flDispRegKeepControl |=  TVOID_CNTRL ; /* CURQUAL_PVOID off->TVOID_CNTRL on */

			{
				PARAREASONCODEINFO  DataReasonCode = { 0 };
				SHORT  sItemNumber = 4, sRetStatus = 0;
				CONST TCHAR  *pLeadThru = _T("Reason");
				TCHAR  *pMnemonics[50];

				DataReasonCode.uchAddress = uchMinor + 3;            // first three slots for Cursor Return, second three slots for Preselect Transaction Return
				ParaReasonCodeInfoParaRead (&DataReasonCode);

				if (DataReasonCode.uchAddress > 0 && DataReasonCode.usMnemonicStartAddr > 0 && DataReasonCode.usMnemonicCount > 0)
				{
					USHORT  usNonemptyCount = 0;
					USHORT  usI;
					TCHAR   *pTemp;
					OPMNEMONICFILE MnemonicFile;
					TCHAR  Mnemonics[50 * 32] = { 0 };

					MnemonicFile.usMnemonicFileId = OPMNEMONICFILE_ID_REASON;
					MnemonicFile.usMnemonicAddress = DataReasonCode.usMnemonicStartAddr;
					pTemp = Mnemonics;
					for (usI = 0; usI < DataReasonCode.usMnemonicCount; usI++) {
						OpMnemonicsFileIndRead(&MnemonicFile, 0);
						_tcsncpy (pTemp, MnemonicFile.aszMnemonicValue, 30);
						pMnemonics[usI] = pTemp;
						if (pTemp[0]) usNonemptyCount++;
						pTemp += 32;
						MnemonicFile.usMnemonicAddress++;
					}

					TrnItemModifier.usNoPerson = 0;
					if (usNonemptyCount) {
						sRetStatus = ItemDisplayOepChooseDialog (pLeadThru, pMnemonics, DataReasonCode.usMnemonicCount);
						if (sRetStatus > 0) {
							TrnItemModifier.usNoPerson = (sRetStatus - 1) + DataReasonCode.usMnemonicStartAddr;
						} else {
							return(LDT_KEYOVER_ADR);
						}
					}
//				} else {
//					NHPOS_ASSERT_TEXT(0, "ParaReacodeCodeInfo is not set for this reason code type.");
				}
			}
		}
	} else {
		NHPOS_ASSERT_TEXT((uchMinor < 2), "FSC_PRE_VOID: Invalid minor code.");
        return(LDT_PROHBT_ADR);
	}

    DispWrite( &RegDispMsgD );

    TrnItem( &TrnItemModifier ); 

    return(ITM_SUCCESS);
}


/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemModRec(VOID) 
*                                     
*     Input:    nothing
*    Output:    nothing 
*
** Return:      LDT_SEQERR_ADR, ITM_SUCCESS
*
** Description: ItemModRec executes no receipt 
*===========================================================================
fhfh*/

SHORT   ItemModRec(VOID)
{

    /* check fsTaxble=0 or not */
//    if (ItemModLocal.fsTaxable != 0 ){
//        return(LDT_SEQERR_ADR);
//    }
//    if (ItemModLocal.fbModifierStatus & MOD_FSMOD) {    /* Saratoga */
//        return(LDT_SEQERR_ADR); 
//    }

    ItemModLocal.fbModifierStatus ^= MOD_NORECEIPT;

    if (ItemModLocal.fbModifierStatus & MOD_NORECEIPT) {
        /*  receipt print(status quo),  bit on --> executes descripter on */
        flDispRegDescrControl |= RECEIPT_CNTRL; 
        flDispRegKeepControl |= RECEIPT_CNTRL;  
    } else {
        /*  receipt print(status quo),  bit off --> executes descripter off */
        flDispRegDescrControl &= ~ RECEIPT_CNTRL;
        flDispRegKeepControl &= ~ RECEIPT_CNTRL;
    }

	RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGMODIFIER, CLASS_UINORECEIPT, DISP_SAVE_TYPE_0);

	return(ITM_SUCCESS);
}

/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemModMenu(UIFREGMODIFIER *ItemModifier) 
*                                     
*     Input:    ItemModifier  -- pointer form UI
*    Output:    nothing 
*
** Return:      LDT_PROHBT_ADR, ITM_SUCCESS
*
** Description: ItemModMenu executes setting "menu sift"
*===========================================================================
fhfh*/

SHORT   ItemModMenu(CONST UIFREGMODIFIER *ItemModifier )
{
    UCHAR   auchCntTableMenuSave[MAX_PAGE_NO];
    USHORT  usCntTableMenuLen;
    SHORT   sStatus = ITM_SUCCESS; 
    ITEMSALESLOCAL  * const pWorkSales = ItemSalesGetLocalPointer();
	PARACTLTBLMENU  MenuRcvBuff = { 0 };

    /* correction10/13 */
    /* check pressing total key or not    R3.0 */
    if ( (  sStatus =  ItemCommonCheckExempt() ) != ITM_SUCCESS ) {
        return(sStatus);
    }

    if (ItemModLocal.fbModifierStatus & MOD_FSMOD) {    /* Saratoga */
        return(LDT_SEQERR_ADR); 
    }

    /* check No,  R3.1 */
    if (ItemModifier->uchMenuPage > MAX_PAGE_NO ) {
        return( LDT_KEYOVER_ADR );
    }
    
    /* GET STATUS,   R3.1 */
    CliParaAllRead(CLASS_PARACTLTBLMENU, &auchCntTableMenuSave[0], MAX_PAGE_NO, 1, &usCntTableMenuLen);

    if ( ItemModifier->uchMenuPage != 0) {
        if ( auchCntTableMenuSave[ ItemModifier->uchMenuPage - 1] != 0 ) {
            return(LDT_PROHBT_ADR);
        } 
        uchDispCurrMenuPage = ItemModifier->uchMenuPage;
    } else {
        uchDispCurrMenuPage = ItemSalesLocalPtr->uchMenuShift; /* get default menu no. */
    }

    /* display */
	RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGMODIFIER, CLASS_UIMENUSHIFT, DISP_SAVE_TYPE_0);

    ItemCommonShiftPage(uchDispCurrMenuPage);          /* R3.0 */

    /*----- check off line tender status 4/16/96 -----*/
    if ( ItemModLocal.fbModifierStatus & MOD_OFFLINE ) {    
        ItemModOffTend();
    }

    /* default adjective by shift page, 2172 */
    MenuRcvBuff.uchMajorClass = CLASS_PARACTLTBLMENU;
    MenuRcvBuff.uchAddress = (UCHAR) (uchDispCurrMenuPage + CTLPG_ADJ_PAGE1_ADR - 1);
    CliParaRead(&MenuRcvBuff);

    /* WorkSales.uchAdjDefShift = MenuRcvBuff.uchPageNumber; */
    if (pWorkSales->uchAdjKeyShift == 0) {                         /* adj. shift key not specified so use default */
		pWorkSales->uchAdjCurShift = MenuRcvBuff.uchPageNumber;    /* set default adjective, 2172 */
    }
    
    return( ITM_SUCCESS );
}

/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemModTable(UIFREGMODIFIER *ItemModifier) 
*                                     
*     Input:    ItemModifier  -- pointer form UI
*               
*    Output:    nothing 
*
** Return:      LDT_SEQERR_ADR, ITM_SUCCESS
*
** Description: ItemModTable executes to control table no.  
*===========================================================================
fhfh*/

SHORT   ItemModTable(UIFREGMODIFIER *ItemModifier) 
{
	ITEMMODIFIER   TrnItemModifier = { 0 };
	REGDISPMSG     RegDispMsgD = { 0 };
    SHORT   sStatus;

    /* check pressing total key or not    R3.0 */
    if ( (  sStatus =  ItemCommonCheckExempt() ) != ITM_SUCCESS ) {
        return(sStatus);
    }
    if (ItemModLocal.fbModifierStatus & MOD_FSMOD) {    /* Saratoga */
        return(LDT_SEQERR_ADR); 
    }

    if ( (  sStatus = ItemCommonCheckComp() ) != ITM_SUCCESS ) {
        return(sStatus);
    }

    if ((sStatus = ItemCommonCheckSplit()) != ITM_SUCCESS) {   /* R3.1 */
        return(sStatus);
    }
        
    /* set major/minor class*/
    TrnItemModifier.uchMajorClass = CLASS_ITEMMODIFIER;
    TrnItemModifier.uchMinorClass = CLASS_TABLENO;

    /* set table no.   from ui */
    TrnItemModifier.usTableNo = ItemModifier->usTableNo;

    /* set print status */
    TrnItemModifier.fsPrintStatus = (PRT_JOURNAL | PRT_SLIP);
    
    /* send data to transaction module */
    if ( (sStatus = ItemPreviousItem( &TrnItemModifier, 0) ) != ITM_SUCCESS ) {
        return(sStatus);
    }

    /* send to enhanced kds, 2172 */
    sStatus = ItemSendKds(&TrnItemModifier, 0);
    if (sStatus != ITM_SUCCESS) {
            
        return sStatus;
    }

    /* display mmnemonics */
    RegDispMsgD.uchMajorClass = CLASS_UIFREGMODIFIER;
    RegDispMsgD.uchMinorClass = CLASS_UITABLENO;
    RegDispMsgD.lAmount = ItemModifier->usTableNo;
	RflGetSpecMnem( RegDispMsgD.aszMnemonic, SPC_TBL_ADR );

    flDispRegDescrControl &= ~ TAXEXMPT_CNTRL;  /* correction10/13 */
    flDispRegKeepControl &= ~ TAXEXMPT_CNTRL;

    DispWrite( &RegDispMsgD );

    /*----- Display Table# to LCD, R3.0 -----*/
    MldScrollWrite(&TrnItemModifier, MLD_NEW_ITEMIZE);

    ItemCommonPutStatus(COMMON_VOID_EC); /* set prohibit e/c bit */
	ItemCommonPutStatus (COMMON_PROHIBIT_SIGNOUT);

    /* correction10/08 */
    /* after pressing total, bit off curqual */
    ItemCommonCancelExempt();

    return( ITM_SUCCESS );

}

/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemModPerson(UIFREGMODIFIER *ItemModifier) 
*                                     
*     Input:    ItemModifier  -- pointer form UI
*               
*    Output:    nothing 
*
** Return:      LDT_SEQERR_ADR, ITM_SUCCESS
*
** Description: ItemModPerson executes to control no. of person. 
*===========================================================================
fhfh*/

SHORT   ItemModPerson(UIFREGMODIFIER *ItemModifier) 
{
    SHORT         sStatus;
    ITEMMODIFIER  TrnItemModifier = { 0 };
	REGDISPMSG    RegDispMsgD = { 0 };
    TRANGCFQUAL   * CONST pWorkGCF = TrnGetGCFQualPtr();

    /*----- check non-gc transaction V3.3 -----*/
    if ( (  sStatus =  ItemCommonCheckNonGCTrans() ) != ITM_SUCCESS ) {
        return(sStatus);
    }

    /* check pressing total key or not    R3.0 */
    if ( (  sStatus =  ItemCommonCheckExempt() ) != ITM_SUCCESS ) {
        return(sStatus);
    }
    if (ItemModLocal.fbModifierStatus & MOD_FSMOD) {    /* Saratoga */
        return(LDT_SEQERR_ADR); 
    }

    if ( ( sStatus = ItemCommonCheckComp() ) != ITM_SUCCESS ) {
        return(sStatus);
    }

    if ((sStatus = ItemCommonCheckSplit()) != ITM_SUCCESS) {   /* R3.1 */
        return(sStatus);
    }

    /*--- Check Split State,    R3.1 ---*/
    if (TrnSplCheckSplit() == TRN_SPL_SEAT
        || TrnSplCheckSplit() == TRN_SPL_MULTI) {
        return(LDT_SEQERR_ADR);
    }

    /* --- Saratoga,    Nov/27/2000 --- */
    if (ItemModifier->fbInputStatus & INPUT_0_ONLY) {
        ItemModLocal.fbModifierStatus |= MOD_ZEROPERSON;
        pWorkGCF->usNoPersonMax  = 0;
        pWorkGCF->usNoPerson     = 0;
    }

    if ( ! (TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN) ) ) {   /* normal transaction */
        if ( (TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_NEWCHECK || (TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_CASHIER ) {
            ItemModLocal.usCurrentPerson = ItemModifier->usNoPerson;    /* affection amount */
        } else {                                                        /* reorder, addcheck */  
            if ( pWorkGCF->usNoPersonMax < ItemModifier->usNoPerson ) {
                ItemModLocal.usCurrentPerson += ItemModifier->usNoPerson - pWorkGCF->usNoPersonMax;   /* affection amount */
                pWorkGCF->usNoPersonMax = ItemModifier->usNoPerson;
            }
        }
     } else {                                                           /* p-void transaction */
        if ( (TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_NEWCHECK || (TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_CASHIER ) {
            ItemModLocal.usCurrentPerson = ItemModifier->usNoPerson;    /* affection amount */
        } else {                                                        /* reorder, addcheck */  
            ItemModLocal.usCurrentPerson = 0;                           /* affection amount */
        }
    }

    /* set major/minor class*/
    TrnItemModifier.uchMajorClass = CLASS_ITEMMODIFIER;
    TrnItemModifier.uchMinorClass = CLASS_NOPERSON;

    /* set No. of person from ui */
    TrnItemModifier.usNoPerson = ItemModifier->usNoPerson;

    /* set print status */
    TrnItemModifier.fsPrintStatus = (PRT_JOURNAL | PRT_SLIP);
                                            
    /* send data to transaction module */           
    if ( (sStatus = ItemPreviousItem( &TrnItemModifier, 0) ) != ITM_SUCCESS ) {
        return(sStatus);
    }

    /* read mnemonics */
    /* display mmnemonics */
    RflGetTranMnem ( RegDispMsgD.aszMnemonic, TRN_PSN_ADR );
 
	RegDispMsgD.uchMajorClass = CLASS_UIFREGMODIFIER;
    RegDispMsgD.uchMinorClass = CLASS_UINOPERSON;
    RegDispMsgD.lAmount = ItemModifier->usNoPerson;
    flDispRegDescrControl &= ~TAXEXMPT_CNTRL;  /* correction10/13 */
    flDispRegKeepControl &= ~TAXEXMPT_CNTRL;
    DispWrite( &RegDispMsgD );
    
    ItemCommonPutStatus(COMMON_VOID_EC); /* set prohibit e/c bit */

    /* after pressing total, bit off curqual */
    ItemCommonCancelExempt();
              
	sStatus = ItemCommonSetupTransEnviron ();
	if(sStatus != ITM_SUCCESS)
		return (sStatus);

	/* send to enhanced kds, 2172 */
    sStatus = ItemSendKds(&TrnItemModifier, 0);
    if (sStatus != ITM_SUCCESS) {
            
        return sStatus;
    }

    /*----- Display # Person to LCD, R3.0 -----*/
    MldScrollWrite(&TrnItemModifier, MLD_NEW_ITEMIZE);

    return( ITM_SUCCESS );
}

/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemModTax(UIFREGMODIFIER *ItemModifier) 
*                                     
*     Input:    ItemModifier  -- pointer form UI
*               
*    Output:    nothing 
*
** Return:      LDT_SEQERR_ADR, ITM_SUCCESS
*
** Description: Taxable Modifier Key (US/Canada/Int'l),         V3.3
*===========================================================================
fhfh*/
SHORT   ItemModTax(UIFREGMODIFIER *ItemModifier)
{
    if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
        ItemModTaxCanada( ItemModifier ); /* execute canadian tax sys. */
        return(ITM_SUCCESS);

    } else if (ItemCommonTaxSystem() == ITM_TAX_US) {
        return( ItemModTaxUS( ItemModifier ) ); /* execute us */

    } else {
		NHPOS_ASSERT_TEXT(!"==PROVISIONING", "==PROVISIONING: Tax modifier key not allowed with Int'l VAT Tax System.");
        return(LDT_SEQERR_ADR);
    }
}
    
/*
*===========================================================================
** Synopsis:    SHORT   ItemModTaxUS(UIFREGMODIFIER *ItemModifier) 
*                                     
*     Input:    ItemModifier  -- pointer form UI
*               
*    Output:    nothing 
*
** Return:      LDT_SEQERR_ADR, ITM_SUCCESS
*
** Description: ItemModTaxUS executes "us tax sys."  
*===========================================================================
*/
SHORT    ItemModTaxUS(CONST UIFREGMODIFIER *ItemModifier)
{
    SHORT           sStatus;
	REGDISPMSG      RegDispMsgD = {0};
	USHORT          usSpecMnemonicAdrs = 0;

    /* initialize */
    if ( (ItemModifier->uchMinorClass >= CLASS_UITAXMODIFIER4) &&
         (ItemModifier->uchMinorClass != CLASS_UITAXEXEMPT1) ) {
        return(LDT_SEQERR_ADR);
    }

    /* check SPV */                                                 
    if ( ItemModifier->uchMinorClass == CLASS_UITAXEXEMPT1 ) {      /* tax exempt */
        if ( CliParaMDCCheck( MDC_TAXEXM_ADR, EVEN_MDC_BIT0) != 0 ) {
            if ( (sStatus = ItemSPVInt(LDT_SUPINTR_ADR) ) != ITM_SUCCESS ) {
                return( sStatus );
            }
        }
        usSpecMnemonicAdrs = SPC_TXMOD10_ADR;
        ItemModLocal.fsTaxable |= MOD_USEXEMPT;

        flDispRegDescrControl |= TAXEXMPT_CNTRL;
        flDispRegKeepControl |= TAXEXMPT_CNTRL;

    } else {
        switch ( ItemModifier->uchMinorClass ) {
        case CLASS_UITAXMODIFIER1:
            usSpecMnemonicAdrs = SPC_TXMOD1_ADR;
            ItemModLocal.fsTaxable |= MOD_TAXABLE1;
            break;

        case CLASS_UITAXMODIFIER2:
            usSpecMnemonicAdrs = SPC_TXMOD2_ADR;
            ItemModLocal.fsTaxable |= MOD_TAXABLE2;
            break;

        case CLASS_UITAXMODIFIER3:
            usSpecMnemonicAdrs = SPC_TXMOD3_ADR;
            ItemModLocal.fsTaxable |= MOD_TAXABLE3;
            break;

        default:
			return(LDT_SEQERR_ADR);
        }

        /* set descriptor flag( regcontrol and keep flag) */
        flDispRegDescrControl |= TAXMOD_CNTRL;
        flDispRegKeepControl |= TAXMOD_CNTRL;
    }

    /* read mnemonics */
    /* display mmnemonics */
    RegDispMsgD.uchMajorClass = CLASS_UIFREGMODIFIER;
    RegDispMsgD.uchMinorClass = ItemModifier->uchMinorClass;
    RflGetSpecMnem( RegDispMsgD.aszMnemonic, usSpecMnemonicAdrs );
    _tcsncpy( RegDispMsgD.aszStringData, ItemModifier->aszNumber, STD_NUMBER_LEN);
    RegDispMsgD.fbSaveControl = DISP_SAVE_TYPE_4; /* set type 4 */
    
    DispWrite( &RegDispMsgD );
    return( ITM_SUCCESS );
}

/*fhfh
*===========================================================================
** Synopsis:    SHORT    ItemModTaxCanada(UIFREGMODIFIER *ItemModifier) 
*                                     
*     Input:    ItemModifier  -- pointer form UI
*               
*    Output:    nothing 
*
** Return:      LDT_SEQERR_ADR, ITM_SUCCESS
*
** Description: ItemModTaxCanada executes "canadian tax sys." 
*===========================================================================
fhfh*/
SHORT    ItemModTaxCanada(CONST UIFREGMODIFIER *ItemModifier)
{
	REGDISPMSG      RegDispMsgD = {0};
    SHORT           sStatus;
	USHORT          usSpecMnemonicAdrs = 0;

    /* check SPV */
    if ( ItemModifier->uchMinorClass == CLASS_UITAXEXEMPT1          /* tax exempt */
        || ItemModifier->uchMinorClass == CLASS_UITAXEXEMPT2 ) {

        if ( CliParaMDCCheck( MDC_TAXEXM_ADR, EVEN_MDC_BIT0) != 0 ) {
            if ( (sStatus = ItemSPVInt(LDT_SUPINTR_ADR) ) != ITM_SUCCESS ) {
                return( sStatus );
            }
        }

        switch ( ItemModifier->uchMinorClass ) {
        case CLASS_UITAXEXEMPT1:
			usSpecMnemonicAdrs = SPC_TXMOD10_ADR;
            ItemModLocal.fsTaxable |= MOD_GSTEXEMPT;
            break;

        default:
			usSpecMnemonicAdrs = SPC_TXMOD11_ADR;
            ItemModLocal.fsTaxable |= MOD_PSTEXEMPT;
            break;
        }
        flDispRegDescrControl |= TAXEXMPT_CNTRL;
        flDispRegKeepControl |= TAXEXMPT_CNTRL;

    } else {                                                        /* taxable modifier */
        switch ( ItemModifier->uchMinorClass ) {
        case CLASS_UITAXMODIFIER1:
			usSpecMnemonicAdrs = SPC_TXMOD1_ADR;
            ItemModLocal.fsTaxable = MOD_TAXABLE1;
            break;

        case CLASS_UITAXMODIFIER2:
			usSpecMnemonicAdrs = SPC_TXMOD2_ADR;
            ItemModLocal.fsTaxable = MOD_TAXABLE2;
            break;

        case CLASS_UITAXMODIFIER3:
			usSpecMnemonicAdrs = SPC_TXMOD3_ADR;
            ItemModLocal.fsTaxable = MOD_TAXABLE3;
            break;

        case CLASS_UITAXMODIFIER4:
			usSpecMnemonicAdrs = SPC_TXMOD4_ADR;
            ItemModLocal.fsTaxable = MOD_TAXABLE4;
            break;
            
        case CLASS_UITAXMODIFIER5:
			usSpecMnemonicAdrs = SPC_TXMOD5_ADR;
            ItemModLocal.fsTaxable = MOD_TAXABLE5;
            break;
    
        case CLASS_UITAXMODIFIER6:
			usSpecMnemonicAdrs = SPC_TXMOD6_ADR;
            ItemModLocal.fsTaxable = MOD_TAXABLE6;
            break;
            
        case CLASS_UITAXMODIFIER7:
			usSpecMnemonicAdrs = SPC_TXMOD7_ADR;
            ItemModLocal.fsTaxable = MOD_TAXABLE7;
            break;
        
        case CLASS_UITAXMODIFIER8:
			usSpecMnemonicAdrs = SPC_TXMOD8_ADR;
            ItemModLocal.fsTaxable = MOD_TAXABLE8;
            break;

        default:
			usSpecMnemonicAdrs = SPC_TXMOD9_ADR;
            ItemModLocal.fsTaxable = MOD_TAXABLE9;
            break;
        }

        flDispRegDescrControl |= TAXMOD_CNTRL;
        flDispRegKeepControl |= TAXMOD_CNTRL;
    }
    
    /* display mmnemonics */
    RegDispMsgD.uchMajorClass = CLASS_UIFREGMODIFIER;
    RegDispMsgD.uchMinorClass = ItemModifier->uchMinorClass;
	RflGetSpecMnem( RegDispMsgD.aszMnemonic, usSpecMnemonicAdrs);
    _tcsncpy( RegDispMsgD.aszStringData, ItemModifier->aszNumber, STD_NUMBER_LEN);
    RegDispMsgD.fbSaveControl = DISP_SAVE_TYPE_4; /* set type4 */
    
    DispWrite( &RegDispMsgD );

    return( ITM_SUCCESS );
}

/*
*===========================================================================
** Synopsis:    SHORT   ItemModOffTend( VOID ) 
*                                     
*     Input:    ItemModifier  -- pointer form UI
*               
*    Output:    nothing 
*
** Return:      LDT_SEQERR_ADR, ITM_SUCCESS
*
** Description: Display Off Line Tender Modifier  
*===========================================================================
*/
SHORT    ItemModOffTend(VOID)
{
	REGDISPMSG      RegDispMsgD = {0};

    ItemModLocal.fbModifierStatus |= MOD_OFFLINE;       /* E-003 corr. 4/18 */    

    /* display mmnemonics */
    RegDispMsgD.uchMajorClass = CLASS_UIFREGMODIFIER;
    RegDispMsgD.uchMinorClass = CLASS_UIOFFTENDER;
    RflGetSpecMnem(RegDispMsgD.aszMnemonic, SPC_OFFTEND_ADR);
    RegDispMsgD.fbSaveControl = DISP_SAVE_TYPE_4; /* set type 4 */
    DispWrite(&RegDispMsgD);
                                    
    return(ITM_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT   ItemModTare(UIFREGMODIFIER *pUifRegModifier) 
*                                     
*     Input:    UIFREGMODIFIER *pUifRegModifier
*               
*    Output:    nothing 
*
** Return:      LDT_SCALEERR_ADR : Illegal weight is read from auto scale.
*
** Description: Scale/tare key process. 
*===========================================================================
*/
SHORT   ItemModTare(UIFREGMODIFIER *pUifRegModifier) 
{
    UCHAR           uchWork;
    SHORT           sReturnStatus;

    memset(&ItemModLocal.ScaleData, 0, sizeof(ITEMMODSCALE));

    /*--- CHECK OPERATION SEQUENCE ---*/
    sReturnStatus = ItemModTareCheckSeq(pUifRegModifier);
    if (sReturnStatus != ITM_SUCCESS) {
        return(sReturnStatus);
    }

    /*--- CHECK COMPULSORY OPERATION  ---*/
    if (ItemSalesLocalPtr->fbSalesStatus & SALES_BOTH_COMPLSRY) {
        return(LDT_PMODCNDI_ADR);           /* return compulsory error */
    }

    ItemModLocal.ScaleData.lTareValue = 0;
    if (pUifRegModifier->lAmount != 0) {    /* tare code entered */
		PARATARE      TareTableBuff = {0};

        /*--- GET TARE WEIGHT ---*/
        TareTableBuff.uchMajorClass = CLASS_PARATARE;
        TareTableBuff.uchAddress = (UCHAR) (pUifRegModifier->lAmount);

        CliParaRead(&TareTableBuff);

        ItemModLocal.ScaleData.lTareValue = TareTableBuff.usWeight;    /* set tare weight */
    }

    /*--- GET MDC FOR SCALE ---*/
    /* get MDC address 29 to determine flags for MOD_READ_SCALE_AT_TARE, etc. */
    uchWork = CliParaMDCCheck(MDC_SCALE_ADR, (ODD_MDC_BIT0 | ODD_MDC_BIT1 | ODD_MDC_BIT2 | ODD_MDC_BIT3));
    uchWork &= 0x0f;                              /* mask upper 4 bits */

    ItemModLocal.ScaleData.fbTareStatus = uchWork;    /* set MDC address 29 for MOD_READ_SCALE_AT_TARE, etc. */

    if (!(ItemModLocal.ScaleData.fbTareStatus & MOD_SCALE_DIV_005_OR_001)) {
        ItemModLocal.ScaleData.lTareValue *= 10L; /* adjust tare weight if MOD_SCALE_DIV_005_OR_001 indicated by MDC_SCALE_ADR */
    }

    sReturnStatus = ItemCommonCheckScaleDiv(&ItemModLocal.ScaleData, ItemModLocal.ScaleData.lTareValue);    /* check tare weight */
    if (sReturnStatus != ITM_SUCCESS) {
        return(sReturnStatus);
    }

    if (ItemModLocal.ScaleData.fbTareStatus & MOD_READ_SCALE_AT_TARE) {
        sReturnStatus = ItemCommonGetScaleData(&ItemModLocal.ScaleData);
        if (sReturnStatus != ITM_SUCCESS) {
            return(sReturnStatus);
        }
    }

    ItemModLocal.ScaleData.fbTareStatus |= MOD_TARE_KEY_DEPRESSED;

    ItemModTareDispMessage(pUifRegModifier);

    ItemCommonPutStatus(COMMON_VOID_EC); /* set prohibit e/c bit */

    /* after pressing total, bit off curqual */
    ItemCommonCancelExempt();
    
    return(ITM_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT   ItemModTareCheckSeq(UIFREGMODIFIER *pUifRegModifier) 
*                                     
*     Input:    UIFREGMODIFIER *pUifRegModifier
*               
*    Output:    nothing 
*
** Return:      LDT_SEQERR_ADR  : Tare key depressed under preselect void 
*                                 transaction.
*               LDT_KEYOVER_ADR : Tare code exceeds 10.
*
** Description: Check tare code data.
*               Check preselect void mode. 
*===========================================================================
*/

SHORT   ItemModTareCheckSeq(CONST UIFREGMODIFIER *pUifRegModifier) 
{
    SHORT  sReturnStatus;

    /*----- check tax exempt key depression     R3.0 -----*/
    if ((sReturnStatus = ItemCommonCheckExempt()) != ITM_SUCCESS) {
        return(sReturnStatus);
    }

    if (TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)) {
                                            /* preselect void trans. */
        return(LDT_SEQERR_ADR);
    }

    if (pUifRegModifier->lAmount > MAX_TARE_SIZE) {   /* illegal tare code entered */
        return(LDT_KEYOVER_ADR);
    }
    
    return(ITM_SUCCESS);
}


/*
*===========================================================================
** Synopsis:    VOID    ItemModTareDispMessage(UIFREGMODIFIER *pUifRegModifier) 
*                                     
*     Input:    UIFREGMODIFIER *pUifRegModifier
*               
*    Output:    nothing 
*
** Return:      VOID
*
** Description: Display leadthrough mnemonics for Scale/tare key.
*===========================================================================
*/

VOID   ItemModTareDispMessage(CONST UIFREGMODIFIER *pUifRegModifier)
{
	REGDISPMSG         RegDispMsgWork = { 0 };

    RegDispMsgWork.uchMajorClass = CLASS_UIFREGMODIFIER;
    RegDispMsgWork.uchMinorClass = CLASS_UITARE;
    RegDispMsgWork.lAmount = pUifRegModifier->lAmount;   /* set tare code */
	RflGetLead(RegDispMsgWork.aszMnemonic, LDT_TARE_ADR ); /* set the mnemonics to the buffer */
    RegDispMsgWork.fbSaveControl = DISP_SAVE_TYPE_4;

    flDispRegDescrControl &= ~ TAXEXMPT_CNTRL;  /* correction10/13 */
    flDispRegKeepControl &= ~ TAXEXMPT_CNTRL;

    DispWrite(&RegDispMsgWork);

}

/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemModAlphaName(VOID)
*                                     
*     Input:    ItemModifier  -- pointer form UI
*    Output:    nothing 
*
** Return:      LDT_PROHBT_ADR, ITM_SUCCESS
*
** Description: Customer Name Entry.
*===========================================================================
fhfh*/

SHORT     ItemModAlphaName(VOID)
{
    SHORT           sStatus;
	ITEMMODIFIER    ItemModifier = { 0 };
    REGDISPMSG      DispMsg = { 0 };
    UIFDIADATA      WorkUI = { 0 };

    if (ItemModLocal.fsTaxable != 0){
        return(LDT_SEQERR_ADR);
    }
    if (ItemModLocal.fbModifierStatus & MOD_FSMOD) {    /* Saratoga */
        return(LDT_SEQERR_ADR); 
    }

    /*----- check non-gc transaction V3.3 -----*/
    if ( (  sStatus =  ItemCommonCheckNonGCTrans() ) != ITM_SUCCESS ) {
        return(sStatus);
    }

    if ((sStatus = ItemCommonCheckComp() ) != ITM_SUCCESS) {
        return(sStatus);
    }

    if ((sStatus = ItemCommonCheckSplit()) != ITM_SUCCESS) {   /* R3.1 */
        return(sStatus);
    }

    /*--- Check Split State,    R3.1 ---*/
    if (TrnSplCheckSplit() == TRN_SPL_SEAT
        || TrnSplCheckSplit() == TRN_SPL_MULTI
        || TrnSplCheckSplit() == TRN_SPL_SPLIT) {
        return(LDT_SEQERR_ADR);
    }

    ItemModifier.uchMajorClass = CLASS_ITEMMODIFIER;
    ItemModifier.uchMinorClass = CLASS_ALPHANAME;
    ItemModifier.fsPrintStatus = PRT_JOURNAL | PRT_SLIP;

    DispMsg.uchMajorClass = CLASS_REGDISPCOMPLSRY;
    DispMsg.uchMinorClass = CLASS_REGDISP_CUSTOMER;
	RflGetLead(DispMsg.aszMnemonic, LDT_CUST_NAME_ADR);
    
    flDispRegDescrControl |= (COMPLSRY_CNTRL | CRED_CNTRL);
    flDispRegKeepControl |= (COMPLSRY_CNTRL | CRED_CNTRL);
    DispMsg.fbSaveControl = DISP_SAVE_TYPE_4;

    DispWrite(&DispMsg);

    flDispRegDescrControl &= ~COMPLSRY_CNTRL;
    flDispRegKeepControl &= ~COMPLSRY_CNTRL;

    if (UifDiaAlphaName(&WorkUI) != UIF_SUCCESS) {
        memset(&DispMsg, 0, sizeof(REGDISPMSG));
        DispMsg.uchMajorClass = CLASS_UIFREGMODIFIER;               /* dummy */
        DispMsg.uchMinorClass = CLASS_UITABLENO;                    /* dummy */
        DispMsg.lAmount = 0L;                                       /* dummy */
        DispMsg.aszMnemonic[0] = 0;                                 /* dummy */
        DispMsg.fbSaveControl = DISP_SAVE_ECHO_ONLY;
        DispWrite(&DispMsg);
                                                
        flDispRegDescrControl &= ~CRED_CNTRL; 
        flDispRegKeepControl &= ~CRED_CNTRL; 
		RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGOTHER, CLASS_UICLEAR, DISP_SAVE_TYPE_0);
        return(UIF_CANCEL);
    }

    _tcsncpy(ItemModifier.aszName, WorkUI.aszMnemonics, NUM_NAME);
    
    if ((sStatus = ItemPreviousItem(&ItemModifier, 0)) != ITM_SUCCESS) {
        return(sStatus);
    }

    DispMsg.uchMajorClass = CLASS_UIFREGMODIFIER;
    DispMsg.uchMinorClass = CLASS_UIALPHANAME;
    _tcsncpy(DispMsg.aszStringData, ItemModifier.aszName, NUM_TRANSMNEM);
    
    flDispRegDescrControl &= ~TAXEXMPT_CNTRL;
    flDispRegKeepControl &= ~TAXEXMPT_CNTRL;
    flDispRegDescrControl &= ~CRED_CNTRL; 
    flDispRegKeepControl &= ~CRED_CNTRL; 
    DispMsg.fbSaveControl = DISP_SAVE_TYPE_0;
    DispWrite(&DispMsg);

    ItemCommonPutStatus(COMMON_VOID_EC);
    ItemCommonCancelExempt();

	sStatus = ItemCommonSetupTransEnviron ();
	if(sStatus != ITM_SUCCESS)
		return (sStatus);

    /* send to enhanced kds, 2172 */
    sStatus = ItemSendKds(&ItemModifier, 0);
    if (sStatus != ITM_SUCCESS) {
            
        return sStatus;
    }

    return(ITM_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT   ItemModSeatNo1(UIFREGMODIFIER *UifRegModifier) 
*
*     Input:    
*    Output:    
*
** Return:      
*
** Description: Handle Seat Number Entry for Tender by splitting the items for
*               the specified seat number into a separate, temporary transaction
*               which can then be tendered out.
*               We have three different kinds of splits which are based on whether
*               the transaction being split already has seat numbers or not and
*               how many seats are in the transaction.
*===========================================================================
*/
static SHORT   ItemModSeatNo1(UIFREGMODIFIER *UifRegModifier)
{
    SHORT           sState;
	ITEMMODIFIER    ItemModifier = {0};

    /*----- for Seat# Tender -----*/
    if (TranCurQualPtr->uchSplit != 0) {
		// we are in the process of splitting an existing transaction and tendering the
		// transaction by seat number. we must have a seat number in order to find the
		// items to be split out if the transaction has seat numbers. if the transaction
		// does not have seat numbers then we will allow the use of Item Transfer to
		// transfer designated items to the seat being split out in order to tender out
		// only specific items.
        if (UifRegModifier->lAmount == 0L) {
			// if no seat number specified then sequence error.
            return(LDT_SEQERR_ADR);
        }

        if ((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_NEWCHECK
            || (TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_REORDER) {
            return(LDT_SEQERR_ADR);
        }

        /*---- guest check close restriction, V3.3 ----*/
        if ( ( sState = ItemCommonCheckCloseCheck() ) != ITM_SUCCESS ) {
            return( sState );
        }

        if (TrnSplSearchUseSeatQueue((UCHAR)UifRegModifier->lAmount) != TRN_SUCCESS) {
			// if this transaction does not have any seat number assignments with items
			// then we are doing a split of a transaction created without seat numbers.
			// so we will now allow items to be transferred from the current transaction
			// into a seat specific transaction with the specified seat number.
			// this is known as TRN_SPL_TYPE2 transaction or Seat# Trans Type 2
			TRANGCFQUAL     *WorkGCFSplA, *WorkGCFSplB;
			ITEMTRANSOPEN   TransOpen = {0};

            TrnGetGCFQualSpl(&WorkGCFSplA, TRN_TYPE_SPLITA);
            TrnGetGCFQualSpl(&WorkGCFSplB, TRN_TYPE_SPLITB);

            if (WorkGCFSplA->usGuestNo == 0 && WorkGCFSplB->usGuestNo == 0) {
                TrnInitializeSpl(TRANI_GCFQUAL_SPLA | TRANI_ITEMIZERS_SPLA | TRANI_CONSOLI_SPLA);
                TrnInitializeSpl(TRANI_GCFQUAL_SPLB | TRANI_ITEMIZERS_SPLB | TRANI_CONSOLI_SPLB);
            } else {
                return(LDT_PROHBT_ADR);
            }

            ItemPrintStart(TYPE_SPLITA);
            ItemPromotion(PRT_RECEIPT, TYPE_SPLITA);
            ItemHeader(TYPE_SPLITA);

            /*----- Transaction Open Data -----*/
            TransOpen.uchMajorClass = CLASS_ITEMTRANSOPEN;
            TransOpen.uchMinorClass = CLASS_NEWCHECK;
            TransOpen.uchSeatNo = (UCHAR)UifRegModifier->lAmount;
            TransOpen.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
            if (CliParaMDCCheck(MDC_TRANNUM_ADR, EVEN_MDC_BIT0) && ! CliParaMDCCheckField( MDC_TRANNUM_ADR, MDC_PARAM_BIT_C ) ) {
				// if MDC 234 Bit D is on to use Unique Transaction Number and MDC 234 Bit C is off then use the number.
				// if MDC 234 Bit C is on then we are to ignore the MDC 234 Bit D setting.
                TransOpen.usTableNo = ItemCommonUniqueTransNo();
            }
            TrnOpenSpl(&TransOpen, TRN_TYPE_SPLITA);

            ItemModifier.uchMinorClass = CLASS_SEAT_TYPE2;
            sState = TRN_SPL_TYPE2;
        } else if (TrnSplCheckSplit() == TRN_SPL_SEAT) {
			/*----- Multi Seat# Trans -----*/
			TRANGCFQUAL     *WorkGCFSplA;

            /*----- AddCheck -----*/
            if ((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) != CURQUAL_ADDCHECK) {
                return(LDT_PROHBT_ADR);
            }

            /*----- Prohibit Individual R/D -----*/
            TrnGetGCFQualSpl(&WorkGCFSplA, TRN_TYPE_SPLITA);
            if (WorkGCFSplA->fsGCFStatus2 & GCFQUAL_REGULAR_SPL) {
                return(LDT_PROHBT_ADR);
            }

            /*----- Check Next Seat# -----*/
            if (TrnSplGetSeatTransCheck((UCHAR)UifRegModifier->lAmount) != TRN_SUCCESS) {
                return(TRN_ERROR);
            }

            /*----- Add Multi Seat# Trans (Multi Seat# Print) -----*/
            TrnSplAddSeatTrans((UCHAR)UifRegModifier->lAmount);

            ItemModifier.uchMinorClass = CLASS_SEAT_MULTI;
            sState = TRN_SPL_MULTI;
        } else {
			/*----- Single Seat# Trans -----*/
            /*----- AddCheck -----*/
            if ((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) != CURQUAL_ADDCHECK) {
                return(LDT_PROHBT_ADR);
            }

            /*----- Get Target Seat# Transaction -----*/
            if (TrnSplGetSeatTrans((UCHAR)UifRegModifier->lAmount, TRN_SPL_UPDATE, TRN_CONSOLI_STO, TRN_TYPE_SPLITA) != TRN_SUCCESS) {
                return(LDT_PROHBT_ADR);
            }

            ItemModifier.uchMinorClass = CLASS_SEAT_SINGLE;
            sState = TRN_SPL_SEAT;
        }

        /*--- cancel auto charge tips by check total, V3.3 ---*/
        ItemPrevCancelAutoTips();

		/*----- Display SplitA Guest Check on LCD Middle -----*/
		{
			TRANINFSPLIT *SplitA = TrnGetInfSpl(TRN_TYPE_SPLITA);

			if (SplitA->TranGCFQual.usGuestNo != 0) {
				TRANITEMIZERS   * const WorkTI = TrnGetTISplPointer(TRN_TYPE_SPLITA);
				MLDTRANSDATA    MldTran = {0};

				MldDisplayInit(MLD_DRIVE_2, 1);
				MldPutSplitTransToScroll(MLD_SCROLL_2, MldMainSetMldDataSplit (&MldTran, TRN_TYPE_SPLITA));
				MldDispSubTotal(MLD_TOTAL_2, WorkTI->lMI);
			}
		}

		/*----- Display SplitB Guest Check on LCD Right -----*/
		{
			TRANINFSPLIT *SplitB = TrnGetInfSpl(TRN_TYPE_SPLITB);

			if (SplitB->TranGCFQual.usGuestNo != 0) {
				TRANITEMIZERS   *WorkTI = TrnGetTISplPointer(TRN_TYPE_SPLITB);
				MLDTRANSDATA    MldTran = {0};

				MldDisplayInit(MLD_DRIVE_3, 0);
				MldPutSplitTransToScroll(MLD_SCROLL_3, MldMainSetMldDataSplit(&MldTran, TRN_TYPE_SPLITB));
				MldDispSubTotal(MLD_TOTAL_3, WorkTI->lMI);
			}
		}
    } else {
		/*----- Seat# Entry State -----*/
		/* ---- send previous data if table no/no of person is put (for display) */
        if (ItemCommonLocalPtr->ItemSales.uchMajorClass == CLASS_ITEMMODIFIER) {
            if ( ItemPreviousItem(0, 0) != ITM_SUCCESS ) { /* Saratoga */
                return(LDT_PROHBT_ADR);
            }
        }
        ItemModifier.uchMinorClass = CLASS_SEATNO;
        sState = 0;
    }

    ItemModifier.uchMajorClass = CLASS_ITEMMODIFIER;
    ItemModifier.uchLineNo = (UCHAR)UifRegModifier->lAmount;
    TrnItem(&ItemModifier);

    /* send to enhanced kds, 2172 */
    ItemSendKds(&ItemModifier, 0);

	{
		REGDISPMSG      Disp = {0};

		Disp.uchMajorClass = CLASS_UIFREGMODIFIER;
		Disp.uchMinorClass = CLASS_UISEATNO1;
		RflGetSpecMnem(Disp.aszMnemonic, SPC_SEAT_ADR);
		Disp.lAmount = ItemModifier.uchLineNo;
		flDispRegDescrControl &= ~TAXEXMPT_CNTRL;
		flDispRegKeepControl &= ~TAXEXMPT_CNTRL;
		DispWrite(&Disp);

		MldScrollWrite(&ItemModifier, MLD_NEW_ITEMIZE);
	}

    ItemCommonCancelExempt();

    if (sState == TRN_SPL_SEAT || sState == TRN_SPL_MULTI) {
        return(UIF_SEAT_TRANS);
    } else if (sState == TRN_SPL_TYPE2) {
        return(UIF_SEAT_TYPE2);
    } else {
        return(ITM_SUCCESS);
    }
}

/*
*===========================================================================
** Synopsis:    SHORT   ItemModSeatNo2(UIFREGMODIFIER *pData)
*                                     
*     Input:    
*    Output:    nothing 
*
** Return:      LDT_PROHBT_ADR, ITM_SUCCESS
*
** Description: Handle FSC_SEAT with minor class of 2 to display specific seat items.
*               Display Seat# Transaction on LCD grouping the items for a specific
*               seat number into a single display and not displaying the other
*               seat numbers and their items.
*               There does not seem to be any use for this other than to just
*               see the list of items for a particular seat.
*===========================================================================
*/
static SHORT   ItemModSeatNo2(UIFREGMODIFIER *pData)
{
    SHORT   i;
    MLDTRANSDATA    WorkMLD;

    /*----- Prohibit Partial Tender -----*/
    if (ItemTenderLocalPtr->fbTenderStatus[0] & TENDER_PARTIAL) {
        return(LDT_PROHBT_ADR);
    }
    if (ItemTenderLocalPtr->fbTenderStatus[1] & TENDER_SEAT_PARTIAL) {
        return(LDT_PROHBT_ADR);
    }

    if (TrnSplCheckSplit() != TRN_SUCCESS) {
        return(LDT_PROHBT_ADR);
    }

    if (pData->lAmount == 0L) {
        return(LDT_SEQERR_ADR);
    }

    /*----- Check Input Seat# -----*/
    for (i = 0; i < NUM_SEAT; i++) { //SR206
        if (TranGCFQualPtr->auchUseSeat[i] == (UCHAR)pData->lAmount) {
            break;
        }
    }
    if (i >= NUM_SEAT) {
        return(LDT_PROHBT_ADR);
    }
    for (i = 0; i < NUM_SEAT; i++) { //SR206
        if (TranGCFQualPtr->auchFinSeat[i] == (UCHAR)pData->lAmount) {
            return(LDT_PROHBT_ADR);
        }
    }

    /* ---- put previous item 07/08/96 ---- */
    if ( ItemPreviousItem(0, 0) != ITM_SUCCESS ) { /* Saratoga */
        return(LDT_PROHBT_ADR);
    }

    /*----- Create Seat# Transaction in Split A Area -----*/
    if (TrnSplGetSeatTrans((UCHAR)pData->lAmount, TRN_SPL_NOT_UPDATE, TRN_CONSOLI_STO, TRN_TYPE_SPLITA) != TRN_SUCCESS) {
        return(LDT_PROHBT_ADR);
    }

    /*----- Display Seat# Transaction on LCD -----*/
	MldPutTransToScroll(MldMainSetMldDataSplit(&WorkMLD, TRN_TYPE_SPLITA));

    MldDispSubTotal(MLD_TOTAL_1, TrnInfSplPtrSplitA->TranItemizers.lMI);

    /*----- Pause for Scroll Display -----*/
	/* ----- clear display lead thru 07/08/96 ---- */
	RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGOTHER, CLASS_UICLEAR, DISP_SAVE_TYPE_0);

	ItemCommonScrollPause();

    /*----- Re-Display Previous Transaction on LCD -----*/
#if 1
	MldPutTransToScroll(MldMainSetMldDataSplit(&WorkMLD, TRN_TYPE_CONSSTORAGE));
#else
    memset(&WorkMLD, 0, sizeof(MLDTRANSDATA));
    WorkMLD.usVliSize = TrnInformationPtr->usTranConsStoVli;
    WorkMLD.sFileHandle = TrnInformationPtr->hsTranConsStorage;
    WorkMLD.sIndexHandle = TrnInformationPtr->hsTranConsIndex;
    MldPutTransToScroll(&WorkMLD);
#endif

    ItemCommonCancelExempt();

    return(ITM_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT   ItemModSeatNo(UIFREGMODIFIER *UifRegModifier) 
*                                     
*     Input:    ItemModifier  -- pointer form UI
*    Output:    nothing 
*
** Return:      LDT_PROHBT_ADR, ITM_SUCCESS
*
** Description: Handle FSC_SEAT Seat Number Entry
*===========================================================================
*/
SHORT   ItemModSeatNo(UIFREGMODIFIER *UifRegModifier)
{
    SHORT           sStatus;

    if ((sStatus = ItemCommonCheckExempt() ) != ITM_SUCCESS) {
        return(sStatus);
    }
    if (ItemModLocal.fbModifierStatus & MOD_FSMOD) {    /* Saratoga */
        return(LDT_SEQERR_ADR); 
    }
    if ((sStatus = ItemCommonCheckComp() ) != ITM_SUCCESS) {
        return(sStatus);
    }

    /*----- Prohibit Cashier Transaction -----*/
    if ((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_CASHIER) {
        return(LDT_SEQERR_ADR);
    }
    /* R3.3 */
    if ( TranModeQualPtr->fsModeStatus & MODEQUAL_CASINT) { /* cashier interrupt */
        return(LDT_SEQERR_ADR);
    }

	if (UifRegModifier->lAmount == 0L) {
		// a seat number was not entered before pressing the FSC_SEAT key so
		// we will prompt the user for a seat number.  they can also Cancel out
		// at this point as well.
		UCHAR  uchSeatNo = 0;
		if ((sStatus = ItemOtherSeatNo(&uchSeatNo)) != ITM_SUCCESS) {
			return sStatus;
		}
		UifRegModifier->lAmount = uchSeatNo;
	}

    /*----- Prohibit Using a Seat # that has already been used -----*/
    if (UifRegModifier->lAmount != 0L) {
		SHORT         i;
		TRANGCFQUAL   *WorkGCF = TrnGetGCFQualPtr();

        for (i = 0; i < NUM_SEAT; i++) { //SR206
            if (WorkGCF->auchFinSeat[i] == (UCHAR)UifRegModifier->lAmount) {
                return(LDT_KEYOVER_ADR);
            }
        }
    }

    /*----- Reset Total Status 6/3/96 -----*/
	TranCurQualTotalStatusClear ();

	// determine what to do based on the extended FSC value which was turned into a minor class.
	// we can either display the transaction data for a 
    if (UifRegModifier->uchMinorClass == CLASS_UISEATNO1) {
		/*----- Get Seat# Transaction data for Tender by seat number -----*/
        return (ItemModSeatNo1 (UifRegModifier));
    } else {
		/*----- Read and display specific Seat# Transaction data -----*/
        return (ItemModSeatNo2 (UifRegModifier));
    }
}


/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemModAdjMenu(UIFREGMODIFIER *ItemModifier) 
*                                     
*     Input:    ItemModifier  -- pointer form UI
*    Output:    nothing 
*
** Return:      LDT_PROHBT_ADR, ITM_SUCCESS
*
** Description: ItemModMenu executes setting "menu sift" 2172
*===========================================================================
fhfh*/

SHORT   ItemModAdjMenu(CONST UIFREGMODIFIER *ItemModifier )
{
    SHORT       sStatus = ITM_SUCCESS; 
	REGDISPMSG  RegDispMsgD = { 0 };
    ITEMSALESLOCAL *pWorkSales = ItemSalesGetLocalPointer();

    /* correction10/13 */
    /* check pressing total key or not    R3.0 */
    if ( (  sStatus =  ItemCommonCheckExempt() ) != ITM_SUCCESS ) {
        return(sStatus);
    }

    /* check No,  R3.1 */
    if (ItemModifier->uchMenuPage > MAX_ADJECTIVE_NO ) {
        return( LDT_KEYOVER_ADR );
    }

	pWorkSales->uchAdjKeyShift = ItemModifier->uchMenuPage;                     /* set default adjective, 2172 */
	pWorkSales->uchAdjCurShift = ItemModifier->uchMenuPage;                     /* set default adjective, 2172 */

    /* display */
    RegDispMsgD.uchMajorClass = CLASS_UIFREGMODIFIER;  /*set majour class */
    RegDispMsgD.uchMinorClass = CLASS_UIADJSHIFT;      /*set minor class */
    RegDispMsgD.aszMnemonic[0] = (UCHAR)(ItemModifier->uchMenuPage | 0x30);    /* adjective code */
    RegDispMsgD.aszMnemonic[1] = 0;
    DispWrite( &RegDispMsgD );

    /*----- check off line tender status 4/16/96 -----*/
    if ( ItemModLocal.fbModifierStatus & MOD_OFFLINE ) {    
        ItemModOffTend();
    }
    
    return( ITM_SUCCESS );
}

/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemModNumber(UIFREGMODIFIER *ItemModifier) 
*                                     
*     Input:    ItemModifier  -- pointer form UI
*               
*    Output:    nothing 
*
** Return:      LDT_SEQERR_ADR, ITM_SUCCESS
*
** Description: ItemModPerson executes to control no. of person. 
*===========================================================================
fhfh*/

SHORT   ItemModNumber(UIFREGMODIFIER *ItemModifier) 
{
    return( ITM_SUCCESS );

    ItemModifier = ItemModifier;

#if 0
    ITEMMODIFIER    TrnItemModifier;        
    SHORT           sStatus;

    /* initialize */
    memset( &TrnItemModifier, 0, sizeof(ITEMMODIFIER) );

    /* set major/minor class*/
    TrnItemModifier.uchMajorClass = CLASS_ITEMMODIFIER;
    TrnItemModifier.uchMinorClass = CLASS_MODNUMBER;

    /* set No. of person from ui */
    _tcsncpy(TrnItemModifier.aszName, ItemModifier->aszNumber, NUM_NUMBER);
    //memcpy(TrnItemModifier.aszName, ItemModifier->aszNumber, NUM_NUMBER);

    /* send to enhanced kds, 2172 */
    sStatus = ItemSendKds(&TrnItemModifier, 0);
    if (sStatus != ITM_SUCCESS) {
            
        return sStatus;
    }

    return( ITM_SUCCESS );

#endif
}

/*
*===========================================================================
** Synopsis:    SHORT   ItemModFS(UIFREGMODIFIER *ItemModifier) 
*                                     
*     Input:    ItemModifier  -- pointer form UI
*               
*    Output:    nothing 
*
** Return:      LDT_SEQERR_ADR, ITM_SUCCESS
*
** Description: ItemModTaxUS executes "us tax sys."  
*===========================================================================
*/
SHORT    ItemModFS(CONST UIFREGMODIFIER *ItemModifier)
{
	REGDISPMSG      Disp = { 0 };

    flDispRegDescrControl   |= FOODSTAMP_CTL;
    flDispRegKeepControl    |= FOODSTAMP_CTL;

    Disp.uchMajorClass = CLASS_UIFREGMODIFIER;
    Disp.uchMinorClass = ItemModifier->uchMinorClass;
	RflGetSpecMnem (Disp.aszMnemonic, SPC_FSMOD_ADR);
    _tcsncpy(Disp.aszStringData, ItemModifier->aszNumber, NUM_NUMBER);
    Disp.fbSaveControl = DISP_SAVE_TYPE_4;

    DispWrite(&Disp);

    ItemModLocal.fbModifierStatus |= MOD_FSMOD;

    return(ITM_SUCCESS);
}

/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemModWic(VOID)
*
*     Input:    None
*    Output:    None
*
** Return:      LDT_SEQERR_ADR, UIF_SUCCESS
*
** Description: ItemModWic executes wic transaction             Saratoga
*===========================================================================
fhfh*/
SHORT   ItemModWIC(VOID)
{
	ITEMMODIFIER    ItemMod = { 0 };
    REGDISPMSG      Disp = { 0 };
    SHORT           sStatus;  

    if (ItemModLocal.fsTaxable != 0) {
        return(LDT_SEQERR_ADR);
    }
    if (ItemModLocal.fbModifierStatus & MOD_FSMOD) {
        return(LDT_SEQERR_ADR); 
    }

	sStatus = ItemCommonCheckCashDrawerOpen ();
	if (sStatus != 0)
		return sStatus;

    Disp.uchMajorClass = CLASS_UIFREGMODIFIER;
    Disp.uchMinorClass = CLASS_UIFWICTRAN;

    if (TranCurQualPtr->fsCurStatus & CURQUAL_WIC) {
		RflGetLead(Disp.aszMnemonic, LDT_PROGRES_ADR);
    } else {
		RflGetLead(Disp.aszMnemonic, LDT_WICTRN_ADR);
	}

    DispWrite(&Disp);

    ItemMod.uchMajorClass = CLASS_ITEMMODIFIER;
    ItemMod.uchMinorClass = CLASS_WICTRN;
    TrnItem(&ItemMod);

    return(ITM_SUCCESS);
}

/*===========================================================================
** Synopsis:    SHORT   ItemModFor( UIFREGOTHER *pItemModifier )
*
*     Input:    UIFREGOTHER *pItemModifier;
*    Output:    None
*
** Return:      UIF_SUCCESS
*
** Description: ItemModFor executes @/For key process
*			Purpose is to allow a user to ring up an arbitrary number of items
*			and charge them at a unit rate based of a multiple price structure.
*			Ex: buying 5 items priced at 3 for $1.00 means the price for 5 is $1.66.
*			This function 
*===========================================================================
fhfh*/

SHORT   ItemModFor( CONST UIFREGSALES *pItemModifier, CONST UIFREGSALES *pRegData )
{
	REGDISPMSG      RegDisplay = { 0 };
	UIFDIADATA      WorkUI;
	SHORT           sRetStatus;

	sRetStatus = ItemCommonCheckCashDrawerOpen ();
	if (sRetStatus != 0)
		return sRetStatus;

    /* --- get transaction mnemonics --- */
	// SR281 Display Quantity Amount in LDT
	RegDisplay.uchMajorClass = CLASS_UIFREGDISP;
    RegDisplay.uchMinorClass = CLASS_UIFDISP_FOR;
	RegDisplay.lAmount = pItemModifier->lQTY * 1000L;
    memcpy( RegDisplay.aszStringData, pItemModifier->aszNumber, NUM_NUMBER);
    DispWrite( &RegDisplay );


	// SR 143 @/For key cwunn peform price entry
	/*  This portion of code makes NHPOS to wait for a dept number entry, 
		the memcpy is putting NHPOS in the accept an entry so that it can get the
		rest of the information required to do the @/For functionality*/
	WorkUI.auchFsc[0] = CLASS_UIFREGOTHER;
	WorkUI.auchFsc[1] = CLASS_UIFFOR;

	/* --- get transaction mnemonics --- */
	WorkUI.ulData = wcstol(pItemModifier->aszNumber, NULL, 10);
	//_tcsncpy( WorkUI.aszMnemonics, ParaTransMnemo.aszMnemonics, NUM_TRANSMNEM );

	if(UifDiaPrice(&WorkUI) == UIFREG_ABORT){
		//Execute cancel logic and do not process @/For sequence
		return(UIE_ERR_KEY_SEQ);
	}

	/* Make sure that the Price Enter Key was pressed */
	if ((WorkUI.auchFsc[0] != FSC_PRICE_ENTER) &&
		(WorkUI.auchFsc[0] != FSC_KEYED_DEPT)) { //SR281 ESMITH 
		return (LDT_SEQERR_ADR);
	}


	//Calculate total price of all items sold
	//pRegData->lQTY is the number sold, pItemModifier->lQTY is the number of items in a package,
	//ulData is the package price, totalPrice represents the calculated total price
	//**This method has a vulnerability that has been determined to be of minimal likelihood.
	//**By casting ulData to long,if the package price is greater than 2,147,483,647,
	//** the upper bound for the long data type, then ItemCalAmount will see the package price as negative
	//** which means incorrect calculation will occur.
	//**Since package price is VERY unlikely to reach this bound, the vulnerability is acceptable.
	//**The same vulnerability applies to the lQty being casted to short:
	//**  if lQty is greater than 32,767, ItemCalAmount will see the number of items in a package as negative
	//**  which will cause incorrect calculations.
	//**Since the number of items in the package is rejected if it is greater than 3 digits in size,
	//** this vulnerability will never be exploited	

	//SR281 ESMITH
	/*  Total Price is calculated later in the process, after the dept key is pressed. Previously total
		price was divided by the quantity to state the price of individual items. Then later the individual
		price was multiplied by the total items to get the expected "overall" price. This was incorrect because
		it resulted in truncating of digits on the total price. 
		Ex. 
			2 @ 3  FOR 5.00 would result in individual items being 1.66 for each item.
			Then 2 * 1.66 = 3.32, which is an incorrect value. The correct total price
			should be 2/3 * 5.00 = 3.33.
	*/
	//ItemCalAmount(pRegData->lQTY, (SHORT)pItemModifier->lQTY, (LONG) WorkUI.ulData, &totalPrice);
	
	/* --- get transaction mnemonics --- */
    RflGetTranMnem( RegDisplay.aszMnemonic, TRN_PRICE_ADR );

	/* --- display  dept # mnemonic --- */ //SR281 ESMITH
    RegDisplay.uchMajorClass = CLASS_UIFREGOTHER;
    RegDisplay.uchMinorClass = CLASS_UIFPRICE;
	RegDisplay.lAmount = WorkUI.ulData;
    memcpy( RegDisplay.aszStringData, pItemModifier->aszNumber, NUM_NUMBER);
    DispWrite( &RegDisplay );

	{
		UIFREGSALES ItemRegSales = {0};

		//Build Item Sales object
		ItemRegSales.uchMajorClass = CLASS_UIFREGSALES;
		ItemRegSales.uchMinorClass = CLASS_UIFOR;
		ItemRegSales.lQTY = pRegData->lQTY;
		ItemRegSales.lUnitPrice = WorkUI.ulData;
		ItemRegSales.usFor = (SHORT)WorkUI.ulData;
		ItemRegSales.usForQty = (SHORT)pItemModifier->lQTY;

		//By adding this to the function here, if the user enters a keyed department key,
		//then we will go ahead and do the functionality so that we do not require any more key
		//presses by the user, SR333
		if(WorkUI.auchFsc[0] == FSC_PRICE_ENTER) {
			//Perform sales action
			sRetStatus = ItemSales(&ItemRegSales);
		} else if(WorkUI.auchFsc[0] == FSC_KEYED_DEPT) {
			UifRegData.regsales = ItemRegSales;
			UifRegData.regsales.uchMinorClass = CLASS_UIDEPT;
			UifRegData.regsales.uchMajorFsc = WorkUI.auchFsc[0]; /* set dept fsc */
			UifRegData.regsales.uchFsc = WorkUI.auchFsc[1]; /* set dept extend fsc */

			sRetStatus = ItemSales(&UifRegData.regsales); /* Sales Modele */
			return(sRetStatus);
		}
	}

    return(UIF_SUCCESS);
}

/*fhfh
*===========================================================================
** Synopsis:    SHORT   ItemModTent(VOID)
*                                     
*     Input:    ItemModifier  -- pointer form UI
*    Output:    nothing 
*
** Return:      LDT_PROHBT_ADR, ITM_SUCCESS
*
** Description: Customer Name Entry.  Used with Receipt Id or Tend Id with
*               FSC_TENT key press to link an order to a plastic tent with
*               a numeric number on it or to link the order to a customer name
*               entered with an alphanumeric key pad.
*===========================================================================
fhfh*/

SHORT     ItemModTent(CONST TCHAR *aszNameEntry, USHORT usNameEntryLen)
{
    SHORT           sStatus;
	ITEMMODIFIER    ItemModifier = { 0 };
	REGDISPMSG      DispMsg = { 0 };
	UIFDIADATA      WorkUI = {0};
	TRANMODEQUAL	*pTranModeQual = TrnGetModeQualPtr();

    if (ItemModLocal.fsTaxable != 0){
        return(LDT_SEQERR_ADR);
    }
    if (ItemModLocal.fbModifierStatus & MOD_FSMOD) {    /* Saratoga */
        return(LDT_SEQERR_ADR); 
    }

	sStatus = ItemCommonCheckCashDrawerOpen ();
	if (sStatus != 0)
		return sStatus;

    /*----- check non-gc transaction V3.3 -----*/
    if ( (  sStatus =  ItemCommonCheckNonGCTrans() ) != ITM_SUCCESS ) {
        return(sStatus);
    }

    if ((sStatus = ItemCommonCheckComp() ) != ITM_SUCCESS) {
        return(sStatus);
    }

    if ((sStatus = ItemCommonCheckSplit()) != ITM_SUCCESS) {   /* R3.1 */
        return(sStatus);
    }

    /*--- Check Split State,    R3.1 ---*/
    if (TrnSplCheckSplit() == TRN_SPL_SEAT
        || TrnSplCheckSplit() == TRN_SPL_MULTI
        || TrnSplCheckSplit() == TRN_SPL_SPLIT) {
        return(LDT_SEQERR_ADR);
    }

    ItemModifier.uchMajorClass = CLASS_ITEMMODIFIER;
    ItemModifier.uchMinorClass = CLASS_ALPHATENT;
    ItemModifier.fsPrintStatus = PRT_JOURNAL | PRT_SLIP;

	if (usNameEntryLen > 0) {
		// we will allow the use of the numeric key pad to enter a tent number
		// if operator enters a number and then presses the FSC_TENT key.
		_tcsncpy(WorkUI.aszMnemonics, aszNameEntry, NUM_NAME);
	} else {
		DispMsg.uchMajorClass = CLASS_REGDISPCOMPLSRY;
		DispMsg.uchMinorClass = CLASS_REGDISP_CUSTOMER;
		RflGetLead(DispMsg.aszMnemonic, LDT_TENT);
		DispMsg.fbSaveControl = DISP_SAVE_TYPE_4;
	    
		flDispRegDescrControl |= (COMPLSRY_CNTRL | CRED_CNTRL);
		flDispRegKeepControl |= (COMPLSRY_CNTRL | CRED_CNTRL);

		DispWrite(&DispMsg);

		flDispRegDescrControl &= ~COMPLSRY_CNTRL;
		flDispRegKeepControl &= ~COMPLSRY_CNTRL;

		if (UifDiaAlphaName(&WorkUI) != UIF_SUCCESS) {
			memset(&DispMsg, 0, sizeof(REGDISPMSG));
			DispMsg.uchMajorClass = CLASS_UIFREGMODIFIER;               /* dummy */
			DispMsg.uchMinorClass = CLASS_UITABLENO;                    /* dummy */
			DispMsg.lAmount = 0L;                                       /* dummy */
			DispMsg.aszMnemonic[0] = 0;                                 /* dummy */
			DispMsg.fbSaveControl = DISP_SAVE_ECHO_ONLY;
			DispWrite(&DispMsg);
	                                                
			flDispRegDescrControl &= ~CRED_CNTRL; 
			flDispRegKeepControl &= ~CRED_CNTRL; 
			RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGOTHER, CLASS_UICLEAR, DISP_SAVE_TYPE_0);
			return(UIF_CANCEL);
		}
	}
    _tcsncpy(ItemModifier.aszName, WorkUI.aszMnemonics, NUM_NAME);
	//Also stores the string in a special buffer used for tent implementation.
	//PDINU
	_tcsncpy(pTranModeQual->aszTent, WorkUI.aszMnemonics, NUM_NAME);
 
    if ((sStatus = ItemPreviousItem(&ItemModifier, 0)) != ITM_SUCCESS) {
        return(sStatus);
    }

    DispMsg.uchMajorClass = CLASS_UIFREGMODIFIER;
    DispMsg.uchMinorClass = CLASS_UIALPHANAME;
    _tcsncpy(DispMsg.aszStringData, ItemModifier.aszName, NUM_TRANSMNEM);
    DispMsg.fbSaveControl = DISP_SAVE_TYPE_0;
    
    flDispRegDescrControl &= ~TAXEXMPT_CNTRL;
    flDispRegKeepControl &= ~TAXEXMPT_CNTRL;
    flDispRegDescrControl &= ~CRED_CNTRL; 
    flDispRegKeepControl &= ~CRED_CNTRL; 

    DispWrite(&DispMsg);

    ItemCommonPutStatus(COMMON_VOID_EC);
    ItemCommonCancelExempt();

	sStatus = ItemCommonSetupTransEnviron ();
	if(sStatus != ITM_SUCCESS)
		return (sStatus);

    /* send to enhanced kds, 2172 */
    sStatus = ItemSendKds(&ItemModifier, 0);
    if (sStatus != ITM_SUCCESS) {
            
        return sStatus;
    }

    return(ITM_SUCCESS);
}
/****** End of Source ******/