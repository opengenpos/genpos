// DActiveWndEdit.cpp : implementation file
//

#include "stdafx.h"
#include "newlayout.h"
#include "DActiveWndEdit.h"

#include "DWindowGroup.h"
#include "WindowGroup.h"
#include "NewLayoutView.h"
#include "WindowDocumentExt.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDActiveWndEdit dialog


CDActiveWndEdit::CDActiveWndEdit(CWindowGroup * wg, CWnd* pParent /*=NULL*/)
	: CDialog(CDActiveWndEdit::IDD, pParent)
{
	//get the layout view for document access
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CChildFrame *pChild = (CChildFrame *) pFrame->GetActiveFrame();
	myView = pChild->GetRightPane();

	if (wg){
		//get the document from the LayoutView so we can access its Group list
		CWindowDocumentExt* pDoc = myView->GetDocument();

		//get the number of groups in the documetns group list
		int nCount = pDoc->listGroups.GetCount();
		//loop through list until we find the selected group
		for (int i=0;i < nCount;i++)
		{
			//get each window group in the list one at a time
			//wg = pDoc->listGroups.GetAt(pDoc->listGroups.FindIndex(i));
			//is this the group we are looking for (is its group ID = data of user selection
			if(pDoc->listGroups.GetAt(pDoc->listGroups.FindIndex(i))->groupAttributes.GroupID == wg->groupAttributes.GroupID){
				//exit for loop with the correct combo box index saved
				m_nSelGrp = i;
				break;
			}
		}
	}
	else{
		//{{AFX_DATA_INIT(CDActiveWndEdit)
			// NOTE: the ClassWizard will add member initialization here
		//}}AFX_DATA_INIT
		m_nSelGrp = 0;
	}
}


void CDActiveWndEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDActiveWndEdit)
	DDX_Control(pDX, IDC_BUTTON_ACTIVATE, m_btnActivate);
	DDX_Control(pDX, IDC_COMBO_GRPLIST, m_cbGroups);
	DDX_Control(pDX, IDC_LIST_GRPWND, m_lbGroupWindows);
	//}}AFX_DATA_MAP
	if(pDX->m_bSaveAndValidate){

	}
	else{
		//get the document from the LayoutView so we can access its Group list
		CWindowDocumentExt* pDoc = myView->GetDocument();
		
		//get the number of groups in the documents list
		int nCount = pDoc->listGroups.GetCount();
		/*loop through documents group list and gets its attributes, use them to format
		CString for label and add the string to the combo box. We will also assign the 
		Groups ID as the data for each item in the list box*/
		for (int i=0;i < nCount;i++)
		{
			CWindowGroup *wg = pDoc->listGroups.GetAt(pDoc->listGroups.FindIndex(i));
			m_cbGroups.InsertString(i,wg->GroupName);
			m_cbGroups.SetItemData(i,wg->groupAttributes.GroupID);
		}

		//set combo box to 1st item in list
		m_cbGroups.SetCurSel (m_nSelGrp);
		//call function to initialize the list box with the groups windows
		OnSelchangeComboGrplist();
	}
}


BEGIN_MESSAGE_MAP(CDActiveWndEdit, CDialog)
	//{{AFX_MSG_MAP(CDActiveWndEdit)
	ON_BN_CLICKED(IDC_BUTTON_ACTIVATE, OnButtonActivate)
	ON_CBN_SELCHANGE(IDC_COMBO_GRPLIST, OnSelchangeComboGrplist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDActiveWndEdit message handlers

void CDActiveWndEdit::OnButtonActivate() 
{
	// TODO: Add your control notification handler code here
	int m_nSelWnd = m_lbGroupWindows.GetCurSel();
	//exit if the btn was pressed while no window was selected
	if(m_nSelWnd == -1){
		return;
	}

	//get the document from the LayoutView so we can access its Group list
	CWindowDocumentExt* pDoc = myView->GetDocument();

	//get the number of groups in the documents list
	int nCount = pDoc->listGroups.GetCount();

	//get the currentlly selected WindowGroup
	CWindowGroup * wg = pDoc->listGroups.GetAt(pDoc->listGroups.FindIndex(m_nSelGrp));

	//set the groups active window to the selected window
	wg->groupAttributes.nActiveWnd = m_lbGroupWindows.GetItemData(m_nSelWnd);
	//redisplay updated data
	OnSelchangeComboGrplist();
	
}

void CDActiveWndEdit::OnSelchangeComboGrplist() 
{
	// TODO: Add your control notification handler code here
	CWindowDocumentExt* pDoc = myView->GetDocument();

	//reset the contents of the list box
	m_lbGroupWindows.ResetContent();

	//get the currentlly selected WindowGroup
	m_nSelGrp = m_cbGroups.GetCurSel();
	CWindowGroup * wg = pDoc->listGroups.GetAt(pDoc->listGroups.FindIndex(m_nSelGrp));

	//instantiate a string for formatting combo box labels
	CString str = _T("");
	//used for index of list box - increments each time a new window item is added
	int cnt = 0;

	//cycle throught the documents control list and add all windows that belong to the selected group to the list box
	CWindowControl *pListControl;
	POSITION currentPos = pDoc->listControls.GetHeadPosition ();
	while (currentPos) {
		pListControl = pDoc->listControls.GetNext (currentPos);

		//is the control a window item
		if (pListControl->controlAttributes.m_nType == CWindowControl::WindowContainer ) {
			CWindowItemExt* wi = CWindowItemExt::TranslateCWindowControl(pListControl);

			//does the window item belong to the selected group
			if(wi->windowAttributes.GroupNum == wg->groupAttributes.GroupID){

				//if this is current active window in group, display ACTIVE with window name
				if(wg->groupAttributes.nActiveWnd == wi->controlAttributes.m_myId){
			
					CString csActiveLbl;
					csActiveLbl.LoadString(IDS_GRPWND_AVTIVE);

					str.Format(_T("%-d\t%-s\t%s"),
						       wi->controlAttributes.m_myId,
							   wi->controlAttributes.m_myName,						
							   csActiveLbl);
				
					m_lbGroupWindows.AddString(str);
					m_lbGroupWindows.SetItemData(cnt,wi->controlAttributes.m_myId);
				}
				//display window name only in list box
				else{
					str.Format(_T("%-d\t%-s"),
							   wi->controlAttributes.m_myId,
							   wi->controlAttributes.m_myName);

					m_lbGroupWindows.AddString(str);
					m_lbGroupWindows.SetItemData(cnt,wi->controlAttributes.m_myId);
				}
				//increment index counter
				cnt++;
			}
		}
	}
	
}


