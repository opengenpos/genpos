/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1998            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Redisplay Old Data Module( SUPER & PROGRAM MODE )                       
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: DSPRED20.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*               
*               DispSupRedisp20()   : Redisplay Old Data Module
*                                         
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date   : Ver.Rev. :   Name    : Description
* Jul-15-92 : 00.00.01 : K.You     : initial                                   
* Dec-16-92 : 01.00.00 : J.Ikeda   : Adds the function that clears all descriptor
*                                  : of customer display
* Jan-08-92 : 01.00.00 : K.You     : Adds Tax Tare Tone Volume Handling and delete 
*                                  : #ifdef option
* Jul-02-93 : 01.00.12 : K.You     : Adds Preset Amount Feature Handling. 
* Jul-15-93 : 01.00.12 : K.You     : Adds Pig Rule Feature Handling. 
* Jul-27-93 : 01.00.12 : K.You     : Adds EJ Feature Handling. 
* Aug-23-93 : 01.00.13 : J.IKEDA   : Adds CLASS_PARASOFTCHK, Del CLASS_PARACHAR44
* Oct-05-93 : 02.00.01 : J.IKEDA   : Add CLASS_MAINTETKFL
* Nov-08-93 : 02.00.01 : Yoshiko.J : del CLASS_PARAPIGRULE 
* Dec-18-95 : 03.01.00 : M.Ozawa   : recover CLASS_PARAPIGRULE 
* Aug-04-98 : 03.03.00 : hrkato    : V3.3
* Aug-11-99 : 03.05.00 : M.Teraki  : Remove WAITER_MODEL
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

#include <ecr.h>
#include <uie.h>
#include <paraequ.h>
#include <para.h>
#include <pif.h>
#include <plu.h>
#include <pararam.h>
#include <maint.h>
#include <display.h>
#include "csstbstb.h"

#include "disp20.h"

/*
*=============================================================================
**  Synopsis: VOID DispSupRedisp20(MAINTWORK *pData) 
*               
*     Input:    *pData          : Pointer to Union for MAINTWORK
*    Output:    nothing
*   
**  Return: nothing 
*               
**  Discription: Control Descriptor and Redisplay Old Data module. 
*
*                See as well function PrtPrintItem() which has a similar type
*                of switch() based on uchMajor and Major Class of an item.
*===============================================================================
*/

VOID DispSupRedisp20(MAINTWORK *pData) 
{
    UCHAR       uchMajor = *(( UCHAR*)pData);

    /* Control Descriptor */
    DispSupOpDscrCntr();
    

    /* Distinguish Data Class */
    switch(uchMajor) {
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
    case CLASS_PARATONECTL:                 /* Tone Volume Assignment */
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
    case CLASS_PARASOFTCHK:
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
        DispSupControl20(&pData->MaintDspCtl);      
        break;

    case CLASS_PARAPLUNOMENU:               /* PLU No.of Appointed Address */   
        DispSupPLUNoMenu20(&pData->PLUNoMenu);   
        break;

    case CLASS_PARAROUNDTBL:                /* Rounding Data */
        DispSupRound20(&pData->RoundTbl);    
        break;

    case CLASS_PARAFSC:                     /* FSC Data */
        DispSupFSC20(&pData->FSC);  
        break;

    case CLASS_PARAHOURLYTIME:              /* Hourly Activity Data */
        DispSupHourlyTime20(&pData->HourlyTime);  
        break;
                          
    case CLASS_PARAFLEXMEM:                 /* Flexbility Memory Assignment */
        DispSupFlexMem20(&pData->FlexMem);  
        break;

    case CLASS_PARASTOREGNO:                /* Store/Register No. */
        DispSupStoreNo20(&pData->StoRegNo); 
        break;
                                            /* Supervisor Level Assignment */
    case CLASS_PARASUPLEVEL:
        DispSupLevel20(&pData->SupLevel);  
        break;
        
    case CLASS_PARACURRENCYTBL:             /* Foreign Currency Rate */
        DispSupCurrency20(&pData->CurrencyTbl);           
        break;

    case CLASS_PARATAXRATETBL:              /* Tax Rate */
        DispSupTaxRate20(&pData->TaxRateTbl);          
        break;

    case CLASS_PARATTLKEYCTL:               /* Total Key Control */
        DispSupTtlKeyCtl20(&pData->TtlKeyCtl);
        break;                 

    case CLASS_PARACASHIERNO:               /* Cashier Assignment */
        DispSupCashierNo20(&pData->MaintCashierIf);  
        break;
                                            
    case CLASS_PARAEMPLOYEENO:               /* Employee No Assignment */
        DispSupEmployeeNo20(&pData->ParaEmployeeNo);  
        break;   

    case CLASS_MAINTTAXTBL1:                /* Tax Table 1 Data */
    case CLASS_MAINTTAXTBL2:                /* Tax Table 2 Data */
    case CLASS_MAINTTAXTBL3:                /* Tax Table 3 Data */
    case CLASS_MAINTTAXTBL4:                /* Tax Table 4 Data */
        DispSupTaxTbl20(&pData->MaintTaxTbl);  
        break;

    case CLASS_PARAMENUPLUTBL:              /* Set Menu of PLU */
        DispSupMenuPLU20(&pData->MenuPLUTbl);
        break;
                                                                           
    case CLASS_MAINTDSPFLINE:               /* Display Lead Through */
        DispSupFLine20(&pData->MaintDspFLine);
        break;

    case CLASS_PARATARE:                    /* Display Tare Table */
        DispSupTare20( &pData->Tare );
        break;

    case CLASS_PARAPRESETAMOUNT:            /* Display Preset Amount Cash Tender */
        DispSupPresetAmount20( &pData->PresetAmount );
        break;

    case CLASS_MAINTETKFL:                 /* Set ETK File Maintenance */
        DispSupEtkFile20 (&pData->EtkFl );  
        break;

    case CLASS_PARAOEPTBL:                 /* Set Order Entry Prompt Table */
        DispSupOepTbl20(&pData->ParaOepTbl);  
        break;

    case CLASS_MAINTCSTR:                  /* Set Cstr File Maintenance */
        DispSupCstrTbl20(&pData->MaintCstr);  
        break;

    case CLASS_PARAFXDRIVE:                /* Set Flexible Drive Through Parameter */
        DispSupFxDrive20(&pData->ParaFxDrive);
        break;

    case CLASS_PARAPPI:                    /* Set PPI Table */
        DispSupPPI20(&pData->MaintPpi);
        break;

    case CLASS_PARAPIGRULE:                 /* Display Pig Rule */
        DispSupPigRule20( &pData->PigRuleTbl );
        break;

    case CLASS_PARADISPPARA:                /* Set Beverage Dispenser */
        DispSupDispPara20(&pData->DispPara);  
        break;

    case CLASS_PARADEPTNOMENU:               /* Dept No.of Appointed Address 2172 */   
        DispSupDeptNoMenu20(&pData->DeptNoMenu);   
        break;

    case CLASS_PARAKDSIP:                    /* KDS IP Address Assignemtn, 2172 */
        DispSupKdsIp20(&pData->KdsIp);
        break;

    case CLASS_PARARESTRICTION:
        DispSupRest20(&pData->Restriction);  /* Set Restriction Table 2172 */
        break;

    default:
        break;                              
    }
}

/*
*===========================================================================
** Synopsis:    VOID DispSupOpDscrCntr( VOID )     
*
*     Input:    Nothing
*     Output:   Nothing
*
** Return:      Nothing
*
** Description: This function compares current descriptor status with old    
*               status and turns the descriptors on/off according to the   
*               status change.
*
*               NOTEL  See also function DispRegOpDscrCntr()
*===========================================================================
*/
VOID DispSupOpDscrCntr(VOID)   
{
    USHORT  i;

    /* Light Off All Descriptor */
    if (!flDispRegDescrControl) {
        /* clear all descriptor of operator display */
        for (i = _2X20_NEW_STORE; i < _2x20_KDS; i++) {
            UieDescriptor(UIE_OPER, i, UIE_DESC_OFF);
        }

        /* clear all descriptor of customer display */
        if ((PifSysConfig()->uchCustType == DISP_2X20) ||
            (PifSysConfig()->uchCustType == DISP_4X20)) { /* 2172 */

            for (i = _2X20_COMM; i < _2x20_KDS; i++) {
                UieDescriptor(UIE_CUST, i, UIE_DESC_OFF);
            }
        } else {
            for (i = _2X20_COMM; i < _2X20_MINUS; i++) {
                UieDescriptor(UIE_CUST, i, UIE_DESC_OFF);
            }
        }
        return;
    }

    /* Control VOID Descriptor */
    if (flDispRegDescrControl & VOID_CNTRL) {
        UieDescriptor(UIE_OPER, _2X20_VOID_DELETE, UIE_DESC_ON);
    } else {
        UieDescriptor(UIE_OPER, _2X20_VOID_DELETE, UIE_DESC_OFF);
    }

    /* Control TRAINING Descriptor */
    if (flDispRegDescrControl & TRAIN_CNTRL) {
        UieDescriptor(UIE_OPER, _2X20_TRAIN, UIE_DESC_ON);
    } else {
        UieDescriptor(UIE_OPER, _2X20_TRAIN, UIE_DESC_OFF);
    }

    /* Control ALPHA Descriptor */
    if (flDispRegDescrControl & CRED_CNTRL) {
        UieDescriptor(UIE_OPER, _2X20_CRED_ALPHA, UIE_DESC_ON);
    } else {
        UieDescriptor(UIE_OPER, _2X20_CRED_ALPHA, UIE_DESC_OFF);
    }

    /* Control NO RECEIPT Descriptor */
    if (flDispRegDescrControl & RECEIPT_CNTRL) {
        UieDescriptor(UIE_OPER, _2X20_RECEIPT, UIE_DESC_ON);
    } else {
        UieDescriptor(UIE_OPER, _2X20_RECEIPT, UIE_DESC_OFF);
    }

	// Store and Forward indicator
	//Gets flag from Para.uchStoreForwardFlag
	// Store and Forward indicator
	//Gets flag from Para.uchStoreForwardFlag
	{
		UCHAR  myStoreAndForwardStatus = ParaStoreForwardFlagAndStatus();

		if ((myStoreAndForwardStatus & PIF_STORE_AND_FORWARD_ENABLED) == PIF_STORE_AND_FORWARD_DISABLED) {
			UieDescriptor(UIE_OPER, _2X20_SF, UIE_DESC_OFF);
		} else if ((myStoreAndForwardStatus & (PIF_STORE_AND_FORWARD_ONE_ON | PIF_STORE_AND_FORWARD_TWO_ON)) == 0) {
			UieDescriptor(UIE_OPER, _2X20_SF, UIE_DESC_BLINK);
		} else if ((myStoreAndForwardStatus & (PIF_STORE_AND_FORWARD_ONE_ON | PIF_STORE_AND_FORWARD_TWO_ON)) != 0) {
			UieDescriptor(UIE_OPER, _2X20_SF, UIE_DESC_ON);
		}

		if ((myStoreAndForwardStatus & (PIF_STORE_AND_FORWARD_ONE_FORWARDING | PIF_STORE_AND_FORWARD_TWO_FORWARDING)) == 0) {
			UieDescriptor(UIE_OPER, _2X20_SF_TRANSFERRING, UIE_DESC_OFF);
		} else {
			UieDescriptor(UIE_OPER, _2X20_SF_TRANSFERRING, UIE_DESC_ON);
		}
	}

	if (CstIamDisconnectedUnjoinedSatellite() == STUB_SUCCESS) {
		UieDescriptor(UIE_OPER, _2X20_DISCON_SAT, UIE_DESC_ON);
	}
	else {
		UieDescriptor(UIE_OPER, _2X20_DISCON_SAT, UIE_DESC_OFF);
	}

}

/****** End of Source ******/
