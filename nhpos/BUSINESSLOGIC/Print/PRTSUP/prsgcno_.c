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
* Title       : Print Guest Check Number Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSGCNO_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function forms GUEST CHECK Number print format.
*
*           The provided function names are as follows: 
* 
*               PrtSupGCNo()    : form GUEST CHECK Number print format
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-29-92: 00.00.01 : K.You     : initial                                   
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
#include <paraequ.h> 
#include <para.h>
#include <maint.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>

#include "prtcom.h"
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtSupGCNo( PARAGUESTNO *pData )
*               
*     Input:    *pData      : Pointer to Structure for PARAGUESTNO
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: Forms GUEST CHECK NO. print format.
*===========================================================================
*/

VOID  PrtSupGCNo( PARAGUESTNO *pData )
{


    /* Define Format Type */

    static const TCHAR FARCONST auchPrtSupGCNo[] = _T("%-6s%-8.8s %4u");/*8 characters JHHJ*/

    TCHAR   aszNull[1] = {'\0'};
    USHORT  usPrtType;


    /* check print control */

    usPrtType = PrtSupChkType(pData->usPrintControl);

    PmgPrintf(usPrtType,                        /* Printer Yype */
              auchPrtSupGCNo,                   /* Format */
              aszNull,                          /* Null */
              pData->aszMnemonics,              /* Transaction Mnemonics */
              pData->usGuestNumber);            /* Guest Check Number */
}
