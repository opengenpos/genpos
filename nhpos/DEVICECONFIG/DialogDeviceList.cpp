// DialogDeviceList.cpp : implementation file
//


#include "stdafx.h"
#include "DeviceConfig.h"
#include "DialogDeviceList.h"
#include "ecr.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogDeviceList dialog


CDialogDeviceList::CDialogDeviceList(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDeviceList::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogDeviceList)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDialogDeviceList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDeviceList)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDeviceList, CDialog)
	//{{AFX_MSG_MAP(CDialogDeviceList)
	ON_NOTIFY(NM_CLICK, IDC_LIST_DEVICES, OnClickListDevices)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CDialogDeviceList::AddColumn(CListCtrl  *pclView, LPCTSTR strItem,int nItem,int nSubItem,int nMask,int nFmt)
{
	LV_COLUMN lvc;
	lvc.mask = nMask;
	lvc.fmt = nFmt;
	lvc.pszText = (LPTSTR) strItem;
	lvc.cx = pclView->GetStringWidth(lvc.pszText) + 15;
	if(nMask & LVCF_SUBITEM){
		if(nSubItem != -1)
			lvc.iSubItem = nSubItem;
		else
			lvc.iSubItem = nItem;
	}
	return pclView->InsertColumn(nItem,&lvc);
}

BOOL CDialogDeviceList::AddItem(CListCtrl  *pclView, int nItem,int nSubItem,LPCTSTR strItem,int nImageIndex)
{
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = nItem;
	lvItem.iSubItem = nSubItem;
	lvItem.pszText = (LPTSTR) strItem;
	if(nImageIndex != -1){
		lvItem.mask |= LVIF_IMAGE;
		lvItem.iImage |= LVIF_IMAGE;
	}
	if(nSubItem == 0)
		return pclView->InsertItem(&lvItem);
	return pclView->SetItem(&lvItem);
}

/////////////////////////////////////////////////////////////////////////////
// CDialogDeviceList message handlers

void CDialogDeviceList::OnClickListDevices(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CListCtrl  *pclView = (CListCtrl *)GetDlgItem (IDC_LIST_DEVICES);
	POSITION pos = pclView->GetFirstSelectedItemPosition();

	if (pos == NULL) {
		TRACE0("No items were selected!\n");
	}
	else
	{
		TCHAR tcItemText[128];

		while (pos)
		{
			int nItem = pclView->GetNextSelectedItem(pos);
			TRACE1("Item %d was selected!\n", nItem);
			// you could do your own processing on nItem here
			pclView->GetItemText(nItem, 0, tcItemText, sizeof(tcItemText)/sizeof(TCHAR));
			TRACE1("Item name is %s\n", tcItemText);
			csNewDevice = tcItemText;
			break;
		}
	}
	
	*pResult = 0;
}


BOOL CDialogDeviceList::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	CString myControlType;

	TCHAR DevListCC [STD_MAX_NUM_DEVICE][48];
	TCHAR *(ListOfDevices [STD_MAX_NUM_DEVICE]);
	DEVICE_CAPS_OPOS ListOfDevicesOPOS [STD_MAX_NUM_DEVICE];

	for (int i = 0; i < STD_MAX_NUM_DEVICE; i++) {
		ListOfDevices[i] = &DevListCC[i][0]; DevListCC[i][0] = _T('\0');
	}

	DWORD dwRet = SCF_NO_DEVICE;
	DWORD dwCountx = 0;
	DWORD nItem;
	CListCtrl  *pclView = (CListCtrl  *)GetDlgItem (IDC_LIST_DEVICES);

	if (csInterface == SCF_INTERFACE_OPOS) {
		dwRet = ScfGetOPOSDevice(csDeviceType, &dwCountx, ListOfDevicesOPOS);
		// look for the default to find the OPOS device name
		nItem = 0;

		if(csDeviceType == _T("POSPrinter"))
		{
			// if the device is the OPOS printer then lets insert as the
			// first item
			_tcscpy(ListOfDevices[nItem], SCF_OPOS_NAME_WIN_PRINTER);
			nItem++;
		}
		for (int i = 0; i < dwCountx; i++, nItem++) {
			_tcscpy (ListOfDevices[nItem], ListOfDevicesOPOS[i].aszNameOPOS);
		}
		if(csDeviceType == _T("POSPrinter"))
		{
			// if device is OPOS printer then we need to increment max number of items.
			dwCountx++;
		}
	}
	else  {
		dwRet = ScfGetStandardDeviceByInterface (csDeviceType, csInterface, &dwCountx, ListOfDevices);
	}

	if (dwRet == SCF_SUCCESS) {
		myControlType.LoadString (IDS_HEADING_1);
		AddColumn (pclView, myControlType, 0); 

		AddItem (pclView, 0, 0, _T("NONE"));
		for (nItem = 1; nItem <= dwCountx; nItem++) {
			AddItem (pclView, nItem, 0, ListOfDevices[nItem-1]);
		}
	}

		//SR 490 & 491, We do not want device config to be able to run while NHPOS is running
	//If it IS running while NHPOS is already running, it will be in a "read only" mode
	//where the user can read the information in device config, but cannot make any new changes
	HANDLE hMutex;
	hMutex = OpenMutex(MUTEX_ALL_ACCESS, 0, _T("NeighborhoodPOSApp"));

	//If the mutex does not exist, we will create it, because this is the first time that
	//the application has been created.  
	if (!hMutex)
	{
	  // Mutex doesn’t exist. This is
	  // the first instance so create
	  // the mutex.
	  hMutex = 
		CreateMutex(0, 0, _T("DeviceConfigApp"));
	}else
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);	
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
