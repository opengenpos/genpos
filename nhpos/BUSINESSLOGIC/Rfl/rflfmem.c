/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2171     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1999            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : fmemxxx functions
* Category    : Reentrant Functions Library, NCR 2171 US hospitarity Application
* Program Name: rflfmem.c
* --------------------------------------------------------------------------
* Compiler    : cl for VC++ Ver 6.0 by Microsoft Corp.                         
* --------------------------------------------------------------------------
* Abstract:
*      * This file contains following funcions,
*       _RflFMemCpy()
*       _RflFMemSet()
*
*      * On 16bit x86 platform (2170HPUS), fmemcpy/fmemset was used.
*      * At the migration work (to 2171HPUS), we desided to create these
*        functions as wrapper.
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Sep-03-99 : 01.00.00 : M.Teraki   : initial (for Win32)
*===========================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===========================================================================
*/

/**
;========================================================================
;+                                                                      +
;+                  I N C L U D E     F I L E s                         +
;+                                                                      +
;========================================================================
**/
#include <stdlib.h>
#include "ecr.h"

/**
;========================================================================
;+                                                                      +
;+        P R O G R A M    D E C L A R A T I O N s                      +
;+                                                                      +
;========================================================================
**/

VOID  RflMemRand (VOID *pDest, USHORT usLen)
{
	ULONG  ulMemSet = rand () * 0x1000 + rand ();
	ULONG  *pulDest = pDest;
	CHAR   *pchDest = 0;

	while (usLen > sizeof(ulMemSet)) {
		*pulDest = ulMemSet;
		usLen -= sizeof(ulMemSet);
		pulDest++;
	}

	for (pchDest = (CHAR *)pulDest; usLen > 1; usLen--, pchDest++) {
		*pchDest = 0;
	}
}

ULONG  RflMemHash (const VOID *pSrc, ULONG ulLen, ULONG ulHashStart)
{
	ULONG  ulHash = 15259;               // start the hash accumulator with a large prime number
	UCHAR  *pSrcMem = (UCHAR *) pSrc;
	
	if (ulHashStart != 0) {
		ulHash = ulHashStart;            // start the hash accumulator with provided value
	}

	for (; ulLen > 0; ulLen--, pSrcMem++) {
		ulHash *= 13;
		ulHash += *pSrcMem;
	}

	return ulHash;
}

/**
;============================================================================
; end of file
;============================================================================
**/
