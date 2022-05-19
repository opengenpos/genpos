/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print module Common Include file                
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrtCOM.H                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-14-93 : 00.00.01 :  R.Itoh    :                                    
*           :          :            :                                    
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
============================================================================
+                       D E F I N I T I O N S
============================================================================
*/

#define PRT_7158SLIP        /* 7158 printer, saratoga */

/*--- for THERMAL PRINTER ---*/
#define PRT_HEADER_PRINTED   1   /* receipt header is printed */

extern const TCHAR aszPrtAM[];
extern const TCHAR aszPrtPM[];

extern UCHAR fbPrtTHHead;

USHORT    PrtPrint(USHORT usPrtType, TCHAR *puchBuff, USHORT usLen);
USHORT    PrtPrintf(USHORT usPrtType, const TCHAR FAR *pszFormat, ...);

