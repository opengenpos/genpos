/*=======================================================================*\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  2170         *             AT&T GIS Corporation, E&M OISO   ||
||        * << ECR-90 >>  *                                              ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
:   Title          : TRANSACTION SPLIT CHECK MODULE
:   Category       : TRANSACTION MODULE, NCR 2170 US Hospitality Application
:   Program Name   : TRNSPLT1.C
* --------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen)
*       2012  -> GenPOS Rel 2.2.0
*       2014  -> GenPOS Rel 2.2.1
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
:  ---------------------------------------------------------------------
:  Abstract:
:
:  ---------------------------------------------------------------------
:  Update Histories
:  Date     : Ver.Rev. :   Name     : Description
: Nov-28-95 : 03.01.00 :  hkato     : R3.1
: Aug-16-99 : 03.05.00 :  K.Iwata   : R3.5 merge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
* Nov-21-15 : 02.02.01 : R.Chambers : using new typedefs for variables and source cleanup.
* Jan-13-16 : 02.02.01 : R.Chambers : transaction storage file index change from USHORT to ULONG.
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
#include    <pmg.h>

/*
*==========================================================================
**  Synopsis:   SHORT   TrnSplCheckSplit(VOID)
*
*       Input:  None
*      Output:  None
*       InOut:  None
*
*    Return:    
*
**  Description:    Return Split Check Status/Condition.
*==========================================================================
*/
SHORT   TrnSplCheckSplit(VOID)
{
    /*----- Seat# Transaction State -----*/
    if (TrnInformation.fsTransStatus & TRN_STATUS_SEAT_TRANS) {
        return(TRN_SPL_SEAT);
    }

    /*----- Multi Seat# Transaction State -----*/
    if (TrnInformation.fsTransStatus & TRN_STATUS_MULTI_TRANS) {
        return(TRN_SPL_MULTI);
    }

    /*----- Type 2 State -----*/
    if (TrnInformation.fsTransStatus & TRN_STATUS_TYPE2_TRANS) {
        return(TRN_SPL_TYPE2);
    }

    /*----- Check Used Split Key -----*/
    if (TrnInformation.TranCurQual.uchSplit == CLASS_SPLIT) {
        return(TRN_SPL_SPLIT);
	}

    /*----- Check Finalize Seat# Transaction -----*/
	{
		SHORT   i;

		for (i = 0; i < NUM_SEAT; i++) { //SR206
			if (TrnSplitA.TranGCFQual.auchFinSeat[i] != 0) {
				return(TRN_SPL_BASE);
			}
		}
	}

    /*----- Base Transaction State -----*/
    return(TRN_SUCCESS);
}


/*
*===========================================================================
** Synopsis:    VOID    TrnSplDispSeatNo(UCHAR *uchMiddle, UCHAR *uchRight)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description:  Determine the next seat numbers to be displayed in the LCD #2 and LCD #3
*                display windows.
*===========================================================================
*/
static VOID    TrnSplDispSeatNo(UCHAR *uchMiddle, UCHAR *uchRight)
{
    UCHAR   auchSeat[NUM_SEAT];
    SHORT   i;

	// make a copy of the in use seat queue, containing the seats in use in the transaction.
	// remove from the copy of the in use seat queue those seats that have already been finalized
	// by repeatedly calling the remove seat from seat queue function specifying each finalized seat.
    memcpy(auchSeat, TrnInformation.TranGCFQual.auchUseSeat, sizeof(auchSeat));
    for (i = 0; i < NUM_SEAT && TrnInformation.TranGCFQual.auchFinSeat[i] != 0; i++) {
        TrnSplDispSeatNo1(TrnInformation.TranGCFQual.auchFinSeat[i], auchSeat);
    }

    *uchMiddle = auchSeat[0];
    *uchRight = auchSeat[1];
}

/*
*==========================================================================
**  Synopsis:   VOID    TrnSplCheck(VOID)
*
*       Input:  
*      Output:  None
*       InOut:  None
*
*    Return:    
*
**  Description:    Create Two Split Checks in File Area by copying the data for
*                   specific seats into the two split check areas.
*                   This makes a copy of the data for the purposes of displaying
*                   the transaction data for specific seats in LCD #2 and LCD #3
*                   so the original transaction data is not modified or reduced.
*==========================================================================
*/
VOID    TrnSplCheck(VOID)
{
	ITEMTRANSOPEN   TransOpen = {0};
    UCHAR   auchWork[sizeof(ITEMDATASIZEUNION)];        // compressed transaction item data record buffer
    UCHAR   uchMiddleSeat = 0, uchRightSeat = 0;
    SHORT   sFlagA = 0, sFlagB = 0;
	SHORT   hsHandle;
    ULONG   ulCurSize, ulReadPos;
	ULONG   ulActualBytesRead;
	TRANSTORAGERECORD  *pauchWork = (TRANSTORAGERECORD  *)auchWork;

    /*----- Initialize Both Split File A and B since we will display up to two seats at a time -----*/
    TrnInitializeSpl(TRANI_GCFQUAL_SPLA | TRANI_ITEMIZERS_SPLA | TRANI_CONSOLI_SPLA);
    TrnInitializeSpl(TRANI_GCFQUAL_SPLB | TRANI_ITEMIZERS_SPLB | TRANI_CONSOLI_SPLB);

    /*----- Decide Seat Numbers to display in LDC #2 and LCD #3 -----*/
    TrnSplDispSeatNo(&uchMiddleSeat, &uchRightSeat);

    /*----- Search First Seat# Item in Consoli Storage -----*/
	TrnSplDecideStorage(TRN_TYPE_CONSSTORAGE, &ulReadPos, &ulCurSize, &hsHandle);
	while (ulReadPos < ulCurSize) {
        /*----- Read 1 Item -----*/
		//11-1103- SR 201 JHHJ
		// read the transaction record into the buffer and process it through the pointer TRANSTORAGERECORD  *pauchWork
		// which overlays the buffer area with the necessary struct to access the interesting parts.
        TrnReadFile(ulReadPos, auchWork, sizeof(auchWork), hsHandle, &ulActualBytesRead);

		if (pauchWork->uchMajorClass == CLASS_ITEMTRANSOPEN) {
			/*----- Transaction Open Data for each Split A and Split B window -----*/
            RflGetStorageItem(&TransOpen, pauchWork, RFL_WITH_MNEM);
        } else {
            /*----- Exist Seat# -----*/
			if (pauchWork->uchSeatNo != 0) {
				switch(pauchWork->uchMajorClass) {
                case    CLASS_ITEMSALES:
                case    CLASS_ITEMDISC:
                case    CLASS_ITEMCOUPON:
                    /*----- Send to Split File if seat number is one of the two we are looking for  -----*/
                    if (pauchWork->uchSeatNo == uchMiddleSeat || pauchWork->uchSeatNo == uchRightSeat) {
						UCHAR   auchTranStorageWork[sizeof(ITEMSALES)] = {0};     // uncompressed transaction item data
						SHORT   sType;

                        if (pauchWork->uchSeatNo == uchMiddleSeat) {
                            sType = TRN_TYPE_SPLITA;
							if (sFlagA == 0) {
								// if we have not inserted a transaction open into this split file then do so.
								// we need to specify which seat this split file contains as part of open.
								TransOpen.uchSeatNo = uchMiddleSeat;
								TrnOpenSpl(&TransOpen, sType);
								sFlagA = 1;
							}
                        } else {
                            sType = TRN_TYPE_SPLITB;
							if (sFlagB == 0) {
								// if we have not inserted a transaction open into this split file then do so.
								// we need to specify which seat this split file contains as part of open.
								TransOpen.uchSeatNo = uchRightSeat;
								TrnOpenSpl(&TransOpen, sType);
								sFlagB = 1;
							}
                        }

                        /*----- De-Compress Item then store it into the proper split transaction file -----*/
                        RflGetStorageItem(auchTranStorageWork, pauchWork, RFL_WITH_MNEM);
                        TrnItemSpl(auchTranStorageWork, sType);
                        break;
                    }
                    break;

                default:
                    break;
                }
            }
        }

        /*----- Increment Read Pointer -----*/
        ulReadPos += RflGetStorageTranRecordLen(auchWork);
    }

    /*----- Seat# on LCD Middle/Right -----*/
    TrnInformation.usGuestNo[0] = uchMiddleSeat;
    TrnInformation.usGuestNo[1] = uchRightSeat;
}


/*
*==========================================================================
**  Synopsis:   SHORT   TrnSplGetSeatTransCheck(UCHAR uchSeat)
*
*       Input:  
*      Output:  None
*       InOut:  None
*
*    Return:    
*
**  Description:    Error Check of Seat# Transaction in Split Check File A.
*==========================================================================
*/
SHORT   TrnSplGetSeatTransCheck(UCHAR uchSeat)
{
    SHORT   i;

    /*----- Error Check -----*/
    if (uchSeat == 0) {
        return(TRN_ERROR);
    }

    /*----- Previous Used Seat# -----*/
    for (i = 0; i < NUM_SEAT; i++) { //SR206
        if (TrnInformation.TranGCFQual.auchFinSeat[i] == uchSeat) {
            return(TRN_ERROR);
        }
    }

    /*----- Used Seat# by Multi -----*/
    for (i = 0; i < NUM_SEAT; i++) { //SR206
        if (TrnSplitA.TranGCFQual.auchFinSeat[i] == uchSeat) {
            return(TRN_ERROR);
        }
    }

    /*----- Not Used Seat# -----*/
    for (i = 0; i < NUM_SEAT; i++) { //SR206
        if (TrnInformation.TranGCFQual.auchUseSeat[i] == uchSeat) {
            break;
        }
        if (TrnInformation.TranGCFQual.auchUseSeat[i] == 0) {
            return(TRN_ERROR);
        }
    }

    return(TRN_SUCCESS);
}

/*
*==========================================================================
**  Synopsis:   VOID    TrnSplGetSeat(UCHAR uchSeat, SHORT sType,
*                           SHORT sUpdate, UCHAR uchStoType)
*
*       Input:  SHORT   sUpdate(0:Not Update, 1:Update, 2:Update Itemizer)
*                       uchStoType(10:Consoli. 11:Post Rec.)
*      Output:  None
*       InOut:  None
*
*    Return:       sSeatBaseFlag - indicating if there is at least one item for the seat.
*                                  0 (false) if not, 1 (true) if there is.
*
**  Description:    Create Seat# from the Transaction in File Area, moving the
*                   seat's transaction data from the main transaction file into a
*                   split file.
*                   This is a move of data and not a copy so we mark the data
*                   being moved from the original file as reduced or eliminated.
*
*                   If the special seat number of 0xff is specified then it means that
*                   seat number is ignored during the procedure of copying items from
*                   the transaction into the destination file (typically Split A).
*==========================================================================
*/
static SHORT   TrnSplMoveReduceRecord (UCHAR uchSeat, TRANSTORAGERECORD  *pauchWork, TrnStorageType  sStorageType, SHORT sUpdate)
{
	UCHAR  uchSeatReal = uchSeat;
	SHORT   sSeatBaseFlag = 0;      // indicates if one or more items match the seat.

	if (uchSeat == 0xff) uchSeatReal = 0;

	if (pauchWork->uchMajorClass == CLASS_ITEMTRANSOPEN) {
		UCHAR           auchTranStorageWork[sizeof(ITEMSALES)] = {0};
		ITEMTRANSOPEN   *pTran = (ITEMTRANSOPEN *)auchTranStorageWork;

        /*----- De-Compress Item -----*/
        RflGetStorageItem(auchTranStorageWork, pauchWork, RFL_WITH_MNEM);
        pTran->uchSeatNo = uchSeatReal;
        TrnOpenSpl(auchTranStorageWork, sStorageType);
    } else {
		if ((uchSeat == 0xff || uchSeat == pauchWork->uchSeatNo)
            && (pauchWork->uchMajorClass == CLASS_ITEMSALES
                || pauchWork->uchMajorClass == CLASS_ITEMDISC
                || pauchWork->uchMajorClass == CLASS_ITEMCOUPON))
		{
			UCHAR           auchTranStorageWork[sizeof(ITEMSALES)] = {0};
			ITEMSALES       *pSales = (ITEMSALES *)auchTranStorageWork;
			ITEMDISC        *pDisc = (ITEMDISC *)auchTranStorageWork;
			ITEMCOUPON      *pCpn = (ITEMCOUPON *)auchTranStorageWork;

            /*----- De-Compress Item -----*/
            RflGetStorageItem(auchTranStorageWork, pauchWork, RFL_WITH_MNEM);
            switch(pauchWork->uchMajorClass) {
            case    CLASS_ITEMSALES:
                /*----- Check Reduced Item - see also TrnSplGetBaseTrans() which has same check -----*/
                if ((pSales->ControlCode.auchPluStatus[2] & PLU_SCALABLE)
                    || pSales->uchMinorClass == CLASS_DEPTMODDISC
                    || pSales->uchMinorClass == CLASS_PLUMODDISC
                    || pSales->uchMinorClass == CLASS_SETMODDISC
                    || pSales->uchMinorClass == CLASS_OEPMODDISC
                    || pSales->uchMinorClass == CLASS_DEPTITEMDISC
                    || pSales->uchMinorClass == CLASS_PLUITEMDISC
                    || pSales->uchMinorClass == CLASS_SETITEMDISC
                    || pSales->uchMinorClass == CLASS_OEPITEMDISC)
				{
                    if (pSales->fbReduceStatus & REDUCE_ITEM) {
                        break;
                    }
                } else {                                                    
                    if ((pSales->sCouponQTY == 0) 
                        && (pSales->lQTY == 0L)             /* 5/16/96 */
                        && (pSales->lProduct == 0L)) {      /* 5/16/96 */
                        break;
                    }
                }

                TrnItemSpl(pSales, sStorageType);
                /*----- Reduce Item from Consoli Storage/Itemizer -----*/
                if (sUpdate == TRN_SPL_UPDATE || sUpdate == TRN_SPL_UPDATE_ITEMIZ) {
                    TrnSplReduce(pSales);
                }
                break;

            case    CLASS_ITEMDISC:
                /*----- Check Reduced Item -----*/
                if (pDisc->fbReduceStatus & REDUCE_ITEM) {
                    break;
                }
                TrnItemSpl(pDisc, sStorageType);
                /*----- Reduce Item from Consoli Storage/Itemizer -----*/
                if (sUpdate == TRN_SPL_UPDATE || sUpdate == TRN_SPL_UPDATE_ITEMIZ) {
                    TrnSplReduce(pDisc);
                }
                break;

            case    CLASS_ITEMCOUPON:
                /*----- Check Reduced Item -----*/
                if (pCpn->fbReduceStatus & REDUCE_ITEM) {
                    break;
                }
                TrnItemSpl(pCpn, sStorageType);
                /*----- Reduce Item from Consoli Storage/Itemizer -----*/
                if (sUpdate == TRN_SPL_UPDATE || sUpdate == TRN_SPL_UPDATE_ITEMIZ) {
                    TrnSplReduce(pCpn);
                }
            }
            sSeatBaseFlag = 1;
        } else { 
			if (pauchWork->uchMajorClass == CLASS_ITEMPRINT) {
				UCHAR           auchTranStorageWork[sizeof(ITEMPRINT)] = {0};

				switch(pauchWork->uchMinorClass) {
                case CLASS_START:
                case CLASS_RESTART:
                case CLASS_PRINT_TRAIN:
                case CLASS_PRINT_PVOID:
                case CLASS_TRAILER1:
                case CLASS_TRAILER2:
                case CLASS_TRAILER3:
                case CLASS_TRAILER4:
                    /*----- De-Compress Item -----*/
                    RflGetStorageItem(auchTranStorageWork, pauchWork, RFL_WITH_MNEM);
                    TrnItemSpl(auchTranStorageWork, sStorageType);
                    break;

                default:
                    break;
                }
            }
        }
	}

	return sSeatBaseFlag;
}

static VOID    TrnSplGetSeat(UCHAR uchSeat, TrnStorageType  sStorageType, SHORT sUpdate, UCHAR uchStoType)
{
    UCHAR           auchWork[sizeof(ITEMDATASIZEUNION)];
	SHORT           hsHandle;
    ULONG           ulCurSize, ulReadPos;
	TRANSTORAGERECORD  *pauchWork = (TRANSTORAGERECORD  *)auchWork;

    if (uchStoType == TRN_CONSOLI_STO) {        /* 6/5/96 */
        /*----- Search First Seat# Item in Consoli Storage -----*/
		TrnSplDecideStorage(TRN_TYPE_CONSSTORAGE, &ulReadPos, &ulCurSize, &hsHandle);
    } else {
		TrnSplDecideStorage(TRN_TYPE_POSTRECSTORAGE, &ulReadPos, &ulCurSize, &hsHandle);
    }

    if (sStorageType == TRN_TYPE_SPLITA) {
		/*----- Initialize Split File A -----*/
        if (sUpdate == TRN_SPL_UPDATE_ITEMIZ) {
            TrnInitializeSpl(TRANI_GCFQUAL_SPLA | TRANI_CONSOLI_SPLA);
        } else {
            TrnInitializeSpl(TRANI_GCFQUAL_SPLA | TRANI_ITEMIZERS_SPLA | TRANI_CONSOLI_SPLA);
        }
    } else {
		/*----- Initialize Split File B -----*/
        if (sUpdate == TRN_SPL_UPDATE_ITEMIZ) {
            TrnInitializeSpl(TRANI_GCFQUAL_SPLB | TRANI_CONSOLI_SPLB);
        } else {
            TrnInitializeSpl(TRANI_GCFQUAL_SPLB | TRANI_ITEMIZERS_SPLB | TRANI_CONSOLI_SPLB);
        }
    }

    while (ulReadPos < ulCurSize) {
		ULONG			ulActualBytesRead;

        /*----- Read 1 Item -----*/
		//11-1103- SR 201 JHHJ
		// read the transaction record into the buffer and process it through the pointer TRANSTORAGERECORD  *pauchWork
		// which overlays the buffer area with the necessary struct to access the interesting parts.
        TrnReadFile(ulReadPos, auchWork, sizeof(auchWork), hsHandle, &ulActualBytesRead);
		TrnSplMoveReduceRecord (uchSeat, pauchWork, sStorageType, sUpdate);
        /*----- Increment Read Pointer -----*/
        ulReadPos += RflGetStorageTranRecordLen(pauchWork);
    }
}

/*
*==========================================================================
**  Synopsis:   SHORT   TrnSplGetSeatTrans(UCHAR uchSeat,
*                           SHORT sUpdate, UCHAR uchStoType, SHORT sSplType)
*
*       Input:  SHORT   sUpdate(0:Not Update, 1:Update)
*                       uchStoType(10:Consoli. 11:Post Rec.)
*      Output:  None    
*       InOut:  None
*
*    Return:    
*
**  Description:    Create Seat# Transaction in Split Check File A.
*==========================================================================
*/
SHORT   TrnSplGetSeatTrans(UCHAR uchSeat,
            SHORT sUpdate, UCHAR uchStoType, TrnStorageType  sStorageType)
{
    /*----- Error Check -----*/
    if (TrnSplGetSeatTransCheck(uchSeat) != TRN_SUCCESS) {
        return(TRN_ERROR);
    }

    /*----- Create Seat# Trans -----*/
    TrnSplGetSeat(uchSeat, sStorageType, sUpdate, uchStoType);

    /*----- Save Current Seat# -----*/
    if (sUpdate == TRN_SPL_UPDATE || sUpdate == TRN_SPL_UPDATE_ITEMIZ) {
        TrnInformation.TranCurQual.uchSeat = uchSeat;
        TrnSplInsertFinSeatQueue(uchSeat);

        /*----- Seat# on LCD Middle/Right -----*/
        TrnInformation.usGuestNo[0] = uchSeat;
    }

    return(TRN_SUCCESS);
}

/*
*==========================================================================
**  Synopsis:   VOID    TrnSplGetDispSeat(TRANINFORMATION **TrnInf,
*                           TRANINFSPLIT **SplitA, TRANINFSPLIT **SplitB)
*
*       Input:  
*      Output:  None
*       InOut:  None
*
*    Return:       a USHORT bit mask indicating status of the split data files.
*                     0x01 - > Middle Seat is set with a seat number
*                     0x02 - > Right Seat is set with a seat number
*                     0x10 - > SplitA has a seat number with data to be displayed
*                     0x20 - > SplitB has a seat number with data to be displayed
*
**  Description:    Determine which Transaction Split memory area is assigned to
*                   which of the two displayed split data areas, Split A and Split B.
*
*                   This function is used to determine which split area contains
*                   which seats when displaying split seats in LCD #2 and LCD #3.
*
*                   Normally a call to this function is then followed by the a call to
*                   MldPutSplitTransToScroll() specifying the LCD scroll window to use.
*
*                   An example of use:
*                      TrnSplGetDispSeat(&TrnInf, &SplitA, &SplitB);
*                
*                      MldDisplayInit(MLD_DRIVE_2 | MLD_DRIVE_3, 0);
*                
*                      //----- Display Base Transaction on LCD Middle -----
*                      if (SplitA->TranGCFQual.usGuestNo != 0) {
*                          MldTran.usVliSize = SplitA->usTranConsStoVli;
*                          MldTran.sFileHandle = SplitA->hsTranConsStorage;
*                          MldTran.sIndexHandle = SplitA->hsTranConsIndex;
*                          MldPutSplitTransToScroll(MLD_SCROLL_2, &MldTran);
*                          MldDispSubTotal(MLD_TOTAL_2, SplitA->TranItemizers.lMI);
*                      }
*==========================================================================
*/
USHORT   TrnSplGetDispSeat(TRANINFORMATION **TrnInf,
                TRANINFSPLIT **SplitA, TRANINFSPLIT **SplitB)
{
	USHORT  usLcdMask = 0;

    if (TrnInformation.usGuestNo[0] == 0 && TrnInformation.usGuestNo[1] == 0) {
		/*----- Not Used Left/Right Arrow Key -----*/
        *SplitA = &TrnSplitA;
        *SplitB = &TrnSplitB;
    } else {
		/*----- Used Left/Right Arrow Key -----*/
        if (TrnInformation.usGuestNo[0] == TrnSplitA.TranGCFQual.usGuestNo) {
			// Middle Seat is in use by Split A so do not swap the pointers to the split files memory area
            *SplitA = &TrnSplitA;
            *SplitB = &TrnSplitB;
        } else {
			// Middle seat is in use by Split B file so swap the pointers to the split file memory areas.
            *SplitA = &TrnSplitB;
            *SplitB = &TrnSplitA;
        }
		usLcdMask |= (TrnInformation.usGuestNo[0] == 0) ? 0 : 0x01;         // Middle Seat set
		usLcdMask |= (TrnInformation.usGuestNo[1] == 0) ? 0 : 0x02;         // Right Seat set
		usLcdMask |= ((*SplitA)->TranGCFQual.usGuestNo == 0) ? 0 : 0x10;    // Middle Seat has displayable data
		usLcdMask |= ((*SplitB)->TranGCFQual.usGuestNo == 0) ? 0 : 0x20;    // Right Seat has displayable data
    }

    *TrnInf = &TrnInformation;

	return usLcdMask;
}

/*
*==========================================================================
**  Synopsis:   SHORT   TrnSplRightArrow(VOID)
*       Input:  
*      Output:  None
*       InOut:  None
*
*    Return:    
*
**  Description:    Create Seat# Transaction for Right Arrow Key.
*==========================================================================
*/
SHORT   TrnSplRightArrow(VOID)
{
    UCHAR   auchWork[sizeof(ITEMDATASIZEUNION)];
	UCHAR   uchMiddleSeat, uchRightSeat;
    SHORT   sType, sFlagA = 0, sFlagB = 0;
    TRANINFSPLIT    *pSpl;
	ITEMTRANSOPEN   TransOpen = {0};
	SHORT           hsHandle;
    ULONG           ulCurSize, ulReadPos;
	ULONG           ulActualBytesRead;
	TRANSTORAGERECORD  *pauchWork = (TRANSTORAGERECORD  *)auchWork;

    /*----- Transaction Open Data for Split B -----*/
    /*----- No Seat# Transaction -----*/
    if (!(TrnInformation.TranGCFQual.fsGCFStatus2 & GCFQUAL_USESEAT)) {
        return(TRN_ERROR);
    }

    /*----- During Seat# Tender -----*/
    if ((TrnInformation.fsTransStatus & TRN_STATUS_SEAT_TRANS) || (TrnInformation.fsTransStatus & TRN_STATUS_MULTI_TRANS)) {
        return(TRN_ERROR);
    }

    /*----- Decide Next Seat# Transaction -----*/
    if (TrnSplRightArrowSeat(&uchMiddleSeat, &uchRightSeat) != TRN_SUCCESS) {
        return(TRN_ERROR);
    }

    /*----- Decide Next File Save Area -----*/
    TrnInformation.usGuestNo[0] = uchMiddleSeat;
    TrnInformation.usGuestNo[1] = uchRightSeat;
    if (uchMiddleSeat == (UCHAR)TrnSplitA.TranGCFQual.usGuestNo) {
        TrnInitializeSpl(TRANI_GCFQUAL_SPLB | TRANI_ITEMIZERS_SPLB | TRANI_CONSOLI_SPLB);
        pSpl = &TrnSplitB;
        sType = TRN_TYPE_SPLITB;
    } else {
        TrnInitializeSpl(TRANI_GCFQUAL_SPLA | TRANI_ITEMIZERS_SPLA | TRANI_CONSOLI_SPLA);
        pSpl = &TrnSplitA;
        sType = TRN_TYPE_SPLITA;
    }

	TrnSplDecideStorage(TRN_TYPE_CONSSTORAGE, &ulReadPos, &ulCurSize, &hsHandle);
    while (ulReadPos < ulCurSize) {
        /*----- Read 1 Item -----*/
		//11-1103- SR 201 JHHJ
        TrnReadFile(ulReadPos, auchWork, sizeof(auchWork), hsHandle, &ulActualBytesRead);

		if (pauchWork->uchMajorClass == CLASS_ITEMTRANSOPEN) {
            /*----- De-Compress Item and keep it to be used if there is right hand seat to split off -----*/
			// we only insert the transaction open if there are items assigned to this seat so we hold it until we know.
            RflGetStorageItem(&TransOpen, pauchWork, RFL_WITH_MNEM);
        } else {
            /*----- Exist Seat# -----*/
			if (pauchWork->uchSeatNo != 0) {
				switch(pauchWork->uchMajorClass) {
                case    CLASS_ITEMSALES:
                case    CLASS_ITEMDISC:
                case    CLASS_ITEMCOUPON:
                    /*----- Send to Split File -----*/
					if (pauchWork->uchSeatNo == uchRightSeat) {
						UCHAR   auchTranStorageWork[sizeof(ITEMSALES)] = {0};

                        if (sType == TRN_TYPE_SPLITA && sFlagA == 0) {
							// if we have not inserted a transaction open into this split file then do so.
							// we need to specify which seat this split file contains as part of open.
                            TransOpen.uchSeatNo = uchRightSeat;
                            TrnOpenSpl(&TransOpen, sType);
                            sFlagA = 1;
                        }
                        if (sType == TRN_TYPE_SPLITB && sFlagB == 0) {
							// if we have not inserted a transaction open into this split file then do so.
							// we need to specify which seat this split file contains as part of open.
                            TransOpen.uchSeatNo = uchRightSeat;
                            TrnOpenSpl(&TransOpen, sType);
                            sFlagB = 1;
                        }

                        /*----- De-Compress Item and then store it in the proper split file -----*/
                        RflGetStorageItem(auchTranStorageWork, pauchWork, RFL_WITH_MNEM);
                        TrnItemSpl(auchTranStorageWork, sType);
                    }
                    break;

                default:
                    break;
                }
            }
        }

        /*----- Increment Read Pointer -----*/
        ulReadPos += RflGetStorageTranRecordLen(auchWork);
    }

    return(TRN_SUCCESS);
}

/*
*==========================================================================
**  Synopsis:   SHORT   TrnSplRightArrowSeat(UCHAR *uchMiddle, UCHAR *uchRight)
*       Input:  
*      Output:  None
*       InOut:  None
*
*    Return:    
*
**  Description:    Decide Next Seat# for LCD Middle/Right.
*==========================================================================
*/
SHORT   TrnSplRightArrowSeat(UCHAR *uchMiddle, UCHAR *uchRight)
{
    UCHAR   auchSeat[NUM_SEAT];
    SHORT   i, sFlag = 0;

	// make a copy of the in use seat queue, containing the seats in use in the transaction.
	// remove from the copy of the in use seat queue those seats that have already been finalized
	// by repeatedly calling the remove seat from seat queue function specifying each finalized seat.
    memcpy(auchSeat, TrnInformation.TranGCFQual.auchUseSeat, sizeof(auchSeat));
    for (i = 0; i < NUM_SEAT && TrnInformation.TranGCFQual.auchFinSeat[i] != 0; i++) {
        TrnSplDispSeatNo1(TrnInformation.TranGCFQual.auchFinSeat[i], auchSeat);
    }

    /*----- Decide Next Seat# Transaction -----*/
    if (TrnInformation.usGuestNo[0] == 0 || TrnInformation.usGuestNo[1] == 0) {
        return(TRN_ERROR);
    } else {
        /*----- Search Next Seat# Transaction -----*/
        for (i = 0; i < NUM_SEAT; i++) {
            if (TrnInformation.usGuestNo[1] == auchSeat[i]) {
                if (i + 1 < NUM_SEAT && auchSeat[i+1] != 0) {
                    *uchMiddle = auchSeat[i];
                    *uchRight = auchSeat[i+1];
                    sFlag = 1;
                    break;
                }
            }
        }
        if (sFlag == 0) {
            return(TRN_ERROR);
        }
    }

    return(TRN_SUCCESS);
}

/*
*==========================================================================
**  Synopsis:   SHORT   TrnSplLeftArrow(VOID)
*       Input:  
*      Output:  None
*       InOut:  None
*
*    Return:    
*
**  Description:    Create Seat# Transaction for Left Arrow Key.
*==========================================================================
*/
SHORT   TrnSplLeftArrow(VOID)
{
    UCHAR   auchWork[sizeof(ITEMDATASIZEUNION)];
	UCHAR   uchMiddleSeat, uchRightSeat;
    SHORT   sType, sFlagA = 0, sFlagB = 0;
    TRANINFSPLIT    *pSpl;
	ITEMTRANSOPEN   TransOpen = {0};
	SHORT           hsHandle;
    ULONG           ulCurSize, ulReadPos;
	ULONG           ulActualBytesRead;
	TRANSTORAGERECORD  *pauchWork = (TRANSTORAGERECORD  *)auchWork;

    /*----- Transaction Open Data for Split B -----*/
    /*----- No Seat# Transaction -----*/
    if (!(TrnInformation.TranGCFQual.fsGCFStatus2 & GCFQUAL_USESEAT)) {
        return(TRN_ERROR);
    }

    /*----- During Seat# Tender -----*/
    if (TrnInformation.fsTransStatus & TRN_STATUS_SEAT_TRANS || TrnInformation.fsTransStatus & TRN_STATUS_MULTI_TRANS) {
        return(TRN_ERROR);
    }

    /*----- Decide Next Seat# Transaction -----*/
    if (TrnSplLeftArrowSeat(&uchMiddleSeat, &uchRightSeat) != TRN_SUCCESS) {
        return(TRN_ERROR);
    }

    /*----- Decide Next File Save Area -----*/
    TrnInformation.usGuestNo[0] = uchMiddleSeat;
    TrnInformation.usGuestNo[1] = uchRightSeat;
    if (uchRightSeat == (UCHAR)TrnSplitA.TranGCFQual.usGuestNo) {
        TrnInitializeSpl(TRANI_GCFQUAL_SPLB | TRANI_ITEMIZERS_SPLB | TRANI_CONSOLI_SPLB);
        pSpl = &TrnSplitB;
        sType = TRN_TYPE_SPLITB;
    } else {
        TrnInitializeSpl(TRANI_GCFQUAL_SPLA | TRANI_ITEMIZERS_SPLA | TRANI_CONSOLI_SPLA);
        pSpl = &TrnSplitA;
        sType = TRN_TYPE_SPLITA;
    }

	TrnSplDecideStorage(TRN_TYPE_CONSSTORAGE, &ulReadPos, &ulCurSize, &hsHandle);

    while (ulReadPos < ulCurSize) {
        /*----- Read 1 Item -----*/
		//11-1103- SR 201 JHHJ
        TrnReadFile(ulReadPos, auchWork, sizeof(auchWork), hsHandle, &ulActualBytesRead);

		if (pauchWork->uchMajorClass == CLASS_ITEMTRANSOPEN) {
            /*----- De-Compress Item and keep it to be used if there is middle seat to split off -----*/
			// we only insert the transaction open if there are items assigned to this seat so we hold it until we know.
			RflGetStorageItem(&TransOpen, auchWork, RFL_WITH_MNEM);
        } else {
            /*----- Exist Seat# -----*/
			if (pauchWork->uchSeatNo != 0) {
				switch(pauchWork->uchMajorClass) {
                case    CLASS_ITEMSALES:
                case    CLASS_ITEMDISC:
                case    CLASS_ITEMCOUPON:
                    /*----- Send to Split File -----*/
					if (pauchWork->uchSeatNo == uchMiddleSeat) {
						UCHAR   auchTranStorageWork[sizeof(ITEMSALES)] = {0};

                        if (sType == TRN_TYPE_SPLITA && sFlagA == 0) {
							// if we have not inserted a transaction open into this split file then do so.
							// we need to specify which seat this split file contains as part of open.
                           TransOpen.uchSeatNo = uchMiddleSeat;
                            TrnOpenSpl(&TransOpen, sType);
                            sFlagA = 1;
                        }
                        if (sType == TRN_TYPE_SPLITB && sFlagB == 0) {
							// if we have not inserted a transaction open into this split file then do so.
							// we need to specify which seat this split file contains as part of open.
                            TransOpen.uchSeatNo = uchMiddleSeat;
                            TrnOpenSpl(&TransOpen, sType);
                            sFlagB = 1;
                        }
                        /*----- De-Compress Item and store it into the proper split file -----*/
                        RflGetStorageItem(auchTranStorageWork, auchWork, RFL_WITH_MNEM);
                        TrnItemSpl(auchTranStorageWork, sType);
                    }
                    break;

                default:
                    break;
                }
            }
        }

        /*----- Increment Read Pointer -----*/
        ulReadPos += RflGetStorageTranRecordLen(auchWork);
    }

    return(TRN_SUCCESS);
}

/*
*==========================================================================
**  Synopsis:   SHORT   TrnSplLeftArrowSeat(UCHAR *uchMiddle, UCHAR *uchRight)
*       Input:  
*      Output:  None
*       InOut:  None
*
*    Return:    
*
**  Description:    Decide Next Seat# for LCD Middle/Right.
*==========================================================================
*/
SHORT   TrnSplLeftArrowSeat(UCHAR *uchMiddle, UCHAR *uchRight)
{
    UCHAR   auchSeat[NUM_SEAT];
    SHORT   i, sFlag = 0;

	// make a copy of the in use seat queue, containing the seats in use in the transaction.
	// remove from the copy of the in use seat queue those seats that have already been finalized
	// by repeatedly calling the remove seat from seat queue function specifying each finalized seat.
    memcpy(auchSeat, TrnInformation.TranGCFQual.auchUseSeat, sizeof(auchSeat));
    for (i = 0; i < NUM_SEAT && TrnInformation.TranGCFQual.auchFinSeat[i] != 0; i++) {
        TrnSplDispSeatNo1(TrnInformation.TranGCFQual.auchFinSeat[i], auchSeat);
    }

    /*----- Decide Next Seat# Transaction -----*/
    /*----- No Seat# Transaction Anymore -----*/
    if (TrnInformation.usGuestNo[0] == 0 || TrnInformation.usGuestNo[1] == 0) {
        return(TRN_ERROR);

    } else {
        for (i = 0; i < NUM_SEAT; i++) {
            if (TrnInformation.usGuestNo[0] == auchSeat[i]) {
                if (i > 0) {
                    *uchMiddle = auchSeat[i-1];
                    *uchRight = auchSeat[i];
                    sFlag = 1;
                    break;
                }
            }
        }
        if (sFlag == 0) {
            return(TRN_ERROR);
        }
    }

    return(TRN_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID    TrnSplAddSeatTrans(UCHAR uchSeat)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description:     Add Split File and Split File in Split A.
*===========================================================================
*/
VOID    TrnSplAddSeatTrans(UCHAR uchSeat)
{
    UCHAR   auchWork[sizeof(ITEMDATASIZEUNION)];
	SHORT   hsHandle;
    ULONG   ulCurSize, ulReadPos;
	TRANSTORAGERECORD  *pauchWork = (TRANSTORAGERECORD  *)auchWork;

    /*----- Save Current Seat# -----*/
    TrnInformation.TranCurQual.uchSeat = uchSeat;

	TrnSplDecideStorage(TRN_TYPE_CONSSTORAGE, &ulReadPos, &ulCurSize, &hsHandle);

    while (ulReadPos < ulCurSize) {
		ULONG	ulActualBytesRead;

        /*----- Read 1 Item -----*/
		//11-1103- SR 201 JHHJ
        TrnReadFile(ulReadPos, auchWork, sizeof(auchWork), hsHandle, &ulActualBytesRead);

        /*----- Target Seat# -----*/
		if (pauchWork->uchSeatNo == uchSeat) {
			UCHAR   auchTranStorageWork[sizeof(ITEMSALES)] = {0};

			switch(pauchWork->uchMajorClass) {
            case    CLASS_ITEMSALES:
            case    CLASS_ITEMDISC:
            case    CLASS_ITEMCOUPON:
                /*----- De-Compress Item -----*/
                RflGetStorageItem(auchTranStorageWork, auchWork, RFL_WITH_MNEM);
                TrnItemSpl(auchTranStorageWork, TRN_TYPE_SPLITA);

                /*----- Reduce Item from Consoli Storage/Itemizer -----*/
                TrnSplReduce(auchTranStorageWork);
                break;

            default:
                break;
            }
        }

        /*----- Increment Read Pointer -----*/
        ulReadPos += RflGetStorageTranRecordLen(auchWork);
    }

    /*----- Insert Finalized Seat# into Queue -----*/
    TrnSplInsertFinSeatQueue(uchSeat);
}

/*
*===========================================================================
** Synopsis:    SHORT   TrnSplGetBaseTrans(SHORT sUpdate, UCHAR uchStoType)
*
*     Input:    SHORT   sUpdate(0:Not Update, 1:Update)
*                       uchStoType(10:Consoli. 11:Post Rec.)
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description:     Create Base Transaction File in Split A Area.
*===========================================================================
*/
SHORT   TrnSplGetBaseTrans(SHORT sUpdate, UCHAR uchStoType)
{
    UCHAR        auchWork[sizeof(ITEMDATASIZEUNION)];
    SHORT        sFlag = 0;
	SHORT        hsHandle;
    ULONG        ulCurSize, ulReadPos;
	ULONG        ulActualBytesRead;
	TRANSTORAGERECORD  *pauchWork = (TRANSTORAGERECORD  *)auchWork;

    /*----- Initialize Split File A -----*/
    TrnInitializeSpl(TRANI_GCFQUAL_SPLA | TRANI_ITEMIZERS_SPLA | TRANI_CONSOLI_SPLA);

	if (uchStoType == TRN_CONSOLI_STO) {        /* 6/5/96 */
        /*----- Search First Seat# Item in Consoli Storage -----*/
		TrnSplDecideStorage(TRN_TYPE_CONSSTORAGE, &ulReadPos, &ulCurSize, &hsHandle);
    } else {
		TrnSplDecideStorage(TRN_TYPE_POSTRECSTORAGE, &ulReadPos, &ulCurSize, &hsHandle);
    }

    while (ulReadPos < ulCurSize) {
        /*----- Read 1 Item -----*/
        TrnReadFile(ulReadPos, auchWork, sizeof(auchWork), hsHandle, &ulActualBytesRead);
		sFlag |= TrnSplMoveReduceRecord (0, pauchWork, TRN_TYPE_SPLITA, sUpdate);
        /*----- Increment Read Pointer -----*/
        ulReadPos += RflGetStorageTranRecordLen(auchWork);
    }

    if (sFlag == 0) {
        return(TRN_ERROR);
    } else {
        return(TRN_SUCCESS);
    }
}

/*
*===========================================================================
** Synopsis:    SHORT   TrnSplGetBaseTransSeat(UCHAR uchStoType)
*
*     Input:    UCHAR   uchStoType(10:Consoli. 11:Post Rec.)
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description:     Create Base Transaction File in Split A Area.
*===========================================================================
*/
SHORT   TrnSplGetBaseTransSeat(UCHAR uchStoType)
{
    UCHAR         auchWork[sizeof(ITEMDATASIZEUNION)];
	SHORT         hsHandle;
    ULONG         ulCurSize, ulReadPos;
	TRANSTORAGERECORD  *pauchWork = (TRANSTORAGERECORD  *)auchWork;

    /*----- Initialize Split File A -----*/
    TrnInitializeSpl(TRANI_GCFQUAL_SPLA | TRANI_ITEMIZERS_SPLA | TRANI_CONSOLI_SPLA);

	if (uchStoType == TRN_CONSOLI_STO) {        /* 6/5/96 */
        /*----- Search First Seat# Item in Consoli Storage -----*/
		TrnSplDecideStorage(TRN_TYPE_CONSSTORAGE, &ulReadPos, &ulCurSize, &hsHandle);
    } else {
		TrnSplDecideStorage(TRN_TYPE_POSTRECSTORAGE, &ulReadPos, &ulCurSize, &hsHandle);
    }

    while (ulReadPos < ulCurSize) {
		ULONG         ulActualBytesRead;

        /*----- Read 1 Item -----*/
        TrnReadFile(ulReadPos, auchWork, sizeof(auchWork), hsHandle, &ulActualBytesRead);
		TrnSplMoveReduceRecord (0xff, pauchWork, TRN_TYPE_SPLITA, TRN_SPL_NOT_UPDATE);  // all records, ignore seat number
        /*----- Increment Read Pointer -----*/
        ulReadPos += RflGetStorageTranRecordLen(pauchWork);
    }

    return(TRN_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID    TrnSplDispSeatNo1(UCHAR uchSeat, UCHAR *auchUseQueue)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description:     Remove the given Seat# from a Seat Queue.
*===========================================================================
*/
VOID    TrnSplDispSeatNo1(UCHAR uchSeat, UCHAR *auchUseQueue)
{
	UCHAR   auchQueue[NUM_SEAT] = {0}; //SR206
    SHORT   i, j = 0;

	// copy the value of those seat queue elements that are not equal to the given seat number
    for (i = 0; i < NUM_SEAT; i++) { //SR206
        if (auchUseQueue[i] != uchSeat) {
            auchQueue[j] = auchUseQueue[i];
            j++;
        }
    }

	// update the seat queue argument with our new seat queue which does not have the given seat.
    memcpy(auchUseQueue, auchQueue, sizeof(auchQueue));
}

/*
*===========================================================================
** Synopsis:    VOID    TrnSplInsertUseSeatQueue(UCHAR uchSeat)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description:     Insert Seat# to Seat# Queue.
*===========================================================================
*/
VOID    TrnSplInsertUseSeatQueue(UCHAR uchSeat)
{
    SHORT   i;

    for (i = 0; i < NUM_SEAT; i++) { //SR206
        if (TrnInformation.TranGCFQual.auchUseSeat[i] == 0 || TrnInformation.TranGCFQual.auchUseSeat[i] == uchSeat) {
            TrnInformation.TranGCFQual.auchUseSeat[i] = uchSeat;
            break;
        }
    }
}


/*
*===========================================================================
** Synopsis:    VOID    TrnSplInsertFinSeatQueue(UCHAR uchSeat)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description:     Insert Seat# to Seat# Queue in TrnInf & Split A.
*===========================================================================
*/
VOID    TrnSplInsertFinSeatQueue(UCHAR uchSeat)
{
    SHORT   i;

    for (i = 0; i < NUM_SEAT; i++) {
        if (TrnInformation.TranGCFQual.auchFinSeat[i] == 0 || TrnInformation.TranGCFQual.auchFinSeat[i] == uchSeat) {
            TrnInformation.TranGCFQual.auchFinSeat[i] = uchSeat;
            break;
        }
    }

    for (i = 0; i < NUM_SEAT; i++) {
        if (TrnSplitA.TranGCFQual.auchFinSeat[i] == 0 || TrnSplitA.TranGCFQual.auchFinSeat[i] == uchSeat) {
            TrnSplitA.TranGCFQual.auchFinSeat[i] = uchSeat;
            break;
        }
    }
}


/*
*===========================================================================
** Synopsis:    SHORT   TrnSplSearchUseSeatQueue(UCHAR uchSeat)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description:     Search Target Seat# from Use Seat Queue.
*===========================================================================
*/
SHORT   TrnSplSearchUseSeatQueue(UCHAR uchSeat)
{
    SHORT   i;

    if (uchSeat == 0) {
        return(TRN_ERROR);
    }

    for (i = 0; i < NUM_SEAT; i++) {
        if (TrnInformation.TranGCFQual.auchUseSeat[i] == uchSeat) {
            return(TRN_SUCCESS);
        }
    }

    return(TRN_ERROR);
}

/*
*===========================================================================
** Synopsis:    SHORT   TrnSplSearchFinSeatQueue(UCHAR uchSeat)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description:     Search Target Seat# from Finalize Seat Queue.
*===========================================================================
*/
SHORT   TrnSplSearchFinSeatQueue(UCHAR uchSeat)
{
    SHORT   i;

    if (uchSeat == 0) {
        return(TRN_ERROR);
    }

    for (i = 0; i < NUM_SEAT; i++) {
        if (TrnInformation.TranGCFQual.auchFinSeat[i] == uchSeat) {
            return(TRN_SUCCESS);
        }
    }

    return(TRN_ERROR);
}

/*
*===========================================================================
** Synopsis:    VOID    TrnSplPrintSeat(UCHAR uchSeat, SHORT sUpdate,
*                           USHORT usTrail, USHORT usAllPrint, UCHAR auchTotalStatus[],
*                           UCHAR uchPrintType)
*
*     Input:    SHORT   sUpdate(0:Not Update, 1:Update)
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description:     Send Seat# Transaction to Print Module.
*===========================================================================
*/
VOID    TrnSplPrintSeat(UCHAR uchSeat, SHORT sUpdate, USHORT usTrail, USHORT usAllPrint,
                        UCHAR auchTotalStatus[], UCHAR uchPrintType)
{
    /*----- check slip compulsory 4/19/96 -----*/
    if (auchTotalStatus[3] & CURQUAL_TOTAL_COMPULSRY_SLIP) {            /* compulsory slip, ITM_TTL_FLAG_COMPULSORY_SLIP() */
        TrnInformation.TranCurQual.fbCompPrint = PRT_SLIP;
    }

    /*----- Print Individual Split File 6/9/96 -----*/
    if (uchPrintType == TRN_PRT_FULL) {
        TrnInformation.TranCurQual.uchPrintStorage = PRT_CONSOLSTORAGE;
    } else {
        TrnInformation.TranCurQual.uchPrintStorage = PRT_CONSOL_CP_EPT;
    }
    PrtPrintSplit(&TrnInformation, &TrnSplitA, usTrail, usAllPrint);

    /*----- Recover Each Storage Status 6/9/96 ------*/
    if (uchPrintType == TRN_PRT_TTL) {
        TrnInformation.TranCurQual.uchPrintStorage = PRT_CONSOLSTORAGE;     
    }

    /*----- Finalized Seat# Transaction -----*/
    if (sUpdate == TRN_SPL_UPDATE) {
        TrnSplInsertFinSeatQueue(uchSeat);
    }
}

/*
*===========================================================================
** Synopsis:    VOID    TrnSplPrintBase(USHORT usTrail, USHORT usAllPrint,
*                                       UCHAR auchTotalStatus[])
*
*     Input:    
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description:     Send Base Transaction to Print Module.
*===========================================================================
*/
VOID    TrnSplPrintBase(USHORT usTrail, USHORT usAllPrint, UCHAR auchTotalStatus[])
{
    /*----- check slip compulsory 4/19/96 -----*/
    if (auchTotalStatus[3] & CURQUAL_TOTAL_COMPULSRY_SLIP) {            /* compulsory slip, ITM_TTL_FLAG_COMPULSORY_SLIP() */
        TrnInformation.TranCurQual.fbCompPrint = PRT_SLIP;
    }
    TrnInformation.TranCurQual.uchPrintStorage = PRT_CONSOLSTORAGE;
    PrtPrintSplit(&TrnInformation, &TrnSplitA, usTrail, usAllPrint);

    TrnInformation.TranCurQual.uchSeat = 0;
    TrnInformation.TranCurQual.auchTotalStatus[0] = 0;    // clear the total key type
    TrnInformation.TranCurQual.uchSplit = 0;
}

/*
*===========================================================================
** Synopsis:    VOID    TrnSplCancel3Disp(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description:     Return to LCD 1 Display.
*===========================================================================
*/
VOID    TrnSplCancel3Disp(VOID)
{
    TrnInformation.TranCurQual.uchSeat = 0;
    TrnInformation.TranCurQual.auchTotalStatus[0] = 0;    // clear the total key type
    TrnInformation.TranCurQual.uchSplit = 0;
    memset(&TrnInformation.usGuestNo[0], 0, sizeof(TrnInformation.usGuestNo));
}

/*
*===========================================================================
** Synopsis:    VOID    TrnSplCancel3DispSplit(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description:     Return to LCD 1 Display for Split Tender.
*===========================================================================
*/
VOID    TrnSplCancel3DispSplit(VOID)
{
    TrnInformation.TranCurQual.uchSeat = 0;
    TrnInformation.TranCurQual.uchSplit = 0;
    memset(&TrnInformation.usGuestNo[0], 0, sizeof(TrnInformation.usGuestNo));
}



/*
*===========================================================================
** Synopsis:    SHORT   TrnSplAddGC2GC(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description:     Add Destination Guest Check and Target Guest Check,     V3.3
*
*                   WARNING: Unusual use of Post Receipt File as temporary file.
*
*                   We are expect that the Post Receipt file, TrnInformation.hsTranPostRecStorage,
*                   is being used as a temporary file to hold one of the guest checks.
*                   The idea is to take the Guest Check data in the Post Receipt file
*                   and insert it into the existing Guest Check in the Consolidated Storage file.
*===========================================================================
*/
SHORT   TrnSplAddGC2GC(VOID)
{
    UCHAR         auchWork[sizeof(ITEMDATASIZEUNION)];
    TrnVliOffset  usVli = 0;       /* item storage Vli */
    ULONG         ulCurSize, ulReadPos;
	ULONG         ulActualBytesRead;
	TRANSTORAGERECORD  *pauchWork = (TRANSTORAGERECORD  *)auchWork;

	// The Post Receipt transaction data file is being used as a temporary file containing one of the
	// Guest Checks that are being merged. The format for the transaction is the standard Consolidated Storage
	// with the standard file header TRANCONSSTORAGEHEADER and not the standard Post Receipt file header
	// of TRANPOSTRECSTORAGEHEADER. See function ItemMiscCheckTransGC2GC().
    ulReadPos = sizeof(TRANCONSSTORAGEHEADER_VLI);
    TrnReadFile(ulReadPos, &usVli, sizeof(usVli), TrnInformation.hsTranPostRecStorage, &ulActualBytesRead);
    ulReadPos = sizeof(TRANCONSSTORAGEHEADER);
    ulCurSize = ulReadPos + usVli;

    while (ulReadPos < ulCurSize) {
        /*----- Read 1 Item -----*/
        TrnReadFile(ulReadPos, auchWork, sizeof(auchWork), TrnInformation.hsTranPostRecStorage, &ulActualBytesRead);

		switch(pauchWork->uchMajorClass) {
        case    CLASS_ITEMSALES:
        case    CLASS_ITEMDISC:
        case    CLASS_ITEMCOUPON:
			{
				UCHAR         auchTranStorageWork[sizeof(ITEMSALES)] = {0};

				/*----- De-Compress Item and store it into the split file -----*/
				RflGetStorageItem(auchTranStorageWork, auchWork, RFL_WITHOUT_MNEM);	/* do not comm. */
				/* RflGetStorageItem(auchTranStorageWork, auchWork, RFL_WITH_MNEM); */
				if (TrnStorage(auchTranStorageWork) != TRN_SUCCESS) {
					return(TRN_ERROR);
				}
			}
            break;

        default:
            break;
        }

        /*----- Increment Read Pointer -----*/
        ulReadPos += RflGetStorageTranRecordLen(auchWork);
    }

    return(TRN_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID    TrnSplAddGC2GCGCFQual(UCHAR *puchBuffer)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
** Return:      nothing
*
** Description:     Add Destination GC Qualifier to Target one,     V3.3
*===========================================================================
*/
VOID    TrnSplAddGC2GCGCFQual(TRANGCFQUAL *pGCF2)
{
    SHORT           i;
    TRANGCFQUAL     *pGCF1 = &TrnInformation.TranGCFQual;
	UCHAR			uchSeat;

    pGCF1->ulCashierID      = pGCF2->ulCashierID;
    pGCF1->ulPrevCashier    = pGCF2->ulPrevCashier;
    pGCF1->usGuestNo        = pGCF2->usGuestNo;
    pGCF1->uchCdv           = pGCF2->uchCdv;
    memcpy(pGCF1->auchCheckOpenTime, pGCF2->auchCheckOpenTime, sizeof(TrnInformation.TranGCFQual.auchCheckOpenTime));
    memcpy(pGCF1->auchCheckOpen, pGCF2->auchCheckOpen, sizeof(TrnInformation.TranGCFQual.auchCheckOpen));
    memcpy (pGCF1->auchCheckHashKey, pGCF2->auchCheckHashKey, 6);
    pGCF1->uchTeamNo        = pGCF2->uchTeamNo;
    pGCF1->usNoPerson       = pGCF2->usNoPerson;
    pGCF1->usNoPersonMax    = pGCF2->usNoPersonMax;
    pGCF1->usTableNo        = pGCF2->usTableNo;
    pGCF1->lCurBalance      = pGCF2->lCurBalance;
    pGCF1->lTranBalance     = pGCF2->lTranBalance;
    pGCF1->fsGCFStatus      = pGCF2->fsGCFStatus;
    pGCF1->fsGCFStatus2     = pGCF2->fsGCFStatus2;
    pGCF1->uchSlipLine     += pGCF2->uchSlipLine;
    pGCF1->uchPageNo       += pGCF2->uchPageNo;
    pGCF1->sPigRuleCo      += pGCF2->sPigRuleCo;
    /* saratoga */
	uchSeat = TrnInformation.TranCurQual.uchSeat;
    memcpy(pGCF1->aszNumber[uchSeat], pGCF2->aszNumber[uchSeat], sizeof(TrnInformation.TranGCFQual.aszNumber[uchSeat]));
    memcpy(pGCF1->auchExpiraDate[uchSeat], pGCF2->auchExpiraDate[uchSeat], sizeof(TrnInformation.TranGCFQual.auchExpiraDate[uchSeat]));
    memcpy(pGCF1->auchMSRData[uchSeat], pGCF2->auchMSRData[uchSeat], sizeof(TrnInformation.TranGCFQual.auchMSRData[uchSeat]));

    memcpy(pGCF1->auchCouponType, pGCF2->auchCouponType, sizeof(TrnInformation.TranGCFQual.auchCouponType));
    _tcsncpy(pGCF1->aszName, pGCF2->aszName, TCHARSIZEOF(TrnInformation.TranGCFQual.aszName));

    for (i = 0; i < NUM_SEAT; i++) {
        TrnSplInsertUseSeatQueue(pGCF2->auchUseSeat[i]);
        TrnSplInsertFinSeatQueue(pGCF2->auchFinSeat[i]);
        pGCF1->auchSeatDisc[i] |= pGCF2->auchSeatDisc[i];
    }
}

/*
*===========================================================================
** Synopsis:    VOID    TrnSplAddGC2GCItemizers(TRANITEMIZERS *pItem)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
** Return:      nothing
*
** Description:     Add Destination GC Itemizers to Target one,     V3.3
*===========================================================================
*/
VOID    TrnSplAddGC2GCItemizers(TRANITEMIZERS *pItem2)
{
    SHORT               i, j;
    TRANITEMIZERS       *pItem1  = &TrnInformation.TranItemizers;
    TRANUSITEMIZERS     *pUs1 = &TrnInformation.TranItemizers.Itemizers.TranUsItemizers;
    TRANUSITEMIZERS     *pUs2 = &pItem2->Itemizers.TranUsItemizers;
    TRANCANADAITEMIZERS *pCan1 = &TrnInformation.TranItemizers.Itemizers.TranCanadaItemizers;
    TRANCANADAITEMIZERS *pCan2 = &pItem2->Itemizers.TranCanadaItemizers;
    TRANINTLITEMIZERS   *pInt1 = &TrnInformation.TranItemizers.Itemizers.TranIntlItemizers;
    TRANINTLITEMIZERS   *pInt2 = &pItem2->Itemizers.TranIntlItemizers;

    /* --- Addition --- */
    pItem1->lMI                 += pItem2->lMI;
    pItem1->lDiscountable[0]    += pItem2->lDiscountable[0];
    pItem1->lDiscountable[1]    += pItem2->lDiscountable[1];
    pItem1->lHourly             += pItem2->lHourly;
    pItem1->sItemCounter        += pItem2->sItemCounter;
    for (i = 0; i < 3; i++) {
        pItem1->lService[i]     += pItem2->lService[i];
    }
    for (i = 0; i < STD_NO_BONUS_TTL; i++) {
        pItem1->lHourlyBonus[i]       += pItem2->lHourlyBonus[i];
    }
    for (i = 0; i < STD_NO_BONUS_TTL; i++) {
        pItem1->lTransBonus[i]       += pItem2->lTransBonus[i];
    }

	/* TAR191625 */
    for (i = 0; i < 6; i++) {
		pItem1->lVoidDiscount[i] += pItem2->lVoidDiscount[i];
    }

    if (TrnTaxSystem() == TRN_TAX_INTL) {
        for (i = 0; i < 3; i++) {
            pInt1->lDiscountable_Ser[i] += pInt2->lDiscountable_Ser[i];
            pInt1->lVATSer[i]           += pInt2->lVATSer[i];
            pInt1->lVATNon[i]           += pInt2->lVATNon[i];
        }
        for (i = 0; i < 3; i++) {
            for (j = 0; j < 3; j++) {
                pInt1->lDisSer[i][j] += pInt2->lDisSer[i][j];
                pInt1->lDisNon[i][j] += pInt2->lDisNon[i][j];
            }
        }
        pInt1->lServiceable += pInt2->lServiceable;

		/* TAR191625 */
		for (i = 0; i < 6; i++) {
	        for (j = 0; j < 3; j++) {
				pInt1->lVoidVATSer[i][j] += pInt2->lVoidVATSer[i][j];
			   	pInt1->lVoidVATNon[i][j] += pInt2->lVoidVATNon[i][j];
			}
		    pInt1->lVoidServiceable[i] += pInt2->lVoidServiceable[i];
		}
    } else if (TrnTaxSystem() == TRN_TAX_CANADA) {
        for (i = 0; i < STD_PLU_ITEMIZERS; i++) {
            pCan1->lTaxable[i] += pCan2->lTaxable[i];
        }
        for (i = 0; i < STD_PLU_ITEMIZERS+2+3; i++) {    // should this be STD_PLU_ITEMIZERS+2+3 rather than 9+2+3?
            pCan1->lDiscTax[i] += pCan2->lDiscTax[i];
        }
        for (i = 0; i < 5; i++) {
            pCan1->lAffTaxable[i] += pCan2->lAffTaxable[i];
            pCan1->lPrtTaxable[i] += pCan2->lPrtTaxable[i];
        }
        for (i = 0; i < 4; i++) {
            pCan1->lAffTax[i] += pCan2->lAffTax[i];
            pCan1->lPrtTax[i] += pCan2->lPrtTax[i];
        }
        for (i = 0; i < STD_DISC_ITEMIZERS_MAX; i++) {
            for (j = 0; j < STD_PLU_ITEMIZERS+2+3; j++) {    // should this be STD_PLU_ITEMIZERS+2+3 rather than 9+2+3?
                pCan1->lDiscTaxable[i][j] += pCan2->lDiscTaxable[i][j];
            }
        }
        pCan1->lMealLimitTaxable    += pCan2->lMealLimitTaxable;
        pCan1->lMealNonLimitTaxable += pCan2->lMealNonLimitTaxable;

		/* TAR191625 */
		for (i = 0; i < STD_DISC_ITEMIZERS_MAX; i++) {
			for (j = 0; j < STD_PLU_ITEMIZERS+2+3; j++) {    // should this be STD_PLU_ITEMIZERS+2+3 rather than 9+2+3?
   				pCan1->lVoidDiscTax[i][j] += pCan2->lVoidDiscTax[i][j];
			}
        }
    } else {
        for (i = 0; i < STD_TAX_ITEMIZERS_MAX; i++) {
            pUs1->lAffectTaxable[i]     += pUs2->lAffectTaxable[i];
            pUs1->lNonAffectTaxable[i]  += pUs2->lNonAffectTaxable[i];
            pUs1->lTaxItemizer[i]       += pUs2->lTaxItemizer[i];
            pUs1->lPrintTaxable[i]      += pUs2->lPrintTaxable[i];
            pUs1->lPrintTax[i]          += pUs2->lPrintTax[i];
            pUs1->lAffectTaxExempt[i]   += pUs2->lAffectTaxExempt[i];
            pUs1->lPrintTaxExempt[i]    += pUs2->lPrintTaxExempt[i];
        }
        for (i = 0; i < STD_TAX_ITEMIZERS_MAX * 2; i++) {
            pUs1->lDiscTaxable[i] += pUs2->lDiscTaxable[i];
        }
		/* TAR191625 */
        for (i = 0; i < STD_DISC_ITEMIZERS_MAX; i++) {
			for (j = 0; j < STD_TAX_ITEMIZERS_MAX; j++) {
   				pUs1->lVoidDiscNonAffectTaxable[i][j] += pUs2->lVoidDiscNonAffectTaxable[i][j];
   				pUs1->lVoidDiscFSDiscTaxable[i][j]    += pUs2->lVoidDiscFSDiscTaxable[i][j];
			}
        }
    }
}


/* ===== End of File ( TRNSPLT1.C ) ===== */
