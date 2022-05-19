// DialogOptions.cpp : implementation file
//
#include "stdafx.h"
#include <scf.h>
#include "DeviceConfig.h"
#include "DeviceConfigDlg.h"
#include "ScfInterface.h"
#include "ScfParameters.h"
#include "pif.h"
#include "DialogDeviceList.h"
//#include "DialogDrawerList.h"
#include "DialogPortSettings.h"
#include "NetworkPortDlg.h"
#include <Winsock.h>
#include <Windows.h>
#include "DialogOptions.h"
#include "scf.h"
#include "DialogOptionValue.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogOptions dialog


CDialogOptions::CDialogOptions(CWnd* pParent /*=NULL*/)
	: CEquipmentDialog(CDialogOptions::IDD, pParent)
{
	m_dwDevCount = 0;
	//{{AFX_DATA_INIT(CDialogOptions)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDialogOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogOptions)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogOptions, CDialog)
	//{{AFX_MSG_MAP(CDialogOptions)
	ON_NOTIFY(NM_CLICK, IDC_LIST_OPTIONS, OnClickListOptions)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_OPTIONS, OnDblclkListOptions)
	ON_BN_CLICKED(IDC_OPTION_CHANGE, OnOptionChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDialogOptions::AddColumn(CListCtrl  *pclView, LPCTSTR strItem,int nItem,int nSubItem,int nMask,int nFmt)
{
	LV_COLUMN lvc;
	lvc.mask = nMask;
	lvc.fmt = nFmt;
	lvc.pszText = (LPTSTR) strItem;
	lvc.cx = pclView->GetStringWidth(lvc.pszText) + 100;
	if(nMask & LVCF_SUBITEM){
		if(nSubItem != -1)
			lvc.iSubItem = nSubItem;
		else
			lvc.iSubItem = nItem;
	}
	return pclView->InsertColumn(nItem,&lvc);
}

BOOL CDialogOptions::AddItem(CListCtrl  *pclView, int nItem,int nSubItem,LPCTSTR strItem,int nImageIndex)
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
// CDialogOptions message handlers

BOOL CDialogOptions::OnInitDialog() 
{
	CString myControlType;
	CString myControlInterface;
	CDialog::OnInitDialog();

	CListCtrl  *pclView = (CListCtrl  *)GetDlgItem (IDC_LIST_OPTIONS);

	readOptions ();

	myControlType.LoadString (IDS_HEADING_7);
	AddColumn (pclView, myControlType, 0);
	myControlType.LoadString (IDS_HEADING_9);
	AddColumn (pclView, myControlType, 1);
	myControlType.LoadString (IDS_HEADING_8);
	AddColumn (pclView, myControlType, 2);

	//For loop is used to interate through lpDevNames and lpDllNames and display in a dialog option window
	//iDevCount is initialized and stores the number of parameters available for the device
	DWORD  iDevCount;
	for (iDevCount = 0; iDevCount < m_dwDevCount; iDevCount++) 
	{	
		//AddItem is used to display results in seperate columns of the dialog option window
		AddItem (pclView, iDevCount-1, 0, xNameList[iDevCount]);
		AddItem (pclView, iDevCount-1, 1, xTypeList[iDevCount]);
		AddItem (pclView, iDevCount-1, 2, xValueList[iDevCount]);
		//The default values are stored in the following CString Array, which are used in other DialogOption functions below.
		xTempValueList[iDevCount]=xValueList[iDevCount];
	}	
	
	return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogOptions::OnClickListOptions(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CDialogOptions::OnDblclkListOptions(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CDialogOptionValue myValueDlg;
	CDialogDeviceList myDevListDialog;
	int nItem;

	CListCtrl  *pclView = (CListCtrl  *)GetDlgItem (IDC_LIST_OPTIONS);
	POSITION pos = pclView->GetFirstSelectedItemPosition();
	if (pos == NULL) 
	{
		TRACE0("No items were selected!\n");
	}
	else
	{	
		//Used to get the default value for the parameter selected and store it in m_newValue so that the OptionChange dialog window will display
		//the currently set value in the OptionCvhange text field
		while (pos)
		{
			nItem = pclView->GetNextSelectedItem(pos);
			myValueDlg.m_newValue = pclView->GetItemText(nItem, 2);
		}

	}

	int iRet = myValueDlg.DoModal();
	
	if(iRet)
	{
		if(csDeviceName == _T("Cash Drawer"))
		{
			//if user changes the currently set value, the new value is stored in the xTempValueList
			xTempValueList[0]=myValueDlg.m_newValue;
			AddItem (pclView, nItem, 2, xTempValueList[0]);//The value for the parameter is updated in the option list with the new value	
		}
		else
		{
			//if user changes the currently set value, the new value is stored in the xTempValueList
			xTempValueList[nItem+1]=myValueDlg.m_newValue;
			AddItem (pclView, nItem, 2, xTempValueList[nItem+1]);
		}
	}

	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CDialogOptions::OnOptionChange() 
{
	CDialogOptionValue myValueDlg;
	CDialogDeviceList myDevListDialog;
	int nItem;

	CListCtrl  *pclView = (CListCtrl  *)GetDlgItem (IDC_LIST_OPTIONS);
	POSITION pos = pclView->GetFirstSelectedItemPosition();
		
	if (pos == NULL) 
	{
		TRACE0("No items were selected!\n");
	}
	else
	{	
		//Used to get the default value for the parameter selected and store it in m_newValue so that the OptionChange dialog window will display
		//the currently set value in the OptionCvhange text field
		while (pos)
		{
			nItem = pclView->GetNextSelectedItem(pos);
			myValueDlg.m_newValue = pclView->GetItemText(nItem, 2);
			
		}

	}

	int iRet = myValueDlg.DoModal();
	
	if(iRet)
	{
		if(csDeviceName == _T("Cash Drawer"))
		{
			//if user changes the currently set value, the new value is stored in the xTempValueList
			xTempValueList[0]=myValueDlg.m_newValue;
			AddItem (pclView, nItem, 2, xTempValueList[0]);//The value for the parameter is updated in the option list with the new value	
		}
		else
		{
			//if user changes the currently set value, the new value is stored in the xTempValueList
			xTempValueList[nItem+1]=myValueDlg.m_newValue;
			AddItem (pclView, nItem, 2, xTempValueList[nItem+1]);//The value for the parameter is updated in the option list with the new value
		}
	}
	// TODO: Add your control notification handler code here
	
}

BOOL CDialogOptions::readOptions()
{
	TCHAR  atchDeviceName [SCF_USER_BUFFER];
	DWORD  dwCount, dwDataType;
	BOOL   bRet = FALSE;
	DWORD  iDevCount;

	if(csDeviceName == _T("CashDrawer"))
	{
		ScfGetParameter (csDeviceName, SCF_DATANAME_OPTION, &dwDataType, atchDeviceName, sizeof(atchDeviceName), &dwCount);
		xNameList[0]=SCF_DATANAME_OPTION;
	
		if(dwDataType == SCF_DATA_STRING)
		{
			xTypeList[0]=_T("REG_SZ");
		}
		else if(dwDataType == SCF_DATA_BINARY)
		{
			xTypeList[0]=_T("REG_DWORD");
		}
	
		xValueList[0]=atchDeviceName;
		xTempValueList[0]=xValueList[0];
		m_dwDevCount = 1;
		return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}
	else
	{
		LPTSTR pDevName,pDevValue;
		TCHAR lpDevNames [4024];    // some devices may have a lot of text with settings created by NCR RSM utility
		TCHAR lpDllNames [4024];    // some devices may have a lot of text with settings created by NCR RSM utility
		DWORD *pdwValue = (DWORD *) atchDeviceName;

		//ScfGetOPOSParameters gets the currently active OPOS device, parameter names and their values.  The parameter names 
		//are stored in lpDevNames and the values are stored in lpDllNames
		m_dwDevCount = 0;
		ScfGetOPOSParameters(csDeviceName, csDeviceNameOption, &m_dwDevCount, lpDevNames, lpDllNames, &dwDataType);

		// Initialize our points for the list of names to the first names of each type.
		// We will then iterate through the list of names, incrementing these pointers as
		// we go.
		pDevName = lpDevNames;
		pDevValue = lpDllNames;
		
		//For loop is used to interate through lpDevNames and lpDllNames and display in a dialog option window
		//iDevCount is initialized and stores the number of parameters available for the device
		for (iDevCount = 0; iDevCount < m_dwDevCount; iDevCount++) 
		{	
			//	ScfGetOPOSType is used to get the Type for each parameter 
			ScfGetOPOSType(csDeviceName,csDeviceNameOption, pDevName, &dwDataType, atchDeviceName, sizeof(atchDeviceName), &dwCount);
			if(iDevCount!=0)
			{
				if(dwDataType == SCF_DATA_STRING)
				{
					//The default values are stored in the following CString Array, which are used in other DialogOption functions below.
					xNameList[iDevCount]=pDevName;
					xTypeList[iDevCount]=_T("REG_SZ");
					xValueList[iDevCount]=pDevValue;
				}
				else if(dwDataType == SCF_DATA_BINARY)
				{
					CString csTempValue; 
					csTempValue.Format(_T("%d"), *pdwValue); //Converts a DWORD value into a decimal so it is easier for the user to recognize
					xNameList[iDevCount]=pDevName;
					xTypeList[iDevCount]=_T("REG_DWORD");
					xValueList[iDevCount]=csTempValue;
				}
				else 
				{
					TRACE2("%S(%d):  CDialogOptions::OnInitDialog() Unknown data type from ScfGetOPOSType()\n", __FILE__, __LINE__);
				}
			}
			else
			{
				//Stores the (defualt) registry entry as the first value in each CString array
				xNameList[iDevCount]=pDevName;
				xTypeList[iDevCount]=_T("REG_SZ");
				xValueList[iDevCount]=pDevValue;
			}
			
			pDevName += _tcslen(pDevName) + 1;
			pDevValue += _tcslen(pDevValue) + 1;
		}
	}

	return bRet;
}

void CDialogOptions::writeChanges()
{
	DWORD   dwCount;
	TCHAR   atchDeviceOptions[SCF_USER_BUFFER];
	memset(&atchDeviceOptions, 0x00, sizeof(atchDeviceOptions));

	if(csDeviceName==_T("Cash Drawer"))
	{
		if(xTypeList[0]=="REG_SZ")
		{
			//Function writes the new value entered by the user into the registry
			_tcscpy(atchDeviceOptions,xTempValueList[0]); 
			ScfSetParameter(csDeviceName, xNameList[0],SCF_DATA_STRING, atchDeviceOptions,_tcslen(atchDeviceOptions) * sizeof(TCHAR), &dwCount);
		}
		else if(xTypeList[0]=="REG_DWORD")
		{
			//Function writes the new value entered by the user into the registry
			_tcscpy(atchDeviceOptions,xTempValueList[0]); 
			ScfSetParameter(csDeviceName, xNameList[0],SCF_DATA_BINARY, atchDeviceOptions,_tcslen(atchDeviceOptions) * sizeof(DWORD), &dwCount);
		}
	}
	else
	{		
		//Writes the values changed by the user into the registry for the selected device
		for(DWORD i=1; i < m_dwDevCount; i++)
		{
			if(xTypeList[i]=="REG_SZ")
			{
				_tcscpy(atchDeviceOptions,xTempValueList[i]); 
				ScfSetOPOSParameter(csDeviceName,csDeviceNameOption, xNameList[i],SCF_DATA_STRING,atchDeviceOptions,_tcslen(atchDeviceOptions) * sizeof(TCHAR), &dwCount);
			}
			else if(xTypeList[i]=="REG_DWORD")
			{
				_tcscpy(atchDeviceOptions,xTempValueList[i]); 
				ScfSetOPOSParameter(csDeviceName,csDeviceNameOption, xNameList[i],SCF_DATA_BINARY,atchDeviceOptions,_tcslen(atchDeviceOptions) * sizeof(DWORD), &dwCount);
				TRACE1("Items value is %s\n", (DWORD)atchDeviceOptions);
			}
			TRACE1("Device Name %s Binary was selected!\n", csDeviceName);
			TRACE1("Device Option %s Binary was selected!\n", csDeviceNameOption);
			TRACE1("Name %s Binary was selected!\n", xNameList[i]);
			TRACE1("Type %s Binary was selected!\n", xTypeList[i]);
			TRACE1("Value %s Binary was selected!\n", xValueList[i]);
		}
	}	
}

//If the user decides not to make any changes to any of the parameters the defualt values are restored.
void CDialogOptions::GetDataFrom(CDialogOptions &fromDialog)
{
	int i;

	csDeviceName = fromDialog.csDeviceName;
	csDeviceNameOption = fromDialog.csDeviceNameOption;
	m_dwDevCount = fromDialog.m_dwDevCount;

	for (i = 0; i < sizeof(xNameList)/sizeof(xNameList[0]); i++) 
	{
		xNameList[i]=fromDialog.xNameList[i]; 
		xTypeList[i]=fromDialog.xTypeList[i];
		xValueList[i]=fromDialog.xValueList[i];
		xTempValueList[i]=fromDialog.xValueList[i];
	}

}