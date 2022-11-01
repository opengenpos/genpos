#ifndef	__FILEMGGP_H
#define	__FILEMGGP_H

#define	FMGP_PLU_FNAME		WIDE("GPPLUTMP")

/* --- response format of PluSenseFile_GP() for 2170GP --- */
typedef struct _MFINFO_GP {					/* file information format      */
    USHORT  usFileNo;                           /* file number              */
    ULONG   ulFileSize;                         /* file size in byte        */
    USHORT  usMaxRec;                           /* no. of max. records      */
    USHORT  usCurRec;                           /* no. of current records   */
} MFINFO_GP, *PMFINFO_GP;

/* --- parameter of PluEnterCritMode_GP() for 2170GP --- */
typedef struct _ECMCOND_GP {				/* data for report by condition */
	UCHAR	aucCond[20];						/* data of conditional exp. */
	USHORT	usLength;							/* length of cond. exp.		*/
	USHORT	fsOption;							/* option flags				*/
	USHORT	usReserve;							/* reserved (0)				*/
} ECMCOND_GP, *PECMCOND_GP;


#define	FMGP_PLU_FILE_NO	0	
#define PLU_USE_ADJ_MASK     0xf0

/* --- PLU Procs (2170GP) --- */
VOID	PluInitialize_GP(VOID);
VOID	PluFinalize_GP(VOID);
USHORT	PluEnterCritMode_GP(USHORT usFile,USHORT usFunc,USHORT *pusHandle,VOID   *pvParam);
USHORT	PluExitCritMode_GP(USHORT usHandle, USHORT usReserve);
USHORT	PluReportRecord_GP(USHORT usHandle, VOID *pvBuffer, USHORT *pusRet);
USHORT	PluSenseFile_GP(USHORT usFile, MFINFO_GP *pFileInfo);

#endif
