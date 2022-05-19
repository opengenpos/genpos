/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1996       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Client/Server STUB, Electronic Journal Source File
* Category    : Client/Server STUB, Hospitality US Model
* Program Name: CSSTBEJ.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:
*
*               CliEJCreat()        Create EJ File
*               CliEJCheckD()       Check and Delete EJ File
*               CliEJCheckC()       Check and Create EJ File
*               CliEJClear()        Reset EJ File
*               CliEJWrite()        Write EJ File   Chg R3.1
*               CliEJReverse()      Reverse EJ File
*               CliEJRead()         Read EJ File
*               CliEJGetFlag()      Get EJ Status
*               CliEJGetStatus()    Get Printer Status
*               CliEJRead1Line()    Read EJ 1 Line
*               CliEJReadFile()     Read Data from EJ File,         R3.1
*
*               SerEJReadFile()
*
*               CstInitEJMsgH()     Initialize E/J message area,    R3.1
* --------------------------------------------------------------------------
* Update Histories
*    Date  :Ver.Rev.:   Name    : Description
* Feb-03-93:00.00.01:K.you      : Initial
* Jun-07-93:00.00.01:H.yamaguchi: Adds function(Same as HPINT'L)
* Aug-09-95:03.00.03:M.Suzuki   : Add CliEJRead1Line R3.0
* Mar-19-96:03.01.00:T.Nakahata : R3.1 Initial (E/J Cluster Reset)
* Apr-02-96:03.01.02:T.Nakatata : support R/J sattelite & B-Master
* May-20-96:03.01.07:T.Nakahata : store err code before release semaphore
*===========================================================================
*===========================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===========================================================================
*/
#include	<tchar.h>
#include    <stdio.h>
#include    <memory.h>

#include    <ecr.h>
#include    <pif.h>
#include    <paraequ.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    <ej.h>
#include    <csstbej.h>
#include    "csstubin.h"

extern XGRAM       CliSndBuf;          /* Send Buffer */    
extern XGRAM       CliRcvBuf;          /* Receive Buffer */
extern  CLICOMIF   CliMsg;


/*
*==========================================================================
**    Synopsis:    SHORT CliEJCreat(USHORT usNoOfBlock, UCHAR fchFlag)
*                                 
*       Input:    usNoOfBlock - Number of E.J. Block
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :
*     Normal End:  EJ_PERFORM
*   Abnormal End:  EJ_NO_MAKE_FILE
*
**  Description:
*           Create Electronic Journal File.    If block size is 0, EJ file 
*                never exist.
*==========================================================================
*/
SHORT CliEJCreat(USHORT usNoOfBlock, UCHAR fchFlag)
{
    return(EJCreat(usNoOfBlock, fchFlag));
}

SHORT CliEjConsistencyCheck (VOID)
{
	return (EJConsistencyCheck ());
}
/*
*==========================================================================
**    Synopsis:    SHORT CliEJCheckD(USHORT usNoOfBlock, UCHAR fchFlag)
*
*       Input:    USHORT  usNoOfBlock - Number of E.J. Block
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :
*     Normal End:  EJ_PERFORM
*   Abnormal End:  EJ_PERFORM
*
**  Description:
*               Delete Electronic Journal File. If opened file is error case, or  
*                file size and status are unmatch case.
*==========================================================================
*/              
SHORT CliEJCheckD(USHORT usNoOfBlock, UCHAR fchFlag)
{
    return(EJCheckD(usNoOfBlock, fchFlag));
}

/*
*==========================================================================
**    Synopsis:    SHORT CliEJCheckC(USHORT usNoOfBlock, UCHAR fchFlag)
*
*       Input:    USHORT  usNoOfBlock - Number of E.J. Block
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :
*     Normal End:  EJ_PERFORM
*   Abnormal End:  EJ_NO_MAKE_FILE
*
**  Description:
*               Create Electronic Journal File. If block size is 0, or EJ file 
*                never exist.
*==========================================================================
*/
SHORT CliEJCheckC(USHORT usNoOfBlock, UCHAR fchFlag)
{
    return(EJCheckC(usNoOfBlock, fchFlag));
}

/*
*==========================================================================
**    Synopsis:    SHORT CliEJClear(UCHAR uchUAddr)
*
*       Input:    uchUAddr  -   Terminal Unique Address
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End:  EJ_PERFORM
*   Abnormal End:  EJ_FILE_NOT_EXIST
*               :  EJ_NO_MAKE_FILE
*
**  Description: Reset Electronic Journal File.     
*==========================================================================
*/
SHORT CliEJClear( UCHAR uchUAddr )
{
    /* === Add E/J Cluster Reset (Release 3.1) BEGIN === */
	CLIREQEJ    ReqMsgH = {0};
    CLIRESEJ    ResMsgH = {0};
    SHORT       sRetCode;

    PifRequestSem( husCliExeNet );

    /********** add backup status for B-Master (4/2/96) ****************
    CliMsg.usFunCode    = CLI_FCEJCLEAR;
    *******************************************************************/
    memset( &CliMsg,  0, sizeof( CLICOMIF ));
    CliMsg.usFunCode    = CLI_FCEJCLEAR | CLI_FCWBACKUP | CLI_FCSERVERWB;
    CliMsg.pReqMsgH     = ( CLIREQCOM * )&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof( CLIREQEJ );

    CliMsg.pResMsgH     = ( CLIRESCOM * )&ResMsgH;
    CliMsg.usResMsgHLen = sizeof( CLIRESEJ );

    if ( STUB_SELF == CstSendTarget( uchUAddr, &CliMsg, &CliSndBuf, &CliRcvBuf )) {
        CliMsg.sRetCode = EJClear();
    }
    sRetCode = CliMsg.sRetCode; /**** FIX A GLITCH (05/20/96) *****/

    PifReleaseSem( husCliExeNet);

    return ( sRetCode );

    /* === Add E/J Cluster Reset (Release 3.1) END === */
/*    return(EJClear()); */
}

/*
*==========================================================================
*   Synopsis:  SHORT CliEJWrite(EJT_HEADER *pCHeader, USHORT usWSize, UCHAR *pBuff, UCHAR uchType))
*
*       Input:    *pCHeader - pointer to structure for EJT_HEADER
*                 usWsize   - write data size
*                 *pBuff    - pointer to write data buffer
*                 uchType   - Type of EJ File
*       Output:   Nothing
*       InOut:    Nothing
*
**  Return    :   
*     Normal End: EJ_PERFORM
*   Abnormal End: EJ_FILE_NOT_EXIST       
*               : EJ_DATA_TOO_LARGE
*               : EJ_FILE_FULL
*               : EJ_FILE_NEAR_FULL
*
**  Description: Write data to EJ File.
*==========================================================================
*/
SHORT CliEJWrite(EJT_HEADER FAR *pCHeader, USHORT usWSize, UCHAR FAR *pBuff,
                                            UCHAR uchType, UCHAR uchOption)
{
    return(EJWrite(pCHeader, usWSize, pBuff, uchType, uchOption));
}

/*
*==========================================================================
**    Synopsis:    SHORT CliEJReverse(EJ_READ *pEJRead, UCHAR *pBuff, UCHAR uchType)
*                           
*       Input:    *pEJRead - Pointer of structure for EJ_READ
*                 *pBuff   - pointer to write data buffer
*                 uchType  - Type of EJ File
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :
*     Normal End: EJ_PERFORM
*   Abnormal End: EJ_FILE_NOT_EXIST       
*               : EJ_NOTHING_DATA
*
**  Description: Read E.J. Report Data from the EJ File.
*==========================================================================
*/
SHORT CliEJReverse(EJ_READ *pEJRead, WCHAR *pBuff, UCHAR uchType)
{
    return(EJReverse(pEJRead, pBuff, uchType));
}

/*
*==========================================================================
*   Synopsis:  SHORT CliEJRead(UCHAR uchUAddr,
*                              EJ_READ *pEJRead, UCHAR *pBuff, UCHAR uchType)
*
*       Input:    uchUAddr  - Terminal Unique Address
*                 *pEJRead - Pointer of structure for EJ_READ
*                 *pBuff    - pointer to read data buffer
*                 uchType   - Type of EJ File
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :
*     Normal End: sLeftLen
*   Abnormal End: EJ_FILE_NOT_EXIST       
*               : EJ_NOTHING_DATA
*
**  Description: Read E.J. Report Data from the EJ File.
*==========================================================================
*/
SHORT CliEJRead(UCHAR uchUAddr, EJ_READ *pEJRead, TCHAR *pBuff, UCHAR uchType)
{
    /* === Add E/J Cluster Reset (Release 3.1) BEGIN === */
    CLIRESEJREPT *pResReptMsgH;
	CLIREQEJ    ReqMsgH = {0};
    CLIRESEJ    ResMsgH = {0};
    SHORT       sRetCode;
	SHORT       sErrorM = STUB_SUCCESS;

    PifRequestSem( husCliExeNet );

    /* --- make request message structure --- */
    memcpy( ReqMsgH.auchEJRead, pEJRead, sizeof( EJ_READ ));
    ReqMsgH.uchType = uchType;

    /* --- link request message to client/server message area --- */    
    /********** add backup status for B-Master (4/2/96) ****************
    CliMsg.usFunCode    = CLI_FCEJREAD;
    *******************************************************************/
    memset( &CliMsg,  0, sizeof( CLICOMIF ));
    CliMsg.usFunCode    = CLI_FCEJREAD | CLI_FCWBACKUP | CLI_FCSERVERWB;
    CliMsg.pReqMsgH     = ( CLIREQCOM * )&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof( CLIREQEJ );

    /* --- link response message to client/server message area --- */    
    CliMsg.pResMsgH     = ( CLIRESCOM * )&ResMsgH;
    CliMsg.usResMsgHLen = sizeof( CLIRESEJ );

    /* --- send message to specified terminal --- */
    if ( STUB_SELF == (sErrorM = CstSendTarget( uchUAddr, &CliMsg, &CliSndBuf, &CliRcvBuf ))) {
        /* --- call own E/J module, if specified terminal is myself --- */
        CliMsg.sRetCode = EJRead( pEJRead, pBuff, uchType );
		sErrorM = STUB_SUCCESS;
    } else {
        /* --- copy retrieved data to user's area, if function is successful --- */
        if ( 0 <= CliMsg.sRetCode ) {
            pResReptMsgH = ( CLIRESEJREPT * )&ResMsgH;
            memcpy( pBuff,   pResReptMsgH->auchEJData, CLI_MAX_EJREPTDATA );
            memcpy( pEJRead, pResReptMsgH->auchEJRead, sizeof( EJ_READ ));
        }
    }

    sRetCode = CliMsg.sRetCode; /**** FIX A GLITCH (05/20/96) *****/
    PifReleaseSem( husCliExeNet);

	if (sErrorM) {
		char xBuff[128];
		sprintf (xBuff, "==NOTE: CliEJRead() sRetCode %d, sErrorM %d", sRetCode, sErrorM);
		NHPOS_NONASSERT_TEXT(xBuff);
	}
    return ( sRetCode );

    /* === Add E/J Cluster Reset (Release 3.1) END === */
/*     return(EJRead(pEJRead, pBuff, uchType)); */
}

/*
*==========================================================================
*   Synopsis:  UCHAR CliEJGetFlag(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :  fchEJFlag Data
*
**  Description: Return EJmgr satus flags
*==========================================================================
*/
UCHAR CliEJGetFlag(VOID)
{
    return(EJGetFlag());            
}

/*
*==========================================================================
*   Synopsis:  USHORT CliEJGetStatus(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :  EJ_THERMAL_NOT_EXIST
*             :  EJ_THERMAL_EXIST
*
**  Description: Return EJmgr satus flags
*==========================================================================
*/
SHORT CliEJGetStatus(VOID)
{
    return(EJGetStatus());            
}

/*
*==========================================================================
*   Synopsis:  SHORT    SerEJReadFile(ULONG ulOffset, VOID *pData, USHORT usDataLen,
*                                     USHORT *pusRetLen)
*
*       Input:    ulOffset - Read Pointer of EJ File
*                 usDataLen - Read Buffer Length
*       InOut:    *usRetLen - Pointer of Readed Data Length 
*                 *pData - Pointer of Buffer 
*
**  Return    :
*     Normal End: EJ_PERFORM
*               : EJ_EOF
*   Abnormal End: EJ_FILE_NOT_EXIST       
*               : EJ_DATA_TOO_LARGE
*
**  Description: Read E.J. File Data from the EJ File.
*==========================================================================
*/
SHORT   SerEJReadFile(ULONG ulOffset,  VOID  *pData,
                      ULONG ulDataLen, ULONG *pulRetLen)
{
    return(EJReadFile(ulOffset, pData, ulDataLen, pulRetLen));
}

/*
*==========================================================================
*   Synopsis:  SHORT    SerEJClear(VOID)
*
*       Input:    
*
**  Return    :
*     Normal End: EJ_PERFORM
*   Abnormal End: EJ_FILE_NOT_EXIST       
*
**  Description: Clear the EJ file..
*==========================================================================
*/
SHORT   SerEJClear(VOID)
{
    return(EJClear());
}


/***************************** Add R3.0 ***********************************/
/*
*==========================================================================
*   Synopsis:  SHORT CliEJRead1Line(EJ_READ *pEJRead, UCHAR *pBuff, UCHAR uchType)
*
*       Input:    *pEJRead - Pointer of structure for EJ_READ
*                 *pBuff   - pointer to read data buffer
*                 uchType  - Type of EJ File
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :
*     Normal End: EJ_PERFORM
*   Abnormal End: EJ_FILE_NOT_EXIST       
*               : EJ_NOTHING_DATA
*
**  Description: Read E.J. 1 Line Report Data from the EJ File.
*==========================================================================
*/
SHORT CliEJRead1Line(EJ_READ *pEJRead, TCHAR *pBuff, UCHAR uchType)
{
    return(EJRead1Line(pEJRead, pBuff, uchType));
}
/***************************** End Add R3.0 *******************************/

/*************** Add New Functions (Release 3.1) BEGIN ********************/
/*
*==========================================================================
*   Synopsis:  SHORT    CliEJReadFile(UCHAR uchUAddr, ULONG ulOffset,
*                                     VOID *pData, USHORT usDataLen,
*                                     USHORT *pusRetLen)
*
*       Input:    uchUAddr  - Terminal Unique Address
*                 ulOffset  - Read Pointer of EJ File
*                 usDataLen - Read Buffer Length
*       InOut:    *usRetLen - Pointer of Readed Data Length 
*                 *pData    - Pointer of Buffer 
*
**  Return    :
*     Normal End: EJ_PERFORM
*               : EJ_EOF
*   Abnormal End: EJ_FILE_NOT_EXIST       
*               : EJ_DATA_TOO_LARGE
*
**  Description: Read E.J. File Data from the EJ File.
*==========================================================================
*/
#ifdef POSSIBLE_DEAD_CODE
SHORT   CliEJReadFile(UCHAR uchUAddr, ULONG ulOffset, VOID *pData,
                      ULONG ulDataLen, ULONG *pulRetLen)
{
    CLIREQEJ    ReqMsgH;
    CLIRESEJ    ResMsgH;
    SHORT       sRetCode;

    if ( ulDataLen < CLI_MAX_EJDATA ) {

        return ( EJ_TOO_SMALL_BUFF );
    }

    PifRequestSem( husCliExeNet );

    CstInitEJMsgH( &ReqMsgH, &ResMsgH );

    /* --- make request message structure --- */

    ReqMsgH.ulOffset    = ulOffset;

    /* --- link request message to client/server message area --- */    

    CliMsg.usFunCode    = CLI_FCEJREADFILE;
    CliMsg.pReqMsgH     = ( CLIREQCOM * )&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof( CLIREQEJ );

    /* --- link response message to client/server message area --- */    

    CliMsg.pResMsgH     = ( CLIRESCOM * )&ResMsgH;
    CliMsg.usResMsgHLen = sizeof( CLIRESEJ );

    /* --- send message to specified terminal --- */

    if ( STUB_SELF == CstSendTarget( uchUAddr, &CliMsg, &CliSndBuf, &CliRcvBuf )) {

        /* --- call own E/J module, if specified terminal is myself --- */

        CliMsg.sRetCode = EJReadFile( ulOffset, pData, ulDataLen, pulRetLen );

    } else {

        /* --- copy retrieved data to user's area, if function is successful --- */

        if (( CliMsg.sRetCode == EJ_PERFORM ) || ( CliMsg.sRetCode == EJ_EOF )) {

            memcpy( pData, ResMsgH.auchEJData, ResMsgH.ulDataLen );
            *pulRetLen = ResMsgH.ulDataLen;
        }
    }    
    sRetCode = CliMsg.sRetCode; /**** FIX A GLITCH (05/20/96) *****/

    PifReleaseSem( husCliExeNet);

    return ( sRetCode );
}
#endif

/*
*==========================================================================
*   Synopsis:   SHORT   SerEJNoOfTrans(VOID)
*
*       Input:    none
*       InOut:    none
**  Return      : status
*
**  Description: Get # of Transaction in E/J File.          V3.3
*==========================================================================
*/
SHORT   SerEJNoOfTrans(VOID)
{
    return(EJNoOfTrans());
}

/*
*==========================================================================
*   Synopsis:   SHORT   SerEJReadResetTransOver(ULONG *pulOffset,
*                           VOID *pData, USHORT usDataLen, USHORT *pusRetLen)
*
*       Input:    ulOffset - Read Pointer of EJ File
*                 usDataLen - Read Buffer Length
*       InOut:    *usRetLen - Pointer of Readed Data Length
*                 *pData - Pointer of Buffer
**  Return      : # of Transaction
*   Abnormal End: EJ_FILE_NOT_EXIST
*               : EJ_DATA_TOO_LARGE
*
**  Description: Read/Reset Trans. Data in E/J File(Override Type). V3.3
*==========================================================================
*/
SHORT   SerEJReadResetTransOver(ULONG *pulOffset,
            VOID *pData, ULONG ulDataLen, ULONG *pulRetLen)
{
    return(EJReadResetTransOver(pulOffset, pData, ulDataLen, pulRetLen));
}

/*
*==========================================================================
*   Synopsis:   SHORT   SerEJReadResetTransStart(ULONG *pulOffset,
*                           VOID *pData, USHORT usDataLen, USHORT *pusRetLen)
*       Input:    ulOffset - Read Pointer of EJ File
*                 usDataLen - Read Buffer Length
*                 uchType   - No of Delete Transaction
*       InOut:    *usRetLen - Pointer of Readed Data Length
*                 *pData - Pointer of Buffer
*       Out:      *pusNumTrans - Number of Transaction
**  Return      : EJ_CONTINUE
*               : EJ_END
*   Abnormal End: EJ_FILE_NOT_EXIST
*               : EJ_DATA_TOO_LARGE
*
**  Description: Read/Reset Trans. Data in E/J File(Not Override).  V3.3
*==========================================================================
*/
SHORT   SerEJReadResetTransStart(ULONG *pulOffset,
                    VOID *pData, ULONG ulDataLen, ULONG *pulRetLen, USHORT *pusNumTrans)
{
    return(EJReadResetTransStart(pulOffset, pData, ulDataLen, pulRetLen, pusNumTrans));
}

/*
*==========================================================================
*   Synopsis:   SHORT   SerEJReadResetTrans(ULONG *pulOffset, VOID *pData,
*                           USHORT usDataLen, USHORT *pusRetLen, USHORT usNumTrans)
*
*       Input:    ulOffset - Read Pointer of EJ File
*                 usDataLen - Read Buffer Length
*                 uchType   - No of Delete Transaction
*       InOut:    *usRetLen - Pointer of Readed Data Length
*                 *pData - Pointer of Buffer
*                 *pusNumTrans - Number of Transaction
**  Return      : EJ_CONTINUE
*               : EJ_END
*   Abnormal End: EJ_FILE_NOT_EXIST
*               : EJ_DATA_TOO_LARGE
*
**  Description: Read/Reset Trans. Data in E/J File(Not Override).  V3.3
*==========================================================================
*/
SHORT   SerEJReadResetTrans(ULONG *pulOffset, VOID *pData,
            ULONG ulDataLen, ULONG *pulRetLen, USHORT *pusNumTrans)
{
    return(EJReadResetTrans(pulOffset, pData, ulDataLen, pulRetLen, pusNumTrans));
}

/*
*==========================================================================
*   Synopsis:   SHORT   SerEJReadResetTransEnd(USHORT usNumTrans)
*
*       Input:    uchType   - No of Delete Transaction
*       InOut:    none
**  Return      : status
*
**  Description: Delete Previous Trans. Data in E/J File(Not Override). V3.3
*==========================================================================
*/
SHORT   SerEJReadResetTransEnd(USHORT usNumTrans)
{
    return(EJReadResetTransEnd(usNumTrans));
}

/*HRESULT
*==========================================================================
*   Synopsis:  SHORT CliEJLock(UCHAR uchUAddr,
*                              UCHAR uchLock)
*
*       Input:    uchUAddr  - Terminal Unique Address
*                 uchType   - Type of EJ Lock
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :
*     Normal End: EJ_PERFORM
*   Abnormal End: EJ_PROHIBT
*
**  Description: Lock EJ File for reset report, V3.3
*==========================================================================
*/
SHORT CliEJLock(UCHAR uchUAddr, UCHAR uchLock)
{
	CLIREQEJ    ReqMsgH = {0};
    CLIRESEJ    ResMsgH = {0};
    SHORT       sRetCode;
	SHORT       sErrorM = STUB_SUCCESS;

    PifRequestSem( husCliExeNet );

    ReqMsgH.uchType = uchLock;

    memset( &CliMsg,  0, sizeof( CLICOMIF ));
    CliMsg.usFunCode    = CLI_FCEJLOCK | CLI_FCWBACKUP | CLI_FCSERVERWB;
    CliMsg.pReqMsgH     = ( CLIREQCOM * )&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof( CLIREQEJ );

    CliMsg.pResMsgH     = ( CLIRESCOM * )&ResMsgH;
    CliMsg.usResMsgHLen = sizeof( CLIRESEJ );

    if ( STUB_SELF == (sErrorM = CstSendTarget( uchUAddr, &CliMsg, &CliSndBuf, &CliRcvBuf )) ) {
        CliMsg.sRetCode = EJLock(uchLock);
		sErrorM = STUB_SUCCESS;
    }

    sRetCode = CliMsg.sRetCode;
    PifReleaseSem( husCliExeNet);

	if (sErrorM) {
		char xBuff[128];
		sprintf (xBuff, "==NOTE: CliEJLock() sRetCode %d, sErrorM %d", sRetCode, sErrorM);
		NHPOS_NONASSERT_TEXT(xBuff);
	}
    return ( sRetCode );

}


/*
*==========================================================================
*   Synopsis:  SHORT CliEJUnLock(UCHAR uchUAddr,
*                              UCHAR uchLock)
*
*       Input:    uchUAddr  - Terminal Unique Address
*                 uchType   - Type of EJ Lock
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :
*     Normal End: EJ_PERFORM
*   Abnormal End: EJ_PROHIBT
*
**  Description: Unlock EJ File for reset report, V3.3.
*==========================================================================
*/
SHORT CliEJUnLock(UCHAR uchUAddr, UCHAR uchLock)
{
	CLIREQEJ    ReqMsgH = {0};
	CLIRESEJ    ResMsgH = {0};
    SHORT       sRetCode;
	SHORT       sErrorM = STUB_SUCCESS;

    PifRequestSem( husCliExeNet );

    ReqMsgH.uchType = uchLock;

    memset( &CliMsg,  0, sizeof( CLICOMIF ));
    CliMsg.usFunCode    = CLI_FCEJUNLOCK | CLI_FCWBACKUP | CLI_FCSERVERWB;
    CliMsg.pReqMsgH     = ( CLIREQCOM * )&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof( CLIREQEJ );

    CliMsg.pResMsgH     = ( CLIRESCOM * )&ResMsgH;
    CliMsg.usResMsgHLen = sizeof( CLIRESEJ );

    if ( STUB_SELF == (sErrorM = CstSendTarget( uchUAddr, &CliMsg, &CliSndBuf, &CliRcvBuf )) ) {
        CliMsg.sRetCode = EJUnLock(uchLock);
		sErrorM = STUB_SUCCESS;
    }

    sRetCode = CliMsg.sRetCode;
    PifReleaseSem( husCliExeNet);

	if (sErrorM) {
		char xBuff[128];
		sprintf (xBuff, "==NOTE: CliEJUnLock() sRetCode %d, sErrorM %d", sRetCode, sErrorM);
		NHPOS_NONASSERT_TEXT(xBuff);
	}
    return ( sRetCode );

}


/******** End of Source *************************/
