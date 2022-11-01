/****************************************************************************\
||																			||
||		*=*=*=*=*=*=*=*=*													||
||		*	NCR 2170	*				NCR Corporation, E&M OISO			||
||	  @	*=*=*=*=*=*=*=*=*  0				(C) Copyright, 1994				||
||	 <|\/~				~\/|>												||
||	_/^\_				 _/^\_												||
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
*	Date		| Version	| Descriptions							| By
* ---------------------------------------------------------------------------
*				|			|										|
*** NCR2172
* 11/05/99	| 1.00.00	| Add #pragma(...)						| K.Iwata
* 01/25/00  | 1.00.00   | For PEP                               | hrkato
\*==========================================================================*/

/*==========================================================================*\
:	PVCS ENTRY
:-------------------------------------------------------------------------
:	$Revision$
:	$Date$
:	$Author$
:	$Log$
\*==========================================================================*/

/*  Saratoga  RJC - pragma pack was commented out but we seem to have alighnment errors*/
#if		(_WIN32_WCE || WIN32) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

/*==========================================================================*\
;+																			+
;+					L O C A L	D E F I N I T I O N s						+
;+																			+
\*==========================================================================*/

/*#define		SMARTALG*/						/* use smart algorithm		*/

/* --- assertion ---    Saratoga  */
#if		(_WIN32_WCE || WIN32) && _MSC_VER >= 800 && _DEBUG
	void	_PLU_ASSERT(char * pchProcName,char * pchMsg,int nVal);
	void	_PLU_TRACE(char * pchProcName,char * pchMsg,int nVal);
#else
	#define	_PLU_ASSERT(x,y,z)	((void)0)
	#define	_PLU_TRACE(x,y,z)	((void)0)
	#define	_PLU_BREAKPOINT(x)	((void)0)
#endif


/* --- some useful language definitions --- */

#define		FOREVER			1					/* forever loop				*/
#define		TBLEND		(USHORT)(-1)			/* table end delimiter		*/
												/* structure member's offset*/
#define		FAROF(member, type)		(USHORT)(&(((type *)NULL)->member))		
												/* structure member's size	*/
#define		LONGOF(member, type)	sizeof(((type *)NULL)->member)
												/* which is max. data ?		*/
#define		MAXOF(x, y)			(((x) > (y)) ? (x) : (y))
												/* which is min. data ?		*/
#define		MINOF(x, y)			(((x) < (y)) ? (x) : (y))
												/* change endianness		*/
#define		EXCHANGE(x)			(((USHORT)(x) >> 8) + ((USHORT)(x) << 8))
												/* no. of structure arrays	*/
#define		ARRAYS(instance, type)	(USHORT)(sizeof(instance) / sizeof(type))

/* --- basic data types --- */

#define		TYPBCD					1			/* packed BCD data			*/
#define		TYPBIN					2			/* binary data				*/
#define		TYPUCH					3			/* UCHAR data				*/
#define		TYPUSH					4			/* USHORT data				*/
#define		TYPULG					5			/* ULONG data				*/
#define		TYPLNG					6			/* LONG data				*/

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
	ULONG	ulQueFirst;							/* a first queued cell		*//* ### MOD 2172 Rel1.0 (100,000item)*/
	ULONG	ulQueLast;							/* a bottom queued cell		*//* ### MOD 2172 Rel1.0 (100,000item)*/
	ULONG	ulQueCount;							/* no. of queues			*//* ### MOD 2172 Rel1.0 (100,000item)*/
} QUEUE, *PQUEUE;

/*==========================================================================*\
;+																			+
;+				U S E R	M A N A G E M E N T	T A B L E						+
;+																			+
\*==========================================================================*/

#define		MAX_USERS				5			/* no. of max. users		*/

#define		UNT_SEARCH				10			/* search value unit		*/
#define		MIN_SEARCH				10			/* min. no. of search a record*/
#define		DEF_SEARCH				30			/* default no. of search	*/
#define		MAX_SEARCH				1000		/* max. no. of search a record*/
#define		UNC_SEARCH		(USHORT)(-1)		/* unconditional search		*/

// See function ExeEnterCritMode() which uses this struct for operations.
// mmember aucFuncData must be at least twice the size of PLU number plus opcodes
// so aucFuncData must be at least twice STD_PLU_NUMBER_LEN * sizeof(WCHAR) plus 4 bytes.
typedef	struct _USRBLK {					/* user block array				*/
	UCHAR	fchControl;							/* control flags			*/
	UCHAR	uchFunc;							/* function number			*/
	USHORT	usFile;								/* file ID accessing		*/
	USHORT	usIndexFile;						/* index file ID accessing	*/
	USHORT	hManip;								/* rec. manipulating handle	*/
	ITEMNO	itemNumber;							/* item no. being locked	*/
	USHORT	usAdjective;						/* adject. no. being locked	*/
	/*USHORT	usDeptNumber;*/					/* dept. no for index file R2.0 *//* ### 2172 Rel1.0 (UCHAR->USHORT)*/
	USHORT	usBatch;							/* batch no. for mainte.	*/
	USHORT	fsOption;							/* function optional data	*/
	USHORT	usSearch;							/* no. of search			*/
	USHORT	usLength;							/* bytes of optional data	*/
	WCHAR	aucFuncData[64];					/* function optional data	*/
} USRBLK, *PUSRBLK;

#define		USRBLK_IN_USE			0x80		/* in use flag				*/
#define		USRBLK_KILLED			0x40		/* kill by other users		*/
#define		USRBLK_REC_LOCKED		0x01		/* lock against rec.		*/
#define		USRBLK_PROH_READ		0x04		/* file lock against reading*/
#define		USRBLK_PROH_WRITE		0x08		/* file lock against writing*/

#define		FILE_READABLE			0x01		/* option to test the readable*/
#define		FILE_WRITABLE			0x02		/* option to test the writable*/

/*==========================================================================*\
;+																			+
;+					F I L E	S T R U C T U R E s								+
;+																			+
\*==========================================================================*/

/* --- file spec. structure --- */

typedef	struct	_FSPECS {					/* file spec. structure			*/
	USHORT	usFileNo;							/* file number				*/
	ULONG	ulFileSize;							/* file size in byte		*/
	ULONG	ulMaxRec;							/* no. of max. rec.			*//* ### MOD 2172 Rel1.0 (100,000item)*/
	ULONG	ulCurRec;							/* no. of cur. rec.			*//* ### MOD 2172 Rel1.0 (100,000item)*/
	USHORT	usRecLen;							/* record size in byte		*/
	ULONG	ulListMainte;						/* no. of mainte. records	*//* ### MOD 2172 Rel1.0 (100,000item)*/
} FSPECS, *PSPECS;

/* --- file header --- */

typedef	struct _FILEHDR {					/* file header structure		*/
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
	QUEUE	queListMainte;						/* mainte. list queue		*/
	QUEUE	queReserved;						/* reserved queue			*/
	UCHAR	aucAdjust[10];						/* reserved (filled w/ 0)	*/
} FILEHDR, *PFILEHDR;

#define		TYPE_HASH				1			/* hash file				*/
#define		TYPE_RELATIVE			2			/* relative file			*/
#define		TYPE_MAINTE				3			/* maintenance file			*/
#define		TYPE_INDEX				4			/* index file, R2.0			*/

typedef	struct _RECCNT {					/* record control field			*/
	UCHAR	fcInfo;								/* record information		*/
	ULONG	ulNext;								/* chain to next rec.		*//* ### MOD 2172 Rel1.0 (100,000item)*/
} RECCNT, *PRECCNT;

#define		REC_LEGAL_POS			0x80		/* located in legal position*/
#define		REC_LINK_NEXT			0x40		/* link to next record		*/

typedef	struct _MNTCNT {					/* mainte. record control field	*/
	ULONG	ulPrev;								/* chain to prev rec.		*//* ### MOD 2172 Rel1.0 (100,000item)*/
	ULONG	ulNext;								/* chain to next rec.		*//* ### MOD 2172 Rel1.0 (100,000item)*/
} MNTCNT, *PMNTCNT;

#define		MNT_NO_LINKED	(USHORT)(-1)		/* no linking address		*/

/* --- calculation for offset of Index Table --- */
#ifdef	_MOD_2172_R10	/* ### MOD 2172 Rel1.0 (100,000item)*/
typedef		ULONG				INDEX;			/* data type of infomation	*/
#define		BYTEFROMINDEX(x)	(USHORT)((INDEX)x >> 16)
												/* compute bit number		*/
#define		BITNFROMINDEX(x)	(USHORT)(((INDEX)x & 0xFFFF) >> 8)
												/* compute bit pattern		*/
#define		PTRNFROMINDEX(x)	(UCHAR)((INDEX)x)
												/* compute byte offset		*/
#endif	/* _MOD_2172_R10 */


typedef struct _INDEX {
	ULONG	ulOffset;							/* offset of Index Table	*/
	UCHAR	uchBitPattern;						/* bitpattern				*/
	USHORT	usBitNumber;						/* bit number				*/
}INDEX,* PINDEX;

#define	BYTEFROMINDEX(x) ((ULONG)x.ulOffset)	/* compute byte offset		*/
#define BITNFROMINDEX(x) ((USHORT)x.usBitNumber) /* compute bit number		*/
#define PTRNFROMINDEX(x) ((UCHAR)x.uchBitPattern) /* compute bit pattern	*/

#define		MSB						0x80		/* MSB bit pattern			*/
#define		LSB						0x01		/* LSB bit pattern			*/

/* --- buffer size for disk accessing --- */

#define		SIZEDISKIO				1024		/* buffer size for disk IO	*/

//#define		ENOUGHREC				100			/* enough size for record	*/
#define		ENOUGHREC				sizeof(RECPLU)	// PLU Color Enhancement - CSMALL

/* --- sequential access control table --- */

typedef	struct	_SEQMNP {					/* sequential access control	*/
	UCHAR	fchFlags;							/* control flags			*/
	USHORT	usFileNo;							/* access file no.			*/
	USHORT	usIndexFileNo;						/* access index file no. R2.0*/
	USHORT	usFileVer;							/* file version, R2.0		*/
	union {										/* file position control	*/
		struct {								/* seq. rec. read access	*/
			ULONG	ulHashNo;					/* original hash/index		*//* ### MOD 2172 Rel1.0 (100,000item)*/
			ULONG	ulCellNo;					/* accessing cell no		*//* ### MOD 2172 Rel1.0 (100,000item)*/
		};
		ULONG	ulOffset;	 					/* block read access		*//* ### DEL 2172 Rel1.0 (100,000item)*/
	};
} SEQMNP, *PSEQMNP;

#define		SEQMNP_IN_USE			0x01		/* in use flag				*/
#define		SEQMNP_INITIAL			0x02		/* initial calling			*/
#define		SEQMNP_VALID_NO			0x04		/* store valid cell no.		*/
#define		SEQMNP_READABLE			0x08		/* accessable cell no.		*/

#define		DEFAULT_HANDLES			2			/* no. of default handles	*/
#define		HMNP_REC_MAIN			101			/* default handle for rec. 1*/
#define		HMNP_REC_SUB			102			/* default handle for rec. 2*/

/* --- index file save area for mirror control R2.0 --- */

typedef struct _MIRRORIDX {					/* mirror control field			*/
	USHORT	usPluFile;							/* PLU file id save area	*/
	USHORT	usIndexFile;						/* index file id save area	*/
} MIRRORIDX, *PMIRRORIDX;

/* ---- file version definition for migration, R2.0 ---- */

#define		PLU_FILE_R10			1			/* plu r1.0 version			*/


/*==========================================================================*\
;+																			+
;+			P R O T O T Y P E		D E C L A R A T I O N s					+
;+																			+
\*==========================================================================*/

/* --- common functions from PLUAPPLI.C --- */

VOID	ExeInitialize(BOOL);
VOID	ExeFinalize(VOID);
USHORT	ExeCreateFile(USHORT, ULONG);
USHORT	ExeDeleteFile(USHORT);
USHORT	ExeClearFile(USHORT);
USHORT	ExeSenseFile(USHORT, PMFINFO);
USHORT	ExeOpenMirror(WCHAR FAR *, WCHAR FAR *, USHORT *);
USHORT	ExeCloseMirror(USHORT);
USHORT	ExeEnterCritMode(USHORT, USHORT, USHORT *, VOID *);
USHORT	ExeExitCritMode(USHORT, USHORT);
USHORT	ExeKillCritMode(USHORT, USHORT);
USHORT	ExeAddRecord(USHORT, VOID *);
USHORT	ExeAddRecordIn(USHORT, VOID *);
USHORT	ExeReplaceRecord(USHORT, VOID *, VOID *, USHORT *);
USHORT	ExeReplaceRecordIn(USHORT, VOID *, VOID *, USHORT *, USHORT *);					/* ### MOD (UCHAR->USHORT) 2172 Rel1.0 */
USHORT	ExeChangeRecord(USHORT, VOID *, ULONG, USHORT, VOID *, USHORT *);
USHORT	ExeChangeRecordIn(USHORT, VOID *, ULONG, USHORT, VOID *, USHORT *, USHORT *);	/* ### MOD (UCHAR->USHORT) 2172 Rel1.0 */
USHORT	ExeDeleteRecord(USHORT, VOID *, USHORT, USHORT, VOID *, USHORT *);
USHORT	ExeDeleteRecordIn(USHORT, VOID *, USHORT, USHORT, VOID *, USHORT *, USHORT *);	/* ### MOD (UCHAR->USHORT) 2172 Rel1.0 */
USHORT	ExeDeleteReserved(USHORT);
USHORT	ExeResetReserved(USHORT);
USHORT	ExeRewriteReserved(USHORT, VOID *);
USHORT	ExeCancelReserved(USHORT);
USHORT	ExeAppendMainte(USHORT, VOID *);
USHORT	ExePurgeMainte(USHORT);
USHORT	ExeApplyMainte(USHORT, VOID *, USHORT *, VOID *, USHORT *, USHORT *);
USHORT	ExeRemoveMainte(USHORT, ULONG);	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
USHORT	ExeModifyMainte(USHORT, ULONG, VOID *);
USHORT	ExeReadMainte(USHORT, ULONG, VOID *, USHORT *);
USHORT	ExeAddMainte(USHORT, VOID *, ULONG *);		/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
USHORT	ExeReadRecord(USHORT, PITEMNO, USHORT, VOID *, USHORT *);
USHORT	ExeReserveRecord(USHORT, PITEMNO, USHORT, VOID *, USHORT *);
USHORT	ExeReportRecord(USHORT, VOID *, ULONG *, USHORT *);	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
USHORT	ExeReadBlock(USHORT, WCHAR *, USHORT, USHORT *);
USHORT	ExeWriteBlock(USHORT, WCHAR *, USHORT, USHORT *);
USHORT	ExeSeekBlock(USHORT, LONG, USHORT, ULONG *);
USHORT	ExeUpdateRecord(USHORT, PITEMNO, USHORT,
										PUPDINF, PUPDINF, VOID *, USHORT *);
USHORT	ExeInformStatus(USHORT, PLU_STATUS *);			/* R1.1 */
USHORT	ExeSetIndexFileId(USHORT usFile);

/* --- common functions from PLUMANIP.C --- */

USHORT	MnpInitialize(BOOL);
USHORT	MnpFinalize(VOID);
USHORT	MnpCreateFile(USHORT, ULONG);	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
USHORT	MnpDeleteFile(USHORT);
USHORT	MnpClearFile(USHORT);
USHORT	MnpGetFileInfo(USHORT, PSPECS);
USHORT	MnpAddRecord(USHORT, VOID *, ULONG *);
USHORT	MnpDeleteRecord(USHORT, WCHAR *, USHORT);
USHORT	MnpReadRecord(USHORT, USHORT, WCHAR *, USHORT, WCHAR *);
USHORT	MnpBeginLookUp(USHORT, USHORT *, USHORT *, USHORT);
USHORT	MnpEndLookUp(USHORT);
USHORT	MnpReadFile(USHORT, WCHAR *, USHORT, USHORT *);
USHORT	MnpWriteFile(USHORT, WCHAR *, USHORT, USHORT *);
USHORT	MnpSeekFile(USHORT, LONG, USHORT, ULONG *);
USHORT	MnpGetLookUp(USHORT, WCHAR *);
USHORT	MnpModifyRecord(USHORT, VOID *);
USHORT	MnpPeekRecord(USHORT, VOID *);
USHORT	MnpRemoveRecord(USHORT);
USHORT	MnpKeepFilePtr(USHORT);
USHORT	MnpMoveFilePtr(USHORT, VOID *);
USHORT	MnpGetFilePtr(USHORT, ULONG *);	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
USHORT	MnpSetFilePtr(USHORT, ULONG);	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
USHORT	MnpQInfoTable(USHORT, PKEYINF *);
USHORT	MnpOpenMirror(WCHAR FAR *, USHORT *);
USHORT	MnpCloseMirror(USHORT);
BOOL	MnpIsMirrorFile(USHORT);
USHORT	MnpFileOfMirror(USHORT);

/* --- common functions from PLUPHYSC.C --- */

USHORT	PhyOpenFile(WCHAR FAR *, USHORT *, BOOL);
USHORT	PhyCreateFile(WCHAR FAR *, USHORT *, USHORT, USHORT, USHORT, ULONG); /* ### MOD 2172 Rel1.0 */
USHORT	PhyDeleteFile(WCHAR FAR *);
USHORT	PhyClearFile(USHORT);
USHORT	PhyOpenFile(WCHAR FAR *, USHORT *, BOOL);
USHORT	PhyAuditFile(USHORT);
USHORT	PhyReadFile(USHORT, WCHAR *, ULONG *, ULONG);
USHORT	PhyWriteFile(USHORT, WCHAR *, ULONG *, ULONG);
USHORT	PhyCloseFile(USHORT);
USHORT	PhyGetInfo(USHORT, PFILEHDR);
USHORT	PhyUpdateInfo(USHORT, PFILEHDR);
USHORT	PhyReadRecord(USHORT, ULONG, WCHAR *, PFILEHDR);		/* ### MOD 2172 Rel1.0 */
USHORT	PhyWriteRecord(USHORT, ULONG, WCHAR *, PFILEHDR);		/* ### MOD 2172 Rel1.0 */
USHORT	PhyExamOccupied(USHORT, ULONG, PFILEHDR, BOOL *);		/* ### MOD 2172 Rel1.0 */
USHORT	PhySetUpOccupied(USHORT, ULONG, PFILEHDR, BOOL);		/* ### MOD 2172 Rel1.0 */
USHORT	PhySearchVacant(USHORT, ULONG, PFILEHDR, ULONG *);		/* ### MOD 2172 Rel1.0 */
USHORT	PhySearchOccupied(USHORT, ULONG, PFILEHDR, ULONG *);


SHORT   Rfl_SpBsearch_PLU32(VOID *, USHORT, VOID *, ULONG, VOID **, SHORT (*psCompar)(VOID *, VOID *));
																/* ### ADD 2172 Rel1.0	*/

#if		(_WIN32_WCE || WIN32) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif


/* ************************************************************************ */
