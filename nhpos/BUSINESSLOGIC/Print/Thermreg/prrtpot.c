/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print Tips Paid Out                 
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRTPOT.c                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               PrtTipsPaidOut() : print tips paid out
*               PrtTipsPaidOut_TH() : print tips paid out ( thermal )
*               PrtTipsPaidOut_EJ() : print tips paid out ( electric journal )
*               PrtTipsPaidOut_SP() : print tips paid out ( slip )
*
*               PrtDflTipsPaidOut() : display tips paid out
*               PrtDflTipsPaidOutForm() : display tips paid out
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-10-93 : 00.00.01 : R.Itoh     : initial                                   
* Jul-13-93 : 01.00.12 : R.Itoh     : add PrtDflTipsPaidOut()                                   
* Oct-29-93 : 02.00.02 : K.You      : del. PrtDflTipsPaidOut()                                   
* Apr-10-94 : 00.00.04 : K.You      : add validation slip print feature.(mod. PrtTipsPaidOut)
* Jan-26-95 : 03.00.00 : M.Ozawa    : add PrtDflTipsPaidOut()                                   
* Dec-15-99 : 00.00.01 : M.Ozawa    : add PrtDflTipsPaidOutForm()                                   
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
#include <prt.h>
#include <pmg.h>
#include <dfl.h>
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
** Format  : VOID  PrtTipsPaidOut(TRANINFORMATION  *pTran, ITEMMISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints tips paid out
*===========================================================================
*/
VOID PrtTipsPaidOut(TRANINFORMATION  *pTran, ITEMMISC  *pItem)
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
            PrtTipsPaidOut_SP(pTran, pItem);
            PrtSPVLTrail(pTran);
        }
        return ;
    }

    if ( fsPrtPrintPort & PRT_SLIP ) {        /* slip print */
        PrtTipsPaidOut_SP(pTran, pItem);
    }

    if ( fsPrtPrintPort & PRT_RECEIPT ) {     /* thermal print */
        PrtTipsPaidOut_TH(pTran, pItem);
    }

    if ( fsPrtPrintPort & PRT_JOURNAL ) {     /* electric journal */
        PrtTipsPaidOut_EJ(pItem);
    }

}

/*
*===========================================================================
** Format  : VOID PrtTipsPaidOut_TH(TRANINFORMATION *pTran, ITEMMISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*   Input  : ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints tips paid out. (thermal)
*===========================================================================
*/
VOID  PrtTipsPaidOut_TH(TRANINFORMATION *pTran, ITEMMISC *pItem)
{
    PrtTHHead(pTran);                            /* print header if necessary */

	PrtTHVoid(pItem->fbModifier, pItem->usReasonCode);                /* void line */

    PrtTHNumber(pItem->aszNumber);               /* number line */

    PrtTHWaiAmtMnem(pItem->ulID, TRN_TIPPO_ADR, pItem->lAmount);
                                                 /* tips paid out line */
}

/*
*===========================================================================
** Format  : VOID PrtTipsPaidOut_EJ(ITEMMISC *pItem);      
*
*   Input  : ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints tips paid out. (electric journal)
*===========================================================================
*/
VOID  PrtTipsPaidOut_EJ(ITEMMISC *pItem)
{
    PrtEJVoid(pItem->fbModifier, pItem->usReasonCode);                /* void line */

    PrtEJNumber(pItem->aszNumber);               /* number line */

    PrtEJWaiter(pItem->ulID);                    /* waiter */

    PrtEJAmtMnem(TRN_TIPPO_ADR, pItem->lAmount); /* tips paid out line */

}

/*
*===========================================================================
** Format  : VOID PrtTipsPaidOut_SP(TRANINFORMATION *pTran, ITEMMISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints tips paid out. (slip)
*===========================================================================
*/
VOID PrtTipsPaidOut_SP(TRANINFORMATION *pTran, ITEMMISC *pItem)
{
    TCHAR  aszSPPrintBuff[2][PRT_SPCOLUMN + 1]; /* print data save area */
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines to be added */
    USHORT  i;   

    /* initialize the buffer */
    memset(aszSPPrintBuff[0], '\0', sizeof(aszSPPrintBuff));
                                                /* initialize the area */
    /* -- set void mnemonic and number -- */
    usSlipLine += PrtSPVoidNumber(aszSPPrintBuff[0], pItem->fbModifier, pItem->usReasonCode, pItem->aszNumber);
    /* -- set paid out mnemonic -- */
    usSlipLine += PrtSPTipsPO(aszSPPrintBuff[usSlipLine], pItem->ulID, TRN_TIPPO_ADR, pItem->lAmount);
                                                        
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
** Format  : VOID  PrtDflTipsPaidOut(TRANINFORMATION  *pTran, ITEMMISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays tips paid out
*===========================================================================
*/
VOID PrtDflTipsPaidOut(TRANINFORMATION  *pTran, ITEMMISC  *pItem)
{

    TCHAR  aszDflBuff[9][PRT_DFL_LINE + 1];   /* display data save area */
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

    usLineNo += PrtDflWaiter(aszDflBuff[usLineNo], pItem->ulID);  

    usLineNo += PrtDflAmtMnem(aszDflBuff[usLineNo], TRN_TIPPO_ADR, pItem->lAmount); 

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
** Format  : VOID  PrtDflTipsPaidOut(TRANINFORMATION  *pTran, ITEMMISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays tips paid out
*===========================================================================
*/
USHORT PrtDflTipsPaidOutForm(TRANINFORMATION  *pTran, ITEMMISC  *pItem, TCHAR *puchBuffer)
{

    TCHAR  aszDflBuff[9][PRT_DFL_LINE + 1];   /* display data save area */
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

    usLineNo += PrtDflWaiter(aszDflBuff[usLineNo], pItem->ulID);  

    usLineNo += PrtDflAmtMnem(aszDflBuff[usLineNo], TRN_TIPPO_ADR, pItem->lAmount); 

    for (i=0; i<usLineNo; i++) {

        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }
    _tcsncpy(puchBuffer, aszDflBuff[0], usLineNo*(PRT_DFL_LINE+1));
    //memcpy(puchBuffer, aszDflBuff, usLineNo*(PRT_DFL_LINE+1));
    
    return usLineNo;

}

/***** End Of Source *****/
