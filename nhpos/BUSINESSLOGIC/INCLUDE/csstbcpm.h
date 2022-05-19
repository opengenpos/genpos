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
*   Category           : Client/Server STUB CPM interface
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

/*
===========================================================================
    DEFINE
===========================================================================
*/

/*  Reset Tally 128 */
/*  EPT   Tally   1 */ 
/*  CPM   Tally   2 */ 

#define CLI_READ_TLY_BOTH      (1+2)        /* Read  CPM/EPT Tally  */
#define CLI_READ_TLY_EPT       (1)          /* Read  EPT     Tally  */
#define CLI_READ_TLY_CPM       (2)          /* Read  CPM     Tally  */
#define CLI_RESET_TLY_BOTH     (1+2+128)    /* Reset CPM/EPT Tally  */
#define CLI_RESET_TLY_EPT      (1+128)      /* Reset EPT     Tally  */
#define CLI_RESET_TLY_CPM      (2+128)      /* Reset CPM     Tally  */

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

SHORT   CliCpmSendMessage(UCHAR  *pSendData,      /* Set by Reg Appli. */
                          USHORT usSendLen);      /* Set by Reg Appli. */

SHORT   CliCpmReceiveMessage(UCHAR  *pRecData,    /* Set by Reg Appli. */
                             USHORT usRecSize);   /* Set by Reg Appli. */

UCHAR   CliCpmGetAsMasUaddr(VOID);                /* Get As Master terminal address */

/* For Supervisor Mode */

SHORT   SerCpmChangeStatus(UCHAR uchState);        /* Set by Sup Appli. */

SHORT   SerCpmEptReadResetTally(UCHAR        uchType,     /* Read Tally        */
                                CPMEPT_TALLY *pTally);

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

/*===== END OF DEFINITION =====*/
