#if defined(POSSIBLE_DEAD_CODE)
/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  Saratoga     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server OPERATIONAL PARAMATER module, Create File (PLU PARAMATER, DEPT PARAMATER)
*   Category           : Client/Server OPERATIONAL PARAMATER module, Saratoga US HOSPITALITY APPLICATION.
*   Program Name       : NWOPV34.C
*  ------------------------------------------------------------------------  
*   Compile            : MS-C Ver. 7.00 A by Microsoft Corp.              
*   Memory Model       : Large Model
*   Options            : /c /Alfw /W4 /Zpi /G2s /Os
*  ------------------------------------------------------------------------
*   Abstract           :
*
*       USER I/F FUNCITONS
*
*  ------------------------------------------------------------------------
*    Update Histories  :
*   Date          : Ver.Rev  :NAME       :Description
*   Jan-31-99     : for pep  :K.Yanagida :Initial
*   May-18-00     :01.00.02  :K.Iwata    :Add Family Code
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
#include    "csopv34.h"

/*
*==========================================================================
**    Synopsis:    SHORT   OpPluMigrate34File(VOID)
*
*       Input: Nothing
*      Output: Nothing
*       InOut: Nothing
*
**  Return    :
*     Normal End: OP_PERFORM
*   Abnormal End: OP_NO_MAKE_FILE
*
**  Description: Create New PLU (Saratoga) File from Old PLU (Ver 3.4).
*==========================================================================
*/
SHORT   OpPluMigrate34File(VOID)
{
    OPPLU_FILEHED     PluFileHedOld;
    PLUIF_V34         PluOld;
    PLUIF             PluNew;
    SHORT sStatus;
    SHORT nRet;
    WCHAR szWork[WORK_SIZE];
    WCHAR szPluNo[PLU_MAX_DIGIT];

    LABELANALYSIS Label;
    BOOL          sStat;
    WCHAR uchItemType;


    PifRequestSem(husOpSem);


    if ((hsOpPluFileHandle = PifOpenFile((WCHAR FAR *)auchOP_PLU_OLD,
            (CHAR FAR *)auchOLD_FILE_READ_WRITE)) < 0) {
        return(OP_FILE_NOT_FOUND);
    }

    if ((PifReadFile(hsOpPluFileHandle, &PluFileHedOld,
                     sizeof(PluFileHedOld))) != sizeof(PluFileHedOld)) {
        return(OP_FILE_NOT_FOUND);
    }


    /* Create New PLU File */

    if((sStatus = (SHORT)PluCreateFile(FILE_PLU, PluFileHedOld.usNumberOfMaxPlu)) 
                != SPLU_COMPLETED) {

        Op_ClosePluFileReleaseSem();
        return(sStatus);
    }

    /* ----- Create PLU Index file ----- */

    if((sStatus = (SHORT)PluCreateFile(FILE_PLU_INDEX, PluFileHedOld.usNumberOfMaxPlu)) 
                != SPLU_COMPLETED) {

        Op_ClosePluFileReleaseSem();
        return(sStatus);
    }

    PluOld.ulCounter = 0L;

    do {

         /* ----- Load PLU Records from PLU File(V3.4) until EOF ----- */

         PluOld.usPluNo                  = 0;
         PluOld.uchPluAdj                = 0;
         memset(PluOld.ParaPlu.auchPluName, 0x00, sizeof(PluOld.ParaPlu.auchPluName));
         memset(PluOld.ParaPlu.auchPrice, 0x00, sizeof(PluOld.ParaPlu.auchPrice));
         PluOld.ParaPlu.uchTableNumber   = 0;
         PluOld.ParaPlu.uchGroupNumber   = 0;
         PluOld.ParaPlu.uchPrintPriority = 0;
         PluOld.ParaPlu.uchLinkPPI       = 0;
         PluOld.ParaPlu.ContPlu.uchDept  = 0;
         PluOld.ParaPlu.ContPlu.uchRept  = 0;
         memset(PluOld.ParaPlu.ContPlu.auchContOther, 0x00,
                sizeof(PluOld.ParaPlu.ContPlu.auchContOther));

         nRet = OpPluAllRead_V34(&PluOld, 0);

         if((nRet == OP_FILE_NOT_FOUND) || (nRet == OP_EOF)) {

           break;

         } else {

           memset(&PluNew, 0x00, sizeof(PluNew));

           memset(szWork, 0x00, sizeof(szWork));
           _itow((int)PluOld.usPluNo, szWork, 10);
           wsPepf(szPluNo, WIDE("%s"),szWork);

           Label.fchModifier = LA_NORMAL;   /* Normal        */
           memcpy(Label.aszScanPlu, szPluNo, PLU_MAX_DIGIT * sizeof(WCHAR));

           /* ----- Analyze PLU No. ----- */
           sStat = RflLabelAnalysis(&Label);

           if (sStat == LABEL_ERR) {

           } else {                   /* sStat == LABEL_OK */

              if (Label.uchType == LABEL_RANDOM) {

                 if (Label.uchLookup == LA_EXE_LOOKUP) {

                    memcpy(szPluNo, 
                             Label.aszMaskLabel, OP_PLU_LEN * sizeof(WCHAR));

                 } else {

                 }

              } else {

                memcpy(szPluNo, 
                         Label.aszLookPlu, OP_PLU_LEN * sizeof(WCHAR));
              }
          }

           memcpy(PluNew.auchPluNo, szPluNo, sizeof(PluNew.auchPluNo));

           PluNew.uchPluAdj = PluOld.uchPluAdj;
           PluNew.ulCounter = 0;
           PluNew.husHandle = 0;
           PluNew.usAdjCo   = 0;

           memcpy(PluNew.ParaPlu.auchPluName, 
                  PluOld.ParaPlu.auchPluName,
                  sizeof(PluOld.ParaPlu.auchPluName));
           memcpy(PluNew.ParaPlu.auchPrice, 
                  PluOld.ParaPlu.auchPrice,
                  sizeof(PluOld.ParaPlu.auchPrice));
           PluNew.ParaPlu.uchTableNumber      = PluOld.ParaPlu.uchTableNumber;
           PluNew.ParaPlu.uchGroupNumber      = PluOld.ParaPlu.uchGroupNumber;
           PluNew.ParaPlu.uchPrintPriority    = PluOld.ParaPlu.uchPrintPriority;
           PluNew.ParaPlu.uchLinkPPI          = PluOld.ParaPlu.uchLinkPPI;
           PluNew.ParaPlu.uchPM               = 0;
           PluNew.ParaPlu.uchRestrict         = 0;
           PluNew.ParaPlu.usLinkNo            = 0;
           PluNew.ParaPlu.ContPlu.usDept      = (USHORT)GETDEPTNO(PluOld.ParaPlu.ContPlu.uchDept);
           PluNew.ParaPlu.usFamilyCode        = 0; /* ### ADD Saratoga (V1_0.02)(051800) */


		// This code is a candidate for removal with the next release of source code
           PluNew.ParaPlu.ContPlu.uchItemType = GETPLUTYPE(PluOld.ParaPlu.ContPlu.uchDept);

           uchItemType                        = GETPLUTYPE(PluOld.ParaPlu.ContPlu.uchDept);

           if (uchItemType == TYPE_OPEN) {
              PluNew.ParaPlu.ContPlu.uchItemType = OPEN_PLU;
           } else if (uchItemType == TYPE_NONADJ) {
              PluNew.ParaPlu.ContPlu.uchItemType = NON_PLU;
           } else {
              PluNew.ParaPlu.ContPlu.uchItemType = ONE_PLU;
           }

           PluNew.ParaPlu.ContPlu.uchRept     = PluOld.ParaPlu.ContPlu.uchRept;
           memcpy(PluNew.ParaPlu.ContPlu.auchContOther,
                  PluOld.ParaPlu.ContPlu.auchContOther,
                  sizeof(PluOld.ParaPlu.ContPlu.auchContOther));

           /* ----- Assign PLU Record ----- */

           OpPluAssign(&PluNew, 0);

         }
         
   } while(nRet == OP_PERFORM);

    /* ----- Create OEP file ----- */
    if((sStatus = OpOepCreatFile((ULONG)PluFileHedOld.usNumberOfMaxPlu, 0)) != OP_PERFORM) {

        Op_ClosePluFileReleaseSem();
        return(sStatus);
    }
    Op_ClosePluFileReleaseSem();

    return(sStatus);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpPluAllRead_V34(PLUIF_V34 *pPif, 
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
SHORT   OpPluAllRead_V34(PLUIF_V34 *pPif, USHORT usLockHnd)
{
    OPPLU_FILEHED PluFileHed_V34;
    SHORT   sStatus;

    PifRequestSem(husOpSem);

    /*  LOCK CHECK */

    if (Op_LockCheck_V34(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return(OP_LOCK);
    }

/*
    if ((hsOpPluFileHandle = PifOpenFile((CHAR FAR *)auchOP_PLU_OLD,
                                         (CHAR FAR *)auchOLD_FILE_READ)) == PIF_ERROR_FILE_NOT_FOUND) {*/  /* pif_error_file_exist */
/*        PifReleaseSem(husOpSem);
        return(OP_FILE_NOT_FOUND);
    }
*/

    Op_ReadPluFile_V34(OP_FILE_HEAD_POSITION, &PluFileHed_V34, sizeof(PluFileHed_V34));

    sStatus = Op_GetPlu_V34(&PluFileHed_V34, pPif->ulCounter++, pPif);
/*    Op_ClosePluFileReleaseSem();*/

    PifReleaseSem(husOpSem);

	return(sStatus);
}


/*
*==========================================================================
**    Synopsis:    SHORT   Op_ReadPluFile_V34(ULONG  offulFileSeek, 
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
SHORT   Op_ReadPluFile_V34(ULONG  offulFileSeek, 
                           VOID   *pTableHeadAddress, 
                           USHORT usSizeofRead)
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
**    Synopsis:    SHORT   Op_GetPlu_V34(OPPLU_FILEHED *pPluFileHed, 
*                                        ULONG         culCounter, 
*                                        PLUIF         *pParaEntry);
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
**  Description: Get Plu No in order of the number.
*==========================================================================
*/
SHORT   Op_GetPlu_V34(OPPLU_FILEHED *pPluFileHed, 
                      ULONG         culCounter, 
                      PLUIF_V34     *pParaEntry)
{
    USHORT                usParaBlkNo;
    OPPLU_INDEXENTRY_V34  PluIndex_V34;

    if ((ULONG)pPluFileHed->usNumberOfResidentPlu <= culCounter) {
        return(OP_EOF);    
    }

    /*  get Plu no in index table */
    Op_ReadPluFile_V34(pPluFileHed->offulOfIndexTbl +
                   (ULONG)(culCounter * sizeof(OPPLU_INDEXENTRY_V34)), 
                    &PluIndex_V34, 
                    sizeof(OPPLU_INDEXENTRY_V34));

    pParaEntry->usPluNo = PluIndex_V34.usPluNo;              /* make output condition */
    pParaEntry->uchPluAdj = PluIndex_V34.uchAdjective;       /* make output condition */

    /*  get Plu no in paramater block no */
    Op_ReadPluFile_V34(pPluFileHed->offulOfIndexTblBlk + 
                   (ULONG)(culCounter * OP_PLU_INDEXBLK_SIZE), 
                   &usParaBlkNo, 
                   OP_PLU_INDEXBLK_SIZE);

    /*  get plu paramater in paramater table */
    Op_ReadPluFile_V34(pPluFileHed->offulOfParaTbl + 
                   ((ULONG)(usParaBlkNo - 1) * 
                   (ULONG)(sizeof(OPPLU_PARAENTRY_V34))),
                   &pParaEntry->ParaPlu, 
                   sizeof(OPPLU_PARAENTRY_V34));

    return(OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   Op_LockCheck_V34(USHORT usLockHnd)
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
SHORT   Op_LockCheck_V34(USHORT usLockHnd)
{

    if (( husOpLockHandle == 0x0000 ) || ( husOpLockHandle == usLockHnd )) {
         return(OP_PERFORM);
    }
    return(OP_LOCK);
}
/*
*==========================================================================
**    Synopsis:    SHORT   OpPluAssign_V34(PLUIF *pPif, USHORT usLockHnd)
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
SHORT OpPluAssign_V34(PLUIF *pPif, USHORT usLockHnd)
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
#endif