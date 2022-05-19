/*************************************************************************
 *
 * unicodeFixes.h
 *
 * $Workfile:: unicodeFixes.h                                            $
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
 * $History:: unicodeFixes.h                                             $
* ---------------------------------------------------------------------------
*   Date     | Version  | Descriptions                          | By
* ---------------------------------------------------------------------------
* 2/26/2003  |  Initial |   Win2k conversion wrapper functions  | cwunn
*            |          |                                       |
************************************************************************/
#include <windows.h>
#ifdef _WIN32_WCE
FARPROC GetProcAddressUnicode(HMODULE, LPCTSTR);
#else
#ifdef __cplusplus
extern "C" {
#endif
//Function declarations
FARPROC GetProcAddressUnicode(HMODULE, LPCTSTR);
#ifdef __cplusplus
} //end extern C {
#endif
#endif
