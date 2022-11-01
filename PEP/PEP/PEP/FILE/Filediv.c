/*
* ---------------------------------------------------------------------------
* Title         :   Divides File
* Category      :   File, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   FILEDIV.C
* Copyright (C) :   1995, AT&T Corp. E&M OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Abstract : This module provides the following functions
*               FileDivide():             Divide the standard PEP file into component files
*               FileToEachFile():         Divide a file to temporary files
*               FileToCouponFile()        Convert coupon file to current release
*               FileToEmployeeTimeFile()  Convert ETK file to current release
*               FileToDepartmentFile()    Convert Department file to current release
*               FileToDeptFile():         Divide a file to department file
*               FileConv():               Convert debug file to file w/ ETK
* ---------------------------------------------------------------------------
* Update Histories
*
*   Date    : Ver.Rev. :   Name     : Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
* Sep-05-95 : 01.00.02 : hkato      : R3.0 FVT.
* Sep-20-95 : 01.00.03 : T.Nakahata : migrate file config for V1.00.02
* Feb-28-96 : 03.00.05 : M.Suzuki   : Add R3.1
* Jan-28-00 : 01.00.00 : hrkato     : Saratoga
* ===========================================================================
* ===========================================================================
* PVCS Entry
* ---------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
* ===========================================================================
*/
/*
* ===========================================================================
*       Include File Declarations
* ===========================================================================
*/
#include    <windows.h>
#include    <string.h>
#include    <stdlib.h>

#include    <ecr.h>
#include    <r20_pif.h>
#include    <plu.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csop.h>
#include    <csetk.h>
#include    <cscas.h>
#include	"..\PEPCOMM\CAS\CSCASIN.H"
#include    <rfl.h>
/* NCR2172 */
#include    <pararam.h>
#include    "..\pepcomm\opepara\csopin.h"

#include    "pep.h"
#include    "pepcomm.h"
#include    "pepflag.h"
#include    "trans.h"
#include    "file.h"
#include    "filel.h"

#include    "../PEPCOMM/OPEPARA/csopHGV210.h"


extern TRANSFER Trans;

SHORT   OpPluAssignRecPlu (PRECPLU pRecPlu, USHORT  usAdjNo, USHORT usLockHnd);



/************************************************************\
|*                                                          *|
|*  Synopsis:   BOOL FileDivide(HWND hDlg, int iFile)       *|
|*              hDlg    handle of dialog box                *|
|*              iFile   handle of file                      *|
|*                                                          *|
|*  Return:     TRUE:   success                             *|
|*              FALSE:  failure                             *|
|*                                                          *|
|* Description: This function divides to each files.        *|
|*                                                          *|
\************************************************************/
BOOL FileDivide(HWND hDlg, HANDLE iFile)
{
	HANDLE	logoHandle;
    WCHAR   szWinPath[256];
	WCHAR	szLogoFile[PEP_FILENAME_LEN]; //ESMITH LAYOUT
    WCHAR    i, szFname[FILE_MAXLEN_TEXT];
    UINT    nID,iCount,j;
	DWORD	bytesRead, fileError;
    void    *pBufStart;  /* NCR2172 */
	CONST WCHAR szSpaceTrans[] = WIDE("                    "),
				szSpaceSpec[] = WIDE("    "),
				szSpacePri[] = WIDE("          "),
				szNull[] = WIDE(""),
				szSpaceLDT[] = WIDE("                    "); //Length of PARA_LEADTHRU_LEN, 20 spaces


    /* move seek pointer    */
    fileError = SetFilePointer(iFile, 0L, NULL, FILE_BEGIN);
	//fileError = GetLastError();
	if ( fileError == (DWORD)-1/*!= NO_ERROR*/) {
        return FALSE;
    }

	ReadFile(iFile, &Fheader, sizeof(FHEADER), &bytesRead, NULL);
	if (bytesRead != sizeof(FHEADER)) {
		return FALSE;
	}

	// We add this test to ensure that if the file read in has a Para
	// length greater than the size of the UNINIRAM buffer, Para, then
	// we set the length to the size of Para to prevent buffer overrun
	// overwriting important data.
	// The necessity for this was found during verification of 32 bit PEP
	// when reading in an old LePeeps PEP file that had the wrong length
	// in its header (by wrong length, the Para.iLen was a number slightly
	// larger than it should have been by a factor of 72 bytes).
	// We go ahead and do other checks on the header info as well just to be safe.
	if (Fheader.Mem.Para.lLen > sizeof(UNINIRAM))
		Fheader.Mem.Para.lLen = sizeof(UNINIRAM);

	if (Fheader.Mem.Config.lLen > sizeof(FCONFIG))
		Fheader.Mem.Config.lLen = sizeof(FCONFIG);

	if (Fheader.Mem.Trans.lLen > sizeof(TRANSFER))
		Fheader.Mem.Trans.lLen = sizeof(TRANSFER);


    /* ----- Finalize mass memory module,   Saratoga ----- */
    PluFinalize();

    /*
		divide the PEP file into each of the component files.

		Fheader.File.aRoom contains the offsets and the number of
		bytes for each of the component files that are incorporated
		into the the PEP data file.  The aRoom array contains the
		following mapping of component files:
			Index      Component File
			   0       IDS_FILE_NAME_DEPT
			   1       IDS_FILE_NAME_PLU
			   2       IDS_FILE_NAME_SER
			   3       IDS_FILE_NAME_CAS
			   4       IDS_FILE_NAME_ETK
			   5       IDS_FILE_NAME_DIRECT
			   6       IDS_FILE_NAME_CPN
			   7       IDS_FILE_NAME_CSTR
			   8       IDS_FILE_NAME_PPI
			   9       IDS_FILE_NAME_MLD
		      10       IDS_FILE_NAME_FPDB          Fingerprint definition file for biometrics
		      11       IDS_FILE_NAME_RSN           Reason Code mnemonics file
			  14       IDS_FILE_NAME_PLUINDEX
			  15       IDS_FILE_NAME_OEPFILE
			  16       IDS_FILE_NAME_LOGOIDX
	 */
    for (i = 0, nID = IDS_FILE_NAME_DEPT; nID <= IDS_FILE_NAME_MLD; i++, nID++) {
        LoadString(hResourceDll, nID, szFname, PEP_STRING_LEN_MAC(szFname));
        if (FileToEachFile(iFile, Fheader.File.aRoom[i].lOffset, Fheader.File.aRoom[i].lLen, szFname) == FALSE) {
            return FALSE;
        }
    }

	i = 10;
	LoadString(hResourceDll,  IDS_FILE_NAME_FPDB, szFname, PEP_STRING_LEN_MAC(szFname));
    if (FileToEachFile(iFile, Fheader.File.aRoom[i].lOffset, Fheader.File.aRoom[i].lLen, szFname) == FALSE) {
       return FALSE;
    }

	i = 11;
	LoadString(hResourceDll,  IDS_FILE_NAME_RSN, szFname, PEP_STRING_LEN_MAC(szFname));
    if (FileToEachFile(iFile, Fheader.File.aRoom[i].lOffset, Fheader.File.aRoom[i].lLen, szFname) == FALSE) {
       return FALSE;
    }

	i = 14;
	LoadString(hResourceDll,  IDS_FILE_NAME_PLUINDEX, szFname, PEP_STRING_LEN_MAC(szFname));
    if (FileToEachFile(iFile, Fheader.File.aRoom[i].lOffset, Fheader.File.aRoom[i].lLen, szFname) == FALSE) {
       return FALSE;
    }

    i = 15;
    LoadString(hResourceDll,  IDS_FILE_NAME_OEPFILE, szFname, PEP_STRING_LEN_MAC(szFname));
    if (FileToEachFile(iFile, Fheader.File.aRoom[i].lOffset, Fheader.File.aRoom[i].lLen, szFname) == FALSE) {
       return FALSE;
    }

    i = 16;
	LoadString(hResourceDll, IDS_FILE_NAME_LOGOIDX,szFname, PEP_STRING_LEN_MAC(szFname));
    if (FileToEachFile(iFile, Fheader.File.aRoom[i].lOffset, Fheader.File.aRoom[i].lLen, szFname) == FALSE) {
       return FALSE;
    }

    /* ----- Initialize mass memory module, Saratoga ----- */
    PluInitialize();

    /* divide each memory   */
    /* parameter            */
    fileError = SetFilePointer(iFile, Fheader.Mem.Para.lOffset, NULL, FILE_BEGIN);
	//fileError = GetLastError();
	if ( fileError == -1) {
        return FALSE;
    }

    pBufStart = &ParaMDC;//we set the pointer to ParaMDC instead of Para, because the first
								//information in Para is now file version information.

	if (Fheader.Mem.Para.lLen > sizeof (Para) - sizeof (Para.szVer)) {
		// ensure that the size of the data does not exceed the size of Para
		// causing a buffer overflow overwriting adjacent data.
		Fheader.Mem.Para.lLen = sizeof (Para) - sizeof (Para.szVer);
	}
	ReadFile(iFile, pBufStart, Fheader.Mem.Para.lLen, &bytesRead, NULL);
	if (bytesRead != (DWORD)Fheader.Mem.Para.lLen) {
        return FALSE;
    }

    /* Check to make sure all Lead Thru Messages have been inserted
	 | If any contain only spaces, szSpaceLDT, or nothing, szNull,
	 | insert the default mnemonic from resource file. ESMITH
	 */

	//Mnemonic Conversion Test
	// If any of the Mnemonics contain only spaces or contain nothing
	// the default mnemonic from the resource file will be loaded. This
	// was tested for Lead Thru message, now these tested will be converted
	// to handle the Transaction Mnemonics and Special Mnemonics and the Print
	// Mnemonics. RZACHARY

	//Transaction Mnemonics
		for(iCount = 0, nID = IDS_P20_TRANS_DEFAULT_001; nID <= IDS_P20_TRANS_DEFAULT_400; iCount++, nID++){
			LoadString(hResourceDll, nID, szFname, PEP_STRING_LEN_MAC(szFname));
			if( 0 == _wcsnicmp(ParaTransMnemo[iCount], szNull, PEP_STRING_LEN_MAC(szNull))	||
				0 == _wcsnicmp(ParaTransMnemo[iCount], szSpaceTrans, PEP_STRING_LEN_MAC(szSpaceTrans)) )
			{
				for(j =0; j < PARA_TRANSMNEMO_LEN; j++){
					ParaTransMnemo[iCount][j] = (WCHAR) szFname[j];
				}
			}
			else {/*Do Nothing*/}
		}
		//Lead_Thru Mnemonics
		for(iCount = 0, nID = IDS_P21_LDT_DEFAULT_001; nID <= IDS_P21_LDT_DEFAULT_192; iCount++, nID++)	{
			LoadString(hResourceDll, nID, szFname, PEP_STRING_LEN_MAC(szFname));
			if( 0 == _wcsnicmp(Para.ParaLeadThru[iCount], szNull, PEP_STRING_LEN_MAC(szNull))	||
				0 == _wcsnicmp(Para.ParaLeadThru[iCount], szSpaceLDT, PEP_STRING_LEN_MAC(szSpaceLDT)) )
			{
				memcpy(Para.ParaLeadThru[iCount], szFname, PARA_LEADTHRU_LEN * 2);//Multiply by 2bytes
			}

			else{/* Do Nothing*/}
		}
		//Special Mnemonics
		for(iCount = 0, nID = IDS_P23_SPCL_MNEM_DEFAULT_001; nID <= IDS_P23_SPCL_MNEM_DEFAULT_050; iCount++, nID++) {
			LoadString(hResourceDll, nID, szFname, PEP_STRING_LEN_MAC(szFname));
			if( 0 == _wcsnicmp(ParaSpeMnemo[iCount], szNull, PARA_SPEMNEMO_LEN)	||
				0 == _wcsnicmp(ParaSpeMnemo[iCount], szSpaceSpec, PARA_SPEMNEMO_LEN) )
			{
				for(j = 0; j < PARA_SPEMNEMO_LEN; j++){
					ParaSpeMnemo[iCount][j] = (WCHAR) szFname[j];
				}
			}
			else{/* Do Nothing*/}
		}

		//Print Modifier Mnemonics
		for(iCount = 0, nID = IDS_P47_PRINT_MNEM_DEFAULT_001; nID <= IDS_P47_PRINT_MNEM_DEFAULT_008; iCount++, nID++) {
			LoadString(hResourceDll, nID, szFname, PEP_STRING_LEN_MAC(szFname));
			if( ( 0 == _wcsnicmp(ParaPrtModi[iCount], szNull, PARA_PRTMODI_LEN)	)||
				( 0 == _wcsnicmp(ParaPrtModi[iCount], szSpacePri, PARA_PRTMODI_LEN) ) )
			{
				memcpy(ParaPrtModi[iCount],szFname,PARA_PRTMODI_LEN * 2);//Multiply by 2bytes
			}
			else{/*Do Nothing*/}
		}

		// fixup the size of the MLD file if we think it is less than our current
		// size.  The number of records in the MLD file may change from release to
		// release but it is an external file not a memory resident file.
		if (ParaFlexMem[FLEX_MLD_ADR - 1].ulRecordNumber < MAX_MLDMNEMO_SIZE) {
			ParaFlexMem[FLEX_MLD_ADR - 1].ulRecordNumber = MAX_MLDMNEMO_SIZE;
		}

    /* configuration        */
    fileError = SetFilePointer(iFile, Fheader.Mem.Config.lOffset, NULL, FILE_BEGIN);
	if (fileError == -1) {
		fileError = GetLastError();
        return FALSE;
    }
    ReadFile(iFile, &Fconfig, Fheader.Mem.Config.lLen, &bytesRead, NULL);
	if (bytesRead != (DWORD)Fheader.Mem.Config.lLen) {
		fileError = GetLastError();
        return FALSE;
    }

    /* --- correct RAM DISK start address for previous version, because it
        did not calculate FAT address. (9/20/95) --- */

    Fconfig.ulStartAddr = (unsigned long)( FILE_MADDR + FILE_FAT_ADDR );
    Fconfig.usMemory = FILE_SIZE_64KB * FILE_SIZE_48MBTOKB;  // set default Flash memory 48MB RJC

    /* transfer             */
    fileError = SetFilePointer(iFile, Fheader.Mem.Trans.lOffset, NULL, FILE_BEGIN);
	if ( fileError == -1) {
        return FALSE;
    }
    ReadFile(iFile, TransGetDataAddr(), Fheader.Mem.Trans.lLen, &bytesRead, NULL);
	if (bytesRead != (UINT)Fheader.Mem.Trans.lLen) {
		fileError = GetLastError();
        return FALSE;
    }

	nID = IDS_FILE_NAME_LOGOIDX;

#if 1
	GetPepTemporaryFolder(NULL, szWinPath, PEP_STRING_LEN_MAC(szWinPath));
#else
	GetPepModuleFileName(NULL, szWinPath, PEP_STRING_LEN_MAC(szWinPath));
	szWinPath[wcslen(szWinPath)-8] = '\0';  //remove pep.exe from path
	wcscat( szWinPath, WIDE("\\"));
#endif

	LoadString(hResourceDll, nID, szLogoFile, PEP_STRING_LEN_MAC(szLogoFile));

	wcscat(szWinPath, szLogoFile);
	logoHandle = CreateFilePep(szWinPath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
					NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	CloseHandle(logoHandle);

    /* --- set progarmmable report ini. file name R3.1 --- */
    wcsncpy(szCommRptSaveName, Fheader.Fid.szRptName, PEP_FILENAME_LEN);

	/* --- set Layout Manager file name Added JHHJ for release 2.0--- */
	wcsncpy(szCommTouchLayoutSaveName, Fheader.Fid.szTouchLayoutName, PEP_FILENAME_LEN); //ESMITH LAYOUT
	wcsncpy(szCommKeyboardLayoutSaveName, Fheader.Fid.szKeyboardLayoutName, PEP_FILENAME_LEN);
	wcsncpy(szCommLogoSaveName, Fheader.szReceiptLogoName, PEP_FILENAME_LEN);
	wcsncpy(szCommIconSaveName, Fheader.Fid.szIconFileDirectory, PEP_FILENAME_LEN);

    PepResetModFlag(PEP_MF_ALL, 0);                 /* reset change flag    */
    PepSetModFlag(hwndPepMain, PEP_MF_OPEN, 0);     /* set open flag        */

    return TRUE;

    hDlg = hDlg;
}

/************************************************************\
|*                                                          *|
|*  Synopsis:   BOOL FileToEachFile(int iFile,              *|
|*                                  LONG lOffset,           *|
|*                                  LONG lSize,             *|
|*                                  char *pszFileName)      *|
|*              iFile       handle of file                  *|
|*              lOffset     file offset                     *|
|*              lSize       file size                       *|
|*              pszFileName pointer of file name            *|
|*                                                          *|
|*  Return:     TRUE:   success                             *|
|*              FALSE:  failure                             *|
|*                                                          *|
|* Description: This function creates the file.             *|
|*                                                          *|
\************************************************************/
BOOL FileToEachFile(HANDLE iFile, LONG lOffset, LONG lSize, WCHAR *pszFileName)
{
    SHORT       sDivFile;
    int         iLoop;
    DWORD       dwRestBytes, dwBytes;
    BOOL        bReturn;
	DWORD		bytesRead, fileError;
	UCHAR       lpGlobalMem[FILE_SIZE_SHORT];

    /* open file    */
    if ((sDivFile = PifOpenFile(pszFileName, "ow")) >= 0) {
		PifCloseFile (sDivFile);
		PifDeleteFile (pszFileName);
	}
    if ((sDivFile = PifOpenFile(pszFileName, "nw")) >= 0) {
        /* get file size    */
        iLoop      = (int)(lSize / FILE_SIZE_SHORT);
        dwRestBytes = (lSize % FILE_SIZE_SHORT);

        /* seek file pointer    */
        fileError = SetFilePointer(iFile, lOffset, NULL, FILE_BEGIN);
		//fileError = GetLastError();
		if ( fileError == (ULONG)lOffset) {
            /* file to file */
            for (;;) {
                if (iLoop == 0) dwBytes = dwRestBytes;
                else            dwBytes = FILE_SIZE_SHORT;

                /* read file to global memory   */
				ReadFile(iFile, lpGlobalMem, dwBytes, &bytesRead, NULL);
				if (bytesRead != dwBytes) {
                    bReturn = FALSE;
                    break;
                }

                /* write global memory to file  */
                PifWriteFile((USHORT)sDivFile, lpGlobalMem, dwBytes);

                if (iLoop == 0) {   /* end  */
                    bReturn = TRUE;
                    break;
                } else  {           /* loop */
                    iLoop--;
                }
            }

        } else {
            bReturn = FALSE;
        }

        PifCloseFile((USHORT)sDivFile);

    } else {
        bReturn = FALSE;
    }

    return(bReturn);
}

/**
 * A generalized file read function that can be utilized by
 * many of the specialized file read functions.
 */
BOOL GenFileRead(HANDLE iFile, VOID *readTo, ULONG size){
	DWORD	bytesRead;

	memset(readTo, 0x00, size);
	ReadFile (iFile, readTo, size, &bytesRead, NULL);
	return ( bytesRead == size );
}

/**
 * A generalized file read function that sets the FilePointer to the offset
 * given, if this is impossible, FALSE is returned.
 * The file is then read according to the size specified.
 */
BOOL GenFileReadAtOffset(HANDLE iFile, LONG lOffset, VOID *readTo, ULONG size){
    if(SetFilePointer(iFile,lOffset,NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER){
    	return FALSE;
    }

    return GenFileRead(iFile,readTo,size);
}

//==============================================================================
//==============================================================================
//==============================================================================
//---------------   Coupon File Division and Creation   -----------------
//
//    Functions that read a particular release coupon file
BOOL  CouponRead_01040000 (HANDLE iFile, LONG lOffset, VOID *pHead, int iItem, COMCOUPONPARA_HGV10 *pParav140){
	return GenFileRead(iFile,pParav140,sizeof(COMCOUPONPARA_HGV10));
}

BOOL  CouponRead_02000100 (HANDLE iFile, LONG lOffset, VOID *pHead, int iItem, COMCOUPONPARA_V201 *pParav201){
	return GenFileRead(iFile,pParav201,sizeof(COMCOUPONPARA_V201));
}

BOOL  CouponRead_02000300 (HANDLE iFile, LONG lOffset, VOID *pHead, int iItem, COMCOUPONPARA_V203 *pParav203){
	return GenFileRead(iFile,pParav203,sizeof(COMCOUPONPARA_V203));
}

BOOL  CouponRead_02010000 (HANDLE iFile, LONG lOffset, VOID *pHead, int iItem, COMCOUPONPARA *pParav210){
	return GenFileRead(iFile,pParav210,sizeof(COMCOUPONPARA));
}

//    Conversion functions that convert the data from one release to the next.
BOOL  Coupon_01040000_02000100 (COMCOUPONPARA_HGV10 *pParav140, COMCOUPONPARA_V201 *pParav201){
	int i, j;

	memset (pParav201, 0, sizeof(COMCOUPONPARA_V201));
	pParav201->uchCouponStatus[0] = pParav140->uchCouponStatus[0] ;
	pParav201->uchCouponStatus[1] = pParav140->uchCouponStatus[1] ;
	pParav201->ulCouponAmount = pParav140->ulCouponAmount;
	for (i = 0; i < OP_CPN_NAME_SIZE_HGV10; i++) {
		pParav201->aszCouponMnem[i] = pParav140->aszCouponMnem[i];
	}
	memcpy (pParav201->uchCouponAdj, pParav140->uchCouponAdj, sizeof(pParav201->uchCouponAdj));
	for (i = 0; i < OP_CPN_TARGET_SIZE_HGV10; i++) {
		for (j = 0; j < OP_PLU_LEN_HGV10; j++) {
			pParav201->auchCouponTarget[i][j] = pParav140->auchCouponTarget[i][j];
		}
	}
	return TRUE;
}

BOOL  Coupon_02000100_02000300 (COMCOUPONPARA_V201 *pParav201, COMCOUPONPARA_V203 *pParav203){
	memset (pParav203, 0, sizeof(COMCOUPONPARA_V203));
	pParav203->uchCouponStatus[0] = pParav201->uchCouponStatus[0] ;
	pParav203->uchCouponStatus[1] = pParav201->uchCouponStatus[1] ;
	pParav203->ulCouponAmount = pParav201->ulCouponAmount;
	wcsncpy (pParav203->aszCouponMnem, pParav201->aszCouponMnem, 12);
	memcpy (pParav203->uchCouponAdj, pParav201->uchCouponAdj, sizeof(pParav201->uchCouponAdj));
	memcpy (pParav203->auchCouponTarget, pParav201->auchCouponTarget, sizeof(pParav201->auchCouponTarget));

	pParav203->uchAutoCPNStatus = 0;		//new value in this version

	return TRUE;
}

BOOL  Coupon_02000300_02010000 (COMCOUPONPARA_V203 *pParav203, COMCOUPONPARA *pParav210){
	memset (pParav210, 0, sizeof(COMCOUPONPARA));
	pParav210->uchCouponStatus[0] = pParav203->uchCouponStatus[0] ;
	pParav210->uchCouponStatus[1] = pParav203->uchCouponStatus[1] ;
	pParav210->ulCouponAmount = pParav203->ulCouponAmount;
	wcsncpy (pParav210->aszCouponMnem, pParav203->aszCouponMnem, 12);
	memcpy (pParav210->uchCouponAdj, pParav203->uchCouponAdj, sizeof(pParav203->uchCouponAdj));
	memcpy (pParav210->auchCouponTarget, pParav203->auchCouponTarget, sizeof(pParav203->auchCouponTarget));

	pParav210->uchAutoCPNStatus = pParav203->uchAutoCPNStatus;

	pParav210->usBonusIndex = 0;			//new value in this version

	return TRUE;
}

BOOL  Coupon_02010000_02010200_NOCHANGE (COMCOUPONPARA *pParav210a, COMCOUPONPARA *pParav210b){
	memcpy (pParav210b, pParav210a, sizeof(COMCOUPONPARA));
	return TRUE;
}

typedef struct _tagConversionFunctionList {
	int   nFileHeadVer;
	int   nBuildNumber;
	BOOL  (* ItemRead) (HANDLE iFile, LONG lOffset, VOID *pHead, int iItem, VOID *pParav201);
	BOOL  (* ItemConvert) (VOID *pFrom, VOID *pTo);
} ConversionFunctionListItem;

/*
	The following table is used to guide the conversion engine for a particular
	file.  The way this table is used is that the conversion function is told
	the version of the file that is being converted from.  The conversion engine
	will then lookup that file version identifier in the table below scanning
	the table until it finds the first table entry with that file version
	identifier.

	Beginning with that row in the table, the conversion engine will begin the
	conversion process.  The first step is to read in the data item from the
	input file that is being converted.  that is what the function pointed to by
	the ItemRead () struct member is used for.

	Next, a series of conversion steps are performed on the data buffer to convert
	the data from the beginning file format through a series of transformations to
	the ending version file format.  The end of the sequence of steps is indicated
	when a ItemConvert () struct member contains a NULL pointer.
*/
ConversionFunctionListItem CouponConversionList[] = { 
	{ FILE_HEAD_VER_GP30, 0, CouponRead_01040000, Coupon_01040000_02000100 }, // Rel 1.4 conversion begins here
	{ FILE_HEAD_VER_GP30, 0, CouponRead_01040000, Coupon_02000100_02000300 }, // Rel 1.4 conversion begins here
	{ FILE_HEAD_VER_GP30, 0, CouponRead_01040000, Coupon_02000300_02010000 }, // Rel 1.4 conversion begins here
	{ FILE_HEAD_VER_GP30, 0, 0, 0 },
	{ FILE_HEAD_VER_GP40, 0, CouponRead_02000100, Coupon_02000100_02000300 }, // Rel 2.0.0 conversion begins here
	{ FILE_HEAD_VER_GP40, 0, CouponRead_02000100, Coupon_02000300_02010000 },
	{ FILE_HEAD_VER_GP40, 0, 0, 0 },
	{ FILE_HEAD_VER_GP41, 0, CouponRead_02000100, Coupon_02000100_02000300 }, // Rel 2.0.0 conversion
	{ FILE_HEAD_VER_GP41, 0, CouponRead_02000100, Coupon_02000300_02010000 },
	{ FILE_HEAD_VER_GP41, 0, 0, 0 },
	{ FILE_HEAD_VER_GP42, 0, CouponRead_02000100, Coupon_02000100_02000300 }, // Rel 2.0.0 conversion
	{ FILE_HEAD_VER_GP42, 0, CouponRead_02000100, Coupon_02000300_02010000 },
	{ FILE_HEAD_VER_GP42, 0, 0, 0 },
	{ FILE_HEAD_VER_GP201, 0, CouponRead_02000100, Coupon_02000100_02000300 }, // Rel. 2.0.1 conversion begins here
	{ FILE_HEAD_VER_GP201, 0, CouponRead_02000100, Coupon_02000300_02010000 },
	{ FILE_HEAD_VER_GP201, 0, 0, 0 },
	{ FILE_HEAD_VER_GP203, 0, CouponRead_02000300, Coupon_02000300_02010000 }, // Rel. 2.0.3 conversion begins here
	{ FILE_HEAD_VER_GP203, 0, 0, 0 },
	{ FILE_HEAD_VER_GP204, 0, CouponRead_02000300, Coupon_02000300_02010000 }, // Rel. 2.0.4 conversion begins here
	{ FILE_HEAD_VER_GP204, 0, 0, 0 },
	{ FILE_HEAD_VER_GP205, 0, CouponRead_02000300, Coupon_02000300_02010000 }, // Rel. 2.0.5 conversion begins here
	{ FILE_HEAD_VER_GP205, 0, 0, 0 },
	{ FILE_HEAD_VER_GP210, 0, CouponRead_02010000, Coupon_02010000_02010200_NOCHANGE },
	{ FILE_HEAD_VER_GP210, 0, 0, 0 },
	{ FILE_HEAD_VER_GP212, 0, CouponRead_02010000, Coupon_02010000_02010200_NOCHANGE },
	{ FILE_HEAD_VER_GP212, 0, 0, 0 },
	{ FILE_HEAD_VER_GP220, 0, CouponRead_02010000, Coupon_02010000_02010200_NOCHANGE },
	{ FILE_HEAD_VER_GP220, 0, 0, 0 }
};

BOOL FileToCouponFile(HANDLE iFile, LONG lOffset, LONG lSize, WCHAR* pszFileName, int iStartConvert)
{
    OPCPN_FILEHED      FileHead;
    CPNIF              InParaData;
	UCHAR              *ucParaBufRay[2], ucParaBuf[1024];
    BOOL               bReturn;
    USHORT             usCpnNo;
    BOOL               fCheck = FALSE;
    SHORT              sError;
	int                j;
	DWORD		       bytesRead, fileError;

	ucParaBufRay[0] = &ucParaBuf[0];
	ucParaBufRay[1] = &ucParaBuf[sizeof(ucParaBuf)/2];

    /*----- R3.0 -----*/

    if (lSize == 0L) {
       return TRUE;
    }

    /* seek file pointer    */
    fileError = SetFilePointer(iFile, lOffset, NULL, FILE_BEGIN);
	//fileError = GetLastError();
	if (fileError == -1/*NO_ERRORFheader.Mem.Para.lOffset*/) {
        return FALSE;
    }
    /* read header file to memory   */
    ReadFile(iFile, &FileHead, sizeof(FileHead), &bytesRead, NULL);
	if ( bytesRead != sizeof(FileHead)) {
        return FALSE;
    }

    /* open file    */
    if(OpCpnCreatFile(FileHead.usNumberOfCpn, 0) != OP_PERFORM){
        return FALSE;
    }

	// look up in the conversion table until we find which file version we
	// are converting from.  We will then begin with that table entry to do
	// the conversion sequence.
	//
	// This lookup is ignoring the build number for now.
	for (j = 0; j < sizeof(CouponConversionList)/sizeof(CouponConversionList[0]); j++) {
		if (CouponConversionList[j].nFileHeadVer == iStartConvert) {
			break;
		}
	}

	bReturn = TRUE;
	if (j < sizeof(CouponConversionList)/sizeof(CouponConversionList[0])) {
		iStartConvert = j;
	    
		/* file to file */
		for (usCpnNo = 1; usCpnNo <= FileHead.usNumberOfCpn; usCpnNo++) {
			int jk1, jk2;
			j = iStartConvert;
			jk1 = 0; jk2 = 1;
			CouponConversionList[j].ItemRead (iFile, 0, 0, 0, ucParaBufRay[jk1]);
			for ( ; CouponConversionList[j].ItemConvert; j++) {
				CouponConversionList[j].ItemConvert (ucParaBufRay[jk1], ucParaBufRay[jk2]);
				jk1 = 1 - jk1;
				jk2 = 1 - jk2;
			}
			InParaData.uchCpnNo = usCpnNo;
			memcpy (&InParaData.ParaCpn, ucParaBufRay[jk1], sizeof(InParaData.ParaCpn));
			sError = OpCpnAssign(&InParaData, 0);
		}
	}
    return bReturn;
}
//-----------------   End of Coupon File Conversion     ------------------------


//----------------   Begin ETK File Conversion     ------------------------
//
//    Functions that read a particular release ETK file
BOOL  EmployeeTimeRead_01040000 (HANDLE iFile, LONG lOffset, VOID *pHead, int iItem, ETK_INDEX_HGV10 *pParav140){
	return GenFileRead(iFile,pParav140,sizeof(ETK_INDEX_HGV10));
}

BOOL  EmployeeTimeRead_02000100 (HANDLE iFile, LONG lOffset, VOID *pHead, int iItem, ETK_INDEX *pParav201){
	return GenFileRead(iFile,pParav201,sizeof(ETK_INDEX));
}

//    Conversion functions that convert the data from one release to the next.
BOOL  EmployeeTime_01040000_02000000 (ETK_INDEX_HGV10 *pParav140, ETK_INDEX *pParav200){
	int i, j;

	memset (pParav200, 0, sizeof(ETK_INDEX));
	pParav200->EtkJC			= pParav140->EtkJC;
	pParav200->ulEmployeeNo	= pParav140->ulEmployeeNo;
	{
		WCHAR *cpyWchar = pParav200->auchEmployeeName;
		UCHAR *cpyUchar = pParav140->auchEmployeeName;
		for (i = 0, j = 1; i < sizeof(pParav140->auchEmployeeName); j++, i++) {
			*cpyWchar = (WCHAR) *cpyUchar;
			cpyWchar++;
			cpyUchar++;
		}
	}
	return TRUE;
}

BOOL  EmployeeTime_02000100_02000300_NOCHANGE (ETK_INDEX *pParav200, ETK_INDEX *pParav203){
	memcpy (pParav203, pParav200, sizeof(ETK_INDEX));
	return TRUE;
}

ConversionFunctionListItem EmployeeTimeConversionList[] = {
	{ FILE_HEAD_VER_GP30, 0, EmployeeTimeRead_01040000, EmployeeTime_01040000_02000000 },
	{ FILE_HEAD_VER_GP40, 0, EmployeeTimeRead_02000100, EmployeeTime_02000100_02000300_NOCHANGE },
	{ FILE_HEAD_VER_GP40, 0, 0, 0 },
	{ FILE_HEAD_VER_GP41, 0, EmployeeTimeRead_02000100, EmployeeTime_02000100_02000300_NOCHANGE },
	{ FILE_HEAD_VER_GP41, 0, 0, 0 },
	{ FILE_HEAD_VER_GP42, 0, EmployeeTimeRead_02000100, EmployeeTime_02000100_02000300_NOCHANGE },
	{ FILE_HEAD_VER_GP42, 0, 0, 0 },
	{ FILE_HEAD_VER_GP201, 0, EmployeeTimeRead_02000100, EmployeeTime_02000100_02000300_NOCHANGE },
	{ FILE_HEAD_VER_GP201, 0, 0, 0 },
	{ FILE_HEAD_VER_GP203, 0, EmployeeTimeRead_02000100, EmployeeTime_02000100_02000300_NOCHANGE },
	{ FILE_HEAD_VER_GP203, 0, 0, 0 },
	{ FILE_HEAD_VER_GP204, 0, EmployeeTimeRead_02000100, EmployeeTime_02000100_02000300_NOCHANGE },
	{ FILE_HEAD_VER_GP204, 0, 0, 0 },
	{ FILE_HEAD_VER_GP205, 0, EmployeeTimeRead_02000100, EmployeeTime_02000100_02000300_NOCHANGE },
	{ FILE_HEAD_VER_GP205, 0, 0, 0 },
	{ FILE_HEAD_VER_GP210, 0, EmployeeTimeRead_02000100, EmployeeTime_02000100_02000300_NOCHANGE },
	{ FILE_HEAD_VER_GP210, 0, 0, 0 },
	{ FILE_HEAD_VER_GP212, 0, EmployeeTimeRead_02000100, EmployeeTime_02000100_02000300_NOCHANGE },
	{ FILE_HEAD_VER_GP212, 0, 0, 0 },
	{ FILE_HEAD_VER_GP220, 0, EmployeeTimeRead_02000100, EmployeeTime_02000100_02000300_NOCHANGE },
	{ FILE_HEAD_VER_GP220, 0, 0, 0 }
};

BOOL FileToEmployeeTimeFile(HANDLE iFile, LONG lOffset, LONG lSize, WCHAR* pszFileName, int iStartConvert)
{
    ETK_FILEHEAD    FileHead;
	UCHAR           *ucParaBufRay[2], ucParaBuf[1024];
    BOOL            bReturn;
    USHORT          usEtkNo;
    SHORT			sNewETKFile;
	DWORD		    bytesRead, fileError;
	int             j;

	ucParaBufRay[0] = &ucParaBuf[0];
	ucParaBufRay[1] = &ucParaBuf[sizeof(ucParaBuf)/2];

    /*----- R3.0 -----*/

    if (lSize == 0L) {
       return(TRUE);
    }

    /* seek file pointer    */
    fileError = SetFilePointer(iFile, lOffset, NULL, FILE_BEGIN);
	//fileError = GetLastError();
	if (fileError == -1) {
        return FALSE;
    }

    /* read header file to memory   */
    ReadFile(iFile, &FileHead, sizeof(FileHead), &bytesRead, NULL);
	if ( bytesRead != sizeof(FileHead)) {
        return(FALSE);
    }

	//create the ETK file to be written into
    sNewETKFile = PifOpenFile (pszFileName, (CHAR *)auchNEW_FILE_WRITE);
    if (sNewETKFile == PIF_ERROR_FILE_EXIST) {
        sNewETKFile = PifOpenFile (pszFileName, (CHAR *)auchOLD_FILE_READ_WRITE);
        if (sNewETKFile < 0) {
            return(OP_FILE_NOT_FOUND); /* File Broken */
        }
    }

	// look up in the conversion table until we find which file version we
	// are converting from.  We will then begin with that table entry to do
	// the conversion sequence.
	for (j = 0; j < sizeof(EmployeeTimeConversionList)/sizeof(EmployeeTimeConversionList[0]); j++) {
		if (EmployeeTimeConversionList[j].nFileHeadVer == iStartConvert) {
			break;
		}
	}

	bReturn = TRUE;

	if (j < sizeof(EmployeeTimeConversionList)/sizeof(EmployeeTimeConversionList[0])) {
	
		iStartConvert = j;

		//write the old file header to the new file, no change has been made
		//in this structure from R10 to R20 JHHJ
		PifWriteFile(sNewETKFile, &FileHead, sizeof(FileHead));

		/* file to file */
		for (usEtkNo = 1; usEtkNo <= FileHead.usNumberOfEtk; usEtkNo++) {
			int jk1 = 0, jk2 = 1, j = 0;
			j = iStartConvert;
			EmployeeTimeConversionList[j].ItemRead (iFile, 0, 0, 0, ucParaBufRay[jk1]);
			for ( ; EmployeeTimeConversionList[j].ItemConvert; j++) {
				EmployeeTimeConversionList[j].ItemConvert (ucParaBufRay[jk1], ucParaBufRay[jk2]);
				jk1 = 1 - jk1;
				jk2 = 1 - jk2;
			}
			PifWriteFile(sNewETKFile, ucParaBufRay[jk1], sizeof(ETK_INDEX));
		}
	}

	PifCloseFile(sNewETKFile);
    return(bReturn);
}
//----------------   End ETK File Conversion     ------------------------


//----------------   Begin Cashier File Conversion     ------------------------
//
//    Functions that read a particular release Cashier file

BOOL	CashierRead_02000300_02010000(HANDLE iFile, LONG lOffset, CAS_INDEXENTRY_HGV210 *pHead,VOID *pPara, ULONG size){
	// We position the read pointer within the PEP data file to the beginning
	// of the Cashier File data area.  The first part of the Cashier File that is
	// contained in the PEP data file is the Cashier File Index.  We are now positioning
	// to the beginning of the Cashier File cashier data records.

	return GenFileReadAtOffset(iFile, lOffset + (size * (pHead->usParaBlock - 1)), pPara, size);
}

BOOL  CashierRead_02000300 (HANDLE iFile, LONG lOffset, CAS_INDEXENTRY_HGV210 *pHead, int iItem, CAS_PARAENTRY_HGV203 *pParav203){
	return CashierRead_02000300_02010000(iFile,lOffset,pHead,pParav203,sizeof(CAS_PARAENTRY_HGV203));
}

BOOL  CashierRead_02010000 (HANDLE iFile, LONG lOffset, CAS_INDEXENTRY_HGV210 *pHead, int iItem, CAS_PARAENTRY_HGV210 *pParav210){
	return CashierRead_02000300_02010000(iFile,lOffset,pHead,pParav210,sizeof(CAS_PARAENTRY_HGV210));
}

BOOL  CashierRead_02010200 (HANDLE iFile, LONG lOffset, CAS_INDEXENTRY_HGV210 *pHead, int iItem, CAS_PARAENTRY_HGV212 *pParav212){
	LONG newOffset = lOffset + (sizeof(CAS_PARAENTRY_HGV212) * (pHead->usParaBlock -1));

	return GenFileReadAtOffset(iFile, newOffset, pParav212, sizeof(CAS_PARAENTRY_HGV212));
}

BOOL  CashierRead_02020000 (HANDLE iFile, LONG lOffset, CAS_INDEXENTRY_HGV220 *pHead, int iItem, CAS_PARAENTRY_HGV220 *pParav220){
	LONG newOffset = lOffset + (sizeof(CAS_PARAENTRY_HGV220) * (pHead->usParaBlock -1));

	return GenFileReadAtOffset(iFile, newOffset, pParav220, sizeof(CAS_PARAENTRY_HGV220));
}

//    Conversion functions that convert the data from one release to the next
BOOL  Cashier_02000000_02000300 (CAS_PARAENTRY_HGV203 *pParav200, CAS_PARAENTRY_HGV203 *pParav203){
	memcpy (pParav200, pParav203, sizeof(CAS_PARAENTRY_HGV203));
	return TRUE;
}

BOOL  Cashier_02000300_02010000 (CAS_PARAENTRY_HGV203 *pParav203, CAS_PARAENTRY_HGV210 *pParav210){
	memset(pParav210, 0x00, sizeof(CAS_PARAENTRY_HGV210));

	memcpy(pParav210->auchCashierName, pParav203->auchCashierName, sizeof(pParav203->auchCashierName));
	memcpy(pParav210->fbCashierStatus, pParav203->fbCashierStatus, sizeof(pParav203->fbCashierStatus));	// <-- the change was here

	pParav210->uchCashierSecret = pParav203->uchCashierSecret;
	pParav210->uchTeamNo = pParav203->uchTeamNo;
	pParav210->uchTerminal = pParav203->uchTerminal;
	pParav210->usGstCheckEndNo = pParav203->usGstCheckEndNo;
	pParav210->usGstCheckStartNo = pParav203->usGstCheckStartNo;
	pParav210->uchChgTipRate = pParav203->uchChgTipRate;

	return TRUE;
}

BOOL  Cashier_02010000_02010200 (CAS_PARAENTRY_HGV210 *pParav210, CAS_PARAENTRY_HGV212 *pParav212){
	memset(pParav212, 0x00, sizeof(CAS_PARAENTRY_HGV212));

	memcpy(pParav212->auchCashierName, pParav210->auchCashierName, sizeof(pParav210->auchCashierName));
	memcpy(pParav212->fbCashierStatus, pParav210->fbCashierStatus, sizeof(pParav210->fbCashierStatus));

	pParav212->ulCashierSecret = pParav210->uchCashierSecret;	// <-- the change was here

	pParav212->uchTeamNo = pParav210->uchTeamNo;
	pParav212->uchTerminal = pParav210->uchTerminal;
	pParav212->usGstCheckEndNo = pParav210->usGstCheckEndNo;
	pParav212->usGstCheckStartNo = pParav210->usGstCheckStartNo;
	pParav212->uchChgTipRate = pParav210->uchChgTipRate;

	return TRUE;
}

BOOL  Cashier_02010200_02020000 (CAS_PARAENTRY_HGV212 *pParav212, CAS_PARAENTRY *pParav220){
	memset(pParav220, 0x00, sizeof(CAS_PARAENTRY));

	memcpy(pParav220->auchCashierName, pParav212->auchCashierName, sizeof(pParav212->auchCashierName));
	memcpy(pParav220->fbCashierStatus, pParav212->fbCashierStatus, sizeof(pParav212->fbCashierStatus));		//the size was increased from 3 to 6

	pParav220->ulCashierSecret	= pParav212->ulCashierSecret;
	pParav220->uchTeamNo		= pParav212->uchTeamNo;
	pParav220->uchTerminal		= pParav212->uchTerminal;
	pParav220->usGstCheckEndNo	= pParav212->usGstCheckEndNo;
	pParav220->usGstCheckStartNo= pParav212->usGstCheckStartNo;
	pParav220->uchChgTipRate	= pParav212->uchChgTipRate;

	pParav220->usSupervisorID		= 0;	//new values in this release
	pParav220->usCtrlStrKickoff		= 0;
	pParav220->usStartupWindow		= 0;
	pParav220->ulGroupAssociations	= 0;

	return TRUE;
}

BOOL  Cashier_02020000_NOCHANGE (CAS_PARAENTRY_HGV220 *pParav220, CAS_PARAENTRY *pParav221){
	memcpy(pParav221, pParav220, sizeof(CAS_PARAENTRY_HGV220));

	return TRUE;
}
ConversionFunctionListItem CashierConversionList[] = {
	{ FILE_HEAD_VER_GP40, 0, CashierRead_02000300, Cashier_02000300_02010000 },
	{ FILE_HEAD_VER_GP40, 0, CashierRead_02000300, Cashier_02010000_02010200 },
	{ FILE_HEAD_VER_GP40, 0, CashierRead_02000300, Cashier_02010200_02020000 },
	{ FILE_HEAD_VER_GP40, 0, 0, 0 },
	{ FILE_HEAD_VER_GP41, 0, CashierRead_02000300, Cashier_02000300_02010000 },
	{ FILE_HEAD_VER_GP41, 0, CashierRead_02000300, Cashier_02010000_02010200 },
	{ FILE_HEAD_VER_GP41, 0, CashierRead_02000300, Cashier_02010200_02020000 },
	{ FILE_HEAD_VER_GP41, 0, 0, 0 },
	{ FILE_HEAD_VER_GP42, 0, CashierRead_02000300, Cashier_02000300_02010000 },
	{ FILE_HEAD_VER_GP42, 0, CashierRead_02000300, Cashier_02010000_02010200 },
	{ FILE_HEAD_VER_GP42, 0, CashierRead_02000300, Cashier_02010200_02020000 },
	{ FILE_HEAD_VER_GP42, 0, 0, 0 },
	{ FILE_HEAD_VER_GP201, 0, CashierRead_02000300, Cashier_02000300_02010000 },
	{ FILE_HEAD_VER_GP201, 0, CashierRead_02000300, Cashier_02010000_02010200 },
	{ FILE_HEAD_VER_GP201, 0, CashierRead_02000300, Cashier_02010200_02020000 },
	{ FILE_HEAD_VER_GP201, 0, 0, 0 },
	{ FILE_HEAD_VER_GP203, 0, CashierRead_02000300, Cashier_02000300_02010000 },
	{ FILE_HEAD_VER_GP203, 0, CashierRead_02000300, Cashier_02010000_02010200 },
	{ FILE_HEAD_VER_GP203, 0, CashierRead_02000300, Cashier_02010200_02020000 },
	{ FILE_HEAD_VER_GP203, 0, 0, 0 },
	{ FILE_HEAD_VER_GP204, 0, CashierRead_02000300, Cashier_02000300_02010000 },
	{ FILE_HEAD_VER_GP204, 0, CashierRead_02000300, Cashier_02010000_02010200 },
	{ FILE_HEAD_VER_GP204, 0, CashierRead_02000300, Cashier_02010200_02020000 },
	{ FILE_HEAD_VER_GP204, 0, 0, 0 },
	{ FILE_HEAD_VER_GP205, 0, CashierRead_02000300, Cashier_02000300_02010000 },
	{ FILE_HEAD_VER_GP205, 0, CashierRead_02000300, Cashier_02010000_02010200 },
	{ FILE_HEAD_VER_GP205, 0, CashierRead_02000300, Cashier_02010200_02020000 },
	{ FILE_HEAD_VER_GP205, 0, 0, 0 },
	{ FILE_HEAD_VER_GP210, 0, CashierRead_02010000, Cashier_02010000_02010200 },
	{ FILE_HEAD_VER_GP210, 0, CashierRead_02010000, Cashier_02010200_02020000 },
	{ FILE_HEAD_VER_GP210, 0, 0, 0 },
	{ FILE_HEAD_VER_GP212, 0, CashierRead_02010200, Cashier_02010200_02020000 },
	{ FILE_HEAD_VER_GP212, 0, 0, 0 },
	{ FILE_HEAD_VER_GP220, 0, CashierRead_02020000, Cashier_02020000_NOCHANGE },
	{ FILE_HEAD_VER_GP220, 0, 0, 0 }
};

// There are a couple of things you should remember about the environment of thse functions.
//
// First of all, the file that iFile is the pointer to is the PEP file that is being divided.
//  The lOffset points to the beginning of the Cashier File portion of the PEP data file.
//  The Cashier File portion of the PEP data file has the same format as the Cashier File
//  but seeks within the input file using handle iFile is done relative to lOffset and not from
//  the beginning of the file.

//	The CAS_INDEXENTRY changed between 210 and 212, so files 212 and beyond should be read with
//	an alternate read function
BOOL FileToCashierFile_210(HANDLE iFile, LONG lOffset, LONG lSize, WCHAR* pszFileName, int iStartConvert)
{
    CAS_FILEHED     FileHead, FileHeadNew;
	CAS_INDEXENTRY_HGV210 CashierIndexEntryIn;		//this will not work for versions after 2.1.0
	CAS_INDEXENTRY  CashierIndexEntryOut;
	CAS_PARAENTRY   *ucParaBufRay[2], ucParaBuf[4*sizeof(CAS_PARAENTRY)];
    BOOL            bReturn;
    USHORT			usCashierIndex;
	DWORD		    fileError;
	ULONG           ulIndexOffsetIn, ulIndexOffsetOut;
	int             j;

	ucParaBufRay[0] = &ucParaBuf[0];
	ucParaBufRay[1] = &ucParaBuf[2];

    /*----- R3.0 -----*/

    if (lSize == 0L) {
       return(TRUE);
    }

    /*
		Position the file pointer to the beginning of the PEP data file and
		then read in the PEP file header.  The header contains the P2 information
		with the item we are interested in the usNumberOfMaxCashier which indicates
		the number of cashiers provisioned for the Cashier File.
	 */

    if(!GenFileReadAtOffset(iFile,lOffset,&FileHead,sizeof(FileHead))){
    	fileError = GetLastError();	// This is here for debugging only in case of file errors
    	return FALSE;
    }

	// Create the Cashier file to be written into.  This is a separate file
	// from the PEP data file and is a normal Cashier File format for this release.
	// After creating the file, we open it using the Cashier File handle global
	// variable because the Cashier File manipulation primitives will be using
	// that variable for the file handle to the file.
	// Finally, we get the header of the new Cashier File for the various offsets.
	CasCreatFile(FileHead.usNumberOfMaxCashier);

	hsCashierFileHandle = PifOpenFile(auchCAS_CASHIER, auchOLD_FILE_READ_WRITE);

	Cas_GetHeader(&FileHeadNew);

	// look up in the conversion table until we find which file version we
	// are converting from.  We will then begin with that table entry to do
	// the conversion sequence.
	for (j = 0; j < sizeof(CashierConversionList)/sizeof(CashierConversionList[0]); j++) {
		if (CashierConversionList[j].nFileHeadVer == iStartConvert) {
			break;
		}
	}

	if (j >= sizeof(CashierConversionList)/sizeof(CashierConversionList[0])) {
		return FALSE;
	}
	iStartConvert = j;

    bReturn = TRUE;

    /*
		Convert the data in the Cashier File section of the PEP data file to the
		new Cashier File data record, put the new data into the new Cashier File
		and then update the index portion of the Cashier File.
		Once we are done transferring the Cashier records, we will write out the
		updated Cashier File header and then close the Cashier File.
	 */
	{
		UCHAR           uchBitMasks[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
		UCHAR           uchParaEmpBlk[CAS_PARA_EMPTY_TABLE_SIZE] = {0};    /* paramater empty table */

		ulIndexOffsetIn = FileHead.offusOffsetOfIndexTbl;
		ulIndexOffsetOut = FileHeadNew.offusOffsetOfIndexTbl;
		for (usCashierIndex = 1; usCashierIndex <= FileHead.usNumberOfResidentCashier; usCashierIndex++) {
			USHORT  iIndex = (usCashierIndex - 1) / 8;    // calculate offset into the blocks in-use flag table.
			USHORT  iBit = (usCashierIndex - 1) % 8;      // calculate offset for bit to be set to indicate block in-use.
			int jk1 = 0, jk2 = 1, j = 0;

			j = iStartConvert;

			GenFileReadAtOffset(iFile, lOffset + ulIndexOffsetIn, &CashierIndexEntryIn, sizeof(CashierIndexEntryIn));

			CashierConversionList[j].ItemRead (iFile, lOffset + FileHead.offusOffsetOfParaTbl, &CashierIndexEntryIn, 0, ucParaBufRay[jk1]);		
			for ( ; CashierConversionList[j].ItemConvert; j++) {
				CashierConversionList[j].ItemConvert (ucParaBufRay[jk1], ucParaBufRay[jk2]);
				jk1 = 1 - jk1;
				jk2 = 1 - jk2;
			}

			CashierIndexEntryOut.ulCashierNo = CashierIndexEntryIn.usCashierNo;		//conversion to long in 212
			CashierIndexEntryOut.usParaBlock =  usCashierIndex;
			uchParaEmpBlk[iIndex] |= uchBitMasks[iBit];                             // flag the block as in use.

			Cas_ParaWrite(&FileHeadNew, CashierIndexEntryOut.usParaBlock, ucParaBufRay[jk1]);
			Cas_ChangeFlag(CashierIndexEntryOut.usParaBlock, CAS_RESET_STATUS, 0xff);
			FileHeadNew.usNumberOfResidentCashier++;
			Cas_WriteFile(ulIndexOffsetOut, &CashierIndexEntryOut, sizeof(CashierIndexEntryOut));
			
			ulIndexOffsetIn  += sizeof(CashierIndexEntryIn);
			ulIndexOffsetOut += sizeof(CashierIndexEntryOut);
		}

		// write out the in-use block table which is needed to know what Cashier Parameter Blocks are in use.
		Cas_WriteFile(FileHeadNew.offusOffsetOfParaEmpTbl, uchParaEmpBlk, CAS_PARA_EMPTY_TABLE_SIZE);
	}

	Cas_PutHeader(&FileHeadNew);
    PifCloseFile(hsCashierFileHandle);

    return(bReturn);
}

//new cashier file read function here
BOOL FileToCashierFile(HANDLE iFile, LONG lOffset, LONG lSize, WCHAR* pszFileName, int iStartConvert)
{
    CAS_FILEHED     FileHead, FileHeadNew;
	union {
		CAS_INDEXENTRY_HGV210 EntryHGV210;		//this will not work for versions after 2.1.0
		CAS_INDEXENTRY_HGV220 EntryHGV220;
	} CashierIndexEntryIn;
	CAS_INDEXENTRY  CashierIndexEntryOut;
	CAS_PARAENTRY   *ucParaBufRay[2], ucParaBuf[4*sizeof(CAS_PARAENTRY)];
    BOOL            bReturn;
    USHORT			usCashierIndex;
	DWORD		    fileError;
	ULONG           ulIndexOffsetIn, ulIndexOffsetOut;
	int             j;

	ucParaBufRay[0] = &ucParaBuf[0];
	ucParaBufRay[1] = &ucParaBuf[2];

    /*----- R3.0 -----*/

    if (lSize == 0L) {
       return(TRUE);
    }

    /*
		Position the file pointer to the beginning of the PEP data file and
		then read in the PEP file header.  The header contains the P2 information
		with the item we are interested in the usNumberOfMaxCashier which indicates
		the number of cashiers provisioned for the Cashier File.
	 */

    if(!GenFileReadAtOffset(iFile,lOffset,&FileHead,sizeof(FileHead))){
    	fileError = GetLastError();	// This is here for debugging only in case of file errors
    	return FALSE;
    }

	// Create the Cashier file to be written into.  This is a separate file
	// from the PEP data file and is a normal Cashier File format for this release.
	// After creating the file, we open it using the Cashier File handle global
	// variable because the Cashier File manipulation primitives will be using
	// that variable for the file handle to the file.
	// Finally, we get the header of the new Cashier File for the various offsets.
	CasCreatFile(FileHead.usNumberOfMaxCashier);

	hsCashierFileHandle = PifOpenFile(auchCAS_CASHIER, auchOLD_FILE_READ_WRITE);

	Cas_GetHeader(&FileHeadNew);

	// look up in the conversion table until we find which file version we
	// are converting from.  We will then begin with that table entry to do
	// the conversion sequence.
	for (j = 0; j < sizeof(CashierConversionList)/sizeof(CashierConversionList[0]); j++) {
		if (CashierConversionList[j].nFileHeadVer == iStartConvert) {
			break;
		}
	}

	if (j >= sizeof(CashierConversionList)/sizeof(CashierConversionList[0])) {
		return FALSE;
	}
	iStartConvert = j;

    bReturn = TRUE;

    /*
		Convert the data in the Cashier File section of the PEP data file to the
		new Cashier File data record, put the new data into the new Cashier File
		and then update the index portion of the Cashier File.
		Once we are done transferring the Cashier records, we will write out the
		updated Cashier File header and then close the Cashier File.
	 */
	{
		UCHAR           uchBitMasks[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
		UCHAR           uchParaEmpBlk[CAS_PARA_EMPTY_TABLE_SIZE] = {0};    /* paramater empty table */

		ulIndexOffsetIn = FileHead.offusOffsetOfIndexTbl;
		ulIndexOffsetOut = FileHeadNew.offusOffsetOfIndexTbl;
		for (usCashierIndex = 1; usCashierIndex <= FileHead.usNumberOfResidentCashier; usCashierIndex++) {
			USHORT  iIndex = (usCashierIndex - 1) / 8;    // calculate offset into the blocks in-use flag table.
			USHORT  iBit = (usCashierIndex - 1) % 8;      // calculate offset for bit to be set to indicate block in-use.
			int jk1 = 0, jk2 = 1, j = 0;
			j = iStartConvert;

			if (CashierConversionList[iStartConvert].nFileHeadVer == FILE_HEAD_VER_GP220) {
				GenFileReadAtOffset(iFile, lOffset + ulIndexOffsetIn, &CashierIndexEntryIn, sizeof(CAS_INDEXENTRY_HGV220));
			} else {
				GenFileReadAtOffset(iFile, lOffset + ulIndexOffsetIn, &CashierIndexEntryIn, sizeof(CAS_INDEXENTRY_HGV210));
			}

			CashierConversionList[j].ItemRead (iFile, lOffset + FileHead.offusOffsetOfParaTbl, &CashierIndexEntryIn, 0, ucParaBufRay[jk1]);		
			for ( ; CashierConversionList[j].ItemConvert; j++) {
				CashierConversionList[j].ItemConvert (ucParaBufRay[jk1], ucParaBufRay[jk2]);
				jk1 = 1 - jk1;
				jk2 = 1 - jk2;
			}

			if (CashierConversionList[iStartConvert].nFileHeadVer == FILE_HEAD_VER_GP220) {
				CashierIndexEntryOut.ulCashierNo = CashierIndexEntryIn.EntryHGV220.ulCashierNo;		//conversion to long in 212
				CashierIndexEntryOut.usParaBlock = usCashierIndex;
				uchParaEmpBlk[iIndex] |= uchBitMasks[iBit];                             // flag the block as in use.

				Cas_ParaWrite(&FileHeadNew, CashierIndexEntryOut.usParaBlock, ucParaBufRay[jk1]);
				Cas_ChangeFlag(CashierIndexEntryOut.usParaBlock, CAS_RESET_STATUS, 0xff);
				Cas_WriteFile(ulIndexOffsetOut, &CashierIndexEntryOut, sizeof(CashierIndexEntryOut));
				
				ulIndexOffsetIn  += sizeof(CashierIndexEntryIn.EntryHGV220);
				ulIndexOffsetOut += sizeof(CashierIndexEntryOut);
			} else {
				CashierIndexEntryOut.ulCashierNo = CashierIndexEntryIn.EntryHGV210.usCashierNo;		//conversion to long in 212
				CashierIndexEntryOut.usParaBlock = usCashierIndex;
				uchParaEmpBlk[iIndex] |= uchBitMasks[iBit];                             // flag the block as in use.

				Cas_ParaWrite(&FileHeadNew, CashierIndexEntryOut.usParaBlock, ucParaBufRay[jk1]);
				Cas_ChangeFlag(CashierIndexEntryOut.usParaBlock, CAS_RESET_STATUS, 0xff);
				Cas_WriteFile(ulIndexOffsetOut, &CashierIndexEntryOut, sizeof(CashierIndexEntryOut));
				
				ulIndexOffsetIn  += sizeof(CashierIndexEntryIn.EntryHGV210);
				ulIndexOffsetOut += sizeof(CashierIndexEntryOut);
			}
			
			FileHeadNew.usNumberOfResidentCashier++;
		}

		// write out the in-use block table which is needed to know what Cashier Parameter Blocks are in use.
		Cas_WriteFile(FileHeadNew.offusOffsetOfParaEmpTbl, uchParaEmpBlk, CAS_PARA_EMPTY_TABLE_SIZE);
	}

	Cas_PutHeader(&FileHeadNew);
    PifCloseFile(hsCashierFileHandle);

    return(bReturn);
}


//----------------   End Cashier File Conversion     ------------------------


//----------------   Begin Department File Conversion     ------------------------
//
//    Functions that read a particular release ETK file
BOOL  DepartmentRead_01040000 (HANDLE iFile, LONG lOffset, OPDEPT_FILEHED *pHead, int iItem, DEPTIF_HGV10 *pParav140){
	return GenFileRead(iFile,pParav140,sizeof(DEPTIF_HGV10));
}

BOOL  DepartmentRead_02000100 (HANDLE iFile, LONG lOffset, OPDEPT_FILEHED *pHead, int iItem, DEPTIF_HGV201 *pParav201){
	OPDEPT_INDEXENTRY	DeptIndex;
	ULONG               myOffset;
	USHORT			    usParaBlkNo;

    memset(pParav201, 0x00, sizeof(DEPTIF_HGV201));

	/*  get Dept No in Dept Index */
	myOffset = pHead->offulOfIndexTbl + lOffset + ((iItem - 1) * sizeof(OPDEPT_INDEXENTRY));
	GenFileReadAtOffset(iFile,myOffset,&DeptIndex,sizeof(DeptIndex));
	pParav201->usDeptNo = DeptIndex.usDeptNo;

	/*  get Dept offset in paramater block no */
	myOffset = pHead->offulOfIndexTblBlk + lOffset + ((iItem - 1) * OP_DEPT_INDEXBLK_SIZE);
	GenFileReadAtOffset(iFile,myOffset,&usParaBlkNo, OP_DEPT_INDEXBLK_SIZE);
	
	/*  get plu paramater in paramater table */
	myOffset = pHead->offulOfParaTbl + lOffset + ((ULONG)(usParaBlkNo - 1) * sizeof(OPDEPT_PARAENTRY_HGV201));
	return GenFileReadAtOffset(iFile,myOffset,&(pParav201->ParaDept),sizeof(OPDEPT_PARAENTRY_HGV201));
}

BOOL  DepartmentRead_02010000 (HANDLE iFile, LONG lOffset, OPDEPT_FILEHED *pHead, int iItem, DEPTIF *pParav210){
	OPDEPT_INDEXENTRY	DeptIndex;
	ULONG               myOffset;
	USHORT			    usParaBlkNo;

    memset(pParav210, 0x00, sizeof(DEPTIF));

	/*  get Dept No in Dept Index */
	myOffset = pHead->offulOfIndexTbl + lOffset + ((iItem - 1) * sizeof(OPDEPT_INDEXENTRY));
	GenFileReadAtOffset(iFile,myOffset,&DeptIndex,sizeof(DeptIndex));
	pParav210->usDeptNo = DeptIndex.usDeptNo;

	/*  get Dept offset in paramater block no */
	myOffset = pHead->offulOfIndexTblBlk + lOffset + ((iItem - 1) * OP_DEPT_INDEXBLK_SIZE);
	GenFileReadAtOffset(iFile,myOffset,&usParaBlkNo, OP_DEPT_INDEXBLK_SIZE);
	
	/*  get plu paramater in paramater table */
	myOffset = pHead->offulOfParaTbl + lOffset + ((ULONG)(usParaBlkNo - 1) * sizeof(OPDEPT_PARAENTRY));
	return GenFileReadAtOffset(iFile,myOffset,&(pParav210->ParaDept),sizeof(OPDEPT_PARAENTRY));
}

//    Conversion functions that convert the data from one release to the next.
BOOL  Department_01040000_02000000 (DEPTIF_HGV10 *pParav140, DEPTIF_HGV201 *pParav200){
	int i;

	memset (pParav200, 0, sizeof(DEPTIF_HGV201));

    pParav200->ParaDept.uchMdept = pParav140->ParaDept.uchMdept;

    memcpy (pParav200->ParaDept.auchControlCode, pParav140->ParaDept.auchControlCode, sizeof(pParav140->ParaDept.auchControlCode));

    memcpy (pParav200->ParaDept.auchHalo, pParav140->ParaDept.auchHalo, sizeof(pParav140->ParaDept.auchHalo));

	for (i = 0; i < OP_DEPT_NAME_SIZE; i++) {
		pParav200->ParaDept.auchMnemonic[i] = pParav140->ParaDept.auchMnemonic[i];
	}

    pParav200->ParaDept.uchPrintPriority = pParav140->ParaDept.uchPrintPriority;

	return TRUE;
}

BOOL  Department_02000100_02000300_NOCHANGE (DEPTIF_HGV201 *pParav200, DEPTIF_HGV201 *pParav203){
	memcpy (pParav203, pParav200, sizeof(DEPTIF_HGV201));
	return TRUE;
}

BOOL  Department_02000300_02010000 (DEPTIF_HGV201 *pParav203, DEPTIF *pParav210){
	UCHAR  uchBonus;

	memset (pParav210, 0, sizeof(DEPTIF));

	memcpy(pParav210->ParaDept.auchControlCode ,pParav203->ParaDept.auchControlCode, sizeof(pParav203->ParaDept.auchControlCode));
	memcpy(pParav210->ParaDept.auchHalo, pParav203->ParaDept.auchHalo, sizeof(pParav203->ParaDept.auchHalo));

	pParav210->ParaDept.uchMdept = pParav203->ParaDept.uchMdept;
	pParav210->ParaDept.uchPrintPriority = pParav203->ParaDept.uchPrintPriority;
	memcpy(pParav210->ParaDept.auchMnemonic, pParav203->ParaDept.auchMnemonic, sizeof(pParav203->ParaDept.auchMnemonic));
	if(pParav203->ParaDept.auchControlCode[3] & BONUS_MASK_DATA)
	{
		uchBonus = (pParav203->ParaDept.auchControlCode[3] & BONUS_MASK_DATA);
		pParav210->ParaDept.usBonusIndex |= uchBonus;
	}

	pParav210->usDeptNo = pParav203->usDeptNo;

	return TRUE;
}

BOOL  Department_02010000_02010200_NOCHANGE(DEPTIF *pParav210, DEPTIF *pParav212){
	memcpy(pParav212, pParav210, sizeof(DEPTIF));
	return TRUE;
}

ConversionFunctionListItem DepartmentConversionList[] = {
	{ FILE_HEAD_VER_GP30, 0, DepartmentRead_01040000, Department_01040000_02000000 },
	{ FILE_HEAD_VER_GP40, 0, DepartmentRead_02000100, Department_02000100_02000300_NOCHANGE },
	{ FILE_HEAD_VER_GP40, 0, 0, 0 },
	{ FILE_HEAD_VER_GP41, 0, DepartmentRead_02000100, Department_02000100_02000300_NOCHANGE },
	{ FILE_HEAD_VER_GP41, 0, 0, 0 },
	{ FILE_HEAD_VER_GP42, 0, DepartmentRead_02000100, Department_02000100_02000300_NOCHANGE },
	{ FILE_HEAD_VER_GP42, 0, 0, 0 },
	{ FILE_HEAD_VER_GP201, 0, DepartmentRead_02000100, Department_02000300_02010000 },
	{ FILE_HEAD_VER_GP201, 0, 0, 0 },
	{ FILE_HEAD_VER_GP203, 0, DepartmentRead_02000100, Department_02000300_02010000 },
	{ FILE_HEAD_VER_GP203, 0, 0, 0 },
	{ FILE_HEAD_VER_GP204, 0, DepartmentRead_02000100, Department_02000300_02010000 },
	{ FILE_HEAD_VER_GP204, 0, 0, 0 },
	{ FILE_HEAD_VER_GP205, 0, DepartmentRead_02000100, Department_02000300_02010000 },
	{ FILE_HEAD_VER_GP205, 0, 0, 0 },
	{ FILE_HEAD_VER_GP210, 0, DepartmentRead_02010000, Department_02010000_02010200_NOCHANGE },
	{ FILE_HEAD_VER_GP210, 0, 0, 0 },
	{ FILE_HEAD_VER_GP212, 0, DepartmentRead_02010000, Department_02010000_02010200_NOCHANGE },
	{ FILE_HEAD_VER_GP212, 0, 0, 0 },
	{ FILE_HEAD_VER_GP220, 0, DepartmentRead_02010000, Department_02010000_02010200_NOCHANGE },
	{ FILE_HEAD_VER_GP220, 0, 0, 0 }
};

BOOL FileToDepartmentFile(HANDLE iFile, LONG lOffset, LONG lSize, WCHAR* pszFileName, int iStartConvert)
{
    OPDEPT_FILEHED      FileHead;
	UCHAR               *ucParaBufRay[2], ucParaBuf[1024];
    BOOL                bReturn = FALSE;
    USHORT              usDeptNo;
	DWORD		        bytesRead, fileError;
	SHORT               sError;
	int                 j;

	ucParaBufRay[0] = &ucParaBuf[0];
	ucParaBufRay[1] = &ucParaBuf[sizeof(ucParaBuf)/2];

    /*----- R3.0 -----*/

    if (lSize == 0L) {
       return(TRUE);
    }

    /* seek file pointer    */
    fileError = SetFilePointer(iFile, lOffset, NULL, FILE_BEGIN);
	//fileError = GetLastError();
	if (fileError == -1/*NO_ERRORFheader.Mem.Para.lOffset*/) {
        return FALSE;
    }

    /* read header file to memory   */
    ReadFile(iFile, &FileHead, sizeof(FileHead), &bytesRead, NULL);
	if ( bytesRead != sizeof(FileHead)) {
        return(FALSE);
    }

	if(OpDeptCreatFile(FileHead.usNumberOfMaxDept, 0) == OP_PERFORM){
		// look up in the conversion table until we find which file version we
		// are converting from.  We will then begin with that table entry to do
		// the conversion sequence.
		for (j = 0; j < sizeof(DepartmentConversionList)/sizeof(DepartmentConversionList[0]); j++) {
			if (DepartmentConversionList[j].nFileHeadVer == iStartConvert) {
				break;
			}
		}

		if (j < sizeof(DepartmentConversionList)/sizeof(DepartmentConversionList[0])) {
			iStartConvert = j;

			bReturn = TRUE;

			/* file to file */
			for (usDeptNo = 1; usDeptNo <= FileHead.usNumberOfResidentDept; usDeptNo++) {
				int jk1 = 0, jk2 = 1, j = 0;
				j = iStartConvert;
				jk1 = 0; jk2 = 1;
				DepartmentConversionList[j].ItemRead (iFile, lOffset, &FileHead, usDeptNo, ucParaBufRay[jk1]);
				for ( ; DepartmentConversionList[j].ItemConvert; j++) {
					DepartmentConversionList[j].ItemConvert (ucParaBufRay[jk1], ucParaBufRay[jk2]);
					jk1 = 1 - jk1;
					jk2 = 1 - jk2;
				}
				sError = OpDeptAssign((DEPTIF *)ucParaBufRay[jk1], 0);
			}
		}
	}
    return(bReturn);
}
//----------------   End Department File Conversion     ------------------------



//----------------   Begin PPI File Conversion     ------------------------
//
//    Functions that read a particular release PPI file
BOOL  PpiRead_02000300 (HANDLE iFile, LONG lOffset, VOID *pHead, int iItem, OPPPI_PARAENTRY_V203 *pParav203){
	return GenFileRead(iFile,pParav203,sizeof(OPPPI_PARAENTRY_V203));
}

BOOL  PpiRead_02010000 (HANDLE iFile, LONG lOffset, VOID *pHead, int iItem, OPPPI_PARAENTRY *pParav210){
	return GenFileRead(iFile,pParav210,sizeof(OPPPI_PARAENTRY));
}

//    Conversion functions that convert the data from one release to the next.
BOOL  Ppi_02000300_02010000 (OPPPI_PARAENTRY_V203 *pParav203, OPPPI_PARAENTRY *pParav210){
	memset (pParav210, 0, sizeof(OPPPI_PARAENTRY));
	memcpy (pParav210, pParav203, sizeof(OPPPI_PARAENTRY_V203));
	return TRUE;
}

BOOL  Ppi_02010000_02010200_NOCHANGE (OPPPI_PARAENTRY *pParav210, OPPPI_PARAENTRY *pParav212){
	memcpy (pParav212, pParav210, sizeof(OPPPI_PARAENTRY));
	return TRUE;
}

ConversionFunctionListItem PpiConversionList[] = {
	{ FILE_HEAD_VER_GP201, 0, PpiRead_02000300, Ppi_02000300_02010000 },
	{ FILE_HEAD_VER_GP201, 0, 0, 0 },
	{ FILE_HEAD_VER_GP203, 0, PpiRead_02000300, Ppi_02000300_02010000 },
	{ FILE_HEAD_VER_GP203, 0, 0, 0 },
	{ FILE_HEAD_VER_GP204, 0, PpiRead_02000300, Ppi_02000300_02010000 },
	{ FILE_HEAD_VER_GP204, 0, 0, 0 },
	{ FILE_HEAD_VER_GP205, 0, PpiRead_02000300, Ppi_02000300_02010000 },
	{ FILE_HEAD_VER_GP205, 0, 0, 0 }, 
	{ FILE_HEAD_VER_GP210, 0, PpiRead_02010000, Ppi_02010000_02010200_NOCHANGE },
	{ FILE_HEAD_VER_GP210, 0, 0, 0 },
	{ FILE_HEAD_VER_GP212, 0, PpiRead_02010000, Ppi_02010000_02010200_NOCHANGE },
	{ FILE_HEAD_VER_GP212, 0, 0, 0 },
	{ FILE_HEAD_VER_GP220, 0, PpiRead_02010000, Ppi_02010000_02010200_NOCHANGE },
	{ FILE_HEAD_VER_GP220, 0, 0, 0 }
};


BOOL FileToPpiFile(HANDLE iFile, LONG lOffset, LONG lSize, WCHAR* pszFileName, int iStartConvert)
{
    PPIIF               InParaData;
    OPPPI_FILEHED       FileHead;
	UCHAR               *ucParaBufRay[2], ucParaBuf[1024];
    BOOL                bReturn = FALSE;
    USHORT              usPpiCode;
	SHORT               sError;
	int                 j;

	ucParaBufRay[0] = &ucParaBuf[0];
	ucParaBufRay[1] = &ucParaBuf[sizeof(ucParaBuf)/2];

    /*----- R3.0 -----*/

    if (lSize == 0L) {
       return TRUE;
    }

	if(!GenFileReadAtOffset(iFile,lOffset,&FileHead,sizeof(FileHead))){
		return bReturn;
	}
	
	if(OpPpiCreatFile(FileHead.usNumberOfPpi, 0) == OP_PERFORM){
		// look up in the conversion table until we find which file version we
		// are converting from.  We will then begin with that table entry to do
		// the conversion sequence.
		for (j = 0; j < sizeof(PpiConversionList)/sizeof(PpiConversionList[0]); j++) {
			if (PpiConversionList[j].nFileHeadVer == iStartConvert) {
				break;
			}
		}
		if (j < sizeof(PpiConversionList)/sizeof(PpiConversionList[0])) {
			bReturn = TRUE;

			iStartConvert = j;

			/* file to file */
			for (usPpiCode = 1; usPpiCode <= FileHead.usNumberOfPpi; usPpiCode++) {
				int jk1 = 0, jk2 = 1, j = 0;
				j = iStartConvert;
				jk1 = 0; jk2 = 1;
				PpiConversionList[j].ItemRead (iFile, lOffset, &FileHead, usPpiCode, ucParaBufRay[jk1]);
				for ( ; PpiConversionList[j].ItemConvert; j++) {
					PpiConversionList[j].ItemConvert (ucParaBufRay[jk1], ucParaBufRay[jk2]);
					jk1 = 1 - jk1;
					jk2 = 1 - jk2;
				}
				memset (&InParaData, 0, sizeof(InParaData));
				InParaData.uchPpiCode = usPpiCode;
				memcpy(InParaData.ParaPpi.PpiRec, ucParaBufRay[jk1], sizeof(InParaData.ParaPpi.PpiRec));
				sError = OpPpiAssign(&InParaData, 0);
			}
		}
	}
    return bReturn;
}
//----------------   End PPI File Conversion     ------------------------


//----------------   Begin PLU File Conversion     ------------------------
//
//    Functions that read a particular release ETK file

BOOL  PluRead_02010000 (HANDLE iFile, LONG lOffset, OPDEPT_FILEHED *pHead, int iItem, DEPTIF *pParav210){
	OPDEPT_INDEXENTRY	DeptIndex;
	ULONG               myOffset;
	USHORT			    usParaBlkNo;

    memset(pParav210, 0x00, sizeof(DEPTIF));

	/*  get Dept No in Dept Index */
	myOffset = pHead->offulOfIndexTbl + lOffset + ((iItem - 1) * sizeof(OPDEPT_INDEXENTRY));
	GenFileReadAtOffset(iFile,myOffset,&DeptIndex,sizeof(DeptIndex));
	pParav210->usDeptNo = DeptIndex.usDeptNo;

	/*  get Dept offset in paramater block no */
	myOffset = pHead->offulOfIndexTblBlk + lOffset + ((iItem - 1) * OP_DEPT_INDEXBLK_SIZE);
	GenFileReadAtOffset(iFile,myOffset,&usParaBlkNo, OP_DEPT_INDEXBLK_SIZE);
	
	/*  get plu paramater in paramater table */
	myOffset = pHead->offulOfParaTbl + lOffset + ((ULONG)(usParaBlkNo - 1) * sizeof(OPDEPT_PARAENTRY));
	return GenFileReadAtOffset(iFile,myOffset,&(pParav210->ParaDept),sizeof(OPDEPT_PARAENTRY));
}

//    Conversion functions that convert the data from one release to the next.


BOOL  Plu_02000100_02000300_NOCHANGE (DEPTIF_HGV201 *pParav200, DEPTIF_HGV201 *pParav203){
	memcpy (pParav203, pParav200, sizeof(DEPTIF_HGV201));
	return TRUE;
}

BOOL  Plu_02010000_02020100 (RECPLU210 *pParav210, RECPLU *pParav221){
	int i;

	memset(pParav221, 0x00, sizeof(RECPLU));

	memcpy(pParav221->aucPluNumber, &pParav210->aucPluNumber, sizeof(pParav221->aucPluNumber));
	pParav221->uchAdjective               = 1; /* adjectve is always 1 on PLU record */
	pParav221->usDeptNumber               = pParav210->usDeptNumber;
	memcpy(pParav221->ulUnitPrice, pParav210->ulUnitPrice, sizeof(pParav210->ulUnitPrice));
	pParav221->uchPriceMulWeight          = pParav210->uchPriceMulWeight;
	for (i = 0; i < PLU_MNEMONIC_LEN; i++) {
		pParav221->aucMnemonic[i] = pParav210->aucMnemonic[i];
	}

	pParav221->usFamilyCode		         = pParav210->usFamilyCode;	/* ### ADD Saratoga (V1_0.02)(051800) */
	pParav221->uchMixMatchTare			= pParav210->uchMixMatchTare;
	pParav221->uchReportCode			 = pParav210->uchReportCode;
	pParav221->usLinkNumber		         = pParav210->usLinkNumber;
	pParav221->uchRestrict		         = pParav210->uchRestrict;
	pParav221->uchLinkPPI	             = (USHORT)pParav210->uchLinkPPI;
	pParav221->uchTableNumber	         = pParav210->uchTableNumber;
	pParav221->uchGroupNumber			 = pParav210->uchGroupNumber;
	pParav221->uchPrintPriority		     = pParav210->uchPrintPriority;
	pParav221->uchItemType				 = pParav210->uchItemType;
	pParav221->usBonusIndex            = pParav210->usBonusIndex;
	pParav221->ulVoidTotalCounter      = pParav210->ulVoidTotalCounter;				// Void Total/Counter for each PLU
	pParav221->ulDiscountTotalCounter  = pParav210->ulDiscountTotalCounter;			// Discount Total/Counter for each PLU
	pParav221->ulFreqShopperPoints     = pParav210->ulFreqShopperPoints;			// Frequent Shopper Discount/Points (GP Feature)
	memcpy(&pParav221->auchAlphaNumRouting, &pParav210->auchAlphaNumRouting, sizeof(pParav210->auchAlphaNumRouting));
	memcpy(&pParav221->futureTotals, &pParav210->futureTotals, sizeof(pParav210->futureTotals));
									//   reorder count, current cost, original cost, etc.)
	memcpy(&pParav221->auchAltPluName, &pParav210->auchAltPluName, sizeof(pParav210->auchAltPluName));
	pParav221->uchColorPaletteCode	= pParav210->uchColorPaletteCode;
	memcpy(&pParav221->auchStatus2, &pParav210->auchStatus2, sizeof(pParav210->auchStatus2));
	pParav221->usTareInformation	= pParav210->usTareInformation;
	memcpy(&pParav221->aucStatus, &pParav210->aucStatus, sizeof(pParav210->aucStatus));

	return TRUE;
}

BOOL  Plu_02010000_02020000_NOCHANGE(DEPTIF *pParav210, DEPTIF *pParav212){
	memcpy(pParav212, pParav210, sizeof(DEPTIF));
	return TRUE;
}

ConversionFunctionListItem PluConversionList[] = {
	{ FILE_HEAD_VER_GP210, 0, PluRead_02010000, Plu_02010000_02020100 },
	{ FILE_HEAD_VER_GP210, 0, 0, 0 },
	{ FILE_HEAD_VER_GP212, 0, PluRead_02010000, Plu_02010000_02020100 },
	{ FILE_HEAD_VER_GP212, 0, 0, 0 },
	{ FILE_HEAD_VER_GP220, 0, PluRead_02010000, Plu_02010000_02020100 },
	{ FILE_HEAD_VER_GP220, 0, 0, 0 }
};

BOOL FileToPluFile(HANDLE iFile, LONG lOffset, LONG lSize, WCHAR* pszFileName, int iStartConvert)
{
	extern SHORT	hsOpPluIndexFileHandle;
	UCHAR           *ucParaBufRay[2], ucParaBuf[1024];
    BOOL            bReturn = FALSE;
    SHORT			sStatus;
    SHORT			nRet;
    FILEHDR_HGV210	PluFileHedOld;
	PLUIF_HGV210	tempPlu;
	ULONG			counter = 0;

	ucParaBufRay[0] = &ucParaBuf[0];
	ucParaBufRay[1] = &ucParaBuf[sizeof(ucParaBuf)/2];

	PifRequestSem(husOpSem);

	//Make a file handle for the old PLU File (R10) JHHJ
    if ((hsOpPluFileHandle = PifOpenFile(auchOP_PLU_OLD, "nwr")) < 0) {
		PifDeleteFile (auchOP_PLU_OLD);
		if ((hsOpPluFileHandle = PifOpenFile(auchOP_PLU_OLD, "nwr")) < 0) {
			PifReleaseSem(husOpSem);
			return(OP_FILE_NOT_FOUND);
		}
    }

	{
		DWORD  bytesRead;
		ULONG  ulActualPosition;
		LONG   lByteCount = 0;

		SetFilePointer(iFile,lOffset,NULL,FILE_BEGIN);
		ReadFile (iFile, ucParaBuf, sizeof(ucParaBuf), &bytesRead, NULL);
		while (bytesRead > 0) {
			if (lSize - lByteCount < bytesRead)
				bytesRead = lSize - lByteCount;
			PifWriteFile (hsOpPluFileHandle, ucParaBuf, bytesRead);
			lByteCount += bytesRead;
			if (lSize <= lByteCount)
				break;
			ReadFile (iFile, ucParaBuf, sizeof(ucParaBuf), &bytesRead, NULL);
		}
		PifSeekFile (hsOpPluFileHandle, 0, &ulActualPosition);
	}

	//Read the header of the old file
    if ((PifReadFile(hsOpPluFileHandle, &PluFileHedOld, sizeof(FILEHDR_HGV210))) != sizeof(FILEHDR_HGV210)) {
		Op_ClosePluFileReleaseSem();
		return(OP_FILE_NOT_FOUND);
    }

	/* ----- Initialize mass memory module, Saratoga ----- */
	PluInitialize();

	/* Create New PLU File based on the number of max records in the old file */
	if((sStatus = (SHORT)PluCreateFile(FILE_PLU, PluFileHedOld.ulMaxRec)) != SPLU_COMPLETED) {
		Op_ClosePluFileReleaseSem();
		return(sStatus);
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

	//Make a file handle for the Temporay Index file so we can read from JHHJ
	if ((hsOpPluIndexFileHandle = PifOpenFile(WIDE("PARAMIDX2"), (CHAR FAR *)auchOLD_FILE_READ_WRITE)) < 0) {
		PifReleaseSem(husOpSem);
		return(OP_FILE_NOT_FOUND);
    }

    tempPlu.ulCounter = 0L;

	do {
		int jk1 = 0, jk2 = 1, j = 0;

		/* ----- Load PLU Records from PLU File(V3.4) until EOF ----- */
		nRet = OpPluAllRead_HGV210(&tempPlu, 0, (RECPLU210 *)ucParaBufRay[0]);
		if((nRet == OP_FILE_NOT_FOUND) || (nRet == OP_EOF)) 
		{
			break;
		}

		// look up in the conversion table until we find which file version we
		// are converting from.  We will then begin with that table entry to do
		// the conversion sequence.
		for (j = 0; j < sizeof(PluConversionList)/sizeof(PluConversionList[0]); j++) {
			if (PluConversionList[j].nFileHeadVer == iStartConvert) {
				break;
			}
		}
		jk1 = 0; jk2 = 1;
		if (j < sizeof(PluConversionList)/sizeof(PluConversionList[0])) {
			bReturn = TRUE;

			iStartConvert = j;

			/* file to file */
			j = iStartConvert;
			for ( ; PluConversionList[j].ItemConvert; j++) {
				PluConversionList[j].ItemConvert (ucParaBufRay[jk1], ucParaBufRay[jk2]);
				jk1 = 1 - jk1;
				jk2 = 1 - jk2;
			}
		}

		{
			RECIDX      RecPluIndex;
			PRECPLU     pRecPlu = (PRECPLU)ucParaBufRay[jk1];

			nRet = OpPluAssignRecPlu (pRecPlu, pRecPlu->uchAdjective, 0);

			/* ----- Set Index Record ---- */
			memcpy(&RecPluIndex.aucPluNumber, pRecPlu->aucPluNumber, OP_PLU_LEN * sizeof(WCHAR));
			RecPluIndex.uchAdjective = pRecPlu->uchAdjective;

			/* ----- Assign record to PLU regular file ----- */
			nRet = PluAddRecord(FILE_PLU_INDEX, &RecPluIndex);
		}

		nRet = OP_PERFORM;
		//breaks out of the loop after it has assigned all of the records in the old file
		tempPlu.ulCounter++;
		if( tempPlu.ulCounter > (PluFileHedOld.ulCurRec))
		{
			break;
		}

	} while(nRet == OP_PERFORM);

    /* ----- Finalize mass memory modual ----- */
    PluFinalize();

	PifCloseFile(hsOpPluIndexFileHandle);
	PifDeleteFile(szPluIndex);
    Op_ClosePluFileReleaseSem();

    return(sStatus);
}
//----------------   End PLU File Conversion     ------------------------

//=================================================================
//=================================================================
//=================================================================

/* ===== End of FILEDIV.C ===== */
