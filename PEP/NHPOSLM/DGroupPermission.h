#pragma once
#include "afxwin.h"
#include "Resource.h"


// DGroupPermission dialog

class DGroupPermission : public CDialog
{
	DECLARE_DYNAMIC(DGroupPermission)

public:
	DGroupPermission(ULONG groupPermissionMask, CWnd* pParent = NULL);   // standard constructor
	virtual ~DGroupPermission();

// Dialog Data
	enum { IDD = IDD_DLG_GROUP_PERMISSIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	BOOL CButtonGroupAllowed[32];
	ULONG groupPermissionMask;
	
public:
	afx_msg void OnBnClickedCheckAll();
	afx_msg void OnBnClickedCheckNone();
	afx_msg void OnBnClickedCheckInvert();
	afx_msg void OnBnClickedOk();

	ULONG GetGroupPermissionMask();

private:
	void ConvertToMask(){
		groupPermissionMask = 0;
		for(int i = 0; i < 32; i++)
			if(CButtonGroupAllowed[i])
				groupPermissionMask |= 0x1 << i;
	}
	void InterpretMask(){
		for(int i = 0; i < 32; i++)
			CButtonGroupAllowed[i] = (groupPermissionMask & (0x00000001 << i)) ? TRUE : FALSE;
	}
};
