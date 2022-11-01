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
*   Category           : Client/Server PC STUB Guest Check File interface                                                
*   Program Name       : PCSTBGCF.H                                            
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
*    Aug-22-95      :M.Suzuki           : Add R3.0
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
    USER I/F G.C.F FUNCTION (SERVER)
----------------------------------------*/

SHORT    STUBENTRY   SerGusReadLock(USHORT usGCNumber,
                                    UCHAR *puchRcvBuffer, 
                                    USHORT usSize);
SHORT    STUBENTRY   SerGusDelete(USHORT usGCNumber);
SHORT    STUBENTRY   SerGusIndRead(USHORT usGCNumber,
                                    UCHAR *puchRcvBuffer, 
                                    USHORT usSize);
SHORT    STUBENTRY   SerGusAllIdRead(USHORT usType,
                                    USHORT FAR *pusRcvBuffer,
                                    USHORT usSize);
SHORT    STUBENTRY   SerGusAllLock(VOID);
SHORT    STUBENTRY   SerGusAllUnLock(VOID);
SHORT    STUBENTRY   SerGusAssignNo(USHORT usGCAssignNo);
SHORT    STUBENTRY   SerGusResetReadFlag(USHORT usGCNumber);
SHORT    STUBENTRY   SerGusCheckExist(VOID);
SHORT    STUBENTRY   SerGusAllIdReadBW(USHORT   usType,
                                        ULONG   ulWaiterNo,
                                        USHORT  *pusRcvBuffer,
                                        USHORT  usSize);
SHORT	 STUBENTRY SerGusResetDeliveryQueue(VOID);      /* add R3.0 */

/*===== END OF SOURCE =====*/


