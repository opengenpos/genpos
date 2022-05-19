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
*   Program Name       : FDT.C
*  ------------------------------------------------------------------------
*    Abstract           :The provided function names are as follows
*
*  ------------------------------------------------------------------------
*   Update Histories
*   Date     :Ver.Rev.:  NAME     :Description
*   Apr-17-95:        :hkato      : Initial.
*   Dec-27-99:01.00.00:hrkato     : Saratoga
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
#include    "termcfg.h"
#include    "appllog.h"
#include    "paraequ.h"
#include    "para.h"
#include    "regstrct.h"
#include    "transact.h"
#include    "trans.h"
#include    "prt.h"
#include    "storage.h"
#include    "fdt.h"
#include    "nb.h"
#include    "csgcs.h"
#include    "csstbgcf.h"
#include    "csstbfcc.h"
#include    "csstbstb.h"
#include    "csstbpar.h"
#include    "mld.h"
#include    "fdtlocal.h"
#include    "uie.h"
#include    "pifmain.h"

TCHAR       FARCONST  aszFDTFile1[]  = _T("FDTSTOR1");  /* Temp file #1 for LCD display #2 */
TCHAR       FARCONST  aszFDTFile2[]  = _T("FDTSTOR2");  /* Temp file #2 for LCD display #3 */
PifSemHandle    husFDTSyncSem = PIF_SEM_INVALID_HANDLE; /* FDT Event Semaphore */
PifSemHandle    husFDTFileSem = PIF_SEM_INVALID_HANDLE; /* FDT File Semaphore */
FDTLOCAL    FDTLocal;
/*USHORT      usFDTStack[FDT_STACK];                      / Stack size */
/*UCHAR       FARCONST  FDTTHREDNAME[] = "FDT";           / FDT Thread Name */
/*static VOID (THREADENTRY *pFunc)(VOID) = FDTMain; */

/*
*===========================================================================
** Synopsis:    VOID FDTInit(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Initialize the FDT thread by creating the semaphores
*               used by the Flexible Drive Through system.
*===========================================================================
*/
VOID    FDTInit(USHORT usMode)
{
    PARAFLEXMEM     ParaFlexMem;

    husFDTSyncSem = PifCreateSem(0);    /* Create Event Semaphore */
    husFDTFileSem = PifCreateSem(1);    /* Create File Semaphore */

    if (usMode & POWER_UP_CLEAR_TOTAL) {
        ParaFlexMem.uchMajorClass = CLASS_PARAFLEXMEM;
        ParaFlexMem.uchAddress = FLEX_CONSSTORAGE_ADR;
        CliParaRead(&ParaFlexMem);
        CliCreateFile(FLEX_CONSSTORAGE_ADR, ParaFlexMem.ulRecordNumber, 0);
    }
}

/*
*===========================================================================
** Synopsis:    VOID    FDTMain(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Flexible Drive Through Main loop for the FDT Thread which
*               processes Notice Board messages to change the displayed
*               list of transactions.  This loop only changes the display
*               if an operator is signed-in.  See also function FDTNoticeMessage()
*               which is used to set a Notice Board message and to release the
*               thread if it is waiting for a message.
*===========================================================================
*/
VOID    THREADENTRY FDTMain(VOID)
{
    FDTPARA     FDTPara;

    memset(&FDTLocal, 0, sizeof(FDTLOCAL));      /* Initialize */
    FDTLocal.husTranHandle1 = PIF_FILE_INVALID_HANDLE;
    FDTLocal.husTranHandle2 = PIF_FILE_INVALID_HANDLE;

    for (;;) {                                   /* Infinite Loop */

		// The semaphore husFDTSyncSem is used to indicate whether the
		// Notice Board thread or the UICMain thread has an FDT item which
		// needs to be dealt with.
		//
		// When this semaphore is created in the function FDTInit(), it is created
		// with an initial value of zero (0).  This means that the first time
		// through this loop, FDTMain () will wait until some other thread releases
		// the semaphore.
		//
		// This semaphore is primarily released by a call to function FDTNoticeMessage ()
		// which updates the FDT data structures and then releases the semaphore to
		// allow FDTMain () to handle the new FDT state.  FDTMain does not release the
		// semaphore so that when the loop repeats, FDTMain will be halted until someone
		// else releases the semaphore indicating more FDT work to be done.
        PifRequestSem(husFDTSyncSem);

		// The semaphore husFDTFileSem is used to control access to the FDTLocal data structure.
		// We obtain access to the FDTLocal structure and then start doing our work.
        PifRequestSem(husFDTFileSem);

        /*----- Check Reg Mode Sign-in -----*/
        if (FDTLocal.husTranHandle1 < 0 && FDTLocal.husTranHandle2 < 0) {
            PifReleaseSem(husFDTFileSem);
            continue;
        }

        /*
			----- Operate by Terminal Type -----
			We determine what kind of a refresh to do to our display depending
			on the type of Notice Board message we have received as well as
			the type of terminal we are supposed to be as set with AC162,
			Flexible Drive Thru Parameters.
		 */
        FDTParameter(&FDTPara);
        switch (FDTPara.uchTypeTerm) {
        case    FX_DRIVE_ORDER_TERM_1:
            if (FDTLocal.fchNBMessage & NB_REQORDER1) {
                FDTRefreshOrder();
            }
            break;

        case    FX_DRIVE_ORDER_TERM_2:
            if (FDTLocal.fchNBMessage & NB_REQORDER2) {
                FDTRefreshOrder();
            }
            break;

        case    FX_DRIVE_ORDER_PAYMENT_TERM_1:
        case    FX_DRIVE_PAYMENT_TERM_1:
            if (FDTLocal.fchNBMessage & NB_REQPAYMENT1) {
                FDTRefresh(GCF_PAYMENT_QUEUE1, GCF_STORE);
            }
            break;

        case    FX_DRIVE_ORDER_PAYMENT_TERM_2:
        case    FX_DRIVE_PAYMENT_TERM_2:
            if (FDTLocal.fchNBMessage & NB_REQPAYMENT2) {
                FDTRefresh(GCF_PAYMENT_QUEUE2, GCF_STORE);
            }
            break;

        case    FX_DRIVE_DELIV_1:
            if (FDTLocal.fchNBMessage & NB_REQDELIVERY1) {
                if (FDTLocal.uchAddonOrder == 0) {
                    FDTRefresh(GCF_DELIVERY_QUEUE1, GCF_STORE_PAYMENT);
                } else {
                    FDTRefreshDelivery(GCF_DELIVERY_QUEUE1, GCF_STORE_PAYMENT);
                }
            }
            break;

        case    FX_DRIVE_DELIV_2:
            if (FDTLocal.fchNBMessage & NB_REQDELIVERY2) {
                if (FDTLocal.uchAddonOrder == 0) {
                    FDTRefresh(GCF_DELIVERY_QUEUE2, GCF_STORE_PAYMENT);
                } else {
                    FDTRefreshDelivery(GCF_DELIVERY_QUEUE2, GCF_STORE_PAYMENT);
                }
            }
            break;

        case    FX_DRIVE_COUNTER_STORE:
        case    FX_DRIVE_COUNTER_STORE_PAY:
        case    FX_DRIVE_ORDER_FUL_TERM_1:
        case    FX_DRIVE_ORDER_FUL_TERM_2:
        case    FX_DRIVE_COUNTER_FUL_STORE:
        case    FX_DRIVE_COUNTER_FUL_STORE_PAY:
			// These are all either non-flexible drive thru types of terminals
			// or these do not have the secondary windows that need refreshing.
			// Therefore we do not need to do any kind of refresh if we
			// receive a message concerning these types.
			break;

        default:
            break;
        }

        /*----- Clear All Message from NB Thread -----*/
        FDTLocal.fchNBMessage = 0;

        PifReleaseSem(husFDTFileSem);
    }
}


/*
*===========================================================================
** Synopsis:    VOID    FDTNoticeMessage(UCHAR uchMessage)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Receive Message from Notice Board Thread
*===========================================================================
*/
VOID    FDTNoticeMessage(UCHAR uchMessage)
{
    PifRequestSem(husFDTFileSem);
    FDTLocal.fchNBMessage |= uchMessage;
    PifReleaseSem(husFDTFileSem);

    PifReleaseSem(husFDTSyncSem);
}

/*
*==========================================================================
**  Synopsis:   SHORT   FDTOpen(VOID)
*
*      Input:   Nothing
*     Output:   Nothing
*      InOut:   Nothing
*
**    Return:
*
**  Description:    Open Two FDT File (for LCD Display).
*==========================================================================
*/
static VOID FDTCloseExec (VOID)
{
    /*----- Close FDT File #1,2 -----*/
    if (FDTLocal.husTranHandle1 >= 0)
		PifCloseFile(FDTLocal.husTranHandle1);
    if (FDTLocal.husTranHandle2 >= 0)
		PifCloseFile(FDTLocal.husTranHandle2);
    FDTLocal.husTranHandle1 = PIF_FILE_INVALID_HANDLE;
    FDTLocal.husTranHandle2 = PIF_FILE_INVALID_HANDLE;
}

SHORT   FDTOpen(VOID)
{
	ULONG	ulActualBytesRead; //RPH 11-10-3 SR# 201

    PifRequestSem(husFDTFileSem);

	FDTCloseExec();    // ensure that the flexible drive thru files are closed if open

    /*----- Clear FDTLocal -----*/
    FDTLocal.fsFDTStatus = 0;
    memset(FDTLocal.auchOrder, 0, sizeof(FDTLocal.auchOrder));

    FDTLocal.uchOrderNo1 = 0;
    FDTLocal.lMI1 = 0L;
    FDTLocal.usTranSize1 = 0;
    FDTLocal.usTranVli1 = 0;
    FDTLocal.fsGCFStatus1 = 0;

    FDTLocal.uchOrderNo2 = 0;
    FDTLocal.lMI2 = 0L;
    FDTLocal.usTranSize2 = 0;
    FDTLocal.usTranVli2 = 0;
    FDTLocal.fsGCFStatus2 = 0;

    memset(FDTLocal.auchWaitOrder, 0, sizeof(FDTLocal.auchWaitOrder));
    memset(FDTLocal.auchTransQueue, 0, sizeof(FDTLocal.auchTransQueue));
    memset(FDTLocal.auchStorePayQueue, 0, sizeof(FDTLocal.auchStorePayQueue));
    FDTLocal.uchOwnOrder = 0;
    FDTLocal.uchGCFOrder = 0;
    FDTLocal.uchPrevPayTrans = 0;
    FDTLocal.uchAddonOrder = 0;
    FDTLocal.husTranHandle1 = PIF_FILE_INVALID_HANDLE;
    FDTLocal.husTranHandle2 = PIF_FILE_INVALID_HANDLE;

    /*----- Open FDT File #1 -----*/
    if ((FDTLocal.husTranHandle1 = PifOpenFile(aszFDTFile1, auchTEMP_OLD_FILE_READ_WRITE)) < 0) {  /* saratoga */
        PifReleaseSem(husFDTFileSem);
        return(LDT_PROHBT_ADR);
    }
	FDTReadFile(0, &FDTLocal.usTranSize1, sizeof(FDTLocal.usTranSize1), FDTLocal.husTranHandle1, &ulActualBytesRead);

    /*----- Open FDT File #2 -----*/
    if ((FDTLocal.husTranHandle2 = PifOpenFile(aszFDTFile2, auchTEMP_OLD_FILE_READ_WRITE)) < 0) {  /* saratoga */
        PifCloseFile(FDTLocal.husTranHandle1);
        FDTLocal.husTranHandle1 = PIF_FILE_INVALID_HANDLE;
        PifReleaseSem(husFDTFileSem);
        return(LDT_PROHBT_ADR);
    }
    //RPH 11-10-3 SR# 201
	FDTReadFile(0, &FDTLocal.usTranSize2, sizeof(FDTLocal.usTranSize2), FDTLocal.husTranHandle2, &ulActualBytesRead);

    /*----- Exist Message from NB ? -----*/
    if (FDTLocal.fchNBMessage != 0) {
        PifReleaseSem(husFDTFileSem);
        PifReleaseSem(husFDTSyncSem);
    } else {
        PifReleaseSem(husFDTFileSem);
        FDTNoticeMessage(NB_REQPAYMENT1 | NB_REQPAYMENT2 | NB_REQDELIVERY1 | NB_REQDELIVERY2 | NB_REQORDER1 | NB_REQORDER2);
    }

    return(FDT_SUCCESS);
}

/*
*==========================================================================
**    Synopsis: VOID    FDTClose(VOID)
*
*       Input:  Nothing
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :  Nothing
*
**  Description:    Close FDT File.
*==========================================================================
*/
VOID    FDTClose(VOID)
{
    PifRequestSem(husFDTFileSem);
	FDTCloseExec();
    PifReleaseSem(husFDTFileSem);
}

/*
*===========================================================================
** Synopsis:    SHORT   FDTRefresh(USHORT usQueue, USHORT usType)
*     Input:    USHORT  usQueue(Target Queue#), USHORT usType(Target GCF Type)
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Refresh Display by Store/Payment.
*===========================================================================
*/
SHORT   FDTRefresh(USHORT usQueue, USHORT usType)
{
    USHORT   uchOrder2, uchOrder3;
	USHORT   usFdtOrders[2];
    SHORT   sStatus = GCF_NOT_IN, sReturn = 0;

    sStatus = CliGusRetrieveFirstQueue(usQueue, usType, &usFdtOrders[0]);
    uchOrder2 = usFdtOrders[0];
    uchOrder3 = usFdtOrders[1];

    /*----- Get Order#2,3 Transaction to FDT File Area -----*/
    if (FDTReadTransFromGCF(uchOrder2, uchOrder3, &sReturn) != FDT_SUCCESS) {
        FDTLocal.auchOrder[0] = 0;
        FDTLocal.auchOrder[1] = 0;
        uchOrder2 = uchOrder3 = 0;
        FDTClear(1);
        FDTClear(2);
        return(FDT_ERROR);
    }

    if (uchOrder2 == 0) {
        if (uchOrder3 == FDTLocal.uchOrderNo1) {
            FDTClear(2);
        } else {
            FDTClear(1);
        }
    }
    if (uchOrder3 == 0) {
        if (uchOrder2 == FDTLocal.uchOrderNo1) {
            FDTClear(2);
        } else {
            FDTClear(1);
        }
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
    }

    /*----- Save Display Order No -----*/
    FDTLocal.auchOrder[0] = uchOrder2;
    FDTLocal.auchOrder[1] = uchOrder3;

    return(FDT_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT   FDTRefreshOrder(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Refresh Order Terminal Display by Payment.
*===========================================================================
*/
SHORT   FDTRefreshOrder(VOID)
{
    SHORT   sReturn = 0;

    /*----- Get Order#2,3 Transaction to FDT File Area -----*/
    if (FDTReadTransFromGCFStorePay(FDTLocal.auchOrder[0],
        FDTLocal.auchOrder[1], &sReturn) != FDT_SUCCESS) {
        FDTLocal.auchOrder[0] = 0;
        FDTLocal.auchOrder[1] = 0;
    }

    if (FDTLocal.auchOrder[0] == 0) {
        FDTClear(1);
    }
    if (FDTLocal.auchOrder[1] == 0) {
        FDTClear(2);
    }

    /*----- Re-Display Transaction on LCD #2 -----*/
    if (FDTLocal.auchOrder[0] != 0 && (sReturn & FDT_STATUS_GET_FILE1)) {
        if (FDTLocal.auchOrder[0] == FDTLocal.uchOrderNo1) {
            MldPutGcfToScroll(MLD_SCROLL_2, FDTLocal.husTranHandle1);
            FDTDispTotal(MLD_TOTAL_2, 1);

        } else {
            MldPutGcfToScroll(MLD_SCROLL_2, FDTLocal.husTranHandle2);
            FDTDispTotal(MLD_TOTAL_2, 2);
        }
    }

    /*----- Re-Display Transaction on LCD #3 -----*/
    if (FDTLocal.auchOrder[1] != 0 && (sReturn & FDT_STATUS_GET_FILE2)) {
        if (FDTLocal.auchOrder[1] == FDTLocal.uchOrderNo1) {
            MldPutGcfToScroll(MLD_SCROLL_3, FDTLocal.husTranHandle1);
            FDTDispTotal(MLD_TOTAL_3, 1);

        } else {
            MldPutGcfToScroll(MLD_SCROLL_3, FDTLocal.husTranHandle2);
            FDTDispTotal(MLD_TOTAL_3, 2);
        }
    }

    return(FDT_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT   FDTRefreshDelivery(USHORT usQueue, USHORT usType)
*     Input:    USHORT  usQueue(Target Queue#), USHORT usType(Target GCF Type)
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Refresh Display by Store/Payment.
*===========================================================================
*/
SHORT   FDTRefreshDelivery(USHORT usQueue, USHORT usType)
{
    USHORT   uchOrder3 = 0;
    SHORT   sStatus = GCF_NOT_IN, sReturn = 0;

    /*----- Display LCD#2 -> LCD#3 -----*/
    if (FDTLocal.auchOrder[1] == 0) {
        /*----- Get Next Order# from Queue -----*/
        sStatus = CliGusInformQueue(usQueue, usType, 0, GCF_FORWARD);
        if (sStatus > 0) {
			uchOrder3 = sStatus;

        /*----- Case of LCD All Clear -----*/
        } else {
            uchOrder3 = 0;
        }

    /*----- Update Display -----*/
    } else {
        uchOrder3 = FDTLocal.auchOrder[1];
    }

    /*----- Get Order#3 Transaction to FDT File Area -----*/
    if (uchOrder3 != 0) {
        if (FDTReadTransFromGCFDeliv(FDTLocal.auchOrder[0],
            uchOrder3, &sReturn) != FDT_SUCCESS) {
            FDTLocal.auchOrder[1] = 0;
            uchOrder3 = 0;
            return(FDT_ERROR);
        }
    }

    if (uchOrder3 == 0) {
        if (FDTLocal.auchOrder[0] == FDTLocal.uchOrderNo1) {
            FDTClear(2);
        } else {
            FDTClear(1);
        }
    }

    /*----- Display Transaction on LCD #3 -----*/
    if (FDTLocal.auchWaitOrder[0] == 0) {
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
           MldDisplayInit(MLD_DRIVE_3, 0);
        }
    }

    /*----- Save Display Order No -----*/
    FDTLocal.auchOrder[1] = uchOrder3;

    return(FDT_SUCCESS);
}

/*========= End of File =========*/