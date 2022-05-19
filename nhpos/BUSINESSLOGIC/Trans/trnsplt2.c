/*=======================================================================*\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  2170         *             AT&T GIS Corporation, E&M OISO   ||
||        * << ECR-90 >>  *                                              ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995-1998      ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
:   Title          : TRANSACTION SPLIT CHECK MODULE
:   Category       : TRANSACTION MODULE, NCR 2170 US Hospitality Application
:   Program Name   : TRNSPLT2.C
:  ---------------------------------------------------------------------
:  Abstract:
:
:  ---------------------------------------------------------------------
:  Update Histories
:  Date     : Ver.Rev. :   Name     : Description
: Nov-28-95 : 03.01.00 :  hkato     : R3.1
: Apr-04-95 : 03.01.02 :  T.Nakahata: correct VoidSearch condition
: Aug-18-98 : 03.03.00 :  hrkato    : V3.3 (VAT/Service)
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
* Nov-21-15 : 02.02.01 : R.Chambers : using new typedefs for variables and source cleanup.
* Feb-16-15 : 02.02.01 : R.Chambers : fix defect on unable cursor void scalable items.
* Feb-16-15 : 02.01.01 : R.Chambers : source cleanup and consolidation, use offsetof(), use TrnSplDecideStorage()
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
#include    <stdlib.h>
#include	<stddef.h>

#include    <ecr.h>
#include    <pif.h>
#include    <regstrct.h>
#include    <transact.h>
#include    <trans.h>
#include    <prt.h>
#include    <rfl.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csstbpar.h>
#include    <appllog.h>
#include    <item.h>
#include    <prtprty.h>

/*
*===========================================================================
** Synopsis:    SHORT   TrnIsSameZipDataSalesNon(VOID *pSalesNon, VOID *pData)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      Non-zero if the two items are the same.
*               zero if the two items are not the same.
*
** Description: compare two transaction items to determine if the two items
*               are the same item or not.  If the two items have the same 
*               values in the initial header information, they are deemed to
*               be the same.
*               Structs that need to be compatible in the first five members
*               uchMajorClass, uchMinorClass, uchSeatno, auchPLUNo, and usDeptNo,
*               include but are not limited to the following structs:
*                  ITEMSALES, TRANSTORAGESALESNON, 
*===========================================================================
*/
SHORT TrnIsSameZipDataSalesNon (VOID *pSalesNon, VOID *pData)
{
	int iCompareSize = offsetof(ITEMSALES, lQTY);

	return (memcmp (pSalesNon, pData, iCompareSize) == 0);
}

SHORT TrnIsSameZipDataSalesNonQty (VOID *pSalesNon, VOID *pData)
{
	int iCompareSize = offsetof(ITEMSALES, lProduct);

	return (memcmp (pSalesNon, pData, iCompareSize) == 0);
}


/*
*===========================================================================
** Synopsis:    SHORT   TrnIsSameZipDataCoupon(VOID *pCouponNon, VOID *pData)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      Non-zero if the two coupons/discounts are the same.
*               zero if the two coupons/discounts are not the same.
*
** Description: compare two transaction coupons/discounts to determine if the
*               two are the same or not.  If the two coupons/discounts have the 
*               same values in the initial header information, they are deemed to
*               be the same.
*               Structs that need to be compatible in the first three members
*               uchMajorClass, uchMinorClass, uchSeatno, include but are
*               not limited to the following structs:
*                  ITEMCOUPON, TRANSTORAGECOUPONNON, ITEMDISC, TRANSTORAGEDISCNON
*               Class codes used with this function are:
*                  CLASS_ITEMCOUPON, CLASS_ITEMDISC, 
*===========================================================================
*/
SHORT TrnIsSameZipDataCoupon (VOID *pCouponNon, VOID *pData)
{
	int iCompareSize = offsetof(TRANSTORAGECOUPONNON, fbReduceStatus);

	return (memcmp (pCouponNon, pData, iCompareSize) == 0);
}

/*
*===========================================================================
** Synopsis:    SHORT   TrnVoidSearch(VOID *pData, SHORT sQTY, SHORT sStorage)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description:     Search Void Item.
*===========================================================================
*/
SHORT   TrnVoidSearch(VOID *pData, SHORT sQTY, SHORT sStorage)
{
    ITEMSALES   *pItem = pData;

    switch(pItem->uchMajorClass) {
    case    CLASS_ITEMSALES:
        if (pItem->uchMinorClass == CLASS_DEPTMODDISC
            || pItem->uchMinorClass == CLASS_PLUMODDISC
            || pItem->uchMinorClass == CLASS_SETMODDISC
            || pItem->uchMinorClass == CLASS_OEPMODDISC
            || pItem->uchMinorClass == CLASS_DEPTITEMDISC
            || pItem->uchMinorClass == CLASS_PLUITEMDISC
            || pItem->uchMinorClass == CLASS_SETITEMDISC
            || pItem->uchMinorClass == CLASS_OEPITEMDISC
            || pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
#if 0
//RJC test
            if (sQTY > 1) {
                return(LDT_KEYOVER_ADR); 
            }
            if (TrnVoidSearchSalesMod(pData, sStorage) != TRN_SUCCESS) {
                return(LDT_NTINFL_ADR);
            }
#else
            if (TrnVoidSearchSales(pData, sQTY, sStorage) != TRN_SUCCESS) {
                return(LDT_NTINFL_ADR);
            }
#endif
        } else {
            if (TrnVoidSearchSales(pData, sQTY, sStorage) != TRN_SUCCESS) {
                return(LDT_NTINFL_ADR);
            }
        }
        break;

    case    CLASS_ITEMDISC:
        if (sQTY > 1) {
//RJC test            return(LDT_KEYOVER_ADR); 
        }
	    if (pItem->uchMinorClass == CLASS_REGDISC1
    	    || pItem->uchMinorClass == CLASS_REGDISC2
        	|| pItem->uchMinorClass == CLASS_REGDISC3
	        || pItem->uchMinorClass == CLASS_REGDISC4
    	    || pItem->uchMinorClass == CLASS_REGDISC5
			|| pItem->uchMinorClass == CLASS_REGDISC6) {
			
	        if (TrnVoidSearchRegDisc(pData, sStorage) != TRN_SUCCESS) {
    	        return(LDT_NTINFL_ADR);
        	}
		} else {
	        if (TrnVoidSearchNonSalesItem(pData, sStorage) != TRN_SUCCESS) {
    	        return(LDT_NTINFL_ADR);
        	}
        }
        break;

    case    CLASS_ITEMCOUPON:
        if (sQTY > 1) {
            return(LDT_KEYOVER_ADR); 
        }
        if (TrnVoidSearchNonSalesItem(pData, sStorage) != TRN_SUCCESS) {
            return(LDT_NTINFL_ADR);
        }
        break;

    case    CLASS_ITEMTENDER:
        if (TrnVoidSearchNonSalesItem(pData, sStorage) != TRN_SUCCESS) {
            return(LDT_NTINFL_ADR);
        }
		break;

    default:
        return(LDT_SEQERR_ADR);
    }

    return(TRN_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT   TrnVoidSearchSales(ITEMSALES *pData, SHORT sQTY, SHORT sStorage)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description:     Search the transaction for to see if this item is in the transaction
*                   and if it is whether the quantity that is available in the transaction
*                   is sufficient to cover the quantity that is being voided.
*
*                   This function should only be used by non-scalable PLUs and should not be
*                   used with discounts.
*===========================================================================
*/
SHORT   TrnVoidSearchSales(ITEMSALES *pItemSales, SHORT sQTY, SHORT sStorage)
{
    UCHAR                  auchWork[TRN_WORKBUFFER];
    ULONG                  ulCurSize, ulReadPos;
	LONG                   lItemQuantity = 0;
	BOOL                   bFoundItem = FALSE;
    PifFileHandle          hsHandle;
    TRANSTORAGESALESNON    *pSalesNon = TRANSTORAGENONPTR(auchWork);

	if (pItemSales->uchMajorClass != CLASS_ITEMSALES) 
		return LDT_NTINFL_ADR;

	/*----- Search First Seat# Item in Item/Consoli Storage -----*/
    TrnSplDecideStorage(sStorage, &ulReadPos, &ulCurSize, &hsHandle);

    while (ulReadPos < ulCurSize) {
		ULONG	ulActualBytesRead = 0;//RPH 11-11-3 SR# 201

        /*----- Read 1 Item -----*/
        //RPH 11-11-3 SR# 201
		TrnReadFile(ulReadPos, auchWork, sizeof(auchWork), hsHandle, &ulActualBytesRead);

		// If we read zero bytes then lets just assume the thing isn't there.
		if (ulActualBytesRead < 1)
			break;

		if ((pSalesNon->fsLabelStatus & ITM_CONTROL_RETURN_OLD) == (pItemSales->fsLabelStatus & ITM_CONTROL_RETURN_OLD)) {
        if (pSalesNon->uchMajorClass == CLASS_ITEMSALES) {
			SHORT  sCheckZippedData = 0;

            /*----- Check Unzipped Data -----*/
            if (TrnIsSameZipDataSalesNon ( pSalesNon, pItemSales)) {
				ITEMSALES  ItemSalesWork = {0};
				sCheckZippedData = 1;

				/*----- Check Zipped Data -----*/
				RflGetStorageItem(&ItemSalesWork, auchWork, RFL_WITH_MNEM);

				/*----- Check Net Qty -----*/
				if ((ItemSalesWork.fbModifier & VOID_MODIFIER) == 0) {
					if (TrnInformation.TranCurQual.fsCurStatus & CURQUAL_PRESELECT_MASK) {
						if (pItemSales->fbModifier & VOID_MODIFIER) {
        					if (sQTY > abs(ItemSalesWork.sCouponQTY)) { 
            					sCheckZippedData = 0;
                			} 
							if (ItemSalesWork.sCouponQTY <= 0) {
								sCheckZippedData = 0;
    						}
    					} else {
							if (sQTY > abs(ItemSalesWork.sCouponQTY)) { 
    							sCheckZippedData = 0;
        					} 
							if (ItemSalesWork.sCouponQTY >= 0) {   /* 4/25/96 */
    							sCheckZippedData = 0;
        					}
        				}
					} else {
						if (pItemSales->fbModifier & VOID_MODIFIER) {
							if (sQTY > abs(ItemSalesWork.sCouponQTY)) { 
    							sCheckZippedData = 0;
        					} 
            				if (ItemSalesWork.sCouponQTY >= 0) {
                				sCheckZippedData = 0;
							}
						} else {
							if (sQTY > ItemSalesWork.sCouponQTY) { 
    							sCheckZippedData = 0;
        					} 
							if (ItemSalesWork.sCouponQTY <= 0) {
    							/* if net Qty is minus, cannot void item.
        						 * Net Qty is plus, return succees to ITEM module.
            					 * TRANS module re-calculates sCouponQTY and sCouponCo
                				 * at TrnStorage() when VOID-PLU item is retrieved.
                    			 */
								sCheckZippedData = 0;
							}
						}
    				}
				}

				if (sCheckZippedData) {
					// First of all lets look to see if this item is in the transaction.
					// We do not modify the fbModifier attribute at this point
					bFoundItem = (bFoundItem || (TrnVoidSearchSalesZip(&ItemSalesWork, pItemSales) == TRN_SUCCESS));

					// Now then lets look to see if this item or its voided counterpart is
					// in the transaction.  This will allow us to determine if we have found
					// either an instance of the item or a void with a partial quantity
					ItemSalesWork.fbModifier = pItemSales->fbModifier;
					if (TrnVoidSearchSalesZip(&ItemSalesWork, pItemSales) == TRN_SUCCESS) {
						// Keep a running total of the total quantity of this item that we find.
						// Voided items will have a negative quantity.
						lItemQuantity += ItemSalesWork.lQTY;
					}
				}
			}
        }
		}

        /*----- Increment Read Pointer -----*/
        ulReadPos += RflGetStorageTranRecordLen(auchWork);
    }

	if (bFoundItem) {
		// we divide quantity by 1000 since it is stored that way for non-scalable items.
		// if a scalable item then match found is good enough.
		if (pItemSales->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
			return (TRN_SUCCESS);
		} else {
			lItemQuantity /= 1000;

			if (TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK) {
				// this is a Preselect Return or an Offline Return in VCS terms
				// so we will allow a void of a voided item.
				sQTY *= -1;
				if (lItemQuantity <= sQTY)
					return (TRN_SUCCESS);
			} else {
				if (lItemQuantity >= sQTY)
					return (TRN_SUCCESS);
			}
		}
	}

	return(LDT_NTINFL_ADR);
}

/*
*===========================================================================
** Synopsis:    SHORT   TrnVoidSearchSalesMod(ITEMSALES *pData, SHORT sStorage)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description:     Search Sales Void Item.
*===========================================================================
*/
SHORT   TrnVoidSearchSalesMod(ITEMSALES *pData, SHORT sStorage)
{
    UCHAR                   auchWork[TRN_WORKBUFFER];
	USHORT                  pDataSave_fbModifier = pData->fbModifier;
	DCURRENCY               pDataSave_lProduct = pData->lProduct;
    ULONG                   ulCurSize, ulReadPos;
    PifFileHandle           hsHandle;
    TRANSTORAGESALESNON     *pSalesNon = TRANSTORAGENONPTR(auchWork);

	if (pData->uchMajorClass != CLASS_ITEMSALES) 
		return LDT_NTINFL_ADR;

    /*----- Search First Seat# Item in Consoli Storage -----*/
    TrnSplDecideStorage(sStorage, &ulReadPos, &ulCurSize, &hsHandle);

	pData->fbModifier |= VOID_MODIFIER;
	pData->lProduct *= -1;

    while (ulReadPos < ulCurSize) {
		ULONG	    ulActualBytesRead;	//RPH 11-11-3 SR# 201
		ITEMSALES   auchTranStorageWork = {0};

        /*----- Read 1 Item -----*/
        //RPH 11-11-3 SR# 201
		TrnReadFile(ulReadPos, auchWork, sizeof(auchWork), hsHandle, &ulActualBytesRead);

		// If we read zero bytes then lets just assume the thing isn't there.
		if (ulActualBytesRead < 1)
			break;

        switch(pSalesNon->uchMajorClass) {
        case    CLASS_ITEMSALES:
            /*----- Already Void Item -----*/
            if (pSalesNon->fbReduceStatus & REDUCE_ITEM) {
                break;
            }

			if ((pSalesNon->fsLabelStatus & ITM_CONTROL_RETURN_OLD) != (pData->fsLabelStatus & ITM_CONTROL_RETURN_OLD))
				break;

            /*----- Check Unzipped Data -----*/
            if ( ! TrnIsSameZipDataSalesNon ( pSalesNon, pData)) {
                break;
            }

            /*----- Already Void Item -----*/
//			if (pSalesNon->usOffset) {	/* 07/24/01 */
//				break;
//			}

            /*----- Check Zipped Data -----*/
            RflGetStorageItem(&auchTranStorageWork, auchWork, RFL_WITH_MNEM);
            if (TrnVoidSearchSalesZip(&auchTranStorageWork, pData) != TRN_SUCCESS) {
                break;
            }

            /* for scalable item, saratoga */
            if (pData->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
                if (auchTranStorageWork.lQTY != pData->lQTY) break;
            }
                
            /*----- Found Target Void Item so we indicate problem -----*/
			pData->fbModifier = pDataSave_fbModifier;
			pData->lProduct = pDataSave_lProduct;
            return(LDT_NTINFL_ADR);

        default:
            break;
        }

        /*----- Increment Read Pointer -----*/
        ulReadPos += RflGetStorageTranRecordLen(auchWork);
    }

	pData->fbModifier = pDataSave_fbModifier;
	pData->lProduct = pDataSave_lProduct;
    return(TRN_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT   TrnVoidSearchSalesZip(ITEMSALES *pSales1, ITEMSALES *pSales2)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description:     Check Sales Zipped Field.
*===========================================================================
*/
SHORT   TrnVoidSearchSalesZip(ITEMSALES *pSales1, ITEMSALES *pSales2)
{
	ULONG  ulReturnsfbModifierMask = ~(RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3 | RETURNS_ORIGINAL);

    if (pSales1->uchAdjective != pSales2->uchAdjective
        || (pSales1->fbModifier & ulReturnsfbModifierMask) != (pSales2->fbModifier & ulReturnsfbModifierMask)
        || pSales1->uchPrintModNo != pSales2->uchPrintModNo
        || pSales1->uchChildNo != pSales2->uchChildNo
        || pSales1->uchPPICode != pSales2->uchPPICode
        || pSales1->uchCondNo != pSales2->uchCondNo) {
        return(TRN_ERROR);
    }

	//SR 527, the problem was that we were comparing only the first number in the number type array
	//by checking ALL of the numbers, we can now determine the correct item to discount JHHJ
    if (memcmp(pSales1->aszNumber[0], pSales2->aszNumber[0], (NUM_NUMBER * NUM_OF_NUMTYPE_ENT * sizeof(TCHAR)))
        || memcmp(pSales1->auchPrintModifier, pSales2->auchPrintModifier, sizeof(pSales1->auchPrintModifier))
        || memcmp(pSales1->Condiment, pSales2->Condiment, sizeof(pSales1->Condiment))) {
        return(TRN_ERROR);
    }

    /* 02/17/01, check different price */
    if (pSales1->lUnitPrice != pSales2->lUnitPrice) {
        return(TRN_ERROR);
    }

    if ((pSales1->uchMinorClass == CLASS_DEPTITEMDISC) ||
        (pSales1->uchMinorClass == CLASS_PLUITEMDISC)  ||
        (pSales1->uchMinorClass == CLASS_SETITEMDISC)  ||
        (pSales1->uchMinorClass == CLASS_OEPITEMDISC)) {
// RJC  test
		if (pSales1->usUniqueID != pSales2->usUniqueID) {
        	return(TRN_ERROR);
        }
    }

	/* if this is a do not consolidate item such as open department or open PLU then we want to
		check that the amounts and the unique ids match other wise these are not the same item. */
    if ((pSales1->fsLabelStatus & ITM_CONTROL_NO_CONSOL) || (pSales2->fsLabelStatus & ITM_CONTROL_NO_CONSOL)) {
// RJC  test
		if (pSales1->usUniqueID != pSales2->usUniqueID) {
        	return(TRN_ERROR);
        }
    }
    
    if ((pSales1->uchMinorClass == CLASS_DEPTMODDISC) ||
        (pSales1->uchMinorClass == CLASS_PLUMODDISC) ||
        (pSales1->uchMinorClass == CLASS_SETMODDISC) ||
        (pSales1->uchMinorClass == CLASS_OEPMODDISC)) {

// RJC  test
		if (pSales1->usUniqueID != pSales2->usUniqueID) {
        	return(TRN_ERROR);
        }
    }

    return(TRN_SUCCESS);
}


SHORT   TrnVoidSearchNonSalesItem(ITEMGENERICHEADER *pData, SHORT sStorage)
{
    UCHAR           auchWork[sizeof(ITEMDATASIZEUNION)];
    PifFileHandle   hsHandle;
    ULONG           ulCurSize, ulReadPos;
	DCURRENCY       lGratuityAmount = 0;

    /*----- Search First Seat# Item in Consoli Storage -----*/
    TrnSplDecideStorage(sStorage, &ulReadPos, &ulCurSize, &hsHandle);

    while (ulReadPos < ulCurSize) {
		ULONG	ulActualBytesRead;	//RPH 11-11-3 SR# 201
        /*----- Read 1 Item -----*/

        //RPH 11-11-3 SR# 201
		TrnReadFile(ulReadPos, auchWork, sizeof(auchWork), hsHandle, &ulActualBytesRead);

		// If we read zero bytes then lets just assume the thing isn't there.
		if (ulActualBytesRead < 1)
			break;

		if (TRANSTORAGEDATAPTR(auchWork)->uchMajorClass == pData->uchMajorClass) {
			UCHAR	auchTranStorageWork[sizeof(ITEMDATASIZEUNION)] = {0};
			TRANSTORAGEDISCNON    *pDiscNon = TRANSTORAGENONPTR(auchWork);
			ITEMDISC              *pDiscData = (ITEMDISC *)pData;
			TRANSTORAGECOUPONNON  *pCpnNon = TRANSTORAGENONPTR(auchWork);
			ITEMCOUPON            *pCpnData = (ITEMCOUPON *)pData;
			TRANSTORAGETENDERNON  *pTenderNon = TRANSTORAGENONPTR(auchWork);
			ITEMTENDER            *pTenderData = (ITEMTENDER *)pData;

			switch(TRANSTORAGEDATAPTR(auchWork)->uchMajorClass) {
				case    CLASS_ITEMDISC:
					/*----- Check Net Qty -----*/
					if (pDiscNon->fbReduceStatus & REDUCE_ITEM) {
						break;
					}
					/*----- Check Unzipped Data -----*/
					if (! TrnIsSameZipDataCoupon (pDiscNon, pDiscData)) {
						break;
					}
					/*----- Check Zipped Data -----*/
					/*----- De-Compress Item -----*/
					RflGetStorageItem(auchTranStorageWork, auchWork, RFL_WITH_MNEM);
					if (memcmp(auchTranStorageWork + sizeof(TRANSTORAGEDISCNON), &pDiscData->auchPLUNo, sizeof(TRANSTORAGEDISC)) != 0) {
					/* if (memcmp(auchTranStorageWork + sizeof(TRANSTORAGEDISCNON),
						&pData->usPLUNo, sizeof(TRANSTORAGEDISC)) != 0) { */
						break;
					}
					/*----- Found Target Item -----*/
					return(TRN_SUCCESS);

				case    CLASS_ITEMCOUPON:
					/*----- Check Net Qty -----*/
					if (pCpnNon->fbReduceStatus & REDUCE_ITEM) {
						break;
					}
					/*----- Coupon# -----*/
					if (pCpnData->uchCouponNo != pCpnNon->uchCouponNo) {
						break;
					}
					if (_tcsncmp(pCpnData->auchUPCCouponNo, pCpnNon->auchUPCCouponNo, NUM_PLU_LEN) != 0) {
						break;
					}
					/* --- determine target item is already voided --- */
					if ( pCpnNon->sVoidCo != 0 ) {
						break;
					}
					/*----- Check Unzipped Data -----*/
					if ( ! TrnIsSameZipDataCoupon (pCpnNon, pCpnData)) {
						break;
					}
					/*----- Check Zipped Data -----*/
					/*----- De-Compress Item -----*/
					RflGetStorageItem(auchTranStorageWork, auchWork, RFL_WITH_MNEM);
					if (memcmp(auchTranStorageWork + sizeof(TRANSTORAGECOUPONNON), &pCpnData->lAmount, sizeof(TRANSTORAGECOUPON)) != 0) {
						break;
					}
					/*----- Found Target Item -----*/
					return(TRN_SUCCESS);

				case    CLASS_ITEMTENDER:
					if (pTenderNon->uchMinorClass != CLASS_TEND_TIPS_RETURN) {
						break;
					}
					if (pTenderNon->fbReduceStatus & REDUCE_ITEM) {
						break;
					}
//					if (pTenderNon->fbReduceStatus & REDUCE_RETURNED) {
					{
						ITEMTENDER *pTenderWork = (ITEMTENDER *)auchTranStorageWork;

						RflGetStorageItem(auchTranStorageWork, auchWork, RFL_WITH_MNEM);
						if (pTenderWork->usCheckTenderId != pTenderData->usCheckTenderId)
							break;
						lGratuityAmount += pTenderWork->lGratuity;
					}
					/*----- Found Target Item -----*/
//					return(TRN_SUCCESS);
					break;

				default:
					break;
			}
		}

        /*----- Increment Read Pointer -----*/
        ulReadPos += RflGetStorageTranRecordLen(auchWork);
    }

	if (lGratuityAmount < 0)
		return(TRN_SUCCESS);  // we have found already done this charge tip
	else
		return(LDT_NTINFL_ADR);
}

/*
*===========================================================================
** Synopsis:    SHORT   TrnVoidSearchDisc(ITEMDISC *pData, SHORT sStorage)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description:     Search Discount Void Item. TAR191625
*===========================================================================
*/
SHORT   TrnVoidSearchRegDisc(ITEMDISC *pData, SHORT sStorage)
{
    UCHAR               auchWork[sizeof(ITEMDATASIZEUNION)];
    PifFileHandle       hsHandle;
    ULONG               ulCurSize, ulReadPos;
	USHORT              i;
    TRANSTORAGEDISCNON  *pDiscNon = TRANSTORAGENONPTR(auchWork);

    /*----- Search First Seat# Item in Consoli Storage -----*/
    TrnSplDecideStorage(sStorage, &ulReadPos, &ulCurSize, &hsHandle);

    while (ulReadPos < ulCurSize) {
		ULONG	ulActualBytesRead;//RPH 11-11-3 SR# 201

        /*----- Read 1 Item -----*/
        //RPH 11-11-3 SR# 201
		TrnReadFile(ulReadPos, auchWork, sizeof(auchWork), hsHandle, &ulActualBytesRead);

		// If we read zero bytes then lets just assume the thing isn't there.
		if (ulActualBytesRead < 1)
			break;

        switch(TRANSTORAGEDATAPTR(auchWork)->uchMajorClass) {
        case    CLASS_ITEMDISC:
            /*----- Check Net Qty -----*/
            if (pDiscNon->fbReduceStatus & REDUCE_ITEM) {
                break;
            }

            /*----- Check Unzipped Data -----*/
            if ( ! TrnIsSameZipDataCoupon (pDiscNon, pData)) {
                break;
            }

			{
				UCHAR     uchRateSave = pData->uchRate;
				UCHAR     auchTranStorageWork[sizeof(ITEMSALES)] = {0};
				ITEMDISC  *pDiscStore = (ITEMDISC *)&auchTranStorageWork;
				int       iCompareSize = offsetof(ITEMDISC, VoidDisc) - offsetof(ITEMDISC, auchPLUNo);

				/*----- Check Zipped Data -----*/
				/*----- De-Compress Item -----*/
				RflGetStorageItem(auchTranStorageWork, auchWork, RFL_WITH_MNEM);

				if (pData->uchRate == 0) {
					pData->uchRate = pDiscStore->uchRate; /* mask for manual entry */
				}

				if (memcmp(&pDiscStore->auchPLUNo, &pData->auchPLUNo, iCompareSize) != 0) {
					pData->uchRate = uchRateSave;
					break;
				}

				/* restore void discount tax amount */
				pData->uchRate = uchRateSave;
    			if (TrnTaxSystem() == TRN_TAX_US) {
					for (i = 0; i < STD_TAX_ITEMIZERS_MAX; i++) {
            			pData->VoidDisc.USVoidDisc.lVoidDiscNonAffectTaxable[i] = pDiscStore->VoidDisc.USVoidDisc.lVoidDiscNonAffectTaxable[i];
            			pData->VoidDisc.USVoidDisc.lVoidDiscFSDiscTaxable[i] = pDiscStore->VoidDisc.USVoidDisc.lVoidDiscFSDiscTaxable[i];
					}
				} else if (TrnTaxSystem() == TRN_TAX_CANADA) {
					for (i = 0; i < STD_PLU_ITEMIZERS+2+3; i++) {    // should this be STD_PLU_ITEMIZERS+2+3 rather than 9+2+3?
                		pData->VoidDisc.CanVoidDisc.lVoidDiscTax[i] = pDiscStore->VoidDisc.CanVoidDisc.lVoidDiscTax[i];
					}
				} else {
					for (i = 0; i < 3; i++) {
						pData->VoidDisc.IntlVoidDisc.lVoidVATSer[i] = pDiscStore->VoidDisc.IntlVoidDisc.lVoidVATSer[i];
	            		pData->VoidDisc.IntlVoidDisc.lVoidVATNon[i] = pDiscStore->VoidDisc.IntlVoidDisc.lVoidVATNon[i];
					}
            		pData->VoidDisc.IntlVoidDisc.lVoidServiceable = pDiscStore->VoidDisc.IntlVoidDisc.lVoidServiceable;
				}
			}

            /*----- Found Target Item -----*/
            return(TRN_SUCCESS);

        default:
            break;
        }

        /*----- Increment Read Pointer -----*/
        ulReadPos += RflGetStorageTranRecordLen(auchWork);
    }

    return(LDT_NTINFL_ADR);
}


/*==========================================================================
**    Synopsis: VOID    TrnVoidSearchRedSalesMod(ITEMSALES *pItem)
*
*    Input:     
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   Set Reduce Bit to Voided Item for Next Void Search.
==========================================================================*/

VOID    TrnVoidSearchRedSalesMod(ITEMSALES *pItem)
{
    TrnVoidSearchRedSalesModSub(TRN_TYPE_ITEMSTORAGE, pItem);
    TrnVoidSearchRedSalesModSub(TRN_TYPE_CONSSTORAGE, pItem);
}

/*==========================================================================
**    Synopsis: VOID    TrnVoidSearchRedSalesModSub(SHORT sStorage, ITEMSALES *pItem)
*
*    Input:     
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   Set Reduce Bit to Voided Item for Next Void Search.
==========================================================================*/

VOID    TrnVoidSearchRedSalesModSub(SHORT sStorage, ITEMSALES *pItem)
{
    UCHAR           auchWork[TRN_WORKBUFFER];
    PifFileHandle   hsHandle;
    ULONG           ulCurSize, ulReadPos;

    TrnSplDecideStorage(sStorage, &ulReadPos, &ulCurSize, &hsHandle);

    while (ulReadPos < ulCurSize) {
		ULONG	    ulActualBytesRead;//RPH 11-11-3 SR# 201
		UCHAR       auchStorageWork[TRN_WORKBUFFER] = {0};
		ITEMSALES   *pSales = (ITEMSALES *)auchStorageWork;

        /*----- Read 1 Item -----*/
        //RPH 11-11-3 SR# 201
		TrnReadFile(ulReadPos, auchWork, sizeof(auchWork), hsHandle, &ulActualBytesRead);

		// If we read zero bytes then lets just assume the thing isn't there.
		if (ulActualBytesRead < 1)
			return;

        /*----- De-Compress Item -----*/
        RflGetStorageItem(auchStorageWork, auchWork, RFL_WITH_MNEM);

        if (pSales->uchMajorClass == CLASS_ITEMSALES
            && (pSales->uchMinorClass == CLASS_DEPTMODDISC
                || pSales->uchMinorClass == CLASS_PLUMODDISC
                || pSales->uchMinorClass == CLASS_SETMODDISC
                || pSales->uchMinorClass == CLASS_OEPMODDISC
                || pSales->uchMinorClass == CLASS_DEPTITEMDISC  /* 6/4/96 */
                || pSales->uchMinorClass == CLASS_PLUITEMDISC   /* 6/4/96 */
                || pSales->uchMinorClass == CLASS_SETITEMDISC   /* 6/4/96 */
                || pSales->uchMinorClass == CLASS_OEPITEMDISC   /* 6/4/96 */
                || pSales->ControlCode.auchPluStatus[2] & PLU_SCALABLE)) {

            /*----- Check UnZipped Area -----*/
            if (TrnIsSameZipDataSalesNonQty (pSales, pItem)) {
                if (!(pSales->fbReduceStatus & REDUCE_ITEM)) {
                    pSales->fbReduceStatus |= REDUCE_ITEM;
                    TrnWriteFile(ulReadPos + 1, pSales, sizeof(TRANSTORAGESALESNON), hsHandle); /* ### Mod (2171 for Win32) V1.0 */
                    return;
                }
            }
        }

        /*----- Increment Read Pointer -----*/
        ulReadPos += RflGetStorageTranRecordLen(auchWork);
    }
}

/*
*===========================================================================
** Synopsis:    SHORT   TrnTransferSearch(VOID *pData, SHORT sQTY, SHORT sStorage)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description:     Search Transfer Item.
*===========================================================================
*/
SHORT   TrnTransferSearch(VOID *pData, SHORT sQTY, SHORT sStorage)
{
    ITEMSALES   *pItem = pData;

    switch(pItem->uchMajorClass) {
    case    CLASS_ITEMSALES:
        if (pItem->uchMinorClass == CLASS_DEPTMODDISC
            || pItem->uchMinorClass == CLASS_PLUMODDISC
            || pItem->uchMinorClass == CLASS_SETMODDISC
            || pItem->uchMinorClass == CLASS_OEPMODDISC
            || pItem->uchMinorClass == CLASS_DEPTITEMDISC
            || pItem->uchMinorClass == CLASS_PLUITEMDISC
            || pItem->uchMinorClass == CLASS_SETITEMDISC
            || pItem->uchMinorClass == CLASS_OEPITEMDISC
            || pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
            if (sQTY > 1) {
                return(LDT_KEYOVER_ADR); 
            }
            if (TrnTransferSearchSales(pData, sQTY, sStorage) != TRN_SUCCESS) {
                return(LDT_NTINFL_ADR);
            }

        } else {
            if (TrnTransferSearchSales(pData, sQTY, sStorage) != TRN_SUCCESS) {
                return(LDT_NTINFL_ADR);
            }
        }
        break;

    case    CLASS_ITEMDISC:
        if (sQTY > 1) {
            return(LDT_KEYOVER_ADR); 
        }
        if (TrnTransferSearchDisc(pData, sStorage) != TRN_SUCCESS) {
            return(LDT_NTINFL_ADR);
        }
        break;

    case    CLASS_ITEMCOUPON:
        if (sQTY > 1) {
            return(LDT_KEYOVER_ADR); 
        }
        if (TrnTransferSearchCoupon(pData, sStorage) != TRN_SUCCESS) {
            return(LDT_NTINFL_ADR);
        }
        break;

    default:
        return(LDT_SEQERR_ADR);
    }

    return(TRN_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT   TrnTransferSearchSales(ITEMSALES *pData, SHORT sQTY, SHORT sStorage)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description:     Search Sales Transfer Item.
*===========================================================================
*/
SHORT   TrnTransferSearchSales(ITEMSALES *pData, SHORT sQTY, SHORT sStorage)
{
    UCHAR                   auchWork[sizeof(ITEMDATASIZEUNION)];
    PifFileHandle           hsHandle;
    ULONG                   ulCurSize, ulReadPos;
    TRANSTORAGESALESNON     *pSalesNon = TRANSTORAGENONPTR(auchWork);

    /*----- Search First Seat# Item in Item/Consoli Storage -----*/
    TrnSplDecideStorage(sStorage, &ulReadPos, &ulCurSize, &hsHandle);

    while (ulReadPos < ulCurSize) {
		ITEMSALES   ItemSales = {0};
		ULONG	    ulActualBytesRead;//RPH 11-11-3 SR# 201

        /*----- Read 1 Item -----*/
        //RPH 11-11-3 SR# 201
		TrnReadFile(ulReadPos, auchWork, sizeof(auchWork), hsHandle, &ulActualBytesRead);

        switch(TRANSTORAGEDATAPTR(auchWork)->uchMajorClass) {
        case    CLASS_ITEMSALES:
            /*----- Scalable Item -----*/
            if (pData->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
                /* 5/16/96  
                if (pData->lQTY != 0) {
                    break;
                } */
                if (pData->lQTY == 0L) {    /* 5/16/96 */
                    break;
                }
                if (pSalesNon->fbReduceStatus & REDUCE_ITEM) {
                    break;
                }
            /*----- Normal Item with REDUCE_ITEM flag 6/4/96 -----*/
            } else if (pSalesNon->uchMinorClass == CLASS_DEPTITEMDISC ||
                pSalesNon->uchMinorClass == CLASS_PLUITEMDISC ||
                pSalesNon->uchMinorClass == CLASS_SETITEMDISC ||
                pSalesNon->uchMinorClass == CLASS_OEPITEMDISC ||
                pSalesNon->uchMinorClass == CLASS_DEPTMODDISC ||
                pSalesNon->uchMinorClass == CLASS_PLUMODDISC ||
                pSalesNon->uchMinorClass == CLASS_SETMODDISC ||
                pSalesNon->uchMinorClass == CLASS_OEPMODDISC) {
                if (pSalesNon->fbReduceStatus & REDUCE_ITEM) {
                    break;
                }
            /*----- Normal Item -----*/
            } else {
                if (TrnInformation.TranCurQual.fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)) {
                    /*----- Check Net Qty -----
                    if (abs(pSalesNon->sCouponQTY) > abs(pSalesNon->sCouponCo)) {
                        if (sQTY > abs(abs(pSalesNon->sCouponQTY) - abs(pSalesNon->sCouponCo))) {
                            break;
                        }
                    } else {
                        break;
                    }   */
                    if (pSalesNon->sCouponQTY >= 0) {
                        break;
                    }
                    if (sQTY > abs(pSalesNon->sCouponQTY)) {
                        break;
                    }
                } else {
                    /*----- Check Net Qty -----
                    if (pSalesNon->sCouponQTY > pSalesNon->sCouponCo) {
                        if (sQTY > abs(pSalesNon->sCouponQTY - pSalesNon->sCouponCo)) {
                            break;
                        }
                    } else {
                        break;
                    }   */
                    if (pSalesNon->sCouponQTY <= 0) {
                        break;
                    }
                    if (sQTY > pSalesNon->sCouponQTY) {
                        break;
                    }
                }
            }

            /*----- Check Unzipped Data -----*/
            if ( ! TrnIsSameZipDataSalesNon ( pSalesNon, pData)) {
                break;
            }

            /*----- Check Zipped Data -----*/
            RflGetStorageItem(&ItemSales, auchWork, RFL_WITH_MNEM);
            if (TrnVoidSearchSalesZip(&ItemSales, pData) != TRN_SUCCESS) {
                break;
            }

            /*----- Found Target Item -----*/
            return(TRN_SUCCESS);

        default:
            break;
        }

        /*----- Increment Read Pointer -----*/
        ulReadPos += RflGetStorageTranRecordLen(auchWork);
    }

    return(LDT_NTINFL_ADR);
}

/*
*===========================================================================
** Synopsis:    SHORT   TrnTransferSearchSalesMod(ITEMSALES *pData, SHORT sStorage)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description:     Search Sales Transfer Item for a similar Void item.
*===========================================================================
*/
SHORT   TrnTransferSearchSalesMod(ITEMSALES *pData, SHORT sStorage)
{
	USHORT                  pDataSave_fbModifier = pData->fbModifier;
    UCHAR                   auchWork[sizeof(ITEMDATASIZEUNION)];
    PifFileHandle           hsHandle;
    ULONG                   ulCurSize, ulReadPos;
    TRANSTORAGESALESNON     *pSalesNon = TRANSTORAGENONPTR(auchWork);

	pData->fbModifier |= VOID_MODIFIER;

    /*----- Search First Seat# Item in Consoli Storage -----*/
    TrnSplDecideStorage(sStorage, &ulReadPos, &ulCurSize, &hsHandle);

    while (ulReadPos < ulCurSize) {
		ITEMSALES   ItemSales = {0};
		ULONG	    ulActualBytesRead;//RPH 11-11-3 SR# 201

        /*----- Read 1 Item -----*/
        //RPH 11-11-3 SR# 201
		TrnReadFile(ulReadPos, auchWork, sizeof(auchWork), hsHandle, &ulActualBytesRead);

        switch(TRANSTORAGEDATAPTR(auchWork)->uchMajorClass) {
        case    CLASS_ITEMSALES:
            /*----- Check Unzipped Data -----*/
            if ( ! TrnIsSameZipDataSalesNon( pSalesNon, pData)) {
                break;
            }

            /*----- Reduced Item -----*/
            if (pSalesNon->fbReduceStatus & REDUCE_ITEM) {
                break;
            }

            /*----- Check Zipped Data -----*/
            RflGetStorageItem(&ItemSales, auchWork, RFL_WITH_MNEM);
            if (TrnVoidSearchSalesZip(&ItemSales, pData) != TRN_SUCCESS) {
                break;
            }

            /*----- Found Target Item -----*/
			pData->fbModifier = pDataSave_fbModifier;
            return(TRN_SUCCESS);

        default:
            break;
        }

        /*----- Increment Read Pointer -----*/
        ulReadPos += RflGetStorageTranRecordLen(auchWork);
    }

	pData->fbModifier = pDataSave_fbModifier;
    return(LDT_NTINFL_ADR);
}

/*
*===========================================================================
** Synopsis:    SHORT   TrnTransferSearchDisc(ITEMDISC *pData, SHORT sStorage)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description:     Search Discount Transfer Item.
*===========================================================================
*/
SHORT   TrnTransferSearchDisc(ITEMDISC *pData, SHORT sStorage)
{
    UCHAR               auchWork[sizeof(ITEMDATASIZEUNION)];
    PifFileHandle       hsHandle;
    ULONG               ulCurSize, ulReadPos;
    TRANSTORAGEDISCNON  *pDiscNon = TRANSTORAGENONPTR(auchWork);

    /*----- Search First Seat# Item in Consoli Storage -----*/
    TrnSplDecideStorage(sStorage, &ulReadPos, &ulCurSize, &hsHandle);

    while (ulReadPos < ulCurSize) {
		UCHAR   auchTranStorageWork[sizeof(ITEMSALES)] = {0};
		ULONG	ulActualBytesRead;//RPH 11-11-3 SR# 201
		int     iCompareSize = offsetof(ITEMDISC, VoidDisc) - offsetof(ITEMDISC, auchPLUNo);

        /*----- Read 1 Item -----*/
        //RPH 11-11-3 SR# 201
		TrnReadFile(ulReadPos, auchWork, sizeof(auchWork), hsHandle, &ulActualBytesRead);
        switch(TRANSTORAGEDATAPTR(auchWork)->uchMajorClass) {
        case    CLASS_ITEMDISC:
            /*----- Check Unzipped Data -----*/
            if ( ! TrnIsSameZipDataCoupon (pDiscNon, pData)) {
                break;
            }

            /*----- Reduced Item -----*/
            if (pDiscNon->fbReduceStatus & REDUCE_ITEM) {
                break;
            }

            /*----- Check Zipped Data -----*/
            /*----- De-Compress Item and compare the compressed area data -----*/
            RflGetStorageItem(auchTranStorageWork, auchWork, RFL_WITH_MNEM);
            if (memcmp(auchTranStorageWork + sizeof(TRANSTORAGEDISCNON), &pData->auchPLUNo, iCompareSize) != 0) {
                break;
            }

            /*----- Found Target Item -----*/
            return(TRN_SUCCESS);

        default:
            break;
        }

        /*----- Increment Read Pointer -----*/
        ulReadPos += RflGetStorageTranRecordLen(auchWork);
    }

    return(LDT_NTINFL_ADR);
}

/*
*===========================================================================
** Synopsis:    SHORT   TrnTransferSearchSalesDisc(ITEMSALES *pSales,
*                           ITEMDISC *pDisc, SHORT sStorage)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description:     Search Sales and Item Discount of Transfer Item.
*===========================================================================
*/
SHORT   TrnTransferSearchSalesDisc(ITEMSALES *pSales, ITEMDISC *pDisc, SHORT sStorage)
{
    UCHAR                   auchWork[sizeof(ITEMDATASIZEUNION)];
    PifFileHandle           hsHandle;
    ULONG                   ulCurSize, ulReadPos;
    TRANSTORAGESALESNON     *pSalesNon = TRANSTORAGENONPTR(auchWork);
    TRANSTORAGEDISCNON      *pDiscNon = TRANSTORAGENONPTR(auchWork);

    /*----- Search First Seat# Item in Consoli Storage -----*/
    TrnSplDecideStorage(sStorage, &ulReadPos, &ulCurSize, &hsHandle);

    while (ulReadPos < ulCurSize) {
		UCHAR   auchTranStorageWork[sizeof(ITEMSALES)] = {0};
		ULONG	ulActualBytesRead;//RPH 11-11-3 SR# 201
		int     iCompareSize;

        /*----- Read 1 Item -----*/
        //RPH 11-11-3 SR# 201
		TrnReadFile(ulReadPos, auchWork, sizeof(auchWork), hsHandle, &ulActualBytesRead);
        switch(TRANSTORAGEDATAPTR(auchWork)->uchMajorClass) {
        case    CLASS_ITEMSALES:
            /*----- Check Sales Data -----*/
            /*----- Check Unzipped Data -----*/
            if (pSalesNon->uchMinorClass != CLASS_DEPTITEMDISC
                && pSalesNon->uchMinorClass != CLASS_PLUITEMDISC
                && pSalesNon->uchMinorClass != CLASS_SETITEMDISC
                && pSalesNon->uchMinorClass != CLASS_OEPITEMDISC) {
                break;
            }

            if ( ! TrnIsSameZipDataSalesNon ( pSalesNon, pSales)) {
                break;
            }
            /*----- Reduced Item -----*/
            if (pSalesNon->fbReduceStatus & REDUCE_ITEM) {
                break;
            }
            if (pSalesNon->fbReduceStatus & REDUCE_NOT_HOURLY) {
                if (!(pSales->fbReduceStatus & REDUCE_NOT_HOURLY)) {
                    break;
                }
            } else {
                if (pSales->fbReduceStatus & REDUCE_NOT_HOURLY) {
                    break;
                }
            }
            /*----- Check Zipped Data -----*/
            RflGetStorageItem(auchTranStorageWork, auchWork, RFL_WITH_MNEM);
            if (TrnVoidSearchSalesZip((ITEMSALES *)auchTranStorageWork, pSales) != TRN_SUCCESS) {
                break;
            }

            /*----- Check Item Discount Data -----*/
            //RPH 11-11-3 SR# 201
            ulReadPos += RflGetStorageTranRecordLen(auchWork);
			TrnReadFile(ulReadPos, auchWork, sizeof(auchWork), hsHandle, &ulActualBytesRead);

            if (pDiscNon->uchMinorClass != CLASS_ITEMDISC1) {
                break;
            }

            /*----- Check Unzipped Data -----*/
            if ( ! TrnIsSameZipDataCoupon (pDiscNon, pDisc)) {
                break;
            }
            /*----- Reduced Item -----*/
            if (pDiscNon->fbReduceStatus & REDUCE_ITEM) {
                break;
            }
            /*----- Check Zipped Data -----*/
            /*----- De-Compress Item -----*/
            memset(auchTranStorageWork, 0, sizeof(auchTranStorageWork));
            RflGetStorageItem(auchTranStorageWork, auchWork, RFL_WITH_MNEM);
			iCompareSize = offsetof(ITEMDISC, VoidDisc) - offsetof(ITEMDISC, auchPLUNo);
            if (memcmp(auchTranStorageWork + sizeof(TRANSTORAGEDISCNON), &pDisc->auchPLUNo, iCompareSize) != 0) {
                break;
            }

            /*----- Found Target Item -----*/
            return(TRN_SUCCESS);

        default:
            break;
        }

        /*----- Increment Read Pointer -----*/
        ulReadPos += RflGetStorageTranRecordLen(auchWork);
    }

    return(LDT_NTINFL_ADR);
}

/*
*===========================================================================
** Synopsis:    SHORT   TrnTransferSearchCoupon(ITEMCOUPON *pData, SHORT sStorage)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description:     Search Coupon Transfer Item.
*===========================================================================
*/
SHORT   TrnTransferSearchCoupon(ITEMCOUPON *pData, SHORT sStorage)
{
    UCHAR                 auchWork[sizeof(ITEMDATASIZEUNION)];
    PifFileHandle         hsHandle;
    ULONG                 ulCurSize, ulReadPos;
    TRANSTORAGECOUPONNON  *pCpnNon = TRANSTORAGENONPTR(auchWork);

    /*----- Search First Seat# Item in Consoli Storage -----*/
    TrnSplDecideStorage(sStorage, &ulReadPos, &ulCurSize, &hsHandle);

    while (ulReadPos < ulCurSize) {
		UCHAR   auchTranStorageWork[sizeof(ITEMSALES)] = {0};
		ULONG	ulActualBytesRead;//RPH 11-11-3 SR# 201

        /*----- Read 1 Item -----*/
        //RPH 11-11-3 SR# 201
		TrnReadFile(ulReadPos, auchWork, sizeof(auchWork), hsHandle, &ulActualBytesRead);
        switch(TRANSTORAGEDATAPTR(auchWork)->uchMajorClass) {
        case    CLASS_ITEMCOUPON:
            /*----- Coupon# -----*/
            if (pData->uchCouponNo != pCpnNon->uchCouponNo) {
                break;
            }

            if (_tcsncmp(pData->auchUPCCouponNo, pCpnNon->auchUPCCouponNo, NUM_PLU_LEN) != 0) {
                break;
            }

            /*----- Reduced Item -----*/
            if (pCpnNon->fbReduceStatus & REDUCE_ITEM) {
                break;
            }

            /* --- determine target item is already voided --- */
            if ( pCpnNon->sVoidCo != 0 ) {
                break;
            }

            /*----- Check Unzipped Data -----*/
            if ( ! TrnIsSameZipDataCoupon (pCpnNon, pData)) {
                break;
            }

            /*----- Check Zipped Data -----*/
            /*----- De-Compress Item -----*/
            RflGetStorageItem(auchTranStorageWork, auchWork, RFL_WITH_MNEM);
            if (memcmp(auchTranStorageWork + sizeof(TRANSTORAGECOUPONNON), &pData->lAmount, sizeof(TRANSTORAGECOUPON)) != 0) {
                break;
            }

            /*----- Found Target Item -----*/
            return(TRN_SUCCESS);

        default:
            break;
        }

        /*----- Increment Read Pointer -----*/
        ulReadPos += RflGetStorageTranRecordLen(auchWork);
    }

    return(LDT_NTINFL_ADR);
}


/*==========================================================================
**  Synopsis:   SHORT   TrnSplReduce(VOID *pItem)
*                           
*   Input:     
*   Output:     none
*   InOut:      none
*
*   Return:     TRN_SUCCESS
*               LDT_TAKETL_ADR
*                   
*   Description:    Reduce Transaction Itemizer and Item/Consoli Storage.
==========================================================================*/
SHORT   TrnSplReduce(VOID *pItem)
{
    switch(ITEMSTORAGENONPTR(pItem)->uchMajorClass) {
    case CLASS_ITEMSALES:
        TrnSplReduceItemizerSales(pItem);
        TrnSplReduceStorageSales(pItem);
        return(TRN_SUCCESS);

    case CLASS_ITEMDISC:
        TrnSplReduceItemizerDisc(pItem);
        TrnSplReduceStorageDisc(pItem);
        return(TRN_SUCCESS);

    case CLASS_ITEMCOUPON:
        TrnSplReduceItemizerCoupon(pItem);
        TrnSplReduceStorageCoupon(pItem);
        return(TRN_SUCCESS);

    default:
        return(TRN_ERROR);
    }
}

/*==========================================================================
**    Synopsis: VOID    TrnSplReduceItemizerSales(ITEMSALES *pItem)
*
*    Input:     ITEMSALES   *ItemSales
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   itemize by sales
==========================================================================*/
VOID    TrnSplReduceItemizerSales(ITEMSALES *pItem)
{
    if (!(pItem->fbReduceStatus & REDUCE_ITEM)) {
        switch(pItem->uchMinorClass) {
        case CLASS_DEPT:
        case CLASS_DEPTITEMDISC:
            TrnSplRedItemSalesDept(pItem);
            break;

        case CLASS_PLU:
        case CLASS_PLUITEMDISC:
        case CLASS_SETMENU:
        case CLASS_SETITEMDISC:
        case CLASS_OEPPLU:
        case CLASS_OEPITEMDISC:
            TrnSplRedItemSalesPLU(pItem);
            break;

        case CLASS_DEPTMODDISC:
        case CLASS_PLUMODDISC:
        case CLASS_SETMODDISC:
        case CLASS_OEPMODDISC:
            TrnSplRedItemSalesMod(pItem);
            break;

        default:
            break;
        }
    }
}

/*==========================================================================
**    Synopsis: VOID    TrnSplRedItemSalesDept(ITEMSALES *pItem)
*
*    Input:     
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   itemize by dept, set menu
==========================================================================*/
VOID    TrnSplRedItemSalesDept(ITEMSALES *pItem)
{
    UCHAR   fchFood = 0;

    TrnInformation.TranItemizers.lMI -= pItem->lProduct;

    /* correct fs affection, 02/14/01 */
	fchFood = TrnSalesItemIsFoodStamp (pItem->ControlCode.auchPluStatus);
	
    if (pItem->ControlCode.auchPluStatus[1] & PLU_AFFECT_DISC1) {
        TrnInformation.TranItemizers.lDiscountable[0] -= pItem->lProduct;
        if (fchFood) {
            TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFSDiscountable[0] -= pItem->lProduct;
        }
    }
    if (pItem->ControlCode.auchPluStatus[1] & PLU_AFFECT_DISC2) {
        TrnInformation.TranItemizers.lDiscountable[1] -= pItem->lProduct;
        if (fchFood) {
            TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFSDiscountable[1] -= pItem->lProduct;
        }
    }

    if (TrnTaxSystem() == TRN_TAX_CANADA) {                     /* V3.3 */
        TrnSalesDeptCanComn(pItem, &TrnInformation.TranItemizers, &TrnInformation.TranGCFQual, TRN_ITEMIZE_REDUCE);
    } else if (TrnTaxSystem() == TRN_TAX_US) {                  /* V3.3 */
        TrnSalesDeptUSComn(pItem, &TrnInformation.TranItemizers, TRN_ITEMIZE_REDUCE);
    } else {                                            /* Int'l Tax,   V3.3 */
        TrnSalesDeptIntlComn(pItem, &TrnInformation.TranItemizers, TRN_ITEMIZE_REDUCE);
    }
}

/*==========================================================================
**    Synopsis: VOID    TrnSplRedItemSalesPLU(ITEMSALES *pItem)
*
*    Input:     
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   itemize by PLU, promotional PLU
==========================================================================*/
VOID    TrnSplRedItemSalesPLU(ITEMSALES *pItem)
{
    USHORT  i;
    USHORT  usMaxChild;
	TCHAR   auchDummy[NUM_PLU_LEN] = {0};
    UCHAR   fchFood = 0;

    TrnInformation.TranItemizers.lMI -= pItem->lProduct;
    /* correct fs affection, 02/14/01 */
	fchFood = TrnSalesItemIsFoodStamp (pItem->ControlCode.auchPluStatus);

	/* --- Food Stampable --- */
    if (fchFood) {
        TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFoodStampable -= pItem->lProduct;
        /* --- Taxable #1-3 ---  */
        if (pItem->ControlCode.auchPluStatus[1] & (PLU_AFFECT_TAX1 | PLU_AFFECT_TAX2 | PLU_AFFECT_TAX3)) {
            TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFoodWholeTax -= pItem->lProduct;
        }
    }

    if (pItem->ControlCode.auchPluStatus[1] & PLU_AFFECT_DISC1) {
        TrnInformation.TranItemizers.lDiscountable[0] -= pItem->lProduct;
        if (fchFood) {
            TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFSDiscountable[0] -= pItem->lProduct;
        }
    }
    if (pItem->ControlCode.auchPluStatus[1] & PLU_AFFECT_DISC2) {
        TrnInformation.TranItemizers.lDiscountable[1] -= pItem->lProduct;
        if (fchFood) {
            TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFSDiscountable[1] -= pItem->lProduct;
        }
    }

    /* 5/7/96 if (pItem->ControlCode.auchStatus[5] & ITM_SALES_HOURLY) {
        TrnInformation.TranItemizers.lHourly -= pItem->lProduct;
        TrnInformation.TranItemizers.sItemCounter -= TrnQTY(pItem);
    } */

    /* --- itemize condiment PLU --- */
    usMaxChild = (pItem->uchChildNo + pItem->uchCondNo + pItem->uchPrintModNo);

	NHPOS_ASSERT(usMaxChild <= sizeof(pItem->Condiment)/sizeof(pItem->Condiment[0]));

    /* link plu, saratoga */
    if (pItem->usLinkNo) {
        i = 0;
    } else {
        i = pItem->uchChildNo;
    }
    for (; i < usMaxChild; i++) {
		DCURRENCY    lChildAmount;

        if ( _tcsncmp(pItem->Condiment[i].auchPLUNo, auchDummy, NUM_PLU_LEN) == 0) {
            continue;
        }

	    /* correct fs affection, 02/14/01 */
		fchFood = TrnSalesItemIsFoodStamp (pItem->Condiment[i].ControlCode.auchPluStatus);
        lChildAmount = (DCURRENCY)TrnQTY(pItem) * pItem->Condiment[i].lUnitPrice;
        TrnInformation.TranItemizers.lMI -= lChildAmount;

        /* --- Food Stampable --- */
        if (fchFood) {
            TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFoodStampable -= lChildAmount;
            /* --- Taxable #1-3 ---  */
            if (pItem->Condiment->ControlCode.auchPluStatus[1] & (PLU_AFFECT_TAX1 | PLU_AFFECT_TAX2 | PLU_AFFECT_TAX3)) {
                TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFoodWholeTax -= lChildAmount;
            }
        }
        if (pItem->Condiment[i].ControlCode.auchPluStatus[1] & PLU_AFFECT_DISC1) {
            TrnInformation.TranItemizers.lDiscountable[0] -= lChildAmount;
            if (fchFood) {
                TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFSDiscountable[0] -= lChildAmount;
            }
        }
        if (pItem->Condiment[i].ControlCode.auchPluStatus[1] & PLU_AFFECT_DISC2) {
            TrnInformation.TranItemizers.lDiscountable[1] -= lChildAmount;
            if (fchFood) {
                TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFSDiscountable[1] -= lChildAmount;
            }
        }
    }

    TrnSplTaxable(pItem);
    TrnSplDiscTax(pItem);
}

/*==========================================================================
**    Synopsis: VOID    TrnSplTaxable(ITEMSALES *pItem)
*
*    Input:     
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   update taxable itemizer
==========================================================================*/
VOID    TrnSplTaxable(ITEMSALES *pItem)
{
    if (TrnTaxSystem() == TRN_TAX_CANADA) {                     /* V3.3 */
        TrnSalesDeptCanComn(pItem, &TrnInformation.TranItemizers, &TrnInformation.TranGCFQual, TRN_ITEMIZE_REDUCE);
        TrnTaxableCanComn(pItem, &TrnInformation.TranItemizers, TRN_ITEMIZE_REDUCE);
    } else if (TrnTaxSystem() == TRN_TAX_US) {                  /* V3.3 */
        TrnTaxableUSComn(pItem, &TrnInformation.TranItemizers, TRN_ITEMIZE_REDUCE);
    } else {                                            /* Int'l Tax,   V3.3 */
        TrnTaxableIntlComn(pItem, &TrnInformation.TranItemizers, TRN_ITEMIZE_REDUCE);
    }
}

/*==========================================================================
**    Synopsis: VOID    TrnSplDiscTax(ITEMSALES *pItem)
*
*    Input:     
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   update discountable/taxable itemizer
==========================================================================*/
VOID    TrnSplDiscTax(ITEMSALES *pItem)
{
    if (TrnTaxSystem() == TRN_TAX_CANADA) {                     /* V3.3 */
        TrnDiscTaxCanComn(pItem, &TrnInformation.TranItemizers, TRN_ITEMIZE_REDUCE);
    } else if (TrnTaxSystem() == TRN_TAX_US) {                  /* V3.3 */
        TrnDiscTaxUSComn(pItem, &TrnInformation.TranItemizers, TRN_ITEMIZE_REDUCE);
    } else {                                        /* Int'l VAT,   V3.3 */
    /* not use */
    }
}

/*==========================================================================
**    Synopsis: VOID    TrnSplRedItemSalesMod(ITEMSALES *pItem)
*
*    Input:     
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   itemize by modifier discount
==========================================================================*/
VOID    TrnSplRedItemSalesMod(ITEMSALES *pItem)
{
/*    UCHAR   uchWorkMDC, uchWorkBit;
    SHORT   i; */
    UCHAR   fchFood = 0;

    TrnInformation.TranItemizers.lMI -= pItem->lDiscountAmount;

    /* correct fs affection, 02/14/01 */
	fchFood = TrnSalesItemIsFoodStamp (pItem->ControlCode.auchPluStatus);
	
    /* --- Food Stampable --- */
    if (fchFood) {
        TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFoodStampable -= pItem->lDiscountAmount;
        /* --- Taxable #1-3 ---  */
        if (pItem->ControlCode.auchPluStatus[1] & (PLU_AFFECT_TAX1 | PLU_AFFECT_TAX2 | PLU_AFFECT_TAX3)) {
            TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFoodWholeTax -= pItem->lDiscountAmount;
        }
    }

    if (pItem->ControlCode.auchPluStatus[1] & PLU_AFFECT_DISC1) {
        TrnInformation.TranItemizers.lDiscountable[0] -= pItem->lDiscountAmount;
        if (fchFood) {
            TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFSDiscountable[0] -= pItem->lDiscountAmount;
        }
    }
    if (pItem->ControlCode.auchPluStatus[1] & PLU_AFFECT_DISC2) {
        TrnInformation.TranItemizers.lDiscountable[1] -= pItem->lDiscountAmount;
        if (fchFood) {
            TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFSDiscountable[1] -= pItem->lDiscountAmount;
        }
    }

    /* 5/7/96 if (pItem->ControlCode.auchStatus[5] & ITM_SALES_HOURLY) {
        TrnInformation.TranItemizers.lHourly -= pItem->lProduct;
        TrnInformation.TranItemizers.sItemCounter -= TrnQTY(pItem);
    } */

    if (TrnTaxSystem() == TRN_TAX_CANADA) {                     /* V3.3 */
        TrnSalesModCanComn(pItem, &TrnInformation.TranItemizers, TRN_ITEMIZE_REDUCE);
    } else if (TrnTaxSystem() == TRN_TAX_US) {                  /* V3.3 */
        TrnSalesModUSComn(pItem, &TrnInformation.TranItemizers, TRN_ITEMIZE_REDUCE);
    } else {                                        /* Int'l VAT,   V3.3 */
        TrnSalesModIntlComn(pItem, &TrnInformation.TranItemizers, TRN_ITEMIZE_REDUCE);
    }
}

/*==========================================================================
**    Synopsis: VOID    TrnSplReduceItemizerDisc(ITEMDISC *pItem)
*
*    Input:     ITEMDISC    *ItemDisc
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   itemize by discount                        R3.1
==========================================================================*/
VOID    TrnSplReduceItemizerDisc(ITEMDISC *pItem)
{
    if (!(pItem->fbReduceStatus & REDUCE_ITEM)) {
        switch(pItem->uchMinorClass) {
        case CLASS_ITEMDISC1:
            TrnSplRedItemDiscID(pItem);
            break;

        case CLASS_REGDISC1:
        case CLASS_REGDISC2:
        case CLASS_REGDISC3:
        case CLASS_REGDISC4:
        case CLASS_REGDISC5:
        case CLASS_REGDISC6:
            TrnSplRedItemDiscRD(pItem);
            break;

        case CLASS_CHARGETIP:
        case CLASS_CHARGETIP2:      /* V3.3 */
        case CLASS_CHARGETIP3:
        case CLASS_AUTOCHARGETIP:
        case CLASS_AUTOCHARGETIP2:
        case CLASS_AUTOCHARGETIP3:
            TrnSplRedItemDiscChg(pItem);
            break;

        default:
            break;
        }
    }
}

/*==========================================================================
**    Synopsis: VOID    TrnSplRedItemDiscID(ITEMDISC *pItem)
*
*    Input:     ITEMDISC    *ItemDisc
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   itemize by item discount
==========================================================================*/
VOID    TrnSplRedItemDiscID(ITEMDISC *pItem)
{
/*    UCHAR   uchWorkMDC, uchWorkBit;
    SHORT   i; */
    UCHAR   fchFood = 0;

    /* correct fs affection, 02/14/01 */
	fchFood = (TrnTaxSystem() == TRN_TAX_US && (pItem->auchDiscStatus[1] & DISC_AFFECT_FOODSTAMP));

    TrnInformation.TranItemizers.lMI -= pItem->lAmount;
    /* --- Food Stampable --- */
    if (fchFood) {
        TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFoodStampable -= pItem->lAmount;
        /* --- Taxable #1-3 ---  */
        if ((pItem->auchDiscStatus[0] & 0x04) ||                 /* Tax #1, DISC_AFFECT_TAXABLE1 */
            (pItem->auchDiscStatus[0] & 0x08) ||                 /* Tax #2, DISC_AFFECT_TAXABLE2 */
            (pItem->auchDiscStatus[0] & 0x10)) {                 /* Tax #3, DISC_AFFECT_TAXABLE3 */
            TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFoodWholeTax -= pItem->lAmount;
        }
    }

    if (pItem->auchDiscStatus[2] & DISC_STATUS_2_REGDISC1) {
        TrnInformation.TranItemizers.lDiscountable[0] -= pItem->lAmount;
        if (fchFood) {
            TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFSDiscountable[0] -= pItem->lAmount;
        }
    }
    if (pItem->auchDiscStatus[2] & DISC_STATUS_2_REGDISC2) {
        TrnInformation.TranItemizers.lDiscountable[1] -= pItem->lAmount;
        if (fchFood) {
            TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFSDiscountable[1] -= pItem->lAmount;
        }
    }

    /* 5/7/96 if ((pItem->auchStatus[0] & 0x20) == 0) {
        TrnInformation.TranItemizers.lHourly -= pItem->lAmount;
    } */

    if (TrnTaxSystem() == TRN_TAX_CANADA) {                     /* V3.3 */
        TrnItemDiscountCanComn(pItem, &TrnInformation.TranItemizers, TRN_ITEMIZE_REDUCE);
    } else if (TrnTaxSystem() == TRN_TAX_US) {                  /* V3.3 */
        TrnItemDiscountUSComn(pItem, &TrnInformation.TranItemizers, TRN_ITEMIZE_REDUCE);
    } else {                                        /* Int'l VAT,   V3.3 */
        TrnItemDiscountIntlComn(pItem, &TrnInformation.TranItemizers, TRN_ITEMIZE_REDUCE);
    }
}

/*==========================================================================
**    Synopsis: VOID    TrnSplRedItemDiscRD(ITEMDISC *pItem)
*
*    Input:     ITEMDISC    *Item
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   itemize by regular discount
==========================================================================*/
VOID    TrnSplRedItemDiscRD(ITEMDISC *pItem)
{
    UCHAR   fchFood = 0;

    /* correct fs affection, 02/14/01 */
	fchFood = (TrnTaxSystem() == TRN_TAX_US && (pItem->auchDiscStatus[1] & DISC_AFFECT_REGDISC_FOOD));   // Transaction Discount Food Stamp indicator

    TrnInformation.TranItemizers.lMI -= pItem->lAmount;
    if (fchFood) {
        TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFoodStampable -= pItem->lAmount;
	}

    /* 5/7/96 if ((pItem->auchStatus[0] & 0x20) == 0) {
        TrnInformation.TranItemizers.lHourly -= pItem->lAmount;
    } */

    if (TrnTaxSystem() == TRN_TAX_CANADA) {                     /* V3.3 */
		/* TAR191625 */
        /* if ((pItem->fbModifier & VOID_MODIFIER) == 0) { */
            TrnRegDiscTaxCanComn(pItem, 0, TRN_ITEMIZE_REDUCE);
            /* TrnSplRegDiscTaxCan(pItem); */
        /* } */
    } else if (TrnTaxSystem() == TRN_TAX_US) {                  /* V3.3 */
        if ((pItem->fbDiscModifier & VOID_MODIFIER) == 0) {
            if (pItem->uchRate == 0) {
                TrnRegDiscTaxAmtComn(pItem, 0, TRN_ITEMIZE_REDUCE);
                /* TrnSplRegDiscTaxAmt(pItem); */
            } else {
                TrnRegDiscTaxRateComn(pItem, 0, TRN_ITEMIZE_REDUCE);
                /* TrnSplRegDiscTaxRate(pItem); */
            }
        } else {
			/* TAR191625 */
            TrnRegDiscTaxAmtVoidComn(pItem, 0, TRN_ITEMIZE_REDUCE);
        }
    } else {                                            /* Int'l VAT,   V3.3 */
        TrnRegDiscVATCalcComn(pItem, 0, TRN_ITEMIZE_REDUCE);
    }

	/* TAR191625 */
	if ((pItem->uchSeatNo == 0) || (pItem->uchSeatNo == 'B')) {	/* Base Transaction */
	    if (!(pItem->fbDiscModifier & VOID_MODIFIER)) {
    		TrnInformation.TranItemizers.lVoidDiscount[pItem->uchMinorClass - CLASS_REGDISC1] -= pItem->lAmount;
	    } else {
    		TrnInformation.TranItemizers.lVoidDiscount[pItem->uchMinorClass - CLASS_REGDISC1] = 0;
    	}
    }
}

/*==========================================================================
**   Synopsis: VOID    TrnSplRedItemDiscChg(ITEMDISC *pItem)
*
*    Input:     ITEMDISC    *pItem
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   itemize by charge tip
==========================================================================*/
VOID    TrnSplRedItemDiscChg(ITEMDISC *pItem)
{

    TrnInformation.TranItemizers.lMI -= pItem->lAmount;
    /* 5/7/96 TrnInformation.TranItemizers.lHourly -= pItem->lAmount; */

    if (TrnTaxSystem() == TRN_TAX_CANADA) {                     /* V3.3 */
		USHORT    usWorkMod = pItem->fbDiscModifier;

        usWorkMod >>= 1;
        usWorkMod &= TRN_MASKSTATCAN;

        if (usWorkMod == 0) {
            usWorkMod = pItem->auchDiscStatus[1];
            usWorkMod &= TRN_MASKSTATCAN;
			if (usWorkMod > STD_PLU_ITEMIZERS_MOD) {
				usWorkMod = STD_PLU_ITEMIZERS_MOD;
			}
            usWorkMod++;
        }

        TrnInformation.TranItemizers.Itemizers.TranCanadaItemizers.lTaxable[usWorkMod - 1] -= pItem->lAmount;
    }
}

/*==========================================================================
**    Synopsis: VOID    TrnSplReduceItemizerCoupon(ITEMCOUPON *pItem)
*
*    Input:     ITEMCOUPON  *pItemCoupon
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   itemize by combination coupon
==========================================================================*/
VOID    TrnSplReduceItemizerCoupon(ITEMCOUPON *pItem)
{
    UCHAR   fchFood = 0;

    /* correct fs affection, 02/14/01 */
	fchFood = (TrnTaxSystem() == TRN_TAX_US && (pItem->fbStatus[0] & FOOD_MODIFIER));

    if (!(pItem->fbReduceStatus & REDUCE_ITEM)) {

        /* --- update main itemizer --- */
        TrnInformation.TranItemizers.lMI -= pItem->lAmount;

    	/* --- Food Stampable --- */
	    if (fchFood) {
        	TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFoodStampable -= pItem->lAmount;
	        /* --- Taxable #1-3 ---  */
        	if ((pItem->fbStatus[0] & TAX_MODIFIER1) ||           /* Tax #1 */
    	        (pItem->fbStatus[0] & TAX_MODIFIER2) ||           /* Tax #2 */
	            (pItem->fbStatus[0] & TAX_MODIFIER3)) {           /* Tax #3 */
            	TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFoodWholeTax -= pItem->lAmount;
    	    }
	    }
        /* 5/7/96 if (pItem->fbStatus[1] & ITM_COUPON_HOURLY) {
            TrnInformation.TranItemizers.lHourly -= pItem->lAmount;
        } */

        /* --- update discountable itemizer # 1 , 2 --- */
        if (pItem->fbStatus[1] & ITM_COUPON_AFFECT_DISC1) {
            TrnInformation.TranItemizers.lDiscountable[0] -= pItem->lAmount;
	        if (fchFood) {
        	    TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFSDiscountable[0] -= pItem->lAmount;
	        }
        }
        if (pItem->fbStatus[1] & ITM_COUPON_AFFECT_DISC2) {
            TrnInformation.TranItemizers.lDiscountable[1] -= pItem->lAmount;
	        if (fchFood) {
    	        TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFSDiscountable[1] -= pItem->lAmount;
	        }
        }

        if (TrnTaxSystem() == TRN_TAX_CANADA) {                 /* V3.3 */
            TrnItemCouponCanComn(pItem, &TrnInformation.TranItemizers, TRN_ITEMIZE_REDUCE);
        } else if (TrnTaxSystem() == TRN_TAX_US) {              /* V3.3 */
            TrnItemCouponUSComn(pItem, &TrnInformation.TranItemizers, TRN_ITEMIZE_REDUCE);
        } else {                                        /* Int'l VAT,   V3.3 */
            TrnItemCouponIntlComn(pItem, &TrnInformation.TranItemizers, TRN_ITEMIZE_REDUCE);
        }
    }
}

/*==========================================================================
**  Synopsis:   VOID    TrnSplReduceStorageSales(ITEMSALES *pItem)
*                           
*   Input:     
*   Output:     none
*   InOut:      none
*
*   Return:     
*                   
*   Description:    Reduce Item in Consoli Storage.
==========================================================================*/

SHORT   TrnSplReduceStorageSales(ITEMSALES *pItem)
{
	SHORT  sReturn = TRN_SUCCESS;

    switch(pItem->uchMinorClass) {
    case CLASS_DEPT:
    case CLASS_PLU:
    case CLASS_SETMENU:
    case CLASS_OEPPLU:
        /*----- Scalable -----*/
        if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
            TrnSplRedStoSalesMod(pItem);
        } else {
            TrnSplRedStoSales(pItem);                               
        }
        break;

    case CLASS_DEPTITEMDISC:
    case CLASS_PLUITEMDISC:
    case CLASS_SETITEMDISC:
    case CLASS_OEPITEMDISC:
        /*----- This is "Reduction at Seat# Tender" Case, Not Transfer -----*/
        TrnSplRedStoSalesMod(pItem);
        break;

    case CLASS_DEPTMODDISC:
    case CLASS_PLUMODDISC:
    case CLASS_SETMODDISC:
    case CLASS_OEPMODDISC:
        TrnSplRedStoSalesMod(pItem);
        break;

    default:
        break;
    }

	return sReturn;
}

/*==========================================================================
**    Synopsis: VOID    TrnSplRedStoSales(ITEMSALES *pItem)
*
*    Input:     
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   Reduce QTY and Product of Consoli Storage/Index.
==========================================================================*/
SHORT   TrnSplRedStoSales(ITEMSALES *pItem)
{
    UCHAR       uchMaxChild, i;
	SHORT       sReturn;
    ITEMSALES   Sales;
    TRANSTOSALESSRCH    NormalItem = {0}, VoidItem = {0}, CompItem = {0};

	// Let make it easy to find out whether this is a void item
	// or not by marking the fsLabelStatus with ITM_CONTROL_VOIDITEM
	// since this is available in the TRANSTORAGESALESNON struct.
	pItem->fsLabelStatus &= ~ITM_CONTROL_VOIDITEM;
	if (pItem->fbModifier & VOID_MODIFIER) {
		pItem->fsLabelStatus |= ITM_CONTROL_VOIDITEM;
	}

    Sales = *pItem;
    Sales.lQTY *= -1L;
    Sales.lProduct *= -1L;
    Sales.sCouponQTY *= -1L;
    uchMaxChild = pItem->uchChildNo + pItem->uchCondNo + pItem->uchPrintModNo;
    for (i = 0; i < uchMaxChild; i++) {
        Sales.Condiment[i].lUnitPrice *= -1L;
    }

    /*----- adjust item storage buffer 4/22/96 -----*/
    CompItem.usItemSize = TrnStoSalesComp(pItem, CompItem.auchBuffer);

    /*----- Decide Sales Item Offset in Item Storage -----*/
    sReturn = TrnStoSalesSearch(pItem, CompItem.auchBuffer, &NormalItem, &VoidItem, TRN_TYPE_ITEMSTORAGE, -1);
	if (sReturn == TRN_ERROR) {
		NHPOS_ASSERT_TEXT((sReturn != TRN_ERROR), "**ERROR: TrnStoSalesSearch() failed.");
		return sReturn;
	}

    /*----- Exist Sales Item in Item Storage -----*/
    if ((NormalItem.usItemOffset != 0) || ((pItem->fbModifier & VOID_MODIFIER) && VoidItem.usItemOffset)) {
        /*----- Reduce Sales Item in Item Storage -----*/
        TrnStoSalesUpdateRed(&Sales, &CompItem, &NormalItem, &VoidItem, TRN_TYPE_ITEMSTORAGE);
        TrnStoIdxUpdate(&Sales, &NormalItem, &VoidItem, TRN_TYPE_ITEMSTORAGE);
        TrnStoNoIdxUpdate(&Sales, &NormalItem, &VoidItem, TRN_TYPE_NOITEMSTORAGE, REDUCE_ITEM);
    }

    /*----- Decide Sales Item Offset in Consoli Storage -----*/
    sReturn = TrnStoSalesSearch(pItem, CompItem.auchBuffer, &NormalItem, &VoidItem, TRN_TYPE_CONSSTORAGE, -1);
	if (sReturn == TRN_ERROR) {
		NHPOS_ASSERT_TEXT((sReturn != TRN_ERROR), "**ERROR: TrnStoSalesSearch() failed.");
		return sReturn;
	}

    /*----- Reduce Sales Item in Consoli Storage -----*/
    TrnStoSalesUpdateRed(&Sales, &CompItem, &NormalItem, &VoidItem, TRN_TYPE_CONSSTORAGE);
    TrnStoIdxUpdate(&Sales, &NormalItem, &VoidItem, TRN_TYPE_CONSSTORAGE);
    TrnStoNoIdxUpdate(&Sales, &NormalItem, &VoidItem, TRN_TYPE_NOCONSSTORAGE, REDUCE_ITEM);

    /*----- Set "sCouponCo" Amount to Argument -----*/
//    pItem->sCouponCo = Sales.sCouponCo;

	return TRN_SUCCESS;
}

/*==========================================================================
**    Synopsis: VOID    TrnSplRedStoSalesMod(ITEMSALES *pItem)
*
*    Input:     
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   Reduce QTY and Product of Consoli Storage/Index.
==========================================================================*/

VOID    TrnSplRedStoSalesMod(ITEMSALES *pItem)
{
    UCHAR           auchWork[sizeof(ITEMDATASIZEUNION)];
	UCHAR           auchTranStorageWork[sizeof(ITEMDATASIZEUNION)];
    SHORT           sFlag = 0;
	PifFileHandle   hsHandle;
    ULONG           ulCurSize, ulReadPos;
    ITEMSALES       *pSales;

    pSales = (ITEMSALES *)auchTranStorageWork;

    /*----- Search Item from Consoli. Storage Buffer -----*/
	TrnSplDecideStorage(TRN_TYPE_CONSSTORAGE, &ulReadPos, &ulCurSize, &hsHandle);
    while ((ulReadPos < ulCurSize) && (sFlag == 0)) {
		ULONG	ulActualBytesRead;//RPH 11-11-3 SR# 201

        /*----- Read 1 Item -----*/
        //RPH 11-11-3 SR# 201
		TrnReadFile(ulReadPos, auchWork, sizeof(auchWork), hsHandle, &ulActualBytesRead);

        /*----- De-Compress Item -----*/
        memset(auchTranStorageWork, 0, sizeof(ITEMSALES));
        RflGetStorageItem(auchTranStorageWork, auchWork, RFL_WITH_MNEM);

        if (pSales->uchMajorClass == CLASS_ITEMSALES
            && (pSales->uchMinorClass == CLASS_DEPTMODDISC
                || pSales->uchMinorClass == CLASS_PLUMODDISC
                || pSales->uchMinorClass == CLASS_SETMODDISC
                || pSales->uchMinorClass == CLASS_OEPMODDISC
                || pSales->uchMinorClass == CLASS_DEPTITEMDISC          /* 5/30/96 */
                || pSales->uchMinorClass == CLASS_PLUITEMDISC           /* 5/30/96 */
                || pSales->uchMinorClass == CLASS_SETITEMDISC           /* 5/30/96 */
                || pSales->uchMinorClass == CLASS_OEPITEMDISC           /* 5/30/96 */
                || pSales->ControlCode.auchPluStatus[2] & PLU_SCALABLE)) {

            /*----- Check UnZipped Area -----*/
            if ( TrnIsSameZipDataSalesNonQty (&pSales->uchMajorClass, &pItem->uchMajorClass)) {
                if (!(pSales->fbReduceStatus & REDUCE_ITEM)) {
                    pSales->fbReduceStatus |= REDUCE_ITEM;
                    TrnWriteFile(ulReadPos + 1, pSales, sizeof(TRANSTORAGESALESNON), hsHandle); /* ### Mod (2171 for Win32) V1.0 */
                    sFlag = 1;
                    break;                      /* 5/28/96 */
                }
            }
        }

        /*----- Increment Read Pointer -----*/
        ulReadPos += RflGetStorageTranRecordLen(auchWork);
    }

    /*----- Search Item from Item Storage Buffer 5/28/96 -----*/ 
	TrnSplDecideStorage(TRN_TYPE_ITEMSTORAGE, &ulReadPos, &ulCurSize, &hsHandle);
    while (ulReadPos < ulCurSize) {
		ULONG	ulActualBytesRead;//RPH 11-11-3 SR# 201

        /*----- Read 1 Item -----*/
        //RPH 11-11-3 SR# 201
		TrnReadFile(ulReadPos, auchWork, sizeof(auchWork), hsHandle, &ulActualBytesRead);

        /*----- De-Compress Item -----*/
        memset(auchTranStorageWork, 0, sizeof(ITEMSALES));
        RflGetStorageItem(auchTranStorageWork, auchWork, RFL_WITH_MNEM);

        if (pSales->uchMajorClass == CLASS_ITEMSALES
            && (pSales->uchMinorClass == CLASS_DEPTMODDISC
                || pSales->uchMinorClass == CLASS_PLUMODDISC
                || pSales->uchMinorClass == CLASS_SETMODDISC
                || pSales->uchMinorClass == CLASS_OEPMODDISC
                || pSales->uchMinorClass == CLASS_DEPTITEMDISC          /* 5/30/96 */
                || pSales->uchMinorClass == CLASS_PLUITEMDISC           /* 5/30/96 */
                || pSales->uchMinorClass == CLASS_SETITEMDISC           /* 5/30/96 */
                || pSales->uchMinorClass == CLASS_OEPITEMDISC           /* 5/30/96 */
                || pSales->ControlCode.auchPluStatus[2] & PLU_SCALABLE)) {

            /*----- Check UnZipped Area -----*/
            if ( TrnIsSameZipDataSalesNonQty (&pSales->uchMajorClass, &pItem->uchMajorClass)) {
                if (!(pSales->fbReduceStatus & REDUCE_ITEM)) {
                    pSales->fbReduceStatus |= REDUCE_ITEM;
                    TrnWriteFile(ulReadPos + 1, pSales, sizeof(TRANSTORAGESALESNON), hsHandle); /* ### Mod (2171 for Win32) V1.0 */
                    return;
                }
            }
        }

        /*----- Increment Read Pointer -----*/
        ulReadPos += RflGetStorageTranRecordLen(auchWork);
    }
}

/*==========================================================================
**    Synopsis: VOID    TrnSplReduceStorageDisc(ITEMDISC *pItem)
*
*    Input:     
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   Reduce QTY and Product of Consoli Storage.
==========================================================================*/

VOID    TrnSplReduceStorageDisc(ITEMDISC *pItem)
{
	PifFileHandle   hsHandle;
    ULONG           ulCurSize, ulReadPos;
    SHORT           sFlag = 0;
    UCHAR           auchWork[sizeof(ITEMDATASIZEUNION)];

    /*----- Search Item from Consoli. Storage Buffer -----*/
	TrnSplDecideStorage(TRN_TYPE_CONSSTORAGE, &ulReadPos, &ulCurSize, &hsHandle);
    while ((ulReadPos < ulCurSize) && (sFlag == 0)) {
		ULONG	ulActualBytesRead;//RPH 11-11-3 SR# 201

        /*----- Read 1 Item -----*/
        //RPH 11-11-3 SR# 201
		TrnReadFile(ulReadPos, auchWork, sizeof(auchWork), hsHandle, &ulActualBytesRead);

        switch(TRANSTORAGEDATAPTR(auchWork)->uchMajorClass) {
        case    CLASS_ITEMDISC:
            if (memcmp(TRANSTORAGENONPTR(auchWork), pItem, sizeof(TRANSTORAGEDISCNON)) == 0) {
				UCHAR           auchTranWork[sizeof(ITEMSALES)];
				ITEMDISC        *pDisc = (ITEMDISC *)auchTranWork;

                /*----- De-Compress Item -----*/
                memset(auchTranWork, 0, sizeof(ITEMSALES));
                RflGetStorageItem(auchTranWork, auchWork, RFL_WITH_MNEM);

                /*----- Already Reduce ? -----*/
                if (pDisc->fbReduceStatus & REDUCE_ITEM || memcmp(&pDisc->auchPLUNo, &pItem->auchPLUNo, sizeof(TRANSTORAGEDISC) - sizeof(ITEMVOIDDISC)) != 0) {
                    /* || memcmp(&pDisc->usPLUNo, &pItem->usPLUNo, sizeof(TRANSTORAGEDISC)) != 0) { */
                    break;
                }

                pDisc->fbReduceStatus |= REDUCE_ITEM;
                TrnWriteFile(ulReadPos + 1, pDisc, sizeof(TRANSTORAGEDISCNON), hsHandle); /* ### Mod (2171 for Win32) V1.0 */

                sFlag = 1;
                break;
            }
            break;

        default:
            break;
        }

        /*----- Increment Read Pointer -----*/
        ulReadPos += RflGetStorageTranRecordLen(auchWork);
    }
    
    /*----- Search Item from Item Storage Buffer 5/28/96 -----*/ 
	TrnSplDecideStorage(TRN_TYPE_ITEMSTORAGE, &ulReadPos, &ulCurSize, &hsHandle);
    while (ulReadPos < ulCurSize) {
		ULONG	ulActualBytesRead;//RPH 11-11-3 SR# 201

        /*----- Read 1 Item -----*/
        //RPH 11-11-3 SR# 201
		TrnReadFile(ulReadPos, auchWork, sizeof(auchWork), hsHandle, &ulActualBytesRead);

        switch(TRANSTORAGEDATAPTR(auchWork)->uchMajorClass) {
        case    CLASS_ITEMDISC:
            if (memcmp(TRANSTORAGENONPTR(auchWork), pItem, sizeof(TRANSTORAGEDISCNON)) == 0) {
				UCHAR           auchTranWork[sizeof(ITEMSALES)];
				ITEMDISC        *pDisc = (ITEMDISC *)auchTranWork;

                /*----- De-Compress Item -----*/
                memset(auchTranWork, 0, sizeof(ITEMSALES));
                RflGetStorageItem(auchTranWork, auchWork, RFL_WITH_MNEM);

                /*----- Already Reduce ? -----*/
                if (pDisc->fbReduceStatus & REDUCE_ITEM || memcmp(&pDisc->auchPLUNo, &pItem->auchPLUNo, sizeof(TRANSTORAGEDISC) - sizeof(ITEMVOIDDISC)) != 0) {
                    /* || memcmp(&pDisc->usPLUNo, &pItem->usPLUNo, sizeof(TRANSTORAGEDISC)) != 0) { */
                    break;
                }

                pDisc->fbReduceStatus |= REDUCE_ITEM;
                TrnWriteFile(ulReadPos + 1, pDisc, sizeof(TRANSTORAGEDISCNON), hsHandle); /* ### Mod (2171 for Win32) V1.0 */
                return;
            }
            break;

        default:
            break;
        }

        /*----- Increment Read Pointer -----*/
        ulReadPos += RflGetStorageTranRecordLen(auchWork);
    }
}

/*==========================================================================
**    Synopsis: VOID    TrnSplReduceStorageCoupon(ITEMCOUPON *pItem)
*
*    Input:     
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   Reduce QTY and Product of Consoli Storage.
==========================================================================*/

VOID    TrnSplReduceStorageCoupon(ITEMCOUPON *pItem)
{
    UCHAR           auchWork[TRN_WORKBUFFER];
    PifFileHandle   hsHandle;
    ULONG           ulCurSize, ulReadPos;
	USHORT          usCpnCmpLen = offsetof(ITEMCOUPON, usItemOffset);  // non-compressed data length for comparison.
    SHORT           sFlag = 0;

	TrnSplDecideStorage(TRN_TYPE_CONSSTORAGE, &ulReadPos, &ulCurSize, &hsHandle);
	while ((ulReadPos < ulCurSize) && (sFlag == 0)) {
		ULONG	ulActualBytesRead;//RPH 11-11-3 SR# 201

        /*----- Read 1 Item -----*/
        //RPH 11-11-3 SR# 201
		TrnReadFile(ulReadPos, auchWork, sizeof(auchWork), hsHandle, &ulActualBytesRead);

        switch(TRANSTORAGEDATAPTR(auchWork)->uchMajorClass) {
        case    CLASS_ITEMCOUPON:
            /*----- Compare from "uchMajorClass" to "sVoidCo" -----*/
            if (memcmp(TRANSTORAGENONPTR(auchWork), pItem, usCpnCmpLen) == 0) {  /* uchMajorClass + uchMinorClass + uchSeatNo + fbReduceStatus + uchCouponNo + auchUPCCouponNo+ sVoidCo = 7 */
				UCHAR           auchTranWork[sizeof(ITEMSALES)] = {0};
				ITEMCOUPON      *pCpn = (ITEMCOUPON *)auchTranWork;

                /*----- De-Compress Item -----*/
                RflGetStorageItem(auchTranWork, auchWork, RFL_WITH_MNEM);

                /*----- Already Reduce ? -----*/
                if (pCpn->fbReduceStatus & REDUCE_ITEM || memcmp(&pCpn->lAmount, &pItem->lAmount, sizeof(TRANSTORAGECOUPON)) != 0) {
                    break;
                }

                pCpn->fbReduceStatus |= REDUCE_ITEM;
                TrnWriteFile(ulReadPos + 1, pCpn, sizeof(TRANSTORAGECOUPONNON), hsHandle); /* ### Mod (2171 for Win32) V1.0 */

                sFlag = 1;
                break;
            }
            break;

        default:
            break;
        }

        /*----- Increment Read Pointer -----*/
        ulReadPos += RflGetStorageTranRecordLen(auchWork);
    }

    /*----- Search Item from Item Storage Buffer 5/28/96 -----*/ 
	TrnSplDecideStorage(TRN_TYPE_ITEMSTORAGE, &ulReadPos, &ulCurSize, &hsHandle);
    while (ulReadPos < ulCurSize) {
		ULONG	ulActualBytesRead;//RPH 11-11-3 SR# 201

        /*----- Read 1 Item -----*/
        //RPH 11-11-3 SR# 201
		TrnReadFile(ulReadPos, auchWork, sizeof(auchWork), hsHandle, &ulActualBytesRead);

        switch(TRANSTORAGEDATAPTR(auchWork)->uchMajorClass) {
        case    CLASS_ITEMCOUPON:
            /*----- Compare from "uchMajorClass" to "sVoidCo" -----*/
            if (memcmp(TRANSTORAGENONPTR(auchWork), pItem, usCpnCmpLen) == 0) {
				UCHAR           auchTranWork[sizeof(ITEMSALES)] = {0};
				ITEMCOUPON      *pCpn = (ITEMCOUPON *)auchTranWork;

                /*----- De-Compress Item -----*/
                RflGetStorageItem(auchTranWork, auchWork, RFL_WITH_MNEM);

                /*----- Already Reduce ? -----*/
                if (pCpn->fbReduceStatus & REDUCE_ITEM || memcmp(&pCpn->lAmount, &pItem->lAmount, sizeof(TRANSTORAGECOUPON)) != 0) {
                    break;
                }

                pCpn->fbReduceStatus |= REDUCE_ITEM;
                TrnWriteFile(ulReadPos + 1, (VOID *)pCpn, sizeof(TRANSTORAGECOUPONNON), hsHandle); /* ### Mod (2171 for Win32) V1.0 */
                return;
            }
            break;

        default:
            break;
        }

        /*----- Increment Read Pointer -----*/
        ulReadPos += RflGetStorageTranRecordLen(auchWork);
    }
}

/*
*===========================================================================
** Synopsis:    VOID    TrnSplReduceStorageSalesDisc(ITEMSALES *pSales,
*                           ITEMDISC *pDisc)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description:     Reduce Sales and Item Discount of Transfer Item.
*===========================================================================
*/
VOID    TrnSplReduceStorageSalesDisc(ITEMSALES *pSales, ITEMDISC *pDisc)
{
    UCHAR           auchWork[TRN_WORKBUFFER];
	UCHAR           auchTranStorageWork[sizeof(ITEMDATASIZEUNION)];
    SHORT           sFlag = 0;
    PifFileHandle   hsHandle;
    ULONG           ulCurSize, ulReadPos;
    TRANSTORAGEDISCNON      *pDiscNon = TRANSTORAGENONPTR(auchWork);
    ITEMSALES               *pItemSales = (ITEMSALES *)auchTranStorageWork;

    /*----- Search Item from Consoli. Storage Buffer -----*/
	TrnSplDecideStorage(TRN_TYPE_CONSSTORAGE, &ulReadPos, &ulCurSize, &hsHandle);
    while ((ulReadPos < ulCurSize) && (sFlag == 0)) {
		ULONG	        ulActualBytesRead;//RPH 11-11-3 SR# 201

        /*----- Read 1 Item -----*/
        //RPH 11-11-3 SR# 201
		TrnReadFile(ulReadPos, auchWork, sizeof(auchWork), hsHandle, &ulActualBytesRead);

        /*----- De-Compress Item -----*/
        memset(auchTranStorageWork, 0, sizeof(ITEMSALES));
        RflGetStorageItem(auchTranStorageWork, auchWork, RFL_WITH_MNEM);

        switch(pItemSales->uchMajorClass) {
        case    CLASS_ITEMSALES:
            if (pItemSales->uchMinorClass != CLASS_DEPTITEMDISC
                && pItemSales->uchMinorClass != CLASS_PLUITEMDISC
                && pItemSales->uchMinorClass != CLASS_SETITEMDISC
                && pItemSales->uchMinorClass != CLASS_OEPITEMDISC) {
                break;
            }
            /*----- Check UnZipped Area -----*/
            if ( TrnIsSameZipDataSalesNonQty (&pItemSales->uchMajorClass, &pSales->uchMajorClass)) {
                if (!(pItemSales->fbReduceStatus & REDUCE_ITEM)) {
					ULONG    ulReadPosSale = ulReadPos;  // remember position this sales data so as to increment to discount following
					TRANSTORAGESALESNON     SalesNon;

                    if (pItemSales->fbReduceStatus & REDUCE_NOT_HOURLY) {
                        if (!(pSales->fbReduceStatus & REDUCE_NOT_HOURLY)) {
                            break;
                        }
                    } else {
                        if (pItemSales->fbReduceStatus & REDUCE_NOT_HOURLY) {
                            break;
                        }
                    }

					// make a copy the non-compressed portion of the ITEMSALES freeing up the read buffer
					// for the discount item, that may follow the sales item data. ITEMDISC for
					// an ITEMSALES is always stored immediately after the ITEMSALES it modifies.
                    memcpy(&SalesNon, pItemSales, sizeof(TRANSTORAGESALESNON));
                    ulReadPos += RflGetStorageTranRecordLen(auchWork);  // increment to after the sales data to read the discount
                    //RPH 11-11-3 SR# 201
					TrnReadFile(ulReadPos, auchWork, sizeof(auchWork), hsHandle, &ulActualBytesRead);
                    if (pDiscNon->uchMinorClass != CLASS_ITEMDISC1) {
                        break;
                    }

                    /*----- Check Unzipped Data -----*/
                    if ( ! TrnIsSameZipDataCoupon (pDiscNon, pDisc)) {
                        break;
                    }
                    /*----- Reduced Item -----*/
                    if (pDiscNon->fbReduceStatus & REDUCE_ITEM) {
                        break;
                    }
                    /*----- Check Zipped Data -----*/
                    /*----- De-Compress Item -----*/
                    memset(auchTranStorageWork, 0, sizeof(ITEMSALES));
                    RflGetStorageItem(auchTranStorageWork, auchWork, RFL_WITH_MNEM);

                    if (memcmp(auchTranStorageWork + sizeof(TRANSTORAGEDISCNON), &pDisc->auchPLUNo, sizeof(TRANSTORAGEDISC)) != 0) {
                        break;
                    }

                    /*----- Set "Reduce" Bit to Sales/Discount -----*/
                    SalesNon.fbReduceStatus |= REDUCE_ITEM;
                    TrnWriteFile((ulReadPosSale + 1), &SalesNon, sizeof(TRANSTORAGESALESNON), hsHandle); /* ### Mod (2171 for Win32) V1.0 */

                    pDiscNon->fbReduceStatus |= REDUCE_ITEM;
                    TrnWriteFile((ulReadPos + 1), pDiscNon, sizeof(TRANSTORAGEDISCNON), hsHandle); /* ### Mod (2171 for Win32) V1.0 */

                    sFlag = 1;
                    break;
                }
            }
            break;

        default:
            break;
        }

        /*----- Increment Read Pointer -----*/
        ulReadPos += RflGetStorageTranRecordLen(auchWork);
    }

    /*----- Search Item from Item Storage Buffer -----*/ 
	TrnSplDecideStorage(TRN_TYPE_ITEMSTORAGE, &ulReadPos, &ulCurSize, &hsHandle);
    while (ulReadPos < ulCurSize) {
		ULONG	        ulActualBytesRead;//RPH 11-11-3 SR# 201

        /*----- Read 1 Item -----*/
        //RPH 11-11-3 SR# 201
		TrnReadFile(ulReadPos, auchWork, sizeof(auchWork), hsHandle, &ulActualBytesRead);

        /*----- De-Compress Item -----*/
        memset(auchTranStorageWork, 0, sizeof(ITEMSALES));
        RflGetStorageItem(auchTranStorageWork, auchWork, RFL_WITH_MNEM);

        switch(pItemSales->uchMajorClass) {
        case    CLASS_ITEMSALES:
            if (pItemSales->uchMinorClass != CLASS_DEPTITEMDISC
                && pItemSales->uchMinorClass != CLASS_PLUITEMDISC
                && pItemSales->uchMinorClass != CLASS_SETITEMDISC
                && pItemSales->uchMinorClass != CLASS_OEPITEMDISC) {
                break;
            }
            /*----- Check UnZipped Area -----*/
            if ( TrnIsSameZipDataSalesNonQty (&pItemSales->uchMajorClass, &pSales->uchMajorClass)) {
                if (!(pItemSales->fbReduceStatus & REDUCE_ITEM)) {
					ULONG  ulReadPosSale = ulReadPos;  // remember position this sales data so as to increment to discount following
					TRANSTORAGESALESNON     SalesNon;

                    if (pItemSales->fbReduceStatus & REDUCE_NOT_HOURLY) {
                        if (!(pSales->fbReduceStatus & REDUCE_NOT_HOURLY)) {
                            break;
                        }
                    } else {
                        if (pItemSales->fbReduceStatus & REDUCE_NOT_HOURLY) {
                            break;
                        }
                    }

					// make a copy the non-compressed portion of the ITEMSALES freeing up the read buffer
					// for the discount item, that may follow the sales item data. ITEMDISC for
					// an ITEMSALES is always stored immediately after the ITEMSALES it modifies.
                    memcpy(&SalesNon, pItemSales, sizeof(TRANSTORAGESALESNON));
                    ulReadPos += RflGetStorageTranRecordLen(auchWork);  // increment to after the sales data to read the discount
                    //RPH 11-11-3 SR# 201
					TrnReadFile(ulReadPos, auchWork, sizeof(auchWork), hsHandle, &ulActualBytesRead);
                    /*----- Check Unzipped Data -----*/
                    if ( ! TrnIsSameZipDataCoupon (pDiscNon, pDisc)) {
                        break;
                    }
                    /*----- Reduced Item -----*/
                    if (pDiscNon->fbReduceStatus & REDUCE_ITEM) {
                        break;
                    }
                    /*----- Check Zipped Data -----*/
                    /*----- De-Compress Item -----*/
                    memset(auchTranStorageWork, 0, sizeof(ITEMSALES));
                    RflGetStorageItem(auchTranStorageWork, auchWork, RFL_WITH_MNEM);

                    if (memcmp(auchTranStorageWork + sizeof(TRANSTORAGEDISCNON), &pDisc->auchPLUNo, sizeof(TRANSTORAGEDISC)) != 0) {
                        break;
                    }

                    /*----- Set "Reduce" Bit to Sales/Discount -----*/
                    SalesNon.fbReduceStatus |= REDUCE_ITEM;
                    TrnWriteFile((ulReadPosSale + 1), &SalesNon, sizeof(TRANSTORAGESALESNON), hsHandle); /* ### Mod (2171 for Win32) V1.0 */

                    pDiscNon->fbReduceStatus |= REDUCE_ITEM;
                    TrnWriteFile(ulReadPos + 1, pDiscNon, sizeof(TRANSTORAGEDISCNON), hsHandle); /* ### Mod (2171 for Win32) V1.0 */

                    sFlag = 1;
                    break;
                }
            }
            break;

        default:
            break;
        }

        /*----- Increment Read Pointer -----*/
        ulReadPos += RflGetStorageTranRecordLen(auchWork);
    }
}

/*
*==========================================================================
**   Synopsis:  SHORT TrnStoSalesUpdateRed(ITEMSALES *pItemSales,
*                                        UCHAR     *puchSrcZip,
*                                        TRANSTOSALESSRCH *pNormalItem,
*                                        TRANSTOSALESSRCH *pVoidItem,
*                                        SHORT     sStorageType )
*
*       Input:  ITEMSALES *pItemSales  - address of unzipped source data
*               UCHAR     *puchSrcZip  - address of zipped source data
*               TRANSTOSALESSRCH *pNormalItem - address of normal item
*               TRANSTOSALESSRCH *pVoidItem - address of void item
*               SHORT     sStorageType - type of transaction storage
*                           TRN_TYPE_ITEMSTORAGE - item storage
*                           TRN_TYPE_CONSSTORAGE - consolidation storage
*      Output:  Nothing
*       InOut:  Nothing
*
*      Return:  TRN_SUCCESS     : item found and consolidate it
*               TRN_ERROR       : not found
*
**  Description:
*           search target "item" at item/consoli. storage
*           if find it, consolidate item/consoli. storage
*==========================================================================
*/
SHORT   TrnStoSalesUpdateRed(ITEMSALES      *pItemSales,
                         TRANSTOSALESSRCH   *pCompItem,
                         TRANSTOSALESSRCH   *pNormalItem,
                         TRANSTOSALESSRCH   *pVoidItem,
                         SHORT              sStorageType)
{
	// auchBuffer is a buffer containing the original, compressed transaction record from the transaction file.
	// first byte of the compressed transaction record indicates length of the noncomprressed segment.
    TRANSTORAGESALESNON *pNoCompNormal = TRANSTORAGENONPTR(pNormalItem->auchBuffer);
    TRANSTORAGESALESNON *pNoCompVoid = TRANSTORAGENONPTR(pVoidItem->auchBuffer);
    TRANSTORAGESALESNON *pNoCompSrc = TRANSTORAGENONPTR(pCompItem->auchBuffer);
	LONG                lVoidItemQty = 0;
    SHORT               sSrcCpnQTY, sVoidCpnCo = 0;

    /* --- calculate sales item's coupon quantity --- */
    if (pVoidItem->usItemOffset) {
		lVoidItemQty  = pNoCompVoid->lQTY;
	}

    /* --- store source coupon quantity --- */
    sSrcCpnQTY = pNoCompSrc->sCouponQTY;

    if ( pNormalItem->usItemOffset ) {
		SHORT   sReturn;
		SHORT   hsStorage;
		USHORT  usStorageVli;
		USHORT  usStorageHdrSize;

		sReturn = TrnStoGetStorageInfo( sStorageType, &hsStorage, &usStorageHdrSize, &usStorageVli );
		if (sReturn != TRN_SUCCESS) {
			return ( sReturn );
		}

		pNoCompNormal->lQTY += pItemSales->lQTY;
		pNoCompNormal->lProduct += pItemSales->lProduct;

        pNoCompNormal->sCouponQTY = ( SHORT )(( pNoCompNormal->lQTY + lVoidItemQty ) / 1000 );
        sVoidCpnCo = (pNoCompNormal->sCouponQTY - pNoCompNormal->sCouponCo);
        if (sVoidCpnCo < 0) {
            TrnStoSalesUpdateRedCpn(sVoidCpnCo, pNormalItem->usItemOffset);
        }

        /* --- update normal item data --- */
        TrnWriteFile( pNormalItem->usItemOffset, pNormalItem->auchBuffer, sizeof( UCHAR ) + sizeof( TRANSTORAGESALESNON ), hsStorage );
    }

    /* --- restore source coupon quantity --- */
    pNoCompSrc->sCouponQTY = sSrcCpnQTY;

    /*----- Save "sCouponCo" for Transfer,   R3.1 -----*/
    if (sStorageType == TRN_TYPE_CONSSTORAGE) {
        pItemSales->sCouponCo = sVoidCpnCo;
    }

    return ( TRN_SUCCESS );
}


/*==========================================================================
**    Synopsis: VOID    TrnStoSalesUpdateRedCpn(SHORT sVoidCpnCo, USHORT usItemOff)
*
*    Input:     
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   Change DEpt/PLU Offset in Coupon Item.
==========================================================================*/

VOID    TrnStoSalesUpdateRedCpn(SHORT sVoidCpnCo, USHORT usItemOff)
{
    UCHAR           auchWork[sizeof(ITEMDATASIZEUNION)];
    SHORT           sCo = sVoidCpnCo, i;
    TRANSTORAGECOUPONNON   *pCpn = TRANSTORAGENONPTR(auchWork);
    PifFileHandle   hsHandle;
    ULONG           ulCurSize, ulReadPos;

	TrnSplDecideStorage(TRN_TYPE_CONSSTORAGE, &ulReadPos, &ulCurSize, &hsHandle);

    while ((ulReadPos < ulCurSize) && (sCo != 0)) {
		ULONG	ulActualBytesRead;//RPH 11-11-3 SR# 201

        /*----- Read 1 Item -----*/
        //RPH 11-11-3 SR# 201
		TrnReadFile(ulReadPos, auchWork, sizeof(auchWork), hsHandle, &ulActualBytesRead);
        switch(TRANSTORAGEDATAPTR(auchWork)->uchMajorClass) {
        case    CLASS_ITEMCOUPON:
            if ((pCpn->fbReduceStatus & REDUCE_ITEM) || pCpn->sVoidCo != 0) {
                break;
            }
            for (i = 0; i < STD_MAX_NUM_PLUDEPT_CPN && sCo != 0; i++) {
                if (pCpn->usItemOffset[i] == usItemOff) {
                    pCpn->usItemOffset[i] = TrnInformation.usTranConsStoVli;
                    sCo--;
                }
            }
			// update the non-compressed portion of the coupon transaction data record
            TrnWriteFile(ulReadPos + 1, pCpn, sizeof(TRANSTORAGECOUPONNON), hsHandle); /* ### Mod (2171 for Win32) V1.0 */
            break;

        default:
            break;
        }

        /*----- Increment Read Pointer -----*/
        ulReadPos += RflGetStorageTranRecordLen(auchWork);
    }
}


/*==========================================================================
**  Synopsis:   VOID    TrnSplDecideStorage(SHORT sStorage,
*                   ULONG *ulReadPos, ULONG *ulCurSize, USHORT *husHandle)
*   Input:      
*   Output:     none
*   InOut:      none
*   Return:     none
*
*   Description:    V3.3
==========================================================================*/
SHORT    TrnSplDecideStorage(TrnStorageType sStorageType, ULONG *ulReadPos, ULONG *ulCurSize, PifFileHandle *hsHandle)
{
	SHORT         sReturn = TRN_SUCCESS;
	USHORT        usHdrSize = 0;
	TrnVliOffset  usVliSize = 0;

	if ((sReturn = TrnStoGetStorageInfo(sStorageType, hsHandle, &usHdrSize, &usVliSize )) == TRN_SUCCESS) {
        *ulReadPos = usHdrSize;
        *ulCurSize = *ulReadPos + usVliSize;
	} else {
		NHPOS_ASSERT_TEXT("**ERROR", "**ERROR: TrnSplDecideStorage() Unknown sStorage type");
        *ulReadPos = sizeof(TRANCONSSTORAGEHEADER);
        *ulCurSize = *ulReadPos + TrnInformation.usTranConsStoVli;
        *hsHandle  = TrnInformation.hsTranConsStorage;
	}

	return sReturn;
}

/* ===== End of File ( TRNSPLT2.C ) ===== */
