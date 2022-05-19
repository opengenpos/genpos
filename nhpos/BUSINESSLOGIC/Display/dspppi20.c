/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : PPI Maintenance Display Module                       
* Category    : Display-2x20, NCR 2170 US Hospitality Application Program        
* Program Name: DSPPPI20.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*               
*               DispSupPPI20()  : Display PPI Table
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Nov-29-95: 03.01.00 : M.Ozawa   : initial       
*
** NCR2171 **                                         
* Aug-26-99: 01.00.00 : M.Teraki  :initial (for 2171)
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
*=======================================================================================
** Synopsis:    VOID DispSupPPI20( MAINTPPI *)
*               
*       Input:  *pData          : Pointer to Structure for MAINTPPI
*      Output:  Nothing 
*
** Return:      Nothing
*
** Description: This function displays PPI Table
*=======================================================================================
*/

VOID DispSupPPI20(  MAINTPPI *pData )
{
    /* Define Display Format */
    static const TCHAR  auchDispSupPPIA[] = _T("%3u %3u %2u\t%3u-%6lu\n%-18s\t%u");
    static const TCHAR  auchDispSupPPIB[] = _T("%3u %3u %2u\t%-10s\n%-18s\t%u");

    TCHAR       aszDspNull[1] = {0};

    DispSupSaveData.MaintPpi = *pData;            /* Save Data */      

    /* Display ForeGround For 2X20 */
    UiePrintf(UIE_OPER,                         /* Operator Display */
              0,                                /* Row */
              0,                                /* Column */
              auchDispSupPPIA,                  /* Format */
              pData->uchACNumber,               /* Super/Program number */
              pData->uchPPINumber,              /* PPI No.*/
              pData->uchAddr,                   /* Address No.*/
              pData->uchQTY,                    /* QTY Data */
              pData->ulPrice,                   /* Price Data */
              pData->aszMnemonics,              /* Lead Through Mnemonics */
              uchMaintMenuPage);                /* Page Number */
   
    /* Display BackGround For 2X20 */
    UieEchoBackGround(UIE_KEEP,                 /* Descriptor control */
                      UIE_ATTR_NORMAL,          /* Attribute */
                      auchDispSupPPIB,      /* Format */
                      pData->uchACNumber,       /* Super/Program number */
                      pData->uchPPINumber,      /* PPI No.*/
                      pData->uchAddr,           /* Address No.*/
                      aszDspNull,               /* Null */
                      pData->aszMnemonics,      /* Lead Through Mnemonics */
                      uchMaintMenuPage);        /* Page Number */
}

/**** End of File ****/
