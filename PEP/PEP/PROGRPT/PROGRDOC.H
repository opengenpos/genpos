/////////////////////////////////////////////////////////////////////////////
// progrdoc.h : interface of the CProgrptDoc class
//
// Copyright (C) 1996-1998 NCR Corporation, All rights reserved.
//
//
// History:
//
// Date       Version  Author       Comments
// =========  =======  ===========  ========================================
// Mar-01-96           O.Nakada     initial
// Oct-12-98           O.Nakada     HPUS V3.3
//
/////////////////////////////////////////////////////////////////////////////

class CReport;
class CReportView;
class CListView;
class CFieldDlg;

#define	REPORT_MAX_SHEET		8
#define	MNEMO_MAX_16CM			20
#define	MNEMO_MAX_40CM			10
#define	MNEMO_INDEX_TRAN		0
#define	MNEMO_INDEX_REPORT		1
#define	MNEMO_INDEX_SPECIAL		2
#define	MNEMO_INDEX_ADJ			3
#define	MNEMO_INDEX_MDEPT		4
#define	MNEMO_INDEX_16CM		5
#define	MNEMO_INDEX_40CM		6
#define	DOC_NUM_EP				2
#define	DOC_INDEX_EOD			0
#define	DOC_INDEX_PTD			1
#define	DOC_EP_NOT				(-1)
#define	DOC_EP_STD				0
#define	DOC_SIZE_FIXED			(64L + 56L + 56L + 336L + 408L)
#define	DOC_SIZE_ITEM			(19L)
#define	DOC_SIZE_REPORT			(52L + DOC_SIZE_ITEM)
#define	DOC_SIZE_FIELDS			(DOC_SIZE_ITEM + DOC_SIZE_ITEM)
#if	1										// HPUS V3.3 - Migration
#define	MIGRATE_ID_OPERATOR		4
#define	MIGRATE_ID_SERVER		5
#endif										// HPUS V3.3 - Migration

typedef struct _EOD_PTD
{
	short	m_nFile;
	short	m_nUnique;
} EOD_PTD;

class CProgrptDoc : public CDocument
{
protected: // create from serialization only
	CProgrptDoc();
	DECLARE_DYNCREATE(CProgrptDoc)

// Attributes
public:
	BOOL		m_bDate;				// state of date, from PEP parameters
	BOOL		m_bTime;				// state of time, from PEP parameters
	CStringList	m_strMnemo[7];			// list of mnemonics
	CPtrList	m_EPList[DOC_NUM_EP];	// list of EOD/PTD
	int			m_nEPState[DOC_NUM_EP];	// state of EOD/PTD
	int			m_anFile[REPORT_MAX_SHEET];
protected:
	int			m_nUnique;				// unique report number
	LONG		m_lAllocSize;			// allocation size, from PEP parameters
	BOOL		m_bMessage;				// state of warning message
	CListView*	m_pListView;			// pointer of list view
	CReport*	m_pReport;				// pre-create frame
	CObList		m_ReportList;			// list of report

// Operations
public:
	BOOL		ReadPepParameters();
	void		ReadMnemonics(int nIndex, int nNumber, const wchar_t * pszSection,
					const wchar_t  *pszFile, BOOL bContinue = TRUE);
	void		WriteMnemonics(int nIndex, int nNumber, const wchar_t * pszSection, const wchar_t *pszFile);
	void		NewEodPtd(int nIndex, int* pnInfo);
	void		DeleteEodPtd(int nIndex);
	void		ReadEodPtd(int nIndex, const wchar_t * pszSection, const wchar_t *pszFile);
	void		WriteEodPtd(int nIndex, const wchar_t* pszSection, const wchar_t *pszFile);

	void		UpdateStatusMsg(UINT nID);
	void		UpdateStatusSize();
	int			CheckSize(long lAdd, BOOL bWarning = TRUE);
	long		GetUsedSize();
	int			GetUnique() { return m_nUnique++; }
	long		GetAllocSize() { return m_lAllocSize; }
	CListView*	GetListView() { return m_pListView; }
	void		SetListView(CListView* pView) { m_pListView = pView; }

	int			CreateNewReportAndFrame();
	void		SetNewReport(CReport* pReport);
	CReport*	GetNewReport();
	BOOL		IsNewReport(int nFile, int nDevice);
	void		AddReport(CReport* pReport, BOOL bModified = TRUE);
	BOOL		RemoveReport(CReport* pReport, BOOL bModified = TRUE);
	void		RemoveAllReports();
	int			GetCountReport(BOOL bLcd);
	CReport*	GetReport(int nNumber);
	int			GetReportNo(int nUnique);
	CReport*	FindActiveReport(CReportView* pView);

	POSITION	GetHeadPositionReport()
					{ return m_ReportList.GetHeadPosition(); }
	CReport*	GetNextReport(POSITION& pos)
					{ return (CReport*)m_ReportList.GetNext(pos); }
	CReport*	GetAtReport(POSITION pos)
					{ return (CReport*)m_ReportList.GetAt(pos); }
	void		RemoveAtReport(POSITION pos)
					{ m_ReportList.RemoveAt(pos); }
	POSITION	FindIndexReport(int nIndex)
					{ return m_ReportList.FindIndex(nIndex); }
	POSITION	InsertBeforeReport(POSITION pos, CObject* pReport)
					{ return m_ReportList.InsertBefore(pos, pReport); }
	POSITION	InsertAfterReport(POSITION pos, CObject* pReport)
					{ return m_ReportList.InsertAfter(pos, pReport); }
protected:

// Implementation
public:
	virtual ~CProgrptDoc();
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual void SetPathName(const wchar_t * pszPathName, BOOL bAddtoMRU = TRUE);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(const wchar_t * pszPathName);
	virtual BOOL OnSaveDocument(const wchar_t * pszPathName);
	virtual BOOL CanCloseFrame(CFrameWnd* pFrameArg);
	virtual void DeleteContents();

// Generated message map functions
protected:
	//{{AFX_MSG(CProgrptDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
