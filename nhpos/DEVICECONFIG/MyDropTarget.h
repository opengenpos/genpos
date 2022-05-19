
#pragma once

#define WM_APP_DROP_STRING    (WM_APP + 1)  // Message indicating dropping a string which may be a URL.

// CMyDropTarget command target

class CMyDropTarget : public COleDropTarget
{
	DECLARE_DYNAMIC(CMyDropTarget)

public:
	CMyDropTarget(CWnd* pWnd = 0);
	virtual ~CMyDropTarget();

	DROPEFFECT OnDragEnter ( CWnd* pWnd, COleDataObject* pDataObject,
                             DWORD dwKeyState, CPoint point );

    DROPEFFECT OnDragOver ( CWnd* pWnd, COleDataObject* pDataObject,
                            DWORD dwKeyState, CPoint point );

    BOOL OnDrop ( CWnd* pWnd, COleDataObject* pDataObject,
                  DROPEFFECT dropEffect, CPoint point );

    void OnDragLeave ( CWnd* pWnd );

	CDialog  *pMyDropDialog;

protected:
	IDropTargetHelper   *m_piDropHelper;
	DECLARE_MESSAGE_MAP()
};


