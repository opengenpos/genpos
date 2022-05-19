/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO   ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1995      ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server OPERATIONAL PARAMATER module, Internal subroutine
*   Category           : Client/Server OPERATIONAL PARAMATER module, NCR2170 US HOSPITALITY APPLICATION
*   Program Name       : NWOPIN.C
*  ------------------------------------------------------------------------
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.
*   Memory Model       : Midium Model
*   Options            : /c /AM /Gs /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*   Abstract           :
*   Op_LockCheck                * Lock Check.
*   Op_WriteDeptFile            * Write data in Dept File.
*   Op_ClosePluFileReleaseSem   * Close PLU file and Release semaphore
*   Op_CloseDeptFileReleaseSem  * Close DEPT file and Release semaphore
*   Op_ReadDeptFile             * Read data in Dept file
*   Op_CompSubIndex             * Compare Sub index entry
*   OpReqDept                   * Request    department file
*   OpResDept                   * Ressponse  department file
*   Op_AllFileCreate        C   * Creates all files
*
*   Op_PluAbort()           A   * PifAbort
*   Op_DeptOpenFile()       A   * Open DEPT File
*
*   OpReqCpn                    * Request    Combination Coupon file
*   OpResCpn                    * Ressponse  Combination Coupon file
*   Op_CpnOpenFile              * Open Combination Coupon File
*   Op_WriteCpnFile             * Write data in Combination Coupon File.
*   Op_CloseCpnFileReleaseSem   * Close Combination Coupon file and Release semaphore
*   Op_ReadCpnFile              * Read data in Combination Coupon file
*   OpReqCstr                   * Request    Control String file
*   OpResCstr                   * Ressponse  Control String file
*   Op_CstrOpenFile             * Open Control String File
*   Op_WriteCstrFile            * Write data in Control String File.
*   Op_CloseCstrFileReleaseSem  * Close Control String file and Release semaphore
*   Op_ReadCstrFile             * Read data in Control String file
*
*   OpReqPpi                    * Request Promotional Pricing Item File, R3.1
*   OpResPpi                    * Response Promotional Pricing Item File R3.1
*   OpPpiAbort                  * PifAbort                               R3.1
*   Op_PpiOpenFile              * Open Promotianal Pricing Item File,    R3.1
*   Op_ClosePpiFileReleaseSem   * Close PPI file and Release semaphore,  R3.1
*   Op_ReadPpiFile              * Read data in PPI file,                 R3.1
*   Op_WritePpiFile             * Write data in PPI File.                R3.1
*
*   OpReqOep                    * Request    Oep File,                   Add 2172 Rel 1.0
*   OpResOep                    * Ressponse  Oep File,                   Add 2172 Rel 1.0
*   Op_OepOpenFile              * Open Oep File,                         Add 2172 Rel 1.0
*   Op_CloseOepFileReleaseSem   * Close Oep File and Release semaphore,  Add 2172 Rel 1.0
*   Op_ReadOepFile              * Read data in Oep File,                 Add 2172 Rel 1.0
*   Op_WriteOepFile             * Write data in Oep File,                Add 2172 Rel 1.0 
*   Op_OepIndex                 * Check or Add and change index entry,   Add 2172 Rel 1.0
*   Op_OepCompIndex             * Compare index entry,                   Add 2172 Rel 1.0
*   Op_OepSearchFile            * Search data on the Table in a File,    Add 2172 Rel 1.0
*   Op_OepInsertTable           * Insert data to Table in a File,        Add 2172 Rel 1.0
*
*  ------------------------------------------------------------------------
*    Update Histories  :
*   Date       Ver.Rev :NAME       :Description
*   Nov-03-93 :00.00.01:H.Yamaguchi:Increase PLU 1000 --> 3000
*   Feb-27-95 :03.00.00:H.Ishida   :Add Process
*             :        :           :   OpReqCpn, OpResCpn, Op_CpnAbort, Op_CpnOpenFile,
*             :        :           :   Op_WriteCpnFile, Op_CloseCpnFileReleaseSem, Op_ReadCpnFile
*   Mar-31-95 :03.00.00:H.Ishida   :Add Process
*             :        :           :   OpReqCstr, OpResCstr, Op_CstrAbort, Op_CstrOpenFile,
*             :        :           :   Op_WriteCstrFile, Op_CloseCstrFileReleaseSem, Op_ReadCstrFile
*   Nov-22-95 :03.01.00:T.Nakahata :R3.1 Initial (Add PPI File)
*   Aug-17-98 :03.03.00:M.Ozawa    : Add MLD Mnemonics File
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
\*=======================================================================*/
/*=======================================================================*\
:   PVCS ENTRY
:-------------------------------------------------------------------------
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/
#include	<tchar.h>
#include    <string.h>
#include    <stdio.h>
#include    <math.h>

#include    <ecr.h>
#include    <pif.h>
#include    <log.h>
#include    <rfl.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csop.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    <csstbstb.h>
#include    <csstbpar.h>
#include    <appllog.h>
#include    <applini.h>
#include    <ej.h>
#include    "csopin.h"
#include    "csttl.h"
#include    "csgcs.h"

//static SHORT (*pComp)(VOID *pKey, VOID *pusHPoint) = Op_CompIndex;    /* Add R3.0 */
//static SHORT (*pCompSub)(VOID *pKey, VOID *pusHPoint) = Op_CompSubIndex;    /* Add R3.0 */
static SHORT (*pCompDept)(VOID *pKey, VOID *pusHPoint) = Op_DeptCompIndex;    /* Add R3.0 */

static UCHAR FARCONST auchFlexMem[] =    {FLEX_DEPT_ADR,
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

/*
*==========================================================================
**    Synopsis:    SHORT   Op_LockCheck(USHORT usLockHnd)
*
*       Input:  usLockHnd    Lock handle that is getted by OpLock().
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return    : OP_PERFORM
*               OP_LOCK
*
**  Description: Lock Check. 
*==========================================================================
*/
SHORT   Op_LockCheck(USHORT usLockHnd)
{
    if (( husOpLockHandle == 0x0000 ) || ( husOpLockHandle == usLockHnd )) {
         return(OP_PERFORM);
    }
    return(OP_LOCK);
}

/*
*==========================================================================
**    Synopsis:    VOID    Op_WriteOepFile(ULONG  offulFileSeek, 
*                                          VOID   *pTableHeadAddress, 
*                                          USHORT usSizeofWrite)
*
*       Input:   offulFileSeek           Number of bytes from the biginning of the oep file.
*                *pTableHeadAddress      Pointer to data to be written.
*                usSizeofWrite           Number of bytes to be written.
*      Output:   Nothing
*       InOut:   Nothing
*
**  Return    :  Nothing (error is system error (progrum is abort)
*
**  Description: Write data in Oep File.
*==========================================================================
*/
VOID   Op_WriteOepFile(ULONG offulFileSeek, 
                        VOID  *pTableHeadAddress, 
                        USHORT usSizeofWrite)
{
    ULONG   ulActualPosition;

    if (!usSizeofWrite) {
        return;
    }

    (PifSeekFile(hsOpOepFileHandle, offulFileSeek, &ulActualPosition) < 0)
    ? PifAbort(MODULE_OP_OEP, FAULT_ERROR_FILE_SEEK) : PifWriteFile(hsOpOepFileHandle,
                                                         pTableHeadAddress, 
                                                         usSizeofWrite);
}
/*

*==========================================================================
**    Synopsis:    SHORT   Op_WriteDeptFile(ULONG  offulFileSeek, 
*                                           VOID   *pTableHeadAddress, 
*                                           USHORT usSizeofWrite)
*
*       Input:   offulFileSeek           Number of bytes from the biginning of the dept file.
*                *pTableHeadAddress      Pointer to data to be written.
*                usSizeofWrite           Number of bytes to be written.
*      Output:   Nothing
*       InOut:   Nothing
*
**  Return    :  Nothing (error is system error (progrum is abort)
*
**  Description: Write data in Dept File.
*==========================================================================
*/
VOID   Op_WriteDeptFile(ULONG offulFileSeek, 
                        VOID  *pTableHeadAddress, 
                        USHORT usSizeofWrite)
{
    ULONG   ulActualPosition;

    if (!usSizeofWrite) {
        return;
    }

    (PifSeekFile(hsOpDeptFileHandle, offulFileSeek, &ulActualPosition) < 0)
    ? PifAbort(MODULE_OP_DEPT, FAULT_ERROR_FILE_SEEK) : PifWriteFile(hsOpDeptFileHandle,
                                                         pTableHeadAddress, 
                                                         usSizeofWrite);
}

/*
*==========================================================================
**    Synopsis:    SHORT   Op_WriteCpnFile(ULONG  offulFileSeek, 
*                                           VOID   *pTableHeadAddress, 
*                                           USHORT usSizeofWrite)
*
*       Input:   offulFileSeek           Number of bytes from the biginning of the 
*                                        Combination Coupon file.
*                *pTableHeadAddress      Pointer to data to be written.
*                usSizeofWrite           Number of bytes to be written.
*      Output:   Nothing
*       InOut:   Nothing
*
**  Return    :  Nothing (error is system error (progrum is abort)
*
**  Description: Write data in Combination Coupon File.
*==========================================================================
*/
VOID   Op_WriteCpnFile(ULONG offulFileSeek, 
                        VOID  *pTableHeadAddress, 
                        USHORT usSizeofWrite)
{
    ULONG   ulActualPosition;

    if (!usSizeofWrite) {
        return;
    }

    (PifSeekFile(hsOpCpnFileHandle, offulFileSeek, &ulActualPosition) < 0)
    ? PifAbort(MODULE_OP_CPN, FAULT_ERROR_FILE_SEEK) : PifWriteFile(hsOpCpnFileHandle, pTableHeadAddress, usSizeofWrite);
}

/*
*==========================================================================
**    Synopsis:    SHORT   Op_WriteCstrFile(ULONG  offulFileSeek, 
*                                            VOID   *pTableHeadAddress, 
*                                            USHORT usSizeofWrite)
*
*       Input:   offulFileSeek           Number of bytes from the biginning of the 
*                                        Control String file.
*                *pTableHeadAddress      Pointer to data to be written.
*                usSizeofWrite           Number of bytes to be written.
*      Output:   Nothing
*       InOut:   Nothing
*
**  Return    :  Nothing (error is system error (progrum is abort)
*
**  Description: Write data in Control String File.
*==========================================================================
*/
VOID   Op_WriteCstrFile(ULONG offulFileSeek, 
                        VOID  *pTableHeadAddress, 
                        USHORT usSizeofWrite)
{
    ULONG   ulActualPosition;

    if (!usSizeofWrite) {
        return;
    }

    (PifSeekFile(hsOpCstrFileHandle, offulFileSeek, &ulActualPosition) < 0)
    ? PifAbort(MODULE_OP_CSTR, FAULT_ERROR_FILE_SEEK) : PifWriteFile(hsOpCstrFileHandle, pTableHeadAddress, usSizeofWrite);
}

/*
*==========================================================================
**    Synopsis:    VOID    Op_ClosePluFileReleaseSem(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing 
*
**  Description:  Close PLU file and Release semaphore
*==========================================================================
*/
VOID    Op_ClosePluFileReleaseSem(VOID)
{
    PifReleaseSem(husOpSem);
}
/*
*==========================================================================
**    Synopsis:    VOID    Op_CloseOepFileReleaseSem(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing 
*
**  Description:
*               Close OEP file and Release semaphore
*==========================================================================
*/
VOID    Op_CloseOepFileReleaseSem(VOID)
{
    PifCloseFile(hsOpOepFileHandle);
	hsOpOepFileHandle = -1;
    PifReleaseSem(husOpSem);
}
/*
*==========================================================================
**    Synopsis:    VOID    Op_CloseDeptFileReleaseSem(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing 
*
**  Description:
*               Close DEPT file and Release semaphore
*==========================================================================
*/
VOID    Op_CloseDeptFileReleaseSem(VOID)
{
    PifCloseFile(hsOpDeptFileHandle);
    PifReleaseSem(husOpSem);
}

/*
*==========================================================================
**    Synopsis:    VOID    Op_CloseCpnFileReleaseSem(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing 
*
**  Description:
*               Close Combination Coupon file and Release semaphore
*==========================================================================
*/
VOID    Op_CloseCpnFileReleaseSem(VOID)
{
    PifCloseFile(hsOpCpnFileHandle);
    PifReleaseSem(husOpSem);
}

/*
*==========================================================================
**    Synopsis:    VOID    Op_CloseCstrFileReleaseSem(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing 
*
**  Description:
*               Close Control String file and Release semaphore
*==========================================================================
*/
VOID    Op_CloseCstrFileReleaseSem(VOID)
{
    PifCloseFile(hsOpCstrFileHandle);
    PifReleaseSem(husOpSem);
}

/*
*==========================================================================
**    Synopsis:  SHORT Op_ReadDeptFile(ULONG  offulFileSeek, 
*                                  VOID   *pTableHeadAddress, 
*                                  ULONG  ulSizeofRead);
*
*       Input:  offulFileSeek           Number of bytes from the biginning of the dept file.
*               ulSizeofRead           Number of bytes to be read.
*      Output:  *pTableHeadAddress      Storage location of items to be read.
*       InOut:  Nothing
*
**  Return    : OP_PERFORM
*               OP_NO_READ_SIZE                  Read size is 0
*               (File error = system halt)
*
**  Description: Read data in Dept file.          
*==========================================================================
*/
SHORT   Op_ReadDeptFile(ULONG offulFileSeek, 
                        VOID  *pTableHeadAddress, 
                        ULONG ulSizeofRead)
{
    ULONG   ulActualPosition;
	ULONG	ulActualBytesRead;	//RPH 11-10-3

    if (ulSizeofRead == 0X00) {
        return (OP_NO_READ_SIZE);
    }
    
    if ((PifSeekFile(hsOpDeptFileHandle, offulFileSeek, &ulActualPosition)) < 0) {
        PifAbort(MODULE_OP_DEPT, FAULT_ERROR_FILE_SEEK);
    }
	
	//RPH 11-10-3 Changes for PifReadFile
	PifReadFile(hsOpDeptFileHandle, pTableHeadAddress, ulSizeofRead, &ulActualBytesRead);
    if ( ulActualBytesRead != ulSizeofRead) {
        PifAbort(MODULE_OP_DEPT, FAULT_ERROR_FILE_READ);
    }
    return(OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:  SHORT Op_ReadCpnFile(ULONG  offulFileSeek, 
*                                  VOID   *pTableHeadAddress, 
*                                  ULONG  ulSizeofRead);
*
*       Input:  offulFileSeek           Number of bytes from the biginning of
*                                       the Combination Coupon file.
*               ulSizeofRead            Number of bytes to be read.
*      Output:  *pTableHeadAddress      Storage location of items to be read.
*       InOut:  Nothing
*
**  Return    : OP_PERFORM
*               OP_NO_READ_SIZE                  Read size is 0
*               (File error = system halt)
*
**  Description: Read data in Conbination Coupon file.          
*==========================================================================
*/
SHORT   Op_ReadCpnFile(ULONG offulFileSeek, 
                        VOID  *pTableHeadAddress, 
                        ULONG ulSizeofRead)
{
    ULONG   ulActualPosition;
	ULONG	ulActualBytesRead; //RPH 11-10-3

    if (ulSizeofRead == 0X00) {
        return (OP_NO_READ_SIZE);
    }
    
    if ((PifSeekFile(hsOpCpnFileHandle, offulFileSeek, &ulActualPosition)) < 0) {
        PifAbort(MODULE_OP_CPN, FAULT_ERROR_FILE_SEEK);
    }
	
	//RPH 11-10-3 Changes for PifReadFile
	PifReadFile(hsOpCpnFileHandle, pTableHeadAddress, ulSizeofRead, &ulActualBytesRead);
    if ( ulActualBytesRead != ulSizeofRead) {
        PifAbort(MODULE_OP_CPN, FAULT_ERROR_FILE_READ);
    }
    return(OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:   SHORT Op_ReadCstrFile(ULONG  offulFileSeek, 
*                                  VOID   *pTableHeadAddress, 
*                                  ULONG ulSizeofRead);
*
*       Input:  offulFileSeek           Number of bytes from the biginning of
*                                       the Control String file.
*               ulSizeofRead            Number of bytes to be read.
*      Output:  *pTableHeadAddress      Storage location of items to be read.
*       InOut:  Nothing
*
**  Return    : OP_PERFORM
*               OP_NO_READ_SIZE                  Read size is 0
*               (File error = system halt)
*
**  Description: Read data in Control String file.          
*==========================================================================
*/
SHORT   Op_ReadCstrFile(ULONG offulFileSeek, 
                        VOID  *pTableHeadAddress, 
                        ULONG ulSizeofRead)
{
    ULONG   ulActualPosition;
	ULONG	ulActualBytesRead;	//RPH 11-10-3

    if (ulSizeofRead == 0X00) {
        return (OP_NO_READ_SIZE);
    }
    
    if ((PifSeekFile(hsOpCstrFileHandle, offulFileSeek, &ulActualPosition)) < 0) {
        PifAbort(MODULE_OP_CSTR, FAULT_ERROR_FILE_SEEK);
    }

	//RPH 11-10-3 Changes for PifReadFile
	PifReadFile(hsOpCstrFileHandle, pTableHeadAddress, ulSizeofRead, &ulActualBytesRead);
    if ( ulActualBytesRead != ulSizeofRead) {
        PifAbort(MODULE_OP_CSTR, FAULT_ERROR_FILE_READ);
    }
    return(OP_PERFORM);
}
/*
*==========================================================================
**    Synopsis:    Op_ReadOepFile(ULONG  offulFileSeek, 
*                                 VOID   *pTableHeadAddress, 
*                                 ULONG  ulSizeofRead);
*
*       Input:  offulFileSeek           Number of bytes from the biginning of the oep file.
*               ulSizeofRead            Number of bytes to be read.
*      Output:  *pTableHeadAddress      Storage location of items to be read.
*       InOut:  Nothing
*
**  Return    : OP_PERFORM
*               OP_NO_READ_SIZE                  Read size is 0
*               (File error = system halt)
*
**  Description: Read data in Oep file.          
*==========================================================================
*/
#if defined(Op_ReadOepFile)
#pragma message("Op_ReadOepFile defined")
SHORT   Op_ReadOepFile_Special(ULONG offulFileSeek, VOID  *pTableHeadAddress, ULONG ulSizeofRead);
SHORT   Op_ReadOepFile_Debug(ULONG offulFileSeek, VOID  *pTableHeadAddress, ULONG ulSizeofRead, char *aszFilePath, int nLineNo)
{
	SHORT  sRetStatus;

	sRetStatus = Op_ReadOepFile_Special(offulFileSeek, pTableHeadAddress, ulSizeofRead);
	if (sRetStatus == OP_EOF) {
		int iLen = 0;
		char  xBuffer[256];

		iLen = strlen (aszFilePath);
		if (iLen > 30) {
			iLen = iLen - 30;
		}
		else {
			iLen = 0;
		}
		sprintf (xBuffer, "Op_ReadOepFile_Debug(): offset %d, File %s, lineno = %d", offulFileSeek, aszFilePath + iLen, nLineNo);
		NHPOS_ASSERT_TEXT(0, xBuffer);
	}

	return sRetStatus;
}

SHORT   Op_ReadOepFile_Special(ULONG offulFileSeek, VOID  *pTableHeadAddress, ULONG ulSizeofRead)
#else
SHORT   Op_ReadOepFile(ULONG offulFileSeek, VOID  *pTableHeadAddress, ULONG ulSizeofRead)
#endif
{
	SHORT   sRetStatus;
    ULONG   ulActualPosition;
	ULONG	ulActualBytesRead;	//RPH 11-10-3

    if (ulSizeofRead == 0X00) {
        return (OP_NO_READ_SIZE);
    }
    
    if ((sRetStatus = PifSeekFile(hsOpOepFileHandle, offulFileSeek, &ulActualPosition)) < 0) {
        PifLog(MODULE_OP_OEP, FAULT_ERROR_FILE_SEEK);
		PifLog (MODULE_DATA_VALUE(MODULE_OP_OEP), (USHORT)offulFileSeek);
		PifLog (MODULE_DATA_VALUE(MODULE_OP_OEP), (USHORT)abs(sRetStatus));
		{
			char xBuff[128];
			sprintf (xBuff, "**ERROR Op_ReadOepFile() PifSeekFile failed handle %d, offset %d, retstatus %d", hsOpOepFileHandle, offulFileSeek, sRetStatus);
			NHPOS_ASSERT_TEXT(0, xBuff);
		}
#if defined(Op_ReadOepFile)
		return(OP_EOF);
#else
        PifAbort(MODULE_OP_OEP, FAULT_ERROR_FILE_SEEK);
#endif
    }
	
	//RPH 11-10-3 Changes for PifReadFile
	PifReadFile(hsOpOepFileHandle, pTableHeadAddress, ulSizeofRead, &ulActualBytesRead);
    if ( ulActualBytesRead != ulSizeofRead) {
		PifLog (MODULE_OP_OEP, FAULT_ERROR_FILE_READ);
		PifLog (MODULE_DATA_VALUE(MODULE_OP_OEP), (USHORT)offulFileSeek);
		{
			char xBuff[128];
			sprintf (xBuff, "**ERROR Op_ReadOepFile() PifReadFile failed handle %d, offset %d", hsOpOepFileHandle, offulFileSeek);
			NHPOS_ASSERT_TEXT(0, xBuff);
		}
#if defined(Op_ReadOepFile)
		return(OP_EOF);
#else
        PifAbort(MODULE_OP_OEP, FAULT_ERROR_FILE_READ);
#endif
    }
    return(OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   Op_OepIndex(OPDEPT_FILEHED *OepFileHed, 
*                                      UCHAR         uchModeType, 
*                                      UCHAR         uchGroupNo, 
*                                      USHORT        *usParaBlockNo)
*
*       Input:    *OepFileHed     Pointer to the Oep file headder
*                 uchModeType     Type of mode (search only or regist)
*                 uchGrouptNo     Group No to be searched.
*                 
*      Output:    *usParaBlockNo  Storage location for Pamataer block No.
*       InOut:    Nothing
*
**  Return    :   OP_NEW_OEP
*                 OP_CHANGE_OEP
*                 OP_NOT_IN_FILE
*                 (file error is system halt)
*
**  Description:
*                 uchModeType
*                 OP_SEARCH_ONLY    Check Assign inputed Group No.
*                 OP_REGIST         Add and change inputted Group No. 2172
*==========================================================================
*/
SHORT   Op_OepIndex(OPOEP_FILEHED *OepFileHed, 
					USHORT         usFilePart,
                    UCHAR          uchModeType, 
                    USHORT         usGroupNo,
                    TCHAR          *uchPluNumber,
                    UCHAR          uchAdjective,
                    ULONG          *poffulSearchPoint)
{
    OPOEP_INDEXENTRY    IndexEntry;
    SHORT               sStatus;
    ULONG               offulSearchPoint;
	ULONG               *poffulFilePart;
	USHORT              *pusNumberOfResidentOep;
	SHORT (*pCompOep)(VOID *pKey, VOID *pusHPoint) = Op_OepCompIndex;    /* Add 2172 Rel 1.0 */
    
    offulSearchPoint = *poffulSearchPoint;

    /* Make Index Table Size */
    IndexEntry.usGroupNo = usGroupNo;
    _tcsncpy(IndexEntry.auchOepPluNo, uchPluNumber, OP_PLU_OEP_LEN);
    IndexEntry.uchAdjective = uchAdjective;

	poffulFilePart = &(OepFileHed->offulOfIndexTbl);
	pusNumberOfResidentOep = &(OepFileHed->usNumberOfResidentOep);
	switch (usFilePart) {
		case 1:
			poffulFilePart = &(OepFileHed->offulOfIndexTbl2);
			pusNumberOfResidentOep = &(OepFileHed->usNumberOfResidentOep2);
			break;
	}

    if (*pusNumberOfResidentOep != 0x0000) 
    {
		/* Search Group No. in Index Table */
        sStatus = Op_OepSearchFile(*poffulFilePart, (ULONG)(*pusNumberOfResidentOep * sizeof(OPOEP_INDEXENTRY)),
                                &offulSearchPoint, &IndexEntry, sizeof(IndexEntry), pCompOep);
    
        if ((sStatus == OP_NOT_IN_FILE) && (uchModeType == OP_SEARCH_ONLY)) 
        {
            *poffulSearchPoint = offulSearchPoint;
            return (OP_NOT_IN_FILE);
        }

        if ((sStatus == OP_PERFORM)  && (uchModeType == OP_SEARCH_ONLY)) /* CHANGE OEP */
        {  
            *poffulSearchPoint = offulSearchPoint;
            return (sStatus);
        }

        /* BELOW IS NEW OEP  ASSIGN */
        /* Check Paramater Table full and get paramater No. */
        /* make space 1 record in index table */
        Op_OepInsertTable(*poffulFilePart, (ULONG)(*pusNumberOfResidentOep * sizeof(OPOEP_INDEXENTRY)),
                          (ULONG)(offulSearchPoint - *poffulFilePart),
                          &IndexEntry, sizeof(OPOEP_INDEXENTRY));
    }
    else 
    {
		/* Index Table size = 0 */
        if (uchModeType == OP_SEARCH_ONLY) 
        {
            *poffulSearchPoint = offulSearchPoint;
            return(OP_NOT_IN_FILE);
        }

        Op_WriteOepFile(*poffulFilePart, &IndexEntry, sizeof(OPOEP_INDEXENTRY));
    }

    (*pusNumberOfResidentOep)++;
    Op_WriteOepFile(OP_FILE_HEAD_POSITION, OepFileHed, sizeof(OPOEP_FILEHED));
    *poffulSearchPoint = offulSearchPoint;
    return(OP_NEW_OEP);
}
/*
*==========================================================================
**    Synopsis:   SHORT   Op_OepSearchFile(USHORT offusTableHeadAddress, 
*                                          USHORT usTableSize, 
*                                          USHORT *poffusSearchPoint,
*                                          VOID   *pSearchData, 
*                                          USHORT usSearchDataSize, 
*                                          SHORT (*Comp)(VOID *pKey, 
*                                          VOID *pusCompKey))
*
*       Input:    offulTableHeadAddress
*                 ulTableSize
*                 *pSearchPoint
*                 *pSearchData
*                 usSearchDataSize
*                 (*Comp)(VOID *A, VOID *B)   A - B
*               
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   OP_PERFORM
*                 OP_NOT_IN_FILE
*                 (file error is system halt)
*
**  Description:  Search data on the Table in a File. 2172
*==========================================================================
*/
SHORT   Op_OepSearchFile(ULONG offulTableHeadAddress, 
                         ULONG ulTableSize, 
                         ULONG  *poffulSearchPoint,
                         OPOEP_INDEXENTRY   *pSearchData, 
                         USHORT usSearchDataSize, 
                         SHORT (*Comp)(VOID *pKey, VOID *pusCompKey))
{
    SHORT sStatus;
    OPOEP_INDEXENTRY  auchKeyData;
    UCHAR auchWorkBuffer[OP_MAX_WORK_BUFFER * 4];
    SHORT sCompStatus;
    ULONG offulTableTailAddress;
    ULONG offulTableSize;
    USHORT *pusHitPoint;
    ULONG ulNumberOfEntry;             /* number of entry */

    sStatus = 0;

    /* make Table Tail address */
    offulTableTailAddress = offulTableHeadAddress + ulTableSize;
    offulTableSize = ulTableSize;

    /* Check this key grater than Current MAX  Key */

/*    if ( usTableSize  != 0 ) {                                                */
/*                                                                              */
/*        *poffusSearchPoint = offusTableTailAddress -  usSearchDataSize;       */
/*                                                                              */
/*        Op_ReadDeptFile( *poffusSearchPoint, auchKeyData, usSearchDataSize);   */
/*                                                                              */
/*        if ((sCompStatus = (*Comp)(pSearchData, auchKeyData)) == 0) {         */
/*            return(OP_PERFORM);                                               */
/*        } else if (sCompStatus > 0) {                                         */
/*            *poffusSearchPoint = offusTableTailAddress;                       */
/*            return(OP_NOT_IN_FILE);                                           */
/*        }                                                                     */
/*    }                                                                         */
/*                                                                              */

    /* Search key in the file */
	// Using a binary search look up the OEP entry in the PARAMOEP file.
	// When the search window is sufficiently small, just read in the entire
	// set of OEP entry records and do a binary search in memory rather than on disk.
    while ( offulTableSize > OP_MAX_WORK_BUFFER ) 
    {    
        ulNumberOfEntry = (offulTableSize / usSearchDataSize) / 2;
        *poffulSearchPoint = (ULONG)((ulNumberOfEntry * usSearchDataSize) + offulTableHeadAddress);
        Op_ReadOepFile( *poffulSearchPoint, &auchKeyData, usSearchDataSize);
        if ((sCompStatus = (*Comp)(pSearchData, &auchKeyData)) == 0) 
        {
            return(OP_PERFORM);
            break;
        } 
        else if (sCompStatus > 0) 
        {   /* pSearchData > auchKeyData */
            offulTableHeadAddress = *poffulSearchPoint;
            offulTableHeadAddress += usSearchDataSize;
        } 
        else 
        {
            offulTableTailAddress = *poffulSearchPoint;
        }
        offulTableSize = offulTableTailAddress - offulTableHeadAddress;
    }

    Op_ReadOepFile(offulTableHeadAddress, auchWorkBuffer, (USHORT)offulTableSize);   /* offusTableSize != 0 */

    /* Search key in a table from the file  */
    ulNumberOfEntry = offulTableSize / usSearchDataSize;

    sStatus = Rfl_SpBsearch(pSearchData, usSearchDataSize, (USHORT *)auchWorkBuffer, (USHORT)ulNumberOfEntry, &pusHitPoint, Comp);

    *poffulSearchPoint = (UCHAR *)pusHitPoint - auchWorkBuffer;
    *poffulSearchPoint += offulTableHeadAddress; /* offset in the file */
    if (sStatus == RFL_HIT) 
    {
        return(OP_PERFORM);
    }
    return(OP_NOT_IN_FILE);
}
/*
*==========================================================================
**    Synopsis:    SHORT   Op_OepCompIndex(USHORT *uchGroupNo, USHORT *pusHitPoint)
*
*       Input:    *uchGroupNo            Key
*                 *pusHitPoint        Data to be compare.
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   0: uchGroupNo = pusHitPoint
*               0 <: uchGroupNo < pusHitPoint  
*               0 >: uchGroupNo > pusHitPoint 
*
**  Description: Compare index entry
*==========================================================================
*/
SHORT   Op_OepCompIndex(OPOEP_INDEXENTRY *Key, OPOEP_INDEXENTRY *pusHitPoint)
{
    SHORT sStatus;

    sStatus = (SHORT)(Key->usGroupNo) - (SHORT)(pusHitPoint->usGroupNo);
    if (sStatus == 0) {
        sStatus = _tcsncmp(Key->auchOepPluNo, pusHitPoint->auchOepPluNo, OP_PLU_OEP_LEN);
        if (sStatus == 0) {
            sStatus = (SHORT)(Key->uchAdjective - pusHitPoint->uchAdjective);
        }
    }
    return(sStatus);            
}

/*
*==========================================================================
**    Synopsis:   VOID    Op_OepInsertTable(USHORT offusTableHeadAddress, 
*                                           USHORT usTableSize
*                                           USHORT offusInsertPoint, 
*                                           VOID   *pInsertData, 
*                                           USHORT usInsertDataSize)
*
*       Input:   offusTableHeadAddress  Table head address in a file.
*                usTableSize            Table size in a file.
*                offInsertPoint         Table Insert Point (offset from tabel head address) in a file.
*                *pInsertData           Inserted Data
*                usInsertDataSize       Insert Data size
*               
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*                 
*
**  Description: Insert data to Table in a File. 2172
*==========================================================================
*/
VOID   Op_OepInsertTable( ULONG offulTableHeadAddress, 
                          ULONG ulTableSize,
                          ULONG  offulInsertPoint, 
                          VOID   *pInsertData, 
                          USHORT usInsertDataSize)
{
    UCHAR   auchWorkBuffer[OP_MAX_WORK_BUFFER];  /* work buffer */
    ULONG   offulTailInFile;    /* Table Tail Address */
    ULONG   ulMoveByte;         /* moving byte save area */

    if (ulTableSize <= offulInsertPoint) 
    {
        Op_WriteOepFile(offulInsertPoint + offulTableHeadAddress, pInsertData, usInsertDataSize );  /* insert data */
        return;
    }

    /* Calculate moving byte */
    ulMoveByte = ulTableSize - offulInsertPoint;
    offulTailInFile = offulTableHeadAddress + ulTableSize;

    /* Move moving byte in a file */
    while ( ulMoveByte > OP_MAX_WORK_BUFFER ) {
        offulTailInFile -= OP_MAX_WORK_BUFFER;
        Op_ReadOepFile(offulTailInFile, auchWorkBuffer, OP_MAX_WORK_BUFFER);
        Op_WriteOepFile(offulTailInFile + usInsertDataSize, auchWorkBuffer, OP_MAX_WORK_BUFFER);
        ulMoveByte -= OP_MAX_WORK_BUFFER;
    }

    if (ulMoveByte != 0) {
        Op_ReadOepFile(offulTableHeadAddress + offulInsertPoint, auchWorkBuffer, (USHORT)ulMoveByte);
        Op_WriteOepFile((offulTableHeadAddress + offulInsertPoint + usInsertDataSize), auchWorkBuffer, (USHORT)ulMoveByte);
    }

    /* insert data */
    Op_WriteOepFile(offulTableHeadAddress + offulInsertPoint, pInsertData, usInsertDataSize );
}


/*
*==========================================================================
**    Synopsis:    SHORT   Op_DeptIndex(OPDEPT_FILEHED *DeptFileHed, 
*                                   UCHAR         uchModeType, 
*                                   USHORT        usDeptNo, 
*                                   USHORT        *usParaBlockNo)
*
*       Input:    *DeptFileHed     Pointer to the Dept file headder
*                 uchModeType     Type of mode (search only or regist)
*                 usDeptNo         Dept No to be searched.
*                 
*      Output:    *usParaBlockNo  Storage location for Pamataer block No.
*       InOut:    Nothing
*
**  Return    :   OP_NEW_DEPT
*                 OP_CHANGE_DEPT
*                 OP_DEPT_FULL
*                 OP_NOT_IN_FILE
*                 (file error is system halt)
*
**  Description:
*                 uchModeType
*                 OP_SEARCH_ONLY    Check Assign inputed Dept No.
*                 OP_REGIST         Add and change inputted Dept No. 2172
*==========================================================================
*/
SHORT   Op_DeptIndex(OPDEPT_FILEHED *DeptFileHed, 
                 UCHAR          uchModeType, 
                 USHORT         usDeptNo, 
                 USHORT         *pusParaBlockNo)
{
    OPDEPT_INDEXENTRY    IndexEntry;
    USHORT              offusSearchPoint;
    USHORT              usIndexBlockNo;
    SHORT               sStatus;
    USHORT              offusIndex;

    /* Make Index Table Size */
    IndexEntry.usDeptNo      = usDeptNo;

    if (DeptFileHed->usNumberOfResidentDept != 0x0000) {
        usIndexBlockNo = DeptFileHed->usNumberOfResidentDept;

    /* Search Dept No. in Index Table */
        sStatus = Op_DeptSearchFile((USHORT)DeptFileHed->offulOfIndexTbl,
                                (USHORT)(DeptFileHed->usNumberOfResidentDept * sizeof(OPDEPT_INDEXENTRY)),
                                &offusSearchPoint, &IndexEntry, 
                                sizeof(IndexEntry), pCompDept);
    
        if ((sStatus == OP_NOT_IN_FILE) && (uchModeType == OP_SEARCH_ONLY)) {
            return (OP_NOT_IN_FILE);
        }

        if (sStatus == OP_PERFORM) {  /* CHANGE Dept */
            offusIndex = (offusSearchPoint - (USHORT)DeptFileHed->offulOfIndexTbl) / sizeof(IndexEntry);
            Op_ReadDeptFile(DeptFileHed->offulOfIndexTblBlk + (ULONG)(offusIndex * OP_DEPT_INDEXBLK_SIZE), pusParaBlockNo, OP_DEPT_INDEXBLK_SIZE);
            return (OP_CHANGE_DEPT);
        }

        /* BELOW IS NEW DEPT  ASSIGN */

        /* Check Paramater Table full and get paramater No. */
        if (Op_DeptEmpBlkGet(DeptFileHed, pusParaBlockNo) == OP_DEPT_FULL) {
            return(OP_DEPT_FULL);
        }

        /* make space 1 record in index table */
        Op_DeptInsertTable((USHORT)DeptFileHed->offulOfIndexTbl, 
                       (USHORT)(DeptFileHed->usNumberOfResidentDept * sizeof(OPDEPT_INDEXENTRY)),
                       (USHORT)(offusSearchPoint - (USHORT)DeptFileHed->offulOfIndexTbl),
                       &IndexEntry, sizeof(OPDEPT_INDEXENTRY));

        /* make space 1 record in paramater block table */
        Op_DeptInsertTable((USHORT)DeptFileHed->offulOfIndexTblBlk, 
                       (USHORT)(DeptFileHed->usNumberOfResidentDept * OP_DEPT_INDEXBLK_SIZE),
                       (USHORT)( ((offusSearchPoint - (USHORT)DeptFileHed->offulOfIndexTbl) / sizeof(OPDEPT_INDEXENTRY)) * OP_DEPT_INDEXBLK_SIZE),
                       pusParaBlockNo, OP_DEPT_INDEXBLK_SIZE);

        /* ResidentDept +1 */
        DeptFileHed->usNumberOfResidentDept++;

        Op_WriteDeptFile(OP_FILE_HEAD_POSITION, DeptFileHed, sizeof(OPDEPT_FILEHED));

        return(OP_NEW_DEPT);

    } else {
		/* Index Table size = 0 */
        if (uchModeType == OP_SEARCH_ONLY) {
            return(OP_NOT_IN_FILE);
        }
        if (Op_DeptEmpBlkGet(DeptFileHed, pusParaBlockNo) == OP_DEPT_FULL) {
            return(OP_DEPT_FULL);
        }
        Op_WriteDeptFile(DeptFileHed->offulOfIndexTbl, &IndexEntry, sizeof(OPDEPT_INDEXENTRY));
        Op_WriteDeptFile(DeptFileHed->offulOfIndexTblBlk, pusParaBlockNo, OP_DEPT_INDEXBLK_SIZE);
        DeptFileHed->usNumberOfResidentDept++;
        Op_WriteDeptFile(OP_FILE_HEAD_POSITION, DeptFileHed, sizeof(OPDEPT_FILEHED));
        return(OP_NEW_DEPT);
    }
}
/*
*==========================================================================
**    Synopsis:    SHORT   Op_DeptIndexDel(OP_FILEHED *DeptFileHed, 
*                                      USHORT     usDeptNo) 
*
*       Input:    *DeptFileHed  Pointer to the Dept file headder.
*                 usDeptNo      Dept No to be written.
*               
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   OP_PERFORM
*                 OP_NOT_IN_FILE
*                 (file error is system halt)
*
**  Description:  Delete inputted Dept No. in Dept File. 2172
*==========================================================================
*/
SHORT   Op_DeptIndexDel(OPDEPT_FILEHED *DeptFileHed, 
                    USHORT        usDeptNo) 
{

    OPDEPT_INDEXENTRY    IndexEntry;
    USHORT              offusSearchPoint;
    USHORT              usParaBlockNo;

    /* Make Index Table Size */
    if (DeptFileHed->usNumberOfResidentDept == 0x0000){
        return(OP_NOT_IN_FILE);
    }
    IndexEntry.usDeptNo      = usDeptNo;

    if ((Op_DeptSearchFile((USHORT)DeptFileHed->offulOfIndexTbl,
                       (USHORT)(DeptFileHed->usNumberOfResidentDept * 
                            sizeof(OPDEPT_INDEXENTRY)),
                       &offusSearchPoint, 
                       &IndexEntry, 
                       sizeof(IndexEntry), 
                       pCompDept))
                            == OP_NOT_IN_FILE) {
        return(OP_NOT_IN_FILE);
     }
    
    /* Get Paramater Block No */
    Op_ReadDeptFile((ULONG)(DeptFileHed->offulOfIndexTblBlk +
                   ((offusSearchPoint - (USHORT)DeptFileHed->offulOfIndexTbl)
                     / sizeof(OPDEPT_INDEXENTRY)) * OP_DEPT_INDEXBLK_SIZE),
                   &usParaBlockNo, 
                   OP_DEPT_INDEXBLK_SIZE);

    /* Delete space 1 record in index table */
    Op_DeptDeleteTable((USHORT)DeptFileHed->offulOfIndexTbl, 
                   (USHORT)(DeptFileHed->usNumberOfResidentDept * 
                         sizeof(OPDEPT_INDEXENTRY)),
                   (USHORT)(offusSearchPoint - (USHORT)DeptFileHed->offulOfIndexTbl), 
                   sizeof(OPDEPT_INDEXENTRY));

    /* Delete space 1 record in paramater block table */
    Op_DeptDeleteTable((USHORT)DeptFileHed->offulOfIndexTblBlk, 
                   (USHORT)(DeptFileHed->usNumberOfResidentDept * OP_DEPT_INDEXBLK_SIZE),
                   (USHORT)(((offusSearchPoint - (USHORT)DeptFileHed->offulOfIndexTbl) 
                   / sizeof(OPDEPT_INDEXENTRY)) * OP_DEPT_INDEXBLK_SIZE),
                   OP_DEPT_INDEXBLK_SIZE);

    /* Empty para empty table */
    Op_DeptBlkEmp( DeptFileHed, usParaBlockNo );

    /* ResidentDept +1 */
    DeptFileHed->usNumberOfResidentDept--;

    /* Save Index Table */
    Op_WriteDeptFile(OP_FILE_HEAD_POSITION, DeptFileHed, sizeof(OPDEPT_FILEHED));
    return(OP_PERFORM);
}
/*
*==========================================================================
**    Synopsis:    VOID   Op_DeptNoGet( OP_DEPTFILEHED *DeotFileHed, 
*                                   ULONG       culCounter, 
*                                   USHORT      *usDeptNo, 
*                                   USHORT      *usParaBlockNo)
*
*       Input:    *DeptFileHed       Pointer to the Dept file headder.
*                 culCounter        Counter That is used in this function.
*                 *usParaBlkNo      Pointer to the Paramater block No to be written.
*                 
*      Output:    *usDeptNo          Dept No to be searched.
*       InOut:    Nothing
*
**  Return    :   OP_PERFORM
*                 OP_NOT_IN_FILE
*                 (file error is system halt)
*
**  Description:  Get Dept No. in Dept File. 2172
*==========================================================================
*/
VOID   Op_DeptNoGet( OPDEPT_FILEHED *OpDeptFileHed, 
                 ULONG         culCounter, 
                 USHORT        *usDeptNo, 
                 USHORT        *usParaBlockNo)
{
    OPDEPT_INDEXENTRY    IndexEntry;

    Op_ReadDeptFile(OpDeptFileHed->offulOfIndexTbl + (ULONG)(culCounter * sizeof(OPDEPT_INDEXENTRY)), &IndexEntry, sizeof(OPDEPT_INDEXENTRY));
    *usDeptNo = IndexEntry.usDeptNo;  /* output condtion */
    Op_ReadDeptFile(OpDeptFileHed->offulOfIndexTblBlk + (ULONG)(culCounter * OP_DEPT_INDEXBLK_SIZE), &usParaBlockNo, OP_DEPT_INDEXBLK_SIZE);
}

/*
*==========================================================================
**    Synopsis:    VOID   Op_DeptParaRead(OP_DEPTFILEHED      *DeptFileHed, 
*                                        USHORT          usParaBlockNo, 
*                                        OPDEPT_PARAENTRY *ParaEntry)
*
*       Input:  *DeptFileHed     Pointer to the Dept file headder.
*               culCounter      Counter (first entry is 0, next is not to be touched)
*               *usParaBlkNo    Paramater table's block No.
*               *usDeptNo        Dept No to be searched.
*               
*      Output:  *ParaEntry  Strage location for Paramater entry.
*       InOut:  Nothing
*
**  Return    : Nothing
*               (file error is system halt)
*
**  Description: Read paramater of inputting Dept No. by usParaBlkNo. 2172
*==========================================================================
*/
VOID   Op_DeptParaRead(OPDEPT_FILEHED    *DeptFileHed, 
                      USHORT           usParaBlockNo, 
                      OPDEPT_PARAENTRY  *ParaEntry)
{
    Op_ReadDeptFile(DeptFileHed->offulOfParaTbl + (ULONG)(sizeof(OPDEPT_PARAENTRY) * ((ULONG)(usParaBlockNo - 1))), ParaEntry, sizeof(OPDEPT_PARAENTRY));
}
/*
*==========================================================================
**    Synopsis:    VOID   OP_DeptParaWrite(OP_DEPTFILEHED      *DeptFileHed, 
*                                         USHORT          usParaBlockNo, 
*                                         OPDEPT_PARAENTRY *ParaEntry)
*
*       Input:  *DeptFileHed     Pointer to the Dept file headder.
*               culCounter      Counter(first entry is 0 , next is not to be touched.
*               *usParaBlkNo    Paramater table block no.
*               *usDeptNo        Dept No to be searched.
*               
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return    : Nothing
*
**  Description: Write Paramater inputted Dept to Dept file . 2172
*==========================================================================
*/
VOID   Op_DeptParaWrite(OPDEPT_FILEHED   *DeptFileHed, 
                       USHORT          usParaBlockNo, 
                       OPDEPT_PARAENTRY *ParaEntry)
{
    Op_WriteDeptFile(DeptFileHed->offulOfParaTbl + (ULONG)(sizeof(OPDEPT_PARAENTRY) * ((ULONG)(usParaBlockNo - 1))), ParaEntry, sizeof(OPDEPT_PARAENTRY));
}
/*
*==========================================================================
**    Synopsis:    SHORT   Op_DeptCompIndex(USHORT *usDeptNo, USHORT *pusHitPoint)
*
*       Input:    *usDeptNo            Key
*                 *pusHitPoint        Data to be compare.
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   0: usDeptNo = pusHitPoint
*               0 <: usDeptNo < pusHitPoint  
*               0 >: usDeptNo > pusHitPoint 
*
**  Description: Compare index entry
*==========================================================================
*/
SHORT   Op_DeptCompIndex(OPDEPT_INDEXENTRY *Key, OPDEPT_INDEXENTRY *pusHitPoint)
{
    SHORT sStatus;

    sStatus = (Key->usDeptNo - pusHitPoint->usDeptNo);
    return(sStatus);            
}

/*
*==========================================================================
**    Synopsis:    SHORT   Op_GetDept(OPDEPT_FILEHED  *pDeptFileHed, 
*                                    ULONG          culCounter, 
*                                    DEPTIF          *pParaEntry);
*
*       Input:    *pDeptFileHed      Point to the Dept File headder.
*                 culCounter        Counter(First entry is 0, next is not to be touched)
*      Output:    *pParaEntry       Storage location for Dept if data.
*       InOut:    Nothing
*
**  Return    :   OP_PERFORM
*                 OP_EOF
*                 (file error is system halt)
*
**  Description: Get Dept No in order of the number. 2172
*==========================================================================
*/
SHORT   Op_GetDept(OPDEPT_FILEHED *pDeptFileHed, 
                  ULONG         culCounter, 
                  DEPTIF         *pParaEntry)
{
    USHORT              usParaBlkNo;
    OPDEPT_INDEXENTRY    DeptIndex;

    if ((ULONG)pDeptFileHed->usNumberOfResidentDept <= culCounter) {
        return(OP_EOF);    
    }

    /*  get Dept no in index table */
    Op_ReadDeptFile(pDeptFileHed->offulOfIndexTbl + (ULONG)(culCounter * sizeof(OPDEPT_INDEXENTRY)), &DeptIndex, sizeof(OPDEPT_INDEXENTRY));
    pParaEntry->usDeptNo = DeptIndex.usDeptNo;              /* make output condition */

    /*  get Dept no in paramater block no */
    Op_ReadDeptFile( pDeptFileHed->offulOfIndexTblBlk + (ULONG)(culCounter * OP_DEPT_INDEXBLK_SIZE), &usParaBlkNo, OP_DEPT_INDEXBLK_SIZE);

    /*  get Dept paramater in paramater table */
    Op_ReadDeptFile( pDeptFileHed->offulOfParaTbl + (((ULONG)(usParaBlkNo - 1)) * ((ULONG)sizeof(OPDEPT_PARAENTRY))), &pParaEntry->ParaDept, sizeof(OPDEPT_PARAENTRY));
    return(OP_PERFORM);
}
/*
*==========================================================================
**    Synopsis:   VOID    Op_DeptInsertTable(USHORT offusTableHeadAddress, 
*                                        USHORT usTableSize
*                                        USHORT offusInsertPoint, 
*                                        VOID   *pInsertData, 
*                                        USHORT usInsertDataSize)
*
*       Input:   offusTableHeadAddress  Table head address in a file.
*                usTableSize            Table size in a file.
*                offInsertPoint         Table Insert Point (offset from tabel head address) in a file.
*                *pInsertData           Inserted Data
*                usInsertDataSize       Insert Data size
*               
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*                 
*
**  Description: Insert data to Table in a File. 2172
*==========================================================================
*/
VOID   Op_DeptInsertTable( USHORT offusTableHeadAddress, 
                       USHORT usTableSize,
                       USHORT offusInsertPoint, 
                       VOID   *pInsertData, 
                       USHORT usInsertDataSize)
{
    UCHAR   auchWorkBuffer[OP_MAX_WORK_BUFFER];  /* work buffer */
    USHORT  offusTailInFile;    /* Table Tail Address */
    USHORT  usMoveByte;         /* moving byte save area */

    if (usTableSize <= offusInsertPoint) {
        Op_WriteDeptFile(offusInsertPoint + offusTableHeadAddress, pInsertData, usInsertDataSize );  /* insert data */
        return;
    }

    /* Calculate moving byte */
    usMoveByte = usTableSize - offusInsertPoint;
    offusTailInFile = offusTableHeadAddress + usTableSize;

    /* Move moving byte in a file */
    while ( usMoveByte > OP_MAX_WORK_BUFFER ) {
        offusTailInFile -= OP_MAX_WORK_BUFFER;
        Op_ReadDeptFile(offusTailInFile, auchWorkBuffer, OP_MAX_WORK_BUFFER);
        Op_WriteDeptFile(offusTailInFile + usInsertDataSize, auchWorkBuffer, OP_MAX_WORK_BUFFER);
        usMoveByte -= OP_MAX_WORK_BUFFER;
    }

    if (usMoveByte != 0) {
        Op_ReadDeptFile(offusTableHeadAddress + offusInsertPoint, auchWorkBuffer, usMoveByte);
        Op_WriteDeptFile((offusTableHeadAddress + offusInsertPoint + usInsertDataSize), auchWorkBuffer, usMoveByte);
    }

    /* insert data */

    Op_WriteDeptFile(offusTableHeadAddress + offusInsertPoint, pInsertData, usInsertDataSize );
}

/*
*==========================================================================
**    Synopsis:   VOID   Op_DeptDeleteTable(USHOT  offusTableHeadAddress, 
*                                       USHORT usTableSize
*                                       USHORT offusDeletePoint, 
*                                       USHORT usDeleteDataSize)
*                                        
*
*       Input:    offusTableHeadAddress     Table head address in a file.
*                 usTableSize               Table size in a file.
*                 offusDeletePoint          Point 
*                 usDeleteDataSize          Data size
*                   
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*                 (file error is system halt)
*
**  Description:
*               Delete data from Index Table in a File. 2172
*==========================================================================
*/
VOID    Op_DeptDeleteTable(USHORT offusTableHeadAddress, 
                       USHORT usTableSize,
                       USHORT offusDeletePoint, 
                       USHORT usDeleteDataSize)
{
    UCHAR   auchWorkBuffer[OP_MAX_WORK_BUFFER];  /* work buffer */
    USHORT  usMoveByte;     /* moving byte save area */
    
    /* Calculate moving byte */
    usMoveByte = usTableSize - offusDeletePoint - usDeleteDataSize;

    /* Move moving byte in a file */
    offusDeletePoint += offusTableHeadAddress;

    while ( usMoveByte > OP_MAX_WORK_BUFFER ) {
        Op_ReadDeptFile( offusDeletePoint + usDeleteDataSize, auchWorkBuffer, OP_MAX_WORK_BUFFER);
        Op_WriteDeptFile( offusDeletePoint, auchWorkBuffer, OP_MAX_WORK_BUFFER);
        usMoveByte -= OP_MAX_WORK_BUFFER;
        offusDeletePoint += OP_MAX_WORK_BUFFER;
    }

    if (usMoveByte != 0) {
        Op_ReadDeptFile(offusDeletePoint + usDeleteDataSize, auchWorkBuffer, usMoveByte);
        Op_WriteDeptFile( offusDeletePoint, auchWorkBuffer, usMoveByte);
    }
}

/*
*==========================================================================
**    Synopsis:   SHORT   Op_DeptSearchFile(USHORT offusTableHeadAddress, 
*                                       USHORT usTableSize, 
*                                       USHORT *poffusSearchPoint,
*                                       VOID   *pSearchData, 
*                                       USHORT usSearchDataSize, 
*                                       SHORT (*Comp)(VOID *pKey, 
*                                                     VOID *pusCompKey))
*
*       Input:    offulTableHeadAddress
*                 ulTableSize
*                 *pSearchPoint
*                 *pSearchData
*                 usSearchDataSize
*                 (*Comp)(VOID *A, VOID *B)   A - B
*               
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   OP_PERFORM
*                 OP_NOT_IN_FILE
*                 (file error is system halt)
*
**  Description:  Search data on the Table in a File. 2172
*==========================================================================
*/
SHORT   Op_DeptSearchFile(USHORT offusTableHeadAddress, 
                      USHORT usTableSize, 
                      USHORT *poffusSearchPoint,
                      VOID   *pSearchData, 
                      USHORT usSearchDataSize, 
                      SHORT (*Comp)(VOID *pKey, VOID *pusCompKey))
{
    SHORT sStatus;
    UCHAR auchKeyData[10];
    UCHAR auchWorkBuffer[OP_MAX_WORK_BUFFER];
    SHORT sCompStatus;
    USHORT offusTableTailAddress;
    USHORT offusTableSize;
    USHORT *pusHitPoint;
    USHORT usNumberOfEntry;             /* number of entry */

    sStatus = 0;

    /* make Table Tail address */
    offusTableTailAddress = offusTableHeadAddress + usTableSize;
    offusTableSize = usTableSize;

    /* Check this key grater than Current MAX  Key */

/*    if ( usTableSize  != 0 ) {                                                */
/*                                                                              */
/*        *poffusSearchPoint = offusTableTailAddress -  usSearchDataSize;       */
/*                                                                              */
/*        Op_ReadDeptFile( *poffusSearchPoint, auchKeyData, usSearchDataSize);   */
/*                                                                              */
/*        if ((sCompStatus = (*Comp)(pSearchData, auchKeyData)) == 0) {         */
/*            return(OP_PERFORM);                                               */
/*        } else if (sCompStatus > 0) {                                         */
/*            *poffusSearchPoint = offusTableTailAddress;                       */
/*            return(OP_NOT_IN_FILE);                                           */
/*        }                                                                     */
/*    }                                                                         */
/*                                                                              */

    /* Search key in the file */
    while ( offusTableSize > OP_MAX_WORK_BUFFER ) {    
        usNumberOfEntry = (offusTableSize / usSearchDataSize) / 2;
        *poffusSearchPoint = (usNumberOfEntry * usSearchDataSize) + offusTableHeadAddress;
        Op_ReadDeptFile( *poffusSearchPoint, auchKeyData, usSearchDataSize);
        if ((sCompStatus = (*Comp)(pSearchData, auchKeyData)) == 0) {
            return(OP_PERFORM);
            break;
        } else if (sCompStatus > 0) {   /* pSearchData > auchKeyData */
            offusTableHeadAddress = *poffusSearchPoint;
            offusTableHeadAddress += usSearchDataSize;
        } else {
            offusTableTailAddress = *poffusSearchPoint;
        }
        offusTableSize = offusTableTailAddress - offusTableHeadAddress;
    }

    Op_ReadDeptFile((ULONG)offusTableHeadAddress, auchWorkBuffer, offusTableSize);   /* offusTableSize != 0 */

    /* Search key in a table from the file  */

    usNumberOfEntry = offusTableSize / usSearchDataSize;

    sStatus = Rfl_SpBsearch(pSearchData, usSearchDataSize, (USHORT *)auchWorkBuffer, usNumberOfEntry, &pusHitPoint, Comp);

    *poffusSearchPoint = (UCHAR *)pusHitPoint - auchWorkBuffer;
    *poffusSearchPoint += offusTableHeadAddress; /* offset in the file */
    if (sStatus == RFL_HIT) {
        return(OP_PERFORM);
    }
    return(OP_NOT_IN_FILE);
}
/*
*==========================================================================
**    Synopsis:   SHORT   Op_DeptEmpBlkGet(OPDEPT_FILEHED *DeptFileHed,
*                                      USHORT        *pusParaBlockNo )
*
*       Input:    *DeptFileHed
*                 *pusParaBlockNo
*               
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   OP_PERFORM
*                 OP_DEPT_FULL
*
**  Description:
*               Let Empty Empty Block No. 2172
*==========================================================================
*/
SHORT   Op_DeptEmpBlkGet( OPDEPT_FILEHED *DeptFileHed, USHORT *pusParaBlockNo )
{
    UCHAR auchParaEmpBlk[OPDEPT_PARA_EMPTY_TABLE_SIZE];
    SHORT sI;
    SHORT sJ;
    SHORT sAnswer;

    Op_ReadDeptFile(DeptFileHed->offulOfParaEmpTbl, auchParaEmpBlk, OPDEPT_PARA_EMPTY_TABLE_SIZE);
    sAnswer = 0;
    for (sI = 0; OPDEPT_PARA_EMPTY_TABLE_SIZE > sI; sI++ ) {
        if (auchParaEmpBlk[sI] != 0xff) {
            for (sJ = 0; sJ < 8; sJ++) {
                if (((auchParaEmpBlk[sI] >> sJ) & 0x01) == 0x00) {
                    auchParaEmpBlk[sI] |= (UCHAR)(0x01 << sJ);
                    sAnswer = 1;
                    break;
                }
            }
        }
        if (sAnswer == 1) {
            break;
        }
    }
    if (sAnswer == 1) {
        *pusParaBlockNo = (USHORT)((sI * 8) + sJ + 1);
        if (*pusParaBlockNo > DeptFileHed->usNumberOfMaxDept) {
            return(OP_DEPT_FULL);
        }
        Op_WriteDeptFile(DeptFileHed->offulOfParaEmpTbl, auchParaEmpBlk, OPDEPT_PARA_EMPTY_TABLE_SIZE);

        return(OP_PERFORM);
    }
    return(OP_DEPT_FULL);
}
/*
*==========================================================================
**    Synopsis:   VOID  Op_DeptBlkEmp(OPDEPT_FILEHED *DeptFileHed, 
*                                 USHORT        uaParaBlockNo )
*
*       Input:    *DeptFileHed
*                 usParaBlockNo
*               
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return:       Nothing
*
**  Description:
*                 Get Empty Block No. 2172
*==========================================================================
*/
VOID   Op_DeptBlkEmp( OPDEPT_FILEHED *DeptFileHed, USHORT usParaBlockNo )
{
    UCHAR auchParaEmpBlk[OPDEPT_PARA_EMPTY_TABLE_SIZE];
    SHORT sI;
    SHORT sJ;

    Op_ReadDeptFile(DeptFileHed->offulOfParaEmpTbl, auchParaEmpBlk, OPDEPT_PARA_EMPTY_TABLE_SIZE);

    sI = (SHORT)(usParaBlockNo - 1) / 8;
    sJ = (SHORT)(usParaBlockNo - 1) % 8;

    auchParaEmpBlk[sI] &= ~( 0x01 << sJ );

    Op_WriteDeptFile(DeptFileHed->offulOfParaEmpTbl, auchParaEmpBlk, OPDEPT_PARA_EMPTY_TABLE_SIZE);
}

/*
*==========================================================================
**    Synopsis:    VOID   Op_CompSubIndex(OP_SUBINDEXENTRY *pArg, 
*                                         VOID             *offulOfIndexTbl)
*
*       Input:    *pArg
*                 *offulOfIndexTbl
*               
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   0: *pArg = *offulOfIndexTbl
*               0 <: *pArg < *offulOfIndexTbl  
*               0 >: *pArg > *offulOfIndexTbl 
*
**  Description:  Compare Sub index entry
*==========================================================================
*/
SHORT   Op_CompSubIndex(OP_SUBINDEXENTRY *pArg, VOID *poffulOfIndexTbl)
{
    SHORT            sStatus;
    OP_SUBINDEXENTRY *psSubIndex;

    psSubIndex = (OP_SUBINDEXENTRY *)poffulOfIndexTbl;
    return(sStatus = pArg->uchTableNo - psSubIndex->uchTableNo);
}
/*
*==========================================================================
**    Synopsis:    SHORT   OpResOep(UCHAR   *puchRcvData,
*                                   USHORT  usRcvLen,
*                                   UCHAR   *puchSndData,
*                                   UCHAR   *pusSndLen)
*
*       Input:    *puchRcvData        * Pointer to Request Data *
*                 usRcvLen            * Width of puchRcvData *
*      Output:    *puchSndData        * Storage location for backup data. *
*       InOut:    *pusSndLen          * Width of puchSndData *
*
**  Return    :   OP_PERFORM
*                 OP_COMM_ERROR;
*
**  Description:  Response Oep File Data (for broadcast)
*==========================================================================
*/
SHORT   OpResOep(UCHAR  *puchRcvData,
                 USHORT  usRcvLen,
                 UCHAR  *puchSndData,
                 USHORT *pusSndLen,
                 USHORT  usLockHnd)
{
	SHORT           sError;
    OP_BACKUP       *pOp_BackUpRcv, *pOp_BackUpSnd;
    OPOEP_FILEHED  OepFileHed;
	USHORT         usActualDataLen = 0;

    if (usRcvLen < sizeof(OP_BACKUP)) {
		PifLog (MODULE_OP_PLU, LOG_EVENT_OP_REQPARA_COMM_ERROR);
		PifLog (MODULE_LINE_NO(MODULE_OP_PLU), (USHORT)__LINE__);
		NHPOS_ASSERT_TEXT((usRcvLen >= sizeof(OP_BACKUP)), "OpResOep(): Bad Parameter.");
        return (OP_COMM_ERROR);
    }
    if (*pusSndLen < sizeof(OP_BACKUP)) {
		PifLog (MODULE_OP_PLU, LOG_EVENT_OP_REQPARA_COMM_ERROR);
		PifLog (MODULE_LINE_NO(MODULE_OP_PLU), (USHORT)__LINE__);
		NHPOS_ASSERT_TEXT((*pusSndLen >= sizeof(OP_BACKUP)), "OpResOep(): Bad Parameter.");
        return (OP_COMM_ERROR);
    }

	// calculate the actual amount of file data to be sent.
	// this is basically the sending buffer length minus the size of the OP_BACKUP struct 
	usActualDataLen = *pusSndLen;
    if (OP_BROADCAST_SIZE < usActualDataLen) {
        usActualDataLen = OP_BROADCAST_SIZE;
    }
	usActualDataLen -= sizeof(OP_BACKUP);

    pOp_BackUpRcv = (OP_BACKUP *)puchRcvData;
    pOp_BackUpSnd = (OP_BACKUP *)puchSndData;

    /*  Request Semaphore */
    PifRequestSem(husOpSem);

    /*  LOCK CHECK */
    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
		NHPOS_ASSERT_TEXT(!OP_LOCK, "Op_LockCheck() is OP_LOCK");
        return(OP_LOCK);
    }

	pOp_BackUpSnd->uchSubClass = pOp_BackUpRcv->uchSubClass;

    /*  File Open */
    if ((sError = Op_OepOpenFile(pOp_BackUpRcv->uchSubClass)) < 0) {  /* pif_error_file_exist */
        *pusSndLen = sizeof(OP_BACKUP);
        pOp_BackUpSnd->uchStatus = OP_END;
        pOp_BackUpSnd->offulFilePosition = pOp_BackUpRcv->offulFilePosition;
        PifReleaseSem(husOpSem);
		{
			char  xBuff[128];
			sprintf (xBuff, "Op_OepOpenFile(): uchSubClass %d, error %d, file position %d", pOp_BackUpRcv->uchSubClass, sError, pOp_BackUpRcv->offulFilePosition);
			NHPOS_ASSERT_TEXT(!sError, xBuff);
		}
        return(OP_FILE_NOT_FOUND);
    }
    /* Get File Header */
    Op_ReadOepFile(OP_FILE_HEAD_POSITION, &OepFileHed, sizeof(OepFileHed));

    if ( OepFileHed.ulOepFileSize <= pOp_BackUpRcv->offulFilePosition ) {
		char  xBuff[128];
        Op_CloseOepFileReleaseSem();
		sprintf (xBuff, "Op_OepOpenFile(): uchSubClass %d, file position %d, file size %d", pOp_BackUpRcv->uchSubClass, pOp_BackUpRcv->offulFilePosition, OepFileHed.ulOepFileSize);
		NHPOS_ASSERT_TEXT((OepFileHed.ulOepFileSize > pOp_BackUpRcv->offulFilePosition), xBuff);
		PifLog (MODULE_OP_PLU, LOG_EVENT_OP_REQPARA_COMM_ERROR);
		PifLog (MODULE_LINE_NO(MODULE_OP_PLU), (USHORT)__LINE__);
        return (OP_COMM_ERROR);
    }
    
    if ((OepFileHed.ulOepFileSize - pOp_BackUpRcv->offulFilePosition) > usActualDataLen) {
        pOp_BackUpSnd->uchStatus = OP_CONTINUE;
    } else {
		usActualDataLen = (USHORT)(OepFileHed.ulOepFileSize - pOp_BackUpRcv->offulFilePosition);
        pOp_BackUpSnd->uchStatus = OP_END;
    }

    pOp_BackUpSnd->offulFilePosition = pOp_BackUpRcv->offulFilePosition;
    pOp_BackUpSnd->usSeqNo   = pOp_BackUpRcv->usSeqNo;
    pOp_BackUpSnd->usRecordNo = OepFileHed.usNumberOfMaxOep;
    
    Op_ReadOepFile(pOp_BackUpRcv->offulFilePosition, puchSndData + sizeof(OP_BACKUP), usActualDataLen);

    *pusSndLen = (USHORT)(usActualDataLen + sizeof(OP_BACKUP));
    Op_CloseOepFileReleaseSem();
    return (OP_PERFORM);   
}

/*
*==========================================================================
**    Synopsis:    VOID   OpReqOep(USHORT usLockHnd)
*
*       Input:    usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*
**  Description:
*               Request Oep File Data (for broadcast)
*==========================================================================
*/
SHORT   OpReqOep(USHORT usFcCode, USHORT usLockHnd)
{
    OP_BACKUP       Op_Backup, *pOp_BackupRcv;
    USHORT          usRcvLen;
	USHORT          usOepIndexNumber = 0;
    UCHAR           auchRcvBuf[OP_BROADCAST_SIZE];
    OPOEP_FILEHED  OepFileHed;
    SHORT           sStatus;

    /*  LOCK CHECK */
    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
		NHPOS_ASSERT_TEXT(!OP_LOCK, "==WARNING: OpReqOep(): Op_LockCheck() is OP_LOCK");
        return(OP_LOCK);
    }

	for (usOepIndexNumber = 0; usOepIndexNumber < 4; usOepIndexNumber++) {
		{
			char xBuff[128];
			sprintf (xBuff, "==NOTE: OpReqOep() usOepIndexNumber %d", usOepIndexNumber);
			NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
		}

		/*  File Open */
		if ((sStatus = Op_OepOpenFile(usOepIndexNumber)) < 0) {  /* pif_error_file_exist */
			char xBuff[128];
			sprintf (xBuff, "==NOTE: OpReqOep() Op_OepOpenFile failed usOepIndexNumber %d, sStatus = %d", usOepIndexNumber, sStatus);
			NHPOS_NONASSERT_TEXT(xBuff);
//			if (usFcCode == CLI_FCBAKOPPARA) {
//				return(OP_PERFORM);
//			}
			continue;
//			return(OP_FILE_NOT_FOUND);
		}

		/* Get File Header */
		Op_ReadOepFile(OP_FILE_HEAD_POSITION, &OepFileHed, sizeof(OepFileHed));

		Op_Backup.uchClass = OP_OEP_FILE;
		Op_Backup.uchSubClass = usOepIndexNumber;

		Op_Backup.offulFilePosition = 0L;
		Op_Backup.usSeqNo = 1;

		usRcvLen = OP_BROADCAST_SIZE;
		pOp_BackupRcv = (OP_BACKUP *)auchRcvBuf;

		{
			char xBuff[128];
			sprintf (xBuff, "==NOTE: OpReqOep() Requesting OEP file subclass %d", Op_Backup.uchSubClass);
			NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
		}

		for (;;) {
			usRcvLen = OP_BROADCAST_SIZE;
			if (usFcCode == CLI_FCBAKOPPARA) {
				if  ((sStatus = SstReqBackUp(CLI_FCBAKOPPARA, (UCHAR *)&Op_Backup, sizeof(OP_BACKUP), auchRcvBuf, &usRcvLen)) < 0 ) {
					sStatus += STUB_RETCODE;     // move the STUB_ type of error code into the OP_ error code range for use by UicDisplayErrorMsg()
					break;
				}
			} else {
				if  ((sStatus = CstReqOpePara(usFcCode, (UCHAR *)&Op_Backup, sizeof(OP_BACKUP), auchRcvBuf, &usRcvLen)) < 0 ) {
					sStatus += STUB_RETCODE;     // move the STUB_ type of error code into the OP_ error code range for use by UicDisplayErrorMsg()
					break;
				}
			}

			if (pOp_BackupRcv->uchSubClass != Op_Backup.uchSubClass) {
				char xBuff[128];
				sprintf (xBuff, "OpReqOep(): uchSubClass requested %d received %d", pOp_BackupRcv->uchSubClass, Op_Backup.uchSubClass);
				NHPOS_NONASSERT_TEXT(xBuff);
			}

			if (pOp_BackupRcv->usSeqNo != Op_Backup.usSeqNo) {
				char  xBuff[128];
				if (Op_Backup.usSeqNo == pOp_BackupRcv->usSeqNo + 1) {
					sprintf (xBuff, "OpReqOep(): OP_COMM_IGNORE usSentSeqNo 0x%x, usRecvSeqNo 0x%x", Op_Backup.usSeqNo, pOp_BackupRcv->usSeqNo);
					NHPOS_ASSERT_TEXT((pOp_BackupRcv->usSeqNo == Op_Backup.usSeqNo), xBuff);
					continue;
				}
				sprintf (xBuff, "OpReqOep(): OP_COMM_ERROR usSentSeqNo 0x%x, usRecvSeqNo 0x%x", Op_Backup.usSeqNo, pOp_BackupRcv->usSeqNo);
				NHPOS_ASSERT_TEXT((pOp_BackupRcv->usSeqNo == Op_Backup.usSeqNo), xBuff);
				PifLog(MODULE_OP_BACKUP, LOG_EVENT_OP_REQPARA_RQST_SEQNO);
				PifLog (MODULE_DATA_VALUE(MODULE_OP_BACKUP), OP_OEP_FILE);
				PifLog (MODULE_LINE_NO(MODULE_OP_BACKUP), (USHORT)__LINE__);
				sStatus =  OP_COMM_ERROR;
				break;
			}

			// We check to see if the file being transferred is the same size as what
			// our file is as far as the max number of records.  This is not a record number
			// per se but rather just a check that the OEP file headers are similar.
			if (pOp_BackupRcv->usRecordNo != OepFileHed.usNumberOfMaxOep) {
				sStatus = OP_OEP_FULL;
				break;
			}

			Op_Backup.usSeqNo++;

			Op_WriteOepFile(pOp_BackupRcv->offulFilePosition, &auchRcvBuf[sizeof(OP_BACKUP)], (USHORT)(usRcvLen - sizeof(OP_BACKUP)));

			if (pOp_BackupRcv->uchStatus == OP_END) {        /* Last Message */
				break;
			}
			Op_Backup.offulFilePosition += usRcvLen - sizeof(OP_BACKUP);
		}
		PifCloseFile(hsOpOepFileHandle);
		hsOpOepFileHandle = -1;
	}
//	BlFwIfReloadOEPGroup();
    return(sStatus);
}
/*
*==========================================================================
**    Synopsis:    VOID   OpReqDept(USHORT usLockHnd)
*
*       Input:    usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*
**  Description:
*               Request Dept File Data (for broadcast)
*==========================================================================
*/
SHORT   OpReqDept(USHORT usFcCode, USHORT usLockHnd)
{
    OP_BACKUP       Op_Backup, *pOp_BackupRcv;
    USHORT          usRcvLen;
    UCHAR           auchRcvBuf[OP_BROADCAST_SIZE];
    OPDEPT_FILEHED  DeptFileHed;
    SHORT           sStatus;

    /*  LOCK CHECK */
    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        return(OP_LOCK);
    }

    /*  File Open */
    if ((Op_DeptOpenFile()) == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        if (usFcCode == CLI_FCBAKOPPARA) {
            return(OP_PERFORM);
        }
        return(OP_FILE_NOT_FOUND);
    }

    /* Get File Header */
    Op_ReadDeptFile(OP_FILE_HEAD_POSITION, &DeptFileHed, sizeof(DeptFileHed));

    Op_Backup.uchClass = OP_DEPT_FILE;
    Op_Backup.offulFilePosition = 0L;
    Op_Backup.usSeqNo = 1;

    usRcvLen = OP_BROADCAST_SIZE;
    pOp_BackupRcv = (OP_BACKUP *)auchRcvBuf;

	NHPOS_NONASSERT_TEXT("OpReqDept(): Requesting Department file.");

    for (;;) {
        usRcvLen = OP_BROADCAST_SIZE;
        if (usFcCode == CLI_FCBAKOPPARA) {
            if  ((sStatus = SstReqBackUp(CLI_FCBAKOPPARA, (UCHAR *)&Op_Backup, sizeof(OP_BACKUP), auchRcvBuf, &usRcvLen)) < 0 ) {
				sStatus += STUB_RETCODE;     // move the STUB_ type of error code into the OP_ error code range for use by UicDisplayErrorMsg()
                break;
            }
        } else {
            if  ((sStatus = CstReqOpePara(usFcCode, (UCHAR *)&Op_Backup, sizeof(OP_BACKUP), auchRcvBuf, &usRcvLen)) < 0 ) {
				sStatus += STUB_RETCODE;     // move the STUB_ type of error code into the OP_ error code range for use by UicDisplayErrorMsg()
                break;
            }
        }
        if (pOp_BackupRcv->usSeqNo != Op_Backup.usSeqNo) {
			char  xBuff[128];
			if (Op_Backup.usSeqNo == pOp_BackupRcv->usSeqNo + 1) {
				sprintf (xBuff, "OpReqDept(): OP_COMM_IGNORE usSentSeqNo 0x%x, usRecvSeqNo 0x%x", Op_Backup.usSeqNo, pOp_BackupRcv->usSeqNo);
				NHPOS_ASSERT_TEXT((pOp_BackupRcv->usSeqNo == Op_Backup.usSeqNo), xBuff);
				continue;
			}
			sprintf (xBuff, "OpReqDept(): OP_COMM_ERROR usSentSeqNo 0x%x, usRecvSeqNo 0x%x", Op_Backup.usSeqNo, pOp_BackupRcv->usSeqNo);
			NHPOS_ASSERT_TEXT((pOp_BackupRcv->usSeqNo == Op_Backup.usSeqNo), xBuff);
			PifLog(MODULE_OP_BACKUP, LOG_EVENT_OP_REQPARA_RQST_SEQNO);
			PifLog (MODULE_DATA_VALUE(MODULE_OP_BACKUP), OP_DEPT_FILE);
			PifLog (MODULE_LINE_NO(MODULE_OP_BACKUP), (USHORT)__LINE__);
            sStatus =  OP_COMM_ERROR;
            break;
        }

        if (pOp_BackupRcv->usRecordNo != DeptFileHed.usNumberOfMaxDept) {
            sStatus = OP_DEPT_FULL;
            break;
        }

        Op_Backup.usSeqNo++;

        Op_WriteDeptFile(pOp_BackupRcv->offulFilePosition, &auchRcvBuf[sizeof(OP_BACKUP)], (USHORT)(usRcvLen - sizeof(OP_BACKUP)));

        if (pOp_BackupRcv->uchStatus == OP_END) {        /* Last Message */
            break;
        }
        Op_Backup.offulFilePosition += usRcvLen - sizeof(OP_BACKUP);
    }
    PifCloseFile(hsOpDeptFileHandle);
    return(sStatus);
}
/*
*==========================================================================
**    Synopsis:    SHORT   OpResDept(UCHAR   *puchRcvData,
*                                   USHORT  usRcvLen,
*                                   UCHAR   *puchSndData,
*                                   UCHAR   *pusSndLen)
*
*       Input:    *puchRcvData        * Pointer to Request Data *
*                 usRcvLen            * Width of puchRcvData *
*      Output:    *puchSndData        * Storage location for backup data. *
*       InOut:    *pusSndLen          * Width of puchSndData *
*
**  Return    :   OP_PERFORM
*                 OP_COMM_ERROR;
*
**  Description:  Response Dept File Data (for broadcast)
*==========================================================================
*/
SHORT   OpResDept(UCHAR  *puchRcvData,
                 USHORT  usRcvLen,
                 UCHAR  *puchSndData,
                 USHORT *pusSndLen,
                 USHORT  usLockHnd)
{
    OP_BACKUP       *pOp_BackUpRcv, *pOp_BackUpSnd;
    OPDEPT_FILEHED  DeptFileHed;
    USHORT          usDeptFileSize;

    if (usRcvLen < sizeof(OP_BACKUP)) {
		PifLog (MODULE_OP_PLU, LOG_EVENT_OP_REQPARA_COMM_ERROR);
		PifLog (MODULE_LINE_NO(MODULE_OP_PLU), (USHORT)__LINE__);
		NHPOS_ASSERT_TEXT((usRcvLen >= sizeof(OP_BACKUP)), "OpResDept(): Bad Parameter.");
        return (OP_COMM_ERROR);
    }
    if (*pusSndLen < sizeof(OP_BACKUP)) {
		PifLog (MODULE_OP_PLU, LOG_EVENT_OP_REQPARA_COMM_ERROR);
		PifLog (MODULE_LINE_NO(MODULE_OP_PLU), (USHORT)__LINE__);
		NHPOS_ASSERT_TEXT((*pusSndLen >= sizeof(OP_BACKUP)), "OpResDept(): Bad Parameter.");
        return (OP_COMM_ERROR);
    }

    /*  Request Semaphore */
    PifRequestSem(husOpSem);

    /*  LOCK CHECK */
    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return(OP_LOCK);
    }

    /*  File Open */
    if ((Op_DeptOpenFile()) == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        PifReleaseSem(husOpSem);
        return(OP_FILE_NOT_FOUND);
    }
    /* Get File Header */
    Op_ReadDeptFile(OP_FILE_HEAD_POSITION, &DeptFileHed, sizeof(DeptFileHed));

    pOp_BackUpRcv = (OP_BACKUP *)puchRcvData;
    pOp_BackUpSnd = (OP_BACKUP *)puchSndData;

    /* Make Dept File size */
    usDeptFileSize = (USHORT)DeptFileHed.ulDeptFileSize;

    if (OP_BROADCAST_SIZE < *pusSndLen) {
        *pusSndLen = OP_BROADCAST_SIZE;
    }

    if ( usDeptFileSize <= (USHORT)pOp_BackUpRcv->offulFilePosition ) {
        Op_CloseDeptFileReleaseSem();
		PifLog (MODULE_OP_PLU, LOG_EVENT_OP_REQPARA_COMM_ERROR);
		PifLog (MODULE_LINE_NO(MODULE_OP_PLU), (USHORT)__LINE__);
        return (OP_COMM_ERROR);
    }
    
    if ((usDeptFileSize - pOp_BackUpRcv->offulFilePosition) > (*pusSndLen - sizeof(OP_BACKUP))) {
        pOp_BackUpSnd->offulFilePosition = pOp_BackUpRcv->offulFilePosition;
        pOp_BackUpSnd->uchStatus = OP_CONTINUE;
    } else {
        *pusSndLen = (USHORT)(usDeptFileSize - pOp_BackUpRcv->offulFilePosition + sizeof(OP_BACKUP));
        pOp_BackUpSnd->uchStatus = OP_END;
        pOp_BackUpSnd->offulFilePosition = pOp_BackUpRcv->offulFilePosition;
    }

    pOp_BackUpSnd->usSeqNo   = pOp_BackUpRcv->usSeqNo;
    pOp_BackUpSnd->usRecordNo = DeptFileHed.usNumberOfMaxDept;
    
    Op_ReadDeptFile(pOp_BackUpRcv->offulFilePosition, puchSndData + sizeof(OP_BACKUP), (USHORT)(*pusSndLen - sizeof(OP_BACKUP)));

    Op_CloseDeptFileReleaseSem();
    return (OP_PERFORM);   
}

/*
*==========================================================================
**    Synopsis:    VOID   OpReqCpn(USHORT usFcCode, USHORT usLockHnd)
*
*       Input:    usLockHnd
*                 usFcCode
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*
**  Description:
*               Request Combination Coupon File Data (for broadcast)
*==========================================================================
*/
SHORT   OpReqCpn(USHORT usFcCode, USHORT usLockHnd)
{
    OP_BACKUP       Op_Backup, *pOp_BackupRcv;
    USHORT          usRcvLen;
    UCHAR           auchRcvBuf[OP_BROADCAST_SIZE];
    OPCPN_FILEHED   CpnFileHed;
    SHORT           sStatus;

    /*  LOCK CHECK */
    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        return(OP_LOCK);
    }

    /*  File Open */
    if ((Op_CpnOpenFile()) == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        if (usFcCode == CLI_FCBAKOPPARA) {
            return(OP_PERFORM);
        }
        return(OP_FILE_NOT_FOUND);
    }

    /* Get File Header */
    Op_ReadCpnFile(OP_FILE_HEAD_POSITION, &CpnFileHed, sizeof(CpnFileHed));

    Op_Backup.uchClass = OP_CPN_FILE;
    Op_Backup.offulFilePosition = 0L;
    Op_Backup.usSeqNo = 1;

    usRcvLen = OP_BROADCAST_SIZE;
    pOp_BackupRcv = (OP_BACKUP *)auchRcvBuf;

	NHPOS_NONASSERT_TEXT("OpReqCpn(): Requesting Coupon file.");

    for (;;) {
        usRcvLen = OP_BROADCAST_SIZE;
        if (usFcCode == CLI_FCBAKOPPARA) {
            if  ((sStatus = SstReqBackUp(CLI_FCBAKOPPARA, (UCHAR *)&Op_Backup, sizeof(OP_BACKUP), auchRcvBuf, &usRcvLen)) < 0 ) {
				sStatus += STUB_RETCODE;     // move the STUB_ type of error code into the OP_ error code range for use by UicDisplayErrorMsg()
                break;
            }

        } else {
            if  ((sStatus = CstReqOpePara(usFcCode, (UCHAR *)&Op_Backup, sizeof(OP_BACKUP), auchRcvBuf, &usRcvLen)) < 0 ) {
				sStatus += STUB_RETCODE;     // move the STUB_ type of error code into the OP_ error code range for use by UicDisplayErrorMsg()
                break;
            }
        }

        if (pOp_BackupRcv->usSeqNo != Op_Backup.usSeqNo) {
			char  xBuff[128];
			if (Op_Backup.usSeqNo == pOp_BackupRcv->usSeqNo + 1) {
				sprintf (xBuff, "OpReqCpn(): OP_COMM_IGNORE usSentSeqNo 0x%x, usRecvSeqNo 0x%x", Op_Backup.usSeqNo, pOp_BackupRcv->usSeqNo);
				NHPOS_ASSERT_TEXT((pOp_BackupRcv->usSeqNo == Op_Backup.usSeqNo), xBuff);
				continue;
			}
			sprintf (xBuff, "OpReqCpn(): OP_COMM_ERROR usSentSeqNo 0x%x, usRecvSeqNo 0x%x", Op_Backup.usSeqNo, pOp_BackupRcv->usSeqNo);
			NHPOS_ASSERT_TEXT((pOp_BackupRcv->usSeqNo == Op_Backup.usSeqNo), xBuff);
			PifLog(MODULE_OP_BACKUP, LOG_EVENT_OP_REQPARA_RQST_SEQNO);
			PifLog (MODULE_DATA_VALUE(MODULE_OP_BACKUP), OP_CPN_FILE);
			PifLog (MODULE_LINE_NO(MODULE_OP_BACKUP), (USHORT)__LINE__);
            sStatus =  OP_COMM_ERROR;
            break;
        }

        if (pOp_BackupRcv->usRecordNo != CpnFileHed.usNumberOfCpn) {
            sStatus = OP_PLU_FULL;
            break;
        }

        Op_Backup.usSeqNo++;

        Op_WriteCpnFile(pOp_BackupRcv->offulFilePosition, &auchRcvBuf[sizeof(OP_BACKUP)], (USHORT)(usRcvLen - sizeof(OP_BACKUP)));

        if (pOp_BackupRcv->uchStatus == OP_END) {        /* Last Message */
            break;
        }
        Op_Backup.offulFilePosition += usRcvLen - sizeof(OP_BACKUP);
    }
    PifCloseFile(hsOpCpnFileHandle);
    return(sStatus);
}
/*
*==========================================================================
**    Synopsis:    SHORT   OpResCpn(UCHAR   *puchRcvData,
*                                   USHORT  usRcvLen,
*                                   UCHAR   *puchSndData,
*                                   UCHAR   *pusSndLen)
*
*       Input:    *puchRcvData        * Pointer to Request Data *
*                 usRcvLen            * Width of puchRcvData *
*      Output:    *puchSndData        * Storage location for backup data. *
*       InOut:    *pusSndLen          * Width of puchSndData *
*
**  Return    :   OP_PERFORM
*                 OP_COMM_ERROR;
*
**  Description:  Response Combination Coupon File Data (for broadcast)
*==========================================================================
*/
SHORT   OpResCpn(UCHAR  *puchRcvData,
                 USHORT  usRcvLen,
                 UCHAR  *puchSndData,
                 USHORT *pusSndLen,
                 USHORT  usLockHnd)
{
    OP_BACKUP       *pOp_BackUpRcv, *pOp_BackUpSnd;
    OPCPN_FILEHED   CpnFileHed;
    USHORT          usCpnFileSize;

    if (usRcvLen < sizeof(OP_BACKUP)) {
		PifLog (MODULE_OP_PLU, LOG_EVENT_OP_REQPARA_COMM_ERROR);
		PifLog (MODULE_LINE_NO(MODULE_OP_PLU), (USHORT)__LINE__);
		NHPOS_ASSERT_TEXT((usRcvLen >= sizeof(OP_BACKUP)), "OpResCpn(): Bad Parameter.");
        return (OP_COMM_ERROR);
    }

    if (*pusSndLen < sizeof(OP_BACKUP)) {
		PifLog (MODULE_OP_PLU, LOG_EVENT_OP_REQPARA_COMM_ERROR);
		PifLog (MODULE_LINE_NO(MODULE_OP_PLU), (USHORT)__LINE__);
		NHPOS_ASSERT_TEXT((*pusSndLen >= sizeof(OP_BACKUP)), "OpResCpn(): Bad Parameter.");
        return (OP_COMM_ERROR);
    }

    /*  Request Semaphore */
    PifRequestSem(husOpSem);

    /*  LOCK CHECK */
    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return(OP_LOCK);
    }

    /*  File Open */
    if ((Op_CpnOpenFile()) == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        PifReleaseSem(husOpSem);
        return(OP_FILE_NOT_FOUND);
    }
    /* Get File Header */
    Op_ReadCpnFile(OP_FILE_HEAD_POSITION, &CpnFileHed, sizeof(CpnFileHed));

    pOp_BackUpRcv = (OP_BACKUP *)puchRcvData;
    pOp_BackUpSnd = (OP_BACKUP *)puchSndData;

    /* Make Dept File size */
    usCpnFileSize = (CpnFileHed.usNumberOfCpn * sizeof(COMCOUPONPARA)) + sizeof(OPCPN_FILEHED);

    if (OP_BROADCAST_SIZE < *pusSndLen) {
        *pusSndLen = OP_BROADCAST_SIZE;
    }

    if ( usCpnFileSize <= (USHORT)pOp_BackUpRcv->offulFilePosition ) {
        Op_CloseCpnFileReleaseSem();
		PifLog (MODULE_OP_PLU, LOG_EVENT_OP_REQPARA_COMM_ERROR);
		PifLog (MODULE_LINE_NO(MODULE_OP_PLU), (USHORT)__LINE__);
        return (OP_COMM_ERROR);
    }
    
    if ((usCpnFileSize - pOp_BackUpRcv->offulFilePosition) > (*pusSndLen - sizeof(OP_BACKUP))) {
        pOp_BackUpSnd->offulFilePosition = pOp_BackUpRcv->offulFilePosition;
        pOp_BackUpSnd->uchStatus = OP_CONTINUE;

    } else {
        *pusSndLen = (USHORT)(usCpnFileSize - pOp_BackUpRcv->offulFilePosition + sizeof(OP_BACKUP));
        pOp_BackUpSnd->uchStatus = OP_END;
        pOp_BackUpSnd->offulFilePosition = pOp_BackUpRcv->offulFilePosition;
    }

    pOp_BackUpSnd->usSeqNo   = pOp_BackUpRcv->usSeqNo;
    pOp_BackUpSnd->usRecordNo = CpnFileHed.usNumberOfCpn;
    
    Op_ReadCpnFile(pOp_BackUpRcv->offulFilePosition, puchSndData + sizeof(OP_BACKUP), (USHORT)(*pusSndLen - sizeof(OP_BACKUP)));

    Op_CloseCpnFileReleaseSem();
    return (OP_PERFORM);   
}

/*
*==========================================================================
**    Synopsis:    VOID   OpReqCstr(USHORT usFcCode, USHORT usLockHnd)
*
*       Input:    usLockHnd
*                 usFcCode
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*
**  Description:
*               Request Control String File Data (for broadcast)
*==========================================================================
*/
SHORT   OpReqCstr(USHORT usFcCode, USHORT usLockHnd)
{
    OP_BACKUP       Op_Backup, *pOp_BackupRcv;
    USHORT          usRcvLen;
    UCHAR           auchRcvBuf[OP_BROADCAST_SIZE];
    OPCSTR_FILEHED  CstrFileHed;
    SHORT           sStatus;

    /*  LOCK CHECK */
    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        return(OP_LOCK);
    }

    /*  File Open */
    if ((Op_CstrOpenFile()) == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        if (usFcCode == CLI_FCBAKOPPARA) {
            return(OP_PERFORM);
        }
        return(OP_FILE_NOT_FOUND);
    }

    /* Get File Header */

    Op_ReadCstrFile(OP_FILE_HEAD_POSITION, &CstrFileHed, sizeof(CstrFileHed));

    Op_Backup.uchClass = OP_CSTR_FILE;
    Op_Backup.offulFilePosition = 0L;
    Op_Backup.usSeqNo = 1;

    usRcvLen = OP_BROADCAST_SIZE;
    pOp_BackupRcv = (OP_BACKUP *)auchRcvBuf;

	NHPOS_NONASSERT_TEXT("OpReqCstr(): Requesting Control String file.");

    for (;;) {
        usRcvLen = OP_BROADCAST_SIZE;
        if (usFcCode == CLI_FCBAKOPPARA) {
            if  ((sStatus = SstReqBackUp(CLI_FCBAKOPPARA, (UCHAR *)&Op_Backup, sizeof(OP_BACKUP), auchRcvBuf, &usRcvLen)) < 0 ) {
				sStatus += STUB_RETCODE;     // move the STUB_ type of error code into the OP_ error code range for use by UicDisplayErrorMsg()
                break;
            }
        } else {
            if  ((sStatus = CstReqOpePara(usFcCode, (UCHAR *)&Op_Backup, sizeof(OP_BACKUP), auchRcvBuf, &usRcvLen)) < 0 ) {
				sStatus += STUB_RETCODE;     // move the STUB_ type of error code into the OP_ error code range for use by UicDisplayErrorMsg()
                break;
            }
        }

        if (pOp_BackupRcv->usSeqNo != Op_Backup.usSeqNo) {
			char  xBuff[128];
			if (Op_Backup.usSeqNo == pOp_BackupRcv->usSeqNo + 1) {
				sprintf (xBuff, "OpReqCstr(): OP_COMM_IGNORE usSentSeqNo 0x%x, usRecvSeqNo 0x%x", Op_Backup.usSeqNo, pOp_BackupRcv->usSeqNo);
				NHPOS_ASSERT_TEXT((pOp_BackupRcv->usSeqNo == Op_Backup.usSeqNo), xBuff);
				continue;
			}
			sprintf (xBuff, "OpReqCstr(): OP_COMM_ERROR usSentSeqNo 0x%x, usRecvSeqNo 0x%x", Op_Backup.usSeqNo, pOp_BackupRcv->usSeqNo);
			NHPOS_ASSERT_TEXT((pOp_BackupRcv->usSeqNo == Op_Backup.usSeqNo), xBuff);
			PifLog(MODULE_OP_BACKUP, LOG_EVENT_OP_REQPARA_RQST_SEQNO);
			PifLog (MODULE_DATA_VALUE(MODULE_OP_BACKUP), OP_CSTR_FILE);
			PifLog (MODULE_LINE_NO(MODULE_OP_BACKUP), (USHORT)__LINE__);
            sStatus =  OP_COMM_ERROR;
            break;
        }

        if (pOp_BackupRcv->usRecordNo != CstrFileHed.usNumberOfCstr) {
            sStatus = OP_PLU_FULL;
            break;
        }

        Op_Backup.usSeqNo++;

        Op_WriteCstrFile(pOp_BackupRcv->offulFilePosition, &auchRcvBuf[sizeof(OP_BACKUP)], (USHORT)(usRcvLen - sizeof(OP_BACKUP)));

        if (pOp_BackupRcv->uchStatus == OP_END) {        /* Last Message */
            break;
        }
        Op_Backup.offulFilePosition += usRcvLen - sizeof(OP_BACKUP);
    }
    PifCloseFile(hsOpCstrFileHandle);
    return(sStatus);
}
/*
*==========================================================================
**    Synopsis:    SHORT   OpResCstr(UCHAR   *puchRcvData,
*                                    USHORT  usRcvLen,
*                                    UCHAR   *puchSndData,
*                                    UCHAR   *pusSndLen)
*
*       Input:    *puchRcvData        * Pointer to Request Data *
*                 usRcvLen            * Width of puchRcvData *
*      Output:    *puchSndData        * Storage location for backup data. *
*       InOut:    *pusSndLen          * Width of puchSndData *
*
**  Return    :   OP_PERFORM
*                 OP_COMM_ERROR;
*
**  Description:  Response Control String File Data (for broadcast)
*==========================================================================
*/
SHORT   OpResCstr(UCHAR  *puchRcvData,
                  USHORT  usRcvLen,
                  UCHAR  *puchSndData,
                  USHORT *pusSndLen,
                  USHORT  usLockHnd)
{
    OP_BACKUP       *pOp_BackUpRcv, *pOp_BackUpSnd;
    OPCSTR_FILEHED  CstrFileHed;
    OPCSTR_INDEXENTRY ausIndexTable[OP_CSTR_INDEX_SIZE];
    USHORT          usCstrFileSize;

    if (usRcvLen < sizeof(OP_BACKUP)) {
		PifLog (MODULE_OP_PLU, LOG_EVENT_OP_REQPARA_COMM_ERROR);
		PifLog (MODULE_LINE_NO(MODULE_OP_PLU), (USHORT)__LINE__);
		NHPOS_ASSERT_TEXT((usRcvLen >= sizeof(OP_BACKUP)), "OpResCstr(): Bad Parameter.");
        return (OP_COMM_ERROR);
    }
    if (*pusSndLen < sizeof(OP_BACKUP)) {
		PifLog (MODULE_OP_PLU, LOG_EVENT_OP_REQPARA_COMM_ERROR);
		PifLog (MODULE_LINE_NO(MODULE_OP_PLU), (USHORT)__LINE__);
		NHPOS_ASSERT_TEXT((*pusSndLen >= sizeof(OP_BACKUP)), "OpResCstr(): Bad Parameter.");
        return (OP_COMM_ERROR);
    }

    /*  Request Semaphore */
    PifRequestSem(husOpSem);

    /*  LOCK CHECK */
    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return(OP_LOCK);
    }

    /*  File Open */
    if ((Op_CstrOpenFile()) == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        PifReleaseSem(husOpSem);
        return(OP_FILE_NOT_FOUND);
    }
    /* Get File Header */
    Op_ReadCstrFile(OP_FILE_HEAD_POSITION, &CstrFileHed, sizeof(CstrFileHed));

    pOp_BackUpRcv = (OP_BACKUP *)puchRcvData;
    pOp_BackUpSnd = (OP_BACKUP *)puchSndData;

    /* Make Cstr File size */
    usCstrFileSize = (sizeof(OPCSTR_FILEHED) + sizeof(ausIndexTable) + (CstrFileHed.usNumberOfCstr * sizeof(OPCSTR_PARAENTRY)));

    if (OP_BROADCAST_SIZE < *pusSndLen) {
        *pusSndLen = OP_BROADCAST_SIZE;
    }

    if ( usCstrFileSize <= (USHORT)pOp_BackUpRcv->offulFilePosition ) {
        Op_CloseCstrFileReleaseSem();
		PifLog (MODULE_OP_PLU, LOG_EVENT_OP_REQPARA_COMM_ERROR);
		PifLog (MODULE_LINE_NO(MODULE_OP_PLU), (USHORT)__LINE__);
        return (OP_COMM_ERROR);
    }
    
    if ((usCstrFileSize - pOp_BackUpRcv->offulFilePosition) > (*pusSndLen - sizeof(OP_BACKUP))) {
        pOp_BackUpSnd->offulFilePosition = pOp_BackUpRcv->offulFilePosition;
        pOp_BackUpSnd->uchStatus = OP_CONTINUE;
    } else {
        *pusSndLen = (USHORT)(usCstrFileSize - pOp_BackUpRcv->offulFilePosition + sizeof(OP_BACKUP));
        pOp_BackUpSnd->uchStatus = OP_END;
        pOp_BackUpSnd->offulFilePosition = pOp_BackUpRcv->offulFilePosition;
    }

    pOp_BackUpSnd->usSeqNo   = pOp_BackUpRcv->usSeqNo;
    pOp_BackUpSnd->usRecordNo = CstrFileHed.usNumberOfCstr;
    
    Op_ReadCstrFile(pOp_BackUpRcv->offulFilePosition, puchSndData + sizeof(OP_BACKUP), (USHORT)(*pusSndLen - sizeof(OP_BACKUP)));

    Op_CloseCstrFileReleaseSem();
    return (OP_PERFORM);   
    ausIndexTable[0] = ausIndexTable[0]; /* Dummy *//* ### ADD (2171 for Win32) V1.0 */
}
/*
*==========================================================================
**    Synopsis:   VOID   Op_AllFileCreate(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   OP_PERFORM
*                 OP_COMM_ERROR;
*
**  Description:
*               Create All Paramater File.
*==========================================================================
*/
VOID   Op_AllFileCreate(VOID)
{
    PARAFLEXMEM     ParaFlexMem;
    SHORT           sStatus;
    UCHAR           cuchFile, uchType;

	NHPOS_NONASSERT_TEXT("Op_AllFileCreate(): Recreate Database files.");

    ParaFlexMem.uchMajorClass = CLASS_PARAFLEXMEM;
    for (cuchFile = 0; auchFlexMem[cuchFile] != OP_EOT; cuchFile++) {
        uchType = auchFlexMem[cuchFile];
        if ((sStatus = CliCreateFile(uchType, 0, 0 )) != 0) {
            ApplTrace.usParaF  = (USHORT)uchType;  /* Save sStatus */
            ApplTrace.usStatus = (USHORT)sStatus;

			PifLog (MODULE_OP_BACKUP, LOG_EVENT_OP_CREATEFILE_FLEX00);
            PifLog(MODULE_DATA_VALUE(MODULE_OP_BACKUP), cuchFile);
            PifLog(MODULE_ERROR_NO(MODULE_OP_BACKUP), (USHORT)abs(sStatus));
            PifAbort(MODULE_OP_BACKUP, FAULT_BAD_ENVIRONMENT);
        }
    }

    for (cuchFile = 0; auchFlexMem[cuchFile] != OP_EOT; cuchFile++) {
        uchType = ParaFlexMem.uchAddress = auchFlexMem[cuchFile];
        CliParaRead(&ParaFlexMem);
        if ((sStatus = CliCreateFile(uchType, ParaFlexMem.ulRecordNumber, ParaFlexMem.uchPTDFlag)) != 0) {
            ApplTrace.usParaF  = (USHORT)uchType;  /* Save sStatus */
            ApplTrace.usStatus = (USHORT)sStatus;

			PifLog (MODULE_OP_BACKUP, LOG_EVENT_OP_CREATEFILE_FLEX);
            PifLog(MODULE_DATA_VALUE(MODULE_OP_BACKUP), cuchFile);
            PifLog(MODULE_ERROR_NO(MODULE_OP_BACKUP), (USHORT)abs(sStatus));
            PifAbort(MODULE_OP_BACKUP, FAULT_BAD_ENVIRONMENT);
        }
    }

    /*----- Adds for enhance -------------------------*/
    ParaFlexMem.uchAddress = FLEX_EJ_ADR;
    CliParaRead(&ParaFlexMem);

    CliCheckDeleteFile(FLEX_EJ_ADR, ParaFlexMem.ulRecordNumber, ParaFlexMem.uchPTDFlag);

    if (EJ_PERFORM != (sStatus = CliCheckCreateFile(FLEX_EJ_ADR, ParaFlexMem.ulRecordNumber, ParaFlexMem.uchPTDFlag)) ) {
        ApplTrace.usParaF  = (USHORT)FLEX_EJ_ADR;  /* Save sStatus */
        ApplTrace.usStatus = (USHORT)sStatus;

		PifLog (MODULE_OP_BACKUP, LOG_EVENT_OP_CREATEFILE_EJ);
        PifLog(MODULE_ERROR_NO(MODULE_OP_BACKUP), (USHORT)abs(sStatus));
        PifAbort(MODULE_OP_BACKUP, FAULT_BAD_ENVIRONMENT);
    }
    /*----- Adds for enhance -------------------------*/
}

VOID   Op_AllTotalFileCreate(VOID)
{
    PARAFLEXMEM     ParaFlexMem;
    SHORT           sError;
    UCHAR           cuchFile, uchType;
	UCHAR FARCONST  auchFlexMemTotals[] =    {
						FLEX_DEPT_ADR,
                        FLEX_CAS_ADR,
                        FLEX_CPN_ADR,
                        OP_EOT};

	NHPOS_NONASSERT_TEXT("Op_AllTotalFileCreate(): Recreate Total files.");

    ParaFlexMem.uchMajorClass = CLASS_PARAFLEXMEM;
    for (cuchFile = 0; auchFlexMemTotals[cuchFile] != OP_EOT; cuchFile++) {
        uchType = ParaFlexMem.uchAddress = auchFlexMemTotals[cuchFile];
        CliParaRead(&ParaFlexMem);
        switch (ParaFlexMem.uchAddress) {
			case FLEX_DEPT_ADR:                 /* dept file creation */
                sError = TtlDeptCreate((USHORT)ParaFlexMem.ulRecordNumber, ParaFlexMem.uchPTDFlag, TTL_COMP_CREATE);
				break;

			case FLEX_CAS_ADR:                  /* Cashier File creation */
                sError = TtlCasCreate((USHORT)ParaFlexMem.ulRecordNumber, ParaFlexMem.uchPTDFlag, TTL_COMP_CREATE); /* V3.3 */
			break;

			case FLEX_CPN_ADR:                 /* Combination Coupon file creation */
                sError = TtlCpnCreate((USHORT)ParaFlexMem.ulRecordNumber, ParaFlexMem.uchPTDFlag, TTL_COMP_CREATE);
			break;

			case FLEX_GC_ADR:                   /* GCF creation */
                if (FLEX_STORE_RECALL == ParaFlexMem.uchPTDFlag) {
                    sError = TtlSerTimeCreate(TTL_CHECK_CREATE);
                } else {
                    sError = TtlSerTimeDeleteFile();
                }
			break;

			default:
				break;
		}
    }
}

/*    Saved memory size   8-25-93     */
/*    Saved memory size   8-25-93     */

/*                 
*==========================================================================
**    Synopsis:    VOID   Op_PluAbort(USHORT usCode)
*
*       Input:    usCode
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description:  System halt.
*==========================================================================
*/
VOID  Op_PluAbort(USHORT usCode)
{
    PifAbort(MODULE_OP_PLU, usCode);
}



/*                 
*==========================================================================
**    Synopsis:   SHORT  Op_DeptOpenFile(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description:  Open "PARAMPLU" file.
*==========================================================================
*/
SHORT  Op_DeptOpenFile(VOID)
{
    hsOpDeptFileHandle = PifOpenFile(auchOP_DEPT, auchOLD_FILE_READ_WRITE);
	if (hsOpDeptFileHandle < 0) {
		char xBuff[128];
		sprintf(xBuff, "**ERROR: Op_DeptOpenFile() error %d", hsOpDeptFileHandle);
		NHPOS_ASSERT_TEXT((hsOpDeptFileHandle >= 0), xBuff);
	}
    return (hsOpDeptFileHandle);
}

/*    Saved memory size   8-25-93      */
/*    Saved memory size   8-25-93      */

/*                 
*==========================================================================
**    Synopsis:   SHORT  Op_CpnOpenFile(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description:  Open "PARAMCPN" file.
*==========================================================================
*/
SHORT  Op_CpnOpenFile(VOID)
{
    hsOpCpnFileHandle = PifOpenFile(auchOP_CPN, auchOLD_FILE_READ_WRITE);
    return (hsOpCpnFileHandle);
}

/*                 
*==========================================================================
**    Synopsis:   SHORT  Op_CstrOpenFile(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description:  Open "PARAMCST" file.
*==========================================================================
*/
SHORT  Op_CstrOpenFile(VOID)
{
    hsOpCstrFileHandle = PifOpenFile(auchOP_CSTR, auchOLD_FILE_READ_WRITE);
    return (hsOpCstrFileHandle);
}

/******************** Add New Functions (Release 3.1) BEGIN **************/
/*
*==========================================================================
**    Synopsis:    SHORT    OpReqPpi(USHORT usFcCode, USHORT usLockHnd)
*
*       Input:    usLockHnd
*                 usFcCode
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*
**  Description:
*               Request Promotional Pricing Item File Data (for broadcast)
*==========================================================================
*/
SHORT   OpReqPpi(USHORT usFcCode, USHORT usLockHnd)
{
    OP_BACKUP       Op_Backup, *pOp_BackupRcv;
    USHORT          usRcvLen;
    UCHAR           auchRcvBuf[OP_BROADCAST_SIZE];
    OPPPI_FILEHED   PpiFileHed;
    SHORT           sStatus;

    /*  LOCK CHECK */
    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        return(OP_LOCK);
    }

    /*  File Open */
    if ((Op_PpiOpenFile()) == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        if (usFcCode == CLI_FCBAKOPPARA) {
            return(OP_PERFORM);
        }
        return(OP_FILE_NOT_FOUND);
    }

    /* Get File Header */
    Op_ReadPpiFile(OP_FILE_HEAD_POSITION, &PpiFileHed, sizeof(PpiFileHed));

    Op_Backup.uchClass = OP_PPI_FILE;
    Op_Backup.offulFilePosition = 0L;
    Op_Backup.usSeqNo = 1;

    usRcvLen = OP_BROADCAST_SIZE;
    pOp_BackupRcv = (OP_BACKUP *)auchRcvBuf;

	NHPOS_NONASSERT_TEXT("OpReqPpi(): Transfering PPI file.");

    for (;;) {
        usRcvLen = OP_BROADCAST_SIZE;
        if (usFcCode == CLI_FCBAKOPPARA) {
            if  ((sStatus = SstReqBackUp(CLI_FCBAKOPPARA, (UCHAR *)&Op_Backup, sizeof(OP_BACKUP), auchRcvBuf, &usRcvLen)) < 0 ) {
				sStatus += STUB_RETCODE;     // move the STUB_ type of error code into the OP_ error code range for use by UicDisplayErrorMsg()
                break;
            }
        } else {
            if  ((sStatus = CstReqOpePara(usFcCode, (UCHAR *)&Op_Backup, sizeof(OP_BACKUP), auchRcvBuf, &usRcvLen)) < 0 ) {
				sStatus += STUB_RETCODE;     // move the STUB_ type of error code into the OP_ error code range for use by UicDisplayErrorMsg()
                break;
            }
        }

        if (pOp_BackupRcv->usSeqNo != Op_Backup.usSeqNo) {
			char  xBuff[128];
			if (Op_Backup.usSeqNo == pOp_BackupRcv->usSeqNo + 1) {
				sprintf (xBuff, "OpReqPpi(): OP_COMM_IGNORE usSentSeqNo 0x%x, usRecvSeqNo 0x%x", Op_Backup.usSeqNo, pOp_BackupRcv->usSeqNo);
				NHPOS_ASSERT_TEXT((pOp_BackupRcv->usSeqNo == Op_Backup.usSeqNo), xBuff);
				continue;
			}
			sprintf (xBuff, "OpReqPpi(): OP_COMM_ERROR usSentSeqNo 0x%x, usRecvSeqNo 0x%x", Op_Backup.usSeqNo, pOp_BackupRcv->usSeqNo);
			NHPOS_ASSERT_TEXT((pOp_BackupRcv->usSeqNo == Op_Backup.usSeqNo), xBuff);
			PifLog(MODULE_OP_BACKUP, LOG_EVENT_OP_REQPARA_RQST_SEQNO);
			PifLog (MODULE_DATA_VALUE(MODULE_OP_BACKUP), OP_PPI_FILE);
			PifLog (MODULE_LINE_NO(MODULE_OP_BACKUP), (USHORT)__LINE__);
            sStatus =  OP_COMM_ERROR;
            break;
        }

        if (pOp_BackupRcv->usRecordNo != PpiFileHed.usNumberOfPpi) {
            sStatus = OP_FILE_FULL;
            break;
        }

        Op_Backup.usSeqNo++;

        Op_WritePpiFile(pOp_BackupRcv->offulFilePosition, &auchRcvBuf[sizeof(OP_BACKUP)], (USHORT)(usRcvLen - sizeof(OP_BACKUP)));

        if (pOp_BackupRcv->uchStatus == OP_END) {        /* Last Message */
            break;
        }
        Op_Backup.offulFilePosition += usRcvLen - sizeof(OP_BACKUP);
    }
    PifCloseFile(hsOpPpiFileHandle);
    return(sStatus);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpResPpi(UCHAR   *puchRcvData,
*                                   USHORT  usRcvLen,
*                                   UCHAR   *puchSndData,
*                                   UCHAR   *pusSndLen)
*
*       Input:    *puchRcvData        * Pointer to Request Data *
*                 usRcvLen            * Width of puchRcvData *
*      Output:    *puchSndData        * Storage location for backup data. *
*       InOut:    *pusSndLen          * Width of puchSndData *
*
**  Return    :   OP_PERFORM
*                 OP_COMM_ERROR;
*
**  Description:  Response Promotional Pricing Item File Data (for broadcast)
*==========================================================================
*/
SHORT   OpResPpi(UCHAR  *puchRcvData,
                 USHORT  usRcvLen,
                 UCHAR  *puchSndData,
                 USHORT *pusSndLen,
                 USHORT  usLockHnd)
{
    OP_BACKUP      *pOp_BackUpRcv, *pOp_BackUpSnd;
    OPPPI_FILEHED  PpiFileHed;
    USHORT         usPpiFileSize;

    if (usRcvLen < sizeof(OP_BACKUP)) {
		PifLog (MODULE_OP_PLU, LOG_EVENT_OP_REQPARA_COMM_ERROR);
		PifLog (MODULE_LINE_NO(MODULE_OP_PLU), (USHORT)__LINE__);
		NHPOS_ASSERT_TEXT((usRcvLen >= sizeof(OP_BACKUP)), "OpResPpi(): Bad Parameter.");
        return (OP_COMM_ERROR);
    }

    if (*pusSndLen < sizeof(OP_BACKUP)) {
		PifLog (MODULE_OP_PLU, LOG_EVENT_OP_REQPARA_COMM_ERROR);
		PifLog (MODULE_LINE_NO(MODULE_OP_PLU), (USHORT)__LINE__);
		NHPOS_ASSERT_TEXT((*pusSndLen >= sizeof(OP_BACKUP)), "OpResPpi(): Bad Parameter.");
        return (OP_COMM_ERROR);
    }

    /*  Request Semaphore */
    PifRequestSem(husOpSem);

    /*  LOCK CHECK */
    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return(OP_LOCK);
    }

    /*  File Open */
    if ((Op_PpiOpenFile()) == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        PifReleaseSem(husOpSem);
        return(OP_FILE_NOT_FOUND);
    }

    /* Get File Header */
    Op_ReadPpiFile(OP_FILE_HEAD_POSITION, &PpiFileHed, sizeof(PpiFileHed));

    pOp_BackUpRcv = (OP_BACKUP *)puchRcvData;
    pOp_BackUpSnd = (OP_BACKUP *)puchSndData;

    /* Make Ppi File size */
    usPpiFileSize = (PpiFileHed.usNumberOfPpi * sizeof(OPPPI_PARAENTRY)) + sizeof(OPPPI_FILEHED);

    if (OP_BROADCAST_SIZE < *pusSndLen) {
        *pusSndLen = OP_BROADCAST_SIZE;
    }

    if ( usPpiFileSize <= (USHORT)pOp_BackUpRcv->offulFilePosition) {
        Op_ClosePpiFileReleaseSem();
        return (OP_COMM_ERROR);
    }
    
    if ((usPpiFileSize - pOp_BackUpRcv->offulFilePosition) > (*pusSndLen - sizeof(OP_BACKUP))) {
        pOp_BackUpSnd->offulFilePosition = pOp_BackUpRcv->offulFilePosition;
        pOp_BackUpSnd->uchStatus = OP_CONTINUE;
    } else {
        *pusSndLen = (USHORT)(usPpiFileSize - pOp_BackUpRcv->offulFilePosition) + sizeof(OP_BACKUP);
        pOp_BackUpSnd->uchStatus = OP_END;
        pOp_BackUpSnd->offulFilePosition = pOp_BackUpRcv->offulFilePosition;
    }

    pOp_BackUpSnd->usSeqNo   = pOp_BackUpRcv->usSeqNo;
    pOp_BackUpSnd->usRecordNo = PpiFileHed.usNumberOfPpi;

    Op_ReadPpiFile(pOp_BackUpRcv->offulFilePosition, puchSndData + sizeof(OP_BACKUP), (USHORT)(*pusSndLen - sizeof(OP_BACKUP)));

    Op_ClosePpiFileReleaseSem();
    return (OP_PERFORM);   
}

/*                 
*==========================================================================
**    Synopsis:   SHORT  Op_PpiOpenFile(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description:  Open "PARAPPI" file.
*==========================================================================
*/
SHORT  Op_PpiOpenFile(VOID)
{
    hsOpPpiFileHandle = PifOpenFile(auchOP_PPI, auchOLD_FILE_READ_WRITE);
    return (hsOpPpiFileHandle);
}

/*
*==========================================================================
**    Synopsis:    VOID    Op_ClosePpiFileReleaseSem(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing 
*
**  Description:
*               Close Promotioanl Pricing Item file and Release semaphore
*==========================================================================
*/
VOID    Op_ClosePpiFileReleaseSem(VOID)
{
    PifCloseFile(hsOpPpiFileHandle);
    PifReleaseSem(husOpSem);
}

/*
*==========================================================================
**    Synopsis:  SHORT Op_ReadPpiFile(ULONG  offulFileSeek,
*                                 VOID   *pTableHeadAddress,
*                                 ULONG ulSizeofRead)
*
*       Input:  offulFileSeek           Number of bytes from the biginning of
*                                       the Promotional Pricing Item file.
*               ulSizeofRead            Number of bytes to be read.
*      Output:  *pTableHeadAddress      Storage location of items to be read.
*       InOut:  Nothing
*
**  Return    : OP_PERFORM
*               OP_NO_READ_SIZE                  Read size is 0
*               (File error = system halt)
*
**  Description: Read data in Promotional Pricing file.
*==========================================================================
*/
SHORT   Op_ReadPpiFile(ULONG offulFileSeek,
                       VOID  *pTableHeadAddress,
                       ULONG ulSizeofRead)
{
    ULONG   ulActualPosition;
	ULONG	ulActualBytesRead;	//RPH 11-10-3

    if (ulSizeofRead == 0X00) {
        return (OP_NO_READ_SIZE);
    }
    
    if ((PifSeekFile(hsOpPpiFileHandle, offulFileSeek, &ulActualPosition)) < 0) {
        PifAbort(MODULE_OP_PPI, FAULT_ERROR_FILE_SEEK);
    }

	//RPH 11-10-3 Changes for PifReadFile
	PifReadFile(hsOpPpiFileHandle, pTableHeadAddress, ulSizeofRead, &ulActualBytesRead);
    if ( ulActualBytesRead != ulSizeofRead) {
        PifAbort(MODULE_OP_PPI, FAULT_ERROR_FILE_READ);
    }
    return(OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   Op_WritePpiFile(ULONG  offulFileSeek,
*                                          VOID   *pTableHeadAddress, 
*                                          USHORT usSizeofWrite)
*
*       Input:   offulFileSeek           Number of bytes from the biginning of the 
*                                        Promotional Pricing Item file.
*                *pTableHeadAddress      Pointer to data to be written.
*                usSizeofWrite           Number of bytes to be written.
*      Output:   Nothing
*       InOut:   Nothing
*
**  Return    :  Nothing (error is system error (progrum is abort)
*
**  Description: Write data in Promotional Pricing Item File.
*==========================================================================
*/
VOID   Op_WritePpiFile(ULONG offulFileSeek,
                       VOID  *pTableHeadAddress,
                       USHORT usSizeofWrite)
{
    ULONG   ulActualPosition;

    if (!usSizeofWrite) {
        return;
    }
    if ( PifSeekFile(hsOpPpiFileHandle, offulFileSeek, &ulActualPosition) < 0) {
        PifAbort(MODULE_OP_PPI, FAULT_ERROR_FILE_SEEK);
    }
    PifWriteFile(hsOpPpiFileHandle, pTableHeadAddress, usSizeofWrite);
}


/******************** Add New Functions (Release 3.1) END ****************/

/*
*==========================================================================
**    Synopsis:    SHORT    OpReqMld(USHORT usFcCode, USHORT usLockHnd)
*
*       Input:    usLockHnd
*                 usFcCode
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*
**  Description:
*               Request MLD Mnmonics File Data (for broadcast) V3.3
*==========================================================================
*/
SHORT   OpReqMld(USHORT usFcCode, USHORT usLockHnd)
{
    OP_BACKUP       Op_Backup, *pOp_BackupRcv;
    USHORT          usRcvLen;
    UCHAR           auchRcvBuf[OP_BROADCAST_SIZE];
    OPMLD_FILEHED   MldFileHed;
    SHORT           sStatus;

    /*  LOCK CHECK */
    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        return(OP_LOCK);
    }

    /*  File Open */
    if ((Op_MldOpenFile()) == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        if (usFcCode == CLI_FCBAKOPPARA) {
            return(OP_PERFORM);
        }
        return(OP_FILE_NOT_FOUND);
    }

    /* Get File Header */
    Op_ReadMldFile(OP_FILE_HEAD_POSITION, &MldFileHed, sizeof(MldFileHed));

    Op_Backup.uchClass = OP_MLD_FILE;
    Op_Backup.offulFilePosition = 0L;
    Op_Backup.usSeqNo = 1;

    usRcvLen = OP_BROADCAST_SIZE;
    pOp_BackupRcv = (OP_BACKUP *)auchRcvBuf;

	NHPOS_NONASSERT_TEXT("OpReqMld(): Requesting MLD file.");

    for (;;) {
        usRcvLen = OP_BROADCAST_SIZE;
        if (usFcCode == CLI_FCBAKOPPARA) {
            if  ((sStatus = SstReqBackUp(CLI_FCBAKOPPARA, (UCHAR *)&Op_Backup, sizeof(OP_BACKUP), auchRcvBuf, &usRcvLen)) < 0 ) {
				sStatus += STUB_RETCODE;     // move the STUB_ type of error code into the OP_ error code range for use by UicDisplayErrorMsg()
                break;
            }
        } else {
            if  ((sStatus = CstReqOpePara(usFcCode, (UCHAR *)&Op_Backup, sizeof(OP_BACKUP), auchRcvBuf, &usRcvLen)) < 0 ) {
				sStatus += STUB_RETCODE;     // move the STUB_ type of error code into the OP_ error code range for use by UicDisplayErrorMsg()
                break;
            }
        }

        if (pOp_BackupRcv->usSeqNo != Op_Backup.usSeqNo) {
			char  xBuff[128];
			if (Op_Backup.usSeqNo == pOp_BackupRcv->usSeqNo + 1) {
				sprintf (xBuff, "OpReqMld(): OP_COMM_IGNORE usSentSeqNo 0x%x, usRecvSeqNo 0x%x", Op_Backup.usSeqNo, pOp_BackupRcv->usSeqNo);
				NHPOS_ASSERT_TEXT((pOp_BackupRcv->usSeqNo == Op_Backup.usSeqNo), xBuff);
				continue;
			}
			sprintf (xBuff, "OpReqMld(): OP_COMM_ERROR usSentSeqNo 0x%x, usRecvSeqNo 0x%x", Op_Backup.usSeqNo, pOp_BackupRcv->usSeqNo);
			NHPOS_ASSERT_TEXT((pOp_BackupRcv->usSeqNo == Op_Backup.usSeqNo), xBuff);
			PifLog(MODULE_OP_BACKUP, LOG_EVENT_OP_REQPARA_RQST_SEQNO);
			PifLog (MODULE_DATA_VALUE(MODULE_OP_BACKUP), OP_MLD_FILE);
			PifLog (MODULE_LINE_NO(MODULE_OP_BACKUP), (USHORT)__LINE__);
            sStatus =  OP_COMM_ERROR;
            break;
        }

        if (pOp_BackupRcv->usRecordNo > MldFileHed.usNumberOfAddress) {
            sStatus = OP_FILE_FULL;
            break;
        }

        Op_Backup.usSeqNo++;

        Op_WriteMldFile(pOp_BackupRcv->offulFilePosition, &auchRcvBuf[sizeof(OP_BACKUP)], (USHORT)(usRcvLen - sizeof(OP_BACKUP)));

        if (pOp_BackupRcv->uchStatus == OP_END) {        /* Last Message */
            break;
        }
        Op_Backup.offulFilePosition += usRcvLen - sizeof(OP_BACKUP);
    }
    PifCloseFile(hsOpMldFileHandle);
    return(sStatus);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpResMld(UCHAR   *puchRcvData,
*                                   USHORT  usRcvLen,
*                                   UCHAR   *puchSndData,
*                                   UCHAR   *pusSndLen)
*
*       Input:    *puchRcvData        * Pointer to Request Data *
*                 usRcvLen            * Width of puchRcvData *
*      Output:    *puchSndData        * Storage location for backup data. *
*       InOut:    *pusSndLen          * Width of puchSndData *
*
**  Return    :   OP_PERFORM
*                 OP_COMM_ERROR;
*
**  Description:  Response Mld Mnemonics File Data (for broadcast) V3.3
*==========================================================================
*/
SHORT   OpResMld(UCHAR  *puchRcvData,
                 USHORT  usRcvLen,
                 UCHAR  *puchSndData,
                 USHORT *pusSndLen,
                 USHORT  usLockHnd)
{
    OP_BACKUP      *pOp_BackUpRcv, *pOp_BackUpSnd;
    OPMLD_FILEHED  MldFileHed;
    USHORT         usMldFileSize;
	SHORT          sRetStatus;

    if (usRcvLen < sizeof(OP_BACKUP)) {
		PifLog (MODULE_OP_PLU, LOG_EVENT_OP_REQPARA_COMM_ERROR);
		PifLog (MODULE_LINE_NO(MODULE_OP_PLU), (USHORT)__LINE__);
		NHPOS_ASSERT_TEXT((usRcvLen >= sizeof(OP_BACKUP)), "OpResMld(): Bad Parameter.");
        return (OP_COMM_ERROR);
    }
    if (*pusSndLen < sizeof(OP_BACKUP)) {
		PifLog (MODULE_OP_PLU, LOG_EVENT_OP_REQPARA_COMM_ERROR);
		PifLog (MODULE_LINE_NO(MODULE_OP_PLU), (USHORT)__LINE__);
		NHPOS_ASSERT_TEXT((*pusSndLen >= sizeof(OP_BACKUP)), "OpResMld(): Bad Parameter.");
        return (OP_COMM_ERROR);
    }

    /*  Request Semaphore */
    PifRequestSem(husOpSem);

    /*  LOCK CHECK */
    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return(OP_LOCK);
    }

    /*  File Open */
    if ((Op_MldOpenFile()) == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        PifReleaseSem(husOpSem);
        return(OP_FILE_NOT_FOUND);
    }

    /* Get File Header */
    Op_ReadMldFile(OP_FILE_HEAD_POSITION, &MldFileHed, sizeof(MldFileHed));

    pOp_BackUpRcv = (OP_BACKUP *)puchRcvData;
    pOp_BackUpSnd = (OP_BACKUP *)puchSndData;

    /* Calculate Mld File size */
    usMldFileSize = (MldFileHed.usNumberOfAddress * sizeof(MLD_RECORD)) + sizeof(OPMLD_FILEHED);
    if ((USHORT)pOp_BackUpRcv->offulFilePosition < usMldFileSize) {
		if (OP_BROADCAST_SIZE < *pusSndLen) {
			*pusSndLen = OP_BROADCAST_SIZE;
		}
		if ((usMldFileSize - pOp_BackUpRcv->offulFilePosition) > (*pusSndLen - sizeof(OP_BACKUP))) {
			pOp_BackUpSnd->offulFilePosition = pOp_BackUpRcv->offulFilePosition;
			pOp_BackUpSnd->uchStatus = OP_CONTINUE;
		} else {
			*pusSndLen = (USHORT)(usMldFileSize - pOp_BackUpRcv->offulFilePosition) + sizeof(OP_BACKUP);
			pOp_BackUpSnd->uchStatus = OP_END;
			pOp_BackUpSnd->offulFilePosition = pOp_BackUpRcv->offulFilePosition;
		}

		pOp_BackUpSnd->usSeqNo   = pOp_BackUpRcv->usSeqNo;
		pOp_BackUpSnd->usRecordNo = MldFileHed.usNumberOfAddress;

		Op_ReadMldFile(pOp_BackUpRcv->offulFilePosition, puchSndData + sizeof(OP_BACKUP), (USHORT)(*pusSndLen - sizeof(OP_BACKUP)));
		sRetStatus = (OP_PERFORM);   
	} else {
		PifLog (MODULE_OP_PLU, LOG_EVENT_OP_REQPARA_COMM_ERROR);
		PifLog (MODULE_LINE_NO(MODULE_OP_PLU), (USHORT)__LINE__);
        sRetStatus = (OP_COMM_ERROR);
    }

    Op_CloseMldFileReleaseSem();
    return sRetStatus;   
}

SHORT   OpReqFileTransfer (USHORT usFcCode, TCHAR CONST *ptcsFileName, UCHAR uchClass, USHORT usLockHnd)
{
	ULONG           ulActualPosition;
    OP_BACKUP       Op_Backup, *pOp_BackupRcv;
    USHORT          usRcvLen;
    UCHAR           auchRcvBuf[OP_BROADCAST_SIZE];
    SHORT           sStatus;
	SHORT           hsBiometricsFile;

    /*  LOCK CHECK */
    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        return(OP_LOCK);
    }

    /*  File Open */
	hsBiometricsFile = PifOpenFile (ptcsFileName, "ow");
    if (hsBiometricsFile == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        if (usFcCode == CLI_FCBAKOPPARA) {
            return(OP_PERFORM);
        }
        return(OP_FILE_NOT_FOUND);
    }

    Op_Backup.uchClass = uchClass;
    Op_Backup.offulFilePosition = 0L;
    Op_Backup.usSeqNo = 1;

    usRcvLen = OP_BROADCAST_SIZE;
    pOp_BackupRcv = (OP_BACKUP *)auchRcvBuf;

    for (;;) {
        usRcvLen = OP_BROADCAST_SIZE;
        if (usFcCode == CLI_FCBAKOPPARA) {
            if  ((sStatus = SstReqBackUp(CLI_FCBAKOPPARA, (UCHAR *)&Op_Backup, sizeof(OP_BACKUP), auchRcvBuf, &usRcvLen)) < 0 ) {
				sStatus += STUB_RETCODE;     // move the STUB_ type of error code into the OP_ error code range for use by UicDisplayErrorMsg()
                break;
            }
        } else {
            if  ((sStatus = CstReqOpePara(usFcCode, (UCHAR *)&Op_Backup, sizeof(OP_BACKUP), auchRcvBuf, &usRcvLen)) < 0 ) {
				sStatus += STUB_RETCODE;     // move the STUB_ type of error code into the OP_ error code range for use by UicDisplayErrorMsg()
                break;
            }
        }

        if (pOp_BackupRcv->usSeqNo != Op_Backup.usSeqNo) {
			char  xBuff[128];
			if (Op_Backup.usSeqNo == pOp_BackupRcv->usSeqNo + 1) {
				sprintf (xBuff, "OpReqFileTransfer(): OP_COMM_IGNORE usSentSeqNo 0x%x, usRecvSeqNo 0x%x", Op_Backup.usSeqNo, pOp_BackupRcv->usSeqNo);
				NHPOS_ASSERT_TEXT((pOp_BackupRcv->usSeqNo == Op_Backup.usSeqNo), xBuff);
				continue;
			}
			sprintf (xBuff, "OpReqFileTransfer(): OP_COMM_ERROR usSentSeqNo 0x%x, usRecvSeqNo 0x%x", Op_Backup.usSeqNo, pOp_BackupRcv->usSeqNo);
			NHPOS_ASSERT_TEXT((pOp_BackupRcv->usSeqNo == Op_Backup.usSeqNo), xBuff);
			PifLog(MODULE_OP_BACKUP, LOG_EVENT_OP_REQPARA_RQST_SEQNO);
			PifLog (MODULE_DATA_VALUE(MODULE_OP_BACKUP), OP_BIOMETRICS_FILE);
			PifLog (MODULE_LINE_NO(MODULE_OP_BACKUP), (USHORT)__LINE__);
            sStatus =  OP_COMM_ERROR;
            break;
        }

        Op_Backup.usSeqNo++;

		PifSeekFile (hsBiometricsFile, pOp_BackupRcv->offulFilePosition,  &ulActualPosition);
		PifWriteFile (hsBiometricsFile, &auchRcvBuf[sizeof(OP_BACKUP)], (USHORT)(usRcvLen - sizeof(OP_BACKUP)));

        if (pOp_BackupRcv->uchStatus == OP_END) {        /* Last Message */
            break;
        }
        Op_Backup.offulFilePosition += usRcvLen - sizeof(OP_BACKUP);
    }
    PifCloseFile(hsBiometricsFile);
    return(sStatus);
}

SHORT   OpReqBiometrics(USHORT usFcCode, USHORT usLockHnd)
{
    /*  LOCK CHECK */
    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        return(OP_LOCK);
    }

	return OpReqFileTransfer (usFcCode, DFPR_FNAME, OP_BIOMETRICS_FILE, usLockHnd);
}

SHORT   OpReqReasonCodes(USHORT usFcCode, USHORT usLockHnd)
{
    /*  LOCK CHECK */
    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        return(OP_LOCK);
    }

	return OpReqFileTransfer (usFcCode, auchOP_RSN, OP_REASONCODES_FILE, usLockHnd);
}

static SHORT   OpResFileTransfer (UCHAR  *puchRcvData,
                 USHORT  usRcvLen,
                 UCHAR  *puchSndData,
                 USHORT *pusSndLen,
				 CONST TCHAR *ptcsFileName,
                 USHORT  usLockHnd)
{
	ULONG          ulActualPosition, ulBytesRead;
	ULONG          ulFileSize, ulFileSizeHigh;
    OP_BACKUP      *pOp_BackUpRcv, *pOp_BackUpSnd;
	SHORT          hsBiometricsFile;

    if (usRcvLen < sizeof(OP_BACKUP)) {
		PifLog (MODULE_OP_PLU, LOG_EVENT_OP_REQPARA_COMM_ERROR);
		PifLog (MODULE_LINE_NO(MODULE_OP_PLU), (USHORT)__LINE__);
		NHPOS_ASSERT_TEXT((usRcvLen >= sizeof(OP_BACKUP)), "OpResBiometrics(): Bad Parameter.");
        return (OP_COMM_ERROR);
    }
    if (*pusSndLen < sizeof(OP_BACKUP)) {
		PifLog (MODULE_OP_PLU, LOG_EVENT_OP_REQPARA_COMM_ERROR);
		PifLog (MODULE_LINE_NO(MODULE_OP_PLU), (USHORT)__LINE__);
		NHPOS_ASSERT_TEXT((*pusSndLen >= sizeof(OP_BACKUP)), "OpResBiometrics(): Bad Parameter.");
        return (OP_COMM_ERROR);
    }

    /*  Request Semaphore */
    PifRequestSem(husOpSem);

    /*  LOCK CHECK */
    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return(OP_LOCK);
    }

    pOp_BackUpRcv = (OP_BACKUP *)puchRcvData;
    pOp_BackUpSnd = (OP_BACKUP *)puchSndData;

    if (OP_BROADCAST_SIZE < *pusSndLen) {
        *pusSndLen = OP_BROADCAST_SIZE;
    }

	hsBiometricsFile = PifOpenFile (ptcsFileName, "or");
	if (hsBiometricsFile < 0) {
		pOp_BackUpSnd->usSeqNo   = pOp_BackUpRcv->usSeqNo;
		pOp_BackUpSnd->usRecordNo = 0;
		pOp_BackUpSnd->uchStatus = OP_END;    // indicate end of file
        PifReleaseSem(husOpSem);
        return(OP_FILE_NOT_FOUND);
    }

	// see also the functionality of OpReqFileTransfer() which is what processes a
	// file transfer by repeatedly sending OP_BACKUP type messages.
	ulFileSize = PifGetFileSize(hsBiometricsFile, &ulFileSizeHigh);
    if (pOp_BackUpRcv->offulFilePosition < ulFileSize) {
		if ((ulFileSize - pOp_BackUpRcv->offulFilePosition) > (*pusSndLen - sizeof(OP_BACKUP))) {
			pOp_BackUpSnd->offulFilePosition = pOp_BackUpRcv->offulFilePosition;
			pOp_BackUpSnd->uchStatus = OP_CONTINUE;
		} else {
			*pusSndLen = (USHORT)(ulFileSize - pOp_BackUpRcv->offulFilePosition) + sizeof(OP_BACKUP);
			pOp_BackUpSnd->uchStatus = OP_END;
			pOp_BackUpSnd->offulFilePosition = pOp_BackUpRcv->offulFilePosition;
		}

		pOp_BackUpSnd->usSeqNo   = pOp_BackUpRcv->usSeqNo;
		pOp_BackUpSnd->usRecordNo = (pOp_BackUpSnd->offulFilePosition & sizeof(OP_BACKUP)) + 1;  // ignored in OpReqFileTransfer().

		PifSeekFile (hsBiometricsFile, pOp_BackUpRcv->offulFilePosition,  &ulActualPosition);
		PifReadFile (hsBiometricsFile, puchSndData + sizeof(OP_BACKUP), (USHORT)(*pusSndLen - sizeof(OP_BACKUP)), &ulBytesRead);
	} else {
		PifLog (MODULE_OP_PLU, LOG_EVENT_OP_REQPARA_COMM_ERROR);
		PifLog (MODULE_LINE_NO(MODULE_OP_PLU), (USHORT)__LINE__);
    }

    PifCloseFile (hsBiometricsFile);
    PifReleaseSem(husOpSem);
    return (OP_PERFORM);   
}


SHORT   OpResBiometrics (UCHAR  *puchRcvData,
                 USHORT  usRcvLen,
                 UCHAR  *puchSndData,
                 USHORT *pusSndLen,
                 USHORT  usLockHnd)
{
	return OpResFileTransfer (puchRcvData, usRcvLen, puchSndData, pusSndLen, DFPR_FNAME, usLockHnd);
}

SHORT   OpResReasonCodes (UCHAR  *puchRcvData,
                 USHORT  usRcvLen,
                 UCHAR  *puchSndData,
                 USHORT *pusSndLen,
                 USHORT  usLockHnd)
{
	return OpResFileTransfer (puchRcvData, usRcvLen, puchSndData, pusSndLen, auchOP_RSN, usLockHnd);
}


/*                 
*==========================================================================
**    Synopsis:   SHORT  Op_MldOpenFile(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description:  Open "PARAMLD" file. V3.3
*==========================================================================
*/
SHORT  Op_MldOpenFile(VOID)
{
    hsOpMldFileHandle = PifOpenFile(auchOP_MLD, auchOLD_FILE_READ_WRITE);
    return (hsOpMldFileHandle);
}

/*
*==========================================================================
**    Synopsis:    VOID    Op_CloseMldFileReleaseSem(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing 
*
**  Description:
*               Close MLD Mnemonics file and Release semaphore V3.3
*==========================================================================
*/
VOID    Op_CloseMldFileReleaseSem(VOID)
{
    PifCloseFile(hsOpMldFileHandle);
    PifReleaseSem(husOpSem);
}

/*
*==========================================================================
**    Synopsis:  SHORT Op_ReadMldFile(ULONG  offulFileSeek,
*                                 VOID   *pTableHeadAddress,
*                                 ULONG ulSizeofRead)
*
*       Input:  offulFileSeek           Number of bytes from the biginning of
*                                       the Mld Mnemonics file.
*               ulSizeofRead            Number of bytes to be read.
*      Output:  *pTableHeadAddress      Storage location of items to be read.
*       InOut:  Nothing
*
**  Return    : OP_PERFORM
*               OP_NO_READ_SIZE                  Read size is 0
*               (File error = system halt)
*
**  Description: Read data in Mld Mnemonics file. V3.3
*==========================================================================
*/
SHORT   Op_ReadMldFile(ULONG offulFileSeek,
                       VOID  *pTableHeadAddress,
                       ULONG ulSizeofRead)
{
    ULONG   ulActualPosition;
	ULONG	ulActualBytesRead;	//RPH 11-10-3

	memset (pTableHeadAddress, 0, ulSizeofRead);

    if (ulSizeofRead == 0X00) {
        return (OP_NO_READ_SIZE);
    }
    
    if ((PifSeekFile(hsOpMldFileHandle, offulFileSeek, &ulActualPosition)) < 0) {
        PifAbort(MODULE_OP_MLD, FAULT_ERROR_FILE_SEEK);
    }

	//RPH 11-10-3 Changes For PifReadFile
	PifReadFile(hsOpMldFileHandle, pTableHeadAddress, ulSizeofRead, &ulActualBytesRead);
    if ( ulActualBytesRead != ulSizeofRead) {
//        PifAbort(MODULE_OP_MLD, FAULT_ERROR_FILE_READ);
    }
    return(OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   Op_WriteMldFile(ULONG  offulFileSeek,
*                                          VOID   *pTableHeadAddress, 
*                                          USHORT usSizeofWrite)
*
*       Input:   offulFileSeek           Number of bytes from the biginning of the 
*                                        Mld Mnemonics file.
*                *pTableHeadAddress      Pointer to data to be written.
*                usSizeofWrite           Number of bytes to be written.
*      Output:   Nothing
*       InOut:   Nothing
*
**  Return    :  Nothing (error is system error (progrum is abort)
*
**  Description: Write data in Mld Mnemonics File. V3.3
*==========================================================================
*/
VOID   Op_WriteMldFile(ULONG offulFileSeek,
                       VOID  *pTableHeadAddress,
                       USHORT usSizeofWrite)
{
    ULONG   ulActualPosition;

    if (!usSizeofWrite) {
        return;
    }
    if ( PifSeekFile(hsOpMldFileHandle, offulFileSeek, &ulActualPosition) < 0) {
        PifAbort(MODULE_OP_MLD, FAULT_ERROR_FILE_SEEK);
    }
    PifWriteFile(hsOpMldFileHandle, pTableHeadAddress, usSizeofWrite);
}


/* Hosts address boradcast, V1.0.04 */
/*
*===========================================================================
**    Synopsis:    SHORT   OpReqHostsIP(USHORT usFcCode, USHORT usLockHnd)
*
*       Input:    usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*
**  Description: Request Hosts Paramater . (for broadcast)
*==========================================================================
*/
SHORT   OpReqHostsIP(USHORT usFcCode, USHORT usLockHnd)
{
	OP_BACKUP   Op_Backup = { 0 };
	UCHAR       auchRcvBuf[OP_BROADCAST_SIZE] = { 0 };
	SHORT       sStatus = OP_PERFORM;
	SHORT       len, i;
	TCHAR       uchHostName[PIF_LEN_HOST_NAME + 1] = { 0 };
    
    /*  LOCK CHECK */
    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        return(OP_LOCK);
    }

    PifGetLocalHostName(uchHostName);

    len = tcharlen(uchHostName);
    for (i = 0; i < len; i++) {
        if (uchHostName[i] == '-') {
            uchHostName[i+1] = '\0';
            break;
        }
    }
    if (i >= len) {
        return(OP_PERFORM);
    }

    Op_Backup.uchClass = OP_HOSTS_FILE;
    Op_Backup.offulFilePosition = 0L;
    Op_Backup.usSeqNo = 1;
    for(;;) {
		OP_BACKUP   * const pOp_BackupRcv = (OP_BACKUP *)auchRcvBuf;
		HOSTSIP     * const pOpHostsIP = (HOSTSIP *)&auchRcvBuf[sizeof(OP_BACKUP)];
		USHORT      usRcvLen = OP_BROADCAST_SIZE;

        if (usFcCode == CLI_FCBAKOPPARA) {
            if  ((sStatus = SstReqBackUp(CLI_FCBAKOPPARA, (UCHAR *)&Op_Backup, sizeof(OP_BACKUP), auchRcvBuf, &usRcvLen)) < 0 ) {
				sStatus += STUB_RETCODE;     // move the STUB_ type of error code into the OP_ error code range for use by UicDisplayErrorMsg()
                break;
            }
        } else {
            if  ((sStatus = CstReqOpePara(usFcCode, (UCHAR *)&Op_Backup, sizeof(OP_BACKUP), auchRcvBuf, &usRcvLen)) < 0 ) {
				sStatus += STUB_RETCODE;     // move the STUB_ type of error code into the OP_ error code range for use by UicDisplayErrorMsg()
                break;
            }
        }

		usRcvLen   -= sizeof(OP_BACKUP);

        if (pOp_BackupRcv->usSeqNo != Op_Backup.usSeqNo) {
			char  xBuff[128];
			if (Op_Backup.usSeqNo == pOp_BackupRcv->usSeqNo + 1) {
				sprintf (xBuff, "OpReqHostsIP(): OP_COMM_IGNORE usSentSeqNo 0x%x, usRecvSeqNo 0x%x", Op_Backup.usSeqNo, pOp_BackupRcv->usSeqNo);
				NHPOS_ASSERT_TEXT((pOp_BackupRcv->usSeqNo == Op_Backup.usSeqNo), xBuff);
				continue;
			}
			sprintf (xBuff, "OpReqHostsIP(): OP_COMM_ERROR usSentSeqNo 0x%x, usRecvSeqNo 0x%x", Op_Backup.usSeqNo, pOp_BackupRcv->usSeqNo);
			NHPOS_ASSERT_TEXT((pOp_BackupRcv->usSeqNo == Op_Backup.usSeqNo), xBuff);
			PifLog(MODULE_OP_BACKUP, LOG_EVENT_OP_REQPARA_RQST_SEQNO);
			PifLog (MODULE_DATA_VALUE(MODULE_OP_BACKUP), OP_HOSTS_FILE);
			PifLog (MODULE_LINE_NO(MODULE_OP_BACKUP), (USHORT)__LINE__);
            return(OP_COMM_ERROR);
        }

        for (i = 0; i < OP_MAX_BROADCAST_IP; i++) {
			TCHAR       uchHosts[PIF_LEN_HOST_NAME + 1] = { 0 };
			UCHAR       uchIPAddress[PIF_LEN_IP + 1];

            if ((pOpHostsIP->aHostNameIP[i].auchIPAddress[0]) ||
                (pOpHostsIP->aHostNameIP[i].auchIPAddress[1]) ||
                (pOpHostsIP->aHostNameIP[i].auchIPAddress[2]) ||
                (pOpHostsIP->aHostNameIP[i].auchIPAddress[3])) {

                _tcsncpy(uchHosts, pOpHostsIP->aHostNameIP[i].auchHostName, PIF_LEN_HOST_NAME);
                memcpy(uchIPAddress, pOpHostsIP->aHostNameIP[i].auchIPAddress, PIF_LEN_IP);
                PifSetHostsAddress(uchHosts, uchIPAddress);
            } else {
            	/* clear hosts data */
		    	RflSPrintf(uchHosts, TCHARSIZEOF(uchHosts), _T("%s%d"), &uchHostName[0], i+1);
	        	PifRemoveHostsAddress(uchHosts);
			}
        }   

        Op_Backup.uchClass = pOp_BackupRcv->uchClass;
        Op_Backup.offulFilePosition = pOp_BackupRcv->offulFilePosition;
        Op_Backup.usSeqNo++;
        if (pOp_BackupRcv->uchStatus == OP_END) {        /* Last Message */
            break;
        }
    }
    return(sStatus);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpResHostsIP(UCHAR   *puchRcvData,
*                                   USHORT  usRcvLen,
*                                   UCHAR   *puchSndData,
*                                   UCHAR   *pusSndLen)
*
*       Input:    *puchRcvData        * Pointer to Request Data *
*                 usRcvLen            * Width of puchRcvData *
*      Output:    *puchSndData        * Storage location for backup data. *
*       InOut:    *pusSndLen          * Width of puchSndData *
*
**  Return    :   OP_PERFORM
*                 OP_COMM_ERROR;
*
**  Description: Response hosts paramater. (for broadcast)
*==========================================================================
*/
SHORT   OpResHostsIP(UCHAR  *puchRcvData,
                 USHORT  usRcvLen,
                 UCHAR  *puchSndData,
                 USHORT *pusSndLen,
                 USHORT  usLockHnd)
{
	OP_BACKUP   * const pOp_BackUpRcv = (OP_BACKUP *)puchRcvData;
	OP_BACKUP   * const pOp_BackUpSnd = (OP_BACKUP *)puchSndData;
    HOSTSIP     * const pOpHostsIP = (HOSTSIP*)puchSndData;
	TCHAR       uchHostName[PIF_LEN_HOST_NAME + 1] = { 0 };
	USHORT      usSndLenWork, usSndLenBack;
    USHORT      len, i, usNo;
    SHORT       sRet;

    PifRequestSem(husOpSem);

    /*  LOCK CHECK */
    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
		PifLog (MODULE_OP_PLU, LOG_EVENT_OP_REQFILE_LOCKCHECK);
		PifLog (MODULE_LINE_NO(MODULE_OP_PLU), (USHORT)__LINE__);
        return(OP_LOCK);
    }
    if (usRcvLen < sizeof(OP_BACKUP)) {
        PifReleaseSem(husOpSem);
		PifLog (MODULE_OP_PLU, LOG_EVENT_OP_REQPARA_COMM_ERROR);
		PifLog (MODULE_LINE_NO(MODULE_OP_PLU), (USHORT)__LINE__);
        return (OP_COMM_ERROR);
    }
    if (*pusSndLen < sizeof(OP_BACKUP)) {
        PifReleaseSem(husOpSem);
		PifLog (MODULE_OP_PLU, LOG_EVENT_OP_REQPARA_COMM_ERROR);
		PifLog (MODULE_LINE_NO(MODULE_OP_PLU), (USHORT)__LINE__);
        return (OP_COMM_ERROR);
    }

	if (OP_BROADCAST_SIZE > *pusSndLen) {
        usSndLenBack = *pusSndLen;
    } else {
        usSndLenBack = OP_BROADCAST_SIZE;
    }
    usSndLenWork = usSndLenBack - sizeof(OP_BACKUP);
    puchSndData += sizeof(OP_BACKUP);
    pOp_BackUpSnd->uchClass = pOp_BackUpRcv->uchClass;
    pOp_BackUpSnd->offulFilePosition = pOp_BackUpRcv->offulFilePosition; 
    pOp_BackUpSnd->usSeqNo = pOp_BackUpRcv->usSeqNo;
    
    PifGetLocalHostName(uchHostName);

    len = tcharlen(uchHostName);
    for (i = 0; i < len; i++) {
        if (uchHostName[i] == _T('-')) {
            uchHostName[i+1] = _T('\0');
            break;
        }
    }
    if (i >= len) {
        return (OP_PERFORM);   
    }
        
    for (i = 0; i < OP_MAX_BROADCAST_IP; i++) {
		TCHAR       uchHosts[PIF_LEN_HOST_NAME + 1] = { 0 };
		UCHAR       uchIPAddress[PIF_LEN_IP + 1] = { 0 };

        usNo = (USHORT)((i + 1) + (pOp_BackUpRcv->usSeqNo - 1) * OP_MAX_BROADCAST_IP);

        RflSPrintf(uchHosts, TCHARSIZEOF(uchHosts), _T("%s%d"), &uchHostName[0], usNo);
        sRet = PifGetHostsAddress(uchHosts, uchIPAddress);
        
        if (sRet == PIF_OK) {
            _tcsncpy(pOpHostsIP->aHostNameIP[i].auchHostName, uchHosts, PIF_LEN_HOST_NAME);
            memcpy(pOpHostsIP->aHostNameIP[i].auchIPAddress, uchIPAddress, PIF_LEN_IP);
        } else {
			memset(&pOpHostsIP->aHostNameIP[i], 0, sizeof(HOSTNAMEIP));
        }

        usSndLenWork -= sizeof(HOSTNAMEIP);
        
    }
    if (usNo >= PIF_NET_MAX_IP) {
        pOp_BackUpSnd->uchStatus = OP_END;
    }
    
    *pusSndLen = usSndLenBack - usSndLenWork;
    PifReleaseSem(husOpSem);
    return (OP_PERFORM);   
}


SHORT   OpDisplayOperatorMessage(USHORT  *pusMessageBufferEnum)
{
	return CstOpRetrieveOperatorMessage(pusMessageBufferEnum);
}


/*
*==========================================================================
**    Synopsis:    SHORT    OpReqFile(USHORT usFcCode, USHORT usLockHnd)
*
*       Input:    usLockHnd
*                 usFcCode
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*
**  Description:
*               Request Keyboard Layout File Data (for broadcast)
*==========================================================================
*/ //ESMITH LAYOUT
SHORT   OpReqFile(TCHAR *pszFileName, USHORT usLockHnd)
{
    LONG    lRmtRead;               /* read length */
	ULONG	ulFilePos=0;
	SHORT	sStatus, sLocalFile;
    UCHAR   auchRcvBuf[OP_BROADCAST_SIZE];


    /*  LOCK CHECK */
    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
		PifLog (MODULE_OP_PLU, LOG_EVENT_OP_REQFILE_LOCKCHECK);
        return(OP_LOCK);
    }
	
	((CLIREQREAD*)auchRcvBuf)->ulPosition = ulFilePos;

	for (;;) { 
		/*  File Open */
		if((sLocalFile = PifOpenFile(pszFileName,"ow")) < 0) {
			// We were unable to open the local file, which we assume already exists
			// since it should have been created at terminal application startup if it
			// did not exist previous. Lets log the error and then return with the
			// error indicator.
			sStatus = (OP_FILE_NOT_FOUND);
			PifLog (MODULE_OP_PLU, LOG_EVENT_OP_REQFILE_PIFOPENFAIL);
			PifLog (MODULE_ERROR_NO(MODULE_OP_PLU), (USHORT)abs(sLocalFile));
			{
				CHAR  xBuff[128];
				sprintf (xBuff, "PifOpenFile(old) error %d, Filename %S", sLocalFile, pszFileName);
				NHPOS_ASSERT_TEXT((sLocalFile >= 0),xBuff);
			}
			break;
		}

		for(;;) {
			lRmtRead = CstReqReadFile(pszFileName, "or", auchRcvBuf, OP_BROADCAST_SIZE, ulFilePos);

			if(lRmtRead > 0) {
				PifWriteFile ((USHORT)sLocalFile, auchRcvBuf, lRmtRead);
				ulFilePos += lRmtRead;

				// If we don't have a full buffer then we know that
				// we have reached the end of the remote file since
				// the CstReqReadFile () function returns as much as
				// it is told to return.
				if (lRmtRead != OP_BROADCAST_SIZE) {
					sStatus = OP_PERFORM;
					break;
				} 
			} else if (lRmtRead == 0) {
				sStatus = OP_PERFORM;
				break;
			} else {
				{
					CHAR  xBuff[128];
					sprintf (xBuff, "CstReqReadFile() error %d, Filename %S", lRmtRead, pszFileName);
					NHPOS_ASSERT_TEXT((lRmtRead >= 0),xBuff);
				}
				sStatus = OP_FILE_NOT_FOUND;
				PifLog (MODULE_OP_PLU, LOG_EVENT_OP_REQFILE_CSTREQREADFILE);
				PifLog (MODULE_ERROR_NO(MODULE_OP_PLU), (USHORT)abs(lRmtRead));
				break;
			}
		}
		
		PifCloseFile(sLocalFile);
		break;
	}

	return (sStatus);
}


/*
*==========================================================================
**    Synopsis:    SHORT    OpTransferFile(USHORT usFcCode, USHORT usLockHnd)
*
*       Input:    usLockHnd
*                 usFcCode
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*
**  Description:
*               Request Keyboard Layout File Data (for broadcast)
*==========================================================================
*/ //ESMITH LAYOUT
SHORT   OpTransferFile(USHORT usTerminalPosition, TCHAR *pszLocalFileName, TCHAR *pszRemoteFileName, USHORT usLockHnd)
{
    LONG    lRmtRead;               /* read length */
	ULONG	ulFilePos=0;
	SHORT	sStatus, sLocalFile;
    UCHAR   auchRcvBuf[OP_BROADCAST_SIZE];

	{
		CHAR  xBuff[128];

		sprintf (xBuff, "OpTransferFile(): Terminal %d, File %S", usTerminalPosition, pszLocalFileName);
		NHPOS_ASSERT_TEXT(0, xBuff);
	}

    /*  LOCK CHECK */
    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
		PifLog (MODULE_OP_PLU, LOG_EVENT_OP_REQFILE_LOCKCHECK);
        return(OP_LOCK);
    }
	
	((CLIREQREAD*)auchRcvBuf)->ulPosition = ulFilePos;

	for (;;) { 
		/*  File Open */
		if((sLocalFile = PifOpenFile(pszLocalFileName,"nw")) < 0) {
			sStatus = (OP_FILE_NOT_FOUND);
			PifLog (MODULE_OP_PLU, LOG_EVENT_OP_REQFILE_PIFOPENFAIL);
			PifLog (MODULE_ERROR_NO(MODULE_OP_PLU), (USHORT)abs(sLocalFile));
			PifLog (MODULE_LINE_NO(MODULE_OP_PLU), (USHORT)abs(__LINE__));
			{
				CHAR  xBuff[128];
				sprintf (xBuff, "OpTransferFile(): 1 PifOpenFile() error %d, File %S", sLocalFile, pszLocalFileName);
				NHPOS_ASSERT_TEXT((sLocalFile >= 0),xBuff);
			}

			if((sLocalFile = PifOpenFile(pszLocalFileName,"ow")) < 0) {
				sStatus = (OP_FILE_NOT_FOUND);
				PifLog (MODULE_OP_PLU, LOG_EVENT_OP_REQFILE_PIFOPENFAIL);
				PifLog (MODULE_ERROR_NO(MODULE_OP_PLU), (USHORT)abs(sLocalFile));
				PifLog (MODULE_LINE_NO(MODULE_OP_PLU), (USHORT)abs(__LINE__));
				{
					CHAR  xBuff[128];
					sprintf (xBuff, "OpTransferFile(): 2 PifOpenFile() error %d, File %S", sLocalFile, pszLocalFileName);
					NHPOS_ASSERT_TEXT((sLocalFile >= 0),xBuff);
				}
				break;
			}
		}

		for(;;) {
			lRmtRead = CstReqTransferFile(usTerminalPosition, pszRemoteFileName, "or", auchRcvBuf, OP_BROADCAST_SIZE, ulFilePos);

			if(lRmtRead > 0) {
				PifWriteFile ((USHORT)sLocalFile, auchRcvBuf, lRmtRead);
				ulFilePos += lRmtRead;

				// If we don't have a full buffer then we know that
				// we have reached the end of the remote file since
				// the CstReqReadFile () function returns as much as
				// it is told to return.
				if (lRmtRead != OP_BROADCAST_SIZE) {
					sStatus = OP_PERFORM;
					break;
				} 
			} else if (lRmtRead == 0) {
				sStatus = OP_PERFORM;
				break;
			} else {
				{
					CHAR  xBuff[128];
					sprintf (xBuff, "OpTransferFile(): CstReqTransferFile() error %d, File %S", lRmtRead, pszLocalFileName);
					NHPOS_ASSERT_TEXT((lRmtRead >= 0),xBuff);
				}
				sStatus = OP_FILE_NOT_FOUND;
				PifLog (MODULE_OP_PLU, LOG_EVENT_OP_REQFILE_CSTREQREADFILE);
				PifLog (MODULE_ERROR_NO(MODULE_OP_PLU), (USHORT)abs(lRmtRead));
				break;
			}
		}
		
		PifCloseFile(sLocalFile);
		break;
	}

	return (sStatus);
}

/*====== End of Source ========*/
