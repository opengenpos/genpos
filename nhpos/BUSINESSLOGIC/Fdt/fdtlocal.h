/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Flexible Drive Through Thread
*   Category           : Flexible Drive Through Thread, NCR2170 US HOSPITALITY MODEL
*   Program Name       : fdtlocal.h
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*                      :
*  ------------------------------------------------------------------------
*   Update Histories   
*   Date     :Ver.Rev.:  NAME    :Description
*   Apr-17-95:        :hkato      : Initial.
*
*** NCR2172 ***
*
*   Oct-04-99:01.00.00: M.Teraki : Initial
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

/*----- FDT Thread Definition -----*/
#define     FDT_STACK                   ((3072*2)/2)    /* Stack Size, Saratoga */

/*----- For FDTLocal.fsFDTStatus;         FDT Status -----*/


/*----- For Read GCF Return Status -----*/
#define     FDT_STATUS_GET_FILE1       0x0001           /* Need Display of File #1 */
#define     FDT_STATUS_GET_FILE2       0x0002           /* Need Display of File #2 */


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

typedef struct {
	USHORT           fsFDTStatus;						/* FDT Status */
	UCHAR            fchNBMessage;						/* Message from Notice Borad */
	GCNUM            auchOrder[2];						/* Order# for Display */
	GCNUM            uchOrderNo1;						/* Order No for File #1 */
	DCURRENCY        lMI1;								/* MI #1 */
	PifFileHandle    husTranHandle1;					/* Transaction File Handle#1 */
	USHORT           usTranSize1;						/* Transaction File Size#1 */
	USHORT           usTranVli1;						/* Transaction Size#1 */
	ULONG            fsGCFStatus1;						/* GCF Status #1 */
	GCNUM            uchOrderNo2;						/* Order No for File #2 */
	DCURRENCY        lMI2;								/* MI #2 */
	PifFileHandle    husTranHandle2;					/* Transaction File Handle#2 */
	USHORT           usTranSize2;						/* Transaction File Size#2 */
	USHORT			 usTranVli2;						/* Transaction Size#2 */
	ULONG            fsGCFStatus2;						/* GCF Status #2 */
	GCNUM            auchWaitOrder[FDT_QUEUE_SIZE];     /* Waiting Order Number GCNUM */
	GCNUM            auchStorePayQueue[FDT_QUEUE_SIZE]; /* Store/Payment Transaction(Order#) Queue GCNUM */
	GCNUM            auchTransQueue[FDT_QUEUE_SIZE];    /* Payment Transaction(Order#) Queue GCNUM */
	GCNUM            uchOwnOrder;						/* Paid Order Number (Without Amount) */
	GCNUM            uchGCFOrder;						/* Paid Order Number (With Amount) */
	GCNUM            uchPrevPayTrans;					/* Previous Payment Order Number */
	GCNUM            uchAddonOrder;						/* Addon Transaction Order Number */
} FDTLOCAL;

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
/*
-------------------------------------------------
   PROTOTYPE Local Functions
-------------------------------------------------
*/
SHORT  FDT_memmove (GCNUM *gcDest, GCNUM *gcSource, int nItems);

SHORT   FDTRefresh(USHORT usQueue, USHORT usType);
SHORT   FDTRefreshOrder(VOID);
SHORT   FDTRefreshDelivery(GCNUM usQueue, USHORT usType);

SHORT   FDTExpandFile(SHORT hsFileHandle, USHORT usInquirySize);
SHORT   FDTReadFile(ULONG ulOffset, VOID *pData, ULONG ulSize, SHORT hsFileHandle, ULONG *pulActualBytesRead);
SHORT   FDTWriteFile(ULONG ulOffset, VOID *pData, ULONG ulSize, SHORT hsFileHandle);
USHORT  FDTCalStoSize(USHORT usNoOfItem);

VOID    FDTDecideQueuePara(USHORT *pusQueue, USHORT *pusType);
VOID    FDTRemoveWaitOrder(GCNUM uchOrder);
VOID    FDTRemoveStoreQueue(GCNUM uchOrder);
VOID    FDTRemoveTransQueue(GCNUM uchOrder);
VOID    FDTInsertWaitOrder(GCNUM uchOrder);
VOID    FDTInsertStoreQueue(GCNUM uchOrder);
VOID    FDTInsertTransQueue(GCNUM uchOrder);
VOID    FDTClear(SHORT sType);
SHORT   FDTReadTransFromGCF(GCNUM uchOrder2, GCNUM uchOrder3, SHORT *psReturn);
SHORT   FDTReadTransFromGCFStorePay(GCNUM uchOrder2, GCNUM uchOrder3, SHORT *psReturn);
SHORT   FDTReadTransFromGCFDeliv(GCNUM uchOrder2, GCNUM uchOrder3, SHORT *psReturn);
SHORT   FDTGetTrans(GCNUM uchOrderNo, SHORT sType);
VOID    FDTDispTotal(USHORT usScroll, SHORT sType);

/*
-------------------------------------
   MEMORY
-------------------------------------
*/
extern  USHORT  husFDTSyncSem;                      /* FDT Event Semaphore */
extern  USHORT  husFDTFileSem;                      /* FDT File Semaphore */
extern  FDTLOCAL    FDTLocal;

extern  TCHAR   FARCONST  aszFDTFile1[];            /* File Name #1 */
extern  TCHAR   FARCONST  aszFDTFile2[];            /* File Name #2 */

/*======= End of Define ============*/
