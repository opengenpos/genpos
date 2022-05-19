/*
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
* Program Name: PrsPmeuT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Midium Model
* Options     : /c /AM /W4 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:
*
*               PrtSupPLUNoMenu_TH()    : This Function Forms Set PLU No. on Menu Page
*
* --------------------------------------------------------------------------
* Update Histories
*    Date  : Ver.Rev. :   Name    : Description
* Mar-25-97: 02.00.04 : S.Kubota  : Initial
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
#include <paraequ.h>
#include <para.h>
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <maint.h>
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <pmg.h>

#include "prtcom.h"
#include "prtsin.h"
/*
*===========================================================================
** Synopsis:    VOID PrtSupPLUNoMenu_TH( PARAPLUNOMENU *pData )
*
*     Input:    *pData      : Pointer to Structure for PARAPLUNOMENU
*    Output:    Nothing
*
** Return:      Nothing
*
** Description: This Function Forms Set PLU No. on Menu Page.
*===========================================================================
*/

VOID PrtThrmSupPLUNoMenu( PARAPLUNOMENU *pData )
{
    /* Define print format */
    static const TCHAR FARCONST auchPrtSupTHPLUNoMenu[] = _T("%1u - %3u  /  %1s %s");
    /* static const TCHAR FARCONST auchPrtSupTHPLUNoMenu[] = "%1u - %3u  /  %1u %1s %s"; */
    static const TCHAR FARCONST auchPrtSupEJPLUNoMenu1[] = _T("%1u - %3u");
    static const TCHAR FARCONST auchPrtSupEJPLUNoMenu2[] = _T("   %1s \t%s");
    /* static const TCHAR FARCONST auchPrtSupEJPLUNoMenu2[] = "   %1u %1s \t%s"; */

    TCHAR   aszStatus[2] = { 0, 0 };
    TCHAR   aszPLUNumber[PLU_MAX_DIGIT+1];

    memset(aszPLUNumber, 0, sizeof(aszPLUNumber));

    if ( 0 == _tcslen(pData->PluNo.aszPLUNumber) ) {
        _tcscpy(aszPLUNumber, _T("0"));
    }
    else {
        _tcscpy(aszPLUNumber, pData->PluNo.aszPLUNumber);
    }

    /* Check Print Control */
    if ( pData->PluNo.uchModStat & MOD_STAT_EVERSION ) {      /* E-Version Code */
        aszStatus[0] = _T('E');
    }
    else if ( pData->PluNo.uchModStat & MOD_STAT_VELOCITY ) { /* Velocity Code */
        aszStatus[0] = _T('V');
    }
    else {
        aszStatus[0] = _T(' ');
    }

    if (pData->usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */
        PmgPrintf(PMG_PRT_RECEIPT, auchPrtSupTHPLUNoMenu, (USHORT)pData->uchMenuPageNumber, (USHORT)pData->uchMinorFSCData, aszStatus, aszPLUNumber);   // print pData->PluNo.aszPLUNumber
    }

    if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */
        PmgPrintf(PMG_PRT_JOURNAL, auchPrtSupEJPLUNoMenu1, (USHORT)pData->uchMenuPageNumber, (USHORT)pData->uchMinorFSCData);
        PmgPrintf(PMG_PRT_JOURNAL, auchPrtSupEJPLUNoMenu2, aszStatus, aszPLUNumber);              //  print pData->PluNo.aszPLUNumber
    }
}

/* === End of PrsPmeuT.c === */