/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS, E&M OISO                **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Multiline Display Department/PLU sales
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDMODD.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*           MldModDisc() : display modifiered discount on scroll area
*           MldModSET() : display modifiered discount on scroll area(set sales)
*
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
* Feb-28-95 : 03.00.00 :  M.Ozawa   : Initial
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
#include <para.h>
#include <csstbpar.h>
#include <mld.h>
#include "mldlocal.h"
#include "mldrin.h"
/* #include "prtrin.h" */
#include "termcfg.h"

/**
;============================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;============================================================================
**/
/*
*===========================================================================
** Format  : SHORT    MLDModDisc(ITEMSALES *pItem, USHORT usType);
*
*   Input  : ITEMSALES        *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function display modified discount.
*===========================================================================
*/
SHORT MldModDisc(ITEMSALES *pItem, USHORT usType, USHORT usScroll)
{
    if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {
        return (MldDriveModDisc(pItem, usType, usScroll));
    } else {
        return (MldPrechkModDisc(pItem, usType, usScroll));
    }
}

/*
*===========================================================================
** Format  : SHORT    MLDModDisc(ITEMSALES *pItem, USHORT usType);
*
*   Input  : ITEMSALES        *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function display modified discount.
*===========================================================================
*/
SHORT MldPrechkModDisc(ITEMSALES *pItem, USHORT usType, USHORT usScroll)
{
	TCHAR      aszScrollBuff[24][MLD_PRECOLUMN1 + 1] = { 0 }; /* scroll data save area */
    LONG       alAttrib[24][MLD_PRECOLUMN1 + 1];                /* reverse video control flag area */
    USHORT     usScrollLine = 0;            /* number of lines to be printed */
    USHORT     usPrice;
	SHORT      numCounter; //US Customs

    if ((pItem->fbModifier & VOID_MODIFIER) && (MldCheckReverseVideo())) {
        /* --- reverse video if void operation ---- */
        memset(&alAttrib[0][0], MLD_ATTRIB_REV_VIDEO, sizeof(alAttrib));
    } else {
        memset(&alAttrib[0][0], MLD_ATTRIB_STD_VIDEO, sizeof(alAttrib));
    }

//Preserve Saratoga operation in US Customs (1.4.2.2)
	if(!CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)){//US Customs off by MDC 21B (on is below)
		/* -- set void mnemonic and number -- */
		usScrollLine += MldVoidOffline(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->fbModifier, MLD_PRECHECK_SYSTEM);

		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop displaying
					break;
				}//Display all #/Type entries that are stored
			}
			if(pItem->aszNumber[numCounter-1] && (numCounter < 0)){
				usScrollLine += MldMnemNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_SKUNO_ADR, pItem->aszNumber[numCounter-1], MLD_PRECHECK_SYSTEM);
			}
		} else {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop displaying
					break;
				}//Display all #/Type entries that are stored
			}
			if(pItem->aszNumber[numCounter-1] && (numCounter < 0)){
				usScrollLine += MldNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszNumber[numCounter-1], MLD_PRECHECK_SYSTEM);
			}
		}
	}
//End Saratoga Operation

    /* -- set tax modifier mnemonic -- */
    usScrollLine += MldTaxMod(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], 0, pItem->fbModifier, MLD_PRECHECK_SYSTEM);

    /* -- set modifiered discount mnemonic -- */
    usScrollLine += MldTranMnem(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_MODID_ADR, MLD_PRECHECK_SYSTEM);

    /* -- scalable ? -- */
    if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
        /* -- manual weight input -- */
        if (pItem->fsPrintStatus & PRT_MANUAL_WEIGHT) {
            /* -- set manual scale sales mnemonic -- */
            usScrollLine += MldTranMnem(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_MSCALE_ADR, MLD_PRECHECK_SYSTEM);
        }
        /* -- set weight and unit price -- */
        usScrollLine += MldWeight(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], 0, pItem, MLD_PRECHECK_SYSTEM);
    }

    /* display price of dept/plu at precheck/post check system */
    if (CliParaMDCCheck(MDC_DISPLAY_ADR, ODD_MDC_BIT1)) {
        usPrice = 0;
    } else {
        usPrice = 1;
    }

    /* -- set mnemonics (adjective, PLU, print mod., condiment) -- */
    usScrollLine += MldItems(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem, usPrice, &alAttrib[MLD_SETLINE_BASE(usScrollLine)][0], MLD_PRECHECK_SYSTEM, usType);

    /* display link plu, saratoga */

    if (pItem->usLinkNo) {
		DCURRENCY  lPrice;
		LONG       lQTY;

        /* -- set void mnemonic and number -- */
        usScrollLine += MldVoidOffline(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->fbModifier, MLD_PRECHECK_SYSTEM);
        /* -- set tax modifier mnemonic -- */
        usScrollLine += MldTaxMod(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], 0, pItem->fbModifier, MLD_PRECHECK_SYSTEM);

        lPrice = pItem->Condiment[0].lUnitPrice;

        if ((pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) == 0) {
           lQTY = pItem->lQTY/1000L;
        } else {
            if (pItem->lQTY >= 0) {
                lQTY = 1;
            } else {
                lQTY = -1;
            }
        }
        lPrice *= lQTY;
        usScrollLine += MldLinkPlu(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRUE, pItem->Condiment[0].uchAdjective, pItem->Condiment[0].aszMnemonic, lQTY, lPrice, MLD_PRECHECK_SYSTEM);
    }

    /* -- set discount rate and discount amount.    -- */
    /* -- "0" means not print transaction mnemonics -- */
    usScrollLine += MldDiscount(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], 0, 0, pItem->uchRate, pItem->lDiscountAmount, MLD_PRECHECK_SYSTEM);

//US Customs operation in US Customs (1.4.2.2)
	if(CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)){ //US Customs on by MDC 21B (off is above)
		/* -- set void mnemonic and number -- */
		usScrollLine += MldVoidOffline(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->fbModifier, MLD_PRECHECK_SYSTEM);

		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter < NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop displaying
					break;
				}//Display all #/Type entries that are stored
			}
			if(pItem->aszNumber[numCounter-1] && (numCounter < 0)){
				usScrollLine += MldMnemNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_SKUNO_ADR, pItem->aszNumber[numCounter-1], MLD_PRECHECK_SYSTEM);
			}
		} else {
			for(numCounter=0; numCounter < NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop displaying
					break;
				}//Display all #/Type entries that are stored
			}
			if(pItem->aszNumber[numCounter-1] && (numCounter < 0)){
				usScrollLine += MldNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszNumber[numCounter-1], MLD_PRECHECK_SYSTEM);
			}
		}
	}
//End US Customs Operation

    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_PRECOLUMN1, &alAttrib[0][0]));
}

/*
*===========================================================================
** Format  : SHORT    MLDModDisc(ITEMSALES *pItem, USHORT usType);
*
*   Input  : ITEMSALES        *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function display modified discount.
*===========================================================================
*/
SHORT MldDriveModDisc(ITEMSALES *pItem, USHORT usType, USHORT usScroll)
{
	TCHAR      aszScrollBuff[27][MLD_DR3COLUMN + 1] = { 0 }; /* scroll data save area */
    LONG       alAttrib[27][MLD_PRECOLUMN1 + 1];                /* reverse video control flag area */
    USHORT     usScrollLine = 0;            /* number of lines to be printed */
	SHORT      numCounter;	//US Customs

    /* initialize the buffer */
    if ((pItem->fbModifier & VOID_MODIFIER) && (MldCheckReverseVideo())) {
        /* --- reverse video if void operation ---- */
        memset(&alAttrib[0][0], MLD_ATTRIB_REV_VIDEO, sizeof(alAttrib));
    } else {
        memset(&alAttrib[0][0], MLD_ATTRIB_STD_VIDEO, sizeof(alAttrib));
    }

//Preserve Saratoga operation in US Customs (1.4.2.2)
	if(!CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)) {//US Customs off by MDC 21B (on is below)
		/* -- set void mnemonic and number -- */
		usScrollLine += MldVoidOffline(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->fbModifier, MLD_DRIVE_THROUGH_3SCROLL);

		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter < NUM_OF_NUMTYPE_ENT; numCounter++) {//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop displaying
					break;
				}//Display all #/Type entries that are stored
			}
			if(pItem->aszNumber[numCounter-1] && (numCounter < 0)){
				usScrollLine += MldMnemNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_SKUNO_ADR, pItem->aszNumber[numCounter-1], MLD_DRIVE_THROUGH_3SCROLL);
			}
		} else {
			for(numCounter=0; numCounter < NUM_OF_NUMTYPE_ENT; numCounter++) {//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0') {//empty slot found, stop displaying
					break;
				}//Display all #/Type entries that are stored
			}
			if(pItem->aszNumber[numCounter-1] && (numCounter < 0)) {
				usScrollLine += MldNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszNumber[numCounter-1], MLD_DRIVE_THROUGH_3SCROLL);
			}
		}
	}
//End Saratoga Operation

    /* -- set tax modifier mnemonic -- */
    usScrollLine += MldTaxMod(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], 0, pItem->fbModifier, MLD_DRIVE_THROUGH_3SCROLL);

    /* -- set modifiered discount mnemonic -- */
    usScrollLine += MldTranMnem(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_MODID_ADR, MLD_DRIVE_THROUGH_3SCROLL);

    /* -- scalable ? -- */
    if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
        /* -- manual weight input -- */
        if (pItem->fsPrintStatus & PRT_MANUAL_WEIGHT) {
            /* -- set manual scale sales mnemonic -- */
            usScrollLine += MldTranMnem(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_MSCALE_ADR, MLD_DRIVE_THROUGH_3SCROLL);
        }
        /* -- set weight and unit price -- */
        usScrollLine += MldWeight(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], 0, pItem, MLD_DRIVE_THROUGH_3SCROLL);
    }

    /* -- set mnemonics (adjective, PLU, print mod., condiment) -- */
    usScrollLine += MldItems(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem, 0, &alAttrib[MLD_SETLINE_BASE(usScrollLine)][0], MLD_DRIVE_THROUGH_3SCROLL, usType);

    /* display link plu, saratoga */

    if (pItem->usLinkNo) {
		DCURRENCY  lPrice;
		LONG       lQTY;

        /* -- set void mnemonic and number -- */
        usScrollLine += MldVoidOffline(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->fbModifier, MLD_DRIVE_THROUGH_3SCROLL);

        /* -- set tax modifier mnemonic -- */
        usScrollLine += MldTaxMod(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], 0, pItem->fbModifier, MLD_DRIVE_THROUGH_3SCROLL);

        lPrice = pItem->Condiment[0].lUnitPrice;
        if ((pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) == 0) {
            lQTY = pItem->lQTY/1000L;
        } else {
            if (pItem->lQTY >= 0) {
                lQTY = 1;
            } else {
                lQTY = -1;
            }
        }
        lPrice *= lQTY;
        usScrollLine += MldLinkPlu(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRUE, pItem->Condiment[0].uchAdjective, pItem->Condiment[0].aszMnemonic, lQTY, lPrice, MLD_DRIVE_THROUGH_3SCROLL);
    }

    /* -- set discount rate and discount amount.    -- */
    /* -- "0" means not print transaction mnemonics -- */
    usScrollLine += MldDiscount(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], 0, 0, pItem->uchRate, pItem->lDiscountAmount, MLD_DRIVE_THROUGH_3SCROLL);

//US Customs operation in US Customs (1.4.2.2)
	if(CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)){//US Customs on by MDC 21B (off is above)
		/* -- set void mnemonic and number -- */
		usScrollLine += MldVoidOffline(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->fbModifier, MLD_DRIVE_THROUGH_3SCROLL);

		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter < NUM_OF_NUMTYPE_ENT; numCounter++) {//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0') {//empty slot found, stop displaying
					break;
				}//Display all #/Type entries that are stored
			}
			if(pItem->aszNumber[numCounter-1] && (numCounter < 0)) {
				usScrollLine += MldMnemNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_SKUNO_ADR, pItem->aszNumber[numCounter-1], MLD_DRIVE_THROUGH_3SCROLL);
			}
		} else {
			for(numCounter=0; numCounter < NUM_OF_NUMTYPE_ENT; numCounter++) {//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0') {//empty slot found, stop displaying
					break;
				}//Display all #/Type entries that are stored
			}
			if(pItem->aszNumber[numCounter-1] && (numCounter < 0)) {
				usScrollLine += MldNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszNumber[numCounter-1], MLD_DRIVE_THROUGH_3SCROLL);
			}
		}
	}
//End US Customs Operation
    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_DR3COLUMN, &alAttrib[0][0]));
}

/*
*===========================================================================
** Format  : SHORT    MldModSET(ITEMSALES *pItem, USHORT usType);
*
*   Input  : ITEMSALES        *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays set sales.
*===========================================================================
*/
SHORT MldModSET(ITEMSALES *pItem, USHORT usType, USHORT usScroll)
{
    if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {
        return (MldDriveModSET(pItem, usType, usScroll));
    } else {
        return (MldPrechkModSET(pItem, usType, usScroll));
    }
}

/*
*===========================================================================
** Format  : SHORT    MldModSET(ITEMSALES *pItem, USHORT usType);
*
*   Input  : ITEMSALES        *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays set sales.
*===========================================================================
*/
SHORT MldPrechkModSET(ITEMSALES *pItem, USHORT usType, USHORT usScroll)
{
    TCHAR  aszScrollBuff[20][MLD_PRECOLUMN1 + 1]; /* scroll data save area */
    LONG   alAttrib[20][MLD_PRECOLUMN1 + 1];                /* reverse video control flag area */
    USHORT usScrollLine = 0;            /* number of lines to be printed */
    USHORT usPrice;
	SHORT numCounter; //US Customs

    /* initialize the buffer */
    memset(aszScrollBuff[0], '\0', sizeof(aszScrollBuff));

    if ((pItem->fbModifier & VOID_MODIFIER) && (MldCheckReverseVideo())) {
        /* --- reverse video if void operation ---- */
        memset(&alAttrib[0][0], MLD_ATTRIB_REV_VIDEO, sizeof(alAttrib));
    } else {
        memset(&alAttrib[0][0], MLD_ATTRIB_STD_VIDEO, sizeof(alAttrib));
    }

    /* -- set void mnemonic and number -- */
    usScrollLine += MldVoidOffline(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->fbModifier, MLD_PRECHECK_SYSTEM);

//Preserve Saratoga operation in US Customs (1.4.2.2)
	if(!CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)){//US Customs off by MDC 21B (on is below)
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter < NUM_OF_NUMTYPE_ENT; numCounter++) {//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop displaying
					break;
				}//Display all #/Type entries that are stored
			}
			if(pItem->aszNumber[numCounter-1] && (numCounter < 0)) {
				usScrollLine += MldMnemNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_SKUNO_ADR, pItem->aszNumber[numCounter-1], MLD_PRECHECK_SYSTEM);
			}
		} else {
			for(numCounter=0; numCounter < NUM_OF_NUMTYPE_ENT; numCounter++) {//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0') {//empty slot found, stop displaying
					break;
				}//Display all #/Type entries that are stored
			}
			if(pItem->aszNumber[numCounter-1] && (numCounter < 0)){
				usScrollLine += MldNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszNumber[numCounter-1], MLD_PRECHECK_SYSTEM);
			}
		}
	}
//End Saratoga Operation

    /* -- set tax modifier mnemonic -- */
    usScrollLine += MldTaxMod(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], 0, pItem->fbModifier, MLD_PRECHECK_SYSTEM);

    /* -- set modifiered discount mnemonic -- */
    usScrollLine += MldTranMnem(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_MODID_ADR, MLD_PRECHECK_SYSTEM);

    /* display price of dept/plu at precheck/post check system */
    if (CliParaMDCCheck(MDC_DISPLAY_ADR, ODD_MDC_BIT1)) {
        usPrice = 0;
    } else {
        usPrice = 1;
    }

    /* -- set mnemonics (adjective, PLU, print mod., condiment) -- */
    usScrollLine += MldItems(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem, usPrice, &alAttrib[MLD_SETLINE_BASE(usScrollLine)][0], MLD_PRECHECK_SYSTEM, usType);

    /* -- set discount rate and discount amount.    -- */
    /* -- "0" means not print transaction mnemonics -- */
    usScrollLine += MldDiscount(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], 0, 0, pItem->uchRate, pItem->lDiscountAmount, MLD_PRECHECK_SYSTEM);

    /* -- send child plu? -- */
    /* read mdc before transaction open */
    if (CliParaMDCCheck(MDC_DEPT2_ADR, EVEN_MDC_BIT0)) {
		USHORT  i;
		TCHAR   auchDummy[NUM_PLU_LEN] = {0};

        for ( i = 0; i < pItem->uchChildNo; i++) {
            /* check condiment PLU exist or not */
            if (_tcsncmp(pItem->Condiment[i].auchPLUNo, auchDummy, NUM_PLU_LEN) == 0) {
                continue;
            }

            usScrollLine += MldChild(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->Condiment[i].uchAdjective, pItem->Condiment[i].aszMnemonic, MLD_PRECHECK_SYSTEM);
        }
    }

//US Customs operation in US Customs (1.4.2.2)
	if(CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)) {//US Customs on by MDC 21B (off is above)
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter < NUM_OF_NUMTYPE_ENT; numCounter++) {//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop displaying
					break;
				}//Display all #/Type entries that are stored
			}
			if(pItem->aszNumber[numCounter-1] && (numCounter < 0)){
				usScrollLine += MldMnemNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_SKUNO_ADR, pItem->aszNumber[numCounter-1], MLD_PRECHECK_SYSTEM);
			}
		} else {
			for(numCounter=0; numCounter < NUM_OF_NUMTYPE_ENT; numCounter++) {//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0') {//empty slot found, stop displaying
					break;
				}//Display all #/Type entries that are stored
			}
			if(pItem->aszNumber[numCounter-1] && (numCounter < 0)) {
				usScrollLine += MldNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszNumber[numCounter-1], MLD_PRECHECK_SYSTEM);
			}
		}
	}
//End US Customs Operation

    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_PRECOLUMN1, &alAttrib[0][0]));
}

/*
*===========================================================================
** Format  : SHORT    MldModSET(ITEMSALES *pItem, USHORT usType);
*
*   Input  : ITEMSALES        *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays set sales.
*===========================================================================
*/
SHORT MldDriveModSET(ITEMSALES *pItem, USHORT usType, USHORT usScroll)
{
    TCHAR  aszScrollBuff[28][MLD_DR3COLUMN + 1]; /* scroll data save area */
    LONG   alAttrib[28][MLD_PRECOLUMN1 + 1];                /* reverse video control flag area */
    USHORT usScrollLine = 0;            /* number of lines to be printed */
	SHORT numCounter; //US Customs

    /* initialize the buffer */
    memset(aszScrollBuff[0], '\0', sizeof(aszScrollBuff));

    if ((pItem->fbModifier & VOID_MODIFIER) && (MldCheckReverseVideo())) {
        /* --- reverse video if void operation ---- */
        memset(&alAttrib[0][0], MLD_ATTRIB_REV_VIDEO, sizeof(alAttrib));
    } else {
        memset(&alAttrib[0][0], MLD_ATTRIB_STD_VIDEO, sizeof(alAttrib));
    }

    /* -- set void mnemonic and number -- */
    usScrollLine += MldVoidOffline(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->fbModifier, MLD_DRIVE_THROUGH_3SCROLL);

//Preserve Saratoga operation in US Customs (1.4.2.2)
	if(!CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)) {//US Customs off by MDC 21B (on is below)
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++) {//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop displaying
					break;
				}//Display all #/Type entries that are stored
			}
			if(pItem->aszNumber[numCounter-1] && (numCounter < 0)) {
				usScrollLine += MldMnemNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_SKUNO_ADR, pItem->aszNumber[numCounter-1], MLD_DRIVE_THROUGH_3SCROLL);
			}
		} else {
			for(numCounter=0; numCounter < NUM_OF_NUMTYPE_ENT; numCounter++) {//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop displaying
					break;
				}//Display all #/Type entries that are stored
			}
			if(pItem->aszNumber[numCounter-1] && (numCounter < 0)){
				usScrollLine += MldNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszNumber[numCounter-1], MLD_DRIVE_THROUGH_3SCROLL);
			}
		}
	}
//End Saratoga Operation

    /* -- set tax modifier mnemonic -- */
    usScrollLine += MldTaxMod(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], 0, pItem->fbModifier, MLD_DRIVE_THROUGH_3SCROLL);

    /* -- set modifiered discount mnemonic -- */
    usScrollLine += MldTranMnem(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_MODID_ADR, MLD_PRECHECK_SYSTEM);

    /* -- set mnemonics (adjective, PLU, print mod., condiment) -- */
    usScrollLine += MldItems(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem, 0, &alAttrib[MLD_SETLINE_BASE(usScrollLine)][0], MLD_DRIVE_THROUGH_3SCROLL, usType);

    /* -- set discount rate and discount amount.    -- */
    /* -- "0" means not print transaction mnemonics -- */
    usScrollLine += MldDiscount(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], 0, 0, pItem->uchRate, pItem->lDiscountAmount, MLD_DRIVE_THROUGH_3SCROLL);

//US Customs operation in US Customs (1.4.2.2)
	if(CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)) {//US Customs on by MDC 21B (off is above)
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter < NUM_OF_NUMTYPE_ENT; numCounter++) {//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop displaying
					break;
				}//Display all #/Type entries that are stored
			}
			if(pItem->aszNumber[numCounter-1] && (numCounter < 0)) {
				usScrollLine += MldMnemNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_SKUNO_ADR, pItem->aszNumber[numCounter-1], MLD_PRECHECK_SYSTEM);
			}
		} else {
			for(numCounter=0; numCounter < NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop displaying
					break;
				}//Display all #/Type entries that are stored
			}
			if(pItem->aszNumber[numCounter-1] && (numCounter < 0)){
				usScrollLine += MldNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszNumber[numCounter-1], MLD_PRECHECK_SYSTEM);
			}
		}
	}
//End US Customs Operation

    /* -- send child plu? -- */
    if (uchMldSystem2 != MLD_SPLIT_CHECK_SYSTEM) {
		USHORT  i;
		TCHAR   auchDummy[NUM_PLU_LEN] = {0};

        for ( i = 0; i < pItem->uchChildNo; i++) {
            /* check condiment PLU exist or not */
            if (_tcsncmp(pItem->Condiment[i].auchPLUNo, auchDummy, NUM_PLU_LEN) == 0) {
                continue;
            }

            usScrollLine += MldChild(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->Condiment[i].uchAdjective, pItem->Condiment[i].aszMnemonic, MLD_DRIVE_THROUGH_3SCROLL);
        }
    }

    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_DR3COLUMN, &alAttrib[0][0]));
}

/****** End of Source ******/
