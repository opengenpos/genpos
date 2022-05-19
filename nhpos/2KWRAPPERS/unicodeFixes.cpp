/*************************************************************************
 *
 * unicodeFixes.cpp
 *
 * $Workfile:: unicodeFixes.cpp                                          $
 *
 * $Revision::	Initial				                                     $
 *
 * $Archive::                                                            $
 *
 * $Date:: 2/26/2003                                                     $
 *
 *************************************************************************
 *
 * Copyright (c) Georgia Southern University 2003.  All rights reserved.
 *
 *************************************************************************
 *
 * $History:: unicodeFixes.cpp                                           $
* ---------------------------------------------------------------------------
*   Date     | Version  | Descriptions                          | By
* ---------------------------------------------------------------------------
* 2/26/2003  |  Initial |   Win2k conversion wrapper functions  | cwunn
*            |          |                                       |
************************************************************************/
#include <unicodeFixes.h>
#ifdef _WIN32_WCE
FARPROC GetProcAddressUnicode (HMODULE hDll, LPCTSTR lpProcName)
{
	return GetProcAddress (hDll, lpProcName);
}
#else
#ifdef __cplusplus
extern "C" {
#endif
/*
**********************************************************************
**                                                                  **
**  Synopsis:   FARPROC GetProcAddressUnicode(HMODULE< LPCTSTR      **
**                                                                  **
**  return:     starting memory address of requested process        **
**                                                                  **
**  Description: Wrapper for Win2k to handle strip down the unicode **
**			string into a null terminiated char array.  Win2k does	**
**			support unicode for GetProcAddress (WinCE did).			**
**			If in WinCE, simply call GetProcAddress, else use our	**
**			stripping function and then call GetProcAddress			**
**                                                                  **
*********************************************************************/
//if WinCE, revert to GetProcAddress with no changes
//#ifdef _WIN32_WCE
//#define GetProcAddressUnicode  GetProcAddress
//#else
//if unicode in another OpSys, call our function
//#if defined(UNICODE) || defined(_UNICODE)
FARPROC GetProcAddressUnicode (HMODULE hDll, LPCTSTR lpProcName)
{
#ifndef _WIN32_WCE
//if unicode in another OpSys, call our function
#if defined(UNICODE) || defined(_UNICODE)
	//convert unicode to char *
	char buffer [128], *pb;
	for (pb = buffer; (*lpProcName) && (pb - buffer < sizeof (buffer)); ) {
		*pb++ = (char) *lpProcName++;
	}
	*pb = 0;
	//buffer cleaned, send to GetProcAddress
	return GetProcAddress (hDll, buffer);	
#else
	return GetProcAddress (hDll, lpProcName);
#endif
#else
	return GetProcAddress (hDll, lpProcName);
#endif

}
//#else
//if not using unicode in other OpSys, revert to GetProcAddress with no changes
//#define GetProcAddressUnicode  GetProcAddress
//#endif
//#endif

#ifdef __cplusplus
} //end extern C {
#endif
#endif
