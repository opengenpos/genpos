/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server ETK module, Program List                        
*   Category           : Client/Server ETK module, NCR2170 US HOSPITALITY APPLICATION
*   Program Name       : NWETK.C
*  ------------------------------------------------------------------------
*    Abstract           :The provided function names are as follows.
*
*   INTERNAL FUNCTION
*                            
*    Etk_GetHeader();         * Get File Header *
*    Etk_PutHeader();         * Put File Header *
*    Etk_Index()              * Search index file and set and modifi *
*    Etk_IndexDel()           * Search index file and delete *
*    Etk_LockCheck();         * Check All lock *
*    Etk_CompIndex();         * Compear Index file *
*    Etk_WriteFile()          * Write data into File *
*    Etk_ReadFile()           * Read data into file *
*    Etk_CloseFileReleaseSem(); * Close file and release semaphore *
*    EtkMakeHeader();         * make file header *
*    EtkEditHeader();         * Edit file header *
*
*  ------------------------------------------------------------------------
*    Update Histories  :
*                                 
*    Date      Ver.Rev :NAME       :Description
*    Oct-07-93:00.00.01:H.YAMAGUCHI:Modified FVT
*    Jan-29-96:03.01.00:M.SUZUKI   : R3.1 Initial
*                                       Increase No. of Employees (200 => 250)
*                                       Increase Employee No. ( 4 => 9digits)
*                                       Add Employee Name (Max. 16 Char.)
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
#include    <paraequ.h>
#include    <rfl.h>
#include    <appllog.h>
#include    <csetk.h>
#include    "csetkin.h"
#include    "mypifdefs.h"

/*
*==========================================================================
**    Synopsis:   VOID  Etk_GetHeader(ETK_FILEHEAD *pEtkFileHed)
*
*       Input:    Nothing
*      Output:    ETK_FILEHEAD *pEtkFileHed
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description:
*                Get Header in File.
*==========================================================================
*/
VOID   Etk_GetHeader(ETK_FILEHEAD *pEtkFileHed)
{
    Etk_ReadFile(ETK_FILE_HED_POSITION, pEtkFileHed, sizeof(ETK_FILEHEAD));
}

/*
*==========================================================================
**    Synopsis:   VOID  Etk_PutHeader(ETK_FILEHEAD *pEtkFileHed)
*
*       Input:    ETK_FILEHEAD *pEtkFileHed
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*
**  Description:
*                Put Header in File
*==========================================================================
*/
VOID   Etk_PutHeader(ETK_FILEHEAD *pEtkFileHed)
{
    Etk_WriteFile(ETK_FILE_HED_POSITION, pEtkFileHed, sizeof(ETK_FILEHEAD));
}

/*
*==========================================================================
**  Synopsis:    SHORT   Etk_Index(UCHAR        uchBufferType,
*                                  ETK_FILEHEAD *pEtkFileHed,
*                                  USHORT       usIndexMode,
*                                  ULONG        ulEtkNo,
*                                  ETK_INDEX    *pEtkEntry,
*                                  ULONG        *pulIndexPosition)
*
*       Input:    ETK_FILEHEAD *pEtkFileHed   file header's address
*                 ULONG  ulEtkNo              Pointer to object to search for
*                        
*      Output:    ETK_INDEX *pEtkEntry,       Serched record data 
*                 ULONG  *pulIndexPosition)   Offset of Serched record data
*                                             from file head.
*
*       InOut:    Nothing
*
**  Return   :
*            Normal End:  ETK_RESIDENT
*          Abnormal End:  ETK_FILE_NOT_FOUND
*                         ETK_NOT_IN_FILE
*                         ETK_FULL
*   
**  Description:
*                Check if Etk exsisted.
*==========================================================================
*/
SHORT   Etk_Index(ETK_FILEHEAD *pEtkHead,
                  USHORT       usIndexMode,
                  ULONG        ulEtkNo,
                  ETK_INDEX    *pEtkEntry,
                  ULONG        *pulIndexPosition)
{
	ETK_INDEX aWorkRcvBuffer[ETK_MAX_EMPLOYEE + 1] = { 0 };  /* index table plus 1 to allow for memmove below */
	ETK_INDEX *pHitPoint, EtkSav = { 0 };                    /* search hit point */
    SHORT     sStatus;                                       /* return status save area */

    /* save Etk Data */
    EtkSav = *pEtkEntry;
    EtkSav.ulEmployeeNo = ulEtkNo;

	if (pEtkHead->usNumberOfEtk > ETK_MAX_EMPLOYEE) {
		pEtkHead->usNumberOfEtk = ETK_MAX_EMPLOYEE;
	}

    /* Serch Index, and Read Job Code , Insert Data, replace data */
    if (Etk_ReadFile(sizeof(ETK_FILEHEAD), aWorkRcvBuffer, (pEtkHead->usNumberOfEtk * sizeof(ETK_INDEX))) != ETK_NO_READ_SIZE) {
		/*
				Do a binary search on the ETK file looking for the specified
				empoyee number..
		 */
    
        sStatus = Rfl_SpBsearch(&ulEtkNo,       
                                sizeof(ETK_INDEX),         
                                aWorkRcvBuffer,            /* read buffer    */
                                pEtkHead->usNumberOfEtk,   /* number of key  */
                                (VOID FAR *)&pHitPoint,    /* hit point      */
                                (SHORT (FAR *)(VOID *pKey, VOID *pusHPoint))Etk_CompIndex);            /* compare routine */

        if (sStatus == RFL_HIT) {               /* exist ETK no */

            if (usIndexMode == ETK_SEARCH_ONLY) {

                *pEtkEntry = *pHitPoint;            /* Copy Index record */
                return(ETK_RESIDENT);
            }

            *pulIndexPosition = (sizeof(ETK_FILEHEAD) + ((UCHAR *)pHitPoint - (UCHAR *)aWorkRcvBuffer));

            Etk_WriteFile(*pulIndexPosition, &EtkSav, sizeof(ETK_INDEX));

            return(ETK_RESIDENT);
        }

        if (usIndexMode == ETK_SEARCH_ONLY) {
            return(ETK_NOT_IN_FILE);
        }

        if (sStatus != RFL_TAIL_OF_TABLE) {
            memmove(pHitPoint+1,                            /* distination */
                    pHitPoint,                              /* source */
                    ( pEtkHead->usNumberOfEtk * sizeof(ETK_INDEX)    - 
                    ( (UCHAR *)pHitPoint - (UCHAR *)aWorkRcvBuffer) ));  /* size of moved byte (DO NOT -1) */
        }

    } else {        /* INDEX TABLE = 0 */

        if (usIndexMode == ETK_SEARCH_ONLY) {
            return(ETK_NOT_IN_FILE);
        }
        pHitPoint = aWorkRcvBuffer;         /* pointer <- pointer */
    }

    if ( pEtkHead->usNumberOfEtk +1 > pEtkHead->usNumberOfMaxEtk) {
         return(ETK_FULL);             /* return if paramater empty area */ 
    }

    /* Insert Assign Etk in Index Table */
    *pHitPoint = EtkSav;                         /* copy structure */

    /* set output condition */
    *pulIndexPosition = sizeof(ETK_FILEHEAD) + (UCHAR *)pHitPoint - (UCHAR *)aWorkRcvBuffer ;

    /* ResidentEtk + 1 */
    pEtkHead->usNumberOfEtk++;

    /* Save(update) Index Table */
    Etk_WriteFile( sizeof(ETK_FILEHEAD), aWorkRcvBuffer, (pEtkHead->usNumberOfEtk * sizeof(ETK_INDEX)));

    return(ETK_RESIDENT);
}

/*
*==========================================================================
**    Synopsis:    SHORT   Etk_IndexDel(ETK_FILEHEAD *pEtkFileHed,
*                                       ULONG        ulEtkNo )
*
*       Input:    ETK_FILEHEAD *pEtkFileHed
*                 ULONG        ulEtkNo
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    ETK_SUCCESS
*          Abnormal End:    ETK_NOT_IN_FILE
*
**  Description:
*                Delete inputted Etk from index table 
*==========================================================================
*/
SHORT   Etk_IndexDel(ETK_FILEHEAD *pEtkHead,
                     ULONG        ulEtkNo )
{
	ETK_INDEX aWorkRcvBuffer[ETK_MAX_EMPLOYEE + 1] = { 0 };  /* index table */
	ETK_INDEX *pEtkIndexEntry = { 0 };                       /* index table entry poiter */
    SHORT     sStatus;                                       /* status save area */
	LONG      lBytesToMove = 0;

    /* get index table to work buffer */

	if (pEtkHead->usNumberOfEtk > ETK_MAX_EMPLOYEE) {
		pEtkHead->usNumberOfEtk = ETK_MAX_EMPLOYEE;
	}

    if (Etk_ReadFile(sizeof(ETK_FILEHEAD), aWorkRcvBuffer, (pEtkHead->usNumberOfEtk * sizeof(ETK_INDEX))) == ETK_NO_READ_SIZE) {
        return(ETK_NOT_IN_FILE);   /* read size = 0 */
    }

    /*
			Do a binary search on the ETK file looking for the specified
			empoyee number.  If not found, then return with ETK_NOT_IN_FILE.
			Otherwise we will then delete the found employee record by rippling
			down the employee records above this one.
	 */
    
    sStatus = Rfl_SpBsearch(&ulEtkNo,                 /* key */
                            sizeof(ETK_INDEX),        /* 1 entry size (w/ key) */
                            aWorkRcvBuffer,           /* read buffer */
                            pEtkHead->usNumberOfEtk,  /* numbe of key */
                            &pEtkIndexEntry,  /* hit point */
                            Etk_CompIndex); /* compare routine */

    if (sStatus != RFL_HIT) {        /* return if don't hit */
        return(ETK_NOT_IN_FILE);
    }

	// Delete the entry by rippling down any ETK_INDEX entries which are greater than
	// the item being deleted.  If the entry being deleted is at the end, then we do
	// not need to ripple down since we will just decrement the number of ETK records
	// hence the value will not be in the span for the employee number search.
	lBytesToMove = (pEtkHead->usNumberOfEtk - (pEtkIndexEntry - aWorkRcvBuffer)) - 1;
	if (lBytesToMove > 0) {
		lBytesToMove *= sizeof(ETK_INDEX);
		memmove (pEtkIndexEntry, pEtkIndexEntry + 1, lBytesToMove );
	}

    /* update number of resident ETK */ 
    pEtkHead->usNumberOfEtk--;

    /* write ETK file contents back to disk */
    Etk_WriteFile(sizeof(ETK_FILEHEAD), aWorkRcvBuffer, (pEtkHead->usNumberOfEtk * sizeof(ETK_INDEX)));

    return(ETK_SUCCESS);
}

/*
*==========================================================================
**    Synopsis:    SHORT   Etk_LockCheck(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :    ETK_UNLOCK
*                  ETK_LOCK
*
**  Description:
*                Check if Etk is Locked.
*==========================================================================
*/
SHORT   Etk_LockCheck(VOID)
{
	UCHAR   uchEtkAllLockMem = 0x00;        /* clear all ETK lock mamory */
    if (!uchEtkAllLockMem) {    /* (uchEtkAllLockMem == 0) */
        return(ETK_UNLOCK);
    }
    return(ETK_LOCK);
}

/*
*==========================================================================
**    Synopsis:    SHORT    Etk_CompIndex(ULONG    *pulKey,
*                                         ETK_INDEX *pusHitPoint )
*
*       Input:    ULONG     *pulKey          Target key
*                 ETK_INDEX *pusHitPoint     Checked key
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :    0x00 : Match
*                < 0x00 : Target key > Checked key
*                > 0x00 : Target key < Checked key    
*
**  Description:
*               Compare routine.
*==========================================================================
*/
SHORT    Etk_CompIndex(ULONG *pulKey, ETK_INDEX *pusHitPoint)
{
    if (*pulKey == pusHitPoint->ulEmployeeNo) {
        return (0);
    }
    if (*pulKey > pusHitPoint->ulEmployeeNo) {
        return (1);
    }
    return (-1);
}
/*
*==========================================================================
**    Synopsis:    VOID    Etk_WriteFile(ULONG  offulFileSeek,
*                                        VOID   *pTableHeadAddress,
*                                        USHORT usSizeofWrite)
*
*       Input:   ULONG offulFileSeek   * offset from file head *
*                VOID  *pTableHeadAddress * table head address for writing into file *
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
VOID    Etk_WriteFile(ULONG  offulFileSeek,
                      VOID   *pTableHeadAddress,
                      ULONG usSizeofWrite)
{
    ULONG   ulActualPosition;   /* DUMMY FOR PifSeekFile */
                                                            
    if (!usSizeofWrite) {
        return;
    }

    if ((PifSeekFile(hsEtkFileHandle, offulFileSeek, &ulActualPosition)) < 0 ) {
        PifAbort(MODULE_ETK, FAULT_ERROR_FILE_SEEK);
    }
        
    PifWriteFile(hsEtkFileHandle, pTableHeadAddress, usSizeofWrite);
}
/*
*==========================================================================
**    Synopsis:    SHORT    Etk_ReadFile(ULONG  offulFileSeek,
*                                        VOID   *pTableHeadAddress,
*                                        USHORT usSizeofRead)
*
*       Input:    ULONG offulFileSeek   * offset from file head *
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
SHORT    Etk_ReadFile(ULONG  offulFileSeek,
                      VOID   *pTableHeadAddress,
                      ULONG usSizeofRead)
{
    ULONG   ulActualPosition;
	USHORT  usActualRead = 0;

    if ((PifSeekFile(hsEtkFileHandle, offulFileSeek, &ulActualPosition)) < 0 ) {
		if (offulFileSeek == 0) {
			// handle the case that an attempt to read the file header fails
			// because the file is empty. scenario seen with conversions from
			// older release databases such as Rel 2.1 which may have operators
			// but no employee records.
			return ETK_NO_READ_SIZE;
		}
        PifAbort(MODULE_ETK, FAULT_ERROR_FILE_SEEK);
    }

    if (usSizeofRead == 0x00){
		// allow this function to seek to a particular location but not do a read.
        return (ETK_NO_READ_SIZE);
    }

	if ((usActualRead = PifReadFile(hsEtkFileHandle, pTableHeadAddress, usSizeofRead)) != usSizeofRead) {
		if (usActualRead == 0) {
			// handle the case that an attempt to read the file header fails
			// because the file is empty. scenario seen with conversions from
			// older release databases such as Rel 2.1 which may have operators
			// but no employee records.
			return ETK_NO_READ_SIZE;
		}
		PifAbort(MODULE_ETK, FAULT_ERROR_FILE_READ);
    }

    return(ETK_SUCCESS);
}
/*
*==========================================================================
**    Synopsis:    VOID    Etk_CloseFileReleaseSem(VOID)
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
VOID    Etk_CloseFileReleaseSem(VOID)
{
    PifCloseFile(hsEtkFileHandle);
    PifReleaseSem(husEtkSem);
}
#if defined(POSSIBLE_DEAD_CODE)
/*                 
*==========================================================================
**    Synopsis:    VOID   Etk_MakeHeader(ETK_FILEHEAD *pEtkFileHed,
*                                        USHORT       usNumberOfEmployee)
*
*       Input:    USHORT usNumberOfEmployee  * Number of employee * 
*      Output:    *pEtkFileHed               * File Header *
*       InOut:    NONE
*
**  Return    :   NONE 
*
**  Description:
*                Make and Set Etk file header's data.
*==========================================================================
*/
VOID   Etk_MakeHeader(ETK_FILEHEAD *pEtkFileHed, USHORT usNumberOfEmployee)
{
	if (usNumberOfEmployee > ETK_MAX_EMPLOYEE) {
		usNumberOfEmployee = ETK_MAX_EMPLOYEE;
	}

    pEtkFileHed->usNumberOfMaxEtk = usNumberOfEmployee;
    pEtkFileHed->usNumberOfEtk = 0;
    pEtkFileHed->usEtkFileSize = (USHORT)(sizeof(ETK_FILEHEAD) +
                                 (sizeof(ETK_INDEX) * usNumberOfEmployee));
}

/*====== End of Source ======*/

#endif