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
*   Title              : Client/Server OPERATIONAL PARAMATER module, Program List
*   Category           : Client/Server OPERATIONAL PARAMATER, NCR2170 US HOSPITALITY APPLICATION
*   Program Name       : NWOP.C
*  ------------------------------------------------------------------------
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.
*   Memory Model       : Medium Model
*   Options            : /c /AM /Gs /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*   Abstract           :The provided function names are as follows.
*
*   OpPluAssign        * Assign/addition plu *
*   OpDeptAssign       * Assign/addition dept *
*   OpPluDelete        * Delete plu *
*   OpPluFileUpDate    * Update Plu file *
*   OpPluIndRead       * Read Plu individual *
*   OpDeptIndRead      * Read Dept individual *
*   OpPluAllRead       * Read plu all *
*   OpCpnAssign        * Assign/addition Combination Coupon *
*   OpCpnIndRead       * Read Combination Coupon individual *
*   OpCstrAssign       * Assign/addition Control String *
*   OpCstrDelete       * Delete Control String *
*   OpCstrIndRead      * Read Control String individual *
*   OpPpiAssign                 * Assign/addition PPI record,   R3.1
*   OpPpiIndRead                * Individual Read PPI record,   R3.1
*   OpMldAssign        * Assign/addition Mld Mnemonics, V3.3 *
*   OpMldIndRead       * Individual Read Mld Mnemonics, V3.3 *
*
*  ------------------------------------------------------------------------
*   Update Histories  :
*   Date      :Ver.Rev :NAME       :Description
*   Nov-08-93 :00.00.01:H.Yamaguchi:Modified 3000 PLU for Hotel Model
*   Mar-07-95 :03.00.00:H.Ishida   :Add process of Combination Coupon & OEP
*   Apr-13-95 :03.00.00:H.Ishida   :Add Control String
*   Feb-01-96 :03.00.05:M.Suzuki   :Add PPI
*   Sep-09-98 :03.03.00:A.Mitsui   :Add MLD
*   Nov-30-99 :        :K.Yanagida :NCR2172
*   May-18-00 :01.00.02:K.Iwata    :Add Family Code
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
#include	"AW_Interfaces.h"
#include    <ecr.h>
#include    <r20_pif.h>
#include    <log.h>
#include    "mypifdefs.h"
#include    "rfl.h"
#include    "plu.h"
#include    "paraequ.h"
#include    "para.h"
#include    "csop.h"
#include    "appllog.h"
#include    "applini.h"
#include    "csopin.h"

USHORT  husOpSem;
USHORT  husOpLockHandle;
SHORT   hsOpPluFileHandle;
SHORT   hsOpDeptFileHandle;
SHORT   hsOpOepFileHandle;          /* Saratoga */
SHORT   hsOpCpnFileHandle;
SHORT   hsOpCstrFileHandle;
/* === Add PPI (Promotional Pricing Item) File (Release 3.1) BEGIN === */
SHORT   hsOpPpiFileHandle;
/* === Add PPI (Promotional Pricing Item) File (Release 3.1) END === */
SHORT   hsOpNewPluFileHandle;

SHORT   hsOpMldFileHandle;  /* V3.3 */

/*================================================*/
/* This Define is Open parameter of PifOpen       */
/*================================================*/
UCHAR FARCONST auchNEW_FILE_WRITE[] = "nw";
UCHAR FARCONST auchNEW_FILE_READ_WRITE[]  = "nwr";
UCHAR FARCONST auchOLD_FILE_WRITE[] = "ow";
UCHAR FARCONST auchOLD_FILE_READ_WRITE[]  = "owr";
UCHAR FARCONST auchOLD_FILE_READ[] = "or";

/*================================================*/
/* This Define is File Name parameter of PifOpen  */
/*================================================*/
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
 /* Saratoga */
WCHAR FARCONST auchOP_PLU[]		=  WIDE("PARAMPLU");
#endif
WCHAR FARCONST auchOP_PLU[]		=  WIDE("PARAM$LU");
WCHAR FARCONST auchOP_DEPT[]	=  WIDE("PARAMDEP");
WCHAR FARCONST auchOP_CPN[]		=  WIDE("PARAMCPN");
WCHAR FARCONST auchOP_CSTR[]	=  WIDE("PARAMCST");
/* === Add PPI (Promotional Pricing Item) File (Release 3.1) BEGIN === */
WCHAR FARCONST auchOP_PPI[ ] = WIDE("PARAMPPI");
/* === Add PPI (Promotional Pricing Item) File (Release 3.1) END === */

WCHAR FARCONST auchOP_MLD[]  = WIDE("PARAMMLD");      /* V3.3 */
WCHAR FARCONST auchOP_OEP[]  = WIDE("PARAMOEP");      /* Saratoga */

WCHAR FARCONST *auchOP_OEP_TABLE[] = {
	L"PARAMOEP",
	L"PARAMOEA",
	L"PARAMOEB",
	L"PARAMOEC",
	L"PARAMOED",
	0
};

/*=================================================*/
/* PARAM!PL file is used only PEP apllication      */
/*=================================================*/
WCHAR FARCONST auchOP_PLU_OLD[] = WIDE("PARAM!LU");


/*===========================================================*/
/* PARAMP34 file is used only PEP apllication, Saratoga      */
/*===========================================================*/
WCHAR FARCONST auchOP_PLU_V34[] = WIDE("PARAMPL!");

/*========================================================*/
/* This table is used AC 75 or AC175 by 2170 Super Mode   */
/*========================================================*/
UCHAR FARCONST auchClassNoSup[] = {CLASS_PARAPLUNOMENU,    /* AC 4      */
                                   CLASS_PARAROUNDTBL,     /* AC 84     */
                                   CLASS_PARADISCTBL,      /* AC 86     */
                                   CLASS_PARAPROMOTION,    /* AC 88     */
                                   CLASS_PARACURRENCYTBL,  /* AC 89     */
                                   CLASS_PARATARE,         /* AC 111    */
                                   CLASS_PARADEPTNOMENU,   /* AC 115 Saratoga */
                                   CLASS_PARAMENUPLUTBL,   /* AC 116    */
                                   CLASS_PARATAXRATETBL,   /* AC 127    */
                                   CLASS_PARATAXTBL1,      /* AC 124    */
                                   CLASS_PARARESTRICTION,  /* AC 170 Saratoga */
                                   CLASS_PARABOUNDAGE,     /* AC 208 Saratoga */
                                   CLASS_PARACTLTBLMENU,   /* AC 5      */
                                   CLASS_PARAMANUALTKITCH, /* AC 6      */
                                   CLASS_PARACASHABASSIGN, /* AC 7      */
                                   CLASS_PARASUPLEVEL,     /* P  8      */
                                   CLASS_PARATONECTL,      /* AC 169    */
                                   CLASS_PARASOFTCHK,      /* AC 87  EH */
                                   CLASS_PARAOEPTBL,       /* AC 160    */
                                   CLASS_PARAFXDRIVE,      /* AC 162    */
                                   CLASS_PARASERVICETIME,  /* AC 133, R3.1 */
                                   CLASS_PARAPIGRULE,      /* AC 130 EH */
                                   CLASS_PARALABORCOST,    /* AC 154, R3.1 */
                                   CLASS_PARADISPPARA,     /* AC 137, R3.1 */
                                   OP_EOT
                                  };
UCHAR FARCONST auchClassNoAll[] = {CLASS_PARAMDC,          /* P  1      */
                                   CLASS_PARAFSC,          /* P  3      */
                                   CLASS_PARASECURITYNO,   /* P  6      */
                                   CLASS_PARASUPLEVEL,     /* P  8      */
                                   CLASS_PARAACTCODESEC,   /* P  9      */
                                   CLASS_PARATRANSHALO,    /* P 10      */
                                   CLASS_PARAHOURLYTIME,   /* P 17      */
                                   CLASS_PARASLIPFEEDCTL,  /* P 18      */
                                   CLASS_PARATRANSMNEMO,   /* P 20      */
                                   CLASS_PARALEADTHRU,     /* P 21      */
                                   CLASS_PARAREPORTNAME,   /* P 22      */
                                   CLASS_PARASPECIALMNEMO, /* P 23      */
                                   CLASS_PARAADJMNEMO,     /* P 46      */
                                   CLASS_PARAPRTMODI,      /* P 47      */
                                   CLASS_PARAMAJORDEPT,    /* P 48      */
                                   CLASS_PARACHAR24,       /* P 57      */
                                   CLASS_PARAKDSIP,        /* P 51 Saratoga */
                                   CLASS_PARAAUTOALTKITCH, /* P 49      */
                                   CLASS_PARAFLEXMEM,      /* P  2      */
                                   CLASS_PARATTLKEYTYP,    /* P 60      */
                                   CLASS_PARATTLKEYCTL,    /* P 61      */
                                   CLASS_PARATEND,         /* P 62, V3.3*/
                                   CLASS_PARAAUTOCPN,	   /* P	 67		*/
                                   CLASS_PARAPCIF,         /* P 39      */
                                   CLASS_PARAPLUNOMENU,    /* AC 4      */
                                   CLASS_PARAROUNDTBL,     /* AC 84     */
                                   CLASS_PARADISCTBL,      /* AC 86     */
                                   CLASS_PARAPROMOTION,    /* AC 88     */
                                   CLASS_PARACURRENCYTBL,  /* AC 89     */
                                   CLASS_PARATARE,         /* AC 111    */
                                   CLASS_PARAMENUPLUTBL,   /* AC 116    */
                                   CLASS_PARATAXRATETBL,   /* AC 127    */
                                   CLASS_PARATAXTBL1,      /* AC 124    */
                                   CLASS_PARACTLTBLMENU,   /* AC 5      */
                                   CLASS_PARAMANUALTKITCH, /* AC 6      */
                                   CLASS_PARACASHABASSIGN, /* AC 7      */
                                   CLASS_PARATONECTL,      /* AC 169    */
                                   CLASS_PARASOFTCHK,      /* AC 87  EH */
                                   CLASS_PARASHRPRT,       /* P  50  EH */
                                   CLASS_PARAPRESETAMOUNT, /* P  15  EH */
                                   CLASS_PARAHOTELID,      /* P  54  HO */
                                   CLASS_PARAOEPTBL,       /* AC 160    */
                                   CLASS_PARAFXDRIVE,      /* AC 162    */
                                   CLASS_PARASERVICETIME,  /* AC 133, R3.1 */
                                   CLASS_PARAPIGRULE,      /* AC 130 EH */
                                   CLASS_PARALABORCOST,    /* AC 154, R3.1 */
                                   CLASS_PARADISPPARA,     /* AC 137, R3.1 */
                                   CLASS_PARADEPTNOMENU,   /* AC 115 Saratoga */
                                   CLASS_PARARESTRICTION,  /* AC 170 Saratoga */
                                   CLASS_PARABOUNDAGE,     /* AC 208 Saratoga */
                                   OP_EOT};
UCHAR FARCONST auchFlexMem[] =  {FLEX_DEPT_ADR,
                                 FLEX_PLU_ADR,
                                 FLEX_WT_ADR,
                                 FLEX_CAS_ADR,
                                 FLEX_GC_ADR,
                                 FLEX_ETK_ADR,
                                 FLEX_ITEMSTORAGE_ADR,
                                 FLEX_CONSSTORAGE_ADR,
                                 FLEX_CPN_ADR,
                                 FLEX_CSTRING_ADR,
                                 FLEX_PROGRPT_ADR,    /* Programmable Report, R3.1 */
                                 FLEX_PPI_ADR,        /* PPI Table,           R3.1 */
                                 FLEX_MLD_ADR,        /* MLD Mnemonic,        V3.3 */
                                 OP_EOT};

#define __NWOP_PROT__


//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------

struct {
	TCHAR  *auchFileName;
	USHORT  usMaxRecordCount;
	USHORT  usRecordSize;
} OpMnemonicsFileTable[] = {
	{0, 0},
	{L"PARAMMLD", MAX_MLDMNEMO_SIZE, STD_MLD_MNEMONICS_LEN},
	{L"PARAMRSN", MAX_RSNMNEMO_SIZE, STD_RSN_MNEMONICS_LEN},
	{0, 0}
};

SHORT   Op_MnemonicsFileOpenFile(USHORT usMnemonicFileId)
{
	TCHAR  *auchFileName;

	if (0 == usMnemonicFileId || usMnemonicFileId > sizeof(OpMnemonicsFileTable)/sizeof(OpMnemonicsFileTable[0])) {
		return -1;
	}

	auchFileName = OpMnemonicsFileTable[usMnemonicFileId].auchFileName;

    hsOpMldFileHandle = PifOpenFile(auchFileName, auchOLD_FILE_READ_WRITE);

	return hsOpMldFileHandle;
}

SHORT   OpMnemonicsFileCreate (USHORT usMnemonicFileId, USHORT usNumberOfAddress, USHORT usLockHnd)
{
    OPMLD_FILEHED   MldFileHed = { 0 };
    ULONG           ulActualPosition;
    ULONG           ulNewSize;
    ULONG           ulWriteOffset;
    TCHAR           *auchMldFile[128];
	TCHAR           *auchFileName;
	USHORT          usRecordSize;

	if (0 == usMnemonicFileId || usMnemonicFileId > sizeof(OpMnemonicsFileTable)/sizeof(OpMnemonicsFileTable[0])) {
		return OP_NO_MAKE_FILE;
	}

	auchFileName = OpMnemonicsFileTable[usMnemonicFileId].auchFileName;
	usRecordSize = OpMnemonicsFileTable[usMnemonicFileId].usRecordSize;

    PifRequestSem(husOpSem);

    Op_LockCheck(usLockHnd);  /* Dummy */

    /* Delete MLD File */
    PifDeleteFile(auchFileName);

    /* Check Number of MLD */
    if (0 == usNumberOfAddress) {
        PifReleaseSem(husOpSem);
        return (OP_PERFORM);
    }

    if ((hsOpMldFileHandle = PifOpenFile(auchFileName, auchNEW_FILE_WRITE)) < 0) {
        PifAbort(MODULE_OP_MLD, FAULT_ERROR_FILE_OPEN);
    }

    /* Check file size and creat file */
    ulNewSize = ( ULONG )( sizeof(OPMLD_FILEHED) + ( usRecordSize * usNumberOfAddress ));
    if (PifSeekFile(hsOpMldFileHandle, ulNewSize, &ulActualPosition) < 0) {   /* PIF_ERROR_DISK_FULL, OpMldCreatFile() */
        PifCloseFile(hsOpMldFileHandle);
        PifDeleteFile(auchFileName);              /* Can't make file then delete file */
        PifReleaseSem(husOpSem);
        return (OP_NO_MAKE_FILE);
    }

    ulWriteOffset = OP_FILE_HEAD_POSITION;
	memset (&MldFileHed, 0, sizeof(MldFileHed));
	MldFileHed.usNumberOfAddress = OpMnemonicsFileTable[usMnemonicFileId].usMaxRecordCount;
    Op_WriteMldFile( ulWriteOffset, &MldFileHed, sizeof(OPMLD_FILEHED));
    ulWriteOffset += ( ULONG )sizeof(OPMLD_FILEHED);

    /* Clear Contents of File */
    memset( auchMldFile, 0x00, sizeof( auchMldFile ));

    while( ulWriteOffset < ulNewSize ) {
        Op_WriteMldFile( ulWriteOffset, auchMldFile, usRecordSize);  // OpMldCreatFile() - create the MLD mnemonics file.
        ulWriteOffset += ( ULONG )usRecordSize;
    }

    Op_CloseMldFileReleaseSem();
    return (OP_PERFORM);
}

SHORT   OpMnemonicsFileCheckAndCreatFile(USHORT usMnemonicFileId, USHORT usNumberOfAddress, USHORT usLockHnd)
{
    SHORT   sStatus;

    if ((sStatus = Op_MnemonicsFileOpenFile(usMnemonicFileId)) < 0) {
		if (sStatus != OP_NO_MAKE_FILE) {
			sStatus = OpMnemonicsFileCreate (usMnemonicFileId, usNumberOfAddress, usLockHnd);
			return (sStatus);
		} else {
			return (sStatus);
		}
    }

    PifCloseFile(hsOpMldFileHandle);
	hsOpMldFileHandle = -1;
    return (OP_PERFORM);
}

SHORT   OpMnemonicsFileCheckAndDeleteFile(USHORT usMnemonicFileId, USHORT usNumberOfAddress, USHORT usLockHnd)
{
    OPMLD_FILEHED MldFileHed = { 0 };

    Op_LockCheck(usLockHnd);

    if ((Op_MnemonicsFileOpenFile(usMnemonicFileId)) == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        return (OP_PERFORM);
    }

    Op_ReadMldFile(OP_FILE_HEAD_POSITION, &MldFileHed, sizeof(OPMLD_FILEHED));

    if ( MldFileHed.usNumberOfAddress < usNumberOfAddress ) {
		MldFileHed.usNumberOfAddress = usNumberOfAddress;
		Op_WriteMldFile (OP_FILE_HEAD_POSITION, &MldFileHed, sizeof(OPMLD_FILEHED));
	}

    PifCloseFile(hsOpMldFileHandle);

    return (OP_PERFORM);
}

SHORT   OpMnemonicsFileAssign(OPMNEMONICFILE *pMnemonicFile, USHORT usLockHnd)
{
    OPMLD_FILEHED MldFileHed = { 0 };
	TCHAR           *auchFileName;
	USHORT          usRecordSize, usMaxRecordCount;

    /*  CHECK LOCK */
	if (0 == pMnemonicFile->usMnemonicFileId || pMnemonicFile->usMnemonicFileId > sizeof(OpMnemonicsFileTable)/sizeof(OpMnemonicsFileTable[0])) {
		return 0;
	}

	auchFileName = OpMnemonicsFileTable[pMnemonicFile->usMnemonicFileId].auchFileName;
	usRecordSize = OpMnemonicsFileTable[pMnemonicFile->usMnemonicFileId].usRecordSize;
	usMaxRecordCount = OpMnemonicsFileTable[pMnemonicFile->usMnemonicFileId].usMaxRecordCount;

    PifRequestSem(husOpSem);

    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return (OP_LOCK);
    }

    /*  OPEN FILE  */
    if (Op_MnemonicsFileOpenFile(pMnemonicFile->usMnemonicFileId) == PIF_ERROR_FILE_NOT_FOUND) {
        PifReleaseSem(husOpSem);
        return (OP_FILE_NOT_FOUND);
    }

    Op_ReadMldFile(OP_FILE_HEAD_POSITION, &MldFileHed, sizeof(OPMLD_FILEHED));

	// If the file header indicates a smaller max record count than we show then
	// this is probably due to a new release using an older file so update the file.
	if (MldFileHed.usNumberOfAddress < usMaxRecordCount) {
		MldFileHed.usNumberOfAddress = usMaxRecordCount;
		Op_WriteMldFile (OP_FILE_HEAD_POSITION, &MldFileHed, sizeof(OPMLD_FILEHED));
	}

    /* CHECK RANGE OF MLD NO  */
    if ( (pMnemonicFile->usMnemonicAddress == 0) || (pMnemonicFile->usMnemonicAddress > MldFileHed.usNumberOfAddress ) ) {
        Op_CloseMldFileReleaseSem();           /* close file and release semaphore */
        return (OP_ABNORMAL_MLD);
    }

    Op_WriteMldFile( sizeof(MldFileHed) + (ULONG)((pMnemonicFile->usMnemonicAddress - 1) * (usRecordSize + 1) * sizeof(WCHAR)), pMnemonicFile->aszMnemonicValue, usRecordSize * sizeof(WCHAR));
    Op_CloseMldFileReleaseSem();           /* close file and release semaphore */
    return (OP_PERFORM);
}

SHORT   OpMnemonicsFileIndRead(OPMNEMONICFILE *pMnemonicFile, USHORT usLockHnd)
{
    OPMLD_FILEHED MldFileHed = { 0 };
	TCHAR           *auchFileName;
	USHORT          usRecordSize, usMaxRecordCount;

    memset(&pMnemonicFile->aszMnemonicValue, 0, sizeof(pMnemonicFile->aszMnemonicValue));

	if (0 == pMnemonicFile->usMnemonicFileId || pMnemonicFile->usMnemonicFileId > sizeof(OpMnemonicsFileTable)/sizeof(OpMnemonicsFileTable[0])) {
		return 0;
	}

	auchFileName = OpMnemonicsFileTable[pMnemonicFile->usMnemonicFileId].auchFileName;
	usRecordSize = OpMnemonicsFileTable[pMnemonicFile->usMnemonicFileId].usRecordSize;
	usMaxRecordCount = OpMnemonicsFileTable[pMnemonicFile->usMnemonicFileId].usMaxRecordCount;

    PifRequestSem(husOpSem);

    /*  LOCK CHECK */
    if (usLockHnd) {
        if (Op_LockCheck(usLockHnd) == OP_LOCK) {
            PifReleaseSem(husOpSem);
            return (OP_LOCK);
        }
    }

    if (Op_MnemonicsFileOpenFile(pMnemonicFile->usMnemonicFileId) == PIF_ERROR_FILE_NOT_FOUND) {
        PifReleaseSem(husOpSem);
        return (OP_FILE_NOT_FOUND);
    }

    Op_ReadMldFile(OP_FILE_HEAD_POSITION, &MldFileHed, sizeof(MldFileHed));

    if ( (pMnemonicFile->usMnemonicAddress == 0) || (pMnemonicFile->usMnemonicAddress > MldFileHed.usNumberOfAddress ) ) {
        Op_CloseMldFileReleaseSem();
        return (OP_PERFORM);
    }

    Op_ReadMldFile((ULONG)(sizeof(MldFileHed) + (pMnemonicFile->usMnemonicAddress - 1) * (usRecordSize + 1) * sizeof(WCHAR)),
                    &pMnemonicFile->aszMnemonicValue, usRecordSize * sizeof(WCHAR));

    Op_CloseMldFileReleaseSem();
    return (OP_PERFORM);
}

//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------

/*
*==========================================================================
**    Synopsis:    SHORT   OpPluAssign(PLUIF *pPif, USHORT usLockHnd)
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
SHORT OpPluAssignRecPlu (PRECPLU pRecPlu, USHORT  usAdjNo, USHORT usLockHnd)
{
    MFINFO mfinfo = { 0 };
	RECPLU recpluold = { 0 };
    USHORT usResult;
    USHORT usRecPluLen;
    USHORT  i;

    usAdjNo--; /* decrement adjective code */

    /*  CHECK LOCK */
    PifRequestSem(husOpSem);

    if (Op_LockCheck(usLockHnd) == OP_LOCK)
    {
        PifReleaseSem(husOpSem);
        return OP_LOCK;
    }

    usResult = PluSenseFile(FILE_PLU, &mfinfo);
    if(usResult == SPLU_FILE_NOT_FOUND)
    {
        PifReleaseSem(husOpSem);
        return (SHORT)OpConvertPluError(usResult);
    }

    usResult = PluReplaceRecord(FILE_PLU, pRecPlu, &recpluold, &usRecPluLen);
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
                if (i != usAdjNo) {
                    pRecPlu->ulUnitPrice[i] = recpluold.ulUnitPrice[i];
                }
            }
            usResult = PluReplaceRecord(FILE_PLU, pRecPlu, &recpluold, &usRecPluLen);
            PifReleaseSem(husOpSem);
            return OP_CHANGE_PLU;
        }
    }

    PifReleaseSem(husOpSem);

    return (SHORT)OpConvertPluError(usResult);
}

SHORT OpPluAssign(PLUIF *pPif, USHORT usLockHnd)
{
    MFINFO mfinfo = { 0 };
    RECPLU recplu = { 0 };
    RECPLU recpluold = { 0 };
    PRECPLU pRecPlu = &recplu;
    PRECPLU pRecPluOld = &recpluold;
    USHORT usResult;
    USHORT usRecPluLen;
    USHORT  usAdj;

    usAdj = pPif->uchPluAdj;
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
    pRecPlu->usFamilyCode             = pPif->ParaPlu.usFamilyCode; /* ### ADD Saratoga (V1_0.02)(051800) */
	pRecPlu->usBonusIndex			  = pPif->ParaPlu.ContPlu.usBonusIndex;

	// PLU Enhancements - CSMALL
	pRecPlu->uchColorPaletteCode	  = pPif->ParaPlu.uchColorPaletteCode; // PLU Color Enhancement - CSMALL
	pRecPlu->usTareInformation        = pPif->ParaPlu.usTareInformation;
	memcpy (pRecPlu->uchExtendedGroupPriority, pPif->ParaPlu.uchExtendedGroupPriority, sizeof(pRecPlu->uchExtendedGroupPriority));
	memcpy(pRecPlu->auchAlphaNumRouting, pPif->ParaPlu.auchAlphaNumRouting, sizeof(pRecPlu->auchAlphaNumRouting));
	memcpy(pRecPlu->auchAltPluName, pPif->ParaPlu.auchAltPluName, sizeof(pRecPlu->auchAltPluName));
	memcpy(pRecPlu->futureTotals, pPif->ParaPlu.futureTotals, sizeof(pRecPlu->futureTotals));

    usResult = PluReplaceRecord(FILE_PLU, pRecPlu, pRecPluOld, &usRecPluLen);
    if(!usResult)
    {
        if(!usRecPluLen)
        {
            PifReleaseSem(husOpSem);
            return OP_NEW_PLU;
        }
        else
        {
			USHORT  i;
			
			/* preserve other adjective price */
            for (i=0; i< MAX_ADJECTIVE_NO; i++) {
                if (i != usAdj) {
                    pRecPlu->ulUnitPrice[i] = pRecPluOld->ulUnitPrice[i];
                }
            }
            usResult = PluReplaceRecord(FILE_PLU, pRecPlu, pRecPluOld, &usRecPluLen);
            PifReleaseSem(husOpSem);
            return OP_CHANGE_PLU;
        }
    }

    PifReleaseSem(husOpSem);

    return OpConvertPluError(usResult);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpPluDelete(PLUIF *pPif, USHORT usLockHnd)
*
*       Input:   pPif->auchPluNo
*                pPif->uchPluAdj
*                USHORT usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*    Normal End:   OP_PERFORM
*   Abnormal End:  OP_PLU_ERROR
*                  OP_FILE_NOT_FOUND
*                  OP_LOCKE
*                  OP_NOT_IN_FILE
*
**  Description: Delete a Plu in file.
*==========================================================================
*/
SHORT OpPluDelete(PLUIF *pPif, USHORT usLockHnd)
{
    MFINFO mfinfo = { 0 };
    USHORT usResult;
    WCHAR  aucOldRec[sizeof(RECPLU)];
    USHORT usOldRecLen;

    PifRequestSem(husOpSem);

    if (pPif->uchPluAdj > 1) {

        /* delete record only adjective code is 1 */
        PifReleaseSem(husOpSem);
        return (OP_PERFORM);
    }

    /*  LOCK CHECK */
    if (Op_LockCheck(usLockHnd) == OP_LOCK)
    {
        PifReleaseSem(husOpSem);
        return OP_LOCK;
    }

    usResult = PluSenseFile(FILE_PLU,&mfinfo);
    if(usResult == SPLU_FILE_NOT_FOUND)
    {
        PifReleaseSem(husOpSem);
        return OpConvertPluError(usResult);
    }
    else
    {
        usResult = PluDeleteRecord(FILE_PLU, pPif->auchPluNo, pPif->uchPluAdj, 0, aucOldRec, &usOldRecLen);
    }

    PifReleaseSem(husOpSem);
    return OpConvertPluError(usResult);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpPluFileUpdate(USHORT usLockHnd)
*
*       Input:    usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End:  OP_PERFORM
                   OP_EOF
*   Abnormal End:  OP_PLU_ERROR
*                  OP_LOCK
*                  OP_FILE_NOT_FOUND
*
*
**  Description:  Update Plu File (Make sub Index table)
*==========================================================================
*/
static SHORT OpPluFileUpdateOep (SHORT hsFileHandle, UCHAR uchItemType, UCHAR uchGroupNumber, UCHAR uchPriority, WCHAR *aucPluNumber)
{
    ULONG          offulSearchPoint = 0;
    OPOEP_FILEHED  OepFileHed = { 0 };
    USHORT         usResult = 1;  // not SPLU_COMPLETED as that is actually an error condition meaning file is full.
	USHORT         usLoop;
	USHORT         *pusNumberOfResidentOep;
	WCHAR  auchOepPlu [OP_PLU_OEP_LEN];

	auchOepPlu[2] = _T('0');
	auchOepPlu[1] = _T('0');
	auchOepPlu[0] = _T('0');
	memcpy (auchOepPlu+3, aucPluNumber, PLU_NUMBER_LEN * sizeof(aucPluNumber[0]));
	pusNumberOfResidentOep = &OepFileHed.usNumberOfResidentOep;

	hsOpOepFileHandle = hsFileHandle;  // override the current setting for the global file handle to insert this PLU into this OEP table file.

	for (usLoop = 0; usLoop < 2; usLoop++) {
		Op_ReadOepFile(OP_FILE_HEAD_POSITION, &OepFileHed, sizeof(OepFileHed));
		/* OEP File limited to 8000 */
		if (*pusNumberOfResidentOep < OepFileHed.usNumberOfMaxOep) {
			if(Op_OepIndex(&OepFileHed, usLoop, OP_REGIST, uchGroupNumber, auchOepPlu, 1, &offulSearchPoint) == OP_NOT_IN_FILE)
			{
				usResult = SPLU_REC_NOT_FOUND;
				break;
			}
		} else {
			usResult = SPLU_COMPLETED;
			break;
		}

		if ((uchItemType & PLU_TYPE_MASK_REG) == PLU_ADJ_TYP) {
			USHORT     i;

			for (i = 2; i <= 5; i++) {
				Op_ReadOepFile(OP_FILE_HEAD_POSITION, &OepFileHed, sizeof(OepFileHed));
				/* OEP File limited to 8000 */
				if (*pusNumberOfResidentOep < OepFileHed.usNumberOfMaxOep) {
					if(Op_OepIndex(&OepFileHed, usLoop, OP_REGIST, uchGroupNumber, auchOepPlu, i, &offulSearchPoint) == OP_NOT_IN_FILE)
					{
						usResult = SPLU_REC_NOT_FOUND;
						break;
					}
				} else {
					usResult = SPLU_COMPLETED;
					break;
				}
			}
		}

		auchOepPlu[2] = (uchPriority % 10) + _T('0');
		uchPriority /= 10;
		auchOepPlu[1] = (uchPriority % 10) + _T('0');
		uchPriority /= 10;
		auchOepPlu[0] = (uchPriority % 10) + _T('0');
		pusNumberOfResidentOep = &OepFileHed.usNumberOfResidentOep2;
	}

	return usResult;
}

SHORT OpPluFileUpdate(USHORT usLockHnd)
{
    USHORT usResult;
    USHORT husHandle;
	USHORT usFilePart = 0;
	USHORT usOepTableNo = 0;
    MFINFO mfinfo = { 0 };
    ECMRANGE ecmrange = { 0 };
    PECMRANGE pEcmRange = &ecmrange;
    RECPLU recplu = { 0 };
    PRECPLU pRecPlu = &recplu;
    USHORT usRecLen;
    ULONG  offulSearchPoint = 0;
	SHORT    husCurrentOepTableFile[4];
    PARAFLEXMEM        WorkFlex = { 0 };

    PifRequestSem(husOpSem);

    /*  LOCK CHECK */
    if (Op_LockCheck(usLockHnd) == OP_LOCK)
    {
        PifReleaseSem(husOpSem);
        return OP_LOCK;
    }

    usResult = PluSenseFile(FILE_PLU,&mfinfo);
    if(usResult == SPLU_FILE_NOT_FOUND)
    {
        PifReleaseSem(husOpSem);
        return OpConvertPluError(usResult);
    }

    {
		int i;
		for(i = 0; i < STD_PLU_NUMBER_LEN; i++){
			pEcmRange->itemFrom.aucPluNumber[i] = WIDE('0');
			pEcmRange->itemTo.aucPluNumber[i] = WIDE('9');
		}
	}
	pEcmRange->fsOption = REPORT_ACTIVE | REPORT_INACTIVE;
    pEcmRange->usReserve = 0;

    usResult = PluEnterCritMode(FILE_PLU, FUNC_REPORT_RANGE, &husHandle, pEcmRange);

    if(usResult)
    {
        PifReleaseSem(husOpSem);
        return OpConvertPluError(usResult);
    }

    /* recreate oep file at the plu file update */
    WorkFlex.uchMajorClass = CLASS_PARAFLEXMEM;
    WorkFlex.uchAddress = FLEX_PLU_ADR;
    ParaFlexMemRead(&WorkFlex);

	for (usOepTableNo = 0; usOepTableNo < 2; usOepTableNo++) {
		if (usResult = OpOepCreatFilev221(usOepTableNo, WorkFlex.ulRecordNumber, 0) != OP_PERFORM) {
			PluExitCritMode(husHandle,0);
			PifReleaseSem(husOpSem);
			PifLog(MODULE_OP_OEP, 1);
			return (usResult);
		}

		if ((husCurrentOepTableFile[usOepTableNo] = Op_OepOpenFile(usOepTableNo)) < 0) /* pif_error_file_exist */
		{
			PluExitCritMode(husHandle,0);
			PifReleaseSem(husOpSem);
			PifLog(MODULE_OP_OEP, 2);
			return OP_FILE_NOT_FOUND;
		}
	}

    for (;;) {
    /* while(1) { */
        usResult = PluReportRecord(husHandle, pRecPlu, &usRecLen);
        if(usResult == SPLU_REC_LOCKED || usResult == SPLU_FUNC_CONTINUED || usResult == SPLU_COMPLETED) {
			usResult = 1;
            if (pRecPlu->uchGroupNumber) { /* */
				usResult = OpPluFileUpdateOep (husCurrentOepTableFile[0], pRecPlu->uchItemType, pRecPlu->uchGroupNumber, pRecPlu->uchPrintPriority, pRecPlu->aucPluNumber);
			}
			if (pRecPlu->uchReportCode) {
				usResult = OpPluFileUpdateOep (husCurrentOepTableFile[1], pRecPlu->uchItemType, pRecPlu->uchReportCode, pRecPlu->uchPrintPriority, pRecPlu->aucPluNumber);
			}
			if (usResult != 1)
				break;
        }
        else if(usResult == SPLU_END_OF_FILE)
        {
            usResult = SPLU_COMPLETED;
            break;
        }
        else {
            break;
		}
    }
	PifCloseFile(husCurrentOepTableFile[0]);
	PifCloseFile(husCurrentOepTableFile[1]);

    PluExitCritMode(husHandle,0);
    PifReleaseSem(husOpSem);

    return OpConvertPluError(usResult);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpOepCreatFile(ULONG ulNumberOfPlu, USHORT usLockHnd)
*
*
*       Input:    usNumberOfPlu - Number of Plu
*                 usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End:  OP_PERFORM
*   Abnormal End:  OP_NO_MAKE_FILE
*
**  Description:  Create Oep File. 2172
*==========================================================================
*/
SHORT OpOepCreatFile(ULONG ulNumberOfPlu, USHORT usLockHnd)
{
    OPOEP_FILEHED OepFileHed = { 0 };
    ULONG         ulActualPosition;

    Op_LockCheck(usLockHnd);  /* Dummy */

    /* Creat Operational file */

    PifDeleteFile((WCHAR FAR *)auchOP_OEP);  /* Saratoga */

    /* Check Number of Dept */

    if (!ulNumberOfPlu) {           /* usNumberOfPlu == 0 */
        return(SHORT)OP_PERFORM;
    }

    if ((hsOpOepFileHandle = PifOpenFile(auchOP_OEP, auchNEW_FILE_WRITE)) < 0x0000) {  /* Don't occured error */
            PifAbort(MODULE_OP_OEP, FAULT_ERROR_FILE_OPEN);
    }

    /* Make File Header */

    if (ulNumberOfPlu > OP_NUMBER_OF_MAX_OEP) {
        OepFileHed.usNumberOfMaxOep = OP_NUMBER_OF_MAX_OEP;
    } else {
        OepFileHed.usNumberOfMaxOep = (SHORT)ulNumberOfPlu;
    }
    OepFileHed.usNumberOfResidentOep = 0x00;
    OepFileHed.offulOfIndexTbl = sizeof(OepFileHed);

    OepFileHed.ulOepFileSize = OepFileHed.offulOfIndexTbl + ((ULONG)OepFileHed.usNumberOfMaxOep * (ULONG)sizeof(OPOEP_INDEXENTRY));

    /* Check file size and creat file */

    if (PifSeekFile(hsOpOepFileHandle, OepFileHed.ulOepFileSize, &ulActualPosition) < 0) { /* PIF_ERROR_DISK_FULL */
        PifCloseFile(hsOpOepFileHandle);
        PifDeleteFile((WCHAR FAR *)auchOP_OEP);      /* Can't make file then delete file,    Saratoga */
        return(OP_NO_MAKE_FILE);
    }

    /* Write File Header */

    Op_WriteOepFile(OP_FILE_HEAD_POSITION, &OepFileHed, sizeof(OepFileHed));

    PifCloseFile(hsOpOepFileHandle);
    return(OP_PERFORM);
}

// The following function creates the new version of the OEP file.
SHORT OpOepCreatFilev221(USHORT  usOepTableNo, ULONG ulNumberOfPlu, USHORT usLockHnd)
{
	WCHAR         *auchOP_OEP_CurrentFile = 0;
    OPOEP_FILEHED OepFileHed = { 0 };
    ULONG         ulActualPosition;

    Op_LockCheck(usLockHnd);  /* Dummy */

	if (usOepTableNo < sizeof(auchOP_OEP_TABLE)/sizeof(auchOP_OEP_TABLE[0])) {
		auchOP_OEP_CurrentFile = auchOP_OEP_TABLE[usOepTableNo];
	} else {
		return(OP_NO_MAKE_FILE);
	}

    /* Creat Operational file */
    PifDeleteFile(auchOP_OEP_CurrentFile);

    /* Check Number of Dept */
    if (!ulNumberOfPlu) {           /* usNumberOfPlu == 0 */
        return(SHORT)OP_PERFORM;
    }

    if ((hsOpOepFileHandle = PifOpenFile(auchOP_OEP_CurrentFile, auchNEW_FILE_WRITE)) < 0x0000) {  /* Don't occured error */
        PifAbort(MODULE_OP_OEP, FAULT_ERROR_FILE_OPEN);
    }

    /* Make File Header */
    if (ulNumberOfPlu > OP_NUMBER_OF_MAX_OEP) {
        OepFileHed.usNumberOfMaxOep = OP_NUMBER_OF_MAX_OEP;
    } else {
        OepFileHed.usNumberOfMaxOep = (SHORT)ulNumberOfPlu;
    }
    OepFileHed.usNumberOfResidentOep = 0x00;
    OepFileHed.usNumberOfResidentOep2 = 0x00;
    OepFileHed.offulOfIndexTbl = sizeof(OepFileHed);
    OepFileHed.offulOfIndexTbl2 = OepFileHed.offulOfIndexTbl + (OepFileHed.usNumberOfMaxOep * sizeof(OPOEP_INDEXENTRY));

    OepFileHed.ulOepFileSize = OepFileHed.offulOfIndexTbl2 + (OepFileHed.usNumberOfMaxOep * sizeof(OPOEP_INDEXENTRY));

    /* Check file size and creat file */
    if (PifSeekFile(hsOpOepFileHandle, OepFileHed.ulOepFileSize, &ulActualPosition) < 0) { /* PIF_ERROR_DISK_FULL */
        PifCloseFile(hsOpOepFileHandle);
        PifDeleteFile(auchOP_OEP_CurrentFile);      /* Can't make file then delete file,    Saratoga */
        return(OP_NO_MAKE_FILE);
    }

    /* Write File Header */
    Op_WriteOepFile(OP_FILE_HEAD_POSITION, &OepFileHed, sizeof(OepFileHed));

    PifCloseFile(hsOpOepFileHandle);
    return(OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpPluIndRead(PLUIF *Pif, USHORT usLockHnd)
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
SHORT OpPluIndRead(PLUIF *pPif, USHORT usLockHnd)
{
	MFINFO mfinfo = { 0 };
    USHORT usResult;
    USHORT usRecPluLen;
    RECPLU recplu = { 0 };
    PRECPLU pRecPlu = &recplu;
    ITEMNO itemno = { 0 };
    PITEMNO pItemNumber = &itemno;

    PifRequestSem(husOpSem);

    /*  LOCK CHECK */
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

    memcpy(pItemNumber->aucPluNumber,pPif->auchPluNo,sizeof(pItemNumber->aucPluNumber));
    usResult = PluReadRecord(FILE_PLU,
                             pItemNumber,
                             1, /* adjective is always 1 */
                             /* (USHORT)pPif->uchPluAdj, */
                             (VOID*)pRecPlu,
                             &usRecPluLen);
    if(!usResult)
    {
        memcpy(pPif->auchPluNo,pRecPlu->aucPluNumber,sizeof(pPif->auchPluNo));
        /* pPif->uchPluAdj                   = pRecPlu->uchAdjective; */
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
		pPif->ParaPlu.ContPlu.usBonusIndex	= pRecPlu->usBonusIndex;

		// PLU Enhancements - CSMALL
		pPif->ParaPlu.uchColorPaletteCode = pRecPlu->uchColorPaletteCode;
		pPif->ParaPlu.usTareInformation = pRecPlu->usTareInformation;
		memcpy (pPif->ParaPlu.uchExtendedGroupPriority, pRecPlu->uchExtendedGroupPriority, sizeof(pPif->ParaPlu.uchExtendedGroupPriority));
		memcpy (pPif->ParaPlu.auchAlphaNumRouting, pRecPlu->auchAlphaNumRouting, sizeof(pPif->ParaPlu.auchAlphaNumRouting));
		memcpy (pPif->ParaPlu.auchAltPluName, pRecPlu->auchAltPluName, sizeof(pPif->ParaPlu.auchAltPluName));
		memcpy (pPif->ParaPlu.futureTotals, pRecPlu->futureTotals, sizeof(pPif->ParaPlu.futureTotals));
    }

    PifReleaseSem(husOpSem);

    return OpConvertPluError(usResult);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpPluAllRead(PLUIF *pPif, USHORT usLockHnd)
*
*       Input:  pPif -> culCounter
*               usLockHnd
*      Output:  pPif -> auchPluNo
*               pPif -> uchPluAdj
*               pPif -> ParaPlu
*       InOut:  Nothing
*
**  Return    :
*     Normal End: OP_PERFORM
*                 OP_EOF
*   Abnormal End: OP_PLU_ERROR
*                 OP_LOCKE
*                 OP_FILE_NOT_FOUND
*
*
**  Description: Read All Plu .
*==========================================================================
*/
SHORT OpPluAllRead(PLUIF *pPif, USHORT usLockHnd)
{
    USHORT usResult;
	MFINFO mfinfo = { 0 };
    USHORT husHandle;
    USHORT usRecLen;
	RECPLU recplu = { 0 };
    PRECPLU pRecPlu = &recplu;
	ECMRANGE ecmrange = { 0 };
    PECMRANGE pEcmRange = &ecmrange;
	ITEMNO itemno = { 0 };
    PITEMNO pItemNumber = &itemno;
    static BOOL bCnt = FALSE;
    static BOOL bFirstAccess = FALSE;

    PifRequestSem(husOpSem);

    /*  LOCK CHECK */
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

    /* first access */

    if((pPif->ulCounter) && (bCnt)) {
      bFirstAccess = TRUE;
    }

    if((!pPif->ulCounter) || (bFirstAccess))
    {
      if(!pPif->ulCounter)
        bCnt = TRUE;

	  {
		  USHORT  i;
		  for(i = 0; i < STD_PLU_NUMBER_LEN; i++){
			  pEcmRange->itemFrom.aucPluNumber[i] = WIDE('0');
			  pEcmRange->itemTo.aucPluNumber[i] = WIDE('9');
		  }
	  }
        pEcmRange->fsOption = REPORT_ACTIVE | REPORT_INACTIVE;
        pEcmRange->usReserve = 0;

        usResult = PluEnterCritMode(FILE_PLU, FUNC_REPORT_RANGE, &husHandle, pEcmRange);
        if(usResult)
        {
            PifReleaseSem(husOpSem);
            return(SHORT)OpConvertPluError(usResult);
        }

        pPif->husHandle = husHandle;
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
        pPif->usAdjCo = 0;
#endif
    if (!pPif->ulCounter)
           pPif->usAdjCo = 0;

    }

    /* read adjective price, before PluReportRecord() */

    if (pPif->usAdjCo)
    {
        memcpy(pItemNumber->aucPluNumber,pPif->auchPluNo,sizeof(pItemNumber->aucPluNumber));
        pPif->usAdjCo++;
        usResult = PluReadRecord(FILE_PLU, pItemNumber, 1, pRecPlu, &usRecLen);
        if(usResult)
        {
            PluExitCritMode(pPif->husHandle,0);
            PifReleaseSem(husOpSem);
            return(SHORT)OpConvertPluError(usResult);
        }
        else
        {
            memcpy(pPif->auchPluNo,pRecPlu->aucPluNumber,sizeof(pPif->auchPluNo));
            pPif->uchPluAdj                   = (UCHAR)pPif->usAdjCo;
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
            pPif->ParaPlu.ContPlu.usBonusIndex = pRecPlu->usBonusIndex;

			// PLU Enhancements - CSMALL
			pPif->ParaPlu.uchColorPaletteCode = pRecPlu->uchColorPaletteCode;
			pPif->ParaPlu.usTareInformation = pRecPlu->usTareInformation;
			memcpy (pPif->ParaPlu.uchExtendedGroupPriority, pRecPlu->uchExtendedGroupPriority, sizeof(pPif->ParaPlu.uchExtendedGroupPriority));
			memcpy(pPif->ParaPlu.auchAlphaNumRouting, pRecPlu->auchAlphaNumRouting, sizeof(pPif->ParaPlu.auchAlphaNumRouting));
			memcpy(pPif->ParaPlu.auchAltPluName, pRecPlu->auchAltPluName, sizeof(pPif->ParaPlu.auchAltPluName));
			memcpy(pPif->ParaPlu.futureTotals, pRecPlu->futureTotals, sizeof(pPif->ParaPlu.futureTotals));
			memcpy(pPif->ParaPlu.auchStatus2, pRecPlu->auchStatus2, sizeof(pPif->ParaPlu.auchStatus2));
        }

        if (pPif->usAdjCo >= MAX_ADJECTIVE_NO)
        {
            pPif->usAdjCo = 0;
        }
   } else {
      /* read each record sequentially */

       if(bFirstAccess)
       {
		   ULONG ulCount = 0;

		   for(ulCount = 0; ulCount < pPif->ulCounter; ulCount++)
			{
				usResult = PluReportRecord(pPif->husHandle, pRecPlu, &usRecLen);
			}

			bFirstAccess = FALSE;
			bCnt = FALSE;
		}

     usResult = PluReportRecord(pPif->husHandle, pRecPlu, &usRecLen);

    if((usResult == SPLU_REC_LOCKED) || (usResult == SPLU_FUNC_CONTINUED) || (usResult == SPLU_COMPLETED))
    {
      pPif->ulCounter++;

        memcpy(pPif->auchPluNo,pRecPlu->aucPluNumber,sizeof(pPif->auchPluNo));
      pPif->uchPluAdj                   = 1; /* adjectve is always 1 on PLU record */
      /* pPif->uchPluAdj                   = pRecPlu->uchAdjective; */
      pPif->ParaPlu.ContPlu.usDept      = pRecPlu->usDeptNumber;
      RflConv4bto3b(pPif->ParaPlu.auchPrice,pRecPlu->ulUnitPrice[0]);
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
      pPif->ParaPlu.ContPlu.usBonusIndex = pRecPlu->usBonusIndex;

	  // PLU Enhancements - CSMALL
	  pPif->ParaPlu.uchColorPaletteCode = pRecPlu->uchColorPaletteCode;
	  pPif->ParaPlu.usTareInformation = pRecPlu->usTareInformation;
	  memcpy (pPif->ParaPlu.uchExtendedGroupPriority, pRecPlu->uchExtendedGroupPriority, sizeof(pPif->ParaPlu.uchExtendedGroupPriority));
	  memcpy(pPif->ParaPlu.auchAlphaNumRouting, pRecPlu->auchAlphaNumRouting, sizeof(pPif->ParaPlu.auchAlphaNumRouting));
	  memcpy(pPif->ParaPlu.auchAltPluName, pRecPlu->auchAltPluName, sizeof(pPif->ParaPlu.auchAltPluName));
	  memcpy(pPif->ParaPlu.futureTotals, pRecPlu->futureTotals, sizeof(pPif->ParaPlu.futureTotals));
	  memcpy(pPif->ParaPlu.auchStatus2, pRecPlu->auchStatus2, sizeof(pPif->ParaPlu.auchStatus2));

      if ((pRecPlu->uchItemType & PLU_TYPE_MASK_REG) == PLU_ADJ_TYP)
      {
        pPif->usAdjCo++;
      }
      else
      {
        pPif->usAdjCo = 0;
      }
      usResult = SPLU_COMPLETED;
    }
  }

    if((usResult == SPLU_END_OF_FILE) || (usResult) || (bCnt))
    {
        PluExitCritMode(pPif->husHandle,0);
        /* PluExitCritMode(husHandle,0); */
    }

    PifReleaseSem(husOpSem);

    return OpConvertPluError(usResult);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpDeptAssign(DEPTIF *pDpi, USHORT usLockHnd)
*
*       Input:  pDpi->usDeptNo
*               pDpi->ParaDept
*               usLockHnd
*
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :
*    Normal End:   OP_NEW_DEPT
*   Abnormal End:  OP_CHANGE_DEPT
*                  OP_FILE_NOT_FOUND
*                  OP_DEPT_FULL
*                  OP_LOCK
*
**  Description:  Add and change dept record.    2172
*==========================================================================
*/
SHORT OpDeptAssign(DEPTIF *pDpi, USHORT usLockHnd)
{
	OPDEPT_FILEHED DeptFileHed = { 0 };
    SHORT         sStatus;
    USHORT        usParaBlockNo;

    /*  CHECK LOCK */

    PifRequestSem(husOpSem);

    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return(OP_LOCK);
    }

    if ((hsOpDeptFileHandle = PifOpenFile(auchOP_DEPT, auchOLD_FILE_READ_WRITE)) == PIF_ERROR_FILE_NOT_FOUND) {
        PifReleaseSem(husOpSem);
        return(OP_FILE_NOT_FOUND);
    }

    /* CHECK ASSIGN */

    Op_ReadDeptFile(OP_FILE_HEAD_POSITION, &DeptFileHed, sizeof(DeptFileHed));  /* get dept file header */
    if ((sStatus = Op_DeptIndex(&DeptFileHed, OP_REGIST, pDpi->usDeptNo, &usParaBlockNo)) == OP_DEPT_FULL) {
        Op_CloseDeptFileReleaseSem();
        return(OP_DEPT_FULL);
    }

    /*  MAKE PARAMATER */

    Op_DeptParaWrite(&DeptFileHed, usParaBlockNo, &pDpi->ParaDept);
    Op_CloseDeptFileReleaseSem();
    return(sStatus);               /* usStatus = OP_NEW_DEPT or OP_CHANGE_DEPT */
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpDeptDelete(DEPTIF *pDpi, USHORT usLockHnd)
*
*       Input:   pDpi->usDeptNo
*                USHORT usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*    Normal End:   OP_PERFORM
*   Abnormal End:  OP_NOT_IN_FILE
*                  OP_FILE_NOT_FOUND
*                  OP_LOCK
*
**  Description: Delete a dept in file. 2172
*==========================================================================
*/
SHORT OpDeptDelete(DEPTIF *pDpi, USHORT usLockHnd)
{
    OPDEPT_FILEHED DeptFileHed = { 0 };
    SHORT         sStatus;

    PifRequestSem(husOpSem);

    /*  LOCK CHECK */

    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return(OP_LOCK);
    }

    /*  OPEN DEPT FILE */

    if ((hsOpDeptFileHandle = PifOpenFile(auchOP_DEPT, auchOLD_FILE_READ_WRITE)) == PIF_ERROR_FILE_NOT_FOUND) {
        PifReleaseSem(husOpSem);
        return(OP_FILE_NOT_FOUND);
    }
    Op_ReadDeptFile(OP_FILE_HEAD_POSITION, &DeptFileHed, sizeof(DeptFileHed));

    /* CHECK ASSIGN DEPT AND DELETED */

    sStatus = Op_DeptIndexDel(&DeptFileHed, pDpi->usDeptNo);

    Op_CloseDeptFileReleaseSem();
    return(sStatus);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpDeptIndRead(DEPTIF *Dpi, USHORT usLockHnd)
*
*       Input:    *Dpi->usDeptNo
*                 usLockHnd
*      Output:    DEPTIF  *Dpi->ParaDept
*       InOut:    Nothing
*
**  Return    :
*     Normal End:  OP_PERFORM
*   Abnormal End:  OP_NOT_IN_FILE
*                  OP_FILE_NOT_FOUND
*                  OP_LOCK
*
**  Description: Read a Dept paramater Individually. 2172
*==========================================================================
*/
SHORT OpDeptIndRead(DEPTIF *pDpi, USHORT usLockHnd)
{
    OPDEPT_FILEHED DeptFileHed = { 0 };
	SHORT         sStatus;
    USHORT        usParaBlockNo;

    PifRequestSem(husOpSem);

    /*  LOCK CHECK */

    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return(OP_LOCK);
    }

    if ((hsOpDeptFileHandle = PifOpenFile(auchOP_DEPT, auchOLD_FILE_READ_WRITE)) == PIF_ERROR_FILE_NOT_FOUND) {
        PifReleaseSem(husOpSem);
        return(OP_FILE_NOT_FOUND);
    }

    Op_ReadDeptFile(OP_FILE_HEAD_POSITION, &DeptFileHed, sizeof(DeptFileHed));

	sStatus = Op_DeptIndex(&DeptFileHed, OP_SEARCH_ONLY, pDpi->usDeptNo, &usParaBlockNo);
	if (sStatus == OP_NOT_IN_FILE || sStatus == OP_DEPT_FULL) {
		Op_CloseDeptFileReleaseSem();
        return(OP_NOT_IN_FILE);
    }

    Op_DeptParaRead(&DeptFileHed, usParaBlockNo, &pDpi->ParaDept);
    Op_CloseDeptFileReleaseSem();
    return(OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpDeptAllRead(DEPTIF *pPif,
*                                                 USHORT usLockHnd)
*
*       Input: pPif -> culCounter
*              usLockHnd
*      Output: pPif -> usDeptNo
*              pPif -> ParaDept
*       InOut: Nothing
*
**  Return   :
*     Normal End: OP_PERFORM
*                 OP_EOF
*   Abnormal End: OP_FILE_NOT_FOUND
*                 OP_LOCK
*
**  Description: Read All Dept .
*==========================================================================
*/
SHORT   OpDeptAllRead(DEPTIF *pPif, USHORT usLockHnd)
{
    OPDEPT_FILEHED DeptFileHed = { 0 };
    SHORT   sStatus;

    PifRequestSem(husOpSem);

    /*  LOCK CHECK */

    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return(OP_LOCK);
    }

    if ((hsOpDeptFileHandle = PifOpenFile(auchOP_DEPT, auchOLD_FILE_READ)) == PIF_ERROR_FILE_NOT_FOUND) {
        PifReleaseSem(husOpSem);
        return(OP_FILE_NOT_FOUND);
    }

    Op_ReadDeptFile(OP_FILE_HEAD_POSITION, &DeptFileHed, sizeof(DeptFileHed));

    sStatus = Op_GetDept(&DeptFileHed, pPif->ulCounter++, pPif);
    Op_CloseDeptFileReleaseSem();
    return(sStatus);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpCpnAssign(CPNIF *pCpni,
*                                               USHORT usLockHnd)
*
*       Input: pCpni->usCpnNo
*              pCpni->ParaCpn
*              USHORT usLockHnd
*      Output: Nothing
*       InOut: Nothing
*
**  Return   :
*     Normal End: OP_PERFORM
*   Abnormal End: OP_FILE_NOT_FOUND
*                 OP_ABNORMAL_CPN - HALT
*                 OP_LOCK
*
**  Description: Add and Chnge Combination Coupon.
*==========================================================================
*/
SHORT   OpCpnAssign(CPNIF *pCpni, USHORT usLockHnd)
{
    OPCPN_FILEHED CpnFileHed = { 0 };

    /*  CHECK LOCK */

    PifRequestSem(husOpSem);

    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return(OP_LOCK);
    }

    /*  OPEN FILE  */

    if ((hsOpCpnFileHandle = PifOpenFile(auchOP_CPN, auchOLD_FILE_READ_WRITE)) == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        PifReleaseSem(husOpSem);
        return(OP_FILE_NOT_FOUND);
    }

    Op_ReadCpnFile(OP_FILE_HEAD_POSITION, &CpnFileHed, sizeof(OPCPN_FILEHED));

    /* CHECK RANGE OF Combination Coupon NO  */

    if (pCpni->uchCpnNo == 0) {
        Op_CloseCpnFileReleaseSem();           /* close file and release semaphore */
        return(OP_ABNORMAL_CPN);
    }

    if ( pCpni->uchCpnNo > CpnFileHed.usNumberOfCpn ) {
        Op_CloseCpnFileReleaseSem();           /* close file and release semaphore */
        return(OP_ABNORMAL_CPN);
    }

    Op_WriteCpnFile( sizeof(CpnFileHed) + (ULONG)((pCpni->uchCpnNo - 1) * sizeof(COMCOUPONPARA)),
                      &pCpni->ParaCpn,
                      sizeof(pCpni->ParaCpn));
    Op_CloseCpnFileReleaseSem();           /* close file and release semaphore */
    return(OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpCstrAssign(CSTRIF *pCstr, USHORT usLockHnd)
*
*       Input:  pCstr->usCstrNo
*               pCstr->IndexCstr
*               pCstr->ParaCstr
*               usLockHnd
*
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :
*    Normal End:   OP_PERFORM
*   Abnormal End:  OP_FILE_NOT_FOUND
*                  OP_ABNORMAL_CSTR - HALT
*                  OP_LOCK
*
**  Description:  Add and Chnge Control String.
*==========================================================================
*/
SHORT  OpCstrAssign(CSTRIF *pCstr, USHORT usLockHnd)
{
	OPCSTR_FILEHED CstrFileHed = { 0 };
    OPCSTR_INDEXENTRY ausIndexTable[OP_CSTR_INDEX_SIZE];
    OPCSTR_PARAENTRY  ausParaCstr[OP_CSTR_PARA_SIZE];
    USHORT  ausNumber,
            ausOffset;

    /*  CHECK LOCK */

    PifRequestSem(husOpSem);

    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return(OP_LOCK);
    }

    /*  OPEN FILE  */

    if ((hsOpCstrFileHandle = PifOpenFile(auchOP_CSTR, auchOLD_FILE_READ_WRITE)) == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        PifReleaseSem(husOpSem);
        return(OP_FILE_NOT_FOUND);
    }

    Op_ReadCstrFile(OP_FILE_HEAD_POSITION, &CstrFileHed, sizeof(OPCSTR_FILEHED));

    /* CHECK RANGE OF Control Sting Number  */

    if (pCstr->usCstrNo == 0) {
         Op_CloseCstrFileReleaseSem();           /* close file and release semaphore */
        return(OP_ABNORMAL_CSTR);
    }

    if ( pCstr->usCstrNo > OP_CSTR_INDEX_SIZE ) {
        Op_CloseCstrFileReleaseSem();           /* close file and release semaphore */
        return(OP_ABNORMAL_CSTR);
    }

    Op_ReadCstrFile((sizeof(CstrFileHed)), &ausIndexTable, (sizeof(OPCSTR_INDEXENTRY) * OP_CSTR_INDEX_SIZE));

    ausNumber = (USHORT)(( CstrFileHed.usOffsetOfTransCstr + pCstr->IndexCstr.usCstrLength -
                          ausIndexTable[pCstr->usCstrNo - 1].usCstrLength ) / sizeof(OPCSTR_PARAENTRY));

    if ( CstrFileHed.usNumberOfCstr < ausNumber ) {
        Op_CloseCstrFileReleaseSem();           /* close file and release semaphore */
        return(OP_FILE_FULL);
    }

    if ((ausIndexTable[pCstr->usCstrNo - 1].usCstrLength != 0) ||
        (ausIndexTable[pCstr->usCstrNo - 1].usCstrOffset != 0)) {

		USHORT  ausSortPtr[OP_CSTR_INDEX_SIZE + 1],
			ausIDBase,
			ausID1,
			ausID2,
			ausID3;

		ausOffset =  ausIndexTable[pCstr->usCstrNo - 1].usCstrOffset;

        ausIDBase = 0;

        ausSortPtr[ausIDBase] = pCstr->usCstrNo;
        ausIDBase++;

        for (ausID1 = 0; ausID1 < OP_CSTR_INDEX_SIZE; ausID1++) {
            if ( ausOffset < ausIndexTable[ausID1].usCstrOffset) {
                ausSortPtr[ausIDBase] = (USHORT)(ausID1 + 1);
                ausIDBase++;
            }
        }

        for (ausID1 = 0; ausID1 < ausIDBase; ausID1++) {

            ausOffset = (USHORT)(ausIndexTable[ausSortPtr[ausID1] - 1].usCstrOffset +
                                 ausIndexTable[ausSortPtr[ausID1] - 1].usCstrLength);
            for (ausID2 = (USHORT)(ausID1 + 1); ausID2 < ausIDBase; ausID2++) {

                if (ausOffset == ausIndexTable[ausSortPtr[ausID2] - 1].usCstrOffset){
                    ausID3 = ausSortPtr[ausID1 + 1];
                    ausSortPtr[ausID1 + 1] = ausSortPtr[ausID2];
                    ausSortPtr[ausID2] = ausID3;
                    break;
                }
            }
        }

        ausOffset =  ausIndexTable[pCstr->usCstrNo - 1].usCstrOffset;
        for (ausID1 = 1; ausID1 < ausIDBase; ausID1++) {

            Op_ReadCstrFile((sizeof(CstrFileHed) + (sizeof(OPCSTR_INDEXENTRY) * OP_CSTR_INDEX_SIZE) +
                                    ausIndexTable[ausSortPtr[ausID1] - 1].usCstrOffset),
                                    &ausParaCstr,
                                    ausIndexTable[ausSortPtr[ausID1] - 1].usCstrLength);

            Op_WriteCstrFile((sizeof(CstrFileHed)  + (sizeof(OPCSTR_INDEXENTRY) * OP_CSTR_INDEX_SIZE) +
                                     ausOffset),
                                     &ausParaCstr,
                                     ausIndexTable[ausSortPtr[ausID1] - 1].usCstrLength);

            /* Set Index table */
            ausIndexTable[ausSortPtr[ausID1] - 1].usCstrOffset = ausOffset;
            ausOffset = (USHORT)(ausOffset + ausIndexTable[ausSortPtr[ausID1] - 1].usCstrLength);

        }

    } else {

        ausOffset = CstrFileHed.usOffsetOfTransCstr;

    }

    /* Set Update Parameter data */

    if (pCstr->IndexCstr.usCstrLength == 0){

        ausOffset = 0;

    } else {

        Op_WriteCstrFile((sizeof(CstrFileHed)  + (sizeof(OPCSTR_INDEXENTRY) * OP_CSTR_INDEX_SIZE) + ausOffset),
                                 &pCstr->ParaCstr, pCstr->IndexCstr.usCstrLength);

    }

    /* Set Transaction Offset of Header */

    CstrFileHed.usOffsetOfTransCstr = (USHORT)(ausOffset + pCstr->IndexCstr.usCstrLength);

    /* Write File Header */

    Op_WriteCstrFile(OP_FILE_HEAD_POSITION, &CstrFileHed, sizeof(CstrFileHed));

    /* Set Index Table */

    ausIndexTable[pCstr->usCstrNo - 1].usCstrOffset = ausOffset;
    ausIndexTable[pCstr->usCstrNo - 1].usCstrLength = pCstr->IndexCstr.usCstrLength;

    /* Write Index table */

    Op_WriteCstrFile(sizeof(CstrFileHed), ausIndexTable, (sizeof(OPCSTR_INDEXENTRY) * OP_CSTR_INDEX_SIZE));

    Op_CloseCstrFileReleaseSem();           /* close file and release semaphore */
    return(OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpCstrDelete(CSTRIF *pCstr, USHORT usLockHnd)
*
*       Input:  pCstr->usCstrNo
*               pCstr->IndexCstr
*               pCstr->ParaCstr
*               usLockHnd
*
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :
*    Normal End:   OP_PERFORM
*   Abnormal End:  OP_FILE_NOT_FOUND
*                  OP_ABNORMAL_CSTR - HALT
*                  OP_LOCK
*
**  Description:  Delete a Control String in file.
*==========================================================================
*/
SHORT  OpCstrDelete(CSTRIF *pCstr, USHORT usLockHnd)
{
	OPCSTR_FILEHED  CstrFileHed = { 0 };
    OPCSTR_INDEXENTRY ausIndexTable[OP_CSTR_INDEX_SIZE];
    OPCSTR_PARAENTRY  ausParaCstr[OP_CSTR_PARA_SIZE];

    /*  CHECK LOCK */
    PifRequestSem(husOpSem);

    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return(OP_LOCK);
    }

    /*  OPEN FILE  */

    hsOpCstrFileHandle = PifOpenFile(auchOP_CSTR, auchOLD_FILE_READ_WRITE);
    if (hsOpCstrFileHandle == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        PifReleaseSem(husOpSem);
        return(OP_FILE_NOT_FOUND);
    }

    Op_ReadCstrFile(OP_FILE_HEAD_POSITION, &CstrFileHed, sizeof(OPCSTR_FILEHED));

    /* CHECK RANGE OF Ccontrol String Number  */

    if (pCstr->usCstrNo == 0) {
         Op_CloseCstrFileReleaseSem();           /* close file and release semaphore */
        return(OP_ABNORMAL_CSTR);
    }

    if ( pCstr->usCstrNo > OP_CSTR_INDEX_SIZE ) {
        Op_CloseCstrFileReleaseSem();           /* close file and release semaphore */
        return(OP_ABNORMAL_CSTR);
    }

    Op_ReadCstrFile(sizeof(CstrFileHed), &ausIndexTable, (sizeof(OPCSTR_INDEXENTRY) * OP_CSTR_INDEX_SIZE));

    /* Check Parameter Data Length */
    if (ausIndexTable[pCstr->usCstrNo - 1].usCstrLength == 0) {
        Op_CloseCstrFileReleaseSem();           /* close file and release semaphore */
        return(OP_PERFORM);
    }

    if ( CstrFileHed.usOffsetOfTransCstr > ausIndexTable[pCstr->usCstrNo - 1].usCstrLength ) {
		USHORT  ausSortPtr[OP_CSTR_INDEX_SIZE + 1],
			ausIDBase,
			ausID1,
			ausID2,
			ausID3;
		USHORT  ausOffset;

        ausOffset =  ausIndexTable[pCstr->usCstrNo - 1].usCstrOffset;

        ausIDBase = 0;

        ausSortPtr[ausIDBase] = pCstr->usCstrNo;
        ausIDBase++;

        for (ausID1 = 0; ausID1 < OP_CSTR_INDEX_SIZE; ausID1++) {
            if ( ausOffset < ausIndexTable[ausID1].usCstrOffset) {
                ausSortPtr[ausIDBase] = (USHORT)(ausID1 + 1);
                ausIDBase++;
            }
        }

        for (ausID1 = 0; ausID1 < ausIDBase; ausID1++) {

            ausOffset = (USHORT)(ausIndexTable[ausSortPtr[ausID1] - 1].usCstrOffset + ausIndexTable[ausSortPtr[ausID1] - 1].usCstrLength);
            for (ausID2 = (USHORT)(ausID1 + 1); ausID2 < ausIDBase; ausID2++) {

                if (ausOffset == ausIndexTable[ausSortPtr[ausID2] - 1].usCstrOffset){
                    ausID3 = ausSortPtr[ausID1 + 1];
                    ausSortPtr[ausID1 + 1] = ausSortPtr[ausID2];
                    ausSortPtr[ausID2] = ausID3;
                    break;
                }
            }
        }

        ausOffset =  ausIndexTable[pCstr->usCstrNo - 1].usCstrOffset;
        for (ausID1 = 1; ausID1 < ausIDBase; ausID1++) {

            Op_ReadCstrFile((sizeof(CstrFileHed) + (sizeof(OPCSTR_INDEXENTRY) * OP_CSTR_INDEX_SIZE) +
                                    ausIndexTable[ausSortPtr[ausID1] - 1].usCstrOffset),
                                    &ausParaCstr, ausIndexTable[ausSortPtr[ausID1] - 1].usCstrLength);

            Op_WriteCstrFile((sizeof(CstrFileHed) + (sizeof(OPCSTR_INDEXENTRY) * OP_CSTR_INDEX_SIZE) + ausOffset),
                                    &ausParaCstr, ausIndexTable[ausSortPtr[ausID1] - 1].usCstrLength);

            /* Set Index table */
            ausIndexTable[ausSortPtr[ausID1] - 1].usCstrOffset = ausOffset;
            ausOffset = (USHORT)(ausOffset + ausIndexTable[ausSortPtr[ausID1] - 1].usCstrLength);
        }

        /* Set Transaction Offset of Header */
        CstrFileHed.usOffsetOfTransCstr = ausOffset;

        /* Set Index Table */
        ausIndexTable[pCstr->usCstrNo - 1].usCstrOffset = 0x00;
        ausIndexTable[pCstr->usCstrNo - 1].usCstrLength = 0x00;
    }
    else  {
        /* Set Transaction Offset of Header */
        CstrFileHed.usOffsetOfTransCstr = 0x00;

        /* Clear Index table */
        memset(ausIndexTable, 0x00, (sizeof(OPCSTR_INDEXENTRY) * OP_CSTR_INDEX_SIZE));
    }

    /* Write File Header */
    Op_WriteCstrFile(OP_FILE_HEAD_POSITION, &CstrFileHed, sizeof(CstrFileHed));

    /* Write Index table */
    Op_WriteCstrFile(sizeof(CstrFileHed), ausIndexTable, (sizeof(OPCSTR_INDEXENTRY) * OP_CSTR_INDEX_SIZE));

    Op_CloseCstrFileReleaseSem();           /* close file and release semaphore */
    return(OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpCpnIndRead(CPNIF *pCpni,
*                                                USHORT usLockHnd)
*
*       Input: *pCpni->usCpnNo
*              usLockHnd
*      Output: *pCpni->ParaCpn
*       InOut: Nothing
*
**  Return   :
*     Normal End: OP_PERFORM
*   Abnormal End: OP_NOT_IN_FILE
*                 OP_FILE_NOT_FOUND
*                 OP_LOCK
*
**  Description: Read a Combination Coupon Individually
*==========================================================================
*/
SHORT   OpCpnIndRead(CPNIF *pCpni, USHORT usLockHnd)
{
    OPCPN_FILEHED CpnFileHed = { 0 };
    PifRequestSem(husOpSem);

    /*  LOCK CHECK */
    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return(OP_LOCK);
    }

    hsOpCpnFileHandle = PifOpenFile(auchOP_CPN, auchOLD_FILE_READ);
    if (hsOpCpnFileHandle == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        PifReleaseSem(husOpSem);
        return(OP_FILE_NOT_FOUND);
    }

    Op_ReadCpnFile(OP_FILE_HEAD_POSITION, &CpnFileHed, sizeof(OPCPN_FILEHED));

    if ((!pCpni->uchCpnNo) || ( pCpni->uchCpnNo > CpnFileHed.usNumberOfCpn)) {
        Op_CloseCpnFileReleaseSem();
        return(OP_ABNORMAL_CPN);
    }

    Op_ReadCpnFile((sizeof(CpnFileHed) + ((pCpni->uchCpnNo - 1) * sizeof(COMCOUPONPARA))), &pCpni->ParaCpn, sizeof(COMCOUPONPARA));

    Op_CloseCpnFileReleaseSem();
    return(OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpCstrIndRead(CSTRIF *pCstr, USHORT usLockHnd)
*
*       Input:   *pCstr->usCstrNo
*                usLockHnd
*      Output:   CSTRIF *pCstr->ParaCstr
*                CSTRIF *pCstr->IndexCstr
*       InOut:   Nothing
*
**  Return    :
*     Normal End:  OP_PERFORM
*   Abnormal End:  OP_ABNORMAL_CSTR
*                  OP_FILE_NOT_FOUND
*                  OP_LOCK
*
**  Description: Read a Control String Individually
*==========================================================================
*/
SHORT   OpCstrIndRead(CSTRIF *pCstr, USHORT usLockHnd)
{
	OPCSTR_FILEHED  CstrFileHed = { 0 };
    OPCSTR_INDEXENTRY ausIndexTable[OP_CSTR_INDEX_SIZE];

    PifRequestSem(husOpSem);

    /*  LOCK CHECK */
    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return(OP_LOCK);
    }

    /*  OPEN FILE  */
    hsOpCstrFileHandle = PifOpenFile(auchOP_CSTR, auchOLD_FILE_READ_WRITE);
    if (hsOpCstrFileHandle == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        PifReleaseSem(husOpSem);
        return(OP_FILE_NOT_FOUND);
    }

    /* Read Header in Control String File */
    Op_ReadCstrFile(OP_FILE_HEAD_POSITION, &CstrFileHed, sizeof(OPCSTR_FILEHED));

    /* Check input Data of Number */
    if ((!pCstr->usCstrNo) || ( pCstr->usCstrNo > OP_CSTR_INDEX_SIZE)) {
        Op_CloseCstrFileReleaseSem();
        return(OP_ABNORMAL_CPN);
    }

    /* Read Index Table in Control String File */
    Op_ReadCstrFile((ULONG)(sizeof(CstrFileHed)), &ausIndexTable, (sizeof(OPCSTR_INDEXENTRY) * OP_CSTR_INDEX_SIZE));

    /* Set Index Data */
    pCstr->IndexCstr.usCstrOffset = ausIndexTable[pCstr->usCstrNo - 1].usCstrOffset;
    pCstr->IndexCstr.usCstrLength = ausIndexTable[pCstr->usCstrNo - 1].usCstrLength;

    /* Read area clear */
    memset(pCstr->ParaCstr, 0x00, (sizeof(OPCSTR_PARAENTRY) * OP_CSTR_PARA_SIZE));

    if (ausIndexTable[pCstr->usCstrNo - 1].usCstrLength != 0) {

        /* Read parameter Table in Control String File */
        Op_ReadCstrFile((sizeof(CstrFileHed) + (sizeof(OPCSTR_INDEXENTRY) * OP_CSTR_INDEX_SIZE) +
                                ausIndexTable[pCstr->usCstrNo - 1].usCstrOffset),
                                pCstr->ParaCstr,
                                ausIndexTable[pCstr->usCstrNo - 1].usCstrLength);
    }


    Op_CloseCstrFileReleaseSem();
    return(OP_PERFORM);
}

/******************** New Functions (Release 3.1) BEGIN *******************/
/*
*==========================================================================
**    Synopsis:    SHORT   OpPpiAssign(PPIIF *pPif, USHORT usLockHnd)
*
*       Input:  pPif->uchPpiCode
*               pPif->ParaPpi
*               usLockHnd
*
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :
*    Normal End:   OP_PERFORM
*   Abnormal End:  OP_CHANGE_PPI
*                  OP_FILE_NOT_FOUND
*                  OP_FILE_FULL
*                  OP_LOCK
*
**  Description:  Add and change plu record.
*==========================================================================
*/
SHORT   OpPpiAssign(PPIIF *pPif, USHORT usLockHnd)
{
	OPPPI_FILEHED PpiFileHed = { 0 };

    /*  CHECK LOCK */
    PifRequestSem(husOpSem);

    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return(OP_LOCK);
    }

    /*  OPEN FILE  */
    if ((Op_PpiOpenFile()) == PIF_ERROR_FILE_NOT_FOUND) {
        PifReleaseSem(husOpSem);
        return(OP_FILE_NOT_FOUND);
    }

    Op_ReadPpiFile(OP_FILE_HEAD_POSITION, &PpiFileHed, sizeof(OPPPI_FILEHED));

    /* CHECK RANGE OF DEPT NO  */
    if (pPif->uchPpiCode == 0) {
        Op_ClosePpiFileReleaseSem();           /* close file and release semaphore */
        return(OP_ABNORMAL_PPI);
    }

    if ( pPif->uchPpiCode > PpiFileHed.usNumberOfPpi ) { //ESMITH
        Op_ClosePpiFileReleaseSem();           /* close file and release semaphore */
        return(OP_ABNORMAL_PPI);
    }

    Op_WritePpiFile( sizeof(PpiFileHed) + (ULONG)((pPif->uchPpiCode - 1) * sizeof(OPPPI_PARAENTRY)), &pPif->ParaPpi, sizeof(pPif->ParaPpi));
    Op_ClosePpiFileReleaseSem();           /* close file and release semaphore */
    return(OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpPpiIndRead(PPIIF *Pif, USHORT usLockHnd)
*
*       Input:    *Pif->uchPpiCode
*                 usLockHnd
*      Output:    PPIIF  *Pif->ParaPpi
*       InOut:    Nothing
*
**  Return    :
*     Normal End:  OP_PERFORM
*   Abnormal End:  OP_NOT_IN_FILE
*                  OP_FILE_NOT_FOUND
*                  OP_LOCK
*
**  Description: Read a PPI paramater Individually.
*==========================================================================
*/
SHORT   OpPpiIndRead(PPIIF *pPif, USHORT usLockHnd)
{
	OPPPI_FILEHED PpiFileHed = { 0 };

    PifRequestSem(husOpSem);

    /*  LOCK CHECK */

    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return(OP_LOCK);
    }

    if ((Op_PpiOpenFile()) == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        PifReleaseSem(husOpSem);
        return(OP_FILE_NOT_FOUND);
    }

    Op_ReadPpiFile(OP_FILE_HEAD_POSITION, &PpiFileHed, sizeof(PpiFileHed));

     if ((!pPif->uchPpiCode) || ( pPif->uchPpiCode > (USHORT)PpiFileHed.usNumberOfPpi)) { //ESMITH
        Op_ClosePpiFileReleaseSem();
        return(OP_ABNORMAL_PPI);
    }

    Op_ReadPpiFile((sizeof(PpiFileHed) + ((pPif->uchPpiCode - 1) * sizeof(OPPPI_PARAENTRY))), &pPif->ParaPpi, sizeof(OPPPI_PARAENTRY));

    Op_ClosePpiFileReleaseSem();
    return(OP_PERFORM);
}

/******************** New Functions (Release 3.1) END *******************/


/******************** New Functions (V3.3) BEGIN ************************/
/*
*==========================================================================
**    Synopsis:    SHORT   OpMldAssign(MLDIF *pMld, USHORT usLockHnd)
*
*       Input:  pMld->uchAddress
*               pMld->aszMnemonics
*               usLockHnd
*
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :
*    Normal End:   OP_PERFORM
*   Abnormal End:  OP_CHANGE_MLD
*                  OP_FILE_NOT_FOUND
*                  OP_FILE_FULL
*                  OP_LOCK
*
**  Description:  Add and change mld mnemonics. V3.3
*==========================================================================
*/
SHORT   OpMldAssign(MLDIF *pMld, USHORT usLockHnd)
{
    OPMLD_FILEHED MldFileHed = { 0 };

    /*  CHECK LOCK */
    PifRequestSem(husOpSem);

    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return(OP_LOCK);
    }

    /*  OPEN FILE  */
    if ((Op_MldOpenFile()) == PIF_ERROR_FILE_NOT_FOUND) {
        PifReleaseSem(husOpSem);
        return(OP_FILE_NOT_FOUND);
    }

    Op_ReadMldFile(OP_FILE_HEAD_POSITION, &MldFileHed, sizeof(OPMLD_FILEHED));
	if (MldFileHed.usNumberOfAddress < MAX_MLDMNEMO_SIZE) {
		MldFileHed.usNumberOfAddress = MAX_MLDMNEMO_SIZE;
		Op_WriteMldFile(OP_FILE_HEAD_POSITION, &MldFileHed, sizeof(OPMLD_FILEHED));
	}

    /* CHECK RANGE OF DEPT NO  */
    if (pMld->uchAddress == 0) {
        Op_CloseMldFileReleaseSem();           /* close file and release semaphore */
        return(OP_ABNORMAL_MLD);
    }

    if ( pMld->uchAddress > (UCHAR)MldFileHed.usNumberOfAddress ) {
        Op_CloseMldFileReleaseSem();           /* close file and release semaphore */
        return(OP_ABNORMAL_MLD);
    }

    Op_WriteMldFile( sizeof(MldFileHed) + (ULONG)((pMld->uchAddress - 1) * sizeof(pMld->aszMnemonics)), pMld->aszMnemonics, sizeof(pMld->aszMnemonics));
    Op_CloseMldFileReleaseSem();           /* close file and release semaphore */
    return(OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpMldIndRead(MLDIF *pMld, USHORT usLockHnd)
*
*       Input:    *pMld->uchAddress
*                 usLockHnd
*      Output:    MLDIF  *pMld->aszMnemonics
*       InOut:    Nothing
*
**  Return    :
*     Normal End:  OP_PERFORM
*   Abnormal End:  OP_NOT_IN_FILE
*                  OP_FILE_NOT_FOUND
*                  OP_LOCK
*
**  Description: Read a MLD mnemonics Individually. V3.3
*==========================================================================
*/
SHORT   OpMldIndRead(MLDIF *pMld, USHORT usLockHnd)
{
	OPMLD_FILEHED MldFileHed = { 0 };

    PifRequestSem(husOpSem);

    /*  LOCK CHECK */
    if (usLockHnd) {
        if (Op_LockCheck(usLockHnd) == OP_LOCK) {
            PifReleaseSem(husOpSem);
            return(OP_LOCK);
        }
    }

    if ((Op_MldOpenFile()) == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        PifReleaseSem(husOpSem);
        return(OP_FILE_NOT_FOUND);
    }

    Op_ReadMldFile(OP_FILE_HEAD_POSITION, &MldFileHed, sizeof(MldFileHed));

    memset(&pMld->aszMnemonics, 0, (OP_MLD_MNEMONICS_LEN * 2));

    if ((!pMld->uchAddress) || ( pMld->uchAddress > (UCHAR)MldFileHed.usNumberOfAddress)) {
        Op_CloseMldFileReleaseSem();
        return(OP_PERFORM);
    }

    Op_ReadMldFile((sizeof(MldFileHed) + (pMld->uchAddress - 1) * sizeof(pMld->aszMnemonics)), pMld->aszMnemonics, sizeof(pMld->aszMnemonics));

    Op_CloseMldFileReleaseSem();
    return(OP_PERFORM);
}

/******************** New Functions (V3.3) END   ************************/

/* Saratoga Start */

/*
*===========================================================================
** Synopsis:    SHORT    OpConvertPluError(USHORT usError);
*     Input:    usError
*     Output:   nothing
*     InOut:    nothing
*
** Return:      ErrorNo
*
** Description: This function converts error code to leadthru number.
*===========================================================================
*/
SHORT   OpConvertPluError(USHORT usError)
{
    SHORT   sErrorNo;

    switch ( usError ) {

    case SPLU_COMPLETED:                 /* Success        */
        sErrorNo = OP_PERFORM;          /* 0              */
        break;

    case SPLU_INVALID_DATA:              /* Error   1002  */
    case SPLU_FILE_NOT_FOUND:            /* Error   1005  */
        sErrorNo = OP_FILE_NOT_FOUND;   /* Error   -2    */
        break;

    case SPLU_DEVICE_FULL:               /* Error   1006  */
        sErrorNo = OP_NO_MAKE_FILE;     /* Error   -6    */
        break;

    case SPLU_FILE_OVERFLOW:             /* Error   1007  */
        sErrorNo = OP_PLU_FULL;         /* 2 * File Full */
        break;

    case SPLU_REC_LOCKED:                /* Error   1008  */
    case SPLU_REC_PROTECTED:             /* Error   1011  */
    case SPLU_FILE_LOCKED:               /* Error   1021  */
        sErrorNo = OP_LOCK;             /* Error   -5    */
        break;

    case SPLU_REC_NOT_FOUND:             /* Error   1010  */
        sErrorNo = OP_NOT_IN_FILE;      /*   -3 * Not In File */
        break;

    case SPLU_FUNC_CONTINUED:            /* Error   1015  */
        sErrorNo = OP_CONT;             /* Error   -38   */
        break;

    case SPLU_END_OF_FILE:               /* Error   1016  */
        sErrorNo = OP_EOF;              /* Error   -38   */
        break;

    case SPLU_INVALID_HANDLE:            /* Error   1003  */
    case SPLU_TOO_MANY_USERS:            /* Error   1004  */
    case SPLU_ADJ_VIOLATION:             /* Error   1013  */
    case SPLU_FUNC_KILLED:               /* Error   1024  */
    default:
        sErrorNo = OP_PLU_ERROR;
        break;
    }
    return (sErrorNo);
}

/* End of function :OpConvertPluError() */

/*                 
*==========================================================================
**    Synopsis:   SHORT  Op_OepOpenFile(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description:  Open "PARAMOEP" file.                         Saratoga
*==========================================================================
*/
SHORT  Op_OepOpenFile(USHORT usOepTableNo)
{
	hsOpOepFileHandle = -1;    // Op_OepOpenFile() initialize the variable in case there is a problem

	if (usOepTableNo < sizeof(auchOP_OEP_TABLE)/sizeof(auchOP_OEP_TABLE[0])) {
		hsOpOepFileHandle = PifOpenFile(auchOP_OEP_TABLE[usOepTableNo], auchOLD_FILE_READ_WRITE);
	}
    return (hsOpOepFileHandle);
}

/*======= End of Source =======*/