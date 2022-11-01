/*
* ---------------------------------------------------------------------------
* Title         :   Convert Programmable Report
* Category      :   Transfer, NCR 2170 PEP for Windows (HP US Model)
* Program Name  :   TRANCONV.C
* Copyright (C) :   1996, NCR Corp. E&M-OISO, All rights reserved.
* --------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /c /AL /W4 /G2s /Zp
* --------------------------------------------------------------------------
* Abstract:     As for this file, the function, which converts the
*               programmable report file.
*
* --------------------------------------------------------------------------
* Update Histories
*
* Date          Ver.       Name             Description
* Feb-08-96 : 03.00.05 : M.Suzuki   : Initial (HP US R3.1)
* Oct-15-98 : 03.03.00 : A.Mitsui   : Checked Old Version
* Jan-29-03 :          : R. Herrington : Fixed alignment 16->32bit conversion
*
*===========================================================================
*===========================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===========================================================================
*/
/*
*===========================================================================
*   Include File Declarations
*===========================================================================
*/
#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <ecr.h>
#include <paraequ.h>
#include <para.h>
#include <r20_pif.h>
#include <progrept.h>
#include <csprgrpt.h>

#include "trans.h"
#include "pepcomm.h"
#include "report.h"
#include "transl.h"

/*                                                                                
*===========================================================================
*   Compile Message Declarations
*===========================================================================
*/
/*                                                                                
*===========================================================================
*   Work Area Declarations
*===========================================================================
*/
/*
*===========================================================================
*   Public Work Area
*===========================================================================
*/
/*
*===========================================================================
*   Static Work Area 
*===========================================================================
*/
static WCHAR szLoopEntry[] = WIDE("%hd,%hd,%hd,%hd,%hd,%hd");
static WCHAR szPrintEntry[] = WIDE("%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd");
static WCHAR szMathOpeEntry[] = WIDE("%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd");
static WCHAR szMathTtlEntry[] = WIDE("%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%ld,%hd,%hd,%hd,%hd");
static WCHAR szMathCoEntry[] = WIDE("%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd");
static WCHAR szMathTmpEntry[] = WIDE("%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd");
/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/

/*
*===========================================================================
** Synopsis:    short TransConvertToIniFile(HWND hWnd)
*
*     Input:    hWnd       - handle of parent window
*
** Return:      0 >= successful
*               0 <  unsuccessful
*
** Description: This function converts the binary programmable report file
*               to ini file.
*===========================================================================
*/
short TransConvertToIniFile(HWND hWnd)
{
    SHORT   hsFile;            /* handle of local file */
    PROGRPT_FHEAD   RptHeader; /* programmable report header */
    SHORT   i;
    WCHAR   szBuffer[TRANS_CONVERT_WORK_SIZE];
//    OFSTRUCT        ofs;

    /* ----- open programmable report file ----- */
    if ((hsFile = PifOpenFile(szTransProgRpt, "or")) < 0) {
        return FALSE;
    }

    /* ----- check ini file name ----- */
    if (szCommRptSaveName[0] == '\0') {     /* no file name         */
        if (ReportFile((HANDLE)GetWindowLong(hWnd, GWL_HINSTANCE),
                       hWnd) == FALSE) {    /* call Report file function */
            PifCloseFile(hsFile);
            return FALSE;
        }
    }

     /* ----- delete ini. file ----- */
    DeleteFilePep(szCommRptSaveName);
	//OpenFile((LPCSTR)szCommRptSaveName, (OFSTRUCT FAR *)&ofs, OF_DELETE);

    /* ----- read programmable report file header ----- */
    if (PifReadFile((USHORT)hsFile, &RptHeader, sizeof(PROGRPT_FHEAD))
            != sizeof(PROGRPT_FHEAD)) {
        PifCloseFile(hsFile);
        return FALSE;
    }

    /* ----- convert file version ----- */
    wsPepf(szBuffer, WIDE("%x"), RptHeader.usVersion);

    WritePrivateProfileStringPep(WIDE("Header"),
                              WIDE("Version"),
                              szBuffer,
                              szCommRptSaveName);

    /* ----- version check , V3.3 ----- */
    


    /* ----- conver EOD  -----*/
    if (TransConvertEODToIniFile((USHORT)hsFile, RptHeader.ulOffsetEOD)
            == FALSE) {
        PifCloseFile(hsFile);
        return FALSE;
    }

    /* ----- conver PTD  -----*/
    if (TransConvertPTDToIniFile((USHORT)hsFile, RptHeader.ulOffsetPTD)
            == FALSE) {
        PifCloseFile(hsFile);
        return FALSE;
    }

    /* ----- conver 16 characters mnemonics  -----*/
    if (TransConvert16CharToIniFile((USHORT)hsFile,
                        RptHeader.ulOffset16Char) == FALSE) {
        PifCloseFile(hsFile);
        return FALSE;
    }

    /* ----- conver 40 characters mnemonics  -----*/
    if (TransConvert40CharToIniFile((USHORT)hsFile,
                        RptHeader.ulOffset40Char) == FALSE) {
        PifCloseFile(hsFile);
        return FALSE;
    }

    /* ----- conver report data  -----*/
    for (i=0; i < MAX_PROGRPT_FORMAT; i++) {
        if (TransConvertRptToIniFile((USHORT)hsFile,
                    RptHeader.aulOffsetReport[i], i) == FALSE) {
            PifCloseFile(hsFile);
            return FALSE;
        }
    }

    PifCloseFile(hsFile);
    return TRUE;
}

/*
*===========================================================================
** Synopsis:    short TransConvertEODToIniFile(USHORT husFile, ULONG ulOffset)
*
*     Input:    husFile   - handle of file
*               ulOffset  - offset of file
*
** Return:      0 >= successful
*               0 <  unsuccessful
*
** Description: This function converts EOD element to ini file.
*===========================================================================
*/
short TransConvertEODToIniFile(USHORT husFile, ULONG ulOffset)
{
    PROGRPT_EODPTD  EodPtd; 
    ULONG   ulActualPosition;
    USHORT  usCount;
    WCHAR   szBuffer[TRANS_CONVERT_WORK_SIZE];
    WCHAR   szItem[TRANS_CONVERT_WORK_SIZE];

        /* ----- check file offset ----- */
    if ( ulOffset == 0L ) {
        return TRUE;
    }

    /* ----- seek file ----- */
    if ((PifSeekFile(husFile, ulOffset, &ulActualPosition)) < 0) {
        return FALSE;
    }

    /* ----- read EOD ----- */
    if (PifReadFile((USHORT)husFile, &EodPtd, sizeof(PROGRPT_EODPTD))
            != sizeof(PROGRPT_EODPTD)) {
        return FALSE;
    }

    /* ----- set status ----- */
    wsPepf(szBuffer, WIDE("%d"), EodPtd.usUseProgRpt);

    WritePrivateProfileStringPep(WIDE("End of Day"),
                              WIDE("State"),
                              szBuffer,
                              szCommRptSaveName);

    /* ----- convert item ----- */
    for (usCount = 0; usCount < EodPtd.usNoOfRecord; usCount++) {

        wsPepf(szItem, WIDE("Address%d"), usCount+1);

        wsPepf(szBuffer, WIDE("%d,%d"),
                  EodPtd.aEODPTD[usCount].usFileType,
                  (SHORT)EodPtd.aEODPTD[usCount].usReportNo);

        WritePrivateProfileStringPep(WIDE("End of Day"),
                                  szItem,
                                  szBuffer,
                                  szCommRptSaveName);
    }

    return TRUE;
}

/*
*===========================================================================
** Synopsis:    short TransConvertPTDToIniFile(USHORT husFile, ULONG ulOffset)
*
*     Input:    husFile   - handle of file
*               ulOffset  - offset of file
*
** Return:      0 >= successful
*               0 <  unsuccessful
*
** Description: This function converts PTD element to ini file.
*===========================================================================
*/
short TransConvertPTDToIniFile(USHORT husFile, ULONG ulOffset)
{
    PROGRPT_EODPTD  EodPtd; 
    ULONG   ulActualPosition;
    USHORT  usCount;
    WCHAR   szBuffer[TRANS_CONVERT_WORK_SIZE];
    WCHAR   szItem[TRANS_CONVERT_WORK_SIZE];

    /* ----- check file offset ----- */
    if ( ulOffset == 0L ) {
        return TRUE;
    }

    /* ----- seek file ----- */
    if ((PifSeekFile(husFile, ulOffset, &ulActualPosition)) < 0) {
        return FALSE;
    }

    /* ----- read PTD ----- */
    if (PifReadFile((USHORT)husFile, &EodPtd, sizeof(PROGRPT_EODPTD))
            != sizeof(PROGRPT_EODPTD)) {
        return FALSE;
    }

    /* ----- set status ----- */
    wsPepf(szBuffer, WIDE("%d"), EodPtd.usUseProgRpt);

    WritePrivateProfileStringPep(WIDE("Period to Day"),
                              WIDE("State"),
                              szBuffer,
                              szCommRptSaveName);

    /* ----- convert item ----- */
    for (usCount = 0; usCount < EodPtd.usNoOfRecord; usCount++) {

        wsPepf(szItem, WIDE("Address%d"), usCount+1);

        wsPepf(szBuffer, WIDE("%d,%d"),
                  EodPtd.aEODPTD[usCount].usFileType,
                  (SHORT)EodPtd.aEODPTD[usCount].usReportNo);

        WritePrivateProfileStringPep(WIDE("Period to Day"),
                                  szItem,
                                  szBuffer,
                                  szCommRptSaveName);
    }

    return TRUE;
}

/*
*===========================================================================
** Synopsis:    short TransConvert16CharToIniFile(USHORT husFile, ULONG ulOffset)
*
*     Input:    husFile   - handle of file
*               ulOffset  - offset of file
*
** Return:      0 >= successful
*               0 <  unsuccessful
*
** Description: This function converts 16 characters mnemonics element
*               to ini file.
*===========================================================================
*/
short TransConvert16CharToIniFile(USHORT husFile, ULONG ulOffset)
{
    HGLOBAL hBuffer;                /* handle of global memory */
    VOID FAR *pBuffer;
    ULONG   ulActualPosition;
    short   nCount;
    WCHAR   szMnemonic[PROGRPT_MAX_16CHAR_LEN+1];
    PROGRPT_16CHAR  FAR *p16charMnemonic;
    WCHAR   szAddress[TRANS_CONVERT_WORK_SIZE];

    /* ----- check file offset ----- */
    if ( ulOffset == 0L ) {
        return TRUE;
    }

    /* ----- seek file ----- */
    if ((PifSeekFile(husFile, ulOffset, &ulActualPosition)) < 0) {
        return FALSE;
    }

    /* ----- get global memory ----- */
    hBuffer = GlobalAlloc(GMEM_FIXED, sizeof(PROGRPT_16CHAR));
    if (!hBuffer) {
        return FALSE;
    }

    /* ----- lock global memory ----- */
    pBuffer = GlobalLock(hBuffer);

    /* ----- read 16 characters mnemonics ----- */
    if (PifReadFile((USHORT)husFile, pBuffer, sizeof(PROGRPT_16CHAR))
            != sizeof(PROGRPT_16CHAR)) {

        /* ----- release global memory -----*/
        GlobalUnlock(hBuffer);
        GlobalFree(hBuffer);

        return FALSE;
    }

    /* ----- clear work ----- */
    memset(szMnemonic, 0, sizeof(szMnemonic));

    p16charMnemonic = (PROGRPT_16CHAR FAR *)pBuffer;

    /* ----- convert item ----- */
    for (nCount = 0; nCount < PROGRPT_MAX_16CHAR_ADR; nCount++) {

        wsPepf(szAddress, WIDE("Address%d"), nCount+1);

        memcpy(szMnemonic, &(p16charMnemonic->a16Char[nCount]),
                 PROGRPT_MAX_16CHAR_LEN);

        WritePrivateProfileStringPep(WIDE("16 Characters Mnemonics"),
                                  szAddress,
                                  szMnemonic,
                                  szCommRptSaveName);
    }

    /* ----- release global memory -----*/
    GlobalUnlock(hBuffer);
    GlobalFree(hBuffer);

    return TRUE;
}

/*
*===========================================================================
** Synopsis:    short TransConvert40CharToIniFile(USHORT husFile, ULONG ulOffset)
*
*     Input:    husFile   - handle of file
*               ulOffset  - offset of file
*
** Return:      0 >= successful
*               0 <  unsuccessful
*
** Description: This function converts 40 characters mnemonics element
*               to ini file.
*===========================================================================
*/
short TransConvert40CharToIniFile(USHORT husFile, ULONG ulOffset)
{
    HGLOBAL hBuffer;                /* handle of global memory */
    VOID FAR *pBuffer;
    ULONG   ulActualPosition;
    short   nCount;
    WCHAR   szMnemonic[PROGRPT_MAX_40CHAR_LEN+1];
    PROGRPT_40CHAR FAR *p40charMnemonic;
    WCHAR   szAddress[TRANS_CONVERT_WORK_SIZE];

    /* ----- check file offset ----- */
    if ( ulOffset == 0L ) {
        return TRUE;
    }

    /* ----- seek file ----- */
    if ((PifSeekFile(husFile, ulOffset, &ulActualPosition)) < 0) {
        return FALSE;
    }

    /* ----- get global memory ----- */
    hBuffer = GlobalAlloc(GMEM_FIXED, sizeof(PROGRPT_40CHAR));
    if (!hBuffer) {
        return FALSE;
    }

    /* ----- lock global memory ----- */
    pBuffer = GlobalLock(hBuffer);

    /* ----- read 40 characters mnemonics ----- */
    if (PifReadFile((USHORT)husFile, pBuffer, sizeof(PROGRPT_40CHAR))
            != sizeof(PROGRPT_40CHAR)) {

        /* ----- release global memory -----*/
        GlobalUnlock(hBuffer);
        GlobalFree(hBuffer);

        return FALSE;
    }

    /* ----- clear work ----- */
    memset(szMnemonic, 0, sizeof(szMnemonic));

    p40charMnemonic = (PROGRPT_40CHAR FAR *)pBuffer;

    /* ----- convert item ----- */
    for (nCount = 0; nCount < PROGRPT_MAX_40CHAR_ADR; nCount++) {

        wsPepf(szAddress, WIDE("Address%d"), nCount+1);

        memcpy(szMnemonic, &(p40charMnemonic->a40Char[nCount]),
                 PROGRPT_MAX_40CHAR_LEN);

        WritePrivateProfileStringPep(WIDE("40 Characters Mnemonics"),
                                  szAddress,
                                  szMnemonic,
                                  szCommRptSaveName);
    }

    /* ----- release global memory -----*/
    GlobalUnlock(hBuffer);
    GlobalFree(hBuffer);

    return TRUE;
}

/*
*===========================================================================
** Synopsis:    short TransConvertRptToIniFile(USHORT husFile, ULONG ulOffset,
*                                              SHORT sReportNo)
*
*     Input:    husFile   - handle of file
*               ulOffset  - offset of file
*               sReportNo - report number
*
** Return:      0 >= successful
*               0 <  unsuccessful
*
** Description: This function converts report element to ini file.
*===========================================================================
*/
short TransConvertRptToIniFile(USHORT husFile, ULONG ulOffset, SHORT sReportNo)
{
    HGLOBAL hBuffer;                /* handle of global memory */
    VOID FAR *pBuffer;
    ULONG   ulActualPosition;
    short   nCount;
    PROGRPT_RPTHEAD Header;          /* report format header */
    WCHAR   szReportNo[TRANS_CONVERT_WORK_SIZE];
    WCHAR   szItemNo[TRANS_CONVERT_WORK_SIZE];
    WCHAR   szItem[TRANS_CONVERT_WORK_SIZE];
    PROGRPT_RECORD FAR *pRecord;
    USHORT  usSize;

    /* ----- check file offset ----- */
    if ( ulOffset == 0L ) {
        return TRUE;
    }

    /* ----- seek file ----- */
    if ((PifSeekFile(husFile, ulOffset, &ulActualPosition)) < 0) {
        return FALSE;
    }

    /* ----- read report header ----- */
    if (PifReadFile((USHORT)husFile, &Header, sizeof(PROGRPT_RPTHEAD))
            != sizeof(PROGRPT_RPTHEAD)) {
        return FALSE;
    }

    /* ----- checke report size ----- */
    if ((Header.ulReportSize - sizeof(PROGRPT_RPTHEAD)) == 0) {
        return TRUE;
    }
    /* ----- get global memory ----- */
    hBuffer = GlobalAlloc(GMEM_FIXED,
                          (Header.ulReportSize - sizeof(PROGRPT_RPTHEAD)));
    if (!hBuffer) {
        return FALSE;
    }

    /* ----- lock global memory ----- */
    pBuffer = GlobalLock(hBuffer);

    /* ----- read report data ----- */
    if (PifReadFile((USHORT)husFile, pBuffer,
                    (USHORT)(Header.ulReportSize - sizeof(PROGRPT_RPTHEAD)))
            != (Header.ulReportSize - sizeof(PROGRPT_RPTHEAD))) {
        return FALSE;
    }

    /* ----- set report number ----- */
    wsPepf(szReportNo, WIDE("Report%d"), sReportNo + 1);

    /* ----- write file type ----- */
    wsPepf(szItem, WIDE("%d"), Header.usFileType);

    WritePrivateProfileStringPep(szReportNo,
                              WIDE("File"),
                              szItem,
                              szCommRptSaveName);

    /* ----- write device ----- */
    wsPepf(szItem, WIDE("%d"), Header.usDevice);

    WritePrivateProfileStringPep(szReportNo,
                              WIDE("Device"),
                              szItem,
                              szCommRptSaveName);

    /* ----- write Key Flag ----- */
    wsPepf(szItem, WIDE("%d"), Header.usKeyFlag);

    WritePrivateProfileStringPep(szReportNo,
                              WIDE("Key"),
                              szItem,
                              szCommRptSaveName);

    /* ----- write description ----- */
    memset(szItem, 0, sizeof(szItem));
    memcpy(szItem, Header.auchDesc, PROGRPT_MAX_DESC_LEN);

    WritePrivateProfileStringPep(szReportNo,
                              WIDE("Title"),
                              szItem,
                              szCommRptSaveName);

    pRecord = (PROGRPT_RECORD FAR *)pBuffer;

    /* ----- convert item ----- */
    for (nCount = 0, usSize = sizeof(PROGRPT_RPTHEAD);
         usSize < Header.ulReportSize;
         nCount++, usSize += sizeof(PROGRPT_RECORD), pRecord++) {

        wsPepf(szItemNo, WIDE("Item%d"), nCount+1);
        
        if (pRecord->Print.uchOperation == PROGRPT_OP_LOOP) {

            /* ----- write loop record ----- */
            wsPepf(szItem, szLoopEntry,
                      (USHORT)pRecord->Loop.uchOperation,
                      (USHORT)pRecord->Loop.uchRow,
                      (SHORT)(pRecord->Loop.uchLoopBrace),
                      (USHORT)pRecord->Loop.uchCondition,
                      (USHORT)pRecord->Loop.LoopItem.uchGroup,
                      (USHORT)pRecord->Loop.LoopItem.uchMajor);

            WritePrivateProfileStringPep(szReportNo,
                                      szItemNo,
                                      szItem,
                                      szCommRptSaveName);

        } else if (pRecord->Print.uchOperation == PROGRPT_OP_PRINT) {

            /* ----- write print record ----- */
            wsPepf(szItem, szPrintEntry,
                      (USHORT)pRecord->Print.uchOperation,
                      (USHORT)pRecord->Print.uchRow,
                      (USHORT)pRecord->Print.uchColumn,
                      (USHORT)pRecord->Print.uchMaxColumn,
                      (USHORT)pRecord->Print.auchFormat[0],
                      (USHORT)pRecord->Print.auchFormat[1],
                      (USHORT)pRecord->Print.uchCondition,
                      (USHORT)pRecord->Print.PrintItem.uchGroup,
                      (USHORT)pRecord->Print.PrintItem.uchMajor,
                      (USHORT)pRecord->Print.PrintItem.uchMinor,
                      (USHORT)pRecord->Print.PrintItem.uchItem,
                      (USHORT)pRecord->Print.Operand1.uchGroup,
                      (USHORT)pRecord->Print.Operand1.uchMajor,
                      (USHORT)pRecord->Print.Operand1.uchMinor,
                      (USHORT)pRecord->Print.Operand1.uchItem,
                      (USHORT)pRecord->Print.Operand2.uchGroup,
                      (USHORT)pRecord->Print.Operand2.uchMajor,
                      (USHORT)pRecord->Print.Operand2.uchMinor,
                      (USHORT)pRecord->Print.Operand2.uchItem);

            WritePrivateProfileStringPep(szReportNo,
                                      szItemNo,
                                      szItem,
                                      szCommRptSaveName);

        } else if (pRecord->Print.uchOperation == PROGRPT_OP_MATH) {

            if (pRecord->MathOpe.auchFormat[0] == PROGRPT_MATH_OPERAND) {

                /* ----- write mathematic record
                         ( accumulator = opr1 (+,-,*,/) opr2 ) ----- */
                wsPepf(szItem, szMathOpeEntry,
                      (USHORT)pRecord->MathOpe.uchOperation,
                      (USHORT)pRecord->MathOpe.uchRow,
                      (USHORT)pRecord->MathOpe.uchColumn,
                      (USHORT)pRecord->MathOpe.uchMaxColumn,
                      (USHORT)pRecord->MathOpe.auchFormat[0],
                      (USHORT)pRecord->MathOpe.auchFormat[1],
                      (USHORT)pRecord->MathOpe.uchCondition,
                      (USHORT)pRecord->MathOpe.Accumulator.uchGroup,
                      (USHORT)pRecord->MathOpe.Accumulator.uchMajor,
                      (USHORT)pRecord->MathOpe.Accumulator.uchMinor,
                      (USHORT)pRecord->MathOpe.Accumulator.uchItem,
                      (USHORT)pRecord->MathOpe.Operand1.uchGroup,
                      (USHORT)pRecord->MathOpe.Operand1.uchMajor,
                      (USHORT)pRecord->MathOpe.Operand1.uchMinor,
                      (USHORT)pRecord->MathOpe.Operand1.uchItem,
                      (USHORT)pRecord->MathOpe.Operand2.uchGroup,
                      (USHORT)pRecord->MathOpe.Operand2.uchMajor,
                      (USHORT)pRecord->MathOpe.Operand2.uchMinor,
                      (USHORT)pRecord->MathOpe.Operand2.uchItem);

                WritePrivateProfileStringPep(szReportNo,
                                          szItemNo,
                                          szItem,
                                          szCommRptSaveName);

            } else if (pRecord->MathOpe.auchFormat[0] == PROGRPT_MATH_ONEVALUE) {

                if (pRecord->MathTtl.Accumulator.uchMajor == PROGRPT_ACCUM_TOTAL) {

                    /* ----- write mathematic record
                             ( accumulator.total = LONG (+,-,*,/) opr2) --- */
                    wsPepf(szItem, szMathTtlEntry,
                          (USHORT)pRecord->MathTtl.uchOperation,
                          (USHORT)pRecord->MathTtl.uchRow,
                          (USHORT)pRecord->MathTtl.uchColumn,
                          (USHORT)pRecord->MathTtl.uchMaxColumn,
                          (USHORT)pRecord->MathTtl.auchFormat[0],
                          (USHORT)pRecord->MathTtl.auchFormat[1],
                          (USHORT)pRecord->MathTtl.uchCondition,
                          (USHORT)pRecord->MathTtl.Accumulator.uchGroup,
                          (USHORT)pRecord->MathTtl.Accumulator.uchMajor,
                          (USHORT)pRecord->MathTtl.Accumulator.uchMinor,
                          (USHORT)pRecord->MathTtl.Accumulator.uchItem,
                          pRecord->MathTtl.lOperand1,
                          (USHORT)pRecord->MathTtl.Operand2.uchGroup,
                          (USHORT)pRecord->MathTtl.Operand2.uchMajor,
                          (USHORT)pRecord->MathTtl.Operand2.uchMinor,
                          (USHORT)pRecord->MathTtl.Operand2.uchItem);

                    WritePrivateProfileStringPep(szReportNo,
                                              szItemNo,
                                              szItem,
                                              szCommRptSaveName);

                } else if (pRecord->MathTtl.Accumulator.uchMajor
                            == PROGRPT_ACCUM_COUNTER) {

                    /* ----- write mathematic record
                             ( accumulator.counter = SHORT (+,-,*,/) opr2) --- */
                    wsPepf(szItem, szMathCoEntry,
                          (USHORT)pRecord->MathCo.uchOperation,
                          (USHORT)pRecord->MathCo.uchRow,
                          (USHORT)pRecord->MathCo.uchColumn,
                          (USHORT)pRecord->MathCo.uchMaxColumn,
                          (USHORT)pRecord->MathCo.auchFormat[0],
                          (USHORT)pRecord->MathCo.auchFormat[1],
                          (USHORT)pRecord->MathCo.uchCondition,
                          (USHORT)pRecord->MathCo.Accumulator.uchGroup,
                          (USHORT)pRecord->MathCo.Accumulator.uchMajor,
                          (USHORT)pRecord->MathCo.Accumulator.uchMinor,
                          (USHORT)pRecord->MathCo.Accumulator.uchItem,
                          pRecord->MathCo.sOperand1,
                          (USHORT)pRecord->MathCo.Operand2.uchGroup,
                          (USHORT)pRecord->MathCo.Operand2.uchMajor,
                          (USHORT)pRecord->MathCo.Operand2.uchMinor,
                          (USHORT)pRecord->MathCo.Operand2.uchItem);

                    WritePrivateProfileStringPep(szReportNo,
                                              szItemNo,
                                              szItem,
                                              szCommRptSaveName);
                }
            }
        } 
    }

    /* ----- release global memory -----*/
    GlobalUnlock(hBuffer);
    GlobalFree(hBuffer);

    return TRUE;
}

/*
*===========================================================================
** Synopsis:    short TransConvertToParaFile(HWND hWnd)
*
*     Input:    hWnd       - handle of parent window
*
** Return:      0 >= successful
*               0 <  unsuccessful
*
** Description: This function converts the ini file
*               to binary praorammable report file.
*===========================================================================
*/
short TransConvertToParaFile(HWND hWnd)
{
    SHORT   hsFile;            /* handle of local file */
    PROGRPT_FHEAD   RptHeader; /* programmable report header */
    SHORT   i, sFlexRead;
    WCHAR   szBuffer[TRANS_CONVERT_WORK_SIZE];
    ULONG   ulSize;
    ULONG   ulActualPosition;
    FLEXMEM FlexMem[TRANS_FLEX_ADR_MAX];

    /* ----- read new flex memory data ----- */
    ParaAllRead(CLASS_PARAFLEXMEM,
                (UCHAR *)FlexMem,
                sizeof(FlexMem),
                0,
                (USHORT *)&sFlexRead);

    /*NCR2172*/
    /*if (FlexMem[FLEX_PROGRPT_ADR-1].usRecordNumber == 0) {*/
    if (FlexMem[FLEX_PROGRPT_ADR-1].ulRecordNumber == 0L) {
        return TRUE;
    }

    /* ----- create programmable report file ----- */
    /*NCR2172*/
    /*if (ProgRptCreate((UCHAR)(FlexMem[FLEX_PROGRPT_ADR-1].usRecordNumber))*/
    /*                                            != PROGRPT_SUCCESS) { */
    if (ProgRptCreate((UCHAR)(FlexMem[FLEX_PROGRPT_ADR-1].ulRecordNumber)) != (ULONG)PROGRPT_SUCCESS) {
        /* create Programmable Report file */
        return FALSE;
    }

    /* ----- open programmable report file ----- */
    if ((hsFile = PifOpenFile(szTransProgRpt, "owr")) < 0) {
		TransMessage(hWnd, MB_ICONEXCLAMATION, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL, IDS_TRANS_NAME, IDS_TRANS_ERR_FILEOPENFAILED);
		return FALSE;
    }

    /* ----- check ini file name ----- */
    if (szCommRptSaveName[0] == '\0') {     /* no file name         */
        if (ReportFile((HANDLE)GetWindowLong(hWnd, GWL_HINSTANCE), hWnd) == FALSE) {    /* call Report file function */
            PifCloseFile(hsFile);
            return FALSE;
        }
    }

    /* ----- clear work ----- */
    memset(&RptHeader, 0, sizeof(PROGRPT_FHEAD));

    /* ----- set header mark ----- */
    wcsncpy(RptHeader.auchMark, PROGRPT_MARK_FHEAD, PROGRPT_MAX_MARK_FHEAD);

    /* ----- convert file version ----- */
    if (GetPrivateProfileStringPep(WIDE("Header"),
                                WIDE("Version"),
                                WIDE(""),
                                szBuffer,
                                sizeof(szBuffer) / sizeof(szBuffer[0]),
                                szCommRptSaveName) == 0) {
        memset(szCommRptSaveName, 0, sizeof(szCommRptSaveName));
        PifCloseFile(hsFile);
		TransMessage(hWnd, MB_ICONEXCLAMATION, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL, IDS_TRANS_NAME, IDS_TRANS_ERR_OLDVERSION);
		return FALSE;
    }

    swscanf(szBuffer, WIDE("%x"), &RptHeader.usVersion);

    /* ----- Check Old Version, V3.3 ----- */
    if (RptHeader.usVersion != PROGRPT_VERSION) {
        PifCloseFile(hsFile);
        TransMessage(hWnd, MB_ICONEXCLAMATION, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL, IDS_TRANS_NAME, IDS_TRANS_ERR_OLDVERSION);
        return FALSE;
    }

    /* ----- set EOD offset -----*/
    RptHeader.ulOffsetEOD = sizeof(PROGRPT_FHEAD);

    /* ----- conver EOD  -----*/
    if (TransConvertEODToParaFile((USHORT)hsFile, RptHeader.ulOffsetEOD) == FALSE) {
        PifCloseFile(hsFile);
        return FALSE;
    }

    /* ----- set PTD offset -----*/
    RptHeader.ulOffsetPTD = RptHeader.ulOffsetEOD + sizeof(PROGRPT_EODPTD);

    /* ----- conver PTD  -----*/
    if (TransConvertPTDToParaFile((USHORT)hsFile, RptHeader.ulOffsetPTD) == FALSE) {
        PifCloseFile(hsFile);
        return FALSE;
    }

    /* ----- set 16 char mnemonics offset -----*/
    RptHeader.ulOffset16Char = RptHeader.ulOffsetPTD + sizeof(PROGRPT_EODPTD);

    /* ----- conver 16 characters mnemonics  -----*/
    if (TransConvert16CharToParaFile((USHORT)hsFile, RptHeader.ulOffset16Char) == FALSE) {
        PifCloseFile(hsFile);
        return FALSE;
    }

    /* ----- set 40 char mnemonics offset -----*/
    RptHeader.ulOffset40Char = RptHeader.ulOffset16Char + sizeof(PROGRPT_16CHAR);

    /* ----- conver 40 characters mnemonics  -----*/
    if (TransConvert40CharToParaFile((USHORT)hsFile, RptHeader.ulOffset40Char) == FALSE) {
        PifCloseFile(hsFile);
        return FALSE;
    }

    /* ----- set file size -----*/
    RptHeader.ulFileSize = RptHeader.ulOffset40Char + sizeof(PROGRPT_40CHAR);

    /* ----- conver report data  -----*/
    for (i=0; i < MAX_PROGRPT_FORMAT; i++) {
        if ((ulSize = TransConvertRptToParaFile((USHORT)hsFile, RptHeader.ulFileSize, i)) == -1L) {
            PifCloseFile(hsFile);
            return FALSE;
        }
        if ( ulSize != 0 ){
            RptHeader.aulOffsetReport[i] = RptHeader.ulFileSize;
            RptHeader.ulFileSize += ulSize;
            RptHeader.usNoOfReport++;
        }
    }

    /* ----- seek file ----- */
    if ((PifSeekFile(hsFile, 0L, &ulActualPosition)) < 0) {
        PifCloseFile(hsFile);
        return FALSE;
    }

    /* ----- set file size ----- */
    /*NCR2172*/
    /*if (RptHeader.ulFileSize  > (ULONG)FlexMem[FLEX_PROGRPT_ADR-1].usRecordNumber*/
    if (RptHeader.ulFileSize  > FlexMem[FLEX_PROGRPT_ADR-1].ulRecordNumber * TRANS_PRGRPT_BLOCK_SIZE) {
        PifCloseFile(hsFile);
		TransMessage(hWnd, MB_ICONEXCLAMATION, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL, IDS_TRANS_NAME, IDS_TRANS_ERR_P2RECSIZEBAD);
		return FALSE;
    }

    /* ----- set file size ----- */
    /*NCR2172*/
    /*RptHeader.ulFileSize = (ULONG)FlexMem[FLEX_PROGRPT_ADR-1].usRecordNumber*/
    RptHeader.ulFileSize = FlexMem[FLEX_PROGRPT_ADR-1].ulRecordNumber * TRANS_PRGRPT_BLOCK_SIZE;

    /* ----- write programmable report file header ----- */
    PifWriteFile((USHORT)hsFile, &RptHeader, sizeof(PROGRPT_FHEAD));

    PifCloseFile(hsFile);
    return TRUE;
}

/*
*===========================================================================
** Synopsis:    short TransConvertEODToParaFile(USHORT husFile, ULONG ulOffset)
*
*     Input:    husFile   - handle of file
*               ulOffset  - offset of file
*
** Return:      0 >= successful
*               0 <  unsuccessful
*
** Description: This function converts EOD element to binary programable
*               report file.
*===========================================================================
*/
short TransConvertEODToParaFile(USHORT husFile, ULONG ulOffset)
{
    PROGRPT_EODPTD  EodPtd; 
    ULONG   ulActualPosition;
    short   nCount;
    WCHAR   szBuffer[TRANS_CONVERT_WORK_SIZE];
    WCHAR   szItem[TRANS_CONVERT_WORK_SIZE];

        /* ----- check file offset ----- */
    if ( ulOffset == 0L ) {
        return TRUE;
    }

    /* ----- clear work ----- */
    memset(&EodPtd, 0, sizeof(PROGRPT_EODPTD));

    /* ----- set header mark ----- */
    wcsncpy( EodPtd.auchMark, PROGRPT_MARK_EOD, PROGRPT_MAX_MARK_LEN );

    /* ----- set status ----- */
    GetPrivateProfileStringPep(WIDE("End of Day"),
                            WIDE("State"),
                            WIDE(""),
                            szBuffer,
                            sizeof(szBuffer) / sizeof(szBuffer[0]),
                            szCommRptSaveName);
    swscanf(szBuffer, WIDE("%hd"), &EodPtd.usUseProgRpt);

    /* ----- convert item ----- */
    for (nCount = 0; nCount < MAX_PROGRPT_EODPTD; nCount++) {

        wsPepf(szItem, WIDE("Address%d"), nCount+1);

        if (GetPrivateProfileStringPep(WIDE("End of Day"),
                                    szItem,
                                    WIDE(""),
                                    szBuffer,
                                    sizeof(szBuffer) / sizeof(szBuffer[0]),
                                    szCommRptSaveName) == 0) {
            break;
        }

        swscanf(szBuffer, WIDE("%hd,%hd"),
                &EodPtd.aEODPTD[nCount].usFileType,
                &EodPtd.aEODPTD[nCount].usReportNo);
    }

    /* ----- set number of EOD record ----- */
    EodPtd.usNoOfRecord = nCount;

    /* ----- seek file ----- */
    if ((PifSeekFile(husFile, ulOffset, &ulActualPosition)) < 0) {
        return FALSE;
    }

    /* ----- write EOD ----- */
    PifWriteFile((USHORT)husFile, &EodPtd, sizeof(PROGRPT_EODPTD));

    return TRUE;
}

/*
*===========================================================================
** Synopsis:    short TransConvertPTDToParaFile(USHORT husFile, ULONG ulOffset)
*
*     Input:    husFile   - handle of file
*               ulOffset  - offset of file
*
** Return:      0 >= successful
*               0 <  unsuccessful
*
** Description: This function converts PTD element to binary programmable
*               report file.
*===========================================================================
*/
short TransConvertPTDToParaFile(USHORT husFile, ULONG ulOffset)
{
    PROGRPT_EODPTD  EodPtd; 
    ULONG   ulActualPosition;
    short   nCount;
    WCHAR   szBuffer[TRANS_CONVERT_WORK_SIZE];
    WCHAR   szItem[TRANS_CONVERT_WORK_SIZE];

    /* ----- check file offset ----- */
    if ( ulOffset == 0L ) {
        return TRUE;
    }

    /* ----- clear work ----- */
    memset(&EodPtd, 0, sizeof(PROGRPT_EODPTD));

    /* ----- set header mark ----- */
    wcsncpy( EodPtd.auchMark, PROGRPT_MARK_PTD, PROGRPT_MAX_MARK_LEN);

    /* ----- set status ----- */
    GetPrivateProfileStringPep(WIDE("Period to Day"),
                            WIDE("State"),
                            WIDE(""),
                            szBuffer,
                            sizeof(szBuffer) / sizeof(szBuffer[0]),
                            szCommRptSaveName);
    swscanf(szBuffer, WIDE("%hd"), &EodPtd.usUseProgRpt);

    /* ----- convert item ----- */
    for (nCount = 0; nCount < MAX_PROGRPT_EODPTD; nCount++) {

        wsPepf(szItem, WIDE("Address%d"), nCount+1);

        if (GetPrivateProfileStringPep(WIDE("Period to Day"),
                                    szItem,
                                    WIDE(""),
                                    szBuffer,
                                    sizeof(szBuffer) / sizeof(szBuffer[0]),
                                    szCommRptSaveName) == 0) {
			break;
        }

        swscanf(szBuffer, WIDE("%hd,%hd"),
                &EodPtd.aEODPTD[nCount].usFileType,
                &EodPtd.aEODPTD[nCount].usReportNo);
    }

    /* ----- set number of EOD record ----- */
    EodPtd.usNoOfRecord = nCount;

    /* ----- seek file ----- */
    if ((PifSeekFile(husFile, ulOffset, &ulActualPosition)) < 0) {
        return FALSE;
    }

    /* ----- write PTD ----- */
    PifWriteFile((USHORT)husFile, &EodPtd, sizeof(PROGRPT_EODPTD));

    return TRUE;
}

/*
*===========================================================================
** Synopsis:    short TransConvert16CharToParaFile(USHORT husFile, ULONG ulOffset)
*
*     Input:    husFile   - handle of file
*               ulOffset  - offset of file
*
** Return:      0 >= successful
*               0 <  unsuccessful
*
** Description: This function converts 16 characters mnemonics element
*               to binary programmable report file.
*===========================================================================
*/
short TransConvert16CharToParaFile(USHORT husFile, ULONG ulOffset)
{
    HGLOBAL hBuffer;                /* handle of global memory */
    VOID FAR *pBuffer;
    ULONG   ulActualPosition;
    short   nCount;
    WCHAR   szMnemonic[PROGRPT_MAX_16CHAR_LEN+1];
    PROGRPT_16CHAR FAR *p16charMnemonic;
    WCHAR   szAddress[TRANS_CONVERT_WORK_SIZE];

    /* ----- check file offset ----- */
    if ( ulOffset == 0L ) {
        return TRUE;
    }

    /* ----- get global memory ----- */
    hBuffer = GlobalAlloc(GMEM_FIXED, sizeof(PROGRPT_16CHAR));
    if (!hBuffer) {
        return FALSE;
    }

    /* ----- lock global memory ----- */
    pBuffer = GlobalLock(hBuffer);

    p16charMnemonic = (PROGRPT_16CHAR FAR *)pBuffer;

    /* ----- clear work ----- */
    memset(p16charMnemonic, 0, sizeof(PROGRPT_16CHAR));

    /* ----- set header mark ----- */
    wcsncpy(p16charMnemonic->auchMark, PROGRPT_MARK_16CHAR, PROGRPT_MAX_MARK_LEN);

    /* ----- set number of record ----- */
    p16charMnemonic->usNoOfRecord = PROGRPT_MAX_16CHAR_ADR;

    /* ----- clear work ----- */
    memset(szMnemonic, 0, sizeof(szMnemonic));

    /* ----- convert item ----- */
    for (nCount = 0; nCount < PROGRPT_MAX_16CHAR_ADR; nCount++) {

        wsPepf(szAddress, WIDE("Address%d"), nCount+1);

        GetPrivateProfileStringPep(WIDE("16 Characters Mnemonics"),
                                szAddress,
                                WIDE(""),
                                szMnemonic,
                                sizeof(szMnemonic) / sizeof(szMnemonic[0]),
                                szCommRptSaveName);

		wcscpy(p16charMnemonic->a16Char[nCount].auchMnemonic,
				szMnemonic);
/*
        swscanf(szMnemonic, WIDE("%s"),
                p16charMnemonic->a16Char[nCount].auchMnemonic);
*/
/*
        memcpy(p16charMnemonic->a16Char[nCount].auchMnemonic,
                 (LPCSTR)szMnemonic, PROGRPT_MAX_16CHAR_LEN );
*/
	}

    /* ----- seek file ----- */
    if ((PifSeekFile(husFile, ulOffset, &ulActualPosition)) < 0) {

        /* ----- release global memory -----*/
        GlobalUnlock(hBuffer);
        GlobalFree(hBuffer);
        return FALSE;
    }

    /* ----- write 16 characters mnemonics ----- */
    PifWriteFile((USHORT)husFile, p16charMnemonic, sizeof(PROGRPT_16CHAR));

    /* ----- release global memory -----*/
    GlobalUnlock(hBuffer);
    GlobalFree(hBuffer);

    return TRUE;
}

/*
*===========================================================================
** Synopsis:    short TransConvert40CharToParaFile(USHORT husFile, ULONG ulOffset)
*
*     Input:    husFile   - handle of file
*               ulOffset  - offset of file
*
** Return:      0 >= successful
*               0 <  unsuccessful
*
** Description: This function converts 40 characters mnemonics element
*               to binary programmable report file.
*===========================================================================
*/
short TransConvert40CharToParaFile(USHORT husFile, ULONG ulOffset)
{
    HGLOBAL hBuffer;                /* handle of global memory */
    VOID FAR *pBuffer;
    ULONG   ulActualPosition;
    short   nCount;
    WCHAR   szMnemonic[PROGRPT_MAX_40CHAR_LEN+1];
    PROGRPT_40CHAR FAR *p40charMnemonic;
    WCHAR   szAddress[TRANS_CONVERT_WORK_SIZE];

    /* ----- check file offset ----- */
    if ( ulOffset == 0L ) {
        return TRUE;
    }

    /* ----- get global memory ----- */
    hBuffer = GlobalAlloc(GMEM_FIXED, sizeof(PROGRPT_40CHAR));
    if (!hBuffer) {
        return FALSE;
    }

    /* ----- lock global memory ----- */
    pBuffer = GlobalLock(hBuffer);

    p40charMnemonic = (PROGRPT_40CHAR FAR *)pBuffer;

    /* ----- clear work ----- */
    memset((LPSTR)p40charMnemonic, 0, sizeof(PROGRPT_40CHAR));

    /* ----- set header mark ----- */
    wcsncpy(p40charMnemonic->auchMark, PROGRPT_MARK_40CHAR, PROGRPT_MAX_MARK_LEN);

    /* ----- set number of record ----- */
    p40charMnemonic->usNoOfRecord = PROGRPT_MAX_40CHAR_ADR;

    /* ----- clear work ----- */
    memset((LPSTR)szMnemonic, 0, sizeof(szMnemonic));

    /* ----- convert item ----- */
    for (nCount = 0; nCount < PROGRPT_MAX_40CHAR_ADR; nCount++) {

        wsPepf(szAddress, WIDE("Address%d"), nCount+1);

        GetPrivateProfileStringPep(WIDE("40 Characters Mnemonics"),
                                szAddress,
                                WIDE(""),
                                szMnemonic,
                                sizeof(szMnemonic) / sizeof(szMnemonic[0]),
                                szCommRptSaveName);
		wcscpy(p40charMnemonic->a40Char[nCount].auchMnemonic,
				szMnemonic);
/*
        swscanf(szMnemonic, WIDE("%s"),
                p40charMnemonic->a40Char[nCount].auchMnemonic);
*/
/*
        memcpy(p40charMnemonic->a40Char[nCount].auchMnemonic,
                 (LPCSTR)szMnemonic, PROGRPT_MAX_40CHAR_LEN );
*/
    }

    /* ----- seek file ----- */
    if ((PifSeekFile(husFile, ulOffset, &ulActualPosition)) < 0) {

        /* ----- release global memory -----*/
        GlobalUnlock(hBuffer);
        GlobalFree(hBuffer);
        return FALSE;
    }

    /* ----- write 40 characters mnemonics ----- */
    PifWriteFile((USHORT)husFile, p40charMnemonic, sizeof(PROGRPT_40CHAR));

    /* ----- release global memory -----*/
    GlobalUnlock(hBuffer);
    GlobalFree(hBuffer);

    return TRUE;
}

/*
*===========================================================================
** Synopsis:    short TransConvertRptToParaFile(USHORT husFile, ULONG ulOffset,
*                                              SHORT sReportNo)
*
*     Input:    husFile   - handle of file
*               ulOffset  - offset of file
*               sReportNo - report number
*
** Return:      0 >= successful
*               0 <  unsuccessful
*
** Description: This function converts report element
*               to binary programmable report file.
*===========================================================================
*/
LONG TransConvertRptToParaFile(USHORT husFile, ULONG ulOffset, SHORT sReportNo)
{
    HGLOBAL hBuffer;                  /* handle of global memory */
    VOID    *pBuffer;
    PROGRPT_RPTHEAD  *pHeader;         /* report format header */
    PROGRPT_RECORD   *pRecord;         /* pointer to current record in the buffer */
    ULONG   ulActualPosition, ulSize;
    short   nCount;
    WCHAR   szReportNo[TRANS_CONVERT_WORK_SIZE];
    WCHAR   szItemNo[TRANS_CONVERT_WORK_SIZE];
    WCHAR   szItem[TRANS_CONVERT_WORK_SIZE];
    USHORT  usLoop1Cnt = 0, usLoop2Cnt = 0;
    USHORT  usLoop1Exist = 0, usLoop2Exist = 0;
    USHORT  usAllCount = 0, usLoopCount = 0, usPrintCount = 0, usMathCount = 0;

    /* ----- check file offset ----- */
    if ( ulOffset == 0L ) {
        return 0L;
    }

    /* ----- get global memory ----- */
    hBuffer = GlobalAlloc(GMEM_FIXED, 0xFFFF);
    if (!hBuffer) {
        return -1L;
    }

    /* ----- lock global memory ----- */
    pBuffer = GlobalLock(hBuffer);

    /* ----- clear work ----- */
    memset(pBuffer, 0, 0xFFFF);

    /* ----- set report number ----- */
    wsPepf(szReportNo, WIDE("Report%d"), sReportNo + 1);

    /* ----- set header pointer ----- */
    pHeader = pBuffer;

    /* ----- set header mark ----- */
    wcsncpy(pHeader->auchMark, PROGRPT_MARK_REPORT, PROGRPT_MAX_MARK_LEN);

    /* ----- get file type ----- */
    GetPrivateProfileStringPep(szReportNo, WIDE("File"), WIDE(""), szItem, sizeof(szItem) / sizeof(szItem[0]), szCommRptSaveName);

    swscanf(szItem, WIDE("%hd"), &pHeader->usFileType);

    /* ----- get device ----- */
    if (GetPrivateProfileStringPep(szReportNo, WIDE("Device"), WIDE(""), szItem, sizeof(szItem) / sizeof(szItem[0]), szCommRptSaveName) == 0) {
        /* ----- release global memory -----*/
        GlobalUnlock(hBuffer);
        GlobalFree(hBuffer);
        return 0L;
    }

    swscanf(szItem, WIDE("%hd"), &pHeader->usDevice);

    /* ----- get Key Flag ----- */
    GetPrivateProfileStringPep(szReportNo, WIDE("Key"), WIDE(""), szItem, sizeof(szItem) / sizeof(szItem[0]), szCommRptSaveName);

    swscanf(szItem, WIDE("%hd"), &pHeader->usKeyFlag);

    /* ----- get description ----- */
    memset(szItem, 0, sizeof(szItem));

    GetPrivateProfileStringPep(szReportNo, WIDE("Title"), WIDE(""), szItem, sizeof(szItem) / sizeof(szItem[0]), szCommRptSaveName);

    memcpy(pHeader->auchDesc, szItem, PROGRPT_MAX_DESC_LEN * sizeof(WCHAR));

    pRecord = (PROGRPT_RECORD *)(pHeader + 1);

    /* ----- convert item ----- */
    for (nCount = 0; ; nCount++) {
		int  iScanCount = 0;
		USHORT  usOperation = 0, usRow, usLoopBrace, usCondition, usItemGroup,
			usItemMajor, usItemMinor, usColumn, usMaxColumn, usFormat1,
			usFormat2, usItemItem, usOperand1Group, usOperand1Major,
			usOperand1Minor, usOperand1Item, usOperand2Group, usOperand2Major,
			usOperand2Minor, usOperand2Item;

        wsPepf(szItemNo, WIDE("Item%d"), nCount+1);
        
        /* ----- get 1 record ----- */
        if (GetPrivateProfileStringPep(szReportNo, szItemNo, WIDE(""), szItem, sizeof(szItem) / sizeof(szItem[0]), szCommRptSaveName) == 0) {
            break;
        }

        memset(pRecord, 0, sizeof(PROGRPT_RECORD));
        swscanf(szItem, WIDE("%hd"), &usOperation);

        if (usOperation == PROGRPT_OP_LOOP) {
			PROGRPT_RECORD *pLoop1Off = 0;
			PROGRPT_RECORD *pLoop2Off = 0;

            /* ----- write loop record ----- */
			iScanCount = swscanf(szItem, szLoopEntry,
                    &usOperation, &usRow, &usLoopBrace, &usCondition,
                    &usItemGroup, &usItemMajor);

            pRecord->Loop.uchOperation = (UCHAR)usOperation;
            pRecord->Loop.uchRow = (UCHAR)usRow;
            pRecord->Loop.uchLoopBrace = (UCHAR)usLoopBrace;
            pRecord->Loop.uchCondition = (UCHAR)usCondition;
            pRecord->Loop.LoopItem.uchGroup = (UCHAR)usItemGroup;
            pRecord->Loop.LoopItem.uchMajor = (UCHAR)usItemMajor;
        
            if (pRecord->Loop.uchLoopBrace == PROGRPT_LOOP_BEGIN_ROOT) {
                pLoop1Off = pRecord;
                usLoop1Cnt = 0;
                usLoop1Exist = 1;
                usLoop2Cnt++;
            } else if (pRecord->Loop.uchLoopBrace == PROGRPT_LOOP_BEGIN_NEST) {
                pLoop2Off = pRecord;
                usLoop2Cnt = 0;
                usLoop2Exist = 1;
                usLoop1Cnt++;
            } else if (pRecord->Loop.uchLoopBrace == PROGRPT_LOOP_END_ROOT) {
                usLoop1Cnt++;
                if (usLoop1Exist && pLoop1Off) {
                    pLoop1Off->Loop.usLoopEnd = usLoop1Cnt;
                    usLoop1Exist = 0;
                }
                usLoop2Cnt++;
            } else if (pRecord->Loop.uchLoopBrace == PROGRPT_LOOP_END_NEST) {
                usLoop2Cnt++;
                if (usLoop2Exist && pLoop2Off) {
                    pLoop2Off->Loop.usLoopEnd = usLoop2Cnt;
                    usLoop2Exist = 0;
                }
                usLoop1Cnt++;
            } else {
                usLoop1Cnt++;
                usLoop2Cnt++;
            }

            usAllCount++;
            usLoopCount++;
            pRecord++;

        } else if (usOperation == PROGRPT_OP_PRINT) {

            /* ----- write print record ----- */
			iScanCount = swscanf(szItem, szPrintEntry,
                   &usOperation, &usRow, &usColumn, &usMaxColumn,
                   &usFormat1, &usFormat2, &usCondition, &usItemGroup,
                   &usItemMajor, &usItemMinor, &usItemItem, &usOperand1Group,
                   &usOperand1Major, &usOperand1Minor, &usOperand1Item,
                   &usOperand2Group, &usOperand2Major, &usOperand2Minor,
                   &usOperand2Item);

            pRecord->Print.uchOperation = (UCHAR)usOperation;
            pRecord->Print.uchRow = (UCHAR)usRow;
            pRecord->Print.uchColumn = (UCHAR)usColumn;
            pRecord->Print.uchMaxColumn = (UCHAR)usMaxColumn;
            pRecord->Print.auchFormat[0] = (UCHAR)usFormat1;
            pRecord->Print.auchFormat[1] = (UCHAR)usFormat2;
            pRecord->Print.uchCondition = (UCHAR)usCondition;
            pRecord->Print.PrintItem.uchGroup = (UCHAR)usItemGroup;
            pRecord->Print.PrintItem.uchMajor = (UCHAR)usItemMajor;
            pRecord->Print.PrintItem.uchMinor = (UCHAR)usItemMinor;
            pRecord->Print.PrintItem.uchItem = (UCHAR)usItemItem;
            pRecord->Print.Operand1.uchGroup = (UCHAR)usOperand1Group;
            pRecord->Print.Operand1.uchMajor = (UCHAR)usOperand1Major;
            pRecord->Print.Operand1.uchMinor = (UCHAR)usOperand1Minor;
            pRecord->Print.Operand1.uchItem = (UCHAR)usOperand1Item;
            pRecord->Print.Operand2.uchGroup = (UCHAR)usOperand2Group;
            pRecord->Print.Operand2.uchMajor = (UCHAR)usOperand2Major;
            pRecord->Print.Operand2.uchMinor = (UCHAR)usOperand2Minor;
            pRecord->Print.Operand2.uchItem = (UCHAR)usOperand2Item;

            usAllCount++;
            usPrintCount++;
            usLoop1Cnt++;
            usLoop2Cnt++;
            pRecord++;

        } else if (usOperation == PROGRPT_OP_MATH) {

            /* ----- convert temp. mathematic record ----- */
			iScanCount = swscanf(szItem, szMathTmpEntry,
                      &usOperation, &usRow, &usColumn, &usMaxColumn,
                      &usFormat1, &usFormat2, &usCondition, &usItemGroup,
                      &usItemMajor, &usItemMinor);

            if (usFormat1 == PROGRPT_MATH_OPERAND) {

                /* ----- write mathematic record
                         ( accumulator = opr1 (+,-,*,/) opr2 ) ----- */
				iScanCount = swscanf(szItem, szMathOpeEntry,
                      &usOperation, &usRow, &usColumn, &usMaxColumn,
                      &usFormat1, &usFormat2, &usCondition, &usItemGroup,
                      &usItemMajor, &usItemMinor, &usItemItem,
                      &usOperand1Group, &usOperand1Major, &usOperand1Minor,
                      &usOperand1Item, &usOperand2Group, &usOperand2Major,
                      &usOperand2Minor, &usOperand2Item);

                pRecord->MathOpe.uchOperation = (UCHAR)usOperation;
                pRecord->MathOpe.uchRow = (UCHAR)usRow;
                pRecord->MathOpe.uchColumn = (UCHAR)usColumn;
                pRecord->MathOpe.uchMaxColumn = (UCHAR)usMaxColumn;
                pRecord->MathOpe.auchFormat[0] = (UCHAR)usFormat1;
                pRecord->MathOpe.auchFormat[1] = (UCHAR)usFormat2;
                pRecord->MathOpe.uchCondition = (UCHAR)usCondition;
                pRecord->MathOpe.Accumulator.uchGroup = (UCHAR)usItemGroup;
                pRecord->MathOpe.Accumulator.uchMajor = (UCHAR)usItemMajor;
                pRecord->MathOpe.Accumulator.uchMinor = (UCHAR)usItemMinor;
                pRecord->MathOpe.Accumulator.uchItem = (UCHAR)usItemItem;
                pRecord->MathOpe.Operand1.uchGroup = (UCHAR)usOperand1Group;
                pRecord->MathOpe.Operand1.uchMajor = (UCHAR)usOperand1Major;
                pRecord->MathOpe.Operand1.uchMinor = (UCHAR)usOperand1Minor;
                pRecord->MathOpe.Operand1.uchItem = (UCHAR)usOperand1Item;
                pRecord->MathOpe.Operand2.uchGroup = (UCHAR)usOperand2Group;
                pRecord->MathOpe.Operand2.uchMajor = (UCHAR)usOperand2Major;
                pRecord->MathOpe.Operand2.uchMinor = (UCHAR)usOperand2Minor;
                pRecord->MathOpe.Operand2.uchItem = (UCHAR)usOperand2Item;

                usAllCount++;
                usMathCount++;
                usLoop1Cnt++;
                usLoop2Cnt++;
                pRecord++;

            } else if (usFormat1 == PROGRPT_MATH_ONEVALUE) {

                if (usItemMajor == PROGRPT_ACCUM_TOTAL) {

                    /* ----- write mathematic record
                             ( accumulator.total = LONG (+,-,*,/) opr2) --- */
					iScanCount = swscanf(szItem, szMathTtlEntry,
                          &usOperation, &usRow, &usColumn, &usMaxColumn,
                          &usFormat1, &usFormat2, &usCondition, &usItemGroup,
                          &usItemMajor, &usItemMinor, &usItemItem,
                          &pRecord->MathTtl.lOperand1,
                          &usOperand2Group, &usOperand2Major, &usOperand2Minor,
                          &usOperand2Item);

                    pRecord->MathTtl.uchOperation = (UCHAR)usOperation;
                    pRecord->MathTtl.uchRow = (UCHAR)usRow;
                    pRecord->MathTtl.uchColumn = (UCHAR)usColumn;
                    pRecord->MathTtl.uchMaxColumn = (UCHAR)usMaxColumn;
                    pRecord->MathTtl.auchFormat[0] = (UCHAR)usFormat1;
                    pRecord->MathTtl.auchFormat[1] = (UCHAR)usFormat2;
                    pRecord->MathTtl.uchCondition = (UCHAR)usCondition;
                    pRecord->MathTtl.Accumulator.uchGroup = (UCHAR)usItemGroup;
                    pRecord->MathTtl.Accumulator.uchMajor = (UCHAR)usItemMajor;
                    pRecord->MathTtl.Accumulator.uchMinor = (UCHAR)usItemMinor;
                    pRecord->MathTtl.Accumulator.uchItem = (UCHAR)usItemItem;
                    pRecord->MathTtl.Operand2.uchGroup = (UCHAR)usOperand2Group;
                    pRecord->MathTtl.Operand2.uchMajor = (UCHAR)usOperand2Major;
                    pRecord->MathTtl.Operand2.uchMinor = (UCHAR)usOperand2Minor;
                    pRecord->MathTtl.Operand2.uchItem = (UCHAR)usOperand2Item;

                    usAllCount++;
                    usMathCount++;
                    usLoop1Cnt++;
                    usLoop2Cnt++;
                    pRecord++;

                } else if (usItemMajor == PROGRPT_ACCUM_COUNTER) {

                    /* ----- write mathematic record
                             ( accumulator.counter = SHORT (+,-,*,/) opr2) --- */
					iScanCount = swscanf(szItem, szMathCoEntry,
                          &usOperation, &usRow, &usColumn, &usMaxColumn,
                          &usFormat1, &usFormat2, &usCondition, &usItemGroup,
                          &usItemMajor, &usItemMinor, &usItemItem,
                          &pRecord->MathCo.sOperand1,
                          &usOperand2Group, &usOperand2Major, &usOperand2Minor,
                          &usOperand2Item);

                    pRecord->MathCo.uchOperation = (UCHAR)usOperation;
                    pRecord->MathCo.uchRow = (UCHAR)usRow;
                    pRecord->MathCo.uchColumn = (UCHAR)usColumn;
                    pRecord->MathCo.uchMaxColumn = (UCHAR)usMaxColumn;
                    pRecord->MathCo.auchFormat[0] = (UCHAR)usFormat1;
                    pRecord->MathCo.auchFormat[1] = (UCHAR)usFormat2;
                    pRecord->MathCo.uchCondition = (UCHAR)usCondition;
                    pRecord->MathCo.Accumulator.uchGroup = (UCHAR)usItemGroup;
                    pRecord->MathCo.Accumulator.uchMajor = (UCHAR)usItemMajor;
                    pRecord->MathCo.Accumulator.uchMinor = (UCHAR)usItemMinor;
                    pRecord->MathCo.Accumulator.uchItem = (UCHAR)usItemItem;
                    pRecord->MathCo.Operand2.uchGroup = (UCHAR)usOperand2Group;
                    pRecord->MathCo.Operand2.uchMajor = (UCHAR)usOperand2Major;
                    pRecord->MathCo.Operand2.uchMinor = (UCHAR)usOperand2Minor;
                    pRecord->MathCo.Operand2.uchItem = (UCHAR)usOperand2Item;

                    usAllCount++;
                    usMathCount++;
                    usLoop1Cnt++;
                    usLoop2Cnt++;
                    pRecord++;

                }
            }
        } 
    }

    /* ----- set count ----- */
    pHeader->usNoOfLoop = usLoopCount;
    pHeader->usNoOfPrint = usPrintCount;
    pHeader->usNoOfMath = usMathCount;

    /* ----- set size ----- */
    ulSize = (ULONG)((usAllCount * sizeof(PROGRPT_RECORD)) + sizeof(PROGRPT_RPTHEAD));
    pHeader->ulReportSize = ulSize;

    /* ----- seek file ----- */
    if ((PifSeekFile(husFile, ulOffset, &ulActualPosition)) < 0) {

        /* ----- release global memory -----*/
        GlobalUnlock(hBuffer);
        GlobalFree(hBuffer);
        return -1L;
    }

    /* ----- read report header ----- */
    PifWriteFile((USHORT)husFile, pHeader, ulSize);

    /* ----- release global memory -----*/
    GlobalUnlock(hBuffer);
    GlobalFree(hBuffer);

    return ulSize;
}
/* ===== End of TRANCONV.C ===== */



