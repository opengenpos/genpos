// DTextItem.cpp : implementation file
//

/*
*===========================================================================
*
*   Copyright (c) 2003, Georgia Southern Research Services Foundation,
*
*===========================================================================
* Title       : CWindowTextItem Edit/Add dialog
* --------------------------------------------------------------------------
* Abstract:     
*
* --------------------------------------------------------------------------
* Update Histories
*
*  Date       Ver.         Name         Description
* Sep-29-15 : 2.02.01  : R.Chambers : added customer display windows to second monitor.
*===========================================================================
*/

#include "stdafx.h"
#include "NewLayout.h"
#include "NewLayoutView.h"
#include "DTextItem.h"
#include "DefaultData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDTextItem dialog


CDTextItem::CDTextItem(CWindowTextExt *pb, CWnd* pParent /*=NULL*/)
	: CDialog(CDTextItem::IDD, pParent)
{
	if (pb) {
		CWindowItem *wi  =(CWindowItem*)pParent;
		wi->WindowDef.AssignTo(df);
		*this = *pb;
	} else {
		//pull in parent windows defaults for control and set attributes to defaults
		CWindowItem *wi  =(CWindowItem*)pParent;
		wi->WindowDef.AssignTo(df);

		myID = 0;
		m_nRow =0;
		m_nColumn =0;
		m_usHeightMultiplier = 3;
		m_usWidthMultiplier = 3;
	//	m_nTop=0;
	//	m_nLeft=0;
		m_nControlType =0;
		colorFace = df.defaultAttributes.TextWinDefault; 
		colorText = df.defaultAttributes.FontColorDefault;	
		myCaption = _T("");
		myName = _T("");

		myTextFont = df.defaultAttributes.FontDefault;
		
	}
	//{{AFX_DATA_INIT(CDTextItem)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CDTextItem & CDTextItem::operator = (const CWindowTextExt & cdt)
{
	myID = (UINT)(cdt.textAttributes.type);//cdt.controlAttributes.m_myId;
	m_nRow = cdt.controlAttributes.m_nRow;
	m_nColumn = cdt.controlAttributes.m_nColumn;
	m_usHeightMultiplier = cdt.controlAttributes.m_usHeightMultiplier;
	m_usWidthMultiplier = cdt.controlAttributes.m_usWidthMultiplier;
//	m_nTop = cdt.textAttributes.m_nTop;
//	m_nLeft = cdt.textAttributes.m_nLeft;
	m_nControlType = cdt.controlAttributes.m_nType;
	colorFace = cdt.controlAttributes.m_colorFace;
	colorText = cdt.controlAttributes.m_colorText;
	myTextFont = cdt.controlAttributes.lfControlFont;

	return *this;
}
void CDTextItem::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDTextItem)
	DDX_Control(pDX, IDC_STATIC_SHOW_FACECOLOR, m_StaticShowFaceColor);
	DDX_Control(pDX, IDC_RADIO_DEF, m_def);
	DDX_Control(pDX, IDC_RADIO_CUST, m_cust);
	DDX_Control(pDX, IDC_DIALOG6_BUTTON_COLOR, m_TextFaceColor);
	DDX_Control(pDX, IDC_BTN_EDITFONT, m_EditFont);
	//}}AFX_DATA_MAP

	if (m_usHeightMultiplier < MIN_TXT_HEIGHT)
		m_usHeightMultiplier = MIN_TXT_HEIGHT;
	if (m_usWidthMultiplier < MIN_TXT_WIDTH)
		m_usWidthMultiplier = MIN_TXT_WIDTH;

	if (pDX->m_bSaveAndValidate) {
		myID =  ((CComboBox *)GetDlgItem (IDC_DIALOG6_COMBO_FUNCTION))->GetCurSel();
		myCaption.LoadString (IDS_TXT_SING_RCT_MAIN + myID);
		m_usHeightMultiplier = ((CComboBox *)GetDlgItem (IDC_COMBO_DIALOG6_NO_ROWS))->GetCurSel () + MIN_TXT_HEIGHT;
		m_usWidthMultiplier = ((CComboBox *)GetDlgItem (IDC_COMBO_DIALOG6_NO_COLS))->GetCurSel () + MIN_TXT_WIDTH;
		colorFace = m_StaticShowFaceColor.m_FillColor;
		
		if(m_def.GetCheck()){
			defFlag = 1;
		}
		if(m_cust.GetCheck()){
			defFlag = 0;
		}
	}
	else {
		//populate combo boxes for width and height
		CString csInsert, csGrid;
		csGrid.LoadString(IDS_STRING_GRIDS);

		for( int z = 0; z < MAX_TXT_HEIGHT; z++ ){
			csInsert.Format(_T("%d %s"), MIN_TXT_HEIGHT + z, csGrid);
			((CComboBox *)GetDlgItem (IDC_COMBO_DIALOG6_NO_ROWS))->AddString(csInsert);
		}

		for( int y = 0; y < MAX_TXT_WIDTH; y++ ){
			csInsert.Format(_T("%d %s"), MIN_TXT_WIDTH + y, csGrid);
			((CComboBox *)GetDlgItem (IDC_COMBO_DIALOG6_NO_COLS))->AddString(csInsert);
		}

		if(myID == CWindowText::TypeMultiReceipt1 || myID == CWindowText::TypeMultiReceipt2 || myID == CWindowText::TypeMultiReceipt3){
			((CComboBox *)GetDlgItem (IDC_COMBO_DIALOG6_NO_ROWS))->SetCurSel (m_usHeightMultiplier + 1);
		} else {
			((CComboBox *)GetDlgItem (IDC_COMBO_DIALOG6_NO_ROWS))->SetCurSel (m_usHeightMultiplier - MIN_TXT_HEIGHT);
		}
		((CComboBox *)GetDlgItem (IDC_DIALOG6_COMBO_FUNCTION))->SetCurSel(myID );
		
		((CComboBox *)GetDlgItem (IDC_COMBO_DIALOG6_NO_COLS))->SetCurSel (m_usWidthMultiplier - MIN_TXT_WIDTH);
		m_StaticShowFaceColor.m_FillColor = colorFace;

		if(defFlag){
			m_def.SetCheck(1);
		} else {
			m_cust.SetCheck(1);
		}
		SetDlgControls (defFlag);
	}
}


BEGIN_MESSAGE_MAP(CDTextItem, CDialog)
	//{{AFX_MSG_MAP(CDTextItem)
	ON_BN_CLICKED(IDC_DIALOG6_BUTTON_COLOR, OnDialog6ButtonColor)
	ON_CBN_CLOSEUP (IDC_DIALOG6_COMBO_FUNCTION, OnSelchange)
	ON_BN_CLICKED(IDC_RADIO_DEF, OnRadioDef)
	ON_BN_CLICKED(IDC_RADIO_CUST, OnRadioCust)
	ON_BN_CLICKED(IDC_BTN_EDITFONT, OnChangeFont)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDTextItem message handlers
void CDTextItem::OnChangeFont() 
{
	LOGFONT lf = myTextFont;

	CFontDialog FontDlg;
	FontDlg.m_cf.lStructSize = sizeof(CHOOSEFONT);
	FontDlg.m_cf.hwndOwner = NULL;
	FontDlg.m_cf.lpLogFont = &lf;
	FontDlg.m_cf.Flags = CF_SCREENFONTS | CF_ENABLEHOOK | CF_INITTOLOGFONTSTRUCT  |  CF_NOSIZESEL | CF_EFFECTS | CF_NOVERTFONTS | CF_FIXEDPITCHONLY;
	FontDlg.m_cf.rgbColors = colorText;

	if(FontDlg.DoModal()== IDOK){		
		FontDlg.GetCurrentFont(&myTextFont);
		colorText = FontDlg.GetColor();
	}
}

void CDTextItem::OnDialog6ButtonColor() 
{
	CColorDialog dlg;
	dlg.m_cc.Flags |= CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT;
	dlg.m_cc.rgbResult = m_StaticShowFaceColor.m_FillColor;
	dlg.m_cc.lpCustColors = myCustomColors;
	if (dlg.DoModal() == IDOK) {
		m_StaticShowFaceColor.m_FillColor = dlg.m_cc.rgbResult;
		CStaticColorPatch *xx = (CStaticColorPatch *) GetDlgItem (IDC_STATIC_SHOW_FACECOLOR);
		xx->m_FillColor = m_StaticShowFaceColor.m_FillColor;
		xx->Invalidate (FALSE);
	}
}

void CDTextItem::OnSelchange() 
{
	myID =  ((CComboBox *)GetDlgItem (IDC_DIALOG6_COMBO_FUNCTION))->GetCurSel();

	myCaption.LoadString (IDS_TXT_SING_RCT_MAIN + myID);

	/*if the control is one of the triple display windows
	it needs to have at least 3 rows if it does not then
	set the index to that number*/
	if( (myID >= CWindowText::TypeMultiReceipt1) && (myID <= CWindowText::TypeMultiReceipt3) ){
		int myDownSizeIndex = ((CComboBox *)GetDlgItem (IDC_COMBO_DIALOG6_NO_ROWS))->GetCurSel();
		if(myDownSizeIndex < 2){
			((CComboBox *)GetDlgItem (IDC_COMBO_DIALOG6_NO_ROWS))->SetCurSel (2);
		}
	}
}

void CDTextItem::OnRadioDef() 
{
	//Color
	m_StaticShowFaceColor.m_FillColor = df.defaultAttributes.TextWinDefault;
	SetDlgControls(TRUE);
}

void CDTextItem::OnRadioCust() 
{
	SetDlgControls(FALSE);
}

void CDTextItem::SetDlgControls(BOOL flag)
{
	m_TextFaceColor.EnableWindow(!flag);
	InvalidateRect (NULL, FALSE);
}

BOOL CDTextItem::OnInitDialog() 
{
	CComboBox *pCombo = (CComboBox *)GetDlgItem (IDC_DIALOG6_COMBO_FUNCTION);
	ASSERT(pCombo);

	int idArray[] = {
		IDS_TXT_SING_RCT_MAIN,        // CWindowText::TypeSnglReceiptMain
		IDS_TXT_MULTI_1,              // CWindowText::TypeMultiReceipt1
		IDS_TXT_MULTI_2,              // CWindowText::TypeMultiReceipt2
		IDS_TXT_MULTI_3,              // CWindowText::TypeMultiReceipt3
		IDS_TXT_REC_NOTUSED_1,        // CWindowText::TypeReceiptNotUsed1
		IDS_TXT_REC_NOTUSED_2,        // CWindowText::TypeReceiptNotUsed2
		IDS_TXT_LEAD_THRU,            // CWindowText::TypeLeadThru
		IDS_TXT_DESC,                 // CWindowText::TypeLeadThruDescriptor
		IDS_TXT_SING_SUB,             // CWindowText::TypeSnglReceiptSub
		IDS_TXT_SING_SCROLL,          // CWindowText::TypeSnglReceiptScroll
		IDS_TXT_CLOCK,                // CWindowText::TypeClock
		IDS_TXT_SUP_PROG,             // CWindowText::TypeSupProg
		0,                            // CWindowText::TypeOEP
		IDS_TXT_SING_CUST_DISP,       // CWindowText::TypeSnglReceiptCustDisplay
		IDS_TXT_LEADTHRU_CUST_DISP,   // CWindowText::TypeLeadThruCustDisplay
		IDS_TXT_CLOCK_CUST_DISP,      // CWindowText::TypeClockCustDisplay
		IDS_TXT_OEP_CUST_DISP         // CWindowText::TypeOEPCustDisplay
	};

	//  WARNING:  Dependency on sequence for these IDs in function CDTextItem::OnSelchange () above.
	//            We have to do a test for triple display window and window sizes.
	CString txt;
	for (int i = 0; i < sizeof(idArray)/sizeof(idArray[0]); i++)
	{
		if (idArray[i] == 0) {
			txt = L"----";
		} else {
			txt.LoadString(idArray[i]);
		}
		pCombo->AddString(txt);
	}

	CDialog::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
