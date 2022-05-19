// ShortCut.cpp: implementation of the CShortCut class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeviceConfig.h"
#include "ShortCut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShortCut::CShortCut()
{
	csLinkPath = _T("");
	csLinkTarget = _T("");
	csLinkFileName = _T("");
	csLinkDescription = _T("");
	csWorkingDirectory = _T("");
}

CShortCut::~CShortCut()
{

}

BOOL CShortCut::CreateShortCut()
{    
	HRESULT hres;
	IShellLink* psl;
	IPersistFile* ppf;
	CString csFullPathName;
	BOOL	bReturn = FALSE;

	CoInitialize(NULL);

	hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, 
                            IID_IShellLink, (LPVOID*)&psl); 

	if (SUCCEEDED(hres)) { 

        // Set the path to the shortcut target and add the description. 
        psl->SetPath(csLinkTarget); 
        psl->SetDescription(csLinkDescription); 
		psl->SetWorkingDirectory(csWorkingDirectory);
 
        // Query IShellLink for the IPersistFile interface for saving the 
        // shortcut in persistent storage. 
        hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf); 
 
        if (SUCCEEDED(hres)) {
			
            TCHAR szPath[MAX_PATH]; 
 
			memset(szPath,'0',sizeof(szPath));
			MakeFullPathName(csFullPathName);
 
            // Save the link by calling IPersistFile::Save. 
			hres = ppf->Save(csFullPathName, TRUE); 
            ppf->Release(); 
        }
		
        psl->Release(); 
	}

	CoUninitialize();

	return bReturn;
}

BOOL CShortCut::CreateInStartMenu()
{
	return CreateInSpecialFolder(CSIDL_COMMON_STARTMENU);
}

BOOL CShortCut::CreateOnDesktop()
{
	return CreateInSpecialFolder(CSIDL_DESKTOP);
}

BOOL CShortCut::CreateInStartup()
{
	return CreateInSpecialFolder(CSIDL_COMMON_STARTUP);
}

BOOL CShortCut::CreateInSpecialFolder(UINT nFolder)
{
	if (GetSpecialFolderPath(nFolder,csLinkPath)) {
		return CreateShortCut();
	}

	return FALSE;
}

BOOL CShortCut::DeleteShortCut()
{
	SHFILEOPSTRUCT shFileIO;
	CString	csLinkPathName;

	memset(&shFileIO, 0, sizeof SHFILEOPSTRUCT);

	MakeFullPathName(csLinkPathName);

	shFileIO.wFunc = FO_DELETE;
	shFileIO.pFrom = csLinkPathName;
	shFileIO.fFlags = FOF_NOERRORUI|FOF_NOCONFIRMATION;

	if (!SHFileOperation(&shFileIO)) {
		return TRUE;
	}

	return FALSE;
}

BOOL CShortCut::DeleteFromStartMenu()
{
	return DeleteInSpecialFolder(CSIDL_COMMON_STARTMENU);
}

BOOL CShortCut::DeleteFromDesktop()
{
	return DeleteInSpecialFolder(CSIDL_DESKTOP);
}

BOOL CShortCut::DeleteFromStartup()
{
	return DeleteInSpecialFolder(CSIDL_COMMON_STARTUP);
}

BOOL CShortCut::DeleteInSpecialFolder(UINT nFolder)
{
	if (GetSpecialFolderPath(nFolder,csLinkPath)) {
		return DeleteShortCut();
	}

	return FALSE;
}

VOID CShortCut::MakeFullPathName(CString &csFullPathName)
{	
	
	if (csLinkFileName.Find(_T(".lnk")) == -1 )	{
		csLinkFileName += _T(".lnk");
	}

	csLinkPath.TrimRight(_T('\\'));

	csFullPathName = csLinkPath + _T('\\') + csLinkFileName + _T('\0');
}

BOOL CShortCut::GetSpecialFolderPath(UINT nSpecialFolder,CString &csFolderPath)
{
	LPITEMIDLIST pidlItem;
	IMalloc * pShellMalloc = NULL;        // A pointer to the shell's IMalloc interface
	TCHAR szPath[MAX_PATH];

	// Get the address of our task allocator's IMalloc interface
	HRESULT hres = SHGetMalloc(&pShellMalloc);
	
	if (FAILED(hres)) {
		return FALSE;
	}

	// Get a pointer to an item ID list that represents the path of a special folder
	hres = SHGetSpecialFolderLocation(NULL, nSpecialFolder,&pidlItem);


	if (SUCCEEDED(hres)) {
		// Convert the item ID list's binary representation into a file system path
		if(SHGetPathFromIDList(pidlItem, szPath)) {
			csFolderPath = szPath;
			csFolderPath += _T('\\');
		}

		// Free the item ID list allocated by SHGetSpecialFolderLocation
		pShellMalloc->Free(pidlItem);
		// Free our task allocator
		pShellMalloc->Release();		
	}

	return TRUE;
}

void CShortCut::setLinkPathStartup()
{
	GetSpecialFolderPath(CSIDL_COMMON_STARTUP,csLinkPath);
}

void  CShortCut::setLinkPathDesktop()
{
	GetSpecialFolderPath(CSIDL_DESKTOP,csLinkPath);
}

void  CShortCut::setLinkPathStartMenu()
{
	GetSpecialFolderPath(CSIDL_COMMON_STARTMENU,csLinkPath);
}

BOOL CShortCut::isShortCut()
{
	CFileStatus cfStatus;
	CString		csFilePathName;
	BOOL bReturn;

	MakeFullPathName(csFilePathName);

	if (CFile::GetStatus(csFilePathName,cfStatus)) {
		bReturn = TRUE;	
	} else {
		bReturn = FALSE;
	}

	return bReturn;
}