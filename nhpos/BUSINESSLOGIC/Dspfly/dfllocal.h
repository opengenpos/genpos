/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*                                                 **
**        *  NCR 2170   *              NCR Corporation, E&M OISO          **
**     @  *=*=*=*=*=*=*=*  0             (C) Copyright, 1993              **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Display on the Fly, Local Header File
* Category    : Display on the Fly Manager, NCR 2170 HOSP US ENH. Application
* Program Name: DflLocal.H
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
*
*** NCR2172 ***
*
* Oct-04-99: 01.00.00 : M.Teraki    : Initial
*                                   : Add #pragma pack(...)
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

#define     DFL_SPOOL_SIZE       1000           /* spool buffer size */

/* -- fbDflControl -- */
#define     DFL_ALREADY_DISP     0x01           /* already disp. error flag */
#define     DFL_PORT_OPEN        0x02           /* already open port */

#define     DFL_SPOOL_FULL       0x01           /* buffer full flag */

#define     DFL_MAX_TERMNO         16           /* terminal number of 1 cluster */

#define     DFL_PORT_NONE          -1           /* chDflPortID: not exist 232 bort or not set device ID */

#define     DFL_SAME               -5           /* same as previous data */
                                    
#define     DFL_RETRY_COUNT         3           /* retry counter */

#define     DFL_NOT_LAST            0           
#define     DFL_LAST_TIME         -10           

#if ! defined(USE_2170_ORIGINAL)
#define     DFL_DEFAULT_SNDTIME     2000        /* default send time */
#define     DFL_DEFAULT_RECTIME     200         /* default recieve time */
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


typedef struct {
    USHORT  usVLI;
    UCHAR   uchTermAdr;
    USHORT  usConsNo;
    TCHAR   auchItemType[2];
    UCHAR   uchSeqNo;
} DFLDATAHEAD;


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
/* -- Dfl.c -- */
VOID    DflCheckTermAdr(USHORT usInfAdr);
VOID    DflGetPort(VOID);
VOID    DflAtoI(SHORT *psDest, TCHAR *pszStr, USHORT usLen);
//VOID    DflFarAtoI(SHORT *psDest, UCHAR FAR *pszStr, USHORT usLen);
SHORT   DflPrevDataCheck(DFLINTERFACE *pData, USHORT usInfAdr);
SHORT   DflWriteSpool(VOID *pData);
VOID    DflItoA(USHORT usSorc, TCHAR *pszStr, USHORT usStrLen);

/* -- DflIn.c -- */
VOID THREADENTRY DflSendProcess(VOID);
VOID   DflOpenSetPort(PROTOCOL *pProt, USHORT usTimerVal);
SHORT  DflSend1Data(SHORT  sState);
VOID   DflErrorDisp(VOID);
VOID   DflMoveReadP(USHORT usLen);
SHORT  DflWriteCom(SHORT sState, USHORT usMove, VOID FAR *pData, USHORT usDataLen);
SHORT  DflCheckComError(SHORT  sState, SHORT sRet, USHORT usMove, USHORT usSeciLen);
SHORT  DflReadACK(SHORT sState, USHORT usMove);
UCHAR  DflFarXor(VOID FAR *pStart, SHORT sLen);    /* V3.3 FVT */

#if ! defined(USE_2170_ORIGINAL)
SHORT   DflIsReady( SHORT sState, USHORT usMove );
#endif

/* ---------- End of DflLocal.H ---------- */