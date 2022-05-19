/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1997           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                Release 2.0              ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Electronic Journal module, Header file for user
*   Category           : EJ module
*   Program Name       : EJ.H
*  ------------------------------------------------------------------------
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.
*   Memory Model       : Medium Model
*   Options            :  /c /AM /G1s /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*   Update Histories   :
*
*   Date                 Ver.Rev    :NAME         :Description
*   Jan-30-93          : 00.00.01   :T.Asawa      :Initial
*   Jun-16-93          : 00.00.01   :H.Yamaguchi  :Adds EJReadFile
*   Aug-09-95          : 03.00.00   :M.Suzuki     :Adds EJRead1Line R3.0
*   Dec-08-95          : 03.01.00   :M.Ozawa      :Change EJWrite argument to FAR
*   Mar-14-97          : 02.00.05   :hrkato       :R2.0(Migration)
*
** NCR2172 **
*
*   Oct-05-99          : 01.00.00   :M.Teraki     :initial (for 2172)
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
#define        EJFG_FULL           (UCHAR)0x80
#define        EJFG_NEAR_FULL      (UCHAR)0x40
#define        EJFG_ADJ_LINE       (UCHAR)0x20
#define        EJFG_NO_FILE        (UCHAR)0x10
#define        EJFG_PAGE_END       (UCHAR)0x08
#define        EJFG_RPAGE_END      (UCHAR)0x04
#define        EJFG_FINAL_PAGE     (UCHAR)0x02
#define        EJFG_OVERRIDE       (UCHAR)0x01
#define        EJ_THERMAL_EXIST     0
#define        EJ_THERMAL_NOT_EXIST 1
#define        EJ_COLUMN           24
#define        EJ_FULL_LIMIT       144
#define        EJ_PERFORM          0
#define        EJ_CONTINUE         0   /* Adds for EJReadResetTrans, R2.0GCA */
#define        EJ_END              -1  /* Adds for EJReadResetTrans, R2.0GCA */
#define        EJ_NO_MAKE_FILE     -10
#define        EJ_TOO_SMALL_BUFF   -11
#define        EJ_DELETED          -12
#define		   EJ_NO_READ_SIZE     -31 //RPH 11-7-3 No Read Size was set //for EJReadFileA
#define        EJ_TOP              0x0L
#define        EJ_BLOCKSIZE        0x200L
#define        EJ_1TR_SIZE         0x1000L
#define        EJ_FILE_FULL        -100
#define        EJ_FILE_NOT_EXIST   -101
#define        EJ_FILE_NEAR_FULL   -102
#define        EJ_DATA_TOO_LARGE   -103
#define        EJ_NOTHING_DATA     -104
#define        EJ_PROHIBIT         -105
#define        EJ_EJLOCK_FAIL      -107   // indicates EJLock(EJLOCKFG_ISP) function failed
#define        EJ_OVERRIDE_STOP    -109   // indicates (FHeader.fchEJFlag & EJFG_OVERRIDE) set so Prohibit
#define        EJ_EOF              -120   /* Adds for EJReadFile */

#define        MINOR_EJ_NEW           1   // indicates that EJ Read or Write is a new transaction restarting sequence number

//Updated so that a whole transaction can fit into one block for reporting.  
//The problem was that when a 3rd party ISV tried to recall transactions in 
//the EJ buffer, they would only get about 47 lines of information, whereas
//our application can allow a user to ring up much more than that. JHHJ
//#define        EJ_PRT_BUFFLEN      2300    /* Buffer Max. Length for Print */
#define        EJ_PRT_BUFFLEN      11000    /* Buffer Max. Length for Print */

#define        EJ_2X20_PRTOFFLINE    3

#define        EJLOCKFG_TERM (UCHAR)0x01 /* for fchEJLock, R2.0 GCA */
#define        EJLOCKFG_ISP (UCHAR)0x02 /* for fchEJLock, R2.0 GCA */

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


// following two structs are used with the Electronic Journal file to provide
// the over all file management, EJF_HEADER, and the entry by entry housekeeping, EJT_HEADER.
//
// WARNING:  Be very careful in making any changes to the layout of these structs.
//           The source code that deals with Electronic Journal entries is scattered
//           throughout both the GenPOS terminal source as well as the PCIF DLL source.
//           Most remote applications using PCIF to pull the Electronic Journal have
//           dependencies on the EJT_HEADER struct and its layout so making changes
//           you will risk:
//              - breaking GenPOS in strange and unexpected ways
//              - breaking remote applications such as CWS in strange and unexpected ways

// Electronic Journal file contains a series of variable length records.  To traverse the file
// you need to start with the first entry pointed to by ulBeginPoint and then use the length of
// that entry (member usCVLI of the EJT_HEADER struct) to compute the offset to the next entry.
// The physical size of the file is fixed by provisioning through the Setup however the amount
// of data will vary.  The last entry near the physical end of file is always complete, there is
// no wrapping of an entry so ulEndofPoint indicates the end of the working physical data size.
// So there is the physical max size of the file, ulEJFSize, and the end of the working physical
// data, ulEndofPoint.  ulEndofPoint should always be smaller than ulEJFSize.  If the file is
// empty being just created then ulBeginPoint equals ulPrevPoint equals ulNextPoint equals ulEndofPoint.
// The first entry added will go to ulBeginPoint, ulPrevPoint will point to the this first entry,
// and ulNextPoint and ulEndofPoint will point to ulBeginPoint + usCVLI.
// If there is insufficient space between ulEndofPoint and ulEJFSize, then ulEndofPoint will remain
// showing where the working physical data ends, and ulNextPoint will wrap around to the beginning
// of the Electronic Journal file.
typedef  struct {
    UCHAR    fchEJFlag;      /* Status Flag */
    ULONG    ulEJFSize;      /* Physical file size at Creation */
    USHORT   usTtlLine;      /* Number of Total Lines of EJ entries */
    USHORT   usMonth;        /* Start Month */
    USHORT   usDay;          /* Start Day */
    USHORT   usHour;         /* Start Hour */
    USHORT   usMin;          /* Start minutue */
    ULONG    ulBeginPoint;   /* Start point of first transaction in the list of entries */
    ULONG    ulPrevPoint;    /* Start point of last transaction in the list of entries */
    ULONG    ulNextPoint;    /* Start point of next transaction or next location to put an entry */
    ULONG    ulEndofPoint;   /* Ending point of end of the last transaction physically stored in the file */
}EJF_HEADER;

#define EJT_HEADER_SIGNATURE  0xF234
typedef  struct {
    USHORT  usPVLI;        /* Previous Length */
    USHORT  usCVLI;        /* Current Length */
    USHORT  usTermNo;      /* Terminal number */
    USHORT  usConsecutive; /* Consecutive number */
    USHORT  usSeqNo;
    USHORT  usMode;        /* Mode */
    USHORT  usOpeNo;       /* Operator number */
    USHORT  fsTranFlag;    /* Transaction Flags */
    USHORT  usGCFNo;       /* G.C.F. number, Not Used   R2.0 */
    USHORT  usDate;        /* Start Date */
    USHORT  usTime;        /* Start Time */
    USHORT  usEjSignature; /* Reserve now used for signature */
}EJT_HEADER;

/* typedef  struct {
    EJT_HEADER   TrHeader;
    UCHAR        azText[1950];
}EJ_DATA; */

typedef  struct {
    UCHAR       uchMajorClass;
    UCHAR       uchMinorClass;
    EJT_HEADER  THeader;
} EJ_WRITE;

typedef  struct {
    UCHAR   uchMajorClass;
    UCHAR   uchMinorClass;
    UCHAR   fchFlag;
    USHORT  usSize;
    USHORT  usLine;
    USHORT  usPage;
    ULONG   ulLeftReport;
    ULONG   ulLLastReport;
    ULONG   ulRightReport;
    ULONG   ulRLastReport;
    SHORT   sLeftOffset;
    SHORT   sLLastOffset;
    SHORT   sRightOffset;
    SHORT   sRLastOffset;
} EJ_READ;

typedef struct {
    UCHAR   uchMajorClass;
    UCHAR   uchMinorClass;
    USHORT  usSize;
} EJ_CTL;

// Calculate an Electronic Journal print buffer size by allowing the user to specify the number of lines
// This calculates a UCHAR buffer size to contain the number of lines specified along with the EJ header
// and an end of string character.
#define EJ_WRITE_CALC_BUF_SIZE(nLines) (sizeof(EJT_HEADER)+(nLines)*EJ_COLUMN*sizeof(TCHAR))
#define EJ_WRITE_CALC_BUF_OFFSET(nLineNo) (sizeof(EJT_HEADER)+(nLineNo-1)*EJ_COLUMN*sizeof(TCHAR))

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
/*
===========================================================================
    EXETERN
===========================================================================
*/

/*
------------------------------------------------
    Define state control (usIspEJState),    V3.3
------------------------------------------------
*/
#define ISP_ST_EJ                   0x00001   /* Wait E/J Request from PC, V3.3 */

extern USHORT        usIspEJState;            /* E/J read/reset state, V3.3 */

/*
==================================================
   PROTOTYPE
==================================================
*/
SHORT	EJConsistencyCheck (VOID);
SHORT   EJCreat(USHORT usNoOfBlock, UCHAR fchFlag);
SHORT   EJCheckC(USHORT usNoOfBlock, UCHAR fchFlag);
SHORT   EJCheckD(USHORT usNoOfBlock, UCHAR fchFlag);
SHORT   EJWrite(EJT_HEADER FAR *pTHeader, USHORT usSize,
            UCHAR FAR *pBuff, UCHAR uchType, UCHAR uchOption);  /* Chg R3.1 */
SHORT   EJRead(EJ_READ *pEJRead, UCHAR *pBuff, UCHAR uchType);
SHORT   EJReverse(EJ_READ *pEJRead, WCHAR *pBuff, UCHAR uchType);
SHORT   EJClear(VOID);
UCHAR   EJGetFlag(VOID);
USHORT  EJConvertError(SHORT sError);
USHORT  EJGetStatus(VOID);
SHORT   EJRead1Line(EJ_READ *pEJRead, UCHAR *pBuff, UCHAR uchType); /* Add R3.0 */
SHORT   EJ_Write(EJT_HEADER FAR *pTHeader, USHORT usSize,
            UCHAR FAR *pBuff, UCHAR uchType, UCHAR uchOption);  /* Chg R3.1 */
SHORT    EJWriteDelayBalance(UCHAR uchStatus);
/*-----------------------------------------------------
    Adds Enhance function to poll from PC
-----------------------------------------------------*/
SHORT   EJReadFile(ULONG ulOffset,  VOID  *pData,
                   ULONG ulDataLen, ULONG *pulRetLen);
SHORT   EJInit(VOID);
SHORT   EJNoOfTrans(VOID);                                      /* R2.0GCA */
SHORT   EJReadResetTransOver(ULONG *pulOffset,
                   VOID *pData, ULONG ulDataLen, ULONG *pulRetLen);
SHORT   EJReadResetTransStart(ULONG *pulOffset,
                   VOID *pData, ULONG ulDataLen, ULONG *pulRetLen, USHORT *pusNumTrans);
SHORT   EJReadResetTrans(ULONG *pulOffset, VOID *pData,
                ULONG ulDataLen, ULONG *pulRetLen, USHORT *pusNumTrans);
SHORT   EJReadResetTransEnd(USHORT usNumTrans);

#if 1
VOID  EJ_ReqSem_Debug (char *aszFilePath, int nLineNo);
VOID  EJ_RelSem_Debug (char *aszFilePath, int nLineNo);
#define EJ_ReqSem() EJ_ReqSem_Debug (__FILE__, __LINE__)
#define EJ_RelSem() EJ_RelSem_Debug (__FILE__, __LINE__)
VOID  EJ_ReqSemIsp_Debug (char *aszFilePath, int nLineNo);
#define EJ_ReqSemIsp() EJ_ReqSemIsp_Debug (__FILE__, __LINE__)
VOID  EJ_RelSemIsp_Debug (char *aszFilePath, int nLineNo);
#define EJ_RelSemIsp() EJ_RelSemIsp_Debug (__FILE__, __LINE__)
#else
VOID    EJ_ReqSem(VOID);
VOID    EJ_RelSem(VOID);
VOID    EJ_ReqSemIsp (VOID);
VOID    EJ_RelSemIsp (VOID);
#endif

/*
==================================================
   PROTOTYPE  Internal Function
==================================================
*/
VOID    EJ_W_Close(EJF_HEADER *pFHeader, USHORT usWSize, UCHAR FAR *pBuff); /* Chg R3.1 */
USHORT  EJ_Set_End(EJF_HEADER *pFHeader, EJ_READ *pEJRead);
USHORT  EJ_Set_Left(EJF_HEADER *pFHeader, EJ_READ *pEJRead, UCHAR *pBuff);
USHORT  EJ_Set_Right(EJF_HEADER *pFHeader, EJ_READ *pEJRead, UCHAR *pBuff);
VOID    EJ_Set_1Tr(USHORT usLine, ULONG ulOffset, UCHAR *pBuff);
SHORT   EJ_DelBlk(EJF_HEADER *pFHeader, SHORT sSize);
VOID    EJ_WriteFile(ULONG ulOffset, VOID FAR *pData, USHORT usSize, SHORT hsHandle);   /* Chg R3.1 */
USHORT  EJ_ReadFile(ULONG ulOffset, VOID *pData, USHORT usSize);
#if 0
SHORT   EJ_ReadFileA_Debug(ULONG ulOffset, VOID *pData, ULONG ulSize, ULONG *pulActualBytesRead, char *pPath, int iLineNo);
#define EJ_ReadFileA(ulOffset,pData,ulSize,pulActualBytesRead)  EJ_ReadFileA_Debug(ulOffset,pData,ulSize,pulActualBytesRead, __FILE__, __LINE__);
#else
SHORT   EJ_ReadFileA(ULONG ulOffset, VOID *pData, ULONG ulSize, ULONG *pulActualBytesRead);
#endif
UCHAR   EJ_ReadPrtCtl(UCHAR    uchAddress);
VOID    EJ_SetDesc(VOID);
VOID    EJ_ResetDesc(VOID);
SHORT   EJ_OpenFile(VOID);
VOID    EJ_CloseFile(SHORT sHandle);
VOID    EJ_CloseAndRelSem(VOID);
VOID    EJ_CloseAndRelSemIsp(VOID);
USHORT  EJReadTrans(EJF_HEADER *pFHeader, ULONG *pulOffset, VOID *pData, ULONG ulDataLen, ULONG *pulRetLen);
SHORT   EJCheckSpace(EJF_HEADER *pFHeader);
#if 0
SHORT   EJLock_Debug (UCHAR uchLock, char *aszFilePath, int nLineNo);
SHORT   EJUnLock_Debug (UCHAR uchLock, char *aszFilePath, int nLineNo);

#define EJLock(uchLock)  EJLock_Debug (uchLock,__FILE__,__LINE__)
#define EJUnLock(uchLock)  EJUnLock_Debug (uchLock,__FILE__,__LINE__)
#else
SHORT   EJLock( UCHAR uchLock );
SHORT   EJUnLock( UCHAR uchLock );
#endif
USHORT EJDelayBalanceUpdateFile(VOID);

/* --- End of Header --- */
