/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Thermal Print Set Menu Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSMENPT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:            
* 
*               PrtThrmSupMenuPLU() : form Set Menu print format     
*                                                                     
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-16-93: 01.00.12 : J.IKEDA   : Initial                                   
*          :          :           :                                    
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

/**
==============================================================================
;                      I N C L U D E     F I L E s                         
=============================================================================
**/
#include	<tchar.h>
#include <string.h>

#include <ecr.h>
/* #include <pif.h> */
/* #include <rfl.h> */
#include <paraequ.h> 
#include <para.h>
#include <csttl.h>
#include <csop.h>
#include <report.h>
/* #include <cswai.h>  */
#include <maint.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <rfl.h>
#include <pmg.h>

#include "prtcom.h"
/* #include "prtrin.h" */
#include "prtsin.h"
#include "prrcolm_.h"

/*
*===========================================================================
** Synopsis:    VOID PrtThrmSupMenuPLU( MAINTMENUPLUTBL *pData )
*               
*     Input:    *pData      : pointer to structure for MAINTMENUPLUTBL
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This function forms Set Menu print format.
*
*               : ASSIGN PROMOTIONAL PLU
*
*===========================================================================
*/

VOID PrtThrmSupMenuPLU( MAINTMENUPLUTBL *pData )
{

    /* define thermal print format */

    static const TCHAR FARCONST auchPrtThrmSupMenuPLU1[] = _T("  %-20s\t%s - %1u");
    static const TCHAR FARCONST auchPrtThrmSupMenuPLU2[] = _T("%6s%1u  /  %-20s\t%s - %1u");

    /* define EJ print format */

    static const TCHAR FARCONST auchPrtSupMenuPLU1[] = _T("%-s\t%s-%1u");
    static const TCHAR FARCONST auchPrtSupMenuPLU2[] = _T("%1u / %-s\t%s-%1u");

    static const TCHAR FARCONST auchPrtSupMenuPLU3[] = _T("%-s\n\t%s-%1u");
    static const TCHAR FARCONST auchPrtSupMenuPLU4[] = _T("%1u / %-s\n\t%s-%1u");

    TCHAR  aszPLUNo[PLU_MAX_DIGIT+1];
    TCHAR  aszPrtNull[1] = {'\0'};

	memset(aszPLUNo, 0x00, sizeof(aszPLUNo));
    RflConvertPLU(aszPLUNo, pData->SetPLU.aszPLUNumber);
    
    /* check print status */

    if (pData->usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */

        if (!(pData->uchArrayAddr)) {                       /* in case of parent PLU */
        
            /* print PARENT PLU */
        
            PrtPrintf(PMG_PRT_RECEIPT,                          /* printer type */
                      auchPrtThrmSupMenuPLU1,                   /* format */
                      pData->aszPLUMnemonics,                   /* PLU mnemonics */
                      /* aszPrtNull,*/                               /* null */
                      aszPLUNo,
                      /* pData->SetPLU.usPLU,*/                      /* PLU number */
                     ( USHORT)pData->SetPLU.uchAdjective);      /* adjective number */

        } else {                                                /* in case of child PLU */
        
            /* print CHILD PLU */
        
            PrtPrintf(PMG_PRT_RECEIPT,                          /* printer type */
                      auchPrtThrmSupMenuPLU2,                   /* format */
                      aszPrtNull,                               /* null */
                      ( USHORT)pData->uchArrayAddr,             /* array number of set PLU */
                      pData->aszPLUMnemonics,                   /* PLU mnemonics */
                      aszPLUNo,
                      /* pData->SetPLU.usPLU,*/                      /* PLU number */
                      ( USHORT)pData->SetPLU.uchAdjective);     /* adjective number */
        }
    } 
    
    if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */
    
        if (!(pData->uchArrayAddr)) {                       /* in case of parent PLU */
        
            /* print PARENT PLU */

            /* 2172 */
            if ((_tcslen(pData->aszPLUMnemonics) + _tcslen(aszPLUNo)) > PRT_EJCOLUMN-2) {

                PrtPrintf(PMG_PRT_JOURNAL,                          /* printer type */
                          auchPrtSupMenuPLU3,                       /* format */
                          pData->aszPLUMnemonics,                   /* PLU mnemonics */
                          aszPLUNo,
                          /* pData->SetPLU.usPLU,*/                      /* PLU number */
                          ( USHORT)pData->SetPLU.uchAdjective);      /* adjective number */
            } else {

                PrtPrintf(PMG_PRT_JOURNAL,                          /* printer type */
                          auchPrtSupMenuPLU1,                       /* format */
                          pData->aszPLUMnemonics,                   /* PLU mnemonics */
                          aszPLUNo,
                          /* pData->SetPLU.usPLU,*/                      /* PLU number */
                         ( USHORT)pData->SetPLU.uchAdjective);      /* adjective number */
            }

        } else {                                            /* in case of child PLU */
        
            /* print CHILD PLU */
        
            /* 2172 */
            if ((_tcslen(pData->aszPLUMnemonics) + _tcslen(aszPLUNo)) > (PRT_EJCOLUMN-6)) {

                PrtPrintf(PMG_PRT_JOURNAL,                          /* printer type */
                          auchPrtSupMenuPLU4,                       /* format */
                          ( USHORT)pData->uchArrayAddr,             /* array number of set PLU */
                          pData->aszPLUMnemonics,                   /* PLU mnemonics */
                          aszPLUNo,
                          /* pData->SetPLU.usPLU,*/                      /* PLU number */
                          ( USHORT)pData->SetPLU.uchAdjective);     /* adjective number */
            } else {

                PrtPrintf(PMG_PRT_JOURNAL,                          /* printer type */
                          auchPrtSupMenuPLU2,                       /* format */
                          ( USHORT)pData->uchArrayAddr,             /* array number of set PLU */
                          pData->aszPLUMnemonics,                   /* PLU mnemonics */
                          aszPLUNo,
                          /* pData->SetPLU.usPLU,*/                      /* PLU number */
                          ( USHORT)pData->SetPLU.uchAdjective);     /* adjective number */
            }
        }
    }
}
/***** End of Source *****/
