/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO   ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1996      ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server ETK module, Program List                        
*   Category           : Client/Server ETK module, NCR2170 US HOSPITALITY APPLICATION
*   Program Name       : CSETK.C
*  ------------------------------------------------------------------------
*    Abstract           :The provided function names are as follows.
*
*
*   INTERNAL FUNCTION
*                            
*    Etk_GetHeader();         * Get File Header *
*    Etk_PutHeader();         * Put File Header *
*    Etk_Index()              * Search index file and set and modifi *
*    Etk_IndexDel()           * Search index file and delete *
*    Etk_TimeFieldRead()      * Read Current time field *
*    Etk_TimeFieldWrite()     * Set Current Time Field *
*    Etk_EmpBlkGet();         * Get Empty Block No *
*    Etk_BlkEmp();            * Set Empty Block No *
*    Etk_CompIndex();         * Compear Index file *
*    Etk_WriteFile()          * Write data into File *
*    Etk_ReadFile()           * Read data into file *
*    Etk_CloseFileReleaseSem(); * Close file and release semaphore *
*    EtkMakeHeader();         * make file header *
*    EtkEditHeader();         * Edit file header *
*    Etk_ClearStatus();       * Clear status in index record *
*    Etk_CheckAllowJob();     * Check Allow job code *
*    Etk_AllowJobRead();      * Read job allow data *
*    Etk_IndexWrite();        * Write Index data to Index Table *
*    Etk_MakeParaFile();      * Make ETK PARAMATER File *
*    Etk_MakeTotalFile();     * Make ETK TOTAL File *
*
*
*  ------------------------------------------------------------------------
*    Update Histories  :
*                                 
*    Date      Ver.Rev :NAME       :Description
*    June-15-93:00.00.01:M.YAMAMOTO :Initial
*    Nov -09-96:03.01.00:T.Nakahata : R3.1 Initial
*                                       Increase No. of Employees (200 => 250)
*                                       Increase Employee No. ( 4 => 9digits)
*                                       Add Employee Name (Max. 16 Char.)
*    Jan -31-96:03.01.01:M.Ozawa    :Modify Etk_IndexPara(), Etk_IndexDelPara()
*                                    for stack size correction.
** NCR2171 **                                         
*    Sep-06-99:01.00.00:M.Teraki   :initial (for 2171)
*===========================================================================
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
*       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
*       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
*       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
*       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
*       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
*       2016  -> GenPOS Rel 2.3.0 (Windows 7, completely Out of Scope, Visual Studio 2013.)
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
*    moved from Visual Studio 2005 to Visual Studio 2013 with Rel 2.3.0
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
#include    <math.h>
#include    <stdio.h>
#include    <string.h>

#include    <ecr.h>
#include    <pif.h>
#include    <paraequ.h>
#include    <rfl.h>
#include    <csstbfcc.h>
#include    <csstbstb.h>
#include    <appllog.h>
#include    <csetk.h>
#include    "csetkin.h"

static SHORT   Etk_SearchFile(ULONG offulTableHeadAddress, 
                       USHORT usTableSize, 
                       ULONG  *poffulSearchPoint,
                       VOID   *pSearchData, 
                       USHORT usSearchDataSize, 
                       SHORT (*Comp)(VOID *pKey, VOID *pusCompKey));
static VOID   Etk_InsertTable( USHORT offusTableHeadAddress, 
                        USHORT usTableSize,
                        USHORT offusInsertPoint, 
                        VOID   *pInsertData, 
                        USHORT usInsertDataSize);
static VOID    Etk_DeleteTable(USHORT offusTableHeadAddress, 
                        USHORT usTableSize,
                        USHORT offusDeletePoint, 
                        USHORT usDeleteDataSize);

/*
*==========================================================================
**    Synopsis:    SHORT    Etk_CompIndex(ULONG *pulKey,
*                                         ETK_TOTALINDEX *pusHitPoint )
*
*       Input:    USHORT         *pusKey     Target key
*                 ETK_INDEX *pusHitPoint     Checked key
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :    0x00   : Match
*                < 0x00 : Target key > Checked key
*                > 0x00 : Target key < Checked key    
*
**  Description:
*               Compare routine.
*==========================================================================
*/
static SHORT    Etk_CompIndex(ULONG *pulKey, ETK_TOTALINDEX *pusHitPoint)
{
    if ((LONG)(*pulKey - pusHitPoint->ulEmployeeNo) > 0) {
        return(1);
    } else if ((LONG)(*pulKey - pusHitPoint->ulEmployeeNo) < 0) {
        return(-1);
    } else {
        return(0);
    }
/*    return((SHORT)(*pulKey - pusHitPoint->ulEmployeeNo)); */
}

/*
*==========================================================================
**    Synopsis:    SHORT    Etk_ParaCompIndex(ULONG *pulKey,
*                                             ETK_PARAINDEX *pusHitPoint )
*
*       Input:    ULONG          *pulKey          Target key
*                 ETK_PARAINDEX  *pusHitPoint     Checked key
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :    0x00   : Match
*                < 0x00 : Target key > Checked key
*                > 0x00 : Target key < Checked key    
*
**  Description:
*               Compare routine for Paramater Index Table .
*==========================================================================
*/
static SHORT    Etk_ParaCompIndex(ULONG *pulKey, ETK_PARAINDEX *pusHitPoint)
{
    if ((LONG)(*pulKey - pusHitPoint->ulEmployeeNo) > 0) {
        return(1);
    } else if ((LONG)(*pulKey - pusHitPoint->ulEmployeeNo) < 0) {
        return(-1);
    } else {
        return(0);
    }
/*    return((SHORT)(*pulKey - pusHitPoint->ulEmployeeNo)); */
}

/*
*==========================================================================
**  Synopsis:    SHORT   Etk_Index(ETK_HEAD EtkHead,
*                                  USHORT usIndexMode,
*                                  ULONG  ulEtkNo,
*                                  ETK_TOTALINDEX *pEtkTotalIndex,
*                                  USHORT *pusParaBlockNo,
*                                  ULONG  *pulIndexPosition)
*
*       Input:    ETK_FILEHEAD *pEtkFileHed   file header's address
*                 ULONG  ulEtkNo              Pointer to object to search for
*                 USHORT usIndexMode          
*                        
*      Output:    ETK_TOTALINDEX *pEtkTotalIndex,       Serched record data 
*                 USHORT *pusParaBlockNo,     Block No 
*                 ULONG  *pulIndexPosition)   Offset of Serched record data
*                                             from file head.
*
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    ETK_RESIDENT
*            Abnormal End:  ETK_FILE_NOT_FOUND
*                           ETK_NOT_IN_FILE
*                           ETK_FULL
*   
**  Description:
*                Serch and Set in Etk Total File.
*==========================================================================
*/
SHORT   Etk_Index(ETK_HEAD *pEtkHead,
                  USHORT usIndexMode,
                  ULONG  ulEtkNo,
                  ETK_TOTALINDEX *pEtkTotalIndex,
                  USHORT *pusParaBlockNo,
                  ULONG  *pulIndexPosition)
{
    ULONG   offulSearchPoint;
    SHORT   sStatus;                /* return status save area */

    /* read 1 entry from index table */    
    pEtkTotalIndex->ulEmployeeNo = ulEtkNo;
    pEtkTotalIndex->uchStatus = 0;
	pEtkTotalIndex->usBlockNo = 0;

    *pusParaBlockNo = 0;
    *pulIndexPosition = 0;

    if ( pEtkHead->usNumberOfEtk == 0 ) {
		// No employees in the table so the search point will be
		// the beginning of the table and our status is ETK_NOT_IN_FILE
		// because this employee is not in the Employee time keeping file.

        offulSearchPoint = pEtkHead->ulOffsetOfIndex;

		sStatus = ETK_NOT_IN_FILE;
    } else {
        sStatus = Etk_SearchFile(pEtkHead->ulOffsetOfIndex,
                                 (USHORT)(pEtkHead->usNumberOfEtk * sizeof(ETK_TOTALINDEX)),
                                 &offulSearchPoint, 
                                 &pEtkTotalIndex->ulEmployeeNo,
                                 sizeof(ETK_TOTALINDEX),
                                 Etk_CompIndex );
    }

    if ((sStatus == ETK_NOT_IN_FILE) && (usIndexMode == ETK_SEARCH_ONLY)) {
        return (ETK_NOT_IN_FILE);
    }

    if (sStatus == ETK_SUCCESS) {
		/* ETK record exists so read the information */
        Etk_ReadFile(hsEtkTotalFile, offulSearchPoint, pEtkTotalIndex, sizeof( ETK_TOTALINDEX ));
    } else {
		/* ETK record not found so create a new one since we are not doing just search */

		/* Check Paramater Table full and get paramater No. */
		if (Etk_EmpBlkGet(pEtkHead, &pEtkTotalIndex->usBlockNo) == ETK_FULL) {
			return(ETK_FULL);
		}

		/* make space 1 record in index table */
		Etk_InsertTable((USHORT)pEtkHead->ulOffsetOfIndex,
						(USHORT)(pEtkHead->usNumberOfEtk *  sizeof(ETK_TOTALINDEX)),
						(USHORT)(offulSearchPoint - (USHORT)pEtkHead->ulOffsetOfIndex),
						pEtkTotalIndex,
						sizeof(ETK_TOTALINDEX));
		pEtkHead->usNumberOfEtk++;
	}

    *pusParaBlockNo = pEtkTotalIndex->usBlockNo;
    *pulIndexPosition = offulSearchPoint;
    return (ETK_RESIDENT);
}

/*
*==========================================================================
**    Synopsis:    SHORT   Etk_IndexDel( ETK_HEAD   *pEtkHead,
*                                        ULONG      ulEtkNo )
*
*       Input:    ETK_HEAD  *pEtkHead,
*                 ULONG     ulEtkNo
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    ETK_SUCCESS
*          Abnormal End:    ETK_NOT_IN_FILE
*
**  Description:
*                Delete inputted Etk from current index table in Total.
*==========================================================================
*/
SHORT   Etk_IndexDel( ETK_TOTALHEAD  *pEtkHead,
                      ULONG     ulEtkNo )
{
    ETK_TOTALINDEX EtkIndexEntry = {0};       /* index table entry poiter */
    ULONG          offulSearchPoint;

    /* get index table to work buffer */
    if ( pEtkHead->EtkCur.usNumberOfEtk == 0 ) {
        return(ETK_NOT_IN_FILE);
    }

    EtkIndexEntry.ulEmployeeNo = ulEtkNo;
    EtkIndexEntry.uchStatus = 0;

    if ( Etk_SearchFile(pEtkHead->EtkCur.ulOffsetOfIndex,
                        (USHORT)(pEtkHead->EtkCur.usNumberOfEtk * sizeof( ETK_TOTALINDEX )),
                        &offulSearchPoint, 
                        &EtkIndexEntry.ulEmployeeNo,
                        sizeof(ETK_TOTALINDEX),
                        Etk_CompIndex ) == ETK_NOT_IN_FILE) {
        return (ETK_NOT_IN_FILE);
    }

    /* Get Paramater Block No */
    Etk_ReadFile(hsEtkTotalFile, offulSearchPoint, &EtkIndexEntry, sizeof( ETK_TOTALINDEX ));

    /* Delete space 1 record in index table */
    Etk_DeleteTable((USHORT)pEtkHead->EtkCur.ulOffsetOfIndex,
                    (USHORT)(pEtkHead->EtkCur.usNumberOfEtk * sizeof(ETK_TOTALINDEX)),
                    (USHORT)(offulSearchPoint - (USHORT)pEtkHead->EtkCur.ulOffsetOfIndex), 
                    sizeof(ETK_TOTALINDEX));

    /* Empty para empty table */
    Etk_BlkEmp(&pEtkHead->EtkCur, EtkIndexEntry.usBlockNo);

    /* ResidentETK -1 */
    if (pEtkHead->EtkCur.usNumberOfEtk > 0) {
		pEtkHead->EtkCur.usNumberOfEtk--;
	}

    /* Save Index Table */
    Etk_WriteFile(hsEtkTotalFile, ETK_FILE_HED_POSITION, pEtkHead, sizeof(ETK_TOTALHEAD));

    return(ETK_SUCCESS);
}

/*
*==========================================================================
**    Synopsis:   SHORT   Etk_SearchFile(USHORT offusTableHeadAddress, 
*                                        USHORT usTableSize, 
*                                        USHORT *poffusSearchPoint,
*                                        VOID   *pSearchData, 
*                                        USHORT usSearchDataSize, 
*                                        SHORT (*Comp)(VOID *pKey, 
*                                                      VOID *pusCompKey))
*
*       Input:    offulTableHeadAddress
*                 ulTableSize
*                 *pSearchPoint
*                 *pSearchData
*                 usSearchDataSize
*                 (*Comp)(VOID *A, VOID *B)   A - B
*               
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   ETK_SUCCESS
*                 ETK_NOT_IN_FILE
*                 (file error is system halt)
*
**  Description:  Search data on the Table in a File.
*==========================================================================
*/
static SHORT   Etk_SearchFile(ULONG offulTableHeadAddress, 
                       USHORT usTableSize, 
                       ULONG  *poffulSearchPoint,
                       VOID   *pSearchData, 
                       USHORT usSearchDataSize, 
                       SHORT (*Comp)(VOID *pKey, VOID *pusCompKey))
{
    SHORT sStatus;
    UCHAR auchKeyData[10];
    UCHAR auchWorkBuffer[ETK_MAX_WORK_BUFFER];
    SHORT sCompStatus;
    ULONG  offulTableTailAddress;
    USHORT offusTableSize;
    USHORT *pusHitPoint;
    USHORT usNumberOfEntry;             /* number of entry */

    sStatus = 0;

	if (hsEtkTotalFile < 0) {
		// if the file handle is bad then lets log it and return something that may be useful.
		NHPOS_ASSERT_TEXT((hsEtkTotalFile >= 0), "==ERROR: Etk_SearchFile() bad file handle.");
		return (ETK_NOT_IN_FILE);
	}

    /* make Table Tail address */
    offulTableTailAddress = offulTableHeadAddress + usTableSize;
    offusTableSize = usTableSize;

    /* Search key in the file */
    while ( offusTableSize > ETK_MAX_WORK_BUFFER ) {    
        usNumberOfEntry = (offusTableSize / usSearchDataSize) / 2;
        *poffulSearchPoint = (usNumberOfEntry * usSearchDataSize) + offulTableHeadAddress;
        Etk_ReadFile( hsEtkTotalFile, *poffulSearchPoint, auchKeyData, usSearchDataSize);
        if ((sCompStatus = (*Comp)(pSearchData, auchKeyData)) == 0) {
            return(ETK_SUCCESS);
            break;
        } else if (sCompStatus > 0) {   /* pSearchData > auchKeyData */
            offulTableHeadAddress = *poffulSearchPoint;
            offulTableHeadAddress += usSearchDataSize;
        } else {
            offulTableTailAddress = *poffulSearchPoint;
        }
        offusTableSize = offulTableTailAddress - offulTableHeadAddress;
    }

    Etk_ReadFile( hsEtkTotalFile, offulTableHeadAddress, auchWorkBuffer, offusTableSize);   /* offusTableSize != 0 */

    /* Search key in a table from the file  */
    usNumberOfEntry = offusTableSize / usSearchDataSize;

    sStatus = Rfl_SpBsearch(pSearchData, 
                            usSearchDataSize, 
                            auchWorkBuffer, 
                            usNumberOfEntry,
                            &pusHitPoint, 
                            Comp);

    *poffulSearchPoint = (UCHAR *)pusHitPoint - auchWorkBuffer;
    *poffulSearchPoint += offulTableHeadAddress; /* offset in the file */
    if (sStatus == RFL_HIT) {
        return (ETK_SUCCESS);
    }
    return (ETK_NOT_IN_FILE);
}

/*
*==========================================================================
**    Synopsis:   VOID   ERtk_InsertTable(USHORT offusTableHeadAddress, 
*                                         USHORT usTableSize
*                                         USHORT offusInsertPoint, 
*                                         VOID   *pInsertData, 
*                                         USHORT usInsertDataSize)
*
*       Input:   offusTableHeadAddress  Table head address in a file.
*                usTableSize            Table size in a file.
*                offInsertPoint         Table Insert Point (offset from tabel head address) in a file.
*                *pInsertData           Inserted Data
*                usInsertDataSize       Insert Data size
*               
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*                 
*
**  Description: Insert data to Table in a File.
*==========================================================================
*/
static VOID   Etk_InsertTable( USHORT offusTableHeadAddress, 
                        USHORT usTableSize,
                        USHORT offusInsertPoint, 
                        VOID   *pInsertData, 
                        USHORT usInsertDataSize)
{
    UCHAR   auchWorkBuffer[ETK_MAX_WORK_BUFFER];    /* work buffer */
    USHORT  offusTailInFile;    /* Table Tail Address */
    USHORT  usMoveByte;         /* moving byte save area */

    if (usTableSize <= offusInsertPoint) {
        Etk_WriteFile( hsEtkTotalFile, offusInsertPoint + offusTableHeadAddress, pInsertData, usInsertDataSize );  /* insert data */
        return;
    }

    /* Calculate moving byte */
    usMoveByte = usTableSize - offusInsertPoint;
    offusTailInFile = offusTableHeadAddress + usTableSize;

    /* Move moving byte in a file */
    while ( usMoveByte > ETK_MAX_WORK_BUFFER ) {
        offusTailInFile -= ETK_MAX_WORK_BUFFER;
        Etk_ReadFile( hsEtkTotalFile, offusTailInFile, auchWorkBuffer, ETK_MAX_WORK_BUFFER);
        Etk_WriteFile( hsEtkTotalFile, offusTailInFile + usInsertDataSize, auchWorkBuffer, ETK_MAX_WORK_BUFFER);
        usMoveByte -= ETK_MAX_WORK_BUFFER;
    }

    if (usMoveByte != 0) {
        Etk_ReadFile( hsEtkTotalFile, offusTableHeadAddress + offusInsertPoint, auchWorkBuffer, usMoveByte);
        Etk_WriteFile( hsEtkTotalFile, (offusTableHeadAddress + offusInsertPoint + usInsertDataSize), auchWorkBuffer, usMoveByte);
    }

    /* insert data */

    Etk_WriteFile( hsEtkTotalFile, offusTableHeadAddress + offusInsertPoint, pInsertData, usInsertDataSize );
}

/*
*==========================================================================
**    Synopsis:   VOID   Etk_DeleteTable(USHOT  offusTableHeadAddress, 
*                                        USHORT usTableSize
*                                        USHORT offusDeletePoint, 
*                                        USHORT usDeleteDataSize)
*                                        
*
*       Input:    offusTableHeadAddress     Table head address in a file.
*                 usTableSize               Table size in a file.
*                 offusDeletePoint          Point 
*                 usDeleteDataSize          Data size
*                   
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*                 (file error is system halt)
*
**  Description:
*               Delete data from Index Table in a File.
*==========================================================================
*/
static VOID    Etk_DeleteTable(USHORT offusTableHeadAddress, 
                        USHORT usTableSize,
                        USHORT offusDeletePoint, 
                        USHORT usDeleteDataSize)
{
    UCHAR   auchWorkBuffer[ETK_MAX_WORK_BUFFER];  /* work buffer */
    USHORT  usMoveByte;     /* moving byte save area */
    
    /* Calculate moving byte */
    usMoveByte = usTableSize - offusDeletePoint - usDeleteDataSize;

    /* Move moving byte in a file */
    offusDeletePoint += offusTableHeadAddress;

    while ( usMoveByte > ETK_MAX_WORK_BUFFER ) {
        Etk_ReadFile( hsEtkTotalFile, offusDeletePoint + usDeleteDataSize, auchWorkBuffer, ETK_MAX_WORK_BUFFER);
        Etk_WriteFile( hsEtkTotalFile, offusDeletePoint, auchWorkBuffer, ETK_MAX_WORK_BUFFER);
        usMoveByte -= ETK_MAX_WORK_BUFFER;
        offusDeletePoint += ETK_MAX_WORK_BUFFER;
    }

    if (usMoveByte != 0) {
        Etk_ReadFile( hsEtkTotalFile, offusDeletePoint + usDeleteDataSize, auchWorkBuffer, usMoveByte);
        Etk_WriteFile( hsEtkTotalFile, offusDeletePoint, auchWorkBuffer, usMoveByte);
    }
}

/*
*==========================================================================
**    Synopsis:    VOID    Etk_TimeFieldRead(ETK_HEAD *pEtkHead,
*                                            ETK_TOTALINDEX *pEtkTotalIndex,
*                                            USHORT usParaBlockNo,
*                                            ETK_FIELD_IN_FILE *pEtkField)
*
*       Input:    ETK_HEAD *pEtkHead
*                 ETK_TOTALINDEX *pEtkTotalIndex
*                 UCHAR usParaBlockNo
*                 ETK_FIELD_IN_FILE *pEtkField
*
*      Output:    ETK_PARAENTRY *pArg
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description:
*                Read Etk Paramater.
*==========================================================================
*/
VOID    Etk_TimeFieldRead(ETK_HEAD *pEtkHead,
                          ETK_TOTALINDEX *pEtkEntry,
                          USHORT usParaBlockNo,
                          ETK_FIELD_IN_FILE *pEtkField)
{
    ULONG   ulActualPosition;  /* for PifSeekFile */
	ULONG	ulActualBytesRead; //For PifReadFile 11-7-3 RPH

    if (PifSeekFile(hsEtkTotalFile, pEtkHead->ulOffsetOfPara
        + (sizeof(ETK_FIELD_IN_FILE) * ETK_MAX_FIELD * (usParaBlockNo - 1))
        + ((pEtkEntry->uchStatus & ETK_MASK_FIELD_POINTER) *
        sizeof(ETK_FIELD_IN_FILE)),
        &ulActualPosition) < 0 ) {
		NHPOS_ASSERT(!"Etk_TimeFieldRead PifSeekFile < 0");
        PifAbort(MODULE_ETK, FAULT_ERROR_FILE_SEEK);
    }
	
	//RPH 11-7-3 Changes for PifReadFile
	PifReadFile(hsEtkTotalFile, pEtkField, sizeof(ETK_FIELD_IN_FILE), &ulActualBytesRead);
    if ( ulActualBytesRead != sizeof(ETK_FIELD_IN_FILE)) {
 		NHPOS_ASSERT(!"Etk_TimeFieldRead PifReadFile < sizeof(ETK_FIELD_IN_FILE)");
       PifAbort(MODULE_ETK, FAULT_ERROR_FILE_READ);
    }
    
}

/*
*==========================================================================
**    Synopsis:    VOID    Etk_TimeFieldWrite(ETK_HEAD *pEtkHead,
*                                             ETK_TOTALINDEX *EtkEntry,
*                                             UCHAR usParaBlockNo,
*                                             ETK_FIELD_IN_FILE *pEtkField)
*
*       Input:    ETK_HEAD *pEtkFileHed
*                 ETK_TOTALINDEX *pTotalIndex
*                 UCHAR usParaBlockNo
*                 ETK_FIELD_IN_FILE *pEtkFieldInFile
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*                Nothing
**  Description:
*                Write Etk Paramater.
*==========================================================================
*/
VOID    Etk_TimeFieldWrite(ETK_HEAD *pEtkHead,
                           ETK_TOTALINDEX *pEtkEntry,
                           USHORT usParaBlockNo,
                           ETK_FIELD_IN_FILE *pEtkField)
{
    ULONG   ulActualPosition;   /* for PifEtkSeekFile */

    if (PifSeekFile(hsEtkTotalFile, pEtkHead->ulOffsetOfPara
        + (sizeof(ETK_FIELD_IN_FILE) * ETK_MAX_FIELD * (usParaBlockNo - 1))
        + ((pEtkEntry->uchStatus & ETK_MASK_FIELD_POINTER) *
        sizeof(ETK_FIELD_IN_FILE)),
        &ulActualPosition) < 0 ) {
 		NHPOS_ASSERT(!"Etk_TimeFieldWrite PifSeekFile < 0");
        PifAbort(MODULE_ETK, FAULT_ERROR_FILE_SEEK);
    }
    PifWriteFile(hsEtkTotalFile, pEtkField, sizeof(ETK_FIELD_IN_FILE));
}

/*
*==========================================================================
**    Synopsis:   SHORT Etk_EmpBlkGet(ETK_HEAD *pEtkHead,
*                                     USHORT   *usParaBlcokNo)
*
*       Input:    ETK_FILEHEAD *pEtkFileHed
*      Output:    USHORT *usParaBlcokNo
*       InOut:    Nothing
*
**  Return    :   ETK_SUCCESS
*                 ETK_FULL
*                
**  Description:
*                Get empty Block No.
*==========================================================================
*/
SHORT   Etk_EmpBlkGet(ETK_HEAD *pEtkHead, USHORT *usParaBlockNo)
{
    ETK_TOTALHEAD EtkFileHead = {0};
    UCHAR    auchParaEmpBlk[ETK_PARA_EMPTY_TABLE_SIZE];  /* paramater empty table */
    SHORT    sI, sJ, sAnswer;

    /* get paramater empty table */
    Etk_ReadFile(hsEtkTotalFile, pEtkHead->ulOffsetOfParaEmp, auchParaEmpBlk, ETK_PARA_EMPTY_TABLE_SIZE);

    sAnswer = 0;

    /* Search empty block from paramater empty table */
    for (sI = 0; sI < ETK_PARA_EMPTY_TABLE_SIZE; sI++ ) {
        if (auchParaEmpBlk[sI] != 0xff) {    
            for (sJ = 0; sJ < 8; sJ++) {          
                if (((auchParaEmpBlk[sI] >> sJ) & 0x01) == 0x00) {
                    auchParaEmpBlk[sI] |= (UCHAR)(0x01 << sJ);
                    sAnswer = 1;
                    break;
                }
            }
        }
        if (sAnswer == 1) {
            break;
        }
    }

    /* Check and Make Paramater Block No.   */
    if (sAnswer == 1) {
        *usParaBlockNo = ((sI * 8) + (sJ + 1));  /* Make Paramater block No */
        Etk_ReadFile(hsEtkTotalFile, ETK_FILE_HED_POSITION, &EtkFileHead, sizeof(ETK_PARAHEAD));
        if (*usParaBlockNo > EtkFileHead.usNumberOfMaxEtk) {
            return(ETK_FULL);
        }
        Etk_WriteFile(hsEtkTotalFile, pEtkHead->ulOffsetOfParaEmp, auchParaEmpBlk, ETK_PARA_EMPTY_TABLE_SIZE);
        return(ETK_SUCCESS);
    }
    return(ETK_FULL);
}

/*
*==========================================================================
**    Synopsis:   VOID  Etk_BlkEmp(ETK_HEAD *pEtkHead,
*                                  USHORT usParaBlockNo)
*
*       Input:    ETK_HEAD *pEtkHead
*                 USHORT usParaBlockNo
*                 
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*
**  Description:
*            Let Empty Block.
*==========================================================================
*/
VOID    Etk_BlkEmp(ETK_HEAD *pEtkHead, USHORT usParaBlockNo)
{
    UCHAR    auchParaEmpBlk[ETK_PARA_EMPTY_TABLE_SIZE];
    SHORT    sI, sJ;

    Etk_ReadFile(hsEtkTotalFile, pEtkHead->ulOffsetOfParaEmp, auchParaEmpBlk, ETK_PARA_EMPTY_TABLE_SIZE);

    sI = (SHORT)(usParaBlockNo - 1) / 8;
    sJ = (SHORT)(usParaBlockNo - 1) % 8;

    auchParaEmpBlk[sI] &= ~( 0x01 << sJ );
                                                          
    Etk_WriteFile(hsEtkTotalFile, (ULONG)pEtkHead->ulOffsetOfParaEmp, auchParaEmpBlk, ETK_PARA_EMPTY_TABLE_SIZE);
}


/*
*==========================================================================
**    Synopsis:    VOID    Etk_WriteFile(SHORT  hsFileHandle,
*                                        ULONG  offulFileSeek,
*                                        VOID   *pTableHeadAddress,
*                                        USHORT usSizeofWrite)
*
*       Input:   SHORT  hsFileHandle    * handle para file or total file.
*                ULONG  offulFileSeek   * offset from file head *
*                VOID   *pTableHeadAddress * table head address for writing into file *
*                USHORT usSizeofWrite  * size of for writing into file *
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Nothing 
*
**  Description:
*               Write data to Etk file. 
*==========================================================================
*/
VOID    Etk_WriteFile(SHORT   hsHandle,
                      ULONG   offulFileSeek,
                      VOID    *pTableHeadAddress,
                      ULONG  usSizeofWrite)
{
    ULONG   ulActualPosition;   /* DUMMY FOR PifSeekFile */
                                                            
    if (usSizeofWrite) {
		if ((PifSeekFile(hsHandle, offulFileSeek, &ulActualPosition)) < 0 ) {
 			NHPOS_ASSERT(!"Etk_WriteFile PifSeekFile < 0");
			PifAbort(MODULE_ETK, FAULT_ERROR_FILE_SEEK);
		}
        
		PifWriteFile(hsHandle, pTableHeadAddress, usSizeofWrite);
    }
}
/*
*==========================================================================
**    Synopsis:    SHORT    Etk_ReadFile(SHORT  hsFileHandle,
*                                        ULONG  offulFileSeek,
*                                        VOID   *pTableHeadAddress,
*                                        USHORT usSizeofRead)
*
*       Input:    SHORT hsFileHandle    * File Handle para or total file.
*                 ULONG offulFileSeek   * offset from file head *
*                 USHORT usSizeofRead   * size of for reading buffer *
*
*      Output:    VOID  *pTableHeadAddress * table head address for reading from file *
*       InOut:    Nothing
*
**  Return   :    ETK_SUCCESS
*                 ETK_NO_READ_SIZE           read size is 0 
*                 (file error is system halt)
*                    
*
**  Description:
*               Read data in the ETK File.
*==========================================================================
*/
SHORT    Etk_ReadFile(SHORT  hsFileHandle,
                      ULONG  offulFileSeek,
                      VOID   *pTableHeadAddress,
                      ULONG  ulSizeofRead)
{
    ULONG ulActualPosition;
	ULONG ulActualBytesRead; //RPH 11-7-3

    if ((PifSeekFile(hsFileHandle, offulFileSeek, &ulActualPosition)) < 0 ) {
 		NHPOS_ASSERT(!"Etk_ReadFile PifSeekFile < 0");
		if (offulFileSeek == 0) {
			// handle the case that an attempt to read the file header fails
			// because the file is empty. scenario seen with conversions from
			// older release databases such as Rel 2.1 which may have operators
			// but no employee records.
			return ETK_NO_READ_SIZE;
		}
        PifAbort(MODULE_ETK, FAULT_ERROR_FILE_SEEK);
    }

    if (ulSizeofRead == 0x00){
        return (ETK_NO_READ_SIZE);
    }
	
	//RPH 11-7-3 Changes For PifReadFile
	PifReadFile(hsFileHandle, pTableHeadAddress, ulSizeofRead, &ulActualBytesRead);
    if ( ulActualBytesRead != ulSizeofRead) {
 		NHPOS_ASSERT(!"Etk_ReadFile PifReadFile < ulSizeofRead");
        PifAbort(MODULE_ETK, FAULT_ERROR_FILE_READ);
    }

    return(ETK_SUCCESS);
}
/*
*==========================================================================
**    Synopsis:    VOID    Etk_CloseAllFileReleaseSem(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :    Nothing (error is system error(progrum is abort))
*
**  Description:
    
*               Close the Etk file and release the ETK semaphore.
*==========================================================================
*/
VOID    Etk_CloseAllFileReleaseSem(VOID)
{
    PifCloseFile(hsEtkParaFile);
	hsEtkParaFile = -1;            // set file handle to invalid value after PifCloseFile()
    PifCloseFile(hsEtkTotalFile);
	hsEtkTotalFile = -1;           // set file handle to invalid value after PifCloseFile()
    PifReleaseSem(husEtkSem);
}
/*
*==========================================================================
**    Synopsis:    VOID    Etk_CloseIndFileReleaseSem(SHORT hsFileHandle)
*
*       Input:    SHORT     hsFileHandle
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :    Nothing (error is system error(progrum is abort))
*
**  Description:
    
*               Close the Etk Para File and release the ETK semaphore.
*==========================================================================
*/
VOID    Etk_CloseIndFileReleaseSem(SHORT hsFileHandle)
{
    PifCloseFile(hsFileHandle);
    PifReleaseSem(husEtkSem);
}
/*                 
*==========================================================================
**    Synopsis:    VOID   Etk_ClearStatus(ETK_HEAD *pEtkHead)
*
*       Input:    ETK_HEAD  *pEtkHead    * Pointer to File Head *
*
*      Output:    NONE
*       InOut:    NONE
*
**  Return    :   NONE 
*
**  Description:
*                Clear Status on index table at ETK file.
*==========================================================================
*/
VOID   Etk_ClearStatus(ETK_HEAD *pEtkHead)
{
    LONG ulWorkOffsetOfIndex;
    ETK_TOTALINDEX EtkIndex = {0};
    USHORT usCount;

    ulWorkOffsetOfIndex = pEtkHead->ulOffsetOfIndex;
    for (usCount = 0; usCount < pEtkHead->usNumberOfEtk; usCount++) {
        Etk_ReadFile(hsEtkTotalFile, ulWorkOffsetOfIndex, &EtkIndex, sizeof(ETK_TOTALINDEX));
        EtkIndex.uchStatus = 0;
        Etk_WriteFile(hsEtkTotalFile, ulWorkOffsetOfIndex, &EtkIndex, sizeof(ETK_TOTALINDEX));
        ulWorkOffsetOfIndex += sizeof(ETK_TOTALINDEX);
    }
}
/*                 
*==========================================================================
**    Synopsis:    SHORT   Etk_CheckStatus(ETK_HEAD *pEtkHead)
*
*       Input:    ETK_HEAD  *pEtkHead    * Pointer to File Head *
*
*      Output:    NONE
*       InOut:    NONE
*
**  Return    :   ETK_SUCCESS
*                 ETK_NOT_TIME_OUT      * DURING TIME_IN *
*
**  Description:
*                Check Status on index table at ETK file.
*==========================================================================
*/
SHORT   Etk_CheckStatus(ETK_HEAD *pEtkHead)
{
    LONG ulWorkOffsetOfIndex;
    ETK_TOTALINDEX EtkIndex = {0};
    USHORT usCount;

    ulWorkOffsetOfIndex = pEtkHead->ulOffsetOfIndex;
    for (usCount = 0; usCount < pEtkHead->usNumberOfEtk; usCount++) {
        Etk_ReadFile(hsEtkTotalFile, ulWorkOffsetOfIndex, &EtkIndex, sizeof(ETK_TOTALINDEX));
        if (EtkIndex.uchStatus & ETK_TIME_IN) {
            return ETK_NOT_TIME_OUT;
        }
        ulWorkOffsetOfIndex += sizeof(ETK_TOTALINDEX);
    }
    return ETK_SUCCESS;
}
/*                 
*==========================================================================
**    Synopsis:   SHORT   Etk_CheckAllowJob(ETK_JOB   *pEtkJob,
*                                           UCHAR     uchJobCode)
*
*       Input:    ETK_JOB  EtkJob                 * Job Code in Assignment
*                 UCHAR    uchJobCode             * Job Code to be check *
*
*      Output:    NONE
*       InOut:    NONE
*
**  Return    :   ETK_SUCCESS
*                 ETK_NOT_IN_JOB
*
**  Description:
*                 Check job code that is argument
*                 whether it is in job code table.
*==========================================================================
*/
SHORT   Etk_CheckAllowJob(ETK_JOB   *pEtkJob,
                          UCHAR     uchJobCode)
{
    if (uchJobCode == 0){
        return ETK_SUCCESS;
    }
    if (pEtkJob->uchJobCode1 != uchJobCode) {
        if (pEtkJob->uchJobCode2 != uchJobCode) {
            if (pEtkJob->uchJobCode3 != uchJobCode) {
                return ETK_NOT_IN_JOB;
            }
        }
    }
    return ETK_SUCCESS;
}
/*                 
*==========================================================================
**    Synopsis:   SHORT    Etk_MakeParaFile(USHORT  usNumberOfEmployee)
*                                          
*
*       Input:    usNumberOfEmployee            * Number Of Employee *
*
*      Output:    NONE
*       InOut:    NONE
*
**  Return    :   ETK_SUCCESS                * File Making is Success *
*                 ETK_NO_MAKE_FILE           * Ram disk full *
*
**  Description:
*                 Make Paramater File according to usNumberOfEmployee.
*==========================================================================
*/
SHORT    Etk_MakeParaFile(USHORT  usNumberOfEmployee)
{
    ETK_PARAHEAD   EtkParaHead = {0};
    ULONG   ulActualPosition;

    /* Make Header */
    EtkParaHead.usFileSize = (sizeof(ETK_PARAHEAD) + (sizeof(ETK_PARAINDEX) * usNumberOfEmployee));
   
    EtkParaHead.usNumberOfMaxEtk = usNumberOfEmployee;
    EtkParaHead.usNumberOfEtk    = 0;

    /* Make Paramater File */
    if (PifSeekFile(hsEtkParaFile, (ULONG)EtkParaHead.usFileSize, &ulActualPosition) < 0) {
        PifCloseFile(hsEtkParaFile);
		hsEtkParaFile = -1;                               // mark the file as closed.
        PifDeleteFile(auchETK_PARA_FILE);
        PifCloseFile(hsEtkTotalFile);
		hsEtkTotalFile = -1;                              // mark the file as closed.
        PifDeleteFile(auchETK_TOTAL_FILE);
		PifLog (MODULE_ETK, LOG_ERROR_ETK_PARA_CREATE);
		PifLog (MODULE_LINE_NO(MODULE_ETK), (USHORT)__LINE__);
        return(ETK_NO_MAKE_FILE);
    }

    Etk_WriteFile(hsEtkParaFile, ETK_FILE_HED_POSITION, &EtkParaHead, sizeof(ETK_PARAHEAD));
    return(ETK_SUCCESS);
}
/*                 
*==========================================================================
**    Synopsis:   SHORT    Etk_MakeTotalFile(USHORT  usNumberOfEmployee)
*                                          
*
*       Input:    usNumberOfEmployee            * Number Of Employee *
*
*      Output:    NONE
*       InOut:    NONE
*
**  Return    :   ETK_SUCCESS                * File Making is Success *
*                 ETK_NO_MAKE_FILE           * Ram disk full *
*
**  Description:
*                 Make Total File according to usNumberOfEmployee.
*==========================================================================
*/
SHORT    Etk_MakeTotalFile(USHORT  usNumberOfEmployee)
{
   ULONG          ulActualPosition;
   ETK_TOTALHEAD  EtkTotalHead = {0};
   UCHAR          auchParaEmp[ETK_PARA_EMPTY_TABLE_SIZE];
   
   EtkTotalHead.usNumberOfMaxEtk = usNumberOfEmployee;
   EtkTotalHead.uchIssuedFlag = ETK_FLAG_CLEAR;
   EtkTotalHead.EtkCur.usNumberOfEtk = 0;
   EtkTotalHead.EtkCur.ulOffsetOfIndex = sizeof(ETK_TOTALHEAD);
   EtkTotalHead.EtkCur.ulOffsetOfParaEmp = EtkTotalHead.EtkCur.ulOffsetOfIndex + sizeof(ETK_TOTALINDEX) * usNumberOfEmployee;
   EtkTotalHead.EtkCur.ulOffsetOfPara    = EtkTotalHead.EtkCur.ulOffsetOfParaEmp + ETK_PARA_EMPTY_TABLE_SIZE;
   EtkTotalHead.EtkSav.usNumberOfEtk = 0;
   EtkTotalHead.EtkSav.ulOffsetOfIndex = EtkTotalHead.EtkCur.ulOffsetOfPara + sizeof(ETK_FIELD_IN_FILE) * ETK_MAX_FIELD * usNumberOfEmployee;
   EtkTotalHead.EtkSav.ulOffsetOfParaEmp = EtkTotalHead.EtkSav.ulOffsetOfIndex + sizeof(ETK_TOTALINDEX) * usNumberOfEmployee;
   EtkTotalHead.EtkSav.ulOffsetOfPara = EtkTotalHead.EtkSav.ulOffsetOfParaEmp + ETK_PARA_EMPTY_TABLE_SIZE;
   EtkTotalHead.ulFileSize = EtkTotalHead.EtkSav.ulOffsetOfPara + sizeof(ETK_FIELD_IN_FILE) * ETK_MAX_FIELD * usNumberOfEmployee;

   {
		DATE_TIME DateTime;          /* for TOD read */

		PifGetDateTime(&DateTime);
		// initialize the Saved Table or Reset Time to the current date/time.
		// this will make the various AC150 and AC151 reports have the correct header
		// and when AC151 is done to reset employee time keeping, date rotation will give correct date.
		EtkTotalHead.EtkTime.usToMonth  = EtkTotalHead.EtkTime.usFromMonth  = DateTime.usMonth;
		EtkTotalHead.EtkTime.usToDay    = EtkTotalHead.EtkTime.usFromDay    = DateTime.usMDay;
		EtkTotalHead.EtkTime.usToTime   = EtkTotalHead.EtkTime.usFromTime   = DateTime.usHour;
		EtkTotalHead.EtkTime.usToMinute = EtkTotalHead.EtkTime.usFromMinute = DateTime.usMin;
   }

    /* Make Total File */
    if (PifSeekFile(hsEtkTotalFile, EtkTotalHead.ulFileSize, &ulActualPosition) < 0) {
        PifCloseFile(hsEtkParaFile);
		hsEtkParaFile = -1;                               // mark the file as closed.
        PifDeleteFile(auchETK_PARA_FILE);
        PifCloseFile(hsEtkTotalFile);
		hsEtkTotalFile = -1;                              // mark the file as closed.
        PifDeleteFile(auchETK_TOTAL_FILE);
		PifLog (MODULE_ETK, LOG_ERROR_ETK_TOTAL_CREATE);
		PifLog (MODULE_LINE_NO(MODULE_ETK), (USHORT)__LINE__);
        return(ETK_NO_MAKE_FILE);
    }

   /* Clear Data Block Empty Table and Write file*/
   memset(auchParaEmp, 0x00, sizeof(auchParaEmp));
   Etk_WriteFile(hsEtkTotalFile, EtkTotalHead.EtkCur.ulOffsetOfParaEmp, auchParaEmp, ETK_PARA_EMPTY_TABLE_SIZE);
   
   memset(auchParaEmp, 0x00, sizeof(auchParaEmp));
   Etk_WriteFile(hsEtkTotalFile, EtkTotalHead.EtkSav.ulOffsetOfParaEmp, auchParaEmp, ETK_PARA_EMPTY_TABLE_SIZE);

    /* Write File Header */
    Etk_WriteFile(hsEtkTotalFile, ETK_FILE_HED_POSITION, &EtkTotalHead, sizeof(ETK_TOTALHEAD));
    return(ETK_SUCCESS);
}
/*
*==========================================================================
**  Synopsis:    SHORT   Etk_IndexPara(USHORT usIndexMode,
*                                      ULONG  ulEtkNo,
*                                      ETK_PARAINDEX *pEtkParaIndex,
*                                      ULONG  *pulIndexPosition)
*
*       Input:    USHORT    usIndexMode         ETK_SEARCH_ONLY
*                                               ETK_REGIST
*                 ULONG     ulEtkNo             Pointer to object to search for
*                        
*      Output:    ETK_PARAINDEX *pEtkParaIndex, Serched record data 
*                 ULONG  *pulIndexPosition)     Offset of Serched record data
*                                               from file head.
*
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    ETK_RESIDENT
*            Abnormal End:  ETK_FILE_NOT_FOUND
*                           ETK_NOT_IN_FILE
*                           ETK_FULL
*   
**  Description:
*                Check if Etk exsisted.
*==========================================================================
*/
SHORT   Etk_IndexPara(USHORT usIndexMode,
                  ULONG ulEtkNo,
                  ETK_PARAINDEX *pEtkEntry,
                  ULONG  *pulIndexPosition)
{
    ETK_PARAINDEX aWorkRcvBuffer[ETK_NO_OF_BUFFER + 1] = {0};  /* index table */
    ETK_PARAINDEX *pHitPoint;                            /* search hit point */
    SHORT    sStatus;                                    /* return status save area */
    SHORT   sTry=0;
    ULONG  ulOffset;
    ETK_PARAHEAD    EtkParaHead = {0};

    /* Get File Header */
    Etk_ReadFile(hsEtkParaFile, ETK_FILE_HED_POSITION, &EtkParaHead, sizeof(ETK_PARAHEAD));

    /* Check Etk Assign by Paramater File */
    do {
    USHORT  usRead;
    if ((EtkParaHead.usNumberOfEtk - sTry*ETK_NO_OF_BUFFER) > ETK_NO_OF_BUFFER) {
        usRead = ETK_NO_OF_BUFFER * sizeof(ETK_PARAINDEX);
    } else {
		USHORT  i;

        for (i=0, usRead=0;
                i<(EtkParaHead.usNumberOfEtk - sTry*ETK_NO_OF_BUFFER);
                i++) {

            usRead += sizeof(ETK_PARAINDEX);
        }
    }
    if (Etk_ReadFile(hsEtkParaFile,
                     sizeof(ETK_PARAHEAD) +             /* Read Position in file */
                        (sTry*ETK_NO_OF_BUFFER * sizeof(ETK_PARAINDEX)),
                     aWorkRcvBuffer,                   /* Storage location for read data */
                     usRead) == ETK_NO_READ_SIZE) {
        /* INDEX TABLE == 0 */
        if (usIndexMode == ETK_SEARCH_ONLY) {
            return(ETK_NOT_IN_FILE);
        }
        pHitPoint = aWorkRcvBuffer;         /* pointer <- pointer */
        sStatus = RFL_TAIL_OF_TABLE;
        break;
    } else {
        
        sStatus = Rfl_SpBsearch(&ulEtkNo,       /* */
                     sizeof(ETK_PARAINDEX),     /* Size of Record */
                     aWorkRcvBuffer,            /* read buffer */
                     (USHORT)((EtkParaHead.usNumberOfEtk - sTry*ETK_NO_OF_BUFFER > ETK_NO_OF_BUFFER) ? 
                     ETK_NO_OF_BUFFER : EtkParaHead.usNumberOfEtk - sTry*ETK_NO_OF_BUFFER), /* number of key  */
                     &pHitPoint,                /* hit point */
                     Etk_ParaCompIndex);                /* compare routine */

        if (sStatus == RFL_HIT) {               /* exist ETK no */
            *pulIndexPosition = sizeof(ETK_PARAHEAD) + ((UCHAR *)pHitPoint - (UCHAR *)aWorkRcvBuffer) + ETK_NO_OF_BUFFER * sTry * sizeof(ETK_PARAINDEX);
            if (usIndexMode == ETK_SEARCH_ONLY) {
                *pEtkEntry = *pHitPoint;            /* Copy Index record */
            } else {
                *pHitPoint = *pEtkEntry;            /* Copy Index record */
                /* Save(update) Index Table */

                Etk_WriteFile(hsEtkParaFile, *pulIndexPosition, pHitPoint, sizeof(ETK_PARAINDEX));
            }
            return(ETK_RESIDENT);
        } else {
            if ((EtkParaHead.usNumberOfEtk - sTry*ETK_NO_OF_BUFFER) > ETK_NO_OF_BUFFER) {
                usRead = ETK_NO_OF_BUFFER;
            } else {
                usRead = EtkParaHead.usNumberOfEtk - sTry*ETK_NO_OF_BUFFER;
            }
            if (usRead) usRead--;
            if (aWorkRcvBuffer[usRead].ulEmployeeNo > ulEtkNo) {
                break;
            }
        }
    }
    sTry++;
    } while((SHORT)(EtkParaHead.usNumberOfEtk - ETK_NO_OF_BUFFER*sTry) > 0);

    if (sStatus != RFL_HIT) {
        if (usIndexMode == ETK_SEARCH_ONLY) {
            return(ETK_NOT_IN_FILE);
        }
    }

    /* Check Employee Full */
    if (EtkParaHead.usNumberOfMaxEtk <= EtkParaHead.usNumberOfEtk) {
        return (ETK_FULL);
    }

    /* set output condition */
    if ((SHORT)(EtkParaHead.usNumberOfEtk - ETK_NO_OF_BUFFER*sTry) <= 0) {
        if(sTry)sTry--;
    }

    *pulIndexPosition = sizeof(ETK_PARAHEAD) + ((UCHAR *)pHitPoint - (UCHAR *)aWorkRcvBuffer) + ETK_NO_OF_BUFFER*sTry*sizeof(ETK_PARAINDEX);

    if (sStatus != RFL_TAIL_OF_TABLE) {
        ulOffset = (ULONG)(sizeof(ETK_PARAHEAD) + (EtkParaHead.usNumberOfEtk -1)* sizeof(ETK_PARAINDEX));
        for(; (LONG)ulOffset>=(LONG)*pulIndexPosition; ulOffset-=(ULONG)(sizeof(ETK_PARAINDEX))) {
            Etk_ReadFile(hsEtkParaFile, ulOffset,
                         aWorkRcvBuffer, sizeof(ETK_PARAINDEX));

            Etk_WriteFile(hsEtkParaFile, (ULONG)(ulOffset+sizeof(ETK_PARAINDEX)),
                         aWorkRcvBuffer, sizeof(ETK_PARAINDEX));
        }
    }

    pEtkEntry->ulEmployeeNo = ulEtkNo;

    *pHitPoint = *pEtkEntry;                         /* copy structure */

    /* ResidentEtk + 1 */
    EtkParaHead.usNumberOfEtk++;

    /* Save(update) Paramater header file  */
    Etk_WriteFile(hsEtkParaFile, ETK_FILE_HED_POSITION, &EtkParaHead, sizeof(ETK_PARAHEAD));

    /* Save(update) Index Table */
    Etk_WriteFile(hsEtkParaFile, *pulIndexPosition, pHitPoint, sizeof(ETK_PARAINDEX));

    return(ETK_RESIDENT);
}
/*
*==========================================================================
**    Synopsis:    SHORT   Etk_IndexDelPara( ULONG ulEtkNo )
*
*       Input:    ULONG     ulEtkNo
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    ETK_SUCCESS
*          Abnormal End:    ETK_NOT_IN_FILE
*
**  Description:
*                Delete inputted Etk from index table in Paramater File.
*==========================================================================
*/
SHORT   Etk_IndexDelPara( ULONG ulEtkNo )
{
    ETK_PARAINDEX aWorkRcvBuffer[ETK_NO_OF_BUFFER + 1] = {0};  /* index table */
    ETK_PARAINDEX *pHitPoint;                            /* search hit point */
    SHORT    sStatus;                                    /* return status save area */
    SHORT   sTry=0;
	ULONG  ulOffset;
    ETK_PARAHEAD    EtkParaHead = {0};

    /* Get File Header */
    Etk_ReadFile(hsEtkParaFile, ETK_FILE_HED_POSITION, &EtkParaHead, sizeof(ETK_PARAHEAD));

    /* Check Etk Assign by Paramater File */
    do {
		USHORT  usRead;

		if ((EtkParaHead.usNumberOfEtk - sTry*ETK_NO_OF_BUFFER) > ETK_NO_OF_BUFFER) {
			usRead = ETK_NO_OF_BUFFER * sizeof(ETK_PARAINDEX);
		} else {
			USHORT  i;

			for (i=0, usRead=0;
					i<(EtkParaHead.usNumberOfEtk - sTry*ETK_NO_OF_BUFFER);
					i++) {

				usRead += sizeof(ETK_PARAINDEX);
			}
		}
		if (Etk_ReadFile(hsEtkParaFile,
						 sizeof(ETK_PARAHEAD) +             /* Read Position in file */
							(sTry*ETK_NO_OF_BUFFER * sizeof(ETK_PARAINDEX)),
						 aWorkRcvBuffer,                   /* Storage location for read data */
						 usRead) == ETK_NO_READ_SIZE) {
			/* INDEX TABLE == 0 */
			return(ETK_NOT_IN_FILE);
			break;
		} else {
        
			sStatus = Rfl_SpBsearch(&ulEtkNo,       /* */
						 sizeof(ETK_PARAINDEX),     /* Size of Record */
						 aWorkRcvBuffer,            /* read buffer */
						 (USHORT)((EtkParaHead.usNumberOfEtk - sTry*ETK_NO_OF_BUFFER > ETK_NO_OF_BUFFER) ? 
						 ETK_NO_OF_BUFFER : EtkParaHead.usNumberOfEtk - sTry*ETK_NO_OF_BUFFER), /* number of key  */
						 &pHitPoint,                /* hit point */
						 Etk_ParaCompIndex);                /* compare routine */

			if (sStatus == RFL_HIT) {               /* exist ETK no */
				break;
			} else {
				if ((EtkParaHead.usNumberOfEtk - sTry*ETK_NO_OF_BUFFER) > ETK_NO_OF_BUFFER) {
					usRead = ETK_NO_OF_BUFFER;
				} else {
					usRead = EtkParaHead.usNumberOfEtk - sTry*ETK_NO_OF_BUFFER;
				}
				if (usRead) usRead--;
				if (aWorkRcvBuffer[usRead].ulEmployeeNo > ulEtkNo) {
					break;
				}
			}
		}
		sTry++;
    } while((SHORT)(EtkParaHead.usNumberOfEtk - ETK_NO_OF_BUFFER*sTry) > 0);

    if (sStatus != RFL_HIT) {
        return(ETK_NOT_IN_FILE);
    }

    /* set output condition */
    if ((SHORT)(EtkParaHead.usNumberOfEtk - ETK_NO_OF_BUFFER*sTry) <= 0) {
        if(sTry)sTry--;
    }

    ulOffset = sizeof(ETK_PARAHEAD) + ((UCHAR *)pHitPoint - (UCHAR *)aWorkRcvBuffer) + ETK_NO_OF_BUFFER*sTry*sizeof(ETK_PARAINDEX);

    for(; ((USHORT)ulOffset+sizeof(ETK_PARAINDEX))<EtkParaHead.usFileSize; ulOffset+=(ULONG)sizeof(ETK_PARAINDEX)) {
        Etk_ReadFile(hsEtkParaFile, (ULONG)(ulOffset+sizeof(ETK_PARAINDEX)), aWorkRcvBuffer, sizeof(ETK_PARAINDEX));

        Etk_WriteFile(hsEtkParaFile, ulOffset, aWorkRcvBuffer, sizeof(ETK_PARAINDEX));
    }

    /* update number of resident ETK */ 
    if (EtkParaHead.usNumberOfEtk > 0) {
		EtkParaHead.usNumberOfEtk--;
	}

    /* Save(update) Paramater header file  */
    Etk_WriteFile(hsEtkParaFile, ETK_FILE_HED_POSITION, &EtkParaHead, sizeof(ETK_PARAHEAD));

    return(ETK_SUCCESS);
}

/*
*==========================================================================
**    Synopsis:    SHORT    Etk_OpenAllFile(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :    ETK_SUCCESS
*                  ETK_FILE_NOT_FOUND
*
**  Description:
    
*               Open all Etk file.
*==========================================================================
*/
SHORT    Etk_OpenAllFile(VOID)
{
    /* Parameter File Open */
    hsEtkParaFile = PifOpenFile(auchETK_PARA_FILE, auchOLD_FILE_READ_WRITE);
    if (hsEtkParaFile < 0) {
        PifReleaseSem(husEtkSem);
		PifLog (MODULE_ETK, LOG_ERROR_ETK_PARAM_OPEN);
		PifLog (MODULE_ERROR_NO(MODULE_ETK), (USHORT)abs(hsEtkParaFile));
		PifLog (MODULE_LINE_NO(MODULE_ETK), (USHORT)__LINE__);
        return(ETK_FILE_NOT_FOUND);
    }

    /* Total File Open */
    hsEtkTotalFile = PifOpenFile(auchETK_TOTAL_FILE, auchOLD_FILE_READ_WRITE);
    if (hsEtkTotalFile < 0) {
		PifCloseFile(hsEtkParaFile);                      // error opening total file so close the para file.
		hsEtkParaFile = -1;                               // mark the file as closed.
		PifReleaseSem(husEtkSem);
		PifLog (MODULE_ETK, LOG_ERROR_ETK_TOTAL_OPEN);
		PifLog (MODULE_ERROR_NO(MODULE_ETK), (USHORT)abs(hsEtkTotalFile));
		PifLog (MODULE_LINE_NO(MODULE_ETK), (USHORT)__LINE__);
        return(ETK_FILE_NOT_FOUND);
    }
    return(ETK_SUCCESS);    
}

/*
*==========================================================================
**    Synopsis:    SHORT    Etk_OpenIndFile( USHORT usFileType)
*
*       Input:    USHORT usFileType  
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :    ETK_SUCCESS
*                  ETK_FILE_NOT_FOUND
*
**  Description:
    
*               Open ind. Etk file.
*==========================================================================
*/
SHORT    Etk_OpenIndFile(USHORT usFileType)
{
    if ( usFileType == ETK_PARAM_FILE ) {
        /* Parameter File Open */
        hsEtkParaFile = PifOpenFile(auchETK_PARA_FILE, auchOLD_FILE_READ_WRITE);
        if (hsEtkParaFile < 0) {
            PifReleaseSem(husEtkSem);
			PifLog (MODULE_ETK, LOG_ERROR_ETK_PARAM_OPEN);
			PifLog (MODULE_ERROR_NO(MODULE_ETK), (USHORT)abs(hsEtkParaFile));
			PifLog (MODULE_LINE_NO(MODULE_ETK), (USHORT)__LINE__);
            return(ETK_FILE_NOT_FOUND);
        }
    } else {
        /* Total File Open */
        hsEtkTotalFile = PifOpenFile(auchETK_TOTAL_FILE, auchOLD_FILE_READ_WRITE);
        if (hsEtkTotalFile < 0) {
            PifReleaseSem(husEtkSem);
			PifLog (MODULE_ETK, LOG_ERROR_ETK_TOTAL_OPEN);
			PifLog (MODULE_ERROR_NO(MODULE_ETK), (USHORT)abs(hsEtkTotalFile));
			PifLog (MODULE_LINE_NO(MODULE_ETK), (USHORT)__LINE__);
            return(ETK_FILE_NOT_FOUND);
        }
    }
    return(ETK_SUCCESS);    
}

SHORT  Etk_GetParaTotalBlocks (ETK_PARA_TOTAL *pList, SHORT sMaxCount)
{
	SHORT           sCount = 0;

	{
		ETK_PARAHEAD    EtkParaHead = {0};
		ULONG           ulParaOffset;

		Etk_ReadFile(hsEtkParaFile, ETK_FILE_HED_POSITION, &EtkParaHead, sizeof(ETK_PARAHEAD));

		ulParaOffset = sizeof(ETK_PARAHEAD);
		if (sMaxCount > EtkParaHead.usNumberOfEtk) sMaxCount = EtkParaHead.usNumberOfEtk;
		while (sCount < sMaxCount && Etk_ReadFile (hsEtkParaFile, ulParaOffset, &pList[sCount].paraData, sizeof(ETK_PARAINDEX)) >= 0) {
			ulParaOffset += sizeof(ETK_PARAINDEX);
			sCount++;
		}
	}

	{
		ETK_TOTALHEAD   EtkTotalHead = {0};
		ETK_TOTALINDEX  EtkTotalIndex;
		ULONG           ulTotalOffset;
		SHORT           sI;
		SHORT           sCountTemp = sCount;

		Etk_ReadFile(hsEtkTotalFile, ETK_FILE_HED_POSITION, &EtkTotalHead, sizeof(ETK_TOTALHEAD));

		// ensure that if this is a newly created file that we will not try to do a PifSeek() which
		// results in a PifAbort(). If there are no employee total records in the file then there is
		// nothing to do.
		if (sCountTemp > EtkTotalHead.EtkCur.usNumberOfEtk) sCountTemp = EtkTotalHead.EtkCur.usNumberOfEtk;
		if (EtkTotalHead.EtkCur.usNumberOfEtk > 0) {
			ulTotalOffset = EtkTotalHead.EtkCur.ulOffsetOfIndex;
			sI = 0;
			while (sI < sCount && Etk_ReadFile (hsEtkTotalFile, ulTotalOffset, &EtkTotalIndex, sizeof(ETK_TOTALINDEX)) >= 0) {
				if (EtkTotalIndex.ulEmployeeNo == pList[sI].paraData.ulEmployeeNo) {
					pList[sI].usBlockNo = EtkTotalIndex.usBlockNo;
					pList[sI].uchStatus = EtkTotalIndex.uchStatus;
					ulTotalOffset += sizeof(ETK_TOTALINDEX);
					sI++;
				} else {
					SHORT i;
					for (i = 0; i < sCount; i++) {
						if (EtkTotalIndex.ulEmployeeNo == pList[i].paraData.ulEmployeeNo) {
							pList[i].usBlockNo = EtkTotalIndex.usBlockNo;
							pList[i].uchStatus = EtkTotalIndex.uchStatus;
							sI++;
							break;
						}
					}
					ulTotalOffset += sizeof(ETK_TOTALINDEX);
					if (i >= sCount) {
						char xBuff[128];
						sprintf (xBuff, "==NOTE: Etk_GetParaTotalBlocks() %d employee total not match. ", EtkTotalIndex.ulEmployeeNo);
						NHPOS_ASSERT_TEXT((i >= sCount), xBuff);
					}
				}
			}
		}
	}

	return sCount;
}

/*====== End of Source ======*/
