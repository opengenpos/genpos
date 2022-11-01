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
*   Title              : Client/Server OPERATIONAL PARAMATER module, Internal subroutine                        
*   Category           : Client/Server OPERATIONAL PARAMATER module, NCR2170 US HOSPITALITY APPLICATION
*   Program Name       : NWOPIN.C                                            
*  ------------------------------------------------------------------------  
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.              
*   Memory Model       : Medium Model
*   Options            : /c /AM /Gs /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*   Abstract           : 
*    SHORT   Op_LockCheck                * Lock Check.
*    VOID    Op_WritePluFile             * Write data in Plu file.
*    VOID    Op_WriteDeptFile            * Write data in Dept File.
*    VOID    Op_WriteCpnFile             * Write data in Combination Coupon File.
*    VOID    Op_WriteCstrFile            * Write data in Control String File.
*    VOID    Op_ClosePluFileReleaseSem   * Close PLU file and Release semaphore
*    VOID    Op_CloseDeptFileReleaseSem  * Close DEPT file and Release semaphore
*    VOID    Op_CloseCpnFileReleaseSem   * Close Combination Coupon file and Release semaphore
*    VOID    Op_CloseCstrFileReleaseSem  * Close Control String file and Release semaphore
*    SHORT   Op_ReadDeptFile             * Read data in Dept file
*    SHORT   Op_ReadPluFile              * Read Data in Plu file.
*    SHORT   Op_ReadCpnFile              * Read data in Combination Coupon file
*    SHORT   Op_ReadCstrFile             * Read data in Control String file
*    SHORT   Op_Index                    * Add , Change and Search Plu.
*    SHORT   Op_IndexDel                 * Delete Plu.
*    VOID    Op_NoGet                    * Get Plu No. in Plu File.
*    VOID    Op_PluParaRead              * Read paramater of inputting Plu No. by usParaBlkNo.
*    VOID    Op_PluParaWrite             * Write Paramater inputted plu to Plu file .
*    SHORT   Op_CompIndex                * Compare index entry
*    SHORT   Op_GetPlu                   * Get Plu No in order of the number.
*    SHORT   Op_GetPluBy                 * Get Plu No. from sub-index table. (In order of Report No. or Dept No)
*    VOID    Op_InsertTable              * Insert data to Table in a File.
*    VOID    Op_DeleteTable              * Delete data from Index Table in a File.
*    SHORT   Op_SearchFile               * Search data on the Table in a File.
*    SHORT   Op_EmpBlkGet                * Let Empty Empty Block No.
*    VOID    Op_BlkEmp                   * Get Empty Block No.
*    SHORT   Op_CompSubIndex             * Compare Sub index entry
*    SHORT   Op_PpiOpenFile              * Open Promotianal Pricing Item File,    R3.1
*    VOID    Op_ClosePpiFileReleaseSem   * Close PPI file and Release semaphore,  R3.1
*    SHORT   Op_ReadPpiFile              * Read data in PPI file,                 R3.1
*    VOID    Op_WritePpiFile             * Write data in PPI File.                R3.1
*    VOID    Op_PpiAbort                 * PifAbort                               R3.1
*    SHORT   Op_MldOpenFile              * Open "PARAMLD" file                    V3.3
*    VOID    Op_CloseMldFileReleaseSem   * Close file and Release semaphore       V3.3
*    SHORT   Op_ReadMldFile              * Read data in Mld Mnemonics file.       V3.3
*    VOID    Op_WriteMldFile             * Write data in Mld Mnemonics File.      V3.3
*    VOID    Op_MldAbort                 * PifAbort                               V3.3
*
*  ------------------------------------------------------------------------
*    Update Histories  :
*   Date       Ver.Rev :NAME       :Description
*   Nov-08-93 :00.00.01:H.Yamaguchi:Modified  1000 PLU --> 3000 PLU 
*   Mar-07-95 :03.00.00:H.Ishida   :Add process of Combination Coupon
*             :        :           :   (Op_CloseCpnFileReleaseSem,
*             :        :           :    Op_ReadCpnFile,  Op_WriteCpnFile)
*   Feb-01-96 :03.00.05:M.Suzuki   :Add process of PPI
*   Sep-09-98 :03.03.00:A.Mitsui   :Add process of MLD
*   Nov-30-99 :        :K.Yanagida :NCR2172
*   
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
#include    "applini.h"
#include    <ej.h>
#include    "csopin.h"

/* static SHORT (*pCompOep)(VOID *pKey, VOID *pusHPoint) = Op_OepCompIndex;    / Add 2172 Rel 1.0 */
/* static SHORT (*pCompDept)(VOID *pKey, VOID *pusHPoint) = Op_DeptCompIndex;    / Add R3.0 */

/*
*==========================================================================
**    Synopsis:    SHORT   Op_LockCheck(USHORT usLockHnd)
*
*       Input: usLockHnd   Lock handle that is getted by OpLock().
*      Output: Nothing
*       InOut: Nothing
*
**  Return   : OP_PERFORM
*              OP_LOCK
*
**  Description: Lock Check. 
*==========================================================================
*/
SHORT   Op_LockCheck(USHORT usLockHnd)
{

    if (( husOpLockHandle == 0x0000 ) || ( husOpLockHandle == usLockHnd )) {
         return(OP_PERFORM);
    }
    return(OP_LOCK);
}


/*
*==========================================================================
**    Synopsis:    Op_ReadOepFile(ULONG  offulFileSeek, 
*                                 VOID   *pTableHeadAddress, 
*                                 USHORT usSizeofRead);
*
*       Input:  offulFileSeek           Number of bytes from the biginning of the oep file.
*               cusSizeofRead           Number of bytes to be read.
*      Output:  *pTableHeadAddress      Storage location of items to be read.
*       InOut:  Nothing
*
**  Return    : CAS_PERFORM
*               CAS_NO_READ_SIZE                  Read size is 0
*               (File error is system holt)
*
**  Description: Read data in Oep file.                         Saratoga
*==========================================================================
*/
SHORT   Op_ReadOepFile(ULONG offulFileSeek, 
                        VOID  *pTableHeadAddress, 
                        ULONG usSizeofRead)
{
    ULONG   ulActualPosition;

    if (usSizeofRead == 0X00) {
        return (OP_NO_READ_SIZE);
    }
    
    if ((PifSeekFile(hsOpOepFileHandle, offulFileSeek, &ulActualPosition)) < 0) {
        PifAbort(MODULE_OP_OEP, FAULT_ERROR_FILE_SEEK);
    }

    if (PifReadFile(hsOpOepFileHandle, 
                    pTableHeadAddress, usSizeofRead) != usSizeofRead) {
        PifAbort(MODULE_OP_OEP, FAULT_ERROR_FILE_READ);
    }
    return(OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    VOID    Op_WriteOepFile(ULONG  offulFileSeek, 
*                                          VOID   *pTableHeadAddress, 
*                                          USHORT usSizeofWrite)
*
*       Input:   offulFileSeek           Number of bytes from the biginning of the oep file.
*                *pTableHeadAddress      Pointer to data to be written.
*                usSizeofWrite           Number of bytes to be written.
*      Output:   Nothing
*       InOut:   Nothing
*
**  Return    :  Nothing (error is system error (progrum is abort)
*
**  Description: Write data in Oep File.                        Saratoga
*==========================================================================
*/
VOID   Op_WriteOepFile(ULONG offulFileSeek, 
                        VOID  *pTableHeadAddress, 
                        ULONG usSizeofWrite)
{
    ULONG   ulActualPosition;

    if (!usSizeofWrite) {
        return;
    }

    PifSeekFile(hsOpOepFileHandle, offulFileSeek, &ulActualPosition) < 0
        ? PifAbort(MODULE_OP_OEP, FAULT_ERROR_FILE_SEEK)
        : PifWriteFile(hsOpOepFileHandle, pTableHeadAddress, usSizeofWrite);
}

/*
*==========================================================================
**    Synopsis:    SHORT   Op_OepIndex(OPDEPT_FILEHED *OepFileHed, 
*                                      UCHAR         uchModeType, 
*                                      UCHAR         uchGroupNo, 
*                                      USHORT        *usParaBlockNo)
*
*       Input:    *OepFileHed     Pointer to the Oep file headder
*                 uchModeType     Type of mode (search only or regist)
*                 uchGrouptNo     Group No to be searched.
*                 
*      Output:    *usParaBlockNo  Storage location for Pamataer block No.
*       InOut:    Nothing
*
**  Return    :   OP_NEW_OEP
*                 OP_CHANGE_OEP
*                 OP_NOT_IN_FILE
*                 (file error is system halt)
*
**  Description:
*                 uchModeType
*                 OP_SEARCH_ONLY    Check Assign inputed Group No.
*                 OP_REGIST         Add and change inputted Group No. 2172
*==========================================================================
*/
SHORT   Op_OepIndex(OPOEP_FILEHED *OepFileHed, 
					USHORT         usFilePart,
                    WCHAR          uchModeType, 
                    UCHAR          uchGroupNo,
                    WCHAR          *uchPluNumber,
                    WCHAR          uchAdjective,
                    ULONG          *poffulSearchPoint)
{
    OPOEP_INDEXENTRY    IndexEntry;
    SHORT               sStatus;
    ULONG               offulSearchPoint;
	ULONG               *poffulFilePart;
	USHORT              *pusNumberOfResidentOep;
    
    offulSearchPoint = *poffulSearchPoint;

    /* Make Index Table Size */
    IndexEntry.usGroupNo = uchGroupNo;
    memcpy(IndexEntry.auchPluNo,uchPluNumber,sizeof(IndexEntry.auchPluNo));
    IndexEntry.uchAdjective = (UCHAR)uchAdjective;

	poffulFilePart = &(OepFileHed->offulOfIndexTbl);
	pusNumberOfResidentOep = &(OepFileHed->usNumberOfResidentOep);
	switch (usFilePart) {
		case 1:
			poffulFilePart = &(OepFileHed->offulOfIndexTbl2);
			pusNumberOfResidentOep = &(OepFileHed->usNumberOfResidentOep2);
			break;
	}

    if (OepFileHed->usNumberOfResidentOep != 0x0000) 
    {
		/* Search Group No. in Index Table */
        sStatus = Op_OepSearchFile(*poffulFilePart, (*pusNumberOfResidentOep * sizeof(OPOEP_INDEXENTRY)),
                                &offulSearchPoint, &IndexEntry, sizeof(IndexEntry), 
                                (SHORT (FAR *)(VOID *, VOID *))Op_OepCompIndex);
    
        if ((sStatus == OP_NOT_IN_FILE) && (uchModeType == OP_SEARCH_ONLY)) 
        {
            *poffulSearchPoint = offulSearchPoint;
            return (OP_NOT_IN_FILE);
        }

        if ((sStatus == OP_PERFORM)  && (uchModeType == OP_SEARCH_ONLY)) /* CHANGE OEP */
        {  
            *poffulSearchPoint = offulSearchPoint;
            return (sStatus);
        }

        /* BELOW IS NEW OEP  ASSIGN */

        /* Check Paramater Table full and get paramater No. */
        /* make space 1 record in index table */
        Op_OepInsertTable(*poffulFilePart, (*pusNumberOfResidentOep * sizeof(OPOEP_INDEXENTRY)),
                          (offulSearchPoint - *poffulFilePart),
                          &IndexEntry, sizeof(OPOEP_INDEXENTRY));

    }
    else 
    {
		/* Index Table size = 0 so there are no entries in the table.  This is the first. */
        if (uchModeType == OP_SEARCH_ONLY) 
        {
            *poffulSearchPoint = offulSearchPoint;
            return(OP_NOT_IN_FILE);
        }

        Op_WriteOepFile(*poffulFilePart, &IndexEntry, sizeof(OPOEP_INDEXENTRY));
    }

    /* ResidentDept +1 */
	(*pusNumberOfResidentOep)++;

    Op_WriteOepFile(OP_FILE_HEAD_POSITION, OepFileHed, sizeof(OPOEP_FILEHED));

    *poffulSearchPoint = offulSearchPoint;
    return(OP_NEW_OEP);
}

/*
*==========================================================================
**    Synopsis:   SHORT   Op_OepSearchFile(USHORT offusTableHeadAddress, 
*                                          USHORT usTableSize, 
*                                          USHORT *poffusSearchPoint,
*                                          VOID   *pSearchData, 
*                                          USHORT usSearchDataSize, 
*                                          SHORT (*Comp)(VOID *pKey, 
*                                          VOID *pusCompKey))
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
**  Return    :   OP_PERFORM
*                 OP_NOT_IN_FILE
*                 (file error is system halt)
*
**  Description:  Search data on the Table in a File. 2172
*==========================================================================
*/
SHORT   Op_OepSearchFile(ULONG offulTableHeadAddress, 
                         ULONG ulTableSize, 
                         ULONG *poffulSearchPoint,
                         VOID   *pSearchData, 
                         USHORT usSearchDataSize, 
                         SHORT (*Comp)(VOID *pKey, VOID *pusCompKey))

{
    SHORT sStatus;
    WCHAR auchKeyData[20];
    UCHAR auchWorkBuffer[OP_MAX_WORK_BUFFER];
    SHORT sCompStatus;
    ULONG offulTableTailAddress;
    ULONG offulTableSize;
    USHORT *pusHitPoint;
    ULONG  ulNumberOfEntry;             /* number of entry */

    sStatus = 0;

    /* make Table Tail address */

    offulTableTailAddress = (ULONG)(offulTableHeadAddress + ulTableSize);
    offulTableSize = ulTableSize;

    /* Check this key grater than Current MAX  Key */

/*    if ( usTableSize  != 0 ) {                                                */
/*                                                                              */
/*        *poffusSearchPoint = offusTableTailAddress -  usSearchDataSize;       */
/*                                                                              */
/*        Op_ReadDeptFile( *poffusSearchPoint, auchKeyData, usSearchDataSize);   */
/*                                                                              */
/*        if ((sCompStatus = (*Comp)(pSearchData, auchKeyData)) == 0) {         */
/*            return(OP_PERFORM);                                               */
/*        } else if (sCompStatus > 0) {                                         */
/*            *poffusSearchPoint = offusTableTailAddress;                       */
/*            return(OP_NOT_IN_FILE);                                           */
/*        }                                                                     */
/*    }                                                                         */
/*                                                                              */

    /* Search key in the file */

    while ( offulTableSize > OP_MAX_WORK_BUFFER ) 
    {    
        ulNumberOfEntry = (ULONG)((offulTableSize / (ULONG)usSearchDataSize) / 2L);    /* Saratoga */
        *poffulSearchPoint = (ULONG)((ulNumberOfEntry * (ULONG)usSearchDataSize) + offulTableHeadAddress);/* Saratoga */
        Op_ReadOepFile( *poffulSearchPoint, auchKeyData, usSearchDataSize);
        if ((sCompStatus = (*Comp)(pSearchData, (VOID *)auchKeyData)) == 0)     /* Saratoga */
        {
            return(OP_PERFORM);
            break;
        } 
        else if (sCompStatus > 0) 
        {   /* pSearchData > auchKeyData */
            offulTableHeadAddress = *poffulSearchPoint;
            offulTableHeadAddress += (ULONG)usSearchDataSize;
        } 
        else 
        {
            offulTableTailAddress = *poffulSearchPoint;
        }
        offulTableSize = (ULONG)(offulTableTailAddress - offulTableHeadAddress);   /* Saratoga */
    }

    Op_ReadOepFile(offulTableHeadAddress, 
                    auchWorkBuffer, 
                    (USHORT)offulTableSize);   /* offusTableSize != 0 */

    /* Search key in a table from the file  */

    ulNumberOfEntry = (ULONG)(offulTableSize / (ULONG)usSearchDataSize);      /* Saratoga */

    sStatus = Rfl_SpBsearch(pSearchData, 
                            usSearchDataSize, 
                            (USHORT *)auchWorkBuffer, 
                            (USHORT)ulNumberOfEntry,
                            (VOID *)&pusHitPoint, 
                            Comp);

    *poffulSearchPoint = (ULONG)((UCHAR *)pusHitPoint - auchWorkBuffer);   /* Saratoga */
    *poffulSearchPoint += offulTableHeadAddress; /* offset in the file */
    if (sStatus == RFL_HIT) 
    {
        return(OP_PERFORM);
    }
    return(OP_NOT_IN_FILE);
}

/*
*==========================================================================
**    Synopsis:    SHORT   Op_OepCompIndex(USHORT *uchGroupNo, USHORT *pusHitPoint)
*
*       Input:    *uchGroupNo            Key
*                 *pusHitPoint        Data to be compare.
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   0: uchGroupNo = pusHitPoint
*               0 <: uchGroupNo < pusHitPoint  
*               0 >: uchGroupNo > pusHitPoint 
*
**  Description: Compare index entry
*==========================================================================
*/
SHORT   Op_OepCompIndex(OPOEP_INDEXENTRY *Key, OPOEP_INDEXENTRY *pusHitPoint)
{
    SHORT sStatus;

    sStatus = (SHORT)(Key->usGroupNo - pusHitPoint->usGroupNo);
    if (sStatus == 0) {
        sStatus = (SHORT)memcmp(Key->auchPluNo, pusHitPoint->auchPluNo, OP_PLU_LEN* sizeof(WCHAR));
        if (sStatus == 0) {
            
            sStatus = (SHORT)(Key->uchAdjective - pusHitPoint->uchAdjective);
            
        }
    }
    return(sStatus);            
}

/*
*==========================================================================
**    Synopsis:   VOID    Op_OepInsertTable(USHORT offusTableHeadAddress, 
*                                           USHORT usTableSize
*                                           USHORT offusInsertPoint, 
*                                           VOID   *pInsertData, 
*                                           USHORT usInsertDataSize)
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
**  Description: Insert data to Table in a File. 2172
*==========================================================================
*/
VOID   Op_OepInsertTable( ULONG  offulTableHeadAddress, 
                          ULONG  ulTableSize,
                          ULONG  offulInsertPoint, 
                          VOID   *pInsertData, 
                          USHORT usInsertDataSize)
{
    UCHAR   auchWorkBuffer[OP_MAX_WORK_BUFFER];  /* work buffer */
    ULONG   offulTailInFile;    /* Table Tail Address */
    ULONG   ulMoveByte;         /* moving byte save area */

    if (ulTableSize <= offulInsertPoint) 
    {
        Op_WriteOepFile(offulInsertPoint + offulTableHeadAddress, pInsertData, usInsertDataSize );  /* insert data */
        return;
    }

    /* Calculate moving byte */
    
    ulMoveByte = (ulTableSize - offulInsertPoint);          /* Saratoga */
    offulTailInFile = (offulTableHeadAddress + ulTableSize);/* Saratoga */

    /* Move moving byte in a file */

    while ( ulMoveByte > OP_MAX_WORK_BUFFER ) {
        offulTailInFile -= OP_MAX_WORK_BUFFER;
        Op_ReadOepFile(offulTailInFile , auchWorkBuffer, OP_MAX_WORK_BUFFER);
        Op_WriteOepFile(offulTailInFile + usInsertDataSize, auchWorkBuffer, OP_MAX_WORK_BUFFER);
        ulMoveByte -= OP_MAX_WORK_BUFFER;
    }

    if (ulMoveByte != 0) {
        Op_ReadOepFile(offulTableHeadAddress + offulInsertPoint, auchWorkBuffer, ulMoveByte);
        Op_WriteOepFile((offulTableHeadAddress + offulInsertPoint + usInsertDataSize), auchWorkBuffer, ulMoveByte);
    }

    /* insert data */

    Op_WriteOepFile(offulTableHeadAddress + offulInsertPoint, pInsertData, usInsertDataSize );
}

/*
*==========================================================================
**    Synopsis:    Op_ReadDeptFile(ULONG  offulFileSeek, 
*                                  VOID   *pTableHeadAddress, 
*                                  USHORT usSizeofRead);
*
*       Input: offulFileSeek       Number of bytes from the biginning of the dept file.
*              cusSizeofRead       Number of bytes to be read.
*      Output: *pTableHeadAddress  Storage location of items to be read.
*       InOut: Nothing
*
**  Return   : OP_PERFORM
*              OP_NO_READ_SIZE                  Read size is 0
*              (File error is system holt)
*
**  Description: Read data in Dept file.          
*==========================================================================
*/
SHORT   Op_ReadDeptFile(ULONG  offulFileSeek, 
                        VOID   *pTableHeadAddress, 
                        ULONG usSizeofRead)
{
    ULONG   ulActualPosition;

    if (usSizeofRead == 0X00) {
        return (OP_NO_READ_SIZE);
    }
    
    if ((PifSeekFile(hsOpDeptFileHandle, offulFileSeek, &ulActualPosition)) < 0) {
        PifAbort(MODULE_OP_DEPT, FAULT_ERROR_FILE_SEEK);
    }

    if (PifReadFile(hsOpDeptFileHandle, pTableHeadAddress, usSizeofRead) != usSizeofRead) {
		// If there is an error reading the file then look to see if the offset indicates
		// we are reading the header portion of the file. If not then error exit.
		// If we are reading the header then return a header of all zeros.
		// This allows us to continue in the face of some kinds of errors which are
		// recoverable.
		if (offulFileSeek > 0) {
			PifAbort(MODULE_OP_DEPT, FAULT_ERROR_FILE_READ);
		}
		else {
			memset(pTableHeadAddress, 0, usSizeofRead);
		}
	}
    return(OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   Op_WriteDeptFile(ULONG  offulFileSeek, 
*                                           VOID   *pTableHeadAddress, 
*                                           USHORT usSizeofWrite)
*
*       Input: offulFileSeek       Number of bytes from the biginning of the dept file.
*              *pTableHeadAddress  Pointer to data to be written.
*              usSizeofWrite       Number of bytes to be written.
*      Output: Nothing
*       InOut: Nothing
*
**  Return   : Nothing (error is system error (progrum is abort)
*
**  Description: Write data in Dept File.
*==========================================================================
*/
VOID   Op_WriteDeptFile(ULONG  offulFileSeek, 
                        VOID   *pTableHeadAddress, 
                        ULONG usSizeofWrite)
{
    ULONG   ulActualPosition;

    if (!usSizeofWrite) {
        return;
    }

    (PifSeekFile(hsOpDeptFileHandle, offulFileSeek, &ulActualPosition) < 0)
    ? PifAbort(MODULE_OP_DEPT, FAULT_ERROR_FILE_SEEK)
    : PifWriteFile(hsOpDeptFileHandle, pTableHeadAddress, usSizeofWrite);
}

/*
*==========================================================================
**    Synopsis:    SHORT   Op_DeptIndex(OPDEPT_FILEHED *DeptFileHed, 
*                                       UCHAR         uchModeType, 
*                                       USHORT        usDeptNo, 
*                                       USHORT        *usParaBlockNo)
*                                                       
*       Input: *DeptFileHed    Pointer to the Dept file headder
*              uchModeType    Type of mode (search only or regist)
*              usDeptNo        Dept No to be searched.
*                                                       
*      Output: *usParaBlockNo       Storage location for Pamataer block No.
*       InOut: Nothing
*
**  Return   : OP_NEW_DEPT
*              OP_CHANGE_DEPT
*              OP_DEPT_FULL
*              OP_NOT_IN_FILE
*              (file error is system halt)
*
**  Description:  uchModeType
*                 OP_SEARCH_ONLY    Check Assign inputed Dept No.
*                 OP_REGIST         Add and change inputted Dept No.
*==========================================================================
*/
SHORT   Op_DeptIndex(OPDEPT_FILEHED *DeptFileHed, 
                 UCHAR         uchModeType, 
                 USHORT        usDeptNo, 
                 USHORT        *pusParaBlockNo)
{
	OPDEPT_INDEXENTRY    IndexEntry = { 0 };
    USHORT              offusSearchPoint;
    USHORT              usIndexBlockNo;
    SHORT               sStatus;
    USHORT              offusIndex;

    /* Make Index Table Size */

	if (DeptFileHed->usNumberOfMaxDept < 1 || DeptFileHed->offulOfIndexTbl == 0 || DeptFileHed->offulOfIndexTblBlk == 0)
		return OP_DEPT_FULL;

    IndexEntry.usDeptNo      = usDeptNo;

    if (DeptFileHed->usNumberOfResidentDept != 0x0000) {
        usIndexBlockNo = DeptFileHed->usNumberOfResidentDept;

    /* Search plu No. in Index Table */

        sStatus = Op_DeptSearchFile((USHORT)(DeptFileHed->offulOfIndexTbl),
                                    (USHORT)((DeptFileHed->usNumberOfResidentDept * 
                                   sizeof(OPDEPT_INDEXENTRY))),
                                    &offusSearchPoint, 
                                    &IndexEntry, 
                                    sizeof(IndexEntry), 
                                    (SHORT (FAR *)(VOID *, VOID *))Op_DeptCompIndex);

        if ((sStatus == OP_NOT_IN_FILE) && (uchModeType == OP_SEARCH_ONLY)) {
            return (OP_NOT_IN_FILE);
        }

        if (sStatus == OP_PERFORM) {  /* CHANGE DEPT */
            offusIndex = (USHORT)((offusSearchPoint - 
                        DeptFileHed->offulOfIndexTbl) / sizeof(IndexEntry));

            Op_ReadDeptFile(DeptFileHed->offulOfIndexTblBlk + 
                            (offusIndex * OP_DEPT_INDEXBLK_SIZE),
                            pusParaBlockNo,
                            OP_DEPT_INDEXBLK_SIZE);
            return (OP_CHANGE_DEPT);
        }

    /* BELOW IS NEW DEPT  ASSIGN */

    /* Check Paramater Table full and get paramater No. */

        if (Op_DeptEmpBlkGet(DeptFileHed, pusParaBlockNo) == OP_DEPT_FULL) {
            return(OP_DEPT_FULL);
        }

    /* make space 1 record in index table */

        Op_DeptInsertTable((ULONG)DeptFileHed->offulOfIndexTbl,
                        (USHORT)(DeptFileHed->usNumberOfResidentDept * 
                        sizeof(OPDEPT_INDEXENTRY)),
                       (USHORT)(offusSearchPoint - 
                       DeptFileHed->offulOfIndexTbl),
                        &IndexEntry, 
                       sizeof(OPDEPT_INDEXENTRY));

    /* make space 1 record in paramater block table */

        Op_DeptInsertTable((ULONG)DeptFileHed->offulOfIndexTblBlk, 
                       (USHORT)(DeptFileHed->usNumberOfResidentDept * 
                        OP_DEPT_INDEXBLK_SIZE),
                       (USHORT)(((offusSearchPoint - 
                        DeptFileHed->offulOfIndexTbl) / sizeof(OPDEPT_INDEXENTRY))
                         * OP_DEPT_INDEXBLK_SIZE),
                        pusParaBlockNo,
                        (USHORT)OP_DEPT_INDEXBLK_SIZE);

    /* ResidentDept +1 */

        DeptFileHed->usNumberOfResidentDept++;

        Op_WriteDeptFile(OP_FILE_HEAD_POSITION, 
                        DeptFileHed, sizeof(OPDEPT_FILEHED));
        return(OP_NEW_DEPT);

    /* Index Table size = 0 */

    } else {
        if (uchModeType == OP_SEARCH_ONLY) {
            return(OP_NOT_IN_FILE);
        }
        if (Op_DeptEmpBlkGet(DeptFileHed, pusParaBlockNo) == OP_DEPT_FULL) {
            return(OP_DEPT_FULL);
        }
        Op_WriteDeptFile(DeptFileHed->offulOfIndexTbl, 
                        &IndexEntry, sizeof(OPDEPT_INDEXENTRY));
        Op_WriteDeptFile(DeptFileHed->offulOfIndexTblBlk, 
                        pusParaBlockNo, OP_DEPT_INDEXBLK_SIZE);
        DeptFileHed->usNumberOfResidentDept++;
        Op_WriteDeptFile(OP_FILE_HEAD_POSITION, 
                        DeptFileHed, sizeof(OPDEPT_FILEHED));
        return(OP_NEW_DEPT);
    }
}

/*
*==========================================================================
**    Synopsis:   SHORT   Op_DeptSearchFile(USHORT  offusTableHeadAddress,
*                                           USHORT usTableSize, 
*                                           USHORT *poffusSearchPoint,
*                                           VOID   *pSearchData, 
*                                           USHORT usSearchDataSize, 
*                                           SHORT (*Comp)
*                                                 (VOID *pKey, VOID *pusCompKey))
*  
*       Input: *DeptFileHed
*              offulTableHeadAddress
*              ulTableSize
*              *pSearchPoint
*              *pSearchData
*              usSearchDataSize
*              (*Comp)(VOID *A, VOID *B)   A - B
*               
*      Output: Nothing
*       InOut: Nothing
*
**  Return   : OP_PERFORM
*              OP_NOT_IN_FILE
*              (file error is system halt)
*
**  Description: Search data on the Table in a File.        Saratoga
*==========================================================================
*/
SHORT   Op_DeptSearchFile(USHORT offusTableHeadAddress, 
                          USHORT usTableSize, 
                          USHORT *poffusSearchPoint,
                          VOID   *pSearchData, 
                          USHORT usSearchDataSize, 
                          SHORT (*Comp)(VOID *pKey, VOID *pusCompKey))
{
    SHORT sStatus;
    UCHAR auchKeyData[10];
    UCHAR auchWorkBuffer[OP_MAX_WORK_BUFFER];
    SHORT sCompStatus;
    USHORT offusTableTailAddress;
    USHORT offusTableSize;
    USHORT *pusHitPoint;
    USHORT usNumberOfEntry;             /* number of entry */

    sStatus = 0;

    /* make Table Tail address */

    offusTableTailAddress = (USHORT)(offusTableHeadAddress + usTableSize);
    offusTableSize = usTableSize;

    /* Search key in the file */

    while ( offusTableSize > OP_MAX_WORK_BUFFER ) {
        usNumberOfEntry = (USHORT)((offusTableSize / usSearchDataSize) / 2);
        *poffusSearchPoint = (USHORT)((usNumberOfEntry * usSearchDataSize) 
                             + offusTableHeadAddress);
        Op_ReadDeptFile( *poffusSearchPoint, auchKeyData, usSearchDataSize);
        if ((sCompStatus = (*Comp)(pSearchData, (VOID *)auchKeyData)) == 0) {   /* Saratoga */
            return(OP_PERFORM);
            break;
        } else if (sCompStatus > 0) {   /* pSearchData > auchKeyData */
            offusTableHeadAddress = *poffusSearchPoint;
            offusTableHeadAddress += usSearchDataSize;
        } else {
            offusTableTailAddress = *poffusSearchPoint;
        }
        offusTableSize = (USHORT)(offusTableTailAddress - 
                            offusTableHeadAddress);
    }

    Op_ReadDeptFile((ULONG)offusTableHeadAddress, 
                    auchWorkBuffer, offusTableSize);   /* offusTableSize != 0 */

    /* Search key in a table from the file  */

    usNumberOfEntry = (USHORT)(offusTableSize / usSearchDataSize);

    sStatus = Rfl_SpBsearch(pSearchData,                /* Saratoga */
                            usSearchDataSize, 
                            (VOID *)auchWorkBuffer, 
                            usNumberOfEntry,
                            (VOID *)&pusHitPoint,
                            Comp);              /* Saratoga */
    *poffusSearchPoint = (USHORT)((UCHAR *)pusHitPoint - auchWorkBuffer);
    *poffusSearchPoint += offusTableHeadAddress; /* offset in the file */
    if (sStatus == RFL_HIT) {
        return(OP_PERFORM);
    }
    return(OP_NOT_IN_FILE);
}

/*
*==========================================================================
**    Synopsis:    SHORT   Op_DeptCompIndex(USHORT *usDeptNo, USHORT *pusHitPoint)
*
*       Input:    *usDeptNo            Key
*                 *pusHitPoint        Data to be compare.
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   0: usDeptNo = pusHitPoint
*               0 <: usDeptNo < pusHitPoint  
*               0 >: usDeptNo > pusHitPoint 
*
**  Description: Compare index entry                        Saratoga
*==========================================================================
*/
SHORT   Op_DeptCompIndex(OPDEPT_INDEXENTRY *Key, OPDEPT_INDEXENTRY *pusHitPoint)
{
    SHORT  sStatus;

    sStatus = (SHORT)(Key->usDeptNo - pusHitPoint->usDeptNo);
    return((USHORT)sStatus);            
}

/*
*==========================================================================
**    Synopsis:   SHORT   Op_DeptEmpBlkGet(OPDEPT_FILEHED *DeptFileHed, 
*                                          USHORT        *pusParaBlockNo)
*
*       Input: *DeptFileHed
*              *pusParaBlockNo
*      Output: Nothing
*       InOut: Nothing
*
**  Return   : OP_PERFORM
*              OP_DEPT_FULL
*
**  Description: Let Empty Empty Block No.
*==========================================================================
*/
SHORT   Op_DeptEmpBlkGet( OPDEPT_FILEHED *DeptFileHed, USHORT *pusParaBlockNo )
{
    UCHAR auchParaEmpBlk[OPDEPT_PARA_EMPTY_TABLE_SIZE];
    SHORT sI;
    SHORT sJ = 0;
    SHORT sAnswer;

    Op_ReadDeptFile((ULONG)DeptFileHed->offulOfParaEmpTbl, 
                   auchParaEmpBlk, OPDEPT_PARA_EMPTY_TABLE_SIZE);
    sAnswer = 0;
    for (sI = 0; OPDEPT_PARA_EMPTY_TABLE_SIZE > sI; sI++ ) {
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
    if (sAnswer == 1) {
        *pusParaBlockNo = (USHORT)((sI * 8) + sJ + 1);
        if (*pusParaBlockNo > DeptFileHed->usNumberOfMaxDept) {
            return(OP_DEPT_FULL);
        }
        Op_WriteDeptFile((ULONG)DeptFileHed->offulOfParaEmpTbl, auchParaEmpBlk, OPDEPT_PARA_EMPTY_TABLE_SIZE);
        return(OP_PERFORM);
    }
    return(OP_DEPT_FULL);
}

/*
*==========================================================================
**    Synopsis:   VOID    Op_DeptInsertTable(ULONG  offusTableHeadAddress, 
*                                            USHORT usTableSize
*                                            USHORT offusInsertPoint, 
*                                            VOID   *pInsertData, 
*                                            USHORT usInsertDataSize)
*
*       Input:  offusTableHeadAddress  Table head address in a file.
*               usTableSize            Table size in a file.
*               offInsertPoint         Table Insert Point (offset from tabel head address) in a file.
*               *pInsertData           Inserted Data
*               usInsertDataSize       Insert Data size
*               
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :  Nothing
*                 
*
**  Description: Insert data to Table in a File.
*==========================================================================
*/
VOID   Op_DeptInsertTable( ULONG  offusTableHeadAddress, 
                           USHORT usTableSize,
                           USHORT offusInsertPoint, 
                           VOID   *pInsertData, 
                           USHORT usInsertDataSize)
{
    UCHAR   auchWorkBuffer[OP_MAX_WORK_BUFFER];  /* work buffer */
    USHORT  offusTailInFile;    /* Table Tail Address */
    USHORT  usMoveByte;         /* moving byte save area */

    if (usTableSize <= offusInsertPoint) {
        Op_WriteDeptFile( offusInsertPoint + offusTableHeadAddress
                         , pInsertData, usInsertDataSize );  /* insert data */
        return;
    }

    /* Calculate moving byte */
    
    usMoveByte = (USHORT)(usTableSize - offusInsertPoint);
    offusTailInFile = (USHORT)(offusTableHeadAddress + usTableSize);

    /* Move moving byte in a file */

    while ( usMoveByte > OP_MAX_WORK_BUFFER ) {
        offusTailInFile -= OP_MAX_WORK_BUFFER;
        Op_ReadDeptFile( offusTailInFile
                        , auchWorkBuffer, OP_MAX_WORK_BUFFER);
        Op_WriteDeptFile(offusTailInFile + usInsertDataSize, 
                        auchWorkBuffer, OP_MAX_WORK_BUFFER);
        usMoveByte -= OP_MAX_WORK_BUFFER;
    }

    if (usMoveByte != 0) {
        Op_ReadDeptFile(offusTableHeadAddress + offusInsertPoint, 
                       auchWorkBuffer, usMoveByte);
        Op_WriteDeptFile((offusTableHeadAddress + offusInsertPoint + 
                         usInsertDataSize), 
                         auchWorkBuffer, usMoveByte);
    }

    /* insert data */

    Op_WriteDeptFile(offusTableHeadAddress + offusInsertPoint, 
                    pInsertData, usInsertDataSize );
}

/*
*==========================================================================
**    Synopsis:    VOID   OP_DeptParaWrite(OP_FILEHED       *DeptFileHed, 
*                                          USHORT           usParaBlockNo, 
*                                          OPDEPT_PARAENTRY *ParaEntry)
*
*       Input: *DeptFileHed    Pointer to the Dept file headder.
*              culCounter      Counter(first entry is 0 , next is not to be touched.
*              *usParaBlkNo    Paramater table block no.
*              *usDeptNo       Dept No to be searched.
*              
*      Output: Nothing
*       InOut: Nothing
*
**  Return   : Nothing
*
**  Description: Write Paramater inputted plu to Dept file .
*==========================================================================
*/
VOID   Op_DeptParaWrite(OPDEPT_FILEHED   *DeptFileHed, 
                       USHORT            usParaBlockNo, 
                       OPDEPT_PARAENTRY  *ParaEntry)
{
    Op_WriteDeptFile(DeptFileHed->offulOfParaTbl + 
                     (ULONG)(sizeof(OPDEPT_PARAENTRY) * 
                     ((ULONG)(usParaBlockNo - 1))),
                     ParaEntry, 
                     sizeof(OPDEPT_PARAENTRY));
}

/*
*==========================================================================
**    Synopsis:    SHORT   Op_WriteCpnFile(ULONG  offulFileSeek, 
*                                          VOID   *pTableHeadAddress, 
*                                          USHORT usSizeofWrite)
*
*       Input: offulFileSeek       Number of bytes from the biginning of the Coupon file.
*              *pTableHeadAddress  Pointer to data to be written.
*              usSizeofWrite       Number of bytes to be written.
*      Output: Nothing
*       InOut: Nothing
*
**  Return   : Nothing (error is system error (progrum is abort)
*
**  Description: Write data in Combination Coupon File.
*==========================================================================
*/
VOID   Op_WriteCpnFile(ULONG  offulFileSeek, 
                       VOID   *pTableHeadAddress, 
                       ULONG usSizeofWrite)
{
    ULONG   ulActualPosition;

    if (!usSizeofWrite) {
        return;
    }

    (PifSeekFile(hsOpCpnFileHandle, offulFileSeek, &ulActualPosition) < 0)
    ? PifAbort(MODULE_OP_CPN, FAULT_ERROR_FILE_SEEK)
    : PifWriteFile(hsOpCpnFileHandle, pTableHeadAddress, usSizeofWrite);
}
/*
*==========================================================================
**    Synopsis:    SHORT   Op_WriteCstrFile(ULONG  offulFileSeek, 
*                                           VOID   *pTableHeadAddress, 
*                                           USHORT usSizeofWrite)
*
*       Input: offulFileSeek       Number of bytes from the biginning of the C.String file.
*              *pTableHeadAddress  Pointer to data to be written.
*              usSizeofWrite       Number of bytes to be written.
*      Output: Nothing
*       InOut: Nothing
*
**  Return   : Nothing (error is system error (progrum is abort)
*
**  Description: Write data in Control String File.
*==========================================================================
*/
VOID   Op_WriteCstrFile(ULONG  offulFileSeek, 
                        VOID   *pTableHeadAddress, 
                        ULONG usSizeofWrite)
{
    ULONG   ulActualPosition;

    if (!usSizeofWrite) {
        return;
    }

    (PifSeekFile(hsOpCstrFileHandle, offulFileSeek, &ulActualPosition) < 0)
    ? PifAbort(MODULE_OP_CSTR, FAULT_ERROR_FILE_SEEK)
    : PifWriteFile(hsOpCstrFileHandle, pTableHeadAddress, usSizeofWrite);
}

/*
*==========================================================================
**    Synopsis:    VOID    Op_ClosePluFileReleaseSem(VOID)
*
*       Input: Nothing
*      Output: Nothing
*       InOut: Nothing
*
**  Return   : Nothing (error is system error (progrum is abort))
*              
**  Description: Close PLU file and Release semaphore
*==========================================================================
*/
VOID    Op_ClosePluFileReleaseSem(VOID)
{
    PifCloseFile(hsOpPluFileHandle);
    PifReleaseSem(husOpSem);
}

/*
*==========================================================================
**    Synopsis:    VOID    Op_CloseDeptFileReleaseSem(VOID)
*
*       Input: Nothing
*      Output: Nothing
*       InOut: Nothing
*
**  Return   : Nothing (error is system error (progrum is abort))
*
**  Description: Close DEPT file and Release semaphore
*==========================================================================
*/
VOID    Op_CloseDeptFileReleaseSem(VOID)
{
    PifCloseFile(hsOpDeptFileHandle);
    PifReleaseSem(husOpSem);
}

/*
*==========================================================================
**    Synopsis:    VOID    Op_CloseCpnFileReleaseSem(VOID)
*
*       Input: Nothing
*      Output: Nothing
*       InOut: Nothing
*
**  Return   : Nothing (error is system error (progrum is abort))
*
**  Description: Close Combination Coupon file and Release semaphore
*==========================================================================
*/
VOID    Op_CloseCpnFileReleaseSem(VOID)
{
    PifCloseFile(hsOpCpnFileHandle);
    PifReleaseSem(husOpSem);
}

/*
*==========================================================================
**    Synopsis:    VOID    Op_CloseCstrFileReleaseSem(VOID)
*
*       Input: Nothing
*      Output: Nothing
*       InOut: Nothing
*
**  Return   : Nothing (error is system error (progrum is abort))
*
**  Description: Close Control String file and Release semaphore
*==========================================================================
*/
VOID    Op_CloseCstrFileReleaseSem(VOID)
{
    PifCloseFile(hsOpCstrFileHandle);
    PifReleaseSem(husOpSem);
}

/*
*==========================================================================
**    Synopsis:    Op_ReadCpnFile(ULONG  offulFileSeek, 
*                                 VOID   *pTableHeadAddress, 
*                                 USHORT usSizeofRead);
*
*       Input: offulFileSeek       Number of bytes from the biginning of the coupon file.
*              cusSizeofRead       Number of bytes to be read.
*      Output: *pTableHeadAddress  Storage location of items to be read.
*       InOut: Nothing
*
**  Return   : OP_PERFORM
*              OP_NO_READ_SIZE                  Read size is 0
*              (File error is system holt)
*
**  Description: Read data in Combination Coupon file.          
*==========================================================================
*/
SHORT   Op_ReadCpnFile(ULONG  offulFileSeek, 
                       VOID   *pTableHeadAddress, 
                       ULONG usSizeofRead)
{
    ULONG   ulActualPosition;

    if (usSizeofRead == 0X00) {
        return (OP_NO_READ_SIZE);
    }
    
    if ((PifSeekFile(hsOpCpnFileHandle, offulFileSeek, &ulActualPosition)) < 0) {
        PifAbort(MODULE_OP_CPN, FAULT_ERROR_FILE_SEEK);
    }

    if (PifReadFile(hsOpCpnFileHandle, pTableHeadAddress, usSizeofRead) != usSizeofRead) {
		// If there is an error reading the file then look to see if the offset indicates
		// we are reading the header portion of the file. If not then error exit.
		// If we are reading the header then return a header of all zeros.
		// This allows us to continue in the face of some kinds of errors which are
		// recoverable.
		if (offulFileSeek > 0) {
			PifAbort(MODULE_OP_CPN, FAULT_ERROR_FILE_READ);
		}
		else {
			memset(pTableHeadAddress, 0, usSizeofRead);
		}
    }
    return(OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    Op_ReadCstrFile(ULONG  offulFileSeek, 
*                                  VOID   *pTableHeadAddress, 
*                                  USHORT usSizeofRead);
*
*       Input: offulFileSeek       Number of bytes from the biginning of the coupon file.
*              cusSizeofRead       Number of bytes to be read.
*      Output: *pTableHeadAddress  Storage location of items to be read.
*       InOut: Nothing
*
**  Return   : OP_PERFORM
*              OP_NO_READ_SIZE                  Read size is 0
*              (File error is system holt)
*
**  Description: Read data in Control String file.          
*==========================================================================
*/
SHORT   Op_ReadCstrFile(ULONG  offulFileSeek, 
                        VOID   *pTableHeadAddress, 
                        ULONG usSizeofRead)
{
    ULONG   ulActualPosition;

    if (usSizeofRead == 0X00) {
        return (OP_NO_READ_SIZE);
    }
    
    if ((PifSeekFile(hsOpCstrFileHandle, offulFileSeek, 
                        &ulActualPosition)) < 0) {
        PifAbort(MODULE_OP_CSTR, FAULT_ERROR_FILE_SEEK);
    }

    if (PifReadFile(hsOpCstrFileHandle, 
                    pTableHeadAddress, usSizeofRead) != usSizeofRead) {
        PifAbort(MODULE_OP_CSTR, FAULT_ERROR_FILE_READ);
    }
    return(OP_PERFORM);
}

/*                 
*==========================================================================
**    Synopsis:   SHORT  Op_PpiOpenFile(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description:  Open "PARAPPI" file.
*==========================================================================
*/
SHORT  Op_PpiOpenFile(VOID)
{
    hsOpPpiFileHandle = PifOpenFile((WCHAR FAR *)auchOP_PPI, 
                                    (CHAR FAR *)auchOLD_FILE_READ_WRITE);
    return (hsOpPpiFileHandle);
}

/*
*==========================================================================
**    Synopsis:    VOID    Op_ClosePpiFileReleaseSem(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing 
*
**  Description:
*               Close Promotioanl Pricing Item file and Release semaphore
*==========================================================================
*/
VOID    Op_ClosePpiFileReleaseSem(VOID)
{
    PifCloseFile(hsOpPpiFileHandle);
    PifReleaseSem(husOpSem);
}

/*
*==========================================================================
**    Synopsis:    Op_ReadPpiFile(ULONG  offulFileSeek,
*                                 VOID   *pTableHeadAddress,
*                                 USHORT usSizeofRead)
*
*       Input:  offulFileSeek           Number of bytes from the biginning of
*                                       the Promotional Pricing Item file.
*               cusSizeofRead           Number of bytes to be read.
*      Output:  *pTableHeadAddress      Storage location of items to be read.
*       InOut:  Nothing
*
**  Return    : CAS_PERFORM
*               CAS_NO_READ_SIZE                  Read size is 0
*               (File error is system holt)
*
**  Description: Read data in Promotional Pricing file.
*==========================================================================
*/
SHORT   Op_ReadPpiFile(ULONG offulFileSeek,
                       VOID  *pTableHeadAddress,
                       ULONG usSizeofRead)
{
    ULONG   ulActualPosition;

    if (usSizeofRead == 0X00) {
        return (OP_NO_READ_SIZE);
    }
    
    if ((PifSeekFile(hsOpPpiFileHandle, offulFileSeek, &ulActualPosition)) < 0) {
        Op_PpiAbort(FAULT_ERROR_FILE_SEEK);
    }

    if (PifReadFile(hsOpPpiFileHandle, 
                    pTableHeadAddress, usSizeofRead) != usSizeofRead) {
        Op_PpiAbort(FAULT_ERROR_FILE_READ);
    }
    return(OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   Op_WritePpiFile(ULONG  offulFileSeek,
*                                          VOID   *pTableHeadAddress, 
*                                          USHORT usSizeofWrite)
*
*       Input:   offulFileSeek           Number of bytes from the biginning of the 
*                                        Promotional Pricing Item file.
*                *pTableHeadAddress      Pointer to data to be written.
*                usSizeofWrite           Number of bytes to be written.
*      Output:   Nothing
*       InOut:   Nothing
*
**  Return    :  Nothing (error is system error (progrum is abort)
*
**  Description: Write data in Promotional Pricing Item File.
*==========================================================================
*/
VOID   Op_WritePpiFile(ULONG offulFileSeek,
                       VOID  *pTableHeadAddress,
                       ULONG usSizeofWrite)
{
    ULONG   ulActualPosition;

    if (!usSizeofWrite) {
        return;
    }
    if ( PifSeekFile(hsOpPpiFileHandle, offulFileSeek, &ulActualPosition) < 0) {
        Op_PpiAbort(FAULT_ERROR_FILE_SEEK);
    }
    PifWriteFile(hsOpPpiFileHandle,
                 pTableHeadAddress, 
                 usSizeofWrite);
}

/*                 
*==========================================================================
**    Synopsis:    VOID   Op_PpiAbort(USHORT usCode)
*
*       Input:    usCode
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description:  System halt.
*==========================================================================
*/
VOID  Op_PpiAbort(USHORT usCode)
{
    PifAbort(MODULE_OP_PPI, usCode);
}

/*                 
*==========================================================================
**    Synopsis:   SHORT  Op_MldOpenFile(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description:  Open "PARAMLD" file. V3.3
*==========================================================================
*/
SHORT  Op_MldOpenFile(VOID)
{
    hsOpMldFileHandle = PifOpenFile((WCHAR FAR *)auchOP_MLD, 
                                    (CHAR FAR *)auchOLD_FILE_READ_WRITE);
    return (hsOpMldFileHandle);
}

/*
*==========================================================================
**    Synopsis:    VOID    Op_CloseMldFileReleaseSem(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing 
*
**  Description:
*               Close MLD Mnemonics file and Release semaphore V3.3
*==========================================================================
*/
VOID    Op_CloseMldFileReleaseSem(VOID)
{
    PifCloseFile(hsOpMldFileHandle);
    PifReleaseSem(husOpSem);
}

/*
*==========================================================================
**    Synopsis:    Op_ReadMldFile(ULONG  offulFileSeek,
*                                 VOID   *pTableHeadAddress,
*                                 USHORT usSizeofRead)
*
*       Input:  offulFileSeek           Number of bytes from the biginning of
*                                       the Mld Mnemonics file.
*               cusSizeofRead           Number of bytes to be read.
*      Output:  *pTableHeadAddress      Storage location of items to be read.
*       InOut:  Nothing
*
**  Return    : CAS_PERFORM
*               CAS_NO_READ_SIZE                  Read size is 0
*               (File error is system holt)
*
**  Description: Read data in Mld Mnemonics file. V3.3
*==========================================================================
*/
SHORT   Op_ReadMldFile(ULONG offulFileSeek,
                       VOID  *pTableHeadAddress,
                       ULONG usSizeofRead)
{
    ULONG   ulActualPosition;

    if (usSizeofRead == 0X00) {
        return (OP_NO_READ_SIZE);
    }
    
    if ((PifSeekFile(hsOpMldFileHandle, offulFileSeek, &ulActualPosition)) < 0) {
        Op_MldAbort(FAULT_ERROR_FILE_SEEK);
    }

    PifReadFile(hsOpMldFileHandle, pTableHeadAddress, usSizeofRead);
    return(OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   Op_WriteMldFile(ULONG  offulFileSeek,
*                                          VOID   *pTableHeadAddress, 
*                                          USHORT usSizeofWrite)
*
*       Input:   offulFileSeek           Number of bytes from the biginning of the 
*                                        Mld Mnemonics file.
*                *pTableHeadAddress      Pointer to data to be written.
*                usSizeofWrite           Number of bytes to be written.
*      Output:   Nothing
*       InOut:   Nothing
*
**  Return    :  Nothing (error is system error (progrum is abort)
*
**  Description: Write data in Mld Mnemonics File. V3.3
*==========================================================================
*/
VOID   Op_WriteMldFile(ULONG offulFileSeek,
                       VOID  *pTableHeadAddress,
                       ULONG usSizeofWrite)
{
    ULONG   ulActualPosition;

    if (!usSizeofWrite) {
        return;
    }
    if ( PifSeekFile(hsOpMldFileHandle, offulFileSeek, &ulActualPosition) < 0) {
        Op_MldAbort(FAULT_ERROR_FILE_SEEK);
    }
    PifWriteFile(hsOpMldFileHandle,
                 pTableHeadAddress, 
                 usSizeofWrite);
}

/*                 
*==========================================================================
**    Synopsis:    VOID   Op_MldAbort(USHORT usCode)
*
*       Input:    usCode
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description:  System halt. V3.3
*==========================================================================
*/
VOID  Op_MldAbort(USHORT usCode)
{
    PifAbort(MODULE_OP_MLD, usCode);
}

/*
*==========================================================================
**    Synopsis:    SHORT   Op_DeptIndexDel(OP_FILEHED *DeptFileHed, 
*                                      USHORT     usDeptNo)
*
*       Input: *DeptFileHed       Pointer to the Dept file headder.
*              usDeptNo           Dept No to be written.
*      Output: Nothing
*       InOut: Nothing
*
**  Return   : OP_PERFORM
*              OP_NOT_IN_FILE
*              (file error is system halt)
*
**  Description: Delete inputted DEPT No. in Dept File.
*==========================================================================
*/
SHORT   Op_DeptIndexDel(OPDEPT_FILEHED *DeptFileHed, 
                        USHORT        usDeptNo) 
{

    OPDEPT_INDEXENTRY    IndexEntry;
    USHORT               offusSearchPoint;
    USHORT               usParaBlockNo;

    /* Make Index Table Size */

    if (DeptFileHed->usNumberOfResidentDept == 0x0000){
        return(OP_NOT_IN_FILE);
    }
    IndexEntry.usDeptNo      = usDeptNo;

    if ((Op_DeptSearchFile((USHORT)DeptFileHed->offulOfIndexTbl,
                           (USHORT)(DeptFileHed->usNumberOfResidentDept *
                                     sizeof(OPDEPT_INDEXENTRY)),
                           &offusSearchPoint, 
                           &IndexEntry, 
                           sizeof(IndexEntry), 
                            (SHORT (FAR *)(VOID *, VOID *))Op_DeptCompIndex))

                           == OP_NOT_IN_FILE) {
        return(OP_NOT_IN_FILE);
     }
    
    /* Get Paramater Block No */

    Op_ReadDeptFile((ULONG)(DeptFileHed->offulOfIndexTblBlk +
                    (((offusSearchPoint - DeptFileHed->offulOfIndexTbl) / 
                    sizeof(OPDEPT_INDEXENTRY)) * OP_DEPT_INDEXBLK_SIZE)),
                    &usParaBlockNo,
                    OP_DEPT_INDEXBLK_SIZE);

    /* Delete space 1 record in index table */

    Op_DeptDeleteTable((USHORT) DeptFileHed->offulOfIndexTbl,
                       (USHORT)(DeptFileHed->usNumberOfResidentDept * 
                                 sizeof(OPDEPT_INDEXENTRY)),
                       (USHORT)(offusSearchPoint - DeptFileHed->offulOfIndexTbl), 
                       sizeof(OPDEPT_INDEXENTRY));

    /* Delete space 1 record in paramater block table */

    Op_DeptDeleteTable( (USHORT)DeptFileHed->offulOfIndexTblBlk, 
                        (USHORT)(DeptFileHed->usNumberOfResidentDept * 
                        OP_DEPT_INDEXBLK_SIZE),
                        (USHORT)(((offusSearchPoint - 
                        DeptFileHed->offulOfIndexTbl) / sizeof(OPDEPT_INDEXENTRY)) * 
                        OP_DEPT_INDEXBLK_SIZE),
                        OP_DEPT_INDEXBLK_SIZE);

    /* Empty para empty table */

    Op_DeptBlkEmp( DeptFileHed, usParaBlockNo );

    /* ResidentDept +1 */

    DeptFileHed->usNumberOfResidentDept--;

    /* Save Index Table */

    Op_WriteDeptFile(OP_FILE_HEAD_POSITION, DeptFileHed, 
                    sizeof(OPDEPT_FILEHED));
    return(OP_PERFORM);
}
/*
*==========================================================================
**    Synopsis:    VOID   Op_DeptNoGet(OP_FILEHED *DeptFileHed, 
*                                  ULONG      culCounter, 
*                                  USHORT     *usDeptNo, 
*                                  USHORT     *usParaBlockNo)
*
*       Input: *DeptFileHed       Pointer to the Dept file headder.
*              culCounter         Counter That is used in this function.
*              *usParaBlkNo       Pointer to the Paramater block No to be written.
*              
*      Output: *usDeptNo          Dept No to be searched.
*       InOut: Nothing
*
**  Return   : OP_PERFORM
*              OP_NOT_IN_FILE
*              (file error is system halt)
*
**  Description: Get Dept No. in Dept File.
*==========================================================================
*/
VOID   Op_DeptNoGet(OPDEPT_FILEHED *OpDeptFileHed, 
                    ULONG          culCounter, 
                    USHORT         *usDeptNo, 
                    USHORT         *usParaBlockNo )
{
    OPDEPT_INDEXENTRY    IndexEntry;

    Op_ReadDeptFile(OpDeptFileHed->offulOfIndexTbl + 
                    (ULONG)(culCounter * sizeof(OPDEPT_INDEXENTRY)),
                    &IndexEntry, 
                    sizeof(OPDEPT_INDEXENTRY));
    *usDeptNo = IndexEntry.usDeptNo;  /* output condtion */
    Op_ReadDeptFile(OpDeptFileHed->offulOfIndexTblBlk + 
                    (ULONG)(culCounter * OP_DEPT_INDEXBLK_SIZE),
                    &usParaBlockNo, 
                    OP_DEPT_INDEXBLK_SIZE);
}

/*
*==========================================================================
**    Synopsis:    VOID   Op_DeptParaRead(OP_FILEHED      *DeptFileHed, 
*                                         USHORT          usParaBlockNo,
*                                         OPDEPT_PARAENTRY *ParaEntry)
*
*       Input:  *DeptFileHed   Pointer to the Dept file headder.
*               culCounter     Counter (first entry is 0, next is not to be touched)
*               *usParaBlkNo   Paramater table's block No.
*               *usDeptNo      Dept No to be searched.
*               
*      Output:  *ParaEntry     Strage location for Paramater entry.
*       InOut:  Nothing
*
**  Return   : Nothing
*               (file error is system halt)
*
**  Description: Read paramater of inputting Dept No. by usParaBlkNo.    
*==========================================================================
*/
VOID   Op_DeptParaRead(OPDEPT_FILEHED   *DeptFileHed, 
                       USHORT           usParaBlockNo, 
                       OPDEPT_PARAENTRY *ParaEntry)
{
    Op_ReadDeptFile(DeptFileHed->offulOfParaTbl + (sizeof(OPDEPT_PARAENTRY) * ((ULONG)(usParaBlockNo - 1)) ),
                    ParaEntry, sizeof(OPDEPT_PARAENTRY));
}

/*
*==========================================================================
**    Synopsis:    SHORT   Op_GetDept(OPDEPT_FILEHED *pDeptFileHed, 
*                                     ULONG          culCounter, 
*                                     DEPTIF         *pParaEntry);
*
*       Input: *pDeptFileHed      Point to the Dept File headder.
*              culCounter         Counter(First entry is 0, 
*                                        next is not to be touched)
*      Output: *pParaEntry        Storage location for Dept if data.
*       InOut: Nothing
*
**  Return   : OP_PERFORM
*              OP_EOF
*              (file error is system halt)
*
**  Description: Get Dept No in order of the number.
*==========================================================================
*/
SHORT   Op_GetDept(OPDEPT_FILEHED *pDeptFileHed, 
                   ULONG           culCounter, 
                   DEPTIF          *pParaEntry)
{
    USHORT              usParaBlkNo;
	OPDEPT_INDEXENTRY   DeptIndex = { 0 };

	if (pDeptFileHed->usNumberOfMaxDept < 1 || pDeptFileHed->offulOfIndexTbl == 0 || pDeptFileHed->offulOfIndexTblBlk == 0)
		return OP_EOF;

	if ((ULONG)pDeptFileHed->usNumberOfResidentDept <= culCounter) {
        return(OP_EOF);    
    }

    /*  get Dept no in index table */
    Op_ReadDeptFile(pDeptFileHed->offulOfIndexTbl + (culCounter * sizeof(OPDEPT_INDEXENTRY)), 
                     &DeptIndex, sizeof(OPDEPT_INDEXENTRY));
    pParaEntry->usDeptNo = DeptIndex.usDeptNo;              /* make output condition */

    /*  get Dept no in paramater block no */
    Op_ReadDeptFile(pDeptFileHed->offulOfIndexTblBlk + (culCounter * OP_DEPT_INDEXBLK_SIZE), 
                    &usParaBlkNo, OP_DEPT_INDEXBLK_SIZE);
    /*  get plu paramater in paramater table */
    Op_ReadDeptFile(pDeptFileHed->offulOfParaTbl + ((ULONG)(usParaBlkNo - 1) * sizeof(OPDEPT_PARAENTRY)),
                    &pParaEntry->ParaDept, sizeof(OPDEPT_PARAENTRY));
    return(OP_PERFORM);
}


/*
*==========================================================================
**    Synopsis:   VOID   Op_DeptDeleteTable(USHORT offusTableHeadAddress, 
*                                           USHORT usTableSize
*                                           USHORT offusDeletePoint, 
*                                           USHORT usDeleteSize, 
*
*       Input: offusTableHeadAddress     Table head address in a file.
*              usTableSize               Table size in a file.
*              offusDeletePoint
*              usDeleteSize
*      Output: Nothing
*       InOut: Nothing
*
**  Return   : Nothing
*              (file error is system halt)
*
**  Description: Delete data from Index Table in a File.
*==========================================================================
*/
VOID    Op_DeptDeleteTable(USHORT offusTableHeadAddress, 
                           USHORT usTableSize,
                           USHORT offusDeletePoint, 
                           USHORT usDeleteDataSize)
{ 
    UCHAR   auchWorkBuffer[OP_MAX_WORK_BUFFER];  /* work buffer */
    USHORT  usMoveByte;     /* moving byte save area */
    
    /* Calculate moving byte */
    usMoveByte = (USHORT)(usTableSize - offusDeletePoint - usDeleteDataSize);

    /* Move moving byte in a file */
    offusDeletePoint += offusTableHeadAddress;

    while ( usMoveByte > OP_MAX_WORK_BUFFER ) {
        Op_ReadDeptFile(offusDeletePoint + usDeleteDataSize, 
                       auchWorkBuffer, OP_MAX_WORK_BUFFER);
        Op_WriteDeptFile(offusDeletePoint, 
                        auchWorkBuffer, OP_MAX_WORK_BUFFER);
        usMoveByte -= OP_MAX_WORK_BUFFER;
        offusDeletePoint += OP_MAX_WORK_BUFFER;
    }

    if (usMoveByte != 0) {
        Op_ReadDeptFile(offusDeletePoint + usDeleteDataSize, 
                       auchWorkBuffer, usMoveByte);
        Op_WriteDeptFile(offusDeletePoint, auchWorkBuffer, usMoveByte);
    }
}



/*
*==========================================================================
**    Synopsis:   VOID  Op_DeptBlkEmp(OPDEPT_FILEHED *DeptFileHed, 
*                                     USHORT        usParaBlockNo )
*
*       Input: *DeptFileHed
*              usParaBlockNo
*      Output: Nothing
*       InOut: Nothing
*
**  Return:     Nothing
*
**  Description: Get Empty Block No.
*==========================================================================
*/
VOID   Op_DeptBlkEmp( OPDEPT_FILEHED *DeptFileHed, USHORT usParaBlockNo )
{
    UCHAR auchParaEmpBlk[OPDEPT_PARA_EMPTY_TABLE_SIZE];
    SHORT sI;
    SHORT sJ;

    Op_ReadDeptFile((ULONG)DeptFileHed->offulOfParaEmpTbl, 
                    auchParaEmpBlk, OPDEPT_PARA_EMPTY_TABLE_SIZE);
    sI = (SHORT)((usParaBlockNo - 1) / 8);
    sJ = (SHORT)((usParaBlockNo - 1) % 8);

    auchParaEmpBlk[sI] &= ~( 0x01 << sJ );

    Op_WriteDeptFile((ULONG)DeptFileHed->offulOfParaEmpTbl, 
                    auchParaEmpBlk, OPDEPT_PARA_EMPTY_TABLE_SIZE);
}

/*** NCR2172 End   ***/

/*
*==========================================================================
**    Synopsis:    VOID    Op_CloseOepFileReleaseSem(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing 
*
**  Description:
*               Close OEP file and Release semaphore
*==========================================================================
*/
VOID    Op_CloseOepFileReleaseSem(VOID)
{
    PifCloseFile(hsOpOepFileHandle);
    PifReleaseSem(husOpSem);
}


/*====== End of Source ========*/