/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1999      ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                Release 3.3              ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server OPERATIONAL PARAMETER module Header
*   Category           : Client/Server OPERATIONAL PARAMETER module
*   Program Name       : CSOP.H  (Header file for user)
*  ------------------------------------------------------------------------
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.
*   Memory Model       : Medium Model
*   Options            :  /c /AM /Gs /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*   Update Histories   :
*
*   Date      :Ver.Rev :NAME       :Description
*   Nov-08-93 :00.00.01:H.YAMAGUCHI:Increase 3000 PLU for HOTEL
*   Mar-14-95 :03.00.00:H.Ishida   :R3.0
*   Apr-06-95 :03.00.00:H.Ishida   :Add Control String Table
*   Jan-31-96 :03.00.00:M.Suzuki   :Add R3.1
*   Sep-02-98 :03.03.00:A.Mitsui   :V3.3
*   Dec-15-99 :01.00.00:hrkato     :Saratoga
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


/*
===========================================================================
    DEFINE
===========================================================================
*/
// define UWCHAR appropriately for database conversion to multi-lingual
#if !defined(UWCHAR)
#if defined(_PEPWIDECHAR)
#define UWCHAR WCHAR
#else
#define UWCHAR UCHAR
#endif
#endif

#define		OP_CPN_STS_SIZE_HGV10	2
#define		OP_CPN_NAME_SIZE_HGV10	12
#define		OP_CPN_TARGET_SIZE_HGV10	7
#define		OP_PLU_LEN_HGV10		13		//old plu len Ver 1.4
#define		OP_CSTR_INDEXV10_SIZE	200		//old CSTR Index size ver 1.4

#define    OP_PLU_LEN                STD_PLU_NUMBER_LEN  		/*** NCR2172 ***/
#define    OP_PLU_OEP_LEN          (STD_PLU_NUMBER_LEN+3)  // used with OEP file to hold 3 digit priority plus PLU number
#define    OP_PLU_NAME_SIZE          20         /* Saratoga */
#define    OP_PLU_NAME_SIZE_OLD      12         /* Saratoga */
#define    OP_PLU_PRICE_SIZE          3

/*#define    OP_PLU_CONT_OTHER        6   Send Remote Printer #5-8 (R3.1) */
#define    OP_PLU_CONT_OTHER         12         /*Send Remote Printer #5-8 (Saratoga) */
/*#define    OP_PLU_CONT_OTHER         10       /* Send Remote Printer #5-8 (Saratoga) *//* ### MOD Saratoga (12 --> 10)(V1_0.02)(051800) */

#define    OP_PLU_CONT_OTHER_OLD      5
#define    OP_DEPT_CONT_CODE_SIZE     5        /* Send Remote Printer #5-8 (R3.1) */
#define    OP_DEPT_CONT_CODE_SIZE_OLD 4
#define    OP_DEPT_NAME_SIZE         20        /* Saratoga */
#define    OP_DEPT_HALO_SIZE          1
#define    OP_CPN_STS_SIZE            2
#define    OP_CPN_NAME_SIZE          STD_CPNNAME_LEN
#define    OP_CPN_TARGET_SIZE         7
#define    OP_MAX_WORK_BUFFER       512        /* OLD BUFFER is 1000 */
#define    OP_MAX_WORK2_BUFFER     1500
#define    OP_NUMBER_OF_MAX_PLU  500000        /* Saratoga, should be same as FLEX_PLU_MAX */
#define    OP_NUMBER_OF_MAX_DEPT    250        /* OLD MAX is 50 NCR2172 */
/* Add 2172 Rel 1.0 */
#define    OP_NUMBER_OF_MAX_OEP    8000
#define    OP_MAX_DEPT_NO          9999        /* Saratoga */
#define    OP_MAX_REPORT_NO          10
#define    OP_MAX_GROUP_NO           99
#define    OP_CSTR_INDEX_SIZE       400
#define    OP_CSTR_PARA_SIZE        200
#define    OP_MAX_PPI_NO            300        /* Promotional Pricing Item, R3.1 ESMITH*/
#define    OP_MAX_PPI_LEVEL          40        /* Price Level,              R3.1 */ /* 20->40 for PPI Enhancement - CSMALL */
#define    OP_MAX_PPI_LEVEL_V203     20        /* changed to 40 in 2.1 - CSMALL */
#define    OP_MLD_MNEMONICS_LEN      32        /* Mld Mnemonis Lenggh,      V3.3 */

#define    OP_OEP_PLU_SIZE           20        /* 2172 */
#define	   OP_MAX_PLU_STATUS_LEN	 10		   /* array size for auchStatus2 in OPPLU_PARAENTRY struct */
/*
===========================================================================
   DEFINE FOR ERROR CODE
===========================================================================
*/

/* External error code for user interface */

#define  OP_PERFORM             0

#define  OP_FILE_NOT_FOUND     -1
#define  OP_PLU_FULL           -2
#define  OP_FILE_FULL          OP_PLU_FULL
/*** NCR2172 ***/
#define  OP_DEPT_FULL          OP_PLU_FULL

/* Add 2172 Rel 1.0 */
#define  OP_OEP_FULL          OP_PLU_FULL

#define  OP_NOT_IN_FILE        -3
#define  OP_LOCK               -5
#define  OP_NO_MAKE_FILE       -6
#define  OP_ABNORMAL_DEPT      -7
#define  OP_ABNORMAL_CPN       -8
#define  OP_ABNORMAL_CSTR      -9

#define  OP_CHANGE_PLU         -10
/*** NCR2172 ***/
#define  OP_CHANGE_DEPT        OP_CHANGE_PLU

#define  OP_NEW_PLU            -11
/*** NCR2172 ***/
#define  OP_NEW_DEPT           OP_NEW_PLU
/* Add 2172 Rel 1.0 */
#define  OP_CHANGE_OEP        OP_CHANGE_PLU
/* Add 2172 Rel 1.0 */
#define  OP_NEW_OEP           OP_NEW_PLU

#define  OP_NO_COMPRESS_FILE   -12
#define  OP_DELETE_CSTR_FILE   -16
#define  OP_DELETE_PLU_FILE    -17
#define  OP_DELETE_DEPT_FILE   -18
#define  OP_DELETE_CPN_FILE    -19
#define  OP_CHANGE_PPI         OP_CHANGE_PLU    /* change PPI Record.  R3.1 */
#define  OP_NEW_PPI            OP_NEW_PLU       /* Add new PPI Record, R3.1 */
#define  OP_DELETE_PPI_FILE    -15              /* delete PPI File,    R3.1 */
#define  OP_ABNORMAL_PPI       -16              /* ppi code is bad,    R3.1 */
#define  OP_CHANGE_MLD         OP_CHANGE_PLU    /* change Mld Record.  V3.3 */
#define  OP_NEW_MLD            OP_NEW_PLU       /* Add new Mld Record, V3.3 */
#define  OP_DELETE_MLD_FILE    -21              /* delete Mld File,    V3.3 */
#define  OP_ABNORMAL_MLD       -22              /* Mld code is bad,    V3.3 */

#define  OP_NOT_MASTER         -20
/* Add 2172 Rel1.0 */
#define  OP_PLU_ERROR          -23

/* Internal error code  */

#define  OP_NO_READ_SIZE       -31
#define  OP_HEAD_OF_TABLE      -32
#define  OP_TAIL_OF_TABLE      -33
#define  OP_DONT_HIT           -34
#define  OP_EOF                -35
#define  OP_SEND_ERROR         -36
#define  OP_COMM_ERROR         -37
#define  OP_CONT               -38

/*
===========================================================================
    TYPEDEF
===========================================================================
*/

//RPH added pragma pack
#pragma pack(push, 1)

typedef struct {
	LONG lAmount;
	SHORT sCounter;
}INV_TOTAL;// SR167 - CSMALL

/*** NCR2172
typedef  struct {
   USHORT   usNumberOfDept;
} OPDEPT_FILEHED;
***/
typedef  struct {
   ULONG    ulDeptFileSize;
   USHORT   usNumberOfMaxDept;
   USHORT   usNumberOfResidentDept;
   ULONG    offulOfIndexTbl;
   ULONG    offulOfIndexTblBlk;
   ULONG    offulOfParaEmpTbl;
   ULONG    offulOfParaTbl;
   ULONG    offulOfWork;
} OPDEPT_FILEHED;

typedef  struct {
   USHORT   usNumberOfCpn;
} OPCPN_FILEHED;

typedef  struct {
   USHORT   usNumberOfCstr;
   USHORT   usOffsetOfTransCstr;
} OPCSTR_FILEHED;

/* === Add PPI (Promotional Pricing Item) File (Release 3.1) BEGIN === */
typedef  struct {
   USHORT   usNumberOfPpi;
} OPPPI_FILEHED;
/* === Add PPI (Promotional Pricing Item) File (Release 3.1) END === */

typedef  struct {
/*    UCHAR   uchDept;  */
    USHORT  usDept;          /* for temporary *//* Saratoga */
    UCHAR   uchItemType;     /* Saratoga */
    UCHAR   uchRept;         // Report Code
    UCHAR   auchContOther[OP_PLU_CONT_OTHER];   // 24
	USHORT	usBonusIndex;    // bonus total index
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
    WCHAR/*UWCHAR*/   auchPluName[OP_PLU_NAME_SIZE + 1];
    UCHAR   auchPrice[OP_PLU_PRICE_SIZE];
    UCHAR   uchTableNumber;     // Order Entry Prompt (OEP) table row number.
    UCHAR   uchGroupNumber;
    UCHAR   uchPrintPriority;
    /* === New Elements (Release 3.1) BEGIN === */
    USHORT   uchLinkPPI;         /* Link PPI Code,   Release 3.1 */
    /* === New Elements (Release 3.1) END === */
    UCHAR   uchPM;
    UCHAR   uchRestrict;
    USHORT  usLinkNo;
	USHORT	usFamilyCode;	/* ### ADD Saratoga (V1_0.02)(051800) */
	// 2.1 Release Changes, SR167 - CSMALL
	ULONG	ulVoidTotalCounter;				// Void Total/Counter for each PLU
	ULONG	ulDiscountTotalCounter;			// Discount Total/Counter for each PLU
	ULONG	ulFreqShopperPoints;			// Frequent Shopper Discount/Points (GP Feature)
	UCHAR	auchAlphaNumRouting[4];			// Alphanumeric routing character for sending to KDS or Printers
	INV_TOTAL	futureTotals[10];			// basic inventory totals (on-hand, receipts/transfers, begin count,
											//   reorder count, current cost, original cost, etc.)
	WCHAR	auchAltPluName[OP_PLU_NAME_SIZE + 1];	// Multilanguage mnemonic for each PLU
	UCHAR	uchColorPaletteCode;			// Color pallete code for PLU
	UCHAR	auchStatus2[OP_MAX_PLU_STATUS_LEN];
	USHORT	usTareInformation;				//USHORT used for High bit will be an identifier, and the low bit will be the number JHHJ
	UCHAR   uchExtendedGroupPriority[12];      // extended group numbers, first byte is group number, second byte is priority within group
	UCHAR   uchPluOptionFlags[6];                 // bit map for option flags
}OPPLU_PARAENTRY;


/*** NCR2172 ***/
typedef  struct {
    UCHAR   uchMdept;
    UCHAR   auchControlCode[OP_DEPT_CONT_CODE_SIZE];
    UCHAR   auchHalo[OP_DEPT_HALO_SIZE];
    WCHAR   auchMnemonic[OP_DEPT_NAME_SIZE];
    UCHAR   uchPrintPriority;
	USHORT	usBonusIndex;
	USHORT	usTareInformation;				//USHORT used for High bit will be an identifier, and the low bit will be the number JHHJ
	UCHAR   uchExtraRoom[8];      // extra space that can be used for added functionality
}OPDEPT_PARAENTRY;


/*** NCR2172 ***/
typedef  struct {
    UCHAR   uchMdept;
    UCHAR   auchControlCode[OP_DEPT_CONT_CODE_SIZE];
    UCHAR   auchHalo[OP_DEPT_HALO_SIZE];
    WCHAR   auchMnemonic[OP_DEPT_NAME_SIZE];
    UCHAR   uchPrintPriority;
}OPDEPT_PARAENTRY_HGV201;

typedef  struct {
    UCHAR   uchMdept;
    UCHAR   auchControlCode[OP_DEPT_CONT_CODE_SIZE];
    UCHAR   auchHalo[OP_DEPT_HALO_SIZE];
    UCHAR   auchMnemonic[OP_DEPT_NAME_SIZE];
    UCHAR   uchPrintPriority;
}OPDEPT_PARAENTRY_HGV10;

/*** NCR21 ***/
typedef  struct {
    UCHAR   uchMdept;
    UCHAR   auchControlCode[OP_DEPT_CONT_CODE_SIZE];
    UCHAR   auchHalo[OP_DEPT_HALO_SIZE];
    UCHAR   auchMnemonic[OP_DEPT_NAME_SIZE];
    UCHAR   uchPrintPriority;
}HGV10_DEPTPARA;


typedef  struct {
    UCHAR   uchCouponStatus[OP_CPN_STS_SIZE_HGV10];
    ULONG   ulCouponAmount;
    UCHAR   aszCouponMnem[OP_CPN_NAME_SIZE_HGV10];
    UCHAR   auchCouponTarget[OP_CPN_TARGET_SIZE_HGV10][OP_PLU_LEN_HGV10];
    UCHAR   uchCouponAdj[OP_CPN_TARGET_SIZE_HGV10];
}COMCOUPONPARA_HGV10;


typedef  struct {
    UCHAR   uchCouponStatus[OP_CPN_STS_SIZE];
	UCHAR   uchAutoCPNStatus;
    ULONG   ulCouponAmount;
    WCHAR   aszCouponMnem[OP_CPN_NAME_SIZE];
    WCHAR   auchCouponTarget[OP_CPN_TARGET_SIZE][OP_PLU_LEN];
    UCHAR   uchCouponAdj[OP_CPN_TARGET_SIZE];
	USHORT	usBonusIndex;
	UCHAR   uchRestrict;          // Sales Code restriction value 0 - 7. high bit 0x80 indicates if Supr Intv allowed.
	UCHAR   uchExtraRoom[7];      // extra space that can be used for added functionality
}COMCOUPONPARA;

typedef  struct {
    UCHAR   uchCouponStatus[2];
	UCHAR   uchAutoCPNStatus;
    ULONG   ulCouponAmount;
    WCHAR   aszCouponMnem[12];
    WCHAR   auchCouponTarget[OP_CPN_TARGET_SIZE][OP_PLU_LEN];
    UCHAR   uchCouponAdj[OP_CPN_TARGET_SIZE];
}COMCOUPONPARA_V203;

typedef  struct {
    UCHAR   uchCouponStatus[2];
    ULONG   ulCouponAmount;
    WCHAR   aszCouponMnem[OP_CPN_NAME_SIZE_HGV10];
    WCHAR   auchCouponTarget[OP_CPN_TARGET_SIZE][OP_PLU_LEN];
    UCHAR   uchCouponAdj[OP_CPN_TARGET_SIZE];
}COMCOUPONPARA_V201;

/* for Migrate, Saratoga */
typedef  struct {
    UCHAR   uchCouponStatus[OP_CPN_STS_SIZE];
    ULONG   ulCouponAmount;
    UCHAR   aszCouponMnem[OP_CPN_NAME_SIZE];
    USHORT  usCouponTarget[OP_CPN_TARGET_SIZE];
    UCHAR   uchCouponAdj[OP_CPN_TARGET_SIZE];
}COMCOUPONPARA_V34;

/* === New Record - Promotional Pricing Item (Release 3.1) BEGIN === */
typedef struct {
    UCHAR   uchQTY;     /* Price Multiple */
    ULONG   ulPrice;    /* Promotional Price */
} PPI_RECORD;

// Following flags are used with ulPpiLogicFlags01 to indicate
// business rule changes and how the data in the PPI_RECORD is treated.
#define PPI_LOGICFLAGS01_PLUDISCOUNT    0x00000002   // ulPrice is a discount percentage nn.n% on PLU price
#define PPI_LOGICFLAGS01_PLUPREMIUM     0x00000004   // ulPrice is a premium percentage nn.n% on PLU price
#define PPI_LOGICFLAGS01_COMBINENOT     0x00000008   // do not combine separate PLUs using same PPI table

typedef struct {
    PPI_RECORD  PpiRec[OP_MAX_PPI_LEVEL];  /* PPI Record */
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
    PPI_RECORD  PpiRec[OP_MAX_PPI_LEVEL_V203];  /* PPI Record */
} OPPPI_PARAENTRY_V203;		// for 2.1 conversion - CSMALL

typedef struct {
    USHORT           uchPpiCode;	//ESMITH
    OPPPI_PARAENTRY ParaPpi;
} PPIIF;

/* === New Record - Promotional Pricing Item (Release 3.1) END === */

typedef    struct {
    /* USHORT  usPluNo; */
    WCHAR	auchPluNo[OP_PLU_LEN];  /* for temporary *//* Saratoga */
    UCHAR   uchPluAdj;
    ULONG   ulCounter;
    USHORT  husHandle;                  /* 2172 */
    USHORT  usAdjCo;                    /* 2172 */
    OPPLU_PARAENTRY    ParaPlu;
}PLUIF;

/*** NCR2172
typedef    struct {
    UCHAR    uchDeptNo;
    DEPTPARA ParaDept;
}DEPTIF;
***/
typedef    struct {
    USHORT   usDeptNo;
    ULONG   ulCounter;
    OPDEPT_PARAENTRY    ParaDept;
}DEPTIF;

typedef    struct {
    USHORT   usDeptNo;
    ULONG   ulCounter;
    OPDEPT_PARAENTRY_HGV10    ParaDept;
}DEPTIF_HGV10;

typedef    struct {
    USHORT   usDeptNo;
    ULONG   ulCounter;
    OPDEPT_PARAENTRY_HGV201    ParaDept;
}DEPTIF_HGV201;

typedef    struct {
    USHORT    uchCpnNo;
    COMCOUPONPARA ParaCpn;
}CPNIF;

typedef    struct {
    UCHAR    uchCpnNo;
    COMCOUPONPARA_HGV10 ParaCpn;
}CPNIF_HGV10;

/* for Migrate from Hosp V3.3/3.4 */
typedef    struct {
    UCHAR    uchCpnNo;
    COMCOUPONPARA_V34 ParaCpn;
}CPNIF_V34;


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

typedef    struct {                 /* Saratoga */
    UCHAR    uchMajorDeptNo;
    OPDEPT_PARAENTRY    ParaDept;
    USHORT    usDeptNo;
    ULONG    ulCounter;
}MDEPTIF;

/*** NCR2172
typedef    struct {
    UCHAR    uchMajorDeptNo;
    DEPTPARA ParaDept;
    UCHAR    uchDeptNo;
}MDEPTIF;
***/

typedef    struct {
    UCHAR    uchReport;
    ULONG    culCounter;
/*    USHORT   usPluNo; */
    WCHAR   auchPluNo[OP_PLU_LEN];      /* for temporary */
    UCHAR    uchPluAdj;
    USHORT   usAdjCo;                   /* 2172 */
    USHORT   husHandle;                 /* 2172 */
    OPPLU_PARAENTRY    ParaPlu;
} PLUIF_REP;

typedef    struct {
    USHORT   usDept;                    /* Saratoga */
    ULONG    culCounter;
    WCHAR    auchPluNo[OP_PLU_LEN];     /* Saratoga */
    UCHAR    uchPluAdj;
    USHORT   usAdjCo;                   /* 2172 */
    USHORT   husHandle;                 /* 2172 */
    OPPLU_PARAENTRY    ParaPlu;
} PLUIF_DEP;

typedef    struct {
    WCHAR    auchPluNo[OP_PLU_LEN];    /* Saratoga */
    UCHAR    uchPluAdj;
    WCHAR    uchPluName[OP_PLU_NAME_SIZE];
} PLU_PARA;

typedef    struct {
    UCHAR    uchGroupNo;
    ULONG    culCounter;
    PLU_PARA PluPara20[OP_OEP_PLU_SIZE];
} PLUIF_OEP;

typedef   struct {
    UCHAR   uchClass;
    ULONG   ulOffset;
}PARA_BRD;

typedef  struct {
    UCHAR   uchDept;
    UCHAR   uchRept;
    UCHAR   auchContOther[OP_PLU_CONT_OTHER_OLD];
}PLU_CONTROL_OLD;

typedef    struct {
    PLU_CONTROL_OLD  ContPlu;
    UCHAR   auchPluName[OP_PLU_NAME_SIZE_OLD];
    UCHAR   auchPrice[OP_PLU_PRICE_SIZE];
}OPPLU_PARAENTRY_R25;

typedef  struct {
    UCHAR   uchMdept;
    UCHAR   auchControlCode[OP_DEPT_CONT_CODE_SIZE_OLD];
    UCHAR   auchHalo[OP_DEPT_HALO_SIZE];
    UCHAR   auchMnemonic[OP_DEPT_NAME_SIZE];
}DEPTPARA_R25;

typedef    struct {
    PLU_CONTROL_OLD  ContPlu;
    UCHAR   auchPluName[OP_PLU_NAME_SIZE];
    UCHAR   auchPrice[OP_PLU_PRICE_SIZE];
    UCHAR   uchTableNumber;
    UCHAR   uchGroupNumber;
    UCHAR   uchPrintPriority;
}OPPLU_PARAENTRY_R30;

typedef  struct {
    UCHAR   uchMdept;
    UCHAR   auchControlCode[OP_DEPT_CONT_CODE_SIZE_OLD];
    UCHAR   auchHalo[OP_DEPT_HALO_SIZE];
    UCHAR   auchMnemonic[OP_DEPT_NAME_SIZE];
    UCHAR   uchPrintPriority;
}DEPTPARA_R30;

/* MLD Mnemonics File, V3.3 */
typedef struct {
    UCHAR   aszMnemonics[OP_MLD_MNEMONICS_LEN];
} MLD_RECORD_V33;

typedef struct {
    UCHAR   uchAddress;
    UCHAR   aszMnemonics[OP_MLD_MNEMONICS_LEN+1];
} MLDIF_V33;

/* MLD Mnemonics File, NHPOS Rel 2.0 multi-lingual*/
typedef struct {
    WCHAR   aszMnemonics[OP_MLD_MNEMONICS_LEN + 1];
} MLD_RECORD;

typedef struct {
    USHORT  uchAddress;
    WCHAR   aszMnemonics[OP_MLD_MNEMONICS_LEN + 1];
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

/* from GP Ver 2.0/2.1/2.2 to Saratoga */

#define    OP_DEPT_NAME_SIZE_R34      12

typedef  struct {
   USHORT   usNumberOfDept;
} OPDEPT_FILEHED_R34;

typedef  struct {
    UCHAR   uchMdept;
    UCHAR   auchControlCode[OP_DEPT_CONT_CODE_SIZE];
    UCHAR   auchHalo[OP_DEPT_HALO_SIZE];
    UCHAR   auchMnemonic[OP_DEPT_NAME_SIZE_R34];
    UCHAR   uchPrintPriority;
}DEPTPARA_R34;


typedef  struct {
   USHORT   usNumberOfDept;
} OPDEPT_FILEHED_GP;

typedef  struct {
    ULONG   ulPrice;
    UCHAR   aszMnemonic[16];
    UCHAR   auchStatus[3];
    UCHAR   uchPM;
    UCHAR   uchTareCode;
    UCHAR   uchGroupA;
    UCHAR   uchGroupB;
    UCHAR   uchHalo;
    UCHAR   uchBonusStatus;
}DEPTPARA_GP;

typedef    struct {
    UCHAR    uchDeptNo;
    DEPTPARA_GP ParaDept;
}DEPTIF_GP;

/*---- Control String Conversion Structure ESMITH---- */
typedef struct{
	OPCSTR_FILEHED		CstrFileHed;                            /* Control String File Header */
	OPCSTR_INDEXENTRY	CstrIndexV10[OP_CSTR_INDEXV10_SIZE];    /* Control String Index table with size of R10 data size */
	OPCSTR_PARAENTRY	ParaEntry[OP_CSTR_INDEXV10_SIZE][OP_CSTR_PARA_SIZE]; /* Control Sting Paraentry table */
}OPCSTR_V10;

#pragma pack(pop)
/*
==================================================
   PROTOTYPE
==================================================
*/

/*-------------------
    User I/F
-------------------*/
VOID    OpInit(VOID);
/* SHORT   PEPENTRY OpPluCreatFile(ULONG, USHORT);                     */     /* Saratoga */
/* SHORT   OpPluCheckAndCreatFile(USHORT usNumberOfPlu, USHORT usLockHnd); */
/* SHORT   OpPluCheckAndDeleteFile(USHORT usNumberOfPlu, USHORT usLockHnd); */

SHORT   OpOepCreatFile(ULONG ulNumberOfPlu, USHORT usLockHnd);		     /* Saratoga */
SHORT   OpOepCreatFilev221(USHORT usOepTableNo, ULONG ulNumberOfPlu, USHORT usLockHnd);		     /* Saratoga */
SHORT   OpPluCreatFile(ULONG ulNumberOfPlu, USHORT usLockHnd);              /* Saratoga */
SHORT   OpPluCheckAndCreatFile(ULONG ulNumberOfPlu, USHORT usLockHnd);      /* Saratoga */
SHORT   OpPluCheckAndDeleteFile(ULONG ulNumberOfPlu, USHORT usLockHnd);     /* Saratoga */

SHORT   OpPluAssign(PLUIF *pPif, USHORT usLockHnd);
SHORT   OpPluDelete(PLUIF *pPif, USHORT usLockHnd);
SHORT   OpPluFileUpdate(USHORT usLockHnd);
SHORT   OpPluIndRead(PLUIF *pPif, USHORT usLockHnd);
SHORT   OpDeptCreatFile(USHORT, USHORT);
/* SHORT   OpDeptCheckAndCreatFile(USHORT usNumberOfDept, USHORT usLockHnd);   */
/* SHORT   OpDeptCheckAndDeleteFile(USHORT usNumberOfDept, USHORT usLockHnd);  */
SHORT   OpDeptAssign(DEPTIF *pDpi, USHORT usLockHnd);
SHORT   OpDeptIndRead(DEPTIF *pDpi, USHORT usLockHnd);
/* SHORT   OpMajorDeptRead(MDEPTIF *pDpi, USHORT usLockHnd); */
SHORT   OpPluAllRead(PLUIF *pPif, USHORT usLockHnd);
/* SHORT   OpRprtPluRead(PLUIF_REP *pPrp, USHORT usLockHnd);  */
/* SHORT   OpDeptPluRead(PLUIF_DEP *pDp, USHORT usLockHnd);   */
SHORT   OpOepPluRead(PLUIF_OEP *pDp, USHORT usLockHnd);
SHORT   OpCpnCreatFile(USHORT usNumberOfCpn, USHORT usLockHnd);
/* SHORT   OpCpnCheckAndCreatFile(USHORT usNumberOfCpn, USHORT usLockHnd);   */
/* SHORT   OpCpnCheckAndDeleteFile(USHORT usNumberOfCpn, USHORT usLockHnd);  */
SHORT   OpCpnAssign(CPNIF *pCpni, USHORT usLockHnd);
SHORT   OpCpnIndRead(CPNIF *pCpni, USHORT usLockHnd);
SHORT   OpPluMigrateFile(VOID);    /* PEP only (for migrate) */
SHORT   OpPluMigrate25File(VOID);  /* PEP only (for migrate) */
SHORT   OpPluMigrate30File(VOID);  /* PEP only (for migrate) */
SHORT   OpCpnMigrate30File(VOID);  /* PEP only (for migrate) */
SHORT   OpCstrCreatFile(USHORT, USHORT);
/* SHORT   OpCstrCheckAndCreatFile(USHORT usNumberOfCstr, USHORT usLockHnd); */
/* SHORT   OpCstrCheckAndDeleteFile(USHORT usNumberOfCstr, USHORT usLockHnd);*/
SHORT   OpCstrAssign(CSTRIF *pCstr, USHORT usLockHnd);
SHORT   OpCstrDelete(CSTRIF *pCstr, USHORT usLockHnd);
SHORT   OpCstrIndRead(CSTRIF *pCstr, USHORT usLockHnd);
/*
SHORT   OpLock(VOID);
VOID    OpUnLock(VOID);
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

USHORT   OpConvertError(SHORT sError);
*/

/* === New Functions - Promotional Pricing Item (Release 3.1) BEGIN === */
SHORT   OpPpiCreatFile(USHORT usNumberOfPpi, USHORT usLockHnd);
/* SHORT   OpPpiCheckAndCreatFile(USHORT usNumberOfPpi, USHORT usLockHnd); */
/* SHORT   OpPpiCheckAndDeleteFile(USHORT usNumberOfPpi, USHORT usLockHnd); */
SHORT   OpPpiAssign(PPIIF *pPif, USHORT usLockHnd);
SHORT   OpPpiDelete(PPIIF *pPif, USHORT usLockHnd);
SHORT   OpPpiIndRead(PPIIF *pPif, USHORT usLockHnd);
/* SHORT   OpReqPpi(USHORT usFcCode, USHORT usLockHnd); */
/* SHORT   OpResPpi(UCHAR  *puchRcvData,
                          USHORT  usRcvLen,
                          UCHAR  *puchSndData,
                          USHORT *pusSndLen,
                          USHORT  usLockHnd);   */
/* === New Functions - Promotional Pricing Item (Release 3.1) END === */

/* Mld Mnemonics File, V3.3 */
SHORT   OpMldCreatFile(ULONG ulNumberOfAddress, USHORT usLockHnd);
SHORT   OpMldCheckAndCreatFile(USHORT usNumberOfAddress, USHORT usLockHnd);
SHORT   OpMldCheckAndDeleteFile(USHORT usNumberOfAddress, USHORT usLockHnd);
SHORT   OpMldAssign(MLDIF *pMld, USHORT usLockHnd);
SHORT   OpMldIndRead(MLDIF *pMld, USHORT usLockHnd);
/* SHORT   OpReqMld(USHORT usFcCode, USHORT usLockHnd); */
/* SHORT   OpResMld(UCHAR  *puchRcvData,
                          USHORT  usRcvLen,
                          UCHAR  *puchSndData,
                          USHORT *pusSndLen,
                          USHORT  usLockHnd); */
SHORT   OpPluDeleteFile();          /* V3.3, PEP ONLY */
SHORT   OpDeptDeleteFile();         /* V3.3, PEP ONLY */
SHORT   OpCpnDeleteFile();          /* V3.3, PEP ONLY */
SHORT   OpCstrDeleteFile();         /* V3.3, PEP ONLY */
SHORT   OpPpiDeleteFile();          /* V3.3, PEP ONLY */
SHORT   OpMldDeleteFile();          /* V3.3, PEP ONLY */

/*** NCR2172 (NWOP.c) ***/
SHORT   OpDeptDelete(DEPTIF *pPif, USHORT usLockHnd);
SHORT   OpDeptFileUpdate(USHORT usLockHnd);
SHORT   OpDeptAllRead(DEPTIF *pDpi, USHORT usLockHnd);
SHORT   OpConvertPluError(USHORT usError);
SHORT   OpOepCreatFile(ULONG ulNumberOfPlu, USHORT usLockHnd);

/*** NCR2172 (NWOPPEIN.C) ***/
SHORT   OpDeptCreatFile(USHORT usNumberOfDept, USHORT usLockHnd);

SHORT   OpPluMigrate34File(VOID);   /* PEP only (migrate), Saratoga */



#endif

/* ======= End of csop.h  ==========*/