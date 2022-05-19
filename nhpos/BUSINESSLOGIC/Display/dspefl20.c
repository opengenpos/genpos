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
* Title       : ETK File Maintenance Module( SUPER & PROGRAM MODE )                       
* Category    : Display-2x20, NCR 2170 US Hospitality Application Program        
* Program Name: DSPEFL20.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*               
*               DispSupEtkFile20()    : Display ETK File data
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Oct-05-93: 02.00.01 : J.Ikeda   : initial                                   
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
#include "rfl.h"
#include "csetk.h"
#include "paraequ.h"
#include "para.h"
/* #include <pmg.h> */
#include "maint.h"
#include "display.h"
#include "disp20.h"

/*
;==============================================================================
;                      C O M M O N   R A M   A R E A                         
;=============================================================================
*/


static VOID DispSupMakeTime(CONST MAINTETKFL *pData, TCHAR aszDispInTime[5], TCHAR aszDispOutTime[5]);

/*
*===========================================================================
** Synopsis:    VOID DispSupEtkFile20( MAINTETKFL *pData )
*               
*     Input:    *pData          : Pointer to Structure for MAINTETKFL
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This function displays ETK File Data
*===========================================================================
*/

VOID DispSupEtkFile20( MAINTETKFL *pData )
{
	TCHAR aszDispInTime[4 + 1] = { 0 };
	TCHAR aszDispOutTime[4 + 1] = { 0 };

    /* define display format */ 
    static const TCHAR  auchDispSupEtkFile20A[] = _T("%3u %2u  %2u %4s %4s\n%-18s\t%u");
    static const TCHAR  auchDispSupEtkFile20B[] = _T("%3u %2u%14s\n%-18s\t%u");

    TCHAR   aszDspNull[1] = {0};

    DispSupSaveData.EtkFl = *pData;    /* Save Data for Redisplay if needed. */

    /* make time */
    DispSupMakeTime(pData, aszDispInTime, aszDispOutTime);

    /* display ForeGround */
    UiePrintf(UIE_OPER,                             /* Operator Display */
              0,                                    /* Row */
              0,                                    /* Column */
              auchDispSupEtkFile20A,                /* Format */
              AC_ETK_MAINT,                         /* A/C number */
              pData->usBlockNo,                     /* Block No. */
              ( USHORT)pData->EtkField.uchJobCode,  /* Job Code */
              aszDispInTime,                        /* Time-in */
              aszDispOutTime,                       /* Time-out */
              pData->aszMnemonics,                  /* Lead Through */
              uchMaintMenuPage);                    /* Page Number */

    /* Display BackGround */
    UieEchoBackGround(UIE_KEEP,                         /* Descriptor Control */
                      UIE_ATTR_NORMAL,                  /* Attribute */
                      auchDispSupEtkFile20B,            /* Format */
                      AC_ETK_MAINT,                     /* A/C number */
                      pData->usBlockNo,                 /* Block No. */
                      aszDspNull,                       /* Null */
                      pData->aszMnemonics,              /* Lead Through */
                      uchMaintMenuPage);                /* Page Number */
}

/*
*===========================================================================
** Synopsis:    VOID  DispSupMakeTime( MAINTETKFL *pData )  
*               
*     Input:    *pData  : pointer to structure for MAINTETKFL 
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function makes TIME data.
*===========================================================================
*/

static VOID DispSupMakeTime(CONST MAINTETKFL *pData, TCHAR aszDispInTime[5], TCHAR aszDispOutTime[5])
{
	const TCHAR  auchDispTime[] = _T("%2u%02u");

	/* check TIME-IN */
    if (pData->EtkField.usTimeinTime == ETK_TIME_NOT_IN) {
        tcharnset(aszDispInTime, _T('*'), 4);
    } else {
        RflSPrintf(aszDispInTime, TCHARSIZEOF(aszDispInTime), auchDispTime, pData->EtkField.usTimeinTime, pData->EtkField.usTimeinMinute);
    }

    /* check TIME-OUT */
    if (pData->EtkField.usTimeOutTime == ETK_TIME_NOT_IN) {
        tcharnset(aszDispOutTime, _T('*'), 4);
    } else {
        RflSPrintf(aszDispOutTime, TCHARSIZEOF(aszDispOutTime), auchDispTime, pData->EtkField.usTimeOutTime, pData->EtkField.usTimeOutMinute);
    }
}
