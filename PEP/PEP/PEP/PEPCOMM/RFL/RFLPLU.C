/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170 GP  *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1996            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Reentrant Functions Library : PLU ANALYSIS & SET FUNCTION
* Category    : Rfl Module, NCR 2170 GP R2.0 Model Application
* Program Name: RFLPLU.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Midium Model
* Options     : /c /AM /W4 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstruct: This function analyses PLU types and set PLU number in buffer.
*
*           The provided function names are as follows:
*
* --------------------------------------------------------------------------
* Update Histories
*   Date    : Version  :   Name     : Description
* May-28-93 : 00.00.02 : M.Inoue    : Initial
* Mar-05-94 : 01.00.00 : M.Sugiyama : Changed RflConvertPLU() source code:
*           :          :            :  When string data pointed by pszSourcePLU
*           :          :            :  is all "0" ASCII character's, return
*           :          :            :  with string data pointed by pszDestPLU
*           :          :            :  is NULL character.
* Dec-03-96 : 02.00.00 : S.Kubota   : Change RflConvertPLU() for UPC Velocity Code
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

/*------------------------------------------------------------------------*\

            I N C L U D E  F I L E S

\*------------------------------------------------------------------------*/
#include <string.h>
#include <stdlib.h>
#include "ecr.h"
#include "rfl.h"
#include "paraequ.h"
#include "para.h"       /* R2.0 */
#include "csstbpar.h"   /* R2.0 */

/*fhfh
***************************************************************************
*
**    Synopsis : VOID RflConvertPLU(UCHAR *pszDestPLU , UCHAR  *pszSourcePLU)
*
*     input    : None
*     in/out   : ;
*     output   : None
*
**    Return   : None
*
**    Description : This function analyses PLU types and set PLU number.
*
*          EAN-13      :
*          EAN-8       :
*          Velocity    :
*          UPC-A,UPC-E :
*
***************************************************************************
fhfh*/
VOID RflConvertPLU(WCHAR *pszDestPLU, WCHAR *pszSourcePLU)
{
    USHORT  i;

    memset(pszDestPLU,0,(PLU_MAX_DIGIT+1) * sizeof(WCHAR));
    for ( i = 0; (*(pszSourcePLU+i) == '0') && (i<PLU_MAX_DIGIT) ; i++) {
    }
    if(i>=PLU_MAX_DIGIT)
        return;

    switch(i) {
	case 0:				//RSS 14
    case 1:             /* EAN-13 */
    case 6:             /* EAN-8 */
    case 2:             /* UPC-A, UPC-E */
    case 3:
    case 4:             // UPC-A from VCS scanned items with fewer non-zero digits in manufacture's id
    case 5:             // UPC-A from VCS scanned items with fewer non-zero digits in manufacture's id
        memcpy(pszDestPLU, (pszSourcePLU), (PLU_MAX_DIGIT) * sizeof(WCHAR));
        break;

    default:            /* Velocity */
        memcpy(pszDestPLU, (pszSourcePLU + i), (PLU_MAX_DIGIT - i - 1) * sizeof(WCHAR));
        /* not print out C/D */
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
                                                            /* R2.0 Start */
        if ( 0 == CliParaMDCCheck( MDC_PLU5_ADR, EVEN_MDC_BIT3 ) ) {
            /*--- Not copy check digit ---*/
            memcpy(pszDestPLU, (pszSourcePLU + i), (PLU_MAX_DIGIT - i - 1) * sizeof(WCHAR));
        } else {
            /*--- Copy Check Digit ---*/
            memcpy(pszDestPLU, (pszSourcePLU + i), (PLU_MAX_DIGIT - i) * sizeof(WCHAR));
        }
#endif
        break;                                              /* R2.0 End */
    }
}

/***** End of rflplu.c *****/
