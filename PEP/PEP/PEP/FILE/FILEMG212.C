/*
* ---------------------------------------------------------------------------
* Title         :   File Migration Module
* Category      :   File, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   FILEMG212.C
* Copyright (C) :   2006, Retail Products and Solutions Group
* ===========================================================================
*/

#define NO_PARA_DEFINES   1    // We do not want the pararam defines because we are using pointers to pararam

#include    <windows.h>
#include    <stdio.h>
#include    <string.h>
#include    <ecr.h>
#include    <paraequ.h>
#include    <para.h>
#include    "csop.h"
#include    "plu.h"
#include    <cscas.h>
#include    <csetk.h>

#include    <pararam.h>

#include    "pep.h"
#include    "pepcomm.h"
#include    "pepflag.h"
#include    "trans.h"
#include    "file.h"
#include    "filel.h"
#include    "filemigr.h"
#include    "filemgtb.c"
#include    "..\prog\p060.h"

#include "UNI_2_0_1.h"
#include "UNI_2_0_3.h"
#include "UNI_2_0_5.h"
#include "UNI_2_1_0.h"
#include "UNI_2_1_2.h"

#ifndef INVALID_SET_FILE_POINTER		//older libraries don't set this, but we're using it for foward compatability with SetFilePointer
#define INVALID_SET_FILE_POINTER 0xFFFFFFFF
#endif

/*
* ===========================================================================
*       Compiler Message Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Work Area Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Public Work Area Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Static Work Area Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/


BOOL  FileMigrateGP_212_Files (HANDLE iFile, UNINIRAM_HGV_2_1_2 *pOldPara, int fRet)
{
	int        iLoop;
    WCHAR      szFname[FILE_MAXLEN_TEXT];
//	DWORD	   bytesRead, fileError;
//	FHEADER_HGV201203 fileHeader_GPV212;
	LONG	   fileOffset =0;

    FSCDATA    *lpFsc = 0;
	WCHAR      szTest[] =  L"                    ";
	CONST WCHAR			szNull[]          = WIDE(""),
						szSpaceTend[]     = WIDE(" "),								//
						szSpaceSpec[]     = WIDE("    "),							//
						szSpacePri[]      = WIDE("          "),
						szSpaceLDT[]      = WIDE("                    "),			//Length of PARA_LEADTHRU_LEN_HGV20, 20 spaces
						szSpaceTrans[]    = WIDE("                    "),			// Length of PARA_TRANSMNEMO_LEN_HGV20 , 20 spaces
						szSpaceTerminal[] = WIDE("                       ");		//

    /* delete old file  */
    FileTempDelete(hPepInst);

	memset(pOldPara, 0, sizeof(UNINIRAM_HGV_2_1_2));

	// read header from start of file
	if(!GenFileReadAtOffset(iFile,0,&Fheader,sizeof(Fheader)))
		return FALSE;

    /* ----- Finalize mass memory modual ----- */
    PluFinalize();

	// The file names are defined in FILE.H and begin with IDS_FILE_NAME_DEPT
	// and continue in a particular order.  We are going to use that order
	// for dividing up the files.
	LoadString(hPepInst, IDS_FILE_NAME_DEPT, szFname, FILE_MAXLEN_TEXT);
	FileToDepartmentFile(iFile,
						 Fheader.File.aRoom[IDS_FILE_NAME_DEPT - IDS_FILE_NAME_DEPT].lOffset,
						 Fheader.File.aRoom[IDS_FILE_NAME_DEPT - IDS_FILE_NAME_DEPT].lLen,
						 szFname, fRet);
				
	//Added for R10 to R20, The Index is held in its own file now, so we must transfer it over
	// to the new file as well  JHHJ
	if(FileToEachFile(iFile,
				Fheader.File.aRoom[IDS_FILE_NAME_PLUINDEX - IDS_FILE_NAME_DEPT].lOffset,
				Fheader.File.aRoom[IDS_FILE_NAME_PLUINDEX - IDS_FILE_NAME_DEPT].lLen,
				WIDE("PARAMIDX2")) == FALSE){
		return TRUE;
	}
	LoadString(hResourceDll, IDS_FILE_NAME_PLUTMP, szFname, PEP_STRING_LEN_MAC(szFname));
	FileToPluFile(iFile,
						Fheader.File.aRoom[IDS_FILE_NAME_PLU - IDS_FILE_NAME_DEPT].lOffset,
						Fheader.File.aRoom[IDS_FILE_NAME_PLU - IDS_FILE_NAME_DEPT].lLen,
					   szFname, fRet);

	LoadString(hResourceDll, IDS_FILE_NAME_SER, szFname, PEP_STRING_LEN_MAC(szFname));
	FileToEachFile(iFile,
						Fheader.File.aRoom[IDS_FILE_NAME_SER - IDS_FILE_NAME_DEPT].lOffset,
						Fheader.File.aRoom[IDS_FILE_NAME_SER - IDS_FILE_NAME_DEPT].lLen,
					   szFname);

	//the index of the cashier file changed since version 210 so a new one is used here
	LoadString(hResourceDll, IDS_FILE_NAME_CAS, szFname, PEP_STRING_LEN_MAC(szFname));
	FileToCashierFile(iFile,
						Fheader.File.aRoom[IDS_FILE_NAME_CAS - IDS_FILE_NAME_DEPT].lOffset,
						Fheader.File.aRoom[IDS_FILE_NAME_CAS - IDS_FILE_NAME_DEPT].lLen,
					   szFname, fRet);

	LoadString(hResourceDll, IDS_FILE_NAME_ETK, szFname, PEP_STRING_LEN_MAC(szFname));
	FileToEmployeeTimeFile(iFile,
						Fheader.File.aRoom[IDS_FILE_NAME_ETK - IDS_FILE_NAME_DEPT].lOffset,
						Fheader.File.aRoom[IDS_FILE_NAME_ETK - IDS_FILE_NAME_DEPT].lLen,
					   szFname, fRet);

	LoadString(hResourceDll, IDS_FILE_NAME_DIRECT, szFname, PEP_STRING_LEN_MAC(szFname));
	FileToEachFile(iFile,
						Fheader.File.aRoom[IDS_FILE_NAME_DIRECT - IDS_FILE_NAME_DEPT].lOffset,
						Fheader.File.aRoom[IDS_FILE_NAME_DIRECT - IDS_FILE_NAME_DEPT].lLen,
					   szFname);

	LoadString(hResourceDll, IDS_FILE_NAME_CPN, szFname, PEP_STRING_LEN_MAC(szFname));
	FileToCouponFile(iFile,
						Fheader.File.aRoom[IDS_FILE_NAME_CPN - IDS_FILE_NAME_DEPT].lOffset,
						Fheader.File.aRoom[IDS_FILE_NAME_CPN - IDS_FILE_NAME_DEPT].lLen,
						szFname, fRet);

	LoadString(hResourceDll, IDS_FILE_NAME_CSTR, szFname, PEP_STRING_LEN_MAC(szFname));
	FileToEachFile(iFile,
						Fheader.File.aRoom[IDS_FILE_NAME_CSTR - IDS_FILE_NAME_DEPT].lOffset,
						Fheader.File.aRoom[IDS_FILE_NAME_CSTR - IDS_FILE_NAME_DEPT].lLen,
					   szFname);

	LoadString(hResourceDll, IDS_FILE_NAME_PPI, szFname, PEP_STRING_LEN_MAC(szFname));
	FileToPpiFile(iFile,
						Fheader.File.aRoom[IDS_FILE_NAME_PPI - IDS_FILE_NAME_DEPT].lOffset,
						Fheader.File.aRoom[IDS_FILE_NAME_PPI - IDS_FILE_NAME_DEPT].lLen,
						szFname, fRet);

	LoadString(hResourceDll, IDS_FILE_NAME_MLD, szFname, PEP_STRING_LEN_MAC(szFname));
	FileToEachFile(iFile,
						Fheader.File.aRoom[IDS_FILE_NAME_MLD - IDS_FILE_NAME_DEPT].lOffset,
						Fheader.File.aRoom[IDS_FILE_NAME_MLD - IDS_FILE_NAME_DEPT].lLen,
					   szFname);

	// create an empty reason code mnemonics file
	OpMnemonicsFileCreate (OPMNEMONICFILE_ID_REASON, MAX_RSNMNEMO_SIZE, 0);

    iLoop = 15;
    LoadString(hResourceDll,  IDS_FILE_NAME_OEPFILE, szFname, PEP_STRING_LEN_MAC(szFname));
    FileToEachFile(iFile,
                       Fheader.File.aRoom[iLoop].lOffset,
                       Fheader.File.aRoom[iLoop].lLen,
                       szFname);

    iLoop = 16;
	LoadString(hResourceDll, IDS_FILE_NAME_LOGOIDX,szFname, PEP_STRING_LEN_MAC(szFname));
    FileToEachFile(iFile,
                       Fheader.File.aRoom[iLoop].lOffset,
                       Fheader.File.aRoom[iLoop].lLen,
                       szFname);

	/* ----- Initialize mass memory module, Saratoga ----- */
	PluInitialize();


	//Added for R10 to R20, The Index is held in its own file now, so we must transfer it over
	// to the new file as well  JHHJ
	if(FileToEachFile(iFile,
				Fheader.File.aRoom[IDS_FILE_NAME_PLUINDEX - IDS_FILE_NAME_DEPT].lOffset,
				Fheader.File.aRoom[IDS_FILE_NAME_PLUINDEX - IDS_FILE_NAME_DEPT].lLen,
				WIDE("PARAMIDX2")) == FALSE){
		return TRUE;
	}

	LoadString(hResourceDll, IDS_FILE_NAME_PLU, szFname, PEP_STRING_LEN_MAC(szFname));
	FileToPlu203File(iFile, 
						Fheader.File.aRoom[IDS_FILE_NAME_PLU - IDS_FILE_NAME_DEPT].lOffset,
						Fheader.File.aRoom[IDS_FILE_NAME_PLU - IDS_FILE_NAME_DEPT].lLen,
						szFname);

	/* divide each memory   */
	/* parameter            */

	//make sure the read doesn't go larger than expected
	//Fheader.Mem.Para.lLen is unreliable so only reading sizeof(UNINIRAM_HGV_2_1_2)
	if(!GenFileReadAtOffset(iFile,Fheader.Mem.Para.lOffset, pOldPara, sizeof(UNINIRAM_HGV_2_1_2)))
		return FALSE;

    /* --- set progarmmable report ini. file name R3.1 --- */
    wcsncpy(szCommRptSaveName, Fheader.Fid.szRptName, PEP_FILENAME_LEN);

	/* --- set Layout Manager file name Added JHHJ for release 2.0--- */
	wcsncpy(szCommTouchLayoutSaveName, Fheader.Fid.szTouchLayoutName, PEP_FILENAME_LEN); //ESMITH LAYOUT
	wcsncpy(szCommKeyboardLayoutSaveName, Fheader.Fid.szKeyboardLayoutName, PEP_FILENAME_LEN);
	wcsncpy(szCommLogoSaveName, Fheader.szReceiptLogoName, PEP_FILENAME_LEN);
	wcsncpy(szCommIconSaveName, Fheader.Fid.szIconFileDirectory, PEP_FILENAME_LEN);

	return TRUE;
}


/*
* ===========================================================================
**  Synopsis:  BOOL  FileMigrateGP_205_212_Para(UNINIRAM_HGV_2_0_5 *pOldPara, UNINIRAM *pNewPara)
*
*   Input   :   UNINIRAM_HGV_2_0_5  * pOldPara   - Rel 2.0.5 type of Param.
**
*   Output  :   UNINIRAM           * pNewPara   - Rel 2.1.2 type of Param.
**
*   Return  :   TRUE           - user process is executed
*               FALSE          - default process is expected
*
**  Description:  This function converts the parameter data in a buffer
*                 from Release 2.0.5 to Release 2.1.2.
*				  The conversion is done from the provided UNINIRAM buffer
*                 for Release 2.0.5 to a buffer for Release 2.1.2.
*
* ===========================================================================
*/
BOOL  FileMigrateGP_205_212_Para (UNINIRAM_HGV_2_0_5 *pOldPara, UNINIRAM_HGV_2_1_2 *pNewPara)
{
    USHORT     j,i;
	LONG	   fileOffset = 0;

	FILE       *pLogFile;

    FSCDATA    *lpFsc = 0;
	WCHAR      szTest[] =  L"                    ";
	CONST WCHAR			szNull[]          = WIDE(""),
						szSpaceTend[]     = WIDE(" "),							//
						szSpaceSpec[]     = WIDE("    "),						//
						szSpacePri[]      = WIDE("          "),
						szSpaceLDT[]      = WIDE("                    "),		// Length of PARA_LEADTHRU_LEN_HGV20, 20 spaces
						szSpaceTrans[]    = WIDE("                    "),		// Length of PARA_TRANSMNEMO_LEN_HGV20 , 20 spaces
						szSpaceTerminal[] = WIDE("                       ");	//

	//Log File Implementation

	pLogFile = fopen ("PepConversion.log", "w");

	/* BEGIN PARAMETER CONVERSION ------------------------------------------------------*/
	//Log File Functionality

	memset (pNewPara, 0, sizeof(UNINIRAM));

	//Prog. #1
	memcpy(pNewPara->ParaMDC, pOldPara->ParaMDC, sizeof(pOldPara->ParaMDC));
	if( sizeof(pNewPara->ParaMDC) != sizeof(pOldPara->ParaMDC) ){
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #1 - MDC conversion size difference.\n");
	}

	//Prog. #2
	memcpy(pNewPara->ParaFlexMem, pOldPara->ParaFlexMem, sizeof(pOldPara->ParaFlexMem));
	if( sizeof(pNewPara->ParaFlexMem) != sizeof(pOldPara->ParaFlexMem) ){
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #1 - FlexMem conversion size difference.\n");
	}

	// Now check the ranges of Flexible Memory settings to ensure they are within allowable limits
	// If a setting is outside of the maximum then set it to the maximum
	if (pNewPara->ParaFlexMem[FLEX_DEPT_ADR - 1].ulRecordNumber > FLEX_DEPT_MAX) {
		pNewPara->ParaFlexMem[FLEX_DEPT_ADR - 1].ulRecordNumber = FLEX_DEPT_MAX;
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #2 - FLEX_DEPT_ADR value > FLEX_DEPT_MAX\n");
	}

	if (pNewPara->ParaFlexMem[FLEX_PLU_ADR - 1].ulRecordNumber > FLEX_PLU_MAX) {
		pNewPara->ParaFlexMem[FLEX_PLU_ADR - 1].ulRecordNumber = FLEX_PLU_MAX;
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #2 - FLEX_PLU_ADR value > FLEX_PLU_MAX\n");
	}

	if (pNewPara->ParaFlexMem[FLEX_WT_ADR - 1].ulRecordNumber > FLEX_WT_MAX) {
		pNewPara->ParaFlexMem[FLEX_WT_ADR - 1].ulRecordNumber = FLEX_WT_MAX;
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #2 - FLEX_WT_ADR value > FLEX_WT_MAX\n");
	}

	if (pNewPara->ParaFlexMem[FLEX_CAS_ADR - 1].ulRecordNumber > FLEX_CAS_MAX) {
		pNewPara->ParaFlexMem[FLEX_CAS_ADR - 1].ulRecordNumber = FLEX_CAS_MAX;
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #2 - LEX_CAS_ADR value > LEX_CAS_MAX\n");
	}

	if (pNewPara->ParaFlexMem[FLEX_EJ_ADR - 1].ulRecordNumber > FLEX_EJ_MAX) {
		pNewPara->ParaFlexMem[FLEX_EJ_ADR - 1].ulRecordNumber = FLEX_EJ_MAX;
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #2 - FLEX_EJ_ADR value > FLEX_EJ_MAX\n");
	}

	if (pNewPara->ParaFlexMem[FLEX_ETK_ADR - 1].ulRecordNumber > FLEX_ETK_MAX) {
		pNewPara->ParaFlexMem[FLEX_ETK_ADR - 1].ulRecordNumber = FLEX_ETK_MAX;
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #2 - FLEX_ETK_ADR value > FLEX_ETK_MAX\n");
	}

	if (pNewPara->ParaFlexMem[FLEX_ITEMSTORAGE_ADR - 1].ulRecordNumber > FLEX_ITEMSTORAGE_MAX) {
		pNewPara->ParaFlexMem[FLEX_ITEMSTORAGE_ADR - 1].ulRecordNumber = FLEX_ITEMSTORAGE_MAX;
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #2 - FLEX_ITEMSTORAGE_ADR value > FLEX_ITEMSTORAGE_MAX\n");
	}

	if (pNewPara->ParaFlexMem[FLEX_GC_ADR - 1].ulRecordNumber > FLEX_GC_MAX) {
		pNewPara->ParaFlexMem[FLEX_GC_ADR - 1].ulRecordNumber = FLEX_GC_MAX;
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #2 - FLEX_GC_ADR value > FLEX_GC_MAX\n");
	}

	if (pNewPara->ParaFlexMem[FLEX_CPN_ADR - 1].ulRecordNumber > FLEX_CPN_MAX) {
		pNewPara->ParaFlexMem[FLEX_CPN_ADR - 1].ulRecordNumber = FLEX_CPN_MAX;
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #2 - FLEX_CPN_ADR value > FLEX_CPN_MAX\n");
	}

	if (pNewPara->ParaFlexMem[FLEX_CSTRING_ADR - 1].ulRecordNumber > FLEX_CSTRING_MAX) {
		pNewPara->ParaFlexMem[FLEX_CSTRING_ADR - 1].ulRecordNumber = FLEX_CSTRING_MAX;
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #2 - FLEX_CSTRING_ADR value > FLEX_CSTRING_MAX\n");
	}

	if (pNewPara->ParaFlexMem[FLEX_PROGRPT_ADR - 1].ulRecordNumber > FLEX_PROGRPT_MAX) {
		pNewPara->ParaFlexMem[FLEX_PROGRPT_ADR - 1].ulRecordNumber = FLEX_PROGRPT_MAX;
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #2 - FLEX_PROGRPT_ADR value > FFLEX_PROGRPT_MAX\n");
	}

	if (pNewPara->ParaFlexMem[FLEX_PPI_ADR - 1].ulRecordNumber > FLEX_PPI_MAX) {
		pNewPara->ParaFlexMem[FLEX_PPI_ADR - 1].ulRecordNumber = FLEX_PPI_MAX;
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #2 - FLEX_PPI_ADR value > FLEX_PPI_MAX\n");
	}

	// Ensure that if the file being converted has a smaller MLD size then
	// we will up it to what ever we are currently using.
	if (pNewPara->ParaFlexMem[FLEX_MLD_ADR - 1].ulRecordNumber < MAX_MLDMNEMO_SIZE) {
		pNewPara->ParaFlexMem[FLEX_MLD_ADR - 1].ulRecordNumber = MAX_MLDMNEMO_SIZE;
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #2 - FLEX_MLD_ADR value > MAX_MLDMNEMO_SIZE\n");
	}

	//Prog. #3, conversion neccessary.
	for( i = 0; i < MAX_FSC_TBL_HGV_2_0_5; i++)
	{
		memcpy(&pNewPara->ParaFSC[i], &pOldPara->ParaFSC[i], sizeof(pOldPara->ParaFSC[i]));
	}

	if( sizeof(pNewPara->ParaFSC[0]) != sizeof(pOldPara->ParaFSC[0]) ){
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #3 - ParaFSC[0] conversion size difference.\n");
	}

	//Prog. #6
	memcpy(&pNewPara->ParaSecurityNo, &pOldPara->ParaSecurityNo, sizeof(pOldPara->ParaSecurityNo));

	if( sizeof(pNewPara->ParaSecurityNo) != sizeof(pOldPara->ParaSecurityNo) ){
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #6 - ParaSecurityNo conversion size difference.\n");
	}

	//Prog. #7
	memcpy(&pNewPara->ParaStoRegNo, &pOldPara->ParaStoRegNo, sizeof(pOldPara->ParaStoRegNo));

	if( sizeof(pNewPara->ParaStoRegNo) != sizeof(pOldPara->ParaStoRegNo) ){
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #7 - ParaStoRegNo conversion size difference.\n");
	}

	//Prog. #8
	memcpy(&pNewPara->ParaSupLevel, &pOldPara->ParaSupLevel, sizeof(pOldPara->ParaSupLevel));

	if( sizeof(pNewPara->ParaSupLevel) != sizeof(pOldPara->ParaSupLevel) ){
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #8 - ParaSupLevel conversion size difference.\n");
	}

	//Prog. #9
	memcpy(&pNewPara->ParaActCodeSec, &pOldPara->ParaActCodeSec, sizeof(pOldPara->ParaActCodeSec));

	if( sizeof(pNewPara->ParaActCodeSec) != sizeof(pOldPara->ParaActCodeSec) ){
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #9 - ParaActCodeSec conversion size difference.\n");
	}

	//Prog. #10
	memcpy(&pNewPara->ParaTransHALO, &pOldPara->ParaTransHALO, sizeof(pOldPara->ParaTransHALO));

	if( sizeof(pNewPara->ParaTransHALO) != sizeof(pOldPara->ParaTransHALO) ){
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #10 - ParaTransHALO conversion size difference.\n");
	}

	//Prog. #17
	memcpy(&pNewPara->ParaHourlyTime, &pOldPara->ParaHourlyTime, sizeof(pOldPara->ParaHourlyTime));

	if( sizeof(pNewPara->ParaHourlyTime) != sizeof(pOldPara->ParaHourlyTime) ){
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #17 - ParaHourlyTime conversion size difference.\n");
	}

	//Prog. #18
	memcpy(&pNewPara->ParaSlipFeedCtl, &pOldPara->ParaSlipFeedCtl, sizeof(pOldPara->ParaSlipFeedCtl));

	if( sizeof(pNewPara->ParaSlipFeedCtl) != sizeof(pOldPara->ParaSlipFeedCtl) ){
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #18 - ParaSlipFeedCtl conversion size difference.\n");
	}


	//LeadThru Mnemonics
	/*
		Para_LeadThru is composed of our current default mnemonics, where HGV20_ParaLeadThru
		is composed of the leadthru values read from the parameter file. If an LDT has no
		value or is empty, szNull , or the LDT is composed of only spaces, szSpaceLDT, we
		will not copy the LDT over the current default mnemonic. If there is a value in the
		LDT other than szNull and szSpaceLDT we will copy it over the default mnemonic. ESMITH
	*/
	//Mnemonics Conversion
	//The new conversion test are exactly the same it only test if the mnemonic is full of
	//spaces or empty, if it is not then it replaces the default mnemonic and if it is empty
	//the default mnemonic stays the same and that is what is printed in PEP. RZACHARY

	//Transaction Mnemonics Conversion
	for( i = 0; i <= MAX_TRANSM_NO_HGV_2_0_5; i++){
		if( !wcsncmp(pOldPara->ParaTransMnemo[i], szNull, PARA_TRANSMNEMO_LEN_HGV_2_0_5) ||
			!wcsncmp(pOldPara->ParaTransMnemo[i], szSpaceTrans, PARA_TRANSMNEMO_LEN_HGV_2_0_5) )
		{/*  | Do Nothing |  */} else {
			wcsncpy(pNewPara->ParaTransMnemo[i], pOldPara->ParaTransMnemo[i], PARA_TRANSMNEMO_LEN_HGV_2_0_5);
		}
	}

	//Lead Thru Mnemonics Conversion
	for( i = 0; i <= MAX_LEAD_NO_HGV_2_0_5; i++) {
		if( !wcsncmp(pOldPara->ParaLeadThru[i], szNull, PARA_LEADTHRU_LEN_HGV_2_0_5) ||
			!wcsncmp(pOldPara->ParaLeadThru[i], szSpaceLDT, PARA_LEADTHRU_LEN_HGV_2_0_5) )
		{ /* Do Nothing */} else {
			wcsncpy(pNewPara->ParaLeadThru[i], pOldPara->ParaLeadThru[i], PARA_LEADTHRU_LEN_HGV_2_0_5);
		}
	}

	//Report Name Mnemonics Conversion
	for( i = 0; i <= MAX_REPO_NO_HGV_2_0_5; i++) {
		if( !wcsncmp(pOldPara->ParaReportName[i], szNull, PARA_REPORTNAME_LEN_HGV_2_0_5) ||
			!wcsncmp(pOldPara->ParaReportName[i], szSpaceLDT, PARA_REPORTNAME_LEN_HGV_2_0_5) )
		{ /* Do Nothing */} else {
			wcsncpy(pNewPara->ParaReportName[i],pOldPara->ParaReportName[i],PARA_REPORTNAME_LEN_HGV_2_0_5);
		}
	}

	//Special Mnemonics Conversion
	for( i = 0; i <= MAX_SPEM_NO_HGV_2_0_5; i++ ){
		if( !wcsncmp(pOldPara->ParaSpeMnemo[i], szNull, PARA_SPEMNEMO_LEN_HGV_2_0_5) ||
			!wcsncmp(pOldPara->ParaSpeMnemo[i], szSpaceSpec, PARA_SPEMNEMO_LEN_HGV_2_0_5) )
		{/* Do Nothing */} 	else {
			wcsncpy(pNewPara->ParaSpeMnemo[i], pOldPara->ParaSpeMnemo[i],PARA_SPEMNEMO_LEN_HGV_2_0_5);
		}
	}

	//Prog. #39
	memcpy(&pNewPara->ParaPCIF, &pOldPara->ParaPCIF, sizeof(pOldPara->ParaPCIF));

	// Print Modifier Mnemonics Conversion
	for(i = 0; i <= MAX_PRTMODI_NO_HGV_2_0_5; i++) {
		if( !wcsncmp(pOldPara->ParaPrtModi[i], szNull, PARA_PRTMODI_LEN_HGV_2_0_5) ||
			!wcsncmp(pOldPara->ParaPrtModi[i], szSpacePri, PARA_PRTMODI_LEN_HGV_2_0_5) )
		{ /* Do Nothing */} else {
			wcsncpy(pNewPara->ParaPrtModi[i],pOldPara->ParaPrtModi[i],PARA_PRTMODI_LEN_HGV_2_0_5);
		}
	}

	//Prog. #48
	memcpy(pNewPara->ParaMajorDEPT, pOldPara->ParaMajorDEPT, sizeof(pOldPara->ParaMajorDEPT));

	//Prog. #49
	memcpy(pNewPara->ParaAutoAltKitch, pOldPara->ParaAutoAltKitch, sizeof(pOldPara->ParaAutoAltKitch));

	//CPMDATA Prog. #54
	{
		memcpy(pNewPara->ParaHotelId.auchHotelId, &pOldPara->ParaHotelId.auchHotelId, sizeof(pOldPara->ParaHotelId.auchHotelId));

		memcpy(pNewPara->ParaHotelId.auchSld, &pOldPara->ParaHotelId.auchSld, sizeof(pOldPara->ParaHotelId.auchSld));

		memcpy(pNewPara->ParaHotelId.auchProductId, &pOldPara->ParaHotelId.auchProductId, sizeof(pOldPara->ParaHotelId.auchProductId));

		for (j = 0; j < PARA_TEND_CPM_NO_HGV_2_0_5; j++) {
			pNewPara->ParaHotelId.auchTend[j] = pOldPara->ParaHotelId.auchTend[j];
		}
		for (j = 0; j < sizeof(pOldPara->ParaHotelId.auchApproval)/sizeof(pOldPara->ParaHotelId.auchApproval[0]); j++) {
			pNewPara->ParaHotelId.auchApproval[j] = pOldPara->ParaHotelId.auchApproval[j];
		}
	}
	//Prog. #57
	memcpy(pNewPara->ParaChar24, pOldPara->ParaChar24, sizeof(pOldPara->ParaChar24));

	// Prog. #60 - Total Key Conversion
	memset(&pNewPara->ParaTtlKeyTyp, 0x00, sizeof(pNewPara->ParaTtlKeyTyp));
	memcpy(&pNewPara->ParaTtlKeyTyp, pOldPara->ParaTtlKeyTyp, sizeof(pOldPara->ParaTtlKeyTyp));

	// Prog. #67 - Auto Coupon conversion
	memcpy(&pNewPara->ParaAutoCoupon, pOldPara->ParaAutoCoupon, sizeof(pOldPara->ParaAutoCoupon));
/*
	for (i = 0; i < MAX_KEYTYPE_SIZE_HGV_2_0_1; i++) {
		totalKeyOldType = P60_HGV20GETTYPE(pOldPara->HGV20_ParaTtlKeyTyp[i]);

		totalKeyOldIndex= P60_HGV20GETINDEX(pOldPara->HGV20_ParaTtlKeyTyp[i]);

		ParaTtlKeyTyp[i] = P60SETTYPE(totalKeyOldType);
		ParaTtlKeyTyp[i] += P60SETINDEX(totalKeyOldIndex);
	}
*/

	//Prog. #49
	memcpy(&pNewPara->ParaAutoAltKitch, &pOldPara->ParaAutoAltKitch, sizeof(pOldPara->ParaAutoAltKitch));

	//Prog. #61
	memcpy(&pNewPara->ParaTtlKeyCtl, &pOldPara->ParaTtlKeyCtl, sizeof(pOldPara->ParaTtlKeyCtl));

	//Prog. #62
	memcpy(&pNewPara->ParaTend, &pOldPara->ParaTend, sizeof(pOldPara->ParaTend));

	//A/C #4 PLU Menu
	memcpy(&pNewPara->ParaPLUNoMenu, &pOldPara->ParaPLUNoMenu, sizeof(pOldPara->ParaPLUNoMenu));

	//A/C #5
	memcpy(&pNewPara->ParaCtlTblMenu, &pOldPara->ParaCtlTblMenu, sizeof(pOldPara->ParaCtlTblMenu));

	//A/C #6
	memcpy(&pNewPara->ParaManuAltKitch, &pOldPara->ParaManuAltKitch, sizeof(pOldPara->ParaManuAltKitch));

	//A/C #7
	//memcpy(&pNewPara->ParaCashABAssign, &pOldPara->ParaCashABAssign, sizeof(pOldPara->ParaCashABAssign));
	for(i = 0; i < MAX_AB_SIZE; i++){
		pNewPara->ParaCashABAssign[i] = pOldPara->ParaCashABAssign[i];	//version 212 changes from USHORT to ULONG
	}

	//A/C #84
	memcpy(&pNewPara->ParaRound, &pOldPara->ParaRound, sizeof(pOldPara->ParaRound));

	//A/C #86
	memcpy(&pNewPara->ParaDisc, &pOldPara->ParaDisc, sizeof(pOldPara->ParaDisc));

	//A/C #88
	memcpy(&pNewPara->ParaPromotion, &pOldPara->ParaPromotion, sizeof(pOldPara->ParaPromotion));

	//A/C #84
	memcpy(&pNewPara->ParaRound, &pOldPara->ParaRound, sizeof(pOldPara->ParaRound));

	//A/C #89
	memcpy(&pNewPara->ParaCurrency, &pOldPara->ParaCurrency, sizeof(pOldPara->ParaCurrency));

	//A/C #169
	memcpy(&pNewPara->ParaToneCtl, &pOldPara->ParaToneCtl, sizeof(pOldPara->ParaToneCtl));

	//A/C #116 Promotional PLU table
	for (i = 0; i < MAX_SETMENU_NO_HGV_2_0_5; i++) {
		memcpy(&pNewPara->ParaMenuPLU[i], &pOldPara->ParaMenuPLU[i], sizeof(pOldPara->ParaMenuPLU[i]));
	}

	//A/C #124-126
	memcpy(&pNewPara->ParaTaxTable, &pOldPara->ParaTaxTable, sizeof(pOldPara->ParaTaxTable));

	//A/C #127
	memcpy(&pNewPara->ParaTaxRate, &pOldPara->ParaTaxRate, sizeof(pOldPara->ParaTaxRate));

	//MISC PARAMETER
	memcpy(&pNewPara->ParaMiscPara, &pOldPara->ParaMiscPara, sizeof(pOldPara->ParaMiscPara));

	//A/C #111
	memcpy(&pNewPara->ParaTare, &pOldPara->ParaTare, sizeof(pOldPara->ParaTare));

	//Prog. #15
	memcpy(&pNewPara->ParaPresetAmount, &pOldPara->ParaPresetAmount, sizeof(pOldPara->ParaPresetAmount));

	//Prog. #30
	memcpy(&pNewPara->ParaSharedPrt, &pOldPara->ParaSharedPrt, sizeof(pOldPara->ParaSharedPrt));

	//A/C #87
	memcpy(&pNewPara->ParaSoftChk, &pOldPara->ParaSoftChk, sizeof(pOldPara->ParaSoftChk));

	//A/C #130
	memcpy(&pNewPara->ParaPigRule, &pOldPara->ParaPigRule, sizeof(pOldPara->ParaPigRule));

	//A/C #160
	memcpy(&pNewPara->ParaOep, &pOldPara->ParaOep, sizeof(pOldPara->ParaOep));

	//A/C #162
	memcpy(&pNewPara->ParaFlexDrive, &pOldPara->ParaFlexDrive, sizeof(pOldPara->ParaFlexDrive));

	//A/C #133
	memcpy(&pNewPara->ParaServiceTime, &pOldPara->ParaServiceTime, sizeof(pOldPara->ParaServiceTime));

	//A/C #154
	memcpy(&pNewPara->ParaLaborCost, &pOldPara->ParaLaborCost, sizeof(pOldPara->ParaLaborCost));

	//A/C #137
	memcpy(&pNewPara->ParaDispPara, &pOldPara->ParaDispPara, sizeof(pOldPara->ParaDispPara));

	//KDS display addresses
	memcpy(&pNewPara->ParaKdsIp, &pOldPara->ParaKdsIp, sizeof(pOldPara->ParaKdsIp));

	//A/C #170
	memcpy(&pNewPara->ParaRestriction, &pOldPara->ParaRestriction, sizeof(pOldPara->ParaRestriction));

	//A/C #208
	memcpy(&pNewPara->auchParaBoundAge, &pOldPara->auchParaBoundAge, sizeof(pOldPara->auchParaBoundAge));

	//A/C #4
	memcpy(&pNewPara->ParaDeptNoMenu, &pOldPara->ParaDeptNoMenu, sizeof(pOldPara->ParaDeptNoMenu));

	//SPECIAL COUNTER
	memcpy(&pNewPara->ParaSpcCo, &pOldPara->ParaSpcCo, sizeof(pOldPara->ParaSpcCo));

	// A/C 209 - Color Palette
	for (i = 0; i < MAX_COLORPALETTE_SIZE; i++) {
		pNewPara->ParaColorPalette[i][0] = 0x000000 ; // initialize foreground color to black
		pNewPara->ParaColorPalette[i][1] = 0xffffff ; // initialize background color to white
	}

	//CHARGE POST INFORMATION
	memcpy(&pNewPara->ChargePostInformation, &pOldPara->ChargePostInformation, sizeof(pOldPara->ChargePostInformation));

	//Prog. 97 TERMINAL INFORMATION
	for( i = 0; i <= MAX_TERMINALINFO_SIZE_HGV_2_0_5; i++) 
	{
		pNewPara->TerminalInformation[i].ulCodePage = pOldPara->TerminalInformation[i].ulCodePage;
		pNewPara->TerminalInformation[i].usLanguageID = pOldPara->TerminalInformation[i].usLanguageID;
		pNewPara->TerminalInformation[i].usSubLanguageID = pOldPara->TerminalInformation[i].usSubLanguageID;
		pNewPara->TerminalInformation[i].usStoreNumber = pOldPara->TerminalInformation[i].usStoreNumber;
		pNewPara->TerminalInformation[i].usRegisterNumber = pOldPara->TerminalInformation[i].usRegisterNumber;
		pNewPara->TerminalInformation[i].uchKeyBoardType = pOldPara->TerminalInformation[i].uchKeyBoardType;
		pNewPara->TerminalInformation[i].uchMenuPageDefault = pOldPara->TerminalInformation[i].uchMenuPageDefault;
		memcpy(pNewPara->TerminalInformation[i].aszDefaultFontFamily, pOldPara->TerminalInformation[i].aszDefaultFontFamily, sizeof(pOldPara->TerminalInformation[i].aszDefaultFontFamily));
		memcpy(pNewPara->TerminalInformation[i].aszUnLockCode, pOldPara->TerminalInformation[i].aszUnLockCode, sizeof(pOldPara->TerminalInformation[i].aszUnLockCode));
		memcpy(pNewPara->TerminalInformation[i].aszTerminalSerialNumber, pOldPara->TerminalInformation[i].aszTerminalSerialNumber, sizeof(pOldPara->TerminalInformation[i].aszTerminalSerialNumber));
		memcpy(pNewPara->TerminalInformation[i].aszDefaultMenuFileName, pOldPara->TerminalInformation[i].aszDefaultMenuFileName, sizeof(pOldPara->TerminalInformation[i].aszDefaultMenuFileName));
	}
	 
	//EPT
	memcpy(&pNewPara->EPT_ENH_Information, &pOldPara->EPT_ENH_Information, sizeof(pOldPara->EPT_ENH_Information));

	//TENDER KEY INFORMATION
	memcpy(&pNewPara->TenderKeyInformation, &pOldPara->TenderKeyInformation, sizeof(pOldPara->TenderKeyInformation));

	//STORE AND FORWARD MISC PARAMETER
	memcpy(&pNewPara->ParaStoreForward, &pOldPara->ParaStoreForward, sizeof(pOldPara->ParaStoreForward));

	OpPluFileUpdate(0);

	fclose(pLogFile);
	return TRUE;
}


/*
* ===========================================================================
**  Synopsis:  BOOL  FileMigrateGP_210_212_Para(UNINIRAM_HGV_2_1_0 *pOldPara, UNINIRAM *pNewPara)
*
*   Input   :   UNINIRAM_HGV_2_1_0  * pOldPara   - Rel 2.1.0 type of Param.
**
*   Output  :   UNINIRAM  * pNewPara   - Rel 2.1.2 type of Param.
**
*   Return  :   TRUE           - user process is executed
*               FALSE          - default process is expected
*
**  Description:  This function converts the parameter data in a buffer
*                 from Release 2.1.0 to Release 2.1.2, the current release.
*                 The conversion is done from the provided UNINIRAM buffer
*                 for Release 2.1.0 to a buffer for Release 2.1.2.
*
* ===========================================================================
*/
BOOL  FileMigrateGP_210_212_Para (UNINIRAM_HGV_2_1_0 *pOldPara, UNINIRAM_HGV_2_1_2 *pNewPara)
{
    USHORT     j,i;
	LONG	   fileOffset = 0;
	FILE       *pLogFile;
    FSCDATA    *lpFsc = 0;
	WCHAR      szTest[] =  L"                    ";
	CONST WCHAR			szNull[]          = WIDE(""),
						szSpaceLDT[]      = WIDE("                    "),			// Length of PARA_LEADTHRU_LEN_HGV20, 20 spaces
						szSpaceTrans[]    = WIDE("                    "),			// Length of PARA_TRANSMNEMO_LEN_HGV20 , 20 spaces
						szSpaceSpec[]     = WIDE("    "),							//
						szSpacePri[]      = WIDE("          "),
						szSpaceTend[]     = WIDE(" "),								//
						szSpaceTerminal[] = WIDE("                       ");		//

\
	//Log File Implementation

	pLogFile = fopen ("PepConversion.log", "w");

	/* BEGIN PARAMETER CONVERSION ------------------------------------------------------*/
	//Log File Functionality

	memset (pNewPara, 0, sizeof(UNINIRAM));


	//Prog. #1
	memcpy(&pNewPara->ParaMDC[0], &pOldPara->ParaMDC[0], sizeof(pOldPara->ParaMDC));
	if( sizeof(pNewPara->ParaMDC) != sizeof(pOldPara->ParaMDC) ){
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #1 - MDC conversion size difference.\n");
	}

	//Prog. #2
	memcpy(&pNewPara->ParaFlexMem, &pOldPara->ParaFlexMem, sizeof(pOldPara->ParaFlexMem));
	if( sizeof(pNewPara->ParaFlexMem) != sizeof(pOldPara->ParaFlexMem) ){
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #1 - FlexMem conversion size difference.\n");
	}

	// Now check the ranges of Flexible Memory settings to ensure they are within allowable limits
	// If a setting is outside of the maximum then set it to the maximum
	if (pNewPara->ParaFlexMem[FLEX_DEPT_ADR - 1].ulRecordNumber > FLEX_DEPT_MAX) {
		pNewPara->ParaFlexMem[FLEX_DEPT_ADR - 1].ulRecordNumber = FLEX_DEPT_MAX;
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #2 - FLEX_DEPT_ADR value > FLEX_DEPT_MAX\n");
	}

	if (pNewPara->ParaFlexMem[FLEX_PLU_ADR - 1].ulRecordNumber > FLEX_PLU_MAX) {
		pNewPara->ParaFlexMem[FLEX_PLU_ADR - 1].ulRecordNumber = FLEX_PLU_MAX;
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #2 - FLEX_PLU_ADR value > FLEX_PLU_MAX\n");
	}

	if (pNewPara->ParaFlexMem[FLEX_WT_ADR - 1].ulRecordNumber > FLEX_WT_MAX) {
		pNewPara->ParaFlexMem[FLEX_WT_ADR - 1].ulRecordNumber = FLEX_WT_MAX;
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #2 - FLEX_WT_ADR value > FLEX_WT_MAX\n");
	}

	if (pNewPara->ParaFlexMem[FLEX_CAS_ADR - 1].ulRecordNumber > FLEX_CAS_MAX) {
		pNewPara->ParaFlexMem[FLEX_CAS_ADR - 1].ulRecordNumber = FLEX_CAS_MAX;
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #2 - LEX_CAS_ADR value > LEX_CAS_MAX\n");
	}

	if (pNewPara->ParaFlexMem[FLEX_EJ_ADR - 1].ulRecordNumber > FLEX_EJ_MAX) {
		pNewPara->ParaFlexMem[FLEX_EJ_ADR - 1].ulRecordNumber = FLEX_EJ_MAX;
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #2 - FLEX_EJ_ADR value > FLEX_EJ_MAX\n");
	}

	if (pNewPara->ParaFlexMem[FLEX_ETK_ADR - 1].ulRecordNumber > FLEX_ETK_MAX) {
		pNewPara->ParaFlexMem[FLEX_ETK_ADR - 1].ulRecordNumber = FLEX_ETK_MAX;
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #2 - FLEX_ETK_ADR value > FLEX_ETK_MAX\n");
	}

	if (pNewPara->ParaFlexMem[FLEX_ITEMSTORAGE_ADR - 1].ulRecordNumber > FLEX_ITEMSTORAGE_MAX) {
		pNewPara->ParaFlexMem[FLEX_ITEMSTORAGE_ADR - 1].ulRecordNumber = FLEX_ITEMSTORAGE_MAX;
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #2 - FLEX_ITEMSTORAGE_ADR value > FLEX_ITEMSTORAGE_MAX\n");
	}

	if (pNewPara->ParaFlexMem[FLEX_GC_ADR - 1].ulRecordNumber > FLEX_GC_MAX) {
		pNewPara->ParaFlexMem[FLEX_GC_ADR - 1].ulRecordNumber = FLEX_GC_MAX;
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #2 - FLEX_GC_ADR value > FLEX_GC_MAX\n");
	}

	if (pNewPara->ParaFlexMem[FLEX_CPN_ADR - 1].ulRecordNumber > FLEX_CPN_MAX) {
		pNewPara->ParaFlexMem[FLEX_CPN_ADR - 1].ulRecordNumber = FLEX_CPN_MAX;
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #2 - FLEX_CPN_ADR value > FLEX_CPN_MAX\n");
	}

	if (pNewPara->ParaFlexMem[FLEX_CSTRING_ADR - 1].ulRecordNumber > FLEX_CSTRING_MAX) {
		pNewPara->ParaFlexMem[FLEX_CSTRING_ADR - 1].ulRecordNumber = FLEX_CSTRING_MAX;
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #2 - FLEX_CSTRING_ADR value > FLEX_CSTRING_MAX\n");
	}

	if (pNewPara->ParaFlexMem[FLEX_PROGRPT_ADR - 1].ulRecordNumber > FLEX_PROGRPT_MAX) {
		pNewPara->ParaFlexMem[FLEX_PROGRPT_ADR - 1].ulRecordNumber = FLEX_PROGRPT_MAX;
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #2 - FLEX_PROGRPT_ADR value > FFLEX_PROGRPT_MAX\n");
	}

	if (pNewPara->ParaFlexMem[FLEX_PPI_ADR - 1].ulRecordNumber > FLEX_PPI_MAX) {
		pNewPara->ParaFlexMem[FLEX_PPI_ADR - 1].ulRecordNumber = FLEX_PPI_MAX;
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #2 - FLEX_PPI_ADR value > FLEX_PPI_MAX\n");
	}

	// Ensure that if the file being converted has a smaller MLD size then
	// we will up it to what ever we are currently using.
	if (pNewPara->ParaFlexMem[FLEX_MLD_ADR - 1].ulRecordNumber < MAX_MLDMNEMO_SIZE) {
		pNewPara->ParaFlexMem[FLEX_MLD_ADR - 1].ulRecordNumber = MAX_MLDMNEMO_SIZE;
		fprintf (pLogFile, "ParamFile conversion warning:  Prog. #2 - FLEX_MLD_ADR value > MAX_MLDMNEMO_SIZE\n");
	}

	//Prog. #3, conversion neccessary.
	for( i = 0; i < MAX_FSC_TBL_HGV_2_1_0; i++)
	{
		memcpy(&pNewPara->ParaFSC[i], &pOldPara->ParaFSC[i], sizeof(pOldPara->ParaFSC[i]));
	}

	//Prog. #6
	memcpy(&pNewPara->ParaSecurityNo, &pOldPara->ParaSecurityNo, sizeof(pOldPara->ParaSecurityNo));

	//Prog. #7
	memcpy (pNewPara->ParaStoRegNo, pOldPara->ParaStoRegNo, sizeof(pOldPara->ParaStoRegNo));

	//Prog. #8
	memcpy(&pNewPara->ParaSupLevel, &pOldPara->ParaSupLevel, sizeof(pOldPara->ParaSupLevel));

	//Prog. #9
	memcpy(&pNewPara->ParaActCodeSec, &pOldPara->ParaActCodeSec, sizeof(pOldPara->ParaActCodeSec));

	//Prog. #10
	memcpy(&pNewPara->ParaTransHALO, &pOldPara->ParaTransHALO, sizeof(pOldPara->ParaTransHALO));

	//Prog. #17
	memcpy(&pNewPara->ParaHourlyTime, &pOldPara->ParaHourlyTime, sizeof(pOldPara->ParaHourlyTime));

	//Prog. #18
	memcpy(&pNewPara->ParaSlipFeedCtl, &pOldPara->ParaSlipFeedCtl, sizeof(pOldPara->ParaSlipFeedCtl));


	//LeadThru Mnemonics
	/*
		Para_LeadThru is composed of our current default mnemonics, where HGV20_ParaLeadThru
		is composed of the leadthru values read from the parameter file. If an LDT has no
		value or is empty, szNull , or the LDT is composed of only spaces, szSpaceLDT, we
		will not copy the LDT over the current default mnemonic. If there is a value in the
		LDT other than szNull and szSpaceLDT we will copy it over the default mnemonic. ESMITH
	*/
	//Mnemonics Conversion
	//The new conversion test are exactly the same it only test if the mnemonic is full of
	//spaces or empty, if it is not then it replaces the default mnemonic and if it is empty
	//the default mnemonic stays the same and that is what is printed in PEP. RZACHARY

	//Transaction Mnemonics Conversion
	for( i = 0; i <= MAX_TRANSM_NO_HGV_2_1_0; i++){
		if( !wcsncmp(pOldPara->ParaTransMnemo[i], szNull, PARA_TRANSMNEMO_LEN_HGV_2_1_0) ||
			!wcsncmp(pOldPara->ParaTransMnemo[i], szSpaceTrans, PARA_TRANSMNEMO_LEN_HGV_2_1_0) )
		{/*  | Do Nothing |  */} else {
			wcsncpy(pNewPara->ParaTransMnemo[i], pOldPara->ParaTransMnemo[i],PARA_TRANSMNEMO_LEN_HGV_2_1_0);
		}
	}

	//Lead Thru Mnemonics Conversion
	for( i = 0; i <= MAX_LEAD_NO_HGV_2_1_0; i++) {
		if( !wcsncmp(pOldPara->ParaLeadThru[i], szNull, PARA_LEADTHRU_LEN_HGV_2_1_0) ||
			!wcsncmp(pOldPara->ParaLeadThru[i], szSpaceLDT, PARA_LEADTHRU_LEN_HGV_2_1_0) )
		{ /* Do Nothing */} else {
			wcsncpy(pNewPara->ParaLeadThru[i],pOldPara->ParaLeadThru[i],PARA_LEADTHRU_LEN_HGV_2_1_0);
		}
	}

	//Report Name Mnemonics Conversion
	for( i = 0; i <= MAX_REPO_NO_HGV_2_1_0; i++) {
		if( !wcsncmp(pOldPara->ParaReportName[i], szNull, PARA_REPORTNAME_LEN_HGV_2_1_0) ||
			!wcsncmp(pOldPara->ParaReportName[i], szSpaceLDT, PARA_REPORTNAME_LEN_HGV_2_1_0) )
		{ /* Do Nothing */} else {
			wcsncpy(pNewPara->ParaReportName[i],pOldPara->ParaReportName[i],PARA_REPORTNAME_LEN_HGV_2_1_0);
		}
	}

	//Special Mnemonics Conversion
	for( i = 0; i <= MAX_SPEM_NO_HGV_2_1_0; i++ ){
		if( !wcsncmp(pOldPara->ParaSpeMnemo[i], szNull, PARA_SPEMNEMO_LEN_HGV_2_1_0) ||
			!wcsncmp(pOldPara->ParaSpeMnemo[i], szSpaceSpec, PARA_SPEMNEMO_LEN_HGV_2_1_0) )
		{/* Do Nothing */} 	else {
			wcsncpy(pNewPara->ParaSpeMnemo[i], pOldPara->ParaSpeMnemo[i],PARA_SPEMNEMO_LEN_HGV_2_1_0);
		}
	}

	//Prog. #39
	memcpy(&pNewPara->ParaPCIF, &pOldPara->ParaPCIF, sizeof(pOldPara->ParaPCIF));

	// Print Modifier Mnemonics Conversion
	for(i = 0; i <= MAX_PRTMODI_NO_HGV_2_1_0; i++) {
		if( !wcsncmp(pOldPara->ParaPrtModi[i], szNull, PARA_PRTMODI_LEN_HGV_2_1_0) ||
			!wcsncmp(pOldPara->ParaPrtModi[i], szSpacePri, PARA_PRTMODI_LEN_HGV_2_1_0) )
		{ /* Do Nothing */} else {
			wcsncpy(pNewPara->ParaPrtModi[i],pOldPara->ParaPrtModi[i],PARA_PRTMODI_LEN_HGV_2_1_0);
		}
	}

	//Prog. #48
	memcpy(&pNewPara->ParaMajorDEPT, &pOldPara->ParaMajorDEPT, sizeof(pOldPara->ParaMajorDEPT));

	//Prog. #49
	memcpy(&pNewPara->ParaAutoAltKitch, &pOldPara->ParaAutoAltKitch, sizeof(pOldPara->ParaAutoAltKitch));

	//CPMDATA Prog. #54
	{
		memcpy(pNewPara->ParaHotelId.auchHotelId, &pOldPara->ParaHotelId.auchHotelId,sizeof(pOldPara->ParaHotelId.auchHotelId));

		memcpy(pNewPara->ParaHotelId.auchSld, &pOldPara->ParaHotelId.auchSld,sizeof(pOldPara->ParaHotelId.auchSld));

		memcpy(pNewPara->ParaHotelId.auchProductId, &pOldPara->ParaHotelId.auchProductId,sizeof(pOldPara->ParaHotelId.auchProductId));

		for (j = 0; j < PARA_TEND_CPM_NO_HGV_2_0_1; j++) {
			pNewPara->ParaHotelId.auchTend[j] = pOldPara->ParaHotelId.auchTend[j];
		}
		for (j = 0; j < sizeof(pOldPara->ParaHotelId.auchApproval)/sizeof(pOldPara->ParaHotelId.auchApproval[0]); j++) {
			pNewPara->ParaHotelId.auchApproval[j] = pOldPara->ParaHotelId.auchApproval[j];
		}
	}
	//Prog. #57
	memcpy(&pNewPara->ParaChar24, &pOldPara->ParaChar24, sizeof(pOldPara->ParaChar24));

	//TotalKeyConversion Prog. #60
	memset(&pNewPara->ParaTtlKeyTyp, 0x00, sizeof(pNewPara->ParaTtlKeyTyp));
	memcpy(&pNewPara->ParaTtlKeyTyp, pOldPara->ParaTtlKeyTyp, sizeof(pOldPara->ParaTtlKeyTyp));
/*
	for (i = 0; i < MAX_KEYTYPE_SIZE_HGV_2_0_1; i++) {
		totalKeyOldType = P60_HGV20GETTYPE(pOldPara->HGV20_ParaTtlKeyTyp[i]);

		totalKeyOldIndex= P60_HGV20GETINDEX(pOldPara->HGV20_ParaTtlKeyTyp[i]);

		ParaTtlKeyTyp[i] = P60SETTYPE(totalKeyOldType);
		ParaTtlKeyTyp[i] += P60SETINDEX(totalKeyOldIndex);
	}
*/

	//Prog. #49
	memcpy(&pNewPara->ParaAutoAltKitch, &pOldPara->ParaAutoAltKitch, sizeof(pOldPara->ParaAutoAltKitch));

	//Prog. #61
	memcpy(&pNewPara->ParaTtlKeyCtl, &pOldPara->ParaTtlKeyCtl, sizeof(pOldPara->ParaTtlKeyCtl));

	//Prog. #62
	memcpy(&pNewPara->ParaTend, &pOldPara->ParaTend, sizeof(pOldPara->ParaTend));

	//A/C #4 PLU Menu
	memcpy(&pNewPara->ParaPLUNoMenu, &pOldPara->ParaPLUNoMenu, sizeof(pOldPara->ParaPLUNoMenu));

	//A/C #5
	memcpy(&pNewPara->ParaCtlTblMenu, &pOldPara->ParaCtlTblMenu, sizeof(pOldPara->ParaCtlTblMenu));

	//A/C #6
	memcpy(&pNewPara->ParaManuAltKitch, &pOldPara->ParaManuAltKitch, sizeof(pOldPara->ParaManuAltKitch));

	//A/C #7
	//memcpy(&pNewPara->ParaCashABAssign, &pOldPara->ParaCashABAssign, sizeof(pOldPara->ParaCashABAssign));
	for(i = 0; i < MAX_AB_SIZE; i++){
		pNewPara->ParaCashABAssign[i] = pOldPara->ParaCashABAssign[i];	//version 212 changes from USHORT to ULONG
	}

	//A/C #84
	memcpy(&pNewPara->ParaRound, &pOldPara->ParaRound, sizeof(pOldPara->ParaRound));

	//A/C #86
	memcpy(&pNewPara->ParaDisc, &pOldPara->ParaDisc, sizeof(pOldPara->ParaDisc));

	//A/C #88
	memcpy(&pNewPara->ParaPromotion, &pOldPara->ParaPromotion, sizeof(pOldPara->ParaPromotion));

	//A/C #84
	memcpy(&pNewPara->ParaRound, &pOldPara->ParaRound, sizeof(pOldPara->ParaRound));

	//A/C #89
	memcpy(&pNewPara->ParaCurrency, &pOldPara->ParaCurrency, sizeof(pOldPara->ParaCurrency));

	//A/C #169
	memcpy(&pNewPara->ParaToneCtl, &pOldPara->ParaToneCtl, sizeof(pOldPara->ParaToneCtl));

	//A/C #116 Promotional PLU table
	for (i = 0; i < MAX_SETMENU_NO_HGV_2_1_0; i++) {
		memcpy(&pNewPara->ParaMenuPLU[i], &pOldPara->ParaMenuPLU[i], sizeof(pOldPara->ParaMenuPLU[i]));
	}

	//A/C #124-126
	memcpy(&pNewPara->ParaTaxTable, &pOldPara->ParaTaxTable, sizeof(pOldPara->ParaTaxTable));

	//A/C #127
	memcpy(&pNewPara->ParaTaxRate, &pOldPara->ParaTaxRate, sizeof(pOldPara->ParaTaxRate));

	//MISC PARAMETER
	memcpy(&pNewPara->ParaMiscPara, &pOldPara->ParaMiscPara, sizeof(pOldPara->ParaMiscPara));

	//A/C #111
	memcpy(&pNewPara->ParaTare, &pOldPara->ParaTare, sizeof(pOldPara->ParaTare));

	//Prog. #15
	memcpy(&pNewPara->ParaPresetAmount, &pOldPara->ParaPresetAmount, sizeof(pOldPara->ParaPresetAmount));

	//Prog. #30
	memcpy(&pNewPara->ParaSharedPrt, &pOldPara->ParaSharedPrt, sizeof(pOldPara->ParaSharedPrt));

	//A/C #87
	memcpy(&pNewPara->ParaSoftChk, &pOldPara->ParaSoftChk, sizeof(pOldPara->ParaSoftChk));

	//A/C #130
	memcpy(&pNewPara->ParaPigRule, &pOldPara->ParaPigRule, sizeof(pOldPara->ParaPigRule));

	//A/C #160
	memcpy(&pNewPara->ParaOep, &pOldPara->ParaOep, sizeof(pOldPara->ParaOep));

	//A/C #162
	memcpy(&pNewPara->ParaFlexDrive, &pOldPara->ParaFlexDrive, sizeof(pOldPara->ParaFlexDrive));

	//A/C #133
	memcpy(&pNewPara->ParaServiceTime, &pOldPara->ParaServiceTime, sizeof(pOldPara->ParaServiceTime));

	//A/C #154
	memcpy(&pNewPara->ParaLaborCost, &pOldPara->ParaLaborCost, sizeof(pOldPara->ParaLaborCost));

	//A/C #137
	memcpy(&pNewPara->ParaDispPara, &pOldPara->ParaDispPara, sizeof(pOldPara->ParaDispPara));

	//KDS display addresses
	memcpy(&pNewPara->ParaKdsIp, &pOldPara->ParaKdsIp, sizeof(pOldPara->ParaKdsIp));

	//A/C #170
	memcpy(&pNewPara->ParaRestriction, &pOldPara->ParaRestriction, sizeof(pOldPara->ParaRestriction));

	//A/C #208
	memcpy(&pNewPara->auchParaBoundAge, &pOldPara->auchParaBoundAge, sizeof(pOldPara->auchParaBoundAge));

	//A/C #4
	memcpy(&pNewPara->ParaDeptNoMenu, &pOldPara->ParaDeptNoMenu, sizeof(pOldPara->ParaDeptNoMenu));

	//SPECIAL COUNTER
	memcpy(&pNewPara->ParaSpcCo, &pOldPara->ParaSpcCo, sizeof(pOldPara->ParaSpcCo));

	// A/C 209 - Color Palette
	for (i = 0; i < MAX_COLORPALETTE_SIZE; i++) {
		pNewPara->ParaColorPalette[i][0] = 0x000000 ; // initialize foreground color to black
		pNewPara->ParaColorPalette[i][1] = 0xffffff ; // initialize background color to white
	}

	//CHARGE POST INFORMATION
	memcpy(&pNewPara->ChargePostInformation, &pOldPara->ChargePostInformation, sizeof(pOldPara->ChargePostInformation));

	//Prog. 97 TERMINAL INFORMATION
	for( i = 0; i <= MAX_TERMINALINFO_SIZE_HGV_2_1_0; i++) 
	{
		pNewPara->TerminalInformation[i].ulCodePage = pOldPara->TerminalInformation[i].ulCodePage;
		pNewPara->TerminalInformation[i].usLanguageID = pOldPara->TerminalInformation[i].usLanguageID;
		pNewPara->TerminalInformation[i].usSubLanguageID = pOldPara->TerminalInformation[i].usSubLanguageID;
		pNewPara->TerminalInformation[i].usStoreNumber = pOldPara->TerminalInformation[i].usStoreNumber;
		pNewPara->TerminalInformation[i].usRegisterNumber = pOldPara->TerminalInformation[i].usRegisterNumber;
		pNewPara->TerminalInformation[i].uchKeyBoardType = pOldPara->TerminalInformation[i].uchKeyBoardType;
		pNewPara->TerminalInformation[i].uchMenuPageDefault = pOldPara->TerminalInformation[i].uchMenuPageDefault;
		memcpy(pNewPara->TerminalInformation[i].aszDefaultFontFamily, pOldPara->TerminalInformation[i].aszDefaultFontFamily, sizeof(pOldPara->TerminalInformation[i].aszDefaultFontFamily));
		memcpy(pNewPara->TerminalInformation[i].aszUnLockCode, pOldPara->TerminalInformation[i].aszUnLockCode, sizeof(pOldPara->TerminalInformation[i].aszUnLockCode));
		memcpy(pNewPara->TerminalInformation[i].aszTerminalSerialNumber, pOldPara->TerminalInformation[i].aszTerminalSerialNumber, sizeof(pOldPara->TerminalInformation[i].aszTerminalSerialNumber));
		memcpy(pNewPara->TerminalInformation[i].aszDefaultMenuFileName, pOldPara->TerminalInformation[i].aszDefaultMenuFileName, sizeof(pOldPara->TerminalInformation[i].aszDefaultMenuFileName));
	}
	 
	//EPT
	memcpy(&pNewPara->EPT_ENH_Information, &pOldPara->EPT_ENH_Information, sizeof(pOldPara->EPT_ENH_Information));

	//TENDER KEY INFORMATION
	memcpy(&pNewPara->TenderKeyInformation, &pOldPara->TenderKeyInformation, sizeof(pOldPara->TenderKeyInformation));

	//STORE FORWARD INFORMATION
	//memcpy(&pNewPara->ParaStoreForward, &pOldPara->ParaStoreForward, sizeof(pOldPara->ParaStoreForward));
	//we can't trust the contents of this section in a 210 file, so we blank it out **disabled** in the new file
	memset(&pNewPara->ParaStoreForward, 0x00, sizeof(pNewPara->ParaStoreForward));

	OpPluFileUpdate(0);

	fclose(pLogFile);
	return TRUE;
}
