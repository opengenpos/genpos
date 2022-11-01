// DisplayTransaction.cpp : implementation file
//

#include "stdafx.h"
#include "SampleEPT.h"
#include "DisplayTransaction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDisplayTransaction dialog


CDisplayTransaction::CDisplayTransaction(CWnd* pParent /*=NULL*/)
	: CDialog(CDisplayTransaction::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDisplayTransaction)
	m_csAmount = _T("");
	m_csFuncType = _T("");
	m_csSequenceNo = _T("");
	m_csDestTerminal = _T("");
	m_csSourceTerminal = _T("");
	//}}AFX_DATA_INIT
}


void CDisplayTransaction::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDisplayTransaction)
	DDX_Control(pDX, IDC_COMBO_RETURN_STAT, m_cbReturnStatus);
	DDX_Text(pDX, IDC_REQ_EDIT_AMMOUNT, m_csAmount);
	DDV_MaxChars(pDX, m_csAmount, 8);
	DDX_Text(pDX, IDC_REQ_EDIT_FUNC_TYPE, m_csFuncType);
	DDX_Text(pDX, IDC_REQ_EDIT_SEQ_NO, m_csSequenceNo);
	DDX_Text(pDX, IDC_REQ_EDIT_TERM_DEST, m_csDestTerminal);
	DDX_Text(pDX, IDC_REQ_EDIT_TERM_SOURCE, m_csSourceTerminal);
	DDV_MaxChars(pDX, m_csSourceTerminal, 2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDisplayTransaction, CDialog)
	//{{AFX_MSG_MAP(CDisplayTransaction)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDisplayTransaction message handlers


CDisplayTransaction::SetDialogData (CPMRCRSPDATA *pData)
{
	UCHAR  ucTempBuf[100];


	memset (ucTempBuf, 0, sizeof(ucTempBuf));
	memcpy (ucTempBuf, pData->auchSystemFunc, sizeof (pData->auchSystemFunc));
	m_csFuncType = ucTempBuf;

	memset (ucTempBuf, 0, sizeof(ucTempBuf));
	memcpy (ucTempBuf, pData->auchDataSeqNo, sizeof (pData->auchDataSeqNo));
	m_csSequenceNo = ucTempBuf;

	memset (ucTempBuf, 0, sizeof(ucTempBuf));
	memcpy (ucTempBuf, pData->auchAcceptAmt, sizeof (pData->auchAcceptAmt));
	m_csAmount = ucTempBuf;

	memset (ucTempBuf, 0, sizeof(ucTempBuf));
	memcpy (ucTempBuf, pData->auchSrcTerminal, sizeof (pData->auchSrcTerminal));
	m_csSourceTerminal = ucTempBuf;

	memset (ucTempBuf, 0, sizeof(ucTempBuf));
	memcpy (ucTempBuf, pData->auchDesTerminal, sizeof (pData->auchDesTerminal));
	m_csDestTerminal = ucTempBuf;

	memset (ucTempBuf, 0, sizeof(ucTempBuf));
	memcpy (ucTempBuf, pData->auchTerminalNo, sizeof (pData->auchTerminalNo));


}
