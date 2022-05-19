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
* Program Name: MLDDEPT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*           MldDeptPLU() : display dept/plu sales on scroll area
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
** Format  : SHORT    MldDeptPLU(ITEMSALES *pItem, USHORT usType);
*
*   Input  : ITEMSALES        *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays dept sales
*===========================================================================
*/
SHORT MldDeptPLU(ITEMSALES *pItem, USHORT usType, USHORT usScroll)
{
    if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {
        return (MldDriveDeptPLU(pItem, usType, usScroll));
    } else {
        return (MldPrechkDeptPLU(pItem, usType, usScroll));
    }
}

/*
*===========================================================================
** Format  : SHORT    MldPrechkDeptPLU(ITEMSALES *pItem, USHORT usType);
*
*   Input  : ITEMSALES        *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays dept sales
*===========================================================================
*/
SHORT MldPrechkDeptPLU(ITEMSALES *pItem, USHORT usType, USHORT usScroll)
{
	TCHAR  aszScrollBuff[24][MLD_PRECOLUMN1 + 1] = { 0 }; /* scroll data save area */
    LONG   alAttrib[24][MLD_PRECOLUMN1 + 1];                /* reverse video control flag area */
    USHORT usScrollLine = 0;            /* number of lines to be printed */
    ULONG  flPrintStatus;
    USHORT usPrice;
	SHORT  numCounter; //US Customs cwunn 4/10/03

	if ((pItem->fbModifier & VOID_MODIFIER) && (MldCheckReverseVideo())) {
        /* --- reverse video if void operation ---- */
        memset(&alAttrib[0][0], MLD_ATTRIB_REV_VIDEO, sizeof(alAttrib));
    } else {
		if(pItem->uchColorPaletteCode)
		{
			memset(&alAttrib[0][0], pItem->uchColorPaletteCode, sizeof(alAttrib));
		}else
		{
			memset(&alAttrib[0][0], MLD_ATTRIB_STD_VIDEO, sizeof(alAttrib));
		}
    }

    /* -- set void mnemonic and number -- */
    usScrollLine += MldVoidOffline(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->fbModifier, MLD_PRECHECK_SYSTEM);
	//US Customs SCER cwunn 4/8/03
	if(!(CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2))){ //if US Cutsoms #/Type function turned off (turned on is lower down)
		//Display #/Type entry before all other item information (Saratoga), parameterized in US Customs SCER cwunn 4/8/03
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter < NUM_OF_NUMTYPE_ENT; numCounter++){
				usScrollLine += MldMnemNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_SKUNO_ADR, pItem->aszNumber[numCounter-1], MLD_PRECHECK_SYSTEM);
				if(pItem->aszNumber[numCounter][0] == '\0') {//empty slot found, stop displaying
					break;
				}
			}
		} else {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){
				usScrollLine += MldNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszNumber[numCounter], MLD_PRECHECK_SYSTEM);
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop displaying
					break;
				}
			}
		}
	}
	//End Display #/Type entry

    /* -- set tax modifier mnemonic -- */
    usScrollLine += MldTaxMod(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], 0, pItem->fbModifier, MLD_PRECHECK_SYSTEM);

    /* -- scalable ? -- */
    if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
        /* read mdc status */
        if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT0)) {
            flPrintStatus = CURQUAL_SCALE3DIGIT;
        } else {
            flPrintStatus = 0L;
        }

        /* -- manual weight input -- */
        if (pItem->fsPrintStatus & PRT_MANUAL_WEIGHT) {
            /* -- set manual scale sales mnemonic -- */
            usScrollLine += MldTranMnem(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_MSCALE_ADR, MLD_PRECHECK_SYSTEM);
        }
        /* -- set weight and unit price -- */
        usScrollLine += MldWeight(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], flPrintStatus, pItem, MLD_PRECHECK_SYSTEM);
    }

    /* display price of dept/plu at precheck/post check system */
    if (CliParaMDCCheck(MDC_DISPLAY_ADR, ODD_MDC_BIT1)) {
        usPrice = 0;
    } else {
        usPrice = 1;
    }

	//SR 143 @/For key adds modified display lines cwunn 8/29/03
	if(pItem->usFor){
		//set mnemonics for @/For key (## @ ## For $##.##)
		//The parameter MLD_PRECHECK_SYSTEM designates What type of Receipt window
		//The parameter usType is passed as a part of the US Customs SCER
		usScrollLine += MldAtFor(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem, usPrice, &alAttrib[MLD_SETLINE_BASE(usScrollLine)][0], MLD_PRECHECK_SYSTEM, usType );
		usScrollLine += MldItems(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem, usPrice, &alAttrib[MLD_SETLINE_BASE(usScrollLine)][0], MLD_PRECHECK_SYSTEM, usType );
	}
	else{
		/* -- set mnemonics (adjective, PLU, print mod., condiment) -- */
		//The parameter MLD_PRECHECK_SYSTEM designates What type of Receipt window
		//The parameter usType is passed as a part of the US Customs SCER
		usScrollLine += MldItems(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem, usPrice, &alAttrib[MLD_SETLINE_BASE(usScrollLine)][0], MLD_PRECHECK_SYSTEM, usType );
	}


    /* display link plu, saratoga */

    if (pItem->usLinkNo) {
		DCURRENCY   lPrice;
		LONG        lQTY;

		/* -- set void mnemonic and number -- */
        usScrollLine += MldVoidOffline(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->fbModifier, MLD_PRECHECK_SYSTEM);
        /* -- set tax modifier mnemonic -- */
        usScrollLine += MldTaxMod(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], 0, pItem->fbModifier, MLD_PRECHECK_SYSTEM);

        lPrice = pItem->Condiment[0].lUnitPrice;

        if ((pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) == 0) {
            /* set qty by void consolidation */
            if (MldChkVoidConsolidation(pItem)) {
                lQTY = (LONG)pItem->sCouponQTY;
            } else {
                lQTY = pItem->lQTY/1000L;
            }
        } else {
            if (pItem->lQTY >= 0) {
                lQTY = 1;
            } else {
                lQTY = -1;
            }
        }
        lPrice *= lQTY;
        usScrollLine += MldLinkPlu(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], FALSE,
          pItem->Condiment[0].uchAdjective, pItem->Condiment[0].aszMnemonic, lQTY, lPrice, MLD_PRECHECK_SYSTEM);
    }

	if(CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)){ //if US Cutsoms #/Type function turned on
		//Display #/Type after all other Item information US Customs SCER cwunn 4/2/2003
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			//if redisplaying at total screen using 'display all transaction'
			if((usType & MLD_TRANS_DISPLAY) == MLD_TRANS_DISPLAY) {
				for(numCounter=0; numCounter < NUM_OF_NUMTYPE_ENT; numCounter++){
					usScrollLine += MldMnemNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_SKUNO_ADR, pItem->aszNumber[numCounter], MLD_PRECHECK_SYSTEM);
				}
			}
			else { //else add a last line to scroll region
				for(numCounter=0; numCounter < NUM_OF_NUMTYPE_ENT; numCounter++){
					if(pItem->aszNumber[numCounter][0] == '\0') {//empty slot found, stop displaying
						break;
					}//Display last #/Type entries
				}
				
				if(pItem->aszNumber[numCounter-1] && (numCounter > 0)) {
					usScrollLine += MldMnemNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_SKUNO_ADR, pItem->aszNumber[numCounter-1], MLD_PRECHECK_SYSTEM);
				}
			}
		} else { //if redisplaying at total screen using 'display all transaction'
			if( ((usType & MLD_TRANS_DISPLAY) == MLD_TRANS_DISPLAY) || (pItem->uchDiscountSignifier & DISC_SIG_DISCOUNT) ) {
				for(numCounter=0; numCounter < NUM_OF_NUMTYPE_ENT; numCounter++) {
					usScrollLine += MldNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszNumber[numCounter], MLD_PRECHECK_SYSTEM);
				}
			}
			else {
				if(usType == MLD_REVERSE_DISPLAY){ //display all #/type entries US Customs cwunn
					for(numCounter=0; numCounter < NUM_OF_NUMTYPE_ENT; numCounter++) {
						usScrollLine += MldNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszNumber[numCounter], MLD_PRECHECK_SYSTEM);
					}
				} else {
					for(numCounter=0; numCounter < NUM_OF_NUMTYPE_ENT; numCounter++) {
						if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop displaying
							break;
						}
						usScrollLine += MldNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszNumber[numCounter], MLD_PRECHECK_SYSTEM);
					}
				}
			}
		}
	}
	//End Display #/Type US Customs

    /* -- set cursor position & display all data in the buffer -- */

	//for use with condiment editing. JHHJ
 
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_PRECOLUMN1, &alAttrib[0][0]));

}

/*
*===========================================================================
** Format  : SHORT    MldPrechkDeptPLU(ITEMSALES *pItem, USHORT usType);
*
*   Input  : ITEMSALES        *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays dept sales
*===========================================================================
*/
SHORT MldDriveDeptPLU(ITEMSALES *pItem, USHORT usType, USHORT usScroll)
{
	TCHAR  aszScrollBuff[24 * (MLD_PRECOLUMN1 + 1)] = { 0 }; /* scroll data save area */
	TCHAR  *paszScrollBuff = aszScrollBuff;      /* scroll data save area */
	LONG   alAttrib[24 * (MLD_PRECOLUMN1 + 1)];  /* reverse video control flag area */
	LONG   *palAttrib = alAttrib;                /* reverse video control flag area */
	USHORT usScrollLine = 0;            /* number of lines to be printed */
    ULONG  flPrintStatus;
	USHORT	usPrice = 0, usWriteLength = 0;
	SHORT numCounter; //US Customs cwunn 4/10/03

	// Determine the lenght of each line of the display.
	// Since the MldWriteScrollDisplay() functionality will use this
	// line length to step through the buffer, we will put the lines
	// of text into the buffer using the same method.  We add one
	// to the line length so that as we increment our buffer pointer
	// it will point to the beginning of the next line.
	// We will subtract one from the line length before actually
	// calling MldWriteScrollDisplay() to write out the lines of text.
	if(CliParaMDCCheck(MDC_DEPT3_ADR, ODD_MDC_BIT3) &&
		(uchMldSystem != MLD_DRIVE_THROUGH_3SCROLL))
	{
		usWriteLength = MLD_PRECOLUMN1 + 1;
	} else
	{
		usWriteLength = MLD_DR3COLUMN + 1;
	}

    if ((pItem->fbModifier & VOID_MODIFIER) && (MldCheckReverseVideo())) {
        /* --- reverse video if void operation ---- */
        memset(&alAttrib[0], MLD_ATTRIB_REV_VIDEO, sizeof(alAttrib));
    } else {
		if(pItem->uchColorPaletteCode)
		{
			memset(&alAttrib[0], pItem->uchColorPaletteCode, sizeof(alAttrib));
		}else
		{
			memset(&alAttrib[0], MLD_ATTRIB_STD_VIDEO, sizeof(alAttrib));
		}
    }

    /* -- set void mnemonic and number -- */
    usScrollLine += MldVoidOffline(paszScrollBuff, pItem->fbModifier, MLD_DRIVE_THROUGH_3SCROLL);

	// We are are always doing is to calculate a pointer using the number of lines
	// of text currently in the buffer, indicated by usScrollLine, and then calculating
	// a new pointer as an offset within the buffer.  We must keep both the text
	// buffer pointer and the attribute buffer pointer synchronized with the same
	// offset.
	paszScrollBuff = aszScrollBuff + (MLD_SETLINE_BASE(usScrollLine) * usWriteLength);
	palAttrib = alAttrib + (MLD_SETLINE_BASE(usScrollLine) * usWriteLength);

	if(!(CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2))){
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for (numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++) {//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter] == '\0') { //empty slot found, stop displaying
					break;
				}//Display all #/Type entries that are stored

				usScrollLine += MldMnemNumber(paszScrollBuff, TRN_SKUNO_ADR, pItem->aszNumber[numCounter], MLD_DRIVE_THROUGH_3SCROLL);
				paszScrollBuff = aszScrollBuff + (MLD_SETLINE_BASE(usScrollLine) * usWriteLength);
				palAttrib = alAttrib + (MLD_SETLINE_BASE(usScrollLine) * usWriteLength);
			}
		} else {
			for (numCounter=0; numCounter < NUM_OF_NUMTYPE_ENT; numCounter++){//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0') { //empty slot found, stop displaying
					break;
				}//Display all #/Type entries that are stored
				usScrollLine += MldNumber(paszScrollBuff, pItem->aszNumber[numCounter], MLD_DRIVE_THROUGH_3SCROLL);
				paszScrollBuff = aszScrollBuff + (MLD_SETLINE_BASE(usScrollLine) * usWriteLength);
				palAttrib = alAttrib + (MLD_SETLINE_BASE(usScrollLine) * usWriteLength);
			}
		}
	}

    /* -- set tax modifier mnemonic -- */
    usScrollLine += MldTaxMod (paszScrollBuff, 0, pItem->fbModifier, MLD_DRIVE_THROUGH_3SCROLL);
	paszScrollBuff = aszScrollBuff + (MLD_SETLINE_BASE(usScrollLine) * usWriteLength);
	palAttrib = alAttrib + (MLD_SETLINE_BASE(usScrollLine) * usWriteLength);

    /* -- scalable ? -- */
    if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
        /* read mdc status */
        if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT0)) {
            flPrintStatus = CURQUAL_SCALE3DIGIT;
        } else {
            flPrintStatus = 0L;
        }

        /* -- manual weight input -- */
        if (pItem->fsPrintStatus & PRT_MANUAL_WEIGHT) {
            /* -- set manual scale sales mnemonic -- */
            usScrollLine += MldTranMnem (paszScrollBuff, TRN_MSCALE_ADR, MLD_DRIVE_THROUGH_3SCROLL);
			paszScrollBuff = aszScrollBuff + (MLD_SETLINE_BASE(usScrollLine) * usWriteLength);
			palAttrib = alAttrib + (MLD_SETLINE_BASE(usScrollLine) * usWriteLength);
        }
        /* -- set weight and unit price -- */
        usScrollLine += MldWeight (paszScrollBuff, flPrintStatus, pItem, MLD_DRIVE_THROUGH_3SCROLL);
		paszScrollBuff = aszScrollBuff + (MLD_SETLINE_BASE(usScrollLine) * usWriteLength);
		palAttrib = alAttrib + (MLD_SETLINE_BASE(usScrollLine) * usWriteLength);
    }

    /* -- set mnemonics (adjective, PLU, print mod., condiment) -- */
    if (uchMldSystem2 == MLD_SPLIT_CHECK_SYSTEM) {
        if (usScroll != MLD_SCROLL_1) {
            pItem->uchSeatNo = 0;   /* not display seat no at split check */
        }
    }
    usScrollLine += MldItems (paszScrollBuff, pItem, 0, palAttrib, MLD_DRIVE_THROUGH_3SCROLL, usType);
	paszScrollBuff = aszScrollBuff + (MLD_SETLINE_BASE(usScrollLine) * usWriteLength);
	palAttrib += (MLD_SETLINE_BASE(usScrollLine) * usWriteLength);

	if((CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2))){
		if (pItem->fbModifier2 & SKU_MODIFIER) {
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++)
			{//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter] == '\0'){ //empty slot found, stop displaying
					break;
				}//Display all #/Type entries that are stored
				usScrollLine += MldMnemNumber (paszScrollBuff, TRN_SKUNO_ADR, pItem->aszNumber[numCounter], MLD_DRIVE_THROUGH_3SCROLL);
				paszScrollBuff = aszScrollBuff + (MLD_SETLINE_BASE(usScrollLine) * usWriteLength);
				palAttrib = alAttrib + (MLD_SETLINE_BASE(usScrollLine) * usWriteLength);
			}
		} else 
		{
			for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++)
			{//US Customs cwunn 4/21/03
				if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found, stop displaying
					break;
				}//Display all #/Type entries that are stored
			
				usScrollLine += MldNumber (paszScrollBuff, pItem->aszNumber[numCounter], MLD_DRIVE_THROUGH_3SCROLL);
				paszScrollBuff = aszScrollBuff + (MLD_SETLINE_BASE(usScrollLine) * usWriteLength);
				palAttrib = alAttrib + (MLD_SETLINE_BASE(usScrollLine) * usWriteLength);
			}
		}
	}
    /* display link plu, saratoga */

    if (pItem->usLinkNo) {
		DCURRENCY   lPrice;
		LONG        lQTY;

		/* -- set void mnemonic and number -- */
        usScrollLine += MldVoidOffline (paszScrollBuff, pItem->fbModifier, MLD_DRIVE_THROUGH_3SCROLL);
		paszScrollBuff = aszScrollBuff + (MLD_SETLINE_BASE(usScrollLine) * usWriteLength);
		palAttrib = alAttrib + (MLD_SETLINE_BASE(usScrollLine) * usWriteLength);

        /* -- set tax modifier mnemonic -- */
        usScrollLine += MldTaxMod (paszScrollBuff, 0, pItem->fbModifier, MLD_DRIVE_THROUGH_3SCROLL);
		paszScrollBuff = aszScrollBuff + (MLD_SETLINE_BASE(usScrollLine) * usWriteLength);
		palAttrib = alAttrib + (MLD_SETLINE_BASE(usScrollLine) * usWriteLength);

        lPrice = pItem->Condiment[0].lUnitPrice;
        if ((pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) == 0) {
            /* set qty by void consolidation */
            if (MldChkVoidConsolidation(pItem)) {
                lQTY = (LONG)pItem->sCouponQTY;
            } else {
                lQTY = pItem->lQTY/1000L;
            }
        } else {
            if (pItem->lQTY >= 0) {
                lQTY = 1;
            } else {
                lQTY = -1;
            }
        }
        lPrice *= lQTY;
        usScrollLine += MldLinkPlu(paszScrollBuff, FALSE, pItem->Condiment[0].uchAdjective, pItem->Condiment[0].aszMnemonic, lQTY, lPrice, MLD_DRIVE_THROUGH_3SCROLL);
    }

    /* -- set cursor position & display all data in the buffer -- */
	usWriteLength--; // subtract one from the length which we added at the beginning.
    return (MldWriteScrollDisplay(aszScrollBuff, usScroll, usType, usScrollLine, usWriteLength, &alAttrib[0]));
}

/****** End of Source ******/
