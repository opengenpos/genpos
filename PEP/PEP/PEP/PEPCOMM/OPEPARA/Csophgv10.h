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

#define    OP_PLU_NAME_SIZE        20   
#define    OP_PLU_NAME_SIZE_HGV10  20   /* Saratoga */
#define    OP_PLU_NAME_SIZE_V34    12
#define    OP_PLU_NAME_SIZE_V31    12   /* Saratoga */

#define    OP_PLU_PRICE_SIZE        3
#define    OP_PLU_PRICE_SIZE_HGV10  3
#define    OP_PLU_PRICE_SIZE_V34    3

#define    OP_PLU_CONT_OTHER       12  
#define    OP_PLU_CONT_OTHER_HGV10 12  /* Send Remote Printer #5-8 (Saratoga) */
#define    OP_PLU_CONT_OTHER_V34    6   /* Send Remote Printer #5-8(R3.1) */
#define    OP_PLU_CONT_OTHER_V31    5

#define    OP_PLU_LEN_HGV10				13

#define    TYPE_OPEN             0x01   /* PLU type definition */
#define    TYPE_NONADJ           0x02   /* PLU type definition */
#define    TYPE_ONEADJ           0x03   /* PLU type definition */

#define    OPEN_PLU              0x40   /* PLU Code Check (Open Price)  */
#define    NON_PLU               0x80   /* PLU Code Check (Non Adj.)    */
#define    ONE_PLU               0xC0   /* PLU Code Check (One Adj.)    */

#define    WORK_SIZE               64

/*=======================================================================*\
:   struct
\*=======================================================================*/

/*** V3.3/V3.4 Struct ***/

typedef	struct _QUEUE_HGV10 {						/* queue array					*/
	ULONG	ulQueFirst;							/* a first queued cell		*//* ### MOD 2172 Rel1.0 (100,000item)*/
	ULONG	ulQueLast;							/* a bottom queued cell		*//* ### MOD 2172 Rel1.0 (100,000item)*/
	ULONG	ulQueCount;							/* no. of queues			*//* ### MOD 2172 Rel1.0 (100,000item)*/
} QUEUE_HGV10, *PQUEUE_HGV10;

typedef  struct {
/*    UCHAR   uchDept;  */
    UCHAR   uchDept;
    UCHAR   uchItemType;     /* Saratoga */
    UCHAR   uchRept;
    UCHAR   auchContOther[OP_PLU_CONT_OTHER];    //12
}PLU_CONTROL_HGV10;

typedef	struct _RECCNT {					/* record control field			*/
	UCHAR	fcInfo;								/* record information		*/
	ULONG	ulNext;								/* chain to next rec.		*//* ### MOD 2172 Rel1.0 (100,000item)*/
} RECCNT, *PRECCNT;

typedef    struct {
    PLU_CONTROL_HGV10  ContPlu;
    UCHAR            auchPluName[OP_PLU_NAME_SIZE_HGV10];
    UCHAR            auchPrice[OP_PLU_PRICE_SIZE_HGV10];
    UCHAR            uchTableNumber;
    UCHAR            uchGroupNumber;
    UCHAR            uchPrintPriority;
    UCHAR            uchLinkPPI;         /* Link PPI Code,   Release 3.1 */
    UCHAR   uchPM;
    UCHAR   uchRestrict;
    USHORT  usLinkNo;
	USHORT	usFamilyCode;	/* ### ADD Saratoga (V1_0.02)(051800) */
}OPPLU_PARAENTRY_HGV10;

typedef  struct {
   /*USHORT   usPluNo; */
   UCHAR   auchPluNo[OP_PLU_LEN_HGV10];  /* Saratoga */
   UCHAR    uchAdjective;
} OPPLU_INDEXENTRY_HGV10;

typedef    struct {
    /* USHORT  usPluNo; */
    UCHAR	auchPluNo[OP_PLU_LEN_HGV10];  /* for temporary *//* Saratoga */
    UCHAR                  uchPluAdj;
    ULONG                  ulCounter;
    USHORT  husHandle;                  /* 2172 */
    USHORT  usAdjCo;                    /* 2172 */
    OPPLU_PARAENTRY_HGV10    ParaPlu;
}PLUIF_HGV10;

/* --- file header --- */
typedef	struct _FILEHDR_HGV10 {					/* file header structure		*/
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
	QUEUE_HGV10	queListMainte;						/* mainte. list queue		*/
	QUEUE_HGV10	queReserved;						/* reserved queue			*/
	UCHAR	aucAdjust[10];						/* reserved (filled w/ 0)	*/
} FILEHDR_HGV10, *PFILEHDR_HGV10;

typedef struct _RECPLU10 {					/* format of PLU record			*/
	UCHAR	aucPluNumber[OP_PLU_LEN_HGV10];		/* PLU number				*/
	UCHAR	uchAdjective;						/* adjective number			*/
	USHORT	usDeptNumber;						/* department number		*//* ### MOD 2172 rel1.0 (UCHAR->USHORT)*/
	ULONG	ulUnitPrice[PLU_UNITPRICE_NUM];		/* unit price				*//* ### ADD 2172 Rel1.0 (01/17/00) */
	UCHAR	uchPriceMulWeight;					/* price multiple / weight	*/
	UCHAR	aucMnemonic[PLU_MNEMONIC_LEN];		/* mnemonics				*/
	UCHAR	aucStatus[PLU_STATUS_LEN];			/* status					*/
    USHORT  usFamilyCode;                       /* family code              *//* add 05/13/00 */
	UCHAR	uchMixMatchTare;					/* mix-match / tare code	*/
	UCHAR	uchReportCode;						/* report code				*/
	USHORT	usLinkNumber;						/* link number				*/
	UCHAR	uchRestrict;						/* restriction code			*/
	UCHAR	uchLinkPPI;							/* link number to PPI		*/
	UCHAR	uchTableNumber;						/* Table No for Order Prompt*/
	UCHAR	uchGroupNumber;						/* Group No for Order Prompt*/
	UCHAR	uchPrintPriority;					/* Print Priority			*/
	/*UCHAR	uchBonusStatus;*/					/* bonus total index/status, R2.0 *//* ### DEL 2172 Rel1.0 */
	UCHAR	uchItemType;						/* item Type				*//* ### ADD 2172 Rel1.0(12.28.99)	*/
} RECPLU10, *PRECPLU10;

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

/*=======================================================================*\
:   prototype
\*=======================================================================*/


SHORT   OpPluAllRead_HGV10(PLUIF_HGV10*, USHORT, RECPLU10 *TempPlu);
SHORT   OpPluAssign_HGV10(PLUIF*, USHORT);
SHORT   Op_ReadPluFile_HGV10(ULONG, VOID*, ULONG);
SHORT   Op_ReadIndexPluFile_HGV10(ULONG  offulFileSeek, 
                           VOID   *pTableHeadAddress, 
                           ULONG usSizeofRead);
VOID    Op_WritePluFile_HGV10(ULONG, VOID*, USHORT);
SHORT   Op_GetPlu_HGV10(FILEHDR_HGV10*, ULONG, PLUIF_HGV10*,  RECPLU10 *TempPlu);
SHORT   OpPluMigrateHGV10File(VOID);
SHORT   Op_LockCheck_HGV10(USHORT);
SHORT OpPluAssignHGV10(RECPLU* recplu,PLUIF *pPif, USHORT usLockHnd);
SHORT   OpPluIndRead_HGV10(PLUIF *Pif, USHORT usLockHnd);
ULONG	ComputeHash_64HGV10(UCHAR *pNum, USHORT usLen,ULONG ulDiv);
USHORT	PhyReadRecord_HGV10(USHORT hf, ULONG ulCell, RECPLU10 *pucBuffer, FILEHDR_HGV10 *pHeader);
static  WCHAR FARCONST  szPluIndex[] = WIDE("PARAMIDX2");  /* PLU index file,R20   */

#define GETDEPTNO(x)        ((unsigned char)((unsigned char)x & 0x3f))
#define GETPLUTYPE(x)       ((unsigned char)((unsigned char)x >> 6))
#define FOREVER 1
