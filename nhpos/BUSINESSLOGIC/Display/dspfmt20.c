/*
************************************************************************************
**                                                                                **
**        *=*=*=*=*=*=*=*=*                                                       **
**        *  NCR 2170     *             NCR Corporation, E&M OISO                 **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992                    **
**    <|\/~               ~\/|>                                                   **
**   _/^\_                 _/^\_                                                  **
**                                                                                **
************************************************************************************
*===================================================================================
* Title       : Display Module                         
* Category    : Display, NCR 2170 US Hospitality Application         
* Program Name: DSPFMT20.C
* ----------------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*       DispRegZeroMnemo() : display "0.00" and mnemonics
*       DispRegAmtMnemo() : display amount and mnemonics
*       DispRegRepeatAmtMnemo() : display amount and mnemonics w/ repeat co.
*       DispRegAmtString() : display amount and mnemonics(max 19 char.)
*       DispRegSales() : display any possible data
*           DispRegUpFmt() : decide upper display type 
*           DispRegUpFmt() : decide lower display type 
*           DispRegGetQty() : set the quantity data
*       DispRegScale() : display scalable item data
*       DispRegForeign() : display foreign amount
*       DispRegOrderNo() : display order No.
*       DispRegNumber() : display Number
*       DispRegQuantity() : display quantity
*       DispRegModeIn() : display for reg mode-in
*       DispRegLowMnemo() : display mnemonics for lower line
*       DispRegLock() : display in lock mode
*       DispRegSave() : save the display data for clear key if necessary  
* ----------------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-09-92: 00.00.01 : R.Itoh    : initial                                   
* Nov-09-93: 02.00.02 : K.You     : add charge posting feature.                                   
* Mar-22-95: 03.00.00 : M.Ozawa   : add DispRegRepeatAmtMnemo()
* Mar-27-95: 03.00.00 : M.Ozawa   : modify for LCD enhancement
*
** NCR2171 **                                         
* Aug-26-99: 01.00.00 : M.Teraki  :initial (for 2171)
*===================================================================================
*===================================================================================
* PVCS Entry
* ----------------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===================================================================================
*/
#include	<tchar.h>

/* #include    <memory.h> */
#include    <string.h>
/* #include    <stdio.h> */
#include    <stdlib.h>
/* #include    <math.h> */
#include    "ecr.h"
#include    "pif.h"
#include    "rfl.h"
#include    "uie.h"
#include    "paraequ.h"
#include    "para.h"
/* #include    <cscas.h> */
/* #include    <cswai.h> */
/* #include    <csop.h> */
/* #include    <csstbpar.h> */
#include    "maint.h"
#include    "display.h"
#include    "csstbpar.h"
#include    "disp20.h"

// If additional items are to be added to this table they must also be defined
// inf disp20.h under "CUSTOMER DISPLAY CONTROL DEFINITION.
static const TCHAR *pMnemonicOverride [] = {
	_T("  "),
	_T("ITEM"),	// C_DISP_MNE_ITEM
	_T("COUPON"),	//C_DISP_MNE_COUPON
	_T("DISCOUNT"),	//C_DISP_MNE_DISCOUNT
	_T("TOTAL"),	//C_DISP_MNE_TOTAL
	_T("TENDER"),	//C_DISP_MNE_TENDER
	_T("BLANK"),	//C_DISP_BLANK
	_T("CHANGE")	//C_DISP_MNE_CHANGE
};

/*
*===========================================================================
** Synopsis:    VOID DispRegZeroMnemo( REGDISPMSG *pData, USHORT fusC_Disp )
*                                     
*     Input:    *pData      - Pointer of the Display Data Buffer
*                fusC_Disp    - Customer Display control
*    Output:    nothing     
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This Function makes the display format which enables to 
*               display "0.00" , mnemonics, and current page No.
*
*   (e.g.)     operator display                          customer display
*
*          +----------------------+                       +------------+  
*          |                 0.00 |                       |       0.00 |
*          |----------------------|                       +------------+    
*          | ITOH               1 |                            
*          +----------------------+                            
*
*===========================================================================
*/
VOID DispRegZeroMnemo( REGDISPMSG *pData, USHORT fusC_Disp )  
{                                              
    static  const TCHAR    format20_OP[] = _T("\t%l$\n%s\t %c");
    static  const TCHAR    format10_CU[] = _T("\t%l$");    

    if (pData->fbSaveControl & DISP_SAVE_ECHO_ONLY) {
        return;
    }
    if (pData->uchArrow) {
        fusC_Disp = 0;
    }

    if (pData->aszAdjMnem[0] ==_T('\0')) {
        UiePrintf(UIE_OPER, 0, 0, format20_OP, DCLL(0), pData->aszMnemonic, DispSetPageChar(pData->uchArrow));
    } else {
        UiePrintf(UIE_OPER, 0, 0, format20_OP, DCLL(0), pData->aszAdjMnem, DispSetPageChar(pData->uchArrow));
    }    

    if ((C_DISP_MASK & fusC_Disp) == C_DISP) {
        if ((PifSysConfig()->uchCustType == DISP_2X20) || (PifSysConfig()->uchCustType == DISP_4X20)) { /* 2172 */
            UiePrintf(UIE_CUST, 0, 0, format20_OP, DCLL(0), pData->aszStringData, DISPPAGESPACE);
        } else if (PifSysConfig()->uchCustType == DISP_10N10D) {
            UiePrintf(UIE_CUST, 0, 0, format10_CU, DCLL(0));
        }
    }

    DispRegSave(pData);             /* save the display data if necessary */
}

/*
*===========================================================================
** Synopsis:    VOID DispRegAmtMnemo( REGDISPMSG *pData, USHORT fusC_Disp )
*                                     
*     Input:    *pData      - Pointer of the Display Data Buffer
*                fusC_Disp    - Customer Display control
*    Output:    nothing     
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This Function makes the display format which enables to 
*               display amount, mnemonics, and current page No. 
*
*   (e.g.)     operator display                          customer display
*
*          +----------------------+                       +------------+  
*          |                -1.00 |                       |      -1.00 |
*          |----------------------|                       +------------+    
*          | Item Disc.#1       1 |                            
*          +----------------------+                            
*
*===========================================================================
*/
VOID DispRegAmtMnemo( REGDISPMSG *pData, USHORT fusC_Disp )   
{                                               
    static  const TCHAR    format20_OP[] = _T("\t%l$\n%s\t%c");
    static  const TCHAR    format10_CU[] = _T("\t%l$");    

    if (pData->fbSaveControl & DISP_SAVE_ECHO_ONLY) {
        return;
    }                                                        
    if (pData->uchArrow) {
        fusC_Disp = 0;
    }

    UiePrintf(UIE_OPER, 0, 0, format20_OP, pData->lAmount, pData->aszMnemonic, DispSetPageChar(pData->uchArrow));

    if ((C_DISP_MASK & fusC_Disp) == C_DISP) {
		CONST TCHAR  *pMnemonic = pData->aszMnemonic;

		// Check if override mnemonic with hard coded string, MDC_PRINT_ITEM_MNEMO
		if ( CliParaMDCCheckField(MDC_PRINT_ITEM_MNEMO, MDC_PARAM_BIT_B) ) {
			int i = C_DISP_INDEX(fusC_Disp);
			pMnemonic = pMnemonicOverride [i];
		}
        if ((PifSysConfig()->uchCustType == DISP_2X20) ||
            (PifSysConfig()->uchCustType == DISP_4X20)) { /* 2172 */
            if (pData->aszStringData[0]) {    /* if special mnemonic is setted */
                UiePrintf(UIE_CUST, 0, 0, format20_OP, pData->lAmount, pData->aszStringData, DISPPAGESPACE);        
            } else {
                UiePrintf(UIE_CUST, 0, 0, format20_OP, pData->lAmount, pMnemonic, DISPPAGESPACE);        
            }
        } else if (PifSysConfig()->uchCustType == DISP_10N10D) {
            UiePrintf(UIE_CUST, 0, 0, format10_CU, pData->lAmount); 
		} else if (PifSysConfig()->uchCustType != DISP_NONE) {
			NHPOS_ASSERT_TEXT(PifSysConfig()->uchCustType == DISP_2X20, "**ERROR: PifSysConfig()->uchCustType invalid value.");
		}
    }

    DispRegSave(pData);             /* save the display data if necessary */
}

/*
*===========================================================================
** Synopsis:    VOID DispRegRepeatAmtMnemo( REGDISPMSG *pData, USHORT fusC_Disp )
*                                     
*     Input:    *pData      - Pointer of the Display Data Buffer
*                fusC_Disp    - Customer Display control
*    Output:    nothing     
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This Function makes the display format which enables to 
*               display amount, mnemonics, current page No, and repeat co. 
*
*   (e.g.)     operator display                          customer display
*
*          +----------------------+                       +------------+  
*          | 2              -1.00 |                       | 2    -1.00 |
*          |----------------------|                       +------------+    
*          | COUPON0001         1 |                            
*          +----------------------+                            
*
*===========================================================================
*/
VOID DispRegRepeatAmtMnemo( REGDISPMSG *pData, USHORT fusC_Disp )   
{                                               
	TCHAR   aszBuff[3 + 1] = {0}, *paszBuff;         /* for repeat counter */
    TCHAR   uchDspNull[1] = {_T('\0')};   /* saratoga */

    static  const TCHAR    format20_OP[] = _T("%c\t %l$\n%.20s\t%c");
    static  const TCHAR    format20_CU[] = _T("%c\t %l$\n%-20s");  /* saratoga */
    static  const TCHAR    format10_CU[] = _T("%c\t %l$");    
    static  const TCHAR    format20_CU_5[] = _T("\t%c\n%20s");

    if (pData->fbSaveControl & DISP_SAVE_ECHO_ONLY) {
        return;
    }
    if (pData->uchArrow) {
        fusC_Disp = 0;
    }

	paszBuff =  aszBuff;
    if (!pData->uchArrow) {         /* not display repeat co. at 2x20 scroll */
		SHORT i;
        _ultot((ULONG)uchDispRepeatCo, aszBuff, 10);
		i = tcharlen(aszBuff) - 1;
		if (i > 0) paszBuff = aszBuff + i;
    }

    UiePrintf(UIE_OPER, 0, 0, format20_OP, *paszBuff, pData->lAmount, pData->aszMnemonic, DispSetPageChar(pData->uchArrow));

    if ((C_DISP_MASK & fusC_Disp) == C_DISP) {
		CONST TCHAR  *pMnemonic = pData->aszMnemonic;

		// Check if override mnemonic with hard coded string, MDC_PRINT_ITEM_MNEMO
		if ( CliParaMDCCheckField(MDC_PRINT_ITEM_MNEMO, MDC_PARAM_BIT_B) ) {
			int i = C_DISP_INDEX(fusC_Disp);
			pMnemonic = pMnemonicOverride [i];
		}
        if ((PifSysConfig()->uchCustType == DISP_2X20) || (PifSysConfig()->uchCustType == DISP_4X20)) { /* 2172 */
            /* clear display, sratoga */
            UiePrintf(UIE_CUST, 0, 0, format20_CU_5, DISPPAGESPACE, uchDspNull);    

            UiePrintf(UIE_CUST, 0, 0, format20_CU, *paszBuff, pData->lAmount, pMnemonic);
        } else if (PifSysConfig()->uchCustType == DISP_10N10D) {
            UiePrintf(UIE_CUST, 0, 0, format10_CU, *paszBuff, pData->lAmount); 
		} else if (PifSysConfig()->uchCustType != DISP_NONE) {
			NHPOS_ASSERT_TEXT(PifSysConfig()->uchCustType == DISP_2X20, "**ERROR: PifSysConfig()->uchCustType invalid value.");
		}
	}

    DispRegSave(pData);             /* save the display data if necessary */
}

/*
*===========================================================================
** Synopsis:    VOID DispRegAmtString( REGDISPMSG *pData, USHORT fusC_Disp )
*                                     
*     Input:    *pData      - Pointer of the Display Data Buffer
*                fusC_Disp    - Customer Display control
*    Output:    nothing     
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This Function makes the display format which enables to 
*               display amount, mnemonics(max 19 char.), and current page No. 
*
*   (e.g.)     operator display                          customer display
*
*          +----------------------+                       +------------+  
*          |                 0.00 |                       |       0.00 |
*          |----------------------|                       +------------+    
*          | Mr.Smigh           1 |                            
*          +----------------------+                            
*
*===========================================================================
*/
VOID DispRegAmtString( REGDISPMSG *pData, USHORT fusC_Disp )   
{                                               
    static  const TCHAR    format20_OP[] = _T("\t%l$\n%s\t%u");

    if (pData->fbSaveControl & DISP_SAVE_ECHO_ONLY) {
        return;
    }                                                        

    UiePrintf(UIE_OPER, 0, 0, format20_OP, pData->lAmount, pData->aszStringData, (USHORT)uchDispCurrMenuPage);        

    DispRegSave(pData);             /* save the display data if necessary */

    return;

    fusC_Disp = fusC_Disp;
}

/*
*===========================================================================
** Synopsis:    VOID DispRegSales( REGDISPMSG *pData, USHORT fusC_Disp )
*                                     
*     Input:    *pData      - Pointer of the Display Data Buffer
*                fusC_Disp    - Customer Display control
*    Output:    nothing     
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This Function makes the display format which enables to 
*               display QTY, repeat counter, amount, mnemonics, and current 
*               page No. 
*
*   (e.g.)     operator display                          customer display
*
*          +----------------------+                       +------------+  
*          | 2              20.00 |                       | 2    20.00 |
*          |----------------------|                       +------------+    
*          | PLU A              1 |                            
*          +----------------------+                            
*
*===========================================================================
*/
VOID DispRegSales( REGDISPMSG *pData, USHORT fusC_Disp ) 
{
    TCHAR   uchDspNull[1] = {_T('\0')}; /* saratoga */
	REGDISPQTY  DispQty = {0};
    USHORT  usUpFmt;
    USHORT  usLowFmt;

  /* for operator display */


  /* for customer display */
    static  const TCHAR    format10_CU_1[] = _T("\t%l$");       /* display amount */
    static  const TCHAR    format10_CU_2[] = _T("%c\t %l$");   /* display repeat counter and mnemonics */

  /* for customer display, 2172 */
    static  const TCHAR    format20_CU_1A[] = _T("\t%l$\n%-20s");             /* display amount, mnemonics */    
    static  const TCHAR    format20_CU_1B[] = _T("\t%l$\n%s %-.15s");         /* display amount, mnemonics, adj. mnemonics */    
    static  const TCHAR    format20_CU_2A[] = _T("%c\t %l$\n%-20s");          /* display repeat counter, amount, mnemonics */   
    static  const TCHAR    format20_CU_2B[] = _T("%c\t %l$\n%s %-.15s");      /* display repeat counter, amount, mnemonics, adj. mnemonics */   
    static  const TCHAR    format20_CU_3A[] = _T("%3ld%s\t %l$\n%-20s");      /* display quantity/weight & symbol, amount, mnemonics */                                    
    static  const TCHAR    format20_CU_3B[] = _T("%3ld%s\t %l$\n%s %-.15s");  /* display quantity/weight & symbol, amount, mnemonics, adj. mnemonics */
    static  const TCHAR    format20_CU_4A[] = _T("%*.*l$%s\t%l$\n%-20s");     /* display weight & symbol, amount, mnemonics */
    static  const TCHAR    format20_CU_4B[] = _T("%*.*l$%s\t%l$\n%s %-.15s"); /* display weight & sylbol, amount, mnemonics, adj. mnemonics */
    static  const TCHAR    format20_CU_5[] = _T("\t%c\n%20s");

    if (pData->fbSaveControl & DISP_SAVE_ECHO_ONLY) {
        return;
    }
    if (pData->uchArrow) {
        fusC_Disp = 0;
    }

    /*** operator display format ***/    

    usUpFmt = DispRegUpFmt(pData, &DispQty);   /* get the upper display type */                                     
    usLowFmt = DispRegLowFmt(pData);           /* get the lower display type */                                      
                                    
    if ( usUpFmt == NO_R_Q || usUpFmt == NO_R_Q_STRING ) {
		static  const TCHAR    format20_OP_1A[] = _T("\t%l$\n%.20s\t%c");     /* display amount, mnemonics, and page No. */    
		static  const TCHAR    format20_OP_1B[] = _T("\t%l$\n%s %.14s\t%c");  /* display amount, mnemonics, adj. mnemonics and page No. */
		static  const TCHAR    format20_OP_1C[] = _T("%.20s\t%l$\n%.20s\t%c");     /* display string, amount, mnemonics, and page No. */    
		static  const TCHAR    format20_OP_1D[] = _T("%.20s\t%l$\n%s %.14s\t%c");  /* display string, amount, mnemonics, adj. mnemonics and page No. */

        if ( usLowFmt == NO_ADJ ) {
			if ( usUpFmt == NO_R_Q ) {
				UiePrintf(UIE_OPER, 0, 0, format20_OP_1A, pData->lAmount, pData->aszMnemonic, DispSetPageChar(pData->uchArrow));
			} else {
				UiePrintf(UIE_OPER, 0, 0, format20_OP_1C, pData->aszStringData, pData->lAmount, pData->aszMnemonic, DispSetPageChar(pData->uchArrow));
			}
        } else {
 			if ( usUpFmt == NO_R_Q ) {
				UiePrintf(UIE_OPER, 0, 0, format20_OP_1B, pData->lAmount, pData->aszAdjMnem, pData->aszMnemonic, DispSetPageChar(pData->uchArrow));        
			} else {
				UiePrintf(UIE_OPER, 0, 0, format20_OP_1D, pData->aszStringData, pData->lAmount, pData->aszAdjMnem, pData->aszMnemonic, DispSetPageChar(pData->uchArrow));        
			}
        }
    }

    if ( usUpFmt == REPEAT ) {
		static  const TCHAR    format20_OP_2A[] = _T("%c\t %l$\n%.20s\t%c");  /* display repeat counter, amount, mnemonics, and page No. */   
		static  const TCHAR    format20_OP_2B[] = _T("%c\t %l$\n%s %.14s\t%c"); /* display repeat counter, amount, mnemonics, adj. mnemonics, and page No. */

		TCHAR   aszBuff[3 + 1] = {0};       /* for repeat counter */

        _ultot((ULONG)uchDispRepeatCo, aszBuff, 10);

        if ( usLowFmt == NO_ADJ ) {
            UiePrintf(UIE_OPER, 0, 0, format20_OP_2A, *(aszBuff + _tcslen(aszBuff) - 1), pData->lAmount,         
                pData->aszMnemonic, DispSetPageChar(pData->uchArrow));
        } else {
            UiePrintf(UIE_OPER, 0, 0, format20_OP_2B, *(aszBuff + _tcslen(aszBuff) - 1), pData->lAmount,         
                pData->aszAdjMnem, pData->aszMnemonic, DispSetPageChar(pData->uchArrow));
        }
    }

    if ( usUpFmt == QTY ) {
		static  const TCHAR    format20_OP_3A[] = _T("%3ld %s\t %l$\n%.20s\t%c"); /* display quantity/weight & symbol, amount, mnemonics, and page No. */                                    
		static  const TCHAR    format20_OP_3B[] = _T("%.3ld%s\t %l$\n%s %.20s\t%c"); /* display quantity/weight & symbol, amount, mnemonics, adj. mnemonics, and page No. */
		static  const TCHAR    format20_OP_3APM[] = _T("%3ld%s/\t %l$\n%.20s\t%c"); /* display quantity/weight & symbol, amount, mnemonics, and page No. */                                    
		static  const TCHAR    format20_OP_3BPM[] = _T("%3ld%s/\t %l$\n%s %.14s\t%c"); /* display quantity/weight & symbol, amount, mnemonics, adj. mnemonics, and page No. */

        if ( usLowFmt == NO_ADJ ) {
            if (pData->uchMinorClass == CLASS_REGDISP_PCHECK_PM) {
                UiePrintf(UIE_OPER, 0, 0, format20_OP_3APM, DispQty.ulQTY, pData->aszSPMnemonic, 
                     pData->lAmount, pData->aszMnemonic, DispSetPageChar(pData->uchArrow));
            } else {
                UiePrintf(UIE_OPER, 0, 0, format20_OP_3A, DispQty.ulQTY, pData->aszSPMnemonic, 
                     pData->lAmount, pData->aszMnemonic, DispSetPageChar(pData->uchArrow));
            }
        } else {
            if (pData->uchMinorClass == CLASS_REGDISP_PCHECK_PM) {
                UiePrintf(UIE_OPER, 0, 0, format20_OP_3BPM, DispQty.ulQTY, pData->aszSPMnemonic, 
                     pData->lAmount, pData->aszAdjMnem, pData->aszMnemonic, DispSetPageChar(pData->uchArrow));
            } else {
                UiePrintf(UIE_OPER, 0, 0, format20_OP_3B, DispQty.ulQTY, pData->aszSPMnemonic, 
                     pData->lAmount, pData->aszAdjMnem, pData->aszMnemonic, DispSetPageChar(pData->uchArrow));
            }
        }
    }

    if ( usUpFmt == WEIGHT ) {
		static  const TCHAR    format20_OP_4A[] = _T("%*.*l$%s\t%l$\n%.20s\t%c"); /* display weight & symbol, amount, mnemonics, and page No. */
		static  const TCHAR    format20_OP_4B[] = _T("%*.*l$%s\t%l$\n%s %.14s\t%c"); /* display weight & sylbol, amount, mnemonics, adj. mnemonics, and page No. */
		static  const TCHAR    format20_OP_4APM[] = _T("%*.*l$%s/\t%l$\n%.20s\t%c"); /* display weight & symbol, amount, mnemonics, and page No. */
		static  const TCHAR    format20_OP_4BPM[] = _T("%*.*l$%s/\t%l$\n%s %.14s\t%c"); /* display weight & sylbol, amount, mnemonics, adj. mnemonics, and page No. */

        if ( usLowFmt == NO_ADJ ) {
			// we need to cast DispQty.ulQTY to (DCURRENCY) in the following due to use of l$ format specifier.
			// the quantity is a weight which uses a decimal point.
            if (pData->uchMinorClass == CLASS_REGDISP_PCHECK_PM) {
                UiePrintf(UIE_OPER, 0, 0, format20_OP_4APM, DispQty.uchLength,  
                     DispQty.uchDecimal, (DCURRENCY)DispQty.ulQTY, pData->aszSPMnemonic, pData->lAmount,
                     pData->aszMnemonic, DispSetPageChar(pData->uchArrow));
            } else {
                UiePrintf(UIE_OPER, 0, 0, format20_OP_4A, DispQty.uchLength,  
                     DispQty.uchDecimal, (DCURRENCY)DispQty.ulQTY, pData->aszSPMnemonic, pData->lAmount,
                     pData->aszMnemonic, DispSetPageChar(pData->uchArrow));
            }
        } else {
			// we need to cast DispQty.ulQTY to (DCURRENCY) in the following due to use of l$ format specifier.
			// the quantity is a weight which uses a decimal point.
			if (pData->uchMinorClass == CLASS_REGDISP_PCHECK_PM) {
                UiePrintf(UIE_OPER, 0, 0, format20_OP_4BPM, DispQty.uchLength,  
                    DispQty.uchDecimal, (DCURRENCY)DispQty.ulQTY, pData->aszSPMnemonic, pData->lAmount,
                    pData->aszAdjMnem, pData->aszMnemonic, DispSetPageChar(pData->uchArrow));
            } else {
                UiePrintf(UIE_OPER, 0, 0, format20_OP_4B, DispQty.uchLength,  
                    DispQty.uchDecimal, (DCURRENCY)DispQty.ulQTY, pData->aszSPMnemonic, pData->lAmount,
                    pData->aszAdjMnem, pData->aszMnemonic, DispSetPageChar(pData->uchArrow));
            }
        }
    }

    /*** customer display format ***/    
    if ((C_DISP_MASK & fusC_Disp) == C_DISP) {      /* R3.1 */
		CONST TCHAR  *pMnemonic = pData->aszMnemonic;

		// Check if override mnemonic with hard coded string, MDC_PRINT_ITEM_MNEMO
		if ( CliParaMDCCheckField(MDC_PRINT_ITEM_MNEMO, MDC_PARAM_BIT_B) ) {
			int i = C_DISP_INDEX(fusC_Disp);
			pMnemonic = pMnemonicOverride [i];
		}

        if ((PifSysConfig()->uchCustType == DISP_2X20) || (PifSysConfig()->uchCustType == DISP_4X20)) { /* 2172 */
			TCHAR   aszBuff[3 + 1] = {0};       /* for repeat counter */

			_ultot((ULONG)uchDispRepeatCo, aszBuff, 10);

            /* clear display, sratoga */
			// following clearing of the display removed as it affects the NCR 5975
			// line display by causing items to not be shown.
//            UiePrintf(UIE_CUST, 0, 0, format20_CU_5, DISPPAGESPACE, uchDspNull);    
                             
            if ( usUpFmt == REPEAT ) {
                if ( usLowFmt == NO_ADJ ) {
                    UiePrintf(UIE_CUST, 0, 0, format20_CU_2A, *(aszBuff + _tcslen(aszBuff) - 1), pData->lAmount, pMnemonic);
                } else {
                    UiePrintf(UIE_CUST, 0, 0, format20_CU_2B, *(aszBuff + _tcslen(aszBuff) - 1), pData->lAmount, pData->aszAdjMnem, pMnemonic);
                }
            } else if ( usUpFmt == QTY ) {
                if ( usLowFmt == NO_ADJ ) {
                    UiePrintf(UIE_CUST, 0, 0, format20_CU_3A, DispQty.ulQTY, pData->aszSPMnemonic, pData->lAmount, pMnemonic);
                } else {
                    UiePrintf(UIE_CUST, 0, 0, format20_CU_3B, DispQty.ulQTY, pData->aszSPMnemonic, pData->lAmount, pData->aszAdjMnem, pMnemonic); 
                }
            } else {
                if ( usLowFmt == NO_ADJ ) {
                    UiePrintf(UIE_CUST, 0, 0, format20_CU_1A, pData->lAmount, pMnemonic);
                } else {
                    UiePrintf(UIE_CUST, 0, 0, format20_CU_1B, pData->lAmount, pData->aszAdjMnem, pMnemonic);        
                }
            }
        } else if (PifSysConfig()->uchCustType == DISP_10N10D) {  
            if (uchDispRepeatCo < 2) { /* no repeat counter entry */       
                UiePrintf(UIE_CUST, 0, 0, format10_CU_1, pData->lAmount);
            } else {                    /* repeat counter entry */
				TCHAR   aszBuff[3 + 1] = {0};       /* for repeat counter */

				_ultot((ULONG)uchDispRepeatCo, aszBuff, 10);
                UiePrintf(UIE_CUST, 0, 0, format10_CU_2, * (aszBuff + _tcslen(aszBuff) - 1), pData->lAmount);
            }
		} else if (PifSysConfig()->uchCustType != DISP_NONE) {
			NHPOS_ASSERT_TEXT(PifSysConfig()->uchCustType == DISP_2X20, "**ERROR: PifSysConfig()->uchCustType invalid value.");
		}
	}

    DispRegSave(pData);             /* save the display data if necessary */
}

/*
*===========================================================================
** Synopsis:    USHORT DispRegUpFmt( REGDISPMSG *pData1, REGDISPQTY *pData2 )
*                                     
*     Input:    *pData1      - Pointer of the Display Data Buffer
*     Input:    *pData2      - Pointer of the Quantity Data Buffer
*    Output:    nothing     
*     InOut:    nothing
*
** Return:      USHORT      NO_R_Q : no repeat counter, qty input
**                          REPEAT : repeat counter input
**                          QTY : quantity input
**                          WEIGHT : weight input
*
** Description: This Function decides the upper display format according to 
*               the entry of QTY, repeat counter.  
*
*===========================================================================
*/
USHORT DispRegUpFmt( REGDISPMSG *pData1, REGDISPQTY *pData2 ) 
{
    pData2->ulQTY = (ULONG)labs(pData1->lQTY);    /* set the abusolute value of pData->lQTY */

    if (pData2->ulQTY == 0L) {      /* no QTY entry */
		if (pData1->aszStringData[0]) {
            return( NO_R_Q_STRING );
		}
        if (pData1->uchArrow) {     /* not display repeat co. at 2x20 scroll */
            return( NO_R_Q );
        }
        if (uchDispRepeatCo < 2) {  /* no repeat counter entry */      
            return( NO_R_Q );
        } else {                    /* repeat counter entry */
            return( REPEAT );
        }
    } else {                        /* QTY entry */
        DispRegGetQty(pData2);
        if (pData2->usQtyType == NO_DEC_POINT) {
            return( QTY );
        } else {                    /* weight entry */        
            return( WEIGHT );
        }      
    }
}

/*
*===========================================================================
** Synopsis:    USHORT DispRegLowFmt( REGDISPMSG *pData )
*                                     
*     Input:    *pData      - Pointer of the Display Data Buffer
*    Output:    nothing     
*     InOut:    nothing
*
** Return:      USHORT      NO_ADJ : no adjective entry
**                          ADJ : adjective entry
*
** Description: This Function decides the lower display format according to 
*               the entry of adjective mnemonics.  
*
*===========================================================================
*/
USHORT DispRegLowFmt( REGDISPMSG *pData ) 
{
    if (pData->aszAdjMnem[0] == _T('\0')) {     /* no adjective entry */
        return( NO_ADJ );    
    } else {                                /* adjective entry */
        return( ADJ );    
    }
}

/*
*===========================================================================
** Synopsis:    VOID DispRegGetQty( REGDISPQTY *pData ) 
*                                     
*     Input:    *pData                             
*    Output:    nothing     
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This function sets the data for quantity display. 
*                
*===========================================================================
*/
VOID DispRegGetQty( REGDISPQTY *pData )
{
    if (pData->ulQTY % 1000L == 0L) {  /* no need of decimal point */
        pData->ulQTY /= 1000L;
        pData->usQtyType = NO_DEC_POINT;        

    } else {                    /* need decimal point */

        if (pData->ulQTY % 10L != 0L) {

            pData->uchLength = 7;      
            pData->uchDecimal = 3;
            pData->usQtyType = DEC_POINT;        

        } else if (pData->ulQTY % 100L != 0L) {

            pData->ulQTY /= 10L;
            pData->uchLength = 6;
            pData->uchDecimal = 2;
            pData->usQtyType = DEC_POINT;        

        } else {

            pData->ulQTY /= 100L;
            pData->uchLength = 5;
            pData->uchDecimal = 1;
            pData->usQtyType = DEC_POINT;        

        }
    }
}
/*
*===========================================================================
** Synopsis:    VOID DispRegScale( REGDISPMSG *pData, UCHAR uchType, USHORT fusC_Disp )
*                                     
*     Input:    *pData      - Pointer of the Display Data Buffer
*                fusC_Disp    - Customer Display control
*    Output:    nothing     
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This Function makes the display format which enables to 
*               display weight, amount, mnemonics, and current page No. 
*
*   (e.g.)     operator display                          customer display
*
*          +----------------------+                       +------------+  
*          | 123.456LB     123.46 |                       |     123.46 |
*          |----------------------|                       +------------+    
*          | PLU A              1 |                            
*          +----------------------+                            
*
*===========================================================================
*/
VOID DispRegScale( REGDISPMSG *pData, UCHAR uchType, USHORT fusC_Disp  ) 
{
    static  const TCHAR    format20_OP_1A[] = _T("%6.2l$%s\t%l$\n%.20s\t%c");       /* display weight & symbol, amount, mnemonics, and page No. */
    static  const TCHAR    format20_OP_1B[] = _T("%6.2l$%s\t%l$\n%s %.14s\t%c");    /* display weight & symbol, amount, mnemonics, adj. mnemonics, and page No. */
    static  const TCHAR    format20_OP_2A[] = _T("%7.3l$%s\t%l$\n%.20s\t%c");       /* display weight & symbol, amount, mnemonics, and page No. */
    static  const TCHAR    format20_OP_2B[] = _T("%7.3l$%s\t%l$\n%s %.14s\t%c");    /* display weight & symbol, amount, mnemonics, adj. mnemonics, and page No. */
    static  const TCHAR    format20_CU_1A[] = _T("\t%l$\n%-20s");                   /* display amount, mnemonics, and page No. */    
    static  const TCHAR    format20_CU_1B[] = _T("\t%l$\n%s %-.15s");               /* display amount, mnemonics, adj. mnemonics and page No. */    
    static  const TCHAR    format20_CU_5[] = _T("\t%c\n%20s");

    ULONG   ulScale;                
    TCHAR   uchDspNull[1] = {_T('\0')}; /* saratoga */

	/* for customer display */
    static  const TCHAR    format10_CU[] = _T("\t%l$");    /* display amount */

    if (pData->fbSaveControl & DISP_SAVE_ECHO_ONLY) {
        return;
    }
    if (pData->uchArrow) {
        fusC_Disp = 0;
    }

	/*** operator display format ***/    
	ulScale = (ULONG)labs(pData->lQTY);              /* set the abusolute value of pData->lQTY */

    if (uchType == _2_DIGIT) {
		/* -- display ".XX" type -- */
        if (pData->aszAdjMnem[0] == _T('\0')) {     /* no adjective entry */
            UiePrintf(UIE_OPER, 0, 0, format20_OP_1A, (DWEIGHT)(ulScale / 10L), pData->aszSPMnemonic, pData->lAmount,
                pData->aszMnemonic, DispSetPageChar(pData->uchArrow));
        } else {
            UiePrintf(UIE_OPER, 0, 0, format20_OP_1B, (DWEIGHT)(ulScale / 10L), pData->aszSPMnemonic, pData->lAmount,
                pData->aszAdjMnem, pData->aszMnemonic, DispSetPageChar(pData->uchArrow));
        }
    } else {
		/* -- display ".XXX" type -- */
        if (pData->aszAdjMnem[0] == _T('\0')) {     /* no adjective entry */
            UiePrintf(UIE_OPER, 0, 0, format20_OP_2A, (DWEIGHT)ulScale, pData->aszSPMnemonic, pData->lAmount,
                pData->aszMnemonic, DispSetPageChar(pData->uchArrow));
        } else {
            UiePrintf(UIE_OPER, 0, 0, format20_OP_2B, (DWEIGHT)ulScale, pData->aszSPMnemonic, pData->lAmount,
                pData->aszAdjMnem, pData->aszMnemonic, DispSetPageChar(pData->uchArrow));
        }
    }

    /*** customer display format ***/    
    if ((C_DISP_MASK & fusC_Disp) == C_DISP) {      /* R3.1 */
        if ((PifSysConfig()->uchCustType == DISP_2X20) || (PifSysConfig()->uchCustType == DISP_4X20)) { /* 2172 */
            /* clear display, sratoga */
            UiePrintf(UIE_CUST, 0, 0, format20_CU_5, DISPPAGESPACE, uchDspNull);    

            if (pData->aszAdjMnem[0] == _T('\0')) {     /* no adjective entry */
                UiePrintf(UIE_CUST, 0, 0, format20_CU_1A, pData->lAmount, pData->aszMnemonic);        
            } else {
                UiePrintf(UIE_CUST, 0, 0, format20_CU_1B, pData->lAmount, pData->aszAdjMnem, pData->aszMnemonic);
            }
        } else if (PifSysConfig()->uchCustType == DISP_10N10D) {  
                UiePrintf(UIE_CUST, 0, 0, format10_CU, pData->lAmount);
		} else if (PifSysConfig()->uchCustType != DISP_NONE) {
			NHPOS_ASSERT_TEXT(PifSysConfig()->uchCustType == DISP_2X20, "**ERROR: PifSysConfig()->uchCustType invalid value.");
		}
	}

    DispRegSave(pData);             /* save the display data if necessary */
}


/*
*===========================================================================
** Synopsis:    VOID DispRegOrderEntry( REGDISPMSG *pData )
*                                     
*     Input:    *pData      - Pointer of the Display Data Buffer
*    Output:    nothing     
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This Function makes the display format which enables
*               Order Entry operation.
*
*   (e.g.)     operator display 
*
*          +----------------------+ 
*          | PLU A          20.00 |
*          |----------------------|
*          | CHILD PLU B        11| 
*          +----------------------+ 
*
* Enhanced to 20 char. mnemonics of PLU, 2172
*===========================================================================
*/
VOID DispRegOrderEntry( REGDISPMSG *pData ) 
{
    static  const TCHAR    format20_OP_1A[] = _T("%s %.*s\t%l$\n%.18s\t%c");
    static  const TCHAR    format20_OP_1B[] = _T("%.*s\t%l$\n%.18s\t%c");
    static  const TCHAR    format20_OP_2A[] = _T("%s %.*s\t%l$\n%s %.*s\t%02d");
    static  const TCHAR    format20_OP_2B[] = _T("%s%.*s\t%l$\n%.18s\t%02d");
    static  const TCHAR    format20_OP_3A[] = _T("%.*s\t%l$\n%s %.*s\t%02d");
    static  const TCHAR    format20_OP_3B[] = _T("%.*s\t%l$\n%.18s\t%02d");
    static  const TCHAR    fmtAmount[] = _T("%l$");

	TCHAR   auchAmount[12 + 1] = { 0 };
    USHORT  usAmtLen, usAdjLen, usAdj2Len, usParentLen, usChildLen;

    if (pData->fbSaveControl & DISP_SAVE_ECHO_ONLY) {
        return;
    }

    RflSPrintf(auchAmount, TCHARSIZEOF(auchAmount), fmtAmount, pData->lAmount);
    usAmtLen = tcharlen(auchAmount);
    
    usAdjLen = tcharlen(pData->aszAdjMnem);
    usParentLen = tcharlen(pData->aszStringData);
    usAdj2Len = tcharlen(pData->aszAdjMnem2);
    usChildLen = tcharlen(pData->aszMnemonic);

    /*** operator display format ***/    

    if ( pData->uchMinorClass == CLASS_REGDISP_OEP_LEAD ) {
        if ( pData->aszAdjMnem[0] ) {

            /* display text on both 2x20 and LCD - RFC 01/31/2001 */
            //usParentLen = (((20-(usAdjLen+1+usAmtLen)) < usParentLen) ? (20-(usAdjLen+1+usAmtLen)) : usParentLen);
            if (PifSysConfig()->uchOperType == DISP_2X20) {
                usParentLen = (((20-(usAdjLen+1+usAmtLen)) < usParentLen) ? (20-(usAdjLen+1+usAmtLen)) : usParentLen);
            }
            
            UiePrintf(UIE_OPER, 0, 0, format20_OP_1A,
            pData->aszAdjMnem, usParentLen, pData->aszStringData, pData->lAmount,    /* parent adj/name/price */
            pData->aszMnemonic, DispSetPageChar(pData->uchArrow));      /* leadthrough */
        } else {
            /* display text on both 2x20 and LCD - RFC 01/31/2001 */
            //usParentLen = (((20-(usAmtLen)) < usParentLen) ? (20-(usAmtLen)) : usParentLen);
            if (PifSysConfig()->uchOperType == DISP_2X20) {
                usParentLen = (((20-(usAmtLen)) < usParentLen) ? (20-(usAmtLen)) : usParentLen);
            }
            
            UiePrintf(UIE_OPER, 0, 0, format20_OP_1B,
            usParentLen, pData->aszStringData, pData->lAmount,                       /* parent name/price */
            pData->aszMnemonic, DispSetPageChar(pData->uchArrow));      /* leadthrough */
        }
    } else {
        if ( pData->aszAdjMnem[0] ) {
            if (pData->aszAdjMnem2[0]) {
                /* display text on both 2x20 and LCD - RFC 01/31/2001 */
                //usParentLen = (((20-(usAdjLen+1+usAmtLen)) < usParentLen) ? (20-(usAdjLen+1+usAmtLen)) : usParentLen);
                //usChildLen = (((20-(usAdj2Len+1+2)) < usChildLen) ? (20-(usAdj2Len+1+2)) : usChildLen);
                if (PifSysConfig()->uchOperType == DISP_2X20) {
                    usParentLen = (((20-(usAdjLen+1+usAmtLen)) < usParentLen) ? (20-(usAdjLen+1+usAmtLen)) : usParentLen);
                    usChildLen = (((20-(usAdj2Len+1+2)) < usChildLen) ? (20-(usAdj2Len+1+2)) : usChildLen);
                }
                
                UiePrintf(UIE_OPER, 0, 0, format20_OP_2A,
                pData->aszAdjMnem, usParentLen, pData->aszStringData, pData->lAmount,    /* parent adj/name/price */
                pData->aszAdjMnem2, usChildLen, pData->aszMnemonic, pData->uchFsc);     /* child adj/name/code */
            } else {
                /* display text on both 2x20 and LCD - RFC 01/31/2001 */
                //usParentLen = (((20-(usAdjLen+1+usAmtLen)) < usParentLen) ? (20-(usAdjLen+1+usAmtLen)) : usParentLen);
                if (PifSysConfig()->uchOperType == DISP_2X20) {
                    usParentLen = (((20-(usAdjLen+1+usAmtLen)) < usParentLen) ? (20-(usAdjLen+1+usAmtLen)) : usParentLen);
                }
                
                UiePrintf(UIE_OPER, 0, 0, format20_OP_2B,
                pData->aszAdjMnem, usParentLen, pData->aszStringData, pData->lAmount,    /* parent adj/name/price */
                pData->aszMnemonic, pData->uchFsc);                         /* child name/code */
            }
        } else {
            if (pData->aszAdjMnem2[0]) {
                /* display text on both 2x20 and LCD - RFC 01/31/2001 */
                //usParentLen = (((20-usAmtLen) < usParentLen) ? (20-usAmtLen) : usParentLen);
                //usChildLen = (((20-(usAdj2Len+1+2)) < usChildLen) ? (20-(usAdj2Len+1+2)) : usChildLen);
                if (PifSysConfig()->uchOperType == DISP_2X20) {
                    usParentLen = (((20-usAmtLen) < usParentLen) ? (20-usAmtLen) : usParentLen);
                    usChildLen = (((20-(usAdj2Len+1+2)) < usChildLen) ? (20-(usAdj2Len+1+2)) : usChildLen);
                }
                
                UiePrintf(UIE_OPER, 0, 0, format20_OP_3A,
                usParentLen, pData->aszStringData, pData->lAmount,                       /* parent name/price */
                pData->aszAdjMnem2, usChildLen, pData->aszMnemonic, pData->uchFsc);     /* child adj/name/code */
            } else {
                /* display text on both 2x20 and LCD - RFC 01/31/2001 */
                //usParentLen = (((20-usAmtLen) < usParentLen) ? (20-usAmtLen) : usParentLen);
                if (PifSysConfig()->uchOperType == DISP_2X20) {
                    usParentLen = (((20-usAmtLen) < usParentLen) ? (20-usAmtLen) : usParentLen);
                }
                
                UiePrintf(UIE_OPER, 0, 0, format20_OP_3B,
                usParentLen, pData->aszStringData, pData->lAmount,                       /* parent name/price */
                pData->aszMnemonic, pData->uchFsc);                         /* child name/code */
            }
        }
    }

    DispRegSave(pData);             /* save the display data if necessary */
}


/*
*===========================================================================
** Synopsis:    VOID DispRegOrderEntry( REGDISPMSG *pData )
*                                     
*     Input:    *pData      - Pointer of the Display Data Buffer
*    Output:    nothing     
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This Function makes the display format which enables
*               Order Entry operation.
*
*   (e.g.)     operator display 
*
*          +----------------------+ 
*          | PLU A          20.00 |
*          |----------------------|
*          | CHILD PLU B        11| 
*          +----------------------+ 
*
* Enhanced to 20 char. mnemonics of PLU, 2172
*===========================================================================
*/
VOID DispRegEventTrigger( REGDISPMSG *pData ) 
{
    static  const TCHAR    format20_OP_1A[] = _T("%s");

    USHORT  usAmtLen = 0, usAdjLen, usAdj2Len, usParentLen, usChildLen;

    if (pData->fbSaveControl & DISP_SAVE_ECHO_ONLY) {
        return;
    }

    usAdjLen = tcharlen(pData->aszAdjMnem);
    usParentLen = tcharlen(pData->aszStringData);
    usAdj2Len = tcharlen(pData->aszAdjMnem2);
    usChildLen = tcharlen(pData->aszMnemonic);

    /*** operator display format ***/    
    if ( pData->aszMnemonic[0] ) {
        /* display text on both 2x20 and LCD - RFC 01/31/2001 */
        //usParentLen = (((20-(usAdjLen+1+usAmtLen)) < usParentLen) ? (20-(usAdjLen+1+usAmtLen)) : usParentLen);
        if (PifSysConfig()->uchOperType == DISP_2X20) {
            usParentLen = (((20-(usAdjLen+1+usAmtLen)) < usParentLen) ? (20-(usAdjLen+1+usAmtLen)) : usParentLen);
        }
        
        UiePrintf(UIE_OPER, 0, 0, format20_OP_1A, pData->aszMnemonic);      /* leadthrough */
    }

    DispRegSave(pData);             /* save the display data if necessary */
}


/*
*===========================================================================
** Synopsis:    VOID DispRegForeign( REGDISPMSG *pData, USHORT fusC_Disp )
*                                     
*     Input:    *pData      - Pointer of the Display Data Buffer
*                fusC_Disp    - Customer Display control
*    Output:    nothing     
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This Function makes the display format which enables to 
*               display foreign total, mnemonics, and current page No.
*               
*
*   (e.g.)     operator display                          customer display
*
*          +----------------------+                       +------------+  
*          |                20.00 |                       |      20.00 |
*          |----------------------|                       +------------+    
*          | Foreign Total      1 |                            
*          +----------------------+                            
*
*       There are 3 types of foreign currency total display. 
*
*       2 point decimal type ... displays "XXX.XX"  
*       3 point decimal type ... displays "XXX.XXX"  
*       no decimal Type ... displays "XXX" 
*
*===========================================================================
*/
VOID DispRegForeign( REGDISPMSG *pData, USHORT fusC_Disp )
{
    static  const TCHAR    format20_OP_1[] = _T("\t%11ld\n%s\t %c");
    static  const TCHAR    format20_OP_2[] = _T("\t%12.*l$\n%s\t %c");
    static  const TCHAR    format20_CU_1[] = _T("\t%10ld");
    static  const TCHAR    format20_CU_2[] = _T("\t%11.*l$");
    UCHAR   uchDecimal;
    UCHAR   fbCheckData;

    if (pData->fbSaveControl & DISP_SAVE_ECHO_ONLY) {
        return;
    }
    if (pData->uchArrow) {
        fusC_Disp = 0;
    }

    DispRegSave(pData);         /* save the display data if necessary */

    fbCheckData = (UCHAR)(pData->fbSaveControl & DISP_SAVE_FOREIGN_MASK); 

    if (fbCheckData == DISP_SAVE_DECIMAL_0) {      /* no decimal type ? */

        UiePrintf(UIE_OPER, 0, 0, format20_OP_1, pData->lAmount, pData->aszMnemonic, DispSetPageChar(pData->uchArrow));    /* for operator display */

        if ((C_DISP_MASK & fusC_Disp) == C_DISP) {
            if ((PifSysConfig()->uchCustType == DISP_2X20) || (PifSysConfig()->uchCustType == DISP_4X20)) {     /* 2172 */
                UiePrintf(UIE_CUST, 0, 0, format20_OP_1, pData->lAmount, pData->aszMnemonic, DISPPAGESPACE);    /* for customer display */
            } else if (PifSysConfig()->uchCustType == DISP_10N10D) {
                UiePrintf(UIE_CUST, 0, 0, format20_CU_1, pData->lAmount);     /* for customer display */
            }
        }    
        return;
    }    

    if ((fbCheckData == 0) || (fbCheckData == DISP_SAVE_DECIMAL_2)) {    
        uchDecimal = 2;                        /* 2 point decimal type ? */
    } else {
        uchDecimal = 3;                        /* 3 point decimal type */             
    }    
    UiePrintf(UIE_OPER, 0, 0, format20_OP_2, uchDecimal, pData->lAmount, pData->aszMnemonic, DispSetPageChar(pData->uchArrow));   /* for operator display */

    if ((C_DISP_MASK & fusC_Disp) == C_DISP) {
        if ((PifSysConfig()->uchCustType == DISP_2X20) || (PifSysConfig()->uchCustType == DISP_4X20)) { /* 2172 */
            UiePrintf(UIE_CUST, 0, 0, format20_OP_2, uchDecimal, pData->lAmount, pData->aszMnemonic, DISPPAGESPACE);      /* for customer display */
        } else if (PifSysConfig()->uchCustType == DISP_10N10D) {
            UiePrintf(UIE_CUST, 0, 0, format20_CU_2, uchDecimal, pData->lAmount);            /* for customer display */
		} else if (PifSysConfig()->uchCustType != DISP_NONE) {
			NHPOS_ASSERT_TEXT(PifSysConfig()->uchCustType == DISP_2X20, "**ERROR: PifSysConfig()->uchCustType invalid value.");
		}
	}
} 
#ifdef DISP_REG_NOFUNC
/*
*===========================================================================
** Synopsis:    VOID DispRegOrderNo( REGDISPMSG *pData )
*                                     
*     Input:    *pData      - Pointer of the Display Data Buffer
*    Output:    nothing     
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This Function makes the display format which enables to 
*               display order No., mnemonics, and current page No.
*
*   (e.g.)     operator display                          
*
*          +----------------------+                         
*          | 03-05-08-10-  -  -   |                       
*          |----------------------|                           
*          | ORDER #           1  |                            
*          +----------------------+                            
*
*===========================================================================
*/
VOID DispRegOrderNo( REGDISPMSG *pData )      
{
    static  const TCHAR    format20_OP[] = "%s\n%s\t %u";
    static  const TCHAR    format_No[] = "%02d";
    SHORT     i;
    TCHAR     auchDispBuff[41];      /* string to format upper display */

    if (pData->fbSaveControl & DISP_SAVE_ECHO_ONLY) { 
        return;
    }
    tcharnset(auchDispBuff, _T(' '), 41); 
    //memset(auchDispBuff, 0x20, sizeof(auchDispBuff)); 
                                    /* intialize the buffer */
    for (i = 0; i < NO_OF_ORDER; i++) {
        if (pData->auchOrderNo[i] != 0) {
            RflSPrintf(&auchDispBuff[i * 3], 3, format_No, pData->auchOrderNo[i]);
        } else {
            RflSPrintf(&auchDispBuff[i * 3], 3, format_No, 0);
        }
        auchDispBuff[i * 3 + 2] = _T('-');
    }

    if (PifSysConfig()->uchOperType == DISP_2X20) {
        auchDispBuff[NO_OF_ORDER * 3 - 1] = _T('\0');
    } else {
        auchDispBuff[NO_OF_ORDER * 3 - 1] = _T(' ');
        auchDispBuff[40] = _T('\0');
    }
    UiePrintf(UIE_OPER, 0, 0, format20_OP, auchDispBuff, 
              pData->aszMnemonic, (USHORT)uchDispCurrMenuPage);                
                                    /* for operator display */         

    DispRegSave(pData);         /* save the display data if necessary */
}
#endif
/*
*===========================================================================
** Synopsis:    VOID DispRegNumber( REGDISPMSG *pData, UCHAR uchDscSave )
*                                     
*     Input:    *pData      - Pointer of the Display Data Buffer
*    Output:    nothing     
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This Function makes the display format which enables to 
*               display number, mnemonics, and current page No.
*
*   (e.g.)     operator display                          
*
*          +----------------------+                         
*          |              123456  |                       
*          |----------------------|                           
*          | NUMBER            1  |                            
*          +----------------------+                            
*
*===========================================================================
*/
VOID DispRegNumber( REGDISPMSG *pData )      
{
    static  const TCHAR    format20_OP[] = _T("\t%s\n%s\t %u");
    
    if (pData->fbSaveControl & DISP_SAVE_ECHO_ONLY) {
        return;
    }    
    UiePrintf(UIE_OPER, 0, 0, format20_OP, pData->aszStringData, pData->aszMnemonic, (USHORT)uchDispCurrMenuPage);    /* for operator display */         

    DispRegSave(pData);             /* save the display data if necessary */
}        

/*
*===========================================================================
** Synopsis:    VOID DispRegQuantity( REGDISPMSG *pData )
*                                     
*     Input:    *pData      - Pointer of the Display Data Buffer
*    Output:    nothing     
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This Function makes the display format which enables to 
*               display quantity amount, mnemonics, and current page No. 
*
*   (e.g.)     operator display                          
*
*          +----------------------+                         
*          |                 0.02 |                       
*          |----------------------|                           
*          | Quantity           1 |                            
*          +----------------------+                            
*
*===========================================================================
*/
VOID DispRegQuantity( REGDISPMSG *pData )
{
    static  const TCHAR    format20_OP_1[] = _T("\t%l$\n%s\t %u");
    static  const TCHAR    format20_OP_2[] = _T("\t%*.*l$\n%s\t %u");
    
	REGDISPQTY   DispQty = {0};
    
    if (pData->fbSaveControl & DISP_SAVE_ECHO_ONLY) {
        return;
    }

	DispQty.ulQTY = (ULONG)RflLLabs(pData->lAmount);            /* set the abusolute value of pData->lQTY */
    DispRegGetQty(&DispQty);

    if (DispQty.usQtyType == NO_DEC_POINT) {
        UiePrintf(UIE_OPER, 0, 0, format20_OP_1, (DCURRENCY)DispQty.ulQTY, pData->aszMnemonic, (USHORT)uchDispCurrMenuPage);
    } else {                            
        UiePrintf(UIE_OPER, 0, 0, format20_OP_2, DispQty.uchLength, DispQty.uchDecimal, (DCURRENCY)DispQty.ulQTY, pData->aszMnemonic, (USHORT)uchDispCurrMenuPage);
    }      

    DispRegSave(pData);         /* save the display data if necessary */
}

/* 
*===========================================================================
** Synopsis:    VOID DispRegModeIn( REGDISPMSG *pData, USHORT fusC_Disp )
*                                     
*     Input:    *pData      - Pointer of the Display Data Buffer
*                fusC_Disp    - Customer Display control
*    Output:    nothing     
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This Function makes the display format which enables to 
*               display "CLOSED" and mnemonics.
*
*   (e.g.)     operator display                          customer display
*
*          +----------------------+                       +------------+  
*          |               CLOSED |                       |     CLOSED |
*          |----------------------|                       +------------+    
*          | Sign-In Please       |                            
*          +----------------------+                            
*
* V3.3
*===========================================================================
*/
VOID DispRegModeIn( REGDISPMSG *pData, USHORT fusC_Disp ) 
{
    static  const TCHAR     format20_OP[] = _T("\t%s\n%-20s");
    static  const TCHAR     format20_OPL[] = _T("\t%s\n%-40s");
    static  const TCHAR     format10_CU[] = _T("CCCCCCCCCC");
    TCHAR   uchDspNull[1] = {_T('\0')};
	WCHAR  aszMnemonics[PARA_LEADTHRU_LEN + 1] = { 0 };

    if (pData->fbSaveControl & DISP_SAVE_ECHO_ONLY) {
        return;
    }

	RflGetLead(aszMnemonics, LDT_REGMSG_ADR);

    /* display text on both 2x20 and LCD - RFC 01/31/2001 */
    //UiePrintf(UIE_OPER, 0, 0, format20_OP, &ParaLeadThruWrk.aszMnemonics,
    //                                                pData->aszMnemonic);
    
    if (PifSysConfig()->uchOperType == DISP_LCD) {
        UiePrintf(UIE_OPER, 0, 0, format20_OPL, aszMnemonics, pData->aszMnemonic);
    } else {
        UiePrintf(UIE_OPER, 0, 0, format20_OP, aszMnemonics, pData->aszMnemonic);
    }
    
    /* for operator display */         
    if ((C_DISP_MASK & fusC_Disp) == C_DISP) { 
        if ((PifSysConfig()->uchCustType == DISP_2X20) || (PifSysConfig()->uchCustType == DISP_4X20)) { /* 2172 */
			RflGetLead(aszMnemonics, LDT_CUSCLOSE_ADR);
			UiePrintf(UIE_CUST, 0, 0, format20_OPL,uchDspNull, aszMnemonics);   /* for operator display */
        } else if (PifSysConfig()->uchCustType == DISP_10N10D) {
            UiePrintf(UIE_CUST, 0, 0, format10_CU);                /* for customer display */
		} else if (PifSysConfig()->uchCustType != DISP_NONE) {
			NHPOS_ASSERT_TEXT(PifSysConfig()->uchCustType == DISP_2X20, "**ERROR: PifSysConfig()->uchCustType invalid value.");
		}
    }

    DispRegSave(pData);             /* save the display data if necessary */
}    

/*
*===========================================================================
** Synopsis:    VOID DispRegLock( REGDISPMSG *pData, USHORT fusC_Disp )
*                                     
*     Input:    *pData      - Pointer of the Display Data Buffer
*                fusC_Disp    - Customer Display control
*    Output:    nothing     
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This Function makes the display format which enables to 
*               display "----------".
*
*   (e.g.)     operator display                          customer display
*
*          +----------------------+                       +------------+  
*          | -------------------- |                       | ---------- |
*          |----------------------|                       +------------+    
*          |                      |                            
*          +----------------------+                            
*
*===========================================================================
*/
VOID DispRegLock( REGDISPMSG *pData, USHORT fusC_Disp )  
{
    static  const TCHAR    format20_OP[] = _T("--------------------\n%20s");
    static  const TCHAR    format20_OPL[] = _T("----------------------------------------\n%40s");
    static  const TCHAR    format10_CU[] = _T("----------");
    TCHAR   uchDspNull[1] = {_T('\0')};

    if (pData->fbSaveControl & DISP_SAVE_ECHO_ONLY) {
        return;
    }

    /* display text on both 2x20 and LCD - RFC 01/30/2001 */
    //UiePrintf(UIE_OPER, 0, 0, format20_OP, uchDspNull);
    
    if (PifSysConfig()->uchOperType == DISP_LCD) {
        UiePrintf(UIE_OPER, 0, 0, format20_OPL, uchDspNull);  
    } else {
        UiePrintf(UIE_OPER, 0, 0, format20_OP, uchDspNull);  
    }
    
                                    /* for operator display */         
    if ((C_DISP_MASK & fusC_Disp) == C_DISP) {  
        if ((PifSysConfig()->uchCustType == DISP_2X20) || (PifSysConfig()->uchCustType == DISP_4X20)) { /* 2172 */
            UiePrintf(UIE_CUST, 0, 0, format20_OP, uchDspNull); 
        } else if (PifSysConfig()->uchCustType == DISP_10N10D) {
            UiePrintf(UIE_CUST, 0, 0, format10_CU);        
		} else if (PifSysConfig()->uchCustType != DISP_NONE) {
			NHPOS_ASSERT_TEXT(PifSysConfig()->uchCustType == DISP_2X20, "**ERROR: PifSysConfig()->uchCustType invalid value.");
		}
    }

    DispRegSave(pData);             /* save the display data if necessary */
}

/*
*===========================================================================
** Synopsis:    VOID DispRegSave( REGDISPMSG *pData )
*                                     
*     Input:    *pData      - Pointer of the Display Data Buffer
*    Output:    nothing     
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This Function saves display data for clear key if necessary.
*               See also function DispWriteSpecial20() which does something
*               similar for DISP_SAVE_TYPE_2 and DISP_SAVE_TYPE_3 and then
*               calling function DispWrite().
*===========================================================================
*/
VOID DispRegSave( REGDISPMSG *pData )
{
    UCHAR   fbCheckData = (UCHAR)(pData->fbSaveControl & DISP_SAVE_TYPE_MASK);

    if (fbCheckData == DISP_SAVE_TYPE_0) {           // clear the saved data area
        memset(&DispRegSaveData, 0, sizeof(DispRegSaveData));   /* DISP_SAVE_TYPE_0, intialize the buffer */
    } else if (fbCheckData == DISP_SAVE_TYPE_1) {    // put new data into saved data area
        DispRegSaveData = *pData;                               /* save display data for redisplay */
    } else if (fbCheckData == DISP_SAVE_TYPE_5) {    // put new data into saved data area
        DispRegSaveData = *pData;                               /* save display data for redisplay */
        DispRegPrevData = *pData;                               /* save display data for redisplay */
    }
}    


/*
*===========================================================================
** Synopsis:    VOID DispRegLowMnemo( REGDISPMSG *pData )
*                                     
*     Input:    *pData      - Pointer of the Display Data Buffer
*    Output:    nothing     
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This Function makes the display format which enables to 
*               display mnemonics. 
*
*   (e.g.)     operator display                          
*
*          +----------------------+                         
*          |                      |                       
*          |----------------------|                           
*          | COMMUNICATING        |                            
*          +----------------------+                            
*
*===========================================================================
*/
VOID DispRegLowMnemo( REGDISPMSG *pData )
{
    static  const TCHAR    format20_OP_1[] = _T("\n%-16s");
    
    if (pData->fbSaveControl & DISP_SAVE_ECHO_ONLY) {
        return;
    }

    UiePrintf(UIE_OPER, 0, 0, format20_OP_1, pData->aszMnemonic);

    DispRegSave(pData);         /* save the display data if necessary */
}

/*
*=============================================================================
**  Synopsis: VOID DispSetPageChar(UCHAR uchArrow) 
*               
*       Input:    
*      Output:  
*
**  Return: nothing 
*               
**  Discription: This function sets the changed page at uchDispRegShift.

*===============================================================================
*/
UCHAR DispSetPageChar(UCHAR uchArrow)
{
    if (uchArrow) {
        return (uchArrow);
    } else {
        return ( (UCHAR)(DISPPAGEASCII+uchDispCurrMenuPage) );
    }
}

/*
*=============================================================================
**  Synopsis: VOID DispSetSeatNo(UCHAR uchSeatNo) 
*               
*       Input:    
*      Output:  
*
**  Return: nothing 
*               
**  Discription: This function sets the seat no.

*===============================================================================
*/
UCHAR DispSetSeatNo(UCHAR uchSeatNo)
{
    return (DISPPAGESPACE);

    uchSeatNo = uchSeatNo;

/****
    if (PifSysConfig()->uchOperType == DISP_LCD) {

        return (DISPPAGESPACE);

    }

    if (uchSeatNo) {

        return ( (UCHAR)(DISPPAGEASCII+uchSeatNo) );

    } else {

        return (DISPPAGESPACE);

    }
****/
}

/*
*===========================================================================
** Synopsis:    VOID DispRegMoney( UIFREGDISPLAY *pData )
*
*     Input:    *pData      - Pointer of the Display Data Buffer
*                fchC_Disp    - Customer Display control
*    Output:    nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This Function makes the display format which enables to
*               display QTY, repeat counter, amount, mnemonics, and current
*               page No.
*
*   (e.g.)     operator display
*
*          +----------------------+
*          | 2              20.00 |
*          |----------------------|
*          | PLU A              1 |
*          +----------------------+
*
*===========================================================================
*/
VOID    DispRegMoney(REGDISPMSG *pData)
{
	REGDISPQTY  DispQty = {0};
    USHORT  usUpFmt;
    UCHAR   uchDecimal;
    UCHAR   fchCheckData;

  /* for operator display */
    static  const TCHAR    format20_DEC[] = _T("\t%12.*l$\n%s\t %c");           /* display amount, mnemonics, and page No. */
    static  const TCHAR    format20_DEC_QTY[] = _T("%3ld\t%12.*l$\n%s\t %c");   /* display quantity, amount, mnemonics, and page No. */
    static  const TCHAR    format20_1[] = _T("\t%11ld\n%s\t %c");               /* display amount, mnemonics, and page No. */
    static  const TCHAR    format20_QTY[] = _T("%3ld\t%11ld\n%s\t %c");         /* display quantity, amount, mnemonics, and page No. */

    if (pData->fbSaveControl & DISP_SAVE_ECHO_ONLY) {
        return;
    }

  /*** operator display format ***/
    fchCheckData = (UCHAR)(pData->fbSaveControl & DISP_SAVE_FOREIGN_MASK);

    usUpFmt = DispRegUpFmt(pData, &DispQty);   /* get the upper display type */

    if (fchCheckData == DISP_SAVE_DECIMAL_0) {     /* no decimal type ? */
        if ( usUpFmt == NO_R_Q ) {
            UiePrintf(UIE_OPER, 0, 0, format20_1, pData->lAmount, pData->aszMnemonic, (USHORT)uchDispCurrMenuPage);
        }
        if ( usUpFmt == QTY ) {
            UiePrintf(UIE_OPER, 0, 0, format20_QTY, DispQty.ulQTY, pData->lAmount, pData->aszMnemonic, (USHORT)uchDispCurrMenuPage);
        }
        return;
    }
    if ((fchCheckData == 0) || (fchCheckData == DISP_SAVE_DECIMAL_2)) {
        uchDecimal = 2;                   /* 2 point decimal type */
    } else {
        uchDecimal = 3;                  /* 3 point decimal type */
    }

    if ( usUpFmt == NO_R_Q ) {
        UiePrintf(UIE_OPER, 0, 0, format20_DEC, uchDecimal, pData->lAmount, pData->aszMnemonic, (USHORT)uchDispCurrMenuPage);
    }
    if ( usUpFmt == QTY ) {
        UiePrintf(UIE_OPER, 0, 0, format20_DEC_QTY, DispQty.ulQTY, uchDecimal, pData->lAmount, pData->aszMnemonic, (USHORT)uchDispCurrMenuPage);
    }
	if (usUpFmt == WEIGHT) { //added for ETKTIMEOUT cwunn
		UiePrintf(UIE_OPER, 0, 0, format20_DEC, uchDecimal, (DCURRENCY)0, pData->aszMnemonic, (USHORT)uchDispCurrMenuPage);
	}
    DispRegSave(pData);            /* save the display data if necessary */
}

/*
*===========================================================================
** Synopsis:    VOID DispRegFoodStamp( REGDISPMSG *pData, USHORT fusC_Disp )
*
*     Input:    *pData      - Pointer of the Display Data Buffer
*                fusC_Disp    - Customer Display control
*    Output:    nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This Function makes the display format which enables to
*               display foodstamp change mnemonics,amount, mnemonics,
                amount and current page No.
*
*   (e.g.)     operator display      customer display     customer display
*                                       (10N10D)              (2x20)
*          +----------------------+  +------------+  +----------------------+
*          |  111            1.00 |  | 111   1.00 |  | 111             1.00 |
*          |----------------------|  +------------+  |----------------------|
*          | FS CHANGE   CHANGE 1 |                  |FS CHANGE      CHANGE |
*          +----------------------+                  +----------------------+
*
*===========================================================================
*/
VOID    DispRegFoodStamp(REGDISPMSG *pData, USHORT fusC_Disp)
{
//    static  const UCHAR FARCONST    format20[] = "%8ld\t%l$\n%-8s  %s\t%c";   /* display  mnemonics, amount, and page No.*/
    static  const TCHAR    format20[] = _T("%8ld\t%l$\n  %s\t%c");     /* display  mnemonics, amount, and page No.*/
    static  const TCHAR    format10_CU[] = _T("%3ld\t%l$");            /* display  amount */

    if (pData->fbSaveControl & DISP_SAVE_ECHO_ONLY) {
        return;
    }

    /* TAR 111547 */
//    UiePrintf(UIE_OPER, 0, 0, format20, pData->lQTY/100L, pData->lAmount,
//                        pData->aszMnemonic2, pData->aszMnemonic, uchDispRegShift);
    UiePrintf(UIE_OPER, 0, 0, format20, pData->lQTY/1000L, pData->lAmount, pData->aszMnemonic, (USHORT)uchDispCurrMenuPage);    /* for operator display */

	/* for customer display */
    if ((C_DISP_MASK & fusC_Disp) == C_DISP) {
        if ((PifSysConfig()->uchCustType == DISP_2X20) || (PifSysConfig()->uchCustType == DISP_4X20)) {   /* 2172 */
            if (pData->aszStringData[0]) {    /* if special mnemonic is setted */
                UiePrintf(UIE_CUST, 0, 0, format10_CU, pData->lQTY/1000L, pData->lAmount);    /* for customer display */
            } else {
                UiePrintf(UIE_CUST, 0, 0, format20, pData->lQTY/1000L, pData->lAmount, pData->aszMnemonic, DISPPAGESPACE);      /* for customer display */         
            }
        } else if (PifSysConfig()->uchCustType == DISP_10N10D) {
            UiePrintf(UIE_CUST, 0, 0, format10_CU, pData->lQTY/1000L, pData->lAmount);        /* for customer display */
		} else if (PifSysConfig()->uchCustType != DISP_NONE) {
			NHPOS_ASSERT_TEXT(PifSysConfig()->uchCustType == DISP_2X20, "**ERROR: PifSysConfig()->uchCustType invalid value.");
		}
    }

    DispRegSave(pData);             /* save the display data if necessary */
}

/****** end of source ******/