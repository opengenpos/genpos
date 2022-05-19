// DialogInitialReset.cpp : implementation file
//

#include "stdafx.h"
#include "DeviceConfig.h"
#include "DialogInitialReset.h"
#include "PluUtilities.h"
#include "PluTtlD.H"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogInitialReset dialog


CDialogInitialReset::CDialogInitialReset(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogInitialReset::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogInitialReset)
	//}}AFX_DATA_INIT

	for (int i = 0; i < 10; i++) {
		arrayInitialReset[i] = -1;
	}
}


void CDialogInitialReset::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogInitialReset)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BOOL CDialogInitialReset::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString strSelection;
	CComboBox  *pclComboBox = (CComboBox  *)GetDlgItem (IDC_INITRESET_SELECT);
	int i, j,k;

	/*
		Populate the combo box with the strings for the different types
		of clear operations.  We put these into the combo box and remember
		which operation goes with which combo box selection.
	 */

	for(i = IDS_INITRESET_PERFORM_RESET,j = 0, k = INIRST_ALL_REST;	i <= IDS_INITRESET_PERFORM_EEPTSTO; i++, j++, k++)
	{
		strSelection.LoadString(i);
		pclComboBox->AddString(strSelection);
		arrayInitialReset[j] = k;
	}


	pclComboBox->SetCurSel (0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_MESSAGE_MAP(CDialogInitialReset, CDialog)
	//{{AFX_MSG_MAP(CDialogInitialReset)
	ON_BN_CLICKED(IDC_INITRESET_CLEAR, ON_IDC_INITRESET_CLEAR)
	ON_BN_CLICKED(IDC_INITRESET_EXIT, ON_IDC_INITRESET_EXIT)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogInitialReset message handlers

void CDialogInitialReset::ON_IDC_INITRESET_CLEAR()
{
	INT nCurSel;
	CString strMsg;


	/*
		Retrieve the current selection from the combo box by get the current
		selection number, the combo box item number, and then using the item
		number to determine which of the operations we are being asked to
		perform.
	 */
	CComboBox  *pclComboBox = (CComboBox  *)GetDlgItem (IDC_INITRESET_SELECT);
	nCurSel = pclComboBox->GetCurSel();

	if (nCurSel == CB_ERR) {
		nCurSel = 0;
	}
	else {
		/* ---- Promp to Check Selection ---- */
		strMsg.LoadString(IDS_INITRESET__WARNING);

		if(AfxMessageBox(strMsg,MB_OKCANCEL) == IDOK)
		{
			/* ---- Clear item Selected ---- */
			if(ExecReset(arrayInitialReset[nCurSel]) == RESET_ERR)
			{
				strMsg.LoadString(IDS_INITRESET_ERR_MSG);
				AfxMessageBox(strMsg,MB_ICONEXCLAMATION);
			}
		}
	}
}

int	CDialogInitialReset::ExecReset(const UINT uKeyCode)
{
	DWORD     dwDelay = 1000;
	int       nRet = RESET_ERR;
	CStatic   *pclStatusLabel = (CStatic  *)GetDlgItem (IDC_STATIC_INITRESET_STATUS);
	PLUTOTAL_DB_HANDLE      g_hdDB;             /* database handle          */

	switch(uKeyCode)
	{
		case INIRST_ALL_REST:
			pclStatusLabel->SetWindowText(L"Begin Clear All");
			Sleep (dwDelay);    // slight delay to give humans time to see the status change.

			/*=============================================================
				Initial Reset will only clear the Flashdisk db (except for
				the defaultlayout file) and it will also clear out temp
				disk. It will not clear out PLU Total Database. ESMITH
			=============================================================*/
			/* ---- Clear Temp Database ---- */
			pclStatusLabel->SetWindowText(L"Clearing Temp Folder");
			Sleep (dwDelay);    // slight delay to give humans time to see the status change.
			if ( ! OpenDir(s_TempBase) ) {
				AfxMessageBox (_T("Error opening TempDir folder."));
				break;
			} else {
				DelFiles();
				CloseDir();
			}

			/* ---- Clear Saratoga Database of PARA* and NHPOS* and DFPR_* ---- */
			pclStatusLabel->SetWindowText(L"Clearing Parameter files and data");
			Sleep (dwDelay);    // slight delay to give humans time to see the status change.
			if ( ! OpenDir(s_DataBase) ) {
				AfxMessageBox (_T("Error opening Database folder."));
				break;
			} else {
				DelFiles(INIRST_FILES_ALLRESET1);
				DelFiles(INIRST_FILES_ALLRESET2);
				DelFiles(INIRST_FILES_ALLRESET3);
				CloseDir();
			}

			/*----- Overwrite (PCI-DSS compliance) and Clear Store and Forward File EEPTSTORE -----*/
			pclStatusLabel->SetWindowText(L"Overwriting Store And Forward");
			Sleep (dwDelay);    // slight delay to give humans time to see the status change.
			if ( ! OpenDir(s_DataBase) ) {
				AfxMessageBox (_T("Error opening Database folder."));
				break;
			} else {
				OverWriteDelFiles(INIRST_FILES_EEPTSTORE_FILE);
				CloseDir();
			}

			/* ---- Clear Saratoga Database of Total files ---- */
			pclStatusLabel->SetWindowText(L"Clearing financial totals");
			Sleep (dwDelay);    // slight delay to give humans time to see the status change.
			OpenDir(s_DataBase);
			DelFiles(INIRST_FILES_TOTALCLEAR1);
			DelFiles(INIRST_FILES_TOTALCLEAR2);
			CloseDir();

			/* ---- Clear the Saved Totals files from the SavedTotal folder in the Database folder ---- */
			pclStatusLabel->SetWindowText(L"Clearing Saved totals");
			Sleep (dwDelay);    // slight delay to give humans time to see the status change.
			OpenDir(s_SavedTotals);
			DelFiles (INIRST_FILES_SAVEDTOTAL_FILE);
			CloseDir();

			/* ----- Clear PLU DB in SQL Server by dropping the tables -------*/
			pclStatusLabel->SetWindowText(L"Clearing PLU totals");
			g_hdDB = PLUTOTAL_HANDLE_NULL;
			PluTotalOpen(&g_hdDB,TRUE,FALSE);

			#if !defined(_WIN32_WCE) || defined(_WIN32_WCE_EMULATION)
				pclStatusLabel->SetWindowText(L"Clearing Daily PLU totals");
				PluTotalDropTbl(g_hdDB,(SHORT)PLUTOTAL_ID_DAILY_CUR);
				PluTotalDropTbl(g_hdDB,(SHORT)PLUTOTAL_ID_DAILY_SAV);
				PluTotalDropTbl(g_hdDB,(SHORT)PLUTOTAL_ID_DAILY_CUR_S);
				PluTotalDropTbl(g_hdDB,(SHORT)PLUTOTAL_ID_DAILY_SAV_S);

				pclStatusLabel->SetWindowText(L"Clearing PTD PLU totals");
				PluTotalDropTbl(g_hdDB,(SHORT)PLUTOTAL_ID_PTD_CUR);
				PluTotalDropTbl(g_hdDB,(SHORT)PLUTOTAL_ID_PTD_SAV);
				PluTotalDropTbl(g_hdDB,(SHORT)PLUTOTAL_ID_PTD_CUR_S);
				PluTotalDropTbl(g_hdDB,(SHORT)PLUTOTAL_ID_PTD_SAV_S);

				PluTotalDropTbl(g_hdDB,(SHORT)PLUTOTAL_ID_TMP_CUR);
				PluTotalDropTbl(g_hdDB,(SHORT)PLUTOTAL_ID_TMP_CUR_S);
			#else
				PluTotalDelFile(PLUTOTAL_ID_DAILY_CUR);
				PluTotalDelFile(PLUTOTAL_ID_DAILY_SAV);
				PluTotalDelFile(PLUTOTAL_ID_DAILY_CUR_S);
				PluTotalDelFile(PLUTOTAL_ID_DAILY_SAV_S);

				PluTotalDelFile(PLUTOTAL_ID_PTD_CUR);
				PluTotalDelFile(PLUTOTAL_ID_PTD_SAV);
				PluTotalDelFile(PLUTOTAL_ID_PTD_CUR_S);
				PluTotalDelFile(PLUTOTAL_ID_PTD_SAV_S);

				PluTotalDelFile(PLUTOTAL_ID_TMP_CUR);
				PluTotalDelFile(PLUTOTAL_ID_TMP_CUR_S);
			#endif

			PluTotalClose(g_hdDB);

			pclStatusLabel->SetWindowText(L"Completed Clear Request");
			nRet = RESET_SUCCESS;
			break;

		case INIRST_ALL_NOTLAY:
			pclStatusLabel->SetWindowText(L"Begin Clear All Except Layouts");
			Sleep (dwDelay);    // slight delay to give humans time to see the status change.

			/* ----- Clear Temp DataBase folder -----*/
			pclStatusLabel->SetWindowText(L"Clearing Temp Folder");
			Sleep (dwDelay);    // slight delay to give humans time to see the status change.
			OpenDir(s_TempBase);
			DelFiles();
			CloseDir();

			/* ---- Clear Saratoga Database of NHPOS* and PARA* (except Layouts) and DFPR_* ---- */
			pclStatusLabel->SetWindowText(L"Clearing Parameter files and data Except Layouts");
			Sleep (dwDelay);    // slight delay to give humans time to see the status change.
			OpenDir(s_DataBase);
			DelFiles(INIRST_FILES_ALLNOTLAY1);
			DelFiles(INIRST_FILES_ALLNOTLAY2);
			DelFiles(INIRST_FILES_ALLNOTLAY3);
			DelFiles(INIRST_FILES_ALLNOTLAY4);
			DelFiles(INIRST_FILES_ALLNOTLAY5);
			DelFiles(INIRST_FILES_ALLNOTLAY6);
			DelFiles(INIRST_FILES_ALLNOTLAY7);
			DelFiles(INIRST_FILES_ALLNOTLAY8);
			DelFiles(INIRST_FILES_ALLNOTLAY9);
			DelFiles(INIRST_FILES_ALLRESET2);
			DelFiles(INIRST_FILES_ALLRESET3);
			CloseDir();

			/*----- Overwrite (PCI-DSS compliance) and Clear Store and Forward File EEPTSTORE -----*/
			pclStatusLabel->SetWindowText(L"Clear and Overwrite Store and Forward");
			Sleep (dwDelay);    // slight delay to give humans time to see the status change.
			OpenDir(s_DataBase);
			OverWriteDelFiles(INIRST_FILES_EEPTSTORE_FILE);
			CloseDir();

			/* ----- Clear Saratoga Totals Database -----*/
			pclStatusLabel->SetWindowText(L"Clearing Financial Totals");
			Sleep (dwDelay);    // slight delay to give humans time to see the status change.
			OpenDir(s_DataBase);
			DelFiles(INIRST_FILES_TOTALCLEAR1);
			DelFiles(INIRST_FILES_TOTALCLEAR2);
			CloseDir();

			/* ---- Clear the Saved Totals files from the SavedTotals folder in the Database folder ---- */
			pclStatusLabel->SetWindowText(L"Clearing Saved totals");
			Sleep (dwDelay);    // slight delay to give humans time to see the status change.
			OpenDir(s_SavedTotals);
			DelFiles (INIRST_FILES_SAVEDTOTAL_FILE);
			CloseDir();

			/* ----- Clear PLU DB in SQL Server by dropping the tables -------*/
			pclStatusLabel->SetWindowText(L"Clearing PLU totals");
			g_hdDB = PLUTOTAL_HANDLE_NULL;
			PluTotalOpen(&g_hdDB,TRUE,FALSE);

			#if !defined(_WIN32_WCE) || defined(_WIN32_WCE_EMULATION)
				pclStatusLabel->SetWindowText(L"Clearing Daily PLU totals");
				PluTotalDropTbl(g_hdDB,(SHORT)PLUTOTAL_ID_DAILY_CUR);
				PluTotalDropTbl(g_hdDB,(SHORT)PLUTOTAL_ID_DAILY_SAV);
				PluTotalDropTbl(g_hdDB,(SHORT)PLUTOTAL_ID_DAILY_CUR_S);
				PluTotalDropTbl(g_hdDB,(SHORT)PLUTOTAL_ID_DAILY_SAV_S);

				pclStatusLabel->SetWindowText(L"Clearing PTD PLU totals");
				PluTotalDropTbl(g_hdDB,(SHORT)PLUTOTAL_ID_PTD_CUR);
				PluTotalDropTbl(g_hdDB,(SHORT)PLUTOTAL_ID_PTD_SAV);
				PluTotalDropTbl(g_hdDB,(SHORT)PLUTOTAL_ID_PTD_CUR_S);
				PluTotalDropTbl(g_hdDB,(SHORT)PLUTOTAL_ID_PTD_SAV_S);

				PluTotalDropTbl(g_hdDB,(SHORT)PLUTOTAL_ID_TMP_CUR);
				PluTotalDropTbl(g_hdDB,(SHORT)PLUTOTAL_ID_TMP_CUR_S);
			#else
				PluTotalDelFile(PLUTOTAL_ID_DAILY_CUR);
				PluTotalDelFile(PLUTOTAL_ID_DAILY_SAV);
				PluTotalDelFile(PLUTOTAL_ID_DAILY_CUR_S);
				PluTotalDelFile(PLUTOTAL_ID_DAILY_SAV_S);

				PluTotalDelFile(PLUTOTAL_ID_PTD_CUR);
				PluTotalDelFile(PLUTOTAL_ID_PTD_SAV);
				PluTotalDelFile(PLUTOTAL_ID_PTD_CUR_S);
				PluTotalDelFile(PLUTOTAL_ID_PTD_SAV_S);

				PluTotalDelFile(PLUTOTAL_ID_TMP_CUR);
				PluTotalDelFile(PLUTOTAL_ID_TMP_CUR_S);
			#endif

			PluTotalClose(g_hdDB);

			pclStatusLabel->SetWindowText(L"Completed Clear Request");
			nRet = RESET_SUCCESS;
			break;

		case INIRST_TOTALS_REST:
			pclStatusLabel->SetWindowText(L"Begin Clear Financial Totals");
			Sleep (dwDelay);    // slight delay to give humans time to see the status change.

			/* ---- Clear Saratoga Database of Total files ---- */
			pclStatusLabel->SetWindowText(L"Clearing Financial Totals");
			Sleep (dwDelay);    // slight delay to give humans time to see the status change.
			OpenDir(s_DataBase);
			DelFiles(INIRST_FILES_TOTALCLEAR1);
			DelFiles(INIRST_FILES_TOTALCLEAR2);
			CloseDir();

			// do not clear the SavedTotal folder in case that data is still wanted.

			// do not clear the PLU totals stored in the SQL Server databases

			pclStatusLabel->SetWindowText(L"Completed Clear Request");
			nRet = RESET_SUCCESS;
			break;

		case INIRST_CLEAR_PLU:
			pclStatusLabel->SetWindowText(L"Begin Clear PLU Totals");
			Sleep (dwDelay);    // slight delay to give humans time to see the status change.

			/*==================================================================
				This clear of the PLU total database clears all of the
				PLU total Databases period to date, daily, , status and temp
				databases. ESMITH
			===================================================================*/

			/* ---- Clear PLU DB ---- */
			pclStatusLabel->SetWindowText(L"Clearing PLU totals");
			g_hdDB = PLUTOTAL_HANDLE_NULL;
			PluTotalOpen(&g_hdDB,TRUE,FALSE);

			#if !defined(_WIN32_WCE) || defined(_WIN32_WCE_EMULATION)
				pclStatusLabel->SetWindowText(L"Clearing Daily PLU totals");
				PluTotalDropTbl(g_hdDB,(SHORT)PLUTOTAL_ID_DAILY_CUR);
				PluTotalDropTbl(g_hdDB,(SHORT)PLUTOTAL_ID_DAILY_SAV);
				PluTotalDropTbl(g_hdDB,(SHORT)PLUTOTAL_ID_DAILY_CUR_S);
				PluTotalDropTbl(g_hdDB,(SHORT)PLUTOTAL_ID_DAILY_SAV_S);

				pclStatusLabel->SetWindowText(L"Clearing PTD PLU totals");
				PluTotalDropTbl(g_hdDB,(SHORT)PLUTOTAL_ID_PTD_CUR);
				PluTotalDropTbl(g_hdDB,(SHORT)PLUTOTAL_ID_PTD_SAV);
				PluTotalDropTbl(g_hdDB,(SHORT)PLUTOTAL_ID_PTD_CUR_S);
				PluTotalDropTbl(g_hdDB,(SHORT)PLUTOTAL_ID_PTD_SAV_S);

				PluTotalDropTbl(g_hdDB,(SHORT)PLUTOTAL_ID_TMP_CUR);
				PluTotalDropTbl(g_hdDB,(SHORT)PLUTOTAL_ID_TMP_CUR_S);
			#else
				PluTotalDelFile(PLUTOTAL_ID_DAILY_CUR);
				PluTotalDelFile(PLUTOTAL_ID_DAILY_SAV);
				PluTotalDelFile(PLUTOTAL_ID_DAILY_CUR_S);
				PluTotalDelFile(PLUTOTAL_ID_DAILY_SAV_S);

				PluTotalDelFile(PLUTOTAL_ID_PTD_CUR);
				PluTotalDelFile(PLUTOTAL_ID_PTD_SAV);
				PluTotalDelFile(PLUTOTAL_ID_PTD_CUR_S);
				PluTotalDelFile(PLUTOTAL_ID_PTD_SAV_S);

				PluTotalDelFile(PLUTOTAL_ID_TMP_CUR);
				PluTotalDelFile(PLUTOTAL_ID_TMP_CUR_S);
			#endif

			PluTotalClose(g_hdDB);

			pclStatusLabel->SetWindowText(L"Completed Clear Request");
			nRet = RESET_SUCCESS;
			break;

		case INIRST_CLEAR_GCF:
			pclStatusLabel->SetWindowText(L"Begin Clear Temp folder and Guest Check file");
			Sleep (dwDelay);    // slight delay to give humans time to see the status change.

			/* 
				Clear only the temp folder and the Guest Check file.
				We allow this since we do make Guest Check file size changes
				from time to time so we provide a mechanism for just a GCF
				delete only.
			 */
			pclStatusLabel->SetWindowText(L"Clearing Temp Folder");
			Sleep (dwDelay);    // slight delay to give humans time to see the status change.
			OpenDir(s_TempBase);
			DelFiles();
			CloseDir();

			/* ---- Clear Saratoga Database of TOTALGCF ---- */
			pclStatusLabel->SetWindowText(L"Clearing Guest Check File");
			Sleep (dwDelay);    // slight delay to give humans time to see the status change.
			OpenDir(s_DataBase);
			DelFiles(INIRST_FILES_GCF_FILES);
			CloseDir();

			pclStatusLabel->SetWindowText(L"Completed Clear Request");
			nRet = RESET_SUCCESS;
			break;

		case INIRST_CLEAR_TTLUPD:
			pclStatusLabel->SetWindowText(L"Begin Clear Total Update (TOTALUPD)");
			Sleep (dwDelay);    // slight delay to give humans time to see the status change.

			/* 
				Clear only the temp folder and the Total Update file.
				We allow this since we do make Total Update file size changes
				from time to time so we provide a mechanism for just a TOTALUPD
				delete only.
			 */
			pclStatusLabel->SetWindowText(L"Clearing Temp Folder");
			Sleep (dwDelay);    // slight delay to give humans time to see the status change.
			OpenDir(s_TempBase);
			DelFiles();
			CloseDir();

			pclStatusLabel->SetWindowText(L"Clearing Total Update file (TOTALUPD)");
			Sleep (dwDelay);    // slight delay to give humans time to see the status change.
			OpenDir(s_DataBase);
			DelFiles(INIRST_FILES_TTLUPD_FILE);
			CloseDir();

			pclStatusLabel->SetWindowText(L"Completed Clear Request");
			nRet = RESET_SUCCESS;
			break;

		case INIRST_CLEAR_EEPTSTO:
			pclStatusLabel->SetWindowText(L"Begin Overwrite Store and Forward");
			Sleep (dwDelay);    // slight delay to give humans time to see the status change.

			/*----- Overwrite (PCI-DSS compliance) and Clear Store and Forward File EEPTSTORE -----*/
			OpenDir(s_DataBase);
			OverWriteDelFiles(INIRST_FILES_EEPTSTORE_FILE);
			CloseDir();

			pclStatusLabel->SetWindowText(L"Completed Clear Request");
			nRet = RESET_SUCCESS;
			break;

		default:
			nRet = RESET_ERR;
			break;
	}

	return nRet;
}


void CDialogInitialReset::ON_IDC_INITRESET_EXIT()
{
	CDialog::OnCancel();
}
