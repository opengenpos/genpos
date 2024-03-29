// SaveWindow.cpp : implementation file
//
#include "stdafx.h"
#include "SaveWindow.h"
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*created by steve*/

/////////////////////////////////////////////////////////////////////////////
// CListWindow dialog
CListWindow::CListWindow(CWnd* pParent /*=NULL*/,bool import)
	: CDialog(CListWindow::IDD, pParent)
{
	imageList = NULL;
	_tcscpy(root,_T("C:\\"));
	addressBar=new CStatic;
	userClick=false;
	SelectedNames=_T("");
	//this is just to know if we have to ask to user for a destination or to use 
	//the one that is pre-set
	if(import)
		m_bexport=false;
	else
		m_bexport=true;

	//{{AFX_DATA_INIT(CListWindow)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
void CListWindow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CListWindow)
	DDX_Control(pDX, IDC_FILE_NAME_BOX, m_fileNames);
	DDX_Control(pDX, IDC_DRIVE_COMBO, cmbbox);
	DDX_Control(pDX, IDC_LIST,listctrl);
	//}}AFX_DATA_MAP
}
BEGIN_MESSAGE_MAP(CListWindow, CDialog)
	//{{AFX_MSG_MAP(CListWindow)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList1)
	ON_BN_CLICKED(IDC_BACK_BUTTON, OnUp)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST, OnColumnclickList1)
	ON_CBN_CLOSEUP(IDC_DRIVE_COMBO, OnCloseupCombo1)
	ON_NOTIFY(NM_CLICK, IDC_LIST, OnClickList1)
	ON_EN_CHANGE(IDC_FILE_NAME_BOX, OnChangeEdit1)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CListWindow message handlers
//Got code online  ss
static int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	// If the BFFM_INITIALIZED message is received
	// set the path to the start path.
	switch (uMsg)
	{
		case BFFM_INITIALIZED:
		{
			if (NULL != lpData)
			{
				SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
			}
		}
	}

	return 0; // The function should always return 0.
}
// HWND is the parent window.
// szCurrent is an optional start folder. Can be NULL.
// szPath receives the selected path on success. Must be MAX_PATH characters in length.
BOOL BrowseForFolder(HWND hwnd, LPCTSTR szCurrent, LPTSTR szPath)
{
	BROWSEINFO   bi = { 0 };
	LPITEMIDLIST pidl;
	TCHAR        szDisplay[MAX_PATH];
	BOOL         retval;

	CoInitialize(NULL);
	bi.hwndOwner      = hwnd;
	bi.pszDisplayName = szDisplay;
	bi.lpszTitle      = TEXT("Please choose a destination to export the icons to.");
	bi.ulFlags        = BIF_RETURNONLYFSDIRS ; 
	bi.lpfn           = BrowseCallbackProc;
	bi.lParam         = (LPARAM) szCurrent;

	pidl = SHBrowseForFolder(&bi);

	if (NULL != pidl)
	{
		retval = SHGetPathFromIDList(pidl, szPath);
		CoTaskMemFree(pidl);
	}
	else
	{
		retval = FALSE;
	}

	if (!retval)
	{
		szPath[0] = TEXT('\0');
	}
	CoUninitialize();
	return retval;
}
void CListWindow::OnCancel() 
{
	CDialog::OnCancel();
}
//when the copy button is pushed
void CListWindow::OnOK() 
{
	CDialog::OnOK();
	POSITION pos;
	TCHAR filePathFrom [356];
	TCHAR filePathTo [356];
	int nItem;
	SHFILEOPSTRUCT st = {0};
	TCHAR filenames[356];
	CString progresstitle = _T("Coping From ");
	
	progresstitle += root;
	m_fileNames.GetWindowText(filenames,256);
	pos = listctrl.GetFirstSelectedItemPosition();

	while (pos)
	{
		nItem = listctrl.GetNextSelectedItem(pos);
		// Set our current directory to the root so that we can
		// do relative file copies.
		SetCurrentDirectory(root);
		st.hwnd = NULL;
		st.wFunc = FO_COPY;
		// copy the name of this file to our path buffer.
		// then add an additional string terminater per spec for
		// SHFILEOPSTRUCT documentation.  Then do the same for the to folder.
		_tcscpy (filePathFrom, listctrl.GetItemText(nItem,0));
		filePathFrom[_tcslen (filePathFrom) + 1] = _T('\0');
		st.pFrom = filePathFrom;

		filePathTo[_tcslen (filePathTo) + 1] = _T('\0');
		st.pTo = destination;

		st.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_SIMPLEPROGRESS;
		st.hNameMappings = NULL;
	
		st.lpszProgressTitle = progresstitle;
		SHFileOperation( &st );
	} //end while
}
//converts the systemtime to the correct hour
int CListWindow::GetHour(int hour,CString &ampm)
{
	int realHour = 0;
	if(hour < 18)
	{
		realHour = hour-5;
		ampm = _T("AM");
	}
	else
	{
		realHour = hour-17;
		ampm = _T("PM");
	}
	return realHour;
}
//the main function that creates and adds the info to the list
void CListWindow::createList(){
	//for file search
	HANDLE fileSearch;
	WIN32_FIND_DATA myFoundFile;
	myFoundFile.dwFileAttributes=FILE_ATTRIBUTE_DIRECTORY;
	TCHAR *myFileName = _T("*.*");
	BOOL doit = TRUE;
	//indexes for the list items to be added
	int nColumnCount = 5;
	int nItems = 0;
	int nSubItems = 0;
	LV_ITEM item;
	//file attributes
	CString filesize;
	TCHAR accessTime[400];
	TCHAR createdTime[400];
	TCHAR modifiedTime[400];
	TCHAR fileSize[20];
	DWORD size;
	int hour = 0;	
	SYSTEMTIME Atime;
	SYSTEMTIME Ctime;
	SYSTEMTIME Mtime;
	CString am_pm;

	bool folder = false;
	bool icon = false;
	int actualItem;
	int folderIndex = 0;
	
	//deletes all items in the list
	listctrl.DeleteAllItems();

	//SETS THE ADDRESS BAR WITH THE PATH
	addressBar->SetWindowText(root);

	_tchdir(root);
	fileSearch = FindFirstFile (myFileName, &myFoundFile);
	//gets all files under the root folder
	while(doit&&fileSearch != INVALID_HANDLE_VALUE)
	{
		//Gets the time of creation, access, and modified
		FileTimeToSystemTime(&myFoundFile.ftLastAccessTime, &Atime);
		if(Atime.wMinute < 10)
		{
			//if the minute time is less than 10 then add a 0 infront of the number
			hour = GetHour(Atime.wHour,am_pm);
			wsprintf(accessTime,_T("%d/%d/%d %d:0%d %s"),
				Atime.wMonth, Atime.wDay, Atime.wYear,
				hour, Atime.wMinute,am_pm);
		}	
		else
		{
			hour = GetHour(Atime.wHour,am_pm);
			wsprintf(accessTime,_T("%d/%d/%d %d:%d %s"),
				Atime.wMonth, Atime.wDay, Atime.wYear,
				hour, Atime.wMinute,am_pm);
		}
		FileTimeToSystemTime(&myFoundFile.ftCreationTime, &Ctime);
		if(Ctime.wMinute < 10)
		{
			hour = GetHour(Ctime.wHour,am_pm);
			wsprintf(createdTime,_T("%d/%d/%d %d:0%d%s"),
				Ctime.wMonth, Ctime.wDay, Ctime.wYear,
				hour, Ctime.wMinute,am_pm);
		}
		else
		{
			hour = GetHour(Ctime.wHour,am_pm);
			wsprintf(createdTime,_T("%d/%d/%d %d:%d %s"),
				Ctime.wMonth, Ctime.wDay, Ctime.wYear,
				hour, Ctime.wMinute,am_pm);
		}
		FileTimeToSystemTime(&myFoundFile.ftLastWriteTime, &Mtime);
		if(Mtime.wMinute < 10)
		{
			hour=GetHour(Mtime.wHour,am_pm);
			wsprintf(modifiedTime,_T("%d/%d/%d %d:0%d %s"),
				Mtime.wMonth, Mtime.wDay, Mtime.wYear,
				hour, Mtime.wMinute,am_pm);
		}
		else
		{
			hour = GetHour(Mtime.wHour,am_pm);
			wsprintf(modifiedTime,_T("%d/%d/%d %d:%d %s"),
				Mtime.wMonth, Mtime.wDay, Mtime.wYear,
				hour, Mtime.wMinute,am_pm);
		}
		size = myFoundFile.nFileSizeLow;
		//if its a folder 
		if(myFoundFile.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
		{	
			icon = true;
			filesize =_T("");
			for(nSubItems = 0;nSubItems < 5;nSubItems++)
			{
				if(myFoundFile.cFileName[0] != _T('.'))
				{
					//gets what row and column to add to next
					item.mask = LVIF_TEXT | (nSubItems == 0? LVIF_IMAGE :0);
					item.iItem = (nSubItems == 0)? folderIndex - 2:actualItem;
					item.iSubItem = nSubItems;
					if(icon)
					{
						item.iImage = 1;
					}
					else
					{
						item.iImage = 0;
					}
					//sets row and first column
					if(nSubItems == 0)
					{
						item.pszText = myFoundFile.cFileName;
						actualItem = listctrl.InsertItem(&item);
					}
					//sets second column
					else if(nSubItems == 1)
					{
						wsprintf(fileSize,_T("%s"),filesize);
						item.pszText = fileSize;
						listctrl.SetItem(&item);
					}
					else if(nSubItems == 2)
					{
						item.pszText = createdTime;
						listctrl.SetItem(&item);
					}
					else if(nSubItems == 3)
					{
						item.pszText = modifiedTime;
						listctrl.SetItem(&item);
					}
					else if(nSubItems == 4)
					{
						item.pszText = accessTime;
						listctrl.SetItem(&item);
					}
				}//end if
			}//end for
			folder = true;
			folderIndex++;
		}//end add folder to top
		//if its a file
		else
		{
			int tempsize;
			//gets size of file
			if((size > 999) && size < 999999)
			{
				size = (size/1000) + 1;
				filesize.Format(_T("%d KB"),size);
			}
			else if(size > 999999)
			{
				tempsize = size / 1000000;
				size = (size / 1000) + 1;
				size = size % 1000;
				filesize.Format(_T("%d,%.3d KB"),tempsize,size);
			}
			else 
			{
				filesize.Format(_T("%d B"),size);
			}
			for(nSubItems = 0;nSubItems < 5;nSubItems++)
			{
				if(myFoundFile.cFileName[0] != _T('.'))
				{
					//gets what row and column to add to next
					item.mask = LVIF_TEXT | (nSubItems == 0? LVIF_IMAGE :0);
					item.iItem = (nSubItems == 0)? nItems - 2:actualItem;
					item.iSubItem = nSubItems;
					if(icon)
					{
						item.iImage = 1;
					}
					else
					{
						item.iImage = 0;
					}
					if(!folder)
					{
						//sets row and first column
						if(nSubItems == 0)
						{
							item.pszText = myFoundFile.cFileName;
							actualItem=listctrl.InsertItem(&item);
						}
						//sets second column
						else if(nSubItems == 1)
						{
							wsprintf(fileSize,_T("%s"),filesize);
							item.pszText = fileSize;
							listctrl.SetItem(&item);
						}
						else if(nSubItems == 2)
						{
							item.pszText = createdTime;
							listctrl.SetItem(&item);
						}
						else if(nSubItems == 3)
						{
							item.pszText = modifiedTime;
							listctrl.SetItem(&item);
						}
						else if(nSubItems == 4)
						{
							item.pszText = accessTime;
							listctrl.SetItem(&item);
						}
					}
				}//end if folder
			}//end for
		}//end if directory
		doit = FindNextFile (fileSearch,&myFoundFile);
		nItems++;
		icon=false;
		folder=false;
	}//end while
	FindClose (fileSearch);
}
//creates the colomns on the list
//creates the image list for the icons
//finds the drives on the system
//sets up the addressbar
//sets the destination for the icons
//creates the list in the listwindow 
BOOL CListWindow::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//sets the columns on the list
	listctrl.InsertColumn(0,_T("Name"),LVCFMT_LEFT,150,0);
	listctrl.InsertColumn(1,_T("Size"),LVCFMT_RIGHT,75,1);
	listctrl.InsertColumn(2,_T("Date Created"),LVCFMT_LEFT,120,2);
	listctrl.InsertColumn(3,_T("Date Modified"),LVCFMT_LEFT,120,3);
	listctrl.InsertColumn(4,_T("Date Accessed"),LVCFMT_LEFT,120,4);
	//addes the icons to the icon list
	imageList= new CImageList();
	imageList->Create(16, 16, TRUE,  5, 5);
	imageList->Add(AfxGetApp()->LoadIcon(IDI_BLANK));
	imageList->Add(AfxGetApp()->LoadIcon(IDI_CLSFOLD));
	imageList->Add(AfxGetApp()->LoadIcon(IDI_FLOPPY));
	imageList->Add(AfxGetApp()->LoadIcon(IDI_DRIVE));
	imageList->Add(AfxGetApp()->LoadIcon(IDI_CDDRIVE));
	listctrl.SetImageList(imageList,LVSIL_SMALL);
	addressBar = (CStatic *)GetDlgItem(IDC_STATIC_ADDRESS);
	addressBar->SetWindowText(root);
	cmbbox.SetCurSel(0);
	//gets the drives off the system
	GetDrives();
	int index = 0;
	int equal;
	equal = _tcscmp(Drives[index],_T(""));
	//addes drives to the combo box
	while(equal != 0)
	{
		cmbbox.InsertString(index+2,"  " + Drives[index]);
		index++;
		equal=_tcscmp(Drives[index],_T(""));
	}
	m_fileNames.SetWindowText(_T(""));
	//gets the folder to export the Icons to
	TCHAR iconbuff[MAX_PATH+1];
	if(!m_bexport)
	{
		destination=_T("C:\\FlashDisk\\NCR\\Saratoga\\Icons");   // should be STD_FOLDER_ICONFILES
	}
	else
	{
		BrowseForFolder(this->m_hWnd,NULL,iconbuff);
		destination=iconbuff;
	}
	if(destination != _T(""))
	{
		createList();
	}
	else
		OnCancel();
	return TRUE;  
}
//gets the drives off the system
void CListWindow::GetDrives()
{
	DWORD    dwDrives = 0;
	TCHAR    tcsDriveStrings [256];
	//gets the drives of the system
	dwDrives = GetLogicalDriveStrings (sizeof(tcsDriveStrings)/sizeof(tcsDriveStrings[0]) - 1, tcsDriveStrings);
	int numDrives = dwDrives;
	int diveIndex = 0;
	for(int x = 0;x < numDrives;x++)
	{
		Drives[diveIndex] += tcsDriveStrings[x];
		if(tcsDriveStrings[x] == _T('\0'))
		{
			diveIndex++;
		}
	}
}
//gets the item that was double clicked
void CListWindow::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
//	*pResult = 0;
	int itemNumber = listctrl.GetNextItem(-1,LVNI_SELECTED);
	CString foldername = listctrl.GetItemText(itemNumber,0) + _T("");
	if(listctrl.GetSelectedCount() > 0)
		GetSubFolder(foldername);
	m_fileNames.SetWindowText(_T(""));
	listctrl.SetItemState(0,0|0,LVIS_FOCUSED|LVIS_SELECTED);
	SelectedNames=_T("");
}
//checks to see of the selected item in the list is a folder
//if it is then add the selected folder to the path name 
//if not removes the new added name
void CListWindow::GetSubFolder(CString folderName)
{	
	int test;
	CString testAddress=root;
	if(testAddress.GetLength()>0)
	{
		int index = (testAddress.GetLength()-1);
		if(	testAddress.GetAt(index) == _T('\\'))
		{
			_tcscat(root,folderName);
		}
		else
		{
			_tcscat(root,_T("\\") + folderName);
		}
		test =_tchdir(root);
		if(test == 0)
		{
			listctrl.DeleteAllItems();
			createList();
		}
		else
		{
			removeLastAddress();
		}
	}
	else
	{
		test = _tchdir(folderName);
		if(test == 0)
		{
			_tcscat(root,folderName);
			_tchdir(root);
			listctrl.DeleteAllItems();
			createList();
		}
		else
		{
			AfxMessageBox(_T("Inset a disk into drive ") + folderName);
		}
	}
}
//removes the last directory from the path name
void CListWindow::removeLastAddress()
{
	CString tempRoot=root;
	int index = 0;
	index=tempRoot.ReverseFind(_T('\\'));
	tempRoot = tempRoot.Left(index);
	//checks for root directory
	if(tempRoot.Find(_T('\\')) == -1)
	{
		tempRoot += _T('\\');
	}
	wcscpy(root,tempRoot);
}
//goes up one directory
void CListWindow::OnUp() 
{
	CString text;
	addressBar->GetWindowText(text);
	if(text != _T(""))
	{
		removeLastAddress();
		listctrl.DeleteAllItems();
		createList();
	}
	int driveIndex = 0;
	while(Drives[driveIndex] != _T(""))
	{
		if(text.Compare(Drives[driveIndex]) == 0)
		{
			myComputer();
		}
		driveIndex++;
	}
	m_fileNames.SetWindowText(_T(""));
	SelectedNames=_T("");
	listctrl.SetItemState(0,0|0,LVIS_FOCUSED|LVIS_SELECTED);
}
//reverses the order of the list of names 
static int CALLBACK MyCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
   // lParamSort contains a pointer to the list view control.
   // The lParam of an item is just its index.
   CListCtrl* pListCtrl = (CListCtrl*) lParamSort;
   CString    strItem1 = pListCtrl->GetItemText(lParam1, 0);
   CString    strItem2 = pListCtrl->GetItemText(lParam2-1, 0);
   return _tcscmp(strItem1, strItem2);
}
//reverses the order of the list of names
void CListWindow::OnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	//*pResult = 0;
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int (( CALLBACK * pFunc) (long, long, long)) = MyCompareProc;
	//Storts the list
	if(pNMListView->iSubItem == 0)
	{
		listctrl.SortItems(pFunc, (LPARAM) &listctrl);
	}
}
//if my computer is selected in the combo box
//this gets the list of drives for the system
void CListWindow::myComputer()
{
	LV_ITEM item;
	TCHAR drives[256];
	int driveIndex = 0;
	int equal;
	
	_tcscpy(root,_T(""));
	listctrl.DeleteAllItems();
	addressBar->SetWindowText(_T(""));
	item.mask = LVIF_TEXT| LVIF_IMAGE;
	item.iSubItem = 0;
	equal=_tcscmp(Drives[driveIndex],_T(""));
	while(equal != 0)
	{
		_tcscpy(drives,Drives[driveIndex]);
		item.iItem = driveIndex;
		if(Drives[driveIndex].Find(_T('A')) > -1)
			item.iImage = 2;
		else if(Drives[driveIndex].Find(_T('C')) > -1)
			item.iImage = 3;
		else if(Drives[driveIndex].Find(_T('E')) > -1)
			item.iImage = 4;
		else
			item.iImage = 3;
		
		item.pszText = drives;
		listctrl.InsertItem(&item);
		driveIndex++;
		equal = _tcscmp(Drives[driveIndex],_T(""));
	}
	//to my computer
	cmbbox.SetCurSel(1);
}
//this is called when the desktop is selected in the combo box
void CListWindow::Desktop()
{
	_tcscpy(root,_T(""));
	_tcscpy(root,_T("C:\\Documents and Settings\\All Users\\Desktop"));
	createList();
}
//when the combo box item is selected 
//and the list closes up.
void CListWindow::OnCloseupCombo1() 
{
	int index = 0;
	CString drive;
	int ifChange;
	index=cmbbox.GetCurSel();
	//gets the drive in the cmblist
	cmbbox.GetLBText(index,drive);
	//because desktop or my computer is not a drive this checkes if the user selected them
	if(_tcscmp(drive,_T("Desktop")) != 0  &&  _tcscmp(drive,_T("My Computer")) != 0)
	{
		drive = drive.Mid(2);
		ifChange=_tchdir(drive);
	}
	//checks to see if the change to the drive was successful
	if(ifChange == 0)
	{
		_tcscpy(root,drive);
		createList();
	}
	//if not then changes to one of these
	else if(drive == _T("Desktop"))
	{
		Desktop();
	}
	else if(drive == _T("My Computer"))
	{
		myComputer();
	}
	//if the drive could not be acessed then display this message
	else
	{
		AfxMessageBox("Inset a disk into drive "+drive);
	}
	m_fileNames.SetWindowText(_T(""));
	listctrl.SetItemState(0,0|0,LVIS_FOCUSED|LVIS_SELECTED);
	SelectedNames=_T("");
}
//gets the seleced files and folders and puts them into the fileName
void CListWindow::OnClickList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
//	*pResult = 0;
	int index = 0;
	POSITION pos;
	int count = listctrl.GetSelectedCount();
	if(count > 0)
	{
		SelectedNames = _T("");
		pos=listctrl.GetFirstSelectedItemPosition();
		//if the count is greater than one add "s to the begining and end of the file name
		if(listctrl.GetSelectedCount() > 1)
		{
			while(pos)
			{
				index=listctrl.GetNextSelectedItem(pos);
				SelectedNames += _T("\"") + listctrl.GetItemText(index,0) + _T("\" ");
			}
		}
		else
		{
			index=listctrl.GetNextSelectedItem(pos);
			SelectedNames = listctrl.GetItemText(index,0);
		}
	}
	userClick = true;
	m_fileNames.SetWindowText(SelectedNames);
}
//when the text is changed in the text box (added or removed)
void CListWindow::OnChangeEdit1() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	TCHAR filenames[256];
	int itemCount = listctrl.GetItemCount();
	m_fileNames.GetWindowText(filenames,256);
	CString file_names = filenames;
	CString names;
	int itemIndex;
	bool endSearch = false;
	//if the user selected files and there are " then do nothing
	if(userClick || file_names.Find(_T('\"')) > 0 )
	{
		listctrl.SetFocus();
		userClick = false;
	}
	//else file the file that the user is typeing in
	else
	{
		//cycles through the list
		for(itemIndex = 0; itemIndex < itemCount; itemIndex++)
		{
			listctrl.SetFocus();
			listctrl.SetSelectionMark(itemIndex);
			listctrl.SetItemState(itemIndex,0, 2);
		}
		//cycles through the list
		for( itemIndex = 0;itemIndex < itemCount; itemIndex++)
		{	
			names = listctrl.GetItemText(itemIndex,0);
			if(!endSearch)
			{
				if(file_names.CompareNoCase(names) == 0)
				{
					listctrl.SetItemState(itemIndex,LVIS_FOCUSED|LVIS_SELECTED ,LVIS_FOCUSED|LVIS_SELECTED);
					listctrl.SetFocus();
					listctrl.SetSelectionMark(itemIndex);
					endSearch = true;
				}
				//if file_names is less than names
				else if(file_names.CompareNoCase(names) < 0)
				{
					listctrl.SetItemState(itemIndex,LVIS_FOCUSED|LVIS_SELECTED ,LVIS_FOCUSED|LVIS_SELECTED);
					listctrl.SetFocus();
					listctrl.SetSelectionMark(itemIndex);
					endSearch = true;
				}
				
			}//end search
		}//end for
		m_fileNames.SetFocus();
	}//end else
}

