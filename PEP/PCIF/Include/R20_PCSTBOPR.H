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

//----------------------------------------------------------------------------
// WARNING:  Following flags used with Operator Send Message must be
// synchronized with the flags as specified in file CSSTBOPR.H in NHPOS.
//
// flags used for setting options for function SerOpDisplayOperatorMessage()
// The macro SEROPDISPLAY_OPTIONS_SET() accepts on of the option masks and
// uses information in the mask to determine which byte of the flags to set
// as well as which bit in the mask to set.
// First byte is the byte offset and the second byte is the bit mask.
// For instance 0x0001 means byte offset 0, bit 1 of that byte
//              0x0104 means byte offset 1, bit 4 of that byte
#define SEROPDISPLAY_NO_BROADCAST    0x0001
#define SEROPDISPLAY_DELETE_MSGS     0x0002
#define SEROPDISPLAY_FORCE_SIGNOUT   0x0004
#define SEROPDISPLAY_AUTO_SIGNOUT    0x0008
#define SEROPDISPLAY_SIGNIN_BLOCK    0x0101
#define SEROPDISPLAY_SIGNIN_UNBLOCK  0x0102
#define SEROPDISPLAY_FORCE_SHUTDOWN  0x0104

#define  SEROPDISPLAY_OPTIONS_SET(op,ucFlags)   ((ucFlags)[((op)>>8)] |= ((op) & 0xff))
#define  SEROPDISPLAY_OPTIONS_UNSET(op,ucFlags) ((ucFlags)[((op)>>8)] &= ~((op) & 0xff))
#define  SEROPDISPLAY_OPTIONS_ISSET(op,ucFlags) ((ucFlags)[((op)>>8)] & ((op) & 0xff))

// Flags used with SerOpRetrieveSystemStatus() to return the system status
// these are the same flag codes used internall within the ISP thread of GenPOS.
#define ISP_COMSTS_M_UPDATE         0x0001    /* master U/D or O/D    */
#define ISP_COMSTS_BM_UPDATE        0x0002    /* B. master U/D or O/D */
#define ISP_COMSTS_M_OFFLINE        0x0004    /* master off-line      */
#define ISP_COMSTS_BM_OFFLINE       0x0008    /* B. master off-line   */
#define ISP_COMSTS_STOP             0x0020    /* STOP requet          */
#define ISP_COMSTS_INQUIRY          0x0040    /* inquiry status       */
#define ISP_COMSTS_INQTIMEOUT       0x0080    /* inquiry timeout      */
#define ISP_COMSTS_AC85             0x4000    /* Execute AC 85        */
#define ISP_COMSTS_AC42             0x8000    /* Execute AC 42        */


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

      NOTE:  New functions added to this library
         may also need to have an entry point
		 added to the file NCR2172Stub.def in
		 order to make the entry point visible.

         You will know to do this if you see
		 link errors with your new function specifed
		 as not found.

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
SHORT   STUBENTRY   CliOpMnemonicsIndRead( MLDIF *pMld, USHORT usLockHnd );   /* V3.3 */

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
SHORT   STUBENTRY   SerOpMnemonicsAssign(MLDIF *pMld, USHORT usLockHnd);      /* V3.3 */
SHORT   STUBENTRY   SerOpDeptDelete(DEPTIF FAR* pPif, USHORT usLockHnd);    /* Saratoga */
SHORT	STUBENTRY   SerOpDisplayOperatorMessage (UCHAR *ucFlags, WCHAR *pTextString, ULONG ulTextCharsCount);
SHORT	STUBENTRY   SerOpRetrieveSystemStatus (USHORT  *usSystemStatus);
SHORT	STUBENTRY   SerOpUpdateExistingPlu (PLUIF *pPif, USHORT usLockHnd);

/*===== END OF DEFINITION =====*/
