/*
* ---------------------------------------------------------------------------
* Title         :   Report File procedure
* Category      :   Report, NCR 2170 PEP for Windows (HP US Model)
* Program Name  :   RPTEXEC.C
* Copyright (C) :   1996, NCR Corp. E&M OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract : This module provides the following functions
*               ReportExec():           Report Execute user selecting file
* ---------------------------------------------------------------------------
* Update Histories
*
*   Date    : Ver.Rev. :   Name     : Description
* Feb-05-96 : 03.01.00 : M.Suzuki   : Initial (HP US R3.1)
*
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
#include    <shellapi.h>

#include    "ecr.h"
#include    "paraequ.h"
#include    "para.h"

#include    "pep.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "report.h"
#include    "reportl.h"

static WCHAR szPep[] = WIDE("\\PARAMPEP.INI");
static WCHAR szExec[] = WIDE("\\PROGRPT.EXE");

/************************************************************\
|*                                                          *|
|*  Synopsis:   BOOL ReportExec(HANDLE hInst, HWND hwnd)    *|
|*              hInst:  instance handle                     *|
|*              hwnd:   handle of the main window           *|
|*                                                          *|
|* Description: This function creates the "Programmable     *|
|*              Report" dialog.                             *|
|*                                                          *|
\************************************************************/
BOOL ReportExec(HANDLE hInst, HWND hwnd)
{
    int     iLen;
    WCHAR    szBuffer[REPORT_MAXLEN];
    WCHAR    szDir[REPORT_MAXLEN];
    WCHAR    szExecFile[REPORT_MAXLEN];
    WCHAR    szErrMsg[REPORT_MAXLEN_TEXT];
    PARAFLEXMEM FlexData;
//    OFSTRUCT        ofs;

    LoadString(hResourceDll, IDS_REPORT_CAP_EXEC, szReportCaption, PEP_STRING_LEN_MAC(szReportCaption));

    /* ----- Set Target Address to Programmable Report ----- */
    FlexData.uchAddress = FLEX_PROGRPT_ADR;

    /* ----- Read the Flexible Memory Data of Programmable Report ----- */
    ParaFlexMemRead(&FlexData);

    /* NCR2172 */
    if (!FlexData.ulRecordNumber) {

        /* ----- Load String from Resource ----- */
        LoadString(hResourceDll, IDS_REPORT_ERR_FLEX_EMPTY, szErrMsg, PEP_STRING_LEN_MAC(szErrMsg));

        /* ---- Display Caution Message ----- */
        MessageBeep(MB_ICONEXCLAMATION);
        MessageBoxPopUp(hwnd, szErrMsg, szReportCaption, MB_ICONEXCLAMATION | MB_OK);
        return TRUE;
    }

    /* ----- check ini file name ----- */
    if (szCommRptSaveName[0] == '\0') {     /* no file name         */
        if (ReportFile(hInst, hwnd) == FALSE) {    /* call Report file function */
            return FALSE;
        }
    }

	GetPepModuleFileName(hInst, szBuffer, REPORT_MAXLEN);
	iLen = wcslen(szBuffer);
	if (!((iLen > 8) && (szBuffer[iLen - 8] == '\\'))) {   /* must end in \PEP.EXE */
        LoadString(hResourceDll, IDS_REPORT_ERR_PEP, szErrMsg, PEP_STRING_LEN_MAC(szErrMsg));
        MessageBoxPopUp(hwnd, szErrMsg, szReportCaption, MB_ICONEXCLAMATION | MB_OK);
        return TRUE;
    }
	iLen -= 8;
	szBuffer[iLen] = '\0';

	lstrcpyW(szDir, szBuffer);
	lstrcpyW(szExecFile, szBuffer);
	lstrcatW(szBuffer, szPep);
	lstrcatW(szExecFile, szExec);

    /* ----- delete ini. file ----- */
    DeleteFilePep(szBuffer);
	//OpenFile((LPCSTR)szBuffer, (OFSTRUCT FAR *)&ofs, OF_DELETE);

    ReportSetParameter(szBuffer);
    ReportSetTransMnemo(szBuffer);
    ReportSetReportMnemo(szBuffer);
    ReportSetSpecialMnemo(szBuffer);
    ReportSetAdjectiveMnemo(szBuffer);
    ReportSetMajorDeptMnemo(szBuffer);

    if (ShellExecutePep(hwnd, NULL, szExecFile, szCommRptSaveName, szDir, SW_SHOWNORMAL) < 32) {
        LoadString(hResourceDll, IDS_REPORT_ERR_EXEC, szErrMsg, PEP_STRING_LEN_MAC(szErrMsg));
        MessageBoxPopUp(hwnd, szErrMsg, szReportCaption, MB_ICONEXCLAMATION | MB_OK);
    }

    return TRUE;
}

/****************************************************************\
|*                                                              *|
|*  Synopsis:   void ReportSetParameter(LPCSTR lpszFname)       *|
|*                                                              *|
|*               LPCSTR  lpszFname - selected file name         *|
|*                                                              *|
|* Description: This function set the parameter to PARAPEP.INI. *|
\****************************************************************/
void ReportSetParameter(LPCWSTR lpszFname)
{
    USHORT          usReturnLen;
    FLEXMEM         FlexMem[FLEX_ADR_MAX];
	WCHAR           szWork[REPORT_MAXLEN] = { 0 };

    ParaAllRead(CLASS_PARAFLEXMEM, (UCHAR *)FlexMem, sizeof(FlexMem), 0, &usReturnLen);

    wsPepf(szWork, WIDE("%d"), FlexMem[FLEX_PROGRPT_ADR - 1].ulRecordNumber);
    WritePrivateProfileStringPep(WIDE("Parameters"), WIDE("Block"), szWork, lpszFname);

    if (ParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {
        WritePrivateProfileStringPep(WIDE("Parameters"), WIDE("Date"), WIDE("1"), lpszFname);
    } else {
        WritePrivateProfileStringPep(WIDE("Parameters"), WIDE("Date"), WIDE("0"), lpszFname);
    }

    if (ParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT3)) {
        WritePrivateProfileStringPep(WIDE("Parameters"), WIDE("Time"), WIDE("1"), lpszFname);
    } else {
        WritePrivateProfileStringPep(WIDE("Parameters"), WIDE("Time"), WIDE("0"), lpszFname);
    }
}

/****************************************************************\
|*                                                              *|
|*  Synopsis:   void ReportSetTransMnemo(LPCSTR lpszFname)      *|
|*                                                              *|
|*               LPCSTR  lpszFname - selected file name         *|
|*                                                              *|
|* Description: This function set the Transaction mnemonics     *|
|*              to PRAMPEP.INI.                                 *|
\****************************************************************/
void ReportSetTransMnemo(LPCWSTR lpszFname)
{
    USHORT          usReturnLen;
    WCHAR           szWork[TRN_ADR_MAX][PARA_TRANSMNEMO_LEN] = { 0 };
    WCHAR           szNumber[REPORT_MAXLEN] = { 0 };
    short           nCount;

    wsPepf(szNumber, WIDE("%d"), TRN_ADR_MAX);
    WritePrivateProfileStringPep(WIDE("Transaction Mnemonics"), WIDE("Number"), szNumber, lpszFname);

    ParaAllRead(CLASS_PARATRANSMNEMO, (UCHAR *)szWork, (TRN_ADR_MAX * PARA_TRANSMNEMO_LEN),  0, &usReturnLen);

	for (nCount = 0; nCount < TRN_ADR_MAX; nCount++) {
		WCHAR           szMnemonic[PARA_TRANSMNEMO_LEN + 1] = { 0 };
		WCHAR           szAddress[REPORT_MAXLEN] = { 0 };

        wsPepf(szAddress, WIDE("Address%d"), nCount + 1);
        memcpy(szMnemonic, szWork[nCount], PARA_TRANSMNEMO_LEN * sizeof(WCHAR));
        WritePrivateProfileStringPep(WIDE("Transaction Mnemonics"), szAddress, szMnemonic, lpszFname);
    }
}

/****************************************************************\
|*                                                              *|
|*  Synopsis:   void ReportSetReportMnemo(LPCSTR lpszFname)     *|
|*                                                              *|
|*               LPCSTR  lpszFname - selected file name         *|
|*                                                              *|
|* Description: This function set the report mnemonics          *|
|*              to PRAMPEP.INI.                                 *|
\****************************************************************/
void ReportSetReportMnemo(LPCWSTR lpszFname)
{
    USHORT          usReturnLen;
    WCHAR           szWork[RPT_ADR_MAX][PARA_REPORTNAME_LEN] = { 0 };
    WCHAR           szNumber[REPORT_MAXLEN] = { 0 };
    short           nCount;

    wsPepf(szNumber, WIDE("%d"), RPT_ADR_MAX);
    WritePrivateProfileStringPep(WIDE("Report Mnemonics"), WIDE("Number"), szNumber, lpszFname);

    ParaAllRead(CLASS_PARAREPORTNAME, (UCHAR *)szWork, (RPT_ADR_MAX * PARA_REPORTNAME_LEN), 0, &usReturnLen);

    for (nCount = 0; nCount < RPT_ADR_MAX; nCount++) {
		WCHAR           szMnemonic[PARA_REPORTNAME_LEN + 1] = { 0 };
		WCHAR           szAddress[REPORT_MAXLEN] = { 0 };

        wsPepf(szAddress, WIDE("Address%d"), nCount + 1);
        memcpy(szMnemonic, szWork[nCount], PARA_REPORTNAME_LEN * sizeof(WCHAR));
        WritePrivateProfileStringPep(WIDE("Report Mnemonics"), szAddress, szMnemonic, lpszFname);
    }
}

/****************************************************************\
|*                                                              *|
|*  Synopsis:   void ReportSetSpecialMnemo(LPCSTR lpszFname)    *|
|*                                                              *|
|*               LPCSTR  lpszFname - selected file name         *|
|*                                                              *|
|* Description: This function set the special mnemonics         *|
|*              to PRAMPEP.INI.                                 *|
\****************************************************************/
void ReportSetSpecialMnemo(LPCWSTR lpszFname)
{
    USHORT          usReturnLen;
    WCHAR           szWork[SPC_ADR_MAX][PARA_SPEMNEMO_LEN] = { 0 };
    WCHAR           szNumber[REPORT_MAXLEN] = { 0 };
    short           nCount;

    wsPepf(szNumber, WIDE("%d"), SPC_ADR_MAX);
    WritePrivateProfileStringPep(WIDE("Special Mnemonics"), WIDE("Number"), szNumber, lpszFname);

    ParaAllRead(CLASS_PARASPECIALMNEMO, (UCHAR *)szWork, (SPC_ADR_MAX * PARA_SPEMNEMO_LEN), 0, &usReturnLen);

    for (nCount = 0; nCount < SPC_ADR_MAX; nCount++) {
		WCHAR           szMnemonic[PARA_SPEMNEMO_LEN + 1] = { 0 };
		WCHAR           szAddress[REPORT_MAXLEN] = { 0 };

        wsPepf(szAddress, WIDE("Address%d"), nCount + 1);
        memcpy(szMnemonic, szWork[nCount], PARA_SPEMNEMO_LEN * sizeof(WCHAR));
        WritePrivateProfileStringPep(WIDE("Special Mnemonics"), szAddress, szMnemonic, lpszFname);
    }
}

/****************************************************************\
|*                                                              *|
|*  Synopsis:   void ReportSetAdjectiveMnemo(LPCSTR lpszFname)  *|
|*                                                              *|
|*               LPCSTR  lpszFname - selected file name         *|
|*                                                              *|
|* Description: This function set the adjective mnemonics       *|
|*              to PRAMPEP.INI.                                 *|
\****************************************************************/
void ReportSetAdjectiveMnemo(LPCWSTR lpszFname)
{
    USHORT          usReturnLen;
	WCHAR           szWork[ADJ_ADR_MAX][PARA_ADJMNEMO_LEN] = { 0 };
	WCHAR           szNumber[REPORT_MAXLEN] = { 0 };
    short           nCount;

    wsPepf(szNumber, WIDE("%d"), ADJ_ADR_MAX);
    WritePrivateProfileStringPep(WIDE("Adjective Mnemonics"), WIDE("Number"), szNumber, lpszFname);

    ParaAllRead(CLASS_PARAADJMNEMO, (UCHAR *)szWork, (ADJ_ADR_MAX * PARA_ADJMNEMO_LEN), 0, &usReturnLen);

    for (nCount = 0; nCount < ADJ_ADR_MAX; nCount++) {
		WCHAR           szMnemonic[PARA_ADJMNEMO_LEN + 1] = { 0 };
		WCHAR           szAddress[REPORT_MAXLEN] = { 0 };

        wsPepf(szAddress, WIDE("Address%d"), nCount + 1);
        memcpy(szMnemonic, szWork[nCount], PARA_ADJMNEMO_LEN * sizeof(WCHAR));
        WritePrivateProfileStringPep(WIDE("Adjective Mnemonics"), szAddress, szMnemonic, lpszFname);
    }
}

/****************************************************************\
|*                                                              *|
|*  Synopsis:   void ReportSetMajorDeptMnemo(LPCSTR lpszFname)  *|
|*                                                              *|
|*               LPCSTR  lpszFname - selected file name         *|
|*                                                              *|
|* Description: This function set the major department mnemonics*|
|*              to PRAMPEP.INI.                                 *|
\****************************************************************/
void ReportSetMajorDeptMnemo(LPCWSTR lpszFname)
{
    USHORT          usReturnLen;
    WCHAR           szWork[MDPT_ADR_MAX][PARA_MAJORDEPT_LEN] = { 0 };
    WCHAR           szNumber[REPORT_MAXLEN] = { 0 };
    short           nCount;

    wsPepf(szNumber, WIDE("%d"), MDPT_ADR_MAX);
    WritePrivateProfileStringPep(WIDE("Major Department Mnemonics"), WIDE("Number"), szNumber, lpszFname);

    ParaAllRead(CLASS_PARAMAJORDEPT, (UCHAR *)szWork, (MDPT_ADR_MAX * PARA_MAJORDEPT_LEN), 0, &usReturnLen);

    for (nCount = 0; nCount < MDPT_ADR_MAX; nCount++) {
		WCHAR           szMnemonic[PARA_MAJORDEPT_LEN+1] = { 0 };
		WCHAR           szAddress[REPORT_MAXLEN] = { 0 };

		wsPepf(szAddress, WIDE("Address%d"), nCount + 1);
		memcpy(szMnemonic, szWork[nCount], PARA_MAJORDEPT_LEN * sizeof(WCHAR));
		WritePrivateProfileStringPep(WIDE("Major Department Mnemonics"), szAddress, szMnemonic, lpszFname);
    }
}
/* ===== End of RPTEXEC.C ===== */
