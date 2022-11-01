// DButtonEdit.cpp : implementation file
//

#include "stdafx.h"
#include "NewLayout.h"
#include "NewLayoutDoc.h"
#include "NewLayoutView.h"
#include "ChildFrm.h"

#include "DButtonEdit.h"
#include "DP03ToolBox.h"
#include "DEntryPLU.h"
#include "DEntryWinID.h"		//definition for the popup dialogue associated with the fsc codes: FSC_WINDOW_DISPLAY and FSC_WINDOW_DISMISS
#include "DEntryDept.h"
#include "DWindowList.h"
#include "ModeEntry.h"
#include "KeypadEntry.h"
#include "DButtonAction.h"

#include "WindowButton.h"
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
HWND showpath;
HINSTANCE ghBtnDlgInstance;

//Folder Name that has been selected
TCHAR PathName[MAX_PATH];
CString IconDirPath;

/////////////////////////////////////////////////////////////////////////////
// CDButtonEdit dialog
CDButtonEdit::CDButtonEdit(CWindowButtonExt *pb /* = NULL */,CWnd* pParent /*=NULL*/)
	: CDialog(CDButtonEdit::IDD, pParent)
{
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CChildFrame *pChild = (CChildFrame *) pFrame->GetActiveFrame();
	CNewLayoutView *myView = pChild->GetRightPane();

	pDoc = myView->GetDocument();
	
	memset (m_csSpecWin, 0, sizeof(m_csSpecWin));

	myAction = -1;

	if (pb) {
		CWindowItem *wi  =(CWindowItem*)pParent;
		wi->WindowDef.AssignTo(df);
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
		wi->WindowDef.AssignTo(df);

		nRow = 0;
		nColumn = 0;
		myShape = df.defaultAttributes.ShapeDefault;
		myPattern = df.defaultAttributes.PatternDefault;
		colorText = df.defaultAttributes.FontColorDefault;
		colorFace = df.defaultAttributes.ButtonDefault;
		m_usWidthMultiplier = df.defaultAttributes.WidthDefault;
		m_usHeightMultiplier = df.defaultAttributes.HeightDefault;
		myCapAlignment = CWindowButton::CaptionAlignmentVMiddle;
		myHorIconAlignment = CWindowButton::IconAlignmentHCenter;
		myVertIconAlignment =	CWindowButton::IconAlignmentVMiddle;
		horizOrient = TRUE;
		m_GroupPermissions = 0;

		memset (&myActionUnion, 0, sizeof (myActionUnion));

		myBtnFont = df.defaultAttributes.FontDefault;
	}

	m_csStaticCaption.LoadString (IDS_STATIC_DIALOG1_BUTTONCAPTION);
}


void CDButtonEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX); 

	//{{AFX_DATA_MAP(CDButtonEdit)
	DDX_Control(pDX, IDC_BUTTON_FACECOLOR, m_SetFaceColor);
	DDX_Control(pDX, IDC_SETBTNFONT, m_SetFont);
	DDX_Control(pDX, IDC_RADIO_USEWINDEF, m_UseWinDef);
	DDX_Control(pDX, IDC_RADIO_USECUSTOM, m_UseCustom);
	DDX_Control(pDX, IDC_STATIC_DIALOG1_ACTION, m_cStaticAction);
	DDX_Control(pDX, IDC_STATIC_SHOW_FACECOLOR, m_StaticShowFaceColor);
	DDX_Control(pDX, IDC_COMBO_DIALOG1_WIDTH, m_ComboWidth);
	DDX_Control(pDX, IDC_COMBO_DIALOG1_HEIGHT, m_ComboHeight);
	DDX_Control(pDX, IDC_DIALOG1_SHAPE_RECT, m_ButtonShapeRectangle);
	DDX_Control(pDX, IDC_DIALOG1_SHAPE_ELIPSE, m_ButtonShapeElipse);
	DDX_Control(pDX, IDC_DIALOG1_SHAPE_ROUNDRECT, m_ButtonShapeRoundedRectangle);
	DDX_Control(pDX, IDC_STATIC_DIALOG1_PATTERN_NONE, m_ButtonPatternNone);
	DDX_Control(pDX, IDC_STATIC_DIALOG1_PATTERN_HORIZ, m_ButtonPatternHoriz);
	DDX_Control(pDX, IDC_STATIC_DIALOG1_PATTERN_VERT, m_ButtonPatternVert);
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

	if (pDX->m_bSaveAndValidate) {
		//Button Shape
		myShape = CWindowButton::ButtonShapeRect;

		if (m_ButtonShapeElipse.GetCheck ()) {
			myShape = CWindowButton::ButtonShapeElipse;
		}
		else if (m_ButtonShapeRoundedRectangle.GetCheck ()) {
			myShape = CWindowButton::ButtonShapeRoundedRect;
		}

		//Button Pattern
		myPattern = CWindowButton::ButtonPatternNone;

		if (m_ButtonPatternHoriz.GetCheck ()) {
			myPattern = CWindowButton::ButtonPatternHoriz;
		}
		else if (m_ButtonPatternVert.GetCheck ()) {
			myPattern = CWindowButton::ButtonPatternVert;
		}

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

		if(m_UseWinDef.GetCheck()){
			defFlag = 1;
		}
		if(m_UseCustom.GetCheck()){
			defFlag = 0;
		}

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
		if(((CButton*)GetDlgItem (IDC_CHECK_SPR_AUTOCLOSE))->GetCheck()) {
			m_csSpecWin[SpecWinIndex_SPR_Type] |= SpecWinMask_SPR_AutoClose;			
		}


		// Determine what the Connection Engine settings for this button should be
		m_csSpecWin[SpecWinIndex_ConnEngine] = 0;
		if(((CButton*)GetDlgItem (IDC_CHECK_CONNENGINE))->GetCheck()) {
			m_csSpecWin[SpecWinIndex_ConnEngine] |= SpecWinEventConnEngine_Indic;			
		}
		if(((CButton*)GetDlgItem (IDC_CHECK_SCRIPTING))->GetCheck()) {
			m_csSpecWin[SpecWinIndex_ConnEngine] |= SpecWinEventScript_Indic;			
		}

		//Horizontal Caption Alignment
		if(((CButton *)GetDlgItem (IDC_RADIO_VERTICAL))->GetCheck()){
			horizOrient = FALSE;
			myCapAlignment = CWindowButton::CaptionAlignmentHLeft;
			if(((CButton *)GetDlgItem (IDC_RADIO_CAP_CENTER))->GetCheck()){
				myCapAlignment = CWindowButton::CaptionAlignmentHCenter;
			}
			else if(((CButton *)GetDlgItem (IDC_RADIO_CAP_RIGHT))->GetCheck()){
				myCapAlignment = CWindowButton::CaptionAlignmentHRight;
			}

		}
		//Vertical Caption Alignment
		if(((CButton *)GetDlgItem (IDC_RADIO_HORIZONTAL))->GetCheck()){
			horizOrient = TRUE;
			myCapAlignment = CWindowButton::CaptionAlignmentVTop;
			if(((CButton *)GetDlgItem (IDC_RADIO_CAP_MIDDLE))->GetCheck()){
				myCapAlignment = CWindowButton::CaptionAlignmentVMiddle;
			}
			else if(((CButton *)GetDlgItem (IDC_RADIO_CAP_BOTTOM))->GetCheck()){
				myCapAlignment = CWindowButton::CaptionAlignmentVBottom;
			}
		}

		//Icon alignment - can have both horizontal and vertical
		myHorIconAlignment = CWindowButton::IconAlignmentHLeft;
		if(((CButton *)GetDlgItem (IDC_RADIO_CAP_CENTER2))->GetCheck()){
			myHorIconAlignment = CWindowButton::IconAlignmentHCenter;
		}
		else if(((CButton *)GetDlgItem (IDC_RADIO_CAP_RIGHT2))->GetCheck()){
			myHorIconAlignment = CWindowButton::IconAlignmentHRight;
		}

		myVertIconAlignment = CWindowButton::IconAlignmentVTop;
		if(((CButton *)GetDlgItem (IDC_RADIO_CAP_MIDDLE2))->GetCheck()){
			myVertIconAlignment = CWindowButton::IconAlignmentVMiddle;
		}
		else if(((CButton *)GetDlgItem (IDC_RADIO_CAP_BOTTOM2))->GetCheck()){
			myVertIconAlignment = CWindowButton::IconAlignmentVBottom;
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

		((CButton*)GetDlgItem (IDC_CHECK_CUST_CAP))->SetCheck(TRUE);

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

		//Button Shape
		m_ButtonShapeRectangle.SetCheck ((myShape == CWindowButton::ButtonShapeRect) ? 1 : 0);
		m_ButtonShapeElipse.SetCheck ((myShape == CWindowButton::ButtonShapeElipse) ? 1 : 0);
		m_ButtonShapeRoundedRectangle.SetCheck ((myShape == CWindowButton::ButtonShapeRoundedRect) ? 1 : 0);

		//Button Pattern
		m_ButtonPatternNone.SetCheck ((myPattern == CWindowButton::ButtonPatternNone) ? 1 : 0);
		m_ButtonPatternHoriz.SetCheck ((myPattern == CWindowButton::ButtonPatternHoriz) ? 1 : 0);
		m_ButtonPatternVert.SetCheck ((myPattern == CWindowButton::ButtonPatternVert) ? 1 : 0);

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

		if(defFlag)
			m_UseWinDef.SetCheck(1);
		else
			m_UseCustom.SetCheck(1);
		SetDlgControls (defFlag);

		//Text and Icon alignment controls*/
		//set initial checks and states for caption/icon alignemnt
		if(!horizOrient){
			((CButton *)GetDlgItem (IDC_RADIO_VERTICAL))->SetCheck(TRUE);
			((CButton *)GetDlgItem (IDC_RADIO_CAP_TOP))->EnableWindow(FALSE);
			((CButton *)GetDlgItem (IDC_RADIO_CAP_MIDDLE))->EnableWindow(FALSE);
			((CButton *)GetDlgItem (IDC_RADIO_CAP_BOTTOM))->EnableWindow(FALSE);
		}
		else{
			((CButton *)GetDlgItem (IDC_RADIO_HORIZONTAL))->SetCheck(TRUE);
			((CButton *)GetDlgItem (IDC_RADIO_CAP_LEFT))->EnableWindow(FALSE);
			((CButton *)GetDlgItem (IDC_RADIO_CAP_CENTER))->EnableWindow(FALSE);
			((CButton *)GetDlgItem (IDC_RADIO_CAP_RIGHT))->EnableWindow(FALSE);
		}

		((CButton *)GetDlgItem (IDC_RADIO_CAP_LEFT))->SetCheck((myCapAlignment == CWindowButton::CaptionAlignmentHLeft)? 1 : 0);
		((CButton *)GetDlgItem (IDC_RADIO_CAP_CENTER))->SetCheck((myCapAlignment == CWindowButton::CaptionAlignmentHCenter)? 1 : 0);
		((CButton *)GetDlgItem (IDC_RADIO_CAP_RIGHT))->SetCheck((myCapAlignment == CWindowButton::CaptionAlignmentHRight)? 1 : 0);
		((CButton *)GetDlgItem (IDC_RADIO_CAP_TOP))->SetCheck((myCapAlignment == CWindowButton::CaptionAlignmentVTop)? 1 : 0);
		((CButton *)GetDlgItem (IDC_RADIO_CAP_MIDDLE))->SetCheck((myCapAlignment == CWindowButton::CaptionAlignmentVMiddle)? 1 : 0);
		((CButton *)GetDlgItem (IDC_RADIO_CAP_BOTTOM))->SetCheck((myCapAlignment == CWindowButton::CaptionAlignmentVBottom)? 1 : 0);
		((CButton *)GetDlgItem (IDC_RADIO_CAP_LEFT2))->SetCheck((myHorIconAlignment == CWindowButton::IconAlignmentHLeft)? 1 : 0);
		((CButton *)GetDlgItem (IDC_RADIO_CAP_CENTER2))->SetCheck((myHorIconAlignment == CWindowButton::IconAlignmentHCenter)? 1 : 0);
		((CButton *)GetDlgItem (IDC_RADIO_CAP_RIGHT2))->SetCheck((myHorIconAlignment == CWindowButton::IconAlignmentHRight)? 1 : 0);
		((CButton *)GetDlgItem (IDC_RADIO_CAP_TOP2))->SetCheck((myVertIconAlignment == CWindowButton::IconAlignmentVTop)? 1 : 0);
		((CButton *)GetDlgItem (IDC_RADIO_CAP_MIDDLE2))->SetCheck((myVertIconAlignment == CWindowButton::IconAlignmentVMiddle)? 1 : 0);
		((CButton *)GetDlgItem (IDC_RADIO_CAP_BOTTOM2))->SetCheck((myVertIconAlignment == CWindowButton::IconAlignmentVBottom)? 1 : 0);
	}
}


BEGIN_MESSAGE_MAP(CDButtonEdit, CDialog)
	//{{AFX_MSG_MAP(CDButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_TEXTCOLOR, OnButtonTextcolor)
	ON_BN_CLICKED(IDC_BUTTON_FACECOLOR, OnButtonFacecolor)
	ON_BN_CLICKED(IDC_BUTTON_ASSIGN_ACTION, OnButtonAssignAction)
	ON_BN_CLICKED(IDC_RADIO_USEWINDEF, OnRadioWinDef)
	ON_BN_CLICKED(IDC_RADIO_USECUSTOM, OnRadioCustom)
	ON_BN_CLICKED(IDC_SETBTNFONT, OnSetFont)
	ON_BN_CLICKED(IDC_RADIO_HORIZONTAL, OnRadioHorizontal)
	ON_BN_CLICKED(IDC_RADIO_VERTICAL, OnRadioVertical)
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(IDC_EDIT_CAPTION, OnChangeEditCaption)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	ON_BN_CLICKED(IDC_BUTTON_SETDIR, OnSetDir)
	ON_BN_CLICKED(IDC_CHECK_CONNENGINE, OnChangeConnEngine)
	ON_BN_CLICKED(IDC_CHECK_SCRIPTING, OnChangeAllowScript)
	ON_BN_CLICKED(IDC_BTN_GROUP_PERMISSIONS, &CDButtonEdit::OnBnClickedGroupPermissions)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDButtonEdit message handlers

CDButtonEdit & CDButtonEdit::operator = (const CWindowButtonExt & cdt)
{
	myShape = cdt.btnAttributes.m_myShape;
	myPattern = cdt.btnAttributes.m_myPattern;
	m_csCaption = cdt.myCaption;
	m_myName = cdt.myName;
	m_csIcon = cdt.m_Icon;
	colorText = cdt.controlAttributes.m_colorText;
	colorFace = cdt.controlAttributes.m_colorFace;
	myAction = cdt.controlAttributes.m_myAction;
	myBtnFont = cdt.controlAttributes.lfControlFont;
	memcpy (&myActionUnion, &cdt.btnAttributes.m_myActionUnion, sizeof (myActionUnion));
	m_usWidthMultiplier = cdt.controlAttributes.m_usWidthMultiplier;
	m_usHeightMultiplier = cdt.controlAttributes.m_usHeightMultiplier;
	myCapAlignment = cdt.btnAttributes.capAlignment;
	myHorIconAlignment = cdt.btnAttributes.horIconAlignment;
	myVertIconAlignment = cdt.btnAttributes.vertIconAlignment;
	horizOrient = cdt.btnAttributes.HorizontalOriented;
	m_GroupPermissions = cdt.controlAttributes.ulMaskGroupPermission;

	for (int i = 0; i < sizeof(cdt.controlAttributes.SpecWin); i++){
		m_csSpecWin[i] = cdt.controlAttributes.SpecWin[i];
	}

	return *this;
}

void CDButtonEdit::OnButtonTextcolor() 
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

void CDButtonEdit::OnButtonFacecolor() 
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

void CDButtonEdit::OnChangeConnEngine()
{
	BOOL isConnEngineChecked = (((CButton*)GetDlgItem (IDC_CHECK_CONNENGINE))->GetCheck() == BST_CHECKED ) ||
								(((CButton*)GetDlgItem (IDC_CHECK_SCRIPTING))->GetCheck() == BST_CHECKED );
	((CEdit *)GetDlgItem (IDC_EDIT_CONNENGINE_BUTTON_NAME))->EnableWindow(isConnEngineChecked);
}

void CDButtonEdit::OnChangeAllowScript()
{
	BOOL isAllowScriptChecked = (((CButton*)GetDlgItem (IDC_CHECK_SCRIPTING))->GetCheck() == BST_CHECKED ) ||
								(((CButton*)GetDlgItem (IDC_CHECK_CONNENGINE))->GetCheck() == BST_CHECKED );
	((CEdit *)GetDlgItem (IDC_EDIT_CONNENGINE_BUTTON_NAME))->EnableWindow(isAllowScriptChecked);
}

void CDButtonEdit::_processAssignAction(CWindowButton::ButtonActionType actionId, int actionTextId)
{
	CString csTemp;
	
	// N-KSo This wipes out previous myActionUnion
	//		 Add a condition not to reset the entire structure if user just changed some variables
	//		 such as myActionUnion.data.extFSC.
	if (myAction != actionTextId)
		memset(&myActionUnion,0,sizeof(myActionUnion));

	CString csStaticAction;
	csStaticAction.LoadString(actionTextId);

	myAction = actionTextId;
	myActionUnion.type = actionId;

	switch (myAction)
	{
	case IDD_P03_CHARACTER_BUTTON:
		m_cCaption.SetWindowText(_T(""));
		_setActionText(&myActionUnion, myAction, false);
		break;
	case IDD_P03_KEYEDPLU: //+
		{
			CDEntryPLU dPlu;
			dPlu.m_csExtMsg.LoadString (IDS_ENTPLUNUM);
			dPlu.m_csExtMnemonics.LoadString (IDS_PLUKEY);
			_copyActionUnion(&dPlu.myActionUnion, &myActionUnion);
			if (dPlu.DoModal() == IDOK)
			{
				_copyActionUnion(&myActionUnion, &dPlu.myActionUnion);
				_setActionText(&myActionUnion, myAction);
			}
		}
		break;

	case IDD_P03_KEYEDDPT:
		{
			// N-KSo Need validation of extFSC
			CDEntryDept dDept;
			dDept.myRangeMin = 1;
			dDept.myRangeMax = STD_MAX_DEPT;
			dDept.m_csExtMsg.LoadString(IDS_ENTEXTFSC);
			dDept.m_csExtMnemonic.LoadString(IDS_DEPTKEY);
			_copyActionUnion(&dDept.myActionUnion, &myActionUnion);
			if (dDept.DoModal() == IDOK)
			{
				_copyActionUnion(&myActionUnion, &dDept.myActionUnion);
				_setActionText(&myActionUnion, myAction);
			}
		}
		break;

	case IDD_P03_OPRINT_A:	
	case IDD_P03_AKEY:		
		myActionUnion.data.AorB = 0;
		_setActionText(&myActionUnion, myAction);
		break;
	case IDD_P03_OPRINT_B:
	case IDD_P03_BKEY:
		myActionUnion.data.AorB = 1; 
		_setActionText(&myActionUnion, myAction);
		break;

	case IDD_P03_GIFT_CARD:	// KSo - This requires special handlings
		{
			CDEntry dEntry;
			_processDEntryDialog(&dEntry, myAction);
			if (dEntry.m_uiExtFsc == 1)
				m_csCaption.LoadString(IDS_BALANCE);
			else if (dEntry.m_uiExtFsc == 2)
				m_csCaption.LoadString(IDS_FREEDOMPAY);
			else
				m_csCaption.LoadStringW(IDD_P03_GIFT_CARD);

			if(((CButton*)GetDlgItem (IDC_CHECK_CUST_CAP))->GetCheck())
				m_cCaption.SetWindowText (m_csCaption);
			m_cStaticAction.SetWindowText (m_csCaption);
		}
		break;

	case IDD_P03_MENUWIND:
		{
			CDWindowList dWndList;
			dWndList.pDoc = pDoc;
			dWndList.forWinBtn = TRUE;
			if (dWndList.DoModal() == IDOK) 
			{
				myActionUnion.data.nWinID = dWndList.uiActiveID;
				_setActionText(&myActionUnion, myAction);
			}
		}
		break;

	case IDD_P03_MENUMODEKEY:
		{
			CModeEntry dModeEntry;
			dModeEntry.m_csExtMnemonic.LoadString(IDS_MODEKEY); 
			dModeEntry.m_csExtMsg.LoadString(IDS_SELECTMODE); 
			dModeEntry.m_iLabel = 0;
			_copyActionUnion(&dModeEntry.myActionUnion, &myActionUnion);
			if (dModeEntry.DoModal() == IDOK)
			{
				_copyActionUnion(&myActionUnion, &dModeEntry.myActionUnion);
				_setActionText(&myActionUnion, myAction);
			}
		}
		break;

	case IDD_P03_NUMBERPAD:
		{
			CKeypadEntry dKeypadEntry; 
			dKeypadEntry.m_csExtMnemonic.LoadString(IDS_NUMPADKEY); 
			dKeypadEntry.m_csExtMsg.LoadString(IDS_SELECTNUM); 
			dKeypadEntry.myActionUnion.data.extFSC = 0;
			_copyActionUnion(&dKeypadEntry.myActionUnion, &myActionUnion);
			if (dKeypadEntry.DoModal() == IDOK)
			{
				_copyActionUnion(&myActionUnion, &dKeypadEntry.myActionUnion);
				this->m_csCaption.Format(_T("%u"), myActionUnion.data.extFSC);
				m_cCaption.SetWindowText(m_csCaption);
				_setActionText(&myActionUnion, myAction, false);
			}
		}
		break;

	case IDD_P03_DEF_KEYPAD:
		_setActionText(&myActionUnion, myAction, false);
		break;

	case IDD_P03_CHARACTER_DELETE:
		m_csCaption.LoadString(IDD_P03_CHARACTER_DELETE);
		if(((CButton*)GetDlgItem (IDC_CHECK_CUST_CAP))->GetCheck())
			m_cCaption.SetWindowText (m_csCaption);

		_setActionText(&myActionUnion, myAction, false);
		break;

	case IDD_P03_WINDOW_DISPLAY:
	case IDD_P03_WINDOW_DISMISS:
		{
			CDEntryWinID dWinID;
			dWinID.m_csExtMsg.LoadString (IDS_ENTWINID);
			dWinID.m_csExtMnemonics.LoadString (IDS_WINID);
			_copyActionUnion(&dWinID.myActionUnion, &myActionUnion);
			if (dWinID.DoModal () == IDOK)
			{
				_copyActionUnion(&myActionUnion, &dWinID.myActionUnion);
				_setActionText(&myActionUnion, myAction);
			}
		}
		break;
	case IDD_P03_PLU_GROUPTABLE:	//+
		{
			CDEntryGroupTable  dGroupTable;
			dGroupTable.m_myTableRangeMin = 0;
			dGroupTable.m_myTableRangeMax = 3; 
			dGroupTable.m_myGroupRangeMin = 1;
			dGroupTable.m_myGroupRangeMax = 9999; 
			dGroupTable.m_csExtMnemonic = _T("Extended Group"); 
			dGroupTable.m_csExtMsg  = _T("Group Index");
			dGroupTable.m_csExtMsg2 = _T("Group Number");

			if (myActionUnion.data.group_table.usGroupNumber == 0)
			{
				myActionUnion.data.group_table.usTableNumber = 0;
				myActionUnion.data.group_table.usGroupNumber = 1;
			}
			_copyActionUnion(&dGroupTable.myActionUnion, &myActionUnion);
			if (dGroupTable.DoModal() == IDOK)
			{
				_copyActionUnion(&myActionUnion, &dGroupTable.myActionUnion);
				_setActionText(&myActionUnion, myAction);
			}
		}
		break;

	case IDD_P03_SUPR_INTRVN:
		// Supervisor Intervention request
		//   keyboard FSC  => FSC_SUPR_INTRVN
	case IDD_P03_OPR_PICKUP:
		// Register Mode Operator Pickup to pickup money from the drawer like AC10
		//   keyboard FSC  => FSC_OPR_PICKUP
	case IDD_P03_ADJ_MOD:
	case IDD_P03_MINIMIZE:
	case IDD_P03_CANCEL:
	default:
		{
			PSDEntryParams pParams = _lookupDEntryParams(myAction);
			if (pParams != NULL)
			{
				CDEntry dEntry;
				_processDEntryDialog(&dEntry, myAction, pParams);				
			}
			else
				_setActionText(&myActionUnion, myAction);
		}
		break;
	}
}

void CDButtonEdit::SetDlgControls(BOOL flag)
{
	m_ButtonShapeRectangle.EnableWindow(!flag);
	m_ButtonShapeElipse.EnableWindow(!flag);
	m_ButtonShapeRoundedRectangle.EnableWindow(!flag);
	m_ButtonPatternNone.EnableWindow(!flag);
	m_ButtonPatternHoriz.EnableWindow(!flag);
	m_ButtonPatternVert.EnableWindow(!flag);
	m_SetFaceColor.EnableWindow(!flag);
	m_ComboWidth.EnableWindow(!flag);
	m_ComboHeight.EnableWindow(!flag);
	m_SetFont.EnableWindow(!flag);
	m_SetGroupPermission.EnableWindow(!flag);
	InvalidateRect (NULL, FALSE);
}

void CDButtonEdit::OnRadioWinDef() 
{
	// TODO: Add your control notification handler code here

	//Button Shape
	m_ButtonShapeRectangle.SetCheck ((df.defaultAttributes.ShapeDefault == CWindowButton::ButtonShapeRect) ? 1 : 0);
	m_ButtonShapeElipse.SetCheck ((df.defaultAttributes.ShapeDefault == CWindowButton::ButtonShapeElipse) ? 1 : 0);
	m_ButtonShapeRoundedRectangle.SetCheck ((df.defaultAttributes.ShapeDefault == CWindowButton::ButtonShapeRoundedRect) ? 1 : 0);
	
	//Button Pattern
	m_ButtonPatternNone.SetCheck ((df.defaultAttributes.PatternDefault == CWindowButton::ButtonPatternNone) ? 1 : 0);
	m_ButtonPatternHoriz.SetCheck ((df.defaultAttributes.PatternDefault == CWindowButton::ButtonPatternHoriz) ? 1 : 0);
	m_ButtonPatternVert.SetCheck ((df.defaultAttributes.PatternDefault == CWindowButton::ButtonPatternVert) ? 1 : 0);
	
	m_usWidthMultiplier = df.defaultAttributes.WidthDefault;
	m_usHeightMultiplier = df.defaultAttributes.HeightDefault;

	((CComboBox *)GetDlgItem (IDC_COMBO_DIALOG1_HEIGHT))->SetCurSel (m_usHeightMultiplier - 1);
	((CComboBox *)GetDlgItem (IDC_COMBO_DIALOG1_WIDTH))->SetCurSel (m_usWidthMultiplier - 1);

	//Color
	m_StaticShowTextColor.m_FillColor = df.defaultAttributes.FontColorDefault;
	m_StaticShowFaceColor.m_FillColor = df.defaultAttributes.ButtonDefault;

	//Font
	myBtnFont = df.defaultAttributes.FontDefault;
	colorText = df.defaultAttributes.FontColorDefault;

	SetDlgControls(TRUE);
}

void CDButtonEdit::OnRadioCustom() 
{
	SetDlgControls(FALSE);
}

void CDButtonEdit::OnSetFont()
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

HBRUSH CDButtonEdit::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
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

void CDButtonEdit::OnRadioHorizontal()
{
	((CButton *)GetDlgItem (IDC_RADIO_HORIZONTAL))->SetCheck(TRUE);
	((CButton *)GetDlgItem (IDC_RADIO_VERTICAL))->SetCheck(FALSE);
	((CButton *)GetDlgItem (IDC_RADIO_CAP_LEFT))->EnableWindow(FALSE);
	((CButton *)GetDlgItem (IDC_RADIO_CAP_CENTER))->EnableWindow(FALSE);
	((CButton *)GetDlgItem (IDC_RADIO_CAP_RIGHT))->EnableWindow(FALSE);
	((CButton *)GetDlgItem (IDC_RADIO_CAP_TOP))->EnableWindow(TRUE);
	((CButton *)GetDlgItem (IDC_RADIO_CAP_MIDDLE))->EnableWindow(TRUE);
	((CButton *)GetDlgItem (IDC_RADIO_CAP_BOTTOM))->EnableWindow(TRUE);
}

void CDButtonEdit::OnRadioVertical()
{
	((CButton *)GetDlgItem (IDC_RADIO_VERTICAL))->SetCheck(TRUE);
	((CButton *)GetDlgItem (IDC_RADIO_HORIZONTAL))->SetCheck(FALSE);
	((CButton *)GetDlgItem (IDC_RADIO_CAP_LEFT))->EnableWindow(TRUE);
	((CButton *)GetDlgItem (IDC_RADIO_CAP_CENTER))->EnableWindow(TRUE);
	((CButton *)GetDlgItem (IDC_RADIO_CAP_RIGHT))->EnableWindow(TRUE);
	((CButton *)GetDlgItem (IDC_RADIO_CAP_TOP))->EnableWindow(FALSE);
	((CButton *)GetDlgItem (IDC_RADIO_CAP_MIDDLE))->EnableWindow(FALSE);
	((CButton *)GetDlgItem (IDC_RADIO_CAP_BOTTOM))->EnableWindow(FALSE);
}

void CDButtonEdit::OnChangeEditCaption() 
{
	if (myActionUnion.type == CWindowButton::BATypeCharacter)
	{
		CString csAddEditCaption;
		CString csStaticAction;
		csStaticAction.LoadString(myAction);
		m_cCaption.GetWindowText(csAddEditCaption);
		if(csAddEditCaption.Compare(_T(" ")) == 0)
		{
			csAddEditCaption.LoadString(IDS_SPACE);
			csStaticAction += _T(": ") + csAddEditCaption;
		}
		m_cStaticAction.SetWindowText(csStaticAction);
	}
	
	CString csTemp;
	GetDlgItemText(IDC_EDIT_CAPTION, csTemp);
	((CEdit *)GetDlgItem (IDC_EDIT_PREVIEW))->SetWindowText(csTemp);
}

BOOL CDButtonEdit::OnInitDialog() 
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

	switch (myActionUnion.type)
	{
	case CWindowButton::BATypeCharacter:
		{
			if (m_csCaption.Compare(_T(" ")) == 0)
			{
				CString charText;
				charText.LoadString(IDS_SPACE);
				csStaticAction.AppendFormat(_T(": %s"), charText);
			}
		}
		break;

	//case CWindowButton::BATypeCharacterDelete:
	//case CWindowButton::BATypeAdj:
	//case CWindowButton::BATypePLU:
	//case CWindowButton::BATypeDept:
	//case CWindowButton::BATypeDisplayLabeledWindow:		//FSC_WINDOW_DISPLAY
	//case CWindowButton::BATypeDismissLabeledWindow:		//FSC_WINDOW_DISMISS
	//case CWindowButton::BATypeWindow:
	//case CWindowButton::BATypePLU_Group:
	//case CWindowButton::BATypePLU_GroupTable:
	default:
		csStaticAction = _getActionText(&myActionUnion, myAction);
		break;
	}
	
	m_cStaticAction.SetWindowText(csStaticAction);
	m_ShowCap.SetWindowText(m_csCaption);
	return TRUE;  // return TRUE unless you set the focus to a control
}


void CDButtonEdit::OnButtonBrowse() 
{
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
		((CEdit *)GetDlgItem (IDC_EDIT_ICONFILENAME))->SetWindowText(m_csIcon);
	}
}

void CDButtonEdit::OnSetDir() 
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

void CDButtonEdit::OnBnClickedGroupPermissions()
{
	DGroupPermission gpDiag(m_GroupPermissions, this);
	gpDiag.DoModal();
	m_GroupPermissions = gpDiag.GetGroupPermissionMask();
}

// A-KSo
void CDButtonEdit::OnButtonAssignAction() 
{
	DButtonAction dlgButtonAction;
	if (myAction != -1)
	{
		dlgButtonAction.m_currentActionId = myActionUnion.type;
		dlgButtonAction.m_currentTextId = myAction;
	}

	int dlgResult = dlgButtonAction.DoModal();
	if (dlgResult == IDOK)
	{
		this->_processAssignAction(dlgButtonAction.m_currentActionId, dlgButtonAction.m_currentTextId);
	}
}

void CDButtonEdit::_copyActionUnion(CWindowButton::ButtonActionUnion *pDst, CWindowButton::ButtonActionUnion *pSrc)
{
	memcpy (pDst, pSrc, sizeof(CWindowButton::ButtonActionUnion));
}

void CDButtonEdit::_setActionText(CWindowButton::ButtonActionUnion *pActionUnion, int actionId, bool fUpdateCaption)
{
	CString actionText = _getActionText(pActionUnion, actionId);
	if (((CButton*)GetDlgItem (IDC_CHECK_CUST_CAP))->GetCheck() &&
		fUpdateCaption == true
		)
		m_cCaption.SetWindowText(actionText);
	m_cStaticAction.SetWindowText(actionText);	
}

SDEntryParams sDEntryParams[] = 
{
//	 ActionTextId,		min,max,			range				extmsg			mnemonic		
	{IDD_P03_PRTMOD,	1,	STD_PRTMOD_MAX, IDS_PRINTMOD_DR,	IDS_ENTEXTFSC,	IDS_PRINTMOD},
	{IDD_P03_ADJ,		1,	STD_ADJ_MAX,	IDS_ADJECTIVE_DR,	IDS_ENTEXTFSC,	IDS_ADJECTIVE},
	{IDD_P03_TENDER,	1,	STD_TENDER_MAX,	IDS_TENDER_DR,		IDS_ENTEXTFSC,	IDS_TENDER},
	{IDD_P03_FC,		1,	STD_FC_MAX,		IDS_FOREIGNCUR_DR,	IDS_ENTEXTFSC,	IDS_FOREIGNCUR},
	{IDD_P03_TTL,		1,	STD_TOTAL_MAX,	IDS_TOTAL_DR,		IDS_ENTEXTFSC,	IDS_TOTAL},
	{IDD_P03_ADDCHK,	1,	STD_ADDCHK_MAX,	IDS_ADDCHECK_DR,	IDS_ENTEXTFSC,	IDS_ADDCHECK},
	{IDD_P03_IDISC,		1,	STD_IDISC_MAX,	IDS_ITEM_DISC_SURCH_DR, IDS_ENTEXTFSC, IDS_ITEM_DISC_SURCH},
	{IDD_P03_RDISC,		1,	STD_RDISC_MAX,	IDS_TRAN_DISC_SURCH_DR, IDS_ENTEXTFSC, IDS_TRAN_DISC_SURCH},
	{IDD_P03_TAXMOD,	1,	STD_TAXMOD_MAX,	IDS_TAXMOD_DR,		IDS_ENTEXTFSC,	IDS_TAXMOD},
	{IDD_P03_PRTDEM,	1,	STD_PRTDMD_MAX,	IDS_POD_DR,			IDS_ENTEXTFSC,	IDS_POD},
	{IDD_P03_CHARTIP,	1,	STD_CHGTIP_MAX,	IDS_CHARGETIPS_DR,	IDS_ENTEXTFSC,	IDS_CHARGETIPS},
	{IDD_P03_PRECASH,	1,	STD_PRECSH_MAX,	IDS_PRESET_CASH_DR,	IDS_ENTEXTFSC,	IDS_PRESET_CASH},
	{IDD_P03_DRCTSHFT,	1,	STD_DRCTSHIFT_MAX, IDS_DIRECTSHIFT_DR, IDS_ENTEXTFSC, IDS_DIRECTSHIFT},
	{IDD_P03_KEYEDCPN,	1,	STD_CPN_MAX,	IDS_COUPONKEY_DR,	IDS_ENTEXTFSC,	IDS_COUPONKEY},
	{IDD_P03_KEYEDSTR,	1,	STD_CS_MAX,		IDS_CONTROLSTRING_DR, IDS_ENTEXTFSC, IDS_CONTROLSTRING},
	{IDD_P03_SEAT,		1,	STD_SEAT_MAX,	IDS_SEATNUM_DR,		IDS_ENTEXTFSC,	IDS_SEATNUM},
	{IDD_P03_ASK,		1,	STD_ASK_MAX,	IDS_ASK_DR,			IDS_ENTEXTFSC,	IDS_ASK},
	{IDD_P03_ADJPLS,	1,	STD_ADJLEV_MAX,	IDS_PRICELEVEL_DR,	IDS_ENTEXTFSC,	IDS_PRICELEVEL},
	{IDD_P03_CURSORVOID,0,	3,				IDS_VOIDRETURN_DR,	IDS_ENTEXTFSC,	IDS_VOIDRETURN},
	{IDD_P03_VTRAN,		0,	3,				IDS_VOIDRETURN_DR,	IDS_ENTEXTFSC,	IDS_VOIDRETURN},
	{IDD_P03_PLU_GROUP,	1,	89,				IDS_ENTPLUGROUP_DR,	IDS_ENTEXTFSC,	IDS_ENTPLUGROUP},
	{IDD_P03_ORDER_DEC,	0,	4,				IDS_ORDER_DEC_DR, IDS_ENTEXTFSC,	IDS_ORDER_DEC},
	{IDD_P03_GIFT_CARD, 1,	STD_GIFT_CRD_MAX, IDS_GIFT_CARD_DR,	IDS_ENTEXTFSC,	IDS_GIFT_CARD},
	{IDD_P03_DOC_LAUNCH,1,	99,				IDS_DOC_LAUNCH_DR,	IDS_ENTEXTFSC,	IDS_DOC_LAUNCH},
	{IDD_P03_CANCEL,    0,	1,				IDS_VOIDRETURN_DR,	IDS_ENTEXTFSC,	IDS_VOIDRETURN},
	{0, 0, 0, 0, 0, 0}
};

void CDButtonEdit::_processDEntryDialog(CDEntry *pDEntry, int actionTextId, PSDEntryParams pParams)
{
	if (pParams == NULL)
		pParams = this->_lookupDEntryParams(actionTextId);
	if (pParams != NULL)
	{
		pDEntry->myRangeMin = pParams->MinRange;
		pDEntry->myRangeMax = pParams->MaxRange;
		pDEntry->m_csExtMnemonic.LoadString(pParams->ExtMnemonicTextId);
		pDEntry->m_csExtMsg.LoadString(pParams->ExtMsgTextId);
		pDEntry->m_csRange.LoadStringW(pParams->RangeTextId);
	}
	_copyActionUnion(&(pDEntry->myActionUnion), &myActionUnion);
	if (pDEntry->DoModal () == IDOK)
	{
		_copyActionUnion(&myActionUnion, &(pDEntry->myActionUnion));
		_setActionText(&myActionUnion, myAction);
	}
}

PSDEntryParams CDButtonEdit::_lookupDEntryParams(int actionTextId)
{
	int	idx = 0;
	PSDEntryParams pRes = NULL;
	PSDEntryParams pParams;

	do 
	{
		pParams = &sDEntryParams[idx++];
		if (pParams->ActionTextId == actionTextId)
		{
			pRes = pParams;
			break;
		}
	} while (pParams->ActionTextId != 0 || pParams->MinRange != 0 || pParams->MaxRange != 0);

	return pRes;
}

CString CDButtonEdit::_getActionText(CWindowButton::ButtonActionUnion *pActionUnion, int actionTextId)
{
	CString actionText;
	actionText.LoadString(actionTextId);

	switch (pActionUnion->type)
	{
	case CWindowButton::BATypeModeKey:
		switch (pActionUnion->data.extFSC)
		{
		case 0: actionText.LoadString(IDS_LOCK); break;
		case 1: actionText.LoadString(IDS_REGISTER); break;
		case 2: actionText.LoadString(IDS_SUPERVISOR); break;
		case 3: actionText.LoadString(IDS_PROGRAM); break;
		}
		break;

	case CWindowButton::BATypeWindow:
		actionText.AppendFormat(_T(": %s"),	_getWindowName(pActionUnion->data.nWinID));
		break;
	case CWindowButton::BATypePLU:
	case CWindowButton::BATypeDept:
	case CWindowButton::BATypeDisplayLabeledWindow:
	case CWindowButton::BATypeDismissLabeledWindow:
		{
			USES_CONVERSION;
			// KSO TODO: Need to convert ASCII to UNICODE (or T)
			LPWSTR pzPlu = A2W((LPCSTR)pActionUnion->data.PLU);
			actionText.AppendFormat(_T(" %s"), pzPlu);
		}
		break;
	case CWindowButton::BATypePLU_GroupTable:
		actionText.AppendFormat(_T(": Idx %d Grp %d"), 
			pActionUnion->data.group_table.usTableNumber, 
			pActionUnion->data.group_table.usGroupNumber);
		break;
	case CWindowButton::BATypeCharacterDelete:
		{
			CString charText;
			charText.LoadString(IDD_P03_CHARACTER_BUTTON);
			actionText = charText + _T(": ") + actionText;
		}
		break;
	case CWindowButton::BATypeCharacter:
		break;
	case CWindowButton::BATypeNumPad:
	case CWindowButton::BATypeOprInt:
	case CWindowButton::BATypeAdj:
	case CWindowButton::BATypePLU_Group:
	default:
		if (pActionUnion->data.extFSC > 0    &&
			actionTextId != IDD_P03_OPRINT_B &&
			actionTextId != IDD_P03_BKEY
			)
			actionText.AppendFormat(_T(": %d"), pActionUnion->data.extFSC);
		break;
	}
	return actionText;
}

CString CDButtonEdit::_getWindowName(UINT windowId)
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