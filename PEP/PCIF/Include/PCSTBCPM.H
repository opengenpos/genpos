#if POSSIBLE_DEAD_CODE
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
*   Program Name       : PCSTBCPM.H                                            
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
*    Date       :NAME       :Description
*    Oct-28-93  :H.Yamaguchi:Initial
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
    Read/Reset Tally Request Message 
------------------------------------------*/
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNo;
    USHORT  usCpmReqNo;
    UCHAR   uchType;
} CLIREQCPM_TLY;

/*-------------------------------------------
    Read/Reset Tally Response Message 
-------------------------------------------*/
typedef struct {
    USHORT  usFunCode;                     
    USHORT  usSeqNo;
    SHORT   sResCode;
    SHORT   sReturn;                       /* Set by CPM Recv Process */
    USHORT  usCpmReqNo;                    
    USHORT  usRetLen;                      /* Set by CPM Recv Process */
    UCHAR   auchCpmData[CLI_MAX_CPMDATA];  /* Set by CPM Recv Process */
} CLIRESCPM;

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

SHORT   STUBENTRY   SerCpmReadResetTally(UCHAR            uchType,     
                                         CPMEPT_TALLY FAR *pTally);
/*
===========================================================================
    MEMORY
===========================================================================
*/

/*===== END OF DEFINITION =====*/

#endif