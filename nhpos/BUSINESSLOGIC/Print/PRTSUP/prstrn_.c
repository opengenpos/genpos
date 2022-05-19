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
* Title       : Print Transaction Mnemonics Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSTRN_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               PrtSupTrans()   : This Function Forms Transaction Mnemonics Print Format
*                               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-22-92: 00.00.01 : K.You     : initial                                   
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
** Synopsis:    VOID  PrtSupTrans( MAINTTRANS *pData )
*               
*     Input:    *pData      : Pointer to Structure for MAINTTRANS
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Forms Transaction Mnemonics Print Format.
*===========================================================================
*/

VOID  PrtSupTrans( MAINTTRANS *pData )
{

    static const TCHAR FARCONST auchPrtSupTrans[] = _T("%s");

    USHORT usPrtType;


    /* Check Print Control */

    usPrtType = PrtSupChkType(pData->usPrtControl);

    PmgPrintf(usPrtType,                          /* Printer Type */
              auchPrtSupTrans,                    /* Format */
              pData->aszTransMnemo);              /* Transaction  Mnemonics */

}

