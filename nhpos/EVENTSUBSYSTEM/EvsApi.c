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
* Title       : Event Sub System API
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
*
* Update Histories                                                         
*   Date     : Ver.Rev. :   Name     : Description
* 12.18.2002: 01.04.00	:	cwunn	 : GSU SR 15 case to for
                                        security reminder screen
*==========================================================================
*/


#include <windows.h>

#include "ecr.h"
#include "pif.h"
#include "log.h"
#include "vosl.h"
#include "termcfg.h"

#define	__EVENT_SUB_SYSTEM_EXPORTS
#include "Evs.h"
#include "EvsL.h"
/*
*===========================================================================
** Synopsis:    SHORT EvsConnect(SHORT sDataID, ULONG ulUser);
*Å@
*     Input:    sDataId   - data source id
*               ulUser    - owner of data sources(win32 handle)
*
** Return:      
*
* Update Histories                                                         
*   Date     : Ver.Rev. :   Name     : Description
* 12.18.2002: 01.04.00	:	cwunn	 : GSU SR 15 added SUPER301 case to for
                                        security reminder screen
*===========================================================================
*/
__declspec(dllexport)  SHORT EvsConnect(SHORT sDataID,ULONG ulUser){
    int   dataTableIdx,usrIdx,status;

#ifdef _DEBUG
    switch(sDataID){
    case LCDWIN_ID_REG101:
		EVS_DEBUG_NOP
        break;
    case LCDWIN_ID_REG208:
		EVS_DEBUG_NOP
        break;
    case LCDWIN_ID_SUPER300:
	case LCDWIN_ID_SUPER301: //case for SUPER301 added by cwunn for GSU SR 15
		EVS_DEBUG_NOP
        break;
    case LCDWIN_ID_PROG400:
		EVS_DEBUG_NOP
        break;
    case 1001:
		EVS_DEBUG_NOP
        break;
    }
#endif

    /* request semaphore */
    PifRequestSem(usVosApiSem);

    /* search dataID from DataSourceTable */
    dataTableIdx = (int)__EvsSearchDataSrc_ID(sDataID);
    if(dataTableIdx < 0){
        /* illeagal data source ID */
        status = EVS_ERR_ILLEAGAL_ID;
        goto EXT_EVSCONNECT;
    }

    /*  */
    usrIdx = (int)__EvsSearchUsr_Empty(dataTableIdx);
    if(usrIdx < 0){
        /* usertable is FULL */
        status = EVS_ERR_NUM_USERS;
        /* PifAbort? */
        goto EXT_EVSCONNECT;
    }

    /* connect */
    __EvsAddUser(dataTableIdx,usrIdx,ulUser);

    status = EVS_OK;

EXT_EVSCONNECT:
    /* release semaphore */
    PifReleaseSem(usVosApiSem);
    return (SHORT)status;
}


/*
*===========================================================================
** Synopsis:    SHORT EvsGetData(SHORT sDataID, ULONG ulUser
*                                ,VOID *pBuf,LONG Len,VOID *pVosWin);
*     Input:    sDataId   - data source id
*               ulUser    - owner of data sources(win32 handle)
*               lLen      - data length
*     In/Out:   pBuf      - copy to
*               pVosWin   - VOS information
*
** Return:      VIO_OK    - okay
*               <0        - ERROR (see EVSL.H)
*===========================================================================
*/
__declspec(dllexport) SHORT EvsGetData(SHORT sDataID,ULONG ulUser,VOID *pBuf,LONG Len,VOID *pVosWin){
    int     dataTableIdx;
    SHORT   status;

    /* request semaphore */
    PifRequestSem(usVosApiSem);

    /* check the data ID & user ID */
    if((dataTableIdx = (int)__EvsSearchDataSrc_ID(sDataID)) < 0){
        /* illeagal data ID */
        status = EVS_ERR_ILLEAGAL_ID;
        goto EXT_EVSGETDATA;
    }
    if(__EvsSearchUser((SHORT)dataTableIdx,ulUser) < 0){
        status = EVS_ERR_ILLEAGAL_USER;
        goto EXT_EVSGETDATA;
    }

    /* check the length */
	if(__vEvsGetData((LONG)dataTableIdx,pBuf,Len) <= 0){
		status = EVS_ERR_BUFSIZE;
        goto EXT_EVSGETDATA;
	}

    /* NULL? */
    if(pVosWin != NULL){
        __vEvsGetVosWin((LONG)dataTableIdx,pVosWin,pBuf);
    }
    status = EVS_OK;

EXT_EVSGETDATA:
    /* release semaphore */
    PifReleaseSem(usVosApiSem);

    return status;
}


__declspec(dllexport)  BOOL EvsResetWindows(void)
{

	//remove the connect the text windows have to evs

	VosDeleteWindow(1);//delete LeadThru Window
	VosDeleteWindow(2);//delete Descriptor window

	__EvsInit();

	return TRUE;
}

__declspec(dllexport)	BOOL EvsCreateStatusWindows(void)
{
	USHORT usHandle;
	
		/* --- create a 2x20 simulation window --- */

	usHandle = LCDWIN_ID_COMMON001;         /* window ID         */
	VosCreateWindow(21,                         /* start row         */
					0,                          /* start column      */
					ECHO_BACK_ROW_LEN,                /* number of row     */
					ECHO_BACK_CLM_LEN,             /* number of column  */
					0,            /* font number       */
					0x01/*0x07*/, /* character attr.   */ //colorpalette-changes
					0,                 /* border attribute  */
					&usHandle);             /* address of handle */

	/* --- create a descriptor window --- */

	usHandle = LCDWIN_ID_COMMON002;         /* window ID         */
	VosCreateWindow(23,                         /* start row         */
					0,                          /* start column      */
					DESCRIPTOR_ROW_LEN,                          /* number of row     */
					DESCRIPTOR_CLM_LEN,                         /* number of column  */
					0,            /* font number       */
					0x01/*0x07*/, /* character attr.   */ //colorpalette-changes
					0,                 /* border attribute  */
					&usHandle);             /* address of handle */

	// Create a customer display window that will be a reflection of what shows in the
	// standard window as seen by the Cashier.
	usHandle = LCDWIN_ID_COMMON004;         /* window ID         */
	VosCreateWindow(21,                         /* start row         */
					0,                          /* start column      */
					ECHO_BACK_ROW_LEN,                /* number of row     */
					ECHO_BACK_CLM_LEN,             /* number of column  */
					0,            /* font number       */
					0x01/*0x07*/, /* character attr.   */ //colorpalette-changes
					0,                 /* border attribute  */
					&usHandle);             /* address of handle */

	return TRUE;
}

/*
*===========================================================================
** Synopsis:    SHORT EvsDisconnect(SHORT sDataID, ULONG ulUser);
*Å@
*     Input:    sDataId   - data source id
*               ulUser    - owner of data sources(win32 handle)
*
** Return:      
*
* Update Histories                                                         
*   Date     : Ver.Rev. :   Name     : Description
* 12.18.2002: 01.04.00	:	cwunn	 : GSU SR 15 added SUPER301 case to for
                                        security reminder screen
*===========================================================================
*/
__declspec(dllexport)  SHORT EvsDisconnect(SHORT sDataID,ULONG ulUser){
    int   dataTableIdx,usrIdx,status;

#ifdef _DEBUG
    switch(sDataID){
    case LCDWIN_ID_REG101:
		EVS_DEBUG_NOP
        break;
    case LCDWIN_ID_REG208:
		EVS_DEBUG_NOP
        break;
    case LCDWIN_ID_SUPER300:
	case LCDWIN_ID_SUPER301: //case for SUPER301 added by cwunn for GSU SR 15
		EVS_DEBUG_NOP
        break;
    case LCDWIN_ID_PROG400:
		EVS_DEBUG_NOP
        break;
    case 1001:
		EVS_DEBUG_NOP
        break;
    }
#endif

    /* request semaphore */
    PifRequestSem(usVosApiSem);

    /* search dataID from DataSourceTable */
    dataTableIdx = (int)__EvsSearchDataSrc_ID(sDataID);
    if(dataTableIdx < 0){
        /* illeagal data source ID */
        status = EVS_ERR_ILLEAGAL_ID;
        goto EXT_EVSDISCONNECT;
    }

    /*  */
    usrIdx = (int)__EvsSearchUser((SHORT)dataTableIdx, ulUser);
    if(usrIdx < 0){
        /* user is not found */
        status = EVS_ERR_ILLEAGAL_USER;
        /* PifAbort? */
        goto EXT_EVSDISCONNECT;
    }

    /* disconnect */
    __EvsRemoveUser(dataTableIdx,usrIdx);

    status = EVS_OK;

EXT_EVSDISCONNECT:
    /* release semaphore */
    PifReleaseSem(usVosApiSem);
    return (SHORT)status;
}


/* ========================================================================= */
