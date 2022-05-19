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
* Title       : Set Menu Format  ( SUPER & PROGRAM MODE )                       
* Category    : 21 Char Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSMENPS.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:            
* 
*               PrtSupMenuPLU_s()     : This Function Forms Set Menu      
*                                                                     
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Sep-22-92: 00.00.01 : J.IKeda   : initial                                   
* Nov-26-92: 01.00.00 : K.You     : Chg Function Name                                   
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
#include <paraequ.h> 
#include <para.h>
#include <csttl.h>
#include <csop.h>
#include <report.h> 
#include <maint.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <rfl.h>
#include <pmg.h>
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID PrtSupMenuPLU_s( MAINTMENUPLUTBL *pData )
*               
*     Input:    *pData      : Pointer to Structure for MAINTMENUPLUTBL
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Forms Set Menu.
*===========================================================================
*/

VOID PrtSupMenuPLU_s( MAINTMENUPLUTBL *pData )
{

    static const TCHAR FARCONST auchPrtSupMenuPLU1[] = _T("%-s\t%s-%1u");
    static const TCHAR FARCONST auchPrtSupMenuPLU2[] = _T("%1u /");

    USHORT      usPrtType;
    TCHAR  aszPLUNo[PLU_MAX_DIGIT+1];

	memset(aszPLUNo, 0x00, sizeof(aszPLUNo));
    RflConvertPLU(aszPLUNo, pData->SetPLU.aszPLUNumber);
    
    /* Check Print Control */

    usPrtType = PrtSupChkType(pData->usPrintControl);

    if (!(pData->uchArrayAddr)) {                       /* Print Parent PLU Case */

        PmgPrintf(usPrtType,                            /* Printer Type */
                  auchPrtSupMenuPLU1,                   /* Format */
                  pData->aszPLUMnemonics,               /* PLU Mnemonics */
                  aszPLUNo,                             /* PLU Number */
                  ( USHORT)pData->SetPLU.uchAdjective); /* Adjective Number */
    } else {

        PmgPrintf(usPrtType,                            /* Printer Type */
                  auchPrtSupMenuPLU2,                   /* Format */
                  ( USHORT)pData->uchArrayAddr);        /* Array Number of Set PLU */

        PmgPrintf(usPrtType,                            /* Printer Type */
                  auchPrtSupMenuPLU1,                   /* Format */
                  pData->aszPLUMnemonics,               /* PLU Mnemonics */
                  aszPLUNo,                             /* PLU Number */
                  ( USHORT)pData->SetPLU.uchAdjective); /* Adjective Number */

    }
}
