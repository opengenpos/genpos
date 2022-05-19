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
* Program Name: PrRModDT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               PrtModDisc() : print modifierd discount
*               PrtModDisc_TH() : print modified discount. (thermal)
*               PrtModDisc_EJ() : print modified discount. (electric journal)
*               PrtModDisc_SP() : print modifierd discount. (slip)
*
*               PrtDflModDisc() : display modifierd discount
*               PrtDflSetModDisc() : display modifierd discount for set menu
*               PrtDflModDiscForm() : display modifierd discount
*               PrtDflSetModDiscForm() : display modifierd discount for set menu
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
* Jun-11-93 : 01.00.12 :  R.Itoh    : initial
* Jun-29-93 : 01.00.12 :  K.You     : mod. PrtSetModDisc_TH, PrtSetModDisc_EJ for US enhance.
* Jul-10-93 : 01.00.12 :  R.Itoh    : add PrtDflModDisc(), PrtDflSetModDisc()
* Oct-29-93 : 02.00.02 :  K.You     : del. PrtDflModDisc(), PrtDflSetModDisc()
* Apr-10-94 : 00.00.04 :  K.You     : add validation slip print feature.(mod. PrtModDisc())
* Jan-26-95 : 03.00.00 :  M.Ozawa   : recover PrtDflModDisc(), PrtDflSetModDisc()
* Mar-02-95 : 03.00.00 : T.Nakahata : Add Promotional PLU with Condiment
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
#include <pmg.h>
#include <dfl.h>
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
            return;
        }
    }

    if ( fsPrtPrintPort & PRT_SLIP ) {        /* slip print */
        PrtModDisc_SP(pTran, pItem);
    }

    if ( fsPrtPrintPort & PRT_RECEIPT ) {     /* thermal print */
        if (( pItem->uchMinorClass == CLASS_DEPTMODDISC) || ( pItem->uchMinorClass == CLASS_PLUMODDISC )) {
            PrtDeptPLUModDisc_TH(pTran, pItem);
        } else {                            /* set menu, OEP item */
            PrtSetModDisc_TH(pTran, pItem);
        }
    }

    if ( fsPrtPrintPort & PRT_JOURNAL ) {     /* eelectric journal */
        if (( pItem->uchMinorClass == CLASS_DEPTMODDISC) || ( pItem->uchMinorClass == CLASS_PLUMODDISC )) {
            PrtDeptPLUModDisc_EJ(pTran, pItem);
        } else {
            PrtSetModDisc_EJ(pTran, pItem);
        }
    }
}

/*
*===========================================================================
** Format  : VOID PrtDeptPLUModDisc_TH(TRANINFORMATION *pTran, ITEMSALES *pItem);
*
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMSALES        *pItem     -Item Data address
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints dept & PLU 's modified discount. (thermal)
*===========================================================================
*/
VOID   PrtDeptPLUModDisc_TH(TRANINFORMATION *pTran, ITEMSALES *pItem)
{
	TCHAR      aszPLUNo[PLU_MAX_DIGIT + 1] = {0};
    DCURRENCY  lPrice;
    LONG       lQTY;
	SHORT      numCounter; //US Counter

    PrtTHHead(pTran);                           /* print header if necessary */

    PrtTHVoid(pItem->fbModifier, pItem->usReasonCode);               /* void line */

//Preserve Saratoga operation in US Customs (1.4.2.2)
	if(!CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)){//US Customs off by MDC 21B (on is below)
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				PrtTHMnemNumber(TRN_SKUNO_ADR, pItem->aszNumber[numCounter]);              /* number line */
			}
		} else {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				PrtTHNumber(pItem->aszNumber[numCounter]);              /* number line */
			}
		}
	}
//End Saratoga Operation

    PrtTHMnem(TRN_MODID_ADR, PRT_SINGLE);       /* mod-disc. mnem. */

    if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
        PrtTHWeight(pTran, pItem);              /* tax, weight, price */
    }  else {
        PrtTHQty(pTran, pItem);                 /* tax, qty, price */
    }


    /* 2172 */
    if (pItem->uchMinorClass == CLASS_PLUMODDISC) {
        if ( CliParaMDCCheck(MDC_PLU2_ADR, ODD_MDC_BIT2) ) {
            RflConvertPLU(aszPLUNo, pItem->auchPLUNo);
            PrtTHPLUNo(aszPLUNo);
        }
    }

    PrtTHItems(pTran, pItem);                          /* mnemonics, productprice */

    if (pItem->usLinkNo) {

        PrtTHVoid(pItem->fbModifier, pItem->usReasonCode);               /* void line */

        if ( CliParaMDCCheck(MDC_PLU2_ADR, ODD_MDC_BIT2) ) {
            RflConvertPLU(aszPLUNo, pItem->Condiment[0].auchPLUNo);
            PrtTHPLUNo(aszPLUNo);
        }

        if ((pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) == 0) {
            PrtTHLinkQty(pTran, pItem);                 /* qty price */
        }

        lPrice = pItem->Condiment[0].lUnitPrice;

        lQTY = pItem->lQTY / PLU_BASE_UNIT;
        if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
            lQTY = (pItem->lQTY >= 0 ? 1L : -1L);
        }

        lPrice *= lQTY;

        PrtTHLinkPLU(TRUE, pItem->Condiment[0].uchAdjective, pItem->Condiment[0].aszMnemonic, lPrice);

    }
//US Customs operation in US Customs (1.4.2.2)
	if(CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)){//US Customs on by MDC 21B (off is above)
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				PrtTHMnemNumber(TRN_SKUNO_ADR, pItem->aszNumber[numCounter]);              /* number line */
			}
		} else {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				PrtTHNumber(pItem->aszNumber[numCounter]);              /* number line */
			}
		}
	}
//End US Customs Operation

    PrtTHPerDisc(0, pItem->uchRate, pItem->lDiscountAmount);
                                                /* disc. rate and disc. amount */

}

/*
*===========================================================================
** Format  : VOID PrtDeptPLUModDisc_EJ(TRANINFORMATION *pTran, ITEMSALES *pItem);
*
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMSALES        *pItem     -Item Data address
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints dept & PLU 's modified discount. (electric journal)
*===========================================================================
*/
VOID   PrtDeptPLUModDisc_EJ(TRANINFORMATION *pTran, ITEMSALES *pItem)
{
	TCHAR       aszPLUNo[PLU_MAX_DIGIT + 1] = {0};
    DCURRENCY   lPrice;
    LONG        lQTY;
	SHORT       numCounter; //US Customs

    PrtEJVoid(pItem->fbModifier, pItem->usReasonCode);               /* void line */

//Preserve Saratoga operation in US Customs (1.4.2.2)
	if(!CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)){//US Customs off by MDC 21B (on is below)
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				PrtEJMnemNumber(TRN_SKUNO_ADR, pItem->aszNumber[numCounter]);              /* number line */
			}
		} else {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				PrtEJNumber(pItem->aszNumber[numCounter]);              /* number line */
			}
		}
	}
//End Saratoga Operation

    PrtEJTaxMod((pTran->TranModeQual).fsModeStatus, pItem->fbModifier);

    PrtEJMnem(TRN_MODID_ADR, PRT_SINGLE);       /* mod-disc. mnem. */

    /* -- if Building operation Mnemonics 2172   -- */
    if (pItem->uchMinorClass == CLASS_PLUMODDISC) {

        RflConvertPLU(aszPLUNo, pItem->auchPLUNo);
        if (pItem->fsLabelStatus & ITM_CONTROL_NOTINFILE)
        {
            PrtEJPLUBuild(TRN_PLUBLD_ADR, aszPLUNo, pItem->usDeptNo);
        } else {
            PrtEJPLUNo(aszPLUNo);
        }
    }
    if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
        PrtEJWeight(pTran, pItem);                     /* weight price */
    }  else {
        PrtEJQty(pItem);                        /* qty price */
    }

    PrtEJItems(pItem);                          /* mnemonics, productprice */

    if (pItem->usLinkNo) {

        PrtEJVoid(pItem->fbModifier, pItem->usReasonCode);               /* void line */

        RflConvertPLU(aszPLUNo, pItem->Condiment[0].auchPLUNo);
        PrtEJPLUNo(aszPLUNo);

        if ((pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) == 0) {

            PrtEJLinkQty(pItem);                 /* qty price */
        }

        lPrice = pItem->Condiment[0].lUnitPrice;

        lQTY = pItem->lQTY / PLU_BASE_UNIT;
        if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
            lQTY = (pItem->lQTY >= 0 ? 1L : -1L);
        }

        lPrice *= lQTY;
        PrtEJLinkPLU(TRUE, pItem->Condiment[0].uchAdjective, pItem->Condiment[0].aszMnemonic, lPrice);
    }

// US Customs operation in US Customs (1.4.2.2)
	if(CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)){//US Customs on by MDC 21B (off is above)
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				PrtEJMnemNumber(TRN_SKUNO_ADR, pItem->aszNumber[numCounter]);              /* number line */
			}
		} else {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				PrtEJNumber(pItem->aszNumber[numCounter]);              /* number line */
			}
		}
	}
//End US Customs Operation

    PrtEJPerDisc(0, pItem->uchRate, pItem->lDiscountAmount);   /* disc. rate and disc. amount */

}

/*
*===========================================================================
** Format  : VOID PrtSetModDisc_TH(TRANINFORMATION *pTran, ITEMSALES *pItem);
*
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMSALES        *pItem     -Item Data address
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints set sale's modified discount. (thermal)
*===========================================================================
*/
VOID   PrtSetModDisc_TH(TRANINFORMATION *pTran, ITEMSALES *pItem)
{
    USHORT i;
    TCHAR  auchDummy[NUM_PLU_LEN] = {0};

    PrtDeptPLUModDisc_TH(pTran, pItem);

    /* -- send child plu? -- */
    if ( (pTran->TranCurQual.flPrintStatus) & CURQUAL_SETMENU ) {

        for ( i = 0; i < pItem->uchChildNo; i++ ) {

            /* check child PLU exist or not */
            if (_tcsncmp(pItem->Condiment[i].auchPLUNo, auchDummy, NUM_PLU_LEN) == 0) {
            /* if (!(pItem->Condiment[i].usPLUNo)) { */
                continue;
            }

            PrtTHChild(pItem->Condiment[i].uchAdjective,
                       pItem->Condiment[i].aszMnemonic);

        }

    }
}

/*
*===========================================================================
** Format  : VOID PrtSetModDisc_EJ(TRANINFORMATION *pTran, ITEMSALES *pItem);
*
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMSALES        *pItem     -Item Data address
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints set sale's modified discount. (electric journal)
*===========================================================================
*/
VOID   PrtSetModDisc_EJ(TRANINFORMATION *pTran, ITEMSALES *pItem)
{
    USHORT i;
    TCHAR  auchDummy[NUM_PLU_LEN] = {0};

    PrtDeptPLUModDisc_EJ(pTran, pItem);

    /* -- send child plu? -- */
    if ( (pTran->TranCurQual.flPrintStatus) & CURQUAL_SETMENU ) {

        for ( i = 0; i < pItem->uchChildNo; i++ ) {

            /* check child PLU exist or not */
            if (_tcsncmp(pItem->Condiment[i].auchPLUNo, auchDummy, NUM_PLU_LEN) == 0) {
            /* if (!(pItem->Condiment[i].usPLUNo)) { */
                continue;
            }

            PrtEJChild(pItem->Condiment[i].uchAdjective,
                       pItem->Condiment[i].aszMnemonic);

        }

    }
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
    TCHAR       aszSPPrintBuff[21][PRT_SPCOLUMN + 1]; /* print data save area */
    USHORT      usSlipLine = 0;            /* number of lines to be printed */
    USHORT      usSaveLine;                /* save slip lines to be added */
    USHORT      i;
    TCHAR       aszPLUNo[PLU_MAX_DIGIT + 1];
    DCURRENCY   lPrice;
    LONG        lQTY;
	SHORT       numCounter; //US Customs

    /* initialize the buffer */
	memset(aszPLUNo, 0x00, sizeof(aszPLUNo));
    memset(aszSPPrintBuff[0], '\0', sizeof(aszSPPrintBuff));

//Preserve Saratoga operation in US Customs (1.4.2.2)
	if(!CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)){//US Customs off by MDC 21B (on is below)
    /* -- set void mnemonic and number -- */
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usSlipLine += PrtSPVoidMnemNumber(aszSPPrintBuff[0], pItem->fbModifier, pItem->usReasonCode, TRN_SKUNO_ADR, pItem->aszNumber[numCounter]);
			}
		} else {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){  //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usSlipLine += PrtSPVoidNumber(aszSPPrintBuff[0], pItem->fbModifier, pItem->usReasonCode, pItem->aszNumber[numCounter]);
			}
		}
	}
//End Saratoga Operation

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
            usSlipLine += PrtSPVoidNumber(aszSPPrintBuff[usSlipLine], pItem->fbModifier, pItem->usReasonCode, aszPLUNo);

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

//US Customs operation in US Customs (1.4.2.2)
	if(CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)){//US Customs on by MDC 21B (off is above)
    /* -- set void mnemonic and number -- */
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usSlipLine += PrtSPVoidMnemNumber(aszSPPrintBuff[0], pItem->fbModifier, pItem->usReasonCode, TRN_SKUNO_ADR, pItem->aszNumber[numCounter]);
			}
		} else {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){  //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usSlipLine += PrtSPVoidNumber(aszSPPrintBuff[0], pItem->fbModifier, pItem->usReasonCode, pItem->aszNumber[numCounter]);
			}
		}
	}
//End US Customs Operation

    /* -- print all data in the buffer -- */
    for (i = 0; i < usSlipLine; i++) {
/*  --- fix a glitch (05/15/2001)
        PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN); */
        PrtPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);
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
    TCHAR      aszDflBuff[31][PRT_DFL_LINE + 1];/* display data save area */
    USHORT     usLineNo;                       /* number of lines to be displayed */
    USHORT     usOffset = 0;
    USHORT     i;
    TCHAR      aszPLUNo[PLU_MAX_DIGIT + 1];
    DCURRENCY  lPrice;
    LONG       lQTY;
	SHORT      numCounter;	//US Customs

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

//Preserve Saratoga operation in US Customs (1.4.2.2)
	if(!CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)){//US Customs off by MDC 21B (on is below)
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usLineNo += PrtDflMnemNumber(aszDflBuff[usLineNo], TRN_SKUNO_ADR, pItem->aszNumber[numCounter]);
			}
		} else {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber[numCounter]);
			}
		}
	}
//End Saratoga Operation

    usLineNo += PrtDflTaxMod(aszDflBuff[usLineNo],
                pTran->TranModeQual.fsModeStatus, pItem->fbModifier);

    usLineNo += PrtDflMnem(aszDflBuff[usLineNo], TRN_MODID_ADR, PRT_SINGLE);       /* mod-disc. mnem. */

    /* 2172 */
    if (pItem->uchMinorClass == CLASS_PLUMODDISC) {

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
        usLineNo += PrtDflLinkPLU(aszDflBuff[usLineNo], TRUE, pItem->Condiment[0].uchAdjective, pItem->Condiment[0].aszMnemonic, lPrice);

    }

    usLineNo += PrtDflPerDisc(aszDflBuff[usLineNo], 0, pItem->uchRate, pItem->lDiscountAmount);

    /* -- set destination CRT -- */
    PrtDflIf.Dfl.DflHead.auchCRTNo[0] = 0x30;
    PrtDflIf.Dfl.DflHead.auchCRTNo[1] = 0x30;

    /* -- check void status -- */
    PrtDflCheckVoid(pItem->fbModifier);

//US Customs operation in US Customs (1.4.2.2)
	if(CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)){//US Customs on by MDC 21B (off is above)
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usLineNo += PrtDflMnemNumber(aszDflBuff[usLineNo], TRN_SKUNO_ADR, pItem->aszNumber[numCounter]);
			}
		} else {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber[numCounter]);
			}
		}
	}
//End US Customs Operation

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
    TCHAR  aszDflBuff[31][PRT_DFL_LINE + 1];/* display data save area */
    USHORT  usLineNo;                       /* number of lines to be displayed */
    USHORT  usOffset = 0;
    USHORT  i;
    TCHAR  auchDummy[NUM_PLU_LEN] = {0};
    TCHAR  aszPLUNo[PLU_MAX_DIGIT + 1];
	SHORT numCounter;	//US Customs

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

//Preserve Saratoga operation in US Customs (1.4.2.2)
	if(!CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)){//US Customs off by MDC 21B (on is below)
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usLineNo += PrtDflMnemNumber(aszDflBuff[usLineNo], TRN_SKUNO_ADR, pItem->aszNumber[numCounter]);
			}
		} else {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber[numCounter]);
			}
		}
	}
//End Saratoga Operation

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

    /* -- set destination CRT -- */
    PrtDflIf.Dfl.DflHead.auchCRTNo[0] = 0x30;
    PrtDflIf.Dfl.DflHead.auchCRTNo[1] = 0x30;

    /* -- check void status -- */
    PrtDflCheckVoid(pItem->fbModifier);

//US Customs operation in US Customs (1.4.2.2)
	if(CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)){//US Customs on by MDC 21B (off is above)
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usLineNo += PrtDflMnemNumber(aszDflBuff[usLineNo], TRN_SKUNO_ADR, pItem->aszNumber[numCounter]);
			}
		} else {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber[numCounter]);
			}
		}
	}
//End US Customs Operation

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
	SHORT numCounter;	//US Customs

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

//Preserve Saratoga operation in US Customs (1.4.2.2)
	if(!CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)){//US Customs off by MDC 21B (on is below)
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usLineNo += PrtDflMnemNumber(aszDflBuff[usLineNo], TRN_SKUNO_ADR, pItem->aszNumber[numCounter]);
			}
		} else {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber[numCounter]);
			}
		}
	}
//End Saratoga Operation

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

//US Customs operation in US Customs (1.4.2.2)
	if(CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)){//US Customs on by MDC 21B (off is above)
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usLineNo += PrtDflMnemNumber(aszDflBuff[usLineNo], TRN_SKUNO_ADR, pItem->aszNumber[numCounter]);
			}
		} else {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber[numCounter]);
			}
		}
	}
//End US Customs Operation

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
	SHORT numCounter;	//US Customs


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

//Preserve Saratoga operation in US Customs (1.4.2.2)
	if(!CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)){//US Customs off by MDC 21B (on is below)
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber[numCounter]);
			}
		} else {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber[numCounter]);
			}
		}
	}
//End Saratoga Operation

    usLineNo += PrtDflTaxMod(aszDflBuff[usLineNo],
                pTran->TranModeQual.fsModeStatus, pItem->fbModifier);

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

//US Customs operation in US Customs (1.4.2.2)
	if(CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)){//US Customs on by MDC 21B (off is above)
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber[numCounter]);
			}
		} else {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber[numCounter]);
			}
		}
	}
//End US CustomsOperation

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
