// DefaultEdit.cpp : implementation file
//

#include "stdafx.h"
#include "NewLayout.h"
#include "DefaultEdit.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DefaultEdit dialog


DefaultEdit::DefaultEdit(CWnd* pParent /*=NULL*/)
	: CDialog(DefaultEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(DefaultEdit)
	m_DefHeight = _T("");
	m_DefWidth = _T("");
	//}}AFX_DATA_INIT
}


void DefaultEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DefaultEdit)
	DDX_Control(pDX, IDC_STATIC_DEF_PATTERN_VERTICAL, m_PatternVertical);
	DDX_Control(pDX, IDC_STATIC_DEF_PATTERN_NONE, m_PatternNone);
	DDX_Control(pDX, IDC_STATIC_DEF_PATTERN_HORIZONTAL, m_PatternHorizontal);
	DDX_Control(pDX, IDC_DEF_SHAPE_ROUNDRECT, m_ShapeRoundRect);
	DDX_Control(pDX, IDC_DEF_SHAPE_RECT, m_ShapeRect);
	DDX_Control(pDX, IDC_DEF_SHAPE_ELIPSE, m_ShapeElipse);
	DDX_Control(pDX, IDC_STATIC_SAMPLE, m_FontSample);
	DDX_Control(pDX, IDC_STATIC_SHOW_TEXTWINCOLOR, m_StaticShowTextWinColor);
	DDX_Control(pDX, IDC_STATIC_SHOW_WINCOLOR, m_StaticShowWinColor);
	DDX_Control(pDX, IDC_STATIC_SHOW_FACECOLOR, m_StaticShowFaceColor);
	DDX_CBString(pDX, IDC_COMBO_DIALOGDEF_HEIGHT, m_DefHeight);
	DDX_CBString(pDX, IDC_COMBO_DIALOGDEF_WIDTH, m_DefWidth);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate) {
		//Button Shape
		ShapeDefault = CWindowButton::ButtonShapeRect;

		if (m_ShapeElipse.GetCheck ()) {
			ShapeDefault = CWindowButton::ButtonShapeElipse;
		}
		else if (m_ShapeRoundRect.GetCheck ()) {
			ShapeDefault = CWindowButton::ButtonShapeRoundedRect;
		}

		//Button Pattern
		PatternDefault = CWindowButton::ButtonPatternNone;

		if (m_PatternHorizontal.GetCheck ()) {
			PatternDefault = CWindowButton::ButtonPatternHoriz;
		}
		else if (m_PatternVertical.GetCheck ()) {
			PatternDefault = CWindowButton::ButtonPatternVert;
		}

		
		//Width and Height
		WidthDefault = _wtoi (m_DefWidth);
		if (WidthDefault < MIN_BTN_WIDTH)
			WidthDefault = MIN_BTN_WIDTH;
		else if (WidthDefault > MAX_BTN_WIDTH)
			WidthDefault = MAX_BTN_WIDTH;

		HeightDefault = _wtoi (m_DefHeight);
		if (HeightDefault < MIN_BTN_HEIGHT)
			HeightDefault = MIN_BTN_HEIGHT;
		else if (HeightDefault > MAX_BTN_HEIGHT)
			HeightDefault = MAX_BTN_HEIGHT;
		
		//Color
		ButtonDefault = m_StaticShowFaceColor.m_FillColor;
		TextWinDefault = m_StaticShowTextWinColor.m_FillColor;
		WindowDefault = m_StaticShowWinColor.m_FillColor;
	}

	else {
		//populate combo boxes for width and height
		CString csInsert, csGrid;
		csGrid.LoadString(IDS_STRING_GRIDS);

		for( int z = 0; z < MAX_BTN_HEIGHT; z++ ){
			csInsert.Format(_T("%d %s"), MIN_BTN_HEIGHT + z, csGrid);
			((CComboBox *)GetDlgItem (IDC_COMBO_DIALOGDEF_HEIGHT))->AddString(csInsert);
		}

		for( int y = 0; y < MAX_BTN_WIDTH; y++ ){
			csInsert.Format(_T("%d %s"), MIN_BTN_WIDTH + y, csGrid);
			((CComboBox *)GetDlgItem (IDC_COMBO_DIALOGDEF_WIDTH))->AddString(csInsert);
		}

		//Button Shape
		m_ShapeRect.SetCheck ((ShapeDefault == CWindowButton::ButtonShapeRect) ? 1 : 0);
		m_ShapeElipse.SetCheck ((ShapeDefault == CWindowButton::ButtonShapeElipse) ? 1 : 0);
		m_ShapeRoundRect.SetCheck ((ShapeDefault == CWindowButton::ButtonShapeRoundedRect) ? 1 : 0);
		
		//Button Pattern
		m_PatternNone.SetCheck ((PatternDefault == CWindowButton::ButtonPatternNone) ? 1 : 0);
		m_PatternHorizontal.SetCheck ((PatternDefault == CWindowButton::ButtonPatternHoriz) ? 1 : 0);
		m_PatternVertical.SetCheck ((PatternDefault == CWindowButton::ButtonPatternVert) ? 1 : 0);
		
		
		//Width and Height --> verify value is between 1 and 6
		if (WidthDefault < MIN_BTN_WIDTH)
			WidthDefault = MIN_BTN_WIDTH;
		else if (WidthDefault > MAX_BTN_WIDTH)
			WidthDefault = MAX_BTN_WIDTH;

		if (HeightDefault < MIN_BTN_HEIGHT)
			HeightDefault = MIN_BTN_HEIGHT;
		else if (HeightDefault > MAX_BTN_HEIGHT)
			HeightDefault = MAX_BTN_HEIGHT;

		((CComboBox *)GetDlgItem (IDC_COMBO_DIALOGDEF_HEIGHT))->SetCurSel (HeightDefault - MIN_BTN_HEIGHT);
		((CComboBox *)GetDlgItem (IDC_COMBO_DIALOGDEF_WIDTH))->SetCurSel (WidthDefault - MIN_BTN_WIDTH);

		//Color
		m_StaticShowTextColor.m_FillColor = FontColorDefault;
		m_StaticShowFaceColor.m_FillColor = ButtonDefault;
		m_StaticShowTextWinColor.m_FillColor = TextWinDefault;
		m_StaticShowWinColor.m_FillColor = WindowDefault;

	}
}


BEGIN_MESSAGE_MAP(DefaultEdit, CDialog)
	//{{AFX_MSG_MAP(DefaultEdit)
	ON_BN_CLICKED(IDC_BUTTON_DEF_BTNCOLOR, OnChangeDefBtnColor)
	ON_BN_CLICKED(IDC_BUTTON_DEF_FONT, OnChangeDefFont)
	ON_BN_CLICKED(IDC_BUTTON_DEF_TEXTWINCOLOR, OnChangeDefTextWinColor)
	ON_BN_CLICKED(IDC_BUTTON_DEFWINCOLOR, OnChangeDefWinColor)
	ON_BN_CLICKED(IDC_BUTTON_DEF_FONTCOLOR, OnChangeDefFontColor)
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DefaultEdit message handlers

void DefaultEdit::OnChangeDefBtnColor() 
{
	// TODO: Add your control notification handler code here
	CColorDialog BtnDlg;
	BtnDlg.m_cc.Flags |= CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT;
	BtnDlg.m_cc.rgbResult = m_StaticShowFaceColor.m_FillColor;
	BtnDlg.m_cc.lpCustColors = myCustomColors;
	if (BtnDlg.DoModal() == IDOK) {
		m_StaticShowFaceColor.m_FillColor = BtnDlg.m_cc.rgbResult;
		CStaticColorPatch *selectedColor = (CStaticColorPatch *) GetDlgItem (IDC_STATIC_SHOW_FACECOLOR);
		selectedColor->m_FillColor = m_StaticShowFaceColor.m_FillColor;
		selectedColor->Invalidate (FALSE);
		m_FontSample.Invalidate(FALSE);
	}
	
}

void DefaultEdit::OnChangeDefFont() 
{
	// TODO: Add your control notification handler code here

	CFontDialog FontDlg;
	FontDlg.m_cf.lStructSize = sizeof(CHOOSEFONT);
	FontDlg.m_cf.hwndOwner = NULL;
	FontDlg.m_cf.lpLogFont = &lf;
	FontDlg.m_cf.Flags = CF_SCREENFONTS | CF_ENABLEHOOK | CF_INITTOLOGFONTSTRUCT  | CF_LIMITSIZE | CF_EFFECTS | CF_NOVERTFONTS;
	FontDlg.m_cf.nSizeMax = MAX_BTN_FONT_SZ;
	FontDlg.m_cf.nSizeMin = MIN_BTN_FONT_SZ;
	FontDlg.m_cf.rgbColors = FontColorDefault;

	if(FontDlg.DoModal()== IDOK){
		memcpy(&lf, FontDlg.m_cf.lpLogFont, sizeof(LOGFONT));
		
		FontDlg.GetCurrentFont(&FontDefault);
		FontColorDefault = FontDlg.GetColor();
		m_FontSample.Invalidate(FALSE);

	
	}
	
}

void DefaultEdit::OnChangeDefFontColor() 
{
	// TODO: Add your control notification handler code here
	CColorDialog FontColorDlg;
	FontColorDlg.m_cc.Flags |= CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT;
	FontColorDlg.m_cc.rgbResult = m_StaticShowTextWinColor.m_FillColor;
	FontColorDlg.m_cc.lpCustColors = myCustomColors;
	if (FontColorDlg.DoModal() == IDOK) {
		m_StaticShowTextColor.m_FillColor = FontColorDlg.GetColor();
		CStaticColorPatch *selectedColor = (CStaticColorPatch *) GetDlgItem (IDC_STATIC_SHOW_TEXTCOLOR);
		selectedColor->m_FillColor = m_StaticShowTextColor.m_FillColor;
		selectedColor->Invalidate (FALSE);
	}
}


void DefaultEdit::OnChangeDefTextWinColor() 
{
	// TODO: Add your control notification handler code here
	CColorDialog TextWinDlg;
	TextWinDlg.m_cc.Flags |= CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT;
	TextWinDlg.m_cc.rgbResult = m_StaticShowTextWinColor.m_FillColor;
	TextWinDlg.m_cc.lpCustColors = myCustomColors;
	if (TextWinDlg.DoModal() == IDOK) {
		m_StaticShowTextWinColor.m_FillColor = TextWinDlg.m_cc.rgbResult;
		CStaticColorPatch *selectedColor = (CStaticColorPatch *) GetDlgItem (IDC_STATIC_SHOW_TEXTWINCOLOR);
		selectedColor->m_FillColor = m_StaticShowTextWinColor.m_FillColor;
		selectedColor->Invalidate (FALSE);
	}
}

void DefaultEdit::OnChangeDefWinColor() 
{
	// TODO: Add your control notification handler code here
	CColorDialog WinDlg;
	WinDlg.m_cc.Flags |= CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT;
	WinDlg.m_cc.rgbResult = m_StaticShowWinColor.m_FillColor;
	WinDlg.m_cc.lpCustColors = myCustomColors;
	if (WinDlg.DoModal() == IDOK) {
		m_StaticShowWinColor.m_FillColor = WinDlg.m_cc.rgbResult;
		CStaticColorPatch *selectedColor = (CStaticColorPatch *) GetDlgItem (IDC_STATIC_SHOW_WINCOLOR);
		selectedColor->m_FillColor = m_StaticShowWinColor.m_FillColor;
		selectedColor->Invalidate (FALSE);
	}
	
}



BOOL DefaultEdit::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	lf = FontDefault;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void DefaultEdit::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your message handler code here and/or call default
	if(nIDCtl == IDC_STATIC_SAMPLE){
		CFont fontSample;
		VERIFY(fontSample.CreateFontIndirect(&FontDefault));

		CDC *pDC = m_FontSample.GetDC();
		CBrush bakBrush(m_StaticShowFaceColor.m_FillColor);
		CRect rect;
		m_FontSample.GetClientRect(&rect);
		pDC->FillRect(&rect, &bakBrush);
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(FontColorDefault);
		pDC->SelectObject(&fontSample);
		pDC->DrawText(_T("Abc123"), &lpDrawItemStruct->rcItem,DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	}else
		CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}
