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
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.              
*   Memory Model       : Medium Model
*   Options            :  /c /AM /G1s /Os /Za /Zp /W4
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

/*
----------------------------------------
   ERROR CODE FOR DEFINE
----------------------------------------
*/

#define  GCF_SUCCESS         0    /* NORMAL END */

#define  GCF_FILE_NOT_FOUND -1    /* FILE NOT FOUND */
#define  GCF_FULL           -2    /* FILE FULL */
#define  GCF_NOT_IN         -3    /* NOT FINED GCN */
#define  GCF_EXIST          -4    /* ALREADY EXIST GCN */
#define  GCF_ALL_LOCKED     -5    /* LOCK FOR DURING RESET */
#define  GCF_NOT_CREATE     -6    /* NOT CREAT GCF FILE */
#define  GCF_FATAL          -7    /* BROKEN INDEX TABLE */

#define  GCF_NOT_LOCKED     -10   /* NOT DURING READING GCN */
#define  GCF_LOCKED         -11   /* DURING READING GCN BY OTHER TERMINAL */
#define  GCF_DUR_LOCKED     -12   /* ALREADY  LOCK */
#define  GCF_DATA_ERROR     -14   /* USER INPUT DATA ERROR */
#define  GCF_BACKUP_ERROR   -15   /* FAIL BACKUP COPY */
#define  GCF_NOT_IN_PAID    -16   /* NOT PAID */
#define  GCF_CHANGE_QUEUE   -17   /* Change Delivery Queue */

#define  GCF_NOT_MASTER     -20   /* Prohibit operation */

#define  GCF_END_FILE       -31   /* END OF FILE  */


#define  GCF_MAX_GCF_NUMBER      5000  /* Maximum drive size *///JHHJ Database Change 800->5000 3-15-04

/*
---------------------------------------
    Guest Check File Type 
---------------------------------------
*/

/* User interface of "uchType" */

#define  GCF_PRE_BUF_TYPE        0  /* Precheck with buffering type */
#define  GCF_PRE_UNBUF_TYPE      1  /* Precheck with unbuffering type */
#define  GCF_POSTCHECK_TYPE      2  /* Post check */
#define  GCF_DRIVE_THROUGH       3  /* Store / Recall system */    

/* User interface  of "usType",     R3.0 */

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

/* User interface  of "usType",     R3.0 */
#define  GCF_REORDER             0  /* Not Change Payment Queue */
#define  GCF_SUPER               1  /* Using Super Mode  */
#define  GCF_PAYMENT_QUEUE1      2  /* Remove GCF# from Payment Queue #1 */
#define  GCF_PAYMENT_QUEUE2      3  /* Remove GCF# from Payment Queue #2 */
#define  GCF_DELIVERY_QUEUE1     4  /* Remove GCF# from Delivery Queue #1 */
#define  GCF_DELIVERY_QUEUE2     5  /* Remove GCF# from Delivery Queue #2 */

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

#if POSSIBLE_DEAD_CODE

/*
----------------------------------------
    Assign Number of Take Que
----------------------------------------
*/
#define  GCF_DATA_BLOCK_SIZE   1800L    /* DATA BLOCK SIZE 1536,   Saratoga */
#define  GCF_DATABLOCK         1800     /* DATA BLOCK SIZE 1536,   Saratoga */

#define GCF_TAKEQUE_TYPE01       1      /* Get Number of Que When Assign.  Pre-Guest Buffering and UnBuffering type */
/* #define GCF_TAKEQUE_TYPE23       ((CONSOLIMAXSIZE + GCF_DATA_BLOCK_SIZE - 1) / GCF_DATA_BLOCK_SIZE) */
                                        /* Get Number of Que When Assign.  Post-Check and Store Recall system */
#define GCF_TYPICAL_QUE_TYPE01   1      /* Creat Number of Que in order 1 Guest Check When File Create */
#define GCF_TYPICAL_QUE_TYPE23   4      /* Creat Number of Que in order 1 Guest Check When File Create */

/*
---------------------------------------
    OTHER DEFINE 
---------------------------------------
*/
#define  GCF_COUNTER_OF_SEM      1    /* semaphore counter */
#define  GCF_MAX_DRIVE_NUMBER    1999   /* Define Maximum Drive Thorugh Number *///JHHJ Database Change 99->999 3-15-04
#define  GCF_FILE_HEAD_POSITION  0L   /* File rewind */
#define  GCF_SET_ALL_LOCK        0xff /* ALL LOCK FLAG SET DATA */
#define  GCF_READ_FLAG_MASK      0x01 /* Control flag mask bit */
#define  GCF_DRIVE_THROUGH_MASK  0x80 /* drive through type mask bit */
#define  GCF_PAYMENT_TRAN_MASK   0x02 /* paid transaction mask bit  Add R3.0 */

/*--------------------------------------------
    USER I/F G.C.F FUNCTION (CLIENT & SERVER)
--------------------------------------------*/
SHORT   GusManAssignNo(USHORT usGCNumber);
SHORT   GusStoreFile(USHORT usType,
                     USHORT usGCNumber,
                     UCHAR  *puchSndBuffer,
                     USHORT usSize);  
SHORT   GusReadLock(USHORT usGCNumber, 
                    UCHAR  *puchRcvBuffer, 
                    USHORT usSize,
                    USHORT usType);
SHORT   GusDelete(USHORT usGCNumber);
SHORT   GusGetAutoNo(USHORT *pusGCNumber);
SHORT   GusBakAutoNo(USHORT usGCNumber);     /* For backup */
SHORT   GusAllIdRead(USHORT usType,
                     USHORT *pusRcvBuffer,
                     USHORT usSize);
SHORT   GusResetReadFlag(USHORT usGCNumber, USHORT usType);
SHORT   GusIndRead(USHORT usGCNumber,
                   UCHAR  *puchRcvBuffer, 
                   USHORT usSize);
SHORT   GusReadAllLockedGCN(USHORT   *pusRcvBuffer,
                            USHORT   usSize);

SHORT   GusStoreFileFH(USHORT usType,
                       USHORT usGCNumber,
                       SHORT  hsFileHandle,
                       ULONG  ulStartPoint,
                       USHORT usSize);

SHORT   GusReadLockFH(USHORT usGCNumber, 
                      SHORT  hsFileHandle, 
                      USHORT usStartPoint, 
                      USHORT usSize,
                      USHORT usType,
					  ULONG  *pulBytesRead);

SHORT   GusIndReadFH(USHORT usGCNumber,
                     SHORT  hsFileHandle, 
                     USHORT usStartPoint, 
                     USHORT usSize,
					 ULONG	*pulBytesRead);

/*---------------------------------------
    USER I/F G.C.F FUNCTION (SERVER)
----------------------------------------*/

VOID    GusInitialize(USHORT usMode);
SHORT   GusCreateGCF (USHORT usRecordNumber,
                      UCHAR uchType);
SHORT   GusCheckAndCreateGCF (USHORT usRecordNumber,
                              UCHAR uchType);
SHORT   GusCheckAndDeleteGCF (USHORT usRecordNumber,
                              UCHAR uchType,
                              BOOL  *pfDeleted /* add R3.1 */ );
SHORT   GusAllLock(VOID);
SHORT   GusAllUnLock(VOID);
SHORT   GusAssignNo(USHORT usGCAssignNo);
SHORT   GusCheckExist(VOID);
SHORT   GusAllIdReadBW(USHORT usType,
                       USHORT usWaiterNo,
                       USHORT *pusRcvBuffer,
                       USHORT usSize);
SHORT   GusReqBackUp(VOID);
SHORT   GusResBackUp(UCHAR  *puchRcvData,
                     USHORT usRcvLen,
                     UCHAR  *puchSndData,
                     ULONG *pulSndLen);

SHORT   GusReqBackUpFH(SHORT hsFileHandle);
SHORT   GusResBackUpFH(UCHAR  *puchRcvData,
                       USHORT usRcvLen,
                       SHORT  hsFileHandle,
                       USHORT usOffset,
                       USHORT *pusSndLen);

USHORT  GusConvertError(SHORT sError);
SHORT   GusDeleteDeliveryQueue(UCHAR  uchOrderNo,           /* R3.0 */
                                USHORT usType);
SHORT   GusInformQueue(USHORT   usQueue,                    /* R3.0 */
                        USHORT  usType,
                        UCHAR   uchOrderNo,
                        UCHAR   uchForBack);
SHORT GusSearchQueue(USHORT usQueue,                        /* R3.0 */
                     UCHAR  uchMiddleNo,
                     UCHAR uchRightNo);
SHORT GusCheckAndReadFH(USHORT     usGCNumber,              /* R3.0 */
                        USHORT     usSize,
                        SHORT      hsFileHandle,
                        USHORT     usStartPoint,
                        USHORT     usFileSize,
						ULONG	   *pulBytesRead);
SHORT GusResetDeliveryQueue(VOID);                          /* R3.0 */
SHORT GusDeliveryIndRead(USHORT     usQueue,                /* R3.0 */     
                         UCHAR      *puchRcvBuffer,
                         USHORT     usSize,
                         UCHAR      *puchOrder);

/*============ End of Define ================*/
#endif