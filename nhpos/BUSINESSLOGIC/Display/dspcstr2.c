/*
		Copyright	Georgia Southern University
					Statesboro,  GA 30460

					NCR Project Development Team
*===========================================================================
* Title       : Control String Maintenance Display Module                       
* Category    : Display-2x20, NCR 2170 US Hospitality Application Program        
* Program Name: DSPcstr2.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*               
*               DispSupCstrTbl20()  : Display Control Strings Table
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
*    -  -95: 00.00.01 :           : initial       
* Aug-18-95: 03.00.04 : M.Ozawa   : distinguish '0' whether numeric data or string terminal.
* Sep-08-95: 03.00.08 : M.Ozawa   : display alpha mnemonic downloaded by PEP
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
#include "fsc.h"
#include "uie.h"
#include "paraequ.h"
#include "para.h"
#include "maint.h"
#include "display.h"
#include "disp20.h"

/*
*=======================================================================================
** Synopsis:    VOID DispSupCstrTbl20( MAINTCSTR *)
*               
*       Input:  *pData          : Pointer to Structure for MAINTCSTR
*      Output:  Nothing 
*
** Return:      Nothing
*
** Description: This function displays Control Strings Table
*=======================================================================================
*/

VOID DispSupCstrTbl20(  MAINTCSTR *pData )
{
    /* Define Display Format */
    static const TCHAR  auchDispSupCSTRTBLA[] = _T("%3u %3u %3u\t%3u-%3u\n%-18s\t%u");
    static const TCHAR  auchDispSupCSTRTBLB[] = _T("%3u %3u %3u %3s %3s\n%-18s\t%u");
    static const TCHAR  auchDispSupCSTRTBLC[] = _T("%3u %3u %3u\t***-***\n%-18s\t%u");
    static const TCHAR  auchDispSupCSTRTBLD[] = _T("%3u %3u %3u\t   -  %1c\n%-18s\t%u");

    TCHAR       aszDspNull[1] = {0};
	TCHAR       aszBuffer1[PARA_LEADTHRU_LEN + 1] = {0};

    _tcsncpy(aszBuffer1, pData->aszMnemonics, PARA_LEADTHRU_LEN);    /* Set LeadThru Mnemonics */
    DispSupSaveData.MaintCstr = *pData;                              /* Save Data for Redisplay if needed. */     

    /* Display ForeGround For 2X20 */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {    /* terminal of string */

        UiePrintf(UIE_OPER,                         /* Operator Display */
                  0,                                /* Row */
                  0,                                /* Column */
                  auchDispSupCSTRTBLC,               /* Format */
                  pData->uchACNumber,               /* Super/Program number */
                  pData->usCstrNumber,              /* Control String No.*/
                  pData->uchAddr,                   /* Address No.*/
                  aszBuffer1,                       /* Lead Through Mnemonics */
                  uchMaintMenuPage);                /* Page Number */
    } else {
        if (pData->uchMajorCstrData == FSC_AN) {        /* alpha mnemonic data downloaded by PEP */
            UiePrintf(UIE_OPER,                         /* Operator Display */
                      0,                                /* Row */
                      0,                                /* Column */
                      auchDispSupCSTRTBLD,              /* Format */
                      pData->uchACNumber,               /* Super/Program number */
                      pData->usCstrNumber,              /* Control String No.*/
                      pData->uchAddr,                   /* Address No.*/
                      pData->uchMinorCstrData,          /* Minor Data */
                      aszBuffer1,                       /* Lead Through Mnemonics */
                      uchMaintMenuPage);                /* Page Number */
        } else {
            UiePrintf(UIE_OPER,                         /* Operator Display */
                      0,                                /* Row */
                      0,                                /* Column */
                      auchDispSupCSTRTBLA,               /* Format */
                      pData->uchACNumber,               /* Super/Program number */
                      pData->usCstrNumber,              /* Control String No.*/
                      pData->uchAddr,                   /* Address No.*/
                      pData->uchMajorCstrData,          /* Major Data */
                      pData->uchMinorCstrData,          /* Minor Data */
                      aszBuffer1,                       /* Lead Through Mnemonics */
                      uchMaintMenuPage);                /* Page Number */
        }
    }

    /* Display BackGround For 2X20 */
    UieEchoBackGround(UIE_KEEP,                 /* Descriptor control */
                      UIE_ATTR_NORMAL,          /* Attribute */
                      auchDispSupCSTRTBLB,      /* Format */
                      pData->uchACNumber,       /* Super/Program number */
                      pData->usCstrNumber,      /* control String No.*/
                      pData->uchAddr,           /* Address No.*/
                      aszDspNull,               /* Null */
                      aszDspNull,               /* Null */
                      aszBuffer1,               /* Lead Through Mnemonics */    
                      uchMaintMenuPage);        /* Page Number */
}

/**** End of File ****/
