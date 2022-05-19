/*
*===========================================================================
*
*   Copyright (c) 1992-1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Local Header File of Display Module
* Category    : User Interface Enigne, 2170 System Application
* Program Name: UieDisp.H
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: 
* Options     : 
* --------------------------------------------------------------------------
* Abstract:     This header file defines User Interface Engine the 2170
*               system application.
*
*               This file contains:
*                   1. Symbol Definitions
*                   2. External Data Definitions
*                   3. Function Prototypes
* --------------------------------------------------------------------------
* Update Histories
*
* Date       Ver.      Name         Description
* Mar-20-92  00.00.01  O.Nakada     Initial
* Sep-16-93  F1        O.Nakada     Divided the User Interface Engine.
* Mar-15-95  G0        O.Nakada     Modified all declarations, correspond to
*                                   hospitality release 3.0 functions.
* Dec-11-95  G4        O.Nakada     Modified prototype of UieDisplay2x20().
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

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

/*
*===========================================================================
*   Display Buffer Size
*===========================================================================
*/
#define UIE_2X20_ROW        2               /* 2x20 display row      */
#define UIE_2X20_COLUMN     20              /* 2x20 display column   */

#define UIE_LCD_ROW         2               /* number of row         */
#define UIE_LCD_COLUMN      40              /* number of columns in display on left*/
#define UIE_LCD_COLUMN2     46              /* number of columns in display at bottom center (indicators)*/
											/* If UIE_LCD_COLUMN2 changes, LCDWIN_LEN_COMMON002 will need to be updated.
												See	voswcrea.c VosCreateWindow() calculation of usBytes, and initialization 
												of aVosMem2Info[] using LCDWIN_LEN_COMMON002 in vostable.c.
											*/

#define UIE_10N10D_ROW      1               /* 10N10D display row    */
#define UIE_10N10D_COLUMN   10              /* 10N10D display column */

/* --- display buffer with attribute --- */

#define UIE_MAX_2X20_BUF    (UIE_2X20_ROW * UIE_2X20_COLUMN * 2)
#define UIE_MAX_LCD_BUF     (UIE_LCD_ROW * UIE_LCD_COLUMN * 2)
#define UIE_MAX_10N10D_BUF  (UIE_10N10D_ROW * UIE_10N10D_COLUMN * 2)

#define UIE_MAX_DISP_BUF    UIE_MAX_LCD_BUF


/*
*===========================================================================
*   Number of Descriptor
*===========================================================================
*/
#define UIE_MAX_2X20_DESC   16              /* 2x20 display   */
#define UIE_MAX_10N10D_DESC 10              /* 10N10D display */


/*
*===========================================================================
*   Operator and Customer Display Buffer
*===========================================================================
*/
typedef struct _UIERECOVERY {
    TCHAR   auchOperDisp[UIE_MAX_DISP_BUF];
    TCHAR   auchCustDisp[UIE_MAX_DISP_BUF];
    ULONG   flOperDesc[2];
    ULONG   flCustDesc[2];
} UIERECOVERY;


/*
*===========================================================================
*   Descriptor Interface on LCD
*===========================================================================
*/
typedef struct _UIELCD {
    CONST   UIEDESCLCD  *pInfo;         /* descriptor information */
    CONST   TCHAR       *puchBuf;       /* descriptor mnemonics   */
	USHORT   usCount;                       // Number of items in pInfo array
} UIELCD;


/*
*===========================================================================
*   External Work Area Declarations
*===========================================================================
*/
extern  PifSemHandle   usUieSyncDisplay;       /* synchronous semaphore    */
extern  USHORT         usUieVosDisp;           /* handle of VOS display    */
extern  USHORT         usUieVosDesc;           /* handle of VOS descriptor */
extern  USHORT         usUieDispCol;           /* number of column         */
extern  TCHAR          auchUiePrevOD[UIE_MAX_DISP_BUF];/* redisplay buffer */
extern  UIERECOVERY    UieRecovery;            /* recovery buffer          */
extern  UIELCD         UieLcd;                 /* LCD status               */


/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/
VOID    UieDisplayInit(VOID);
SHORT   UieChooseDisplay(USHORT usDisplay, USHORT usRow, USHORT usColumn,
                         UCHAR uchAttr, CONST TCHAR FAR *pszFmt,
                         SHORT *psArgs, TCHAR *puchOperBuf,
                         TCHAR *puchCustBuf);
VOID    UieDisplay2x20(USHORT usRow, USHORT usColumn, UCHAR uchAttr,
                       TCHAR *pszStr, TCHAR *puchDispBuf, USHORT usWidth);
VOID    UieDisplay10N10D(USHORT usColumn, UCHAR uchAttr, TCHAR *pszStr,
                         TCHAR *puchDispBuf);
VOID    UieBuf2SwitchDispRecovery(USHORT usDisplay, TCHAR *puchOperBuf,
                                  TCHAR *puchCustBuf);
VOID    UieBuf2Disp(USHORT usDisplay, TCHAR *puchOperBuf, TCHAR *puchCustBuf);
VOID    UieLightDescr(USHORT usDisplay, USHORT usNo, UCHAR uchAttr);


#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

/* ====================================== */
/* ========== End of UieDisp.H ========== */
/* ====================================== */
