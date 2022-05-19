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
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print  Department sales
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRDept_.C
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*           PrtDept() : prints dept sales
*           PrtDeptPLU_RJ() : prints dept sales ( receipt & journal )
*           PrtDeptPLUSet_VL() : prints Dept PLU Setmenu sales on validation
*           PrtDeptPLU_SP() : prints dept sales ( slip )
*           PrtDflDeptPLU() : displays dept/plu sales
*
*           PrtDflDeptPLU() : displays dept/plu sales
*           PrtDflDeptPLUForm() : displays dept/plu sales
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
* Jun-12-92 : 00.00.01 : K.Nakajima :
* Jul-10-93 : 01.00.12 :  R.Itoh    : Add PrtDflDeptPLU()
* Oct-28-93 : 02.00.02 :  K.You     : Del. PrtDflDeptPLU()
* Apr-08-94 : 00.00.04 :  K.You     : add validation slip print feature.(mod. PrtDept())
* Jan-17-95 : 03.00.00 : M.Ozawa    : add PrtDflDeptPLU() for display on the fly
* Jul-21-95 : 03.00.04 : T.Nakahata : display customer name at 1st frame
* Nov-20-95 : 03.01.00 : M.Ozawa    : print seat no.
* Nov-18-99 : 00.00.01 :  M.Ozawa   : Enhanced to 2172
* Dec-14-99 : 00.00.01 :  M.Ozawa   : Add PrtDflDeptPLUForm()
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
#include <ecr.h>
#include <regstrct.h>
#include <transact.h>
#include <paraequ.h>
#include <rfl.h>
#include <para.h>
#include <csstbpar.h>
#include <dfl.h>
#include <pmg.h>
#include "prtrin.h"
#include "prtdfl.h"
#include "prrcolm_.h"

/**
;============================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;============================================================================
**/
/*
*===========================================================================
** Format  : VOID    PrtDept(TRANINFORMATION  *pTran, ITEMSALES *pItem);
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMSALES        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints dept sales.
*===========================================================================
*/
VOID PrtDept(TRANINFORMATION  *pTran, ITEMSALES  *pItem)
{

    /* -- set print portion to static area "fsPrtPrintPort" -- */
    if ((pTran->TranCurQual.auchTotalStatus[4] & CURQUAL_TOTAL_NOCONSOLIDATE_PRINT) ||    /* not consolidation */
        ((pTran->TranCurQual.flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_UNBUFFER) ||        /* unbuffering print */
        (pItem->fsPrintStatus & (PRT_SINGLE_RECPT | PRT_DOUBLE_RECPT))) {  /* tichet */

        PrtPortion(pItem->fsPrintStatus);
    } else {                                                    /* consolidation */
        PrtPortion2(pItem);
    }

    if ( fsPrtStatus & PRT_REQKITCHEN ) {     /* kitchen print */
        PrtKPItem(pTran, pItem);
    }

    if ( pItem->fsPrintStatus & PRT_VALIDATION ) { /* validation print */
        if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) {
            if(PRT_SUCCESS != PrtSPVLInit())
			{
				return;
			}
			PmgBeginSmallValidation( PMG_PRT_SLIP );//change to allow Small Slips
            PrtSPVLHead(pTran);
            PrtDeptPLU_SP(pTran, pItem);
            PrtSPVLTrail(pTran);
			PmgEndSmallValidation( PMG_PRT_SLIP );//change to allow Small Slips
        } else {
            PrtDeptPLUSet_VL(pTran, pItem);
        }
        return ;
    }

    if ( fsPrtPrintPort & PRT_SLIP ) {        /* slip print */
        PrtDeptPLU_SP(pTran, pItem);
    }

    if (   (fsPrtPrintPort & PRT_RECEIPT)     /* receipt, journal print */
        || (fsPrtPrintPort & PRT_JOURNAL)) {

        PrtDeptPLU_RJ(pTran, pItem);
    }
}

/*
*===========================================================================
** Format  : VOID   PrtDeptPLU_RJ(TRANINFORMATION  *pTran, ITEMSALES *pItem);
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMSALES        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints dept sales ( receipt & journal ) .
*===========================================================================
*/
VOID   PrtDeptPLU_RJ(TRANINFORMATION  *pTran, ITEMSALES *pItem)
{
	TCHAR      aszPLUNo[PLU_MAX_DIGIT + 1] = {0};
    DCURRENCY  lPrice;
    LONG       lQTY;
	SHORT      numCounter; //US Customs

    PrtRJVoid(pItem->fbModifier);               /* void line */

//Saratoga Functionality (US Customs cwunn 4/25/03)
	if(!(CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2))) { //if US Customs #/type bit turned off (turned on is lower down)
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				PrtRJMnemNumber(TRN_SKUNO_ADR, pItem->aszNumber[numCounter]);              /* number line */
			}
		} else {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				PrtRJNumber(pItem->aszNumber[numCounter]);              /* number line */
			}
		}
	}
//End Saratoga Functionality

    PrtRJTaxMod(pTran->TranModeQual.fsModeStatus, pItem->fbModifier);

    if ((fsPrtStatus & PRT_TAKETOKIT) ||  /* take to kitchen status */
            (pItem->fsPrintStatus & (PRT_SINGLE_RECPT | PRT_DOUBLE_RECPT)) ) {
            /* ticket print status */

        if (pItem->fsPrintStatus & (PRT_SINGLE_RECPT | PRT_DOUBLE_RECPT)) {
            PrtRJSeatNo(pItem->uchSeatNo);                     /* seat no. */
        } else {
            PrtRJSeatNo2(pItem->uchSeatNo);                     /* seat no. */
        }
    }

    /* -- if Building operation Mnemonics 2172   -- */
    if ((pItem->uchMinorClass == CLASS_PLU) || (pItem->uchMinorClass == CLASS_PLUITEMDISC)) {
        RflConvertPLU(aszPLUNo, pItem->auchPLUNo);
        if (pItem->fsLabelStatus & ITM_CONTROL_NOTINFILE)
        {
            PrtJPLUBuild(TRN_PLUBLD_ADR, aszPLUNo, pItem->usDeptNo);
        } else {
            PrtJPLUNo(aszPLUNo);
        }
        if ( CliParaMDCCheck(MDC_PLU2_ADR, ODD_MDC_BIT2) ) {
            PrtRPLUNo(aszPLUNo);
        }
    }

    if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
        PrtRJWeight(pTran, pItem);              /* weight price */
    }  else {
        PrtRJQty(pTran, pItem);                 /* qty price */
    }

    PrtRItems(pTran, pItem);
    PrtJItems(pItem);

    if (pItem->usLinkNo) {
        PrtRJVoid(pItem->fbModifier);               /* void line */
        PrtRJTaxMod(pTran->TranModeQual.fsModeStatus, pItem->fbModifier);
        RflConvertPLU(aszPLUNo, pItem->Condiment[0].auchPLUNo);
        PrtJPLUNo(aszPLUNo);
        if ( CliParaMDCCheck(MDC_PLU2_ADR, ODD_MDC_BIT2) ) {
            PrtRPLUNo(aszPLUNo);
        }

        if ((pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) == 0) {
            PrtRJLinkQty(pTran, pItem);                 /* qty price */
        }

        lPrice = pItem->Condiment[0].lUnitPrice;
        if ( fsPrtPrintPort & PRT_RECEIPT ) {     /* Only Receipt */
            /* set qty by void consolidation */
            if (PrtChkVoidConsolidation(pTran, pItem)) {
                lQTY = (LONG)pItem->sCouponQTY;
            } else {
                lQTY = pItem->lQTY / PLU_BASE_UNIT;
            }
        } else {                                /* Journal */
            lQTY = pItem->lQTY / PLU_BASE_UNIT;
        }
        if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
            lQTY = (pItem->lQTY >= 0 ? 1L : -1L);
        }

        lPrice *= lQTY;
        PrtRJLinkPLU(FALSE, pItem->Condiment[0].uchAdjective, pItem->Condiment[0].aszMnemonic, lPrice);
    }

//US Customs Functionality (US Customs cwunn 4/25/03)
	if(CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)) { //if US Customs #/type bit turned off (turned on is lower down)
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				PrtRJMnemNumber(TRN_SKUNO_ADR, pItem->aszNumber[numCounter]);              /* number line */
			}
		} else {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				PrtRJNumber(pItem->aszNumber[numCounter]);              /* number line */
			}
		}
	}
//End US Customs Functionality

}

/**fh
;============================================================================
;
;   function : Print Dept PLU Setmenu sales ( Validation )
;
;   format   : SHORT   PrtDeptPLUSet_VL(TRANINFORMATION  *pTran,
;                                       ITEMSALES *pItem);
;
;   input    : TRANINFORMATION  *pTran     -Transaction information address
;              ITEMSALES        *pItem     -Item Data address
;
;   return   : none
;
;   synopis  : This function prints Dept PLU Setmenu sales on validation.
;
;============================================================================
fh**/
VOID   PrtDeptPLUSet_VL(TRANINFORMATION *pTran, ITEMSALES *pItem)
{
    TCHAR  aszPLUNo[PLU_MAX_DIGIT + 1];

	memset(aszPLUNo, 0x00, sizeof(aszPLUNo));

    PmgBeginValidation(PMG_PRT_RCT_JNL);        /* begin validation */

    /* -- in case of 24 char printer -- */
    if ( usPrtVLColumn == PRT_VL_24 ) {

        if ((pItem->fsLabelStatus & ITM_CONTROL_NOTINFILE) == 0)
        {
            PrtVLHead(pTran, pItem->fbModifier);    /* send header lines */
        }

        /* -- scalable item? -- */
        if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
            PrtVLNumWeight(pTran, pItem);
        }  else {
            PrtVLNumQty(pItem);
        }

        if (pItem->fsLabelStatus & ITM_CONTROL_NOTINFILE)
        {
            RflConvertPLU(aszPLUNo, pItem->auchPLUNo);
            PrtVLPLUBuild(TRN_PLUBLD_ADR, aszPLUNo, pItem->usDeptNo);
        }

        /* -- send dept mnemonics, tax modifier, product price --*/
        PrtVLAmtPluMnem(pItem, pTran->TranModeQual.fsModeStatus);

        PrtVLTrail(pTran);                      /* send trailer lines */

    }

    /* -- in case of 21 char printer -- */

    if ( usPrtVLColumn == PRT_VL_21 ) {

        PrtVSDeptPlu(pTran, pItem);

    }

    PrtEndValidation(PMG_PRT_RCT_JNL);          /* end validation */

}

/*
*===========================================================================
** Format  : SHORT    PrtDeptPLU_SP(TRANINFORMATION  *pTran, ITEMSALES *pItem);
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMSALES        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints dept sales ( slip ) .
*===========================================================================
*/
VOID PrtDeptPLU_SP(TRANINFORMATION  *pTran, ITEMSALES *pItem)
{
	TCHAR       aszSPPrintBuff[30][PRT_SPCOLUMN + 1] = {0}; /* print data save area */
    TCHAR       aszPLUNo[PLU_MAX_DIGIT + 1] = {0};
    USHORT      usSlipLine = 0;            /* number of lines to be printed */
    USHORT      usSaveLine;                /* save slip lines to be added */
    USHORT      i;
    DCURRENCY   lPrice;
    LONG        lQTY;
	SHORT       numCounter; //US Customs

//Saratoga Functionality (US Customs cwunn 4/25/03)
	if(!(CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2))) { //if US Customs #/type bit turned off (turned on is lower down)
		/* -- set void mnemonic and number -- */
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usSlipLine += PrtSPVoidMnemNumber(aszSPPrintBuff[0], pItem->fbModifier, pItem->usReasonCode, TRN_SKUNO_ADR, pItem->aszNumber[numCounter]);
			}
		} else {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop printing
					break;
				}//print all #/Type entries that are stored
				usSlipLine += PrtSPVoidNumber(aszSPPrintBuff[0], pItem->fbModifier, pItem->usReasonCode, pItem->aszNumber[numCounter]);
			}
		}
	}
//End Saratoga Functionality

    /* -- set seat mnemonic and number -- */
    if (fsPrtStatus & PRT_TAKETOKIT) {  /* take to kitchen status */
        usSlipLine += PrtSPSeatNo(aszSPPrintBuff[0], pItem->uchSeatNo);    /* seat no. */
    }

    /* 2172 */
    if ((pItem->uchMinorClass == CLASS_PLU) || (pItem->uchMinorClass == CLASS_PLUITEMDISC)) {
        if ( pItem->fsPrintStatus & PRT_VALIDATION ) { /* validation print */
            if (pItem->fsLabelStatus & ITM_CONTROL_NOTINFILE)
            {
                RflConvertPLU(aszPLUNo, pItem->auchPLUNo);
                usSlipLine += PrtSPPLUBuild(aszSPPrintBuff[usSlipLine], TRN_PLUBLD_ADR, aszPLUNo, pItem->usDeptNo);
            } else {
                if ( CliParaMDCCheck(MDC_PLU2_ADR, ODD_MDC_BIT2) ) {
                    RflConvertPLU(aszPLUNo, pItem->auchPLUNo);
                    usSlipLine += PrtSPPLUNo(aszSPPrintBuff[usSlipLine], aszPLUNo);
                }
            }
        } else {
            if ( CliParaMDCCheck(MDC_PLU2_ADR, ODD_MDC_BIT2) ) {
                RflConvertPLU(aszPLUNo, pItem->auchPLUNo);
                usSlipLine += PrtSPPLUNo(aszSPPrintBuff[usSlipLine], aszPLUNo);
            }
        }
    }
    /* -- scalable ? -- */
    if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
        /* -- set tax modifier mnemonic, weight and unit price -- */
        usSlipLine += PrtSPWeight(aszSPPrintBuff[usSlipLine], pTran->TranModeQual.fsModeStatus, pTran->TranCurQual.flPrintStatus, pItem);
        /* -- manual weight input -- */
        if (pItem->fsPrintStatus & PRT_MANUAL_WEIGHT) {
            /* -- set unit price symbol and price -- */
            usSlipLine += PrtSPMnlWeight(aszSPPrintBuff[usSlipLine], pItem->lUnitPrice);
        }
    } else {
        /* -- set tax modifier mnemonic, quantity and unit price -- */
        usSlipLine += PrtSPQty(aszSPPrintBuff[usSlipLine], pTran, pItem);
    }

    /* -- set mnemonics (adjective, PLU, print mod., condiment) -- */
    usSlipLine += PrtSPItems(aszSPPrintBuff[usSlipLine], pTran, pItem);
    if ((pItem->fsPrintStatus & PRT_VALIDATION ) == 0) { /* slip print */
        if (pItem->usLinkNo) {
            memset(aszPLUNo, 0, sizeof(aszPLUNo));
            usSlipLine += PrtSPVoidNumber(aszSPPrintBuff[usSlipLine], pItem->fbModifier, pItem->usReasonCode, aszPLUNo);
            if ( CliParaMDCCheck(MDC_PLU2_ADR, ODD_MDC_BIT2) ) {
                RflConvertPLU(aszPLUNo, pItem->Condiment[0].auchPLUNo);
                usSlipLine += PrtSPPLUNo(aszSPPrintBuff[usSlipLine], aszPLUNo);
            }

            if ((pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) == 0) {
                usSlipLine += PrtSPLinkQty(aszSPPrintBuff[usSlipLine], pTran, pItem);
            }

            lPrice = pItem->Condiment[0].lUnitPrice;
            /* set qty by void consolidation */
            if (PrtChkVoidConsolidation(pTran, pItem)) {
                lQTY = (LONG)pItem->sCouponQTY;
            } else {
                lQTY = pItem->lQTY / PLU_BASE_UNIT;
            }
            if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
                lQTY = (pItem->lQTY >= 0 ? 1L : -1L);
            }

            lPrice *= lQTY;
            usSlipLine += PrtSPLinkPLU(aszSPPrintBuff[usSlipLine], FALSE, pItem->Condiment[0].uchAdjective, pItem->Condiment[0].aszMnemonic, lPrice);
        }
    }
//US Customs Functionality (US Customs cwunn 4/25/03)
	if(CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)) { //if US Customs #/type bit turned on (turned off is above)
		/* -- set void mnemonic and number -- */
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usSlipLine += PrtSPVoidMnemNumber(aszSPPrintBuff[0], pItem->fbModifier, pItem->usReasonCode, TRN_SKUNO_ADR, pItem->aszNumber[numCounter]);
			}
		} else {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop printing
					break;
				}//print all #/Type entries that are stored
				usSlipLine += PrtSPVoidNumber(aszSPPrintBuff[0], pItem->fbModifier, pItem->usReasonCode, pItem->aszNumber[numCounter]);
			}
		}
	}
//End US Customs Functionality

    /* -- check if paper change is necessary or not -- */
    usSaveLine = PrtCheckLine(usSlipLine, pTran);

    /* -- print all data in the buffer -- */
    for (i = 0; i < usSlipLine; i++) {
        PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);
    }

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;
}

/*
*===========================================================================
** Format  : VOID    PrtDflDeptPLU(TRANINFORMATION  *pTran, ITEMSALES *pItem);
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMSALES        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays dept sales.
*===========================================================================
*/
VOID PrtDflDeptPLU(TRANINFORMATION  *pTran, ITEMSALES  *pItem)
{

	TCHAR       aszDflBuff[40][PRT_DFL_LINE + 1] = {0}; /* display data save area */
    TCHAR       aszPLUNo[PLU_MAX_DIGIT + 1] = {0};
    USHORT      usLineNo;                       /* number of lines to be displayed */
    USHORT      usOffset = 0;
    USHORT      i;
    DCURRENCY   lPrice;
    LONG        lQTY;
	SHORT       numCounter; //US Customs

    /* --- if this frame is 1st frame, display customer name --- */
    PrtDflCustHeader( pTran );

    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);

    /* -- set item data -- */
    usLineNo += PrtDflVoid(aszDflBuff[usLineNo], pItem->fbModifier, pItem->usReasonCode);

//Saratoga Functionality (US Customs cwunn 4/25/03)
	if(!(CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2))) { //if US Customs #/type bit turned off (turned on is lower down)
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usLineNo += PrtDflMnemNumber(aszDflBuff[usLineNo], TRN_SKUNO_ADR, pItem->aszNumber[numCounter]);
			}
		} else {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber[numCounter]);
			}
		}
	}
//End Saratoga Functionality

    usLineNo += PrtDflTaxMod(aszDflBuff[usLineNo],
                pTran->TranModeQual.fsModeStatus, pItem->fbModifier);

    if (pItem->uchSeatNo) {

        usLineNo += PrtDflSeatNo(aszDflBuff[usLineNo], pItem);  /* seat no */
    }

    /* 2172 */
    if ((pItem->uchMinorClass == CLASS_PLU) ||
        (pItem->uchMinorClass == CLASS_PLUITEMDISC)) {

        if ( CliParaMDCCheck(MDC_PLU2_ADR, ODD_MDC_BIT2) ) {
            RflConvertPLU(aszPLUNo, pItem->auchPLUNo);
            usLineNo += PrtDflPLUNo(aszDflBuff[usLineNo], aszPLUNo);
        }
    }
    if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {

        usLineNo += PrtDflWeight(aszDflBuff[usLineNo], pTran, pItem);              /* weight price */

    }  else {

        usLineNo += PrtDflQty(aszDflBuff[usLineNo], pItem);                        /* qty price */
    }

    usLineNo += PrtDflItems(aszDflBuff[usLineNo], pItem);

    if (pItem->usLinkNo) {

        usLineNo += PrtDflVoid(aszDflBuff[usLineNo], pItem->fbModifier, pItem->usReasonCode);

        usLineNo += PrtDflTaxMod(aszDflBuff[usLineNo],
                    pTran->TranModeQual.fsModeStatus, pItem->fbModifier);

        if ( CliParaMDCCheck(MDC_PLU2_ADR, ODD_MDC_BIT2) ) {
            RflConvertPLU(aszPLUNo, pItem->Condiment[0].auchPLUNo);
            usLineNo += PrtDflPLUNo(aszDflBuff[usLineNo], aszPLUNo);
        }

        if ((pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) == 0) {

            usLineNo += PrtDflLinkQty(aszDflBuff[usLineNo], pItem);                        /* qty price */
        }

        lPrice = pItem->Condiment[0].lUnitPrice;

        lQTY = pItem->lQTY / PLU_BASE_UNIT;
        if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
            lQTY = (pItem->lQTY >= 0 ? 1L : -1L);
        }

        lPrice *= lQTY;

        usLineNo += PrtDflLinkPLU(aszDflBuff[usLineNo], FALSE, pItem->Condiment[0].uchAdjective, pItem->Condiment[0].aszMnemonic, lPrice);

    }

//US Customs Functionality (US Customs cwunn 4/25/03)
	if(CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)) { //if US Customs #/type bit turned on (turned off is above)
			if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usLineNo += PrtDflMnemNumber(aszDflBuff[usLineNo], TRN_SKUNO_ADR, pItem->aszNumber[numCounter]);
			}
		} else {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber[numCounter]);
			}
		}
	}
//End US Customs Functionality

    /* -- set destination CRT -- */
    PrtDflSetCRTNo(pTran, pItem);

    /* -- check void status -- */
    PrtDflCheckVoid(pItem->fbModifier);

    /* -- set display data in the buffer -- */
    PrtDflIType(usLineNo, DFL_SALES);

    for ( i = 0; i < usLineNo; i++ ) {
        PrtDflSetData(aszDflBuff[i], &usOffset);
        if (aszDflBuff[i][PRT_DFL_LINE] != '\0') {
            i++;
        }
    }

    /* -- send display data -- */
    PrtDflSend();
}

/*
*===========================================================================
** Format  : VOID    PrtDflDeptPLU(TRANINFORMATION  *pTran, ITEMSALES *pItem);
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMSALES        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays dept sales.
*===========================================================================
*/
USHORT PrtDflDeptPLUForm(TRANINFORMATION  *pTran, ITEMSALES  *pItem, TCHAR *puchBuffer)
{

    TCHAR  aszDflBuff[34][PRT_DFL_LINE + 1];/* display data save area */
    USHORT  usLineNo=0, i, j;                       /* number of lines to be displayed */
    TCHAR  aszPLUNo[PLU_MAX_DIGIT + 1];
	SHORT numCounter; //US Customs

    memset(aszDflBuff, '\0', sizeof(aszDflBuff));
	memset(aszPLUNo, 0x00, sizeof(aszPLUNo));

#if 0
    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);
#endif
    /* -- set item data -- */
    usLineNo += PrtDflVoid(aszDflBuff[usLineNo], pItem->fbModifier, pItem->usReasonCode);

//Saratoga Functionality (US Customs cwunn 4/25/03)
	if(!CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)) { //if US Customs #/type bit turned off (turned on is lower down)
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usLineNo += PrtDflMnemNumber(aszDflBuff[usLineNo], TRN_SKUNO_ADR, pItem->aszNumber[numCounter]);
			}
		} else {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/02
				if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber[numCounter]);
			}
		}
	}
//End Saratoga Functionality

    usLineNo += PrtDflTaxMod(aszDflBuff[usLineNo],
                pTran->TranModeQual.fsModeStatus, pItem->fbModifier);

    if (pItem->uchSeatNo) {

        usLineNo += PrtDflSeatNo(aszDflBuff[usLineNo], pItem);  /* seat no */
    }

    /* 2172 */
    if ((pItem->uchMinorClass == CLASS_PLU) ||
        (pItem->uchMinorClass == CLASS_PLUITEMDISC)) {

        if ( CliParaMDCCheck(MDC_PLU2_ADR, ODD_MDC_BIT2) ) {
            RflConvertPLU(aszPLUNo, pItem->auchPLUNo);
            usLineNo += PrtDflPLUNo(aszDflBuff[usLineNo], aszPLUNo);
        }
    }

    if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
        usLineNo += PrtDflWeight(aszDflBuff[usLineNo], pTran, pItem);              /* weight price */
    }  else {
        usLineNo += PrtDflQty(aszDflBuff[usLineNo], pItem);                        /* qty price */
    }

#if 1
    for (i=0; i<usLineNo; i++) {
	    for (j=0; j<(PRT_DFL_LINE+1); j++) {
        	if (aszDflBuff[i][j] == _T('\0')) {

        		aszDflBuff[i][j] = _T(' ');
			}
        }
	}
    for (i=0; i<usLineNo; i++) {

        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }
#endif
    usLineNo += PrtDflItemsEx(aszDflBuff[usLineNo], pItem);		/* for enhanced KDS */
    /* usLineNo += PrtDflItems(aszDflBuff[usLineNo], pItem); */

//US Customs Functionality (US Customs cwunn 4/25/03)
	if(CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)) { //if US Customs #/type bit turned on (turned off is above)
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usLineNo += PrtDflMnemNumber(aszDflBuff[usLineNo], TRN_SKUNO_ADR, pItem->aszNumber[numCounter]);
			}
		} else {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/02
				if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber[numCounter]);
			}
		}
	}
//End US Customs Functionality

#if 1
    aszDflBuff[usLineNo-1][PRT_DFL_LINE] = PRT_RETURN;
#else
    for (i=0; i<usLineNo; i++) {

        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }
#endif
    _tcsncpy(puchBuffer, aszDflBuff[0], usLineNo*(PRT_DFL_LINE+1));
    //memcpy(puchBuffer, aszDflBuff, usLineNo*(PRT_DFL_LINE+1));

    return usLineNo;
}

/*
*===========================================================================
** Format  : VOID    PrtDflDeptPLU(TRANINFORMATION  *pTran, ITEMSALES *pItem);
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMSALES        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays dept sales.
*===========================================================================
*/
USHORT PrtDflPrtModForm(TRANINFORMATION  *pTran, UIFREGSALES *pUifRegSales, TCHAR *puchBuffer)
{

    TCHAR  aszDflBuff[33][PRT_DFL_LINE + 1];/* display data save area */
    USHORT  usLineNo=0, i;                       /* number of lines to be displayed */

    memset(aszDflBuff, '\0', sizeof(aszDflBuff));

#if 0
    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);
#endif
    /* -- set item data -- */
    usLineNo += PrtDflVoid(aszDflBuff[usLineNo], pUifRegSales->fbModifier, 0);

    usLineNo += PrtDflPrtMod(aszDflBuff[usLineNo], pUifRegSales->uchPrintModifier, 0L);

    for (i=0; i<usLineNo; i++) {

        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }
    _tcsncpy(puchBuffer, aszDflBuff[0], usLineNo*(PRT_DFL_LINE+1));
    //memcpy(puchBuffer, aszDflBuff, usLineNo*(PRT_DFL_LINE+1));

    return usLineNo;
}

/****** End of Source ******/
