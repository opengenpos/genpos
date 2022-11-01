// NewLayoutDoc.h : interface of the CNewLayoutDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEWLAYOUTDOC_H__494EE630_9CB6_40C1_BF7B_79081C4A5969__INCLUDED_)
#define AFX_NEWLAYOUTDOC_H__494EE630_9CB6_40C1_BF7B_79081C4A5969__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WindowDocumentExt.h"
#include "DefaultEdit.h"
//#include "DefaultDataExt.h"
#include "DefaultData.h"
#include "WindowControl.h"
#include "WindowButtonExt.h"


class CNewLayoutDoc : public CDocument
{
protected: // create from serialization only
	CNewLayoutDoc();
	DECLARE_DYNCREATE(CNewLayoutDoc)

// Attributes
public:
	CWindowDocumentExt myWindowDocument;
	CStdioFile * myExportFile;


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewLayoutDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void OnFileExport();
	virtual void OnFileImport();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	protected:
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL UpdateButtonAction(CWindowButtonExt *bi);

	CWindowControl* FindImportControl(UINT id);
	BOOL ExpSubControl(CWindowItem *wi);
	BOOL WriteExportData(CWindowControl *wc);
	virtual ~CNewLayoutDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:


// Generated message map functions
protected:
	//{{AFX_MSG(CNewLayoutDoc)
	afx_msg void OnFileSummary();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWLAYOUTDOC_H__494EE630_9CB6_40C1_BF7B_79081C4A5969__INCLUDED_)
