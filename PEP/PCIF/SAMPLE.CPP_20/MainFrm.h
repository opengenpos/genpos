/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  MainFrm.h
//
//  PURPOSE:    Main Frame Window class definitions and declarations.
//
//  AUTHOR:
//
//      Ver 1.00.00 : 1998-09-24 : Initial Release
//
////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//            D E F I N I T I O N s    A N D    I N C L U D E s
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)
#define AFX_MAINFRM_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
//
//                  C L A S S    D E F I N I T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//  CMainFrame
//---------------------------------------------------------------------------

class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	static const int iTransferBufferSize = 1024;

// Operations
public:
    BOOL    IsLoggedOn() const;
    BOOL    IsModemConnected() const;
    BOOL    IsKeyboardLocked() const;
    USHORT  GetLoggedOnTerminalNo() const;
    BOOL    GetNoOfTerminals( UCHAR& uchNoOfTerminals );
    BOOL    LogOn( const USHORT usTerminalNo );
    BOOL    LogOnAndLockTerminal( const USHORT usTerminalNo );
    BOOL    ReadLockedEJ( CReportEJ& reportEJ );
    BOOL    PrintOnJournal( const CString& strMnemonic );
    BOOL    PrintOnJournal( const UCHAR uchActionCode );
    BOOL    InsertEjNote( const CString& strMnemonic );

	BOOL    IsLanConnected() const;	// ###ADD Saratoga

	SHORT   RetrieveRemoteFile (CFile &myFile, const SHORT sRemoteFile);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnConnectLogon();
	afx_msg void OnUpdateConnectLogon(CCmdUI* pCmdUI);
	afx_msg void OnConnectLogoff();
	afx_msg void OnUpdateConnectLogoff(CCmdUI* pCmdUI);
	afx_msg void OnConnectLockKbd();
	afx_msg void OnUpdateConnectLockKbd(CCmdUI* pCmdUI);
	afx_msg void OnConnectUnlockKbd();
	afx_msg void OnUpdateConnectUnlockKbd(CCmdUI* pCmdUI);
	afx_msg void OnConnectChgPassword();
	afx_msg void OnUpdateConnectChgPassword(CCmdUI* pCmdUI);
	afx_msg void OnParaConfig();
	afx_msg void OnParaSetDate();
	afx_msg void OnParaSetRegno();
	afx_msg void OnReadCpmTally();
	afx_msg void OnReadEJ();
	afx_msg void OnResetEJ();
	afx_msg void OnResetEJOverride();
	afx_msg void OnResetEJNonOverride();
	afx_msg void OnResetCpmTally();
	afx_msg void OnResetEOD();
	afx_msg void OnConnectModemOn();
	afx_msg void OnUpdateConnectModemOn(CCmdUI* pCmdUI);
	afx_msg void OnConnectModemOff();
	afx_msg void OnUpdateConnectModemOff(CCmdUI* pCmdUI);
	afx_msg void OnConectLanOff();
	afx_msg void OnUpdateConectLanOff(CCmdUI* pCmdUI);
	afx_msg void OnConnectLanOn();
	afx_msg void OnUpdateConnectLanOn(CCmdUI* pCmdUI);
	afx_msg void OnRcvDebgLog();
	afx_msg void OnRcvAssrtLog();
	afx_msg void OnDelAssrtLog();
	afx_msg void OnRcvLayout();
	afx_msg void OnRcvLgoFile();
	afx_msg void OnRcvLogFile();
	afx_msg void OnEodDb();
	afx_msg void OnRecieveSavTtl();
	afx_msg void OnReadSavTotal();
	afx_msg void OnSendOpMessage();
	afx_msg void OnInsertEjNote();
	afx_msg void OnUpdateMenuElement(CCmdUI* pCmdUI);
	afx_msg void OnSelectFromActivityList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    CPCSampleDoc*   GetMyDocument();
    VOID            GetCurrentDateTime( DATE& thisDate );
    BOOL            ReadAndResetEJ( class CReportEJ& reportEJ,
                                    const BOOL fOverride );

private:
    class CPcIfPort   m_PcIfPort;
    class CTerminal   m_Terminal;
    class CSerialInfo m_Serial;
    CString m_strModemCommand;
    CString m_strPhoneNumber;
    int m_nPhoneType;
};

/////////////////////////////////////////////////////////////////////////////
//
//                  I N L I N E    F U N C T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//
//  MEMBER VARIABLE ACCESS FUNCTIONs
//
//---------------------------------------------------------------------------

//===========================================================================
//  FUNCTION :  CMainFrame::GetMyDocument()
//===========================================================================

inline CPCSampleDoc* CMainFrame::GetMyDocument()
{
    CDocument* pDoc;
    pDoc = GetActiveDocument();
    return ( dynamic_cast< CPCSampleDoc* >( pDoc ) );
}

//===========================================================================
//  FUNCTION :  CMainFrame::IsLoggedOn()
//===========================================================================

inline BOOL CMainFrame::IsLoggedOn() const
{
    return ( m_Terminal.IsLoggedOn());
}

//===========================================================================
//  FUNCTION :  CMainFrame::IsModemConnected()
//===========================================================================

inline BOOL CMainFrame::IsModemConnected() const
{
    return ( m_PcIfPort.IsModemConnected());
}


//===========================================================================
//  FUNCTION :  CMainFrame::IsLanConnected(), // ###ADD Saratoga
//===========================================================================

inline BOOL CMainFrame::IsLanConnected() const
{
    return ( m_PcIfPort.IsLanConnected());
}

//===========================================================================
//  FUNCTION :  CMainFrame::IsKeyboardLocked()
//===========================================================================

inline BOOL CMainFrame::IsKeyboardLocked() const
{
    return ( m_Terminal.IsKeyboardLocked());
}

//===========================================================================
//  FUNCTION :  CMainFrame::GetLoggedOnTerminalNo()
//===========================================================================

inline USHORT CMainFrame::GetLoggedOnTerminalNo() const
{
    return ( m_Terminal.GetLoggedOnTerminalNo());
}

//===========================================================================
//  FUNCTION :  CMainFrame::GetNoOfTerminals()
//===========================================================================

inline BOOL CMainFrame::GetNoOfTerminals( UCHAR& uchNoOfTerminals )
{
    return ( m_Terminal.GetNoOfTerminals( uchNoOfTerminals ));
}

//===========================================================================
//  FUNCTION :  CMainFrame::LogOn()
//===========================================================================

inline BOOL CMainFrame::LogOn( const USHORT usTerminalNo )
{
    return ( m_Terminal.LogOn( usTerminalNo ));

}

//===========================================================================
//  FUNCTION :  CMainFrame::PrintOnJournal()
//===========================================================================

inline BOOL CMainFrame::PrintOnJournal( const CString& strMnemonic )
{
    return ( m_Terminal.PrintOnJournal( strMnemonic ));
}

//===========================================================================
//  FUNCTION :  CMainFrame::PrintOnJournal()
//===========================================================================

inline BOOL CMainFrame::PrintOnJournal( const UCHAR uchActionCode )
{
    return ( m_Terminal.PrintOnJournal( uchActionCode ));
}

inline BOOL CMainFrame::InsertEjNote( const CString& strMnemonic )
{
    return ( m_Terminal.InsertEjNote( strMnemonic ));
}
//===========================================================================
//  FUNCTION :  CMainFrame::GetCurrentDateTime()
//===========================================================================

inline VOID CMainFrame::GetCurrentDateTime( DATE& thisDate )
{
    time_t  timeCurrent;
    struct tm*  ptmTime;

    time( &timeCurrent );
    ptmTime = localtime( &timeCurrent );

    thisDate.usMDay  = static_cast< USHORT >( ptmTime->tm_mday    );
    thisDate.usMonth = static_cast< USHORT >( ptmTime->tm_mon + 1 );
    thisDate.usHour  = static_cast< USHORT >( ptmTime->tm_hour    );
    thisDate.usMin   = static_cast< USHORT >( ptmTime->tm_min     );
}

//---------------------------------------------------------------------------
//
//  MENU STATE UPDATE FUNCTIONs
//
//---------------------------------------------------------------------------

//===========================================================================
//  FUNCTION :  CMainFrame::OnUpdateConnectLogon()
//===========================================================================

inline void CMainFrame::OnUpdateConnectLogon(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable( ! IsLoggedOn());
}

//===========================================================================
//  FUNCTION :  CMainFrame::OnUpdateConnectLogoff()
//===========================================================================

inline void CMainFrame::OnUpdateConnectLogoff(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable( IsLoggedOn() && ( ! IsModemConnected()));
}

//===========================================================================
//  FUNCTION :  CMainFrame::OnUpdateConnectModemOn()
//===========================================================================

inline void CMainFrame::OnUpdateConnectModemOn(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable( ! IsLoggedOn());
}

//===========================================================================
//  FUNCTION :  CMainFrame::OnUpdateConnectModemOff()
//===========================================================================

inline void CMainFrame::OnUpdateConnectModemOff(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable( IsLoggedOn() && ( IsModemConnected()));
}

//===========================================================================
//  FUNCTION :  CMainFrame::OnUpdateConnectLanOn(), // ###ADD Saratoga
//===========================================================================

inline void CMainFrame::OnUpdateConnectLanOn(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
    pCmdUI->Enable( ! IsLoggedOn());
	
}

//===========================================================================
//  FUNCTION :  CMainFrame::OnUpdateConnectLanOff(), // ###ADD Saratoga
//===========================================================================

inline void CMainFrame::OnUpdateConectLanOff(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
    pCmdUI->Enable( IsLoggedOn() && ( IsLanConnected()));
}

//===========================================================================
//  FUNCTION :  CMainFrame::OnUpdateConnectLockKbd()
//===========================================================================

inline void CMainFrame::OnUpdateConnectLockKbd(CCmdUI* pCmdUI) 
{
    if ( IsLoggedOn())
    {
        pCmdUI->Enable( ! IsKeyboardLocked());
    }
    else
    {
        pCmdUI->Enable( FALSE );
    }
}

//===========================================================================
//  FUNCTION :  CMainFrame::OnUpdateConnectLockKbd()
//===========================================================================

inline void CMainFrame::OnUpdateConnectUnlockKbd(CCmdUI* pCmdUI) 
{
    if ( m_Terminal.IsLoggedOn())
    {
        pCmdUI->Enable( IsKeyboardLocked());
    }
    else
    {
        pCmdUI->Enable( FALSE );
    }
}



//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)

////////////////// END OF FILE ( MainFrm.h ) ////////////////////////////////
