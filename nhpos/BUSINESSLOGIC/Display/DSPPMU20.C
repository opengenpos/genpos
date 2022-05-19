/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*==========================================================================
* Title       : Set PLU No. on Menu Page Module( SUPER & PROGRAM MODE )
* Category    : Display-2x20, NCR 2170 US GP Application Program
* Program Name: DSPPMU20.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Midium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:
*
*               DispSupPLUNoMenu20()    : Display Set PLU No. on Menu Page
*
* --------------------------------------------------------------------------
* Update Histories
*    Date  : Ver.Rev. :   Name    : Description
* Jun-23-93: 00.00.01 : M.Ozawa   : Ininial for GP
* Jul-09-93: 00.00.02 : T.Koike   : Modify for 13 digits keyed PLU handling
* Oct-09-96: 02.00.00 : Y.Sakuma  : Change DispSupPLUNoMenu20() for UPC Velocity Code
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
#include "paraequ.h"
#include "para.h"
#include "maint.h"
#include "regstrct.h"
#include "display.h"
/* #include <rfl.h> */
#include "disp20.h"

/*
*===========================================================================
** Synopsis:    VOID DispSupPLUNoMenu20( PARAPLUNOMENU *pData )
*
*     Input:    *pData          : Pointer to Structure for PARAPLUNOMENU
*    Output:    Nothing
*
** Return:      Nothing
*
** Description: This Function Displays PLU No. on Menu Page
*===========================================================================
*/

VOID DispSupPLUNoMenu20( PARAPLUNOMENU *pData )
{
    /* Define Display Format */
    static const TCHAR  auchDispSupPLUNoMenu20A[] = _T("%1u%3u %1s\t%s\n%-16s\t%1u");
    static const TCHAR  auchDispSupPLUNoMenu20B[] = _T("%1u%3u  %1s\t%s\n%-16s\t%1u");
    /* static const UCHAR  auchDispSupPLUNoMenu20A[] = "%1u%3u %1u%1s\t%s\n%-16s\t%1u";
    static const UCHAR  auchDispSupPLUNoMenu20B[] = "%1u%3u  %1s\t%s\n%-16s\t%1u"; */

    TCHAR   aszDspNull[1] = {0};
    TCHAR   aszDspStatus[2] = { _T(' '), 0};              /* Addition for R2.0 */
	TCHAR   aszPLUNumber[PLU_MAX_DIGIT + 1] = { 0 };

    if(0 == _tcslen(pData->PluNo.aszPLUNumber))
        _tcscpy(aszPLUNumber, _T("0"));
    else
        _tcscpy(aszPLUNumber, pData->PluNo.aszPLUNumber);

    DispSupSaveData.PLUNoMenu = *pData;    /* Save Data for Redisplay if needed. */

    /* --- Addition for R2.0 (UPC Velocity Code) --- */
    if( pData->PluNo.uchModStat & MOD_STAT_EVERSION) {          /* E-Version Code */
        aszDspStatus[0] = _T('E');
    }
    else if( pData->PluNo.uchModStat & MOD_STAT_VELOCITY) {     /* Velocity Code */
        aszDspStatus[0] = _T('V');
    }
    else {
        aszDspStatus[0] = _T(' ');
    }
    /* --- End of addition for R2.0 --- */

    if (pData->PluNo.uchAdjective) {  /* if display adjective code */

        /* Display ForeGround */

        UiePrintf(UIE_OPER,                                     /* Operator Display */
                  0,                                            /* Row */
                  0,                                            /* Column */
                  auchDispSupPLUNoMenu20A,                      /* Format */
                  AC_PLU_MENU ,                                 /* A/C Number */
                  (USHORT)pData->uchMinorFSCData,               /* Minor FSC Data (Address) */
                  /* (USHORT)pData->PluNo.uchAdjective, */                  /* Adjective Code */
/* Chg(R2.0)      pData->PluNo.uchModStat&MOD_STAT_EVERSION ? "E":" ",        E-Version Code */
                  aszDspStatus,                                 /* E-Version Code (Chg R2.0) */
                  aszPLUNumber,                                 /* PLU Number */
                  pData->aszMnemonics,                          /* Lead Thru data */
                  (USHORT)pData->uchMenuPageNumber);            /* Menu Page Number */

    } else {      /* if not display adjective code */

        /* Display ForeGround */

        UiePrintf(UIE_OPER,                                     /* Operator Display */
                  0,                                            /* Row */
                  0,                                            /* Column */
                  auchDispSupPLUNoMenu20B,                      /* Format */
                  AC_PLU_MENU ,                                 /* A/C Number */
                  (USHORT)pData->uchMinorFSCData,               /* Minor FSC Data (Address) */
/* Chg(R2.0)      pData->PluNo.uchModStat&MOD_STAT_EVERSION ? "E":" ",        E-Version Code */
                  aszDspStatus,                                 /* E-Version Code (Chg R2.0) */
                  aszPLUNumber,                                 /* PLU Number */
                  pData->aszMnemonics,                          /* Lead Thru data */
                  (USHORT)pData->uchMenuPageNumber);            /* Menu Page Number */

    }

    /* Display BackGround */

    if (pData->uchStatus & MAINT_DATA_INPUT) {  /* if display adjective code */

        /* Echo back with Adjective Code */

        UieEchoBackGround(UIE_KEEP,                             /* Descriptor Control */
                          UIE_ATTR_NORMAL,                      /* Attribute */
                          auchDispSupPLUNoMenu20A,              /* Format */
                          AC_PLU_MENU ,                         /* A/C Number */
                          (USHORT)pData->uchMinorFSCData,       /* Minor FSC Data (Address) */
                          /* (USHORT)pData->PluNo.uchAdjective, */   /* Adjective Code */
/* del. for R2.0          pData->uchStatus & MAINT_DATA_INPUT2 ? "E":" ",    E-Version Code */
                          pData->uchStatus & MAINT_DATA_INPUT2 ? aszDspStatus:_T(" "),  /*  E-Version Code (R2.0) */

                          aszDspNull,                           /* Null */
                          pData->aszMnemonics,                  /* Lead Thru data */
                          (USHORT)pData->uchMenuPageNumber);   /* Menu Page Number */
    } else {
        /* Not echo back with Adjective Code */

        UieEchoBackGround(UIE_KEEP,                             /* Descriptor Control */
                          UIE_ATTR_NORMAL,                      /* Attribute */
                          auchDispSupPLUNoMenu20B,              /* Format */
                          AC_PLU_MENU ,                         /* A/C Number */
                          (USHORT)pData->uchMinorFSCData,       /* Minor FSC Data (Address) */
/* del. for R2.0          pData->uchStatus & MAINT_DATA_INPUT2 ? "E":" ",    E-Version Code */
                          pData->uchStatus & MAINT_DATA_INPUT2 ? aszDspStatus:_T(" "),  /*  E-Version Code (R2.0) */
                          aszDspNull,                           /* Null */
                          pData->aszMnemonics,                  /* Lead Thru data */
                          (USHORT)pData->uchMenuPageNumber);    /* Menu Page Number */
    }
}

/*****  End of dsppmu20.c  *****/
