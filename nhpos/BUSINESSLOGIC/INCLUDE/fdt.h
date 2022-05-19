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
  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*                      :
*  ------------------------------------------------------------------------
*   Update Histories   
*   Date     :Ver.Rev.:  NAME    :Description
*   Apr-17-95:        :hkato      : Initial.
*
** NCR2172 **
*
*   Oct-05-99:01.00.00:M.Teraki   :initial (for 2172)
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
------------------------------------------------
    DEFINE
------------------------------------------------
*/
#if !defined(FDT_H_INCLUDED)

#define FDT_H_INCLUDED

#include "ecr.h"

// Flexible Drive Thru error codes.
//  WARNING: Some functions within FDT also check GCF type error
//           codes so be careful when creating new error codes for
//           Flexible Drive Thru to ensure no conflicts in meaning
//           between GCF type error codes and FDT type error codes.
#define     FDT_SUCCESS                0
#define     FDT_ERROR                 -1
#define     FDT_DISP_CHANGE           -2
#define		FDT_NO_READ_SIZE		  -3
// FDT_QUEUE_SIZE indicates the physical size of the GCNUM arrays used
// for the flexible drive thru.  FDT_QUEUE_NUM_ITEMS is one less and is
// used to indicate the max number of items.  It is one less than FDT_QUEUE_SIZE
// because we want to handle display of the last two transactions which, if
// the queue is full, would be FDT_QUE_NUM_ITEMS and FDT_QUE_NUM_ITEMS + 1
// for uchOrder2 and uchOrder3 respectively.  See code in functions
// FDTPaidOrderRecallOwn(), FDTPaidOrderRecallGCF(), and others.
//
#define		FDT_QUEUE_SIZE				31
#define		FDT_QUEUE_NUM_ITEMS			(FDT_QUEUE_SIZE - 1)

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
	GCNUM  uchOrderNo[FDT_QUEUE_SIZE];  //  sized similar to FDT_QUEUE_NUM_ITEMS
} FDTWAITORDER;

typedef struct {
	GCNUM            uchOwnOrder;						/* Paid Order Number (Without Amount) */
	GCNUM            uchGCFOrder;						/* Paid Order Number (With Amount) */
	GCNUM            uchPrevPayTrans;					/* Previous Payment Order Number */
	GCNUM            uchAddonOrder;						/* Addon Transaction Order Number */
} FDTSTATUS;

typedef struct {
	UCHAR   uchDelivery;
	UCHAR   uchPayment;
	UCHAR   uchSysTypeTerm;
	UCHAR   uchTypeTerm;
}FDTPARA;

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
VOID    FDTInit(USHORT usMode);
/*VOID    THREADENTRY FDTMain(VOID);*/
VOID    FDTNoticeMessage(UCHAR uchMessage);
SHORT   FDTOpen(VOID);
VOID    FDTClose(VOID);

SHORT   FDTCreateFile(USHORT usNoOfItem);
SHORT   FDTChkAndCreFile(USHORT usNoOfItem);
VOID    FDTChkAndDelFile(USHORT usNoOfItem);

VOID    FDTTransOpen(GCNUM uchOrder);
VOID    FDTTransOpenStorePay(GCNUM uchOrder);
VOID    FDTSaveTrans(GCNUM  usGuestNo);
VOID    FDTSaveTransStorePay(GCNUM  usGuestNo);
SHORT   FDTForward(VOID);
SHORT   FDTBackward(VOID);
SHORT   FDTDeliveryRecall(VOID);
SHORT   FDTDeliveryAddon(VOID);
SHORT   FDTWait(GCNUM uchOrderNo, SHORT sType);
SHORT   FDTPaidOrderRecallOwn(VOID);
SHORT   FDTPaidOrderRecallGCF(GCNUM uchOrder);
VOID    FDTStore(VOID);
VOID    FDTPayment(GCNUM uchOrderNo);
VOID    FDTCancel(VOID);
VOID    FDTLeftArrow(VOID);
VOID    FDTRightArrow(VOID);
VOID    FDTDownArrow(VOID);
VOID    FDTUpArrow(VOID);
USHORT   FDTGetStoreQueue(VOID);
VOID    FDTGetStatus(FDTSTATUS *pData);

VOID    FDTParameter(FDTPARA    *pData);

#endif
/*======= End of Define ============*/
