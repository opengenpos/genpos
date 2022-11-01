/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1998      ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server STUB module, Header file for user                        
*   Category           : Client/Server STUB Parameter interface
*   Program Name       : CSSTBPAR.H                                            
*  ------------------------------------------------------------------------  
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.              
*   Memory Model       : Medium Model
*   Options            :  /c /AM /G1s /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*    Update Histories  :
*
*    Date           :NAME               :Description
*    May-21-92      :H.NAKASHIMA        :Initial
*    Sep-02-98      :A.Mitsui           :V3.3 Initial
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

/*------------------------------
    USER I/F SYSTEM PARAMETER
-------------------------------*/

SHORT   CliParaAllRead(UCHAR uchClass,
                        UCHAR *puchRcvBuffer,
                        USHORT usRcvBufLen,
                        USHORT usStartPointer,
                        USHORT *pusReturnLen);
SHORT   CliParaAllWrite(UCHAR uchClass,
                        UCHAR *puchWrtBuffer,
                        USHORT usRWrtBufLen,
                        USHORT usStartPointer,
                        USHORT *pusReturnLen);
VOID    CliParaRead(VOID *pDATA);
VOID    CliParaWrite(VOID *pDATA);
UCHAR   CliParaMDCCheck(USHORT address, UCHAR field);
SHORT   CliParaSupLevelRead(PARASUPLEVEL *pData); 
SHORT   CliParaMenuPLURead(PARAMENUPLUTBL *pData); 
VOID    CliParaMenuPLUWrite(UCHAR type, PARAMENUPLUTBL *pData); 
VOID    CliParaOepRead( PARAOEPTBL *pData );
VOID    CliParaOepWrite( PARAOEPTBL *pData );

/*===== END OF DEFINITION =====*/
