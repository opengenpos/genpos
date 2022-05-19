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
*   Category           : Client/Server STUB Total interface
*   Program Name       : CSSTBTTL.H                                            
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
*    Dec-09-93      :H.YAMAGUCHI        :Adds SerTtlReadWriteChkAndWait()
*    May-17-94      :Yoshiko.Jimbo      :Adds SerTtlWaiTotalCheck(),
*                                             SerTtlCasTotalCheck()
*    Nov-15-95      :T.Nakahata         :R3.1 Initial
*    Dec-03-99      :hrkato             :Saratoga
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
/*
===========================================================================
    TYPEDEF
===========================================================================
*/
/*
===========================================================================
    PROTOTYPE
===========================================================================
*/

/*---------------------------------------
    USER I/F TOTAL FUNCTION (CLIENT)
----------------------------------------*/

SHORT    CliTtlUpdateFile(UCHAR *puchSendBuf, USHORT usSize, UCHAR *puchDelayBalance);
SHORT    CliTtlUpdateFileFH(ULONG ulOffset, SHORT hsFileHandle, USHORT usSize);
SHORT    CliTtlTotalRead(VOID *pTotal);             /* Saratoga */
SHORT    CliTtlTotalReadIncr(VOID *pTotal);
SHORT    CliCasTtlTenderAmountRead(ULONG ulCashierNumber, VOID *pTtlCas);  /* Saratoga */
SHORT    CliRecvTtlGetLen(UCHAR uchMajorClass);

/*---------------------------------------
    USER I/F TOTAL FUNCTION (SERVER)
----------------------------------------*/

SHORT    SerTtlTotalRead(VOID *pTotal);
SHORT    SerTtlTotalIncrRead(VOID *pTotal);
SHORT    SerTtlTotalReset(VOID *pTotal, USHORT usMDCBit);
/* SHORT    SerTtlTotalReset(VOID *pTotal, UCHAR uchMDCBit); */
SHORT    SerTtlIssuedSet(VOID *pTotal, UCHAR fIssued);
SHORT    SerTtlIssuedCheck(VOID *pTotal, UCHAR fIssued);
SHORT    SerTtlIssuedCheckAllReset(VOID);
SHORT    SerTtlIssuedReset(VOID *pTotal, UCHAR fIssued);
SHORT    SerTtlTotalCheck(VOID *pTotal);
SHORT    SerTtlReadWriteChkAndWait(USHORT usTime);
SHORT    SerTtlWaiTotalCheck(VOID);
SHORT    SerTtlCasTotalCheck(VOID);

SHORT   SerTtlPLUOptimize(UCHAR uchMinorClass);	/* 06/26/01 */
SHORT   SerTtlTotalDelete(VOID *pTotal);
SHORT   SerTtlTotalReadPluEx(VOID *pTotal);
SHORT   SerTtlTumUpdateDelayedBalance(VOID);

VOID    SerTtlStartTransferredTotalsUpdate (USHORT usTerminalNo);
VOID  SerTtlStartUpdateTotalsMsg (USHORT  usTerminalPosition);

/*===== END OF DEFINITION =====*/