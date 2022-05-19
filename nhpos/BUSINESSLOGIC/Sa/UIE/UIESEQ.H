/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1993       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Key Seqeunce Module, Header File
* Category    : User Interface Enigne, NCR 2170 High Level System Application
* Program Name: UIESEQ.H
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Meduim/Large Model
* Options     : /c /A[M|Lw] /W4 /G[1|2]s /Os /Zap
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
* Sep-04-92  00.01.00  O.Nakada     Changed number of feed lines.
* Nov-04-92  00.01.09  O.Nakada     Changed number of journal feed.
* Sep-16-93  E1        O.Nakada     Divided the User Interface Engine.
* Oct-12-93  E1        O.Nakada     Apply a function to Round-Trip Service.
*
** NCR2172 **
*
* Oct-05-99  01.00.00  M.Teraki     Added #pragma(...)
*
*** NHPOS 1.4.x, NCR 7448  ***
* Aug-01-02  NHPOS 1.4 R.Chambers   Product donated to Georgia Southern University
*** NHPOS 2.0.x Touchscreen ***
* Mar-01-04  NHPOS 2.0 R.Chambers   Conversion from NCR 7448 to Touchscreen terminals
*** GenPOS 2.2.x Touchscreen ***
* Nov-19-14  GenPOS 2.2 R.Chambers  Source cleanup to remove old, before NCR 7448,
*                                   code that applied to NCR 2170 back in the 1990s.
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
/*
*===========================================================================
*   Retry Flag
*
*	Used with struct UieRetry.fchStatus in order to indicate conditions
*   as FSCs are processed.  See also functions UieCheckSeq(), UieClearRetryBuf(),
*   as well as function UieInitCheckSequence().
*===========================================================================
*/
#define UIE_ACCEPT          0x01            /* Accept Flag */
#define UIE_RETRY           0x02            /* Retry Flag */
#define UIE_RETRY_FULL      0x04            /* Retry Buffer Full */
#define UIE_CLEAR_RETRY_BUF 0x08            /* Clear Retry Buffer Flag */
#define UIE_SKIP_RETRY_WRT  0x10            /* Skip Retry Buffer Write */
#define UIE_DEF_PROC        0x20            /* Default Procedure Flag */


/*
*===========================================================================
*   Function Return Status
*===========================================================================
*/
#define UIE_FUNC_EXIT       0               /* Request to Function Exit */
#define UIE_FUNC_CONTINUE   1               /* Request to Function Continue */

#define UIE_END_CHAR        0x00            /* End Character Code */


/*
*===========================================================================
*   Define - Function 
*===========================================================================
*/
#define UIE_MAX_FUNC_TBL    64              /* Max Function Link Table, saratoga */
/* #define UIE_MAX_FUNC_TBL    32              / Max Function Link Table */

#define UIE_MAX_FUNC        3               /* Normal / Dialog 1 / Dialog 2 (sub dialog) */   

#define UIE_MAX_RETRY_BUF   256             /* Retry Buffer, see also UIE_MAX_KEY_BUF */

#define UIE_LINK_EMPTY      0xFF            /* not Use to Function Link Table */


/*
*===========================================================================
*   Define - Error Code
*===========================================================================
*/
#define UIE_SUC_KEY_SEQ     0
#define UIE_ERR_KEY_SEQ     -100
#define UIE_SUC_KEY_TRANSPARENT     -500
#define UIE_SUC_KEY_IGNORE          -501


/*
*===========================================================================
*   Receipt/Journal Feed
*===========================================================================
*/
/*
===== Quation =====
UIESEQ.H and UIEERROR.H should be the same as this definition.
===== Quation =====
*/
#if !defined(UIE_RECEIPT_FEED_ROW)
#define UIE_RECEIPT_FEED_ROW    8           /* Number of Receipt Feed Lines */
#define UIE_JOURNAL_FEED_ROW    4           /* Number of Journal Feed Line */
#endif


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

/*
*===========================================================================
*   Structure Type Define - FUNCLINK
*===========================================================================
*/
typedef struct {
    UCHAR  iuchParent;                      /* Parent Pointer */
    UCHAR  iuchChild;                       /* Child Pointer */
    UCHAR  iuchOBro;                        /* Old Brother Pointer */
    UCHAR  iuchYBro;                        /* Young Pointer */
    UIMENU CONST *pFunction;                /* Function Pointe  */
    UISEQ  CONST *pSequence;                /* Key Sequence Pointr */
} UIEFUNCLINK;


/*
*===========================================================================
*   Structure Define - Retry Status 
*===========================================================================
*/
typedef struct {
    UCHAR iuchWrite;                        /* Write Index */
    UCHAR iuchRead;                         /* Read Index */
    UCHAR fchStatus;                        /* Bit 0 - Accept Flag */
                                            /* Bit 1 - Retry Flag */
                                            /* Bit 2 - Retry Buffer Full Flag */
                                            /* Bit 3 - Clear Retry Buffer Flag */
} UIERETRYSTS;


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
extern UIEFUNCLINK UieFuncTbl[UIE_MAX_FUNC_TBL];    /* Function Link Table */

extern UCHAR iuchUieRootFunc[UIE_MAX_FUNC];         /* Root Function Pointer */
extern UCHAR iuchUieActiveFunc[UIE_MAX_FUNC];       /* Active Function Pointer */
extern UCHAR iuchUieCurrentFunc[UIE_MAX_FUNC];      /* Current Function Pointer */

extern TCHAR UieRetryBuf[UIE_MAX_RETRY_BUF];        /* Retry Buffer */

extern UIERETRYSTS UieRetry;                        /* Retry Status */
                                                    /* Bit 0 - Accept Flag */
                                                    /* Bit 1 - Retry Flag */
                                                    /* Bit 2 - Retry Buffer Full Flag */
                                                    /* Bit 3 - Clear Retry Buffer Flag */

extern UCHAR iuchUieFuncSts;                        /* UIE_NORMAL / UIE_DIALOG */


/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/
SHORT   UieCheckSeq(KEYMSG *pData);
SHORT   UieFindSeq(KEYMSG *pData, UCHAR iuchCurrent, UCHAR iuchExclude);
SHORT   UieCurrentFunc(KEYMSG *pData);
SHORT   UieRetryFunc(KEYMSG *pData);
SHORT   UieCheckRetryBuf(UCHAR iuchCurrent);
VOID    UieSendCancel(UCHAR iuchCurrent);
SHORT   UieSendRedisplay(USHORT usProgress);

VOID    UieGetRetryBuf(KEYMSG *pData);
VOID    UieWriteRetryBuf(KEYMSG *pData);
VOID    UieClearRetryBuf(VOID);

/* ---------- End of UIESEQ.H ---------- */
