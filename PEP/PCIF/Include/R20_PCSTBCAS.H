/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1996      ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server PC STUB module, Header file for user                        
*   Category           : Client/Server PC STUB Cashier interface                                                
*   Program Name       : PCSTBCAS.H                                            
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
*    May-07-96      :T.Nakahata         :R3.1 Initial
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
    USER I/F CASHIER FUNCTION (SERVER)
----------------------------------------*/

SHORT   STUBENTRY   SerCasClose(ULONG  ulCashierNo);
SHORT   STUBENTRY   SerCasAssign(CASIF  *Casif);
SHORT   STUBENTRY   SerCasDelete(ULONG  ulCashierNo);
SHORT   STUBENTRY   SerCasSecretClr(ULONG ulCashierNo);
SHORT   STUBENTRY   SerCasAllIdRead(USHORT usRcvBufferSize, ULONG *ausRcvBuffer);
SHORT   STUBENTRY   SerCasIndRead(CASIF  *Casif);
SHORT   STUBENTRY   SerCasIndLock(ULONG ulCashierNo);
SHORT   STUBENTRY   SerCasIndUnLock(ULONG ulCashierNo);
SHORT   STUBENTRY   SerCasAllLock(VOID);
VOID    STUBENTRY   SerCasAllUnLock(VOID);
SHORT   STUBENTRY   SerCasIndTermLock(USHORT usTerminalNo);     /* R3.1 */
SHORT   STUBENTRY   SerCasIndTermUnLock(USHORT usTerminalNo);   /* R3.1 */
SHORT   STUBENTRY   SerCasAllTermLock(VOID);                    /* R3.1 */
VOID    STUBENTRY   SerCasAllTermUnLock(VOID);                  /* R3.1 */
SHORT   STUBENTRY   SerCasChkSignIn(ULONG  *ausRcvBuffer);

SHORT   STUBENTRY   SerCasOpenPickupLoan(CASIF  *pCasif);    /* Saratoga */
SHORT   STUBENTRY   SerCasClosePickupLoan(CASIF  *pCasif); 	/* Saratoga */
SHORT   STUBENTRY	SerCasDelayedBalance(UCHAR uchTermNo); //Delayed Balance JHHJ

/*===== END OF DEFINITION =====*/
