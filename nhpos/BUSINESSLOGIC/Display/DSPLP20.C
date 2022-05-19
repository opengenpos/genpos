/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Display Loan, Pickup Amount
* Category    : Display-2x20, NCR 2170 US GP Application Program
* Program Name: DSPLP20.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Midium Model
* Options     : /c /AM /W4 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:
*
*               DispSupLoanPickup20() : display loan, pickup amount
*
* --------------------------------------------------------------------------
* Update Histories
*    Date  : Ver.Rev. :   Name    : Description
* Mar-13-93: 00.00.01 : M.Ozawa   : initial for GP
* Oct-09-96: 02.00.00 : Y.Sakuma  : Change DispSupLoanPickup20() for FC #3-8
* Dec-02-99: 01.00.00 : hrkato    : Saratoga
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
#include <uie.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <regstrct.h>
#include <display.h>
#include "disp20.h"


/*
*=====================================================================================
** Synopsis:    VOID DispSupLoanPickup20( MAINTDSPLOANPICKUP *pData )
*
*     Input:    *pData
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function displays Loan, Pickup Amount
*======================================================================================
*/
VOID    DispSupLoanPickup20( MAINTDSPLOANPICKUP *pData )
{
    static const TCHAR FARCONST auchDispSupLoanPickup20A[] = _T("\t%l$\n%-20s");
    static const TCHAR FARCONST auchDispSupLoanPickup20B[] = _T("\t%10s\n%-20s");
    static const TCHAR FARCONST auchDispSupLoanPickup20C[] = _T("\t%ld\n%-20s");
    static const TCHAR FARCONST auchDispSupLoanPickup20D[] = _T("\t%.*l$\n%-20s");

    TCHAR   aszDspNull[1] = {_T('\0')};
    UCHAR   uchDecimal;

    /* Control Descriptor */
    DispSupOpDscrCntr();

    switch (pData->uchMinorClass) {
    case CLASS_MAINTOTHER:                          /* for/qty display */
        UiePrintf(UIE_OPER,                         /* operator display */
                  0,                                /* row */
                  0,                                /* column */
                  auchDispSupLoanPickup20C,         /* format */
                  (LONG)pData->lAmount,             /* quantity, a LONG format of %ld */
                  pData->aszMnemonics);             /* transaction mnemonic */

        /* Display Background */
        UieEchoBackGround(UIE_KEEP,                 /* descriptor control */
                          UIE_ATTR_NORMAL,          /* attribute */
                          auchDispSupLoanPickup20B, /* format */
                          aszDspNull,               /* null */
                          aszDspNull);              /* null */
        break;

    case CLASS_MAINTDSPCTL_AMT:                     /* void discriptor */
        UiePrintf(UIE_OPER,                         /* operator display */
                  0,                                /* row */
                  0,                                /* column */
                  auchDispSupLoanPickup20A,         /* format */
                  pData->lAmount,                   /* amount, a DCURRENCY format of %l$ */
                  pData->aszMnemonics);             /* transaction mnemonic */

        /* Display Background */
        UieEchoBackGround(UIE_KEEP,                 /* descriptor control */
                          UIE_ATTR_NORMAL,          /* attribute */
                          auchDispSupLoanPickup20B, /* format */
                          aszDspNull,               /* null */
                          aszDspNull);              /* null */
        break;

    case CLASS_MAINTFOREIGN1:
    case CLASS_MAINTFOREIGN2:
    case CLASS_MAINTFOREIGN3:
    case CLASS_MAINTFOREIGN4:
    case CLASS_MAINTFOREIGN5:
    case CLASS_MAINTFOREIGN6:
    case CLASS_MAINTFOREIGN7:
    case CLASS_MAINTFOREIGN8:

        if ((pData->uchFCMDC & 0x04) && (pData->uchFCMDC & 0x02)) {  /* no decimal point */
            UiePrintf(UIE_OPER,                         /* operator display */
                      0,                                /* row */
                      0,                                /* column */
                      auchDispSupLoanPickup20C,         /* format */
                      (LONG)pData->lAmount,             /* quantity, a LONG format of %ld */
                      pData->aszMnemonics);             /* transaction mnemonic */

            /* Display Background */
            UieEchoBackGround(UIE_DESC10_OFF,           /* reset descriptor */
                          UIE_ATTR_NORMAL,          /* attribute */
                          auchDispSupLoanPickup20B, /* format */
                          aszDspNull,               /* null */
                          aszDspNull);              /* null */

        } else {
            if (pData->uchFCMDC & 0x02) {
                uchDecimal = 3;                         /* 0.000 display */
            } else {
                uchDecimal = 2;                         /* 0.00 display */
            }
            UiePrintf(UIE_OPER,                         /* operator display */
                      0,                                /* row */
                      0,                                /* column */
                      auchDispSupLoanPickup20D,         /* format */
                      uchDecimal,                       /* decimal position count */
                      pData->lAmount,                   /* amount, a DCURRENCY format of %.*l$ */
                      pData->aszMnemonics);             /* transaction mnemonic */

            /* Display Background */
            UieEchoBackGround(UIE_DESC10_OFF,           /* reset descriptor */
                          UIE_ATTR_NORMAL,          /* attribute */
                          auchDispSupLoanPickup20B, /* format */
                          aszDspNull,               /* null */
                          aszDspNull);              /* null */
        }
        break;

    default:
        UiePrintf(UIE_OPER,                         /* operator display */
                  0,                                /* row */
                  0,                                /* column */
                  auchDispSupLoanPickup20A,         /* format */
                  pData->lAmount,                   /* amount, a DCURRENCY*/
                  pData->aszMnemonics);             /* transaction mnemonic */

        /* Display Background */
        UieEchoBackGround(UIE_DESC10_OFF,           /* reset descriptor */
                          UIE_ATTR_NORMAL,          /* attribute */
                          auchDispSupLoanPickup20B, /* format */
                          aszDspNull,               /* null */
                          aszDspNull);              /* null */
    }
}

/* --- End of Source File --- */