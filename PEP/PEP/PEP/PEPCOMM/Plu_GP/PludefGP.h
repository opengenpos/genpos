/****************************************************************************\
||																			||
||		  *=*=*=*=*=*=*=*=*													||
||        *  NCR 2170     *             NCR Corporation, E&M OISO			||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1994				||
||    <|\/~               ~\/|>												||
||   _/^\_                 _/^\_											||
||																			||
\****************************************************************************/

/*==========================================================================*\
*	Title:
*	Category:
*	Program Name:
* ---------------------------------------------------------------------------
*	Compile:		MS-C Ver. 6.00 A by Microsoft Corp.              
*	Memory Model:	Medium Model
*	Options:		/c /AM /Gs /Os /Za /Zp /W4
* ---------------------------------------------------------------------------
*	Abstract:		A local header for Mass Memory Module
*
* ---------------------------------------------------------------------------
*	Update Histories:
* ---------------------------------------------------------------------------
*	Date     | Version  | Descriptions							| By
* ---------------------------------------------------------------------------
*			 |			|										|
\*==========================================================================*/

/*==========================================================================*\
:	PVCS ENTRY
:-------------------------------------------------------------------------
:	$Revision$
:	$Date$
:	$Author$
:	$Log$
\*==========================================================================*/

/*==========================================================================*\
;+																			+
;+					L O C A L    D E F I N I T I O N s						+
;+                                                                      	+
\*==========================================================================*/

/*  RJC - pragma pack missing but similar structs in pludefin.h in PLU are packed*/
#if		(_WIN32_WCE || WIN32) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

#define		SMARTALG						/* use smart algorithm			*/

/* --- some useful language definitions --- */

#define     FOREVER         1				/* forever loop					*/
#define		TBLEND		(USHORT)(-1)		/* table end delimiter			*/
											/* structure member's offset	*/
#define		FAROF(member, type)		(USHORT)(&(((type *)NULL)->member))		
											/* structure member's size		*/
#define		LONGOF(member, type)	sizeof(((type *)NULL)->member)
											/* which is max. data ?			*/
#define		MAXOF(x, y)			(((x) > (y)) ? (x) : (y))
											/* which is min. data ?			*/
#define		MINOF(x, y)			(((x) < (y)) ? (x) : (y))
											/* change endianness			*/
#define		EXCHANGE(x)			(((USHORT)(x) >> 8) + ((USHORT)(x) << 8))
											/* no. of structure arrays		*/
#define		ARRAYS(instance, type)	(USHORT)(sizeof(instance) / sizeof(type))

/* --- basic data types --- */

#define		TYPBCD				1				/* packed BCD data			*/
#define		TYPBIN				2				/* binary data				*/
#define		TYPUCH				3				/* UCHAR data				*/
#define		TYPUSH				4				/* USHORT data				*/
#define		TYPULG				5				/* ULONG data				*/
#define		TYPLNG				6				/* LONG data				*/

/* --- definition of record dictionary --- */

typedef	struct	_RECDIC {					/* struct. of record dictinary	*/
	UCHAR	uchField;							/* field number				*/
	UCHAR	uchType;							/* data type				*/
	USHORT	usOffset;							/* data offset				*/
	USHORT	usLength;							/* data length				*/
} RECDIC, FAR *PRECDIC;

/* --- key record information table --- */

typedef	struct	_KEYINF {					/* key record information array	*/
	PRECDIC	pDictionary;						/* ptr. to record dictinary	*/
	USHORT	usKeyLen;							/* bytes of key data		*/
	USHORT	usKeyType;							/* data type of main key	*/
	USHORT	usSubKeyOffset;						/* offset of sub key		*/
	USHORT	usSubKeyType;						/* data type of sub key		*/
	USHORT	usBytesRec;							/* bytes per record			*/
	USHORT	usBytesSys;							/* bytes for system control	*/
} KEYINF, FAR *PKEYINF;

/* --- file information table --- */

typedef	struct	_INFTBL	{					/* file information array		*/
	USHORT		usFileNo;						/* file no.					*/
	USHORT		usFileType;						/* file type				*/
	USHORT		*pusHandle;						/* ptr. to access handle	*/
	WCHAR FAR	*pszFileName;					/* ptr. to file name		*/
	PKEYINF		pKeyInfo;						/* ptr. to key information	*/
} INFTBL, FAR *PINFTBL;

/* --- queue structure --- */

typedef	struct _QUEUE {						/* queue array					*/
	USHORT	usQueFirst;							/* a first queued cell		*/
	USHORT	usQueLast;							/* a bottom queued cell		*/
	USHORT	usQueCount;							/* no. of queues			*/
} QUEUE, *PQUEUE;

/*==========================================================================*\
;+																			+
;+				U S E R    M A N A G E M E N T    T A B L E					+
;+                                                                      	+
\*==========================================================================*/

#define		MAX_USERS			   5		/* no. of max. users			*/

#define		UNT_SEARCH			  10		/* search value unit			*/
#define		MIN_SEARCH			  10		/* min. no. of search a record	*/
#define		DEF_SEARCH			  30		/* default no. of search		*/
#define		MAX_SEARCH			1000		/* max. no. of search a record	*/
#define		UNC_SEARCH		(USHORT)(-1)	/* unconditional search			*/

// See function ExeEnterCritMode() which uses this struct for operations.
// mmember aucFuncData must be at least twice the size of PLU number plus opcodes
// so aucFuncData must be at least twice STD_PLU_NUMBER_LEN * sizeof(WCHAR) plus 4 bytes.
typedef	struct _USRBLK {					/* user block array				*/
	UCHAR	fchControl;							/* control flags			*/
	UCHAR	uchFunc;							/* function number			*/
	USHORT	usFile;								/* file ID accessing		*/
	USHORT	usIndexFile;						/* index file ID accessing		*/
	USHORT	hManip;								/* rec. manipulating handle	*/
	ITEMNO	itemNumber;							/* item no. being locked	*/
	USHORT	usAdjective;						/* adject. no. being locked	*/
    WCHAR   uchDeptNumber;                      /* dept. no for index file R2.0 */
	USHORT	usBatch;							/* batch no. for mainte.	*/
	USHORT	fsOption;							/* function optional data	*/
	USHORT	usSearch;							/* no. of search			*/
	USHORT	usLength;							/* bytes of optional data	*/
	WCHAR	aucFuncData[64];					/* function optional data	*/
} USRBLK, *PUSRBLK;

#define		USRBLK_IN_USE		0x80		/* in use flag					*/
#define		USRBLK_KILLED		0x40		/* kill by other users			*/
#define		USRBLK_REC_LOCKED	0x01		/* lock against rec.			*/
#define		USRBLK_PROH_READ	0x04		/* file lock against reading	*/
#define		USRBLK_PROH_WRITE	0x08		/* file lock against writing	*/

#define		FILE_READABLE		0x01		/* option to test the readable	*/
#define		FILE_WRITABLE		0x02		/* option to test the writable	*/

/*==========================================================================*\
;+																			+
;+					F I L E    S T R U C T U R E s							+
;+																			+
\*==========================================================================*/

/* --- file spec. structure --- */

typedef	struct	_FSPECS {					/* file spec. structure			*/
	USHORT	usFileNo;							/* file number				*/
	ULONG	ulFileSize;							/* file size in byte		*/
	USHORT	usMaxRec;							/* no. of max. rec.			*/
	USHORT	usCurRec;							/* no. of cur. rec.			*/
	USHORT	usRecLen;							/* record size in byte		*/
	USHORT	usListMainte;						/* no. of mainte. records	*/
} FSPECS, *PSPECS;

/* --- file header --- */

typedef	struct _FILEHDR {					/* file header structure		*/
	ULONG	ulFileSize;							/* file size in byte		*/
	USHORT	usFileNumber;						/* file's unique number		*/
	USHORT	usFileType;							/* file type				*/
	USHORT	usHashKey;							/* a key value for hashing	*/
	USHORT	usMaxRec;							/* no. of max. rec.			*/
	USHORT	usCurRec;							/* no. of cur. rec.			*/
	USHORT	usRecLen;							/* record size in byte		*/
	USHORT	usOffsetIndex;						/* offset to index table	*/
	USHORT	usBytesIndex;						/* bytes of index table		*/
	USHORT	usOffsetRecord;						/* offset to record cell	*/
	ULONG	ulBytesRecord;						/* bytes of record area		*/
	QUEUE	queListMainte;						/* mainte. list queue		*/
	QUEUE	queReserved;						/* reserved queue			*/
	UCHAR	aucAdjust[10];						/* reserved (filled w/ 0)	*/
} FILEHDR, *PFILEHDR;

#define		TYPE_HASH			1			/* hash file					*/
#define		TYPE_RELATIVE		2			/* relative file				*/
#define		TYPE_MAINTE			3			/* maintenance file				*/
#define		TYPE_INDEX			4			/* index file, R2.0				*/

typedef	struct _RECCNT {					/* record control field			*/
	UCHAR	fcInfo;								/* record information		*/
	USHORT	usNext;								/* chain to next rec.		*/
} RECCNT, *PRECCNT;

#define		REC_LEGAL_POS		0x80		/* located in legal position	*/
#define		REC_LINK_NEXT		0x40		/* link to next record			*/

typedef	struct _MNTCNT {					/* mainte. record control field	*/
	USHORT	usPrev;								/* chain to prev rec.		*/
	USHORT	usNext;								/* chain to next rec.		*/
} MNTCNT, *PMNTCNT;

#define		MNT_NO_LINKED	(USHORT)(-1)	/* no linking address			*/

/* --- calculation for offset of Index Table --- */

typedef		ULONG				INDEX;		/* data type of infomation		*/

#define		MSB					0x80		/* MSB bit pattern				*/
#define		LSB					0x01		/* LSB bit pattern				*/
											/* compute byte offset			*/
#define		BYTEFROMINDEX(x)	(USHORT)((INDEX)x >> 16)
											/* compute bit number			*/
#define		BITNFROMINDEX(x)	(USHORT)(((INDEX)x & 0xFFFF) >> 8)
											/* compute bit pattern			*/
#define		PTRNFROMINDEX(x)	(UCHAR)((INDEX)x)

/* --- buffer size for disk accessing --- */

#define		SIZEDISKIO			1024		/* buffer size for disk IO		*/

//#define		ENOUGHREC		100			/* enough size for record		*/
#define		ENOUGHREC			sizeof(RECPLU) // PLU Color Enhancement - CSMALL
/* --- sequential access control table --- */

typedef	struct	_SEQMNP {					/* sequential access control	*/
	UCHAR	fchFlags;							/* control flags			*/
	USHORT	usFileNo;							/* access file no.			*/
	USHORT	usIndexFileNo;						/* access index file no. R2.0*/
    USHORT  usFileVer;                          /* file version, R2.0 */
	union {										/* file position control	*/
		struct {									/* seq. rec. read access*/
			USHORT	usHashNo;							/* original hash/index	*/
			USHORT	usCellNo;							/* accessing cell no*/
		};
		ULONG	ulOffset;							/* block read access	*/
	};
} SEQMNP, *PSEQMNP;

#define		SEQMNP_IN_USE		0x01		/* in use flag					*/
#define		SEQMNP_INITIAL		0x02		/* initial calling				*/
#define		SEQMNP_VALID_NO		0x04		/* store valid cell no.			*/
#define		SEQMNP_READABLE		0x08		/* accessable cell no.			*/

#define		DEFAULT_HANDLES		  2			/* no. of default handles		*/
#define		HMNP_REC_MAIN		101			/* default handle for rec. 1	*/
#define		HMNP_REC_SUB		102			/* default handle for rec. 2	*/

/* --- index file save area for mirror control R2.0 --- */

typedef struct _MIRRORIDX {                 /* mirror control field         */
    USHORT  usPluFile;                      /* PLU file id save area        */
    USHORT  usIndexFile;                    /* index file id save area      */
} MIRRORIDX, *PMIRRORIDX;

/* ---- file version definition for migration, R2.0 ---- */

#define     PLU_FILE_R10          1         /* plu r1.0 version */


/*==========================================================================*\
;+																			+
;+			P R O T O T Y P E     D E C L A R A T I O N s					+
;+																			+
\*==========================================================================*/

/* --- common functions from PLUAPPLI.C --- */

VOID	ExeInitialize_GP(BOOL);
VOID	ExeFinalize_GP(VOID);
USHORT	ExeCreateFile_GP(USHORT, USHORT);
USHORT	ExeDeleteFile_GP(USHORT);
USHORT	ExeClearFile_GP(USHORT);
USHORT	ExeSenseFile_GP(USHORT, PMFINFO);
USHORT	ExeOpenMirror_GP(WCHAR FAR *, WCHAR FAR *, USHORT *);
USHORT	ExeCloseMirror_GP(USHORT);
USHORT	ExeEnterCritMode_GP(USHORT, USHORT, USHORT *, VOID *);
USHORT	ExeExitCritMode_GP(USHORT, USHORT);
USHORT	ExeKillCritMode_GP(USHORT, USHORT);
USHORT	ExeAddRecord_GP(USHORT, VOID *);
USHORT	ExeAddRecordIn_GP(USHORT, VOID *);
USHORT	ExeReplaceRecord_GP(USHORT, VOID *, VOID *, USHORT *);
USHORT	ExeReplaceRecordIn_GP(USHORT, VOID *, VOID *, WCHAR *, USHORT *);
USHORT	ExeChangeRecord_GP(USHORT, VOID *, ULONG, USHORT, VOID *, USHORT *);
USHORT	ExeChangeRecordIn_GP(USHORT, VOID *, ULONG, USHORT, VOID *, WCHAR *, USHORT *);
USHORT	ExeDeleteRecord_GP(USHORT, VOID *, USHORT, USHORT, VOID *, USHORT *);
USHORT	ExeDeleteRecordIn_GP(USHORT, VOID *, USHORT, USHORT, VOID *, WCHAR *, USHORT *);
USHORT	ExeDeleteReserved_GP(USHORT);
USHORT	ExeResetReserved_GP(USHORT);
USHORT	ExeRewriteReserved_GP(USHORT, VOID *);
USHORT	ExeCancelReserved_GP(USHORT);
USHORT	ExeAppendMainte_GP(USHORT, VOID *);
USHORT	ExePurgeMainte_GP(USHORT);
USHORT	ExeApplyMainte_GP(USHORT, VOID *, USHORT *, VOID *, USHORT *, USHORT *);
USHORT	ExeRemoveMainte_GP(USHORT, USHORT);
USHORT	ExeModifyMainte_GP(USHORT, USHORT, VOID *);
USHORT	ExeReadMainte_GP(USHORT, USHORT, VOID *, USHORT *);
USHORT	ExeAddMainte_GP(USHORT, VOID *, USHORT *);
USHORT	ExeReadRecord_GP(USHORT, PITEMNO, USHORT, VOID *, USHORT *);
USHORT	ExeReserveRecord_GP(USHORT, PITEMNO, USHORT, VOID *, USHORT *);
USHORT	ExeReportRecord_GP(USHORT, VOID *, USHORT *, USHORT *);
USHORT	ExeReadBlock_GP(USHORT, WCHAR *, USHORT, USHORT *);
USHORT	ExeWriteBlock_GP(USHORT, WCHAR *, USHORT, USHORT *);
USHORT	ExeSeekBlock_GP(USHORT, LONG, USHORT, ULONG *);
USHORT	ExeUpdateRecord_GP(USHORT, PITEMNO, USHORT,
										PUPDINF, PUPDINF, VOID *, USHORT *);
USHORT	ExeInformStatus_GP(USHORT, PLU_STATUS *);          /* R1.1 */
USHORT  ExeSetIndexFileId_GP(USHORT usFile);

/* --- common functions from PLUMANIP.C --- */

USHORT	MnpInitialize_GP(BOOL);
USHORT	MnpFinalize_GP(VOID);
USHORT	MnpCreateFile_GP(USHORT, USHORT);
USHORT	MnpDeleteFile_GP(USHORT);
USHORT	MnpClearFile_GP(USHORT);
USHORT	MnpGetFileInfo_GP(USHORT, PSPECS);
USHORT	MnpAddRecord_GP(USHORT, VOID *, USHORT *);
USHORT	MnpDeleteRecord_GP(USHORT, WCHAR *, USHORT);
USHORT	MnpReadRecord_GP(USHORT, USHORT, WCHAR *, USHORT, WCHAR *);
USHORT	MnpBeginLookUp_GP(USHORT, USHORT *, USHORT *, USHORT);
USHORT	MnpEndLookUp_GP(USHORT);
USHORT	MnpReadFile_GP(USHORT, WCHAR *, USHORT, USHORT *);
USHORT	MnpWriteFile_GP(USHORT, WCHAR *, USHORT, USHORT *);
USHORT	MnpSeekFile_GP(USHORT, LONG, USHORT, ULONG *);
USHORT	MnpGetLookUp_GP(USHORT, WCHAR *);
USHORT	MnpModifyRecord_GP(USHORT, VOID *);
USHORT	MnpPeekRecord_GP(USHORT, VOID *);
USHORT	MnpRemoveRecord_GP(USHORT);
USHORT	MnpKeepFilePtr_GP(USHORT);
USHORT	MnpMoveFilePtr_GP(USHORT, VOID *);
USHORT	MnpGetFilePtr_GP(USHORT, USHORT *);
USHORT	MnpSetFilePtr_GP(USHORT, USHORT);
USHORT	MnpQInfoTable_GP(USHORT, PKEYINF *);
USHORT	MnpOpenMirror_GP(WCHAR *, USHORT *);
USHORT	MnpCloseMirror_GP(USHORT);
BOOL	MnpIsMirrorFile_GP(USHORT);
USHORT	MnpFileOfMirror_GP(USHORT);

/* --- common functions from PLUPHYSC.C --- */

USHORT	PhyOpenFile_GP(WCHAR *, USHORT *, BOOL);
USHORT	PhyCreateFile_GP(WCHAR *, USHORT *, USHORT, USHORT, USHORT, USHORT);
USHORT	PhyDeleteFile_GP(WCHAR *);
USHORT	PhyClearFile_GP(USHORT);
USHORT	PhyOpenFile_GP(WCHAR *, USHORT *, BOOL);
USHORT	PhyAuditFile_GP(USHORT);
USHORT	PhyReadFile_GP(USHORT, WCHAR *, ULONG *, ULONG);
USHORT	PhyWriteFile_GP(USHORT, WCHAR *, ULONG *, ULONG);
USHORT	PhyCloseFile_GP(USHORT);
USHORT	PhyGetInfo_GP(USHORT, PFILEHDR);
USHORT	PhyUpdateInfo_GP(USHORT, PFILEHDR);
USHORT	PhyReadRecord_GP(USHORT, USHORT, WCHAR *, PFILEHDR);
USHORT	PhyWriteRecord_GP(USHORT, USHORT, WCHAR *, PFILEHDR);
USHORT	PhyExamOccupied_GP(USHORT, USHORT, PFILEHDR, BOOL *);
USHORT	PhySetUpOccupied_GP(USHORT, USHORT, PFILEHDR, BOOL);
USHORT	PhySearchVacant_GP(USHORT, USHORT, PFILEHDR, USHORT *);
USHORT	PhySearchOccupied_GP(USHORT, USHORT, PFILEHDR, USHORT *);

#if		(_WIN32_WCE || WIN32) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

