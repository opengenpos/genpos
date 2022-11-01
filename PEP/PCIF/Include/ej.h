/*========================================================================*\
*   Title              : Electronic Journal module, Header file for user
*   Category           : EJ module
*   Program Name       : EJ.H
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
\*=======================================================================*/
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
#define        EJ_EOF              -120   /* Adds for EJReadFile */
#define        MINOR_EJ_NEW           1

//Updated so that a whole transaction can fit into one block for reporting.  
//The problem was that when a 3rd party ISV tried to recall transactions in 
//the EJ buffer, they would only get about 47 lines of information, whereas
//our application can allow a user to ring up much more than that. JHHJ
//#define        EJ_PRT_BUFFLEN      2300    /* Buffer Max. Length for Print */
#define        EJ_PRT_BUFFLEN      11000    /* Buffer Max. Length for Print */

#define        EJ_2X20_TAKE_TO_KTN    3

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


typedef  struct {
    UCHAR    fchEJFlag;      /* Status Flag */
    ULONG    ulEJFSize;      /* File size at Creation */
    USHORT   usTtlLine;      /* Number of Total Line */
    USHORT   usMonth;        /* Start Month */
    USHORT   usDay;          /* Start Day */
    USHORT   usHour;         /* Start Hour */
    USHORT   usMin;          /* Start minutue */
    ULONG    ulBeginPoint;   /* Start point of begin transaction */
    ULONG    ulPrevPoint;    /* Start point of last transaction */
    ULONG    ulNextPoint;    /* Start point of next transaction */
    ULONG    ulEndofPoint;   /* Start point of end of transaction */
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
extern UCHAR fchEJFlag;                             /* E.J. Status */
extern SHORT hsEJFileHandle;                        /* E.J. File Handle */

extern UCHAR FARCONST  auchNEW_FILE_WRITE[];
extern UCHAR FARCONST  auchOLD_FILE_WRITE[];
extern UCHAR FARCONST  auchOLD_FILE_READ_WRITE[];
extern UCHAR FARCONST  auchOLD_FILE_READ[];

/*
==================================================
   PROTOTYPE
==================================================
*/
SHORT   EJCreat(USHORT usNoOfBlock, UCHAR fchFlag);
SHORT   EJCheckC(USHORT usNoOfBlock, UCHAR fchFlag);
SHORT   EJCheckD(USHORT usNoOfBlock, UCHAR fchFlag);
SHORT   EJWrite(EJT_HEADER FAR *pTHeader, USHORT usSize,
            UCHAR FAR *pBuff, UCHAR uchType, UCHAR uchOption);  /* Chg R3.1 */
SHORT   EJRead(EJ_READ *pEJRead, UCHAR *pBuff, UCHAR uchType);
SHORT   EJReverse(EJ_READ *pEJRead, UCHAR *pBuff, UCHAR uchType);
SHORT   EJClear(VOID);
UCHAR   EJGetFlag(VOID);
USHORT  EJConvertError(SHORT sError);
USHORT  EJGetStatus(VOID);
SHORT   EJRead1Line(EJ_READ *pEJRead, UCHAR *pBuff, UCHAR uchType); /* Add R3.0 */

/*-----------------------------------------------------
    Adds Enhance function to poll from PC
-----------------------------------------------------*/
SHORT   EJReadFile(ULONG  ulOffset,  VOID   *pData,
                   USHORT usDataLen, USHORT *pusRetLen);
SHORT   EJInit(VOID);
SHORT   EJNoOfTrans(VOID);                                      /* R2.0GCA */
SHORT   EJReadResetTransOver(ULONG *pulOffset,
                   VOID *pData, USHORT usDataLen, USHORT *pusRetLen);
SHORT   EJReadResetTransStart(ULONG *pulOffset,
                   VOID *pData, USHORT usDataLen, USHORT *pusRetLen, USHORT *pusNumTrans);
SHORT   EJReadResetTrans(ULONG *pulOffset, VOID *pData,
                USHORT usDataLen, USHORT *pusRetLen, USHORT *pusNumTrans);
SHORT   EJReadResetTransEnd(USHORT usNumTrans);
VOID    EJ_RelSem(VOID);

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
VOID    EJ_DelBlk(EJF_HEADER *pFHeader, SHORT sSize);
VOID    EJ_WriteFile(ULONG ulOffset, VOID FAR *pData, USHORT usSize);   /* Chg R3.1 */
USHORT  EJ_ReadFile(ULONG ulOffset, VOID *pData, USHORT usSize);
SHORT   EJ_ReadFileA(ULONG ulOffset, VOID *pData, ULONG ulSize, ULONG *pulActualBytesRead);
UCHAR   EJ_ReadPrtCtl(UCHAR    uchAddress);
VOID    EJ_SetDesc(VOID);
VOID    EJ_ResetDesc(VOID);
SHORT   EJ_OpenFile(VOID);
VOID    EJ_CloseFile(VOID);
VOID    EJ_CloseAndRelSem(VOID);
VOID    EJ_ReqSem(VOID);
USHORT  EJReadTrans(ULONG *pulOffset,
            VOID *pData, ULONG ulDataLen, ULONG *pulRetLen);
SHORT   EJCheckSpace(EJF_HEADER *pFHeader);
SHORT   EJLock( UCHAR uchLock );
SHORT   EJUnLock( UCHAR uchLock );

/* --- End of Header --- */
