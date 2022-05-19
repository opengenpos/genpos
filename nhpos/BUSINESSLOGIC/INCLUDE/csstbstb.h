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
*   Category           : Client/Server STUB internal interface
*   Program Name       : CSSTBSTB.H                                            
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
*    Dec-14-99      :hrkato             :Saratoga
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
*--------------------------------------------------------------------------
*    Define Descriptor control for " COMM "
*--------------------------------------------------------------------------
*/
#if !defined(CSSTBSTB_H_INCLUDED)
#define CSSTBSTB_H_INCLUDED

#include <paraequ.h>
#include <para.h>

/*   Argument " CliDispDescriptor " function  */

#define CLI_DESC_OFF                    1   /* Descriptor OFF        */
#define CLI_DESC_START_ON               2   /* Descriptor ON start   */
#define CLI_DESC_STOP_ON                3   /* Descriptor ON stop    */
#define CLI_DESC_START_BLINK            4   /* Descriptor BLINK start */
#define CLI_DESC_STOP_BLINK             5   /* Descriptor BLINK stop  */

/*   Argument " IspDispDescriptor " function */

#define CLI_MODULE_STUB            0x0001   /* Stub       Module ID */
#define CLI_MODULE_SERVER          0x0002   /* Server     Module ID */
#define CLI_MODULE_ISP             0x0004   /* ISP Server Module ID */
#define CLI_MODULE_ISPNOTPROVIDE   0xfbfb   /* ISP Server not provide */
                                               /* Reserved Bit 3/4/5/6/7 */
/*   Control descriptor   */

#define CLI_DESC_CONT_ON          0x000ff   /* Check ON     modlule  */
#define CLI_DESC_CONT_BLINK       0x0ff00   /* Check BLINK  modlule  */

/*------------------------------------------------
    DESCRIPTOR POSITION (Display Communication)
-------------------------------------------------*/
#define CLI_DESC_COMM                   0   /* descriptor position */

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

/*----------------------
    USER I/F STUB
----------------------*/

VOID    CliInitialize(USHORT usMode);
UCHAR   CliReadUAddr(VOID);
UCHAR   CliReadUAddrReal(VOID);
SHORT   CliCheckAsMaster(VOID);
SHORT   CliCreateFile(UCHAR uchType, ULONG ulSize, UCHAR uchPTD);       /* Saratoga */
SHORT   CliCheckCreateFile(UCHAR uchType, ULONG ulSize, UCHAR uchPTD);  /* Saratoga */
SHORT   CliCheckDeleteFile(UCHAR uchType, ULONG ulSize, UCHAR uchPTD);  /* Saratoga */
VOID    CliDispDescriptor(USHORT usModleID, USHORT usContDes);

SHORT   CstReqOpePara(USHORT usType, UCHAR *puchReqBuf, USHORT usSize, UCHAR *puchRcvBuf, USHORT *pusRcvLen);

VOID    CliSleep(VOID); /* R3.3 */
SHORT   CliIsMasterOrBMaster(VOID); /* Saratoga */

SHORT   CstIamMaster(VOID);
SHORT   CstIamBMaster(VOID);
SHORT   CstIamDisconnectedSatellite(VOID);
SHORT   CstIamDisconnectedUnjoinedSatellite(VOID);
SHORT   CstIamSatellite(VOID);

/*------------------------------
    STUB I/F BACK UP FUNCTION
-------------------------------*/

// The following #if is used to enable or disable
// functionality to issues NHPOS_ASSERT log entries when
// the function TtlConvertError () is called during NeighborhoodPOS
// execution.  This functionality provides a simple way to turn on the
// ability to have Assert logs created by calls to SstReqBackUp()
// that provide the file and line number in the file from whence the
// the function was called.  This can provide additional error debugging.
#if 0
// generate a warning so this place is easy to find from a compiler warning.
#pragma message("  ====++++====          SstReqBackUp_Debug() is ENABLED \z         ====++++====")
#define SstReqBackUp(usType, puchReqBuf, usSize, puchRcvBuf, pusRcvLen) SstReqBackUp_Debug(usType, puchReqBuf, usSize, puchRcvBuf, pusRcvLen, __FILE__, __LINE__)
USHORT  SstReqBackUp_Debug(USHORT usType,
                        VOID   *puchReqBuf,
                        USHORT usSize,
                        VOID   *puchRcvBuf,
                        USHORT *pusRcvLen, char *aszFilePath, int nLineNo);               /* Convert error code  */
#else
SHORT   SstReqBackUp(USHORT usType,
                        VOID   *puchReqBuf,
                        USHORT usSize,
                        VOID   *puchRcvBuf,
                        USHORT *pusRcvLen);

#endif
SHORT   SstReqBackUpFH(USHORT usType,
                        VOID   *puchReqBuf,
                        USHORT usSize,
                        SHORT hsFileHandle,
                        USHORT *pusRcvLen);
/*----------------------
    USER I/F STUB File ESMITH LAYOUT
----------------------*/
LONG CstReqReadFile(CONST USHORT* paszFileName, 
					 CONST CHAR* paszMode,
					 VOID *pBuffer, 
					 USHORT usBytes,
					 ULONG ulFilePosition);

LONG CstReqTransferFile(USHORT usTerminalPosition, CONST USHORT* paszFileName,
					 CONST CHAR* paszMode,
					 VOID *pBuffer, 
					 USHORT usBytes,
					 ULONG ulFilePosition);

SHORT CstErrCtl(SHORT sStubErr, LONG sPifErr);

SHORT CstOpRetrieveOperatorMessage(USHORT  *pusMessageBufferEnum);

#endif
/*===== END OF DEFINITION =====*/
