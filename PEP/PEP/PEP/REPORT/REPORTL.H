/*
* ---------------------------------------------------------------------------
* Title         :   Local Header File of Report Module
* Category      :   REPORT, NCR 2170 PEP for Windows (HP US Model)
* Program Name  :   REPORTL.H
* Copyright (C) :   1996, NCR Corp. E&M OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract : The only Report for PEP included this header file. This header
*           declares Structure Type Define Declarations, Define Declarations
*           and Function Prototype Declarations to use the functions of the
*           File for PEP.
* ---------------------------------------------------------------------------
* Update Histories
*
*   Date    : Ver.Rev. :   Mame     : Description
* Feb-05-96 : 03.01.00 : M.Suzuki   : Initial (HP US R3.1)
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

/************************\
|*  Define Declarations *|
\************************/
#define REPORT_MAXLEN_FILENAME    13
#define REPORT_MAXLEN_TEXT        128
#define REPORT_MAXLEN             256

/* ----- dialog idintifiers ----- */
#define REPORT_INIT_PROC          0
#define REPORT_FILE_PROC          1

/* ----- offset of a drive name ----- */
#define REPORT_DRVNAME_OFS        2

/* */

/************************************\
|*  Function Prototype Declarations *|
\************************************/
/* Dialog Procedure */
BOOL WINAPI ReportFileDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL WINAPI  CurrentRptFileDlgProc(HWND, UINT, WPARAM, LPARAM);

void    ReportSetParameter(LPCWSTR);
void    ReportSetTransMnemo(LPCWSTR);
void    ReportSetReportMnemo(LPCWSTR);
void    ReportSetSpecialMnemo(LPCWSTR);
void    ReportSetAdjectiveMnemo(LPCWSTR);
void    ReportSetMajorDeptMnemo(LPCWSTR);

/************************************\
|*  External Work Area Declarations *|
\************************************/

extern  WCHAR        szReportCaption[REPORT_MAXLEN_TEXT];
extern  WCHAR        szReportWild[];

/* ===== End of FILEL.H ===== */
