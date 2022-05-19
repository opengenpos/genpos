// MyDropTarget.cpp : implementation file
//

#include "stdafx.h"
#include "MyDropTarget.h"


// CMyDropTarget

IMPLEMENT_DYNAMIC(CMyDropTarget, COleDropTarget)


CMyDropTarget::CMyDropTarget(CWnd* pWnd) : pMyDropDialog(0)
{
	if ( SUCCEEDED( CoCreateInstance ( CLSID_DragDropHelper, NULL, CLSCTX_INPROC_SERVER, IID_IDropTargetHelper, (void**) &m_piDropHelper ) )) {
 		if (pWnd) {
			Register(pWnd);
		}
	} else {
		m_piDropHelper = 0;
	}
}


CMyDropTarget::~CMyDropTarget()
{
	if ( NULL != m_piDropHelper )
		m_piDropHelper->Release();
}


BEGIN_MESSAGE_MAP(CMyDropTarget, COleDropTarget)
END_MESSAGE_MAP()



// CMyDropTarget message handlers
DROPEFFECT CMyDropTarget::OnDragEnter ( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point )
{
		return DROPEFFECT_COPY;
	// Get the HDROP data from the data object.
	if (pDataObject->GetGlobalData ( CF_HDROP ))
		return DROPEFFECT_COPY;
	else
		return DROPEFFECT_NONE;
}

DROPEFFECT CMyDropTarget::OnDragOver ( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point )
{
		return DROPEFFECT_COPY;
	// Get the HDROP data from the data object.
	if (pDataObject->GetGlobalData ( CF_HDROP ))
		return DROPEFFECT_COPY;
	else
		return DROPEFFECT_NONE;
}

BOOL CMyDropTarget::OnDrop ( CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point )
{

	STGMEDIUM  StgMediumText;
	BOOL  bTextCheck = pDataObject->GetData (CF_TEXT, &StgMediumText);
	if (bTextCheck) {
		// We need to lock the HGLOBAL handle because we can't
        // be sure if this is GMEM_FIXED (i.e. normal heap) data or not
        char *data = (char *)GlobalLock(StgMediumText.hGlobal);
 
		pMyDropDialog->SendMessage (WM_APP_DROP_STRING, (WPARAM)data, 0);

        // cleanup
        GlobalUnlock(StgMediumText.hGlobal);
        ReleaseStgMedium(&StgMediumText);
		return TRUE;
	}

	STGMEDIUM  StgMediumFile;
	BOOL  bFileCheck = pDataObject->GetData (CF_HDROP, &StgMediumFile);
	if (bFileCheck) {
		// This is a file drop OLE event so lets process it.
		int    iLen=255;
		int    iLoop = 0;
		HDROP   hdrop = (HDROP) GlobalLock ( StgMediumFile.hGlobal );

		if (hdrop ) {
			// Send the file drop notification to the dialog to be processed.
			// We use SendMessage() so that we process and then unlock it.
			pMyDropDialog->SendMessage (WM_DROPFILES, (WPARAM)hdrop, 0);
		}
		GlobalUnlock ( StgMediumFile.hGlobal );
		return TRUE;
	}

	return FALSE;
}

void CMyDropTarget::OnDragLeave ( CWnd* pWnd )
{
}
