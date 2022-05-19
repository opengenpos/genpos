
// The waiter functionality no longer exists in GenPOS however there are
// still some vestiges of when it was a part of NHPOS. This removal
// was done with NHPOS source code by NCR before the software was donated
// to Georgia Southern University in 2002. Or it may be that development
// was started and never completed by the Oiso, Japan team at NCR.
//
// I am now ifdefing out the contents of this include file, csstbwai.h, and the associated
// file cswai.h however there are a couple of defines that are still used in several
// parts of the GenPOS source which need to be removed at a later time.
//
// See also use of the define FLEX_WT_ADR which is the address in Flexible Memory
// for the Waiter File size. However this size is no longer used and should be
// a value of zero (0).
//
//    Richard Chambers, Georgia Southern University, Aug-10-2017

#if defined(POSSIBLE_DEAD_CODE)

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
*   Category           : Client/Server STUB Waiter interface
*   Program Name       : CSSTBWAI.H                                            
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
*    Dec-06-95      :M.OZAWA            :Add SerWaiIndTermLock/SerWaiIndTermUnLock
*                                        SerWaiAllTermLock/SerWaiAllTermUnLock
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
    USER I/F WAITER FUNCTION (CLIENT)
----------------------------------------*/

SHORT   CliWaiSignIn(WAIIF *Waiif);
SHORT   CliWaiSignOut(WAIIF *Waiif);
SHORT   CliWaiPreSignOut(WAIIF *Waiif);
SHORT   CliWaiIndRead(WAIIF *Waiif);

/*---------------------------------------
    USER I/F WAITER FUNCTION (SERVER)
----------------------------------------*/

SHORT   SerWaiClose(USHORT usWaiterNo);
SHORT   SerWaiAssign(WAIIF *Waiif);
SHORT   SerWaiDelete(USHORT usWaiterNo);
SHORT   SerWaiSecretClr(USHORT usWaiterNo);
SHORT   SerWaiAllIdRead(USHORT usRcvBufferSize, USHORT *ausRcvBuffer);
SHORT   SerWaiIndRead(WAIIF *Waiif);
SHORT   SerWaiIndLock(USHORT usWaiterNo);
SHORT   SerWaiIndUnLock(USHORT usWaiterNo);
SHORT   SerWaiAllLock(VOID);
VOID    SerWaiAllUnLock(VOID);
SHORT   SerWaiIndTermLock(USHORT usTerminalNo);     /* R3.1 */
SHORT   SerWaiIndTermUnLock(USHORT usTerminalNo);   /* R3.1 */
SHORT   SerWaiAllTermLock(VOID);                    /* R3.1 */
VOID    SerWaiAllTermUnLock(VOID);                  /* R3.1 */
SHORT   SerWaiChkSignIn(USHORT *ausRcvBuffer);

/*---------------------------------------
    INTERNAL WAITER FUNCTION (CLIENT)
----------------------------------------*/
SHORT   CstWaiSignIn(WAIIF *pWaiif);

/*===== END OF DEFINITION =====*/

#endif