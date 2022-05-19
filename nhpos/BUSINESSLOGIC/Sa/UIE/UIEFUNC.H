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
* Program Name: UIEFUNC.H
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Meduim/Large Model
* Options     : /c /A[M|Lw] /W4 /G[1|2]s /Os /Zap
* --------------------------------------------------------------------------
* Abstract:     This header file defines User Interface Engine the 2170
*               system application.
*
*               This file contains:
*                   1. Data type Definitions
*                   2. Function Prototypes
* --------------------------------------------------------------------------
* Update Histories
*
* Date       Ver.      Name         Description
* Mar-20-92  00.00.01  O.Nakada     Initial
* Sep-16-93  E1        O.Nakada     Divided the User Interface Engine.
* Oct-12-93  E1        O.Nakada     Apply a function to Round-Trip Service.
*
*** NCR2172 ***
* Oct-05-99  01.00.00  M.Teraki     Added #pragma pack(...)
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
*   Structure Type Define Declarations
*===========================================================================
*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

/*
*---------------------------------------------------------------------------
*   Structure Define - Sub Message
*   This struct is used in a variety of places as a shortened version of KEYMSG
*   in order to call a UIE menu function with a specific message id that is
*   meant for managing the menu such as UIM_INIT, UIM_REDISPLAY, etc. which only
*   need the message type and a function name identifier.
*---------------------------------------------------------------------------
*/
typedef struct {                /* Input Device Message */
    UCHAR   uchMsg;             /* Message Type, UIM_INIT etc. */
    USHORT  usFunc;             /* Function Name ID, same offset as SEL.usFunc */
} UIESUBMSG;



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
SHORT UieQuitFunc(VOID);
VOID  UieInitLinkTbl(VOID);
UCHAR UieCheckFuncTbl(USHORT usFaultCode);
VOID  UieDeleteFunc(UCHAR iuchExit, UCHAR iuchExclude);
VOID UieDeleteRoot2Crt(UIESUBMSG *SubMsg, UCHAR iuchCurrent);



/* ---------- End of UIEFUNC.H ----------*/
