/*
*---------------------------------------------------------------------------
*  Georgia Southern University, Rsearch Services and Sponsored Programs
*    (C) Copyright 2002 - 2020
*
*  NHPOS, donated by NCR Corp to Georgia Southern University, August, 2002.
*  Developemnt with NCR 7448 then ported to Windows XP and generic x86 hardware
*  along with touch screen support.
*
*---------------------------------------------------------------------------
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1997            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print Set PLU No. on Menu Page Format  ( SUPER & PROGRAM MODE )
* Category    : Print, NCR 2170 GP R2.0 Application Program
* Program Name: PRSPMEU_.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:
*
*               PrtSupPLUNoMenu()    :
*               PrtSupPLUNoMenu_RJ() : This Function Forms Set PLU No. on Menu Page
*
* --------------------------------------------------------------------------
* Update Histories
*    Date  : Ver.Rev. :   Name    : Description
* Jun-23-93: 00.00.01 : M.Ozawa   : initial
* Jul-09-93: 00.00.02 : T.Koike   : Modify for 13 Digits Keyed PLU
* Oct-15-96: 02.00.00 : S.Kubota  : Change  for UPC Velocity Code
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
#include <ecr.h>
/* #include <pif.h> */
#include <paraequ.h>
#include <para.h>
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <maint.h>
#include <regstrct.h>
#include <transact.h>
#include <pmg.h>
/* #include <rfl.h> */
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID PrtSupPLUNoMenu( PARAPLUNOMENU *pData )
*
*     Input:    *pData      : Pointer to Structure for PARAPLUNOMENU
*    Output:    Nothing
*
** Return:      Nothing
*
** Description: This Function Forms Set PLU No. on Menu Page.
*===========================================================================
*/
VOID PrtSupPLUNoMenu( PARAPLUNOMENU *pData )
{
    static const TCHAR  auchPrtSupRJPLUNoMenu1[] = _T("%1u - %3u");
    static const TCHAR  auchPrtSupRJPLUNoMenu2[] = _T("   %1s \t%s");
    /* static const UCHAR FARCONST auchPrtSupRJPLUNoMenu2[] = "   %1u %1s \t%s"; */

    USHORT  usPrtType;
    TCHAR   aszStatus[2] = { 0, 0 };                /* R2.0 */
	TCHAR   aszPLUNumber[PLU_MAX_DIGIT + 1] = { 0 };

    if(0 == _tcslen(pData->PluNo.aszPLUNumber))
        _tcscpy(aszPLUNumber, _T("0"));
    else
        _tcscpy(aszPLUNumber, pData->PluNo.aszPLUNumber);

    /* Check Print Control */
                                                            /* R2.0 Start */
    if (pData->PluNo.uchModStat & MOD_STAT_EVERSION) {      /* E-Version Code */
        aszStatus[0] = _T('E');
    }
    else if (pData->PluNo.uchModStat & MOD_STAT_VELOCITY) { /* Velocity Code */
        aszStatus[0] = _T('V');
    }
    else {
        aszStatus[0] = _T(' ');
    }                                                       /* R2.0 End */

    usPrtType = PrtSupChkType(pData->usPrintControl);

    PmgPrintf(usPrtType,                            /* Printer Type */
              auchPrtSupRJPLUNoMenu1,                 /* Format */
              (USHORT)pData->uchMenuPageNumber,     /* Page Number */
              (USHORT)pData->uchMinorFSCData);      /* Minor FSC Data */

    PmgPrintf(usPrtType,                            /* Printer Type */
              auchPrtSupRJPLUNoMenu2,                 /* Format */
              /* (USHORT)pData->PluNo.uchAdjective,*/    /* Adjevtive Code */
              aszStatus,                            /* E-Version Code R2.0 */
              aszPLUNumber);                        /* PLU Number */

}

/*****  End of prspmeu_.c  *****/
