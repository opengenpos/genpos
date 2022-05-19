/*
*---------------------------------------------------------------------------
*  Georgia Southern University, Rsearch Services and Sponsored Programs
*    (C) Copyright 2002 - 2020
*
*  NHPOS, donated by NCR Corp to Georgia Southern University, August, 2002.
*  Developemnt with NCR 7448 then ported to Windows XP and generic x86 hardware
*  along with touch screen support.
*
*---------------------------------------------------------------------------
=======================================================================*\
||  Erectronic  Cash  Register  -  90                                    ||
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  2170         *             AT&T GIS Corporation, E&M OISO   ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                Release 3.0              ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
:   Title              : Ecr-90 Header Definition                         :
:   Category           :                                                  :
:   Program Name       : TRANSACT.H                                       :
: --------------------------------------------------------------------------
:    Georgia Southern University Research Services Foundation
:    donated by NCR to the research foundation in 2002 and maintained here
:    since.
:       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
:       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
:       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
:       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
:       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
:       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
:       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
:
:    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
: --------------------------------------------------------------------------
:                      : YY-MM-DD   Ver.Rev          Comments             :
:   Program Written    : 92-06-03 : 00.00.00     :                        :
:                      : 95-05-30 : 03.00.00     : Release 3.0            :
:                      : 95-07-19 : 03.00.04     : CURQUAL_SCALE3DIGIT    :
:                      :          :              : (0x1000 -> 0x8000000L) :
:                      : 95-11-28 : 03.01.00     : R3.1                   :
:                      : 98-10-21 : 03.03.00     : TAR 89859              :
* Nov-21-15 : 02.02.01 : R.Chambers : using new typedefs for variables and source cleanup.
\*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*\
:   PVCS ENTRY
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

#if !defined(TRANSACT_H_INCLUDED)

#define TRANSACT_H_INCLUDED

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif


#define     TRN_SUCCESS_REPLACE     1    // TRN_SUCCESS and item was replaced and not added to end of file
#define     TRN_SUCCESS             0
#define     TRN_ERROR               (- 1)
#define		TRN_NO_READ_SIZE		(- 2)
#define		TRN_END_OF_FILE			(- 3)
#define		TRN_UPDATE_INDEX		(- 4)
#define     TRN_NEW_ITEM			(- 5)
#define		TRN_PREV_ADDCHK			(- 6)
#define     TRN_BUFFER_NEAR_FULL    (-45)
#define     TRN_BUFFER_FULL         (-46)        // indicates GCFQUAL_BUFFER_FULL condition reported by TrnCheckSize()
#define     TRN_BUFFER_ILLEGAL      (-47)


// The following defines are used to indicate the type of transaction
// storage file structure being used.  Based on this type will be determined
// what part of the TrnInformation global variable will contain the transaction
// file information such as file handles for index and storage files,
// index and storage file header offsets, and current index and storage file size.
//
// There are two versions of the memory resident current transaction data:
//   - TRANINFORMATION which is the standard current transaction data
//   - TRANINFSPLIT which is used with split from the transaction and contains a subset of TRANINFORMATION
//
// See the following functions as well as use of type TrnStorageType. Some of these are
// older versions of a utility function that need to be replaced by the newer version.
//  - TrnStoGetStorageInfo()
//  - TrnGetGCFQualSplPointer()   -> return a pointer to the correct GCF Qual, TranGCFQual
//  - TrnGetGCFQualSpl()
//  - TrnGetTISplPointer()  -> return a pointer to the correct itemizers
//  - TrnGetTISpl()
//  - TrnOpenSpl()
//  - ItemPreviousItemSpl()
//  - TrnItemSpl()
//  - TrnGetInfSpl()  -> return a pointer to the correct TRANINFSPLIT, a variant of TRANINFORMATION
#define     TRN_TYPE_ITEMSTORAGE    0
#define     TRN_TYPE_CONSSTORAGE    1
#define     TRN_TYPE_NOITEMSTORAGE  2
#define     TRN_TYPE_NOCONSSTORAGE  3
#define     TRN_TYPE_SPLITNONE      0       /* R3.1, request pointer to TrnInformation.TranItemizers, see TrnGetTISplPointer() */
#define     TRN_TYPE_SPLITA         4       /* R3.1, request pointer to TrnSplitA.TranItemizers, see TrnGetTISplPointer() */
#define     TRN_TYPE_SPLITB         5       /* R3.1, request pointer to TrnSplitB.TranItemizers, see TrnGetTISplPointer() */
#define		TRN_TYPE_PREVIOUS_CONDIMENT	6		/*SR 192 Condiment editing*/
#define		TRN_TYPE_PREVIOUS_CONDIMENT_CONS 7 /*SR 192 Condiment editing*/
#define     TRN_TYPE_POSTRECSTORAGE  8


#define		ITEM_UPDATE_ADJ			1

// see usage of the following transaction types in function ItemModSeatNo1() and the decision
// in function TrnSplCheckSplit() whose value is used in a number of places to change the logic
// based on whether the current transaction is being split or not during tender and totals.
//
// Change from incrementing numeric values (1, 2, 3, etc.) to bit map to allow for return value from single
// call to TrnSplCheckSplit() to be checked by Bitwise Oring defines together to eliminate function TrnSplBitCheckSplit().
//
// Can now use the following or change:
//    if (TrnSplCheckSplit() == TRN_SPL_SEAT
//        || TrnSplCheckSplit() == TRN_SPL_MULTI
//        || TrnSplCheckSplit() == TRN_SPL_TYPE2)
//    {
//
// to
//        if (TrnSplCheckSplit() & ( TRN_SPL_SEAT | TRN_SPL_MULTI | TRN_SPL_TYPE2)) {
//
/*----- Return of "TrnSplCheckSplit()",  R3.1 -----*/
#define     TRN_SPL_SEAT            0x01       /* Single Seat Transaction, TrnInformation.fsTransStatus & TRN_STATUS_SEAT_TRANS  */
#define     TRN_SPL_MULTI           0x02       /* Multi Seat Transaction, TrnInformation.fsTransStatus & TRN_STATUS_MULTI_TRANS  */
#define     TRN_SPL_TYPE2           0x04       /* Split Type 2 splitting transaction without seat assignments, TrnInformation.fsTransStatus & TRN_STATUS_TYPE2_TRANS */
#define     TRN_SPL_BASE            0x08       /* Base Transaction,  */
#define     TRN_SPL_SPLIT           0x10       /* Split Key Depressed, TrnInformation.TranCurQual.uchSplit == CLASS_SPLIT  */

/*----- Update Type of "TrnSplGetSeatTrans(),TrnSplGetBaseTrans()",  R3.1 -----*/
#define     TRN_SPL_NOT_UPDATE      0       /* Not Update Storage (Read) */
#define     TRN_SPL_UPDATE          1       /* Update Storage */
#define     TRN_SPL_UPDATE_ITEMIZ   2       /* Update Itemizer Only */
#define     TRN_SPL_WHOLETAX_CAL    3       /* Get Base Trans. with Seat, V3.3 */

/*----- Consoli./Post Sorage status(for TrnSplGetSeatTrans) 6/5/96 -----*/
#define     TRN_CONSOLI_STO         10      /* Consoli. Storage */
#define     TRN_POSTREC_STO         11      /* Post Rec. Storage */

/*----- Print Full Item/Print Only Total/Tender for CP Dup. Rec.(for TrnSplPrintSeat) 6/9/96 -----*/
#define     TRN_PRT_FULL         20      /* Print Full Item */
#define     TRN_PRT_TTL          21      /* Print Only Total/Tender */

/*----- Return of "TrnTaxSystem()", V3.3 -----*/
#define     TRN_TAX_US              0       /* US Tax System */
#define     TRN_TAX_CANADA          1       /* Canada Tax System */
#define     TRN_TAX_INTL            2       /* Int'l VAT System */


/*--------------------------------------------------------------------------
*       transaction mode qualifier
*       Memory resident data structure for the Cashier who is currently signed in.
*       This data area should be used for fairly static information only such
*       as data from the operator record of the currently signed in Cashier.
*
*       This data is stored in the totals update file to provide information to
*       the totals update procedure.  See TtlTUMMain() for when it is read from
*       the totals file and TtlUpdateFileFH() where it is written to the file.
--------------------------------------------------------------------------*/

    typedef  struct {
        ULONG   ulCashierID;                    /* cashier ID */
		UCHAR	uchCashierDrawer;				// cashier drawer
        ULONG   ulCashierSecret;                /* cashier secret code */
        UCHAR   fbCashierStatus;                /* cashier status. See also ITEMOPEOPEN */
        TCHAR   aszCashierMnem[NUM_OPERATOR];   /* cashier mnemonic */
        UCHAR   auchCasStatus[PARA_LEN_CASHIER_STATUS];               /* cashier status, V3.3 sized to CAS_NUMBER_OF_CASHIER_STATUS */
        USHORT  usFromTo[2];                    /* guest check No, from/to */
        UCHAR   uchCasChgTipRate;               /* charge tips rate, V3.3 */
        UCHAR   uchCasTeamNo;                   /* team no, V3.3 */
        ULONG   ulWaiterID;                     /* waiter ID */
        UCHAR   fbWaiterStatus;                 /* waiter status */
        TCHAR   aszWaiterMnem[NUM_OPERATOR];    /* waiter mnemonic */
        UCHAR   auchWaiStatus[2];               /* waiter status, V3.3 */
        UCHAR   uchWaiChgTipRate;               /* waiter tips rate, V3.3 */
        USHORT  fsModeStatus;                   /* mode status */
        UCHAR   uchTerm;                        /* Terminal Address,   R3.1 */
		TCHAR	aszTent[NUM_NAME];				// Tent implementation - tent name entry, FSC_TENT  ***PDINU
		SHORT	sCasSignInKey;					// Key used to sign in, used for Auto Sign Out
		USHORT  usSupervisorID;
		USHORT  usCtrlStrKickoff;               // control string number to execute at operator Sign-in
		USHORT  usWaiCtrlStrKickoff;            // control string number to execute at waiter/bartender/surrogate Sign-in
		USHORT  usStartupWindow;                // window number of window to display at operator Sign-in
		USHORT  usWaiStartupWindow;             // window number of window to display at waiter/bartender/surrogate Sign-in
		ULONG	ulGroupAssociations;            // group associations bit mask for the current operator
		ULONG	ulWaiGroupAssociations;         // group associations bit mask for waiter/bartender/surrogate
        USHORT  usReturnType;               /* default return type for CURQUAL_PRETURN */
		USHORT  usReasonCode;               // contains the reason code for a return from the reason code map and OEP window selection
    } TRANMODEQUAL;

/*--------------------------------------------------------------------------
*       mode qualifier status
--------------------------------------------------------------------------*/
#define     MODEQUAL_CASHIER_NOT_OPEN   0       /* cashier doesn't open */
#define     MODEQUAL_WAITER_NOT_OPEN    0       /* waiter doesn't open */

/* mode status, TRANMODEQUAL->fbCashierStatus or TRANMODEQUAL->fbWaiterStatus or ITEMOPEOPEN->fbCashierStatus */
#define     MODEQUAL_CASHIER_TRAINING   0x01    /* training cashier, used with TRANMODEQUAL->fbCashierStatus and ITEMOPEOPEN->fbCashierStatus */
#define     MODEQUAL_DRAWER_B           0x02    /* drawer B, used with TRANMODEQUAL->fbCashierStatus  */
#define     MODEQUAL_CASHIER_B          0x04    /* cashier B, used with TRANMODEQUAL->fbCashierStatus and ITEMOPEOPEN->fbCashierStatus */
#define     MODEQUAL_WAITER_TRAINING    0x01    /* training waiter, used with TRANMODEQUAL->fbCashierStatus and TRANMODEQUAL->fbWaiterStatus */
#define     MODEQUAL_WAITER_LOCK        0x10    /* waiter lock */
/* #define     MODEQUAL_WAITER_LOCK        0x02    / waiter lock */
#define     MODEQUAL_CASHIER_SIGNEDIN   0x40    /* cashier is signed-in in REG mode */

/* mode status, TRANMODEQUAL->fsModeStatus */
#define     MODEQUAL_CANADIAN           0x0001  /* Canadian tax system. see as well ItemCommonTaxSystem() and ITM_TAX_CANADA */
#define     MODEQUAL_INTL               0x0004  /* Int'l VAT System. see as well ItemCommonTaxSystem() and ITM_TAX_INTL,    V3.3 */
#define     MODEQUAL_NORECEIPT_DEF      0x0008  /* no receipt(default) */
#define     MODEQUAL_BARTENDER          0x0010  /* Bartender surrogate transaction */
#define     MODEQUAL_CASINT             0x0020  /* Cashier Interrupt transaction, R3.3 */
#define     MODEQUAL_WIC                0x0040  /* wic transaction, Saratoga */


/*--------------------------------------------------------------------------
*       transaction GCF qualifier
*  WARNING:  This structure is part of the data that is stored with a guest check
*            in the guest check file.  The guest check file uses blocks of space
*            with several blocks in the file being used to store the data for a
*            single guest check.  Before making any changes to this struct, you
*            must determine if any special data that is assumed to be in the first
*            block of the guest check file read is still in that first data block
*            after your changes.
*            Several members of this struct are required to be in that first data
*            block and should not be moved so that they are no longer there.
*            Also make sure that changes to this struct are coordinated with changes
*            to the same struct in the PCIF header files.
--------------------------------------------------------------------------*/
    typedef struct {
        ULONG   ulCashierID;                /* cashier ID */
        ULONG   ulPrevCashier;              /* previous cashier ID, V3.3 */
        USHORT  usConsNo;                   // 49:consecutive No
        ULONG   ulStoreregNo;               // store/reg No.  See ItemCountCons() - encoded as store number * 1000L + register number
        USHORT  usGuestNo;                  /* guest check number, also used with seat number in Post Guest Check split TrnSplitA/TrnSplitB  */
        UCHAR   uchCdv;                     /* guest check number check digit, MDC 12 Bit C */
        UCHAR   auchCheckOpenTime[3];       /* time (HH:MM:SS) and date (YY/MM/DD) check open */
        UCHAR   uchTeamNo;                  /* Team No, V3.3 */
        UCHAR   uchHourlyOffset;            /* Hourly total block offset, TRANGCFQUAL */
        USHORT  usNoPerson;                 /* No of person */
        USHORT  usNoPersonMax;              /* No of person (max amount) */
        USHORT  usTableNo;                  /* table No or unique transaction number MDC 234 Bit D, see ItemCommonUniqueTransNo() and flPrintStatus |= CURQUAL_UNIQUE_TRANS_NO */
        DCURRENCY   lCurBalance;                /* current balance */
        DCURRENCY   lTranBalance;               /* transfer balance */
        ULONG   fsGCFStatus;                /* current GCF status. See GCFQUAL_TRAINING, GCFQUAL_REGULAR1, etc. below. */
        ULONG   fsGCFStatus2;               /* GCF status. See GCFQUAL_PAYMENT_TRANS, GCFQUAL_REGULAR3, etc. below.   R3.0 */
		DCURRENCY   lCashBackAmount;            // amount for a Debit Cash Back tender
        UCHAR   uchSlipLine;                /* slip print line */
        UCHAR   uchPageNo;                  /* slip page No */
        USHORT  sPigRuleCo;                 /* pig rule counter,   R3.1 */
        TCHAR   auchCouponType[NUM_CPNTYPE];         /* single type coupon  R3.0 */
		USHORT  blRecalled;					// Recalled status   ***PDINU
		USHORT  usItemKdsCurLine;           // the current line of the KDS for the current transaction
        USHORT  usReturnType;               /* default return type for CURQUAL_PRETURN */
		USHORT  usReasonCode;               // contains the reason code for a return from the reason code map and OEP window selection
        TCHAR   aszName[NUM_NAME];          /* customer name R3.0 */
		USHORT	sItemCount;					/* Retains Item count in guest check - Justin Mason 4/10/07*/
        UCHAR   auchCheckOpen[6];           /* time (HH:MM:SS) and date (YY/MM/DD) check open */
        UCHAR   auchCheckHashKey[6];        /* date and time as mm.dd.hh.mm.ss.0, */
		UCHAR   uchUniqueIdentifier[24];    // Unique identifer for returns as digits, binary coded decimal
		UCHAR   uchUniqueIdentifierReturn[24];    // Unique identifer of original transaction for returns as digits, binary coded decimal
		USHORT  usPreauthItemNumber;              // the item number in the Preauth batch file if this is a Preauth Batch transaction, zero otherwise.
		USHORT  usCheckTenderId;                  // Save area for unique tender id for split checks identifying specific tender
		ULONG	ulStoredBlockOffset;
		DATE_TIME DateTimeStamp;                  // date time stamp for transaction start
        UCHAR   auchUseSeat[NUM_SEAT];             /* use seat#,          R3.1 */
        UCHAR   auchFinSeat[NUM_SEAT];             /* finalize seat#,     R3.1 */
        UCHAR   auchSeatDisc[NUM_SEAT+1];          /* bit indicators for discounts for Each Seat plus seat 'B' - see GCFQUAL_REG1_SEAT, V3.3 */
		UCHAR   aszNumber[NUM_SEAT_CARD + 1][NUM_NUMBER_PADDED];      /* account number */
		UCHAR   auchExpiraDate[NUM_SEAT_CARD + 1][NUM_EXPIRA];        /* acct expiration date */
		UCHAR   auchMSRData[NUM_SEAT_CARD + 1][NUM_MSRDATA_PADDED];            /* msr data  */
		UCHAR	auchAcqRefData[NUM_SEAT_CARD + 1][NUM_ACQREFDATA];	// XEPT Acquirer Reference Number - used for pre-authorization
		ITEMTENDERAUTHCODE  authcode[NUM_SEAT_CARD + 1];           // XEPT authorization Code - from DSI Client XML.
		ITEMTENDERREFNO     refno[NUM_SEAT_CARD + 1];		        // XEPT Reference Number generated for processor of transaction for EEPT
		ITEMTENDERINVNO     invno[NUM_SEAT_CARD + 1];              // XEPT Invoice number from processor of transaction for EEPT
		ITEMTENDERSRNO      srno[NUM_SEAT_CARD + 1];               // XEPT RecordNo - from DSI Client XML.  record identifier in current batch.
		ITEMSALESPPI	    TrnSalesPpi;
		ITEMSALESPM		    TrnSalesPM;
		ITEMSALESGIFTCARD	TrnGiftCard[NUM_SEAT_CARD + 1];  //Gift Card information
    } TRANGCFQUAL;


/*--------------------------------------------------------------------------
*       GCF qualifier status  (fsGCFStatus)
--------------------------------------------------------------------------*/
#define     GCFQUAL_TRAINING            0x00001      /* training transaction */
#define     GCFQUAL_WIC                 0x00002      /* wic transaction, Saratoga */
#define     GCFQUAL_PVOID               0x00008      /* preselect void GCF */
#define     GCFQUAL_REGULAR1            0x00010      /* regular discount 1. See GCFQUAL_REGULAR3, etc. used with fsGCFStatus2 below. */
#define     GCFQUAL_REGULAR2            0x00020      /* regular discount 2. See GCFQUAL_REGULAR3, etc. used with fsGCFStatus2 below. */
#define     GCFQUAL_CHARGETIP           0x00040      /* charge tip */
#define     GCFQUAL_NEWCHEK_CASHIER     0x00080      /* newcheck is cashier */
#define     GCFQUAL_ADDCHECK_CASHIER    0x00100      /* addcheck is cashier */
#define     GCFQUAL_BUFFER_FULL         0x00200      /* buffer full */
#define     GCFQUAL_DRIVE_THROUGH       0x00400      /* drive through type GCF */
#define     GCFQUAL_BUFFER_PRINT        0x00800      /* buffer print for service total key */
#define     GCFQUAL_SERVICE1            0x01000      /* use service total #1, STD_MAX_SERVICE_TOTAL */
#define     GCFQUAL_SERVICE2            0x02000      /* use service total #2 */
#define     GCFQUAL_SERVICE3            0x04000      /* use service total #3 */
#define     GCFQUAL_USEXEMPT            0x08000      /* tax exempt transaction for US, see also MOD_USEXEMPT and CLASS_UITAXEXEMPT1 with FSC_TAX_MODIF  */
#define		GCFQUAL_TRETURN				0x10000		 /* this transaction is a return */

#define     GCFQUAL_GSTEXEMPT           0x08000      /* GST tax exempt,     R3.1 */
#define     GCFQUAL_PSTEXEMPT           0x00002      /* PST tax exempt,     R3.1 */

/*--------------------------------------------------------------------------
*       GCF qualifier status  (fsGCFStatus2)
--------------------------------------------------------------------------*/
#define     GCFQUAL_PAYMENT_TRANS       0x00001      /* Payment Transaction */
#define     GCFQUAL_USESEAT             0x00002      /* Use Seat# at Newcheck/Reorder, R3.1 */
#define     GCFQUAL_REGULAR3            0x00010      /* regular discount 3, R3.1  See GCFQUAL_REGULAR1 used with fsGCFStatus above. */
#define     GCFQUAL_REGULAR4            0x00020      /* regular discount 4, R3.1  See GCFQUAL_REGULAR1 used with fsGCFStatus above. */
#define     GCFQUAL_REGULAR5            0x00040      /* regular discount 5, R3.1  See GCFQUAL_REGULAR1 used with fsGCFStatus above. */
#define     GCFQUAL_REGULAR6            0x00080      /* regular discount 6, R3.1  See GCFQUAL_REGULAR1 used with fsGCFStatus above. */
#define     GCFQUAL_REGULAR_SPL         0x00100      /* R/D Performed for Split, R3.1 */
#define     GCFQUAL_AUTOCHARGETIP       0x00200      /* auto charge tip, V3.3 */
#define     GCFQUAL_EPT_HASH_MADE       0x01000      /* */

/*--------------------------------------------------------------------------
*       GCF qualifier status  (auchCouponType)
--------------------------------------------------------------------------*/
#define     GCFQUAL_SINGLE_COUPON       0x01        /* on = single type */

/*--------------------------------------------------------------------------
*       GCF qualifier status  (auchSeatDisc[10]),               V3.3
*       since auchSeatDisc[i] is a bit map to show which regular discount used for
*       a seat, most places doing a check for discount on a seat will use a shift
*       operation with the number of bits shifted depending on the regular discount
*       offset from CLASS_REGDISC1 with the starting shift value being GCFQUAL_REG1_SEAT.
--------------------------------------------------------------------------*/
#define     GCFQUAL_REG1_SEAT           0x0001      /* regular discount 1, CLASS_REGDISC1 */
#define     GCFQUAL_REG2_SEAT           0x0002      /* regular discount 2, CLASS_REGDISC2 */
#define     GCFQUAL_REG3_SEAT           0x0004      /* regular discount 3, CLASS_REGDISC3 */
#define     GCFQUAL_REG4_SEAT           0x0008      /* regular discount 4, CLASS_REGDISC4 */
#define     GCFQUAL_REG5_SEAT           0x0010      /* regular discount 5, CLASS_REGDISC5 */
#define     GCFQUAL_REG6_SEAT           0x0020      /* regular discount 6, CLASS_REGDISC6 */


/*--------------------------------------------------------------------------
*       transaction current qualifier
*       Memory resident data structure for the transaction being rung up.
*       This data area should be used for temporary transaction information
*       such as printer status information.
*
*       This data is stored in the totals update file to provide information to
*       the totals update procedure.  See TtlTUMMain() for when it is read from
*       the totals file and TtlUpdateFileFH() where it is written to the file.
--------------------------------------------------------------------------*/
    typedef struct {
        ULONG   flPrintStatus;              /* print status */
        ULONG   fsCurStatus;                /* current status */
        ULONG   fsCurStatus2;               /* current status 2,  R3.1 */
        ULONG   ulStoreregNo;               /* store/reg No. See ItemCountCons() - encoded as store number * 1000L + register number */
        USHORT  usConsNo;                   /* consecutive No */
		USHORT  usReturnsFrom;              // contains the guest check number used for creating a returns transaction, COMMON_PAIDOUT_RETURNS.
        UCHAR   auchTotalStatus[NUM_TOTAL_STATUS];   /* total key parameter,   R3.1,  auchTotalStatus[0] contains total key number and type if total key pressed or zero */
        UCHAR   uchSplit;                   /* split key#, normally total key type, ItemTotal->auchTotalStatus[0]  R3.1 */
        UCHAR   uchPrintStorage;            /* target storage at print */
        UCHAR   uchKitchenStorage;          /* target storage at kitchen */
        UCHAR   fbNoPrint;                  /* no print bit map, see CURQUAL_NO_R etc defines below */
        UCHAR   fbCompPrint;                /* compulsory print bit map, see CURQUAL_COMP_R etc defines below  */
        UCHAR   uchSeat;                    /* seat#, when doing a split transaction   R3.1 */
		UCHAR	fbOepCondimentEdit;			/* flag that tells if we are editing an OEP condiment JHHJ*/
		UCHAR	uchOrderDec;
} TRANCURQUAL;



/*--------------------------------------------------------------------------
*       current qualifier status
*	These masks are used with the fbNoPrint member of the 
*	TRANCURQUAL struct to indicate various printer provisioning settings.
*	These are typically used to reflect various MDC bit settings.
--------------------------------------------------------------------------*/
#define     CURQUAL_NO_R                PRT_RECEIPT       /* no print(receipt), will turn off PRT_RECEIPT setting */
#define     CURQUAL_NO_J                PRT_JOURNAL       /* no print(journal), will turn off PRT_JOURNAL setting */
#define     CURQUAL_NO_V                PRT_VALIDATION    /* no print(validation) */
#define     CURQUAL_NO_S                PRT_SLIP          /* no print(slip), will turn off PRT_SLIP */

#define     CURQUAL_NO_EPT_LOGO_SIG     0x0010    /* no print EPT Logo or signature line if tender below floor limit */
#define     CURQUAL_NO_SECONDCOPY       0x0020    /* no print second copy of receipt */
#define     CURQUAL_NO_EPT_RESPONSE     0x0040    /* no print EPT authorization response text from card processor */

// following defines are used to mask on/off compulsory printer device target bits in TrnInformation.TranCurQual.fbCompPrint
#define     CURQUAL_COMP_R              PRT_RECEIPT       /* compulsory print(R) */
#define     CURQUAL_COMP_J              PRT_JOURNAL       /* compulsory print(J) */
#define     CURQUAL_COMP_V              PRT_VALIDATION    /* compulsory print(V) */
#define     CURQUAL_COMP_S              PRT_SLIP          /* compulsory print(S) */

/**
*	print status current qualifier
*
*	These masks are used with the flPrintStatus member of the 
*	TRANCURQUAL struct to indicate various printer provisioning settings.
*	These are typically used to reflect various MDC bit settings.
*
*	See function ItemCurPrintStatus() for an example of setting the
*	flPrintStatus based on MDC bit settings.
*
*	The mask CURQUAL_GC_SYS_MASK is used to isolate the section of
*	flPrintStatus in order to test the type of system which is indicated
*	by two bits to show the four types of systems using values from 0 to 3
*	in that area of the variable.
**/

#define     CURQUAL_VAL_TYPE1           0x0000L /* cons#, id, time, CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT0) */
#define     CURQUAL_VAL_TYPE2           0x0001L /* cons#, id, date */
#define     CURQUAL_VAL_TYPE3           0x0003L /* cons#, time, date, CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT1)*/
#define     CURQUAL_VAL_MASK            0x0003L /* mask for different CURQUAL_VAL_TYPE bits */
#define     CURQUAL_TAKEKITCHEN         0x0004L /* not take to kitchen, CliParaMDCCheck(MDC_KPTR_ADR, ODD_MDC_BIT1)  */

// System Types which are System Type from CLASS_PARAFLEXMEM.FLEX_GC_ADR shifted left 4 bits
// typically used such as if ( (TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK )
// or in some cases if ( TranCurQualPtr->flPrintStatus & CURQUAL_POSTCHECK )      // postcheck or store/recall
//  - Post Check is normally used for Table Service applications
//  - Store/Recall is normally used for Quick Service applications, probably most recent functionality
//  - Pre-Check is normally used for Cafeteria or Retail applications, probably oldest functionality
#define     CURQUAL_GC_SYS_MASK         0x0030L /* mask to isolate the bits indicating system type */
#define     CURQUAL_PRE_BUFFER          0x0000L /* precheck/buffering, FLEX_PRECHK_BUFFER */
#define     CURQUAL_PRE_UNBUFFER        0x0010L /* precheck/unbuffering, FLEX_PRECHK_UNBUFFER */
#define     CURQUAL_POSTCHECK           0x0020L /* postcheck, FLEX_POST_CHK, may be used to check for not a Precheck system */
#define     CURQUAL_STORE_RECALL        0x0030L /* store/recall, FLEX_STORE_RECALL*/

//------------------------------------------------------------------------------------------
// Following defines used with TrnInformation.TranCurQual.flPrintStatus to control printing.
//    following are active if ItemCommonTaxSystem() == ITM_TAX_CANADA
#define     CURQUAL_CANADA_ALL          0x0000L /* Canadian tax print (all) */
#define     CURQUAL_CANADA_GST_PST      0x0200L /* Canadian tax print (GST/PST) */
#define     CURQUAL_CANADA_INDI         0x0100L /* Canadian tax print (individual) */

	// following two defines indicate MDC 28, MDC_DEPT2_ADR Bit C and Bit D. See also CLASS_SETMENU 
#define     CURQUAL_SETMENU_KITCHEN     0x1000L /* setmenu child PLU print (K/P), Bit C Print linked menu items on Remote Device */
#define     CURQUAL_SETMENU             0x2000L /* setmenu child PLU print (R/J), Bit D Print linked menu items on R/J printer */

//    following control print element formats such as Date format
#define     CURQUAL_DDMMYY              0x00004000L     /* DD/MM/YY date format */
#define     CURQUAL_MILITARY            0x00008000L     /* use military hour */
#define     CURQUAL_NO_ELECTRO          0x00010000L   /* electro stamp provided */
#define     CURQUAL_NOT_PRINT_PROM      0x00020000L   /* don't print promition header */
#define     CURQUAL_SINGLE_CHCK_SUM     0x00040000L   /* print check sum line by single wide */
#define     CURQUAL_CD_GCF_NO           0x00080000L   /* GCF no. with check digit */
#define     CURQUAL_SOFTCHECK           0x00100000L   /* soft check unbuffer system */
#define     CURQUAL_PMCOND_SORT         0x00200000L   /* PM/Condiment is sort, R3.0 */
#define     CURQUAL_POSTRECT            0x00400000L   /* post receipt */
#define     CURQUAL_NOT_SLIP_PROM       0x00800000L   /* not print promotion header at slip */
#define     CURQUAL_UNIQUE_TRANS_NO     0x01000000L  /* use unique transaction number, CliParaMDCCheck(MDC_TRANNUM_ADR, EVEN_MDC_BIT0) */
#define     CURQUAL_PRINT_PRIORITY      0x02000000L  /* print priority,  R3.0 */
#define     CURQUAL_PARKING             0x04000000L  /* Parking Receipt,  R3.0 */
#define     CURQUAL_SCALE3DIGIT         0x08000000L  /* scale's digits under decimal point */
#define     CURQUAL_EPAY_BALANCE        0x10000000L  /* Account balance for Gift Card or EPD, see also CLASS_BALANCE */

/* V3.3 */
#define     CURQUAL_NOT_PRINT_VAT       0x0200L     /* not print VAT */
#define     CURQUAL_NOT_PRINT_VATAPP    0x0400L     /* not print VATable */
#define     CURQUAL_NOT_PRINT_SER       0x0800L     /* not print service */
#define     CURQUAL_EXCLUDE_VAT         0x0040L     /* excluded vat */
#define     CURQUAL_EXCLUDE_SERV        0x0100L     /* excluded service */
                                                    /* shares with canadian tax */
/*--------------------------------------------------------------------------
*       current qualifier status  (auchTotalStatus) - Total key parameters
*       auchTotalStatus[0] contains total key number and type if total key pressed or zero
*       auchTotalStatus[3] lowest nibble contains kitchen printer indicators #1 through #4 - auchTotalStatus[3] & 0x0F
*       auchTotalStatus[5] lowest nibble contains kitchen printer indicators #5 through #8 - auchTotalStatus[5] & 0x0F
*
*    See Also: macros beginning with TTL_STAT_FLAG_ which accept a auchTotalStatus[] and test the appropriate bit of the correct element.
*              we want to move forward with replacing these defines with those defines instead in order to reduce the amount of duplicated
*              and there fore confusing defines.
--------------------------------------------------------------------------*/
#define     CURQUAL_TOTAL_FLAG_TAX_1            0x0001      // auchTotalStatus[1] calculate US tax #1 if set - ODD_MDC_BIT0 -, see total key macro ITM_TTL_FLAG_TAX_1()
#define     CURQUAL_TOTAL_FLAG_TAX_2            0x0010      // auchTotalStatus[1] calculate US tax #2 if set - EVEN_MDC_BIT0 -, see total key macro ITM_TTL_FLAG_TAX_2()

#define     CURQUAL_TOTAL_FLAG_TAX_3            0x0001      // auchTotalStatus[2] calculate US tax #3 if set - ODD_MDC_BIT0 -, see total key macro ITM_TTL_FLAG_TAX_3()
#define     CURQUAL_TOTAL_NOT_CLEAR_TAX_ITEMIZER   0x0010   // auchTotalStatus[2] NOT clear tax itemizer - EVEN_MDC_BIT0 -, see total key macro ITM_TTL_FLAG_NOT_CLEAR_TAX_ITEMIZER
#define     CURQUAL_TOTAL_PRINT_BUFFERING       0x0020      // auchTotalStatus[2] print buffering option if set - EVEN_MDC_BIT1 -, see total key macro ITM_TTL_FLAG_PRINT_BUFFERING()
#define     CURQUAL_TOTAL_ALLOW_AUTO_COUPON     0x0040      // auchTotalStatus[2] allow auto coupon processing if set - EVEN_MDC_BIT2 -, see total key macro ITM_TTL_FLAG_ALLOW_AUTO_COUPON()
#define     CURQUAL_TOTAL_COMPUL_DRAWER         0x0080      // auchTotalStatus[2] compulsory drawer if set - EVEN_MDC_BIT3 -, see total key macro ITM_TTL_FLAG_DRAWER()

#define     CURQUAL_TOTAL_KITPRT_PRINTER_MASK   0x000F      // use with AND to obtain kitchen printer number from auchTotalStatus[3] and auchTotalStatus[5]
#define     CURQUAL_TOTAL_KITPRT_MENU_SEND      0x0010      // auchTotalStatus[3] use AND logic on send Menu to kitchen printer if set - EVEN_MDC_BIT0 -, auchTotalStatus[3]
#define     CURQUAL_TOTAL_PRINT_DOUBLE          0x0020      // auchTotalStatus[3] PRT_DOUBLE if set - EVEN_MDC_BIT1 -, see total key macro ITM_TTL_FLAG_PRINT_DOUBLEWIDE()
#define     CURQUAL_TOTAL_COMPULSRY_VALIDATION  0x0040      // auchTotalStatus[3] compulsory validation print - EVEN_MDC_BIT2 -, see total key macro ITM_TTL_FLAG_PRINT_VALIDATION()
#define     CURQUAL_TOTAL_COMPULSRY_SLIP        0x0080      // auchTotalStatus[3] compulsory slip print - EVEN_MDC_BIT3 -, see total key macro ITM_TTL_FLAG_COMPULSORY_SLIP()

#define     CURQUAL_TOTAL_BUFFERED_SUBTOTAL     0x0001      // auchTotalStatus[4] buffered subtotal, Subtotal key only! - ODD_MDC_BIT0 -
#define     CURQUAL_TOTAL_DISPLAY_LCD_3         0x0001      // auchTotalStatus[4] display on LCD #3 window - ODD_MDC_BIT0 -, see total key macro ITM_TTL_FLAG_DISPLAY_LCD3()
#define     CURQUAL_TOTAL_PRE_AUTH_TOTAL        0x0010      // auchTotalStatus[4] total key is Pre Auth key - EVEN_MDC_BIT0 -, see total key macro ITM_TTL_FLAG_PRE_AUTH()
#define     CURQUAL_TOTAL_NOCONSOLIDATE_PRINT   0x0020      // auchTotalStatus[4] do not consolidate print - EVEN_MDC_BIT1 -, see total key macro ITM_TTL_FLAG_NOCONSOLIDATEPRINT()

#define     CURQUAL_TOTAL_EPT_PRE_AUTH          0x0010      //  auchTotalStatus[5] indicates using EPT for total key Pre Auth - EVEN_MDC_BIT0 -,
#define     CURQUAL_TOTAL_DT_PUSH_BACK          0x0020      //  auchTotalStatus[5] indicates push back for Flexible Drive Thru terminal - EVEN_MDC_BIT1 -, see total key macro ITM_TTL_FLAG_DT_PUSH_BACK


/** ==============================================================================
	work space qualifiers.
**/

/**

	These masks are used with the fsCurStatus member of the
	TRANCURQUAL struct to indicate various system operational
	states as a transaction is processed by various parts of
	BusinessLogic in response to operator actions.

**/
#define     CURQUAL_PVOID               0x00000001      /* preselect void condition */
#define     CURQUAL_TRAINING            0x00000002      /* training print */
#define     CURQUAL_TABLECHANGE         0x00000004      /* table No changed, checked in receipt printing */
#define     CURQUAL_PERSONCHANGE        0x00000008      /* No of person changed, checked in receipt printing */
#define     CURQUAL_CASHIER             0x00000000      /* cashier operation type, used with CURQUAL_OPEMASK */
#define     CURQUAL_NEWCHECK            0x00000010      /* newcheck operation type - see ItemTransNCCommon(), used with CURQUAL_OPEMASK */
#define     CURQUAL_REORDER             0x00000020      /* reorder operation type - see ItemTransOpenRO(), used with CURQUAL_OPEMASK */
#define     CURQUAL_ADDCHECK            0x00000030      /* addcheck operation type - see ItemTransOpenAC(), used with CURQUAL_OPEMASK */
#define     CURQUAL_OPEMASK             0x00000030      /* operation mask to check operation type */
#define     CURQUAL_CANCEL              0x00000040      /* Transaction Cancel operation */
#define     CURQUAL_TRANSFER            0x00000080      /* Check Transfer operation */
#define     CURQUAL_MULTICHECK          0x00000100      /* multi check payment operation */
#define     CURQUAL_ADDCHECK_SERVT      0x00000200      /* service total at addcheck,   R3.1*/
#define     CURQUAL_WIC                 0x00000400      /* wic transaction, Saratoga */
#define     CURQUAL_TRAY                0x00000800      /* tray total transaction started, along with ItemSalesLocal.fbSalesStatus & SALES_ITEMIZE */
#define		CURQUAL_SERVTOTAL			0x00001000		/* SR 597 Service Total has ben pressed */
#define		CURQUAL_CUST_DUP			0x00002000		/* print customer duplicate operation */
#define		CURQUAL_MERCH_DUP			0x00004000		/* print merchant duplicate operation */
#define		CURQUAL_TRETURN				0x00008000		/* this transaction is a return */
#define     CURQUAL_PRINT_EPT_LOGO      0x00010000      /* print the EPT Logo and signature line, PrtSoftCHK(SOFTCHK_EPT1_ADR) */
#define		CURQUAL_PRETURN				0x00020000		/* this transaction is a preselect return */
#define		CURQUAL_DUPLICATE_COPY		0x00040000		/* this transaction is a preselect return */

#define     CURQUAL_PRESELECT_MASK      (CURQUAL_PVOID | CURQUAL_PRETURN)

#define		SET_RECEIPT_FIRST_COPY      0x01        // Used with SetReceiptPrint to determine Customer/Merchant receipt print order
#define		SET_RECEIPT_SECOND_COPY     0x02        // Used with SetReceiptPrint to determine Customer/Merchant receipt print order
#define     SET_RECEIPT_TRAN_EPT        0x10        // Used with SetReceiptPrint to indicate the transaction type is an EPT transaction such as TENDERKEY_TRANTYPE_CREDIT
#define     SET_RECEIPT_TRAN_PREPAID    0x20        // Used with SetReceiptPrint to indicate the transaction type is a prepaid such as TENDERKEY_TRANTYPE_PREPAID

/**
	These masks are used with the fsCurStatus2 member of the
	TRANCURQUAL struct to indicate various system states.
**/
#define		CURQUAL_ORDERDEC_DECLARED	0x0001		/* Order Declaration JHHHJ*/
#define		CURQUAL_ORDERDEC_PRINTED	0x0002		/* Has Order Dec. Been Printed? JHHJ*/
#define		CURQUAL_ORDERDEC_KPRINTED	0x0004		/* Has Order Dec. Been Kitchen Printed? JHHJ*/ 
//			--CURQUAL_Empty--			0x0008
//			--CURQUAL_Empty--			0x0010
#define     CURQUAL_REQUIRE_NON_GIFT    0x0020      // used with FreedomPay, not allow FreedomPay or other gift card at tender
//			--CURQUAL_Empty--			0x0040
//			--CURQUAL_Empty--			0x0080
//#define	CURQUAL_PRT_GIFT_RECEIPT       0x0100		/* Gift Receipt JHHJ, same as PRT_GIFT_RECEIPT which is what is used in the source. */
//#define	CURQUAL_PRT_POST_RECEIPT       0x0200		/* Print Post Receipt JHHJ, same as PRT_POST_RECEIPT which is what is used in the source. */
//#define	CURQUAL_PRT_DBL_POST_RECEIPT   0x0400		/* Print Post Receipt JHHJ, same as PRT_DBL_POST_RECEIPT which is what is used in the source. */
//#define	CURQUAL_PRT_PREAUTH_RECEIPT    0x0800		/* Print Preauth Receipt, same as PRT_PREAUTH_RECEIPT which is what is used in the source. */
//#define	CURQUAL_PRT_DUPLICATE_COPY     0x1000		/* Print Post Receipt, same as PRT_DUPLICATE_COPY which is what is used in the source. */
//#define	CURQUAL_PRT_DEMAND_COPY        0x2000		/* Print Post Receipt, same as PRT_DEMAND_COPY which is what is used in the source. */
//			--CURQUAL_Empty--              0x4000
#define		CURQUAL_NOTALLOW_0_TEND		0x8000		/* Allow $.00 tender   ***PDINU


/*--------------------------------------------------------------------------
*       transaction itemizers (us unique)
--------------------------------------------------------------------------*/

    typedef struct {
        DCURRENCY    lAffectTaxable[STD_TAX_ITEMIZERS_MAX];      /* taxable itemizer */
        DCURRENCY    lNonAffectTaxable[STD_TAX_ITEMIZERS_MAX];   /* taxable itemizer */
        DCURRENCY    lDiscTaxable[STD_TAX_ITEMIZERS_MAX * 2];    /* discountable taxable must hold taxable for two discount itemizers, see function TrnDiscTaxUSComn(), TrnSalesModUSComn() */
        DCURRENCY    lTaxItemizer[STD_TAX_ITEMIZERS_MAX];        /* tax itemizer */
        DCURRENCY    lPrintTaxable[STD_TAX_ITEMIZERS_MAX];       /* print taxable itemizer (postcheck) */
        DCURRENCY    lPrintTax[STD_TAX_ITEMIZERS_MAX];           /* print tax itemizer (postcheck) */
        DCURRENCY    lAffectTaxExempt[STD_TAX_ITEMIZERS_MAX];    /* tax exempt itemizer */
        DCURRENCY    lPrintTaxExempt[STD_TAX_ITEMIZERS_MAX];     /* print tax exempt itemizer (postcheck) */
        DCURRENCY    lFoodStampable;         /* food stampable itemizer */
        DCURRENCY    lFoodTaxable[STD_TAX_ITEMIZERS_MAX];        /* food stampable/taxable itemizer */
        DCURRENCY    lFSDiscountable[STD_DISC_ITEMIZERS_MAX];    /* food stampable disc. itemizer */
        DCURRENCY    lFSDiscTaxable[STD_DISC_ITEMIZERS_MAX][STD_TAX_ITEMIZERS_MAX];
        DCURRENCY    lFoodWholeTax;          /* food stampable whole taxable */
        DCURRENCY    lVoidDiscNonAffectTaxable[STD_DISC_ITEMIZERS_MAX][STD_TAX_ITEMIZERS_MAX];   /* TAR191625 */
        DCURRENCY    lVoidDiscFSDiscTaxable[STD_DISC_ITEMIZERS_MAX][STD_TAX_ITEMIZERS_MAX];	/* TAR191625 */
    } TRANUSITEMIZERS;

/*--------------------------------------------------------------------------
*       transaction itemizers (Canadian unique)
--------------------------------------------------------------------------*/

//  Canadian tax calculations use a series of tax buckets such as the following indexes.
//  This list should have as many elements as the value of STD_PLU_ITEMIZERS.
//  Originally NHPOS Rel 1.4 seemed to divide itemizers into three segments, 9+2+3,
//  and originally had only 9 types corresponding to TRANCANADA_MEAL through TRANCANADA_BAKED below.
//
//  However a tenth type was added for special baked goods for Rel 2.0.3 and it appears the change
//  was not properly carried through to all of the source code that needed changes.
//  See Changes 3-29-04 JHHJ as well as TH ISSUE JHHJ concerning Tim Horton's tax calculation issue.
//  Changes appear to have actually been made in NHPOS 2.0.3.11 by J.Hall on 3/08/2006.
//
//  See also defines for CANTAX_MEAL, CANTAX_CARBON, etc. for the bit maps used with PLU status settings
//  accessed through pSalesItem->ControlCode.auchPluStatus[1].
//
//  See also use of define STD_PLU_ITEMIZERS_PLUS for itemizer count size.
//  In a number of places arrays are defined with a size of STD_PLU_ITEMIZERS+2+3 which corresponds to:
//    - count of STD_PLU_ITEMIZERS array elements for the PLU type of tax itemizers
//    - two specialized tax itemizers (limitable and Tax-On-Tax)
//    - three tax type itemizers (GST, PST1 limitable, PST1 limitable Tax on Tax)
#define TRANCANADA_MEAL         0        // meal - CANTAX_MEAL
#define TRANCANADA_CARB_BEV     1        // carbonated beverage - CANTAX_CARBON
#define TRANCANADA_SNACK        2        // snack - CANTAX_SNACK
#define TRANCANADA_BEER_WINE    3        // beer and wine - CANTAX_BEER
#define TRANCANADA_LIQUOR       4        // liquor - CANTAX_LIQUOR
#define TRANCANADA_GROCERY      5        // grocery - CANTAX_GROCERY
#define TRANCANADA_TIP          6        // tip - CANTAX_TIP
#define TRANCANADA_PST1_ONLY    7        // PST1 only - CANTAX_PST1
#define TRANCANADA_BAKED        8        // baked goods - CANTAX_BAKED
#define TRANCANADA_SPECL_BAKED  9        // special baked goods (added for Rel 2.0.3 by JHHJ) - CANTAX_SPEC_BAKED

#define TRANCANADA_LIMITABLE   10
#define TRANCANADA_TAXONTAX    11
#define TRANCANADA_GST         12        // GST
#define TRANCANADA_PST1LIMIT   13        // PST1 limitable
#define TRANCANADA_PST1TOT     14        // PST1 limitable for tax on tax. See also STD_PLU_ITEMIZERS_PLUS.

    typedef struct {
        DCURRENCY  lAffTaxable[STD_TAX_ITEMIZERS_MAX];      // there are four Canadian tax indicators (GST, PST1, PST2, PST3)
        DCURRENCY  lAffTax[STD_TAX_ITEMIZERS_MAX];          // there are four Canadian tax indicators (GST, PST1, PST2, PST3)
        DCURRENCY  lPrtTaxable[STD_TAX_ITEMIZERS_MAX];      // there are four Canadian tax indicators (GST, PST1, PST2, PST3)
        DCURRENCY  lPrtTax[STD_TAX_ITEMIZERS_MAX];          // there are four Canadian tax indicators (GST, PST1, PST2, PST3)

        DCURRENCY  lTaxable[STD_PLU_ITEMIZERS];             // tax itemizers for TRANCANADA_MEAL through TRANCANADA_SPECL_BAKED

					// discount itemizers for each of the Canadian PLU tax types, TRANCANADA_MEAL through TRANCANADA_SPECL_BAKED,
					// plus the limitable and Tax-On-Tax itemizers plus the GST and PST limitable and PST Tax-On-Tax itemizers.
        DCURRENCY  lDiscTax[STD_PLU_ITEMIZERS_PLUS];        /* discountable/tax total itemizer - was 9+2+3 */
        DCURRENCY  lDiscTaxable[STD_DISC_ITEMIZERS_MAX][STD_PLU_ITEMIZERS_PLUS];  /* discountable/taxable itemizer by discount type 4/24/96 */// - was 9+2+3 Database Changes 3-29-04 JHHJ
        DCURRENCY  lVoidDiscTax[STD_DISC_ITEMIZERS_MAX][STD_PLU_ITEMIZERS_PLUS];  /* TAR191625 - was 9+2+3 */

        DCURRENCY  lMealLimitTaxable;
        DCURRENCY  lMealNonLimitTaxable;
        DCURRENCY  lPigRuleTaxable;            /* Add 10-21-98 */
    } TRANCANADAITEMIZERS;

/*--------------------------------------------------------------------------
*       Transaction Itemizers (Int'l Unique)                    V3.3
* International or VAT discount calculations have three different buckets.
* See function TrnRegDiscVATCalcComn() and function ItemDiscGetTaxMDC().
*   - discount itemizer #1 totals for Regular Discount #1 and optionally Regular Discount #3 through #6
*   - discount itemizer #2 totals for Regular Discount #2 and optionally Regular Discount #3 through #6
*   - discount main itemizer totals for Regular Discount #3 through #6
--------------------------------------------------------------------------*/

    typedef struct {
        DCURRENCY      lServiceable;           /* serviceable itemizer */
        DCURRENCY      lDiscountable_Ser[STD_DISC_ITEMIZERS_MAX];   /* discountable itemizer
                                            with service, 0:SI#1, 1:SI#2, 2:MI 3->6 JHHJ 3-29-04 Database changes*/
        DCURRENCY      lVATSer[STD_TAX_ITEMIZERS_MAX];             /* VATable with service */
        DCURRENCY      lVATNon[STD_TAX_ITEMIZERS_MAX];             /* VATable without service */
        DCURRENCY      lDisSer[STD_DISC_ITEMIZERS_MAX][STD_TAX_ITEMIZERS_MAX];       /* discountable with service */
        DCURRENCY      lDisNon[STD_DISC_ITEMIZERS_MAX][STD_TAX_ITEMIZERS_MAX];       /* discountable without service */
        DCURRENCY      lVoidServiceable[STD_DISC_ITEMIZERS_MAX];                     /* serviceable itemizer */
        DCURRENCY      lVoidVATSer[STD_DISC_ITEMIZERS_MAX][STD_TAX_ITEMIZERS_MAX];   /* VATable with service */
        DCURRENCY      lVoidVATNon[STD_DISC_ITEMIZERS_MAX][STD_TAX_ITEMIZERS_MAX];   /* VATable without service */
    } TRANINTLITEMIZERS;

/*--------------------------------------------------------------------------
*       transaction itemizers
--------------------------------------------------------------------------*/

    typedef union {
        TRANUSITEMIZERS     TranUsItemizers;        /* us itemizers */
        TRANCANADAITEMIZERS TranCanadaItemizers;    /* Canadian itemizers */
        TRANINTLITEMIZERS   TranIntlItemizers;      /* Int'l itemizers, V3.3 */
    } ITEMIZERS;


/*--------------------------------------------------------------------------
*       transaction itemizers (us/Canada)
--------------------------------------------------------------------------*/

    typedef struct {
        DCURRENCY      lMI;                        /* main itemizer */
        DCURRENCY      lDiscountable[STD_DISC_ITEMIZERS_MAX];           /* discountable itemizer *///Database Changes 3-29-04 JHHJ
        DCURRENCY      lHourly;                    /* hourly itemizer */
        SHORT          sItemCounter;               /* item counter */
        DCURRENCY      lService[STD_MAX_SERVICE_TOTAL];  /* service total itemizer */
        DCURRENCY      lHourlyBonus[STD_NO_BONUS_TTL];   /* bonus total of hourly, V3.3 */
        DCURRENCY      lTransBonus[STD_NO_BONUS_TTL];    /* bonus total of transaction for GOV PAY and others */
        DCURRENCY      lTenderMoney[STD_TENDER_MAX];  /* tender on hand itemizer, Saratoga */
        DCURRENCY      lFCMoney[STD_NO_FOREIGN_TTL];  /* FC on hand itemizer, Saratoga STD_NO_FOREIGN_TTL */
        ITEMIZERS      Itemizers;                  /* us/Canada unique itemizers */
        DCURRENCY      lVoidDiscount[STD_DISC_ITEMIZERS_MAX];			/* TAR191625 */
		BOOL		   blDblShrdPrinting;			// Used for double sided printing and shared printing   ***PDINU
    } TRANITEMIZERS;


/*--------------------------------------------------------------------------
*       transaction item storage header
--------------------------------------------------------------------------*/
    typedef struct {
		//Added the UCHAR flag for delay balance to show that there is a 
		//new UCHAR in the file. This structure is only used for size calculation JHHJ
		UCHAR			uchDelayBalanceFlag;/* 1: Delay Balance Mode */
        TrnFileSize     usFSize;            /* 2: file size */
        TrnFileSize     usActLen;           /* 3: actual length */
        TRANMODEQUAL    TranModeQual;       /* 4: mode qualifier */
        TRANCURQUAL     TranCurQual;        /* 5: current qualifier */
        TrnVliOffset    usVli;              /* 6: end of transaction data offset */
    } TRANITEMSTORAGEHEADER;

    typedef struct {
		//Added the UCHAR flag for delay balance to show that there is a 
		//new UCHAR in the file. This structure is only used for size calculation JHHJ
		UCHAR			uchDelayBalanceFlag;/* 1: Delay Balance Mode */
        TrnFileSize     usFSize;            /* 2: file size */
        TrnFileSize     usActLen;           /* 3: actual length */
        TRANMODEQUAL    TranModeQual;       /* 4: mode qualifier */
        TRANCURQUAL     TranCurQual;        /* 5: current qualifier */
    } TRANITEMSTORAGEHEADER_VLI;


/*--------------------------------------------------------------------------
*       transaction consolidation storage header
--------------------------------------------------------------------------*/
    typedef struct {
        TrnFileSize     usFSize;            /* 1: file size */
        TrnFileSize     usActLen;           /* 2: actual length */
        TRANGCFQUAL     TranGCFQual;        /* 3: GCF qualifier */
        TRANITEMIZERS   TranItemizers;      /* 4: transaction itemizers */
        TrnVliOffset    usVli;              /* 5: end of transaction data offset */
    } TRANCONSSTORAGEHEADER;

    typedef struct {
        TrnFileSize     usFSize;            /* 1: file size */
        TrnFileSize     usActLen;           /* 2: actual length */
        TRANGCFQUAL     TranGCFQual;        /* 3: GCF qualifier */
        TRANITEMIZERS   TranItemizers;      /* 4: transaction itemizers */
    } TRANCONSSTORAGEHEADER_VLI;

// TOTALTUM totals update transaction file entry header used
// to contain the transaction environment data used when processing
// transaction data and updating the totals.
	typedef struct {
		UCHAR				TtlTranDBStatus;
		USHORT              TtlTranVli;         /* total transaction data storage size */
		TRANMODEQUAL        TtlTranModeQual;    /* Mode Qualifier */
		TRANCURQUAL         TtlTranCurQual;     /* Current Qualifier */
	} TTLTUMTRANQUAL;

/*--------------------------------------------------------------------------
*       transaction post receipt storage header
--------------------------------------------------------------------------*/
    typedef struct {
        TrnFileSize     usFSize;            /* 2: file size */
        TRANGCFQUAL     TranGCFQual;        /* GCF qualifier */
        TRANCURQUAL     TranCurQual;        /* current qualifier */
        TrnVliOffset    usVli;              /* 6: end of transaction data offset */
    } TRANPOSTRECSTORAGEHEADER;

    typedef struct {
        TrnFileSize     usFSize;            /* 2: file size */
        TRANGCFQUAL     TranGCFQual;        /* GCF qualifier */
        TRANCURQUAL     TranCurQual;        /* current qualifier */
    } TRANPOSTRECSTORAGEHEADER_VLI;

/*--------------------------------------------------------------------------
*       transaction information main
--------------------------------------------------------------------------*/

    typedef struct {
        USHORT          fsTransStatus;          /* Transaction Module Status, R3.1 */
        USHORT          usGuestNo[2];           /* GCF# on LCD for Split,  R3.1 */
        TRANMODEQUAL    TranModeQual;           /* terminal mode qualifier, state data such as Signed-in Cashier data  */
        TRANGCFQUAL     TranGCFQual;            /* Guest Check qualifier, data is saved along with the transaction data */
        TRANCURQUAL     TranCurQual;            /* current qualifier */
        TRANITEMIZERS   TranItemizers;          /* transaction itemizers */
        PifFileHandle   hsTranItemStorage;      /* item storage file handle */
        TrnFileSize     usTranItemStoFSize;     /* item storage file size */
        TrnVliOffset    usTranItemStoVli;       /* item storage Vli */
        PifFileHandle   hsTranConsStorage;      /* consoli storage file handle */
        TrnFileSize     usTranConsStoFSize;     /* consoli storage file size */
        TrnVliOffset    usTranConsStoVli;       /* consoli storage Vli */
        PifFileHandle   hsTranPostRecStorage;   /* post rec. storage file handle */
        PifFileHandle   hsTranItemIndex;        /* handle of item storage index */
        PifFileHandle   hsTranNoItemIndex;      /* handle of not cons. storage index */
        PifFileHandle   hsTranConsIndex;        /* handle of cons. storage index */
        PifFileHandle   hsTranNoConsIndex;      /* handle of not cons. storage index */
		PifFileHandle   hsTranTmpStorage;		/* handle of transaction storage*/
		ITEMTENDERINVNO invno;					// Unique Transacton Invoice Number used in EPT messages, paded for end of string.
		ITEMTENDERINVNO invnoLastTender;		// Unique Transacton Invoice Number used in last or previous EPT messages, paded for end of string.
		ITEMTENDER      tenderLastTender;       // last tender used in last or previous EPT message.
		TCHAR           tchCardHolder[NUM_CPRSPTEXT];       // NUM_CPRSPCO_CARDHOLDER card holder name for Electronic Payment
		ITEMDISC        TranChargeTips;         // contains the last charge tips.  used for Preauth Capture tender type.
		ULONG           ulCustomerSettingsFlags;  // Same as SYSCONFIG struct member ulCustomerSettingsFlags
    } TRANINFORMATION;


/*==========  Status of "TrnInformation.fsTransStatus"  ==========*/
// See also use of function TrnSplCheckSplit() to detect the TRN_SPL_aaa type in various places.
#define     TRN_STATUS_SEAT_TRANS       0x01    /* During Single Seat# Transaction, CLASS_SEAT_SINGLE, TRN_SPL_SEAT */
#define     TRN_STATUS_MULTI_TRANS      0x02    /* During Multi Seat# Transaction, CLASS_SEAT_MULTI, TRN_SPL_MULTI */
#define     TRN_STATUS_TYPE2_TRANS      0x04    /* During Type 2 Transaction, CLASS_SEAT_TYPE2, TRN_SPL_TYPE2 */


/*--------------------------------------------------------------------------
*       transaction information for split,      R3.1
--------------------------------------------------------------------------*/

    typedef struct {
        TRANGCFQUAL     TranGCFQual;            /* GCF qualifier */
        TRANITEMIZERS   TranItemizers;          /* transaction itemizers */
        PifFileHandle   hsTranConsStorage;      /* consoli storage file handle */
        TrnFileSize     usTranConsStoFSize;     /* consoli storage file size */
        TrnVliOffset    usTranConsStoVli;       /* consoli storage Vli */
        PifFileHandle   hsTranConsIndex;        /* handle of cons. storage index */
    } TRANINFSPLIT;


/*--------------------------------------------------------------------------
*       transaction information status,             R3.1
*       These flags are used with function TrnInitialize() and function
*       TrnInitializeSpl() to determine which of the memory resident data
*       areas containing the transaction state data are to be cleared.
--------------------------------------------------------------------------*/
#define     TRANI_MODEQUAL          0x0001      /* initialize mode qualifier */
#define     TRANI_GCFQUAL           0x0002      /* initialize GCF qualifier */
#define     TRANI_CURQUAL           0x0004      /* initialize cuurent qualifier  */
#define     TRANI_ITEMIZERS         0x0008      /* initialize itemizers */
#define     TRANI_ITEM              0x0010      /* initialize item storage */
#define     TRANI_CONSOLI           0x0020      /* initialize consoli storage */
#define     TRANI_SIGNINOUT         0x0040      /* initialize stuff for sign in or sign out */

#define     TRANI_GCFQUAL_SPLA      0x0100      /* Split A, GCF qualifier */
#define     TRANI_ITEMIZERS_SPLA    0x0200      /* Split A, itemizers */
#define     TRANI_CONSOLI_SPLA      0x0400      /* Split A, consoli storage */
#define     TRANI_GCFQUAL_SPLB      0x0800      /* Split B, GCF qualifier */
#define     TRANI_ITEMIZERS_SPLB    0x1000      /* Split B, itemizers */
#define     TRANI_CONSOLI_SPLB      0x2000      /* Split B, consoli storage */

// -----------------------------------------------------------------
//	modifiers for the table of value codes in UPC coupon.
//  See functions TrnStoCpnUPCSearch () and TrnQueryCouponUPCValue ()
//  in trncpn.c as well as the table aTrnUPC in trncpn.c to see how
//  these are used.
#define TRN_UPC_SPECIAL     (-1000)
#define TRN_CHECKER         (TRN_UPC_SPECIAL - 0)   /* checker intervention */
#define TRN_FREE            (TRN_UPC_SPECIAL - 1)   /* free marchandise */
#define TRN_BUY1_GET1       (TRN_UPC_SPECIAL - 2)   /* buy 1, get 1 free */
#define TRN_BUY2_GET1       (TRN_UPC_SPECIAL - 3)   /* buy 2, get 1 free */
#define TRN_BUY3_GET1       (TRN_UPC_SPECIAL - 4)   /* buy 3, get 1 free */
#define TRN_BUY4_GET1       (TRN_UPC_SPECIAL - 5)   /* buy 4, get 1 free */

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

/*--------------------------------------------------------------------------
*       prototype declaration
--------------------------------------------------------------------------*/

    SHORT   TrnOpen( VOID *TrnOpen );                           /* open */
    SHORT   TrnItem( VOID *TrnItem );                           /* itemize */
    SHORT   TrnClose( VOID *TrnClose );                         /* close */
    VOID    TrnModeIn( ITEMREGMODEIN *ItemRegModeIn );          /* reg mode-in */
    SHORT   TrnStorage(VOID *TrnStorage);                       /* Moved, R3.1 */
    VOID    TrnThrough( VOID *TrnItem );                        /* through */
	VOID    TrnSlipRelease (VOID);
    VOID    TrnThroughDisp( VOID *TrnItem );                    /* through for display on the flay R3.1 */
    SHORT   TrnGetGC( USHORT usGuestNo, SHORT sType, USHORT usType );/* get target GCF */
    SHORT   TrnQueryGC( USHORT usGuestNo );                     /* query target GCF */
    SHORT   TrnSaveGC( USHORT usType, USHORT usGuestNo );       /* save target GCF */
    SHORT   TrnConnEngineSendGC( USHORT usGuestNo );       /* save target GCF */
    SHORT   TrnCancelGC( USHORT usGuestNo, USHORT usType );     /* cancel target GCF */
    SHORT   TrnSendTotal( VOID );                               /* send target GCF */
    VOID    TrnInit( VOID );                                    /* initalize file handle value, V3.3 */
    VOID    TrnInitialize( USHORT usTraniType );                /* initialize transaction information */

    SHORT   TrnCouponSearch( ITEMCOUPONSEARCH *pItemSearch, SHORT sStorageType);        /* coupon search */
    SHORT   TrnSingleCouponSearch( ITEMCOUPONSEARCH *pItemSearch, SHORT sStorageType);  /* single coupon search */
	SHORT	TrnCouponSearchAll( ITEMCOUPONSEARCH *pItemSearch, SHORT sStorageType);

	TRANINFORMATION  * TrnGetTranInformationPointer (VOID);
    VOID    TrnGetTranInformation(TRANINFORMATION **TranInfo);
    VOID    TrnPutTranInformation(TRANINFORMATION *TranInfo);
	TRANMODEQUAL *TrnGetModeQualPtr(VOID);                      // get pointer to the current mode data area
    VOID    TrnGetModeQual( TRANMODEQUAL *pData );              /* get mode qualifier */
    VOID    TrnPutModeQual( CONST TRANMODEQUAL *pData );        /* put mode qualifier */
	TRANGCFQUAL *TrnGetGCFQualPtr(VOID);
    VOID    TrnGetGCFQual( TRANGCFQUAL **pData );               /* get GCF qualifier */
    VOID    TrnPutGCFQual( CONST TRANGCFQUAL *pData );          /* put GCF qualifier */
	TRANCURQUAL *TrnGetCurQualPtr(VOID);                        // get pointer to the current qualifier data area
	VOID    TrnGetCurQual( TRANCURQUAL *pData );                /* get current qualifier */
    VOID    TrnPutCurQual( CONST TRANCURQUAL *pData );          /* put current qualifier */
	VOID    TranCurQualTotalStatusClear (VOID);                  // clear or zero out the auchTotalStatus total key status of current qualifier.
	VOID    TranCurQualTotalStatusGet (UCHAR  *auchTotalStatus); // get or fetch a copy of the auchTotalStatus total key status of current qualifier.
	VOID    TranCurQualTotalStatusPut (CONST UCHAR  *auchTotalStatus); // put or update the auchTotalStatus total status of current qualifier with new total key data.
	BOOL    TranCurQualTotalStatusExist(VOID);                   // check to see if a Total Key has been pressed and there is a Total Key Status to process.
	ITEMDISC *TrnGetCurChargeTip(ITEMDISC **TranChargeTips);
	ITEMDISC *TrnClearCurChargeTip (ITEMDISC **TranChargeTips);

	TRANITEMIZERS  *TrnGetTIPtr (VOID);
    VOID    TrnGetTI(TRANITEMIZERS **pData);                    /* get transaction itemizers */
    VOID    TrnPutTI(CONST TRANITEMIZERS *pData);               /* put transaction itemizers */

	TrnVliOffset  TrnFetchLastStoConsOffset (VOID);
    SHORT   TrnCheckSize( VOID *pData, USHORT Data );           /* check storage size */
    TrnVliOffset  TrnGetStorageSize(TrnStorageType  sStorageType);              /* get storage size */
    SHORT   TrnCreateFile(USHORT usSize, UCHAR uchStorageType);        /* create item/cons./post rec. storage file */
    SHORT   TrnChkAndCreFile(USHORT usSize, UCHAR uchStorageType);     /* check and create item/cons./post rec. storage file */
    VOID    TrnChkAndDelFile(USHORT usSize, UCHAR uchStorageType);     /* check and delete item/cons./post rec. storage file */

//------------------------------------------------------------------------
// Testing with GenPOS Rel 2.2.1.145 with Table Service, Post Guest Check, to
// review the functionality we discovered that using Guest Check Transfer to
// combine a guest check with a second guest check was not working properly.
// We found an error with the memory caching functionality due to a defect
// in handling read of only part of a guest check.  Disabling this functionality
// addressed the problem.
// This memory caching functionality should probably just be removed as it
// has a defect and does not seem to provide any advantage.
//   Richard Chambers, Feb-06-2015

//#define TrnReadWriteFile_Memory

#if defined(TrnReadWriteFile_Memory)
    SHORT   TrnReadFile_Memory(ULONG ulOffset, VOID *pData,
                        ULONG ulSize, SHORT hsFileHandle,
						ULONG *pulActualBytesRead);

    SHORT   TrnWriteFile_Memory(ULONG ulOffset, VOID *pData,
                         ULONG ulSize, SHORT hsFileHandle);

	SHORT TrnReadFile_MemoryForce (ULONG ulOffset, VOID *pData,
                   ULONG ulSize, SHORT hsFileHandle,
				   ULONG *pulActualBytesRead);

#define TrnReadFile(ulOffset,pData,ulSize,hsFileHandle,pulActualBytesRead) TrnReadFile_Memory(ulOffset, pData, ulSize, hsFileHandle, pulActualBytesRead)
#define TrnWriteFile(ulOffset,pData,ulSize,hsFileHandle) TrnWriteFile_Memory(ulOffset, pData, ulSize, hsFileHandle)

#else

#if 0
    SHORT   TrnReadFile_Debug(ULONG ulOffset, VOID *pData,
                        ULONG ulSize, PifFileHandle hsFileHandle,
						ULONG *pulActualBytesRead, CONST char *pFileName, int iLineNo);
    SHORT   TrnWriteFile_Debug(ULONG ulOffset, VOID *pData,
                         ULONG ulSize, PifFileHandle hsFileHandle, CONST char *pFileName, int iLineNo);

#define TrnReadFile(ulOffset,pData,ulSize,hsFileHandle,pulActualBytesRead) TrnReadFile_Debug(ulOffset, pData, ulSize, hsFileHandle, pulActualBytesRead, __FILE__, __LINE__)
#define TrnWriteFile(ulOffset,pData,ulSize,hsFileHandle) TrnReadFile_Debug(ulOffset, pData, ulSize, hsFileHandle, __FILE__, __LINE__)

#define TrnReadFile_MemoryForce(ulOffset,pData,ulSize,hsFileHandle,pulActualBytesRead) TrnReadFile_Debug(ulOffset, pData, ulSize, hsFileHandle, pulActualBytesRead, __FILE__, __LINE__)

#else
    SHORT   TrnReadFile(ULONG ulOffset, VOID *pData,
                        ULONG ulSize, PifFileHandle hsFileHandle, ULONG *pulActualBytesRead);
    SHORT   TrnWriteFile(ULONG ulOffset, VOID *pData, ULONG ulSize, PifFileHandle hsFileHandle);

#define TrnReadFile_MemoryForce(ulOffset,pData,ulSize,hsFileHandle,pulActualBytesRead) TrnReadFile(ulOffset, pData, ulSize, hsFileHandle, pulActualBytesRead)

#endif
#endif
    VOID    TrnStoGetConsToPostR(VOID);
    SHORT   TrnStoPutPostRToCons(VOID);                     /* V3.3 */
    VOID    TrnPutInfGuestSpl(UCHAR uchMiddle, UCHAR uchRight);
    SHORT   TrnICPOpenFile(VOID);
    VOID    TrnICPCloseFile(VOID);
    ULONG   TrnCalStoSize(ULONG ulNoOfItem, UCHAR uchType);
    USHORT  TrnConvertError(SHORT sError);

    VOID    TrnSetConsNoToPostR( USHORT usConsecutiveNo );

    VOID    TrnPrintType( VOID *pItem );                /* V3.3 */

    /*----- R3.1 -----*/
	SHORT   TrnIsSameZipDataSalesNon (VOID *pSalesNon, VOID *pData);
	SHORT   TrnIsSameZipDataCoupon (VOID *pCouponNon, VOID *pData);
    SHORT   TrnVoidSearch(VOID *pData, SHORT sQTY, SHORT sStorage);
    VOID    TrnVoidSearchRedSalesMod(ITEMSALES *pItem);
    SHORT   TrnTransferSearch(VOID *pData, SHORT sQTY, SHORT sStorage);
    SHORT   TrnTransferSearchSales(ITEMSALES *pData, SHORT sQTY, SHORT sStorage);
    SHORT   TrnTransferSearchSalesMod(ITEMSALES *pData, SHORT sStorage);
    SHORT   TrnTransferSearchDisc(ITEMDISC *pData, SHORT sStorage);
    SHORT   TrnTransferSearchCoupon(ITEMCOUPON *pData, SHORT sStorage);
    SHORT   TrnTransferSearchSalesDisc(ITEMSALES *pSales, ITEMDISC *pDisc, SHORT sStorage);
    VOID    TrnSplReduceItemizerSales(ITEMSALES *pItem);
    VOID    TrnSplReduceItemizerDisc(ITEMDISC *pItem);
    VOID    TrnSplReduceStorageSalesDisc(ITEMSALES *pSales, ITEMDISC *pDisc);

    SHORT   TrnSplCheckSplit(VOID);
    VOID    TrnSplCheck(VOID);
    SHORT   TrnSplGetSeatTrans(UCHAR uchSeat, SHORT sUpdate, UCHAR uchStoType, TrnStorageType  sStorageType);

    SHORT   TrnSplGetSeatTransCheck(UCHAR uchSeat);
    USHORT  TrnSplGetDispSeat(TRANINFORMATION **TrnInf, TRANINFSPLIT **SplitA, TRANINFSPLIT **SplitB);
    SHORT   TrnSplRightArrow(VOID);
    SHORT   TrnSplLeftArrow(VOID);
    SHORT   TrnSplCheckFinSeatTrans(VOID);
    VOID    TrnSplPrintSeat(UCHAR uchSeat, SHORT sUpdate, USHORT usTrail, USHORT usAllPrint,
                UCHAR auchTotalStatus[], UCHAR uchPrintType);
    VOID    TrnSplPrintBase(USHORT usTrail, USHORT usAllPrint, UCHAR auchTotalStatus[]);
    VOID    TrnSplAddSeatTrans(UCHAR uchSeat);
    SHORT   TrnSplGetBaseTrans(SHORT sUpdate, UCHAR uchStoType);
    SHORT   TrnSplGetBaseTransSeat(UCHAR uchStoType);
    VOID    TrnSplCancel3Disp(VOID);
    VOID    TrnSplCancel3DispSplit(VOID);
    SHORT   TrnSplSearchUseSeatQueue(UCHAR uchSeat);
    SHORT   TrnSplSearchFinSeatQueue(UCHAR uchSeat);
    SHORT   TrnSplAddGC2GC(VOID);
    VOID    TrnSplAddGC2GCGCFQual(TRANGCFQUAL *puchBuffer);
    VOID    TrnSplAddGC2GCItemizers(TRANITEMIZERS *pItem2);

    SHORT   TrnOpenSpl(VOID *ItemOpen, TrnStorageType  sStorageType);
    SHORT   TrnItemSpl(VOID *pItem, TrnStorageType  sStorageType);
    VOID    TrnThroughSpl(VOID *TrnItem);
    VOID    TrnInitializeSpl(USHORT usTraniType);
    SHORT   TrnCreateFileSpl(USHORT usNoOfItem);
    SHORT   TrnChkAndCreFileSpl(USHORT usNoOfItem);
    VOID    TrnChkAndDelFileSpl(USHORT usNoOfItem);
    SHORT   TrnOpenFileSpl(VOID);
    VOID    TrnCloseFileSpl(VOID);

	TRANGCFQUAL *TrnGetGCFQualSplPointer(CONST SHORT sType);
	VOID    TrnGetGCFQualSpl(TRANGCFQUAL **TranGCFQual, CONST SHORT sType);

	TRANITEMIZERS * TrnGetTISplPointer(CONST SHORT sType);
    VOID    TrnGetTISpl(TRANITEMIZERS **pItem, CONST SHORT sType);

    TRANINFSPLIT *TrnGetInfSpl(TrnStorageType  sStorageType);

    SHORT   TrnLoanPickup(VOID *pData);                         /* Saratoga */
    SHORT TrnQueryCouponUPCValue(ITEMCOUPONSEARCH *pItemCpnSearch);

	USHORT   TrnPrtGetSlipPageLine(VOID);
	VOID   TrnPrtSetSlipPageLine(USHORT usLine);

	SHORT  TrnDisplayFetchGuestCheck (TCHAR *pLeadThruMnemonic, ULONG ulCashierId);
	SHORT TrnFetchServicedGuestCheck (VOID *pKeyMsg);          // (KEYMSG *)pKeyMsg

	extern USHORT          semStoreForward;     /* Semapho, Store and Forward */

	// See https://stackoverflow.com/questions/4592762/difference-between-const-const-volatile
	extern CONST  volatile TRANINFORMATION  * CONST TrnInformationPtr;
	extern CONST  volatile TRANMODEQUAL     * CONST TranModeQualPtr;
	extern CONST  volatile TRANGCFQUAL      * CONST TranGCFQualPtr;
	extern CONST  volatile TRANCURQUAL      * CONST TranCurQualPtr;
	extern CONST  volatile TRANITEMIZERS    * CONST TranItemizersPtr;

	// externs for split checks functionality with Pre-Guest Check Buffering
	// allows access to either Split A or Split B transaction data structures.
	extern CONST volatile TRANINFSPLIT * CONST TrnInfSplPtrSplitA;
	extern CONST volatile TRANINFSPLIT * CONST TrnInfSplPtrSplitB;

	// externs for split checks functionality with Pre-Guest Check Buffering
	// allows access to either Split A or Split B itemizer data structures.
	extern CONST volatile TRANITEMIZERS * CONST TrnTISplPtrSplitA;
	extern CONST volatile TRANITEMIZERS * CONST TrnTISplPtrSplitB;

	// externs for split checks functionality with Pre-Guest Check Buffering
	// allows access to either Split A or Split B GCF Qualifier data structures.
	extern CONST volatile TRANGCFQUAL * CONST TrnGCFQualSplPtrSplitA;
	extern CONST volatile TRANGCFQUAL * CONST TrnGCFQualSplPtrSplitB;

#endif

/* ===== End of File (TRANSACT.H) ===== */
