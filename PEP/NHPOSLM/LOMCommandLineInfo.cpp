// LOMCommandLineInfo.cpp: implementation of the CLOMCommandLineInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "newlayout.h"
#include "LOMCommandLineInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*overide ParsParam function from CCommandLineInfo to handle language flag-
if not language flag - pass to base class function*/
void CLOMCommandLineInfo::ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast)
{
	
	CString csLang;
	CString csSublang;
	if(bFlag) {
		CString sParam(lpszParam);
		if (sParam.Left(3) == "lg:") {
			csLang.Format(_T("%s"),sParam.Mid(3,2));
			csSublang.Format(_T("%s"),sParam.Right(sParam.GetLength() - 5));
			swscanf(csLang,_T("%hX"),&lang);
			swscanf(csSublang,_T("%hX"), &sublang);
			ParseLast(bLast);		//called to set m_nShellCommand member to FileOpen if called from PEP with a file name to open
			return;
		}

	}

	// Call the base class to ensure proper command line processing
	CCommandLineInfo::ParseParam(lpszParam, bFlag, bLast);
}


