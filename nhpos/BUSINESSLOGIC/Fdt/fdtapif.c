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
*   Program Name       : FDTAPIF.C
*  ------------------------------------------------------------------------
*    Abstract           :The provided function names are as follows
*		SHORT   FDT_memmove (GCNUM *gcDest, GCNUM *gcSource, int nItems)
*		VOID    FDTTransOpen(GCNUM uchOrder)
*		VOID    FDTTransOpenStorePay(GCNUM uchOrder)
*		VOID    FDTSaveTrans(TRANINFORMATION *pData)
*		VOID    FDTSaveTransStorePay(TRANINFORMATION *pData)
*		SHORT   FDTForward(VOID)
*		SHORT   FDTBackward(VOID)
*		SHORT   FDTDeliveryRecall(TRANINFORMATION *pData)
*		SHORT   FDTDeliveryAddon(TRANINFORMATION *pData)
*		SHORT   FDTWait(GCNUM uchOrderNo, SHORT sType)
*		SHORT   FDTPaidOrderRecallOwn(TRANINFORMATION *pData)
*		SHORT   FDTPaidOrderRecallGCF(GCNUM uchOrder, TRANINFORMATION *pData)
*		VOID    FDTStore(VOID)
*		VOID    FDTPayment(GCNUM uchOrderNo)
*		VOID    FDTCancel(VOID)
*		VOID    FDTLeftArrow(VOID)
*		VOID    FDTRightArrow(VOID)
*		VOID    FDTDownArrow(VOID)
*		VOID    FDTUpArrow(VOID)
*		USHORT  FDTGetStoreQueue(VOID)
*		VOID    FDTGetStatus(FDTSTATUS *pData)
*
*  ------------------------------------------------------------------------
*   Update Histories
*   Date     :Ver.Rev.:  NAME     :Description
*   Apr-18-95:        :hkato      : Initial.
*
** NCR2171 **                                         
*   Aug-26-99:01.00.00:mteraki    : initial (for 2171)
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
#include	"rfl.h"
#include    "log.h"
#include    "plu.h"
#include    "appllog.h"
#include    "uie.h"
#include    "paraequ.h"
#include    "para.h"
#include    "regstrct.h"
#include    "transact.h"
#include    "storage.h"
#include    "csgcs.h"
#include    "csstbgcf.h"
#include    "csstbfcc.h"
#include    "csstbstb.h"
#include    "csstbpar.h"
#include    "fdt.h"
#include    "mld.h"
#include    "fdtlocal.h"


// used in someplaces as more than max GCNUM value.  was 0xff
#define     FDT_OVER_MAX_GCNUM        0x7fff


// used to replace instances of memmove () functions which were used
// to move guest check number queues when guest check numbers were UCHARs
SHORT  FDT_memmove (GCNUM *gcDest, GCNUM *gcSource, int nItems)
{
	if (nItems) {
		memmove (gcDest, gcSource, nItems * sizeof(GCNUM));
	}
	return nItems;
}


/*
*===========================================================================
** Synopsis:    VOID    FDTTransOpen(UCHAR uchOrder)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Start Transaction at Order or Counter Terminal.
*===========================================================================
*/
VOID    FDTTransOpen(GCNUM uchOrder)
{
	GCNUM		uchOrder2 = 0, uchOrder3 = 0;
    SHORT       sReturn = 0, sFlag = 0;
	FDTPARA     FDTPara = { 0 };

    PifRequestSem(husFDTFileSem);

    FDTParameter(&FDTPara);
      
    if (FDTPara.uchTypeTerm == FX_DRIVE_ORDER_FUL_TERM_1
        || FDTPara.uchTypeTerm == FX_DRIVE_ORDER_FUL_TERM_2
        || FDTPara.uchTypeTerm == FX_DRIVE_COUNTER_STORE
        || FDTPara.uchTypeTerm == FX_DRIVE_COUNTER_FUL_STORE) {

        /*----- Use Forward Key -----*/
        if (FDTLocal.auchOrder[0] != 0
            && FDTLocal.auchOrder[0] != FDTLocal.auchStorePayQueue[0]) {

            if (uchOrder == FDTLocal.auchOrder[0]
                || uchOrder == FDTLocal.auchOrder[1]) {

                FDTRemoveStoreQueue(uchOrder);
                uchOrder2 = FDTLocal.auchStorePayQueue[0];
                uchOrder3 = FDTLocal.auchStorePayQueue[1];

            } else {
                uchOrder2 = FDTLocal.auchOrder[0];
                uchOrder3 = FDTLocal.auchOrder[1];
                FDTRemoveStoreQueue(uchOrder);
            }

        /*----- Not Use Forward Key -----*/
        } else {
            if (uchOrder != 0) {
                FDTRemoveStoreQueue(uchOrder);
            }
            uchOrder2 = FDTLocal.auchStorePayQueue[0];
            uchOrder3 = FDTLocal.auchStorePayQueue[1];
        }

        /*----- Get Order#2,3 Transaction to FDT File Area -----*/
        if (FDTReadTransFromGCF(uchOrder2, uchOrder3, &sReturn) != FDT_SUCCESS) {
            FDTLocal.auchOrder[0] = 0;
            FDTLocal.auchOrder[1] = 0;
            uchOrder2 = uchOrder3 = 0;
        }

        if (uchOrder2 != 0) {
			/*----- Display Transaction on LCD #2 -----*/
            if (uchOrder2 != FDTLocal.auchOrder[0]
                || sReturn & FDT_STATUS_GET_FILE1)
			{
				/*----- Already Display Case -----*/

                if (uchOrder2 == FDTLocal.uchOrderNo1)
				{
					/*----- Display File #1 -----*/
                    MldPutGcfToScroll(MLD_SCROLL_2, FDTLocal.husTranHandle1);
                    FDTDispTotal(MLD_TOTAL_2, 1);
                } else {
					/*----- Display File #2 -----*/
                    MldPutGcfToScroll(MLD_SCROLL_2, FDTLocal.husTranHandle2);
                    FDTDispTotal(MLD_TOTAL_2, 2);
                }
            }
        } else {
			/*----- Not Display on LCD #2 -----*/
            MldDisplayInit(MLD_DRIVE_2, 0);
        }

        /*----- Check Counter(Store) Terminal ----*/
        if (FDTPara.uchTypeTerm == FX_DRIVE_COUNTER_STORE
            || FDTPara.uchTypeTerm == FX_DRIVE_COUNTER_FUL_STORE)
		{
			FDTRemoveWaitOrder(uchOrder);
			/*----- Case of Trans Not Display on LCD#3 ----*/
			if (FDTLocal.auchWaitOrder[0] == 0) {
				sFlag = 1;
			}
        }

        /*----- Display Transaction on LCD #3 -----*/
        if (FDTLocal.auchWaitOrder[0] == 0) {
            /*----- Display Transaction on LCD #3 -----*/
            if (uchOrder3 != 0) {
                /*----- Not Already Display Case -----*/
                if (uchOrder3 != FDTLocal.auchOrder[1]
                    || sReturn & FDT_STATUS_GET_FILE2
                    || sFlag == 1) {

                    MldDisplayInit(MLD_DRIVE_3, 0);

                    /*----- Display File #1 -----*/
                    if (uchOrder3 == FDTLocal.uchOrderNo1) {
                        MldPutGcfToScroll(MLD_SCROLL_3, FDTLocal.husTranHandle1);
                        FDTDispTotal(MLD_TOTAL_3, 1);

                    /*----- Display File #2 -----*/
                    } else {
                        MldPutGcfToScroll(MLD_SCROLL_3, FDTLocal.husTranHandle2);
                        FDTDispTotal(MLD_TOTAL_3, 2);
                    }
                }

            /*----- Not Display on LCD #2 -----*/
            } else {
               MldDisplayInit(MLD_DRIVE_3, 0);
            }
        } else {
			FDTWAITORDER    WorkWait = { 0 };

			/*----- Copy Waited Order# -----*/
            memcpy(WorkWait.uchOrderNo, FDTLocal.auchWaitOrder, sizeof(WorkWait.uchOrderNo));

            /*----- Display Order# to LCD #3 -----*/
            MldDispWaitKey(&WorkWait);
        }

        /*----- Save Display Order No -----*/
        FDTLocal.auchOrder[0] = uchOrder2;
        FDTLocal.auchOrder[1] = uchOrder3;
    }

    PifReleaseSem(husFDTFileSem);
}

/*
*===========================================================================
** Synopsis:    VOID    FDTTransOpenStorePay(UCHAR uchOrder)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Start Transaction at Counter Terminal(Store/Payment).
*===========================================================================
*/
VOID    FDTTransOpenStorePay(GCNUM uchOrder)
{
    GCNUM       uchOrder2 = 0, uchOrder3 = 0;
    SHORT       sReturn = 0, i;
	FDTPARA     FDTPara = { 0 };

    PifRequestSem(husFDTFileSem);

    FDTParameter(&FDTPara);

    if (FDTPara.uchTypeTerm == FX_DRIVE_ORDER_TERM_1
        || FDTPara.uchTypeTerm == FX_DRIVE_ORDER_TERM_2
        || FDTPara.uchTypeTerm == FX_DRIVE_COUNTER_STORE_PAY
        || FDTPara.uchTypeTerm == FX_DRIVE_COUNTER_FUL_STORE_PAY) {

        /*----- Use Forward Key -----*/
        if (FDTLocal.auchOrder[0] != 0
            && FDTLocal.auchOrder[0] != FDTLocal.auchStorePayQueue[0]) {

            if (uchOrder == FDTLocal.auchOrder[0]
                || uchOrder == FDTLocal.auchOrder[1]) {

                FDTRemoveStoreQueue(uchOrder);
                uchOrder2 = FDTLocal.auchStorePayQueue[0];
                uchOrder3 = FDTLocal.auchStorePayQueue[1];

            } else {
                uchOrder2 = FDTLocal.auchOrder[0];
                uchOrder3 = FDTLocal.auchOrder[1];
                FDTRemoveStoreQueue(uchOrder);
            }

        /*----- Not Use Forward Key -----*/
        } else {
            if (uchOrder != 0) {
                FDTRemoveStoreQueue(uchOrder);
            }
            uchOrder2 = FDTLocal.auchStorePayQueue[0];
            uchOrder3 = FDTLocal.auchStorePayQueue[1];
        }

        /*----- Get Order#2,3 Transaction to FDT File Area -----*/
        for (i = 0; i < FDT_QUEUE_SIZE; i++) {
            if (FDTReadTransFromGCFStorePay(uchOrder2, uchOrder3, &sReturn) != FDT_SUCCESS) {

                /*----- Start from First Queue -----*/
                uchOrder2 = FDTLocal.auchStorePayQueue[0];
                uchOrder3 = FDTLocal.auchStorePayQueue[1];

            } else {
                break;
            }
        }
        if (i == FDT_QUEUE_SIZE) {
            FDTLocal.auchOrder[0] = 0;
            FDTLocal.auchOrder[1] = 0;
            uchOrder2 = uchOrder3 = 0;
        }

        /*----- Display Transaction on LCD #2 -----*/
        if (uchOrder2 != 0) {

            /*----- Already Display Case -----*/
            if (uchOrder2 != FDTLocal.auchOrder[0]
                || sReturn & FDT_STATUS_GET_FILE1) {

                /*----- Display File #1 -----*/
                if (uchOrder2 == FDTLocal.uchOrderNo1) {
                    MldPutGcfToScroll(MLD_SCROLL_2, FDTLocal.husTranHandle1);
                    FDTDispTotal(MLD_TOTAL_2, 1);

                /*----- Display File #2 -----*/
                } else {
                    MldPutGcfToScroll(MLD_SCROLL_2, FDTLocal.husTranHandle2);
                    FDTDispTotal(MLD_TOTAL_2, 2);
                }
            }

        /*----- Not Display on LCD #2 -----*/
        } else {
            MldDisplayInit(MLD_DRIVE_2, 0);
        }

        /*----- Display Transaction on LCD #3 -----*/
        if (FDTLocal.auchWaitOrder[0] == 0) {

            /*----- Display Transaction on LCD #3 -----*/
            if (uchOrder3 != 0) {

                /*----- Not Already Display Case -----*/
                if (uchOrder3 != FDTLocal.auchOrder[1]
                    || sReturn & FDT_STATUS_GET_FILE2) {

                    MldDisplayInit(MLD_DRIVE_3, 0);

                    /*----- Display File #1 -----*/
                    if (uchOrder3 == FDTLocal.uchOrderNo1) {
                        MldPutGcfToScroll(MLD_SCROLL_3, FDTLocal.husTranHandle1);
                        FDTDispTotal(MLD_TOTAL_3, 1);

                    /*----- Display File #2 -----*/
                    } else {
                        MldPutGcfToScroll(MLD_SCROLL_3, FDTLocal.husTranHandle2);
                        FDTDispTotal(MLD_TOTAL_3, 2);
                    }
                }

            /*----- Not Display on LCD #2 -----*/
            } else {
               MldDisplayInit(MLD_DRIVE_3, 0);
            }
        } else {
			FDTWAITORDER    WorkWait = { 0 };

			/*----- Copy Waited Order# -----*/
            memcpy(WorkWait.uchOrderNo, FDTLocal.auchWaitOrder, sizeof(WorkWait.uchOrderNo));

            /*----- Display Order# to LCD #3 -----*/
            MldDispWaitKey(&WorkWait);
        }

        /*----- Save Display Order No -----*/
        FDTLocal.auchOrder[0] = uchOrder2;
        FDTLocal.auchOrder[1] = uchOrder3;
    }

    PifReleaseSem(husFDTFileSem);
}

/*
*===========================================================================
** Synopsis:    VOID    FDTSaveTrans(TRANINFORMATION *pData)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Save Transaction Information to FDT Local Data.
*===========================================================================
*/
VOID    FDTSaveTrans(GCNUM  usGuestNo)
{
    GCNUM       uchOrder2 = 0, uchOrder3 = 0;
    SHORT       sReturn = 0;
	FDTPARA     FDTPara = { 0 };

    PifRequestSem(husFDTFileSem);

    FDTParameter(&FDTPara);

    if (FDTPara.uchTypeTerm == FX_DRIVE_COUNTER_STORE
        || FDTPara.uchTypeTerm == FX_DRIVE_ORDER_FUL_TERM_1
        || FDTPara.uchTypeTerm == FX_DRIVE_ORDER_FUL_TERM_2
        || FDTPara.uchTypeTerm == FX_DRIVE_COUNTER_FUL_STORE) {

        /*----- Use Forward Key -----*/
        if (FDTLocal.auchOrder[0] != FDTLocal.auchStorePayQueue[0]) {
			FDTInsertStoreQueue(usGuestNo);
            uchOrder2 = FDTLocal.auchOrder[0];
            uchOrder3 = FDTLocal.auchOrder[1];

        /*----- Not Use Forward Key -----*/
        } else {
			FDTInsertStoreQueue(usGuestNo);
            uchOrder2 = FDTLocal.auchStorePayQueue[0];
            uchOrder3 = FDTLocal.auchStorePayQueue[1];
        }

        /*----- Get Order#2,3 Transaction to FDT File Area -----*/
        if (FDTReadTransFromGCF(uchOrder2, uchOrder3, &sReturn) != FDT_SUCCESS) {
            FDTLocal.auchOrder[0] = 0;
            FDTLocal.auchOrder[1] = 0;
            uchOrder2 = uchOrder3 = 0;
        }

        /*----- Display Transaction on LCD #2 -----*/
        if (uchOrder2 != 0) {
            if (uchOrder2 != FDTLocal.auchOrder[0]
                || sReturn & FDT_STATUS_GET_FILE1) {

                if (uchOrder2 == FDTLocal.uchOrderNo1) {
                    MldPutGcfToScroll(MLD_SCROLL_2, FDTLocal.husTranHandle1);
                    FDTDispTotal(MLD_TOTAL_2, 1);

                } else {
                    MldPutGcfToScroll(MLD_SCROLL_2, FDTLocal.husTranHandle2);
                    FDTDispTotal(MLD_TOTAL_2, 2);
                }
            }

        } else {
            MldDisplayInit(MLD_DRIVE_2, 0);
        }

        /*----- Display Transaction on LCD #3 -----*/
        if (FDTLocal.auchWaitOrder[0] == 0) {
            if (uchOrder3 != 0) {
                if (uchOrder3 != FDTLocal.auchOrder[1]
                    || sReturn & FDT_STATUS_GET_FILE2) {

                    MldDisplayInit(MLD_DRIVE_3, 0);

                    if (uchOrder3 == FDTLocal.uchOrderNo1) {
                        MldPutGcfToScroll(MLD_SCROLL_3, FDTLocal.husTranHandle1);
                        FDTDispTotal(MLD_TOTAL_3, 1);

                    } else {
                        MldPutGcfToScroll(MLD_SCROLL_3, FDTLocal.husTranHandle2);
                        FDTDispTotal(MLD_TOTAL_3, 2);
                    }
                }

            } else {
               MldDisplayInit(MLD_DRIVE_3, 0);
            }

        } else {
			FDTWAITORDER    WorkWait = { 0 };

			/*----- Copy Waited Order# -----*/
            memcpy(WorkWait.uchOrderNo, FDTLocal.auchWaitOrder, sizeof(WorkWait.uchOrderNo));

            /*----- Display Order# to LCD #3 -----*/
            MldDispWaitKey(&WorkWait);
        }

        /*----- Save Display Order No -----*/
        FDTLocal.auchOrder[0] = uchOrder2;
        FDTLocal.auchOrder[1] = uchOrder3;
    }

    PifReleaseSem(husFDTFileSem);
}

/*
*===========================================================================
** Synopsis:    VOID    FDTSaveTransStorePay(TRANINFORMATION *pData)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Save Transaction Information to FDT Local Data.
*===========================================================================
*/
VOID    FDTSaveTransStorePay(GCNUM  usGuestNo)
{
    SHORT       sReturn = 0, i;
	FDTPARA     FDTPara = { 0 };

    PifRequestSem(husFDTFileSem);

    FDTParameter(&FDTPara);

    if (FDTPara.uchTypeTerm == FX_DRIVE_ORDER_TERM_1
        || FDTPara.uchTypeTerm == FX_DRIVE_ORDER_TERM_2
        || FDTPara.uchTypeTerm == FX_DRIVE_COUNTER_STORE_PAY
        || FDTPara.uchTypeTerm == FX_DRIVE_COUNTER_FUL_STORE_PAY) {
		GCNUM       uchOrder2 = 0, uchOrder3 = 0;

        /*----- Use Forward Key -----*/
        if (FDTLocal.auchOrder[0] != FDTLocal.auchStorePayQueue[0]) {
			FDTInsertStoreQueue(usGuestNo);
            uchOrder2 = FDTLocal.auchOrder[0];
            uchOrder3 = FDTLocal.auchOrder[1];

        /*----- Not Use Forward Key -----*/
        } else {
            FDTInsertStoreQueue(usGuestNo);
            uchOrder2 = FDTLocal.auchStorePayQueue[0];
            uchOrder3 = FDTLocal.auchStorePayQueue[1];
        }

        /*----- Get Order#2,3 Transaction to FDT File Area -----*/
        for (i = 0; i < FDT_QUEUE_SIZE; i++) {
            if (FDTReadTransFromGCFStorePay(uchOrder2, uchOrder3, &sReturn) != FDT_SUCCESS) {

                /*----- Start from First Queue -----*/
                uchOrder2 = FDTLocal.auchStorePayQueue[0];
                uchOrder3 = FDTLocal.auchStorePayQueue[1];

            } else {
                break;
            }
        }
        if (i == FDT_QUEUE_SIZE) {
            FDTLocal.auchOrder[0] = 0;
            FDTLocal.auchOrder[1] = 0;
            uchOrder2 = uchOrder3 = 0;
        }

        /*----- Display Transaction on LCD #2 -----*/
        if (uchOrder2 != 0) {
            if (uchOrder2 != FDTLocal.auchOrder[0]
                || sReturn & FDT_STATUS_GET_FILE1) {

                if (uchOrder2 == FDTLocal.uchOrderNo1) {
                    MldPutGcfToScroll(MLD_SCROLL_2, FDTLocal.husTranHandle1);
                    FDTDispTotal(MLD_TOTAL_2, 1);

                } else {
                    MldPutGcfToScroll(MLD_SCROLL_2, FDTLocal.husTranHandle2);
                    FDTDispTotal(MLD_TOTAL_2, 2);
                }
            }

        } else {
            MldDisplayInit(MLD_DRIVE_2, 0);
        }

        /*----- Display Transaction on LCD #3 -----*/
        if (FDTLocal.auchWaitOrder[0] == 0) {
            if (uchOrder3 != 0) {
                if (uchOrder3 != FDTLocal.auchOrder[1]
                    || sReturn & FDT_STATUS_GET_FILE2) {

                    MldDisplayInit(MLD_DRIVE_3, 0);

                    if (uchOrder3 == FDTLocal.uchOrderNo1) {
                        MldPutGcfToScroll(MLD_SCROLL_3, FDTLocal.husTranHandle1);
                        FDTDispTotal(MLD_TOTAL_3, 1);

                    } else {
                        MldPutGcfToScroll(MLD_SCROLL_3, FDTLocal.husTranHandle2);
                        FDTDispTotal(MLD_TOTAL_3, 2);
                    }
                }

            } else {
               MldDisplayInit(MLD_DRIVE_3, 0);
            }

        } else {
			FDTWAITORDER    WorkWait = { 0 };

			/*----- Copy Waited Order# -----*/
            memcpy(WorkWait.uchOrderNo, FDTLocal.auchWaitOrder, sizeof(WorkWait.uchOrderNo));

            /*----- Display Order# to LCD #3 -----*/
            MldDispWaitKey(&WorkWait);
        }

        /*----- Save Display Order No -----*/
        FDTLocal.auchOrder[0] = uchOrder2;
        FDTLocal.auchOrder[1] = uchOrder3;
    }

    PifReleaseSem(husFDTFileSem);
}

/*
*===========================================================================
** Synopsis:    SHORT   FDTForward(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Display Forward Transaction to LCD.
*===========================================================================
*/
SHORT   FDTForward(VOID)
{
	GCNUM	uchOrderNo2 = 0, uchOrderNo3 = 0;
    SHORT   sStatus = GCF_NOT_IN;
	SHORT   sReturn = 0;
	FDTPARA FDTPara = { 0 };

    PifRequestSem(husFDTFileSem);

    FDTParameter(&FDTPara);

    /*----- No Change Case -----*/
    if (FDTLocal.usTranVli1 == 0 || FDTLocal.usTranVli2 == 0) {
        PifReleaseSem(husFDTFileSem);
        return(FDT_SUCCESS);
    }

    /*----- Decide Next Order# -----*/
    uchOrderNo2 = FDTLocal.auchOrder[1];

    /*----- Decide Target Queue, Type -----*/
    if (FDTPara.uchTypeTerm == FX_DRIVE_COUNTER_STORE
        || FDTPara.uchTypeTerm == FX_DRIVE_COUNTER_STORE_PAY
        || FDTPara.uchTypeTerm == FX_DRIVE_ORDER_TERM_1
        || FDTPara.uchTypeTerm == FX_DRIVE_ORDER_TERM_2) {
		GCNUM   *puchPtr = NULL;
		SHORT   i;

		puchPtr = NULL;
		for(i = 0; i < FDT_QUEUE_SIZE; i++)////SR 441 JHHJ
		{
			if(FDTLocal.auchStorePayQueue[i] == uchOrderNo2)
			{
				puchPtr = &FDTLocal.auchStorePayQueue[i];
				break;
			}
		}

        if (puchPtr) {
            if (*(puchPtr + 1) != 0) {
                uchOrderNo3 = *(puchPtr + 1);
            } else {
                PifReleaseSem(husFDTFileSem);
                return(FDT_SUCCESS);
            }
        } else {
            PifReleaseSem(husFDTFileSem);
            return(FDT_SUCCESS);
        }
    } else {
		USHORT  usQueue = 0, usType = 0;
		/*----- Get Next Order# from GCF Queue -----*/
        FDTDecideQueuePara(&usQueue, &usType);
        sStatus = CliGusInformQueue(usQueue, usType, uchOrderNo2, GCF_FORWARD);
        if (sStatus > 0) {
			uchOrderNo3 = sStatus;
        } else if (sStatus == GCF_NOT_IN) {
            PifReleaseSem(husFDTFileSem);
            return(FDT_SUCCESS);
        } else {
            PifReleaseSem(husFDTFileSem);
            return(GusConvertError(sStatus));
        }
    }

    /*----- Get Order#2,3 Transaction to FDT File Area -----*/
    if (FDTPara.uchTypeTerm == FX_DRIVE_COUNTER_STORE_PAY
        || FDTPara.uchTypeTerm == FX_DRIVE_ORDER_TERM_1
        || FDTPara.uchTypeTerm == FX_DRIVE_ORDER_TERM_2) {
		SHORT   i;

        for (i = 0; i < FDT_QUEUE_SIZE; i++) {
            if (FDTReadTransFromGCFStorePay(uchOrderNo2, uchOrderNo3, &sReturn) != FDT_SUCCESS) {

                /*----- Start from First Queue -----*/
                uchOrderNo2 = FDTLocal.auchStorePayQueue[0];
                uchOrderNo3 = FDTLocal.auchStorePayQueue[1];

            } else {
                break;
            }
        }
        if (i == FDT_QUEUE_SIZE) {
            FDTLocal.auchOrder[0] = FDTLocal.auchOrder[1] = 0;
            uchOrderNo2 = uchOrderNo3 = 0;
            FDTClear(1);
            FDTClear(2);

            PifReleaseSem(husFDTFileSem);
            return(GusConvertError(sStatus));
        }

    } else {
        if (FDTReadTransFromGCF(uchOrderNo2, uchOrderNo3, &sReturn) != FDT_SUCCESS) {
            FDTLocal.auchOrder[0] = FDTLocal.auchOrder[1] = 0;
            uchOrderNo2 = uchOrderNo3 = 0;
            FDTClear(1);
            FDTClear(2);

            PifReleaseSem(husFDTFileSem);
            return(GusConvertError(sStatus));
        }
    }

    /*----- Display Transaction to LCD #2 -----*/
    if (uchOrderNo2 != FDTLocal.auchOrder[0]
        || sReturn & FDT_STATUS_GET_FILE1) {

        if (uchOrderNo2 == FDTLocal.uchOrderNo1) {
            MldPutGcfToScroll(MLD_SCROLL_2, FDTLocal.husTranHandle1);
            FDTDispTotal(MLD_TOTAL_2, 1);

        } else {
            MldPutGcfToScroll(MLD_SCROLL_2, FDTLocal.husTranHandle2);
            FDTDispTotal(MLD_TOTAL_2, 2);
        }
    }

    /*----- Display Transaction to LCD #3 -----*/
    if (FDTLocal.auchWaitOrder[0] == 0) {
        if (uchOrderNo3 != FDTLocal.auchOrder[1]
            || sReturn & FDT_STATUS_GET_FILE2) {

            if (uchOrderNo3 == FDTLocal.uchOrderNo1) {
                MldPutGcfToScroll(MLD_SCROLL_3, FDTLocal.husTranHandle1);
                FDTDispTotal(MLD_TOTAL_3, 1);

            } else {
                MldPutGcfToScroll(MLD_SCROLL_3, FDTLocal.husTranHandle2);
                FDTDispTotal(MLD_TOTAL_3, 2);
            }
        }
    }

    /*----- Save Display Order No -----*/
    FDTLocal.auchOrder[0] = uchOrderNo2;
    FDTLocal.auchOrder[1] = uchOrderNo3;

    PifReleaseSem(husFDTFileSem);

    return(FDT_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT   FDTBackward(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Display Backward Transaction to LCD.
*===========================================================================
*/
SHORT   FDTBackward(VOID)
{
	GCNUM	uchOrderNo2 = 0, uchOrderNo3 = 0;
    SHORT   sStatus = GCF_NOT_IN;
	SHORT   sReturn = 0, sFlag = 0;
	FDTPARA FDTPara = { 0 };

    PifRequestSem(husFDTFileSem);
 
    FDTParameter(&FDTPara);

    /*----- No Change Case -----*/
    if (FDTLocal.usTranVli1 == 0 || FDTLocal.usTranVli2 == 0) {
        PifReleaseSem(husFDTFileSem);
        return(FDT_SUCCESS);
    }

    /*----- Decide Next Order# -----*/
    uchOrderNo3 = FDTLocal.auchOrder[0];

    /*----- Decide Target Queue, Type -----*/
    if (FDTPara.uchTypeTerm == FX_DRIVE_COUNTER_STORE
        || FDTPara.uchTypeTerm == FX_DRIVE_COUNTER_STORE_PAY
        || FDTPara.uchTypeTerm == FX_DRIVE_ORDER_TERM_1
        || FDTPara.uchTypeTerm == FX_DRIVE_ORDER_TERM_2) {
		GCNUM   *puchPtr = NULL;

		if (uchOrderNo3) {
			SHORT   i;
			for(i = FDT_QUEUE_SIZE - 1; i >= 0; i--)//SR 441 JHHJ
			{
				if(FDTLocal.auchStorePayQueue[i] == uchOrderNo3)
				{
					puchPtr = &FDTLocal.auchStorePayQueue[i];
					break;
				}
			}
		}
        if (puchPtr) {
            if (FDTLocal.auchStorePayQueue[0] == uchOrderNo3) {
                PifReleaseSem(husFDTFileSem);
                return(FDT_SUCCESS);
            } else {
                uchOrderNo2 = *(puchPtr - 1);
            }
        } else {
            PifReleaseSem(husFDTFileSem);
            return(FDT_SUCCESS);
        }

    } else {
        /*----- Get Next Order# from GCF Queue -----*/
		USHORT  usQueue = 0, usType = 0;
		FDTDecideQueuePara(&usQueue, &usType);
        sStatus = CliGusInformQueue(usQueue, usType, uchOrderNo3, GCF_BACKWARD);
        if (sStatus > 0) {
			uchOrderNo2 = sStatus;
        } else if (sStatus == GCF_NOT_IN) {
            PifReleaseSem(husFDTFileSem);
            return(FDT_SUCCESS);
        } else {
            PifReleaseSem(husFDTFileSem);
            return(GusConvertError(sStatus));
        }
    }

    /*----- Get Order#2,3 Transaction to FDT File Area -----*/
    if (FDTPara.uchTypeTerm == FX_DRIVE_COUNTER_STORE_PAY
        || FDTPara.uchTypeTerm == FX_DRIVE_ORDER_TERM_1
        || FDTPara.uchTypeTerm == FX_DRIVE_ORDER_TERM_2) {
		SHORT   i;

        for (i = 0; i < FDT_QUEUE_SIZE; i++) {
            if (FDTReadTransFromGCFStorePay(uchOrderNo2, uchOrderNo3, &sReturn) != FDT_SUCCESS) {
                /*----- Start from First Queue -----*/
                uchOrderNo2 = FDTLocal.auchStorePayQueue[0];
                uchOrderNo3 = FDTLocal.auchStorePayQueue[1];
            } else {
                break;
            }
        }
        if (i == FDT_QUEUE_SIZE) {
            FDTLocal.auchOrder[0] = FDTLocal.auchOrder[1] = 0;
            uchOrderNo2 = uchOrderNo3 = 0;
            FDTClear(1);
            FDTClear(2);

            PifReleaseSem(husFDTFileSem);
            return(GusConvertError(sStatus));
        }
    } else {
        if (FDTReadTransFromGCF(uchOrderNo2, uchOrderNo3, &sReturn) != FDT_SUCCESS) {
            FDTLocal.auchOrder[0] = FDTLocal.auchOrder[1] = 0;
            uchOrderNo2 = uchOrderNo3 = 0;
            FDTClear(1);
            FDTClear(2);

            PifReleaseSem(husFDTFileSem);
            return(GusConvertError(sStatus));
        }
    }

    /*----- Display Transaction to LCD #2 -----*/
    if (uchOrderNo2 != FDTLocal.auchOrder[0]
        || sReturn & FDT_STATUS_GET_FILE1) {

        /*----- Display Changed -----*/
        if (uchOrderNo2 != FDTLocal.auchOrder[0]) {
            sFlag = 1;
        }

        if (uchOrderNo2 == FDTLocal.uchOrderNo1) {
            MldPutGcfToScroll(MLD_SCROLL_2, FDTLocal.husTranHandle1);
            FDTDispTotal(MLD_TOTAL_2, 1);

        } else {
            MldPutGcfToScroll(MLD_SCROLL_2, FDTLocal.husTranHandle2);
            FDTDispTotal(MLD_TOTAL_2, 2);
        }
    }

    /*----- Display Transaction to LCD #3 -----*/
    if (FDTLocal.auchWaitOrder[0] == 0) {
        if (uchOrderNo3 != FDTLocal.auchOrder[1]
            || sReturn & FDT_STATUS_GET_FILE2) {

            /*----- Display Changed -----*/
            if (uchOrderNo3 != FDTLocal.auchOrder[1]) {
                sFlag = 1;
            }

            if (uchOrderNo3 == FDTLocal.uchOrderNo1) {
                MldPutGcfToScroll(MLD_SCROLL_3, FDTLocal.husTranHandle1);
                FDTDispTotal(MLD_TOTAL_3, 1);

            } else {
                MldPutGcfToScroll(MLD_SCROLL_3, FDTLocal.husTranHandle2);
                FDTDispTotal(MLD_TOTAL_3, 2);
            }
        }
    }

    /*----- Save Display Order No -----*/
    FDTLocal.auchOrder[0] = uchOrderNo2;
    FDTLocal.auchOrder[1] = uchOrderNo3;

    PifReleaseSem(husFDTFileSem);

    if (sFlag == 0) {
        return(FDT_SUCCESS);
    } else {
        return(FDT_DISP_CHANGE);
    }
}

/*
*===========================================================================
** Synopsis:    SHORT   FDTDeliveryRecall(TRANINFORMATION *pData)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Shift Transaction to Left at Delivery Terminal.
*===========================================================================
*/
SHORT   FDTDeliveryRecall(VOID)
{
	GCNUM   uchOrder1 = 0, uchOrder2 = 0, uchOrder3 = 0, auchOrder[2] = { 0 };
    SHORT   sStatus, sValue = 0, sReturn = FDT_SUCCESS;
    USHORT  usQueue, usType, usStatus;

    PifRequestSem(husFDTFileSem);

    FDTDecideQueuePara(&usQueue, &usType);

    /*----- Previous is Addon Transaction -----*/
    if (FDTLocal.uchAddonOrder != 0) {

        /*----- Get Second Order# from Queue -----*/
        sStatus = CliGusInformQueue(usQueue, usType, 0, GCF_FORWARD);
        if (sStatus > 0) {
			uchOrder3 = sStatus;
        } else {
            uchOrder3 = FDT_OVER_MAX_GCNUM;
        }

        /*----- Not Forward State -----*/
        if (uchOrder3 == FDTLocal.auchOrder[1]) {
            if (FDTLocal.auchOrder[1] != 0) {
				ULONG   fsGCFStat = 0;

                /*----- LCD#3 is Store Transaction -----*/
                if (FDTLocal.auchOrder[1] == FDTLocal.uchOrderNo1) {
                    fsGCFStat = FDTLocal.fsGCFStatus1;
                } else {
                    fsGCFStat = FDTLocal.fsGCFStatus2;
                }
                if ((fsGCFStat & GCFQUAL_PAYMENT_TRANS) == 0) {
                    uchOrder1 = 0;
                    uchOrder2 = FDTLocal.auchOrder[1];

                    /*----- for LCD#3 -----*/
                    sStatus = CliGusInformQueue(usQueue, usType, uchOrder2, GCF_FORWARD);
                    if (sStatus > 0) {
						uchOrder3 = sStatus;
                    } else {
                        uchOrder3 = 0;
                    }
                /*----- LCD#3 is Payment Transaction -----*/
                } else {
                    uchOrder1 = FDTLocal.auchOrder[1];

                    /*----- for LCD#2 -----*/
                    sStatus = CliGusInformQueue(usQueue, usType, uchOrder1, GCF_FORWARD);
                    if (sStatus > 0) {
						uchOrder2 = sStatus;

                        /*----- for LCD#3 -----*/
                        sStatus = CliGusInformQueue(usQueue, usType, uchOrder2, GCF_FORWARD);
                        if (sStatus > 0) {
							uchOrder3 = sStatus;
                        } else {
                            uchOrder3 = 0;
                        }
                    } else {
                        uchOrder2 = uchOrder3 = 0;
                    }
                }
            } else {
                uchOrder1 = uchOrder2 = uchOrder3 = 0;
            }

        /*----- Forward State -----*/
        } else {
            /*----- Transaction for LCD#2 -----*/
            uchOrder1 = 0;
            sStatus = CliGusInformQueue(usQueue, usType, 0, GCF_FORWARD);
            if (sStatus > 0) {
				uchOrder2 = sStatus;

                /*----- Transaction for LCD#3 -----*/
                sStatus = CliGusInformQueue(usQueue, usType, uchOrder2, GCF_FORWARD);
                if (sStatus > 0) {
					uchOrder3 = sStatus;
                } else {
                    uchOrder3 = 0;
                }
            } else {
                uchOrder2 = uchOrder3 = 0;
            }
        }

    /*----- Previous is Not Addon Transaction -----*/
    } else {
		TRANGCFQUAL     WorkGCF;

		/*----- for LCD#1 -----*/
        sStatus = CliGusDeliveryIndRead(usQueue, (UCHAR *)&WorkGCF, sizeof(TRANGCFQUAL), auchOrder);

        /*----- First Trans. in Queue is Payment State -----*/
        if (sStatus > 0) {
			uchOrder1 = WorkGCF.usGuestNo;

            /*----- Not Forward State -----*/
            if (uchOrder1 == FDTLocal.auchOrder[0]) {

                /*----- for LCD#2 -----*/
                sStatus = CliGusInformQueue(usQueue, usType, uchOrder1, GCF_FORWARD);
                if (sStatus > 0) {
                    //uchOrder2 = (UCHAR)sStatus;
					uchOrder2 = sStatus;

                    /*----- for LCD#3 -----*/
                    sStatus = CliGusInformQueue(usQueue, usType, uchOrder2, GCF_FORWARD);
                    if (sStatus > 0) {
                        //uchOrder3 = (UCHAR)sStatus;
						uchOrder3 = sStatus;
                    } else {
                        uchOrder3 = 0;
                    }
                } else {
                    uchOrder2 = uchOrder3 = 0;
                }
            /*----- Forward State -----*/
            } else {
                uchOrder2 = FDTLocal.auchOrder[0];
                uchOrder3 = FDTLocal.auchOrder[1];
            }

        /*----- All Trans in Queue are Store State -----*/
        } else if (sStatus == GCF_NOT_IN_PAID) {
            uchOrder1 = 0;
            uchOrder2 = FDTLocal.auchOrder[0];
            uchOrder3 = FDTLocal.auchOrder[1];
            sReturn = LDT_PROHBT_ADR;

        /*----- Exist Payment Trans in Queue -----*/
        } else if (sStatus == GCF_CHANGE_QUEUE) {
            uchOrder1 = 0;

            /*----- Not Forward State -----*/
            if (auchOrder[0] == FDTLocal.auchOrder[1]) {
                uchOrder2 = auchOrder[0];
                uchOrder3 = auchOrder[1];

            /*----- Forward State -----*/
            } else {
                uchOrder2 = FDTLocal.auchOrder[0];
                uchOrder3 = FDTLocal.auchOrder[1];
            }

        /*----- Comm. Error -----*/
        } else {
            uchOrder1 = uchOrder2 = uchOrder3 = 0;
        }
    }

    /*----- Delete Latest Order# from Delivery Queue -----*/
    if (uchOrder1 != 0) {
		FDTPARA     FDTPara = { 0 };

		FDTParameter(&FDTPara);
        if (FDTPara.uchTypeTerm == FX_DRIVE_DELIV_1) {
            usType = GCF_DRIVE_TH_DELIVERY1;
        } else {
            usType = GCF_DRIVE_TH_DELIVERY2;
        }
        while ((sStatus = CliGusDeleteDeliveryQueue(uchOrder1, usType)) != GCF_SUCCESS) {
            usStatus = GusConvertError(sStatus);
            UieErrorMsg(usStatus, UIE_WITHOUT);
        }
    }

    FDTLocal.uchAddonOrder = 0;

    /*----- Get Transaction for LCD#1, 2, 3 -----*/
    /*----- Get Order#1 Transaction to Transaction Information (LCD#1) -----*/
    if (uchOrder1 != 0) {
        if ((sStatus = TrnQueryGC(uchOrder1)) != TRN_SUCCESS) {
            PifReleaseSem(husFDTFileSem);
            return(GusConvertError(sStatus));
        }
    } else {
        TrnInitialize( TRANI_GCFQUAL | TRANI_CURQUAL | TRANI_ITEMIZERS | TRANI_ITEM | TRANI_CONSOLI );
    }

    /*----- Get Order#2,3 Transaction to FDT File Area -----*/
    if (FDTReadTransFromGCF(uchOrder2, uchOrder3, &sValue) != FDT_SUCCESS) {
        FDTLocal.auchOrder[0] = 0;
        FDTLocal.auchOrder[1] = 0;
        FDTClear(1);
        FDTClear(2);

        PifReleaseSem(husFDTFileSem);
        return(GusConvertError(sStatus));
    }

    /*----- Display Transaction on LCD#1 -----*/
    if (uchOrder1 != 0) {
		MldDisplayInit(MLD_DRIVE_1, 0);
        MldPutGcfToScroll(MLD_SCROLL_1, TrnInformationPtr->hsTranConsStorage);
        MldDispSubTotal(MLD_TOTAL_1, TranItemizersPtr->lMI);
    } else {
        MldDisplayInit(MLD_DRIVE_1, 0);
    }

    /*----- Display Transaction on LCD #2 -----*/
    if (uchOrder2 != 0) {
        if (uchOrder2 != FDTLocal.auchOrder[0]
            || sValue & FDT_STATUS_GET_FILE1) {

            MldDisplayInit(MLD_DRIVE_2, 0);

            if (uchOrder2 == FDTLocal.uchOrderNo1) {
                MldPutGcfToScroll(MLD_SCROLL_2, FDTLocal.husTranHandle1);
                FDTDispTotal(MLD_TOTAL_2, 1);

            } else {
                MldPutGcfToScroll(MLD_SCROLL_2, FDTLocal.husTranHandle2);
                FDTDispTotal(MLD_TOTAL_2, 2);
            }
        }

    } else {
        MldDisplayInit(MLD_DRIVE_2, 0);
    }

    /*----- Display Transaction on LCD #3 -----*/
    if (uchOrder3 != 0) {
        if (uchOrder3 != FDTLocal.auchOrder[1]
            || sValue & FDT_STATUS_GET_FILE2) {

            if (uchOrder3 == FDTLocal.uchOrderNo1) {
                MldPutGcfToScroll(MLD_SCROLL_3, FDTLocal.husTranHandle1);
                FDTDispTotal(MLD_TOTAL_3, 1);

            } else {
                MldPutGcfToScroll(MLD_SCROLL_3, FDTLocal.husTranHandle2);
                FDTDispTotal(MLD_TOTAL_3, 2);
            }
        }

    } else {
        MldDisplayInit(MLD_DRIVE_3, 0);
    }

    /*----- Save Display Order No -----*/
    FDTLocal.auchOrder[0] = uchOrder2;
    FDTLocal.auchOrder[1] = uchOrder3;
    
    PifReleaseSem(husFDTFileSem);

    return(sReturn);
}

/*
*===========================================================================
** Synopsis:    SHORT   FDTDeliveryAddon(TRANINFORMATION *pData)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Add-on Transaction at Delivery Terminal.
*===========================================================================
*/
SHORT   FDTDeliveryAddon(VOID)
{
    GCNUM   uchOrder2 = 0, uchOrder3 = 0;
    SHORT   sReturn = 0;

    PifRequestSem(husFDTFileSem);

    /*----- for LCD#2, 3 -----*/
    /*----- Previous Operation is Paid Order Recall -----*/
	if (TrnInformationPtr->TranGCFQual.usGuestNo == FDTLocal.uchOwnOrder
		|| TrnInformationPtr->TranGCFQual.usGuestNo == FDTLocal.uchGCFOrder) {
        uchOrder2 = FDTLocal.auchOrder[0];
        uchOrder3 = FDTLocal.auchOrder[1];

    } else {
		uchOrder2 = TrnInformationPtr->TranGCFQual.usGuestNo;

        if (FDTLocal.auchOrder[0] != 0) {
            uchOrder3 = FDTLocal.auchOrder[0];
        } else {
            uchOrder3 = 0;
        }

        /*----- Save Payment Order# -----*/
        if (TrnInformationPtr->TranGCFQual.fsGCFStatus2 & GCFQUAL_PAYMENT_TRANS) {
            FDTLocal.uchAddonOrder = uchOrder2;
        }
    }

    /*----- Get Order#2,3 Transaction to FDT File Area -----*/
    if (FDTReadTransFromGCF(uchOrder2, uchOrder3, &sReturn) != FDT_SUCCESS) {
        FDTLocal.auchOrder[0] = 0;
        FDTLocal.auchOrder[1] = 0;
        uchOrder2 = uchOrder3 = 0;
    }

    /*----- Display Transaction on LCD #2 -----*/
    if (uchOrder2 != 0) {
        if (uchOrder2 != FDTLocal.auchOrder[0]
            || sReturn & FDT_STATUS_GET_FILE1) {

            MldDisplayInit(MLD_DRIVE_2, 1);

            if (uchOrder2 == FDTLocal.uchOrderNo1) {
                MldPutGcfToScroll(MLD_SCROLL_2, FDTLocal.husTranHandle1);
                FDTDispTotal(MLD_TOTAL_2, 1);

            } else {
                MldPutGcfToScroll(MLD_SCROLL_2, FDTLocal.husTranHandle2);
                FDTDispTotal(MLD_TOTAL_2, 2);
            }
        }

    } else {
        FDTClear(1);
        MldDisplayInit(MLD_DRIVE_2, 0);
    }

    /*----- Display Transaction on LCD #3 -----*/
    if (uchOrder3 != 0) {
        if (uchOrder3 != FDTLocal.auchOrder[1]
            || sReturn & FDT_STATUS_GET_FILE2) {

            if (uchOrder3 == FDTLocal.uchOrderNo1) {
                MldPutGcfToScroll(MLD_SCROLL_3, FDTLocal.husTranHandle1);
                FDTDispTotal(MLD_TOTAL_3, 1);

            } else {
                MldPutGcfToScroll(MLD_SCROLL_3, FDTLocal.husTranHandle2);
                FDTDispTotal(MLD_TOTAL_3, 2);
            }
        }

    } else {
        FDTClear(2);
        MldDisplayInit(MLD_DRIVE_3, 0);
    }

    /*----- Save Display Order No -----*/
    FDTLocal.auchOrder[0] = uchOrder2;
    FDTLocal.auchOrder[1] = uchOrder3;

    PifReleaseSem(husFDTFileSem);

    return(FDT_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT   FDTWait(UCHAR uchOrderNo, SHORT sType)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Display Waited Order# to LCD.
*===========================================================================
*/
SHORT   FDTWait(GCNUM uchOrderNo, SHORT sType)
{
	FDTPARA     FDTPara = { 0 };

    /*----- Check Order# ----*/
    if (uchOrderNo == 0) {
        return(LDT_PROHBT_ADR);
    }

    PifRequestSem(husFDTFileSem);

    FDTParameter(&FDTPara);
    if (FDTPara.uchTypeTerm == FX_DRIVE_PAYMENT_TERM_1
        || FDTPara.uchTypeTerm == FX_DRIVE_PAYMENT_TERM_2
        || FDTPara.uchTypeTerm == FX_DRIVE_ORDER_PAYMENT_TERM_1
        || FDTPara.uchTypeTerm == FX_DRIVE_ORDER_PAYMENT_TERM_2
        || FDTPara.uchTypeTerm == FX_DRIVE_COUNTER_STORE_PAY
        || FDTPara.uchTypeTerm == FX_DRIVE_COUNTER_FUL_STORE_PAY
        || sType == 1)
	{
		FDTWAITORDER    WorkWait = {0};

        /*----- Save Order# to FDTLocal -----*/
        FDTInsertWaitOrder(uchOrderNo);

        /*----- copy Waited Order# -----*/
        memcpy(WorkWait.uchOrderNo, FDTLocal.auchWaitOrder, sizeof(WorkWait.uchOrderNo));
        
        /*----- Display Order# to LCD #3 -----*/
        MldDisplayInit(MLD_DRIVE_3, 0);
        MldDispWaitKey(&WorkWait);
    }

    PifReleaseSem(husFDTFileSem);

    return(FDT_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT   FDTPaidOrderRecallOwn(TRANINFORMATION *pData)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Paid Order Recall.
*===========================================================================
*/
SHORT   FDTPaidOrderRecallOwn(VOID)
{
    GCNUM   *puchPtr = 0;
    GCNUM   uchOrder1 = 0;
    SHORT   sStatus = FDT_SUCCESS, sFlag = 0;
	UCHAR i;

    PifRequestSem(husFDTFileSem);

    /*----- First Depression -----*/
    if (FDTLocal.uchOwnOrder == 0) {

        /*----- No Transaction -----*/
        if (FDTLocal.auchTransQueue[0] == 0) {
            uchOrder1 = 0;
            sStatus = LDT_NTINFL_ADR;

        /*----- Exist Transaction -----*/
        } else {
            uchOrder1 = FDTLocal.auchTransQueue[0];
        }

    /*----- Not First Depression -----*/
    } else {
		puchPtr = NULL;
		for (i = 0; i < FDT_QUEUE_SIZE; i++)//SR 441 JHHJ
		{
			if(FDTLocal.auchTransQueue[i] == FDTLocal.uchOwnOrder)
			{
				puchPtr = &FDTLocal.auchTransQueue[i];
				break;
			}
		}
        /*----- Decide Next Order# -----*/
        if (puchPtr) {

            /*----- Next Order# -----*/
            if (*(puchPtr + 1) != 0) {
                uchOrder1 = *(puchPtr + 1);

            /*----- Next Order# -----*/
            } else {
                uchOrder1 = 0;
                sStatus = LDT_NTINFL_ADR;
            }
        }
    }

    /*----- Display Waited Order# on LCD#3 ? ----*/

	puchPtr = NULL;
	for (i = 0; i < FDT_QUEUE_SIZE; i++)//SR 441 JHHJ
	{
		if(FDTLocal.auchWaitOrder[i] == uchOrder1)
		{
			puchPtr = &FDTLocal.auchWaitOrder[i];
			break;
		}
	}
    if (puchPtr && uchOrder1 != 0) {
		FDTPARA     FDTPara = { 0 };

        /*----- Check Counter(Store) Terminal ----*/
        FDTParameter(&FDTPara);
        if (FDTPara.uchTypeTerm != FX_DRIVE_COUNTER_STORE
            && FDTPara.uchTypeTerm != FX_DRIVE_COUNTER_FUL_STORE) {
            FDTRemoveWaitOrder(uchOrder1);
    
            /*----- Case of Trans Not Display on LCD#3 ----*/
            if (FDTLocal.auchWaitOrder[0] == 0) {
                sFlag = 1;
            }
        }
    }

    /*----- Get Order#1 Transaction to Transaction Information (LCD#1) -----*/
    if (uchOrder1 != 0) {
        if ((sStatus = TrnQueryGC(uchOrder1)) != TRN_SUCCESS) {
            uchOrder1 = 0;
            sStatus = GusConvertError(sStatus);
        } else {
            if (TrnInformationPtr->TranModeQual.fbCashierStatus & MODEQUAL_CASHIER_TRAINING) {
                if (!(TranGCFQualPtr->fsGCFStatus & GCFQUAL_TRAINING)) {
                    uchOrder1 = 0;
                    sStatus = LDT_PROHBT_ADR;
                    TrnInitialize( TRANI_GCFQUAL | TRANI_CURQUAL | TRANI_ITEMIZERS | TRANI_ITEM | TRANI_CONSOLI );
                }
            } else {
                if (TranGCFQualPtr->fsGCFStatus & GCFQUAL_TRAINING) {
                    uchOrder1 = 0;
                    sStatus = LDT_PROHBT_ADR;
                    TrnInitialize( TRANI_GCFQUAL | TRANI_CURQUAL | TRANI_ITEMIZERS | TRANI_ITEM | TRANI_CONSOLI );
                }
            }
        }

    } else {
        TrnInitialize( TRANI_GCFQUAL | TRANI_CURQUAL | TRANI_ITEMIZERS | TRANI_ITEM | TRANI_CONSOLI );
    }

    /*----- Display Transaction on LCD#1 -----*/
    if (uchOrder1 != 0) {
		MldDisplayInit(MLD_DRIVE_1, 0);
        MldPutGcfToScroll(MLD_SCROLL_1, TrnInformationPtr->hsTranConsStorage);
        MldDispSubTotal(MLD_TOTAL_1, TranItemizersPtr->lMI);
    } else {
        MldDisplayInit(MLD_DRIVE_1, 0);
    }

    /*----- Display Transaction on LCD #3 -----*/
    if (uchOrder1 != 0) {
        if (FDTLocal.auchWaitOrder[0] == 0) {
            if (sFlag == 1) {
                if (FDTLocal.auchOrder[1] != 0) {
                    if (FDTLocal.auchOrder[1] == FDTLocal.uchOrderNo1) {
                        MldDisplayInit(MLD_DRIVE_3, 0);
                        MldPutGcfToScroll(MLD_SCROLL_3, FDTLocal.husTranHandle1);
                        FDTDispTotal(MLD_TOTAL_3, 1);
                    } else {
                        MldDisplayInit(MLD_DRIVE_3, 0);
                        MldPutGcfToScroll(MLD_SCROLL_3, FDTLocal.husTranHandle2);
                        FDTDispTotal(MLD_TOTAL_3, 2);
                    }
                } else {
                    MldDisplayInit(MLD_DRIVE_3, 0);
                }
            }

        /*----- Display Waited Order# on LCD #3 -----*/
        } else {
			FDTWAITORDER    WorkWait = { 0 };

			/*----- Copy Waited Order# -----*/
            memcpy(WorkWait.uchOrderNo, FDTLocal.auchWaitOrder, sizeof(WorkWait.uchOrderNo));

            /*----- Display Order# to LCD #3 -----*/
            MldDispWaitKey(&WorkWait);
        }

        /*----- Save Current Order# -----*/
        FDTLocal.uchOwnOrder = uchOrder1;
    }

    FDTLocal.uchPrevPayTrans = 0;

    PifReleaseSem(husFDTFileSem);

    return(sStatus);
}

/*
*===========================================================================
** Synopsis:    SHORT   FDTPaidOrderRecallGCF(UCHAR uchOrder, TRANINFORMATION *pData)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Paid Order Recall.
*===========================================================================
*/
SHORT   FDTPaidOrderRecallGCF(GCNUM uchOrder)
{
    USHORT    *puchPtr;
    GCNUM   uchOrder1 = 0;
    SHORT   sStatus = FDT_SUCCESS, i = 0, sFlag = 0;

    PifRequestSem(husFDTFileSem);

    /*----- First Depression -----*/
    if (uchOrder != 0) {
        uchOrder1 = uchOrder;

        /*----- Read Transaction -----*/
        if ((sStatus = TrnQueryGC(uchOrder1)) == TRN_SUCCESS) {
            /*----- Check Payment Transaction -----*/
            if ((TranGCFQualPtr->fsGCFStatus2 & GCFQUAL_PAYMENT_TRANS) == 0) {
                TrnInitialize(TRANI_GCFQUAL | TRANI_ITEMIZERS | TRANI_ITEM | TRANI_CONSOLI);
                uchOrder1 = 0;
                sStatus = LDT_PROHBT_ADR;
            }

            /*----- Check Normal/Training Transaction -----*/
            if (TrnInformationPtr->TranModeQual.fbCashierStatus & MODEQUAL_CASHIER_TRAINING) {
                if (!(TranGCFQualPtr->fsGCFStatus & GCFQUAL_TRAINING)) {
                    TrnInitialize(TRANI_GCFQUAL | TRANI_ITEMIZERS
                        | TRANI_ITEM | TRANI_CONSOLI);
                    uchOrder1 = 0;
                    sStatus = LDT_PROHBT_ADR;
                }
            } else {
                if (TranGCFQualPtr->fsGCFStatus & GCFQUAL_TRAINING) {
                    TrnInitialize(TRANI_GCFQUAL | TRANI_ITEMIZERS
                        | TRANI_ITEM | TRANI_CONSOLI);
                    uchOrder1 = 0;
                    sStatus = LDT_PROHBT_ADR;
                }
            }

        /*----- Error(Communication etc.) -----*/
        } else {
            uchOrder1 = 0;
            sStatus = GusConvertError(sStatus);
        }

    /*----- Not First Depression -----*/
    } else {
        /*----- Read Order Trans from GCF Module -----*/
        for (i = 0; i < GCF_MAX_DRIVE_NUMBER; i++) {
            /*----- Decide Target Order# -----*/
            if (FDTLocal.uchGCFOrder == 1) {
                uchOrder1 = GCF_MAX_DRIVE_NUMBER;
            } else {
                if (FDTLocal.uchGCFOrder > 0) {
					FDTLocal.uchGCFOrder--;
				}
                uchOrder1 = FDTLocal.uchGCFOrder;
            }

            /*----- Read Transaction -----*/
            if ((sStatus = TrnQueryGC(uchOrder1)) == GCF_NOT_IN) {
                continue;

            /*----- Check Transaction -----*/
            } else if (sStatus == TRN_SUCCESS) {
                /*----- Check Normal/Training Transaction -----*/
                if (TrnInformationPtr->TranModeQual.fbCashierStatus & MODEQUAL_CASHIER_TRAINING) {
                    if (!(TranGCFQualPtr->fsGCFStatus & GCFQUAL_TRAINING)) {
                        TrnInitialize(TRANI_GCFQUAL | TRANI_ITEMIZERS | TRANI_ITEM | TRANI_CONSOLI);
                        continue;
                    }
                } else {
                    if (TranGCFQualPtr->fsGCFStatus & GCFQUAL_TRAINING) {
                        TrnInitialize(TRANI_GCFQUAL | TRANI_ITEMIZERS | TRANI_ITEM | TRANI_CONSOLI);
                        continue;
                    }
                }
        
                /*----- Store Transaction -----*/
                if ((TranGCFQualPtr->fsGCFStatus2 & GCFQUAL_PAYMENT_TRANS) == 0) {
                    TrnInitialize(TRANI_GCFQUAL | TRANI_ITEMIZERS | TRANI_ITEM | TRANI_CONSOLI);
                    continue;
                }

                /*----- OK -----*/
                break;

            /*----- Error(Communication etc.) -----*/
            } else {
                sStatus = GusConvertError(sStatus);
            }
        }
    }

    if (i == GCF_MAX_DRIVE_NUMBER) {
        uchOrder1 = 0;
        sStatus = LDT_PROHBT_ADR;
    }

	puchPtr = NULL;
	for (i = 0; i < FDT_QUEUE_SIZE; i++)//SR 441 JHHJ
	{
		if (FDTLocal.auchWaitOrder[i] == uchOrder1)
		{
			puchPtr = &FDTLocal.auchWaitOrder[i];
			break;
		}
	}

    /*----- Display Waited Order# on LCD#3 ? ----*/
    if (puchPtr && uchOrder1 != 0) {
		FDTPARA     FDTPara = { 0 };

        /*----- Check Counter(Store) Terminal ----*/
        FDTParameter(&FDTPara);
        if (FDTPara.uchTypeTerm != FX_DRIVE_COUNTER_STORE
            && FDTPara.uchTypeTerm != FX_DRIVE_COUNTER_FUL_STORE) {
            FDTRemoveWaitOrder(uchOrder1);
    
            /*----- Case of Trans Not Display on LCD#3 ----*/
            if (FDTLocal.auchWaitOrder[0] == 0) {
                sFlag = 1;
            }
        }
    }

    /*----- Display Transaction on LCD#1 -----*/
    if (uchOrder1 != 0) {
		MldDisplayInit(MLD_DRIVE_1, 0);
        MldPutGcfToScroll(MLD_SCROLL_1, TrnInformationPtr->hsTranConsStorage);
        MldDispSubTotal(MLD_TOTAL_1, TranItemizersPtr->lMI);
    } else {
        MldDisplayInit(MLD_DRIVE_1, 0);
    }

    /*----- Display Transaction on LCD #3 -----*/
    if (uchOrder1 != 0) {
        if (FDTLocal.auchWaitOrder[0] == 0) {
            if (sFlag == 1) {
                if (FDTLocal.auchOrder[1] != 0) {
                    if (FDTLocal.auchOrder[1] == FDTLocal.uchOrderNo1) {
                        MldDisplayInit(MLD_DRIVE_3, 0);
                        MldPutGcfToScroll(MLD_SCROLL_3, FDTLocal.husTranHandle1);
                        FDTDispTotal(MLD_TOTAL_3, 1);
                    } else {
                        MldDisplayInit(MLD_DRIVE_3, 0);
                        MldPutGcfToScroll(MLD_SCROLL_3, FDTLocal.husTranHandle2);
                        FDTDispTotal(MLD_TOTAL_3, 2);
                    }
                } else {
                    MldDisplayInit(MLD_DRIVE_3, 0);
                }
            }

        /*----- Display Waited Order# on LCD #3 -----*/
        } else {
			FDTWAITORDER    WorkWait = {0};
            /*----- Copy Waited Order# -----*/
            memcpy(WorkWait.uchOrderNo, FDTLocal.auchWaitOrder, sizeof(WorkWait.uchOrderNo));

            /*----- Display Order# to LCD #3 -----*/
            MldDispWaitKey(&WorkWait);
        }

        /*----- Save Current Order# -----*/
        FDTLocal.uchGCFOrder = uchOrder1;
    }

    FDTLocal.uchPrevPayTrans = 0;

    PifReleaseSem(husFDTFileSem);

    return(sStatus);
}

/*
*===========================================================================
** Synopsis:    VOID    FDTStore(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Save Order# to FDT Local for Forward/Backward Operation.
*===========================================================================
*/
VOID    FDTStore(VOID)
{
    PifRequestSem(husFDTFileSem);

    FDTLocal.uchOwnOrder = 0;
    FDTLocal.uchGCFOrder = 0;
    FDTLocal.uchPrevPayTrans = 0;

    PifReleaseSem(husFDTFileSem);
}

/*
*===========================================================================
** Synopsis:    VOID    FDTPayment(UCHAR uchOrderNo)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Save Order# to FDT Local for Paid Order Recall Operation.
*===========================================================================
*/
VOID    FDTPayment(GCNUM uchOrderNo)
{
	FDTPARA     FDTPara = { 0 };

    PifRequestSem(husFDTFileSem);

    FDTParameter(&FDTPara);

    switch (FDTPara.uchTypeTerm) {
    case    FX_DRIVE_ORDER_PAYMENT_TERM_1:
    case    FX_DRIVE_PAYMENT_TERM_1:
    case    FX_DRIVE_ORDER_PAYMENT_TERM_2:
    case    FX_DRIVE_PAYMENT_TERM_2:
    case    FX_DRIVE_DELIV_1:
    case    FX_DRIVE_DELIV_2:
    case    FX_DRIVE_ORDER_TERM_1:
    case    FX_DRIVE_ORDER_TERM_2:
    case    FX_DRIVE_COUNTER_STORE:
    case    FX_DRIVE_COUNTER_STORE_PAY:
    case    FX_DRIVE_ORDER_FUL_TERM_1:
    case    FX_DRIVE_ORDER_FUL_TERM_2:
    case    FX_DRIVE_COUNTER_FUL_STORE:
    case    FX_DRIVE_COUNTER_FUL_STORE_PAY:
        /*----- Shift Order# -----*/
        FDT_memmove(&FDTLocal.auchTransQueue[1], &FDTLocal.auchTransQueue[0], FDT_QUEUE_NUM_ITEMS);

        /*----- Insert Latest Order# -----*/
        FDTLocal.auchTransQueue[0] = uchOrderNo;
        FDTLocal.uchPrevPayTrans = uchOrderNo;
        break;

    default:
        break;
    }

    FDTLocal.uchOwnOrder = 0;
    FDTLocal.uchGCFOrder = 0;

    PifReleaseSem(husFDTFileSem);
}

/*
*===========================================================================
** Synopsis:    VOID    FDTCancel(UCHAR uchOrderNo)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Erase Save Order# from FDT Local for Wait Key.
*===========================================================================
*/
VOID    FDTCancel(VOID)
{
    PifRequestSem(husFDTFileSem);

    FDTLocal.uchOwnOrder = 0;
    FDTLocal.uchGCFOrder = 0;
    FDTLocal.uchPrevPayTrans = 0;

    PifReleaseSem(husFDTFileSem);
}

/*
*===========================================================================
** Synopsis:    VOID    FDTLeftArrow(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Move Cursor to Left LCD.
*===========================================================================
*/
VOID    FDTLeftArrow(VOID)
{
    /*----- Display Cursor -----*/
    MldMoveCursor(1);
}

/*
*===========================================================================
** Synopsis:    VOID    FDTRightArrow(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Move Cursor to Right LCD.
*===========================================================================
*/
VOID    FDTRightArrow(VOID)
{
    /*----- Display Cursor -----*/
    MldMoveCursor(0);
}

/*
*===========================================================================
** Synopsis:    VOID    FDTDownArrow(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Down Display on LCD.
*===========================================================================
*/
VOID    FDTDownArrow(VOID)
{
    /*----- Down Display -----*/
    MldDownCursor();
}

/*
*===========================================================================
** Synopsis:    VOID    FDTUpArrow(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Up Diaplay on LCD.
*===========================================================================
*/
VOID    FDTUpArrow(VOID)
{
    /*----- Up Display -----*/
    MldUpCursor();
}

/*
*===========================================================================
** Synopsis:    UCHAR   FDTGetStoreQueue(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Get Oldest Order# from Store/Payment Queue.
*===========================================================================
*/
USHORT   FDTGetStoreQueue(VOID)
{
    SHORT        sLoopCnt, sStatus;
	FDTPARA      FDTPara = { 0 };

    PifRequestSem(husFDTFileSem);

    FDTParameter(&FDTPara);

    for (sLoopCnt = FDT_QUEUE_SIZE - 1; sLoopCnt >= 0; sLoopCnt--) {
        if (FDTLocal.auchStorePayQueue[sLoopCnt] != 0) {

            /*----- Counter(Store) Terminal -----*/
            if (FDTPara.uchTypeTerm == FX_DRIVE_COUNTER_STORE
                || FDTPara.uchTypeTerm == FX_DRIVE_COUNTER_FUL_STORE) {
                PifReleaseSem(husFDTFileSem);
                return(FDTLocal.auchStorePayQueue[sLoopCnt]);

            /*----- Counter(Store/Payment) Terminal -----*/
            } else {
				TRANGCFQUAL  WorkGCF;

                sStatus = CliGusIndRead(FDTLocal.auchStorePayQueue[sLoopCnt],
                    (UCHAR *)&WorkGCF, sizeof(TRANGCFQUAL));

                if (sStatus == GCF_NOT_IN) {
                    continue;
                }
                if (sStatus <= 0) {
                    PifReleaseSem(husFDTFileSem);
                    return(0);
                } else {                    
                    if (WorkGCF.fsGCFStatus2 & GCFQUAL_PAYMENT_TRANS) {
                        continue;
                    } else {
                        PifReleaseSem(husFDTFileSem);
                        return(FDTLocal.auchStorePayQueue[sLoopCnt]);
                    }
                }
            }
        }
    }

    PifReleaseSem(husFDTFileSem);

    return(0);
}

/*
*===========================================================================
** Synopsis:    VOID    FDTGetStatus(FDTSTATUS *FDTLocal)
*     Input:    nothing
*     Output:   FDTSTATUS *pData
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Make a copy of the Flexible Drive Thru status
*               data needed by the display logic
*===========================================================================
*/
VOID    FDTGetStatus(FDTSTATUS *pData)
{
	pData->uchOwnOrder = FDTLocal.uchOwnOrder;				/* Paid Order Number (Without Amount) */
	pData->uchGCFOrder = FDTLocal.uchGCFOrder;				/* Paid Order Number (With Amount) */
	pData->uchPrevPayTrans = FDTLocal.uchPrevPayTrans;		/* Previous Payment Order Number */
	pData->uchAddonOrder = FDTLocal.uchAddonOrder;			/* Addon Transaction Order Number */

}

/*========= End of File =========*/
