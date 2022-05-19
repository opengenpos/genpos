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
*   Category           : Flexible Drive Through Thread, NCR2170 US HOSPITALITY MODEL.
*   Program Name       : FDTCOM.C
*  ------------------------------------------------------------------------
*    Abstract           :The provided function names are as follows
*        VOID    FDTParameter(FDTPARA *pData)
*        VOID    FDTDecideQueuePara(USHORT *pusQueue, USHORT *pusType)
*        VOID    FDTInsertWaitOrder(USHORT uchOrderNo)
*        VOID    FDTInsertStoreQueue(USHORT uchOrderNo)
*        VOID    FDTInsertTransQueue(USHORT uchOrderNo)
*        VOID    FDTRemoveWaitOrder(USHORT uchOrder)
*        VOID    FDTRemoveStoreQueue(USHORT uchOrderNo)
*        VOID    FDTRemoveTransQueue(USHORT uchOrder)
*        VOID    FDTClear(SHORT sType)
*        SHORT   FDTReadTransFromGCF(USHORT uchOrder2, USHORT uchOrder3, SHORT *psReturn)
*        SHORT   FDTReadTransFromGCFStorePay(USHORT uchOrder2, USHORT uchOrder3, SHORT *psReturn)
*        SHORT   FDTReadTransFromGCFDeliv(USHORT uchOrder2, USHORT uchOrder3, SHORT *psReturn)
*        SHORT   FDTGetTrans(USHORT uchOrderNo, SHORT sType)
*        VOID    FDTDispTotal(USHORT usScroll, SHORT sType)
*  ------------------------------------------------------------------------
*   Update Histories
*   Date     :Ver.Rev.:  NAME     :Description
*   Apr-17-95:        :hkato      : Initial.
\*=======================================================================*/
/*=======================================================================*\
:   PVCS ENTRY
:-------------------------------------------------------------------------
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/
#include	<tchar.h>
#include    <string.h>

#include    "ecr.h"
#include    "pif.h"
#include    "log.h"
#include    "plu.h"
#include    "appllog.h"
#include    "paraequ.h"
#include    "para.h"
#include    "regstrct.h"
#include    "transact.h"
#include    "storage.h"
#include    "prt.h"
#include    "fdt.h"
#include    "csgcs.h"
#include    "csstbgcf.h"
#include    "csstbfcc.h"
#include    "csstbstb.h"
#include    "csstbpar.h"
#include    "mld.h"
#include    "fdtlocal.h"


/*
*===========================================================================
** Synopsis:    VOID    FDTParameter(FDTPARA *pData)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Return Terminal Parameter(System, Terminal, Queue ...).
*===========================================================================
*/
VOID    FDTParameter(FDTPARA *pData)
{
    UCHAR        uchTermAddr = CliReadUAddr();
    USHORT       usDummy;
	PARAFXDRIVE  ParaFDT = { 0 };

    /*
			----- Set Terminal Unique Parameter -----
			We use CliReadUAddr to find out our terminal number.
			Terminal number is then used to index into the FDT terminal
			table data which is provisioned using PEP with Action Code 162.
			AC162 uses first four (4) table entries to determine the
			Delivery Terminal (addr 1 and 2) and the Payment Terminal (addr 3 and 4).

			So we take the terminal number, translate it to an odd value between 5 and 35
			to determine the index for the System Type Of Terminal to obtain the System
			Type Of Terminal settings for this terminal.
			Then we take the terminal number, translate it to an even value between 6 and 36
			to determine the index for the Terminal Type Of Terminal to obtain the Terminal
			Type of Terminal settings for this terminal.

			Read this with PEP up displaying Action Code 162 from the Maintenance Menu and
			all will be clear, Grasshopper.  Basically, each terminal has two different settings
			one after the other so the settings for terminal 1 are located in address 5 and 6
			for the System Type Of Terminal and the Terminal Type Of Terminal respectively.
			The settings for terminal 2 are located in address 7 and 8 and so on for each terminal
			up to terminal 16 where the settings are located in address 35 and 36.
	*/

    CliParaAllRead(CLASS_PARAFXDRIVE, ParaFDT.uchTblData, sizeof(ParaFDT.uchTblData), 0, &usDummy);

	memset(pData, 0, sizeof(FDTPARA));
	pData->uchSysTypeTerm = ParaFDT.uchTblData[FX_DRIVE_SYSTERM_1 - 1 + (uchTermAddr - 1) * 2];
    pData->uchTypeTerm    = ParaFDT.uchTblData[FX_DRIVE_TERMTYPE_1 - 1 + (uchTermAddr - 1) * 2];

    switch (pData->uchTypeTerm) {
    case    FX_DRIVE_ORDER_TERM_1:              /* Order Termninal (Queue#1) */
    case    FX_DRIVE_ORDER_PAYMENT_TERM_1:      /* Order/Payment Termninal (Queue#1) */
    case    FX_DRIVE_PAYMENT_TERM_1:            /* Payment Termninal (Queue#1) */
    case    FX_DRIVE_DELIV_1:                   /* Delivery Termninal (Queue#1) */
    case    FX_DRIVE_ORDER_FUL_TERM_1:
        pData->uchDelivery = ParaFDT.uchTblData[FX_DRIVE_DELIVERY_1];
        pData->uchPayment = ParaFDT.uchTblData[FX_DRIVE_PAYMENT_1];
        break;
    case    FX_DRIVE_ORDER_TERM_2:              /* Order Termninal (Queue#2) */
    case    FX_DRIVE_ORDER_PAYMENT_TERM_2:      /* Order/Payment Termninal (Queue#2) */
    case    FX_DRIVE_PAYMENT_TERM_2:            /* Payment Termninal (Queue#2) */
    case    FX_DRIVE_DELIV_2:                   /* Delivery Termninal (Queue#2) */
    case    FX_DRIVE_ORDER_FUL_TERM_2:
        pData->uchDelivery = ParaFDT.uchTblData[FX_DRIVE_DELIVERY_2];
        pData->uchPayment = ParaFDT.uchTblData[FX_DRIVE_PAYMENT_2];
        break;
    default:
        break;
    }
}

/*
*===========================================================================
** Synopsis:    FDTDecideQueuePara(USHORT *pusQueue, USHORT *pusType)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Decide Target Queue.
*===========================================================================
*/
VOID    FDTDecideQueuePara(USHORT *pusQueue, USHORT *pusType)
{
	FDTPARA     FDTPara = { 0 };

    FDTParameter(&FDTPara);

    /*----- Decide Target Queue/Transaction Type -----*/
    switch (FDTPara.uchTypeTerm) {
    case    FX_DRIVE_ORDER_PAYMENT_TERM_1:
    case    FX_DRIVE_PAYMENT_TERM_1:
        *pusQueue = GCF_PAYMENT_QUEUE1;
        *pusType = GCF_STORE;
        break;

    case    FX_DRIVE_ORDER_PAYMENT_TERM_2:
    case    FX_DRIVE_PAYMENT_TERM_2:
        *pusQueue = GCF_PAYMENT_QUEUE2;
        *pusType = GCF_STORE;
        break;

    case    FX_DRIVE_DELIV_1:
        *pusQueue = GCF_DELIVERY_QUEUE1;
        *pusType = GCF_STORE_PAYMENT;
        break;

    case    FX_DRIVE_DELIV_2:
        *pusQueue = GCF_DELIVERY_QUEUE2;
        *pusType = GCF_STORE_PAYMENT;
        break;

    case    FX_DRIVE_COUNTER_STORE:
    case    FX_DRIVE_COUNTER_STORE_PAY:
    case    FX_DRIVE_COUNTER_FUL_STORE:
    case    FX_DRIVE_COUNTER_FUL_STORE_PAY:
        *pusQueue = 0;
        *pusType = GCF_STORE_PAYMENT;
        break;

    default:
        *pusQueue = 0;
        *pusType = 0;
        break;
    }
}

/*
*===========================================================================
** Synopsis:    VOID    FDTInsertWaitOrder(USHORT uchOrderNo)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Insert Waited Order# to Wait Order Number Queue.
*===========================================================================
*/
VOID    FDTInsertWaitOrder(GCNUM uchOrderNo)
{
	SHORT sReturn = -1;
	int		i;

	for(i = 0; i < FDT_QUEUE_SIZE; i++)//SR 441, 473 JHHJ
	{
		if(FDTLocal.auchWaitOrder[i] == 0)
		{
			FDTLocal.auchWaitOrder[i] = uchOrderNo;
			sReturn = 0;
			break;
		}
    }

	NHPOS_ASSERT_TEXT(sReturn == 0, "ERROR: FDTInsertWaitOrder() no Room FDTLocal.auchWaitOrder");
}

/*
*===========================================================================
** Synopsis:    VOID    FDTInsertStoreQueue(USHORT uchOrderNo)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Insert Order# to Store/Payment Queue.
				This queue will only remember the last FDT_QUEUE_NUM_ITEMS
				guest checks.  If the queue is already full with the maximum
				number of guest checks, then when the new one is added to the
				front of the queue, the oldest drops off the end.
*===========================================================================
*/
VOID    FDTInsertStoreQueue(GCNUM uchOrderNo)
{
	SHORT sReturn = (FDTLocal.auchStorePayQueue[FDT_QUEUE_NUM_ITEMS] == 0) ? 0 : -1;

	NHPOS_ASSERT_TEXT(sReturn == 0, "ERROR: FDTInsertStoreQueue() no Room FDTLocal.auchStorePayQueue");

	/*----- Shift Order# -----*/
    FDT_memmove (&FDTLocal.auchStorePayQueue[1], &FDTLocal.auchStorePayQueue[0], FDT_QUEUE_NUM_ITEMS);

    /*----- Insert Latest Order# -----*/
    FDTLocal.auchStorePayQueue[0] = uchOrderNo;
}

/*
*===========================================================================
** Synopsis:    VOID    FDTInsertTransQueue(USHORT uchOrderNo)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Insert Order# to Transaction Queue.
				This queue will only remember the last FDT_QUEUE_NUM_ITEMS
				guest checks.  If the queue is already full with the maximum
				number of guest checks, then when the new one is added to the
				front of the queue, the oldest drops off the end.
*===========================================================================
*/
VOID    FDTInsertTransQueue(GCNUM uchOrderNo)
{
	SHORT sReturn = (FDTLocal.auchTransQueue[FDT_QUEUE_NUM_ITEMS] == 0) ? 0 : -1;

	NHPOS_ASSERT_TEXT(sReturn == 0, "ERROR: FDTInsertTransQueue() no Room FDTLocal.auchTransQueue");

	/*----- Shift Order# -----*/
    FDT_memmove(&FDTLocal.auchTransQueue[1], &FDTLocal.auchTransQueue[0], FDT_QUEUE_NUM_ITEMS);

    /*----- Insert Latest Order# -----*/
    FDTLocal.auchTransQueue[0] = uchOrderNo;
}

static VOID    FDTRemoveItemFromQueue(GCNUM uchOrderNo, USHORT usQueue[FDT_QUEUE_SIZE])
{
	if (uchOrderNo) {
		GCNUM   auchQueue[FDT_QUEUE_SIZE] = { 0 };
		SHORT   i, j = 0;

		for (i = 0; i < FDT_QUEUE_SIZE; i++) {
			if (usQueue[i] != uchOrderNo) {
				auchQueue[j] = usQueue[i];
				j++;
			}
		}

		memcpy(usQueue, auchQueue, sizeof(auchQueue));
	}
}

/*
*===========================================================================
** Synopsis:    VOID    FDTRemoveWaitOrder(USHORT uchOrderNo)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Remove Waited Order# from Wait Order Number Queue.

			WARNING: there are several dependencies on the queue entries being
			binary zero if the queue slot does not have a guest check number in
			it.  This function is designed to ensure that after removing a
			guest check number from the queue, the remaining items are rippled
			or shifted down and the queue is zero filled.
*
*           WARNING: This function modifies memory resident data guarded by
*                    PifRequestSem(husFDTFileSem); and it is expected the caller
*                    has the semaphore.
*
*===========================================================================
*/
VOID    FDTRemoveWaitOrder(GCNUM uchOrderNo)
{
#if 1
	FDTRemoveItemFromQueue(uchOrderNo, FDTLocal.auchWaitOrder);
#else
	if (uchOrder) {
		GCNUM   auchQueue[FDT_QUEUE_SIZE] = { 0 };
		SHORT   i, j = 0;

		for (i = 0; i < FDT_QUEUE_SIZE; i++) {
			if (FDTLocal.auchWaitOrder[i] != uchOrder) {
				auchQueue[j] = FDTLocal.auchWaitOrder[i];
				j++;
			}
		}

		memcpy(FDTLocal.auchWaitOrder, auchQueue, sizeof(FDTLocal.auchWaitOrder));
	}
#endif
}

/*
*===========================================================================
** Synopsis:    VOID    FDTRemoveStoreQueue(USHORT uchOrderNo)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Remove Order# from Store/Payment Queue.

			WARNING: there are several dependencies on the queue entries being
			binary zero if the queue slot does not have a guest check number in
			it.  This function is designed to ensure that after removing a
			guest check number from the queue, the remaining items are rippled
			or shifted down and the queue is zero filled.
*
*           WARNING: This function modifies memory resident data guarded by
*                    PifRequestSem(husFDTFileSem); and it is expected the caller
*                    has the semaphore.
*
*===========================================================================
*/
VOID    FDTRemoveStoreQueue(GCNUM uchOrderNo)
{
#if 1
	FDTRemoveItemFromQueue(uchOrderNo, FDTLocal.auchStorePayQueue);
#else
	if (uchOrderNo) {
		GCNUM   auchQueue[FDT_QUEUE_SIZE] = { 0 };
		SHORT   i, j = 0;

		for (i = 0; i < FDT_QUEUE_SIZE; i++) {
			if (FDTLocal.auchStorePayQueue[i] != uchOrderNo) {
				auchQueue[j] = FDTLocal.auchStorePayQueue[i];
				j++;
			}
		}

		memcpy(FDTLocal.auchStorePayQueue, auchQueue, sizeof(FDTLocal.auchStorePayQueue));
	}
#endif
}

/*
*===========================================================================
** Synopsis:    VOID    FDTRemoveTransQueue(USHORT uchOrderNo)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Remove Order# from Transaction Queue.

			WARNING: there are several dependencies on the queue entries being
			binary zero if the queue slot does not have a guest check number in
			it.  This function is designed to ensure that after removing a
			guest check number from the queue, the remaining items are rippled
			or shifted down and the queue is zero filled.
*
*           WARNING: This function modifies memory resident data guarded by
*                    PifRequestSem(husFDTFileSem); and it is expected the caller
*                    has the semaphore.
*
*===========================================================================
*/
VOID    FDTRemoveTransQueue(GCNUM uchOrderNo)
{
#if 1
	FDTRemoveItemFromQueue(uchOrderNo, FDTLocal.auchTransQueue);
#else
	if (uchOrderNo) {
		GCNUM   auchQueue[FDT_QUEUE_SIZE] = { 0 };
		SHORT   i, j = 0;

		for (i = 0; i < FDT_QUEUE_SIZE; i++) {
			if (FDTLocal.auchTransQueue[i] != uchOrderNo) {
				auchQueue[j] = FDTLocal.auchTransQueue[i];
				j++;
			}
		}

		memcpy(FDTLocal.auchTransQueue, auchQueue, sizeof(FDTLocal.auchTransQueue));
	}
#endif
}

/*
*===========================================================================
** Synopsis:    VOID    FDTClear(SHORT sType)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Clear FDTLocal #1 Area.
*===========================================================================
*/
VOID    FDTClear(SHORT sType)
{
    if (sType == 1) {
    /*----- Clear FDTLocal #1 Area -----*/
        FDTLocal.auchOrder[0] = 0;
        FDTLocal.uchOrderNo1 = 0;
        FDTLocal.lMI1 = 0L;
        FDTLocal.usTranVli1 = 0;
        FDTLocal.fsGCFStatus1 = 0;

    /*----- Clear FDTLocal #2 Area -----*/
    } else {
        FDTLocal.auchOrder[1] = 0;
        FDTLocal.uchOrderNo2 = 0;
        FDTLocal.lMI2 = 0L;
        FDTLocal.usTranVli2 = 0;
        FDTLocal.fsGCFStatus2 = 0;
    }
}

/*
*===========================================================================
** Synopsis:    SHORT   FDTReadTransFromGCF(USHORT uchOrder2,
*                                   USHORT uchOrder3, SHORT *psReturn)
*     Input:    
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Read Target Transaction from GCF, and Save to FDT Local File.
*===========================================================================
*/
SHORT   FDTReadTransFromGCF(GCNUM uchOrder2, GCNUM uchOrder3, SHORT *psReturn)
{
    SHORT       sStatus, sPosition2, sPosition3;

    /*----- Clear Display Case -----*/
    if (uchOrder2 == 0 && uchOrder3 == 0) {
        sPosition2 = 1;
        sPosition3 = 2;
        FDTClear(1);
        FDTClear(2);

    /*----- Trans to LCD#2 -----*/
    } else if (uchOrder2 != 0 && uchOrder3 == 0) {
        if (uchOrder2 == FDTLocal.uchOrderNo1) {
            sPosition2 = 1;
            sPosition3 = 2;
        } else if (uchOrder2 == FDTLocal.uchOrderNo2) {
            sPosition2 = 2;
            sPosition3 = 1;
        } else {
            sPosition2 = 1;
            sPosition3 = 2;
        }

    /*----- Trans to LCD#2,3 -----*/
    } else {
        if (uchOrder2 == FDTLocal.uchOrderNo1) {
            sPosition2 = 1;
            sPosition3 = 2;
        } else {
            if (uchOrder2 == FDTLocal.uchOrderNo2) {
                sPosition2 = 2;
                sPosition3 = 1;
            } else {
                if (uchOrder3 == FDTLocal.uchOrderNo1) {
                    sPosition2 = 2;
                    sPosition3 = 1;
                } else if (uchOrder3 == FDTLocal.uchOrderNo2) {
                    sPosition2 = 1;
                    sPosition3 = 2;
                } else {
                    sPosition2 = 1;
                    sPosition3 = 2;
                }
            }
        }
    }

    /*----- Get Order#2 Trans to FDT File Area(Empty) -----*/
    if (uchOrder2 != 0) {
        /*----- Read Order#2 Trans from GCF and Save to File Area -----*/
        sStatus = FDTGetTrans(uchOrder2, sPosition2);
        if (sStatus > 0) {
            *psReturn |= FDT_STATUS_GET_FILE1;

        } else if (sStatus == GCF_EXIST) {
            *psReturn &= ~FDT_STATUS_GET_FILE1;

        } else {
            FDTLocal.auchOrder[sPosition2 - 1] = 0;
            return(FDT_ERROR);
        }
    } else {
        if (uchOrder2 == 0) {
            FDTLocal.auchOrder[sPosition2 - 1] = 0;
        }
    }

    /*----- Get Order#3 Trans to FDT File Area(Empty) -----*/
    if (uchOrder3 != 0) {
        /*----- Read Order#3 Trans from GCF and Save to File Area -----*/
        sStatus = FDTGetTrans(uchOrder3, sPosition3);
        if (sStatus > 0) {
            *psReturn |= FDT_STATUS_GET_FILE2;

        } else if (sStatus == GCF_EXIST) {
            *psReturn &= ~FDT_STATUS_GET_FILE2;

        } else {
            FDTLocal.auchOrder[sPosition3 - 1] = 0;
            return(FDT_ERROR);
        }
    } else {
        if (uchOrder3 == 0) {
            FDTLocal.auchOrder[sPosition3 - 1] = 0;
        }
    }

    return(FDT_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT   FDTReadTransFromGCFStotePay(USHORT uchOrder2,
*                                   USHORT uchOrder3, SHORT *psReturn)
*     Input:    
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Read Target Transaction from GCF, and Save to FDT Local File.
*===========================================================================
*/
SHORT   FDTReadTransFromGCFStorePay(GCNUM uchOrder2, GCNUM uchOrder3, SHORT *psReturn)
{
    SHORT       sStatus, sPosition2, sPosition3;

    /*----- Clear Display Case -----*/
    if (uchOrder2 == 0 && uchOrder3 == 0) {
        sPosition2 = 1;
        sPosition3 = 2;
        FDTClear(1);
        FDTClear(2);

    /*----- Trans to LCD#2 -----*/
    } else if (uchOrder2 != 0 && uchOrder3 == 0) {
        if (uchOrder2 == FDTLocal.uchOrderNo1) {
            sPosition2 = 1;
            sPosition3 = 2;
        } else if (uchOrder2 == FDTLocal.uchOrderNo2) {
            sPosition2 = 2;
            sPosition3 = 1;
        } else {
            sPosition2 = 1;
            sPosition3 = 2;
        }

    /*----- Trans to LCD#2,3 -----*/
    } else {
        if (uchOrder2 == FDTLocal.uchOrderNo1) {
            sPosition2 = 1;
            sPosition3 = 2;
        } else {
            if (uchOrder2 == FDTLocal.uchOrderNo2) {
                sPosition2 = 2;
                sPosition3 = 1;
            } else {
                if (uchOrder3 == FDTLocal.uchOrderNo1) {
                    sPosition2 = 2;
                    sPosition3 = 1;
                } else if (uchOrder3 == FDTLocal.uchOrderNo2) {
                    sPosition2 = 1;
                    sPosition3 = 2;
                } else {
                    sPosition2 = 1;
                    sPosition3 = 2;
                }
            }
        }
    }

    /*----- Get Order#2 Trans to FDT File Area(Empty) -----*/
    if (uchOrder2 != 0) {
        /*----- Read Order#2 Trans from GCF and Save to File Area -----*/
        sStatus = FDTGetTrans(uchOrder2, sPosition2);
        if (sStatus > 0) {
            *psReturn |= FDT_STATUS_GET_FILE1;

        } else if (sStatus == GCF_EXIST) {
            *psReturn &= ~FDT_STATUS_GET_FILE1;

        } else if (sStatus == GCF_NOT_IN) {
            FDTRemoveStoreQueue(uchOrder2);
            return(FDT_ERROR);

        } else {
            FDTLocal.auchOrder[sPosition2 - 1] = 0;
            return(FDT_ERROR);
        }
    } else {
        if (uchOrder2 == 0) {
            FDTLocal.auchOrder[sPosition2 - 1] = 0;
        }
    }

    /*----- Get Order#3 Trans to FDT File Area(Empty) -----*/
    if (uchOrder3 != 0) {
        /*----- Read Order#3 Trans from GCF and Save to File Area -----*/
        sStatus = FDTGetTrans(uchOrder3, sPosition3);
        if (sStatus > 0) {
            *psReturn |= FDT_STATUS_GET_FILE2;

        } else if (sStatus == GCF_EXIST) {
            *psReturn &= ~FDT_STATUS_GET_FILE2;

        } else if (sStatus == GCF_NOT_IN) {
            FDTRemoveStoreQueue(uchOrder3);
            return(FDT_ERROR);

        } else {
            FDTLocal.auchOrder[sPosition3 - 1] = 0;
            return(FDT_ERROR);
        }
    } else {
        if (uchOrder3 == 0) {
            FDTLocal.auchOrder[sPosition3 - 1] = 0;
        }
    }

    return(FDT_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT   FDTReadTransFromGCFDeliv(USHORT uchOrder2,
*                                       USHORT uchOrder3, SHORT *psReturn)
*     Input:    
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Read Target Transaction from GCF, and Save to FDT Local File.
*===========================================================================
*/
SHORT   FDTReadTransFromGCFDeliv(GCNUM uchOrder2, GCNUM uchOrder3, SHORT *psReturn)
{
    SHORT   sStatus, sPosition3;

    if (uchOrder2 == FDTLocal.uchOrderNo1) {
        sPosition3 = 2;
    } else {
        sPosition3 = 1;
    }

    /*----- Get Order#3 Trans to FDT File Area(Empty) -----*/
    sStatus = FDTGetTrans(uchOrder3, sPosition3);
    if (sStatus > 0) {
        *psReturn |= FDT_STATUS_GET_FILE2;

    } else if (sStatus == GCF_EXIST) {
        *psReturn &= ~FDT_STATUS_GET_FILE2;

    } else {
        FDTLocal.auchOrder[sPosition3 - 1] = 0;
        return(FDT_ERROR);
    }

    return(FDT_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT   FDTGetTrans(USHORT uchOrderNo, SHORT sType)
*     Input:    USHORT   uchOrderNo(Target Order#), SHORT sType(Storage#)
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Get Target Transaction from GCF, and Save to FDT Local File.
*===========================================================================
*/
SHORT   FDTGetTrans(GCNUM uchOrderNo, SHORT sType)
{
    SHORT           sStatus;
    SHORT           husHandle;
    USHORT          usTranVli;
	ULONG		    ulActualBytesRead;	//RPH 11-10-3 SR# 201

    if (sType == 1) {
		if (FDTLocal.husTranHandle1 < 0) return FDT_ERROR;
        husHandle = FDTLocal.husTranHandle1;
        if (uchOrderNo == FDTLocal.uchOrderNo1) {
            usTranVli = FDTLocal.usTranVli1;

            /*----- Not Need to Read in case of Payment Trans -----*/
            if (FDTLocal.fsGCFStatus1 & GCFQUAL_PAYMENT_TRANS) {
                return(GCF_EXIST);
            }
        } else {
            usTranVli = 0;
        }
    } else {
		if (FDTLocal.husTranHandle2 < 0) return FDT_ERROR;
        husHandle = FDTLocal.husTranHandle2;
        if (uchOrderNo == FDTLocal.uchOrderNo2) {
            usTranVli = FDTLocal.usTranVli2;

            /*----- Not Need to Read in case of Payment Trans -----*/
            if (FDTLocal.fsGCFStatus2 & GCFQUAL_PAYMENT_TRANS) {
                return(GCF_EXIST);
            }
        } else {
            usTranVli = 0;
        }
    }

	//RPH SR#201
	sStatus = CliGusCheckAndReadFH(uchOrderNo, usTranVli, husHandle,
        sizeof(USHORT) + sizeof(USHORT), FDTLocal.usTranSize1, &ulActualBytesRead);
    /*----- Get Terget Transaction from GCF -----*/
    if (sStatus <= 0) {
        return(sStatus);
    } else {
		TRANCONSSTORAGEHEADER  TempStorageHeader;
        /*----- Set Order# to FDT Local -----*/
        /*----- Set Transaction Itemizer to FDT Local -----*/
        FDTReadFile(0, &TempStorageHeader, sizeof(TRANCONSSTORAGEHEADER), husHandle, &ulActualBytesRead);

        if (sType == 1) {
            FDTLocal.uchOrderNo1 = TempStorageHeader.TranGCFQual.usGuestNo;
            FDTLocal.lMI1 = TempStorageHeader.TranItemizers.lMI;
            FDTLocal.usTranVli1 = sStatus;
            FDTLocal.fsGCFStatus1 = TempStorageHeader.TranGCFQual.fsGCFStatus2;
        } else {
            FDTLocal.uchOrderNo2 = TempStorageHeader.TranGCFQual.usGuestNo;
            FDTLocal.lMI2 = TempStorageHeader.TranItemizers.lMI;
            FDTLocal.usTranVli2 = sStatus;
            FDTLocal.fsGCFStatus2 = TempStorageHeader.TranGCFQual.fsGCFStatus2;
        }
		return(sStatus);
    }
}

/*
*===========================================================================
** Synopsis:    VOID    FDTDispTotal(USHORT usScroll, SHORT sType)
*     Input:    SHORT sType(Storage#)
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Display Total in case of Not Payment GCF.
*===========================================================================
*/
VOID    FDTDispTotal(USHORT usScroll, SHORT sType)
{
     if (sType == 1) {
        MldDispSubTotal(usScroll, FDTLocal.lMI1);
     } else {
        MldDispSubTotal(usScroll, FDTLocal.lMI2);
     }
}

/*========= End of File =========*/