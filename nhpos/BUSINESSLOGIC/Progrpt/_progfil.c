/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO   ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1996           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*=======================================================================*\
*   Title              : Programmable Report module, Program List
*   Category           : Prog. Report, NCR2170 US HOSPITALITY APPLICATION
*   Program Name       : _PROGFIL.C
*  ------------------------------------------------------------------------
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.
*   Memory Model       : Midium Model
*   Options            : /c /AM /G1s /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*    Abstract           :The provided function names are as follows.
*
*   ProgRpt_NewCreate()     - Create New Programmable Report File
*   ProgRpt_OldOpen()       - Open Old Programmble Report File.
*   ProgRpt_Close()         - Close Open Programmable Report File.
*   ProgRpt_Delete()        - Delete Programmable Report File.
*   ProgRpt_ExpandFile()    - Expand Programmable Report File Size.
*
*   ProgRpt_LoadNoOfBlock() - Retrieve PROGRPT file size in file block.
*   ProgRpt_InitFileHeader()- Initialize PROGRPT file header.
*   ProgRpt_GetFileHeader() - Retrieve PROGRPT file header.
*
*   ProgRpt_ReadFile()      - Read a data from PROGRPT file at position.
*   ProgRpt_WriteFile()     - Write a data to PROGRPT file at position.
*
*  ------------------------------------------------------------------------
*    Update Histories  :
*                                 
*    Date      Ver.Rev :NAME       :Description
*    Feb-29-96:00.00.01:T.Nakahata :Initial
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

#include	<tchar.h>
#include    <string.h>
#include    <ecr.h>
#include    <pif.h>
#include    <log.h>
#include    <appllog.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csttl.h>
#include    <cscas.h>
#include    <cswai.h>
#include    <csop.h>
#include    <progrept.h>
#include    <csprgrpt.h>
#include    "_progrpt.h"

/*
===========================================================================
    GLOBAL VARIABLES DECLARATIONs
===========================================================================
*/
    /* --- name of programmable report file --- */
TCHAR FARCONST auchPROG_RPT[] = _T("PARAMRPT");

    /* --- file handle of programmable report file --- */
SHORT   hsProgRptFile = PROGRPT_INVALID_HANDLE;

    /* --- actual file size of programmable report file --- */
ULONG   ulProgRptFileSize = 0L;

/*
===========================================================================
    PROGRAMMABLE REPORT FILE I/F FUNCTIONs
===========================================================================
*/
/*
*==========================================================================
**  Synopsis:   VOID ProgRpt_InitFile( VOID )
*
*   Input   :   Nothing
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   Nothing
*               
**  Description:
*    This function initializes PROGRAMMABLE REPORT file.
*==========================================================================
*/
VOID ProgRpt_InitFile( VOID )
{
    hsProgRptFile = PROGRPT_INVALID_HANDLE;
    ulProgRptFileSize = 0L;
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRpt_NewCreate( VOID )
*
*   Input   :   Nothing
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode      - PROGRPT_SUCCESS
*                                   - PROGRPT_NO_MAKE_FILE
*               
**  Description:
*    This function creates the PROGRAMMABLE REPORT file. The function fails
*   if the PROGRAMMABLE REPORT file already exists.
*==========================================================================
*/
SHORT ProgRpt_NewCreate( VOID )
{
    SHORT   sRetCode;       /* return code for user application */

    ASSERT( hsProgRptFile <= PROGRPT_INVALID_HANDLE );

    sRetCode = PROGRPT_SUCCESS;

    hsProgRptFile = PifOpenFile( auchPROG_RPT, auchNEW_FILE_WRITE );
    if ( hsProgRptFile <= PROGRPT_INVALID_HANDLE ) {
        sRetCode = PROGRPT_NO_MAKE_FILE;
    }
    return ( sRetCode );
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRpt_OldOpen( VOID )
*
*   Input   :   Nothing
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode      - PROGRPT_SUCCESS
*                                   - PROGRPT_FILE_NOT_FOUND
*                                   - PROGRPT_INVALID_FILE
*                                   - PROGRPT_INVALID_VERSION
*                                   - PROGRPT_NO_READ_SIZE
*
**  Description:
*    This function opens the PROGRAMMABLE REPORT file. The function fails
*   if the PROGRAMMABLE REPORT file does not exist.
*==========================================================================
*/
SHORT ProgRpt_OldOpen( VOID )
{
    ULONG  ulActualRead;   /* actual read size of PifRead() function */
    PROGRPT_FHEAD FileInfo; /* information about prog report file */
    ULONG   ulActualSeek;   /* actual seek size of PifSeek() function */

    ASSERT( hsProgRptFile <= PROGRPT_INVALID_HANDLE );

    hsProgRptFile = PifOpenFile( auchPROG_RPT, auchOLD_FILE_READ_WRITE );
    if ( hsProgRptFile <= PROGRPT_INVALID_HANDLE ) {
        return ( PROGRPT_FILE_NOT_FOUND );
    }

    /* --- retrieve information about opened file to determine opened file
        is valid or not --- */

    PifSeekFile( hsProgRptFile, PROGRPT_OFFSET_FILEHEADER, &ulActualSeek );
    if ( ulActualSeek != PROGRPT_OFFSET_FILEHEADER ) {
        ProgRpt_Close();
        return ( PROGRPT_NO_READ_SIZE );
    }

	//11-10-03 JHHJ Change made in PifReadFile, see piffile.c for new usage
    PifReadFile( hsProgRptFile,( CHAR FAR * )&FileInfo, sizeof( PROGRPT_FHEAD ), &ulActualRead);

    if ( ulActualRead != sizeof( PROGRPT_FHEAD )) {
        ProgRpt_Close();
        return ( PROGRPT_NO_READ_SIZE );
    }
    if ( _tcsncmp( FileInfo.auchMark, PROGRPT_MARK_FHEAD, PROGRPT_MAX_MARK_FHEAD )) {
        ProgRpt_Close();
        return ( PROGRPT_INVALID_FILE );
    }
    if ( FileInfo.usVersion != PROGRPT_VERSION ) {
        ProgRpt_Close();
        return ( PROGRPT_INVALID_VERSION );
    }
    if ( FileInfo.ulFileSize < ( ULONG )PROGRPT_BYTE_PER_BLOCK ) {
        ProgRpt_Close();
        return ( PROGRPT_INVALID_FILE );
    }
    ulProgRptFileSize = FileInfo.ulFileSize;

    return ( PROGRPT_SUCCESS );
}

/*
*==========================================================================
**  Synopsis:   VOID ProgRpt_Close( VOID )
*
*   Input   :   Nothing
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   Nothing
*               
**  Description:
*    This function closes the PROGRAMMABLE REPORT file.
*==========================================================================
*/
VOID ProgRpt_Close( VOID )
{
    ASSERT( PROGRPT_INVALID_HANDLE < hsProgRptFile );

    PifCloseFile( hsProgRptFile );

    ProgRpt_InitFile();
}

/*
*==========================================================================
**  Synopsis:   VOID ProgRpt_Delete( VOID )
*
*   Input   :   Nothing
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   Nothing
*               
**  Description:
*    This function deletes the PROGRAMMABLE REPORT file.
*==========================================================================
*/
VOID ProgRpt_Delete( VOID )
{
    ASSERT( hsProgRptFile <= PROGRPT_INVALID_HANDLE );

    PifDeleteFile( auchPROG_RPT );
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRpt_ExpandFile( UCHAR uchNoOfBlock )
*
*   Input   :   UCHAR uchNoOfBlock  - no. of blocks for file expansion
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode      - PROGRPT_SUCCESS
*                                   - PROGRPT_DEVICE_FULL
*               
**  Description:
*    This function expands the PROGRAMMABLE REPORT file size to specified
*   file size. The function fails if the device is full.
*==========================================================================
*/
SHORT ProgRpt_ExpandFile( UCHAR uchNoOfBlock )
{
    SHORT   sRetCode;       /* return code for user application */
    SHORT   sSeekState;     /* return status for PifSeek function */
    ULONG   ulInquirySize;  /* inquiry size of PROGRPT file in bytes */
    ULONG   ulActualSize;   /* actual size of PROGRPT file in bytes */
    ULONG   ulWritePosition;/* write position for initialize file */
    UCHAR   auchInitBlock[ PROGRPT_BYTE_PER_BLOCK ];
                            /* work area for initialize file */

    ASSERT( PROGRPT_INVALID_HANDLE < hsProgRptFile );
    ASSERT( ulProgRptFileSize == 0UL );
    ASSERT( uchNoOfBlock <= FLEX_PROGRPT_MAX );

    sRetCode = PROGRPT_DEVICE_FULL;

    /* --- resize PROGRPT file to inquiry size --- */

    ulInquirySize = ( ULONG )( PROGRPT_BYTE_PER_BLOCK * uchNoOfBlock );
    sSeekState = PifSeekFile( hsProgRptFile, ulInquirySize, &ulActualSize );

    if (( sSeekState == PIF_OK ) && ( ulInquirySize == ulActualSize )) {

        /* --- initialize programmable report file --- */

        memset( auchInitBlock, 0x00, sizeof( auchInitBlock ));
        ulWritePosition = 0UL;
        while ( ulWritePosition < ulInquirySize ) {
            PifSeekFile( hsProgRptFile, ulWritePosition, &ulActualSize );
            if ( ulWritePosition != ulActualSize ) {
                return ( PROGRPT_DEVICE_FULL );
            }
            PifWriteFile( hsProgRptFile, auchInitBlock, PROGRPT_BYTE_PER_BLOCK);
            ulWritePosition += PROGRPT_BYTE_PER_BLOCK;
        }
        sRetCode = PROGRPT_SUCCESS;
    }
    return ( sRetCode );
}

/*
*==========================================================================
**  Synopsis:   UCHAR ProgRpt_LoadNoOfBlock( VOID )
*
*   Input   :   Nothing
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   UCHAR uchNoOfBlock - no. of blocks for PROGRPT file.
*               
**  Description:
*    This function retrieves a PROGRPT file size in file block.
*==========================================================================
*/
UCHAR ProgRpt_LoadNoOfBlock( VOID )
{
    UCHAR   uchNoOfBlock;   /* no. of blocks */

    ASSERT( PROGRPT_INVALID_HANDLE < hsProgRptFile );
    ASSERT( 0UL < ulProgRptFileSize );

    uchNoOfBlock = ( UCHAR )( ulProgRptFileSize / PROGRPT_BYTE_PER_BLOCK );
    ASSERT( uchNoOfBlock <= FLEX_PROGRPT_MAX );

    return ( uchNoOfBlock );
}

/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_InitFileHeader( UCHAR uchNoOfBlock )
*
*   Input   :   UCHAR uchNoOfBlock  - no. of blocks of created PROGRPT file
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   BOOL    TRUE    -   function is successful.
*                       FALSE   -   function is failed.
*               
**  Description:
*    This function initializes the file header of PROGRPT file.
*==========================================================================
*/
BOOL ProgRpt_InitFileHeader( UCHAR uchNoOfBlock )
{
    ULONG   ulActualSeek;   /* actual seek size in bytes */
    PROGRPT_FHEAD FileInfo; /* information about prog report file */
    TCHAR   auchTmpBuff[ sizeof( PROGRPT_40CHAR ) ];
    PROGRPT_EODPTD  *pEODPTD;
    PROGRPT_16CHAR  *pChar16Info;
    PROGRPT_40CHAR  *pChar40Info;

    ASSERT( PROGRPT_INVALID_HANDLE < hsProgRptFile );
    ASSERT( ulProgRptFileSize == 0UL );
    ASSERT( uchNoOfBlock <= FLEX_PROGRPT_MAX );

    /* --- initialize file header --- */

    memset( &FileInfo, 0x00, sizeof( PROGRPT_FHEAD ));

    _tcsncpy( FileInfo.auchMark, PROGRPT_MARK_FHEAD, PROGRPT_MAX_MARK_FHEAD );
    //memcpy( FileInfo.auchMark, PROGRPT_MARK_FHEAD, PROGRPT_MAX_MARK_FHEAD );
    FileInfo.usVersion      = PROGRPT_VERSION;
    FileInfo.ulFileSize     = ( ULONG )( PROGRPT_BYTE_PER_BLOCK * uchNoOfBlock );
    FileInfo.ulOffsetEOD    = sizeof( PROGRPT_FHEAD );
    FileInfo.ulOffsetPTD    = FileInfo.ulOffsetEOD + sizeof( PROGRPT_EODPTD );
    FileInfo.ulOffset16Char = FileInfo.ulOffsetPTD + sizeof( PROGRPT_EODPTD );
    FileInfo.ulOffset40Char = FileInfo.ulOffset16Char + sizeof( PROGRPT_16CHAR );
    FileInfo.usNoOfReport   = 0;

    /* --- store initialized file header --- */

    PifSeekFile( hsProgRptFile, PROGRPT_OFFSET_FILEHEADER, &ulActualSeek );
    if ( ulActualSeek != PROGRPT_OFFSET_FILEHEADER ) {
        return ( FALSE );
    }
    PifWriteFile( hsProgRptFile, &FileInfo, sizeof( PROGRPT_FHEAD ));

    ulProgRptFileSize = FileInfo.ulFileSize;

    /* --- application may call ProgRpt_ReadFile/ProgRpt_WriteFile function
        because file header initialization is completed --- */

    /* --- initialize each record headers --- */

    /* EOD info */
    memset( auchTmpBuff, 0x00, sizeof( PROGRPT_EODPTD ));
    pEODPTD = ( PROGRPT_EODPTD * )auchTmpBuff;
    _tcsncpy( pEODPTD->auchMark, PROGRPT_MARK_EOD, PROGRPT_MAX_MARK_LEN );
    //memcpy( pEODPTD->auchMark, PROGRPT_MARK_EOD, PROGRPT_MAX_MARK_LEN );
    if ( ! ProgRpt_WriteFile( FileInfo.ulOffsetEOD, pEODPTD, sizeof( PROGRPT_EODPTD ))) {
        return ( FALSE );
    }

    /* PTD info */
    memset( auchTmpBuff, 0x00, sizeof( PROGRPT_EODPTD ));
    pEODPTD = ( PROGRPT_EODPTD * )auchTmpBuff;
    _tcsncpy( pEODPTD->auchMark, PROGRPT_MARK_PTD, PROGRPT_MAX_MARK_LEN );
    //memcpy( pEODPTD->auchMark, PROGRPT_MARK_PTD, PROGRPT_MAX_MARK_LEN );
    if ( ! ProgRpt_WriteFile( FileInfo.ulOffsetPTD, pEODPTD, sizeof( PROGRPT_EODPTD ))) {
        return ( FALSE );
    }

    /* 16 char mnemonic info */
    memset( auchTmpBuff, 0x00, sizeof( PROGRPT_16CHAR ));
    pChar16Info = ( PROGRPT_16CHAR * )auchTmpBuff;
    _tcsncpy( pChar16Info->auchMark, PROGRPT_MARK_16CHAR, PROGRPT_MAX_MARK_LEN );
    //memcpy( pChar16Info->auchMark, PROGRPT_MARK_16CHAR, PROGRPT_MAX_MARK_LEN );
    pChar16Info->usNoOfRecord = PROGRPT_MAX_16CHAR_ADR;
    if ( ! ProgRpt_WriteFile( FileInfo.ulOffset16Char, pChar16Info, sizeof( PROGRPT_16CHAR ))) {
        return ( FALSE );
    }

    /* 40 char mnemonic info */
    memset( auchTmpBuff, 0x00, sizeof( PROGRPT_40CHAR ));
    pChar40Info = ( PROGRPT_40CHAR * )auchTmpBuff;
    _tcsncpy( pChar40Info->auchMark, PROGRPT_MARK_40CHAR, PROGRPT_MAX_MARK_LEN );
    //memcpy( pChar40Info->auchMark, PROGRPT_MARK_40CHAR, PROGRPT_MAX_MARK_LEN );
    pChar40Info->usNoOfRecord = ( USHORT )PROGRPT_MAX_40CHAR_ADR;
    if ( ! ProgRpt_WriteFile( FileInfo.ulOffset40Char, pChar40Info, sizeof( PROGRPT_40CHAR ))) {
        return ( FALSE );
    }
    return ( TRUE );
}

/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_GetFileHeader( PROGRPT_FHEAD *pFHeader )
*
*   Input   :   Nothing
*   Output  :   FHEADER *pFHeader - points to buffer that retrieves the file header.
*   InOut   :   Nothing
*
**  Return  :   BOOL    TRUE    -   function is successful.
*                       FALSE   -   function is failed.
*
**  Description:
*    This function retrieves file header of PROGRPT file.
*==========================================================================
*/
BOOL ProgRpt_GetFileHeader( PROGRPT_FHEAD *pFHeader )
{
    ULONG  ulActualRead;   /* actual read size in bytes */

    ASSERT( PROGRPT_INVALID_HANDLE < hsProgRptFile );
    ASSERT( pFHeader != ( PROGRPT_FHEAD * )NULL );

    ProgRpt_ReadFile( PROGRPT_OFFSET_FILEHEADER, pFHeader, sizeof( PROGRPT_FHEAD ), &ulActualRead );
    if ( ulActualRead != sizeof( PROGRPT_FHEAD )) {
        return ( FALSE );
    }
    return ( TRUE );
}

/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_ReadFile( ULONG  ulReadPosition,
*                                      VOID   *pvBuffer,
*                                      USHORT usNoOfBytesToRead,
*                                      ULONG *pulActualRead )
*
*   Input   :   ULONG  ulReadPosition    - offset to read a data
*               USHORT usNoOfBytesToRead - maximum no. of bytes to read
*   Output  :   VOID   *pvBuffer         - point to buffer that receives data
*               USHORT *pusActualRead    - point to actual read data size
*   InOut   :   Nothing
*
**  Return  :   BOOL    TRUE    -   function is successful.
*                       FALSE   -   function is failed.
*               
**  Description:
*    This function reads a data from PROGRPT file at specified position.
*   And then it returns to actual read size (in bytes), if function is
*   successful.
*==========================================================================
*/
BOOL ProgRpt_ReadFile( ULONG  ulReadPosition,
                       VOID   *pvBuffer,
                       USHORT  usNoOfBytesToRead,
                       ULONG *pulActualRead )
{
    ULONG  ulActualSeek;   /* actual seek size in bytes */
	ULONG  ulActualBytesRead;

    ASSERT( PROGRPT_INVALID_HANDLE < hsProgRptFile );
    ASSERT( ulReadPosition < ulProgRptFileSize );
    ASSERT( pvBuffer != ( VOID * )NULL );
    ASSERT( 0 < usNoOfBytesToRead );

    PifSeekFile( hsProgRptFile, ulReadPosition, &ulActualSeek );
    if ( ulReadPosition != ulActualSeek ) {
        return ( FALSE );
    }

	//11-10-03 JHHJ Change made in PifReadFile, see piffile.c for new usage
    PifReadFile( hsProgRptFile, pvBuffer, usNoOfBytesToRead , &ulActualBytesRead);

    if ( &ulActualBytesRead != NULL ) {
        *pulActualRead = ulActualBytesRead;
    }
    return ( TRUE );
}

/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_WriteFile( ULONG  ulWritePosition,
*                                       VOID   *pvBuffer,
*                                       USHORT usNoOfBytesToWrite )
*
*   Input   :   ULONG  ulReadPosition     - offset to write a data
*               USHORT usNoOfBytesToWrite - maximum no. of bytes to write
*   Output  :   VOID   *pvBuffer          - point to buffer to be written
*   InOut   :   Nothing
*
**  Return  :   BOOL    TRUE    -   function is successful.
*                       FALSE   -   function is failed.
*               
**  Description:
*    This function writes a data to PROGRPT file at specified position.
*==========================================================================
*/
BOOL ProgRpt_WriteFile( ULONG  ulWritePosition,
                        VOID   *pvBuffer,
                        ULONG  ulNoOfBytesToWrite )
{
    ULONG   ulActualSeek;   /* actual seek size in bytes */

    ASSERT( PROGRPT_INVALID_HANDLE < hsProgRptFile );
    ASSERT( ulWritePosition < ulProgRptFileSize );
    ASSERT( pvBuffer != ( VOID * )NULL );
    ASSERT( 0 < ulNoOfBytesToWrite );

    PifSeekFile( hsProgRptFile, ulWritePosition, &ulActualSeek );
    if ( ulWritePosition != ulActualSeek ) {
        return ( FALSE );
    }

    PifWriteFile( hsProgRptFile, pvBuffer, ulNoOfBytesToWrite );

    return ( TRUE );
}

/* ===== End of File (_PROGFIL.C) ===== */
