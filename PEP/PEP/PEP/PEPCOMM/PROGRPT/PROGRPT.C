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
*   Program Name       : PROGRPT.C
*  ------------------------------------------------------------------------  
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.
*   Memory Model       : Midium Model
*   Options            : /AM /W4 /G1s /Za /Zp /Os /Zi
*  ------------------------------------------------------------------------
*   Abstract           : The provided function names are as follows.
*
*       --- for file creation/deletion
*   ProgRptInitialize()     - Initialize Programmable Report Module.
*   ProgRptCreate()         - Create Programmable Report File.
*
*  ------------------------------------------------------------------------
*    Update Histories  :
*                                 
*    Date      Ver.Rev :NAME       :Description
*    Jan-26-96:00.00.01:T.Nakahata :Initial
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

#include    <string.h>
#include    <ecr.h>
#include    <r20_pif.h>
#include    <log.h>
#include    <appllog.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csop.h>
#include    <cscas.h>
#include    <csprgrpt.h>
#include    <progrept.h>
#include    "_progrpt.h"
#include    "mypifdefs.h"

/*
===========================================================================
    GLOBAL VARIABLES DECLARATIONs
===========================================================================
*/
    /* --- handle of semaphore --- */
static USHORT  husProgRptSem;

    /* --- buffer for print data --- */
static UCHAR   auchProgRptFile[ FLEX_PROGRPT_MAX * PROGRPT_BYTE_PER_BLOCK ];

/*
===========================================================================
    USER I/F FUNCTIONs
===========================================================================
*/
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
    ASSERT( husProgRptSem == PROGRPT_INVALID_HANDLE );

    husProgRptSem = PifCreateSem( PROGRPT_SEM_INIT_CO );

    ProgRpt_InitFile();
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
    SHORT   sReturn;  
    SHORT   sExpandState;   /* return status for file expand */
    PROGRPT_FHEAD *pFileInfo; /* information about prog report file */
    ULONG   ulActualSeek;   /* actual seek size of PifSeek() function */
    USHORT  usActualRead;   /* actual read size of PifRead() function */

    ASSERT( uchNoOfBlock <= FLEX_PROGRPT_MAX );

    PifRequestSem( husProgRptSem );

    if (!uchNoOfBlock) {         /* uchNoOfBlock == 0 */
        ProgRpt_Delete();
        PifReleaseSem( husProgRptSem );
        return(PROGRPT_SUCCESS);
    }

    sRetCode = PROGRPT_SUCCESS;

    if ( ProgRpt_NewCreate() != PROGRPT_SUCCESS ) {
        if ((hsProgRptFile = PifOpenFile(auchPROG_RPT, auchOLD_FILE_READ)) < 0) {
            PifAbort(MODULE_PROGRPT, FAULT_ERROR_FILE_OPEN);
        }
        /* Size 0 Check */
        if (PifSeekFile(hsProgRptFile, PROGRPT_OFFSET_FILEHEADER,  &ulActualSeek) == PIF_ERROR_FILE_EOF) {
            ProgRpt_Close();
            if ((hsProgRptFile = PifOpenFile(auchPROG_RPT, auchOLD_FILE_WRITE)) < 0) {
                PifAbort(MODULE_PROGRPT, FAULT_ERROR_FILE_OPEN);
            }                    
            /* Clear Contents of File */
            memset(auchProgRptFile, 0x00, (uchNoOfBlock * PROGRPT_BYTE_PER_BLOCK));
            ProgRpt_InitFileHeader( uchNoOfBlock );
        } else {
            ProgRpt_Close();
            if ((sReturn = ProgRpt_OldOpen()) == PROGRPT_FILE_NOT_FOUND) {
                PifAbort(MODULE_PROGRPT, FAULT_ERROR_FILE_OPEN);
            }
            if (sReturn != PROGRPT_SUCCESS) {
                ProgRpt_Close();
                ProgRpt_Delete();
                if ( ProgRpt_NewCreate() != PROGRPT_SUCCESS ) {
                    PifAbort(MODULE_PROGRPT, FAULT_ERROR_FILE_OPEN);
                }
                /* Clear Contents of File */
                memset(auchProgRptFile, 0x00, (uchNoOfBlock * PROGRPT_BYTE_PER_BLOCK));
                ProgRpt_InitFileHeader( uchNoOfBlock );
            }
            if ((UCHAR)(ulProgRptFileSize/PROGRPT_BYTE_PER_BLOCK) > uchNoOfBlock) {
                ProgRpt_Close();
                ProgRpt_Delete();
                if ( ProgRpt_NewCreate() != PROGRPT_SUCCESS ) {
                    PifAbort(MODULE_PROGRPT, FAULT_ERROR_FILE_OPEN);
                }
                /* Clear Contents of File */
                memset(&auchProgRptFile, 0x00, (uchNoOfBlock * PROGRPT_BYTE_PER_BLOCK));
                ProgRpt_InitFileHeader( uchNoOfBlock );
            } else {
                /* Clear Contents of File */
                memset(auchProgRptFile, 0x00, (uchNoOfBlock * PROGRPT_BYTE_PER_BLOCK));
                PifSeekFile( hsProgRptFile, PROGRPT_OFFSET_FILEHEADER, &ulActualSeek );
                if ( ulProgRptFileSize >= 0x8000) {
                    usActualRead = PifReadFile( hsProgRptFile, auchProgRptFile, 0x7fff);
                    if ( usActualRead != 0x7fff) {
                        ProgRpt_Close();
                        PifReleaseSem( husProgRptSem );
                        return ( PROGRPT_NO_READ_SIZE );
                    }
                    usActualRead = PifReadFile( hsProgRptFile, &auchProgRptFile[0x7fff], (ulProgRptFileSize - 0x7fffUL));
                    if ( usActualRead != (ulProgRptFileSize - 0x7fffUL)) {
                        ProgRpt_Close();
                        PifReleaseSem( husProgRptSem );
                        return ( PROGRPT_NO_READ_SIZE );
                    }
                } else {
                    usActualRead = PifReadFile( hsProgRptFile, auchProgRptFile, ulProgRptFileSize);
                    if ( usActualRead != ulProgRptFileSize) {
                        ProgRpt_Close();
                        PifReleaseSem( husProgRptSem );
                        return ( PROGRPT_NO_READ_SIZE );
                    }
                }
                pFileInfo = (PROGRPT_FHEAD *)auchProgRptFile;
                pFileInfo->ulFileSize = PROGRPT_BYTE_PER_BLOCK * uchNoOfBlock;
                ulProgRptFileSize = pFileInfo->ulFileSize;
            }
        }
    } else {
        /* Clear Contents of File */
        memset(&auchProgRptFile, 0x00, (uchNoOfBlock * PROGRPT_BYTE_PER_BLOCK));
        ProgRpt_InitFileHeader( uchNoOfBlock );
    }

    /* --- expand PROGRPT file to specified size --- */

    sExpandState = ProgRpt_ExpandFile( uchNoOfBlock );
    ASSERT(( sExpandState == PROGRPT_SUCCESS ) || ( sExpandState == PROGRPT_DEVICE_FULL ));

    switch ( sExpandState ) {
    case PROGRPT_SUCCESS:

        /* --- initialize file header of PROGRPT file --- */

        PifSeekFile( hsProgRptFile, PROGRPT_OFFSET_FILEHEADER, &ulActualSeek );
        if ( uchNoOfBlock == FLEX_PROGRPT_MAX) {
            PifWriteFile( hsProgRptFile, auchProgRptFile, 0xFFFF);
            PifWriteFile( hsProgRptFile, &auchProgRptFile[0xFFFF], 1);
        } else {
            PifWriteFile( hsProgRptFile, auchProgRptFile, ulProgRptFileSize);
        }
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
    PifReleaseSem( husProgRptSem );
    return ( sRetCode );
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRptDelete()
*
*   Input   :   UCHAR uchNoOfBlock  - no. of blocks for file creation
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode      - PROGRPT_SUCCESS
*                                   
**  Description:
*    This function deletes PROGRPT file.
*==========================================================================
*/
SHORT ProgRptDelete()
{

    if (hsProgRptFile) {    /* File exist */

        PifCloseFile(hsProgRptFile);
        PifDeleteFile((WCHAR FAR *)auchPROG_RPT);

    }

    return(PROGRPT_SUCCESS);
    
}


/* ===== End of File (PROGRPT.C) ===== */
