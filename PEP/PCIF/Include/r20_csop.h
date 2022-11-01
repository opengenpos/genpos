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
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.
*   Memory Model       : Midium Model
*   Options            :  /c /AM /Gs /Os /Za /Zp /W4
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
*   Jan-25-00 : 01.00.00 :K.Yanagida :Saratoga initial
*
\*=======================================================================*/
/*=======================================================================*\
:   PVCS ENTRY
:-------------------------------------------------------------------------
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

/*
===========================================================================
    DEFINE
===========================================================================
*/

#define    OP_PLU_LEN              STD_PLU_NUMBER_LEN
#define    OP_PLU_NAME_SIZE        20   /* Saratoga */
#define    OP_PLU_PRICE_SIZE        3
#define    OP_PLU_CONT_OTHER       10      /* Saratoga */
#define    OP_DEPT_CONT_CODE_SIZE   5
#define    OP_DEPT_NAME_SIZE       20    /* 2172 */
#define    OP_DEPT_HALO_SIZE        1
#define    OP_CPN_STS_SIZE          2
#define    OP_CPN_NAME_SIZE        STD_CPNNAME_LEN
#define    OP_CPN_TARGET_SIZE       7
#define    OP_MAX_WORK_BUFFER    1024   /* saratoga */
#define    OP_MAX_WORK2_BUFFER   1500
#define    OP_NUMBER_OF_MAX_PLU  500000 /* Saratoga should be same as FLEX_PLU_MAX */
/* Add 2172 Rel 1.0 */
#define    OP_NUMBER_OF_MAX_OEP  8000
#define    OP_MAX_DEPT_NO        9999
#define    OP_MAX_REPORT_NO        10
#define    OP_MAX_GROUP_NO         99
#define    OP_CSTR_INDEX_SIZE     400
#define    OP_CSTR_PARA_SIZE      200
/* === New Elements - Promotional Pricing Item (Release 3.1) BEGIN === */
#define    OP_MAX_PPI_NO          300   /* Promotional Pricing Item, R3.1 */
#define    OP_MAX_PPI_LEVEL        40   /* Price Level,              R3.1 */ /* 20->40 for PPI Enhancement - CSMALL*/
/* === New Elements - Promotional Pricing Item (Release 3.1) END === */
#define    OP_MLD_MNEMONICS_LEN    32   /* Mld Mnemonis Lenggh,      V3.3 */

#define    OP_OEP_PLU_SIZE         20   /* 2172 */

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
#define  OP_NEW_PLU            -11
#define  OP_CHANGE_DEPT        OP_CHANGE_PLU
#define  OP_NEW_DEPT           OP_NEW_PLU
/* Add 2172 Rel 1.0 */
#define  OP_CHANGE_OEP         OP_CHANGE_PLU
/* Add 2172 Rel 1.0 */
#define  OP_NEW_OEP            OP_NEW_PLU
#define  OP_DELETE_CSTR_FILE   -16
#define  OP_DELETE_PLU_FILE    -17
#define  OP_DELETE_DEPT_FILE   -18
#define  OP_DELETE_CPN_FILE    -19
/* === New Functions - Promotional Pricing Item (Release 3.1) BEGIN === */
#define  OP_CHANGE_PPI         OP_CHANGE_PLU    /* change PPI Record.  R3.1 */
#define  OP_NEW_PPI            OP_NEW_PLU       /* Add new PPI Record, R3.1 */
#define  OP_DELETE_PPI_FILE    -15              /* delete PPI File,    R3.1 */
#define  OP_ABNORMAL_PPI       -16              /* ppi code is bad,    R3.1 */
/* === New Functions - Promotional Pricing Item (Release 3.1) END === */
#define  OP_CHANGE_MLD         OP_CHANGE_PLU    /* change Mld Record.  V3.3 */
#define  OP_NEW_MLD            OP_NEW_PLU       /* Add new Mld Record, V3.3 */
#define  OP_DELETE_MLD_FILE    -21              /* delete Mld File,    V3.3 */
#define  OP_ABNORMAL_MLD       -22              /* Mld code is bad,    V3.3 */

#define  OP_NOT_MASTER         -20
/* Add 2172 Rel1.0 */
#define  OP_PLU_ERROR          -23
#define  OP_DELETE_OEP_FILE    -24              /* delete OEP File,    saratoga */
#define  OP_TEST			   -25				// Used for Testing (when data fields recieved as 0)   ***PDINU

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
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

typedef struct {
	LONG lAmount;
	SHORT sCounter;
}INV_TOTAL;// SR167 - CSMALL

typedef  struct {
    USHORT  usDept;          /* for temporary */
    UCHAR   uchItemType;
    UCHAR   uchRept;
    UCHAR   auchContOther[OP_PLU_CONT_OTHER];
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
    WCHAR   auchPluName[OP_PLU_NAME_SIZE];
    UCHAR   auchPrice[OP_PLU_PRICE_SIZE];
    UCHAR   uchTableNumber;
    UCHAR   uchGroupNumber;
    UCHAR   uchPrintPriority;
    /* === New Elements (Release 3.1) BEGIN === */
    USHORT   uchLinkPPI;         /* Link PPI Code,   Release 3.1 */
    /* === New Elements (Release 3.1) END === */
    UCHAR   uchPM;
    UCHAR   uchRestrict;
    USHORT  usLinkNo;
    USHORT  usFamilyCode;   /* ### ADD Saratoga FamilyCod (052900) */
	// 2.1 Release Changes, SR167 - CSMALL
	ULONG	ulVoidTotalCounter;				// Void Total/Counter for each PLU
	ULONG	ulDiscountTotalCounter;			// Discount Total/Counter for each PLU
	ULONG	ulFreqShopperPoints;			// Frequent Shopper Discount/Points (GP Feature)
	UCHAR	auchAlphaNumRouting[4];			// Alphanumeric routing character for sending to KDS or Printers
	INV_TOTAL	futureTotals[10];			// basic inventory totals (on-hand, receipts/transfers, begin count,
											//   reorder count, current cost, original cost, etc.)
	WCHAR	auchAltPluName[OP_PLU_NAME_SIZE + 1];	// Multilanguage mnemonic for each PLU
	UCHAR	uchColorPaletteCode;			// Color pallete code for PLU
	UCHAR	auchStatus2[PLU_STATUS_LEN];
	USHORT  usExtraSpace;
	USHORT	usTareInformation;				//USHORT used for High bit will be an identifier, and the low bit will be the number JHHJ
	UCHAR   uchExtendedGroupPriority[12];      // extended group numbers, first byte is group number, second byte is priority within group
	UCHAR   uchPluOptionFlags[6];                 // bit map for option flags
}OPPLU_PARAENTRY;

/* 2172 Flexible Dept */

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
	UCHAR	ucExtra1[10];	// extra space for future use
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
    ULONG   ulCounter;
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

typedef    struct {
    UCHAR    uchMajorDeptNo;
    OPDEPT_PARAENTRY    ParaDept;
    USHORT    usDeptNo;
    ULONG    ulCounter;
}MDEPTIF;


typedef    struct {
    UCHAR    uchReport;
    ULONG    culCounter;
    WCHAR   auchPluNo[OP_PLU_LEN];      /* for temporary */
    UCHAR    uchPluAdj;
    USHORT   usAdjCo;   /* 2172 */
    USHORT   husHandle; /* 2172 */
    OPPLU_PARAENTRY    ParaPlu;
} PLUIF_REP;

typedef    struct {
    USHORT   usDept;                    /* for temporary */
    ULONG    culCounter;
    WCHAR   auchPluNo[OP_PLU_LEN];      /* for temporary */
    UCHAR    uchPluAdj;
    USHORT   usAdjCo;   /* 2172 */
    USHORT   husHandle; /* 2172 */
    OPPLU_PARAENTRY    ParaPlu;
} PLUIF_DEP;


typedef    struct {
    WCHAR    auchPluNo[OP_PLU_LEN];     /* for temporary */
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
SHORT   OpOepCreatFile(ULONG ulNumberOfPlu, USHORT usLockHnd);
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
SHORT   OpDeptDelete(DEPTIF *pDpi, USHORT usLockHnd);           /* 2172 */
SHORT   OpDeptAllRead(DEPTIF *pDpi, USHORT usLockHnd);          /* 2172 */
SHORT   OpMajorDeptRead(MDEPTIF *pDpi, USHORT usLockHnd);
SHORT   OpPluAllRead(PLUIF *pPif, USHORT usLockHnd);
SHORT   OpRprtPluRead(PLUIF_REP *pPrp, USHORT usLockHnd);
SHORT   OpDeptPluRead(PLUIF_DEP *pDp, USHORT usLockHnd);
SHORT   OpOepPluRead(PLUIF_OEP *pDp, USHORT usLockHnd);
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
/* Add 2172 Rel 1.0 */
USHORT   OpConvertPluError(USHORT usError);

/* === New Functions - Promotional Pricing Item (Release 3.1) BEGIN === */
SHORT   OpPpiCreatFile(USHORT usNumberOfPpi, USHORT usLockHnd);
SHORT   OpPpiCheckAndCreatFile(USHORT usNumberOfPpi, USHORT usLockHnd);
SHORT   OpPpiCheckAndDeleteFile(USHORT usNumberOfPpi, USHORT usLockHnd);
SHORT   OpPpiAssign(PPIIF *pPif, USHORT usLockHnd);
SHORT   OpPpiDelete(PPIIF *pPif, USHORT usLockHnd);
SHORT   OpPpiIndRead(PPIIF *pPif, USHORT usLockHnd);
SHORT   OpReqPpi(USHORT usFcCode, USHORT usLockHnd);
SHORT   OpResPpi(UCHAR  *puchRcvData,
                 USHORT  usRcvLen,
                 UCHAR  *puchSndData,
                 USHORT *pusSndLen,
                 USHORT  usLockHnd);
/* === New Functions - Promotional Pricing Item (Release 3.1) END === */
/* Mld Mnemonics File, V3.3 */
SHORT   OpMldCreatFile(USHORT usNumberOfAddress, USHORT usLockHnd);
SHORT   OpMldCheckAndCreatFile(USHORT usNumberOfAddress, USHORT usLockHnd);
SHORT   OpMldCheckAndDeleteFile(USHORT usNumberOfAddress, USHORT usLockHnd);
SHORT   OpMldAssign(MLDIF *pMld, USHORT usLockHnd);
SHORT   OpMldIndRead(MLDIF *pMld, USHORT usLockHnd);
SHORT   OpReqMld(USHORT usFcCode, USHORT usLockHnd);
SHORT   OpResMld(UCHAR  *puchRcvData,
                 USHORT  usRcvLen,
                 UCHAR  *puchSndData,
                 USHORT *pusSndLen,
                 USHORT  usLockHnd);

/*=========== End of header ================*/