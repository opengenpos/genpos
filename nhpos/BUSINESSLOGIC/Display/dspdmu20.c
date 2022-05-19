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
*==========================================================================
* Title       : Set Dept No. on Menu Page Module( SUPER & PROGRAM MODE )                       
* Category    : Display-2x20, NCR 2170 US Hospitality Application Program        
* Program Name: DSPDMU20.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*               
*               DispSupDeptNoMenu20()    : Display Set Dept No. on Menu Page 
*                                                   
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jul-15-92: 00.00.01 : K.You     : initial                                   
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

#include "ecr.h"
#include "uie.h"
#include "paraequ.h"
#include "para.h"
#include "maint.h"
#include "display.h"
#include "disp20.h"

/*
*===========================================================================
** Synopsis:    VOID DispSupDeptNoMenu20( PARADEPTNOMENU *pData )
*               
*     Input:    *pData          : Pointer to Structure for PARADEPTNOMENU
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Displays Dept No. on Menu Page
*===========================================================================
*/

VOID DispSupDeptNoMenu20( PARADEPTNOMENU *pData )
{
    /* Define Display Format */ 
    static const TCHAR  auchDispSupDeptNoMenu20A[] = _T("%3u %3u\t%4u\n%-16s\t%1u");
    static const TCHAR  auchDispSupDeptNoMenu20B[] = _T("%3u %3u\t%10s\n%-16s\t%1u");

    TCHAR   aszDspNull[1] = {0};

    pData->uchMenuPageNumber = uchMaintMenuPage;
    DispSupSaveData.DeptNoMenu = *pData;      /* Save Data for Redisplay if needed. */

    /* Display ForeGround */
    UiePrintf(UIE_OPER,                                     /* Operator Display */
              0,                                            /* Row */
              0,                                            /* Column */
              auchDispSupDeptNoMenu20A,                      /* Format */
              AC_DEPT_MENU ,                                 /* A/C Number */
              ( USHORT)pData->uchMinorFSCData,              /* Minor FSC Data */
              pData->usDeptNumber,                           /* Dept Number */
              pData->aszMnemonics,                          /* Lead Thru data */
              ( USHORT)pData->uchMenuPageNumber);           /* Menu Page Number */

    /* Display BackGround */
    UieEchoBackGround(UIE_KEEP,                             /* Descriptor Control */
                      UIE_ATTR_NORMAL,                      /* Attribute */
                      auchDispSupDeptNoMenu20B,              /* Format */
                      AC_DEPT_MENU ,                         /* A/C Number */
                      ( USHORT)pData->uchMinorFSCData,      /* Minor FSC Data */
                      aszDspNull,                           /* Null */
                      pData->aszMnemonics,                  /* Lead Thru data */
                      ( USHORT)pData->uchMenuPageNumber);   /* Menu Page Number */

}
