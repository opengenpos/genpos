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
* Title       : Employee No. Assignment Module( SUPER & PROGRAM MODE )                       
* Category    : Display-2x20, NCR 2170 US Hospitality Application Program        
* Program Name: DSPETK20.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*               
*               DispSupEmployeeNo20()    : Display Employee No. Assignment
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-25-93: 00.00.01 : M.Yamamoto: initial                                   
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
#include "rfl.h"

/*
*===========================================================================
** Synopsis:    VOID DispSupEmployeeNo20( PARAEMPLOYEENO *pData )
*               
*     Input:    *pData          : Pointer to Structure for PARAEMPLOYEENO
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This function displays Employee No. Assignment Data
*===========================================================================
*/

VOID DispSupEmployeeNo20( PARAEMPLOYEENO *pData )
{
    /* Define Display Format */ 
    static const TCHAR  *auchDispSupEmployeeNo20A = _T("%3u %8.8Mu %2u\t%2u\n%-18s\t%u");
    static const TCHAR  *auchDispSupEmployeeNo20B = _T("%3u %8.8Mu %2u\t%3s\n%-18s\t%u");
    
    TCHAR   aszDspNull[1] = {0};
    UCHAR   uchJobCode = 0;

    DispSupSaveData.ParaEmployeeNo = *pData;    /* Save Data for Redisplay if needed. */
    
    /* Display ForeGround */
    if (pData->uchFieldAddr == 1) {
        uchJobCode = pData->uchJobCode1;
    } else if (pData->uchFieldAddr == 2) {
        uchJobCode = pData->uchJobCode2;
    } else if (pData->uchFieldAddr == 3) {
        uchJobCode = pData->uchJobCode3;
    }

    /* Display ForeGround */
    UiePrintf(UIE_OPER,                         /* Operator Display   */
              0,                                /* Row                */
              0,                                /* Column             */
              auchDispSupEmployeeNo20A,         /* Format             */
              AC_ETK_ASSIN,                     /* A/C Number         */
              RflTruncateEmployeeNumber(pData->ulEmployeeNo),              /* Employee Number    */
              pData->uchFieldAddr,              /* Address            */
              uchJobCode,                       /* Job code           */
              pData->aszMnemonics,              /* Lead Thru Data     */
              uchMaintMenuPage);                /* Page Number */

    /* Display BackGroud */
    UieEchoBackGround(UIE_KEEP,                 /* Descriptor Control */
                      UIE_ATTR_NORMAL,          /* Attribute          */
                      auchDispSupEmployeeNo20B, /* Format             */
                      AC_ETK_ASSIN,             /* A/C Number         */
                      RflTruncateEmployeeNumber(pData->ulEmployeeNo),      /* Employee Number    */
                      pData->uchFieldAddr,      /* Address            */
                      aszDspNull,
                      pData->aszMnemonics,      /* Lead Thru Data     */
                      uchMaintMenuPage);        /* Page Number */
}
