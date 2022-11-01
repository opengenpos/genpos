
#if defined(POSSIBLE_DEAD_CODE)
		// This file is a candidate for removal with the next release of source code


/*************************************************************************\
||                                                                       ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server OPERATIONAL PARAMATER module, Create File (PLU PARAMATER, DEPT PARAMATER)
*   Category           : Client/Server OPERATIONAL PARAMATER module, Saratoga US HOSPITALITY APPLICATION.
*   Program Name       : NWOPHGV10.C
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
#include    "csopHGV10.h"

SHORT	hsOpPluIndexFileHandle;

/*
*==========================================================================
**    Synopsis:    SHORT   OpPluMigrateHGV10File(VOID)
*
*       Input: Nothing
*      Output: Nothing
*       InOut: Nothing
*
**  Return    :
*     Normal End: OP_PERFORM
*   Abnormal End: OP_NO_MAKE_FILE
*
**  Description: Create New PLU (R20) File from Old PLU (HGV10).
*==========================================================================
*/
SHORT   OpPluMigrateHGV10File(VOID)
{
    FILEHDR_HGV10   PluFileHedOld;
    RECPLU10		oldPluRec;
	RECPLU			newPluRec;
	PLUIF_HGV10		tempPlu;
    PLUIF           newPlu;
    SHORT			sStatus;
    SHORT			nRet;
//    RECPLU pRecPluOld;
	UCHAR			uchBonus;
    WCHAR szWork[WORK_SIZE];
    WCHAR szPluNo[PLU_MAX_DIGIT];
    //UCHAR uchItemType;  not needed for R10 -> R20  JHHJ
	int				i, j;
	ULONG			counter = 0;


	 PifRequestSem(husOpSem);


	//Make a file handle for the old PLU File (R10) JHHJ
    if ((hsOpPluFileHandle = PifOpenFile((WCHAR FAR *)auchOP_PLU_OLD,
            (CHAR FAR *)auchOLD_FILE_READ_WRITE)) < 0) {
        return(OP_FILE_NOT_FOUND);
    }

	//Read the header of the old file
    if ((PifReadFile(hsOpPluFileHandle, &PluFileHedOld,
                      sizeof(FILEHDR_HGV10))) != sizeof(FILEHDR_HGV10)) {
        return(OP_FILE_NOT_FOUND);
    }

		/* Create New PLU File based on the number of max records in the old file */
	if((sStatus = (SHORT)PluCreateFile(FILE_PLU, PluFileHedOld.ulMaxRec))
                != SPLU_COMPLETED) {

        Op_ClosePluFileReleaseSem();
        return(sStatus);
    }

	//Make a file handle for the Temporay Index file so we can read from JHHJ
	if ((hsOpPluIndexFileHandle = PifOpenFile((WCHAR FAR *)szPluIndex,
            (CHAR FAR *)auchOLD_FILE_READ_WRITE)) < 0) {
        return(OP_FILE_NOT_FOUND);
    }

	//Create the new index file based on the number of max records in the old file
	if ((PluCreateFile(FILE_PLU_INDEX, PluFileHedOld.ulMaxRec)) != OP_PERFORM){
		return(OP_FILE_NOT_FOUND);
	}

    tempPlu.ulCounter = 0L;

    do {

         /* ----- Load PLU Records from PLU File(V3.4) until EOF ----- */

          nRet = OpPluAllRead_HGV10(&tempPlu, 0, &oldPluRec);

         if((nRet == OP_FILE_NOT_FOUND) || (nRet == OP_EOF)) {
           break;
         }

		   // Begin to Transfer the old PLU parameter to the new
		   // empty one, adjusting UCHARS to WCHARS when necessary
           memset(&newPlu, 0x00, sizeof(newPlu));
		   memset(&newPluRec, 0x00, sizeof(newPluRec));

		   for (i = 0; i < OP_PLU_LEN_HGV10; i++) {
				newPlu.auchPluNo[i] = tempPlu.auchPluNo[i];
		   }
			newPlu.uchPluAdj = tempPlu.uchPluAdj;
			newPlu.ulCounter = tempPlu.ulCounter;
			newPlu.husHandle = tempPlu.husHandle;
			newPlu.usAdjCo = tempPlu.usAdjCo;

			// Convert the OPPLU_PARAENTRY struc which is contained within the PLUIF struct
		   for (i = 0; i < OP_PLU_LEN_HGV10; i++) {
				newPlu.ParaPlu.auchPluName[i] = tempPlu.ParaPlu.auchPluName[i];
		   }

		   for (i = 0; i < OP_PLU_PRICE_SIZE_HGV10; i++) {
				newPlu.ParaPlu.auchPrice[i] = tempPlu.ParaPlu.auchPrice[i];
		   }
			newPlu.ParaPlu.uchTableNumber = tempPlu.ParaPlu.uchTableNumber;
			newPlu.ParaPlu.uchGroupNumber = tempPlu.ParaPlu.uchGroupNumber;
			newPlu.ParaPlu.uchPrintPriority = tempPlu.ParaPlu.uchPrintPriority;
			newPlu.ParaPlu.uchLinkPPI = tempPlu.ParaPlu.uchLinkPPI;
			newPlu.ParaPlu.uchPM = tempPlu.ParaPlu.uchPM;
			newPlu.ParaPlu.uchRestrict = tempPlu.ParaPlu.uchRestrict;
			newPlu.ParaPlu.usLinkNo = tempPlu.ParaPlu.usLinkNo;
			newPlu.ParaPlu.usFamilyCode = tempPlu.ParaPlu.usFamilyCode;

			// Convert the PLU_CONTROL struct within the OPPLU_PARAENTRY struct
			newPlu.ParaPlu.ContPlu.usDept = tempPlu.ParaPlu.ContPlu.uchDept;
			newPlu.ParaPlu.ContPlu.uchItemType = tempPlu.ParaPlu.ContPlu.uchItemType;
			newPlu.ParaPlu.ContPlu.uchRept = tempPlu.ParaPlu.ContPlu.uchRept;
		   for (i = 0; i < OP_PLU_CONT_OTHER; i++) {
				newPlu.ParaPlu.ContPlu.auchContOther[i] = tempPlu.ParaPlu.ContPlu.auchContOther[i];    //12
		   }

           memset(szWork, 0x00, sizeof(szWork));
//FIX THIS RPH           _itoa((int)tempPlu.usPluNo, (char *)szWork, 10);
//		   memcpy(tempWork, tempPlu.auchPluNo, sizeof(tempPlu.auchPluNo));
			{
			WCHAR *cpyWchar = szWork;
			UCHAR *cpyUchar = oldPluRec.aucPluNumber;
			*cpyWchar = '0';
			cpyWchar++;
				for (i = 0; i < sizeof(oldPluRec.aucPluNumber); i++) {
					*cpyWchar = (WCHAR) *cpyUchar;
					cpyWchar++;
					cpyUchar++;
				}
		   memcpy(&szPluNo, szWork, sizeof(szPluNo));
			}



		 // After being read from the old file, we set the old RECPLU to the new  RECPLU
		 // FORMAT

		// Convert the old 13 digit PLU number to the new 14 digit PLU number
			if (oldPluRec.aucPluNumber[0] != ' ') {
				newPluRec.aucPluNumber[0] = L'0';
			}
			else {
				newPluRec.aucPluNumber[0] = L' ';
			}
		  for (i = 0; i < OP_PLU_LEN_HGV10; i++) {
				j = i + 1;
				newPluRec.aucPluNumber[j] = oldPluRec.aucPluNumber[i];
		  }
		  newPluRec.uchAdjective               = 1; /* adjectve is always 1 on PLU record */
		  newPluRec.usDeptNumber               = oldPluRec.usDeptNumber;
		  memcpy(newPluRec.ulUnitPrice,oldPluRec.ulUnitPrice, sizeof(oldPluRec.ulUnitPrice));
		  newPluRec.uchPriceMulWeight          = oldPluRec.uchPriceMulWeight;
		  for (i = 0; i < PLU_MNEMONIC_LEN; i++) {
				newPluRec.aucMnemonic[i] = oldPluRec.aucMnemonic[i];
		  }
		  newPluRec.uchReportCode			     = oldPluRec.uchReportCode;
		  newPluRec.usLinkNumber		         = oldPluRec.usLinkNumber;
		  newPluRec.uchRestrict		         = oldPluRec.uchRestrict;
		  newPluRec.uchTableNumber	         = oldPluRec.uchTableNumber;
		  newPluRec.uchGroupNumber			 = oldPluRec.uchGroupNumber;
		  newPluRec.uchPrintPriority		     = oldPluRec.uchPrintPriority;
		  newPluRec.uchItemType				 = oldPluRec.uchItemType;
		  newPluRec.uchLinkPPI	             = (USHORT)oldPluRec.uchLinkPPI;
		  newPluRec.usFamilyCode		         = oldPluRec.usFamilyCode;	/* ### ADD Saratoga (V1_0.02)(051800) */
		  memcpy(&newPluRec.aucStatus, &oldPluRec.aucStatus, sizeof(oldPluRec.aucStatus));
		  if(oldPluRec.aucStatus[3] & BONUS_MASK_DATA)
		  {
			  uchBonus = (oldPluRec.aucStatus[3] & BONUS_MASK_DATA);
			  newPluRec.usBonusIndex |= uchBonus;
			  oldPluRec.aucStatus[3]&= ~BONUS_MASK_DATA;
		  }

			OpPluAssignHGV10(&newPluRec, &newPlu, 0);


#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
           newPlu.ParaPlu.ContPlu.uchItemType = GETPLUTYPE(tempPlu.ParaPlu.ContPlu.uchDept);
#endif

		//breaks out of the loop after it has assigned all of the records in the old file
		 counter++;
		 if( counter > (PluFileHedOld.ulCurRec))
		 {
			 break;
		 }

  } while(nRet == OP_PERFORM);

    /* ----- Create OEP file ----- */
    if((sStatus = OpOepCreatFile(PluFileHedOld.ulMaxRec, 0)) != OP_PERFORM) {

        Op_ClosePluFileReleaseSem();
        return(sStatus);
    }
	PifCloseFile(hsOpPluIndexFileHandle);
	PifDeleteFile(szPluIndex);
    Op_ClosePluFileReleaseSem();


    return(sStatus);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpPluAllRead_HGV10(PLUIF_HGV10 *pPif,
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
SHORT   OpPluAllRead_HGV10(PLUIF_HGV10 *pPif, USHORT usLockHnd, RECPLU10 *TempPlu)
{
    FILEHDR_HGV10 PluFileHed_HGV10;
    SHORT   sStatus;

    PifRequestSem(husOpSem);

    /*  LOCK CHECK */

    if (Op_LockCheck_HGV10(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return(OP_LOCK);
    }

	//gets the old file(R10) header  information
    Op_ReadPluFile_HGV10(OP_FILE_HEAD_POSITION, &PluFileHed_HGV10, sizeof(PluFileHed_HGV10));

	//Gets the plu information from the old(R10) plu file
    sStatus = Op_GetPlu_HGV10(&PluFileHed_HGV10, pPif->ulCounter, pPif, TempPlu);
/*    Op_ClosePluFileReleaseSem();*/

	PifReleaseSem(husOpSem);

    return(sStatus);
}

/*
*==========================================================================
**    Synopsis:    SHORT   Op_ReadPluFile_HGV10(ULONG  offulFileSeek,
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
SHORT   Op_ReadPluFile_HGV10(ULONG  offulFileSeek,
                           VOID   *pTableHeadAddress,
                           ULONG usSizeofRead)
{
    ULONG   ulActualPosition;

    if (usSizeofRead == 0x00) {
        PifAbort(MODULE_OP_PLU, FAULT_ERROR_FILE_READ);
    }
    if ((PifSeekFile(hsOpPluFileHandle, offulFileSeek,
                     &ulActualPosition)) < 0) {
        PifAbort(MODULE_OP_PLU, FAULT_ERROR_FILE_SEEK);
    }
    if ((PifReadFile(hsOpPluFileHandle,
                    pTableHeadAddress, usSizeofRead)) != usSizeofRead) {
        PifAbort(MODULE_OP_PLU, FAULT_ERROR_FILE_READ);
    }

    return(OP_PERFORM);
}


/*
*==========================================================================
**    Synopsis:    SHORT   Op_ReadIndexPluFile_HGV10(ULONG  offulFileSeek,
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
SHORT   Op_ReadIndexPluFile_HGV10(ULONG  offulFileSeek,
                           VOID   *pTableHeadAddress,
                           ULONG usSizeofRead)
{
    ULONG   ulActualPosition;

	//added JHHJ
    if (usSizeofRead == 0x00) {
        PifAbort(MODULE_OP_PLU, FAULT_ERROR_FILE_READ);
    }
    if ((PifSeekFile(hsOpPluIndexFileHandle, offulFileSeek,
                     &ulActualPosition)) < 0) {
        PifAbort(MODULE_OP_PLU, FAULT_ERROR_FILE_SEEK);
    }
    if ((PifReadFile(hsOpPluIndexFileHandle,
                    pTableHeadAddress, usSizeofRead)) != usSizeofRead) {
        PifAbort(MODULE_OP_PLU, FAULT_ERROR_FILE_READ);
    }

    return(OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   Op_GetPlu_HGV10(FILEHDR_HGV10 PluFileHed,
*                                        ULONG         culCounter,
*                                        PLUIF_HGV10    *pParaEntry);
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
SHORT   Op_GetPlu_HGV10(FILEHDR_HGV10 *pPluFileHed,
                      ULONG         culCounter,
					  PLUIF_HGV10	*pParaEntry,
                      RECPLU10 *TempPlu)
{
//    USHORT					usParaBlkNo; JHHJ
    OPPLU_INDEXENTRY_HGV10  PluIndex_HGV10;
	ULONG					ulCell;
	USHORT			usStat;


    if (pPluFileHed->ulMaxRec <= culCounter) {
        return(OP_EOF);
    }

    /*  get Plu no in index file  JHHJ*/
    Op_ReadIndexPluFile_HGV10(pPluFileHed->ulOffsetIndex +
                   (ULONG)(culCounter * sizeof(OPPLU_INDEXENTRY_HGV10)),
                    &PluIndex_HGV10,
                    sizeof(OPPLU_INDEXENTRY_HGV10));

	//compute the cell required to be read in the file to find the correct information
	ulCell = ComputeHash_64HGV10(PluIndex_HGV10.auchPluNo, sizeof(PluIndex_HGV10.auchPluNo), pPluFileHed->ulHashKey);

	//read from the file (R10) for the PLU information
	PhyReadRecord_HGV10(hsOpPluFileHandle, ulCell, TempPlu, pPluFileHed);

	//Checks the PLU number read from the file, with the number given in the index file,
	//if they DO NOT match, it enters the loop and goes through the file until it finds it
	if (memcmp(&PluIndex_HGV10.auchPluNo, &TempPlu->aucPluNumber, OP_PLU_LEN_HGV10) != 0)
	{
		ulCell = 0;

		while(FOREVER){

				/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */

			if (usStat = PhyReadRecord_HGV10(hsOpPluFileHandle, ulCell, TempPlu, pPluFileHed)) {
				return (usStat);
			}

			if (memcmp(&PluIndex_HGV10.auchPluNo, &TempPlu->aucPluNumber, OP_PLU_LEN_HGV10) == 0)
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
**    Synopsis:    SHORT   Op_LockCheck_HGV10(USHORT usLockHnd)
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
SHORT   Op_LockCheck_HGV10(USHORT usLockHnd)
{

    if (( husOpLockHandle == 0x0000 ) || ( husOpLockHandle == usLockHnd )) {
         return(OP_PERFORM);
    }
    return(OP_LOCK);
}
/*
*==========================================================================
**    Synopsis:    SHORT   OpPluAssign_HGV10(PLUIF *pPif, USHORT usLockHnd)
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
SHORT OpPluAssign_HGV10(PLUIF *pPif, USHORT usLockHnd)
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
SHORT OpPluIndRead_HGV10(PLUIF *pPif, USHORT usLockHnd)
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
ULONG	ComputeHash_64HGV10(UCHAR *pNum, USHORT usLen,ULONG ulDiv){
	char	*	pStop;
	UCHAR		uchUpr[10],uchLw[10];
	ULONG		ulUp,ulLw,lHash = 0L;

	memcpy(uchUpr,pNum,9); uchUpr[9] = '\0';
	memcpy(uchLw,&pNum[9],4); uchLw[4] = '\0';
	ulUp = strtoul((const char *)uchUpr,&pStop,10);
	ulLw = strtoul((const char *)uchLw,&pStop,10);

	ulLw += (ULONG)((ULONG)(ulUp % ulDiv)*10000L);
	return	(ulLw % ulDiv);

	usLen = usLen;
}
#endif

/**
;========================================================================
;
;   function : Read a record
;
;   format : USHORT		PhyReadRecord_HGV10(hf, usCell, pucBuffer, pHeader);
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


USHORT	PhyReadRecord_HGV10(USHORT hf, ULONG ulCell, RECPLU10 *pucBuffer, FILEHDR_HGV10 *pHeader)
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

	/* --- read a record directly --- */

	if (PifReadFile(hf, pucBuffer, usBytes) != usBytes) {
		return (SPLU_FILE_BROKEN);						/* must be broken	*/
	}

	/* --- exit ... --- */

	return (0);
}


/*
*==========================================================================
**    Synopsis:    SHORT   OpPluAssignHGV10(PLUIF *pPif, USHORT usLockHnd)
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
SHORT OpPluAssignHGV10(RECPLU* recplu,PLUIF *pPif, USHORT usLockHnd)
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

#endif