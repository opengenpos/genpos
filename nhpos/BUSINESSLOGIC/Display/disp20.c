/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1998-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Main 2x20 Display Module                         
* Category    : Display, NCR 2170 US Hospitality Application         
* Program Name: DISP20.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*       VOID DispWrite(VOID *pData): display module main
*       VOID DispWriteSpecial(REGDISPMSG *pData1, REGDISPMSG *pData2)   
*                                  : display module main
*                   
*           DispRegWriteSpecial() is called when the display in clear key 
*           entry is different from that before clear key.        
*                    
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date   : Ver.Rev  :  Name    : Description
* Jun-18-92 : 00.00.01 : J.Ikeda  : initial                                   
* Nov-25-92 : 01.00.00 : J.Ikeda  : Adds DispSupTare20()
* Jun-22-93 : 02.00.00 : M.Yamamot: Adds CLASSEMPLOYEENO()
* Jun-23-93 : 01.00.12 : J.IKEDA  : Adds CLASS_PARASHRPRT
* Jul-02-93 : 01.00.12 : K.You    : Adds CLASS_PARAPRESETAMOUNT 
* Jul-15-93 : 01.00.12 : K.You    : Adds CLASS_PARAPIGRULE 
* Aug-23-93 : 01.00.13 : J.IKEDA  : Add CLASS_PARASOFTCHK, Del CLASS_PARACHAR44
* Oct-05-93 : 02.00.01 : J.IKEDA  : Add CLASS_MAINTETKFL
* Nov-08-93 : 02.00.02 : K.You    : Del. CLASS_PARAPIGRULE 
* Mar-01-95 : 03.00.00 : H.Ishida : Add  CLASS_PARACPN
* Nov-09-95 : 03.01.00 : M.Ozawa  : Change DispWrite to DispWrite20 for R3.1
* Dec-18-95 : 03.01.00 : M.Ozawa  : Recover CLASS_PARAPIGRULE 
* Aug-04-98 : 03.03.00 : hrkato   : V3.3
* Aug-11-99 : 03.05.00 : M.Teraki : Remove WAITER_MODEL
* Dec-02-99 : 01.00.00 : hrkato   : Saratoga
* Dec-19-02:           :R.Chambers : SR 15 for Software Security
* Jan-06-15 : 02.02.01 : R.Chambers : fix Preset Tender not showing change or balance due on partial tender
*===========================================================================
*===========================================================================
* PVCS Entry                                               
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===========================================================================
*/
/*
==============================================================================
;                      I N C L U D E     F I L E s                         
;=============================================================================
*/
#include	<tchar.h>
#include <string.h>

#include "ecr.h"
#include "paraequ.h"
#include "para.h"
#include "maint.h"
#include "regstrct.h"
#include "display.h"
#include "mld.h"
#include "disp20.h"

REGDISPMSG DispRegSaveData;       /* saved data structure for re-display */
MAINTWORK  DispSupSaveData;       /* saved data structure for re-display */  
REGDISPMSG DispRegPrevData;		  /* saved data structure for re-display SR155*/

UCHAR   uchDispCurrMenuPage;      /* current menu page 1 to 5 */
UCHAR   uchDispRepeatCo;          /* repeat counter */

// Descriptor control flags are status indicators that are displayed and are
// analogous to labeled LEDs that are either turned on or turned off or
// are blinking to indicate the state of a particular three state application
// characteristic or attribute. The defines for these three states are
// UIE_DESC_OFF, UIE_DESC_BLINK, and UIE_DESC_ON.
// These characteristics or attributes are things like:
//   
ULONG   flDispRegDescrControl;    /* descriptor control flag */
ULONG   flDispRegKeepControl;     /* descriptor keep control flag */
ULONG   ulDispRegOpOldStat;       /* save current descriptor status for next timing */  
UCHAR   uchDispRegCuOldStat;      /* save current descriptor status for next timing */  
ULONG   ulDispRegSaveDsc;         /* save descriptor status for clear key entry */  
ULONG   ulDispRegOpBackStat;      /* save descriptor status in echo back */

/*
*=============================================================================
**  Synopsis: VOID DispWrite(VOID *pData) 
*               
*       Input:  *pData  
*      Output:  nothing
*
**  Return: nothing 
*               
**  Discription: This function is the main function of display module. 
*===============================================================================
*/

VOID DispWrite20(VOID *pData) 
{
    UCHAR       uchMajor = *((UCHAR *)pData);

    /* Distinguish Data Class */
    switch(uchMajor) {
    /* Regsiter Mode */
    case CLASS_UIFREGOPEN:                  /* Operator Open */
        DispRegOpen20(pData);
        break;

    case CLASS_UIFREGTRANSOPEN:             /* transaction open */
        DispRegTransOpen20(pData);
        break;

    case CLASS_UIFREGSALES:                 /* Sales */
        DispRegSales20(pData);
        break;

    case CLASS_REGDISP_SALES:               /* Scalable item */
        DispRegScale20(pData);
        break;

    case CLASS_UIFREGDISC:                  /* Discount */
        DispRegDisc20(pData);
        break;

    case CLASS_UIFREGCOUPON:                /* Coupon */
        DispRegCoupon20(pData);
        break;

    case CLASS_UIFREGTOTAL:                 /* Total Key */
        DispRegTotal20(pData);
        break;

    case CLASS_UIFREGPAMTTENDER:            /* Preset Cash Tender */
    case CLASS_UIFREGTENDER:                /* Tender */
        DispRegTender20(pData);
        break;

    case CLASS_UIFREGMISC:                  /* Misc. Transaction */
        DispRegMisc20(pData);
        break;

    case CLASS_UIFREGMODIFIER:              /* Modifier */
        DispRegModifier20(pData);
        break;

    case CLASS_UIFREGOTHER:                 /* Other */
        DispRegOther20(pData);
        break;

    case CLASS_UIFREGOPECLOSE:              /* Operator Close */
        DispRegClose20(pData);
        break;

    case CLASS_UIFREGMODE:                  /* Mode-In */
        DispRegModeIn20(pData);
        break;

    case CLASS_UIFREGDISP:                  /* Display Module */
        DispRegDisp20(pData);
        break;

    case CLASS_REGDISPCOMPLSRY:             /* Display Module */
        DispRegCompulsory20(pData);
        break;
        
    /* Supervisor and Program Mode */    
    case CLASS_PARADISCTBL:                 /* Discount/Bonus Rate */
    case CLASS_PARASECURITYNO:              /* Security No. Data */
    case CLASS_PARATRANSHALO:               /* Transaction HALO Data */
    case CLASS_PARATEND:                    /* Tender Para Data,    V3.3 */
    case CLASS_PARASLIPFEEDCTL:             /* Slip Feed Control */
    case CLASS_PARACTLTBLMENU:              /* Control Menu Table */
    case CLASS_PARAAUTOALTKITCH:            /* Auto Alternative Kitchen Printer */
    case CLASS_PARAMANUALTKITCH:            /* Manual Alternative Kitchen Printer */
    case CLASS_PARATTLKEYTYP:               /* Total Key Type Assignment */
    case CLASS_PARACASHABASSIGN:            /* Cashier A/B Key Assignment */
    case CLASS_PARATONECTL:                 /* Cashier A/B Key Assignment */
    case CLASS_PARASHRPRT:                  /* Assignment Terminal No. for Shared Printer */
    case CLASS_PARASERVICETIME:             /* Service Time Borser */
    case CLASS_PARALABORCOST:               /* ETK Labor Cost Value */
    case CLASS_PARABOUNDAGE:                /* Boundary Age 2172 */
        DispSupComData20(pData);              
        break;

    case CLASS_PARAPROMOTION:               /* Promotion Sales Header */
    case CLASS_PARATRANSMNEMO:              /* Transaction Mnemonics */
    case CLASS_PARALEADTHRU:                /* Lead Through Mnemonics */
    case CLASS_PARAREPORTNAME:              /* Report Name Mnemonics */
    case CLASS_PARASPECIALMNEMO:            /* Special Mnemonics */
    case CLASS_PARAADJMNEMO:                /* Adjective Mnemonics */
    case CLASS_PARAPRTMODI:                 /* print Modifier Mnemonics */
    case CLASS_PARAMAJORDEPT:               /* Major DEPT Mnemonics */
    case CLASS_PARACHAR24:                  /* 24 Char. Mnemonics */
    case CLASS_PARAPCIF:                    /* PC I/F Mnemonics */
    case CLASS_PARASOFTCHK:                 /* Trailer Print for Soft Check */
    case CLASS_PARAHOTELID:                 /* Hotel Id and Sld */
    case CLASS_PARAMLDMNEMO:                /* LCD menmonics, V3.3 */
        DispSupMnemo20(pData);   
        break;

    case CLASS_PARAMDC:                     /* MDC Data */
    case CLASS_PARAACTCODESEC:
        DispSupBiData20(pData);  
        break;

    case CLASS_PARADEPT:                    /* Department Data */
    case CLASS_PARAPLU:                     /* PLU Data */
    case CLASS_PARACPN:                     /* Combination Coupon Data */
        DispSupDEPTPLU20(pData);         
        break;

    case CLASS_MAINTDSPCTL:                 /* Control Display */
        DispSupControl20(pData);      
        break;

    case CLASS_PARAPLUNOMENU:               /* PLU No.of Appointed Address */   
        DispSupPLUNoMenu20(pData);   
        break;

    case CLASS_PARAROUNDTBL:                /* Rounding Data */
        DispSupRound20(pData);    
        break;

    case CLASS_PARAFSC:                     /* FSC Data */
        DispSupFSC20(pData);  
        break;

    case CLASS_PARAHOURLYTIME:              /* Hourly Activity Data */
        DispSupHourlyTime20(pData);  
        break;
                          
    case CLASS_PARAFLEXMEM:                 /* Flexbility Memory Assignment */
        DispSupFlexMem20(pData);  
        break;

    case CLASS_PARASTOREGNO:                /* Store/Register No. */
        DispSupStoreNo20(pData); 
        break;
                                            /* Supervisor Level Assignment */
    case CLASS_PARASUPLEVEL:
        DispSupLevel20(pData);  
        break;
        
    case CLASS_PARACURRENCYTBL:             /* Foreign Currency Rate */
        DispSupCurrency20(pData);           
        break;

    case CLASS_PARATAXRATETBL:              /* Tax Rate */
        DispSupTaxRate20(pData);          
        break;

    case CLASS_PARAMISCPARA:
        DispSupMiscPara20(pData);  /* Set Misc. Para */
        break;

    case CLASS_PARATTLKEYCTL:               /* Total Key Control */
        DispSupTtlKeyCtl20(pData);
        break;                 

    case CLASS_PARACASHIERNO:               /* Cashier Assignment */
        DispSupCashierNo20(pData);  
        break;
                                            
    case CLASS_PARAEMPLOYEENO:               /* Employee No Assignment */
        DispSupEmployeeNo20(pData);  
        break;   

    case CLASS_MAINTTAXTBL1:                /* Tax Table 1 Data */
    case CLASS_MAINTTAXTBL2:                /* Tax Table 2 Data */
    case CLASS_MAINTTAXTBL3:                /* Tax Table 3 Data */
    case CLASS_MAINTTAXTBL4:                /* Tax Table 4 Data */
        DispSupTaxTbl20(pData);  
        break;

    case CLASS_PARAMENUPLUTBL:              /* Set Menu of PLU */
        DispSupMenuPLU20(pData);
        break;
                                                                           
    case CLASS_MAINTDSPFLINE:               /* Display Lead Through */
        DispSupFLine20(pData);
        break;

    case CLASS_MAINTREDISP:                 /* Control Descriptor and Display Old Data */
        DispSupRedisp20(&DispSupSaveData);
        break;

    case CLASS_PARATARE:
        DispSupTare20(pData);  /* Set Tare Table */
        break;
                                            
    case CLASS_PARAPRESETAMOUNT:            /* Set Preset Amount */
        DispSupPresetAmount20(pData);  
        break;

    case CLASS_MAINTETKFL:                 /* Set ETK File Maintenance */
        DispSupEtkFile20(pData);  
        break;

    case CLASS_PARAOEPTBL:                 /* Set Order Entry Table */
        DispSupOepTbl20(pData);  
        break;

    case CLASS_MAINTCSTR:                  /* Set Cstr File Maintenance */
        DispSupCstrTbl20(pData);  
        break;

    case CLASS_PARAFXDRIVE:                /* Set Flexible Drive Through Parameter */
        DispSupFxDrive20(pData);
        break;

    case CLASS_PARAPPI:                    /* Set PPI Table */
        DispSupPPI20(pData);
        break;

    case CLASS_PARAPIGRULE:                 /* Set Pig Rule */
        DispSupPigRule20(pData);  
        break;

    case CLASS_PARADISPPARA:                /* Set Beverage Dispenser */
        DispSupDispPara20(pData);  
        break;

    case CLASS_PARADEPTNOMENU:               /* Dept No.of Appointed Address 2172 */   
        DispSupDeptNoMenu20(pData);   
        break;

    case CLASS_PARAKDSIP:                    /* KDS IP Address Assignemtn, 2172 */
    case CLASS_MAINTHOSTSIP:                 /* KDS IP Address Assignemtn, 2172 */
        DispSupKdsIp20(pData);
        break;

    case CLASS_PARAUNLOCKNO:                 /* KDS IP Address Assignemtn, 2172 */
        DispSupUnlockNo(pData);
        break;

    case CLASS_PARARESTRICTION:
        DispSupRest20(pData);  /* Set Restriction Table 2172 */
        break;

    case CLASS_MAINTLOAN:                   /* Loan Amount */
    case CLASS_MAINTPICKUP:                 /* Pickup Amount */
        DispSupLoanPickup20(pData);
        break;

	case CLASS_PARAAUTOCPN:
		DispSupAutoCoupon20(pData);
		break;
/*
#ifdef  DISP_REG_DEBUG
    default:
        PifLog(MODULE_DISP_ID, LOG_ERROR_MAJOR_NOT_FOUND);
                                            / "Major Class not found" /
        break;                              / 'CLASS_UIFREGTRANSOPEN' /
#endif                                      /  is possible to be entered. /
*/
    }

    /* display cursor on scroll display */
    MldDispCursor();    /* R3.0 */

}

/*
*=============================================================================
**  Synopsis: VOID DispWriteSpecial(REGDISPMSG *pData1, REGDISPMSG *pData2) 
*               
*       Input:  *pData1, *pData2  
*      Output:  nothing
*
**  Return: nothing 
*               
**  Discription: This function is the main function of display module. 
*                (in special case.)
*===============================================================================
*/
VOID DispWriteSpecial20(REGDISPMSG *pData1, REGDISPMSG *pData2) 
{
    UCHAR   fbCheckData = (UCHAR)(pData1->fbSaveControl & DISP_SAVE_TYPE_MASK); 

    if ((fbCheckData == DISP_SAVE_TYPE_2) || (fbCheckData == DISP_SAVE_TYPE_3)) {
        DispRegSaveData = *pData2;   /* save display data for redisplay */
		DispRegPrevData = *pData1;   //Save Mnemonics for ReDisplay SR 155
        DispWrite(pData1);
    }

    /* display cursor on scroll display */
    MldDispCursor();    /* R3.0 */
}

/****** End of Source ******/