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
*    USER I/F FUNCTION
*
*    EtkInit();              * ETK function initiarize  *
*    EtkAssign();            * ETK assign function *
*    EtkDelete();            * ETK delete function *
*    EtkIndJobRead();        * read allowance job code *
*    EtkCurAllIdRead();      * read current all id *
*    EtkAllReset();          * reset all record *
*    EtkAllLock();           * lock all ETK record. do not update *
*    EtkAllUnLock();         * unlock all ETK record. do not update *
*
*  ------------------------------------------------------------------------
*    Update Histories  :
*
*    Date     : Ver.Rev :NAME       :Description
*    Oct-07-93:00.00.01 :H.YAMAGUCHI:Modified for FVT comments
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
#include    <rfl.h>
#include    <paraequ.h>
#include    <appllog.h>
#include    <csetk.h>
/* #include    <csttl.h>    */
#include    "csetkin.h"
#include    "mypifdefs.h"

#define WIDE(s) L##s

WCHAR   FARCONST  auchETK_FILE[] = WIDE("PARAMETK");

USHORT  husEtkSem;    
SHORT   hsEtkFileHandle;
#if defined(POSSIBLE_DEAD_CODE)
/*
*==========================================================================
**    Synopsis:   VOID    PEPENTRY EtkInit(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description:
*               Initialize for Etk Module.
*               Save semaphore handle. 
*==========================================================================
*/
VOID    PEPENTRY EtkInit(VOID)
{
    /* Create Semaphore */
    husEtkSem = PifCreateSem( ETK_COUNTER_OF_SEM );  /* save semaphore handle */
}
#endif
/*
*==========================================================================
**    Synopsis:    SHORT   PEPENTRY EtkAssign(ULONG     ulEmployeeNo,
*                                             ETK_JOB   *pEtkJob,
*                                             UCHAR     *puchEmployeeName)
*
*       Input:      ulEmployeeNo    -   Employee Number to be assign
*                   struct {
*                       UCHAR   uchJobCode1;     Allowance Job code 1
*                       UCHAR   uchJobCode2;     Allowance Job code 2
*                       UCHAR   uchJobCode3;     Allowance Job code 3
*                   } ETK_JOB
*                   puchEmployeeName -   Employee Name   
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    ETK_SUCCESS
*          Abnormal End:    ETK_LOCK
*                           ETK_FILE_NOT_FOUND
*                           ETK_FULL
*
**  Description:
*                Add, Regist Inputed Etk.
*==========================================================================
*/
SHORT   PEPENTRY EtkAssign(ULONG   ulEmployeeNo,
                           ETK_JOB  *pEtkJob,
                           WCHAR    *puchEmployeeName)
{
	ETK_FILEHEAD EtkFileHed = { 0 };
	ETK_INDEX    EtkIndex = { 0 };
    ULONG        ulIndexPos;          /* Etk_Index() 's */

    PifRequestSem(husEtkSem);

    /* CHECK LOCK */
    if (Etk_LockCheck() == ETK_LOCK) {
        PifReleaseSem(husEtkSem);
        return(ETK_LOCK);
    }

    /* Open File */
    if ((hsEtkFileHandle = PifOpenFile(auchETK_FILE, auchOLD_FILE_READ_WRITE)) == PIF_ERROR_FILE_NOT_FOUND) {
        PifReleaseSem(husEtkSem);
        return(ETK_FILE_NOT_FOUND);
    }

    /* Get File header */
    Etk_GetHeader(&EtkFileHed);

    EtkIndex.EtkJC = *pEtkJob;

    /* ===== Add Employee Name (R3.1) BEGIN ===== */
    memset( EtkIndex.auchEmployeeName, 0x00, sizeof(EtkIndex.auchEmployeeName) );
    memcpy( EtkIndex.auchEmployeeName, puchEmployeeName, sizeof(EtkIndex.auchEmployeeName) );
    /* ===== Add Employee Name (R3.1) END ===== */

    /* Check Assign Etk */
    if (Etk_Index(&EtkFileHed, ETK_REGIST, ulEmployeeNo, &EtkIndex, &ulIndexPos) == ETK_FULL) {
           Etk_CloseFileReleaseSem();
           return(ETK_FULL);   /* return if assign ETK full */
    }

    Etk_PutHeader(&EtkFileHed); /* update file header */

    Etk_CloseFileReleaseSem();  /* file close and release semaphore */
    return(ETK_SUCCESS);
}

/*
*==========================================================================
**    Synopsis:    SHORT   PEPENTRY EtkDelete(ULONG ulEmployeeNo)
*
*       Input:    ULONG ulEtkNo
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    ETK_SUCCESS
*          Abnormal End:    ETK_LOCK
*                           ETK_FILE_NOT_FOUND
*                           ETK_NOT_IN_FILE
*                           ETK_DATA_EXIST
*
**  Description:
*                Delete Inputed Etk No from index table
*==========================================================================
*/
SHORT   PEPENTRY EtkDelete(ULONG ulEmployeeNo)
{
    SHORT        sStatus;               /* status save area */
	ETK_FILEHEAD EtkFileHed = { 0 };    /* file header */
	ETK_INDEX    EtkIndex = { 0 };      /* Etk Record to be read */
    ULONG        ulIndexPos;

    PifRequestSem(husEtkSem);   /* request semaphore */

    /* Open file */
    if ((hsEtkFileHandle = PifOpenFile(auchETK_FILE, auchOLD_FILE_READ_WRITE)) == PIF_ERROR_FILE_NOT_FOUND) {
        PifReleaseSem(husEtkSem);
        return(ETK_FILE_NOT_FOUND);
    }

    /* get file header */
    Etk_GetHeader(&EtkFileHed);

    /* Check ETK assign and delete */
    if (((sStatus = Etk_Index(&EtkFileHed, ETK_SEARCH_ONLY, ulEmployeeNo, &EtkIndex, &ulIndexPos)) != ETK_NOT_IN_FILE)) {
        sStatus = Etk_IndexDel(&EtkFileHed, ulEmployeeNo);
        Etk_PutHeader(&EtkFileHed);
    }
    Etk_CloseFileReleaseSem();      /* file close and release semaphore */
    return(sStatus);        
}

/*
*==========================================================================
**    Synopsis:    SHORT   PEPENTRY EtkIndJobRead(ULONG    ulEmployeeNo,
*                                                 ETK_JOB  *pEtkJob,
*                                                 UCHAR    *puchEmployeeName)
*
*       Input:    ULONG     ulEmployeeNo
*      Output:    ETK_JOB   *pEtkJob
*                 UCAHR     *puchEmployeeName
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    ETK_SUCCESS
*          Abnormal End:    ETK_FILE_NOT_FOUND
*
**  Description:
*                Read all allowance Job Code.
*==========================================================================
*/
SHORT   PEPENTRY EtkIndJobRead(ULONG    ulEmployeeNo,
                               ETK_JOB  *pEtkJob,
                               WCHAR    *puchEmployeeName )
{
    SHORT        sStatus;              /* Save status */
	ETK_FILEHEAD EtkFileHed = { 0 };   /* file header save area */
	ETK_INDEX    EtkIndex = { 0 };     /* Index record save area */
    ULONG        ulIndexPos;           /* Etk_Index() 's */

	// initialize the return values to a known value.
	*pEtkJob = EtkIndex.EtkJC;
	*puchEmployeeName = 0;

	PifRequestSem(husEtkSem);   /* request samaphore */

    /* open file */
    if ((hsEtkFileHandle = PifOpenFile(auchETK_FILE, auchOLD_FILE_READ_WRITE)) == PIF_ERROR_FILE_NOT_FOUND) {
        PifReleaseSem(husEtkSem);
        return(ETK_FILE_NOT_FOUND);
    }

    /* get file header */
    Etk_GetHeader(&EtkFileHed);

    /* check ETK assgin */
    if ((sStatus = Etk_Index(&EtkFileHed, ETK_SEARCH_ONLY, ulEmployeeNo, &EtkIndex, &ulIndexPos)) == ETK_NOT_IN_FILE) {
        Etk_CloseFileReleaseSem();
		return(sStatus);
    }

    *pEtkJob = EtkIndex.EtkJC;

    /* ===== Add Employee Name (Release 3.1) BEGIN ===== */
    memcpy( puchEmployeeName, EtkIndex.auchEmployeeName, sizeof(EtkIndex.auchEmployeeName) );
    /* ===== Add Employee Name (Release 3.1) END ===== */

    Etk_CloseFileReleaseSem();                                /* close file and release semaphore */
    return(ETK_SUCCESS);
}

/*
*==========================================================================
**    Synopsis:   SHORT  PEPENTRY EtkCurAllIdRead(USHORT usRcvBufferSize,
*                                                 ULONG  *aulRcvBuffer)
*
*       Input:    USHORT usRcvBufferSize
*       InOut:    ULONG  *aulRcvBuffer
*
**  Return    :
*           Number of Etk          < 0
*           NOT_IN_FILE            = 0
*           ETK_FILE_NOT_FOUND     > 0
*
**  Description:
*                Read all Etk No. in this file.
*==========================================================================
*/
SHORT  PEPENTRY EtkCurAllIdRead(USHORT usRcvBufferSize, 
                                ULONG *aulRcvBuffer)
{
    USHORT       cusi;                                                
	ETK_FILEHEAD EtkFileHed = { 0 };        /* file header */
	ETK_INDEX    EtkIndexEntry = { 0 };     /* index table entry */

    PifRequestSem(husEtkSem);       /* request semaphore */

    /* open file */
    if ((hsEtkFileHandle = PifOpenFile(auchETK_FILE, auchOLD_FILE_READ)) == PIF_ERROR_FILE_NOT_FOUND) {
        PifReleaseSem(husEtkSem);
        return(ETK_FILE_NOT_FOUND);
    }

    /* get file header */
    Etk_GetHeader(&EtkFileHed);

    /* Count inputed ETK no in index table */
    for (cusi = 0 ;((cusi * sizeof(EtkIndexEntry.ulEmployeeNo)) < usRcvBufferSize) &&
        (cusi < EtkFileHed.usNumberOfEtk); cusi++) {

        Etk_ReadFile( sizeof(ETK_FILEHEAD) + (cusi * sizeof(ETK_INDEX)), &EtkIndexEntry, sizeof(ETK_INDEX) );
        *aulRcvBuffer++ = EtkIndexEntry.ulEmployeeNo;
    }

    Etk_CloseFileReleaseSem();      /* close file and release semaphore */
    return(cusi);
}
#if defined(POSSIBLE_DEAD_CODE)
/*
*==========================================================================
**    Synopsis:    SHORT   PEPENTRY EtkAllReset(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    ETK_SUCCESS
*
**  Description:
*                
*==========================================================================
*/
SHORT   PEPENTRY EtkAllReset(VOID)
{
    return(ETK_SUCCESS);
}

/*
*==========================================================================
**    Synopsis:    SHORT   PEPENTRY EtkAllLock(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    ETK_SUCCESS
*
**  Description:
*                Lock All Etk. 
*==========================================================================
*/
SHORT   PEPENTRY EtkAllLock(VOID)
{
    return(ETK_SUCCESS);
}


/*
*==========================================================================
**    Synopsis:    VOID   PEPENTRY EtkAllUnLock(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*
**  Description:
*                Unlock All Etk
*==========================================================================
*/
VOID   PEPENTRY EtkAllUnLock(VOID)
{
    PifRequestSem(husEtkSem);       /* reqest semaphore */
    PifReleaseSem(husEtkSem);       /* release semaphore */
}

/*====== End of Source ======*/
#endif