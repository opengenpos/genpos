#if !defined(AFX_WINDOWCONTROLSAMPLE_H__9C9AD739_458A_4DD5_B392_11A99918F588__INCLUDED_)
#define AFX_WINDOWCONTROLSAMPLE_H__9C9AD739_458A_4DD5_B392_11A99918F588__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WindowControlSample.h : header file
//

#include "WindowControl.h"

/////////////////////////////////////////////////////////////////////////////
// CWindowControlSample window
/*

	CWindowControlSample provides the basics for generating objects at run time.

	its primary purpose is to provide a mechanism for loading objects from
	persistant store using the SerializeNew method of this class.

 */

#include "WindowText.h"
#include "WindowButton.h"
#include "WindowItem.h"
#include "WindowGroup.h"
#include "WindowLabel.h"

class CWindowControlSample : public CWindowControl
{
public:
	CWindowControlSample();
	virtual ~CWindowControlSample();
	virtual BOOL WindowCreate (CWindowControl *pParentWnd, UINT nID = 0, int scale = 100) { return FALSE; }
	
	virtual BOOL WindowDisplay (CDC* pDC) {return FALSE;}
	virtual BOOL WindowDestroy (void) {PostMessage(WM_QUIT, 0, 0); return FALSE;}
	// Every control should provide its own Control Serialize mechanism
	virtual void ControlSerialize (CArchive &ar) { ASSERT (0);}
	
	CWindowControl * SerializeNew (CArchive &ar) {
		CWindowControl *wcReturn = NULL;
		this->Serialize (ar);
		switch (controlAttributes.m_nType) {
			case TextControl:
				wcReturn = new CWindowText (this);
				wcReturn->ControlSerialize (ar);
				break;
			case ButtonControl:
				wcReturn = new CWindowButton (this);
				wcReturn->ControlSerialize (ar);
				break;
			case WindowContainer:
				wcReturn = new CWindowItem (this);
				wcReturn->ControlSerialize (ar);
				break;
			case LabelControl:
				wcReturn = new CWindowLabel (this);
				wcReturn->ControlSerialize (ar);
				break;
			default:
				ASSERT (0);
				AfxThrowArchiveException(CArchiveException::badClass, ar.m_strFileName);
				break;
		}
		return wcReturn;
	}
	//{{AFX_MSG(CTesterWindowControlSample)
	// NOTE - the ClassWizard will add and remove member functions here.
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINDOWCONTROLSAMPLE_H__9C9AD739_458A_4DD5_B392_11A99918F588__INCLUDED_)
