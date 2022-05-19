/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-9         ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server STUB module, Header file for user                        
*   Category           : Client/Server STUB Kitchin Printer interface
*   Program Name       : CSSTBKPS.H                                            
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
*    Dec-16-99      :hrkato             :Saratoga
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
    USER I/F K.P.S FUNCTION (CLIENT)
----------------------------------------*/

SHORT   CliKpsPrint(UCHAR *auchSndBuffer, USHORT usSndBufLen,
                    UCHAR uchUniqueAddress, USHORT *pusPrintStatus);
/* ### MOD 2172 Rel1.0 (Saratoga) */
/*SHORT   CliKpsPrintEx(UCHAR *auchSndBuffer, USHORT usSndBufLen,
                    UCHAR uchUniqueAddress, USHORT *pusPrintStatus, UCHAR uchTarget);*/
SHORT   CliKpsPrintEx(UCHAR *auchSndBuffer, USHORT usSndBufLen,
                    UCHAR uchUniqueAddress, USHORT *pusPrintStatus, UCHAR uchTarget,USHORT usOutPrinterInfo);

/*---------------------------------------
    USER I/F K.P.S FUNCTION (SERVER)
----------------------------------------*/

SHORT    SerAlternativeKP(UCHAR *puchSendBuf, USHORT usSize);
VOID     SerKpsTimerStopAll(VOID);

/*===== END OF DEFINITION =====*/
