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
* Nov-16-95 : 03.01.00 :  M.Ozawa   : Initial
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
#include <stdlib.h>
#include <string.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <pif.h>
#include <regstrct.h>
#include <transact.h>
#include <paraequ.h> 
#include <para.h>
#include <csstbpar.h>
#include <uie.h>
#include <fsc.h>
#include <rfl.h>
#include <display.h>
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
	REGDISPMSG         DisplayData = { 0 };
    USHORT             uchCond, uchSetCond;
    SHORT              i;
	TCHAR              auchDummy[NUM_PLU_LEN] = { 0 };
 
    /* set current condiment position */
    uchSetCond = (pItem->uchCondNo  + pItem->uchPrintModNo );

    if (pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2] & PLU_SCALABLE) {    
        /*--- DISPLAY DECIMAL DATA ---*/
        DisplayData.uchMajorClass = CLASS_REGDISP_SALES;
    
        if (CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT0)) {        /* address 29, bit 0 */
            DisplayData.uchMinorClass = CLASS_REGDISP_SCALE3DIGIT;
        } else {
            DisplayData.uchMinorClass = CLASS_REGDISP_SCALE2DIGIT;
        }

        /*--- DISPLAY LB/ KG MNEMONICS ---*/
		RflGetSpecMnem (DisplayData.aszSPMnemonic, SPC_LB_ADR);  /* special mnemonics add. 25 */
        DisplayData.lQTY = pItem->lQTY;

    } else {                                /* non scalable */
        DisplayData.uchMajorClass = CLASS_UIFREGSALES;
        DisplayData.uchMinorClass = CLASS_UIDEPT;

        /* void consolidation */
        if ((pItem->usItemOffset == 0) ||
            (pItem->uchMinorClass == CLASS_DEPTMODDISC) ||
            (pItem->uchMinorClass == CLASS_PLUMODDISC) ||
            (pItem->uchMinorClass == CLASS_SETMODDISC) ||
            (pItem->uchMinorClass == CLASS_OEPMODDISC) ||
            (pItem->uchPM) ||   /* saratoga */
            (pItem->uchPPICode)) {

            DisplayData.lQTY = pItem->lQTY;
        } else {
            DisplayData.lQTY = (LONG)pItem->sCouponQTY * 1000L;
        }
        if (labs(DisplayData.lQTY) == 1000L) {
            DisplayData.lQTY = 0L;        /* don't display QTY */
        }
    }

    switch (pItem->uchMinorClass) {
    case CLASS_DEPTMODDISC:             /* dept modifiered disc. */
    case CLASS_PLUMODDISC:              /* plu modifiered disc. */
    case CLASS_OEPMODDISC:              /* PLU w/ order entry prompt */
		RflGetTranMnem (DisplayData.aszMnemonic, TRN_MODID_ADR);   /* trans. mnemonics add. for Mod. item disc. */
        DisplayData.lAmount = pItem->lDiscountAmount; /* set discount amount */
        break;

    default:
        if (usType == MLD_NEXT_CONDIMENT) {
            /* display next condiment */
            if (uchSetCond) {
                uchCond = MldScroll1Buff.uchDispCond;
                if (uchCond < uchSetCond) {
                    MldSetCondiment(DisplayData.aszMnemonic, pItem, uchCond);
                    MldScroll1Buff.uchDispCond = ++uchCond;
                } else {    /* last condiment is displayed */
                    return(MLD_SUCCESS);
                }
            } else {        /* noun is displayed */
                return(MLD_SUCCESS);
            }

        } else if (usType == MLD_BEFORE_CONDIMENT) {

            /* display before condiment */
            if (uchSetCond) {
                uchCond = MldScroll1Buff.uchDispCond;
                if (uchCond) {
                    uchCond--;
                    if (uchCond) {
                        MldSetCondiment(DisplayData.aszMnemonic, pItem, (UCHAR)(uchCond-1));
                    } else {
                        MldSetNoun(DisplayData.aszMnemonic, DisplayData.aszAdjMnem, pItem);
                    }
                    MldScroll1Buff.uchDispCond = uchCond;
                } else {    /* noun is displayed */
                    return(MLD_SUCCESS);
                }
            } else {        /* noun is displayed */
                return(MLD_SUCCESS);
            }
        } else {

            MldSetNoun(DisplayData.aszMnemonic, DisplayData.aszAdjMnem, pItem);
        }

        /* set plu amount with condiment price */
        if ((pItem->usItemOffset == 0) || (pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2] & PLU_SCALABLE) ||
            (pItem->uchPM) || (pItem->uchPPICode)) {

            DisplayData.lAmount = pItem->lProduct;
            for (i = pItem->uchChildNo; i < STD_MAX_COND_NUM; i++) {
                if (_tcsncmp(pItem->Condiment[i].auchPLUNo, auchDummy, NUM_PLU_LEN) != 0) {
                    DisplayData.lAmount += ((pItem->lQTY / 1000L) * pItem->Condiment[i].lUnitPrice);
                }
            }
        } else {
			D13DIGITS  d13Work;

            /* recalculate sales price from sCouponQTY */
            d13Work = pItem->lUnitPrice;
            for (i = pItem->uchChildNo; i < STD_MAX_COND_NUM; i++) {
                if (_tcsncmp(pItem->Condiment[i].auchPLUNo, auchDummy, NUM_PLU_LEN) != 0) {
                    d13Work += pItem->Condiment[i].lUnitPrice;
                }
            }
            /* --- calcurate product * qty --- */
            d13Work *= (LONG)pItem->sCouponQTY;
            DisplayData.lAmount =  d13Work;
        }
    }

    DisplayData.fbSaveControl = 1;        /* 0: not save data, 1: save display data */

    /* ---- set arrow symbol for scroll display ---- */
    if (uchSetCond) {
        if (usType == MLD_BEFORE_CONDIMENT) {
            if (MldScroll1Buff.uchDispCond == 0) {
                DisplayData.uchArrow = 0xAF;  /* right arrow */
            } else {
                DisplayData.uchArrow = 0xAE;  /* left arrow */
            }
        } else {
            if (MldScroll1Buff.uchDispCond == uchSetCond) {
                DisplayData.uchArrow = 0xAE;  /* left arrow */
            } else {
                DisplayData.uchArrow = 0xAF;  /* right arrow */
            }
        }
    } else {
        DisplayData.uchArrow = MldSetScrollSymbol(usType);
    }

    DisplayData.uchFsc = pItem->uchSeatNo;  /* display seat no. */

    if (pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_0] & PLU_MINUS) {
        flDispRegDescrControl |= CRED_CNTRL;    /* turn on CREDIT descriptor control flag */
    } 

    if (pItem->fbModifier & TAX_MOD_MASK) {  /* tax status is on */
        flDispRegDescrControl |= TAXMOD_CNTRL;    /* turn on descriptor control flag for condiment */
        flDispRegKeepControl |= TAXMOD_CNTRL;    /* turn on descriptor control flag for condiment */
    } 
    
    if (pItem->fbModifier & VOID_MODIFIER) {
        flDispRegDescrControl |= VOID_CNTRL;    /* turn on descriptor control flag for condiment */
        flDispRegKeepControl |= VOID_CNTRL;    /* turn on descriptor control flag for condiment */
    } 

    if ((pItem->uchMinorClass == CLASS_DEPTITEMDISC) ||
        (pItem->uchMinorClass == CLASS_PLUITEMDISC) ||
        (pItem->uchMinorClass == CLASS_OEPITEMDISC) ||     /* R3.0 */
        (pItem->uchMinorClass == CLASS_SETITEMDISC) ||
        (pItem->uchMinorClass == CLASS_DEPTMODDISC) ||
        (pItem->uchMinorClass == CLASS_PLUMODDISC) ||
        (pItem->uchMinorClass == CLASS_OEPMODDISC) ||     /* R3.0 */
        (pItem->uchMinorClass == CLASS_SETMODDISC)) {

        flDispRegDescrControl |= ITEMDISC_CNTRL;
        flDispRegKeepControl |= ITEMDISC_CNTRL;
    }

    if ((pItem->uchMinorClass == CLASS_DEPT) || (pItem->uchMinorClass == CLASS_DEPTMODDISC)) { 
        flDispRegKeepControl &= ~(TAXMOD_CNTRL | VOID_CNTRL | ITEMDISC_CNTRL);
    }

    /*--- CALL DISPLAY MODULE ---*/
    DispWrite(&DisplayData);    
    
    /*--- TURN OFF DESCRIPTOR CONTROL FLAG ---*/
    flDispRegDescrControl &=~ (CRED_CNTRL | VOID_CNTRL | ITEMDISC_CNTRL | TAXMOD_CNTRL | ADJ_CNTRL);  /* for 10N10D display */

    /*--- TURN OFF KEEP CONTROL FLAG ---*/
    flDispRegKeepControl &=~ (CRED_CNTRL | VOID_CNTRL | ITEMDISC_CNTRL | TAXMOD_CNTRL | ADJ_CNTRL);  /* for 10N10D display */

    MldScroll1Buff.uchSetCond = uchSetCond;

    return(MLD_SUCCESS);

    usScroll = usScroll;
}

/*
*===========================================================================
** Format   : VOID MldSetNoun( UCHAR *pszMnemonic,
*                                   ITEMSALES *pItem,
*                                   UCHAR  uchDispCond)
*
*   Input   : ITEMSALES *pItem      - address of source sales item
*             UCHAR     uchDispCond - target condiment
*
*   Output  : UCHAR     *pszMnemonic- destination buffer address
*
*   InOut   : none
** Return   : none
*
** Synopsis : This function prepares print modifiers and condiments.
*===========================================================================
*/
VOID MldSetNoun( TCHAR *pszMnemonic, TCHAR *pszAdjMnem, ITEMSALES *pItem)
{
    PARAADJMNEMO       AdjMnemRcvBuff;

    _tcsncpy(pszMnemonic, pItem->aszMnemonic, PARA_PLU_LEN);
        
    if (pItem->uchAdjective != 0 && pItem->uchMinorClass != CLASS_ITEMORDERDEC) {
        memset(AdjMnemRcvBuff.aszMnemonics, '\0', PARA_ADJMNEMO_LEN + 1);
        AdjMnemRcvBuff.uchMajorClass = CLASS_PARAADJMNEMO;                             
        AdjMnemRcvBuff.uchAddress = pItem->uchAdjective;       
                                            /* Address of Adjective Mnemonics */
        CliParaRead(&AdjMnemRcvBuff);
        
        _tcsncpy(pszAdjMnem, AdjMnemRcvBuff.aszMnemonics, PARA_ADJMNEMO_LEN);
    }
}

/*
*===========================================================================
** Format   : VOID MldSetCondiment( UCHAR *pszMnemonic,
*                                   ITEMSALES *pItem,
*                                   UCHAR  uchDispCond)
*
*   Input   : ITEMSALES *pItem      - address of source sales item
*             UCHAR     uchDispCond - target condiment
*
*   Output  : UCHAR     *pszMnemonic- destination buffer address
*
*   InOut   : none
** Return   : none
*
** Synopsis : This function prepares print modifiers and condiments.
*===========================================================================
*/
VOID MldSetCondiment( TCHAR *pszMnemonic, ITEMSALES *pItem, UCHAR uchDispCond)
{
    BOOL        fIsDeptItem;
    PARAPRTMODI PrtMod;
    USHORT      usMaxChild;
    USHORT      usI, usSetCond = 0;
    USHORT      usPriority = 0, usPrintMod;
    TCHAR       auchDummy[NUM_PLU_LEN];

    memset(auchDummy, 0, sizeof(auchDummy));

    /* ---- check mdc of print modifier & condiment plu priority --- */
    if ( CliParaMDCCheck( MDC_PRINTPRI_ADR, EVEN_MDC_BIT0 ) && CliParaMDCCheck( MDC_PRINTPRI_ADR, EVEN_MDC_BIT2 )) {
         usPriority = 1;
    }

    /* --- determine source item is department item or not --- */
    fIsDeptItem = ((pItem->uchMinorClass == CLASS_DEPT) || (pItem->uchMinorClass == CLASS_DEPTITEMDISC) || (pItem->uchMinorClass == CLASS_DEPTMODDISC));

    /* --- prepare print modifiers and condiment items --- */
    PrtMod.uchMajorClass = CLASS_PARAPRTMODI;

    usMaxChild = ( pItem->uchChildNo + pItem->uchCondNo + pItem->uchPrintModNo );

	NHPOS_ASSERT(usMaxChild <= sizeof(pItem->Condiment)/sizeof(pItem->Condiment[0]));

    for (usPrintMod=0; usPrintMod<(USHORT)(usPriority?2:1); usPrintMod++) {
    for ( usI = 0; usI < usMaxChild; usI++ ) {
        if ((usPriority) && (usPrintMod)) {
            ;   /* if print modifier is already displayed */
        } else if ( pItem->auchPrintModifier[ usI ] ) {
            if (usI == (USHORT)uchDispCond) {
                /* --- prepare print modifier mnemonic --- */

                PrtMod.uchAddress = pItem->auchPrintModifier[ usI ];
                CliParaRead( &PrtMod );

                _tcsncpy(pszMnemonic, PrtMod.aszMnemonics, PARA_PRTMODI_LEN);
                usSetCond = 1;
                break;
            }
        }

        if ((usPriority) && (!usPrintMod)) {
            ;   /* if print modifier is not displayed */
        } else if (_tcsncmp(pItem->Condiment[usI].auchPLUNo, auchDummy, NUM_PLU_LEN) != 0) {
            /* --- prepare condiment item's mnemonic --- */
            if ((usI - pItem->uchChildNo) == (usPriority ? (uchDispCond-pItem->uchPrintModNo) : uchDispCond)) {
                _tcsncpy(pszMnemonic, pItem->Condiment[ usI ].aszMnemonic, PARA_PLU_LEN);
                usSetCond = 1;
                break;

            } /* end if */
        } /* end if else */
    } /* end for */
    if (usSetCond) break;
    }

}

/****** End of Source ******/
