/*
* ---------------------------------------------------------------------------
* Title         :   Public Header File of Report Module
* Category      :   REPORT, NCR 2170 PEP for Windows (HP US Model)
* Program Name  :   FILE.H
* Copyright (C) :   1996, NCR Corp. E&M OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract : To use the functions of the Report for PEP, included
*           this header file. This header declares Structure Type
*           Define Declarations, Define Declarations and Function
*           Prototype Declarations to use the functions of the File
*           for PEP.
* ---------------------------------------------------------------------------
* Update Histories
*
*   Date    : Ver.Rev. :   Mame     : Description
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
#ifndef REPORT_INCLUDED_DEFINED
#define REPORT_INCLUDED_DEFINED


/************************\
|*  Define Declarations *|
\************************/
/* Child Window ID  */
#define IDD_REPORT                100
#define IDD_REPORT_CURRENT        101
#define IDD_REPORT_CURRPT1        102
#define IDD_REPORT_CURRPT2        103
#define IDD_REPORT_CURRPT3        104
#define IDD_REPORT_CURRPT4        105
#define IDD_REPORT_CURRPT5        106
#define IDD_REPORT_CURRPT6        107

#define IDD_REPORT_CAPTION1		108

/* string resource ID */
#define IDS_REPORT                24064
#define IDS_REPORT_ERR_EXEC       (IDS_REPORT + 1)
#define IDS_REPORT_ERR_LOAD       (IDS_REPORT + 2)
#define IDS_REPORT_ERR_PEP        (IDS_REPORT + 3)
#define IDS_REPORT_ERR_FLEX_EMPTY (IDS_REPORT + 4)
#define IDS_REPORT_ERR_NAME       (IDS_REPORT + 5)
#define IDS_REPORT_ERR_PATH		(IDS_REPORT + 6)

#define IDS_REPORT_FILTER       (IDS_REPORT + 10)
#define IDS_REPORT_FILE         (IDS_REPORT + 11)
#define IDS_REPORT_CAP_FILE     (IDS_REPORT + 12)
#define IDS_REPORT_CAP_EXEC     (IDS_REPORT + 13)
#define IDS_REPORT_EXT          (IDS_REPORT + 14)

/************************************\
|*  Function Prototype Declarations *|
\************************************/
BOOL    ReportFile(HANDLE, HWND);
BOOL    ReportExec(HANDLE, HWND);
BOOL    CurrentReportFile(HANDLE, HWND);

#endif
/* ===== End of File ===== */
