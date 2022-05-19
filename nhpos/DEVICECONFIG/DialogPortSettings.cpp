// DialogPortSettings.cpp : implementation file
//

#include "stdafx.h"
#include <scf.h>
#include "DeviceConfig.h"
#include "DialogPortSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogPortSettings dialog


CDialogPortSettings::CDialogPortSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogPortSettings::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogPortSettings)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDialogPortSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogPortSettings)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogPortSettings, CDialog)
	//{{AFX_MSG_MAP(CDialogPortSettings)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogPortSettings message handlers

BOOL CDialogPortSettings::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	// We assume that the portSettings string has been set using
	// the following standard notation:
	//  baud,charbits,parity,stopbits,handshake
	//  example:  9600,8,NONE,1,NONE

	CString csValue, csTempStr;

	if (csInterface == SCF_INTERFACE_DIRECT) {
//		CWnd  *pWnd = (CWnd  *)GetDlgItem (IDC_IPADDRESS);
//		pWnd->EnableWindow (FALSE);
//		pWnd = (CWnd  *)GetDlgItem (IDC_EDIT_IP_PORT);
//		pWnd->EnableWindow (FALSE);

		CButton  *pCheckBox = (CButton *)GetDlgItem (IDC_CHECK_IP_PORT);
		pCheckBox->SetCheck(BST_UNCHECKED);

		csValue = port;
		csValue.TrimLeft (); csValue.TrimRight ();

		if (csValue.IsEmpty () || csValue.Compare (_T("")) == 0)
			csValue = _T("COM1");

		CComboBox  *pclComboBox = (CComboBox  *)GetDlgItem (IDC_COMBO_PORT);
		int nSel = pclComboBox->FindString (-1, csValue);
		pclComboBox->SetCurSel (nSel);
		
		csTempStr = portSettings;
		csTempStr.TrimLeft ();
		int nPos = csTempStr.Find (_T(','));

		csValue = csTempStr.Left (nPos);
		csValue.TrimRight (_T(','));
		csTempStr.Delete (0, nPos);
		csTempStr.TrimLeft (_T(','));
		csTempStr.TrimLeft ();

		nPos = csTempStr.Find (_T(','));
		if (nPos >= 0) {
			pclComboBox = (CComboBox  *)GetDlgItem (IDC_COMBO_BAUD);
			nSel = pclComboBox->FindString (-1, csValue);
			pclComboBox->SetCurSel (nSel);
			csValue = csTempStr.Left (nPos);
			csValue.TrimRight (_T(','));
			csTempStr.Delete (0, nPos);
			csTempStr.TrimLeft (_T(','));
			csTempStr.TrimLeft ();
			pclComboBox = (CComboBox  *)GetDlgItem (IDC_COMBO_CHARBITS);
			nSel = pclComboBox->FindString (-1, csValue);
			pclComboBox->SetCurSel (nSel);

			nPos = csTempStr.Find (_T(','));
			csValue = csTempStr.Left (nPos);
			csValue.TrimRight (_T(','));
			csTempStr.Delete (0, nPos);
			csTempStr.TrimLeft (_T(','));
			csTempStr.TrimLeft ();
			pclComboBox = (CComboBox  *)GetDlgItem (IDC_COMBO_PARITY);
			nSel = pclComboBox->FindString (-1, csValue);
			pclComboBox->SetCurSel (nSel);

			nPos = csTempStr.Find (_T(','));
			csValue = csTempStr.Left (nPos);
			csValue.TrimRight (_T(','));
			csTempStr.Delete (0, nPos);
			csTempStr.TrimLeft (_T(','));
			csTempStr.TrimLeft ();
			pclComboBox = (CComboBox  *)GetDlgItem (IDC_COMBO_STOPBITS);
			nSel = pclComboBox->FindString (-1, csValue);
			pclComboBox->SetCurSel (nSel);

		//	nPos = csTempStr.Find (_T(','));
			csValue = csTempStr;
			csValue.TrimRight (_T(','));
			pclComboBox = (CComboBox  *)GetDlgItem (IDC_COMBO_HANDSHAKE);
			nSel = pclComboBox->FindString (-1, csValue);
			pclComboBox->SetCurSel (nSel);
		} else {
			pCheckBox->SetCheck(BST_CHECKED);
			BYTE  nField0, nField1, nField2, nField3;
			CIPAddressCtrl  *pclIPAddress = (CIPAddressCtrl  *)GetDlgItem (IDC_IPADDRESS);
			CEdit *pclEditBox = (CEdit *)GetDlgItem (IDC_EDIT_IP_PORT);		
			
			csTempStr = portSettings;
			csTempStr.TrimLeft ();
			int nPos = csTempStr.Find (_T('.'));

			nField0 = _ttoi(csTempStr.Left (nPos));
			csTempStr.Delete (0, nPos);
			csTempStr.TrimLeft (_T('.'));
			csTempStr.TrimLeft ();

			nPos = csTempStr.Find (_T('.'));
			nField1 = _ttoi(csTempStr.Left (nPos));
			csTempStr.Delete (0, nPos);
			csTempStr.TrimLeft (_T('.'));
			csTempStr.TrimLeft ();

			nPos = csTempStr.Find (_T('.'));
			nField2 = _ttoi(csTempStr.Left (nPos));
			csTempStr.Delete (0, nPos);
			csTempStr.TrimLeft (_T('.'));
			csTempStr.TrimLeft ();

			nPos = csTempStr.Find (_T(','));
			nField3 = _ttoi(csTempStr.Left (nPos));
			csTempStr.Delete (0, nPos);
			csTempStr.TrimLeft (_T(','));
			csTempStr.TrimLeft ();

			pclIPAddress->SetAddress(nField0,nField1,nField2,nField3);
			pclEditBox->SetWindowText (csTempStr);
		}
	}
	else if (csInterface == SCF_INTERFACE_ETHERNET) {
		CWnd  *pWnd = (CWnd  *)GetDlgItem (IDC_COMBO_PORT);
		pWnd->EnableWindow (FALSE);
		pWnd = (CWnd  *)GetDlgItem (IDC_COMBO_BAUD);
		pWnd->EnableWindow (FALSE);
		pWnd = (CWnd  *)GetDlgItem (IDC_COMBO_CHARBITS);
		pWnd->EnableWindow (FALSE);
		pWnd = (CWnd  *)GetDlgItem (IDC_COMBO_PARITY);
		pWnd->EnableWindow (FALSE);
		pWnd = (CWnd  *)GetDlgItem (IDC_COMBO_STOPBITS);
		pWnd->EnableWindow (FALSE);
		pWnd = (CWnd  *)GetDlgItem (IDC_COMBO_HANDSHAKE);
		pWnd->EnableWindow (FALSE);
		
		/* =================================================
			Update to retrieve IP Address and Port from 
			initial dialog to insert into IP Address and
			port for initial settings. ESMITH ETHERNET SUPPORT
		   ================================================= */
		if(!portSettings.IsEmpty())
		{

			BYTE  nField0, nField1, nField2, nField3;
			CIPAddressCtrl  *pclIPAddress = (CIPAddressCtrl  *)GetDlgItem (IDC_IPADDRESS);
			CEdit *pclEditBox = (CEdit *)GetDlgItem (IDC_EDIT_IP_PORT);		
			
			csTempStr = portSettings;
			csTempStr.TrimLeft ();
			int nPos = csTempStr.Find (_T('.'));

			nField0 = _ttoi(csTempStr.Left (nPos));
			csTempStr.Delete (0, nPos);
			csTempStr.TrimLeft (_T('.'));
			csTempStr.TrimLeft ();

			nPos = csTempStr.Find (_T('.'));
			nField1 = _ttoi(csTempStr.Left (nPos));
			csTempStr.Delete (0, nPos);
			csTempStr.TrimLeft (_T('.'));
			csTempStr.TrimLeft ();

			nPos = csTempStr.Find (_T('.'));
			nField2 = _ttoi(csTempStr.Left (nPos));
			csTempStr.Delete (0, nPos);
			csTempStr.TrimLeft (_T('.'));
			csTempStr.TrimLeft ();

			nPos = csTempStr.Find (_T(','));
			nField3 = _ttoi(csTempStr.Left (nPos));
			csTempStr.Delete (0, nPos);
			csTempStr.TrimLeft (_T(','));
			csTempStr.TrimLeft ();

			pclIPAddress->SetAddress(nField0,nField1,nField2,nField3);
			pclEditBox->SetWindowText (csTempStr);
		}
	}
	else if (csInterface == SCF_INTERFACE_SPECIAL) {
		CWnd  *pWnd = (CWnd  *)GetDlgItem (IDC_COMBO_PORT);
		pWnd->EnableWindow (FALSE);
		pWnd = (CWnd  *)GetDlgItem (IDC_COMBO_BAUD);
		pWnd->EnableWindow (FALSE);
		pWnd = (CWnd  *)GetDlgItem (IDC_COMBO_CHARBITS);
		pWnd->EnableWindow (FALSE);
		pWnd = (CWnd  *)GetDlgItem (IDC_COMBO_PARITY);
		pWnd->EnableWindow (FALSE);
		pWnd = (CWnd  *)GetDlgItem (IDC_COMBO_STOPBITS);
		pWnd->EnableWindow (FALSE);
		pWnd = (CWnd  *)GetDlgItem (IDC_COMBO_HANDSHAKE);
		pWnd->EnableWindow (FALSE);
		
		/* =================================================
			Update to retrieve IP Address and Port from 
			initial dialog to insert into IP Address and
			port for initial settings. ESMITH ETHERNET SUPPORT
		   ================================================= */
		if(!portSettings.IsEmpty())
		{

			BYTE  nField0, nField1, nField2, nField3;
			CIPAddressCtrl  *pclIPAddress = (CIPAddressCtrl  *)GetDlgItem (IDC_IPADDRESS);
			CEdit *pclEditBox = (CEdit *)GetDlgItem (IDC_EDIT_IP_PORT);		
			
			csTempStr = portSettings;
			csTempStr.TrimLeft ();
			int nPos = csTempStr.Find (_T('.'));

			nField0 = _ttoi(csTempStr.Left (nPos));
			csTempStr.Delete (0, nPos);
			csTempStr.TrimLeft (_T('.'));
			csTempStr.TrimLeft ();

			nPos = csTempStr.Find (_T('.'));
			nField1 = _ttoi(csTempStr.Left (nPos));
			csTempStr.Delete (0, nPos);
			csTempStr.TrimLeft (_T('.'));
			csTempStr.TrimLeft ();

			nPos = csTempStr.Find (_T('.'));
			nField2 = _ttoi(csTempStr.Left (nPos));
			csTempStr.Delete (0, nPos);
			csTempStr.TrimLeft (_T('.'));
			csTempStr.TrimLeft ();

			nPos = csTempStr.Find (_T(','));
			nField3 = _ttoi(csTempStr.Left (nPos));
			csTempStr.Delete (0, nPos);
			csTempStr.TrimLeft (_T(','));
			csTempStr.TrimLeft ();

			pclIPAddress->SetAddress(nField0,nField1,nField2,nField3);
			pclEditBox->SetWindowText (csTempStr);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogPortSettings::OnOK() 
{
	// TODO: Add extra validation here
	CString csTempStr;
	portSettings.Empty ();

	if (csInterface == SCF_INTERFACE_DIRECT) {
		CComboBox  *pclComboBox = (CComboBox  *)GetDlgItem (IDC_COMBO_PORT);
		pclComboBox->GetWindowText (port);

		CButton  *pCheckBox = (CButton *)GetDlgItem (IDC_CHECK_IP_PORT);

		if (BST_UNCHECKED == pCheckBox->GetCheck()) {
			pclComboBox = (CComboBox  *)GetDlgItem (IDC_COMBO_BAUD);
			pclComboBox->GetWindowText (csTempStr);
			portSettings += csTempStr + _T(",");

			pclComboBox = (CComboBox  *)GetDlgItem (IDC_COMBO_CHARBITS);
			pclComboBox->GetWindowText (csTempStr);
			portSettings += csTempStr + _T(",");

			pclComboBox = (CComboBox  *)GetDlgItem (IDC_COMBO_PARITY);
			pclComboBox->GetWindowText (csTempStr);
			portSettings += csTempStr + _T(",");

			pclComboBox = (CComboBox  *)GetDlgItem (IDC_COMBO_STOPBITS);
			pclComboBox->GetWindowText (csTempStr);
			portSettings += csTempStr + _T(",");

			pclComboBox = (CComboBox  *)GetDlgItem (IDC_COMBO_HANDSHAKE);
			pclComboBox->GetWindowText (csTempStr);
			portSettings += csTempStr;
		} else {
			BYTE  nField0, nField1, nField2, nField3;

			CIPAddressCtrl  *pclIPAddress = (CIPAddressCtrl  *)GetDlgItem (IDC_IPADDRESS);
			pclIPAddress->GetAddress(nField0, nField1, nField2, nField3 );
			portSettings.Format (_T("%d.%d.%d.%d,"), nField0, nField1, nField2, nField3);
			CEdit *pclEditBox = (CEdit *)GetDlgItem (IDC_EDIT_IP_PORT);
			pclEditBox->GetWindowText (csTempStr);
			portSettings += csTempStr;
		}
	}
	else if (csInterface == SCF_INTERFACE_ETHERNET || csInterface == SCF_INTERFACE_SPECIAL) {
		BYTE  nField0, nField1, nField2, nField3;

		CIPAddressCtrl  *pclIPAddress = (CIPAddressCtrl  *)GetDlgItem (IDC_IPADDRESS);
		pclIPAddress->GetAddress(nField0, nField1, nField2, nField3 );
		portSettings.Format (_T("%d.%d.%d.%d,"), nField0, nField1, nField2, nField3);
		CEdit *pclEditBox = (CEdit *)GetDlgItem (IDC_EDIT_IP_PORT);
		pclEditBox->GetWindowText (csTempStr);
		portSettings += csTempStr;
	}

	CDialog::OnOK();
}
