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
* Title       : Thermal Print DEPT Setting Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSDEPTT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function forms DEPT print format.
*
*           The provided function names are as follows: 
* 
*               PrtThrmSupDEPT() : form DEPT print format
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-16-92: 01.00.12 : J.IKEDA   : Initial                                   
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
#include <ecr.h>
#include <stdlib.h>  
/* #include <pif.h> */
/* #include <rfl.h> */
#include <paraequ.h> 
#include <para.h>
/* #include <cswai.h> */
#include <maint.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>

#include "prtcom.h"
/* #include "prtrin.h" */
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtThrmSupDEPT( PARADEPT *pData )
*               
*     Input:    *pData      : pointer to structure for PARADEPT
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms DEPT print format.
*
*             : DEPARTMENT MAINTENANCE
*
*===========================================================================
*/

VOID  PrtThrmSupDEPT( PARADEPT *pData )
{

    /* define thermal print format */

    static const TCHAR FARCONST auchPrtThrmSupDEPT1[] = _T(" %4d\t%-20s");
    static const TCHAR FARCONST auchPrtThrmSupDEPT2[] = _T("    %2d  %4s  %4s  %4s  %4s  %4s  %4s");
    static const TCHAR FARCONST auchPrtThrmSupDEPT3[] = _T("     %1s  %4s    %02u    %2d  %4s  %4s");

    /* define EJ print format */

    static const TCHAR FARCONST auchPrtSupDEPT1[] = _T("%4d\t%-s");
    static const TCHAR FARCONST auchPrtSupDEPT2[] = _T("%2d %4s %4s %4s");
    static const TCHAR FARCONST auchPrtSupDEPT3[] = _T("%4s %4s %4s %s");
    static const TCHAR FARCONST auchPrtSupDEPT4[] = _T("%4s %02d %2d %4s");
    static const TCHAR FARCONST auchPrtSupDEPT5[] = _T("%4s");

    /* initialize buffer */

    TCHAR   aszAddress2[7] = { 0, 0, 0, 0, 0, 0, 0 };
    TCHAR   aszAddress3[7] = { 0, 0, 0, 0, 0, 0, 0 };
    TCHAR   aszAddress4[7] = { 0, 0, 0, 0, 0, 0, 0 };
    TCHAR   aszAddress5[7] = { 0, 0, 0, 0, 0, 0, 0 };
    TCHAR   aszAddress6[7] = { 0, 0, 0, 0, 0, 0, 0 };
    TCHAR   aszAddress7[7] = { 0, 0, 0, 0, 0, 0, 0 };
    TCHAR   aszAddress8[7] = { 0, 0, 0, 0, 0, 0, 0 };
    TCHAR   aszAddress9[7] = { 0, 0, 0, 0, 0, 0, 0 };
    TCHAR   aszAddress13[7] = { 0, 0, 0, 0, 0, 0, 0 };
    TCHAR   aszAddress14[7] = { 0, 0, 0, 0, 0, 0, 0 };

    /* convert DEPT status data to binary ASCII data */

    PrtSupItoa(( UCHAR)(pData->auchDEPTStatus[0] & 0x0F), aszAddress2);
    PrtSupItoa(( UCHAR)((pData->auchDEPTStatus[0] & 0xF0) >> 4), aszAddress3);
    PrtSupItoa(( UCHAR)(pData->auchDEPTStatus[1] & 0x0F), aszAddress4);
    PrtSupItoa(( UCHAR)((pData->auchDEPTStatus[1] & 0xF0) >> 4), aszAddress5);
    PrtSupItoa(( UCHAR)(pData->auchDEPTStatus[2] & 0x0F), aszAddress6);
    PrtSupItoa(( UCHAR)((pData->auchDEPTStatus[2] & 0xF0) >> 4), aszAddress7);
    _itot(pData->auchDEPTStatus[3] & 0x0F, aszAddress8, 10);
    PrtSupItoa(( UCHAR)((pData->auchDEPTStatus[3] & 0xF0) >> 4), aszAddress9);
    PrtSupItoa(( UCHAR)(pData->auchDEPTStatus[4] & 0x0F), aszAddress13);
    PrtSupItoa(( UCHAR)((pData->auchDEPTStatus[4] & 0xF0) >> 4), aszAddress14);

    /* check print control */

    if (pData->usPrintControl & PRT_RECEIPT) {    /* THERMAL PRINTER */

        /* print DEPT No./NAME */

        PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                  auchPrtThrmSupDEPT1,              /* format */             
                  (pData->usDEPTNumber),            /* dept number, 2172 */
                  pData->aszMnemonic);              /* mnemonic */

        /* print MDEPT No./PRT.PRIORITY/STATUS */

        PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                  auchPrtThrmSupDEPT2,              /* format */
                  ( USHORT)(pData->uchMajorDEPT),   /* major dept */
                  aszAddress2,                      /* status of address 2 */
                  aszAddress3,                      /* status of address 3 */
                  aszAddress4,                      /* status of address 4 */
                  aszAddress5,                      /* status of address 5 */
                  aszAddress6,                      /* status of address 6 */
                  aszAddress7);                     /* status of Address 7 */
                                                
        /* print STATUS/HALO */

        PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                  auchPrtThrmSupDEPT3,              /* format */
                  aszAddress8,                      /* status of address 8 */
                  aszAddress9,                      /* status of address 9 */
                  ( USHORT)(pData->uchHALO),        /* HALO */
                  pData->uchPrintPriority,          /* Print Priority */
                  aszAddress13,                     /* status of address 13 */
                  aszAddress14);                    /* status of address 14 */

    } 
    
    if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */

        /* print DEPT No./NAME */

        PrtPrintf(PMG_PRT_JOURNAL,                  /* printer type */
                  auchPrtSupDEPT1,                  /* format */
                  pData->usDEPTNumber,              /* dept number */
                  pData->aszMnemonic);              /* mnemonic */
        
        /* print MDEPT No./STATUS */

        PrtPrintf(PMG_PRT_JOURNAL,                  /* printer type */
                  auchPrtSupDEPT2,                  /* format */
                  ( USHORT)(pData->uchMajorDEPT),   /* major dept */
                  aszAddress2,                      /* Status of Address 2 */
                  aszAddress3,                      /* Status of Address 3 */
                  aszAddress4);                     /* Status of Address 4 */
                                                    
        /* print STATUS */

        PrtPrintf(PMG_PRT_JOURNAL,                  /* printer type */
                  auchPrtSupDEPT3,                  /* format */
                  aszAddress5,                      /* Status of Address 5 */
                  aszAddress6,                      /* Status of Address 6 */
                  aszAddress7,                      /* Status of Address 7 */
                  aszAddress8);                     /* Status of Address 8 */
        
        /* print STATUS/HALO */

        PrtPrintf(PMG_PRT_JOURNAL,                  /* printer type */
                  auchPrtSupDEPT4,                  /* format */
                  aszAddress9,                      /* Status of Address 9 */
                  ( USHORT)(pData->uchHALO),        /* HALO */
                  pData->uchPrintPriority,          /* Print Priority */
                  aszAddress13);                    /* status of address 13 */

        /* print STATUS */

        PrtPrintf(PMG_PRT_JOURNAL,                  /* printer type */
                  auchPrtSupDEPT5,                  /* format */
                  aszAddress14);                    /* status of address 14 */
    }
}
/***** End of Source *****/
