/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993-1996       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Display on the Fly, Header File
* Category    : Display on the Fly, NCR 2170 HOSP US ENH. Application
* Program Name: DFL.H
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1s /Os /Zap
* --------------------------------------------------------------------------
* Abstract:
*               This file contains:
*                   1. Symbol Definitions
*                   2. Data type Definitions
*                   3. Function Prototypes
*
* --------------------------------------------------------------------------
* Update Histories
*                                       
* Date      Ver.        Name            Description
* Jun-16-93 00.00.01    NAKAJIMA,Ken    Initial
* Jul-20-95 03.00.04    T.Nakahata      add coupon & customer name
* Dec-08-95 03.01.00    M.Ozawa         chg uchCRT -> auchCRT[2]
* Apr-16-96 03.01.04    T.Nakahata      add DFL message sequential counter.
*
** NCR2172 **
*
* Oct-05-99 01.00.00    M.Teraki        initial (for 2172)
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
#define DFL_PERFORM         0
#define DFL_FAIL          -30
#define DFL_BUSY          -31

#define DFL_STX                 0x02
#define DFL_ETX                 0x03
#define DFL_ENQ                 0x05
#define DFL_ACK                 0x06

/* -- DFLINF auchItemType[2] -- */
/* 1st byte */
#define DFL_NORMAL              0x30
#define DFL_TRAIN               0x31
#define DFL_NORMAL_VOID         0x32
#define DFL_TRAIN_VOID          0x33
    /* -- following parameter is not used at FVT R3.1, but adds DFL message
            sequential number -- */
#define DFL_NORMAL_KDS          0x34    /* normal item at Retry, RESERVED */
#define DFL_TRAIN_KDS           0x35    /* training item at Retry, RESERVED */
#define DFL_NORMAL_VOID_KDS     0x36    /* normal void item at Retry, RESERVED */
#define DFL_TRAIN_VOID_KDS      0x37    /* training void item at Retry, RESERVED */
/* 2nd byte */
#define DFL_END                 0x30
#define DFL_SALES               0x31
#define DFL_DISC                0x32
#define DFL_TOTAL               0x33
#define DFL_TAX                 0x34
#define DFL_TENDER              0x35
#define DFL_CANCEL              0x36
#define DFL_SINGLE              0x37
#define DFL_MODIFIER            0x38
#define DFL_TRANOPEN            0x39
#define DFL_COUPON              0x41    /* add (REL 3.0) */
#define DFL_CUSTNAME            0x42    /* add (REL 3.0) */

/* DFL message sequential no. */
#define DFL_MSGSEQNO_INIT       0x31    /* add FVT comment, (R3.1) */
#define DFL_MSGSEQNO_MAX        0x39    /* add FVT comment, (R3.1) */

/*
*===========================================================================
*   Structure Type Define Declarations
*===========================================================================
*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif


typedef struct {
    UCHAR   uchSTX;
    TCHAR   aszVLI[3];
    TCHAR   aszLineNo[2];
    TCHAR   aszTermAdr[2];
    TCHAR   aszConsNo[4];
    UCHAR   auchItemType[2];
    UCHAR   auchCRTNo[2];               /* R3.1 */
    UCHAR   uchFrameType;
    UCHAR   uchDflMsgSeqNo;         /* DFL msg sequence #, add FVT R3.1 */
} DFLINF;

typedef struct {
    DFLINF  DflHead;
    UCHAR   uchSeqNo;
} DFLINTERFACE;


#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
/*
*===========================================================================
*   External Work Area Declarations
*===========================================================================
*/


/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/
SHORT DflPrint(VOID *pData);
VOID  DflInit(VOID);

/* ---------- End of DFL.H ---------- */


