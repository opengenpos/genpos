/*
*===========================================================================
*
*   Copyright (c) 1992-1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Local Header File of "printf" Format Function
* Category    : Reentrant Functions Library, 2170 Application
* Program Name: RFLFORM.H
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: 
* Options     : 
* --------------------------------------------------------------------------
* Abstract:     This header file defines Reentrant Functions Library the
*               2170 system application.
*
*               This file contains:
*                   1. Symbol Definitions
*                   2. Data type Definitions
* --------------------------------------------------------------------------
* Update Histories
*
* Date       Ver.      Name         Description
* Mar-20-92  00.00.01  O.Nakada     Initial
* Feb-27-95  R3.0      O.Nakada     Refreshed all declarations.
*
* 
*** NCR2171 **
* Aug-30-99  01.00.00  M.Teraki     initial (for Win32)
* Oct-05-99            M.Teraki     Added #pragma pack(...)
*===========================================================================
*===========================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===========================================================================
*/


/*
*===========================================================================
*   Define Declarations 
*===========================================================================
*/
#define _RFL_LEN_BUF        256             /* length of buffer */

#define _RFL_INIT_DECIMAL   ((SHORT)0xFFFF) /* initiale value of decimal */
#define _RFL_INIT_COMMA     ((SHORT)0xFFFF) /* initiale value of comma   */


/*
*===========================================================================
*   Structure Type Define Declarations
*===========================================================================
*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif


typedef struct {
    SHORT   sDecimal;                       /* position of decimal */
    SHORT   sComma;                         /* position of comma   */
	UCHAR	uchDecimal;						/* symbol to use for decimal */	//ESMITH CURRENCY
	UCHAR	uchDigitGroup;					/* symbol to use for digit grouping */
} _RFLFMTCTRL;


#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
/* ====================================== */
/* ========== End of RFLFORM.C ========== */
/* ====================================== */

