/*=======================================================================*\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  2170         *             NCR Corporation, E&M OISO        ||
||        * << ECR-90 >>  *                                              ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995-1996      ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
:   Title          : TRANSACTION COUPON MODULE
:   Category       : TRANSACTION MODULE, NCR 2170 US Hospitality Application
:   Program Name   : TRNCPN.C
:  ---------------------------------------------------------------------
:   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.
:   Memory Model       : Midium Model
:   Options            : /c /AM /W4 /G1s /Os /Za /Zp
:  ---------------------------------------------------------------------
:  Abstract:
:
:#  TrnStoVoidCouponItem()  : add ( REL 3.0 )
:#  TrnStoIsSalesLinked()   : add ( REL 3.0 )
:#  TrnStoMakeVoidCpn()     : add ( REL 3.0 )
:#  TrnStoCouponSearch()    : add ( REL 3.0 )
:#  TrnStoCpnItemSearch()   : add ( REL 3.0 )
:#  TrnStoCpnPLUSearch()    : add ( REL 3.0 )
:#  TrnStoCpnDeptSearch()   : add ( REL 3.0 )
:#  TrnStoIsCouponItem()    : add ( REL 3.0 )
:#  TrnStoIsAllItemFound()  : add ( REL 3.0 )
:#  TrnStoUpdateCpnCo()     : add ( REL 3.0 )
:#  TrnStoUpdateVoidCo()    : add ( REL 3.0 )
:#  TrnStoCpnVoidSearch()   : add ( REL 3.0 )
:#  TrnStoIsCpnVoidItem()   : add ( REL 3.0 )
:#  TrnStoQueryCpnType()    : add ( REL 3.0 )
:
:  ---------------------------------------------------------------------
:  Update Histories
:  Date     : Ver.Rev. :   Name     : Description
: Jun-20-95 : 01.00.00 : T.Nakahata : Initial ( Release 3.0 )
: Jun-30-95 : 01.00.01 : T.Nakahata : fixed no. of void coupon calculation
: Jul-05-95 : 01.00.02 : T.Nakahata : reduce search buffer size (1K->0.5K)
: Aug-29-95 : 01.00.05 : O.Nakada   : corrected TrnStoIsAllItemFound()
: May-27-96 : 03.01.07 : T.Nakahata : correctes OR logic
: Jun-24-96 : 03.01.09 : M.Ozawa    : correctes logic of update coupon co. at item transfer
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
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

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

typedef struct tagTRNUPCTBL {
    UCHAR   uchQty;                         /* quantity */
    SHORT   sAmount;                        /* amount   */
} TRNUPCTBL;


#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

/*
*===========================================================================
*   Static Work Area 
*===========================================================================
*/

/* after 1/1/1995

		table for the value code in UPC coupon.

	This table is created from data provided by the Joint Industry Coupon Committee
	which manages the standard Coupon Value Code Table.

	The code is a two digit code, the last two digits of the UPC read in, which
	represents different coupon values and purchase requirements.

	Examples are:
		Code	Purchase Requirements 
		05		$1.40 off one 
		07		Buy three or more, get $1.50 off 
		17		$2.10 off one 

	See http://www.barcode-us.com/support_desk/coupon.htm

	The basic UPC coupon code is comprised of 5 pieces.

	1. Number System Character - all coupons should have an NSC of 5.

	2. Manufacturer's Number - five (5) digit manufacturer's code which should
		be the same as what is on the product itself.

	3. Family Code - a three (3) digit code after the Manufacturer's Number which
		indicates the product family.  There may be multiple individual products
		within a product family due to selections of color, size, etc.

		Family code 992 is for "no product verification". Technically, the 992 is a
		message to the *computer* to not verify whether you bought the item. Its
		intended use is for things that don't fall into a 'regular' family - like
		'$1 off any Heinz X or Campbell's Y or BallPark Z'. Wine tags are often
		992's because if you get to save off *any* seafood, there's no way to know
		which seafood you will choose, and no other way to encode it.

	4. Value Code - a two (2) digit code which identifies the redemption value of the
		coupon.

	In addition to the standard UPC coupon code, the FMI/GMA Coupon Scanning Subcommittee
	of the Joint Industry Coupon Committee has developed an additional bar code symbol with
	additional information.  Beginning Jan-1997 all coupons are to have this additional 
	information.  It has 4 fields.

	1. Clearing House Code - a five (5) digit number indicating the coupon clearing
		house or redemption center.
  
	1. Number System Character - 

	2. Offer Code - a five (5) digit number issued by the manufacturer

	3. Expiration Date of Offer - a four (4) digit number of MMYY

*/
/*
	Edit History of this Table
	---------------------------------------------------
	R.Chambers		Jan-20-2004		SR 133
		-----------------------------------------------
		Made the following changes:
		Valid New Value Codes as of July 1, 2000
			Value Code Purchase Requirements 
			05 $1.40 off one 
			07 Buy three or more, get $1.50 off 
			08 Buy two or more, get $3.00 off 
			09 Buy three or more, get $2.00 off 
			13 Buy four or more, get $1.00 off 

		Invalid Value Codes as of July 1, 2000
			Value Code Purchase Requirements 
			17 $2.10 off one 
			94 $4.25 off one 
			97 $4.75 off one
		-----------------------------------------------

 */
TRNUPCTBL FARCONST aTrnUPC[] = {{1,    TRN_CHECKER},    /* code = 00 */
                                {1,       TRN_FREE},    /* code = 01 */
                                {5,  TRN_BUY4_GET1},     /* code = 02 */
                                {1,            110},    /* code = 03 */
                                {1,            135},    /* code = 04 */
                                {1,            140},    /* code = 05 */
                                {1,            160},    /* code = 06 */
                                {3,            150},    /* code = 07 */
                                {2,            300},    /* code = 08 */
                                {3,            200},    /* code = 09 */
                                {1,             10},    /* code = 10 */
                                {1,            185},    /* code = 11 */
                                {1,             12},    /* code = 12 */
                                {4,            100},    /* code = 13 */
                                {2,  TRN_BUY1_GET1},    /* code = 14 */
                                {1,             15},    /* code = 15 */
                                {3,  TRN_BUY2_GET1},    /* code = 16 */
                                {0,              0},    /* code = 17 */
                                {1,            260},    /* code = 18 */
                                {4,  TRN_BUY3_GET1},    /* code = 19 */
                                {1,             20},    /* code = 20 */
                                {2,             35},    /* code = 21 */
                                {2,             40},    /* code = 22 */
                                {2,             45},    /* code = 23 */
                                {2,             50},    /* code = 24 */
                                {1,             25},    /* code = 25 */
                                {1,            285},    /* code = 26 */
                                {0,              0},    /* code = 27 */
                                {2,             55},    /* code = 28 */
                                {1,             29},    /* code = 29 */
                                {1,             30},    /* code = 30 */
                                {2,             60},    /* code = 31 */
                                {2,             75},    /* code = 32 */
                                {2,            100},    /* code = 33 */
                                {2,            125},    /* code = 34 */
                                {1,             35},    /* code = 35 */
                                {2,            150},    /* code = 36 */
                                {3,             25},    /* code = 37 */
                                {3,             30},    /* code = 38 */
                                {1,             39},    /* code = 39 */
                                {1,             40},    /* code = 40 */
                                {3,             50},    /* code = 41 */
                                {3,            100},    /* code = 42 */
                                {2,            110},    /* code = 43 */
                                {2,            135},    /* code = 44 */
                                {1,             45},    /* code = 45 */
                                {2,            160},    /* code = 46 */
                                {2,            175},    /* code = 47 */
                                {2,            185},    /* code = 48 */
                                {1,             49},    /* code = 49 */
                                {1,             50},    /* code = 50 */
                                {2,            200},    /* code = 51 */
                                {3,             55},    /* code = 52 */
                                {2,             10},    /* code = 53 */
                                {2,             15},    /* code = 54 */
                                {1,             55},    /* code = 55 */
                                {2,             20},    /* code = 56 */
                                {2,             25},    /* code = 57 */
                                {2,             30},    /* code = 58 */
                                {1,             59},    /* code = 59 */
                                {1,             60},    /* code = 60 */
                                {1,           1000},    /* code = 61 */
                                {1,            950},    /* code = 62 */
                                {1,            900},    /* code = 63 */
                                {1,            850},    /* code = 64 */
                                {1,             65},    /* code = 65 */
                                {1,            800},    /* code = 66 */
                                {1,            750},    /* code = 67 */
                                {1,            700},    /* code = 68 */
                                {1,             69},    /* code = 69 */
                                {1,             70},    /* code = 70 */
                                {1,            650},    /* code = 71 */
                                {1,            600},    /* code = 72 */
                                {1,            550},    /* code = 73 */
                                {1,            500},    /* code = 74 */
                                {1,             75},    /* code = 75 */
                                {1,            100},    /* code = 76 */
                                {1,            125},    /* code = 77 */
                                {1,            150},    /* code = 78 */
                                {1,             79},    /* code = 79 */
                                {1,             80},    /* code = 80 */
                                {1,            175},    /* code = 81 */
                                {1,            200},    /* code = 82 */
                                {1,            225},    /* code = 83 */
                                {1,            250},    /* code = 84 */
                                {1,             85},    /* code = 85 */
                                {1,            275},    /* code = 86 */
                                {1,            300},    /* code = 87 */
                                {1,            325},    /* code = 88 */
                                {1,             89},    /* code = 89 */
                                {1,             90},    /* code = 90 */
                                {1,            350},    /* code = 91 */
                                {1,            375},    /* code = 92 */
                                {1,            400},    /* code = 93 */
                                {0,              0},    /* code = 94 */
                                {1,             95},    /* code = 95 */
                                {1,            450},    /* code = 96 */
                                {0,              0},    /* code = 97 */
                                {2,             65},     /* code = 98 */
                                {1,             99}};   /* code = 99 */

#if 0

TRNUPCTBL FARCONST aTrnUPC[] = {{1,    TRN_CHECKER},    /* code = 00 */
                                {1,       TRN_FREE},    /* code = 01 */
                                {0,              0},    /* code = 02 */
                                {1,            110},    /* code = 03 */
                                {1,            135},    /* code = 04 */
                                {1,              5},    /* code = 05 */
                                {1,            160},    /* code = 06 */
                                {1,              7},    /* code = 07 */
                                {1,              8},    /* code = 08 */
                                {1,              9},    /* code = 09 */
                                {1,             10},    /* code = 10 */
                                {1,            185},    /* code = 11 */
                                {1,             12},    /* code = 12 */
                                {1,             13},    /* code = 13 */
                                {2,  TRN_BUY1_GET1},    /* code = 14 */
                                {1,             15},    /* code = 15 */
                                {3,  TRN_BUY2_GET1},    /* code = 16 */
                                {1,            210},    /* code = 17 */
                                {1,            260},    /* code = 18 */
                                {4,  TRN_BUY3_GET1},    /* code = 19 */
                                {1,             20},    /* code = 20 */
                                {2,             35},    /* code = 21 */
                                {2,             40},    /* code = 22 */
                                {2,             45},    /* code = 23 */
                                {2,             50},    /* code = 24 */
                                {1,             25},    /* code = 25 */
                                {1,            285},    /* code = 26 */
                                {1,             27},    /* code = 27 */
                                {2,             55},    /* code = 28 */
                                {1,             29},    /* code = 29 */
                                {1,             30},    /* code = 30 */
                                {2,             60},    /* code = 31 */
                                {2,             75},    /* code = 32 */
                                {2,            100},    /* code = 33 */
                                {2,            125},    /* code = 34 */
                                {1,             35},    /* code = 35 */
                                {2,            150},    /* code = 36 */
                                {3,             25},    /* code = 37 */
                                {3,             30},    /* code = 38 */
                                {1,             39},    /* code = 39 */
                                {1,             40},    /* code = 40 */
                                {3,             50},    /* code = 41 */
                                {3,            100},    /* code = 42 */
                                {2,            110},    /* code = 43 */
                                {2,            135},    /* code = 44 */
                                {1,             45},    /* code = 45 */
                                {2,            160},    /* code = 46 */
                                {2,            175},    /* code = 47 */
                                {2,            185},    /* code = 48 */
                                {1,             49},    /* code = 49 */
                                {1,             50},    /* code = 50 */
                                {2,            200},    /* code = 51 */
                                {0,              0},    /* code = 52 */
                                {2,             10},    /* code = 53 */
                                {2,             15},    /* code = 54 */
                                {1,             55},    /* code = 55 */
                                {2,             20},    /* code = 56 */
                                {2,             25},    /* code = 57 */
                                {2,             30},    /* code = 58 */
                                {1,             59},    /* code = 59 */
                                {1,             60},    /* code = 60 */
                                {1,           1000},    /* code = 61 */
                                {1,            950},    /* code = 62 */
                                {1,            900},    /* code = 63 */
                                {1,            850},    /* code = 64 */
                                {1,             65},    /* code = 65 */
                                {1,            800},    /* code = 66 */
                                {1,            750},    /* code = 67 */
                                {1,            700},    /* code = 68 */
                                {1,             69},    /* code = 69 */
                                {1,             70},    /* code = 70 */
                                {1,            650},    /* code = 71 */
                                {1,            600},    /* code = 72 */
                                {1,            550},    /* code = 73 */
                                {1,            500},    /* code = 74 */
                                {1,             75},    /* code = 75 */
                                {1,            100},    /* code = 76 */
                                {1,            125},    /* code = 77 */
                                {1,            150},    /* code = 78 */
                                {1,             79},    /* code = 79 */
                                {1,             80},    /* code = 80 */
                                {1,            175},    /* code = 81 */
                                {1,            200},    /* code = 82 */
                                {1,            225},    /* code = 83 */
                                {1,            250},    /* code = 84 */
                                {1,             85},    /* code = 85 */
                                {1,            275},    /* code = 86 */
                                {1,            300},    /* code = 87 */
                                {1,            325},    /* code = 88 */
                                {1,             89},    /* code = 89 */
                                {1,             90},    /* code = 90 */
                                {1,            350},    /* code = 91 */
                                {1,            375},    /* code = 92 */
                                {1,            400},    /* code = 93 */
                                {1,            425},    /* code = 94 */
                                {1,             95},    /* code = 95 */
                                {1,            450},    /* code = 96 */
                                {1,            475},    /* code = 97 */
                                {0,              0},    /* code = 98 */
                                {1,             99}};   /* code = 99 */

#endif

/*
*==========================================================================
**  Synopsis : SHORT TrnStoVoidCouponItem( UCHAR  *puchSalesItem,
*                                          USHORT usSalesOffset,
*                                          SHORT  sStorageType )
*
*       Input: UCHAR  *puchSalesItem - address of coupon target sales item
*              USHORT usSalesOffset  - coupon target sales item offset
*              SHORT  sStorageType  - type of transaction storage
*                           TRN_TYPE_ITEMSTORAGE - item storage
*                           TRN_TYPE_CONSSTORAGE - consolidation storage
*      Output: Nothing
*       InOut: Nothing
*
*    Return:   TRN_SUCCESS - function is success
*              TRN_ERROR   - function is failed
*
**  Description:
*       void coupon item
*==========================================================================
*/

SHORT TrnStoVoidCouponItem( ITEMSALES *pItemSales,
						    UCHAR  *puchSalesItem,
                            USHORT usSalesOffset,
                            SHORT  sStorageType )
{
    TRANSTORAGESALESNON * const pTran = TRANSTORAGENONPTR( puchSalesItem );
    ULONG   const fulTrnPrtSts = TrnInformation.TranCurQual.flPrintStatus;
    ULONG   const fusTrnGCFSts = TrnInformation.TranGCFQual.fsGCFStatus;
    ULONG   const fusTrnCurSts = TrnInformation.TranCurQual.fsCurStatus;
    ULONG   ulMaxStoSize;
    ULONG   ulReadPosition;
    SHORT   sSalesCpnCo;
    SHORT   sCurrentCpnCo;
    SHORT   hsStorage;
    USHORT  usStorageVli;
    USHORT  usStorageHdrSize;
    SHORT   sReturn;
    SHORT   sIncrementCo;
    USHORT  fStoreRecall;
    USHORT  fCashOpe;
    USHORT  fPostChkGCOpe;
    UCHAR   const uchValidPrtCpn = CliParaMDCCheck( MDC_COMBCOUPON_ADR, ODD_MDC_BIT1 );  // determine validation print is compulsory or not
    UCHAR   const uchValidPrtVoid = CliParaMDCCheck( MDC_VOID_ADR, ODD_MDC_BIT1 );       // determine validation print is compulsory or not
    UCHAR   auchTranStoWork[ TRN_TEMPBUFF_SIZE ];
    TRANSTORAGECOUPONNON *pCpn;

    pCpn = TRANSTORAGENONPTR( auchTranStoWork );

    fStoreRecall = (( fulTrnPrtSts & CURQUAL_GC_SYS_MASK ) == CURQUAL_STORE_RECALL );
    fCashOpe     = (( fusTrnCurSts & CURQUAL_OPEMASK ) == CURQUAL_CASHIER );
	sIncrementCo = ( fusTrnCurSts & CURQUAL_PRESELECT_MASK ) ? ( -1 ) : 1;

    fPostChkGCOpe = (( fulTrnPrtSts & CURQUAL_POSTCHECK ) && (( fusTrnCurSts & CURQUAL_OPEMASK ) != CURQUAL_CASHIER ));

    /* --- determine coupon counter of source item --- */
    sCurrentCpnCo  = 0;
    sSalesCpnCo = 0;
    if ( pTran->sCouponQTY < 0 ) {
        if ( 0 < pTran->sCouponCo ) {
            sSalesCpnCo = pTran->sCouponCo;
        } else if ( pTran->sCouponCo < pTran->sCouponQTY ) {
            sSalesCpnCo = pTran->sCouponQTY - pTran->sCouponCo;
        }
    } else {    /* 0 <= Coupon Qty */
        if ( pTran->sCouponCo < 0 ) {
            sSalesCpnCo = pTran->sCouponCo * ( -1 );
        } else if ( pTran->sCouponQTY < pTran->sCouponCo ) {
            sSalesCpnCo = pTran->sCouponCo - pTran->sCouponQTY;
        }
    }

    sReturn = TrnStoGetStorageInfo( sStorageType, &hsStorage, &usStorageHdrSize, &usStorageVli );
    if (sReturn != TRN_SUCCESS ) {
        return ( sReturn );
    }

    ulReadPosition = usStorageHdrSize;
    ulMaxStoSize   = ulReadPosition + usStorageVli;
    while (( ulReadPosition < ulMaxStoSize ) && ( sCurrentCpnCo < sSalesCpnCo )) {
		USHORT  usTargetItemSize;

        /* --- read item data, and store to work buffer --- */
		sReturn = TrnStoIsItemInWorkBuff(hsStorage, ulReadPosition, auchTranStoWork, sizeof( auchTranStoWork ), &usTargetItemSize);
		if (sReturn == TRN_ERROR) {
			NHPOS_ASSERT_TEXT((sReturn != TRN_ERROR), "**ERROR: TrnStoIsItemInWorkBuff() item record size exceeds buffer");
			return TRN_ERROR;
		}

        /* --- determine this item is coupon item or not --- */
        if (( pCpn->uchMajorClass == CLASS_ITEMCOUPON ) &&
            (( pCpn->uchMinorClass == CLASS_COMCOUPON ) || ( pCpn->uchMinorClass == CLASS_UPCCOUPON ))) {

            /* --- determine coupon item links source sales item --- */
            if ( TrnStoIsSalesLinked( auchTranStoWork, usSalesOffset )) {
				SHORT       sSize;
				ITEMCOUPON  VoidCpn = {0};
				UCHAR       auchBuffer[ TRN_WORKBUFFER ];

                /* --- void this coupon item, and clear all linked item --- */
                TrnStoMakeVoidCpn( auchTranStoWork, &VoidCpn );
                TrnItemCoupon( &VoidCpn );

                /* --- compress coupon, and store compressed data to work buffer --- */
                /* --- are there a vacant area in item & consoli. storages ? --- */
                sSize = TrnStoCouponComp( &VoidCpn, &auchBuffer[ 0 ] );
                if (( sReturn = TrnChkStorageSize( sSize )) != TRN_SUCCESS ) {
                    return ( sReturn );
                }

                /* --- store compressed coupon data to each storages --- */
                /* --- if void coupon is entered, update void counter of target coupon item --- */
                pCpn->sVoidCo += sIncrementCo;

                TrnWriteFile( ulReadPosition, auchTranStoWork, (sizeof( TRANSTORAGECOUPONNON ) + sizeof( UCHAR )), hsStorage );

                /* --- update coupon status of target sales items --- */
                if (( sReturn = TrnStoUpdateCpnCo( &VoidCpn, sStorageType, pTran )) != TRN_SUCCESS ) {
                    return ( sReturn );
                }        

                if ( sStorageType == TRN_TYPE_ITEMSTORAGE ) {
                    TrnStoItemNormal( &auchBuffer[0], sSize );
                } else {
                    TrnStoConsNormal( &auchBuffer[0], sSize );

                    if ( ! ( VoidCpn.fbStorageStatus & NOT_ITEM_STORAGE )) {
                        if (( fStoreRecall && fCashOpe ) || ( fPostChkGCOpe )) {
                            TrnStoItemNormal( &auchBuffer[0], sSize );
                        }
                    }
                }

                /* --- update single type coupon status of GCF qualifier --- */
                TrnStoUpdateCouponType( &VoidCpn );

                /* --- if validation print is compulsory, execute validation print --- */
                if ( uchValidPrtCpn || uchValidPrtVoid ) {
                    VoidCpn.fsPrintStatus = PRT_VALIDATION;
                    TrnThrough( &VoidCpn );
                }
                VoidCpn.fsPrintStatus = ( PRT_SLIP | PRT_JOURNAL );
                ItemCouponVoidDisp( pItemSales, &VoidCpn );
                TrnPrintType( &VoidCpn );

                sCurrentCpnCo++;
            }
        }

        /* --- increment read position to read next work buffer --- */
        ulReadPosition += usTargetItemSize;
    }
    return (( sCurrentCpnCo < sSalesCpnCo ) ? TRN_ERROR : TRN_SUCCESS );
}


/*
*==========================================================================
**    Synopsis: SHORT TrnStoIsSalesLinked( UCHAR  *puchCoupon,
*                                          USHORT usSalesOffset )
*
*       Input:  UCHAR  *puchCoupon   - address of coupon item
*               USHORT usSalesOffset - offset of sales item
*      Output:  Nothing
*       InOut:  Nothing
*
*    Return:    TRUE    : coupon item has sales offset
*               FALSE   : not
*
**  Description:
*           determine coupon item has specified sales item offset
*==========================================================================
*/

BOOL TrnStoIsSalesLinked( UCHAR *puchCoupon, USHORT usSalesOffset )
{
    TRANSTORAGECOUPONNON *pCpn = TRANSTORAGENONPTR( puchCoupon );
    USHORT      usIndex;
    BOOL        fFound  = FALSE;
                                        
    /* --- determine target coupon item is already voided --- */
    if ( pCpn->sVoidCo != 0 ) {
        return ( FALSE );
    }
    /* --- Check Reduced Item --- */
    if ( pCpn->fbReduceStatus & REDUCE_ITEM ) {
        return ( FALSE );
    }

    /* --- determine if target coupon has specified sales item --- */
    for (usIndex = 0; usIndex < STD_MAX_NUM_PLUDEPT_CPN && ! fFound; usIndex++) {
        fFound = (pCpn->usItemOffset[ usIndex ] == usSalesOffset);
    }
    return ( fFound );
}


/*
*==========================================================================
**    Synopsis: VOID TrnStoMakeVoidCpn( UCHAR      *puchSource,
*                                       ITEMCOUPON *pVoidCpn )
*
*       Input:  UCHAR      *puchSource - address of source coupon data
*               ITEMCOUPON *pVoidCpn   - address of destination coupon
*      Output:  Nothing
*       InOut:  Nothing
*
*    Return:    no return value
*
**  Description:
*           make a void coupon item from a transaction record containing a coupon.
*==========================================================================
*/

VOID TrnStoMakeVoidCpn( UCHAR *puchSource, ITEMCOUPON *pVoidCpn )
{
	ITEMCOUPON ItemCoupon = {0};

	NHPOS_ASSERT(TRANSTORAGEDATAPTR(puchSource)->uchMajorClass == CLASS_ITEMCOUPON);

    RflGetStorageItem( &ItemCoupon, puchSource, RFL_WITH_MNEM );

#if 1
	// copy this coupon we are going to void and then fix up those parts
	// that need to be 
	*pVoidCpn = ItemCoupon;

    pVoidCpn->fbReduceStatus  = 0;
    pVoidCpn->sVoidCo         = 1;
    pVoidCpn->lAmount         = ( ItemCoupon.lAmount * -1 );
    pVoidCpn->lDoubleAmount   = ( ItemCoupon.lDoubleAmount * -1 );
    pVoidCpn->lTripleAmount   = ( ItemCoupon.lTripleAmount * -1 );
    pVoidCpn->fbModifier      = ( ItemCoupon.fbModifier | VOID_MODIFIER );
    pVoidCpn->fbStorageStatus = 0;
#else
    memset( pVoidCpn, 0x00, sizeof( ITEMCOUPON ));

    pVoidCpn->uchMajorClass   = ItemCoupon.uchMajorClass;
    pVoidCpn->uchMinorClass   = ItemCoupon.uchMinorClass;
    pVoidCpn->uchSeatNo       = ItemCoupon.uchSeatNo;           /* R3.1 */
    pVoidCpn->uchCouponNo     = ItemCoupon.uchCouponNo;         /* UPC Coupon, 2172 */
    _tcsncpy(pVoidCpn->auchUPCCouponNo, ItemCoupon.auchUPCCouponNo, NUM_PLU_LEN);
    pVoidCpn->sVoidCo         = 1;
    pVoidCpn->lAmount         = ( ItemCoupon.lAmount * -1 );
    pVoidCpn->fbStatus[ 0 ]   = ItemCoupon.fbStatus[ 0 ];
    pVoidCpn->fbStatus[ 1 ]   = ItemCoupon.fbStatus[ 1 ];
    pVoidCpn->fsPrintStatus   = ItemCoupon.fsPrintStatus;
    pVoidCpn->uchNoOfItem     = ItemCoupon.uchNoOfItem;
    pVoidCpn->fbStorageStatus = 0;                              /* R3.1 */
    pVoidCpn->fbModifier      = ( ItemCoupon.fbModifier | VOID_MODIFIER );

    _tcsncpy( pVoidCpn->aszMnemonic, ItemCoupon.aszMnemonic, NUM_DEPTPLU );
    _tcsncpy( pVoidCpn->aszNumber,   ItemCoupon.aszNumber,   NUM_NUMBER );

    memcpy( pVoidCpn->usItemOffset, ItemCoupon.usItemOffset, sizeof(pVoidCpn->usItemOffset));
    
    _tcsncpy( pVoidCpn->auchPLUNo, ItemCoupon.auchPLUNo, NUM_PLU_LEN);
    pVoidCpn->usDeptNo        = ItemCoupon.usDeptNo;
    pVoidCpn->uchAdjective    = ItemCoupon.uchAdjective;
#endif
}

/*
*==========================================================================
**    Synopsis: SHORT TrnStoCouponSearch( ITEMCOUPONSEARCH *pItemCpnSearch,
*                                         SHORT             sStorageType )
*
*       Input:  ITEMCOUPONSEARCH *pItemCpnSearch - coupon item structure
*               SHORT             sStorageType   - type of trans storage
*                       TRN_TYPE_ITEMSTORAGE, TRN_TYPE_CONSSTORAGE
*      Output:  Nothing
*       InOut:  Nothing
*
*    Return:    TRN_SUCCESS     : item found and consolidate it
*               TRN_ERROR       : not found
*
**  Description:
*           coupon search for target "sales item" in item/consoli. storage
*==========================================================================
*/

SHORT TrnStoCouponSearch( ITEMCOUPONSEARCH *pItemCpnSearch,
                          SHORT             sStorageType )
{
    TRANSTORAGESALESNON *pTran;
    ULONG   ulMaxStoSize;
    ULONG   ulReadPosition;
    SHORT   sReturn;
    USHORT  usAllItemFound = FALSE;
    SHORT   hsStorage;
    USHORT  usStorageVli;
    USHORT  usStorageHdrSize;
    UCHAR   auchTranStorageWork[ TRN_TEMPBUFF_SIZE ];

    pTran = TRANSTORAGENONPTR( auchTranStorageWork );

    /* --- allow coupon search ? --- */
    if (( sReturn = TrnStoQueryCpnType( pItemCpnSearch )) != TRN_SUCCESS ) {
        return ( sReturn );
    }

    /* --- set target item status to "NOT FOUND" --- */
    for ( ulReadPosition = 0, ulMaxStoSize = pItemCpnSearch->uchNoOfItem; ulReadPosition < ulMaxStoSize; ulReadPosition++ ) {
        pItemCpnSearch->aSalesItem[ ulReadPosition ].fbStatus |= ITM_COUPON_NOT_FOUND;
    }

    sReturn = TrnStoGetStorageInfo( sStorageType, &hsStorage, &usStorageHdrSize, &usStorageVli );
    if (sReturn != TRN_SUCCESS ) {
        return ( sReturn );
    }
	ulReadPosition  = usStorageHdrSize;
    ulMaxStoSize    = ulReadPosition + usStorageVli;

    /* --- retrieve item data which is stored in transaction storage --- */
    while (( ulReadPosition < ulMaxStoSize ) && ( ! usAllItemFound )) {
		USHORT  usTargetItemSize;

        /* --- read item data, and store to work buffer --- */
		sReturn = TrnStoIsItemInWorkBuff(hsStorage, ulReadPosition, auchTranStorageWork, sizeof( auchTranStorageWork ), &usTargetItemSize);
		if (sReturn == TRN_ERROR) {
			NHPOS_ASSERT_TEXT((sReturn != TRN_ERROR), "**ERROR: TrnStoIsItemInWorkBuff() item record size exceeds buffer");
			return TRN_ERROR;
		}

		/* --- determine target item is sales item or not --- */
        if ( pTran->uchMajorClass == CLASS_ITEMSALES && pTran->uchMinorClass != CLASS_ITEMORDERDEC ) {
			DCURRENCY   lDiscountAmount = 0;
			ULONG	    ulActualBytesRead;
			USHORT      usTargetItemOffset = ulReadPosition;

            /* saratoga */
            if ((pTran->uchMinorClass == CLASS_PLUITEMDISC) || (pTran->uchMinorClass == CLASS_SETITEMDISC)) {
				UCHAR      auchItemDiscWork[sizeof(ITEMDISC)+10];
				ITEMDISC   itemDisc = {0};

				//11-1103- SR 201 JHHJ
                TrnReadFile( (usTargetItemOffset + usTargetItemSize), auchItemDiscWork, sizeof( auchItemDiscWork ), hsStorage, &ulActualBytesRead );
                RflGetStorageItem( &itemDisc, auchItemDiscWork, RFL_WITHOUT_MNEM );
                lDiscountAmount = itemDisc.lAmount;
            }
            usAllItemFound = TrnStoCpnItemSearch( pItemCpnSearch, auchTranStorageWork, usTargetItemOffset, lDiscountAmount);
        }

        /* --- increment read position to read next work buffer --- */
        ulReadPosition += usTargetItemSize;
    }

    /* --- return success, if all of coupon target item is found --- */
    return (( usAllItemFound ) ? ( TRN_SUCCESS ) : ( TRN_ERROR ));
}
/*
*==========================================================================
**    Synopsis: SHORT TrnStoCouponSearch( ITEMCOUPONSEARCH *pItemCpnSearch,
*                                         SHORT             sStorageType )
*
*       Input:  ITEMCOUPONSEARCH *pItemCpnSearch - coupon item structure
*               SHORT             sStorageType   - type of trans storage
*                       TRN_TYPE_ITEMSTORAGE, TRN_TYPE_CONSSTORAGE
*      Output:  Nothing
*       InOut:  Nothing
*
*    Return:    TRN_SUCCESS     : item found and consolidate it
*               TRN_ERROR       : not found
*
**  Description:
*           coupon search for all coupons in a transaction.  The number 
*			gets stored in uchNoOfItem variable of the ITEMCOUPONSEARCH struct
*==========================================================================
*/
SHORT TrnStoCouponSearchAll( ITEMCOUPONSEARCH *pItemCpnSearch, SHORT sStorageType )
{
    ULONG   ulMaxStoSize;
    ULONG   ulReadPosition;
    SHORT   sReturn;
    SHORT   hsStorage;
    USHORT  usStorageVli;
    USHORT  usStorageHdrSize;
    UCHAR   auchTranStorageWork[ TRN_TEMPBUFF_SIZE ];
    TRANSTORAGECOUPONNON *pTran;

    pTran = TRANSTORAGENONPTR( auchTranStorageWork );

    /* --- allow coupon search ? --- */
    if (( sReturn = TrnStoQueryCpnType( pItemCpnSearch )) != TRN_SUCCESS ) {
        return ( sReturn );
    }

    /* --- retrieve item data which is stored in transaction storage --- */
    sReturn = TrnStoGetStorageInfo( sStorageType, &hsStorage, &usStorageHdrSize, &usStorageVli );
    if (sReturn != TRN_SUCCESS ) {
        return ( sReturn );
    }

	ulReadPosition  = usStorageHdrSize;
    ulMaxStoSize    = ulReadPosition + usStorageVli;

    while (ulReadPosition < ulMaxStoSize ) {
		USHORT   usTargetItemSize = 0;

        /* --- read item data, and store to work buffer --- */
		sReturn = TrnStoIsItemInWorkBuff(hsStorage, ulReadPosition, auchTranStorageWork, sizeof( auchTranStorageWork ), &usTargetItemSize);
		if (sReturn == TRN_ERROR) {
			NHPOS_ASSERT_TEXT((sReturn != TRN_ERROR), "**ERROR: TrnStoIsItemInWorkBuff() item record size exceeds buffer");
			return TRN_ERROR;
		}

		/* --- determine target item is sales item or not --- */
        if ( pTran->uchMajorClass == CLASS_ITEMCOUPON ) 
		{
			ITEMCOUPON  itemCoupon = {0};
                                    
			//get the item, and add the amount that the coupon has already
			//used against the transaction. JHHJ
			RflGetStorageItem( &itemCoupon, auchTranStorageWork, RFL_WITHOUT_MNEM );

			if(itemCoupon.lAmount < 0)
			{
				pItemCpnSearch->uchNoOfItem++;
			} else if((itemCoupon.lAmount) && (pItemCpnSearch->uchNoOfItem))
			{
				//if there is a positive amount, then we have a voided coupon, we 
				//need to subtract one from our search counter
				pItemCpnSearch->uchNoOfItem--;
			}
			pItemCpnSearch->lAmount += itemCoupon.lAmount;
        }

        /* --- increment read position to read next work buffer --- */
        ulReadPosition += usTargetItemSize;
    }

    /* --- return success, if all of coupon target item is found --- */
    return TRN_SUCCESS;
}
/*
*==========================================================================
**    Synopsis: SHORT TrnStoCpnItemSearch( ITEMCOUPONSEARCH *pItemCpnSearch,
*                                          UCHAR            *puchWorkBuffer,
*                                          USHORT           usTargetOffset )
*
*       Input:  ITEMCOUPONSEARCH *pItemCpnSearch - coupon item structure
*               UCHAR            *puchWorkBuffer - point to work buffer
*               USHORT           usTargetOffset  - offset of sales item
*      Output:  Nothing
*       InOut:  Nothing
*
*    Return:    TRN_SUCCESS     : taget item is found
*               TRN_ERROR       : not found
*
**  Description:
*           search for a target item is in the target item list, and the
*   item is able to coupon discount or not.
*==========================================================================
*/

SHORT TrnStoCpnItemSearch( ITEMCOUPONSEARCH *pItemCpnSearch,
                           UCHAR            *puchWorkBuffer,
                           USHORT           usTargetOffset,
                           DCURRENCY        lDiscountAmount)
{
	ITEMSALES   ItemSales = {0};
    SHORT       sNoOfAvailItem;
    USHORT      usAllItemFound = FALSE;

    RflGetStorageItem( &ItemSales, puchWorkBuffer, RFL_WITHOUT_MNEM );

	if ((ItemSales.ControlCode.auchPluStatus[2] & PLU_SCALABLE) != 0) {
		// for scalable items, the lQTY value is in weight units and is not
		// a count of the number of items. The logic used for coupons
		// depends on the lQTY value being a count of items sold and not
		// a weight. For now we will just ignore scalable units. This
		// prevents the possibility of a divide by zero exception.
		//    Richard Chambers, Jan-18-2020
		return (usAllItemFound);
	}

    /* adjust plu unit price at item discount */
    if ((ItemSales.uchMinorClass == CLASS_PLUITEMDISC) || (ItemSales.uchMinorClass == CLASS_SETITEMDISC)) {
		// Prevent an exception from a divide by zero in cases where automated coupons are set but the
		// the item quantity is zero due to item voids.
		if (ItemSales.lQTY != 0) {
			ItemSales.lUnitPrice += (lDiscountAmount / (ItemSales.lQTY/1000L));
			ItemSales.lProduct   += lDiscountAmount;
		}
    }
    
	// check to see how many items that are in a PPI item of a transaction have not
	// yet had a coupon assigned to it.  The count of the number of coupons against
	// a PPI item is stored in ItemSales.sCouponCo.
	// If this is not a PPI item (ItemSales.uchPPICode == 0) then we will just check
	// to see if the ooupon count is greater than or equal to the sales quantity.
	// For a PPI item, these are usually the same and the sCouponCo variable indicates
	// the number of items that are still available for a coupon.
    sNoOfAvailItem = ItemSales.sCouponQTY - ItemSales.sCouponCo;
	if ( ItemSales.uchPPICode == 0 && ItemSales.sCouponQTY > (ItemSales.lQTY/1000L) ) {
		sNoOfAvailItem = 0;
	} else {
	    TRANCURQUAL *pWorkCur = TrnGetCurQualPtr();

		if (pWorkCur->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)) {
			sNoOfAvailItem *= -1;
		}
	}

	if ( ItemSales.fbModifier & VOID_MODIFIER ) {
		sNoOfAvailItem = 0;
	}

    usAllItemFound = FALSE;

	if (sNoOfAvailItem > 0) {
		/* --- determine target item is in target item list  --- */
		switch ( ItemSales.uchMinorClass ) {
			case CLASS_PLU:
			case CLASS_SETMENU:
			case CLASS_PLUITEMDISC: /* allow for UPC coupon */
			case CLASS_SETITEMDISC: /* allow for UPC coupon */
				//we search through the transaction and try to find
				//if this item is in the coupon.
				if (pItemCpnSearch->uchMinorClass == CLASS_UPCCOUPON) 
				{
					/* UPC Coupon, 2172 */
					usAllItemFound = TrnStoCpnUPCSearch (pItemCpnSearch, &ItemSales, usTargetOffset);
				} else {
					if (( ItemSales.uchMinorClass == CLASS_PLU) || ( ItemSales.uchMinorClass == CLASS_SETMENU)) 
					{
						usAllItemFound = TrnStoCpnPLUSearch (pItemCpnSearch, &ItemSales, usTargetOffset);
					}
				}
				break;    

			case CLASS_DEPT:
				usAllItemFound = TrnStoCpnDeptSearch (pItemCpnSearch, &ItemSales, usTargetOffset);
				break;

			default:
				break;
		}
	}
    return ( usAllItemFound );
}

/*
*==========================================================================
**    Synopsis: SHORT TrnStoCpnPLUSearch( ITEMCOUPONSEARCH  *pItemCpnSearch,
*                                         ITEMSALES         *pItemSales,
*                                         USHORT            *pusFoundIndex )
*
*       Input:  ITEMCOUPONSEARCH    *pItemCpnSearch - coupon item structure
*               ITEMSALES           *pItemSales     - sales item structure
*               USHORT              *pusFoundIndex  - index of target list
*      Output:  Nothing
*       InOut:  Nothing
*
*    Return:    TRN_SUCCESS     : taget item is found
*               TRN_ERROR       : not found
*
**  Description:
*           search for a target item is in the target item list, and the
*   item is able to coupon discount or not.
*==========================================================================
*/
SHORT TrnStoCpnPLUSearch(ITEMCOUPONSEARCH  *pItemCpnSearch, ITEMSALES *pItemSales, USHORT usTargetOffset)
{
    SHORT   usAllItemFound = FALSE;
    USHORT  usCpnIndex;
    UCHAR   uchTargetAdjNo;
    UCHAR   uchCpnAdjNo;

    /* --- retrieve adjective no. of target PLU item --- */
    if ( pItemSales->uchAdjective && pItemSales->uchMinorClass != CLASS_ITEMORDERDEC ) {
        uchTargetAdjNo = ( UCHAR )(( pItemSales->uchAdjective - 1 ) % 5 + 1 );
    } else {
        uchTargetAdjNo = 0;
    }

    /* --- search target PLU item is in the coupon target item list --- */
    for ( usCpnIndex = 0; usCpnIndex < pItemCpnSearch->uchNoOfItem; usCpnIndex ++ ) {
        if (pItemCpnSearch->aSalesItem[usCpnIndex].fbStatus & ITM_COUPON_NOT_FOUND) {

			/* --- compare target adjective, if necessary --- */
			uchCpnAdjNo = pItemCpnSearch->aSalesItem[usCpnIndex].uchAdjective;
			if (( uchCpnAdjNo ) && ( uchTargetAdjNo != uchCpnAdjNo )) {
				continue;
			}

			/* --- determine type of list item is PLU item or a department number --- */
			if ( pItemCpnSearch->aSalesItem[usCpnIndex].fbStatus & ITM_COUPON_DEPT_REC ) {
				if (pItemSales->usDeptNo != pItemCpnSearch->aSalesItem[usCpnIndex].usDeptNo) {
					continue;
				}

				/* --- determine coupon target item is coupon discountable --- */
				if ( TrnStoIsCouponItem( pItemSales, ITM_COUPON_DEPT_REC )) {
					pItemCpnSearch->aSalesItem[usCpnIndex].fbStatus &= (~ITM_COUPON_NOT_FOUND);
					pItemCpnSearch->aSalesItem[usCpnIndex].usItemOffset = usTargetOffset;

					/* --- determine whether all target item is found or not --- */
					if ( TrnStoIsAllItemFound( pItemCpnSearch )) {
						usAllItemFound = TRUE;
					}
					break;
				}
			} else {    /* listed item is a PLU item */
				/* --- compare target PLU no. with list item PLU no. --- */
				if (_tcsncmp(pItemSales->auchPLUNo, pItemCpnSearch->aSalesItem[usCpnIndex].auchPLUNo, NUM_PLU_LEN) != 0) {
					continue;
				}

				/* --- determine target item is coupon discountable --- */
				if ( TrnStoIsCouponItem( pItemSales, ITM_COUPON_PLU_REC )) {
					pItemCpnSearch->aSalesItem[usCpnIndex].fbStatus &= (~ITM_COUPON_NOT_FOUND);
					pItemCpnSearch->aSalesItem[usCpnIndex].usItemOffset = usTargetOffset;

					/* --- determine whether all target item is found or not --- */
					if ( TrnStoIsAllItemFound( pItemCpnSearch )) {
						usAllItemFound = TRUE;
					}
					break;
				}
			}
        }
    }

    return usAllItemFound;
}



/*
*==========================================================================
**    Synopsis: SHORT TrnStoCpnDeptSearch( ITEMCOUPONSEARCH *pItemCpnSearch,
*                                          ITEMSALES        *pItemSales,
*                                          USHORT           *pusFoundIndex )
*
*       Input:  ITEMCOUPONSEARCH    *pItemCpnSearch - coupon item structure
*               ITEMSALES           *pItemSales     - sales item structure
*               USHORT              *pusFoundIndex  - index of target list
*      Output:  Nothing
*       InOut:  Nothing
*
*    Return:    TRN_SUCCESS     : taget item is found
*               TRN_ERROR       : not found
*
**  Description:
*       search for a target item which is in the target item list, and the
*   item is able to coupon discount or not.
*==========================================================================
*/

SHORT   TrnStoCpnDeptSearch( ITEMCOUPONSEARCH   *pItemCpnSearch,
                             ITEMSALES          *pItemSales,
                             USHORT             usTargetOffset )
{
    SHORT   usAllItemFound = FALSE;
    USHORT  usCpnIndex;
    UCHAR   uchListItemType;
    UCHAR   uchTargetAdjNo;
    UCHAR   uchCpnAdjNo;

    /* --- retrieve adjective no. of target department item --- */
    if ( pItemSales->uchAdjective && pItemSales->uchMinorClass != CLASS_ITEMORDERDEC ) {
        uchTargetAdjNo = ( UCHAR )(( pItemSales->uchAdjective - 1 ) % 5 + 1 );
    } else {
        uchTargetAdjNo = 0;
    }

    /* --- search target dept. item is in the coupon target item list --- */
    for ( usCpnIndex = 0; usCpnIndex < pItemCpnSearch->uchNoOfItem; usCpnIndex ++ ) {
        if (pItemCpnSearch->aSalesItem[usCpnIndex].fbStatus & ITM_COUPON_NOT_FOUND) {
			/* --- determine type of list item is department item --- */
			uchListItemType = ( UCHAR ) ( pItemCpnSearch->aSalesItem[usCpnIndex].fbStatus & ITM_COUPON_REC_MASK );

			if (pItemCpnSearch->aSalesItem[usCpnIndex].fbStatus & ITM_COUPON_DEPT_REC ) {
				/* --- compare target dpet. no. with list item dept. no. --- */
				if ( pItemSales->usDeptNo != pItemCpnSearch->aSalesItem[usCpnIndex].usDeptNo ) {
					continue;
				}

				/* --- compare target adjective, if necessary --- */
				uchCpnAdjNo = pItemCpnSearch->aSalesItem[usCpnIndex].uchAdjective;

				if (( uchCpnAdjNo ) && ( uchTargetAdjNo != uchCpnAdjNo )) {
					continue;
				}

				/* --- determine target item is coupon discountable --- */
				if ( TrnStoIsCouponItem( pItemSales, ITM_COUPON_DEPT_REC )) {
					/* --- declare the target item is found in target list --- */
					pItemCpnSearch->aSalesItem[usCpnIndex].usItemOffset = usTargetOffset;
					pItemCpnSearch->aSalesItem[usCpnIndex].fbStatus &= ~ITM_COUPON_NOT_FOUND;

					/* --- determine whether all target item is found or not --- */
					if ( TrnStoIsAllItemFound( pItemCpnSearch )) {
						usAllItemFound = TRUE;
						break;
					}
				}
			}
        }
    }
    return usAllItemFound;
}

/*
*==========================================================================
**    Synopsis: SHORT TrnStoCpnUPCSearch( ITEMCOUPONSEARCH  *pItemCpnSearch,
*                                         ITEMSALES         *pItemSales,
*                                         USHORT            *pusFoundIndex )
*
*       Input:  ITEMCOUPONSEARCH    *pItemCpnSearch - coupon item structure
*               ITEMSALES           *pItemSales     - sales item structure
*               USHORT              *pusFoundIndex  - index of target list
*      Output:  Nothing
*       InOut:  Nothing
*
*    Return:    TRN_SUCCESS     : taget item is found
*               TRN_ERROR       : not found
*
**  Description:
*           search for a target item is in the target item list, and the
*   item is able to coupon discount or not. UPC Coupon, 2172
*==========================================================================
*/

SHORT TrnStoCpnUPCSearch( ITEMCOUPONSEARCH  *pItemCpnSearch,
                          ITEMSALES         *pItemSales,
                          USHORT            usTargetOffset)
{
	SHORT   usAllItemFound = FALSE;
    USHORT  usCpnIndex;
    UCHAR   uchTargetAdjNo;
    UCHAR   iuchValue;

    /* --- retrieve adjective no. of target PLU item --- */
    if ( pItemSales->uchAdjective && pItemSales->uchMinorClass != CLASS_ITEMORDERDEC ) {
        uchTargetAdjNo = ( UCHAR )(( pItemSales->uchAdjective - 1 ) % 5 + 1 );
    } else {
        uchTargetAdjNo = 0;
    }

    /* --- search target PLU item is in the coupon target item list --- */

    /* ----- get value code in UPC coupon ----- */
    iuchValue = (UCHAR)TrnASCII2BIN(pItemCpnSearch->auchUPCCouponNo, 12, 13);
    pItemCpnSearch->uchNoOfItem = aTrnUPC[iuchValue].uchQty;
    
    /* ----- check value code table ----- */
    for ( usCpnIndex = 0; (usCpnIndex < aTrnUPC[iuchValue].uchQty); usCpnIndex ++ ) {
        if ( pItemCpnSearch->aSalesItem[usCpnIndex].fbStatus & ITM_COUPON_NOT_FOUND) {
			/* --- compare target PLU no. with list item PLU no. --- */
			if (TrnStoIsUPCCouponItem(pItemSales, pItemCpnSearch->auchUPCCouponNo) == FALSE) {
				break;
			}

			/* --- determine target item is coupon discountable --- */
			if ( TrnStoIsCouponItem( pItemSales, ITM_COUPON_PLU_REC )) {
				DCURRENCY    lUnitPrice;

				_tcsncpy(pItemCpnSearch->aSalesItem[usCpnIndex].auchPLUNo, pItemSales->auchPLUNo, NUM_PLU_LEN);
				pItemCpnSearch->aSalesItem[usCpnIndex].uchAdjective = uchTargetAdjNo;
				pItemCpnSearch->aSalesItem[usCpnIndex].usDeptNo = pItemSales->usDeptNo;
            
				pItemCpnSearch->sValue = aTrnUPC[iuchValue].sAmount;
            
				/* copy taxable status */
				pItemCpnSearch->auchStatus[0] = pItemSales->ControlCode.auchPluStatus[1];
				pItemCpnSearch->fbModifier &= ~TAX_MOD_MASK;
				pItemCpnSearch->fbModifier |= (pItemSales->fbModifier & TAX_MOD_MASK);

				/* calculate unit price of PM/PPI */
				if ((pItemSales->uchPM) || (pItemSales->uchPPICode)) {
					D13DIGITS d13Work, d13Remain;
					LONG      lActualQuant = labs(pItemSales->lQTY) / 1000L;
                
					d13Remain = d13Work = labs(pItemSales->lProduct);
					d13Work /= lActualQuant;     

					lUnitPrice = d13Work;

					if (d13Remain % lActualQuant) {
						/* --- round up --- */
						lUnitPrice++;
					}
				} else {
					lUnitPrice = RflLLabs(pItemSales->lUnitPrice);
				}
            
				if (aTrnUPC[iuchValue].sAmount <= TRN_UPC_SPECIAL) { /* case of checker int. or free */
					if (pItemCpnSearch->lAmount < lUnitPrice) {
						pItemCpnSearch->lAmount = lUnitPrice;
					}
				} else {
					pItemCpnSearch->lAmount = (LONG)aTrnUPC[iuchValue].sAmount;                /* discount amount */
					if (pItemCpnSearch->lAmount > lUnitPrice) {
						/* not allow over unit price discount */
						continue;
					}
				}

				pItemCpnSearch->aSalesItem[usCpnIndex].fbStatus &= ~ITM_COUPON_NOT_FOUND;
				pItemCpnSearch->aSalesItem[usCpnIndex].usItemOffset = usTargetOffset;
				/* --- determine whether all target item is found or not --- */
				if ( TrnStoIsAllItemFound( pItemCpnSearch )) {
					usAllItemFound = TRUE;
					break;
				}
			}
        }
    }
    return usAllItemFound;
}



/*
*==========================================================================
**    Synopsis: SHORT TrnStoIsCouponItem( ITEMSALES *pItemSales,
*                                         UCHAR     uchRecType )
*
*       Input:  ITEMSALES   *pItemSales - sales item to determine
*               UCHAR       uchRecType  - type of target sales item    
*      Output:  Nothing
*       InOut:  Nothing
*
*    Return:    TRUE            : item is coupon discountable
*               FALSE           : item is not coupon discountable
*
**  Description:
*           determine target sales item is coupon discountable or not.
*==========================================================================
*/

SHORT   TrnStoIsCouponItem( ITEMSALES *pItemSales, UCHAR uchRecType )
{
    /* --- determine target item is void item or not --- */
    if ( pItemSales->fbModifier & VOID_MODIFIER ) {
        return ( FALSE );
    }

    if (pItemSales->uchMinorClass == CLASS_DEPTITEMDISC ||
        pItemSales->uchMinorClass == CLASS_PLUITEMDISC ||
        pItemSales->uchMinorClass == CLASS_SETITEMDISC ) {
        if (pItemSales->fbReduceStatus & REDUCE_ITEM) {
            return ( FALSE );
        }
    }

    /* --- retrieve coupon status of target sales item --- */
    if (( 0L <= pItemSales->lQTY ) && ( pItemSales->sCouponQTY < 0 )) {
        return ( FALSE );
    }

    /* --- determine target item is minus item or not --- */
    if ( pItemSales->ControlCode.auchPluStatus[ 0 ] & PLU_MINUS ) {
        return ( FALSE );
    }

    /* --- determine target item is scalable item or not --- */
    if ( pItemSales->ControlCode.auchPluStatus[ 2 ] & PLU_SCALABLE ) {
        return ( FALSE );
    }

    /* --- if target item is PLU item, detemrmine condiment item or not --- */
    if (( uchRecType == ITM_COUPON_PLU_REC ) && ( pItemSales->ControlCode.auchPluStatus[ 2 ] & PLU_CONDIMENT )) {
        return ( FALSE );
    }

    if ( TrnInformation.TranCurQual.fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN) ) {
        if ( 0 <= pItemSales->sCouponQTY ) {
            return ( FALSE );
        }
    } else {
        if ( pItemSales->sCouponQTY <= 0 ) {
            return ( FALSE );
        }
		if (( pItemSales->sCouponQTY - pItemSales->sCouponCo ) <= 0 ) {
			return ( FALSE );
		}
    }
    return ( TRUE );
}



/*
*==========================================================================
**    Synopsis: SHORT TrnStoIsAllItemFound( ITEMCOUPONSEARCH *pItemCpnSearch )
*
*       Input:  ITEMCOUPONSEARCH *pItemCpnSearch - coupon item structure
*      Output:  Nothing
*       InOut:  Nothing
*
*    Return:    TRUE    :   all of target item is found
*               FALSE   :   not found target item
*
**  Description: determine whether all items/departments specified in a.
*                coupon, combination coupon normally, are represented in
*                the transaction by looking to see which items/departments
*                specified in the combination coupon are in the transaction
*                by checking to see if the ITM_COUPON_NOT_FOUND flag is off
*                for all of the components of the combination coupon.
*
*                This function also implements the logic of group numbers
*                for coupons so as to provide a more flexible logic for
*                deciding if a combination coupon should be applied or not.
*
*                WARNING:  This code is dependent on group numbers being
*                          either 0, 1, or 2 because it is using bitwise
*                          operations to check group numbers.
*==========================================================================
*/
SHORT TrnStoIsAllItemFound( ITEMCOUPONSEARCH *pCoupon )
{
    USHORT  usTarget;
    USHORT  usNoOfItem = pCoupon->uchNoOfItem;
    USHORT  fusObject = 0x0;  // indicates which group numbers are expected in transaction
    USHORT  fusResult = 0x0;  // indicates which group number are found in transaction
    USHORT  fusNotHit = 0x0;  // indicates which coupon couponents, max count of 15, have been found in transaction

	NHPOS_ASSERT(pCoupon->uchNoOfItem <= STD_MAX_NUM_PLUDEPT_CPN);

#if STD_MAX_NUM_PLUDEPT_CPN > 15
	// cause a compilation error if max count is greater than 15.
	// fusNotHit is a bit map for detecting coupons components and has 16 bits.
	junkyjunk += 1;
#endif

	if (usNoOfItem > STD_MAX_NUM_PLUDEPT_CPN) usNoOfItem = STD_MAX_NUM_PLUDEPT_CPN;

    for (usTarget = 0; usTarget < usNoOfItem; usTarget++ ) {
		USHORT  usGroupNo = pCoupon->aSalesItem[usTarget].uchGroup;

        /* --- check condition group --- */
		// Group #0 means item is required to be in transaction
		// Group #1 or #2 means at least one item with that group number required to be in transaction.
        switch (usGroupNo) {
			case 0:                                 /* AND condition group, requires this item   */
				if ( pCoupon->aSalesItem[usTarget].fbStatus & ITM_COUPON_NOT_FOUND ) {   /* required item not found in transaction */
					return ( FALSE );               /* exit ...              */
				}
				break;                              /* next ...              */

			case 1:                                 /* OR condition group, at least one item with a 1 in this field */
			case 2:                                 /* OR condition group, at least one item with a 2 in this field */
				fusObject |= usGroupNo;             /* set object group, indicates at least one item of group requested */
				if ( ! (pCoupon->aSalesItem[usTarget].fbStatus & ITM_COUPON_NOT_FOUND) ) {   /* item was found in transaction?    */
					if ( fusResult & usGroupNo ) {       /* already found item in this group?  */
						fusNotHit |= ( 0x0001 << usTarget );   /* set mask bit for this item indicating item from group already found  */
					} else {                             /* must be first item in this group   */
						fusResult |= usGroupNo;                /* set target group, at least one item of group found   */
					}
				}
				break;

			default:
				break;
        }
    }

    if ( fusObject != fusResult ) {       /* at least one item of each non-zero group is not found     */
        return ( FALSE );                 /* exit ...              */
    }

	// Set the item not found flag for those items that are in a Group #1 or #2 which are not needed for this match.
	// Since only one item from a group is needed to meet the criteria of a match for Group #1 and #2, we clear this
	// bit in order to allow the items to be used in a different match attempt.
    for ( usTarget = 0; usTarget < usNoOfItem; usTarget++ ) {
        if ( fusNotHit & ( 0x0001 << usTarget )) { /* not object item   */
            pCoupon->aSalesItem[usTarget].fbStatus |= ITM_COUPON_NOT_FOUND;
        }
    }
    return ( TRUE );
}

/*
*==========================================================================
**    Synopsis: SHORT TrnStoUpdateCpnCo( ITEMCOUPON *pItemCoupon,
*                                        SHORT       sStorageType )
*
*       Input:  ITEMCOUPON *pItemCoupon  - point to coupon item strucure
*               SHORT       sStorageType - type of transaction storage
*      Output:  Nothing
*       InOut:  Nothing
*
*    Return:    TRN_SUCCESS :   function is successful
*               TRN_ERROR   :   function is failed
*
**  Description:
*           update target sales item's coupon counter
*==========================================================================
*/

SHORT TrnStoUpdateCpnCo( ITEMCOUPON *pItemCoupon, SHORT sStorageType, TRANSTORAGESALESNON *pNon )
{
    TRANSTORAGESALESNON *pTranNon;
    ULONG   ulWorkBuffReadSize;
    SHORT   sReturn;
    SHORT   fPreVoid;
    SHORT   fVoidMod;
    SHORT   sIncrementCo;
    SHORT   hsStorage;
    USHORT  usReadPosition;
    UCHAR   auchWorkBuff[ TRN_WORKBUFFER ];
    UCHAR   uchItemIndex = 0, uchItemCounter = 0;
	ITEMSALES	myItemSales = {0};

    pTranNon = TRANSTORAGENONPTR( auchWorkBuff );

    if (pItemCoupon->fbReduceStatus & REDUCE_NOT_HOURLY) {
        /* not increment couon counter at item transfer */
        return ( TRN_SUCCESS );
    }

    fPreVoid = ( TrnInformation.TranCurQual.fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN) ) ? TRUE : FALSE;
    fVoidMod = ( pItemCoupon->fbModifier & VOID_MODIFIER ) ? TRUE : FALSE;
    sIncrementCo = (( fPreVoid ^ fVoidMod ) ? ( - 1 ) : ( 1 ));

    /* --- determine type of transaction storage --- */
    sReturn = TrnStoGetStorageInfo( sStorageType, &hsStorage, NULL, NULL );
    if (sReturn != TRN_SUCCESS ) {
        return ( sReturn );
    }

    /* --- update coupon counter of target sales item --- */
	uchItemIndex = 0;
    while ( uchItemCounter < pItemCoupon->uchNoOfItem ) {
        /* --- read target sales item from transaction storage --- */
		if(pItemCoupon->usItemOffset[uchItemIndex] == 0)
		{
			uchItemIndex++;
			continue;
		}

        usReadPosition = pItemCoupon->usItemOffset[ uchItemIndex ];

		//11-1103- SR 201 JHHJ
         TrnReadFile( usReadPosition, auchWorkBuff, sizeof( auchWorkBuff ), hsStorage, &ulWorkBuffReadSize );

        /* --- increment/decrement target coupon counter --- */
        if ( fVoidMod ) {
            if (( fPreVoid ) && ( 0 < pTranNon->sCouponCo )) {
                pTranNon->sCouponCo--;
            } else if (( ! fPreVoid ) && ( pTranNon->sCouponCo < 0 )) {
                pTranNon->sCouponCo++;
            } else {
                pTranNon->sCouponCo += sIncrementCo;
            }
        } else {
            pTranNon->sCouponCo += sIncrementCo;
        }
        
        /* to avoid override, saratoga */
        if (pNon->usItemOffset) {
            if (pNon->usItemOffset == pTranNon->usItemOffset) {
                pNon->sCouponCo = pTranNon->sCouponCo;
            }
        }

        /* --- write sales item to transaction storage --- */
        TrnWriteFile( usReadPosition, auchWorkBuff, ulWorkBuffReadSize, hsStorage );

		uchItemIndex++;
        uchItemCounter++;
    }
    return ( TRN_SUCCESS );
}

/*
*==========================================================================
**    Synopsis: SHORT TrnStoUpdateVoidCo( ITEMCOUPON *pItemCoupon,
*                                         ITEMCOUPON *pVoidedCoupon,
*                                         SHORT       sStorageType )
*
*       Input:  ITEMCOUPON *pItemCoupon     - coupon item to void
*               ITEMCOUPON *pVoidedCoupon   - voided coupon item
*               SHORT       sStorageType    - type of transaction storage
*      Output:  Nothing
*       InOut:  Nothing
*
*    Return:    TRN_SUCCESS :   function is successful
*               TRN_ERROR   :   function is failed
*
**  Description:
*       search for coupon item to void, and then update its void counter.
*==========================================================================
*/

SHORT TrnStoUpdateVoidCo( ITEMCOUPON *pItemCoupon,
                          ITEMCOUPON *pVoidedCoupon,
                          SHORT       sStorageType )
{
    ULONG   ulOffset;
    SHORT   sStatus;
    USHORT  fbModSave = pItemCoupon->fbModifier;

    /* --- search for target coupon item in transaction storage --- */
    pItemCoupon->fbModifier &= ~( VOID_MODIFIER );
    sStatus = TrnStoCpnVoidSearch( pItemCoupon, &ulOffset, sStorageType );
    if ( sStatus == TRN_SUCCESS ) {
		TRANSTORAGECOUPONNON *pTranNon;
		ULONG   ulReadSize;
		SHORT   hsStorage;
		UCHAR   auchBuffer[ TRN_WORKBUFFER ];

		pTranNon = TRANSTORAGENONPTR( auchBuffer );

        /* --- update void counter of target coupon item --- */
        sStatus = TrnStoGetStorageInfo( sStorageType, &hsStorage, NULL, NULL );
        if (sStatus != TRN_SUCCESS ) {
            return ( sStatus );
        }

		//11-1103- SR 201 JHHJ
        TrnReadFile( ulOffset, auchBuffer, sizeof( auchBuffer ), hsStorage, &ulReadSize );

        if ( TrnInformation.TranCurQual.fsCurStatus & CURQUAL_PRESELECT_MASK ) {
            ( pTranNon->sVoidCo )--;
        } else {
            ( pTranNon->sVoidCo )++;
        }

        TrnWriteFile( ulOffset, auchBuffer, ulReadSize, hsStorage );

        /* --- store voided coupon item to user's buffer --- */
        memset( pVoidedCoupon, 0x00, sizeof( ITEMCOUPON ));
        RflGetStorageItem( pVoidedCoupon, auchBuffer, RFL_WITH_MNEM );
    }
    pItemCoupon->fbModifier = fbModSave;

    return ( sStatus );
}

/*
*==========================================================================
**    Synopsis: SHORT TrnStoIsCpnVoidItem( ITEMCOUPON   *pCouponVoid,
*                                          UCHAR        *puchTarget )
*
*       Input:  ITEMCOUPON  *pItemCoupon : coupon item to void 
*               UCHAR       *puchTarget  : address of target item
*      Output:  Nothing
*       InOut:  Nothing
*
*    Return:    TRUE        :   target item is able to void
*               FALSE       :   target item is not void
*
**  Description:
*       determine target coupon item is able to void or not.
*==========================================================================
*/

static SHORT TrnStoIsCpnVoidItem( ITEMCOUPON *pCouponVoid, UCHAR *puchTarget )
{
    TRANSTORAGECOUPONNON *pTranNon = TRANSTORAGENONPTR( puchTarget );
	ITEMCOUPON           ItemCoupon = {0};
    USHORT               usItemReadLen;

    /* --- determine target item is coupon item or not --- */
    if ( ! (pTranNon->uchMajorClass == CLASS_ITEMCOUPON )) {
        return ( FALSE );
    }
    if ( ! ( pTranNon->uchMinorClass == CLASS_COMCOUPON || pTranNon->uchMinorClass == CLASS_UPCCOUPON )) {
        return ( FALSE );
    }

    /* --- compare coupon number of source item with target item --- */
    if ( pCouponVoid->uchCouponNo != pTranNon->uchCouponNo ) {
        return ( FALSE );
    }

    if (_tcsncmp(pCouponVoid->auchUPCCouponNo, pTranNon->auchUPCCouponNo, NUM_PLU_LEN) != 0) {
        return ( FALSE );
    }

    /* --- determine target item is already voided --- */
    if ( pTranNon->sVoidCo != 0 ) {
        return ( FALSE );
    }

	// easy tests that can be done without decompressing the coupon data have been done.
	// unzip the coupon data to complete the checks.
    usItemReadLen = RflGetStorageItem( &ItemCoupon, puchTarget, RFL_WITHOUT_MNEM );

    if (ItemCoupon.uchMinorClass == CLASS_UPCCOUPON) {
        if (pCouponVoid->fbModifier & TAX_MOD_MASK) {
            if ( pCouponVoid->fbModifier != ItemCoupon.fbModifier ) {
                return ( FALSE );
            }
        } else {
            /* ignor tax modifier, because coupon tax modifier status inherits parent plu status */
            if ( (pCouponVoid->fbModifier & ~TAX_MOD_MASK) != (ItemCoupon.fbModifier & ~TAX_MOD_MASK) ) {
                return ( FALSE );
            }
        }
    } else {
        if ( pCouponVoid->fbModifier != ItemCoupon.fbModifier ) {
            return ( FALSE );
        }
    }

    /* --- compare number mnemonic of source item with target item --- */
    if ( _tcsncmp( pCouponVoid->aszNumber, ItemCoupon.aszNumber, NUM_NUMBER )) {
        return ( FALSE );
    }

    if ( TrnInformation.TranCurQual.fsCurStatus & CURQUAL_PRESELECT_MASK ) {
        if ( ItemCoupon.lAmount < 0L ) {
            return ( FALSE );
        }
    } else {
        if ( 0L < ItemCoupon.lAmount ) {
            return ( FALSE );
        }
    }
    return ( TRUE );
}

/*
*==========================================================================
**    Synopsis: SHORT TrnStoCpnVoidSearch( ITEMCOUPON   *pItemCoupon,
*                                          USHORT       *pusOffset,
*                                          SHORT        sStorageType )
*
*       Input:  ITEMCOUPON  *pItemCoupon : coupon item to void 
*               USHORT      *pusOffset   : target coupon item offset
*               SHORT       sStorageType : type of transaction storage
*      Output:  Nothing
*       InOut:  Nothing
*
*    Return:    TRN_SUCCESS     : item found
*               TRN_ERROR       : not found
*
**  Description:
*       This function searches for specified coupon item in storage to
*   void.
*==========================================================================
*/
SHORT TrnStoCpnVoidSearch( ITEMCOUPON *pItemCoupon,
                           ULONG     *pulOffset,
                           SHORT      sStorageType )
{
    ULONG   ulMaxStoSize;
    ULONG   ulReadPosition;
    SHORT   sReturn;
    SHORT   hsStorage;
    USHORT  usStorageHdrSize;
    USHORT  usStorageVli;
    UCHAR   auchTranStorageWork[ TRN_TEMPBUFF_SIZE ];

    /* --- determine type of transaction storage --- */
    sReturn = TrnStoGetStorageInfo( sStorageType, &hsStorage, &usStorageHdrSize, &usStorageVli );
    if (sReturn != TRN_SUCCESS ) {
        return ( sReturn );
    }

    ulReadPosition  = usStorageHdrSize;
    ulMaxStoSize    = ulReadPosition + usStorageVli;

    /* --- search for specified coupon item in transaction storage --- */
    while ( ulReadPosition < ulMaxStoSize ) {
		USHORT  usTargetItemSize;

        /* --- read storage data to work buffer --- */
		sReturn = TrnStoIsItemInWorkBuff(hsStorage, ulReadPosition, auchTranStorageWork, sizeof( auchTranStorageWork ), &usTargetItemSize);
		if (sReturn == TRN_ERROR) {
			NHPOS_ASSERT_TEXT((sReturn != TRN_ERROR), "**ERROR: TrnStoIsItemInWorkBuff() item record size exceeds buffer");
			return TRN_ERROR;
		}

		if (TRANSTORAGEDATAPTR(auchTranStorageWork)->uchMajorClass == CLASS_ITEMCOUPON) {
			/* --- compare target item with source item --- */
			if ( TrnStoIsCpnVoidItem( pItemCoupon, auchTranStorageWork )) {
				/* --- found target coupon item --- */
				/* saratoga */
				if (pItemCoupon->uchMinorClass == CLASS_UPCCOUPON) {
					TrnStoMakeVoidCpn( auchTranStorageWork, pItemCoupon );
				}

				*pulOffset = ulReadPosition;
				return ( TRN_SUCCESS );
			}
		}
        /* --- increment read position to read next work buffer --- */
        ulReadPosition += usTargetItemSize;
    }

    return ( TRN_ERROR );
}


/*==========================================================================
*    Synopsis:  VOID    TrnStoQueryCpnType( ITEMCOUPONSEARCH *pItem )
*
*    Input:     ITEMCOUPONSEARCH    *pItem
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS :     allow coupon search
*               TRN_ERROR   : not allow coupon search
*
*    Description:
==========================================================================*/

USHORT  TrnStoQueryCpnType( ITEMCOUPONSEARCH *pItem )
{
    USHORT  usNo;
    USHORT  usMask;
    USHORT  usOff;

    /* --- check coupon type --- */
    if ( ! ( pItem->fbStatus & ITM_COUPON_SINGLE )) {   /* multi. coupon */
        return ( TRN_SUCCESS );
    }

    usNo   = ( USHORT )( pItem->uchCouponNo - 1 );
    usMask = 0x01 << ( usNo % 8 );
    usOff  = usNo / 8;

    if ( TrnInformation.TranGCFQual.auchCouponType[ usOff ] & usMask ) {
        return ( TRN_ERROR );
    } else {
        return ( TRN_SUCCESS );
    }
}

/*
*==========================================================================
**    Synopsis: SHORT TrnStoIsUPCCouponItem( ITEMSALES *pItemSales,
*                                         UCHAR     *pauchCouponNo )
*
*       Input:  ITEMSALES   *pItemSales - sales item to determine
*               UCHAR       *pauchCouponNo  - UPC Coupon No.    
*      Output:  Nothing
*       InOut:  Nothing
*
*    Return:    TRUE            : item is coupon discountable
*               FALSE           : item is not coupon discountable
*
**  Description:
*           determine target sales item is coupon discountable or not.
*==========================================================================
*/
SHORT TrnStoIsUPCCouponItem(ITEMSALES *pPLU, TCHAR *pauchCouponNo)
{
    
    USHORT      usNS;
    USHORT      usFamily;
    UCHAR       uchValue;

    /* ----- PLU is UPC-A label or not ----- */
    if (TrnASCII2BIN(pPLU->auchPLUNo, 2, 2) != 0) {
        /* EAN 13 Item */
        return FALSE;                                 /* next item */
    }
    if (TrnASCII2BIN(pPLU->auchPLUNo, 2, 6) == 0) {
        /* EAN 8 Item */
        return FALSE;                                 /* next item */
    }
    if (TrnASCII2BIN(pPLU->auchPLUNo, 2, 7) == 0) {
        /* Velocity Item */
        return FALSE;                                 /* next item */
    }
    
    /* ----- get value and family code in UPC coupon ----- */
    if (CliParaMDCCheck(MDC_UPC1_ADR, ODD_MDC_BIT1)) {
        usFamily = 0;
    } else {
        usFamily = (USHORT)TrnASCII2BIN(pauchCouponNo, 9, 11);
    }
    uchValue = (UCHAR)TrnASCII2BIN(pauchCouponNo, 12, 13);

    /* ----- check PPI ----- */
    if ((pPLU->uchPPICode) &&
        (aTrnUPC[uchValue].sAmount == TRN_FREE ||
         aTrnUPC[uchValue].sAmount == TRN_BUY1_GET1 ||
         aTrnUPC[uchValue].sAmount == TRN_BUY2_GET1 ||
         aTrnUPC[uchValue].sAmount == TRN_BUY3_GET1 ||
         aTrnUPC[uchValue].sAmount == TRN_BUY4_GET1)) {
        return FALSE;                                 /* next item */
    }

    /* ----- check manufacturer's number ----- */
    usNS = (USHORT)TrnASCII2BIN(pPLU->auchPLUNo, 3, 4);
    if (usNS == 30 || usNS == 38) {             /* drug or national health item */
        if (TrnASCII2BIN(pauchCouponNo, 4, 8) !=
            TrnASCII2BIN(pPLU->auchPLUNo, 4, 7)) {
            return FALSE;                                 /* next item */
        }
    } else {                                    /* other item */
        if (TrnASCII2BIN(pauchCouponNo, 4, 8) !=
            TrnASCII2BIN(pPLU->auchPLUNo, 4, 8)) {
            return FALSE;                                 /* next item */
        }
    }

    /* ----- check family code ----- */
    if (usFamily == 0) {                        /* allow any item */
        return TRUE;
    }

    if (usFamily == pPLU->usFamilyCode) {
        return TRUE;
    }

    if (usFamily % 10 == 0 &&                   /* summary code */
        usFamily / 10 == pPLU->usFamilyCode / 10) {
        return TRUE;
    }

    if (usFamily % 100 == 0 &&                  /* super summary code */
        usFamily / 100 == pPLU->usFamilyCode / 100) {
        return TRUE;
    }

    return FALSE;                                 /* next item */
}

/*
*===========================================================================
** Synopsis:    LONG TrnASCII2BIN(UCHAR *puchData, UCHAR iuchStart,
*                               UCHAR iuchEnd)
*     Input:    puchData  - pointer of BCD
*               iuchStart - start of BCD offset
*               iuchEnd   - end of BCD offset
*
** Return:      0  >= convert data
*               -1 == data ranage error
*
** Description: This function converts packed-BCD data into the binary data.
*===========================================================================
*/
LONG TrnASCII2BIN(TCHAR *puchData, TCHAR iuchStart, TCHAR iuchEnd)
{
    TCHAR   *pszAscii;
    TCHAR   szAscii[NUM_PLU_LEN+1];
    USHORT  i;

    /* ----- check data range ----- */
    if (iuchStart > iuchEnd) {
        return (-1L);
    }

    pszAscii = szAscii;
    for (i=iuchStart; i<=iuchEnd; i++) {
        *pszAscii = puchData[i-1];
        pszAscii++;
    }

    *pszAscii = '\0';                               /* null terminate */

    return (_ttol(szAscii));
}

/*
*===========================================================================
** Synopsis:    SHORT TrnQueryCouponUPCValue(ITEMCOUPONSEARCH *pItemCpnSearch)
*     Input:    noting
*     Output:   nothing
*     Inout:    *pItemCpnSearch
*
** Return:      TRN_SUCCESS
*               TRN_ERROR
*
** Description: This function returns UPC coupon face value, saratoga
*===========================================================================
*/
SHORT TrnQueryCouponUPCValue(ITEMCOUPONSEARCH *pItemCpnSearch)
{
    UCHAR       iuchValue;
    SHORT       sResult = TRN_ERROR;

    /* ----- get value code in UPC coupon ----- */
    iuchValue = (UCHAR)TrnASCII2BIN(pItemCpnSearch->auchUPCCouponNo, 12, 13);

    /* ----- check value code table ----- */
    if (aTrnUPC[iuchValue].uchQty) {                                                /* use table */
        /* ----- check discount type or amount ----- */
        if (aTrnUPC[iuchValue].sAmount >= 0) {                                  /* normal coupon */
            pItemCpnSearch->lAmount = (LONG)aTrnUPC[iuchValue].sAmount;                /* discount amount */
            pItemCpnSearch->sValue = aTrnUPC[iuchValue].sAmount;
            sResult          = TRN_SUCCESS;
        } else {                                                                /* special discount */
            pItemCpnSearch->lAmount = 0;
            pItemCpnSearch->sValue = TRN_CHECKER;
            sResult          = TRN_SUCCESS;
        }
    }

    return (sResult);
}

/* ===== End of File ( TrnCpn.C ) ===== */
