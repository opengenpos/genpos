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
    USER I/F CASHIER FUNCTION (SERVER)
----------------------------------------*/

SHORT   STUBENTRY   SerCasClose(USHORT usCashierNo);
SHORT   STUBENTRY   SerCasAssign(CASIF FAR *Casif);
SHORT   STUBENTRY   SerCasDelete(USHORT usCashierNo);
SHORT   STUBENTRY   SerCasSecretClr(USHORT usCashierNo);
SHORT   STUBENTRY   SerCasAllIdRead(USHORT usRcvBufferSize, USHORT FAR *ausRcvBuffer);
SHORT   STUBENTRY   SerCasIndRead(CASIF FAR *Casif);
SHORT   STUBENTRY   SerCasIndLock(USHORT usCashierNo);
SHORT   STUBENTRY   SerCasIndUnLock(USHORT usCashierNo);
SHORT   STUBENTRY   SerCasAllLock(VOID);
VOID    STUBENTRY   SerCasAllUnLock(VOID);
SHORT   STUBENTRY   SerCasChkSignIn(USHORT FAR *ausRcvBuffer);

/*===== END OF DEFINITION =====*/

