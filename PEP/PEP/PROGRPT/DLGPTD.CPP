/////////////////////////////////////////////////////////////////////////////
// dlgptd.cpp : implementation file
//
// Copyright (C) 1996 NCR Corporation, All rights reserved.
//
//
// History:
//
// Date       Version  Author       Comments
// =========  =======  ===========  ========================================
// Mar-01-96           O.Nakada     initial
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "progrpt.h"

#include "progrdoc.h"
#include "report.h"
#include "dlgptd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPtdDlg dialog


CPtdDlg::CPtdDlg(CProgrptDoc* pDoc, CWnd* pParent /*=NULL*/)
    : CDialog(CPtdDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CPtdDlg)
    m_nStd = -1;
    //}}AFX_DATA_INIT

    m_pDoc = pDoc;
}

void CPtdDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CPtdDlg)
    DDX_Control(pDX, IDC_PTD_TITLE, m_Title);
    DDX_Control(pDX, IDC_PTD_STATE, m_State);
    DDX_Control(pDX, IDC_PTD_NO, m_No);
    DDX_Control(pDX, IDC_PTD_FILE, m_File);
    DDX_Control(pDX, IDC_PTD_DEVICE, m_Device);
    DDX_Control(pDX, IDC_PTD_UP, m_btnUp);
    DDX_Control(pDX, IDC_PTD_OBJECT, m_LbObject);
    DDX_Control(pDX, IDC_PTD_LIST, m_LbList);
    DDX_Control(pDX, IDC_PTD_DOWN, m_btnDown);
    DDX_Radio(pDX, IDC_PTD_STD, m_nStd);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPtdDlg, CDialog)
    //{{AFX_MSG_MAP(CPtdDlg)
    ON_BN_CLICKED(IDC_PTD_STD, OnPtdStd)
    ON_LBN_SELCHANGE(IDC_PTD_LIST, OnSelchangePtdList)
    ON_LBN_SELCHANGE(IDC_PTD_OBJECT, OnSelchangePtdObject)
    ON_BN_CLICKED(IDC_PTD_UP, OnPtdUp)
    ON_BN_CLICKED(IDC_PTD_DOWN, OnPtdDown)
    ON_BN_CLICKED(IDC_PTD_ORG, OnPtdOrg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPtdDlg implementations

static wchar_t szPtdNot[] = L"\tDo not execute";
static wchar_t szPtdStd[] = L"\tStandard Report";

void CPtdDlg::RefreshList()
{
    m_LbList.ResetContent();

    POSITION pos = m_List.GetHeadPosition();
    while (pos != NULL)
    {
        EOD_PTD* pData = (EOD_PTD*)m_List.GetNext(pos);

        wchar_t  szMsg[32];
        CString strMsg(ptheInfo->GetFile(pData->m_nFile)->m_strName);
        switch (pData->m_nUnique)
        {
        case DOC_EP_NOT:                // Do not execute
            strMsg += szPtdNot;
            break;
        case DOC_EP_STD:                // Standard Report
            strMsg += szPtdStd;
            break;
        default:                        // Programmable Report
            wsprintf(szMsg, L"\tExecute Report#%d", m_pDoc->GetReportNo(pData->m_nUnique));
            strMsg += szMsg;
            break;
        }
        m_LbList.InsertString(-1, strMsg);
    }
}

void CPtdDlg::RefreshObject()
{
    m_LbObject.ResetContent();

    int nIndex = m_LbList.GetCurSel();
    if (nIndex == LB_ERR)
        return;

    POSITION pos = m_List.FindIndex(nIndex);
    ASSERT(pos != NULL);
    EOD_PTD* pData = (EOD_PTD*)m_List.GetAt(pos);
    ASSERT(pData != NULL);

    CString strMsg(L"\t");
    m_LbObject.InsertString(-1, strMsg + szPtdNot);
    m_LbObject.InsertString(-1, strMsg + szPtdStd);

    // insert report item
    nIndex = 0;
    int nNo = 1, nSelect = LB_ERR;
    int nMax = m_pDoc->GetCountReport(FALSE);   // exclude LCD
    pos = m_pDoc->GetHeadPositionReport();
    while (pos != NULL && nNo <= nMax)
    {
        CReport* pReport = m_pDoc->GetNextReport(pos);

        int nFileIndex = pReport->GetFileIndex();
#if 1                                       // HPUS V3.3 - Migration (TAR #127354)
        if (nFileIndex >= MIGRATE_ID_OPERATOR)
        {
            nFileIndex += 1;
        }
#endif                                      // HPUS V3.3 - Migration (TAR #127354)
        if (nFileIndex == pData->m_nFile - 1)
        {
            INFO_DEVICE* pDevice = ptheInfo->GetDevice(pReport->GetDevice());
            ASSERT(pDevice != NULL);
            m_anUnique[nIndex] = pReport->GetUnique();
            if (m_anUnique[nIndex] == pData->m_nUnique)
                nSelect = 2 + nIndex;           // bias 2 item
            nIndex++;
            wchar_t szData[64];
            wsprintf(szData, L"%d\t%s\t%s", nNo,
                     pDevice->m_strName,
                     pReport->GetExtendedTitle());
            m_LbObject.InsertString(-1, szData);
        }
        nNo++;
    }

    if (pData->m_nUnique == DOC_EP_NOT)
        m_LbObject.SetCurSel(0);
    else if (pData->m_nUnique == DOC_EP_STD)
        m_LbObject.SetCurSel(1);
    else
        m_LbObject.SetCurSel(nSelect);
}

void CPtdDlg::EnableControl(BOOL bState)
{
    m_File.EnableWindow(bState);
    m_State.EnableWindow(bState);
    m_LbList.EnableWindow(bState);
    m_No.EnableWindow(bState);
    m_Device.EnableWindow(bState);
    m_Title.EnableWindow(bState);
    m_LbObject.EnableWindow(bState);

    BOOL bUp = FALSE, bDown = FALSE;
    int nIndex = m_LbList.GetCurSel();
    if (nIndex != LB_ERR)
    {
        int nCount = m_LbList.GetCount();
//        if (nCount == 1)
//            ;
        if (nIndex == 0)
            bDown = TRUE;
        else if (nIndex == nCount - 1)
            bUp = TRUE;
        else
        {
            bUp = TRUE;
            bDown = TRUE;
        }
    }

    m_btnUp.EnableWindow(bUp);
    m_btnDown.EnableWindow(bDown);

    if (! bUp || ! bDown)
        ((CButton*)(GetDlgItem(IDOK)))->SetFocus();
}

/////////////////////////////////////////////////////////////////////////////
// CPtdDlg message handlers

BOOL CPtdDlg::OnInitDialog()
{
    m_nStd = m_pDoc->m_nEPState[DOC_INDEX_PTD];

    CDialog::OnInitDialog();
    
    // set tab stops in list box
    int nTabs[2] = {15, 80};
    m_LbList.SetTabStops(1, &nTabs[1]);
    m_LbObject.SetTabStops(2, nTabs);

    BOOL bDelete = FALSE;

    // copy data list
    POSITION pos = m_pDoc->m_EPList[DOC_INDEX_PTD].GetHeadPosition();
    while (pos != NULL)
    {
        EOD_PTD* pData = (EOD_PTD*)m_pDoc->m_EPList[DOC_INDEX_PTD].GetNext(pos);
        EOD_PTD* pTmp = new EOD_PTD;
        *pTmp = *pData;
        if (DOC_EP_STD < pTmp->m_nUnique &&
            m_pDoc->GetReportNo(pTmp->m_nUnique) == -1)
        {
            pTmp->m_nUnique = DOC_EP_NOT;
            bDelete = TRUE;
        }
        m_List.AddTail(pTmp);
    }

    if (bDelete)
        AfxMessageBox(IDS_ERRMSG_EODPTD);
    
    RefreshList();
    EnableControl((m_nStd == 0) ? (FALSE) : (TRUE));
    
    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPtdDlg::OnOK()
{
    ASSERT(m_pDoc->m_EPList[DOC_INDEX_PTD].GetCount() == m_List.GetCount());

    POSITION pos = m_pDoc->m_EPList[DOC_INDEX_PTD].GetHeadPosition();
    POSITION tmp = m_List.GetHeadPosition();
    while (pos != NULL)
    {
        EOD_PTD* pData = (EOD_PTD*)m_pDoc->m_EPList[DOC_INDEX_PTD].GetNext(pos);
        EOD_PTD* pTmp = (EOD_PTD*)m_List.GetNext(tmp);
        *pData = *pTmp;
        delete pTmp;
    }
    m_List.RemoveAll();
    
    CDialog::OnOK();

    m_pDoc->m_nEPState[DOC_INDEX_PTD] = m_nStd;
}

void CPtdDlg::OnCancel()
{
    POSITION tmp = m_List.GetHeadPosition();
    while (tmp != NULL)
    {
        EOD_PTD* pTmp = (EOD_PTD*)m_List.GetNext(tmp);
        delete pTmp;
    }
    m_List.RemoveAll();
    
    CDialog::OnCancel();
}

void CPtdDlg::OnPtdStd()
{
    if (m_nStd == 0)
        return;

    m_nStd = 0;
    RefreshList();
    EnableControl(FALSE);
    m_LbObject.ResetContent();
}

void CPtdDlg::OnPtdOrg()
{
    if (m_nStd != 0)
        return;

    m_nStd = 1;
    RefreshList();
    EnableControl(TRUE);
}

void CPtdDlg::OnSelchangePtdList()
{
    RefreshObject();
    EnableControl(TRUE);
}

void CPtdDlg::OnSelchangePtdObject()
{
    int nList = m_LbList.GetCurSel();
    if (nList == LB_ERR)
        return;
    POSITION pos = m_List.FindIndex(nList);
    EOD_PTD* pData = (EOD_PTD*)m_List.GetAt(pos);

    int nIndex = m_LbObject.GetCurSel();
    if (nIndex == LB_ERR)
        return;
    if (nIndex == 0)                    // Do not execute
        pData->m_nUnique = DOC_EP_NOT;
    else if (nIndex == 1)               // Standard Report
        pData->m_nUnique = DOC_EP_STD;
    else                                // Programmable Report
        pData->m_nUnique = m_anUnique[nIndex - 2];

    RefreshList();
    m_LbList.SetCurSel(nList);
}

void CPtdDlg::OnPtdUp()
{
    int nIndex = m_LbList.GetCurSel();
    if (nIndex == LB_ERR)
        return;
    ASSERT(nIndex > 0);
    POSITION pos = m_List.FindIndex(nIndex);
    EOD_PTD* pData = (EOD_PTD*)m_List.GetAt(pos);
    m_List.RemoveAt(pos);
    pos = m_List.FindIndex(nIndex - 1);
    m_List.InsertBefore(pos, pData);

    RefreshList();
    m_LbList.SetCurSel(nIndex - 1);
    EnableControl(TRUE);
}

void CPtdDlg::OnPtdDown()
{
    int nIndex = m_LbList.GetCurSel();
    if (nIndex == LB_ERR)
        return;
    ASSERT(nIndex + 1 < m_LbList.GetCount());
    POSITION pos = m_List.FindIndex(nIndex);
    EOD_PTD* pData = (EOD_PTD*)m_List.GetAt(pos);
    m_List.RemoveAt(pos);
    pos = m_List.FindIndex(nIndex);
    m_List.InsertAfter(pos, pData);

    RefreshList();
    m_LbList.SetCurSel(nIndex + 1);
    EnableControl(TRUE);
}
