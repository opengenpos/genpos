/*
* ---------------------------------------------------------------------------
* Title         :   Header File of Common Routines for Terminal Communication
* Category      :   Transfer, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   TRATCOMM.H
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All rights reserved.
* --------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /c /AL /W4 /G2s /Zp
* --------------------------------------------------------------------------
* Abstract:
*
*
* --------------------------------------------------------------------------
* Update Histories
*
* Date          Ver.       Name             Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
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
/*
*===========================================================================
*   Misc
*===========================================================================
*/
#define TRANS_TERM_COMM_MODEM_WAIT  10000       /* 10 sec */

/*
*===========================================================================
*   Return Status
*===========================================================================
*/
#define TRANS_TERM_COMM_SUCCESSFUL            (1)
#define TRANS_TERM_COMM_ABORT_SYSTEM          (-1000)
#define TRANS_TERM_COMM_ABORT_USER            (-1001)
#define TRANS_TERM_COMM_ABORT_LOCAL           (-1002)
#define TRANS_TERM_COMM_ABORT_REMOTE          (-1003)
#define TRANS_TERM_COMM_ABORT_LOCK            (-1004)
#define TRANS_TERM_COMM_ABORT_WORK_AS         (-1005)


/*
*===========================================================================
*   Transfer value definition
*===========================================================================
*/
#define TRANS_TERM_COMM_NO_PROG    30       /* number of para class(setup) *//* Saratoga 23->24 */
#define TRANS_TERM_COMM_NO_ACT     21       /* number of para class(maint) *//* Saratoga 18->21 */

/*
*===========================================================================
*   IP Address Table
*===========================================================================
*/
#define TRANS_TERM_COMM_IP_LEN          4


/*
*===========================================================================
*   Structure Type Define Declarations
*===========================================================================
*/


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
/* ===== TRATCOMM.C ===== */
short TransTermCommPortOpen(HWND);
short TransTermCommPortClose(HWND);
short TransTermCommModemConnect(HWND);
short TransTermCommModemDisconnect(HWND);
short TransTermCommLogon(HWND);
short TransTermCommLogoff(HWND);
short TransTermCommLock(HWND);
short TransTermCommUnLock(HWND);
short TransTermCommBroadcast(HWND);
short TransTermCommLanOpen(HWND);   /* saratoga */
short TransTermCommLanClose(HWND);  /* saratoga */
short TransTermCommLanLogon(HWND);  /* saratoga */
short TransTermCommLanLogoff(HWND); /* saratoga */

/* ===== TRATSEND.C ===== */
short TransTermSend(HWND);
short TransTermSendLan(HWND);   /* saratoga */
short TransTermSendWarning(HWND);
short TransTermSendData(HWND);
short TransTermSendCheckTotal(HWND, SHORT, void *, void *);
short TransTermSendProg(HWND);
short TransTermSendAct(HWND);
short TransTermSendParaClass(HWND, UCHAR);
short TransTermSendParaReset(HWND, WORD);
short TransTermSendParaFlex(HWND, UCHAR);
short TransTermSendFile(HWND, WCHAR *, WORD, ULONG, WORD);       /* Saratoga */
short TransTermSendFileDir(HWND, WCHAR *, WORD,WORD);

/* ===== TRATRECE.C ===== */
short TransTermReceive(HWND);
short TransTermReceiveLan(HWND);    /* saratoga */
short TransTermReceiveWarning(HWND);
short TransTermReceiveData(HWND);
short TransTermReceiveProg(HWND);
short TransTermReceiveAct(HWND);
short TransTermReceiveParaClass(HWND, UCHAR);
SHORT TransTermReceiveParaFlex(HWND, UCHAR);
short TransTermReceiveFile(HWND, WCHAR *, WORD, ULONG, WORD);    /* Saratoga */
ULONG TransTermReceiveSizeOnTerm(WCHAR* ); //ESMITH LAYOUT
short TransTermReceiveFileDir(HWND hWnd, WCHAR *szDir, WORD wFlag, ULONG ulBlockSize, WORD wMessage);
/* ===== End of TRATCOMM.H ===== */

