/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server STUB module, Header file for user                        
*   Category           : Client/Server STUB CPM (Charge Post) interface
*   Program Name       : CSSTBCPM.H                                            
*  ------------------------------------------------------------------------  
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.              
*   Memory Model       : Midium Model
*   Options            :  /c /AM /G1s /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*    Update Histories  :
*
*    Date           :NAME               :Description
*    Oct-28-93      :H.Yamaguchi        :Initial
*
** NCR2171 **
*
*    Oct-05-99      :M.Teraki           :initial (for 2171)
*
\*=======================================================================*/
/*=======================================================================*\
:   PVCS ENTRY
:-------------------------------------------------------------------------
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

#if !defined(CSSTBCPM_H_INCLUDED)

#define CSSTBCPM_H_INCLUDED


/*
===========================================================================
    DEFINE
===========================================================================
*/

//**  NOTE: DEPRECATED FUNCTIONALITY.
// 
// CPM was the server that handled a version of in house electronic payments in the NHPOS 2170 for
// hotels and other installations with a Charge Post application tied to something such as a room number.
// This functionality is no longer supported.  See function SerRecvCpm().
//     Richard Chambers, Jul-15-2025

/*  Reset Tally 128 */
/*  EPT   Tally   1 */ 
/*  CPM   Tally   2 */ 

#define CLI_READ_TLY_BOTH      (1+2)        /* Read  CPM/EPT Tally  */
#define CLI_READ_TLY_EPT       (1)          /* Read  EPT     Tally  */
#define CLI_READ_TLY_CPM       (2)          /* Read  CPM     Tally  */
#define CLI_RESET_TLY_BOTH     (1+2+128)    /* Reset CPM/EPT Tally  */
#define CLI_RESET_TLY_EPT      (1+128)      /* Reset EPT     Tally  */
#define CLI_RESET_TLY_CPM      (2+128)      /* Reset CPM     Tally  */

#define CLI_MAX_CPMDATA        (400)    /* Maximumm CPM data Modifed for new Charge Post IF */

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
	Charge Posting Request Message (Request)
------------------------------------------*/
typedef struct {
	USHORT	usFunCode;
	USHORT	usSeqNo;
	USHORT  usCpmReqNo;
	USHORT   usSndLen;
	UCHAR   auchCpmData[CLI_MAX_CPMDATA];
} CLIREQCPM;

/*-------------------------------------------
	Charge Posting Request Message (Response)
-------------------------------------------*/
typedef struct {
	USHORT	usFunCode;                     
	USHORT	usSeqNo;
	SHORT	sResCode;
    SHORT   sReturn;                       /* Set by CPM Recv Process */
	USHORT  usCpmReqNo;                    
	USHORT  usRetLen;                      /* Set by CPM Recv Process */
	UCHAR   auchCpmData[CLI_MAX_CPMDATA];  /* Set by CPM Recv Process */
} CLIRESCPM;

/*------------------------------------------
	Read/Reset Tally Request Message 
------------------------------------------*/
typedef struct {
	USHORT	usFunCode;
	USHORT	usSeqNo;
	USHORT  usCpmReqNo;
	UCHAR   uchType;
} CLIREQCPM_TLY;

/*============================================================
    CPM Send Buffer for CPM Receive Process 
============================================================*/
typedef struct {
        XGHEADER    CpmMesHed;    /* Header 8 bytes */
        CLIRESCPM   CpmResDat;    /* Response Data  from Host computer */
}CPMSENDMES;

/*============================================================
    CPM Send Buffer for CPM Receive Process 
============================================================*/
typedef struct {
        CPMTALLY    CpmTally;    /* CPM Tally  */
        EPTTALLY    EptTally;    /* EPT Tally  */
}CPMEPT_TALLY;


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
    USER I/F DFL FUNCTION (CLIENT)
----------------------------------------*/
/* For register Mode */

// following two functions were the interface for sending and receiving CPM messages.
// CPM was the server that handled a version of in house electronic payments in the NHPOS 2170 for
// hotels and other installations with a Charge Post application tied to something such as a room number.
// This functionality is no longer supported.  See function SerRecvCpm().
//     Richard Chambers, Jul-15-2025
//SHORT   CliCpmSendMessage(UCHAR  *pSendData, USHORT usSendLen);      /* Set by Reg Appli. */
//SHORT   CliCpmReceiveMessage(UCHAR  *pRecData, USHORT usRecSize);   /* Set by Reg Appli. */

UCHAR   CliCpmGetAsMasUaddr(VOID);                /* Get As Master terminal address */

/* For Supervisor Mode */
SHORT   SerCpmChangeStatus(UCHAR uchState);        /* Set by Sup Appli. */
SHORT   SerCpmEptReadResetTally(UCHAR uchType, CPMEPT_TALLY *pTally);    /* Read Tally        */

/*-----------------------------
    Internal Function
-----------------------------*/
USHORT    CstCpmGetUpdNo(VOID);

/*
===========================================================================
    MEMORY
===========================================================================
*/
extern CPMSENDMES    CpmSndBuf;                   /* Send Buffer */    

#endif

/*===== END OF DEFINITION =====*/
