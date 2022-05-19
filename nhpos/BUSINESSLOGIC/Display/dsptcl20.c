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
* Title       : Total Key Control Module ( SUPER & PROGRAM MODE )                       
* Category    : Display-2x20, NCR 2170 US Hospitality Application Program        
* Program Name: DSPTCL20.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*               
*               DispSupTtlKeyCtl20()    : Display Total Key Control
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
#include <string.h>
#include <stdlib.h>

#include "ecr.h"
#include "uie.h"
#include "paraequ.h"
#include "para.h"
#include "maint.h"
#include "display.h"
#include "disp20.h"

/*
*===========================================================================
** Synopsis:    VOID DispSupTtlKeyCtl20( PARATTLKEYCTL *pData )
*               
*       Input:  *pData          : Pointer to Structure for PARATTLKEYCTL  
*      Output:  Nothing 
*
** Return:      Nothing
*
** Description: This Function Displays Total Key Control 
*===========================================================================
*/

VOID DispSupTtlKeyCtl20( PARATTLKEYCTL *pData )
{
    /* Defined Display Format */
    static const TCHAR  auchDispSupTtlKeyCtl20A[] = _T("%3u  %3u  %2u\t%4s\n\t%s");
    static const TCHAR  auchDispSupTtlKeyCtl20B[] = _T("%3u  %3u  %2u\t%8s\n\t%s");

    TCHAR   aszDataString[7] = {0};
    TCHAR   aszDspNull[1] = {0};

    DispSupSaveData.TtlKeyCtl = *pData;      /* Save Data for Redisplay if needed */

    /* Convert Binary Data to ASCII Data */
    _itot(pData->uchTtlKeyMDCData, aszDataString, 2);                            /* Convert MDC Data to Binary ASCII Data */

    /* Display ForeGround */
    UiePrintf(UIE_OPER,                             /* Operator Display */
              0,                                    /* Row */
              0,                                    /* Column */
              auchDispSupTtlKeyCtl20A,              /* Format */
              PG_TTLKEY_CTL,                        /* Program number */
              ( USHORT)pData->uchAddress,           /* Address */
              ( USHORT)pData->uchField,             /* Field */
              aszDataString,                        /* String Data */
              aszDspNull);                          /* Null */

    /* Display BackGround */
    UieEchoBackGround(UIE_KEEP,                     /* Descriptor control */
                      UIE_ATTR_NORMAL,              /* Attribute */
                      auchDispSupTtlKeyCtl20B,      /* Format */
                      PG_TTLKEY_CTL,                /* Program number */
                      ( USHORT)pData->uchAddress,   /* Address */
                      ( USHORT)pData->uchField,     /* Field */
                      aszDspNull,                   /* Null */
                      aszDspNull);                  /* Null */    
}
