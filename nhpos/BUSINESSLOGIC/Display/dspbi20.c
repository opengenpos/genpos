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
* Title       : Data Setting II( SUPER & PROGRAM MODE )                       
* Category    : Display-2x20, NCR 2170 US Hospitality Application Program        
* Program Name: DSPBI20.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*               
*               DispSupBiData20()   : Display MDC,Action Code Security
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
/* #include <pif.h> */
/* #include <log.h> */
#include "paraequ.h"
#include "para.h"
#include "maint.h"
#include <appllog.h>
#include "display.h"
#include "disp20.h"

/*
*===========================================================================
** Synopsis:    VOID DispSupBiData20( VOID *pData )
*               
*       Input:  *pData          : Pointer to Structure for PARAMDC,PARAACTCODESEC  
*      Output:  Nothing 
*
** Return:      Nothing
*
** Description: This function displays MDC,Action Code Security 
*===========================================================================
*/

VOID DispSupBiData20( VOID *pData )
{
    /* Defined Display Format */
    static const TCHAR  auchDispSupBi20A[] = _T("%3u  %3u\t%4s\n\t%s");
    static const TCHAR  auchDispSupBi20B[] = _T("%3u  %3u\t%10s\n\t%s");

    TCHAR           aszDataString[7] = {0};
    TCHAR           aszDspNull[1] = {0};
    UCHAR           uchData = 0;
    USHORT          usAddress = 0;
    USHORT          usNoData = 0;
    PARAMDC         *pParaMDC = pData;
    PARAACTCODESEC  *pParaActCode = pData;

    switch(pParaMDC->uchMajorClass) {
    case CLASS_PARAMDC:                                         /* MDC Case */
        usAddress = pParaMDC->usAddress;                        /* Set Address */
        uchData = pParaMDC->uchMDCData;
        usNoData = PG_MDC;                                      /* Set Program Number */
        DispSupSaveData.MDC = *pParaMDC;                        /* Save Data for Redisplay if needed. */  
        break;

    case CLASS_PARAACTCODESEC:                                  /* Action Code Security Case */
        usAddress = pParaActCode->usAddress;                    /* Set Address */
        uchData = pParaActCode->uchSecurityBitPatern;
        usNoData = PG_ACT_SEC;                                  /* Set Program Number */
		DispSupSaveData.ActCodeSec = *pParaActCode;             /* Save Data for Redisplay if needed. */
        break;

    default:
        PifLog(MODULE_DISPSUP_ID, LOG_EVENT_UI_DISPLAY_ERR_02);
		return;
        break;
                                                               
    }

    _itot(uchData, aszDataString, 2);   /* Convert MDC Data to Binary ASCII Data */                         

    /* Display ForeGround */
    UiePrintf(UIE_OPER,                     /* Operator Display */
              0,                            /* Row */
              0,                            /* Column */
              auchDispSupBi20A,             /* Format */
              usNoData,                     /* Super/Program number */
              usAddress,                    /* Address */
              aszDataString,                /* String Data */
              aszDspNull);                  /* Null */

    /* Display BackGround */
    UieEchoBackGround(UIE_KEEP,             /* Descriptor control */
                      UIE_ATTR_NORMAL,      /* Attribute */
                      auchDispSupBi20B,     /* Format */
                      usNoData,             /* Super/Program number */
                      usAddress,            /* Address */
                      aszDspNull,           /* Null */
                      aszDspNull);          /* Null */    
}
