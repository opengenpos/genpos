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
*   Category           : Client/Server PC STUB Parameter interface
*   Program Name       : PCSTBPAR.H                                            
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

    NOTE:  New functions added to this library
         may also need to have an entry point
		 added to the file NCR2172Stub.def in
		 order to make the entry point visible.

         You will know to do this if you see
		 link errors with your new function specifed
		 as not found.

===========================================================================
*/

/*------------------------------
    USER I/F SYSTEM PARAMETER
-------------------------------*/

SHORT   STUBENTRY   CliParaAllRead(UCHAR uchClass,
                                    UCHAR FAR *puchRcvBuffer,
                                    USHORT usRcvBufLen,
                                    USHORT usStartPointer,
                                    USHORT FAR *pusReturnLen);
SHORT   STUBENTRY   CliParaAllWrite(UCHAR uchClass,
                                    UCHAR FAR *puchWrtBuffer,
                                    USHORT usRWrtBufLen,
                                    USHORT usStartPointer,
                                    USHORT FAR *pusReturnLen);

/*===== END OF DEFINITION =====*/

