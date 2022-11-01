// DWinItem.cpp : implementation file
//

#include "stdafx.h"
#include "NewLayout.h"
#include "DWinItem.h"
#include "ChildFrm.h"

#include "WindowControl.h"
#include "WindowItemExt.h"

#include "DGroupPermission.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDWinItem dialog


CDWinItem::CDWinItem(CWindowItemExt *pw /* = NULL */,CWnd* pParent /*=NULL*/)
	: CDialog(CDWinItem::IDD, pParent)
{
	memset (m_csSpecWin, 0, sizeof(m_csSpecWin));

	m_csStaticName.LoadString (IDS_STATIC_DIALOG2_WINDOWNAME);

	if (pw) {
		CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
		CChildFrame *pChild = (CChildFrame *) pFrame->GetActiveFrame();
		CNewLayoutView *myView = pChild->GetRightPane();

		pDoc = myView->GetDocument();
		pDoc->DocumentDef.AssignTo(df);
		*this = *pw;  // assign the CWindowItemExt attributes to our object attributes
		temp = pw;
	}
	else{
		//{{AFX_DATA_INIT(CDWinItem)
		m_csCaption = _T("");
		m_csName = _T("");
		//}}AFX_DATA_INIT

		CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
		CChildFrame *pChild = (CChildFrame *) pFrame->GetActiveFrame();
		CNewLayoutView *myView = pChild->GetRightPane();

		pDoc = myView->GetDocument();
		pDoc->DocumentDef.AssignTo(df);

		m_colorFace = df.defaultAttributes.WindowDefault;
		m_csCaption = _T("");
		m_csName = _T("");
		//	m_csStaticCaption.LoadString (IDS_STATIC_DIALOG2_WINDOWCAPTION);
		//m_csStaticName.LoadString (IDS_STATIC_DIALOG2_WINDOWNAME);
		m_usHeightMultiplier = STD_WND_HEIGHT;
		m_usWidthMultiplier = STD_WND_WIDTH;
		temp = NULL;
		m_nGroup = 0;
		NumPicks = 0;
		m_groupPermission = 0;
	}
}


void CDWinItem::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDWinItem)
	DDX_Control(pDX, IDC_COMBO_NO_PICKS, m_nNumPicks);
	DDX_Control(pDX, IDC_COMBO_GROUP, m_ComboGroup);
	DDX_Control(pDX, IDC_BUTTONRESET, m_SetDefaults);
	DDX_Control(pDX, IDC_RADIO_WINDEF, m_UseWinDef);
	DDX_Control(pDX, IDC_RADIO_DOCDEF, m_UseDocDef);
	DDX_Control(pDX, IDC_RADIO_VIRTUAL, m_ButtonVirtual);
	DDX_Control(pDX, IDC_RADIO_POPUP, m_ButtonPopup);
	DDX_Control(pDX, IDC_COMBO_DIALOG2_NO_COLS, m_ComboNoColumns);
	DDX_Control(pDX, IDC_COMBO_DIALOG2_NO_ROWS, m_ComboNoRows);
	DDX_Control(pDX, IDC_STATIC_SHOW_FACECOLOR, m_StaticColorPatchFace);
//	DDX_Text(pDX, IDC_EDIT_WIN_CAPTION, m_csCaption);
	DDX_Text(pDX, IDC_EDIT_WIN_NAME, m_csName);
	DDV_MaxChars(pDX, m_csName, MaxSizeOfMyCaption);
//	DDX_Text(pDX, IDC_STATIC_WIN_CAPTION, m_csStaticCaption);
	DDX_Text(pDX, IDC_STATIC_WIN_NAME, m_csStaticName);
	DDX_Control(pDX, IDC_BTN_SET_GROUP_PERMISSION, m_SetGroupPermission);
	DDX_Text(pDX, IDC_EDIT_BROWSER_URL, m_csWebBrowserUrl);
	//}}AFX_DATA_MAP

	if (m_usHeightMultiplier < 1)
		m_usHeightMultiplier = 1;
	else if(m_usHeightMultiplier >MAX_WND_HEIGHT)
		m_usHeightMultiplier = MAX_WND_HEIGHT;
	if (m_usWidthMultiplier < 1)
		m_usWidthMultiplier = 1;
	else if(m_usWidthMultiplier > MAX_WND_WIDTH)
		m_usWidthMultiplier = MAX_WND_WIDTH;


	if (pDX->m_bSaveAndValidate) {

		m_usHeightMultiplier = ((CComboBox *)GetDlgItem (IDC_COMBO_DIALOG2_NO_ROWS))->GetCurSel () + MIN_WND_HEIGHT;
		m_usWidthMultiplier = ((CComboBox *)GetDlgItem (IDC_COMBO_DIALOG2_NO_COLS))->GetCurSel () + MIN_WND_WIDTH;
		m_colorFace = m_StaticColorPatchFace.m_FillColor;

		m_nVirtual = 1;
		if(m_ButtonPopup.GetCheck()){
			m_nVirtual = 0;
		}
		if(m_UseWinDef.GetCheck()){
			defFlag = 0;
		}
		if(m_UseDocDef.GetCheck()){
			defFlag = 1;
		}

		m_csSpecWin[UserDefaultWinIndex] = 0;
		if(((CButton*)GetDlgItem (IDC_DEFAULT_WINDOW))->GetCheck()) {
			m_csSpecWin[UserDefaultWinIndex] |= UserDefaultWin_FLAG;
		}

		if(((CButton*)GetDlgItem (IDC_CHK_NO_TRANSLATE))->GetCheck()) {
			m_csSpecWin[UserDefaultWinIndex] |= UserDefaultWin_WINTRANSLATE;
		}

		if(((CButton*)GetDlgItem (IDC_CHK_GROUP_SHOWHIDE))->GetCheck()) {
			m_csSpecWin[UserDefaultWinIndex] |= UserDefaultWin_SHOWHIDE_GROUP;
		}

		if(((CButton*)GetDlgItem (IDC_WEB_BROWSER))->GetCheck()) {
			m_csSpecWin[UserDefaultWinIndex] |= UserDefaultWin_WEB_BROWSER;
		}

		if(((CButton*)GetDlgItem (IDC_REPORT_SCROLLABLE))->GetCheck()) {
			m_csSpecWin[UserDefaultWinIndex] |= UserDefaultWin_DOCU_VIEW;
		}

		if(((CButton*)GetDlgItem (IDC_DISPLAY_TRANSLATE))->GetCheck()) {
			m_csSpecWin[UserDefaultWinIndex] |= UserDefaultWin_DISPLAYTRANS;
		}

		if(((CButton*)GetDlgItem (IDC_DISPLAY_ROLLUP_ENABLE))->GetCheck()) {
			m_csCaption = L"Rollup Window Here";
			m_csSpecWin[UserDefaultWinIndex] |= UserDefaultWin_ROLLUP_WIN;
		}

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

		m_csSpecWin[SpecWinIndex_EventTrigger] = 0;
		if (((CButton*)GetDlgItem (IDC_EVENTTRIGGER_ENABLED))->GetCheck()) {

			// Check to see if the Order Declaration event is indicated
			if (((CButton*)GetDlgItem (IDC_EVENTTRIGGER_ORDERDEC))->GetCheck()) {
				m_csSpecWin[SpecWinIndex_EventTrigger] = SpecWinEventTrigger_WId_OrderDec;
			}

			// Indicate that this window is enabled for an event
			m_csSpecWin[SpecWinIndex_EventTrigger] |= SpecWinEventTrigger_Indic;
		}

		m_csSpecWin[SpecWinIndex_ConnEngine] = 0;
		if(((CButton*)GetDlgItem (IDC_CHECK_CONNENGINE))->GetCheck()) {
			m_csSpecWin[SpecWinIndex_ConnEngine] |= SpecWinEventConnEngine_Indic;			
		}
		if(((CButton*)GetDlgItem (IDC_CHECK_SCRIPTING))->GetCheck()) {
			m_csSpecWin[SpecWinIndex_ConnEngine] |= SpecWinEventScript_Indic;			
		}

		// for Dynamic PLU windows we want to be able to describe what adjectives PLUs should be
		// displayed.  what this does is to limit the different versions of an adjective PLU that
		// will display in the window so that a window can be designated as displaying PLUs with
		// a specific size.  for instance if a PLU is Soft Drink with Small, Medium, and Large we
		// can specify that a window is only for displaying the Large size.
		// See also function BlFwIfIntrnlGetGroupPluInformationNext() in GenPOS source code file FrameworkIf.c
		// which performs filtering for Dynamic PLU buttons.
		m_csSpecWin[SpecWinIndex_FILTER_Conditions] = 0;
		if(((CButton*)GetDlgItem (IDC_CHECK_FILTER_ADJ))->GetCheck()) {
			CString csFilterValue;
			((CEdit *)GetDlgItem (IDC_CHECK_FILTER_VALUE))->GetWindowText(csFilterValue);
			m_csSpecWin[SpecWinIndex_FILTER_Conditions] = (_ttoi (csFilterValue) & SpecWinFilter_ValueMask);			
			m_csSpecWin[SpecWinIndex_FILTER_Conditions] |= SpecWinFilter_Adj;			
			int iCurSel = ((CComboBox *)GetDlgItem (IDC_COMBO_FILTER_VALUE_LOGIC))->GetCurSel ();
			m_csSpecWin[SpecWinIndex_FILTER_Conditions] |= ((iCurSel << 6) & SpecWinFilter_AdjValueLogic);
		}
		if(((CButton*)GetDlgItem (IDC_CHECK_FILTER_ADJ_ZERO))->GetCheck()) {
			m_csSpecWin[SpecWinIndex_FILTER_Conditions] |= SpecWinFilter_AdjNonZero;			
		}

		NumPicks = ((CComboBox *)GetDlgItem (IDC_COMBO_NO_PICKS))->GetCurSel();
		UINT index = ((CComboBox *)GetDlgItem (IDC_COMBO_GROUP))->GetCurSel();
		UINT nCount = pDoc->listGroups.GetCount();
		ASSERT(index < nCount);

		CWindowGroup *wg = pDoc->listGroups.GetAt(pDoc->listGroups.FindIndex(index));
		m_nGroup = wg->groupAttributes.GroupID;

	}
	else {
		//populate combo boxes for width and height
		CString csInsert, csGrid;
		csGrid.LoadString(IDS_STRING_GRIDS);

		for( int z = 0; z < MAX_WND_HEIGHT; z++ ){
			csInsert.Format(_T("%d %s"), MIN_BTN_HEIGHT + z, csGrid);
			m_ComboNoRows.AddString(csInsert);
		}

		for( int y = 0; y < MAX_WND_WIDTH; y++ ){
			csInsert.Format(_T("%d %s"), MIN_BTN_WIDTH + y, csGrid);
			m_ComboNoColumns.AddString(csInsert);
		}

		//populate # of picks combo box
		csInsert.LoadString(IDS_USR_DSM);
		m_nNumPicks.AddString(csInsert);

		if(m_csSpecWin[UserDefaultWinIndex] & UserDefaultWin_FLAG){			
			((CButton*)GetDlgItem (IDC_DEFAULT_WINDOW))->SetCheck(BST_CHECKED);
		}
		if(m_csSpecWin[UserDefaultWinIndex] & UserDefaultWin_WEB_BROWSER){			
			((CButton*)GetDlgItem (IDC_WEB_BROWSER))->SetCheck(BST_CHECKED);
		}
		if(m_csSpecWin[UserDefaultWinIndex] & UserDefaultWin_DOCU_VIEW){			
			((CButton*)GetDlgItem (IDC_REPORT_SCROLLABLE))->SetCheck(BST_CHECKED);
		}
		if(m_csSpecWin[UserDefaultWinIndex] & UserDefaultWin_WINTRANSLATE) {
			((CButton*)GetDlgItem (IDC_CHK_NO_TRANSLATE))->SetCheck(BST_CHECKED);
		}
		if(m_csSpecWin[UserDefaultWinIndex] & UserDefaultWin_DISPLAYTRANS) {
			((CButton*)GetDlgItem (IDC_DISPLAY_TRANSLATE))->SetCheck(BST_CHECKED);
		}
		if(m_csSpecWin[UserDefaultWinIndex] & UserDefaultWin_ROLLUP_WIN) {
			((CButton*)GetDlgItem (IDC_DISPLAY_ROLLUP_ENABLE))->SetCheck(BST_CHECKED);
		}

		if (m_groupPermission == 0) {
			m_csSpecWin[UserDefaultWinIndex] &= ~UserDefaultWin_SHOWHIDE_GROUP;
			((CButton*)GetDlgItem (IDC_CHK_GROUP_SHOWHIDE))->EnableWindow (0);
		}

		if(m_csSpecWin[UserDefaultWinIndex] & UserDefaultWin_SHOWHIDE_GROUP) {
			((CButton*)GetDlgItem (IDC_CHK_GROUP_SHOWHIDE))->SetCheck(BST_CHECKED);
		}

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

		((CButton*)GetDlgItem (IDC_CHECK_CONNENGINE))->SetCheck(BST_UNCHECKED);
		if(m_csSpecWin[SpecWinIndex_ConnEngine] & SpecWinEventConnEngine_Indic) {
			((CButton*)GetDlgItem (IDC_CHECK_CONNENGINE))->SetCheck(BST_CHECKED);
		}
		((CButton*)GetDlgItem (IDC_CHECK_SCRIPTING))->SetCheck(BST_UNCHECKED);
		if(m_csSpecWin[SpecWinIndex_ConnEngine] & SpecWinEventScript_Indic) {
			((CButton*)GetDlgItem (IDC_CHECK_SCRIPTING))->SetCheck(BST_CHECKED);
		}

		// for Dynamic PLU windows we want to be able to describe what adjectives PLUs should be
		// displayed.  what this does is to limit the different versions of an adjective PLU that
		// will display in the window so that a window can be designated as displaying PLUs with
		// a specific size.  for instance if a PLU is Soft Drink with Small, Medium, and Large we
		// can specify that a window is only for displaying the Large size.
		// See also function BlFwIfIntrnlGetGroupPluInformationNext() in GenPOS source code file FrameworkIf.c
		// which performs filtering for Dynamic PLU buttons.
		((CButton*)GetDlgItem (IDC_CHECK_FILTER_ADJ))->SetCheck(BST_UNCHECKED);
		((CEdit *)GetDlgItem (IDC_CHECK_FILTER_VALUE))->SetWindowText(_T("0"));
		CString csValueLogic;
		csValueLogic.LoadString(IDS_STRING_ADJ_LOG_EQUAL);
		((CComboBox *)GetDlgItem (IDC_COMBO_FILTER_VALUE_LOGIC))->AddString(csValueLogic);
		csValueLogic.LoadString(IDS_STRING_ADJ_LOG_LESS);
		((CComboBox *)GetDlgItem (IDC_COMBO_FILTER_VALUE_LOGIC))->AddString(csValueLogic);
		csValueLogic.LoadString(IDS_STRING_ADJ_LOG_GREATER);
		((CComboBox *)GetDlgItem (IDC_COMBO_FILTER_VALUE_LOGIC))->AddString(csValueLogic);
		((CComboBox *)GetDlgItem (IDC_COMBO_FILTER_VALUE_LOGIC))->SetCurSel (0);
		if(m_csSpecWin[SpecWinIndex_FILTER_Conditions] & SpecWinFilter_Adj) {
			((CButton*)GetDlgItem (IDC_CHECK_FILTER_ADJ))->SetCheck(BST_CHECKED);
			CString myValue;
			myValue.Format (_T("%d"), (m_csSpecWin[SpecWinIndex_FILTER_Conditions] & SpecWinFilter_ValueMask));
//			((CEdit *)GetDlgItem (IDC_CHECK_FILTER_VALUE))->EnableWindow(TRUE);
			((CEdit *)GetDlgItem (IDC_CHECK_FILTER_VALUE))->SetWindowText(myValue);
			int iCurSel = ((m_csSpecWin[SpecWinIndex_FILTER_Conditions] & SpecWinFilter_AdjValueLogic) >> 6);
			((CComboBox *)GetDlgItem (IDC_COMBO_FILTER_VALUE_LOGIC))->SetCurSel (iCurSel);
		}
		((CButton*)GetDlgItem (IDC_CHECK_FILTER_ADJ_ZERO))->SetCheck(BST_UNCHECKED);
		if(m_csSpecWin[SpecWinIndex_FILTER_Conditions] & SpecWinFilter_AdjNonZero) {
			((CButton*)GetDlgItem (IDC_CHECK_FILTER_ADJ_ZERO))->SetCheck(BST_CHECKED);
		}

		for( int x = 1; x <= MAX_WND_USRDIS; x++ ){
			csInsert.Format(_T("%d"), x );
			m_nNumPicks.AddString(csInsert);
		}

		if (m_usHeightMultiplier < 1)
			m_usHeightMultiplier = 1;
		else if(m_usHeightMultiplier > MAX_WND_HEIGHT)
			m_usHeightMultiplier = MAX_WND_HEIGHT;
		if (m_usWidthMultiplier < 1)
			m_usWidthMultiplier = 1;
		else if(m_usWidthMultiplier >MAX_WND_WIDTH)
			m_usWidthMultiplier = MAX_WND_WIDTH;


		((CComboBox *)GetDlgItem (IDC_COMBO_DIALOG2_NO_ROWS))->SetCurSel (m_usHeightMultiplier - MIN_WND_HEIGHT);
		((CComboBox *)GetDlgItem (IDC_COMBO_DIALOG2_NO_COLS))->SetCurSel (m_usWidthMultiplier - MIN_WND_HEIGHT);

		m_StaticColorPatchFace.m_FillColor = m_colorFace;

		//check to see if the OEP flag is turned on
		//if so, we gray out the options to make a window
		//a nonpop-up or pop up so that the oep window cannot be
		//set to be a pop up. A window that has the OEP display on
		//it cannot be a pop up window. This array is defined in WindowControl.h
		if (m_csSpecWin[SpecWinIndex_OEP] & SpecWinMask_OEP_FLAG){
			m_ButtonVirtual.SetCheck(1);
			GetDlgItem(IDC_RADIO_VIRTUAL)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_POPUP)->EnableWindow(FALSE);
			((CComboBox *)GetDlgItem (IDC_COMBO_NO_PICKS))->EnableWindow(FALSE);
			GetDlgItem (IDC_DEFAULT_WINDOW)->EnableWindow(FALSE);
			((CButton *)GetDlgItem (IDC_DEFAULT_WINDOW))->SetCheck(BST_UNCHECKED);
		}

		if (m_csSpecWin[SpecWinIndex_EventTrigger] & SpecWinEventTrigger_Indic) {
			((CButton *)GetDlgItem (IDC_EVENTTRIGGER_ENABLED))->SetCheck(BST_CHECKED);
			switch (m_csSpecWin[SpecWinIndex_EventTrigger] & SpecWinEventTrigger_Mask) {
				case SpecWinEventTrigger_WId_OrderDec:
					((CButton *)GetDlgItem (IDC_EVENTTRIGGER_ORDERDEC))->SetCheck(BST_CHECKED);
					break;
				default:
					break;
			}
		}
		OnEventTriggerEnabled();  // Setup the dialog appropriately for EventTrigger

		if(m_nVirtual){
			m_ButtonVirtual.SetCheck(1);
			((CComboBox *)GetDlgItem (IDC_COMBO_NO_PICKS))->EnableWindow(FALSE);
		}
		else{
			m_ButtonPopup.SetCheck(1);
			((CComboBox *)GetDlgItem (IDC_COMBO_NO_PICKS))->EnableWindow(TRUE);
			((CComboBox *)GetDlgItem (IDC_COMBO_NO_PICKS))->SetCurSel(NumPicks);
		}

		if(defFlag){
			m_UseDocDef.SetCheck(1);
		}
		else{
			m_UseWinDef.SetCheck(1);
		}

		if (((CButton *)GetDlgItem (IDC_DEFAULT_WINDOW))->GetCheck()){
			m_ButtonVirtual.SetCheck(1);
			m_ButtonPopup.SetCheck(0);
			GetDlgItem(IDC_RADIO_VIRTUAL)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_POPUP)->EnableWindow(FALSE);
			((CComboBox *)GetDlgItem (IDC_COMBO_NO_PICKS))->EnableWindow(FALSE);
		}
		/* No 'else' needed.  Window properties should not be reset just
		//  because the 'default window' is not checked - CSMALL 10/07/05
		else
		{
			m_ButtonVirtual.SetCheck(1);
			m_ButtonPopup.SetCheck(0);
			GetDlgItem(IDC_RADIO_VIRTUAL)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_POPUP)->EnableWindow(TRUE);
			((CComboBox *)GetDlgItem (IDC_COMBO_NO_PICKS))->EnableWindow(TRUE);
		}
		*/
		m_SetDefaults.EnableWindow(!defFlag);

		if (((CButton *)GetDlgItem (IDC_WEB_BROWSER))->GetCheck()) {
			((CEdit *)GetDlgItem (IDC_EDIT_BROWSER_URL))->EnableWindow(TRUE);
		}
		else
		{
			((CEdit *)GetDlgItem (IDC_EDIT_BROWSER_URL))->EnableWindow(FALSE);
		}
	}
}

CDWinItem::CDWinItem (int row, int column, int width , int height , CWnd* pParent)
: m_nRow(row), m_nColumn(column), m_usWidthMultiplier(width), m_usHeightMultiplier(height), CDialog(CDWinItem::IDD, pParent)
{
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CChildFrame *pChild = (CChildFrame *) pFrame->GetActiveFrame();
	CNewLayoutView *myView = pChild->GetRightPane();

	pDoc = myView->GetDocument();
	pDoc->DocumentDef.AssignTo(df);

	m_colorFace = df.defaultAttributes.WindowDefault;
	m_csCaption = _T("");
	m_csName = _T("");
//	m_csStaticCaption.LoadString (IDS_STATIC_DIALOG2_WINDOWCAPTION);
	m_csStaticName.LoadString (IDS_STATIC_DIALOG2_WINDOWNAME);
	m_usHeightMultiplier = STD_WND_HEIGHT;
	m_usWidthMultiplier = STD_WND_WIDTH;
	temp = NULL;
	m_nGroup = 0;
	NumPicks = 0;
	m_groupPermission = 0;
	memset (m_csSpecWin, 0, sizeof(m_csSpecWin));
}

BEGIN_MESSAGE_MAP(CDWinItem, CDialog)
	//{{AFX_MSG_MAP(CDWinItem)
	ON_BN_CLICKED(IDC_DIALOG2_BUTTON_COLOR, OnDialog2ButtonColor)
	ON_BN_CLICKED(IDC_BUTTONRESET, OnResetWinDefaults)
	ON_BN_CLICKED(IDC_RADIO_DOCDEF, OnRadioDocDef)
	ON_BN_CLICKED(IDC_RADIO_WINDEF, OnRadioWinDef)
	ON_BN_CLICKED(IDC_RADIO_POPUP, OnRadioPopup)
	ON_BN_CLICKED(IDC_RADIO_VIRTUAL, OnRadioVirtual)
	ON_BN_CLICKED(IDC_DEFAULT_WINDOW, OnDefaultWindow)
	ON_BN_CLICKED(IDC_EVENTTRIGGER_ENABLED, OnEventTriggerEnabled)
	ON_BN_CLICKED(IDC_BTN_SET_GROUP_PERMISSION, &CDWinItem::OnBnClickedSetGroupPermission)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_WEB_BROWSER, &CDWinItem::OnBnClickedWebBrowser)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDWinItem message handlers

/*
	Assignment operator to assign the values of the object attributes
	from a CWindowItemExt to a CDWinItem dialog object.  This will
	fill in the attributes of the dialog for when the dialog box is
	displayed to the user.
 */
CDWinItem & CDWinItem::operator = (const CWindowItemExt & cdt)
{
	m_nRow = cdt.controlAttributes.m_nRow;
	m_nColumn = cdt.controlAttributes.m_nColumn;
	m_usWidthMultiplier = cdt.controlAttributes.m_usWidthMultiplier;
	m_usHeightMultiplier = cdt.controlAttributes.m_usHeightMultiplier;
	m_csCaption = cdt.myCaption;
	m_csName = cdt.myName;

	for (int i = 0; i < sizeof(cdt.controlAttributes.SpecWin); i++){
		m_csSpecWin[i] = cdt.controlAttributes.SpecWin[i];
	}

	m_colorFace = cdt.controlAttributes.m_colorFace;
	m_colorText = cdt.controlAttributes.m_colorText;
	m_nVirtual = cdt.controlAttributes.isVirtualWindow;
	defFlag = cdt.controlAttributes.useDefault;

	m_nGroup = cdt.windowAttributes.GroupNum;
	NumPicks = cdt.windowAttributes.busRules.NumOfPicks;
	if (NumPicks > 100)
		NumPicks = 0;
	m_groupPermission = cdt.controlAttributes.ulMaskGroupPermission;
	m_csWebBrowserUrl = cdt.windowAttributes.webbrowser.url;

	//  Lets do a rough check to see that the sizes have reasonable
	//  values to them.  These are rough outer bounds for these
	//  variables.
	ASSERT(m_nRow < /*54*/MAX_WND_WIDTH + 11); // add 11 for 1024x768 resolution
	ASSERT(m_nColumn < /*41*/MAX_WND_HEIGHT + 15); // add 15 for 1024x768 resolution
	ASSERT(m_usWidthMultiplier < /*54*/MAX_WND_WIDTH + 10);
	ASSERT(m_usHeightMultiplier < /*41*/MAX_WND_HEIGHT + 10);

	return *this;
}

void CDWinItem::OnDialog2ButtonColor() 
{
	// TODO: Add your control notification handler code here
	CColorDialog dlg;
	dlg.m_cc.Flags |= CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT;
	dlg.m_cc.rgbResult = m_StaticColorPatchFace.m_FillColor;
	dlg.m_cc.lpCustColors = myCustomColors;
	if (dlg.DoModal() == IDOK) {
		m_StaticColorPatchFace.m_FillColor = dlg.m_cc.rgbResult;
		CStaticColorPatch *xx = (CStaticColorPatch *) GetDlgItem (IDC_STATIC_SHOW_FACECOLOR);
		xx->m_FillColor = m_StaticColorPatchFace.m_FillColor;
		xx->Invalidate (FALSE);
	}
}


BOOL CDWinItem::OnInitDialog() 
{
	// TODO: Add extra initialization here
	CDialog::OnInitDialog();
	UINT id = 0;

	int nCount = pDoc->listGroups.GetCount();
	for (int i=0;i < nCount;i++)
	{
		CWindowGroup *wg = pDoc->listGroups.GetAt(pDoc->listGroups.FindIndex(i));
	   ((CComboBox *)GetDlgItem (IDC_COMBO_GROUP))->AddString( wg->GroupName );
	   if(temp && (temp->windowAttributes.GroupNum == wg->groupAttributes.GroupID)){
		   id = /*wg->GroupID*/i;
	   }
	}

	((CComboBox *)GetDlgItem (IDC_COMBO_GROUP))->SetCurSel(id);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDWinItem::OnResetWinDefaults() 
{	
	//get main frame window
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	//get child of main frame window...current document that is being worked on
	CChildFrame *pChild = (CChildFrame *) pFrame->GetActiveFrame();
	//get handle for right side of layout
	CNewLayoutView *myView = pChild->GetRightPane();

	if(temp){
		
		
		BOOL newWin = FALSE;										//changing an EXISTING window so this flag is set to false
		myView->PopupDefaultWinDlg(newWin,&(temp->WindowDef));		//call function to change window defaults
		
		//cycle through the controls in this window and set new attributes if they use defualts
		CWindowControl *pwc = temp->GetDataFirst ();
		while(pwc){
			switch(pwc->controlAttributes.m_nType){
			case CWindowControl::TextControl:		//text
				if(pwc->controlAttributes.useDefault){
					pwc->controlAttributes.m_colorFace = temp->WindowDef.defaultAttributes.TextWinDefault;
				}
				break;
			case CWindowControl::ButtonControl:		//button
				if(pwc->controlAttributes.useDefault){
					CWindowButton *wb = CWindowButton::TranslateCWindowControl (pwc);
					wb->controlAttributes.m_colorFace = temp->WindowDef.defaultAttributes.ButtonDefault;
					wb->controlAttributes.m_colorText = temp->WindowDef.defaultAttributes.FontColorDefault;
					wb->controlAttributes.m_usHeightMultiplier = temp->WindowDef.defaultAttributes.HeightDefault;
					wb->controlAttributes.m_usWidthMultiplier = temp->WindowDef.defaultAttributes.WidthDefault;
					wb->btnAttributes.m_myShape = temp->WindowDef.defaultAttributes.ShapeDefault;
					wb->btnAttributes.m_myPattern = temp->WindowDef.defaultAttributes.PatternDefault;

				}
				break;
			}
			pwc = temp->GetDataNext ();
		}

	}
	else
	{
		//get main frame window
		CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
		//get child of main frame window...current document that is being worked on
		CChildFrame *pChild = (CChildFrame *) pFrame->GetActiveFrame();
		//get handle for right side of layout
		CNewLayoutView *myView = pChild->GetRightPane();

		BOOL newWin = FALSE;										//changing an EXISTING window so this flag is set to false
		myView->PopupDefaultWinDlg(newWin,&df);
	}
	myView->InvalidateRect(NULL,TRUE);
}



void CDWinItem::OnRadioDocDef() 
{
	// TODO: Add your control notification handler code here
	m_SetDefaults.EnableWindow(FALSE);

	//get main frame window
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	//get child of main frame window...current document that is being worked on
	CChildFrame *pChild = (CChildFrame *) pFrame->GetActiveFrame();
	//get handle for right side of layout
	CNewLayoutView *myView = pChild->GetRightPane();

	DefaultData ddf;// = &((myView->GetDocument())->DocumentDef);
	((myView->GetDocument())->DocumentDef).AssignTo(ddf);

	if(temp){
		
		ddf.AssignTo(temp->WindowDef);

		//cycle through the controls in this window and set new attributes if they use defualts
		CWindowControl *pwc = temp->GetDataFirst ();
		while(pwc){
			switch(pwc->controlAttributes.m_nType){
			case CWindowControl::TextControl:		//text
				if(pwc->controlAttributes.useDefault){
					pwc->controlAttributes.m_colorFace = temp->WindowDef.defaultAttributes.TextWinDefault;
				}
				break;
			case CWindowControl::ButtonControl:		//button
				if(pwc->controlAttributes.useDefault){
					CWindowButton *wb = CWindowButton::TranslateCWindowControl (pwc);
					wb->controlAttributes.m_colorFace = temp->WindowDef.defaultAttributes.ButtonDefault;
					wb->controlAttributes.m_colorText = temp->WindowDef.defaultAttributes.FontColorDefault;
					wb->controlAttributes.m_usHeightMultiplier = temp->WindowDef.defaultAttributes.HeightDefault;
					wb->controlAttributes.m_usWidthMultiplier = temp->WindowDef.defaultAttributes.WidthDefault;
					wb->btnAttributes.m_myShape = temp->WindowDef.defaultAttributes.ShapeDefault;
					wb->btnAttributes.m_myPattern = temp->WindowDef.defaultAttributes.PatternDefault;
				}
				break;
			}
			pwc = temp->GetDataNext ();
		}

	}
	else{
		ddf.AssignTo(df);
	}
	myView->InvalidateRect(NULL,TRUE);
}

void CDWinItem::OnRadioWinDef() 
{
	// TODO: Add your control notification handler code here
	m_SetDefaults.EnableWindow(TRUE);
}


void CDWinItem::OnRadioPopup() 
{
	// TODO: Add your control notification handler code here
	((CComboBox *)GetDlgItem (IDC_COMBO_NO_PICKS))->EnableWindow(TRUE);
	((CComboBox *)GetDlgItem (IDC_COMBO_NO_PICKS))->SetCurSel(NumPicks);
	
}

void CDWinItem::OnRadioVirtual() 
{
	// TODO: Add your control notification handler code here
	((CComboBox *)GetDlgItem (IDC_COMBO_NO_PICKS))->EnableWindow(FALSE);
	
}

void CDWinItem::OnDefaultWindow() 
{
	// TODO: Add your control notification handler code here
	if (((CButton *)GetDlgItem (IDC_DEFAULT_WINDOW))->GetCheck()){
		m_ButtonVirtual.SetCheck(1);
		m_ButtonPopup.SetCheck(0);
		GetDlgItem(IDC_RADIO_VIRTUAL)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_POPUP)->EnableWindow(FALSE);
		((CComboBox *)GetDlgItem (IDC_COMBO_NO_PICKS))->EnableWindow(FALSE);
	}
	else
	{
		m_ButtonVirtual.SetCheck(1);
		m_ButtonPopup.SetCheck(0);
		GetDlgItem(IDC_RADIO_VIRTUAL)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_POPUP)->EnableWindow(TRUE);
		((CComboBox *)GetDlgItem (IDC_COMBO_NO_PICKS))->EnableWindow(FALSE);
	}
}


void CDWinItem::OnEventTriggerEnabled() 
{
	// TODO: Add your control notification handler code here
	if (((CButton *)GetDlgItem (IDC_EVENTTRIGGER_ENABLED))->GetCheck()) {
		((CButton *)GetDlgItem (IDC_EVENTTRIGGER_ORDERDEC))->EnableWindow(TRUE);
	}
	else
	{
		((CButton *)GetDlgItem (IDC_EVENTTRIGGER_ORDERDEC))->EnableWindow(FALSE);
	}
}

void CDWinItem::OnBnClickedSetGroupPermission()
{
	DGroupPermission gpDiag(m_groupPermission, this);
	gpDiag.DoModal();
	m_groupPermission = gpDiag.GetGroupPermissionMask();
	if (m_groupPermission == 0) {
		m_csSpecWin[UserDefaultWinIndex] &= ~UserDefaultWin_SHOWHIDE_GROUP;
		((CButton*)GetDlgItem (IDC_CHK_GROUP_SHOWHIDE))->SetCheck (0);
		((CButton*)GetDlgItem (IDC_CHK_GROUP_SHOWHIDE))->EnableWindow (0);
	} else {
		((CButton*)GetDlgItem (IDC_CHK_GROUP_SHOWHIDE))->EnableWindow (1);
	}
}

void CDWinItem::OnBnClickedWebBrowser()
{
	if (((CButton *)GetDlgItem (IDC_WEB_BROWSER))->GetCheck()) {
		((CEdit *)GetDlgItem (IDC_EDIT_BROWSER_URL))->EnableWindow(TRUE);
	}
	else
	{
		((CEdit *)GetDlgItem (IDC_EDIT_BROWSER_URL))->EnableWindow(FALSE);
	}
}
