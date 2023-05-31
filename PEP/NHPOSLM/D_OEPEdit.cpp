// D_OEPEdit.cpp : implementation file
//

#include "stdafx.h"
#include "NewLayout.h"
#include "D_OEPEdit.h"
//#include "WindowItem.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CD_OEPEdit dialog


CD_OEPEdit::CD_OEPEdit(CWindowTextExt *pb, CWindowItemExt *pParent /*=NULL*/)
	: CDialog(CD_OEPEdit::IDD, pParent)
{
	ASSERT( sizeof(m_csSpecWin) == sizeof(pb->controlAttributes.SpecWin) );

	if (pb) {
		CWindowItem *wi  =(CWindowItem*)pParent;
		wi->WindowDef.AssignTo(df);
		*this = *pb;

		// We do not need the CWindowTextExt objects SpecWin array.  We use
		// the CWindowTextExt parent window's SpecWin array which is a CWindowItemExt.
		ASSERT( sizeof(m_csSpecWin) == sizeof(pParent->controlAttributes.SpecWin) );

		for (int i = 0; i < sizeof(m_csSpecWin)/sizeof(m_csSpecWin[0]); i++) {
			m_csSpecWin[i] = pParent->controlAttributes.SpecWin[i];
		}
	}
	else {

		//pull in parent windows defaults for control and set attributes to defaults
		CWindowItem *wi  =(CWindowItem*)pParent;
		wi->WindowDef.AssignTo(df);

		myID = CWindowText::TypeOEP;
		mRow =0;
		mColumn =0;
		m_usHeightMultiplier =1;
		m_usWidthMultiplier =1;
		colorFace = df.defaultAttributes.TextWinDefault; 
		colorText = df.defaultAttributes.FontColorDefault;
		myCaption = _T("");
		myName = _T("");
		myTextFont = df.defaultAttributes.FontDefault;
		
		m_BtnDown = 7;
		m_BtnAcross = 4;
		m_usBtnWidth = df.defaultAttributes.WidthDefault;;
		m_usBtnHeight = df.defaultAttributes.HeightDefault;;
		btnColorFace = df.defaultAttributes.ButtonDefault;
		btnFontColor = df.defaultAttributes.FontColorDefault;
		myShape = df.defaultAttributes.ShapeDefault;
		myPattern = df.defaultAttributes.PatternDefault;
		myBtnFont = df.defaultAttributes.FontDefault;

		//Additions made for Multiple-OEP Window Feature - CSMALL 1/11/06
		m_OEPGroupNumbers = _T("");
		memset (m_csSpecWin, 0, sizeof(m_csSpecWin));
		m_csSpecWin[SpecWinIndex_OEP] |= SpecWinMask_OEP_FLAG;

		//added icon support to buttons
		m_nOEPIconHorizRadioSelection	= CWindowButton::IconAlignmentHCenter;
		m_nOEPIconVertRadioSelection	= CWindowButton::IconAlignmentVMiddle;
//		m_csOEPIconFileName = _T("");
//		m_csOEPIconFilePathName = NULL:	//ignore for now, set when user interacts
	}

	//{{AFX_DATA_INIT(CD_OEPEdit)
	m_nOEPHeight = -1;
	m_nOEPWidth = -1;
	//}}AFX_DATA_INIT
}


void CD_OEPEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CD_OEPEdit)
	DDX_Control(pDX, IDC_OEP_SHAPE_RECT, m_buttonOEPShapeRect);
	DDX_Control(pDX, IDC_OEP_SHAPE_ELIPSE, m_buttonOEPShapeElipse);
	DDX_Control(pDX, IDC_OEP_SHAPE_ROUNDRECT, m_buttonOEPShapeRoundRect);
	DDX_Control(pDX, IDC_OEP_PATTERN_NONE, m_buttonOEPPattNone);
	DDX_Control(pDX, IDC_OEP_PATTERN_VERT, m_buttonOEPPattVert);
	DDX_Control(pDX, IDC_OEP_PATTERN_HORIZ, m_buttonOEPPattHoriz);
	DDX_Control(pDX, IDC_OEP_FACECOLOR, m_buttonOEPFaceColor);
	DDX_Control(pDX, IDC_COMBO_OEP_WIDTH, m_comboOEPWidth);
	DDX_Control(pDX, IDC_COMBO_OEP_HEIGHT, m_comboOEPHeight);
	DDX_Control(pDX, IDC_STATIC_SHOW_OEP_FACECOLOR,m_staticOEPBtnColor);
	DDX_Control(pDX, IDC_STATIC_SHOW_OEP_WINCOLOR,m_staticOEPWinColor);
	DDX_CBIndex(pDX, IDC_COMBO_OEP_HEIGHT, m_nOEPHeight);
	DDX_CBIndex(pDX, IDC_COMBO_OEP_WIDTH, m_nOEPWidth);
	//Additions made for Multiple-OEP Window Feature - CSMALL 1/11/06
	DDX_Text(pDX, IDC_EDIT1_OEPGroupNumbers, m_OEPGroupNumbers);
	DDX_Text(pDX, IDC_OEP_ICON_FILENAME_EDIT, m_csOEPIconFileName);
	DDX_Radio(pDX, IDC_OEP_ICON_HOR_LEFT, m_nOEPIconHorizRadioSelection);
	DDX_Radio(pDX, IDC_OEP_ICON_VERT_TOP, m_nOEPIconVertRadioSelection);
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate) {

		//Button Shape
		myShape = CWindowButton::ButtonShapeRect;

		if (m_buttonOEPShapeElipse.GetCheck ()) {
			myShape = CWindowButton::ButtonShapeElipse;
		}
		else if (m_buttonOEPShapeRoundRect.GetCheck ()) {
			myShape = CWindowButton::ButtonShapeRoundedRect;
		}

		//Button Pattern
		myPattern = CWindowButton::ButtonPatternNone;

		if (m_buttonOEPPattHoriz.GetCheck ()) {
			myPattern = CWindowButton::ButtonPatternHoriz;
		}
		else if (m_buttonOEPPattVert.GetCheck ()) {
			myPattern = CWindowButton::ButtonPatternVert;
		}
		
		//Button Width and Height
		m_usBtnWidth = m_nOEPWidth + 1;
		if (m_usBtnWidth < 1)
			m_usBtnWidth = 1;
		else if (m_usBtnWidth > 8)
			m_usBtnWidth = 8;

		m_usBtnHeight = m_nOEPHeight + 1;;
		if (m_usBtnHeight < 1)
			m_usBtnHeight = 1;
		else if (m_usBtnHeight > 8)
			m_usBtnHeight = 8;

		m_BtnDown = ((CComboBox *)GetDlgItem (IDC_COMBO_OEP_BTN_DOWN))->GetCurSel () + 2;
		m_BtnAcross =((CComboBox *)GetDlgItem (IDC_COMBO_OEP_BTN_ACROSS))->GetCurSel () + 1;

		//ASSERT((m_BtnDown -1) * m_BtnAcross >= 21); /* removed for testing with implementing smaller OEP window sizes - CSMALL */
		
		//Color
		btnColorFace = m_staticOEPBtnColor.m_FillColor;
		colorFace = m_staticOEPWinColor.m_FillColor;

		m_usWidthMultiplier = m_BtnAcross * m_usBtnWidth;
		m_usHeightMultiplier = m_BtnDown * m_usBtnHeight;
		
		//Additions made for Multiple-OEP Window Feature - CSMALL 1/11/06
		//m_OEPGroupID = ((CComboBox *)GetDlgItem (IDC_CB_OEP_GroupID))->GetCurSel () + 1;
		((CEdit *)GetDlgItem(IDC_EDIT1_OEPGroupNumbers))->GetWindowText(m_OEPGroupNumbers);
		//m_usWidthMultiplier = ((CComboBox *)GetDlgItem (IDC_COMBO_OEP_WinGridWidth))->GetCurSel () + 1;
		//m_usHeightMultiplier = ((CComboBox *)GetDlgItem (IDC_COMBO_OEP_WinGridHeight))->GetCurSel () + 1;

		// Check the state of the auto OEP window checkbox and set the appropriate
		// bit in the Special Window attributes area that will be copied into the
		// window's controlattributes.SpecWin[] array if the user chooses OK.
		if (((CButton *)GetDlgItem (IDC_CHECK_AUTO_OEP))->GetCheck()) {
			m_csSpecWin[SpecWinIndex_OEP] |= SpecWinMask_AUTO_OEP_FLAG;
		}
		else {
			m_csSpecWin[SpecWinIndex_OEP] &= ~SpecWinMask_AUTO_OEP_FLAG;
		}
		if (((CButton *)GetDlgItem (IDC_CHECK_SPECIAL_OEP))->GetCheck()) {
			m_csSpecWin[SpecWinIndex_OEP] |= SpecWinMask_SPCL_OEP_FLAG;
		}
		else {
			m_csSpecWin[SpecWinIndex_OEP] &= ~SpecWinMask_SPCL_OEP_FLAG;
		}

		// Most UserDefaultWin type flags do not apply to OEP Windows since they
		// are only displayed when needed however the following do apply.
		if(((CButton*)GetDlgItem (IDC_CHK_NO_TRANSLATE))->GetCheck()) {
			m_csSpecWin[UserDefaultWinIndex] |= UserDefaultWin_WINTRANSLATE;
		} else {
			m_csSpecWin[UserDefaultWinIndex] &= ~UserDefaultWin_WINTRANSLATE;
		}
	}
	else {
		// TODO: Add your control notification handler code here
		
		//Additions made for Multiple-OEP Window Feature - CSMALL 1/11/06
		((CEdit *)GetDlgItem (IDC_EDIT1_OEPGroupNumbers))->LimitText (25);

		//Button Shape
		m_buttonOEPShapeRect.SetCheck ((myShape == CWindowButton::ButtonShapeRect) ? 1 : 0);
		m_buttonOEPShapeElipse.SetCheck ((myShape == CWindowButton::ButtonShapeElipse) ? 1 : 0);
		m_buttonOEPShapeRoundRect.SetCheck ((myShape == CWindowButton::ButtonShapeRoundedRect) ? 1 : 0);
		
		//Button Pattern
		m_buttonOEPPattNone.SetCheck ((myPattern == CWindowButton::ButtonPatternNone) ? 1 : 0);
		m_buttonOEPPattHoriz.SetCheck ((myPattern == CWindowButton::ButtonPatternHoriz) ? 1 : 0);
		m_buttonOEPPattVert.SetCheck ((myPattern == CWindowButton::ButtonPatternVert) ? 1 : 0);
		
		//Button Width and Height --> verify value is between 1 and 8
		if (m_usBtnWidth < 1)
			m_usBtnWidth = 1;
		else if (m_usBtnWidth > 8)
			m_usBtnWidth = 8;

		if (m_usBtnHeight < 1)
			m_usBtnHeight = 1;
		else if (m_usBtnHeight > 8)
			m_usBtnHeight = 8;

		((CComboBox *)GetDlgItem (IDC_COMBO_OEP_HEIGHT))->SetCurSel (m_usBtnHeight - 1);
		((CComboBox *)GetDlgItem (IDC_COMBO_OEP_WIDTH))->SetCurSel (m_usBtnWidth - 1);

		((CComboBox *)GetDlgItem (IDC_COMBO_OEP_BTN_ACROSS))->SetCurSel (m_BtnAcross - 1);
		((CComboBox *)GetDlgItem (IDC_COMBO_OEP_BTN_DOWN))->SetCurSel (m_BtnDown - 2);
		CalculateWinDimensions();
			
		//Color
		m_staticOEPBtnColor.m_FillColor = btnColorFace;
		m_staticOEPWinColor.m_FillColor = colorFace;

		//Additions made for Multiple-OEP Window Feature - CSMALL 1/11/06
		//((CComboBox *)GetDlgItem (IDC_CB_OEP_GroupID))->SetCurSel (m_OEPGroupID - 1);
		((CEdit *)GetDlgItem (IDC_EDIT1_OEPGroupNumbers))->SetWindowText (m_OEPGroupNumbers);

		//Window width and height

		// Most UserDefaultWin type flags do not apply to OEP Windows since they
		// are only displayed when needed however the following do apply.
		if(m_csSpecWin[UserDefaultWinIndex] & UserDefaultWin_WINTRANSLATE) {
			((CButton*)GetDlgItem (IDC_CHK_NO_TRANSLATE))->SetCheck(1);
		}

		// if the auto OEP window flag is set for this OEP window, indicate that fact
		// by checking the checkbox.
		if (m_csSpecWin[SpecWinIndex_OEP] & SpecWinMask_AUTO_OEP_FLAG) {
			((CButton *)GetDlgItem (IDC_CHECK_AUTO_OEP))->SetCheck(BST_CHECKED);
		}
		if (m_csSpecWin[SpecWinIndex_OEP] & SpecWinMask_SPCL_OEP_FLAG) {
			((CButton *)GetDlgItem (IDC_CHECK_SPECIAL_OEP))->SetCheck(BST_CHECKED);
			if (_tcsncmp (m_OEPGroupNumbers, SPECWIN_SPCL_OEPGROUP_STR_PREAUTH, _tcslen(SPECWIN_SPCL_OEPGROUP_STR_PREAUTH)) == 0) {
				((CEdit *)GetDlgItem (IDC_EDIT1_OEPGroupNumbers))->SetWindowText (SPECWIN_SPCL_OEPGROUP_STR_PREAUTH);
				((CButton *)GetDlgItem (IDC_RADIO_OEP_SPL_PREAUTH))->SetCheck(BST_CHECKED);
			} else if (_tcsncmp (m_OEPGroupNumbers, SPECWIN_SPCL_OEPGROUP_STR_REASONCODE, _tcslen(SPECWIN_SPCL_OEPGROUP_STR_REASONCODE)) == 0) {
				((CEdit *)GetDlgItem (IDC_EDIT1_OEPGroupNumbers))->SetWindowText (SPECWIN_SPCL_OEPGROUP_STR_REASONCODE);
				((CButton *)GetDlgItem (IDC_RADIO_OEP_SPL_REASONCODE))->SetCheck(BST_CHECKED);
			} else if (_tcsncmp (m_OEPGroupNumbers, SPECWIN_SPCL_OEPGROUP_STR_AUTOCOUPON, _tcslen(SPECWIN_SPCL_OEPGROUP_STR_AUTOCOUPON)) == 0) {
				((CEdit *)GetDlgItem (IDC_EDIT1_OEPGroupNumbers))->SetWindowText (SPECWIN_SPCL_OEPGROUP_STR_AUTOCOUPON);
				((CButton *)GetDlgItem (IDC_RADIO_OEP_SPL_AUTOCOUPON))->SetCheck(BST_CHECKED);
			} else if (_tcsncmp (m_OEPGroupNumbers, SPECWIN_SPCL_OEPGROUP_STR_OPENCHECK, _tcslen(SPECWIN_SPCL_OEPGROUP_STR_OPENCHECK)) == 0) {
				((CEdit *)GetDlgItem (IDC_EDIT1_OEPGroupNumbers))->SetWindowText (SPECWIN_SPCL_OEPGROUP_STR_OPENCHECK);
				((CButton *)GetDlgItem (IDC_RADIO_OEP_SPL_OPENCHECK))->SetCheck(BST_CHECKED);
			} else {
				((CEdit *)GetDlgItem (IDC_EDIT1_OEPGroupNumbers))->SetWindowText (SPECWIN_SPCL_OEPGROUP_STR_REASONCODE);
				((CButton *)GetDlgItem (IDC_RADIO_OEP_SPL_REASONCODE))->SetCheck(BST_CHECKED);
			}
		}
		OnBnClickedSpecialOep();
	}
}

BEGIN_MESSAGE_MAP(CD_OEPEdit, CDialog)
	//{{AFX_MSG_MAP(CD_OEPEdit)
	ON_BN_CLICKED(IDC_OEP_EDIT_BTNFONT, OnOepEditBtnFont)
	ON_BN_CLICKED(IDC_OEP_FACECOLOR, OnOepSetBtnFaceColor)
	ON_BN_CLICKED(IDC_OEP_WINCOLOR, OnOepSetWinColor)
	ON_CBN_SELCHANGE(IDC_COMBO_OEP_WIDTH, OnChangeComboWidthHeight)
	ON_CBN_SELCHANGE(IDC_COMBO_OEP_HEIGHT, OnChangeComboWidthHeight)
	ON_CBN_SELCHANGE(IDC_COMBO_OEP_BTN_ACROSS, OnChangeComboOepBtnAcross)
	ON_CBN_SELCHANGE(IDC_COMBO_OEP_BTN_DOWN, OnChangeComboOepBtnDown)
	ON_BN_CLICKED(IDC_OEP_ICON_BROWSE_BTN, OnBnClickedIconBrowse)
	//}}AFX_MSG_MAP	
	ON_BN_CLICKED(IDC_CHECK_SPECIAL_OEP, &CD_OEPEdit::OnBnClickedSpecialOep)
	ON_BN_CLICKED(IDC_RADIO_OEP_SPL_PREAUTH, &CD_OEPEdit::OnBnClickedSplPreauth)
	ON_BN_CLICKED(IDC_RADIO_OEP_SPL_REASONCODE, &CD_OEPEdit::OnBnClickedSplReasonCode)
	ON_BN_CLICKED(IDC_RADIO_OEP_SPL_AUTOCOUPON, &CD_OEPEdit::OnBnClickedSplAutoCoupon)
	ON_BN_CLICKED(IDC_RADIO_OEP_SPL_OPENCHECK, &CD_OEPEdit::OnBnClickedSplOpenCheck)
	ON_EN_CHANGE(IDC_EDIT1_OEPGroupNumbers, &CD_OEPEdit::OnEnChangeEdit1Oepgroupnumbers)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CD_OEPEdit message handlers
CD_OEPEdit & CD_OEPEdit::operator = (const CWindowTextExt & cdt)
{
	myID = cdt.controlAttributes.m_myId;
	mRow = cdt.controlAttributes.m_nRow;
	mColumn = cdt.controlAttributes.m_nColumn;
	m_usHeightMultiplier = cdt.controlAttributes.m_usHeightMultiplier;
	m_usWidthMultiplier = cdt.controlAttributes.m_usWidthMultiplier;

	colorFace = cdt.controlAttributes.m_colorFace;
	colorText = cdt.controlAttributes.m_colorText;
	myTextFont = cdt.controlAttributes.lfControlFont;
	defFlag = cdt.controlAttributes.useDefault;
	myCaption = cdt.controlAttributes.m_myCaption;
	myName = cdt.controlAttributes.m_myName;
	

	m_BtnDown = cdt.textAttributes.oepBtnDown;
	m_BtnAcross = cdt.textAttributes.oepBtnAcross;
	m_usBtnWidth = cdt.textAttributes.oepBtnWidth;
	m_usBtnHeight = cdt.textAttributes.oepBtnHeight;
	btnColorFace = cdt.textAttributes.oepBtnColor;
	myShape = cdt.textAttributes.oepBtnShape;
	myPattern = cdt.textAttributes.oepBtnPattern;
	btnFontColor = cdt.textAttributes.oepBtnFontColor;
	myBtnFont = cdt.textAttributes.oepBtnFont;

	//Additions made for Multiple-OEP Window Feature - CSMALL 1/11/06
	//m_OEPGroupID = cdt.textAttributes.mOEPGroupID;
	m_OEPGroupNumbers = cdt.textAttributes.mOEPGroupNumbers;
	m_usHeightMultiplier = cdt.controlAttributes.m_usHeightMultiplier;
	m_usWidthMultiplier = cdt.controlAttributes.m_usWidthMultiplier;

	m_nOEPIconHorizRadioSelection	= cdt.textAttributes.oepBtnIconHorizAlign;
	m_nOEPIconVertRadioSelection	= cdt.textAttributes.oepBtnIconVertAlign;
	m_csOEPIconFileName = cdt.textAttributes.oepIconFileName;
	m_csOEPIconFilePath = cdt.csOEPIconFilePath;

	ASSERT( sizeof(m_csSpecWin) == sizeof(cdt.controlAttributes.SpecWin) );

	for (int i = 0; i < sizeof(m_csSpecWin)/sizeof(m_csSpecWin[0]); i++) {
		m_csSpecWin[i] = cdt.controlAttributes.SpecWin[i];
	}
	return *this;
}

void CD_OEPEdit::saveTo(CWindowTextExt* cdt){
	//This is an OEP window
	cdt->textAttributes.type = CWindowText::TypeOEP;

	//general
	cdt->controlAttributes.m_myId;			//	= tt.myID;
	cdt->controlAttributes.m_nRow				= mRow;
	cdt->controlAttributes.m_nColumn			= mColumn;
	cdt->controlAttributes.m_usHeightMultiplier	= m_usHeightMultiplier;
	cdt->controlAttributes.m_usWidthMultiplier	= m_usWidthMultiplier;
	cdt->controlAttributes.m_nHeight			= m_usHeightMultiplier * CWindowButton::stdHeight;
	cdt->controlAttributes.m_nWidth				= m_usWidthMultiplier * CWindowButton::stdWidth;
	cdt->controlAttributes.m_colorText			= colorText;
	cdt->controlAttributes.m_colorFace			= colorFace;
	cdt->controlAttributes.useDefault			= defFlag;
	cdt->controlAttributes.lfControlFont		= myTextFont;
		//Additions made for Multiple-OEP Window Feature - CSMALL 1/11/06
	_tcscpy_s(cdt->controlAttributes.mOEPGroupNumbers, m_OEPGroupNumbers);

	//OEP Specific
	cdt->textAttributes.oepBtnWidth				= m_usBtnWidth;
	cdt->textAttributes.oepBtnHeight			= m_usBtnHeight;
	cdt->textAttributes.oepBtnColor				= btnColorFace;
	cdt->textAttributes.oepBtnFontColor			= btnFontColor;
	cdt->textAttributes.oepBtnShape				= myShape;
	cdt->textAttributes.oepBtnPattern			= myPattern;
	cdt->textAttributes.oepBtnDown				= m_BtnDown;
	cdt->textAttributes.oepBtnAcross			= m_BtnAcross;
	cdt->textAttributes.oepBtnFont				= myBtnFont;
		//Additions made for Multiple-OEP Window Feature - CSMALL 1/11/06
	_tcscpy_s(cdt->textAttributes.mOEPGroupNumbers, m_OEPGroupNumbers);
		//added Icon support for OEP window buttons (v2.2.0)
	cdt->textAttributes.oepBtnIconHorizAlign	= (CWindowButton::HorizontalIconAlignment) m_nOEPIconHorizRadioSelection;
	cdt->textAttributes.oepBtnIconVertAlign		= (CWindowButton::VerticalIconAlignment) m_nOEPIconVertRadioSelection;
	_tcscpy_s(cdt->textAttributes.oepIconFileName, m_csOEPIconFileName);

	cdt->csOEPIconFileName						= m_csOEPIconFileName;
	cdt->csOEPIconFilePath						= m_csOEPIconFilePath;
}

void CD_OEPEdit::OnOepEditBtnFont() 
{
	// TODO: Add your control notification handler code here
	LOGFONT myLF;
	myLF = myBtnFont; 

	CFontDialog FontDlg;

	FontDlg.m_cf.lStructSize = sizeof(CHOOSEFONT);
	FontDlg.m_cf.hwndOwner = NULL;
	FontDlg.m_cf.lpLogFont = &myLF;
	FontDlg.m_cf.Flags = CF_SCREENFONTS | CF_ENABLEHOOK | CF_INITTOLOGFONTSTRUCT  | CF_LIMITSIZE | CF_EFFECTS | CF_NOVERTFONTS;
	FontDlg.m_cf.nSizeMax = MAX_BTN_FONT_SZ;
	FontDlg.m_cf.nSizeMin = MIN_BTN_FONT_SZ;
	FontDlg.m_cf.rgbColors = btnFontColor;
	
	myLF.lfWidth = 0;

	if(FontDlg.DoModal()== IDOK){
		memcpy(&myBtnFont, FontDlg.m_cf.lpLogFont, sizeof(LOGFONT));
		
		FontDlg.GetCurrentFont(&myBtnFont);
		btnFontColor = FontDlg.GetColor();

	}	
}

void CD_OEPEdit::OnOepSetBtnFaceColor() 
{
	// TODO: Add your control notification handler code here
	CColorDialog dlg;
	dlg.m_cc.Flags |= CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT;
	dlg.m_cc.rgbResult = m_staticOEPBtnColor.m_FillColor;
	dlg.m_cc.lpCustColors = myCustomColors;
	if (dlg.DoModal() == IDOK) {
		m_staticOEPBtnColor.m_FillColor = dlg.m_cc.rgbResult;
		CStaticColorPatch *selectedColor = (CStaticColorPatch *) GetDlgItem (IDC_STATIC_SHOW_OEP_FACECOLOR);
		selectedColor->m_FillColor = m_staticOEPBtnColor.m_FillColor;
		selectedColor->Invalidate (FALSE);
	
	}	
}

void CD_OEPEdit::OnOepSetWinColor() 
{
	// TODO: Add your control notification handler code here
	CColorDialog dlg;
	dlg.m_cc.Flags |= CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT;
	dlg.m_cc.rgbResult = m_staticOEPWinColor.m_FillColor;
	dlg.m_cc.lpCustColors = myCustomColors;
	if (dlg.DoModal() == IDOK) {
		m_staticOEPWinColor.m_FillColor = dlg.m_cc.rgbResult;
		CStaticColorPatch *xx = (CStaticColorPatch *) GetDlgItem (IDC_STATIC_SHOW_OEP_WINCOLOR);
		xx->m_FillColor = m_staticOEPWinColor.m_FillColor;
		xx->Invalidate (FALSE);
	}	
}

void CD_OEPEdit::CalculateWinDimensions()
{
	CString height;
	CString width;

	m_usBtnHeight = ((CComboBox *)GetDlgItem (IDC_COMBO_OEP_HEIGHT))->GetCurSel() + 1;
	m_usBtnWidth = ((CComboBox *)GetDlgItem (IDC_COMBO_OEP_WIDTH))->GetCurSel() + 1;
	m_BtnDown = ((CComboBox *)GetDlgItem (IDC_COMBO_OEP_BTN_DOWN))->GetCurSel () + 2;
	m_BtnAcross =((CComboBox *)GetDlgItem (IDC_COMBO_OEP_BTN_ACROSS))->GetCurSel () + 1;

	height.Format(_T("%d"),m_usBtnHeight * m_BtnDown);
	width.Format(_T("%d"), m_usBtnWidth * m_BtnAcross);
	((CStatic *)GetDlgItem(IDC_STATIC_OEP_WIDTH))->SetWindowText(width);
	((CStatic *)GetDlgItem(IDC_STATIC_OEP_HEIGHT))->SetWindowText(height);

	m_usWidthMultiplier = m_BtnAcross * m_usBtnWidth;
	m_usHeightMultiplier = m_BtnDown * m_usBtnHeight;
}

void CD_OEPEdit::OnChangeComboWidthHeight() 
{
	// TODO: Add your control notification handler code here
	CalculateWinDimensions();
}

void CD_OEPEdit::OnChangeComboOepBtnAcross() 
{
	// TODO: Add your control notification handler code here
	int down = ((CComboBox *)GetDlgItem (IDC_COMBO_OEP_BTN_DOWN))->GetCurSel () + 2;
	int across = ((CComboBox *)GetDlgItem (IDC_COMBO_OEP_BTN_ACROSS))->GetCurSel () + 1;

	m_BtnAcross = across;

//	if((down - 1) * across > 21) {

	/* This will prevent windows from being created which would allow larger
	 * than the max. allowable buttons per window. -CSMALL */
	while((across - 1) * down >= 81){
		down--;
		if((across - 1) * down < 81){
			m_BtnDown = down + 1;
			ASSERT((m_BtnAcross -1) * m_BtnDown >= 81);
			((CComboBox *)GetDlgItem (IDC_COMBO_OEP_BTN_DOWN))->SetCurSel (m_BtnDown - 2);
			CalculateWinDimensions();
			return;
		}
	}

//	if ((down - 1) * across < 21) {
/***  Resetting dimensions of window based on minimum-area value not needed anymore
	while((down - 1) * across < 81){
		down++;
		if((down - 1) * across >= 81){
			m_BtnDown = down ;
			ASSERT((m_BtnDown -1) * m_BtnAcross >= 81);
			((CComboBox *)GetDlgItem (IDC_COMBO_OEP_BTN_DOWN))->SetCurSel (m_BtnDown - 2);
			CalculateWinDimensions();
			return;
		}
	}
*/
	CalculateWinDimensions();
	

}

void CD_OEPEdit::OnChangeComboOepBtnDown() 
{
	// TODO: Add your control notification handler code here
	int down = ((CComboBox *)GetDlgItem (IDC_COMBO_OEP_BTN_DOWN))->GetCurSel () + 2;
	int across = m_BtnAcross =((CComboBox *)GetDlgItem (IDC_COMBO_OEP_BTN_ACROSS))->GetCurSel () + 1;

	m_BtnDown = down;

//	if((down - 1) * across > 21){

	/* This will prevent windows from being created which would allow larger
	 * than the max. allowable buttons per window. -CSMALL */
	while((down - 1) * across >= 81){
		across--;
		if((down - 1) * across < 81){
			m_BtnAcross = across + 1;
			ASSERT((m_BtnDown -1) * m_BtnAcross >= 81);
			((CComboBox *)GetDlgItem (IDC_COMBO_OEP_BTN_ACROSS))->SetCurSel (m_BtnAcross - 1);
			CalculateWinDimensions();
			return;
		}
	}

//	if ((down - 1) * across < 21){
/***  Resetting dimensions of window based on minimum-area value not needed anymore
	while((down - 1) * across < 81){
		across++;
		if((down - 1) * across >= 81){
			m_BtnAcross = across ;
			ASSERT((m_BtnDown -1) * m_BtnAcross >= 81);
			((CComboBox *)GetDlgItem (IDC_COMBO_OEP_BTN_ACROSS))->SetCurSel (m_BtnAcross - 1);
			CalculateWinDimensions();
			return;
		}
	}	
*/
	CalculateWinDimensions();

}

void CD_OEPEdit::OnBnClickedIconBrowse()
{
	CString szFilter = _T("Bitmap Files (*.bmp)|*.bmp||");
	CFileDialog fd(TRUE,_T("bmp"),NULL,OFN_HIDEREADONLY | OFN_PATHMUSTEXIST,szFilter);
	if(m_csOEPIconFilePath)
		fd.m_ofn.lpstrInitialDir = m_csOEPIconFilePath;
	
	if(fd.DoModal() == IDOK){
		m_csOEPIconFilePath = fd.GetPathName();
		m_csOEPIconFileName = fd.GetFileName();

		SetDlgItemText(IDC_OEP_ICON_FILENAME_EDIT,m_csOEPIconFileName);
	}
}

void CD_OEPEdit::OnBnClickedSpecialOep()
{
	// TODO: Add your control notification handler code here
	BOOL  bSpclOep = ((CButton *)GetDlgItem (IDC_CHECK_SPECIAL_OEP))->GetCheck();

	((CEdit *)GetDlgItem (IDC_EDIT1_OEPGroupNumbers))->EnableWindow (!bSpclOep);
	((CButton *)GetDlgItem (IDC_RADIO_OEP_SPL_PREAUTH))->EnableWindow (bSpclOep);
	((CButton *)GetDlgItem (IDC_RADIO_OEP_SPL_REASONCODE))->EnableWindow (bSpclOep);
	((CButton *)GetDlgItem (IDC_RADIO_OEP_SPL_AUTOCOUPON))->EnableWindow (bSpclOep);
	((CButton *)GetDlgItem (IDC_RADIO_OEP_SPL_OPENCHECK))->EnableWindow (bSpclOep);

	m_csSpecWin[SpecWinIndex_OEP] &= ~SpecWinMask_SPCL_OEP_FLAG;
	if (bSpclOep) {
		m_csSpecWin[SpecWinIndex_OEP] |= SpecWinMask_SPCL_OEP_FLAG;
		if (((CButton *)GetDlgItem (IDC_RADIO_OEP_SPL_PREAUTH))->GetCheck ()) {
			((CEdit *)GetDlgItem (IDC_EDIT1_OEPGroupNumbers))->SetWindowText (SPECWIN_SPCL_OEPGROUP_STR_PREAUTH);
		} else if (((CButton *)GetDlgItem (IDC_RADIO_OEP_SPL_REASONCODE))->GetCheck ()) {
			((CEdit *)GetDlgItem (IDC_EDIT1_OEPGroupNumbers))->SetWindowText (SPECWIN_SPCL_OEPGROUP_STR_REASONCODE);
		} else if (((CButton *)GetDlgItem (IDC_RADIO_OEP_SPL_AUTOCOUPON))->GetCheck ()) {
			((CEdit *)GetDlgItem (IDC_EDIT1_OEPGroupNumbers))->SetWindowText (SPECWIN_SPCL_OEPGROUP_STR_AUTOCOUPON);
		} else if (((CButton *)GetDlgItem (IDC_RADIO_OEP_SPL_OPENCHECK))->GetCheck ()) {
			((CEdit *)GetDlgItem (IDC_EDIT1_OEPGroupNumbers))->SetWindowText (SPECWIN_SPCL_OEPGROUP_STR_OPENCHECK);
		} else {
			((CButton *)GetDlgItem (IDC_RADIO_OEP_SPL_REASONCODE))->SetCheck(BST_CHECKED);
			((CEdit *)GetDlgItem (IDC_EDIT1_OEPGroupNumbers))->SetWindowText (SPECWIN_SPCL_OEPGROUP_STR_REASONCODE);
		}
	}
}

void CD_OEPEdit::OnBnClickedSplPreauth()
{
	OnBnClickedSpecialOep();
}
void CD_OEPEdit::OnBnClickedSplReasonCode()
{
	OnBnClickedSpecialOep();
}
void CD_OEPEdit::OnBnClickedSplAutoCoupon()
{
	OnBnClickedSpecialOep();
}
void CD_OEPEdit::OnBnClickedSplOpenCheck()
{
	OnBnClickedSpecialOep();
}

void CD_OEPEdit::OnEnChangeEdit1Oepgroupnumbers()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString csTemp, csTemp2;;
	CEdit *editOep = (CEdit *)GetDlgItem(IDC_EDIT1_OEPGroupNumbers);

	editOep->GetWindowText(csTemp);
	
	// check that only number and commas and dashes are in the text
	int iError = 0;

	for (int i = 0; i < csTemp.GetLength(); i++) {
		switch (csTemp[i]) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case ',':
			case '-':
				csTemp2 += csTemp[i];
				break;
			default:
				iError = 1;
				break;
		}
	}
	if (iError) {
		// if an error then lets refresh the edit box with the modified text that
		// has removed the invalid characters.
		editOep->SetWindowText(csTemp2);
		MessageBeep(MB_ICONEXCLAMATION);    /* make a tone  */
	}
}
