/*-----------------------------------------------------------------------*\
:   Title          : TRANSACTION STORAGE MODULE
:   Category       : TRANSACTION MODULE, NCR 2170 US Hospitality Application
:   Program Name   : TRNSTOR.C
:  ---------------------------------------------------------------------
:  Abstract:
:
:   TrnStorage()            :
:   TrnStoOpen()            :
:   TrnStoOpenComp()        :
:   TrnStoSales()           :
:   TrnStoSalesComp()       :
:   TrnStoDisc()            :
:   TrnStoDiscComp()        :
:#  TrnStoCoupon()          : add ( REL 3.0 )
:#  TrnStoCouponComp()      : add ( REL 3.0 )
:   TrnStoTotal()           :
:   TrnStoTotalComp()       :
:   TrnStoTender()          :
:   TrnStoTenderComp()      :
:   TrnStoMisc()            :
:   TrnStoMiscComp()        :
:   TrnStoAffect()          :
:   TrnStoAffectComp()      :
:   TrnStoPrint()           :
:   TrnStoPrintComp()       :
:   TrnStoMulti()           :
:   TrnStoMultiComp()       :
:   TrnStoOpeClose()        :
:   TrnStoOpeCloseComp()    :
:
:#  TrnStoItemNormal()      : modify ( REL 3.0 )
:#  TrnStoConsNormal()      : modify ( REL 3.0 )
:#  TrnStoNoItemNormal()    : add ( REL 3.0 )
:#  TrnStoNoConsNormal()    : add ( REL 3.0 )
:#  TrnStoAppendIndex()     : add ( REL 3.0 )
:#  TrnStoInsertIndex()     : add ( REL 3.0 )
:#  TrnStoSetupPrtIdx()     : add ( REL 3.0 )
:#  TrnStoCompareIndex()    : add ( REL 3.0 )
:   TrnGetTranInformation() :
:   TrnPutTranIntormation() :
:#  TrnCheckSize()          : modify ( REL 3.0 )
:#  TrnPaidOrderCheckSize() : add ( REL 3.0 )
:   TrnGetStorageSize()     :
:#  TrnStoGetConsToPostR()  : modify ( REL 3.0 )
:#  TrnStoPutPostRToCons()  : modify ( REL 3.0 )
:#  TrnSetConsNoToPostR()   : add ( REL 3.0 )
:#  TrnChkStorageSize()     : modify ( REL 3.0 )
:#  TrnStoSalesSearch()     : modify ( REL 3.0 )
:#  TrnStoSalesUpdate()     : add ( REL 3.0 )
:#  TrnStoIsPairItem()      : add ( REL 3.0 )
:#  TrnStoIsItemInWorkBuff(): add ( REL 3.0 )
:#  TrnStoGetStorageInfo()  : add ( REL 3.0 )
:#  TrnStoUpdateCouponType(): add ( REL 3.0 )
:
:  ---------------------------------------------------------------------
:  Update Histories
:  Date     : Ver.Rev. :   Name     : Description
: Jun-01-92 : 00.00.01 : H.Kato     :
: Apr-26-94 : 00.00.04 : M.Suzuki   : add TrnPutTranInformation()
: Jun-07-95 : 03.00.00 : T.Nakahata : Add Print Priority Feature
: Jun-26-95 : 03.00.00 : T.Nakahata : Add Combination Coupon Feature
: Jun-28-95 : 03.00.02 : T.Nakahata : Void Coupon on Cons Storage at D-Thru
: Jun-29-95 : 03.00.02 : T.Nakahata : delete abs() function at SalesUpdate
: Jul-28-95 : 03.00.02 : T.Nakahata : add TrnPaidOrderCheckSize()
: Aug-18-95 : 03.00.04 : T.Nakahata : no consolidate by total key option
: Nov-28-95 : 03.01.00 : hkato      : R3.1
: Aug-16-99 : 03.05.00 : K.Iwata    : R3.5 merge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
: Aug-25-99 : 01.00.00 : K.Iwata    : R1.0 for Win32
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
* Dec-02-99 : 01.00.00 : hrkato     : Saratoga
* Nov-21-15 : 02.02.01 : R.Chambers : using new typedefs for variables and source cleanup.
* Feb-15-15 : 02.02.01 : R.Chambers : added TRN_TYPE_POSTRECSTORAGE case to TrnStoGetStorageInfo()
* Sep-22-17 : 02.02.02 : R.Chambers : modify TrnCheckSize() to handle uchMajorClass of 0 and report size 0.
-------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*\
:   PVCS ENTRY
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

/* ---------------     An explanation about transaction file         -------

	The NeighborhoodPOS product has several different kinds of transaction files
	that it uses to contain the transaction data.  The different kinds of transation
	files reflect the different types of systems that are used by NeighborhoodPOS
	users.  The different types of systems, in turn, reflect the needs of the users.

	NeighborhoodPOS has the following types of systems that are specified in the P2
	provisioning information (Flexible Memory Assignment).  The types are divided
	by when a guest check is printed, whether cursor void is allowed, and whether
	a check can be serviced (stored or suspended) and recalled (resumed).  The types
	determine what information is stored in the transaction file because some types
	store more transaction specific information than other types.

	The options for system type are:
	  Pre-GuestCheck Buffering (Pre GCB) - cursor void is allowed
	  Pre-GuestCheck Unbuffering (Pre GCU) - cafeterias, retail, cursor void is not allowed
	  Post-GuestCheck Buffering (Post GCB) - table service, split checks, cursor void allowed
	  Store/Recall Buffering (SRB) - quick service, flexible drive thru, cursor void allowed

	The Buffering means that the transaction data is not printed until an action such
	as a tender is done that causes the receipt to print.  Unbuffered means that as
	items are entered, the previously entered item is printed.  With Unbuffered, you can
	only modify the last item entered.  With Buffered, you can modify any of the items
	in the transaction.

	Transaction files are used with two different file structures.  One is just a single
	file, the storage file, and the other uses two files, the index file and the storage
	file.
	
	The first file structure is when the transaction is stored into the file in a single
	piece.  This type of transaction file is a single file that contains a memory image of
	the transaction data.  To traverse the different items in the transaction, you must
	start from the beginning of the file and step from one item to the next using the
	length information of each item to determine the file offset for the next item.

	The second file structure is when the transaction data is stored as multiple pieces,
	one per item, and there is an index file that contains information that allows the
	pieces to be found by going directly to the proper offset in the storage file.

	The various transaction file types are specified by a storage type which is one of the
	following constants.  The following table provides :
		TRN_TYPE_ITEMSTORAGE
		TRN_TYPE_PREVIOUS_CONDIMENT
			storage file header:  TRANITEMSTORAGEHEADER
			storage file size:    TrnInformation.usTranItemStoVli
            index file handle:    TrnInformation.hsTranItemIndex    TRN_TYPE_ITEMSTORAGE

		TRN_TYPE_NOITEMSTORAGE
			storage file header:  TRANITEMSTORAGEHEADER
            storage file handle:  TrnInformation.hsTranItemStorage
			storage file size:    TrnInformation.usTranItemStoVli
            index file handle:    TrnInformation.hsTranNoItemIndex 

		TRN_TYPE_CONSSTORAGE
		TRN_TYPE_PREVIOUS_CONDIMENT_CONS
			storage file header:  TRANCONSSTORAGEHEADER
            storage file handle:  TrnInformation.hsTranConsStorage
			storage file size:    TrnInformation.usTranConsStoVli
            index file handle:    TrnInformation.hsTranConsIndex

		TRN_TYPE_NOCONSSTORAGE
			storage file header:  TRANITEMSTORAGEHEADER
            storage file handle:  TrnInformation.hsTranConsStorage
			storage file size:    TrnInformation.usTranConsStoVli
            index file handle:    TrnInformation.hsTranNoConsIndex;

		TRN_TYPE_SPLITA
			storage file header:  TRANCONSSTORAGEHEADER
			memory resident data: TrnSplitA
            storage file handle:  TrnSplitA.hsTranConsStorage
			storage file size:    TrnSplitA.usTranConsStoVli
            index file handle:    TrnSplitA.hsTranConsIndex

		TRN_TYPE_SPLITB
			storage file header:  TRANCONSSTORAGEHEADER
			memory resident data: TrnSplitB
            storage file handle:  TrnSplitB.hsTranConsStorage
			storage file size:    TrnSplitB.usTranConsStoVli
            index file handle:    TrnSplitB.hsTranConsIndex

		TRN_CONSOLI_STO:
			storage file header:  TRANCONSSTORAGEHEADER
			memory resident data: TrnInformation
            storage file handle:  TrnInformation.hsTranConsStorage
			storage file size:    sizeof(TRANCONSSTORAGEHEADER) + TrnInformation.usTranConsStoVli
            index file handle:    

		TRN_POSTREC_STO:
			storage file header:  sizeof(USHORT) + sizeof(TRANGCFQUAL) + sizeof(TRANCURQUAL) + sizeof(USHORT)
			memory resident data: TrnInformation
            storage file handle:  TrnInformation.hsTranPostRecStorage
			storage file size:    
            index file handle:    


Now then, first of all, what I now understand is the way the transaction storage file
and the index files are being used.

The original architecture was that the transaction file contained all items except
for the last one entered.  So long as a new item was not entered or a total key
pressed, the last item entered was held in a memory buffer so that it could be modified
such as by adding condiments.  However, once a new item was entered or a total key
pressed, the current item was moved from the local memory buffer to the transaction
file where it could no longer be modified.

As part of this process of moving the current item to the transaction file, NHPOS
tries to economize on file space.  So instead of always adding the current item to
the transaction storage file, NHPOS instead looks to see if there is already a
similar item with similar condiments.  If there is then instead of adding the
current item, NHPOS instead increments the quantities of the existing item in the
transaction storage file adding a new index entry to the two index files that hold
all of the index data,  TRN_TYPE_NOITEMSTORAGE and TRN_TYPE_NOCONSSTORAGE.

If there is not such an item then it is added to the storage file and an index is added
to both of the index files for each of the storage files.  For storage type TRN_TYPE_ITEMSTORAGE
then the index files for both TRN_TYPE_NOITEMSTORAGE and TRN_TYPE_ITEMSTORAGE are updated.  For
storage type TRN_TYPE_CONSSTORAGE then the index files for both TRN_TYPE_NOCONSSTORAGE and
TRN_TYPE_CONSSTORAGE are updated.

What this means is that there is only a single version of the item in the transaction
storage file but there may be multiple index entries pointing to the single version.
The actual quantities for a particular item entry are kept in the index which points
to the single version of the item in the transaction storage file where the details
of the item such as condiments are stored.

This architecture worked because once an item had been transferred to the transaction
storage file, it could no longer be modified.  It could be voided or discounted but
it could not have its condiments, modified.

Each stored item has a pointer to a void for the item.  This pointer may be zero
in which case the void does not exist.  There is only one void item in the transaction
storage file for a particular normal item.  If multiple voids are done, then there
will be a new index entry for each of the void operations but they will all point
to the single version of the void in the transaction storage file.

Using this storage scheme it is very easy for the consolidation to work because
items are consolidated as they are entered so by displaying the transaction storage
file, a consolidated view can be shown.  If an itemized view is desired then the
transaction is displayed by using the index file to pull the particular items
from the transaction storage file and before displaying the item modify the
quantity, etc using the data that is stored in the index entry.


Now in the above, I speak of a transaction storage file but there are actually
several transaction storage files each of which have their own index files.  These
are basically called the Consolidated Transaction File and the Non-Consolidated
Transaction File.  It appears that there are some types of actions that generate
transaction items that go into both files while other types of actions generate
transaction items that go only into one or the other.  Both of these files seem
to be handled in the same way so far as consolidation of items and the index files
so I am not sure what the difference between Consolidated and Non-Consolidated
is, what is the meaning of the two terms.  The meaning seems to be a business
domain meaning rather than a technical data file meaning.

What I am seeing is that at some places in the code there are two flags that
are used to indicate that a particular item being stored should go in one or
the other or both of the types of transaction storage files.  If the flag
NOT_ITEM_STORAGE is set in the data of the item being stored, then the item
is not to be stored in the Item Storage File.  If the flag NOT_CONSOLI_STORAGE is
set in the data of the item being stored, then the item is not to be store in the
Consolidated Storage File.

For some types of items created as part of handling an operator action, these
flags are ignored and they are always stored in the two files however for most
types of items the flags are checked.  By operator actions I mean such things as
starting a transaction by entering the first action, entering an item or voiding
it, pressing a Total key, pressing a Tender key, entering a discount, entering a coupon, etc.

Some types of item actions are auto generated by the software.  For instance
when the first item of a transaction is entered, the system auto generates an
item of Transaction Open.  When a Tender key or Total key is pressed, the system
will auto generate an item of Print Data with the trailer information.


**/

#include	<tchar.h>
#include    <stdlib.h>
#include    <stdio.h>
#include    <string.h>

#include    <ecr.h>
#include    <pif.h>
#include    <regstrct.h>
#include    <transact.h>
#include    <trans.h>
#include    <prt.h>
#include    <rfl.h>
#include    <paraequ.h>
#include    <appllog.h>
#include    <item.h>
#include    <prtprty.h>
#include    <para.h>
#include    <maint.h>


SHORT TrnStoSalesSearchVoidItem( ITEMSALES          *pItemSales,
                         UCHAR              *puchSrcZip,
                         TRANSTOSALESSRCH   *pNormalItem,
                         TRANSTOSALESSRCH   *pVoidItem,
                         TrnStorageType     sStorageType,
                         SHORT              sType );
SHORT TrnStoSalesUpdateVoidItem( ITEMSALES          *pItemSales,
                         UCHAR              *puchSrcZip,
                         TRANSTOSALESSRCH   *pNormalItem,
                         TRANSTOSALESSRCH   *pVoidItem,
                         TrnStorageType     sStorageType );

SHORT TrnStoInsertFrontIndex( TRANSTORAGESALESNON *pTranNon,
                         TrnVliOffset         usItemOffset,
                         TrnStorageType       sStorageType );

static SHORT   TrnPaidOrderCheckSize( VOID *pData );


/* 05/20/01 */
extern TRNITEMSTOREPLUMNEM		TrnItemStorePluMnem;
extern USHORT					usItemStoredPluMnem;
extern PifSemHandle				husTrnSemaphore;

static TrnVliOffset   usTrnStoItemNormalOffset = 0;
static TrnVliOffset   usTrnStoConsNormalOffset = 0;

TrnVliOffset  TrnFetchLastStoItemOffset (VOID)
{
	return usTrnStoItemNormalOffset;
}

TrnVliOffset  TrnFetchLastStoConsOffset (VOID)
{
	return usTrnStoConsNormalOffset;
}

/*==========================================================================
**    Synopsis: SHORT  TrnIsTranSameAsItem(UCHAR *puchWorkBuffer, ITEMSALES *pItemSales )
*
*    Input:     TRANSTORAGESALESNON *pTranNon
*               UCHAR *puchWorkBuffer
*               ITEMSALES *pItemSales
*    Output:    none
*    InOut:     none
*
*    Return:    0 if not a match, 1 if is a match
*
*    Description:  Uncompress the item read from the Transaction File, if it is an ITEMSALES
*                  and compare specific fields of the Transaction File item to the ITEMSALES
*                  item we are looking to store into the Transaction File.
*                  This function is used to determine if an item can be combined with an item
*                  already existing in the Transaction File or if it needs to be stored as a
*                  separate item.
==========================================================================*/
static SHORT  TrnIsTranSameAsItem (UCHAR *auchTranStorageWork, ITEMSALES *pItemSales)
{
	TRANSTORAGESALESNON * pTranNon = TRANSTORAGENONPTR ( auchTranStorageWork );
	SHORT      sRetStatus = 0;
    USHORT     usReadLen;

	if (pTranNon->uchMajorClass == CLASS_ITEMSALES) {
		if (pTranNon->uchMajorClass == pItemSales->uchMajorClass &&
				pTranNon->uchMinorClass == pItemSales->uchMinorClass &&
				pTranNon->usDeptNo == pItemSales->usDeptNo &&
				pTranNon->uchPPICode == pItemSales->uchPPICode &&
				memcmp (pTranNon->auchPLUNo, pItemSales->auchPLUNo, sizeof(pTranNon->auchPLUNo)) == 0 &&
				pTranNon->fsLabelStatus == pItemSales->fsLabelStatus)
		{
			ITEMSALES  TranItem = {0};

			/* decompress itemize data without mnemonic to reduce processing time on Satellite Terminal */
			usReadLen = RflGetStorageItem (&TranItem, auchTranStorageWork, RFL_WITHOUT_MNEM);
			if (TranItem.uchAdjective == pItemSales->uchAdjective &&
				TranItem.uchCondNo == pItemSales->uchCondNo &&
				TranItem.usReasonCode == pItemSales->usReasonCode)
			{
				// check to see if the condiment data is the same or not. if it is not then even
				// though the main item PLU is the same, different condiments means it is really a
				// different item.
				int iLoop;
				for (iLoop = 0; iLoop < pItemSales->uchCondNo; iLoop++) {
					if (memcmp (TranItem.Condiment[iLoop].auchPLUNo, pItemSales->Condiment[iLoop].auchPLUNo, sizeof(TranItem.Condiment[iLoop].auchPLUNo)) != 0)
						// condiments do not match so return Not A Match.
						sRetStatus = 0;
						return  sRetStatus;
				}
				if (iLoop >= pItemSales->uchCondNo) {
					// condiments do match so probably a match but check other conditions first.
					sRetStatus = 1;
				}
			}
			if (TranItem.aszNumber[0][0] || pItemSales->aszNumber[0][0]) {
				// check to see if the reference number information attached to the ITEMSALES item
				// that was rung up matches this possible duplicate item in the Transaction File data.
				// we want to separate out items that have different reference number information.
				int iLoop;
				for (iLoop = 0; iLoop < NUM_OF_NUMTYPE_ENT; iLoop++) {
					if (_tcsncmp (TranItem.aszNumber[iLoop], pItemSales->aszNumber[iLoop], NUM_NUMBER) != 0) {
						// reference numbers do not match so return Not A Match.
						sRetStatus = 0;
						return  sRetStatus;
					}
				}
				if (iLoop >= NUM_OF_NUMTYPE_ENT) {
					// reference numbers do match so probably a match but check other conditions first.
					sRetStatus = 1;
				}
			}
		}
	}
	return  sRetStatus;
}

static PifFileHandle  TrnDetermineIndexFileHandle (TrnStorageType  sStorageType)
{
    PifFileHandle   hsIndexFile = TRN_ERROR;

    switch ( sStorageType ) {
		case TRN_TYPE_ITEMSTORAGE:      /* item storage */
			hsIndexFile = TrnInformation.hsTranItemIndex;
			break;

		case TRN_TYPE_NOITEMSTORAGE:    /* no-cons. item storage */
			hsIndexFile = TrnInformation.hsTranNoItemIndex;
			break;

		case TRN_TYPE_CONSSTORAGE:      /* consolidation storage */
			hsIndexFile = TrnInformation.hsTranConsIndex;
			break;

		case TRN_TYPE_NOCONSSTORAGE:    /* no-cons. consolidation storage */
			hsIndexFile = TrnInformation.hsTranNoConsIndex;
			break;

		case TRN_TYPE_SPLITA:
			hsIndexFile = TrnSplitA.hsTranConsIndex;
			break;

		case TRN_TYPE_SPLITB:
			hsIndexFile = TrnSplitB.hsTranConsIndex;
			break;

		default:
			hsIndexFile =  ( TRN_ERROR );
			break;
    }

	return hsIndexFile;
}

/*==========================================================================
**    Synopsis: USHORT  TrnGetStorageSize( USHORT usType )
*
*    Input:     USHORT  usType
*    Output:    none
*    InOut:     none
*
*    Return:    storage size
*
*    Description:   get item/consoli storage size
==========================================================================*/

TrnVliOffset  TrnGetStorageSize(TrnStorageType  sStorageType)
{
	TrnVliOffset  usVliSize = 0;

	switch (sStorageType) {
		case TRN_TYPE_ITEMSTORAGE:  /* item storage */
		case TRN_TYPE_PREVIOUS_CONDIMENT:  /* item storage */
			usVliSize = TrnInformation.usTranItemStoVli;
			break;

		case TRN_TYPE_CONSSTORAGE:  /* consolidation storage */
		case TRN_TYPE_PREVIOUS_CONDIMENT_CONS:  /* item storage */
			usVliSize = TrnInformation.usTranConsStoVli;
			break;

		case TRN_TYPE_SPLITA:       /* Split A,  R3.1 */
			usVliSize = TrnSplitA.usTranConsStoVli;
			break;

		case TRN_TYPE_SPLITB:       /* Split B,  R3.1 */
			usVliSize = TrnSplitB.usTranConsStoVli;
			break;

		default:
			{
				char  xBuff[128];
				sprintf (xBuff, "==ERROR: TrnGetStorageSize() Unexpected sStorageType %d", sStorageType);
				NHPOS_ASSERT_TEXT((TRN_TYPE_ITEMSTORAGE == sStorageType), xBuff);
			}
			break;
	}

	return usVliSize;
}

/*
*==========================================================================
**    Synopsis: SHORT   TrnStoGetStorageInfo( SHORT  sStorageType,
*                                             SHORT  *phsStorage,
*                                             USHORT *pusHdrSize,
*                                             USHORT *pusVliSize )
*
*       Input:  SHORT   sStorageType    -   type of storage file
*                           TRN_TYPE_ITEMSTORAGE, TRN_TYPE_CONSSTORAGE
*                           TRN_TYPE_SPLITA, TRN_TYPE_SPLITB
*               SHORT   *phsStorage     -   address of storage handle
*               USHORT  *pusHdrSize     -   address of storage header size
*               USHORT  *pusVliSize     -   address of storage Vli size
*      Output:  Nothing
*       InOut:  Nothing
*
*    Return:    TRN_SUCCESS -   function is successful         R3.1
*               TRN_ERROR   -   function is failed
*
**  Description:
*       retrieve specified storage file information.
*==========================================================================
*/
TrnStorageType TrnDetermineStorageType (VOID)
{
	TrnStorageType  sStorageType;

    /*--- System is Postcheck or Store/Recall ---*/
    switch (TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) {
		case CURQUAL_STORE_RECALL:
			sStorageType = TRN_TYPE_CONSSTORAGE;
			break;

		case CURQUAL_POSTCHECK:
			if ((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_CASHIER) {
				sStorageType = TRN_TYPE_ITEMSTORAGE;
			} else {
				sStorageType = TRN_TYPE_CONSSTORAGE;
			}
			break;

		case CURQUAL_PRE_BUFFER:
		case CURQUAL_PRE_UNBUFFER:
		default:
			sStorageType = TRN_TYPE_ITEMSTORAGE;
			break;
    }
	return sStorageType;
}

SHORT TrnStoGetStorageInfo( TrnStorageType  sStorageType, PifFileHandle  *phsStorage, USHORT *pusHdrSize, TrnVliOffset *pusVliSize )
{
    switch ( sStorageType ) {

    case TRN_TYPE_ITEMSTORAGE:  /* item storage */
	case TRN_TYPE_PREVIOUS_CONDIMENT:  /* item storage */
        if ( phsStorage != NULL ) {
            *phsStorage = TrnInformation.hsTranItemStorage;
        }
        if ( pusHdrSize != NULL ) {
            *pusHdrSize = sizeof( TRANITEMSTORAGEHEADER );
        }
        if ( pusVliSize != NULL ) {
            *pusVliSize = TrnInformation.usTranItemStoVli;
        }
        break;

    case TRN_TYPE_CONSSTORAGE:  /* consolidation storage */
	case TRN_TYPE_PREVIOUS_CONDIMENT_CONS:  /* item storage */
        if ( phsStorage != NULL ) {
            *phsStorage = TrnInformation.hsTranConsStorage;
        }
        if ( pusHdrSize != NULL ) {
            *pusHdrSize = sizeof( TRANCONSSTORAGEHEADER );
        }
        if ( pusVliSize != NULL ) {
            *pusVliSize = TrnInformation.usTranConsStoVli;
        }
        break;

    case TRN_TYPE_SPLITA:       /* Split A,  R3.1 */
        if ( phsStorage != NULL ) {
            *phsStorage = TrnSplitA.hsTranConsStorage;
        }
        if ( pusHdrSize != NULL ) {
            *pusHdrSize = sizeof( TRANCONSSTORAGEHEADER );
        }
        if ( pusVliSize != NULL ) {
            *pusVliSize = TrnSplitA.usTranConsStoVli;
        }
        break;

    case TRN_TYPE_SPLITB:       /* Split B,  R3.1 */
        if ( phsStorage != NULL ) {
            *phsStorage = TrnSplitB.hsTranConsStorage;
        }
        if ( pusHdrSize != NULL ) {
            *pusHdrSize = sizeof( TRANCONSSTORAGEHEADER );
        }
        if ( pusVliSize != NULL ) {
            *pusVliSize = TrnSplitB.usTranConsStoVli;
        }
        break;
   
    case TRN_TYPE_POSTRECSTORAGE:       /* Post Receipt,  R3.1 */
        if ( phsStorage != NULL ) {
            *phsStorage = TrnInformation.hsTranPostRecStorage;
        }
        if ( pusHdrSize != NULL ) {
            *pusHdrSize = sizeof( TRANPOSTRECSTORAGEHEADER );
        }
        if ( pusVliSize != NULL ) {
			// in order to determine the end of data in the Post Receipt file we have to read it
			// from the file itself. the usVli of the Post Receipt file is not maintained in memory.
			ULONG           ulReadPos = sizeof( TRANPOSTRECSTORAGEHEADER_VLI );
			ULONG			ulActualBytesRead;
			TrnVliOffset    usVli;                      /* 6/5/96 */

			//11-1103- SR 201 JHHJ
			TrnReadFile(ulReadPos, &usVli, sizeof(usVli), TrnInformation.hsTranPostRecStorage, &ulActualBytesRead);
            *pusVliSize = usVli;
        }
        break;
   
    default:
 		NHPOS_ASSERT_TEXT("**ERROR", "**ERROR: TrnStoGetStorageInfo() Unknown sStorage type");
       return ( TRN_ERROR );
    }
    return ( TRN_SUCCESS );
}

SHORT TrnStoGetStorageIndexInfo( TrnStorageType  sStorageType,
                            PifFileHandle  *phsStorage,
                            PifFileHandle  *phsItemIndex,
                            PifFileHandle  *phsNoItemIndex,
                            USHORT         *pusHdrSize,
                            TrnVliOffset   *pusVliSize )
{
    switch ( sStorageType ) {

    case TRN_TYPE_ITEMSTORAGE:         /* item storage */
	case TRN_TYPE_PREVIOUS_CONDIMENT:  /* item storage */
	case TRN_TYPE_NOITEMSTORAGE:       /* no-cons. item storage */
        if ( phsStorage != NULL ) {
            *phsStorage = TrnInformation.hsTranItemStorage;
        }
        if ( phsItemIndex != NULL ) {
            *phsItemIndex = TrnInformation.hsTranItemIndex;
        }
        if ( phsNoItemIndex != NULL ) {
            *phsNoItemIndex = TrnInformation.hsTranNoItemIndex;
        }
        if ( pusHdrSize != NULL ) {
            *pusHdrSize = sizeof( TRANITEMSTORAGEHEADER );
        }
        if ( pusVliSize != NULL ) {
            *pusVliSize = TrnInformation.usTranItemStoVli;
        }
        break;


    case TRN_TYPE_CONSSTORAGE:              /* consolidation storage */
	case TRN_TYPE_PREVIOUS_CONDIMENT_CONS:  /* item storage */
	case TRN_TYPE_NOCONSSTORAGE:            /* no-cons. consolidation storage */
        if ( phsStorage != NULL ) {
            *phsStorage = TrnInformation.hsTranConsStorage;
        }
        if ( phsItemIndex != NULL ) {
            *phsItemIndex = TrnInformation.hsTranConsIndex;
        }
        if ( phsNoItemIndex != NULL ) {
            *phsNoItemIndex = TrnInformation.hsTranNoConsIndex;
        }
        if ( pusHdrSize != NULL ) {
            *pusHdrSize = sizeof( TRANCONSSTORAGEHEADER );
        }
        if ( pusVliSize != NULL ) {
            *pusVliSize = TrnInformation.usTranConsStoVli;
        }
        break;

    case TRN_TYPE_SPLITA:       /* Split A,  R3.1 */
        if ( phsStorage != NULL ) {
            *phsStorage = TrnSplitA.hsTranConsStorage;
        }
        if ( phsItemIndex != NULL ) {
            *phsItemIndex = TrnSplitA.hsTranConsIndex;
        }
        if ( phsNoItemIndex != NULL ) {
            *phsNoItemIndex = 0;
        }
        if ( pusHdrSize != NULL ) {
            *pusHdrSize = sizeof( TRANCONSSTORAGEHEADER );
        }
        if ( pusVliSize != NULL ) {
            *pusVliSize = TrnSplitA.usTranConsStoVli;
        }
        break;

    case TRN_TYPE_SPLITB:       /* Split B,  R3.1 */
        if ( phsStorage != NULL ) {
            *phsStorage = TrnSplitB.hsTranConsStorage;
        }
        if ( phsItemIndex != NULL ) {
            *phsItemIndex = TrnSplitB.hsTranConsIndex;
        }
        if ( phsNoItemIndex != NULL ) {
            *phsNoItemIndex = 0;
        }
        if ( pusHdrSize != NULL ) {
            *pusHdrSize = sizeof( TRANCONSSTORAGEHEADER );
        }
        if ( pusVliSize != NULL ) {
            *pusVliSize = TrnSplitB.usTranConsStoVli;
        }
        break;
   
    default:
        return ( TRN_ERROR );
    }
    return ( TRN_SUCCESS );
}


static  SHORT TrnStoReplaceInsertAppendIndexStatus = TRN_SUCCESS;

static UCHAR  TrnStoConflateMinorClass (UCHAR  uchMinorClass)
{
	switch (uchMinorClass) {
		case CLASS_PLU:            /* PLU */
		case CLASS_PLUITEMDISC:    /* PLU w/item discount */
		case CLASS_PLUMODDISC:     /* PLU w/modifier discount */
			uchMinorClass = CLASS_PLU;
			break;

		case CLASS_DEPT:              /* dept */
		case CLASS_DEPTITEMDISC:      /* dept w/item discount */
		case CLASS_DEPTMODDISC:       /* dept w/modifier discount */
			uchMinorClass = CLASS_DEPT;
			break;

		case CLASS_OEPPLU:          /* order entry promot item */
		case CLASS_OEPITEMDISC:     /* OEP item w/item discount */
		case CLASS_OEPMODDISC:      /* OEP item w/modifier discount */
			uchMinorClass = CLASS_OEPPLU;
			break;
	}

	return uchMinorClass;
}

SHORT TrnStoReplaceStatusIndex (VOID)
{
	return TrnStoReplaceInsertAppendIndexStatus;
}
SHORT TrnStoReplaceIndex ( TRANSTORAGESALESNON *pTranNon, TrnVliOffset usItemOffset, TrnStorageType sStorageType )
{
    PifFileHandle  hsIndexFile;
    PRTIDX         IndexNew;

	if ((hsIndexFile = TrnDetermineIndexFileHandle(sStorageType)) < 0) {
		TrnStoReplaceInsertAppendIndexStatus = (TRN_ERROR);
		return (TrnStoReplaceInsertAppendIndexStatus);
	}

    /* --- prepare print priority index to insert storage file --- */
    TrnStoReplaceInsertAppendIndexStatus = TrnStoSetupPrtIdx( &IndexNew, pTranNon, usItemOffset );

    if (TrnStoReplaceInsertAppendIndexStatus != TRN_SUCCESS) {
        return (TrnStoReplaceInsertAppendIndexStatus);
    }

    /* --- search for replacement position  --- */
	if (pTranNon != NULL) {
		if (pTranNon->uchMajorClass == CLASS_ITEMSALES) {
			PRTIDXHDR      IdxFileInfo = {0};
			ULONG		   ulActualBytesRead;//RPH 11-11-3 SR# 201

			TrnReadFile( 0, &IdxFileInfo, sizeof(PRTIDXHDR), hsIndexFile, &ulActualBytesRead );
			if (pTranNon->uchMinorClass == CLASS_ITEMORDERDEC) {
				TrnVliOffset   sMiddleIdx;

				for (sMiddleIdx = 0; sMiddleIdx < IdxFileInfo.uchNoOfItem; sMiddleIdx++) {
					PRTIDX         PrtIdxMid = {0};

					TrnReadFile( PRTIDX_OFFSET(sMiddleIdx), &PrtIdxMid, sizeof(PRTIDX), hsIndexFile, &ulActualBytesRead );

					// Okay, we look for an existing order declaration item so
					// we replace it with the new one.
					if (PrtIdxMid.uchMajorClass == pTranNon->uchMajorClass &&
						PrtIdxMid.uchMinorClass == pTranNon->uchMinorClass) {
						TrnWriteFile( PRTIDX_OFFSET(sMiddleIdx), &IndexNew, sizeof(PRTIDX), hsIndexFile );
						TrnStoReplaceInsertAppendIndexStatus = TRN_SUCCESS_REPLACE;
						break;
					}
				}
			}
			else if ((pTranNon->fsLabelStatus & ITM_CONTROL_PCHG) == 0) {
				TrnVliOffset   sMiddleIdx;

				for (sMiddleIdx = 0; sMiddleIdx < IdxFileInfo.uchNoOfItem; sMiddleIdx++) {
					PRTIDX         PrtIdxMid = {0};

					TrnReadFile( PRTIDX_OFFSET(sMiddleIdx), &PrtIdxMid, sizeof(PRTIDX), hsIndexFile, &ulActualBytesRead );

					// Okay, we look for an existing sales item so we replace it 
					// with the new one.  We also are checking the sign of the quantity
					// because we want to update void with void and normal with normal.
					if (PrtIdxMid.uchMajorClass == pTranNon->uchMajorClass &&
						PrtIdxMid.uchMinorClass == pTranNon->uchMinorClass &&
						PrtIdxMid.usUniqueID == pTranNon->usUniqueID
					) {
						PrtIdxMid.lProduct += IndexNew.lProduct;
						PrtIdxMid.lQTY += IndexNew.lQTY;
						TrnWriteFile( PRTIDX_OFFSET(sMiddleIdx), &PrtIdxMid, sizeof(PRTIDX), hsIndexFile );
						TrnStoReplaceInsertAppendIndexStatus = (TRN_SUCCESS_REPLACE);
						break;
					}
				}
			}
		}
    }
	return TrnStoReplaceInsertAppendIndexStatus;
}


SHORT TrnStoReplaceInsertIndex ( TRANSTORAGESALESNON *pTranNon,
                         TrnVliOffset    usItemOffset,
                         TrnStorageType  sStorageType )
{
    PifFileHandle    hsIndexFile;
    PRTIDX           IndexNew;

    /* --- retrieve handle of index file to append index --- */
	if ((hsIndexFile = TrnDetermineIndexFileHandle(sStorageType)) < 0) {
		TrnStoReplaceInsertAppendIndexStatus = (TRN_ERROR);
		return (TrnStoReplaceInsertAppendIndexStatus);
	}

    /* --- prepare print priority index to insert storage file --- */
    TrnStoReplaceInsertAppendIndexStatus = TrnStoSetupPrtIdx( &IndexNew, pTranNon, usItemOffset );

    if (TrnStoReplaceInsertAppendIndexStatus != TRN_SUCCESS) {
        return (TrnStoReplaceInsertAppendIndexStatus);
    }

	// First of all search the index to see if this item already exists in the
	// index file.

    /* --- search for insert position, and create vacant space to be stored new index --- */
	if (pTranNon != NULL) {
		if (pTranNon->uchMajorClass == CLASS_ITEMSALES) {
			PRTIDXHDR        IdxFileInfo = {0};
			ULONG		     ulActualBytesRead;//RPH 11-11-3 SR# 201

			TrnReadFile( 0, &IdxFileInfo, sizeof(PRTIDXHDR), hsIndexFile, &ulActualBytesRead );
			if (pTranNon->uchMinorClass == CLASS_ITEMORDERDEC) {
				TrnVliOffset     sMiddleIdx;

				for (sMiddleIdx = 0; sMiddleIdx < IdxFileInfo.uchNoOfItem; sMiddleIdx++) {
					PRTIDX           PrtIdxMid = {0};

					TrnReadFile( PRTIDX_OFFSET(sMiddleIdx), &PrtIdxMid, sizeof(PRTIDX), hsIndexFile, &ulActualBytesRead );

					// Okay, we look for an existing order declaration item so
					// we replace it with the new one.
					if (PrtIdxMid.uchMajorClass == pTranNon->uchMajorClass &&
						PrtIdxMid.uchMinorClass == pTranNon->uchMinorClass) {
						TrnWriteFile( PRTIDX_OFFSET(sMiddleIdx), &IndexNew, sizeof(PRTIDX), hsIndexFile );
						TrnStoReplaceInsertAppendIndexStatus = TRN_SUCCESS_REPLACE;
						return TrnStoReplaceInsertAppendIndexStatus;
					}
				}
				// We did not find an existing order declaration item so we are now
				// going to insert this one.
				// We are using the standard that an order declaration is always
				// the first item in the transaction list.  So since this is an
				// order declaration item, we will insert it as the first index.
				pTranNon->uchPrintPriority = 0;
				TrnStoReplaceInsertAppendIndexStatus = TrnStoInsertFrontIndex (pTranNon, usItemOffset, sStorageType);
				return TrnStoReplaceInsertAppendIndexStatus;
			}
			else if ((pTranNon->fsLabelStatus & ITM_CONTROL_PCHG) == 0 && (pTranNon->fsLabelStatus & ITM_CONTROL_RETURN_OLD) == 0) {
				TrnVliOffset     sMiddleIdx;

				for (sMiddleIdx = 0; sMiddleIdx < IdxFileInfo.uchNoOfItem; sMiddleIdx++) {
					PRTIDX           PrtIdxMid = {0};

					TrnReadFile( PRTIDX_OFFSET(sMiddleIdx), &PrtIdxMid, sizeof(PRTIDX), hsIndexFile, &ulActualBytesRead );

					// Okay, we look for an existing sales item so we replace it 
					// with the new one.  We also are checking the sign of the quantity
					// because we want to update void with void and normal with normal.
					if (PrtIdxMid.uchMajorClass == pTranNon->uchMajorClass &&
						PrtIdxMid.uchMinorClass == pTranNon->uchMinorClass &&
						PrtIdxMid.usUniqueID == pTranNon->usUniqueID       &&
						( (PrtIdxMid.lQTY < 0 && pTranNon->lQTY < 0) || (PrtIdxMid.lQTY > 0 && pTranNon->lQTY > 0) )
					) {
						TrnWriteFile( PRTIDX_OFFSET(sMiddleIdx), &IndexNew, sizeof(PRTIDX), hsIndexFile );
						TrnStoReplaceInsertAppendIndexStatus = (TRN_SUCCESS_REPLACE);
						return TrnStoReplaceInsertAppendIndexStatus;
					}
				}
			}
		}
    }
	TrnStoReplaceInsertAppendIndexStatus = TrnStoInsertIndex (pTranNon, usItemOffset, sStorageType);
	return TrnStoReplaceInsertAppendIndexStatus;
}



SHORT TrnStoReplaceAppendIndex ( TRANSTORAGESALESNON *pTranNon, TrnVliOffset usItemOffset, TrnStorageType sStorageType )
{
    PifFileHandle  hsIndexFile;
    PRTIDX         IndexNew;

    /* --- retrieve handle of index file to append index --- */
	if ((hsIndexFile = TrnDetermineIndexFileHandle(sStorageType)) < 0) {
		TrnStoReplaceInsertAppendIndexStatus = (TRN_ERROR);
		return (TrnStoReplaceInsertAppendIndexStatus);
	}

    /* --- prepare print priority index to insert storage file --- */
    TrnStoReplaceInsertAppendIndexStatus = TrnStoSetupPrtIdx( &IndexNew, pTranNon, usItemOffset );

    if (TrnStoReplaceInsertAppendIndexStatus != TRN_SUCCESS) {
        return (TrnStoReplaceInsertAppendIndexStatus);
    }

	// First of all search the index to see if this item already exists in the
	// index file.

    /* --- search for insert position, and create vacant space to be stored
        new index --- */

	if (pTranNon != NULL) {
		UCHAR uchMinorClassTemp = pTranNon->uchMinorClass;

		// handle if uchMinor

		if (pTranNon->uchMajorClass == CLASS_ITEMSALES) {
			PRTIDXHDR      IdxFileInfo = {0};
			ULONG		   ulActualBytesRead;//RPH 11-11-3 SR# 201

			TrnReadFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), hsIndexFile, &ulActualBytesRead );
			if (pTranNon->uchMinorClass == CLASS_ITEMORDERDEC) {
				TrnVliOffset   sMiddleIdx;

				for (sMiddleIdx = 0; sMiddleIdx < IdxFileInfo.uchNoOfItem; sMiddleIdx++) {
					PRTIDX         PrtIdxMid = {0};

					//RPH 11-11-3 SR# 201
					TrnReadFile (PRTIDX_OFFSET(sMiddleIdx), &PrtIdxMid, sizeof(PRTIDX), hsIndexFile, &ulActualBytesRead );

					// Okay, we have found an existing order declaration item so
					// we replace it with the new one.
					if (PrtIdxMid.uchMajorClass == pTranNon->uchMajorClass &&
						PrtIdxMid.uchMinorClass == pTranNon->uchMinorClass) {
						TrnWriteFile (PRTIDX_OFFSET(sMiddleIdx), &IndexNew, sizeof(PRTIDX), hsIndexFile );
						TrnStoReplaceInsertAppendIndexStatus = TRN_SUCCESS_REPLACE;
						return TrnStoReplaceInsertAppendIndexStatus;
					}
				}
				// We did not find an existing order declaration item so we are now
				// going to insert this one.
				// We are using the standard that an order declaration is always
				// the first item in the transaction list.  So since this is an
				// order declaration item, we will insert it as the first index.
				pTranNon->uchPrintPriority = 0;
				return TrnStoInsertFrontIndex (pTranNon, usItemOffset, sStorageType );
				TrnStoReplaceInsertAppendIndexStatus = TrnStoInsertFrontIndex (pTranNon, usItemOffset, sStorageType);
				return TrnStoReplaceInsertAppendIndexStatus;
			}
			else if ((pTranNon->fsLabelStatus & ITM_CONTROL_PCHG) == 0 && (pTranNon->fsLabelStatus & ITM_CONTROL_RETURN_OLD) == 0) {
				TrnVliOffset   sMiddleIdx;

				for (sMiddleIdx = 0; sMiddleIdx < IdxFileInfo.uchNoOfItem; sMiddleIdx++) {
					PRTIDX         PrtIdxMid = {0};

					TrnReadFile( PRTIDX_OFFSET(sMiddleIdx), &PrtIdxMid, sizeof(PRTIDX), hsIndexFile, &ulActualBytesRead );

					// Okay, we look for an existing sales item so we replace it 
					// with the new one.  We also are checking the sign of the quantity
					// because we want to update void with void and normal with normal.
					if (PrtIdxMid.uchMajorClass == pTranNon->uchMajorClass &&
						PrtIdxMid.uchMinorClass == pTranNon->uchMinorClass &&
						PrtIdxMid.usUniqueID == pTranNon->usUniqueID       &&
						( (PrtIdxMid.lQTY < 0 && pTranNon->lQTY < 0) || (PrtIdxMid.lQTY > 0 && pTranNon->lQTY > 0) )
					) {
						TrnWriteFile( PRTIDX_OFFSET(sMiddleIdx), &IndexNew, sizeof(PRTIDX), hsIndexFile );
						TrnStoReplaceInsertAppendIndexStatus = TRN_SUCCESS_REPLACE;
						return TrnStoReplaceInsertAppendIndexStatus;
					}
				}
			}
		}
    }
	TrnStoReplaceInsertAppendIndexStatus = TrnStoAppendIndex (pTranNon, usItemOffset, sStorageType);
	return TrnStoReplaceInsertAppendIndexStatus;
}


SHORT TrnStoFindCopyIndexEntry ( TRANSTORAGESALESNON *pTranNon, TrnVliOffset usItemOffset, SHORT sFromStorageType, SHORT sToStorageType )
{
    PifFileHandle  hsFromIndexFile;
    PRTIDXHDR      FromIdxFileInfo;
    TrnVliOffset   sMiddleIdx;
	ULONG		   ulActualBytesRead;//RPH 11-11-3 SR# 201

    /* --- retrieve handle of index file to append index --- */
	if ((hsFromIndexFile = TrnDetermineIndexFileHandle(sFromStorageType)) < 0) {
		TrnStoReplaceInsertAppendIndexStatus = (TRN_ERROR);
		return (TrnStoReplaceInsertAppendIndexStatus);
	}

	TrnReadFile( 0, &FromIdxFileInfo, sizeof( PRTIDXHDR ), hsFromIndexFile, &ulActualBytesRead );

    /* --- search for insert position, and create vacant space to be stored
        new index --- */
	for (sMiddleIdx = 0; sMiddleIdx < FromIdxFileInfo.uchNoOfItem; sMiddleIdx++) {
		PRTIDX         PrtIdxMid = {0};

		//RPH 11-11-3 SR# 201
		TrnReadFile (PRTIDX_OFFSET(sMiddleIdx), &PrtIdxMid, sizeof(PRTIDX), hsFromIndexFile, &ulActualBytesRead );

		if (PrtIdxMid.usItemOffset == usItemOffset && pTranNon->usUniqueID != PrtIdxMid.usUniqueID) {
			pTranNon->usUniqueID = PrtIdxMid.usUniqueID;
			TrnStoInsertIndex (pTranNon, usItemOffset, sToStorageType);
			return sMiddleIdx + 1;
		}
	}
	return 0;
}


// --------------------------------------------------------------------------
// -------   Begin for TrnStorage() functionality    --------------------------

/*==========================================================================
**    Synopsis: SHORT   TrnStoOpen( ITEMTRANSOPEN *pItemTransOpen )
*
*    Input:     ITEMTRANSOPEN   *pItemTransOpen
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   compress and store transaction open data into the transaction file.
==========================================================================*/

SHORT TrnStoOpen( ITEMTRANSOPEN *pItemTransOpen )
{
    UCHAR   auchBuffer[TRN_WORKBUFFER];
    SHORT   sSize;
    SHORT   sReturn;

	NHPOS_ASSERT(pItemTransOpen->uchMajorClass == CLASS_ITEMTRANSOPEN);

    // compress the item data into a transaction storage record and
	// check if item and/or consoli. storage is full.
	// if not full then store the transaction data record into the appropriate files.

	sSize = TrnStoOpenComp( pItemTransOpen, auchBuffer );
    if (( sReturn = TrnChkStorageSize( sSize )) != TRN_SUCCESS ) {
        return ( sReturn );
    }

    if ( ! ( pItemTransOpen->fbStorageStatus & NOT_ITEM_STORAGE )) {
        TrnStoItemNormal( auchBuffer, sSize );
    }

    if ( ! ( pItemTransOpen->fbStorageStatus & NOT_CONSOLI_STORAGE )) {
        TrnStoConsNormal( auchBuffer, sSize );
    }
    return ( TRN_SUCCESS );
}

/*==========================================================================
**    Synopsis: SHORT   TrnStoOpenComp( ITEMTRANSOPEN *ItemTransOpen,
                            UCHAR *uchBuffer )
*
*    Input:     ITEMTRANSOPEN   *ItemTransOpen
*               UCHAR   *uchBuffer
*    Output:    UCHAR   *uchBuffer
*    InOut:     none
*
*    Return:    compressed size
*
*    Description:   compress transaction open data             R3.1
==========================================================================*/

SHORT    TrnStoOpenComp(ITEMTRANSOPEN *ItemTransOpen, UCHAR *uchBuffer)
{
    UCHAR   uchSize1;
    SHORT   sSize2;
    TRANSTORAGETRANSOPEN    Work;
	SHORT	sCompressedSize;

    *uchBuffer = uchSize1 = sizeof(TRANSTORAGETRANSOPENNON) + 1;    /* Major/Minor, Seat# */
    *(uchBuffer + 1) = ItemTransOpen->uchMajorClass;                /* major class */
    *(uchBuffer + 2) = ItemTransOpen->uchMinorClass;                /* minor class */
    *(uchBuffer + 3) = ItemTransOpen->uchSeatNo;                    /* Seat# */

    memcpy(&Work, &ItemTransOpen->ulCashierID, sizeof(TRANSTORAGETRANSOPEN));
    Work.fbStorageStatus = 0;
	/* storage status */

	NHPOS_ASSERT((TRN_WORKBUFFER - uchSize1 - 2) > 0);

     sCompressedSize = RflZip((VOID *)(uchBuffer + uchSize1 + 2),
        (USHORT)(TRN_WORKBUFFER - uchSize1 - 2), (VOID *)&Work, (USHORT)sizeof(TRANSTORAGETRANSOPEN)); /* ### Mod (2171 for Win32) V1.0 */

	 if(sCompressedSize == RFL_FAIL)
	 {
		 PifLog(MODULE_COMPRESS_DATA,LOG_EVENT_COMPRESS_OPEN_DATA);
	 }

	 sSize2 = 2 + sCompressedSize;

    *(USHORT *)(uchBuffer + uchSize1) = sSize2;                     /* compress size */

    return((SHORT)(uchSize1 + sSize2));                             /* whole size */
}

/*==========================================================================
**    Synopsis: SHORT   TrnStoSales( ITEMSALES *pItemSales )
*
*    Input:     ITEMSALES   *pItemSales
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   store sales data
==========================================================================*/
SHORT    TrnStoSalesAndDisc ( ITEMSALES *pItemSales, ITEMDISC *pItemDisc )
{
	TRANSTOSALESSRCH    CompItem = {0};
    SHORT               sReturn;
	BOOL                bDiscountNotUpdated = TRUE;

	// Let make it easy to find out whether this is a void item
	// or not by marking the fsLabelStatus with ITM_CONTROL_VOIDITEM
	// since this is available in the TRANSTORAGESALESNON struct.
	pItemSales->fsLabelStatus &= ~ITM_CONTROL_VOIDITEM;
	if (pItemSales->fbModifier & VOID_MODIFIER) {
		pItemSales->fsLabelStatus |= ITM_CONTROL_VOIDITEM;
	}

	pItemSales->fsLabelStatus &= ~ITM_CONTROL_RETURNORIG;
	if (pItemSales->fbModifier & RETURNS_ORIGINAL) {
		pItemSales->fsLabelStatus |= ITM_CONTROL_RETURNORIG;
	}

    CompItem.usItemSize = TrnStoSalesComp( pItemSales, CompItem.auchBuffer );
    if (( sReturn = TrnChkStorageSizeIfSales( CompItem.usItemSize )) != TRN_SUCCESS ) {
        return ( sReturn );
    }

    if ( (pItemSales->fbStorageStatus & NOT_ITEM_STORAGE) == 0) {
        if ( pItemSales->fbStorageStatus & NOT_ITEM_CONS ) {
            if ((pItemSales->uchMinorClass == CLASS_PLUITEMDISC) || (pItemSales->uchMinorClass == CLASS_SETITEMDISC)) {
				TRANSTOSALESSRCH    NormalItem = {0};
				TRANSTOSALESSRCH    VoidItem = {0};

                TrnStoSalesSearchVoidItem( pItemSales, CompItem.auchBuffer, &NormalItem, &VoidItem, TRN_TYPE_ITEMSTORAGE, 0 );
                TrnStoSalesUpdateVoidItem( pItemSales, CompItem.auchBuffer, &NormalItem, &VoidItem, TRN_TYPE_ITEMSTORAGE );
            } else {
                TrnStoItemNormal( CompItem.auchBuffer, CompItem.usItemSize );
            }
        } else {
			TRANSTOSALESSRCH    NormalItem = {0};
			TRANSTOSALESSRCH    VoidItem = {0};

            sReturn = TrnStoSalesSearch( pItemSales, CompItem.auchBuffer, &NormalItem, &VoidItem, TRN_TYPE_ITEMSTORAGE, 0 );
			if (sReturn == TRN_ERROR) {
				NHPOS_ASSERT_TEXT((sReturn != TRN_ERROR), "**ERROR: TrnStoSalesSearch() failed.");
				return TRN_ERROR;
			}

			if ( NormalItem.usItemOffset && (pItemSales->fbModifier & VOID_MODIFIER) == 0) {
				TRANSTORAGESALESNON *pNoCompNormal = TRANSTORAGENONPTR( NormalItem.auchBuffer );
				// In order to allow edit of an Order Declare by only a change of the uncompressed portion of the ITEMSALES
				// record, we are storing the Order Declare number in the usDeptNo field, which is part of the
				// uncompressed portion of the record, TRANSTORAGESALESNON, so we do not need to do the uncompress and
				// recompress of the compressed portion of the record, TRANSTORAGESALES, in order to update the
				// uchAdjective member. We do this to avoid changes in compressed data which may result in the compression
				// changing the length of the compressed portion of the record.
				//    Richard Chambers, Dec-21-2018
				pNoCompNormal->usDeptNo = pItemSales->usDeptNo;     // update Order Declare Number, (uchOrderDecNum) for CLASS_ITEMORDERDEC
				pNoCompNormal->lQTY += pItemSales->lQTY;
				pNoCompNormal->lProduct += pItemSales->lProduct;
				pNoCompNormal->sCouponQTY += pItemSales->sCouponQTY;
				if (pItemDisc && NormalItem.pDiscBuffer) {
					TRANSTORAGEDISCNON *pNoCompDisc = (TRANSTORAGEDISCNON *) (NormalItem.pDiscBuffer);
					pNoCompDisc->lAmount += pItemDisc->lAmount;
					pNoCompDisc->lQTY += pItemDisc->lQTY;
					bDiscountNotUpdated = FALSE;
				}
			}
			if ( VoidItem.usItemOffset && (pItemSales->fbModifier & VOID_MODIFIER) != 0) {
				TRANSTORAGESALESNON *pNoCompVoid = TRANSTORAGENONPTR( VoidItem.auchBuffer );
				pNoCompVoid->lQTY += pItemSales->lQTY;
				pNoCompVoid->lProduct += pItemSales->lProduct;
				pNoCompVoid->sCouponQTY += pItemSales->sCouponQTY;
				if (pItemDisc && VoidItem.pDiscBuffer) {
					TRANSTORAGEDISCNON *pNoCompDisc = (TRANSTORAGEDISCNON *) (VoidItem.pDiscBuffer);
					pNoCompDisc->lAmount += pItemDisc->lAmount;
					pNoCompDisc->lQTY += pItemDisc->lQTY;
					bDiscountNotUpdated = FALSE;
				}
			}
            TrnStoSalesUpdate( pItemSales, &CompItem, &NormalItem, &VoidItem, TRN_TYPE_ITEMSTORAGE );
            TrnStoIdxUpdate( pItemSales, &NormalItem, &VoidItem, TRN_TYPE_ITEMSTORAGE );
            /* V3.3 */
            TrnStoNoIdxUpdate( pItemSales, &NormalItem, &VoidItem, TRN_TYPE_NOITEMSTORAGE, 0 );
        }

		/* 05/20/01 */
		TrnStoItemSalesPLUMnemo(pItemSales);
    }

    if ( (pItemSales->fbStorageStatus & NOT_CONSOLI_STORAGE) == 0) {
		ITEMSALES     SalesWork = *pItemSales;

        if ( pItemSales->fbStorageStatus & NOT_ITEM_CONS ) {
            if ((pItemSales->uchMinorClass == CLASS_PLUITEMDISC) || (pItemSales->uchMinorClass == CLASS_SETITEMDISC)) {
				TRANSTOSALESSRCH    NormalItem = {0};
				TRANSTOSALESSRCH    VoidItem = {0};

                TrnStoSalesSearchVoidItem( &SalesWork, CompItem.auchBuffer, &NormalItem, &VoidItem, TRN_TYPE_CONSSTORAGE, 0 );
                TrnStoSalesUpdateVoidItem( &SalesWork, CompItem.auchBuffer, &NormalItem, &VoidItem, TRN_TYPE_CONSSTORAGE );
            } else {
                TrnStoConsNormal( CompItem.auchBuffer, CompItem.usItemSize);
            }
        } else {
			TRANSTOSALESSRCH    NormalItem = {0};
			TRANSTOSALESSRCH    VoidItem = {0};

            sReturn = TrnStoSalesSearch( &SalesWork, CompItem.auchBuffer, &NormalItem, &VoidItem, TRN_TYPE_CONSSTORAGE, 0 );
			if (sReturn == TRN_ERROR) {
				NHPOS_ASSERT_TEXT((sReturn != TRN_ERROR), "**ERROR: TrnStoSalesSearch() failed.");
				return TRN_ERROR;
			}

			if ( NormalItem.usItemOffset && (pItemSales->fbModifier & VOID_MODIFIER) == 0) {
				// In order to allow edit of an Order Declare by only a change of the uncompressed portion of the ITEMSALES
				// record, we are storing the Order Declare number in the usDeptNo field, which is part of the
				// uncompressed portion of the record, TRANSTORAGESALESNON, so we do not need to do the uncompress and
				// recompress of the compressed portion of the record, TRANSTORAGESALES, in order to update the
				// uchAdjective member. We do this to avoid changes in compressed data which may result in the compression
				// changing the length of the compressed portion of the record.
				//    Richard Chambers, Dec-21-2018
				TRANSTORAGESALESNON *pNoCompNormal = TRANSTORAGENONPTR( NormalItem.auchBuffer );
				pNoCompNormal->usDeptNo = pItemSales->usDeptNo;     // update Order Declare Number, (uchOrderDecNum) for CLASS_ITEMORDERDEC
				pNoCompNormal->lQTY += pItemSales->lQTY;
				pNoCompNormal->lProduct += pItemSales->lProduct;
				pNoCompNormal->sCouponQTY += pItemSales->sCouponQTY;
				if (pItemDisc && NormalItem.pDiscBuffer) {
					TRANSTORAGEDISCNON *pNoCompDisc = (TRANSTORAGEDISCNON *) (NormalItem.pDiscBuffer);
					pNoCompDisc->lAmount += pItemDisc->lAmount;
					pNoCompDisc->lQTY += pItemDisc->lQTY;
					bDiscountNotUpdated = FALSE;
				}
			}
			if ( VoidItem.usItemOffset && (pItemSales->fbModifier & VOID_MODIFIER) != 0) {
				TRANSTORAGESALESNON *pNoCompVoid = TRANSTORAGENONPTR( VoidItem.auchBuffer );
				pNoCompVoid->lQTY += pItemSales->lQTY;
				pNoCompVoid->lProduct += pItemSales->lProduct;
				pNoCompVoid->sCouponQTY += pItemSales->sCouponQTY;
				if (pItemDisc && VoidItem.pDiscBuffer) {
					TRANSTORAGEDISCNON *pNoCompDisc = (TRANSTORAGEDISCNON *) (VoidItem.pDiscBuffer);
					pNoCompDisc->lAmount += pItemDisc->lAmount;
					pNoCompDisc->lQTY += pItemDisc->lQTY;
					bDiscountNotUpdated = FALSE;
				}
			}
            TrnStoSalesUpdate( &SalesWork, &CompItem, &NormalItem, &VoidItem, TRN_TYPE_CONSSTORAGE );
            TrnStoIdxUpdate( &SalesWork, &NormalItem, &VoidItem, TRN_TYPE_CONSSTORAGE );
            TrnStoNoIdxUpdate( pItemSales, &NormalItem, &VoidItem, TRN_TYPE_NOCONSSTORAGE, 0 );
        }
    }

	if (pItemDisc && bDiscountNotUpdated) {
		TrnStoDisc(pItemDisc);
	}

    return ( TRN_SUCCESS );
}

SHORT    TrnStoSales( ITEMSALES *pItemSales )
{
	return TrnStoSalesAndDisc (pItemSales, 0);
}

SHORT    TrnStoSalesItemUpdate( ITEMSALES *pItemSales )
{
    SHORT               sReturn;

	// Let make it easy to find out whether this is a void item
	// or not by marking the fsLabelStatus with ITM_CONTROL_VOIDITEM
	// since this is available in the TRANSTORAGESALESNON struct.
	pItemSales->fsLabelStatus &= ~ITM_CONTROL_VOIDITEM;
	if (pItemSales->fbModifier & VOID_MODIFIER)
	{
		pItemSales->fsLabelStatus |= ITM_CONTROL_VOIDITEM;
	}

    if ( (pItemSales->fbStorageStatus & NOT_ITEM_STORAGE) == 0)
	{
		TRANSTOSALESSRCH    CompItem = {0};

        CompItem.usItemSize = TrnStoSalesComp( pItemSales, CompItem.auchBuffer );
        if (( sReturn = TrnChkStorageSizeIfSales( CompItem.usItemSize )) != TRN_SUCCESS )
		{
            return ( sReturn );
        }

        if ( pItemSales->fbStorageStatus & NOT_ITEM_CONS )
		{
            if ((pItemSales->uchMinorClass == CLASS_PLUITEMDISC) || (pItemSales->uchMinorClass == CLASS_SETITEMDISC))
			{
				TRANSTOSALESSRCH    NormalItem = {0};
				TRANSTOSALESSRCH    VoidItem = {0};

                TrnStoSalesSearchVoidItem( pItemSales, CompItem.auchBuffer, &NormalItem, &VoidItem, TRN_TYPE_ITEMSTORAGE, 0 );
                TrnStoSalesUpdateVoidItem( pItemSales, CompItem.auchBuffer, &NormalItem, &VoidItem, TRN_TYPE_ITEMSTORAGE );
            } else {
                TrnStoItemNormal( CompItem.auchBuffer, CompItem.usItemSize );
            }
        } else {
			TRANSTOSALESSRCH    NormalItem = {0}, NormalItemCons = {0};
			TRANSTOSALESSRCH    VoidItem = {0}, VoidItemCons = {0};
			TRANSTOSALESSRCH    SameItem = {0}, SameItemCons = {0};

            TrnStoSalesSearchIndex( pItemSales, CompItem.auchBuffer, &NormalItemCons, &VoidItemCons, &SameItemCons, &NormalItem, &VoidItem, &SameItem, TRN_TYPE_NOITEMSTORAGE, 0 );
            TrnStoSalesUpdateIndex( pItemSales, &CompItem, &NormalItem, &VoidItem, &SameItem, TRN_TYPE_ITEMSTORAGE );
            TrnStoIdxUpdate( pItemSales, &NormalItem, &VoidItem, TRN_TYPE_ITEMSTORAGE );
            /* V3.3 */
            TrnStoNoIdxUpdate( pItemSales, &NormalItem, &VoidItem, TRN_TYPE_NOITEMSTORAGE, 0 );
        }

		/* 05/20/01 */
		TrnStoItemSalesPLUMnemo(pItemSales);
    }

    if ( (pItemSales->fbStorageStatus & NOT_CONSOLI_STORAGE ) == 0)
	{
		ITEMSALES           SalesWork = *pItemSales;
		TRANSTOSALESSRCH    CompItem = {0};

        CompItem.usItemSize = TrnStoSalesComp( &SalesWork, CompItem.auchBuffer );
        if (( sReturn = TrnChkStorageSizeIfSales( CompItem.usItemSize )) != TRN_SUCCESS )
		{
            return ( sReturn );
        }

        if ( pItemSales->fbStorageStatus & NOT_ITEM_CONS )
		{
            if ((pItemSales->uchMinorClass == CLASS_PLUITEMDISC) || (pItemSales->uchMinorClass == CLASS_SETITEMDISC))
			{
				TRANSTOSALESSRCH    NormalItem = {0};
				TRANSTOSALESSRCH    VoidItem = {0};

                TrnStoSalesSearchVoidItem( &SalesWork, CompItem.auchBuffer, &NormalItem, &VoidItem, TRN_TYPE_CONSSTORAGE, 0 );
                TrnStoSalesUpdateVoidItem( &SalesWork, CompItem.auchBuffer, &NormalItem, &VoidItem, TRN_TYPE_CONSSTORAGE );
            } else
			{
                TrnStoConsNormal( CompItem.auchBuffer, CompItem.usItemSize );
            }
        } else {
			TRANSTOSALESSRCH    NormalItem = {0}, NormalItemCons = {0};
			TRANSTOSALESSRCH    VoidItem = {0}, VoidItemCons = {0};
			TRANSTOSALESSRCH    SameItem = {0}, SameItemCons = {0};

            TrnStoSalesSearchIndex( &SalesWork, CompItem.auchBuffer, &NormalItemCons, &VoidItemCons, &SameItemCons, &NormalItem, &VoidItem, &SameItem, TRN_TYPE_NOCONSSTORAGE, 0 );
            TrnStoSalesUpdateIndex( &SalesWork, &CompItem, &NormalItem, &VoidItem, &SameItem, TRN_TYPE_CONSSTORAGE );
            TrnStoIdxUpdate( &SalesWork, &NormalItem, &VoidItem, TRN_TYPE_CONSSTORAGE );
        }
    }
    return ( TRN_SUCCESS );
}


/*==========================================================================
**    Synopsis: SHORT   TrnStoSalesComp( ITEMSALES *ItemSales, UCHAR *uchBuffer
*
*    Input:     ITEMSALES   *ItemSales, UCHAR   *uchBuffer
*    Output:    none
*    InOut:     UCHAR   *uchBuffer
*
*    Return:    compressd size
*
*    Description:   compress sales data
*                   compression buffer is made up of a non-compressed and a compressed section.
*                   each section has its own length.
*                    - uchBuffer[0] - contains length of non-compressed section
*                    - uchBuffer[1] - first byte of non-compressed section
*                    - uchBuffer[uchBuffer[0]] - first byte of two byte (USHORT) lenth of compressed section.
*                    - uchBuffer[uchBuffer[0] + 2] - first byte of compressed section
*
*                   WARNING:  Any changes made to the TRANSTORAGESALES struct
*                   and the ITEMSALES struct must be reflected in this compression
*                   function, TrnStoSalesComp() as well as in the uncompression
*                   function RflGetStorageItem().
==========================================================================*/

SHORT    TrnStoSalesComp( ITEMSALES *ItemSales, UCHAR *uchBuffer )
{
    UCHAR    i;
    SHORT    sSize1, sSize2;
	SHORT    sCompressedSize;
	SHORT    numCounter=0; //US Customs
	TRANSTORAGESALES    Work = {0};
	
    sSize2 = sizeof( TRANSTORAGESALESNON ) + sizeof( UCHAR );
	NHPOS_ASSERT(sSize2 <= UCHAR_MAX);

    *uchBuffer = (UCHAR) sSize2;                     /* non-compress size */
    memcpy( &uchBuffer[1], &ItemSales->uchMajorClass, sizeof(TRANSTORAGESALESNON) );

    Work.lUnitPrice = ItemSales->lUnitPrice;                            /* unit price */
    Work.uchAdjective = ItemSales->uchAdjective;                        /* adjective */
    Work.fbModifier = ItemSales->fbModifier;                            /* modifier */
    Work.fbModifier2 = ItemSales->fbModifier2;                            /* modifier */
	//US Customs to store all #/Type entries
	for( ; numCounter < NUM_OF_NUMTYPE_ENT; numCounter++){
		memcpy(Work.aszNumber[numCounter], ItemSales->aszNumber[numCounter], sizeof(Work.aszNumber[0]) );
		if(Work.aszNumber[numCounter][0] == '\0'){
			break;
		}
	}
	if(ItemSales->uchMinorClass == CLASS_ITEMORDERDEC)
	{
		memcpy(Work.aszNumber[STD_OD_ASZNUMBER_INDEX], ItemSales->aszMnemonic, sizeof(ItemSales->aszMnemonic));
	}
	//End US Customs
	Work.ControlCode = ItemSales->ControlCode;      /* status */
    Work.uchPrintModNo = ItemSales->uchPrintModNo;                      /* No of print modifier */
    memcpy( &Work.auchPrintModifier[0], &ItemSales->auchPrintModifier[0], sizeof( ItemSales->auchPrintModifier ) );                       /* print modifier */
    Work.fsPrintStatus = ItemSales->fsPrintStatus;                      /* print status */
    Work.uchCondNo = ItemSales->uchCondNo;                              /* No of condiment */
    Work.uchChildNo = ItemSales->uchChildNo;                            /* no. of child PLU */
    Work.uchRate = ItemSales->uchRate;                                  /* rate */
    Work.lDiscountAmount = ItemSales->lDiscountAmount;                  /* discount amount */
    /* correct to save storage status for item transfer 06/25/96 */
    Work.fbStorageStatus = (UCHAR)(ItemSales->fbStorageStatus & NOT_ITEM_CONS);/* storage status */
    Work.uchPM = ItemSales->uchPM;                                      /* price multiple, 2172 */
    Work.usLinkNo = ItemSales->usLinkNo;                                /* link plu no. 2172 */
    Work.usFamilyCode = ItemSales->usFamilyCode;                        /* family code 2172 */
	Work.uchTableNumber = ItemSales->uchTableNumber;
	Work.uchDiscountSignifier = ItemSales->uchDiscountSignifier;
	Work.usFor = ItemSales->usFor;
	Work.usForQty = ItemSales->usForQty;

	Work.uchColorPalette = ItemSales->uchColorPaletteCode; // colorpalette-changes
	Work.uchHourlyOffset = ItemSales->uchHourlyOffset;     // Hourly total block offset
	Work.usTareInformation = ItemSales->usTareInformation; // tare information for specific PLU
	Work.blPrecedence = ItemSales->blPrecedence; // Precedence allowed?
	Work.usReasonCode = ItemSales->usReasonCode; // a reason code associated with this operator action.

	NHPOS_ASSERT((ItemSales->uchChildNo + ItemSales->uchCondNo + ItemSales->uchPrintModNo) <= sizeof(ItemSales->Condiment)/sizeof(ItemSales->Condiment[0]));

    for ( i = 0;
          i < (ItemSales->uchChildNo + ItemSales->uchCondNo + ItemSales->uchPrintModNo);
          i++ ) {     /* condiment */
        _tcsncpy(Work.Condiment[i].auchPLUNo, ItemSales->Condiment[i].auchPLUNo, NUM_PLU_LEN);
        Work.Condiment[i].usDeptNo = ItemSales->Condiment[i].usDeptNo;    /* dept No */
        Work.Condiment[i].lUnitPrice = ItemSales->Condiment[i].lUnitPrice;  /* price */
        Work.Condiment[i].uchAdjective = ItemSales->Condiment[i].uchAdjective;  /* adjective */
        Work.Condiment[i].uchPrintPriority = ItemSales->Condiment[i].uchPrintPriority;
        Work.Condiment[i].ControlCode = ItemSales->Condiment[i].ControlCode;     /* status */
		Work.Condiment[i].uchCondColorPaletteCode = ItemSales->Condiment[i].uchCondColorPaletteCode; /*color palette code*/
    }

	NHPOS_ASSERT((TRN_WORKBUFFER - sizeof(TRANSTORAGESALESNON) - 3) > 0);

	sCompressedSize = RflZip( uchBuffer + sizeof(TRANSTORAGESALESNON) + 3,
        TRN_WORKBUFFER - sizeof(TRANSTORAGESALESNON) - 3, &Work, sizeof( TRANSTORAGESALES ) );

	if(sCompressedSize == RFL_FAIL)
	{
		NHPOS_ASSERT_TEXT((sCompressedSize != RFL_FAIL), "TrnStoSalesComp(): RflZip() failed.");
		PifLog(MODULE_COMPRESS_DATA,LOG_EVENT_COMPRESS_SALES_DATA);
	}

    sSize1 = 2 + sCompressedSize;

	if(sSize1 < 173)
	{
		sSize1 = sSize1;
	}

    *(USHORT *)(uchBuffer + sizeof(TRANSTORAGESALESNON) + 1 ) = sSize1; /* compress size */

    return( sSize1 + sSize2 );                                          /* whole size */
}

/*==========================================================================
**    Synopsis: SHORT   TrnStoDisc( ITEMDISC *pItemDisc )
*
*    Input:     ITEMDISC    *pItemDisc
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   store discount data
==========================================================================*/

SHORT    TrnStoDisc( ITEMDISC *pItemDisc )
{
	SHORT       sSize;
	SHORT       sReturn = TRN_SUCCESS;
	UCHAR       auchBuffer[ TRN_WORKBUFFER ];

    if ( ! ( pItemDisc->fbStorageStatus & NOT_ITEM_STORAGE )) {

        sSize = TrnStoDiscComp( pItemDisc, auchBuffer );
        if (( sReturn = TrnChkStorageSizeIfSales( sSize )) != TRN_SUCCESS ) {
            return ( sReturn );
        }
        TrnStoItemNormal( auchBuffer, sSize );
    }

    if ( ! ( pItemDisc->fbStorageStatus & NOT_CONSOLI_STORAGE )) {
		ULONG       fulTrnPrtSts = TranCurQualPtr->flPrintStatus;
		ULONG       fusTrnCurSts = TranCurQualPtr->fsCurStatus;
		ULONG       fusTrnGCFSts = TranGCFQualPtr->fsGCFStatus;
		ITEMDISC    DiscWork = *pItemDisc;

        if (( fulTrnPrtSts & CURQUAL_POSTCHECK ) &&
            ( ! ( fusTrnGCFSts & GCFQUAL_PVOID )) &&
            ( fusTrnCurSts & CURQUAL_REORDER ) &&
            ( fusTrnCurSts & CURQUAL_PRESELECT_MASK ) &&
            ( pItemDisc->fbDiscModifier & VOID_MODIFIER )) {
            DiscWork.fbDiscModifier &= ~VOID_MODIFIER ;
        }

        sSize = TrnStoDiscComp( &DiscWork, auchBuffer );
        if (( sReturn = TrnChkStorageSizeIfSales( sSize )) != TRN_SUCCESS ) {
            return ( sReturn );
        }
        TrnStoConsNormal( auchBuffer, sSize );
    }
    return ( TRN_SUCCESS );
}

/*==========================================================================
**    Synopsis: SHORT   TrnStoDiscComp( ITEMDISC *ItemDisc, UCHAR *uchBuffer )
*
*    Input:     ITEMDISC    *ItemDisc, UCHAR    *uchBuffer
*    Output:    none
*    InOut:     UCHAR   *uchBuffer
*
*    Return:    compressed size
*
*    Description:   compress discount data                     R3.1
*                   compression buffer is made up of a non-compressed and a compressed section.
*                   each section has its own length.
*                    - uchBuffer[0] - contains length of non-compressed section
*                    - uchBuffer[1] - first byte of non-compressed section
*                    - uchBuffer[uchBuffer[0]] - first byte of two byte (USHORT) lenth of compressed section.
*                    - uchBuffer[uchBuffer[0] + 2] - first byte of compressed section
==========================================================================*/

SHORT   TrnStoDiscComp(ITEMDISC *ItemDisc, UCHAR *uchBuffer)
{
	TRANSTORAGEDISCNON  *pDiscNon = (TRANSTORAGEDISCNON  *)(uchBuffer + 1);
    SHORT   sSize2;
	SHORT   sNoCompSize, sCompressedSize;
    TRANSTORAGEDISC Work;

    /* --- calculate size of non-compress data --- */
    sNoCompSize = sizeof( UCHAR ) + sizeof( TRANSTORAGEDISCNON );
	NHPOS_ASSERT(sNoCompSize <= UCHAR_MAX);

    /* --- store size of non-compress data to destination buffer --- */
    *uchBuffer = (UCHAR) sNoCompSize;

	pDiscNon->uchMajorClass = ItemDisc->uchMajorClass;
    pDiscNon->uchMinorClass = ItemDisc->uchMinorClass;
    pDiscNon->uchSeatNo = ItemDisc->uchSeatNo;
    pDiscNon->fbReduceStatus = ItemDisc->fbReduceStatus;
    pDiscNon->lAmount = ItemDisc->lAmount;
    pDiscNon->lQTY = ItemDisc->lQTY;

    memcpy(&Work, &ItemDisc->auchPLUNo, sizeof(TRANSTORAGEDISC));
    /* memcpy(&Work, &ItemDisc->usPLUNo, sizeof(TRANSTORAGEDISC)); */
    Work.fbStorageStatus = 0;                                       /* storage status */

	NHPOS_ASSERT((TRN_WORKBUFFER - sNoCompSize - 2) > 0);

	sCompressedSize = RflZip((VOID *)(uchBuffer + sNoCompSize + 2),
        (USHORT)(TRN_WORKBUFFER - sNoCompSize - 2), (VOID *)&Work, (USHORT)sizeof(TRANSTORAGEDISC)); /* ### Mod (2171 for Win32) V1.0 */
   
	if(sCompressedSize == RFL_FAIL)
	{
		NHPOS_ASSERT_TEXT((sCompressedSize != RFL_FAIL), "TrnStoDiscComp(): RflZip() failed.");
		 PifLog(MODULE_COMPRESS_DATA,LOG_EVENT_COMPRESS_DISC_DATA);
	}

	sSize2 = 2 + sCompressedSize;

    *(USHORT *)(uchBuffer + sNoCompSize) = sSize2;                     /* compress size */

    return((SHORT)(sNoCompSize + sSize2));                             /* whole size - non-compressed and compressed */
}

/*==========================================================================
**    Synopsis: SHORT   TrnStoCoupon( ITEMCOUPON *pItemCoupon )
*
*    Input:     ITEMCOUPON  *pItemCoupon
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   store coupon data                          R3.1
==========================================================================*/

SHORT    TrnStoCoupon( ITEMCOUPON *pItemCoupon )
{
    ULONG       fulTrnPrtSts = TranCurQualPtr->flPrintStatus;
    ULONG       fusTrnGCFSts = TranGCFQualPtr->fsGCFStatus;
    ULONG       fusTrnCurSts = TranCurQualPtr->fsCurStatus;
    USHORT      fbVoidMod;         // same as fbModifier struct memember of ITEMCOUPON
    SHORT       sSize;
    SHORT       sReturn;
    SHORT       fPostChkGCOpe;
    SHORT       fStoreRecall;
    UCHAR       auchBuffer[ TRN_WORKBUFFER ];
    UCHAR       fbStorageSts;
    UCHAR       fbTransfer;        // same as fbReduceStatus struct member of ITEMCOUPON

	NHPOS_ASSERT(pItemCoupon->uchMajorClass == CLASS_ITEMCOUPON);

    fbVoidMod = ( pItemCoupon->fbModifier & VOID_MODIFIER );
    fbTransfer = ( UCHAR )( pItemCoupon->fbReduceStatus & REDUCE_NOT_HOURLY );
    fPostChkGCOpe = ( fulTrnPrtSts & CURQUAL_POSTCHECK ) && (( fusTrnCurSts & CURQUAL_OPEMASK ) != CURQUAL_CASHIER );
    fStoreRecall = (( fulTrnPrtSts & CURQUAL_GC_SYS_MASK ) == CURQUAL_STORE_RECALL );

    /* --- store compressed coupon data to each storages --- */
    fbStorageSts = pItemCoupon->fbStorageStatus;

    if ( ! ( fbStorageSts & NOT_ITEM_STORAGE )) {       /* item storage */

        /* --- compress coupon, and store compressed data to work buffer --- */
        sSize = TrnStoCouponComp( pItemCoupon, &auchBuffer[ 0 ] );

        /* --- are there a vacant area in item & consoli. storages ? --- */
        if (( sReturn = TrnChkStorageSizeIfSales( sSize )) != TRN_SUCCESS ) {
            return ( sReturn );
        }

        if ( ! ( fPostChkGCOpe || fStoreRecall )) {
			TRANSTORAGESALESNON TranDummy = {0};

            if ( fbVoidMod || fbTransfer ) {    /* R3.1 */
				ITEMCOUPON  VoidedCoupon;

                /* --- if void coupon is entered, increment void counter of existing coupon item --- */
				if (TRN_SUCCESS == TrnStoUpdateVoidCo( pItemCoupon, &VoidedCoupon, TRN_TYPE_ITEMSTORAGE )) {
					/* --- retrieve list linked sales item of voided coupon --- */
					memcpy( pItemCoupon->usItemOffset, VoidedCoupon.usItemOffset, sizeof(pItemCoupon->usItemOffset) );
				}
            }

            /* --- increment/decrement coupon co. of sales item --- */
            TrnStoUpdateCpnCo( pItemCoupon, TRN_TYPE_ITEMSTORAGE, &TranDummy);
        }
        TrnStoItemNormal( &auchBuffer[0], sSize );
    }

    if ( ! ( fbStorageSts & NOT_CONSOLI_STORAGE )) {    /* consoli. storage */
		ITEMCOUPON  CouponWork = *pItemCoupon;

        /* --- compress coupon, and store compressed data to work buffer --- */
        if (( fulTrnPrtSts & CURQUAL_POSTCHECK ) &&
            ( ! ( fusTrnGCFSts & GCFQUAL_PVOID )) &&
            ( fusTrnCurSts & CURQUAL_REORDER ) &&
            ( fusTrnCurSts & CURQUAL_PRESELECT_MASK ) &&
            fbVoidMod ) {
            CouponWork.fbModifier &= ~VOID_MODIFIER ;
            CouponWork.sVoidCo = 1;
            sSize = TrnStoCouponComp( &CouponWork, &auchBuffer[ 0 ] );
            CouponWork.fbModifier |= VOID_MODIFIER ;
        } else {
            sSize = TrnStoCouponComp( &CouponWork, &auchBuffer[ 0 ] );
        }

        if (( sReturn = TrnChkStorageSizeIfSales( sSize )) != TRN_SUCCESS ) {
            return ( sReturn );
        }

        if ( fPostChkGCOpe || fStoreRecall ) {
			TRANSTORAGESALESNON TranDummy = {0};

            if ( fbVoidMod || fbTransfer ) {    /* R3.1 */
				ITEMCOUPON  VoidedCoupon;

                /* --- if void coupon is entered, increment void counter of existing coupon item --- */
				if (TRN_SUCCESS == TrnStoUpdateVoidCo( &CouponWork, &VoidedCoupon, TRN_TYPE_CONSSTORAGE )) {
					/* --- retrieve list of linked sales item of voided coupon --- */
					memcpy (CouponWork.usItemOffset, VoidedCoupon.usItemOffset, sizeof(CouponWork.usItemOffset));
				}
            }

            /* --- update coupon status of target sales items --- */
            TrnStoUpdateCpnCo( &CouponWork, TRN_TYPE_CONSSTORAGE, &TranDummy );
        }
        TrnStoConsNormal( &auchBuffer[0], sSize );
    }

    /* --- update single type coupon status of GCF qualifier --- */
    TrnStoUpdateCouponType( pItemCoupon );

    return ( TRN_SUCCESS );
}

/*==========================================================================
**    Synopsis: SHORT   TrnStoCouponComp( ITEMCOUPON *pItemCoupon,
*                                         UCHAR      *puchBuffer )
*
*    Input:     ITEMCOUPON  *pItemCoupon,
*               UCHAR       *puchBuffer
*    Output:    none
*    InOut:     UCHAR       *puchBuffer
*
*    Return:    size of compressed data, in bytes
*
*    Description:   compress coupon data                 R3.1
*
*                   See function RflGetCouponItem() for the uncompression.
==========================================================================*/

SHORT    TrnStoCouponComp( ITEMCOUPON *pItemCoupon, UCHAR *puchBuffer )
{
    TRANSTORAGECOUPONNON    *pNoCompData;
    SHORT                   sCompressSize;
    SHORT                   sNoCompSize;
	TRANSTORAGECOUPON       Work = {0};

    /* --- calculate size of non-compress data --- */
    sNoCompSize = sizeof( UCHAR ) + sizeof( TRANSTORAGECOUPONNON );
	NHPOS_ASSERT(sNoCompSize <= UCHAR_MAX);

    /* --- store size of non-compress data to destination buffer --- */
    *puchBuffer = ( UCHAR )sNoCompSize;

    /* --- copy source non-compress data to destination buffer --- */
    pNoCompData = ( TRANSTORAGECOUPONNON * )( puchBuffer + sizeof( UCHAR ));

    pNoCompData->uchMajorClass = pItemCoupon->uchMajorClass;
    pNoCompData->uchMinorClass = pItemCoupon->uchMinorClass;
    pNoCompData->uchSeatNo     = pItemCoupon->uchSeatNo;
    pNoCompData->fbReduceStatus= pItemCoupon->fbReduceStatus;
    pNoCompData->uchCouponNo   = pItemCoupon->uchCouponNo;
    _tcsncpy(pNoCompData->auchUPCCouponNo, pItemCoupon->auchUPCCouponNo, NUM_PLU_LEN);
    pNoCompData->sVoidCo       = pItemCoupon->sVoidCo;
    memcpy(pNoCompData->usItemOffset, pItemCoupon->usItemOffset, sizeof(pNoCompData->usItemOffset));

    /* --- copy source data to work buffer, and then compress it --- */
    Work.lAmount         = pItemCoupon->lAmount;
    Work.lDoubleAmount   = pItemCoupon->lDoubleAmount;
    Work.lTripleAmount   = pItemCoupon->lTripleAmount;
    Work.fbModifier      = pItemCoupon->fbModifier;
    Work.fbStatus[ 0 ]   = pItemCoupon->fbStatus[ 0 ];
    Work.fbStatus[ 1 ]   = pItemCoupon->fbStatus[ 1 ];
    Work.fsPrintStatus   = pItemCoupon->fsPrintStatus;
    Work.uchNoOfItem     = pItemCoupon->uchNoOfItem;
    _tcsncpy(Work.aszMnemonic, pItemCoupon->aszMnemonic, STD_DEPTPLUNAME_LEN);
    _tcsncpy(Work.auchPLUNo, pItemCoupon->auchPLUNo, NUM_PLU_LEN);
    Work.usDeptNo        = pItemCoupon->usDeptNo;                           /* saratoga */
    Work.uchAdjective    = pItemCoupon->uchAdjective;                       /* saratoga */
    Work.fbStorageStatus = pItemCoupon->fbStorageStatus;
	Work.uchHourlyOffset = pItemCoupon->uchHourlyOffset;   // TrnStoCouponComp() Hourly Total bucket for when the coupon was rung up.
	Work.usBonusIndex	 = pItemCoupon->usBonusIndex;      // TrnStoCouponComp()  SR 157 Bonus Totals to 100 JHHJ

    _tcsncpy( Work.aszNumber, pItemCoupon->aszNumber, NUM_NUMBER );

    sCompressSize = RflZip((VOID *)( puchBuffer + sNoCompSize + sizeof( USHORT )),
                           (USHORT)( TRN_WORKBUFFER - sNoCompSize - sizeof( USHORT )),
                           (VOID *)&Work,
                           (USHORT)sizeof( TRANSTORAGECOUPON )); /* ### Mod (2171 for Win32) V1.0 */

	 if(sCompressSize == RFL_FAIL)
	 {
		NHPOS_ASSERT_TEXT((sCompressSize != RFL_FAIL), "TrnStoCouponComp(): RflZip() failed.");
		 PifLog(MODULE_COMPRESS_DATA,LOG_EVENT_COMPRESS_COUP_DATA);
	 }

    /* --- store size of compressed data to destination buffer --- */

    *(( USHORT * )( puchBuffer + sNoCompSize )) = sizeof( USHORT ) + sCompressSize;

    return ( sCompressSize + sNoCompSize + sizeof( USHORT ));
}

/*==========================================================================
**    Synopsis: SHORT   TrnStoTotal( ITEMTOTAL *ItemTotal )
*
*    Input:     ITEMTOTAL   *ItemTotal
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   compress and store total key data into the transaction file.
==========================================================================*/

SHORT    TrnStoTotal( ITEMTOTAL *pItemTotal )
{
    UCHAR   uchBuffer[TRN_WORKBUFFER];
    SHORT   sSize;
    SHORT   sReturn;

	NHPOS_ASSERT(pItemTotal->uchMajorClass == CLASS_ITEMTOTAL);

    // compress the item data into a transaction storage record and
	// check if item and/or consoli. storage is full.
	// if not full then store the transaction data record into the appropriate files.

    sSize = TrnStoTotalComp( pItemTotal, &uchBuffer[0] );        /* data compress */
    if ((sReturn = TrnChkStorageSize(sSize)) != TRN_SUCCESS) {
        return(sReturn);                            /* item or consoli. storage full */
    }

    if ( ( pItemTotal->fbStorageStatus & NOT_ITEM_STORAGE ) == 0 ) {
        TrnStoItemNormal( &uchBuffer[0], sSize );               /* store item storage */
    }
    if ( ( pItemTotal->fbStorageStatus & NOT_CONSOLI_STORAGE ) == 0 ) {
        TrnStoConsNormal( &uchBuffer[0], sSize );               /* store consoli storage */
    }

    return( TRN_SUCCESS );                                      /* success */
}

/*==========================================================================
**    Synopsis: SHORT   TrnStoTotalComp( ITEMTOTAL *ItemTotal, UCHAR *uchBuffer )
*
*    Input:     ITEMTOTAL   *ItemTotal, UCHAR   *uchBuffer
*    Output:    none
*    InOut:     UCHAR   *uchBuffer
*
*    Return:    compressed size
*
*    Description:   compress total key data
==========================================================================*/

SHORT    TrnStoTotalComp( ITEMTOTAL *ItemTotal, UCHAR *uchBuffer )
{
    UCHAR   uchSize1;
    SHORT   sSize2;
    TRANSTORAGETOTAL Work;
	SHORT sCompressedSize;

	NHPOS_ASSERT(sizeof(Work) == sizeof(ITEMTOTAL) - ((UCHAR *)(&ItemTotal->fbModifier) - (UCHAR *)ItemTotal));

    *(uchBuffer + 1 ) = ItemTotal->uchMajorClass;                       /* major class */
    uchSize1 = sizeof( ItemTotal->uchMajorClass) + 1;

    *(uchBuffer + uchSize1 ) = ItemTotal->uchMinorClass;                /* minor class */
    uchSize1 += sizeof( ItemTotal->uchMinorClass );

    *uchBuffer = uchSize1;                                              /* non-compress size */

	memcpy( &Work, &ItemTotal->fbModifier, sizeof( TRANSTORAGETOTAL ) );
    Work.fbStorageStatus = 0;                                           /* storage status */

	NHPOS_ASSERT((TRN_WORKBUFFER - uchSize1 - 2) > 0);

	sCompressedSize = RflZip( (VOID *)(uchBuffer + uchSize1 + 2), (USHORT)(TRN_WORKBUFFER - uchSize1 - 2),
        (VOID *)&Work, (USHORT)sizeof( TRANSTORAGETOTAL ) ); /* ### Mod (2171 for Win32) V1.0 */
	 
	if(sCompressedSize == RFL_FAIL)
	{
		NHPOS_ASSERT_TEXT((sCompressedSize != RFL_FAIL), "TrnStoTotalComp(): RflZip() failed.");
		 PifLog(MODULE_COMPRESS_DATA,LOG_EVENT_COMPRESS_TTL_DATA);
	}

    sSize2 = 2 + sCompressedSize;

    *(USHORT *)(uchBuffer + uchSize1) = sSize2;                         /* compress size */

    return( ( SHORT )( uchSize1 + sSize2 ) );                           /* whole size */
}

/*==========================================================================
**    Synopsis: SHORT   TrnStoTender( ITEMTENDER *ItemTender )
*
*    Input:     ITEMTENDER  *ItemTender
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   compress and store tender data into the transaction file.
==========================================================================*/

SHORT    TrnStoTender( ITEMTENDER *pItemTender )
{
    UCHAR   uchBuffer[TRN_WORKBUFFER];
    SHORT   sSize;
    SHORT   sReturn;

	NHPOS_ASSERT(pItemTender->uchMajorClass == CLASS_ITEMTENDER);

    // compress the item data into a transaction storage record and
	// check if item and/or consoli. storage is full.
	// if not full then store the transaction data record into the appropriate files.

    sSize = TrnStoTenderComp( pItemTender, &uchBuffer[0] );          /* data compress */
    if ((sReturn = TrnChkStorageSize(sSize)) != TRN_SUCCESS) {
        return(sReturn);                            /* item or consoli. storage full */
    }

    if ( ( pItemTender->fbStorageStatus & NOT_ITEM_STORAGE ) == 0 ) {
        TrnStoItemNormal( &uchBuffer[0], sSize );                   /* store item storage    */
    }
    if ( ( pItemTender->fbStorageStatus & NOT_CONSOLI_STORAGE ) == 0 ) {
        TrnStoConsNormal( &uchBuffer[0], sSize );                   /* store consoli storage    */
    }

    return( TRN_SUCCESS );                                          /* success */
}

/*==========================================================================
**    Synopsis: SHORT   TrnStoTenderComp( ITEMTENDER *ItemTender, UCHAR *uchBuffer )
*
*    Input:     ITEMTENDER  *ItemTender, UCHAR  *uchBuffer
*    Output:    none
*    InOut:     UCHAR   *uchBuffer
*
*    Return:    compressed size
*
*    Description:   compress tender data
==========================================================================*/

SHORT    TrnStoTenderComp( ITEMTENDER *ItemTender, UCHAR *uchBuffer )
{
    SHORT   sSize1;
    SHORT   sSize2;
	SHORT   sCompressedSize;
    TRANSTORAGETENDER Work;

	NHPOS_ASSERT(sizeof(Work) == sizeof(ITEMTENDER) - ((UCHAR *)(&ItemTender->fbModifier) - (UCHAR *)ItemTender));

	sSize1 = 1;                                               // first byte contains size of uncompressed part
    *(uchBuffer + sSize1 ) = ItemTender->uchMajorClass;       // set the major class
    sSize1 += sizeof( ItemTender->uchMajorClass);

    *(uchBuffer + sSize1 ) = ItemTender->uchMinorClass;       // set the minor class
    sSize1 += sizeof( ItemTender->uchMinorClass);

	*(uchBuffer + sSize1 ) = ItemTender->fbReduceStatus;      // set the reduce status flags.
    sSize1 += sizeof( ItemTender->fbReduceStatus);

	NHPOS_ASSERT(sSize1 <= UCHAR_MAX);
	*uchBuffer = (UCHAR)sSize1;                               // set the non-compress part size

    memcpy( &Work, &ItemTender->fbModifier, sizeof( TRANSTORAGETENDER ) );
    Work.fbStorageStatus = 0;                                           /* storage status */

	NHPOS_ASSERT((TRN_WORKBUFFER - sSize1 - 2) > 0);

	sCompressedSize = RflZip( (VOID *)(uchBuffer + sSize1 + 2), (USHORT)(TRN_WORKBUFFER - sSize1 - 2),
        (VOID *)&Work, (USHORT)sizeof( TRANSTORAGETENDER ) ); /* ### Mod (2171 for Win32) V1.0 */

	if(sCompressedSize == RFL_FAIL)
	{
		NHPOS_ASSERT_TEXT((sCompressedSize != RFL_FAIL), "TrnStoTenderComp(): RflZip() failed.");
		PifLog(MODULE_COMPRESS_DATA,LOG_EVENT_COMPRESS_TEND_DATA);
	}
    sSize2 = 2 + sCompressedSize;

    *(USHORT *)(uchBuffer + sSize1 ) = sSize2;                // set the compressed part size

    return( sSize1 + sSize2 );                   // return the whole size, non-compressed and compressed
}

/*==========================================================================
**    Synopsis: SHORT   TrnStoMisc( ITEMMISC *ItemMisc )
*
*    Input:     ITEMMISC    *ItemMisc
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   compress and store misc. transaction data into the transaction file.
==========================================================================*/

SHORT    TrnStoMisc( ITEMMISC *pItemMisc )
{
    UCHAR   uchBuffer[TRN_WORKBUFFER];
    SHORT   sSize;
    SHORT   sReturn;

	NHPOS_ASSERT(pItemMisc->uchMajorClass == CLASS_ITEMMISC);

    // compress the item data into a transaction storage record and
	// check if item and/or consoli. storage is full.
	// if not full then store the transaction data record into the appropriate files.

    sSize = TrnStoMiscComp( pItemMisc, &uchBuffer[0] );
    if ((sReturn = TrnChkStorageSize(sSize)) != TRN_SUCCESS) {
        return(sReturn);
    }

    if ( ( pItemMisc->fbStorageStatus & NOT_ITEM_STORAGE ) == 0 ) {
        TrnStoItemNormal( &uchBuffer[0], sSize );                  /* store item storage    */
    }
    if ( ( pItemMisc->fbStorageStatus & NOT_CONSOLI_STORAGE ) == 0 ) {
        TrnStoConsNormal( &uchBuffer[0], sSize );                  /* store consoli storage    */
    }

    return( TRN_SUCCESS );                                         /* success */
}

/*==========================================================================
**    Synopsis: SHORT   TrnStoMiscComp( ITEMMISC *ItemMisc, UCHAR *uchBuffer )
*
*    Input:     ITEMMISC    *ItemMisc, UCHAR    *uchBuffer
*    Output:    none
*    InOut:     UCHAR   *uchBuffer
*
*    Return:    compressed size
*
*    Description:   compress misc. transaction data
==========================================================================*/

SHORT    TrnStoMiscComp( ITEMMISC *ItemMisc, UCHAR *uchBuffer )
{
    UCHAR   uchSize1;
    SHORT   sSize2;
    TRANSTORAGEMISC Work;
	SHORT sCompressedSize;

	NHPOS_ASSERT(sizeof(TRANSTORAGEMISC) == sizeof(ITEMMISC) - sizeof(ItemMisc->uchMajorClass) - sizeof(ItemMisc->uchMinorClass));

    *(uchBuffer + 1 ) = ItemMisc->uchMajorClass;                        /* major class */
    uchSize1 = sizeof( ItemMisc->uchMajorClass) + 1;

    *(uchBuffer + uchSize1 ) = ItemMisc->uchMinorClass;                 /* minor class */
    uchSize1 += sizeof( ItemMisc->uchMinorClass);

    *uchBuffer = uchSize1;                                              /* non-compress size */

    memcpy( &Work, &ItemMisc->ulID, sizeof( TRANSTORAGEMISC ) );
    Work.fbStorageStatus = 0;                                           /* storage status */

	NHPOS_ASSERT((TRN_WORKBUFFER - uchSize1 - 2) > 0);

	sCompressedSize = RflZip( (VOID *)(uchBuffer + uchSize1 + 2), (USHORT)(TRN_WORKBUFFER - uchSize1 - 2),
        (VOID *)&Work, (USHORT)sizeof( TRANSTORAGEMISC ) ); /* ### Mod (2171 for Win32) V1.0 */

	if(sCompressedSize == RFL_FAIL)
	{
		NHPOS_ASSERT_TEXT((sCompressedSize != RFL_FAIL), "TrnStoMiscComp(): RflZip() failed.");
		 PifLog(MODULE_COMPRESS_DATA,LOG_EVENT_COMPRESS_MISC_DATA);
	}

    sSize2 = 2 + sCompressedSize;

    *(USHORT *)(uchBuffer + uchSize1 ) = sSize2;                        /* compress size */

    return( ( SHORT )( uchSize1 + sSize2 ) );                           /* whole size */
}

/*==========================================================================
**    Synopsis: SHORT   TrnStoOther(ITEMMISC *ItemMisc)
*
*    Input:
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   compress and store other transaction data into the transaction file.
==========================================================================*/

SHORT    TrnStoOther(ITEMOTHER *pItemOther)
{
    UCHAR   uchBuffer[TRN_WORKBUFFER];
    SHORT   sSize, sReturn;

	NHPOS_ASSERT(pItemOther->uchMajorClass == CLASS_ITEMOTHER);

    // compress the item data into a transaction storage record and
	// check if item and/or consoli. storage is full.
	// if not full then store the transaction data record into the appropriate files.

    sSize = TrnStoOtherComp(pItemOther, uchBuffer);       /* data compress */
    if ((sReturn = TrnChkStorageSize(sSize)) != TRN_SUCCESS) {
        return(sReturn);
    }

    TrnStoItemNormal(uchBuffer, sSize);
    TrnStoConsNormal(uchBuffer, sSize);

    return(TRN_SUCCESS);
}

/*==========================================================================
**    Synopsis: SHORT   TrnStoOtherComp(ITEMOTHER *ItemOther, UCHAR *uchBuffer)
*
*    Input:
*    Output:
*    InOut:
*
*    Return:
*
*    Description:   compress other transaction data
==========================================================================*/

SHORT    TrnStoOtherComp(ITEMOTHER *ItemOther, UCHAR *uchBuffer)
{
    UCHAR               uchSize1;
    SHORT               sSize2;
    TRANSTORAGEOTHER    Work;
	SHORT sCompressedSize;

	NHPOS_ASSERT(sizeof(TRANSTORAGEOTHER) == sizeof(ITEMOTHER) - sizeof(ItemOther->uchMajorClass) - sizeof(ItemOther->uchMinorClass));

    *(uchBuffer + 1) = ItemOther->uchMajorClass;
    uchSize1 = sizeof(ItemOther->uchMajorClass) + 1;

    *(uchBuffer + uchSize1) = ItemOther->uchMinorClass;
    uchSize1 += sizeof(ItemOther->uchMinorClass);

    *uchBuffer = uchSize1;
    memcpy(&Work, &ItemOther->lAmount, sizeof(TRANSTORAGEOTHER));

	NHPOS_ASSERT((TRN_WORKBUFFER - uchSize1 - 2) > 0);

	sCompressedSize = RflZip((VOID *)(uchBuffer + uchSize1 + 2),
        (USHORT)(TRN_WORKBUFFER - uchSize1 - 2), (VOID *)&Work, (USHORT)sizeof(TRANSTORAGEOTHER)); /* ### Mod (2171 for Win32) V1.0 */
	 
	if(sCompressedSize == RFL_FAIL)
	 {
		NHPOS_ASSERT_TEXT((sCompressedSize != RFL_FAIL), "TrnStoOtherComp(): RflZip() failed.");
		 PifLog(MODULE_COMPRESS_DATA,LOG_EVENT_COMPRESS_OTHR_DATA);
	 }

    sSize2 = 2 + sCompressedSize;
    
	
	*(USHORT *)(uchBuffer + uchSize1 ) = sSize2;

    return((SHORT)(uchSize1 + sSize2));
}

/*==========================================================================
**    Synopsis: SHORT   TrnStoAffect( ITEMAFFECTION *itemAffection )
*
*    Input:     ITEMAFFECTION   *ItemAffection
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   compress and store affection data into the transaction file.
==========================================================================*/

SHORT    TrnStoAffect( ITEMAFFECTION *pItemAffection )
{
    UCHAR   uchBuffer[TRN_WORKBUFFER];
    SHORT   sSize;
    SHORT   sReturn;

	NHPOS_ASSERT(pItemAffection->uchMajorClass == CLASS_ITEMAFFECTION);

    // compress the item data into a transaction storage record and
	// check if item and/or consoli. storage is full.
	// if not full then store the transaction data record into the appropriate files.

    sSize = TrnStoAffectComp( pItemAffection, &uchBuffer[0] );       /* data compress */
    if ((sReturn = TrnChkStorageSize(sSize)) != TRN_SUCCESS) {
        return(sReturn);                            /* item or consoli. storage full */
    }

    if ( ( pItemAffection->fbStorageStatus & NOT_ITEM_STORAGE ) == 0 ) {
        TrnStoItemNormal( &uchBuffer[0], sSize );                   /* store item storage    */
    }
    if ( ( pItemAffection->fbStorageStatus & NOT_CONSOLI_STORAGE ) == 0 ) {
        TrnStoConsNormal( &uchBuffer[0], sSize );                   /* store consoli storage    */
    }

    return( TRN_SUCCESS );                                          /* success */
}

/*==========================================================================
**    Synopsis: SHORT   TrnStoAffectComp( ITEMAFFECTION *itemAffection,
                            UCHAR *uchBuffer )
*    Input:     ITEMAFFECTION   *ItemAffection, UCHAR   *uchBuffer
*    Output:    none
*    InOut:     UCHAR   *uchBuffer
*
*    Return:    compressed size
*
*    Description:   compress affection data
==========================================================================*/

SHORT    TrnStoAffectComp( ITEMAFFECTION *ItemAffection, UCHAR *uchBuffer )
{
    UCHAR   uchSize1;
    SHORT   sSize2;
    TRANSTORAGEAFFECTION Work;
	SHORT sCompressedSize;

	NHPOS_ASSERT(sizeof(TRANSTORAGEAFFECTION) == sizeof(ITEMAFFECTION) - sizeof(ItemAffection->uchMajorClass) - sizeof(ItemAffection->uchMinorClass));

    *(uchBuffer + 1 ) = ItemAffection->uchMajorClass;                   /* major class */
    uchSize1 = sizeof( ItemAffection->uchMajorClass) + 1;

    *(uchBuffer + uchSize1 ) = ItemAffection->uchMinorClass;            /* minor class */
    uchSize1 += sizeof( ItemAffection->uchMinorClass);

    *uchBuffer = uchSize1;                                              /* non-compress size */

    memcpy( &Work, &ItemAffection->lAmount, sizeof( TRANSTORAGEAFFECTION ) );
    Work.fbStorageStatus = 0;                                           /* storage status */

	NHPOS_ASSERT((TRN_WORKBUFFER - uchSize1 - 2) > 0);

	sCompressedSize = RflZip( (VOID *)(uchBuffer + uchSize1 + 2), (USHORT)(TRN_WORKBUFFER - uchSize1 - 2),
        (VOID *)&Work, (USHORT)sizeof( TRANSTORAGEAFFECTION ) ); /* ### Mod (2171 for Win32) V1.0 */

	if(sCompressedSize == RFL_FAIL)
	{
		NHPOS_ASSERT_TEXT((sCompressedSize != RFL_FAIL), "TrnStoAffectComp(): RflZip() failed.");
		PifLog(MODULE_COMPRESS_DATA,LOG_EVENT_COMPRESS_AFFCT_DATA);
	}

    sSize2 = 2 + sCompressedSize;

    *(USHORT *)(uchBuffer + uchSize1) = sSize2;                         /* compress size */

    return( ( SHORT )( uchSize1 + sSize2 ) );                           /* whole size */
}

/*==========================================================================
**    Synopsis: SHORT   TrnStoPrint( ITEMPRINT *ItemPrint )
*
*    Input:     ITEMPRINT   *ItemPrint
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   compress and store print data into the transaction file.
==========================================================================*/

SHORT    TrnStoPrint( ITEMPRINT *pItemPrint )
{
    UCHAR   uchBuffer[TRN_WORKBUFFER];
    SHORT   sSize;
    SHORT   sReturn;

	NHPOS_ASSERT(pItemPrint->uchMajorClass == CLASS_ITEMPRINT);

    // compress the item data into a transaction storage record and
	// check if item and/or consoli. storage is full.
	// if not full then store the transaction data record into the appropriate files.

    sSize = TrnStoPrintComp( pItemPrint, &uchBuffer[0] );            /* data compress */
    if ((sReturn = TrnChkStorageSize(sSize)) != TRN_SUCCESS) {
        return(sReturn);                            /* item or consoli. storage full */
    }

    if ( ( pItemPrint->fbStorageStatus & NOT_ITEM_STORAGE ) == 0 ) {
        TrnStoItemNormal( &uchBuffer[0], sSize );                   /* store item storage    */
    }
    if ( ( pItemPrint->fbStorageStatus & NOT_CONSOLI_STORAGE ) == 0 ) {
        TrnStoConsNormal( &uchBuffer[0], sSize );                   /* store consoli storage    */
    }

    return( TRN_SUCCESS );                                          /* success */
}

/*==========================================================================
**    Synopsis: SHORT   TrnStoPrintComp( ITEMPRINT *ItemPrint, UCHAR *uchBuffer )
*
*    Input:     ITEMPRINT   *ItemPrint, UCHAR   *uchBuffer
*    Output:    none
*    InOut:     UCHAR   *uchBuffer
*
*    Return:    compressed size
*
*    Description:   compress print data
==========================================================================*/

SHORT    TrnStoPrintComp( ITEMPRINT *ItemPrint, UCHAR *uchBuffer )
{
    UCHAR   uchSize1;
    SHORT   sSize2;
    TRANSTORAGEPRINT Work;
	SHORT sCompressedSize;

	NHPOS_ASSERT(sizeof(TRANSTORAGEPRINT) == sizeof(ITEMPRINT) - sizeof(ItemPrint->uchMajorClass) - sizeof(ItemPrint->uchMinorClass));

    *(uchBuffer + 1 ) = ItemPrint->uchMajorClass;                   /* major class */
    uchSize1 = sizeof( ItemPrint->uchMajorClass) + 1;

    *(uchBuffer + uchSize1 ) = ItemPrint->uchMinorClass;            /* minor class */
    uchSize1 += sizeof( ItemPrint->uchMinorClass);

    *uchBuffer = uchSize1;                                          /* non-compress size */

    memcpy( &Work, &ItemPrint->ulID, sizeof( TRANSTORAGEPRINT ) );
    Work.fbStorageStatus = 0;                                           /* storage status */

	NHPOS_ASSERT((TRN_WORKBUFFER - uchSize1 - 2) > 0);

	sCompressedSize = RflZip( (VOID *)(uchBuffer + uchSize1 + 2), (USHORT)(TRN_WORKBUFFER - uchSize1 - 2),
        (VOID *)&Work, (USHORT)sizeof( TRANSTORAGEPRINT ) ); /* ### Mod (2171 for Win32) V1.0 */

	if(sCompressedSize == RFL_FAIL)
	 {
		NHPOS_ASSERT_TEXT((sCompressedSize != RFL_FAIL), "TrnStoPrintComp(): RflZip() failed.");
		 PifLog(MODULE_COMPRESS_DATA,LOG_EVENT_COMPRESS_PRNT_DATA);
	 }
    sSize2 = 2 + sCompressedSize;

    *(USHORT *)(uchBuffer + uchSize1) = sSize2;                     /* compress size */

    return( ( SHORT )( uchSize1 + sSize2 ) );                       /* whole size */
}

/*==========================================================================
**    Synopsis: SHORT   TrnStoMulti( ITEMMULTI *ItemMulti )
*
*    Input:     ITEMMULTI   *ItemMulti
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   compress and store multi check payment print data
==========================================================================*/

SHORT    TrnStoMulti( ITEMMULTI *pItemMulti )
{
    UCHAR   uchBuffer[TRN_WORKBUFFER];
    SHORT   sSize;
    SHORT   sReturn;

	NHPOS_ASSERT(pItemMulti->uchMajorClass == CLASS_ITEMMULTI);

    // compress the item data into a transaction storage record and
	// check if item and/or consoli. storage is full.
	// if not full then store the transaction data record into the appropriate files.

    sSize = TrnStoMultiComp( pItemMulti, &uchBuffer[0] );           /* data compress */
    if ((sReturn = TrnChkStorageSize(sSize)) != TRN_SUCCESS) {
        return(sReturn);                                            /* item or consoli. storage full */
    }

    if ( ( pItemMulti->fbStorageStatus & NOT_ITEM_STORAGE ) == 0 ) {
        TrnStoItemNormal( &uchBuffer[0], sSize );                   /* store item storage    */
    }
    if ( ( pItemMulti->fbStorageStatus & NOT_CONSOLI_STORAGE ) == 0 ) {
        TrnStoConsNormal( &uchBuffer[0], sSize );                   /* store consoli storage    */
    }

    return( TRN_SUCCESS );                                          /* success */
}

/*==========================================================================
**    Synopsis: SHORT   TrnStoMultiComp( ITEMMULTI *ItemMulti,
                        UCHAR *uchBuffer )
*
*    Input:     ITEMMULTI   *ItemMulti, UCHAR   *uchBuffer
*    Output:    none
*    InOut:     UCHAR   *uchBuffer
*
*    Return:    compressed size
*
*    Description:   compress multi check payment print data
==========================================================================*/

SHORT    TrnStoMultiComp( ITEMMULTI *ItemMulti, UCHAR *uchBuffer )
{
    UCHAR   uchSize1;
    SHORT   sSize2;
    TRANSTORAGEMULTICHECK   Work;
	SHORT sCompressedSize;

	NHPOS_ASSERT(sizeof(TRANSTORAGEMULTICHECK) == sizeof(ITEMMULTI) - sizeof(ItemMulti->uchMajorClass) - sizeof(ItemMulti->uchMinorClass));

	*(uchBuffer + 1 ) = ItemMulti->uchMajorClass;                   /* major class */
    uchSize1 = sizeof( ItemMulti->uchMajorClass) + 1;

    *(uchBuffer + uchSize1 ) = ItemMulti->uchMinorClass;            /* minor class */
    uchSize1 += sizeof( ItemMulti->uchMinorClass);

    *uchBuffer = uchSize1;                                          /* non-compress size */

    memcpy( &Work, &ItemMulti->lService[0], sizeof( TRANSTORAGEMULTICHECK ) );
    Work.fbStorageStatus = 0;                                       /* storage status */

	NHPOS_ASSERT((TRN_WORKBUFFER - uchSize1 - 2) > 0);

	sCompressedSize = RflZip( (VOID *)(uchBuffer + uchSize1 + 2), (USHORT)(TRN_WORKBUFFER - uchSize1 - 2),
        (VOID *)&Work, (USHORT)sizeof( TRANSTORAGEMULTICHECK ) ); /* ### Mod (2171 for Win32) V1.0 */

	if(sCompressedSize == RFL_FAIL)
	{
		NHPOS_ASSERT_TEXT((sCompressedSize != RFL_FAIL), "TrnStoMultiComp(): RflZip() failed.");
		 PifLog(MODULE_COMPRESS_DATA,LOG_EVENT_COMPRESS_MLTI_DATA);
	}

    sSize2 = 2 + sCompressedSize;

    *(USHORT *)(uchBuffer + uchSize1) = sSize2;                     /* compress size */

    return( ( SHORT )( uchSize1 + sSize2 ) );                       /* whole size */
}

/*==========================================================================
**    Synopsis: SHORT   TrnStoOpeClose( ITEMOPECLOSE *ItemOpeClose )
*
*    Input:     ITEMOPECLOSE    *ItemOpeClose
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*
*    Description:   compress and store operator close data into the transaction file.
==========================================================================*/

SHORT    TrnStoOpeClose( ITEMOPECLOSE *pItemOpeClose )
{
    UCHAR   uchBuffer[TRN_WORKBUFFER];
    SHORT   sSize;

	NHPOS_ASSERT(pItemOpeClose->uchMajorClass == CLASS_ITEMOPECLOSE);

    // compress the item data into a transaction storage record and
	// check if item and/or consoli. storage is full.
	// if not full then store the transaction data record into the appropriate files.

    sSize = TrnStoOpeCloseComp( pItemOpeClose, &uchBuffer[0] );     /* data compress */

    TrnStoItemNormal( &uchBuffer[0], sSize );                       /* store item storage    */

    return( TRN_SUCCESS );                                          /* success */
}

/*==========================================================================
**    Synopsis: SHORT   TrnStoOpeCloseComp( ITEMOPECLOSE *ItemOpeClose,
*                           UCHAR *uchBuffer )
*
*    Input:     ITEMOPECLOSE    *ItemOpeClose, UCHAR    *uchBuffer
*    Output:    none
*    InOut:     UCHAR   *uchBuffer
*
*    Return:    compressed size
*
*    Description:   compress operator close data
==========================================================================*/

SHORT    TrnStoOpeCloseComp( ITEMOPECLOSE *ItemOpeClose, UCHAR *uchBuffer )
{
    UCHAR   uchSize1;
    SHORT   sSize2;
    TRANSTORAGEOPECLOSE     Work;
	SHORT sCompressedSize;

    *(uchBuffer + 1 ) = ItemOpeClose->uchMajorClass;                /* major class */
    uchSize1 = sizeof( ItemOpeClose->uchMajorClass) + 1;

    *(uchBuffer + uchSize1 ) = ItemOpeClose->uchMinorClass;         /* minor class */
    uchSize1 += sizeof( ItemOpeClose->uchMinorClass);

    *uchBuffer = uchSize1;                                          /* non-compress size */

    memcpy( &Work, &ItemOpeClose->ulID, sizeof( TRANSTORAGEOPECLOSE ) );

	NHPOS_ASSERT((TRN_WORKBUFFER - uchSize1 - 2) > 0);

	sCompressedSize = RflZip( (VOID *)(uchBuffer + uchSize1 + 2), (USHORT)(TRN_WORKBUFFER - uchSize1 - 2),
        (VOID *)&Work, (USHORT)sizeof( TRANSTORAGEOPECLOSE ) ); /* ### Mod (2171 for Win32) V1.0 */

	if(sCompressedSize == RFL_FAIL)
	{
		NHPOS_ASSERT_TEXT((sCompressedSize != RFL_FAIL), "TrnStoOpeCloseComp(): RflZip() failed.");
		 PifLog(MODULE_COMPRESS_DATA,LOG_EVENT_COMPRESS_CLOSE_DATA);
	}

    sSize2 = 2 + sCompressedSize;

    *(USHORT *)(uchBuffer + uchSize1) = sSize2;                     /* compress size */

    return( ( SHORT )( uchSize1 + sSize2 ) );                       /* whole size */
}

#if defined(POSSIBLE_DEAD_CODE)
// This function to store a Loan or Pickup into the transaction data file is not used
// because Loan and/or Pickup (AC10 or AC11) are used from Supervisor Mode hence not put
// into a transaction or a Pickup is used from Operator Mode however it is not part of a
// transaction either.
// The transaction file with the item compressed and stored into the transaction file is
// only used for items (sales items, affectation, discounts, total/tender, etc.) that are
// part of a normal transaction. Loan and Pickup is not part of a normal transaction.
//    Richard Chambers, Oct-05-2016

/*
*==========================================================================
**    Synopsis: SHORT   TrnStoLoanPick(VOID *pItem)
*
*    Input:     MAINTLOANPICKUP     *pItem
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   store loan/pick-up data
*==========================================================================
*/
SHORT   TrnStoLoanPick(VOID *pLP)
{
    UCHAR   uchBuffer[TRN_WORKBUFFER];
    SHORT   sSize;
    SHORT   sReturn;
    MAINTLOANPICKUP *pItem;
    TRANSTORAGELOANPICK LP;

    pItem = (MAINTLOANPICKUP *)pLP;

    memset(&LP, 0, sizeof(LP));
    LP.lAmount      = pItem->lAmount;
    LP.lForQty      = pItem->lForQty;
    LP.lNativeAmount= pItem->lNativeAmount;
    LP.lTotal       = pItem->lTotal;
    LP.lUnitAmount  = pItem->lUnitAmount;
    LP.uchFCMDC     = pItem->uchFCMDC;
    LP.uchFCMDC2    = pItem->uchFCMDC2;
    LP.ulCashierNo  = pItem->ulCashierNo;
    LP.usModifier   = pItem->usModifier;
    sSize = sizeof(TRANSTORAGELOANPICK);

    if ((sReturn = TrnChkStorageSize(sSize)) != TRN_SUCCESS) {
        return(sReturn);
    }
    TrnStoItemNormal(uchBuffer, sSize);
    TrnStoConsNormal(uchBuffer, sSize);

    return(TRN_SUCCESS);
}
#endif

/*==========================================================================
**   Synopsis:  SHORT   TrnStorage( VOID *TrnStorage )
*
*    Input:     VOID    *TrnStorage
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   transaction storage                        R3.1
==========================================================================*/

SHORT    TrnStorage(VOID *TrnStorage)
{
#if defined(_DEBUG) || defined(DEBUG)
	// some basic checks for item data and transaction data storage record consistency.
	// if any of these fail then we may have a problem with change made to one that was not made to another.
	NHPOS_ASSERT(sizeof(TRANSTORAGETRANSOPENNON) + sizeof(TRANSTORAGETRANSOPEN) == sizeof(ITEMTRANSOPEN));
	NHPOS_ASSERT(sizeof(TRANSTORAGECOUPONNON) + sizeof(TRANSTORAGECOUPON) == sizeof(ITEMCOUPON));
	NHPOS_ASSERT(sizeof(TRANSTORAGETRANSMININON) + sizeof(TRANSTORAGETOTAL) == sizeof(ITEMTOTAL));
	NHPOS_ASSERT(sizeof(TRANSTORAGETENDERNON) + sizeof(TRANSTORAGETENDER) == sizeof(ITEMTENDER));
	NHPOS_ASSERT(sizeof(TRANSTORAGETRANSMININON) + sizeof(TRANSTORAGEMISC) == sizeof(ITEMMISC));
	NHPOS_ASSERT(sizeof(TRANSTORAGETRANSMININON) + sizeof(TRANSTORAGEOTHER) == sizeof(ITEMOTHER));
	NHPOS_ASSERT(sizeof(TRANSTORAGETRANSMININON) + sizeof(TRANSTORAGEAFFECTION) == sizeof(ITEMAFFECTION));
	NHPOS_ASSERT(sizeof(TRANSTORAGETRANSMININON) + sizeof(TRANSTORAGEPRINT) == sizeof(ITEMPRINT));
	NHPOS_ASSERT(sizeof(TRANSTORAGETRANSMININON) + sizeof(TRANSTORAGEMULTICHECK) == sizeof(ITEMMULTI));
	NHPOS_ASSERT(sizeof(TRANSTORAGETRANSMININON) + sizeof(TRANSTORAGEOPECLOSE) == sizeof(ITEMOPECLOSE));
#endif

    switch(ITEMSTORAGENONPTR(TrnStorage)->uchMajorClass) {
    case CLASS_ITEMTRANSOPEN:
        return(TrnStoOpen(TrnStorage));

    case CLASS_ITEMSALES:
        return(TrnStoSales(TrnStorage));

    case CLASS_ITEMDISC:
        return(TrnStoDisc(TrnStorage));

    case CLASS_ITEMCOUPON:
        return(TrnStoCoupon(TrnStorage));

    case CLASS_ITEMTOTAL:
        return(TrnStoTotal(TrnStorage));

    case CLASS_ITEMTENDER:
        return(TrnStoTender(TrnStorage));

    case CLASS_ITEMMISC:
        return(TrnStoMisc(TrnStorage));

    case CLASS_ITEMOTHER:
        return(TrnStoOther(TrnStorage));

    case CLASS_ITEMAFFECTION:
        return(TrnStoAffect(TrnStorage));

    case CLASS_ITEMPRINT:
        return(TrnStoPrint(TrnStorage));

    case CLASS_ITEMMULTI:
        return(TrnStoMulti(TrnStorage));

    case CLASS_ITEMOPECLOSE:
        return(TrnStoOpeClose(TrnStorage));

//    case CLASS_MAINTPICKUP:                 /* Saratoga */
//        return(TrnStoLoanPick(TrnStorage));

    default:
		{
			char xBuff[128];
			sprintf (xBuff, "TrnStorage(): uchMajorClass = %d, uchMinorClass = %d", ITEMSTORAGENONPTR(TrnStorage)->uchMajorClass, ITEMSTORAGENONPTR(TrnStorage)->uchMinorClass);
			NHPOS_ASSERT_TEXT(!"TrnStorage->uchMajorClass error", xBuff);
		}
        return(TRN_ERROR);
    }
}

// -------   End for TrnStorage() functionality    --------------------------
// --------------------------------------------------------------------------

/*==========================================================================
**  Synopsis:   VOID TrnStoItemNormal( VOID *pData, SHORT sSize )
*
*   Input:      VOID  *pData    - address of item data to write file
*               SHORT sSize     - size of source data in bytes
*   Output:     none
*   InOut:      none
*
*   Return:     none
*
*   Description:    write "item" to item storage
*
*                 In this function we are manipulating the global status
*                 variable TrnStoReplaceInsertAppendIndexStatus so that the
*                 caller of this function can use the status retrieval function
*                 TrnStoReplaceStatusIndex() to check to see if this function
*                 replaced an index item in the index file for TRN_TYPE_ITEMSTORAGE.
*                 If the index item is replaced then it may mean that there
*                 is an item in the associated storage file that no longer is
*                 pointed to by an index entry in the index file for TRN_TYPE_ITEMSTORAGE.
*                 If this is the case, the caller may want to put a copy of a valid
*                 index entry from file TRN_TYPE_NOITEMSTORAGE so that the item
*                 data will be available when displaying the consolidated data.
==========================================================================*/
TrnVliOffset TrnStoItemNormalUpdateIndex( TRANSTORAGESALESNON *pTranNon, TrnVliOffset usItemOffset )
{
	SHORT                sStatusTemp;

    if ( TrnInformation.TranCurQual.flPrintStatus & CURQUAL_PRINT_PRIORITY ) {
        if ( pTranNon->uchMajorClass == CLASS_ITEMSALES ) {
			/* --- if source item is sales item, analyze it's print priority
				and store it at analyzed position --- */
			TrnStoReplaceInsertIndex( pTranNon, usItemOffset, TRN_TYPE_ITEMSTORAGE );
			sStatusTemp = TrnStoReplaceInsertAppendIndexStatus;
			TrnStoReplaceInsertIndex( pTranNon, usItemOffset, TRN_TYPE_NOITEMSTORAGE );
			TrnStoReplaceInsertAppendIndexStatus = sStatusTemp;
        } else {
			TrnStoReplaceAppendIndex( pTranNon, usItemOffset, TRN_TYPE_ITEMSTORAGE );
			sStatusTemp = TrnStoReplaceInsertAppendIndexStatus;
			TrnStoReplaceAppendIndex( pTranNon, usItemOffset, TRN_TYPE_NOITEMSTORAGE );
			TrnStoReplaceInsertAppendIndexStatus = sStatusTemp;
        }
    } else {
        /* --- append print priority index to index file --- */
        TrnStoReplaceAppendIndex( pTranNon, usItemOffset, TRN_TYPE_ITEMSTORAGE );
		sStatusTemp = TrnStoReplaceInsertAppendIndexStatus;
        TrnStoReplaceAppendIndex( pTranNon, usItemOffset, TRN_TYPE_NOITEMSTORAGE );
		TrnStoReplaceInsertAppendIndexStatus = sStatusTemp;
    }
	return usItemOffset;
}

TrnVliOffset TrnStoItemNormalItemOnly ( VOID *pData, SHORT sSize )
{
    TrnVliOffset         usItemOffset;
	extern TrnVliOffset  usTrnStoItemNormalOffset;

    /* --- store source item to item storage file --- */

    usItemOffset = ( sizeof( TRANITEMSTORAGEHEADER ) + TrnInformation.usTranItemStoVli );

	usTrnStoItemNormalOffset = usItemOffset;

    TrnWriteFile( usItemOffset, pData, sSize, TrnInformation.hsTranItemStorage );

    /* --- update size of actual data in item storage file --- */
	NHPOS_ASSERT((ULONG)TrnInformation.usTranItemStoVli + sSize < 0x0000ffff);
    TrnInformation.usTranItemStoVli += sSize;
	return usItemOffset;
}


TrnVliOffset TrnStoItemNormal( VOID *pData, SHORT sSize )
{
    TRANSTORAGESALESNON *pTranNon = TRANSTORAGENONPTR( pData );
    TrnVliOffset         usItemOffset;
	extern TrnVliOffset  usTrnStoItemNormalOffset;

    /* --- store source item to item storage file --- */
    usItemOffset = ( sizeof( TRANITEMSTORAGEHEADER ) + TrnInformation.usTranItemStoVli );

	usTrnStoItemNormalOffset = usItemOffset;

    TrnWriteFile( usItemOffset, pData, sSize, TrnInformation.hsTranItemStorage );

    /* --- determine current system uses print priority --- */
	TrnStoItemNormalUpdateIndex( pTranNon, usItemOffset );

    /* --- update size of actual data in item storage file --- */
	NHPOS_ASSERT((ULONG)TrnInformation.usTranItemStoVli + sSize < 0x0000ffff);
    TrnInformation.usTranItemStoVli += sSize;
	return usItemOffset;
}


/*==========================================================================
**  Synopsis:   VOID TrnStoConsNormal( VOID *pData, SHORT sSize )
*
*   Input:      VOID  *pData    - address of item data to write file
*               SHORT sSize     - size of source data in bytes
*   Output:     none
*   InOut:      none
*
*   Return:     none
*
*   Description:    write "item" to consoli storage.
*
*                 In this function we are manipulating the global status
*                 variable TrnStoReplaceInsertAppendIndexStatus so that the
*                 caller of this function can use the status retrieval function
*                 TrnStoReplaceStatusIndex() to check to see if this function
*                 replaced an index item in the index file for TRN_TYPE_CONSSTORAGE.
*                 If the index item is replaced then it may mean that there
*                 is an item in the associated storage file that no longer is
*                 pointed to by an index entry in the index file for TRN_TYPE_CONSSTORAGE.
*                 If this is the case, the caller may want to put a copy of a valid
*                 index entry from file TRN_TYPE_NOCONSSTORAGE so that the item
*                 data will be available when displaying the consolidated data.
==========================================================================*/

TrnVliOffset TrnStoConsNormalUpdateIndex( TRANSTORAGESALESNON *pTranNon, TrnVliOffset usItemOffset )
{
	SHORT   sStatusTemp;  // temporary save variable for TrnStoReplaceInsertAppendIndexStatus

    if ( TrnInformation.TranCurQual.flPrintStatus & CURQUAL_PRINT_PRIORITY ) {
        if ( pTranNon->uchMajorClass == CLASS_ITEMSALES ) {
            /* --- if source item is sales item, analyze it's print priority
                and store it at analyzed position --- */
            TrnStoReplaceInsertIndex( pTranNon, usItemOffset, TRN_TYPE_CONSSTORAGE );
			sStatusTemp = TrnStoReplaceInsertAppendIndexStatus;
            TrnStoReplaceInsertIndex( pTranNon, usItemOffset, TRN_TYPE_NOCONSSTORAGE );
			TrnStoReplaceInsertAppendIndexStatus = sStatusTemp;
        } else {
            /* --- if source item is not sales item, store it at the end of
                item storage file --- */
            TrnStoReplaceAppendIndex( pTranNon, usItemOffset, TRN_TYPE_CONSSTORAGE );
			sStatusTemp = TrnStoReplaceInsertAppendIndexStatus;
            TrnStoReplaceAppendIndex( pTranNon, usItemOffset, TRN_TYPE_NOCONSSTORAGE );
			TrnStoReplaceInsertAppendIndexStatus = sStatusTemp;
        }
    } else {
        /* --- append print priority index to index file --- */
        TrnStoReplaceAppendIndex( pTranNon, usItemOffset, TRN_TYPE_CONSSTORAGE );
		sStatusTemp = TrnStoReplaceInsertAppendIndexStatus;
        TrnStoReplaceAppendIndex( pTranNon, usItemOffset, TRN_TYPE_NOCONSSTORAGE );
		TrnStoReplaceInsertAppendIndexStatus = sStatusTemp;
    }
	return usItemOffset;
}

TrnVliOffset TrnStoConsNormalItemOnly ( VOID *pData, SHORT sSize )
{
    TrnVliOffset         usItemOffset;
	extern TrnVliOffset  usTrnStoConsNormalOffset;

    /* --- store source item to consolidate storage file --- */
    usItemOffset = ( sizeof( TRANCONSSTORAGEHEADER ) + TrnInformation.usTranConsStoVli );

	usTrnStoConsNormalOffset = usItemOffset;

    TrnWriteFile( usItemOffset, pData, sSize, TrnInformation.hsTranConsStorage );

    /* --- update size of actual data in item storage file --- */
	NHPOS_ASSERT((ULONG)TrnInformation.usTranConsStoVli + sSize < 0x0000ffff);
    TrnInformation.usTranConsStoVli += sSize;
	return usItemOffset;
}


TrnVliOffset TrnStoConsNormal( VOID *pData, SHORT sSize )
{
    TRANSTORAGESALESNON  *pTranNon = TRANSTORAGENONPTR( pData );
    TrnVliOffset         usItemOffset;
	extern TrnVliOffset  usTrnStoConsNormalOffset;

    /* --- store source item to consolidate storage file --- */
    usItemOffset = ( sizeof( TRANCONSSTORAGEHEADER ) + TrnInformation.usTranConsStoVli );

	usTrnStoConsNormalOffset = usItemOffset;

    TrnWriteFile( usItemOffset, pData, sSize, TrnInformation.hsTranConsStorage );

    /* --- determine current system uses print priority --- */
	TrnStoConsNormalUpdateIndex( pTranNon, usItemOffset );

    /* --- update size of actual data in item storage file --- */
	NHPOS_ASSERT((ULONG)TrnInformation.usTranConsStoVli + sSize < 0x0000ffff);
    TrnInformation.usTranConsStoVli += sSize;
	return usItemOffset;
}

/*==========================================================================
**  Synopsis:   VOID TrnStoNoItemNormal( VOID *pData, USHORT usItemOffset )
*
*   Input:      VOID    *pData          -   address of item data to write
*               USHORT  usItemOffset    -   offset of target item
*   Output:     none
*   InOut:      none
*
*   Return:     none
*
*   Description:    write "item" to no-consoli item storage
==========================================================================*/

VOID TrnStoNoItemNormal( VOID *pData, TrnVliOffset usItemOffset )
{
    TRANSTORAGESALESNON *pTranNon = TRANSTORAGENONPTR( pData );

    /* --- determine current system uses print priority --- */
    if ( TrnInformation.TranCurQual.flPrintStatus & CURQUAL_PRINT_PRIORITY ) {
        if ( pTranNon->uchMajorClass == CLASS_ITEMSALES) {
            /* --- if source item is sales item, analyze it's print priority
                and store it at analyzed position --- */
			if (pTranNon->uchMinorClass == CLASS_ITEMORDERDEC) {
				TrnStoReplaceInsertIndex( pTranNon, usItemOffset, TRN_TYPE_NOITEMSTORAGE );
			}
			else {
				TrnStoReplaceInsertAppendIndexStatus = TrnStoInsertIndex (pTranNon, usItemOffset, TRN_TYPE_NOITEMSTORAGE);
			}
        } else {
            /* --- if source item is not sales item, store it at the end of
                item storage file --- */
            TrnStoAppendIndex( pTranNon, usItemOffset, TRN_TYPE_NOITEMSTORAGE );
        }
    } else {
        /* --- append print priority index to index file --- */
        TrnStoAppendIndex( pTranNon, usItemOffset, TRN_TYPE_NOITEMSTORAGE );
    }
}

/*==========================================================================
**  Synopsis:   VOID TrnStoNoConsNormal( VOID *pData, USHORT usItemOffset )
*
*   Input:      VOID    *pData          -   address of item data to write
*               USHORT  usItemOffset    -   offset of target item
*   Output:     none
*   InOut:      none
*
*   Return:     none
*
*   Description:    write "item" to no-consoli storage
==========================================================================*/

VOID TrnStoNoConsNormal( VOID *pData, TrnVliOffset usItemOffset )
{
    TRANSTORAGESALESNON *pTranNon = TRANSTORAGENONPTR( pData );

    /* --- determine current system uses print priority --- */
	usTrnStoConsNormalOffset = usItemOffset;

    if ( TrnInformation.TranCurQual.flPrintStatus & CURQUAL_PRINT_PRIORITY ) {
        if ( pTranNon->uchMajorClass == CLASS_ITEMSALES ) {
			/* --- if source item is sales item, analyze it's print priority
				and store it at analyzed position --- */
			if (pTranNon->uchMinorClass == CLASS_ITEMORDERDEC) {
				TrnStoReplaceInsertIndex( pTranNon, usItemOffset, TRN_TYPE_NOCONSSTORAGE );
			}
			else {
				TrnStoReplaceInsertAppendIndexStatus = TrnStoInsertIndex (pTranNon, usItemOffset, TRN_TYPE_NOCONSSTORAGE);
			}
        } else {
			/* --- if source item is not sales item, store it at the end of
				item storage file --- */
			TrnStoAppendIndex( pTranNon, usItemOffset, TRN_TYPE_NOCONSSTORAGE );
        }
    } else {
        /* --- append print priority index to index file --- */
        TrnStoAppendIndex( pTranNon, usItemOffset, TRN_TYPE_NOCONSSTORAGE );
    }
}


/*==========================================================================
**  Synopsis:   SHORT TrnStoAppendIndex( TRANSTORAGESALESNON *pTranNon,
*                                        USHORT               usItemOffset,
*                                        SHORT                sStorageType )
*
*   Input:      TRANSTORAGESALESNON *pTranNon       - address of sales data
*               USHORT               usItemOffset   - offset from top of storage
*               SHORT                sStorageType  - type of storage file
*                       TRN_TYPE_ITEMSTORAGE, TRN_TYPE_CONSSTORAGE
*   Output:     none
*   InOut:      none
*
*   Return:     TRN_SUCCESS - function is successful.
*               TRN_ERROR   - function is failed.
*
*   Description:    append an item index to index file.
==========================================================================*/

SHORT TrnStoAppendIndex( TRANSTORAGESALESNON *pTranNon,
                         TrnVliOffset         usItemOffset,
                         TrnStorageType       sStorageType )
{
    PifFileHandle  hsIndexFile;
    SHORT          sReturn;
    PRTIDX         IndexNew;
    PRTIDXHDR      IdxFileInfo;
    TrnVliOffset   usWriteOffset;
	ULONG		   ulActualBytesRead;//RPH 11-11-3 SR# 201

    /* --- retrieve handle of index file to append index --- */
	if ((hsIndexFile = TrnDetermineIndexFileHandle(sStorageType)) < 0) {
		return (TRN_ERROR);
	}

    /* --- prepare print priority index to append storage file --- */
    sReturn = TrnStoSetupPrtIdx( &IndexNew, pTranNon, usItemOffset );

    if ( sReturn != TRN_SUCCESS ) {
        return ( sReturn );
    }

    /* --- append print priority index to bottom of index file --- */
    //RPH 11-11-3 SR# 201
	TrnReadFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), hsIndexFile, &ulActualBytesRead );

    usWriteOffset = ( sizeof( PRTIDXHDR ) + ( sizeof( PRTIDX ) * IdxFileInfo.uchNoOfItem ));

    /* --- DEBUG --- */
/*****
    if ( IdxFileInfo.usIndexFSize < ( usWriteOffset + sizeof( PRTIDX ))) {
        PifAbort( MODULE_TRANS_STOR, 0xffff );
    }
*****/

    TrnWriteFile( usWriteOffset, &IndexNew, sizeof( PRTIDX ), hsIndexFile );

    /* --- update information about index file --- */
    IdxFileInfo.uchNoOfItem++;
    IdxFileInfo.usIndexVli += sizeof( PRTIDX );

    TrnWriteFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), hsIndexFile );

    return ( TRN_SUCCESS );
}

SHORT TrnStoInsertFrontIndex( TRANSTORAGESALESNON *pTranNon, TrnVliOffset usItemOffset, TrnStorageType sStorageType )
{
    PifFileHandle  hsIndexFile;
    SHORT          sReturn;
    TrnVliOffset   sHighestIdx;
    PRTIDX         IndexNew;
    PRTIDXHDR      IdxFileInfo;
    TrnVliOffset   usWriteOffset;
	ULONG		   ulActualBytesRead;//RPH 11-11-3 SR# 201

    /* --- retrieve handle of index file to append index --- */
	if ((hsIndexFile = TrnDetermineIndexFileHandle(sStorageType)) < 0) {
		return (TRN_ERROR);
	}

	/* --- prepare print priority index to append storage file --- */
    sReturn = TrnStoSetupPrtIdx( &IndexNew, pTranNon, usItemOffset );

    if ( sReturn != TRN_SUCCESS ) {
        return ( sReturn );
    }

	// Read the index file header information to obtain the index file
	// description such as number of items and the size of the stored
	// information.
	TrnReadFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), hsIndexFile, &ulActualBytesRead );

	sHighestIdx = IdxFileInfo.usSalesHighest;
	if (sHighestIdx > 0) {
		int            i;

		for (i = IdxFileInfo.uchNoOfItem - 1; i >= sHighestIdx; i--) {
			PRTIDX        IndexOld = {0};

			usWriteOffset = PRTIDX_OFFSET(i);
			TrnReadFile( usWriteOffset, &IndexOld, sizeof( PRTIDX ), hsIndexFile, &ulActualBytesRead );
			usWriteOffset += sizeof( PRTIDX );
			TrnWriteFile( usWriteOffset, &IndexOld, sizeof( PRTIDX ), hsIndexFile );
		}
		IdxFileInfo.usSalesLowest++;
	}
	else {
		sHighestIdx = IdxFileInfo.usSalesHighest = IdxFileInfo.usSalesLowest = IdxFileInfo.uchNoOfItem;
	}

    usWriteOffset = PRTIDX_OFFSET(sHighestIdx);
    TrnWriteFile( usWriteOffset, &IndexNew, sizeof( PRTIDX ), hsIndexFile );

    /* --- update information about index file --- */
    IdxFileInfo.uchNoOfItem++;
    IdxFileInfo.uchNoOfSales++;
    IdxFileInfo.usIndexVli += sizeof( PRTIDX );

    TrnWriteFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), hsIndexFile );

    return ( TRN_SUCCESS );
}


/*==========================================================================
**  Synopsis:   SHORT TrnStoInsertIndex( TRANSTORAGESALESNON *pTranNon,
*                                        USHORT               usItemOffset,
*                                        SHORT                sStorageType )
*
*   Input:      TRANSTORAGESALESNON *pTranNon       - address of sales data
*               USHORT               usItemOffset   - offset from top of storage
*               SHORT                sStorageType   - type of storage file
*                       TRN_TYPE_ITEMSTORAGE, TRN_TYPE_CONSSTORAGE
*   Output:     none
*   InOut:      none
*
*   Return:     TRN_SUCCESS - function is successful.
*               TRN_ERROR   - function is failed.
*
*   Description:    insert an item index to index file.  This function is used
*                to insert into the index file a new index.  This index entry
*                is for a sales type of item.  The index file is structured
*                so that there is a particular, contiguous region of the file
*                that contains the indexes for transaction items that are sales
*                items.
*
*                The index file header variable IdxFileInfo.usSalesHighest has
*                the value of the index item for the first sales item in the
*                contiguous region.  The index file header variable 
*                IdxFileInfo.usSalesLowest contains the value of the index item
*                for the last sales item in the coniguous region.
*
*                What this function does is to traverse the contiguous region
*                of sales indexes looking for the place to insert a new sales
*                item.  The decision as to where to insert the new item is
*                determined by TrnStoCompareIndex() which uses the print class
*                as well as seat number to make a determination.
==========================================================================*/

SHORT TrnStoInsertIndexFileHandle (SHORT hsIndexFile, TRANSTORAGESALESNON *pTranNon, USHORT usItemOffset )
{
    SHORT       sReturn;
    PRTIDX      IndexNew;
    PRTIDXHDR   IdxFileInfo;
    UCHAR       auchIdxWork[ sizeof( PRTIDX ) * 80 ];
    PRTIDX      PrtIdxMid;
    SHORT       sHighestIdx;
    SHORT       sLowestIdx;
    SHORT       sMiddleIdx;
    SHORT       sCompare;
    USHORT      usMoveIdxSize;
    USHORT      usWriteOffset;
    USHORT      usTtlIdxReadSize;
    USHORT      usCurIdxReadSize;
    USHORT      usMoveIdxOffset;
	ULONG		ulActualBytesRead;//RPH 11-11-3 SR# 201

    /* --- prepare print priority index to insert storage file --- */
    sReturn = TrnStoSetupPrtIdx( &IndexNew, pTranNon, usItemOffset );

    if ( sReturn != TRN_SUCCESS ) {
        return ( sReturn );
    }

    /* --- search for insert position, and create vacant space to be stored new index --- */
    //RPH 11-11-3 SR# 201
	TrnReadFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), hsIndexFile, &ulActualBytesRead );

    if ( IdxFileInfo.uchNoOfSales > 0) {
		if(pTranNon->uchMajorClass == CLASS_ITEMSALES && pTranNon->uchMinorClass == CLASS_ITEMORDERDEC) {
			sMiddleIdx = IdxFileInfo.usSalesHighest;
		}
		else {
			sHighestIdx = IdxFileInfo.usSalesHighest;
			sLowestIdx  = IdxFileInfo.usSalesLowest;
			sCompare    = 1;

			while (( sHighestIdx <= sLowestIdx ) && ( sCompare )) {
				sMiddleIdx = (( sHighestIdx + sLowestIdx ) / 2 );
				//RPH 11-11-3 SR# 201
				TrnReadFile (PRTIDX_OFFSET(sMiddleIdx), &PrtIdxMid, sizeof( PRTIDX ), hsIndexFile, &ulActualBytesRead );

				sCompare = TrnStoCompareIndex( &IndexNew, &PrtIdxMid );

				if ( sCompare < 0 ) {           /* new is higher than middle */
					sLowestIdx  = sMiddleIdx - 1;
				} else if ( 0 < sCompare ) {    /* new is lower than middle */
					sHighestIdx = sMiddleIdx + 1;
				}
			}

			if ( sCompare == 0 ) { /* same priority record is found */
				if ( sMiddleIdx == sLowestIdx ) {
					sMiddleIdx++;
				} else if ( sMiddleIdx < sLowestIdx ) {
					/* --- insert new item to end of the same priority item --- */
					do {
						sMiddleIdx++;
						//RPH 11-11-3 SR# 201
						TrnReadFile (PRTIDX_OFFSET(sMiddleIdx), &PrtIdxMid, sizeof( PrtIdxMid ), hsIndexFile, &ulActualBytesRead );

						sCompare = TrnStoCompareIndex( &IndexNew, &PrtIdxMid );
					} while (( sMiddleIdx <= sLowestIdx ) && ( sCompare == 0 ));
				}
			} else {            /* same priority record is not found */
				/* --- determine the last analyzed item is higher or lower
					than new item --- */
				if ( 0 < sCompare ) {   /* new is lower than middle */
					/* --- insert new item to next to last analyzed item --- */
					sMiddleIdx++;
				} else {                /* new is higher than middle */
					/* --- insert new item to last item position, and last item
						is moved to next cell --- */
					;   /* do nothing */
				}
			}
		}

        /* --- move index items which are lower than new item to make a vacant
            cell for storing new item --- */

        usWriteOffset = PRTIDX_OFFSET(sMiddleIdx);

		// If we are inserting by actually just adding to the end of the file
		// then we do not need to move anything.  However, if we are inserting
		// in the middle of the file then we need to move stuff.
		// Remember that IdxFileInfo.usIndexVli is the offset within the index
		// part of the file.  It does not include the length of the index file
		// header which is a struct PRTIDXHDR so we need to include the size of
		// the struct in our offset calculations.
		if (IdxFileInfo.usIndexVli + sizeof( PRTIDXHDR ) > usWriteOffset) {
			usMoveIdxSize = IdxFileInfo.usIndexVli + sizeof( PRTIDXHDR ) - usWriteOffset;

			usCurIdxReadSize = sizeof( auchIdxWork );
			if (usCurIdxReadSize > usMoveIdxSize) {
				usCurIdxReadSize = usMoveIdxSize;
			}

			usMoveIdxOffset = IdxFileInfo.usIndexVli + sizeof( PRTIDXHDR ) - usCurIdxReadSize;

			usTtlIdxReadSize = 0;
			while ( usTtlIdxReadSize < usMoveIdxSize ) {

				//RPH 11-11-3 SR# 201
				TrnReadFile( usMoveIdxOffset, auchIdxWork, usCurIdxReadSize, hsIndexFile, &ulActualBytesRead );

				TrnWriteFile( usMoveIdxOffset + sizeof( PRTIDX ), auchIdxWork, usCurIdxReadSize, hsIndexFile ); /* ### Mod (2171 for Win32) V1.0 */

				usTtlIdxReadSize += usCurIdxReadSize;
				usMoveIdxOffset -= usCurIdxReadSize; 

				if (usCurIdxReadSize > usMoveIdxSize - usTtlIdxReadSize) {
					usCurIdxReadSize = usMoveIdxSize - usTtlIdxReadSize;
				}
			}
		}

        IdxFileInfo.usSalesLowest++;
    } else {
        /* --- this is first sales item to be stored in index file --- */
        IdxFileInfo.usSalesHighest = IdxFileInfo.uchNoOfItem;
        IdxFileInfo.usSalesLowest  = IdxFileInfo.uchNoOfItem;

        usWriteOffset = sizeof(PRTIDXHDR) + (sizeof(PRTIDX) * IdxFileInfo.uchNoOfItem);
    }

    /* --- DEBUG --- */
/********************************************
    if ( IdxFileInfo.usIndexFSize < ( usWriteOffset + sizeof( PRTIDX ))) {
        PifAbort( MODULE_TRANS_STOR, 0xffff );
    }
********************************************/

    TrnWriteFile( usWriteOffset, &IndexNew, sizeof( PRTIDX ), hsIndexFile );

    /* --- update information about index file --- */

    IdxFileInfo.uchNoOfItem++;
    IdxFileInfo.uchNoOfSales++;
    IdxFileInfo.usIndexVli += sizeof( PRTIDX );

    TrnWriteFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), hsIndexFile );

    return ( TRN_SUCCESS );
}

SHORT TrnStoInsertIndex( TRANSTORAGESALESNON *pTranNon,
                         TrnVliOffset         usItemOffset,
                         TrnStorageType       sStorageType )
{
    SHORT       hsIndexFile;

    /* --- retrieve handle of index file to append index --- */
#if 1
	if ((hsIndexFile = TrnDetermineIndexFileHandle(sStorageType)) < 0) {
		return (TRN_ERROR);
	}
#else
    switch ( sStorageType ) {
		case TRN_TYPE_ITEMSTORAGE:      /* item storage */
			hsIndexFile = TrnInformation.hsTranItemIndex;
			break;

		case TRN_TYPE_NOITEMSTORAGE:    /* no-cons. item storage */
			hsIndexFile = TrnInformation.hsTranNoItemIndex;
			break;

		case TRN_TYPE_CONSSTORAGE:      /* consolidation storage */
			hsIndexFile = TrnInformation.hsTranConsIndex;
			break;

		case TRN_TYPE_NOCONSSTORAGE:    /* no-cons. consolidation storage */
			hsIndexFile = TrnInformation.hsTranNoConsIndex;
			break;

		default:
			return ( TRN_ERROR );
    }
#endif

	return TrnStoInsertIndexFileHandle (hsIndexFile, pTranNon, usItemOffset);
}



/*==========================================================================
**  Synopsis: SHORT TrnStoSetupPrtIndex( PRTIDX  *pPrtIdx,
*                                        VOID    *pvTranNon,
*                                        USHORT  usItemOffset )
*
*   Input:    PRTIDX *pPrtIdx       - address of print index
*             VOID   *pvTranNon     - address of sales data
*             USHORT usItemOffset   - offset from top of storage
*   Output:   none
*   InOut:    none
*
*   Return:   TRN_SUCCESS - function is successful.
*             TRN_ERROR   - function is failed.
*
*   Description:    set up print priority index.
==========================================================================*/

SHORT TrnStoSetupPrtIdx( PRTIDX *pPrtIdx, VOID *pvTranNon, TrnVliOffset usItemOffset )
{
    UCHAR   uchPrintClass;
    TRANSTORAGESALESNON *pSalesNon = pvTranNon;

	/* 07/26/01 */
	if (usItemOffset == 0) {
		return (TRN_ERROR);
	}

    /*
		Initialize the index struct.
		
		if the sales item point is NULL or if the sales
		item is a non-sales item, prepare the print index for
        a non-sales item
	**/

    pPrtIdx->usItemOffset  = usItemOffset;
    pPrtIdx->uchSeatNo     = 0;
    pPrtIdx->uchPrintClass = PRT_MAKE_PRICLASS( 0, PRT_OTHER );
    pPrtIdx->usDeptNo   = 0;
    pPrtIdx->lQTY          = 0L;
    pPrtIdx->lProduct      = 0L;
    pPrtIdx->sNetQTY       = 0;
	pPrtIdx->usUniqueID    = 0;
	pPrtIdx->usKdsLineNo   = 0;
	pPrtIdx->uchMajorClass = 0;
	pPrtIdx->uchMinorClass = 0;

	_tcsncpy (pPrtIdx->auchPLUNo, MLD_NONSALESITEM_PLU, STD_PLU_NUMBER_LEN);

    if ( pSalesNon == NULL) {
        return ( TRN_SUCCESS );
    }

	// The pointer is non-NULL so lets capture the minor class regardless
	// of whether item is sales or non-sales item.
	// Non-sales item are such pseudo items as Total, Cancel, and
	// Order Declare.
	pPrtIdx->uchMajorClass = pSalesNon->uchMajorClass;
	pPrtIdx->uchMinorClass = pSalesNon->uchMinorClass;

    if ( pSalesNon->uchMajorClass != CLASS_ITEMSALES ) {
        return ( TRN_SUCCESS );
    }

	pPrtIdx->usUniqueID    = pSalesNon->usUniqueID;
	pPrtIdx->usKdsLineNo   = pSalesNon->usKdsLineNo;

    switch ( pSalesNon->uchMinorClass )
	{
		case CLASS_ITEMORDERDEC:
			uchPrintClass = 0;
			break;

		case CLASS_DEPT:
		case CLASS_DEPTITEMDISC:
		case CLASS_DEPTMODDISC:
			uchPrintClass = ( UCHAR )( pSalesNon->uchMinorClass - CLASS_DEPT + PRT_DEPT );
			break;

		case CLASS_PLU:
		case CLASS_PLUITEMDISC:
		case CLASS_PLUMODDISC:
			uchPrintClass = ( UCHAR )( pSalesNon->uchMinorClass - CLASS_PLU + PRT_PLU );
			break;

		case CLASS_SETMENU:
		case CLASS_SETITEMDISC:
		case CLASS_SETMODDISC:
			uchPrintClass = ( UCHAR )( pSalesNon->uchMinorClass - CLASS_SETMENU + PRT_SETMENU );
			break;

		case CLASS_OEPPLU:
		case CLASS_OEPITEMDISC:
		case CLASS_OEPMODDISC:
			uchPrintClass = ( UCHAR )( pSalesNon->uchMinorClass - CLASS_OEPPLU + PRT_OEPPLU );
			break;

		default:
			return ( TRN_ERROR );
    }

    pPrtIdx->uchPrintClass = PRT_MAKE_PRICLASS( pSalesNon->uchPrintPriority, uchPrintClass );
    _tcsncpy(pPrtIdx->auchPLUNo, pSalesNon->auchPLUNo, NUM_PLU_LEN);
    pPrtIdx->usItemOffset  = usItemOffset;
    pPrtIdx->lQTY          = pSalesNon->lQTY;
    pPrtIdx->lProduct      = pSalesNon->lProduct;

    /* V3.3 */
    pPrtIdx->uchSeatNo     = pSalesNon->uchSeatNo;
    pPrtIdx->sNetQTY       = (SHORT)(pSalesNon->lQTY/1000L);
	pPrtIdx->usDeptNo      = pSalesNon->usDeptNo;

    return ( TRN_SUCCESS );
}

/*==========================================================================
**  Synopsis:   SHORT TrnStoCompareIndex( PRTIDX *pPrtIdx1,
*                                         PRTIDX *pPrtIdx2 )
*
*   Input:      PRTIDX *pPrtIdx1 - address of source index
*               PRTIDX *pPrtIdx2 - address of source index
*   Output:     none
*   InOut:      none
*
*   Return:     value < 0   -   pPrtIdx1 is higher priority than pPrtIdx2
*               0 < value   -   pPrtIdx1 is lower priority than pPrtIdx2
*               value == 0  -   pPrtIdx1 is the same priority as pPrtIdx2
*
*   Description:    compare two print priority indexes.           R3.1
==========================================================================*/

SHORT TrnStoCompareIndex( PRTIDX *pPrtIdx1, PRTIDX *pPrtIdx2 )
{
    UCHAR   uchPriority1;
    UCHAR   uchPriority2;

    /* --- compare seat no, if print priority is used, V3.3 --- */
    /* if seat no. is not set, assume as last */
    uchPriority1 = (pPrtIdx1->uchSeatNo ? pPrtIdx1->uchSeatNo : (UCHAR)0xFF);
    uchPriority2 = (pPrtIdx2->uchSeatNo ? pPrtIdx2->uchSeatNo : (UCHAR)0xFF);

    if ( uchPriority1 < uchPriority2 ) {
        return ( -1 );

    } else if ( uchPriority2 < uchPriority1 ) {
        return ( 1 );
    }

    /* --- compare print priority code --- */
    uchPriority1 = PRT_GET_PRIORITY( pPrtIdx1->uchPrintClass );
    uchPriority2 = PRT_GET_PRIORITY( pPrtIdx2->uchPrintClass );

    if ( uchPriority1 < uchPriority2 ) {
        return ( -1 );

    } else if ( uchPriority2 < uchPriority1 ) {
        return ( 1 );
    }

    /* --- each item numbers are the same, two item have the same
        print priority --- */
    return ( 0 );
}

/*==========================================================================
**    Synopsis: VOID    TrnPutInfGuestSpl(UCHAR uchMiddle, UCHAR uchRight)
*
*    Input:
*    Output:    none
*    InOut:
*
*    Return:    none
*
*    Description:   Set Seat# to "TrnInformation.usGuestNo"
==========================================================================*/
VOID    TrnPutInfGuestSpl(UCHAR uchMiddle, UCHAR uchRight)
{
    if (uchMiddle != 0) {
        TrnInformation.usGuestNo[0] = uchMiddle;
    }
    if (uchRight != 0) {
        TrnInformation.usGuestNo[1] = uchRight;
    }
}

/*==========================================================================
**    Synopsis: SHORT   TrnCheckSize( VOID *pData, USHORT Data )
*
*    Input:     VOID    *pData, USHORT  Data
*    Output:    none
*    InOut:     none
*
*    Return:    compressed size
*               0                    (pData->uchMajorClass is zero)
*               TRN_BUFFER_ILLEGAL   (error that is not storage related)
*               TRN_BUFFER_NEAR_FULL (storage is nearly full but not full)
*               TRN_BUFFER_FULL      (storage is full)
*
*    Description:   check item/consoli storage size to determine if this item
*                   will fit into the Transaction File once it is compressed.
*                   This function does a check on remaining available storage
*                   when using non-consolidated storage since items are not
*                   consolidated into a single record.
*
*                   See also function TrnCalStoSize() which is used to calculate
*                   the transaction storage file size based on number of items
*                   per transaction.
*
*                   See also function TrnChkStorageSizeWithOrWithoutReserve() which
*                   is used as items are entered to check if we are near the
*                   transaction data file size limit.
==========================================================================*/

SHORT    TrnCheckSize( VOID *pData, USHORT Data )
{
    LONG            lRestI;
    LONG            lRestC;
    LONG            lItemVli;
    LONG            lConsoliVli;
	ULONG			ulActualBytesRead;//RPH 11-11-3 SR# 201
    LONG            lVacantSize;
    LONG            lInquirySize;
    SHORT           sStatus = TRN_SUCCESS;
    PRTIDXHDR       IdxFileInfo;
    UCHAR           uchBuffer[TRN_WORKBUFFER];

    /* --- if paid order recall function, always return compress size.
        because if paid order trans is buffer full, new itemize is always
        buffer full error --- */
    if ( TrnInformation.TranGCFQual.fsGCFStatus2 & GCFQUAL_PAYMENT_TRANS ) {
        sStatus = TrnPaidOrderCheckSize( pData );
        return ( sStatus );
    }

    /* --- determine item index file has a vacant cell which is storing
        new item --- */

    /* add auto charge tips index, V3.3 */
    lInquirySize = ( sizeof( PRTIDX ) + sizeof( PRTIDX ) + ( sizeof( PRTIDX ) * PRTIDX_RESERVE ));

    //RPH 11-11-3 SR# 201
	TrnReadFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), TrnInformation.hsTranItemIndex, &ulActualBytesRead );

    lVacantSize  = (LONG)IdxFileInfo.usIndexFSize - sizeof( PRTIDXHDR ) - IdxFileInfo.usIndexVli;
    if ( lVacantSize < lInquirySize ) {
		NHPOS_ASSERT_TEXT((lVacantSize < lInquirySize), "==ERROR: TrnCheckSize - insufficient room.");
        return( TRN_BUFFER_FULL );
    }

    /* --- determine no-consoli index file has a vacant cell which is storing
        new item --- */

    //RPH 11-11-3 SR# 201
	TrnReadFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), TrnInformation.hsTranNoItemIndex, &ulActualBytesRead );

    lVacantSize  = (LONG)IdxFileInfo.usIndexFSize - sizeof( PRTIDXHDR ) - IdxFileInfo.usIndexVli;
    if ( lVacantSize < lInquirySize ) {
		NHPOS_ASSERT_TEXT((lVacantSize < lInquirySize), "==ERROR: TrnCheckSize - insufficient room.");
       return( TRN_BUFFER_FULL );
    }

    /* --- determine consoli index file has a vacant cell which is storing
        new item --- */

    //RPH 11-11-3 SR# 201
	TrnReadFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), TrnInformation.hsTranConsIndex, &ulActualBytesRead );

    lVacantSize  = (LONG)IdxFileInfo.usIndexFSize - sizeof( PRTIDXHDR ) - IdxFileInfo.usIndexVli;
    if ( lVacantSize < lInquirySize ) {
		NHPOS_ASSERT_TEXT((lVacantSize < lInquirySize), "==ERROR: TrnCheckSize - insufficient room.");
        return( TRN_BUFFER_FULL );
    }

    /* --- determine no-consoli index file has a vacant cell which is storing
        new item --- */

    //RPH 11-11-3 SR# 201
	TrnReadFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), TrnInformation.hsTranNoConsIndex, &ulActualBytesRead );

    lVacantSize  = (LONG)IdxFileInfo.usIndexFSize - sizeof( PRTIDXHDR ) - IdxFileInfo.usIndexVli;
    if ( lVacantSize < lInquirySize ) {
		NHPOS_ASSERT_TEXT((lVacantSize < lInquirySize), "==ERROR: TrnCheckSize - insufficient room.");
        return( TRN_BUFFER_FULL );
    }

    lItemVli    = TrnInformation.usTranItemStoVli;
    lConsoliVli = TrnInformation.usTranConsStoVli;

	// calculate two different values for the remaining amount of room in the transaction data file.
	// the first, lRestI, is for the amount of room in the Itemized Storage file and the second, lRestC,
	// is for the amount of room in the Consolidated Storage file.
	// the Consolidated Storage file also contains the associated Index Storage file as well so that size
	// is part of the calculation.
    lRestI = (LONG)TrnInformation.usTranItemStoFSize - (TRN_1ITEM_BYTES * 2) - sizeof(TRANITEMSTORAGEHEADER );
    lRestC = (LONG)TrnInformation.usTranConsStoFSize - (TRN_1ITEM_BYTES * 2) - sizeof(TRANCONSSTORAGEHEADER) - IdxFileInfo.usIndexFSize;

	switch( ITEMSTORAGENONPTR(pData)->uchMajorClass ) {
		case 0:
			// in some cases this function is used to calculate the size of the currently buffered item
			// in the memory resident data area ItemCommonLocal.ItemSales which will normally contain the
			// last item that was entered but has not yet been stored in the transaction data file.
			// if that data area has been cleared the the ItemCommonLocal.ItemSales.uchMajorClass will
			// have a value of zero meaning there is no data there.
			// in this special case we will return a value of zero so that we will not get a bogus error
			// of LDT_TAKETL_ADR indicating the Transaction File is full.
			//    Richard Chambers, Sep-22-2017 for GenPOS Rel 2.2.2 main trunk development.
			sStatus = 0;     // special case if uchMajorClass is zero then return size of zero.
			break;

    case CLASS_ITEMTRANSOPEN :                                      /* transaction open */
		{
			ITEMTRANSOPEN   *pItemTransOpen = pData;

			sStatus = TrnStoOpenComp(pItemTransOpen, uchBuffer );
			if ( ((pItemTransOpen->fbStorageStatus & NOT_ITEM_STORAGE) == 0) && ((lItemVli + sStatus) > lRestI) ) {
				   sStatus = TRN_BUFFER_FULL;                          /* storage full */
			}
			if ( ((pItemTransOpen->fbStorageStatus & NOT_CONSOLI_STORAGE) == 0) && ((lConsoliVli + sStatus) > lRestC) ) {
				   sStatus = TRN_BUFFER_FULL;                          /* storage full */
			}
		}
        break;

    case CLASS_ITEMSALES :                                          /* sales */
		{
			ITEMSALES   *pItemSales = pData;

			sStatus = TrnStoSalesComp(pItemSales, uchBuffer );
			if ( (pItemSales->fbStorageStatus & NOT_ITEM_STORAGE) == 0) {
				if ((lItemVli + sStatus ) > lRestI) {
				   sStatus = TRN_BUFFER_FULL;                           /* storage full */
				}
				else if ((lItemVli + sStatus * 3) > lRestI) {
				   sStatus = TRN_BUFFER_NEAR_FULL;                      /* storage near full */
				}
			}
			if ( (pItemSales->fbStorageStatus & NOT_CONSOLI_STORAGE) == 0) {
				if ((lConsoliVli + sStatus) > lRestC) {
				   sStatus = TRN_BUFFER_FULL;                           /* storage full */
				}
				else if ((lConsoliVli + sStatus * 3) > lRestC) {
				   sStatus = TRN_BUFFER_NEAR_FULL;                      /* storage near full */
				}
			}
		}
        break;

    case CLASS_ITEMDISC :                                           /* discount */
		{
			ITEMDISC  *pItemDisc = pData;

			sStatus = TrnStoDiscComp(pItemDisc, uchBuffer );
			if ( (pItemDisc->fbStorageStatus & NOT_ITEM_STORAGE) == 0) {
				if ((lItemVli + sStatus ) > lRestI) {
				   sStatus = TRN_BUFFER_FULL;                           /* storage full */
				}
				else if ((lItemVli + sStatus * 3) > lRestI) {
				   sStatus = TRN_BUFFER_NEAR_FULL;                      /* storage near full */
				}
			}
			if ( (pItemDisc -> fbStorageStatus & NOT_CONSOLI_STORAGE) == 0) {
				if ((lConsoliVli + sStatus) > lRestC) {
				   sStatus = TRN_BUFFER_FULL;                           /* storage full */
				}
				else if ((lConsoliVli + sStatus * 3) > lRestC) {
				   sStatus = TRN_BUFFER_NEAR_FULL;                      /* storage near full */
				}
			}
		}
        break;

    case CLASS_ITEMCOUPON:                                          /* coupon */
		{
			ITEMCOUPON      *pItemCoupon = pData;

			sStatus = TrnStoCouponComp(pItemCoupon, uchBuffer );
			if (( ! (pItemCoupon->fbStorageStatus & NOT_ITEM_STORAGE)) && ((lItemVli + sStatus) > lRestI)) {
				   sStatus = TRN_BUFFER_FULL;                           /* storage full */
			}
			if (( ! (pItemCoupon -> fbStorageStatus & NOT_CONSOLI_STORAGE)) && ((lConsoliVli + sStatus) > lRestC)) {
				   sStatus = TRN_BUFFER_FULL;                           /* storage full */
			}
		}
        break;

    case CLASS_ITEMTOTAL :                                          /* total key */
		{
			ITEMTOTAL  *pItemTotal = pData;

			sStatus = TrnStoTotalComp(pItemTotal, uchBuffer );
			if ( ((pItemTotal->fbStorageStatus & NOT_ITEM_STORAGE) == 0) && ((lItemVli + sStatus ) > lRestI) ) {
				   sStatus = TRN_BUFFER_FULL;                           /* storage full */
			}
			if ( ((pItemTotal->fbStorageStatus & NOT_CONSOLI_STORAGE) == 0) && ((lConsoliVli + sStatus ) > lRestC) ) {
				   sStatus = TRN_BUFFER_FULL;                           /* storage full */
			}
		}
        break;

    case CLASS_ITEMTENDER :                                         /* tender */
		{
			ITEMTENDER  *pItemTender = pData;

			sStatus = TrnStoTenderComp(pItemTender, uchBuffer );
			if ( ((pItemTender->fbStorageStatus & NOT_ITEM_STORAGE) == 0) && ((lItemVli + sStatus) > lRestI) ) {
				   sStatus = TRN_BUFFER_FULL;                           /* storage full */
			}
			if ( ((pItemTender->fbStorageStatus & NOT_CONSOLI_STORAGE) == 0) && ((lConsoliVli + sStatus ) > lRestC) ) {
				   sStatus = TRN_BUFFER_FULL;                           /* storage full */
			}
		}
        break;

    case CLASS_ITEMMISC :                                           /* misc. transaction */
		{
			ITEMMISC  *pItemMisc = pData;

			sStatus = TrnStoMiscComp(pItemMisc, uchBuffer );
			if ( ((pItemMisc->fbStorageStatus & NOT_ITEM_STORAGE) == 0) &&  ((lItemVli + sStatus) > lRestI) ) {
				   sStatus = TRN_BUFFER_FULL;                          /* storage full */
			}
			if ( ((pItemMisc->fbStorageStatus & NOT_CONSOLI_STORAGE) == 0) && ((lConsoliVli + sStatus ) > lRestC) ) {
				   sStatus = TRN_BUFFER_FULL;                          /* storage full */
			}
		}
        break;

    case CLASS_ITEMAFFECTION :                                      /* affection */
		{
			ITEMAFFECTION   *pItemAffection = pData;

			sStatus = TrnStoAffectComp(pItemAffection, uchBuffer );
			if ( ((pItemAffection->fbStorageStatus & NOT_ITEM_STORAGE) == 0)) {
				if ((lItemVli + sStatus) > lRestI) {
				   sStatus = TRN_BUFFER_FULL;                           /* storage full */
				}
				else if ((lItemVli + sStatus * 2) > lRestI) {
				   sStatus = TRN_BUFFER_NEAR_FULL;                      /* storage near full */
				}
			}
			if ( ((pItemAffection->fbStorageStatus & NOT_CONSOLI_STORAGE) == 0)) {
				if ((lConsoliVli + sStatus) > lRestC) {
				   sStatus = TRN_BUFFER_FULL;                           /* storage full */
				}
				else if ((lConsoliVli + sStatus * 2) > lRestC) {
				   sStatus = TRN_BUFFER_NEAR_FULL;                      /* storage near full */
				}
			}
		}
        break;

    case CLASS_ITEMPRINT :                                          /* print */
		{
			ITEMPRINT  *pItemPrint = pData;

			sStatus = TrnStoPrintComp (pItemPrint, uchBuffer );
			if ( ((pItemPrint->fbStorageStatus & NOT_ITEM_STORAGE) == 0) && ((lItemVli + sStatus ) > lRestI) ) {
				   sStatus = TRN_BUFFER_FULL;                           /* storage full */
			}
			if ( ((pItemPrint->fbStorageStatus & NOT_CONSOLI_STORAGE) == 0) && ((lConsoliVli + sStatus ) > lRestC) ) {
				   sStatus = TRN_BUFFER_FULL;                           /* storage full */
			}
		}
        break;

    default :
		NHPOS_ASSERT_TEXT(0, "TrnCheckSize(): Illegal major class");
        sStatus = TRN_BUFFER_ILLEGAL;                               /* Illegal major class */
    }

	// return either an error condition if negative value or the compressed size in bytes if positive
	{
		char  xBuff[128];
		sprintf (xBuff, "==ERROR: TrnCheckSize - class %d sStatus = %d.", *( UCHAR *)pData, sStatus);
		NHPOS_ASSERT_TEXT((sStatus >= 0), xBuff);
	}
    return( sStatus );
}

/*==========================================================================
**    Synopsis: SHORT   TrnPaidOrderCheckSize( VOID *pData )
*
*    Input:     VOID    *pData
*    Output:    none
*    InOut:     none
*
*    Return:    compressed size
*               TRN_ERROR (storage full)
*
*    Description:   check item/consoli storage size
==========================================================================*/

static SHORT TrnPaidOrderCheckSize( VOID *pData )
{
    UCHAR   auchBuffer[ TRN_WORKBUFFER ];
    SHORT   sCompSize;

	switch( ITEMSTORAGENONPTR(pData)->uchMajorClass ) {

    case CLASS_ITEMTRANSOPEN:
        sCompSize = TrnStoOpenComp(pData, auchBuffer );
        break;

    case CLASS_ITEMSALES:
        sCompSize = TrnStoSalesComp(pData, auchBuffer );
        break;

    case CLASS_ITEMDISC:
        sCompSize = TrnStoDiscComp(pData, auchBuffer );
        break;

    case CLASS_ITEMCOUPON:
        sCompSize = TrnStoCouponComp(pData, auchBuffer );
        break;

    case CLASS_ITEMTOTAL:
        sCompSize = TrnStoTotalComp(pData, auchBuffer );
        break;

    case CLASS_ITEMTENDER:
        sCompSize = TrnStoTenderComp(pData, auchBuffer );
        break;

    case CLASS_ITEMMISC:
        sCompSize = TrnStoMiscComp(pData, auchBuffer );
        break;

    case CLASS_ITEMAFFECTION:
        sCompSize = TrnStoAffectComp(pData, auchBuffer );
        break;

    case CLASS_ITEMPRINT:
        sCompSize = TrnStoPrintComp(pData, auchBuffer );
        break;

    default:
        sCompSize = TRN_ERROR;
    }
    return ( sCompSize );
}


/**===========================================================================
** Synopsis:    VOID   TrnStoGetConsToPostR( VOID )
*
*     Input:    nothing
*    Output:    nothing
*
** Return:      nothing
*
** Description: copy data from consoli. storage file to post rec. storage file which
*               stores the data for a Post Receipt or a Print on Demand.
*
*               SEE ALSO: complementary function to TrnStoPutPostRToCons() which
*                         will retrieve the data from the Post Receipt storage.
*===========================================================================*/
VOID TrnStoGetConsToPostR( VOID )
{
    ULONG       ulPostRQualSize;
    ULONG       ulReadLen;
    ULONG       ulReadOffset;
    ULONG       ulWriteOffset;
    USHORT      usTtlVliSize;
    ULONG       ulTtlWriteSize;
    UCHAR       auchTranStorageWork[ TRN_TEMPBUFF_SIZE ];
    UCHAR       *puchTempBuff;
    USHORT      husIndexFile;
    PRTIDXHDR   IdxFileInfo;
    ULONG       ulTtlIdxReadSize;
    ULONG       ulCurIdxReadSize;
	ULONG		ulActualBytesRead;//RPH 11-11-3 SR# 201

	/* Save item count to TranGCFQual before storing to hard disk*/
	TrnInformation.TranGCFQual.sItemCount = TrnInformation.TranItemizers.sItemCounter;

    /* --- store qualifier data to post receipt file --- */

    ulWriteOffset   = sizeof( USHORT );
    ulPostRQualSize = sizeof( TRANGCFQUAL ) + sizeof( TRANCURQUAL );

    TrnWriteFile( ulWriteOffset, &( TrnInformation.TranGCFQual ), ulPostRQualSize, TrnInformation.hsTranPostRecStorage );

    /* --- copy consolidate storage's transaction data to post receipt --- */

    ulReadOffset   = sizeof( USHORT ) + sizeof( USHORT ) + sizeof( TRANITEMIZERS ) + sizeof( TRANGCFQUAL );
    ulWriteOffset += ulPostRQualSize;
    ulTtlWriteSize = 0;
    usTtlVliSize   = sizeof( USHORT ) + TrnInformation.usTranConsStoVli;
    puchTempBuff   = &auchTranStorageWork[ 0 ];

    do {
		TrnReadFile( ulReadOffset, puchTempBuff, sizeof( auchTranStorageWork ), TrnInformation.hsTranConsStorage, &ulReadLen );

        TrnWriteFile( ulWriteOffset, puchTempBuff, ulReadLen, TrnInformation.hsTranPostRecStorage );

        ulReadOffset   += ulReadLen;
        ulWriteOffset  += ulReadLen;
        ulTtlWriteSize += ulReadLen;

    } while ( ulTtlWriteSize < usTtlVliSize );

    /* --- write actual size of trans. data to post receipt --- */

    TrnWriteFile( (sizeof( USHORT ) + ulPostRQualSize), &TrnInformation.usTranConsStoVli,
                  sizeof( USHORT ), TrnInformation.hsTranPostRecStorage ); /* ### Mod (2171 for Win32) V1.0 */

    /* --- write consolidate's print priority index to post receipt --- */

    husIndexFile = ( TrnInformation.TranCurQual.auchTotalStatus[ 4 ] & CURQUAL_TOTAL_NOCONSOLIDATE_PRINT) ? TrnInformation.hsTranNoConsIndex : TrnInformation.hsTranConsIndex ;

    TrnReadFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), husIndexFile, &ulActualBytesRead );

    TrnWriteFile( (sizeof( USHORT ) + ulPostRQualSize + usTtlVliSize),
                  &IdxFileInfo, sizeof( PRTIDXHDR ), TrnInformation.hsTranPostRecStorage ); /* ### Mod (2171 for Win32) V1.0 */

    /* --- save print priority index to GCF --- */

    ulWriteOffset    =  sizeof( USHORT ) + ulPostRQualSize + usTtlVliSize + sizeof( PRTIDXHDR );
    ulTtlIdxReadSize = 0;
    ulCurIdxReadSize = sizeof( auchTranStorageWork );

    while ( ulTtlIdxReadSize < IdxFileInfo.usIndexVli ) {

        //RPH 11-11-3 SR# 201
		TrnReadFile( (sizeof( PRTIDXHDR ) + ulTtlIdxReadSize), puchTempBuff, sizeof( auchTranStorageWork ), husIndexFile, &ulCurIdxReadSize ); /* ### Mod (2171 for Win32) V1.0 */

        if ( IdxFileInfo.usIndexVli < ( ulTtlIdxReadSize + ulCurIdxReadSize )) {
            ulCurIdxReadSize = IdxFileInfo.usIndexVli - ulTtlIdxReadSize;
        }

        TrnWriteFile( (ulWriteOffset + ulTtlIdxReadSize), puchTempBuff, ulCurIdxReadSize, TrnInformation.hsTranPostRecStorage ); /* ### Mod (2171 for Win32) V1.0 */

        ulTtlIdxReadSize += ulCurIdxReadSize;
    }
}



/**===========================================================================
** Synopsis:    VOID   TrnStoPutPostRToCons( VOID )
*
*     Input:    nothing
*    Output:    nothing
*
** Return:      nothing
*
** Description: copy data from post rec. storage file to consoli. storage file.
*               doing this copy to consolidated allows for any Post Receipt data
*               to be printed to the receipt printer.
*
*               SEE ALSO: complementary function to TrnStoGetConsToPostR() which
*                         will retrieve the data from the Post Receipt storage.
*===========================================================================*/
SHORT TrnStoPutPostRToCons( VOID )
{
    ULONG       ulPostRQualSize;
    ULONG	    ulReadLen;
    ULONG       ulWriteOffset;
    ULONG       ulReadOffset;
    USHORT      usTtlVliSize;
    ULONG       ulTtlReadLen;
    UCHAR       auchTranStorageWork[ TRN_TEMPBUFF_SIZE ];
    UCHAR       *puchTempBuff;
    PRTIDXHDR   IdxFileInfo;
    ULONG       ulTtlIdxReadSize;
    ULONG       ulCurIdxReadSize;
	ULONG		ulActualBytesRead;//RPH 11-11-3 SR# 201

    /* return, if post receipt file have been closed, V3.3 */

    if (TrnInformation.hsTranPostRecStorage < 0) {

        return (TRN_ERROR);
    }

    /* --- retrieve qualifier data and store it to trans. information --- */

    ulReadOffset    = sizeof( USHORT );
    ulPostRQualSize = sizeof( TRANGCFQUAL ) + sizeof( TRANCURQUAL );

    TrnReadFile( ulReadOffset, &( TrnInformation.TranGCFQual ), ulPostRQualSize, TrnInformation.hsTranPostRecStorage, &ulActualBytesRead );

    /* --- copy post receipt's transaction data to consolidate storage --- */

    puchTempBuff = &auchTranStorageWork[ 0 ];

    ulReadOffset += ulPostRQualSize;
    //RPH 11-11-3 SR# 201
	TrnReadFile( ulReadOffset, puchTempBuff, sizeof( auchTranStorageWork ), TrnInformation.hsTranPostRecStorage, &ulReadLen );

    ulWriteOffset  = sizeof( USHORT ) + sizeof( USHORT ) + sizeof( TRANGCFQUAL ) + sizeof( TRANITEMIZERS );
    TrnWriteFile( ulWriteOffset, puchTempBuff, ulReadLen, TrnInformation.hsTranConsStorage );

    TrnInformation.usTranConsStoVli = *(( USHORT * )puchTempBuff );

    usTtlVliSize = sizeof( USHORT ) + TrnInformation.usTranConsStoVli;
    ulTtlReadLen = ulReadLen;

    /* --- if post receipt's trans. data remains in post receipt file,
        copy remained data to consolidate storage --- */

    while ( ulTtlReadLen < usTtlVliSize ) {

        ulReadOffset  += ulReadLen;
        ulWriteOffset += ulReadLen;

        //RPH 11-11-3 SR# 201
		TrnReadFile( ulReadOffset, puchTempBuff, sizeof( auchTranStorageWork ), TrnInformation.hsTranPostRecStorage, &ulReadLen );

        TrnWriteFile( ulWriteOffset, puchTempBuff, ulReadLen, TrnInformation.hsTranConsStorage );

        ulTtlReadLen  += ulReadLen;
    }

    /* --- write print priority index to consolidate index file --- */

    //RPH 11-11-3 SR# 201
	TrnReadFile( (sizeof( USHORT ) + ulPostRQualSize + usTtlVliSize),
                 &IdxFileInfo,
                 sizeof( PRTIDXHDR ),
                 TrnInformation.hsTranPostRecStorage, &ulActualBytesRead); /* ### Mod (2171 for Win32) V1.0 */

    TrnWriteFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), TrnInformation.hsTranConsIndex );
    TrnWriteFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), TrnInformation.hsTranNoConsIndex );

    /* --- get print priority index, and save it to
        index file --- */

    ulReadOffset = sizeof( USHORT ) + ulPostRQualSize + usTtlVliSize + sizeof( PRTIDXHDR );
    ulTtlIdxReadSize = 0;
    ulCurIdxReadSize = sizeof( auchTranStorageWork );

    while ( ulTtlIdxReadSize < IdxFileInfo.usIndexVli ) {

        //RPH 11-11-3 SR# 201
		TrnReadFile( (ulReadOffset + ulTtlIdxReadSize),
                    puchTempBuff,
                    sizeof( auchTranStorageWork ),
                    TrnInformation.hsTranPostRecStorage, &ulCurIdxReadSize); /* ### Mod (2171 for Win32) V1.0 */


        if ( IdxFileInfo.usIndexVli < ( ulTtlIdxReadSize + ulCurIdxReadSize )) {
            ulCurIdxReadSize = IdxFileInfo.usIndexVli - ulTtlIdxReadSize;
        }

        TrnWriteFile( (sizeof( PRTIDXHDR ) + ulTtlIdxReadSize),
                      puchTempBuff,
                      ulCurIdxReadSize,
                      TrnInformation.hsTranConsIndex ); /* ### Mod (2171 for Win32) V1.0 */

        TrnWriteFile( (sizeof( PRTIDXHDR ) + ulTtlIdxReadSize),
                      puchTempBuff,
                      ulCurIdxReadSize,
                      TrnInformation.hsTranNoConsIndex ); /* ### Mod (2171 for Win32) V1.0 */

        ulTtlIdxReadSize += ulCurIdxReadSize;
    }

    return (TRN_SUCCESS);
}

/**===========================================================================
** Synopsis:    VOID   TrnSetConsNoToPostR( USHORT usConsecutiveNo )
*
*     Input:    USHORT  usConsecutiveNo - consecutive no. to store post rec.
*    Output:    nothing
*
** Return:      nothing
*
** Description: set specified consecutive no. to post receipt file.
*===========================================================================*/
VOID TrnSetConsNoToPostR( USHORT usConsecutiveNo )
{
    USHORT  usWriteOffset;

    /* --- store qualifier data to post receipt file --- */

    usWriteOffset = sizeof( USHORT ) +  sizeof( TRANGCFQUAL );

    TrnWriteFile( usWriteOffset, &usConsecutiveNo, sizeof( USHORT ), TrnInformation.hsTranPostRecStorage );
}

/*
*==========================================================================
**    Synopsis: SHORT   TrnChkStorageSize(SHORT sSize)
*
*       Input:  SHORT   sSize - data size to be written
*      Output:  Nothing
*       InOut:  Nothing
*
*    Return:    TRN_SUCCESS     : not file full
*               LDT_TAKETL_ADR  : file full
*
**  Description:
*           Check item and consoli. storage file size and input data size.
*==========================================================================
*/
static SHORT TrnChkStorageSizeWithOrWithoutReserve (SHORT sSize, USHORT usReserveCount)
{
    ULONG       ulInquirySize;
	ULONG		ulActualBytesRead;//RPH 11-11-3 SR# 201
    PRTIDXHDR   IdxFileInfo;
	USHORT      usSize = sSize;    // we want the size to be unsigned short

	if (sSize < 0) {
		return TRN_ERROR;
	}

	NHPOS_ASSERT(TrnInformation.usTranItemStoFSize > sizeof(TRANITEMSTORAGEHEADER));
	NHPOS_ASSERT(TrnInformation.usTranConsStoFSize > sizeof(TRANCONSSTORAGEHEADER));

    /* --- determine item index file has a vacant cell --- */
    //RPH 11-11-3 SR# 201
	TrnReadFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), TrnInformation.hsTranItemIndex, &ulActualBytesRead );

    ulInquirySize = ( sizeof( PRTIDXHDR ) + IdxFileInfo.usIndexVli + sizeof( PRTIDX ) * usReserveCount);
    if ( IdxFileInfo.usIndexFSize < ulInquirySize ) {
        return ( LDT_TAKETL_ADR );
    }

    /* --- determine no-cons item index file has a vacant cell --- */
    //RPH 11-11-3 SR# 201
	TrnReadFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), TrnInformation.hsTranNoItemIndex, &ulActualBytesRead );

    ulInquirySize = ( sizeof( PRTIDXHDR ) + IdxFileInfo.usIndexVli + sizeof( PRTIDX ) * usReserveCount);
    if ( IdxFileInfo.usIndexFSize < ulInquirySize ) {
        return ( LDT_TAKETL_ADR );
    }

    /* --- determine consolidate index file has a vacant cell --- */
    //RPH 11-11-3 SR# 201
	TrnReadFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), TrnInformation.hsTranConsIndex, &ulActualBytesRead );

    ulInquirySize = ( sizeof( PRTIDXHDR ) + IdxFileInfo.usIndexVli + sizeof( PRTIDX ) * usReserveCount);
    if ( IdxFileInfo.usIndexFSize < ulInquirySize ) {
        return ( LDT_TAKETL_ADR );
    }

    //RPH 11-11-3 SR# 201
	TrnReadFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), TrnInformation.hsTranNoConsIndex, &ulActualBytesRead );

    ulInquirySize = ( sizeof( PRTIDXHDR ) + IdxFileInfo.usIndexVli + sizeof( PRTIDX ) * usReserveCount);
    if ( IdxFileInfo.usIndexFSize < ulInquirySize ) {
        return ( LDT_TAKETL_ADR );
    }

    /* --- retrieve size of item storage index file --- */
    if (sizeof(TRANITEMSTORAGEHEADER) + TrnInformation.usTranItemStoVli + usSize > TrnInformation.usTranItemStoFSize) {
        return(LDT_TAKETL_ADR);                         /* item storage full */
    }

    /* --- retrieve size of consolidate storage index file --- */
    if (sizeof(TRANCONSSTORAGEHEADER) + IdxFileInfo.usIndexFSize + TrnInformation.usTranConsStoVli + usSize > TrnInformation.usTranConsStoFSize) { /* ### Mod V1.0 */
        return(LDT_TAKETL_ADR);                         /* consoli storage full */
    }
    return(TRN_SUCCESS);
}

SHORT TrnChkStorageSize (SHORT sSize)
{
	return TrnChkStorageSizeWithOrWithoutReserve (sSize, 2);
}

SHORT TrnChkStorageSizeIfSales (SHORT sSize)
{
	return TrnChkStorageSizeWithOrWithoutReserve (sSize, 5);
}


/*
*==========================================================================
**   Synopsis:  SHORT TrnStoSalesSearch( ITEMSALES          *pItemSales,
*                                        UCHAR              *puchSrcZip,
*                                        TRANSTOSALESSRCH   *pNormalItem,
*                                        TRANSTOSALESSRCH   *pVoidItem,
*                                        SHORT              sStorageType,
*                                        SHORT              sType )
*
*       Input:  ITEMSALES        *pItemSales  - address of unzipped data
*               UCHAR            *puchSrcZip  - address of zipped source data
*               TRANSTOSALESSRCH *pNormalItem - address of normal item
*               TRANSTOSALESSRCH *pVoidItem   - address of void item
*               SHORT            sStorageType - type of transaction storage
*                   TRN_TYPE_ITEMSTORAGE - item storage
*                   TRN_TYPE_CONSSTORAGE - consolidation storage
*               SHORT            sType        - 0:Normal, -1:Reduce
*      Output:  Nothing
*       InOut:  Nothing
*
*      Return:  TRN_SUCCESS     : item found and consolidate it
*               TRN_ERROR       : not found
*
**  Description:
*           search target "item" at item/consoli. storage
*           if find it, consolidate item/consoli. storage         R3.1
*==========================================================================
*/
SHORT TrnStoSalesSearch( ITEMSALES          *pItemSales,
                         UCHAR              *puchSrcZip,
                         TRANSTOSALESSRCH   *pNormalItem,
                         TRANSTOSALESSRCH   *pVoidItem,
                         TrnStorageType     sStorageType,
                         SHORT              sType )
{
    TRANSTORAGESALESNON *pTranNon;
    ULONG   ulReadPosition;
    ULONG   ulMaxStoSize;
    BOOL    fNormalItemFound,fInitialLoop;
    BOOL    fVoidItemFound;
    SHORT   sReturn;
	SHORT   hsStorage;
    USHORT  usStorageVli;
    USHORT  usStorageHdrSize;
    USHORT  usSrcZipSize;
    USHORT  usTargetItemSize;
    USHORT  usUnzipSize;
    USHORT  usZipSize;
	USHORT	us = 0;
	SHORT	sCompareResult = 0;
	ITEMSALES	ItemSales1 = {0};
	ITEMSALES	ItemSales2 = {0};
    UCHAR   auchTranStorageWork[ TRN_STOSIZE_BLOCK ];

    sReturn = TrnStoGetStorageInfo( sStorageType, &hsStorage, &usStorageHdrSize, &usStorageVli );
    if (sReturn != TRN_SUCCESS) {
        return ( sReturn );
    }

    /* --- calculate size of source sales item data --- */
    usUnzipSize  = ( USHORT )( *puchSrcZip );
    usZipSize    = *(( USHORT * )( puchSrcZip + usUnzipSize ));
    usSrcZipSize = ( usUnzipSize + usZipSize );

    pNormalItem->usItemOffset = 0;
    pNormalItem->usItemSize = 0;
	pNormalItem->pDiscBuffer = 0;
	pNormalItem->usDiscOffset = 0;
    pVoidItem->usItemOffset = 0;
    pVoidItem->usItemSize = 0;
	pVoidItem->pDiscBuffer = 0;
	pVoidItem->usDiscOffset = 0;

    fNormalItemFound  = FALSE;
    fVoidItemFound    = FALSE;
	fInitialLoop	  = TRUE;

    ulReadPosition    = usStorageHdrSize;
    ulMaxStoSize      = ulReadPosition + usStorageVli;

    pTranNon = TRANSTORAGENONPTR( auchTranStorageWork );

    while (( ulReadPosition < ulMaxStoSize ) && ( (fNormalItemFound == FALSE) || (fVoidItemFound == FALSE))) {
		BOOL    bIsDiscountedItem = FALSE;
		USHORT  usDiscOffset = 0;

		us++;
        /* --- read item data, and store to work buffer --- */
 		sReturn = TrnStoIsItemInWorkBuff(hsStorage, ulReadPosition, auchTranStorageWork, sizeof( auchTranStorageWork ), &usTargetItemSize);
		if (sReturn == TRN_ERROR) {
			NHPOS_ASSERT_TEXT((sReturn != TRN_ERROR), "**ERROR: TrnStoIsItemInWorkBuff() item record size exceeds buffer");
			return TRN_ERROR;
		}

        /* --- compare read item with source sales item --- */
		if (RflIsSalesItemDisc(pTranNon)) {
			USHORT usTemp = RflGetStorageItemLen (auchTranStorageWork + usTargetItemSize, -1);

			usDiscOffset = usTargetItemSize + 1;
			usTargetItemSize += usTemp;
			bIsDiscountedItem = TRUE;
		}

		if (pItemSales->uchMajorClass == CLASS_ITEMSALES && pItemSales->uchMinorClass == CLASS_ITEMORDERDEC) {
			// In order to allow edit of an Order Declare by only a change of the uncompressed portion of the ITEMSALES
			// record, we are storing the Order Declare number in the usDeptNo field, which is part of the
			// uncompressed portion of the record, TRANSTORAGESALESNON, so we do not need to do the uncompress and
			// recompress of the compressed portion of the record, TRANSTORAGESALES, in order to update the
			// uchAdjective member. We do this to avoid changes in compressed data which may result in the compression
			// changing the length of the compressed portion of the record.
			//    Richard Chambers, Dec-21-2018
			if (pTranNon->uchMajorClass == CLASS_ITEMSALES && pTranNon->uchMinorClass == CLASS_ITEMORDERDEC) {
                pNormalItem->usItemOffset = ulReadPosition;
				pNormalItem->usItemSize = usTargetItemSize;
                memcpy( pNormalItem->auchBuffer, auchTranStorageWork, usTargetItemSize );
                fNormalItemFound = TRUE;
				break;
			}
		}

		// Any changes to this logic should be tested with Void Consolidation (MDC 235) turned on and turned off to double
		// check that receipt printing is not affected by testing receipt printing.  In addition any changes may affect the
		// output of the Connection Engine Interface which also does Void Consolidation.
		// Also review the source of function TrnStoSalesSearchVoidItem() for any necessary changes.
		if ((pTranNon->fsLabelStatus & ITM_CONTROL_RETURN_OLD) == (pItemSales->fsLabelStatus & ITM_CONTROL_RETURN_OLD)) {
        if ( (TrnIsSameZipDataSalesNon ( puchSrcZip, auchTranStorageWork) &&
				((pTranNon->fsLabelStatus & ITM_CONTROL_PCHG) == 0) &&
              ( ! (pTranNon->fbReduceStatus & REDUCE_ITEM) || ((pTranNon->fbReduceStatus & REDUCE_ITEM) && (sType != 0))) ) ) {   /* 5/27/96 */
		if ( (pItemSales->fsLabelStatus & ITM_CONTROL_NO_CONSOL) && (pTranNon->usUniqueID != pItemSales->usUniqueID || pTranNon->uchMajorClass != pItemSales->uchMajorClass || pTranNon->uchMinorClass != pItemSales->uchMinorClass ) ) {
			;    // we will do nothing in this case so just ignore it.
		} else {
            if ( memcmp (puchSrcZip + usUnzipSize, auchTranStorageWork + usUnzipSize, usZipSize) == 0 ||  /* zipped data is same */
					TrnIsTranSameAsItem(auchTranStorageWork, pItemSales) ) {
                /* --- consolidate source item to item which is already stored --- */
                /*----- Normal, R3.1 -----*/
				if (pTranNon->lQTY == 0) {
					// Indicate item has zero quantity which means that this
					// item does not have any indexes point to it, more than likely.
					pItemSales->fsLabelStatus |= ITM_CONTROL_ZERO_QTY;
				}

                if ( pTranNon->fsLabelStatus & ITM_CONTROL_VOIDITEM ) {
                    /* --- declare void item already exists in transaction storage,
                        and store it to buffer --- */
					if (usDiscOffset > 0) {
						pVoidItem->usDiscOffset = usDiscOffset;
						pVoidItem->pDiscBuffer = pVoidItem->auchBuffer + usDiscOffset;
					}
                    pVoidItem->usItemOffset = ulReadPosition;
					pVoidItem->usItemSize = usTargetItemSize;
                    memcpy( pVoidItem->auchBuffer, auchTranStorageWork, usTargetItemSize );
                    fVoidItemFound = TRUE;
                } else {                                        /* source item is normal */
                    /* --- declare normal item already exists in transaction storage,
                        and store it to buffer --- */
					if (usDiscOffset > 0) {
						pNormalItem->usDiscOffset = usDiscOffset;
						pNormalItem->pDiscBuffer = pNormalItem->auchBuffer + usDiscOffset;
					}
                    pNormalItem->usItemOffset = ulReadPosition;
					pNormalItem->usItemSize = usTargetItemSize;
                    memcpy( pNormalItem->auchBuffer, auchTranStorageWork, usTargetItemSize );
                    fNormalItemFound = TRUE;
                }
            } else if (TrnStoIsPairItem( pItemSales, auchTranStorageWork ) > 0) 
			{
                /* --- this target item is pair of source item.
                    ( SRC = normal , TRG = void ) or ( SRC = void, TRG = normal ) --- */
                if ( ( pTranNon->fsLabelStatus & ITM_CONTROL_VOIDITEM ) == 0 ){
                    /* --- declare normal item already exists in transaction storage,
                        and store it to buffer --- */
					if (usDiscOffset > 0) {
						pNormalItem->usDiscOffset = usDiscOffset;
						pNormalItem->pDiscBuffer = pNormalItem->auchBuffer + usDiscOffset;
					}
                    pNormalItem->usItemOffset = ulReadPosition;
					pNormalItem->usItemSize = usTargetItemSize;
                    memcpy( pNormalItem->auchBuffer, auchTranStorageWork, usTargetItemSize );
                    fNormalItemFound = TRUE;
                } else {                                        /* source item is normal */
                    /* --- declare void item already exists in transaction storage,
                        and store it to buffer --- */
					if (usDiscOffset > 0) {
						pVoidItem->usDiscOffset = usDiscOffset;
						pVoidItem->pDiscBuffer = pVoidItem->auchBuffer + usDiscOffset;
					}
                    pVoidItem->usItemOffset = ulReadPosition;
					pVoidItem->usItemSize = usTargetItemSize;
                    memcpy( pVoidItem->auchBuffer, auchTranStorageWork, usTargetItemSize );
                    fVoidItemFound = TRUE;
                }
            }
        }
        }
		}

        /* --- increment read position to read next work buffer --- */
        ulReadPosition += usTargetItemSize;
    }


	// Following code is to be used for those PLUs that are set
	// to not consolidate even if there are instances of the PLU
	// already in the transaction file.
	// This functionality added as a prototype to address problems
	// with receipt printout for Gift Cards which were not providing
	// the information for individual gift card purchases since
	// multiple gift cards of the same denomination were being
	// consolidated in the transaction file.
	//   if((pItemSales->ControlCode.auchStatus[PLU_CONTROLCODE_ADR_7] & PLU_GIFT_CARD) || 
	//	  (pItemSales->ControlCode.auchStatus[PLU_CONTROLCODE_ADR_7] & PLU_GIFT_RELOAD))
	//  Richard Chambers, June 16, 2008
	if ((pItemSales->fsLabelStatus & ITM_CONTROL_NO_CONSOL) && (sType == 0) ) {
		if ((pItemSales->fsLabelStatus & ITM_CONTROL_VOIDITEM) == 0) {
			pNormalItem->usItemOffset = 0;
			pNormalItem->usItemSize = 0;
			pVoidItem->usItemOffset = 0;
			pVoidItem->usItemSize = 0;
#if 0
	// we now allow the consolidation of voids on items that are marked as no consolidation
	// in order to accomodate Amtrak and use of quantity void of discounted items.
	// this change affects a number of no consolidation items such as open department.
	//     Richard Chambers, May 16, 2013
		} else {
			pVoidItem->usOffset = 0;
			pVoidItem->usItemSize = 0;
#endif
		}
	}

	if ((pItemSales->fsLabelStatus & ITM_CONTROL_RETURN_OLD) && (pItemSales->fbModifier & (RETURNS_ORIGINAL | VOID_MODIFIER)) == VOID_MODIFIER) {
		if (pVoidItem->usItemOffset) {
			pTranNon = TRANSTORAGENONPTR( pVoidItem->auchBuffer );
			if (pTranNon->fsLabelStatus & ITM_CONTROL_RETURNORIG) {
				pVoidItem->usItemOffset = 0;
				pVoidItem->usItemSize = 0;
			}
		}
	}

    return ( TRN_SUCCESS );
}

SHORT TrnStoSalesSearchIndex( ITEMSALES          *pItemSales,
                              UCHAR              *puchSrcZip,
                              TRANSTOSALESSRCH   *pNormalItemCons,
                              TRANSTOSALESSRCH   *pVoidItemCons,
                              TRANSTOSALESSRCH   *pSameItemCons,
                              TRANSTOSALESSRCH   *pNormalItem,
                              TRANSTOSALESSRCH   *pVoidItem,
                              TRANSTOSALESSRCH   *pSameItem,
                              TrnStorageType     sStorageType,
                              SHORT              sType )
{
    TRANSTORAGESALESNON *pTranNon;
    UCHAR   *puchWorkBuffer;
	ULONG		ulActualBytesRead;//RPH 11-11-3 SR# 201
    SHORT       sMiddleIdx;
    SHORT   sReturn;
	SHORT   hsStorage;
	SHORT   hsIndexFile;
	SHORT   hsNoIndexFile;
    USHORT  usStorageVli;
    USHORT  usStorageHdrSize;
    USHORT  usSrcZipSize;
    USHORT  usTargetItemSize;
    USHORT  usUnzipSize;
    USHORT  usZipSize;
    PRTIDXHDR   IdxFileInfo;
    PRTIDX      PrtIdxMid;
    UCHAR   auchTranStorageWork[ TRN_TEMPBUFF_SIZE ];

    sReturn = TrnStoGetStorageIndexInfo( sStorageType, &hsStorage, &hsIndexFile, &hsNoIndexFile, &usStorageHdrSize, &usStorageVli );
    if (sReturn != TRN_SUCCESS) {
        return ( sReturn );
    }

	TrnReadFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), hsNoIndexFile , &ulActualBytesRead);

    /* --- calculate size of source sales item data --- */
    usUnzipSize  = ( USHORT )( *puchSrcZip );
    usZipSize    = *(( USHORT * )( puchSrcZip + usUnzipSize ));
    usSrcZipSize = ( usUnzipSize + usZipSize );

    pNormalItemCons->usItemOffset = 0;
	pNormalItemCons->pDiscBuffer = 0;
	pNormalItemCons->usDiscOffset = 0;
    pNormalItem->usItemOffset = 0;
	pNormalItem->pDiscBuffer = 0;
	pNormalItem->usDiscOffset = 0;
    pVoidItemCons->usItemOffset = 0;
	pVoidItemCons->pDiscBuffer = 0;
	pVoidItemCons->usDiscOffset = 0;
    pVoidItem->usItemOffset = 0;
	pVoidItem->pDiscBuffer = 0;
	pVoidItem->usDiscOffset = 0;
    pSameItemCons->usItemOffset = 0;
	pSameItemCons->pDiscBuffer = 0;
	pSameItemCons->usDiscOffset = 0;
    pSameItem->usItemOffset = 0;
	pSameItem->pDiscBuffer = 0;
	pSameItem->usDiscOffset = 0;

    puchWorkBuffer = &auchTranStorageWork[ 0 ];
	pTranNon = TRANSTORAGENONPTR( auchTranStorageWork );

	for (sMiddleIdx = 0; sMiddleIdx < IdxFileInfo.uchNoOfItem; sMiddleIdx++) {
		TrnReadFile( PRTIDX_OFFSET(sMiddleIdx), &PrtIdxMid, sizeof(PRTIDX), hsNoIndexFile, &ulActualBytesRead );

		// Okay, we look for an existing item so
		// we replace it with the new one.  We know that the
		// item in pItemSales is uchMajorClass of CLASS_ITEMSALES so
		// we can count on the usUniqueID of being a good unique identifier.
		if (PrtIdxMid.uchMajorClass == pItemSales->uchMajorClass && PrtIdxMid.uchMinorClass == pItemSales->uchMinorClass) {
			if (PrtIdxMid.usUniqueID == pItemSales->usUniqueID) {
				/* --- read item data, and store to work buffer --- */
 				sReturn = TrnStoIsItemInWorkBuff(hsStorage, PrtIdxMid.usItemOffset, puchWorkBuffer, sizeof( auchTranStorageWork ), &usTargetItemSize);
				if (sReturn == TRN_ERROR) {
					NHPOS_ASSERT_TEXT((sReturn != TRN_ERROR), "**ERROR: TrnStoIsItemInWorkBuff() item record size exceeds buffer");
					return TRN_ERROR;
				}

				if (RflIsSalesItemDisc(pTranNon)) {
					USHORT usTemp = puchWorkBuffer [usTargetItemSize];

					usTargetItemSize += usTemp;
					usTargetItemSize += *(USHORT *)(puchWorkBuffer + usTargetItemSize);
				}

				/* --- compare read item with source sales item --- */
				if ( pTranNon->fsLabelStatus & ITM_CONTROL_VOIDITEM ) { /* source item is void item */
					/* --- declare void item already exists in transaction storage,
						and store it to buffer --- */
					pVoidItem->usItemOffset = PrtIdxMid.usItemOffset;
					pVoidItem->usItemSize = usTargetItemSize;
					memcpy( pVoidItem->auchBuffer, puchWorkBuffer, usTargetItemSize );
				} else {                                        /* source item is normal */
					/* --- declare normal item already exists in transaction storage,
						and store it to buffer --- */
					pNormalItem->usItemOffset = PrtIdxMid.usItemOffset;
					pNormalItem->usItemSize = usTargetItemSize;
					memcpy( pNormalItem->auchBuffer, puchWorkBuffer, usTargetItemSize );
				}
			}
			else if (PrtIdxMid.usUniqueID > 0 && memcmp (PrtIdxMid.auchPLUNo, pItemSales->auchPLUNo, sizeof(PrtIdxMid.auchPLUNo)) == 0) {
 				sReturn = TrnStoIsItemInWorkBuff(hsStorage, PrtIdxMid.usItemOffset, puchWorkBuffer, sizeof( auchTranStorageWork ), &usTargetItemSize);
				if (sReturn == TRN_ERROR) {
					NHPOS_ASSERT_TEXT((sReturn != TRN_ERROR), "**ERROR: TrnStoIsItemInWorkBuff() item record size exceeds buffer");
					return TRN_ERROR;
				}

				if (RflIsSalesItemDisc(pTranNon)) {
					USHORT usTemp = puchWorkBuffer [usTargetItemSize];

					usTargetItemSize += usTemp;
					usTargetItemSize += *(USHORT *)(puchWorkBuffer + usTargetItemSize);
				}

				/* --- compare read item with source sales item --- */
				if (TrnIsTranSameAsItem(puchWorkBuffer, pItemSales)) {
					/* --- declare normal item already exists in transaction storage,
						and store it to buffer --- */
					pSameItem->usItemOffset = PrtIdxMid.usItemOffset;
					pSameItem->usItemSize = usTargetItemSize;
					memcpy( pSameItem->auchBuffer, puchWorkBuffer, usTargetItemSize );
				}
			}
        }
    }

    return ( TRN_SUCCESS );
}

/*
*==========================================================================
**   Synopsis:  SHORT TrnStoSalesUpdate( ITEMSALES *pItemSales,
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
**  Description:  This function will update the transaction file with the specified
*                 information.  Before calling this function, you should call function
*                 TrnStoSalesSearch() which is used to see if the specified item
*                 already exists in the transaction file or it's void exists.
*           
*==========================================================================
*/
SHORT TrnStoSalesUpdate( ITEMSALES          *pItemSales,
                         TRANSTOSALESSRCH   *pCompItem,
                         TRANSTOSALESSRCH   *pNormalItem,
                         TRANSTOSALESSRCH   *pVoidItem,
                         TrnStorageType     sStorageType )
{
    TRANSTORAGESALESNON *pNoCompNormal = TRANSTORAGENONPTR( pNormalItem->auchBuffer );
    TRANSTORAGESALESNON *pNoCompVoid = TRANSTORAGENONPTR( pVoidItem->auchBuffer );
    TRANSTORAGESALESNON *pNoCompSrc = TRANSTORAGENONPTR( pCompItem->auchBuffer );
    SHORT               sSrcCpnQTY;
    SHORT               hsStorage;
    USHORT              usStorageVli;
    USHORT              usStorageHdrSize;
    USHORT              usSrcZipSize;
    USHORT              usNoCompSize;
    USHORT              usCompSize;
    USHORT              usItemOffset;
    SHORT               sReturn;
    BOOL                fOverCoupon;

    sReturn = TrnStoGetStorageInfo( sStorageType, &hsStorage, &usStorageHdrSize, &usStorageVli );
    if (sReturn != TRN_SUCCESS ) {
        return ( sReturn );
    }

    usNoCompSize = ( USHORT )( pCompItem->auchBuffer[0] );
    usCompSize   = *(( USHORT * )( pCompItem->auchBuffer + usNoCompSize ));
    usSrcZipSize = ( usNoCompSize + usCompSize );

    /* --- calculate sales item's coupon quantity --- */
    /* --- store source coupon quantity --- */
    sSrcCpnQTY = pNoCompSrc->sCouponQTY;

    /* --- R3.1 --- */
    usItemOffset = usStorageHdrSize + usStorageVli;

    if ( pNormalItem->usItemOffset ) {
        if ( pVoidItem->usItemOffset ) {    /* normal && void */
            pNoCompNormal->sCouponQTY = ( SHORT )(( pNoCompNormal->lQTY + pNoCompVoid->lQTY ) / 1000 );

            /*----- Save Offset of Void Item,   R3.1 -----*/
			//SR 597. If the user is storing the item, this offset does not need to be set.
			//If we do set it, it will look in the wrong location of the Transaction file.JHHJ
			if((TranCurQualPtr->fsCurStatus & CURQUAL_SERVTOTAL) != CURQUAL_SERVTOTAL)
			{
				pNoCompNormal->usItemOffset = pVoidItem->usItemOffset;
			}else
			{
				pNoCompNormal->usItemOffset = 0;
			}
        } else {                        /* normal && ! void */
            if ((pItemSales->fbModifier & VOID_MODIFIER) && (pItemSales->fbModifier & VOID_NO_MODIFY_QTY) == 0) {
                pNoCompNormal->sCouponQTY = (SHORT)(( pNoCompNormal->lQTY + pItemSales->lQTY ) / 1000 );

                /*----- Save Offset of 1'st Void Item,   R3.1 -----*/
				//SR 597. If the user is storing the item, this offset does not need to be set.
				//If we do set it, it will look in the wrong location of the Transaction file.JHHJ

				if(((TranCurQualPtr->fsCurStatus & CURQUAL_SERVTOTAL) != CURQUAL_SERVTOTAL) && (pItemSales->fbModifier & VOID_MODIFIER ))
				{
					pNoCompNormal->usItemOffset = usItemOffset;
				}else
				{
					pNoCompNormal->usItemOffset = 0;
				}
            } else {
                pNoCompNormal->sCouponQTY = ( SHORT )( pNoCompNormal->lQTY / 1000 );
            }
        }

        /* --- update normal item data --- */
		if (RflIsSalesItemDisc(pNoCompNormal)) {
			ITEMDISC  *pItemDisc = 0;
			USHORT    usItemSize = 0;
			
			usItemSize = ( USHORT )( pNormalItem->auchBuffer[0] );
			usItemSize += *(( USHORT * )( pNormalItem->auchBuffer + usItemSize ));
			pItemDisc = (ITEMDISC *)(((UCHAR *)pNoCompNormal) + usItemSize);
		}

		if (pNoCompSrc->uchMinorClass != pNoCompNormal->uchMinorClass) {
			// check to see if this item whose minor class has changed contains all of the consolidated
			// items or only some of the consolidated items.
			// this code primarily for handling Item Discount (CLASS_ITEMDISC1) after selecting an item
			// by moving the cursor then doing pressing an Item Discount key.
			if (pNoCompNormal->lQTY == pNoCompSrc->lQTY) {
				pNoCompNormal->fbReduceStatus  |= REDUCE_ITEM;
			}
			else {
				// we need to back out the quantity, price, etc.
				pNoCompNormal->lQTY -= pNoCompSrc->lQTY;
				pNoCompNormal->lProduct -= pNoCompSrc->lProduct;
				pNoCompNormal->sCouponQTY -= pNoCompSrc->sCouponQTY;
				if (pNoCompNormal->lQTY == 0) {
					pNoCompNormal->fbReduceStatus  |= REDUCE_ITEM;
				}
			}

            if ( sStorageType == TRN_TYPE_ITEMSTORAGE ) {
                pCompItem->usItemOffset = TrnStoItemNormalItemOnly ( pCompItem->auchBuffer, usSrcZipSize );
				TrnStoItemNormalUpdateIndex( pNoCompSrc, pCompItem->usItemOffset );
            } else if (sStorageType == TRN_TYPE_CONSSTORAGE) {    /* R3.1 */
                pCompItem->usItemOffset = TrnStoConsNormalItemOnly ( pCompItem->auchBuffer, usSrcZipSize );
				TrnStoConsNormalUpdateIndex( pNoCompSrc, pCompItem->usItemOffset );
			}
		}

        TrnWriteFile( pNormalItem->usItemOffset, pNormalItem->auchBuffer, sizeof( UCHAR ) + sizeof( TRANSTORAGESALESNON ), hsStorage );

		if (pNoCompSrc->uchMinorClass != pNoCompNormal->uchMinorClass) {
			pNormalItem->usItemOffset = 0;
		}
		else {

			/* --- determine current linked coupon item is over than coupon quantity --- */
			fOverCoupon = FALSE;
			if ( pNoCompNormal->sCouponQTY < 0 ) {
				if ( 0 < pNoCompNormal->sCouponCo ) {
					fOverCoupon = TRUE;
				} else
				if ( pNoCompNormal->sCouponCo < pNoCompNormal->sCouponQTY ) {
					fOverCoupon = TRUE;
				}
			} else {    /* 0 <= CouponQTY */
				if ( pNoCompNormal->sCouponCo < 0 ) {
					fOverCoupon = TRUE;
				} else
				if ( pNoCompNormal->sCouponQTY < pNoCompNormal->sCouponCo ) {
					fOverCoupon = TRUE;
				}
			}

			if ( fOverCoupon ) {
				ULONG   fulTrnPrtSts = TranCurQualPtr->flPrintStatus;
				ULONG   fusTrnCurSts = TranCurQualPtr->fsCurStatus;
				SHORT   fPostChkGCOpe;
				SHORT   fStoreRecall;

				fPostChkGCOpe = ( fulTrnPrtSts & CURQUAL_POSTCHECK ) && (( fusTrnCurSts & CURQUAL_OPEMASK ) != CURQUAL_CASHIER );
				fStoreRecall = (( fulTrnPrtSts & CURQUAL_GC_SYS_MASK ) == CURQUAL_STORE_RECALL );

				if ( fStoreRecall ) {
					TrnStoVoidCouponItem( pItemSales, pNormalItem->auchBuffer, pNormalItem->usItemOffset, TRN_TYPE_CONSSTORAGE );
				} else {
					if (( sStorageType == TRN_TYPE_ITEMSTORAGE ) && ( ! fPostChkGCOpe )) {
						/* --- void coupon item --- */
						TrnStoVoidCouponItem( pItemSales, pNormalItem->auchBuffer, pNormalItem->usItemOffset, sStorageType );
					} else if (( sStorageType == TRN_TYPE_CONSSTORAGE ) && ( fPostChkGCOpe )) {
						/* --- void coupon item --- */
						TrnStoVoidCouponItem( pItemSales, pNormalItem->auchBuffer, pNormalItem->usItemOffset, sStorageType );
					}
				}

				/* --- R3.1 --- */
				if ( pItemSales->fbModifier & VOID_MODIFIER ) {
					/*----- Reset Offset of 1'st Void Item,   R3.1 -----*/
					pNoCompNormal->usItemOffset = usItemOffset;

					/* --- update normal item data --- */
					TrnWriteFile( pNormalItem->usItemOffset, pNormalItem->auchBuffer, sizeof( UCHAR ) + sizeof( TRANSTORAGESALESNON ), hsStorage );
				}
			}

			/* --- update void item data --- */
			if ( pItemSales->fbModifier & VOID_MODIFIER ) {
				if ( pVoidItem->usItemOffset ) {    /* void item is found */
					/*----- Save Offset of Normal Item,   R3.1 -----*/
					TrnWriteFile( pVoidItem->usItemOffset, pVoidItem->auchBuffer, pVoidItem->usItemSize, hsStorage );

				} else {                        /* void item is not found */
					TRANSTORAGESALESNON  *pSalesNon = TRANSTORAGENONPTR(pCompItem->auchBuffer);

					/*----- Save Offset of Normal Item,   R3.1 -----*/
					pSalesNon->usItemOffset = pNormalItem->usItemOffset;

					if ( sStorageType == TRN_TYPE_ITEMSTORAGE ) {
						TrnStoItemNormal( pCompItem->auchBuffer, usSrcZipSize );
					} else if (sStorageType == TRN_TYPE_CONSSTORAGE) {   /* R3.1 */
						TrnStoConsNormal( pCompItem->auchBuffer, usSrcZipSize );
					} else {                                             /* R3.1 */
						TrnStoConsNormalSpl(pCompItem->auchBuffer, usSrcZipSize, sStorageType);
					}
				}
			}
		}
    } else {    /* normal item does not exist in transaction storage */
        if ( pVoidItem->usItemOffset ) {    /* ! normal && void */
            if ( pItemSales->fbModifier & VOID_MODIFIER ) {
                pNoCompVoid->sCouponQTY = ( SHORT )( pNoCompVoid->lQTY / 1000 );
            } else {
                /*----- Save Offset of Normal Item,   R3.1 -----*/
                pNoCompSrc->sCouponQTY += ( SHORT )( pNoCompVoid->lQTY / 1000 );
                pNoCompSrc->usItemOffset = pVoidItem->usItemOffset;

                /*----- Save Offset of Void Item,   R3.1 -----*/
                pNoCompVoid->sCouponQTY += ( SHORT )( pItemSales->lQTY / 1000 );
                pNoCompVoid->usItemOffset = usItemOffset;
                TrnWriteFile( pVoidItem->usItemOffset, pVoidItem->auchBuffer, sizeof(UCHAR) + sizeof(TRANSTORAGESALESNON), hsStorage);
				if (pVoidItem->pDiscBuffer && pVoidItem->usDiscOffset > 0) {
					TrnWriteFile( pVoidItem->usItemOffset + pVoidItem->usDiscOffset, pVoidItem->pDiscBuffer, sizeof(TRANSTORAGEDISCNON), hsStorage);
				}
            }
        } else {                        /* ! normal && ! void */
            ;   /* do nothing */
        }

        if ( pItemSales->fbModifier & VOID_MODIFIER ) {
            if ( pVoidItem->usItemOffset ) {    /* void item is found */
                TrnWriteFile( pVoidItem->usItemOffset, pVoidItem->auchBuffer, sizeof( UCHAR ) + sizeof( TRANSTORAGESALESNON ), hsStorage );
				if (pVoidItem->pDiscBuffer && pVoidItem->usDiscOffset > 0) {
					TrnWriteFile( pVoidItem->usItemOffset + pVoidItem->usDiscOffset, pVoidItem->pDiscBuffer, sizeof(TRANSTORAGEDISCNON), hsStorage);
				}
            } else {                        /* void item is not found */
                if ( sStorageType == TRN_TYPE_ITEMSTORAGE ) {
                    TrnStoItemNormal( pCompItem->auchBuffer, usSrcZipSize );
                } else if (sStorageType == TRN_TYPE_CONSSTORAGE) {   /* R3.1 */
                    TrnStoConsNormal( pCompItem->auchBuffer, usSrcZipSize );
                } else {                                             /* R3.1 */
                    TrnStoConsNormalSpl(pCompItem->auchBuffer, usSrcZipSize, sStorageType);
                }
            }
        } else {    /* source item is normal item */
            if ( sStorageType == TRN_TYPE_ITEMSTORAGE ) {
                TrnStoItemNormal( pCompItem->auchBuffer, usSrcZipSize );
            } else if (sStorageType == TRN_TYPE_CONSSTORAGE) {    /* R3.1 */
                TrnStoConsNormal( pCompItem->auchBuffer, usSrcZipSize );
            }else if(sStorageType == TRN_TYPE_PREVIOUS_CONDIMENT){
			//	TrnStoPreviousCondiment(puchSrcZip, usSrcZipSize);
			}else {                                              /* R3.1 */
                TrnStoConsNormalSpl(pCompItem->auchBuffer, usSrcZipSize, sStorageType);
            }
        }
    }

    /* --- restore source coupon quantity --- */
    pNoCompSrc->sCouponQTY = sSrcCpnQTY;

    return ( TRN_SUCCESS );
}


SHORT TrnStoSalesUpdateIndex( ITEMSALES     *pItemSales,
                         TRANSTOSALESSRCH   *pCompItem,
                         TRANSTOSALESSRCH   *pNormalItem,
                         TRANSTOSALESSRCH   *pVoidItem,
                         TRANSTOSALESSRCH   *pSameItem,
                         TrnStorageType     sStorageType )
{
	BOOL                bSplitItem = FALSE;
    TRANSTORAGESALESNON *pNoCompNormal = TRANSTORAGENONPTR( pNormalItem->auchBuffer );
    TRANSTORAGESALESNON *pNoCompVoid = TRANSTORAGENONPTR( pVoidItem->auchBuffer );
    TRANSTORAGESALESNON *pNoCompSame = TRANSTORAGENONPTR( pSameItem->auchBuffer );
    TRANSTORAGESALESNON *pNoCompSrc = TRANSTORAGENONPTR( pCompItem->auchBuffer );
    SHORT               sSrcCpnQTY;
    SHORT               hsStorage;
    USHORT              usStorageVli;
    USHORT              usStorageHdrSize;
    USHORT              usSrcZipSize;
    USHORT              usNoCompSize;
    USHORT              usCompSize;
    USHORT              usItemOffset;
    SHORT               sReturn;
    BOOL                fOverCoupon;

    sReturn = TrnStoGetStorageInfo( sStorageType, &hsStorage, &usStorageHdrSize, &usStorageVli );
    if (sReturn != TRN_SUCCESS ) {
        return ( sReturn );
    }

    usNoCompSize = ( USHORT )( pCompItem->auchBuffer[0] );
    usCompSize   = *(( USHORT * )( pCompItem->auchBuffer + usNoCompSize ));
    usSrcZipSize = ( usNoCompSize + usCompSize );

    /* --- calculate sales item's coupon quantity --- */
    /* --- store source coupon quantity --- */
    sSrcCpnQTY = pNoCompSrc->sCouponQTY;

    /* --- R3.1 --- */
    usItemOffset = usStorageHdrSize + usStorageVli;

    if ( pNormalItem->usItemOffset ) {
		SHORT  sCouponQTY_Temp = pNoCompNormal->sCouponQTY;
		SHORT  sCouponCo_Temp  = pNoCompNormal->sCouponCo;

		if ( pVoidItem->usItemOffset ) {    /* normal && void */
            pNoCompNormal->sCouponQTY = ( SHORT )(( pNoCompNormal->lQTY + pNoCompVoid->lQTY ) / 1000 );
        } else {                        /* normal && ! void */
            if (pItemSales->fbModifier & VOID_MODIFIER) {
                pNoCompNormal->sCouponQTY = (SHORT)(( pNoCompNormal->lQTY + pItemSales->lQTY ) / 1000 );
            } else {
                pNoCompNormal->sCouponQTY = ( SHORT )( pNoCompNormal->lQTY / 1000 );
            }
        }

		sCouponQTY_Temp = pNoCompNormal->sCouponQTY;
		sCouponCo_Temp  = pNoCompNormal->sCouponCo;

        if ( pItemSales->fbModifier & VOID_MODIFIER ) {
            if ( pVoidItem->usItemOffset ) {    /* void item is found */
                TrnWriteFile( pVoidItem->usItemOffset, pVoidItem->auchBuffer, sizeof( UCHAR ) + sizeof( TRANSTORAGESALESNON ), hsStorage );

                if ( sStorageType == TRN_TYPE_ITEMSTORAGE ) {
                    TrnStoNoItemNormal( pCompItem->auchBuffer, pVoidItem->usItemOffset );
                }
                if ( sStorageType == TRN_TYPE_CONSSTORAGE ) {
                    TrnStoNoConsNormal( pCompItem->auchBuffer, pVoidItem->usItemOffset );
                }
            } else {                        /* void item is not found */
                if ( sStorageType == TRN_TYPE_ITEMSTORAGE ) {
                    TrnStoItemNormal( pCompItem->auchBuffer, usSrcZipSize );
                } else if (sStorageType == TRN_TYPE_CONSSTORAGE) {   /* R3.1 */
                    TrnStoConsNormal( pCompItem->auchBuffer, usSrcZipSize );
                } else {                                             /* R3.1 */
                    TrnStoConsNormalSpl(pCompItem->auchBuffer, usSrcZipSize, sStorageType);
                }
            }
        } else {    /* source item is normal item */
			// First of all, lets modify the quantities, etc of the root item.
			// We do this in case the root item is actually a consolidation of several
			// items that are the same item.  This may be if someone has entered hamburger
			// several times so that the display shows, say 5 hamburgers but in the transaction
			// file there is only a single hamburger entry with a quantity of five.
			bSplitItem = (pNoCompSrc->usUniqueID == pNoCompNormal->usUniqueID && pNoCompNormal->lQTY != pNoCompSrc->lQTY);
			if (bSplitItem) {
				pNoCompNormal->sCouponQTY -= pNoCompSrc->sCouponQTY;
				pNoCompNormal->lQTY -= pNoCompSrc->lQTY;
				pNoCompNormal->lProduct -= pNoCompSrc->lProduct;
				TrnWriteFile( pNormalItem->usItemOffset, pNormalItem->auchBuffer, sizeof( UCHAR ) + sizeof( TRANSTORAGESALESNON ), hsStorage );
				TrnStoReplaceInsertIndex (pNoCompNormal, pNormalItem->usItemOffset, sStorageType );
			}

			// If there is a similar item that has already been stored then we will want
			// to consolidate this item with that one so that it will show up consolidated
			// in the consolidated view.
			if (pSameItem->usItemOffset != 0) {
				pNoCompSame->sCouponQTY += pNoCompSrc->sCouponQTY;
				pNoCompSame->lQTY += pNoCompSrc->lQTY;
				pNoCompSame->lProduct += pNoCompSrc->lProduct;
				TrnWriteFile( pSameItem->usItemOffset, pSameItem->auchBuffer, sizeof( UCHAR ) + sizeof( TRANSTORAGESALESNON ), hsStorage );
				TrnStoReplaceInsertIndex (pNoCompSame, pSameItem->usItemOffset, sStorageType );
				if (! bSplitItem) {
					// If we are updating an item by basically creating a new item entry in the transaction
					// then we need to ensure that the old entry is no longer visible.  This prevents MLD display
					// issues and such from cropping up because after doing this the MLD subsystem will no longer
					// see this item as a sales item if the total key is pressed and the display is being redone.
					memset (pNoCompNormal, 0, sizeof(TRANSTORAGESALESNON));
					TrnWriteFile( pNormalItem->usItemOffset, pNormalItem->auchBuffer, sizeof( UCHAR ) + sizeof( TRANSTORAGESALESNON ), hsStorage );
				}
			}
			else {
				pNoCompSrc->usItemOffset = 0;
				if ( sStorageType == TRN_TYPE_ITEMSTORAGE ) {
					if (bSplitItem) {
						// the item that has been modified is the root item so we need to split off any
						// other items that are using this item entry.
						// the consolidated file no longer has an entry pointing to the normal
						// data item.  If the quantity of the normal item data is greater than
						// 0 then we will need to copy an existing index from the NO file to this
						// file for example from a TRN_TYPE_NOITEMSTORAGE to a TRN_TYPE_ITEMSTORAGE
						// index file.
						TrnStoFindCopyIndexEntry (pNoCompNormal, pNormalItem->usItemOffset, TRN_TYPE_NOITEMSTORAGE, TRN_TYPE_ITEMSTORAGE);
						TrnWriteFile( pNormalItem->usItemOffset, pNormalItem->auchBuffer, sizeof( UCHAR ) + sizeof( TRANSTORAGESALESNON ), hsStorage );
					}
					else if (pNoCompSrc->usUniqueID == pNoCompNormal->usUniqueID) {
						// If we are updating an item by basically creating a new item entry in the transaction
						// then we need to ensure that the old entry is no longer visible.  This prevents MLD display
						// issues and such from cropping up because after doing this the MLD subsystem will no longer
						// see this item as a sales item if the total key is pressed and the display is being redone.
						memset (pNoCompNormal, 0, sizeof(TRANSTORAGESALESNON));
						TrnWriteFile( pNormalItem->usItemOffset, pNormalItem->auchBuffer, sizeof( UCHAR ) + sizeof( TRANSTORAGESALESNON ), hsStorage );
					}
					TrnStoItemNormal( pCompItem->auchBuffer, usSrcZipSize );
				} else if (sStorageType == TRN_TYPE_CONSSTORAGE) {    /* R3.1 */
					if (bSplitItem) {
						// the item that has been modified is the root item so we need to split off any
						// other items that are using this item entry.
						// the consolidated file no longer has an entry pointing to the normal
						// data item.  If the quantity of the normal item data is greater than
						// 0 then we will need to copy an existing index from the NO file to this
						// file for example from a TRN_TYPE_NOITEMSTORAGE to a TRN_TYPE_ITEMSTORAGE
						// index file.
						TrnStoFindCopyIndexEntry (pNoCompNormal, pNormalItem->usItemOffset, TRN_TYPE_NOCONSSTORAGE, TRN_TYPE_CONSSTORAGE);
						TrnWriteFile( pNormalItem->usItemOffset, pNormalItem->auchBuffer, sizeof( UCHAR ) + sizeof( TRANSTORAGESALESNON ), hsStorage );
					}
					else if (pNoCompSrc->usUniqueID == pNoCompNormal->usUniqueID) {
						// If we are updating an item by basically creating a new item entry in the transaction
						// then we need to ensure that the old entry is no longer visible.  This prevents MLD display
						// issues and such from cropping up because after doing this the MLD subsystem will no longer
						// see this item as a sales item if the total key is pressed and the display is being redone.
						memset (pNoCompNormal, 0, sizeof(TRANSTORAGESALESNON));
						TrnWriteFile( pNormalItem->usItemOffset, pNormalItem->auchBuffer, sizeof( UCHAR ) + sizeof( TRANSTORAGESALESNON ), hsStorage );
					}
					TrnStoConsNormal( pCompItem->auchBuffer, usSrcZipSize );
				}else if(sStorageType == TRN_TYPE_PREVIOUS_CONDIMENT){
				//	TrnStoPreviousCondiment(puchSrcZip, usSrcZipSize);
				}else {                                              /* R3.1 */
					TrnStoConsNormalSpl(pCompItem->auchBuffer, usSrcZipSize, sStorageType);
				}
			}
        }

	/* --- determine current linked coupon item is over than coupon quantity --- */
        fOverCoupon = FALSE;

        if ( sCouponQTY_Temp < 0 ) {
            if ( 0 < sCouponCo_Temp ) {
                fOverCoupon = TRUE;
            } else
            if ( sCouponCo_Temp < sCouponQTY_Temp ) {
                fOverCoupon = TRUE;
            }
        } else {    /* 0 <= CouponQTY */
            if ( sCouponCo_Temp < 0 ) {
                fOverCoupon = TRUE;
            } else
            if ( sCouponQTY_Temp < sCouponCo_Temp ) {
                fOverCoupon = TRUE;
            }
        }

        if ( fOverCoupon ) {
			ULONG   fulTrnPrtSts = TranCurQualPtr->flPrintStatus;
			ULONG   fusTrnCurSts = TranCurQualPtr->fsCurStatus;
			SHORT   fPostChkGCOpe;
			SHORT   fStoreRecall;

            fPostChkGCOpe = ( fulTrnPrtSts & CURQUAL_POSTCHECK ) && (( fusTrnCurSts & CURQUAL_OPEMASK ) != CURQUAL_CASHIER );
            fStoreRecall = (( fulTrnPrtSts & CURQUAL_GC_SYS_MASK ) == CURQUAL_STORE_RECALL );

            if ( fStoreRecall ) {
                TrnStoVoidCouponItem( pItemSales, pNormalItem->auchBuffer, pNormalItem->usItemOffset, TRN_TYPE_CONSSTORAGE );
            } else {
                if (( sStorageType == TRN_TYPE_ITEMSTORAGE ) && ( ! fPostChkGCOpe )) {
                    /* --- void coupon item --- */
                    TrnStoVoidCouponItem( pItemSales, pNormalItem->auchBuffer, pNormalItem->usItemOffset, sStorageType );
                } else if (( sStorageType == TRN_TYPE_CONSSTORAGE ) && ( fPostChkGCOpe )) {
                    /* --- void coupon item --- */
                    TrnStoVoidCouponItem( pItemSales, pNormalItem->auchBuffer, pNormalItem->usItemOffset, sStorageType );
                }
            }

            /* --- R3.1 --- */
            if ( pItemSales->fbModifier & VOID_MODIFIER ) {
                /*----- Reset Offset of 1'st Void Item,   R3.1 -----*/
                pNoCompNormal->usItemOffset = usItemOffset;

                /* --- update normal item data --- */
                TrnWriteFile( pNormalItem->usItemOffset, pNormalItem->auchBuffer, sizeof( UCHAR ) + sizeof( TRANSTORAGESALESNON ), hsStorage );
            }
        }
    }
    /* --- restore source coupon quantity --- */
    pNoCompSrc->sCouponQTY = sSrcCpnQTY;

    return ( TRN_SUCCESS );
}

/*
*==========================================================================
**   Synopsis:  SHORT TrnStoSalesSearch( ITEMSALES          *pItemSales,
*                                        UCHAR              *puchSrcZip,
*                                        TRANSTOSALESSRCH   *pNormalItem,
*                                        TRANSTOSALESSRCH   *pVoidItem,
*                                        SHORT              sStorageType,
*                                        SHORT              sType )
*
*       Input:  ITEMSALES        *pItemSales  - address of unzipped data
*               UCHAR            *puchSrcZip  - address of zipped source data
*               TRANSTOSALESSRCH *pNormalItem - address of normal item
*               TRANSTOSALESSRCH *pVoidItem   - address of void item
*               SHORT            sStorageType - type of transaction storage
*                   TRN_TYPE_ITEMSTORAGE - item storage
*                   TRN_TYPE_CONSSTORAGE - consolidation storage
*               SHORT            sType        - 0:Normal, -1:Reduce
*      Output:  Nothing
*       InOut:  Nothing
*
*      Return:  TRN_SUCCESS     : item found and consolidate it
*               TRN_ERROR       : not found
*
**  Description:
*           search target "item" at item/consoli. storage
*           if find it, consolidate item/consoli. storage         R3.1
*==========================================================================
*/
SHORT TrnStoSalesSearchVoidItem( ITEMSALES          *pItemSales,
                         UCHAR              *puchSrcZip,
                         TRANSTOSALESSRCH   *pNormalItem,
                         TRANSTOSALESSRCH   *pVoidItem,
                         TrnStorageType     sStorageType,
                         SHORT              sType )
{
    TRANSTORAGESALESNON *pTranNon;
    ULONG   ulReadPosition;
    ULONG   ulMaxStoSize;
    SHORT   sReturn;
    SHORT   hsStorage;
    USHORT  usStorageVli;
    USHORT  usStorageHdrSize;
    USHORT  usSrcZipSize;
    USHORT  usTargetItemSize;
    USHORT  usUnzipSize;
    USHORT  usZipSize;
    BOOL    fNormalItemFound;
    BOOL    fVoidItemFound;
    UCHAR   auchTranStorageWork[ TRN_STOSIZE_BLOCK ];

    sReturn = TrnStoGetStorageInfo( sStorageType, &hsStorage, &usStorageHdrSize, &usStorageVli );
    if ( sReturn != TRN_SUCCESS ) {
        return ( sReturn );
    }

    /* --- calculate size of source sales item data --- */
    usUnzipSize  = ( USHORT )( *puchSrcZip );
    usZipSize    = *(( USHORT * )( puchSrcZip + usUnzipSize ));
    usSrcZipSize = ( usUnzipSize + usZipSize );

    pNormalItem->usItemOffset = 0;
    pVoidItem->usItemOffset = 0;

    fNormalItemFound  = FALSE;
    fVoidItemFound    = FALSE;
    ulReadPosition    = usStorageHdrSize;
    ulMaxStoSize      = ulReadPosition + usStorageVli;

    pTranNon = TRANSTORAGENONPTR( auchTranStorageWork );
    while (( ulReadPosition < ulMaxStoSize ) && (fNormalItemFound == FALSE || fVoidItemFound == FALSE)) {
        
		/* --- read item data, and store to work buffer --- */
		sReturn = TrnStoIsItemInWorkBuff(hsStorage, ulReadPosition, auchTranStorageWork, sizeof( auchTranStorageWork ), &usTargetItemSize);
		if (sReturn == TRN_ERROR) {
			NHPOS_ASSERT_TEXT((sReturn != TRN_ERROR), "**ERROR: TrnStoIsItemInWorkBuff() item record size exceeds buffer");
			return TRN_ERROR;
		}

        /* --- compare read item with source sales item --- */
		if (RflIsSalesItemDisc(pTranNon)) {
			USHORT usTemp = RflGetStorageItemLen (auchTranStorageWork + usTargetItemSize, -1);

			usTargetItemSize += usTemp;
		}

		// Any changes to this logic should be tested with Void Consolidation (MDC 235) turned on and turned off to double
		// check that receipt printing is not affected by testing receipt printing.  In addition any changes may affect the
		// output of the Connection Engine Interface which also does Void Consolidation.
		// Also review the source of function TrnStoSalesSearch() for any changes needed.
		if ((pTranNon->fsLabelStatus & ITM_CONTROL_RETURN_OLD) == (pItemSales->fsLabelStatus & ITM_CONTROL_RETURN_OLD)) {
        if ( pTranNon->usUniqueID == pItemSales->usUniqueID ||
			( (TrnIsSameZipDataSalesNon (puchSrcZip, auchTranStorageWork) &&
              ( ! (pTranNon->fbReduceStatus & REDUCE_ITEM) ||                 /* 5/27/96 */
              ((pTranNon->fbReduceStatus & REDUCE_ITEM) && (sType != 0)))) ) ) {    /* 5/27/96 */

            if ( (memcmp (puchSrcZip + usUnzipSize, auchTranStorageWork + usUnzipSize, usZipSize) == 0 ||
				TrnIsTranSameAsItem(auchTranStorageWork, pItemSales))) {  /* zipped data is same */

                /* --- consolidate source item to item which is already stored --- */
                /*----- Normal, R3.1 -----*/
                /*if (sType == 0) {
                    pTranNon->lQTY     += pItemSales->lQTY;
                    pTranNon->lProduct += pItemSales->lProduct;
                /----- Reduce, R3.1 -----/
                } else {
                    pTranNon->lQTY     -= pItemSales->lQTY;
                    pTranNon->lProduct -= pItemSales->lProduct;
                }*/

                if ( pTranNon->fsLabelStatus & ITM_CONTROL_VOIDITEM ) {
                    /* --- declare void item already exists in transaction storage,
                        and store it to buffer --- */

					if (fVoidItemFound == FALSE) {	/* 07/24/01 */
	                    pVoidItem->usItemOffset = ulReadPosition;
    	                memcpy( pVoidItem->auchBuffer, auchTranStorageWork, usSrcZipSize );
        	            fVoidItemFound = TRUE;
        	        }
                } else {                                        /* source item is normal */
                    /* --- declare normal item already exists in transaction storage,
                        and store it to buffer --- */
					if (fNormalItemFound == FALSE) {	/* 07/24/01 */
	                    pNormalItem->usItemOffset = ulReadPosition;
    	                memcpy( pNormalItem->auchBuffer, auchTranStorageWork, usSrcZipSize );
        	            fNormalItemFound = TRUE;
        	        }
                }
            } else if ( TrnStoIsPairItem( pItemSales, auchTranStorageWork ) > 0) {
                /* --- this target item is pair of source item.
                    ( SRC = normal , TRG = void ) or ( SRC = void, TRG = normal ) --- */
                if ( ( pTranNon->fsLabelStatus & ITM_CONTROL_VOIDITEM ) == 0 ) {
                    /* --- declare normal item already exists in transaction storage,
                        and store it to buffer --- */
					if (fNormalItemFound == FALSE) {
	                    pNormalItem->usItemOffset = ulReadPosition;
    	                memcpy( pNormalItem->auchBuffer, auchTranStorageWork, usSrcZipSize );
        	            fNormalItemFound = TRUE;
        	        }
                } else {                                        /* source item is normal */
                    /* --- declare void item already exists in transaction storage,
                        and store it to buffer --- */
					if (fVoidItemFound == FALSE) {
	                    pVoidItem->usItemOffset = ulReadPosition;
    	                memcpy( pVoidItem->auchBuffer, auchTranStorageWork, usSrcZipSize );
        	            fVoidItemFound = TRUE;
        	        }
                }
            }
        }
		}

        /* --- increment read position to read next work buffer --- */
        ulReadPosition += usTargetItemSize;
    }
    return ( TRN_SUCCESS );
}
/*
*==========================================================================
**   Synopsis:  SHORT TrnStoSalesUpdateVoidItem( ITEMSALES *pItemSales,
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
SHORT TrnStoSalesUpdateVoidItem( ITEMSALES  *pItemSales,
                         UCHAR              *puchSrcZip,
                         TRANSTOSALESSRCH   *pNormalItem,
                         TRANSTOSALESSRCH   *pVoidItem,
                         TrnStorageType     sStorageType )
{
    TRANSTORAGESALESNON *pNoCompNormal = TRANSTORAGENONPTR( pNormalItem->auchBuffer );
    TRANSTORAGESALESNON *pNoCompVoid = TRANSTORAGENONPTR( pVoidItem->auchBuffer );
    TRANSTORAGESALESNON *pNoCompSrc = TRANSTORAGENONPTR( puchSrcZip );
    SHORT               sSrcCpnQTY;
    SHORT               hsStorage;
    USHORT              usStorageVli;
    USHORT              usStorageHdrSize;
    USHORT              usSrcZipSize;
    USHORT              usNoCompSize;
    USHORT              usCompSize;
    USHORT              usItemOffset;
    SHORT               sReturn;
    BOOL                fOverCoupon;

    sReturn = TrnStoGetStorageInfo( sStorageType, &hsStorage, &usStorageHdrSize, &usStorageVli );
    if (sReturn != TRN_SUCCESS ) {
        return ( sReturn );
    }

    usNoCompSize = ( USHORT )( *puchSrcZip );
    usCompSize   = *(( USHORT * )( puchSrcZip + usNoCompSize ));
    usSrcZipSize = ( usNoCompSize + usCompSize );

    /* --- calculate sales item's coupon quantity --- */
    /* --- store source coupon quantity --- */
    sSrcCpnQTY = pNoCompSrc->sCouponQTY;

    /* --- R3.1 --- */
    usItemOffset = usStorageHdrSize + usStorageVli;

    if ( pNormalItem->usItemOffset ) {
        if ( pVoidItem->usItemOffset ) {    /* normal && void */
            pNoCompNormal->sCouponQTY = ( SHORT )(( pNoCompNormal->lQTY + pNoCompVoid->lQTY ) / 1000 );
            /*----- Save Offset of Void Item,   R3.1 -----*/
            pNoCompNormal->usItemOffset = pVoidItem->usItemOffset;
        } else {                        /* normal && ! void */
            if ( pItemSales->fbModifier & VOID_MODIFIER ) {
                pNoCompNormal->sCouponQTY = ( SHORT )(( pNoCompNormal->lQTY + pItemSales->lQTY ) / 1000 );
                /*----- Save Offset of 1'st Void Item,   R3.1 -----*/
                pNoCompNormal->usItemOffset = usItemOffset;
            } else {
                pNoCompNormal->sCouponQTY = ( SHORT )( pNoCompNormal->lQTY / 1000 );
            }
        }

        /* --- update normal item data --- */
        TrnWriteFile( pNormalItem->usItemOffset, pNormalItem->auchBuffer, sizeof( UCHAR ) + sizeof( TRANSTORAGESALESNON ), hsStorage );

        if ( ( pItemSales->fbModifier & VOID_MODIFIER ) == 0) {
            if ( sStorageType == TRN_TYPE_ITEMSTORAGE ) {
                TrnStoItemNormal( puchSrcZip, usSrcZipSize );
            } else if (sStorageType == TRN_TYPE_CONSSTORAGE) {   /* R3.1 */
                TrnStoConsNormal( puchSrcZip, usSrcZipSize );
            } else {                                             /* R3.1 */
                TrnStoConsNormalSpl(puchSrcZip, usSrcZipSize, sStorageType);
            }
            /*if ( sStorageType == TRN_TYPE_ITEMSTORAGE ) {
                TrnStoNoItemNormal( puchSrcZip, pNormalItem->usOffset );
            }
            if ( sStorageType == TRN_TYPE_CONSSTORAGE ) {
                TrnStoNoConsNormal( puchSrcZip, pNormalItem->usOffset);
            }*/
        }

        /* --- determine current linked coupon item is over than coupon quantity --- */
/****** bug fixed (1995-6-29, V3.00.02)
        if ( abs( pNoCompNormal->sCouponQTY ) < abs( pNoCompNormal->sCouponCo )) {
******/
        fOverCoupon = FALSE;
        if ( pNoCompNormal->sCouponQTY < 0 ) {
            if ( 0 < pNoCompNormal->sCouponCo ) {
                fOverCoupon = TRUE;
            } else if ( pNoCompNormal->sCouponCo < pNoCompNormal->sCouponQTY ) {
                fOverCoupon = TRUE;
            }
        } else {    /* 0 <= CouponQTY */
            if ( pNoCompNormal->sCouponCo < 0 ) {
                fOverCoupon = TRUE;
            } else if ( pNoCompNormal->sCouponQTY < pNoCompNormal->sCouponCo ) {
                fOverCoupon = TRUE;
            }
        }

        if ( fOverCoupon ) {
			ULONG   fulTrnPrtSts = TranCurQualPtr->flPrintStatus;
			ULONG   fusTrnCurSts = TranCurQualPtr->fsCurStatus;
			SHORT   fPostChkGCOpe;
			SHORT   fStoreRecall;

            fPostChkGCOpe = ( fulTrnPrtSts & CURQUAL_POSTCHECK ) && (( fusTrnCurSts & CURQUAL_OPEMASK ) != CURQUAL_CASHIER );
            fStoreRecall = (( fulTrnPrtSts & CURQUAL_GC_SYS_MASK ) == CURQUAL_STORE_RECALL );

            if ( fStoreRecall ) {
                TrnStoVoidCouponItem( pItemSales, pNormalItem->auchBuffer, pNormalItem->usItemOffset, TRN_TYPE_CONSSTORAGE );
            } else {
                if (( sStorageType == TRN_TYPE_ITEMSTORAGE ) && ( ! fPostChkGCOpe )) {
                    /* --- void coupon item --- */
                    TrnStoVoidCouponItem( pItemSales, pNormalItem->auchBuffer, pNormalItem->usItemOffset, sStorageType );
                } else if (( sStorageType == TRN_TYPE_CONSSTORAGE ) && ( fPostChkGCOpe )) {
                    /* --- void coupon item --- */
                    TrnStoVoidCouponItem( pItemSales, pNormalItem->auchBuffer, pNormalItem->usItemOffset, sStorageType );
                }
            }

            /* --- R3.1 --- */
            if ( pItemSales->fbModifier & VOID_MODIFIER ) {
                /*----- Reset Offset of 1'st Void Item,   R3.1 -----*/
                pNoCompNormal->usItemOffset = usItemOffset;

                /* --- update normal item data --- */
                TrnWriteFile( pNormalItem->usItemOffset, pNormalItem->auchBuffer, sizeof( UCHAR ) + sizeof( TRANSTORAGESALESNON ), hsStorage );
            }
        }

        /* --- update void item data --- */
        if ( pItemSales->fbModifier & VOID_MODIFIER ) {
            if ( pVoidItem->usItemOffset ) {    /* void item is found */
                /*----- Save Offset of Normal Item,   R3.1 -----*/
                TrnWriteFile( pVoidItem->usItemOffset, pVoidItem->auchBuffer, sizeof( UCHAR ) + sizeof( TRANSTORAGESALESNON ), hsStorage );

                if ( sStorageType == TRN_TYPE_ITEMSTORAGE ) {
                    TrnStoItemNormal( puchSrcZip, usSrcZipSize );
                } else if (sStorageType == TRN_TYPE_CONSSTORAGE) {   /* R3.1 */
                    TrnStoConsNormal( puchSrcZip, usSrcZipSize );
                } else {                                             /* R3.1 */
                    TrnStoConsNormalSpl(puchSrcZip, usSrcZipSize, sStorageType);
                }
                /*if ( sStorageType == TRN_TYPE_ITEMSTORAGE ) {
                    TrnStoNoItemNormal( puchSrcZip, pVoidItem->usOffset );
                }
                if ( sStorageType == TRN_TYPE_CONSSTORAGE ) {
                    TrnStoNoConsNormal( puchSrcZip, pVoidItem->usOffset );
                }*/
            } else {                        /* void item is not found */
				TRANSTORAGESALESNON  *pSalesNon = TRANSTORAGENONPTR(puchSrcZip);

                /*----- Save Offset of Normal Item,   R3.1 -----*/
                pSalesNon->usItemOffset = pNormalItem->usItemOffset;

                if ( sStorageType == TRN_TYPE_ITEMSTORAGE ) {
                    TrnStoItemNormal( puchSrcZip, usSrcZipSize );
                } else if (sStorageType == TRN_TYPE_CONSSTORAGE) {   /* R3.1 */
                    TrnStoConsNormal( puchSrcZip, usSrcZipSize );
                } else {                                             /* R3.1 */
                    TrnStoConsNormalSpl(puchSrcZip, usSrcZipSize, sStorageType);
                }
            }
        }
    } else {    /* normal item does not exist in transaction storage */
        if ( pVoidItem->usItemOffset ) {    /* ! normal && void */
            if ( pItemSales->fbModifier & VOID_MODIFIER ) {
                pNoCompVoid->sCouponQTY = ( SHORT )( pNoCompVoid->lQTY / 1000 );
            } else {
                /*----- Save Offset of Normal Item,   R3.1 -----*/
                pNoCompSrc->sCouponQTY += ( SHORT )( pNoCompVoid->lQTY / 1000 );
                pNoCompSrc->usItemOffset = pVoidItem->usItemOffset;

                /*----- Save Offset of Void Item,   R3.1 -----*/
                pNoCompVoid->sCouponQTY += ( SHORT )( pItemSales->lQTY / 1000 );
                pNoCompVoid->usItemOffset = usItemOffset;
                TrnWriteFile( pVoidItem->usItemOffset, pVoidItem->auchBuffer, sizeof(UCHAR) + sizeof(TRANSTORAGESALESNON), hsStorage);
            }
        } else {                        /* ! normal && ! void */
            ;   /* do nothing */
        }

        if ( pItemSales->fbModifier & VOID_MODIFIER ) {
            if ( pVoidItem->usItemOffset ) {    /* void item is found */
                TrnWriteFile( pVoidItem->usItemOffset, pVoidItem->auchBuffer, sizeof( UCHAR ) + sizeof( TRANSTORAGESALESNON ), hsStorage );

                if ( sStorageType == TRN_TYPE_ITEMSTORAGE ) {
                    TrnStoItemNormal( puchSrcZip, usSrcZipSize );
                } else if (sStorageType == TRN_TYPE_CONSSTORAGE) {   /* R3.1 */
                    TrnStoConsNormal( puchSrcZip, usSrcZipSize );
                } else {                                             /* R3.1 */
                    TrnStoConsNormalSpl(puchSrcZip, usSrcZipSize, sStorageType);
                }
                /*if ( sStorageType == TRN_TYPE_ITEMSTORAGE ) {
                    TrnStoNoItemNormal( puchSrcZip, pVoidItem->usOffset );
                }
                if ( sStorageType == TRN_TYPE_CONSSTORAGE ) {
                    TrnStoNoConsNormal( puchSrcZip, pVoidItem->usOffset );
                }*/
            } else {                        /* void item is not found */
                if ( sStorageType == TRN_TYPE_ITEMSTORAGE ) {
                    TrnStoItemNormal( puchSrcZip, usSrcZipSize );
                } else if (sStorageType == TRN_TYPE_CONSSTORAGE) {   /* R3.1 */
                    TrnStoConsNormal( puchSrcZip, usSrcZipSize );
                } else {                                             /* R3.1 */
                    TrnStoConsNormalSpl(puchSrcZip, usSrcZipSize, sStorageType);
                }
            }
        } else {    /* source item is normal item */
            if ( sStorageType == TRN_TYPE_ITEMSTORAGE ) {
                TrnStoItemNormal( puchSrcZip, usSrcZipSize );
            } else if (sStorageType == TRN_TYPE_CONSSTORAGE) {    /* R3.1 */
                TrnStoConsNormal( puchSrcZip, usSrcZipSize );
            } else {                                              /* R3.1 */
                TrnStoConsNormalSpl(puchSrcZip, usSrcZipSize, sStorageType);
            }
        }
    }

    /* --- restore source coupon quantity --- */
    pNoCompSrc->sCouponQTY = sSrcCpnQTY;

    return ( TRN_SUCCESS );
}


/*
*==========================================================================
**    Synopsis: SHORT TrnStoIsPairItem( ITEMSALES *pSource,
*                                       UCHAR     *puchTarget )
*
*       Input:  ITEMSALES *pSource    - address of source sales item
*               UCHAR     *puchTarget - address of target sales item
*      Output:  Nothing
*       InOut:  Nothing
*
*    Return:    TRUE    : source and target is pair item.
*               FALSE   : not pair
*
**  Description:
*           determine source item and pair item is pair item.
*==========================================================================
*/

SHORT TrnStoIsPairItem( ITEMSALES *pSource, UCHAR *puchTarget )
{
    USHORT    usI;
    SHORT     fSameItem;
	ITEMSALES ItemSales = {0};

    RflGetStorageItem( &ItemSales, puchTarget, RFL_WITHOUT_MNEM );

	if( (pSource->uchMinorClass == CLASS_ITEMORDERDEC) && (ItemSales.uchMinorClass == CLASS_ITEMORDERDEC))
	{
		return (TRUE);
	}

    if ( pSource->lUnitPrice != ItemSales.lUnitPrice ) {
        return ( FALSE );
    }

    if ( pSource->uchAdjective != ItemSales.uchAdjective ) {
        return ( FALSE );
    }

    if ( pSource->usReasonCode != ItemSales.usReasonCode ) {
        return ( FALSE );
    }

    if ( pSource->uchPrintModNo != ItemSales.uchPrintModNo ) {
        return ( FALSE );
    }

    if ( pSource->uchChildNo != ItemSales.uchChildNo ) {
        return ( FALSE );
    }

    if ( pSource->uchPPICode != ItemSales.uchPPICode ) {
        return ( FALSE );
    }

    if ((pSource->fbModifier & ~VOID_MODIFIER) != (ItemSales.fbModifier & ~VOID_MODIFIER)) {
        return ( FALSE );
    }

	//SR 527, the problem was that we were comparing only the first number in the number type array
	//by checking ALL of the numbers, we can now determine the correct item to discount JHHJ
    if ( memcmp( pSource->aszNumber[0], ItemSales.aszNumber[0], (NUM_NUMBER * NUM_OF_NUMTYPE_ENT * sizeof(TCHAR)))) {
        return ( FALSE );
    }

    if (memcmp (pSource->auchPrintModifier, ItemSales.auchPrintModifier, sizeof(ItemSales.auchPrintModifier)) != 0) {
        return ( FALSE );
    }

    /* --- determine condiment items match if not same unique id in both --- */
	fSameItem = 1;
    if ( pSource->usUniqueID != ItemSales.usUniqueID ) {
		if ( pSource->uchCondNo != ItemSales.uchCondNo ) {
			return ( -2 );
		}

		for ( usI = 0; (( usI < STD_MAX_COND_NUM ) && ( fSameItem )); usI++ ) {
			if ( _tcsncmp(pSource->Condiment[ usI ].auchPLUNo, ItemSales.Condiment[ usI ].auchPLUNo, NUM_PLU_LEN) != 0 ) {    /* 2172 */
				 fSameItem = 0;
			} else if ( pSource->Condiment[ usI ].uchAdjective != ItemSales.Condiment[ usI ].uchAdjective ) {
				 fSameItem = 0;
			} else if ( pSource->Condiment[ usI ].usDeptNo != ItemSales.Condiment[ usI ].usDeptNo ) {    /* 2172 */
				 fSameItem = 0;
			}
		}
    }

	/* 07/24/01 */
#if 0
    if ((ItemSales.uchMinorClass == CLASS_PLUITEMDISC) || (ItemSales.uchMinorClass == CLASS_SETITEMDISC)) {
		if (ItemSales.usItemOffset) {	/* already voided item */
			fSameItem = 0;
		}
	}
#endif

    return ( fSameItem );
}

/*
*==========================================================================
**    Synopsis: SHORT SHORT TrnStoIsItemInWorkBuff(SHORT hsStorageFile, 
*							USHORT usReadPosition,  UCHAR  *puchWorkBuff, 
*							USHORT usMaxBuffSize,USHORT *pusItemSize)
*
*       Input:  SHORT   hsStorageFile - handle of the file to be read
*               USHORT  usReadPosition  - offset of item data in the file
*				UCHAR	*puchWorkBuffer - pointer to the area in which to put
										  the data read from the file
*               USHORT  usMaxBuffSize - maximum size of work buffer
*               USHORT  *pusItemSize  - size of item data in work buffer

*      Output:  Nothing
*       InOut:  Nothing
*
*    Return:    TRUE    : whole item data is in work buffer.
*               FALSE   : part of item data is out of work buffer.
*
**  Description:
*       This function reads the file from the point specified in usReadPosition,
*		and then we return back the size of the item found at that point.
*
*       Simce this function reads as much of the data as will fit into the buffer,
*       it is up to the caller to determine if the data returned is valid. We only
*       do a minimum consistency check.
*==========================================================================
*/
SHORT TrnStoIsItemInWorkBuff(PifFileHandle hsStorageFile, ULONG ulReadPosition,
							  UCHAR  *puchWorkBuff, USHORT usMaxBuffSize,
							  USHORT *pusItemSize)
{
	ULONG   ulActualBytesRead = 0;
	SHORT   sRetStatus = TRN_SUCCESS;

	*pusItemSize = 0;    // default is zero length for the item.

	/*
		First of all, we will read the maximum amount of data requested beginning at the
		position specified.  Then we will check the item size information and let the caller
		know how much of what was read is actually good information.

		Ignore the return value from TrnReadFile() since if the read is near the end of the
		transaction file, there may be a final record which is smaller than the requested
		buffer size but still a valid record.
	 */
	TrnReadFile( ulReadPosition, puchWorkBuff, usMaxBuffSize, hsStorageFile, &ulActualBytesRead );

	// check that the minimum size of an item data record is met.
	if (ulActualBytesRead < 8) {
		sRetStatus = TRN_ERROR;
	} else {
		// an item data storage record has fixed length and a variable length parts.
		// the buffer may be large enough for the fixed length but not the variable length.
		// the function RflGetStorageItemLen() does a check on the fixed length and the number of bytes read.
		*pusItemSize  = RflGetStorageItemLen(puchWorkBuff, ulActualBytesRead);
		if(*pusItemSize > usMaxBuffSize)
		{
			NHPOS_ASSERT((*pusItemSize < usMaxBuffSize));
			sRetStatus = TRN_ERROR;
		}
	}

	return sRetStatus;
}



/*==========================================================================
**    Synopsis: SHORT   TrnStoUpdateCouponType( ITEMCOUPON *pItem )
*
*    Input:     ITEMCOUPON  *pItem
*    Output:    none
*    InOut:     none
*
*    Return:    nothing
*
*    Description:   update single type coupon status of GCF qualifier
==========================================================================*/

VOID    TrnStoUpdateCouponType( ITEMCOUPON *pItem )
{
    USHORT  usNo;
    USHORT  usMask;
    USHORT  usOff;

    /* --- check multiple type coupon --- */

    if ( ! ( pItem->fbStatus[1] & ITM_COUPON_SINGLE_TYPE )) {
        return;                                 /* exit ...    */
    }

    usNo   = ( USHORT )( pItem->uchCouponNo - 1 );
    usMask = 0x01 << ( usNo % 8 );
    usOff  = usNo / 8;

    /* --- update single type coupon status --- */

    if ( pItem->fbModifier & VOID_MODIFIER ) {  /* void item   */
        TrnInformation.TranGCFQual.auchCouponType[ usOff ] &= ~usMask;
    } else {                                    /* normal item */
        TrnInformation.TranGCFQual.auchCouponType[ usOff ] |= usMask;
    }
}

/*==========================================================================
**   Synopsis:  SHORT TrnStoIdxUpdate( ITEMSALES *pItemSales,
*                                      TRANSTOSALESSRCH *pNormalItem,
*                                      TRANSTOSALESSRCH *pVoidItem,
*                                      SHORT     sStorageType )
*
*       Input:  ITEMSALES *pItemSales  - address of unzipped source data
*               TRANSTOSALESSRCH *pNormalItem - address of normal item
*               TRANSTOSALESSRCH *pVoidItem - address of void item
*               SHORT     sStorageType - type of transaction storage
*                           TRN_TYPE_ITEMSTORAGE - item storage
*                           TRN_TYPE_CONSSTORAGE - consolidation storage
*                           TRN_TYPE_SPLITA      - Split A Storage
*                           TRN_TYPE_SPLITB      - Split B Storage
*      Output:  Nothing
*       InOut:  Nothing
*
*      Return:  TRN_SUCCESS     : item found and consolidate it
*               TRN_ERROR       : not found
*
**  Description:
*           update quantity & product data in index file.         R3.1
*==========================================================================
*/
SHORT TrnStoIdxUpdate( ITEMSALES          *pItemSales,
                       TRANSTOSALESSRCH   *pNormalItem,
                       TRANSTOSALESSRCH   *pVoidItem,
                       TrnStorageType     sStorageType )
{
    SHORT       hsIndexFile;
    PRTIDXHDR   IdxFileInfo;
    UCHAR       auchIdxWork[ sizeof( PRTIDX ) * 80 ];
    BOOL        fUpdateIdx = FALSE;
    ULONG       ulTtlIdxReadSize;
    ULONG       ulCurIdxReadSize;
    PRTIDX      *pPrtIdxCurrent;
    UCHAR       uchMaxItemCo;
    UCHAR       uchItemCo;
    USHORT      usTargetOffset;
	ULONG		ulActualBytesRead;//RPH 11-11-3 SR# 201

	if ((hsIndexFile = TrnDetermineIndexFileHandle(sStorageType)) < 0) {
		return (TRN_ERROR);
	}

    /* --- determine whether inputed sales item already exists in storage,
        and increments QTY & Product --- */
    if (( pNormalItem->usItemOffset ) && ( ! ( pItemSales->fbModifier & VOID_MODIFIER ))) {
        usTargetOffset = pNormalItem->usItemOffset;
        fUpdateIdx = TRUE;
    } else if (( pVoidItem->usItemOffset ) && ( pItemSales->fbModifier & VOID_MODIFIER )) {
        usTargetOffset = pVoidItem->usItemOffset;
        fUpdateIdx = TRUE;
    }

    if ( ! fUpdateIdx ) {
        return ( TRN_SUCCESS );
    }

	TrnReadFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), hsIndexFile , &ulActualBytesRead);

    ulTtlIdxReadSize = 0;
    ulTtlIdxReadSize = 0;

    while ( ulTtlIdxReadSize < IdxFileInfo.usIndexVli ) {
		TrnReadFile( (sizeof( PRTIDXHDR ) + ulTtlIdxReadSize), (VOID *)auchIdxWork, sizeof( auchIdxWork ), hsIndexFile, &ulCurIdxReadSize); /* ### Mod (2171 for Win32) V1.0 */
        if ( IdxFileInfo.usIndexVli < ( ulTtlIdxReadSize + ulCurIdxReadSize )) {
            ulCurIdxReadSize = IdxFileInfo.usIndexVli - ulTtlIdxReadSize;
        }
        ulTtlIdxReadSize += ulCurIdxReadSize;

        /* --- retrieve item data in storage and print its data --- */
        pPrtIdxCurrent = ( PRTIDX * )( auchIdxWork );
        uchMaxItemCo   = ( UCHAR )(( ulCurIdxReadSize < sizeof( auchIdxWork )) ?
                         ( ulCurIdxReadSize / sizeof( PRTIDX )):
                         ( sizeof( auchIdxWork ) / sizeof( PRTIDX )));

        for ( uchItemCo = 0; uchItemCo < uchMaxItemCo; uchItemCo++, pPrtIdxCurrent++ ) {
			// for this type of index file there is only one index per item even if there are multiple of the same item
            if ( pPrtIdxCurrent->usItemOffset == usTargetOffset ) {
                pPrtIdxCurrent->lQTY     += pItemSales->lQTY;
                pPrtIdxCurrent->lProduct += pItemSales->lProduct;

                TrnWriteFile( (USHORT)(sizeof( PRTIDXHDR ) + ulTtlIdxReadSize - ulCurIdxReadSize), (VOID *)auchIdxWork, ulCurIdxReadSize, hsIndexFile );
                break;
            }
        }
    }
    return ( TRN_SUCCESS );
}

/*==========================================================================
**   Synopsis:  SHORT TrnStoNoIdxUpdate( ITEMSALES *pItemSales,
*                                      TRANSTOSALESSRCH *pNormalItem,
*                                      TRANSTOSALESSRCH *pVoidItem,
*                                      SHORT     sStorageType )
*
*       Input:  ITEMSALES *pItemSales  - address of unzipped source data
*               TRANSTOSALESSRCH *pNormalItem - address of normal item
*               TRANSTOSALESSRCH *pVoidItem - address of void item
*               SHORT     sStorageType - type of transaction storage
*                           TRN_TYPE_ITEMSTORAGE - item storage
*                           TRN_TYPE_CONSSTORAGE - consolidation storage
*                           TRN_TYPE_SPLITA      - Split A Storage
*                           TRN_TYPE_SPLITB      - Split B Storage
*      Output:  Nothing
*       InOut:  Nothing
*
*      Return:  TRN_SUCCESS     : item found and consolidate it
*               TRN_ERROR       : not found
*
**  Description:
*           update quantity & product data in non-consolidated index file. V3.3
*==========================================================================
*/
SHORT TrnStoNoIdxUpdate( ITEMSALES          *pItemSales,
                       TRANSTOSALESSRCH   *pNormalItem,
                       TRANSTOSALESSRCH   *pVoidItem,
                       TrnStorageType     sStorageType,
                       SHORT              sType)
{
    SHORT       hsIndexFile;
    PRTIDXHDR   IdxFileInfo;
    UCHAR       auchIdxWork[ sizeof( PRTIDX ) * 50 ];
    BOOL        fUpdateIdx = FALSE;
    ULONG       ulTtlIdxReadSize;
    ULONG       ulCurIdxReadSize;
    PRTIDX      *pPrtIdxCurrent;
    UCHAR       uchMaxItemCo;
    UCHAR       uchItemCo;
    USHORT      usTargetOffset;
    SHORT       sReducePPI = 1;
	ULONG		ulActualBytesRead;//RPH 11-11-3 SR# 201

	if ((hsIndexFile = TrnDetermineIndexFileHandle(sStorageType)) < 0) {
		return (TRN_ERROR);
	}

    /* --- determine whether inputed sales item already exists in storage,
        and increments QTY & Product --- */
    if (( pNormalItem->usItemOffset ) ) {
        usTargetOffset = pNormalItem->usItemOffset;
        fUpdateIdx = TRUE;
    } else if (( pVoidItem->usItemOffset )) {
        usTargetOffset = pVoidItem->usItemOffset;
        fUpdateIdx = TRUE;
	} else if (( pNormalItem->usItemOffset ) && ( pItemSales->fbModifier & VOID_MODIFIER )) {
		/* if void is executed */
        usTargetOffset = pNormalItem->usItemOffset;
        if (sType == REDUCE_ITEM) {
        	usTargetOffset = pVoidItem->usItemOffset;
        }
        fUpdateIdx = TRUE;
    } else if (( pItemSales->usItemOffset ) && ( ! ( pItemSales->fbModifier & VOID_MODIFIER ))) {
		/* if void is executed previously */
        usTargetOffset = pItemSales->usItemOffset;
        fUpdateIdx = TRUE;
    } else if (( pVoidItem->usItemOffset ) && ( ! ( pItemSales->fbModifier & VOID_MODIFIER ))) {
		/* if void is executed at first */
        usTargetOffset = pVoidItem->usItemOffset;
        fUpdateIdx = TRUE;
    }

    if ( ! fUpdateIdx ) {
        return ( TRN_SUCCESS );
    }

    TrnReadFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), hsIndexFile , &ulActualBytesRead);

    ulTtlIdxReadSize = 0;

    while ( ulTtlIdxReadSize < IdxFileInfo.usIndexVli ) {
		TrnReadFile( (sizeof( PRTIDXHDR ) + ulTtlIdxReadSize), auchIdxWork, sizeof( auchIdxWork ), hsIndexFile, &ulCurIdxReadSize);
        if ( IdxFileInfo.usIndexVli < ( ulTtlIdxReadSize + ulCurIdxReadSize )) {
            ulCurIdxReadSize = IdxFileInfo.usIndexVli - ulTtlIdxReadSize;
        }
        ulTtlIdxReadSize += ulCurIdxReadSize;

        /* --- retrieve item data in storage and print its data --- */
        pPrtIdxCurrent = ( PRTIDX * )( auchIdxWork );
        uchMaxItemCo   = ( UCHAR )(( ulCurIdxReadSize < sizeof( auchIdxWork )) ?
                         ( ulCurIdxReadSize / sizeof( PRTIDX )):
                         ( sizeof( auchIdxWork ) / sizeof( PRTIDX )));

        for ( uchItemCo = 0; uchItemCo < uchMaxItemCo; uchItemCo++, pPrtIdxCurrent++ ) {

			if (( pPrtIdxCurrent->usItemOffset == usTargetOffset)) {   /* pair item */
				// This index is the correct index for this item.  Both the offset to the storage area and the unique Id for
				// the item matches this index.
				if (((pItemSales->uchPPICode)||(pItemSales->uchPM)) && (sType == REDUCE_ITEM)) {
					if ((pPrtIdxCurrent->lProduct == pItemSales->lProduct) && (pPrtIdxCurrent->lQTY == pItemSales->lQTY)) {
	                    /* clear qty for void print option */
		                pPrtIdxCurrent->lQTY = 0L;
                        pPrtIdxCurrent->sNetQTY     = 0;    /* clear as void item */
                        pPrtIdxCurrent->lProduct = 0;
                        pPrtIdxCurrent->usDeptNo = 0;

						sReducePPI--;
						if (sReducePPI < 0) break;	/* search index of reduce item */
                        continue;
					}
				}

                /* --- reduce index file qty & product for non-void non-consoli. print, V3.3 --- */
				pPrtIdxCurrent->lQTY     += pItemSales->lQTY;
                pPrtIdxCurrent->lProduct += pItemSales->lProduct;
            }
        }

        TrnWriteFile( (sizeof( PRTIDXHDR ) + ulTtlIdxReadSize - ulCurIdxReadSize), auchIdxWork, ulCurIdxReadSize, hsIndexFile );
    }
    return ( TRN_SUCCESS );

}

/* 12/05/01 */
SHORT	cmpIdx(const TRNPLUMNEMO * ptr1,const TRNPLUMNEMO * ptr2);
static SHORT (*pComp)(VOID *pKey, VOID *pusHPoint) = cmpIdx;

/* 05/20/01 */
/* 12/05/01 */
VOID TrnStoItemSalesPLUMnemo( ITEMSALES *pItemSales)
{

	if (pItemSales->uchMajorClass != CLASS_ITEMSALES) return;

    PifRequestSem(husTrnSemaphore);

	if ((pItemSales->uchMinorClass == CLASS_PLU) ||
		(pItemSales->uchMinorClass == CLASS_PLUITEMDISC) ||
		(pItemSales->uchMinorClass == CLASS_PLUMODDISC) ||
		(pItemSales->uchMinorClass == CLASS_SETMENU) ||
		(pItemSales->uchMinorClass == CLASS_SETITEMDISC) ||
		(pItemSales->uchMinorClass == CLASS_SETMODDISC))
	{
		USHORT i;
		USHORT j, usNoOfChild;

    	usNoOfChild = pItemSales->uchCondNo + pItemSales->uchPrintModNo + pItemSales->uchChildNo;

		if (usItemStoredPluMnem == 0) {
			_tcsncpy(TrnItemStorePluMnem.aPluMnemo[usItemStoredPluMnem].auchPLUNo, pItemSales->auchPLUNo, NUM_PLU_LEN);
    		_tcsncpy(TrnItemStorePluMnem.aPluMnemo[usItemStoredPluMnem].aszMnemonic, pItemSales->aszMnemonic, NUM_DEPTPLU);
    		_tcsncpy(TrnItemStorePluMnem.aPluMnemo[usItemStoredPluMnem].aszMnemonicAlt, pItemSales->aszMnemonicAlt, NUM_DEPTPLU);
    		usItemStoredPluMnem++;
    	} else if (usItemStoredPluMnem < TRN_ITEMSTORE_SIZE) {
			USHORT *pusHitPoint;
			TRNPLUMNEMO TrnPluMnemo = {0};

			_tcsncpy(TrnPluMnemo.auchPLUNo, pItemSales->auchPLUNo, NUM_PLU_LEN);
			if (Rfl_SpBsearch(&TrnPluMnemo, sizeof(TRNPLUMNEMO), &TrnItemStorePluMnem, usItemStoredPluMnem, &pusHitPoint, pComp) != RFL_HIT) {
				USHORT offusSearchPoint;

    			offusSearchPoint = (UCHAR *)pusHitPoint - (UCHAR *)&TrnItemStorePluMnem;
		    	offusSearchPoint /= sizeof(TRNPLUMNEMO);
				for (i = usItemStoredPluMnem; i > offusSearchPoint; i--) {
					TrnItemStorePluMnem.aPluMnemo[i] = TrnItemStorePluMnem.aPluMnemo[i-1];
		    	}
				_tcsncpy(TrnItemStorePluMnem.aPluMnemo[i].auchPLUNo, pItemSales->auchPLUNo, NUM_PLU_LEN);
	    		_tcsncpy(TrnItemStorePluMnem.aPluMnemo[i].aszMnemonic, pItemSales->aszMnemonic, NUM_DEPTPLU);
    			_tcsncpy(TrnItemStorePluMnem.aPluMnemo[i].aszMnemonicAlt, pItemSales->aszMnemonicAlt, NUM_DEPTPLU);
	    		usItemStoredPluMnem++;
			}
    	}

		for ( j = 0; j < usNoOfChild; j++) {
			TRNPLUMNEMO TrnPluMnemo = {0};

			_tcsncpy(TrnPluMnemo.auchPLUNo, pItemSales->Condiment[j].auchPLUNo, NUM_PLU_LEN);

			if (usItemStoredPluMnem < TRN_ITEMSTORE_SIZE) {
				USHORT *pusHitPoint;

				if (Rfl_SpBsearch(&TrnPluMnemo, sizeof(TRNPLUMNEMO), &TrnItemStorePluMnem, usItemStoredPluMnem, &pusHitPoint, pComp) != RFL_HIT) {
					USHORT offusSearchPoint;

	    			offusSearchPoint = (UCHAR *)pusHitPoint - (UCHAR *)&TrnItemStorePluMnem;
			    	offusSearchPoint /= sizeof(TRNPLUMNEMO);
					for (i = usItemStoredPluMnem; i > offusSearchPoint; i--) {
						TrnItemStorePluMnem.aPluMnemo[i] = TrnItemStorePluMnem.aPluMnemo[i-1];
		    		}
					_tcsncpy(TrnItemStorePluMnem.aPluMnemo[i].auchPLUNo, pItemSales->Condiment[j].auchPLUNo, NUM_PLU_LEN);
		    		_tcsncpy(TrnItemStorePluMnem.aPluMnemo[i].aszMnemonic, pItemSales->Condiment[j].aszMnemonic, NUM_DEPTPLU);
		    		_tcsncpy(TrnItemStorePluMnem.aPluMnemo[i].aszMnemonicAlt, pItemSales->Condiment[j].aszMnemonicAlt, NUM_DEPTPLU);
		    		usItemStoredPluMnem++;
				}
			} else {
				break;
    		}
		}
	}

    PifReleaseSem(husTrnSemaphore);
}

/* 05/20/01 */
/* 12/05/01 */
USHORT TrnItemGetPLUMnemo(TCHAR  *pszPLUMnem, TCHAR  *pszPLUMnemAlt, TCHAR  *auchPLUNo)
{
    USHORT   *pusHitPoint;
	USHORT   usResult = FALSE;
	TRNPLUMNEMO TrnPluMnemo = {0};

    PifRequestSem(husTrnSemaphore);

	_tcsncpy(TrnPluMnemo.auchPLUNo, auchPLUNo, NUM_PLU_LEN);

	if (Rfl_SpBsearch(&TrnPluMnemo, sizeof(TRNPLUMNEMO), &TrnItemStorePluMnem, usItemStoredPluMnem, &pusHitPoint, pComp) == RFL_HIT) {
		USHORT offusSearchPoint = (UCHAR *)pusHitPoint - (UCHAR *)&TrnItemStorePluMnem;

    	offusSearchPoint /= sizeof(TRNPLUMNEMO);
		if (pszPLUMnem) {
			_tcsncpy( pszPLUMnem, TrnItemStorePluMnem.aPluMnemo[offusSearchPoint].aszMnemonic, NUM_DEPTPLU);
			*(pszPLUMnem + STD_PLU_MNEMONIC_LEN) = 0;
		}
		if (pszPLUMnemAlt) {
			_tcsncpy( pszPLUMnemAlt, TrnItemStorePluMnem.aPluMnemo[offusSearchPoint].aszMnemonicAlt, NUM_DEPTPLU);
			*(pszPLUMnemAlt + STD_PLU_MNEMONIC_LEN) = 0;
		}
	    usResult = TRUE;
	}

    PifReleaseSem(husTrnSemaphore);
	return usResult;
}

VOID TrnItemStoPLUMnemo(TCHAR  *pszPLUMnem, TCHAR  *pszPLUMnemAlt, TCHAR  *auchPLUNo)
{
    PifRequestSem(husTrnSemaphore);

	if (usItemStoredPluMnem == 0) {
		_tcsncpy(TrnItemStorePluMnem.aPluMnemo[usItemStoredPluMnem].auchPLUNo, auchPLUNo, NUM_PLU_LEN);
		if (pszPLUMnem) {
    		_tcsncpy(TrnItemStorePluMnem.aPluMnemo[usItemStoredPluMnem].aszMnemonic, pszPLUMnem, NUM_DEPTPLU);
		}
		if (pszPLUMnemAlt) {
    		_tcsncpy(TrnItemStorePluMnem.aPluMnemo[usItemStoredPluMnem].aszMnemonicAlt, pszPLUMnemAlt, NUM_DEPTPLU);
		}
    	usItemStoredPluMnem++;
    } else if (usItemStoredPluMnem < TRN_ITEMSTORE_SIZE) {
		USHORT  *pusHitPoint;
		TRNPLUMNEMO TrnPluMnemo = {0};

		_tcsncpy(TrnPluMnemo.auchPLUNo, auchPLUNo, NUM_PLU_LEN);
		if (Rfl_SpBsearch(&TrnPluMnemo, sizeof(TRNPLUMNEMO), &TrnItemStorePluMnem, usItemStoredPluMnem, &pusHitPoint, pComp) != RFL_HIT) {
				USHORT  i;
				USHORT  offusSearchPoint = (UCHAR *)pusHitPoint - (UCHAR *)&TrnItemStorePluMnem;

		    	offusSearchPoint /= sizeof(TRNPLUMNEMO);
				for (i = usItemStoredPluMnem; i > offusSearchPoint; i--) {
					TrnItemStorePluMnem.aPluMnemo[i] = TrnItemStorePluMnem.aPluMnemo[i-1];
		    	}
				_tcsncpy(TrnItemStorePluMnem.aPluMnemo[i].auchPLUNo, auchPLUNo, NUM_PLU_LEN);
	    		if (pszPLUMnem) _tcsncpy(TrnItemStorePluMnem.aPluMnemo[i].aszMnemonic, pszPLUMnem, NUM_DEPTPLU);
	    		if (pszPLUMnemAlt) _tcsncpy(TrnItemStorePluMnem.aPluMnemo[i].aszMnemonicAlt, pszPLUMnemAlt, NUM_DEPTPLU);
	    		usItemStoredPluMnem++;
		}
   	}
    PifReleaseSem(husTrnSemaphore);
}

SHORT		cmpIdx(const TRNPLUMNEMO * ptr1,const TRNPLUMNEMO * ptr2){
	return	_tcsncmp(ptr1->auchPLUNo,ptr2->auchPLUNo,NUM_PLU_LEN);	/* plunumber */
}

/*
*==========================================================================
**    Synopsis: SHORT TrnStoItemSearchAll(ITEMSALES          *pItemSales,
                         SHORT              sStorageType)
*
*       Input:  ITEMSALES          *pItemSales - item ssales tructure
*               SHORT             sStorageType   - type of trans storage
*                       TRN_TYPE_ITEMSTORAGE, TRN_TYPE_CONSSTORAGE
*      Output:  Nothing
*       InOut:  Nothing
*
*    Return:    TRN_SUCCESS     : item found and consolidate it
*               TRN_ERROR       : not found
*
**  Description:	This function reads the entire transaction file
*					and returns each item sales structure, so that
*					the function calling this can view each item in the 
*					transaction.
*==========================================================================
*/
SHORT TrnStoItemSearchAll(ITEMSALES *pItemSales, TrnStorageType sStorageType)
{
    static ULONG    ulReadPosition = 0;
    static USHORT   usTargetItemSize;
    ULONG   ulMaxStoSize;
    SHORT   sReturn;
    SHORT   hsStorage;
    USHORT  usStorageVli;
    USHORT  usStorageHdrSize;
    UCHAR   auchTranStorageWork[ TRN_STOSIZE_BLOCK ];

	if (pItemSales == NULL || sStorageType == -1)
	{
		ulMaxStoSize = 0;
		ulReadPosition = 0;
		usTargetItemSize = 0;
		return (TRN_SUCCESS);
	}

    sReturn = TrnStoGetStorageInfo( sStorageType, &hsStorage, &usStorageHdrSize, &usStorageVli );
    if (sReturn != TRN_SUCCESS ) {
		ulMaxStoSize = 0;
		ulReadPosition = 0;
		usTargetItemSize = 0;
        return ( sReturn );
    }

    /* --- calculate size of source sales item data --- */
	if(ulReadPosition == 0)
	{
		ulReadPosition = usStorageHdrSize;
	}
    ulMaxStoSize = usStorageHdrSize + usStorageVli;

	while (ulReadPosition < ulMaxStoSize ) {
        /* --- read item data, and store to work buffer --- */
		sReturn = TrnStoIsItemInWorkBuff(hsStorage, ulReadPosition, auchTranStorageWork, sizeof( auchTranStorageWork ), &usTargetItemSize);
		if(usTargetItemSize == 0)
		{
			break;
		}
		if (sReturn == TRN_ERROR) {
			NHPOS_ASSERT_TEXT((sReturn != TRN_ERROR), "**ERROR: TrnStoIsItemInWorkBuff() item record size exceeds buffer");
			return TRN_ERROR;
		}

		/* --- determine target item is sales item or not --- */
		//get the item, and add the amount that the coupon has already
		//used against the transaction. JHHJ
		if (0 != RflGetStorageItemRequire (CLASS_ITEMSALES, pItemSales, auchTranStorageWork, RFL_WITHOUT_MNEM )) {
			ulReadPosition += usTargetItemSize;
			return ( TRN_SUCCESS);
	    }

        /* --- increment read position to read next work buffer --- */
        ulReadPosition += usTargetItemSize;
    }

    return ( TRN_END_OF_FILE );
}

/****** End of Source ******/


