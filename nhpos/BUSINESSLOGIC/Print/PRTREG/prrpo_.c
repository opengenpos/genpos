/*
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
* Title       : Print Paid Out                 
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRPO_.c                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               PrtPaidOut() : print paid out
*               PrtPaidOut_RJ() : print paid out ( receipt & journal )
*               PrtPaidOut_VL() : print paid out ( validation )
*               PrtPaidOut_SP() : print paid out ( slip )
*               PrtDflPaidOut() : display paid out
*
*               PrtDflPaidOutForm() : display paid out
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-01-92 : 00.00.01 : K.Nakajima :                                    
* Jul-13-93 : 01.00.12 : R.Itoh     : add PrtDflPaidOut()                                   
* Oct-28-93 : 02.00.02 : K.You      : del. PrtDflPaidOut()                                   
* Apr-08-94 : 00.00.04 : K.You      : add validation slip print feature.(mod. PrtPaidOut())
* Jan-17-95 : 03.00.00 : M.Ozawa    : add PrtDflPaidOut() for display on the fly
* Jul-21-95 : 03.00.04 : T.Nakahata : display customer name at 1st frame
* Dec-15-99 : 00.00.01 : M.Ozawa    : add PrtDflPaidOutForm()
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
#include <ecr.h>
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

/**
;========================================================================
;+        P R O G R A M    D E C L A R A T I O N s                      +
;========================================================================
**/
/*
*===========================================================================
** Format  : VOID    PrtPaidOut(TRANINFORMATION  *pTran, ITEMMISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints paid out.
*===========================================================================
*/
VOID PrtPaidOut(TRANINFORMATION  *pTran, ITEMMISC  *pItem)
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
            PrtPaidOut_SP(pTran, pItem);
            PrtSPVLTrail(pTran);
        } else {
            PrtPaidOut_VL(pTran, pItem);
        }
        return ;
    }

    if ( fsPrtPrintPort & PRT_SLIP ) {        /* slip print */
        PrtPaidOut_SP(pTran, pItem);
    }

    if (   (fsPrtPrintPort & PRT_RECEIPT)     /* receipt, journal print */
        || (fsPrtPrintPort & PRT_JOURNAL)) {

        PrtPaidOut_RJ(pItem);
    }

}

/*
*===========================================================================
** Format  : VOID    PrtPaidOut_RJ(TRANINFORMATION  *pTran, ITEMMISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints paid out .(receipt & journal)
*===========================================================================
*/
VOID  PrtPaidOut_RJ(ITEMMISC *pItem)
{
    PrtRJVoid(pItem->fbModifier);             /* void line */

    PrtRJNumber(pItem->aszNumber);            /* number line */

    PrtRJAmtMnem(TRN_PO_ADR, pItem->lAmount); /* paid out line */

}

/*
*===========================================================================
** Format  : VOID    PrtPaidOut_VL(TRANINFORMATION  *pTran, ITEMMISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints paid out .(validation)
*===========================================================================
*/
VOID  PrtPaidOut_VL(TRANINFORMATION *pTran, ITEMMISC *pItem)
{

    PmgBeginValidation(PMG_PRT_RCT_JNL);        /* begin validation */


    /* -- in case of 24 char printer -- */

    if ( usPrtVLColumn == PRT_VL_24 ) {

        PrtVLHead(pTran, pItem->fbModifier);    /* send header lines */

        PrtVLNumber(pItem->aszNumber);          /* send number line */

        PrtVLAmtMnem(TRN_PO_ADR, pItem->lAmount);   /* send amount line */

        PrtVLTrail(pTran);                      /* send trailer lines */
    
    }

    /* -- in case of 21 char printer -- */

    if ( usPrtVLColumn == PRT_VL_21 ) {

        PrtVSAmtMnem(pTran, pItem->fbModifier, TRN_PO_ADR, pItem->lAmount);

    }

    PrtEndValidation(PMG_PRT_RCT_JNL);          /* end validation */

}

/*
*===========================================================================
** Format  : VOID    PrtPaidOut_SP(TRANINFORMATION  *pTran, ITEMMISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints paid out. (slip)
*===========================================================================
*/
VOID PrtPaidOut_SP(TRANINFORMATION *pTran, ITEMMISC *pItem)
{
    TCHAR  aszSPPrintBuff[2][PRT_SPCOLUMN + 1]; /* print data save area */
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines to be added */
    USHORT  i;   

    /* initialize the buffer */
    memset(aszSPPrintBuff[0], '\0', sizeof(aszSPPrintBuff));

    /* -- set void mnemonic and number -- */
    usSlipLine += PrtSPVoidNumber(aszSPPrintBuff[0], pItem->fbModifier, pItem->usReasonCode, pItem->aszNumber);
    /* -- set paid out mnemonic -- */
    usSlipLine += PrtSPMnemAmt(aszSPPrintBuff[usSlipLine], TRN_PO_ADR, pItem->lAmount);

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
** Format  : VOID    PrtDflPaidOut(TRANINFORMATION  *pTran, ITEMMISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays paid out.
*===========================================================================
*/
VOID PrtDflPaidOut(TRANINFORMATION  *pTran, ITEMMISC  *pItem)
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

    usLineNo += PrtDflAmtMnem(aszDflBuff[usLineNo], TRN_PO_ADR, pItem->lAmount); 

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
** Format  : VOID    PrtDflPaidOut(TRANINFORMATION  *pTran, ITEMMISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays paid out.
*===========================================================================
*/
USHORT PrtDflPaidOutForm(TRANINFORMATION  *pTran, ITEMMISC  *pItem, TCHAR *puchBuffer)
{
    TCHAR  aszDflBuff[8][PRT_DFL_LINE + 1]; /* display data save area */
    USHORT  usLineNo=0, i;                       /* number of lines to be displayed */

    /* --- if this frame is 1st frame, display customer name --- */

    PrtDflCustHeader( pTran );

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

    usLineNo += PrtDflAmtMnem(aszDflBuff[usLineNo], TRN_PO_ADR, pItem->lAmount); 

    for (i=0; i<usLineNo; i++) {

        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }
    _tcsncpy(puchBuffer, aszDflBuff[0], usLineNo*(PRT_DFL_LINE+1));
    //memcpy(puchBuffer, aszDflBuff, usLineNo*(PRT_DFL_LINE+1));
    
    return usLineNo;

}

/***** End Of Source *****/