/*
-----------------------------------------------------------------------------------------------------------------------
 NHPOSLM
 Copyright (C) 2011 GSU
-----------------------------------------------------------------------------------------------------------------------
 Date	|	Changes Made																	| Version	|	By
-----------------------------------------------------------------------------------------------------------------------
 111108		New.  Replace the old DP03ToolBox.																KSo@NCR
-----------------------------------------------------------------------------------------------------------------------
*/
#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "WindowButton.h"

// DButtonAction dialog

typedef struct 
{
	CWindowButton::ButtonActionType ActionTypeId;	// Id
	int TextId;										// Text Id (also used as SubTypeId) from old implementation
	LPCTSTR	Abbreviation;							// Short name
	LPCTSTR	Group;									// Group name
	LPCTSTR	Description;							// Full description
} SActionItem, *PSActionItem;

class DButtonAction : public CDialog
{
	DECLARE_DYNAMIC(DButtonAction)

public:
	DButtonAction(CWnd* pParent = NULL);   // standard constructor
	virtual ~DButtonAction();
	BOOL OnInitDialog();

	enum { IDD = IDD_P03_BUTTON_ACTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

public:
	CListCtrl m_cActionList;
	CStatic m_cCurrentActionName;
	CWindowButton::ButtonActionType m_currentActionId;
	int m_currentTextId;

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnHdnItemclickListAction(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
public:
	afx_msg void OnNMDblclkListAction(NMHDR *pNMHDR, LRESULT *pResult);
};
