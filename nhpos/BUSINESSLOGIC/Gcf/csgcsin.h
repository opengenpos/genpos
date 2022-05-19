/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server Guest Check module, Program List                        
*   Category           : Client/Server Guest Check module, NCR2170 US HOSPITALITY MODEL 
*   Program Name       : CSGCFIN.H                                            
*  ------------------------------------------------------------------------  
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.              
*   Memory Model       : Midium Model
*   Options            :  /c /AM /G1s /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*                      :
*  ------------------------------------------------------------------------
*   Update Histories   
*   Date     :Ver.Rev.:  NAME    :Description
*   May-07-92:00.00.01:M.YAMAMOTO:Initial
*   Apr-27-95:02.05.04:M.Suzuki  : R3.0
*            :        :          :
*** NCR2172 ***
*
*   Oct-04-99:01.00.00:M.Teraki  : Initial
*                                : Add #pragma pack(...)
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
------------------------------------------------
    DEFINE
------------------------------------------------
*/
/* For Gcf_ReadFile */
#define  GCF_SEEK_READ        1      /* Read data after seek */
#define  GCF_CONT_READ        2      /* Read data */
#define  GCF_SEEK_ONLY        3      /* Seek only */
#define  GCF_NO_READ_SIZE    -1      /* Read buffer size  is equal 0 byte */

/* For Gcf_Index */
#define  GCF_REGIST           1      /* Search , if not in then insert  */
#define  GCF_SEARCH_ONLY      2      /* Search only */

/* For Gcf_ReqSemOpenFileGetHeadChkLck */
#define GCF_LOCKCHECK           1    /* Check all locked condition     */
#define GCF_NOT_LOCKCHECK       2    /* Not check all locked condition */

/*  Buffer Size Define */
#define  GCF_MAXWORK_SIZE     1024   /* Max work buffer size swas 512 but now GC num is USHORT */ 

/* Flag Define */
#define  GCF_COMP_KEYSIZE_MAX   10   /* Max key length */

/* Backup control flag for backup function */
#define  GCF_BACKUP_STORE_FILE   0x8000   /* Backup request flag */      

/* Paid/Not Paid Transaction Add R3.0 */
#define  GCF_NOT_PAID           0       /* Not Paid transaction */      
#define  GCF_PAID               1       /* Paid transaction */      

/* Drive through Queue Number Add R3.0 */
#define  GCF_PAYMENT_1           0       /* Payment Queue #1 */      
#define  GCF_PAYMENT_2           1       /* Payment Queue #2 */      
#define  GCF_DELIVERY_1          2       /* Delivery Queue #1 */      
#define  GCF_DELIVERY_2          3       /* Delivery Queue #2 */      

#define  GCF_MAX_QUEUE           4       /* max queue number */      

/*
------------------------------------------------
    TYPEDEF / STRUCTURE
------------------------------------------------
*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif


typedef struct {                /* FILE HEADER */
   ULONG    ulGCFSize;          /* Number of Bytes in File */
   USHORT   usSystemtype;       /* System Type 0 ~ 4. That is setted by Flexible memory assignment */
   GCNUM   usStartGCN;         /* Start Guest Check No in Auto Genaration system use. */
   GCNUM   usMaxGCN;           /* Maximum number of GC, That is setted by Flexible memory assignment */
   GCNUM   usCurGCN;           /* Current number of GC to be assignment */
   GCNUM   usCurNoPayGCN;      /* Current number of Non Pay GC to be assignment  Add R3.0 */
   GCNUM   usCurDrive[GCF_MAX_QUEUE]; /* Current number of DriveThrough GC to be assibnment  Chg R3.0 */
   GCNUM   usCurPayGC;         /* Current number of Payment Transaction Chg R3.0 */
   USHORT   offusFreeGC;        /* Start Free Que Block No */
   ULONG    offulDrvNOFile[GCF_MAX_QUEUE];  /* Number of bytes from file head to beginning of the DriveThrough index Table Chg R3.0 */
   ULONG    offulPayTranNO;     /* Number of bytes from file head to beginning of the Payment Transaction index Table Chg R3.0 */
   ULONG    offulGCIndFile;     /* Number of bytes from file head to beginning of the Index Tabe File */
   ULONG    offulGCDataFile;    /* Number of bytes from file head to beginning of the Data Table File */
}GCF_FILEHED;

typedef struct {
		UCHAR   uchGcfYear;
		UCHAR   uchGcfMonth;
		UCHAR   uchGcfDay;
		UCHAR   uchGcfHour;
		UCHAR   uchGcfMinute;
		UCHAR   uchGcfSecond;
} GCF_INDEXDATE;

typedef struct {                    /* Guest Check Index entry */   
   GCNUM   usGCNO;                 /* Guest Check No */
   USHORT   fbContFlag;             /* Control Flag */
   USHORT   offusReserveBlockNo;    /* Reserve Block No que */
   USHORT   offusBlockNo;           /* Stored Block No que */
   GCF_INDEXDATE  GcfCreateTime;
}GCF_INDEXFILE;

typedef struct {                    /* Guest Check Drive Through entry */
   UCHAR    uchDriveNO;             /* DriveThrouth Number */
}GCF_DRIVEFILE;                 

typedef struct {                            /* First Data Block */
   USHORT   offusNextBKNO;                  /* Next Block No */
   USHORT   offusSelfBKNO;                  /* Self Block No */
   USHORT   usDataLen;                      /* Data length */
   UCHAR    auchData[(GCF_DATABLOCK - 6)];  /* Storage location for data */
}GCF_FSTDATAFILE;

typedef struct {                            /* Second Data Block */
   USHORT   offusNextBKNO;                  /* Next Block No */
   USHORT   offusSelfBKNO;                  /* Self Block No */
   UCHAR    auchData[(GCF_DATABLOCK - 4)];  /* Storage location for data */
}GCF_SNDDATAFILE;

typedef struct {                    /* First Data Block Header */
    USHORT  offusNextBKNO;          /* Next Block No */
    USHORT  offusSelfBKNO;          /* Self Block No */
    USHORT  usDataLen;
}GCF_DATAHEAD;

typedef struct {                    /* Second Data Block Header */
    USHORT  offusNextBKNO;          /* Next Block No */
    USHORT  offusSelfBKNO;          /* Self Block No */
}GCF_DATAHEAD_SEC;

typedef struct {                    /* Back up header */
    USHORT  usSeqNO;                /* Sequence number */
    USHORT  usType;                 /* Data type */
    GCNUM  usGCNumber;             /* GCN       */
}GCF_BACKUP;


typedef struct {                    /* Back up Data header */
    USHORT  usDataLen;              /* GC Data length */
    UCHAR   auchData[1];            /* GC data  */
}GCF_BAKDATA;


typedef struct {
	GCF_INDEXFILE gcf_Index;
	GCF_DATAHEAD  gcf_InUseChain;
	GCF_DATAHEAD  gcf_ReserveChain;
	USHORT        usNoBlocksInUse;
	USHORT        usNoBlocksReserve;
	USHORT        usNoFragsInUse;
	USHORT        usNoFragsReserve;
	USHORT        usWidestInUse;
	USHORT        usWidestReserve;
} GCF_TALLY_INDEX;

typedef struct {
	GCF_FILEHED   gcf_Head;
	USHORT        usGlobalTotalCount;
	USHORT        usGlobalUnPaidCount;
	USHORT        usGlobalPaidCount;
	USHORT        usGlobalBlocksCountInUse;
	USHORT        usGlobalBlocksCountFree;
	UCHAR         ucUnPaidDidNotBalance;
	UCHAR         ucPaidDidNotBalance;
	UCHAR         ucNumberOfBlocksWrong;
	UCHAR         ucBlockCountInconsistent;
} GCF_TALLY_HEAD;

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)

#else
#pragma pack()
#endif

/*
-------------------------------------------------
   PROTOTYPE
-------------------------------------------------
*/
SHORT  Gcf_memmove (GCNUM *gcDest, GCNUM *gcSource, int nItems);
USHORT Gcf_WalkGcfDataFile (GCF_FILEHED *pGcf_FileHed, GCF_TALLY_INDEX *pTallyArray, USHORT nArrayItems);
SHORT  Gcf_ReadFile(ULONG offulSeekPos, VOID *pReadBuffer, ULONG ulReadSize, UCHAR uchMode); 
SHORT  Gcf_WriteFile(ULONG offulSeekPos, VOID *pWriteBuffer, USHORT usWriteSize);
SHORT  Gcf_Index(GCF_FILEHED *pgcf_filehed, UCHAR uchMode, GCF_INDEXFILE *pGcf_Index, ULONG *pHit_Point);
SHORT  Gcf_IndexDel(GCF_FILEHED   *pGcfFileHed, GCF_INDEXFILE *pGcf_IndexFile);
SHORT  Gcf_CompIndex(GCF_INDEXFILE *usKey, GCF_INDEXFILE *pGcf_IndexFile);
VOID   Gcf_FreeQueReturn(GCF_FILEHED *pGcf_FileHed, USHORT      *pusSourceQue);
VOID   Gcf_ReserveQueReturn(GCF_FILEHED *pGcf_FileHed, GCF_INDEXFILE *pGcf_Index);
SHORT  Gcf_FreeQueTake(GCF_FILEHED *pGcf_FileHed, USHORT usNumberOfBlock, USHORT *pusFreeStartBlock);

SHORT Gcf_DriveIndexSer(USHORT        usCurNumber,  /* change R3.0 */
                        GCNUM         usGCNumber,
                        ULONG        *pHitPoint,
                        ULONG         ulPosition);

VOID  Gcf_DriveIndexDeleteStd (GCF_FILEHED *pGcf_FileHed, ULONG ulHitPoint);
VOID  Gcf_DriveIndexDeleteSpl (GCF_FILEHED *pGcf_FileHed, USHORT usPos, ULONG ulHitPoint);

SHORT Gcf_SearchIndexinFile(ULONG   offulIndexTable, 
                            USHORT  usNumberOfIndex, 
                            ULONG   *poffulHitPoint, 
                            GCF_INDEXFILE    *pKey) ; 

VOID  Gcf_InsertTableinFile(ULONG offulTableHead ,
                            USHORT usNumberOfIndex ,
                            ULONG  offulInsertPoint ,
                            VOID   *pInsertData    ,
                            USHORT usInsertDataSize );

VOID  Gcf_DeleteTableinFile(ULONG offulTableHead ,
                            USHORT usNumberOfIndex ,
                            ULONG  offulDeletePoint ,
                            USHORT usDeleteDataSize );

SHORT Gcf_DataBlockCopy(GCF_FILEHED *Gcf_FileHed, 
                        GCNUM      usStartBlockNO, 
                        UCHAR       *puchRcvBuffer, 
                        USHORT      usRcvSize, 
                        USHORT      *pusCopyBlock);

SHORT Gcf_StoreData(GCF_FILEHED     *pGcf_FileHed,
                    USHORT          usType,
                    GCNUM          usGCNumber,
                    UCHAR           *puchSndBuffer,
                    USHORT          usSize);

VOID  Gcf_CreatAllFreeQue(ULONG  offulPoint, GCNUM usRecordNumber);
SHORT   Gcf_BlockReserve(GCF_FILEHED *pGcf_FileHed, USHORT      *pusFreeStartBlock);
VOID Gcf_GetNumberofBlockTypical(USHORT usType, USHORT *puchBlockNo);
VOID Gcf_GetNumberofBlockAssign(USHORT  usType, USHORT  *pusAssignBlock);

SHORT   Gcf_StoreDataFH(GCF_FILEHED *pGcf_FileHed,  /* Change R3.0 */
                        USHORT      usStatus,
                        GCNUM      usGCNumber,
                        SHORT       hsFileHandle,
                        ULONG       ulStartPoint,
                        ULONG       ulSize,
                        USHORT      usType);

SHORT   Gcf_DataBlockCopyFH(GCF_FILEHED *pGcf_FileHed, 
                            GCNUM      usStartBlockNO, 
                            SHORT       hsFileHandle, 
                            USHORT      usStartPoint, 
                            USHORT      usRcvSize,
                            USHORT      *pusCopyBlock,
							ULONG		*pulBytesRead);
                                        
SHORT   Gcf_ReadFileFH(ULONG offulSeekPos, VOID *pReadBuffer, ULONG ulReadSize, SHORT hsFileHandle);
SHORT Gcf_WriteFileFH(ULONG  offulSeekPos, VOID *pWriteBuffer, ULONG ulWriteSize, SHORT hsFileHanlde);
USHORT Gcf_CheckPayment(ULONG  offulSeekPos, SHORT  hsFileHandle); /* Add R3.0 */
SHORT Gcf_QueDrive(GCF_FILEHED *pGcf_FileHed, GCNUM  usGCNumber, USHORT  usQueNumber);  /* Add R3.0 */
SHORT Gcf_PrependQueDrive(GCF_FILEHED *pGcf_FileHed, GCNUM  usGCNumber, USHORT  usQueNumber);       /* Add R3.0 */
SHORT Gcf_RemoveQueDrive(GCF_FILEHED *pGcf_FileHed, GCNUM  usGCNumber, USHORT  usQueNumber);  /* Add R3.0 */
SHORT Gcf_DeletePaidTran(GCF_FILEHED *pGcf_FileHed, GCNUM  usGCNumber);   /* Add R3.0 */

//PDINU
USHORT Gcf_OptimizeGcfDataFile (GCF_FILEHED *pGcf_FileHed,
							GCF_TALLY_INDEX *pTallyArray, USHORT nArrayItems, USHORT usOptimize);
USHORT Gcf_WalkGcfDataFile (GCF_FILEHED *pGcf_FileHed,
							GCF_TALLY_INDEX *pTallyArray, USHORT nArrayItems);

/*
-------------------------------------
   MEMORY
-------------------------------------
*/
extern   USHORT husGcfSem;     /* semaphore */
extern   SHORT  hsGcfFile;
extern   SHORT  fsGcfDurAllLock;


/*======= End of Define ============*/
