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
* Title       : Enhanced Kitchen Display System , Local Header File
* Category    : Enhanced Kitchen Display System, NCR 2170 HOSP US ENH. Application
* Program Name: KdsLocal.H
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
* Jan-21-03: 02.00.00 : E.Smith		: SR 14 Expand KDS to support 8 Devices
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

#define     KDS_SPOOL_SIZE      256             /* spool buffer size, number of KDSRING items  */

#define     KDS_NUM_CRT         8				/* SR 14 ESMITH */

#define     KDS_MAX_SEQNO       99999L          // KDS_MSN_LEN number of digits

/* -- fbKdsControl -- */
#define     KDS_ALREADY_DISP     0x01           /* already disp. error flag */

#define     KDS_CRT1             0x0001
#define     KDS_CRT2             0x0002
#define     KDS_CRT3             0x0040
#define     KDS_CRT4             0x0080

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
    ULONG   ulSeqNo;
    KDSINF  KdsInf;
} KDSRING;

typedef struct {
    UCHAR  ausIPAddr[PIF_LEN_IP];
    USHORT  usPortNo;	//Port Address to talk to KDS from NHPOS
	USHORT  usFPortNo;	//Port Address to talk to NHPOS from KDS
    SHORT   sHandle;    // handle to the network interface
} KDSIPADDR;

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
/* -- enhkds.c -- */
VOID   KdsAtoL(LONG *plDest, UCHAR *pszStr, USHORT usLen);
SHORT  KdsWriteSpool(KDSINF *pData);
VOID   KdsItoA(USHORT usSorc,  UCHAR *pszStr, USHORT usStrLen);
VOID   KdsLtoA(ULONG ulSorc,  UCHAR *pszStr, USHORT usStrLen);
VOID    KdsConvData(KDSSENDDATA *pData, KDSINF *pInf);
UCHAR KdsCalcCRC(UCHAR *puchData, USHORT usLen);
USHORT KdsGetTerminalNo(VOID);

/* -- enhkdsin.c -- */
VOID THREADENTRY  KdsSendProcess(USHORT usSendPointer);	/* 12/21/01 */
SHORT  KdsSend(USHORT usKds, KDSINF *pData);
VOID THREADENTRY  KdsRecvProcess(USHORT usData);
VOID   KdsErrorDisp(VOID);
SHORT KdsNetOpen(USHORT usKds);
SHORT KdsNetSend(USHORT usKds, KDSINF *pKdsInf);
SHORT KdsNetReceive(USHORT usKds, KDSRESINF *pKdsResInf, USHORT *pusKds);
/* SHORT KdsNetReceive(USHORT usKds, KDSINF *pKdsInf, USHORT *pusKds); */
VOID KdsNetClose(USHORT usKds);

/* ---------- End of DflLocal.H ---------- */
