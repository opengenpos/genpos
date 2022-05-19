// ShortCut.h: interface for the CShortCut class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHORTCUT_H__6951FDBE_EFA3_44FC_8FE5_B8E50BEC9437__INCLUDED_)
#define AFX_SHORTCUT_H__6951FDBE_EFA3_44FC_8FE5_B8E50BEC9437__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CShortCut  
{
public:
	CShortCut();
	virtual ~CShortCut();

public:
	BOOL isShortCut();
	BOOL CreateShortCut();
	BOOL DeleteShortCut();

	BOOL CreateInStartMenu();
	BOOL CreateOnDesktop();
	BOOL CreateInStartup();

	BOOL DeleteFromStartMenu();
	BOOL DeleteFromDesktop();
	BOOL DeleteFromStartup();

public:
	void setLinkPath(LPCTSTR csLPath) {csLinkPath = csLPath; }
	void setLinkFileName(LPCTSTR csLFN) {csLinkFileName = csLFN; }
	void setLinkDescription(LPCTSTR csDesc) { csLinkDescription = csDesc; }
	void setLinkTarget(LPCTSTR csTarget) { csLinkTarget = csTarget; }
	void setWorkingDirectory(LPCTSTR csWorkDir) {csWorkingDirectory = csWorkDir; }

	void setLinkPathStartup();
	void setLinkPathDesktop();
	void setLinkPathStartMenu();

private:
	VOID MakeFullPathName(CString &csFullPathName);
	BOOL CreateInSpecialFolder(UINT nFolder);
	BOOL DeleteInSpecialFolder(UINT nFolder);
	BOOL GetSpecialFolderPath(UINT nSpecialFolder, CString &csFolderPath);

private:
	CString	csLinkPath;				// Path to where the link will be placed
	CString csLinkFileName;			// Name of the link file
	CString csLinkDescription;		// Optional Description for the link
	CString csLinkTarget;			// The executable file that the link is targeting
	CString csWorkingDirectory;		// Optional Directory where link target is located
};

#endif // !defined(AFX_SHORTCUT_H__6951FDBE_EFA3_44FC_8FE5_B8E50BEC9437__INCLUDED_)
