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
* Title       : Print DEPT Setting Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSDEPT_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function forms DEPT print format.
*
*           The provided function names are as follows: 
* 
*               PrtSupDEPT()    : form DEPT print format
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-13-92: 00.00.01 : K.You     : initial                                   
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
#include <paraequ.h> 
#include <para.h>
#include <maint.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtSupDEPT( PARADEPT *pData )
*               
*     Input:    *pData      : Pointer to Structure for PARADEPT
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms DEPT print format.
*===========================================================================
*/

VOID  PrtSupDEPT( PARADEPT *pData )
{

    /* Define Format Type */

    static const TCHAR FARCONST auchPrtSupDEPT1[] = _T("%4d\t%-s");
    static const TCHAR FARCONST auchPrtSupDEPT2[] = _T("%2d %4s %4s %4s");
    static const TCHAR FARCONST auchPrtSupDEPT3[] = _T("%4s %4s %4s %s");
    static const TCHAR FARCONST auchPrtSupDEPT4[] = _T("%4s %02d %2d %4s");
    static const TCHAR FARCONST auchPrtSupDEPT5[] = _T("%4s");

    /* Initialize Buffer */

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
    
    USHORT usPrtType;


    /* Check Print Control */
    
    usPrtType = PrtSupChkType(pData->usPrintControl);

    /* Convert DEPT Status Data to Binary ASCII Data */

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

    /* 1st Line */

    PmgPrintf(usPrtType,                        /* printer type */
              auchPrtSupDEPT1,                  /* format */
              pData->usDEPTNumber,              /* dept number */
              pData->aszMnemonic);              /* mnemonic */

    /* 2nd Line */

    PmgPrintf(usPrtType,                        /* printer type */
              auchPrtSupDEPT2,                  /* format */
              ( USHORT)(pData->uchMajorDEPT),   /* major dept */
              aszAddress2,                      /* Status of Address 2 */
              aszAddress3,                      /* Status of Address 3 */
              aszAddress4);                     /* Status of Address 4 */
                                                
    /* 3rd line */

    PmgPrintf(usPrtType,                        /* printer type */
              auchPrtSupDEPT3,                  /* format */
              aszAddress5,                      /* Status of Address 5 */
              aszAddress6,                      /* Status of Address 6 */
              aszAddress7,                      /* Status of Address 7 */
              aszAddress8);                     /* Status of Address 8 */

    /* 4th line */

    PmgPrintf(usPrtType,                        /* printer type */
              auchPrtSupDEPT4,                  /* format */
              aszAddress9,                      /* Status of Address 9 */
              ( USHORT)(pData->uchHALO),        /* HALO */
              pData->uchPrintPriority,          /* Print Priority */
              aszAddress13);                    /* Status of Address 13 */

    /* 5th line */

    PmgPrintf(usPrtType,                        /* printer type */
              auchPrtSupDEPT5,                  /* format */
              aszAddress14);                    /* status of address 14 */
}
