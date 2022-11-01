//#if defined(POSSIBLE_DEAD_CODE)
/*************************************************************************\
||                                                                       ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server OPERATIONAL PARAMATER module, Create File (PLU PARAMATER, DEPT PARAMATER)
*   Category           : Client/Server OPERATIONAL PARAMATER module, Saratoga US HOSPITALITY APPLICATION.
*   Program Name       : NWOPHGV210.C
*  ------------------------------------------------------------------------
*  ------------------------------------------------------------------------
*   Abstract           :
*
*       USER I/F FUNCITONS
*
*  ------------------------------------------------------------------------
*    Update Histories  :
*   Date          : Ver.Rev  :NAME       :Description
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

#include    <Windows.h>
#include    <string.h>
#include    <stdlib.h>
#include    <ecr.h>
#include    <r20_pif.h>
#include    <rfl.h>

#include	"AW_Interfaces.h"
#include    "mypifdefs.h"
#include    "paraequ.h"
#include    "para.h"
#include    "plu.h"
#include    "appllog.h"
#include    "csop.h"
#include    "csopin.h"
#include    "csopHGV210.h"

SHORT	hsOpPluIndexFileHandle;

/*
*==========================================================================
**    Synopsis:    SHORT   Op_ReadPluFile_HGV210(ULONG  offulFileSeek,
*                                             VOID   *pTableHeadAddress,
*                                             USHORT usSizeofRead)
*
*       Input: offulFileSeek      Number of bytes from the biginning of the Plu file.
*              usSizeofRead       Number of bytes to be read.
*      Output: *pTableHeadAddress Pointer to data to be written.
*       InOut:  Nothing
*
**  Return   : OP_PERFORM
*              OP_NO_READ_SIZE         read size is 0
*              (file error is system halt)
*
**  Description: Read Data in Plu file.
*==========================================================================
*/
SHORT   Op_ReadPluFile_HGV210(ULONG  offulFileSeek,
                           VOID   *pTableHeadAddress,
                           ULONG usSizeofRead)
{
    ULONG   ulActualPosition;

    if (usSizeofRead == 0x00) {
        PifAbort(MODULE_OP_PLU, FAULT_ERROR_FILE_READ);
    }
    if ((PifSeekFile(hsOpPluFileHandle, offulFileSeek, &ulActualPosition)) < 0) {
        PifAbort(MODULE_OP_PLU, FAULT_ERROR_FILE_SEEK);
    }
    if ((PifReadFile(hsOpPluFileHandle, pTableHeadAddress, usSizeofRead)) != usSizeofRead) {
        PifAbort(MODULE_OP_PLU, FAULT_ERROR_FILE_READ);
    }

    return(OP_PERFORM);
}


/*
*==========================================================================
**    Synopsis:    SHORT   Op_ReadIndexPluFile_HGV210(ULONG  offulFileSeek,
*                                             VOID   *pTableHeadAddress,
*                                             USHORT usSizeofRead)
*
*       Input: offulFileSeek      Number of bytes from the biginning of the Plu file.
*              usSizeofRead       Number of bytes to be read.
*      Output: *pTableHeadAddress Pointer to data to be written.
*       InOut:  Nothing
*
**  Return   : OP_PERFORM
*              OP_NO_READ_SIZE         read size is 0
*              (file error is system halt)
*
**  Description: Read Data in Plu Index file.  JHHJ
*==========================================================================
*/
SHORT   Op_ReadIndexPluFile_HGV210(ULONG  offulFileSeek,
                           VOID   *pTableHeadAddress,
                           ULONG usSizeofRead)
{
    ULONG   ulActualPosition;

	//added JHHJ
    if (usSizeofRead == 0x00) {
        PifAbort(MODULE_OP_PLU, FAULT_ERROR_FILE_READ);
    }
    if ((PifSeekFile(hsOpPluIndexFileHandle, offulFileSeek, &ulActualPosition)) < 0) {
        PifAbort(MODULE_OP_PLU, FAULT_ERROR_FILE_SEEK);
    }
    if ((PifReadFile(hsOpPluIndexFileHandle, pTableHeadAddress, usSizeofRead)) != usSizeofRead) {
        PifAbort(MODULE_OP_PLU, FAULT_ERROR_FILE_READ);
    }

    return(OP_PERFORM);
}



/**
;========================================================================
;
;   function : Read a record
;
;   format : USHORT		PhyReadRecord_HGV210(hf, usCell, pucBuffer, pHeader);
;
;   input  : USHORT		hf;			- file handle
;			 ULONG 		ulCell;		- cell no.@### MOD 2172 Rel1.0 (USHORT->ULONG)
;			 UCHAR		*pucBuffer;	- pointer to respond a record
;			 PFILEHDR	pHeader;	- header information ptr.
;
;   output : USHORT		usStat;		- status
;
;========================================================================
**/


USHORT	PhyReadRecord_HGV210(USHORT hf, ULONG ulCell, RECPLU210 *pucBuffer, FILEHDR_HGV210 *pHeader)
{
	ULONG	usBytes;
	ULONG	ulRecOffset/* ### */, ulMoved;

	/* --- compute dirty handle --- */

	//hfDirty = hf - MAGIC_HANDLE;

	/* --- compute a file position --- */
	usBytes = (USHORT)pHeader->usRecLen;					/* record length in byte*/
	ulRecOffset   = (ULONG)(pHeader->ulOffsetRecord);		/* top of record data	*/
	ulRecOffset  += (ULONG)usBytes * ulCell;				/* add record offset	*/

	/* --- is in a file ? --- */
	if (ulCell >= pHeader->ulMaxRec) {				/* if overflow ...		*/
		return (SPLU_FILE_OVERFLOW);					/* overflow !		*/
	}

	/* --- move to the record position --- */
	if (PifSeekFile(hf, ulRecOffset, &ulMoved) != PIF_OK) {
		return (SPLU_FILE_BROKEN);						/* must be broken	*/
	}

	//don't read more than the buffer size
	//perhaps there should be a warning that these values don't align
	if (usBytes > sizeof(RECPLU210))
		usBytes = sizeof(RECPLU210);

	/* --- read a record directly --- */
	if (PifReadFile(hf, pucBuffer, usBytes) != usBytes) {
		return (SPLU_FILE_BROKEN);						/* must be broken	*/
	}

	/* --- exit ... --- */
	return (0);
}

/*
*==========================================================================
**    Synopsis:    ULONG	ComputeHash_64(WCHAR *pNum, USHORT usLen,ULONG ulDiv)
*
*       Input:    WCHAR	*	pStop;
	WCHAR		uchUpr[10],uchLw[10];
	ULONG		ulUp,ulLw,lHash = 0L;
*      Output:    PLUIF  *Pif->ParaPlu
*       InOut:    Nothing
*
**  Return    :
*     Normal End:  OP_PERFORM
*   Abnormal End:  OP_PLU_ERROR
*                  OP_FILE_NOT_FOUND
*                  OP_LOCK
*                  OP_NOT_IN_FILE
*
*
**  Description: Read a Plu paramater Individually.
*==========================================================================
*/

#ifdef	PEP
ULONG	ComputeHash_64HGV210(WCHAR *pNum, USHORT usLen,ULONG ulDiv){
	WCHAR	*	pStop;
	WCHAR		uchUpr[10],uchLw[10];
	ULONG		ulUp,ulLw,lHash = 0L;

	//memcpy(uchUpr,pNum,9); uchUpr[9] = '\0';
	wcsncpy(uchUpr, pNum, 9); uchUpr[9] = '\0';
	//memcpy(uchLw,&pNum[9],4); uchLw[4] = '\0';
	wcsncpy(uchLw,&pNum[9],5); uchLw[5] = '\0';
	ulUp = wcstoul((const WCHAR *)uchUpr,&pStop,10);
	ulLw = wcstoul((const WCHAR *)uchLw,&pStop,10);

	ulLw += (ULONG)((ULONG)(ulUp % ulDiv)*10000L);
	return	(ulLw % ulDiv);

	usLen = usLen;
}
#endif



/*
*==========================================================================
**    Synopsis:    SHORT   Op_GetPlu_HGV210(FILEHDR_HGV210 PluFileHed,
*                                        ULONG         culCounter,
*                                        PLUIF_HGV210    *pParaEntry);
*
*       Input: *pPluFileHed      Point to the Plu File headder.
*              culCounter        Counter(First entry is 0, next is not to be touched)
*      Output: *pParaEntry       Storage location for Plu if data.
*       InOut: Nothing
*
**  Return   : OP_PERFORM
*              OP_EOF
*              (file error is system halt)
*
**  Description: Pulls the Plu Number and Adjective from the Index file, then calculates
*				 the correct cell to look into in the temporary plu file(the R10 PLU copied file)
*				 it then reads the plu information from the temp file
**
*==========================================================================
*/
SHORT   Op_GetPlu_HGV210(FILEHDR_HGV210 *pPluFileHed,
                      ULONG         culCounter,
					  PLUIF_HGV210	*pParaEntry,
                      RECPLU210 *TempPlu)
{
//    USHORT					usParaBlkNo; JHHJ
    OPPLU_INDEXENTRY_HGV210  PluIndex_HGV210;
	ULONG					ulCell;
	USHORT			usStat;


    if (pPluFileHed->ulMaxRec <= culCounter) {
        return(OP_EOF);
    }

    /*  get Plu no in index file  JHHJ*/
    Op_ReadIndexPluFile_HGV210(pPluFileHed->ulOffsetIndex + (culCounter * sizeof(OPPLU_INDEXENTRY_HGV210)),
                    &PluIndex_HGV210, sizeof(OPPLU_INDEXENTRY_HGV210));

	//compute the cell required to be read in the file to find the correct information
	ulCell = ComputeHash_64HGV210(PluIndex_HGV210.auchPluNo, sizeof(PluIndex_HGV210.auchPluNo), pPluFileHed->ulHashKey);

	//read from the file (R10) for the PLU information
	PhyReadRecord_HGV210(hsOpPluFileHandle, ulCell, TempPlu, pPluFileHed);

	//Checks the PLU number read from the file, with the number given in the index file,
	//if they DO NOT match, it enters the loop and goes through the file until it finds it
	if (memcmp(&PluIndex_HGV210.auchPluNo, &TempPlu->aucPluNumber, (OP_PLU_LEN_HGV210 * sizeof(WCHAR))) != 0)
	{
		ulCell = 0;

		while(FOREVER){

				/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */

			if (usStat = PhyReadRecord_HGV210(hsOpPluFileHandle, ulCell, TempPlu, pPluFileHed)) {
				return (usStat);
			}

			if (memcmp(&PluIndex_HGV210.auchPluNo, &TempPlu->aucPluNumber, (OP_PLU_LEN_HGV210 * sizeof(WCHAR))) == 0)
			{
				break;
			}

			ulCell++;
		}


	}
	culCounter++;
	pParaEntry->ulCounter = culCounter;

    return(OP_PERFORM);
}


/*
*==========================================================================
**    Synopsis:    SHORT   Op_LockCheck_HGV210(USHORT usLockHnd)
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
SHORT   Op_LockCheck_HGV210(USHORT usLockHnd)
{

    if (( husOpLockHandle == 0x0000 ) || ( husOpLockHandle == usLockHnd )) {
         return(OP_PERFORM);
    }
    return(OP_LOCK);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpPluAllRead_HGV210(PLUIF_HGV210 *pPif,
*                                                USHORT usLockHnd)
*
*       Input: pPif -> culCounter
*              usLockHnd
*      Output: pPif -> usPluNo
*              pPif -> uchPluAdj
*              pPif -> ParaPlu
*       InOut: Nothing
*
**  Return   :
*     Normal End: OP_PERFORM
*                 OP_EOF
*   Abnormal End: OP_FILE_NOT_FOUND
*                 OP_LOCK
*
**  Description: Read All Plu .
*==========================================================================
*/
SHORT   OpPluAllRead_HGV210(PLUIF_HGV210 *pPif, USHORT usLockHnd, RECPLU210 *TempPlu)
{
    FILEHDR_HGV210 PluFileHed_HGV210;
    SHORT   sStatus;

    PifRequestSem(husOpSem);

    /*  LOCK CHECK */

    if (Op_LockCheck_HGV210(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return(OP_LOCK);
    }

	//gets the old file(R10) header  information
    Op_ReadPluFile_HGV210(OP_FILE_HEAD_POSITION, &PluFileHed_HGV210, sizeof(PluFileHed_HGV210));

	//Gets the plu information from the old(R10) plu file
    sStatus = Op_GetPlu_HGV210(&PluFileHed_HGV210, pPif->ulCounter, pPif, TempPlu);
/*    Op_ClosePluFileReleaseSem();*/

	PifReleaseSem(husOpSem);

    return(sStatus);
}


/*
*==========================================================================
**    Synopsis:    SHORT   OpPluAssign_HGV210(PLUIF *pPif, USHORT usLockHnd)
*
*       Input:  pPif->usPluNo
*               pPif->uchPluAdj
*               pPif->ParaPlu
*               usLockHnd
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return    :
*     Normal End: OP_NEW_PLU
*   Abnormal End: OP_CHANGE_PLU
*                 OP_FILE_NOT_FOUND
*                 OP_PLU_FULL
*                 OP_LOCK
*                 OP_PLU_ERROR
*
**  Description: Add and change plu record.
*==========================================================================
*/
SHORT OpPluAssign_HGV210(PLUIF *pPif, USHORT usLockHnd)
{
    MFINFO mfinfo;
    USHORT usResult;
    USHORT usRecPluLen;
    RECPLU recplu;
    RECPLU recpluold;
    PRECPLU pRecPlu;
    PRECPLU pRecPluOld;
    USHORT  usAdj, i;

    pRecPlu = &recplu;
    pRecPluOld = &recpluold;

    usAdj = (USHORT)pPif->uchPluAdj;
    usAdj--; /* decrement adjective code */

    /*  CHECK LOCK */

    PifRequestSem(husOpSem);

    if (Op_LockCheck(usLockHnd) == OP_LOCK)
    {
        PifReleaseSem(husOpSem);
        return OP_LOCK;
    }

    usResult = PluSenseFile(FILE_PLU,&mfinfo);
    if(usResult == SPLU_FILE_NOT_FOUND)
    {
        PifReleaseSem(husOpSem);
        return (SHORT)OpConvertPluError(usResult);
    }

    usRecPluLen = 0;
    memset(pRecPlu, 0, sizeof(pRecPlu));
    memset(pRecPluOld,0,sizeof(pRecPluOld));

    memcpy(pRecPlu->aucPluNumber,pPif->auchPluNo,sizeof(pRecPlu->aucPluNumber));
    pRecPlu->uchAdjective             = 1;  /* always adjective code is 1 on PLU record */
    /* pRecPlu->uchAdjective             = pPif->uchPluAdj; */
    pRecPlu->usDeptNumber             = pPif->ParaPlu.ContPlu.usDept;
    RflConv3bto4b(&pRecPlu->ulUnitPrice[usAdj],pPif->ParaPlu.auchPrice);
    pRecPlu->uchPriceMulWeight        = pPif->ParaPlu.uchPM;
    memcpy(pRecPlu->aucMnemonic,pPif->ParaPlu.auchPluName,sizeof(pRecPlu->aucMnemonic));
    memcpy(pRecPlu->aucStatus,pPif->ParaPlu.ContPlu.auchContOther,sizeof(pRecPlu->aucStatus));
    pRecPlu->uchReportCode            = pPif->ParaPlu.ContPlu.uchRept;
    pRecPlu->usLinkNumber             = pPif->ParaPlu.usLinkNo;
    pRecPlu->uchRestrict              = pPif->ParaPlu.uchRestrict;
    pRecPlu->uchTableNumber           = pPif->ParaPlu.uchTableNumber;
    pRecPlu->uchGroupNumber           = pPif->ParaPlu.uchGroupNumber;
    pRecPlu->uchPrintPriority         = pPif->ParaPlu.uchPrintPriority;
    pRecPlu->uchItemType              = pPif->ParaPlu.ContPlu.uchItemType;
    pRecPlu->uchLinkPPI               = pPif->ParaPlu.uchLinkPPI;
    pRecPlu->uchMixMatchTare          = 0;  /* reserved */
    pRecPlu->usFamilyCode             = pPif->ParaPlu.usFamilyCode;	/* ### ADD Saratoga (V1_0.02)(051800) */

    usResult = PluReplaceRecord(FILE_PLU,
                                (VOID*)pRecPlu,
                                (VOID*)pRecPluOld,
                                &usRecPluLen);
    if(!usResult)
    {
        if(!usRecPluLen)
        {
            PifReleaseSem(husOpSem);
            return OP_NEW_PLU;
        }
        else
        {
            /* preserve other adjective price */

            for (i=0; i< MAX_ADJECTIVE_NO; i++) {
                if (i != usAdj) {

                    pRecPlu->ulUnitPrice[i] = pRecPluOld->ulUnitPrice[i];
                }
            }
            usResult = PluReplaceRecord(FILE_PLU,
                                    (VOID*)pRecPlu,
                                    (VOID*)pRecPluOld,
                                    &usRecPluLen);
            PifReleaseSem(husOpSem);
            return OP_CHANGE_PLU;
        }
    }

    PifReleaseSem(husOpSem);

    return (SHORT)OpConvertPluError(usResult);
}
/*
*==========================================================================
**    Synopsis:
*
*       Input:    *Pif->auchPluNo
*                 *Pif->uchPluAdj
*                 usLockHnd
*      Output:    PLUIF  *Pif->ParaPlu
*       InOut:    Nothing
*
**  Return    :
*     Normal End:  OP_PERFORM
*   Abnormal End:  OP_PLU_ERROR
*                  OP_FILE_NOT_FOUND
*                  OP_LOCK
*                  OP_NOT_IN_FILE
*
*
**  Description: Read a Plu paramater Individually.
*==========================================================================
*/
SHORT OpPluIndRead_HGV210(PLUIF *pPif, USHORT usLockHnd)
{
    MFINFO mfinfo;
    USHORT usResult;
    USHORT usRecPluLen;
    RECPLU recplu;
    PRECPLU pRecPlu;
    ITEMNO itemno;
    PITEMNO pItemNumber;

    pRecPlu = &recplu;
    pItemNumber = &itemno;

    PifRequestSem(husOpSem);

    /*  LOCK CHECK */
    if (Op_LockCheck(usLockHnd) == OP_LOCK)
    {
        PifReleaseSem(husOpSem);
        return OP_LOCK;
    }

    usResult = PluSenseFile(FILE_OLD_PLU,&mfinfo);
    if(usResult == SPLU_FILE_NOT_FOUND)
    {
        PifReleaseSem(husOpSem);
        return (SHORT)OpConvertPluError(usResult);
    }


    memcpy(pItemNumber->aucPluNumber,pPif->auchPluNo,sizeof(pItemNumber->aucPluNumber));

    usResult = PluReadRecord(hsOpPluFileHandle,
                             pItemNumber,
							 1, /* adjective is always 1 */
                             /* (USHORT)pPif->uchPluAdj, */
                             (VOID*)pRecPlu,
                            &usRecPluLen);
    if(!usResult)
    {
        memcpy(pPif->auchPluNo,pRecPlu->aucPluNumber,sizeof(pPif->auchPluNo));
        /* pPif->uchPluAdj                   = pRecPlu->uchAdjective;*/
        pPif->ParaPlu.ContPlu.usDept      = pRecPlu->usDeptNumber;
        RflConv4bto3b(pPif->ParaPlu.auchPrice,pRecPlu->ulUnitPrice[pPif->uchPluAdj-1]);
        pPif->ParaPlu.uchPM               = pRecPlu->uchPriceMulWeight;
        memcpy(pPif->ParaPlu.auchPluName,pRecPlu->aucMnemonic,sizeof(pPif->ParaPlu.auchPluName));
        memcpy(pPif->ParaPlu.ContPlu.auchContOther,pRecPlu->aucStatus,sizeof(pPif->ParaPlu.ContPlu.auchContOther));
        pPif->ParaPlu.ContPlu.uchRept     = pRecPlu->uchReportCode;
        pPif->ParaPlu.usLinkNo            = pRecPlu->usLinkNumber;
        pPif->ParaPlu.uchRestrict         = pRecPlu->uchRestrict;
        pPif->ParaPlu.uchTableNumber      = pRecPlu->uchTableNumber;
        pPif->ParaPlu.uchGroupNumber      = pRecPlu->uchGroupNumber;
        pPif->ParaPlu.uchPrintPriority    = pRecPlu->uchPrintPriority;
        pPif->ParaPlu.ContPlu.uchItemType = pRecPlu->uchItemType;
        pPif->ParaPlu.uchLinkPPI          = pRecPlu->uchLinkPPI;
        pPif->ParaPlu.usFamilyCode        = pRecPlu->usFamilyCode;	/* ### ADD Saratoga (V1_0.02)(051800) */
    }

    PifReleaseSem(husOpSem);

    return(SHORT)OpConvertPluError(usResult);
}


/*
*==========================================================================
**    Synopsis:    SHORT   OpPluAssignHGV210(RECPLU* recplu, PLUIF *pPif, USHORT usLockHnd)
*
*       Input:  pPif->usPluNo
*               pPif->uchPluAdj
*               pPif->ParaPlu
*               usLockHnd
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return    :
*     Normal End: OP_NEW_PLU
*   Abnormal End: OP_CHANGE_PLU
*                 OP_FILE_NOT_FOUND
*                 OP_PLU_FULL
*                 OP_LOCK
*                 OP_PLU_ERROR
*
**  Description: Add and change plu record.
*==========================================================================
*/
SHORT OpPluAssignHGV210(RECPLU* recplu,PLUIF *pPif, USHORT usLockHnd)
{
    MFINFO mfinfo;
    USHORT usResult;
    USHORT usRecPluLen;
    RECPLU recpluold;
    PRECPLU pRecPlu;
    PRECPLU pRecPluOld;
    USHORT  usAdj, i;

    pRecPlu = recplu;
    pRecPluOld = &recpluold;

    usAdj = (USHORT)recplu->uchAdjective;
    usAdj--; /* decrement adjective code */

    /*  CHECK LOCK */

    PifRequestSem(husOpSem);

    if (Op_LockCheck(usLockHnd) == OP_LOCK)
    {
        PifReleaseSem(husOpSem);
        return OP_LOCK;
    }

    usResult = PluSenseFile(FILE_PLU,&mfinfo);
    if(usResult == SPLU_FILE_NOT_FOUND)
    {
        PifReleaseSem(husOpSem);
        return (SHORT)OpConvertPluError(usResult);
    }

    usRecPluLen = 0;
    memset(pRecPluOld,0,sizeof(pRecPluOld));

    usResult = PluReplaceRecord(FILE_PLU,
                                (VOID*)pRecPlu,
                                (VOID*)pRecPluOld,
                                &usRecPluLen);
    if(!usResult)
    {
        if(!usRecPluLen)
        {
            PifReleaseSem(husOpSem);
            return OP_NEW_PLU;
        }
        else
        {
            /* preserve other adjective price */

            for (i=0; i< MAX_ADJECTIVE_NO; i++) {
                if (i != usAdj) {

                    pRecPlu->ulUnitPrice[i] = pRecPluOld->ulUnitPrice[i];
                }
            }
            usResult = PluReplaceRecord(FILE_PLU,
                                        (VOID*)pRecPlu,
                                        (VOID*)pRecPluOld,
                                        &usRecPluLen);
            PifReleaseSem(husOpSem);
            return OP_CHANGE_PLU;
        }
    }

    PifReleaseSem(husOpSem);

    return (SHORT)OpConvertPluError(usResult);
}



/*
* ===========================================================================
*
**  Synopsis:   BOOL FileToCpn210File(int iFile,
*                                     LONG lOffset,
*                                     LONG lSize,
*                                     char *pszFileName)
*              iFile       handle of file
*              lOffset     file offset
*              lSize       file size
*              pszFileName pointer of file name
*
**  Return:     TRUE:   success
*              FALSE:  failure
*
**  Description: This function creates the Department file.
*
* ===========================================================================
*/
BOOL FileToPlu210File(HANDLE iFile, LONG lOffset, LONG lSize, WCHAR* pszFileName)
{
    FILEHDR_HGV210	PluFileHedOld;
    RECPLU210		oldPluRec;
	RECPLU			newPluRec;
	PLUIF_HGV210	tempPlu;
    PLUIF           newPlu;
    SHORT			sStatus;
    SHORT			nRet;
	int				i;
	ULONG			counter = 0;

	PifRequestSem(husOpSem);

	//Make a file handle for the old PLU File (R10) JHHJ
    if ((hsOpPluFileHandle = PifOpenFile((WCHAR FAR *)auchOP_PLU_OLD, (CHAR FAR *)auchOLD_FILE_READ_WRITE)) < 0) {
		PifReleaseSem(husOpSem);
		return(OP_FILE_NOT_FOUND);
    }

	//Read the header of the old file
    if ((PifReadFile(hsOpPluFileHandle, &PluFileHedOld, sizeof(FILEHDR_HGV210))) != sizeof(FILEHDR_HGV210)) {
		Op_ClosePluFileReleaseSem();
		return(OP_FILE_NOT_FOUND);
    }

		/* Create New PLU File based on the number of max records in the old file */
	if((sStatus = (SHORT)PluCreateFile(FILE_PLU, PluFileHedOld.ulMaxRec)) != SPLU_COMPLETED) {
		Op_ClosePluFileReleaseSem();
		return(sStatus);
    }

	//Make a file handle for the Temporay Index file so we can read from JHHJ
	if ((hsOpPluIndexFileHandle = PifOpenFile((WCHAR FAR *)szPluIndex, (CHAR FAR *)auchOLD_FILE_READ_WRITE)) < 0) {
		PifReleaseSem(husOpSem);
		return(OP_FILE_NOT_FOUND);
    }

    /* ----- Create OEP file ----- */
    if((sStatus = OpOepCreatFile(PluFileHedOld.ulMaxRec, 0)) != OP_PERFORM) {
		Op_ClosePluFileReleaseSem();
		return(sStatus);
    }
	//Create the new index file based on the number of max records in the old file
	if ((PluCreateFile(FILE_PLU_INDEX, PluFileHedOld.ulMaxRec)) != OP_PERFORM){
		return(OP_FILE_NOT_FOUND);
	}

    tempPlu.ulCounter = 0L;

    do {

         /* ----- Load PLU Records from PLU File(V3.4) until EOF ----- */

         nRet = OpPluAllRead_HGV210(&tempPlu, 0, &oldPluRec);

         if((nRet == OP_FILE_NOT_FOUND) || (nRet == OP_EOF)) 
		 {
           break;
         }

		   // Begin to Transfer the old PLU parameter to the new
		   // empty one, adjusting UCHARS to WCHARS when necessary
          memset(&newPluRec, 0x00, sizeof(newPluRec));

		  memcpy(&newPluRec.aucPluNumber, &oldPluRec.aucPluNumber, sizeof(newPluRec.aucPluNumber));
		  newPluRec.uchAdjective               = 1; /* adjectve is always 1 on PLU record */
		  newPluRec.usDeptNumber               = oldPluRec.usDeptNumber;
		  memcpy(newPluRec.ulUnitPrice, oldPluRec.ulUnitPrice, sizeof(oldPluRec.ulUnitPrice));
		  newPluRec.uchPriceMulWeight          = oldPluRec.uchPriceMulWeight;
		  for (i = 0; i < PLU_MNEMONIC_LEN; i++) {
				newPluRec.aucMnemonic[i] = oldPluRec.aucMnemonic[i];
		  }
		  
		  newPluRec.usFamilyCode		         = oldPluRec.usFamilyCode;	/* ### ADD Saratoga (V1_0.02)(051800) */
		  newPluRec.uchMixMatchTare			= oldPluRec.uchMixMatchTare;
 		  newPluRec.uchReportCode			     = oldPluRec.uchReportCode;
		  newPluRec.usLinkNumber		         = oldPluRec.usLinkNumber;
		  newPluRec.uchRestrict		         = oldPluRec.uchRestrict;
		  newPluRec.uchLinkPPI	             = (USHORT)oldPluRec.uchLinkPPI;
		  newPluRec.uchTableNumber	         = oldPluRec.uchTableNumber;
		  newPluRec.uchGroupNumber			 = oldPluRec.uchGroupNumber;
		  newPluRec.uchPrintPriority		     = oldPluRec.uchPrintPriority;
		  newPluRec.uchItemType				 = oldPluRec.uchItemType;
		  newPluRec.usBonusIndex = oldPluRec.usBonusIndex;
		  newPluRec.ulVoidTotalCounter  = oldPluRec.ulVoidTotalCounter;				// Void Total/Counter for each PLU
		  newPluRec.ulDiscountTotalCounter  = oldPluRec.ulDiscountTotalCounter;			// Discount Total/Counter for each PLU
		  newPluRec.ulFreqShopperPoints  = oldPluRec.ulFreqShopperPoints;			// Frequent Shopper Discount/Points (GP Feature)
		  memcpy(&newPluRec.auchAlphaNumRouting, &oldPluRec.auchAlphaNumRouting, sizeof(oldPluRec.auchAlphaNumRouting));
		  memcpy(&newPluRec.futureTotals, &oldPluRec.futureTotals, sizeof(oldPluRec.futureTotals));
											//   reorder count, current cost, original cost, etc.)
		  memcpy(&newPluRec.auchAltPluName, &oldPluRec.auchAltPluName, sizeof(oldPluRec.auchAltPluName));
		  newPluRec.uchColorPaletteCode	= oldPluRec.uchColorPaletteCode;
		  memcpy(&newPluRec.auchStatus2, &oldPluRec.auchStatus2, sizeof(oldPluRec.auchStatus2));
		  newPluRec.usTareInformation	= oldPluRec.usTareInformation;
		  memcpy(&newPluRec.aucStatus, &oldPluRec.aucStatus, sizeof(oldPluRec.aucStatus));
		  OpPluAssignHGV210(&newPluRec, &newPlu, 0);

           newPlu.ParaPlu.ContPlu.uchItemType = GETPLUTYPE(tempPlu.ParaPlu.ContPlu.usDept);

		//breaks out of the loop after it has assigned all of the records in the old file
		 counter++;
		 if( counter > (PluFileHedOld.ulCurRec))
		 {
			 break;
		 }

  } while(nRet == OP_PERFORM);


	PifCloseFile(hsOpPluIndexFileHandle);
	PifDeleteFile(szPluIndex);
    Op_ClosePluFileReleaseSem();


    return(sStatus);
}

//#endif