/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995           ||
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
*    Feb-28-95      :hkato              :R3.0
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

SHORT   CliParaAllRead(USHORT usClass,
                        VOID *puchRcvBuffer,
                        USHORT usRcvBufLen,
                        USHORT usStartPointer,
                        USHORT *pusReturnLen);
SHORT   CliParaAllWrite(USHORT usClass,
                        VOID *puchWrtBuffer,
                        USHORT usRWrtBufLen,
                        USHORT usStartPointer,
                        USHORT *pusReturnLen);
VOID    CliParaRead(VOID *pDATA);
VOID    CliParaWrite(VOID *pDATA);
SHORT   CliParaSupLevelRead(PARASUPLEVEL *pData);
SHORT   CliParaMenuPLURead(PARAMENUPLUTBL *pData);
VOID    CliParaMenuPLUWrite(UCHAR type, PARAMENUPLUTBL *pData);
VOID    CliParaOepRead( PARAOEPTBL *pData );
VOID    CliParaOepWrite( PARAOEPTBL *pData );
VOID	CliParaSaveFarData(VOID);

/*===== END OF DEFINITION =====*/
