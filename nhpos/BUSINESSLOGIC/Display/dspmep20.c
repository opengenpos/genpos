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
* Title       : Assign Set Menu Module( SUPER & PROGRAM MODE )                       
* Category    : Display-2x20, NCR 2170 US Hospitality Application Program        
* Program Name: DSPMEP20.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*               
*               DispSupMenuPLU20()      : Display Set Menu
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
#include "rfl.h"
#include "display.h"
#include "disp20.h"

/*
*===========================================================================
** Synopsis:    VOID DispSupMenuPLU20( PARAMENUPLUTBL *pData )
*               
*     Input:    *pData          : Pointer to Structure for PARAMENUPLUTBL
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Displays Set Menu.
*===========================================================================
*/

VOID DispSupMenuPLU20( PARAMENUPLUTBL *pData )
{
    /* Define Display Format */ 
    static const TCHAR  auchDispSupMenuPLU20A[] = _T("%s\t%s-%1u\n%-s\t%1s");
    static const TCHAR  auchDispSupMenuPLU20B[] = _T("%s\t%16s\n%-s\t%1s");
    /* static const UCHAR FARCONST auchDispSupMenuPLU20A[] = "%3u  %s\t%4u-%1u\n%-s\t%1s";
    static const UCHAR FARCONST auchDispSupMenuPLU20B[] = "%3u  %s\t%10s\n%-s\t%1s"; */

    TCHAR   aszDspNull[1] = {0};
    TCHAR   aszBuffer1[7] = { 0 };
    TCHAR   aszBuffer2[7] = { 0 };
	TCHAR  aszPLUNo[PLU_MAX_DIGIT + 1] = { 0 };

    pData->uchPageNumber = uchMaintMenuPage;        /* Page Number */
    DispSupSaveData.MenuPLUTbl = *pData;      /* Save Data for Redisplay if needed. */

    /* convert array number to ASCII data */
    if (pData->uchArrayAddr) {
        _itot(pData->uchArrayAddr, aszBuffer1, 10);
    }

    if (pData->uchPageNumber) {
        _itot(pData->uchPageNumber, aszBuffer2, 10);
    }

    RflConvertPLU(aszPLUNo, pData->SetPLU[pData->uchArrayAddr].aszPLUNumber);

    /* Display ForeGround */
    UiePrintf(UIE_OPER,                                     /* Operator Display */
              0,                                            /* Row */
              0,                                            /* Column */
              auchDispSupMenuPLU20A,                        /* Format */
              /* AC_SET_PLU,                                   / A/C Number */
              aszBuffer1,                                   /* Array Number of Set Menu */
              aszPLUNo,                                     /* PLU Number */
              /* pData->SetPLU[pData->uchArrayAddr].usPLU,     / PLU Number */
              ( USHORT)(pData->SetPLU[pData->uchArrayAddr].uchAdjective), /* Adjective Number */
              pData->aszLeadMnemonics,                      /* Lead Thru Mnemonics */
              aszBuffer2);                                  /* Page Number */

    /* Display BackGround */

    UieEchoBackGround(UIE_KEEP,                             /* Descriptor Control */
                      UIE_ATTR_NORMAL,                      /* Attribute */
                      auchDispSupMenuPLU20B,                /* Format */
                      /* AC_SET_PLU,                           / A/C Number */
                      aszBuffer1,                           /* Array Number of Set Menu */
                      aszDspNull,                           /* Null */
                      pData->aszLeadMnemonics,              /* Lead Thru Mnemonics */
                      aszBuffer2);                          /* Page Number */

}
