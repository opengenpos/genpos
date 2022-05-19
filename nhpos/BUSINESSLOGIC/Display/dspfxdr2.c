/*
		Copyright	Georgia Southern University
					Statesboro,  GA 30460

					NCR Project Development Team
*===========================================================================
* Title       : FLEXIBLE DRIVE THROUGH PARAMETER Setting Module                       
* Category    : Display-2x20, NCR 2170 US Hospitality Application Program        
* Program Name: DSPFXDR2.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*               
*               DispSupFxDrive20()  : Display Flexible Drive Through Parameter
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Apr-18-95: 03.00.00 : T.Satoh   : initial                                   
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
** Synopsis:    VOID DispSupFxDrive20( PARAFXDRIVE *pData )
*               
*       Input:  *pData          : Pointer to Structure
*      Output:  Nothing 
*
** Return:      Nothing
*
** Description: This function displays Flexible Drive Through Parameter
*=======================================================================================
*/
VOID DispSupFxDrive20( PARAFXDRIVE *pData )
{
    /* Define Display Format */
    static const TCHAR  auchDispSupFXDRVA[]  = _T("%3u   %2u\t%3u\n%-18s\t%u");
    static const TCHAR  auchDispSupFXDRVB[]  = _T("%3u   %2u %6s %3s\n%-18s\t%u");

    TCHAR       aszDspNull[1] = {0};
	TCHAR       aszBuffer1[PARA_LEADTHRU_LEN + 1] = { 0 };
    USHORT      usNoData = 162;
    UCHAR       i;

	NHPOS_ASSERT(sizeof(aszBuffer1[0]) == sizeof(pData->aszMnemonics[0]));

	NHPOS_ASSERT(pData->uchTblAddr > 0);

    _tcsncpy(aszBuffer1, pData->aszMnemonics, PARA_LEADTHRU_LEN); /* Set LeadThru Mnemonics  */
    DispSupSaveData.ParaFxDrive = *pData;            /* Save Data for Redisplay if needed */      

    i = (UCHAR)(pData->uchTblAddr - 1);

    /* Display ForeGround */
    UiePrintf(UIE_OPER,                         /* Operator Display */
              0,                                /* Row */
              0,                                /* Column */
              auchDispSupFXDRVA,                /* Format */
              usNoData,                         /* Super/Program number */
              pData->uchTblAddr,                /* Address No.*/
              pData->uchTblData[i],             /* Data */
              aszBuffer1,                       /* Lead Through Mnemonics */
              uchMaintMenuPage);                /* Page Number */

    /* Display BackGround */
    UieEchoBackGround(UIE_KEEP,                 /* Descriptor control */
                      UIE_ATTR_NORMAL,          /* Attribute */
                      auchDispSupFXDRVB,        /* Format */
                      usNoData,                 /* Super/Program number */
                      pData->uchTblAddr,        /* Address No.*/
                      aszDspNull,               /* Null */
                      aszDspNull,               /* Null */
                      aszBuffer1,               /* Lead Through Mnemonics */    
                      uchMaintMenuPage);        /* Page Number */

}
