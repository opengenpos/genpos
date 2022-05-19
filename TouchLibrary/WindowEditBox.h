// WindowLabel.h: interface for the CWindowEditBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINDOWEDITBOX_H__BD769099_7B8E_443B_8027_7B4158F9A895__INCLUDED_)
#define AFX_WINDOWEDITBOX_H__BD769099_7B8E_443B_8027_7B4158F9A895__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include "WindowControl.h"



class CWindowEditBox : public CWindowControl  
{
public:
	typedef enum {CaptionAlignmentHLeft = 0, CaptionAlignmentHCenter, CaptionAlignmentHRight, CaptionAlignmentVTop, CaptionAlignmentVMiddle, CaptionAlignmentVBottom} CaptionAlignment;
	typedef enum {IconAlignmentHLeft = 0, IconAlignmentHCenter, IconAlignmentHRight} HorizontalIconAlignment;
	typedef enum {IconAlignmentVTop = 0, IconAlignmentVMiddle, IconAlignmentVBottom} VerticalIconAlignment;

	/*Structure containing all the attributes to be serialized for the WindowEditBox class
	any new attributes need to be added at the end of the struct so data from 
	old layout files does not get corrupted during serialization*/
	typedef struct _tagWinEbAttributes{
		unsigned long signatureStart;
		CaptionAlignment capAlignment;					//alignment of label caption
		HorizontalIconAlignment horIconAlignment;	    //horizontal alignment of label icon	
		VerticalIconAlignment vertIconAlignment;        //vertical alignment of label icon
		BOOL HorizontalOriented;						//Caption Orientation - TRUE for horizontal, FALSE for Vertical
		BOOL Show;
		TCHAR  tcsLabelText[256];                       // contains the text for a label.
	}WinEbAttributes;

	CWindowEditBox(CWindowControl *wc = 0);
	CWindowEditBox (int id, int row, int column, int width = 3, int height = 3);
	virtual ~CWindowEditBox();

	virtual BOOL WindowDisplay (CDC* pDC) {ASSERT(0); return FALSE; }
	virtual BOOL WindowDestroy (void) { PostMessage (WM_QUIT, 0, 0); return TRUE;}

	virtual void Serialize( CArchive& ar );
	virtual void ControlSerialize (CArchive &ar);
	virtual BOOL WindowCreate (CWindowControl *pParentWnd, UINT nID = 0, int scale = 100);

	WinEbAttributes editboxAttributes;
	CString m_IconName;

	int nEndOfArchive;

	static CWindowEditBox * TranslateCWindowControl (CWindowControl *pwc) {
		if (pwc && pwc->controlAttributes.m_nType == EditBoxControl)
			return (static_cast <CWindowEditBox *> (pwc));
		else
			return NULL;
	}
private:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

};

#endif // !defined(AFX_WINDOWEDITBOX_H__BD769099_7B8E_443B_8027_7B4158F9A895__INCLUDED_)
