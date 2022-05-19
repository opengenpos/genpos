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
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1997-2000       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print  Loan/Pickup Amount Print
* Category    : Print, NCR 2170 GP R2.0 Application
* Program Name: PrSLPJ_.C
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               PrtSupLoanPickup_EJ() : prints loan/pickup (receipt & journal)
*               PrtSupForeignTender_EJ() : prints loan/pickup (receipt & journal)
*
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
* Apr-04-97 : 02.00.05 :T.Yatuhasi  : initial
* Feb-19-00 : 01.00.00 :hrkato      : Saratoga
*============================================================================
*============================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*============================================================================
*/

/*
=============================================================================
+                        I N C L U D E   F I L E s
=============================================================================
*/
/**------- MS - C ---------**/
#include	<tchar.h>
#include <string.h>

/**------- 2170 local------**/
#include "ecr.h"
#include "rfl.h"
/* #include <uie.h> */
#include "regstrct.h"
#include "transact.h"
#include "paraequ.h"
#include "para.h"
#include "maint.h"
#include "csttl.h"
#include "csop.h"
#include "report.h"
#include "pmg.h"
#include "prtrin.h"
#include "prtsin.h"

/**
;============================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;============================================================================
**/

/*
*===========================================================================
** Format  : VOID  PrtSupLoanPickup_EJ(MAINTLOANPICKUP *pData);
*
*   Input  : MAINTLOANPICKUP      *pData     -Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints loan/pickup amount (receipt & journal)
*
*            See function PrtSupLoanPickup() which calls into here along with
*            calls to other versions of this function depending on the printer
*            setting.
*
*            See also functions PrtSupLoanPickup_SP() and PrtSupLoanPickup_TH().
*===========================================================================
*/
VOID    PrtSupLoanPickup_EJ( MAINTLOANPICKUP *pData )
{
    static const TCHAR FARCONST  aszPrtTHLPQty[]   = _T(" %5ld X\t%s ");
/*    static const UCHAR FARCONST  aszPrtRJLPFor[]   = " %5ld /";*/
    static const TCHAR FARCONST  aszPrtTHLPAmount[] = _T("%s\t%l$");

	TCHAR      aszSpecAmt[PARA_SPEMNEMO_LEN +PRT_AMOUNT_LEN + 1] = {0};
    TCHAR      aszTransBuff[PARA_TRANSMNEMO_LEN + 1] = {0};
    DCURRENCY  lAmount;
    USHORT     usPrtType;
    UCHAR      uchFCAdr;

/*    usPrtType = (PRT_RECEIPT | PRT_JOURNAL);*/
	usPrtType = PRT_JOURNAL;

    /* void, e/c print */
    /*PrtEJModifier( pData->usModifier );         /* common to reg. */

    PrtEJVoid(pData->usModifier, 0);

	/* e/c print */
	if(pData->usModifier & MAINT_MODIFIER_EC){
		PrtEJMnem(TRN_EC_ADR,PRT_DOUBLE);     /* E/C with double wide */
	}

    /* qty print */
    if(pData->usModifier & MAINT_MODIFIER_QTY){
        memset(&aszSpecAmt, 0x00, sizeof(aszSpecAmt));

       switch(pData->uchMinorClass){
       case CLASS_MAINTFOREIGN1:
           uchFCAdr  =(UCHAR)(SPC_CNSYMFC1_ADR);
            /* -- adjust foreign mnemonic and amount sign(+.-) -- */
            PrtAdjustForeign(aszSpecAmt, pData->lUnitAmount, uchFCAdr, pData->uchFCMDC);
           break;

       case CLASS_MAINTFOREIGN2:
           uchFCAdr  =(UCHAR)(SPC_CNSYMFC2_ADR);
            /* -- adjust foreign mnemonic and amount sign(+.-) -- */
            PrtAdjustForeign(aszSpecAmt, pData->lUnitAmount, uchFCAdr, pData->uchFCMDC);
           break;

       case CLASS_MAINTFOREIGN3:
           uchFCAdr  =(UCHAR)(SPC_CNSYMFC3_ADR);
            /* -- adjust foreign mnemonic and amount sign(+.-) -- */
            PrtAdjustForeign(aszSpecAmt, pData->lUnitAmount, uchFCAdr, pData->uchFCMDC);
           break;

       case CLASS_MAINTFOREIGN4:
           uchFCAdr  =(UCHAR)(SPC_CNSYMFC4_ADR);
            /* -- adjust foreign mnemonic and amount sign(+.-) -- */
            PrtAdjustForeign(aszSpecAmt, pData->lUnitAmount, uchFCAdr, pData->uchFCMDC);
           break;

       case CLASS_MAINTFOREIGN5:
           uchFCAdr  =(UCHAR)(SPC_CNSYMFC5_ADR);
            /* -- adjust foreign mnemonic and amount sign(+.-) -- */
            PrtAdjustForeign(aszSpecAmt, pData->lUnitAmount, uchFCAdr, pData->uchFCMDC);
           break;

       case CLASS_MAINTFOREIGN6:
           uchFCAdr  =(UCHAR)(SPC_CNSYMFC6_ADR);
            /* -- adjust foreign mnemonic and amount sign(+.-) -- */
            PrtAdjustForeign(aszSpecAmt, pData->lUnitAmount, uchFCAdr, pData->uchFCMDC);
           break;

       case CLASS_MAINTFOREIGN7:
           uchFCAdr  =(UCHAR)(SPC_CNSYMFC7_ADR);
            /* -- adjust foreign mnemonic and amount sign(+.-) -- */
            PrtAdjustForeign(aszSpecAmt, pData->lUnitAmount, uchFCAdr, pData->uchFCMDC);
           break;

       case CLASS_MAINTFOREIGN8:
           uchFCAdr  =(UCHAR)(SPC_CNSYMFC8_ADR);
            /* -- adjust foreign mnemonic and amount sign(+.-) -- */
            PrtAdjustForeign(aszSpecAmt, pData->lUnitAmount, uchFCAdr, pData->uchFCMDC);
           break;

        default:
            /* -- adjust native mnemonic and amount sign(+.-) -- */
            PrtAdjustNative(aszSpecAmt, pData->lUnitAmount);
            break;
       }

        PrtPrintf(usPrtType, aszPrtTHLPQty, pData->lForQty, aszSpecAmt);                      /* unit amount */
    }

    /* amount print */
    lAmount = pData->lAmount;

    switch ( pData->uchMinorClass ) {
    case CLASS_MAINTFOREIGN1:
    case CLASS_MAINTFOREIGN2:
    case CLASS_MAINTFOREIGN3:                       /* R2.0 Start */
    case CLASS_MAINTFOREIGN4:
    case CLASS_MAINTFOREIGN5:
    case CLASS_MAINTFOREIGN6:
    case CLASS_MAINTFOREIGN7:
    case CLASS_MAINTFOREIGN8:                       /* R2.0 End   */
        PrtSupForeignTender_EJ(pData);
        break;

    case CLASS_MAINTCOUNTUP:
        lAmount = pData->lTotal;                    /* set total amount */

    default:
		RflGetTranMnem(aszTransBuff, PrtChkLoanPickupAdr(pData->uchMajorClass, pData->uchMinorClass));

        PrtPrintf(usPrtType, aszPrtTHLPAmount, aszTransBuff, lAmount);
        break;
    }
}

/*
*===========================================================================
** Format  : VOID  PrtSupForeignTender_EJ(MAINTLOANPICKUP *pData);
*
*   Input  : MAINTLOANPICKUP      *pData     -loan/pick up data
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints foreign tender loan/pickup
*===========================================================================
*/
VOID PrtSupForeignTender_EJ(MAINTLOANPICKUP *pData)
{
    UCHAR     uchSymAdr;
    USHORT    usTrnsAdr;
                                                                    /* R2.0 Start */
    switch(pData->uchMinorClass){
    case CLASS_MAINTFOREIGN1:
        uchSymAdr  =(UCHAR)(SPC_CNSYMFC1_ADR);
        usTrnsAdr = (TRN_FT1_ADR);
        break;

    case CLASS_MAINTFOREIGN2:
        uchSymAdr  =(UCHAR)(SPC_CNSYMFC2_ADR);
        usTrnsAdr = (TRN_FT2_ADR);
        break;

    case CLASS_MAINTFOREIGN3:
        uchSymAdr  =(UCHAR)(SPC_CNSYMFC3_ADR);
        usTrnsAdr = (TRN_FT3_ADR);
        break;

    case CLASS_MAINTFOREIGN4:
        uchSymAdr  =(UCHAR)(SPC_CNSYMFC4_ADR);
        usTrnsAdr = (TRN_FT4_ADR);
        break;

    case CLASS_MAINTFOREIGN5:
        uchSymAdr  =(UCHAR)(SPC_CNSYMFC5_ADR);
        usTrnsAdr = (TRN_FT5_ADR);
        break;

    case CLASS_MAINTFOREIGN6:
        uchSymAdr  =(UCHAR)(SPC_CNSYMFC6_ADR);
        usTrnsAdr = (TRN_FT6_ADR);
        break;

    case CLASS_MAINTFOREIGN7:
        uchSymAdr  =(UCHAR)(SPC_CNSYMFC7_ADR);
        usTrnsAdr = (TRN_FT7_ADR);
        break;

    case CLASS_MAINTFOREIGN8:
        uchSymAdr  =(UCHAR)(SPC_CNSYMFC8_ADR);
        usTrnsAdr = (TRN_FT8_ADR);
        break;
    }
                                                                    /* R2.0 End   */
    PrtEJForeign1(pData->lAmount, uchSymAdr, pData->uchFCMDC);
    PrtEJForeign2(pData->ulFCRate, pData->uchFCMDC2);                /* foreign tender */
    PrtEJAmtSym(TRN_FT_EQUIVALENT, pData->lNativeAmount, PRT_SINGLE);      /* native tender */
}

/* --- End of Source File --- */