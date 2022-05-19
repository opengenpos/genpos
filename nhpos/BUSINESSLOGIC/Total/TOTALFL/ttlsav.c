/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *     NHPOS     *             Georgia Southern University       **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 2005-2006       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : TOTAL, Total Function Library Subroutine                             
* Program Name: Ttlsav.c                                                      
* --------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
*       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
*       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
*       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
*       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
*       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
* --------------------------------------------------------------------------
* Abstract:        
*    ULONG TtlWriteSavFile()  - write a totals structure to the SavedTotals file currently open.
*    SHORT TtlSavCpnSavFile() - write a totals structure to the SavedTotals file currently open.
*    VOID TtlCloseSavFile()   - close the SavedTotals file currently open and delete oldest versions.
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data   : Ver.Rev. :   Name      : Description
* Jul-17-06 : 02.01.00 : J.Hall      : Initial version of SavedTotals functionality.
*===========================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===========================================================================
*/
#include <Windows.h>
#include <tchar.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>

#include <ecr.h>
#include <regstrct.h>
#include <pif.h>
#include <paraequ.h>
#include <para.h>
#include <csttl.h>
#include <ttl.h>
#include <csop.h>
#include <..\report\rptcom.h>
#include <appllog.h>
#include <rfl.h>
#include <csstbopr.h>
#include <csstbttl.h>
#include <cscas.h>
#include <report.h>


/*
*============================================================================
**Synopsis:     ULONG TtlSavCreateIndexFile(SAVTTLFILEINDEX *IndexFile)
*
*    Input:     USHORT hsHandle         - File Handle
*               ULONG ulPosition        - Read File Position
*               VOID *pTmpBuff          - Pointer of Read File Data Buffer
*               USHORT usSize           - Read Buffer Size
*
*   Output:     VOID *pTmpBuff          - Read File Data
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed Reading Traget  
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  Create the SavedTotals file header calculating the various offsets
*               into the file where the various totals records will be stored.
*
*============================================================================
*/
static ULONG  TtlSavCreateIndexFile(SAVTTLFILEINDEX *IndexFile)
{
	DATE_TIME	dt = {0};
	USHORT	    i;

	PifGetDateTime(&dt);
	IndexFile->dtCreationDate.usMin		= dt.usMin;
	IndexFile->dtCreationDate.usHour	= dt.usHour;
	IndexFile->dtCreationDate.usMDay	= dt.usMDay;
	IndexFile->dtCreationDate.usMonth	= dt.usMonth;
	IndexFile->usYear					= dt.usYear;

	// Starting with an offset after the Index information in the SavedTotals file
	// populate the header index information so that the various fields in the SavedTotals
	// file can be located.  We do this in the order they are in the SAVTTLFILEINDEX struct.
	IndexFile->ulFileSize = sizeof(SAVTTLFILEINDEX);
	for(i = 0; i < STD_NUM_OF_TERMINALS; i++)
	{
		IndexFile->ulIndTerminalOffset[i] = IndexFile->ulFileSize;
		IndexFile->ulFileSize += sizeof(TTLCSREGFIN);
	}

	IndexFile->ulFinancialTotalOffset = IndexFile->ulFileSize;
	IndexFile->ulFileSize += sizeof(TTLCSREGFIN);

	IndexFile->ulDeptTotalOffset = IndexFile->ulFileSize;
	// plus one to hold the RPT_EOF record indicating end of list. See function RptEODOnlyReset().
	IndexFile->ulFileSize += (sizeof(DEPTTBL) * (FLEX_DEPT_MAX + 1));

	IndexFile->ulHourlyTotalOffset = IndexFile->ulFileSize;
	IndexFile->ulFileSize += sizeof(TTLCSHOURLY);

	IndexFile->ulServiceTotalOffset = IndexFile->ulFileSize;
	IndexFile->ulFileSize += sizeof(TTLCSSERTIME);
	for( i = 0; i < FLEX_CAS_MAX; i++)
	{
		IndexFile->ulCashierTotalOffset[i] = IndexFile->ulFileSize;
		IndexFile->ulFileSize += sizeof(TTLCASHIER);
	}
	IndexFile->ulCouponTotalOffset = IndexFile->ulFileSize;
	IndexFile->ulFileSize += (sizeof(CPNTBL) * FLEX_CPN_MAX);

	IndexFile->ulPluTotalBeginOffset = IndexFile->ulFileSize;
	IndexFile->ulPLUTotalCounter = 0;

	return 1;
}


/*
*============================================================================
**Synopsis:     SHORT TtlCreateSavFile(VOID)
*
*    Input:     USHORT hsHandle         - File Handle
*               ULONG ulPosition        - Read File Position
*               VOID *pTmpBuff          - Pointer of Read File Data Buffer
*               USHORT usSize           - Read Buffer Size
*
*   Output:     VOID *pTmpBuff          - Read File Data
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed Reading Traget  
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  This function reads the Traget.
*
*============================================================================
*/
static SHORT  TtlCreateSavFile(TTLCSREGFIN *pMyTotal)
{
    ULONG   ulActualPosition;   //JHHJ 11-10-03/* Actual Seek Pointer */
	TCHAR	aszFileName[100], *aszFileNameStandard = _T("SavedTotals");
	SHORT   sRetvalue;
	SAVTTLFILEINDEX newIndex = {0};
    
    /* return no. of satellite from MDC, saratoga */
    if(hsTtlSavFileHandle < 0)
	{
		TtlSavCreateIndexFile(&newIndex);

		_stprintf(aszFileName, _T("%s_%2.2d_%2.2d_%2.2d_%2.2d_TO_%2.2d_%2.2d_%2.2d_%2.2d.sav"),
					aszFileNameStandard, pMyTotal->FromDate.usMonth, pMyTotal->FromDate.usMDay,
					pMyTotal->FromDate.usHour, pMyTotal->FromDate.usMin, pMyTotal->ToDate.usMonth,
					pMyTotal->ToDate.usMDay, pMyTotal->ToDate.usHour, pMyTotal->ToDate.usMin); 

		hsTtlSavFileHandle = PifOpenFile(aszFileName, auchNEW_FILE_READ_WRITE_SAVTTL);   /* Create the Base Total File */
		if (hsTtlSavFileHandle == PIF_ERROR_FILE_EXIST)
		{
			PifLog (MODULE_TTL_TFL, LOG_EVENT_TTL_SAVEFILE_SAVED_NEW);
			hsTtlSavFileHandle = PifOpenFile(aszFileName, auchOLD_FILE_READ_WRITE_SAVTTL);
			if (hsTtlSavFileHandle >= 0)
			{
				return (TTL_SUCCESS); 
			}
		}

		if (hsTtlSavFileHandle < 0)
		{
			PifLog (MODULE_TTL_TFL, LOG_EVENT_TTL_SAVEFILE_SAVED_OLD);
			return (TTL_SUCCESS); 
		}

		/* calculate file size */
		/* Set Update Total File Size(? bytes) */
		sRetvalue = PifSeekFile(hsTtlSavFileHandle, newIndex.ulFileSize, &ulActualPosition);
		if ((sRetvalue < 0) || (newIndex.ulFileSize != ulActualPosition))
		{ 
			PifCloseFile(hsTtlSavFileHandle);
			hsTtlSavFileHandle = -1;
			PifDeleteFile(aszFileName); 
			PifLog (MODULE_TTL_TFL, LOG_EVENT_TTL_SAVEFILE_SAVFILE_SIZE);
			return(TTL_FAIL);
		}else
		{
			PifSeekFile(hsTtlSavFileHandle, 0, &ulActualPosition);
			PifWriteFile(hsTtlSavFileHandle,&newIndex, sizeof(newIndex));
		}
	}

    return (TTL_SUCCESS);           /* Return Success */
}

/*
*============================================================================
**Synopsis:     SHORT TtlWriteSavFile(VOID)
*
*    Input:     USHORT hsHandle         - File Handle
*               ULONG ulPosition        - Read File Position
*               VOID *pTmpBuff          - Pointer of Read File Data Buffer
*               USHORT usSize           - Read Buffer Size
*
*   Output:     VOID *pTmpBuff          - Read File Data
*    InOut:     Nothing
*
** Return:      TTL_SUCCESS         - Successed Reading Traget  
*               TTL_FILE_SEEK_ERR   - Occured File Seek Error
*               TTL_FILE_READ_ERR   - Occured File Read Error
*
** Description  Write the specified totals record to the SavedTotals file.
*               If there is not a currently opened SavedTotals file then we
*               will create a new one using the date and time from the totals
*               record to use in naming the file.  The file name format is
*               SavedTotals_mm_dd_hh_mm_TO_nn_dd_hh_mm.sav where mm_dd is the
*               month and day and hh_mm is the hour and minute.  There are two
*               fields used, the From date/time and the To date/time.
*
*               When we create and open up the SavedTotals file we also initialize
*               the operator totals count to zero.  This will ensure that the
*               operator totals index will be correctly starting at zero each time
*               a new SavedTotals file is generated.
*
*               The PCSample application has functionality to transfer from a GenPOS
*               terminal to a machine running PCSample as a remote application a
*               SavedTotals file.  The function to read the SavedTotals file
*               is BOOL CSavedTotal::Read(const UCHAR uchClassToRead).
*               See also void CMainFrame::OnReadSavTotal() in
*============================================================================
*/
ULONG TtlWriteSavFile(UCHAR uchClass, VOID *puchTmpBuff, ULONG ulFileSize, USHORT usNumber)
{
	ULONG			ulActualBytesRead;
	SAVTTLFILEINDEX IndexFile;
	static USHORT   usCashierTotalIndex = 0;

	if ( hsTtlSavFileHandle < 0 )
	{
		usCashierTotalIndex = 0;
		TtlCreateSavFile(puchTmpBuff);
	}

	if (hsTtlSavFileHandle < 0)
	{
		PifLog (MODULE_TTL_TFL, LOG_EVENT_TTL_SAVEFILE_WRT_BADHNDLE);
		return 0;
	}

	TtlReadFileFH(0, &IndexFile, sizeof(SAVTTLFILEINDEX), hsTtlSavFileHandle, &ulActualBytesRead);    /* read data from interfaced file handle */
	NHPOS_ASSERT(ulActualBytesRead == sizeof(SAVTTLFILEINDEX));
    switch(uchClass) {

	case CLASS_TTLHOURLY:
		NHPOS_ASSERT(ulFileSize == sizeof(TTLCSHOURLY));
		TtlWriteFile (hsTtlSavFileHandle, IndexFile.ulHourlyTotalOffset, puchTmpBuff, ulFileSize);
		break;

	case CLASS_TTLEODRESET:         /* Reset EOD Total File */
		NHPOS_ASSERT(ulFileSize == sizeof(TTLCSREGFIN));
		TtlWriteFile (hsTtlSavFileHandle, IndexFile.ulFinancialTotalOffset, puchTmpBuff, ulFileSize);
        break;
    
    case CLASS_TTLCPN:               /* Reset Coupon Total File, R3.0 */
		NHPOS_ASSERT(ulFileSize <= FLEX_CPN_MAX * sizeof(CPNTBL));
		TtlWriteFile (hsTtlSavFileHandle, IndexFile.ulCouponTotalOffset, puchTmpBuff, ulFileSize);
		break;
    
    case CLASS_TTLCASHIER:          /* Reset Cashier Total File */
		NHPOS_ASSERT_TEXT((usCashierTotalIndex < FLEX_CAS_MAX), "**ERROR: IndexFile.ulCashierTotalOffset[usNumber] usNumber out of range.");
		NHPOS_ASSERT(ulFileSize == sizeof(TTLCASHIER));
		if (usCashierTotalIndex < FLEX_CAS_MAX) {
			TtlWriteFile (hsTtlSavFileHandle, IndexFile.ulCashierTotalOffset[usCashierTotalIndex], puchTmpBuff, ulFileSize);
			usCashierTotalIndex++;
		}
        break;

    /* ===== New Functions (Release 3.1) BEGIN ===== */
    case CLASS_TTLSERVICE:          /* Reset Service Time File */
		NHPOS_ASSERT(ulFileSize == sizeof(TTLCSSERTIME));
		TtlWriteFile (hsTtlSavFileHandle, IndexFile.ulServiceTotalOffset, puchTmpBuff, ulFileSize);
        break;

    case CLASS_TTLINDFIN:           /* Reset Individual Financial File */
		NHPOS_ASSERT_TEXT((usNumber < STD_NUM_OF_TERMINALS), "**ERROR: IndexFile.ulIndTerminalOffset[usNumber] usNumber out of range.");
		NHPOS_ASSERT(ulFileSize == sizeof(TTLCSREGFIN));
		if (usNumber < STD_NUM_OF_TERMINALS) {
			TtlWriteFile (hsTtlSavFileHandle, IndexFile.ulIndTerminalOffset[usNumber], puchTmpBuff, ulFileSize);
		}
        break;

	case CLASS_TTLDEPT:
		NHPOS_ASSERT(ulFileSize <=  (FLEX_DEPT_MAX + 1) * sizeof(DEPTTBL));    // see function RptEODOnlyReset() where this table is created.
		TtlWriteFile (hsTtlSavFileHandle, IndexFile.ulDeptTotalOffset, puchTmpBuff, ulFileSize);
		break;

	case CLASS_TTLPLU:
		{
			ULONG   ulOffset = IndexFile.ulPluTotalBeginOffset;
			ulOffset += (IndexFile.ulPLUTotalCounter * sizeof(TTLPLU));

			NHPOS_ASSERT(ulFileSize == sizeof(TTLPLU));
			//we need to right the PLU file information here.
			TtlWriteFile (hsTtlSavFileHandle, ulOffset, puchTmpBuff, ulFileSize);
			
			IndexFile.ulPLUTotalCounter++;
			TtlWriteFile (hsTtlSavFileHandle, 0, &IndexFile, sizeof(IndexFile));
		}
		break;
		
    /* ===== New Functions (Release 3.1) END ===== */
        
    default:
        break;
    }

	return 1;
}
/*
*===========================================================================
**  Synopsis: SHORT RptCpnAllProc(UCHAR uchMinorClass)
*               
*       Input:  UCHAR  uchMinorClass   : CLASS_TTLCURDAY
*                                      : CLASS_TTLCURPTD
*                                      : CLASS_TTLSAVDAY
*                                      : CLASS_TTLSAVPTD
*
*      Output:  nothing
*
**  Return: SUCCESS      
*           RPT_ABORTED
*           LDT_ERR_ADR     (TTL_FAIL)
*           LDT_PROHBT_ADR  (OP_FILE_NOT_FOUND)
* 
**  Description: Write coupon totals to the current Saved Totals File.
*
*                 NOTE: this function is desigend to be used for End of Day
*                       and it reads the coupon totals and then
*                       generates the coupons section of the Saved Totals File.
*
*                       See function TtlSavCpnSavFileBuff() below for use when
*                       the coupon totals have already been read.
*
*                 WARNING: function SerTtlTotalRead() accesses the CLI semaphore
*                       which means deadlock if this function is called when a
*                       thread already has the semaphore. This function is really
*                       intended to be used by the End Of Day Report only.
*
*===========================================================================
*/
SHORT  TtlSavCpnSavFile(UCHAR uchMinorClass, UCHAR uchType)
{
	ULONG    ulNoOfCoupons;
	SHORT    sRetStatus = SUCCESS;
    USHORT   usCpnNumber;
	CPNTBL	 Table[FLEX_CPN_MAX] = {0};

    if (uchRptMldAbortStatus) {                         /* aborted by MLD */
        return (RPT_ABORTED);
    }

	ulNoOfCoupons = RflGetMaxRecordNumberByType (FLEX_CPN_ADR);
    for (usCpnNumber = 0; usCpnNumber < ulNoOfCoupons; usCpnNumber++){  /* Saratoga */
		CPNIF    CpnIf = {0};
		TTLCPN   TtlCpn = {0};
		SHORT    sReturn;

		TtlCpn.uchMajorClass = CLASS_TTLCPN;            /* Set Major Class for TtlCpn */
		TtlCpn.uchMinorClass = CLASS_TTLSAVDAY;           /* Set Minor Class for TtlCpn */
        TtlCpn.usCpnNumber = usCpnNumber + 1;                              /* Set Cpn No for TtlCpn     */
        if ((sReturn = SerTtlTotalRead(&TtlCpn)) != TTL_SUCCESS) {         /* Call Func for Total data    */
			// if there is an error then we want to report the error but we also want
			// to write out the data that we have.
            sRetStatus = TtlConvertError(sReturn);
			break;
        }
		CpnIf.uchCpnNo = TtlCpn.usCpnNumber;            /* Coupon Number Set */
		CliOpCpnIndRead(&CpnIf, 0);                     /* Mnemonic Get */
		Table[usCpnNumber].usCpnNo = TtlCpn.usCpnNumber;
		Table[usCpnNumber].CPNTotal = TtlCpn.CpnTotal;
		_tcsncpy(Table[usCpnNumber].auchMnemo, CpnIf.ParaCpn.aszCouponMnem, PARA_CPN_LEN); /* Copy Mnemonics    */
    }

	TtlWriteSavFile(CLASS_TTLCPN, &Table, sizeof(Table), 0);

    return(sRetStatus);
}

SHORT  TtlSavCpnSavFileBuff(TOTAL *pTotal, USHORT usMaxCpn)
{
	ULONG    ulNoOfCoupons;
	SHORT    sRetStatus = SUCCESS;
	USHORT   usCpnNumber;
	CPNTBL	 Table[FLEX_CPN_MAX] = { 0 };

	if (uchRptMldAbortStatus) {                         /* aborted by MLD */
		return (RPT_ABORTED);
	}

	ulNoOfCoupons = RflGetMaxRecordNumberByType(FLEX_CPN_ADR);
	if (usMaxCpn > ulNoOfCoupons) usMaxCpn = ulNoOfCoupons;
	for (usCpnNumber = 0; usCpnNumber < usMaxCpn; usCpnNumber++) {  /* Saratoga */
		CPNIF    CpnIf = { 0 };

		CpnIf.uchCpnNo = usCpnNumber + 1;                  /* Coupon Number Set */
		CliOpCpnIndRead(&CpnIf, 0);                     /* Mnemonic Get */
		Table[usCpnNumber].usCpnNo = CpnIf.uchCpnNo;
		Table[usCpnNumber].CPNTotal = pTotal[usCpnNumber];
		_tcsncpy(Table[usCpnNumber].auchMnemo, CpnIf.ParaCpn.aszCouponMnem, PARA_CPN_LEN); /* Copy Mnemonics    */
	}

	TtlWriteSavFile(CLASS_TTLCPN, &Table, sizeof(Table), 0);

	return(sRetStatus);
}

static SHORT  TtlSavDeleteOldFiles(USHORT  usDaysOld)
{
	TCHAR            CurrentDir[512], *pCurrentDir;
	SYSTEMTIME       mySystemTime;
	FILETIME         myFileTime;
	WIN32_FIND_DATA  myFoundFile;
	HANDLE           fileSearch;
	TCHAR            *myFileName = _T("*.sav");

	//we need to change to this directory to look for the files
	pCurrentDir = _tgetcwd (CurrentDir, sizeof(CurrentDir)/sizeof(CurrentDir[0]));
	_tchdir(STD_FOLDER_SAVTTLFILES);

	GetSystemTime (&mySystemTime);

	if (mySystemTime.wMonth > 2) {
		mySystemTime.wMonth -= 2;
	}
	else {
		mySystemTime.wYear--;
		mySystemTime.wMonth = 13 - mySystemTime.wMonth;
	}
	mySystemTime.wDay = 1;

	SystemTimeToFileTime (&mySystemTime, &myFileTime);

	fileSearch = FindFirstFile (myFileName, &myFoundFile); //find the first file

	while (fileSearch != INVALID_HANDLE_VALUE)
	{
		if (0 >= CompareFileTime (&myFoundFile.ftLastWriteTime, &myFileTime)) {
			DWORD   dwLastError;

			if (0 == DeleteFile(myFoundFile.cFileName) ) {
				dwLastError = GetLastError ();
			}
		}

		if (0 == FindNextFile (fileSearch, &myFoundFile)) {
			break;
		}
	}

	FindClose (fileSearch);

	if (pCurrentDir)
		_tchdir(pCurrentDir);
	return 0;
}


/*
*============================================================================
**Synopsis:     SHORT TtlCloseSavFile(VOID)
*
** Description  This function closes the Saved Totals File
*
*============================================================================
*/
VOID TtlCloseSavFile(VOID)
{
	if (hsTtlSavFileHandle >= 0) {
		PifCloseFile(hsTtlSavFileHandle);
		hsTtlSavFileHandle = -1;
	}

	TtlSavDeleteOldFiles(60);
}
