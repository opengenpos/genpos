// DButtonEdit.cpp : implementation file
//

#include "stdafx.h"
#include "NewLayout.h"
#include "NewLayoutDoc.h"
#include "NewLayoutView.h"
#include "ChildFrm.h"

#include "DListBoxEdit.h"

#include "WindowListBox.h"
#include "DefaultData.h"

#include "DGroupPermission.h"

//used for browse dialog
#include<windows.h>
#include<windowsx.h>
#include<commctrl.h>
#include<shlobj.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//used for browse dialog
static int CALLBACK BrowseCallbackProc(
  HWND hwnd,      
  UINT uMsg,        
  LPARAM lParam, 
  LPARAM  lpData
);
static HWND showpath;
static HINSTANCE ghBtnDlgInstance;

//Folder Name that has been selected
static TCHAR PathName[MAX_PATH];
static CString IconDirPath;

/////////////////////////////////////////////////////////////////////////////
// CDButtonEdit dialog
CDListBoxEdit::CDListBoxEdit(CWindowListBoxExt *pb /* = NULL */,CWnd* pParent /*=NULL*/)
	: CDialog(CDListBoxEdit::IDD, pParent)
{
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CChildFrame *pChild = (CChildFrame *) pFrame->GetActiveFrame();
	CNewLayoutView *myView = pChild->GetRightPane();

	pDoc = myView->GetDocument();
	
	memset (m_csSpecWin, 0, sizeof(m_csSpecWin));

	myAction = -1;

	if (pb) {
		CWindowItem *wi  =(CWindowItem*)pParent;
		wi->WindowDef.AssignTo(m_defParent);
		*this = *pb;

		myBtnFont = pb->controlAttributes.lfControlFont;

	}
	else {
		//{{AFX_DATA_INIT(CDButtonEdit)
		m_csCaption = _T("");
		m_myName = _T("");
		m_csIcon = _T("");
		m_csStaticCaption = _T("");
		m_csHeightMultiplier = _T("");
		m_nWidthMultiplier = _T("");
		//}}AFX_DATA_INIT

		//pull in parent windows defaults for control 
		
		CWindowItem *wi  =(CWindowItem*)pParent;
		wi->WindowDef.AssignTo(m_defParent);

		nRow = 0;
		nColumn = 0;
		colorText = m_defParent.defaultAttributes.FontColorDefault;
		colorFace = m_defParent.defaultAttributes.ButtonDefault;
		m_usWidthMultiplier = m_defParent.defaultAttributes.WidthDefault;
		m_usHeightMultiplier = m_defParent.defaultAttributes.HeightDefault;
		horizOrient = TRUE;
		m_GroupPermissions = 0;

		myBtnFont = m_defParent.defaultAttributes.FontDefault;
	}

	m_csStaticCaption.LoadString (IDS_STATIC_DIALOG1_BUTTONCAPTION);
}


void CDListBoxEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX); 

	//{{AFX_DATA_MAP(CDButtonEdit)
	DDX_Control(pDX, IDC_BUTTON_FACECOLOR, m_SetFaceColor);
	DDX_Control(pDX, IDC_SETBTNFONT, m_SetFont);
	DDX_Control(pDX, IDC_STATIC_DIALOG1_ACTION, m_cStaticAction);
	DDX_Control(pDX, IDC_STATIC_SHOW_FACECOLOR, m_StaticShowFaceColor);
	DDX_Control(pDX, IDC_COMBO_DIALOG1_WIDTH, m_ComboWidth);
	DDX_Control(pDX, IDC_COMBO_DIALOG1_HEIGHT, m_ComboHeight);
	DDX_Control(pDX, IDC_EDIT_PREVIEW, m_ShowCap);
	DDX_Control(pDX, IDC_EDIT_CAPTION, m_cCaption);
	DDX_Text(pDX, IDC_EDIT_CAPTION, m_csCaption);
	DDX_Text(pDX, IDC_EDIT_PREVIEW, m_csPreview);
	DDX_Text(pDX, IDC_EDIT_ICONFILENAME, m_csIcon);
	DDX_Text(pDX, IDC_EDIT_CONNENGINE_BUTTON_NAME, m_myName);
	DDX_Text(pDX, IDC_STATIC_CAPTION, m_csStaticCaption);
	DDX_CBString(pDX, IDC_COMBO_DIALOG1_HEIGHT, m_csHeightMultiplier);
	DDX_CBString(pDX, IDC_COMBO_DIALOG1_WIDTH, m_nWidthMultiplier);
	DDX_Control(pDX, IDC_BTN_GROUP_PERMISSIONS, m_SetGroupPermission);
	//}}AFX_DATA_MAP

	m_defFlag = 0;

	if (pDX->m_bSaveAndValidate) {

		//Width and Height
		m_usWidthMultiplier = _wtoi (m_nWidthMultiplier );
		if (m_usWidthMultiplier < MIN_BTN_WIDTH)
			m_usWidthMultiplier = MIN_BTN_WIDTH;
		else if (m_usWidthMultiplier > MAX_BTN_WIDTH)
			m_usWidthMultiplier = MAX_BTN_WIDTH;

		m_usHeightMultiplier = _wtoi (m_csHeightMultiplier );
		if (m_usHeightMultiplier < MIN_BTN_HEIGHT)
			m_usHeightMultiplier = MIN_BTN_HEIGHT;
		else if (m_usHeightMultiplier > MAX_BTN_HEIGHT)
			m_usHeightMultiplier = MAX_BTN_HEIGHT;

		//Color
		colorFace = m_StaticShowFaceColor.m_FillColor;


		m_csSpecWin[UserDefaultWinIndex] = 0;

		m_csSpecWin[SpecWinIndex_SPR_Type] = 0;			
		if(((CButton*)GetDlgItem (IDC_CHECK_SPR_SUPR))->GetCheck()) {
			m_csSpecWin[SpecWinIndex_SPR_Type] |= SpecWinMask_SPR_SUPR;			
		}
		if(((CButton*)GetDlgItem (IDC_CHECK_SPR_PROG))->GetCheck()) {
			m_csSpecWin[SpecWinIndex_SPR_Type] |= SpecWinMask_SPR_PROG;			
		}
		if(((CButton*)GetDlgItem (IDC_CHECK_SPR_REG))->GetCheck()) {
			m_csSpecWin[SpecWinIndex_SPR_Type] |= SpecWinMask_SPR_REG;			
		}

		// Determine what the Connection Engine settings for this button should be
		m_csSpecWin[SpecWinIndex_ConnEngine] = 0;
		if(((CButton*)GetDlgItem (IDC_CHECK_CONNENGINE))->GetCheck()) {
			m_csSpecWin[SpecWinIndex_ConnEngine] |= SpecWinEventConnEngine_Indic;			
		}
		if(((CButton*)GetDlgItem (IDC_CHECK_SCRIPTING))->GetCheck()) {
			m_csSpecWin[SpecWinIndex_ConnEngine] |= SpecWinEventScript_Indic;			
		}
	}
	else {
		//populate combo boxes for width and height
		CString csInsert, csGrid;
		csGrid.LoadString(IDS_STRING_GRIDS);

		for( int z = 0; z < MAX_BTN_HEIGHT; z++ ){
			csInsert.Format(_T("%d %s"), MIN_BTN_HEIGHT + z, csGrid);
			m_ComboHeight.AddString(csInsert);
		}

		for( int y = 0; y < MAX_BTN_WIDTH; y++ ){
			csInsert.Format(_T("%d %s"), MIN_BTN_WIDTH + y, csGrid);
			m_ComboWidth.AddString(csInsert);
		}

		((CEdit*)GetDlgItem (IDC_EDIT_ICONFILENAME))->SetWindowText(m_csIcon);

		// Display the terminal type restrictions for this button
		((CButton*)GetDlgItem (IDC_CHECK_SPR_REG))->SetCheck(BST_UNCHECKED);
		((CButton*)GetDlgItem (IDC_CHECK_SPR_PROG))->SetCheck(BST_UNCHECKED);
		((CButton*)GetDlgItem (IDC_CHECK_SPR_SUPR))->SetCheck(BST_UNCHECKED);

		if(m_csSpecWin[SpecWinIndex_SPR_Type] & SpecWinMask_SPR_SUPR){			
			((CButton*)GetDlgItem (IDC_CHECK_SPR_SUPR))->SetCheck(BST_CHECKED);
		}
		if(m_csSpecWin[SpecWinIndex_SPR_Type] & SpecWinMask_SPR_PROG){			
			((CButton*)GetDlgItem (IDC_CHECK_SPR_PROG))->SetCheck(BST_CHECKED);
		}
		if(m_csSpecWin[SpecWinIndex_SPR_Type] & SpecWinMask_SPR_REG){			
			((CButton*)GetDlgItem (IDC_CHECK_SPR_REG))->SetCheck(BST_CHECKED);
		}
		if(m_csSpecWin[SpecWinIndex_SPR_Type] & SpecWinMask_SPR_AutoClose){
			((CButton*)GetDlgItem (IDC_CHECK_SPR_AUTOCLOSE))->SetCheck(BST_CHECKED);
		}

		// Determine what the Connection Engine settings for this button should be
		if(m_csSpecWin[SpecWinIndex_ConnEngine] & SpecWinEventConnEngine_Indic){			
			((CButton*)GetDlgItem (IDC_CHECK_CONNENGINE))->SetCheck(BST_CHECKED);
		}
		else {
			((CButton*)GetDlgItem (IDC_CHECK_CONNENGINE))->SetCheck(BST_UNCHECKED);
		}
		if(m_csSpecWin[SpecWinIndex_ConnEngine] & SpecWinEventScript_Indic){			
			((CButton*)GetDlgItem (IDC_CHECK_SCRIPTING))->SetCheck(BST_CHECKED);
		}
		else {
			((CButton*)GetDlgItem (IDC_CHECK_SCRIPTING))->SetCheck(BST_UNCHECKED);
		}
		if(m_csSpecWin[SpecWinIndex_ConnEngine] & (SpecWinEventConnEngine_Indic | SpecWinEventScript_Indic)){			
			((CEdit *)GetDlgItem (IDC_EDIT_CONNENGINE_BUTTON_NAME))->EnableWindow(TRUE);
		}
		else {
			((CEdit *)GetDlgItem (IDC_EDIT_CONNENGINE_BUTTON_NAME))->EnableWindow(FALSE);
		}

		//Width and Height --> verify value is between 1 and 6
		if (m_usWidthMultiplier < MIN_BTN_WIDTH)
			m_usWidthMultiplier = MIN_BTN_WIDTH;
		else if (m_usWidthMultiplier > MAX_BTN_WIDTH)
			m_usWidthMultiplier = MAX_BTN_WIDTH;

		if (m_usHeightMultiplier < MIN_BTN_HEIGHT)
			m_usHeightMultiplier = MIN_BTN_HEIGHT;
		else if (m_usHeightMultiplier > MAX_BTN_HEIGHT)
			m_usHeightMultiplier = MAX_BTN_HEIGHT;

		((CComboBox *)GetDlgItem (IDC_COMBO_DIALOG1_HEIGHT))->SetCurSel (m_usHeightMultiplier - MIN_BTN_HEIGHT);
		((CComboBox *)GetDlgItem (IDC_COMBO_DIALOG1_WIDTH))->SetCurSel (m_usWidthMultiplier - MIN_BTN_WIDTH);

		//Color
		m_StaticShowFaceColor.m_FillColor = colorFace;

		CString csStaticAction;
		csStaticAction.LoadString (myAction);
		m_cStaticAction.SetWindowText(csStaticAction);

		SetDlgControls (m_defFlag);
	}
}


BEGIN_MESSAGE_MAP(CDListBoxEdit, CDialog)
	//{{AFX_MSG_MAP(CDListBoxEdit)
	ON_BN_CLICKED(IDC_BUTTON_TEXTCOLOR, OnButtonTextcolor)
	ON_BN_CLICKED(IDC_BUTTON_FACECOLOR, OnButtonFacecolor)
	ON_BN_CLICKED(IDC_SETBTNFONT, OnSetFont)
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(IDC_EDIT_CAPTION, OnChangeEditCaption)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	ON_BN_CLICKED(IDC_BUTTON_SETDIR, OnSetDir)
	ON_BN_CLICKED(IDC_CHECK_CONNENGINE, OnChangeConnEngine)
	ON_BN_CLICKED(IDC_CHECK_SCRIPTING, OnChangeAllowScript)
	ON_BN_CLICKED(IDC_BTN_GROUP_PERMISSIONS, &CDListBoxEdit::OnBnClickedGroupPermissions)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDButtonEdit message handlers

CDListBoxEdit & CDListBoxEdit::operator = (const CWindowListBoxExt & cdt)
{
	m_csCaption = cdt.myCaption;
	m_myName = cdt.myName;
	m_csIcon = cdt.m_Icon;
	colorText = cdt.controlAttributes.m_colorText;
	colorFace = cdt.controlAttributes.m_colorFace;
	myAction = cdt.controlAttributes.m_myAction;
	myBtnFont = cdt.controlAttributes.lfControlFont;
	m_usWidthMultiplier = cdt.controlAttributes.m_usWidthMultiplier;
	m_usHeightMultiplier = cdt.controlAttributes.m_usHeightMultiplier;
	m_GroupPermissions = cdt.controlAttributes.ulMaskGroupPermission;

	for (int i = 0; i < sizeof(cdt.controlAttributes.SpecWin); i++){
		m_csSpecWin[i] = cdt.controlAttributes.SpecWin[i];
	}

	return *this;
}

void CDListBoxEdit::OnButtonTextcolor() 
{
	CColorDialog dlg;
	dlg.m_cc.Flags |= CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT;
	dlg.m_cc.rgbResult = m_StaticShowTextColor.m_FillColor;
	dlg.m_cc.lpCustColors = myCustomColors;
	if (dlg.DoModal()==IDOK) 
	{
		m_StaticShowTextColor.m_FillColor = dlg.m_cc.rgbResult;
		CStaticColorPatch *selectedColor = (CStaticColorPatch *) GetDlgItem (IDC_STATIC_SHOW_TEXTCOLOR);
		selectedColor->m_FillColor = m_StaticShowTextColor.m_FillColor;
		selectedColor->Invalidate (FALSE);
	}
}

void CDListBoxEdit::OnButtonFacecolor() 
{
	CColorDialog dlg;
	dlg.m_cc.Flags |= CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT;
	dlg.m_cc.rgbResult = m_StaticShowFaceColor.m_FillColor;
	dlg.m_cc.lpCustColors = myCustomColors;
	if (dlg.DoModal()==IDOK) 
	{
		m_StaticShowFaceColor.m_FillColor = dlg.m_cc.rgbResult;
		CStaticColorPatch *selectedColor = (CStaticColorPatch *) GetDlgItem (IDC_STATIC_SHOW_FACECOLOR);
		selectedColor->m_FillColor = m_StaticShowFaceColor.m_FillColor;
		selectedColor->Invalidate (FALSE);
		GetDlgItem(IDC_EDIT_PREVIEW)->Invalidate(FALSE);
	}
}

void CDListBoxEdit::OnChangeConnEngine()
{
	BOOL isConnEngineChecked = (((CButton*)GetDlgItem (IDC_CHECK_CONNENGINE))->GetCheck() == BST_CHECKED ) ||
								(((CButton*)GetDlgItem (IDC_CHECK_SCRIPTING))->GetCheck() == BST_CHECKED );
	((CEdit *)GetDlgItem (IDC_EDIT_CONNENGINE_BUTTON_NAME))->EnableWindow(isConnEngineChecked);
}

void CDListBoxEdit::OnChangeAllowScript()
{
	BOOL isAllowScriptChecked = (((CButton*)GetDlgItem (IDC_CHECK_SCRIPTING))->GetCheck() == BST_CHECKED ) ||
								(((CButton*)GetDlgItem (IDC_CHECK_CONNENGINE))->GetCheck() == BST_CHECKED );
	((CEdit *)GetDlgItem (IDC_EDIT_CONNENGINE_BUTTON_NAME))->EnableWindow(isAllowScriptChecked);
}

void CDListBoxEdit::SetDlgControls(BOOL flag)
{
	m_SetFaceColor.EnableWindow(!flag);
	m_ComboWidth.EnableWindow(!flag);
	m_ComboHeight.EnableWindow(!flag);
	m_SetFont.EnableWindow(!flag);
	m_SetGroupPermission.EnableWindow(!flag);
	InvalidateRect (NULL, FALSE);
}

void CDListBoxEdit::OnRadioWinDef() 
{
	// TODO: Add your control notification handler code here
	
	m_usWidthMultiplier = m_defParent.defaultAttributes.WidthDefault;
	m_usHeightMultiplier = m_defParent.defaultAttributes.HeightDefault;

	((CComboBox *)GetDlgItem (IDC_COMBO_DIALOG1_HEIGHT))->SetCurSel (m_usHeightMultiplier - 1);
	((CComboBox *)GetDlgItem (IDC_COMBO_DIALOG1_WIDTH))->SetCurSel (m_usWidthMultiplier - 1);

	//Color
	m_StaticShowTextColor.m_FillColor = m_defParent.defaultAttributes.FontColorDefault;
	m_StaticShowFaceColor.m_FillColor = m_defParent.defaultAttributes.ButtonDefault;

	//Font
	myBtnFont = m_defParent.defaultAttributes.FontDefault;
	colorText = m_defParent.defaultAttributes.FontColorDefault;

	SetDlgControls(TRUE);
}

void CDListBoxEdit::OnRadioCustom() 
{
	SetDlgControls(FALSE);
}

void CDListBoxEdit::OnSetFont()
{
	LOGFONT myLF;
	memset(&myLF,0,sizeof(LOGFONT));
	myLF = myBtnFont;

	CFontDialog FontDlg;

	FontDlg.m_cf.lStructSize = sizeof(CHOOSEFONT);
	FontDlg.m_cf.hwndOwner = NULL;
	FontDlg.m_cf.lpLogFont = &myLF;
	FontDlg.m_cf.Flags = CF_SCREENFONTS | CF_ENABLEHOOK | CF_INITTOLOGFONTSTRUCT  | CF_LIMITSIZE | CF_EFFECTS | CF_NOVERTFONTS;
	FontDlg.m_cf.nSizeMax = MAX_BTN_FONT_SZ;
	FontDlg.m_cf.nSizeMin = MIN_BTN_FONT_SZ;
	FontDlg.m_cf.rgbColors = colorText;

	myLF.lfWidth = 0;

	if(FontDlg.DoModal()== IDOK){
		memcpy(&myBtnFont,FontDlg.m_cf.lpLogFont,sizeof(LOGFONT));
		
		FontDlg.GetCurrentFont(&myBtnFont);
		colorText = FontDlg.GetColor();

		CFont fontSample;
		VERIFY(fontSample.CreateFontIndirect(&myBtnFont));
		m_ShowCap.SetFont(&fontSample,TRUE);

		CDC *pDC = m_ShowCap.GetDC();
		pDC->SetTextColor (FontDlg.GetColor());
		pDC->SelectObject(&fontSample);
		GetDlgItem(IDC_EDIT_PREVIEW)->Invalidate(FALSE);
	}
}

HBRUSH CDListBoxEdit::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	if(nCtlColor == CTLCOLOR_STATIC && pWnd->GetDlgCtrlID() == IDC_EDIT_PREVIEW){
		CFont fontSample;
		VERIFY(fontSample.CreateFontIndirect(&myBtnFont));
		pDC->SelectObject(&fontSample);

		pDC->SetBkMode(TRANSPARENT);
		pDC->SetBkColor(m_StaticShowFaceColor.m_FillColor);

		pDC->SetTextColor(colorText);
		pDC->SetTextAlign(TA_LEFT | TA_TOP);

		CRect pWndRect;

		CDC* pDCWin = GetDC();
		myBtnFont.lfHeight;
		ReleaseDC(pDCWin);

		hbr = CreateSolidBrush(m_StaticShowFaceColor.m_FillColor);

		return (HBRUSH) hbr;
	}
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

void CDListBoxEdit::OnChangeEditCaption() 
{

	CString csTemp;
	GetDlgItemText(IDC_EDIT_CAPTION, csTemp);
	((CEdit *)GetDlgItem (IDC_EDIT_PREVIEW))->SetWindowText(csTemp);
}

BOOL CDListBoxEdit::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_cCaption.LimitText(MaxSizeOfMyCaption);

	CFont fontSample;
	VERIFY(fontSample.CreateFontIndirect(&myBtnFont));
	m_ShowCap.SetFont(&fontSample,TRUE);
	CDC *pDC = m_ShowCap.GetDC();
	pDC->SelectObject(&fontSample);

	CString csStaticAction;
	csStaticAction.LoadString(myAction);

	m_cStaticAction.SetWindowText(csStaticAction);
	m_ShowCap.SetWindowText(m_csCaption);
	return TRUE;  // return TRUE unless you set the focus to a control
}


void CDListBoxEdit::OnButtonBrowse() 
{
	if(!pDoc->csIconDir.GetLength()){
		AfxMessageBox(IDS_SET_ICONDIR_MSG, MB_OK | MB_ICONSTOP, 0);
		OnSetDir();
	}

	UINT nFlags =  CFile::modeRead;
	CString szFilter = _T("Bitmap Files (*.bmp)|*.bmp||");

	//initialize File Dialog so user can get the import file
	CFileDialog fd(TRUE,_T("bmp"),NULL,OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR,szFilter,NULL);
	fd.m_ofn.lpstrInitialDir = pDoc->csIconDir;
	int status = fd.DoModal();
	if(status == 2){
		return;
	}	
	else{
		CString csDir = fd.GetPathName();
		int nCount = fd.GetPathName().GetLength() - fd.GetFileName().GetLength();
		if(pDoc->csIconDir.CompareNoCase(csDir.Left( nCount - 1))){
			AfxMessageBox(IDS_ICONDIR_ERR, MB_OK | MB_ICONSTOP, 0);
		}
		m_csIcon = fd.GetFileName();
		((CEdit *)GetDlgItem (IDC_EDIT_ICONFILENAME))->SetWindowText(m_csIcon);
	}
}

void CDListBoxEdit::OnSetDir() 
{
	IMalloc *imalloc; 
	SHGetMalloc(&imalloc);

	BROWSEINFO bi; 
	memset(&bi, 0, sizeof(bi));

	bi.hwndOwner = m_hWnd;
	CString csTitle;
	csTitle.LoadString(IDS_ICONDIR_SEL_MSG);
	bi.lpszTitle = csTitle;//_T("Select the folder where your icon files will reside");
	bi.ulFlags = BIF_RETURNONLYFSDIRS ;
	bi.lpfn = BrowseCallbackProc;

	IconDirPath = pDoc->csIconDir;

	ITEMIDLIST *pidl = SHBrowseForFolder(&bi);

	//set the selected folder
	if (pidl!=NULL)
		pDoc->csIconDir  = PathName;
	imalloc->Free(pidl);
	imalloc->Release();	
}


int CALLBACK  BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	//Initialization callback message
	if(uMsg==BFFM_INITIALIZED)
	{
		//Rectangles for getting the positions
		RECT ListViewRect,Dialog;
		//Create the control on the dialog box
		showpath = CreateWindowEx(0,
							  _T("STATIC"),
							  _T(""),
							  WS_CHILD | WS_VISIBLE | SS_PATHELLIPSIS,
							  0,
							  100,
							  100,
							  50,
							  hwnd,
							  0,
							  ghBtnDlgInstance,
							  NULL);

		HWND ListView = FindWindowEx(hwnd, NULL, _T("SysTreeView32"), NULL);

		//Gets the dimentions of the windows
		GetWindowRect(hwnd, &Dialog);
		GetWindowRect(ListView, &ListViewRect);

		//Sets the listview controls dimentions
		SetWindowPos(ListView,0,(ListViewRect.left-Dialog.left) ,(ListViewRect.top-Dialog.top )-20,290,170,0);
		//Sets the window positions of edit and dialog controls
		SetWindowPos(showpath,HWND_BOTTOM,(ListViewRect.left-Dialog.left),(ListViewRect.top-Dialog.top )+170,290,18,SWP_SHOWWINDOW);

		//set edit font
		HFONT hf;
		LOGFONT lf={0};
		HDC hdc = GetDC(showpath);
		
		GetObject(GetWindowFont(showpath),sizeof(lf),&lf);
		lf.lfWeight = FW_REGULAR;
		lf.lfHeight = (LONG)12;
		lstrcpy( lf.lfFaceName, _T("MS Sans Serif") );
		hf=CreateFontIndirect(&lf);
		SetBkMode(hdc,OPAQUE);

		// Setting the initial folder selection to the last set
		//  folder.  If the path does not exist, then default
		//  to the system root. If no folder has been selected
		//  previously, savedPath will equal "" and setting an
		//  initial folder is not necessary. - CSMALL
		if( IconDirPath != "") {
			if( _taccess(IconDirPath, 0) == 0)
			{	
				int savedLength = IconDirPath.GetLength();
				SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)IconDirPath.GetBuffer(savedLength));
			}
			else if( _taccess(IconDirPath, 0) == -1)
			{
				AfxMessageBox(_T("Directory does not exist."), MB_OK|MB_ICONEXCLAMATION,-1);
				SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)_T("%SYSROOT%"));
			}
		}

		SendMessage(showpath,WM_SETFONT,(WPARAM)hf,TRUE);
		ReleaseDC(showpath,hdc);
	}

	//Selection change message
	if(uMsg==BFFM_SELCHANGED)
	{
		BOOL t = SHGetPathFromIDList((ITEMIDLIST*)lParam, PathName);
		//Sets the text of the edit control to the current folder
		SetWindowText(showpath,PathName);
    }
	return 0;
}

void CDListBoxEdit::OnBnClickedGroupPermissions()
{
	DGroupPermission gpDiag(m_GroupPermissions, this);
	gpDiag.DoModal();
	m_GroupPermissions = gpDiag.GetGroupPermissionMask();
}


CString CDListBoxEdit::_getWindowName(UINT windowId)
{
	CString windowName;
	POSITION currentPos = pDoc->listControls.GetHeadPosition();
	POSITION currentPosLast = currentPos;
	while (currentPos != NULL) 
	{
		CWindowControl *pwc = pDoc->listControls.GetNext(currentPos);
		if (pwc->controlAttributes.m_myId == windowId)
		{
			windowName = pwc->myName;
			break;
		}
	}
	return windowName;
}
// A-KSo