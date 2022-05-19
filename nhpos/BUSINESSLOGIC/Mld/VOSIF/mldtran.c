/*
*===========================================================================
* Title       : Multiline Display all items in transaction control
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDTRAN.C
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Apl-05-95 : 03.00.00 : M.Ozawa    : Initial
* Apr-16-99 : 03.05.00 : M.Teraki   : Merge STORE_RECALL_MODEL
*           :          :            :       /GUEST_CHECK_MODEL
* 
*** NCR2171 **
* Aug-26-99 : 01.00.00 : M.Teraki   : initial (for Win32)
* Dec-15-15 : 02.02.01 : R.Chambers : handle usScroll flags usScroll = usScrollIn & MLD_SCROLL_MASK
*===========================================================================
*/

/**
;========================================================================
;+                  I N C L U D E     F I L E s                         +
;========================================================================
**/
/**------- MS - C ---------**/
#include	<tchar.h>
#include <stdlib.h>
#include <string.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <pif.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <regstrct.h>
#include <transact.h>
#include <trans.h>
#include <rfl.h>
#include <prtprty.h>
#include <vos.h>
#include <mld.h>
#include "mldlocal.h"
#include "mldrin.h"
#include "termcfg.h"

/*
============================================================================
+                  S T A T I C   R A M s
============================================================================
*/
extern TCHAR   aszMldOrderNo[];

/**
;========================================================================
;+        P R O G R A M    D E C L A R A T I O N s                      +
;========================================================================
**/

/*
*===========================================================================
** Format  : USHORT   MldPutTransToScroll(USHORT usScroll, USHORT usVliSize,
*                       SHORT sFileHandle, SHORT sIndexHandle);
*               
*    Input : USHORT           usScroll   -destination scroll display
*            USHORT           usVliSize  -vli size of storage file
*            SHORT            sFileHandle -storage file handle
*            SHORT            sIndexHandle -file handle of index
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function displays all transaction data from storage file
*===========================================================================
*/
SHORT   MldPutTransToScroll(MLDTRANSDATA *pData1)
{
    SHORT sStatus;

    /* request semaphore */
    PifRequestSem(husMldSemaphore);

    sStatus = MldLocalPutTransToScroll(pData1->usVliSize, pData1->sFileHandle, pData1->sIndexHandle, MLD_TRANS_DISPLAY);

    /* release semaphore */
    PifReleaseSem(husMldSemaphore);

    return (sStatus);
}

#if POSSIBLE_DEAD_CODE
/*
*===========================================================================
** Format  : USHORT   MldPutTransToScroll2(USHORT usScroll, USHORT usVliSize,
*                       SHORT sFileHandle, SHORT sIndexHandle);
*               
*    Input : USHORT           usScroll   -destination scroll display
*            USHORT           usVliSize  -vli size of storage file
*            SHORT            sFileHandle -storage file handle
*            SHORT            sIndexHandle -file handle of index
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function displays all transaction data from storage file
*            (check wait key status)
*===========================================================================
*/
SHORT   MldPutTransToScroll2(MLDTRANSDATA *pData1)
{
    SHORT sStatus;

    /* request semaphore */
    PifRequestSem(husMldSemaphore);

    if (MldScroll1Buff.uchStatus & MLD_WAIT_STATUS) {
        /* release semaphore */
        PifReleaseSem(husMldSemaphore);
        return (MLD_ALREADY_DISPLAYED);
    }

    sStatus = MldLocalPutTransToScroll(pData1->usVliSize, pData1->sFileHandle, pData1->sIndexHandle, MLD_TRANS_DISPLAY);

    /* release semaphore */
    PifReleaseSem(husMldSemaphore);
    return (sStatus);
}
#endif

/*
*===========================================================================
** Format  : USHORT   MldPutTrnToScrollReverse(USHORT usScroll, USHORT usVliSize,
*                       SHORT sFileHandle, SHORT sIndexHandle);
*               
*    Input : USHORT           usScroll   -destination scroll display
*            USHORT           usVliSize  -vli size of storage file
*            SHORT            sFileHandle -storage file handle
*            SHORT            sIndexHandle -file handle of index
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function displays all transaction data from storage file
*            from last itemize data
*===========================================================================
*/
SHORT   MldPutTrnToScrollReverse(MLDTRANSDATA *pData1)
{
    SHORT sStatus;

    /* request semaphore */
    PifRequestSem(husMldSemaphore);

    sStatus = MldLocalPutTransToScroll(pData1->usVliSize, pData1->sFileHandle, pData1->sIndexHandle, MLD_REVERSE_DISPLAY);

    /* release semaphore */
    PifReleaseSem(husMldSemaphore);

    return (sStatus);
}

#if POSSIBLE_DEAD_CODE
/*
*===========================================================================
** Format  : USHORT   MldPutTrnToScrollReverse2(USHORT usScroll, USHORT usVliSize,
*                       SHORT sFileHandle, SHORT sIndexHandle);
*               
*    Input : USHORT           usScroll   -destination scroll display
*            USHORT           usVliSize  -vli size of storage file
*            SHORT            sFileHandle -storage file handle
*            SHORT            sIndexHandle -file handle of index
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function displays all transaction data from storage file
*            from last itemize data(check wait key status)
*===========================================================================
*/
SHORT   MldPutTrnToScrollReverse2(MLDTRANSDATA *pData1)
{
    SHORT sStatus;

    /* request semaphore */
    PifRequestSem(husMldSemaphore);

    if (MldScroll1Buff.uchStatus & MLD_WAIT_STATUS) {
        /* release semaphore */
        PifReleaseSem(husMldSemaphore);
        return (MLD_ALREADY_DISPLAYED);
    }

    sStatus = MldLocalPutTransToScroll(pData1->usVliSize, pData1->sFileHandle, pData1->sIndexHandle, MLD_REVERSE_DISPLAY);

    /* release semaphore */
    PifReleaseSem(husMldSemaphore);
    return (sStatus);
}
#endif

/*
*===========================================================================
** Format  : USHORT   MldPutSplitTransToScroll(USHORT usScroll, MLDTRANSDATA *pData1);
*               
*    Input : USHORT           usScroll   -destination scroll display
*            USHORT           usVliSize  -vli size of storage file
*            SHORT            sFileHandle -storage file handle
*            SHORT            sIndexHandle -file handle of index
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function displays all transaction data from storage file
*===========================================================================
*/
SHORT   MldPutSplitTransToScroll(USHORT usScroll, MLDTRANSDATA *pData1)
{
    SHORT sStatus;

    /* request semaphore */
    PifRequestSem(husMldSemaphore);

    if ((uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) && (uchMldSystem2 == MLD_SPLIT_CHECK_SYSTEM)) {

        switch(usScroll) {
        case MLD_SCROLL_1:
            /* copy transaction storage file to mld storage file and display */
            sStatus = MldLocalPutTransToScroll(pData1->usVliSize, pData1->sFileHandle, pData1->sIndexHandle, MLD_TRANS_DISPLAY);
            break;

        case MLD_SCROLL_2:
            /* initialize display */
            MldLocalDisplayInit(MLD_SCROLL_2, MldScroll2Buff.usAttrib);

            /* set transaction storage handle to mld buffer */
            MldScroll2Buff.sFileHandle = pData1->sFileHandle;
            MldScroll2Buff.sIndexHandle = pData1->sIndexHandle;
            MldScroll2Buff.usIndexHeadOffset = 0;

            /* display */
            MldDispAllStoredData(usScroll);

            sStatus = MLD_SUCCESS;
            break;

        case MLD_SCROLL_3:
            /* initialize display */
            MldLocalDisplayInit(MLD_SCROLL_3, MldScroll3Buff.usAttrib);

            /* set transaction storage handle to mld buffer */
            MldScroll3Buff.sFileHandle = pData1->sFileHandle;
            MldScroll3Buff.sIndexHandle = pData1->sIndexHandle;
            MldScroll3Buff.usIndexHeadOffset = 0;

            /* display */
            MldDispAllStoredData(usScroll);

            sStatus = MLD_SUCCESS;
            break;
        }
    } else {
        sStatus = MLD_ERROR;
    }

    /* release semaphore */
    PifReleaseSem(husMldSemaphore);

    return (sStatus);
}

#if POSSIBLE_DEAD_CODE
/*
*===========================================================================
** Format  : USHORT   MldPutSplitTransToScroll2(USHORT usScroll, MLDTRANSDATA *pData1);
*               
*    Input : USHORT           usScroll   -destination scroll display
*            USHORT           usVliSize  -vli size of storage file
*            SHORT            sFileHandle -storage file handle
*            SHORT            sIndexHandle -file handle of index
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function displays all transaction data from storage file
*===========================================================================
*/
SHORT   MldPutSplitTransToScroll2(USHORT usScroll, MLDTRANSDATA *pData1)
{
    SHORT sStatus;

    /* request semaphore */
    PifRequestSem(husMldSemaphore);

    if ((uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) && (uchMldSystem2 == MLD_SPLIT_CHECK_SYSTEM)) {
        switch(usScroll) {
        case MLD_SCROLL_1:
            if (MldScroll1Buff.uchStatus & MLD_WAIT_STATUS) {
                sStatus = MLD_ALREADY_DISPLAYED;
            } else {
                /* copy transaction storage file to mld storage file and display */
                sStatus = MldLocalPutTransToScroll(pData1->usVliSize, pData1->sFileHandle, pData1->sIndexHandle, MLD_TRANS_DISPLAY);
            }
            break;

        case MLD_SCROLL_2:
            if (MldScroll2Buff.uchStatus & MLD_WAIT_STATUS) {
                sStatus = MLD_ALREADY_DISPLAYED;
            } else {
                /* initialize display */
                MldLocalDisplayInit(MLD_SCROLL_2, MldScroll2Buff.usAttrib);

                /* set transaction storage handle to mld buffer */
                MldScroll2Buff.sFileHandle = pData1->sFileHandle;
                MldScroll2Buff.sIndexHandle = pData1->sIndexHandle;
                MldScroll2Buff.usIndexHeadOffset = 0;

                /* display */
                MldDispAllStoredData(usScroll);

                sStatus = MLD_SUCCESS;
            }
            break;

        case MLD_SCROLL_3:
            if (MldScroll3Buff.uchStatus & MLD_WAIT_STATUS) {
                sStatus = MLD_ALREADY_DISPLAYED;
            } else {
                /* initialize display */
                MldLocalDisplayInit(MLD_SCROLL_3, MldScroll3Buff.usAttrib);

                /* set transaction storage handle to mld buffer */
                MldScroll3Buff.sFileHandle = pData1->sFileHandle;
                MldScroll3Buff.sIndexHandle = pData1->sIndexHandle;
                MldScroll3Buff.usIndexHeadOffset = 0;

                /* display */
                MldDispAllStoredData(usScroll);

                sStatus = MLD_SUCCESS;
            }
            break;
        }
    } else {
        sStatus = MLD_ERROR;
    }

    /* release semaphore */
    PifReleaseSem(husMldSemaphore);

    return (sStatus);
}
#endif

/*
*===========================================================================
** Format  : USHORT   MldPutGcfToScroll(USHORT usScroll, SHORT sFileHandle);
*               
*    Input : USHORT           usScroll   -destination scroll display
*            SHORT            sFileHandle -guest check file handle
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function displays all transaction data from storage file
*===========================================================================
*/
SHORT   MldPutGcfToScroll(USHORT usScroll, SHORT sFileHandle)
{
    SHORT sStatus;

    /* request semaphore */
    PifRequestSem(husMldSemaphore);

    sStatus = MldLocalPutGcfToScroll(usScroll, sFileHandle);

    /* release semaphore */
    PifReleaseSem(husMldSemaphore);

    return (sStatus);
}

SHORT   MldRedisplayToScroll(USHORT usScroll)
{
    MLDCURTRNSCROL *pMldCurTrnScrol;
    SHORT sStatus = MLD_SUCCESS;

    /* request semaphore */
    PifRequestSem(husMldSemaphore);

    switch (usScroll) {
		case MLD_SCROLL_1:
			pMldCurTrnScrol = &MldScroll1Buff;
			break;

		case MLD_SCROLL_2:
			if (uchMldSystem != MLD_DRIVE_THROUGH_3SCROLL) return(MLD_ERROR);
			pMldCurTrnScrol = &MldScroll2Buff;
			break;

		case MLD_SCROLL_3:
			if (uchMldSystem != MLD_DRIVE_THROUGH_3SCROLL) return(MLD_ERROR);
			pMldCurTrnScrol = &MldScroll3Buff;
			break;

		default:
			sStatus = MLD_ERROR;
    }

    /* if cursor does not displayed on display #1 */
    if (sStatus == MLD_SUCCESS && usScroll == MLD_SCROLL_1) {
        if (uchMldCurScroll != MLD_SCROLL_1) {
            if (MldLocalSetCursor(MLD_SCROLL_1) != MLD_SUCCESS) {
                sStatus = MLD_ERROR;
            }
        }
    }

	if (sStatus == MLD_SUCCESS) {
		/* display from first itemize data */
		VosSetCurType (pMldCurTrnScrol->usWinHandle, VOS_INVISIBLE);
		VosSetCurPos (pMldCurTrnScrol->usWinHandle, 0, 0);
		VosCls (pMldCurTrnScrol->usWinHandle);
        memset (&(pMldCurTrnScrol->uchCurItem), 0x00, MLDSCROLLDISPLAY);
		MldDispAllStoredData(usScroll);
		VosSetCurPos (pMldCurTrnScrol->usWinHandle, 0, 0);

		if (uchMldCurScroll == MLD_SCROLL_1) {
			VosSetCurType (pMldCurTrnScrol->usWinHandle, VOS_VISIBLE);
		}
	}
    /* release semaphore */
    PifReleaseSem(husMldSemaphore);

    return (sStatus);
}

#if POSSIBLE_DEAD_CODE
/*
*===========================================================================
** Format  : USHORT   MldPutGcfToScroll2(USHORT usScroll, SHORT sFileHandle);
*               
*    Input : USHORT           usScroll   -destination scroll display
*            SHORT            sFileHandle -guest check file handle
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function displays all transaction data from storage file
*===========================================================================
*/
SHORT   MldPutGcfToScroll2(USHORT usScroll, SHORT sFileHandle)
{
    SHORT sStatus;
    MLDCURTRNSCROL *pMldCurTrnScrol;

    switch (usScroll) {
		case MLD_SCROLL_1:
			pMldCurTrnScrol = &MldScroll1Buff;
			break;

		case MLD_SCROLL_2:
			pMldCurTrnScrol = &MldScroll2Buff;
			break;

		case MLD_SCROLL_3:
			pMldCurTrnScrol = &MldScroll3Buff;
			break;

		default:
			return(MLD_ERROR);
    }

    /* request semaphore */
    PifRequestSem(husMldSemaphore);

    if (pMldCurTrnScrol->uchStatus & MLD_WAIT_STATUS) {
        /* release semaphore */
        PifReleaseSem(husMldSemaphore);
        return (MLD_ALREADY_DISPLAYED);
    }

    sStatus = MldLocalPutGcfToScroll(usScroll, sFileHandle);

    /* release semaphore */
    PifReleaseSem(husMldSemaphore);
    return (sStatus);
}
#endif

/*
*===========================================================================
** Format  : USHORT   MldPutAllGcfToScroll(USHORT usScroll,
*                                SHORT sFileHandle1);
*                                SHORT sFileHandle2,
*                                SHORT sFileHandle3)
*               
*    Input : USHORT           usScroll   -destination scroll display
*            SHORT            sFileHandle1 -guest check file handle
*            SHORT            sFileHandle2 -guest check file handle
*            SHORT            sFileHandle3 -guest check file handle
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function displays all transaction data from storage file
*===========================================================================
*/
SHORT   MldPutAllGcfToScroll(SHORT sFileHandle1, SHORT sFileHandle2, SHORT sFileHandle3)
{
    SHORT sStatus;

    /* --- display scroll #1 --- */
    sStatus = MldPutGcfToScroll(MLD_SCROLL_1, sFileHandle1);

    if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {
        if (sStatus != MLD_SUCCESS) {
            VosCls(MldOtherHandle1.usOrderHandle);
            VosCls(MldOtherHandle1.usTotalHandle);
        }
    }

    /* --- display scroll #2 --- */
    sStatus = MldPutGcfToScroll(MLD_SCROLL_2, sFileHandle2);

    if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {
        if (sStatus != MLD_SUCCESS) {
            VosCls(MldOtherHandle2.usOrderHandle);
            VosCls(MldOtherHandle2.usTotalHandle);
        }
    }

    /* --- display scroll #3 --- */
    sStatus = MldPutGcfToScroll(MLD_SCROLL_3, sFileHandle3);

    if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {
        if (sStatus != MLD_SUCCESS) {
            VosCls(MldOtherHandle3.usOrderHandle);
            VosCls(MldOtherHandle3.usTotalHandle);
        }
    }

    return (MLD_SUCCESS);
}

#if POSSIBLE_DEAD_CODE
/*
*===========================================================================
** Format  : USHORT   MldPutAllGcfToScroll2(USHORT usScroll,
*                                SHORT sFileHandle1);
*                                SHORT sFileHandle2,
*                                SHORT sFileHandle3)
*               
*    Input : USHORT           usScroll   -destination scroll display
*            SHORT            sFileHandle1 -guest check file handle
*            SHORT            sFileHandle2 -guest check file handle
*            SHORT            sFileHandle3 -guest check file handle
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function displays all transaction data from storage file
*===========================================================================
*/
SHORT   MldPutAllGcfToScroll2(SHORT sFileHandle1, SHORT sFileHandle2, SHORT sFileHandle3)
{
    SHORT sStatus;

    /* --- display scroll #1 --- */
    sStatus = MldPutGcfToScroll2(MLD_SCROLL_1, sFileHandle1);

    if (sStatus == MLD_ALREADY_DISPLAYED) {
        return (MLD_ALREADY_DISPLAYED_1);
    }

    if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {
        if (sStatus != MLD_SUCCESS) {
            VosCls(MldOtherHandle1.usOrderHandle);
            VosCls(MldOtherHandle1.usTotalHandle);
        }
    }

    /* --- display scroll #2 --- */
    sStatus = MldPutGcfToScroll2(MLD_SCROLL_2, sFileHandle2);

    if (sStatus == MLD_ALREADY_DISPLAYED) {
        return (MLD_ALREADY_DISPLAYED_2);
    }

    if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {
        if (sStatus != MLD_SUCCESS) {
            VosCls(MldOtherHandle2.usOrderHandle);
            VosCls(MldOtherHandle2.usTotalHandle);
        }
    }

    /* --- display scroll #3 --- */
    sStatus = MldPutGcfToScroll2(MLD_SCROLL_3, sFileHandle3);

    if (sStatus == MLD_ALREADY_DISPLAYED) {
        return (MLD_ALREADY_DISPLAYED_3);
    }

    if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {
        if (sStatus != MLD_SUCCESS) {
            VosCls(MldOtherHandle3.usOrderHandle);
            VosCls(MldOtherHandle3.usTotalHandle);
        }
    }

    return (MLD_SUCCESS);
}
#endif

/*
*===========================================================================
** Format  : USHORT   MldLocalPutTransToScroll(USHORT usScroll, USHORT usVliSize,
*                       SHORT sFileHandle, SHORT sIndexHandle, USHORT usType);
*               
*    Input : USHORT           usScroll   -destination scroll display
*            USHORT           usVliSize  -vli size of storage file
*            SHORT            sFileHandle -storage file handle
*            SHORT            sIndexHandle -file handle of index
*            USHORT           usType      -reverse or not
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function displays all transaction data from storage file
*===========================================================================
*/
SHORT   MldLocalPutTransToScroll(USHORT usVliSize, SHORT sFileHandle,
                                    SHORT sIndexHandle, USHORT usType)
{
    UCHAR uchSpecialStatus;

    /* save special status contorl bit */
    uchSpecialStatus = MldScroll1Buff.uchSpecialStatus;

    /* initialize display */
    MldLocalDisplayInit(MLD_SCROLL_1, MldScroll1Buff.usAttrib);

    if ((sFileHandle < 0) || (sIndexHandle < 0)) {
        MldScroll1Buff.sFileHandle = -1;
        MldScroll1Buff.sIndexHandle = -1;
        return(MLD_INVALID_HANDLE);
    }

    /* if cursor does not displayed on display #1 */
    if (uchMldCurScroll != MLD_SCROLL_1) {
        if (MldLocalSetCursor(MLD_SCROLL_1) != MLD_SUCCESS) {
            return (MLD_ERROR);
        }
    }

    /* copy transaction storage to mld storage */
    MldScroll1Buff.usStoVli = usVliSize;
    if (MldGetTrnStoToMldSto(usVliSize, sFileHandle, sIndexHandle,
                    MldScroll1Buff.sFileHandle, MldScroll1Buff.sIndexHandle)
                != MLD_SUCCESS) {
        return (MLD_ERROR);
    }

    /* restore special status contorl bit */
    MldScroll1Buff.uchSpecialStatus = uchSpecialStatus;

    if (usType == MLD_REVERSE_DISPLAY) {
        /* display from last itemize data */
        MldDispAllReverseData(MLD_SCROLL_1);
    } else {
        /* display from first itemize data */
        MldDispAllStoredData(MLD_SCROLL_1);
    }

    return (MLD_SUCCESS);
}

/*
*===========================================================================
** Format  : USHORT   MldLocalPutGcfToScroll(USHORT usScroll, SHORT sFileHandle);
*               
*    Input : USHORT           usScroll   -destination scroll display
*            SHORT            sFileHandle -guest check file handle
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function displays all transaction data from storage file
*===========================================================================
*/
SHORT   MldLocalPutGcfToScroll(USHORT usScroll, SHORT sFileHandle)
{
    MLDCURTRNSCROL *pMldCurTrnScrol;

    switch (usScroll) {
		case MLD_SCROLL_1:
			pMldCurTrnScrol = &MldScroll1Buff;
			break;

		case MLD_SCROLL_2:
			if (uchMldSystem != MLD_DRIVE_THROUGH_3SCROLL) return(MLD_ERROR);
			pMldCurTrnScrol = &MldScroll2Buff;
			break;

		case MLD_SCROLL_3:
			if (uchMldSystem != MLD_DRIVE_THROUGH_3SCROLL) return(MLD_ERROR);
			pMldCurTrnScrol = &MldScroll3Buff;
			break;

		default:
			return(MLD_ERROR);
    }

    /* initialize display */
    MldLocalDisplayInit(usScroll, pMldCurTrnScrol->usAttrib);

    if (sFileHandle < 0) {
        pMldCurTrnScrol->sFileHandle = -1;
        pMldCurTrnScrol->sIndexHandle = -1;
        return(MLD_INVALID_HANDLE);
    }

    /* if cursor does not displayed on display #1 */
    if (usScroll == MLD_SCROLL_1) {
        if (uchMldCurScroll != MLD_SCROLL_1) {
            if (MldLocalSetCursor(MLD_SCROLL_1) != MLD_SUCCESS) {
                return (MLD_ERROR);
            }
        }
    }

    if (MldCopyGcfToStorage(usScroll, sFileHandle) != MLD_SUCCESS) {
        return (MLD_ERROR);
    }

    /* display from first itemize data */
    MldDispAllStoredData(usScroll);
    return (MLD_SUCCESS);
}

/*
*===========================================================================
** Format  : ULONG   MldTransactionAttributesFound (USHORT usScroll);
*               
*    Input : USHORT           usScroll   -destination scroll display
*   Output : none
*    InOut : none
*
** Return  : ULONG  
*
** Synopsis: This function provides a way to check for particular settings
*            of a given scroll buffer.
*
*            Bit mask values returned:
*              MLD_TRANS_ATTRIB_ORDERDEC - an MLD_ORDER_DECLARATION has been processed
*===========================================================================
*/

ULONG  MldTransactionAttributesFound (USHORT usScroll)
{
    MLDCURTRNSCROL *pMldCurTrnScrol;
	ULONG  ulBitMask = 0;

    switch (usScroll) {
		case MLD_SCROLL_1:
			pMldCurTrnScrol = &MldScroll1Buff;
			break;

		case MLD_SCROLL_2:
			pMldCurTrnScrol = &MldScroll2Buff;
			break;

		default:
			pMldCurTrnScrol = &MldScroll3Buff;
			break;
    }

	if ((pMldCurTrnScrol->uchStatus2 & MLD_ORDERDEC_MADE) != 0) {
		ulBitMask |= MLD_TRANS_ATTRIB_ORDERDEC;
	}

	return ulBitMask;
}

/*
*===========================================================================
** Format  : USHORT   MldDispAllStoredData(USHORT usScroll);
*               
*    Input : USHORT           usScroll   -destination scroll display
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function displays all transaction data from first itemize
*===========================================================================
*/
USHORT MldDispStoredDataFindRegion (USHORT usScroll, USHORT usNumberLines)
{
    MLDCURTRNSCROL *pMldCurTrnScrol;
    PRTIDXHDR      IndexHeader;
    PRTIDX         Index;
    USHORT         usReadOffset, uchDispItem, usFirstLine, usLineCount = 0;
    SHORT          sStatus = MLD_SUCCESS;
	ULONG          ulActualBytesRead;//RPH SR# 201

    switch (usScroll) {
		case MLD_SCROLL_1:
			pMldCurTrnScrol = &MldScroll1Buff;
			break;

		case MLD_SCROLL_2:
			pMldCurTrnScrol = &MldScroll2Buff;
			break;

		default:
			pMldCurTrnScrol = &MldScroll3Buff;
			break;
    }

    /* read index file header */
    //RPH SR# 201
	MldReadFile(pMldCurTrnScrol->usIndexHeadOffset, &IndexHeader, sizeof( PRTIDXHDR ), pMldCurTrnScrol->sIndexHandle, &ulActualBytesRead);

	usFirstLine = 1;
	usLineCount = 0;
    for (uchDispItem = 1; uchDispItem <= IndexHeader.uchNoOfItem; uchDispItem++)
	{
        /* read index from index file */
		usReadOffset = pMldCurTrnScrol->usIndexHeadOffset + (sizeof(PRTIDXHDR) + (sizeof(PRTIDX) * (uchDispItem - (UCHAR)1)));

        //RPH SR# 201
		MldReadFile (usReadOffset, &Index, sizeof(PRTIDX), pMldCurTrnScrol->sIndexHandle, &ulActualBytesRead );

		if (Index.uchMajorClass == CLASS_ITEMSALES) {
			usLineCount++;
			if (usLineCount > usNumberLines)
				usFirstLine++;
		}
	}

	return usFirstLine;
}


SHORT MldDispStoredDataRegion(USHORT usScrollIn, USHORT usFirstLine)
{
    MLDCURTRNSCROL *pMldCurTrnScrol;
    PRTIDXHDR  IndexHeader;
    PRTIDX Index;
    UCHAR  auchTempStoWork[sizeof(ITEMSALESDISC)];
    CHAR   achBuff[sizeof(ITEMDATASIZEUNION)];
    USHORT usReadOffset, usReadLen;
    SHORT  sStatus = MLD_SUCCESS;
    UCHAR  uchPrtMdc,uchOrderDec = FALSE;
	ULONG	ulActualBytesRead;//RPH SR# 201
	USHORT  usScroll = usScrollIn & MLD_SCROLL_MASK;

    switch (usScroll) {
		case MLD_SCROLL_1:
			pMldCurTrnScrol = &MldScroll1Buff;
			break;

		case MLD_SCROLL_2:
			pMldCurTrnScrol = &MldScroll2Buff;
			break;

		default:
			pMldCurTrnScrol = &MldScroll3Buff;
			break;
    }

    if (uchMldCurScroll == (UCHAR)usScroll) {
        VosSetCurType(pMldCurTrnScrol->usWinHandle, VOS_INVISIBLE);
    }

    /* read print priority mdc for item discount control */
    uchPrtMdc = CliParaMDCCheck(MDC_PRINTPRI_ADR, EVEN_MDC_BIT0);

    /* read index file header */
    //RPH SR# 201
	MldReadFile(pMldCurTrnScrol->usIndexHeadOffset, &IndexHeader, sizeof( PRTIDXHDR ), pMldCurTrnScrol->sIndexHandle, &ulActualBytesRead);

    for (pMldCurTrnScrol->uchDispItem = usFirstLine; pMldCurTrnScrol->uchDispItem <= IndexHeader.uchNoOfItem; pMldCurTrnScrol->uchDispItem++)
	{

        /* read index from index file */
		usReadOffset = pMldCurTrnScrol->usIndexHeadOffset + (sizeof(PRTIDXHDR) + (sizeof(PRTIDX) * (pMldCurTrnScrol->uchDispItem - 1)));

        //RPH SR# 201
		MldReadFile (usReadOffset, &Index, sizeof(PRTIDX), pMldCurTrnScrol->sIndexHandle, &ulActualBytesRead );

        /* read itemize data which directed by index file offset */
		if (Index.usItemOffset < 2)
			continue;

        //RPH SR# 201
		MldReadFile (Index.usItemOffset, auchTempStoWork, sizeof(auchTempStoWork), pMldCurTrnScrol->sFileHandle, &ulActualBytesRead);

        /* decompress itemize data */

        memset (&achBuff, 0, sizeof(achBuff));
        usReadLen = RflGetStorageItem (achBuff, auchTempStoWork, RFL_WITH_MNEM);

        /* ---- skip item discount without parent plu */
        if (( achBuff [0] == CLASS_ITEMDISC ) &&
            ( achBuff [1] == CLASS_ITEMDISC1 )) {
            continue;
        }

		// skip total items to eliminate showing old total lines
        if (achBuff [0] == CLASS_ITEMTOTAL) {
            continue;
        }

        if (achBuff [0] == CLASS_ITEMSALES)
		{
			// Order Declaration is a type of CLASS_ITEMSALES so
			// lets see if this is an Order Declaration.  If so then
			// process it separately.
			if (achBuff [1] == CLASS_ITEMORDERDEC)
			{
				if (uchOrderDec == 0)
				{
					uchOrderDec = TRUE;
					MldOrderDecSearch ((VOID *)achBuff);
					// Since this is an order declare, we will set the
					// bit indicating that an order declare has been created
					// so that future changes will just change the existing
					// order declare line rather than create a new one.
					pMldCurTrnScrol->uchStatus2 |= MLD_ORDERDEC_MADE;
					sStatus = MldLocalScrollWrite (achBuff, MLD_TRANS_DISPLAY, usScroll);
				}
				continue;
			}

			{
				TRANSTORAGESALESNON * pNoCompNormal;
				pNoCompNormal = ( TRANSTORAGESALESNON * )( achBuff);
				if (pNoCompNormal->fbReduceStatus & REDUCE_ITEM)
					continue;
			}

            /* ---- set item qty/price by index data ---- */
            MldSetItemSalesQtyPrice ( (ITEMSALES *)achBuff, &Index);

            MldSetSalesCouponQTY ((ITEMSALES *)achBuff, pMldCurTrnScrol->sFileHandle, usScroll);

            /* ---- display check ---- */
            if (MldChkDisplayPluNo ((ITEMSALES *)achBuff ) == FALSE)
			{
                continue;
            }
        }

        /* ---- display item discount with noun plu ---- */
        if ( MldIsSalesIDisc (achBuff))
		{
            /* create format and display on the scroll area */
            sStatus = MldLocalScrollWrite(achBuff, MLD_TRANS_DISPLAY, usScroll);

            if ((sStatus == MLD_STOP_SCROLL) || (sStatus == MLD_ERROR))
			{
                break;
            }

            /* skip item discount index counter, if not priority */
            if (uchPrtMdc == 0)
			{
                pMldCurTrnScrol->uchDispItem++;
            }

            /* decompress itemize data */
            memset(&achBuff, 0, sizeof(achBuff));
            RflGetStorageItem(achBuff, auchTempStoWork+usReadLen, RFL_WITH_MNEM);

            /* create format and display on the scroll area */
            sStatus = MldLocalScrollWrite(achBuff, MLD_TRANS_DISPLAY2, usScroll);
        }
		else
		{
            /* create format and display on the scroll area */
            sStatus = MldLocalScrollWrite(achBuff, MLD_TRANS_DISPLAY, usScroll);
        }

        if ((sStatus == MLD_STOP_SCROLL) || (sStatus == MLD_ERROR))
		{
            break;
        }
    }

    VosSetCurPos (pMldCurTrnScrol->usWinHandle, pMldCurTrnScrol->uchCurCursor, 0);

	if (pMldCurTrnScrol->uchDispItem <= IndexHeader.uchNoOfItem) {
		MldDispContinue (usScroll, MLD_LOWER_ARROW);
	}

	if (uchMldCurScroll == (UCHAR)usScroll) {
        VosSetCurType (pMldCurTrnScrol->usWinHandle, VOS_VISIBLE);
    }

    pMldCurTrnScrol->uchCurItem = IndexHeader.uchNoOfItem;

    pMldCurTrnScrol->uchStatus2 &= ~MLD_UNMATCH_DISPLAY;   /* clear unmatch status to storage */

    return (MLD_SUCCESS);
}


SHORT MldDispAllStoredData(USHORT usScroll)
{
    MldClearContinue(usScroll, MLD_UPPER_ARROW);
    MldClearContinue(usScroll, MLD_LOWER_ARROW);

	return MldDispStoredDataRegion(usScroll, 1);
}

SHORT MldDispLastStoredData(USHORT usScroll)
{
	USHORT  usFirstLine = MldDispStoredDataFindRegion (usScroll, 5);
	return MldDispStoredDataRegion(usScroll, usFirstLine);
}


/*
*===========================================================================
** Format  : USHORT   MldDispAllReverseData(USHORT usScroll);
*               
*    Input : USHORT           usScroll   -destination scroll display
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function displays all transaction data from last itemize
*===========================================================================
*/
SHORT MldDispReverseDataRegion (USHORT usScrollIn, USHORT usLastLine)
{
    MLDCURTRNSCROL *pMldCurTrnScrol;
    PRTIDXHDR  IndexHeader;
    PRTIDX Index;
    UCHAR  auchTempStoWork[sizeof(ITEMSALESDISC)];
    CHAR   achBuff[sizeof(ITEMDATASIZEUNION)];
    USHORT usReadOffset, usReadLen;
    SHORT  sStatus = MLD_SUCCESS;
	ULONG	ulActualBytesRead; //RPH SR# 201
	UCHAR	 uchOrderDec = FALSE;
	USHORT   usScroll = usScrollIn & MLD_SCROLL_MASK;

    switch (usScroll) {
		case MLD_SCROLL_1:
			pMldCurTrnScrol = &MldScroll1Buff;
			break;

		case MLD_SCROLL_2:
			pMldCurTrnScrol = &MldScroll2Buff;
			break;

		default:
			pMldCurTrnScrol = &MldScroll3Buff;
			break;
    }

    if (uchMldCurScroll == (UCHAR)usScroll) {
        VosSetCurType (pMldCurTrnScrol->usWinHandle, VOS_INVISIBLE);
    }

    /* read print priority mdc for item discount control */
/*    uchPrtMdc = CliParaMDCCheck(MDC_PRINTPRI_ADR, EVEN_MDC_BIT0); */

    /* read index file header */
    //RPH SR# 201
	MldReadFile (pMldCurTrnScrol->usIndexHeadOffset, &IndexHeader, sizeof( PRTIDXHDR ), pMldCurTrnScrol->sIndexHandle, &ulActualBytesRead );

	if (usLastLine < 1)
		usLastLine = IndexHeader.uchNoOfItem;

    for (pMldCurTrnScrol->uchDispItem = usLastLine; pMldCurTrnScrol->uchDispItem > 0; pMldCurTrnScrol->uchDispItem--)
	{
        /* read index from index file */
        usReadOffset = pMldCurTrnScrol->usIndexHeadOffset + ( sizeof(PRTIDXHDR) + ( sizeof(PRTIDX) * (pMldCurTrnScrol->uchDispItem - 1)));

        //RPH SR# 201
		MldReadFile (usReadOffset, &Index, sizeof( PRTIDX ), pMldCurTrnScrol->sIndexHandle, &ulActualBytesRead );

        /* read itemize data which directed by index file offset */
		if (Index.usItemOffset < 2)
			continue;

        //RPH SR# 201
		MldReadFile (Index.usItemOffset, auchTempStoWork, sizeof(auchTempStoWork), pMldCurTrnScrol->sFileHandle, &ulActualBytesRead);

        /* decompress itemize data */
        memset (&achBuff, 0, sizeof(achBuff));
        usReadLen = RflGetStorageItem (achBuff, auchTempStoWork, RFL_WITH_MNEM);

        /* ---- skip item discount without parent plu */
        if ((achBuff [0] == CLASS_ITEMDISC ) && (achBuff [1] == CLASS_ITEMDISC1 )) {
            continue;
        }

		// skip total items to eliminate showing old total lines
        if (achBuff [0] == CLASS_ITEMTOTAL) {
            continue;
        }

        /* ---- display check ---- */
        if ( achBuff [0] == CLASS_ITEMSALES ) {
			// Order Declaration is a type of CLASS_ITEMSALES so
			// lets see if this is an Order Declaration.  If so then
			// process it separately.
			if (achBuff [1] == CLASS_ITEMORDERDEC)
			{
				if (uchOrderDec == 0)
				{
					uchOrderDec = TRUE;
					MldOrderDecSearch ((VOID *)achBuff);
					// Since this is an order declare, we will set the
					// bit indicating that an order declare has been created
					// so that future changes will just change the existing
					// order declare line rather than create a new one.
					pMldCurTrnScrol->uchStatus2 |= MLD_ORDERDEC_MADE;
					sStatus = MldLocalScrollWrite (achBuff, MLD_REVERSE_DISPLAY, usScroll);
				}
				continue;
			}

			{
				TRANSTORAGESALESNON * pNoCompNormal;
				pNoCompNormal = ( TRANSTORAGESALESNON * )( achBuff);
				if (pNoCompNormal->fbReduceStatus & REDUCE_ITEM)
					continue;
			}

            /* ---- set item qty/price by index data ---- */
            MldSetItemSalesQtyPrice ((ITEMSALES *)achBuff, &Index);
            MldSetSalesCouponQTY ((ITEMSALES *)achBuff, pMldCurTrnScrol->sFileHandle, usScroll);
            if (MldChkDisplayPluNo ((ITEMSALES *)achBuff ) == FALSE) {
                continue;
            }
        }

        /* ---- display item discount with noun plu ---- */
        if ( MldIsSalesIDisc (achBuff)) {
            /* decompress itemize data */
            memset (&achBuff, 0, sizeof(achBuff));
            RflGetStorageItem (achBuff, auchTempStoWork+usReadLen, RFL_WITH_MNEM);

            /* create format and display on the scroll area */
            sStatus = MldLocalScrollWrite (achBuff, MLD_REVERSE_DISPLAY2, usScroll);

            if ((sStatus == MLD_STOP_SCROLL) || (sStatus == MLD_ERROR)) {
                break;
            }

            /* skip item discount index counter, if not priority */
/*** remove counter modification of reverse display, 10/12/98
            if (!uchPrtMdc) {
                pMldCurTrnScrol->uchDispItem--;
            }
***/
            memset (&achBuff, 0, sizeof(achBuff));
            usReadLen = RflGetStorageItem (achBuff, auchTempStoWork, RFL_WITH_MNEM);

            if ( achBuff [0] == CLASS_ITEMSALES ) {
                /* ---- set item qty/price by index data ---- */
                MldSetItemSalesQtyPrice ((ITEMSALES *)achBuff, &Index);
                MldSetSalesCouponQTY ((ITEMSALES *)achBuff, pMldCurTrnScrol->sFileHandle, usScroll);
            }

            /* create format and display on the scroll area */
            sStatus = MldLocalScrollWrite (achBuff, MLD_REVERSE_DISPLAY, usScroll);

        } else {
            /* create format and display on the scroll area */
            sStatus = MldLocalScrollWrite (achBuff, MLD_REVERSE_DISPLAY, usScroll);
        }

        if ((sStatus == MLD_STOP_SCROLL) || (sStatus == MLD_ERROR)) {
            break;
        }
    }

    if (sStatus == MLD_STOP_SCROLL) {
        MldDispContinue (usScroll, MLD_UPPER_ARROW);
    }
    if (uchMldCurScroll == (UCHAR)usScroll) {
        VosSetCurType (pMldCurTrnScrol->usWinHandle, VOS_VISIBLE);
    }

    pMldCurTrnScrol->uchCurItem = IndexHeader.uchNoOfItem;

    pMldCurTrnScrol->uchStatus2 &= ~MLD_UNMATCH_DISPLAY;   /* clear unmatch status to storage */

    return (MLD_SUCCESS);
}

SHORT MldDispAllReverseData(USHORT usScroll)
{
    MldClearContinue(usScroll, MLD_UPPER_ARROW);
    MldClearContinue(usScroll, MLD_LOWER_ARROW);

	return MldDispReverseDataRegion(usScroll, 0);
}

SHORT MldDispFirstReverseData(USHORT usScroll)
{
	USHORT  usFirstLine = MldDispStoredDataFindRegion (usScroll, 5);
	return MldDispReverseDataRegion(usScroll, usFirstLine);
}


/*
*===========================================================================
** Format  : USHORT   MldDispGCFQualifier(USHORT usScroll, TRANGCFQUAL *pData);
*               
*    Input : USHORT     usScroll
*          : TRANGCFQUAL *pData
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function displays order no and subtotal from GCF qualifier
*===========================================================================
*/
SHORT MldDispGCFQualifier(USHORT usScrollIn, TRANGCFQUAL *pData)
{
    TCHAR  szMnemonic[PARA_TRANSMNEMO_LEN + 1];
    ITEMTRANSOPEN ItemTransOpen;
    TRANGCFQUAL *pTranGcfQual;
    UCHAR   uchAttr;
	USHORT  usScroll = usScrollIn & MLD_SCROLL_MASK;

    pTranGcfQual = pData;

    if (pTranGcfQual->fsGCFStatus & GCFQUAL_PVOID) {
        MldSetSpecialStatus(usScroll, MLD_TVOID_STATUS);
    }

    if (pTranGcfQual->fsGCFStatus & GCFQUAL_TRETURN) {
        MldSetSpecialStatus(usScroll, MLD_TVOID_STATUS);
    }

    if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {

        if (pTranGcfQual->fsGCFStatus & GCFQUAL_TRAINING) {
			RflGetTranMnem(szMnemonic, TRN_TRNGGT_ADR);
        } else {
			RflGetTranMnem(szMnemonic, TRN_GCNO_ADR);
        }

        if (usScroll & MLD_DRIVE_1) {
            uchAttr = (UCHAR)(MldOtherHandle1.usOrderAttrib ? MLD_C_WHITE : MLD_C_BLACK);
            VosPrintfAttr(MldOtherHandle1.usOrderHandle, uchAttr, aszMldOrderNo, szMnemonic, pTranGcfQual->usGuestNo);   /* order no */
        }
        if (usScroll & MLD_DRIVE_2) {
            uchAttr = (UCHAR)(MldOtherHandle2.usOrderAttrib ? MLD_C_WHITE : MLD_C_BLACK);
            VosPrintfAttr(MldOtherHandle2.usOrderHandle, uchAttr, aszMldOrderNo, szMnemonic, pTranGcfQual->usGuestNo);   /* order no */
        }
        if (usScroll & MLD_DRIVE_3) {
            uchAttr = (UCHAR)(MldOtherHandle3.usOrderAttrib ? MLD_C_WHITE : MLD_C_BLACK);
            VosPrintfAttr(MldOtherHandle3.usOrderHandle, uchAttr, aszMldOrderNo, szMnemonic, pTranGcfQual->usGuestNo);   /* order no */
        }
    } else if (uchMldSystem == MLD_DRIVE_THROUGH_1SCROLL) {
        /* use precheck scorll #2 display */
        memset(&ItemTransOpen, 0x00, sizeof(ITEMTRANSOPEN));
        ItemTransOpen.uchMajorClass = CLASS_ITEMTRANSOPEN;
        ItemTransOpen.usGuestNo = pTranGcfQual->usGuestNo;
        ItemTransOpen.usTableNo = pTranGcfQual->usTableNo;
        ItemTransOpen.usNoPerson = pTranGcfQual->usNoPerson;
        return (MldPrechkNewChk(&ItemTransOpen, MLD_NEW_ITEMIZE, MLD_SCROLL_2));
    }

    return (MLD_SUCCESS);
}

/*
*===========================================================================
** Format  : VOID MldOrderDecSearch(ITEMSALES *pItem)      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints total key  (slip)
*===========================================================================
*/
SHORT MldOrderDecSearch(VOID *pItem)
{
    SHORT   sType, sRet;

	sType = TrnDetermineStorageType();
	sRet = TrnStoOrderDecSearch((ITEMSALES *)pItem, sType);

	// remove the special mnemonic that is stored in the third array element for safe keeping
	// so that it will not print.
	((ITEMSALES *)pItem)->aszNumber[STD_OD_ASZNUMBER_INDEX][0] = 0;

	return sRet;

}
/***** End Of Source *****/
