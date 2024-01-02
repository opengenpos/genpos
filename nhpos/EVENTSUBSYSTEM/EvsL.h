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
* Title       : Event Sub System Headder File (LOCAL)
* Category    : Event Sub System Dynamic Link Library, NCR 2172 for Win32
* Program Name: EvsL.h
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
* May-17-15 : 2.02.01  : R.Chambers : change __wEvsASSERT() to __wEvsASSERT_func() for ASSRTLOG log
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
#ifndef _EVSL_H
#define _EVSL_H

#ifdef __cplusplus
extern "C" {  /* Assume C declarations for C++ */
#endif /* __cplusplus */

/*
*===========================================================================
*   Define Declarations
*===========================================================================
*/

#define	EVS_MAX_SRCUSERS        (20)             /* MAX number of users  */
#define EVS_MAX_DATASRC         (30)            /* MAX number of data sources */

/* defaults */
#define EVS_USER_DEFAULT        (0xFFFFFFFF)    /* */
#define EVS_DATAID_DEFAULT      (-1)            /* */
						

/* data type */
#define EVS_TYPE_UNKNOWN        (0)             /*                      */
#define EVS_TYPE_DISPDATA       (1)             /* a window which displays lines of text or lines of buttons */
#define EVS_TYPE_SCREENMODE     (2)             /*  */
#define EVS_TYPE_DISPIMAGE      (3)             /* a window which displays an image, bitmap or png file */

/* events */
#define EVS_NULL                (0)
#define EVS_UPDATE              (1)
#define EVS_CREATE              (2)
#define EVS_DELETE              (3)

#ifdef _DEBUG
// lets make the default to not do any kind of a 
// debugbreak() in the case of EVS events since enabling
// this will result in multiple breaks happening from
// within the window display source code.
// Trust me, it's painful to do debugging with it
// enabled.  Nov-02-2023  Richard Chambers
#if 1
#define EVS_DEBUG_NOP
#else
#define EVS_DEBUG_NOP          __debugbreak();   // replaced _asm {  nop   }; to allow ARM compiles for Windows on Arm
#endif
#endif
/*
*===========================================================================
*   Structure Type Define Declarations
*===========================================================================
*/

#if     (_WIN32_WCE || WIN32) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

typedef struct _EVSDATASOURCE{
    ULONG     ulUsers[EVS_MAX_SRCUSERS];        /* handle of windows(for Win32) */
    SHORT     sDataID;                          /* index of data sources     */
    SHORT     sType;                            /* type of data              */
    VOID      *pData;                           /* pointer to data source    */
} EVSDATASOURCE;

typedef short  EVS_EVENT;
typedef struct _SCREENMODE_TABLE{
    EVS_EVENT Event;
    SHORT     sDataID;
    SHORT     sMode;
}SCREENMODE_TABLE;

#if (_WIN32_WCE || WIN32) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

/*
*===========================================================================
*   Global Valiables
*===========================================================================
*/

#ifdef __EVENT_SUB_SYSTEM__DLLMAIN
    #define __EXTERN__
#else
    #define __EXTERN__  extern
#endif /* _EVS_DLLMAIM */

__EXTERN__ EVSDATASOURCE  DataSourceTable[EVS_MAX_DATASRC];
//extern EVSDATASOURCE  DataSourceTable[];
extern const SCREENMODE_TABLE ScreenModeTable[];

/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/
/* --- EvsInit.c --- */
VOID	__EvsInit(VOID);
VOID    __EvsSetScreenMode(SHORT modeTblIdx);
SHORT __EvsGetScreenMode(VOID);

/* --- EvsPrcV.c --- */
LONG    __vEvsGetData(LONG lDataSrcIdx,VOID *pData,LONG Len);
LONG    __vEvsGetVosWin(LONG lDataSrcIdx,VOID *pVosWin,VOID *pData);
VOID    __vEvsCreateData_VOSWINDOW(VOID *pVosWin);
VOID    __vEvsDeleteData_VOSWINDOW(VOID *pVosWin);
VOID    __vEvsUpdateData_VOSWINDOW(VOID *pVosWin);
VOID    __vEvsUpdateData_SCRMODE(VOID *pVosWin);
VOID    __vEvsUpdateCheck_SCRMODE(EVS_EVENT Ev,VOID *pVosWin);
VOID    __vEvsUpdateCursor_VOSWINDOW(VOID *pVosWin);
VOID    __vEvsGetParam_VOSWINDOW (VOID *pVosWin, ULONG wParam, VOID *lParam); /* used to get button dimensions from Framework - CSMALL */
VOID    __vEvsPutParam_VOSWINDOW (VOID *pVosWin, ULONG wParam, VOID *lParam); /* used to get button dimensions from Framework - CSMALL */

/* --- EvsPrcW.c --- */
VOID    __wEvsPostMessage(ULONG ulMsgNo,LONG lDataSrcIdx,ULONG wParam,ULONG lParam);
SHORT    __wEvsSendMessage(ULONG ulMsgNo,LONG lDataSrcIdx,ULONG wParam,ULONG lParam); /* used to get button dimensions from Framework - CSMALL */
#if defined(EVS_DEBUG)
    VOID __wEvsASSERT_func(int nSert, char *pchAssert, char *pchFile, int nLine);
    VOID _wEvsDebugMsgTrace(ULONG ulMsg,ULONG ulUser,ULONG wParam,ULONG lParam,char * pMsg);
	VOID _wEvsDebugTrace(char * pMsg);
#define __wEvsASSERT(azSert,pchProcName,pchMsg)   __wEvsASSERT_func(azSert, (UCHAR *)  #azSert, __FILE__, __LINE__)
#else
# define __wEvsASSERT(v,n,m)  ((void)0)
# define _wEvsDebugMsgTrace(ulMsg,ulUser,wParam,lParam,pMsg)  ((void)0)
# define _wEvsDebugTrace(pMsg)  ((void)0)
#endif /* EVS_DEBUG */

/* --- EvsProc.c --- */
LONG    __EvsSearchDataSrc_ID(SHORT sDataId);
LONG    __EvsSearchDataSrc_Empty(VOID);
LONG    __EvsSearchUser(SHORT sIdx,ULONG ulUser);
LONG    __EvsSearchUsr_Empty(LONG lDataSrcIdx);
SHORT   __EvsAddUser(LONG lDataSrcIdx,LONG lUsrIdx,ULONG ulhUser);
VOID	__EvsRemoveUser(LONG lDataTblIdx,LONG lUsrIdx);


/* --- EvsVio.c --- */
VOID    EvsVioStrAttr(USHORT usRow, USHORT usColumn,UCHAR *puchString, USHORT usChars,
                               USHORT usPage,VOID *pVosWin);
/* VOID    EvsVioStr(USHORT usRow, USHORT usColumn,
                           CONST UCHAR FAR *puchString, USHORT usChars,
                           UCHAR  uchAttr, USHORT usPage); */
VOID    EvsVioCurOnOff(USHORT usControl, VOID *pVosWin);
VOID    EvsVioCurType(UCHAR uchTop, UCHAR uchBottom, VOID *pVosWin);
/*VOID    EvsVioGetCurPos(USHORT FAR *pusRow, USHORT FAR *pusColumn,USHORT usPage);*/
VOID    EvsVioSetCurPos(USHORT usRow, USHORT usColumn,
                                 USHORT usPage, VOID *pVosWin);
VOID    EvsVioScrollUp(USHORT usTopRow, USHORT usLeftCol, USHORT usBotRow,
                            USHORT usRightCol, USHORT usNumRow, UCHAR uchAttr);
VOID    EvsVioScrollDown(USHORT usTopRow, USHORT usLeftCol, USHORT usBotRow,
                            USHORT usRightCol, USHORT usNumRow, UCHAR uchAttr);
VOID    EvsVioMode(USHORT usMode);
SHORT   EvsVioBackLight(USHORT usControl);
/*VOID    EvsVioActivePage(USHORT usPage);*/
/*VOID    EvsVioLoadCG(UCHAR FAR *puchFonts, UCHAR uchBytes);*/


/* ========================================================================= */
#ifdef __cplusplus
} /* End of extern "C" { */
#endif /* __cplusplus */
#endif /* _EVS_H */
/* ========================================================================= */

