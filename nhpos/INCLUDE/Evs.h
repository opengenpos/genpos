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
* Title       : Event Sub System Headder File           
* Category    : Event Sub System Dynamic Link Library, NCR 2172 for Win32
* Program Name: Evs.h                                                      
* --------------------------------------------------------------------------
* Compiler    : VC++ Ver. 6.00  by Microsoft Corp.                         
* Memory Model: 
* Options     : 
* --------------------------------------------------------------------------
* Abstruct:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
** NCR2172 **
* Oct-06-99 : 1.00.00  : K.Iwata    : initial (for 2172)
*           :          :            :
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
#ifndef _EVS_H
#define _EVS_H

#ifdef __cplusplus
extern "C" {  /* Assume C declarations for C++ */
#endif /* __cplusplus */

/*
*===========================================================================
*   Define Declarations
*===========================================================================
*/

/* status */
#define EVS_OK                  (0)             /* NORMAL               */
#define EVS_ERR_ILLEAGAL_ID     (-1)            /* */
#define EVS_ERR_NUM_USERS       (-2)            /* */
#define EVS_ERR_NUM_DATASRC     (-3)            /* */
#define EVS_ERR_ILLEAGAL_USER   (-4)            /* user id error        */
#define EVS_ERR_BUFSIZE         (-5)            /*      */

/* screen modes */
#define EVS_MODE_UNKNOWN        (0)             /* default mode         */
#define EVS_MODE_LOCK	        (1)             /* LOCK mode            */
#define EVS_MODE_REG1	        (2)             /* REGISTER 1           */
#define EVS_MODE_REG2	        (3)             /*          2           */
#define EVS_MODE_SPV	        (4)             /* SUPER VISER mode     */
#define EVS_MODE_PRG	        (5)             /* PROGRAM mode         */

/* data resource ID */
/* screen No -> termcfg.h */
#define	SCRMODE_ID              (1001)            /* screen mode          */

/*
*===========================================================================
*   Define Messages
*===========================================================================
*/

//#ifdef  _WINDOWS_       /* Use types from WINDOWS.H */
//#pragma message("INCLUDING windows.h!!!")

#define	WU_EVS_UPDATE   (1024 + 2000)        /* update data          */
#define	WU_EVS_CREATE   (1024 + 2001)        /* create data          */
#define WU_EVS_DELETE   (1024 + 2002)        /* delete data          */
#define WU_EVS_CURSOR   (1024 + 2003)        /* update cursor        */
#define WU_EVS_DFLT_LOAD   (1024 + 2004)     /* load the default layout        */
#define WU_EVS_GETPARAM   (1024 + 2005)      /* Get the specified window parameter from a window
											  * used to get button dimensions from Framework - CSMALL */
#define WU_EVS_PUTPARAM   (1024 + 2006)      /* Put the specified window parameter to a window */
#define WU_EVS_POPUP_WINDOW   (1024 + 2007)  // Popup the specified window using the window name
#define WU_EVS_POPDOWN_WINDOW (1024 + 2008)  // Popdown the specified window using the window name
#define WM_APP_CONNENGINE_MSG_RCVD    (1024 + 2009)  // Message indicating that ConnEngine has received a message
#define WM_APP_DEVICEENGINE_IOCTL     (1024 + 2010)  // Message indicating an IOCTL type request for DeviceEngine to process
#define WM_APP_BIOMETRIC_MSG_RCVD	(1024 + 2011)	//Message indicating that a biometric device has sent a message
#define WM_APP_XMLMSG_MSG_RCVD	    (1024 + 2012)	//Message indicating XML message for CFrameworkWndDoc::OnXmlMsgRcvd()
#define WM_APP_SHUTDOWN_MSG	        (1024 + 2013)	//Shutdown message from PifShutDownApp() for CFrameworkWndDoc::OnShutdownMsgRcvd() sends event consumed by FrameworkShutdownThread()
#define WM_APP_SHOW_HIDE_GROUP      (1024 + 2014)	//Message indicating to Show (lParam == 1) or Hide (lParam == 0) a group association mask (wParam)
#define WM_APP_OEP_IMAGERY          (1024 + 2015)	//Message indicating OEP imagery display change

//#endif /* _INC_WINDOWS */

/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/

#if defined(__EVENT_SUB_SYSTEM_EXPORTS)
    #pragma message("DLL EXPORT")
    #define _DLLSPEC _declspec(dllexport)
#else
    #pragma message("DLL IMPORT")
    #define _DLLSPEC _declspec(dllimport)
#endif /* __EventSubSystem */

_DLLSPEC SHORT EvsConnect(SHORT,unsigned long);
_DLLSPEC SHORT EvsDisconnect(SHORT sDataID,ULONG ulUser);
_DLLSPEC SHORT EvsGetData(SHORT,ULONG,VOID *,LONG,VOID *);
_DLLSPEC BOOL EvsResetWindows(void);
_DLLSPEC BOOL EvsCreateStatusWindows(void);

_DLLSPEC VOID    VosCreateWindow(USHORT usRow, USHORT usCol, USHORT usNumRow,
                        USHORT usNumCol, USHORT usFont, UCHAR uchAttr,
                        USHORT usBorder, USHORT *pusHandle);
_DLLSPEC VOID    VosDeleteWindow(USHORT usHandle);
/* ========================================================================= */
#ifdef __cplusplus
} /* End of extern "C" { */
#endif /* __cplusplus */

#endif /* _EVS_H */
/* ========================================================================= */
