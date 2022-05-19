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
* Title       : PLU Sales Restriction Table Setting Module                       
* Category    : Display-2x20, NCR 2170 US GP Application Program        
* Program Name: DSPRST20.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*               
*			    DispSupRest20()  : Display Sales Restriction Table
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Apr-20-92: 00.00.01 : M.Ozawa   : initial for GP
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
#include <stdlib.h>

#include "ecr.h"
#include "uie.h"
#include "paraequ.h"
#include "para.h"
#include "maint.h"
#include "regstrct.h"
#include "display.h"
#include "disp20.h"

/*
*=======================================================================================
** Synopsis:    VOID DispSupRest20( PARARESTRICTION *pData )
*               
*       Input:  *pData          : Pointer to Structure for PARARESTRICTION
*      Output:  Nothing 
*
** Return:      Nothing
*
** Description: This function displays Restriction Table
*=======================================================================================
*/

VOID DispSupRest20( PARARESTRICTION *pData )
{
    /* Define Display Format */
    static const TCHAR  auchDispSupRest20A[] = _T("%3u    %1u   %1u\t%2u\n%-20s");
    static const TCHAR  auchDispSupRest20B[] = _T("%3u    %1u   %1u\t%2s\n%-20s");

    TCHAR       aszDspNull[1] = {0};

	DispSupSaveData.Restriction = *pData;    /* Save Data for Redisplay if needed. */

    /* Display ForeGround */
    UiePrintf(UIE_OPER,                         /* Operator Display */
              0,                                /* Row */
              0,                                /* Column */
              auchDispSupRest20A,               /* Format */
              AC_PLU_SLREST,                    /* Super/Program number */
              pData->uchAddress,                /* Address */
              pData->uchField,                  /* field # */
              pData->uchData,                   /* data */
              pData->aszMnemonics);             /* Leadthough */

    /* Display BackGround */
    UieEchoBackGround(UIE_KEEP,                 /* Descriptor control */
                      UIE_ATTR_NORMAL,          /* Attribute */
                      auchDispSupRest20B,       /* Format */
                      AC_PLU_SLREST,            /* Super/Program number */
                      pData->uchAddress,        /* Address */
                      pData->uchField,          /* field # */
                      aszDspNull,               /* Null */
                      pData->aszMnemonics);     /* Leadthough */

}
