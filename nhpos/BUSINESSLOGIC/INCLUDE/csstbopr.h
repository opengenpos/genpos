/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO   ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1999      ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server STUB module, Header file for user
*   Category           : Client/Server STUB Operational Parameter interface
*   Program Name       : CSSTBOPR.H
*  ------------------------------------------------------------------------
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.
*   Memory Model       : Medium Model
*   Options            :  /c /AM /G1s /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*    Update Histories  :
*
*    Date           :NAME               :Description
*    May-21-92      :H.NAKASHIMA        :Initial
*    Feb-28-95      :hkato              :R3.0
*    Feb-01-96      :T.Nakahata         :R3.1 Initial (Add PPI File)
* 
** NCR2171 **
*
*    Oct-05-99      :M.Teraki           : initial (for Win32)
*    Dec-17-99      :hrkato             : Saratoga
\*=======================================================================*/
/*=======================================================================*\
:   PVCS ENTRY
:-------------------------------------------------------------------------
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

#if !defined(CSSTBOPR_H_INCLUDED)
#define  CSSTBOPR_H_INCLUDED

/*
===========================================================================
    DEFINE
===========================================================================
*/
// Any changes to the following constants need to be coordinated with the constants
// in file uiccom.h which contains similar constants used with function CliReqOpPara().
#define     CLI_LOAD_ALL    1           /* Load All Parameters, same as UIC_EXECALL */
#define     CLI_LOAD_SUP    2           /* Load Super Parameters, same as UIC_EXECSUPER */
#define     CLI_LOAD_PLU    3           /* Load PLU Parameters, same as UIC_EXECPLU */
#define     CLI_LOAD_LAY    4           /* Load Layout Files, same as UIC_EXECLAYOUT */ //ESMITH LAYOUT
#define     CLI_LOAD_PRINTER_LOGOS    5     /* Load printer logo Files, same as UIC_EXECPRINTER_LOGOS */
#define     CLI_LOAD_ADDED_PARAM      6     /* Load PARAM Files such as PARAMINI, same as UIC_EXECPRINTER_LOGOS */

#define     CLI_LOAD_AC75_ARG_LAY    12       // argument for AC75 to transfer layouts as well.


/*
===========================================================================
    TYPEDEF
===========================================================================
*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif


/*------------------------------------------
    Broadcast Information Structure
-------------------------------------------*/
typedef struct {
    USHORT  usBcasRegNo;            /* register # (Prog #7) */    
    UCHAR   uchBcasStatus;          /* 0: offline, 1: success, 2: timeout */
} CLIOPBCAS;

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
/*
===========================================================================
    PROTOTYPE
===========================================================================
*/
SHORT   OpUnjoinCluster (TCHAR *pwszHostName, UCHAR *pauchHostIpAddress, UCHAR *pauchTerminalNo);

/*---------------------------------------
    USER I/F OP. PARA. FUNCTION (CLIENT)
----------------------------------------*/

SHORT   CliReqOpPara(USHORT usType);
SHORT   CliOpPluIndRead(PLUIF *pPif, USHORT usLockHnd);
SHORT   CliOpDeptIndRead(DEPTIF *pDpi, USHORT usLockHnd);
SHORT   CliOpCpnIndRead(CPNIF *pCpn, USHORT usLockHnd);       /* R3.0 */
        /* Added ucNumOfPLUrequested to hold how many items requested	- CSMALL */
SHORT   CliOpOepPluRead(PLUIF_OEP *pPlu, USHORT usLockHnd, UCHAR ucNumOfPLUrequested);
SHORT   CliOpOepPluReadMultiTable(USHORT usTableNo, USHORT usFilePart, PLUIF_OEP *pPlu, USHORT usLockHnd, UCHAR ucNumOfPLUrequested);

SHORT   CliOpJoinCluster(TCHAR *pwszHostName, UCHAR *pauchHostIpAddress, UCHAR *pauchTerminalNo);
SHORT   CliOpUnjoinCluster(TCHAR *pwszHostName, UCHAR *pauchHostIpAddress, UCHAR *pauchTerminalNo);
SHORT   CliOpTransferTotals(USHORT usTerminalPosition);
SHORT   CliOpUpdateTotalsMsg(VOID  *pTtlMessage);  // TTLMESSAGEUNION  *pTtlMessage

SHORT   CliOpConnEngine (const TCHAR *pwszConnEngineRequest, LONG tcBufferSize);
//SHORT   CliOpConnEngineObject (AllObjects  *pAllObjects);
SHORT   CliOpConnEngineObject (/* AllObjects */ VOID  *pAllObjects);
SHORT   CliOpConnEngineFH (SHORT  hsFileHandle, USHORT usSize);

SHORT   CliOpCstrIndRead(CSTRIF *pCstr, USHORT usLockHnd);    /* R3.0 */
/* === New functions - PPI File (Release 3.1) BEGIN === */
SHORT   CliOpPpiIndRead( PPIIF *pPpi, USHORT usLockHnd );   /* R3.1 */
/* === New functions - PPI File (Release 3.1) END === */
SHORT   CliOpMldIndRead( MLDIF *pMld, USHORT usLockHnd );   /* V3.3 */

/*---------------------------------------
    USER I/F OP. PARA. FUNCTION (SERVER)
----------------------------------------*/

SHORT   SerChangeOpPara(USHORT usType, CLIOPBCAS *pBcasRegNo);
SHORT   SerOpDeptAssign(DEPTIF *pDpi, USHORT usLockHnd);
SHORT   SerOpCpnAssign(CPNIF *pCpni, USHORT usLockHnd);       /* R3.0 */
SHORT   SerOpPluFileUpdate(USHORT usLockHnd);
#if 0
#pragma message("  ====++++====   SerOpPluAllRead_Debug() is ENABLED    ====++++====\z")
SHORT   SerOpPluAllRead_Debug(PLUIF *pPif, USHORT usLockHnd, char *aszFilePath, int nLineNo);
#define SerOpPluAllRead(pPif,usLock) SerOpPluAllRead_Debug(pPif,usLock,__FILE__,__LINE__)
#else
SHORT   SerOpPluAllRead(PLUIF *pPif, USHORT usLockHnd);
#endif
SHORT   SerOpRprtPluRead(PLUIF_REP *pPrp, USHORT usLockHnd);
SHORT   SerOpDeptPluRead(PLUIF_DEP *pPdp, USHORT usLockHnd);
SHORT   SerOpPluAbort(USHORT husHandle, USHORT usLockHnd);  /* saratoga */
SHORT   SerOpCstrAssign(CSTRIF *pPdp, USHORT usLockHnd);
SHORT   SerOpCstrDelete(CSTRIF *pPdp, USHORT usLockHnd);
SHORT   SerOpLock(VOID);
VOID   SerOpUnLock(VOID);
SHORT   SerOpMajorDeptRead(MDEPTIF *pDpi, USHORT usLockHnd);
/* === New functions - PPI File (Release 3.1) BEGIN === */
SHORT   SerOpPpiAssign(PPIIF *pPpi, USHORT usLockHnd);      /* R3.1 */
SHORT   SerOpPpiDelete(PPIIF *pPpi, USHORT usLockHnd);      /* R3.1 */
/* === New functions - PPI File (Release 3.1) END === */
SHORT   SerOpMldAssign(MLDIF *pMld, USHORT usLockHnd);      /* V3.3 */

SHORT   SerOpDeptDelete(DEPTIF *pDpi, USHORT usLockHnd); /* 2172 */
SHORT   SerOpDeptAllRead(DEPTIF *pDpi, USHORT usLockHnd); /* 2172 */

SHORT   SerPluEnterCritMode(USHORT usFile, USHORT usFunc, USHORT *pusHandle, VOID *pvParam);
SHORT   CstPluEnterCritMode(USHORT usFile, USHORT usFunc, 
                                USHORT *pusHandle, VOID *pvParam, USHORT usUser);
SHORT   SerPluExitCritMode(USHORT usHandle, USHORT usReserve);
SHORT   CstPluExitCritMode(USHORT usHandle, USHORT usReserve, USHORT usUser);
USHORT  CstPluGetBMHandle(USHORT usHandle);
SHORT   CliOpPluAssign(PLUIF *pPlu, USHORT usLockHnd);
SHORT   CliOpPluDelete(PLUIF *pPlu, USHORT usLockHnd);
SHORT   SerOpPluAssign(PLUIF *pPlu, USHORT usLockHnd);
SHORT   SerOpPluDelete(PLUIF *pPlu, USHORT usLockHnd);
SHORT   SerIspPluEnterCritMode(USHORT usFile, USHORT usFunc, 
                               USHORT *pusHandle, VOID *pvParam);           /* Saratoga */
SHORT   SerIspPluExitCritMode(USHORT usHandle, USHORT usReserve);           /* Saratoga */
SHORT   SerPluReportRecord(USHORT usHandle, VOID *pvBuffer, USHORT *pusRet);/* Saratoga */    

SHORT   CliOpPluOepRead(OPPLUOEPIF *pPlu, USHORT usLockHnd);
SHORT  CliReqSystemStatus(USHORT *pusCstComReadStatus);
#endif
/*===== END OF DEFINITION =====*/