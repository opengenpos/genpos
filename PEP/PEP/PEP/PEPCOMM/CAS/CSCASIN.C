/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993-1998      ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server CASHIER module, Program List                        
*   Category           : C/S CASHIER module, NCR2170 US GENERAL PURPOSE APPLICATION
*   Program Name       : CSCASIN.C                                            
*  ------------------------------------------------------------------------  
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.              
*   Memory Model       : Medium Model
*   Options            : /c /AM /G1s /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*    Abstract           :The provided function names are as floolws.
*
*       Cas_GetHeader();             * Get Header *
*       Cas_PutHeader();             * Put Header *
*       Cas_Index();                 * Get a designate cashier   *
*       Cas_IndexDel();              * Delete a designate cahier *
*       Cas_ParaRead();              * Read   a cashier parameter *
*       Cas_ParaWrite();             * Write  a cashier parameter *
*       Cas_EmpBlkGet();             * Get empty block *
*       Cas_BlkEmp();                * Clesr Empty Block *                        
*       Cas_IndLockCheck();          * Check individual lock *
*       Cas_LockCheck();             * Ckeck condition *
*       Cas_IndSignInCheck();        * Check individual signin *
*       Cas_CompIndex();             * Compare cashier no *
*       Cas_WriteFile();             * Write file *
*       Cas_ReadFile();              * Read  file *
*       Cas_CloseFileReleaseSem();   * Close file and semaphore *
*       Cas_ChangeFlag();            * Set or Reset Flag 
*
*  ------------------------------------------------------------------------
*    Update Histories  :
*    Date     :Ver.Rev :NAME       :Description
*    Feb-15-93:00.00.01:H.YAMAGUCHI:Initial
*    Jun-01-93:00.00.02:H.YAMAGUCHI:Modified
*    Sep-03-98:03.03.00:A.Mitsui   :V3.3 Modified
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
#include    <paraequ.h>
#include    "cscas.h"
#include    <rfl.h>
#include    <csstbfcc.h>
/*#include    <csstbstb.h>*/
#include    <appllog.h>
#include    "cscasin.h"
#include    "mypifdefs.h"

/*
*==========================================================================
**    Synopsis:   VOID  Cas_GetHeader(CAS_FILEHED *pCasFileHed)
*
*       Input:    Nothing
*      Output:    *pCasFileHed
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description: Get Header in File.
*==========================================================================
*/
VOID   Cas_GetHeader(CAS_FILEHED *pCasFileHed)
{
    Cas_ReadFile(CAS_FILE_HED_POSITION, pCasFileHed, sizeof(CAS_FILEHED));
}

/*
*==========================================================================
**    Synopsis:   VOID  Cas_PutHeader(CAS_FILEHED *pCasFileHed)
*
*       Input:    *pCasFileHed
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description: Put Header in File
*==========================================================================
*/
VOID   Cas_PutHeader(CAS_FILEHED *pCasFileHed)
{
    Cas_WriteFile(CAS_FILE_HED_POSITION, pCasFileHed, sizeof(CAS_FILEHED));
}

/*
*==========================================================================
**  Synopsis:    SHORT   Cas_Index(CAS_FILEHED *pCasFileHed, 
*                                  USHORT      usIndexMode,
*                                  USHORT      usCashierNo, 
*                                  USHORT      *usParaBlockNo)
*
*       Input:  *pCasFileHed     file header
*               usCashierNo      cashier no.
*               usIndexMode      Search mode
*      Output:  *usParaBlockNo  paramater block no.
*       InOut:  Nothing
*
**  Return   :
*            Normal End:    CAS_RESIDENT (At CAS_SEARCH_ONLY)
*                           CAS_REGISTED
*          Abnormal End:    CAS_NOT_IN_FILE
*                           CAS_CASHIER_FULL
*   
**  Description: Check if Cashier exsisted
*==========================================================================
*/
SHORT   Cas_Index(CAS_FILEHED *pCasFileHed, USHORT usIndexMode, ULONG ulCashierNo, USHORT *usParaBlockNo)
{
	CAS_INDEXENTRY aWorkRcvBuffer[CAS_NUMBER_OF_MAX_CASHIER + 1] = { 0 };  /* index table */
	CAS_INDEXENTRY CasIndexEntry = { 0 };                              /* index table entry */
    CAS_INDEXENTRY *pHitPoint;                                 /* search hit point */
    SHORT          sStatus;                                    /* return status save area */

    /* read 1 entry from index table */    
    if (Cas_ReadFile(pCasFileHed->offusOffsetOfIndexTbl, aWorkRcvBuffer, (pCasFileHed->usNumberOfResidentCashier * sizeof(CAS_INDEXENTRY))) != CAS_NO_READ_SIZE) {
        sStatus = Rfl_SpBsearch(&ulCashierNo, sizeof(CAS_INDEXENTRY),
						aWorkRcvBuffer,							/* read buffer */
                        pCasFileHed->usNumberOfResidentCashier, /* number of key  */
                        &pHitPoint,								/* hit point */
                        Cas_CompIndex);         /* compare routine */
        if (sStatus == RFL_HIT) {               /* exist cashier no */
            *usParaBlockNo = pHitPoint->usParaBlock;      /* make output condition */
            return(CAS_RESIDENT);
        }
        if (usIndexMode == CAS_SEARCH_ONLY) {
            return(CAS_NOT_IN_FILE);
        }
        if (sStatus != RFL_TAIL_OF_TABLE) {
            memmove(pHitPoint + 1, pHitPoint,
				(pCasFileHed->usNumberOfResidentCashier * sizeof(CAS_INDEXENTRY) - 
					((UCHAR *)pHitPoint - (UCHAR *)aWorkRcvBuffer)));  /* size of moved byte (DO NOT -1) */
                    
        }
    } else {           /* INDEX TABLE = 0 */
        if (usIndexMode == CAS_SEARCH_ONLY) {
            return(CAS_NOT_IN_FILE);
        }
        pHitPoint = aWorkRcvBuffer;         /* pointer <- pointer */
    }

    if (Cas_EmpBlkGet(pCasFileHed, &CasIndexEntry.usParaBlock) == CAS_CASHIER_FULL) {    /* get empty block */
        return(CAS_CASHIER_FULL);                       /* return if paramater empty area */ 
    }

    /* Insert Assign Cashier in Index Table */
    CasIndexEntry.ulCashierNo = ulCashierNo;

    *pHitPoint = CasIndexEntry;                         /* copy structure */

    /* set output condition */
    *usParaBlockNo = CasIndexEntry.usParaBlock;

    /* ResidentCashier + 1 */
    pCasFileHed->usNumberOfResidentCashier++;

    /* Save(update) Index Table */
    Cas_WriteFile(pCasFileHed->offusOffsetOfIndexTbl, aWorkRcvBuffer, (pCasFileHed->usNumberOfResidentCashier * sizeof(CAS_INDEXENTRY)));

    Cas_PutHeader(pCasFileHed); /* update file header */
    return(CAS_REGISTED);
}

/*
*==========================================================================
**    Synopsis:    SHORT   Cas_IndexDel( CAS_FILEHED *pCasFileHed, ULONG ulCashierNo )
*
*       Input:    *pCasFileHed
*                 ulCashierNo
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :
*            Normal End:  CAS_PERFORM
*          Abnormal End:  CAS_NOT_IN_FILE
*
**  Description: Delete inputted Cashier from index table
*==========================================================================
*/
SHORT   Cas_IndexDel( CAS_FILEHED *pCasFileHed, ULONG ulCashierNo )
{
	CAS_INDEXENTRY aWorkRcvBuffer[CAS_NUMBER_OF_MAX_CASHIER] = { 0 };   /* index table */
    CAS_INDEXENTRY *CasIndexEntry;                              /* index table entry poiter */
    SHORT          sStatus;                                     /* status save area */

    /* get index table to work buffer */
    if (Cas_ReadFile(pCasFileHed->offusOffsetOfIndexTbl, aWorkRcvBuffer, (pCasFileHed->usNumberOfResidentCashier * sizeof(CAS_INDEXENTRY))) == CAS_NO_READ_SIZE){
        return(CAS_NOT_IN_FILE);   /* read size = 0 */
    }

    /* binary search */    
    sStatus = Rfl_SpBsearch(&ulCashierNo,               /* key */
                            sizeof(CAS_INDEXENTRY),     /* 1 entry size (w/ key) */
                            aWorkRcvBuffer,             /* read buffer */
                            pCasFileHed->usNumberOfResidentCashier, /* number of key */
                            &CasIndexEntry,             /* hit point */
                            Cas_CompIndex);             /* compare routine */

    if (sStatus != RFL_HIT) {        /* return if don't hit */
        return(CAS_NOT_IN_FILE);
    }

    Cas_BlkEmp(pCasFileHed, CasIndexEntry->usParaBlock);  /* let empty block is empty */

    memmove(CasIndexEntry,         /* distination */
            CasIndexEntry + 1,     /* source */
            (pCasFileHed->usNumberOfResidentCashier * sizeof(CAS_INDEXENTRY)    - 
            ((UCHAR *)CasIndexEntry - (UCHAR *)aWorkRcvBuffer)));  /* size of moved byte (DO NOT -1)*/

    /* update number of resident cashier */
    pCasFileHed->usNumberOfResidentCashier--;

    /* write index table */
    Cas_WriteFile(pCasFileHed->offusOffsetOfIndexTbl, aWorkRcvBuffer, (pCasFileHed->usNumberOfResidentCashier * sizeof(CAS_INDEXENTRY)));

    /* write file header */
    Cas_PutHeader(pCasFileHed);
    return(CAS_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    VOID    Cas_ParaRead(CAS_FILEHED *pCasFileHed, USHORT usParablockNo, CAS_PARAENTRY *pArg)
*
*       Input:    *pCasFileHed
*                 usParablockNo
*      Output:    *pArg
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description: Read Cashier Paramater.
*==========================================================================
*/
VOID    Cas_ParaRead(CAS_FILEHED *pCasFileHed, USHORT usParablockNo, CAS_PARAENTRY *pArg)
{
	ULONG   ulActualPosition = pCasFileHed->offusOffsetOfParaTbl + (sizeof(CAS_PARAENTRY) * (usParablockNo - 1));  /* for PifSeekFile */

	if (PifSeekFile(hsCashierFileHandle, ulActualPosition, &ulActualPosition) < 0 ) {
        PifAbort(MODULE_CASHIER, FAULT_ERROR_FILE_SEEK);
    }

    if (PifReadFile(hsCashierFileHandle, pArg, sizeof(CAS_PARAENTRY)) != sizeof(CAS_PARAENTRY)) {
        PifAbort(MODULE_CASHIER, FAULT_ERROR_FILE_READ);
    }
}

/*
*==========================================================================
**    Synopsis:    VOID    Cas_ParaWrite(CAS_FILEHED   *pCasFileHed, 
*                                        USHORT        usParaBlockNo, 
*                                        CAS_PARAENTRY *pArg)
*
*       Input:    *pCasFileHed
*                 usParaBlockNo
*                 *pArg
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Noting
*                
**  Description:  Write Cashier Paramater 
*==========================================================================
*/
VOID    Cas_ParaWrite(CAS_FILEHED   *pCasFileHed, 
                      USHORT         usParaBlockNo, 
                      CAS_PARAENTRY *pArg)
{
	ULONG   ulActualPosition = pCasFileHed->offusOffsetOfParaTbl + (sizeof(CAS_PARAENTRY) * (usParaBlockNo - 1));   /* for PifOpSeekFile */

	if (PifSeekFile(hsCashierFileHandle, ulActualPosition, &ulActualPosition) < 0)  {
        PifAbort(MODULE_CASHIER, FAULT_ERROR_FILE_SEEK);
    }
    PifWriteFile(hsCashierFileHandle, pArg, sizeof(CAS_PARAENTRY));
}

/*
*==========================================================================
**    Synopsis:   SHORT Cas_EmpBlkGet(CAS_FILEHED *pCasFileHed, 
*                                     USHORT      *usParaBlcokNo)
*
*       Input:    *pCasFileHed
*      Output:    *usParaBlcokNo
*       InOut:    Nothing
*
**  Return   :   Normal End: CAS_PERFORM
*              Abnormal End: CAS_CASHIER_FULL
*                
**  Description: Get empty Block No.
*==========================================================================
*/
SHORT   Cas_EmpBlkGet(CAS_FILEHED *pCasFileHed, USHORT *usParaBlockNo)
{
	UCHAR    uchParaEmpBlk[CAS_PARA_EMPTY_TABLE_SIZE] = { 0 };  /* paramater empty table */
    SHORT    sI;

    /* get paramater empty table */
    Cas_ReadFile(pCasFileHed->offusOffsetOfParaEmpTbl, uchParaEmpBlk, CAS_PARA_EMPTY_TABLE_SIZE);

    /* Search empty block from paramater empty table */
    for (sI = 0; sI < CAS_PARA_EMPTY_TABLE_SIZE; sI++ ) {
		if (uchParaEmpBlk[sI] != 0xff) {
			SHORT    sJ, i;

			for (i = 1, sJ = 0x01; sJ <= 0x80; i++, sJ <<= 1) {
				if ((uchParaEmpBlk[sI] & sJ) == 0x00) {
					/* Check and Make Paramater Block No.   */
					uchParaEmpBlk[sI] |= (UCHAR)sJ;
					*usParaBlockNo = (USHORT)((sI * 8) + i);   /* Make Paramater block No */
					if (*usParaBlockNo > pCasFileHed->usNumberOfMaxCashier) {
						return(CAS_CASHIER_FULL);
					}
					Cas_WriteFile((ULONG)pCasFileHed->offusOffsetOfParaEmpTbl, uchParaEmpBlk, CAS_PARA_EMPTY_TABLE_SIZE);
					return(CAS_PERFORM);
				}
			}
		}
    }

    return(CAS_CASHIER_FULL);
}

/*
*==========================================================================
**    Synopsis:   SHORT Cas_CheckBlockData(CAS_FILEHED *pCasFileHed)
*
*       Input:    *pCasFileHed
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :   Normal End: 0x0000
*                Abnormal End: Bitwise OR of following bit masks
*                    0x0001 - updated in-use table
*                    0x0002 - duplicate block found
*                    0x0004 - cloned duplicate block, updated in-use table
*
**  Description: Walk through the Cashier data file to make sure that the file data
*                is consistent. We perform the following checks:
*                  - index block numbers match the in-use flag table
*                  - each index has a unique block number
*
*                We have discovered that the Cashier data file conversion routines used
*                to convert from Rel 2.1 to Rel 2.2 had an error in which the in-use
*                flag table was not created properly during the conversion.
*
*                See functions FileToCashierFile() and FileToCashierFile_210().
*==========================================================================
*/
SHORT   Cas_CheckBlockData (CAS_FILEHED *pCasFileHed)
{
	UCHAR           uchBitMasks[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
	UCHAR           uchParaEmpBlk[CAS_PARA_EMPTY_TABLE_SIZE] = {0};    /* paramater empty table */
	CAS_INDEXENTRY  aWorkRcvBuffer[CAS_NUMBER_OF_MAX_CASHIER];         /* index table */
	SHORT           sStatus, sRetStatus = 0;                           /* return status save area */
	SHORT           i, j;
	USHORT          kSave = 1;

	Cas_ReadFile(pCasFileHed->offusOffsetOfParaEmpTbl, uchParaEmpBlk, CAS_PARA_EMPTY_TABLE_SIZE);

	/* read 1 entry from index table */
	sStatus = Cas_ReadFile(pCasFileHed->offusOffsetOfIndexTbl, aWorkRcvBuffer, pCasFileHed->usNumberOfResidentCashier * sizeof(CAS_INDEXENTRY));

	// first check. Check for in-use flag and the actual index entry block numbers agree.
	for (i = 0; i < pCasFileHed->usNumberOfResidentCashier; i++) {
		USHORT  iIndex = (aWorkRcvBuffer[i].usParaBlock - 1) / 8;
		USHORT  iBit = (aWorkRcvBuffer[i].usParaBlock - 1) % 8;

		if (aWorkRcvBuffer[i].usParaBlock < 1) continue;

		if ((uchParaEmpBlk[iIndex] & uchBitMasks[iBit]) == 0) {
			uchParaEmpBlk[iIndex] |= uchBitMasks[iBit];   // block is flagged as not in use but it is so change the flag.
			sRetStatus |= 0x01;
		}
	}

	// second check. Check for duplicated block numbers and clone any found into an unused block.
	kSave = 1;
	for (i = 0; i < pCasFileHed->usNumberOfResidentCashier; i++) {

		if (aWorkRcvBuffer[i].usParaBlock < 1) continue;

		for (j = i + 1; j < pCasFileHed->usNumberOfResidentCashier; j++) {
			if (aWorkRcvBuffer[i].usParaBlock == aWorkRcvBuffer[j].usParaBlock) {
				USHORT   k;

				for (k = kSave; k <= pCasFileHed->usNumberOfResidentCashier; k++) {
					USHORT  iIndex = (k - 1) / 8;
					USHORT  iBit = (k - 1) % 8;

					if ((uchParaEmpBlk[iIndex] & uchBitMasks[iBit]) == 0) {
						CAS_PARAENTRY  ParaCashier = { 0 };

						Cas_ParaRead(pCasFileHed, aWorkRcvBuffer[j].usParaBlock, &ParaCashier);
						aWorkRcvBuffer[j].usParaBlock = k;
						Cas_ParaWrite(pCasFileHed, aWorkRcvBuffer[j].usParaBlock, &ParaCashier);
						uchParaEmpBlk[iIndex] |= uchBitMasks[iBit];   // flag the block as in use.
						sRetStatus |= 0x04;
						kSave = k;
						break;
					}
				}
				sRetStatus |= 0x02;
			}
		}
	}

	if (sRetStatus & (0x01 | 0x04)) {
		Cas_WriteFile(pCasFileHed->offusOffsetOfParaEmpTbl, uchParaEmpBlk, CAS_PARA_EMPTY_TABLE_SIZE);
	}

	if (sRetStatus & 0x02) {
		Cas_WriteFile(pCasFileHed->offusOffsetOfIndexTbl, aWorkRcvBuffer, pCasFileHed->usNumberOfResidentCashier * sizeof(CAS_INDEXENTRY));
	}
	return sRetStatus;
}

/*
*==========================================================================
**    Synopsis:   VOID  Cas_BlkEmp(CAS_FILEHED *pCasFileHed, 
*                                  USHORT      usParaBlockNo)
*
*       Input:    *pCasFileHed
*                 usParaBlockNo
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*
**  Description: Let Empty Empty Block
*==========================================================================
*/
VOID    Cas_BlkEmp(CAS_FILEHED *pCasFileHed, USHORT usParaBlockNo)
{
    UCHAR    auchParaEmpBlk[CAS_PARA_EMPTY_TABLE_SIZE];
    SHORT    sI, sJ;

    Cas_ReadFile(pCasFileHed->offusOffsetOfParaEmpTbl, auchParaEmpBlk, CAS_PARA_EMPTY_TABLE_SIZE);
    sI = (SHORT)((usParaBlockNo - 1) / 8);
    sJ = (SHORT)((usParaBlockNo - 1) % 8);

    auchParaEmpBlk[sI] &= ~( 0x01 << sJ );
                                                          
    Cas_WriteFile(pCasFileHed->offusOffsetOfParaEmpTbl, auchParaEmpBlk, CAS_PARA_EMPTY_TABLE_SIZE);
}

/*
*==========================================================================
**    Synopsis:   SHORT   Cas_IndLockCheck(ULONG ulCashierNo)
*
*       Input:    ulCashierNo
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   
*        Normal End : CAS_UNLOCK
*      Abnormal End : CAS_LOCK
*                     CAS_ALLLOCK
*
**  Description: Check during lock
*==========================================================================
*/
SHORT   Cas_IndLockCheck(ULONG ulCashierNo)
{
    if (ulCasIndLockMem == ulCashierNo) {
        return(CAS_LOCK);
    } else  if (!uchCasAllLockMem) {    /* (uchCasAllLockMem = 0) */
        return(CAS_UNLOCK);
    }
    return(CAS_ALLLOCK);
}

/*
*==========================================================================
**    Synopsis:   SHORT   Cas_LockCheck(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :  Normal End: CAS_UNLOCK
*               Abnorma End: CAS_LOCK
*
**  Description: Check if Locked Cashier existed.
*==========================================================================
*/
SHORT   Cas_LockCheck(VOID)
{
    if (!ulCasIndLockMem) {         /* (ulCasIndLockMem == 0) */
        if (!uchCasAllLockMem) {    /* (uchCasAllLockMem == 0) */
            return(CAS_UNLOCK);
        }
    }    
    return(CAS_LOCK);
}

/*
*==========================================================================
**    Synopsis:   SHORT   Cas_IndSignInCheck(ULONG ulCashierNo)
*
*       Input:    ulCashierNo
*      Output:    Nothing
*       InOut:    Nothing
*
*   Return    :  Normal End: CAS_NOT_SIGNIN
*              Abnormal End: CAS_DURING_SIGNIN
*
**  Description: Check a cashier signin at other terminal.
*==========================================================================
*/
SHORT   Cas_IndSignInCheck(ULONG ulCashierNo)
{
    USHORT    usI;

    for (usI = 0; usI < CAS_TERMINAL_OF_CLUSTER; usI++) {
        if (aulCasTerminalTbl[usI] == ulCashierNo) {
            return(CAS_DURING_SIGNIN);
        }
    }
    return(CAS_NOT_SIGNIN);    
}

/*
*==========================================================================
**    Synopsis:   SHORT    Cas_CompIndex(USHORT         *pusKey, 
*                                        CAS_INDEXENTRY *pusHitPoint )
*
*       Input:    *pusKey          Target key
*                 *pusHitPoint     Checked key
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :   Normal End: 0x00   : Match
*              Abnormal End: < 0x00 : Target key > Checked key
*                            > 0x00 : Target key < Checked key    
*
**  Description: Compare routine.
*==========================================================================
*/
SHORT    Cas_CompIndex(ULONG *pusKey, CAS_INDEXENTRY *pusHitPoint)
{
    LONG  lKeyDifference = (((LONG)(*pusKey) - (LONG)pusHitPoint->ulCashierNo));
	SHORT sKeyDifference = 0;

	if (lKeyDifference < 0)
		sKeyDifference = -1;
	else if (lKeyDifference > 0)
		sKeyDifference = 1;

    return sKeyDifference;
}
/*
*==========================================================================
**    Synopsis:   VOID    Cas_WriteFile(ULONG  offulFileSeek, 
*                                       VOID   *pTableHeadAddress,
*                                       USHORT usSizeofWrite)
*
*       Input:    offulFileSeek      * offset from file head *
*                 *pTableHeadAddress * start address  *
*                 usSizeofWrite      * size of for writing into file *
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Nothing (error is system error(progrum is abort)
*
**  Description: Write data to Cashier file.
*==========================================================================
*/
VOID    Cas_WriteFile(ULONG offulFileSeek, 
                      VOID  *pTableHeadAddress, 
                      ULONG usSizeofWrite)
{
    ULONG   ulActualPosition;   /* DUMMY FOR PifSeekFile */

    if (!usSizeofWrite) {
        return;
    }

    if ((PifSeekFile(hsCashierFileHandle, offulFileSeek, &ulActualPosition)) < 0 ) {
        PifAbort(MODULE_CASHIER, FAULT_ERROR_FILE_SEEK);
    }
        
    PifWriteFile(hsCashierFileHandle, pTableHeadAddress, usSizeofWrite);
}
/*
*==========================================================================
**    Synopsis:   SHORT    Cas_ReadFile(ULONG  offulFileSeek, 
*                                       VOID   *pTableHeadAddress, 
*                                       USHORT usSizeofRead)
*
*       Input:    offulFileSeek      * offset from file head *
*                 usSizeofRead       * size of for reading buffer *
*      Output:    *pTableHeadAddress * start address    *
*       InOut:    Nothing
*
**  Return   :      Normal End: CAS_PERFORM
*                 Abnormal End: CAS_NO_READ_SIZE      read size is 0 
*
**  Description: Read data in the cashier file
*==========================================================================
*/
SHORT    Cas_ReadFile(ULONG offulFileSeek, VOID *pTableHeadAddress, ULONG usSizeofRead)
{
    ULONG ulActualPosition;

    if (usSizeofRead == 0x00)
        return (CAS_NO_READ_SIZE);
    
    if ((PifSeekFile(hsCashierFileHandle, offulFileSeek, &ulActualPosition)) < 0 )
        PifAbort(MODULE_CASHIER, FAULT_ERROR_FILE_SEEK);
    
    if (PifReadFile(hsCashierFileHandle, pTableHeadAddress, usSizeofRead) != usSizeofRead)
        PifAbort(MODULE_CASHIER, FAULT_ERROR_FILE_READ);
    

    return(CAS_PERFORM);
}
/*
*==========================================================================
**    Synopsis:   VOID    Cas_CloseFileReleaseSem(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing (error is system error(progrum is abort))
*                
**  Description:
*               Close the Cashier file and release the cashier semaphore.
*==========================================================================
*/
VOID    Cas_CloseFileReleaseSem(VOID)
{
    PifCloseFile(hsCashierFileHandle);
    PifReleaseSem(husCashierSem);
}

/*
*==========================================================================
**    Synopsis:   VOID    Cas_ChangeFlag(USHORT   usParaBlockNo, 
*                                        USHORT   usMode,
*                                        UCHAR    fbStatus)
*
*       Input:    usParaBlockNo
*                 usMode
*                 fbStatus
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description: Set or Reset opened flag.
*==========================================================================
*/
VOID    Cas_ChangeFlag(USHORT  usParaBlockNo, 
                       USHORT  usMode,
                       UCHAR   fbStatus)
{
    UCHAR   *pfbFlag;

    pfbFlag = &auchCasFlag[usParaBlockNo - 1];
    
    if ( usMode == CAS_SET_STATUS ) {
        *pfbFlag |= fbStatus;         /* OR   status */
    }  else {
        *pfbFlag &= ~(fbStatus);      /* EXOR status */
    }

}

/*====== End of Source ======*/

