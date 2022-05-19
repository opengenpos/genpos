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
* Title       : Print  set sales
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRSetT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Midium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*           PrtSET() : prints set sales
*           PrtSET_TH() : prints setmenu ( thermal )
*           PrtSETKP_TH() : prints setmenu to "take to KP" stub ( thermal )
*           PrtSET_EJ() : prints setmenu ( electric journal )
*           PrtSET_SP() : prints setmenu ( slip )
*
*           PrtDflSET() : displays set sales
*             PrDflSETChkChildKP()
*             PrDflSETParent()
*             PrDflSETSendAsNotKP()
*             PrDflSETSendNoKPChild()
*             PrDflSETSend()
*           PrtDflSETForm() : displays set sales
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
* Jun-11-93 : 01.00.12 : R.Itoh     : initial
* Jun-29-93 : 01.00.12 : K.You      : mod. PrtSET_TH, PrtSETKP_TH, PrtSET_EJ, PrtSET_SP for US enhance.
* Jul-10-93 : 01.00.12 : R.Itoh     : Add PrtDflSET()
* Oct-29-93 : 02.00.02 : K.You      : Del. PrtDflSET()
* Apr-25-94 : 00.00.05 : Yoshiko.Jim: E-061 corr.(mod. PrtSET())
* Jan-26-95 : 03.00.00 : M.Ozawa    : recover PrtDflSET()
* Jun-01-95 : 03.00.00 : T.Nakahata : Print Condiment of Promotional PLU
* Jul-24-95 : 03.00.04 : T.Nakahata : display customer name at 1st frame
* Apr-03-96 : 03.01.02 : M.Ozawa    : Bug fix, correct kp#5 - 8 status of child plu
* Nov-18-99 : 00.00.01 : M.Ozawa    : Enhanced to 2172
* Dec-14-93 : 00.00.01 : M.Ozawa    : Add PrtDflSETForm()
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

/**
;============================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;============================================================================
**/
/*
*===========================================================================
** Format  : VOID  PrtSET(TRANINFORMATION  *pTran, ITEMSALES *pItem);
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMSALES        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints set sales.
*
*            See usage of CLASS_SETMENU and function ItemSalesSetMenu() which
*            used to implement AC116 Promotional Pricing functionality.
*
*            See also function MldSET() which does something similar for displayed
*            receipt window.
*===========================================================================
*/
VOID PrtSET(TRANINFORMATION  *pTran, ITEMSALES  *pItem)
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */
/*    if ((pTran->TranCurQual.auchTotalStatus[4] & EVEN_MDC_BIT1) ||    / not consolidation */
    /* V3.3 */
    if (((pTran->TranCurQual.flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_UNBUFFER) ||        /* unbuffering print */
        (pItem->fsPrintStatus & (PRT_SINGLE_RECPT | PRT_DOUBLE_RECPT))) {  /* tichet */

        PrtPortion(pItem->fsPrintStatus);
    } else {                                                    /* consolidation */
        PrtPortion2(pItem);
    }

    if ( fsPrtStatus & PRT_REQKITCHEN ) {     /* kitchen print */
        switch ( pItem->uchMinorClass ) {
        case CLASS_SETMENU:
        case CLASS_SETITEMDISC:
        case CLASS_SETMODDISC:
            PrtKPSetMenuItem(pTran, pItem);
            break;

        case CLASS_OEPPLU:
        case CLASS_OEPITEMDISC:
        case CLASS_OEPMODDISC:
            PrtKPItem( pTran, pItem );
            break;

        default:
            break;
        }
    }
                                                        /* E-061 corr. 4/25 */
    if ( pItem->fsPrintStatus & PRT_VALIDATION ) {           /* validation print */
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
            return;
        }
    }

    if ( fsPrtPrintPort & PRT_SLIP ) {        /* slip print */
        PrtSET_SP(pTran, pItem);
    }

    if ( fsPrtPrintPort & PRT_RECEIPT ) {     /* thermal print */
        if (fsPrtStatus & PRT_TAKETOKIT) {
            PrtSETKP_TH(pTran, pItem);
        } else {
            PrtSET_TH(pTran, pItem);
        }
    }

    if ( fsPrtPrintPort & PRT_JOURNAL ) {     /* journal print */
            PrtSET_EJ(pTran, pItem);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtSET_TH(TRANINFORMATION  *pTran, ITEMSALES *pItem);
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMSALES        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints set sales. (thermal)
*===========================================================================
*/
VOID   PrtSET_TH(TRANINFORMATION  *pTran, ITEMSALES *pItem)
{
    USHORT  i;
    TCHAR   auchDummy[NUM_PLU_LEN] = {0};
    TCHAR   aszPLUNo[PLU_MAX_DIGIT + 1] = {0};
	SHORT numCounter; //US Customs

    PrtTHHead(pTran);                  /* print header if necessary */

    PrtTHVoid(pItem->fbModifier, pItem->usReasonCode);      /* void line */

//Saratoga Functionality (US Customs cwunn 4/25/03)
	if(!(CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2))) { //if US Customs #/type bit turned off (turned on is lower down)
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				PrtTHMnemNumber(TRN_SKUNO_ADR, pItem->aszNumber[numCounter]);     /* number line */
			}
		} else {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				PrtTHNumber(pItem->aszNumber[numCounter]);     /* number line */
			}
		}
	}
//End Saratoga Functionality

    if (pItem->fsPrintStatus & (PRT_SINGLE_RECPT | PRT_DOUBLE_RECPT) ) {
            /* ticket print status */
        PrtTHSeatNo(pItem->uchSeatNo);                     /* seat no. */
    }

    /* 2172 */
    if ( CliParaMDCCheck(MDC_PLU2_ADR, ODD_MDC_BIT2) ) {
        RflConvertPLU(aszPLUNo, pItem->auchPLUNo);
        PrtTHPLUNo(aszPLUNo);
    }

    if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
        PrtTHWeight(pTran, pItem);
    }  else {
        PrtTHQty(pTran, pItem);
    }

//US Customs Functionality (US Customs cwunn 4/25/03)
	if(CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)) { //if US Customs #/type bit turned on (turned off is above)
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				PrtTHMnemNumber(TRN_SKUNO_ADR, pItem->aszNumber[numCounter]);     /* number line */
			}
		} else {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				PrtTHNumber(pItem->aszNumber[numCounter]);     /* number line */
			}
		}
	}
//End US Customs Functionality

    /* --- print noun plu and its modifiers and condiments --- */
    PrtTHItems(pTran, pItem);

    /* -- send child plu? -- */
    if ( (pTran->TranCurQual.flPrintStatus) & CURQUAL_SETMENU ) {
        for ( i = 0; i < pItem->uchChildNo; i++ ) {
            /* check condiment PLU exist or not */
            if (_tcsncmp(pItem->Condiment[i].auchPLUNo, auchDummy, NUM_PLU_LEN) == 0) {
                continue;
            }

            PrtTHChild(pItem->Condiment[i].uchAdjective, pItem->Condiment[i].aszMnemonic);
        }
    }
}

/*
*===========================================================================
** Format  : VOID  PrtSETKP_TH(TRANINFORMATION  *pTran, ITEMSALES *pItem);
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMSALES        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints setmenu when take to kp print. (thermal)
*===========================================================================
*/
VOID   PrtSETKP_TH(TRANINFORMATION  *pTran, ITEMSALES *pItem)
{
    USHORT i;
    UCHAR  fbKpPort;
    TCHAR  auchDummy[NUM_PLU_LEN] = {0};
	SHORT numCounter; //US Customs

    PrtTHVoid(pItem->fbModifier, pItem->usReasonCode);      /* void line */

//Saratoga Functionality (US Customs cwunn 4/25/03)
	if(!(CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2))) { //if US Customs #/type bit turned off (turned on is lower down)
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				PrtTHMnemNumber(TRN_SKUNO_ADR, pItem->aszNumber[numCounter]);     /* number line */
			}
		} else {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				PrtTHNumber(pItem->aszNumber[numCounter]);     /* number line */
			}
		}
	}
//End Saratoga Fucntionality

    PrtTHSeatNo2(pItem->uchSeatNo);                     /* seat no. */

    if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
        PrtTHWeight(pTran, pItem);
    }  else {
        PrtTHQty(pTran, pItem);
    }

//US Customs Functionality (US Customs cwunn 4/25/03)
	if(CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)) { //if US Customs #/type bit turned on (turned off is above)
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				PrtTHMnemNumber(TRN_SKUNO_ADR, pItem->aszNumber[numCounter]);     /* number line */
			}
		} else {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				PrtTHNumber(pItem->aszNumber[numCounter]);     /* number line */
			}
		}
	}
//End US Customs Fucntionality

    /* --- print noun plu and its modifiers and condiments --- */
    PrtTHItems(pTran, pItem);

    if ( (pTran->TranCurQual.flPrintStatus) & CURQUAL_SETMENU_KITCHEN ) {
        return;
    }

    for ( i = 0; i < pItem->uchChildNo; i++ ) {
        /* check condiment PLU exist or not */
        if (_tcsncmp(pItem->Condiment[i].auchPLUNo, auchDummy, NUM_PLU_LEN) == 0) {
            continue;
        }

        fbKpPort = 0;
        PrtChkKPPortSub(&fbKpPort, (UCHAR)((pItem->Condiment[i].ControlCode.auchPluStatus[2]) >> 4), pTran, PRT_MDC);

        /* ---- set printer port #5 - #8    R3.1 ---- */
        PrtChkKPPortSub2(&fbKpPort, (UCHAR)( (pItem->Condiment[i].ControlCode.auchPluStatus[6]) << 4 ), pTran, PRT_MDC);

        if ( fbKpPort & uchPrtCurKP ) {
            PrtTHChild(pItem->Condiment[i].uchAdjective, pItem->Condiment[i].aszMnemonic);
        }
    }
}

/*
*===========================================================================
** Format  : VOID  PrtSET_EJ(TRANINFORMATION  *pTran, ITEMSALES *pItem);
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMSALES        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints set sales. (electric journal)
*===========================================================================
*/
VOID   PrtSET_EJ(TRANINFORMATION  *pTran, ITEMSALES *pItem)
{
    USHORT  i;
    TCHAR   auchDummy[NUM_PLU_LEN] = {0};
    TCHAR  aszPLUNo[PLU_MAX_DIGIT + 1 + 4] = {0};
    TCHAR  aszPLUSymbol[] = _T("PLU#");
	SHORT numCounter; //US Customs

    PrtEJVoid(pItem->fbModifier, pItem->usReasonCode);      /* void line */

//Saratoga Functionality (US Customs cwunn 4/25/03)
	if(!(CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2))) { //if US Customs #/type bit turned off (turned on is lower down)
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				PrtEJMnemNumber(TRN_SKUNO_ADR, pItem->aszNumber[numCounter]);     /* number line */
			}
		} else {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				PrtEJNumber(pItem->aszNumber[numCounter]);     /* number line */
			}
		}
	}
//End Saratoga Functionality

    PrtEJTaxMod((pTran->TranModeQual).fsModeStatus, pItem->fbModifier);

    /* -- if Building operation Mnemonics 2172   -- */
    RflConvertPLU(aszPLUNo+4, pItem->auchPLUNo);
    if (pItem->fsLabelStatus & ITM_CONTROL_NOTINFILE) {
        PrtEJPLUBuild(TRN_PLUBLD_ADR, aszPLUNo+4, pItem->usDeptNo);
    } else {
        _tcsncpy(aszPLUNo, aszPLUSymbol, 4);
        //memcpy(aszPLUNo, aszPLUSymbol, 4);      /* Dec/23/2000 */
        PrtEJPLUNo(aszPLUNo);
    }

    if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
        PrtEJWeight(pTran, pItem);
    }  else {
        PrtEJQty(pItem);
    }

//US Customs Functionality (US Customs cwunn 4/25/03)
	if(CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)) { //if US Customs #/type bit turned on (turned off is above)
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				PrtEJMnemNumber(TRN_SKUNO_ADR, pItem->aszNumber[numCounter]);     /* number line */
			}
		} else {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				PrtEJNumber(pItem->aszNumber[numCounter]);     /* number line */
			}
		}
	}
//End US Customs Functionality

    PrtEJItems(pItem);

    /* -- send child plu? -- */
    if ( (pTran->TranCurQual.flPrintStatus) & CURQUAL_SETMENU ) {
        for ( i = 0; i < pItem->uchChildNo; i++ ) {
            /* check child PLU exist or not */
            if (_tcsncmp(pItem->Condiment[i].auchPLUNo, auchDummy, NUM_PLU_LEN) == 0) {
                continue;
            }

            PrtEJChild(pItem->Condiment[i].uchAdjective, pItem->Condiment[i].aszMnemonic);
        }
    }
}

/*
*===========================================================================
** Format  : VOID  PrtSET_SP(TRANINFORMATION  *pTran, ITEMSALES *pItem);
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMSALES        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints setmenu. (slip)
*===========================================================================
*/
VOID PrtSET_SP(TRANINFORMATION  *pTran, ITEMSALES  *pItem)
{
    TCHAR   aszSPPrintBuff[13][PRT_SPCOLUMN + 1] = {0}; /* print data save area */
    TCHAR   aszPLUNo[PLU_MAX_DIGIT + 1] = {0};
    USHORT  usSlipLine = 0;             /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines to be added */
    USHORT  i;

	//Saratoga Functionality (US Customs cwunn 4/25/03)
	if(!(CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2))) { //if US Customs #/type bit turned off (turned on is lower down)
		/* -- set void mnemonic and number -- */
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(i = 0; i < NUM_OF_NUMTYPE_ENT; i++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[i][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usSlipLine += PrtSPVoidMnemNumber(aszSPPrintBuff[0], pItem->fbModifier, pItem->usReasonCode, TRN_SKUNO_ADR, pItem->aszNumber[i]);
			}
		} else {
			for(i = 0; i < NUM_OF_NUMTYPE_ENT; i++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[i][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usSlipLine += PrtSPVoidNumber(aszSPPrintBuff[0], pItem->fbModifier, pItem->usReasonCode, pItem->aszNumber[i]);
			}
		}
	}
//End Saratoga Functionality

    /* -- set seat mnemonic and number -- */
    if (fsPrtStatus & PRT_TAKETOKIT) {  /* take to kitchen status */
        usSlipLine += PrtSPSeatNo(aszSPPrintBuff[0], pItem->uchSeatNo);    /* seat no. */
    }

    /* 2172 */
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

    /* -- set tax modifier mnemonic, quantity and unit price -- */
    usSlipLine += PrtSPQty(aszSPPrintBuff[usSlipLine], pTran, pItem);

    /* -- set mnemonics (adjective, PLU, print mod.) -- */
    usSlipLine += PrtSPItems(aszSPPrintBuff[usSlipLine], pTran, pItem);

    /* -- send child plu? -- */
    if ( (pTran->TranCurQual.flPrintStatus) & CURQUAL_SETMENU ) {
		TCHAR   auchDummy[NUM_PLU_LEN] = {0};

        for ( i = 0; i < pItem->uchChildNo; i++ ) {
            /* check child PLU exist or not */
            if (_tcsncmp(pItem->Condiment[i].auchPLUNo, auchDummy, NUM_PLU_LEN) == 0) {
                continue;
            }
            usSlipLine += PrtSPChild(aszSPPrintBuff[usSlipLine], pItem->Condiment[i].uchAdjective, pItem->Condiment[i].aszMnemonic);
        }
    }

//US Customs Functionality (US Customs cwunn 4/25/03)
	if(CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)) { //if US Customs #/type bit turned on (turned off is above)
		/* -- set void mnemonic and number -- */
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(i = 0; i < NUM_OF_NUMTYPE_ENT; i++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[i][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usSlipLine += PrtSPVoidMnemNumber(aszSPPrintBuff[0], pItem->fbModifier, pItem->usReasonCode, TRN_SKUNO_ADR, pItem->aszNumber[i]);
			}
		} else {
			for(i = 0; i < NUM_OF_NUMTYPE_ENT; i++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[i][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usSlipLine += PrtSPVoidNumber(aszSPPrintBuff[0], pItem->fbModifier, pItem->usReasonCode, pItem->aszNumber[i]);
			}
		}
	}
//End US CustomsFunctionality

    /* -- check if paper change is necessary or not -- */
    usSaveLine = PrtCheckLine(usSlipLine, pTran);

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
** Format  : VOID  PrtDflSET(TRANINFORMATION  *pTran, ITEMSALES *pItem);
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMSALES        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints set sales.
*===========================================================================
*/
VOID PrtDflSET(TRANINFORMATION  *pTran, ITEMSALES  *pItem)
{
	PRTDFLBUFF  dbuff = {0};
    UCHAR  fbPrtPort;                     /* print status for kp */
    UCHAR  fbStatWork;                    /* print status for kp */
    UCHAR  i, j;
    TCHAR  auchDummy[NUM_PLU_LEN] = {0};

    if ( (pItem->fsPrintStatus & PRT_SPCL_PRINT) && (pItem->uchPrintModNo == 0) ) {
        /* send all child plu as not kp item */
        PrtDflSETSendAsNotKP(&dbuff, pTran, pItem);
        return;
    }

    /* send child plu whose kp destination is none */
    PrtDflSETSendNoKPChild(&dbuff, pTran, pItem);

    /* check setmenu children plu's kp portion */
    fbPrtPort = PrtDflSETChkChildKP(pTran, pItem);

    for ( i = 0; i < MAX_DEST_SIZE;  i++ ) {
		USHORT usLineNo;                      /* number of lines to be displayed */
		UCHAR  fbCurPrtState = (1 << i);      /* current print status*/

        /* check setmenu children plu's kp portion */
        if ( !(fbPrtPort & fbCurPrtState) ) {
            continue;
        }

        memset(&dbuff, '\0', sizeof(dbuff));

        /* -- set header, trailer, void, parent plu, etc. -- */
        usLineNo = PrtDflSETParent(&dbuff, pTran, pItem);

        /* -- set child PLU -- */
        for ( j = 0; j < pItem->uchChildNo; j++ ) {
            /* check child PLU exist or not */
            if (_tcsncmp(pItem->Condiment[j].auchPLUNo, auchDummy, NUM_PLU_LEN) == 0) {
                continue;
            }

            if ( (pTran->TranCurQual.flPrintStatus) & CURQUAL_SETMENU_KITCHEN ) {
                break;
            }

            fbStatWork = 0;
            PrtChkKPPortSub(&fbStatWork, (UCHAR)( (pItem->Condiment[j].ControlCode.auchPluStatus[2]) >> 4 ), pTran, PRT_NOT_MDC);

            /* ---- set printer port #5 - #8    R3.1 ---- */
            PrtChkKPPortSub2(&fbStatWork, (UCHAR)( (pItem->Condiment[j].ControlCode.auchPluStatus[6]) << 4 ), pTran, PRT_NOT_MDC);

            if (fbStatWork & fbCurPrtState) {
                usLineNo += PrtDflChild(dbuff.aszDflBuff[usLineNo], pItem->Condiment[j].uchAdjective, pItem->Condiment[j].aszMnemonic);
            }
        }

        /* -- send data to DFL -- */
        if (i < 4) {    /* CRT #1 - #4 */
            PrtDflSETSend( (UCHAR)((1 << i) + 0x30), 0x30, usLineNo, &dbuff, pItem);
        } else {        /* CRT #5 - #8 */
            PrtDflSETSend( 0x30, (UCHAR)((1 << (i - 4)) + 0x30), usLineNo, &dbuff, pItem);
        }
    }
}
/*
*===========================================================================
** Format  : UCHAR  PrtDflSETChkChildKP(TRANINFORMATION  *pTran, ITEMSALES *pItem);
*
*   Input  : TRANINFORMATION  *pTran     -transaction information
*            ITEMSALES        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : UCHAR    uchPort            -destination kp
*
** Synopsis: This function checks child PLU's destination kp.
*===========================================================================
*/
UCHAR  PrtDflSETChkChildKP(TRANINFORMATION  *pTran, ITEMSALES *pItem)
{
    UCHAR fbPrtPort = 0;
    USHORT i;
	TCHAR  auchDummy[NUM_PLU_LEN] = {0};

    for ( i = 0; i < pItem->uchChildNo; i++ ) {
		UCHAR fbStatWork;

        /* check condiment PLU exist or not */
        if (_tcsncmp(pItem->Condiment[i].auchPLUNo, auchDummy, NUM_PLU_LEN)) {
            continue;
        }

        fbStatWork = 0;
        PrtChkKPPortSub(&fbStatWork, (UCHAR)( (pItem->Condiment[i].ControlCode.auchPluStatus[2]) >> 4), pTran, PRT_NOT_MDC);

        /* ---- set printer port #5 - #8    R3.1 ---- */
        PrtChkKPPortSub2(&fbStatWork, (UCHAR)( (pItem->Condiment[i].ControlCode.auchPluStatus[6]) << 4 ), pTran, PRT_NOT_MDC);

        fbPrtPort |= fbStatWork;
    }

    return(fbPrtPort);
}

/*
*===========================================================================
** Format  : SHORT  PrtDflSETParent(PRTDFLBUFF *pBuff, TRANINFORMATION  *pTran, ITEMSALES *pItem);
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMSALES        *pItem     -Item Data address
*   Output : UCHAR            **pBuff     -format data
*   InOut  : none
** Return  : SHORT  usLineno             -line number
*
** Synopsis: This function sets header, trailer, void, number, parent PLU.
*===========================================================================
*/
SHORT  PrtDflSETParent(PRTDFLBUFF *pBuff, TRANINFORMATION  *pTran, ITEMSALES *pItem)
{
    USHORT usLineNo;
	TCHAR  aszPLUNo[PLU_MAX_DIGIT + 1] = {0};
	SHORT numCounter; //US Customs

    /* --- if this frame is 1st frame, display customer name --- */
    PrtDflCustHeader( pTran );

    /* -- set header -- */
    usLineNo = PrtDflHeader(pBuff->aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(pBuff->aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);

    /* -- set item data -- */
    usLineNo += PrtDflVoid(pBuff->aszDflBuff[usLineNo], pItem->fbModifier, pItem->usReasonCode);

//Saratoga Functionality (US Customs cwunn 4/25/03)
	if(!(CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2))) { //if US Customs #/type bit turned off (turned on is lower down)
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usLineNo += PrtDflMnemNumber(pBuff->aszDflBuff[usLineNo], TRN_SKUNO_ADR, pItem->aszNumber[numCounter]);
				}
		} else {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usLineNo += PrtDflNumber(pBuff->aszDflBuff[usLineNo], pItem->aszNumber[numCounter]);
			}
		}
	}
//End Saratoga Functionality

    usLineNo += PrtDflTaxMod(pBuff->aszDflBuff[usLineNo], pTran->TranModeQual.fsModeStatus, pItem->fbModifier);

    if (pItem->uchSeatNo) {
        usLineNo += PrtDflSeatNo(pBuff->aszDflBuff[usLineNo], pItem);  /* seat no */
    }

    /* 2172 */
    if ( CliParaMDCCheck(MDC_PLU2_ADR, ODD_MDC_BIT2) ) {
        RflConvertPLU(aszPLUNo, pItem->auchPLUNo);
        usLineNo += PrtDflPLUNo(pBuff->aszDflBuff[usLineNo], aszPLUNo);
    }

    if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
        usLineNo += PrtDflWeight(pBuff->aszDflBuff[usLineNo], pTran, pItem);              /* weight price */
    }  else {
        usLineNo += PrtDflQty(pBuff->aszDflBuff[usLineNo], pItem);                        /* qty price */
    }

//US Customs Functionality (US Customs cwunn 4/25/03)
	if(CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)) { //if US Customs #/type bit turned on (turned off is above)
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usLineNo += PrtDflMnemNumber(pBuff->aszDflBuff[usLineNo], TRN_SKUNO_ADR, pItem->aszNumber[numCounter]);
				}
		} else {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop printing
					break;
				}//Print all #/Type entries that are stored
				usLineNo += PrtDflNumber(pBuff->aszDflBuff[usLineNo], pItem->aszNumber[numCounter]);
			}
		}
	}
//End US Customs Functionality

    usLineNo += PrtDflItems(pBuff->aszDflBuff[usLineNo], pItem);
    return(usLineNo);
}
/*
*===========================================================================
** Format  : VOID  PrtDflSETSendAsNotKP(PRTDFLBUFF *pBuff, TRANINFORMATION  *pTran, ITEMSALES *pItem);
*
*   Input  : PRTDFLBUFF       *pBuff  -work buffer
*            TRANINFORMATION  *pTran     -Transaction information address
*            ITEMSALES        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints send all child plu as no kp item.
*===========================================================================
*/
VOID  PrtDflSETSendAsNotKP(PRTDFLBUFF *pBuff, TRANINFORMATION  *pTran, ITEMSALES *pItem)
{
    USHORT i;
    USHORT usLineNo;
	TCHAR auchDummy[NUM_PLU_LEN] = {0};

    /* -- set header, trailer, void, parent plu, etc. -- */
    usLineNo = PrtDflSETParent(pBuff, pTran, pItem);

    /* -- set all child PLU -- */
    for ( i = 0; i < pItem->uchChildNo; i++ ) {
       /* check condiment PLU exist or not */
       if (_tcsncmp(pItem->Condiment[i].auchPLUNo, auchDummy, NUM_PLU_LEN) == 0) {
           continue;
       }

       if ( (pTran->TranCurQual.flPrintStatus) & CURQUAL_SETMENU_KITCHEN ) {
           break;
       }

       usLineNo += PrtDflChild(pBuff->aszDflBuff[usLineNo], pItem->Condiment[i].uchAdjective, pItem->Condiment[i].aszMnemonic);
    }

    /* -- send data to DFL -- */
    PrtDflSETSend(0x30, 0x30, usLineNo, pBuff, pItem);

}

/*
*===========================================================================
** Format  : VOID  PrtDflSETSendNoKPChild(PRTDFLBUFF *pBuff, TRANINFORMATION  *pTran, ITEMSALES *pItem);
*
*   Input  : PRTDFLBUFF       *pBuff     -work buffer
*            TRANINFORMATION  *pTran     -Transaction information address
*            ITEMSALES        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function sends child plu (not kp item).
*===========================================================================
*/
VOID  PrtDflSETSendNoKPChild(PRTDFLBUFF *pBuff, TRANINFORMATION  *pTran, ITEMSALES *pItem)
{
    USHORT i, j;
    USHORT usLineNo;
	TCHAR  auchDummy[NUM_PLU_LEN] = {0};

    for ( i = j = 0; i < pItem->uchChildNo; i++ ) {
		UCHAR fbStatWork = 0;

        /* check condiment PLU exist or not */
        if (_tcsncmp(pItem->Condiment[i].auchPLUNo, auchDummy, NUM_PLU_LEN) == 0) {
            continue;
        }

        PrtChkKPPortSub(&fbStatWork, (UCHAR)( (pItem->Condiment[i].ControlCode.auchPluStatus[2]) >> 4), pTran, PRT_NOT_MDC);

        /* ---- set printer port #5 - #8    R3.1 ---- */
        PrtChkKPPortSub2(&fbStatWork, (UCHAR)( (pItem->Condiment[i].ControlCode.auchPluStatus[6]) << 4 ), pTran, PRT_NOT_MDC);

        if (fbStatWork == 0) {
            j=1;
            break;
        }
    }

    /* There is no child plu whose destination kp is none */
    if (j != 1) {
      return;
    }

    /* send child plu  whose destination kp is none */
    /* -- set header, trailer, void, parent plu, etc. -- */
    usLineNo = PrtDflSETParent(pBuff, pTran, pItem);

    /* -- set child PLU -- */
    for ( j = 0; j < pItem->uchChildNo; j++ ) {
		UCHAR fbStatWork = 0;

        /* check condiment PLU exist or not */
        if (_tcsncmp(pItem->Condiment[j].auchPLUNo, auchDummy, NUM_PLU_LEN) == 0) {
            continue;
        }

        if ( (pTran->TranCurQual.flPrintStatus) & CURQUAL_SETMENU_KITCHEN ) {
            break;
        }

        PrtChkKPPortSub(&fbStatWork, (UCHAR)( (pItem->Condiment[j].ControlCode.auchPluStatus[2]) >> 4 ), pTran, PRT_NOT_MDC);

        /* ---- set printer port #5 - #8    R3.1 ---- */
        PrtChkKPPortSub2(&fbStatWork, (UCHAR)( (pItem->Condiment[j].ControlCode.auchPluStatus[6]) << 4 ), pTran, PRT_NOT_MDC);

        if (fbStatWork == 0) {
            usLineNo += PrtDflChild(pBuff->aszDflBuff[usLineNo], pItem->Condiment[j].uchAdjective, pItem->Condiment[j].aszMnemonic);
        }
    }

    /* -- send data to DFL (destination CRT as no destination kp " 0x30" )-- */
    PrtDflSETSend(0x30, 0x30, usLineNo, pBuff, pItem);

}

/*
*===========================================================================
** Format  : VOID  PrtDflSETSend(UCHAR uchDestKP, USHORT  usLineNo,
*                               PRTDFLBUFF *pBuff, ITEMSALES *pItem);
*
*
*   Input  : UCHAR            uchDestKP   -destination kp number
*            USHORT           usLineNo    -line number
*            PRTDFLBUFF *pBuff            -buffer
*            ITEMSALES       *pItem       -item data
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function sends dfl data.
*===========================================================================
*/
VOID  PrtDflSETSend(UCHAR uchDestKP1, UCHAR uchDestKP2, USHORT  usLineNo,
                                        PRTDFLBUFF *pBuff, ITEMSALES *pItem)
{
    USHORT i;
    USHORT usOffset = 0;

    /* -- set destination CRT -- */
    PrtDflIf.Dfl.DflHead.auchCRTNo[0] = uchDestKP1;
    PrtDflIf.Dfl.DflHead.auchCRTNo[1] = uchDestKP2;

    /* -- check void status -- */
    PrtDflCheckVoid(pItem->fbModifier);

    /* -- set display data in the buffer -- */
    PrtDflIType(usLineNo, DFL_SALES);

    for ( i = 0; i < usLineNo; i++ ) {
        PrtDflSetData(pBuff->aszDflBuff[i], &usOffset);
        if ( pBuff->aszDflBuff[i][PRT_DFL_LINE] != '\0' ) {
            i++;
        }
    }

    /* -- send display data -- */
    PrtDflSend();
}


/*
*===========================================================================
** Format  : VOID  PrtDflSET(TRANINFORMATION  *pTran, ITEMSALES *pItem);
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMSALES        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints set sales.
*===========================================================================
*/
USHORT PrtDflSETForm(TRANINFORMATION  *pTran, ITEMSALES  *pItem, TCHAR *puchBuffer)
{
	TCHAR  aszDflBuff[33][PRT_DFL_LINE + 1] = {0}; /* display data save area */
    USHORT usLineNo = 0, i;                        /* number of lines to be displayed */
    UCHAR  j;

    /* set parent plu format */
    usLineNo = PrtDflDeptPLUForm(pTran, pItem, &aszDflBuff[0][0]);

    /* -- set child PLU -- */
    for ( j = 0; j < pItem->uchChildNo; j++ ) {
        usLineNo += PrtDflChild(aszDflBuff[usLineNo], pItem->Condiment[j].uchAdjective, pItem->Condiment[j].aszMnemonic);
    }

    for (i = 0; i < usLineNo; i++) {
        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }

	NHPOS_ASSERT(PRT_DFL_LINE_MAX > usLineNo);   // limit to size KDS_LINE_DATA_LEN characters. see size of aszLineData[] in KDSSENDDATA

    _tcsncpy(puchBuffer, aszDflBuff[0], usLineNo * (PRT_DFL_LINE + 1));
    //memcpy(puchBuffer, aszDflBuff, usLineNo*(PRT_DFL_LINE+1));

    return usLineNo;
}

/****** End of Source ******/
