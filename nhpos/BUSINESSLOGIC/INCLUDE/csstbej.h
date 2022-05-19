/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *                 NCR Corporation, E&M OISO    ||
||     @  *=*=*=*=*=*=*=*=*  0                  (C) Copyright, 1997      ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*=======================================================================*\
*   Title              : Client/Server STUB module, Header file for user
*   Category           : Client/Server STUB Electoric Journal interface
*   Program Name       : CSSTBEJ.H
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
*    Date       :Ver       :NAME         :Description
*    Feb-03-93  :00.00.01  :K.YOU        :Initial
*    Jun-07-93  :00.00.01  :H.Yamaguchi  :Adds function
*    Aug-09-95  :03.00.03  :M.Suzuki     :Add function
*    Nov-20-95  :03.01.00  :T.Nakahata   :R3.1 Initial (E/J Cluster Reset)
*    Mar-14-97  :02.00.04  :hrkato       :R2.0(Migration)
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
    USER I/F EJ FUNCTION (CLIENT)
----------------------------------------*/
SHORT   CliEjConsistencyCheck (VOID);
SHORT   CliEJCreat(USHORT usNoOfBlock, UCHAR fchFlag);
SHORT   CliEJCheckD(USHORT usNoOfBlock, UCHAR fchFlag);
SHORT   CliEJCheckC(USHORT usNoOfBlock, UCHAR fchFlag);
SHORT   CliEJWrite(EJT_HEADER *pCHeader, USHORT usWSize, UCHAR *pBuff, UCHAR uchType, UCHAR uchOption);
SHORT   CliEJReverse(EJ_READ *pEJRead, WCHAR *pBuff, UCHAR uchType);
UCHAR   CliEJGetFlag(VOID);
SHORT   CliEJGetStatus(VOID);

/* === Add E/J Cluster Reset (Release 3.1) BEGIN === */
SHORT   CliEJRead(UCHAR uchUAddr, EJ_READ *pEJRead, TCHAR *pBuff, UCHAR uchType);
SHORT   CliEJReadFile(UCHAR  uchUAddr, ULONG ulOffset, VOID *pData, ULONG ulDataLen, ULONG *pulRetLen);
SHORT   CliEJClear(UCHAR uchUAddr);
/* === Add E/J Cluster Reset (Release 3.1) END === */

SHORT   SerEJReadFile(ULONG ulOffset, VOID *pData,
                    ULONG ulDataLen, ULONG *pulRetLen);
SHORT   CliEJRead1Line(EJ_READ *pEJRead, TCHAR *pBuff, UCHAR uchType);          /* Add R3.0 */
SHORT   SerEJNoOfTrans(VOID);
SHORT   SerEJReadResetTransOver(ULONG *pulOffset,
                    VOID *pData, ULONG ulDataLen, ULONG *pulRetLen);
SHORT   SerEJReadResetTransStart(ULONG *pulOffset,
                    VOID *pData, ULONG ulDataLen, ULONG *pulRetLen, USHORT *pusNumTrans);
SHORT   SerEJReadResetTrans(ULONG *pulOffset, VOID *pData,
                    ULONG ulDataLen, ULONG *pulRetLen, USHORT *pusNumTtrans);
SHORT   SerEJReadResetTransEnd(USHORT usNumTrans);
SHORT   SerEJClear(VOID);

SHORT   CliEJLock(UCHAR uchUAddr, UCHAR uchLock);
SHORT   CliEJUnLock(UCHAR uchUAddr, UCHAR uchLock);

/******* End od Source ****************************/
