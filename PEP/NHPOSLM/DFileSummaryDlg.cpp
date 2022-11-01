// DFileSummaryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "newlayout.h"
#include "DFileSummaryDlg.h"
#include <time.h>

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
HWND edit;
HINSTANCE ghInstance;
//Folder Name that has been selected
TCHAR FolderName[MAX_PATH];

// Path for last set folder - CSMALL
CString savedPath;


/////////////////////////////////////////////////////////////////////////////
// CDFileSummaryDlg dialog


CDFileSummaryDlg::CDFileSummaryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDFileSummaryDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDFileSummaryDlg)
	m_nMjVersion = 0;
	m_nMnVersion = 0;
	m_nRelease = 0;
	m_editSummary = _T("");
	m_csPEPFile = _T("");
	m_csIconDir = _T("");
	//}}AFX_DATA_INIT
	csDialogTitle = _T("");
}


void CDFileSummaryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDFileSummaryDlg)
	DDX_Control(pDX, IDC_STATIC_DOT2, m_Dot2);
	DDX_Control(pDX, IDC_STATIC_DOT, m_Dot);
	DDX_Text(pDX, IDC_EDIT_MAJOR, m_nMjVersion);
	DDV_MinMaxUInt(pDX, m_nMjVersion, 0, 99);
	DDX_Text(pDX, IDC_EDIT_MINOR, m_nMnVersion);
	DDV_MinMaxUInt(pDX, m_nMnVersion, 0, 99);
	DDX_Text(pDX, IDC_EDIT_RELEASE, m_nRelease);
	DDX_Text(pDX, IDC_EDIT_SUMMARY, m_editSummary);
	DDV_MaxChars(pDX, m_editSummary, 1024);
	DDX_Text(pDX, IDC_EDIT_PEPFILE, m_csPEPFile);
	DDV_MaxChars(pDX, m_csPEPFile, 30);
	DDX_Text(pDX, IDC_EDIT_ICON_DIR, m_csIconDir);
	//}}AFX_DATA_MAP

	((CStatic *)GetDlgItem(IDC_EDIT_MAJOR))->EnableWindow(FALSE);
	((CStatic *)GetDlgItem(IDC_EDIT_MINOR))->EnableWindow(FALSE);
	((CStatic *)GetDlgItem(IDC_EDIT_RELEASE))->EnableWindow(FALSE);

	if (pDX->m_bSaveAndValidate) {
		
	}
	else {
		//srings for formatting dialog display of system times
		CString csTimeFormat;
		CString csDay;
		CString csMonth;
		CString csHour;
		CString csMin;
		CString csAMPM;

		//loop through the last 5 times the file has been saved
		for(int x = 0; x < 5; x++) {
			//create CTime object
			const SYSTEMTIME systime = pDoc->documentAttributes.lastModified[x];
			CTime time(systime, -1);
			
			//if no time entry in pDoc->documentAttributes.lastModified[x], continue to next entry
			if(!time.GetTime())
				continue;

			//initialize format strings to null for each iteration of the loop
			csTimeFormat = _T("");
			csDay = _T("");
			csMonth = _T("");
			csHour = _T("");
			csMin = _T("");
			csAMPM = _T("");


			//set the Weekday format string
			switch (time.GetDayOfWeek())
			{
			case 1:
			   csDay.LoadString(IDS_STRING_SUN);// = _T("Sunday");
			   break;

			case 2:
			   csDay.LoadString(IDS_STRING_MON);// = _T("Monday");
			   break;

			case 3:
			   csDay.LoadString(IDS_STRING_TUE);// = _T("Tuesday");
			   break;

			case 4:
			   csDay.LoadString(IDS_STRING_WED);// = _T("Wednesday");
			   break;

			case 5:
			   csDay.LoadString(IDS_STRING_THU);// = _T("Thursday");
			   break;

			case 6:
			   csDay.LoadString(IDS_STRING_FRI);// = _T("Friday");
			   break;

			case 7:
			   csDay.LoadString(IDS_STRING_SAT);// = _T("Saturday");
			   break;
			}

			//set the month format string
			switch (time.GetMonth())
			{
			case 1:
			   csMonth.LoadString(IDS_STRING_JAN);// = _T("January");
			   break;

			case 2:
			   csMonth.LoadString(IDS_STRING_FEB);// = _T("February");
			   break;

			case 3:
			   csMonth.LoadString(IDS_STRING_MAR);// = _T("March");
			   break;

			case 4:
			   csMonth.LoadString(IDS_STRING_APR);// = _T("April");
			   break;

			case 5:
			   csMonth.LoadString(IDS_STRING_MAY);// = _T("May");
			   break;

			case 6:
			   csMonth.LoadString(IDS_STRING_JUN);// = _T("June");
			   break;

			case 7:
			   csMonth.LoadString(IDS_STRING_JUL);// = _T("July");
			   break;

			case 8:
			   csMonth.LoadString(IDS_STRING_AUG);// = _T("August");
			   break;

			case 9:
			   csMonth.LoadString(IDS_STRING_SEP);// = _T("September");
			   break;

			case 10:
			   csMonth.LoadString(IDS_STRING_OCT);// = _T("October");
			   break;

			case 11:
			   csMonth.LoadString(IDS_STRING_NOV);// = _T("November");
			   break;

			case 12:
			   csMonth.LoadString(IDS_STRING_DEC);// = _T("December");
			   break;

			}

			//set AM/PM format string 
			int nHr = time.GetHour();
			if(nHr < 12){
				csAMPM = _T("AM");
			}
			else{
				csAMPM = _T("PM");
			}

			//set the hour format string - CTime.GetHour() defaults to military time - convert to 1-12 AM/PM
			switch (nHr)
			{
			case 0:
			case 12:
			   csHour = _T("12");
			   break;

			case 1:
			case 13:
			   csHour = _T("1");
			   break;

			case 2:
			case 14:
			   csHour = _T("2");
			   break;

			case 3:
			case 15:
			   csHour = _T("3");
			   break;

			case 4:
			case 16:
			   csHour = _T("4");
			   break;

			case 5:
			case 17:
			   csHour = _T("5");
			   break;

			case 6:
			case 18:
			   csHour = _T("6");
			   break;

			case 7:
			case 19:
			   csHour = _T("7");
			   break;

			case 8:
			case 20:
			   csHour = _T("8");
			   break;

			case 9:
			case 21:
			   csHour = _T("9");
			   break;

			case 10:
			case 22:
			   csHour = _T("10");
			   break;

			case 11:
			case 23:
			   csHour = _T("11");
			   break;

			}

			//set the minute format string - minutes 0-9 will only display 1 digit from the GetMinute() function
			int nMin = time.GetMinute();
			if(nMin < 10){
				csMin.Format(_T("0%d"), nMin);
			}
			else{
				csMin.Format(_T("%d"), nMin);
			}

			//format output string to display in dialog
			csTimeFormat.Format(_T("%s, %s %d, %d, %s:%s %s"),
								csDay,
								csMonth,
								time.GetDay(),
								time.GetYear(),
								csHour,
								csMin,
								csAMPM);

			//insert string into dialog
			UINT dlgID = IDC_STATIC_MOD1 + x;
			((CStatic *)GetDlgItem (dlgID))->SetWindowText(csTimeFormat);
		}
		
	}
}


BEGIN_MESSAGE_MAP(CDFileSummaryDlg, CDialog)
	//{{AFX_MSG_MAP(CDFileSummaryDlg)
	ON_BN_CLICKED(IDC_BUTTON_SETICONDIR, OnSetIconDir)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDFileSummaryDlg message handlers

BOOL CDFileSummaryDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString csTitle;
	csTitle.Format(_T("%s - File Summary"),csDialogTitle);
	SetWindowText(csTitle);



	//create bold font to bold the dots between Maj - Min - rel versions
	CFont bold;
	VERIFY(bold.CreateFont(
	   12,                        // nHeight
	   0,                         // nWidth
	   0,                         // nEscapement
	   0,                         // nOrientation
	   FW_HEAVY,                 // nWeight
	   FALSE,                     // bItalic
	   FALSE,                     // bUnderline
	   0,                         // cStrikeOut
	   DEFAULT_CHARSET,              // nCharSet
	   OUT_DEFAULT_PRECIS,        // nOutPrecision
	   CLIP_DEFAULT_PRECIS,       // nClipPrecision
	   DEFAULT_QUALITY,           // nQuality
	   DEFAULT_PITCH | FF_DONTCARE,  // nPitchAndFamily
	   _T("Arial")));                 // lpszFacename

	m_Dot.SetFont(&bold,TRUE);
	m_Dot2.SetFont(&bold,TRUE);


	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDFileSummaryDlg::OnSetIconDir() 
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

	// setting 'savedPath' from path stored in edit box 'm_csIconDir' - CSMALL
	savedPath.Format(_T("%s"), m_csIconDir);
	
	ITEMIDLIST *pidl = SHBrowseForFolder(&bi);

	//Displays the selected folder
	if (pidl!=NULL)
		m_csIconDir = FolderName;

	
	((CEdit *)GetDlgItem (IDC_EDIT_ICON_DIR))->SetWindowText(m_csIconDir);
		
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
		edit = CreateWindowEx(0,
							  _T("STATIC"),
							  _T(""),
							  WS_CHILD | WS_VISIBLE | SS_PATHELLIPSIS,
							  0,
							  100,
							  100,
							  50,
							  hwnd,
							  0,
							  ghInstance,
							  NULL);

		HWND ListView = FindWindowEx(hwnd, NULL, _T("SysTreeView32"), NULL);

		//Gets the dimentions of the windows
		GetWindowRect(hwnd, &Dialog);
		GetWindowRect(ListView, &ListViewRect);

		//Sets the listview controls dimentions
		SetWindowPos(ListView,0,(ListViewRect.left-Dialog.left) ,(ListViewRect.top-Dialog.top )-20,290,170,0);
		//Sets the window positions of edit and dialog controls
		SetWindowPos(edit,HWND_BOTTOM,(ListViewRect.left-Dialog.left),(ListViewRect.top-Dialog.top )+170,290,18,SWP_SHOWWINDOW);

		//set edit font
		HFONT hf;
		LOGFONT lf={0};
		HDC hdc = GetDC(edit);
		
		GetObject(GetWindowFont(edit),sizeof(lf),&lf);
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
		if( savedPath != "") {
			if( _taccess(savedPath, 0) == 0)
			{	
				int savedLength = savedPath.GetLength();
				SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)savedPath.GetBuffer(savedLength));
			}
			else if( _taccess(savedPath, 0) == -1)
			{
				AfxMessageBox(_T("Directory does not exist."), MB_OK|MB_ICONEXCLAMATION,-1);
				SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)_T("%SYSROOT%"));
			}
		}

		// set the font for path text on Folder Browse Window
		SendMessage(edit,WM_SETFONT,(WPARAM)hf,TRUE);
		ReleaseDC(edit,hdc);

		
	}

	//Selection change message
	if(uMsg==BFFM_SELCHANGED)
	{
		BOOL t = SHGetPathFromIDList((ITEMIDLIST*)lParam, FolderName);

		//Sets the text of the edit control to the current folder
		SetWindowText(edit,FolderName);

    }
	
	return 0;
}
