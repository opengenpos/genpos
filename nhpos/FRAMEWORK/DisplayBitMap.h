#pragma once
#include "afxwin.h"


// CDisplayBitMap dialog

class CDisplayBitMap : public CDialog
{
	DECLARE_DYNAMIC(CDisplayBitMap)

public:
	CDisplayBitMap(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDisplayBitMap();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

	CBitmap  myBitMap;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_BitmapDisplay2;
};
