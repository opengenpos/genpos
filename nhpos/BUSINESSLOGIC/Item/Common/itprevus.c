/*
*===========================================================================
* Title       : ItemPreviousItem() Module                          
* Category    : Item Common Module, NCR 2170 US Hsopitality Model Application         
* Program Name: ITPREVUS.C
* --------------------------------------------------------------------------
* Abstract: ItemPreviousItem()   
*            
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver. Rev. :   Name    : Description
* Jun- 5-92: 00.00.01  : K.Maeda   : initial                                   
* Dec- 4-92: 01.00.00  : K.Maeda   : Total proc for modifier class.                                   
* Jun-29-93: 01.00.12  : K.You     : add condi./print modi. priority feature.
*          :           :           : (mod. ItemPreviousItem)
* Feb-27-95: 03.00.00  : hkato     : R3.0
* Jan-11-96: 03.01.00  : hkato     : R3.1
* Jan-08-98: 03.01.16  : M.Ozawa   : Modify charge tips storage control.
* Jul-22-98: 03.03.00  : hrkato    : V3.3
* Aug-11-99: 03.05.00  : M.Teraki  : Remove WAITER_MODEL
*
** NCR2171 **
* Sep-06-99: 01.00.00  : m.teraki  : initial (for 2172)
* Oct-28-99:           : m.teraki  : fix bug in ItemPrevTotalProc()
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
#include    <stdlib.h>
#include    <string.h>

#include    "ecr.h"
#include    "pif.h"
#include    "uie.h"
#include    "rfl.h"
#include    "uireg.h"
#include    "regstrct.h"
#include    "transact.h"
#include    "item.h"
#include    "paraequ.h"
#include    "para.h"
#include    "csstbpar.h"
#include    "display.h"
#include    "mld.h"
#include    "itmlocal.h"
#include	"trans.h"

#define     ITM_PERFORM     100

static SHORT (*pComp)(VOID *pKey, VOID *pusHPoint) = ItemPModCompare;    /* Add R3.0 */


/*
*===========================================================================
**Synopsis: USHORT ItemSalesVoidCondiment(ITEMSALES *ItemSales,USHORT usCondNumber)
*
*    Input: Nothing
*   Output: Nothing
*    InOut: UIFREGSALES *pCurrentItem - used to pass @For key data
*			 back to caller's variable
**Return: VOID
*
** Description: Void a condiment that the user has chosen. This functionality
*				allows a user to choose a certain condiment off of an item
*				and delete it by itself instead of deleting the entire item.
*===========================================================================
*/
static USHORT ItemSalesVoidCondiment(ITEMSALES *pItemSales, USHORT usCondNumber)
{
	if(pItemSales->uchCondNo > 0)
	{
		USHORT     usCount;
		USHORT     usMaxChild = ( pItemSales->uchChildNo + pItemSales->uchCondNo + pItemSales->uchPrintModNo );
		ITEMSALES  CondimentWorkArea = {0};

		//Assign ItemSales so we can do some amount calculation
		CondimentWorkArea.uchMajorClass = pItemSales->uchMajorClass;	
		CondimentWorkArea.uchMinorClass = CLASS_ITEMMODIFICATION;

		//GetCursor has returned a value, therefore we will be editing a condiment
		//we subtract by one because we are using a zero based array.
		usCondNumber -= 1;	

		// First of all we are going to adjust the product price removing the cost of the
		// condiment and adjusting the transaction data.

		// We now remove this condiment by using the quantity of the ItemSales
		// and multiply it by negative one to remove this condiment from the
		// itemizer calculations including tax stuff.
		// To make things easier for the purposes of PPI recalculation, we will just
		// remove all of the existing condiments from the itemizer calculations,
		// adjust the condiments, recalculate the condiment pricing, and then we
		// will add all of the condiments back into the itemizer calculations.

		CondimentWorkArea.lQTY = pItemSales->lQTY * (-1);
		CondimentWorkArea.lProduct = 0;
		for(usCount = 0; usCount < usMaxChild; usCount++)
		{
			CondimentWorkArea.Condiment[usCount] = pItemSales->Condiment[usCount];
		}
		CondimentWorkArea.uchCondNo = pItemSales->uchCondNo; 
		CondimentWorkArea.uchChildNo = pItemSales->uchChildNo;
		CondimentWorkArea.uchPrintModNo = pItemSales->uchPrintModNo;
		TrnSalesPLU(&CondimentWorkArea);

		// Now we remove the condiment that is being voided. Once removed we will
		// recalculate the condiment pricing and then update the itemizer data.

		pItemSales->uchCondNo --;
		for(usCount = usCondNumber; usCount < usMaxChild; usCount++)
		{
			pItemSales->Condiment[usCount] = pItemSales->Condiment[usCount + 1];
		}
		memset(&pItemSales->Condiment[usCount], 0x00, sizeof(pItemSales->Condiment[0]));

		// Now recalculate the condiment pricing to include any condiments with PPI.
		// Once we have recalculated the condiment pricing we will update the itemizer
		// data with our modified condiment pricing by adding the condiments back into
		// the itemizer data.
		ItemSalesCalcCondimentPPIReplace (pItemSales);

		CondimentWorkArea.lQTY = pItemSales->lQTY;
		CondimentWorkArea.lProduct = 0;
		for(usCount = 0; usCount < usMaxChild; usCount++)
		{
			CondimentWorkArea.Condiment[usCount] = pItemSales->Condiment[usCount];
		}
		CondimentWorkArea.uchCondNo = pItemSales->uchCondNo; 
		CondimentWorkArea.uchChildNo = pItemSales->uchChildNo;
		CondimentWorkArea.uchPrintModNo = pItemSales->uchPrintModNo;
		TrnSalesPLU(&CondimentWorkArea);
	}
	return SUCCESS;
}

/*
*===========================================================================
**Synopsis: USHORT ItemSalesAddReplaceCondiment(ITEMSALES *pItemSales, USHORT usCondNumber, ITEMCOND *pCondimentData, LONG *lPrice)
*
*    Input: Nothing
*   Output: Nothing
*    InOut: UIFREGSALES *pCurrentItem - used to pass @For key data
*			 back to caller's variable
**Return: VOID
*
** Description: Modify the condiment data for an item and update the totals
*               with the new condiment data..  
*===========================================================================
*/
static USHORT ItemSalesAddReplaceCondiment (ITEMSALES *pItemSales, USHORT usCondNumber, ITEMCOND *pCondimentData)
{
	if (usCondNumber > 0) {
		ITEMSALES CondimentWorkArea = {0};

		//Assign ItemSales so we can do some amount calculation
		CondimentWorkArea.uchMajorClass = CLASS_ITEMSALES;	
		CondimentWorkArea.uchMinorClass = CLASS_ITEMMODIFICATION;

		//GetCursor has returned a value, therefore we will be editing a condiment
		//we subtract by one because we are using a zero based array.
		usCondNumber -= 1;	

		//We are only changing 1 condiment , so we assign it 1
		CondimentWorkArea.uchCondNo = 1; 

		// We now remove this condiment by using the quantity of the ItemSales
		// and multiply it by negative one to remove this condiment from the
		// itemizer calculations including tax stuff.
		if (pItemSales->Condiment[usCondNumber].auchPLUNo[0] != 0) {
			CondimentWorkArea.lQTY = pItemSales->lQTY * (-1);
			CondimentWorkArea.lProduct = 0;
			CondimentWorkArea.Condiment[0] = pItemSales->Condiment[usCondNumber]; 		
			TrnSalesPLU(&CondimentWorkArea);
		}

		// copy the new data into the location of the old condiment data
		// and then recalculate the condiment pricing to include any
		// condiments with PPI.
		pItemSales->Condiment[usCondNumber] = *pCondimentData;
		ItemSalesCalcCondimentPPIReplace (pItemSales);

		// Now we will redo the calculations with the new condiment data
		// We do this update after recalculating the condiment pricing which
		// may include PPI calculations.
		CondimentWorkArea.lQTY = pItemSales->lQTY;
		CondimentWorkArea.lProduct = 0;
		CondimentWorkArea.Condiment[0] = pItemSales->Condiment[usCondNumber];
		TrnSalesPLU(&CondimentWorkArea);
	}
	return SUCCESS;
}


/*
*===========================================================================
**Synopsis: SHORT   ItemPreviousItem( VOID *pReceiveItem, USHORT usTempBuffSize)
*
*    Input: VOID *pReceiveItem
*
*   Output: None
*
*    InOut: None
*
**Return: COMMON_SUCCESS    : Function Performed succesfully
*         LDT_TAKETL_ADR : Function aborted with Illegal operation sequense error
*
** Description: Send buffered previous item to transaction module and 
*               put current item into save buffer in common module local data area.
*                
*===========================================================================
*/
static VOID	ItemPreviousItemClear (VOID)
{
	memset(&ItemCommonLocal.ItemSales, 0, sizeof(ITEMSALES));
	memset(&ItemCommonLocal.ItemDisc, 0, sizeof(ITEMDISC));
}

static SHORT  ItemPreviousItemItemCommonLocal (SHORT bClearStore)
{
    SHORT           sReturnStatus = ITM_SUCCESS;

	switch(ItemCommonLocal.ItemSales.uchMajorClass) {//Analyze previous item by major class
    case CLASS_ITEMSALES:
        if (ItemSalesDiscountClassIsMod (ItemCommonLocal.ItemSales.uchMinorClass)) {
            /*--- MODIFIED DISC. PROCESS ---*/
            if ((sReturnStatus = ItemPrevModDisc()) != ITM_SUCCESS) {
                ItemCommonLocal.fbCommonStatus &= ~COMMON_VOID_EC;     /* turn off disable E.C bit */
                return(sReturnStatus);
            }
        }

        /*----- PRINT VALIDATION FOR PREVIOUS ITEM -----*/
        if (ItemCommonLocal.ItemSales.fsPrintStatus & PRT_VALIDATION) {
            ItemPrevSalesValPrint();
        }
        
        /*----- PRINT TICKET FOR PREVIOUS ITEM -----*/
		if (ItemCommonLocal.ItemSales.fsPrintStatus & PRT_SINGLE_RECPT) {
			if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) != CURQUAL_PRE_UNBUFFER) {
				ItemPrevSalesTicketPrint();
			} else {
				ItemCommonLocal.ItemSales.fsPrintStatus &= ~(PRT_SINGLE_RECPT | PRT_DOUBLE_RECPT);  /* tirn off ticket print status */
			}
		}
        break;
    
    case CLASS_ITEMDISC:
        if ((ItemCommonLocal.ItemSales.uchMinorClass == CLASS_CHARGETIP) ||
            (ItemCommonLocal.ItemSales.uchMinorClass == CLASS_CHARGETIP2) ||
            (ItemCommonLocal.ItemSales.uchMinorClass == CLASS_CHARGETIP3)) {
        
            /*----- PRINT TICKET FOR PREVIOUS ITEM -----*/
            if (((ITEMDISC *) &(ItemCommonLocal.ItemSales))->fsPrintStatus & PRT_SINGLE_RECPT) {
                if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) != CURQUAL_PRE_UNBUFFER) {
                    ItemPrevChargeTipTicketPrint((ITEMDISC *) &ItemCommonLocal.ItemSales);
                } else {
                    ((ITEMDISC *) &(ItemCommonLocal.ItemSales))->fsPrintStatus &= ~(PRT_SINGLE_RECPT | PRT_DOUBLE_RECPT);  /* turn off ticket status */
                }
            }
            if((sReturnStatus = ItemPrevChargeTipAffection((ITEMDISC *) &ItemCommonLocal.ItemSales)) != ITM_SUCCESS) {
                return(sReturnStatus);
            }
        }
        break;

    default:
        break;

    }

	if (bClearStore & 0x0001) {
		/*----- SEND PREVIOUS ITEM TO TRANSACTION MODULE -----*/
		if ((sReturnStatus = ItemPrevSendSalesAndDiscount()) != ITM_SUCCESS) {
			return(sReturnStatus);
		}
	} else if (bClearStore & 0x0002) {
		ItemPreviousItemClear ();
	}

    /*----- TURN OFF E.C DISABLE STATUS -----*/
    ItemCommonLocal.fbCommonStatus &= ~COMMON_VOID_EC;   /* turn off disable E.C bit */

    return(ITM_SUCCESS);    /* return after sending */    
}

// Create a condiment struct of type ITEMCOND from an ITEMSALES struct
// and the return the created condiment.
// there are other places where a condiment struct is created from
// other types of PLU or item data.
//
// See also function ItemSalesSetMenu().

ITEMCOND  ItemCondimentData(CONST ITEMSALES *pItemSales)
{
	ITEMCOND        CondimentData = { 0 };

	/*----- set condiment data to previouse item -----*/
	// The auchPLUNo is exactly sized to NUM_PLU_LEN hence is not treated as
	// a zero terminated character string.
	memcpy(CondimentData.auchPLUNo, pItemSales->auchPLUNo, sizeof(CondimentData.auchPLUNo));
	CondimentData.usDeptNo = pItemSales->usDeptNo;
	CondimentData.lUnitPrice = pItemSales->lUnitPrice;
	CondimentData.uchPrintPriority = pItemSales->uchPrintPriority; /* R3.0 */

	CondimentData.uchCondColorPaletteCode = pItemSales->uchColorPaletteCode; //color-palette-changes

	/* update module updates Cond. Ttl by using noun's uchAdjective */
	/* so don't set CondimentData.uchAdjective                      */
	CondimentData.uchAdjective = pItemSales->uchAdjective;

	// The aszMnemonic is exactly sized to NUM_DEPTPLU hence is not treated as
	// a zero terminated character string.
	memcpy(CondimentData.aszMnemonic, pItemSales->aszMnemonic, sizeof(CondimentData.aszMnemonic));
	memcpy(CondimentData.aszMnemonicAlt, pItemSales->aszMnemonicAlt, sizeof(CondimentData.aszMnemonicAlt));
	CondimentData.ControlCode = pItemSales->ControlCode;

	return CondimentData;
}

SHORT   ItemPreviousItemClearStore (SHORT bClearStore, VOID *pReceiveItem, USHORT usTempBuffSize)
{
    SHORT           sReturnStatus, sSize;
    SHORT           sCompSize;
    ITEMSALES       * const pCurrentItem = pReceiveItem;      /* pointer save buffer for casted receive item */
	USHORT			usCondNumber = 0, usScroll = 0, usRetFlag = FALSE;

    /*----- POINTER OF CURRENT ITEM IS 0 (Total or Tender Key Used) -----*/
    if (pCurrentItem == 0 || pCurrentItem->uchMajorClass == 0) {
		return ItemPreviousItemItemCommonLocal (bClearStore);
	}
    
  //POINTER OF CURRENT ITEM IS NON 0 (Item entry case) 
    /*----- CURRENT PRINT MOD., CONDIMENT MODIFIER -----*/
    switch(pCurrentItem->uchMajorClass) {
    case CLASS_UIFREGSALES:
    case CLASS_ITEMSALES:
        /*----- Print modifier, condiment edit process -----*/
        if (((pCurrentItem->uchMajorClass == CLASS_UIFREGSALES) && (pCurrentItem->uchMinorClass == CLASS_UIPRINTMODIFIER)) ||
            ((pCurrentItem->uchMajorClass == CLASS_ITEMSALES) && (pCurrentItem->ControlCode.auchPluStatus[2] & PLU_CONDIMENT)) ||
			 (pCurrentItem->uchMajorClass == CLASS_ITEMSALES) && (pCurrentItem->uchMinorClass == 0) ||  //catch #/Type entries Added US Customs cwunn 4/9/03
			 ((pCurrentItem->uchMajorClass == CLASS_UIFREGSALES) && (pCurrentItem->uchMinorClass == CLASS_UIFOR)) )  //UIFOR added SR 143 @/For Key cwunn
		{
			union {
				ITEMSALES   ItemSales;
				ITEMTOTAL   ItemTotal;
				ITEMTENDER  ItemTender;
			} PrevItemWork = {0};       /* work buffer */

			if ((pCurrentItem->uchMajorClass == CLASS_UIFREGSALES) && (pCurrentItem->uchMinorClass == CLASS_UIFOR)) {
				ItemPreviousGetForKeyDataItemSales (&(PrevItemWork.ItemSales));
			}
			else {
				/* copy ItemCommonLocal.ItemSales to PrevItemWork */ 
				PrevItemWork.ItemSales = ItemCommonLocal.ItemSales;
			}
	//US Customs cwunn 4/9/03
			if(pCurrentItem->aszNumber[0][0] != '\0'){ //if #/Type entry, copy number to the previous item.
				if((PrevItemWork.ItemSales.uchMajorClass == 0) && (PrevItemWork.ItemSales.uchMinorClass == 0)
					&& !(pCurrentItem->uchMinorClass == CLASS_UIFOR)){ //UIFOR added SR 143 @/For Key cwunn
					//Prohibit #/Type entry if nothing in previous item buffer
					return(LDT_PROHBT_ADR);
				}
				else {
					SHORT			numCounter = 0;			//US Customs

					switch (PrevItemWork.ItemSales.uchMajorClass) {
						case CLASS_ITEMTOTAL:
							memcpy(PrevItemWork.ItemTotal.aszNumber, pCurrentItem->aszNumber[0], sizeof(PrevItemWork.ItemTotal.aszNumber));
							break;

						default:
							for(numCounter = 0; numCounter < NUM_OF_NUMTYPE_ENT; numCounter++)
							{
								if(PrevItemWork.ItemSales.aszNumber[numCounter][0] != _T('\0')){ //empty slot found
									continue;
								}
								memcpy(PrevItemWork.ItemSales.aszNumber[numCounter], pCurrentItem->aszNumber[0], sizeof(PrevItemWork.ItemSales.aszNumber[numCounter]));
								break;
							}
							//if 6th entry attempt, return prohibited...only 5 allowed.
							if(numCounter >= NUM_OF_NUMTYPE_ENT) {
								return(LDT_PROHBT_ADR);
							}
							break;
					}
				}
			}
//End US Customs
			if (pCurrentItem->uchMinorClass == CLASS_UIPRINTMODIFIER) {
				UIFREGSALES     * const pCurPrintMod = pReceiveItem;      /* pointer save buffer for casted receive item */
        
				/* check consolidation mode or not */
				if (CliParaMDCCheck(MDC_PRINTPRI_ADR, EVEN_MDC_BIT0) && CliParaMDCCheck(MDC_PRINTPRI_ADR, EVEN_MDC_BIT2)) { /* R3.0 */
					if (PrevItemWork.ItemSales.uchPrintModNo == 0) {
						PrevItemWork.ItemSales.auchPrintModifier[0] = pCurPrintMod->uchPrintModifier;
					} else {
						UCHAR           *puchHitPoint;

						/*----- sort print modifier on PrevItemWork -----*/
						sReturnStatus = Rfl_SpBsearch(&pCurPrintMod->uchPrintModifier, 
													  sizeof(pCurPrintMod->uchPrintModifier),
													  PrevItemWork.ItemSales.auchPrintModifier,
													  PrevItemWork.ItemSales.uchPrintModNo,
													  &puchHitPoint,
													  pComp);
        
						if (sReturnStatus != RFL_TAIL_OF_TABLE) {
							memmove(puchHitPoint +1, puchHitPoint, 
									PrevItemWork.ItemSales.uchPrintModNo - (puchHitPoint - PrevItemWork.ItemSales.auchPrintModifier));
						}
						*puchHitPoint = pCurPrintMod->uchPrintModifier;
					}
				} else {
					int iIndex = PrevItemWork.ItemSales.uchPrintModNo + PrevItemWork.ItemSales.uchChildNo + PrevItemWork.ItemSales.uchCondNo;

					PrevItemWork.ItemSales.auchPrintModifier[iIndex] = pCurPrintMod->uchPrintModifier;
				}
				PrevItemWork.ItemSales.uchPrintModNo ++;

			} else {
				//if() added in US Customs
				if((pCurrentItem->uchMajorClass == CLASS_ITEMSALES) && (pCurrentItem->ControlCode.auchPluStatus[2] & PLU_CONDIMENT)) {
					ITEMCOND        CondimentData = {0};

#if 1
					CondimentData = ItemCondimentData(pCurrentItem);
#else
					/*----- set condiment data to previouse item -----*/
					// The auchPLUNo is exactly sized to NUM_PLU_LEN hence is not treated as
					// a zero terminated character string.
					memcpy(CondimentData.auchPLUNo, pCurrentItem->auchPLUNo, sizeof(CondimentData.auchPLUNo));          
					CondimentData.usDeptNo = pCurrentItem->usDeptNo;      
					CondimentData.lUnitPrice = pCurrentItem->lUnitPrice; 
					CondimentData.uchPrintPriority = pCurrentItem->uchPrintPriority; /* R3.0 */

					CondimentData.uchCondColorPaletteCode = pCurrentItem->uchColorPaletteCode; //color-palette-changes

					/* update module updates Cond. Ttl by using noun's uchAdjective */
					/* so don't set CondimentData.uchAdjective                      */
					// The aszMnemonic is exactly sized to NUM_DEPTPLU hence is not treated as
					// a zero terminated character string.
					memcpy(CondimentData.aszMnemonic, pCurrentItem->aszMnemonic, sizeof(CondimentData.aszMnemonic)); 
					memcpy(CondimentData.aszMnemonicAlt, pCurrentItem->aszMnemonicAlt, sizeof(CondimentData.aszMnemonicAlt)); 
					CondimentData.ControlCode = pCurrentItem->ControlCode;
#endif

					//Begin Condiment Addition/Editing/Deletion
					if (CliParaMDCCheck(MDC_DEPT3_ADR, ODD_MDC_BIT3)) //SR 192
					{
						USHORT		usReturn = 0;

						usReturn = ItemPreviousCondiment(&CondimentData, MLD_CONDIMENT_UPDATE);
						if(usReturn)
						{
							pCurrentItem->usUniqueID = usReturn;
							return ITM_PREVIOUS;
						}
					}

					/* check consolidation mode or not */
					if (CliParaMDCCheck(MDC_PRINTPRI_ADR, EVEN_MDC_BIT0) && CliParaMDCCheck(MDC_PRINTPRI_ADR, EVEN_MDC_BIT2)) { /* R3.0 */
						if(PrevItemWork.ItemSales.uchCondNo + PrevItemWork.ItemSales.uchChildNo == 0) {
							PrevItemWork.ItemSales.Condiment[0] = CondimentData;
						} else {
							UCHAR           *puchHitPoint;

							/*----- Sort Condiment by Print Priority, R3.0 -----*/
							sReturnStatus = ItemCommonSort(pCurrentItem, &PrevItemWork.ItemSales, &puchHitPoint);
							if (sReturnStatus == 1) {
								memmove(puchHitPoint + sizeof(ITEMCOND),
										puchHitPoint, 
										( USHORT)(PrevItemWork.ItemSales.uchCondNo + PrevItemWork.ItemSales.uchChildNo)
											* sizeof(ITEMCOND)
											- (puchHitPoint - (UCHAR *)PrevItemWork.ItemSales.Condiment));
							}
							memcpy(puchHitPoint, &CondimentData, sizeof(ITEMCOND));
						}    
					} else {
						int iIndex = PrevItemWork.ItemSales.uchCondNo + PrevItemWork.ItemSales.uchPrintModNo + PrevItemWork.ItemSales.uchChildNo;

						PrevItemWork.ItemSales.Condiment[iIndex] = CondimentData;
					}
					//end if(ITEMSALES && PLU_CONDIMENT)
					PrevItemWork.ItemSales.uchCondNo++;
				}			
				else { //SR 143 @/For Key cwunn
					if((pCurrentItem->uchMajorClass == CLASS_UIFREGSALES) &&
						pCurrentItem->uchMinorClass == CLASS_UIFOR) {
						PrevItemWork.ItemSales.uchMajorClass = CLASS_ITEMSALES;//pCurrentItem->uchMajorClass;
						PrevItemWork.ItemSales.uchMinorClass = pCurrentItem->uchMinorClass;
						PrevItemWork.ItemSales.lQTY = pCurrentItem->lQTY;
						PrevItemWork.ItemSales.lUnitPrice = pCurrentItem->lUnitPrice;
						PrevItemWork.ItemSales.usFor	= pCurrentItem->usFor;
						PrevItemWork.ItemSales.usForQty = pCurrentItem->usForQty;
						PrevItemWork.ItemSales.lProduct = pCurrentItem->lProduct;
						PrevItemWork.ItemSales.usUniqueID	= pCurrentItem->usUniqueID;
						ItemPrevSendSales();
						
					}
				}//end else SR 143
			}

			/*----- check strage size remaining -----*/
			if ((sCompSize = ItemCommonCheckSize(&PrevItemWork.ItemSales, 0)) < 0) {
				ITEMMODIFIER    DummyModifier = {0};

				ItemSalesCalcECCom(&PrevItemWork.ItemSales);                  /* R3.1 */
				memset(&ItemCommonLocal.ItemSales, 0, sizeof(ITEMSALES)); /* clear ItemCommonLocal.ItemDisc */
				ItemCommonLocal.usDiscBuffSize = 0;
				ItemCommonLocal.uchItemNo = 0;

				MldECScrollWrite();                                 /* R3.0 */
				ItemCommonDispECSubTotal(&DummyModifier);           /* Saratoga */

				return(LDT_TAKETL_ADR);
			} 
            ItemCommonLocal.ItemSales = PrevItemWork.ItemSales;
            ItemCommonLocal.usSalesBuffSize = sCompSize;
            ItemCommonLocal.ItemSales.fsPrintStatus &= ~PRT_SPCL_PRINT;
            
			return(ITM_SUCCESS);
        } 

        break;

    case CLASS_ITEMDISC:
        if (pCurrentItem->uchMinorClass == CLASS_ITEMDISC1) {
            /*----- PRINT VALIDATION FOR PREVIOUS ITEM -----*/
            if (ItemCommonLocal.ItemSales.fsPrintStatus & PRT_VALIDATION) {
                ItemPrevSalesValPrint();
            }
            
            /*----- PRINT TICKET FOR PREVIOUS ITEM -----*/
            if (ItemCommonLocal.ItemSales.fsPrintStatus & PRT_SINGLE_RECPT) {
                if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) != CURQUAL_PRE_UNBUFFER) {
                    ItemPrevSalesTicketPrint();
                } else {
                    ItemCommonLocal.ItemSales.fsPrintStatus &= ~(PRT_SINGLE_RECPT | PRT_DOUBLE_RECPT);  /* tirn off ticket print status */
                }
            }

			if ((((ITEMDISC *)pCurrentItem)->fbDiscModifier & VOID_MODIFIER) == 0) {
				memcpy(&ItemCommonLocal.ItemDisc, (ITEMDISC *)pCurrentItem, sizeof(ITEMDISC));
				ItemCommonLocal.uchItemNo ++;
				ItemCommonLocal.usDiscBuffSize = usTempBuffSize;
    
				/*----- TURN OFF E.C DISABLE STATUS -----*/
				ItemCommonLocal.fbCommonStatus &= ~COMMON_VOID_EC;   /* turn off disable E.C bit */
				return(ITM_SUCCESS);    /* don't move current item in case of Item disc.1 */
			}
        }
        break;  /* not use */

    case CLASS_ITEMMODIFIER:
         /* if minor classes of current modifier is identical with previous one, */ 
         /* override previous modifier with current total                        */
            
        if (ItemCommonLocal.ItemSales.uchMajorClass == CLASS_ITEMMODIFIER &&
            (ItemCommonLocal.ItemSales.uchMinorClass == pCurrentItem->uchMinorClass)) { 

            sSize = RflGetStorageItemClassLen(pCurrentItem);        /* Saratoga */
            memcpy(&ItemCommonLocal.ItemSales, pCurrentItem, sSize);
            ItemCommonLocal.usSalesBuffSize = usTempBuffSize;

            /*----- TURN OFF E.C DISABLE STATUS -----*/
            ItemCommonLocal.fbCommonStatus &= ~COMMON_VOID_EC;   /* turn off disable E.C bit */
            return(ITM_SUCCESS);
        }

		if(pCurrentItem->uchMinorClass == CLASS_ALPHANAME)
		{
			SHORT			numCounter = 0;			//US Customs
			ITEMMODIFIER    *pItemMod = (ITEMMODIFIER *)pCurrentItem;			//US Customs
			union {
				ITEMSALES   ItemSales;
				ITEMTOTAL   ItemTotal;
				ITEMTENDER  ItemTender;
			} PrevItemWork;       /* work buffer */

            memset(&PrevItemWork, 0, sizeof(PrevItemWork)); /* clear work structure */
            /* copy ItemCommonLocal.ItemSales to PrevItemWork */ 
	//US Customs JHHJ 7/14/05

			if(pItemMod->aszName != '\0'){ //if #/Type entry, copy number to the previous item.
					if((ItemCommonLocal.ItemSales.uchMajorClass == '\0') && (ItemCommonLocal.ItemSales.uchMinorClass == '\0')
					&& !(pCurrentItem->uchMinorClass == CLASS_UIFOR)){ //UIFOR added SR 143 @/For Key cwunn
					//Prohibit #/Type entry if nothing in previous item buffer
					break;
				}
				else {
					for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++)
					{
						if(ItemCommonLocal.ItemSales.aszNumber[numCounter][0] != _T('\0')){ //empty slot found
							continue;
						}
						memcpy(ItemCommonLocal.ItemSales.aszNumber[numCounter], pItemMod->aszName, sizeof(ItemCommonLocal.ItemSales.aszNumber[numCounter]));
						break;
					}
				}
				//if 6th entry attempt, return prohibited...only 5 allowed.
				if(numCounter == NUM_OF_NUMTYPE_ENT) {
					return(LDT_PROHBT_ADR);
				}

				return(ITM_SUCCESS);
			}
//End US Customs
		}
        break;  /* not use */

    case CLASS_UIFREGMISC:
    case CLASS_ITEMMISC:
        if ((pCurrentItem->uchMinorClass == CLASS_UICANCEL) || (pCurrentItem->uchMinorClass == CLASS_CANCEL)) {
            /*--- NOT PRINT VALIDATION TICKET ---*/
            switch(ItemCommonLocal.ItemSales.uchMajorClass) {
            case CLASS_ITEMSALES:
                ((ITEMSALES *) &ItemCommonLocal.ItemSales)->fsPrintStatus &= ~(PRT_VALIDATION | PRT_SINGLE_RECPT | PRT_DOUBLE_RECPT);  
                                            /* turn off validation, ticket print status */
                break;

            case CLASS_ITEMDISC:
                ((ITEMDISC *) &ItemCommonLocal.ItemSales)->fsPrintStatus &= ~(PRT_VALIDATION | PRT_SINGLE_RECPT | PRT_DOUBLE_RECPT);  
                                            /* turn off validation, ticket print status */
                break;

            case CLASS_ITEMTOTAL:
                memset(&ItemCommonLocal.ItemSales, 0, sizeof(ITEMSALES));  /* R3.0 */
                break;

            default:
                break;

            }

        /* --- Saratoga */
        } else if (pCurrentItem->uchMinorClass == CLASS_MONEY) {
            ItemCommonLocal.fbCommonStatus &= ~COMMON_VOID_EC;
        }
        break;  

    default:
        break;
    }

    /*----- PREVIOUS ITEM PROCESSING -----*/
    switch(ItemCommonLocal.ItemSales.uchMajorClass) {
    case 0:                                 /* there is no previous item */ 
        /* only move current item to previous item buffer */
        sSize = RflGetStorageItemClassLen(pCurrentItem);        /* Saratoga */
        memcpy(&ItemCommonLocal.ItemSales, pCurrentItem, sSize);
        ItemCommonLocal.usSalesBuffSize = usTempBuffSize;
        ItemCommonLocal.uchItemNo ++;

        /*----- TURN OFF E.C DISABLE STATUS -----*/
        ItemCommonLocal.fbCommonStatus &= ~COMMON_VOID_EC;   /* turn off disable E.C bit */
        return(ITM_SUCCESS);    

    case CLASS_ITEMSALES:
        if (ItemSalesDiscountClassIsMod (ItemCommonLocal.ItemSales.uchMinorClass)) {
            /*--- MODIFIED DISC. PROCESS ---*/
            if ((sReturnStatus = ItemPrevModDisc()) != ITM_SUCCESS) {
                ItemCommonLocal.fbCommonStatus &= ~COMMON_VOID_EC;     /* turn off disable E.C bit */
                return(sReturnStatus);
            }
        }

        /*----- PRINT VALIDATION FOR PREVIOUS ITEM -----*/
        if (ItemCommonLocal.ItemSales.fsPrintStatus & PRT_VALIDATION) {
            ItemPrevSalesValPrint();
        }
        
        /*----- PRINT TICKET FOR PREVIOUS ITEM -----*/
        if (ItemCommonLocal.ItemSales.fsPrintStatus & PRT_SINGLE_RECPT) {
		    if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) != CURQUAL_PRE_UNBUFFER) {
                ItemPrevSalesTicketPrint();
			} else {
                ItemCommonLocal.ItemSales.fsPrintStatus &= ~(PRT_SINGLE_RECPT | PRT_DOUBLE_RECPT);  /* tirn off ticket print status */
            }
        }
        break; 

    case CLASS_ITEMDISC:
        if ((ItemCommonLocal.ItemSales.uchMinorClass == CLASS_CHARGETIP) ||
            (ItemCommonLocal.ItemSales.uchMinorClass == CLASS_CHARGETIP2) ||
            (ItemCommonLocal.ItemSales.uchMinorClass == CLASS_CHARGETIP3)) {

            /*----- PRINT TICKET FOR PREVIOUS ITEM -----*/
            if (((ITEMDISC *) &(ItemCommonLocal.ItemSales))->fsPrintStatus & PRT_SINGLE_RECPT) {
                if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) != CURQUAL_PRE_UNBUFFER) {
                    ItemPrevChargeTipTicketPrint((ITEMDISC *) &(ItemCommonLocal.ItemSales));
                } else {
                    ((ITEMDISC *) &(ItemCommonLocal.ItemSales))->fsPrintStatus &= ~(PRT_SINGLE_RECPT | PRT_DOUBLE_RECPT);  /* turn off ticket status */
                }
            }
            if((sReturnStatus = ItemPrevChargeTipAffection((ITEMDISC *) &(ItemCommonLocal.ItemSales))) != ITM_SUCCESS) {
                return(sReturnStatus);
            }
        }
        break;

    case CLASS_ITEMTOTAL:
        if ((sReturnStatus = ItemPrevTotalProc(pReceiveItem, usTempBuffSize)) != ITM_PERFORM) {
            /*----- TURN OFF E.C DISABLE STATUS -----*/
            ItemCommonLocal.fbCommonStatus &= ~COMMON_VOID_EC;   /* turn off disable E.C bit */

            return(sReturnStatus);
        }
        break;    

    case CLASS_ITEMMISC:            /* Saratoga */
        if (ItemCommonLocal.ItemSales.uchMinorClass == CLASS_MONEY) {
            TrnItem(&ItemCommonLocal.ItemSales);
            memcpy(&ItemCommonLocal.ItemSales, pCurrentItem, sizeof(ITEMMISC));
            return(ITM_SUCCESS);
        }
        break;   

    default:
        break;
        
    }

	if (bClearStore & 0x0001) {
		/*----- SEND PREVIOUS SALES TO TRANSACTION MODULE -----*/
		if ((sReturnStatus = ItemPrevSendSalesAndDiscount()) != ITM_SUCCESS) {
			return(sReturnStatus);
		}
	} else if (bClearStore & 0x0002) {
		ItemPreviousItemClear ();
	}

    /*----- TURN OFF E.C DISABLE STATUS -----*/
    ItemCommonLocal.fbCommonStatus &= ~COMMON_VOID_EC;   /* turn off disable E.C bit */

    /*----- move current item to previous item buffer -----*/
    memset(&ItemCommonLocal.ItemSales, 0, sizeof(ITEMSALES));
    sSize = RflGetStorageItemClassLen(pCurrentItem);        /* Saratoga */
    memcpy(&ItemCommonLocal.ItemSales, pCurrentItem, sSize);

    ItemCommonLocal.uchItemNo++;
    ItemCommonLocal.usSalesBuffSize = usTempBuffSize;

    return(ITM_SUCCESS);
}

SHORT   ItemPreviousItem(VOID *pReceiveItem, USHORT usTempBuffSize)
{
	return ItemPreviousItemClearStore (1, pReceiveItem, usTempBuffSize);
}
/*
*===========================================================================
**Synopsis: SHORT   ItemPreviousItemSpl(VOID *pItem, SHORT sType)
*
*    Input: 
*   Output: None
*    InOut: None
**Return: 
*
** Description:     Buffering Item Temporary.                  R3.1
*                   Handle the adding of items to a split transaction by copying
*                   an item into the temporary or working buffer and by moving
*                   any existing item currently in the working buffer into the
*                   transaction storage.
*===========================================================================
*/
SHORT   ItemPreviousItemSpl(CONST VOID *pItem, SHORT sType)
{
    SHORT                    sSplType, sSize;
    CONST ITEMGENERICHEADER  * CONST pItemHeader = pItem;

    if (sType == 0) {
        sSplType = TRN_TYPE_SPLITA;
    } else {
        sSplType = sType;
    }

    /*----- Set "Perform R/D Once" Bit for Multi Split -----*/
    if (pItemHeader && pItemHeader->uchMajorClass == CLASS_ITEMDISC) {
        if (pItemHeader->uchMinorClass != CLASS_ITEMDISC1) {
			TRANGCFQUAL     *WorkGCF;

            TrnGetGCFQualSpl(&WorkGCF, TRN_TYPE_SPLITA);
            WorkGCF->fsGCFStatus2 |= GCFQUAL_REGULAR_SPL;
        }
    }

    if (pItemHeader == 0 || pItemHeader->uchMajorClass == 0) {
		// nothing to put into the working buffer however we do need to commit any
		// pending item that is in the working buffer.
        if (ItemCommonLocal.uItemSalesHeader.uchMajorClass != 0) {
			ITEMDISC   *pDisc = (VOID *)&ItemCommonLocal.ItemSales;

            /* modify charge tips storage control 1/8/98 */
            if (ItemCommonLocal.uItemSalesHeader.uchMajorClass  == CLASS_ITEMDISC) {
                if ((pDisc->uchMinorClass == CLASS_CHARGETIP) ||
                    (pDisc->uchMinorClass == CLASS_CHARGETIP2) ||
                    (pDisc->uchMinorClass == CLASS_CHARGETIP3)) {

                    if (pDisc->fsPrintStatus & PRT_SINGLE_RECPT) {
                        ItemPrevChargeTipTicketPrint(pDisc);
                    }
                    ItemPrevChargeTipAffection(pDisc);
                }
            }

            TrnItemSpl(&ItemCommonLocal.ItemSales, sSplType);
            if (ItemCommonLocal.uItemSalesHeader.uchMajorClass == CLASS_ITEMDISC) {
                pDisc->fbStorageStatus = NOT_CONSOLI_STORAGE;
                TrnStorage(pDisc);                              // (ITEMDISC *)&ItemCommonLocal.ItemSales;

				/*-- save auto charge tips for add on sales case, V3.3 --*/
                if ((pDisc->uchMinorClass == CLASS_AUTOCHARGETIP) ||
                    (pDisc->uchMinorClass == CLASS_AUTOCHARGETIP2) ||
                    (pDisc->uchMinorClass == CLASS_AUTOCHARGETIP3))
				{
                    ItemCommonLocal.ItemDisc = *pDisc;
                    ItemCommonLocal.usDiscBuffSize = ItemCommonLocal.usSalesBuffSize;
                }
            }
            memset(&ItemCommonLocal.ItemSales, 0, sizeof(ITEMSALES));
            ItemCommonLocal.uchItemNo--;
            ItemCommonLocal.fbCommonStatus &= ~COMMON_VOID_EC;
        }
    } else {
		// we are now to put a new item into the temporary or working buffer.
		// we will only put certain types of items into the temporary or working buffer
		// since we are splitting a transaction.
		// also we will only move certain classes of items from the working buffer into storage.
        switch (pItemHeader->uchMajorClass) {
        case    CLASS_ITEMDISC:
            if (ItemCommonLocal.uItemSalesHeader.uchMajorClass == CLASS_ITEMDISC) {
				ITEMDISC  *pDisc = (ITEMDISC *)&ItemCommonLocal.ItemSales;
                TrnItemSpl(pDisc, sSplType);                   // (ITEMDISC *)&ItemCommonLocal.ItemSales;
                pDisc->fbStorageStatus = NOT_CONSOLI_STORAGE;
                TrnStorage(pDisc);                             // (ITEMDISC *)&ItemCommonLocal.ItemSales;
                ItemCommonLocal.uchItemNo--;
            }
            sSize = RflGetStorageItemClassLen(pItem);           /* Saratoga */
            memcpy(&ItemCommonLocal.ItemSales, pItem, sSize);
            ItemCommonLocal.uchItemNo++;
            ItemCommonLocal.fbCommonStatus &= ~COMMON_VOID_EC;

			TranCurQualTotalStatusClear ();   // clear total key status
            break;

        case    CLASS_ITEMTOTAL:
            if (ItemCommonLocal.uItemSalesHeader.uchMajorClass == CLASS_ITEMDISC) {
				ITEMDISC  *pDisc = (ITEMDISC *)&ItemCommonLocal.ItemSales;

                /* modify charge tips storage control 1/8/98 */
                if ((pDisc->uchMinorClass == CLASS_CHARGETIP) ||
                    (pDisc->uchMinorClass == CLASS_CHARGETIP2) ||
                    (pDisc->uchMinorClass == CLASS_CHARGETIP3)) {

                    if (pDisc->fsPrintStatus & PRT_SINGLE_RECPT) {
                        ItemPrevChargeTipTicketPrint(pDisc);
                    }
                    ItemPrevChargeTipAffection(pDisc);
                }

                TrnItemSpl(pDisc, sSplType);
                pDisc->fbStorageStatus = NOT_CONSOLI_STORAGE;
                TrnStorage(pDisc);

                /*-- save auto charge tips for add on sales case, V3.3 --*/
                if ((pDisc->uchMinorClass == CLASS_AUTOCHARGETIP) ||
                    (pDisc->uchMinorClass == CLASS_AUTOCHARGETIP2) ||
                    (pDisc->uchMinorClass == CLASS_AUTOCHARGETIP3)) {

                    ItemCommonLocal.ItemDisc = *pDisc;
                    ItemCommonLocal.usDiscBuffSize = ItemCommonLocal.usSalesBuffSize;
                }

                memset(&ItemCommonLocal.ItemSales, 0, sizeof(ITEMSALES));
                ItemCommonLocal.uchItemNo--;
            }
            if (pItemHeader->uchMinorClass != CLASS_SPLIT) {
                sSize = RflGetStorageItemClassLen(pItem);           /* Saratoga */
                memcpy(&ItemCommonLocal.ItemSales, pItem, sSize);
				ItemCommonLocal.usSalesBuffSize = sSize;
            }
            ItemCommonLocal.fbCommonStatus &= ~COMMON_VOID_EC;
            break;

        case    CLASS_ITEMTENDER:
            if (ItemCommonLocal.uItemSalesHeader.uchMajorClass != 0) {
				// commit the pending item to prepare for this item to put into working buffer.
                TrnItemSpl(&ItemCommonLocal.ItemSales, sSplType);
                ItemCommonLocal.uchItemNo--;
            }
            sSize = RflGetStorageItemClassLen(pItem);           /* Saratoga */
            memcpy(&ItemCommonLocal.ItemSales, pItem, sSize);
            ItemCommonLocal.uchItemNo++;
            ItemCommonLocal.fbCommonStatus &= ~COMMON_VOID_EC;
            break;

        default:
            break;
        }
    }

    return(ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: VOID    ItemPrevSalesValPrint( VOID ); 
*
*    Input: Nothing
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return: VOID 
*         
*
** Description: Print validation of previous voided item sales. 
*                       
*===========================================================================
*/

VOID    ItemPrevSalesValPrint( VOID )
{ 
    USHORT  fsPrintStatusSave = ItemCommonLocal.ItemSales.fsPrintStatus;   /* store print status */

    ItemCommonLocal.ItemSales.fsPrintStatus  &= (PRT_VALIDATION | PRT_MANUAL_WEIGHT);
    TrnThrough(&ItemCommonLocal.ItemSales); /* print validation (VOID function) */
        
    ItemCommonLocal.ItemSales.fsPrintStatus  = (fsPrintStatusSave & ~PRT_VALIDATION);  /* restore print status */
}

/*
*===========================================================================
**Synopsis: VOID    ItemPrevSalesTicketPrint( VOID ); 
*
*    Input: Nothing
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return: VOID
*         
*
** Description: Print ticket of previous sales.
*                       
*===========================================================================
*/

VOID    ItemPrevSalesTicketPrint( VOID )
{        
	USHORT fsPrintStatusSave = ItemCommonLocal.ItemSales.fsPrintStatus;   /* store print status */

    ItemCommonLocal.ItemSales.fsPrintStatus &= (PRT_SINGLE_RECPT | PRT_MANUAL_WEIGHT);
    ItemPrevTicketHeader(ItemCommonLocal.ItemSales.fsPrintStatus);
    TrnThrough(&ItemCommonLocal.ItemSales); /* send sales data for ticket print */
        
    if (fsPrintStatusSave & PRT_DOUBLE_RECPT) {          /* issue double receipt ? */
        ItemTrailer(TYPE_DOUBLE_TICKET);    /* print trailer and header for double receipt */
        ItemPrevTicketHeader(ItemCommonLocal.ItemSales.fsPrintStatus);
        TrnThrough(&ItemCommonLocal.ItemSales);  /* issue double receipt */ 
    } 
    
    ItemTrailer(TYPE_SINGLE_TICKET);        /* print trailer for single receipt */
        
    ItemCommonLocal.ItemSales.fsPrintStatus = (fsPrintStatusSave & ~(PRT_SINGLE_RECPT | PRT_DOUBLE_RECPT));  /* restore print status */
}    

/*
*===========================================================================
**Synopsis: VOID    ItemTicketHeader(USHORT fsPrintStatus)
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
*               module.  
*                
*===========================================================================
*/

VOID    ItemPrevTicketHeader(USHORT fsPrtStat)
{
    TRANGCFQUAL    * const GCFQualRcvBuff = TrnGetGCFQualPtr();

    if (TranCurQualPtr->fsCurStatus & CURQUAL_TRAINING) {
		ITEMPRINT      ItemPrintData = {0};

        ItemPrintData.uchMajorClass = CLASS_ITEMPRINT;      /* class for print data */
        ItemPrintData.uchMinorClass = CLASS_PRINT_TRAIN;
        ItemPrintData.fsPrintStatus = (fsPrtStat | PRT_RECEIPT);
        TrnThrough(&ItemPrintData);         /* stored print of promotion header */                                        
    }

    if (TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)) {
		ITEMPRINT      ItemPrintData = {0};

        ItemPrintData.uchMajorClass = CLASS_ITEMPRINT;      /* class for print data */
		if (TranCurQualPtr->fsCurStatus & CURQUAL_PVOID)
			ItemPrintData.uchMinorClass = CLASS_PRINT_PVOID;

		if (TranCurQualPtr->fsCurStatus & (CURQUAL_TRETURN | CURQUAL_PRETURN))
			ItemPrintData.uchMinorClass = CLASS_PRINT_TRETURN;

        ItemPrintData.fsPrintStatus = (fsPrtStat | PRT_RECEIPT);
        TrnThrough(&ItemPrintData);         /* real time print of promotion header */                                        
    }

    if (GCFQualRcvBuff->aszName[0] != '\0') {
		ITEMMODIFIER   ItemModifier = {0};

        ItemModifier.uchMajorClass = CLASS_ITEMMODIFIER; 
        ItemModifier.uchMinorClass = CLASS_ALPHANAME;
        ItemModifier.fsPrintStatus = (fsPrtStat | PRT_RECEIPT);

        _tcsncpy (ItemModifier.aszName, GCFQualRcvBuff->aszName, NUM_NAME);
        TrnThrough(&ItemModifier);
    }

    if (TranCurQualPtr->flPrintStatus & CURQUAL_UNIQUE_TRANS_NO) {
		ITEMMODIFIER   ItemModifier = {0};

        ItemModifier.uchMajorClass = CLASS_ITEMMODIFIER; 
        ItemModifier.uchMinorClass = CLASS_TABLENO;
        ItemModifier.usTableNo     = GCFQualRcvBuff->usTableNo;
        ItemModifier.fsPrintStatus = (fsPrtStat | PRT_RECEIPT);

        TrnThrough(&ItemModifier);
    }
}                                       


/*
*===========================================================================
**Synopsis: VOID    ItemPrevChargeTipTicketPrint(VOID); 
*
*    Input: 
*
*   Output: None
*
*    InOut: None
*
**Return: 
*         
*
** Description: Print ticket for previous Charge Tip. V3.3
*                       
*===========================================================================
*/

VOID    ItemPrevChargeTipTicketPrint(ITEMDISC *pDisc)
{
	USHORT fsPrintStatusSave = pDisc->fsPrintStatus;  /* store print status */
    pDisc->fsPrintStatus = PRT_SINGLE_RECPT;
    ItemPrevTicketHeader(pDisc->fsPrintStatus);
    TrnThrough(pDisc);                                /* send sales data for single receipt */
        
    if (fsPrintStatusSave & PRT_DOUBLE_RECPT) {       /* issue double receipt */
        ItemTrailer(TYPE_DOUBLE_TICKET);              /* print trailer and header for double receipt */
        ItemPrevTicketHeader(pDisc->fsPrintStatus);
        TrnThrough(pDisc);                            /* issue double receipt requested */ 
    }

    ItemTrailer(TYPE_SINGLE_TICKET);                  /* print trailer for single receipt */
    
    pDisc->fsPrintStatus = (fsPrintStatusSave & ~(PRT_SINGLE_RECPT |PRT_DOUBLE_RECPT));
}    


/*
*===========================================================================
**Synopsis: SHORT   ItemPrevSendSales(VOID)
*    Input: Nothing
*   Output: Nothing
*    InOut: Nothing
*
**Return:   ITM_SUCCESS
*         
** Description: Send buffered item except Item discount at the next item.
                V3.3: Void Supervisor Intervention when no item found 
                      in current transaction (item) buffer.
*===========================================================================
*/
SHORT   ItemPrevSendSales(VOID)
{
    SHORT               sStatus, sQty;
    TRANINFORMATION     * const WorkTran = TrnGetTranInformationPointer();

    /* --- Void Super Int.  V3.3 --- */
/*
    if ((WorkTran->TranCurQual.flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK &&
        CliParaMDCCheck(MDC_NEWKEYSEQ_ADR, ODD_MDC_BIT2) &&
        (ItemCommonLocal.ItemSales.fbModifier & VOID_MODIFIER)) { */
        
    if ((WorkTran->TranCurQual.flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK &&
        CliParaMDCCheck(MDC_NEWKEYSEQ_ADR, ODD_MDC_BIT2)) {
        if ((ItemCommonLocal.ItemSales.uchMajorClass == CLASS_ITEMSALES) &&
            (ItemCommonLocal.ItemSales.fbModifier & VOID_MODIFIER))
		{
			ITEMSALES           Sales;
            
            Sales = ItemCommonLocal.ItemSales;
            Sales.lQTY *= -1L;
            if (Sales.uchPPICode == 0) {
                Sales.lProduct *= -1L;
            } else {
                if (TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK) {
                    Sales.lProduct *= -1L;
                }
            }
            Sales.sCouponQTY *= -1;
            Sales.lDiscountAmount *= -1L;
            if (ItemSalesDiscountClassIsMod (Sales.uchMinorClass) ||
                ItemSalesDiscountClassIsItem (Sales.uchMinorClass) ||
                (Sales.ControlCode.auchPluStatus[2] & PLU_SCALABLE) )
			{
                sQty = 1;
            } else {
                sQty = (SHORT)(Sales.lQTY / 1000L);
            }
            Sales.fbModifier &= ~VOID_MODIFIER;

            if ((sStatus = TrnVoidSearch(&Sales, sQty, TRN_TYPE_ITEMSTORAGE)) != TRN_SUCCESS) {
                if (ItemSPVInt(LDT_SUPINTR_ADR) != ITM_SUCCESS) {
                    memset(&ItemCommonLocal.ItemSales, 0, sizeof(ITEMSALES));
                    ItemCommonLocal.usSalesBuffSize = 0;
                    ItemCommonLocal.uchItemNo--;
                    UieErrorMsg(LDT_VOID_NOALLOW_ADR, UIE_WITHOUT);
                    MldECScrollWrite();
                    ItemSalesCalcECCom( &ItemCommonLocal.ItemSales);    /* saratoga */

                    /* send to enhanced kds, 2172 */
                    ItemSendKds(&ItemCommonLocal.ItemSales, 1);
    
                    /* ----- send link plu to kds, 2172 ----- */
                    ItemCommonLocal.ItemSales.fbModifier |= VOID_MODIFIER;
                    ItemSalesLinkPLU(&ItemCommonLocal.ItemSales, 2);
        
                    return(ITM_SUCCESS);
                }
            }
        }
    }

    if ((sStatus = TrnItem(&ItemCommonLocal.ItemSales)) != TRN_SUCCESS ) {
        return(sStatus);
    }

    /*----- V3.3 -----*/
    if ((ItemCommonLocal.ItemSales.uchMajorClass == CLASS_ITEMDISC) 
        && ((ItemCommonLocal.ItemSales.uchMinorClass == CLASS_AUTOCHARGETIP) ||
            (ItemCommonLocal.ItemSales.uchMinorClass == CLASS_AUTOCHARGETIP2) ||
            (ItemCommonLocal.ItemSales.uchMinorClass == CLASS_AUTOCHARGETIP3)))
	{
        /*-- save auto charge tips for add on sales case          --*/
        /*-- not display auto charge tips at total key depression --*/
        memcpy(&ItemCommonLocal.ItemDisc, &ItemCommonLocal.ItemSales, sizeof(ITEMDISC));
        ItemCommonLocal.usDiscBuffSize = ItemCommonLocal.usSalesBuffSize;
    } else {
        MldScrollFileWriteSpl(&ItemCommonLocal.ItemSales);
    }

    memset(&ItemCommonLocal.ItemSales, 0, sizeof(ITEMSALES));   /* clear ItemCommonLocal.ItemSales */
    ItemCommonLocal.usSalesBuffSize = 0;
    ItemCommonLocal.uchItemNo--;
        
    return(ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemPrevSendDisc( VOID )
*
*    Input: Nothing
*
*   Output: None
*
*    InOut: None
*
**Return: ITM_SUCCESS
*         
*
** Description: Send buffered Item Discount at the next item. 
*                
*                
*===========================================================================
*/
SHORT   ItemPrevSendDisc( VOID ) 
{
    SHORT  sReturnStatus;

    /*----- V3.3 -----*/
    if ((ItemCommonLocal.ItemDisc.uchMajorClass == CLASS_ITEMDISC) 
        && ((ItemCommonLocal.ItemDisc.uchMinorClass == CLASS_AUTOCHARGETIP) ||
            (ItemCommonLocal.ItemDisc.uchMinorClass == CLASS_AUTOCHARGETIP2) ||
            (ItemCommonLocal.ItemDisc.uchMinorClass == CLASS_AUTOCHARGETIP3))) {

        /* not send auto charge tips class (inner use only) */
        return(ITM_SUCCESS);
    }
    if ((sReturnStatus = TrnItem(&ItemCommonLocal.ItemDisc)) != TRN_SUCCESS ) {
        return(sReturnStatus);
    }
    /*----- R3.0 -----*/
    MldScrollFileWrite(&ItemCommonLocal.ItemDisc);
        
    memset(&ItemCommonLocal.ItemDisc, 0, sizeof(ITEMDISC)); /* clear ItemCommonLocal.ItemDisc */
    
    ItemCommonLocal.usDiscBuffSize = 0;
    
    ItemCommonLocal.uchItemNo --;

    return(ITM_SUCCESS);

}

SHORT   ItemPrevSendSalesAndDiscount(VOID)
{
	SHORT  sReturnStatus = ITM_SUCCESS;

	do {
		/*----- SEND PREVIOUS SALE TO TRANSACTION MODULE -----*/
		if (ItemCommonLocal.ItemSales.uchMajorClass != 0) {
			if (ItemCommonLocal.ItemDisc.uchMajorClass != 0) {
				// if there is a discount associated with this item then we need to
				// differentiate this item from other discounted items.
				ItemCommonLocal.ItemSales.fsLabelStatus |= ITM_CONTROL_NO_CONSOL;
			}
			if ((sReturnStatus = ItemPrevSendSales()) != ITM_SUCCESS) {
				// if error then bail out and return.
				break;
			}
		}
	    
		/*----- SEND PREVIOUS DISCOUNT TO TRANSACTION MODULE -----*/
		if (ItemCommonLocal.ItemDisc.uchMajorClass != 0) { 
			if ((sReturnStatus = ItemPrevSendDisc())  != ITM_SUCCESS) {
				// if error then bail out and return.
				break;
			}
		}
	} while (0);  // do the loop only once.

	return sReturnStatus;
}


/*
	This is a special purpose function that is used only with returns.
	It is designed to update the return transaction with any transaction
	discount data that needs to be used to modify the return amount to
	be given to the customer.
**/
SHORT  ItemProcessReturnsDiscount (ITEMDISC  *pItemDisc)
{
	DCURRENCY   lItemizer = 0;
	DCURRENCY   lVoidDiscount = 0;
	SHORT       sReturnStatus = 0;
    UCHAR       uchRoundType = 0;

	switch (pItemDisc->uchMinorClass) {
		case CLASS_REGDISC1:       // ItemProcessReturnsDiscount() allowed discount type
		case CLASS_REGDISC2:       // ItemProcessReturnsDiscount() allowed discount type
		case CLASS_REGDISC3:       // ItemProcessReturnsDiscount() allowed discount type
		case CLASS_REGDISC4:       // ItemProcessReturnsDiscount() allowed discount type
		case CLASS_REGDISC5:       // ItemProcessReturnsDiscount() allowed discount type
		case CLASS_REGDISC6:       // ItemProcessReturnsDiscount() allowed discount type
            uchRoundType = RND_REGDISC1_ADR;   
			break;
		default:
			return (ITM_SUCCESS);
	}

    if ((sReturnStatus = ItemDiscGetItemizer(pItemDisc, &lItemizer, &lVoidDiscount)) != ITM_SUCCESS) {
        return(sReturnStatus);
    }
    if ((sReturnStatus = RflRateCalc1(&pItemDisc->lAmount, lItemizer, pItemDisc->uchRate * RFL_DISC_RATE_MUL, uchRoundType)) != RFL_SUCCESS) {
        return(LDT_PROHBT_ADR);
    }

	pItemDisc->lAmount *= -1L;    // we need to change the sign for a return

	pItemDisc->fsPrintStatus |= (PRT_JOURNAL | PRT_SLIP | PRT_RECEIPT );
	pItemDisc->fbDiscModifier |= RETURNS_MODIFIER_1;              // Transaction Discount will use this for now
	pItemDisc->fbReduceStatus |= REDUCE_RETURNED;

    if ((sReturnStatus = ItemDiscItemDiscModif(pItemDisc)) != ITM_SUCCESS) {
        return(sReturnStatus);
    }

	if ((sReturnStatus = TrnItem(pItemDisc)) != TRN_SUCCESS ) {
        return(sReturnStatus);
    }
    /*----- R3.0 -----*/
	MldScrollWrite(pItemDisc, MLD_NEW_ITEMIZE);
	MldScrollFileWrite(pItemDisc);

    /*--- DISPLAY DISCOUNT MESSAGE ---*/
//    ItemDiscDisplay(pItemDisc);
    return(ITM_SUCCESS);
}

static struct {
	USHORT  usCount;
	ITEMDISC ItemDisc[10];
} ItemReturnRegularDiscountList = {0, {0}};

USHORT ItemReturnRegularDiscountInit (VOID)
{
	memset (&ItemReturnRegularDiscountList, 0, sizeof(ItemReturnRegularDiscountList));

	return 0;
}

USHORT ItemReturnRegularDiscountAdd (ITEMDISC  *pItemDisc)
{
	switch (pItemDisc->uchMinorClass) {
		case CLASS_REGDISC1:       // ItemReturnRegularDiscountAdd() allowed discount type so add to list
		case CLASS_REGDISC2:       // ItemReturnRegularDiscountAdd() allowed discount type so add to list
		case CLASS_REGDISC3:       // ItemReturnRegularDiscountAdd() allowed discount type so add to list
		case CLASS_REGDISC4:       // ItemReturnRegularDiscountAdd() allowed discount type so add to list
		case CLASS_REGDISC5:       // ItemReturnRegularDiscountAdd() allowed discount type so add to list
		case CLASS_REGDISC6:       // ItemReturnRegularDiscountAdd() allowed discount type so add to list
			break;
		default:
			return (ITM_SUCCESS);
	}

	if (ItemReturnRegularDiscountList.usCount < sizeof(ItemReturnRegularDiscountList.ItemDisc)/sizeof(ItemReturnRegularDiscountList.ItemDisc[0])) {
		ItemReturnRegularDiscountList.ItemDisc[ItemReturnRegularDiscountList.usCount] = *pItemDisc;
		ItemReturnRegularDiscountList.usCount++;
	}

	return ItemReturnRegularDiscountList.usCount;
}
SHORT ItemProcessRegularDiscountsForReturn ()
{
	USHORT  usIndex;

	for (usIndex = 0; usIndex < ItemReturnRegularDiscountList.usCount; usIndex++) {
		ItemProcessReturnsDiscount (&ItemReturnRegularDiscountList.ItemDisc[usIndex]);
	}

	return (ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemPModCompare(UCHAR *puchCurPrintMod, UCHAR *puchPrevPrintMod)
*
*    Input: UCHAR *puchCurPrintMod, UCHAR *puchPrevPrintMod
*
*   Output: None
*
*    InOut: None
*
**Return: 
*         
*
** Description: Compare print modifier. 
*                
*===========================================================================
*/

SHORT   ItemPModCompare(UCHAR *puchCurPrintMod, UCHAR *puchPrevPrintMod)
{                           
    return(*puchCurPrintMod - *puchPrevPrintMod);
}

#if 0
/*
*===========================================================================
**Synopsis: SHORT   ItemCondCompare(USHORT *pusCurCondiment, ITEMCOMMONLOCAL *pItemCommonLocal)
*
*    Input: USHORT *pusCurCondiment, ITEMCOMMONLOCAL *pItemCommonLocal
*
*   Output: None
*
*    InOut: None
*
**Return: 
*         
*
** Description: Compare PLU No. of condiment. 
*                
*===========================================================================
*/
/* not used actually */
SHORT   ItemCondCompare(USHORT *pusCurCondiment, ITEMCOND *pPrevCondiment)
{                           
    return((SHORT)*pusCurCondiment - (SHORT) pPrevCondiment->usPLUNo);
}


#endif
/*
*===========================================================================
**Synopsis: SHORT   ItemPrevModDisc(VOID)
*
*    Input: Nothing
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return: 
*         
*
** Description: Modified discount process. 
*                
*===========================================================================
*/
#define     REQUIRE_SPVINT      1

SHORT   ItemPrevModDisc(VOID)
{
    LONG            lSign = 1L;
    DCURRENCY       lItemizer = 0;
    ITEMSALES       ItemSalesWork = ItemCommonLocal.ItemSales;

    if (ItemSalesWork.fbModifier & VOID_MODIFIER) {    /* voided ? */
        lSign *= -1L;
    }

    if (TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK) {   
        lSign *= -1L;
    }

    ItemSalesWork.lQTY *= lSign;            /* reflect the above condition */
    ItemSalesWork.lProduct *= lSign;        
    ItemSalesWork.lDiscountAmount *= lSign; 

    /*--- GET DISC/ SURCHARGE OPTION ---*/
    if (! CliParaMDCCheck(MDC_MODID23_ADR, ODD_MDC_BIT0)) {     /* discount */
        ItemSalesWork.lDiscountAmount *= -1L;        /* sign for discount */  
    }

    lItemizer = 0L;
    ItemSalesSalesAmtCalc(&ItemSalesWork, &lItemizer);

    if (lItemizer < ItemSalesWork.lDiscountAmount) { 
		ITEMMODIFIER    DummyModifier = {0};
                                            /* disc. amount exceeds itemizer */
        memset(&ItemCommonLocal.ItemSales, 0, sizeof(ITEMSALES));
        ItemCommonLocal.uchItemNo = 0;
        ItemCommonLocal.usSalesBuffSize = 0;

        ItemCommonLocal.fbCommonStatus &= ~COMMON_VOID_EC;    /* enable E.C */
        MldECScrollWrite();                                 /* R3.0 */
        ItemCommonDispECSubTotal(&DummyModifier);           /* Saratoga */
        return(LDT_KEYOVER_ADR);
    }

    return(ITM_SUCCESS);
}


/*
*===========================================================================
**Synopsis: VOID   ItemPrevTotalProc(ITEMSALES *pCurrentItem,  USHORT usTempBuffSize)
*
*    Input: Nothing
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return: 
*         
*
** Description: Add on sales process after total key depression
*                
*===========================================================================
*/


SHORT   ItemPrevTotalProc(VOID *pReceiveItem, USHORT usTempBuffSize) 
{
    SHORT           sReturnStatus, sSize;
    ITEMSALES       * const pCurrentItem = pReceiveItem;
    
    switch(pCurrentItem->uchMajorClass) {
    case CLASS_ITEMSALES:
    case CLASS_ITEMDISC:
    case CLASS_ITEMCOUPON:                  /* R3.0 */
    case CLASS_ITEMMODIFIER:                /* 12/02 '92 */
        if (ItemCommonLocal.ItemSales.uchMinorClass == CLASS_TOTAL1) {
            /*----- SEND PREVIOUS SALES TO TRANSACTION MODULE -----*/
            if ((sReturnStatus = ItemPrevSendSales())  != ITM_SUCCESS) {
                return(sReturnStatus);
            }
    
            /*----- move current item to previous item buffer -----*/
            memset(&ItemCommonLocal.ItemSales, 0, sizeof(ITEMSALES));
            sSize = RflGetStorageItemClassLen(pCurrentItem);        /* Saratoga */
            memcpy(&ItemCommonLocal.ItemSales, pCurrentItem, sSize);
            ItemCommonLocal.uchItemNo++;
            ItemCommonLocal.usSalesBuffSize = usTempBuffSize;
            
            /* clear total key parameter */
            TranCurQualTotalStatusClear();

            /*----- TURN OFF E.C DISABLE STATUS -----*/
            ItemCommonLocal.fbCommonStatus &= ~COMMON_VOID_EC;   /* turn off disable E.C bit */
        } else {
            /* V3.3 */
            ItemPrevCancelAutoTips();

            /* move current item to previous item buffer */
            sSize = RflGetStorageItemClassLen(pCurrentItem);        /* Saratoga */
            memcpy(&ItemCommonLocal.ItemSales, pCurrentItem, sSize);
            ItemCommonLocal.usSalesBuffSize = usTempBuffSize;
 
            /* clear total key parameter */
            TranCurQualTotalStatusClear();
        }

        return(ITM_SUCCESS);

        break;  /* not use */                          

    case CLASS_ITEMTOTAL:
        if ((ItemCommonLocal.ItemSales.uchMajorClass == CLASS_ITEMTOTAL) &&
            (ItemCommonLocal.ItemSales.uchMinorClass != CLASS_TOTAL1)) {
			ITEMTOTAL  * const pLocal = (ITEMTOTAL *)&(ItemCommonLocal.ItemSales);

            /* clear current total key parameter stored in memory resident data base */
            TranCurQualTotalStatusClear();

			if (pLocal->aszNumber[0]) {
				ITEMTOTAL  * const pItem = (ITEMTOTAL *)pCurrentItem;

				memcpy(pItem->aszNumber, pLocal->aszNumber, sizeof(pItem->aszNumber));
			}
        }

        /* move current total to previous item buffer */
        sSize = RflGetStorageItemClassLen(pCurrentItem);        /* Saratoga */
        memcpy(&ItemCommonLocal.ItemSales, pCurrentItem, sSize);
        ItemCommonLocal.usSalesBuffSize = usTempBuffSize;
        return(ITM_SUCCESS);
        break;

    case CLASS_ITEMTENDER:
        return(ITM_PERFORM);

    default:                                
        return(ITM_PERFORM);
    }

    return(ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: SHORT    ItemPrevChargeTipAffection(VOID); 
*
*    Input: VOID 
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return: ITM_SUCCESS
*         
*
** Description: Update memory with previous charge tip data. V3.3
*
*               Note: The Affectation classes for Waiters are no longer
*                     used in the totals accumulation functionality.
*                     It was not ported from the NCR 2170 Hospitatlity
*                     version of the old NHPOS to the NCR 7448 NHPOS
*                     version of the software. Nor has it been updated
*                     or used with the touch screen version of Rel 2.x
*                     of NHPOS nor GenPOS.
*                         Richard Chambers, Dec-03-2020
*===========================================================================
*/

SHORT    ItemPrevChargeTipAffection(ITEMDISC *pDisc) 
{
    TRANINFORMATION    * const WorkTran = TrnGetTranInformationPointer();

    if (WorkTran->TranGCFQual.ulCashierID != WorkTran->TranModeQual.ulCashierID) {
		ITEMAFFECTION      AffectionData = {0};
        
        AffectionData.uchMajorClass = CLASS_ITEMAFFECTION;
        if ((pDisc->uchMinorClass == CLASS_CHARGETIP) ||
            (pDisc->uchMinorClass == CLASS_AUTOCHARGETIP)) {

            AffectionData.uchMinorClass = CLASS_CHARGETIPWAITER;
        } else
        if ((pDisc->uchMinorClass == CLASS_CHARGETIP2) ||
            (pDisc->uchMinorClass == CLASS_AUTOCHARGETIP2)) {

            AffectionData.uchMinorClass = CLASS_CHARGETIPWAITER2;
        } else {
            AffectionData.uchMinorClass = CLASS_CHARGETIPWAITER3;
        }
        AffectionData.ulId = WorkTran->TranGCFQual.ulCashierID;
        AffectionData.lAmount = pDisc->lAmount;                                   /* cahrge tip amount */
        AffectionData.fbModifier = pDisc->fbDiscModifier;                         /* set modifier status */
        AffectionData.fbStorageStatus = STORAGE_STATUS_MASK & ~NOT_ITEM_STORAGE;  /* set only item storage status */
        return(TrnItem(&AffectionData));
    } else {
        return (ITM_SUCCESS);
    }
}

/*
*===========================================================================
**Synopsis: SHORT   ItemCommonSort(ITEMSALES *pCurrentItem,
*                       ITEMSALES *pPrevItem, UCHAR **puchHitPoint)
*    Input:  
*   Output: Nothing
*    InOut: Nothing
**Return: ITM_SUCCESS
*
** Description: 
*===========================================================================
*/
SHORT   ItemCommonSort(ITEMSALES *pCurrentItem,
                        ITEMSALES *pPrevItem, UCHAR **puchHitPoint)
{
    UCHAR   i, uchUnder = 0, uchEqual = 0;
    UCHAR   uchUnderDP = 0, uchEqualDP = 0;

    if (pPrevItem->uchCondNo == 0) {
        *puchHitPoint = (UCHAR *)&pPrevItem->Condiment[pPrevItem->uchChildNo];
        return(0);
    }
    for (i = 0; i < pPrevItem->uchCondNo; i++) {
        if (pPrevItem->Condiment[i + pPrevItem->uchChildNo].uchPrintPriority < pCurrentItem->uchPrintPriority) {
            uchUnder++;
        }
    }
    for (i = 0; i < pPrevItem->uchCondNo; i++) {
        if (pPrevItem->Condiment[i + pPrevItem->uchChildNo].uchPrintPriority == pCurrentItem->uchPrintPriority) {
            uchEqual++;
        }
    }
    for (i = 0; i < uchEqual; i++) {
        if (_tcsncmp(pPrevItem->Condiment[i + uchUnder + pPrevItem->uchChildNo].auchPLUNo, pCurrentItem->auchPLUNo, NUM_PLU_LEN) < 0) {
            uchUnderDP++;
        }
    }
    for (i = 0; i < uchEqual; i++) {
        if (_tcsncmp(pPrevItem->Condiment[i + uchUnder + pPrevItem->uchChildNo].auchPLUNo, pCurrentItem->auchPLUNo, NUM_PLU_LEN) == 0) {
            uchEqualDP++;
        }
    }
    *puchHitPoint = (UCHAR *)&pPrevItem->Condiment[pPrevItem->uchChildNo + uchUnder + uchUnderDP + uchEqualDP];
    return(1);
}

/*
*===========================================================================
**Synopsis: VOID   ItemPrevCancelAutoTip(VOID)
*
*    Input: Nothing
*
*   Output: Nothing
*
*    InOut: Nothing
*
**Return: 
*         
*
** Description: cancel auto charge tips class by item transfer, V3.3
*                
*===========================================================================
*/
VOID   ItemPrevCancelAutoTips(VOID) 
{

    if (ItemCommonLocal.ItemDisc.uchMajorClass == CLASS_ITEMDISC) {
        if ((ItemCommonLocal.ItemDisc.uchMinorClass == CLASS_AUTOCHARGETIP) ||
            (ItemCommonLocal.ItemDisc.uchMinorClass == CLASS_AUTOCHARGETIP2) ||
            (ItemCommonLocal.ItemDisc.uchMinorClass == CLASS_AUTOCHARGETIP3))
		{
			TRANGCFQUAL     * const WorkGCF = TrnGetGCFQualPtr();

            /* auto charge tips data already send to trans. module */
            /* cancel charge tips */

            TrnSplReduce(&ItemCommonLocal.ItemDisc); /* reduce charge tips */

            memset(&ItemCommonLocal.ItemDisc, 0 , sizeof(ITEMDISC));
            ItemCommonLocal.usDiscBuffSize = 0;
            ItemCommonLocal.lChargeTip = 0L;

			WorkGCF->fsGCFStatus &= ~GCFQUAL_CHARGETIP;
            WorkGCF->fsGCFStatus2 &= ~GCFQUAL_AUTOCHARGETIP;
        }
    }
}

VOID   ItemTenderClearChargeAutoTips(VOID) 
{
    TRANGCFQUAL     * const pWorkGCF = TrnGetGCFQualPtr();
    ITEMCOMMONLOCAL * const pCommonLocalData = ItemCommonGetLocalPointer();

    pWorkGCF->fsGCFStatus &= ~GCFQUAL_CHARGETIP;
    pWorkGCF->fsGCFStatus2 &= ~GCFQUAL_AUTOCHARGETIP;

	pCommonLocalData->lChargeTip = 0L;
}


/*
*===========================================================================
**Synopsis: void ItemPreviousGetForKeyData(UIFREGSALES *pCurrentItem){
*
*    Input: Nothing
*   Output: Nothing
*    InOut: UIFREGSALES *pCurrentItem - used to pass @For key data
*			 back to caller's variable
**Return: VOID
*
** Description: Query Previous Item buffer and retrieve @/For data when PI
*	is an @/For operation.  If PI is not @/for, does nothing.
*===========================================================================
*/
static  UIFREGSALES  ItemAtForKeyData = {0};

void ItemPreviousGetForKeyData(UIFREGSALES *pCurrentItem)
{
	if (ItemAtForKeyData.uchMajorClass != 0) {
		pCurrentItem->lUnitPrice = ItemAtForKeyData.lUnitPrice;
		pCurrentItem->lQTY = ItemAtForKeyData.lQTY;
		pCurrentItem->usFor = ItemAtForKeyData.usFor;
		pCurrentItem->usForQty = ItemAtForKeyData.usForQty;
	}
}

void ItemPreviousGetForKeyDataItemSales (ITEMSALES *pCurrentItem)
{
	if (ItemAtForKeyData.uchMajorClass != 0) {
		pCurrentItem->uchMajorClass = ItemAtForKeyData.uchMajorClass;
		pCurrentItem->uchMinorClass = ItemAtForKeyData.uchMinorClass;
		pCurrentItem->lUnitPrice = ItemAtForKeyData.lUnitPrice;
		pCurrentItem->lQTY = ItemAtForKeyData.lQTY;
		pCurrentItem->usFor = ItemAtForKeyData.usFor;
		pCurrentItem->usForQty = ItemAtForKeyData.usForQty;
	}
}


void ItemPreviousSaveForKeyData(UIFREGSALES *pCurrentItem)
{
	ItemAtForKeyData.uchMajorClass = pCurrentItem->uchMajorClass;
	ItemAtForKeyData.uchMinorClass = pCurrentItem->uchMinorClass;
	ItemAtForKeyData.lUnitPrice = pCurrentItem->lUnitPrice;
	ItemAtForKeyData.lQTY = pCurrentItem->lQTY;
	ItemAtForKeyData.usFor = pCurrentItem->usFor;
	ItemAtForKeyData.usForQty = pCurrentItem->usForQty;
}

void ItemPreviousClearForKeyData(VOID)
{
	ItemAtForKeyData.uchMajorClass = 0;
	ItemAtForKeyData.uchMinorClass = 0;
	ItemAtForKeyData.lUnitPrice = 0;
	ItemAtForKeyData.lQTY = 0;
	ItemAtForKeyData.usFor = 0;
	ItemAtForKeyData.usForQty = 0;
}

/*
*===========================================================================
**Synopsis: USHORT ItemPreviousCondiment(ITEMCOND *CondimentData){
*
*    Input: Nothing
*   Output: Nothing
*    InOut: UIFREGSALES *pCurrentItem - used to pass @For key data
*			 back to caller's variable
**Return: VOID
*
** Description: Add a condiment to an item that has already been itemized
*===========================================================================
*/
USHORT ItemPreviousCondiment(ITEMCOND *pCondimentData, USHORT usType)
{
	USHORT		usScroll, usCondNumber = 0, usRetFlag;
	SHORT		sReturnStatus = 0, qtyHolder = 0;
	ITEMSALES	ItemSales = {0};
	USHORT		usRet;
	ITEMCOND    Condiment[10] = {0};

	usScroll = MldQueryCurScroll();	//determine which screen we are using
 
	//determine which item the user has chosen and see if 
	//the item is a previous item, and not current
 	if(MLD_ERROR != MldGetCursorDisplay(usScroll, &ItemSales, &usCondNumber, MLD_GET_COND_NUM) ){
		if(ItemCommonLocal.ItemSales.usUniqueID != ItemSales.usUniqueID)	
		{
			UCHAR  uchMajorClassSave = ItemSales.uchMajorClass;  // save major class to change it temporarily below.

			{
				SHORT   sStatus, sQty, sStorage;

				// If this is a void item then we do not allow a condiment edit on it.
				if((ItemSales.fbModifier & VOID_MODIFIER) != 0)
				{
					return LDT_PROHBT_ADR;
				}

				sStorage = TrnDetermineStorageType();

				sQty = 1;
				if (ItemSales.uchMajorClass == CLASS_ITEMSALES) {
					if (ItemSalesDiscountClassIsMod (ItemSales.uchMinorClass) ||     /* R3.0 */
						ItemSalesDiscountClassIsItem (ItemSales.uchMinorClass) ||
						(ItemSales.ControlCode.auchPluStatus[2] & PLU_SCALABLE) )
					{
						sQty = 1;
					} else {
						sQty = (SHORT)(ItemSales.lQTY / 1000L);
					}
				}

				if ((sStatus = TrnVoidSearch(&ItemSales, sQty, sStorage)) != TRN_SUCCESS) {
					return(sStatus);
				}
			}

			//CondNumber is sent back from GetCursorDisplay, if there is not one, then the user
			// has selected the item itself so we must add it to an empty slot				
			ItemSales.fbStorageStatus |= IS_CONDIMENT_EDIT;
			if(usCondNumber == 0)
			{
				//go through the item's condiment list and find an open spot to place the new
				//condiment into
				while(usCondNumber < STD_MAX_COND_NUM && ItemSales.Condiment[usCondNumber].auchPLUNo[0])
				{
					usCondNumber++;
				}
				if (usCondNumber >= STD_MAX_COND_NUM) {
					return MLD_ERROR;
				}
				usCondNumber++;    // This is one based and not zero based so that if 0 then it is the item and not a condiment
				ItemSales.uchCondNo++; //increment the items condiment counter
			}

			if(usType == MLD_CONDIMENT_VOID) //cursor void
			{

				// Indicate to subfunctions that we are doing a condiment edit.
				// We don't need to clear this since this is a temporary ItemSales variable.
				ItemSalesVoidCondiment(&ItemSales, usCondNumber);

				// In the following code section, we will use the function MldScrollFileWrite()
				// to update the MLD file then use the function MldScrollWrite() to
				// update the displayed receipt window.
				// WARNING: This order must be maintained because the function MldScrollWrite()
				//          will update the scrolling region data structure and that data struct
				//          is used by MldScrollFileWrite () to determine the item number to modify.
				//          If MldScrollWrite() is called first then the line where the current cursor
				//          is located will have its item number removed and it will be zero if the
				//          condiment is the last condiment of the last item.
				ItemSales.uchMajorClass = CLASS_ITEMPREVCOND;		//Major class to preform requested previous condiment function
				if( MLD_SUCCESS != MldScrollFileWrite(&ItemSales))  //write to the MLD file and make sure it works
				{
					return MLD_ERROR;
				}
			
				ItemSales.uchMajorClass = uchMajorClassSave;		//Major class to preform requested previous condiment function
				ItemCommonDispSubTotal (&ItemSales);

				// Now update the stored item data with the modified item data, changed condiments.
				TrnStoSalesItemUpdate(&ItemSales);

				ItemSendKds(&ItemSales, 0);
				MldScrollWrite(&ItemSales, MLD_CONDIMENT_UPDATE);	//update the chosen condiment
				return ItemSales.usUniqueID;
			}

			//adjust the Condiment price if necessary
			ItemSalesAddReplaceCondiment (&ItemSales, usCondNumber, pCondimentData);
			TrnStoSalesItemUpdate(&ItemSales);

			usRetFlag = TRUE;

			ItemSales.uchMajorClass = CLASS_ITEMPREVCOND;		//Major class to preform requested previous condiment function
			if( MLD_SUCCESS != MldScrollFileWrite(&ItemSales))  //write to the MLD file and make sure it works
			{
				return MLD_ERROR;
			}
			ItemSales.uchMajorClass = uchMajorClassSave;		//Major class to preform requested previous condiment function
			
			// The function ItemCommonDispSubTotal() displays the current calculated
			// subtotal.  However, it does not include what is in the local working
			// buffer ItemCommonLocal in its summation.  But it does include any
			// condiment changes made by the code above.  So we will include the
			// ItemcommonLocal.ItemSales in the subtotal calculations.
			ItemCommonDispSubTotal(&ItemCommonLocal.ItemSales);
			ItemSendKds(&ItemSales, 0);
			MldScrollWrite(&ItemSales, usType); //write to the display
			return ItemSales.usUniqueID;
		} else
		{
				//we are modifying the current item
				//ItemPreviousItem(&ItemSales,0);
				if(usType == MLD_CURSOR_CHANGE_UP || usType == MLD_CURSOR_CHANGE_DOWN)
				{
					//move the condiment either up or down, based of the user 
					//pressing cursor up or down.
					usRet = ItemSalesMoveCondiment(&ItemSales,usCondNumber,usType);

					if (usRet != SUCCESS)
					{
						return usRet;
					}
				}
			
				// CondNumber is sent back from GetCursorDisplay, if there is not one, then the user
				// has selected the item itself so we must add it to an empty slot				
				if (0 == usCondNumber)
				{
					// go through the item's condiment list and find an open spot to place the new
					// condiment into
					while(usCondNumber < STD_MAX_COND_NUM && ItemCommonLocal.ItemSales.Condiment[usCondNumber].auchPLUNo[0])
					{
						usCondNumber++;
					}
					if (usCondNumber >= STD_MAX_COND_NUM) {
						return MLD_ERROR;
					}
					ItemCommonLocal.ItemSales.uchCondNo++; //increment the items condiment counter
					ItemCommonLocal.ItemSales.fbStorageStatus &= ~IS_CONDIMENT_EDIT;
					// copy the new data into the location of the old condiment data
					ItemCommonLocal.ItemSales.Condiment[usCondNumber] = *pCondimentData;
				}
				else //GetCursor has returned a value, therefore we will be editing a condiment
				{
					usCondNumber -=1;
					ItemCommonLocal.ItemSales.fbStorageStatus |= IS_CONDIMENT_EDIT;
					if(usType == MLD_CONDIMENT_VOID) //cursor void
					{
						ItemCommonLocal.ItemSales.fbStorageStatus |= IS_CONDIMENT_EDIT;
						if(ItemCommonLocal.ItemSales.uchCondNo > 0)
						{
							USHORT  usCount;

							ItemCommonLocal.ItemSales.uchCondNo --;
							for(usCount = usCondNumber; usCount < ItemCommonLocal.ItemSales.uchCondNo; usCount++)
							{
								ItemCommonLocal.ItemSales.Condiment[usCount] = ItemCommonLocal.ItemSales.Condiment[usCount + 1];
							}
							memset(&ItemCommonLocal.ItemSales.Condiment[usCount], 0x00, sizeof(ItemCommonLocal.ItemSales.Condiment[0]));
						}
					}
					else {
						// copy the new data into the location of the old condiment data
						ItemCommonLocal.ItemSales.Condiment[usCondNumber] = *pCondimentData;
					}
				}

				ItemCommonDispSubTotal(&ItemCommonLocal.ItemSales);
				ItemSendKds(&ItemCommonLocal.ItemSales, 0);
				MldScrollWrite(&ItemCommonLocal.ItemSales, MLD_CONDIMENT_UPDATE);	//update the chosen condiment

				ItemCommonLocal.ItemSales.fbStorageStatus &= ~IS_CONDIMENT_EDIT;
				return ItemCommonLocal.ItemSales.usUniqueID;
		}	
			
		return ItemSales.usUniqueID;
	}
	return 0;
}

/*
*===========================================================================
**Synopsis: USHORT ItemSalesMoveCondiment(Itemsales *ItemSales,usCondNumber,usType)
*
*    Input: ItemSales		- ItemSales variable that has information of the item
*							  that the cursor was selected on.
*			usCondNumber	- Array location of the condiment selected
*			usType			- Which way to move the condiment
*
*	Output: ItemSales		- Condiment will be moved to the correct location
*							  with any item in that location moved also
**	Return: SUCCESS or Prohibited
*
** Description: Moves a condiment up or down in the display.  We use an 
**				empty ITEMCOND structure for moving one item up in the list,
*				and move the item that is currently in that one out of the way.
*===========================================================================
*/
static USHORT ItemSalesMoveCondiment(ITEMSALES *pItemSales,USHORT usCondNumber,USHORT usType)
{
	ITEMCOND	CondimentHolder = {0};

	//move up or move down? based on FSC press.
	switch(usType)
	{
		//move the condiment up, if there is an item above it,
		//we need to move it down.
		case MLD_CURSOR_CHANGE_UP:
			//if this is less than or equal to 1, we either have an
			//error, or the user is trying to move the topmost condiment up.
			//so we will return a prohibited error.
			if((usCondNumber > 1) && (usCondNumber <= STD_MAX_COND_NUM))
			{
				//We are using a zero based array, so we subtract by one
				usCondNumber --;
				
				//move the condiment that is in the spot we want to move up into a holder
				CondimentHolder = pItemSales->Condiment[usCondNumber - 1];
				//move the item we are asking to move up, into the appropriate location
				pItemSales->Condiment[usCondNumber - 1] = pItemSales->Condiment[usCondNumber];
				//restore the condiment to its new location
				pItemSales->Condiment[usCondNumber] = CondimentHolder;

				//Do the same thing for the ItemCommonLocal, which is the saved variable
				//move the condiment that is in the spot we want to move up into a holder
				CondimentHolder = ItemCommonLocal.ItemSales.Condiment[usCondNumber- 1];
				//move the item we are asking to move up, into the appropriate location
				ItemCommonLocal.ItemSales.Condiment[usCondNumber - 1] = ItemCommonLocal.ItemSales.Condiment[usCondNumber];
				//restore the condiment to its new location
				ItemCommonLocal.ItemSales.Condiment[usCondNumber] = CondimentHolder;
			} else
			{
				return LDT_PROHBT_ADR;
			}
			break;
		case MLD_CURSOR_CHANGE_DOWN:
			if((usCondNumber <= STD_MAX_COND_NUM) &&(usCondNumber >= 1))
			{
				//check to see if there is any data below the condiment,
				//if not, we return prohibited, because this is the last caondiment
				if(pItemSales->Condiment[usCondNumber].auchPLUNo[0])
				{

					//move the condiment that is in the spot we want to move down into a holder
					CondimentHolder = pItemSales->Condiment[usCondNumber];
					//move the item we are asking to move up, into the appropriate location
					pItemSales->Condiment[usCondNumber] = pItemSales->Condiment[usCondNumber-1];
					//restore the condiment to its new location
					pItemSales->Condiment[usCondNumber-1] = CondimentHolder;

					//Do the same thing for the ItemCommonLocal, which is the saved variable
					//move the condiment that is in the spot we want to move down into a holder
					CondimentHolder = ItemCommonLocal.ItemSales.Condiment[usCondNumber];
					//move the item we are asking to move up, into the appropriate location
					ItemCommonLocal.ItemSales.Condiment[usCondNumber] = ItemCommonLocal.ItemSales.Condiment[usCondNumber-1];
					//restore the condiment to its new location
					ItemCommonLocal.ItemSales.Condiment[usCondNumber-1] = CondimentHolder;
				} else
				{
					return LDT_PROHBT_ADR;
				}
			} else
			{
				return LDT_PROHBT_ADR;
			}
			break;

		default:
			return LDT_PROHBT_ADR;
	}

	//send item sales to be written in the transaction file,
	//so that the item will show up as being moved when the
	//user presses total, or tender/*PreviousCondiment*/
//	TrnStoPreviousCondimentSales(pItemSales,0);

	return SUCCESS;
}
/*** PDINU
*===========================================================================
**Synopsis: SHORT CheckAllowFunction()
*
*    Input: Nothing
*   Output: Nothing
*
**Return: SUCCESS or LDT_PROHBT_ADR
*
** Description: This function checks to see if there are items alreaded in the
*				transaction.
*===========================================================================
*/
SHORT CheckAllowFunction()
{
	if ((ItemCommonLocal.fbCommonStatus & COMMON_PROHIBIT_SIGNOUT) != 0) {
		return (LDT_PROHBT_ADR);
	}

	if ((ItemCommonLocal.ItemSales.uchMajorClass == 0) && (ItemCommonGetStatus(COMMON_PROHIBIT_SIGNOUT) == -1))
		return (SUCCESS);
	else 
		return (LDT_PROHBT_ADR);
}

/****** End of Source ******/