// DWindowGroup.cpp : implementation file
//

#include "stdafx.h"
#include "NewLayout.h"
#include "DWindowGroup.h"
#include "WindowGroup.h"
#include "NewLayoutView.h"
#include "WindowDocumentExt.h"
#include "WindowControl.h"
#include "ChildFrm.h"
#include "DActiveWndEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDWindowGroup dialog

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////																							  
//DESCRIPTION - Class constructor
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CDWindowGroup::CDWindowGroup(CWindowGroup *wg, CWnd* pParent /*=NULL*/)
	: CDialog(CDWindowGroup::IDD, pParent)
{
	if(wg){
		m_csGroupDesc = wg->GroupDesc;
		m_csGroupName = wg->GroupName;
	}
	else{
		//{{AFX_DATA_INIT(CDWindowGroup)
	m_csGroupDesc = _T("");
	m_csGroupName = _T("");
	//}}AFX_DATA_INIT
	}
	//get the layout view for document access
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CChildFrame *pChild = (CChildFrame *) pFrame->GetActiveFrame();
	myView = pChild->GetRightPane();

}


void CDWindowGroup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDWindowGroup)
	DDX_Control(pDX, IDC_EDIT_BUTTON, m_btnEdit);
	DDX_Control(pDX, IDC_SUBMIT, m_btnSubmit);
	DDX_Control(pDX, IDC_RADIO_EDIT, m_rdoEdit);
	DDX_Control(pDX, IDC_RADIO_ADD, m_rdoSubmit);
	DDX_Control(pDX, IDC_LIST_GROUPS, m_ListBoxGroups);
	DDX_Text(pDX, IDC_EDIT_GRP_DESC, m_csGroupDesc);
	DDX_Text(pDX, IDC_EDIT_GRP_NAME, m_csGroupName);
	//}}AFX_DATA_MAP
	
	if(pDX->m_bSaveAndValidate){

	}
	else{
		//instantiate a string for formatting list box labels
		CString str = _T("");
		//get the document from the LayoutView so we can access its Group list
		CWindowDocumentExt* pDoc = myView->GetDocument();
		
		//get the number of groups in teh documents list
		int nCount = pDoc->listGroups.GetCount();
		/*loop through documents group list and gets its attributes, use them to format
		CString for label and add the string to the list box. We will also assign the 
		Groups ID as the data for each item in the list box*/
		for (int i=0;i < nCount;i++)
		{
			CWindowGroup *wg = pDoc->listGroups.GetAt(pDoc->listGroups.FindIndex(i));
	
			str.Format(_T("%-d\t      %-d\t\t\t%-s"),
					   wg->groupAttributes.GroupID,
					   wg->groupAttributes.nActiveWnd,
					   wg->GroupName);

			m_ListBoxGroups.AddString(str);
			m_ListBoxGroups.SetItemData(i,wg->groupAttributes.GroupID);
		}
		//deselect all groups and enble the add button. We will be creating by default, not editing
		m_ListBoxGroups.SetCurSel (-1);
		OnRadioAdd();
	}

}


BEGIN_MESSAGE_MAP(CDWindowGroup, CDialog)
	//{{AFX_MSG_MAP(CDWindowGroup)
	ON_BN_CLICKED(IDC_DELETE_BUTTON, OnDeleteGroup)
	ON_BN_CLICKED(IDC_EDIT_BUTTON, OnEditGroup)
	ON_BN_CLICKED(IDC_SUBMIT, OnSubmitChange)
	ON_BN_CLICKED(IDC_RADIO_ADD, OnRadioAdd)
	ON_BN_CLICKED(IDC_RADIO_EDIT, OnRadioEdit)
	ON_LBN_SELCHANGE(IDC_LIST_GROUPS, OnGroupListSelect)
	ON_BN_CLICKED(IDC_BUTTON_SETACTWND, OnButtonSetactwnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDWindowGroup message handlers

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////																							   
//FUNCTION CDWindowGroup::OnDeleteGroup()
//RETURN VALUE - void
//PARAMETERS -
//DESCRIPTION - This function is called when the user selects a window group 
//				and then clicks the delete button. It finds the correct Window 
//				Group and deletes it from the documents list
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void CDWindowGroup::OnDeleteGroup() 
{
	// TODO: Add your control notification handler code here
	//get the index of the users selection
	int nSelection = m_ListBoxGroups.GetCurSel();
	//are there no items selected
	if(nSelection < 0 ){
		//alert user and exit function
		AfxMessageBox(IDS_NO_SEL_GRP, MB_OK, 0);
		return;
	}
	//is this the default group - we dont want to delete it
	if(nSelection == 0){
		//alert user and exit function
		AfxMessageBox(IDS_DEL_DEFGRP, MB_OK, 0);
		return;
	}
	//get the document from the LayoutView so we can access its Group list
	CWindowDocumentExt* pDoc = myView->GetDocument();



	//Get the data from the user selection (data = the groups GroupID)
	DWORD data = m_ListBoxGroups.GetItemData(nSelection);

	//cycle through all the documents windows
	POSITION currentPos = pDoc->listControls.GetHeadPosition ();
	POSITION  currentPosLast = currentPos;
	while (currentPos) {
		//get the current window in the list
		CWindowControl *pwc = pDoc->listControls.GetNext (currentPos);
		CWindowItemExt *wi = CWindowItemExt::TranslateCWindowControl(pwc);
		if(!ValidateGroupDelete(wi,data)){
			AfxMessageBox(IDS_GRP_IN_USE, MB_OK, 0);
			return;
		}

	}

	//instantiate a NULL CWindowGroup to be used in our iteraton through the list
	CWindowGroup *wg = NULL;

	//get the number of groups in teh documents list
	int nCount = pDoc->listGroups.GetCount();
	//loop through all the groups in the list until we find the correct one then we delete it
	for (int i=0;i < nCount;i++)
	{
		//get each window group in the list one at a time
		wg = pDoc->listGroups.GetAt(pDoc->listGroups.FindIndex(i));
		//is this the group we are looking for (is its group ID = data of user selection
		if(wg->groupAttributes.GroupID == data){
			//remove the group from the list and exit function
			pDoc->listGroups.RemoveAt(pDoc->listGroups.FindIndex(i));
			ASSERT(pDoc->listGroups.GetCount() == nCount-1);
			break;
		}
	}
	
	//delete all items from the list box
	m_ListBoxGroups.ResetContent();
	//repopulate the list box with the updated list
	UpdateData(FALSE);
	//erase data from the CEdit fields
	((CEdit *)GetDlgItem (IDC_EDIT_GRP_NAME))->SetWindowText(_T(""));
	((CEdit *)GetDlgItem (IDC_EDIT_GRP_DESC))->SetWindowText(_T(""));
	
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////																							   
//FUNCTION CDWindowGroup::OnEditGroup() 
//RETURN VALUE - void
//PARAMETERS -
//DESCRIPTION - This function is called when the user selects a window group 
//				and then clicks the edit button. It finds the correct Window 
//				Group and assigns the edited attributes. This function also 
//				validates against duplicated group names
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void CDWindowGroup::OnEditGroup() 
{
	// TODO: Add your control notification handler code here
	//update controls data
	UpdateData(TRUE);
	//verify that the group name will not be empty
	if(m_csGroupName.IsEmpty()){
		//alert user and exit function
		AfxMessageBox(IDS_EMP_GRPNAME, MB_OK, 0);
		return;
	}
	//get the index of the users selection
	int nSelection = m_ListBoxGroups.GetCurSel();
	//are there no items selected
	if(nSelection < 0 ){
		//alert user and exit function
		AfxMessageBox(IDS_NO_SEL_GRP, MB_OK, 0);
		return;
	}
	//is this the default group - we dont want to edit it
	if(nSelection == 0){
		//alert user and exit function
		AfxMessageBox(IDS_EDIT_DEFGRP, MB_OK, 0);
		return;
	}
	//get the document from the LayoutView so we can access its Group list
	CWindowDocumentExt* pDoc = myView->GetDocument();

	//instantiate a NULL CWindowGroup to be used in our iteraton through the list
	CWindowGroup *wg = NULL;
	//Get the data from the user selection (data = the groups GroupID)
	DWORD data = m_ListBoxGroups.GetItemData(nSelection);
	
	//get the number of groups in teh documents list
	int nCount = pDoc->listGroups.GetCount();
	/*loop through all the groups in the list until we find the correct one 
	We then break from the loop with the correct Group assigned to wg to later edit*/
	int i;	//defined here so that the next loop can check against it
	for (i=0;i < nCount;i++)
	{
		//get each window group in the list one at a time
		wg = pDoc->listGroups.GetAt(pDoc->listGroups.FindIndex(i));
		//is this the group we are looking for (is its group ID = data of user selection
		if(wg->groupAttributes.GroupID == data){
			//exit for loop
			break;
		}
	}
	
	//Instantiate a new window group to use in loop and access its group name
	CWindowGroup *temp = NULL;
	//loop through the doucments group list and validate no duplicate names will be created
	for (int j=0;j < nCount;j++)
	{	
		//get each window group in the list one at a time
		temp = pDoc->listGroups.GetAt(pDoc->listGroups.FindIndex(j));
		/*is this the current window we are editing - we dont want to 
		validate the name because it will show as a duplicated window*/
		if(i==j){
			continue;
		}
		//does the name we want to change to match this window groups name
		if(!m_csGroupName.Compare(temp->GroupName)){
			//alert user and exit function
			AfxMessageBox(IDS_DUP_GRPNAME, MB_OK, 0);
			return;
		}
	}
	//assign edited attributes, and reset all CEdit areas
	
	wg->GroupName = m_csGroupName;
	wg->GroupDesc = m_csGroupDesc;
	m_ListBoxGroups.ResetContent();
	UpdateData(FALSE);
	((CEdit *)GetDlgItem (IDC_EDIT_GRP_NAME))->SetWindowText(_T(""));
	((CEdit *)GetDlgItem (IDC_EDIT_GRP_DESC))->SetWindowText(_T(""));
		
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////																							   
//FUNCTION CDWindowGroup::OnSubmitChange() 
//RETURN VALUE - void
//PARAMETERS -
//DESCRIPTION - This function is called when the user clicks the ad new group 
//				button. It creats a new group, assigns the attributes, adds it
//				to the documents group list. This function also 
//				validates against duplicated group names
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void CDWindowGroup::OnSubmitChange() 
{
	// TODO: Add your control notification handler code here
	//extra validation to ensure the add button and not the edit button was clicked
	if(isNew){
		//update all the controls data
		UpdateData(TRUE);
		//verify that the group name will not be empty
		if(m_csGroupName.IsEmpty()){
			//alert user and exit function
			AfxMessageBox(IDS_EMP_GRPNAME, MB_OK, 0);
			return;
		}
		
		//get the document from the LayoutView so we can access its Group list
		CWindowDocumentExt * pDoc = myView->GetDocument();
		ASSERT_VALID(pDoc);
		
		//Instantiate a new window group to use in loop and access its group name
		CWindowGroup *temp = NULL;
		//get the number of groups in the documents group list
		int nCount = pDoc->listGroups.GetCount();
		//loop through the documents group list and validate no duplicate names will be created
		int j;
		for (j=0;j < nCount;j++)
		{
			//get each window group in the list one at a time
			temp = pDoc->listGroups.GetAt(pDoc->listGroups.FindIndex(j));
			//will the new group name be the same as this groups name
			if(!m_csGroupName.Compare(temp->GroupName)){
				//alert user and exit function
				AfxMessageBox(IDS_DUP_GRPNAME, MB_OK, 0);
				return;
			}
		}
		
		//create the new window group and assign attributes
		CWindowGroup *gg = new CWindowGroup();
		gg->GroupName = m_csGroupName;
		gg->GroupDesc = m_csGroupDesc;
		//assign the group ID and increment the static ID variable
		gg->groupAttributes.GroupID = ++gg->uiGlobalGroupID;
		//add the group to the documents group list
		pDoc->listGroups.AddTail(gg);
		
		//clear the listbox, update control data, and populate listbox with documents new list
		m_ListBoxGroups.ResetContent();
		UpdateData(FALSE);
		//set the selection to the new group
		m_ListBoxGroups.SetCurSel(j);
	}
	
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////																							   
//FUNCTION CDWindowGroup::OnSubmitChange() 
//RETURN VALUE - void
//PARAMETERS -
//DESCRIPTION - This function is called when the user clicks the radio button 
//				for adding a group. Only Add or Edit can be enabled at one time
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void CDWindowGroup::OnRadioAdd() 
{
	// TODO: Add your control notification handler code here
	//set flag and enble and select apprpriate radio buttons and push buttons
	isNew = TRUE;
	m_btnEdit.EnableWindow(!isNew);
	m_btnSubmit.EnableWindow(isNew);
	m_rdoEdit.SetCheck(!isNew);
	m_rdoSubmit.SetCheck(isNew);
	
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////																							   
//FUNCTION CDWindowGroup::OnRadioEdit() 
//RETURN VALUE - void
//PARAMETERS -
//DESCRIPTION - This function is called when the user clicks the radio button 
//				for editing a group. Only Add or Edit can be enabled at one time
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void CDWindowGroup::OnRadioEdit() 
{
	// TODO: Add your control notification handler code here
	//set flag and enble and select apprpriate radio buttons and push buttons
	isNew = FALSE;
	m_btnEdit.EnableWindow(!isNew);
	m_btnSubmit.EnableWindow(isNew);
	m_rdoEdit.SetCheck(!isNew);
	m_rdoSubmit.SetCheck(isNew);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////																							   
//FUNCTION CDWindowGroup::OnGroupListSelect()
//RETURN VALUE - void
//PARAMETERS -
//DESCRIPTION - This function is called when the user selects a group in the 
//				list box control. It updates the properties shown in the CEdit
//				controls and enbles the Edit button
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void CDWindowGroup::OnGroupListSelect() 
{
	// TODO: Add your control notification handler code here
	//get the document from the LayoutView so we can access its Group list
	CWindowDocumentExt* pDoc = myView->GetDocument();
	//call function to enable edit, disable add
	OnRadioEdit();
	//change the radio button selection
	m_rdoEdit.SetCheck(!isNew);
	m_rdoSubmit.SetCheck(isNew);
	
	//Instantiate a new window group to use in loop and access its attributes
	CWindowGroup *wg = NULL;

	//get the index of the user selected group
	int nSelection = m_ListBoxGroups.GetCurSel();
	//get the data of the selected group (data = groups group ID)
	DWORD data = m_ListBoxGroups.GetItemData(nSelection);
	
	//get the number of groups in the documetns group list
	int nCount = pDoc->listGroups.GetCount();
	//loop through list until we find the selected group
	for (int i=0;i < nCount;i++)
	{
		//get each window group in the list one at a time
		wg = pDoc->listGroups.GetAt(pDoc->listGroups.FindIndex(i));
		//is this the group we are looking for (is its group ID = data of user selection
		if(wg->groupAttributes.GroupID == data){
			//exit for loop with the correct Group saved to wg
			break;
		}
	}
	
	//update the CEdits texts 
	((CEdit *)GetDlgItem (IDC_EDIT_GRP_NAME))->SetWindowText(wg->GroupName);
	((CEdit *)GetDlgItem (IDC_EDIT_GRP_DESC))->SetWindowText(wg->GroupDesc);
	UpdateData(TRUE);

}

BOOL CDWindowGroup::ValidateGroupDelete(CWindowItemExt *wi, UINT index)
{
	if(wi->windowAttributes.GroupNum == index){
		return FALSE;
	}

	POSITION  currentPos = wi->ButtonItemList.GetHeadPosition ();
	POSITION  currentPosLast = currentPos;
	while (currentPos) {
		CWindowControl *pwc = wi->ButtonItemList.GetNext (currentPos);
		ASSERT(pwc);
		if(pwc->controlAttributes.m_nType == CWindowControl::WindowContainer){
			CWindowItemExt *subWin = CWindowItemExt::TranslateCWindowControl(pwc);
			if(!ValidateGroupDelete(subWin, index)){
				return FALSE;
			}
		}
		currentPosLast = currentPos;
	}
	return TRUE;

}

void CDWindowGroup::OnButtonSetactwnd() 
{
	// TODO: Add your control notification handler code here
	//get the index of the user selected group
	int nSelection = m_ListBoxGroups.GetCurSel();
	//get the data of the selected group (data = groups group ID)
	DWORD data = m_ListBoxGroups.GetItemData(nSelection);

	CWindowGroup *wg = NULL;
	//get the document from the LayoutView so we can access its Group list
	CWindowDocumentExt* pDoc = myView->GetDocument();

	//get the number of groups in the documetns group list
	int nCount = pDoc->listGroups.GetCount();
	//loop through list until we find the selected group
	for (int i=0;i < nCount;i++)
	{
		//get each window group in the list one at a time
		wg = pDoc->listGroups.GetAt(pDoc->listGroups.FindIndex(i));
		//is this the group we are looking for (is its group ID = data of user selection
		if(wg->groupAttributes.GroupID == data){
			//exit for loop with the correct Group saved to wg
			break;
		}
	}

	if(wg){
		CDActiveWndEdit *we = new CDActiveWndEdit(wg);
		we->DoModal();
		//update info from change
		m_ListBoxGroups.ResetContent();
		UpdateData(FALSE);
	}
}
