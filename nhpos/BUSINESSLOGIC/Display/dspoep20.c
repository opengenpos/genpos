/*
		Copyright	Georgia Southern University
					Statesboro,  GA 30460

					NCR Project Development Team
*===========================================================================
* Title       : DEPT/PLU Setting Module                       
* Category    : Display-2x20, NCR 2170 US Hospitality Application Program        
* Program Name: DSPOEP20.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*               
*               DispSupOepTbl20()  : Display Order Entry Prompt Table
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
*    -  -95: 00.00.01 :           : initial                                   
*          :          :           :                                    
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
/* #include <pif.h> */
/* #include <log.h> */
#include "paraequ.h"
#include "para.h"
#include "maint.h"
/* #include <appllog.h> */
#include "display.h"
#include "disp20.h"

/*
*=======================================================================================
** Synopsis:    VOID DispSupOepTbl20( PARAOEPTBL *)
*               
*       Input:  *pData          : Pointer to Structure for PARAOEPTBL
*      Output:  Nothing 
*
** Return:      Nothing
*
** Description: This function displays Order Entry Prompt Table
*=======================================================================================
*/

VOID DispSupOepTbl20(  PARAOEPTBL *pData )
{
    /* Define Display Format */
    static const TCHAR  auchDispSupOEPTBLA[] = _T("%3u  %2u %2u\t%3u\n%-18s\t%u");
    static const TCHAR  auchDispSupOEPTBLB[] = _T("%3u  %2u %2u %6s %3s\n%-18s\t%u");

    TCHAR       aszDspNull[1] = {_T('\0')};
	TCHAR       aszBuffer1[PARA_LEADTHRU_LEN + 1] = { 0 };
    USHORT      usNoData = 160;

    _tcsncpy(aszBuffer1, pData->aszMnemonics, PARA_LEADTHRU_LEN);    /* Set LeadThru Mnemonics */
    DispSupSaveData.ParaOepTbl = *pData;            /* Save Data */      

    /* Display ForeGround */
    UiePrintf(UIE_OPER,                         /* Operator Display */
              0,                                /* Row */
              0,                                /* Column */
              auchDispSupOEPTBLA,               /* Format */
              usNoData,                         /* Super/Program number */
              pData->uchTblNumber,              /* OEP Table No.*/
              pData->uchTblAddr,                /* OEP Address No.*/
              pData->uchOepData[0],             /* OEP Data */
              aszBuffer1,                       /* Lead Through Mnemonics */
              uchMaintMenuPage);                /* Page Number */

    /* Display BackGround */
    UieEchoBackGround(UIE_KEEP,                 /* Descriptor control */
                      UIE_ATTR_NORMAL,          /* Attribute */
                      auchDispSupOEPTBLB,       /* Format */
                      usNoData,                 /* Super/Program number */
                      pData->uchTblNumber,      /* OEP Table No.*/
                      pData->uchTblAddr,        /* OEP Address No.*/
                      aszDspNull,               /* Null */
                      aszDspNull,               /* Null */
                      aszBuffer1,               /* Lead Through Mnemonics */    
                      uchMaintMenuPage);        /* Page Number */
}
