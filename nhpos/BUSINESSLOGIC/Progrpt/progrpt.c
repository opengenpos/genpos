/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1996-9         ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*=======================================================================*\
*   Title              : Programmable Report module, Program List
*   Category           : Prog. Report, NCR2170 US HOSPITALITY APPLICATION
*   Program Name       : PROGRPT.C
*  ------------------------------------------------------------------------
*   Abstract           : The provided function names are as follows.
*
*       --- for file creation/deletion
*   ProgRptInitialize()     - Initialize Programmable Report Module.
*   ProgRptCreate()         - Create Programmable Report File.
*   ProgRptCheckAndCreate() - Create File, if file size is changed.
*   ProgRptCheckAndDelete() - Delete File, if file size is changed.
*       --- for file back up
*   ProgRptSendFile()       - Request PROGRPT file to Master/B-Master.
*   ProgRptRcvFile()        - Response PROGRPT file for Master/B-Master.
*       --- for retrieve information
*   ProgRptGetReportInfo()  - Retrieve information about prog. report.
*   ProgRptGetMLDReportNo() - Retrieve report no. with MLD format.
*   ProgRptGetEODPTDInfo()  - Retrieve information about EOD/PTD report.
*       --- for issue programmable report
*   ProgRptDeptRead()       - Issue department read report.
*   ProgRptPluRead()        - Issue plu read report.
*   ProgRptPluReset()       - Issue plu reset report.
*   ProgRptCpnRead()        - Issue coupon read report.
*   ProgRptCpnReset()       - Issue coupon reset report.    
*   ProgRptCasRead()        - Issue cashier read report.
*   ProgRptCasReset()       - Issue cashier reset report.
*   ProgRptWaiRead()        - Issue server read report.
*   ProgRptWaiReset()       - Issue server reset report.
*   ProgRptFinRead()        - Issue register financial read report.
*   ProgRptIndFinRead()     - Issue individual financial read report.
*   ProgRptIndFinReset()    - Issue individual financial reset report.
*   ProgRptEODReset()       - Issue EOD reset report.
*   ProgRptPTDReset()       - Issue PTD reset report.
*
*  ------------------------------------------------------------------------
*    Update Histories  :
*                                 
*    Date      Ver.Rev :NAME       :Description
*    Mar-13-96:03.01.00:T.Nakahata :Initial
*    Apr-12-96:03.01.03:T.Nakahata :Set backup work buffer size
*    May-24-96:03.01.07:T.Nakahata :Add resetted terminal# print routine
*    Aug-08-99:03.05.00:M.Teraki   :Remove WAITER_MODEL
*    Dec-13-99:01.00.00:hrkato     :Saratoga
\*=======================================================================*/
/*=======================================================================*\
:   PVCS ENTRY
:-------------------------------------------------------------------------
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

#include	<tchar.h>
#include    <string.h>

#include    <ecr.h>
#include    <regstrct.h>
#include    <pif.h>
#include    <log.h>
#include    <appllog.h>
#include    <paraequ.h>
#include    <para.h>
#include    <maint.h>
#include    <ej.h>
#include    <csttl.h>
#include    <csop.h>
#include    <cscas.h>
#include    <cswai.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    <csstbstb.h>
#include    <csstbpar.h>
#include    <report.h>
#include    <progrept.h>
#include    <csprgrpt.h>
#include    "_progrpt.h"

/*
===========================================================================
    GLOBAL VARIABLES DECLARATIONs
===========================================================================
*/
    /* --- handle of semaphore --- */
PifSemHandle  husProgRptSem = PIF_SEM_INVALID_HANDLE;

    /* --- buffer for print data --- */
TCHAR   auchProgRptBuff[ PROGRPT_MAX_COLUMN + 1 ];

    /* --- row & column for formatted string in "auchProgRptBuff" --- */
UCHAR   uchProgRptColumn;
UCHAR   uchProgRptRow;

    /* --- determine current system has an Electoric Journal  --- */
BOOL    fProgRptWithEJ;
BOOL    fProgRptEmptyLoop;

/*
===========================================================================
    USER I/F FUNCTIONs
===========================================================================
*/
/*-------------------------------------------------------------------------
            for file creation and deletion
-------------------------------------------------------------------------*/
/*
*==========================================================================
**  Synopsis:   VOID ProgRptInitialize( VOID )
*
*   Input   :   Nothing
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   Nothing
*               
**  Description:
*    This function initializes PROGRAMMABLE REPORT module. An application
*   must call this function before calling any other PROGRPT functions.
*==========================================================================
*/
VOID ProgRptInitialize( VOID )
{
    ASSERT( husProgRptSem == (USHORT)PROGRPT_INVALID_HANDLE );

    husProgRptSem = PifCreateSem( PROGRPT_SEM_INIT_CO );

    ProgRpt_InitFile();

    fProgRptWithEJ = ( EJGetStatus() == EJ_THERMAL_EXIST ) ? TRUE : FALSE;
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRptCreate( UCHAR uchNoOfBlock )
*
*   Input   :   UCHAR uchNoOfBlock  - no. of blocks for file creation
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode      - PROGRPT_SUCCESS
*                                   - PROGRPT_NO_MAKE_FILE
*                                   
**  Description:
*    This function creates PROGRPT file.
*==========================================================================
*/
SHORT ProgRptCreate( UCHAR uchNoOfBlock )
{
    SHORT   sRetCode;       /* return code for user application */
    SHORT   sExpandState;   /* return status for file expand */

    ASSERT( uchNoOfBlock <= FLEX_PROGRPT_MAX );

    PifRequestSem( husProgRptSem );

    /* --- if PROGRPT file already exists, delete it before new creation --- */

    ProgRpt_Delete();

    sRetCode = PROGRPT_SUCCESS;

    if ( 0 < uchNoOfBlock ) {

        if ( ProgRpt_NewCreate() != PROGRPT_SUCCESS ) {
            return ( PROGRPT_NO_MAKE_FILE );
        }

        /* --- expand PROGRPT file to specified size --- */

        sExpandState = ProgRpt_ExpandFile( uchNoOfBlock );
        ASSERT(( sExpandState == PROGRPT_SUCCESS ) ||\
               ( sExpandState == PROGRPT_DEVICE_FULL ));

        switch ( sExpandState ) {
        case PROGRPT_SUCCESS:

            /* --- initialize file header of PROGRPT file --- */

            ProgRpt_InitFileHeader( uchNoOfBlock );
            ProgRpt_Close();
            break;

        case PROGRPT_DEVICE_FULL:
        default:

            /* --- if file expansion is failed, delete garbage --- */

            ProgRpt_Close();
            ProgRpt_Delete();
            sRetCode = PROGRPT_NO_MAKE_FILE;
            break;
        }
    }
    PifReleaseSem( husProgRptSem );
    return ( sRetCode );
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRptCheckAndCreate( UCHAR uchNoOfBlock )
*
*   Input   :   UCHAR uchNoOfBlock  - no. of blocks for file creation
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode      - PROGRPT_SUCCESS
*                                   - PROGRPT_NO_MAKE_FILE
*                                   
**  Description:
*    This function creates PROGRPT file, if PROGRPT file does not exist.
*==========================================================================
*/
SHORT ProgRptCheckAndCreate( UCHAR uchNoOfBlock )
{
    SHORT   sRetCode;       /* return code for user application */
    SHORT   sOpenState;     /* return status for old file open */

    ASSERT( uchNoOfBlock <= FLEX_PROGRPT_MAX );

    sRetCode = PROGRPT_SUCCESS;

    /* --- open PROGRPT file, if it already exists --- */

    sOpenState = ProgRpt_OldOpen();

    switch ( sOpenState ) {

    case PROGRPT_SUCCESS:

        ProgRpt_Close();
        break;

    case PROGRPT_NO_READ_SIZE:

        sRetCode = PROGRPT_NO_MAKE_FILE;
        break;

    case PROGRPT_FILE_NOT_FOUND:
    case PROGRPT_INVALID_FILE:
    case PROGRPT_INVALID_VERSION:
    default:

        /* --- if old file not exists, create new PROGRPT file --- */

        sRetCode = ProgRptCreate( uchNoOfBlock );
        break;
    }
    return ( sRetCode );
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRptCheckAndDelete( UCHAR uchNoOfBlockNew )
*
*   Input   :   UCHAR uchNoOfBlockNew - no. of blocks for file deletion
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode      - PROGRPT_SUCCESS
*                                   - PROGRPT_DELETE_FILE
*                                   
**  Description:
*    This function deletes PROGRPT file, if specified no. of blocks is
*   different from actual no. of blocks.
*==========================================================================
*/
SHORT ProgRptCheckAndDelete( UCHAR uchNoOfBlockNew )
{
    SHORT   sRetCode;               /* return code for user application */
    UCHAR   uchNoOfBlockCurrent;    /* current no. of block size */

    ASSERT( uchNoOfBlockNew <= FLEX_PROGRPT_MAX );

    sRetCode = PROGRPT_SUCCESS;

    /* --- open PROGRPT file, if it already exists --- */

    if ( ProgRpt_OldOpen() == PROGRPT_SUCCESS ) {

        /* --- retrieve current existance PROGRPT file size --- */

        uchNoOfBlockCurrent = ProgRpt_LoadNoOfBlock();
        ASSERT( uchNoOfBlockCurrent <= FLEX_PROGRPT_MAX );
        ProgRpt_Close();

        /* --- delete old file, if inquiry size is different from actual size --- */

        if ( uchNoOfBlockCurrent != uchNoOfBlockNew ) {

            ProgRpt_Delete();
            PifLog( MODULE_PROGRPT, LOG_EVENT_PROGRPT_DELETE_FILE );

            sRetCode = PROGRPT_DELETE_FILE;
        }
    }
    return ( sRetCode );
}


/*-------------------------------------------------------------------------
            for file backup
-------------------------------------------------------------------------*/
/*
*==========================================================================
**  Synopsis:   SHORT ProgRptSendFile( VOID )
*
*   Input   :   Nothing
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode      - PROGRPT_SUCCESS
*                                   - PROGRPT_COMERROR
*                                   
**  Description:
*    This function requests to backup PROGRPT file.
*==========================================================================
*/
SHORT ProgRptSendFile( VOID )
{
    SHORT           sRetCode;       /* return code for user application */
    PROGRPT_BACKUP  BackupReq;      /* information about request backup */
    UCHAR           auchRcvBuf[ PROGRPT_BACKUP_WORK_SIZE ];
                                    /* buffer for recieve PROGRPT data */
    USHORT          usRcvLen;       /* recieve buffer length in bytes */
    USHORT          usActDataLen;   /* actual PROGRPT data length in bytes */
    PROGRPT_BACKUP  *pBackupRcv;    /* information about response backup */
    PROGRPT_BAKDATA *pBackupData;   /* actual retrieved PROGRPT data */

    PifRequestSem( husProgRptSem );

    if ( ProgRpt_OldOpen() != PROGRPT_SUCCESS ) {
        PifReleaseSem( husProgRptSem );
        return ( PROGRPT_SUCCESS );
    }

    memset( &BackupReq, 0x00, sizeof( PROGRPT_BACKUP ));
    pBackupRcv  = ( PROGRPT_BACKUP  * )auchRcvBuf;
    pBackupData = ( PROGRPT_BAKDATA * )( auchRcvBuf + sizeof( PROGRPT_BACKUP ));

    do {

        /* --- request PROGRPT file from MASTER/B-MASTER --- */

        usRcvLen = PROGRPT_BACKUP_WORK_SIZE;    /* backup buffer size (4/12/96) */
        sRetCode = SstReqBackUp( CLI_FCBAKPROGRPT, 
                                 ( UCHAR * )&BackupReq,
                                 sizeof( PROGRPT_BACKUP ),
                                 auchRcvBuf,
                                 &usRcvLen );
        if ( sRetCode < 0 ) {
            break;
        }

        /* --- determine retrieved data is correct or incorrect --- */

        usActDataLen = usRcvLen - sizeof( PROGRPT_BACKUP ) - sizeof( USHORT );

        if (( pBackupRcv->usSeqNO    == BackupReq.usSeqNO ) &&
            ( pBackupData->ulDataLen == usActDataLen )) {

            /* --- save retrieved data to own PROGRPT file --- */

            if ( ! ProgRpt_WriteFile( BackupReq.offulFilePosition,
                                      pBackupData->auchData,
                                      pBackupData->ulDataLen )) {
                sRetCode = PROGRPT_COMERROR;
            }
            BackupReq.offulFilePosition += pBackupData->ulDataLen;
            BackupReq.usSeqNO++;

        } else {    /* retrieved data is incorrect */

            sRetCode = PROGRPT_COMERROR;
        }

    } while (( pBackupRcv->uchStatus == PROGRPT_CONTINUE ) &&
             ( sRetCode == PROGRPT_SUCCESS ));

    ProgRpt_Close();
    PifReleaseSem( husProgRptSem );

    return ( sRetCode );
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRptRcvFile( UCHAR  *puchRcvData,
*                                     USHORT usRcvLen,
*                                     UCHAR  *puchSndData,
*                                     USHORT *pusSndLen )
*
*   Input   :   UCHAR  *puchRcvData - address of information about request
*               USHORT usRcvLen     - size of request data
*
*   Output  :   UCHAR  *puchSndData - address of buffer to response data
*               USHORT *pusSndLen   - address of size of response data
*
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode      - PROGRPT_SUCCESS
*                                   - PROGRPT_COMERROR
*                                   - PROGRPT_FILE_NOT_FOUND
*                                   
**  Description:
*    This function resposes to backup PROGRPT file.
*==========================================================================
*/
SHORT ProgRptRcvFile( UCHAR  *puchRcvData,
                      USHORT usRcvLen,
                      UCHAR  *puchSndData,
                      USHORT *pusSndLen)
{
    PROGRPT_BACKUP  *pBackupReq;    /* information about request backup */
    PROGRPT_BACKUP  *pBackupSnd;    /* information about response backup */
    PROGRPT_BAKDATA *pBackupData;   /* buffer pointer to store PROGRPT data */
    UCHAR           uchNoOfBlock;   /* allocated block size of PROGRPT file */
    ULONG           ulAllFileSize;  /* actual PROGRPT file size in bytes */
    USHORT          usSendDataSize; /* size of response data in bytes */
    ULONG           ulActualRead;   /* actual size of read PROGRPT data */

    ASSERT( puchRcvData != ( UCHAR * )NULL );
    ASSERT( puchSndData != ( UCHAR * )NULL );
    ASSERT( pusSndLen   != ( USHORT * )NULL ); /* ### 990825 Mod by Iwata*/

    PifRequestSem( husProgRptSem );

    /* --- determine request information is correct or incorrect --- */

    if ( usRcvLen != sizeof( PROGRPT_BACKUP )) {
        PifReleaseSem( husProgRptSem );
        return ( PROGRPT_COMERROR );
    }

    if ( ProgRpt_OldOpen() != PROGRPT_SUCCESS ) {
        PifReleaseSem( husProgRptSem );
        return ( PROGRPT_FILE_NOT_FOUND );
    }
    
    pBackupReq  = ( PROGRPT_BACKUP  * )puchRcvData;
    pBackupSnd  = ( PROGRPT_BACKUP  * )puchSndData;
    pBackupData = ( PROGRPT_BAKDATA * )( puchSndData + sizeof( PROGRPT_BACKUP ));

    /* --- check requested read position is over than actual file size --- */

    uchNoOfBlock = ProgRpt_LoadNoOfBlock();
    ASSERT( uchNoOfBlock <= FLEX_PROGRPT_MAX );

    ulAllFileSize = ( ULONG )( PROGRPT_BYTE_PER_BLOCK * uchNoOfBlock );
    if ( ulAllFileSize <= pBackupReq->offulFilePosition ) {
        ProgRpt_Close();
        PifReleaseSem( husProgRptSem );
        return ( PROGRPT_COMERROR );
    }

    /* --- determine this flame is the last flame --- */

    usSendDataSize = PROGRPT_BACKUP_WORK_SIZE -
                        sizeof( PROGRPT_BACKUP ) - sizeof( USHORT );

    if ( pBackupReq->offulFilePosition + usSendDataSize < ulAllFileSize ) {
        pBackupSnd->uchStatus = PROGRPT_CONTINUE;
    } else {
        usSendDataSize = ( USHORT )( ulAllFileSize - pBackupReq->offulFilePosition );
        pBackupSnd->uchStatus = PROGRPT_EOF;
    }

    /* --- retrieve own PROGRPT data to specified buffer --- */

    if ( ! ProgRpt_ReadFile( pBackupReq->offulFilePosition,
                             pBackupData->auchData,
                             usSendDataSize,
                             &ulActualRead )) {
        ProgRpt_Close();
        PifReleaseSem( husProgRptSem );
        return ( PROGRPT_COMERROR );
    }

    /* --- make information about response backup --- */

    pBackupSnd->usSeqNO           = pBackupReq->usSeqNO;
    pBackupSnd->offulFilePosition = pBackupReq->offulFilePosition;

    pBackupData->ulDataLen = ulActualRead;

    *pusSndLen = sizeof( PROGRPT_BACKUP) + sizeof( USHORT ) +
                 pBackupData->ulDataLen;

    ProgRpt_Close();
    PifReleaseSem( husProgRptSem );

    return ( PROGRPT_SUCCESS );
}


/*-------------------------------------------------------------------------
            for retrieve file information
-------------------------------------------------------------------------*/
/*
*==========================================================================
**  Synopsis:   SHORT ProgRptGetReportInfo( USHORT usInfoType,
*                                           UCHAR  uchProgRptNo,
*                                           USHORT *pusFileType )
*
*   Input   :   USHORT usInfoType   - type of information to retrive
*               UCHAR  uchProgRptNo - programmable report no.
*   Output  :   USHORT *pusInfo     - points to report information
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode  - SUCCESS
*                                 LDT_KEYOVER_ADR
*                                 LDT_PROHBT_ADR
*                                 LDT_ERR_ADR
*                                   
**  Description:
*    This function retrieves information about specified total file object
*   of programmable report.
*==========================================================================
*/
SHORT ProgRptGetReportInfo( USHORT usInfoType,
                            UCHAR  uchProgRptNo,
                            USHORT *pusInfo )
{
    SHORT           sRetCode;   /* return code for user application */
    PROGRPT_RPTHEAD ReportInfo; /* information about prog report */

    ASSERT(( usInfoType == PROGRPT_FILETYPE ) ||\
           ( usInfoType == PROGRPT_DEVICE ) ||\
           ( usInfoType == PROGRPT_LOOPKEY ));
    ASSERT( pusInfo != ( USHORT * )NULL );

    if (( uchProgRptNo < 1 ) || ( MAX_PROGRPT_FORMAT < uchProgRptNo )) {
        return ( LDT_KEYOVER_ADR );
    }

    PifRequestSem( husProgRptSem );

    sRetCode = ProgRpt_OldOpen();

    if ( sRetCode == PROGRPT_SUCCESS ) {

        sRetCode = ProgRpt_GetProgRptInfo( uchProgRptNo, &ReportInfo );
        ProgRpt_Close();

        switch ( usInfoType ) {
        case PROGRPT_FILETYPE:
            *pusInfo = ReportInfo.usFileType;
            break;
        case PROGRPT_DEVICE:
            *pusInfo = ReportInfo.usDevice;
            break;
        case PROGRPT_LOOPKEY:
        default:
            *pusInfo = ReportInfo.usKeyFlag;
            break;
        }
    }
    PifReleaseSem( husProgRptSem );

    if ( ReportInfo.usDevice == PROGRPT_DEVICE_MLD ) {
        sRetCode = PROGRPT_FILE_NOT_FOUND;
    }
    return ( ProgRpt_ConvertError( sRetCode ));
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRptGetMLDReportNo( UCHAR uchProgRptNo,
*                                            UCHAR *puchMLDRptNo )
*
*   Input   :   UCHAR uchProgRptNo  - programmable report no.
*   Output  :   UCHAR *puchMLDRptNo - MLD formatted report no.
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode  - SUCCESS
*                                 LDT_KEYOVER_ADR
*                                 LDT_PROHBT_ADR
*                                 LDT_NTINFL_ADR  
*
**  Description:
*    This function retrieves report number with MLD format that is the same
*   total file type with specified report number.
*    If function is successful, it returns SUCCESS and copies report number
*   with MLD format to buffer that is pointed by puchMLDRptNo parameter.
*    If MLD format is not found, it returns SUCCESS and copies 0 to buffer
*   that is pointed by puchMLDRptNo parameter.
*    If specified report no. is not found, it returns LDT_NTINFL_ADR;
*==========================================================================
*/
SHORT ProgRptGetMLDReportNo( UCHAR uchProgRptNo,
                             UCHAR *puchMLDRptNo )
{
    SHORT   sRetCode;           /* return code for user application */
    USHORT  usSrcFileType;      /* total file type of specified rpt no. */
    USHORT  usLoopRptNo;        /* prog report no. for loop */
    BOOL    fFound;             /* boolean value for record is found */
    PROGRPT_FHEAD   FileInfo;   /* information about prog report file */
    PROGRPT_RPTHEAD ReportInfo; /* information about prog report */

    ASSERT( puchMLDRptNo != ( UCHAR * )NULL );

    if (( uchProgRptNo < 1 ) || ( MAX_PROGRPT_FORMAT < uchProgRptNo )) {
        return ( LDT_KEYOVER_ADR );
    }

    PifRequestSem( husProgRptSem );

    if (( sRetCode = ProgRpt_OldOpen()) != PROGRPT_SUCCESS ) {
        PifReleaseSem( husProgRptSem );
        return ( ProgRpt_ConvertError( sRetCode ));
    }

    /* --- retrieve available no. of programmed report --- */

    ProgRpt_GetFileHeader( &FileInfo );

    if ( FileInfo.usNoOfReport == 0 ) {
        ProgRpt_Close();
        PifReleaseSem( husProgRptSem );
        return ( LDT_PROHBT_ADR );
    }

    /* --- retrieve total file type of source report no. --- */

    sRetCode = ProgRpt_GetProgRptInfo( uchProgRptNo, &ReportInfo );
    if ( sRetCode != PROGRPT_SUCCESS ) {
        ProgRpt_Close();
        PifReleaseSem( husProgRptSem );
        return ( ProgRpt_ConvertError( sRetCode ));
    }
    if ( ReportInfo.usDevice == PROGRPT_DEVICE_MLD ) {
        ProgRpt_Close();
        PifReleaseSem( husProgRptSem );
        return ( LDT_PROHBT_ADR );
    }
    usSrcFileType = ReportInfo.usFileType;

    /* --- search MLD device in another programmed report format --- */

    *puchMLDRptNo   = 0;
    usLoopRptNo    = 0;
    fFound          = FALSE;
    sRetCode        = PROGRPT_SUCCESS;

    while (( ! fFound ) && (usLoopRptNo < FileInfo.usNoOfReport )) {

        /* --- determine output device of loop record is MLD or not --- */

        usLoopRptNo++;
        sRetCode = ProgRpt_GetProgRptInfo( usLoopRptNo, &ReportInfo );
        if ( sRetCode != PROGRPT_SUCCESS ) {
            break;
        }
        if ( ReportInfo.usDevice != PROGRPT_DEVICE_MLD ) {
            continue;
        }

        /* --- determine total file type of loop record is
            as same as source record --- */

        if ( ReportInfo.usFileType == usSrcFileType ) {
            *puchMLDRptNo = usLoopRptNo;
            fFound = TRUE;
        }
    }

    ProgRpt_Close();
    PifReleaseSem( husProgRptSem );
    return ( ProgRpt_ConvertError( sRetCode ));
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRptGetEODPTDInfo( USHORT           usReportType,
*                                           USHORT           *pusNoOfRpt,
*                                           PROGRPT_EODPTDIF *paEODPTDRec )
*
*   Input   :   USHORT           usReportType - type of information.
*   Output  :   USHORT           *pusNoOfRpt  - number of EOD/PTD report
*               PROGRPT_EODPTDIF *paEODPTDRec - points to array of EOD/PTD info.
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode  - SUCCESS
*                                 LDT_KEYOVER_ADR
*                                 LDT_PROHBT_ADR
*                                   
**  Description:
*    This function retrieves information about EOD/PTD reset report.
*==========================================================================
*/
SHORT ProgRptGetEODPTDInfo( USHORT           usReportType,
                            USHORT           *pusNoOfRpt,
                            PROGRPT_EODPTDIF *paEODPTDRec )
{
    SHORT   sRetCode = PROGRPT_FILE_NOT_FOUND;           /* return code for user application */

    ASSERT(( usReportType == PROGRPT_EOD ) ||\
           ( usReportType == PROGRPT_PTD ));
    ASSERT( pusNoOfRpt != ( USHORT * )NULL );
    ASSERT( paEODPTDRec != ( PROGRPT_EODPTDIF * )NULL );

#if 0
	// There are issues and problems with Programmable Reports so we are going to disable
	// the End of Day Reset Report for now and just return that there is none available.
	//    Richard Chambers, Oct-28-2016, GenPOS Rel 2.2.1
    PifRequestSem( husProgRptSem );

    if (( sRetCode = ProgRpt_OldOpen()) == PROGRPT_SUCCESS ) {

        sRetCode = ProgRpt_GetEODPTDInfo( usReportType,
                                          pusNoOfRpt,
                                          paEODPTDRec );
        ProgRpt_Close();
    }
    PifReleaseSem( husProgRptSem );
#endif

    return ( ProgRpt_ConvertError( sRetCode ));
}


/*-------------------------------------------------------------------------
            for issue programmable report
-------------------------------------------------------------------------*/
/*
*==========================================================================
**  Synopsis:   SHORT ProgRptDeptRead( UCHAR  uchProgRptNo,
*                                      UCHAR  uchDailyPTD,
*                                      USHORT usDeptNo,
*                                      UCHAR  uchMjrDeptNo )
*
*   Input   :   UCHAR  uchProgRptNo  - programmable report no.
*               UCHAR  uchDailyPTD   - target total file type (Daily/PTD)
*               USHORT usDeptNo      - target department no.
*               UCHAR  uchMjrDeptNo  - target major department no.
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode  - SUCCESS
*                                 RPT_ABORTED
*                                 LDT_KEYOVER_ADR
*                                 LDT_NTINFL_ADR
*                                 LDT_ERR_ADR
*                                 LDT_PROHBT_ADR
*
**  Description:
*    This function reports department read report.
*==========================================================================
*/
SHORT ProgRptDeptRead( UCHAR  uchProgRptNo, /* 1...8 */
                       UCHAR  uchDailyPTD,
                       USHORT usDeptNo,     /* 0(All Read), 1...50 */
                       UCHAR  uchMjrDeptNo )/* 0(All Read), 1...10 */
{
    SHORT   sLDTCode;       /* return code for user application */
    SHORT   sRetCode;       /* return code for internal function */
    UCHAR   uchMinorClass;  /* minor class for total file */
    USHORT  usPrintStsSave; /* print status save area */

    ASSERT(( uchDailyPTD == ( UCHAR )RPT_TYPE_DALY ) ||\
           ( uchDailyPTD == ( UCHAR )RPT_TYPE_PTD ));

    if ( ProgRpt_DeptIsBadLoopKey( &sLDTCode,
                                   uchProgRptNo,
                                   usDeptNo,
                                   uchMjrDeptNo )) {
        return ( sLDTCode );
    }

    /* --- prepare programmable report file to report --- */

    PifRequestSem( husProgRptSem );

    if (( sRetCode = ProgRpt_OldOpen()) != PROGRPT_SUCCESS ) {
        PifReleaseSem( husProgRptSem );
        return ( ProgRpt_ConvertError( sRetCode ));
    }

    /* --- initialize accumulator before execute programmable report --- */

    ProgRpt_InitAccumulator();

    /* --- display header of department read report --- */

    if ( uchDailyPTD == ( UCHAR )RPT_TYPE_DALY ) {
        uchMinorClass = CLASS_TTLCURDAY;
    } else {
        uchMinorClass = CLASS_TTLCURPTD;
    }

    ProgRpt_PrintHeader( uchMinorClass,
                         uchProgRptNo,
                         RPT_DEPT_ADR,
                         RPT_READ_REPORT );
    MaintIncreSpcCo( SPCO_CONSEC_ADR );

    /* --- determine output printer with MDC option --- */

    RptPrtStatusSet( uchMinorClass );

    /* --- if EJ system, not report on EJ ( only header & trailer ) --- */

    usPrintStsSave = usRptPrintStatus;
    if ( fProgRptWithEJ ) {
        usRptPrintStatus &= ~( PRT_JOURNAL );
    }

    /* --- display department total with specified programmed format --- */

    if (( sLDTCode = ProgRpt_DeptPrint( uchProgRptNo,
                                        uchMinorClass,
                                        usDeptNo,
                                        uchMjrDeptNo )) == RPT_ABORTED ) {

        /* --- report is aborted by user while printing --- */

        RptFeed( RPT_DEFALTFEED );
        MaintMakeAbortKey();
    }

    /* --- restore print status --- */

    usRptPrintStatus = usPrintStsSave;

    /* --- display trailer of department read report --- */

    MaintMakeTrailer( CLASS_MAINTTRAILER_PRTSUP );

    /* --- clean up prog. report file before exit this function --- */

    ProgRpt_Close();
    PifReleaseSem( husProgRptSem );

    return ( sLDTCode );
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRptPluRead( UCHAR  uchProgRptNo,
*                                     UCHAR  uchDailyPTD,
*                                     UCHAR  *puchPluNo,
*                                     USHORT usDeptNo,
*                                     UCHAR  uchReportCode )
*
*   Input   :   UCHAR  uchProgRptNo  - programmable report no.
*               UCHAR  uchDailyPTD   - target total file type (Daily/PTD)
*               UCHAR  *puchPluNo    - target plu no.
*               USHORT usDeptNo      - target department no.
*               UCHAR  uchReportCode - target report code.
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode  - SUCCESS
*                                 RPT_ABORTED
*                                 LDT_KEYOVER_ADR
*                                 LDT_NTINFL_ADR
*                                 LDT_ERR_ADR
*                                 LDT_PROHBT_ADR
*
**  Description:
*    This function reports plu read report.
*==========================================================================
*/
SHORT ProgRptPluRead( UCHAR  uchProgRptNo,  /* 1...8 */
                      UCHAR  uchDailyPTD,
                      TCHAR  *puchPluNo,    /* 0(All Read), 13 digits */
                      USHORT usDeptNo,      /* 0(All Read), 1...50 */
                      UCHAR  uchReportCode )/* 0(All Read), 1...10 */
{
    SHORT   sLDTCode;       /* return code for user application */
    SHORT   sRetCode;       /* return code for internal function */
    UCHAR   uchMinorClass;  /* minor class for total file */
    USHORT  usPrintStsSave; /* print status save area */

    ASSERT(( uchDailyPTD == ( UCHAR )RPT_TYPE_DALY ) ||\
           ( uchDailyPTD == ( UCHAR )RPT_TYPE_PTD ));

    if ( ProgRpt_PluIsBadLoopKey( &sLDTCode,
                                  uchProgRptNo,
                                  puchPluNo,
                                  usDeptNo,
                                  uchReportCode )) {
        return ( sLDTCode );
    }

    /* --- prepare programmable report file to report --- */

    PifRequestSem( husProgRptSem );

    if (( sRetCode = ProgRpt_OldOpen()) != PROGRPT_SUCCESS ) {
        PifReleaseSem( husProgRptSem );
        return ( ProgRpt_ConvertError( sRetCode ));
    }

    /* --- initialize accumulator before execute programmable report --- */

    ProgRpt_InitAccumulator();

    /* --- display header of read report --- */

    if ( uchDailyPTD == ( UCHAR )RPT_TYPE_DALY ) {
        uchMinorClass = CLASS_TTLCURDAY;
    } else {
        uchMinorClass = CLASS_TTLCURPTD;
    }

    ProgRpt_PrintHeader( uchMinorClass,
                         uchProgRptNo,
                         RPT_PLU_ADR,
                         RPT_READ_REPORT );
    MaintIncreSpcCo( SPCO_CONSEC_ADR );

    /* --- determine output printer with MDC option --- */

    RptPrtStatusSet( uchMinorClass );

    /* --- if EJ system, not report on EJ ( only header & trailer ) --- */

    usPrintStsSave = usRptPrintStatus;
    if ( fProgRptWithEJ ) {
        usRptPrintStatus &= ~( PRT_JOURNAL );
    }

    /* --- display plu total with specified programmed format --- */

    if (( sLDTCode = ProgRpt_PluPrint( uchProgRptNo,
                                       uchMinorClass,
                                       puchPluNo,
                                       usDeptNo,
                                       uchReportCode )) == RPT_ABORTED ) {

        /* --- report is aborted by user while printing --- */

        RptFeed( RPT_DEFALTFEED );
        MaintMakeAbortKey();
    }

    /* --- restore print status --- */

    usRptPrintStatus = usPrintStsSave;

    /* --- display trailer of plu read report --- */

    MaintMakeTrailer( CLASS_MAINTTRAILER_PRTSUP );

    /* --- clean up prog. report file before exit this function --- */

    ProgRpt_Close();
    PifReleaseSem( husProgRptSem );

    return ( sLDTCode );
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRptPluReset( UCHAR uchProgRptNo,
*                                      UCHAR uchDailyPTD,
*                                      UCHAR uchReportType )
*
*   Input   :   UCHAR uchProgRptNo  - programmable report no.
*               UCHAR uchDailyPTD   - target total file type (Daily/PTD)
*               UCHAR uchReportType - type of reset report.
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode  - SUCCESS
*                                 RPT_ABORTED
*                                 LDT_KEYOVER_ADR
*                                 LDT_NTINFL_ADR
*                                 LDT_ERR_ADR
*                                 LDT_PROHBT_ADR
*                                 LDT_NOT_ISSUED_ADR
*
**  Description:
*    This function reports plu reset report.
*==========================================================================
*/
SHORT ProgRptPluReset( UCHAR uchProgRptNo,   /* 1...8 */
                       UCHAR uchDailyPTD,
                       UCHAR uchReportType )
{
    SHORT   sLDTCode;       /* return code for user application */
    SHORT   sRetCode;       /* return code for internal function */
    UCHAR   uchResetClass;  /* minor class for reset total */
    UCHAR   uchReadClass;   /* minor class for read resetted total */
    UCHAR   uchResetCoAddr; /* reset counter address */
    USHORT  usPrintStsSave; /* print status save area */

    ASSERT(( uchDailyPTD == ( UCHAR )RPT_TYPE_DALY ) ||\
           ( uchDailyPTD == ( UCHAR )RPT_TYPE_PTD ));
    ASSERT(( uchReportType == RPT_ONLY_PRT_RESET ) ||\
           ( uchReportType == RPT_PRT_RESET      ) ||\
           ( uchReportType == RPT_ONLY_RESET     ));

    if ( ProgRpt_PluIsBadLoopKey( &sLDTCode,
                                  uchProgRptNo,
                                  PROGRPT_ALLRESET,
                                  PROGRPT_ALLRESET,
                                  PROGRPT_ALLRESET )) {
        return ( sLDTCode );
    }

    /* --- prepare programmable report file to report --- */

    PifRequestSem( husProgRptSem );

    if (( sRetCode = ProgRpt_OldOpen()) != PROGRPT_SUCCESS ) {
        PifReleaseSem( husProgRptSem );
        return ( ProgRpt_ConvertError( sRetCode ));
    }

    /* --- initialize accumulator before execute programmable report --- */

    ProgRpt_InitAccumulator();

    if ( uchDailyPTD == ( UCHAR )RPT_TYPE_DALY ) {
        uchResetClass  = CLASS_TTLCURDAY;
        uchReadClass   = CLASS_TTLSAVDAY;
        uchResetCoAddr = SPCO_EODRST_ADR;
    } else {
        uchResetClass  = CLASS_TTLCURPTD;
        uchReadClass   = CLASS_TTLSAVPTD;
        uchResetCoAddr = SPCO_PTDRST_ADR;
    }

    /* --- reset current DAILY/PTD total --- */

    if (( uchReportType == RPT_PRT_RESET ) ||
        ( uchReportType == RPT_ONLY_RESET )) {

        if ( ! ProgRpt_PluIsResettable( uchResetClass )) {
            ProgRpt_Close();
            PifReleaseSem( husProgRptSem );
            return ( LDT_NOT_ISSUED_ADR );
        }
        if (( sLDTCode = ProgRpt_PluReset( uchResetClass )) != SUCCESS ) {
            ProgRpt_Close();
            PifReleaseSem( husProgRptSem );
            return ( sLDTCode );
        } else {                /* reset current total is successful */
            MaintIncreSpcCo( uchResetCoAddr );
        }
    }

    ProgRpt_PrintHeader( uchReadClass,
                         uchProgRptNo,
                         RPT_PLU_ADR,
                         uchReportType );
    MaintIncreSpcCo( SPCO_CONSEC_ADR );

    usPrintStsSave = usRptPrintStatus;

    if (( uchReportType == RPT_ONLY_PRT_RESET ) ||
        ( uchReportType == RPT_PRT_RESET )) {

        /* --- determine output printer with MDC option --- */

        RptPrtStatusSet( uchReadClass );

        /* --- if EJ system, not report on EJ ( only header & trailer ) --- */

        if ( fProgRptWithEJ ) {
            usRptPrintStatus &= ~( PRT_JOURNAL );
        }

        /* --- display plu total with specified programmed format --- */

        if (( sLDTCode = ProgRpt_PluPrint( uchProgRptNo,
                                           uchReadClass,
                                           PROGRPT_ALLRESET,
                                           PROGRPT_ALLRESET,
                                           PROGRPT_ALLRESET )) == RPT_ABORTED ) {

            /* --- report is aborted by user while printing --- */

            RptFeed( RPT_DEFALTFEED );
            MaintMakeAbortKey();
        }
    } else {
        sLDTCode = SUCCESS;
    }

    /* --- restore print status --- */

    usRptPrintStatus = usPrintStsSave;

    /* --- display trailer of plu reset report --- */

    MaintMakeTrailer( CLASS_MAINTTRAILER_PRTSUP );

    /* --- clean up prog. report file before exit this function --- */

    ProgRpt_Close();
    PifReleaseSem( husProgRptSem );

    return ( sLDTCode );
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRptCpnRead( UCHAR uchProgRptNo,
*                                     UCHAR uchDailyPTD,
*                                     UCHAR uchCouponNo )
*
*   Input   :   UCHAR uchProgRptNo  - programmable report no.
*               UCHAR uchDailyPTD   - target total file type (Daily/PTD)
*               UCHAR uchCouponNo   - target coupon no.
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode  - SUCCESS
*                                 RPT_ABORTED
*                                 LDT_KEYOVER_ADR
*                                 LDT_NTINFL_ADR
*                                 LDT_ERR_ADR
*                                 LDT_PROHBT_ADR
*
**  Description:
*    This function reports coupon read report.
*==========================================================================
*/
SHORT ProgRptCpnRead( UCHAR uchProgRptNo,   /* 1...8 */
                      UCHAR uchDailyPTD,
                      UCHAR uchCouponNo )   /* 0(All Read), 1...100 */
{
    SHORT   sLDTCode;       /* return code for user application */
    SHORT   sRetCode;       /* return code for internal function */
    UCHAR   uchMinorClass;  /* minor class for total file */
    USHORT  usPrintStsSave; /* print status save area */

    ASSERT(( uchDailyPTD == ( UCHAR )RPT_TYPE_DALY ) ||\
           ( uchDailyPTD == ( UCHAR )RPT_TYPE_PTD ));

    if ( ProgRpt_CpnIsBadLoopKey( &sLDTCode, uchProgRptNo, uchCouponNo )) {
        return ( sLDTCode );
    }

    /* --- prepare programmable report file to report --- */

    PifRequestSem( husProgRptSem );

    if (( sRetCode = ProgRpt_OldOpen()) != PROGRPT_SUCCESS ) {
        return ( ProgRpt_ConvertError( sRetCode ));
    }

    /* --- initialize accumulator before execute programmable report --- */

    ProgRpt_InitAccumulator();

    /* --- display header of coupon read report --- */

    if ( uchDailyPTD == ( UCHAR )RPT_TYPE_DALY ) {
        uchMinorClass = CLASS_TTLCURDAY;
    } else {
        uchMinorClass = CLASS_TTLCURPTD;
    }

    ProgRpt_PrintHeader( uchMinorClass,
                         uchProgRptNo,
                         RPT_CPN_ADR,
                         RPT_READ_REPORT );
    MaintIncreSpcCo( SPCO_CONSEC_ADR );

    /* --- determine output printer with MDC option --- */

    RptPrtStatusSet( uchMinorClass );

    /* --- if EJ system, not report on EJ ( only header & trailer ) --- */

    usPrintStsSave = usRptPrintStatus;
    if ( fProgRptWithEJ ) {
        usRptPrintStatus &= ~( PRT_JOURNAL );
    }

    /* --- display department total with specified programmed format --- */

    if (( sLDTCode = ProgRpt_CpnPrint( uchProgRptNo,
                                       uchMinorClass,
                                       uchCouponNo )) == RPT_ABORTED ) {

        /* --- report is aborted by user while printing --- */

        RptFeed( RPT_DEFALTFEED );
        MaintMakeAbortKey();
    }

    /* --- restore print status --- */

    usRptPrintStatus = usPrintStsSave;

    /* --- display trailer of coupon read report --- */

    MaintMakeTrailer( CLASS_MAINTTRAILER_PRTSUP );

    /* --- clean up prog. report file before exit this function --- */

    ProgRpt_Close();
    PifReleaseSem( husProgRptSem );

    return ( sLDTCode );
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRptCpnReset( UCHAR uchProgRptNo,
*                                      UCHAR uchDailyPTD,
*                                      UCHAR uchReportType )
*
*   Input   :   UCHAR uchProgRptNo  - programmable report no.
*               UCHAR uchDailyPTD   - target total file type (Daily/PTD)
*               UCHAR uchReportType - type of reset report.
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode  - SUCCESS
*                                 RPT_ABORTED
*                                 LDT_KEYOVER_ADR
*                                 LDT_NTINFL_ADR
*                                 LDT_ERR_ADR
*                                 LDT_PROHBT_ADR
*                                 LDT_NOT_ISSUED_ADR
*
**  Description:
*    This function reports coupon reset report.
*==========================================================================
*/
SHORT ProgRptCpnReset( UCHAR uchProgRptNo,  /* 1...8 */
                       UCHAR uchDailyPTD,
                       UCHAR uchReportType )
{
    SHORT   sLDTCode;       /* return code for user application */
    SHORT   sRetCode;       /* return code for internal function */
    UCHAR   uchResetClass;  /* minor class for reset total */
    UCHAR   uchReadClass;   /* minor class for read resetted total */
    UCHAR   uchResetCoAddr; /* reset counter address */
    USHORT  usPrintStsSave; /* print status save area */

    ASSERT(( uchDailyPTD == ( UCHAR )RPT_TYPE_DALY ) ||\
           ( uchDailyPTD == ( UCHAR )RPT_TYPE_PTD ));
    ASSERT(( uchReportType == RPT_ONLY_PRT_RESET ) ||\
           ( uchReportType == RPT_PRT_RESET      ) ||\
           ( uchReportType == RPT_ONLY_RESET     ));

    if ( ProgRpt_CpnIsBadLoopKey( &sLDTCode,
                                  uchProgRptNo,
                                  PROGRPT_ALLRESET )) {
        return ( sLDTCode );
    }

    /* --- prepare programmable report file to report --- */

    PifRequestSem( husProgRptSem );

    if (( sRetCode = ProgRpt_OldOpen()) != PROGRPT_SUCCESS ) {
        PifReleaseSem( husProgRptSem );
        return ( ProgRpt_ConvertError( sRetCode ));
    }

    /* --- initialize accumulator before execute programmable report --- */

    ProgRpt_InitAccumulator();

    if ( uchDailyPTD == ( UCHAR )RPT_TYPE_DALY ) {
        uchResetClass  = CLASS_TTLCURDAY;
        uchReadClass   = CLASS_TTLSAVDAY;
        uchResetCoAddr = SPCO_EODRST_ADR;
    } else {
        uchResetClass  = CLASS_TTLCURPTD;
        uchReadClass   = CLASS_TTLSAVPTD;
        uchResetCoAddr = SPCO_PTDRST_ADR;
    }

    /* --- reset current DAILY/PTD total --- */

    if (( uchReportType == RPT_PRT_RESET ) ||
        ( uchReportType == RPT_ONLY_RESET )) {

        if ( ! ProgRpt_CpnIsResettable( uchResetClass )) {
            ProgRpt_Close();
            PifReleaseSem( husProgRptSem );
            return ( LDT_NOT_ISSUED_ADR );
        }
        if (( sLDTCode = ProgRpt_CpnReset( uchResetClass )) != SUCCESS ) {
            ProgRpt_Close();
            PifReleaseSem( husProgRptSem );
            return ( sLDTCode );
        } else {                /* reset current total is successful */
            MaintIncreSpcCo( uchResetCoAddr );
        }
    }

    ProgRpt_PrintHeader( uchReadClass,
                         uchProgRptNo,
                         RPT_CPN_ADR,
                         uchReportType );
    MaintIncreSpcCo( SPCO_CONSEC_ADR );

    usPrintStsSave = usRptPrintStatus;

    if (( uchReportType == RPT_ONLY_PRT_RESET ) ||
        ( uchReportType == RPT_PRT_RESET )) {

        /* --- determine output printer with MDC option --- */

        RptPrtStatusSet( uchReadClass );

        /* --- if EJ system, not report on EJ ( only header & trailer ) --- */

        if ( fProgRptWithEJ ) {
            usRptPrintStatus &= ~( PRT_JOURNAL );
        }

        /* --- display plu total with specified programmed format --- */

        if (( sLDTCode = ProgRpt_CpnPrint( uchProgRptNo,
                                           uchReadClass,
                                           PROGRPT_ALLRESET )) == RPT_ABORTED ) {

            /* --- report is aborted by user while printing --- */

            RptFeed( RPT_DEFALTFEED );
            MaintMakeAbortKey();
        }
    } else {
        sLDTCode = SUCCESS;
    }

    /* --- restore print status --- */

    usRptPrintStatus = usPrintStsSave;

    /* --- display trailer of coupon reset report --- */

    MaintMakeTrailer( CLASS_MAINTTRAILER_PRTSUP );

    /* --- clean up prog. report file before exit this function --- */

    ProgRpt_Close();
    PifReleaseSem( husProgRptSem );

    return ( sLDTCode );
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRptCasRead( UCHAR  uchProgRptNo,
*                                     USHORT usCashierNo )
*
*   Input   :   UCHAR  uchProgRptNo  - programmable report no.
*               USHORT usCashierNo   - target cashier no.
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode  - SUCCESS
*                                 RPT_ABORTED
*                                 LDT_KEYOVER_ADR
*                                 LDT_NTINFL_ADR
*                                 LDT_ERR_ADR
*                                 LDT_PROHBT_ADR
*
**  Description:
*    This function reports cashier read report. V3.3
*==========================================================================
*/
SHORT ProgRptCasRead( UCHAR  uchProgRptNo,  /* 1...8 */
                      UCHAR  uchDailyPTD,
                      ULONG  ulCashierNo )  /* 0(All Read), 1...999 */
{
    SHORT   sLDTCode;       /* return code for user application */
    SHORT   sRetCode;       /* return code for internal function */
    UCHAR   uchMinorClass;  /* minor class for total file */
    USHORT  usPrintStsSave; /* print status save area */

    /* V3.3 */
    ASSERT(( uchDailyPTD == ( UCHAR )RPT_TYPE_DALY ) || ( uchDailyPTD == ( UCHAR )RPT_TYPE_PTD ));

    if ( ProgRpt_CasIsBadLoopKey( &sLDTCode, uchProgRptNo, ulCashierNo )) {
        return ( sLDTCode );
    }

    /* --- prepare programmable report file to report --- */
    PifRequestSem( husProgRptSem );

    if (( sRetCode = ProgRpt_OldOpen()) != PROGRPT_SUCCESS ) {
        return ( ProgRpt_ConvertError( sRetCode ));
    }

    /* --- initialize accumulator before execute programmable report --- */
    ProgRpt_InitAccumulator();

    /* --- display header of cashier read report --- */
    if ( uchDailyPTD == ( UCHAR )RPT_TYPE_DALY ) {
        uchMinorClass = CLASS_TTLCURDAY;
    } else {
        uchMinorClass = CLASS_TTLCURPTD;
    }

    ProgRpt_PrintHeader( uchMinorClass, uchProgRptNo, RPT_CAS_ADR, RPT_READ_REPORT );
    MaintIncreSpcCo( SPCO_CONSEC_ADR );

    /* --- determine output printer with MDC option --- */
    RptPrtStatusSet( uchMinorClass );

    /* --- if EJ system, not report on EJ ( only header & trailer ) --- */
    usPrintStsSave = usRptPrintStatus;
    if ( fProgRptWithEJ ) {
        usRptPrintStatus &= ~( PRT_JOURNAL );
    }

    /* --- display cashier total with specified programmed format --- */
    if (( sLDTCode = ProgRpt_CasPrint( uchProgRptNo, uchMinorClass, ulCashierNo )) == RPT_ABORTED ) {
        /* --- report is aborted by user while printing --- */
        RptFeed( RPT_DEFALTFEED );
        MaintMakeAbortKey();
    }

    /* --- restore print status --- */
    usRptPrintStatus = usPrintStsSave;

    /* --- display trailer of cashier read report --- */
    MaintMakeTrailer( CLASS_MAINTTRAILER_PRTSUP );

    /* --- clean up prog. report file before exit this function --- */
    ProgRpt_Close();
    PifReleaseSem( husProgRptSem );

    return ( sLDTCode );
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRptCasReset( UCHAR uchProgRptNo,
*                                      UCHAR uchDailyPTD,
*                                      UCHAR uchReportType,
*                                      USHORT usCashierNo )
*
*   Input   :   UCHAR uchProgRptNo  - programmable report no.
*               UCHAR uchDailyPTD   - target total file type (Daily/PTD)
*               UCHAR uchReportType - type of reset report.
*               USHORT usCashierNo  - target cashier no.
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode  - SUCCESS
*                                 RPT_ABORTED
*                                 LDT_KEYOVER_ADR
*                                 LDT_NTINFL_ADR
*                                 LDT_ERR_ADR
*                                 LDT_PROHBT_ADR
*                                 LDT_NOT_ISSUED_ADR
*
**  Description:
*    This function reports cashier reset report. V3.3
*==========================================================================
*/
SHORT ProgRptCasReset( UCHAR uchProgRptNo,  /* 1...8 */
                       UCHAR uchDailyPTD,
                       UCHAR uchReportType,
                       ULONG ulCashierNo ) /* 0(All Read), 1...999 */
{
    SHORT   sLDTCode;       /* return code for user application */
    SHORT   sRetCode;       /* return code for internal function */
    USHORT  usPrintStsSave; /* print status save area */
    UCHAR   uchResetClass;  /* minor class for reset total */
    UCHAR   uchReadClass;   /* minor class for read resetted total */
    UCHAR   uchResetCoAddr; /* reset counter address */

    /* V3.3 */
    ASSERT(( uchDailyPTD == ( UCHAR )RPT_TYPE_DALY ) || ( uchDailyPTD == ( UCHAR )RPT_TYPE_PTD ));
    ASSERT(( uchReportType == RPT_ONLY_PRT_RESET) || (uchReportType == RPT_PRT_RESET) || (uchReportType == RPT_ONLY_RESET));

    if ( ProgRpt_CasIsBadLoopKey( &sLDTCode, uchProgRptNo, ulCashierNo )) {
        return ( sLDTCode );
    }

    /* --- prepare programmable report file to report --- */
    PifRequestSem( husProgRptSem );

    if (( sRetCode = ProgRpt_OldOpen()) != PROGRPT_SUCCESS ) {
        PifReleaseSem( husProgRptSem );
        return ( ProgRpt_ConvertError( sRetCode ));
    }

    /* --- initialize accumulator before execute programmable report --- */
    ProgRpt_InitAccumulator();

    if ( uchDailyPTD == ( UCHAR )RPT_TYPE_DALY ) {
        uchResetClass  = CLASS_TTLCURDAY;
        uchReadClass   = CLASS_TTLSAVDAY;
        uchResetCoAddr = SPCO_EODRST_ADR;
    } else {
        uchResetClass  = CLASS_TTLCURPTD;
        uchReadClass   = CLASS_TTLSAVPTD;
        uchResetCoAddr = SPCO_PTDRST_ADR;
    }

    /* --- reset current DAILY total --- */
    if (( uchReportType == RPT_PRT_RESET ) || ( uchReportType == RPT_ONLY_RESET )) {

        if ( ! ProgRpt_CasIsResettable(uchResetClass, ulCashierNo )) {
            ProgRpt_Close();
            PifReleaseSem( husProgRptSem );
            return ( LDT_NOT_ISSUED_ADR );
        }
        if (( sLDTCode = ProgRpt_CasReset(uchResetClass, ulCashierNo )) != SUCCESS ) {
            ProgRpt_Close();
            PifReleaseSem( husProgRptSem );
            return ( sLDTCode );
        } else {                /* reset current total is successful */
            MaintIncreSpcCo( SPCO_EODRST_ADR );
        }
    }

    ProgRpt_PrintHeader( uchReadClass, uchProgRptNo, RPT_CAS_ADR, uchReportType );
    MaintIncreSpcCo( SPCO_CONSEC_ADR );

    usPrintStsSave = usRptPrintStatus;

    if (( uchReportType == RPT_ONLY_PRT_RESET ) || ( uchReportType == RPT_PRT_RESET )) {

        /* --- determine output printer with MDC option --- */
        RptPrtStatusSet( uchReadClass );

        /* --- if EJ system, not report on EJ ( only header & trailer ) --- */
        if ( fProgRptWithEJ ) {
            usRptPrintStatus &= ~( PRT_JOURNAL );
        }

        /* --- display plu total with specified programmed format --- */
        if (( sLDTCode = ProgRpt_CasPrint( uchProgRptNo, uchReadClass, ulCashierNo )) == RPT_ABORTED ) {
            /* --- report is aborted by user while printing --- */
            RptFeed( RPT_DEFALTFEED );
            MaintMakeAbortKey();
        }
    } else {
        ProgRpt_CasPrintResetCasID( uchProgRptNo, ulCashierNo );
        sLDTCode = SUCCESS;
    }

    /* --- restore print status --- */
    usRptPrintStatus = usPrintStsSave;

    /* --- display trailer of coupon reset report --- */
    MaintMakeTrailer( CLASS_MAINTTRAILER_PRTSUP );

    /* --- clean up prog. report file before exit this function --- */
    ProgRpt_Close();
    PifReleaseSem( husProgRptSem );

    return ( sLDTCode );
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRptFinRead( UCHAR uchProgRptNo,
*                                     UCHAR uchDailyPTD )
*
*   Input   :   UCHAR  uchProgRptNo  - programmable report no.
*               UCHAR  uchDailyPTD   - target total file type (Daily/PTD)
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode  - SUCCESS
*                                 RPT_ABORTED
*                                 LDT_KEYOVER_ADR
*                                 LDT_NTINFL_ADR
*                                 LDT_ERR_ADR
*                                 LDT_PROHBT_ADR
*
**  Description:
*    This function reports financial read report.
*==========================================================================
*/
SHORT ProgRptFinRead( UCHAR uchProgRptNo,   /* 1...8 */
                      UCHAR uchDailyPTD )
{
    SHORT   sLDTCode;       /* return code for user application */
    SHORT   sRetCode;       /* return code for internal function */
    UCHAR   uchMinorClass;  /* minor class for total file */
    USHORT  usPrintStsSave; /* print status save area */

    ASSERT(( uchDailyPTD == ( UCHAR )RPT_TYPE_DALY ) || ( uchDailyPTD == ( UCHAR )RPT_TYPE_PTD ));

    /* --- determine specified prog. report no. is valid or invalid --- */
    if (( uchProgRptNo < 1 ) || ( MAX_PROGRPT_FORMAT < uchProgRptNo )) {
        return ( LDT_KEYOVER_ADR );
    }

    /* --- prepare programmable report file to report --- */
    PifRequestSem( husProgRptSem );

    if (( sRetCode = ProgRpt_OldOpen()) != PROGRPT_SUCCESS ) {
        return ( ProgRpt_ConvertError( sRetCode ));
    }

    /* --- initialize accumulator before execute programmable report --- */
    ProgRpt_InitAccumulator();

    /* --- display header of cashier read report --- */
    if ( uchDailyPTD == ( UCHAR )RPT_TYPE_DALY ) {
        uchMinorClass = CLASS_TTLCURDAY;
    } else {
        uchMinorClass = CLASS_TTLCURPTD;
    }

    ProgRpt_PrintHeader( uchMinorClass, uchProgRptNo, RPT_FINANC_ADR, RPT_READ_REPORT );
    MaintIncreSpcCo( SPCO_CONSEC_ADR );

    /* --- determine output printer with MDC option --- */
    RptPrtStatusSet( uchMinorClass );

    /* --- if EJ system, not report on EJ ( only header & trailer ) --- */
    usPrintStsSave = usRptPrintStatus;
    if ( fProgRptWithEJ ) {
        usRptPrintStatus &= ~( PRT_JOURNAL );
    }

    /* --- display financial total with specified programmed format --- */
    if (( sLDTCode = ProgRpt_FinPrint( uchProgRptNo, uchMinorClass )) == RPT_ABORTED ) {
        /* --- report is aborted by user while printing --- */
        RptFeed( RPT_DEFALTFEED );
        MaintMakeAbortKey();
    }

    /* --- restore print status --- */
    usRptPrintStatus = usPrintStsSave;

    /* --- display trailer of cashier read report --- */
    MaintMakeTrailer( CLASS_MAINTTRAILER_PRTSUP );

    /* --- clean up prog. report file before exit this function --- */
    ProgRpt_Close();
    PifReleaseSem( husProgRptSem );

    return ( sLDTCode );
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRptIndFinRead( UCHAR  uchProgRptNo,
*                                         USHORT usTerminalNo )
*
*   Input   :   UCHAR  uchProgRptNo  - programmable report no.
*               USHORT usTerminalNo  - target terminal no.
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode  - SUCCESS
*                                 RPT_ABORTED
*                                 LDT_KEYOVER_ADR
*                                 LDT_NTINFL_ADR
*                                 LDT_ERR_ADR
*                                 LDT_PROHBT_ADR
*
**  Description:
*    This function reports individual terminal financial read report.
*==========================================================================
*/
SHORT ProgRptIndFinRead( UCHAR  uchProgRptNo,   /* 1...8 */
                         USHORT usTerminalNo )  /* 0(All Read), 1...16 */
{
    SHORT   sLDTCode;       /* return code for user application */
    SHORT   sRetCode;       /* return code for internal function */
    USHORT  usPrintStsSave; /* print status save area */

    if ( ProgRpt_IndIsBadLoopKey( &sLDTCode, uchProgRptNo, usTerminalNo )) {
        return ( sLDTCode );
    }

    /* --- prepare programmable report file to report --- */
    PifRequestSem( husProgRptSem );

    if (( sRetCode = ProgRpt_OldOpen()) != PROGRPT_SUCCESS ) {
        return ( ProgRpt_ConvertError( sRetCode ));
    }

    /* --- initialize accumulator before execute programmable report --- */
    ProgRpt_InitAccumulator();

    /* --- display header of cashier read report --- */
    ProgRpt_PrintHeader( CLASS_TTLCURDAY, uchProgRptNo, RPT_FINANC_ADR, RPT_READ_REPORT );
    MaintIncreSpcCo( SPCO_CONSEC_ADR );

    /* --- determine output printer with MDC option --- */
    RptPrtStatusSet( CLASS_TTLCURDAY );

    /* --- if EJ system, not report on EJ ( only header & trailer ) --- */
    usPrintStsSave = usRptPrintStatus;
    if ( fProgRptWithEJ ) {
        usRptPrintStatus &= ~( PRT_JOURNAL );
    }

    /* --- display cashier total with specified programmed format --- */
    if (( sLDTCode = ProgRpt_IndPrint( uchProgRptNo, CLASS_TTLCURDAY, usTerminalNo )) == RPT_ABORTED ) {
        /* --- report is aborted by user while printing --- */
        RptFeed( RPT_DEFALTFEED );
        MaintMakeAbortKey();
    }

    /* --- restore print status --- */
    usRptPrintStatus = usPrintStsSave;

    /* --- display trailer of cashier read report --- */
    MaintMakeTrailer( CLASS_MAINTTRAILER_PRTSUP );

    /* --- clean up prog. report file before exit this function --- */
    ProgRpt_Close();
    PifReleaseSem( husProgRptSem );

    return ( sLDTCode );
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRptIndFinReset( UCHAR uchProgRptNo,
*                                         UCHAR uchReportType,
*                                         USHORT usTerminalNo )
*
*   Input   :   UCHAR uchProgRptNo  - programmable report no.
*               UCHAR uchReportType - type of reset report.
*               USHORT usTerminalNo - target terminal no.
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode  - SUCCESS
*                                 RPT_ABORTED
*                                 LDT_KEYOVER_ADR
*                                 LDT_NTINFL_ADR
*                                 LDT_ERR_ADR
*                                 LDT_PROHBT_ADR
*                                 LDT_NOT_ISSUED_ADR
*
**  Description:
*    This function reports individual terminal reset report.
*==========================================================================
*/
SHORT ProgRptIndFinReset( UCHAR uchProgRptNo,   /* 1...8 */
                          UCHAR uchReportType,
                          USHORT usTerminalNo ) /* 0(All Read), 1...16 */
{
    SHORT   sLDTCode;       /* return code for user application */
    SHORT   sRetCode;       /* return code for internal function */
    USHORT  usPrintStsSave; /* print status save area */

    ASSERT(( uchReportType == RPT_ONLY_PRT_RESET ) ||\
           ( uchReportType == RPT_PRT_RESET      ) ||\
           ( uchReportType == RPT_ONLY_RESET     ));

    if ( ProgRpt_IndIsBadLoopKey( &sLDTCode, uchProgRptNo, usTerminalNo )) {
        return ( sLDTCode );
    }

    /* --- prepare programmable report file to report --- */
    PifRequestSem( husProgRptSem );

    if (( sRetCode = ProgRpt_OldOpen()) != PROGRPT_SUCCESS ) {
        PifReleaseSem( husProgRptSem );
        return ( ProgRpt_ConvertError( sRetCode ));
    }

    /* --- initialize accumulator before execute programmable report --- */
    ProgRpt_InitAccumulator();

    /* --- reset current DAILY/PTD total --- */
    if (( uchReportType == RPT_PRT_RESET ) ||
        ( uchReportType == RPT_ONLY_RESET )) {

        if ( ! ProgRpt_IndIsResettable( usTerminalNo )) {
            ProgRpt_Close();
            PifReleaseSem( husProgRptSem );
            return ( LDT_NOT_ISSUED_ADR );
        }
        if (( sLDTCode = ProgRpt_IndReset( RPT_ALL_RESET, usTerminalNo )) != SUCCESS ) {
            ProgRpt_Close();
            PifReleaseSem( husProgRptSem );
            return ( sLDTCode );
        } else {                /* reset current total is successful */
            MaintIncreSpcCo( SPCO_EODRST_ADR );
        }
    }

    ProgRpt_PrintHeader( CLASS_TTLSAVDAY, uchProgRptNo, RPT_FINANC_ADR, uchReportType );
    MaintIncreSpcCo( SPCO_CONSEC_ADR );

    usPrintStsSave = usRptPrintStatus;

    if (( uchReportType == RPT_ONLY_PRT_RESET ) || ( uchReportType == RPT_PRT_RESET )) {
        /* --- determine output printer with MDC option --- */
        RptPrtStatusSet( CLASS_TTLSAVDAY );

        /* --- if EJ system, not report on EJ ( only header & trailer ) --- */
        if ( fProgRptWithEJ ) {
            usRptPrintStatus &= ~( PRT_JOURNAL );
        }

        /* --- display plu total with specified programmed format --- */
        if (( sLDTCode = ProgRpt_IndPrint( uchProgRptNo, CLASS_TTLSAVDAY, usTerminalNo )) == RPT_ABORTED ) {
            /* --- report is aborted by user while printing --- */
            RptFeed( RPT_DEFALTFEED );
            MaintMakeAbortKey();
        }
    } else {
        ProgRpt_IndPrintResetTermID( uchProgRptNo, usTerminalNo );
        sLDTCode = SUCCESS;
    }

    /* --- restore print status --- */
    usRptPrintStatus = usPrintStsSave;

    /* --- display trailer of coupon reset report --- */
    MaintMakeTrailer( CLASS_MAINTTRAILER_PRTSUP );

    /* --- clean up prog. report file before exit this function --- */
    ProgRpt_Close();
    PifReleaseSem( husProgRptSem );

    return ( sLDTCode );
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRptEODReset( UCHAR             uchReportType,
*                                      PROGRPT_EODPTDIF *paEODRec,
*                                      USHORT            usNoOfRpt )
*
*   Input   :   UCHAR            uchReportType - report type of EOD report.
*               PROGRPT_EODPTDIF *paEODRec     - points to array of EOD info.
*               USHORT           usNoOfRpt     - number of EOD report
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode  - SUCCESS
*                                 LDT_KEYOVER_ADR
*                                 LDT_ERR_ADR
*                                 LDT_PROHBT_ADR
*                                 LDT_NOT_ISSUED_ADR
*                                   
**  Description:
*    This function resets the specified total file, and then reports the
*   resetted total at EOD reset report.
*==========================================================================
*/
SHORT ProgRptEODReset( UCHAR            uchReportType,
                       PROGRPT_EODPTDIF *paEODRec,
                       USHORT           usNoOfRpt )
{
    SHORT   sRetCode;       /* return code for internal function */
    UCHAR   uchProgRptNo;   /* programmable report no */

    ASSERT(( uchReportType == RPT_ONLY_PRT_RESET ) ||\
           ( uchReportType == RPT_PRT_RESET      ) ||\
           ( uchReportType == RPT_ONLY_RESET     ));
    ASSERT( paEODRec != ( PROGRPT_EODPTDIF * )NULL );
    ASSERT(( 0 < usNoOfRpt ) && ( usNoOfRpt <= MAX_PROGRPT_EODPTD ));

    /* --- prepare programmable report file to report --- */

    PifRequestSem( husProgRptSem );

    if (( sRetCode = ProgRpt_OldOpen()) != PROGRPT_SUCCESS ) {
        PifReleaseSem( husProgRptSem );
        return ( ProgRpt_ConvertError( sRetCode ));
    }

    /* --- print header of EOD reset report --- */
    uchProgRptNo = 0;
    ProgRpt_PrintHeader( CLASS_TTLSAVDAY, uchProgRptNo, RPT_EOD_ADR, uchReportType );
    MaintIncreSpcCo( SPCO_CONSEC_ADR );

    /* --- reset current daily file before printing --- */
    if (( uchReportType == RPT_PRT_RESET  ) ||
        ( uchReportType == RPT_ONLY_RESET )) {

		ProgRptLog(5);
        if ( ! ProgRpt_EODIsResettable( paEODRec, usNoOfRpt )) {

            /* --- exit function, if there is saved daily total which
                is not issued --- */
            RptPrtError( LDT_NOT_ISSUED_ADR );

            MaintMakeTrailer( CLASS_MAINTTRAILER_PRTSUP );

            ProgRpt_Close();
            PifReleaseSem( husProgRptSem );
            return ( LDT_NOT_ISSUED_ADR );
        }

		ProgRptLog(18);
        ProgRpt_EODReset( uchReportType, paEODRec, usNoOfRpt );
		ProgRptLog(33);

        MaintIncreSpcCo( SPCO_EODRST_ADR );
    }

    /* --- report saved (resetted) daily total --- */
    if (( uchReportType == RPT_ONLY_PRT_RESET ) || ( uchReportType == RPT_PRT_RESET )) {
		ProgRptLog(34);
        sRetCode = ProgRpt_EODPrint( uchReportType, paEODRec, usNoOfRpt );
		ProgRptLog(57);
    }

    /* --- print out EOD trailer --- */
    MaintMakeTrailer( CLASS_MAINTTRAILER_PRTSUP );

    if ( sRetCode == RPT_ABORTED ) {
        /* --- report is aborted by user --- */
        ProgRpt_Close();
        PifReleaseSem( husProgRptSem );
        return ( SUCCESS );
    }

    if (( uchReportType == RPT_PRT_RESET  ) || ( uchReportType == RPT_ONLY_RESET )) {

        if ( CliParaMDCCheck( MDC_SUPER_ADR, ODD_MDC_BIT1 )) {
            /* --- reset consecutive conter, if MDC option is selected --- */
			MaintResetSpcCo(SPCO_CONSEC_ADR);
        }
    }

    ProgRpt_Close();
    PifReleaseSem( husProgRptSem );
    return ( SUCCESS );
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRptPTDReset( UCHAR             uchReportType,
*                                      PROGRPT_EODPTDIF *paPTDRec,
*                                      USHORT            usNoOfRpt )
*
*   Input   :   UCHAR            uchReportType - report type of PTD report.
*               PROGRPT_EODPTDIF *paPTDRec     - points to array of PTD info.
*               USHORT           usNoOfRpt     - number of PTD report
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode  - SUCCESS
*                                 LDT_KEYOVER_ADR
*                                 LDT_ERR_ADR
*                                 LDT_PROHBT_ADR
*                                 LDT_NOT_ISSUED_ADR
*                                   
**  Description:
*    This function resets the specified total file, and then reports the
*   resetted total at PTD reset report.
*==========================================================================
*/
SHORT ProgRptPTDReset( UCHAR            uchReportType,
                       PROGRPT_EODPTDIF *paPTDRec,
                       USHORT           usNoOfRpt )
{
    SHORT   sRetCode;       /* return code for internal function */
    UCHAR   uchProgRptNo;   /* programmable report no */

    ASSERT(( uchReportType == RPT_ONLY_PRT_RESET ) ||\
           ( uchReportType == RPT_PRT_RESET  ) ||\
           ( uchReportType == RPT_ONLY_RESET ));
    ASSERT( paPTDRec != ( PROGRPT_EODPTDIF * )NULL );
    ASSERT(( 0 < usNoOfRpt ) && ( usNoOfRpt <= MAX_PROGRPT_PTD ));

    /* --- prepare programmable report file to report --- */
    PifRequestSem( husProgRptSem );

    if (( sRetCode = ProgRpt_OldOpen()) != PROGRPT_SUCCESS ) {
        PifReleaseSem( husProgRptSem );
        return ( ProgRpt_ConvertError( sRetCode ));
    }

    /* --- print header of PTD reset report --- */
    uchProgRptNo = 0;
    ProgRpt_PrintHeader( CLASS_TTLSAVPTD, uchProgRptNo, RPT_PTD_ADR, uchReportType );
    MaintIncreSpcCo( SPCO_CONSEC_ADR );

    /* --- reset current PTD file before printing --- */
    if (( uchReportType == RPT_PRT_RESET  ) || ( uchReportType == RPT_ONLY_RESET )) {

        if ( ! ProgRpt_PTDIsResettable( paPTDRec, usNoOfRpt )) {

            /* --- exit function, if there is saved PTD total which
                is not issued --- */
            RptPrtError( LDT_NOT_ISSUED_ADR );

            MaintMakeTrailer( CLASS_MAINTTRAILER_PRTSUP );

            ProgRpt_Close();
            PifReleaseSem( husProgRptSem );
            return ( LDT_NOT_ISSUED_ADR );
        }

        ProgRpt_PTDReset( uchReportType, paPTDRec, usNoOfRpt );

        MaintIncreSpcCo( SPCO_PTDRST_ADR );
    }

    /* --- report saved (resetted) PTD total --- */
    if (( uchReportType == RPT_ONLY_PRT_RESET ) || ( uchReportType == RPT_PRT_RESET )) {
        sRetCode = ProgRpt_PTDPrint( uchReportType, paPTDRec, usNoOfRpt );
    }

    /* --- print out PTD trailer --- */
    MaintMakeTrailer( CLASS_MAINTTRAILER_PRTSUP );

    if ( sRetCode == RPT_ABORTED ) {
        /* --- report is aborted by user --- */
        ProgRpt_Close();
        PifReleaseSem( husProgRptSem );
        return ( SUCCESS );
    }

    if (( uchReportType == RPT_PRT_RESET  ) || ( uchReportType == RPT_ONLY_RESET )) {
        if ( CliParaMDCCheck( MDC_SUPER_ADR, ODD_MDC_BIT1 )) {
            /* --- reset consecutive conter, if MDC option is selected --- */
			MaintResetSpcCo(SPCO_CONSEC_ADR);
        }
    }

    ProgRpt_Close();
    PifReleaseSem( husProgRptSem );
    return ( SUCCESS );
}

VOID ProgRptLog(USHORT usCode)
{
	if (CliParaMDCCheck(MDC_POWERSTATE_INDIC, EVEN_MDC_BIT0)) {
		PifLog(999, usCode);
	}
}

/* ===== End of File (PROGRPT.C) ===== */