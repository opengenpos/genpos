/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993-1998      ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server OPERATIONAL PARAMATER , Header file for internal                        
*   Category           : Client/Server OPERATIONAL PARAMATER  module, HOSPITAL FOR US                                                 
*   Program Name       : CSOPv34.H                                            
*  ------------------------------------------------------------------------  
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.              
*   Memory Model       : Medium Model
*   Options            :  /c /AM /G1s /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*   Update Histories  :
*          
*   Date      :Ver.Rev :NAME       :Description
*
*   Jan-31-00 :        :K.Yannagida:Saratoga Initial
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

/*=======================================================================*\
:   define
\*=======================================================================*/

#if		(_WIN32_WCE || WIN32) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

#define OP_PLU_CONT_OTHER_HGV210 10
/*=======================================================================*\
:   struct
\*=======================================================================*/

/*** V3.3/V3.4 Struct ***/

typedef	struct _QUEUE_HGV210 {						/* queue array					*/
	ULONG	ulQueFirst;							/* a first queued cell		*//* ### MOD 2172 Rel1.0 (100,000item)*/
	ULONG	ulQueLast;							/* a bottom queued cell		*//* ### MOD 2172 Rel1.0 (100,000item)*/
	ULONG	ulQueCount;							/* no. of queues			*//* ### MOD 2172 Rel1.0 (100,000item)*/
} QUEUE_HGV210, *PQUEUE_HGV210;

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
}PLU_CONTROL_HGV210;

typedef	struct _RECCNT {					/* record control field			*/
	UCHAR	fcInfo;								/* record information		*/
	ULONG	ulNext;								/* chain to next rec.		*//* ### MOD 2172 Rel1.0 (100,000item)*/
} RECCNT, *PRECCNT;

typedef    struct {
    PLU_CONTROL_HGV210    ContPlu;
    WCHAR   auchPluName[OP_PLU_NAME_SIZE + 1];
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
//	USHORT  usExtraSpace;
	USHORT	usTareInformation;				//USHORT used for High bit will be an identifier, and the low bit will be the number JHHJ
}OPPLU_PARAENTRY_HGV210;

typedef  struct {
   USHORT   auchPluNo[OP_PLU_LEN];  /* Saratoga */
   UCHAR    uchAdjective;
} OPPLU_INDEXENTRY_HGV210;

typedef    struct {
    WCHAR   auchPluNo[OP_PLU_LEN];      /* for temporary */
    UCHAR   uchPluAdj;
    ULONG   ulCounter;
    USHORT  husHandle;  /* 2172 */
    USHORT  usAdjCo;    /* 2172 */
    OPPLU_PARAENTRY_HGV210    ParaPlu;
}PLUIF_HGV210;

/* --- file header --- */
typedef	struct _FILEHDR210 {					/* file header structure		*/
	ULONG	ulFileSize;							/* file size in byte		*/
	USHORT	usFileNumber;						/* file's unique number		*/
	USHORT	usFileType;							/* file type				*/
	ULONG	ulHashKey;							/* a key value for hashing	*//* ### MOD 2172 Rel1.0 (100,000item)*/
	ULONG	ulMaxRec;							/* no. of max. rec.			*//* ### MOD 2172 Rel1.0 (100,000item)*/
	ULONG	ulCurRec;							/* no. of cur. rec.			*//* ### MOD 2172 Rel1.0 (100,000item)*/
	USHORT	usRecLen;							/* record size in byte		*/
	ULONG	ulOffsetIndex;						/* offset to index table	*//* ### MOD 2172 Rel1.0 (100,000item)*/
	ULONG	ulBytesIndex;						/* bytes of index table		*//* ### MOD 2172 Rel1.0 (100,000item)*/
	ULONG	ulOffsetRecord;						/* offset to record cell	*//* ### MOD 2172 Rel1.0 (100,000item)*/
	ULONG	ulBytesRecord;						/* bytes of record area		*//* ### MOD 2172 Rel1.0 (100,000item)*/
	QUEUE_HGV210	queListMainte;						/* mainte. list queue		*/
	QUEUE_HGV210	queReserved;						/* reserved queue			*/
	UCHAR	aucAdjust[10];						/* reserved (filled w/ 0)	*/
} FILEHDR_HGV210, *PFILEHDR_HGV210;

typedef struct _RECPLU210 {					/* format of PLU record			*/
    WCHAR   aucPluNumber[PLU_NUMBER_LEN];       /* PLU number               */
    UCHAR   uchAdjective;                       /* adjective number         */
    USHORT  usDeptNumber;                       /* department number        *//* ### MOD 2172 rel1.0 (UCHAR->USHORT)*/
    ULONG   ulUnitPrice[PLU_UNITPRICE_NUM];     /* unit price               *//* ### ADD 2172 Rel1.0 (01/17/00) */
    UCHAR   uchPriceMulWeight;                  /* price multiple / weight  */
    WCHAR   aucMnemonic[PLU_MNEMONIC_LEN];      /* mnemonics                */
    UCHAR   aucStatus[PLU_STATUS_LEN];          /* status                   */
	USHORT  usFamilyCode;                       /* family code              *//* ### ADD Saratoga FamilyCode (052900) */
    UCHAR   uchMixMatchTare;                    /* mix-match / tare code    */
    UCHAR   uchReportCode;                      /* report code              */
    USHORT  usLinkNumber;                       /* link number              */
    UCHAR   uchRestrict;                        /* restriction code         */
    USHORT	uchLinkPPI;                         /* link number to PPI       */	//changed to USHORT to match 210 files
    UCHAR   uchTableNumber;                     /* Table No for Order Prompt*/
    UCHAR   uchGroupNumber;                     /* Group No for Order Prompt*/
    UCHAR   uchPrintPriority;                   /* Print Priority           */
    UCHAR   uchItemType;                        /* item Type                *//* ### ADD 2172 Rel1.0(12.28.99)  */
	USHORT	usBonusIndex;
	// 2.1 Release Changes, SR167 - CSMALL
	ULONG	ulVoidTotalCounter;				// Void Total/Counter for each PLU
	ULONG	ulDiscountTotalCounter;			// Discount Total/Counter for each PLU
	ULONG	ulFreqShopperPoints;			// Frequent Shopper Discount/Points (GP Feature)
	UCHAR	auchAlphaNumRouting[4];			// Alphanumeric routing character for sending to KDS or Printers
	RECINV_TOTAL	futureTotals[10];			// basic inventory totals (on-hand, receipts/transfers, begin count,
											//   reorder count, current cost, original cost, etc.)
	WCHAR	auchAltPluName[PLU_MNEMONIC_LEN];	// Multilanguage mnemonic for each PLU
	UCHAR	uchColorPaletteCode;			// Color palette code for PLU
	UCHAR	auchStatus2[PLU_STATUS_LEN];
	USHORT	usTareInformation;
} RECPLU210, *PRECPLU210;

#if 0
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
/*** V3.1 Struct ***/

typedef  struct {
    UCHAR   uchDept;
    UCHAR   uchRept;
    UCHAR   auchContOther[OP_PLU_CONT_OTHER_V34];
}PLU_CONTROL_V34;

typedef    struct {
    PLU_CONTROL_OLD  ContPlu;
    UCHAR   auchPluName[OP_PLU_NAME_SIZE];
    UCHAR   auchPrice[OP_PLU_PRICE_SIZE];
    UCHAR   uchTableNumber;
    UCHAR   uchGroupNumber;
    UCHAR   uchPrintPriority;
}OPPLU_PARAENTRY_V34;

#endif
#endif

#define    WORK_SIZE               64
#define    OP_PLU_LEN_HGV210       14  		/*** NCR2172 ***/
/*=======================================================================*\
:   prototype
\*=======================================================================*/

#if defined(POSSIBLE_DEAD_CODE)
SHORT   OpPluAllRead_HGV210(PLUIF_HGV210*, USHORT, RECPLU210 *TempPlu);
SHORT   OpPluAssign_HGV210(PLUIF*, USHORT);
SHORT   Op_ReadPluFile_HGV210(ULONG, VOID*, ULONG);
SHORT   Op_ReadIndexPluFile_HGV210(ULONG  offulFileSeek, 
                           VOID   *pTableHeadAddress, 
                           ULONG usSizeofRead);
VOID    Op_WritePluFile_HGV210(ULONG, VOID*, USHORT);
SHORT   Op_GetPlu_HGV210(FILEHDR_HGV210*, ULONG, PLUIF_HGV210*,  RECPLU210 *TempPlu);
SHORT   OpPluMigrateHGV210File(VOID);
SHORT   Op_LockCheck_HGV210(USHORT);
SHORT OpPluAssignHGV210(RECPLU* recplu,PLUIF *pPif, USHORT usLockHnd);
SHORT   OpPluIndRead_HGV210(PLUIF *Pif, USHORT usLockHnd);
ULONG	ComputeHash_64HGV210(WCHAR *pNum, USHORT usLen,ULONG ulDiv);
USHORT	PhyReadRecord_HGV210(USHORT hf, ULONG ulCell, RECPLU210 *pucBuffer, FILEHDR_HGV210 *pHeader);
#endif
static  WCHAR FARCONST  szPluIndex[] = WIDE("PARAMIDX2");  /* PLU index file,R20   */

#define GETDEPTNO(x)        ((unsigned char)((unsigned char)x & 0x3f))
#define GETPLUTYPE(x)       ((unsigned char)((unsigned char)x >> 6))
#define FOREVER 1
