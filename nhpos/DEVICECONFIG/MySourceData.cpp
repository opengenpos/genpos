// MySourceData.cpp : implementation file
//

#include "stdafx.h"
#include "MySourceData.h"

//  see http://www.codeproject.com/Articles/12342/Drag-and-Drop-Listbox-Items-using-OLE

// CMySourceData

IMPLEMENT_DYNAMIC(CMySourceData, COleDataSource)


CMySourceData::CMySourceData()
{
}

CMySourceData::~CMySourceData()
{
}


BEGIN_MESSAGE_MAP(CMySourceData, COleDataSource)
END_MESSAGE_MAP()



// CMySourceData message handlers
BOOL CMySourceData::StartDragFile (CString FileName)
{
	HGLOBAL hDrop = GlobalAlloc (GMEM_ZEROINIT|GMEM_MOVEABLE, (sizeof(DROPFILES) + 512) );
	if (hDrop) {
		DROPFILES  *pDropFilesList;

		pDropFilesList = (DROPFILES  *) GlobalLock (hDrop);
		if (pDropFilesList) {
			char *pFileList = (char *)(pDropFilesList + 1);

			sprintf (pFileList, "%S", (LPCTSTR)FileName);
			pDropFilesList->pFiles = sizeof(DROPFILES);
			GlobalUnlock (hDrop);

			CacheGlobalData (CF_HDROP, hDrop);
			// Create IDataObject and IDropSource COM objects

			DROPEFFECT DropEffect = DoDragDrop (DROPEFFECT_COPY);
		}
	}

	return TRUE;
}

BOOL CMySourceData::StartDragText (CString TextString)
{
	HGLOBAL hTextArea = GlobalAlloc (GMEM_ZEROINIT|GMEM_MOVEABLE, 512);
	if (hTextArea) {
		char *pFileList = (char  *) GlobalLock (hTextArea);
		if (pFileList) {

			sprintf (pFileList, "%S", (LPCTSTR)TextString);
			GlobalUnlock (hTextArea);

			CacheGlobalData (CF_TEXT, hTextArea);
			// Create IDataObject and IDropSource COM objects

			DROPEFFECT DropEffect = DoDragDrop (DROPEFFECT_COPY);
		}
	}

	return TRUE;
}

