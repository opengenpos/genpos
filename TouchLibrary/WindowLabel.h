// WindowLabel.h: interface for the CWindowLabel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINDOWLABEL_H__BD769099_7B8E_443B_8027_7B4158F9A895__INCLUDED_)
#define AFX_WINDOWLABEL_H__BD769099_7B8E_443B_8027_7B4158F9A895__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include "WindowControl.h"



class CWindowLabel : public CWindowControl  
{
public:
	typedef enum {CaptionAlignmentHLeft = 0, CaptionAlignmentHCenter, CaptionAlignmentHRight, CaptionAlignmentVTop, CaptionAlignmentVMiddle, CaptionAlignmentVBottom} CaptionAlignment;
	typedef enum {IconAlignmentHLeft = 0, IconAlignmentHCenter, IconAlignmentHRight} HorizontalIconAlignment;
	typedef enum {IconAlignmentVTop = 0, IconAlignmentVMiddle, IconAlignmentVBottom} VerticalIconAlignment;

	/*Structure containing all the attributes to be serialized for the WindowLabel class
	any new attributes need to be added at the end of the struct so data from 
	old layout files does not get corrupted during serialization*/
	typedef struct _tagWinLblAttributes{
		unsigned long signatureStart;
		TCHAR myIcon[30];								//Label icon file nale
		CaptionAlignment capAlignment;					//alignment of label caption
		HorizontalIconAlignment horIconAlignment;	    //horizontal alignment of label icon	
		VerticalIconAlignment vertIconAlignment;        //vertical alignment of label icon
		BOOL HorizontalOriented;						//Caption Orientation - TRUE for horizontal, FALSE for Vertical
		BOOL Show;
		BOOL PickCounter;                               // indicates if label is a pick counter, TRUE, or not, FALSE
		TCHAR  tcsLabelText[256];                       // contains the text for a label.
	}WinLblAttributes;

	CWindowLabel(CWindowControl *wc = 0);
	CWindowLabel (int id, int row, int column, int width = 3, int height = 3);
	virtual ~CWindowLabel();

	virtual BOOL WindowDisplay (CDC* pDC) {ASSERT(0); return FALSE; }
	virtual BOOL WindowDestroy (void) { PostMessage (WM_QUIT, 0, 0); return TRUE;}

	virtual void Serialize( CArchive& ar );
	virtual void ControlSerialize (CArchive &ar);
	virtual BOOL WindowCreate (CWindowControl *pParentWnd, UINT nID = 0, int scale = 100);

	WinLblAttributes labelAttributes;
	CString m_IconName;

	int nEndOfArchive;

	static CWindowLabel * TranslateCWindowControl (CWindowControl *pwc) {
		if (pwc && pwc->controlAttributes.m_nType == LabelControl)
			return (static_cast <CWindowLabel *> (pwc));
		else
			return NULL;
	}
private:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

};

#endif // !defined(AFX_WINDOWLABEL_H__BD769099_7B8E_443B_8027_7B4158F9A895__INCLUDED_)
