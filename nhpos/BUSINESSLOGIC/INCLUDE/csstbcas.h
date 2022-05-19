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
*   Title              : Client/Server STUB module, Header file for user                        
*   Category           : Client/Server STUB Cashier interface                                                
*   Program Name       : CSSTBCAS.H                                            
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
*    Dec-06-95      :M.OZAWA            :Add SerCasIndTermLock/SerCasIndTermUnLock/
*                                        SerCasAllTermLock/SerCasIndTermUnLock
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
    USER I/F CASHIER FUNCTION (CLIENT)
----------------------------------------*/

SHORT   CliCasSignIn(CASIF *Casif);
SHORT   CliCasSignOut(CASIF *Casif);
SHORT   CliCasPreSignOut(CASIF *Casif);
SHORT   CliCasOpenPickupLoan(CASIF *pCasif);        /* Saratoga */
SHORT   CliCasClosePickupLoan(CASIF *pCasif);       /* Saratoga */
SHORT   CliCasDelayedBalance(UCHAR uchTermNo, ULONG ulState);					//Delayed balance JHHJ
SHORT   CliCasAllIdRead(USHORT usRcvBufferSize, ULONG *auchRcvBuffer);
SHORT   CliCasIndRead(CASIF *pCasif);

SHORT   Cas_SignInCheckAgainstForcedOut(VOID);

/*---------------------------------------
    USER I/F CASHIER FUNCTION (SERVER)
----------------------------------------*/

SHORT   SerCasClose(ULONG ulCashierNo);
SHORT   SerCasAssign(CASIF *pCasif);
SHORT   SerCasDelete(ULONG ulCashierNo);
SHORT   SerCasSecretClr(ULONG ulCashierNo);
SHORT   SerCasAllIdRead(USHORT usRcvBufferSize, ULONG *aulRcvBuffer);
SHORT   SerCasIndRead(CASIF *pCasif);
SHORT   SerCasIndLock(ULONG ulCashierNo);
SHORT   SerCasIndUnLock(ULONG ulCashierNo);
SHORT   SerCasAllLock(VOID);
SHORT   SerCasAllLockForceOutCheck(VOID);
VOID    SerCasAllUnLock(VOID);
SHORT   SerCasIndTermLock(USHORT usTerminalNo);     /* R3.1 */
SHORT   SerCasIndTermUnLock(USHORT usTerminalNo);   /* R3.1 */
SHORT   SerCasAllTermLock(VOID);                    /* R3.1 */
VOID    SerCasAllTermUnLock(VOID);                  /* R3.1 */
SHORT   SerCasChkSignIn(ULONG *aulRcvBuffer);
SHORT   SerCasOpenPickupLoan(CASIF *Casif);         /* Saratoga */
SHORT   SerCasClosePickupLoan(CASIF *Casif);        /* Saratoga */
SHORT   SerCasDelayedBalance(UCHAR uchTermNo, ULONG ulState);		//Delay Balance JHHJ


/*---------------------------------------
    INTERNAL CASHIER FUNCTION (CLIENT)
-----------------------------------------*/
SHORT   CstCasPreSignOut(CASIF *pCasif);    /* R3.3 */

/*===== END OF DEFINITION =====*/
