/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1996      ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server GUEST CHECK module, Header file for user
*   Category           : Client/Server GUEST CHECK module, NCR2170 US HOSPITALITY MODEL
*   Program Name       : CSGCS.H
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*                      :
*  ------------------------------------------------------------------------
*   Update Histories
*   Date     :Ver.Rev.:  NAME     :Description
*   May-07-92:00.00.01:M.YAMAMOTO :Initial
*   Oct-23-92:00.01.10:H.YAMAGUCHI:Change block size from 2 to 1 at pre-check system
*   Apr-21-95:03.00.00:hkato      :R3.0
*   Nov-29-95:03.01.00:T.Nakahata :R3.1 Initial (Mod. GusCheckAndDeleteGCF)
*   Feb-05-96:03.01.00:M.Suzuki   :R3.1 Mod. GCF_DATA_BLOCK_SIZE
\*=======================================================================*/
/*=======================================================================*\
:   PVCS ENTRY
:-------------------------------------------------------------------------
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

#if !defined(INCLUDE_CSGCS_H_INCLUDED)
#define INCLUDE_CSGCS_H_INCLUDED

/* Guest Check subsystem error codes
   These error codes are converted by function GusConvertError()
   into LDT_ type codes for use by function UieErrorMsg() to
   display errors to the operator.

   These codes should be synchronized with similar values used by
   the lower level STUB_ error codes as well as the Totals
   error codes defined in file csttl.h
 */
#define  GCF_SUCCESS         0    /* NORMAL END */

#define  GCF_FILE_NOT_FOUND   (-1)    /* FILE NOT FOUND */
#define  GCF_FULL             (-2)    /* FILE FULL */
#define  GCF_NOT_IN           (-3)    /* NOT FINED GCN */
#define  GCF_EXIST            (-4)    /* ALREADY EXIST GCN */
#define  GCF_ALL_LOCKED       (-5)    /* LOCK FOR DURING RESET */
#define  GCF_NOT_CREATE       (-6)    /* NOT CREAT GCF FILE */
#define  GCF_FATAL            (-7)    /* BROKEN INDEX TABLE */

#define  GCF_NOT_LOCKED       (-10)   /* NOT DURING READING GCN */
#define  GCF_LOCKED           (-11)   /* DURING READING GCN BY OTHER TERMINAL */
#define  GCF_DUR_LOCKED       (-12)   /* ALREADY  LOCK */
#define  GCF_DATA_ERROR       (-14)   /* USER INPUT DATA ERROR */
#define  GCF_BACKUP_ERROR     (-15)   /* FAIL BACKUP COPY */
#define  GCF_NOT_IN_PAID      (-16)   /* NOT PAID */
#define  GCF_CHANGE_QUEUE     (-17)   /* Change Delivery Queue */

#define  GCF_NOT_ALLOWED      (-19)
#define  GCF_NOT_MASTER       (-20)   /* Prohibit operation same as CAS_NOT_MASTER */

#define  GCF_END_FILE         (-31)   /* END OF FILE  */
#define  GCF_COMERROR         (-37)   /* Error found during request  */

// Following are used to transform Network Layer (STUB_) errors into GCF_ errors
// Notice that error code is same ast STUB_ plus STUB_RETCODE.
#define  GCF_M_DOWN_ERROR        (STUB_RETCODE+STUB_M_DOWN)
#define  GCF_BM_DOWN_ERROR       (STUB_RETCODE+STUB_BM_DOWN)
#define  GCF_BUSY_ERROR          (STUB_RETCODE+STUB_BUSY)
#define  GCF_TIME_OUT_ERROR      (STUB_RETCODE+STUB_TIME_OUT)
#define  GCF_UNMATCH_TRNO        (STUB_RETCODE+STUB_UNMATCH_TRNO)
#define  GCF_DUR_INQUIRY         (STUB_RETCODE+STUB_DUR_INQUIRY)


/*
---------------------------------------
    Guest Check File Type
---------------------------------------
*/

/* User interface of "uchType" */
//#define  GCF_PRE_BUF_TYPE        0  /* Precheck with buffering type - same as FLEX_PRECHK_BUFFER */
//#define  GCF_PRE_UNBUF_TYPE      1  /* Precheck with unbuffering type - same as FLEX_PRECHK_UNBUFFER */
//#define  GCF_POSTCHECK_TYPE      2  /* Post check - same as FLEX_POST_CHK */
//#define  GCF_DRIVE_THROUGH       3  /* Store / Recall system - same as FLEX_STORE_RECALL */

/*
	User interface  of "usType",

	The following defines are used to communicate between the totals
	and tender areas of BusinessLogic and the guest check area of
	BusinessLogic.  These defines are used to indicate whether
	Flexible Drive Through (FDT) is in operation or not and whether
	the FDT queues need to be updated which in turn will cause the
	displayed transactions in a 3 window displays to update and change.

	These operation codes are also used to indicate which of the two
	FDT queues to put the transaction onto, queue #1 or queue #2,
 */

#define  GCF_COUNTER_NOCONSOLI  100   // indicates Non-Consolidated guest check so no item index data. was TRN_NOCONSOLI

#define  GCF_COUNTER_TYPE        0  /* Counter type */
#define  GCF_DRIVE_THROUGH_STORE 1  /* Drive Through type */
#define  GCF_DRIVE_DELIVERY1     2  /* Append GCF# to Delivery/Payment Queue #1 */
#define  GCF_DRIVE_THROUGH2      3  /* Append GCF# to Payment Queue #2 */
#define  GCF_DRIVE_DELIVERY2     4  /* Append GCF# to Delivery/Payment Queue #2 */
#define  GCF_DRIVE_TH_TENDER1    5  /* Remove GCF# from /Delivery Payment Queue #1 */
#define  GCF_DRIVE_DE_TENDER1    6  /* Remove GCF# from Payment Queue #1 */
#define  GCF_DRIVE_TH_TENDER2    7  /* Remove GCF# from /Delivery Payment Queue #2 */
#define  GCF_DRIVE_DE_TENDER2    8  /* Remove GCF# from Payment Queue #2 */
#define  GCF_DRIVE_TH_DELIVERY1  9  /* Remove GCF# from Delivery Queue #1 */
#define  GCF_DRIVE_TH_DELIVERY2  10 /* Remove GCF# from Delivery Queue #2 */

#define  GCF_DRIVE_PUSH_THROUGH_STORE  11  /* Prepend GCF# to Payment Queue #1 */
#define  GCF_DRIVE_PUSH_DELIVERY1      12  /* Prepend GCF# to Delivery/Payment Queue #1 */
#define  GCF_DRIVE_PUSH_THROUGH2       13  /* Prepend GCF# to Payment Queue #2 */
#define  GCF_DRIVE_PUSH_DELIVERY2      14  /* Prepend GCF# to Delivery/Payment Queue #2 */

#define  GCF_DRIVE_THROUGH_STORE_MAX   GCF_DRIVE_PUSH_DELIVERY2   /* used in check in in GusStoreFileFH() */
#define  GCF_DRIVE_THROUGH_FIELD       0x1F  // used to mask off GCF_DRIVE_ type

/* User interface  of "usType",     R3.0 */
#define  GCF_REORDER             0  /* Not Change Payment Queue */
#define  GCF_SUPER               1  /* Using Super Mode  */
#define  GCF_PAYMENT_QUEUE1      2  /* Remove GCF# from Payment Queue #1 */
#define  GCF_PAYMENT_QUEUE2      3  /* Remove GCF# from Payment Queue #2 */
#define  GCF_DELIVERY_QUEUE1     4  /* Remove GCF# from Delivery Queue #1 */
#define  GCF_DELIVERY_QUEUE2     5  /* Remove GCF# from Delivery Queue #2 */
#define  GCF_RETURNS             6  /* fetching copy of guest check for returns */

/* User interface  of "uchForBack",     R3.0 */
#define  GCF_FORWARD             0  /* Forward */
#define  GCF_BACKWARD            1  /* Backward */

/* User interface  of "usType",     R3.0 */
#define  GCF_STORE               0  /* Store */
#define  GCF_STORE_PAYMENT       1  /* Store/Payment */
#define  GCF_PAYMENT             2  /* Payment */

/* User interface  of "usType",     R3.0 */
#define  GCF_NO_APPEND           0  /* No Append */
#define  GCF_APPEND_QUEUE1       1  /* Append GCF# to Delivery/Payment Queue #1 */
#define  GCF_APPEND_QUEUE2       2  /* Append GCF# to Delivery/Payment Queue #2 */


/*
----------------------------------------
    Assign Number of Take Que
----------------------------------------
*/
/*This block was changed due to the size of the structures now using TCHARS
before, the 1024 was just large enough to hold the regular data without going over,
the TCHARs pushed the size over the limit.*/
#define  GCF_DATA_BLOCK_SIZE   4096L    /* DATA BLOCK SIZE 4096 (4K), size same as TRN_STOSIZE_BLOCK */
#define  GCF_DATABLOCK         4096     /* DATA BLOCK SIZE 4096 (4K), size same as TRN_STOSIZE_BLOCK */

#define GCF_TAKEQUE_TYPE01       2      /* Get Number of Que When Assign.  Pre-Guest Buffering and UnBuffering type */
/* #define GCF_TAKEQUE_TYPE23       ((CONSOLIMAXSIZE + GCF_DATA_BLOCK_SIZE - 1) / GCF_DATA_BLOCK_SIZE) */
                                        /* Get Number of Que When Assign.  Post-Check and Store Recall system */
#define GCF_TYPICAL_QUE_TYPE01   2      /* Creat Number of Que in order 1 Guest Check When File Create */
#define GCF_TYPICAL_QUE_TYPE23   4      /* Creat Number of Que in order 1 Guest Check When File Create */

/*
---------------------------------------
    OTHER DEFINE
---------------------------------------
*/

#define  GCF_MAX_GCF_NUMBER      5000  /* Maximum drive size *///800 JHHJ Guestcheck Change 800->5000 3-15-04
#define  GCF_MAX_DRIVE_NUMBER    1999   /* Define Maximum Drive Thorugh Number *///99 JHHJ Guestcheck Change 99->999 3-15-04
#define	 GCF_MAX_CHK_NUMBER		 9999  // Define Maximum Check Number for Non Store/Recall Guestchecks JHHJ 3-1-05
#define  GCF_FILE_HEAD_POSITION  0L   /* File rewind */
#define  GCF_SET_ALL_LOCK        0xff /* ALL LOCK FLAG SET DATA */
#define  GCF_READ_FLAG_MASK      0x01 /* Control flag mask bit */
#define  GCF_DRIVE_THROUGH_MASK  0x80 /* drive through type mask bit */
#define  GCF_PAYMENT_TRAN_MASK   0x02 /* paid transaction mask bit  Add R3.0 */
#define  GCF_RETURNS_TRAN_MASK   0x04 /* already processed as returns transaction mask bit */

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

typedef struct {                    /* Guest Check Index entry */   
	GCNUM      usGCNO;                 /* Guest Check No */
	USHORT     fbContFlag;             /* Control Flag */
	ULONG      ulCashierID;                /* cashier ID */
	DCURRENCY  lCurBalance;
	USHORT     usTableNo;                  /* table No */
	UCHAR      uchGcfYear;
	UCHAR      uchGcfMonth;
	UCHAR      uchGcfDay;
	UCHAR      uchGcfHour;
	UCHAR      uchGcfMinute;
	UCHAR      uchGcfSecond;
}GCF_STATUS_STATE;

typedef struct {
	GCNUM   usMaxGcn;
	GCNUM   usStartGCN;
} GCF_STATUS_HEADER;

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)

#else
#pragma pack()
#endif


/*--------------------------------------------
    USER I/F G.C.F FUNCTION (CLIENT & SERVER)
--------------------------------------------*/
SHORT   GusManAssignNo(GCNUM usGCNumber);
SHORT   GusStoreFile(USHORT usType, GCNUM usGCNumber, UCHAR  *puchSndBuffer, USHORT usSize);
SHORT   GusReadLock(GCNUM usGCNumber, UCHAR  *puchRcvBuffer, USHORT usSize, USHORT usType);
SHORT   GusDelete(GCNUM usGCNumber);
SHORT   GusGetAutoNo(GCNUM *pusGCNumber);
SHORT   GusBakAutoNo(GCNUM usGCNumber);     /* For backup */
SHORT   GusAllIdRead(USHORT usType, USHORT *pusRcvBuffer, USHORT usSize);
SHORT   GusResetReadFlag(GCNUM usGCNumber, USHORT usType);
SHORT   GusIndRead(GCNUM usGCNumber, UCHAR  *puchRcvBuffer, USHORT usSize);
SHORT   GusReadAllLockedGCN(USHORT   *pusRcvBuffer, USHORT   usSize);

SHORT   GusStoreFileFH(USHORT usType, GCNUM usGCNumber, SHORT  hsFileHandle, ULONG  ulStartPoint, ULONG  ulSize);

SHORT   GusReadLockFH(GCNUM usGCNumber, SHORT  hsFileHandle, USHORT usStartPoint, USHORT usSize, USHORT usType, ULONG  *pulBytesRead);
SHORT   GusReturnsLockClear(GCNUM  usGCNumber);
SHORT   GusIndReadFH(GCNUM usGCNumber, SHORT  hsFileHandle, USHORT usStartPoint, USHORT usSize, ULONG	*pulBytesRead);

SHORT   GusReadAllGuestChecksWithStatus (GCF_STATUS_STATE *pRcvBuffer, USHORT usRcvMaxCount, GCF_STATUS_HEADER *pGcf_FileHed);
SHORT   GusAllIdReadFiltered (USHORT usType, ULONG ulWaiterNo, USHORT usTableNo, GCF_STATUS_STATE *pusRcvBuffer, USHORT usCount);

/*---------------------------------------
    USER I/F G.C.F FUNCTION (SERVER)
----------------------------------------*/

VOID    GusInitialize(USHORT usMode);
SHORT   GusCreateGCF (GCNUM usRecordNumber, UCHAR uchType);
SHORT   GusCheckAndCreateGCF (GCNUM usRecordNumber, UCHAR uchType);
SHORT   GusCheckAndDeleteGCF (GCNUM usRecordNumber, UCHAR uchType, BOOL  *pfDeleted /* add R3.1 */ );
SHORT   GusAllLock(VOID);
SHORT   GusAllUnLock(VOID);
SHORT   GusAssignNo(GCNUM usGCAssignNo);
SHORT   GusCheckExist(VOID);
SHORT   GusAllIdReadBW(USHORT usType, ULONG  ulWaiterNo, USHORT *pusRcvBuffer, USHORT usSize);
SHORT   GusAllIdReadFiltered (USHORT usType, ULONG ulWaiterNo, USHORT usTableNo, GCF_STATUS_STATE *pusRcvBuffer, USHORT usCount);
SHORT   GusSearchForBarCode (USHORT usType, UCHAR *uchUniqueIdentifier, GCF_STATUS_STATE *pusRcvBuffer, USHORT usCount);
SHORT   GusReqBackUp(VOID);
SHORT   GusResBackUp(UCHAR  *puchRcvData, USHORT usRcvLen, UCHAR  *puchSndData, ULONG *pulSndLen);

SHORT   GusReqBackUpFH(SHORT hsFileHandle);
SHORT   GusResBackUpFH(UCHAR  *puchRcvData, USHORT usRcvLen, SHORT  hsFileHandle, USHORT usOffset, USHORT *pusSndLen);

// The following #if is used to enable or disable
// functionality to issues NHPOS_ASSERT log entries when
// the function TtlConvertError () is called during NeighborhoodPOS
// execution.  This functionality provides a simple way to turn on the
// ability to have Assert logs created by calls to TtlConvertError()
// that provide the file and line number in the file from whence the
// the function was called.  This can provide additional error debugging.
#if 1
// generate a warning so this place is easy to find from a compiler warning.
//#pragma message("  ====++++====   GusConvertError_Debug() is ENABLED  \z  ====++++====")
#define GusConvertError(sError) GusConvertError_Debug(sError, __FILE__, __LINE__)
USHORT  GusConvertError_Debug(SHORT sError, char *aszFilePath, int nLineNo);               /* Convert error code  */
#else
USHORT   GusConvertError(SHORT sError);
#endif

SHORT   GusDeleteDeliveryQueue(GCNUM  uchOrderNo, USHORT usType);
SHORT   GusInformQueue(USHORT   usQueue,                    /* R3.0 */
                        USHORT  usType,
                        GCNUM   uchOrderNo,
                        UCHAR   uchForBack);
SHORT   GusRetrieveFirstQueue(USHORT usQueue, USHORT  usType, GCNUM   *uchOrderNo);
SHORT GusSearchQueue(USHORT usQueue, USHORT  uchMiddleNo, USHORT uchRightNo);
SHORT GusCheckAndReadFH(GCNUM     usGCNumber,              /* R3.0 */
                        USHORT     usSize,
                        SHORT      hsFileHandle,
                        USHORT     usStartPoint,
                        USHORT     usFileSize,
						ULONG	   *pulBytesRead);
SHORT GusResetDeliveryQueue(VOID);                          /* R3.0 */
SHORT GusDeliveryIndRead(USHORT     usQueue,                /* R3.0 */
                         UCHAR      *puchRcvBuffer,
                         USHORT     usSize,
                         GCNUM      *puchOrder);

#endif
/*============ End of Define ================*/
