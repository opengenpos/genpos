#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include "rptlocal.h"
#include "mld.h"



BOOL GetNHPOSSystemInformation( TCHAR *pInfoBuffer)
{
//	OSVERSIONINFOEX osvi;
//	BOOL bOsVersionInfoEx;
	DWORD dwHandle;
	DWORD viSize = 0;
	TCHAR infobuffer [1028];
	//TCHAR			infoBuffer2[1028];
	VS_FIXEDFILEINFO *lpBuf;
	unsigned int puLen;
	WIN32_FIND_DATA myFoundFile;
	HANDLE fileSearch;
	BOOL doit = TRUE;
	TCHAR		versionNumber[64];
	PULARGE_INTEGER lpFreeBytesAvailable;    // bytes available to caller
	PULARGE_INTEGER lpTotalNumberOfBytes;    // bytes on disk
	PULARGE_INTEGER lpTotalNumberOfFreeBytes; // free bytes on disk
	ULARGE_INTEGER FreeBytesAvailable;    // bytes available to caller
	ULARGE_INTEGER TotalNumberOfBytes;    // bytes on disk
	ULARGE_INTEGER TotalNumberOfFreeBytes; // free bytes on disk
	ULONG	ulTotalSize;
	ULONG   ulUsedSize;

	TCHAR pszDrive[] = _T("C:\\");

	//this is the first type of file that we are 
	//looking for, executables.
	TCHAR *myFileName = _T("*.exe");

	lpFreeBytesAvailable = &FreeBytesAvailable;    // bytes available to caller
	lpTotalNumberOfBytes = &TotalNumberOfBytes;    // bytes on disk
	lpTotalNumberOfFreeBytes = &TotalNumberOfFreeBytes; // free bytes on disk
	memset(&infobuffer, 0x00, sizeof(infobuffer));
	memset(versionNumber, 0x00, sizeof(versionNumber));

	//we need to change to this directory to look for the files
	_tchdir(STD_FOLDER_PROGRAMFILES);

	fileSearch = FindFirstFile (myFileName, &myFoundFile); //find the first file

	//this while loop will continue as long as there are .exe in the directory
	while (doit && fileSearch != INVALID_HANDLE_VALUE) 
	{
		//clean the buffer so we dont get erroneous data 
		memset(&versionNumber, 0x00, sizeof(versionNumber));
		//the next two functions get the file name, we must call the first one
		// in order to get the name, i dunno why, ask MSDN.
		viSize = GetFileVersionInfoSize (myFoundFile.cFileName, &dwHandle);
		GetFileVersionInfo (myFoundFile.cFileName, dwHandle, sizeof (infobuffer), infobuffer);
		VerQueryValue (infobuffer, _T("\\"), (void **)&lpBuf, &puLen);


		//format the information that we have just received to be printed
		_stprintf(versionNumber, _T("%.28s\t%2.2d.%2.2d.%2.2d.%2.2d\n"),
			myFoundFile.cFileName,
			HIWORD(lpBuf->dwFileVersionMS), LOWORD(lpBuf->dwFileVersionMS),
			HIWORD(lpBuf->dwFileVersionLS), LOWORD(lpBuf->dwFileVersionLS));

		//place it into the buffer
		_tcscat(pInfoBuffer, versionNumber);

		//find the next file, if there is not one, doit will be FALSE
		doit = FindNextFile (fileSearch, &myFoundFile);
	}
	
	FindClose (fileSearch);

	myFileName = _T("*.dll"); // find all .dll files
	doit = TRUE;

	fileSearch = FindFirstFile (myFileName, &myFoundFile);
	
	//this while loop will continue as long as there are .exe in the directory
	while (doit && fileSearch != INVALID_HANDLE_VALUE) 
	{
		//clean the buffer so we dont get erroneous data 
		memset(&versionNumber, 0x00, sizeof(versionNumber));
		//the next two functions get the file name, we must call the first one
		// in order to get the name, i dunno why, ask MSDN.
		viSize = GetFileVersionInfoSize (myFoundFile.cFileName, &dwHandle);
		GetFileVersionInfo (myFoundFile.cFileName, dwHandle, sizeof (infobuffer), infobuffer);
		VerQueryValue (infobuffer, _T("\\"), (void **)&lpBuf, &puLen);


		//format the information that we have just received to be printed
		_stprintf(versionNumber, _T("%.28s\t%2.2d.%2.2d.%2.2d.%2.2d\n"),
			myFoundFile.cFileName,
			HIWORD(lpBuf->dwFileVersionMS), LOWORD(lpBuf->dwFileVersionMS),
			HIWORD(lpBuf->dwFileVersionLS), LOWORD(lpBuf->dwFileVersionLS));

		//place it into the buffer
		_tcscat(pInfoBuffer, versionNumber);

		//find the next file, if there is not one, doit will be FALSE
		doit = FindNextFile (fileSearch, &myFoundFile);
	}
	

	FindClose (fileSearch);

#if defined(POSSIBLE_DEAD_CODE)
	// Function GetVersionEx() has been deprecated beginning with Windows 8.1 and should no longer
	// be used for obtaining the Windows version.  R.Chambers, Sep-11-2015

   // Try calling GetVersionEx using the OSVERSIONINFOEX structure.
   //
   // If that fails, try using the OSVERSIONINFO structure.

	_tcscat(pInfoBuffer, _T("OS Version:\t"));

   ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
   osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

   if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
   {
      // If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO.

      osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
      if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
         return FALSE;
   }

   //we use this to find out the correct Operating System, these defines are listed
   //in the MSDN information for the OSVERSIONINFO structure
   switch( osvi.dwMajorVersion)
   {
	 case 3:
	    _tcscat(pInfoBuffer, _T("Windows NT 3.51\n")); //i doubt anyone is using this, but we have it in case.
	    break;

	 case 4:
		   switch(osvi.dwMinorVersion)
		   {
			   case 0:
				  _tcscat(pInfoBuffer, _T("Windows 95\n"));
				   break;
			   case 10:
				 _tcscat(pInfoBuffer, _T("Windows  98\n"));
				   break;
			   case 90:
				 _tcscat(pInfoBuffer, _T("Windows  ME\n"));
				   break;
			   default:
				   break;
		   }
		break;

	 case 5:
		   switch(osvi.dwMinorVersion)
		   {
			   case 0:
				  _tcscat(pInfoBuffer, _T("Windows 2000\n"));
				   break;
			   case 1:
				 _tcscat(pInfoBuffer, _T("Windows  XP\n"));
				   break;
			   default:
				   break;
		   }

	 default:
			break;
	}

   //put the build Number into the information buffer
	_tcscat(pInfoBuffer, _T("OS Build:\t"));
	memset(&versionNumber, 0x00, sizeof(versionNumber));
	_stprintf(versionNumber, _T("%2.2d\n"), osvi.dwBuildNumber);

	_tcscat(pInfoBuffer, versionNumber);

    _tcscat(pInfoBuffer, osvi.szCSDVersion);
	_tcscat(pInfoBuffer, _T("\n"));
#endif

	GetDiskFreeSpaceEx(pszDrive,lpFreeBytesAvailable,lpTotalNumberOfBytes,lpTotalNumberOfFreeBytes);

	ulTotalSize = (ULONG)(TotalNumberOfBytes.QuadPart / 1000000000);
	ulUsedSize = (ULONG)(TotalNumberOfFreeBytes.QuadPart /1000000000);


	memset(versionNumber, 0x00, sizeof(versionNumber));
	_stprintf(versionNumber, _T("Bytes Used (Used/Total) in GB\t %3.3d/%3.3d\n"), (ulTotalSize-ulUsedSize), ulTotalSize);
	_tcscat(pInfoBuffer, versionNumber);
   
   return TRUE; 
}
