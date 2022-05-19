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
* Title       : Multiline Display Department/PLU Set sales
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDSET.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*           MldSET() : display dept/plu set sales on scroll area
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
#include "termcfg.h"

/**
;============================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;============================================================================
**/
/*
*===========================================================================
** Format  : SHORT    MldSET(ITEMSALES *pItem, USHORT usType);
*
*   Input  : ITEMSALES        *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays set sales.
*
*            See usage of CLASS_SETMENU and function ItemSalesSetMenu() which
*            used to implement AC116 Promotional Pricing functionality.
*
*            See also function PrtSET() which does somethign similar for receipt printing.
*===========================================================================
*/
SHORT MldSET(ITEMSALES *pItem, USHORT usType, USHORT usScroll)
{
    if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {
        return (MldDriveSET(pItem, usType, usScroll));
    } else {
        return (MldPrechkSET(pItem, usType, usScroll));
    }
}

/*
*===========================================================================
** Format  : SHORT    MldSET(ITEMSALES *pItem, USHORT usType);
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
SHORT MldPrechkSET(ITEMSALES *pItem, USHORT usType, USHORT usScroll)
{
	TCHAR  aszScrollBuff[19][MLD_PRECOLUMN1 + 1] = {0};  /* scroll data save area */
    LONG   alAttrib[19][MLD_PRECOLUMN1 + 1] = {0};       /* reverse video control flag area */
    USHORT usScrollLine = 0;            /* number of lines to be printed */
    USHORT i;
    USHORT usPrice;
	SHORT numCounter; //US Customs

    if ((pItem->fbModifier & VOID_MODIFIER) && (MldCheckReverseVideo())) {
        /* --- reverse video if void operation ---- */
        memset(&alAttrib[0][0], MLD_ATTRIB_REV_VIDEO, sizeof(alAttrib));
    } else {
        memset(&alAttrib[0][0], MLD_ATTRIB_STD_VIDEO, sizeof(alAttrib));
    }

    /* -- set void mnemonic and number -- */
    usScrollLine += MldVoidOffline(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->fbModifier, MLD_PRECHECK_SYSTEM);
//Preserve Saratoga operation in US Customs (1.4.2.2)
	if(!CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)) {
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter < NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0') {//empty slot found, stop displaying
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
			if(pItem->aszNumber[numCounter-1] && (numCounter < 0)) {
				usScrollLine += MldNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszNumber[numCounter-1], MLD_PRECHECK_SYSTEM);
			}
		}
	}
//End Saratoga operation.

    /* -- set tax modifier mnemonic -- */
    usScrollLine += MldTaxMod(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], 0, pItem->fbModifier, MLD_PRECHECK_SYSTEM);

/*******************
    / -- scalable ? -- /
    if (pItem->ControlCode.auchStatus[2] & PLU_SCALABLE) {

        / -- manual weight input -- /
        if (pItem->fsPrintStatus & PRT_MANUAL_WEIGHT) {

            / -- set manual scale sales mnemonic -- /
            usScrollLine += MldMnlWeight(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)]);

        }
        / -- set weight and unit price -- /
        usScrollLine += MldWeight(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TranModeQual.fsModeStatus, pItem);


    }
*****************/

    /* display price of dept/plu at precheck/post check system */
    if (CliParaMDCCheck(MDC_DISPLAY_ADR, ODD_MDC_BIT1)) {
        usPrice = 0;
    } else {
        usPrice = 1;
    }

    /* -- set mnemonics (adjective, PLU, print mod., condiment) -- */
    usScrollLine += MldItems(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem, usPrice, &alAttrib[MLD_SETLINE_BASE(usScrollLine)][0], MLD_PRECHECK_SYSTEM, usType);

//MDC controlled US Customs operation
	if(CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)){
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
				if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop displaying
					break;
				}//Display all #/Type entries that are stored
			}
			if(pItem->aszNumber[numCounter-1] && (numCounter < 0)){
				usScrollLine += MldNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszNumber[numCounter-1], MLD_PRECHECK_SYSTEM);
			}
		}
	}
//End US Customs

	if (!pItem->aszNumber[0][0]){ //if no #/Type entry send child PLU US Customs cwunn
		/* -- send child plu? -- */
		/* read mdc before transaction open */
		if (CliParaMDCCheck(MDC_DEPT2_ADR, EVEN_MDC_BIT0)) {
			TCHAR  auchDummy[NUM_PLU_LEN] = {0};

			for ( i = 0; i < pItem->uchChildNo; i++) {
				/* check condiment PLU exist or not */
				if (_tcsncmp(pItem->Condiment[i].auchPLUNo, auchDummy, NUM_PLU_LEN) == 0) {
					continue;
				}

				usScrollLine += MldChild(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->Condiment[i].uchAdjective, pItem->Condiment[i].aszMnemonic, MLD_PRECHECK_SYSTEM);
			}
		}
	}

    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_PRECOLUMN1, &alAttrib[0][0]));
}

/*
*===========================================================================
** Format  : SHORT    MldSET(ITEMSALES *pItem, USHORT usType);
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
SHORT MldDriveSET(ITEMSALES *pItem, USHORT usType, USHORT usScroll)
{
	TCHAR  aszScrollBuff[23][MLD_DR3COLUMN + 1] = {0};  /* scroll data save area */
    LONG   alAttrib[23][MLD_DR3COLUMN + 1] = {0};       /* reverse video control flag area */
    USHORT usScrollLine = 0;            /* number of lines to be printed */
    USHORT i;
	SHORT numCounter; //US Customs

    if ((pItem->fbModifier & VOID_MODIFIER) && (MldCheckReverseVideo())) {
        /* --- reverse video if void operation ---- */
        memset(&alAttrib[0][0], MLD_ATTRIB_REV_VIDEO, sizeof(alAttrib));
    } else {
        memset(&alAttrib[0][0], MLD_ATTRIB_STD_VIDEO, sizeof(alAttrib));
    }

    /* -- set void mnemonic and number -- */
    usScrollLine += MldVoidOffline(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->fbModifier, MLD_DRIVE_THROUGH_3SCROLL);

//Preserve Saratoga operation in US Customs (1.4.2.2)
	if(!CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)){
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for (numCounter = 0; numCounter < NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){//empty slot found, stop displaying
					break;
				}//Display all #/Type entries that are stored
			}
			if (pItem->aszNumber[numCounter-1] && (numCounter < 0)){
				usScrollLine += MldMnemNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_SKUNO_ADR, pItem->aszNumber[numCounter-1], MLD_DRIVE_THROUGH_3SCROLL);
			}
		} else {
			for (numCounter = 0; numCounter < NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs 4/21/03
				if (pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop displaying
					break;
				}//Display all #/Type entries that are stored
			}
			if (pItem->aszNumber[numCounter-1] && (numCounter < 0)){
				usScrollLine += MldNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszNumber[numCounter-1], MLD_DRIVE_THROUGH_3SCROLL);
			}
		}
	}
//End Saratoga operation

    /* -- set tax modifier mnemonic -- */
    usScrollLine += MldTaxMod(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], 0, pItem->fbModifier, MLD_DRIVE_THROUGH_3SCROLL);

    /* -- set mnemonics (adjective, PLU, print mod., condiment) -- */
    if (uchMldSystem2 == MLD_SPLIT_CHECK_SYSTEM) {
        if (usScroll != MLD_SCROLL_1) {
            pItem->uchSeatNo = 0;   /* not display seat no at split check */
        }
    }
    usScrollLine += MldItems(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem, 0, &alAttrib[MLD_SETLINE_BASE(usScrollLine)][0], MLD_DRIVE_THROUGH_3SCROLL, usType);

//MDC controlled US Customs operation (1.4.2.2)
	if(CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)){
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for (numCounter = 0; numCounter < NUM_OF_NUMTYPE_ENT; numCounter++) {//US Customs cwunn 4/21/03
				if (pItem->aszNumber[numCounter][0] == '\0') {//empty slot found, stop displaying
					break;
				}//Display all #/Type entries that are stored
			}
			if (pItem->aszNumber[numCounter-1] && (numCounter < 0)) {
				usScrollLine += MldMnemNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_SKUNO_ADR, pItem->aszNumber[numCounter-1], MLD_DRIVE_THROUGH_3SCROLL);
			}
		} else {
			for (numCounter=0; numCounter < NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs 4/21/03
				if (pItem->aszNumber[numCounter][0] == '\0') { //empty slot found, stop displaying
					break;
				}//Display all #/Type entries that are stored
			}
			if (pItem->aszNumber[numCounter-1] && (numCounter < 0)) {
				usScrollLine += MldNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszNumber[numCounter-1], MLD_DRIVE_THROUGH_3SCROLL);
			}
		}
	}
//End US Customs

    /* -- send child plu? -- */
    if (uchMldSystem2 != MLD_SPLIT_CHECK_SYSTEM) {  /* not display at split check */
		TCHAR  auchDummy[NUM_PLU_LEN] = {0};

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
