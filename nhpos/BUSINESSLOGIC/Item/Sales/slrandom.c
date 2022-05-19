/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170 GP  *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1996            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Random Weight Label Pre Check module
* Category    : Item Module, NCR 2170 US General Purpose Model Application
* Program Name: SLRANDOM.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Midium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: ItemSalesLabelPreCheck()  : Label Pre Check function
*           ItemSalesLabelDisplayNo() : Print Number on Label
*
* --------------------------------------------------------------------------
* Update Histories
*   Date    : Version   :   Name    : Description
* May-13-93 : 00.00.01  : M.Suzuki  : Initial
* May-24-93 : 00.00.02  : M.Suzuki  : Changed by Unit Test
* Jun-03-93 : 00.00.03  : M.Suzuki  : Changed by Unit Test 2
* Jun-21-93 : 00.00.04  : H.Mamiya  : Changed by Unit Test 4 & FVT
* Mar-07-94 :           : hkato     : random weight label
* Jun-10-94 : 01.01.05  : M.Ozawa   : Save random weight label of dept for repeat,e/c
* Sep-14-95 : 01.01.06  : M.Ozawa   : Correct last digit check of weight read.
* Oct-15-96 : 02.00.00  : T.Yatuhasi: Change ItemSalesLabelPreCheck
* Nov-25-96 : 02.00.00  : hrkato    : R2.0
*===========================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===========================================================================
*/
#include	<tchar.h>
#include    <memory.h>
#include    <string.h>
#include    "ecr.h"
#include    "uie.h"
#include    "pif.h"
#include    "log.h"
#include    "regstrct.h"
#include    "uireg.h"
#include    "transact.h"
#include    "item.h"
#include    "paraequ.h"
#include    "para.h"
#include    "csstbpar.h"
#include    "csop.h"
#include    "csstbopr.h"
#include    "display.h"
#include    "appllog.h"
#include    "csetk.h"
#include    "csstbetk.h"
#include    "cpm.h"
#include    "eept.h"
#include    "rfl.h"
#include    "fsc.h"
#include    "itmlocal.h"

/*
*===========================================================================
**Synopsis: SHORT  ItemSalesLabelPreCheck(UIFREGSALES *pUifRegSales,
*                                         ITEMSALES *pItemSales,
*                                         LABELANALYSIS *pPluLabel)
*    Input: UIFREGSALES   *pUifRegSales
*   Output: None
*    InOut: ITEMSALES     *pItemSales
*           LABELANALYSIS *pPluLabel
**Return:   ITM_SUCCESS       : Function Performed succesfully
*           LDT_SEQERR_ADR    : Key Sequence error
*
**Description: This module is the Lavel Check module.
*===========================================================================
*/
SHORT   ItemSalesLabelPreCheck(UIFREGSALES *pUifRegSales,
                          ITEMSALES *pItemSales, VOID *pVoid)
/* SHORT   ItemSalesLabelPreCheck(UIFREGSALES *pUifRegSales,
                          ITEMSALES *pItemSales, LABELANALYSIS *pPluLabel) */
{
    LABELANALYSIS *pPluLabel = pVoid;
    USHORT       usDept;
    UCHAR        uchMajorFsc, uchMinorFsc;
    SHORT        sStatus;
    USHORT       usControl;

    pItemSales->uchRndType |= ITM_TYPE_RANDOM;

    if (pPluLabel->uchLookup & LA_EXE_LOOKUP) {

        /* ----- Label is on MMM ----- */
        pItemSales->uchRndType |= ITM_TYPE_ARTNO;

        pItemSales->uchMinorClass = CLASS_PLU;
        _tcsncpy(pItemSales->auchPLUNo, pPluLabel->aszMaskLabel, NUM_PLU_LEN);

        if (pPluLabel->fsBitMap & LA_F_PRICE) {
            if (pUifRegSales->lUnitPrice) {
                return (LDT_SEQERR_ADR);
            }
            pUifRegSales->lUnitPrice = _ttol(pPluLabel->aszPrice);
            pItemSales->uchRndType |= ITM_TYPE_PRICE;

        } else if (pPluLabel->fsBitMap & LA_F_WEIGHT) {
			UCHAR       uchMDC;

            if (pUifRegSales->lQTY) {
                return (LDT_SEQERR_ADR);
            }
            if (ItemModLocalPtr->ScaleData.fbTareStatus & MOD_TARE_KEY_DEPRESSED) {
                return(LDT_TARENONSCALABLE_ADR);         /* tare key for non scalable item */
            }
            pUifRegSales->lQTY = _ttol(pPluLabel->aszPrice); /* set weight */

            /* get MDC conditions (Addr. 29) */
            uchMDC = CliParaMDCCheck(MDC_SCALE_ADR, (ODD_MDC_BIT0 | ODD_MDC_BIT2));

            if (uchMDC & ODD_MDC_BIT0) {            /* 2 digits auto scale */
                if (! (uchMDC & ODD_MDC_BIT2)) {    /* 0 or 5 option */
                    if (pUifRegSales->lQTY % ITM_SL_00OR05) {
                        return (LDT_PROHBT_ADR);
                    }
                }
            } else {                                /* 3 digits auto scale */

                pUifRegSales->lQTY *= ITM_SL_2DIGITS;   /* adjust weight */
#if 0
                /* R2.0 Start */
                if (pPluLabel->aszLookPlu[0] == '2' && pPluLabel->aszLookPlu[1] == '3') {
                    if(! (CliParaMDCCheck( MDC_PLULABEL1_ADR,ODD_MDC_BIT3)))
                        pUifRegSales->lQTY *= ITM_SL_2DIGITS;   /* adjust weight */

                } else if ( pPluLabel->aszLookPlu[0] == '2' && pPluLabel->aszLookPlu[1] == '4') {
                    if(! (CliParaMDCCheck( MDC_PLULABEL2_ADR,EVEN_MDC_BIT0)))
                        pUifRegSales->lQTY *= ITM_SL_2DIGITS;   /* adjust weight */

                } else if ( pPluLabel->aszLookPlu[0] == '2' && pPluLabel->aszLookPlu[1] == '5') {
                    if(! (CliParaMDCCheck( MDC_PLULABEL2_ADR,EVEN_MDC_BIT1)))
                        pUifRegSales->lQTY *= ITM_SL_2DIGITS;   /* adjust weight */

                } else {
                    pUifRegSales->lQTY *= ITM_SL_2DIGITS;       /* R2.0 */
                }
                /* R2.0 End   */
#endif
            }
            pItemSales->uchRndType |= ITM_TYPE_WEIGHT;

        } else if (pPluLabel->fsBitMap & LA_F_QUANTITY) {
            if (pUifRegSales->lQTY) {
                return (LDT_SEQERR_ADR);
            }
            if (ItemModLocalPtr->ScaleData.fbTareStatus & MOD_TARE_KEY_DEPRESSED) {
                return(LDT_TARENONSCALABLE_ADR);         /* tare key for non scalable item */
            }
            pUifRegSales->lQTY = _ttol(pPluLabel->aszPrice) * ITM_SL_QTY;
            pItemSales->uchRndType |= ITM_TYPE_QTY;
        }

    } else {
        /* ----- Label is not on MMM ----- */
        switch (pPluLabel->fsBitMap) {
        case (LA_F_PRICE | LA_F_DEPT):
            pItemSales->uchMinorClass = CLASS_DEPT;

            /* ----- check Price & Dept are entered or not ----- */
            if (pUifRegSales->lUnitPrice || pUifRegSales->usDeptNo) {
                return (LDT_SEQERR_ADR);
            }

            /* ----- check other key enter ----- */
            if ((pUifRegSales->fbModifier2) || (pUifRegSales->uchAdjective)) {
                return (LDT_SEQERR_ADR);
            }
            if ( ( usDept = (USHORT)_ttol(pPluLabel->aszDept) ) > MAX_DEPT_NO ) {
                return (LDT_SEQERR_ADR);
            }

            pUifRegSales->usDeptNo = usDept;
            pUifRegSales->lUnitPrice = _ttol(pPluLabel->aszPrice);

            /* copy random weight label to previous buffer */
            /* memcpy(pItemSales->auchPLUNo, pPluLabel->aszLookPlu, NUM_PLU_LEN); */
                    /* if dept sales, random weight label is not masked */

            pItemSales->uchRndType |= (ITM_TYPE_PRICE | ITM_TYPE_DEPT);
            break;

        case (LA_F_PRICE | LA_F_SKU_NO):
            pItemSales->uchMinorClass = CLASS_DEPT;

            /* ----- check Price is entered or not ----- */
            if (pUifRegSales->lUnitPrice) {
                return (LDT_SEQERR_ADR);
            }

            /* ----- check other key enter ----- */
            if ((pUifRegSales->fbModifier2) || (pUifRegSales->uchAdjective)) {
                return (LDT_SEQERR_ADR);
            }

            /* ----- get sales dept number ----- */
            usControl = UieNotonFile(UIE_ENABLE);                   /* disable using scanner */
            if (ItemSalesGetDeptNo(&usDept, &uchMajorFsc, &uchMinorFsc) == UIF_CANCEL) {
                UieNotonFile(usControl);                      /* enable scanner */
                return (UIF_CANCEL);
            }
            pUifRegSales->usDeptNo = usDept;
            pUifRegSales->uchMajorFsc = uchMajorFsc;
            pUifRegSales->uchFsc = uchMinorFsc;
            pUifRegSales->lUnitPrice = _ttol(pPluLabel->aszPrice);

            /* ----- save SKU # to print later ----- */
            _tcsncpy(pUifRegSales->aszNumber, pPluLabel->aszLabelNumber, LA_LEN_SKU5);
            pUifRegSales->fbModifier2 |= SKU_MODIFIER;

            /* copy random weight label to previous buffer */
            /* memcpy(pItemSales->auchPLUNo, pPluLabel->aszLookPlu, NUM_PLU_LEN); */
                    /* if dept sales, random weight label is not masked */

            pItemSales->uchRndType |= (ITM_TYPE_PRICE | ITM_TYPE_NUMBER);
            UieNotonFile(usControl);                      /* enable scanner */
            break;

        case LA_F_NUMBER:
            pItemSales->uchMinorClass = 0;
            _tcsncpy(pUifRegSales->aszNumber, pPluLabel->aszLabelNumber, LA_LEN_NUMBER);
            /* return to UIFREG */
            return(UIF_NUMBER);

        case LA_F_SKU_NO:
            pItemSales->uchMinorClass = 0;
            _tcsncpy(pUifRegSales->aszNumber, pPluLabel->aszLabelNumber, LA_LEN_NUMBER);
            /* return to UIFREG */
            return(UIF_SKUNUMBER);

        case LA_F_CONS_NO:
            pItemSales->uchMinorClass = 0;

            /* ----- check any data is entered or not ----- */
            if ((pUifRegSales->usDeptNo)
                                    || (pUifRegSales->lUnitPrice)
                                    || (pUifRegSales->uchAdjective)
                                    || (pUifRegSales->lQTY)
                                    || (pUifRegSales->fbModifier)
                                    || (pUifRegSales->fbModifier2)) {
                return (LDT_SEQERR_ADR);
            }

			sStatus = ItemCommonSetupTransEnviron ();
			if(sStatus != ITM_SUCCESS)
				return (sStatus);

            /* ----- print SKU/Consumer/Account Number directly ----- */
			{
				ITEMPRINT   ItemPrint = {0};
				REGDISPMSG  DispMsg = {0};

				ItemPrint.uchMajorClass = CLASS_ITEMPRINT;
				ItemPrint.uchMinorClass = CLASS_NUMBER;
	            
				_tcsncpy(ItemPrint.aszNumber[0], pPluLabel->aszLabelNumber, LA_LEN_NUMBER);

				if (pPluLabel->fsBitMap == LA_F_SKU_NO) {
					ItemPrint.usFuncCode = TRN_SKUNO_ADR;
				} else if (pPluLabel->fsBitMap == LA_F_CONS_NO) {
					ItemPrint.usFuncCode = TRN_CONSNO_ADR;
				} else {
					ItemPrint.usFuncCode = TRN_NUM_ADR;
				}
	            
				/* set print status */
				ItemPrint.fsPrintStatus = (PRT_JOURNAL | PRT_SLIP);
	                                            
				/* send data to transaction module */           
				if ( (sStatus = ItemPreviousItem( &ItemPrint, 0) ) != ITM_SUCCESS ) {
					return(sStatus);
				}

				/* ----- display Number ----- */
				DispMsg.uchMajorClass = CLASS_UIFREGDISP;   /* set display class */
				DispMsg.uchMinorClass = CLASS_UIFDISP_CONSNUMBER;   
				_tcsncpy(DispMsg.aszStringData, pPluLabel->aszLabelNumber, LA_LEN_NUMBER);     /* set number */
				DispWrite(&DispMsg);                    /* display */
			}

            break;
        }
    }
    return (ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesGetDeptNo(UIFDIADATA *pDiaData);
*
*   Input:  None
*
*   Output: None
*
*   InOut:  UIFDIADATA  *pDiaData
*
**Return: UIFREG_SUCCESS : Function Performed succesfully
*         UIFREG_CANCEL  : Function Canceled by user using cancel key
*
** Description: This function gets the Department number.
*
*===========================================================================
*/
SHORT   ItemSalesGetDeptNo(USHORT *pusDept, UCHAR *puchMajorFsc, UCHAR *puchMinorFsc)
{
    REGDISPMSG      DispMsg = {0};
    SHORT           sStatus;

    /* ----- display "ENTER DEPT" ----- */
    DispMsg.uchMajorClass = CLASS_REGDISPCOMPLSRY;
    DispMsg.uchMinorClass = CLASS_REGDISP_DEPTNO;
    RflGetLead (DispMsg.aszMnemonic, LDT_DEPTNO_ADR);

    flDispRegDescrControl |= COMPLSRY_CNTRL;
    flDispRegKeepControl |= COMPLSRY_CNTRL;
    DispMsg.fbSaveControl = 4;
        
    DispWrite(&DispMsg);
         
    flDispRegDescrControl &= ~COMPLSRY_CNTRL;
    flDispRegKeepControl &= ~COMPLSRY_CNTRL;

    do {
		UIFREGMODIFIER  ItemModifier = {0};
		UIFDIADATA      WorkUI = {0};
        
        if (UifDiaDept(&WorkUI)) {               /* req. Department #  */
            return (UIF_CANCEL);                 /* cancel by user */
        }
    
        if (WorkUI.auchFsc[0] == FSC_KEYED_DEPT) {
			// The Keyed Department functionality is from the AC4 PLU key assignment
			// functionality used by keyboard type interfaces. Touchscreen does not
			// use that approach.
            if (WorkUI.ulData) return LDT_KEYOVER_ADR;
            *puchMajorFsc = WorkUI.auchFsc[0];
            *puchMinorFsc = WorkUI.auchFsc[1];
            *pusDept = 0;
            return ITM_SUCCESS;
        } else if (WorkUI.auchFsc[0] == FSC_DEPT) {
			// standard Department Number entry using the Dept. Key.
            *pusDept = (USHORT)WorkUI.ulData;
            return ITM_SUCCESS;
        } else if (WorkUI.auchFsc[0] == FSC_MENU_SHIFT) {
            ItemModifier.uchMajorClass = CLASS_UIFREGMODIFIER;  /* set tax modifier class */
            ItemModifier.uchMinorClass = CLASS_UIMENUSHIFT;
            ItemModifier.uchMenuPage = (UCHAR)WorkUI.ulData; /* set input page # */
            sStatus = ItemModMenu(&ItemModifier);     /* Modifier Module */
            if (sStatus != ITM_SUCCESS) {
                return (sStatus);
            }
        } else if (WorkUI.auchFsc[0] == FSC_D_MENU_SHIFT) {
            ItemModifier.uchMajorClass = CLASS_UIFREGMODIFIER;  /* set tax modifier class */
            ItemModifier.uchMinorClass = CLASS_UIMENUSHIFT;
            ItemModifier.uchMenuPage = WorkUI.auchFsc[1]; /* set input page # */
            sStatus = ItemModMenu(&ItemModifier);     /* Modifier Module */
            if (sStatus != ITM_SUCCESS) {
                return (sStatus);
            }
        } else {
            return LDT_SEQERR_ADR;
        }
    } while (1);
    
    return(ITM_SUCCESS);
}

SHORT   ItemSalesGetDeptNoSans(USHORT *pusDept)
{
	UCHAR uchMajorFsc, uchMinorFsc;

	return ItemSalesGetDeptNo(pusDept,&uchMajorFsc, &uchMinorFsc);
}

/* ===== End of File ===== */
