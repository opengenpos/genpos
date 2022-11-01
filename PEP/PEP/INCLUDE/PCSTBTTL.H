/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server PC STUB module, Header file for user                        
*   Category           : Client/Server PC STUB Total interface
*   Program Name       : PCSTBTTL.H                                            
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
*
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
    USER I/F TOTAL FUNCTION (SERVER)
----------------------------------------*/

SHORT    STUBENTRY   SerTtlTotalRead(VOID FAR *pTotal);
SHORT    STUBENTRY   SerTtlTotalReset(VOID FAR *pTotal, UCHAR uchMDCBit);
SHORT    STUBENTRY   SerTtlIssuedSet(VOID FAR *pTotal, UCHAR fIssued);
SHORT    STUBENTRY   SerTtlIssuedCheck(VOID FAR *pTotal, UCHAR fIssued);
SHORT    STUBENTRY   SerTtlIssuedReset(VOID FAR *pTotal, UCHAR fIssued);
SHORT    STUBENTRY   SerTtlTotalCheck(VOID FAR *pTotal);
SHORT    STUBENTRY   SerTtlWaiTotalCheck(VOID);
SHORT    STUBENTRY   SerTtlCasTotalCheck(VOID);

/*===== END OF DEFINITION =====*/

