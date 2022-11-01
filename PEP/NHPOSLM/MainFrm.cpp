// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "NewLayout.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "NewLayoutView.h"
#include <locale.h>
#include <mbctype.h>
#include "DLanguageEdit.h"
#include <winuser.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



static UINT myCodePage = 0;

/*
	The CodePageTable structure is used in several places for user interactions dealing
	with languages other than English.  This table is designed to provide information concerning
	the language specific string to use when allowing users to pick a language.  It also contains
	information such as the font size, printer code page, character set, etc.

	The end of the table is indicated by a NULL value for the structure member tcCountry which
	would normally contain a pointer to a string for the name of the DLL containing the strings
	for that language.  Any entries in the table after the entry that begins "{NULL, 0, ..."
	will be ignored.
 */
	CodePageTable myCodePageTable [] = {
		{_T("SimplifiedChinese"), IDS_LANG_CHINESE, 10, 936, LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED, GB2312_CHARSET, DEFAULT_QUALITY, FIXED_PITCH},
		{_T("English"), IDS_LANG_ENGLISH, 8, 1252, LANG_ENGLISH, SUBLANG_ENGLISH_US, ANSI_CHARSET, DEFAULT_QUALITY, DEFAULT_PITCH},
		{_T("French"), IDS_LANG_FRENCH, 8, 1252, LANG_FRENCH, SUBLANG_FRENCH, ANSI_CHARSET, DEFAULT_QUALITY, DEFAULT_PITCH},
		{_T("German"), IDS_LANG_GERMAN, 8, 1252, LANG_GERMAN, SUBLANG_GERMAN, ANSI_CHARSET, DEFAULT_QUALITY, DEFAULT_PITCH},
		{_T("Spanish"), IDS_LANG_SPANISH, 8, 1252, LANG_SPANISH, SUBLANG_SPANISH_MODERN, ANSI_CHARSET, DEFAULT_QUALITY, DEFAULT_PITCH},
		{NULL, 0, 10, 1252, LANG_NEUTRAL, SUBLANG_SYS_DEFAULT, ANSI_CHARSET, PROOF_QUALITY, FIXED_PITCH},
		{_T("Greek"), IDS_LANG_GREEK, 8, 1253, LANG_GREEK, 0x01, GREEK_CHARSET, DEFAULT_QUALITY, DEFAULT_PITCH},
		{_T("Italian"), IDS_LANG_ITALIAN, 8, 1252, LANG_ITALIAN, SUBLANG_ITALIAN, ANSI_CHARSET, DEFAULT_QUALITY, DEFAULT_PITCH},
		{_T("Japanese"), IDS_LANG_JAPANESE, 10, 932, LANG_JAPANESE, 0x01, SHIFTJIS_CHARSET, PROOF_QUALITY, FIXED_PITCH},
		{_T("Korean"), IDS_LANG_KOREAN, 10, 949, LANG_KOREAN, SUBLANG_KOREAN, HANGUL_CHARSET, DEFAULT_QUALITY, DEFAULT_PITCH}, /* ESMITH */
		{_T("Russian"), IDS_LANG_RUSSIAN, 8, 1251, LANG_RUSSIAN, 0x01, RUSSIAN_CHARSET, PROOF_QUALITY, FIXED_PITCH}
	};


/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_VIEW_LANGUAGE, OnChangeLanguage)
	ON_COMMAND(ID_TOOLBAR_LANG, OnChangeLanguage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_SEPARATOR,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	uiCurrentCodePage = 0;

}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetPaneInfo(0, 0, SBPS_NORMAL, 40);
	m_wndStatusBar.SetPaneInfo(1, 1, SBPS_NORMAL|SBPS_STRETCH, 0);


	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers





void CMainFrame::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CMDIFrameWnd::OnRButtonDown(nFlags, point);
}





//-----------------------   Multi-lingual code--------------------------------------------
void CMainFrame::GetLangFont(int iLiSel, CodePageTable  myCodePageTable[])
{
	LOGFONT logfont;


	memset (&logfont, 0, sizeof (LOGFONT));

	logfont.lfHeight = -MulDiv(10, GetDeviceCaps(GetDC()->GetSafeHdc(), LOGPIXELSY), 72);
	logfont.lfOutPrecision = OUT_TT_PRECIS;
	logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logfont.lfQuality = DEFAULT_QUALITY;
	logfont.lfPitchAndFamily = DEFAULT_PITCH;


	logfont.lfHeight = -MulDiv(myCodePageTable[iLiSel].unFontSize, GetDeviceCaps(GetDC()->GetSafeHdc(), LOGPIXELSY), 72);
	logfont.lfCharSet = myCodePageTable[iLiSel].uiCharSet ;
	logfont.lfQuality = myCodePageTable[iLiSel].ucQuality;
	logfont.lfPitchAndFamily = myCodePageTable[iLiSel].ucPitch;

	if (!CFont::FromHandle(resourceFont)) {
		resourceFont.CreateFontIndirect(&logfont);
	}
}


UINT CMainFrame::setCodePage (UINT iCodePage)
{
	UINT cp = myCodePage;

	_setmbcp (iCodePage);

	myCodePage = iCodePage;

	return cp;
}


void CMainFrame::OnChangeLanguage()
{
	// TODO: Add your command handler code here
	UINT iLiSel = 0;
	CDLanguageEdit le;

	if (le.DoModal() == IDOK){

		iLiSel = le.Selection;
		LoadLangDLL(iLiSel);
		return;
	}
}


BOOL CMainFrame::HandleKeyBoardChange(UINT message, WPARAM wParam, LPARAM lParam)
{
	int nBuff = 30;
	int iLiSel = 0;
	TCHAR  AvailableLangName[100];
	CPINFO myCPInfo;

	UINT j = 0;

	for (iLiSel = 0; myCodePageTable[iLiSel].tcCountry; iLiSel++) {
		LANGID xlan = (LANGID)(LOWORD(lParam) & 0x00ff);
		if ((UCHAR)xlan == myCodePageTable[iLiSel].wLanguage)
			break;
	}
	if (! myCodePageTable[iLiSel].tcCountry) {
		AfxMessageBox(IDS_NOT_SUP,MB_OK, 0);
		return FALSE;
	}

	if (! GetCPInfo(myCodePageTable[iLiSel].unCodePage, &myCPInfo)) {
		AfxMessageBox(IDS_CPNOTFOUND, MB_OK);
		return FALSE;
	}

	/*
		Performs Language Dll change.

	Create the path to the appropriate language dll then try to load it.

	If the load fails pop up a dialog box then reset the current dll handle
	back to our current resource dll.

	If the load succeeds then set the locale and code page information as
	provide in the language description.

	English is treated as a special case because English is the default resource
	file and does not require a load library.

	 */

	_tcscpy_s(AvailableLangName, _T(".\\LM_Res\\"));
	_tcscat(AvailableLangName, myCodePageTable[iLiSel].tcCountry);
	_tcscat(AvailableLangName, _T(".dll"));
	if (_tcscmp(_T("English"), myCodePageTable[iLiSel].tcCountry) != 0) {
		hResourceDll = AfxLoadLibrary (AvailableLangName);
	}
	else {
		hResourceDll = AfxGetInstanceHandle( );
	}

	if (!hResourceDll) {
		hResourceDll = AfxGetResourceHandle( );
		ASSERT(hResourceDll);
		AfxMessageBox(IDS_DLLNF, MB_OK);
	}
	else{
		AfxSetResourceHandle(hResourceDll);
		_wsetlocale (LC_ALL, myCodePageTable[iLiSel].tcCountry);
		uiCurrentCodePage = myCodePageTable[iLiSel].unCodePage;
		setCodePage (uiCurrentCodePage);
	}
	/*Load new menu for CMDIFrameWnd derived MainFrame class. We want to load the menu for the Main Frame
	even if there is an active child. If we dont, if the child is destroyed, the Main Frames menu will still
	have the old menu instead of the translated one*/

	HMENU m_hMainMenu = ::LoadMenu(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	ASSERT(m_hMainMenu);
	m_hMenuDefault = m_hMainMenu;

	/*Load New Menu for CMDIChildWnd derived ChildFrm class if there is an instance of a child window. We have
	to first check to see if there are any active child windows. If there are, we cycle through all of the existing
	child windows and set the new menus. If not, We set the Main Frame's menu to the previously loaded one. All
	new instances of CHild Windows are created with the correct resource, so this makes changes to existing windows*/

	CFrameWnd* temp =MDIGetActive(NULL);
	if(temp){
		while (temp && IsChild(temp)){
			CChildFrame *cf;
			cf = (CChildFrame*)temp;
			cf->ChangeMenu();
			temp = temp->m_pNextFrameWnd;
		}
	}

	else{
		MDISetMenu(CMenu::FromHandle(m_hMainMenu),NULL);
		DrawMenuBar();
	}

	GetLangFont(iLiSel, myCodePageTable);

	CString s;
	s.LoadString(AFX_IDS_APP_TITLE);
	this->SetWindowText(s);


	return TRUE;
}


void CMainFrame::LoadLangDLL(UINT iLiSel)
{
	int nBuff = 30;
	HKL lpList[30];
	TCHAR  AvailableLangName[100];

	CPINFO myCPInfo;
	UINT j = 0;
	UINT myN = GetKeyboardLayoutList(nBuff, lpList);

	if (! GetCPInfo(myCodePageTable[iLiSel].unCodePage, &myCPInfo)) {
		AfxMessageBox (IDS_CPNOTFOUND, MB_OK);
		return;
	}

	LANGID supLang = MAKELANGID(myCodePageTable[iLiSel].wLanguage, myCodePageTable[iLiSel].wSubLanguage);
	BOOL bPrimaryLangMatch = FALSE;

	//Check the keyboard Input local for
	//the language and sublanguage
	for (j = 0; j < myN; j++) {
		LANGID xlan = (LANGID)lpList[j];
		if (xlan == supLang)
			break;
		if(PRIMARYLANGID(xlan) == PRIMARYLANGID(supLang)){
			bPrimaryLangMatch = TRUE;
		}
	}
	if (j >= myN && !bPrimaryLangMatch) {
		AfxMessageBox(IDS_NOTKBLIST, MB_OK);
	}else if(j >= myN && bPrimaryLangMatch){
		AfxMessageBox(IDS_NOTSUBLANG, MB_OK);
	}

	/*
		Performs Language Dll change.

	Create the path to the appropriate language dll then try to load it.

	If the load fails pop up a dialog box then reset the current dll handle
	back to our current resource dll.

	If the load succeeds then set the locale and code page information as
	provide in the language description.

	English is treated as a special case because English is the default resource
	file and does not require a load library.

	 */

	_tcscpy_s(AvailableLangName, _T(".\\LM_Res\\"));
		_tcscat(AvailableLangName, myCodePageTable[iLiSel].tcCountry);
		_tcscat(AvailableLangName, _T(".dll"));
		if (_tcscmp(_T("English"), myCodePageTable[iLiSel].tcCountry) != 0) {
			hResourceDll = AfxLoadLibrary (AvailableLangName);
			if (!hResourceDll) {
				hResourceDll = AfxGetResourceHandle( );
				ASSERT(hResourceDll);
				AfxMessageBox(IDS_DLLNF, MB_OK);
			}
			else{
				AfxSetResourceHandle(hResourceDll);
				_wsetlocale (LC_ALL, myCodePageTable[iLiSel].tcCountry);
				uiCurrentCodePage = myCodePageTable[iLiSel].unCodePage;
				setCodePage (uiCurrentCodePage);
				}
			AfxSetResourceHandle(hResourceDll);
		}
		else {
			hResourceDll = AfxGetInstanceHandle( );
			AfxSetResourceHandle(hResourceDll);
			_wsetlocale(LC_ALL, myCodePageTable[iLiSel].tcCountry);
			uiCurrentCodePage = myCodePageTable[iLiSel].unCodePage;
			setCodePage(uiCurrentCodePage);
	}


	/*Load new menu for CMDIFrameWnd derived MainFrame class. We want to load the menu for the Main Frame
	even if there is an active child. If we dont, if the child is destroyed, the Main Frames menu will still
	have the old menu instead of the translated one*/

	HMENU m_hMainMenu = ::LoadMenu(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	ASSERT(m_hMainMenu);
	m_hMenuDefault = m_hMainMenu;

	/*Load New Menu for CMDIChildWnd derived ChildFrm class if there is an instance of a child window. We have
	to first check to see if there are any active child windows. If there are, we cycle through all of the existing
	child windows and set the new menus. If not, We set the Main Frame's menu to the previously loaded one. All
	new instances of CHild Windows are created with the correct resource, so this makes changes to existing windows*/

	CFrameWnd* temp =MDIGetActive(NULL);
	if(temp){
		while (temp && IsChild(temp)){
			CChildFrame *cf;
			cf = (CChildFrame*)temp;
			cf->ChangeMenu();
			temp = temp->m_pNextFrameWnd;
		}
	}

	else{
	MDISetMenu(CMenu::FromHandle(m_hMainMenu),NULL);
	DrawMenuBar();
	}

	GetLangFont(iLiSel, myCodePageTable);

	HKL hkl = GetKeyboardLayout(0);
	CString s;
	s.LoadString(AFX_IDS_APP_TITLE);
	SetFont(&resourceFont, TRUE);
	this->SetWindowText(s);

	return;
}

BOOL CMainFrame::ProcessCmdLineLang(WORD lang, WORD sublang)
{

	for(int i = 0; myCodePageTable[i].tcCountry; i++){

		if(lang == myCodePageTable[i].wLanguage){

			if(sublang == myCodePageTable[i].wSubLanguage){
				LoadLangDLL(i);
				return TRUE;
			}
		}
	}
	return FALSE;
}


