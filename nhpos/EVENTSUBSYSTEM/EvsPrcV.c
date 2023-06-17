/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2172     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1999            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Evs
* Category    : Event Sub System Dynamic Link Library, NCR 2172 for Win32
* Program Name: EvsInit.c
* --------------------------------------------------------------------------
* Abstruct:
*
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
** NCR2172 **
* Oct-07-99 : 1.00.00  : K.Iwata    : initial (for 2172)
* Sep-29-15 : 2.02.01  : R.Chambers : added reflection of messages to customer display windows.
*===========================================================================
*===========================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===========================================================================
*/

#include <tchar.h>
#include <memory.h>

#include "ecr.h"
#include "pif.h"
#include "log.h"
#include "vosl.h"
#include "rfl.h"

#include "vostable.h"
#include "termcfg.h"

#include "Evs.H"
#include "EvsL.H"

#ifndef	MAKEWORD
#define MAKEWORD(a, b)	((USHORT)(((UCHAR)(a)) | ((USHORT)((UCHAR)(b))) << 8))
#endif
#ifndef	MAKELONG
#define MAKELONG(a, b)	((LONG)(((USHORT)(a)) | ((ULONG)((USHORT)(b))) << 16))
#endif


/*
*===========================================================================
** Synopsis:    static SHORT VOS_DATASRC_ID(pVW);
*
*     Input:    pVW       - VOSWINDOW
*
** Return:      DATA ID of aVosMem2Info
*
*===========================================================================
*/
static SHORT VOS_DATASRC_ID(VOID *pVW){
    int  idx;
    idx = ((VOSWINDOW *)pVW)->usMemInfo;

    __wEvsASSERT((0 <= idx && idx < VOS_NUM_MEMINFO),"VOS_DATASRC_ID","");

    /* CAUTION!!! ---> (idx - 1) see vosmem2.c(VosMemAlloc()) */
    return (SHORT)aVosMem2Info[idx - 1].usID;
}

/*
*===========================================================================
** Synopsis:    SHORT LookupShadowDataId (SHORT  dataId)
*     Input:    SHORT dataId
*
** Return:      SHORT  dataIdShadow
*
** Description: This function performs a lookup against a table of window handles
*               and looks to see if there is a corresponding shadow window.  A
*               shadow window is a window which shadows what is displayed in its
*               companion window displaying what appears in the companion window.
*               The shadow or reflection functionality is normally used to replicate a
*               copy of some window to a second monitor for the purpose of providing
*               a customer facing replication of the window as a customer display.
*===========================================================================
*/
static SHORT LookupShadowDataId (SHORT  dataId)
{
	SHORT  dataIdShadow = 0;

	switch (dataId) {
		case LCDWIN_ID_REG100:          // lookup CWindowText::TypeSnglReceiptMain return CWindowText::TypeSnglReceiptCustDisplay
			dataIdShadow = LCDWIN_ID_REG110;      // LookupShadowDataId()
			break;
		case LCDWIN_ID_COMMON001:       // lookup CWindowText::TypeLeadThru return CWindowText::TypeLeadThruCustDisplay
			dataIdShadow = LCDWIN_ID_COMMON004;   // LookupShadowDataId()
			break;
		case LCDWIN_ID_REG102:          // lookup CWindowText::TypeOEP return CWindowText::TypeOEPCustDisplay
			dataIdShadow = LCDWIN_ID_REG104;   // LookupShadowDataId()
			break;
	}

	return dataIdShadow;
}

/*
*===========================================================================
** Synopsis:    LONG    __vEvsGetData(LONG lDataTblIdx,VOID *pData,LONG Len);
*
*     Input:    lDataTblIdx - INDEX of DataSourceTable
*               pData       - 
*               
*
** Return:      size of VOSWINDOW or ZERO
*
*===========================================================================
*/
LONG    __vEvsGetData (LONG lDataTblIdx, VOID *pData, LONG Len){
    LONG       lsize = 0L;
    int        vosmem2Idx = 0;
    VOSWINDOW *pVosWin = NULL;

    /*  */
    if(DataSourceTable[lDataTblIdx].pData == NULL)
        return 0L;

    __wEvsASSERT((int)(DataSourceTable[lDataTblIdx].sType == EVS_TYPE_DISPDATA ||
        DataSourceTable[lDataTblIdx].sType == EVS_TYPE_SCREENMODE),"__vEvsGetData","");

    /* copy data */
    switch(DataSourceTable[lDataTblIdx].sType){
        /* not created */
        case EVS_TYPE_UNKNOWN:
            lsize = 0L;
            break;

        /* display data */
        case EVS_TYPE_DISPDATA:
            /*lsize = DataSourceTable[lDataSrcIdx].pData->PhySize.usRow * DataSourceTable[lDataSrcIdx].pData->PhySize.usCol * 2;*/
            pVosWin = (VOSWINDOW *)(DataSourceTable[lDataTblIdx].pData);
            vosmem2Idx = (int)(pVosWin->usMemInfo - 1);
            lsize = (LONG)aVosMem2Info[vosmem2Idx].usBytes;
            if (Len >= lsize){
                _tcsncpy(pData, pVosWin->puchBuf, lsize);
            }
            else{
                lsize = 0L;
            }
            break;

        /* screenmode & others */
        case EVS_TYPE_SCREENMODE:
            if(Len >= (LONG)sizeof(SHORT)){
                (*(SHORT *)pData) = (*(SHORT *)DataSourceTable[lDataTblIdx].pData);
                lsize = (LONG)sizeof(SHORT);
            }
            break;

		case EVS_TYPE_DISPIMAGE:
			lsize = 1;
			break;
    }
    return lsize;
}


/*
*===========================================================================
** Synopsis:    LONG    __vEvsGetVosWin(LONG lDataTblIdx,VOID *pVosWin,
*                       VOID *pUserDataBuf)
*
*     Input:    lDataTblIdx   - DataSourceTable INDEX
*               pVosWin       - VOSWINDOW
*               pUserDataBuf  - users buffer
*
** Return:      size of VOSWINDOW or ZERO
*
*===========================================================================
*/
LONG    __vEvsGetVosWin(LONG lDataTblIdx,VOID *pVosWin,VOID *pUserDataBuf){

    /* NULL check */
    if(DataSourceTable[lDataTblIdx].pData == NULL)
        return 0L;

    if(DataSourceTable[lDataTblIdx].sType == EVS_TYPE_DISPDATA){
        memcpy (pVosWin, DataSourceTable[lDataTblIdx].pData, sizeof(VOSWINDOW));
        if(pUserDataBuf != NULL)
            ((VOSWINDOW*)pVosWin)->puchBuf = (TCHAR *)pUserDataBuf;
        else
            ((VOSWINDOW*)pVosWin)->puchBuf = (TCHAR *)NULL;
        return sizeof(VOSWINDOW);
    }
    else{
        return 0L;
    }
}

VOID __vEvsGetParam_VOSWINDOW (VOID *pVosWin, ULONG wParam, VOID *lParam) /* used to get button dimensions from Framework - CSMALL */
{
    LONG   dataTblIdx;
    SHORT dataId;

    dataId = VOS_DATASRC_ID(pVosWin);
    dataTblIdx = __EvsSearchDataSrc_ID(dataId);

    __wEvsASSERT((dataTblIdx >= 0),"__vEvsGetParam_VOSWINDOW","");
    __wEvsASSERT((DataSourceTable[dataTblIdx].pData != NULL),"__vEvsGetParam_VOSWINDOW","");

    /* register pointer(data) */
    DataSourceTable[dataTblIdx].pData = (VOID *)pVosWin;

    /* send mesage to FrameWork */
    __wEvsSendMessage(WU_EVS_GETPARAM, dataTblIdx, wParam, (ULONG)lParam);
}

VOID __vEvsPutParam_VOSWINDOW (VOID *pVosWin, ULONG wParam, VOID *lParam) /* used to get button dimensions from Framework - CSMALL */
{
    LONG   dataTblIdx;
	LONG   dataTblIdxShadow = 0;
    SHORT  dataId = VOS_DATASRC_ID(pVosWin);
    SHORT  dataIdShadow = 0;

    dataTblIdx = __EvsSearchDataSrc_ID(dataId);
	if (0 != (dataIdShadow = LookupShadowDataId(dataId))) {
		dataTblIdxShadow = __EvsSearchDataSrc_ID(dataIdShadow);
	}

    __wEvsASSERT((dataTblIdx >= 0),"__vEvsGetParam_VOSWINDOW","");
    __wEvsASSERT((DataSourceTable[dataTblIdx].pData != NULL),"__vEvsGetParam_VOSWINDOW","");

    /* register pointer(data) */
    DataSourceTable[dataTblIdx].pData = (VOID *)pVosWin;

    /* send mesage to FrameWork */
    __wEvsPostMessage(WU_EVS_PUTPARAM, dataTblIdx, wParam, (ULONG)lParam);

	if (dataIdShadow) {
		DataSourceTable[dataTblIdxShadow].pData = pVosWin;
		__wEvsPostMessage (WU_EVS_PUTPARAM, dataTblIdxShadow, wParam, (ULONG)lParam);
	}
}



/*
*===========================================================================
** Synopsis:    VOID __vEvsCreateData_VOSWINDOW(VOID *pVosWin);
*
*     Input:    pVosWin       - VOSWINDOW
*
** Return:      nothing
*
*===========================================================================
*/
VOID __vEvsCreateData_VOSWINDOW(VOID *pVosWin)
{
    LONG   dataTblIdx;
	LONG   dataTblIdxShadow = 0;
    SHORT  dataId = VOS_DATASRC_ID(pVosWin);
    SHORT  dataIdShadow = 0;

    dataTblIdx = __EvsSearchDataSrc_ID(dataId);
	if (0 != (dataIdShadow = LookupShadowDataId(dataId))) {
		dataTblIdxShadow = __EvsSearchDataSrc_ID(dataIdShadow);
	}

#ifdef _DEBUG
    switch(dataId){
    case LCDWIN_ID_REG101:
        EVS_DEBUG_NOP
        break;
    case LCDWIN_ID_REG208:
        EVS_DEBUG_NOP
        break;
    case LCDWIN_ID_SUPER300:
        EVS_DEBUG_NOP
        break;
    case LCDWIN_ID_PROG400:
        EVS_DEBUG_NOP
        break;
    }
#endif

    __wEvsASSERT((dataTblIdx >= 0),"__vEvsCreateData_VOSWINDOW","");
    __wEvsASSERT((DataSourceTable[dataTblIdx].pData == NULL),"__vEvsCreateData_VOSWINDOW","");

    /* register pointer(data) */
    DataSourceTable[dataTblIdx].pData = (VOID *)pVosWin;

    /* send mesage to FrameWork */
    __wEvsPostMessage(WU_EVS_CREATE,dataTblIdx,(ULONG)dataId,(ULONG)0);

	if (dataIdShadow) {
		DataSourceTable[dataTblIdxShadow].pData = pVosWin;
		__wEvsPostMessage (WU_EVS_CREATE, dataTblIdxShadow, (ULONG)dataIdShadow, (ULONG)0);
	}

   /* check SCREEN MODE */
   __vEvsUpdateCheck_SCRMODE(EVS_CREATE,pVosWin);
}


/*
*===========================================================================
** Synopsis:    VOID __vEvsDeleteData_VOSWINDOW(VOID *pVosWin);
*
*     Input:    pVosWin       - VOSWINDOW
*
** Return:      nothing
*
*===========================================================================
*/
VOID __vEvsDeleteData_VOSWINDOW(VOID *pVosWin){
    LONG   dataTblIdx;
	LONG   dataTblIdxShadow = 0;
    SHORT  dataId = VOS_DATASRC_ID(pVosWin);
    SHORT  dataIdShadow = 0;

    dataTblIdx = __EvsSearchDataSrc_ID(dataId);
	if (0 != (dataIdShadow = LookupShadowDataId(dataId))) {
		dataTblIdxShadow = __EvsSearchDataSrc_ID(dataIdShadow);
	}

	__wEvsASSERT((dataTblIdx >= 0),"__vEvsDeleteData_VOSWINDOW","");
    __wEvsASSERT((DataSourceTable[dataTblIdx].pData != NULL),"__vEvsDleteData_VOSWINDOW","data is already EMPTY");

#ifdef _DEBUG
    switch(dataId){
    case LCDWIN_ID_REG101:
        EVS_DEBUG_NOP
        break;
    case LCDWIN_ID_REG208:
        EVS_DEBUG_NOP
        break;
    case LCDWIN_ID_SUPER300:
        EVS_DEBUG_NOP
        break;
    case LCDWIN_ID_PROG400:
        EVS_DEBUG_NOP
        break;
    }
#endif

    DataSourceTable[dataTblIdx].pData = NULL;

    /* send mesage to FrameWork */
    __wEvsPostMessage(WU_EVS_DELETE,dataTblIdx,(ULONG)dataId,(ULONG)0);

	if (dataIdShadow) {
		DataSourceTable[dataTblIdxShadow].pData = pVosWin;
		__wEvsPostMessage (WU_EVS_DELETE, dataTblIdxShadow, (ULONG)dataIdShadow, (ULONG)0);
	}

   /* check SCREEN MODE */
   __vEvsUpdateCheck_SCRMODE(EVS_DELETE,pVosWin);
}


/*
*===========================================================================
** Synopsis:    VOID __vEvsUpdateData_VOSWINDOW(VOID *pVosWin);
*
*     Input:    pVosWin       - VOSWINDOW
*
** Return:      nothing
*
*===========================================================================
*/
VOID __vEvsUpdateData_VOSWINDOW(VOID *pVosWin)
{
    LONG   dataTblIdx;
	LONG   dataTblIdxShadow = 0;
    SHORT  dataId = VOS_DATASRC_ID(pVosWin);
    SHORT  dataIdShadow = 0;

    dataTblIdx = __EvsSearchDataSrc_ID(dataId);
	if (0 != (dataIdShadow = LookupShadowDataId(dataId))) {
		dataTblIdxShadow = __EvsSearchDataSrc_ID(dataIdShadow);
	}

    __wEvsASSERT((dataTblIdx >= 0),"__vEvsUpdateData_VOSWINDOW","");
    //__wEvsASSERT((DataSourceTable[dataTblIdx].pData != NULL),"__vEvsUpdateData_VOSWINDOW","data is empty");

    /* send mesage to FrameWork */
    if(DataSourceTable[dataTblIdx].pData != NULL){
        __wEvsPostMessage(WU_EVS_UPDATE,dataTblIdx,(ULONG)dataId,(ULONG)0);

		if (dataIdShadow) {
			DataSourceTable[dataTblIdxShadow].pData = DataSourceTable[dataTblIdx].pData;
			__wEvsPostMessage (WU_EVS_UPDATE, dataTblIdxShadow, (ULONG)dataIdShadow, (ULONG)0);
		}
    }
}


/*
*===========================================================================
** Synopsis:    VOID    __vEvsUpdateCheck_SCRMODE(EVS_EVENT Ev,VOID *pVosWin);
*
*     Input:    Ev            - event(EVS_CREATE|EVS_DELETE|EVS_UPDATE)
*               pVosWin       - VOSWINDOW
*
** Return:      nothing
*
*===========================================================================
*/
VOID    __vEvsUpdateCheck_SCRMODE(EVS_EVENT Ev,VOID *pVosWin){
    LONG   dataTblIdx,smodeDataTblIdx;
    SHORT  dataId;
    SHORT  cnt;

    __wEvsASSERT((Ev==EVS_CREATE||Ev==EVS_DELETE||Ev==EVS_UPDATE),"__vEvsUpdateCheck_SCRMODE","");

    dataId = VOS_DATASRC_ID(pVosWin);
    dataTblIdx = __EvsSearchDataSrc_ID(dataId);

    /* search Event type & Window ID */
    for(cnt = 0;ScreenModeTable[cnt].Event != EVS_NULL;cnt++){
        if(ScreenModeTable[cnt].Event == Ev && 
            ScreenModeTable[cnt].sDataID == dataId){

            /* set screen Mode */
            __EvsSetScreenMode(cnt);

            /* send mesage to FrameWork */
            smodeDataTblIdx = __EvsSearchDataSrc_ID(SCRMODE_ID);
            __wEvsPostMessage(WU_EVS_UPDATE,smodeDataTblIdx,(ULONG)SCRMODE_ID,(ULONG)__EvsGetScreenMode());
            return;
        }
    }
}


/*
*===========================================================================
** Synopsis:    VOID __vEvsUpdateCursor_VOSWINDOW(VOID *pVosWin);
*
*     Input:    pVosWin       - VOSWINDOW
*
** Return:      nothing
*
*===========================================================================
*/
VOID __vEvsUpdateCursor_VOSWINDOW(VOID *pVosWin)
{
	ULONG  lData;
    VOSWINDOW * pWin;
    LONG   dataTblIdx;
	LONG   dataTblIdxShadow = 0;
    SHORT  dataId = VOS_DATASRC_ID(pVosWin);
    SHORT  dataIdShadow = 0;

    dataTblIdx = __EvsSearchDataSrc_ID(dataId);
	if (0 != (dataIdShadow = LookupShadowDataId(dataId))) {
		dataTblIdxShadow = __EvsSearchDataSrc_ID(dataIdShadow);
	}

    __wEvsASSERT((dataTblIdx >= 0),"__vEvsUpdateData_VOSWINDOW","");
    //__wEvsASSERT((DataSourceTable[dataTblIdx].pData != NULL),"__vEvsUpdateData_VOSWINDOW","data is empty");

	pWin = (VOSWINDOW *)pVosWin;
	if (pWin->usState & VOS_STATE_CTYPE)	// visible
		lData = MAKELONG(MAKEWORD(pWin->Cursor.usCol, pWin->Cursor.usRow), 1);
	else									// invisible
		lData = MAKELONG(MAKEWORD(pWin->Cursor.usCol, pWin->Cursor.usRow), 0);

    /* send mesage to FrameWork */
    if(DataSourceTable[dataTblIdx].pData != NULL){
        __wEvsPostMessage(WU_EVS_CURSOR,dataTblIdx,(ULONG)dataId,(ULONG)lData);

		if (dataIdShadow) {
			DataSourceTable[dataTblIdxShadow].pData = DataSourceTable[dataTblIdx].pData;
			__wEvsPostMessage (WU_EVS_CURSOR, dataTblIdxShadow, (ULONG)dataIdShadow, (ULONG)lData);
		}
    }
}


/* ========================================================================== */

