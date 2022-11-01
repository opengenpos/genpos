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
*	Compiler:		MS-C Ver. 6.00 A by Microsoft Corp.              
*	Memory Model:	Medium Model
*	Options:		/c /AM /Gs /Os /Za /Zp /W4 /G1
* ---------------------------------------------------------------------------
*	Abstract:
*
* ---------------------------------------------------------------------------
*	Update Histories:
* ---------------------------------------------------------------------------
*	Date     | Version  | Descriptions							| By
* ---------------------------------------------------------------------------
*			 |			|										|
*** 2172
* 11-05-99   | 1.00     | Initial                               | K.Iwata


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
;+					I N C L U D E    F I L E s								+
;+																			+
\*==========================================================================*/

#include    <memory.h>							/* memory C-library			*/
#include    <stdlib.h>                          /* standard library         */
#include	<ecr.h>								/* 2170 system header		*/
#include    <r20_pif.h>								/* PifXXXX() header			*/
#include	<PLU.H>								/* PLU common header		*/
#include	"PLUDEFIN.H"						/* Mass Memory Module header*/



/*==========================================================================*\
;+																			+
;+					L O C A L    D E F I N I T I O N s						+
;+																			+
\*==========================================================================*/

/* --- associated maintenance record adjustment --- */

#define		ADJUST_BOUND_KEY		1			/* adjustment for key		*/

/* --- mirror file ID adjustment --- */

#define		ADJUSTFILE(f)		(MnpIsMirrorFile(f) ? MnpFileOfMirror(f) : f)



/*==========================================================================*\
;+																			+
;+			P R O T O T Y P E     D E C L A R A T I O N s					+
;+																			+
\*==========================================================================*/

/* --- internal functions --- */

USHORT	GetBatchNumber(USHORT, WCHAR *);
BOOL	IsActiveRecord(USHORT, WCHAR *);
BOOL	IsSatisfiedRecord(WCHAR *, WCHAR *, PRECDIC);
USHORT	QueryCritUsers(USHORT);
BOOL	IsLockedFile(USHORT, USHORT);
BOOL	IsLockedRecord(USHORT, PITEMNO, USHORT);
USHORT	ReturnUpdateRecord(VOID *, PRECPLU, PRECPLU, PRECMIX/*,PRECPPI*/);/* ### MOD 2172 Rel1.0 */
BOOL	IsValidCondition(WCHAR *, USHORT, PRECDIC);
BOOL	IsSatisfiedData(VOID *, VOID *, UCHAR, USHORT, UCHAR);
/* R2.0 */
VOID    SetIndexRecord(RECIDX *pRecIdx, RECPLU *pRecPlu);



/*==========================================================================*\
;+																			+
;+					S T A T I C   V A R I A B L E s							+
;+																			+
\*==========================================================================*/

/* --- user management tables --- */

static	USRBLK	aUserBlock[MAX_USERS];			/* user management table	*/

static  MIRRORIDX   aMirrorIdx[MAX_USERS];



/*==========================================================================*\
;+																			+
;+				C O N S T A N T    D E F I N I T I O N s					+
;+																			+
\*==========================================================================*/





/*==========================================================================*\
;+																			+
;+				A P I    E x e c u t i v e    R o u t i n e s				+
;+																			+
\*==========================================================================*/



/**
;========================================================================
;
;   function :	(Re)Initialize Mass Memory Module
;
;   format : VOID	ExeInitialize(fRefresh);
;
;   input  : nothing
;
;   output : nothing
;
;========================================================================
**/

VOID	ExeInitialize(BOOL fRefresh)
{
	/* --- initialize user management table --- */

	memset(aUserBlock, 0, sizeof(aUserBlock));	/* clear table w/ 0			*/
	memset(aMirrorIdx, 0, sizeof(aMirrorIdx));	/* clear table w/ 0			*/

	/* --- pass a control to manipulation --- */

	MnpInitialize(fRefresh);

	/* --- exit ... --- */

	return;
}

/**
;========================================================================
;
;   function :	Finalize Mass Memory Module
;
;   format : VOID	ExeFinalize(VOID);
;
;   input  : nothing
;
;   output : nothing
;
;========================================================================
**/

VOID	ExeFinalize(VOID)
{
	/* --- initialize user management table --- */

	memset(aUserBlock, 0, sizeof(aUserBlock));	/* clear table w/ 0			*/
	memset(aMirrorIdx, 0, sizeof(aMirrorIdx));	/* clear table w/ 0			*/

	/* --- pass a control to manipulation --- */

	MnpFinalize();

	/* --- exit ... --- */

	return;
}

/**
;========================================================================
;
;   function : Create a Mass Memory file
;
;   format : USHORT		ExeCreateFile(usFile, ulNoOfRec);
;
;   input  : USHORT		usFile;			- file ID to create
;			 USHORT		ulNoOfRec;		- no. of records ### MOD 2172 Rel1.0 (USHORT->ULONG)
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	ExeCreateFile(USHORT usFile, ULONG ulNoOfRec)
{
	USHORT	usStat;
	FSPECS	specInfo;

	/* --- is mirror file given ? --- */

	if (MnpIsMirrorFile(usFile)) {				/* is mirror file given ?	*/
		return (SPLU_FUNC_NOT_AVAIL);				/* cannot support		*/
	}

	/* --- is valid file ID given ? --- */

	switch (usFile) {							/* what kind of file ?		*/
	case FILE_PLU:								/* PLU file ?				*/
	case FILE_MAINT_PLU:						/* Maintenance PLU file ?	*/
	case FILE_MIX_MATCH:						/* Mix-Match file ?			*/
	/*case FILE_PPI:	*/						/* PPI file ?				*//* ### DEL 2172 Rel1.0 */
    case FILE_PLU_INDEX:                        /* PLU Index file ? R2.0    */
		break;										/* these are valid !	*/
	default:									/* others ...				*/
		return (SPLU_INVALID_DATA);					/* invalid data			*/
	}

	/* --- is file locked ? --- */

	if (IsLockedFile(usFile, FILE_READABLE + FILE_WRITABLE)) {
		return (SPLU_FILE_LOCKED);
	}

	/* --- is any locked rec. in the file ? --- */

	if (IsLockedRecord(usFile, (PITEMNO)NULL, 0)) {
		return (SPLU_REC_LOCKED);
	}

    /* --- check index file status R2.0 --- */

    if (ADJUSTFILE(usFile) == FILE_PLU) {

    	if (! (usStat = MnpGetFileInfo(FILE_PLU_INDEX, &specInfo))) {

        	/* --- is file locked ? --- */

        	if (IsLockedFile(FILE_PLU_INDEX, FILE_READABLE + FILE_WRITABLE)) {
    		    return (SPLU_FILE_LOCKED);
	        }

        	/* --- is any locked rec. in the file ? --- */

        	if (IsLockedRecord(FILE_PLU_INDEX, (PITEMNO)NULL, 0)) {
		        return (SPLU_REC_LOCKED);
        	}

            /* --- delete index file --- */

            usStat = MnpDeleteFile(FILE_PLU_INDEX);				/* just delete it		*/

            if (usStat) return(usStat);

        }
    }

	/* --- create a 0 record file or not ? --- */

	switch (ulNoOfRec) {						/* how many records ?		*/
	case 0:										/* create a 0 rec. file ?	*/
		usStat = MnpDeleteFile(usFile);				/* just delete it		*/
		break;
	default:									/* others ...				*/
		usStat = MnpCreateFile(usFile, ulNoOfRec);	/* create the file		*/
		break;
	}

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Delete a Mass Memory file
;
;   format : USHORT		ExeDeleteFile(usFile);
;
;   input  : USHORT		usFile;			- file ID to delete
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	ExeDeleteFile(USHORT usFile)
{
	USHORT	usStat;
	FSPECS	specInfo;

	/* --- is mirror file given ? --- */

	if (MnpIsMirrorFile(usFile)) {				/* is mirror file given ?	*/
		return (SPLU_FUNC_NOT_AVAIL);				/* cannot support		*/
	}

	/* --- is valid file ID given ? --- */

	switch (usFile) {							/* what kind of file ?		*/
	case FILE_PLU:								/* PLU file ?				*/
	case FILE_MAINT_PLU:						/* Maintenance PLU file ?	*/
	case FILE_MIX_MATCH:						/* Mix-Match file ?			*/
	/*case FILE_PPI:	*/						/* PPI file ?				*//* ### DEL 2172 Rel1.0 */
    case FILE_PLU_INDEX:                        /* PLU Index file ? R2.0    */
		break;										/* these are valid !	*/
	default:									/* others ...				*/
		return (SPLU_INVALID_DATA);					/* invalid data !		*/
	}

	/* --- is file locked ? --- */

	if (IsLockedFile(usFile, FILE_READABLE + FILE_WRITABLE)) {
		return (SPLU_FILE_LOCKED);
	}

	/* --- is any locked rec. in the file ? --- */

	if (IsLockedRecord(usFile, (PITEMNO)NULL, 0)) {
		return (SPLU_REC_LOCKED);
	}

    /* --- check index file status R2.0 --- */

    if (ADJUSTFILE(usFile) == FILE_PLU) {

    	if (! (usStat = MnpGetFileInfo(FILE_PLU_INDEX, &specInfo))) {

        	/* --- is file locked ? --- */

        	if (IsLockedFile(FILE_PLU_INDEX, FILE_READABLE + FILE_WRITABLE)) {
    		    return (SPLU_FILE_LOCKED);
	        }

        	/* --- is any locked rec. in the file ? --- */

        	if (IsLockedRecord(FILE_PLU_INDEX, (PITEMNO)NULL, 0)) {
		        return (SPLU_REC_LOCKED);
        	}

            /* --- delete index file --- */

            usStat = MnpDeleteFile(FILE_PLU_INDEX);				/* just delete it		*/

            if (usStat) return(usStat);

        }
    }

	/* --- just delete it --- */

	usStat = MnpDeleteFile(usFile);				/* just delete it			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Clear contents of a Mass Memory file
;
;   format : USHORT		ExeClearFile(usFile);
;
;   input  : USHORT		usFile;			- file ID to clear
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	ExeClearFile(USHORT usFile)
{
	USHORT	usStat;
    USHORT  usIndexFile;
	FSPECS	specInfo;

	/* --- is valid file ID given ? --- */

	switch (ADJUSTFILE(usFile)) {				/* what kind of file ?		*/
	case FILE_PLU:								/* PLU file ?				*/
	case FILE_MAINT_PLU:						/* Maintenance PLU file ?	*/
	case FILE_MIX_MATCH:						/* Mix-Match file ?			*/
	/*case FILE_PPI:*/							/* PPI file ?				*//* ### DEL 2172 Rel1.0 */
		break;										/* these are valid !	*/
	default:									/* others ...				*/
		return (SPLU_INVALID_DATA);					/* unknown file ID !	*/
	}
	
	/* --- is file locked ? --- */

	if (IsLockedFile(usFile, FILE_READABLE + FILE_WRITABLE)) {
		return (SPLU_FILE_LOCKED);
	}

	/* --- is any locked rec. in the file ? --- */

	if (IsLockedRecord(usFile, (PITEMNO)NULL, 0)) {
		return (SPLU_REC_LOCKED);
	}

    /* --- check index file status R2.0 --- */

    if (ADJUSTFILE(usFile) == FILE_PLU) {

        usIndexFile = ExeSetIndexFileId(usFile);

    	if (! (usStat = MnpGetFileInfo(usIndexFile, &specInfo))) {

        	/* --- is file locked ? --- */

        	if (IsLockedFile(usIndexFile, FILE_READABLE + FILE_WRITABLE)) {
    		    return (SPLU_FILE_LOCKED);
	        }

        	/* --- is any locked rec. in the file ? --- */

        	if (IsLockedRecord(usIndexFile, (PITEMNO)NULL, 0)) {
		        return (SPLU_REC_LOCKED);
        	}

            /* --- clear index file --- */

    	    usStat = MnpClearFile(usIndexFile);				/* just clear it			*/

            if (usStat) return(usStat);

        }
    }

	/* --- just clear it --- */

	usStat = MnpClearFile(usFile);				/* just clear it			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Get contents of a Mass Memory file
;
;   format : USHORT		ExeSenseFile(usFile, pFileInfo);
;
;   input  : USHORT		usFile;			- file ID to get info.
;			 PMFINFO	pFileInfo;		- buffer ptr. to respond info.
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	ExeSenseFile(USHORT usFile, PMFINFO pFileInfo)
{
	USHORT	usStat;
	FSPECS	specInfo;

	/* --- is valid file ID given ? --- */

	switch (ADJUSTFILE(usFile)) {				/* what kind of file ?		*/
	case FILE_PLU:								/* PLU file ?				*/
	case FILE_MAINT_PLU:						/* Maintenance PLU file ?	*/
	case FILE_MIX_MATCH:						/* Mix-Match file ?			*/
	/*case FILE_PPI:*/							/* PPI file ?				*//* ### DEL 2172 Rel1.0 */
    case FILE_PLU_INDEX:                        /* PLU Index file ?  R2.0   */
		break;										/* these are valid !	*/
	default:									/* others ...				*/
		return (SPLU_INVALID_DATA);					/* unknown file ID !	*/
	}
	
	/* --- is file locked ? --- */

	if (IsLockedFile(usFile, FILE_READABLE)) {	/* can be read from a file ?*/
		return (SPLU_FILE_LOCKED);					/* No ! being locked	*/
	}

	/* --- get a file info --- */

	if (! (usStat = MnpGetFileInfo(usFile, &specInfo))) {

		/* --- respond it --- */

		pFileInfo->usFileNo   = specInfo.usFileNo;		/* file number		*/
		pFileInfo->ulFileSize = specInfo.ulFileSize;	/* size of file		*/
		pFileInfo->ulMaxRec   = specInfo.ulMaxRec;		/* no. of records	*/
		pFileInfo->ulCurRec   = specInfo.ulCurRec;		/* no. of cur. rec.	*/
	}

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Open a mirror file
;
;   format : USHORT		ExeOpenMirror(pszFileName, pszIndexName, pusMirrorFile);
;
;   input  : UCHAR FAR	*pszFileName;		- ptr. to file name
;            UCAAR FAR  *pszIndexName;      - prt. to index file name
;			 USHORT		*pusMirrorFile;		- buffer ptr. to receive file ID
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	ExeOpenMirror(WCHAR FAR *pszFileName, WCHAR FAR *pszIndexName, USHORT *pusMirrorFile)
{
	USHORT	usStat;
    USHORT  usIndexMirror;
    USHORT  i;

	/* --- just pass a control to manipulation --- */

	usStat = MnpOpenMirror(pszFileName, pusMirrorFile);

    if (!usStat) {

        /* --- open mirror index file, R2.0 --- */

        if (pszIndexName) {

    	    usStat = MnpOpenMirror(pszIndexName, &usIndexMirror);

        } else {

            usIndexMirror = 0;

        }

        /* --- set mirror index file handle table, R2.0 --- */

        if (!usStat) {

            for (i=0; i<MAX_USERS; i++) {

                if (aMirrorIdx[i].usPluFile == 0) {
                    aMirrorIdx[i].usPluFile = *pusMirrorFile;
                    aMirrorIdx[i].usIndexFile = usIndexMirror;
                    break;
                }
            }
            if (i>=MAX_USERS) {
                
            	MnpCloseMirror(*pusMirrorFile);
                if (pszIndexName) {
                	MnpCloseMirror(usIndexMirror);
                }
                usStat = SPLU_TOO_MANY_USERS;
            }
        } else {
          	MnpCloseMirror(*pusMirrorFile);
        }
    }

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Close a mirror file
;
;   format : USHORT		ExeCloseMirror(pszFileName, pusMirrorFile);
;
;   input  : USHORT		usMirrorFile;	- file ID of mirror file
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	ExeCloseMirror(USHORT usMirrorFile)
{
	USHORT	usStat, i;

	/* --- just pass a control to manipulation --- */

	usStat = MnpCloseMirror(usMirrorFile);

    /* --- close mirror index file, R2.0 --- */

    for (i=0; i<MAX_USERS; i++) {

        if (aMirrorIdx[i].usPluFile == usMirrorFile) {

        	MnpCloseMirror(aMirrorIdx[i].usIndexFile);

            aMirrorIdx[i].usPluFile = 0;
            aMirrorIdx[i].usIndexFile = 0;
            break;
        }
    }

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Enter a critical mode
;
;   format : USHORT		ExeEnterCritMode(usFile, usFunc, pusHandle, pvParam);
;
;   input  : USHORT		usFile;			- file ID to enter a crit. mode
;			 USHORT		usFunc;			- function no.
;			 USHORT		*pusHandle;		- buffer ptr. to respond handle
;			 VOID		*pvParam;		- parameter data ptr.
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	ExeEnterCritMode(USHORT usFile,
						 USHORT usFunc,
						 USHORT *pusHandle,
						 VOID   *pvParam)
{
	BOOL		fTurns, fAnyUser;
	WCHAR		*pucData, uchItem;
	USHORT		i, usStat, usManip, usBytes, usAccess, usSeq, usFileVer;
	PKEYINF		pTable;
	PRECDIC		pRecord;
	PUSRBLK		pUser, pPick;
    RECIDX      RecIdx;

	/* --- initialize --- */

	*pusHandle = 0;								/* assume no handle respond	*/

	/* --- is valid file ID given ? --- */
	switch (ADJUSTFILE(usFile)) {				/* what kind of file ?		*/
	case FILE_PLU:								/* PLU file ?				*/
	case FILE_MAINT_PLU:						/* Maintenance PLU file ?	*/
    case FILE_PLU_INDEX:                        /* PLU index file ? ,R2.0   */
		uchItem = PLUID_PLU_NUMBER;				/* prepare key ID			*/
		break;
	case FILE_MIX_MATCH:						/* Mix-Match file ?			*/
		uchItem = MIXID_MIX_NUMBER;					/* prepare key ID		*/
		break;
#ifdef _DEL_2172_Rel10	/* ### DEL 2172 Rel1.0 */
	case FILE_PPI:								/* PPI file ?				*/
		uchItem = PPIID_PPI_CODE;					/* prepare key ID		*/
		break; 
#endif /* _DEL_2172_R10 */
	default:									/* others ...				*/
		return (SPLU_INVALID_DATA);					/* unknown file ID !	*/
	}
	/* --- is valid function with valid file ? --- */

	switch (usFunc) {							/* what kind of function ?	*/
	case FUNC_INDIV_LOCK:						/* read a rec. w/ lock ?	*/
	case FUNC_REPORT_RANGE:						/* report rec. by range ?	*/
	case FUNC_REPORT_RANGE2:					/* report rec. by range ?	*/
	case FUNC_REPORT_ADJECT:					/* report adjective rec. ?	*/
	case FUNC_REPORT_COND:						/* report rec. w/ cond. ?	*/
	case FUNC_REPORT_COND2:						/* report rec. w/ cond. ?	*/
    case FUNC_REPORT_INDEX:                     /* report only index ?      */
    case FUNC_REPORT_R10:                       /* report by r1.0 record ?  */
		if (ADJUSTFILE(usFile) == FILE_MAINT_PLU) {
			return (SPLU_INVALID_DATA);				/* invalid function !	*/
		}
		break;
	case FUNC_REPORT_MAINTE:					/* report mainte. rec. ?	*/
	case FUNC_APPLY_MAINTE:						/* apply mainte. rec. ?		*/
	case FUNC_PURGE_MAINTE:						/* purge mainte. rec. ?		*/
	case FUNC_BOUND_MAINTE:						/* associated mainte. ?		*/
		if (ADJUSTFILE(usFile) != FILE_MAINT_PLU) {
			return (SPLU_INVALID_DATA);				/* invalid function !	*/
		}
		break;
	case FUNC_BLOCK_ACCESS:						/* block read / write ?		*/
		break;
	default:									/* others ...				*/
		return (SPLU_INVALID_DATA);					/* invalid function !	*/
	}

	/* --- query its file info. --- */

	if (usStat = MnpQInfoTable(usFile, &pTable)) {	/* get information table*/
		return (usStat);
	}

	/* --- get record dictionary table --- */

	pRecord = pTable->pDictionary;				/* record dictionary		*/

	/* --- is conditional expression given ? --- */

	if ((usFunc == FUNC_REPORT_COND) || (usFunc == FUNC_REPORT_MAINTE) ||
	    (usFunc == FUNC_REPORT_COND2)) {

		/* --- get conditional expression data ptr. --- */

		switch (usFunc) {						/* which function ?			*/
		case FUNC_REPORT_COND:					/* report rec. w/ cond. ?	*/
		case FUNC_REPORT_COND2:					/* report rec. w/ cond. ?	*/
			pucData = ((PECMCOND)pvParam)->aucCond;
			usBytes = ((PECMCOND)pvParam)->usLength;
			break;
		case FUNC_REPORT_MAINTE:				/* report mainte. rec. ?	*/
		default:								/* others ...				*/
			pucData = ((PECMMAINTE)pvParam)->aucCond;
			usBytes = ((PECMMAINTE)pvParam)->usLength;
			break;
		}

		/* --- is valid data length ? --- */

		if (usBytes > sizeof(pUser->aucFuncData)) {
			return (SPLU_INVALID_DATA);
		}

		/* --- is valid expression ? --- */

		if (! IsValidCondition(pucData, usBytes, pRecord)) {
			return (SPLU_INVALID_DATA);
		}
	}

	/* --- initialize before traceing user table --- */

	fAnyUser = FALSE;							/* assume no user			*/
	fTurns   = FALSE;							/* assume no writing func	*/
	pPick    = aUserBlock;						/* top of user table		*/
	pUser    = (PUSRBLK)NULL;					/* assume not found vacant	*/

	/* --- look for free user table --- */

	for (i = 0; i < ARRAYS(aUserBlock, USRBLK); i++, pPick++) {

		/* --- is in use with same file ? --- */

		if ((pPick->fchControl & USRBLK_IN_USE) && (pPick->usFile == usFile)){

			/* --- set some user on this file --- */

			fAnyUser = TRUE;					/* some users on this file	*/

			/* --- what function ? --- */

			switch (pPick->uchFunc) {			/* what function doing ?	*/
			case FUNC_INDIV_LOCK:				/* read a rec. w/ lock ?	*/
			case FUNC_APPLY_MAINTE:				/* apply mainte. rec. ?		*/
			case FUNC_PURGE_MAINTE:				/* purge mainte. rec. ?		*/
			case FUNC_BOUND_MAINTE:				/* associated mainte. ?		*/
				fTurns = TRUE;						/* assume writing func.	*/
				break;
			case FUNC_REPORT_RANGE:				/* report rec. by range ?	*/
			case FUNC_REPORT_RANGE2:			/* report rec. by range ?	*/
			case FUNC_REPORT_ADJECT:			/* report adjective rec. ?	*/
			case FUNC_REPORT_COND:				/* report rec. w/ cond. ?	*/
			case FUNC_REPORT_COND2:				/* report rec. w/ cond. ?	*/
				fTurns = (pPick->fsOption & REPORT_AS_RESET)  ? TRUE : fTurns;
				fTurns = (pPick->fsOption & REPORT_WITH_LOCK) ? TRUE : fTurns;
				break;
			case FUNC_BLOCK_ACCESS:				/* block read / write ?		*/
				fTurns = (pPick->fsOption & BLOCK_WRITE) ? TRUE : fTurns;
				break;
			case FUNC_REPORT_MAINTE:			/* report mainte. rec. ?	*/
			default:							/* other functions ...		*/
				break;
			}
		}

		/* --- else, this is vacant table ? --- */

		else if ((! (pPick->fchControl & USRBLK_IN_USE)) && (! pUser)) {
			pUser = pPick;							/* remember this table	*/
		}
	}

	/* --- could be found ? --- */

	if (! pUser) {								/* could not find ?			*/
		return (SPLU_TOO_MANY_USERS);				/* error as too many	*/
	}

	/* --- initialize before allocating data --- */

	pUser->usBatch  = 0;						/* no batch number			*/
	pUser->fsOption = 0;						/* no optional data			*/
	pUser->usSearch = 0;						/* no. of max. search		*/
	pUser->usLength = 0;						/* no optional data			*/
	pUser->usFile   = usFile;					/* access file ID			*/
	pUser->usIndexFile = ExeSetIndexFileId(usFile);  /* access file ID for index	*/
	pUser->uchFunc  = (UCHAR)usFunc;			/* function number			*/
	usAccess        = FILE_READABLE;			/* file must be readable	*/

	/* --- is valid function ? --- */

	switch (usFunc) {							/* what kind of function ?	*/
	case FUNC_INDIV_LOCK:						/* read a rec. w/ lock ?	*/
	case FUNC_BOUND_MAINTE:						/* associated mainte. ?		*/
		usAccess |= FILE_WRITABLE;					/* file must be writable*/
		break;										/* no optional data		*/
	case FUNC_REPORT_RANGE:						/* report rec. by range ?	*/
	case FUNC_REPORT_RANGE2:  					/* report rec. by range ?	*/
		pucData    = pUser->aucFuncData;			/* make option data		*/
		*pucData++ = uchItem;						/* make start item no.	*/
		memcpy(pucData, &(((PECMRANGE)pvParam)->itemFrom), pTable->usKeyLen);
		pucData   += pTable->usKeyLen;
		*pucData++ = COMP_GREATER_EQUAL | LOGIC_AND;
		*pucData++ = uchItem;						/* make end item no.	*/
		memcpy(pucData, &(((PECMRANGE)pvParam)->itemTo), pTable->usKeyLen);
		pucData        += pTable->usKeyLen;
		*pucData++      = COMP_LESS_EQUAL | LOGIC_END;
		pUser->usLength = (USHORT)(pucData - pUser->aucFuncData);
		pUser->fsOption = ((PECMRANGE)pvParam)->fsOption;
		pUser->usSearch = ((PECMRANGE)pvParam)->usReserve;
		usAccess |= (pUser->fsOption & REPORT_AS_RESET)  ? FILE_WRITABLE : 0;
		usAccess |= (pUser->fsOption & REPORT_WITH_LOCK) ? FILE_WRITABLE : 0;
		break;
	case FUNC_REPORT_ADJECT:					/* report adjective rec. ?	*/
		pucData    = pUser->aucFuncData;			/* make option data		*/
		*pucData++ = uchItem;						/* make end item no.	*/
		memcpy(pucData,								/* save PLU number		*/
			   &(((PECMADJECT)pvParam)->itemPluNumber),
			   pTable->usKeyLen);
		pucData        += pTable->usKeyLen;
		*pucData++      = COMP_EQUAL | LOGIC_END;
		pUser->usLength = (USHORT)(pucData - pUser->aucFuncData);
		pUser->fsOption = ((PECMADJECT)pvParam)->fsOption;
		pUser->usSearch = ((PECMADJECT)pvParam)->usReserve;
		usAccess |= (pUser->fsOption & REPORT_AS_RESET)  ? FILE_WRITABLE : 0;
		usAccess |= (pUser->fsOption & REPORT_WITH_LOCK) ? FILE_WRITABLE : 0;
		break;
	case FUNC_REPORT_COND:						/* report rec. w/ cond. ?	*/
	case FUNC_REPORT_COND2:						/* report rec. w/ cond. ?	*/
		pUser->usLength = usBytes;
		pUser->fsOption = ((PECMCOND)pvParam)->fsOption;
		pUser->usSearch = ((PECMCOND)pvParam)->usReserve;
		memcpy(pUser->aucFuncData, pucData, usBytes);
		usAccess |= (pUser->fsOption & REPORT_AS_RESET)  ? FILE_WRITABLE : 0;
		usAccess |= (pUser->fsOption & REPORT_WITH_LOCK) ? FILE_WRITABLE : 0;
		break;
	case FUNC_REPORT_MAINTE:					/* report mainte. rec. ?	*/
		pUser->usBatch  = ((PECMMAINTE)pvParam)->usBatch;
		pUser->usSearch = ((PECMMAINTE)pvParam)->usReserve;
		pUser->usLength = usBytes;
		memcpy(pUser->aucFuncData, pucData, usBytes);
		break;
	case FUNC_APPLY_MAINTE:						/* apply mainte. rec. ?		*/
		pUser->usBatch  = ((PECMAPPLY)pvParam)->usBatch;
		pUser->fsOption = ((PECMAPPLY)pvParam)->fsProg;
		pUser->usSearch = ((PECMAPPLY)pvParam)->usReserve;
		usAccess       |= FILE_WRITABLE;			/* must be writable		*/
		break;
	case FUNC_PURGE_MAINTE:						/* purge mainte. rec. ?		*/
		pUser->usBatch  = ((PECMPURGE)pvParam)->usBatch;
		pUser->fsOption = ((PECMPURGE)pvParam)->fsOption;
		pUser->usSearch = ((PECMPURGE)pvParam)->usReserve;
		usAccess       |= FILE_WRITABLE;			/* must be writable		*/
		break;
	case FUNC_BLOCK_ACCESS:						/* block read / write ?		*/
		pUser->fsOption = ((PECMBLOCK)pvParam)->fsOption;
		usAccess        = (pUser->fsOption & BLOCK_READ)  ? FILE_READABLE : 0;
		usAccess       |= (pUser->fsOption & BLOCK_WRITE) ? FILE_WRITABLE : 0;
		break;
    case FUNC_REPORT_INDEX:
    case FUNC_REPORT_R10:
		pUser->fsOption = (REPORT_ACTIVE|REPORT_INACTIVE);
        break;
	default:									/* others ...				*/
		return (SPLU_INVALID_DATA);					/* invalid function !	*/
	}

	/* --- is valid search value ? --- */

	switch (pUser->usSearch) {					/* how many searches ?		*/
	case 0:										/* used default value ?		*/
	case UNC_SEARCH:							/* unconditional search ?	*/
		break;										/* these are OK !		*/
	default:									/* some values ...			*/
		if ((pUser->usSearch % UNT_SEARCH) ||		/* multiple by unit ?	*/
			(pUser->usSearch < MIN_SEARCH) ||		/* over min. value	?	*/
			(pUser->usSearch > MAX_SEARCH)) {		/* greater max. value ?	*/
				return (SPLU_INVALID_DATA);				/* invalid data !	*/
		}
		break;
	}

	/* --- is the file accessable ? --- */

	if (IsLockedFile(usFile, usAccess)) {		/* is the file accessable ?	*/
		return (SPLU_FILE_LOCKED);					/* assume file locked	*/
	}

    if (ADJUSTFILE(usFile) == FILE_PLU) {

	    if (IsLockedFile(ExeSetIndexFileId(usFile), FILE_READABLE)) {		/* is the file accessable ?	*/
		    return (SPLU_FILE_LOCKED);					/* assume file locked	*/
    	}
    }

	/* --- read / write access to the file ? --- */

	switch (usFunc) {							/* what kind of function ?	*/
	case FUNC_BLOCK_ACCESS:						/* block read / write ?		*/
		if (((pUser->fsOption & BLOCK_READ)  && fTurns) ||
			((pUser->fsOption & BLOCK_WRITE) && fAnyUser)) {
			return (SPLU_FILE_UNLOCKABLE);			/* cannot be lockable	*/
		}
		break;
	case FUNC_BOUND_MAINTE:						/* associated mainte. ?		*/
		if (fAnyUser) {								/* is lockable ?		*/
			return (SPLU_FILE_UNLOCKABLE);			/* cannot be lockable	*/
		}
		break;
	default:									/* other functions ...		*/
		break;										/* don't mind it !		*/
	}

	/* --- is sequential function ? R2.0 --- */

	switch (usFunc) {							/* what kind of function ?	*/
	case FUNC_REPORT_RANGE2:  					/* report rec. by range     */
	case FUNC_REPORT_COND2:						/* report rec. w/ cond.     */
	case FUNC_REPORT_ADJECT:					/* report adjective rec.    */
    case FUNC_REPORT_R10:                       /* read old version plu file */
        usSeq = 0;                              /* ignor index file */
        break;
	default:									/* other functions ...		*/
        usSeq = 1;                              /* read by index file */
        break;
	}

	/* --- is file migration ? R2.0 --- */

	switch (usFunc) {							/* what kind of function ?	*/
    case FUNC_REPORT_R10:                       /* read old version plu file */
        usFileVer = PLU_FILE_R10;
        break;
	default:									/* other functions ...		*/
        usFileVer = 0;
        break;
	}

	/* --- allocate sequential access control --- */

	if (usStat = MnpBeginLookUp(usFile, &usManip, &usSeq, usFileVer)) {
		return (usStat);
	}

	/* --- make up user management table --- */

	pUser->fchControl = USRBLK_IN_USE;			/* set the table used		*/
	pUser->hManip     = usManip;				/* seq. access handle		*/

	/* --- make additional options --- */

	switch (usFunc) {							/* what kind of function ?	*/
	case FUNC_REPORT_ADJECT:					/* report adjective rec. ?	*/
		MnpMoveFilePtr(usManip, &(((PECMADJECT)pvParam)->itemPluNumber));
		break;
	case FUNC_BLOCK_ACCESS:						/* block read / write ?		*/
		pUser->fchControl |= (UCHAR)((pUser->fsOption & BLOCK_READ)
							? (USRBLK_PROH_WRITE)                    : 0);
		pUser->fchControl |= (UCHAR)((pUser->fsOption & BLOCK_WRITE)
							? (USRBLK_PROH_WRITE + USRBLK_PROH_READ) : 0);
		break;
	case FUNC_BOUND_MAINTE:						/* associated mainte. ?		*/
		pUser->fchControl |= (UCHAR)(USRBLK_PROH_WRITE);
		break;
    /* ---- set first read point, if index file is provided, R2.0 ---- */
    /* for AC29/39 Type 2 Report */
	case FUNC_REPORT_COND:						/* report rec. w/ cond.     */
#ifdef	__DEL_2172	/* ### DEL 2172 Rel1.0 (01/07/00) */
        if ((usSeq) && (pUser->usLength)) {
    		if ((pUser->aucFuncData[0] == PLUID_DEPT_NUMBER) &&
	    	    (pUser->aucFuncData[2] & COMP_EQUAL)) {

                memset(&RecIdx, 0, sizeof(RecIdx));
                RecIdx.usDeptNumber = pUser->aucFuncData[1]; /* ### MOD 2172 Rel1.0 */
        		MnpMoveFilePtr(usManip, &RecIdx);
            }
        }
#endif	/* ### DEL 2172 Rel1.0 (01/07/00) */
        break;
    /* for PEP PLU Parameter Report */
	case FUNC_REPORT_RANGE:  					/* report rec. by range     */
        if ((usSeq) && (pUser->usLength)) {
          /*RecIdx.usDeptNumber = 1;*/	/* ### DEL 2172 Rel1.0 */
            memcpy(&RecIdx.aucPluNumber, &(((PECMRANGE)pvParam)->itemFrom), pTable->usKeyLen);
            RecIdx.uchAdjective = 0;
            MnpMoveFilePtr(usManip, &RecIdx);
           /*pUser->usDeptNumber = 1;*/	/* ### DEL 2172 rel1.0 (01/07/00) */
        }
        break;
	default:									/* other functions ...		*/
		break;
	}

	/* --- respond access handle --- */

	*pusHandle = (USHORT)(pUser - aUserBlock) + 1;

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Exit from a critical mode
;
;   format : USHORT		ExeExitCritMode(usHandle, usReserve);
;
;   input  : USHORT		usHandle;		- handle value
;			 USHORT		usReserve;		- reserved (must be 0)
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	ExeExitCritMode(USHORT usHandle, USHORT usReserve)
{
	USHORT	usStat;
	PUSRBLK	pUser;

	/* --- is valid parameter given ? --- */

	if (usReserve) {							/* this must be 0 !			*/
		return (SPLU_INVALID_DATA);					/* invalid data			*/
	}

	/* --- compute user block table ptr. --- */

	pUser = aUserBlock + usHandle - 1;

	/* --- is it used table ? --- */

	if (! (pUser->fchControl & USRBLK_IN_USE)) {	/* used table ? 		*/
		return (SPLU_INVALID_HANDLE);				/* assume invalid handle*/
	}

	/* --- free the sequential access --- */

	usStat = MnpEndLookUp(pUser->hManip);

	/* --- free the table --- */

	pUser->fchControl = 0;

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Kill all users in a critical mode
;
;   format : USHORT		ExeKillCritMode(usFile, usReserve);
;
;   input  : USHORT		usFile;			- file number
;			 USHORT		usReserve;		- reserved (must be 0)
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	ExeKillCritMode(USHORT usFile, USHORT usReserve)
{
	USHORT	i, usStat;
	PUSRBLK	pUser;

	/* --- is valid parameter given ? --- */

	if (usReserve) {							/* this must be 0 !			*/
		return (SPLU_INVALID_DATA);					/* invalid data			*/
	}

	/* --- is valid file ID given ? --- */

	switch (ADJUSTFILE(usFile)) {				/* what kind of file ?		*/
	case FILE_ALL:								/* means all file			*/
	case FILE_PLU:								/* PLU file ?				*/
	case FILE_MAINT_PLU:						/* Maintenance PLU file ?	*/
	case FILE_MIX_MATCH:						/* Mix-Match file ?			*/
	/*case FILE_PPI:*/							/* PPI file ?				*//* ### DEL 2172 Rel1.0 */
    case FILE_PLU_INDEX:                        /* PLU index file ?, R2.0   */
		break;
	default:									/* others ...				*/
		return (SPLU_INVALID_DATA);					/* unknown file ID !	*/
	}

	/* --- initialize before doing my job --- */

	pUser  = aUserBlock;						/* top of user table		*/
	usStat = 0;									/* assume good status		*/

	/* --- investigate all user tables --- */

	for (i = 0; i < ARRAYS(aUserBlock, USRBLK); i++) {

		/* --- is in use ? --- */

		if (pUser->fchControl & USRBLK_IN_USE) {

			/* --- is same file number ? --- */

			if ((usFile == FILE_ALL) || (pUser->usFile == usFile)) {
				pUser->fchControl |= USRBLK_KILLED;
			}
		}

		/* --- go on to next user table --- */

		pUser++;
	}

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Add a record
;
;   format : USHORT		ExeAddRecord(usFile, pvRec);
;
;   input  : USHORT		usFile;			- file ID
;			 VOID		*pvRec;			- ptr. to record data
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	ExeAddRecord(USHORT usFile, VOID *pvRec)
{
	USHORT	usStat;
    USHORT  usIndexFile;
    RECIDX  RecIdx;

    if (ADJUSTFILE(usFile) == FILE_PLU) {

        usIndexFile = ExeSetIndexFileId(usFile);

       	if (IsLockedFile(usIndexFile, FILE_READABLE + FILE_WRITABLE)) {

    		return (SPLU_FILE_LOCKED);
        }
    }

	/* --- pass the control to executive routine --- */

	usStat = ExeAddRecordIn(usFile, pvRec);

    /* --- add index record to index file, R2.0 ---  */

    if ((!usStat) && (ADJUSTFILE(usFile) == FILE_PLU)) {

        SetIndexRecord(&RecIdx, (RECPLU *)pvRec);

	    usStat = ExeAddRecordIn(usIndexFile, &RecIdx);

        if ((usStat == SPLU_FILE_NOT_FOUND) ||
            (usStat == SPLU_REC_DUPLICATED)) {

            usStat = SPLU_COMPLETED;
        }
    }

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Add a record
;
;   format : USHORT		ExeAddRecordIn(usFile, pvRec);
;
;   input  : USHORT		usFile;			- file ID
;			 VOID		*pvRec;			- ptr. to record data
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	ExeAddRecordIn(USHORT usFile, VOID *pvRec)
{
	USHORT	usStat;

	/* --- is valid file ID given ? --- */
	
	switch (ADJUSTFILE(usFile)) {				/* what kind of file ?		*/
	case FILE_PLU:								/* PLU file ?				*/
	case FILE_MIX_MATCH:						/* Mix-Match file ?			*/
	/*case FILE_PPI:*/							/* PPI file ?				*//* ### DEL 2172 Rel1.0 */
    case FILE_PLU_INDEX:                        /* PLU Index file ?  R2.0   */
		break;										/* these are valid !	*/
	default:									/* others ...				*/
		return (SPLU_INVALID_DATA);					/* invalid file ID !	*/
	}

	/* --- is file locked ? --- */

	if (IsLockedFile(usFile, FILE_READABLE + FILE_WRITABLE)) {
		return (SPLU_FILE_LOCKED);
	}

	/* --- just add it --- */

	usStat = MnpAddRecord(usFile, pvRec, NULL);	/* just add it				*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Replace a record
;
;   format : USHORT		ExeReplaceRecord(usFile, pvRec, pvBuffer, pusRet);
;
;   input  : USHORT		usFile;			- file ID
;			 VOID		*pvRec;			- ptr. to record data
;			 VOID		*pvBuffer;		- buffer ptr. to respond old rec.
;			 USHORT		*usRet;			- buffer ptr. to respond rec. length
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	ExeReplaceRecord(USHORT usFile, VOID *pvRec, VOID *pvBuffer, USHORT *pusRet)
{
	USHORT	usStat,usRet;
    USHORT  usIndexFile;
    RECIDX  RecIdx;
    /* UCHAR   uchPreDept = 0; */
	USHORT	usPreDept = 0;	/* ### MOD (UCHAR->USHORT) 2172 Rel1.0 */

    if (ADJUSTFILE(usFile) == FILE_PLU) {

        usIndexFile = ExeSetIndexFileId(usFile);

       	if (IsLockedFile(usIndexFile, FILE_READABLE + FILE_WRITABLE)) {

    		return (SPLU_FILE_LOCKED);
        }
    }

	/* --- pass the control to executive routine --- */

	usStat = ExeReplaceRecordIn(usFile,
                                 pvRec,
                                 pvBuffer,
                                 &usPreDept, /* ### MOD (UCHAR->USHORT) 2172 Rel1.0 */
                                 pusRet);

    /* --- delete and add index record R2.0 --- */

    if ((!usStat) && (ADJUSTFILE(usFile) == FILE_PLU)) {

        /* --- compare dept. no --- */

#ifdef	__DEL_2172 /* ### DEL 2172 Rel1.0 (1/7/00) */
        if (usPreDept) {   /* if old record exists *//* ### MOD (UCHAR->USHORT) 2172 Rel1.0 */
            if (((RECPLU *)pvRec)->usDeptNumber != usPreDept) { /* ### MOD (UCHAR->USHORT) 2172 Rel1.0 */

                /* delete old dept record */

                SetIndexRecord(&RecIdx, (RECPLU *)pvRec);
                RecIdx.usDeptNumber = usPreDept;	/* ### MOD (UCHAR->USHORT) 2172 Rel1.0 */

              	usStat = ExeDeleteRecordIn(usIndexFile,
           	            					 &RecIdx,
           		    	        			 0,
    	        	    		        	 0,
       			            				 NULL,
                                             &usPreDept,	/* ### MOD (UCHAR->USHORT) 2172 Rel1.0 */
            		            			 &usRet);

                if (usStat == SPLU_FILE_NOT_FOUND) {

                    return(SPLU_COMPLETED);
                }

                /* add new record */

                SetIndexRecord(&RecIdx, (RECPLU *)pvRec);

                usStat = ExeAddRecordIn(usIndexFile, &RecIdx);

                if ((usStat == SPLU_FILE_NOT_FOUND) ||
                    (usStat == SPLU_REC_DUPLICATED)) {

                    usStat = SPLU_COMPLETED;
                }
            }
        } else {    /* new addition case */
#endif 

            /* add new record */

            SetIndexRecord(&RecIdx, (RECPLU *)pvRec);

            /* ### OLD usStat = ExeAddRecordIn(usIndexFile, &RecIdx);*/
			usStat = ExeReplaceRecordIn(usIndexFile,&RecIdx,NULL, &usPreDept, &usRet); /* ### MOD 2172 Rel1.0 (01/15/00) */
            if ((usStat == SPLU_FILE_NOT_FOUND) ||
                (usStat == SPLU_REC_DUPLICATED)) {

                usStat = SPLU_COMPLETED;
            }
     /* } ### DEL 2172 Rel1.0 */
    }

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Replace a record
;
;   format : USHORT		ExeReplaceRecordIn(usFile, pvRec, pvBuffer, pusRet);
;
;   input  : USHORT		usFile;			- file ID
;			 VOID		*pvRec;			- ptr. to record data
;			 VOID		*pvBuffer;		- buffer ptr. to respond old rec.
;			 USHORT		*usRet;			- buffer ptr. to respond rec. length
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/


USHORT	ExeReplaceRecordIn(USHORT usFile,
                     VOID *pvRec,
                     VOID *pvBuffer,
                     USHORT *pusDept, /* ### MOD (UCHAR->USHORT) 2172 Rel1.0 */
                     USHORT *pusRet)
{
	WCHAR		*pucKey;
	WCHAR		aucRecord[ENOUGHREC];
	USHORT		usStat, usAdj;/*,i;*/
	PKEYINF		pTable;
    /*RECIDX  RecIdx;*/

	/* --- initialize --- */

	*pusRet = 0;								/* assume no data returned	*/

	/* --- is valid file ID given ? --- */
	switch (ADJUSTFILE(usFile)) {				/* what kind of file ?		*/
	case FILE_PLU:								/* PLU file ?				*/
		pucKey = ((PRECPLU)pvRec)->aucPluNumber;
		usAdj  = (USHORT)(((PRECPLU)pvRec)->uchAdjective);
		break;										/* these are valid !	*/
	case FILE_MIX_MATCH:						/* Mix-Match file ?			*/
		pucKey = &(((PRECMIX)pvRec)->uchMixMatch);
		usAdj  = 0;
		break;										/* these are valid !	*/
#ifdef _DEL_2172_R10	/* ### DEL 2172 Rel1.0 */
	case FILE_PPI:							/* PPI file ?				*/
		pucKey = &(((PRECPPI)pvRec)->uchPPI);
		usAdj  = 0; 
		break;										/* these are valid !	*/
#endif /* _DEL_2172_R10 */
	case FILE_PLU_INDEX:							/* PLU index file ?		*/
		pucKey = ((PRECIDX)pvRec)->aucPluNumber;
		usAdj  = (USHORT)(((PRECIDX)pvRec)->uchAdjective);
		break;										/* these are valid !	*/
	default:									/* others ...				*/
		return (SPLU_INVALID_DATA);					/* invalid file ID !	*/
	}
	/* --- query its file info. --- */

	if (usStat = MnpQInfoTable(usFile, &pTable)) {	/* get information table*/
		return (usStat);
	}

	/* --- is file locked ? --- */

	if (IsLockedFile(usFile, FILE_READABLE + FILE_WRITABLE)) {
		return (SPLU_FILE_LOCKED);
	}

	/* --- is it a locked rec. ? --- */

	if (IsLockedRecord(usFile, (PITEMNO)pucKey, usAdj)) {
		return (SPLU_REC_LOCKED);
	}

	/* --- try to read it --- */
#ifdef	XXX_XXX /* 2000.1.7 */
    if (ADJUSTFILE(usFile) == FILE_PLU_INDEX) {

        memcpy(&RecIdx, pvRec, sizeof(RECIDX));

        for (i=0; i<MAX_DEPT; i++) {

            RecIdx.usDeptNumber = (USHORT)i; /* ### 2172 Rel1.0 */

    	    usStat = MnpReadRecord(usFile, HMNP_REC_MAIN, (WCHAR *)&RecIdx, 0, NULL);

            if (usStat == 0) break;
            if (usStat == SPLU_REC_NOT_FOUND) continue;
        }

    } else {
#endif
	    usStat = MnpReadRecord(usFile, HMNP_REC_MAIN, pucKey, usAdj, aucRecord);

/*    } ### 2000.1.7 */

	/* --- could be read ? --- */

	switch (usStat) {							/* how's it going ?			*/
	case 0:										/* completed to read ?		*/
        /* --- replace index record R2.0 --- */

        if (ADJUSTFILE(usFile) == FILE_PLU_INDEX) {

            /* --- delete old index record --- */
			#ifdef	__DEL_2172	/* ### DEL 2172 Rel1.0 (01/15/00) */
  	        usStat = MnpDeleteRecord(usFile, (UCHAR *)&RecIdx, 0);	/* delete index file*/

            if (usStat == SPLU_COMPLETED) {

                usStat = MnpAddRecord(usFile, pvRec, NULL);	/* add on index file*/
            }
			#endif /* __DEL_2172 */
            if (pvBuffer) {
                memcpy(pvBuffer, aucRecord, pTable->usBytesRec);/* respond record	*/
            }
		    *pusRet = pTable->usBytesRec;
            break;

        } else {

            /* set previous dept no. for index file, R2.0 */

            if (ADJUSTFILE(usFile) == FILE_PLU) {

                *pusDept = ((RECPLU *)aucRecord)->usDeptNumber;	/* ### MOD (UCHAR->USHORT) 2172 Rel1.0 */
            }

		    if (! (usStat = MnpModifyRecord(HMNP_REC_MAIN, pvRec))) {

                if (pvBuffer) {
            		memcpy(pvBuffer, aucRecord, pTable->usBytesRec);/* respond record	*/
                }
   			    *pusRet = pTable->usBytesRec;
            }
		}
		break;

	case SPLU_REC_NOT_FOUND:					/* record not found ?		*/
		usStat = MnpAddRecord(usFile, pvRec, NULL);	/* just add it			*/

        /* set previous dept no. for index file, R2.0 */
        *pusDept = 0;

		break;

	default:									/* other errors ...			*/
		break;
	}

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Change a record
;
;   format : USHORT		ExeChangeRecord(usFile, pvRec, flChanges,
;												fsProg, pvBuffer, pusRet);
;
;   input  : USHORT		usFile;			- file ID
;			 VOID		*pvRec;			- ptr. to record data
;			 ULONG		flChanges;		- change fields flag
;			 USHORT		fsProg;			- programability
;			 VOID		*pvBuffer;		- buffer ptr. to respond old rec.
;			 USHORT		*usRet;			- buffer ptr. to respond rec. length
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	ExeChangeRecord(USHORT usFile,
						VOID   *pvRec,
						ULONG  flChanges,
						USHORT fsProg,
						VOID   *pvBuffer,
						USHORT *pusRet)
{
	USHORT	usStat, usRet;
    USHORT  usIndexFile;
    RECIDX  RecIdx;
	USHORT	usPreDept = 0;	/* ### MOD (UCHAR->USHORT) 2172 Rel1.0 */

    if (ADJUSTFILE(usFile) == FILE_PLU) {

        usIndexFile = ExeSetIndexFileId(usFile);

       	if (IsLockedFile(usIndexFile, FILE_READABLE + FILE_WRITABLE)) {

    		return (SPLU_FILE_LOCKED);
        }
    }

	/* --- pass the control to executive routine --- */

	usStat = ExeChangeRecordIn(usFile,
							 pvRec,
							 flChanges,
							 fsProg,
							 pvBuffer,
                             &usPreDept, /* ### MOD (UCHAR->USHORT) 2172 Rel1.0 */
							 pusRet);

    /* --- delete and add index record R2.0 --- */

    if ((!usStat) && (ADJUSTFILE(usFile) == FILE_PLU)) {

        /* --- compare dept. no --- */

        if (((RECPLU *)pvRec)->usDeptNumber != usPreDept) { /* ### MOD (UCHAR->USHORT) 2172 Rel1.0 */

            /* delete old dept record */

            SetIndexRecord(&RecIdx, (RECPLU *)pvRec);
          /*RecIdx.usDeptNumber = usPreDept;*/	/* ### DEL 2172 Rel1.0 */

            usStat = ExeDeleteRecordIn(usIndexFile,
                                        &RecIdx,
        			        			 0,
        					        	 0,
    	        						 NULL,
                                         &usPreDept,	/* ### MOD (UCHAR->USHORT) 2172 Rel1.0 */
	        	        				 &usRet);

            if (usStat == SPLU_FILE_NOT_FOUND) {

                return (SPLU_COMPLETED);
            }

            /* add new record */

            SetIndexRecord(&RecIdx, (RECPLU *)pvRec);

	        usStat = ExeAddRecordIn(usIndexFile, &RecIdx);

            if ((usStat == SPLU_FILE_NOT_FOUND) ||
                (usStat == SPLU_REC_DUPLICATED)) {

                usStat = SPLU_COMPLETED;
            }
        }
    }

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Change a record
;
;   format : USHORT		ExeChangeRecordIn(usFile, pvRec, flChanges,
;												fsProg, pvBuffer, pusRet);
;
;   input  : USHORT		usFile;			- file ID
;			 VOID		*pvRec;			- ptr. to record data
;			 ULONG		flChanges;		- change fields flag
;			 USHORT		fsProg;			- programability
;			 VOID		*pvBuffer;		- buffer ptr. to respond old rec.
;			 USHORT		*usRet;			- buffer ptr. to respond rec. length
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	ExeChangeRecordIn(USHORT usFile,
						VOID   *pvRec,
						ULONG  flChanges,
						USHORT fsProg,
						VOID   *pvBuffer,
                        USHORT  *pusDept,	/* ### MOD (UCHAR->USHORT) 2172 Rel1.0 */
						USHORT *pusRet)
{
	BOOL		fPluFile, fResetTotal, fSetVV;
	WCHAR		*pucKey;
	WCHAR		aucRecord[ENOUGHREC];
	WCHAR		*pucStatus;
	USHORT		usStat, usAdj;
	PKEYINF		pTable;
	PRECDIC		pRecord;
    RECIDX		RecIdx;
    /*USHORT		i;*/

	/* --- initialize --- */

	*pusRet     = 0;							/* assume no data returned	*/
	fResetTotal = FALSE;						/* assume no need to reset	*/
	fSetVV      = FALSE;						/* assume no need to set VV	*/

	/* --- is valid file ID given ? --- */
	switch (ADJUSTFILE(usFile)) {				/* what kind of file ?		*/
	case FILE_PLU:								/* PLU file ?				*/
		pucKey   = ((PRECPLU)pvRec)->aucPluNumber;
		usAdj    = (USHORT)(((PRECPLU)pvRec)->uchAdjective);
		fPluFile = TRUE;
		break;										/* these are valid !	*/
	case FILE_MIX_MATCH:						/* Mix-Match file ?			*/
		pucKey   = &(((PRECMIX)pvRec)->uchMixMatch);
		usAdj    = 0;
		fPluFile = FALSE;
		break;										/* these are valid !	*/
#ifdef _DEL_2172_R10 /* ### DEL 2172 Rel1.0 */
	case FILE_PPI:								/* PPI file ?				*/
		pucKey   = &(((PRECPPI)pvRec)->uchPPI);
		usAdj    = 0;
		fPluFile = FALSE;
		break;										/* these are valid !	*/
#endif /* _DEL_2172_R10 */
	case FILE_PLU_INDEX:							/* PLU index file ?		*/
		pucKey   = ((PRECIDX)pvRec)->aucPluNumber;
		usAdj    = (USHORT)(((PRECIDX)pvRec)->uchAdjective);
		fPluFile = FALSE;
		break;										/* these are valid !	*/
	default:									/* others ...				*/
		return (SPLU_INVALID_DATA);					/* invalid file ID !	*/
	}
	/* --- is file locked ? --- */

	if (IsLockedFile(usFile, FILE_READABLE + FILE_WRITABLE)) {
		return (SPLU_FILE_LOCKED);
	}

	/* --- is it a locked rec. ? --- */

	if (IsLockedRecord(usFile, (PITEMNO)pucKey, usAdj)) {
		return (SPLU_REC_LOCKED);
	}

	/* --- query its file info. --- */

	if (usStat = MnpQInfoTable(usFile, &pTable)) {	/* get information table*/
		return (usStat);
	}

	/* --- get file information table ptr. --- */

	pRecord = pTable->pDictionary;				/* record dictionary table	*/

	/* --- try to read it --- */
#ifdef	__DEL_2172	/* ### DEL 2172 Rel1.0 (01/07/00) */
    if (ADJUSTFILE(usFile) == FILE_PLU_INDEX) {

        memcpy(&RecIdx, pvRec, sizeof(RECIDX));

        for (i=0; i<MAX_DEPT; i++) {

            RecIdx.usDeptNumber = (USHORT)i; /* ### 2172 Rel1.0 */

    	    usStat = MnpReadRecord(usFile, HMNP_REC_MAIN, pvRec, 0, pvBuffer);

            if (usStat == 0) {
                break;
            } else 
            if (usStat == SPLU_REC_NOT_FOUND) {
                continue;
            } else {
                return (usStat);
            }
        }

    } else {
#endif	/* ### DEL 2172 Rel1.0 (01/07/00) */
      	if (usStat = MnpReadRecord(usFile,          /* read the record			*/
							   HMNP_REC_MAIN,		/* remember its position*/
							   pucKey,				/* ptr. to key number	*/
							   usAdj,				/* adjective number		*/
							   aucRecord)) {   		/* buffer ptr.			*/
	    	return (usStat);
        }
/*	} ### DEL 2172 Rel1.0 (01/07/00) */

	/* --- respond old record contents --- */

	if (pvBuffer) {								/* is buffer provided ?		*/
		memcpy(pvBuffer, aucRecord, pTable->usBytesRec);/* respond record	*/
		*pusRet = pTable->usBytesRec;					/* its length		*/
	}

    /* set previous dept no. for index file, R2.0 */

    if (ADJUSTFILE(usFile) == FILE_PLU) {

        *pusDept = ((RECPLU *)aucRecord)->usDeptNumber;	/* ### MOD (UCHAR->USHORT) 2172 Rel1.0 */
    }

	/* --- check a record against programmability --- */

	if (fPluFile) {								/* is PLU record ?			*/

		/* --- is protect against active rec. ? --- */
		/* ### DEL 2172 Rel1.0
		if (fsProg & PROG_PROTECT_CHANGE) {

			/* --- is active record ? --- *

			if (((PRECPLU)aucRecord)->lItemCounts
				|| ((PRECPLU)aucRecord)->lSalesTotal
				|| ((PRECPLU)aucRecord)->lDiscTotal) {
				return (SPLU_REC_PROTECTED);
			}
		} ### */

		/* --- change fields related to price field ? --- */

		if ((flChanges & CHGFLD(PLUID_UNIT_PRICE))
			|| (flChanges & CHGFLD(PLUID_PMUL_WEIGHT))) {

			/* --- need to reset total / set VV flag ? --- */

			fResetTotal = (fsProg & PROG_RESET_TOTALS) ? TRUE : FALSE;
			fSetVV      = (fsProg & PROG_SET_VV_FLAG)  ? TRUE : FALSE;
		}
	}

	/* --- modify the record data --- */

	while (pRecord->uchField != (UCHAR)TBLEND) {/* modify specified field	*/

		/* --- is this field modified ? --- */

		if (flChanges & CHGFLD(pRecord->uchField)) {

			/* --- override the data --- */

			memcpy((UCHAR *)aucRecord + pRecord->usOffset,
				   (UCHAR *)pvRec     + pRecord->usOffset,
				   pRecord->usLength);
		}

		/* --- go to next field --- */

		pRecord++;
	}

	/* --- is required to reset total ? --- */

	if (fPluFile && fResetTotal) {				/* reset total fields ?		*/
		/* ### DEL 2172 Rel1.0 ((PRECPLU)aucRecord)->lItemCounts = 0L;	*/	/* reset item counter	*/
		/* ### DEL 2172 Rel1.0 ((PRECPLU)aucRecord)->lSalesTotal = 0L;	*/	/* reset sales total	*/
		/* ### DEL 2172 Rel1.0 ((PRECPLU)aucRecord)->lDiscTotal  = 0L;	*/	/* reset discount total	*/
	}

	/* --- is required to set VV flag ? --- */

	if (fPluFile && fSetVV) {					/* set visual-verify flag ?	*/
		pucStatus   = ((PRECPLU)aucRecord)->aucStatus;	/* base pointer		*/
		pucStatus  += OFSFLG(PLUST_VV_ITEM);			/* compute pointer	*/
		*pucStatus |= BITFLG(PLUST_VV_ITEM);			/* set VV flag		*/
	}

	/* --- write the record data --- */
    
    /* --- change index record R2.0 --- */

    if ((ADJUSTFILE(usFile) == FILE_PLU_INDEX)) {

        usStat = MnpDeleteRecord(usFile, (WCHAR *)&RecIdx, usAdj);    /* delete index file*/

        if ((usStat == SPLU_COMPLETED) ||
            (usStat == SPLU_REC_NOT_FOUND)) {

	        usStat = MnpAddRecord(usFile, pvRec, NULL);	/* add on index file*/
        }
  		return (usStat);

    } else {

    	usStat = MnpModifyRecord(HMNP_REC_MAIN, aucRecord);
    }

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Delete a record
;
;   format : USHORT		ExeDeleteRecord(usFile, pItemNumber, usAdj,
;												fsProg, pvBuffer, pusRet);
;
;   input  : USHORT		usFile;			- file ID
;			 PITEMNO	pItemNumber;	- ptr. to item number
;			 USHORT		usAdj;			- adjective number
;			 USHORT		fsProg;			- programability
;			 VOID		*pvBuffer;		- buffer ptr. to respond old rec.
;			 USHORT		*usRet;			- buffer ptr. to respond rec. length
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	ExeDeleteRecord(USHORT  usFile,
						VOID	*pItemNumber,
						USHORT  usAdj,
						USHORT  fsProg,
						VOID    *pvBuffer,
						USHORT  *pusRet)
{
	USHORT	usStat, usRet;
	USHORT	usIndexFile;
    RECIDX	RecIdx;
	USHORT	usPreDept;	/* ### MOD (UCHAR->USHORT) 2172 Rel1.0 */

	if (ADJUSTFILE(usFile) == FILE_PLU) {

		usIndexFile = ExeSetIndexFileId(usFile);

		if (IsLockedFile(usIndexFile, FILE_READABLE + FILE_WRITABLE)) {

    		return (SPLU_FILE_LOCKED);
        }
    }

	/* --- pass the control to executive routine --- */

	usStat = ExeDeleteRecordIn(usFile,
							 pItemNumber,
							 usAdj,
							 fsProg,
							 pvBuffer,
                             &usPreDept,	/* ### MOD (UCHAR->USHORT) 2172 Rel1.0 */
							 pusRet);

    /* --- delete index record R2.0 --- */

    if ((!usStat) && (ADJUSTFILE(usFile) == FILE_PLU)) {
        
      /*RecIdx.usDeptNumber = usPreDept;*/	/* ### DEL 2172 Rel1.0 (01/07/00) */
        memcpy(&RecIdx.aucPluNumber, pItemNumber, (PLU_NUMBER_LEN * 2)); /* ### MOD 2172 Rel1.0 */
        RecIdx.uchAdjective = (UCHAR)usAdj;

    	usStat = ExeDeleteRecordIn(usIndexFile,
    	        					 &RecIdx,
	    		        			 0,
		    			        	 0,
    		    					 NULL,
                                     &usPreDept,	/* ### MOD (UCHAR->USHORT) 2172 Rel1.0 */
	        	    				 &usRet);

        if ((usStat == SPLU_FILE_NOT_FOUND) ||
            (usStat == SPLU_REC_NOT_FOUND)) {

            usStat = SPLU_COMPLETED;
        }
    }

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Delete a record
;
;   format : USHORT		ExeDeleteRecordIn(usFile, pItemNumber, usAdj,
;												fsProg, pvBuffer, pusRet);
;
;   input  : USHORT		usFile;			- file ID
;			 PITEMNO	pItemNumber;	- ptr. to item number
;			 USHORT		usAdj;			- adjective number
;			 USHORT		fsProg;			- programability
;			 VOID		*pvBuffer;		- buffer ptr. to respond old rec.
;			 USHORT		*usRet;			- buffer ptr. to respond rec. length
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	ExeDeleteRecordIn(USHORT  usFile,
						VOID	*pItemNumber,
						USHORT	usAdj,
						USHORT	fsProg,
						VOID	*pvBuffer,
                        USHORT	*pusDept,	/* ### MOD (UCHAR->USHORT) 2172 Rel1.0 */
						USHORT	*pusRet)
{
	WCHAR		aucRecord[ENOUGHREC];
	USHORT		usStat;
	PRECPLU		pPlu;
	PKEYINF		pTable;
	WCHAR		*pucKey;
    USHORT      usIndAdj;

	/* --- initialize --- */

	*pusRet = 0;								/* assume no data returned	*/

	/* --- is valid file ID given ? --- */
	switch (ADJUSTFILE(usFile)) {				/* what kind of file ?		*/
	case FILE_PLU:								/* PLU file ?				*/
		pPlu = (PRECPLU)aucRecord;					/* mark as PLU file		*/
		break;
	case FILE_MIX_MATCH:						/* Mix-Match file ?			*/
#ifdef _DEL_2172_R10
	case FILE_PPI:								/* PPI file ?				*/
		pPlu = (PRECPLU)NULL;						/* not PLU file			*/
		break;										/* these are valid !	*/
#endif /* _DEL_2172_R10 */
	case FILE_PLU_INDEX:							/* PLU index file ?		*/
		pPlu     = (PRECPLU)NULL;						/* not PLU file			*/
		pucKey   = ((PRECIDX)pItemNumber)->aucPluNumber;
		usIndAdj = (USHORT)(((PRECIDX)pItemNumber)->uchAdjective);
        break;
	default:									/* others ...				*/
		return (SPLU_INVALID_DATA);					/* invalid file ID !	*/
	}
	/* --- is file locked ? --- */

	if (IsLockedFile(usFile, FILE_READABLE + FILE_WRITABLE)) {
		return (SPLU_FILE_LOCKED);
	}

	/* --- is it a locked rec. ? --- */

    if (ADJUSTFILE(usFile) == FILE_PLU_INDEX) {

    	if (IsLockedRecord(usFile, (PITEMNO)pucKey, usIndAdj)) {
		    return (SPLU_REC_LOCKED);
    	}
    } else {
    	if (IsLockedRecord(usFile, (PITEMNO)pItemNumber, usAdj)) {
	    	return (SPLU_REC_LOCKED);
        }
	}

	/* --- query its file info. --- */

	if (usStat = MnpQInfoTable(usFile, &pTable)) {	/* get information table*/
		return (usStat);
	}

    /* --- try to read it --- */

	if (usStat = MnpReadRecord(usFile,		/* read the record			*/
							   0,				/* no need to remember	*/
							   (WCHAR *)pItemNumber,	/* ptr. to key	*/
							   usAdj,			/* adjective number		*/
							   aucRecord)) {	/* buffer ptr.			*/
	    return (usStat);
	}

    /* set previous dept no. for index file, R2.0 */

    if (ADJUSTFILE(usFile) == FILE_PLU) {

        *pusDept = ((RECPLU *)aucRecord)->usDeptNumber;	/* ### MOD (UCHAR->USHORT) 2172 Rel1.0 */
    }

	/* --- is required to protect delete / read old rec. ? --- */

	if ((fsProg & PROG_PROTECT_DELETE) || pvBuffer) {

		/* --- respond old record contents --- */

		if (pvBuffer) {							/* is buffer provided ?		*/
			memcpy(pvBuffer, (UCHAR *)aucRecord, pTable->usBytesRec);/* respond rec.	*/
			*pusRet = pTable->usBytesRec;					/* its length	*/
		}

		/* --- check a record against programmability --- */

		if (pPlu && (fsProg & PROG_PROTECT_DELETE)) {

			/* --- is active record ?, bug fixed at 12/04/96 --- */

			/* ### DEL 2172 Rel1.0 if (pPlu->lItemCounts || pPlu->lSalesTotal || pPlu->lDiscTotal) { */

/*			if (pPlu->lItemCounts || pPlu->lSalesTotal || pPlu->lSalesTotal) {  */

				/* --- cannot delete it ! --- */

			/* ### DEL 2172 Rel1.0	return (SPLU_REC_PROTECTED);
			} */
		}
	}

	/* --- delete the record data --- */

	usStat = MnpDeleteRecord(usFile, (WCHAR *)pItemNumber, usAdj);

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Delete a reserved record
;
;   format : USHORT		ExeDeleteReserved(usHandle);
;
;   input  : USHORT		usHandle;		- handle value
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	ExeDeleteReserved(USHORT usHandle)
{
	USHORT		usStat;
	PUSRBLK		pUser;
    RECIDX		RecIdx;
	FSPECS		specInfo;

	/* --- is valid handle ? --- */

	if ((! usHandle) || (usHandle > ARRAYS(aUserBlock, USRBLK))) {
		return (SPLU_INVALID_HANDLE);
	}

	/* --- evaluate user block table --- */

	pUser = aUserBlock + usHandle - 1;			/* user management block ptr*/

	/* --- is valid file ID ? --- */
	
	switch (ADJUSTFILE(pUser->usFile)) {		/* what kind of file ?		*/
	case FILE_PLU:								/* PLU file ?				*/
	case FILE_MIX_MATCH:						/* Mix-Match file ?			*/
	/*case FILE_PPI:							/* PPI file ?				*//* ### DEL 2172 Rel1.0 */
    case FILE_PLU_INDEX:                        /* PLU index file ? */
		break;										/* these are valid !	*/
	default:									/* others ...				*/
		return (SPLU_INVALID_HANDLE);				/* invalid file ID !	*/
	}

	/* --- is reserved ? --- */

	if (! (pUser->fchControl & USRBLK_IN_USE)) {	/* is not used area ?	*/
		return (SPLU_INVALID_HANDLE);
	}

	/* --- is killed ? --- */

	if (pUser->fchControl & USRBLK_KILLED) {		/* killed by others ?	*/
		return (SPLU_FUNC_KILLED);
	}

	/* --- is any locked record ? --- */

	if (! (pUser->fchControl & USRBLK_REC_LOCKED)) {/* any locked record ?	*/
		return(SPLU_REC_NOT_RESERVED);
	}

	/* --- delete its record --- */

    if ((ADJUSTFILE(pUser->usFile) == FILE_PLU_INDEX)) {

        /* --- delete index record R2.0 --- */

     	memcpy(&RecIdx.aucPluNumber, &(pUser->itemNumber), PLU_NUMBER_LEN); /* ### MOD 2172 Rel1.0 */
        RecIdx.uchAdjective = (UCHAR)pUser->usAdjective;
      /*RecIdx.usDeptNumber = pUser->usDeptNumber;*/	/* ### DEL 2172 Rel1.0 (01/07/00) */

    	usStat = MnpDeleteRecord(pUser->usFile, (WCHAR *)&RecIdx, 0);

    } else {

    	usStat = MnpDeleteRecord(pUser->usFile,			/* delete the record	*/
							 (WCHAR *)(&(pUser->itemNumber)),	/* item no.	*/
							 pUser->usAdjective);		/* adj. number		*/
    }

    /* --- delete index record R2.0 --- */

    if (usStat == SPLU_COMPLETED) {

  	    if (! (usStat = MnpGetFileInfo(pUser->usIndexFile, &specInfo))) {

        	memcpy(&RecIdx.aucPluNumber, &(pUser->itemNumber), PLU_NUMBER_LEN); /* ### MOD 2172 Rel1.0 */
            RecIdx.uchAdjective = (UCHAR)pUser->usAdjective;
          /*RecIdx.usDeptNumber = pUser->usDeptNumber;*/ /* ### DEL 2172 Rel1.0 (01/07/00) */

            usStat = MnpDeleteRecord(pUser->usIndexFile, (WCHAR *)&RecIdx, 0);   /* delete index file*/

            if (usStat == SPLU_REC_NOT_FOUND) {

                usStat = SPLU_COMPLETED;
            }
        }
    }
	/* --- mark as no record locked --- */

	pUser->fchControl &= ~USRBLK_REC_LOCKED;	/* no record locked			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Reset a reserved record
;
;   format : USHORT		ExeResetReserved(usHandle);
;
;   input  : USHORT		usHandle;		- handle value
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	ExeResetReserved(USHORT usHandle)
{
	BOOL		fPlu;
	UCHAR		aucRecord[ENOUGHREC];
	USHORT		usStat;
	PUSRBLK		pUser;

	/* --- is valid handle ? --- */

	if ((! usHandle) || (usHandle > ARRAYS(aUserBlock, USRBLK))) {
		return (SPLU_INVALID_HANDLE);
	}

	/* --- evaluate user block table --- */

	pUser = aUserBlock + usHandle - 1;			/* user management block ptr*/

	/* --- is valid file ID ? --- */
	
	switch (ADJUSTFILE(pUser->usFile)) {		/* what kind of file ?		*/
	case FILE_PLU:								/* PLU file ?				*/
		fPlu = TRUE;								/* mark as PLU file		*/
		break;										/* valid data			*/
	case FILE_MIX_MATCH:						/* Mix-Match file ?			*/
	/*case FILE_PPI:							/* PPI file ?				*//* ### DEL 2172 Rel1.0 */
		fPlu = FALSE;								/* not a PLU file		*/
		break;										/* these are valid !	*/
	default:									/* others ...				*/
		return (SPLU_INVALID_HANDLE);				/* invalid file ID !	*/
	}

	/* --- is reserved ? --- */

	if (! (pUser->fchControl & USRBLK_IN_USE)) {	/* is not used area ?	*/
		return (SPLU_INVALID_HANDLE);
	}

	/* --- is killed ? --- */

	if (pUser->fchControl & USRBLK_KILLED) {		/* killed by others ?	*/
		return (SPLU_FUNC_KILLED);
	}

	/* --- is any locked record ? --- */

	if (! (pUser->fchControl & USRBLK_REC_LOCKED)) {/* any locked record ?	*/
		return(SPLU_REC_NOT_RESERVED);
	}

	/* --- mark as no record locked --- */

	if (pUser->uchFunc == FUNC_INDIV_LOCK) {	/* read individualy w/ lock	*/
		pUser->fchControl &= ~USRBLK_REC_LOCKED;	/* no record locked		*/
	}

	/* --- is record of PLU file ? --- */

	if (! fPlu) {								/* if out of PLU file ...	*/
		return (0);									/* always success		*/
	}

	/* --- read its record --- */

	if (usStat = MnpPeekRecord(pUser->hManip, aucRecord)) {
		return (usStat);
	}

	/* --- reset only total fields --- */

	/* ### DEL 2172 Rel1.0 ((PRECPLU)aucRecord)->lItemCounts = 0L;*/	/* reset item counts		*/
	/* ### DEL 2172 Rel1.0 ((PRECPLU)aucRecord)->lSalesTotal = 0L;*/	/* reset sales total		*/
	/* ### DEL 2172 Rel1.0 ((PRECPLU)aucRecord)->lDiscTotal  = 0L;*/	/* reset discount total		*/

	/* --- override its record --- */

	usStat = MnpModifyRecord(pUser->hManip, aucRecord);

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Rewrite a reserved record
;
;   format : USHORT		ExeRewriteReserved(usHandle, pvRecord);
;
;   input  : USHORT		usHandle;		- handle value
;			 VOID		*pvRecord;		- ptr. to record data
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	ExeRewriteReserved(USHORT usHandle, VOID *pvRecord)
{
	USHORT		usStat;
	PKEYINF		pTable;
	PUSRBLK		pUser;
    RECIDX		RecIdx;
	FSPECS		specInfo;
    USHORT		usFile;

	/* --- is valid handle ? --- */

	if ((! usHandle) || (usHandle > ARRAYS(aUserBlock, USRBLK))) {
		return (SPLU_INVALID_HANDLE);
	}

	/* --- evaluate user block table --- */

	pUser = aUserBlock + usHandle - 1;			/* user management block ptr*/

	/* --- is valid file ID ? --- */
	
	switch (ADJUSTFILE(pUser->usFile)) {		/* what kind of file ?		*/
	case FILE_PLU:								/* PLU file ?				*/
	case FILE_MIX_MATCH:						/* Mix-Match file ?			*/
	/*case FILE_PPI:							/* PPI file ?				*//* ### DEL 2172 Rel1.0 */
    case FILE_PLU_INDEX:                        /* PLU index file ? */
		if (usStat = MnpQInfoTable(pUser->usFile, &pTable)) {
			return (usStat);
		}
		break;										/* these are valid !	*/
	default:									/* others ...				*/
		return (SPLU_INVALID_HANDLE);				/* invalid file ID !	*/
	}

	/* --- is reserved ? --- */

	if (! (pUser->fchControl & USRBLK_IN_USE)) {	/* is not used area ?	*/
		return (SPLU_INVALID_HANDLE);
	}

	/* --- is killed ? --- */

	if (pUser->fchControl & USRBLK_KILLED) {		/* killed by others ?	*/
		return (SPLU_FUNC_KILLED);
	}

	/* --- is any locked record ? --- */

	if (! (pUser->fchControl & USRBLK_REC_LOCKED)) {/* any locked record ?	*/
		return(SPLU_REC_NOT_RESERVED);
	}

	/* --- is valid data ? --- */

	if (memcmp(pvRecord, &(pUser->itemNumber), pTable->usKeyLen)) {
		return (SPLU_INVALID_DATA);
	}

	if (ADJUSTFILE(pUser->usFile) == FILE_PLU) {	/* case of PLU file ...	*/
		if (((PRECPLU)pvRecord)->uchAdjective != (UCHAR)(pUser->usAdjective)){
			return (SPLU_INVALID_DATA);
		}
	}

	/* --- mark as no record locked --- */

	if (pUser->uchFunc == FUNC_INDIV_LOCK) {	/* read individualy w/ lock	*/
		pUser->fchControl &= ~USRBLK_REC_LOCKED;	/* no record locked		*/
	}

	/* --- override its record --- */

    if (ADJUSTFILE(pUser->usFile) == FILE_PLU_INDEX) {

        usStat = SPLU_COMPLETED;    /* change by next step  */

    } else {

	    usStat = MnpModifyRecord(pUser->hManip, pvRecord);
    }

    /* --- change index record R2.0 --- */

    if (usStat == SPLU_COMPLETED) {

        if ((ADJUSTFILE(pUser->usFile) == FILE_PLU) ||
            (ADJUSTFILE(pUser->usFile) == FILE_PLU_INDEX)) {

            if (ADJUSTFILE(pUser->usFile) == FILE_PLU) {

                usFile = pUser->usIndexFile;

            } else {

                usFile = pUser->usFile;
            }

        	if (! (usStat = MnpGetFileInfo(usFile, &specInfo))) {

                /* --- compare dept. no --- */

                /*if (pUser->usDeptNumber != ((RECPLU *)pvRecord)->usDeptNumber) {*//* ### DEL 2172 Rel1.0 (01/07/99) */

                	memcpy(&RecIdx.aucPluNumber, &(pUser->itemNumber), PLU_NUMBER_LEN); /* ### 2172 Rel1.0 */
                    RecIdx.uchAdjective = (UCHAR)pUser->usAdjective;
                  /*RecIdx.usDeptNumber = pUser->usDeptNumber;*/ /* ### DEL 2172 Rel1.0 (01/07/00) */

        	        usStat = MnpDeleteRecord(usFile, (WCHAR *)&RecIdx, 0);	/* delete index file*/

                    if ((usStat == SPLU_COMPLETED) || (usStat == SPLU_REC_NOT_FOUND)) {

                        SetIndexRecord(&RecIdx, (RECPLU *)pvRecord);

                        usStat = MnpAddRecord(usFile, &RecIdx, NULL);	/* add on index file*/
                    }
                /*} ### DEL 2172 Rel1.0 (01/07/00) */
            }
        }
    }

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Cancel a reserved record
;
;   format : USHORT		ExeCancelReserved(usHandle);
;
;   input  : USHORT		usHandle;		- handle value
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	ExeCancelReserved(USHORT usHandle)
{
	PUSRBLK		pUser;

	/* --- is valid handle ? --- */

	if ((! usHandle) || (usHandle > ARRAYS(aUserBlock, USRBLK))) {
		return (SPLU_INVALID_HANDLE);
	}

	/* --- evaluate user block table --- */

	pUser = aUserBlock + usHandle - 1;			/* user management block ptr*/

	/* --- is valid file ID ? --- */
	
	switch (ADJUSTFILE(pUser->usFile)) {		/* what kind of file ?		*/
	case FILE_PLU:								/* PLU file ?				*/
	case FILE_MIX_MATCH:						/* Mix-Match file ?			*/
	/*case FILE_PPI:							/* PPI file ?				*//* ### DEL 2172 Rel1.0 */
    case FILE_PLU_INDEX:                        /* PLU index file ? */
		break;										/* these are valid !	*/
	default:									/* others ...				*/
		return (SPLU_INVALID_HANDLE);				/* invalid file ID !	*/
	}

	/* --- is reserved ? --- */

	if (! (pUser->fchControl & USRBLK_IN_USE)) {	/* is not used area ?	*/
		return (SPLU_INVALID_HANDLE);
	}

	/* --- is killed ? --- */

	if (pUser->fchControl & USRBLK_KILLED) {		/* killed by others ?	*/
		return (SPLU_FUNC_KILLED);
	}

	/* --- is any locked record ? --- */

	if (! (pUser->fchControl & USRBLK_REC_LOCKED)) {/* any locked record ?	*/
		return(SPLU_REC_NOT_RESERVED);
	}

	/* --- mark as no record locked --- */

	if (pUser->uchFunc == FUNC_INDIV_LOCK) {	/* read individualy w/ lock	*/
		pUser->fchControl &= ~USRBLK_REC_LOCKED;	/* no record locked		*/
	}

	/* --- exit ... --- */

	return (0);
}

/**
;========================================================================
;
;   function : Append a maintenance record
;
;   format : USHORT		ExeAppendMainte(usFile, pvRec);
;
;   input  : USHORT		usFile;			- file ID
;			 VOID		*pvRec;			- ptr. to record data
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	ExeAppendMainte(USHORT usFile, VOID *pvRec)
{
	USHORT		usStat, usType, *pusStatus;

	/* --- is valid file ID given ? --- */
	
	switch (ADJUSTFILE(usFile)) {				/* what kind of file ?		*/
	case FILE_MAINT_PLU:						/* PLU maintenance file ?	*/
		usType    = ((PMNTPLU)pvRec)->usMntType;	/* mainte. type			*/
		pusStatus = &(((PMNTPLU)pvRec)->usMntStatus);
		break;										/* these are valid !	*/
	default:									/* others ...				*/
		return (SPLU_INVALID_DATA);					/* invalid file ID !	*/
	}

	/* --- is valid function given ? --- */

	switch (usType) {							/* what function do you do ?*/
	case MNT_TYPE_ADD:							/* add a record ?			*/
	case MNT_TYPE_REPLACE:						/* replace a record ?		*/
	case MNT_TYPE_CHANGE:						/* change a record ?		*/
	case MNT_TYPE_DELETE:						/* delete a record ?		*/
		break;										/* these are valid		*/
	default:									/* others ...				*/
		return (SPLU_INVALID_DATA);					/* invalid data given !	*/
	}

	/* --- is file locked ? --- */

	if (IsLockedFile(usFile, FILE_READABLE + FILE_WRITABLE)) {
		return (SPLU_FILE_LOCKED);
	}

	/* --- refresh status field --- */

	*pusStatus = SPLU_MNT_UNAPPLY;				/* not yet apply it			*/

	/* --- just add it --- */

	usStat = MnpAddRecord(usFile, pvRec, NULL);	/* just add it				*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Purge a maintenance record
;
;   format : USHORT		ExePurgeMainte(usHandle);
;
;   input  : USHORT		usHandle;		- handle value
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	ExePurgeMainte(USHORT usHandle)
{
	BOOL		fApplied;
	WCHAR		aucRecord[ENOUGHREC];
	USHORT		usStat, usLast;
	PUSRBLK		pUser;
	PMNTPLU		pMainte;

	/* --- is valid handle ? --- */

	if ((! usHandle) || (usHandle > ARRAYS(aUserBlock, USRBLK))) {
		return (SPLU_INVALID_HANDLE);
	}

	/* --- evaluate user block table --- */

	pUser = aUserBlock + usHandle - 1;			/* user management block ptr*/

	/* --- is reserved ? --- */

	if (! (pUser->fchControl & USRBLK_IN_USE)) {	/* is not used area ?	*/
		return (SPLU_INVALID_HANDLE);
	}

	/* --- is killed ? --- */

	if (pUser->fchControl & USRBLK_KILLED) {		/* killed by others ?	*/
		return (SPLU_FUNC_KILLED);
	}

	/* --- is valid function ? --- */

	if (pUser->uchFunc != FUNC_PURGE_MAINTE) {	/* purge a mainte. rec. ?	*/
		return (SPLU_FUNC_NOT_AVAIL);				/* invalid function !	*/
	}

	/* --- get record data ptr. --- */

	pMainte = (PMNTPLU)aucRecord;				/* top of data ptr.			*/
	usLast  = pUser->usSearch ? pUser->usSearch : DEF_SEARCH;

	/* --- search a record --- */

	for ( ; usLast; usLast--) {

		/* --- get a next record --- */

		if (usStat = MnpGetLookUp(pUser->hManip, aucRecord)) {
			break;
		}

		/* --- is desired batch number ? --- */

		if (pUser->usBatch && (pUser->usBatch != pMainte->usMntBatch)) {
			continue;
		}

		/* --- is this an applied record ? --- */

		fApplied = (pMainte->usMntStatus == SPLU_MNT_UNAPPLY) ? FALSE : TRUE;

		/* --- is satisfied condition ? --- */

		if (((pUser->fsOption & PURGE_UNAPPLY) && (! fApplied))
			|| ((pUser->fsOption & PURGE_APPLIED) && fApplied)) {

			/* --- purge the record --- */

			if (usStat = MnpRemoveRecord(pUser->hManip)) {
				break;
			}
		}
	}

	/* --- evaluate status --- */

	if (usStat = usLast ? usStat : SPLU_FUNC_CONTINUED) {
		return (usStat);
	}

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Apply a maintenance record
;
;   format : USHORT		ExeApplyMainte(usHandle, pvBuffer, pusRet,
;											pvMainte, pusLen, pusApplied);
;
;   input  : USHORT		usHandle;		- handle value
;			 VOID		*pvBuffer;		- buffer ptr. to respond old rec.
;			 USHORT		*pusRet;		- buffer ptr. to respond rec. length
;			 VOID		*pvMainte;		- buffer ptr. to receive mainte. rec
;			 USHORT		*pusLen;		- buffer ptr. to receive its length
;			 USHORT		*pusApplied;	- buffer ptr. to receive status
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	ExeApplyMainte(USHORT usHandle,
					   VOID   *pvBuffer,
					   USHORT *pusRet,
					   VOID   *pvMainte,
					   USHORT *pusLen,
					   USHORT *pusApplied)
{
	WCHAR		aucRecord[ENOUGHREC];
	USHORT		usStat, usLast, usFile, usBytes;
	PUSRBLK		pUser;
	PMNTPLU		pMainte;

	/* --- initialize --- */

	if (pusLen) {								/* respond record length ?	*/
		*pusLen = 0;								/* initialize it		*/
	}

	/* --- is valid handle ? --- */

	if ((! usHandle) || (usHandle > ARRAYS(aUserBlock, USRBLK))) {
		return (SPLU_INVALID_HANDLE);
	}

	/* --- evaluate user block table --- */

	pUser = aUserBlock + usHandle - 1;			/* user management block ptr*/

	/* --- is reserved ? --- */

	if (! (pUser->fchControl & USRBLK_IN_USE)) {	/* is not used area ?	*/
		return (SPLU_INVALID_HANDLE);
	}

	/* --- is killed ? --- */

	if (pUser->fchControl & USRBLK_KILLED) {		/* killed by others ?	*/
		return (SPLU_FUNC_KILLED);
	}

	/* --- is valid function ? --- */

	if (pUser->uchFunc != FUNC_APPLY_MAINTE) {	/* apply a mainte. rec. ?	*/
		return (SPLU_FUNC_NOT_AVAIL);				/* invalid function !	*/
	}

	/* --- get record data ptr. --- */

	pMainte = (PMNTPLU)aucRecord;				/* top of data ptr.			*/
	usFile  = FILE_PLU;							/* always PLU file			*/
	usBytes = sizeof(MNTPLU);					/* record length			*/
	usLast  = pUser->usSearch ? pUser->usSearch : DEF_SEARCH;

	/* --- search a record --- */

	for ( ; usLast; usLast--) {

		/* --- get a next record --- */

		if (usStat = MnpGetLookUp(pUser->hManip, aucRecord)) {
			break;
		}

		/* --- is desired batch number ? --- */

		if (pUser->usBatch && (pUser->usBatch != pMainte->usMntBatch)) {
			continue;
		}

		/* --- is already applied ? --- */

		if (pMainte->usMntStatus != SPLU_MNT_UNAPPLY) {
			continue;
		}

		/* --- pass a contol to each function --- */
		
		switch (pMainte->usMntType) {			/* what function ?			*/
		case MNT_TYPE_ADD:						/* add a record ?			*/
			usStat  = ExeAddRecord(usFile, pMainte);
			*pusRet = 0;
			break;
		case MNT_TYPE_REPLACE:					/* replace a record ?		*/
			usStat = ExeReplaceRecord(usFile, pMainte, pvBuffer, pusRet);
			break;
		case MNT_TYPE_CHANGE:					/* change a record ?		*/
			usStat = ExeChangeRecord(usFile,
									 pMainte,
									 pMainte->flMntChanges,
									 pUser->fsOption,
									 pvBuffer,
									 pusRet);
			break;
		case MNT_TYPE_DELETE:					/* delete a record ?		*/
			usStat = ExeDeleteRecord(usFile,
									 (PITEMNO)pMainte,
									 pMainte->uchAdjective,
									 pUser->fsOption,
									 pvBuffer,
									 pusRet);
			break;
		default:								/* unknown function ...		*/
			usStat = SPLU_FUNC_NOT_AVAIL;
			break;
		}

		/* --- adjust applied status --- */

		pMainte->usMntStatus = usStat ? usStat : SPLU_MNT_APPLIED;
		*pusApplied          = usStat ? usStat : SPLU_MNT_APPLIED;

		/* --- rewrite its mainte. record --- */

		usStat = MnpModifyRecord(pUser->hManip, aucRecord);

		/* --- required to respond mainte. rec. ? --- */

		if (pvMainte && pusLen) {
			memcpy(pvMainte, aucRecord, usBytes);
			*pusLen = usBytes;
		}

		/* --- break my job --- */

		break;
	}

	/* --- evaluate status --- */

	usStat = usLast ? usStat : SPLU_FUNC_CONTINUED;

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Remove associated maintenance records
;
;   format : USHORT		ExeRemoveMainte(usHandle, ulKey);
;
;   input  : USHORT		usHandle;		- handle
;			 ULONG		ulKey;			- key of record to remove	### MOD 2172 Rel1.0 (USHORT->ULONG)
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	ExeRemoveMainte(USHORT usHandle, ULONG ulKey)
{
	USHORT		usStat;
	ULONG		ulCell;		/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
	PUSRBLK		pUser;

	/* --- is valid handle ? --- */

	if ((! usHandle) || (usHandle > ARRAYS(aUserBlock, USRBLK))) {
		return (SPLU_INVALID_HANDLE);
	}

	/* --- evaluate user block table --- */

	pUser = aUserBlock + usHandle - 1;			/* user management block ptr*/

	/* --- is reserved ? --- */

	if (! (pUser->fchControl & USRBLK_IN_USE)) {	/* is not used area ?	*/
		return (SPLU_INVALID_HANDLE);
	}

	/* --- is killed ? --- */

	if (pUser->fchControl & USRBLK_KILLED) {		/* killed by others ?	*/
		return (SPLU_FUNC_KILLED);
	}

	/* --- is valid function ? --- */

	if (pUser->uchFunc != FUNC_BOUND_MAINTE) {	/* bound mainte. rec. ?		*/
		return (SPLU_FUNC_NOT_AVAIL);				/* invalid function !	*/
	}

	/* --- compute cell no. from the key --- */

	ulCell = ulKey - ADJUST_BOUND_KEY;			/* compute cell no.			*/

	/* --- adjust file ptr. --- */

	if (usStat = MnpSetFilePtr(pUser->hManip, ulCell)) {
		return (usStat);
	}

	/* --- remove the record --- */

	usStat = MnpRemoveRecord(pUser->hManip);

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Modify associated maintenance records
;
;   format : USHORT		ExeModifyMainte(usHandle, ulKey, pvRec);
;
;   input  : USHORT		usHandle;		- handle
;			 ULONG		ulKey;			- key of record to modify ### MOD 2172 Rel1.0 (USHORT->ULONG)
;			 VOID		*pvRec;			- ptr. to record data
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	ExeModifyMainte(USHORT usHandle, ULONG ulKey, VOID *pvRec)
{
	USHORT		usStat, usType, *pusStatus;
	ULONG		ulCell;	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
	PUSRBLK		pUser;

	/* --- is valid handle ? --- */

	if ((! usHandle) || (usHandle > ARRAYS(aUserBlock, USRBLK))) {
		return (SPLU_INVALID_HANDLE);
	}

	/* --- evaluate user block table --- */

	pUser = aUserBlock + usHandle - 1;			/* user management block ptr*/

	/* --- is reserved ? --- */

	if (! (pUser->fchControl & USRBLK_IN_USE)) {	/* is not used area ?	*/
		return (SPLU_INVALID_HANDLE);
	}

	/* --- is killed ? --- */

	if (pUser->fchControl & USRBLK_KILLED) {		/* killed by others ?	*/
		return (SPLU_FUNC_KILLED);
	}

	/* --- is valid function ? --- */

	if (pUser->uchFunc != FUNC_BOUND_MAINTE) {	/* bound mainte. rec. ?		*/
		return (SPLU_FUNC_NOT_AVAIL);				/* invalid function !	*/
	}

	/* --- is valid file ID given ? --- */
	
	switch (ADJUSTFILE(pUser->usFile)) {		/* what kind of file ?		*/
	case FILE_MAINT_PLU:						/* PLU maintenance file ?	*/
		usType    = ((PMNTPLU)pvRec)->usMntType;	/* mainte. type			*/
		pusStatus = &(((PMNTPLU)pvRec)->usMntStatus);
		break;										/* these are valid !	*/
	default:									/* others ...				*/
		return (SPLU_INVALID_DATA);					/* invalid file ID !	*/
	}

	/* --- is valid function given ? --- */

	switch (usType) {							/* what function do you do ?*/
	case MNT_TYPE_ADD:							/* add a record ?			*/
	case MNT_TYPE_REPLACE:						/* replace a record ?		*/
	case MNT_TYPE_CHANGE:						/* change a record ?		*/
	case MNT_TYPE_DELETE:						/* delete a record ?		*/
		*pusStatus = SPLU_MNT_UNAPPLY;				/* not yet apply it		*/
		break;										/* these are valid		*/
	default:									/* others ...				*/
		return (SPLU_INVALID_DATA);					/* invalid data given !	*/
	}

	/* --- compute cell no. from the key --- */

	ulCell = ulKey - ADJUST_BOUND_KEY;			/* compute cell no.			*/

	/* --- adjust file ptr. --- */

	if (usStat = MnpSetFilePtr(pUser->hManip, ulCell)) {
		return (usStat);
	}

	/* --- modify the record --- */

	usStat = MnpModifyRecord(pUser->hManip, pvRec);

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Read associated maintenance records
;
;   format : USHORT		ExeReadMainte(usHandle, ulKey, pvBuffer, pusRet);
;
;   input  : USHORT		usHandle;		- handle
;			 ULONG		ulKey;			- key of record to modify ### MOD 2172 Rel1.0 (USHORT->ULONG)
;			 VOID		*pvBuffer;		- buffer ptr. to respond record
;			 USHORT		*pusRet;		- buffer ptr. to respond rec. len.
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/


USHORT	ExeReadMainte(USHORT usHandle, ULONG ulKey, VOID *pvBuffer, USHORT *pusRet)
{
	USHORT		usStat;
	ULONG		ulCell;	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
	PKEYINF		pTable;
	PUSRBLK		pUser;

	/* --- initialize --- */

	*pusRet = 0;								/* assume no data returned	*/

	/* --- is valid handle ? --- */

	if ((! usHandle) || (usHandle > ARRAYS(aUserBlock, USRBLK))) {
		return (SPLU_INVALID_HANDLE);
	}

	/* --- evaluate user block table --- */

	pUser = aUserBlock + usHandle - 1;			/* user management block ptr*/

	/* --- is reserved ? --- */

	if (! (pUser->fchControl & USRBLK_IN_USE)) {	/* is not used area ?	*/
		return (SPLU_INVALID_HANDLE);
	}

	/* --- is killed ? --- */

	if (pUser->fchControl & USRBLK_KILLED) {		/* killed by others ?	*/
		return (SPLU_FUNC_KILLED);
	}

	/* --- is valid function ? --- */

	if (pUser->uchFunc != FUNC_BOUND_MAINTE) {	/* bound mainte. rec. ?		*/
		return (SPLU_FUNC_NOT_AVAIL);				/* invalid function !	*/
	}

	/* --- compute cell no. from the key --- */

	ulCell = ulKey - ADJUST_BOUND_KEY;			/* compute cell no.			*/

	/* --- query its file info. --- */

	if (usStat = MnpQInfoTable(pUser->usFile, &pTable)) {
		return (usStat);
	}

	/* --- adjust file ptr. --- */

	if (usStat = MnpSetFilePtr(pUser->hManip, ulCell)) {
		return (usStat);
	}

	/* --- peek the record --- */

	if (! (usStat = MnpPeekRecord(pUser->hManip, pvBuffer))) {
		*pusRet = pTable->usBytesRec;
	}

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Add associated maintenance records
;
;   format : USHORT		ExeAddMainte(usHandle, pvRec, pulKey);
;
;   input  : USHORT		usHandle;		- handle
;			 VOID		*pvRec;			- ptr. to record data
;			 ULONG		*pulKey;		- buffer ptr. to respond key ### MOD 2172 Rel1.0 (USHORT->ULONG)
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	ExeAddMainte(USHORT usHandle, VOID *pvRec, ULONG *pulKey)
{
	USHORT		usStat, usType, *pusStatus;
	ULONG		ulCell;	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
	PUSRBLK		pUser;

	/* --- is valid handle ? --- */

	if ((! usHandle) || (usHandle > ARRAYS(aUserBlock, USRBLK))) {
		return (SPLU_INVALID_HANDLE);
	}

	/* --- evaluate user block table --- */

	pUser = aUserBlock + usHandle - 1;			/* user management block ptr*/

	/* --- is reserved ? --- */

	if (! (pUser->fchControl & USRBLK_IN_USE)) {	/* is not used area ?	*/
		return (SPLU_INVALID_HANDLE);
	}

	/* --- is killed ? --- */

	if (pUser->fchControl & USRBLK_KILLED) {		/* killed by others ?	*/
		return (SPLU_FUNC_KILLED);
	}

	/* --- is valid function ? --- */

	if (pUser->uchFunc != FUNC_BOUND_MAINTE) {	/* bound mainte. rec. ?		*/
		return (SPLU_FUNC_NOT_AVAIL);				/* invalid function !	*/
	}

	/* --- is valid file ID given ? --- */
	
	switch (ADJUSTFILE(pUser->usFile)) {		/* what kind of file ?		*/
	case FILE_MAINT_PLU:						/* PLU maintenance file ?	*/
		usType    = ((PMNTPLU)pvRec)->usMntType;	/* record ptr.			*/
		pusStatus = &(((PMNTPLU)pvRec)->usMntStatus);
		break;										/* these are valid !	*/
	default:									/* others ...				*/
		return (SPLU_INVALID_DATA);					/* invalid file ID !	*/
	}

	/* --- is valid function given ? --- */

	switch (usType) {							/* what function do you do ?*/
	case MNT_TYPE_ADD:							/* add a record ?			*/
	case MNT_TYPE_REPLACE:						/* replace a record ?		*/
	case MNT_TYPE_CHANGE:						/* change a record ?		*/
	case MNT_TYPE_DELETE:						/* delete a record ?		*/
		break;										/* these are valid		*/
	default:									/* others ...				*/
		return (SPLU_INVALID_DATA);					/* invalid data given !	*/
	}

	/* --- refresh status field --- */

	*pusStatus = SPLU_MNT_UNAPPLY;				/* not yet apply it			*/

	/* --- just add it --- */

	usStat  = MnpAddRecord(pUser->usFile, pvRec, &ulCell);
	*pulKey = ulCell + ADJUST_BOUND_KEY;		/* compute cell no.			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Read a record
;
;   format : USHORT
;			 ExeReadRecord(usFile, pItemNumber, usAdj, pvBuffer, pusRet);
;
;   input  : USHORT		usFile;			- file ID
;			 PITEMNO	pItemNumber;	- ptr. to item number
;			 USHORT		usAdj;			- adjective number
;			 VOID		*pvBuffer;		- buffer ptr. to respond rec.
;			 USHORT		*pusRet;		- buffer ptr. to respond rec. length
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	ExeReadRecord(USHORT  usFile,
					  PITEMNO pItemNumber,
					  USHORT  usAdj,
					  VOID    *pvBuffer,
					  USHORT  *pusRet)
{
	USHORT		usStat;
	PKEYINF		pTable;

	/* --- initialize --- */

	*pusRet = 0;								/* assume no data returned	*/

	/* --- is valid file ID given ? --- */
	
	switch (ADJUSTFILE(usFile)) {				/* what kind of file ?		*/
	case FILE_PLU:								/* PLU file ?				*/
	case FILE_OLD_PLU:
	case FILE_MIX_MATCH:						/* Mix-Match file ?			*/
	/*case FILE_PPI:							/* PPI file ?				*//* ### DEL 2172 Rel1.0 */
    case FILE_PLU_INDEX:                       /* PLU Index file ? , R2.0  */
		break;										/* these are valid !	*/
	default:									/* others ...				*/
		return (SPLU_INVALID_DATA);					/* invalid file ID !	*/
	}

	/* --- is file locked ? --- */

	if (IsLockedFile(usFile, FILE_READABLE)) {	/* can be read from a file ?*/
		return (SPLU_FILE_LOCKED);					/* No ! being locked !	*/
	}

	/* --- query its file info. --- */

	if (usStat = MnpQInfoTable(usFile, &pTable)) {	/* get information table*/
		return (usStat);
	}

	/* --- read the record --- */

	if (usStat = MnpReadRecord(usFile,			/* read the record			*/
							   0,					/* manipulation handle	*/
							   (WCHAR *)pItemNumber,/* ptr. to key			*/
							   usAdj,				/* adjective number		*/
							   pvBuffer)) {			/* buffer ptr.			*/
		return (usStat);
	}

	/* --- respond record length --- */

	*pusRet = pTable->usBytesRec;				/* record length in byte	*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Read a record and reserve it
;
;   format : USHORT
;			 ExeReserveRecord(usHandle, pItemNumber, usAdj, pvBuffer, pusRet);
;
;   input  : USHORT		usHandle;		- handle value
;			 PITEMNO	pItemNumber;	- ptr. to item number
;			 USHORT		usAdj;			- adjective number
;			 VOID		*pvBuffer;		- buffer ptr. to respond rec.
;			 USHORT		*pusRet;		- buffer ptr. to respond rec. length
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	ExeReserveRecord(USHORT  usHandle,
						 PITEMNO pItemNumber,
						 USHORT  usAdj,
						 VOID    *pvBuffer,
						 USHORT  *pusRet)
{
	USHORT		usStat;
	PUSRBLK		pUser;
	PKEYINF		pTable;

	/* --- initialize --- */

	*pusRet = 0;								/* assume no data returned	*/

	/* --- is valid handle ? --- */

	if ((! usHandle) || (usHandle > ARRAYS(aUserBlock, USRBLK))) {
		return (SPLU_INVALID_HANDLE);
	}

	/* --- evaluate user block table --- */

	pUser = aUserBlock + usHandle - 1;			/* user management block ptr*/

	/* --- is reserved ? --- */

	if (! (pUser->fchControl & USRBLK_IN_USE)) {	/* is not used area ?	*/
		return (SPLU_INVALID_HANDLE);
	}

	/* --- is killed ? --- */

	if (pUser->fchControl & USRBLK_KILLED) {		/* killed by others ?	*/
		return (SPLU_FUNC_KILLED);
	}

	/* --- is valid function ? --- */

	if (pUser->uchFunc != FUNC_INDIV_LOCK) {	/* out of indiv. lock ?		*/
		return (SPLU_FUNC_NOT_AVAIL);				/* not support !		*/
	}

	/* --- release current locked record --- */

	pUser->fchControl &= ~USRBLK_REC_LOCKED;	/* free current lock		*/

	/* --- is it a locked rec. ? --- */

	if (IsLockedRecord(pUser->usFile, pItemNumber, usAdj)) {
		return (SPLU_REC_LOCKED);
	}

	/* --- query its file info. --- */

	if (usStat = MnpQInfoTable(pUser->usFile, &pTable)) {
		return (usStat);
	}

	/* --- read the record --- */

	if (usStat = MnpReadRecord(pUser->usFile,	/* read the record			*/
							   pUser->hManip,		/* manipulation handle	*/
							   (WCHAR *)pItemNumber,/* ptr. to key			*/
							   usAdj,				/* adjective number		*/
							   pvBuffer)) {			/* buffer ptr.			*/
		return (usStat);
	}

	/* --- respond record length --- */

	*pusRet = pTable->usBytesRec;				/* record length in byte	*/

	/* --- set the record locked --- */

	memcpy(&(pUser->itemNumber), pItemNumber, pTable->usKeyLen);
	pUser->usAdjective = usAdj;					/* remember key & adj. no.	*/
  /*pUser->usDeptNumber = ((RECPLU *)pvBuffer)->usDeptNumber;*//* ### DEL 2172 Rel1.0 (01/07/00) *//* remember dept. no for index R2.0 */
	pUser->fchControl |= USRBLK_REC_LOCKED;		/* make record locked		*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Report a record sequentialy
;
;   format : USHORT		ExeReportRecord(usHandle, pvBuffer, pulKey, pusRet);
;
;   input  : USHORT		usHandle;		- handle value
;			 VOID		*pvBuffer;		- buffer ptr. to respond rec.
;			 ULONG		*pulKey;		- key no. reported	### MOD 2172 Rel1.0 (USHORT->ULONG)
;			 USHORT		*pusRet;		- buffer ptr. to respond rec. length
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	ExeReportRecord(USHORT usHandle, VOID *pvBuffer, ULONG *pulKey, USHORT *pusRet)
{
	BOOL		fReset, fSatisfy, fLock, fActive, fAttached, fRetry;
	WCHAR		aucRecord[sizeof(RECPLU)];
	USHORT		usStat, usLast, usBatch;
	ULONG		ulCell;		/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
	PUSRBLK		pUser;
	PKEYINF		pTable;
	PRECDIC		pRecord;
	FSPECS	    specInfo;
    /*RECIDX      RecIdx;*//* ### DEL 2172 Rel1.0 (01/07/00) */

	/* --- initialize --- */

	*pusRet = 0;								/* assume no data returned	*/
	fReset  = FALSE;							/* assume not reset previous*/

	/* --- is valid handle ? --- */

	if ((! usHandle) || (usHandle > ARRAYS(aUserBlock, USRBLK))) {
		return (SPLU_INVALID_HANDLE);
	}

	/* --- evaluate user block table --- */

	pUser = aUserBlock + usHandle - 1;			/* user management block ptr*/

	/* --- is reserved ? --- */

	if (! (pUser->fchControl & USRBLK_IN_USE)) {	/* is not used area ?	*/
		return (SPLU_INVALID_HANDLE);
	}

	/* --- is killed ? --- */

	if (pUser->fchControl & USRBLK_KILLED) {		/* killed by others ?	*/
		return (SPLU_FUNC_KILLED);
	}

	/* --- is valid function ? --- */

	switch (pUser->uchFunc) {
	case FUNC_REPORT_RANGE:						/* report rec. by range ?	*/
	case FUNC_REPORT_RANGE2:					/* report rec. by range ?	*/
	case FUNC_REPORT_ADJECT:					/* report adjective rec. ?	*/
	case FUNC_REPORT_COND:						/* report rec. w/ cond. ?	*/
	case FUNC_REPORT_COND2:						/* report rec. w/ cond. ?	*/
		fReset = (pUser->fsOption & REPORT_AS_RESET)     ? TRUE : FALSE;
		fLock  = (pUser->fsOption & REPORT_WITH_LOCK)    ? TRUE : fReset;
		fRetry = (pUser->fsOption & REPORT_RETRY_LOCKED) ? TRUE : FALSE;
		break;										/* valid function		*/
    case FUNC_REPORT_INDEX:                     /* reort only index record ? */
    case FUNC_REPORT_R10:                       /* report by R1.0 record ?  */
		fReset = FALSE;
		fLock  = (pUser->fsOption & REPORT_WITH_LOCK)    ? TRUE : fReset;
		fRetry = (pUser->fsOption & REPORT_RETRY_LOCKED) ? TRUE : FALSE;
        break;
	case FUNC_REPORT_MAINTE:					/* report mainte. rec. ?	*/
	case FUNC_BOUND_MAINTE:						/* bound mainte. rec. ?		*/
		fReset = FALSE;
		fLock  = FALSE;
		fRetry = FALSE;
		break;										/* valid function		*/
	default:									/* others ...				*/
		return (SPLU_FUNC_NOT_AVAIL);				/* invalid function !	*/
	}

	/* --- evaluate record reset condition --- */

	fReset = (pUser->usFile == FILE_PLU)              ? fReset : FALSE;
	fReset = (pUser->fchControl & USRBLK_REC_LOCKED)  ? fReset : FALSE;

	/* --- release current locked record --- */

	pUser->fchControl &= ~USRBLK_REC_LOCKED;	/* free current lock		*/

	/* --- require to reset its total ? --- */

	if (fReset) {

		/* --- read its record --- */

		if (usStat = MnpPeekRecord(pUser->hManip, aucRecord)) {
			return (usStat);
		}

		/* --- reset only total fields --- */

		/* ### DEL 2172 Rel1.0 ((PRECPLU)aucRecord)->lItemCounts = 0L;*/	/* reset item counts	*/
		/* ### DEL 2172 Rel1.0 ((PRECPLU)aucRecord)->lSalesTotal = 0L;*/	/* reset sales total	*/
		/* ### DEL 2172 Rel1.0 ((PRECPLU)aucRecord)->lDiscTotal  = 0L;*/	/* reset discount total	*/

		/* --- override its record --- */

		if (usStat = MnpModifyRecord(pUser->hManip, aucRecord)) {
			return (usStat);
		}
	}

	/* --- query its file info. --- */

	if (usStat = MnpQInfoTable(pUser->usFile, &pTable)) {
		return (usStat);
	}

	/* --- evaluate dictionary table --- */

	pRecord = pTable->pDictionary;				/* dictionary table ptr.	*/
	usLast  = pUser->usSearch ? pUser->usSearch : DEF_SEARCH;

	/* --- search a record --- */

	for ( ; usLast; usLast--) {

		/* --- get a next record --- */

 		if (usStat = MnpGetLookUp(pUser->hManip, aucRecord)) {
			break;
		}

		/* --- is satisfied record ? --- */

		switch (pUser->uchFunc) {				/* what report function ?	*/
		case FUNC_REPORT_RANGE:					/* report rec. by range ?	*/
		case FUNC_REPORT_RANGE2:				/* report rec. by range ?	*/
		case FUNC_REPORT_ADJECT:				/* report adjective rec. ?	*/
		case FUNC_REPORT_COND:					/* report rec. w/ cond. ?	*/
		case FUNC_REPORT_COND2:					/* report rec. w/ cond. ?	*/
			fActive  = IsActiveRecord(pUser->usFile, aucRecord);
			fSatisfy = ((pUser->fsOption & REPORT_ACTIVE)   &&    fActive)
															? TRUE : FALSE;
			fSatisfy = ((pUser->fsOption & REPORT_INACTIVE) && (! fActive))
															? TRUE : fSatisfy;
			break;
		case FUNC_REPORT_MAINTE:				/* report mainte. rec. ?	*/
			usBatch  = GetBatchNumber(pUser->usFile, aucRecord);
			fSatisfy = ((! pUser->usBatch) || (pUser->usBatch == usBatch))
															? TRUE : FALSE;
			break;
		case FUNC_BOUND_MAINTE:					/* bound mainte. rec. ?		*/
		default:								/* others ...				*/
			fSatisfy = TRUE;						/* always satisfy		*/
			break;
		}

		/* --- is defined conditional expression ? --- */

		if (fSatisfy && pUser->usLength) {
			fSatisfy = IsSatisfiedRecord(aucRecord,
										 pUser->aucFuncData,
										 pRecord);
		}

		/* --- break if found a satisfied record --- */

		if (fSatisfy) {							/* could be satisfied ?		*/
			break;									/* break my job			*/
		}

		/* --- is report of adjective rec. ? --- */

		if (pUser->uchFunc == FUNC_REPORT_ADJECT) {	/* report adj. rec. ?	*/

			/* --- try to test the key number, to make sure --- */

			if (! IsSatisfiedRecord(aucRecord, pUser->aucFuncData, pRecord)) {
				usStat = SPLU_END_OF_FILE;				/* assume EOF		*/
				break;									/* break my job		*/
			}
		}

        /* ---- move read pointer, if index file is provided, R2.0 ---- */

        /* for AC29/39 Type 2 Report */

        if (pUser->uchFunc == FUNC_REPORT_COND) {
      	    if (! (usStat = MnpGetFileInfo(pUser->usIndexFile, &specInfo))) {
                if (pUser->usLength) {
            		if ((pUser->aucFuncData[0] == PLUID_DEPT_NUMBER) &&
	            	    (pUser->aucFuncData[2] & (COMP_EQUAL|LOGIC_END))) {

            			if (!(IsSatisfiedRecord(aucRecord,
			        							 pUser->aucFuncData,
					        					 pRecord))) {

    				        usStat = SPLU_END_OF_FILE;				/* assume EOF		*/
        	    			break;									/* break my job		*/
                        }
                    }
                }
            }
        }

        /* for PEP PLU Parameter Report */
#ifdef	__DEL_2172	/* ### DEL 2172 Rel1.0 (01/07/00)  */
        if (pUser->uchFunc == FUNC_REPORT_RANGE) {
      	    if (! (usStat = MnpGetFileInfo(pUser->usIndexFile, &specInfo))) {
                if (pUser->usLength) {

         			if (IsSatisfiedRecord(aucRecord,
	        							 pUser->aucFuncData,
			        					 pRecord)) {

                        continue;
                    }
                    for (++pUser->usDeptNumber; pUser->usDeptNumber <MAX_DEPT; pUser->usDeptNumber++) { /* ### MOD 2172 Rel1.0 */

                        RecIdx.usDeptNumber = pUser->usDeptNumber; /* ### MOD 2172 Rel1.0 */
                        memcpy(&RecIdx.aucPluNumber, &pUser->aucFuncData[1], pTable->usKeyLen);
                        RecIdx.uchAdjective = 0;

                        if (!(usStat = MnpMoveFilePtr(pUser->hManip, &RecIdx))) {

                            break;
                        }
                    }
                }
                if (pUser->usDeptNumber > MAX_DEPT) { /* ### MOD 2172 Rel1.0 */
       				usStat = SPLU_END_OF_FILE;				/* assume EOF		*/
	    			break;									/* break my job		*/
                }
            }
        }
#endif /* ### DEL 2000.1.7 */
	}
	/* --- evaluate status --- */

	if (usStat = usLast ? usStat : SPLU_FUNC_CONTINUED) {
		return (usStat);
	}

	/* --- respond record data --- */
    if ((pUser->uchFunc == FUNC_REPORT_R10) &&
        (ADJUSTFILE(pUser->usFile) == FILE_PLU)) {

  	    *pusRet = sizeof(RECPLU22);                 /* record length in byte	*/
        memcpy(pvBuffer, aucRecord, sizeof(RECPLU22));

    } else {

      	*pusRet = pTable->usBytesRec;				/* record length in byte	*/
        memcpy(pvBuffer, aucRecord, pTable->usBytesRec);
    }

	/* --- required to respond cell no. ? --- */

	if (pulKey) {
		usStat  = MnpGetFilePtr(pUser->hManip, &ulCell);
		*pulKey	= ulCell + ADJUST_BOUND_KEY;
	}

	/* --- set the record locked --- */

	if (fLock) {

		/* --- remember item number --- */

		memcpy(&(pUser->itemNumber), aucRecord, pTable->usKeyLen);

		/* --- rememeber adjective number --- */

		if ((pUser->usFile == FILE_PLU) ||
		    (pUser->usFile == FILE_PLU_INDEX)) {

			pUser->usAdjective = (USHORT)(((PRECPLU)aucRecord)->uchAdjective);
          /*pUser->usDeptNumber = ((RECPLU *)aucRecord)->usDeptNumber;*//* ### DEL 2172 Rel1.0 (01/07/00) *//* remember dept. no for index R2.0 */

		} else {

			pUser->usAdjective = 0;
		}

		/* --- is this locked already ? --- */

		fAttached = IsLockedRecord(pUser->usFile,	/* someone locked this ?*/
								   &(pUser->itemNumber),	/* ptr. to item	*/
								   pUser->usAdjective);		/* adjective	*/

		/* --- set it locked --- */

		if (! fAttached) {						/* if lockable rec. ...		*/
			pUser->fchControl |= USRBLK_REC_LOCKED;	/* make record locked	*/
		} else if (fRetry) {					/* else, unlockable now ...	*/
			MnpKeepFilePtr(pUser->hManip);			/* try it later			*/
		}

		/* --- adjust response status --- */

		usStat = fAttached ? SPLU_REC_LOCKED : usStat;	/* its status		*/
	}

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Read from a file
;
;   format : USHORT		ExeReadBlock(usHandle, pucBuffer, usSize, pusRead);
;
;   input  : USHORT		usHandle;		- handle value
;			 UCHAR		*pucBuffer;		- output buffer ptr.
;			 USHORT		usSize;			- output buffer size
;			 USHORT		*pusRead;		- no. of bytes to have been read
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/


USHORT	ExeReadBlock(USHORT usHandle, WCHAR *pucBuffer, USHORT usSize, USHORT *pusRead)
{
	USHORT		usStat;
	PUSRBLK		pUser;

	/* --- initialize --- */

	*pusRead = 0;								/* assume no data returned	*/

	/* --- is valid handle ? --- */

	if ((! usHandle) || (usHandle > ARRAYS(aUserBlock, USRBLK))) {
		return (SPLU_INVALID_HANDLE);
	}

	/* --- evaluate user block table --- */

	pUser = aUserBlock + usHandle - 1;			/* user management block ptr*/

	/* --- is reserved ? --- */

	if (! (pUser->fchControl & USRBLK_IN_USE)) {	/* is not used area ?	*/
		return (SPLU_INVALID_HANDLE);
	}

	/* --- is killed ? --- */

	if (pUser->fchControl & USRBLK_KILLED) {		/* killed by others ?	*/
		return (SPLU_FUNC_KILLED);
	}

	/* --- is valid function ? --- */

	if (pUser->uchFunc != FUNC_BLOCK_ACCESS) {	/* out of function ?		*/
		return (SPLU_FUNC_NOT_AVAIL);				/* not support !		*/
	}

	/* --- does the user have access to read --- */

	if (! (pUser->fsOption & BLOCK_READ)) {		/* no access to read		*/
		return (SPLU_FUNC_NOT_AVAIL);				/* not support !		*/
	}

	/* --- read from the file --- */

	usStat = MnpReadFile(pUser->hManip, pucBuffer, usSize, pusRead);

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Write to a file
;
;   format : USHORT		ExeWriteBlock(usHandle, pucData, usBytes, pusWritten);
;
;   input  : USHORT		usHandle;		- handle value
;			 UCHAR		*pucData;		- data buffer ptr.
;			 USHORT		usBytes;		- data length
;			 USHORT		*pusWritten;	- no. of bytes to have been written
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/


USHORT	ExeWriteBlock(USHORT usHf, WCHAR *pucData, USHORT usBytes, USHORT *pusWritten)
{
	USHORT		usStat;
	PUSRBLK		pUser;

	/* --- initialize --- */

	*pusWritten = 0;							/* assume no data written	*/

	/* --- is valid handle ? --- */

	if ((! usHf) || (usHf > ARRAYS(aUserBlock, USRBLK))) {
		return (SPLU_INVALID_HANDLE);
	}

	/* --- evaluate user block table --- */

	pUser = aUserBlock + usHf - 1;				/* user management block ptr*/

	/* --- is reserved ? --- */

	if (! (pUser->fchControl & USRBLK_IN_USE)) {	/* is not used area ?	*/
		return (SPLU_INVALID_HANDLE);
	}

	/* --- is killed ? --- */

	if (pUser->fchControl & USRBLK_KILLED) {		/* killed by others ?	*/
		return (SPLU_FUNC_KILLED);
	}

	/* --- is valid function ? --- */

	if (pUser->uchFunc != FUNC_BLOCK_ACCESS) {	/* out of function ?		*/
		return (SPLU_FUNC_NOT_AVAIL);				/* not support !		*/
	}

	/* --- does the user have access to write --- */

	if (! (pUser->fsOption & BLOCK_WRITE)) {	/* no access to write		*/
		return (SPLU_FUNC_NOT_AVAIL);				/* not support !		*/
	}

	/* --- write to the file --- */

	usStat = MnpWriteFile(pUser->hManip, pucData, usBytes, pusWritten);

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Seek a file pointer
;
;   format : USHORT		ExeSeekBlock(usHandle, lMoves, usOrigin, pulNewPtr);
;
;   input  : USHORT		usHandle;		- handle value
;			 LONG		lMoves;			- value to move
;			 USHORT		usOrigin;		- origin parameter
;			 ULONG		ulNewPtr;		- new file pointer
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/


USHORT	ExeSeekBlock(USHORT usHandle, LONG lMoves, USHORT usOrigin, ULONG *pulNewPtr)
{
	USHORT		usStat;
	PUSRBLK		pUser;

	/* --- is valid handle ? --- */

	if ((! usHandle) || (usHandle > ARRAYS(aUserBlock, USRBLK))) {
		return (SPLU_INVALID_HANDLE);
	}

	/* --- evaluate user block table --- */

	pUser = aUserBlock + usHandle - 1;			/* user management block ptr*/

	/* --- is reserved ? --- */

	if (! (pUser->fchControl & USRBLK_IN_USE)) {	/* is not used area ?	*/
		return (SPLU_INVALID_HANDLE);
	}

	/* --- is killed ? --- */

	if (pUser->fchControl & USRBLK_KILLED) {		/* killed by others ?	*/
		return (SPLU_FUNC_KILLED);
	}

	/* --- is valid function ? --- */

	if (pUser->uchFunc != FUNC_BLOCK_ACCESS) {	/* out of function ?		*/
		return (SPLU_FUNC_NOT_AVAIL);				/* not support !		*/
	}

	/* --- write to the file --- */

	usStat = MnpSeekFile(pUser->hManip, lMoves, usOrigin, pulNewPtr);

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Update a record
;
;   format : USHORT		ExeUpdateRecord(usFile, pNumber, usAdj, pMainInfo,
;											pLinkInfo, pvBuffer, pfsStatus);
;
;   input  : USHORT		usHandle;		- handle value
;			 PITEMNO	pNumber;		- ptr. to item number
;			 USHORT		usAdj;			- adjective number
;			 PUPDINF	pMainInfo;		- ptr. to update info. for main rec.
;			 PUPDINF	pLinkInfo;		- ptr. to update info. for link rec.
;			 VOID		*pvBuffer;		- buffer ptr. to respond rec.
;			 USHORT		*pfsStatus;		- buffer ptr. to respond status
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	ExeUpdateRecord(USHORT  usFile, PITEMNO pNumber, USHORT  usAdj,
					PUPDINF pMainInfo, PUPDINF pLinkInfo, VOID *pvBuffer, USHORT *pfsStatus)
{
#ifdef	__DEL_2172	/* ### DEL 2172 Rel1.0 (01/17/00) */
	BOOL		fLocked, fUnableUpdate;
	UCHAR		aucKey[LONGOF(aucPluNumber, RECPLU)];
	UCHAR		*pucNext, *pucKey, *pfcFlags;
	UCHAR		aucEnough[2*sizeof(RECPLU) + sizeof(RECMIX) /* + sizeof(RECPPI)*/];/* ### MOD 2172 Rel1.0 */
	USHORT		usStat, fsMainInfo, fsLinkInfo, fsNotWhy, fsAction;
	LONG		lQty, lAmt, lDisc;
	ULONG		ulMul;
	PRECPLU		pPlu, pLink;
	PRECMIX		pMix;
	/*PRECPPI	pPpi; *//* ### DEL 2172 Rel1.0 */

	/* --- initialize --- */

	*pfsStatus = 0;								/* initialize resp. status	*/
	fsMainInfo = pMainInfo->fsControl;			/* update control flags		*/

	/* --- parse link rec's info --- */

	if (pLinkInfo) {							/* given a link info. ?		*/
		fsLinkInfo  = pLinkInfo->fsControl & ~(CNTRL_IGNORE_SV_INT);
		fsLinkInfo |= fsMainInfo           &   CNTRL_IGNORE_SV_INT;
	} else {									/* else, not given ...		*/
		fsLinkInfo  = pMainInfo->fsControl;
		fsLinkInfo &= ~(CNTRL_WITH_DISCOUNT | CNTRL_UNCOND_UPDATE);
	}

	/* --- is valid file ID given ? --- */

	if (usFile != FILE_PLU) {					/* other than PLU file ?	*/
		return (SPLU_INVALID_DATA);					/* invalid data !		*/
	}

	/* --- is it a locked rec. ? --- */

	fLocked   = IsLockedRecord(FILE_PLU, pNumber, usAdj);
	fsAction  = FILE_READABLE;					/* file must be readable	*/
	fsAction |= (fsMainInfo & CNTRL_EXEC_UPDATE) ? FILE_WRITABLE : 0;

	/* --- cancel my job if locked rec. --- */

	if ((fsMainInfo & CNTRL_EXEC_UPDATE) && fLocked) {
		return (SPLU_REC_LOCKED);
	}

	/* --- is locked file ? --- */

	if (IsLockedFile(FILE_PLU, fsAction)) {		/* is PLU file seized ?		*/
		return (SPLU_FILE_LOCKED);					/* file lock violation	*/
	}

	/* --- read the main record --- */

	if (usStat = MnpReadRecord(FILE_PLU,		/* read the record			*/
							   HMNP_REC_MAIN,		/* remember its position*/
							   (UCHAR *)pNumber,	/* ptr. to key			*/
							   usAdj,				/* adjective number		*/
							   aucEnough)) {		/* buffer ptr.			*/
		return (usStat);
	}

	/* --- read a related records from here --- */

	pPlu     = (PRECPLU)aucEnough;				/* PLU rec. is top			*/
	pfcFlags = pPlu->aucStatus;					/* base ptr. of status flag	*/
	pucNext  = (UCHAR *)(pPlu + 1);				/* next rec. buffer ptr.	*/
	pLink    = (PRECPLU)NULL;					/* assume not having LINK	*/
	pMix     = (PRECMIX)NULL;					/* assume not having Mix	*/
	/*pPpi     = (PRECPPI)NULL;					/* assume not having PPI	*//* ### DEL 2172 Rel1.0 */
	fsNotWhy = 0;								/* init. INT status			*/

	/* --- initialize key buffer --- */

	memset(aucKey, '\x00', sizeof(aucKey));		/* initialize key buffer	*/

	/* --- is linking with LINK Record ? --- */
	/*if (memcmp((VOID *)(&pPlu->usLinkNumber), aucKey, LONGOF(usLinkNumber, RECPLU)))*/
	if (pPlu->usLinkNumber == 0) { /* ### MOD 2172 Rel1.0 */

		/* --- make complete key data --- */

		pucKey = aucKey
				+ LONGOF(aucPluNumber, RECPLU)
				- LONGOF(usLinkNumber, RECPLU);	/* ### MOD 2172 rel1.0 */
		memcpy(pucKey, (VOID *)&(pPlu->usLinkNumber), LONGOF(usLinkNumber, RECPLU)); /* ### MOD 2172 Rel1.0 */

		/* --- is it a locked rec. ? --- */

		fLocked = IsLockedRecord(FILE_PLU, (PITEMNO)aucKey, 0);

		/* --- cancel my job if locked rec. --- */

		if ((fsMainInfo & CNTRL_EXEC_UPDATE) && fLocked) {
			return (SPLU_REC_LOCKED);
		}

		/* --- read the linked record --- */

		usStat = MnpReadRecord(FILE_PLU, HMNP_REC_SUB, aucKey, 0, pucNext);

		/* --- how's finished ? --- */

		switch (usStat) {						/* how's it going ?			*/
		case 0:									/* completed ?				*/
			pLink = ((PRECPLU)pucNext)++;			/* remember data ptr.	*/
			break;
		case SPLU_ADJ_VIOLATION:				/* adjective rule error ?	*/
		case SPLU_REC_NOT_FOUND:				/* record not found ?		*/
		case SPLU_FILE_NOT_FOUND:				/* file not found ?			*/
			usStat    = 0;							/* adjust output status	*/
			fsNotWhy |= MISSING_LINK;				/* the reason why		*/
			break;
		default:								/* other errors ...			*/
			return (usStat);						/* critical error !		*/
		}
	}

	/* --- is linking with Mix-Match Record ? --- */

	if (pPlu->uchMixMatchTare && (! DEFFLG(pfcFlags, PLUST_SCALE_ITEM))) {

		/* --- is Mix-Match file locked ? --- */

		if (IsLockedFile(FILE_MIX_MATCH, FILE_READABLE)) {
			return (SPLU_FILE_LOCKED);
		}

		/* --- read the mix-match record --- */

		pucKey = &(pPlu->uchMixMatchTare);
		usStat = MnpReadRecord(FILE_MIX_MATCH, 0, pucKey, 0, pucNext);

		/* --- how's finished ? --- */

		switch (usStat) {						/* how's it going ?			*/
		case 0:									/* completed ?				*/
			pMix = ((PRECMIX)pucNext)++;			/* remember data ptr.	*/
			break;
		case SPLU_REC_NOT_FOUND:				/* record not found ?		*/
		case SPLU_FILE_NOT_FOUND:				/* file not found ?			*/
			usStat      = 0;						/* adjust output status	*/
			*pfsStatus |= MISSING_MIX_MATCH;		/* however not found it,*/
			break;									/* update is available.	*/
		default:								/* other errors ...			*/
			return (usStat);						/* critical error !		*/
		}
	}

	/* --- is linking with PPI Record ? --- */
#ifdef _DEL_2172_R10 /* ### DEL 2172 Rel1.0 */
	if (pPlu->uchLinkPPI) {						/* linking with PPI Rec. ?	*/

		/* --- is PPI file locked ? --- */

		if (IsLockedFile(FILE_PPI, FILE_READABLE)) {
			return (SPLU_FILE_LOCKED);
		}

		/* --- read the main record --- */

		usStat = MnpReadRecord(FILE_PPI, 0, &(pPlu->uchLinkPPI), 0, pucNext);

		/* --- how's finished ? --- */

		switch (usStat) {						/* how's it going ?			*/
		case 0:									/* completed ?				*/
			pPpi = ((PRECPPI)pucNext)++;			/* remember data ptr.	*/
			break;
		case SPLU_REC_NOT_FOUND:				/* record not found ?		*/
		case SPLU_FILE_NOT_FOUND:				/* file not found ?			*/
			usStat    = 0;							/* adjust output status	*/
			fsNotWhy |= MISSING_PPI;				/* the reason why		*/
			break;
		default:								/* other errors ...			*/
			return (usStat);						/* critical error !		*/
		}
	}
#endif /* _DEL_2172_R10 */
	/* --- need to respond record ? --- */

	if ((fsMainInfo & CNTRL_EXEC_READ) && pvBuffer) {
		ReturnUpdateRecord(pvBuffer, pPlu, pLink, pMix/*, pPpi*/);/* ### MOD 2172 Rel1.0 */
		*pfsStatus |= pLink ? RETURN_LINK      : 0;
		*pfsStatus |= pMix  ? RETURN_MIX_MATCH : 0;
		/**pfsStatus |= pPpi  ? RETURN_PPI       : 0;*//* ### DEL 2172 Rel1.0 */
	}

	/* --- respond record status --- */

	*pfsStatus |= fsNotWhy;						/* respond record status	*/

	/* --- is required to update ? --- */

	if (! (fsMainInfo & CNTRL_EXEC_UPDATE)) {	/* update a record ?		*/
		return (usStat);							/* break my job here	*/
	}

	/* --- initialize before updating --- */

	fUnableUpdate = FALSE;						/* assume able to update	*/

	/* --- check the linking record --- */

	if (pLink && (! (fsLinkInfo & CNTRL_UNCOND_UPDATE))) {

		/* --- does the link record have valid values ? --- */

		pucKey    = pLink->aucStatus;			/* base ptr. of status flag	*/
		fsNotWhy |= (pLink->uchPriceMulWeight > 1)   ? INT_WRONG_LINK : 0;
		fsNotWhy |= DEFFLG(pucKey, PLUST_SCALE_ITEM) ? INT_WRONG_LINK : 0;

		/* --- tell the reason why --- */

		*pfsStatus |= fsNotWhy;					/* respond record status	*/
	}

	/* --- is valid link record ? --- */

	fUnableUpdate = (fsNotWhy & MISSING_LINK)   ? TRUE : fUnableUpdate;
	fUnableUpdate = (fsNotWhy & INT_WRONG_LINK) ? TRUE : fUnableUpdate;

	/* --- check against update rule for main key --- */

	if (! (fsMainInfo & CNTRL_UNCOND_UPDATE)) {	/* if not unconditionally	*/

		/* --- having a mix-match / PPI rec. ? --- */
		/* fsNotWhy |= (pMix || pPpi) ? INT_MISC_REC : 0;*/
		fsNotWhy |= ((pMix != NULL) ? INT_MISC_REC : 0); /* ### MOD 2172 Rel1.0 (DELETED PPI) */

		/* --- is valid price multiple ? --- */

		ulMul     = (ULONG)(pPlu->uchPriceMulWeight);	/* get price multi.	*/
		ulMul     = (ulMul ? ulMul : 1L) * PLU_BASE_UNIT;	/* adjust 0 to 1*/
		fsNotWhy |= (labs(pMainInfo->lSalesQty) % ulMul) ? INT_MISC_REC : 0L;

		/* --- is this a scalable / visual-verify item ? --- */

		fsNotWhy |= DEFFLG(pfcFlags, PLUST_SCALE_ITEM) ? INT_SCALE_REC : 0;
		fsNotWhy |= DEFFLG(pfcFlags, PLUST_VV_ITEM)    ? INT_VV_REC    : 0;

		/* --- ignore supervisor-intervention ? --- */

		if (! (fsMainInfo & CNTRL_IGNORE_SV_INT)) {

			/* --- is this a rec. to require SV-Int ? --- */

			fsNotWhy |= DEFFLG(pfcFlags, PLUST_SV_INTERVENT) ? INT_SV_REC : 0;

			/* --- how about link record's ? --- */

			if (pLink) {
				fsNotWhy |= DEFFLG(pLink->aucStatus, PLUST_SV_INTERVENT)
															 ? INT_SV_REC : 0;
			}
		}

		/* --- respond check status --- */

		*pfsStatus   |= fsNotWhy;				/* respond record status	*/
		fUnableUpdate = fsNotWhy ? TRUE : fUnableUpdate;
	}

	/* --- can be updated ? --- */

	if (fUnableUpdate) {						/* if cannot update ...		*/
		return (SPLU_NOT_UPDATED);					/* break my job here	*/
	}

	/* --- compute main record's total --- */

	ulMul = pPlu->uchPriceMulWeight ? (ULONG)(pPlu->uchPriceMulWeight) : 1L;
	lQty  = pMainInfo->lSalesQty;				/* no. of sales quantities	*/

    /* bux fix for calculation overflow, 05/22/97 */

    if ((labs(lQty) % PLU_BASE_UNIT)) {     /* scalable sale case */

	    lAmt  = pPlu->ulUnitPrice * labs(lQty) / (ulMul * PLU_BASE_UNIT);

    } else {                                /* normal sales case */

	    lAmt  = pPlu->ulUnitPrice * (labs(lQty) / (ulMul * PLU_BASE_UNIT));
    }

    lAmt  = (lQty < 0)                         ? (-lAmt)              : lAmt;
	lAmt  = (fsMainInfo & CNTRL_UNCOND_UPDATE) ? pMainInfo->lSalesAmt : lAmt;
	lDisc = (fsMainInfo & CNTRL_UNCOND_UPDATE) ? pMainInfo->lDiscAmt  : 0L;

	/* --- are these negative values ? --- */

	lQty  = (fsMainInfo & CNTRL_MINUS_UPDATE) ? (-lQty)  : lQty;
	lAmt  = (fsMainInfo & CNTRL_MINUS_UPDATE) ? (-lAmt)  : lAmt;
	lDisc =	(fsMainInfo & CNTRL_MINUS_UPDATE) ? (-lDisc) : lDisc;

	/* --- adjust by "Coupon Item" flag --- */

	if (! (fsMainInfo & CNTRL_UNCOND_UPDATE)) {	/* if not unconditionally	*/
		lAmt  = DEFFLG(pPlu->aucStatus, PLUST_COUPON_ITEM) ? (-lAmt)  : lAmt;
		lDisc = DEFFLG(pPlu->aucStatus, PLUST_COUPON_ITEM) ? (-lDisc) : lDisc;
	}

	/* --- update to main record --- */

	/* ### DEL 2172 Rel1.0 pPlu->lItemCounts += lQty;*/
	/* ### DEL 2172 Rel1.0 pPlu->lSalesTotal += lAmt;*/
	/* ### DEL 2172 Rel1.0 pPlu->lDiscTotal  += lDisc;*/
	/* ### DEL 2172 Rel1.0 pPlu->lSalesTotal += (fsMainInfo & CNTRL_WITH_DISCOUNT) ? lAmt : 0L;*/

	/* --- update a main PLU record --- */

	usStat = MnpModifyRecord(HMNP_REC_MAIN, pPlu);

	/* --- required to update a linked rec. ? --- */

	if (! pLink) {								/* update a linked record ?	*/
		return (usStat);							/* no linked rec.		*/
	}

	/* --- compute linked record's total --- */

	ulMul = pLink->uchPriceMulWeight ? (ULONG)(pLink->uchPriceMulWeight) : 1L;
	lQty  = pLinkInfo ? pLinkInfo->lSalesQty : pMainInfo->lSalesQty;

    /* bux fix for calculation overflow, 05/22/97 */

    if ((labs(lQty) % PLU_BASE_UNIT)) {     /* scalable sale case */

	    lAmt  = pLink->ulUnitPrice * labs(lQty) / (ulMul * PLU_BASE_UNIT);

    } else {                                /* normal sales case */

	    lAmt  = pLink->ulUnitPrice * (labs(lQty) / (ulMul * PLU_BASE_UNIT));
    }

    lAmt  = (lQty < 0)                         ? (-lAmt)              : lAmt;
	lAmt  = (fsLinkInfo & CNTRL_UNCOND_UPDATE) ? pLinkInfo->lSalesAmt : lAmt;
	lDisc = (fsLinkInfo & CNTRL_UNCOND_UPDATE) ? pLinkInfo->lDiscAmt  : 0L;

	/* --- are these negative values ? --- */

	lQty  = (fsLinkInfo & CNTRL_MINUS_UPDATE) ? (-lQty)  : lQty;
	lAmt  = (fsLinkInfo & CNTRL_MINUS_UPDATE) ? (-lAmt)  : lAmt;
	lDisc =	(fsLinkInfo & CNTRL_MINUS_UPDATE) ? (-lDisc) : lDisc;

	/* --- adjust by "Coupon Item" flag --- */

	if (((  pLinkInfo) && (! (fsLinkInfo & CNTRL_UNCOND_UPDATE))) ||
		((! pLinkInfo) && (! (fsMainInfo & CNTRL_UNCOND_UPDATE)))) {
		lAmt  = DEFFLG(pLink->aucStatus, PLUST_COUPON_ITEM) ? (-lAmt)  : lAmt;
		lDisc = DEFFLG(pLink->aucStatus, PLUST_COUPON_ITEM) ? (-lDisc) : lDisc;
	}

	/* --- update to link record --- */

	/* ### DEL 2172 Rel1.0  pLink->lItemCounts += lQty;*/
	/* ### DEL 2172 Rel1.0  pLink->lSalesTotal += lAmt;*/
	/* ### DEL 2172 Rel1.0  pLink->lDiscTotal  += lDisc;*/
	/* ### DEL 2172 Rel1.0  pLink->lSalesTotal += (fsLinkInfo & CNTRL_WITH_DISCOUNT) ? lAmt : 0L;*/

	/* --- update it --- */

	usStat = MnpModifyRecord(HMNP_REC_SUB, pLink);

	/* --- exit ... --- */

	return (usStat);
#endif	/* ### __DEL_2172 */
	return	0;
}

/**
;========================================================================
;
;   function : Inform PLU Module Status
;
;   format : USHORT		ExeInformStatus(usHandle, pPlu_Status);
;
;   input  : USHORT		usHandle;	    - handle value
;			 PLU_STATUS	*pPlu_Status;	- buffer ptr. to respond status
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	ExeInformStatus(USHORT  usHandle,
						PLU_STATUS  *pPlu_Status)
{
	PUSRBLK		pUser;

	/* --- is valid handle ? --- */

	if ((! usHandle) || (usHandle > ARRAYS(aUserBlock, USRBLK))) {
		return (SPLU_INVALID_HANDLE);
	}

	/* --- evaluate user block table --- */

	pUser = aUserBlock + usHandle - 1;			    /* user management block ptr*/

	/* --- is reserved ? --- */

	if (! (pUser->fchControl & USRBLK_IN_USE)) {	/* is not used area ?	*/
		return (SPLU_INVALID_HANDLE);
	}

	/* --- set return PLU status --- */

	pPlu_Status->fchControl = pUser->fchControl;    /* control flags			*/
	pPlu_Status->uchFunc    = pUser->uchFunc;  	    /* function number			*/
	pPlu_Status->usFile     = pUser->usFile;		/* file ID accessing		*/
	pPlu_Status->fsOption   = pUser->fsOption;		/* function optional data	*/

    return(SPLU_COMPLETED);
}

/**
;========================================================================
;
;   function : Set PLU Index File ID, R2.0
;
;   format : USHORT		ExeSetIndexFileId(usFile);
;
;	input	 USHORT		usFile; 		- parent file id
;
;   output : USHORt     usIndexFile     - index file id
;
;	notes  :
;
;========================================================================
**/


USHORT ExeSetIndexFileId(USHORT usFile)
{
    USHORT  i;

    switch (ADJUSTFILE(usFile)) {
    case FILE_PLU:
        /* --- read mirror file id --- */

        for (i=0; i<MAX_USERS; i++) {

            if (aMirrorIdx[i].usPluFile == usFile) {

                return (aMirrorIdx[i].usIndexFile);
            }
        }
        /* --- assume as regular file --- */

        return (FILE_PLU_INDEX);

    case FILE_PLU_INDEX:
        return (usFile);

    default:
        return (0);
    }
}


/*==========================================================================*\
;+																			+
;+				M I S C .    S U B - R O U T I N E s						+
;+																			+
\*==========================================================================*/

/**
;========================================================================
;
;   function : Query no. of critical users
;
;   format : USHORT		QueryCritUsers(usFunc);
;
;   input  : USHORT		usFunc;			- function on critical mode
;
;   output : USHORT		usUsers;		- no. of users
;
;	notes  : If usFunc equals to 0, examine no. of users on all functions.
;
;========================================================================
**/

USHORT	QueryCritUsers(USHORT usFunc)
{
	USHORT		i, usUsers;
	PUSRBLK		pUser;

	/* --- initialize --- */

	pUser   = aUserBlock;						/* top of user block		*/
	usUsers = 0;								/* assume no users			*/

	/* --- examine all of the table --- */

	for (i = 0; i < ARRAYS(aUserBlock, USRBLK); i++, pUser++) {

		/* --- is this table used ? --- */

		if (! (pUser->fchControl & USRBLK_IN_USE)) {
			continue;
		}

		/* --- is same function doing --- */

		if ((! usFunc) || (pUser->uchFunc == (UCHAR)usFunc)) {
			usUsers++;
		}
	}

	/* --- exit ... --- */

	return (usUsers);
}

/**
;========================================================================
;
;   function : Is locked record
;
;   format : BOOL		IsLockedRecord(usFile, pItem, usAdj);
;
;   input  : USHORT		usFile;			- file ID
;			 PITEMNO	pItem;			- ptr. to item number
;			 USHORT		usAdj;			- adjective number of PLU
;
;   output : BOOL		fLocked;		- locked or not
;
;	notes  : If pItem equals to NULL, examine whether the file has locked
;			records.
;
;========================================================================
**/

BOOL	IsLockedRecord(USHORT usFile, PITEMNO pItem, USHORT usAdj)
{
	USHORT		i;
	PUSRBLK		pUser;
	PKEYINF		pTable;

	/* --- initialize --- */

	pUser = aUserBlock;							/* top of user block		*/

	/* --- query its file info. --- */

	if (MnpQInfoTable(usFile, &pTable)) {		/* get info. table ptr.		*/
		return (FALSE);
	}

	/* --- examine all of the table --- */

	for (i = 0; i < ARRAYS(aUserBlock, USRBLK); i++, pUser++) {

		/* --- is this table used ? --- */

		if (! (pUser->fchControl & USRBLK_IN_USE)) {
			continue;
		}

		/* --- is this table killed ? --- */

		if (pUser->fchControl & USRBLK_KILLED) {
			continue;
		}

		/* --- is any locked record ? --- */

		if (! (pUser->fchControl & USRBLK_REC_LOCKED)) {
			continue;
		}

		/* --- is same file ? --- */

		if (pUser->usFile != usFile) {
			continue;
		}

		/* --- examine loked rec. in a file ? --- */

		if (! pItem) {							/* examine a file status ?	*/
			return (TRUE);							/* any rec. in the file	*/
		}

		/* --- is same item number ? --- */

		if (memcmp(&(pUser->itemNumber), pItem, pTable->usKeyLen)) {
			continue;
		}

		/* --- is PLU file ? --- */

		switch (usFile) {						/* what kind of file ?		*/
		case FILE_PLU:							/* PLU record ?				*/
        case FILE_PLU_INDEX:                    /* PLU index record ?       */
			if (pUser->usAdjective == usAdj) {		/* is same adjective # ?*/
				return (TRUE);							/* locked record !	*/
			}
			break;
		default:								/* other records ...		*/
			return (TRUE);							/* locked record !		*/
		}
	}

	/* --- exit ... --- */

	return (FALSE);
}

/**
;========================================================================
;
;   function : Is locked file
;
;   format : BOOL		IsLockedFile(usFile, fsAction);
;
;   input  : USHORT		usFile;			- file ID
;			 USHORT		fsAction;		- action to access
;
;   output : BOOL		fLocked;		- locked or not
;
;	notes  : The fsAction is combination of FILE_READABLE, FILE_WRITABLE.
;
;========================================================================
**/

BOOL	IsLockedFile(USHORT usFile, USHORT fsAction)
{
	BOOL		fLocked;
	UCHAR		fcProhibit;
	USHORT		i;
	PUSRBLK		pUser;

	/* --- initialize --- */

	fLocked     = FALSE;						/* assume no locked			*/
	pUser       = aUserBlock;					/* top of user block		*/
	fcProhibit  = (UCHAR)((fsAction & FILE_READABLE) ? USRBLK_PROH_READ  : 0);
	fcProhibit |= (UCHAR)((fsAction & FILE_WRITABLE) ? USRBLK_PROH_WRITE : 0);

	/* --- examine all of the table --- */

	for (i = 0; i < ARRAYS(aUserBlock, USRBLK); i++, pUser++) {

		/* --- is this table used ? --- */

		if (! (pUser->fchControl & USRBLK_IN_USE)) {
			continue;
		}

		/* --- is this table killed ? --- */

		if (pUser->fchControl & USRBLK_KILLED) {
			continue;
		}

		/* --- is same file ? --- */

		if (pUser->usFile != usFile) {
			continue;
		}

		/* --- is any locked ? --- */

		if (pUser->fchControl & fcProhibit) {
			fLocked = TRUE;
			break;
		}
	}

	/* --- exit ... --- */

	return (fLocked);
}

/**
;========================================================================
;
;   function : Respond records related to update
;
;   format : USHORT		ReturnUpdateRecord(pvBuffer, pPlu, pLink, pMix, pPpi);
;
;   input  : VOID		*pvBuffer;		- buffer ptr. to respond
;			 PRECPLU	pPlu;			- ptr. to PLU record
;			 PRECPLU	pLink;			- ptr. to LINK record
;			 PRECMIX	pMix;			- ptr. to mix-match record
;			 PRECPPI	pPpi;			- ptr. to PPI record (### DEL 2172 Rel1.0)
;
;   output : USHORT		usBytes;		- no. of bytes to output
;
;========================================================================
**/

USHORT	ReturnUpdateRecord(VOID *pvBuffer, PRECPLU pPlu, PRECPLU pLink, PRECMIX pMix)
{
	UCHAR	*pucTarget;

	/* --- initialize --- */

	pucTarget = (UCHAR *)pvBuffer;

	/* --- respond PLU record, R2.0 --- */

	memcpy(pucTarget, pPlu, PLU_BEFORE_STATUS_LEN);
	pucTarget += PLU_BEFORE_STATUS_LEN;
	/*memcpy(pucTarget, (VOID *)&pPlu->uchBonusStatus, sizeof(UCHAR));*/		/* ### MOD 2172 R1.0 */
	memcpy(pucTarget, (VOID *)&pPlu->uchItemType, sizeof(UCHAR));		/* ### MOD 2172 R1.0 */
	pucTarget += sizeof(UCHAR);
	memcpy(pucTarget, (VOID *)&pPlu->uchMixMatchTare, PLU_AFTER_STATUS_LEN);/* ### MOD 2172 R1.0 */
	pucTarget += PLU_AFTER_STATUS_LEN;
	/* ### DEL 2172 Rel1.0 memcpy(pucTarget, (VOID *)&pPlu->usFqsValue, sizeof(USHORT));
	pucTarget += sizeof(USHORT); */

/*****
	memcpy(pucTarget, pPlu, sizeof(SCNPLU));
	pucTarget += sizeof(SCNPLU);
*****/
	/* --- respond link record --- */

	if (pLink) {
		memcpy(pucTarget, pLink, sizeof(SCNPLU));
		pucTarget += sizeof(SCNPLU);
	}

	/* --- respond mix-match record --- */

	if (pMix) {
		memcpy(pucTarget, pMix, sizeof(RECMIX));
		pucTarget += sizeof(RECMIX);
	}

	/* --- respond PPI record --- */
#ifdef _DEL_2172_R10 /* ### DEL 2172 Rel1.0 */
	if (pPpi) {
		memcpy(pucTarget, pPpi, sizeof(RECPPI));
		pucTarget += sizeof(RECPPI);
	}
#endif /* _DEL_2172_R10 */
	/* --- exit ... --- */

	return ((USHORT)(pucTarget - (UCHAR *)pvBuffer));
}

/**
;========================================================================
;
;   function : Is valid conditional expression ?
;
;   format : BOOL		IsValidCondition(pucCond, usBytes, pDictionary);
;
;   input  : UCHAR		*pucCond;		- ptr. to conditional expression
;			 USHORT		usBytes;		- bytes of conditional expression
;			 PRECDIC	pDictionary;	- ptr. to record dictionary
;
;   output : BOOL		fValid;			- valid or invalid
;
;========================================================================
**/

BOOL	IsValidCondition(WCHAR *pucCond, USHORT usBytes, PRECDIC pDictionary)
{
	BOOL		fEndOfData;
	WCHAR		*pucTrace, uchByte;
	SHORT		sRest;
	PRECDIC		pTable;

	/* --- is any condition given ? --- */

	if (! usBytes) {							/* no condition given ?		*/
		return (TRUE);								/* this is good !		*/
	}

	/* --- initialize --- */

	fEndOfData = FALSE;							/* not yet end of data		*/
	pucTrace   = pucCond;						/* top of data ptr.			*/
	sRest      = (SHORT)usBytes;				/* data length				*/

	/* --- examine whole data --- */

	while (sRest > 0) {							/* check whole expression	*/

		/* --- get field ID data --- */

		uchByte = *pucTrace++;					/* field ID data			*/
		sRest--;								/* adjust data length		*/

		/* --- is valid field ID ? --- */

		for (pTable = pDictionary; pTable->uchField != uchByte; pTable++) {

			/* --- come to an end of table ? --- */

			if (pTable->uchField == (UCHAR)TBLEND) {
				return (FALSE);
			}
		}

		/* --- adjust length by compare value data --- */

		pucTrace += pTable->usLength;			/* adjust data ptr.			*/
		sRest    -= pTable->usLength;			/* adjust remained bytes	*/

		/* --- get function ID --- */

		uchByte = *pucTrace++;					/* function ID data			*/
		sRest--;								/* adjust data length		*/

		/* --- is valid compare value ? --- */

		switch (uchByte & COMP_MASK) {			/* what compare value ?		*/
		case COMP_EQUAL:						/* equal ?					*/
		case COMP_NOT_EQUAL:					/* not equal ?				*/
		case COMP_LESS_THAN:					/* less than ?				*/
		case COMP_GREATER_THAN:					/* greater than ?			*/
		case COMP_LESS_EQUAL:					/* less or equal ?			*/
		case COMP_GREATER_EQUAL:				/* greater or equal ?		*/
		case COMP_BIT_TRUE:						/* bit true ?				*/
		case COMP_BIT_FALSE:					/* bit false ?				*/
			break;									/* these are valid !	*/
		default:								/* others ...				*/
			return (FALSE);							/* invalid compare value*/
		}

		/* --- is valid logic data ? --- */

		switch (uchByte & LOGIC_MASK) {			/* what logic data ?		*/
		case LOGIC_AND:							/* AND ?					*/
		case LOGIC_OR:							/* OR ?						*/
			fEndOfData = FALSE;						/* not yet end of data	*/
			break;									/* valid logic data		*/
		case LOGIC_END:							/* end of expression ?		*/
			fEndOfData = TRUE;						/* end of logic data	*/
			break;									/* valid logic data		*/
		default:								/* others ...				*/
			return (FALSE);							/* invalid logic data !	*/
		}
	}

	/* --- exit ... --- */

	return (((! sRest) && fEndOfData) ? TRUE : FALSE);
}

/**
;========================================================================
;
;   function : Get a batch number of maintenance record
;
;   format : USHORT		GetBatchNumber(usFile, pucRecord);
;
;   input  : USHORT		usFile;			- file ID
;			 UCHAR		*pucRecord;		- ptr. to record data
;
;   output : USHORT		usBatch;		- batch number
;
;========================================================================
**/

USHORT	GetBatchNumber(USHORT usFile, WCHAR *pucRecord)
{
	/* --- is PLU file ? --- */

	if (usFile != FILE_MAINT_PLU) {				/* if out of PLU file ...	*/
		return (0);									/* no batch no.			*/
	}

	/* --- get a batch number & respond it --- */

	return (((PMNTPLU)pucRecord)->usMntBatch);
}

/**
;========================================================================
;
;   function : Is active record or not
;
;   format : BOOL		IsActiveRecord(usFile, pucRecord);
;
;   input  : USHORT		usFile;			- file ID
;			 UCHAR		*pucRecord;		- ptr. to record data
;
;   output : BOOL		fActive;		- ative record or not
;
;========================================================================
**/

BOOL	IsActiveRecord(USHORT usFile, WCHAR *pucRecord)
{
	BOOL		fActive = TRUE; /* ### MOD 2172 Rel1.0 */
	/* ### DEL 2172 Rel1.0 PRECPLU		pPlu; */

	/* --- is PLU file ? --- */

	if (usFile != FILE_PLU) {					/* if out of PLU file ...	*/
		return (FALSE);								/* always inactive		*/
	}

	/* --- abbreciate record ptr. --- */

	/* ### DEL 2172 Rel1.0 pPlu = (PRECPLU)pucRecord;*/ /* top of data ptr.			*/

	/* --- is active record ? --- */
	/* ### DEL 2172 Rel1.0 
	if ((pPlu->lItemCounts) || (pPlu->lSalesTotal) || (pPlu->lDiscTotal)) {
		fActive = TRUE;
	} else {
		fActive = FALSE;
	}*/

	/* --- exit ... --- */

	return (fActive);
}

/**
;========================================================================
;
;   function : Is satisfied record with conditional expression ?
;
;   format : BOOL		IsSatisfiedRecord(pucRec, pucCond, pDictionary);
;
;   input  : UCHAR		*pucRecord;		- ptr. to record data
;			 UCHAR		*pucCond;		- ptr. to conditional expression
;			 PRECDIC	pDictionary;	- ptr. to record dictionary
;
;   output : BOOL		fSatisfied;		- satisfied or not
;
;	notes  : The conditinal expression must be valid formula.
;
;========================================================================
**/


BOOL	IsSatisfiedRecord(WCHAR *pucRecord, WCHAR *pucCond, PRECDIC pDictionary)
{
	BOOL		fBreak, fSatisfied;
	WCHAR		*pucTrace, *pucValue, *pucData, uchByte;
	PRECDIC		pTable;

	/* --- initialize --- */

	fBreak   = FALSE;							/* not yet end of data		*/
	pucTrace = pucCond;							/* top of data ptr.			*/

	/* --- examine whole data --- */

	while (! fBreak) {							/* check whole expression	*/

		/* --- get field ID data --- */

		uchByte = *pucTrace++;					/* field ID data			*/

		/* --- is valid field ID ? --- */

		for (pTable = pDictionary; pTable->uchField != uchByte; pTable++) {

			/* --- come to an end of table ? --- */

			if (pTable->uchField == (UCHAR)TBLEND) {
				return (FALSE);
			}
		}

		/* --- adjust length by compare value data --- */

		pucValue  = pucTrace;						/* get compare value	*/
		pucData   = pucRecord + pTable->usOffset;	/* get record data ptr.	*/
		pucTrace += pTable->usLength;				/* adjust data ptr.		*/

		/* --- get function ID --- */

		uchByte = *pucTrace++;					/* function ID data			*/

		/* --- is satisfied with compare value ? --- */

		fSatisfied = IsSatisfiedData(pucData,	/* compare with value data	*/
									 pucValue,			/* compare value	*/
									 pTable->uchType,	/* data type		*/
									 pTable->usLength,	/* data length		*/
									 (UCHAR)(uchByte & COMP_MASK));

		/* --- is valid logic data ? --- */

		switch (uchByte & LOGIC_MASK) {			/* what logic data ?		*/
		case LOGIC_AND:							/* AND ?					*/
			fBreak = fSatisfied ? FALSE : TRUE;		/* continue to check ?	*/
			break;
		case LOGIC_OR:							/* OR ?						*/
			fBreak = fSatisfied ? TRUE : FALSE;		/* continue to check ?	*/
			break;
		case LOGIC_END:							/* end of expression ?		*/
			fBreak = TRUE;							/* end of logic data	*/
			break;
		default:								/* others ...				*/
			fBreak     = TRUE;						/* break my job			*/
			fSatisfied = FALSE;						/* not satisfied		*/
			break;
		}
	}

	/* --- exit ... --- */

	return (fSatisfied);
}

/**
;========================================================================
;
;   function : Is satisfied data with condition ?
;
;   format : BOOL		IsSatisfiedData(pucRec, pucCond, pDictionary);
;
;   input  : UCHAR		*pucRecord;		- ptr. to record data
;			 UCHAR		*pucCond;		- ptr. to conditional expression
;			 PRECDIC	pDictionary;	- ptr. to record dictionary
;
;   output : BOOL		fSatisfied;		- satisfied or not
;
;	notes  : The conditinal expression must be valid formula.
;
;========================================================================
**/

BOOL	IsSatisfiedData(VOID *pvData, VOID *pvValue, UCHAR uchType, USHORT usLength, UCHAR uchComp)
{
	BOOL	fSatisfied;
	UCHAR	*pucByte, *pucTest, uchCh;
	SHORT	sResult;
	USHORT	i;

	/* --- test for bit true or flase ? --- */

	if ((uchComp == COMP_BIT_TRUE) || (uchComp == COMP_BIT_FALSE)) {

		/* --- initialize before testing --- */

		pucByte    = (UCHAR *)pvData;				/* source data ptr.		*/
		pucTest    = (UCHAR *)pvValue;				/* compare data ptr.	*/
		fSatisfied = TRUE;							/* assume satisfied		*/

		/* --- test whole bytes --- */

		for (i = 0; i < usLength; i++) {			/* check whole bytes	*/

			/* --- get source data byte --- */

			uchCh = *pucByte++;						/* get byte data		*/
			uchCh = (uchComp == COMP_BIT_TRUE) ? (UCHAR)(~uchCh) : uchCh;

			/* --- test it --- */

			if (uchCh & *pucTest++) {				/* is satisfied ?		*/
				fSatisfied = FALSE;						/* not satisfied	*/
				break;
			}
		}

		/* --- break its job here --- */

		return (fSatisfied);
	}

	/* --- try to compute large and small relations --- */

	switch (uchType) {							/* what king of data type ?	*/
	case TYPBCD:								/* packed BCD ?				*/
	case TYPBIN:								/* binary data ?			*/
		sResult = memcmp(pvData, pvValue, usLength);	/* compare as memory*/
		break;
	case TYPUCH:								/* UCHAR type ?				*/
		sResult = (SHORT)(*(UCHAR *)pvData  - *(UCHAR *)pvValue);
		break;
	case TYPUSH:								/* USHORT type ?			*/
		sResult = (SHORT)(*(USHORT *)pvData - *(USHORT *)pvValue);
		break;
	case TYPULG:								/* ULONG type ?				*/
		sResult = (SHORT)(*(ULONG *)pvData  - *(ULONG *)pvValue);
		break;
	case TYPLNG:								/* LONG type ?				*/
		sResult = (SHORT)(*(LONG *)pvData   - *(LONG *)pvValue);
		break;
	default:									/* others ...				*/
		return (FALSE);								/* unable to compare	*/
	}

	/* --- what kind of comparison type ? --- */

	switch (uchComp) {							/* what compare value ?		*/
	case COMP_EQUAL:							/* equal ?					*/
		fSatisfied = (sResult == 0) ? TRUE : FALSE;	/* compute it			*/
		break;
	case COMP_NOT_EQUAL:						/* not equal ?				*/
		fSatisfied = (sResult != 0) ? TRUE : FALSE;	/* compute it			*/
		break;
	case COMP_LESS_THAN:						/* less than ?				*/
		fSatisfied = (sResult  < 0) ? TRUE : FALSE;	/* compute it			*/
		break;
	case COMP_GREATER_THAN:						/* greater than ?			*/
		fSatisfied = (sResult  > 0) ? TRUE : FALSE;	/* compute it			*/
		break;
	case COMP_LESS_EQUAL:						/* less or equal ?			*/
		fSatisfied = (sResult <= 0) ? TRUE : FALSE;	/* compute it			*/
		break;
	case COMP_GREATER_EQUAL:					/* greater or equal ?		*/
		fSatisfied = (sResult >= 0) ? TRUE : FALSE;	/* compute it			*/
		break;
	default:									/* others ...				*/
		fSatisfied = FALSE;							/* unable to compare	*/
		break;
	}

	/* --- exit ... --- */

	return (fSatisfied);
}

/**
;========================================================================
;
;   function : Set PLU Index Record from PLU Record, R2.0
;
;   format : VOID		SetIndexRecord(pRecIdx, pRecPlu);
;
;	input	 RECPLU		*pRecPlu;		- ptr. to PLU record data
;
;   output : RECIDX		*pRecIdx;		- ptr. to index record data
;
;	notes  :
;
;========================================================================
**/
VOID SetIndexRecord(RECIDX *pRecIdx, RECPLU *pRecPlu)
{
    if (pRecPlu) {

        /* pRecIdx->usDeptNumber = pRecPlu->usDeptNumber;*/ /* ### DEL 2172 Rel1.0 (01/07/00) */
        memcpy(pRecIdx->aucPluNumber, pRecPlu->aucPluNumber, sizeof(pRecPlu->aucPluNumber)); /* ### 2172 Rel1.0 */
        pRecIdx->uchAdjective = pRecPlu->uchAdjective;

    } else {

        memset(pRecIdx, 0x00, sizeof(RECIDX));
    }
}

/*==========================================================================*\
||							End of PLUAPPLI.C								||
\*==========================================================================*/
