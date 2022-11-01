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
*   Title              : Client/Server cashier module, Create File (cashier paramater file)
*   Category           : Client/Server cashier module, NCR2170 US HOSPITALITY PEP.
*   Program Name       : CSCASPE.C
*  ------------------------------------------------------------------------  
*   Compile            : MS-C Ver. 7.00 A by Microsoft Corp.              
*   Memory Model       : Large Model
*   Options            : /c /Alfw /W4 /Zpi /G2s /Os
*  ------------------------------------------------------------------------
*   Abstract           :
*
*       USER I/F FUNCITONS
*
*       CasCreatFile              * Create Cas File.
*
*       INTERNAL FUNCTIONS
*
*       Cas_NewCasCreatFile          * Create Cas File newly. 
*       Cas_ExpandCasFile            * Expand Cas File.
*       Cas_CompressCasFile          * Compress Cas File.
*
*       Cas_CreatCasFileHeader       * Create Cas File Header.
*       Cas_CompressParaTable        * Compress Paramater Table.
*       Cas_MoveTableExpand          * Expand Table.
*       Cas_MoveTableCompress        * Compress Table.
*       
*
*  ------------------------------------------------------------------------
*    Update Histories  :
*   Date                 Ver.Rev    :NAME         :Description
*   Nov-24-92          : for pep    :M.YAMAMOTO   :Initial
*   Apr-10-93          : for pep    :H.YAMAGUCHI  :Modified
*   Sep-03-98          : for pep    :A.Mitsui     :V3.3 Modified
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
#include    <plu.h>
#include    <rfl.h>
#include    <csstbfcc.h>
#include    <paraequ.h>
#include    <cscas.h>
#include    <appllog.h>
#include    "cscaspe.h"
#include    "cscasin.h"
#include    "mypifdefs.h"

/*
*==========================================================================
**    Synopsis:    SHORT CasCreatFile(USHORT usNumberOfCashier)
*
*       Input:    USHORT  usNumberOfCashier - Number of Cashier.
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:  CAS_PERFORM
*          Abnormal End:  CAS_NO_MAKE_FILE
*
**  Description:
*               Create Cashier File. V3.3
*==========================================================================
*/
SHORT CasCreatFile(USHORT usNumberOfCashier)
{
	CAS_FILEHED CasFileHed = { 0 };
    SHORT sStatus;
    ULONG ulActualPosition;

    PifRequestSem(husCashierSem);

    if (!usNumberOfCashier) {           /* usNumberOfCashier == 0 */
        PifDeleteFile(auchCAS_CASHIER);
        PifReleaseSem(husCashierSem);
        return(CAS_PERFORM);
    }          
    if ((hsCashierFileHandle = PifOpenFile(auchCAS_CASHIER, auchNEW_FILE_WRITE)) == PIF_ERROR_FILE_EXIST) {
        if ((hsCashierFileHandle = PifOpenFile(auchCAS_CASHIER, auchOLD_FILE_READ)) < 0) {
            PifAbort(MODULE_CASHIER, FAULT_ERROR_FILE_OPEN);
        }
        /* Size 0 Check */
        if (PifSeekFile(hsCashierFileHandle, sizeof(CAS_FILEHED), &ulActualPosition) == PIF_ERROR_FILE_EOF) {
            PifCloseFile(hsCashierFileHandle);
            if ((hsCashierFileHandle = PifOpenFile(auchCAS_CASHIER, auchOLD_FILE_WRITE)) < 0) {
                PifAbort(MODULE_CASHIER, FAULT_ERROR_FILE_OPEN);
            }
            sStatus = Cas_NewCashierCreatFile(usNumberOfCashier);
        } else {
            PifCloseFile(hsCashierFileHandle);
            if ((hsCashierFileHandle = PifOpenFile(auchCAS_CASHIER, auchOLD_FILE_READ_WRITE)) < 0) {
                PifAbort(MODULE_CASHIER, FAULT_ERROR_FILE_OPEN);
            }
            Cas_ReadFile(CAS_FILE_HED_POSITION, &CasFileHed, sizeof(CasFileHed));
            if (CasFileHed.usNumberOfMaxCashier > usNumberOfCashier) {
                sStatus = Cas_CompressCashierFile(usNumberOfCashier);
            } else if (CasFileHed.usNumberOfMaxCashier < usNumberOfCashier) {
                sStatus = Cas_ExpandCashierFile(usNumberOfCashier);
            } else {
                sStatus = PifControlFile(hsCashierFileHandle, PIF_FILE_CHANGE_SIZE, CasFileHed.ulCasFileSize);
                if (sStatus == PIF_OK) {
                    sStatus = CAS_PERFORM;
                }
            }
        }
    } else {                         
        sStatus = Cas_NewCashierCreatFile(usNumberOfCashier);
    }

	//initialize the fingerprint file
	// DFPRInitFile(L"DigitalPersona 4500",3);

    Cas_CloseFileReleaseSem();
    return(sStatus);
}
/*
*==========================================================================
**   Synopsis:    SHORT   Cas_NewCashierCreatFile(USHORT usNumberOfCashier)
*
*       Input:    USHORT  usNumberOfCashier - Number of Cashier
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:  CAS_PERFORM
*          Abnormal End:  CAS_NO_MAKE_FILE
*
**  Description:
*               Create Cashier File.
*==========================================================================
*/
SHORT   Cas_NewCashierCreatFile(USHORT usNumberOfCashier)
{

	CAS_FILEHED CasFileHed = { 0 };
    ULONG  ulActualPosition;
    UCHAR  auchParaEmpTable[CAS_PARA_EMPTY_TABLE_SIZE];

    /* Make File Header */
    Cas_CreatCashierFileHeader(&CasFileHed, usNumberOfCashier);
                                      
    /* Check file size and creat file */
    if (PifSeekFile(hsCashierFileHandle, CasFileHed.ulCasFileSize, &ulActualPosition) < 0) { /* PIF_ERROR_DISK_FULL */
        PifCloseFile(hsCashierFileHandle);
        PifDeleteFile(auchCAS_CASHIER);          /* Can't make file then delete file */
        return(CAS_NO_MAKE_FILE);
    }

    /* Write File Header */
    Cas_WriteFile(CAS_FILE_HED_POSITION, &CasFileHed, sizeof(CasFileHed));

    /* Clear and Write Empty table */
    memset(auchParaEmpTable, 0x00, CAS_PARA_EMPTY_TABLE_SIZE);
    Cas_WriteFile((ULONG)CasFileHed.offusOffsetOfParaEmpTbl, auchParaEmpTable, CAS_PARA_EMPTY_TABLE_SIZE);
    return CAS_PERFORM;
}
/*
*==========================================================================
**   Synopsis:    SHORT   Cas_ExpandCashierFile(USHORT usNumberOfCashier)
*
*       Input:    USHORT  usNumberOfCashier - Number of Cashier
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:  CAS_PERFORM
*          Abnormal End:  CAS_NO_MAKE_FILE
*
**  Description:
*               Create Cashier File Expanded.
*==========================================================================
*/
SHORT   Cas_ExpandCashierFile(USHORT usNumberOfCashier)
{
	CAS_FILEHED CasFileHed = { 0 };
	CAS_FILEHED CasFileHed_Old = { 0 };
	UCHAR   auchParaEmpTable[CAS_PARA_EMPTY_TABLE_SIZE];
	ULONG   ulActualPosition;

    /* Make File Header */
    Cas_CreatCashierFileHeader(&CasFileHed, usNumberOfCashier);

    /* Check file size and creat file */
    if (PifSeekFile(hsCashierFileHandle, CasFileHed.ulCasFileSize, &ulActualPosition) < 0) { /* PIF_ERROR_DISK_FULL */
        PifCloseFile(hsCashierFileHandle);
        PifDeleteFile(auchCAS_CASHIER);          /* Can't make file then delete file */
        return(CAS_NO_MAKE_FILE);
    }

    /* Read File Header */
    Cas_ReadFile(CAS_FILE_HED_POSITION, &CasFileHed_Old, sizeof(CasFileHed_Old));

    /* Move Data In File */
    /* Move Paramater */
    Cas_MoveTableExpand(CasFileHed_Old.offusOffsetOfParaTbl, CasFileHed.offusOffsetOfParaTbl,
                 (USHORT)(CasFileHed_Old.usNumberOfMaxCashier * (USHORT)sizeof(CAS_PARAENTRY)));

    /* Move Empty Block Table */
    memset(auchParaEmpTable, 0x00, CAS_PARA_EMPTY_TABLE_SIZE);
    Cas_ReadFile((ULONG)CasFileHed_Old.offusOffsetOfParaEmpTbl, auchParaEmpTable, CAS_PARA_EMPTY_TABLE_SIZE);
    Cas_WriteFile((ULONG)CasFileHed.offusOffsetOfParaEmpTbl, auchParaEmpTable, CAS_PARA_EMPTY_TABLE_SIZE);

    /* Move Index Table */
    Cas_MoveTableExpand(CasFileHed_Old.offusOffsetOfIndexTbl, CasFileHed.offusOffsetOfIndexTbl,
                 (USHORT)(CasFileHed_Old.usNumberOfResidentCashier * (USHORT)sizeof(CAS_INDEXENTRY)));

    /* Write File Header */
    CasFileHed.usNumberOfResidentCashier = CasFileHed_Old.usNumberOfResidentCashier;
    Cas_WriteFile(CAS_FILE_HED_POSITION, &CasFileHed, sizeof(CasFileHed));
   
    return (CAS_PERFORM);
}
/*
*==========================================================================
**   Synopsis:    SHORT   Cas_CompressCashierFile(USHORT usNewNumberOfCashier)
*
*       Input:    USHORT  usNewNumberOfCashier - Number of Cashier (Creating File)
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:  CAS_PERFORM
*          Abnormal End:  CAS_NO_MAKE_FILE
*
**  Description:
*               Compress Cashier File.
*==========================================================================
*/
SHORT Cas_CompressCashierFile(USHORT usNewNumberOfCashier)
{
	CAS_FILEHED CasFileHed = { 0 };
	CAS_FILEHED CasFileHed_Old = { 0 };
	UCHAR  auchParaEmpTable[CAS_PARA_EMPTY_TABLE_SIZE];
	SHORT  sStatus;

    /* Check Compress or not */
    if ((sStatus = Cas_CompressParaTable(usNewNumberOfCashier)) == CAS_NO_MAKE_FILE) {
        return (CAS_NO_COMPRESS_FILE);
    }
    Cas_ReadFile(CAS_FILE_HED_POSITION, &CasFileHed_Old, sizeof(CAS_FILEHED));
    Cas_CreatCashierFileHeader(&CasFileHed, usNewNumberOfCashier);

    /* Write File Header */
    CasFileHed.usNumberOfResidentCashier = CasFileHed_Old.usNumberOfResidentCashier;
    Cas_WriteFile(CAS_FILE_HED_POSITION, &CasFileHed, sizeof(CasFileHed));

    /* Move Index Table */
    Cas_MoveTableCompress(CasFileHed_Old.offusOffsetOfIndexTbl, CasFileHed.offusOffsetOfIndexTbl,
                 (USHORT)(CasFileHed_Old.usNumberOfResidentCashier * sizeof(CAS_INDEXENTRY)));
    
    /* Move Empty Block Table */
    memset(auchParaEmpTable, 0x00, CAS_PARA_EMPTY_TABLE_SIZE);
    Cas_ReadFile((ULONG)CasFileHed_Old.offusOffsetOfParaEmpTbl, auchParaEmpTable, CAS_PARA_EMPTY_TABLE_SIZE);
    Cas_WriteFile((ULONG)CasFileHed.offusOffsetOfParaEmpTbl, auchParaEmpTable, CAS_PARA_EMPTY_TABLE_SIZE);

    /* Move Paramater */
    Cas_MoveTableCompress(CasFileHed_Old.offusOffsetOfParaTbl, CasFileHed.offusOffsetOfParaTbl,
                 (USHORT)(CasFileHed_Old.usNumberOfResidentCashier * sizeof(CAS_PARAENTRY)));

    if ((sStatus = PifControlFile(hsCashierFileHandle, PIF_FILE_CHANGE_SIZE, CasFileHed.ulCasFileSize)) < 0 ) {
        return (CAS_NO_MAKE_FILE);
    }
    return CAS_PERFORM;
}
/*
*==========================================================================
**   Synopsis:    VOID   Cas_CreatCashierFileHeader(CAS_FILEHED FAR *pCasFileHed, USHORT usNumberOfCashier)
*
*       Input:    CAS_FILEHED *pCasFileHed 
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*
**  Description:
*               Create File Header.
*==========================================================================
*/
VOID   Cas_CreatCashierFileHeader(CAS_FILEHED FAR *pCasFileHed, USHORT usNumberOfCashier)
{
    pCasFileHed->usNumberOfMaxCashier = usNumberOfCashier;  /* V3.3 */
    pCasFileHed->usNumberOfResidentCashier = 0x00;
    pCasFileHed->offusOffsetOfIndexTbl = sizeof(CAS_FILEHED);
    pCasFileHed->offusOffsetOfParaEmpTbl =  (USHORT)(pCasFileHed->offusOffsetOfIndexTbl + (usNumberOfCashier * sizeof(CAS_INDEXENTRY)));
    pCasFileHed->offusOffsetOfParaTbl = (USHORT)(pCasFileHed->offusOffsetOfParaEmpTbl + CAS_PARA_EMPTY_TABLE_SIZE);
    pCasFileHed->ulCasFileSize = (ULONG)(pCasFileHed->offusOffsetOfParaTbl + ((LONG)usNumberOfCashier * sizeof(CAS_PARAENTRY)));
}
/*
*==========================================================================
**   Synopsis:    SHORT   Cas_CompressParaTable(USHORT usNewNumberOfCashier)
*
*       Input:    USHORT  usNewNumberOfCashier - Number of Cashier (Creating File)
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:  CAS_PERFORM
*          Abnormal End:  CAS_NO_MAKE_FILE
*
**  Description:
*               Compress Cashier File.
*==========================================================================
*/
SHORT   Cas_CompressParaTable(USHORT usNewNumberOfCashier)
{
	CAS_FILEHED     CasFileHed = { 0 };
	USHORT          offusIndex,usI;
	CAS_PARAENTRY   ParaCashier = { 0 };
	CAS_INDEXENTRY  IndexCashier = { 0 }, IndexNew = { 0 };

    Cas_ReadFile(CAS_FILE_HED_POSITION, &CasFileHed, sizeof(CasFileHed));

    if (CasFileHed.usNumberOfResidentCashier > usNewNumberOfCashier) {
        return(CAS_NO_MAKE_FILE);
    }
    for (usI = 0; usI < CasFileHed.usNumberOfResidentCashier; usI++) {
        offusIndex = (USHORT)(CasFileHed.offusOffsetOfIndexTbl + (usI * sizeof(CAS_INDEXENTRY)));
        Cas_ReadFile(offusIndex, &IndexCashier, sizeof(CAS_INDEXENTRY));
        if (IndexCashier.usParaBlock > usNewNumberOfCashier) {
            if (Cas_EmpBlkGet(&CasFileHed, &IndexNew.usParaBlock) == CAS_CASHIER_FULL) {
                PifAbort(MODULE_CASHIER, FAULT_ERROR_FILE_BROKEN); /* File Broken */
            }
            IndexNew.ulCashierNo = IndexCashier.ulCashierNo;
            Cas_WriteFile(offusIndex, &IndexNew, sizeof(CAS_INDEXENTRY));
            Cas_ParaRead(&CasFileHed, IndexCashier.usParaBlock, &ParaCashier);
            Cas_ParaWrite(&CasFileHed, IndexNew.usParaBlock, &ParaCashier);
            Cas_BlkEmp( &CasFileHed, IndexCashier.usParaBlock );
        }
    }
    return CAS_PERFORM;
}
/*
*==========================================================================
**   Synopsis:    SHORT   Cas_MoveTableExpand(USHORT offusSrcTable, USHORT offusObjTable, USHORT usMoveByte)
*
*       Input:    USHORT  offusSrcTable 
*                 USHORT  offusObjTable  
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:  CAS_PERFORM
*          Abnormal End:  CAS_NO_MAKE_FILE
*
**  Description:
*               Move a Table offusSrcTable to offusObjTable by usMoveByte.
*==========================================================================
*/
VOID   Cas_MoveTableExpand(USHORT offusSrcTable, USHORT offusObjTable, USHORT usMoveByte)
{
    UCHAR   auchWorkBuffer[CAS_MAX_WORK_BUFFER];
    USHORT  offusTailSrcTable;
    USHORT  offusTailObjTable;

    offusTailSrcTable = (USHORT)(offusSrcTable + usMoveByte);
    offusTailObjTable = (USHORT)(offusObjTable + usMoveByte);

    while (usMoveByte > CAS_MAX_WORK_BUFFER) {
        offusTailSrcTable -= CAS_MAX_WORK_BUFFER;
        offusTailObjTable -= CAS_MAX_WORK_BUFFER;
        Cas_ReadFile(offusTailSrcTable, auchWorkBuffer, CAS_MAX_WORK_BUFFER);
        Cas_WriteFile(offusTailObjTable, auchWorkBuffer, CAS_MAX_WORK_BUFFER);
        usMoveByte -= CAS_MAX_WORK_BUFFER;
    }
    if (usMoveByte != 0) {
        Cas_ReadFile(offusSrcTable, auchWorkBuffer, usMoveByte);
        Cas_WriteFile(offusObjTable, auchWorkBuffer, usMoveByte);
    }
}
/*
*==========================================================================
**   Synopsis:    SHORT   Cas_MoveTableCompress(USHORT offusSrcTable, USHORT offusObjTable, USHORT usMoveByte)
*
*       Input:    USHORT  offusSrcTable 
*                 USHORT  offusObjTable  
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:  CAS_PERFORM
*          Abnormal End:  CAS_NO_MAKE_FILE
*
**  Description:
*               Move a Table offusSrcTable to offusObjTable by usMoveByte.
*==========================================================================
*/
VOID   Cas_MoveTableCompress(USHORT offusSrcTable, USHORT offusObjTable, USHORT usMoveByte)
{
    UCHAR   auchWorkBuffer[CAS_MAX_WORK_BUFFER];

    while (usMoveByte > CAS_MAX_WORK_BUFFER) {
        Cas_ReadFile(offusSrcTable, auchWorkBuffer, CAS_MAX_WORK_BUFFER);
        Cas_WriteFile(offusObjTable, auchWorkBuffer, CAS_MAX_WORK_BUFFER);
        usMoveByte -= CAS_MAX_WORK_BUFFER;
        offusSrcTable += CAS_MAX_WORK_BUFFER;
        offusObjTable += CAS_MAX_WORK_BUFFER;
    }
    if (usMoveByte != 0) {
        Cas_ReadFile(offusSrcTable, auchWorkBuffer, usMoveByte);
        Cas_WriteFile(offusObjTable, auchWorkBuffer, usMoveByte);
    }
}


/*
*==========================================================================
**    Synopsis:    SHORT   CasMigrate31File(VOID)
*
*       Input: Nothing
*      Output: Nothing
*       InOut: Nothing
*
**  Return    :
*     Normal End:  CAS_PERFORM
*   Abnormal End:  CAS_NO_MAKE_FILE
*
**  Description: Create New CAS (Ver 3.3) File from Old CAS.(Header copy)
*==========================================================================
*/
SHORT   CasMigrate31File(VOID)
{
	SHORT           sStatus;
	CAS_FILEHED     CasFileHed = { 0 };
	CAS_FILEHEDOLD  CasFileHedOld = { 0 };
	USHORT          usMovePosition;
	ULONG           ulActualPosition, ulRemainData;
	UCHAR   auchWorkBuffer[CAS_MAX_WORK_BUFFER];
    
    PifRequestSem(husCashierSem);

    /* get File handle */
    if ((hsCashierFileHandle = PifOpenFile(auchCAS_CASHIEROLD, auchOLD_FILE_READ_WRITE)) == PIF_ERROR_FILE_EXIST) {
        PifReleaseSem(husCashierSem);
        return(CAS_FILE_NOT_FOUND);
    }
    
    /* Read Old Header */
    if ((PifReadFile(hsCashierFileHandle, &CasFileHedOld, sizeof(CasFileHedOld))) != sizeof(CasFileHedOld)) {
        PifCloseFile(hsCashierFileHandle);
        PifReleaseSem(husCashierSem);
        return(CAS_FILE_NOT_FOUND);
    }

    /* Make New Header */
    Cas_CreatCashierFileHeader(&CasFileHed, (USHORT)CasFileHedOld.uchNumberOfMaxCashier);
    CasFileHed.usNumberOfResidentCashier = (USHORT)CasFileHedOld.uchNumberOfResidentCashier;

    /* Make New File */
    PifDeleteFile(auchCAS_CASHIER);
    if (( hsCashierNewFileHandle = PifOpenFile(auchCAS_CASHIER, auchNEW_FILE_WRITE)) == PIF_ERROR_FILE_EXIST) {
        if ((hsCashierNewFileHandle = PifOpenFile(auchCAS_CASHIER, auchOLD_FILE_READ_WRITE)) < 0) {
            PifAbort(MODULE_CASHIER, FAULT_ERROR_FILE_BROKEN); /* File Broken */
        }
    }

    /* Check new file size */
    if (PifSeekFile(hsCashierNewFileHandle, CasFileHed.ulCasFileSize, &ulActualPosition) < 0) { /* PIF_ERROR_DISK_FULL */
        PifCloseFile(hsCashierNewFileHandle);
        PifDeleteFile(auchCAS_CASHIER);          /* Can't make file then delete file */
        return(CAS_NO_MAKE_FILE);
    }

    /* Clear New File */
    ulRemainData   = CasFileHed.ulCasFileSize;
    usMovePosition = CAS_FILE_HED_POSITION;

    while (ulRemainData > CAS_MAX_WORK_BUFFER) {
        memset(auchWorkBuffer, 0x00, sizeof(auchWorkBuffer));
        Cas_MigrNewWrite(usMovePosition, auchWorkBuffer, CAS_MAX_WORK_BUFFER);
        ulRemainData   -= CAS_MAX_WORK_BUFFER;
        usMovePosition += CAS_MAX_WORK_BUFFER;
    }

    if (ulRemainData != 0) {
        memset(auchWorkBuffer, 0x00, sizeof(auchWorkBuffer));
        Cas_MigrNewWrite(usMovePosition, auchWorkBuffer, (USHORT)ulRemainData);
    }

    /* Copy PARAM!CA to PARACAS */

        /* INDEX ENTRY */
    Cas_ChangeIndexTable((USHORT)CasFileHedOld.offusOffsetOfIndexTbl, CasFileHed.offusOffsetOfIndexTbl,
                         (USHORT)(CasFileHedOld.offusOffsetOfParaEmpTbl - CasFileHedOld.offusOffsetOfIndexTbl));

        /* Para Empty Table */
    Cas_MigrCopyTable((USHORT)CasFileHedOld.offusOffsetOfParaEmpTbl, CasFileHed.offusOffsetOfParaEmpTbl,
                      (USHORT)(CasFileHedOld.offusOffsetOfParaTbl - CasFileHedOld.offusOffsetOfParaEmpTbl));

        /* PARAENTRY */
    Cas_ChangeParaTable((USHORT)CasFileHedOld.offusOffsetOfParaTbl, CasFileHed.offusOffsetOfParaTbl,
                        (USHORT)((USHORT)CasFileHedOld.ulCasFileSize - CasFileHedOld.offusOffsetOfParaTbl));

    /* Write new header */
    Cas_MigrNewWrite(CAS_FILE_HED_POSITION, &CasFileHed, sizeof(CasFileHed));

    sStatus = CAS_PERFORM;

    PifCloseFile(hsCashierFileHandle);
    PifCloseFile(hsCashierNewFileHandle);
    PifReleaseSem(husCashierSem);
    return(sStatus);

}

/*
*==========================================================================
**   Synopsis:    SHORT   Cas_MigrCopyTable()
*
*       Input:    USHORT  offusSrcTable
*                 USHORT  offusObjTable
*                 USHORT  usSize
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:  CAS_PERFORM
*          Abnormal End:  CAS_NO_MAKE_FILE
*
**  Description:
*               Migration Cashier File Copy from Old file to New file
*==========================================================================
*/
SHORT Cas_MigrCopyTable(USHORT  offusSrcTable, USHORT  offusObjTable,USHORT  usSize)
{
    UCHAR   auchWorkBuffer[CAS_MAX_WORK_BUFFER];

    while (usSize > CAS_MAX_WORK_BUFFER) {
        /* Read File */
        Cas_ReadFile(offusSrcTable, auchWorkBuffer, CAS_MAX_WORK_BUFFER);
        /* Write File */
        Cas_MigrNewWrite(offusObjTable, auchWorkBuffer, CAS_MAX_WORK_BUFFER);
        usSize        -= CAS_MAX_WORK_BUFFER;
        offusSrcTable += CAS_MAX_WORK_BUFFER;
        offusObjTable += CAS_MAX_WORK_BUFFER;
    }

    if (usSize != 0) {
        Cas_ReadFile(offusSrcTable, auchWorkBuffer, usSize);
        /* Write File */
        Cas_MigrNewWrite(offusObjTable, auchWorkBuffer, usSize);
    }
    
    return(TRUE);
}

/*
*==========================================================================
**   Synopsis:    VOID   Cas_MigrNewWrite()
*
*       Input:    offulFileSeek      * offset from file head *
*                 *pTableHeadAddress * start address  *
*                 usSizeofWrite      * size of for writing into file *
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:  CAS_PERFORM
*          Abnormal End:  CAS_NO_MAKE_FILE
*
**  Description:
*               Write Data from Old Cashier file to New Cashier file
*==========================================================================
*/
VOID    Cas_MigrNewWrite(ULONG offulFileSeek, 
                         VOID  *pTableHeadAddress, 
                         ULONG usSizeofWrite)
{
    ULONG   ulActualPosition;   /* DUMMY FOR PifSeekFile */

    if (!usSizeofWrite) {
        return;
    }

    if ((PifSeekFile(hsCashierNewFileHandle, offulFileSeek, &ulActualPosition)) < 0 ) {
        PifAbort(MODULE_CASHIER, FAULT_ERROR_FILE_SEEK);
    }
        
    PifWriteFile(hsCashierNewFileHandle, pTableHeadAddress, usSizeofWrite);
}

/*
*==========================================================================
**   Synopsis:    SHORT   Cas_ChangeIndexTable()
*
*       Input:    USHORT  offusSrcTable
*                 USHORT  offusObjTable
*                 USHORT  usSize
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:  CAS_PERFORM
*          Abnormal End:  CAS_NO_MAKE_FILE
*
**  Description:
*               Change a CAS_INDEX Table offusSrcTable to offusObjTable by usSize.
*==========================================================================
*/
SHORT   Cas_ChangeIndexTable(USHORT offusSrcTable, USHORT offusObjTable, USHORT usSize)
{
    UCHAR   uchWorkBuffer;
    USHORT  usoffset;
    UCHAR   auchOldEntry[CAS_INDEXENTRY_NUM] = {1,          /* usCashierNo LOWBIT */
                                                1,          /* usCashierNo HIBIT  */
                                                1,          /* usParaBlock LOWBIT */
                                                0};         /* usParaBlock HIBIT  */

    /* read area clear */
    memset (&uchWorkBuffer, 0x00, sizeof(uchWorkBuffer));
    while(usSize > 0) {
        /* change ENTRY */
        for (usoffset = 0; usoffset < (USHORT)CAS_INDEXENTRY_NUM; usoffset++) {
            if (auchOldEntry[usoffset]) {   /* copy */
                Cas_ReadFile(offusSrcTable, &uchWorkBuffer, sizeof(UCHAR));
                Cas_MigrNewWrite(offusObjTable, &uchWorkBuffer, sizeof(UCHAR));
                usSize        -= sizeof(UCHAR);
                offusSrcTable += sizeof(UCHAR);
                offusObjTable += sizeof(UCHAR);
            } else {                        /* default */
                memset (&uchWorkBuffer, 0x00, sizeof(uchWorkBuffer));
                Cas_MigrNewWrite(offusObjTable, &uchWorkBuffer, sizeof(UCHAR));
                offusObjTable += sizeof(UCHAR);
            }
        }
    }

    return(CAS_PERFORM);
}

/*
*==========================================================================
**   Synopsis:    SHORT   Cas_ChangeParaTable()
*
*       Input:    USHORT  offusSrcTable
*                 USHORT  offusObjTable
*                 USHORT  usSize
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:  CAS_PERFORM
*          Abnormal End:  CAS_NO_MAKE_FILE
*
**  Description:
*               Change a CAS_PARAENTRY Table offusSrcTable to offusObjTable by usSize.
*==========================================================================
*/
SHORT   Cas_ChangeParaTable(USHORT offusSrcTable, USHORT offusObjTable, USHORT usSize)
{
    UCHAR   uchWorkBuffer;
    USHORT  usoffset;
    UCHAR   auchOldEntry[CAS_PARAENTRY_NUM] = {
                                             1,0,       /* fbCashierStatus[2]      */
                                               0,       /* usGstCheckStartNo LOBIT */
                                               0,       /* usGstCheckStartNo HIBIT */
                                               0,       /* usGstCheckEndNo   LOBIT */
                                               0,       /* usGstCheckEndNo   HIBIT */
                                               0,       /* uchChgTipRate           */
                                               0,       /* uchTeamNo               */
                                               0,       /* uchTerminal             */
         1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,       /* auchCashierName[20]     */
                                               1};      /* uchCashierSecret        */

    /* read area clear */
    memset(&uchWorkBuffer, 0x00, sizeof(uchWorkBuffer));
    while (usSize > 0) {
        /* change ENTRY */
        for (usoffset = 0; usoffset < CAS_PARAENTRY_NUM; usoffset++) {
            if (!(auchOldEntry[usoffset])) {      /* default (new parameter) */
                memset(&uchWorkBuffer, 0x00, sizeof(uchWorkBuffer));
                Cas_MigrNewWrite(offusObjTable, &uchWorkBuffer, sizeof(UCHAR));
                offusObjTable += sizeof(UCHAR);
            } else {                                        /* copy old parameter */
                Cas_ReadFile(offusSrcTable, &uchWorkBuffer, sizeof(UCHAR));
                /* migration CashierStatus Training bit1 -> bit3 */
                if ((usoffset == CAS_OFFSET_TRAINING) && (uchWorkBuffer & CAS_OLDTRAINING_BIT)) {
                        uchWorkBuffer &= ~CAS_OLDTRAINING_BIT;  /* Clear */
                        uchWorkBuffer |= CAS_NEWTRAINING_BIT;   /* New Status Write */
                }

                Cas_MigrNewWrite(offusObjTable, &uchWorkBuffer, sizeof(UCHAR));

                usSize        -= sizeof(UCHAR);
                offusSrcTable += sizeof(UCHAR);
                offusObjTable += sizeof(UCHAR);
            }
        }
    }

    return(CAS_PERFORM);
}


/*
*==========================================================================
**    Synopsis:    SHORT CasDeleteFile()
*
*       Input:    USHORT  usNumberOfCashier - Number of Cashier.
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:  CAS_PERFORM
*
**  Description:
*               Delete Cashier File.
*==========================================================================
*/
SHORT CasDeleteFile()
{
    if (hsCashierFileHandle) {  /* File exist */
        PifCloseFile(hsCashierFileHandle);
        PifDeleteFile(auchCAS_CASHIER);
    }

    return(CAS_PERFORM);
}
/*
*==========================================================================
**    Synopsis:    SHORT   CasMigrateHGV10File(VOID)
*
*       Input: Nothing
*      Output: Nothing
*       InOut: Nothing
*
**  Return    :
*     Normal End:  CAS_PERFORM
*   Abnormal End:  CAS_NO_MAKE_FILE
*
**  Description: Reads from the Temporary cashier file, and then adjusts each record
*					(UCHAR to WCHAR for auchCashierName,
*					 USHORT to ULONG for ulCashierSecret) and saves it into the new file.
*==========================================================================
*/
SHORT   CasMigrateHGV10File(VOID)
{
	USHORT			usBytes = 0;
	int				i , j;
	CAS_PARAENTRY   Arg;
    SHORT           sStatus;
	CAS_INDEXENTRY_HGV210  aWorkRcvBuffer[CAS_NUMBER_OF_MAX_CASHIER];   /* index table */
    CAS_FILEHED     CasFileHedOld, CasFileHedNew;
	CASIF_HGV10		tempCashier;
	USHORT			usTempParaBlock,unCur = 0;


    /* get File handle */
    if ((hsCashierNewFileHandle = PifOpenFile(auchCAS_CASHIEROLD, auchOLD_FILE_READ_WRITE)) == PIF_ERROR_FILE_EXIST) {
        PifReleaseSem(husCashierSem);
        return(CAS_FILE_NOT_FOUND);
    }
    
    /* Read Header */
    if ((PifReadFile(hsCashierNewFileHandle, &CasFileHedOld, sizeof(CasFileHedOld))) != sizeof(CasFileHedOld)) {
        PifCloseFile(hsCashierFileHandle);
        PifReleaseSem(husCashierSem);
        return(CAS_FILE_NOT_FOUND);
    }

	PifReadFile(hsCashierNewFileHandle, &aWorkRcvBuffer, (CasFileHedOld.usNumberOfResidentCashier * sizeof(CAS_INDEXENTRY)));

	//Get Index Information from the File
/*	Cas_ReadFile((ULONG)CasFileHedOld.offusOffsetOfIndexTbl, 
                 aWorkRcvBuffer,
                 (USHORT)(CasFileHedOld.usNumberOfResidentCashier * sizeof(CAS_INDEXENTRY)));*/
	
	CasCreatFile(CasFileHedOld.usNumberOfMaxCashier);

	//PifRequestSem(husCashierSem);
	hsCashierFileHandle = PifOpenFile(auchCAS_CASHIER, auchOLD_FILE_READ_WRITE);

	//PifWriteFile(hsCashierFileHandle, &CasFileHedOld, sizeof(CasFileHedOld));
	

    /* Check new file size */
    PifReadFile(hsCashierFileHandle, &CasFileHedNew, sizeof(CasFileHedOld));

	//Goes through the Index and reads from the old file, then assigns it to the new
	//File with the appropriate changes being made JHHJ
	for(; unCur < (CasFileHedOld.usNumberOfResidentCashier); unCur++)
	{
		tempCashier.usCashierNo = aWorkRcvBuffer[unCur].usCashierNo;
		CasIndReadHGV10(&tempCashier, aWorkRcvBuffer[unCur].usParaBlock, &CasFileHedOld);

		if (Cas_Index(&CasFileHedNew, CAS_REGIST, aWorkRcvBuffer[unCur].usCashierNo, &usTempParaBlock) == CAS_CASHIER_FULL) {
           Cas_CloseFileReleaseSem();
           return(CAS_CASHIER_FULL);   /* return if assign cashier full */}

		{
			WCHAR *cpyWchar = Arg.auchCashierName;
			UCHAR *cpyUchar = tempCashier.auchCashierName;
			for (i = 0, j = 1; i < sizeof(tempCashier.auchCashierName); i++, j++) {
				*cpyWchar = (WCHAR) *cpyUchar;
				cpyWchar++;
				cpyUchar++;

			/*adds a null character at the end of the mnemonic, because a full size
			//mnemonic will still need a null at the end
				if (j % 20 == 0)
			{
				*cpyWchar = 0x00;
			}*/
			}		
		}

		memcpy(Arg.fbCashierStatus, tempCashier.fbCashierStatus, sizeof(Arg.fbCashierStatus));

		Arg.ulCashierSecret   = (ULONG) tempCashier.uchCashierSecret;	//update for version 212

		Arg.usGstCheckStartNo = tempCashier.usGstCheckStartNo;  /* V3.3 */
		Arg.usGstCheckEndNo   = tempCashier.usGstCheckEndNo;    /* V3.3 */
		Arg.uchChgTipRate     = tempCashier.uchChgTipRate;      /* V3.3 */
		Arg.uchTeamNo         = tempCashier.uchTeamNo;          /* V3.3 */
		Arg.uchTerminal       = tempCashier.uchTerminal;        /* V3.3 */
		Cas_ParaWrite(&CasFileHedNew, usTempParaBlock, &Arg);
		Cas_ChangeFlag(usTempParaBlock, CAS_RESET_STATUS, 0xff);
	}

    sStatus = CAS_PERFORM;

    PifCloseFile(hsCashierFileHandle);
    PifCloseFile(hsCashierNewFileHandle);
    return(sStatus);

}
/*
*==========================================================================
**    Synopsis:    SHORT   CasMigrateHGV203File(VOID)
*
*       Input: Nothing
*      Output: Nothing
*       InOut: Nothing
*
**  Return    :
*     Normal End:  CAS_PERFORM
*   Abnormal End:  CAS_NO_MAKE_FILE
*
**  Description: Reads from the Temporary cashier file, and then adjusts each record
*				 (UCHAR to WCHAR) and saves it into the new file.
*==========================================================================
*/
SHORT   CasMigrateHGV203File(VOID)
{
	USHORT			usBytes = 0;
	int				i , j;
	CAS_PARAENTRY   Arg;
    SHORT           sStatus;
	CAS_INDEXENTRY_HGV210  aWorkRcvBuffer[CAS_NUMBER_OF_MAX_CASHIER];   /* index table */
    CAS_FILEHED     CasFileHedOld, CasFileHedNew;
	CASIF_HGV10		tempCashier;
	USHORT			usTempParaBlock,unCur = 0;

    /* get File handle */
    if ((hsCashierNewFileHandle = PifOpenFile(auchCAS_CASHIEROLD, auchOLD_FILE_READ_WRITE)) == PIF_ERROR_FILE_EXIST) {
        PifReleaseSem(husCashierSem);
        return(CAS_FILE_NOT_FOUND);
    }
    
    /* Read Header */
    if ((PifReadFile(hsCashierNewFileHandle, &CasFileHedOld, sizeof(CasFileHedOld))) != sizeof(CasFileHedOld)) {
        PifCloseFile(hsCashierFileHandle);
        PifReleaseSem(husCashierSem);
        return(CAS_FILE_NOT_FOUND);
    }

	PifReadFile(hsCashierNewFileHandle, &aWorkRcvBuffer, (CasFileHedOld.usNumberOfResidentCashier * sizeof(CAS_INDEXENTRY)));

	//Get Index Information from the File
/*	Cas_ReadFile((ULONG)CasFileHedOld.offusOffsetOfIndexTbl, 
                 aWorkRcvBuffer,
                 (USHORT)(CasFileHedOld.usNumberOfResidentCashier * sizeof(CAS_INDEXENTRY)));*/

	CasCreatFile(CasFileHedOld.usNumberOfMaxCashier);

	//PifRequestSem(husCashierSem);
	hsCashierFileHandle = PifOpenFile(auchCAS_CASHIER, auchOLD_FILE_READ_WRITE);

	//PifWriteFile(hsCashierFileHandle, &CasFileHedOld, sizeof(CasFileHedOld));
	
    /* Check new file size */
    PifReadFile(hsCashierFileHandle, &CasFileHedNew, sizeof(CasFileHedOld));

	//Goes through the Index and reads from the old file, then assigns it to the new
	//File with the appropriate changes being made JHHJ
	for(; unCur < (CasFileHedOld.usNumberOfResidentCashier); unCur++)
	{
		tempCashier.usCashierNo = aWorkRcvBuffer[unCur].usCashierNo;
		CasIndReadHGV10(&tempCashier, aWorkRcvBuffer[unCur].usParaBlock, &CasFileHedOld);

		if (Cas_Index(&CasFileHedNew, CAS_REGIST, aWorkRcvBuffer[unCur].usCashierNo, &usTempParaBlock) == CAS_CASHIER_FULL) {
           Cas_CloseFileReleaseSem();
           return(CAS_CASHIER_FULL);   /* return if assign cashier full */}

			{
				WCHAR *cpyWchar = Arg.auchCashierName;
				UCHAR *cpyUchar = tempCashier.auchCashierName;
				for (i = 0, j = 1; i < sizeof(tempCashier.auchCashierName); i++, j++) {
					*cpyWchar = (WCHAR) *cpyUchar;
					cpyWchar++;
					cpyUchar++;

				/*adds a null character at the end of the mnemonic, because a full size
				//mnemonic will still need a null at the end
					if (j % 20 == 0)
				{
					*cpyWchar = 0x00;
				}*/
				}		
			}
			memcpy(Arg.fbCashierStatus, tempCashier.fbCashierStatus, sizeof(Arg.fbCashierStatus));
			
			Arg.ulCashierSecret   = (ULONG) tempCashier.uchCashierSecret;	//update for version 212

			Arg.usGstCheckStartNo = tempCashier.usGstCheckStartNo;  /* V3.3 */
			Arg.usGstCheckEndNo   = tempCashier.usGstCheckEndNo;    /* V3.3 */
			Arg.uchChgTipRate     = tempCashier.uchChgTipRate;      /* V3.3 */
			Arg.uchTeamNo         = tempCashier.uchTeamNo;          /* V3.3 */
			Arg.uchTerminal       = tempCashier.uchTerminal;        /* V3.3 */
			Cas_ParaWrite(&CasFileHedNew, usTempParaBlock, &Arg);
			Cas_ChangeFlag(usTempParaBlock, CAS_RESET_STATUS, 0xff);
	}

    sStatus = CAS_PERFORM;

    PifCloseFile(hsCashierFileHandle);
    PifCloseFile(hsCashierNewFileHandle);
    return(sStatus);

}

/*
*==========================================================================
**    Synopsis:   SHORT CasIndRead(CASIF *pCasif)
*
*       Input:    pCasif->usCashierNo     
*      Output:    pCasif->ucharCashierStatus
*                 pCasif->auchCashierName
*                 pCasif->uchCashierSecret
*       InOut:
*
**  Return    :
*     Normal End: CAS_PERFORM
*   Abnormal End: CAS_NOT_IN_FILE
*                 CAS_FILE_NOT_FOUND
*
**  Description: Reads the file from the temporary file, and sets it to a
*				 temporary variable to be saved later.
*==========================================================================
*/
SHORT CasIndReadHGV10(CASIF_HGV10 *pCasif, USHORT usParaBlockNo, CAS_FILEHED *CasFileHed)
{
    SHORT           sStatus;          /* error status save area */
    CAS_PARAENTRY_HGV10   CasParaEntry;     /* paramater entry table save area */
    //CAS_FILEHED     CasFileHed;       /* file header save area */
	ULONG   ulActualPosition;  /* for PifSeekFile */
	USHORT		holder;

		holder = usParaBlockNo;

		if (PifSeekFile(hsCashierNewFileHandle, CasFileHed->offusOffsetOfParaTbl + (sizeof(CAS_PARAENTRY_HGV10) * (usParaBlockNo - 1)), &ulActualPosition) < 0 ) {
			PifAbort(MODULE_CASHIER, FAULT_ERROR_FILE_SEEK);
		}

		if (PifReadFile(hsCashierNewFileHandle, &CasParaEntry, sizeof(CAS_PARAENTRY_HGV10)) != sizeof(CAS_PARAENTRY_HGV10)) {
			PifAbort(MODULE_CASHIER, FAULT_ERROR_FILE_READ);
		}

        //Cas_ParaRead(&CasFileHed, usParaBlockNo, &CasParaEntry);
/*        pCasif->fbCashierStatus  = CasParaEntry.fbCashierStatus; */
        memcpy(pCasif->fbCashierStatus, CasParaEntry.fbCashierStatus, sizeof(pCasif->fbCashierStatus));

        memcpy(pCasif->auchCashierName, CasParaEntry.auchCashierName, CAS_NUMBER_OF_CASHIER_NAME);
        pCasif->uchCashierSecret = CasParaEntry.uchCashierSecret;
        pCasif->usGstCheckStartNo = CasParaEntry.usGstCheckStartNo; /* V3.3 */
        pCasif->usGstCheckEndNo   = CasParaEntry.usGstCheckEndNo;   /* V3.3 */
        pCasif->uchChgTipRate     = CasParaEntry.uchChgTipRate;     /* V3.3 */
        pCasif->uchTeamNo         = CasParaEntry.uchTeamNo;         /* V3.3 */
        pCasif->uchTerminal       = CasParaEntry.uchTerminal;       /* V3.3 */
        sStatus = CAS_PERFORM;

    //Cas_CloseFileReleaseSem();          /* close file and release semaphore */
    return(sStatus);
}
/*====== End of Source ========*/
