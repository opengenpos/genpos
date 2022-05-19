/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print Received On Account                 
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRROAT.c                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*       PrtRecvOnAcnt() : prints received on acount
*       PrtRecvOnAcnt_TH() : prints received on acount (thermal)
*       PrtRecvOnAcnt_EJ() : prints received on acount (electric journal)
*       PrtRecvOnAcnt_SP() : prints received on acount (slip)
*
*       PrtDflRecvOnAcnt() : displays received on acount
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-10-93 : 01.00.12 : R.Itoh     : initial                                   
* Jul-13-93 : 01.00.12 : R.Itoh     : add PrtDflRecvOnAcnt()                                   
* Oct-29-93 : 02.00.02 : K.You      : del. PrtDflRecvOnAcnt()                                   
* Apr-10-94 : 00.00.04 : K.You      : add validation slip print feature.(mod. PrtRecvOnAcnt())
* Jan-26-95 : 03.00.00 : M.Ozawa    : recover PrtDflRecvOnAcnt() 
* Nov-29-99 : 01.00.00 : hrkato     : Saratoga (Money)
* Dec-15-99 : 00.00.01 : M.Ozawa    : add PrtDflRecvOnAcntForm() 
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
#include "ecr.h"
#include "paraequ.h"
#include <para.h>
#include <csstbpar.h>
#include "regstrct.h"
#include "transact.h"
#include "pmg.h"
#include "dfl.h"
#include "prtrin.h"
#include "prtdfl.h"
#include "prrcolm_.h"

/**
;========================================================================
;+        P R O G R A M    D E C L A R A T I O N s 
;========================================================================
**/
/*
*===========================================================================
** Format  : VOID  PrtRecvOnAcnt(TRANINFORMATION  *pTran, ITEMMISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints received on acount
*===========================================================================
*/
VOID PrtRecvOnAcnt(TRANINFORMATION  *pTran, ITEMMISC  *pItem)
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( pItem->fsPrintStatus & PRT_VALIDATION ) { /* validation print */
        if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) {
            if(PRT_SUCCESS != PrtSPVLInit())
			{
				return;
			}
            PrtSPVLHead(pTran);    
            PrtRecvOnAcnt_SP(pTran, pItem);
            PrtSPVLTrail(pTran);
        }
        return ;
    }

    if ( fsPrtPrintPort & PRT_SLIP ) {        /* slip print */
        PrtRecvOnAcnt_SP(pTran, pItem);
    }

    if ( fsPrtPrintPort & PRT_RECEIPT ) {     /* thermal print */
        PrtRecvOnAcnt_TH(pTran, pItem);
    }

    if ( fsPrtPrintPort & PRT_JOURNAL ) {     /* electric journal */
        PrtRecvOnAcnt_EJ(pItem);
    }

}

/*
*===========================================================================
** Format  : VOID PrtRecvOnAcnt_TH(TRANINFORMATION *pTran, ITEMMISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints received on acount .(thermal)
*===========================================================================
*/
VOID  PrtRecvOnAcnt_TH(TRANINFORMATION *pTran, ITEMMISC *pItem)
{
    PrtTHHead(pTran);                         /* print header if neccesary */  

    PrtTHVoid(pItem->fbModifier, pItem->usReasonCode);             /* void line */

    PrtTHNumber(pItem->aszNumber);            /* number line */

    PrtTHAmtMnem(TRN_RA_ADR, pItem->lAmount); /* ROA line */
    
}

/*
*===========================================================================
** Format  : VOID PrtRecvOnAcnt_EJ(ITEMMISC *pItem);      
*
*   Input  : ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints received on acount .(electric journal)
*===========================================================================
*/
VOID  PrtRecvOnAcnt_EJ(ITEMMISC *pItem)
{
    PrtEJVoid(pItem->fbModifier, pItem->usReasonCode);             /* void line */

    PrtEJNumber(pItem->aszNumber);            /* number line */

    PrtEJAmtMnem(TRN_RA_ADR, pItem->lAmount); /* ROA line */
    
}

/*
*===========================================================================
** Format  : VOID PrtRecvOnAcnt_SP(TRANINFORMATION *pTran, ITEMMISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information
*            ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints received on acount. (slip)
*===========================================================================
*/
VOID PrtRecvOnAcnt_SP(TRANINFORMATION *pTran, ITEMMISC *pItem)
{
    TCHAR  aszSPPrintBuff[2][PRT_SPCOLUMN + 1]; /* print data save area */
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines to be added */
    USHORT  i;   

    /* initialize the buffer */
    memset(aszSPPrintBuff[0], '\0', sizeof(aszSPPrintBuff));

    /* -- set void mnemonic and number -- */
    usSlipLine += PrtSPVoidNumber(aszSPPrintBuff[0], pItem->fbModifier, pItem->usReasonCode, pItem->aszNumber);
    /* -- set received on account mnemonic -- */
    usSlipLine += PrtSPMnemAmt(aszSPPrintBuff[usSlipLine], TRN_RA_ADR, pItem->lAmount);

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
** Format  : VOID  PrtDflRecvOnAcnt(TRANINFORMATION  *pTran, ITEMMISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays received on acount
*===========================================================================
*/
VOID PrtDflRecvOnAcnt(TRANINFORMATION  *pTran, ITEMMISC  *pItem)
{

    TCHAR  aszDflBuff[8][PRT_DFL_LINE + 1]; /* display data save area */
    USHORT  usLineNo;                       /* number of lines to be displayed */
    USHORT  usOffset = 0;                       
    USHORT  i;                       

    /* --- if this frame is 1st frame, display customer name --- */

    PrtDflCustHeader( pTran );

    memset(aszDflBuff, '\0', sizeof(aszDflBuff));

    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);

    /* -- set item data -- */
    usLineNo += PrtDflVoid(aszDflBuff[usLineNo], pItem->fbModifier, pItem->usReasonCode);   

    usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber);  

    usLineNo += PrtDflAmtMnem(aszDflBuff[usLineNo], TRN_RA_ADR, pItem->lAmount); 

    /* -- set destination CRT -- */
    PrtDflIf.Dfl.DflHead.auchCRTNo[0] = 0x30;
    PrtDflIf.Dfl.DflHead.auchCRTNo[1] = 0x30;

    /* -- check void status -- */
    PrtDflCheckVoid(pItem->fbModifier);

    /* -- set display data in the buffer -- */ 
    PrtDflIType(usLineNo, DFL_SINGLE); 

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
** Format  : VOID  PrtDflRecvOnAcnt(TRANINFORMATION  *pTran, ITEMMISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays received on acount
*===========================================================================
*/
USHORT PrtDflRecvOnAcntForm(TRANINFORMATION  *pTran, ITEMMISC  *pItem, TCHAR *puchBuffer)
{

    TCHAR  aszDflBuff[8][PRT_DFL_LINE + 1]; /* display data save area */
    USHORT  usLineNo=0, i;                       /* number of lines to be displayed */


    memset(aszDflBuff, '\0', sizeof(aszDflBuff));
#if 0
    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);
#endif
    /* -- set item data -- */
    usLineNo += PrtDflVoid(aszDflBuff[usLineNo], pItem->fbModifier, pItem->usReasonCode);   

    usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber);  

    usLineNo += PrtDflAmtMnem(aszDflBuff[usLineNo], TRN_RA_ADR, pItem->lAmount); 

    for (i=0; i<usLineNo; i++) {

        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }
    _tcsncpy(puchBuffer, aszDflBuff[0], usLineNo*(PRT_DFL_LINE+1));
    //memcpy(puchBuffer, aszDflBuff, usLineNo*(PRT_DFL_LINE+1));
    
    return usLineNo;
}

/*
*===========================================================================
** Format  : VOID  PrtMoney(TRANINFORMATION  *pTran, ITEMMISC *pItem);
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints money
*===========================================================================
*/
VOID PrtMoney(TRANINFORMATION *pTran, ITEMMISC *pItem)
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if (pItem->fsPrintStatus == 0) {
        return;
    }
    if (fsPrtPrintPort & PRT_SLIP) {
        PrtMoney_SP(pTran, pItem);
    }
    if (usPrtSlipPageLine == 0 && fsPrtPrintPort & PRT_RECEIPT) {
        PrtMoney_TH(pItem);
    }
    if (fsPrtPrintPort & PRT_JOURNAL) {
        PrtMoney_EJ(pItem);
    }
}

/*
*===========================================================================
** Format  : VOID PrtRecvOnAcnt_TH(TRANINFORMATION *pTran, ITEMMISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints received on acount .(thermal)
*===========================================================================
*/
VOID  PrtMoney_TH(ITEMMISC *pItem)
{
    UCHAR  uchSymAdr;
    USHORT usTrnsAdr;

    PrtTHVoid(pItem->fbModifier, pItem->usReasonCode);             /* void line */
    PrtTHNumber(pItem->aszNumber);            /* number line */
    PrtGetMoneyMnem(pItem->uchTendMinor, &usTrnsAdr, &uchSymAdr);

    if((pItem->uchTendMinor >= CLASS_FOREIGN1) && (pItem->uchTendMinor <= CLASS_FOREIGN8)) {
        PrtTHMoneyForeign(pItem->lAmount, TRN_FT_EQUIVALENT, uchSymAdr, pItem->fchStatus);
    } else if ((pItem->uchTendMinor >= CLASS_TENDER1) && (pItem->uchTendMinor <= CLASS_TENDER20)) {
        PrtTHAmtMnem(TRN_FT_EQUIVALENT, pItem->lAmount);
	}
}


/*
*===========================================================================
** Format  : VOID   PrtMoney_EJ(ITEMMISC *pItem);      
*
*   Input  : ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints money. (electric journal)
*===========================================================================
*/
VOID    PrtMoney_EJ(ITEMMISC *pItem)
{
    UCHAR  uchSymAdr;
    USHORT usTrnsAdr;

    PrtEJVoid(pItem->fbModifier, pItem->usReasonCode);             /* void line */
    PrtEJNumber(pItem->aszNumber);            /* number line */
    PrtGetMoneyMnem(pItem->uchTendMinor, &usTrnsAdr, &uchSymAdr);

    if((pItem->uchTendMinor >= CLASS_FOREIGN1) && (pItem->uchTendMinor <= CLASS_FOREIGN8)) {
        PrtEJMoneyForeign(pItem->lAmount, TRN_FT_EQUIVALENT, uchSymAdr, pItem->fchStatus);
    } else if ((pItem->uchTendMinor >= CLASS_TENDER1) && (pItem->uchTendMinor <= CLASS_TENDER20)) {
        PrtEJAmtMnem(TRN_FT_EQUIVALENT,  pItem->lAmount);
	}
}

/*
*===========================================================================
** Format  : VOID PrtMoney_SP(TRNINFORMATION  *pTran, ITEMMISC  *pItem)
*
*   Input  : ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints money. (slip)
*===========================================================================
*/
VOID    PrtMoney_SP(TRANINFORMATION *pTran, ITEMMISC *pItem)
{
    TCHAR   aszSPPrintBuff[3][PRT_SPCOLUMN + 1]; /* print data save area */
    USHORT  usSlipLine = 0,            /* number of lines to be printed */
            usSaveLine,                /* save slip lines to be added */
            i;
    UCHAR  uchSymAdr;                  /* R2.0 */
    USHORT usTrnsAdr;                 /* R2.0 */

    /* -- initialize the buffer */
    memset(aszSPPrintBuff, '\0', sizeof(aszSPPrintBuff));

    /* -- set void mnemonic and number -- */
    usSlipLine += PrtSPVoidNumber(aszSPPrintBuff[0], pItem->fbModifier, pItem->usReasonCode, pItem->aszNumber);
    PrtGetMoneyMnem(pItem->uchTendMinor, &usTrnsAdr, &uchSymAdr);

    /* -- set mnemonic -- */
    if(pItem->uchTendMinor >= CLASS_FOREIGN1 && pItem->uchTendMinor <= CLASS_FOREIGN8) {
        usSlipLine += PrtSPMoneyForeign(aszSPPrintBuff[usSlipLine], pItem->lAmount, usTrnsAdr, uchSymAdr, pItem->fchStatus);
    } else {
        /* -- set qty */
        usSlipLine += PrtSPMnemAmt(aszSPPrintBuff[usSlipLine], usTrnsAdr, pItem->lAmount);
    }
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
** Format  : VOID   PrtGetMoneyMnem(UCHAR uchTendMinor, UCHAR *puchTran, UCHAR *puchSym)
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function determines the Transaction mnemonic and the symbol mnemonic
*            based on the tender type.
*            This function also will handle a Foreign Total key press as well
*            so long as CLASS_FOREIGNTOTAL1 is not in the same range as tender classes.
*===========================================================================
*/
VOID    PrtGetMoneyMnem(UCHAR uchTendMinor, USHORT *pusTran, UCHAR *puchSym)
{
	*puchSym = 0;
	*pusTran = 0;
    if(uchTendMinor >= CLASS_FOREIGN1 && uchTendMinor <= CLASS_FOREIGN2) {
        *pusTran = (uchTendMinor - CLASS_FOREIGN1 + TRN_FT1_ADR);
        *puchSym  = (UCHAR)(uchTendMinor - CLASS_FOREIGN1 + SPC_CNSYMFC1_ADR);
    } else if(uchTendMinor >= CLASS_FOREIGN3 && uchTendMinor <= CLASS_FOREIGN8) {
        *pusTran = (uchTendMinor - CLASS_FOREIGN3 + TRN_FT3_ADR);
        *puchSym  = (UCHAR)(uchTendMinor - CLASS_FOREIGN3 + SPC_CNSYMFC3_ADR);
    }  else if (uchTendMinor >= CLASS_TENDER1 && uchTendMinor <= CLASS_TENDER8) {
        *pusTran = (uchTendMinor - CLASS_TENDER1 + TRN_TEND1_ADR);
    }  else if (uchTendMinor >= CLASS_TENDER9 && uchTendMinor <= CLASS_TENDER11) {
        *pusTran = (uchTendMinor - CLASS_TENDER9 + TRN_TEND9_ADR);
    }  else if (uchTendMinor >= CLASS_TENDER12 && uchTendMinor <= CLASS_TENDER20) {
        *pusTran = (uchTendMinor - CLASS_TENDER12 + TRN_TENDER12_ADR);
    } else if(uchTendMinor >= CLASS_FOREIGNTOTAL1 && uchTendMinor <= CLASS_FOREIGNTOTAL8) {
        *pusTran = (uchTendMinor - CLASS_FOREIGNTOTAL1 + TRN_FT1_ADR);
        *puchSym  = (UCHAR)(uchTendMinor - CLASS_FOREIGNTOTAL1 + SPC_CNSYMFC1_ADR);
	} else {
		NHPOS_ASSERT(uchTendMinor <= CLASS_TENDER20);
		return;
	}
}

/***** End Of Source *****/