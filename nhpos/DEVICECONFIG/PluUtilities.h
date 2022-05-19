
#if !defined(PLUUTILITIES_H_INCLUDED)
#define PLUUTILITIES_H_INCLUDED

/*
/////////////////////////////////////////////////////////////////////////////
//  Definition Default Parameters
/////////////////////////////////////////////////////////////////////////////
*/
#define PIFROOTKEY	TEXT("SOFTWARE\\NCR\\Saratoga\\Pif")
#define FILEKEY		TEXT("File")
#define PATH        TEXT("Path")        /* saratoga path */
#define TEMPPATH    TEXT("TempPath")    /* saratoga path for temporary DRAM file */
#define DATABASE    TEXT("DataBase")    /* file directory */
#define TOTALDATA   TEXT("TotalData")   /* total file directory */
#define PIFLOG      TEXT("Log")         /* piflog file directory */

#define	s_DataBase	    0
#define s_TempBase	    1
#define s_SavedTotals	2

#define MAX_REGKEY_LENGTH		100
#define MAX_DIRECTORY_LENGTH	100

#define	INIRST_FILES_TOTALCLEAR1		(TEXT("SPOOLSHR"))
#define	INIRST_FILES_TOTALCLEAR2		(TEXT("TOTAL*"))

#define INIRST_FILES_ALLRESET1			(TEXT("PARA*"))
#define INIRST_FILES_ALLRESET2			(TEXT("NHPOS*"))     // memory resident database snapshot UNINIRAM
#define INIRST_FILES_ALLRESET3			(TEXT("DFPR_*"))     // finger print database
#define INIRST_FILES_ALLNOTLAY1			(TEXT("PARAM$*"))
#define INIRST_FILES_ALLNOTLAY2			(TEXT("PARAMC*"))
#define INIRST_FILES_ALLNOTLAY3			(TEXT("PARAMD*"))
#define INIRST_FILES_ALLNOTLAY4			(TEXT("PARAME*"))
#define INIRST_FILES_ALLNOTLAY5			(TEXT("PARAMID*"))    // do not delete PARAMINI file per US Customs.
#define INIRST_FILES_ALLNOTLAY6			(TEXT("PARAMM*"))
#define INIRST_FILES_ALLNOTLAY7			(TEXT("PARAMO*"))
#define INIRST_FILES_ALLNOTLAY8			(TEXT("PARAMP*"))
#define	INIRST_FILES_ALLNOTLAY9			(TEXT("PARAMR*"))

#define INIRST_FILES_GCF_FILES          (TEXT("TOTALGCF"))
#define INIRST_FILES_TTLUPD_FILE		(TEXT("TOTALUPD"))
#define INIRST_FILES_EEPTSTORE_FILE		(TEXT("EEPTST*"))
#define INIRST_FILES_SAVEDTOTAL_FILE	(TEXT("Saved*"))

#define INIRST_ALL_REST			0
#define INIRST_ALL_NOTLAY		1
#define INIRST_TOTALS_REST		2
#define INIRST_CLEAR_GCF		3
#define INIRST_CLEAR_PLU		4
#define INIRST_CLEAR_TTLUPD		5		//delete the total update file
#define INIRST_CLEAR_EEPTSTO	6		//delete the EEPTSTO files, EEPTSTO*

#define	RESET_ERR		0
#define RESET_SUCCESS	1

/*
/////////////////////////////////////////////////////////////////////////////
// Function Definitions
/////////////////////////////////////////////////////////////////////////////
*/
BOOL OpenDir(const SHORT nType);
BOOL GetPathString(LPCTSTR lpPathKey,LPCTSTR lpDirKey,LPTSTR lpszPath);
void MakePathString(LPTSTR szPath,LPCTSTR szDir);
BOOL DelFiles(LPCTSTR lpcFname=NULL);
BOOL OverWriteDelFiles(LPCTSTR lpcFname=NULL);
void CloseDir(void);

#endif
