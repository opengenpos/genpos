#include "StdAfx.h"
#include "MsxmlWrapper.h"

CMsxmlWrapper::CMsxmlWrapper(void)
{
	pDOMDoc = new MSXML2::IXMLDOMDocumentPtr (__uuidof(MSXML2::DOMDocument));
}

CMsxmlWrapper::~CMsxmlWrapper(void)
{
}

BOOL CMsxmlWrapper::LoadText (LPCTSTR tcsXml)
{
	try {
		VARIANT_BOOL varResult = (*pDOMDoc)->loadXML(tcsXml);
	}
	catch (_com_error e) {
		TRACE(_T("Caught Exception: CResponseDlg::InterpretResponse"));
		AfxMessageBox(e.ErrorMessage());
		return FALSE;
	}

	return TRUE;
}

BOOL CMsxmlWrapper::LoadText (char * tcsXml)
{
	try {
		VARIANT_BOOL varResult = (*pDOMDoc)->loadXML(tcsXml);
	}
	catch (_com_error e) {
		TRACE(_T("Caught Exception: CResponseDlg::InterpretResponse"));
		AfxMessageBox(e.ErrorMessage());
		return FALSE;
	}

	return TRUE;
}
BOOL CMsxmlWrapper::SelectNode (char *sNodeString)
{
	try {
		pNode = (*pDOMDoc)->selectSingleNode(sNodeString);
	}
	catch (_com_error e) {
		pNode = 0;
	}

	return (pNode) ? TRUE : FALSE;
}

BOOL CMsxmlWrapper::SelectNode (LPCTSTR sNodeString)
{
	try {
		pNode = (*pDOMDoc)->selectSingleNode(sNodeString);
	}
	catch (_com_error e) {
		pNode = 0;
	}

	return (pNode) ? TRUE : FALSE;
}

BOOL  CMsxmlWrapper::Gettext (char *sTextString, UINT nCharLen)
{
	BOOL bRetStatus = FALSE;

	if (sTextString && nCharLen > 0) {
		*sTextString = 0;
		if (pNode) {
			try {
				strncpy (sTextString, pNode->Gettext(), nCharLen);
				bRetStatus = TRUE;
			}
			catch (_com_error e) {
			}
		}
	}

	return bRetStatus;
}

BOOL CMsxmlWrapper::Gettext (wchar_t *sTextString, UINT nCharLen)
{
	BOOL bRetStatus = FALSE;

	if (sTextString && nCharLen > 0) {
		*sTextString = 0;
		if (pNode) {
			try {
				wcsncpy (sTextString, pNode->Gettext(), nCharLen);
				bRetStatus = TRUE;
			}
			catch (_com_error e) {
			}
		}
	}

	return bRetStatus;
}

BOOL CMsxmlWrapper::SelectNodeGettext (char *sNodeString, char *sTextString, UINT nCharLen)
{
	try {
		pNode = (*pDOMDoc)->selectSingleNode(sNodeString);
		Gettext(sTextString, nCharLen);
	}
	catch (_com_error e) {
		pNode = 0;
	}

	return (pNode) ? TRUE : FALSE;
}

BOOL CMsxmlWrapper::SelectNodeGettext (LPCTSTR sNodeString, wchar_t *sTextString, UINT nCharLen)
{
	try {
		pNode = (*pDOMDoc)->selectSingleNode(sNodeString);
		Gettext (sTextString, nCharLen);
	}
	catch (_com_error e) {
		pNode = 0;
	}

	return (pNode) ? TRUE : FALSE;
}

