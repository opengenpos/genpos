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
*   Category           : Client/Server STUB Back Up Copy interface                                                
*   Program Name       : CSSTBBAK.H                                            
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

/*------------------------------------------
    Define for Backup Files  
    Define for Backup MMM        Saratoga
------------------------------------------*/
/*  Define for error code  */

#define CLI_ERROR_BAK_COMM      -37
#define CLI_ERROR_BAK_FULL      -2

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
    USER I/F BACK UP FUNCTION (CLIENT)
----------------------------------------*/

SHORT   CliReqBackUp(VOID);

/*---------------------------------------
    USER I/F BACK UP FUNCTION (SERVER)
----------------------------------------*/

SHORT   SerStartBackUp(VOID);
SHORT   SerChangeInqStat(VOID);
SHORT   SerForceInqStatus (VOID);

// The following #if is used to enable or disable
// functionality to issues NHPOS_ASSERT log entries when
// the function CstConvertError () is called during NeighborhoodPOS
// execution.  This functionality provides a simple way to turn on the
// ability to have Assert logs created by calls to CstConvertError()
// that provide the file and line number in the file from whence the
// the function was called.  This can provide additional error debugging.
#if 0
// generate a warning so this place is easy to find from a compiler warning.
#pragma message("  ====++++====   CstConvertError_Debug() is ENABLED     ====++++==== \z")
#define CstConvertError(sError) CstConvertError_Debug(sError, __FILE__, __LINE__)
USHORT  CstConvertError_Debug(SHORT sError, char *aszFilePath, int nLineNo);               /* Convert error code  */
#else
USHORT   CstConvertError(SHORT sError);
#endif
        
/*-------------------------
    CSETKBAK.C 
--------------------------*/
SHORT   CliReqBackupETK(USHORT usFun, USHORT usFile);
SHORT   CliResBackupETK(UCHAR  *puchRcvData,
                        USHORT   usRcvLen,
                        UCHAR  *puchSndData,
                        USHORT  *pusSndLen);
/*-------------------------
    CSSTBOPR.C 
--------------------------*/
SHORT   CliReqBackupMMM(USHORT usFile, USHORT usFun);       /* Saratoga */
SHORT   CliResBackupMMM(UCHAR *puchRcvData,                 /* Saratoga */
                        USHORT usRcvLen, UCHAR *puchSndData, USHORT *pusSndLen);
// The following #if is used to enable or disable
// functionality to issues NHPOS_ASSERT log entries when
// the function CliConvertErrorMMM () is called during NeighborhoodPOS
// execution.  This functionality provides a simple way to turn on the
// ability to have Assert logs created by calls to CliConvertErrorMMM()
// that provide the file and line number in the file from whence the
// the function was called.  This can provide additional error debugging.
#if 0
// generate a warning so this place is easy to find from a compiler warning.
#pragma message("  ====++++====   CliConvertErrorMMM_Debug() is ENABLED    ====++++==== \z")
#define CliConvertErrorMMM(sError) CliConvertErrorMMM_Debug(sError, __FILE__, __LINE__)
USHORT CliConvertErrorMMM_Debug(SHORT sError, char *aszFilePath, int nLineNo);               /* Convert error code  */
#else
USHORT CliConvertErrorMMM(SHORT sError);               /* Convert error code  */
#endif

/*===== END OF DEFINITION =====*/