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
*   Title              : Client/Server ETK module, Create File (ETK paramater file)
*   Category           : Client/Server ETK module, NCR2170 US HOSPITALITY PEP.
*   Program Name       : CSETKPE.C
*  ------------------------------------------------------------------------  
*   Compile            : MS-C Ver. 7.00 A by Microsoft Corp.              
*   Memory Model       : Large Model
*   Options            : /c /Alfw /W4 /Zpi /G2s /Os
*  ------------------------------------------------------------------------
*   Abstract           :
*  ------------------------------------------------------------------------
*    Update Histories  :
*   Date               : Ver.Rev    :NAME         :Description
*   Oct-07-93          : for pep    :H.YAMAGUCHI  :Modified FVT
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
#include    <csetk.h>
#include    "mypifdefs.h"
#include    <appllog.h>
#include    "csetkin.h"
#include    "csetkpe.h"

#define WIDE(s) L##s
#if defined(POSSIBLE_DEAD_CODE)

WCHAR   FARCONST  auchETK_OLD[] = WIDE("PARAM!ET");
SHORT   hsEtkNewFileHandle;
#endif
/*
*==========================================================================
**    Synopsis:    SHORT   PEPENTRY EtkCreatFile(USHORT usNumberOfEtk, 
*                                                USHORT usLockHnd)
*
*       Input:    USHORT  usNumberOfEtk - Number of Etk.
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:  ETK_PERFORM
*          Abnormal End:  ETK_NO_MAKE_FILE
*
**  Description:
*               Create Etk File.
*==========================================================================
*/
SHORT   PEPENTRY EtkCreatFile(USHORT usNumberOfEtk)
{
    ETK_FILEHEAD *pEtkFileHed;
    ULONG        ulActualPosition;
    UCHAR        auchEtk[sizeof(ETK_FILEHEAD)+ 
                         sizeof(ETK_INDEX) * ETK_MAX_EMPLOYEE];

    PifRequestSem(husEtkSem);
    
    if (!usNumberOfEtk) {           /* usNumberOfEtk == 0 */
        PifDeleteFile(auchETK_FILE);
        PifReleaseSem(husEtkSem);
        return(ETK_SUCCESS);
    } 
         
    memset(auchEtk, 0x00, sizeof(auchEtk));

    if ((hsEtkFileHandle = PifOpenFile((WCHAR FAR *)auchETK_FILE, (CHAR *)auchNEW_FILE_WRITE)) == PIF_ERROR_FILE_EXIST) {
        if ((hsEtkFileHandle = PifOpenFile((WCHAR FAR *)auchETK_FILE, 
                                            (CHAR *)auchOLD_FILE_READ)) < 0) {
            PifAbort(MODULE_ETK, FAULT_ERROR_FILE_OPEN);
        }
        /* Size 0 Check */
        if (PifSeekFile(hsEtkFileHandle, 
                        sizeof(ETK_FILEHEAD), 
                        &ulActualPosition) == PIF_ERROR_FILE_EOF) {

            PifCloseFile(hsEtkFileHandle);
            if ((hsEtkFileHandle = PifOpenFile((WCHAR FAR *)auchETK_FILE,
                                               (CHAR *)auchOLD_FILE_WRITE)) < 0) {
                PifAbort(MODULE_ETK, FAULT_ERROR_FILE_OPEN);
            }
        } else {
            PifCloseFile(hsEtkFileHandle);
            if ((hsEtkFileHandle = PifOpenFile((WCHAR FAR *)auchETK_FILE,
                                               (CHAR *)auchOLD_FILE_READ_WRITE)) < 0) {
                PifAbort(MODULE_ETK, FAULT_ERROR_FILE_OPEN);
            }
            Etk_ReadFile(ETK_FILE_HED_POSITION, 
                         auchEtk, 
                         sizeof(ETK_FILEHEAD));
            pEtkFileHed = (ETK_FILEHEAD *)auchEtk;

            if (pEtkFileHed->usNumberOfMaxEtk > usNumberOfEtk) {

                Etk_ReadFile(sizeof(ETK_FILEHEAD), 
                             &auchEtk[sizeof(ETK_FILEHEAD)], 
                             (USHORT)(sizeof(ETK_INDEX) * usNumberOfEtk));

                if (PifControlFile(hsEtkFileHandle, 
                                   PIF_FILE_CHANGE_SIZE,
                                   (LONG)((usNumberOfEtk * sizeof(ETK_INDEX)) 
                                   + sizeof(ETK_FILEHEAD))) < 0) {
                    PifCloseFile(hsEtkFileHandle);
                    PifReleaseSem(husEtkSem);
                    return ETK_NO_MAKE_FILE;
                }

                pEtkFileHed->usNumberOfMaxEtk = usNumberOfEtk;

            } else  {

                Etk_ReadFile(sizeof(ETK_FILEHEAD), 
                             &auchEtk[sizeof(ETK_FILEHEAD)], 
                             (USHORT)(sizeof(ETK_INDEX) * pEtkFileHed->usNumberOfEtk));

            } 
        }
    } else { 
                        
    }

    /* Check file size and creat file */

    if (PifSeekFile(hsEtkFileHandle,
                    (ULONG)(sizeof(ETK_FILEHEAD) 
                    + (usNumberOfEtk * sizeof(ETK_INDEX))),
                    &ulActualPosition)    < 0 ) {  /* PIF_ERROR_DISK_FULL */
        PifCloseFile(hsEtkFileHandle);
        PifReleaseSem(husEtkSem);
        return ETK_NO_MAKE_FILE;
    }

    /* Make Header File */

    pEtkFileHed = (ETK_FILEHEAD *)auchEtk;
    pEtkFileHed->usNumberOfMaxEtk = usNumberOfEtk;
    pEtkFileHed->usEtkFileSize = (USHORT)(sizeof(ETK_FILEHEAD) 
                                 + (sizeof(ETK_INDEX) * usNumberOfEtk) );

    /* Write File */

    Etk_WriteFile(ETK_FILE_HED_POSITION, 
                  auchEtk,
                  pEtkFileHed->usEtkFileSize); 

    Etk_CloseFileReleaseSem();
    return(ETK_SUCCESS);
}
#if defined(POSSIBLE_DEAD_CODE)
/*
*==========================================================================
**    Synopsis:    SHORT   PEPENTRY EtkMigrateFile(VOID)
*
*       Input: Nothing
*      Output: Nothing
*       InOut: Nothing
*
**  Return    :
*     Normal End:  ETK_PERFORM
*   Abnormal End:  ETK_NO_MAKE_FILE
*
**  Description: Create New Etk (Ver 3.1) File from Old Etk.
*==========================================================================
*/
SHORT   PEPENTRY EtkMigrateFile(VOID)
{
    ETK_FILEHEAD     EtkFileHedNew;
    ETK_FILEHEAD     EtkFileHedOld;
    SHORT sStatus;

    PifRequestSem(husEtkSem);

    if ((hsEtkFileHandle = PifOpenFile((WCHAR FAR *)auchETK_OLD,
            (CHAR *)auchOLD_FILE_READ_WRITE)) < 0) {
        return(ETK_FILE_NOT_FOUND);
    }

    if ((PifReadFile(hsEtkFileHandle, &EtkFileHedOld,   /* changed */
                     sizeof(EtkFileHedOld))) != sizeof(EtkFileHedOld)) {
        return(ETK_FILE_NOT_FOUND);
    }

    Etk_MakeHeader(&EtkFileHedNew, EtkFileHedOld.usNumberOfMaxEtk);
    
    EtkFileHedNew.usNumberOfEtk = EtkFileHedOld.usNumberOfEtk ;

    sStatus = Etk_CopyFile(&EtkFileHedNew , &EtkFileHedOld);

    if ( sStatus != ETK_SUCCESS) {
        return(sStatus);
    }

    Etk_CloseFileReleaseSem();
    return(sStatus);
}

/*
*==========================================================================
**    Synopsis:    SHORT    Etk_CopyFile(ETK_FILEHEAD     *pEtkFileHedNew,
*                                        ETK_FILEHEAD     *pEtkFileHedOld)
*
*       Input: *pEtkFileHedNew
*              *pEtkFileHedOld
*      Output: Nothing
*       InOut: Nothing
*
**  Return   :
*     Normal End: ETK_PERFORM
*   Abnormal End: ETK_NO_MAKE_FILE
*
**  Description: Create New ETK (Ver 3.1) File from Old ETK.
*==========================================================================
*/
SHORT   Etk_CopyFile(ETK_FILEHEAD     *pEtkFileHedNew,
                     ETK_FILEHEAD     *pEtkFileHedOld)
{
    SHORT    sStatus;
    ULONG   ulActualPosition;
    ETK_INDEX  EtkIndex;
    USHORT   usI;
    ULONG    ulSou, ulDes;

    PifDeleteFile(auchETK_FILE);  

    if (( hsEtkNewFileHandle = PifOpenFile((WCHAR FAR *)auchETK_FILE, 
                (CHAR *)auchNEW_FILE_WRITE)) == PIF_ERROR_FILE_EXIST) {
        if ((hsEtkNewFileHandle = PifOpenFile((WCHAR FAR *)auchETK_FILE, 
                    (CHAR *)auchOLD_FILE_READ_WRITE)) < 0) {
            PifAbort(MODULE_ETK, FAULT_ERROR_FILE_OPEN);
        }
    }

    /* Check file size and creat file */

    if (PifSeekFile(hsEtkNewFileHandle, 
                    pEtkFileHedNew->usEtkFileSize, 
                    &ulActualPosition) < 0) { /* PIF_ERROR_DISK_FULL */
        PifCloseFile(hsEtkNewFileHandle);
        PifDeleteFile(auchETK_FILE);          /* Can't make file then delete file */
        return(ETK_NO_MAKE_FILE);
    }

    /* Copy PARAM!ET to PARAMETK */

    memset(&EtkIndex, 0x00, sizeof(ETK_INDEX));

    ulDes = (ULONG)sizeof(ETK_FILEHEAD);
    ulSou = (ULONG)sizeof(ETK_FILEHEAD);

    for (usI=0; usI < pEtkFileHedOld->usNumberOfEtk; usI++){

        Etk_ReadFile(ulSou, &EtkIndex, sizeof(USHORT));
        Etk_ReadFile(ulSou + sizeof(USHORT), &EtkIndex.EtkJC, sizeof(ETK_JOB));
        Etk_WriteNew(ulDes, &EtkIndex, sizeof(ETK_INDEX));

        ulDes = (ULONG)(ulDes + (ULONG)sizeof(ETK_INDEX));
        ulSou = (ULONG)(ulSou + (ULONG)sizeof(ETK_INDEX_OLD));
    }

    /* Write new header */

    Etk_WriteNew(ETK_FILE_HED_POSITION, 
                   pEtkFileHedNew, sizeof(ETK_FILEHEAD));

    sStatus = ETK_SUCCESS;

    PifCloseFile(hsEtkNewFileHandle);
    return(sStatus);
}

/*
*==========================================================================
**    Synopsis:    SHORT   Etk_WriteNew(ULONG  offulFileSeek, 
*                                       VOID   *pTableHeadAddress, 
*                                       USHORT usSizeofWrite)
*                         
*       Input: offulFileSeek         Number of bytes from the biginning of the plu file.
*              *pTableHeadAddress    Pointer to data to be written.
*              usSizeofWrite         Number of bytes to be written.
*      Output: Nothing
*       InOut: Nothing
*
**  Return   : Nothing (error is system error (progrum is abort)
*
**  Description: Write data in Etk file.
*==========================================================================
*/
VOID   Etk_WriteNew(ULONG  offulFileSeek, 
                    VOID   *pTableHeadAddress, 
                    USHORT usSizeofWrite)
{
    ULONG   ulActualPosition;

    if (!usSizeofWrite) {
        return ;
    }
    if ((PifSeekFile(hsEtkNewFileHandle, offulFileSeek, 
                     &ulActualPosition)) < 0) {
        PifAbort(MODULE_ETK, FAULT_ERROR_FILE_SEEK);
    }
    PifWriteFile(hsEtkNewFileHandle, pTableHeadAddress, usSizeofWrite);
}


/*
*==========================================================================
**    Synopsis:    SHORT   PEPENTRY EtkDeleteFile()
*
*       Input:    USHORT  usNumberOfEtk - Number of Etk.
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:  ETK_PERFORM
*
**  Description:
*               Delete Etk File.
*==========================================================================
*/
SHORT   PEPENTRY EtkDeleteFile()
{

    if (hsEtkFileHandle) {  /* File exist */

        PifCloseFile(hsEtkFileHandle);
        PifDeleteFile(auchETK_FILE);

    }

    return(ETK_SUCCESS);

}

/*====== End of Source ========*/

#endif