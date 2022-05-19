/*
*===========================================================================
*
*   Copyright (c) 1992-1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Ring Buffer Module, Header File
* Category    : User Interface Enigne, 2170 System Application
* Program Name: UieRing.H
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
*                   2. Data type Definitions
*                   3. External Data Definitions
*                   4. Function Prototypes
* --------------------------------------------------------------------------
* Update Histories
*
* Date       Ver.      Name         Description
* Mar-20-92  00.00.01  O.Nakada     Initial
* Sep-25-92  00.01.05  O.Nakada     Modify Ring Buffer and Auxiliary Ring
*                                   Buffer Status.
* Sep-25-92  00.01.05  O.Nakada     Added return status of Auxiliary Ring
*                                   Buffer.
* Mar-25-93  B1,PL-17  O.Nakada     Modify number of auxiliary ring buffer.
* Mar-15-95  G0        O.Nakada     Modified all declarations, correspond to
*                                   hospitality release 3.0 functions.
* Feb-14-96  G4        O.Nakada     Moved UieWriteRingBuf().
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
*   Symbol Define or Structure Type Define Declarations
*===========================================================================
*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

//RPH 4-29-4 increased Ring Num Inf from 16 to 160 due to errors from rapidly entering
//long Touchscreen Preset PLUs, 160 gives 10 times more room, which seems to be more than
//adequate to handle
#define UIE_RING_NUM_INF        160          /* number of device info.    */
//RPH 3-15-4 increased ring buffer size to 512 from 224 MSR data would not fit
#define UIE_RING_LEN_BUF        2048        /* length of input data lets make big enough for several UIE_MSR buffers  */
#define UIE_RING_FULL           0x01        /* ring buffer full          */

#define UIE_CNT_CLEAR           0x01        /* in error                  */
#define UIE_CNT_ABORT           0x02        /* in error                  */

typedef struct _UIERINGCNT {
    UCHAR   uchClear;                       /* read counter of clear key */
    UCHAR   uchAbort;                       /* read counter of abort key */
    UCHAR   fchControl;                     /* control status            */
} UIERINGCNT;

typedef struct _UIERINGSTS {
    UCHAR   uchNumber;                      /* number of device information */
    USHORT  uchWrite;                       /* write offset of device info. */
    USHORT  uchRead;                        /* read offset of device info.  */
    USHORT  usLength;                       /* length of input data         */
    USHORT  usOffset;                       /* write offset of input data   */
    UCHAR   fchControl;                     /* control status               */
} UIERINGSTS;

typedef struct _UIERINGINF {
    FSCTBL  Fsc;                            /* function selection code */
    UCHAR   uchMode;                        /* position of mode key    */
    USHORT  usLength;                       /* length of input data    */
    USHORT  usOffset;                       /* offset of ring buffer   */
} UIERINGINF;


#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/
VOID    UieRingBufInit(VOID);
VOID    UieClearRingBufSub(VOID);
SHORT   UieFscRingBuf(FSCTBL *pFsc);
VOID    UieSwitchRingBuf(VOID);


/* ====================================== */
/* ========== End of UieRing.H ========== */
/* ====================================== */
