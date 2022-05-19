/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print  Modifiered discount
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRModD_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               PrtModDisc() : print modifierd discount
*               PrtModDisc_RJ() : print modified discount. (receipt & journal)
*               PrtModDisc_VL() : print modifierd discount. (validation)
*               PrtModDisc_SP() : print modifierd discount. (slip)
*               PrtDflModDisc() : display modifierd discount
*               PrtDflSetModDisc() : display modifierd discount for set menu
*
*               PrtDflModDiscForm() : display modifierd discount
*               PrtDflSetModDiscForm() : display modifierd discount for set menu
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
* Jun-01-92 : 00.00.01 : K.Nakajima :
* Jun-29-93 : 01.00.12 : K.You      : mod. PrtSetModDisc_RJ for US enhance.
* Jul-10-93 : 01.00.12 : R.Itoh     : add PrtDflModDisc(), PrtDflSetModDisc()
* Oct-28-93 : 02.00.02 : K.You      : del. PrtDflModDisc(), PrtDflSetModDisc()
* Apr-10-94 : 00.00.04 :  K.You     : add validation slip print feature.(mod. PrtModDisc())
* Jan-17-95 : 03.00.00 : M.Ozawa    : add PrtDflModDisc(), PrtDflSetModDisc()
* Mar-02-95 : 03.00.00 : T.Nakahata : add set menu with condiment feature
* Jul-21-95 : 03.00.04 : T.Nakahata : display customer name at 1st frame
* Nov-20-95 : 03.01.00 : M.Ozawa    : print seat no.
* Nov-18-99 : 00.00.01 : M.Ozawa    : Enhanced to 2172
* Dec-14-93 : 00.00.01 : M.Ozawa    : Add PrtDflModForm(), PrtDflSetModDiscForm()
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


CONST TCHAR FARCONST  aszPrtRJUnitPrice[] = _T("\t %l$ ");

/**
;============================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;============================================================================
**/
/*
*===========================================================================
** Format  : VOID PrtModDisc(TRANINFORMATION *pTran, ITEMSALES *pItem);
*
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMSALES        *pItem     -Item Data address
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints modifierd discount.
*       Department, PLU, Promotional PLU, Order Entry Prompt
*===========================================================================
*/
VOID PrtModDisc(TRANINFORMATION  *pTran, ITEMSALES  *pItem)
{

    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( pItem->fsPrintStatus & PRT_VALIDATION ) { /* validation print */
        if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) {
            if(PRT_SUCCESS != PrtSPVLInit())
			{
				return;
			}
            PrtSPVLHead(pTran);
            PrtModDisc_SP(pTran, pItem);
            PrtSPVLTrail(pTran);
        } else {
            PrtModDisc_VL(pTran, pItem);
        }
        return ;
    }

    if ( fsPrtPrintPort & PRT_SLIP ) {        /* slip print */
        PrtModDisc_SP(pTran, pItem);
    }

    if (   (fsPrtPrintPort & PRT_RECEIPT)     /* receipt, journal print */
        || (fsPrtPrintPort & PRT_JOURNAL)) {

        if (( pItem->uchMinorClass == CLASS_DEPTMODDISC ) ||
            ( pItem->uchMinorClass == CLASS_PLUMODDISC )) {

            PrtDeptPLUModDisc_RJ(pTran, pItem);

        } else {                                /* set menu, OEP item */

            PrtSetModDisc_RJ(pTran, pItem);
        }

    }
}

/*
*===========================================================================
** Format  : VOID PrtDeptPLUModDisc_RJ(TRANINFORMATION *pTran, ITEMSALES *pItem);
*
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMSALES        *pItem     -Item Data address
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints dept & PLU 's modified discount. (receipt & journal)
*===========================================================================
*/
VOID   PrtDeptPLUModDisc_RJ(TRANINFORMATION *pTran, ITEMSALES *pItem)
{
	TCHAR       aszPLUNo[PLU_MAX_DIGIT + 1] = {0};
    DCURRENCY   lPrice;
    LONG        lQTY;
	SHORT       numCounter; //US Customs

    PrtRJVoid(pItem->fbModifier);               /* void line */

    if (pItem->fbModifier2 & SKU_MODIFIER) {
		for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
			if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop printing
				break;
			}//Print all #/Type entries that are stored
			PrtRJMnemNumber(TRN_SKUNO_ADR, pItem->aszNumber[numCounter]);              /* number line */
		}
    } else {
        for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
			if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop print
				break;
			}//Print all #/Type entries that are stored
			PrtRJNumber(pItem->aszNumber[numCounter]);              /* number line */
		}
    }

    PrtRJTaxMod((pTran->TranModeQual).fsModeStatus, pItem->fbModifier);

    PrtRJMnem(TRN_MODID_ADR, PRT_SINGLE);       /* mod-disc. mnem. */

    /* -- if Building operation Mnemonics 2172   -- */
    if (pItem->uchMinorClass == CLASS_PLUMODDISC) {

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
        PrtRJWeight(pTran, pItem);                     /* weight price */
    }  else {
        PrtRJQty(pTran, pItem);                        /* qty price */
    }

    PrtRItems(pTran, pItem);                          /* mnemonics, productprice */
    PrtJItems(pItem);                          /* mnemonics, productprice */

    if (pItem->usLinkNo) {
        PrtRJVoid(pItem->fbModifier);               /* void line */
        RflConvertPLU(aszPLUNo, pItem->Condiment[0].auchPLUNo);
        PrtJPLUNo(aszPLUNo);
        if ( CliParaMDCCheck(MDC_PLU2_ADR, ODD_MDC_BIT2) ) {
            PrtRPLUNo(aszPLUNo);
        }

        if ((pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) == 0) {
            PrtRJLinkQty(pTran, pItem);                 /* qty price */
        }

        lPrice = pItem->Condiment[0].lUnitPrice;

        lQTY = pItem->lQTY / PLU_BASE_UNIT;
        if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
            lQTY = (pItem->lQTY >= 0 ? 1L : -1L);
        }

        lPrice *= lQTY;
        PrtRJLinkPLU(TRUE, pItem->Condiment[0].uchAdjective, pItem->Condiment[0].aszMnemonic, lPrice);
    }

    PrtRJPerDisc(0, pItem->uchRate, pItem->lDiscountAmount);    /* disc. rate and disc. amount */

}

/*
*===========================================================================
** Format  : VOID PrtSetModDisc_RJ(TRANINFORMATION *pTran, ITEMSALES *pItem);
*
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMSALES        *pItem     -Item Data address
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints set sale's modified discount. (receipt & journal)
*===========================================================================
*/
VOID   PrtSetModDisc_RJ(TRANINFORMATION *pTran, ITEMSALES *pItem)
{
    USHORT i;
    TCHAR  auchDummy[NUM_PLU_LEN] = {0};

    PrtDeptPLUModDisc_RJ(pTran, pItem);

    /* -- send child plu? -- */
    if ( (pTran->TranCurQual.flPrintStatus) & CURQUAL_SETMENU ) {

        for ( i = 0; i < pItem->uchChildNo; i++) {

            /* check condiment PLU exist or not */
            if (_tcsncmp(pItem->Condiment[i].auchPLUNo, auchDummy, NUM_PLU_LEN) == 0) {
            /* if (!(pItem->Condiment[i].usPLUNo)) { */
                continue;
            }

            PrtRJChild(pItem->Condiment[i].uchAdjective,
                       pItem->Condiment[i].aszMnemonic);

        }

    }
}

/*
*===========================================================================
** Format  : VOID PrtModDisc_VL(TRANINFORMATION *pTran, ITEMSALES *pItem);
*
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMSALES        *pItem     -Item Data address
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints modifiered discount. (validation)
*===========================================================================
*/
VOID   PrtModDisc_VL(TRANINFORMATION *pTran, ITEMSALES *pItem)
{
	SHORT numCounter; //US Customs

    PmgBeginValidation(PMG_PRT_RCT_JNL);        /* begin validation */

    /* -- in case of 24 char printer -- */

    if ( usPrtVLColumn == PRT_VL_24 ) {

        PrtVLHead(pTran, pItem->fbModifier);    /* send header lines */

        if (pItem->fbModifier2 & SKU_MODIFIER) {
            for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop print
					break;
				}//Print all #/Type entries that are stored
				PrtVLMnemNumber(TRN_SKUNO_ADR, pItem->aszNumber[numCounter]);          /* send number line */
			}
        } else {
            for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
			if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop displaying
				break;
			}//Print all #/Type entries that are stored
				PrtVLNumber(pItem->aszNumber[numCounter]);          /* send number line */
			}
        }

        PrtVLAmtPerMnem(TRN_MODID_ADR, pItem->uchRate, pTran->TranModeQual.fsModeStatus, pItem->fbModifier, pItem->lDiscountAmount);

        PrtVLTrail(pTran);                      /* send trailer lines */
    }

    /* -- in case of 21 char printer -- */

    if ( usPrtVLColumn == PRT_VL_21 ) {
        PrtVSAmtPerMnem(pTran, pItem->fbModifier, TRN_MODID_ADR, pItem->uchRate, pItem->lDiscountAmount);
    }

    PrtEndValidation(PMG_PRT_RCT_JNL);          /* end validation */

}

/*
*===========================================================================
** Format  : VOID PrtModDisc_SP(TRANINFORMATION *pTran, ITEMSALES *pItem);
*
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMSALES        *pItem     -Item Data address
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints modifierd discount. (slip)
*===========================================================================
*/
VOID PrtModDisc_SP(TRANINFORMATION *pTran, ITEMSALES *pItem)
{
	TCHAR       aszSPPrintBuff[21][PRT_SPCOLUMN + 1] = {0}; /* print data save area */
    TCHAR       aszPLUNo[PLU_MAX_DIGIT + 1] = {0};
    USHORT      usSlipLine = 0;            /* number of lines to be printed */
    USHORT      usSaveLine;                /* save slip lines to be added */
    USHORT      i;
    DCURRENCY   lPrice;
    LONG        lQTY;
	SHORT       numCounter; //US Customs

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
			}//Print all #/Type entries that are stored
			usSlipLine += PrtSPVoidNumber(aszSPPrintBuff[0], pItem->fbModifier, pItem->usReasonCode, pItem->aszNumber[numCounter]);
		}
    }

    /* 2172 */
    if (pItem->uchMinorClass == CLASS_PLUMODDISC) {
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
    /* -- set seat mnemonic and number -- */
    if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
		/* -- Modifierd discount mnemonics -- */
       usSlipLine += PrtSPTranMnem(aszSPPrintBuff[usSlipLine],TRN_MODID_ADR);

		/* --  tax mod. 1-3 mnem., weight, and unit price -- */
        usSlipLine += PrtSPWeight(aszSPPrintBuff[usSlipLine], pTran->TranModeQual.fsModeStatus, pTran->TranCurQual.flPrintStatus, pItem);

        /* -- manual weight input -- */
        if (pItem->fsPrintStatus & PRT_MANUAL_WEIGHT) {
            /* -- set unit price symbol and price -- */
            usSlipLine += PrtSPMnlWeight(aszSPPrintBuff[usSlipLine], pItem->lUnitPrice);
        }
    } else {
		/* -- set modifiered i-disc., tax mod. 1-3 mnem., qty, and unit price -- */
        usSlipLine += PrtSPMnemTaxQty(aszSPPrintBuff[usSlipLine], TRN_MODID_ADR, pTran->TranModeQual.fsModeStatus, pItem);

    }

    /* -- set mnemonics (adjective, PLU, print mod., condiment) -- */
    usSlipLine += PrtSPItems(aszSPPrintBuff[usSlipLine], pTran, pItem);

    if ((pItem->fsPrintStatus & PRT_VALIDATION ) == 0) { /* slip print */
        if (pItem->usLinkNo) {
            memset(aszPLUNo, 0, sizeof(aszPLUNo));
            usSlipLine += PrtSPVoidNumber(aszSPPrintBuff[usSlipLine], pItem->fbModifier, pItem->usReasonCode, NULL);

            if ( CliParaMDCCheck(MDC_PLU2_ADR, ODD_MDC_BIT2) ) {
                RflConvertPLU(aszPLUNo, pItem->Condiment[0].auchPLUNo);
                usSlipLine += PrtSPPLUNo(aszSPPrintBuff[usSlipLine], aszPLUNo);
            }

            if ((pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) == 0) {
                usSlipLine += PrtSPLinkQty(aszSPPrintBuff[usSlipLine], pTran, pItem);
            }

            lPrice = pItem->Condiment[0].lUnitPrice;

            lQTY = pItem->lQTY / PLU_BASE_UNIT;
            if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
                lQTY = (pItem->lQTY >= 0 ? 1L : -1L);
            }

            lPrice *= lQTY;
            usSlipLine += PrtSPLinkPLU(aszSPPrintBuff[usSlipLine], TRUE, pItem->Condiment[0].uchAdjective, pItem->Condiment[0].aszMnemonic, lPrice);
        }
    }

    /* -- set discount rate and discount amount.    -- */
    /* -- "0" means not print transaction mnemonics -- */
    usSlipLine += PrtSPDiscount(aszSPPrintBuff[usSlipLine], 0,
                                pItem->uchRate, pItem->lDiscountAmount);

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
** Format  : VOID PrtDflModDisc(TRANINFORMATION *pTran, ITEMSALES *pItem);
*
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMSALES        *pItem     -Item Data address
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function displays modifierd discount.
*===========================================================================
*/
VOID PrtDflModDisc(TRANINFORMATION  *pTran, ITEMSALES  *pItem)
{
    TCHAR       aszDflBuff[33][PRT_DFL_LINE + 1] = {0};/* display data save area */
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

    if (pItem->fbModifier2 & SKU_MODIFIER) {
        for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
			if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop printing
				break;
			}//print all #/Type entries that are stored
			usLineNo += PrtDflMnemNumber(aszDflBuff[usLineNo], TRN_SKUNO_ADR, pItem->aszNumber[numCounter]);
		}
    } else {
		for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
			if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop printing
				break;
			}//print all #/Type entries that are stored
			usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber[numCounter]);
		}
    }
    usLineNo += PrtDflTaxMod(aszDflBuff[usLineNo], pTran->TranModeQual.fsModeStatus, pItem->fbModifier);
    usLineNo += PrtDflMnem(aszDflBuff[usLineNo], TRN_MODID_ADR, PRT_SINGLE);       /* mod-disc. mnem. */

    /* 2172 */
    if (pItem->uchMinorClass == CLASS_PLUMODDISC) {
        if ( CliParaMDCCheck(MDC_PLU2_ADR, ODD_MDC_BIT2) ) {
            RflConvertPLU(aszPLUNo, pItem->auchPLUNo);
            PrtDflPLUNo(aszDflBuff[usLineNo], aszPLUNo);
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
        usLineNo += PrtDflTaxMod(aszDflBuff[usLineNo], pTran->TranModeQual.fsModeStatus, pItem->fbModifier);
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

    usLineNo += PrtDflPerDisc(aszDflBuff[usLineNo], 0, pItem->uchRate, pItem->lDiscountAmount);

    /* -- set destination CRT -- */
    PrtDflIf.Dfl.DflHead.auchCRTNo[0] = 0x30;
    PrtDflIf.Dfl.DflHead.auchCRTNo[1] = 0x30;

    /* -- check void status -- */
    PrtDflCheckVoid(pItem->fbModifier);

    /* -- set display data in the buffer -- */
    PrtDflIType(usLineNo, DFL_SALES);

    for ( i = 0; i < usLineNo; i++ ) {
        PrtDflSetData(aszDflBuff[i], &usOffset);
        if ( aszDflBuff[i][PRT_DFL_LINE] != '\0' ) {
            i++;
        }
    }

    /* -- send display data -- */
    PrtDflSend();
}
/*
*===========================================================================
** Format  : VOID PrtDflSetModDisc(TRANINFORMATION *pTran, ITEMSALES *pItem);
*
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMSALES        *pItem     -Item Data address
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function displays modifier discount for set menu.
*===========================================================================
*/
VOID PrtDflSetModDisc(TRANINFORMATION  *pTran, ITEMSALES  *pItem)
{
    TCHAR  aszDflBuff[32][PRT_DFL_LINE + 1];/* display data save area */
    USHORT  usLineNo;                       /* number of lines to be displayed */
    USHORT  usOffset = 0;
    USHORT  i;
    TCHAR   auchDummy[NUM_PLU_LEN] = {0};
    TCHAR  aszPLUNo[PLU_MAX_DIGIT + 1];
	SHORT numCounter; //US Customs

	memset(aszPLUNo, 0x00, sizeof(aszPLUNo));
    /* --- if this frame is 1st frame, display customer name --- */

    PrtDflCustHeader( pTran );

    memset(aszDflBuff, '\0', sizeof(aszDflBuff));

    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);

    /* -- set item data -- */
    usLineNo += PrtDflVoid(aszDflBuff[usLineNo], pItem->fbModifier, pItem->usReasonCode);

    if (pItem->fbModifier2 & SKU_MODIFIER) {
        for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
			if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop printing
				break;
			}//print all #/Type entries that are stored
			usLineNo += PrtDflMnemNumber(aszDflBuff[usLineNo], TRN_SKUNO_ADR, pItem->aszNumber[numCounter]);
		}
    } else {
		for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
			if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop printing
				break;
			}//print all #/Type entries that are stored
			usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber[numCounter]);
		}
    }

    usLineNo += PrtDflTaxMod(aszDflBuff[usLineNo],
                pTran->TranModeQual.fsModeStatus, pItem->fbModifier);

    usLineNo += PrtDflMnem(aszDflBuff[usLineNo], TRN_MODID_ADR, PRT_SINGLE);       /* mod-disc. mnem. */

    /* 2172 */
    if ( CliParaMDCCheck(MDC_PLU2_ADR, ODD_MDC_BIT2) ) {
        RflConvertPLU(aszPLUNo, pItem->auchPLUNo);
        usLineNo += PrtDflPLUNo(aszDflBuff[usLineNo], aszPLUNo);
    }
    if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
        usLineNo += PrtDflWeight(aszDflBuff[usLineNo], pTran, pItem);              /* weight price */
    }  else {
        usLineNo += PrtDflQty(aszDflBuff[usLineNo], pItem);                        /* qty price */
    }

    usLineNo += PrtDflItems(aszDflBuff[usLineNo], pItem);

    usLineNo += PrtDflPerDisc(aszDflBuff[usLineNo], 0, pItem->uchRate, pItem->lDiscountAmount);

    /* -- send child plu? -- */
    if ( (pTran->TranCurQual.flPrintStatus) & CURQUAL_SETMENU ) {

        for ( i = 0; i < pItem->uchChildNo; i++) {

            /* check condiment PLU exist or not */
            if (_tcsncmp(pItem->Condiment[i].auchPLUNo, auchDummy, NUM_PLU_LEN) == 0) {
            /* if (!(pItem->Condiment[i].usPLUNo)) { */
                continue;
            }

            usLineNo += PrtDflChild(aszDflBuff[usLineNo], pItem->Condiment[i].uchAdjective,
                                        pItem->Condiment[i].aszMnemonic);

        }

    }

    /* -- set destination CRT -- */
    PrtDflIf.Dfl.DflHead.auchCRTNo[0] = 0x30;
    PrtDflIf.Dfl.DflHead.auchCRTNo[1] = 0x30;

    /* -- check void status -- */
    PrtDflCheckVoid(pItem->fbModifier);

    /* -- set display data in the buffer -- */
    PrtDflIType(usLineNo, DFL_SALES);

    for ( i = 0; i < usLineNo; i++ ) {
        PrtDflSetData(aszDflBuff[i], &usOffset);
        if ( aszDflBuff[i][PRT_DFL_LINE] != '\0' ) {
            i++;
        }
    }

    /* -- send display data -- */
    PrtDflSend();
}

/*
*===========================================================================
** Format  : VOID PrtDflModDisc(TRANINFORMATION *pTran, ITEMSALES *pItem);
*
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMSALES        *pItem     -Item Data address
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function displays modifierd discount.
*===========================================================================
*/
USHORT PrtDflModDiscForm(TRANINFORMATION  *pTran, ITEMSALES  *pItem, TCHAR *puchBuffer)
{
    TCHAR  aszDflBuff[26][PRT_DFL_LINE + 1];/* display data save area */
    USHORT  usLineNo = 0, i;                       /* number of lines to be displayed */
    USHORT  usOffset = 0;
    TCHAR  aszPLUNo[PLU_MAX_DIGIT + 1];
	SHORT numCounter; //US Customs

	memset(aszPLUNo, 0x00, sizeof(aszPLUNo));
    memset(aszDflBuff, '\0', sizeof(aszDflBuff));
#if 0
    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);
#endif
    /* -- set item data -- */
    usLineNo += PrtDflVoid(aszDflBuff[usLineNo], pItem->fbModifier, pItem->usReasonCode);

    if (pItem->fbModifier2 & SKU_MODIFIER) {
		for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
			if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop printing
				break;
			}//print all #/Type entries that are stored
			usLineNo += PrtDflMnemNumber(aszDflBuff[usLineNo], TRN_SKUNO_ADR, pItem->aszNumber[numCounter]);
		}
    } else {
        for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
			if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop printing
				break;
			}//print all #/Type entries that are stored
			usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber[numCounter]);
		}
    }

    usLineNo += PrtDflTaxMod(aszDflBuff[usLineNo],
                pTran->TranModeQual.fsModeStatus, pItem->fbModifier);

    usLineNo += PrtDflMnem(aszDflBuff[usLineNo], TRN_MODID_ADR, PRT_SINGLE);       /* mod-disc. mnem. */

    /* 2172 */
    if (pItem->uchMinorClass == CLASS_PLUMODDISC) {

        if ( CliParaMDCCheck(MDC_PLU2_ADR, ODD_MDC_BIT2) ) {
            RflConvertPLU(aszPLUNo, pItem->auchPLUNo);
            PrtDflPLUNo(aszDflBuff[usLineNo], aszPLUNo);
        }
    }

    if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
        usLineNo += PrtDflWeight(aszDflBuff[usLineNo], pTran, pItem);              /* weight price */
    }  else {
        usLineNo += PrtDflQty(aszDflBuff[usLineNo], pItem);                        /* qty price */
    }

    usLineNo += PrtDflItems(aszDflBuff[usLineNo], pItem);

    usLineNo += PrtDflPerDisc(aszDflBuff[usLineNo], 0, pItem->uchRate, pItem->lDiscountAmount);

    for (i=0; i<usLineNo; i++) {

        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }
    _tcsncpy(puchBuffer, aszDflBuff[0], usLineNo*(PRT_DFL_LINE+1));
    //memcpy(puchBuffer, aszDflBuff, usLineNo*(PRT_DFL_LINE+1));

    return usLineNo;

}

/*
*===========================================================================
** Format  : VOID PrtDflSetModDisc(TRANINFORMATION *pTran, ITEMSALES *pItem);
*
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMSALES        *pItem     -Item Data address
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function displays modifier discount for set menu.
*===========================================================================
*/
USHORT PrtDflSetModDiscForm(TRANINFORMATION  *pTran, ITEMSALES  *pItem, TCHAR *puchBuffer)
{
    TCHAR  aszDflBuff[31][PRT_DFL_LINE + 1];/* display data save area */
    USHORT  usLineNo=0;                       /* number of lines to be displayed */
    USHORT  i;
    TCHAR  auchDummy[NUM_PLU_LEN] = {0};
    TCHAR  aszPLUNo[PLU_MAX_DIGIT + 1];
	SHORT numCounter; //US Customs


	memset(aszPLUNo, 0x00, sizeof(aszPLUNo));
    memset(aszDflBuff, '\0', sizeof(aszDflBuff));
#if 0
    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);
#endif
    /* -- set item data -- */
    usLineNo += PrtDflVoid(aszDflBuff[usLineNo], pItem->fbModifier, pItem->usReasonCode);

    if (pItem->fbModifier2 & SKU_MODIFIER) {
        for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
			if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop printing
				break;
			}//print all #/Type entries that are stored
			usLineNo += PrtDflMnemNumber(aszDflBuff[usLineNo], TRN_SKUNO_ADR, pItem->aszNumber[numCounter]);
		}
    } else {
		for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
			if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop printing
				break;
			}//print all #/Type entries that are stored
			usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber[numCounter]);
		}
    }
    usLineNo += PrtDflTaxMod(aszDflBuff[usLineNo], pTran->TranModeQual.fsModeStatus, pItem->fbModifier);

    usLineNo += PrtDflMnem(aszDflBuff[usLineNo], TRN_MODID_ADR, PRT_SINGLE);       /* mod-disc. mnem. */

    if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
        usLineNo += PrtDflWeight(aszDflBuff[usLineNo], pTran, pItem);              /* weight price */
    }  else {
        usLineNo += PrtDflQty(aszDflBuff[usLineNo], pItem);                        /* qty price */
    }

    /* 2172 */
    if ( CliParaMDCCheck(MDC_PLU2_ADR, ODD_MDC_BIT2) ) {
        RflConvertPLU(aszPLUNo, pItem->auchPLUNo);
        PrtDflPLUNo(aszDflBuff[usLineNo], aszPLUNo);
    }

    usLineNo += PrtDflItems(aszDflBuff[usLineNo], pItem);

    usLineNo += PrtDflPerDisc(aszDflBuff[usLineNo], 0, pItem->uchRate, pItem->lDiscountAmount);

    /* -- send child plu? -- */
    if ( (pTran->TranCurQual.flPrintStatus) & CURQUAL_SETMENU ) {

        for ( i = 0; i < pItem->uchChildNo; i++ ) {

            /* check child PLU exist or not */
            if (_tcsncmp(pItem->Condiment[i].auchPLUNo, auchDummy, NUM_PLU_LEN) == 0) {
            /* if (!(pItem->Condiment[i].usPLUNo)) { */
                continue;
            }

            usLineNo += PrtDflChild(aszDflBuff[usLineNo], pItem->Condiment[i].uchAdjective,
                                        pItem->Condiment[i].aszMnemonic);

        }

    }

    for (i=0; i<usLineNo; i++) {

        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }
    _tcsncpy(puchBuffer, aszDflBuff[0], usLineNo*(PRT_DFL_LINE+1));
    //memcpy(puchBuffer, aszDflBuff, usLineNo*(PRT_DFL_LINE+1));

    return usLineNo;

}

/****** End of Source ******/
