#if defined(POSSIBLE_DEAD_CODE)
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
*    Sep-02-98      :A.Mitsui           :V3.3 Initial
*
*    Jan-24-00      :K.Yanagida         :Saratoga Initial
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
SHORT   CliCheckAsMaster(VOID);
SHORT   CliCreateFile(UCHAR uchType, USHORT usSize, BOOL fPTD);
/* SHORT   CliCheckCreateFile(UCHAR uchType, USHORT usSize, BOOL fPTD); */
/* SHORT   CliCheckDeleteFile(UCHAR uchType, USHORT usSize, BOOL fPTD); */
/* VOID    CliDispDescriptor(USHORT usModleID, USHORT usContDes);       */

/*SHORT   CliCreateFile(UCHAR uchType, ULONG ulSize, BOOL fPTD);          * Saratoga */
SHORT   CliCheckCreateFile(UCHAR uchType, ULONG ulSize, BOOL fPTD);     /* Saratoga */
SHORT   CliCheckDeleteFile(UCHAR uchType, ULONG ulSize, BOOL fPTD);     /* Saratoga */
VOID    CliDispDescriptor(USHORT usModleID, USHORT usContDes);

SHORT   CstReqOpePara(USHORT usType,
                      UCHAR *puchReqBuf,
                      USHORT usSize,
                      UCHAR *puchRcvBuf,
                      USHORT *pusRcvLen);

VOID    CliSleep(VOID); /* R3.3 */

/*------------------------------
    STUB I/F BACK UP FUNCTION
-------------------------------*/

SHORT   SstReqBackUp(USHORT usType,
                     UCHAR *puchReqBuf,
                     USHORT usSize,
                     UCHAR *puchRcvBuf,
                     USHORT *pusRcvLen);

/* SHORT    SstReqBackUpFH(USHORT usType,                         */
/*                      UCHAR *puchReqBuf,    */
/*                      USHORT usSize,        */
/*                        SHORT hsFileHandle,                         */
/*                      USHORT *pusRcvLen);   */

/*===== END OF DEFINITION =====*/

#endif