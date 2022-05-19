/****************************************************************\
|*                                                              *|
|*  Title:          Remote File System Header                   *|
|*  Category:       Remote File System                          *|
|*  Program Name:   RMT.H                                       *|
|*                                                              *|
|****************************************************************|  
|*                                                              *|
|*  ON PC                                                       *|
|*  Compile:        MS-C Ver.7.00 Microsoft Corp.               *|
|*  Memory Model:   Large Model                                 *|
|*      Options:        /c /Alfw /G2s /Os /Zp                   *|
|*                                                              *|
|*  ON 2170                                                     *|
|*  Compile:        MS-C Ver.6.00 Microsoft Corp.               *|
|*  Memory Model:   Mideum Model                                *|
|*      Options:        /c /AMw /G1s /Os /Zap                   *|
|*                                                              *|
|****************************************************************|
|*                                                              *|
|*      Histories                                               *|
|*                                                              *|
|*      Date               Name            Description          *|
|*      Sep/23/92      T.KOJIMA    Initial                      *|
|*                                                              *|
|*** NCR2172 **                                                 *|
|*                                                              *|
|*      Oct/05/99      M.Teraki    Initial(for Win32)           *|
|*                                 Added #pragma(...)           *|
\****************************************************************/

/*******************************\
|   Define Symbolic Constants   |
\*******************************/
#define RMT_NUM_FILETBL                 1
#define RMT_FILENAME_SIZE               STD_FILENAME_LENGTH_MAX
#define RMT_MODE_SIZE                   4
#define RMT_MAXBYTES_SIZE               32767
#define RMT_MSGDATA_SIZE                448
#define RMT_NUM_DBG_FILE				2 /*JHHJ Total number of saved debug files 10-18-05*/
#define RMT_NUM_SAV_FILE_NM				10 /* Number of sav file names to send at once. Roughly
											* PIF_LEN_UDATA/(STD_FILENAME_LENGTH_MAX*sizeof(WCHAR)) */

/*
 *  See also the defines in file piffile.c which are used by the function dfckmd()
 *  and the PifOpenFile() function. The bit usage is a bit different.
*/
#define RMT_READ                        0x0001    /* FLREAD, fl_mode bit for "read"                */
#define RMT_WRITE                       0x0002    /* FLWRITE, fl_mode bit for "write"              */
#define RMT_OLD                         0x0004    /* FLOLD, fl_mode bit for "old file"             */
#define RMT_NEW                         0x0008    /* FLNEW, fl_mode bit for "new file"             */

#define RMT_TEMP						0x0010    /* FLTMP, fl_mode bit for "temp file"            */
#define RMT_FLASH						0x0020    /* FLFLASH, fl_mode bit for "flash file"         */
#define RMT_SAVTTL						0x0040    /* FLSAVTTL, fl_mode bit for "saved total file"  */
#define RMT_ICON						0x0080    /* FLICON, fl_mode bit for "icon file"           */
#define RMT_PRINTFILES					0x0100    /* FLPRINT, fl_mode bit for "print spool file"   */
#define RMT_CONFIG						0x0200    /* FLCONFIG, fl_mode bit for "configuration folder file"   */
#define RMT_HISFOLDER					0x0400    /* FLHISFOLDER, fl_mode bit for "historical reports folder file"   */
#define RMT_IMAGESFOLDER				0x0800    /* FLIMAGESFOLDER, fl_mode bit for "suggestive selling images folder file"   */

#define RMT_OK                          0
#define RMT_ERROR                       -200
#define RMT_ERROR_INVALID_HANDLE        (RMT_ERROR - 1)
#define RMT_ERROR_OPENED_FILE           (RMT_ERROR - 2)
#define RMT_ERROR_INVALID_MODE          (RMT_ERROR - 3)
#define RMT_ERROR_INVALID_BYTES         (RMT_ERROR - 4)
#define RMT_ERROR_FILE_NOT_FOUND        (RMT_ERROR - 5)
#define RMT_ERROR_FILE_EXIST            (RMT_ERROR - 6)
#define RMT_ERROR_FILE_EOF              (RMT_ERROR - 7)
#define RMT_ERROR_FILE_DISK_FULL        (RMT_ERROR - 8)
#define RMT_ERROR_NOT_LOCKED            (RMT_ERROR - 9)
#define RMT_ERROR_COM_ERRORS            (RMT_ERROR - 10)
#define RMT_ERROR_CANNOT_RESET_TTLFILE  (RMT_ERROR - 11)
#define RMT_ERROR_END_OF_FILE_SEARCH	(RMT_ERROR - 12)

#define CLI_FCRMTFILE                   0x0800
#define CLI_FCRMTCLOSEFILE              (CLI_FCRMTFILE + 1)
#define CLI_FCRMTDELETEFILE             (CLI_FCRMTFILE + 2)
#define CLI_FCRMTOPENFILE               (CLI_FCRMTFILE + 3)
#define CLI_FCRMTREADFILE               (CLI_FCRMTFILE + 4)
#define CLI_FCRMTSEEKFILE               (CLI_FCRMTFILE + 5)
#define CLI_FCRMTWRITEFILE              (CLI_FCRMTFILE + 6)
#define CLI_FCRMTRELOADLAY              (CLI_FCRMTFILE + 7)
#define CLI_FCRMTDBGFILENM				(CLI_FCRMTFILE + 8)
#define CLI_FCRMTRELOADOEP				(CLI_FCRMTFILE + 9)
#define CLI_FCRMTSAVFILENM				(CLI_FCRMTFILE + 10)

/***********************\
|   Declare Structures  |
\***********************/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif


/* close    */
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNum;
    USHORT  usFile;
} RMTREQCLOSE;

typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNum;
    SHORT   sResCode;
    SHORT   sReturn;
} RMTRESCLOSE;

/* delete   */
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNum;
    TCHAR    aszFileName[RMT_FILENAME_SIZE];
} RMTREQDELETE;

typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNum;
    SHORT   sResCode;
    SHORT   sReturn;
} RMTRESDELETE;

/* open     */
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNum;
    TCHAR    aszFileName[RMT_FILENAME_SIZE];
    CHAR    aszMode[RMT_MODE_SIZE];
} RMTREQOPEN;

typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNum;
    SHORT   sResCode;
    SHORT   sReturn;
} RMTRESOPEN;

/* get debug file names JHHJ 10-18-05     */
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNum;
	CHAR    aszMode[RMT_MODE_SIZE];
	WCHAR	aszExtension[7];
} RMTREQNAME;

typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNum;
} RMTREQSAVNAME;

typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNum;
    SHORT   sResCode;
    SHORT   sReturn;
	WCHAR    aszFileName[RMT_NUM_DBG_FILE][RMT_FILENAME_SIZE];
} RMTRESNAME;

typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNum;
    SHORT   sResCode;
    SHORT   sReturn;
	WCHAR   aszFileName[RMT_NUM_SAV_FILE_NM][RMT_FILENAME_SIZE];
} RMTRESSAVNAME;

/* read     */
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNum;
    USHORT  usFile;
    ULONG   ulPosition;
    ULONG   usBytes;
} RMTREQREAD;

typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNum;
    SHORT   sResCode;
    LONG    lReturn;
    ULONG   ulPosition;
    UCHAR   auchData[RMT_MSGDATA_SIZE];
} RMTRESREAD;

/* seek     */
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNum;
    USHORT  usFile;
    ULONG   ulBytes;
} RMTREQSEEK;

typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNum;
    SHORT   sResCode;
    SHORT   sReturn;
    ULONG   ulBytes;
} RMTRESSEEK;

/* write    */
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNum;
    USHORT  usFile;
    ULONG   ulPosition;
    USHORT  usBytes;
    UCHAR   auchData[RMT_MSGDATA_SIZE];
} RMTREQWRITE;

typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNum;
    SHORT   sResCode;
    SHORT   sReturn;
    ULONG   ulPosition;
} RMTRESWRITE;

typedef struct {
    USHORT  usFile;
    CHAR    chOpenFile;
    ULONG   ulPosition;
} RMTFILETBL;


#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
/***********************************\
|   Declare Function Prototypes     |
\***********************************/
/* PC   */
SHORT STUBENTRY RmtCloseFile(USHORT usFile);
SHORT STUBENTRY RmtDeleteFile(CONST TCHAR FAR *paszFileName);
SHORT STUBENTRY RmtOpenFile(CONST TCHAR FAR *paszFileName, CONST CHAR FAR *paszMode);
SHORT STUBENTRY RmtReadFile(USHORT usFile, VOID FAR *pBuffer, USHORT usBytes);
SHORT STUBENTRY RmtSeekFile(USHORT usFile, ULONG ulPosition, ULONG FAR *pulActualPosition);
SHORT STUBENTRY RmtWriteFile(USHORT usFile, VOID FAR *pBuffer, USHORT usBytes);
/*
SHORT   RmtCloseFile(USHORT usFile);
SHORT   RmtDeleteFile(CONST CHAR *paszFileName);
SHORT   RmtOpenFile(CONST CHAR *paszFileName, CONST CHAR *paszMode);
SHORT   RmtReadFile(USHORT usFile, VOID *pBuffer, USHORT usBytes);
SHORT   RmtSeekFile(USHORT usFile, ULONG ulPosition, ULONG *pulActualPosition);
SHORT   RmtWriteFile(USHORT usFile, VOID *pBuffer, USHORT usBytes);
*/
SHORT   RmtErrCtl(SHORT sStubErr, SHORT sPifErr);

/* 2170 */
VOID    RmtInitFileTbl(VOID);
VOID    RmtCloseAllFile(VOID);
VOID    RmtFileServer(VOID);
BOOL    RmtCheckOpenedFile(VOID);
