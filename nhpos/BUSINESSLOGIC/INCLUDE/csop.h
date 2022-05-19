/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1999      ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                Release 3.1              ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server OPERATIONAL PARAMETER module Header
*   Category           : Client/Server OPERATIONAL PARAMETER module
*   Program Name       : CSOP.H  (Header file for user)
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*   Update Histories   :
*
*   Date      : Ver.Rev  :NAME       :Description
*   Nov-03-93 : 00.00.01 :H.YAMAGUCHI:Increase PLU 1000 --> 3000
*   Feb-06-95 : 03.00.00 :h.ishida   :Adds uchTableNumber of PLUIF
*             :          :           :Adds uchClassNumber of PLUIF
*   Mar-01-95 : 03.00.00 :H.Ishida   :Chg. from fsCouponStatu to uchCouponStatus
*   Mar-14-95 : 03.00.00 :T.Satoh    :Adds uchCouponAdj[] of COMCOPONPARA
*   Mar-31-95 : 03.00.00 :H.Ishida   :Adds Control String
*   Nov-29-95 : 03.01.00 :T.Nakahata :R3.1 Initial
*       Add New File (Promotional Pricing Item)
*       Add Linked PPI Code at PLU Record
*       Increase 1 Byte at Dept/PLU Control Code (Send KP #5-8)
*   Mar-14-96 : 03.01.00 :M.Ozawa    :Change define of MAX_OP_WORK_BUFFER
*   Aug-17-98 : 03.03.00 :M.Ozawa    :Add MLD Mnemonics File
*
** NCR2171 **
*
*   Oct-05-99 : 01.00.00 :M.Teraki   : initial (for 2171)
*   Dec-14-99 : 01.00.00 :hrkato     : Saratoga
*
** GENPOS **
*
*   Jun-23-16 : 02.02.01 :R.Chambers : add Sales Code restriction to COMCOUPONPARA.
\*=======================================================================*/
/*=======================================================================*\
:   PVCS ENTRY
:-------------------------------------------------------------------------
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

#if !defined(CSOP_H_INCLUDED)

#define CSOP_H_INCLUDED

#include "paraequ.h"

/*
===========================================================================
    DEFINE
===========================================================================
*/

#define    OP_PLU_LEN              STD_PLU_NUMBER_LEN
#define    OP_PLU_OEP_LEN          (STD_PLU_NUMBER_LEN+3)  // used with OEP file to hold 3 digit priority plus PLU number
#define    OP_PLU_NAME_SIZE        STD_PLU_MNEMONIC_LEN   /* Saratoga */
#define    OP_PLU_PRICE_SIZE         3
#define    OP_PLU_CONT_OTHER        MAX_PLUSTS_SIZE    // changed 05/13/00, Saratoga same as MAX_PLUSTS_SIZE
#define    OP_DEPT_CONT_CODE_SIZE    5    // size of Departmental settings
#define    OP_PLU_SHARED_STATUS_LEN  4    // Number of bytes of shared PLU status provisioning data
#define    OP_PLU_PLU_SHARED_STATUS_LEN  5    // Number of bytes of shared PLU status provisioning and print data
#define    OP_PLU_OPTIONS_STATUS_LEN     4    // additional options from PLU status provisioning

#define    OP_DEPT_NAME_SIZE       20   /* 2172 */
#define    OP_DEPT_HALO_SIZE        1
#define    OP_CPN_STS_SIZE          2
#define    OP_MAX_WORK_BUFFER    1024   /* saratoga */
#define    OP_MAX_WORK2_BUFFER   1500
#define    OP_NUMBER_OF_MAX_PLU  100000 /* Saratoga */

/* Add 2172 Rel 1.0 */
#define    OP_NUMBER_OF_MAX_OEP  8000
#define    OP_MAX_DEPT_NO        9999
#define    OP_MAX_REPORT_NO        10
#define    OP_MAX_GROUP_NO         99
#define    OP_CSTR_INDEX_SIZE     400 //ESMITH CSTR
#define    OP_CSTR_PARA_SIZE      200

/* === New Elements - Promotional Pricing Item (Release 3.1) BEGIN === */
#define    OP_MAX_PPI_NO          300   /* Promotional Pricing Item, ESMITH, must be same value as FLEX_PPI_MAX */
#define    OP_MAX_PPI_LEVEL        40   /* Price Level, must be same value as MAX_PARAPPI_SIZE     R3.1 */ /*20->40 for PPI Enhancement - CSMALL */
/* === New Elements - Promotional Pricing Item (Release 3.1) END === */
#define    OP_MLD_MNEMONICS_LEN    STD_MLD_MNEMONICS_LEN   /* Mld Mnemonis Lenggh,      V3.3 */

#define    OP_OEP_PLU_SIZE         STD_OEP_MAX_NUM   /* 2172 */ /* set to STD_OEP_MAX_NUM - CSMALL */
/*
===========================================================================
   DEFINE FOR ERROR CODE
===========================================================================
*/

/* External error code for user interface */

#define  OP_PERFORM             0

#define  OP_FILE_NOT_FOUND     (-1)
#define  OP_PLU_FULL           (-2)
#define  OP_FILE_FULL          OP_PLU_FULL
#define  OP_DEPT_FULL          OP_PLU_FULL
/* Add 2172 Rel 1.0 */
#define  OP_OEP_FULL           OP_PLU_FULL
#define  OP_NOT_IN_FILE        (-3)
#define  OP_LOCK               (-5)
#define  OP_NO_MAKE_FILE       (-6)
#define  OP_ABNORMAL_DEPT      (-7)
#define  OP_ABNORMAL_CPN       (-8)
#define  OP_ABNORMAL_CSTR      (-9)

#define  OP_CHANGE_PLU         (-10)
#define  OP_NEW_PLU            (-11)
#define  OP_CHANGE_DEPT        OP_CHANGE_PLU
#define  OP_NEW_DEPT           OP_NEW_PLU
/* Add 2172 Rel 1.0 */
#define  OP_CHANGE_OEP         OP_CHANGE_PLU
/* Add 2172 Rel 1.0 */
#define  OP_NEW_OEP            OP_NEW_PLU
#define  OP_DELETE_CSTR_FILE   (-16)
#define  OP_DELETE_PLU_FILE    (-17)
#define  OP_DELETE_DEPT_FILE   (-18)
#define  OP_DELETE_CPN_FILE    (-19)
/* === New Functions - Promotional Pricing Item (Release 3.1) BEGIN === */
#define  OP_CHANGE_PPI         OP_CHANGE_PLU    /* change PPI Record.  R3.1 */
#define  OP_NEW_PPI            OP_NEW_PLU       /* Add new PPI Record, R3.1 */
#define  OP_DELETE_PPI_FILE    (-15)              /* delete PPI File,    R3.1 */
#define  OP_ABNORMAL_PPI       (-16)              /* ppi code is bad,    R3.1 */
/* === New Functions - Promotional Pricing Item (Release 3.1) END === */
#define  OP_CHANGE_MLD         OP_CHANGE_PLU    /* change Mld Record.  V3.3 */
#define  OP_NEW_MLD            OP_NEW_PLU       /* Add new Mld Record, V3.3 */
#define  OP_DELETE_MLD_FILE    (-21)              /* delete Mld File,    V3.3 */
#define  OP_ABNORMAL_MLD       (-22)              /* Mld code is bad,    V3.3 */

#define  OP_NOT_MASTER         (-20)
/* Add 2172 Rel1.0 */
#define  OP_PLU_ERROR          (-23)
#define  OP_DELETE_OEP_FILE    (-24)            /* delete OEP File,    saratoga */
#define  OP_TEST			   (-25)			// Used for Testing (when data fields recieved as 0)   ***PDINU

/* Internal error code  */

#define  OP_NO_READ_SIZE       (-31)
#define  OP_HEAD_OF_TABLE      (-32)
#define  OP_TAIL_OF_TABLE      (-33)
#define  OP_DONT_HIT           (-34)
#define  OP_EOF                (-35)
#define  OP_SEND_ERROR         (-36)
#define  OP_COMM_ERROR         (-37)
#define  OP_CONT               (-38)

// Following are used to transform Network Layer (STUB_) errors into OP_ errors
// Notice that error code is same ast STUB_ plus STUB_RETCODE.
#define  OP_M_DOWN_ERROR        (STUB_RETCODE+STUB_M_DOWN)
#define  OP_BM_DOWN_ERROR       (STUB_RETCODE+STUB_BM_DOWN)
#define  OP_BUSY_ERROR          (STUB_RETCODE+STUB_BUSY)
#define  OP_TIME_OUT_ERROR      (STUB_RETCODE+STUB_TIME_OUT)
#define  OP_UNMATCH_TRNO        (STUB_RETCODE+STUB_UNMATCH_TRNO)
#define  OP_DUR_INQUIRY         (STUB_RETCODE+STUB_DUR_INQUIRY)

#define  OP_PLU_STATUS_LEN      10


/*
===========================================================================
    TYPEDEF
===========================================================================
*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

typedef struct {
	LONG lAmount;
	SHORT sCounter;
}INV_TOTAL; //SR167 - CSMALL

/*
	Control code conversions.

	The control codes contain settings that indicate such provisioning
	information as which taxable itemizer to use, which adjective group to
	use, which remote device to use, etc.

	These provisioned settings are actually stored in several different
	data structures.  The variables holding the provisioning data are different
	sizes depending on the particular struct and its purpose.

		ITEMSALES -> ITEMCONTCODE -> UCHAR [ITM_PLU_CONT_OTHER]
			e.g. ItemSales.ControlCode.auchStatus
			contains item data for an item that is rung up
		DEPTIF -> OPDEPT_PARAENTRY -> UCHAR  [OP_DEPT_CONT_CODE_SIZE]
			e.g. DeptRecRcvBuffer.ParaDept.auchControlCode
			contains the department data for a particular department (AC114)
		PLUIF -> OPPLU_PARAENTRY -> PLU_CONTROL -> UCHAR  [OP_PLU_CONT_OTHER]
			e.g. PLURecRcvBuffer.ParaPlu.ContPlu.auchContOther
			contains PLU data for a particular PLU (AC63/64/68/82)
		MAINTWORK -> PARAPLU -> UCHAR  [MAX_PLUSTS_SIZE]
			e.g. MaintWork.PLU.auchPLUStatus

	The first four bytes of the above structs have the same format implying that when
	transfering provisioning data from one to the other a simple memcpy () can be
	used to transfer the information.  We use OP_PLU_SHARED_STATUS_LEN for this length.

	However beginning with the fifth byte (byte five if one based or byte four if zero based)
	the above structs begin to vary.

	The PLURecRcvBuffer.ParaPlu.ContPlu.auchContOther[4] contains print control data for
	PLU adjectives.  See RptPLUPrintChk2().  This byte has the same usage and format as
	does ItemSales.ControlCode.auchStatus[4].  We use OP_PLU_PLU_SHARED_STATUS_LEN for this length.

	The ItemSales.ControlCode.auchStatus[5] is used for state information as part of peforming the item processing.
	The settings in this byte of the status comes from several different MDC settings and are not contained
	in the PLU record:
	  - MDC_DEPT1_ADR    MDC 27 settings, "Department/PLU Item 1" in PEP.
	  - MDC_SCALE_ADR    MDC 29 settings, "Scale/Tare (Scale)" in PEP
	  - MDC_MODID23_ADR  MDC 37 settings, "Item Disc./Surcharge 2 key"

	PLURecRcvBuffer.ParaPlu.ContPlu.auchContOther[5], DeptRecRcvBuffer.ParaDept.auchControlCode[4],
	and ItemSales.ControlCode.auchStatus[6] have the same format and contain the same data.  This byte
	is used to contain indicators for Remote CRTs 5 through 8. 

	PLURecRcvBuffer.ParaPlu.ContPlu.auchContOther[6] and ItemSales.ControlCode.auchStatus[7] have the
	same format and contain the same data.  This byte is used to contain indicators for Gift Card and
	other indicators for quantity restrictions. 

	MaintWork.PLU.auchPLUStatus is the same size and usage as PLURecRcvBuffer.ParaPlu.ContPlu.auchContOther.

**/
typedef  struct {
    USHORT  usDept;          /* for temporary */
    UCHAR   uchItemType;
    UCHAR   uchRept;
    UCHAR   auchContOther[OP_PLU_CONT_OTHER];   // see notes with function ItemSalesItemPluTypeInfo().
	USHORT	usBonusIndex;
	// 2.1 Release Changes, SR167 - CSMALL
	UCHAR	auchContOther2[20]; //New Status Fields[20]:
								//	Reference remote kitchen devices 9-16
								//	Wether this PLU requires reason code
								//	Allows PLU to call a control String (3 digit value that calls a Cntrl String)
								//	Indicate Inventory Item Reference Number
								//	Indicate wether PLU has serial number
}PLU_CONTROL;

typedef    struct {
    PLU_CONTROL    ContPlu;
    WCHAR   auchPluName[STD_PLU_MNEMONIC_LEN];
    UCHAR   auchPrice[OP_PLU_PRICE_SIZE];
    UCHAR   uchTableNumber;
    UCHAR   uchGroupNumber;
    UCHAR   uchPrintPriority;
    /* === New Elements (Release 3.1) BEGIN === */
    USHORT   uchLinkPPI;         /* Link PPI Code,  ESMITH */
    /* === New Elements (Release 3.1) END === */
    UCHAR   uchPriceMulWeight;
    UCHAR   uchRestrict;
    USHORT  usLinkNo;
    USHORT  usFamilyCode;   /* add 05/13/00 */
	// 2.1 Release Changes, SR167 - CSMALL
	ULONG	ulVoidTotalCounter;				// Void Total/Counter for each PLU
	ULONG	ulDiscountTotalCounter;			// Discount Total/Counter for each PLU
	ULONG	ulFreqShopperPoints;			// Frequent Shopper Discount/Points (GP Feature)
	UCHAR	auchAlphaNumRouting[4];			// Alphanumeric routing character for sending to KDS or Printers
	INV_TOTAL	futureTotals[10];			// basic inventory totals (on-hand, receipts/transfers, begin count,
											//   reorder count, current cost, original cost, etc.)
	WCHAR	auchAltPluName[STD_PLU_MNEMONIC_LEN + 1];	//	Multilanguage mnemonic for each PLU
	UCHAR	uchColorPaletteCode;				// Color pallete code for PLU
	UCHAR	auchStatus2[OP_PLU_STATUS_LEN];
	USHORT  usExtraSpace;
	USHORT  usTareInformation;
	UCHAR   uchExtendedGroupPriority[12];      // extended group numbers, first byte is group number, second byte is priority within group
	UCHAR   uchPluOptionFlags[6];                 // bit map for option flags
}OPPLU_PARAENTRY;

/* 2172 Flexible Dept to which PLUs are assigned, see AC114 Department Maintenance */
typedef  struct {
    UCHAR   uchMdept;             // Major department number, 1 - 30, this department assigned to. see P48 for mnemonics.
    UCHAR   auchControlCode[OP_DEPT_CONT_CODE_SIZE];  // see notes with function ItemSalesItemPluTypeInfo().
    UCHAR   auchHalo[OP_DEPT_HALO_SIZE];
    WCHAR   auchMnemonic[OP_DEPT_NAME_SIZE];
    UCHAR   uchPrintPriority;
	USHORT	usBonusIndex;
	USHORT  usTareInformation;		
	UCHAR   uchExtraRoom[8];      // extra space that can be used for added functionality
}OPDEPT_PARAENTRY;

/* coupon status ( uchCouponStatus[0] ) */
/* bit 7 6 5 4 3 2 1 0
       | | | | | | | +----- affect US taxable itemizer #1
       | | | | | | +----- affect US taxable itemizer #2
       | | | | | +------- affect US taxable itemizer #3
       | | | | +-------- affect US food stamp itemizer - CPN_STATUS0_FOOD_MOD
       | | | +-------- coupon is an Auto Coupon - CPN_STATUS0_AUTOCOUPON
       | | +--------- single use in transaction - CPN_STATUS0_SINGLE_TYPE
       | +--------- affect discountable itemizer #1 - CPN_STATUS0_AFFECT_DISC1
       +-------- affect discountable itemizer #2 - CPN_STATUS0_AFFECT_DISC2
*/

//=========================================================================
//
//   Coupon record status bit constants

//  COMCOUPONPARA.uchCouponStatus[0] bit fields
//     see also MOD_USTAXMOD and MOD_CANTAXMOD in itemlocal.h
//
// ParaPlu.ContPlu.auchContOther[1] is assigned to ITEMCOUPONSEARCH.auchStatus[0] in TrnStoCpnUPCSearch().
// Use the PLU_AFFECT_ type defines for testing tax itemizers affected, food stamp, etc.
//
// Canadian tax type index is a 4 bit value from 0 to 9 indicating which of the Canadian tax types
// (meal, carbonated beverage, etc.) is to be applied with the coupon. use mask MOD_CANTAXMOD to isolate
// the 4 bit value as in  canTaxIndex = (WorkCoupon.ParaCpn.uchCouponStatus[0] & MOD_CANTAXMOD;
#define CPN_STATUS0_USTAXABLE1    PLU_AFFECT_TAX1   // indicates coupon affects US taxable itemizer #1 or Canadian tax type index bit 0 or VAT Int'l tax index bit 0
#define CPN_STATUS0_USTAXABLE2    PLU_AFFECT_TAX2   // indicates coupon affects US taxable itemizer #2 or Canadian tax type index bit 1 or VAT Int'l tax index bit 1
#define CPN_STATUS0_USTAXABLE3    PLU_AFFECT_TAX3   // indicates coupon affects US taxable itemizer #3 or Canadian tax type index bit 2
#define CPN_STATUS0_FOOD_MOD      PLU_AFFECT_FS     // indicates coupon affects US Food Stamp itemizer or Canadian tax type index bit 3 or Affect VAT Int'l Servicable Itemizer
#define CPN_STATUS0_AUTOCOUPON    0x10              // indicates coupon is an Auto coupon
#define CPN_STATUS0_SINGLE_TYPE   0x20              // indicates coupon is a single use type of coupon
#define CPN_STATUS0_AFFECT_DISC1  PLU_AFFECT_DISC1  // indicates coupon affects Discountable Itemizer #1
#define CPN_STATUS0_AFFECT_DISC2  PLU_AFFECT_DISC2  // indicates coupon affects Discountable Itemizer #2
//#define CPN_STATUS0_USTAXABLE4    PLU_AFFECT_TAX4   // indicates coupon affects US taxable itemizer #4
//#define CPN_STATUS0_USTAXABLE5    PLU_AFFECT_TAX5   // indicates coupon affects US taxable itemizer #5

// uchCouponStatus[1] contains department flags for the STD_MAX_NUM_PLUDEPT_CPN possible PLU/Dept entries.
//   0x01 is first, 0x02 is second, 0x04 is third, 0x08 is fourth, 0x10 is fifth, 0x20 is sixth, 0x40 is seventh.
#define CPN_AUTO_PRI_MAX	  25 // Highest priority number for Auto Coupon from AC161

// See also struct ITEMCOUPONSALES in struct ITEMCOUPONSEARCH and usage with applying coupons, ItemCouponGetTable().

typedef  struct {
    UCHAR   uchCouponStatus[OP_CPN_STS_SIZE];   // coupon status such as taxable itemizers affected, etc.
	UCHAR   uchAutoCPNStatus;                   // range 1 to CPN_AUTO_PRI_MAX
    ULONG   ulCouponAmount;
    WCHAR   aszCouponMnem[STD_CPNNAME_LEN];
    WCHAR   auchCouponTarget[STD_MAX_NUM_PLUDEPT_CPN][OP_PLU_LEN];  // PLU Number or Department Number. type indicated by uchCouponStatus[1].
    UCHAR   uchCouponAdj[STD_MAX_NUM_PLUDEPT_CPN];    // PLU Adj # (0 - 5) lower nibble, Group # (0, 1, 2) in upper nibble.
	USHORT	usBonusIndex;
	UCHAR   uchRestrict;          // Sales Code restriction value 0 - 7. high bit 0x80 indicates if Supr Intv allowed.
	UCHAR   uchExtraRoom[7];      // extra space that can be used for added functionality
}COMCOUPONPARA;

/* === New Record - Promotional Pricing Item (Release 3.1) BEGIN === */
typedef struct {
    UCHAR   uchQTY;     /* Price Multiple */
    ULONG   ulPrice;    /* Promotional Price */
} PPI_RECORD;           // must have same structure as PPIPARAENTRY

// Following flags are used with ulPpiLogicFlags01 to indicate
// business rule changes and how the data in the PPI_RECORD is treated.
#define PPI_LOGICFLAGS01_PLUDISCOUNT    0x00000002   // ulPrice is a discount percentage nn.n% on PLU price
#define PPI_LOGICFLAGS01_PLUPREMIUM     0x00000004   // ulPrice is a premium percentage nn.n% on PLU price
#define PPI_LOGICFLAGS01_COMBINENOT     0x00000008   // do not combine separate PLUs using same PPI table

typedef struct {
    PPI_RECORD  PpiRec[MAX_PARAPPI_SIZE];  /* PPI Record */
	// PPI Enhancments - CSMALL 6/7/2006
	//After Max QTY reached, 0:Continue Pattern of PP Discount
	//						 1:Revert Additional QTY sales to normal price
	UCHAR	ucAfterMaxReached;	
	//Before Initial QTY reach, 0:Use PPI price
	//							1:Use Default PLU price
	UCHAR	ucBeforeInitialQty;
	//Min. Total Sale Amount, 0:Not Required for PPI discount/pricing
	//						  #:Required for PPI discount/pricing (# = Total Sales Amount required)
	ULONG	ulMinTotalSale;
	ULONG   ulPpiLogicFlags01;	// Flags to indicate business logic enhancements with PPI_LOGICFLAGS01_ above
	UCHAR	ucExtra1[6];	// extra space for future use
	UCHAR	ucExtra2[10];	// extra space for future use
	UCHAR	ucExtra3[10];	// extra space for future use
} OPPPI_PARAENTRY;

typedef struct {
    USHORT           uchPpiCode; //ESMITH
    OPPPI_PARAENTRY ParaPpi;
} PPIIF;
/* === New Record - Promotional Pricing Item (Release 3.1) END === */

typedef    struct {
    WCHAR   auchPluNo[OP_PLU_LEN];      /* for temporary */
    UCHAR   uchPluAdj;
    ULONG   ulCounter;                  // contains the iteration control counter, initialize to zero to start.
    USHORT  husHandle;  /* 2172 */
    USHORT  usAdjCo;    /* 2172 */
    OPPLU_PARAENTRY    ParaPlu;
}PLUIF;

/* 2172 */
typedef    struct {
    USHORT   usDeptNo;
    ULONG   ulCounter;
    OPDEPT_PARAENTRY    ParaDept;
}DEPTIF;

typedef    struct {
     USHORT    uchCpnNo;  //UCHAR to USHORT coupon max 100->300 JHHJ
    COMCOUPONPARA ParaCpn;
}CPNIF;

typedef  struct {
   USHORT   usCstrOffset;
   USHORT   usCstrLength;
} OPCSTR_INDEXENTRY;

typedef  struct {
   UCHAR    uchMajorCstrData;
   UCHAR    uchMinorCstrData;
} OPCSTR_PARAENTRY;

typedef    struct {
    USHORT    usCstrNo;
    OPCSTR_INDEXENTRY  IndexCstr;
    OPCSTR_PARAENTRY    ParaCstr[OP_CSTR_PARA_SIZE];
}CSTRIF;

// following struct used to retrieve department data for PLU Department. may be used to iterate over all
// departments. see function OpMajorDeptRead().
typedef    struct {
    UCHAR    uchMajorDeptNo;        // major department number, 1 - 30, or 0xff to mean all major departments when iterating.
    OPDEPT_PARAENTRY    ParaDept;   // AC114 Department Maintenance data
    USHORT    usDeptNo;             // PLU department number, 1 - 9999. see AC114 Department Maintence and AC68 PLU Maintenance.
    ULONG    ulCounter;             // counter used for iterating. initialize to zero to start.
}MDEPTIF;

typedef    struct {
    UCHAR    uchReport;                 // contains PLU Report Code to be matched
    ULONG    culCounter;                // contains the iteration control counter, initialize to zero to start.
    WCHAR   auchPluNo[OP_PLU_LEN];      /* for temporary */
    UCHAR    uchPluAdj;
    USHORT   usAdjCo;   /* 2172 */
    USHORT   husHandle; /* 2172 */
    OPPLU_PARAENTRY    ParaPlu;
} PLUIF_REP;

typedef    struct {
    USHORT   usDept;                    // contains PLU Department Number to be matched
    ULONG    culCounter;                // contains the iteration control counter, initialize to zero to start.
    WCHAR   auchPluNo[OP_PLU_LEN];      /* for temporary */
    UCHAR    uchPluAdj;
    USHORT   usAdjCo;   /* 2172 */
    USHORT   husHandle; /* 2172 */
    OPPLU_PARAENTRY    ParaPlu;
} PLUIF_DEP;

typedef    struct {
    WCHAR    auchPluNo[OP_PLU_LEN];     /* for temporary */
    UCHAR    uchPluAdj;
    WCHAR    uchPluName[STD_PLU_MNEMONIC_LEN];
} PLU_PARA;

typedef    struct {
    USHORT   usGroupNo;                   // contains PLU Group Number to be matched
    ULONG    culCounter;                  // contains the iteration control counter, initialize to zero to start.
    PLU_PARA PluPara20[OP_OEP_PLU_SIZE];
} PLUIF_OEP;

// The following struct allows for varying number of
// PLUs to be retrieved using CliOpOepPluRead().
// This struct must have the same members as the
// above struct PLUIF_OEP in order to work.
typedef    struct {
    USHORT   usGroupNo;
    ULONG    culCounter;
    PLU_PARA PluPara20[1];
} PLUIF_OEP_VARY;

typedef   struct {
    UCHAR   uchClass;
    ULONG   ulOffset;
}PARA_BRD;

/* MLD Mnemonics File, V3.3 */
typedef struct {
    WCHAR   aszMnemonics[OP_MLD_MNEMONICS_LEN];
} MLD_RECORD;

typedef struct {
    USHORT  usAddress;
    WCHAR   aszMnemonics[OP_MLD_MNEMONICS_LEN+1];
} MLDIF;

#define OPMNEMONICFILE_ID_MLD       1   // access the MLD mnemonics file
#define OPMNEMONICFILE_ID_REASON    2   // access the Reason code mnemonics file

typedef struct {
	USHORT  usMnemonicFileId;        // indicates which file OPMNEMONICFILE_ID_xxxx above.
	USHORT  usMnemonicAddress;
	WCHAR   aszMnemonicValue[48];
} OPMNEMONICFILE;

/* Add 2172 Rel1.0 */
typedef struct {
    UCHAR   uchGroupNo;
    WCHAR   uchPluNo[OP_PLU_LEN];
    UCHAR   uchAdjecive;
}OEPPLUIF;

/*--------------------------------------------
    The following struct, OPPLUOEPPLUNO, must
	be the same size as the message body part
	of the struct CLIREQOEPPLU.
--------------------------------------------*/
typedef struct {
    WCHAR   auchPluNo[OP_OEP_PLU_SIZE][OP_PLU_LEN];      /* for temporary */
} OPPLUOEPPLUNO;

/*--------------------------------------------
    The following struct, OPPLUOEPPLUNAME, must
	be the same size as the message body part
	of the struct CLIRESOEPPLU.
--------------------------------------------*/
typedef struct {
    WCHAR   auchPluName[OP_OEP_PLU_SIZE][STD_PLU_MNEMONIC_LEN];
    UCHAR   uchItemType[OP_OEP_PLU_SIZE];
    UCHAR   auchContOther[3][OP_OEP_PLU_SIZE];				//SR 336 increased to 3 so we can store the control codes
} OPPLUOEPPLUNAME;											//for prohibiting certain adjective levels

typedef union {
	OPPLUOEPPLUNO PluNo;
	OPPLUOEPPLUNAME PluName;
} OPPLUOEPIF;

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
/*
==================================================
   PROTOTYPE
==================================================
*/

/*-------------------
    User I/F
-------------------*/
VOID    OpInit(VOID);
/* Add 2172 Rel 1.0 */
SHORT   OpOepCreatFile(USHORT  usOepTableNo, ULONG ulNumberOfPlu, USHORT usLockHnd);
SHORT   OpOepCheckAndCreatFile(USHORT  usOepTableNo, ULONG ulNumberOfPlu, USHORT usLockHnd);      /* Saratoga */
SHORT   OpOepCheckAndDeleteFile(USHORT  usOepTableNo, ULONG ulNumberOfPlu, USHORT usLockHnd);     /* Saratoga */
SHORT   OpPluCreatFile(ULONG ulNumberOfPlu, USHORT usLockHnd);              /* Saratoga */
SHORT   OpPluCheckAndCreatFile(ULONG ulNumberOfPlu, USHORT usLockHnd);      /* Saratoga */
SHORT   OpPluCheckAndDeleteFile(ULONG ulNumberOfPlu, USHORT usLockHnd);     /* Saratoga */
SHORT   OpPluAssign(PLUIF *pPif, USHORT usLockHnd);
SHORT   OpPluDelete(PLUIF *pPif, USHORT usLockHnd);
SHORT   OpPluFileUpdate(USHORT usLockHnd);
SHORT   OpPluIndRead(PLUIF *pPif, USHORT usLockHnd);
SHORT   OpDeptCreatFile(USHORT usNumberOfDept, USHORT usLockHnd);
SHORT   OpDeptCheckAndCreatFile(USHORT usNumberOfDept, USHORT usLockHnd);
SHORT   OpDeptCheckAndDeleteFile(USHORT usNumberOfDept, USHORT usLockHnd);
SHORT   OpDeptAssign(DEPTIF *pDpi, USHORT usLockHnd);
SHORT   OpDeptIndRead(DEPTIF *pDpi, USHORT usLockHnd);
SHORT   OpDeptDelete(DEPTIF *pDpi, USHORT usLockHnd); /* 2172 */
SHORT   OpDeptAllRead(DEPTIF *pDpi, USHORT usLockHnd); /* 2172 */
SHORT   OpMajorDeptRead(MDEPTIF *pDpi, USHORT usLockHnd);
SHORT   OpPluAllRead(PLUIF *pPif, USHORT usLockHnd);
SHORT   OpRprtPluRead(PLUIF_REP *pPrp, USHORT usLockHnd);
SHORT   OpDeptPluRead(PLUIF_DEP *pDp, USHORT usLockHnd);
SHORT   OpOepPluRead(PLUIF_OEP *pDp, USHORT usLockHnd, UCHAR ucNumOfPLUrequested);
SHORT   OpOepPluReadMultiTable(USHORT usTableNo, USHORT usFilePart, PLUIF_OEP *pDp, USHORT usLockHnd, UCHAR ucNumOfPLUrequested);
SHORT   OpPluAbort(USHORT husHandle, USHORT usLockHnd); /* saratoga */
SHORT   OpCpnCreatFile(USHORT usNumberOfCpn, USHORT usLockHnd);
SHORT   OpCpnCheckAndCreatFile(USHORT usNumberOfCpn, USHORT usLockHnd);
SHORT   OpCpnCheckAndDeleteFile(USHORT usNumberOfCpn, USHORT usLockHnd);
SHORT   OpCpnAssign(CPNIF *pCpni, USHORT usLockHnd);
SHORT   OpCpnIndRead(CPNIF *pCpni, USHORT usLockHnd);
SHORT   OpCstrCreatFile(USHORT usNumberOfCstr, USHORT usLockHnd);
SHORT   OpCstrCheckAndCreatFile(USHORT usNumberOfCstr, USHORT usLockHnd);
SHORT   OpCstrCheckAndDeleteFile(USHORT usNumberOfCstr, USHORT usLockHnd);
SHORT   OpCstrAssign(CSTRIF *pCstr, USHORT usLockHnd);
SHORT   OpCstrDelete(CSTRIF *pCstr, USHORT usLockHnd);
SHORT   OpCstrIndRead(CSTRIF *pCstr, USHORT usLockHnd);
SHORT   OpLock(VOID);
VOID    OpUnLock(VOID);

SHORT   OpSignInBlockOn (VOID);
SHORT   OpSignInBlockOff (VOID);
SHORT   OpSignInStatus (VOID);

BOOL    OpIsUnLockedPluFile (VOID);
SHORT   OpReqPlu(USHORT usFcCode, USHORT usLockHnd);
SHORT   OpResPlu(UCHAR  *puchRcvData,
                 USHORT  usRcvLen,
                 UCHAR  *puchSndData,
                 USHORT *pusSndLen,
                 USHORT  usLockHnd);
SHORT   OpReqDept(USHORT usFcCode, USHORT usLockHnd);
SHORT   OpResDept(UCHAR  *puchRcvData,
                  USHORT  usRcvLen,
                  UCHAR  *puchSndData,
                  USHORT *pusSndLen,
                  USHORT  usLockHnd);
/* Add 2172 Rel1.0 */
SHORT   OpReqOep(USHORT usFcCode, USHORT usLockHnd);
/* Add 2172 Rel1.0 */
SHORT   OpResOep(UCHAR  *puchRcvData,
                  USHORT  usRcvLen,
                  UCHAR  *puchSndData,
                  USHORT *pusSndLen,
                  USHORT  usLockHnd);
SHORT   OpReqCpn(USHORT usFcCode, USHORT usLockHnd);
SHORT   OpResCpn(UCHAR  *puchRcvData,
                  USHORT  usRcvLen,
                  UCHAR  *puchSndData,
                  USHORT *pusSndLen,
                  USHORT  usLockHnd);
SHORT   OpReqCstr(USHORT usFcCode, USHORT usLockHnd);
SHORT   OpResCstr(UCHAR  *puchRcvData,
                  USHORT  usRcvLen,
                  UCHAR  *puchSndData,
                  USHORT *pusSndLen,
                  USHORT  usLockHnd);
SHORT   OpResAll(UCHAR  *puchRcvData,
                 USHORT  usRcvLen,
                 UCHAR  *puchSndData,
                 USHORT *pusSndLen,
                 USHORT  usLockHnd);
SHORT   OpReqAll(USHORT usLockHnd);
SHORT   OpReqAllPara(USHORT usLockHnd);
SHORT   OpReqPluOep(USHORT usFcode, USHORT usLockHnd);
SHORT   OpReqSup(USHORT usLockHnd);
SHORT   OpReqSupPara(USHORT usLockHnd);
SHORT   OpResSup(UCHAR  *puchRcvData,
                    USHORT  usRcvLen,
                    UCHAR  *puchSndData,
                    USHORT *pusSndLen,
                    USHORT  usLockHnd);
SHORT   OpReqBackUp(USHORT usLockHnd);
SHORT   OpReqBackUpPara(USHORT usLockHnd);
SHORT   OpResBackUp(UCHAR  *puchRcvData,
                    USHORT  usRcvLen,
                    UCHAR  *puchSndData,
                    USHORT *pusSndLen,
                    USHORT  usLockHnd);


// The following #if is used to enable or disable
// functionality to issues NHPOS_ASSERT log entries when
// the function TtlConvertError () is called during NeighborhoodPOS
// execution.  This functionality provides a simple way to turn on the
// ability to have Assert logs created by calls to TtlConvertError()
// that provide the file and line number in the file from whence the
// the function was called.  This can provide additional error debugging.
#if 0
// generate a warning so this place is easy to find from a compiler warning.
#pragma message("  ====++++====   OpConvertError_Debug() is ENABLED   \z   ====++++====")
#define OpConvertError(sError) OpConvertError_Debug(sError, __FILE__, __LINE__)
USHORT  OpConvertError_Debug(SHORT sError, char *aszFilePath, int nLineNo);               /* Convert error code  */
#else
USHORT   OpConvertError(SHORT sError);
#endif

/* Add 2172 Rel 1.0 */
#if 0
// generate a warning so this place is easy to find from a compiler warning.
#pragma message("  ====++++====   OpConvertPluError_Debug() is ENABLED   \z   ====++++====")
#define OpConvertPluError(usError) OpConvertPluError_Debug(usError, __FILE__, __LINE__)
USHORT  OpConvertPluError_Debug(USHORT usError, char *aszFilePath, int nLineNo);               /* Convert error code  */
#else
SHORT    OpConvertPluError(USHORT usError);
#endif

/* === New Functions - Promotional Pricing Item (Release 3.1) BEGIN === */
SHORT   OpPpiCreatFile(USHORT usNumberOfPpi, USHORT usLockHnd);
SHORT   OpPpiCheckAndCreatFile(USHORT usNumberOfPpi, USHORT usLockHnd);
SHORT   OpPpiCheckAndDeleteFile(USHORT usNumberOfPpi, USHORT usLockHnd);
SHORT   OpPpiAssign(PPIIF *pPif, USHORT usLockHnd);
SHORT   OpPpiDelete(PPIIF *pPif, USHORT usLockHnd);
SHORT   OpPpiIndRead(PPIIF *pPif, USHORT usLockHnd);
SHORT   OpReqPpi(USHORT usFcCode, USHORT usLockHnd);
SHORT   OpResPpi(UCHAR  *puchRcvData, USHORT  usRcvLen, UCHAR  *puchSndData, USHORT *pusSndLen, USHORT  usLockHnd);
/* === New Functions - Promotional Pricing Item (Release 3.1) END === */
/* Mld Mnemonics File, V3.3 */
SHORT   OpMldCreatFile(USHORT usNumberOfAddress, USHORT usLockHnd);
SHORT   OpMldCheckAndCreatFile(USHORT usNumberOfAddress, USHORT usLockHnd);
SHORT   OpMldCheckAndDeleteFile(USHORT usNumberOfAddress, USHORT usLockHnd);
SHORT   OpMldAssign(MLDIF *pMld, USHORT usLockHnd);
SHORT   OpMldIndRead(MLDIF *pMld, USHORT usLockHnd);
SHORT   OpReqMld(USHORT usFcCode, USHORT usLockHnd);
SHORT   OpResMld(UCHAR  *puchRcvData, USHORT  usRcvLen, UCHAR  *puchSndData, USHORT *pusSndLen, USHORT  usLockHnd);
SHORT   OpPluOepRead(OPPLUOEPPLUNO *pPluNo, OPPLUOEPPLUNAME *pPluName, USHORT usLockHnd);
SHORT   OpReqBiometrics(USHORT usFcCode, USHORT usLockHnd);
SHORT   OpResBiometrics(UCHAR  *puchRcvData, USHORT  usRcvLen, UCHAR  *puchSndData, USHORT *pusSndLen, USHORT  usLockHnd);
SHORT   OpReqReasonCodes(USHORT usFcCode, USHORT usLockHnd);
SHORT   OpResReasonCodes(UCHAR  *puchRcvData, USHORT  usRcvLen, UCHAR  *puchSndData, USHORT *pusSndLen, USHORT  usLockHnd);

/* Layout Files ESMITH LAYOUT */
SHORT   OpReqLay(USHORT usLockHnd);
SHORT   OpReqPrinterLogos(USHORT usLockHnd);
SHORT   OpReqAdditionalParams(USHORT usLockHnd);
SHORT   OpReqBiometricsFile(USHORT usLockHnd);
SHORT   OpReqFile(TCHAR *pszFileName, USHORT usLockHnd);
SHORT   OpTransferFile(USHORT usTerminalPosition, TCHAR *pszLocalFileName, TCHAR *pszRemoteFileName, USHORT usLockHnd);
SHORT   OpDisplayOperatorMessage(USHORT  *pusMessageBufferEnum);

SHORT   OpMnemonicsFileCreate (USHORT usMnemonicFileId, USHORT usNumberOfAddress, USHORT usLockHnd);
SHORT   OpMnemonicsFileCheckAndCreatFile(USHORT usMnemonicFileId, USHORT usNumberOfAddress, USHORT usLockHnd);
SHORT   OpMnemonicsFileCheckAndDeleteFile(USHORT usMnemonicFileId, USHORT usNumberOfAddress, USHORT usLockHnd);
SHORT   OpMnemonicsFileAssign(OPMNEMONICFILE *pMnemonicFile, USHORT usLockHnd);
SHORT   OpMnemonicsFileIndRead(OPMNEMONICFILE *pMnemonicFile, USHORT usLockHnd);

#endif
/*=========== End of header ================*/