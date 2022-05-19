/*
*===========================================================================
*
*   Copyright (c) 1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Local Header File
* Category    : Viode Output Service, 2170 System Application
* Program Name: VOSL.H
* --------------------------------------------------------------------------
* Abstract:     This file contains:
*                   1. Symbol Definitions
*                   2. Data type Definitions
*
* --------------------------------------------------------------------------
* Update Histories
*
* Data       Ver.      Name         Description
* Feb-23-95  G0        O.Nakada     Initial
*
** NCR2172 **
*
* Oct-05-99  01.00.00  M.Teraki     Added #pragma(...)
*
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


/*
*===========================================================================
*   Define Declarations
*===========================================================================
*/
#define VOS_NUM_WINDOW      (8 + 16)        /* number of window         */
#define VOS_NUM_ROW25       25              /* number of row            */
#define VOS_NUM_COL80       80              /* number of column         */
//#define VOS_NUM_COL40		40              /* number of column (Saratoga) */
#define VOS_NUM_COL42		42              /* number of column (US Customs SCER cwunn) */
#define VOS_NUM_COL46		46				/* number of columns (room for store forward indicator) */
#define VOS_NUM_OVL         ((VOS_NUM_WINDOW + 7) / 8)
#define VOS_NUM_BUFFER      (128 + 1)       /* buffer size of printf    */
#define VOS_NUM_MEMINFO     (8 + 16 + 16 + 16 + 16)

#define VOS_MEM2_MARK       0x2b            /* used memory, mark is '+' */
#define VOS_MEM2_SPECIAL    0               /* use special function     */

#define VOS_NUM_STACK       512             /* stack size of thread     */
#define VOS_SLEEP_THREAD    5000            /* 5 msec                   */

/* --- VOS configuration --- */

#define VOS_CONFIG_INIT     0x0001          /* initialization           */
#define VOS_CONFIG_LCD      0x0002          /* LCD provide              */

/* --- window state flag --- */

#define VOS_STATE_FREE      0               /* free window buffer       */
#define VOS_STATE_USE       0x0001          /* use window information   */
#define VOS_STATE_OVERLAP   0x0002          /* overlap window           */
#define VOS_STATE_WNORMAL   0x0004          /* window type, 1 = normal  */
#define VOS_STATE_WBUTTONS  0x0008          /* window type, 1 = contains buttons not text  */
#define VOS_STATE_CTYPE     0x0010          /* cursor type, 1 = visible */
#define VOS_STATE_BLIGHT    0x0100          /* back light,  1 = visible */
#define VOS_STATE_CHANGE    0x0200          /* changed back light state */

/* --- virtual memory control --- */

#define VOS_NUM_MEM         512             /* special heap buffer      */
#define VOS_NUM_MEMBLKS     ((VOS_NUM_MEM + sizeof(VOSMEM) - 1) / sizeof(VOSMEM))


/* --- macro --- */

#define ISBAD_ROW(usRow)    (VOS_ROW_TOP <= usRow && usRow <= VOS_ROW_BOTTOM) ? (FALSE) : (TRUE)
#define ISBAD_COL(usCol)    (VOS_COL_LEFT <= usCol && usCol <= VOS_COL_RIGHT) ? (FALSE) : (TRUE)

#define BUF_POS(Size, Pos)  (Size.usRow * Pos.usRow + Size.usCol * Pos.usCol)

#define BORDER_H(usB)       (UCHAR)((usB & VOS_B_SINGLE) ? (0xC4) : (0xCD))
#define BORDER_V(usB)       (UCHAR)((usB & VOS_B_SINGLE) ? (0xB3) : (0xBA))
#define BORDER_TL(usB)      (UCHAR)((usB & VOS_B_SINGLE) ? (0xDA) : (0xC9))
#define BORDER_TR(usB)      (UCHAR)((usB & VOS_B_SINGLE) ? (0xBF) : (0xBB))
#define BORDER_BL(usB)      (UCHAR)((usB & VOS_B_SINGLE) ? (0xC0) : (0xC8))
#define BORDER_BR(usB)      (UCHAR)((usB & VOS_B_SINGLE) ? (0xD9) : (0xBC))


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


typedef struct _VOSPOS {
    USHORT  usRow;                          /* position of row             */
    USHORT  usCol;                          /* position of column          */
} VOSPOS;

typedef struct _VOSVIDEO {
    USHORT  usState;                        /* state of window             */
    USHORT  usMode;                         /* current video mode          */
    VOSPOS  Size;                           /* current video size          */
    VOSPOS  Cursor;                         /* current cursor position     */
    USHORT  usMinutes;                      /* automatic back light off    */
} VOSVIDEO;

typedef struct _VOSWINDOW {
    USHORT  usState;                        /* state of window             */
    UCHAR   auchOvl[VOS_NUM_OVL];           /* flag of overlap window      */
    VOSPOS  PhyBase;                        /* physical position of window */
    VOSPOS  PhySize;                        /* physical size of window     */
    VOSPOS  CharBase;                 /* base position of character window */
    VOSPOS  CharSize;                       /* size of character window    */
    VOSPOS  Cursor;                         /* current cursor position     */
    USHORT  usFont;                         /* font attribute              */
    USHORT  usBorder;                       /* border state                */
    UCHAR   uchAttr;                        /* character attribute         */
    TCHAR   *puchBuf;                       /* address of window buffer    */
    USHORT  usMemInfo;                      /* information table of memory */
	USHORT  usIdentifier;                   // identifier such as 
} VOSWINDOW;


#define VOS_TAG_FREE        '-'
#define VOS_TAG_USED        '+'

typedef struct _VOSMEM {
    USHORT  usTag;                      /* TAG_FREE or TAG_USED        */
	USHORT	usLen;                      /* the length of the block     */
	USHORT	usNext;                     /* index to the next block     */
    USHORT  usPrev;                     /* index to the previous block */
} VOSMEM;


#if     (_WIN32_WCE || WIN32) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
/*
*===========================================================================
*    External Data Declarations
*===========================================================================
*/
/* --- VosInit.C --- */
extern  USHORT       usVosConfig;                /* VOS configuration    */
extern  PifSemHandle usVosApiSem;                /* exclusive semaphore  */
extern  VOSVIDEO     VosVideo;                   /* video information    */
extern  USHORT       usVosPage;                  /* video page           */

/* --- VosWCrea.C --- */
extern  USHORT       usVosNumWindow;             /* number of window     */
extern  VOSWINDOW    aVosWindow[VOS_NUM_WINDOW]; /* window information   */

/* --- VosQueue.C --- */
extern  USHORT       ausVosQueue[VOS_NUM_WINDOW];/* queue of window      */


/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/
/* --- VosWCrea.C --- */
SHORT   VosUpdateWindow(VOSWINDOW  *pWin);
VOID    VosEditBorder(VOSWINDOW  *pWin);

/* --- VosThrea.C --- */
//VOID    THREADENTRY VosThread(VOID);

/* --- VosMisc.C --- */
VOID    VosExecWindowActive(USHORT usReal, USHORT usComplusory);
VOID    VosUpdateOverlapFlag(USHORT usReal, USHORT usState);
VOID    VosDisplayPhyWindow(VOSWINDOW  *pWin);
VOID    VosDisplayRow(VOSWINDOW  *pWin, USHORT usRow);

/* --- VosQueue.C --- */
VOID    VosInsertQueue(USHORT usReal);
VOID    VosUpdateQueue(USHORT usReal);
VOID    VosDeleteQueue(USHORT usReal);

/* --- VosMem.C --- */
VOID    VosVirtualInit(VOID);
VOID    * VosVirtualAlloc(USHORT usBytes);
VOID    VosVirtualFree(VOID  *pvBlock);

/* --- VosMem2.C --- */
VOID    * VosMemoryAlloc(USHORT usID, USHORT usBytes, USHORT  *pusMemInfo);
VOID    VosMemoryFree(USHORT usMemInfo, VOID  *pvAddr);

/* --- VosXxxxx.C --- */
VOID    VosExecInit(USHORT usMode, USHORT usMinutes);
USHORT  VosExecBLight(USHORT usState);
VOID    VosExecRefresh(VOID);
SHORT   VosEditBorderString(VOSWINDOW  *pWin, TCHAR *puchStr, USHORT usStrLen, UCHAR uchAttr, USHORT usBorder);
VOID    VosEditCls(VOSWINDOW  *pWin, USHORT usTopRow, USHORT usNumRow);
VOID    VosExecCursor(VOSWINDOW  *pWin);
VOID    VosExecCurPos(VOSWINDOW  *pWin);
VOID    VosExecScrollUp(VOSWINDOW  *pWin, USHORT usNumber);
VOID    VosExecStringAttr(USHORT usHandle, TCHAR *puchString, USHORT usLength, UCHAR uchAttr, USHORT fsControl);

USHORT  VosEditPrintf(VOSWINDOW  *pWin, CONST TCHAR  *pszFormat, SHORT *psArgs, TCHAR *pszBuf, USHORT usBufLen);


/* =================================== */
/* ========== End of VOSL.H ========== */
/* =================================== */
