// FrameworkWndCtrl.h : header file
//

/*

	CFrameworkWndControl provides the basics for generating objects at run time.

	its primary purpose is to provide a mechanism for loading objects from
	persistant store using the SerializeNew method of this class.
*/
 

#include "FrameworkWndText.h"
#include "FrameworkWndButton.h"
#include "FrameworkWndItem.h"
#include "FrameworkWndLabel.h"
#include "FrameworkWndListBox.h"
#include "FrameworkWndEditBox.h"

#define  ICON_FILES_DIR_FORMAT  _T("\\FlashDisk\\NCR\\Saratoga\\Icons\\%s")

class CFrameworkWndControl : public CWindowControl
{
public:
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
				wcReturn = new CFrameworkWndText(this);
				wcReturn->ControlSerialize (ar);
				break;
			case ButtonControl:
				wcReturn = new CFrameworkWndButton(this);
				wcReturn->ControlSerialize (ar);
				break;
			case WindowContainer:
				wcReturn = new CFrameworkWndItem(this);
				wcReturn->ControlSerialize (ar);
				break;
			case LabelControl:
				wcReturn = new CFrameworkWndLabel(this);
				wcReturn->ControlSerialize (ar);
				break;
			case ListBoxControl:
				wcReturn = new CFrameworkWndListBox(this);
				wcReturn->ControlSerialize (ar);
				break;
			case EditBoxControl:
				wcReturn = new CFrameworkWndEditBox(this);
				wcReturn->ControlSerialize (ar);
				break;
			default:
				ASSERT (0);
				AfxThrowArchiveException(CArchiveException::badClass, ar.m_strFileName);
				break;
		}
		return wcReturn;
	}
};