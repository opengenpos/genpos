/*
*===========================================================================
*
*   Copyright (c) 1992-1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Local Header File of Key Track Module
* Category    : User Interface Enigne, 2170 System Application
* Program Name: UieTrack.H
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
*                   2. Function Prototypes
* --------------------------------------------------------------------------
* Update Histories
*
* Date       Ver.      Name         Description
* Mar-20-92  00.00.01  O.Nakada     Initial
* Sep-29-92  00.01.05  O.Nakada     Modify key track function.
* Jun-05-93  C1        O.Nakada     Modify key track function.
* Jul-05-93  C3        O.Nakada     Removed UIE_TEST_INIT.
* Mar-15-95  G0        O.Nakada     Modified all declarations, correspond to
*                                   hospitality release 3.0 functions.
* Jul-12-95  G1        O.Nakada     Added UieWakeupTrack() function.
*
** NCR2172 **
*
* Oct-05-99  01.00.00  M.Teraki     Added #pragma(...)
* Apr-19-00            M.Teraki     Change value of UIE_TEST_PASS from 2170
*                                   to 2172
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
*   Define Declarations
*===========================================================================
*/

/* --- major code --- */
#define UIE_TKEY_AN             1       /* alpha/numeric key      */
#define UIE_TKEY_FUNC           2       /* function key           */
#define UIE_TKEY_POWER          3       /* power down recovery    */
#define UIE_TKEY_MODE           4       /* mode change            */
#define UIE_TKEY_DRAWER         5       /* closed drawer message  */
#define UIE_TKEY_SCANNER        6       /* scanner data           */
#define UIE_TKEY_MSR            7       /* msr data               */
#define UIE_TKEY_WAITER         8       /* changed waiter lock    */
#define UIE_TKEY_SCALE          9       /* scale data             */
#define UIE_TKEY_ERROR          10      /* error occred           */

/* --- minor code of error --- */
#define UIE_TKEY_KEYDP          1       /* double dpression error */
#define UIE_TKEY_RB             2       /* ring buffer full       */


/*
*===========================================================================
*   Loop Tester Status
*===========================================================================
*/
#define UIE_TEST_TRACK          0x01        /* tracking           */
#define UIE_TEST_EXECUTE        0x04        /* loop tester        */

typedef struct _UIETEST {
    UCHAR   fchStatus;                      /* control status     */
    USHORT  usNumber;                       /* number of data     */
    USHORT  usRead;                         /* offset of reading  */
    UCHAR   uchFuncWait; /* wait duration before function key event */
    UCHAR   uchBetween;                     /* transaction timer  */
    UCHAR   uchError;                       /* error cancel timer */
    UCHAR   uchPara1000;                    /* parameter          */
    UCHAR   uchPara100;                     /* parameter          */
    UCHAR   uchPara10;                      /* parameter          */
    UCHAR   uchPara1;                       /* parameter          */
} UIETEST;


/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/
SHORT   UieTracking(UCHAR uchMsg, UCHAR uchData1, UCHAR uchData2);
VOID    UieTrackKey(UCHAR uchData);
SHORT   UieTrackTrigger(UCHAR uchMajor);
SHORT   UieReadTrack(USHORT *pusType, FSCTBL *pRing, UCHAR *puchData);
UCHAR   UieGetTrackKey(VOID);
VOID    UieWakeupTrack(VOID);
VOID    UieCancelTester(VOID);
VOID    UieSetStanza(USHORT usNumber);

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

/* ======================================= */
/* ========== End of UieTrack.H ========== */
/* ======================================= */
