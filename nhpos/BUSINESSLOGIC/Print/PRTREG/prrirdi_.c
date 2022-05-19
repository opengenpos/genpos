/*
*---------------------------------------------------------------------------
*  Georgia Southern University, Rsearch Services and Sponsored Programs
*    (C) Copyright 2002 - 2020
*
*  NHPOS, donated by NCR Corp to Georgia Southern University, August, 2002.
*  Developemnt with NCR 7448 then ported to Windows XP and generic x86 hardware
*  along with touch screen support.
*
*---------------------------------------------------------------------------
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print Item  & Regular Discount                 
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRIRDiL.c                                                      
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               PrtItemRegDisc() : print item / regular discount
*               PrtItemRegDisc_RJ() : prints discount item (receipt & journal)
*               PrtItemRegDisc_VL() : prints discount item (validation)
*               PrtItemRegDisc_SP() : prints discount item (slip)
*               PrtDispRegDisc() : display item / regular discount
*
*               PrtDispRegDiscForm() : display item / regular discount
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* May-21-92 : 00.00.01 : K.Nakajima :                                    
* Jul-13-93 : 01.00.12 :  R.Itoh    : add PrtDflRegDisc()                                   
* Oct-28-93 : 02.00.02 :  K.You     : del. PrtDflRegDisc()                                   
* Apr-08-94 : 00.00.04 : K.You      : add validation slip print feature.(mod. PrtItemRegDisc())
* Jan-17-95 : 03.00.00 : M.Ozawa    : add PrtDispRegDisc() for display on the fly
* Jul-21-95 : 03.00.04 : T.Nakahata : display customer name at 1st frame
* Nov-15-95 : 03.01.00 : M.Ozawa    : add CLASS_REGDISC3 - CLASS_REGDISC6
* Nov-15-95 : 03.01.00 :  M.Ozawa   : add CLASS_REGDISC3 - CLASS_REGDISC6
* Dec-14-99 : 00.00.01 :  M.Ozawa   : add PrtDispRegDiscForm()
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

/**
;========================================================================
;+                  I N C L U D E     F I L E s                         +
;========================================================================
**/
/**------- MS - C ---------**/
#include	<tchar.h>
#include <string.h>

/**------- 2170 local------**/
#include<ecr.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <regstrct.h>
#include <transact.h>
#include <dfl.h>
#include <pmg.h>
#include <prt.h>
#include "prtrin.h"
#include "prtdfl.h"
#include "prrcolm_.h"
#include <rfl.h>

/**
;========================================================================
;+        P R O G R A M    D E C L A R A T I O N s                      +
;========================================================================
**/
/*
*===========================================================================
** Format  : VOID PrtItemRegDisc(TRANINFORMATION *pTran, ITEMDISC *pItem);      
*               
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMSALES        *pItem     -Item Data address   
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints item / regular discount.
*===========================================================================
*/
VOID PrtItemRegDisc(TRANINFORMATION  *pTran, ITEMDISC  *pItem)
{

    if (fsPrtStatus & PRT_TAKETOKIT) {  /* take to kitchen status */
        return;     /* not print item discount */
    }

    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);
    
    if ( pItem->fsPrintStatus & PRT_VALIDATION ) { /* validation print */
        if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) {
            if(PRT_SUCCESS != PrtSPVLInit())
			{
				return;
			}
            PrtSPVLHead(pTran);    
            PrtItemRegDisc_SP(pTran, pItem);
            PrtSPVLTrail(pTran);
        } else {
            PrtItemRegDisc_VL(pTran, pItem);
        }
        return ;
    }

    if ( fsPrtPrintPort & PRT_SLIP ) {        /* slip print */
        PrtItemRegDisc_SP(pTran, pItem);
    }

    if (   (fsPrtPrintPort & PRT_RECEIPT)     /* receipt, journal print */
        || (fsPrtPrintPort & PRT_JOURNAL)) {

        PrtItemRegDisc_RJ(pTran, pItem);
    }

}

/*
*===========================================================================
** Format  : VOID PrtItemRegDisc_RJ(TRANINFORMATION *pTran, ITEMDISC *pItem);      
*               
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMDISC         *pItem     -Item Data address   
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints discount item. (receipt & journal)
*===========================================================================
*/
VOID  PrtItemRegDisc_RJ(TRANINFORMATION *pTran, ITEMDISC *pItem)
{
    USHORT  usAdr;
    TCHAR  aszSpecMnem[PARA_SPEMNEMO_LEN + 1];  /* PARA_... defined in "paraequ.h" */

    usAdr = RflChkDiscAdr(pItem);      /* set discount mnemonic */

    PrtRJVoid(pItem->fbDiscModifier);      /* void line */

    PrtRJNumber(pItem->aszNumber);     /* number line */

    PrtRJTaxMod(pTran->TranModeQual.fsModeStatus, pItem->fbDiscModifier);  /* tax modifier */

	/* SI symnbol, before mnemonics 21RFC05437 */
	if ( CliParaMDCCheck(MDC_TAX2_ADR, EVEN_MDC_BIT1) ) {
	    if (PrtGetSISymDisc(aszSpecMnem, pItem)) {						/* si symbol for item disc. */
		    PrtRJPerDiscSISym(usAdr, pItem->uchRate, pItem->lAmount, aszSpecMnem, CliParaMDCCheck(MDC_TAX2_ADR, EVEN_MDC_BIT2) );  /* % discount */
		} else {
		    PrtRJPerDisc(usAdr, pItem->uchRate, pItem->lAmount);  /* % discount */
		}
	} else {
	    PrtRJPerDisc(usAdr, pItem->uchRate, pItem->lAmount);  /* % discount */
	}
}

/*
*===========================================================================
** Format  : VOID PrtItemRegDisc_VL(TRANINFORMATION *pTran, ITEMDISC *pItem);      
*               
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMDISC         *pItem     -Item Data address   
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints discount item. (validation)
*===========================================================================
*/
VOID  PrtItemRegDisc_VL(TRANINFORMATION *pTran, ITEMDISC *pItem)
{
    USHORT  usAdr;

    usAdr = RflChkDiscAdr(pItem);      /* set discount mnemonic */

    PmgBeginValidation(PMG_PRT_RCT_JNL);        /* begin validation */

    /* -- in case of 24 char printer -- */
    if ( usPrtVLColumn == PRT_VL_24 ) {
        PrtVLHead(pTran, pItem->fbDiscModifier);    /* send header lines */
        PrtVLNumber(pItem->aszNumber);          /* send number line */
        PrtVLAmtPerMnem(usAdr, pItem->uchRate, pTran->TranModeQual.fsModeStatus, pItem->fbDiscModifier, pItem->lAmount);
        PrtVLTrail(pTran);                      /* send trailer lines */
    }

    /* -- in case of 21 char printer -- */
    if ( usPrtVLColumn == PRT_VL_21 ) {
        PrtVSAmtPerMnem(pTran, pItem->fbDiscModifier, usAdr, pItem->uchRate, pItem->lAmount);
    }

    PrtEndValidation(PMG_PRT_RCT_JNL);          /* end validation */
}

/*
*===========================================================================
** Format  : VOID PrtItemRegDisc_SP(TRANINFORMATION *pTran, ITEMDISC *pItem);      
*               
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMDISC         *pItem     -Item Data address   
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints discount item. (slip)
*===========================================================================
*/
VOID PrtItemRegDisc_SP(TRANINFORMATION  *pTran, ITEMDISC  *pItem)
{
    TCHAR  aszSPPrintBuff[3][PRT_SPCOLUMN + 1]; /* print data save area */
    USHORT  usSlipLine = 0;       /* number of lines to be printed */
    USHORT  usSaveLine;           /* save slip lines to be added */
    USHORT  usAdr;               /* set the adrress for spec. mnemonic */
    USHORT  i;   

    /* initialize the buffer */
    memset(aszSPPrintBuff[0], '\0', sizeof(aszSPPrintBuff));

    usAdr = RflChkDiscAdr(pItem);      /* set discount mnemonic */

    /* -- set void mnemonic and number -- */
    usSlipLine += PrtSPVoidNumber(aszSPPrintBuff[0], pItem->fbDiscModifier, pItem->usReasonCode, pItem->aszNumber);
    /* -- set tax modifier mnemonic -- */
    usSlipLine += PrtSPTaxMod(aszSPPrintBuff[usSlipLine], pTran->TranModeQual.fsModeStatus, pItem->fbDiscModifier);

    /* -- set discount mnemonic, rate, and amount -- */
    usSlipLine += PrtSPDiscount(aszSPPrintBuff[usSlipLine], usAdr, pItem->uchRate, pItem->lAmount);

    /* -- check if paper change is necessary or not -- */ 
    usSaveLine = PrtCheckLine(usSlipLine, pTran);

    /* -- print all data in the buffer -- */ 
    for (i = 0; i < usSlipLine; i++) {
        PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);       
    }

   /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;         
}


/*
*===========================================================================
** Format  : VOID PrtDflRegDisc(TRANINFORMATION *pTran, ITEMDISC *pItem);      
*               
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMSALES        *pItem     -Item Data address   
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints item / regular discount.
*===========================================================================
*/
VOID PrtDflRegDisc(TRANINFORMATION  *pTran, ITEMDISC  *pItem)
{

    TCHAR  aszDflBuff[9][PRT_DFL_LINE + 1]; /* display data save area */
    USHORT  usLineNo;                       /* number of lines to be displayed */
    USHORT  usOffset = 0;                       
    USHORT  i;                       
    USHORT  usAdr;

    /* --- if this frame is 1st frame, display customer name --- */

    PrtDflCustHeader( pTran );

    usAdr = RflChkDiscAdr(pItem);      /* set discount mnemonic */

    memset(aszDflBuff, '\0', sizeof(aszDflBuff));

    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);

    /* -- set item data -- */
    usLineNo += PrtDflVoid(aszDflBuff[usLineNo], pItem->fbDiscModifier, pItem->usReasonCode);   

    usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber);  

    usLineNo += PrtDflTaxMod(aszDflBuff[usLineNo], pTran->TranModeQual.fsModeStatus, pItem->fbDiscModifier);

    usLineNo += PrtDflPerDisc(aszDflBuff[usLineNo], usAdr, pItem->uchRate, pItem->lAmount);  

    /* -- set destination CRT -- */
    PrtDflIf.Dfl.DflHead.auchCRTNo[0] = 0x30;
    PrtDflIf.Dfl.DflHead.auchCRTNo[1] = 0x30;

    /* -- check void status -- */
    PrtDflCheckVoid(pItem->fbDiscModifier);

    /* -- set display data in the buffer -- */ 
    PrtDflIType(usLineNo, DFL_DISC); 

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
** Format  : VOID PrtDflRegDisc(TRANINFORMATION *pTran, ITEMDISC *pItem);      
*               
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMSALES        *pItem     -Item Data address   
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints item / regular discount.
*===========================================================================
*/
USHORT PrtDflRegDiscForm(TRANINFORMATION  *pTran, ITEMDISC  *pItem, TCHAR *puchBuffer)
{
    TCHAR  aszDflBuff[9][PRT_DFL_LINE + 1]; /* display data save area */
    USHORT  usLineNo=0, i;                       /* number of lines to be displayed */
    USHORT  usOffset = 0;                       
    USHORT  usAdr;

    usAdr = RflChkDiscAdr(pItem);  /* set discount mnemonic */

    memset(aszDflBuff, '\0', sizeof(aszDflBuff));
#if 0
    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);
#endif
    /* -- set item data -- */
    usLineNo += PrtDflVoid(aszDflBuff[usLineNo], pItem->fbDiscModifier, pItem->usReasonCode);   

    usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber);  

    usLineNo += PrtDflTaxMod(aszDflBuff[usLineNo], pTran->TranModeQual.fsModeStatus, pItem->fbDiscModifier);

    usLineNo += PrtDflPerDisc(aszDflBuff[usLineNo], usAdr, pItem->uchRate, pItem->lAmount);  

    for (i=0; i<usLineNo; i++) {
        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }
    _tcsncpy(puchBuffer, aszDflBuff[0], usLineNo*(PRT_DFL_LINE+1));
    
    return usLineNo;

}

/***** End Of Source *****/
