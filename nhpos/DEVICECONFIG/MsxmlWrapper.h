#pragma once

#include "afxconv.h"
#include <afxole.h>         // MFC OLE classes

// this is required to build the wrappers for the msxml3 dll
// notice MSXML3 lives in the MSXML2 namespace as MSXML2 is obsolete
// and Microsoft is providing backwards compatibility to MSXML2.
//
// See also the following postings on StackOverflow concerning MSXML and MSXML versions.
//    https://stackoverflow.com/questions/1075891/what-became-of-msxml-4-0
//    https://stackoverflow.com/questions/951804/which-version-of-msxml-should-i-use
//
#import <msxml3.dll>


class CMsxmlWrapper
{
public:
	CMsxmlWrapper(void);
	virtual ~CMsxmlWrapper(void);

	BOOL  LoadText (LPCTSTR  tcsXml);
	BOOL  LoadText (char *csXml);
	BOOL  SelectNode (char *sNodeString);
	BOOL  SelectNode (LPCTSTR sNodeString);
	BOOL  Gettext (char *sTextString, UINT nCharLen);
	BOOL  Gettext (wchar_t *sTextString, UINT nCharLen);
	BOOL  SelectNodeGettext (char *sNodeString, char *sTextString, UINT nCharLen);
	BOOL  SelectNodeGettext (LPCTSTR sNodeString, wchar_t *sTextString, UINT nCharLen);

	MSXML2::IXMLDOMDocumentPtr *pDOMDoc;
	MSXML2::IXMLDOMNodePtr      pNode;

};
