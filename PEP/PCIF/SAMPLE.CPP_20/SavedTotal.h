#if !defined(AFX_SAVEDTOTAL_H__B1356ABB_7555_4285_8DCC_05D11B0A5414__INCLUDED_)
#define AFX_SAVEDTOTAL_H__B1356ABB_7555_4285_8DCC_05D11B0A5414__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SavedTotal.h : header file
//
#include "r20_pc2172.h"
#include "Total.h"
#include "TtlRegFin.h"
#include "TtlHourly.h"


/////////////////////////////////////////////////////////////////////////////
// CSavedTotal window

class CSavedTotal
{
// Construction
public:
	CSavedTotal(class CPCSampleDoc* const pDocument );

// Attributes
public:
	SAVTTLFILEINDEX m_myIndex;
	CString         m_mySaveTotalFileName;
	
// Operations
public:
	BOOL			Read( const UCHAR uchClassToRead );
	BOOL            Write(const UCHAR uchClassToRead);
	BOOL            Write(CTtlRegFin& ttlRegFin);
	BOOL            Write(CTtlIndFin& ttlRegFin);
	BOOL            Write(CTtlCashier & ttlCashier, int iIndex);
	BOOL            Write(DEPTTBL & ttlDept, int iIndex);
	BOOL            Write(TTLPLU & ttPlu);
	BOOL            Write(const CPNTBL  & ttlCoupon, USHORT usCouponNo);
	BOOL            Write(const TTLCSHOURLY &ttlHourly);
	BOOL            WriteIndex(void);
	BOOL			Open(LPCTSTR cstFileName);
	BOOL			Create(LPCTSTR cstFileName);
	BOOL			Report( const UCHAR uchClassToRead );
    CPCSampleDoc*   m_pDocument;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSavedTotal)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSavedTotal();

	// Generated message map functions
protected:
	CFile			m_mySaveTotalFile;
	TTLCSHOURLY		m_ttlHourly;
	TTLCSSERTIME	m_ttlService;
	TTLCASHIER		m_ttlCashier[STD_NUM_OF_CASHIERS];
	TTLCSREGFIN		m_ttlRegFinTerminal[STD_NUM_OF_TERMINALS];
	TTLCSREGFIN		m_ttlRegFinTotal;
	DEPTTBL			m_ttlDept[FLEX_DEPT_MAX];
	CPNTBL			m_ttlCpn[FLEX_CPN_MAX];
	CTtlPlu			m_ttlPLU;
	ULONG			m_ulMyPLUCounter;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAVEDTOTAL_H__B1356ABB_7555_4285_8DCC_05D11B0A5414__INCLUDED_)
