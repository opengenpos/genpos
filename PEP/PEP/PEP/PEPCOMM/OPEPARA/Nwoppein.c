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
*   Title              : Client/Server OPERATIONAL PARAMATER module, Create File (PLU PARAMATER, DEPT PARAMATER)
*   Category           : Client/Server OPERATIONAL PARAMATER module, NCR2170 US HOSPITALITY APPLICATION.
*   Program Name       : NWOPPEIN.C
*  ------------------------------------------------------------------------  
*   Compile            : MS-C Ver. 7.00 A by Microsoft Corp.              
*   Memory Model       : Large Model
*   Options            : /c /Alfw /W4 /Zpi /G2s /Os
*  ------------------------------------------------------------------------
*   Abstract           :
*
*       USER I/F FUNCITONS
*
*       OpPluCreatFile              * Create Plu File.
*       OpDeptCreatFile             * Create Dept File.
*       OpCpnCreatFile              * Create Combination Coupon File.
*       OpCstrCreatFile             * Create Control String File.
*       OpPpiCreatFile              * Create PPI File.
*       OpMldCreatFile              * Create MLD File.(V 3.3)
*       OpPluMigrateFile            * Migrate Plu File.
*       OpPluMigrate25File          * Migrate Plu File(Ver 2.5).
*       OpPluMigrate30File          * Migrate Plu File(Ver 3.0).
*
*       OpCpnMigrate30File          * Migrate Coupon File(Ver 3.0).
*
*       INTERNAL FUNCTIONS
*
*       Op_NewPluCreatFile          * Create Plu File newly. 
*       Op_ExpandPluFile            * Expand Plu File.
*       Op_CompressPluFile          * Compress Plu File.
*
*       Op_NewCstrCreatFile         * Create Control String File newly. 
*       Op_ExpandCstrFile           * Expand Control String File.
*       Op_CompressCstrFile         * Compress Control String File.
*
*       Op_CreatPluFileHeader       * Create Plu File Header.
*       Op_CompressParaTable        * Compress Paramater Table.
*       Op_MoveTableExpand          * Expand Table.
*       Op_MoveTableCompress        * Compress Table.
*       Op_CopyFile                 * Copy OLD (Ver 1.0, 2.0) --> NEW (Ver 3.1)
*       Op_Copy25File               * Copy OLD (Ver 2.5)      --> NEW (Ver 3.1)
*       Op_Copy30File               * Copy OLD (Ver 3.0)      --> NEW (Ver 3.1)
*       Op_WriteNewPlu              * Write Plu File 
*       Op_CreateEmptyTbl           * Create Empty table 
*       Op_CopyFileBlock            * Write Block Data
*
*  ------------------------------------------------------------------------
*    Update Histories  :
*   Date          : Ver.Rev  :NAME       :Description
*   Nov-08-93     : for pep  :H.YAMAGUCHI:Initial
*   Jan-20-94     : HRUS R.1 :M.YAMAMOTO :Changed OpPluMigradeFile
*   Apr-12-95     : HRUS R3.0:H.Ishida   :
*   Jan-31-96     : R3.1     :M.Suzuki   :
*   Sep-09-98     : 03.03.00 :A.Mitsui   :V3.3
*   Nov-30-99     :          :K.Yanagida :NCR2172
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
#include    <rfl.h>
#include    "mypifdefs.h"
#include    "paraequ.h"
#include    "para.h"
#include    "csop.h"
#include    "appllog.h"
#include    "csopin.h"
#include    "csoppein.h"

static UCHAR auchPpiFile[sizeof(OPPPI_FILEHED) + 
                         sizeof(OPPPI_PARAENTRY) * OP_PPI_NUMBER_OF_MAX]; 

static UCHAR auchCpnFile[sizeof(OPCPN_FILEHED) + 
                       sizeof(COMCOUPONPARA) * OP_CPN_NUMBER_OF_MAX];   /* saratoga */
/*
*==========================================================================
**    Synopsis:   VOID    OpInit(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :    Nothing
*
**  Description: Initialize for Operational Paramater Module.

  This code copied from NHPOS source code as part of 32 bit conversion hack.
  Semaphore was being used in the File->New menu chain and it had not been
  initialized before using.  Added a call to this function from pep.c when
  the main window is being created and initialized.
  Richard Chambers
*==========================================================================
*/
VOID    OpInit(VOID)
{
    /* Create Semaphore */
    husOpSem = PifCreateSem( OP_COUNTER_OF_SEM );
//    PluInitialize();  //RJC removed as does not appear to be needed.
}

																		/*
*==========================================================================
**    Synopsis:    SHORT   OpCpnCreatFile(USHORT usNumberOfCpn,
*                                                  USHORT usLockHnd)
*
*       Input: usNumberOfCpn - Number of Combination coupon
*              usLockHnd
*      Output: Nothing
*       InOut: Nothing
*
**  Return    :
*     Normal End: OP_PERFORM
*   Abnormal End: OP_NO_MAKE_FILE
*
**  Description: Create Combination Coupon File.
*==========================================================================
*/
SHORT   OpCpnCreatFile(USHORT usNumberOfCpn, USHORT usLockHnd)
{
    OPCPN_FILEHED *pCpnFileHead;
    ULONG ulActualPosition;
    /* move to static area, saratoga */
    /* UCHAR auchCpnFile[sizeof(OPCPN_FILEHED) + 
                       sizeof(COMCOUPONPARA) * OP_CPN_NUMBER_OF_MAX]; */

    PifRequestSem(husOpSem);
    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return(OP_LOCK);
    }

    /* Check Number of Combination Coupn */
    if (!usNumberOfCpn) {         /* usNumberOfCpn == 0 */
        PifDeleteFile((WCHAR FAR *)auchOP_CPN);
        PifReleaseSem(husOpSem);
        return(OP_PERFORM);
    }

    /* Creat Combination Coupon File */
    if ((hsOpCpnFileHandle = PifOpenFile((WCHAR FAR *)auchOP_CPN, 
                (CHAR FAR *)auchNEW_FILE_WRITE)) == PIF_ERROR_FILE_EXIST) {
        if ((hsOpCpnFileHandle = PifOpenFile((WCHAR FAR *)auchOP_CPN, 
                                  (CHAR FAR *)auchOLD_FILE_READ)) < 0) {
            PifAbort(MODULE_OP_CPN, FAULT_ERROR_FILE_OPEN);
        }
        /* Size 0 Check */
        if (PifSeekFile(hsOpCpnFileHandle, 
                        sizeof(OPCPN_FILEHED), 
                        &ulActualPosition) == PIF_ERROR_FILE_EOF) {
            PifCloseFile(hsOpCpnFileHandle);
            if ((hsOpCpnFileHandle = PifOpenFile((WCHAR FAR *)auchOP_CPN, 
                                      (CHAR FAR *)auchOLD_FILE_WRITE)) < 0) {
                PifAbort(MODULE_OP_CPN, FAULT_ERROR_FILE_OPEN);
            }
            /* Clear Contents of File */
            memset(&auchCpnFile[sizeof(OPCPN_FILEHED)], 
                   0x00, usNumberOfCpn * sizeof(COMCOUPONPARA));
        } else {
            PifCloseFile(hsOpCpnFileHandle);
            if ((hsOpCpnFileHandle = PifOpenFile((WCHAR FAR *)auchOP_CPN, 
                    (CHAR FAR *)auchOLD_FILE_READ_WRITE)) < 0) {
                PifAbort(MODULE_OP_CPN, FAULT_ERROR_FILE_OPEN);
            }

            Op_ReadCpnFile(0L, auchCpnFile, sizeof(OPCPN_FILEHED));
            pCpnFileHead = (OPCPN_FILEHED *)auchCpnFile;

            if (pCpnFileHead->usNumberOfCpn > usNumberOfCpn) {
                Op_ReadCpnFile(sizeof(OPCPN_FILEHED),
                                       &auchCpnFile[sizeof(OPCPN_FILEHED)],
                                       (USHORT)(usNumberOfCpn * 
                                       sizeof(COMCOUPONPARA)));
                if (PifControlFile(hsOpCpnFileHandle, PIF_FILE_CHANGE_SIZE,
                                  (LONG)((usNumberOfCpn * sizeof(COMCOUPONPARA)) 
                                  + sizeof(OPCPN_FILEHED))) < 0) {
                    PifCloseFile(hsOpCpnFileHandle);
                    PifReleaseSem(husOpSem);
                    return OP_NO_MAKE_FILE;
                }
            } else {
                memset(&auchCpnFile[sizeof(OPCPN_FILEHED)], 
                       0x00, usNumberOfCpn * sizeof(COMCOUPONPARA));
                Op_ReadCpnFile(sizeof(OPCPN_FILEHED),
                                       &auchCpnFile[sizeof(OPCPN_FILEHED)],
                                       (USHORT)(pCpnFileHead->usNumberOfCpn * 
                                       sizeof(COMCOUPONPARA)));
            }
        }
    } else {

        /* Clear Contents of File */
    
        memset(&auchCpnFile[sizeof(OPCPN_FILEHED)], 
                            0x00, usNumberOfCpn * sizeof(COMCOUPONPARA));
    }

    /* Check file size and creat file */

    if (PifSeekFile(hsOpCpnFileHandle,
                    (ULONG)(sizeof(OPCPN_FILEHED) + 
                    (usNumberOfCpn * sizeof(COMCOUPONPARA))),
                    &ulActualPosition) < 0) {  /* PIF_ERROR_DISK_FULL */
        PifCloseFile(hsOpCpnFileHandle);
        PifReleaseSem(husOpSem);
        return(OP_NO_MAKE_FILE);
    }

    /* Make Header File */
    pCpnFileHead = (OPCPN_FILEHED *)auchCpnFile;
    pCpnFileHead->usNumberOfCpn = usNumberOfCpn;

    /* Write File */
    Op_WriteCpnFile(OP_FILE_HEAD_POSITION, auchCpnFile,
                     (USHORT)(sizeof(OPCPN_FILEHED) + 
                     (sizeof(COMCOUPONPARA) * usNumberOfCpn))); /* if error, let system error */
    Op_CloseCpnFileReleaseSem();
    return(OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpCstrCreatFile(USHORT usNumberOfCstr,
*                                                   USHORT usLockHnd)
*
*       Input: usNumberOfCstr - Number of Control String
*              usLockHnd
*      Output: Nothing
*       InOut: Nothing
*
**  Return    :
*     Normal End: OP_PERFORM
*   Abnormal End: OP_NO_MAKE_FILE
*
**  Description: Create Control String File.
*==========================================================================
*/
SHORT   OpCstrCreatFile(USHORT usNumberOfCstr, USHORT usLockHnd)
{
    OPCSTR_FILEHED CstrFileHed;
    SHORT sStatus;
    ULONG ulActualPosition;

    PifRequestSem(husOpSem);

    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return(OP_LOCK);
    }

    if (!usNumberOfCstr) {           /* usNumberOfCstr == 0 */
        PifDeleteFile((WCHAR FAR *)auchOP_CSTR);
        PifReleaseSem(husOpSem);
        return(OP_PERFORM);
    }
    if ((hsOpCstrFileHandle = PifOpenFile((WCHAR FAR *)auchOP_CSTR, 
                (CHAR FAR *)auchNEW_FILE_WRITE)) == PIF_ERROR_FILE_EXIST) {
        if ((hsOpCstrFileHandle = PifOpenFile((WCHAR FAR *)auchOP_CSTR, 
                (CHAR FAR *)auchOLD_FILE_READ)) < 0) {
            PifAbort(MODULE_OP_CSTR, FAULT_ERROR_FILE_OPEN);
        }
        /* Size 0 Check */
        if (PifSeekFile(hsOpCstrFileHandle, sizeof(OPCSTR_FILEHED), 
                        &ulActualPosition) == PIF_ERROR_FILE_EOF) {
            PifCloseFile(hsOpCstrFileHandle);
            if ((hsOpCstrFileHandle = PifOpenFile((WCHAR FAR *)auchOP_CSTR, 
                    (CHAR FAR *)auchOLD_FILE_WRITE)) < 0) {
                PifAbort(MODULE_OP_CSTR, FAULT_ERROR_FILE_OPEN);
            }
            sStatus = Op_NewCstrCreatFile(usNumberOfCstr);
        } else {
            PifCloseFile(hsOpCstrFileHandle);
            if ((hsOpCstrFileHandle = PifOpenFile((WCHAR FAR *)auchOP_CSTR,
                    (CHAR FAR *)auchOLD_FILE_READ_WRITE)) < 0) {
                PifAbort(MODULE_OP_CSTR, FAULT_ERROR_FILE_OPEN);
            }
            Op_ReadCstrFile(OP_FILE_HEAD_POSITION, 
                           &CstrFileHed, sizeof(CstrFileHed));
            if (CstrFileHed.usNumberOfCstr > usNumberOfCstr) {
                sStatus = Op_CompressCstrFile(usNumberOfCstr);
            } else if (CstrFileHed.usNumberOfCstr < usNumberOfCstr) {
                sStatus = Op_ExpandCstrFile(usNumberOfCstr);
            } else {
                sStatus = PifControlFile(hsOpCstrFileHandle, 
                                         PIF_FILE_CHANGE_SIZE, 
                                         (ULONG)(sizeof(OPCSTR_FILEHED) +
                                           (sizeof(OPCSTR_INDEXENTRY) * OP_CSTR_INDEX_SIZE) +
                                           (sizeof(OPCSTR_PARAENTRY)  * usNumberOfCstr)));
                if (sStatus == PIF_OK) {
                    sStatus = OP_PERFORM;
                }
            }
        }
    } else {                         
        sStatus = Op_NewCstrCreatFile(usNumberOfCstr);
    }
    Op_CloseCstrFileReleaseSem();
    return(sStatus);
}

/******************** New Functions (Release 3.1) BEGIN *******************/
/*
*==========================================================================
**    Synopsis:    SHORT   OpPpiCreatFile(USHORT usNumberOfPpi, 
*                                                  USHORT usLockHnd)
*
*       Input:    usNumberOfPpi - Number of Promotional Pricing Item
*                 usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End:  OP_PERFORM
*   Abnormal End:  OP_NO_MAKE_FILE
*
**  Description: Create PPI (Promotional Pricing Item) File.
*==========================================================================
*/
SHORT   OpPpiCreatFile(USHORT usNumberOfPpi, USHORT usLockHnd)
{
    OPPPI_FILEHED   *pPpiFileHead;
    ULONG           ulActualPosition;
            
    PifRequestSem(husOpSem);

    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return(OP_LOCK);
    }

    /* Check Number of PPI */

    if (!usNumberOfPpi) {
        PifDeleteFile((WCHAR FAR *)auchOP_PPI);
        PifReleaseSem(husOpSem);
        return(OP_PERFORM);
    }

    if ((hsOpPpiFileHandle = PifOpenFile((WCHAR FAR *)auchOP_PPI, 
                (CHAR FAR *)auchNEW_FILE_WRITE)) == PIF_ERROR_FILE_EXIST) {
        if ((hsOpPpiFileHandle = PifOpenFile((WCHAR FAR *)auchOP_PPI, 
                                  (CHAR FAR *)auchOLD_FILE_READ)) < 0) {
            PifAbort(MODULE_OP_PPI, FAULT_ERROR_FILE_OPEN);
        }
        /* Size 0 Check */
        if (PifSeekFile(hsOpPpiFileHandle, 
                        sizeof(OPPPI_FILEHED), 
                        &ulActualPosition) == PIF_ERROR_FILE_EOF) {
            PifCloseFile(hsOpPpiFileHandle);
            if ((hsOpPpiFileHandle = PifOpenFile((WCHAR FAR *)auchOP_PPI, 
                                      (CHAR FAR *)auchOLD_FILE_WRITE)) < 0) {
                PifAbort(MODULE_OP_PPI, FAULT_ERROR_FILE_OPEN);
            }
            /* Clear Contents of File */
            memset(&auchPpiFile[sizeof(OPPPI_FILEHED)], 
                   0x00, usNumberOfPpi * sizeof(OPPPI_PARAENTRY));
        } else {
            PifCloseFile(hsOpPpiFileHandle);
            if ((hsOpPpiFileHandle = PifOpenFile((WCHAR FAR *)auchOP_PPI, 
                    (CHAR FAR *)auchOLD_FILE_READ_WRITE)) < 0) {
                PifAbort(MODULE_OP_PPI, FAULT_ERROR_FILE_OPEN);
            }

            Op_ReadPpiFile(0L, auchPpiFile, sizeof(OPPPI_FILEHED));
            pPpiFileHead = (OPPPI_FILEHED *)auchPpiFile;

            if (pPpiFileHead->usNumberOfPpi > usNumberOfPpi) {
                Op_ReadPpiFile(sizeof(OPPPI_FILEHED),
                                       &auchPpiFile[sizeof(OPPPI_FILEHED)],
                                       (USHORT)(usNumberOfPpi * 
                                       sizeof(OPPPI_PARAENTRY)));
                if (PifControlFile(hsOpPpiFileHandle, PIF_FILE_CHANGE_SIZE,
                                  (LONG)((usNumberOfPpi * sizeof(OPPPI_PARAENTRY)) 
                                  + sizeof(OPPPI_FILEHED))) < 0) {
                    PifCloseFile(hsOpPpiFileHandle);
                    PifReleaseSem(husOpSem);
                    return OP_NO_MAKE_FILE;
                }
            } else {
                memset(&auchPpiFile[sizeof(OPPPI_FILEHED)], 
                       0x00, usNumberOfPpi * sizeof(OPPPI_PARAENTRY));
                Op_ReadPpiFile(sizeof(OPPPI_FILEHED),
                                       &auchPpiFile[sizeof(OPPPI_FILEHED)],
                                       (USHORT)(pPpiFileHead->usNumberOfPpi * 
                                       sizeof(OPPPI_PARAENTRY)));
            }
        }
    } else {
        /* Clear Contents of File */
    
        memset(&auchPpiFile[sizeof(OPPPI_FILEHED)], 
                            0x00, usNumberOfPpi * sizeof(OPPPI_PARAENTRY));
    }

    /* Check file size and creat file */

    if (PifSeekFile(hsOpPpiFileHandle,
                    (ULONG)( sizeof( OPPPI_FILEHED ) + 
                    usNumberOfPpi * sizeof( OPPPI_PARAENTRY )),
                    &ulActualPosition) < 0) {   /* PIF_ERROR_DISK_FULL */
        PifCloseFile(hsOpPpiFileHandle);
        PifDeleteFile((WCHAR FAR *)auchOP_PPI);              /* Can't make file then delete file */
        PifReleaseSem(husOpSem);
        return(OP_NO_MAKE_FILE);
    }

    /* Make Header File */

    pPpiFileHead = ( OPPPI_FILEHED * )auchPpiFile;
    pPpiFileHead->usNumberOfPpi = usNumberOfPpi;

    /* Write File */

    Op_WritePpiFile(OP_FILE_HEAD_POSITION, auchPpiFile,
                     (USHORT)(sizeof(OPPPI_FILEHED) + 
                     sizeof(OPPPI_PARAENTRY) * usNumberOfPpi)); /* if error, let system error */
    Op_ClosePpiFileReleaseSem();
    return(OP_PERFORM);
}
/******************** New Functions (Release 3.1) END *******************/

/******************** New Functions (V3.3) BEGIN ************************/
/*
*==========================================================================
**    Synopsis:    SHORT   OpMldCreatFile(ULONG ulNumberOfAddress, 
*                                                  USHORT usLockHnd)
*
*       Input:    usNumberOfAddress
*                 usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End:  OP_PERFORM
*   Abnormal End:  OP_NO_MAKE_FILE
*
**  Description: Create Multi-line Display Mnemonics File. V3.3
*==========================================================================
*/
SHORT   OpMldCreatFile(ULONG ulNumberOfAddress, USHORT usLockHnd)
{
    OPMLD_FILEHED   MldFileHed;
    ULONG           ulActualPosition;
    ULONG           ulNewSize;
    ULONG           ulWriteOffset;
    WCHAR           auchMldFile[ sizeof( MLD_RECORD ) ];

    PifRequestSem(husOpSem);

    /* LockCheck */

    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return(OP_LOCK);
    }

    /* Delete MLD File */

    PifDeleteFile((WCHAR FAR *)auchOP_MLD);

    /* Check Number of MLD */

    if (!ulNumberOfAddress) {
        PifReleaseSem(husOpSem);
        return(OP_PERFORM);
    }

    if ((hsOpMldFileHandle = PifOpenFile((WCHAR FAR *)auchOP_MLD,
                                         (CHAR FAR *)auchNEW_FILE_WRITE)) < 0) {

        Op_MldAbort(FAULT_ERROR_FILE_OPEN);
    }

    /* Check file size and creat file */

    ulNewSize = ( ULONG )( sizeof( OPMLD_FILEHED ) +
                         ( sizeof( MLD_RECORD ) * ulNumberOfAddress ));
    if (PifSeekFile(hsOpMldFileHandle,
                    ulNewSize,
                    &ulActualPosition) < 0) {   /* PIF_ERROR_DISK_FULL */
        PifCloseFile(hsOpMldFileHandle);
        PifDeleteFile((WCHAR FAR*)auchOP_MLD);              /* Can't make file then delete file */
        PifReleaseSem(husOpSem);
        return(OP_NO_MAKE_FILE);
    }

    /* Make Header File */

    memset( &MldFileHed, 0x00, sizeof( OPMLD_FILEHED ));
    MldFileHed.usNumberOfAddress = (USHORT)ulNumberOfAddress;

    ulWriteOffset = OP_FILE_HEAD_POSITION;
    Op_WriteMldFile( ulWriteOffset, &MldFileHed, sizeof( OPMLD_FILEHED ));
    ulWriteOffset += ( ULONG )sizeof( OPMLD_FILEHED );

    /* Clear Contents of File */

    memset( auchMldFile, 0x31, sizeof( auchMldFile ));
    
    while( ulWriteOffset < ulNewSize ) {

        Op_WriteMldFile( ulWriteOffset,
                         auchMldFile,
                         sizeof( auchMldFile ));
        ulWriteOffset += ( ULONG )sizeof( auchMldFile );
    }

    Op_CloseMldFileReleaseSem();
    return(OP_PERFORM);
}


/******************** New Functions (V3.3) END   ************************/

/*
*==========================================================================
**   Synopsis:    SHORT   Op_NewCstrCreatFile(USHORT usNumberOfCstr, 
*                                             USHORT usLockHnd)
*
*       Input: usNumberOfCstr - Number of Control String
*      Output: Nothing
*       InOut: Nothing
*
**  Return    :
*     Normal End:  OP_PERFORM
*   Abnormal End:  OP_NO_MAKE_FILE
*
**  Description: Create Control String File.
*==========================================================================
*/
SHORT   Op_NewCstrCreatFile(USHORT usNumberOfCstr)
{

    OPCSTR_FILEHED CstrFileHed;
    ULONG  ulActualPosition;
    OPCSTR_INDEXENTRY CstrIndex[OP_CSTR_INDEX_SIZE];

    /* Make File Header */

    CstrFileHed.usNumberOfCstr = usNumberOfCstr;
    CstrFileHed.usOffsetOfTransCstr = 0;

    /* Check file size and creat file */

    if (PifSeekFile(hsOpCstrFileHandle, 
                   (ULONG)(sizeof(OPCSTR_FILEHED) +
                          (sizeof(OPCSTR_INDEXENTRY) * OP_CSTR_INDEX_SIZE) +
                          (sizeof(OPCSTR_PARAENTRY)  * usNumberOfCstr)),
                    &ulActualPosition) < 0) { /* PIF_ERROR_DISK_FULL */
        PifCloseFile(hsOpCstrFileHandle);
        PifDeleteFile((WCHAR FAR *)auchOP_CSTR);          /* Can't make file then delete file */
        return(OP_NO_MAKE_FILE);
    }

    /* Write File Header */

    Op_WriteCstrFile(OP_FILE_HEAD_POSITION, &CstrFileHed, sizeof(CstrFileHed));

    /* Clear and Write Empty table */

    memset(CstrIndex, 0x00, (sizeof(OPCSTR_INDEXENTRY) * OP_CSTR_INDEX_SIZE));
    Op_WriteCstrFile((ULONG)(sizeof(CstrFileHed)), &CstrIndex,
                             (SHORT)(sizeof(OPCSTR_INDEXENTRY) * OP_CSTR_INDEX_SIZE));
    return OP_PERFORM;
}
/*
*==========================================================================
**   Synopsis:    SHORT   Op_ExpandCstrFile(USHORT usNumberOfCstr)
*
*       Input: usNumberOfCstr - Number of Control String
*      Output: Nothing
*       InOut: Nothing
*
**  Return   :
*     Normal End: OP_PERFORM
*   Abnormal End: OP_NO_MAKE_FILE
*
**  Description: Create Control String File Expanded.
*==========================================================================
*/
SHORT   Op_ExpandCstrFile(USHORT usNumberOfCstr)
{
    OPCSTR_FILEHED CstrFileHed;
    ULONG  ulActualPosition;

    /* Check file size and creat file */

    if (PifSeekFile(hsOpCstrFileHandle, 
                   (ULONG)(sizeof(OPCSTR_FILEHED) +
                          (sizeof(OPCSTR_INDEXENTRY) * OP_CSTR_INDEX_SIZE) +
                          (sizeof(OPCSTR_PARAENTRY)  * usNumberOfCstr)),
                    &ulActualPosition) < 0) { /* PIF_ERROR_DISK_FULL */
        PifCloseFile(hsOpCstrFileHandle);
        PifDeleteFile((WCHAR FAR *)auchOP_CSTR);          /* Can't make file then delete file */
        return(OP_NO_MAKE_FILE);
    }

    /* Read File Header */

    Op_ReadCstrFile(OP_FILE_HEAD_POSITION, &CstrFileHed, sizeof(CstrFileHed));

    /* Make File Header */

    CstrFileHed.usNumberOfCstr = usNumberOfCstr;

    /* Write File Header */

    Op_WriteCstrFile(OP_FILE_HEAD_POSITION, &CstrFileHed, sizeof(CstrFileHed));
   
    return (OP_PERFORM);
}
/*
*==========================================================================
**   Synopsis:    SHORT   Op_CompressCstrFile(USHORT usNewNumberOfCstr)
*
*       Input: usNewNumberOfCstr - Number of Control String (Creating File)
*      Output: Nothing
*       InOut: Nothing
*
**  Return   :
*     Normal End: OP_PERFORM
*   Abnormal End: OP_NO_MAKE_FILE
*                 OP_NO_COMPRESS_FILE
*
**  Description: Compress Control String File.
*==========================================================================
*/
SHORT Op_CompressCstrFile(USHORT usNewNumberOfCstr)
{
    OPCSTR_FILEHED CstrFileHed;
    SHORT sStatus;
    SHORT wID;
    OPCSTR_INDEXENTRY CstrIndex[OP_CSTR_INDEX_SIZE];

    /* Read File Header */

    Op_ReadCstrFile(OP_FILE_HEAD_POSITION, &CstrFileHed, sizeof(OPCSTR_FILEHED));

    /* Check Compress or not */

    if (CstrFileHed.usOffsetOfTransCstr > (usNewNumberOfCstr * sizeof(OPCSTR_PARAENTRY))) {
        return (OP_NO_COMPRESS_FILE);
    }

    /* Make File Header */

    CstrFileHed.usNumberOfCstr = usNewNumberOfCstr;
    CstrFileHed.usOffsetOfTransCstr = 0;


    /* Read Index Table */

    Op_ReadCstrFile((ULONG)sizeof(OPCSTR_FILEHED), &CstrIndex,
                           (SHORT)(sizeof(OPCSTR_INDEXENTRY) * OP_CSTR_INDEX_SIZE));

    /* Make Index Table */

    for (wID = 0; wID < OP_CSTR_INDEX_SIZE; wID++ ){

        if (CstrIndex[wID].usCstrOffset < (usNewNumberOfCstr * sizeof(OPCSTR_PARAENTRY))) {

            if ((CstrIndex[wID].usCstrOffset + CstrIndex[wID].usCstrLength) >
                                                   CstrFileHed.usOffsetOfTransCstr) {
                CstrFileHed.usOffsetOfTransCstr =
                          (USHORT)(CstrIndex[wID].usCstrOffset + CstrIndex[wID].usCstrLength);

            }

        } else {

             CstrIndex[wID].usCstrOffset  = 0;
             CstrIndex[wID].usCstrLength  = 0;

        }
    }


    /* Write File Header */

    Op_WriteCstrFile(OP_FILE_HEAD_POSITION, &CstrFileHed, sizeof(OPCSTR_FILEHED));

    /* Write Index Table */

    Op_WriteCstrFile(sizeof(OPCSTR_FILEHED), &CstrIndex,
                           (sizeof(OPCSTR_INDEXENTRY) * OP_CSTR_INDEX_SIZE));

    if ((sStatus = PifControlFile(hsOpCstrFileHandle, 
                                  PIF_FILE_CHANGE_SIZE, 
                                  (ULONG)(sizeof(OPCSTR_FILEHED) +
                                         (sizeof(OPCSTR_INDEXENTRY) * OP_CSTR_INDEX_SIZE) +
                                         (sizeof(OPCSTR_PARAENTRY)  * usNewNumberOfCstr)))) < 0 ) {
        return (OP_NO_MAKE_FILE);
    }
    return (OP_PERFORM);
}


/*
*==========================================================================
**   Synopsis:    SHORT   Op_MoveTableExpand(ULONG offulSrcTable,
*                                            ULONG offulObjTable, 
*                                            ULONG ulMoveByte)
*       Input: offulSrcTable 
*              offulObjTable  
*      Output: Nothing
*       InOut: Nothing
*
**  Return   :
*     Normal End: OP_PERFORM
*   Abnormal End: OP_NO_MAKE_FILE
*
**  Description: Move a Table offusSrcTable to offusObjTable by usMoveByte.
*==========================================================================
*/
VOID   Op_MoveTableExpand(ULONG offulSrcTable, 
                          ULONG offulObjTable, 
                          ULONG ulMoveByte)
{
    UCHAR   auchWorkBuffer[OP_MAX_WORK_BUFFER];
    ULONG   offulTailSrcTable;
    ULONG   offulTailObjTable;

    offulTailSrcTable = offulSrcTable + ulMoveByte;
    offulTailObjTable = offulObjTable + ulMoveByte;

    while (ulMoveByte > OP_MAX_WORK_BUFFER) {
        offulTailSrcTable -= OP_MAX_WORK_BUFFER;
        offulTailObjTable -= OP_MAX_WORK_BUFFER;
        Op_ReadDeptFile(offulTailSrcTable, 
                       auchWorkBuffer, OP_MAX_WORK_BUFFER);
        Op_WriteDeptFile(offulTailObjTable, 
                        auchWorkBuffer, OP_MAX_WORK_BUFFER);
        ulMoveByte -= OP_MAX_WORK_BUFFER;
    }
    if (ulMoveByte != 0) {
        Op_ReadDeptFile(offulSrcTable, auchWorkBuffer, (USHORT)ulMoveByte);
        Op_WriteDeptFile(offulObjTable, auchWorkBuffer, (USHORT)ulMoveByte);
    }
}
/*
*==========================================================================
**   Synopsis:    SHORT   Op_MoveTableCompress(ULONG offulSrcTable, 
*                                              ULONG offulObjTable, 
*                                              ULONG ulMoveByte)
*
*       Input: offulSrcTable 
*              offulObjTable  
*      Output: Nothing
*       InOut: Nothing
*
**  Return    :
*     Normal End: OP_PERFORM
*   Abnormal End: OP_NO_MAKE_FILE
*
**  Description: Move a Table offusSrcTable to offusObjTable by usMoveByte.
*==========================================================================
*/
VOID   Op_MoveTableCompress(ULONG offulSrcTable, 
                            ULONG offulObjTable, 
                            ULONG ulMoveByte)
{
    UCHAR   auchWorkBuffer[OP_MAX_WORK_BUFFER];

    while (ulMoveByte > OP_MAX_WORK_BUFFER) {
        Op_ReadDeptFile(offulSrcTable, auchWorkBuffer, OP_MAX_WORK_BUFFER);
        Op_WriteDeptFile(offulObjTable, auchWorkBuffer, OP_MAX_WORK_BUFFER);
        ulMoveByte -= OP_MAX_WORK_BUFFER;
        offulSrcTable += OP_MAX_WORK_BUFFER;
        offulObjTable += OP_MAX_WORK_BUFFER;
    }
    if (ulMoveByte != 0) {
        Op_ReadDeptFile(offulSrcTable, auchWorkBuffer, (USHORT)ulMoveByte);
        Op_WriteDeptFile(offulObjTable, auchWorkBuffer,(USHORT)ulMoveByte);
    }
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpPluMigrateFile(VOID)
*
*       Input: Nothing
*      Output: Nothing
*       InOut: Nothing
*
**  Return    :
*     Normal End: OP_PERFORM
*   Abnormal End: OP_NO_MAKE_FILE
*
**  Description: Create New PLU (Ver 3.1) File from Old PLU (Ver 1.0, 2.0).
*==========================================================================
*/
SHORT   OpPluMigrateFile(VOID)
{
    OPPLU_FILEHED     PluFileHedNew;
    OPPLU_FILEHED_THO PluFileHedOld;
    SHORT sStatus = OP_PERFORM;

    PifRequestSem(husOpSem);

    if ((hsOpPluFileHandle = PifOpenFile((WCHAR FAR *)auchOP_PLU_OLD,
            (CHAR FAR *)auchOLD_FILE_READ_WRITE)) < 0) {
        return(OP_FILE_NOT_FOUND);
    }

    if ((PifReadFile(hsOpPluFileHandle, &PluFileHedOld,   /* changed */
                     sizeof(PluFileHedOld))) != sizeof(PluFileHedOld)) {
        return(OP_FILE_NOT_FOUND);
    }

/*    Op_CreatPluFileHeader(&PluFileHedNew, PluFileHedOld.usNumberOfMaxPlu);    */
    
    PluFileHedNew.usNumberOfResidentPlu = 
                            PluFileHedOld.usNumberOfResidentPlu ;

/*    sStatus = Op_CopyFile(&PluFileHedNew , &PluFileHedOld);   

    if ( sStatus != OP_PERFORM) {
        return(sStatus);
    }
*/

    Op_ClosePluFileReleaseSem();
/*    PifDeleteFile((CHAR FAR *)auchOP_PLU_OLD);             */
    return(sStatus);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpPluMigrate25File(VOID)
*
*       Input: Nothing
*      Output: Nothing
*       InOut: Nothing
*
**  Return    :
*     Normal End: OP_PERFORM
*   Abnormal End: OP_NO_MAKE_FILE
*
**  Description: Create New PLU (Ver 3.1) File from Old PLU (Ver 2.5).
*==========================================================================
*/
SHORT   OpPluMigrate25File(VOID)
{
    OPPLU_FILEHED     PluFileHedNew;
    OPPLU_FILEHED_R25 PluFileHedOld;
    SHORT sStatus = OP_PERFORM;

    PifRequestSem(husOpSem);

    if ((hsOpPluFileHandle = PifOpenFile((WCHAR FAR *)auchOP_PLU_OLD,
            (CHAR FAR *)auchOLD_FILE_READ_WRITE)) < 0) {
        return(OP_FILE_NOT_FOUND);
    }

    if ((PifReadFile(hsOpPluFileHandle, &PluFileHedOld,   /* changed */
                     sizeof(PluFileHedOld))) != sizeof(PluFileHedOld)) {
        return(OP_FILE_NOT_FOUND);
    }

/*    Op_CreatPluFileHeader(&PluFileHedNew, PluFileHedOld.usNumberOfMaxPlu);    */
    
    PluFileHedNew.usNumberOfResidentPlu = 
                            PluFileHedOld.usNumberOfResidentPlu ;

/*    sStatus = Op_Copy25File(&PluFileHedNew , &PluFileHedOld); 

    if ( sStatus != OP_PERFORM) {
        return(sStatus);
    }
*/

    Op_ClosePluFileReleaseSem();
/*    PifDeleteFile((CHAR FAR *)auchOP_PLU_OLD);             */
    return(sStatus);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpPluMigrate30File(VOID)
*
*       Input: Nothing
*      Output: Nothing
*       InOut: Nothing
*
**  Return    :
*     Normal End: OP_PERFORM
*   Abnormal End: OP_NO_MAKE_FILE
*
**  Description: Create New PLU (Ver 3.1) File from Old PLU (Ver 3.0).
*==========================================================================
*/
SHORT   OpPluMigrate30File(VOID)
{
    OPPLU_FILEHED     PluFileHedNew;
    OPPLU_FILEHED     PluFileHedOld;
    SHORT sStatus = OP_PERFORM;

    PifRequestSem(husOpSem);

    if ((hsOpPluFileHandle = PifOpenFile((WCHAR FAR *)auchOP_PLU_OLD,
            (CHAR FAR *)auchOLD_FILE_READ_WRITE)) < 0) {
        return(OP_FILE_NOT_FOUND);
    }

    if ((PifReadFile(hsOpPluFileHandle, &PluFileHedOld,   /* changed */
                     sizeof(PluFileHedOld))) != sizeof(PluFileHedOld)) {
        return(OP_FILE_NOT_FOUND);
    }

/*    Op_CreatPluFileHeader(&PluFileHedNew, PluFileHedOld.usNumberOfMaxPlu);    */
    
    PluFileHedNew.usNumberOfResidentPlu = 
                            PluFileHedOld.usNumberOfResidentPlu ;

/*    sStatus = Op_Copy30File(&PluFileHedNew , &PluFileHedOld); /

    if ( sStatus != OP_PERFORM) {
        return(sStatus);
    } */

    Op_ClosePluFileReleaseSem();
/*    PifDeleteFile((CHAR FAR *)auchOP_PLU_OLD);             */
    return(sStatus);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpPluDeleteFile()
*
*       Input: usNumberOfPlu - Number of Plu
*              usLockHnd
*      Output: Nothing
*       InOut: Nothing
*
**  Return    :
*     Normal End:  OP_PERFORM
*
**  Description: Delete Plu File.
*==========================================================================
*/
SHORT   OpPluDeleteFile()
{

    if (hsOpPluFileHandle) {    /* File exist */

        PifCloseFile(hsOpPluFileHandle);
        PifDeleteFile((WCHAR FAR *)auchOP_PLU);

    }

    return(OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpDeptDeleteFile()
*
*       Input: usNumberOfPlu - Number of Plu
*              usLockHnd
*      Output: Nothing
*       InOut: Nothing
*
**  Return    :
*     Normal End:  OP_PERFORM
*
**  Description: Delete Dept File.
*==========================================================================
*/
SHORT   OpDeptDeleteFile()
{

    if (hsOpDeptFileHandle) {   /* File exist */

        PifCloseFile(hsOpDeptFileHandle);
        PifDeleteFile((WCHAR FAR *)auchOP_DEPT);

    }

    return(OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpCpnDeleteFile()
*
*       Input: usNumberOfCpn - Number of Combination coupon
*              usLockHnd
*      Output: Nothing
*       InOut: Nothing
*
**  Return    :
*     Normal End: OP_PERFORM
*
**  Description: Delete Combination Coupon File.
*==========================================================================
*/
SHORT   OpCpnDeleteFile()
{

    if (hsOpCpnFileHandle) {    /* File exist */

        PifCloseFile(hsOpCpnFileHandle);
        PifDeleteFile((WCHAR FAR *)auchOP_CPN);

    }

    return(OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpCstrDeleteFile()
*
*       Input: usNumberOfCstr - Number of Control String
*              usLockHnd
*      Output: Nothing
*       InOut: Nothing
*
**  Return    :
*     Normal End: OP_PERFORM
*
**  Description: Delete Control String File.
*==========================================================================
*/
SHORT   OpCstrDeleteFile()
{

    if (hsOpCstrFileHandle) {   /* File exist */

        PifCloseFile(hsOpCstrFileHandle);
        PifDeleteFile((WCHAR FAR *)auchOP_CSTR);

    }

    return(OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpPpiDeleteFile()
*
*       Input:    usNumberOfPpi - Number of Promotional Pricing Item
*                 usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End:  OP_PERFORM
*
**  Description: Delete PPI (Promotional Pricing Item) File.
*==========================================================================
*/
SHORT   OpPpiDeleteFile()
{

    if (hsOpPpiFileHandle) {    /* File exist */

        PifCloseFile(hsOpPpiFileHandle);
        PifDeleteFile((WCHAR FAR *)auchOP_PPI);

    }

    return(OP_PERFORM);
    
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpMldDeleteFile()
*
*       Input:    usNumberOfAddress
*                 usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End:  OP_PERFORM
*
**  Description: Create Multi-line Display Mnemonics File. V3.3
*==========================================================================
*/
SHORT   OpMldDeleteFile()
{
    
    if (hsOpMldFileHandle) {    /* File exist */

        PifCloseFile(hsOpMldFileHandle);
        PifDeleteFile((WCHAR FAR *)auchOP_MLD);

    }

    return(OP_PERFORM);
    
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpCpnMigrate30File(VOID)
*
*       Input: Nothing
*      Output: Nothing
*       InOut: Nothing
*
**  Return    :
*     Normal End: OP_PERFORM
*   Abnormal End: OP_NO_MAKE_FILE
*
**  Description: Create New Coupon (Ver 3.1) File from Old Coupon (Ver 3.0).
*==========================================================================
*/
SHORT   OpCpnMigrate30File(VOID)
{
    OPCPN_FILEHED CpnFileHed;
    USHORT  i;
    CPNIF   Cpni;
    UCHAR   uchStatus;

    /*  CHECK LOCK */

    PifRequestSem(husOpSem);

    /*  OPEN FILE  */

    if ((hsOpCpnFileHandle = PifOpenFile((WCHAR FAR *)auchOP_CPN,
                                          (CHAR FAR *)auchOLD_FILE_READ_WRITE)) == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        PifReleaseSem(husOpSem);
        return(OP_FILE_NOT_FOUND);
    }

    if ((PifReadFile(hsOpCpnFileHandle, &CpnFileHed,
                     sizeof(OPCPN_FILEHED))) != sizeof(OPCPN_FILEHED)) {
        Op_CloseCpnFileReleaseSem();           /* close file and release semaphore */
        return(OP_FILE_NOT_FOUND);
    }

    for (i = 0; i  < CpnFileHed.usNumberOfCpn; i++ ) {

        Op_ReadCpnFile((ULONG)(sizeof(CpnFileHed) + (i * sizeof(COMCOUPONPARA))), 
                        &Cpni.ParaCpn, 
                        sizeof(COMCOUPONPARA));

        uchStatus = (UCHAR)(Cpni.ParaCpn.uchCouponStatus[0] & 0x27);

        if (Cpni.ParaCpn.uchCouponStatus[0] & 0x08) {
            uchStatus |= 0x40;
        }

        if (Cpni.ParaCpn.uchCouponStatus[0] & 0x10) {
            uchStatus |= 0x80;
        }

        Cpni.ParaCpn.uchCouponStatus[0] = uchStatus;

        Op_WriteCpnFile( sizeof(CpnFileHed) + (ULONG)(i * sizeof(COMCOUPONPARA)),
                          &Cpni.ParaCpn, 
                          sizeof(Cpni.ParaCpn));
    }

    Op_CloseCpnFileReleaseSem();           /* close file and release semaphore */
    return(OP_PERFORM);
}

/*** NCR2172 Start ***/
/*
*==========================================================================
**    Synopsis:    SHORT   OpDeptCreatFile(USHORT usNumberOfDept,
*                                                   USHORT usLockHnd)
*
*       Input: usNumberOfDept - Number of Dept
*              usLockHnd
*      Output: Nothing
*       InOut: Nothing
*
**  Return    :
*     Normal End: OP_PERFORM
*   Abnormal End: OP_NO_MAKE_FILE
*
**  Description: Create Dept File.
*==========================================================================
*/
SHORT   OpDeptCreatFile(USHORT usNumberOfDept, USHORT usLockHnd)
{
    OPDEPT_FILEHED DeptFileHed;
    SHORT sStatus;
    ULONG ulActualPosition;

    PifRequestSem(husOpSem);

    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return(OP_LOCK);
    }

    if (!usNumberOfDept) {           /* usNumberOfDept == 0 */
        PifDeleteFile((WCHAR FAR *)auchOP_DEPT);
        PifReleaseSem(husOpSem);
        return(OP_PERFORM);
    }
    if ((hsOpDeptFileHandle = PifOpenFile((WCHAR FAR *)auchOP_DEPT, 
                (CHAR FAR *)auchNEW_FILE_WRITE)) == PIF_ERROR_FILE_EXIST) {
        if ((hsOpDeptFileHandle = PifOpenFile((WCHAR FAR *)auchOP_DEPT, 
                (CHAR FAR *)auchOLD_FILE_READ)) < 0) {
            PifAbort(MODULE_OP_DEPT, FAULT_ERROR_FILE_OPEN);
        }
        /* Size 0 Check */
        if (PifSeekFile(hsOpDeptFileHandle, sizeof(OPDEPT_FILEHED), 
                        &ulActualPosition) == PIF_ERROR_FILE_EOF) {
            PifCloseFile(hsOpDeptFileHandle);
            if ((hsOpDeptFileHandle = PifOpenFile((WCHAR FAR *)auchOP_DEPT, 
                    (CHAR FAR *)auchOLD_FILE_WRITE)) < 0) {
                PifAbort(MODULE_OP_DEPT, FAULT_ERROR_FILE_OPEN);
            }
            sStatus = Op_NewDeptCreatFile(usNumberOfDept);
        } else {
            PifCloseFile(hsOpDeptFileHandle);
            if ((hsOpDeptFileHandle = PifOpenFile((WCHAR FAR *)auchOP_DEPT,
                    (CHAR FAR *)auchOLD_FILE_READ_WRITE)) < 0) {
                PifAbort(MODULE_OP_DEPT, FAULT_ERROR_FILE_OPEN);
            }
            Op_ReadDeptFile(OP_FILE_HEAD_POSITION, 
                           &DeptFileHed, sizeof(DeptFileHed));
            if (DeptFileHed.usNumberOfMaxDept > usNumberOfDept) {
                sStatus = Op_CompressDeptFile(usNumberOfDept);
            } else if (DeptFileHed.usNumberOfMaxDept < usNumberOfDept) {
                sStatus = Op_ExpandDeptFile(usNumberOfDept);
            } else {
                sStatus = PifControlFile(hsOpDeptFileHandle, 
                                         PIF_FILE_CHANGE_SIZE, 
                                         DeptFileHed.ulDeptFileSize);
                if (sStatus == PIF_OK) {
                    sStatus = OP_PERFORM;
                }
            }
        }
    } else {                         
        sStatus = Op_NewDeptCreatFile(usNumberOfDept);
    }
    Op_CloseDeptFileReleaseSem();
    return(sStatus);
}

/*
*==========================================================================
**   Synopsis:    SHORT   Op_NewDeptCreatFile(USHORT usNumberOfDept, 
*                                            USHORT usLockHnd)
*
*       Input: usNumberOfDept - Number of Dept
*      Output: Nothing
*       InOut: Nothing
*
**  Return    :
*     Normal End:  OP_PERFORM
*   Abnormal End:  OP_NO_MAKE_FILE
*
**  Description: Create Dept File.
*==========================================================================
*/
SHORT   Op_NewDeptCreatFile(USHORT usNumberOfDept)
{
    OPDEPT_FILEHED DeptFileHed;
    ULONG  ulActualPosition;
    UCHAR  auchParaEmpTable[OPDEPT_PARA_EMPTY_TABLE_SIZE];

    /* Make File Header */

    Op_CreatDeptFileHeader(&DeptFileHed, usNumberOfDept);
                                      
    /* Check file size and creat file */

    if (PifSeekFile(hsOpDeptFileHandle, 
                    DeptFileHed.ulDeptFileSize, 
                    &ulActualPosition) < 0) { /* PIF_ERROR_DISK_FULL */
        PifCloseFile(hsOpDeptFileHandle);
        PifDeleteFile((WCHAR FAR *)auchOP_DEPT);          /* Can't make file then delete file */
        return(OP_NO_MAKE_FILE);
    }

    /* Write File Header */

    Op_WriteDeptFile(OP_FILE_HEAD_POSITION, &DeptFileHed, sizeof(DeptFileHed));

    /* Clear and Write Empty table */

    memset(auchParaEmpTable, 0x00, OPDEPT_PARA_EMPTY_TABLE_SIZE);
    Op_WriteDeptFile((ULONG)DeptFileHed.offulOfParaEmpTbl, 
                    auchParaEmpTable, OPDEPT_PARA_EMPTY_TABLE_SIZE);
    return OP_PERFORM;
}
/*
*==========================================================================
**   Synopsis:    SHORT   Op_ExpandDeptFile(USHORT usNumberOfDept)
*
*       Input: usNumberOfDept - Number of Dept
*      Output: Nothing
*       InOut: Nothing
*
**  Return   :
*     Normal End: OP_PERFORM
*   Abnormal End: OP_NO_MAKE_FILE
*
**  Description: Create Dept File Expanded.
*==========================================================================
*/
SHORT   Op_ExpandDeptFile(USHORT usNumberOfDept)
{
    OPDEPT_FILEHED DeptFileHed;
    OPDEPT_FILEHED DeptFileHed_Old;
    UCHAR auchParaEmpTable[OPDEPT_PARA_EMPTY_TABLE_SIZE];
    ULONG   ulActualPosition;

    /* Make File Header */

    Op_CreatDeptFileHeader(&DeptFileHed, usNumberOfDept);

    /* Check file size and creat file */

    if (PifSeekFile(hsOpDeptFileHandle, 
                    DeptFileHed.ulDeptFileSize, &ulActualPosition) < 0) { /* PIF_ERROR_DISK_FULL */
        PifCloseFile(hsOpDeptFileHandle);
        PifDeleteFile((WCHAR FAR *)auchOP_DEPT);          /* Can't make file then delete file */
        return(OP_NO_MAKE_FILE);
    }
    /* Read File Header */

    Op_ReadDeptFile(OP_FILE_HEAD_POSITION, &DeptFileHed_Old, 
                   sizeof(DeptFileHed_Old));

    /* Move Data In File */
    /* Move Paramater */

    Op_MoveTableExpand(DeptFileHed_Old.offulOfParaTbl,
                       DeptFileHed.offulOfParaTbl,
                       ((ULONG)(DeptFileHed_Old.usNumberOfMaxDept) * 
                       ((ULONG)(sizeof(OPDEPT_PARAENTRY)))));

    /* Move Empty Block Table */

    memset(auchParaEmpTable, 0x00, OPDEPT_PARA_EMPTY_TABLE_SIZE);
    Op_ReadDeptFile((ULONG)DeptFileHed_Old.offulOfParaEmpTbl, 
                   auchParaEmpTable, OPDEPT_PARA_EMPTY_TABLE_SIZE);
    Op_WriteDeptFile((ULONG)DeptFileHed.offulOfParaEmpTbl, 
                   auchParaEmpTable, OPDEPT_PARA_EMPTY_TABLE_SIZE);

    /* Move Dept Index Table */

/*** NCR2172
    Op_MoveTableExpand(DeptFileHed_Old.offulOfIndexTblDept, 
                       DeptFileHed.offulOfIndexTblDept,
                       ((ULONG)(DeptFileHed_Old.usNumberOfResidentDept)
                       * ((ULONG)(sizeof(OPDEPT_INDEXENTRYBYDEPT)))));
***/
    /* Move Report Index Table */

/*** NCR2172
    Op_MoveTableExpand(DeptFileHed_Old.offulOfIndexTblRept, 
                       DeptFileHed.offulOfIndexTblRept,
                       ((ULONG)(DeptFileHed_Old.usNumberOfResidentDept)
                       * ((ULONG)(sizeof(OPDEPT_INDEXENTRYBYREPORT)))));
***/

    /* Move Index Block Table */

    Op_MoveTableExpand(DeptFileHed_Old.offulOfIndexTblBlk, 
                       DeptFileHed.offulOfIndexTblBlk,
                       ((ULONG)(DeptFileHed_Old.usNumberOfResidentDept)
                       * ((ULONG)(sizeof(OP_DEPT_INDEXBLK_SIZE)))));

    /* Move Index Table */

    Op_MoveTableExpand(DeptFileHed_Old.offulOfIndexTbl, 
                       DeptFileHed.offulOfIndexTbl,
                       ((ULONG)(DeptFileHed_Old.usNumberOfResidentDept) * 
                       ((ULONG)(sizeof(OPDEPT_INDEXENTRY)))));

    /* Write File Header */

    DeptFileHed.usNumberOfResidentDept = DeptFileHed_Old.usNumberOfResidentDept;
    Op_WriteDeptFile(OP_FILE_HEAD_POSITION, &DeptFileHed, sizeof(DeptFileHed));
   
    return (OP_PERFORM);
}
/*
*==========================================================================
**   Synopsis:    SHORT   Op_CompressDeptFile(USHORT usNewNumberOfDept)
*
*       Input: usNewNumberOfDept - Number of Dept (Creating File)
*      Output: Nothing
*       InOut: Nothing
*
**  Return   :
*     Normal End: OP_PERFORM
*   Abnormal End: OP_NO_MAKE_FILE
*
**  Description: Compress Dept File.
*==========================================================================
*/
SHORT Op_CompressDeptFile(USHORT usNewNumberOfDept)
{
    OPDEPT_FILEHED DeptFileHed;
    OPDEPT_FILEHED DeptFileHed_Old;
    UCHAR  auchParaEmpTable[OPDEPT_PARA_EMPTY_TABLE_SIZE];
    SHORT sStatus;

    /* Check Compress or not,   Saratoga *
    if ((sStatus = Op_CompressParaTable(usNewNumberOfDept)) 
                                                == OP_NO_MAKE_FILE) {
        return (OP_NO_COMPRESS_FILE);
    }   */

    Op_ReadDeptFile(OP_FILE_HEAD_POSITION, &DeptFileHed_Old, 
                  sizeof(OPDEPT_FILEHED));
    Op_CreatDeptFileHeader(&DeptFileHed, usNewNumberOfDept);

    /* Write File Header */

    DeptFileHed.usNumberOfResidentDept = DeptFileHed_Old.usNumberOfResidentDept;
    
    Op_WriteDeptFile(OP_FILE_HEAD_POSITION, &DeptFileHed, sizeof(DeptFileHed));

    /* Move Index Table */

    Op_MoveTableCompress(DeptFileHed_Old.offulOfIndexTbl, 
                         DeptFileHed.offulOfIndexTbl,
                         ((ULONG)(DeptFileHed_Old.usNumberOfResidentDept)
                         * ((ULONG)(sizeof(OPDEPT_INDEXENTRY)))));
    
    /* Move Index Block Table */

    Op_MoveTableCompress(DeptFileHed_Old.offulOfIndexTblBlk, 
                         DeptFileHed.offulOfIndexTblBlk,
                         ((ULONG)(DeptFileHed_Old.usNumberOfResidentDept) * 
                         ((ULONG)sizeof(OP_DEPT_INDEXBLK_SIZE))));

    /* Move Report Index Table */

/*** NCR2172
    Op_MoveTableCompress(DeptFileHed_Old.offulOfIndexTblRept, 
                         DeptFileHed.offulOfIndexTblRept,
                         ((ULONG)(DeptFileHed_Old.usNumberOfResidentDept) * 
                         ((ULONG)sizeof(OPDEPT_INDEXENTRYBYREPORT))));
***/

    /* Move Dept Index Table */

/*** NCR2172
    Op_MoveTableCompress(DeptFileHed_Old.offulOfIndexTblDept, 
                         DeptFileHed.offulOfIndexTblDept,
                         ((ULONG)(DeptFileHed_Old.usNumberOfResidentDept) *
                         ((ULONG)sizeof(OPDEPT_INDEXENTRYBYDEPT))));
***/

    /* Move Empty Block Table */

    memset(auchParaEmpTable, 0x00, OPDEPT_PARA_EMPTY_TABLE_SIZE);
    Op_ReadDeptFile((ULONG)DeptFileHed_Old.offulOfParaEmpTbl, 
                    auchParaEmpTable, OPDEPT_PARA_EMPTY_TABLE_SIZE);
    Op_WriteDeptFile((ULONG)DeptFileHed.offulOfParaEmpTbl, 
                    auchParaEmpTable, OPDEPT_PARA_EMPTY_TABLE_SIZE);

    /* Move Paramater */

    Op_MoveTableCompress(DeptFileHed_Old.offulOfParaTbl, 
                         DeptFileHed.offulOfParaTbl,
                         ((ULONG)(DeptFileHed_Old.usNumberOfResidentDept) * 
                         ((ULONG)sizeof(OPDEPT_PARAENTRY))));

    if ((sStatus = PifControlFile(hsOpDeptFileHandle, 
                                  PIF_FILE_CHANGE_SIZE, 
                                  DeptFileHed.ulDeptFileSize)) < 0 ) {
        return (OP_NO_MAKE_FILE);
    }
    return OP_PERFORM;
}
/*
*==========================================================================
**   Synopsis:    VOID   Op_CreatDeptFileHeader(OPDEPT_FILEHED FAR *pDeptFileHed,
*                                               USHORT usNumberOfDept)
*
*       Input: *pDeptFileHed 
*      Output: Nothing
*       InOut: Nothing
*
**  Return   :
*
**  Description: Create File Header.
*==========================================================================
*/
VOID   Op_CreatDeptFileHeader(OPDEPT_FILEHED FAR *pDeptFileHed, 
                              USHORT usNumberOfDept)
{
    pDeptFileHed->usNumberOfMaxDept = usNumberOfDept;
    pDeptFileHed->usNumberOfResidentDept = 0x00;
    pDeptFileHed->offulOfIndexTbl = sizeof(OPDEPT_FILEHED);
    pDeptFileHed->offulOfIndexTblBlk = pDeptFileHed->offulOfIndexTbl +
                  (ULONG)(usNumberOfDept * sizeof(OPDEPT_INDEXENTRY));

/*** NCR2172
    pDeptFileHed->offulOfIndexTblRept = pDeptFileHed->offulOfIndexTblBlk
                + (ULONG)(usNumberOfDept * OP_DEPT_INDEXBLK_SIZE);

    pDeptFileHed->offulOfIndexTblDept = pDeptFileHed->offulOfIndexTblRept 
                + (ULONG)(usNumberOfDept * sizeof(OPDEPT_INDEXENTRYBYREPORT));

    pDeptFileHed->offulOfIndexTblGrpNo = pDeptFileHed->offulOfIndexTblDept 
                + (ULONG)(usNumberOfDept * sizeof(OPDEPT_INDEXENTRYBYDEPT));

    pDeptFileHed->offulOfParaEmpTbl = pDeptFileHed->offulOfIndexTblBlk
                + (ULONG)(usNumberOfDept * sizeof(OPDEPT_INDEXENTRYBYGROUPNO));
***/
    /* Saratoga */
    pDeptFileHed->offulOfParaEmpTbl =  pDeptFileHed->offulOfIndexTblBlk +
                        (usNumberOfDept * sizeof(OP_DEPT_INDEXBLK_SIZE));

    pDeptFileHed->offulOfParaTbl = pDeptFileHed->offulOfParaEmpTbl +
                (ULONG)(OPDEPT_PARA_EMPTY_TABLE_SIZE);

    pDeptFileHed->offulOfWork   = pDeptFileHed->offulOfParaTbl +
                    ((ULONG)usNumberOfDept) * 
                    ((ULONG)sizeof(OPDEPT_PARAENTRY));

    pDeptFileHed->ulDeptFileSize = pDeptFileHed->offulOfWork + usNumberOfDept;
}


/*** NCR2172 End   ***/
/*====== End of Source ========*/