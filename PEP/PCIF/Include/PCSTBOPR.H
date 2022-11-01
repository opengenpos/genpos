#if POSSIBLE_DEAD_CODE
/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server STUB module, Header file for user                        
*   Category           : Client/Server STUB Operational Parameter interface                                                
*   Program Name       : PCSTBOPR.H                                            
*  ------------------------------------------------------------------------  
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.              
*   Memory Model       : Midium Model
*   Options            :  /c /AM /Gs /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*    Update Histories  :
*
*    Date           :NAME               :Description
*    May-21-92      :H.NAKASHIMA        :Initial
*    Aug-10-95      :T.Koyama           :2170 R3.0
*    Sep-07-98      :O.Nakada           :Add V3.3
\*=======================================================================*/
/*=======================================================================*\
:   PVCS ENTRY
:-------------------------------------------------------------------------
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

/*
===========================================================================
    DEFINE
===========================================================================
*/

#define     CLI_LOAD_ALL    1           /* Load All Parameters */
#define     CLI_LOAD_SUP    2           /* Load Super Parameters */
#define     CLI_LOAD_PLU    3           /* Load PLU Parameters */



/*
===========================================================================
    TYPEDEF
===========================================================================
*/
/*------------------------------------------
    Broadcast Information Structure
-------------------------------------------*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

typedef struct {
    USHORT  usRegNo;                /* register # (Prog #7) */    
    UCHAR   uchStatus;              /* 0: offline, 1: success, 2: timeout */
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

/*---------------------------------------
    USER I/F OP. PARA. FUNCTION (CLIENT)
----------------------------------------*/


SHORT   STUBENTRY   CliOpPluIndRead(PLUIF FAR *pPif, USHORT usLockHnd);
SHORT   STUBENTRY   CliOpDeptIndRead(DEPTIF FAR *pDpi, USHORT usLockHnd);
SHORT   STUBENTRY   CliOpCpnIndRead(CPNIF FAR *pCpn, USHORT usLockHnd);       /* R3.0 */
SHORT   STUBENTRY   CliOpOepPluRead(PLUIF_OEP FAR *pPlu, USHORT usLockHnd);   /* R3.0 */
SHORT   STUBENTRY   CliOpCstrIndRead(CSTRIF FAR *pCstr, USHORT usLockHnd);    /* R3.0 */
/* === New functions - PPI File (Release 3.1) BEGIN === */
SHORT   STUBENTRY   CliOpPpiIndRead( PPIIF FAR *pPpi, USHORT usLockHnd );   /* R3.1 */
/* === New functions - PPI File (Release 3.1) END === */
SHORT   STUBENTRY   CliOpMldIndRead( MLDIF *pMld, USHORT usLockHnd );   /* V3.3 */

/*---------------------------------------
    USER I/F OP. PARA. FUNCTION (SERVER)
----------------------------------------*/

SHORT   STUBENTRY   SerOpDeptAssign(DEPTIF FAR *pDpi, USHORT usLockHnd);
SHORT   STUBENTRY   SerOpPluAssign(PLUIF FAR *pPif, USHORT usLockHnd);
SHORT   STUBENTRY   SerOpCpnAssign(CPNIF FAR *pCpni, USHORT usLockHnd);       /* R3.0 */
SHORT   STUBENTRY   SerOpPluDelete(PLUIF FAR *pPif, USHORT usLockHnd);
SHORT   STUBENTRY   SerOpPluFileUpdate(USHORT usLockHnd);
SHORT   STUBENTRY   SerOpPluAllRead(PLUIF FAR *pPif, USHORT usLockHnd);
SHORT   STUBENTRY   SerOpRprtPluRead(PLUIF_REP FAR *pPrp, USHORT usLockHnd);
SHORT   STUBENTRY   SerOpDeptPluRead(PLUIF_DEP FAR *pPdp, USHORT usLockHnd);
SHORT   STUBENTRY   SerOpCstrAssign(CSTRIF FAR *pPdp, USHORT usLockHnd);
SHORT   STUBENTRY   SerOpCstrDelete(CSTRIF FAR *pPdp, USHORT usLockHnd);
SHORT   STUBENTRY   SerOpLock(VOID);
SHORT   STUBENTRY   SerOpUnLock(VOID);
SHORT   STUBENTRY   SerOpMajorDeptRead(MDEPTIF FAR *pDpi, USHORT usLockHnd);
/* === New functions - PPI File (Release 3.1) BEGIN === */
SHORT   STUBENTRY   SerOpPpiAssign(PPIIF FAR *pPpi, USHORT usLockHnd);      /* R3.1 */
SHORT   STUBENTRY   SerOpPpiDelete(PPIIF FAR *pPpi, USHORT usLockHnd);      /* R3.1 */
/* === New functions - PPI File (Release 3.1) END === */
SHORT   STUBENTRY   SerOpMldAssign(MLDIF *pMld, USHORT usLockHnd);      /* V3.3 */
SHORT   STUBENTRY   SerOpDeptDelete(DEPTIF FAR* pPif, USHORT usLockHnd);    /* Saratoga */

/*===== END OF DEFINITION =====*/
#endif