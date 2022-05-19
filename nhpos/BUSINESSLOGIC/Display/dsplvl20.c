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
* Title       : Supervisor Number/Level Assignment Module( SUPER & PROGRAM MODE )                       
* Category    : Display-2x20, NCR 2170 US Hospitality Application Program        
* Program Name: DSPLVL20.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*               
*               DispSupLevel20()     : Display Supervisor Number/Level Assignment
*                                           
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-17-92: 00.00.01 : K.You     : initial                                   
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
/* #include <pif.h> */
#include "paraequ.h"
#include "para.h"
#include "maint.h"
#include "display.h"
#include "disp20.h"

/*
*===========================================================================
** Synopsis:    VOID DispSupLevel20( PARASUPLEVEL *pData )
*               
*     Input:    *pData          : Pointer to Structure for PARASUPLEVEL
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Displays Supervisor Number/Level Assignment
*===========================================================================
*/

VOID DispSupLevel20( PARASUPLEVEL *pData )
{
    /* Define Display Format */ 
    static const TCHAR  auchDispSupSupLevel20A[] = _T("%3u  %3u\t%3u\n\t%s");
    static const TCHAR  auchDispSupSupLevel20B[] = _T("%3u  %3u\t%10s\n\t%s");

    TCHAR   aszDspNull[1] = {0};

    DispSupSaveData.SupLevel = *pData;    /* Save Data for Redisplay if needed. */

    /* Display ForeGround */
    UiePrintf(UIE_OPER,                             /* Operator Display */
              0,                                    /* Row */
              0,                                    /* Column */
              auchDispSupSupLevel20A,               /* Format */
              PG_SUPLEVEL,                          /* Program Number */
              ( USHORT)pData->uchAddress,           /* Address */
              pData->usSuperNumber,                 /* Supervisor Number */
              aszDspNull);                          /* Null */

    /* Display BackGround */
    UieEchoBackGround(UIE_KEEP,                     /* Descriptor Control */
                      UIE_ATTR_NORMAL,              /* Attribute */
                      auchDispSupSupLevel20B,       /* Format */
                      PG_SUPLEVEL,                  /* Program Number */
                      ( USHORT)pData->uchAddress,   /* Address */
                      aszDspNull,                   /* Null */
                      aszDspNull);                  /* Null */    
}
