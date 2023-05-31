// DWinLabel.cpp : implementation file
//

#include "stdafx.h"
#include "NewLayout.h"
#include "DWinLabel.h"
#include "ChildFrm.h"

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
	HWND iconpath;
	HINSTANCE ghLblDlgInstance;
	//Folder Name that has been selected
	TCHAR IconPath[MAX_PATH];
	CString savedIconPath;

/////////////////////////////////////////////////////////////////////////////
// CDWinLabel dialog


CDWinLabel::CDWinLabel(CWindowLabelExt *pl,CWnd* pParent /*=NULL*/)
	: CDialog(CDWinLabel::IDD, pParent)
{
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CChildFrame *pChild = (CChildFrame *) pFrame->GetActiveFrame();
	CNewLayoutView *myView = pChild->GetRightPane();

	pDoc = myView->GetDocument();

	isCounterLabel = FALSE;

	if(pl){
		*this = *pl;
	}
	else{
		bSpecialBorder = 0;
		bLeftBorder = 0;
		bRightBorder = 0;
		bTopBorder = 0;
		bBottomBorder = 0;

		pDoc->DocumentDef.AssignTo(df);

		//{{AFX_DATA_INIT(CDWinLabel)
		m_csIcon = _T("");
		m_csCaption = _T("");
		m_nHeightMultiplier = -1;
		m_nWidthMultiplier = -1;
		//}}AFX_DATA_INIT

		nRow = 0;
		nColumn = 0;
		m_usWidthMultiplier = df.defaultAttributes.WidthDefault;
		m_usHeightMultiplier = df.defaultAttributes.HeightDefault;
		myCapAlignment = CWindowLabel::CaptionAlignmentVMiddle;
		myHorIconAlignment = CWindowLabel::IconAlignmentHCenter;
		myVertIconAlignment =	CWindowLabel::IconAlignmentVMiddle;
		horizOrient = TRUE;
		myLblFont = df.defaultAttributes.FontDefault;
		colorText = df.defaultAttributes.FontColorDefault;
		colorFace = df.defaultAttributes.ButtonDefault;
		memset (m_csSpecWin, 0, sizeof(m_csSpecWin));
	}
}

CDWinLabel::CDWinLabel(CWindowEditBoxExt *pl,CWnd* pParent /*=NULL*/)
	: CDialog(CDWinLabel::IDD, pParent)
{
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CChildFrame *pChild = (CChildFrame *) pFrame->GetActiveFrame();
	CNewLayoutView *myView = pChild->GetRightPane();

	pDoc = myView->GetDocument();

	isCounterLabel = FALSE;

	if(pl){
		*this = *pl;
	}
	else{
		bSpecialBorder = 0;
		bLeftBorder = 0;
		bRightBorder = 0;
		bTopBorder = 0;
		bBottomBorder = 0;

		pDoc->DocumentDef.AssignTo(df);

		//{{AFX_DATA_INIT(CDWinLabel)
		m_csIcon = _T("");
		m_csCaption = _T("");
		m_nHeightMultiplier = -1;
		m_nWidthMultiplier = -1;
		//}}AFX_DATA_INIT

		nRow = 0;
		nColumn = 0;
		m_usWidthMultiplier = df.defaultAttributes.WidthDefault;
		m_usHeightMultiplier = df.defaultAttributes.HeightDefault;
		myCapAlignment = CWindowLabel::CaptionAlignmentVMiddle;
		myHorIconAlignment = CWindowLabel::IconAlignmentHCenter;
		myVertIconAlignment =	CWindowLabel::IconAlignmentVMiddle;
		horizOrient = TRUE;
		myLblFont = df.defaultAttributes.FontDefault;
		colorText = df.defaultAttributes.FontColorDefault;
		colorFace = df.defaultAttributes.ButtonDefault;
		memset (m_csSpecWin, 0, sizeof(m_csSpecWin));
	}
}

void CDWinLabel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDWinLabel)
	DDX_Control(pDX, IDC_COMBO_WINLBL_WIDTH, m_ComboWidth);
	DDX_Control(pDX, IDC_COMBO_WINLBL_HEIGHT, m_ComboHeight);
	DDX_Control(pDX, IDC_STATIC_SHOW_FACECOLOR, m_StaticShowFaceColor);
	DDX_Text(pDX, IDC_EDIT_LBL_ICONFILENAME, m_csIcon);
	DDX_Text(pDX, IDC_EDIT_LBLCAPTION, m_csCaption);
	DDX_Text(pDX, IDC_EDIT_PREVIEW, m_csPreview);
	DDX_CBIndex(pDX, IDC_COMBO_WINLBL_HEIGHT, m_nHeightMultiplier);
	DDX_CBIndex(pDX, IDC_COMBO_WINLBL_WIDTH, m_nWidthMultiplier);
	DDX_Control(pDX, IDC_EDIT_PREVIEW, m_ShowCap);
	DDX_Text(pDX, IDC_EDIT_CONNENGINE_BUTTON_NAME, m_myName);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate) {

		if (m_usHeightMultiplier < 1)
			m_usHeightMultiplier = 1;
		else if(m_usHeightMultiplier >40)
			m_usHeightMultiplier = 40;
		if (m_usWidthMultiplier < 1)
			m_usWidthMultiplier = 1;
		else if(m_usWidthMultiplier > 53)
			m_usWidthMultiplier = 53;

		//Width and Height
		m_usHeightMultiplier = ((CComboBox *)GetDlgItem (IDC_COMBO_WINLBL_HEIGHT))->GetCurSel () + 1;
		m_usWidthMultiplier = ((CComboBox *)GetDlgItem (IDC_COMBO_WINLBL_WIDTH))->GetCurSel () + 1;
		
		//Color
		colorFace = m_StaticShowFaceColor.m_FillColor;

		//Horizontal Caption Alignment
		if(((CButton *)GetDlgItem (IDC_RADIO_LBL_VERTICAL))->GetCheck()){
			horizOrient = FALSE;
			myCapAlignment = CWindowLabel::CaptionAlignmentHLeft;
			if(((CButton *)GetDlgItem (IDC_RADIO_LBLCAP_CENTER))->GetCheck()){
				myCapAlignment = CWindowLabel::CaptionAlignmentHCenter;
			}
			else if(((CButton *)GetDlgItem (IDC_RADIO_LBLCAP_RIGHT))->GetCheck()){
				myCapAlignment = CWindowLabel::CaptionAlignmentHRight;
			}
		}

		//Vertical Caption Alignment
		if(((CButton *)GetDlgItem (IDC_RADIO_LBL_HORIZONTAL))->GetCheck()){
			horizOrient = TRUE;
			myCapAlignment = CWindowLabel::CaptionAlignmentVTop;
			if(((CButton *)GetDlgItem (IDC_RADIO_LBLCAP_MIDDLE))->GetCheck()){
				myCapAlignment = CWindowLabel::CaptionAlignmentVMiddle;
			}
			else if(((CButton *)GetDlgItem (IDC_RADIO_LBLCAP_BOTTOM))->GetCheck()){
				myCapAlignment = CWindowLabel::CaptionAlignmentVBottom;
			}
		}

		isCounterLabel = FALSE;
		if(((CButton *)GetDlgItem (IDC_CHECK_IS_COUNTER))->GetCheck()){
			isCounterLabel = TRUE;
		}

		//Icon alignment - can have both horizontal and vertical
		myHorIconAlignment = CWindowLabel::IconAlignmentHLeft;
		if(((CButton *)GetDlgItem (IDC_RADIO_LBLICON_CENTER))->GetCheck()){
			myHorIconAlignment = CWindowLabel::IconAlignmentHCenter;
		}
		else if(((CButton *)GetDlgItem (IDC_RADIO_LBLICON_RIGHT))->GetCheck()){
			myHorIconAlignment = CWindowLabel::IconAlignmentHRight;
		}

		myVertIconAlignment = CWindowLabel::IconAlignmentVTop;
		if(((CButton *)GetDlgItem (IDC_RADIO_LBLICON_MIDDLE))->GetCheck()){
			myVertIconAlignment = CWindowLabel::IconAlignmentVMiddle;
		}
		else if(((CButton *)GetDlgItem (IDC_RADIO_LBLICON_BOTTOM))->GetCheck()){
			myVertIconAlignment = CWindowLabel::IconAlignmentVBottom;
		}

		// Borders 
		bSpecialBorder = (((CButton *)GetDlgItem (IDC_CHECK_LABEL_BORDER_SPL))->GetCheck() == BST_CHECKED);
		if (bSpecialBorder) {
			bLeftBorder = (((CButton *)GetDlgItem (IDC_CHECK_LABEL_BORDER_LFT))->GetCheck() == BST_CHECKED);
			bRightBorder = ((((CButton *)GetDlgItem (IDC_CHECK_LABEL_BORDER_RHT))->GetCheck()) == BST_CHECKED);
			bTopBorder = ((((CButton *)GetDlgItem (IDC_CHECK_LABEL_BORDER_TOP))->GetCheck()) == BST_CHECKED);
			bBottomBorder = ((((CButton *)GetDlgItem (IDC_CHECK_LABEL_BORDER_BOT))->GetCheck()) == BST_CHECKED);
		}
		else {
			bLeftBorder = 0;
			bRightBorder = 0;
			bTopBorder = 0;
			bBottomBorder = 0;
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
		if (m_usHeightMultiplier < 1)
			m_usHeightMultiplier = 1;
		else if(m_usHeightMultiplier > 40)
			m_usHeightMultiplier = 40;
		if (m_usWidthMultiplier < 1)
			m_usWidthMultiplier = 1;
		else if(m_usWidthMultiplier >53)
			m_usWidthMultiplier = 53;
				
		((CEdit*)GetDlgItem (IDC_EDIT_LBL_ICONFILENAME))->SetWindowText(m_csIcon);
		
		
		//Added the this section to correct SR 263
		//This section populates the combo box for the label height and width{
		CString csInsert, csGrid;
		csGrid.LoadString(IDS_STRING_GRIDS);

		for (int i = 0; i < MAX_LBL_WIDTH; i++)
		{
			csInsert.Format(_T("%d %s"), MIN_LBL_WIDTH + i, csGrid);
			((CComboBox *)GetDlgItem (IDC_COMBO_WINLBL_WIDTH))->AddString(csInsert);
		}
	
		for (int j = 0; j < MAX_LBL_HEIGHT; j++)
		{
			csInsert.Format(_T("%d %s"), MIN_LBL_HEIGHT + j, csGrid);
			((CComboBox *)GetDlgItem (IDC_COMBO_WINLBL_HEIGHT))->AddString(csInsert);
		}
		
		//Color
		m_StaticShowFaceColor.m_FillColor = colorFace;

		//Text and Icon alignment controls*/
		//set initial checks and states for caption/icon alignemnt
		if(!horizOrient){
			((CButton *)GetDlgItem (IDC_RADIO_LBL_VERTICAL))->SetCheck(TRUE);
			((CButton *)GetDlgItem (IDC_RADIO_LBLCAP_TOP))->EnableWindow(FALSE);
			((CButton *)GetDlgItem (IDC_RADIO_LBLCAP_MIDDLE))->EnableWindow(FALSE);
			((CButton *)GetDlgItem (IDC_RADIO_LBLCAP_BOTTOM))->EnableWindow(FALSE);
		}
		else{
			((CButton *)GetDlgItem (IDC_RADIO_LBL_HORIZONTAL))->SetCheck(TRUE);
			((CButton *)GetDlgItem (IDC_RADIO_LBLCAP_LEFT))->EnableWindow(FALSE);
			((CButton *)GetDlgItem (IDC_RADIO_LBLCAP_CENTER))->EnableWindow(FALSE);
			((CButton *)GetDlgItem (IDC_RADIO_LBLCAP_RIGHT))->EnableWindow(FALSE);
		}
		
		if (isCounterLabel) {
			((CButton *)GetDlgItem (IDC_CHECK_IS_COUNTER))->SetCheck(TRUE);
		}
		else {
			((CButton *)GetDlgItem (IDC_CHECK_IS_COUNTER))->SetCheck(FALSE);
		}

		((CButton *)GetDlgItem (IDC_RADIO_LBLCAP_LEFT))->SetCheck((myCapAlignment == CWindowLabel::CaptionAlignmentHLeft)? 1 : 0);
		((CButton *)GetDlgItem (IDC_RADIO_LBLCAP_CENTER))->SetCheck((myCapAlignment == CWindowLabel::CaptionAlignmentHCenter)? 1 : 0);
		((CButton *)GetDlgItem (IDC_RADIO_LBLCAP_RIGHT))->SetCheck((myCapAlignment == CWindowLabel::CaptionAlignmentHRight)? 1 : 0);
		((CButton *)GetDlgItem (IDC_RADIO_LBLCAP_TOP))->SetCheck((myCapAlignment == CWindowLabel::CaptionAlignmentVTop)? 1 : 0);
		((CButton *)GetDlgItem (IDC_RADIO_LBLCAP_MIDDLE))->SetCheck((myCapAlignment == CWindowLabel::CaptionAlignmentVMiddle)? 1 : 0);
		((CButton *)GetDlgItem (IDC_RADIO_LBLCAP_BOTTOM))->SetCheck((myCapAlignment == CWindowLabel::CaptionAlignmentVBottom)? 1 : 0);
		((CButton *)GetDlgItem (IDC_RADIO_LBLICON_LEFT))->SetCheck((myHorIconAlignment == CWindowLabel::IconAlignmentHLeft)? 1 : 0);
		((CButton *)GetDlgItem (IDC_RADIO_LBLICON_CENTER))->SetCheck((myHorIconAlignment == CWindowLabel::IconAlignmentHCenter)? 1 : 0);
		((CButton *)GetDlgItem (IDC_RADIO_LBLICON_RIGHT))->SetCheck((myHorIconAlignment == CWindowLabel::IconAlignmentHRight)? 1 : 0);
		((CButton *)GetDlgItem (IDC_RADIO_LBLICON_TOP))->SetCheck((myVertIconAlignment == CWindowLabel::IconAlignmentVTop)? 1 : 0);
		((CButton *)GetDlgItem (IDC_RADIO_LBLICON_MIDDLE))->SetCheck((myVertIconAlignment == CWindowLabel::IconAlignmentVMiddle)? 1 : 0);
		((CButton *)GetDlgItem (IDC_RADIO_LBLICON_BOTTOM))->SetCheck((myVertIconAlignment == CWindowLabel::IconAlignmentVBottom)? 1 : 0);

		// Borders 
		if (bSpecialBorder)
		{
			((CButton *)GetDlgItem (IDC_CHECK_LABEL_BORDER_SPL))->SetCheck(BST_CHECKED);
			if (bLeftBorder)
			{
				((CButton *)GetDlgItem (IDC_CHECK_LABEL_BORDER_LFT))->SetCheck(BST_CHECKED);
			}
			if (bRightBorder)
			{
				(((CButton *)GetDlgItem (IDC_CHECK_LABEL_BORDER_RHT))->SetCheck(BST_CHECKED));
			}
			if (bTopBorder)
			{
				(((CButton *)GetDlgItem (IDC_CHECK_LABEL_BORDER_TOP))->SetCheck(BST_CHECKED));
			}
			if (bBottomBorder)
			{
				(((CButton *)GetDlgItem (IDC_CHECK_LABEL_BORDER_BOT))->SetCheck(BST_CHECKED));
			}
		}
		else {
			((CButton *)GetDlgItem (IDC_CHECK_LABEL_BORDER_LFT))->EnableWindow (FALSE);
			((CButton *)GetDlgItem (IDC_CHECK_LABEL_BORDER_RHT))->EnableWindow (FALSE);
			((CButton *)GetDlgItem (IDC_CHECK_LABEL_BORDER_TOP))->EnableWindow (FALSE);
			((CButton *)GetDlgItem (IDC_CHECK_LABEL_BORDER_BOT))->EnableWindow (FALSE);
		}

		// Determine what the Connection Engine and Script Engine settings for this button should be
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
	}
}


BEGIN_MESSAGE_MAP(CDWinLabel, CDialog)
	//{{AFX_MSG_MAP(CDWinLabel)
	ON_BN_CLICKED(IDC_WINLBL_FACECOLOR, OnBtnFacecolor)
	ON_BN_CLICKED(IDC_SETWINLBLFONT, OnSetFont)
	ON_BN_CLICKED(IDC_RADIO_LBL_HORIZONTAL, OnRadioHorizontal)
	ON_BN_CLICKED(IDC_RADIO_LBL_VERTICAL, OnRadioVertical)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	ON_BN_CLICKED(IDC_BUTTONSETDIR, OnSetDir)
	ON_BN_CLICKED(IDC_CHECK_LABEL_BORDER_SPL, OnSpecialBorder)
	ON_EN_CHANGE(IDC_EDIT_LBLCAPTION, OnChangeEditLblcaption)
	ON_BN_CLICKED(IDC_CHECK_CONNENGINE, OnChangeConnEngine)
	ON_BN_CLICKED(IDC_CHECK_SCRIPTING, OnChangeAllowScript)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDWinLabel message handlers

// WARNING:  If modifying the CDWinLabel class, make sure that you also make
//           any necessary changes to method void CNewLayoutView::OnAddLabel() 
CDWinLabel & CDWinLabel::operator = (const CWindowLabelExt & cdt)
{
	m_csCaption = cdt.labelAttributes.tcsLabelText;
	m_csIcon = cdt.m_IconName;
	colorText = cdt.controlAttributes.m_colorText;
	colorFace = cdt.controlAttributes.m_colorFace;
	myLblFont = cdt.controlAttributes.lfControlFont;
	m_myName  = cdt.controlAttributes.m_myName;
	m_usWidthMultiplier = cdt.controlAttributes.m_usWidthMultiplier;
	m_usHeightMultiplier = cdt.controlAttributes.m_usHeightMultiplier;
	myCapAlignment = cdt.labelAttributes.capAlignment;
	myHorIconAlignment = cdt.labelAttributes.horIconAlignment;
	myVertIconAlignment = cdt.labelAttributes.vertIconAlignment;
	horizOrient = cdt.labelAttributes.HorizontalOriented;
	isCounterLabel = cdt.labelAttributes.PickCounter;

	bSpecialBorder = ((cdt.controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] & SpecWinIndex_WIN_NOBORDER) != 0);
	bLeftBorder = ((cdt.controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] & SpecWinIndex_WIN_BORDERLFT) != 0);
	bRightBorder = ((cdt.controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] & SpecWinIndex_WIN_BORDERRHT) != 0);
	bTopBorder = ((cdt.controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] & SpecWinIndex_WIN_BORDERTOP) != 0);
	bBottomBorder = ((cdt.controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] & SpecWinIndex_WIN_BORDERBOT) != 0);

	for (int i = 0; i < sizeof(cdt.controlAttributes.SpecWin); i++){
		m_csSpecWin[i] = cdt.controlAttributes.SpecWin[i];
	}
	return *this;
}

CDWinLabel & CDWinLabel::operator = (const CWindowEditBoxExt & cdt)
{
	m_csCaption = cdt.editboxAttributes.tcsLabelText;
	m_csIcon = cdt.m_IconName;
	colorText = cdt.controlAttributes.m_colorText;
	colorFace = cdt.controlAttributes.m_colorFace;
	myLblFont = cdt.controlAttributes.lfControlFont;
	m_myName  = cdt.controlAttributes.m_myName;
	m_usWidthMultiplier = cdt.controlAttributes.m_usWidthMultiplier;
	m_usHeightMultiplier = cdt.controlAttributes.m_usHeightMultiplier;
	horizOrient = cdt.editboxAttributes.HorizontalOriented;
	isCounterLabel = FALSE;

	bSpecialBorder = ((cdt.controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] & SpecWinIndex_WIN_NOBORDER) != 0);
	bLeftBorder = ((cdt.controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] & SpecWinIndex_WIN_BORDERLFT) != 0);
	bRightBorder = ((cdt.controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] & SpecWinIndex_WIN_BORDERRHT) != 0);
	bTopBorder = ((cdt.controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] & SpecWinIndex_WIN_BORDERTOP) != 0);
	bBottomBorder = ((cdt.controlAttributes.SpecWin[SpecWinIndex_WIN_ATTRIB01] & SpecWinIndex_WIN_BORDERBOT) != 0);

	for (int i = 0; i < sizeof(cdt.controlAttributes.SpecWin); i++){
		m_csSpecWin[i] = cdt.controlAttributes.SpecWin[i];
	}
	return *this;
}

void CDWinLabel::OnBtnFacecolor() 
{
	// TODO: Add your control notification handler code here
	CColorDialog dlg;
	dlg.m_cc.Flags |= CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT;
	dlg.m_cc.rgbResult = m_StaticShowFaceColor.m_FillColor;
	dlg.m_cc.lpCustColors = myCustomColors;
	if (dlg.DoModal() == IDOK) {
		m_StaticShowFaceColor.m_FillColor = dlg.m_cc.rgbResult;
		CStaticColorPatch *selectedColor = (CStaticColorPatch *) GetDlgItem (IDC_STATIC_SHOW_FACECOLOR);
		selectedColor->m_FillColor = m_StaticShowFaceColor.m_FillColor;
		selectedColor->Invalidate (FALSE);
		GetDlgItem(IDC_EDIT_PREVIEW)->Invalidate(FALSE);
	}
	
}

void CDWinLabel::OnSetFont() 
{
	// TODO: Add your control notification handler code here
	LOGFONT myLF;
	memset(&myLF,0,sizeof(LOGFONT));
	myLF = myLblFont;

	CFontDialog FontDlg;

	FontDlg.m_cf.lStructSize = sizeof(CHOOSEFONT);
	FontDlg.m_cf.hwndOwner = NULL;
	FontDlg.m_cf.lpLogFont = &myLF;
	FontDlg.m_cf.Flags = CF_SCREENFONTS | CF_ENABLEHOOK | CF_INITTOLOGFONTSTRUCT  | /*CF_LIMITSIZE |*/ CF_EFFECTS | CF_NOVERTFONTS;
	FontDlg.m_cf.nSizeMax = MAX_LBL_FONT_SZ;
	FontDlg.m_cf.nSizeMin = MIN_LBL_FONT_SZ;
	FontDlg.m_cf.rgbColors = colorText;
	
	
	CDC *pDCWin = GetDC ();
	ReleaseDC(pDCWin);

	myLF.lfWidth = 0;

	if(FontDlg.DoModal()== IDOK){
		memcpy(&myLblFont,FontDlg.m_cf.lpLogFont,sizeof(LOGFONT));
		
		FontDlg.GetCurrentFont(&myLblFont);
		colorText = FontDlg.GetColor();

		CFont fontSample;
		VERIFY(fontSample.CreateFontIndirect(&myLblFont));
		m_ShowCap.SetFont(&fontSample,TRUE);

		CDC *pDC = m_ShowCap.GetDC();
		pDC->SetTextColor (FontDlg.GetColor());
		pDC->SelectObject(&fontSample);
		GetDlgItem(IDC_EDIT_PREVIEW)->Invalidate(FALSE);
	}
	
}

void CDWinLabel::OnRadioHorizontal() 
{
	// TODO: Add your control notification handler code here
	((CButton *)GetDlgItem (IDC_RADIO_LBL_HORIZONTAL))->SetCheck(TRUE);
	((CButton *)GetDlgItem (IDC_RADIO_LBLCAP_LEFT))->EnableWindow(FALSE);
	((CButton *)GetDlgItem (IDC_RADIO_LBLCAP_CENTER))->EnableWindow(FALSE);
	((CButton *)GetDlgItem (IDC_RADIO_LBLCAP_RIGHT))->EnableWindow(FALSE);
	((CButton *)GetDlgItem (IDC_RADIO_LBLCAP_TOP))->EnableWindow(TRUE);
	((CButton *)GetDlgItem (IDC_RADIO_LBLCAP_MIDDLE))->EnableWindow(TRUE);
	((CButton *)GetDlgItem (IDC_RADIO_LBLCAP_BOTTOM))->EnableWindow(TRUE);
	
}

void CDWinLabel::OnRadioVertical() 
{
	// TODO: Add your control notification handler code here
	((CButton *)GetDlgItem (IDC_RADIO_LBL_VERTICAL))->SetCheck(TRUE);
	((CButton *)GetDlgItem (IDC_RADIO_LBLCAP_LEFT))->EnableWindow(TRUE);
	((CButton *)GetDlgItem (IDC_RADIO_LBLCAP_CENTER))->EnableWindow(TRUE);
	((CButton *)GetDlgItem (IDC_RADIO_LBLCAP_RIGHT))->EnableWindow(TRUE);
	((CButton *)GetDlgItem (IDC_RADIO_LBLCAP_TOP))->EnableWindow(FALSE);
	((CButton *)GetDlgItem (IDC_RADIO_LBLCAP_MIDDLE))->EnableWindow(FALSE);
	((CButton *)GetDlgItem (IDC_RADIO_LBLCAP_BOTTOM))->EnableWindow(FALSE);
}

HBRUSH CDWinLabel::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	if(nCtlColor == CTLCOLOR_STATIC && pWnd->GetDlgCtrlID() == IDC_EDIT_PREVIEW){
		CFont fontSample;
		VERIFY(fontSample.CreateFontIndirect(&myLblFont));
		pDC->SelectObject(&fontSample);
		
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetBkColor(m_StaticShowFaceColor.m_FillColor);
		
		

		pDC->SetTextColor(colorText);
		
		pDC->SetTextAlign(TA_LEFT | TA_TOP);

		hbr = CreateSolidBrush(m_StaticShowFaceColor.m_FillColor);

		return (HBRUSH) hbr;
	}

	// TODO: Return a different brush if the default is not desired
	return hbr;
}

void CDWinLabel::OnButtonBrowse() 
{
	// TODO: Add your control notification handler code here
	if(!pDoc->csIconDir.GetLength()){
		AfxMessageBox(IDS_SET_ICONDIR_MSG,
					  MB_OK | MB_ICONSTOP,
					  0);
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
			AfxMessageBox(IDS_ICONDIR_ERR,
						  MB_OK | MB_ICONSTOP,
						  0);
		}
		m_csIcon = fd.GetFileName();
		((CEdit *)GetDlgItem (IDC_EDIT_LBL_ICONFILENAME))->SetWindowText(m_csIcon);
	}
	
	
}

void CDWinLabel::OnSetDir() 
{
	// TODO: Add your control notification handler code here
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

	savedIconPath = pDoc->csIconDir;
	
	ITEMIDLIST *pidl = SHBrowseForFolder(&bi);

	//set the selected folder
	if (pidl!=NULL)
		pDoc->csIconDir  = IconPath;
			
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
		iconpath = CreateWindowEx(0,
							  _T("STATIC"),
							  _T(""),
							  WS_CHILD | WS_VISIBLE | SS_PATHELLIPSIS,
							  0,
							  100,
							  100,
							  50,
							  hwnd,
							  0,
							  ghLblDlgInstance,
							  NULL);

		HWND ListView = FindWindowEx(hwnd, NULL, _T("SysTreeView32"), NULL);

		//Gets the dimentions of the windows
		GetWindowRect(hwnd, &Dialog);
		GetWindowRect(ListView, &ListViewRect);

		//Sets the listview controls dimentions
		SetWindowPos(ListView,0,(ListViewRect.left-Dialog.left) ,(ListViewRect.top-Dialog.top )-20,290,170,0);
		//Sets the window positions of edit and dialog controls
		SetWindowPos(iconpath,HWND_BOTTOM,(ListViewRect.left-Dialog.left),(ListViewRect.top-Dialog.top )+170,290,18,SWP_SHOWWINDOW);

		//set edit font
		HFONT hf;
		LOGFONT lf={0};
		HDC hdc = GetDC(iconpath);
		
		GetObject(GetWindowFont(iconpath),sizeof(lf),&lf);
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
		if( savedIconPath != "") {
			if( _taccess(savedIconPath, 0) == 0)
			{	
				int savedLength = savedIconPath.GetLength();
				SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)savedIconPath.GetBuffer(savedLength));
			}
			else if( _taccess(savedIconPath, 0) == -1)
			{
				AfxMessageBox(_T("Directory does not exist."), MB_OK|MB_ICONEXCLAMATION,-1);
				SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)_T("%SYSROOT%"));
			}
		}

		SendMessage(iconpath,WM_SETFONT,(WPARAM)hf,TRUE);
		ReleaseDC(iconpath,hdc);
		
	}

	//Selection change message
	if(uMsg==BFFM_SELCHANGED)
	{
		BOOL t = SHGetPathFromIDList((ITEMIDLIST*)lParam, IconPath);

		//Sets the text of the edit control to the current folder
		SetWindowText(iconpath,IconPath);
		
    }
	
	return 0;
}

BOOL CDWinLabel::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	//Added these two lines of code to correct SR 263
	//This two lines of code select the saved height and width of the label box
	((CComboBox *)GetDlgItem (IDC_COMBO_WINLBL_WIDTH))->SetCurSel (m_usWidthMultiplier - 1);
	((CComboBox *)GetDlgItem (IDC_COMBO_WINLBL_HEIGHT))->SetCurSel (m_usHeightMultiplier - 1);
	((CEdit *)GetDlgItem (IDC_EDIT_LBLCAPTION))->LimitText (256);

	CFont	fontSample;


	CDC* pDCWin = GetDC();
	ReleaseDC(pDCWin);

	VERIFY(fontSample.CreateFontIndirect(&myLblFont));
	m_ShowCap.SetFont(&fontSample,TRUE);
		
	m_ShowCap.ModifyStyle(NULL, WS_CLIPCHILDREN);

	CDC *pDC = m_ShowCap.GetDC();
	pDC->SelectObject(&fontSample);

	((CEdit *)GetDlgItem (IDC_EDIT_PREVIEW))->SetWindowText(m_csCaption);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDWinLabel::OnChangeEditLblcaption() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
	//The following code generates the text in the preview area
	CString csTemp;
	GetDlgItemText(IDC_EDIT_LBLCAPTION, csTemp);
	((CEdit *)GetDlgItem (IDC_EDIT_PREVIEW))->SetWindowText(csTemp);
}


void CDWinLabel::OnSpecialBorder() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
	bSpecialBorder = (((CButton *)GetDlgItem (IDC_CHECK_LABEL_BORDER_SPL))->GetCheck() == BST_CHECKED);
	((CButton *)GetDlgItem (IDC_CHECK_LABEL_BORDER_LFT))->EnableWindow (bSpecialBorder);
	((CButton *)GetDlgItem (IDC_CHECK_LABEL_BORDER_RHT))->EnableWindow (bSpecialBorder);
	((CButton *)GetDlgItem (IDC_CHECK_LABEL_BORDER_TOP))->EnableWindow (bSpecialBorder);
	((CButton *)GetDlgItem (IDC_CHECK_LABEL_BORDER_BOT))->EnableWindow (bSpecialBorder);
}

void CDWinLabel::OnChangeConnEngine()
{
	BOOL isConnEngineChecked = (((CButton*)GetDlgItem (IDC_CHECK_CONNENGINE))->GetCheck() == BST_CHECKED ) ||
								(((CButton*)GetDlgItem (IDC_CHECK_SCRIPTING))->GetCheck() == BST_CHECKED );
	((CEdit *)GetDlgItem (IDC_EDIT_CONNENGINE_BUTTON_NAME))->EnableWindow(isConnEngineChecked);
}

void CDWinLabel::OnChangeAllowScript()
{
	BOOL isAllowScriptChecked = (((CButton*)GetDlgItem (IDC_CHECK_SCRIPTING))->GetCheck() == BST_CHECKED ) ||
								(((CButton*)GetDlgItem (IDC_CHECK_CONNENGINE))->GetCheck() == BST_CHECKED );
	((CEdit *)GetDlgItem (IDC_EDIT_CONNENGINE_BUTTON_NAME))->EnableWindow(isAllowScriptChecked);
}