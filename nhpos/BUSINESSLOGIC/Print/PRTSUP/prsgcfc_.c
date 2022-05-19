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
* Title       : Print Emergent Guest Check File Close Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSGCFC_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function forms GUEST CHECK FILE print format.
*
*           The provided function names are as follows: 
* 
*               PrtSupGCFClose() : form GUEST CHECK No. print format
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-21-93: 00.00.01 : J.IKEDA   : Initial                                   
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
#include <stdlib.h>
#include <ecr.h>
/* #include <pif.h> */
/* #include <log.h> */
#include <paraequ.h> 
#include <para.h>
/* #include <cswai.h> */
#include <maint.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <csttl.h>
#include <csop.h>
#include <report.h>
/* #include <csstbpar.h> */
#include <pmg.h>
/* #include <appllog.h> */
#include "prtcom.h"
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtSupGCFClose( RPTGUEST *pData )
*               
*     Input:    *pData      : pointer to structure for RPTGUEST
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms GUEST CHEFCK No. print format.
*===========================================================================
*/

VOID  PrtSupGCFClose( RPTGUEST *pData )
{

    /* define print format */

    static const TCHAR FARCONST auchPrtSupGCF1[] = _T("   %-8.8s\t%4u%02u   ");/*8 characters JHHJ*/
    static const TCHAR FARCONST auchPrtSupGCF2[] = _T("   %-8.8s\t%4u   ");/*8 characters JHHJ*/

    USHORT usPrtType;

    /* check print control */

    usPrtType = PrtSupChkType(pData->usPrintControl);

    /* distinguish minor class */

    switch(pData->uchMinorClass) {
    case CLASS_RPTGUEST_PRTCHKNOWCD:
        
        PmgPrintf(usPrtType,                        /* printer type */
                  auchPrtSupGCF1,                   /* format */
                  pData->aszMnemo,                  /* transaction mnemonics */
                  pData->usGuestNo,                 /* guest check number */
                  ( USHORT)(pData->uchCdv));         /* check digit */
        break;

    case CLASS_RPTGUEST_PRTCHKNOWOCD:

        PmgPrintf(usPrtType,                        /* printer type */
                  auchPrtSupGCF2,                   /* format */
                  pData->aszMnemo,                  /* transaction mnemonics */
                  pData->usGuestNo);                /* guest check number */
        break;                                      

    default:
/*        PifAbort(MODULE_PRINT_SUPPRG_ID, FAULT_INVALID_DATA); */
        break;                                      
    }
}
